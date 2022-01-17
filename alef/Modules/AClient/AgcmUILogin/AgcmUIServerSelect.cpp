#include "AgcmUIServerSelect.h"
#include "AgpmWorld.h"
#include "AgcmWorld.h"
#include "AgcmUIManager2.h"
#include "AgcmUILogin.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AuStrTable.h"
#include "ApMutualEx.h"
#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
#endif




BOOL stServerGroup::AddServer( INT32 nServerIndex, void* pWorld, BOOL bHaveMyCharacter, INT16 nServerState, INT32 nAgeRate, INT32 nOpenState )
{
	stServerInfomation* pServer = m_mapServer.Get( nServerIndex );
	if( pServer )
	{
		pServer->m_nServerIndex = nServerIndex;
		pServer->m_ppdWorld = pWorld;
		pServer->m_bHaveMyCharacter = bHaveMyCharacter;
		pServer->m_nServerState = nServerState;
		pServer->m_nServerAgeRate = nAgeRate;
		pServer->m_nServerOpenState = nOpenState;
		strcpy( pServer->m_strServerName, ( ( AgpdWorld* )pServer->m_ppdWorld )->m_szName );
	}
	else
	{
		stServerInfomation NewServer;

		NewServer.m_nServerIndex = nServerIndex;
		NewServer.m_ppdWorld = pWorld;
		NewServer.m_bHaveMyCharacter = bHaveMyCharacter;
		NewServer.m_nServerState = nServerState;
		NewServer.m_nServerAgeRate = nAgeRate;
		NewServer.m_nServerOpenState = nOpenState;

		if( !NewServer.m_ppdWorld ) return FALSE;

		strcpy( NewServer.m_strServerName, ( ( AgpdWorld* )NewServer.m_ppdWorld )->m_szName );
		m_mapServer.Add( nServerIndex, NewServer );
	}

	return TRUE;
}

BOOL stServerGroup::DeleteServer( INT32 nServerIndex )
{
	m_mapServer.Delete( nServerIndex );
	return TRUE;
}

stServerInfomation* stServerGroup::GetServer( INT32 nServerIndex )
{
	return m_mapServer.Get( nServerIndex );
}

stServerInfomation* stServerGroup::GetServerByIndex( INT32 nIndex )
{
	return m_mapServer.GetByIndex( nIndex );
}

stServerInfomation*	stServerGroup::GetServerByName( CHAR* pServerName )
{
	if( !pServerName || strlen( pServerName ) == 0 ) return NULL;
	INT32 nServerCount = m_mapServer.GetSize();

	for( INT32 nCount = 0 ; nCount < nServerCount ; ++nCount )
	{
		stServerInfomation* pServer = GetServerByIndex( nCount );
		if( pServer )
		{
			if( strcmp( pServer->m_strServerName, pServerName ) == 0 )
			{
				return pServer;
			}
		}
	}

	return NULL;
}





AgcmUIServerSelect::AgcmUIServerSelect( void )
: m_nSelectServerIndex( -1 ), m_pcmUILogin( NULL ), m_pcdUserData( NULL ),
	m_bIsSelectDialogOpen( FALSE ), m_bIsServerSelectComplete( FALSE ), m_nPrevUpdateTime( 0 )
{
	OnClear();
}

AgcmUIServerSelect::~AgcmUIServerSelect( void )
{
	OnClear();
}

BOOL AgcmUIServerSelect::OnInitialize( void* pUILogin )
{
	OnClear();

	m_pcmUILogin = pUILogin;
	return TRUE;
}

