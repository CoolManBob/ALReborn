// AcUIMinimap.cpp: implementation of the AcUIMinimap class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmMinimap.h"
#include "AcUIMinimap.h"
#include "AgcEngine.h"
#include "Apmmap.h"
#include "AgcmTargeting.h"
#include "AgcmMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static	FLOAT	__sRate	= 0.20f;

inline FLOAT	__GetDivisionStartX( INT32 nDivision , INT32 nPartIndex )
{
	INT32	lFirstSectorIndexX	= ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision ) );
	INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision ) );

	if( nPartIndex % 2 == 0 )
		return GetSectorStartX( lFirstSectorIndexX );
	else
		return GetSectorStartX( lFirstSectorIndexX ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
}

inline FLOAT	__GetDivisionStartZ( INT32 nDivision , INT32 nPartIndex )
{
	INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision ) );

	if( nPartIndex / 2 == 0 )
		return GetSectorStartZ( lFirstSectorIndexZ );
	else
		return GetSectorStartZ( lFirstSectorIndexZ ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
}


inline INT32 __GetMMPos( FLOAT fPos )
{
	return		( INT32 ) ( fPos /
				( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH ) *
				( ACUIMINIMAP_TEXTURE_SIZE << 1 ) );
}

inline FLOAT __GetMMPosINV( INT32 nPos )
{
	return		( FLOAT ) ( nPos ) / ( ACUIMINIMAP_TEXTURE_SIZE << 1 ) * ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH );
}

//static INT32	g_TextColor	= 0xfffff8e7;
//static INT32	g_BackColor	= 0x77000000;

AcUIMinimap::AcUIMinimap()
{
	m_pAgcmMinimap		= NULL;
	m_pAgpmCharacter	= NULL;
	
	m_nXPos	= 0;
	m_nZPos	= 0;

	m_nDivisionIndex	= -1;

	m_fDuration			= MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f * 
		( FLOAT ) ACUIMINIMAP_NEXT_DURATION / ( FLOAT ) ACUIMINIMAP_TEXTURE_SIZE;
}

AcUIMinimap::~AcUIMinimap()
{

}

