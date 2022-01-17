// AgcmMinimap.cpp: implementation of the AgcmMinimap class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmMinimap.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgcmUIOption.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FLOAT	__sRate	= 0.20f;

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

inline INT32 __GetRaiderPos( FLOAT fPos )
{
	return		( INT32 ) ( fPos / 
		( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH ) *
		( ACUIRAIDER_RANGE_SIZE << 1 ) );
}

inline FLOAT __GetMMPosINV( INT32 nPos )
{
	return		( FLOAT ) ( nPos ) / ( ACUIMINIMAP_TEXTURE_SIZE << 1 ) * ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH );
}

inline RwTexture * __LoadTextureMacro( AgcmMinimap * pThis , TCHAR * pFolder , TCHAR * pStr )
{
	RwTexture * pTexture;
	int nRetryCount = 2;
	do
	{
		pTexture = pThis->m_pcsAgcmResourceLoader->LoadTexture
					( pStr , NULL , NULL , NULL , -1 , pFolder );

		if( pTexture ) return pTexture;
	}
	while( nRetryCount-- );

	return NULL;
}

#define LOADTEXTUREMACRO( filename )			__LoadTextureMacro( this , AGCMMAP_MINIMAP_FOLDER , filename )


AgcmMinimap::AgcmMinimap():m_bShowAllCharacter( false )
{
	SetModuleName("AgcmMinimap");

	InitMinimapTextureBuffer	();

	m_pcsAgcmUIManager2		= NULL;
	m_pcsAgpmParty			= NULL;
	m_pcsAgcmResourceLoader	= NULL;
	m_pcsAgcmFont			= NULL;
	m_pcsApmMap				= NULL;
	m_pcsAgpmCharacter		= NULL;

	m_pDirectionCursor		= NULL;
	m_pWorldMap				= NULL;
	m_pMemberPos			= NULL;
	m_pPcPos				= NULL;
	m_pNpcNormal			= NULL;	//파티맴버 포지션.
	m_pNpcQuestNew	= NULL;	//파티맴버 포지션.
	m_pNpcQuestIncomplete	= NULL;	//파티맴버 포지션.

	m_pSiegeNormal			= NULL;
	m_pSiegeDestroied		= NULL;

	m_nWorldMap				= -1;

	ZeroMemory(&m_stSelfCharacterPos, sizeof(m_stSelfCharacterPos));
	
	m_pcsSelfCharacter		= NULL;
	m_pstUDPosition		= NULL;

	m_bIsOpenMiniMap	= TRUE;
	m_bIsOpenRaider		= TRUE;

	m_pclMinimapWindow1	= new AcUIMinimap	;
	m_pclMinimapWindow2	= new AcUIMinimap	;
	m_pclMinimapWindow3	= new AcUIMinimap	;
	m_pclWorldWindow	= new AcUIWorldmap	;
	m_pclRaiderWindow	= new AcUIRaider	;

	m_lEventMinimapForceClose	= 0;
	m_lEventMinimapYouCanOpen	= 0;

	m_bMurderer		= TRUE;
	m_bCriminal		= TRUE;
	m_bEnemyGuild		= TRUE;
}

AgcmMinimap::~AgcmMinimap()
{
	DEF_SAFEDELETE( m_pclMinimapWindow1	);
	DEF_SAFEDELETE( m_pclMinimapWindow2	);
	DEF_SAFEDELETE( m_pclMinimapWindow3	);
	DEF_SAFEDELETE( m_pclWorldWindow	);
	DEF_SAFEDELETE( m_pclRaiderWindow	);
}

// 미니맵 텍스쳐 관리.
void			AgcmMinimap::InitMinimapTextureBuffer	()
{
	for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
		m_pMinimapTextureBuffer[ i ].pTexture = NULL;
}

RwTexture	*	AgcmMinimap::GetMinimapDummyTexture		()
{
	RwTexture	* pTexture;

	// 미니맵 포멧은..
	char		strFilename[ 256 ];

	// 땜빵 이미지 하드코딩.
	wsprintf( strFilename , AGCMMAP_MINIMAP_FILE_FORMAT , 1722 , 0 + 'a' );

	pTexture	= LOADTEXTUREMACRO( strFilename );

	ASSERT( pTexture );

	return pTexture;
}

