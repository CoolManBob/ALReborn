#include "AgcmUIEventGacha.h"   
#include "AgcmItem.h"
#include "AgpmItem.h"

const INT32 g_cnWidth = 50; // 슬롯 가로 크기. 
const FLOAT g_cfDisplayScrollOffset = 1.3f;	// 1초에 보여주는 아이템 갯수
static UINT32 s_nGachaRollingTime = cGacha_Rolling_Time; // 10초

INT32	g_nRotateCount	= 7;

static	AgpmItem	*	s_pcsAgpmItem	= NULL;
static	AgcmItem	*	s_pcsAgcmItem	= NULL;

AcUIGacha::AcUIGacha():
	m_nPosition		( 0 ),
	m_nMaxPosition	( 0 ),
	m_uPrevTick		( 0 ),
	m_eRollingMode	( RM_DISPLAY ),
	m_uRollingStartTime( 0 ),
	m_nResultStartOffset( 0 ),
	m_pcsAgcmUIEventGacha( NULL ),
	m_nResultTID			( 0 ),
	m_pSplineData			( NULL ),
	m_pCurrentSlot			( NULL )
{
}

AcUIGacha:: ~AcUIGacha()
{
}

BOOL AcUIGacha::OnInit			()
{
	// 모듈 포인터 확보.
	s_pcsAgpmItem	= ( AgpmItem * ) g_pEngine->GetModule( "AgpmItem" );
	s_pcsAgcmItem	= ( AgcmItem * ) g_pEngine->GetModule( "AgcmItem" );

	m_uPrevTick			= ::GetTickCount();
	m_uRollingStartTime	= ::GetTickCount();

	return TRUE;
}

BOOL AcUIGacha::OnPostInit()
{
	return TRUE;
}

BOOL	AcUIGacha::SetSlotItems( vector< INT32 > * pVector )
{
	// 변수 초기화
	m_nPosition		= 0;
	m_nMaxPosition	= ( INT32 ) g_cnWidth * ( INT32 ) pVector->size();
	m_uPrevTick		= 0;
	m_eRollingMode	= RM_DISPLAY;
	m_pCurrentSlot	= NULL;

	m_vecSlot.clear();

	vector< INT32 >::iterator	iter;
	for( iter = pVector->begin();
		iter != pVector->end();
		iter++ )
	{
		INT32	nTID = *iter;

		AgpdItemTemplate	* pcsAgpdItemTemplate	 = s_pcsAgpmItem->GetItemTemplate( nTID );
		ASSERT( pcsAgpdItemTemplate );	
		
		AgcdItemTemplate	* pcsAgcdItemTemplate	 = s_pcsAgcmItem->GetTemplateData( pcsAgpdItemTemplate );
		ASSERT( pcsAgcdItemTemplate );

		VERIFY( s_pcsAgcmItem->LoadTemplateTexture( pcsAgpdItemTemplate , pcsAgcdItemTemplate ) );

		RwTexture * pcsTexture = pcsAgcdItemTemplate->m_pTexture;

		// 에러가 있어선 안된다.
		ASSERT( pcsTexture );

		SlotMachine	stSlot;
		stSlot.nTID		= nTID		;
		stSlot.pTexture	= pcsTexture;
		stSlot.strName	= pcsAgpdItemTemplate->m_szName;

		m_vecSlot.push_back( stSlot );
	}

	//최초 표시될 녀석 설정
	if( m_vecSlot.size() )
	{
		m_pcsAgcmUIEventGacha->m_stringName = m_vecSlot[ ( 2 ) % m_vecSlot.size() ].strName;
	}
	else
	{
		m_pcsAgcmUIEventGacha->m_stringName = "----";
	}
	return TRUE;
}

