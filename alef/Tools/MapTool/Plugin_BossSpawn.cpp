// Plugin_BossSpawn.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MainFrm.h"
#include "MainWindow.h"
#include "Plugin_BossSpawn.h"
#include "ApModuleStream.h"
#include "PositionDlg.h"

void __DrawWireFrame( RpClump * pClump );
void _GetIntersectPos( AuPOS *pPos , RwV3d *pNormal , RwV3d *pStart , RwV3d *pEnd , RwV3d *pCollisionPoint );

CPlugin_BossSpawn * CPlugin_BossSpawn::m_spThis = NULL;
extern MainWindow			g_MainWindow	;
static const char g_strINIObjectBossInfo	[]	= "BossCID"		;


ApdObject * CPlugin_BossSpawn::Spawn::GetObject		()
{
	return g_pcsApmObject->GetObject( this->nOID );
}

AgpdCharacter * CPlugin_BossSpawn::Spawn::GetCharacter	()
{
	return g_pcsAgpmCharacter->GetCharacter( this->nCID );
}

// CPlugin_BossSpawn

IMPLEMENT_DYNAMIC(CPlugin_BossSpawn, CWnd)
CPlugin_BossSpawn::CPlugin_BossSpawn():
	m_bLButtonDown( FALSE )
{
	SetModuleName("Plugin_BossSpawn");

	m_strShortName = "B.Spawn";
	m_nCharacterIDCounter	= BOSSMOB_OFFSET;

	m_pSelectedClump		= NULL	;

	m_PrevMouseStatus.pos.x	= 0;
	m_PrevMouseStatus.pos.y	= 0;

	m_nLastReadSpawnCharacterID	= 0;

	m_stOption.bExcludeWhenExport = true;

	CPlugin_BossSpawn::m_spThis = this;
}

CPlugin_BossSpawn::~CPlugin_BossSpawn()
{
}


BEGIN_MESSAGE_MAP(CPlugin_BossSpawn, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CPlugin_BossSpawn 메시지 처리기입니다.


void CPlugin_BossSpawn::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 255 ) );
}

BOOL CPlugin_BossSpawn::OnSelectedPlugin		()
{
	m_pCurrentCharacter	= NULL;
	return TRUE;
}

BOOL CPlugin_BossSpawn::OnDeSelectedPlugin		()
{
	m_pCurrentCharacter	= NULL;
	return TRUE;
}