RwTexture	*	AgcmMinimap::GetMinimapTexture	( INT32 nDivisionIndex , INT32 nPartIndex )
{
	// 우선 있는지 검사..

	for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
	{
		if( m_pMinimapTextureBuffer[ i ].nIndex	== nDivisionIndex	&&
			m_pMinimapTextureBuffer[ i ].nPart	== nPartIndex		)
		{
			// 발견!
			m_pMinimapTextureBuffer[ i ].uLastAccess	= GetTickCount();
			return m_pMinimapTextureBuffer[ i ].pTexture;
		}
	}

	// 없으면 파일에서 읽어들임..

	RwTexture	* pTexture;

	// 미니맵 포멧은..
	char		strFilename[ 256 ];
	wsprintf( strFilename , AGCMMAP_MINIMAP_FILE_FORMAT , nDivisionIndex , nPartIndex + 'a' );

	pTexture	= LOADTEXTUREMACRO( strFilename );

	if( NULL == pTexture )
	{
		// 2005/01/12
		// 텍스쳐가 없는경우 땜빵 택스쳐를 사용한다.
		// 현재는 그냥 빈 이미지를 하드코딩하는데..
		// 후에 땜빵용 이미지를 따로 준비하도록 한다.
		
		pTexture	= GetMinimapDummyTexture();

		// 그래도 안나오면 어쩔수 없다 -_-;
	}

	{
		// 텍스쳐가 없어도 실행한다.

		INT32	nOldIndex = 0;

		// Find Older One
		for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
		{
			//if( NULL == m_pMinimapTextureBuffer[ i ].pTexture	)
			//{
			//	nOldIndex = i;
			//	break;
			//}

			if( m_pMinimapTextureBuffer[ i ].uLastAccess < m_pMinimapTextureBuffer[ nOldIndex ].uLastAccess)
			{
				nOldIndex = i;
			}
		}

		// 히죽.;
		if( m_pMinimapTextureBuffer[ nOldIndex ].pTexture )
		{
			// 텍스쳐 삭제.
			RwTextureDestroy( m_pMinimapTextureBuffer[ nOldIndex ].pTexture );
			m_pMinimapTextureBuffer[ nOldIndex ].pTexture	= NULL	;
			m_pMinimapTextureBuffer[ nOldIndex ].nIndex	= -1	;
		}
		else
		{
			// do nothing.
		}
		
		if( pTexture )
		{
			RwTextureSetFilterMode( pTexture , rwFILTERNEAREST );
			RwTextureSetAddressing ( pTexture , rwTEXTUREADDRESSCLAMP );
		}

		// 끝에거에 삽입후.
		// 제일 앞에 넣음.
		ASSERT( nOldIndex < MAX_MINIMAP_TEXTURE_BUFFER );
		
		m_pMinimapTextureBuffer[ nOldIndex ].pTexture		= pTexture		;
		m_pMinimapTextureBuffer[ nOldIndex ].nPart			= nPartIndex	;
		m_pMinimapTextureBuffer[ nOldIndex ].nIndex			= nDivisionIndex;
		m_pMinimapTextureBuffer[ nOldIndex ].uLastAccess	= GetTickCount();

		return m_pMinimapTextureBuffer[ nOldIndex ].pTexture;		
	}	
}

void			AgcmMinimap::FlushMinimapTexture	()
{
	for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
	{
		if( m_pMinimapTextureBuffer[ i ].pTexture )
		{
			RwTextureDestroy( m_pMinimapTextureBuffer[ i ].pTexture );
			m_pMinimapTextureBuffer[ i ].pTexture = NULL;
		}

		m_pMinimapTextureBuffer[ i ].nIndex	= -1;
	}
}