BOOL AgcmUIServerSelect::OnRefresh( void )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	if( m_pcdUserData )
	{
		pcmUIManager->SetUserDataRefresh( ( AgcdUIUserData* )m_pcdUserData );
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnClear( void )
{
	m_mapServerGroup.Clear();
	m_nSelectServerIndex = -1;
	memset( m_nServerIndexList, 0, sizeof( INT32 ) * SERVER_LIST_MAX );

	m_pcmUILogin = NULL;
	m_pcdUserData = NULL;

	for( INT32 nCount = 0 ; nCount < SERVER_LIST_MAX ; ++nCount )
	{
		m_nServerIndexList[ nCount ] = nCount;
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnLoadServerList( void )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmWorld* pcmWorld = ( AgcmWorld* )pcmUILogin->GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;

	WorldGroupVec& vecWorldGroup = pcmWorld->GetWorldGroup();
	WorldGroupVecItr Iter = vecWorldGroup.begin();

	INT32 nServerCount = 0;
	m_mapServerGroup.Clear();
	
	while( Iter != vecWorldGroup.end() )
	{
		_ClearServerList( nServerCount );

		OnSelectServer( nServerCount, nServerCount == m_nSelectServerIndex );
		OnChangeTreeState( nServerCount, ( *Iter ).m_bExtend );

		stServerGroup NewGroup;
		stServerGroup* pGroup = m_mapServerGroup.Get( nServerCount );
		if( pGroup )
		{
			pGroup->m_nServerGroupIndex = nServerCount;
			memset( pGroup->m_strServerGroupName, 0, sizeof( CHAR ) * 64 );
			strcpy( pGroup->m_strServerGroupName, ( *Iter ).m_strName.c_str() );
			pGroup->m_bIsTreeOpen = ( *Iter ).m_bExtend;
		}
		else
		{
			NewGroup.m_nServerGroupIndex = nServerCount;
			strcpy( NewGroup.m_strServerGroupName, ( *Iter ).m_strName.c_str() );
			NewGroup.m_bIsTreeOpen = ( *Iter ).m_bExtend;

//#ifdef _DEBUG
//			CHAR strDebug[ 256 ] = { 0, };
//			sprintf( strDebug, "Add ServerGroup, Index = %d, Name = %s\n", nServerCount, NewGroup.m_strServerGroupName );
//			OutputDebugString( strDebug );
//#endif
		}

		++nServerCount;

		if( ( *Iter ).m_bExtend )
		{
			WorldVecItr IterWorld = ( *Iter ).m_vecWorld.begin();
			while( IterWorld != ( *Iter ).m_vecWorld.end() )
			{
				OnSelectServer( nServerCount, nServerCount == m_nSelectServerIndex );
				_DisableServerGroupFlag( nServerCount );

				AgpdWorld* ppdWorld = ( AgpdWorld* )( *IterWorld );
				OnChangeServerState( ppdWorld->m_nStatus, nServerCount );

				BOOL bHaveMyCharacter = pcmWorld->GetAD( ppdWorld )->m_lCharacterCount > 0 ? TRUE : FALSE;
				OnChangeServerCharacter( nServerCount, bHaveMyCharacter );

				OnChangeServerAgeRate( nServerCount, 0 );

				INT32 nServerAgeRate = 1;
				if( AP_SERVICE_AREA_KOREA == g_eServiceArea)
					nServerAgeRate = ppdWorld->IsNC17() ? 2 : 1;
				else
					nServerAgeRate = 0; //해외는 15세 서버 없음.

				OnChangeServerAgeRate( nServerCount, nServerAgeRate );

				INT32 nServerOpen = 0;
				
				//if( ppdWorld->IsTestServer() )
				//{
				//	nServerOpen	=	3;
				//}
				//else
				{
					nServerOpen	=	ppdWorld->IsEvent() ? 2 : ( ppdWorld->IsNew() ? 1 : 0 );
				}

				OnChangeServerOpen( nServerCount, nServerOpen );

				if( pGroup )
				{
					pGroup->AddServer( nServerCount, ppdWorld, bHaveMyCharacter, ppdWorld->m_nStatus, nServerAgeRate, nServerOpen );
				}
				else
				{
					NewGroup.AddServer( nServerCount, ppdWorld, bHaveMyCharacter, ppdWorld->m_nStatus, nServerAgeRate, nServerOpen );
				}

				AgcmUIItem* pcmUIItem = ( AgcmUIItem* )pcmUILogin->GetModule( "AgcmUIItem" );
				if( pcmUIItem )
				{
					pcmUIItem->m_AutoPickUp.AddWorld( ppdWorld->m_szName );
				}

				++nServerCount;
				++IterWorld;
			}
		}

		if( !pGroup )
		{
			m_mapServerGroup.Add( NewGroup.m_nServerGroupIndex, NewGroup );
		}

		++Iter;
	}

	( ( AgcdUIUserData* )m_pcdUserData )->m_stUserData.m_lCount = nServerCount;
	OnRefresh();
	return TRUE;
}

BOOL AgcmUIServerSelect::OnUpdateServerInfo( INT32 nClockCount )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmWorld* pcmWorld = ( AgcmWorld* )pcmUILogin->GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	if( m_bIsSelectDialogOpen && ( m_nPrevUpdateTime + AGCMUILOGIN_TERM_UPDATE_SERVER < nClockCount ) )
	{
		m_nPrevUpdateTime = nClockCount;
		pcmWorld->SendPacketGetWorld( NULL, pcmLogin->m_nNID );
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnSelectServer( INT32 nServerIndex, BOOL bIsSelect )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	eAGCMUILOGIN_EVENT eEvent = bIsSelect ? AGCMUILOGIN_EVENT_ON_SELECT_SERVER_BAR : AGCMUILOGIN_EVENT_OFF_SELECT_SERVER_BAR;
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ eEvent ], nServerIndex );
	return TRUE;
}