void AcUIMinimap::OnWindowRender	()
{
	PROFILE("AcUIMinimap::OnWindowRender");

	if (!this->m_pAgcmMinimap->m_pcsSelfCharacter)
		return;

	RenderMinimap();

	UINT8	uAlphaWindow = ( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1.0f) );

	// 파티원 출력..

	RwTexture		*pMemberPosTexture	= m_pAgcmMinimap->GetMemberPosTexture();
	AgpdCharacter	*pcsSelfCharacter	= m_pAgcmMinimap->m_pcsSelfCharacter; 
	INT32 nX , nZ;
	INT32	nOffsetX, nOffsetZ	;

	if( pMemberPosTexture && pcsSelfCharacter )
	{
		INT32 nCenterX	= ( this->w >> 1 );
		INT32 nCenterZ	= ( this->h >> 1 );

		AgpdParty	*pcsParty	= this->m_pAgcmMinimap->m_pcsAgpmParty->GetPartyLock(pcsSelfCharacter); 
		
		if (pcsParty && m_pAgpmCharacter) 
		{ 
			for (int i = 0 ; i < pcsParty->m_nCurrentMember; ++i) 
			{
				AgpdCharacter	*pcsMember	= m_pAgpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);

				if (pcsMember == pcsSelfCharacter	||
					pcsMember == NULL				) 
					continue; 
				
				nX = __GetMMPos( pcsMember->m_stPos.x ) - m_nXPos + nCenterX;
				nZ = __GetMMPos( pcsMember->m_stPos.z ) - m_nZPos + nCenterZ;

				if( nX < 4				) continue;
				if( nX >= this->w - 4	) continue;
				if( nZ < 4				) continue;
				if( nZ >= this->h - 4	) continue;

				// 마고자 (2004-04-07 오후 9:44:18) : 
				// 중앙점 8,19
				nOffsetX	= nX - 4;// 6은 커서의 중점 옵셋..
				nOffsetZ	= nZ - 4;

				this->ClientToScreen(&nOffsetX, &nOffsetZ);

				g_pEngine->DrawIm2DPixel(
						pMemberPosTexture	, 
						nOffsetX			, 
						nOffsetZ			, 
						8					,
						8					,
						0					,
						0					,
						-1					,
						-1					,
						m_lColor			,
						uAlphaWindow
						);

			} 

			pcsParty->m_Mutex.Release();
		}	
	}
	

	// 미니맵 포인트 출력..
	{	

		INT32	nCenterX, nCenterZ	;
		BOOL	bNeedPopup	= FALSE;
		//INT32	nType;
		char	*pStrText = NULL;
		INT32	nPopupX , nPopupY;
		INT32	nStartY;
		INT32	lDisplayCount	= 0;
		INT32	lFontHeight		= m_pAgcmMinimap->m_pcsAgcmFont->m_astFonts[m_lFontType].lSize;

		AgcmMinimap::MPInfo *	pArray = this->m_pAgcmMinimap->GetMPArray();
		INT32					nCount = this->m_pAgcmMinimap->GetMPCount();

		RwTexture * pTexturePoint;
		INT32		nWidth		;
		INT32		nWidthHalf	;

		nCenterX	= ( this->w >> 1 );
		nCenterZ	= ( this->h >> 1 );

		for( int i = 0 ; i < nCount ; i ++ )
		{
			if( !pArray[ i ].bDisabled )
			{
				pArray[ i ].bRendered = FALSE;

				switch( pArray[ i ].nType )
				{
				case AgcmMinimap::MPInfo::MPI_QUESTNEW:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetNpcQuestNewTexture();
						nWidth			= 16;
					}
					break;
				case AgcmMinimap::MPInfo::MPI_QUESTINCOMPLETE:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetNpcQuestIncompleteTexture();
						nWidth			= 16;
					}
					break;

				default:
				case AgcmMinimap::MPInfo::MPI_NPC:
				case AgcmMinimap::MPInfo::MPI_NONE:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetNpcNormalTexture();
						nWidth			= 10;
					}
					break;
				case AgcmMinimap::MPInfo::MPI_SIEGENORMAL:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetTextureSiegeNormal();
						nWidth			= 32;
					}
					break;
				case AgcmMinimap::MPInfo::MPI_SIEGEDESTROIED:
					{
						pTexturePoint	= this->m_pAgcmMinimap->GetTextureSiegeDestroied();
						nWidth			= 32;
					}
					break;
				}

				nWidthHalf		= nWidth >> 1;

				// protection!
				if( NULL == pTexturePoint ) continue;

				nX = __GetMMPos( pArray[ i ].pos.x ) - m_nXPos + nCenterX;
				nZ = __GetMMPos( pArray[ i ].pos.z ) - m_nZPos + nCenterZ;

				if( nX < nWidthHalf				) continue;
				if( nX >= this->w - nWidthHalf	) continue;
				if( nZ < nWidthHalf				) continue;
				if( nZ >= this->h - nWidthHalf	) continue;

				nOffsetX	= nX - nWidthHalf;
				nOffsetZ	= nZ - nWidthHalf;

				// 팝업 체크..
				if( ( INT32 ) m_prevMouseState.pos.x >= nOffsetX			&&
					( INT32 ) m_prevMouseState.pos.x < nOffsetX + nWidth	&&
					( INT32 ) m_prevMouseState.pos.y >= nOffsetZ			&&
					( INT32 ) m_prevMouseState.pos.y < nOffsetZ + nWidth	)
				{
					++lDisplayCount;
					pArray[ i ].bOnMouse	= TRUE;
				}
				else
				{
					pArray[ i ].bOnMouse	= FALSE;
				}

				this->ClientToScreen(&nOffsetX, &nOffsetZ);

				g_pEngine->DrawIm2DPixel(
						pTexturePoint	, 
						nOffsetX			, 
						nOffsetZ			, 
						nWidth				,
						nWidth				,
						0					,
						0					,
						-1					,
						-1					,
						m_lColor					,
						uAlphaWindow );

				pArray[ i ].bRendered = TRUE;
			}
		}

		if( this->m_pAgcmMinimap->m_bShowAllCharacter )
		{
			AgpdCharacter			*pcsCharacter;
			INT32					lIndex = 0;

			for (pcsCharacter = m_pAgpmCharacter->GetCharSequence(&lIndex);
				pcsCharacter;
				pcsCharacter = m_pAgpmCharacter->GetCharSequence(&lIndex))
			{
				if( this->m_pAgcmMinimap->m_pcsAgpmCharacter->IsPC(pcsCharacter )  &&
					pcsCharacter != this->m_pAgcmMinimap->m_pcsSelfCharacter )
				{
					RwTexture * pTexturePoint = this->m_pAgcmMinimap->GetNpcNormalTexture();
					INT32		nWidth		= 10;
					INT32		nWidthHalf	= nWidth >> 1;

					bool bShowName = false;
					nX = __GetMMPos( pcsCharacter->m_stPos.x ) - m_nXPos + nCenterX;
					nZ = __GetMMPos( pcsCharacter->m_stPos.z ) - m_nZPos + nCenterZ;

					if( nX < nWidthHalf				) continue;
					if( nX >= this->w - nWidthHalf	) continue;
					if( nZ < nWidthHalf				) continue;
					if( nZ >= this->h - nWidthHalf	) continue;

					nOffsetX	= nX - nWidthHalf;
					nOffsetZ	= nZ - nWidthHalf;

					// 팝업 체크..
					if( ( INT32 ) m_prevMouseState.pos.x >= nOffsetX			&&
						( INT32 ) m_prevMouseState.pos.x < nOffsetX + nWidth	&&
						( INT32 ) m_prevMouseState.pos.y >= nOffsetZ			&&
						( INT32 ) m_prevMouseState.pos.y < nOffsetZ + nWidth	)
					{
						++lDisplayCount;
						bShowName = true;
					}

					this->ClientToScreen(&nOffsetX, &nOffsetZ);

					g_pEngine->DrawIm2DPixel(
							pTexturePoint	, 
							nOffsetX			, 
							nOffsetZ			, 
							nWidth				,
							nWidth				,
							0					,
							0					,
							-1					,
							-1					,
							m_lColor					,
							uAlphaWindow );

					if( bShowName )
					{
						m_pAgcmMinimap->m_pcsAgcmFont->FontDrawStart(m_lFontType);
						m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2D(
							( FLOAT ) nOffsetX, ( FLOAT ) nOffsetZ - lFontHeight ,
							pcsCharacter->m_szID,	//  Text
							0 ,			//  font type
							uAlphaWindow,		//  alpha
							0xfffff8e7, //g_TextColor,	// color
							true,		// Draw Shadow
							false,		// Blod
							true,		// Draw Background
							0x77000000	//g_BackColor	// Background color
							);
						m_pAgcmMinimap->m_pcsAgcmFont->FontDrawEnd();
					}
				}
			}
		}

		// 커서 출력.
		{
			RwTexture * pCursor = this->m_pAgcmMinimap->GetCursorTexture();

			// 중앙점..
			nCenterX	= ( this->w >> 1 );
			nCenterZ	= ( this->h >> 1 );
			if( pCursor )
			{
				// 커서 출력..
				// 방위 알아냄.. 
				INT32	nOffsetX, nOffsetZ	;


				// 마고자 (2004-04-07 오후 9:44:18) : 
				// 중앙점 8,19
				nOffsetX	= nCenterX - 6	;// 6은 커서의 중점 옵셋..
				nOffsetZ	= nCenterZ - 23	;
				
				this->ClientToScreen(&nOffsetX, &nOffsetZ);
				this->ClientToScreen(&nCenterX, &nCenterZ);

				g_pEngine->DrawIm2DRotate( 
					pCursor, 
					(float) ( nOffsetX ), 
					(float) ( nOffsetZ ), 
					(float) ( 14 )	,	// 이미지 싸이즈
					(float) ( 29 )	,	// 이미지 헤이트.
					(float) 0.0f	,			
					(float) 0.0f	,			
					(float) 0.4375f	,			
					(float) 0.90625f,
					( FLOAT ) nCenterX , ( FLOAT ) nCenterZ , 360 - this->m_fCurrentTurnY + 180,
					m_lColor					,
					uAlphaWindow
					);

				// 커서 빙빙..
				static	UINT32	uLastTime	= GetTickCount();
				UINT32	uTimeDelta	= GetTickCount() - uLastTime;
				uLastTime = GetTickCount();

				FLOAT	fCurrentY	= m_fCurrentTurnY										;
				FLOAT	fTurnY		= this->m_pAgcmMinimap->m_pcsSelfCharacter->m_fTurnY	;

				FLOAT	fDelta	= fTurnY - fCurrentY;
				FLOAT	fDelta2	;
				if( fDelta > 180.0f )
				{
					fDelta -= 360;
				}
				else if( fDelta < -180.0f )
				{
					fDelta += 360;
				}

				if( fDelta > 0 )
				{
					m_fCurrentTurnY += ( FLOAT )uTimeDelta * __sRate;

					fDelta2	= fTurnY - m_fCurrentTurnY;
					if( fDelta2 > 180.0f )
					{
						fDelta2 -= 360;
					}
					else if( fDelta2 < -180.0f )
					{
						fDelta2 += 360;
					}

					if( fDelta2 < 0 )
					{
						m_fCurrentTurnY = this->m_pAgcmMinimap->m_pcsSelfCharacter->m_fTurnY;
					}
				}
				else if( fDelta < 0 )
				{
					m_fCurrentTurnY -= ( FLOAT )uTimeDelta * __sRate;

					fDelta2	= fTurnY - m_fCurrentTurnY;
					if( fDelta2 > 180.0f )
					{
						fDelta2 -= 360;
					}
					else if( fDelta2 < -180.0f )
					{ 
						fDelta2 += 360;
					}

					if( fDelta2 > 0 )
					{
						m_fCurrentTurnY = this->m_pAgcmMinimap->m_pcsSelfCharacter->m_fTurnY;
					}
				}
				else
				{
					// do nothing..
				}
			}
		}

		if( lDisplayCount)
		{
			INT32	nTextWidth;
			BOOL	bAlignBottom;
			INT32	lMarginYTop;
			INT32	lMarginYBottom;

			lMarginYBottom	= h - ( INT32 ) (m_prevMouseState.pos.y + lFontHeight * (lDisplayCount + 1));
			lMarginYTop		= ( INT32 ) ( m_prevMouseState.pos.y - lFontHeight * (lDisplayCount ) );
			if (lMarginYTop >= 0)
				bAlignBottom	= FALSE;
			else if (lMarginYBottom >= 0)
				bAlignBottom	= TRUE;
			else if (lMarginYTop >= lMarginYBottom)
				bAlignBottom	= FALSE;
			else
				bAlignBottom	= TRUE;

			if (bAlignBottom)
				nStartY = ( INT32 ) m_prevMouseState.pos.y + lFontHeight;
			else
				nStartY = ( INT32 ) m_prevMouseState.pos.y - lFontHeight;

			// 팝업 표시..
			m_pAgcmMinimap->m_pcsAgcmFont->FontDrawStart(m_lFontType);

			for( int i = 0 ; i < nCount ; i ++ )
			{
				if (pArray[ i ].bDisabled || !pArray[ i ].bOnMouse || !pArray[ i ].strText[0])
					continue;

				nTextWidth = m_pAgcmMinimap->m_pcsAgcmFont->GetTextExtent( 0 , pArray[ i ].strText, strlen(pArray[ i ].strText) );

				// 가운데로 위치시킴..
				nPopupX = ( INT32 ) m_prevMouseState.pos.x - (nTextWidth >> 1);
				nPopupY = nStartY;

				ClientToScreen(&nPopupX, &nPopupY);

//static INT32	g_TextColor	= 0xfffff8e7;
//static INT32	g_BackColor	= 0x77000000;

				m_pAgcmMinimap->m_pcsAgcmFont->DrawTextIM2D(
					( FLOAT ) nPopupX, ( FLOAT ) nPopupY ,
					pArray[ i ].strText,	//  Text
					0 ,			//  font type
					uAlphaWindow,		//  alpha
					0xfffff8e7, //g_TextColor,	// color
					true,		// Draw Shadow
					false,		// Blod
					true,		// Draw Background
					0x77000000	//g_BackColor	// Background color
					);

				if (bAlignBottom)
					nStartY += lFontHeight;
				else
					nStartY -= lFontHeight;
			}

			m_pAgcmMinimap->m_pcsAgcmFont->FontDrawEnd();
		}
	}




}