BOOL AcUIGacha::OnIdle	( UINT32 ulClockCount	)
{
	ulClockCount	= ::GetTickCount();

	UINT32 uDiff = ulClockCount - m_uPrevTick;
	UINT32	uOffsetTime	= ulClockCount - m_uRollingStartTime;

	switch( m_eRollingMode )
	{
	case RM_DISPLAY:
		{
			const INT32 nCount = ( INT32 ) m_vecSlot.size(); // 아이템의 갯수

			m_nPosition = ( UINT32 ) ( ( FLOAT )( uOffsetTime ) * ( FLOAT ) g_cnWidth * g_cfDisplayScrollOffset / 1000.0f );
		}
		break;
	case RM_ROLLING:
		{
			//const INT32 nTargetPosition ;

			// 이건 결과 TID의 Index를 알아내고 , 일정 바퀴 돈후에 해당 포지션에 멈추게 하기 위해 미리 계산해둔다.
			// nTargetIndex = 알아서 구-하기;
			// nTargetPosition = nWidth * ( nCount * nRolling + nTargetIndex + 2 ( 가운데 놓기 위해서 ) );

			if( uOffsetTime >= s_nGachaRollingTime )
			{
				m_nPosition = m_nResultPosition;
				m_eRollingMode = RM_RESULT;
				this->m_pcsAgcmUIEventGacha->OnRollEnd();
			}
			else
			{
				double	dTime	= ( double ) uOffsetTime / ( double ) s_nGachaRollingTime;

				BOOL	bShift = g_pEngine->IsLShiftDown();

				if( m_pSplineData )
				{
					// 스프라인 데이타가 있으면 그걸 이요함.
					RwMatrix	mat;
					m_pSplineData->bOnIdleOffset( mat , uOffsetTime );
					m_nPosition = m_nResultStartOffset + ( INT32 ) ( mat.pos.y * ( double ) ( m_nResultPosition - m_nResultStartOffset )) ;
				}
				else
				{
					if( bShift )
					{
						dTime	= 0.2 + dTime * 0.8;
						double	x	= (1-cos( 3.1415927 * dTime )) * 0.5 ;
						double	x2	= 3.1415927 * x;
						m_nPosition = m_nResultStartOffset + ( INT32 ) ( ( 1.0 - cos( x2 ) ) / 2.0 * ( double ) ( m_nResultPosition - m_nResultStartOffset ));
					}
					else
					{
						m_nPosition = m_nResultStartOffset + ( INT32 ) ( ( 1.0 - cos( 3.1415927 * dTime ) ) / 2.0 * ( double ) ( m_nResultPosition - m_nResultStartOffset ));
					}
				}
			}
		}
		break;
	case RM_RESULT:
		// 암것도 안함...
		// 걸린 아이템 표시해줌?..
		break;
	}

	m_uPrevTick = ulClockCount;

	if( m_vecSlot.size() )
	{
		static INT32 nLastImage = 0;

		INT32 nFirstImage = ( m_nPosition / g_cnWidth ) % m_vecSlot.size();    // 최초 출력될 이미지

		if( nFirstImage != nLastImage )
		{
			m_pCurrentSlot	= &m_vecSlot[ ( nFirstImage + 2 ) % m_vecSlot.size() ];
			m_pcsAgcmUIEventGacha->m_stringName = m_pCurrentSlot->strName;
			nLastImage = nFirstImage;

			m_pcsAgcmUIEventGacha->UpdateControlItem();

			static	AgcmSound	* pcsAgcmSound = ( AgcmSound * ) g_pEngine->GetModule( "AgcmSound" );

			if( ulClockCount % 2 )	pcsAgcmSound->PlaySampleSound( "SOUND\\UI\\U_LB_A1.wav" );
			else					pcsAgcmSound->PlaySampleSound( "SOUND\\UI\\U_LB_A2.wav" );
		}
	}

	return TRUE;
}