BOOL AgcmUIServerSelect::OnClickListGroup( INT32 nClickIndex )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmWorld* pcmWorld = ( AgcmWorld* )pcmUILogin->GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;

	WorldGroupVec& vecWorldGroup = pcmWorld->GetWorldGroup();
	vecWorldGroup[ nClickIndex ].ToggleExtend();

	OnLoadServerList();
	return TRUE;
}

BOOL AgcmUIServerSelect::OnClickListServer( INT32 nClickIndex )
{
	return TRUE;
}

BOOL AgcmUIServerSelect::OnChangeTreeState( INT32 nGroupIndex, BOOL bIsOpen )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	eAGCMUILOGIN_EVENT eEvent = bIsOpen ? AGCMUILOGIN_EVENT_SERVER_MINUS_ON : AGCMUILOGIN_EVENT_SERVER_PLUS_ON;
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ eEvent ], nGroupIndex );

	stServerGroup* pServerGroup = _GetServerGroup( nGroupIndex );
	if( pServerGroup )
	{
		pServerGroup->m_bIsTreeOpen = bIsOpen;
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnChangeServerState( INT32 nServerIndex, INT16 nServerState )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	eAGCMUILOGIN_EVENT eEvent;
	switch( nServerState )
	{
	case AGPDWORLD_STATUS_GOOOOOOD :		eEvent = AGCMUILOGIN_EVENT_SERVER_STATUS_GOOD;		break;
	case AGPDWORLD_STATUS_ABOVE_NORMAL :	eEvent = AGCMUILOGIN_EVENT_SERVER_STATUS_NORMAL;	break;
	case AGPDWORLD_STATUS_NORMAL :			eEvent = AGCMUILOGIN_EVENT_SERVER_STATUS_NORMAL;	break;
	case AGPDWORLD_STATUS_BELOW_NORMAL :	eEvent = AGCMUILOGIN_EVENT_SERVER_STATUS_BUSY;		break;
	case AGPDWORLD_STATUS_BAD :				eEvent = AGCMUILOGIN_EVENT_SERVER_STATUS_FULL;		break;
	default :								eEvent = AGCMUILOGIN_EVENT_SERVER_STATUS_CLOSE;		break;
	}

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ eEvent ], nServerIndex );

	stServerInfomation* pServer = _GetServer( nServerIndex );
	if( pServer )
	{
		pServer->m_nServerState = nServerState;
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnChangeServerCharacter( INT32 nServerIndex, BOOL bHaveMyCharacter )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	eAGCMUILOGIN_EVENT eEvent = bHaveMyCharacter ? AGCMUILOGIN_EVENT_ON_CHARACTER_MARK : AGCMUILOGIN_EVENT_OFF_CHARACTER_MARK;
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ eEvent ], nServerIndex );

	stServerInfomation* pServer = _GetServer( nServerIndex );
	if( pServer )
	{
		pServer->m_bHaveMyCharacter = bHaveMyCharacter;
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnChangeServerAgeRate( INT32 nServerIndex, INT32 nAgeRate )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	eAGCMUILOGIN_EVENT eEvent = AGCMUILOGIN_EVENT_SERVER_RATE_NONE;

	switch( nAgeRate )
	{
	case 0 :	eEvent = AGCMUILOGIN_EVENT_SERVER_RATE_NONE;	break;
	case 1 :	eEvent = AGCMUILOGIN_EVENT_SERVER_RATE_15;		break;
	case 2 :	eEvent = AGCMUILOGIN_EVENT_SERVER_RATE_17;		break;
	}

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ eEvent ], nServerIndex );

	stServerInfomation* pServer = _GetServer( nServerIndex );
	if( pServer )
	{
		pServer->m_nServerAgeRate = nAgeRate;
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnChangeServerOpen( INT32 nServerIndex, INT32 nOpenState )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	eAGCMUILOGIN_EVENT eEvent = AGCMUILOGIN_EVENT_SERVER_OPEN_OLD;

	switch( nOpenState )
	{
	case 0 :	eEvent = AGCMUILOGIN_EVENT_SERVER_OPEN_OLD;			break;
	case 1 :	eEvent = AGCMUILOGIN_EVENT_SERVER_OPEN_NEW;			break;
	case 2 :	eEvent = AGCMUILOGIN_EVENT_SERVER_OPEN_EVENT;		break;
	case 3:		eEvent = AGCMUILOGIN_EVENT_SERVER_OPEN_TEST;		break;
	}

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ eEvent ], nServerIndex );

	stServerInfomation* pServer = _GetServer( nServerIndex );
	if( pServer )
	{
		pServer->m_nServerOpenState = nOpenState;
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnEnterCharacterSelect( void* pWorld )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgpdWorld* ppdWorld = ( AgpdWorld* )pWorld;
	if( !ppdWorld ) return FALSE;
	if( ppdWorld->m_nStatus == AGPDWORLD_STATUS_UNKNOWN ) return FALSE;
	if( ppdWorld->m_nStatus == AGPDWORLD_STATUS_BAD )
	{
		pcmUIManager->ActionMessageOKDialog( "서버가 포화상태이므로 현재는 접속할 수 없습니다." );
		return FALSE;
	}

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	m_bIsSelectDialogOpen = FALSE;
	m_bIsServerSelectComplete = TRUE;
	_tcsncpy( pcmLogin->m_szWorldName, ppdWorld->m_szName, AGPDWORLD_MAX_WORLD_NAME );

	AgcmUIMain* pcmUIMain = ( AgcmUIMain* )pcmUILogin->GetModule( "AgcmUIMain" );
	if( pcmUIMain )
	{
		sprintf( pcmUIMain->m_szGameServerName, "%s%s", ppdWorld->m_szName, ClientStr().GetStr( STI_SERVER ) );
		pcmUIMain->m_bIsNC17 = ppdWorld->IsNC17();
	}

	AgcmWorld* pcmWorld = ( AgcmWorld* )pcmUILogin->GetModule( "AgcmWorld" );
	if( pcmWorld )
	{
		pcmWorld->m_pAgpdWorldSelected = ppdWorld;
	}

	( ( AgcdUIUserData* )m_pcdUserData )->m_lSelectedIndex = m_nSelectServerIndex;
	OnRefresh();
	pcmLogin->SendGetUnion( ppdWorld->m_szName, pcmLogin->m_szAccount );

	if( g_pEngine )
	{
		g_pEngine->WaitingDialog( NULL, pcmUIManager->GetUIMessage( "LOGIN_WAITING" ) );
	}

	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )pcmUILogin->GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		pcmUIItem->m_AutoPickUp.UseWorld( ppdWorld->m_szName );
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::OnServerClose( void )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	m_bIsSelectDialogOpen = FALSE;
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CLOSE_SELECT_SERVER_WINDOW ] );

	if( pcmUILogin->m_bLoadSImpleUI )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_2D_CLOSE ] );
	}

	pcmUILogin->InitAgcmUILogin();
	pcmLogin->DisconnectLoginServer();
	pcmUILogin->StartLoginProcess();
	return TRUE;
}