BOOL AcUIMinimap::OnInit			()
{
	w = 150;
	h = 150;
	m_fCurrentTurnY	= 0.0f;
	return TRUE;
}

void AcUIMinimap::OnClose			()
{
}

// 디비젼과 파트인덱스로 맵 텍스쳐를 생성한다.
BOOL	AcUIMinimap::RenderMinimap	()
// nPartIndex는 16까지의 숫자로 디비젼을 16등분을 한다.
// 해당 텍스쳐 로딩과 뭉쳐진 새 텍스쳐 생성을 한다.
{
	if (this->m_pAgcmMinimap->m_pcsSelfCharacter == NULL)
		return TRUE;

	INT32	nDivisionIndex , nPartIndex;
	{
		FLOAT	fX = this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.x , fZ = this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.z;
		FLOAT	fLocalX , fLocalZ	;

		// 좌측위 픽셀의 디비젼 인덱스를 구함.
		fX	-= MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH * this->w / ( 2.0f * 2.0f * ACUIMINIMAP_TEXTURE_SIZE );
		fZ	-= MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH * this->h / ( 2.0f * 2.0f * ACUIMINIMAP_TEXTURE_SIZE );

		nDivisionIndex	= GetDivisionIndexF( fX , fZ );

		// 해당 디비젼에서의 옵셋을 구함..
		INT32	lFirstSectorIndexX	= ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
		INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );

		fLocalX = this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.x - GetSectorStartX( lFirstSectorIndexX );
		fLocalZ = this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.z - GetSectorStartZ( lFirstSectorIndexZ );

		INT32	nX , nZ;

		nX	= ( INT32 ) ( fLocalX / ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 4.0f ) );
		nZ	= ( INT32 ) ( fLocalZ / ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 4.0f ) );

		if( nX > 3 ) nX = 3;
		if( nZ > 3 ) nZ = 3;

		nPartIndex = nX + nZ * 4;
	}

	// nDivisionIndex , nPartIndex;

	FLOAT	fLocalX , fLocalZ	;
	INT32	lFirstSectorIndexX	= ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
	INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );
	fLocalX = GetSectorStartX( lFirstSectorIndexX );
	fLocalZ = GetSectorStartZ( lFirstSectorIndexZ );

	m_fWidth	= MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;

	INT32	nDVX				= GetDivisionXIndex( nDivisionIndex );
	INT32	nDVZ				= GetDivisionZIndex( nDivisionIndex );

	INT32	nDivisionRight			= MakeDivisionIndex( nDVX + 1	, nDVZ		);
	INT32	nDivisionBottom			= MakeDivisionIndex( nDVX		, nDVZ + 1	);
	INT32	nDivisionRightBottom	= MakeDivisionIndex( nDVX + 1	, nDVZ + 1	);
	INT32	nDivisionTemp			;
	INT32	nOffset					;

	INT32	nHalf				= ACUIMINIMAP_TEXTURE_SIZE >> 1;

	RwTexture	* pMinimapTexture = NULL;
	RwTexture	* pCurrentTexture = NULL;
	bool		bFirst = true;

	BOOL		bInvalidMapPosition = TRUE;

	for( int j = 0 ; j < 5 ; ++ j )
	{
		for( int i = 0 ; i < 6 ; ++ i )
		{
			// 디비젼 인덱스 체크.
			nDivisionTemp	= MakeDivisionIndex( nDVX + i / 2 , nDVZ + j / 2 );
			nOffset			= i % 2 + ( j % 2 ) * 2;
			pCurrentTexture = RenderDivision( nDivisionTemp , nOffset );

			if( pCurrentTexture )
			{
				if( bFirst  )
				{
					bFirst = false;
					pMinimapTexture = pCurrentTexture;
				}
				else
				{
					if( pMinimapTexture != pCurrentTexture )
						bInvalidMapPosition = FALSE;
				}
			}
		}
	}

	/*
	if( bInvalidMapPosition )
	{
		// 맵에 표시돼는 부분이 모두 같은 이미지
		// 즉 더미이다. 잘못됀 포지션이므로 UI 를 닫아 버린다.
		this->m_pAgcmMinimap->m_pcsAgcmUIManager2->ThrowEvent(this->m_pAgcmMinimap->m_lEventCloseMiniMapUI);
	}
	*/

	if( this->m_pAgcmMinimap->m_pcsSelfCharacter )
	{
		ApmMap::RegionTemplate*	pTemplate = this->m_pAgcmMinimap->m_pcsApmMap->GetTemplate( this->m_pAgcmMinimap->m_pcsSelfCharacter->m_nBindingRegionIndex );

		if( pTemplate && pTemplate->ti.stType.bDisableMinimap )
		{
			this->m_pAgcmMinimap->m_pcsAgcmUIManager2->ThrowEvent(this->m_pAgcmMinimap->m_lEventCloseMiniMapUI);
		}
	}
		
	return TRUE;
}