void AcUIGacha::OnWindowRender	()
{
	if( !m_vecSlot.size() ) return;

	UINT8	uAlphaWindow = ( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1.0f) );

	const INT32 nDisplayCount = 6;
	INT32 nModulatePosition = 0;
	nModulatePosition = m_vecSlot.size() ? m_nPosition % ( INT32 ) ( m_vecSlot.size() * g_cnWidth ) : 200 ;
	const INT32 nScreenOffsetX = 0;		// 이미지 옵셋..
	const INT32 nScreenOffsetY = 0;		// 이미지 옵셋..

	INT32 nFirstImage = ( m_nPosition / g_cnWidth ) % m_vecSlot.size();    // 최초 출력될 이미지

	for( INT32 nIndex = nFirstImage ; nIndex < nFirstImage + nDisplayCount ; nIndex ++ ) // 디스플레이 갯수만큼 반복
	{
		INT32 nStart = nIndex * g_cnWidth - nModulatePosition ;
		if( nStart + x < 0 ) continue;
		if( nStart + x + g_cnWidth >= 320) continue; // 320은 UI하드코딩

		SlotMachine	* pSlotMachine = &m_vecSlot[ nIndex % m_vecSlot.size() ];

		INT32	nOffsetX , nOffsetZ;
		nOffsetX = nScreenOffsetX + nStart;
		nOffsetZ = nScreenOffsetY;

		this->ClientToScreen(&nOffsetX, &nOffsetZ);

		g_pEngine->DrawIm2DPixel(
			pSlotMachine->pTexture	, 
			nOffsetX				, 
			nOffsetZ				, 
			g_cnWidth				,
			g_cnWidth				,
			0					,
			0					,
			-1					,
			-1					,
			m_lColor			,
			uAlphaWindow );
	}

	const INT32 lCoverImage = 1;
	RwTexture * pTexture = m_stRenderInfo.lRenderID ? m_csTextureList.GetImage_ID( lCoverImage ) : NULL;
	if( pTexture )
	{
		INT32	nOffsetX , nOffsetZ;

		// 커버이미지 위치 하드 코딩
		nOffsetX = 0;
		nOffsetZ = 68;

		this->pParent->ClientToScreen(&nOffsetX, &nOffsetZ);

		g_pEngine->DrawIm2DPixel(
			pTexture	, 
			nOffsetX				, 
			nOffsetZ				, 
			316					,	// 커버이미지 위치 하드 코딩
			95					,	// 커버이미지 위치 하드 코딩
			0					,
			0					,
			-1					,
			-1					,
			m_lColor			,
			uAlphaWindow );
	}

	
	switch( m_eRollingMode )
	{
	default:
		break;

	case RM_RESULT:
		{
			const INT32 lPopupImage = 2;
			RwTexture * pTexture = m_stRenderInfo.lRenderID ? m_csTextureList.GetImage_ID( lPopupImage ) : NULL;

			if( !m_pCurrentSlot ) break;
			RwTexture * pItemImage = m_pCurrentSlot->pTexture;

			if( pTexture )
			{
				// 커버이미지 위치 하드 코딩
				INT32 nOffsetX = 107;
				INT32 nOffsetZ = 66;

				this->pParent->ClientToScreen(&nOffsetX, &nOffsetZ);
				RwRaster* pRaster = RwTextureGetRaster( pTexture );
				if( pRaster )
				{
					g_pEngine->DrawIm2DPixel( pTexture, nOffsetX, nOffsetZ, pRaster->width, pRaster->height, 0, 0, -1, -1, m_lColor, uAlphaWindow );
				}

				nOffsetX = 120;
				nOffsetZ = 79;

				this->pParent->ClientToScreen(&nOffsetX, &nOffsetZ);
				RwTextureFilterMode eFilterMode = RwTextureGetFilterMode( pItemImage );
				RwTextureSetFilterMode(pItemImage, rwFILTERLINEAR);

				pRaster = RwTextureGetRaster( pItemImage );
				if( pRaster )
				{
					//g_pEngine->DrawIm2DPixel( pItemImage, nOffsetX, nOffsetZ, pRaster->width, pRaster->height, 0, 0, -1, -1, m_lColor, uAlphaWindow );
					g_pEngine->DrawIm2D( pItemImage, ( float )nOffsetX, ( float )nOffsetZ, 76, 76, 0.0f, 0.0f, 1.0f, 1.0f, m_lColor, uAlphaWindow );
				}

				RwTextureSetFilterMode(pItemImage, eFilterMode);
			}

		}
		break;
	}

}