BOOL CPlugin_BossSpawn::OnSelectObject			( RpClump * pClump )
{
	TRACE( "Select Object\n" );
	INT32	nID;
	switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID ) )
	{
	case ACUOBJECT_TYPE_CHARACTER:
		{
			// ID의 범위를 확인..
			if( ISBOSSID( nID ) )
			{
				m_pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				if( NULL == m_pCurrentCharacter )
				{
					return FALSE;
				}
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
BOOL CPlugin_BossSpawn::OnLButtonDownGeometry	( RwV3d * pPos )
{
	AgpdCharacter * pCharacter;

	int	nSelect = m_wndCombo.GetCurSel();
	if( nSelect == CB_ERR )
	{
		MD_SetErrorMessage( "No item is selected-_-\n" );
		return TRUE;
	}

	CString	str;
	m_wndCombo.GetLBText( nSelect , str );

	int	nTemplateID = 1;	// 템플릿을 얻어냄 

	nTemplateID = atoi( ( LPCTSTR ) str );

	// 템플릿 벨리드체크..
	if( nTemplateID < 0 )
	{
		MD_SetErrorMessage( "케릭터 템플릿 아이디 이상..\n" );
		return FALSE;
	}

	// 이렇게 하면 안되지... -_-;
	// m_nCharacterIDCounter NPC파일을 읽으면 다음부턴 어떻게 찍으라고... -_-; By Parn
	char	strNPCName[ 256 ];

	m_nCharacterIDCounter = rand() % 10000 + BOSSMOB_OFFSET;
	wsprintf( strNPCName , "BOSS_%04d" , m_nCharacterIDCounter % BOSSMOB_OFFSET );

	g_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ) );		// modify by gemani
	pCharacter = g_pcsAgpmCharacter->AddCharacter( m_nCharacterIDCounter++ , nTemplateID , strNPCName );

	if( NULL == pCharacter )
	{
		DisplayMessage( RGB( 232 , 121 , 25 ) , "해당 케릭터에 대한 템플릿이 없어요~" );
		return TRUE;
	}

	// 툴 작업용으로 임의로 삽입..
	AgpdCharacterTemplate * pTemplate = pCharacter->m_pcsCharacterTemplate;
	g_pcsAgpmCharacter->SetTypeTrap( pTemplate );

	// 해당 위치에 템플릿 삽입..

	AuPOS	pos;
	pos.x	= pPos->x;
	pos.y	= 0.0f;
	pos.z	= pPos->z;

	ApWorldSector * pWorldSector;
	pWorldSector = g_pcsApmMap->GetSector( pos.x , pos.z );

	if( pWorldSector )
	{
		pos.y = AGCMMAP_THIS->GetHeight_Lowlevel( pos.x , pos.z , SECTOR_MAX_HEIGHT);
		//g_pcsAgcmCamera.UpdateCamera();
		pCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD	;
		pCharacter->m_stPos				= pos							;

		g_pcsAgpmCharacter->UpdateInit( pCharacter );

		g_pcsAgpmCharacter->UpdatePosition	( pCharacter , &pos					); 
		//g_pcsAgcmCharacter->TurnSelfCharacter( 0.0f , 0 );//(float) 0 * 180.0f / 3.1415927f	);

		// 월드에 Add 돼었으므로..
		// 스폰 오브젝트를 같은 위치에 삽입.
		// 그리고 이벤트 등록.
		{
			AddSpawnObject( pCharacter );
		}
	}
	else
	{
		// 실패했으므로 삭제..

		// 어떻게? -_-;;;
	}

	SetSaveData();

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnLButtonUpGeometry	( RwV3d * pPos )
{
	m_pCurrentCharacter	= NULL;
	return TRUE;
}

BOOL CPlugin_BossSpawn::OnMouseMoveGeometry	( RwV3d * pPos )
{
	if( m_pCurrentCharacter && pPos && m_bLButtonDown )
	{
		// 케릭터를 pPos 위치로 이동시킴..
		AuPOS	pos;
		pos.x	= pPos->x;
		pos.y	= AGCMMAP_THIS->GetHeight_Lowlevel( pPos->x , pPos->z , SECTOR_MAX_HEIGHT);
		pos.z	= pPos->z;

		g_pcsAgpmCharacter->UpdatePosition( m_pCurrentCharacter , &pos , FALSE , FALSE );
		UpdateCharacter( m_pCurrentCharacter );

		SetSaveData();
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , "Boss 스폰 설정" );

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnSaveData				()
{
	// 헤헤 -_-;;
	// 저자앙~
	VERIFY( AGCMMAP_THIS->EnumLoadedDivision( CPlugin_BossSpawn::CBSave , ( PVOID ) this ) );

	return TRUE;
}

BOOL CPlugin_BossSpawn::CBCheckSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData );
	if( NULL == pData ) return FALSE;

	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	// 스폰 붙은것만 저장함.
	if( g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_SPAWN ) )
		return TRUE;
	else
		return FALSE;
}