BOOL __bUseClip = FALSE;
RwTexture *	AcUIMinimap::RenderDivision		( INT32 nDivisionIdnex , INT32 nPartIndex  )
{
	RwTexture * pTexture ;
	FLOAT fStartX	= __GetDivisionStartX( nDivisionIdnex , nPartIndex );
	FLOAT fStartZ	= __GetDivisionStartZ( nDivisionIdnex , nPartIndex );

	INT32	nX;
	INT32	nZ;

	nX		=	__GetMMPos( fStartX );
	nZ		=	__GetMMPos( fStartZ );

	// 케릭터 기준으로 옵셋을 구함..
	// nX , nZ 는 스크린 좌표..
	// 실제론. .여기서 부터 찍어야 정상..

	nX		=	nX - m_nXPos + this->w / 2 ;
	nZ		=	nZ - m_nZPos + this->h / 2 ;

	INT32	nStartX , nStartZ	;
	INT32	nEndX	, nEndZ		;
	INT32	nUStart = 0, nVStart = 0	;
	INT32	nUEnd = ACUIMINIMAP_TEXTURE_SIZE, nVEnd = ACUIMINIMAP_TEXTURE_SIZE;

	nStartX	= nX		;
	nStartZ	= nZ		;
	nEndX	= nStartX	+ ACUIMINIMAP_TEXTURE_SIZE		;
	nEndZ	= nStartZ	+ ACUIMINIMAP_TEXTURE_SIZE		;

	if( nStartX < 0 )
	{
		nUStart	= (-nStartX);
		nStartX	= 0;
	}

	if( nStartZ < 0 )
	{
		nVStart	= (-nStartZ );
		nStartZ = 0;
	}

	if( nEndX >= this->w )
	{
		nUEnd	-= nEndX - this->w;
		nEndX	= this->w ;
	}
	if( nEndZ >= this->h )
	{
		nVEnd	-= nEndZ - this->h;
		nEndZ	= this->h ;
	}

	this->ClientToScreen(&nStartX, &nStartZ);
	this->ClientToScreen(&nEndX, &nEndZ);

	pTexture = this->m_pAgcmMinimap->GetMinimapTexture( nDivisionIdnex , nPartIndex );

	if( ( nEndX - nStartX ) > 0 && ( nEndZ - nStartZ ) > 0 && pTexture )
	{
// 마고자 (2004-05-25 오후 8:03:14) : 테스트용 코드
		//if( pTexture )
		//{
			if( __bUseClip )
			{
				this->ClientToScreen(&nX, &nZ);
				g_pEngine->DrawIm2DPixel(
						pTexture					, 
						nX							, 
						nZ							, 
						ACUIMINIMAP_TEXTURE_SIZE	,
						ACUIMINIMAP_TEXTURE_SIZE	,
						0							,			
						0							,			
						ACUIMINIMAP_TEXTURE_SIZE	,			
						ACUIMINIMAP_TEXTURE_SIZE	);
			}
			else
			{
				g_pEngine->DrawIm2DPixel(
						pTexture					, 
						nStartX						, 
						nStartZ						, 
						nEndX - nStartX				,
						nEndZ - nStartZ				,
						nUStart						,			
						nVStart						,			
						nUEnd						,			
						nVEnd						,
						m_lColor					,
						( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1)));
			}
		//}
		return pTexture;
	}
	else
	{
		return NULL;
	}
}