void* AgcmUIServerSelect::GetCurrentApgdWorld( void )
{
	return GetServerAgpdWorld( m_nSelectServerIndex );
}

void* AgcmUIServerSelect::GetServerAgpdWorld( INT32 nServerIndex )
{
	stServerInfomation* pServer = _GetServer( nServerIndex );
	if( !pServer ) return NULL;
	return pServer->m_ppdWorld;
}

INT32 AgcmUIServerSelect::GetGroupIndexByListIndex( INT32 nListIndex )
{
	INT32 nServerGroupCount = m_mapServerGroup.GetSize();
	for( INT32 nCount = 0 ; nCount < nServerGroupCount ; ++nCount )
	{
		stServerGroup* pServerGroup = m_mapServerGroup.GetByIndex( nCount );
		if( pServerGroup )
		{
			if( pServerGroup->m_nServerGroupIndex == nListIndex )
			{
				return nCount;
			}
		}
	}

	return -1;
}

void AgcmUIServerSelect::SetCurrentSelectIndex( INT32 nIndex )
{
	m_nSelectServerIndex = nIndex;
	( ( AgcdUIUserData* )m_pcdUserData )->m_lSelectedIndex = nIndex;
}
 
CHAR* AgcmUIServerSelect::GetServerGroupName( INT32 nIndex )
{
	stServerGroup* pServerGroup = m_mapServerGroup.Get( nIndex );
	if( !pServerGroup ) return "";

	return pServerGroup->m_strServerGroupName;
}