BOOL	CPlugin_BossSpawn::CBSave ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	AcuFrameMemory::Clear();

	char	strFilename[ 256 ];

	// OBDN : Object Division Normal
	// OBDS : Object Division Static

	//TRACE( "지형 익스포트 '%s'\b" , strFilename );

	AuBOX bbox;
	bbox.inf.x	=	pDivisionInfo->fStartX;
	bbox.inf.z	=	pDivisionInfo->fStartZ;
	bbox.inf.y	=	0.0f;
	bbox.sup.x	=	pDivisionInfo->fStartX + pDivisionInfo->fWidth;
	bbox.sup.z	=	pDivisionInfo->fStartZ + pDivisionInfo->fWidth;
	bbox.sup.y	=	0.0f;

	wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN ,
		ALEF_CURRENT_DIRECTORY , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

	if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, CPlugin_BossSpawn::CBCheckSpawn )	)
	{
		// 성공
		// do nothing..
	}
	else
	{
		// 실패
		MD_SetErrorMessage("ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , FALSE )\n" , strFilename );
		return FALSE;
	}

	{
		CUITileList_ObjectTabWnd * pWnd = ( CUITileList_ObjectTabWnd * ) pData;

		// 디렉토리 준비..
		GetSubDataDirectory( pWnd->m_strSubDataMainDir );

		_CreateDirectory( pWnd->m_strSubDataMainDir );
		_CreateDirectory( "%s\\ini"							, pWnd->m_strSubDataMainDir );
		_CreateDirectory( "%s\\ini\\object"					, pWnd->m_strSubDataMainDir );
		_CreateDirectory( "%s\\ini\\object\\design"			, pWnd->m_strSubDataMainDir );

		//////////////////////////////////////////////////////////////////////////
		// 마고자 (2004-11-29 오후 12:03:07) : 
		// SubData 저장..
		wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN ,
			pWnd->m_strSubDataMainDir , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

		if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, CPlugin_BossSpawn::CBCheckSpawn )	)
		{
			// 성공
			// do nothing..
		}
		else
		{
			// 실패
			MD_SetErrorMessage("ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , FALSE )\n" , strFilename );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnLoadData				()
{
	DivisionCallbackResult	result;
	VERIFY( AGCMMAP_THIS->EnumLoadedDivision( CPlugin_BossSpawn::CBLoad , static_cast< PVOID >( &result ) ) );

	return TRUE;
}

BOOL CPlugin_BossSpawn::CBLoad ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	DivisionCallbackResult	* pResult = ( DivisionCallbackResult * ) pData;

	ASSERT( NULL != pResult );

	pResult->nDivisionCount++;

	AcuFrameMemory::Clear();

	char	strFilename[ 256 ];

	CFileFind	ff;

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN ,  
		GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , CPlugin_BossSpawn::CBReadSpawn , TRUE )	);
	}
	else
	{
		// 없어서 그냥 리턴한다.
		return TRUE;
	}

	pResult->nSuccessCount++;

	return TRUE;
}

BOOL CPlugin_BossSpawn::CBReadSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData	);
	ASSERT( NULL != pClass	);
	
	if( NULL == pData ) return FALSE;

	CPlugin_BossSpawn	* pThis = CPlugin_BossSpawn::m_spThis;
	ApdObject	*		pcsApdObject = ( ApdObject * ) pData;

	// 툴 작업용으로 임의로 삽입..
	AgpdCharacterTemplate * pTemplate;
	pTemplate = g_pcsAgpmCharacter->GetCharacterTemplate(pThis->m_nLastReadSpawnCharacterID);
	// 트랩 설정 .. 툴작업 편의용.
	g_pcsAgpmCharacter->SetTypeTrap( pTemplate );

	// 케릭터 추가..
	pThis->AddCharacter( pcsApdObject , pThis->m_nLastReadSpawnCharacterID );

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnCleanUpData			()
{
	// 케릭터 돌면서 특정 옵셋 이상의 케릭터 제거 &
	// 연결됀 스폰 오브젝트 제거..
	return TRUE;
}