BOOL	AgcmMinimap::OnAddModule	()
{
	m_pcsAgcmUIManager2		= ( AgcmUIManager2		* )	GetModule( "AgcmUIManager2"		);
	m_pcsAgpmParty			= ( AgpmParty			* )	GetModule( "AgpmParty"			);
	m_pcsAgcmResourceLoader	= ( AgcmResourceLoader	* )	GetModule( "AgcmResourceLoader"	);
	m_pcsAgcmFont			= ( AgcmFont			* )	GetModule( "AgcmFont"			);
	m_pcsApmMap				= ( ApmMap				* )	GetModule( "ApmMap"				);
	m_pcsAgpmCharacter		= ( AgpmCharacter		* ) GetModule( "AgpmCharacter"		);
	

	ASSERT( NULL != m_pcsAgcmUIManager2		);
	ASSERT( NULL != m_pcsAgpmParty			);
	ASSERT( NULL != m_pcsAgcmResourceLoader	);
	ASSERT( NULL != m_pcsAgcmFont			);
	ASSERT( NULL != m_pcsApmMap				);
	ASSERT( NULL != m_pcsAgpmCharacter		);
	
	AgcmCharacter	*pAgcmCharacer = ( AgcmCharacter * ) GetModule( "AgcmCharacter" );
	ASSERT( NULL != pAgcmCharacer );


	if( pAgcmCharacer )
	{
		// 케릭터 이동 콜백 지정.
		VERIFY( pAgcmCharacer->SetCallbackSelfUpdatePosition( CBSelfCharacterPositionCallback , this ) );
		VERIFY( pAgcmCharacer->SetCallbackSetSelfCharacter	( CBSetSelfCharacter, this ) );

		// 리젼 변화를 체크.
		VERIFY( pAgcmCharacer->SetCallbackSelfRegionChange	( CBBindingRegionChange , this ) );
	}
	else
	{
		return FALSE;
	}

	m_pclMinimapWindow1->SetMinimapModule( this );
	m_pclMinimapWindow2->SetMinimapModule( this );
	m_pclMinimapWindow3->SetMinimapModule( this );
	m_pclRaiderWindow->SetMinimapModule( this );

	m_pclMinimapWindow1->SetCharacterModule( m_pcsAgpmCharacter );
	m_pclMinimapWindow2->SetCharacterModule( m_pcsAgpmCharacter );
	m_pclMinimapWindow3->SetCharacterModule( m_pcsAgpmCharacter );
	m_pclRaiderWindow->SetCharacterModule( m_pcsAgpmCharacter );

	if (m_pcsAgcmUIManager2)
	{
		// 컨트롤 1
		if (!m_pcsAgcmUIManager2->AddCustomControl("MiniMap1", m_pclMinimapWindow1))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "MiniMap_Pos1", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("MiniMap_Pos1", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;

		// 컨트롤 2
		if (!m_pcsAgcmUIManager2->AddCustomControl("MiniMap2", m_pclMinimapWindow2))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "MiniMap_Pos2", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("MiniMap_Pos2", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;

		// 컨트롤 3
		if (!m_pcsAgcmUIManager2->AddCustomControl("MiniMap3", m_pclMinimapWindow3))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "MiniMap_Pos3", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("MiniMap_Pos3", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "MiniMap_ToggleUI", CBToggleMiniMapUI, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "WorldMap_ToggleUI", CBToggleWorldMapUI, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Raider_ToggleUI", CBToggleRaiderUI, 0))
			return FALSE;

		// Raider 컨트롤
		if( !m_pcsAgcmUIManager2->AddCustomControl( "Raider" , m_pclRaiderWindow ) )
			return FALSE;

		if( !m_pcsAgcmUIManager2->AddDisplay(this, "Raider_Pos" , 0 , CBDisplayPos , AGCDUI_USERDATA_TYPE_POS ))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData( "Raider_Pos" , &m_stSelfCharacterPos , sizeof(AuPOS) , 1 , AGCDUI_USERDATA_TYPE_POS );
		if( !m_pstUDPosition )
			return FALSE;

		
	}

	m_pclWorldWindow->SetMinimapModule( this );
	m_pclWorldWindow->SetCharacterModule( m_pcsAgpmCharacter );

	if (m_pcsAgcmUIManager2)
	{
		if (!m_pcsAgcmUIManager2->AddCustomControl("WorldMap", m_pclWorldWindow))
			return FALSE;

		/*
		if (!m_pcsAgcmUIManager2->AddDisplay(this, "WorldMap_Pos", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("World_Pos", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;
		*/
	}


	if( m_pcsAgcmUIManager2 )
	{
//		m_lEventPlusButton = m_pcsAgcmUIManager2->AddEvent("MiniMap_PlusButton");
//		if (m_lEventPlusButton < 0)
//			return FALSE;
//		m_lEventMinusButton = m_pcsAgcmUIManager2->AddEvent("MiniMap_MinusButton");
//		if (m_lEventMinusButton < 0)
//			return FALSE;

		m_lEventOpenMiniMapUI = m_pcsAgcmUIManager2->AddEvent("MiniMap_OpenUI");
		if (m_lEventOpenMiniMapUI < 0)
			return FALSE;
		m_lEventCloseMiniMapUI = m_pcsAgcmUIManager2->AddEvent("MiniMap_CloseUI");
		if (m_lEventCloseMiniMapUI < 0)
			return FALSE;

		m_lEventOpenRaiderUI	=	m_pcsAgcmUIManager2->AddEvent( "Raider_OpenUI" );
		if( m_lEventOpenRaiderUI < 0 )
			return FALSE;

		m_lEventCloseRaiderUI	=	m_pcsAgcmUIManager2->AddEvent( "Raider_CloseUI" );
		if( m_lEventCloseRaiderUI < 0 )
			return FALSE;

//		if (!m_pcsAgcmUIManager2->AddFunction(this, "MiniMap_SetPlusButton", CBPlusButton, 0))
//			return FALSE;
//		if (!m_pcsAgcmUIManager2->AddFunction(this, "MiniMap_SetMinusButton", CBMinusButton, 0))
//			return FALSE;

		m_lEventMinimapForceClose = m_pcsAgcmUIManager2->AddEvent("MiniMap_ForcedClose");
		if (m_lEventMinimapForceClose < 0)
			return FALSE;
		m_lEventMinimapYouCanOpen = m_pcsAgcmUIManager2->AddEvent("MiniMap_YouCanOpenUI");
		if (m_lEventMinimapYouCanOpen < 0)
			return FALSE;

		m_lEventOpenWorldMapUI = m_pcsAgcmUIManager2->AddEvent("WorldMap_OpenUI");
		if (m_lEventOpenWorldMapUI < 0)
			return FALSE;

		// 월드맵 아이템이 없어서볼수없다.
		m_lEventWorldMapNoItem = m_pcsAgcmUIManager2->AddEvent("WorldMap_NoItem");
		if (m_lEventWorldMapNoItem < 0)
			return FALSE;

		m_lEventCloseWorldMapUI = m_pcsAgcmUIManager2->AddEvent("WorldMap_CloseUI");
		if (m_lEventCloseWorldMapUI < 0)
			return FALSE;

		if( m_pclWorldWindow )
		{
			m_pclWorldWindow->OnAddEvent( m_pcsAgcmUIManager2 );
			m_pclWorldWindow->OnAddFunction( m_pcsAgcmUIManager2 );
			m_pclWorldWindow->OnAddDisplay( m_pcsAgcmUIManager2 );
			m_pclWorldWindow->OnAddUserData( m_pcsAgcmUIManager2 );
		}
	}

	{
		AgcmItem	* pAgcmItem = static_cast< AgcmItem * > ( GetModule( "AgcmItem" ) );
		ASSERT( NULL != pAgcmItem );

		if( pAgcmItem )
		{
			VERIFY( pAgcmItem->SetCallbackUseMapItem( CBUseMapItem , this ) );
		}
		else
		{
			return FALSE;
		}
	}

	// 마고자 (2005-07-21 오후 12:19:29) : 
	// 월드맵 설정 읽어들임.
	{
		ApmMap	* pcsApmMap = m_pcsApmMap;
		// 월드맵 인포
		#ifdef USE_MFC
		if( pcsApmMap->LoadWorldMap( "Ini\\" WORLDMAPTEMPLATE , FALSE ) )
		#else
		if( pcsApmMap->LoadWorldMap( "Ini\\" WORLDMAPTEMPLATE , TRUE ) )
		#endif // USE_MFC
		{
			// 지도 정보 추가.
			ApmMap::WorldMap	* pWorldMap;
			for( int i = 0 ; i < ( int ) pcsApmMap->m_arrayWorldMap.size() ; i ++ )
			{
				pWorldMap = &pcsApmMap->m_arrayWorldMap[ i ];

				m_pclWorldWindow->SetWorldMapInfo( pWorldMap );
			}

			/// 퍼블릭에건 날림.
			pcsApmMap->RemoveAllWorldMap();

			m_pclWorldWindow->m_bThereIsNoWorldMapTemplate = FALSE;
		}
		else
		{
			// 없으면 어쩌지..
			ASSERT( "월드맵 템플릿이 없습니다." );
			// 안나오는거지 뭐 ~ >_<;;;

			// 원래거라도 나오게 한다.
			// 하드코디잉~
			m_pclWorldWindow->SetWorldMapInfo( NULL ); // 고정

			m_pclWorldWindow->m_bThereIsNoWorldMapTemplate = TRUE;
		}
	}
	/*
	// 디버그용..
	AuPOS pos;
	pos.x	= -412041.0f;
	pos.z	= 148215.0f;
	pos.y	= 0.0f;
	AddPoint( 0 , "에헤헤" , &pos );

	pos.x	= -396807.0f;
	pos.z	= 154223.0f;
	pos.y	= 0.0f;
	AddPoint( 0 , "크르릉" , &pos );
	*/

	return TRUE;
}

BOOL	AgcmMinimap::OnInit()
{
	m_pcsAgpmWantedCriminal	= ( AgpmWantedCriminal	* ) GetModule( "AgpmWantedCriminal" );
	m_pcsAgpmPVP			= ( AgpmPvP	*	)			GetModule( "AgpmPvP" );
	m_pcsAgcmCharacter		= ( AgcmCharacter* )		GetModule( "AgcmCharacter" );
	m_pcsAgpmGuild			= ( AgpmGuild*		)		GetModule( "AgpmGuild" );
	m_pcsAgpmBattleGround	= ( AgpmBattleGround* )		GetModule( "AgpmBattleGround" );

	return TRUE;
}

BOOL	AgcmMinimap::OnDestroy		()
{
	FlushMinimapTexture();

	if( m_pDirectionCursor	)
	{
		RwTextureDestroy( m_pDirectionCursor	);
		m_pDirectionCursor = NULL;
	}

	if( m_pWorldMap			)
	{
		RwTextureDestroy( m_pWorldMap			);
		m_pWorldMap = NULL;
	}

	if( m_pMemberPos		)
	{
		RwTextureDestroy( m_pMemberPos			);
		m_pMemberPos = NULL;
	}

	if( m_pPcPos			)
	{
		RwTextureDestroy( m_pPcPos				);
		m_pPcPos = NULL;
	}

	if( m_pNpcNormal		)
	{
		RwTextureDestroy( m_pNpcNormal			);
		m_pNpcNormal = NULL;
	}

	if( m_pNpcQuestNew		)
	{
		RwTextureDestroy( m_pNpcQuestNew		);
		m_pNpcQuestNew = NULL;
	}

	if( m_pNpcQuestIncomplete)
	{
		RwTextureDestroy( m_pNpcQuestIncomplete	);
		m_pNpcQuestIncomplete = NULL;
	}

	if( m_pSiegeNormal		)
	{
		RwTextureDestroy( m_pSiegeNormal		);
		m_pSiegeNormal = NULL;
	}

	if( m_pSiegeDestroied	)
	{
		RwTextureDestroy( m_pSiegeDestroied		);
		m_pSiegeDestroied = NULL;
	}

	return TRUE;
}

BOOL	AgcmMinimap::CBSelfCharacterPositionCallback(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmMinimap::CBSelfCharacterPositionCallback");

	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
	AgpdCharacter	*	pcsAgpdCharacter	= (AgpdCharacter *) pData					;
	//AgcdCharacter	*	pcsAgcdCharacter	= pThis->GetCharacterData(pcsAgpdCharacter)	;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsAgpdCharacter );
	if( NULL == pThis ) return TRUE;

	if (pcsAgpdCharacter)
	{
		pThis->m_stSelfCharacterPos	= pcsAgpdCharacter->m_stPos;
	}
	// 이동 정보 업데이트 .
	pThis->m_pstUDPosition->m_stUserData.m_pvData	= &pThis->m_stSelfCharacterPos;

	if( pThis->m_pcsAgcmUIManager2 )
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDPosition)	;
	}

	pThis->m_pcsSelfCharacter	= pcsAgpdCharacter;
	//pThis->m_pclMinimapWindow.ProcessPositionChange( pcsAgpdCharacter->m_stPos.x , pcsAgpdCharacter->m_stPos.z );

	pThis->m_pclMinimapWindow1->PositionUpdate();
	pThis->m_pclMinimapWindow2->PositionUpdate();
	pThis->m_pclMinimapWindow3->PositionUpdate();

	pThis->m_pclRaiderWindow->RaiderPositionUpdate();

	return TRUE;
}

BOOL AgcmMinimap::CBBindingRegionChange(PVOID	pData,PVOID	pClass,PVOID	pCustData)
{
	AgcmMinimap*	pThis		= (AgcmMinimap*) pClass;
	AgpdCharacter*	pdCharacter = (AgpdCharacter*)	pData;
	INT16 nPrevRegionIndex = pCustData ? *(INT16*)pCustData : -1;	// NULL 로 올때는 ADD_CHARACTER_TO_MAP 에서 불린것임.

	// 템플릿 얻어내고..
	ApmMap::RegionTemplate*	pTemplate		= pThis->m_pcsApmMap->GetTemplate( pdCharacter->m_nBindingRegionIndex );
	ApmMap::RegionTemplate*	pTemplatePrev	= pThis->m_pcsApmMap->GetTemplate( nPrevRegionIndex );

	if( pTemplate && pTemplatePrev )
	{
		if( pTemplate->ti.stType.bDisableMinimap )
		{
			// 미니맵 사용 안됌.

			if(pThis->m_bIsOpenMiniMap)
			{
				// 떠있으니까 끄고..
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseMiniMapUI);
				pThis->m_bIsOpenMiniMap	= FALSE;

				// 강제 닫기 메시지 표시.
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinimapForceClose);
			}
		}
		else
		{
			// 미니맵 사용이 가능한곳..
			if( pThis->m_bIsOpenMiniMap )
			{
				// do nothing.
			}
			else
			{
				if( pTemplatePrev->ti.stType.bDisableMinimap )
				{
					// 원래는 안됐는데
					// 가능한 곳으로 나옴.

					// 열수 있음을 알려줌.
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinimapYouCanOpen);
				}
			}
		}

		if( pTemplate->nWorldMapIndex != pTemplatePrev->nWorldMapIndex )
		{
			// 월드맵 끄기
			if( pThis->m_pclWorldWindow->m_bIsOpenWorldMap )
			{
				// 닫기..
				//pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
				//pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );

				if( pThis->m_pclWorldWindow->SetWorldMapIndex( pTemplate->nWorldMapIndex ) )
				{
					AgpmItem	* pAgpmItem = static_cast< AgpmItem * > ( pThis->GetModule( "AgpmItem" ) );

					ASSERT( NULL != pAgpmItem );

					// do nothing..

					AcUIWorldmap::stWorldMap * pWorldmap = pThis->m_pclWorldWindow->GetCurrentWorldMap();
					ASSERT( NULL != pWorldmap );

					// 아이템 있는지 검사..
					if( pWorldmap->nMapItemID == -1 || 				
						pAgpmItem->GetInventoryItemByTID( pThis->m_pcsSelfCharacter , pWorldmap->nMapItemID ) )
					{
						// do nothing 
					}
					else
					{
						// 아이템이 없다.
						// 마고자 (2005-07-21 오후 5:04:44) : 
						// 여기서 return FALSE를 하더라도 UI는 닫히지 않는다.

						pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
						pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );
						pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
					}
				}
				else
				{
					// 매칭돼는 맵이 없다.. 나오면 안돼는 에러.
					pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
					pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );
					pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
				}
			}
		}
	}

    return	TRUE;
}