CHAR* AgcmUIServerSelect::GetUIMessage( CHAR* pMsg )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return NULL;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return NULL;

	return pcmUIManager->GetUIMessage( pMsg );
}

BOOL AgcmUIServerSelect::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_pcdUserData = pcmUIManager->AddUserData( "Login_Servers", m_nServerIndexList, sizeof( INT32 ), SERVER_LIST_MAX, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pcdUserData ) return FALSE;

	return TRUE;
}

BOOL AgcmUIServerSelect::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->AddFunction( this, "LG_SelectServer",			CB_OnSelectServer, 1, "BTN_SERVER" );
	pcmUIManager->AddFunction( this, "LG_ClickServerList",		CB_OnClickList, 0 );
	pcmUIManager->AddFunction( this, "LG_SelectServerClose",	CB_OnCloseServer, 0 );
	pcmUIManager->AddFunction( this, "LG_ServerTreeButton",		CB_OnClickTree, 0 );
	return TRUE;
}

BOOL AgcmUIServerSelect::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->AddDisplay( this, "Server_Name", 0,	CB_OnDisplayServerName, AGCDUI_USERDATA_TYPE_INT32 );
	pcmUIManager->AddDisplay( this, "Server_Status", 0, CB_OnDisplayServerState, AGCDUI_USERDATA_TYPE_INT32 );
	return TRUE;
}

BOOL AgcmUIServerSelect::OnAddBoolean( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->AddBoolean( this, "CBIsActiveLoginBackButton",	CB_IsActiveBtnBack, AGCDUI_USERDATA_TYPE_BOOL );
	pcmUIManager->AddBoolean( this, "CBIsServerTreeButton",			CB_IsClickedBtnTree, AGCDUI_USERDATA_TYPE_BOOL );
	return TRUE;
}

BOOL AgcmUIServerSelect::_DisableServerGroupFlag( INT32 nServerIndex )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_PLUS_OFF ], nServerIndex );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_MINUS_OFF ], nServerIndex );
	return TRUE;
}

BOOL AgcmUIServerSelect::_ClearServerList( INT32 nListIndex )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_PLUS_OFF ], nListIndex );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_MINUS_OFF ], nListIndex );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_STATUS_CLOSE ], nListIndex );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_OFF_CHARACTER_MARK ], nListIndex );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_RATE_NONE ], nListIndex );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_SERVER_OPEN_OLD ], nListIndex );
	return TRUE;
}

stServerGroup* AgcmUIServerSelect::_GetServerGroup( INT32 nServerGroupIndex )
{
	return m_mapServerGroup.Get( nServerGroupIndex );
}