void	AcUIMinimap::PositionUpdate()
{
	if( NULL == this->m_pAgcmMinimap ) return;
	if( NULL == this->m_pAgcmMinimap->m_pcsSelfCharacter ) return;

	// 위치계산..
	m_nXPos	=	__GetMMPos( this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.x );
	m_nZPos	=	__GetMMPos( this->m_pAgcmMinimap->m_pcsSelfCharacter->m_stPos.z );                          
}

BOOL AcUIMinimap::OnMouseMove	( RsMouseStatus *ms	)
{
	m_prevMouseState = *ms;

	//ScreenToClient( stat );

	return FALSE;
}

BOOL AcUIMinimap::OnLButtonDown	( RsMouseStatus *ms	)
{
	INT32 nCenterX	= ( this->w >> 1 );
	INT32 nCenterZ	= ( this->h >> 1 );

	FLOAT	fX , fZ;
	fX = __GetMMPosINV( (INT32) ms->pos.x + m_nXPos - nCenterX );
	fZ = __GetMMPosINV( (INT32) ms->pos.y + m_nZPos - nCenterZ );

	char str[ 256 ];
	sprintf( str , "미니맵 클릭 (%f,%f)\n" , fX , fZ );
	TRACE( str );

	AgcmMap * pAgcmMap = ( AgcmMap * ) this->m_pAgcmMinimap->GetModule( "AgcmMap" );

	// 마고자 (2005-03-11 오후 3:09:47) : 
	// 패스파인딩 사용함.
	AuPOS pos;
	pos.x = fX;
	pos.z = fZ;
	if( pAgcmMap )
		pos.y = pAgcmMap->GetHeight( fX , fZ , SECTOR_MAX_HEIGHT );
	else
		pos.y = 0.0f;

	AgcmTargeting	*pAgcmTargeting = ( AgcmTargeting * ) this->m_pAgcmMinimap->GetModule( "AgcmTargeting" );
	ASSERT( NULL != pAgcmTargeting );
	if( pAgcmTargeting )
		pAgcmTargeting->MoveSelfCharacter( &pos );

	return TRUE;
}

BOOL AcUIMinimap::OnRButtonUp	( RsMouseStatus *ms	)
{
	INT32 nCenterX	= ( this->w >> 1 );
	INT32 nCenterZ	= ( this->h >> 1 );

	FLOAT	fX , fZ;
	fX = __GetMMPosINV( (INT32) ms->pos.x + m_nXPos - nCenterX );
	fZ = __GetMMPosINV( (INT32) ms->pos.y + m_nZPos - nCenterZ );

	char strMessage[ 256 ];
	sprintf( strMessage , "/move %f,%f" , fX , fZ );

	AgcmChatting2 * pcsAgcmChatting2 = ( AgcmChatting2 * ) m_pAgcmMinimap->GetModule( "AgcmChatting2" );

	if( pcsAgcmChatting2 )
	{
		pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , strMessage , strlen( strMessage ) , NULL );
	}

	return TRUE;
}