BOOL	AgcmMinimap::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMinimap		*	pThis				= (AgcmMinimap *)	pClass					;
	AgpdCharacter	*	pcsCharacter		= (AgpdCharacter *)	pData					;
	
	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsCharacter );
	if( NULL == pThis ) return TRUE;

	if (pcsCharacter)
	{
		pThis->m_stSelfCharacterPos	= pcsCharacter->m_stPos;
	}

	if( pThis->m_pstUDPosition )
	{
		pThis->m_pstUDPosition->m_stUserData.m_pvData	= &pThis->m_stSelfCharacterPos;
	}

	if( pThis->m_pcsAgcmUIManager2 )
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDPosition)	;
	}

	pThis->m_pcsSelfCharacter	= pcsCharacter;

	return TRUE;
}

RwTexture	*	AgcmMinimap::GetCursorTexture			()
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;

	if( m_pDirectionCursor ) return m_pDirectionCursor;
	{ 
		m_pDirectionCursor	= LOADTEXTUREMACRO( AGCMMAP_MINIMAP_FILE_CURSOR );

		// 마고자 (2004-05-26 오후 4:07:14) : 커서는 리니어 안씀.
		//RwTextureSetFilterMode( m_pDirectionCursor , rwFILTERNEAREST );
		if( m_pDirectionCursor )
			RwTextureSetAddressing ( m_pDirectionCursor , rwTEXTUREADDRESSCLAMP );
		
		return m_pDirectionCursor;
	}
}