BOOL CPlugin_BossSpawn::Window_OnKeyDown		( RsKeyStatus *ks	)
{
	if( NULL == m_pCurrentCharacter ) return TRUE;

    switch( ks->keyCharCode )
    {
	case rsDEL:
		{
			// 먼저 정보 제거
			DeleteCharacter( m_pCurrentCharacter );

			// 선택한 케릭터 삭제..
			g_pcsAgpmCharacter->RemoveCharacter( m_pCurrentCharacter->m_lID , FALSE , FALSE );
			m_pCurrentCharacter	= NULL;
			m_pSelectedClump	= NULL;

			SetSaveData();
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::Window_OnLButtonDown	( RsMouseStatus *ms )
{
	RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

	if( pClump )
	{
		INT32	nID;
		switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID )  )
		{
		case ACUOBJECT_TYPE_CHARACTER:
			{
				m_pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				if( NULL == m_pCurrentCharacter )
				{
					return FALSE;
				}

				Invalidate( FALSE );
			}
			break;
		default:
			return FALSE;
		}

		m_pSelectedClump	= pClump;
		m_bLButtonDown		= TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_BossSpawn::Window_OnLButtonDblClk	( RsMouseStatus *ms )
{
	RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

	if( pClump )
	{
		INT32	nID;
		switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID )  )
		{
		case ACUOBJECT_TYPE_CHARACTER:
			{
				m_pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				if( NULL == m_pCurrentCharacter )
				{
					return FALSE;
				}

				Invalidate( FALSE );

				// 스폰 정보 수정..

				Spawn * pSpawn = GetSpawn( m_pCurrentCharacter->m_lID );

				if( pSpawn )
				{
					ApdObject * pObject;
					pObject = pSpawn->GetObject();

					ASSERT( NULL != pObject );

					// 스폰 이벤트 수정.

					ApdEvent *	pstEvent;

					// EventNature Event Struct 얻어냄..
					pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_SPAWN );

					if( pstEvent )
					{
						// 이벤트 수정.
						g_pcsAgcmEventSpawnDlg->Open( pstEvent );
					}

					SetSaveData();

					return TRUE;
				}

			}
			break;
		default:
			return FALSE;
		}

		m_pSelectedClump	= pClump;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_BossSpawn::Window_OnLButtonUp		( RsMouseStatus *ms )
{
	m_pSelectedClump	= NULL;
	m_bLButtonDown		= FALSE;

	return FALSE;
}

BOOL CPlugin_BossSpawn::Window_OnMouseMove		( RsMouseStatus *ms )
{
	if( m_pCurrentCharacter )
	{
		if (ms->control && ms->shift )
		{
			FLOAT	fDeltaX	= ms->pos.x - m_PrevMouseStatus.pos.x;	
			FLOAT	fDeltaY	= ms->pos.y - m_PrevMouseStatus.pos.y;
			
			g_pcsAgpmCharacter->TurnCharacter( m_pCurrentCharacter , 0 , fDeltaY );
			UpdateCharacter( m_pCurrentCharacter );
			SetSaveData();

			return TRUE;
		}
		else if( ms->shift )
		{
			// 높이 조절..

			// y축으로만 움직임..
			//g_pMainFrame->m_pTileList->m_pObjectWnd->TransformObject(
			//	GetSelectedObject() , ms , fDeltaX , fDeltaY , &m_Position, 0 );

			// 그냥 Translate..
			FLOAT	fHeightDistance = 0.0f;

			// 변화할 높이 차를 구함.
			{
				RwV3d			*	camLookAt		= RwMatrixGetAt	( RwFrameGetLTM( RwCameraGetFrame( g_pEngine->m_pCamera ) ) );

				RwV3d normalvector	;
				RwV3d CollisionPoint;

				// 면의 노멀벡터..
				normalvector.x	= - camLookAt->x;
				normalvector.y	= 0.0f			;
				normalvector.z	= - camLookAt->z;

				RwLine	CameraPixelRay		;

				RwCameraCalcPixelRay( g_pEngine->m_pCamera , &CameraPixelRay	, &ms->pos );
				
				_GetIntersectPos( 
					&m_pCurrentCharacter->m_stPos	,
					&normalvector					,
					&CameraPixelRay.start			,
					&CameraPixelRay.end				,
					&CollisionPoint					);

				fHeightDistance = CollisionPoint.y - m_pCurrentCharacter->m_stPos.y;
			}

			m_pCurrentCharacter->m_stPos.y += fHeightDistance;
			g_pcsAgpmCharacter->UpdatePosition	( m_pCurrentCharacter , &m_pCurrentCharacter->m_stPos ); 
			UpdateCharacter( m_pCurrentCharacter );
			return TRUE;
		}
	}

	m_PrevMouseStatus = * ms;
	return FALSE;
}

BOOL CPlugin_BossSpawn::OnWindowRender			()
{
	__DrawWireFrame( m_pSelectedClump );

	static BOOL _bShowSphere = FALSE;

	if( _bShowSphere )
	{
		INT32			lIndex			;
		ApdObject	*	pcsObject		;

		lIndex = 0;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{

			if( CPlugin_BossSpawn::CBCheckSpawn( ( void * ) pcsObject , NULL , NULL ) )
			{
				//AgcdObject * pstAgcdObject = g_pcsAgcmObject->GetObjectData(pcsObject);
				// AcuObjecWire::bGetInst().bRenderClump( pstAgcdObject->m_stGroup.m_pstList[ 0 ].m_csData.m_pstClump );
				g_MainWindow.DrawAreaSphere( pcsObject->m_stPosition.x , pcsObject->m_stPosition.y , pcsObject->m_stPosition.z ,
					g_pMainFrame->m_Document.m_fBrushRadius );
			}
		}
	}

	return TRUE;
}

int CPlugin_BossSpawn::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUITileList_PluginBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect	rect ;
	rect.SetRect( 0 , 0 , 300 , 200 );
	m_wndCombo.Create( CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL , rect , this , 21532 );

	// 케릭터 이누머레이팅 하여..
	// "%04d , 케릭터 네임" 으로 콤보에 삽입한다..

	if( g_pcsAgpmCharacter )
	{
		INT32						lIndex		= 0;
		AgpdCharacterTemplate	*	pTemplate	;
		char						str[ 256 ]	;
		for (	pTemplate	= g_pcsAgpmCharacter->GetTemplateSequence(&lIndex);
				pTemplate	;
				pTemplate	= g_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
		{
			wsprintf( str , "%04d, %s" , pTemplate->m_lID , pTemplate->m_szTName );
			m_wndCombo.AddString( str );
		}
	}

	/*
	if ( g_pcsApmObject &&
		!g_pcsApmObject->AddStreamCallback(
			APMOBJECT_DATA_OBJECT		,
			BossInfoStreamReadCB		,
			BossInfoStreamWriteCB		,
			this						)
		)
		return FALSE;
	*/

	return 0;
}

// 새로 스폰 오브젝트를 생성할때 디폴트 값으로 오브젝트 추가와 이벤트 등록.
BOOL	CPlugin_BossSpawn::AddSpawnObject( AgpdCharacter * pCharacter )
{
	ASSERT( NULL != pCharacter );
	if( NULL == pCharacter ) return FALSE;

	ApdObjectTemplate	*	pstApdObjectTemplate	;
	ApdObject			*	pstApdObject			;
	FLOAT	fRadius = 0.0f;

	pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(467);
	if( !pstApdObjectTemplate )
	{
		TRACE( "템플릿 ID에서 템플릿 정보 얻기 실패..\n" );
		return FALSE;
	}

	//pstApdObject = g_pcsApmObject->AddObject( 
	//	g_pcsApmObject->GetEmptyIndex( GetDivisionIndexF( pCharacter->m_stPos.x , pCharacter->m_stPos.z ) ) , pstApdObjectTemplate->m_lID );

	RwV3d	pos;
	pos.x	= pCharacter->m_stPos.x;
	pos.y	= pCharacter->m_stPos.y;
	pos.z	= pCharacter->m_stPos.z;

	pstApdObject = g_pMainFrame->m_pTileList->m_pObjectWnd->AddObject(
		pstApdObjectTemplate ,
		&pos , 
		ACUOBJECT_TYPE_DUNGEON_STRUCTURE	// 던젼 스트럭쳐로 등록함..
		);

	ASSERT( NULL != pstApdObject );
	if( !pstApdObject )
	{
		MD_SetErrorMessage( "g_pcsApmObject->AddObject 오브젝트 추가실패.." );
		return NULL;
	}

	// 위치지정..
	pstApdObject->m_stPosition	= pCharacter->m_stPos;

	pstApdObject->m_stScale.x	= 0.000001f; // 보이지 말아라 >_<;;
	pstApdObject->m_stScale.y	= 0.000001f;
	pstApdObject->m_stScale.z	= 0.000001f;

	g_pcsAgcmObject->SetupObjectClump_TransformOnly( pstApdObject );

	try
	{
		ApdObject * pObject = pstApdObject;
		ApdEvent *	pstEvent;

		// EventNature Event Struct 얻어냄..
		pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_SPAWN );

		if( NULL == pstEvent )
		{
			// 이벤트 삽입..
			ApdEventAttachData *pstEventAD = g_pcsApmEventManager->GetEventData( pObject );

			pstEvent = g_pcsApmEventManager->AddEvent(pstEventAD, APDEVENT_FUNCTION_SPAWN , pObject , FALSE);
		}

		if( pstEvent )
		{
			// 범위설정~
			g_pcsApmEventManager->SetCondition( pstEvent , APDEVENT_COND_AREA );
			pstEvent->m_pstCondition->m_pstArea->m_eType = APDEVENT_AREA_SPHERE;
			pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius = fRadius;

			// 스폰 정보 설정 여기서..
			g_pcsAgcmEventSpawnDlg->Open( pstEvent );

			// 마고자 (2005-06-20 오후 3:56:29) : 
			// 이거 의미 없음..
			/*
			// 스폰 정보 설정..
			AgpdSpawn	*	pSpawn			;
			pSpawn = (AgpdSpawn *) pstEvent->m_pvData;
			pSpawn->m_stConfig.m_lSpawnChar = 1;
			pSpawn->m_stConfig.m_astChar[0].m_lCTID = pCharacter->m_pcsCharacterTemplate->m_lID;
			*/
		}
		
		// 어레이에 추가해둠
		Spawn	stSpawn;
		stSpawn.nCID	= pCharacter->m_lID	;
		stSpawn.nOID	= pObject->m_lID	;
		stSpawn.nTID	= pCharacter->m_pcsCharacterTemplate->m_lID;
		m_vecSpawn.push_back( stSpawn );
	}
	catch(...)
	{
	}

	return TRUE;
}

// 스폰오브젝트가 로드된후 , 그 정보로 케릭터를 추가하기 위한것.
BOOL	CPlugin_BossSpawn::AddCharacter( ApdObject * pObject , INT32 nTemplateID )
{
	AgpdCharacter * pCharacter;

	{
		/*
		ApdEvent *	pstEvent;

		// EventNature Event Struct 얻어냄..
		pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_SPAWN );

		ASSERT( pstEvent );

		if( pstEvent )
		{
			// 스폰 템플릿 ID를 구함..
			AgpdSpawn	*	pSpawn			;
			pSpawn = (AgpdSpawn *) pstEvent->m_pvData;
			nTemplateID = pSpawn->m_stConfig.m_astChar[0].m_lCTID;
		}
		else
		{
			MD_SetErrorMessage( "보스 스폰 오브젝트 이상.." );
			return FALSE;
		}
		*/
	}

	// 이렇게 하면 안되지... -_-;
	// m_nCharacterIDCounter NPC파일을 읽으면 다음부턴 어떻게 찍으라고... -_-; By Parn
	char	strNPCName[ 256 ];

	m_nCharacterIDCounter = rand() % 10000 + BOSSMOB_OFFSET;
	wsprintf( strNPCName , "BOSS_%04d" , m_nCharacterIDCounter % BOSSMOB_OFFSET );

	g_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ) );		// modify by gemani
	pCharacter = g_pcsAgpmCharacter->AddCharacter( m_nCharacterIDCounter++ , nTemplateID , strNPCName );

	if( NULL == pCharacter )
	{
		DisplayMessage( RGB( 232 , 121 , 25 ) , "해당 케릭터에 대한 템플릿이 없어요~" );
		return FALSE;
	}

	// 해당 위치에 템플릿 삽입..

	AuPOS	pos;
	pos	= pObject->m_stPosition;

	ApWorldSector * pWorldSector;
	pWorldSector = g_pcsApmMap->GetSector( pos.x , pos.z );

	if( pWorldSector )
	{
		pCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD	;
		pCharacter->m_stPos				= pos							;
		pCharacter->m_fTurnX			= pObject->m_fDegreeX			;
		pCharacter->m_fTurnY			= pObject->m_fDegreeY			;

		g_pcsAgpmCharacter->UpdateInit( pCharacter );

		g_pcsAgpmCharacter->UpdatePosition	( pCharacter , &pos					); 

		// 어레이에 추가해둠
		Spawn	stSpawn;
		stSpawn.nCID	= pCharacter->m_lID	;
		stSpawn.nOID	= pObject->m_lID	;
		stSpawn.nTID	= nTemplateID		;
		m_vecSpawn.push_back( stSpawn );
	}
	else
	{
		// 실패했으므로 삭제..

		// 어떻게? -_-;;;
	}

	SetSaveData();

	return FALSE;
}

