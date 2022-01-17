#include "AcUIBar.h"


extern AgcEngine* g_pEngine;


#define	ACUIBAR_AFTERIMAGE_MOVE_SPEED					(0.00008f)
#define	ACUIBAR_AFTERIMAGE_MOVE_ACCELERATION			(0.0005f)	



AcUIBar::AcUIBar( void )
{
	m_lTotalPoint			= 1;
	m_lCurrentPoint			= 0;
	m_fCurrRate				= 0.0f;
	m_lEdgeImageID			= 0;
	m_lBodyImageID			= 0;
	
	m_nType					= TYPE_BAR;

	m_bVertical				= FALSE;
	m_eBarType				= 0;

	m_eAnim					= ACUIBAR_ANIM_NONE;
	m_fAfterImageRate		= 0.0f;
	m_fAfterImageMoveSpeed	= 0.0f;

	m_ulLastTick			= 0;
}

AcUIBar::~AcUIBar()
{	
}

/*****************************************************************
*   Function : SetPointInfo
*   Comment  : Point Info 를 Setting 한다 - 여기서 이 Control의 Width가 결정된다
*   Date&Time : 2003-07-18, 오후 3:10
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIBar::SetPointInfo( INT32 lTotalPoint, INT32 lCurrentPoint )
{
	// 최대치가 0 이하면 비정상 수치
	if( lTotalPoint <= 0 )
	{
		m_lTotalPoint = 1;
		m_lCurrentPoint = 0;
		m_fCurrRate = 0.0f;
		return;
	}

	_CalculateNextRate( m_eBarType, m_eAnim, lTotalPoint, lCurrentPoint );

	m_lTotalPoint = lTotalPoint;	
	m_lCurrentPoint = lCurrentPoint;
}

void AcUIBar::_CalculateNextRate( INT32 eBarType, AcUIBarAnimType eAnimType, INT32 lTotalPoint, INT32 lCurrentPoint )
{
	// 새로운 진행률 계산
	FLOAT fNextRate = ( FLOAT )lCurrentPoint / ( FLOAT )lTotalPoint ;
	if( fNextRate > 1.0f )
	{
		// 진행률이 계산상의 오류든 실제 수치가 잘못 들어왔든.. 최대치를 넘지 않게 보정
		fNextRate = 1.0f;
	}

	// 새로 계산된 진행률과 기존의 진행률이 다르다면 증가든 감소든 애니메이션은 진행될 것이며 속도는 일정하다.
	if( fNextRate != m_fCurrRate )
	{
		// 애니메이션 연출을 부드럽게 하도록 되어 있다면..
		if( eBarType & ACUIBAR_TYPE_SMOOTH )
		{
			// 새로운 목표치가 현재치보다 크다면 증가 애니메이션
			if( fNextRate > m_fCurrRate )
			{
				m_fAfterImageRate = fNextRate;
				m_eAnim = ACUIBAR_ANIM_INCREASE;
			}
			// 아니라면 감소 애니메이션
			else
			{
				m_fAfterImageRate = m_fCurrRate;
				m_eAnim = ACUIBAR_ANIM_DECREASE;
			}
		}

		// 계산된 수치를 목표 수치로 설정
		m_fCurrRate = fNextRate;

		// 증가 및 감소 애니메이션 속도는 일정속도로 고정한다.
		m_fAfterImageMoveSpeed = ACUIBAR_AFTERIMAGE_MOVE_SPEED;
	}
	// 계산된 진행률과 기존 진행률이 같다면..
	else
	{
		// 정지상태로 설정
		m_eAnim = ACUIBAR_ANIM_NONE;
		m_fAfterImageMoveSpeed = 0.0f;
	}
}

RwTexture* AcUIBar::_GetBodyTexture( void )
{
	return m_csTextureList.GetImage_ID( m_lBodyImageID );
}

RwTexture* AcUIBar::_GetEdgeTexture( void )
{
	return m_csTextureList.GetImage_ID( m_lEdgeImageID );
}

BOOL AcUIBar::_IsValidTexture( RwTexture* pTexture )
{
	// 포인터가 정상인가
	if( !pTexture ) return FALSE;
	
	// 래스터가 정상인가
	RwRaster* pRaster = RwTextureGetRaster( pTexture );
	if( !pRaster ) return FALSE;

	// 이미지 사이즈가 정상인가
	INT32 lImageWidth = RwRasterGetWidth( pRaster );
	INT32 lImageHeight = RwRasterGetHeight( pRaster );

	if( !lImageWidth || !lImageHeight )	return FALSE;

	// 정상인갑다;;
	return TRUE;
}

BOOL AcUIBar::_DrawTexture( RwTexture* pTexture, INT32 lPosX, INT32 lPosY, INT32 lWidth, INT32 lHeight, INT32 lUStart, INT32 lVStart, INT32 lUEnd, INT32 lVEnd, FLOAT fAlphaValue )
{
	if( !pTexture ) return FALSE;

	DWORD uColor = 0xffffffff;
	FLOAT fAlphaRate = m_pfAlpha ? *m_pfAlpha : 1.0f;

	if( g_pEngine )
	{
		return g_pEngine->DrawIm2DPixel( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd, uColor, ( UINT8 )( fAlphaValue * fAlphaRate ) );
	}

	return FALSE;
}

BOOL AcUIBar::_DrawTexture( RwTexture* pTexture, FLOAT fPosX, FLOAT fPosY, FLOAT fWidth, FLOAT fHeight, FLOAT fUStart, FLOAT fVStart, FLOAT fUEnd, FLOAT fVEnd, FLOAT fAlphaValue )
{
	if( !pTexture ) return FALSE;

	DWORD uColor = 0xffffffff;
	FLOAT fAlphaRate = m_pfAlpha ? *m_pfAlpha : 1.0f;

	if( g_pEngine )
	{
		return g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, fWidth, fHeight, fUStart, fVStart, fUEnd, fVEnd, uColor, ( UINT8 )( fAlphaValue * fAlphaRate ) );
	}

	return FALSE;
}

BOOL AcUIBar::_DrawBody( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	switch( m_eAnim )
	{
	case ACUIBAR_ANIM_NONE :
		{
			return _DrawBodyNoAnim( pTexture, lAbsX, lAbsY );
		}
		break;

	default :
		{
			return _DrawBodySmooth( pTexture, lAbsX, lAbsY );
		}
		break;
	}
}

BOOL AcUIBar::_DrawBodyNoAnim( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	INT32 lPosX = 0;
	INT32 lPosY = 0;
	INT32 lOffSet = 0;
	INT32 lWidth = 0;
	INT32 lHeight = 0;
	INT32 lUStart = 0;
	INT32 lVStart = 0;
	INT32 lUEnd = 0;
	INT32 lVEnd = 0;

	// 수직 Bar 의 경우..
	if( m_bVertical )
	{
		lPosX = lAbsX;
		lPosY = lAbsY + lOffSet;
		lOffSet = ( int )( ( 1.0f - m_fCurrRate ) * h );
		lWidth = w;
		lHeight = h - lOffSet;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h - lOffSet;
		}
		else
		{
			lUStart = 0;
			lVStart = lOffSet;
			lUEnd = w;
			lVEnd = h;
		}
	}
	// 수평 Bar 의 경우..
	else
	{
		lPosX = lAbsX;
		lPosY = lAbsY;
		lOffSet = ( int )( m_fCurrRate * w );
		lWidth = lOffSet;
		lHeight = h;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = lOffSet;
			lVEnd = h;
		}
		else
		{
			lUStart = w - lOffSet;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h;
		}
	}

	return _DrawTexture( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd );
}

BOOL AcUIBar::_DrawBodySmooth( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	INT32 lOffSetShadow = 0;
	INT32 lOffSetBody = 0;

	// 애니메이션 진행시간을 계산..
	UINT32	curtick = g_pEngine->GetClockCount();
	UINT32	tickdiff = curtick - m_ulLastTick;

	m_ulLastTick = curtick;
	m_fAfterImageMoveSpeed *= ( 1.0f + ACUIBAR_AFTERIMAGE_MOVE_ACCELERATION * tickdiff );

	FLOAT fAlphaValue = 0.0f;

	// 애니메이션 진행상황 체크..
	switch( m_eAnim )
	{
	case ACUIBAR_ANIM_INCREASE :
		{
			// 증가 애니메이션을 진행..
			m_fCurrRate += m_fAfterImageMoveSpeed * tickdiff;

			// 목표비율까지 진행되었으면..
			if( m_fCurrRate >= m_fAfterImageRate)
			{
				// 애니메이션 중지
				m_eAnim = ACUIBAR_ANIM_NONE;
				m_fCurrRate = m_fAfterImageRate;
			}

			fAlphaValue = 192.0f;
		}
		break;

	case ACUIBAR_ANIM_DECREASE :
		{
			// 감소 애니메이션을 진행..
			m_fAfterImageRate -= m_fAfterImageMoveSpeed * tickdiff;

			// 목표비율까지 진행되었으면..
			if( m_fAfterImageRate <= m_fCurrRate )
			{
				// 애니메이션 중지
				m_eAnim = ACUIBAR_ANIM_NONE;
			}

			fAlphaValue = 128.0f;
		}
		break;

	default :
		{
			// 이 함수에 진입했다는 것은 애니메이션이 있다는 것인데 증가도 감소도 아니라면 에러!
			return FALSE;
		}
		break;
	}

	// 수직 혹은 수평 Bar에 따라 해당하는 잔상과 Body 의 OffSet 을 구한다.
	if( m_bVertical )
	{
		lOffSetShadow = ( INT32 )( ( 1.0f - m_fAfterImageRate ) * h );
		lOffSetBody = ( INT32 )( ( 1.0f - m_fCurrRate ) * h );
	}
	else
	{
		lOffSetShadow = ( INT32 )( m_fAfterImageRate * w );
		lOffSetBody = ( int ) ( m_fCurrRate * w );
	}

	// 잔상을 먼저 그려주고..
	if( !_DrawBodySmoothShadow( pTexture, lAbsX, lAbsY, lOffSetShadow, fAlphaValue ) )
	{
		return FALSE;
	}

	// 실제 Body를 그려준다.
	if( !_DrawBodySmoothBody( pTexture, lAbsX, lAbsY, lOffSetBody, 255.0f ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AcUIBar::_DrawBodySmoothShadow( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue )
{
	INT32 lPosX = 0;
	INT32 lPosY = 0;
	INT32 lWidth = 0;
	INT32 lHeight = 0;
	INT32 lUStart = 0;
	INT32 lVStart = 0;
	INT32 lUEnd = 0;
	INT32 lVEnd = 0;

	if( m_bVertical )
	{
		lPosX = lAbsX;
		lPosY = lAbsY + lOffSet;
		lWidth = w;
		lHeight = h - lOffSet;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h - lOffSet;
		}
		else
		{
			lUStart = 0;
			lVStart = lOffSet;
			lUEnd = w;
			lVEnd = h;
		}
	}
	else
	{
		lPosX = lAbsX;
		lPosY = lAbsY;
		lWidth = lOffSet;
		lHeight = h;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = lOffSet;
			lVEnd = h;
		}
		else
		{
			lUStart = w - lOffSet;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h;
		}
	}

	RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDONE );
	return _DrawTexture( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd, fAlphaValue );
}

BOOL AcUIBar::_DrawBodySmoothBody( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue )
{
	INT32 lPosX = 0;
	INT32 lPosY = 0;
	INT32 lWidth = 0;
	INT32 lHeight = 0;
	INT32 lUStart = 0;
	INT32 lVStart = 0;
	INT32 lUEnd = 0;
	INT32 lVEnd = 0;

	if( m_bVertical )
	{
		lPosX = lAbsX;
		lPosY = lAbsY + lOffSet;
		lWidth = w;
		lHeight = h - lOffSet;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h - lOffSet;
		}
		else
		{
			lUStart = 0;
			lVStart = lOffSet;
			lUEnd = w;
			lVEnd = h;
		}
	}
	else
	{
		lPosX = lAbsX;
		lPosY = lAbsY;
		lWidth = lOffSet;
		lHeight = h;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = lOffSet;
			lVEnd = h;
		}
		else
		{
			lUStart = w - lOffSet;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h;
		}
	}

	RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCALPHA );
	return _DrawTexture( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd, fAlphaValue );
}

BOOL AcUIBar::_DrawEdge( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	FLOAT fPosX = 0.0f;
	FLOAT fPosY = 0.0f;
	FLOAT fWidth = 0.0f;
	FLOAT fHeight = 0.0f;
	FLOAT fUStart = 0.0f;
	FLOAT fVStart = 0.0f;
	FLOAT fUEnd = 0.0f;
	FLOAT fVEnd = 0.0f;
	FLOAT fAlphaValue = 255.0f;

	if( m_bVertical )
	{
		fPosX = ( FLOAT )lAbsX;
		fPosY = ( FLOAT )( lAbsY + h - 1 );
		fWidth = w;
		fHeight = 1.0f;
		fUStart = 0.0f;
		fVStart = 1.0f - 1.0f / ( FLOAT )pTexture->raster->height;
		fUEnd = 1.0f;
		fVEnd = 1.0f;
	}
	else
	{
		fPosX = ( FLOAT )lAbsX;
		fPosY = ( FLOAT )lAbsY;
		fWidth = 1.0f;
		fHeight = h;
		fUStart = 0.0f;
		fVStart = 0.0f;
		fUEnd = 1.0f / ( FLOAT )pTexture->raster->width;
		fVEnd = 1.0f;
	}

	return _DrawTexture( pTexture, fPosX, fPosY, fWidth, fHeight, fUStart, fVStart, fUEnd, fVEnd, fAlphaValue );
}



/*****************************************************************
*   Function : OnWindowRender
*   Comment  : OnWindowRender
*   Date&Time : 2003-09-29, 오후 4:02
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIBar::OnWindowRender()
{
	PROFILE( "AcUIBar::OnWindowRender" );

	// 기본적으로 Edge, Body Image가 각각 있어야 한다 
	if( !_IsTextureLoaded() )
	{
		// 텍스처가 로드되지 않은 경우 상위 함수를 콜하고 넘어간다.
		AcUIBase::OnWindowRender();
		return;
	}

	RwTexture* pBodyTexture = _GetBodyTexture();
	if( !_IsValidTexture( pBodyTexture ) ) return;

	RwTexture* pEdgeTexture = _GetEdgeTexture();
	if( !_IsValidTexture( pEdgeTexture ) ) return;

	// 현재 화면의 최대 X, Y 값을 구한다.
	INT32 lAbsoluteX = 0;
	INT32 lAbsoluteY = 0;
	ClientToScreen( &lAbsoluteX, &lAbsoluteY );

	// Body 그려주고..
	_DrawBody( pBodyTexture, lAbsoluteX, lAbsoluteY );

	// Edge 그리고..
	_DrawEdge( pEdgeTexture, lAbsoluteX, lAbsoluteY );
}