RwTexture	*	AgcmMinimap::GetWorldMapTexture			( int nIndex )
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;

	if( nIndex < 0 ) return NULL;

	// 마고자 (2004-07-18 오후 9:27:50) : 이미 로딩되어 있으면..
	if( m_nWorldMap == nIndex && m_pWorldMap ) return m_pWorldMap;

	if( m_pWorldMap )
	{
		// 마고자 (2004-07-18 오후 9:29:36) : 청소~
		RwTextureDestroy( m_pWorldMap );
		m_pWorldMap	= NULL;
	}

	char	str[ 1024 ];
	wsprintf( str , AGCMMAP_WORLDMAP_TEST , nIndex );

	AgcmUIOption * pcsAgcmUIOption = ( AgcmUIOption * ) this->GetModule( "AgcmUIOption" );
	ASSERT( NULL != pcsAgcmUIOption );

	// m_pWorldMap	= LOADTEXTUREMACRO( str );

	UINT32 uUIType = 0;
	if( pcsAgcmUIOption )
	{
		uUIType = pcsAgcmUIOption->m_ulUISkinType;
	}

	const int nStringMax = 256;
	char	pFolder[ nStringMax ];


	switch( uUIType )
	{
	default:
	case 0:
		{
			strncpy( pFolder , AGCMMAP_WORLDMAP_FOLDER_DEFAULT , nStringMax );
		}
		break;

	/*
		// 1이상의 경우에는 하드코드를 풀어주어야함.
	case 1:
		{
			wsprintf( pFolder , AGCMMAP_WORLDMAP_FOLDER , uUIType );
		}
		break;
	*/
	}

	m_pWorldMap	= __LoadTextureMacro( this , pFolder , str );

	if( m_pWorldMap )
	{
		RwTextureSetFilterMode( m_pWorldMap , rwFILTERNEAREST );
		RwTextureSetAddressing ( m_pWorldMap , rwTEXTUREADDRESSCLAMP );
		m_nWorldMap = nIndex;
	}
	else
	{
		TRACE( "AgcmMinimap::GetWorldMapTexture = '%s' 파일이 존재하지 않아요\n" , str );
	}

	return m_pWorldMap;
}