// 위치,방향의 변경 정보를 가지고 스폰정보와 싱크를 맞춤.
BOOL	CPlugin_BossSpawn::UpdateCharacter( AgpdCharacter * pCharacter )
{
	ASSERT( NULL != pCharacter );
	if( NULL == pCharacter ) return FALSE;

	Spawn * pSpawn = GetSpawn( pCharacter->m_lID );

	if( pSpawn )
	{
		ApdObject * pObject;
		pObject = pSpawn->GetObject();

		// 위지 초절...
		// 데이타만 변경하고 따로 Init를 시키지 않음.
		pObject->m_stPosition	= pCharacter->m_stPos	;
		pObject->m_fDegreeX		= pCharacter->m_fTurnX	;
		pObject->m_fDegreeY		= pCharacter->m_fTurnY	;

		return TRUE;
	}
	else
	{
		ASSERT( !"이거 나오면 못쓰는데 -_-.." );
	}

	return FALSE;
}

// 케릭터를 삭제하면서 , 같이 등록된 스폰 오브젝트도 삭제함.
// m_vecSpawn에는 두 인덱스를 0으로 설정해둬서 나중에 UpdateList 에서 삭제돼게함.
BOOL	CPlugin_BossSpawn::DeleteCharacter( AgpdCharacter * pCharacter )
{
	ASSERT( NULL != pCharacter );
	if( NULL == pCharacter ) return FALSE;

	Spawn * pSpawn = GetSpawn( pCharacter->m_lID );

	if( pSpawn )
	{
		ApdObject * pObject;
		pObject = pSpawn->GetObject();

		// 오브젝트 삭제..
		if( pObject )
		{
			g_pcsApmObject->DeleteObject( pObject );
		}

		pSpawn->Delete();
		UpdateList();

		return TRUE;
	}
	else
	{
		ASSERT( !"이거 나오면 못쓰는데 -_-.." );
	}

	return FALSE;
}