stServerInfomation* AgcmUIServerSelect::_GetServer( INT32 nServerIndex )
{
	INT32 nServerGroupCount = m_mapServerGroup.GetSize();

	for( INT32 nCount = 0 ; nCount < nServerGroupCount ; ++nCount )
	{
		stServerGroup* pServerGroup = m_mapServerGroup.GetByIndex( nCount );
		if( pServerGroup )
		{
			stServerInfomation* pServer = pServerGroup->GetServer( nServerIndex );
			if( pServer )
			{
				return pServer;
			}
		}
	}

	return NULL;
}

BOOL AgcmUIServerSelect::CB_OnSelectServer( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIServerSelect* pThis = ( AgcmUIServerSelect* )pClass;
	if( !pThis || !pControl ) return FALSE;
	if( pThis->GetIsServerSelectComplete() ) return TRUE;

	AgpdWorld* ppdWorld = ( AgpdWorld* )pThis->GetServerAgpdWorld( pThis->m_nSelectServerIndex );
	if( ppdWorld )
	{
		pThis->OnEnterCharacterSelect( ppdWorld );
	}
	else
	{
		INT32 nClickIndex = pThis->GetGroupIndexByListIndex( pThis->GetCurrentSelectIndex() );
		pThis->OnClickListGroup( nClickIndex );
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::CB_OnClickList( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIServerSelect* pThis = ( AgcmUIServerSelect* )pClass;
	if( !pThis || !pControl ) return FALSE;
	if( pThis->GetIsServerSelectComplete() ) return TRUE;

	if( pThis->GetCurrentSelectIndex() == pControl->m_lUserDataIndex )
	{
		AgpdWorld* ppdWorld = ( AgpdWorld* )pThis->GetServerAgpdWorld( pThis->GetCurrentSelectIndex() );
		if( ppdWorld )
		{
			pThis->OnEnterCharacterSelect( ppdWorld );
		}
		else
		{
			INT32 nClickIndex = pThis->GetGroupIndexByListIndex( pThis->GetCurrentSelectIndex() );
			pThis->OnClickListGroup( nClickIndex );
		}
	}
	else
	{
		pThis->OnLoadServerList();
	}

	pThis->SetCurrentSelectIndex( pControl->m_lUserDataIndex );
	pThis->OnRefresh();
	return TRUE;
}

BOOL AgcmUIServerSelect::CB_OnCloseServer( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIServerSelect* pThis = ( AgcmUIServerSelect* )pClass;
	if( !pThis ) return FALSE;

	pThis->OnServerClose();
	return TRUE;
}

BOOL AgcmUIServerSelect::CB_OnClickTree( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIServerSelect* pThis = ( AgcmUIServerSelect* )pClass;
	if( !pThis || !pControl ) return FALSE;

	AgpdWorld* ppdWorld = ( AgpdWorld* )pThis->GetServerAgpdWorld( pControl->m_lUserDataIndex );
	if( !ppdWorld )
	{
		INT32 nClickIndex = pThis->GetGroupIndexByListIndex( pControl->m_lUserDataIndex );
		pThis->OnClickListGroup( nClickIndex );
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::CB_OnDisplayServerName( void* pClass, void* pData, AgcdUIDataType eType, INT32 lID, CHAR* pDisplay, INT32* pValue, AgcdUIControl* pControl )
{
	AgcmUIServerSelect* pThis = ( AgcmUIServerSelect* )pClass;
	if( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 ) return FALSE;

	INT32 nIndex = *( ( INT32* )pData );
	
	AgpdWorld* ppdWorld = ( AgpdWorld* )pThis->GetServerAgpdWorld( nIndex );
	if( ppdWorld )
	{
		sprintf( pDisplay, "%s", ppdWorld->m_szName );
		pControl->m_stDisplayMap.m_stFont.m_ulColor = 0xff8bfad1;

//#ifdef _DEBUG
//		CHAR strDebug[ 256 ] = { 0, };
//		sprintf( strDebug, "Display ServerName, Index = %d, Name = %s\n", nIndex, ppdWorld->m_szName );
//		OutputDebugString( strDebug );
//#endif
	}
	else
	{
		strcpy( pDisplay, pThis->GetServerGroupName( nIndex ) );
		pControl->m_stDisplayMap.m_stFont.m_ulColor = 0xffffffff;

//#ifdef _DEBUG
//		CHAR strDebug[ 256 ] = { 0, };
//		sprintf( strDebug, "Display ServerGroupName, Index = %d, GroupIndex = %d, Name = %s\n", nIndex, nGroupIndex, pDisplay );
//		OutputDebugString( strDebug );
//#endif
	}
	
	return TRUE;
}

BOOL AgcmUIServerSelect::CB_OnDisplayServerState( void* pClass, void* pData, AgcdUIDataType eType, INT32 lID, CHAR* pDisplay, INT32* pValue, AgcdUIControl* pControl )
{
	AgcmUIServerSelect* pThis = ( AgcmUIServerSelect* )pClass;
	if( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 ) return FALSE;

	INT32 nIndex = *( ( INT32* )pData );
	
	AgpdWorld* ppdWorld = ( AgpdWorld* )pThis->GetServerAgpdWorld( nIndex );
	if( ppdWorld )
	{
		CHAR* pStatus = NULL;

		switch( ppdWorld->m_nStatus )
		{
		case AGPDWORLD_STATUS_GOOOOOOD :
			{
				pStatus = pThis->GetUIMessage( UI_MESSAGE_ID_SERVERSTATUS_GOOD );
				pControl->m_pcsBase->m_lCurrentStatusID = 0;
			}
			break;

		case AGPDWORLD_STATUS_ABOVE_NORMAL :
			{
				pStatus = pThis->GetUIMessage( UI_MESSAGE_ID_SERVERSTATUS_ABOVE_NORMAL );
				pControl->m_pcsBase->m_lCurrentStatusID = 1;
			}
			break;

		case AGPDWORLD_STATUS_NORMAL :
			{
				pStatus = pThis->GetUIMessage( UI_MESSAGE_ID_SERVERSTATUS_NORMAL );
				pControl->m_pcsBase->m_lCurrentStatusID = 2;
			}
			break;

		case AGPDWORLD_STATUS_BELOW_NORMAL :
			{
				pStatus = pThis->GetUIMessage( UI_MESSAGE_ID_SERVERSTATUS_BELOW_NORMAL );
				pControl->m_pcsBase->m_lCurrentStatusID = 3;
			}
			break;

		case AGPDWORLD_STATUS_BAD :
			{
				pStatus = pThis->GetUIMessage( UI_MESSAGE_ID_SERVERSTATUS_BAD );
				pControl->m_pcsBase->m_lCurrentStatusID = 4;
			}
			break;

		default :
			{
				pStatus = pThis->GetUIMessage( UI_MESSAGE_ID_SERVERSTATUS_UNKNOWN );
				pControl->m_pcsBase->m_lCurrentStatusID = 4;
			}
			break;
		}

		if( pStatus )
		{
			sprintf( pDisplay, "%s", pStatus );
		}

//#ifdef _DEBUG
//		CHAR strDebug[ 256 ] = { 0, };
//		sprintf( strDebug, "Display ServerState, Index = %d, Msg = %s\n", nIndex, pStatus );
//		OutputDebugString( strDebug );
//#endif
	}
	else
	{
		strcpy( pDisplay, "" );

//#ifdef _DEBUG
//		CHAR strDebug[ 256 ] = { 0, };
//		sprintf( strDebug, "Cannot Display ServerState, Index = %d\n", nIndex );
//		OutputDebugString( strDebug );
//#endif
	}

	return TRUE;
}

BOOL AgcmUIServerSelect::CB_IsActiveBtnBack( void* pClass, void* pData, AgcdUIDataType eType, AgcdUIControl* pControl )
{
#ifdef USE_MFC
#elif _AREA_JAPAN_
	return FALSE;
#elif defined(_AREA_KOREA_)
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( !pWebzenAuth || pWebzenAuth->IsAutoLogin() ) return FALSE;
#endif
	return TRUE;
}

BOOL AgcmUIServerSelect::CB_IsClickedBtnTree( void* pClass, void* pData, AgcdUIDataType eType, AgcdUIControl* pControl )
{
	return TRUE;
}