RwTexture	*	AgcmMinimap::GetMemberPosTexture			()
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;

	if( m_pMemberPos ) return m_pMemberPos;
	{
		m_pMemberPos	= LOADTEXTUREMACRO( AGCMMAP_MEMBERPOSITION );

		// RwTextureSetFilterMode( m_pMemberPos , rwFILTERNEAREST );
		if( m_pMemberPos )
			RwTextureSetAddressing ( m_pMemberPos , rwTEXTUREADDRESSCLAMP );
		
		return m_pMemberPos;
	}
}

#define TEXTURE_LOAD_MODULE( TEXTURE , FILE__NAME )									\
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;						\
	if( TEXTURE ) return TEXTURE;													\
	{																				\
		TEXTURE	=																	\
			LOADTEXTUREMACRO( FILE__NAME	);										\
		if( TEXTURE )																\
			RwTextureSetAddressing ( TEXTURE , rwTEXTUREADDRESSCLAMP );				\
		return TEXTURE;																\
	}

RwTexture	*	AgcmMinimap::GetPcPosTexture				()
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;
	
	if( m_pPcPos ) return m_pPcPos;
	{
		m_pPcPos	= LOADTEXTUREMACRO( AGCMMAP_PCPOSITION );

		// RwTextureSetFilterMode( m_pPcPos , rwFILTERNEAREST );
		if( m_pPcPos )
			RwTextureSetAddressing ( m_pPcPos , rwTEXTUREADDRESSCLAMP );

		return m_pPcPos;
	}
}

RwTexture	*		AgcmMinimap::GetNpcNormalTexture			()
{
	TEXTURE_LOAD_MODULE( m_pNpcNormal , AGCMMAP_NPC_NORMAL )
}

RwTexture	*		AgcmMinimap::GetNpcQuestNewTexture	()
{
	TEXTURE_LOAD_MODULE( m_pNpcQuestNew , AGCMMAP_NPC_QUESTNEW )
}
RwTexture	*		AgcmMinimap::GetNpcQuestIncompleteTexture	()
{
	TEXTURE_LOAD_MODULE( m_pNpcQuestIncomplete , AGCMMAP_NPC_QUESTINCOMPLETE )
}