// m_vecSpawn에서 사라진 오브젝트들을 정리한다.
void	CPlugin_BossSpawn::UpdateList()
{
	vector< Spawn >::iterator iter;
	
	BOOL bFound = FALSE;
	do
	{
		bFound = FALSE;
		for( iter = m_vecSpawn.begin();
			iter != m_vecSpawn.end();
			iter ++ )
		{
			if( (*iter).nCID == 0	&&
				(*iter).nOID == 0	) 
			{
				// 식제 대상.
				m_vecSpawn.erase( iter );
				bFound = TRUE;
				break;
			}
		}
	}
	while( bFound );
}

BOOL	CPlugin_BossSpawn::BossInfoStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	CPlugin_BossSpawn	*pThis			= (CPlugin_BossSpawn *) pClass;
	ApdObject			*pstApdObject	= (ApdObject *) pData;

	Spawn * pSpawn = pThis->GetSpawnByObject( pstApdObject->m_lID );

	if( pSpawn )
	{
		if( !pcsStream->WriteValue( ( char * ) g_strINIObjectBossInfo , pSpawn->nTID ) )
			return FALSE;
	}

	return TRUE;
}

BOOL	CPlugin_BossSpawn::BossInfoStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	CPlugin_BossSpawn	*pThis = (CPlugin_BossSpawn *) pClass;
	ApdObject			*pstApdObject	= (ApdObject *) pData;

	const CHAR *szValueName;
	//char	strData		[ 256 ];

	while(pcsStream->ReadNextValue())
	{
		szValueName = pcsStream->GetValueName();

		if (!strcmp(szValueName, ( char * ) g_strINIObjectBossInfo))
		{
			// 마고자 (2005-06-20 오후 3:52:14) : 
			// 최후에 얻어둔 CID를 보관해둠..
			pcsStream->GetValue ( (INT32 *) &pThis->m_nLastReadSpawnCharacterID );
		}
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::Window_OnRButtonDown	( RsMouseStatus *ms )
{
	if( GetAsyncKeyState( VK_SHIFT ) < 0 )
	{
		RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

		if( pClump )
		{
			INT32	nID;
			switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID )  )
			{
			case ACUOBJECT_TYPE_CHARACTER:
				{
					m_pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
					if( NULL == m_pCurrentCharacter )
					{
						return FALSE;
					}

					Invalidate( FALSE );
				}
				break;
			default:
				return FALSE;
			}

			m_pSelectedClump	= pClump;

			CPositionDlg	dlg;

			dlg.m_fX		= m_pCurrentCharacter->m_stPos.x;
			dlg.m_fY		= m_pCurrentCharacter->m_stPos.y;
			dlg.m_fZ		= m_pCurrentCharacter->m_stPos.z;
			dlg.m_fDegreeX	= m_pCurrentCharacter->m_fTurnX;
			dlg.m_fDegreeY	= m_pCurrentCharacter->m_fTurnY;

			if( IDOK == dlg.DoModal() )
			{
				AuPOS	pos;
				pos.x	= dlg.m_fX;
				pos.y	= dlg.m_fY;
				pos.z	= dlg.m_fZ;

				g_pcsAgpmCharacter->UpdatePosition( m_pCurrentCharacter , &pos , FALSE , FALSE );
				g_pcsAgpmCharacter->TurnCharacter( m_pCurrentCharacter , 0 , dlg.m_fDegreeY );
				UpdateCharacter( m_pCurrentCharacter );

				SetSaveData();
			}

			m_pSelectedClump	= NULL;
			m_pCurrentCharacter	= NULL;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