BOOL    AcUIGacha::StartRoll( INT32 nTIDResult )
{
	if( m_eRollingMode == RM_ROLLING ) return FALSE;

	// 우선 들어있는 놈인지 계산..

	INT32	nIndex = 0;
	BOOL	bFound = FALSE;
	vector< SlotMachine >::iterator	iter;
	for( iter = m_vecSlot.begin();
		iter != m_vecSlot.end();
		iter++ , nIndex ++ )
	{
		SlotMachine	* pSlot = &(*iter);

		if( pSlot->nTID == nTIDResult )
		{
			bFound = TRUE;
			break;
		}
	}

	if( !bFound )
	{
		// 가차 테이블에 없는 아이템이 -_-?...
		// 그냥 롤링 화면 없이 결과를 바로 표시함.
		this->m_pcsAgcmUIEventGacha->OnRollEnd();
		return FALSE;
	}

	m_nPosition			= m_nPosition % m_nMaxPosition;
	m_nResultStartOffset= m_nPosition;
	m_nResultPosition	= 
		( 
			m_vecSlot.size() * g_nRotateCount  // 기본적으로 몇바퀴 돌고..
			+ nIndex // 이게 목표
			- 2			// 이건 중앙 맞춰주기위한 옵셋
		) * g_cnWidth;

	// 모드 변경 시간 초기화.,.
	m_eRollingMode		= RM_ROLLING;
	m_uPrevTick			= ::GetTickCount();
	m_uRollingStartTime	= ::GetTickCount();

	m_nResultTID		= nTIDResult;

	{
		AgcmCharacter * pcsAgcmCharacter = ( AgcmCharacter * ) g_pEngine->GetModule( "AgcmCharacter" );
		AgpdCharacter * pcsSelfCharacter = pcsAgcmCharacter->GetSelfCharacter();
		if( pcsSelfCharacter ) 
			pcsSelfCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;

		this->m_pcsAgcmUIEventGacha->m_pcsAgcmEventGacha->SetGachaWindowOpen( TRUE );
	}

	return TRUE;
}

BOOL	AcUIGacha::LoadGachaSetting( const char * pFileName )
{
	if( m_pSplineData )
	{
		delete m_pSplineData;
		m_pSplineData = NULL;
	}

	vector< RwMatrix >	vecMatrix;

	// 파일 로딩
	{
		FILE	 *pFile = fopen( pFileName , "rt" );
		if(! pFile ) return FALSE;

		FLOAT	fXOffset = 0.0f;
		FLOAT	fXIncrement	= 1000.0f;
		INT32	nCount = 0;

		while( !feof( pFile ) )
		{
			RwMatrix	mat;
			RwMatrixSetIdentity( &mat );

			mat.pos.x	= fXOffset + fXIncrement * nCount;
			fscanf( pFile , "%f" , &mat.pos.y );

			vecMatrix.push_back( mat );
		}

		fclose( pFile );
	}

	if( vecMatrix.size() < 4 )
	{
		// 컨트롤 셋 부족.
		//for_each( vecMatrix.begin() , vecMatrix.end() , fncDeleter() );
		return FALSE; 
	}

	AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( s_nGachaRollingTime );

	pSpline->bNoLoop();
	pSpline->bReset();
	pSpline->bForward();
	pSpline->bSetActType(1);		//flight
	pSpline->bSetAccel(0);			//accel

	stSetSpline	stSet;
	stSet.closed		= false;
	stSet.ctrlMatrices	= &vecMatrix[ 0 ];
	stSet.numCtrl		= vecMatrix.size();

	pSpline->bInitSpline(stSet);

	m_pSplineData = pSpline;

	{
		// ResultLog
		FILE	*pFile = fopen( "RollResult.txt" , "wt" );
		for( UINT32 uTime = 0 ; uTime < s_nGachaRollingTime ; uTime += 100 )
		{
			RwMatrix	mat;
			m_pSplineData->bOnIdleOffset( mat , uTime );

			fprintf( pFile , "%d\t%f\n" , uTime , mat.pos.y ); 
		}
		fclose( pFile );
	}

	return TRUE;
}

void AcUIGacha::OnCloseUI()
{
	if( m_eRollingMode == RM_ROLLING )
	{
		AgcmCharacter * pcsAgcmCharacter = ( AgcmCharacter * ) g_pEngine->GetModule( "AgcmCharacter" );
		AgpdCharacter * pcsSelfCharacter = pcsAgcmCharacter->GetSelfCharacter();
		if( pcsSelfCharacter ) 
			pcsSelfCharacter->m_ulSpecialStatus &= ~AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;

		this->m_pcsAgcmUIEventGacha->m_pcsAgcmEventGacha->SetGachaWindowOpen( FALSE );
	}
}