RwTexture	*		AgcmMinimap::GetTextureSiegeNormal		()
{
	TEXTURE_LOAD_MODULE( m_pSiegeNormal , AGCMMAP_SIEGE_NORMAL )
}
RwTexture	*		AgcmMinimap::GetTextureSiegeDestroied	()
{
	TEXTURE_LOAD_MODULE( m_pSiegeDestroied , AGCMMAP_SIEGE_DESTROIED )
}

BOOL	AgcmMinimap::CBDisplayPos(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
	AuPOS *				pos					= (AuPOS *) pData							;

	if (pos)
		sprintf(szDisplay, "%d,%d", (INT32) pos->x, (INT32) pos->z);
	else
		szDisplay[0] = '\0';

	return TRUE;
}

//BOOL	AgcmMinimap::CBPlusButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
//
//	if( NULL == pThis ) return FALSE;
//	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPlusButton);
//}
//
//BOOL	AgcmMinimap::CBMinusButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
//
//	if( NULL == pThis ) return FALSE;
//	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinusButton);
//}

BOOL	AgcmMinimap::CBToggleRaiderUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{

	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;

	if( NULL == pThis ) return FALSE;
	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
	if( NULL == pThis->m_pcsSelfCharacter ) return FALSE;

	if (pThis->m_bIsOpenRaider)
	{
		// 떠있으니까 끄고..
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseRaiderUI);
		pThis->m_bIsOpenRaider	= FALSE;
	}
	else
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenRaiderUI);
		pThis->m_bIsOpenRaider	= TRUE;
	}

	return TRUE;
}


BOOL	AgcmMinimap::CBToggleMiniMapUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	// 미니맵 토글 버튼을 누를때마다 호출됨
	// 즉 . 'm'키를 누르면 여기로 불림..
	// 여기서 UI를 켜고 끄고를 처리함.

	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;

	if( NULL == pThis ) return FALSE;
	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
	if( NULL == pThis->m_pcsSelfCharacter ) return FALSE;
	
	if (pThis->m_bIsOpenMiniMap)
	{
		// 떠있으니까 끄고..
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseMiniMapUI);
		pThis->m_bIsOpenMiniMap	= FALSE;
	}
	else
	{
		// 템플릿 얻어내고..
		ApmMap::RegionTemplate*	pTemplate = pThis->m_pcsApmMap->GetTemplate( pThis->m_pcsSelfCharacter->m_nBindingRegionIndex );

		if( pTemplate )
		{
			if( pTemplate->ti.stType.bDisableMinimap )
			{
				// 미니맵 사용 안됌.

				// do nothing..

				// 미니맵 열수없음 메시지 표시.
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinimapForceClose);
			}
			else
			{
				// 꺼져있으니까 켜고..
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenMiniMapUI);
				pThis->m_bIsOpenMiniMap	= TRUE;
			}
		}
	}

	return TRUE;
}

BOOL	AgcmMinimap::CBToggleWorldMapUI				(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;

	if( pThis->m_pclWorldWindow->m_bIsOpenWorldMap )
	{
		// 닫기..
		pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );
		return TRUE;
	}
	else
	{
		// 열기인데...
		// 여기서 아이템 체크..
		if( pThis->m_pclWorldWindow->SetWorldMap( &pThis->m_stSelfCharacterPos ) )
		{
			AgpmItem	* pAgpmItem = static_cast< AgpmItem * > ( pThis->GetModule( "AgpmItem" ) );

			ASSERT( NULL != pAgpmItem );

			// do nothing..

			AcUIWorldmap::stWorldMap * pWorldmap = pThis->m_pclWorldWindow->GetCurrentWorldMap();
			ASSERT( NULL != pWorldmap );

			// 아이템 있는지 검사..
			if( pWorldmap->nMapItemID == -1 || 				
				pAgpmItem->GetInventoryItemByTID( pThis->m_pcsSelfCharacter , pWorldmap->nMapItemID ) )
			{
				// 아이템이 있으므로 UI표시
				pThis->m_pclWorldWindow->m_bIsOpenWorldMap = TRUE;
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenWorldMapUI );
				pThis->m_pclWorldWindow->OnGetControls( pThis->m_pcsAgcmUIManager2 );
				pThis->m_pclWorldWindow->OnUpdateWorldMapAlpha();
				return TRUE;
			}
			else
			{
				// 아이템이 없다.
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
				return FALSE;
			}
		}
		else
		{
			// 매칭돼는 맵이 없다.. 나오면 안돼는 에러.
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
			return FALSE;
		}
	}
	}

INT32		AgcmMinimap::GetDisabledIndex()
{
	MPInfo *	pArray = GetMPArray();
	INT32		nCount = GetMPCount();

	for( int i = 0 ; i < nCount ; i ++ )
	{
		if( pArray[ i ].bDisabled ) return i ;
	}

	return (-1); // 없어요..
}

