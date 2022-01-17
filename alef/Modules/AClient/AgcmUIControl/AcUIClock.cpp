#include "AcUIClock.h"
#include "AuMath.h"

extern AgcEngine *	g_pEngine;

#define	ACUICLOCK_AFTERIMAGE_MOVE_SPEED						( 0.00008f )
#define	ACUICLOCK_AFTERIMAGE_MOVE_ACCELERATION				( 0.0005f )	

AcUIClock::AcUIClock( void )
{
	m_lTotalPoint			= 1;
	m_lCurrentPoint			= 0;
	m_fLengthRate			= 0.0f;
	m_lAlphaImageID			= 0;
	m_nType					= TYPE_CLOCK;
	m_eClockType			= 0;
	m_eAnim					= ACUICLOCK_ANIM_NONE;
	m_fAfterImageRate		= 0.0f;
	m_fAfterImageMoveSpeed	= 0.0f;
	m_ulLastTick			= 0;

	for( INT32 lIndex = 0 ; lIndex < 7 ; ++lIndex )
	{
		m_vAlphaFan[ lIndex ].color	= 0xffffffff;
		m_vAlphaFan[ lIndex ].u		= 0.5f;
		m_vAlphaFan[ lIndex ].v		= 0.5f;
	}
}

AcUIClock::~AcUIClock( void )
{
}

void AcUIClock::SetAlphaImageID( INT32 lImageID )
{
	m_lAlphaImageID = lImageID;
}

/*****************************************************************
*   Function : SetPointInfo
*   Comment  : Point Info 를 Setting 한다 - 여기서 이 Control의 Width가 결정된다
*   Date&Time : 2003-07-18, 오후 3:10
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIClock::SetPointInfo( INT32 lTotalPoint, INT32 lCurrentPoint )
{
	if( lTotalPoint <= 0 )
	{
		m_fLengthRate = 0.0f;
		return;
	}

	m_lTotalPoint = lTotalPoint;	
	m_lCurrentPoint	= lCurrentPoint;
}

RwTexture* AcUIClock::_GetAlphaTexture( void )
{
	return m_csTextureList.GetImage_ID( m_lAlphaImageID );
}

BOOL AcUIClock::_IsVaildTexture( RwTexture* pTexture )
{
	if( !pTexture ) return FALSE;
	if( !RwTextureGetRaster( pTexture ) ) return FALSE;
	return TRUE;
}
/*****************************************************************
*   Function : OnWindowRender
*   Comment  : OnWindowRender
*   Date&Time : 2003-09-29, 오후 4:02
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIClock::OnWindowRender( void )
{
	PROFILE("AcUIClock::OnWindowRender");

	if( !m_lTotalPoint || !h ) return;
	AcUIBase::OnWindowRender();

	RwTexture* pTexture = _GetAlphaTexture();
	if( !_IsVaildTexture( pTexture ) ) return;

	FLOAT fRate = 1.0f - ( ( FLOAT )m_lCurrentPoint / ( FLOAT )m_lTotalPoint );

	if( fRate > 1.0f )
	{
		fRate = 1.0f;
	}
	else if( fRate < 0.0f )
	{
		fRate = 0.0f;
	}

	FLOAT fDegree =	fRate * 2 * AUMATH_PIF;
	INT32 lNumTriangle = 1;
	FLOAT fHeight =	h / 2.0f;
	FLOAT fWidth = w / 2.0f;
	FLOAT fTemp	= atan( w / ( h + 0.0f ) );

	INT32 lAbsX = 0;
	INT32 lAbsY = 0;
	ClientToScreen( &lAbsX, &lAbsY );

	m_vAlphaFan[ 0 ].x = lAbsX + w / 2.0f;
    m_vAlphaFan[ 0 ].y = lAbsY + h / 2.0f;

	m_vAlphaFan[ 1 ].x = m_vAlphaFan[ 0 ].x;
    m_vAlphaFan[ 1 ].y = ( float )lAbsY;

	m_vAlphaFan[ 2 ].y = ( float )lAbsY;

	if( fDegree < fTemp )
	{
		m_vAlphaFan[ 2 ].x = ( float )lAbsX + fWidth - fHeight * tan( fDegree );
	}
	else
	{
		m_vAlphaFan[ 2 ].x = ( float )lAbsX;
		++lNumTriangle;

		m_vAlphaFan[ 3 ].x = ( float )lAbsX;

		if( fDegree < AUMATH_PI - fTemp )
		{
			if( fDegree == AUMATH_PI / 2.0f )
			{
				m_vAlphaFan[ 3 ].y = lAbsY + fHeight;
			}
			else
			{
				m_vAlphaFan[ 3 ].y = lAbsY + fHeight - fWidth / tan( fDegree );
			}
		}
		else
		{
			m_vAlphaFan[ 3 ].y = ( float )lAbsY + h;
			++lNumTriangle;

			m_vAlphaFan[ 4 ].y = ( float )lAbsY + h;

			if( fDegree < AUMATH_PI + fTemp )
			{
				m_vAlphaFan[ 4 ].x = ( float )lAbsX + fWidth + fHeight * tan( fDegree );
			}
			else
			{
				m_vAlphaFan[ 4 ].x = ( float )lAbsX + w;
				++lNumTriangle;

				m_vAlphaFan[ 5 ].x = ( float )lAbsX + w;

				if( fDegree < 2.0f * AUMATH_PIF - fTemp )
				{
					if( fDegree == AUMATH_PI / 2.0f )
					{
						m_vAlphaFan[ 5 ].y = ( float )lAbsY + fHeight;
					}
					else
					{
						m_vAlphaFan[ 5 ].y = ( float )lAbsY + fHeight + fWidth / tan( fDegree );
					}
				}
				else
				{
					m_vAlphaFan[ 5 ].y = ( float )lAbsY;
					++lNumTriangle;

					m_vAlphaFan[ 6 ].y = m_vAlphaFan[ 1 ].y;
					m_vAlphaFan[ 6 ].x = ( float )lAbsX + fWidth - fHeight * tan( fDegree );
				}
			}
		}
	}

	RwD3D9SetTexture( pTexture, 0 );
	RwD3D9SetFVF( MY2D_VERTEX_FLAG );
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , lNumTriangle , m_vAlphaFan , SIZE_MY2D_VERTEX );
}