INT32 				AgcmMinimap::GetPoint		( char * strText )
{
	if( !strText )
		return (-1);

	// 이미 있는 놈인지 검사
	AgcmMinimap::MPInfo *	pArray = GetMPArray();

	for( int i = 0 ; i < GetMPCount() ; i ++ )
	{
		if( pArray[ i ].strText &&
			strcmp(strText, pArray[ i ].strText) == 0 )
		{
			return i;
		}
	}

	return (-1);
}

INT32				AgcmMinimap::AddPoint		( INT32 nType , char * strText , AuPOS * pPos , INT32 nControlGroupID )
{
	// 최대 갯수 체크..

	MPInfo	newInfo;

	newInfo.nType	= nType;
	newInfo.pos		= *pPos;
	strncpy( newInfo.strText , strText , AGCMMAP_MINIMAP_POINT_TEXT_LENGTH );
	newInfo.nControlGroupID	= nControlGroupID;

	// 이미 있는 놈인지 검사
	// 이름이 없는것도 있으므로 검사안함..
	/*
	INT32	nExistIndex	= GetPoint(strText);
	if( nExistIndex != -1 )
	{
		MPInfo * pExistInfo = GetMPInfo( nExistIndex );
		*pExistInfo = newInfo;
		return nExistIndex;
	}
	*/

	// 일단 빈거 있는지 검사..

	INT32	nDisabled = GetDisabledIndex();
	if( nDisabled != -1 )
	{
		MPInfo * pInfo = GetMPInfo( nDisabled );
		*pInfo = newInfo;
		return nDisabled;
	}
	else
	{
        m_arrayMPInfo.push_back( newInfo );
		return GetMPCount() - 1;
	}
}

BOOL				AgcmMinimap::RemovePoint		( INT32 nIndex )
{
	ASSERT( nIndex < GetMPCount() );
	if( nIndex >= GetMPCount() ) return FALSE;

	MPInfo * pInfo = GetMPInfo( nIndex );
	pInfo->bDisabled = TRUE;
	return TRUE;
}

INT32				AgcmMinimap::RemovePointGroup		( INT32 nControlGroupID )
{
	// 한개에서 여러개로 등록됀 경우 , 등록자의 ID를 등록해서
	// 한꺼번에 삭제하게한다.
	if( nControlGroupID == -1 ) return FALSE; // 머시라!

	int nCount = 0;

	MPInfo * pArray = GetMPArray();
	for ( int i = 0 ; i < GetMPCount() ; i ++ )
	{
		if( pArray[ i ].nControlGroupID == nControlGroupID )
		{
			pArray[ i ].bDisabled = TRUE;
			nCount ++;
		}
	}

	return nCount;
}

AgcmMinimap::MPInfo *	AgcmMinimap::GetPointInfo		( INT32 nIndex )
{
	// 쪼금 프로텍션 추가..
	// 잘못된거 들어올 수도 있으니 Validation Check만 (Parn)
	if(  nIndex < 0 || nIndex >= GetMPCount() ) return NULL;

	MPInfo * pInfo = GetMPInfo( nIndex );
	if( pInfo->bDisabled ) return NULL;
	else return pInfo;
}

BOOL				AgcmMinimap::UpdatePointPosition( INT32 nIndex , AuPOS * pPos )
{
	MPInfo * pInfo = GetPointInfo( nIndex );
	if( pInfo )
	{
		pInfo->pos = * pPos;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL	AgcmMinimap::CBUseMapItem(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmMinimap	* pThis		= static_cast< AgcmMinimap * > ( pClass );
	AgpdItem	* pcsItem	= static_cast< AgpdItem * > ( pData );

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsItem );
	ASSERT( NULL != pcsItem->m_pcsItemTemplate );
	if( NULL == pThis ) return FALSE;
	if( NULL == pcsItem ) return FALSE;
	if( NULL == pcsItem->m_pcsItemTemplate ) return FALSE;

	{
		// 현재 맵 설정함..
		// 아이템에 연결된 맵을 확인함.
		if( pThis->m_pclWorldWindow->SetWorldMapByItem( pcsItem->m_pcsItemTemplate->m_lID ) )
		{
			// 월드맵 UI Open
			if( pThis->m_pclWorldWindow->m_bIsOpenWorldMap )
			{
				// do nothing
			}
			else
			{
				// 안떠 있을경우만 연다.
				pThis->m_pclWorldWindow->m_bIsOpenWorldMap = TRUE;
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenWorldMapUI );		
			}
		}
		else
		{
			// 매칭돼는 아이템이 없다!?
			// 나오면 안돼는 에러
			ASSERT( !"매칭돼는 맵이 없어!?" );
		}
	}

	return TRUE;
}

LuaGlue	LG_TDC( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	bUse		= ( INT32 ) pLua->GetNumberArgument( 1 , 1 );

	AgcmMinimap * pAgcmMinimap = ( AgcmMinimap * ) g_pEngine->GetModule( "AgcmMinimap" );

	pAgcmMinimap->m_bShowAllCharacter = bUse ? true : false;

	LUA_RETURN( TRUE );
}


void AgcmMinimap::OnLuaInitialize( AuLua * pLua )
{
	luaDef MVLuaGlue[] = 
	{
		{"TDC"		,	LG_TDC	},
		{NULL				,	NULL			},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}
