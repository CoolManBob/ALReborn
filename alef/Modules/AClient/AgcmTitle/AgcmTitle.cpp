#include "AgcmTitle.h"
#include "AgpmTitle.h"

#include "AgcmUIManager2.h"
#include "AgcmChatting2.h"
#include "AgcmTextBoardMng.h"
#include "AgcmIDBoard.h"
#include "AgcmHPBar.h"


#include "AuXmlParser/TinyXML/TinyXML.h"


#define UI_TITLE_WINDOWNAME_ALL				"UI_TitleAll"
#define UI_TITLE_WINDOWNAME_CURRUNT			"UI_TitleCurrent"
#define UI_TITLE_WINDOWNAME_MINI			"UI_TitleMini"


static AgcmTitle g_AgcmTitle;
AgcmTitle* GetAgcmTitle( void )
{
	return &g_AgcmTitle;
}



AgcmTitle::AgcmTitle( void )
{
	m_bIsInitialized = FALSE;

	m_pCurrentSelectCategory = NULL;
	m_pCurrentSelectTitle = NULL;
	m_pCurrentSelectTitleCurrent = NULL;
	m_pCurrentSelectTitleMini = NULL;
	m_pCurrentActivateTitle = NULL;

	m_nCurrentTitleListPage = 1;

	m_pEditTitlePage = NULL;
	m_pEditTitleName = NULL;
	m_pEditTitleInfo = NULL;
	m_pEditTitleEffect = NULL;
	m_pEditTitleNameCurrent = NULL;
	m_pEditTitleInfoCurrent = NULL;
	m_pEditTitleEffectCurrent = NULL;

	for( int nCount = 0 ; nCount < TITLELIST_BUTTON_PAGE_COUNT ; nCount++ )
	{
		m_pBtnListPage[ nCount ] = NULL;
	}

	m_pBtnSetTargetTitle = NULL;
	m_pBtnActivateTitle = NULL;
	m_pBtnCompleteTitle = NULL;

	m_pUserDataCategoryIndex = NULL;
	m_pUserDataTitleIndex = NULL;
	m_pUserDataTitleIndexCurrent = NULL;
	m_pUserDataTitleIndexMini = NULL;

	memset( m_nCategoryIndex, 0, sizeof( int ) * TITLELIST_CATEGOGRY_ITEMCOUNT_MAX );
	memset( m_nTitleIndex, 0, sizeof( int ) * TITLELIST_TITLE_ITEMCOUNT_MAX );
	memset( m_nTitleIndexCurrent, 0, sizeof( int ) * AGPDTITLE_MAX_TITLE_QUEST_NUMBER );
	memset( m_nTitleIndexMini, 0, sizeof( int ) * AGPDTITLE_MAX_TITLE_QUEST_NUMBER );
}

AgcmTitle::~AgcmTitle( void )
{
}

BOOL AgcmTitle::OnInitialize( void )
{
	if( m_bIsInitialized ) return TRUE;

	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	if( !ppmTitle ) return FALSE;

	ppmTitle->SetMaxTitleTemplate( 20000 );

	if( !ppmTitle->StreamReadTitleDataTemplate( "ini\\titledatatable.txt", TRUE ) )
	{
		ASSERT(!".\\ini\\titledatatable.txt");
		return FALSE;
	}

	if( !ppmTitle->StreamReadTitleStringTemplate( "ini\\titlestringtable.txt", TRUE ) )
	{
		ASSERT(!".\\ini\\titlestringtable.txt");
		return FALSE;
	}

	if( !_RegisterCallBack() )
	{
		ASSERT(!"Failed to AgcmTitle::_RegisterCallBack()!!");
		return FALSE;
	}

	if( !_GetDialogControls() )
	{
		ASSERT(!"Failed to AgcmTitle::_GetDialogControls()!!");
		return FALSE;
	}

	if( !_LoadAllTitleCategoryInfo() )
	{
		ASSERT(!"Invalid Title Category Info!!");
		return FALSE;
	}

	if( !OnLoadTitleSetting( "INI\\TitleSetting.xml" ) )
	{
		ASSERT(!"Failed to load Title settings. ( INI\\TitleSettings.xml )!!");
		return FALSE;
	}

	m_bIsInitialized = TRUE;
	return TRUE;
}

BOOL AgcmTitle::OnUnInitialize( void )
{
	return TRUE;
}

BOOL AgcmTitle::OnClearTitleState( void )
{
	// 모든 타이틀을 Ready 상태로 초기화시킨다.
	OnClearTitleStateAll();

	// 타이틀 IDBoard 를 초기화한다.
	OnClearTitleIDBoard();

	// 타이틀 UI 리스트 컨트롤들 초기화
	OnClearTitleCategorySelection();
	OnClearTitleSelection( TitleUi_ByNPC );
	OnClearTitleSelection( TitleUi_ByHotKey );
	OnClearTitleSelection( TitleUi_Mini );
	OnClearTitleDescription( TitleUi_ByNPC );
	OnClearTitleDescription( TitleUi_Mini );

	// 타이틀 리스트 페이지 초기화
	m_nCurrentTitleListPage = 1;
	return TRUE;
}

BOOL AgcmTitle::OnInitTitleSetting( void )
{
	memset( &m_stTitleTimeLimit, 0, sizeof( stTitleActivateTimeLimit ) );
	m_mapTitleFontColor.Clear();
	return TRUE;
}

BOOL AgcmTitle::OnLoadTitleSetting( char* pFileName )
{
	if( !OnInitTitleSetting() ) return FALSE;
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	TiXmlDocument Doc;
	if( !AgcmTitle::LoadXMLFileToDocument( &Doc, pFileName, TRUE ) ) return FALSE;

	TiXmlNode* pNodeRoot = Doc.FirstChild( "TitleSetting" );
	if( !pNodeRoot ) return FALSE;

	TiXmlNode* pNodeFontColor = pNodeRoot->FirstChild( "TitleFontColor" );
	if( pNodeFontColor )
	{
		TiXmlNode* pFontColor = pNodeFontColor->FirstChild( "FontColor" );
		while( pFontColor )
		{
			eTitleStateType eState = ( eTitleStateType )( atoi( pFontColor->ToElement()->Attribute( "StateIndex" ) ) );

			int nRed = atoi( pFontColor->ToElement()->Attribute( "Red" ) );
			int nGreen = atoi( pFontColor->ToElement()->Attribute( "Green" ) );
			int nBlue = atoi( pFontColor->ToElement()->Attribute( "Blue" ) );

			stTitleFontColorEntry NewEntry;

			NewEntry.m_eTitleState = eState;
			NewEntry.m_dwColor = ( 255 << 24 ) | ( nRed << 16 ) | ( nGreen << 8 ) | nBlue;

			m_mapTitleFontColor.Add( eState, NewEntry );			
			pFontColor = pFontColor->NextSibling();
		}
	}

	TiXmlNode* pNodeTitleTime = pNodeRoot->FirstChild( "TitleTimeSetting" );
	if( pNodeTitleTime )
	{
		TiXmlNode* pTimeLimit = pNodeTitleTime->FirstChild( "TimeLimit" );
		if( pTimeLimit )
		{
			m_stTitleTimeLimit.m_nYear		= atoi( pTimeLimit->ToElement()->Attribute( "Year" ) );
			m_stTitleTimeLimit.m_nMonth		= atoi( pTimeLimit->ToElement()->Attribute( "Month" ) );
			m_stTitleTimeLimit.m_nDay		= atoi( pTimeLimit->ToElement()->Attribute( "Day" ) );
			m_stTitleTimeLimit.m_nHour		= atoi( pTimeLimit->ToElement()->Attribute( "Hour" ) );
			m_stTitleTimeLimit.m_nMinute	= atoi( pTimeLimit->ToElement()->Attribute( "Minute" ) );
			m_stTitleTimeLimit.m_nSecond	= atoi( pTimeLimit->ToElement()->Attribute( "Second" ) );
		}
	}

	return TRUE;
}

BOOL AgcmTitle::OnOpenUI( eTitleUiType eUiType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 일단 창을 연다
	char* pUIName = NULL;
	switch( eUiType )
	{
	case TitleUi_ByNPC :	pUIName = UI_TITLE_WINDOWNAME_ALL;		break;
	case TitleUi_ByHotKey :	pUIName = UI_TITLE_WINDOWNAME_CURRUNT;	break;
	case TitleUi_Mini :		pUIName = UI_TITLE_WINDOWNAME_MINI;		break;
	default :				return FALSE;							break;
	}

	AgcdUI* pcdUI = pcmUIManager->GetUI( pUIName );
	if( !pcdUI ) return FALSE;
	if( !pcmUIManager->OpenUI( pcdUI ) ) return FALSE;

	// 컨트롤들 찾아놓고..
	_GetDialogControls();

	// 창별로 초기화 진행
	m_pCurrentSelectTitle = NULL;
	switch( eUiType )
	{
	case TitleUi_ByNPC :
		{
			OnClearTitleCategorySelection();
			OnClearTitleSelection( eUiType );
			OnClearTitleDescription( eUiType );
		}
		break;

	case TitleUi_ByHotKey :
		{
			OnClearTitleSelection( eUiType );
			OnClearTitleDescription( eUiType );
		}
		break;

	case TitleUi_Mini :
		{
			OnClearTitleSelection( eUiType );
		}
		break;

	default :
		{
			return FALSE;
		}
		break;
	}

	// 한번 업데이트 해준다. 창 초기화 대신으로다가..
	return OnUpateUI( eUiType );
}

BOOL AgcmTitle::OnUpateUI( eTitleUiType eUiType )
{
	if( !IsOpenUI( eUiType ) ) return TRUE;

	switch( eUiType )
	{
	case TitleUi_ByNPC :
		{
			// 카테고리 목록 갱신
			_UpdateCategoryList();

			// 타이틀 목록 갱신
			_UpdateTitleList( eUiType );

			// 현재 선택된 타이틀의 정보 갱신
			_UpdateTitleInfo( eUiType, m_pCurrentSelectTitle );

			// 현재 선택된 타이틀의 부가효과 정보 갱신
			_UpdateTitleEffect( eUiType, m_pCurrentSelectTitle );

			// 다이얼로그의 버튼상태 갱신
			_UpdateDialogButtonState( eUiType );
		}
		break;

	case TitleUi_ByHotKey :
		{
			// 타이틀 목록 갱신
			_UpdateTitleList( eUiType );

			// 현재 선택된 타이틀의 정보 갱신
			_UpdateTitleInfo( eUiType, m_pCurrentSelectTitleCurrent );

			// 현재 선택된 타이틀의 부가효과 정보 갱신
			_UpdateTitleEffect( eUiType, m_pCurrentSelectTitleCurrent );
		}
		break;

	case TitleUi_Mini :
		{
			// 타이틀 목록 갱신
			_UpdateTitleList( eUiType );
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::OnCloseUI( eTitleUiType eUiType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pUIName = NULL;
	switch( eUiType )
	{
	case TitleUi_ByNPC :	pUIName = UI_TITLE_WINDOWNAME_ALL;		break;
	case TitleUi_ByHotKey :	pUIName = UI_TITLE_WINDOWNAME_CURRUNT;	break;
	case TitleUi_Mini :		pUIName = UI_TITLE_WINDOWNAME_MINI;		break;
	default :				return FALSE;							break;
	}

	AgcdUI* pcdUI = pcmUIManager->GetUI( pUIName );
	if( !pcdUI ) return FALSE;
	if( !pcmUIManager->CloseUI( pcdUI ) ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::OnSendSetTargetTitle( void )
{
	if( !m_pCurrentSelectTitle ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	switch( m_pCurrentSelectTitle->m_eState )
	{
	case TitleState_Ready :
		{
			if( !_CheckIsMaxRunningTitle() ) return FALSE;
			if( !_CheckIsMaxEnableTitle() ) return FALSE;
			if( !_CheckIsMaxEnableAndRunningTitle() ) return FALSE;
			if( !_CheckIsValidTitleRequirement() ) return FALSE;

			// 획득 목표로 설정하겠냐고 확인 창을 하나 띄운다.
			char* pMessage = pcmUIManager->GetUIMessage( "ConfirmSetTargetTitle" );
			if( !pMessage || strlen( pMessage ) <= 0 ) return FALSE;

			// 획득목표로 설정 여부를 확인한다.
			if( !pcmUIManager->ActionMessageOKCancelDialog( pMessage ) ) return FALSE;

			// 선택된 타이틀이 아직 목표로 설정되지 않은 상태라면 목표로 설정하겠다는 패킷을 보낸다.
#ifdef _DEBUG
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_QUEST_REQUEST ), Title = %s\n", m_pCurrentSelectTitle->m_strName );
			OutputDebugString( strDebug );
#endif

			PACKET_AGPPTITLE_QUEST_REQUEST Packet( ppdCharacter->m_szID, m_pCurrentSelectTitle->m_nTitleID );
			g_pEngine->SendPacket( Packet );
		}
		break;

	case TitleState_Enable :
		{
			// 완료되어 사용가능한 상태인 타이틀의 경우에는 타이틀 삭제할거냐고 물어본다.
			char* pConfirmMsg = pcmUIManager->GetUIMessage( "ConfirmDeleteTitle" );
			if( !pConfirmMsg || strlen( pConfirmMsg ) <= 0 ) return FALSE;

			// 타이틀 삭제 여부를 확인한다.
			if( !pcmUIManager->ActionMessageOKCancelDialog( pConfirmMsg ) ) return FALSE;

			// 서버로 삭제요청 패킷을 보낸다.
#ifdef _DEBUG
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_DELETE ), Title = %s\n", m_pCurrentSelectTitle->m_strName );
			OutputDebugString( strDebug );
#endif

			PACKET_AGPPTITLE_DELETE Packet( ppdCharacter->m_szID, m_pCurrentSelectTitle->m_nTitleID );
			g_pEngine->SendPacket( Packet );
		}
		break;

	default :
		{
			// 그 외의 상태에서는 아무짓도 하면 안된다.
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::OnSendTitleActivate( void )
{
	if( !m_pCurrentSelectTitle ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 선택한 타이틀의 장착상태에 따라서..
	switch( m_pCurrentSelectTitle->m_eState )
	{
	case TitleState_Enable :
		{
			// 장착가능한 상태인 경우에는 장착할거냐고 물어본다.
			char* pConfirmMsg = pcmUIManager->GetUIMessage( "ConfirmActivateTitle" );
			if( !pConfirmMsg || strlen( pConfirmMsg ) <= 0 ) return FALSE;

			//// 2시간 이내에는 타이틀을 바꿔달지 못한다.
			//if( !_CheckTitleExchangeTimeLimit( ppdCharacter->m_szID ) )
			//{
			//	char* pErrMessageFormat = pcmUIManager->GetUIMessage( "TitleErrorMsg_CannotExChangeTitleIn2Hour" );
			//	if( pErrMessageFormat && strlen( pErrMessageFormat ) > 0 )
			//	{
			//		int nMonth = 0;
			//		int nDay = 0;
			//		int nHour = 0;
			//		int nMinute = 0;

			//		unsigned int nLastActivateTime = _GetLastActivateTime( ppdCharacter->m_szID );
			//		_ParseTimeStamp( nLastActivateTime, NULL, &nMonth, &nDay, &nHour, &nMinute, NULL );

			//		char strErrorMsg[ 256 ] = { 0, };
			//		sprintf_s( strErrorMsg, sizeof( char ) * 256, pErrMessageFormat, nMonth, nDay, nHour, nMinute );
			//		pcmUIManager->ActionMessageOKDialog( strErrorMsg );
			//	}

			//	return FALSE;
			//}

			// 장착 여부를 확인한다.
			if( !pcmUIManager->ActionMessageOKCancelDialog( pConfirmMsg ) ) return FALSE;

			// 서버로 장착요청 패킷을 보낸다.
#ifdef _DEBUG
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_USE, TRUE ), Title = %s\n", m_pCurrentSelectTitle->m_strName );
			OutputDebugString( strDebug );
#endif

			PACKET_AGPPTITLE_USE Packet( ppdCharacter->m_szID, m_pCurrentSelectTitle->m_nTitleID, AGPMTITLE_PACKET_OPERATION_TITLE_USE, TRUE );
			g_pEngine->SendPacket( Packet );
		}
		break;

	case TitleState_Activate :
		{
			// 장착중인 타이틀은 장착해제할 거냐고 물어본다.
			char* pConfirmMsg = pcmUIManager->GetUIMessage( "ConfirmDeActivateTitle" );
			if( !pConfirmMsg || strlen( pConfirmMsg ) <= 0 ) return FALSE;

			// 장착해제 여부를 확인한다.
			if( !pcmUIManager->ActionMessageOKCancelDialog( pConfirmMsg ) ) return FALSE;

			// 서버로 장착해제요청 패킷을 보낸다.
#ifdef _DEBUG
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_USE, FALSE ), Title = %s\n", m_pCurrentSelectTitle->m_strName );
			OutputDebugString( strDebug );
#endif

			PACKET_AGPPTITLE_USE Packet( ppdCharacter->m_szID, m_pCurrentSelectTitle->m_nTitleID, AGPMTITLE_PACKET_OPERATION_TITLE_USE, FALSE );
			g_pEngine->SendPacket( Packet );
		}
		break;

	default :
		{
			// 위의 2가지 상태 이외에는 아무것도 하지 않는다.. 
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::OnSendSurrenderTitle( eTitleUiType eUiType )
{
	stTitleEntry* pDeleteTitle = NULL;

	switch( eUiType )
	{
	case TitleUi_ByNPC :	pDeleteTitle = m_pCurrentSelectTitle;			break;
	case TitleUi_ByHotKey :	pDeleteTitle = m_pCurrentSelectTitleCurrent;	break;
	case TitleUi_Mini :		pDeleteTitle = m_pCurrentSelectTitleMini;		break;
	}

	if( !pDeleteTitle ) return FALSE;

	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmTitle || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pSurrenderMsg = pcmUIManager->GetUIMessage( "ConfirmSurrenderTitle" );
	if( !pSurrenderMsg || strlen( pSurrenderMsg ) <= 0 ) return FALSE;

	// 포기 여부를 확인한다.
	if( !pcmUIManager->ActionMessageOKCancelDialog( pSurrenderMsg ) ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_DELETE ), Title = %s\n", pDeleteTitle->m_strName );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_DELETE Packet( ppdCharacter->m_szID, pDeleteTitle->m_nTitleID );
	g_pEngine->SendPacket( Packet );
	return TRUE;
}

BOOL AgcmTitle::OnSendRequestTitleList( void )
{
	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmTitle || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 진행중인 타이틀 목록 요청
	PACKET_AGPPTITLE_QUEST_LIST PacketRequestTitleQuest( ppdCharacter->m_szID );
	g_pEngine->SendPacket( PacketRequestTitleQuest );

	// 사용가능한 타이틀 목록 요청
#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_LIST ), Character = %s\n", ppdCharacter->m_szID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_LIST PacketRequestTitleEnable( ppdCharacter->m_szID );
	g_pEngine->SendPacket( PacketRequestTitleEnable );
	return TRUE;
}

BOOL AgcmTitle::OnSendCompleteTitle( void )
{
	if( !m_pCurrentSelectTitle ) return FALSE;

	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmTitle || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	if( m_pCurrentSelectTitle->m_eState != TitleState_Complete )
	{
		// 아직 완료상태가 아니다.. 한번 검사해본다..
		if( !ppmTitle->SatisfyTitleQuestCompleteCondition( ppdCharacter, m_pCurrentSelectTitle->m_nTitleID ) )
		{
			// 검사도 해봤는데 역시나 아직 완료 안됬다.. 에러 메세지 출력하고 나간다.
			AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
			if( !pcmUIManager ) return FALSE;

			char* pErrorMsg = pcmUIManager->GetUIMessage( "TitleErrorMsg_NotCompletedYet" );
			if( !pErrorMsg || strlen( pErrorMsg ) <= 0 ) return FALSE;

			pcmUIManager->ActionMessageOKDialog( pErrorMsg );
			return FALSE;
		}

		// 검사결과 완료되었다면 상태를 바꿔준다.
		OnChangeTitleState( m_pCurrentSelectTitle->m_nTitleID, TitleState_Complete, ppdCharacter );
	}

	// 타이틀 완료 요청
#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "[ Title ] SendPacket( PACKET_AGPPTITLE_QUEST_COMPLETE ), Character = %s\n", m_pCurrentSelectTitle->m_strName );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_QUEST_COMPLETE PacketRequestTitleComplete( ppdCharacter->m_szID, m_pCurrentSelectTitle->m_nTitleID );
	g_pEngine->SendPacket( PacketRequestTitleComplete );
	return TRUE;
}

BOOL AgcmTitle::OnChangeTitleState( int nTitleID, eTitleStateType eState, void* pCharacter, BOOL bIsSelfCharacter, BOOL bResult, int nCounterValue )
{
	// 내 캐릭터가 아니라면 TitleState_Activate 상태변화만 처리한다.
	if( !bIsSelfCharacter && eState != TitleState_Activate ) return FALSE;

	stTitleEntry* pTitle = _GetTitleByID( nTitleID );
	if( !pTitle ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "[ Title ] Try Title state change( Title = %s, From = %d, To = %d ).....", pTitle->m_strName, pTitle->m_eState, eState );
	OutputDebugString( strDebug );
#endif

	pTitle->m_eState = eState;
	switch( eState )
	{
	case TitleState_Running :
	case TitleState_Complete :
		{
			// 현재 진행중인 타이틀 목록에 추가한다.
			_AddRunningTitle( pTitle );

			// 카운터 수치가 들어왔으면 카운터수치를 업데이트 한다.
			if( nCounterValue > 0 )
			{
				OnUpdateTitleCounter( nTitleID, nCounterValue );
			}
		}
		break;

	case TitleState_Activate :
		{
			if( bResult )
			{
				OnActivateTitle( pTitle, pCharacter, bIsSelfCharacter );
			}
			else
			{
				OnDeActivateTitle( pTitle, pCharacter, bIsSelfCharacter );
			}
		}
		break;

	default :
		{
			// 현재 진행중인 타이틀 목록에서 제거한다.
			_DeleteRunningTitle( nTitleID );
		}
		break;
	}

	// 아래의 작업은 내 캐릭터에게만 한다.
	if( bIsSelfCharacter )
	{
		if( nCounterValue >= 0 )
		{
			pTitle->m_nCounterCur = nCounterValue;
			if( pTitle->m_nCounterCur > pTitle->m_nCounterMax )
			{
				pTitle->m_nCounterCur = pTitle->m_nCounterMax;
			}
		}

		OnUpateUI( TitleUi_ByNPC );
		OnUpateUI( TitleUi_ByHotKey );
		OnUpateUI( TitleUi_Mini );
	}

#ifdef _DEBUG
	OutputDebugString( "Complete\n" );
#endif

	return TRUE;
}

BOOL AgcmTitle::OnUpdateTitleCounter( int nTitleID, int nCounterValue )
{
	stTitleEntry* pTitle = _GetTitleByID( nTitleID );
	if( pTitle && pTitle->m_nCounterMax > 0 )
	{
		pTitle->m_nCounterCur = nCounterValue;
		if( pTitle->m_nCounterCur > pTitle->m_nCounterMax )
		{
			pTitle->m_nCounterCur = pTitle->m_nCounterMax;
		}
	}

	return TRUE;
}

BOOL AgcmTitle::OnUpdateTitleTimeStamp( char* pCharacterName, int nTitleID, unsigned int nTimeStamp )
{
	stTitleEntry* pTitle = _GetTitleByID( nTitleID );
	if( !pTitle ) return FALSE;

	pTitle->m_nTimeStamp = nTimeStamp;

	// 가장 최근의 타임스탬프를 저장
	_SaveTitleActivateLog( pCharacterName, nTimeStamp );
	return TRUE;
}

BOOL AgcmTitle::OnSelectCategory( char* pCategoryName )
{
	stTitleCategory* pCategory = _GetCategoryByName( pCategoryName );
	if( !pCategory ) return FALSE;

	OnClearTitleSelection( TitleUi_ByNPC );
	OnClearTitleDescription( TitleUi_ByNPC );

	m_pCurrentSelectCategory = pCategory;
	m_nCurrentTitleListPage = 1;

	return _UpdateTitleList( TitleUi_ByNPC );
}

BOOL AgcmTitle::OnSelectCategoryByID( int nID )
{
	stTitleCategory* pCategory = _GetCategoryByID( nID );
	if( !pCategory ) return FALSE;

	OnClearTitleSelection( TitleUi_ByNPC );
	OnClearTitleDescription( TitleUi_ByNPC );

	m_pCurrentSelectCategory = pCategory;
	m_nCurrentTitleListPage = 1;

	return _UpdateTitleList( TitleUi_ByNPC );
}

BOOL AgcmTitle::OnSelectCategoryByIndex( int nIndex )
{
	stTitleCategory* pCategory = _GetCategoryByIndex( nIndex );
	if( !pCategory ) return FALSE;

	OnClearTitleSelection( TitleUi_ByNPC );
	OnClearTitleDescription( TitleUi_ByNPC );

	m_pCurrentSelectCategory = pCategory;
	m_nCurrentTitleListPage = 1;

	return _UpdateTitleList( TitleUi_ByNPC );
}

BOOL AgcmTitle::OnSelectTitle( char* pTitleName )
{
	m_pCurrentSelectTitle = _GetTitleByName( m_pCurrentSelectCategory, pTitleName );
	if( !m_pCurrentSelectTitle ) return FALSE;

	_UpdateTitleInfo( TitleUi_ByNPC, m_pCurrentSelectTitle );
	_UpdateTitleEffect( TitleUi_ByNPC, m_pCurrentSelectTitle );
	return TRUE;
}

BOOL AgcmTitle::OnSelectTitleByID( int nID )
{
	m_pCurrentSelectTitle = _GetTitleByID( m_pCurrentSelectCategory, nID );
	if( !m_pCurrentSelectTitle ) return FALSE;

	_UpdateTitleInfo( TitleUi_ByNPC, m_pCurrentSelectTitle );
	_UpdateTitleEffect( TitleUi_ByNPC, m_pCurrentSelectTitle );
	return TRUE;
}

BOOL AgcmTitle::OnSelectTitleByIndex( int nIndex )
{
	int nFindIndex = ( ( m_nCurrentTitleListPage - 1 ) * TiTLELIST_TITLE_ITEMCOUNT_PER_PAGE ) + nIndex;
	m_pCurrentSelectTitle = _GetTitleByIndex( m_pCurrentSelectCategory, nFindIndex );
	if( !m_pCurrentSelectTitle ) return FALSE;

	// 타이틀 정보 업데이트
	_UpdateTitleInfo( TitleUi_ByNPC, m_pCurrentSelectTitle );

	// 타이틀 부가효과정보 업데이트
	_UpdateTitleEffect( TitleUi_ByNPC, m_pCurrentSelectTitle );

	// 선택된 타이틀에 따라 버튼 활성 / 비활성 처리
	_UpdateDialogButtonState( TitleUi_ByNPC );
	return TRUE;
}

BOOL AgcmTitle::OnSelectTitleCurrentByIndex( int nIndex )
{
	m_pCurrentSelectTitleCurrent = _GetTitleCurrentByIndex( nIndex );
	if( !m_pCurrentSelectTitleCurrent ) return FALSE;

	_UpdateTitleInfo( TitleUi_ByHotKey, m_pCurrentSelectTitleCurrent );
	_UpdateTitleEffect( TitleUi_ByHotKey, m_pCurrentSelectTitleCurrent );
	return TRUE;
}

BOOL AgcmTitle::OnSelectTitleMiniByIndex( int nIndex )
{
	m_pCurrentSelectTitleMini = _GetTitleCurrentByIndex( nIndex );
	if( !m_pCurrentSelectTitleMini ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::OnSetSelfCharacter( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	if( m_pCurrentActivateTitle )
	{
		DWORD dwColor = GetTitleFontColor( TitleState_Activate );
		OnUpdateTitleTextBoard( ppdCharacter, m_pCurrentActivateTitle->m_strName, dwColor, TRUE );
	}

	return TRUE;
}

BOOL AgcmTitle::OnActivateTitle( stTitleEntry* pTitle, void* pCharacter, BOOL bIsSelfCharacter )
{
	if( !pTitle ) return FALSE;

	// 내 캐릭터만 세부 타이틀 상태검사 등을 수행한다.
	if( bIsSelfCharacter )
	{
		// 현재 활성화된 타이틀로 설정한다.
		m_pCurrentActivateTitle = pTitle;
		m_pCurrentActivateTitle->m_eState = TitleState_Activate;
	}

	// IDBoard 의 타이틀명을 교체한다.
	DWORD dwColor = GetTitleFontColor( TitleState_Activate );
	OnUpdateTitleTextBoard( pCharacter, pTitle->m_strName, dwColor, bIsSelfCharacter );

	// 캐릭터에게 설정된 것을 저장해둔다.
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter || strlen( ppdCharacter->m_szID ) <= 0 ) return FALSE;

	stCharacterTitleActivate* pCharacterTitle = m_mapCharacterTitleActivate.Get( ppdCharacter->m_szID );
	if( !pCharacterTitle )
	{
		stCharacterTitleActivate NewTitle;

		NewTitle.m_nCharacterID = ppdCharacter->m_lID;
		NewTitle.m_nTitleID = pTitle->m_nTitleID;
		strcpy_s( NewTitle.m_strCharacterName, sizeof( char ) * 64, ppdCharacter->m_szID );

		m_mapCharacterTitleActivate.Add( ppdCharacter->m_szID, NewTitle );
	}
	else
	{
		pCharacterTitle->m_nTitleID = pTitle->m_nTitleID;
	}

	return TRUE;
}

BOOL AgcmTitle::OnActivateTitle( int nTitleID, void* pCharacter, BOOL bIsSelfCharacter )
{
	stTitleEntry* pTitle = _GetTitleByID( nTitleID );
	return OnActivateTitle( pTitle, pCharacter, bIsSelfCharacter );
}

BOOL AgcmTitle::OnDeActivateTitle( stTitleEntry* pTitle, void* pCharacter, BOOL bIsSelfCharacter )
{
	// pTitle 은 NULL 일수도 있으며 정상적인 경우이다. NULL 체크하면 안됨
	// 내 캐릭터만 세부 타이틀 상태검사 등을 수행한다.
	if( bIsSelfCharacter )
	{
		// 현재 장착중인 타이틀을 Enable 상태로 전환시키고..
		if( m_pCurrentActivateTitle )
		{
			m_pCurrentActivateTitle->m_eState = TitleState_Enable;

			// 현재 활성화된 타이틀을 초기화한다.
			m_pCurrentActivateTitle = NULL;
		}
	}

	// IDBoard 의 타이틀명을 초기화한다.
	OnUpdateTitleTextBoard( pCharacter, "", 0xFFFFFFFF, bIsSelfCharacter );

	// 타이틀 활성목록에서 삭제한다.
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	m_mapCharacterTitleActivate.Delete( ppdCharacter->m_szID );
	return TRUE;
}

BOOL AgcmTitle::OnDeActivateTitle( int nTitleID, void* pCharacter, BOOL bIsSelfCharacter )
{
	stTitleEntry* pTitle = _GetTitleByID( nTitleID );
	return OnDeActivateTitle( pTitle, pCharacter, bIsSelfCharacter );
}

BOOL AgcmTitle::_RegisterCallBack( void )
{
	// Packet CallBack..
	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	if( !ppmTitle ) return FALSE;

	if( !ppmTitle->SetCallBackTitleAddResult( CallBack_RecvTitleAdd, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleUseResult( CallBack_RecvTitleUse, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleListResult( CallBack_RecvTitleList, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleDeleteResult( CallBack_RecvTitleDelete, this ) ) return FALSE;

	if( !ppmTitle->SetCallBackTitleQuestRequestResult( CallBack_RecvTitleQuestRequest, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleQuestCheckResult( CallBack_RecvTitleQuestCheck, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleQuestCompleteResult( CallBack_RecvTitleQuestComplete, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleQuestListResult( CallBack_RecvTitleQuestList, this ) ) return FALSE;

	if( !ppmTitle->SetCallBackTitleUseNear( CallBack_RecvTitleUseNear, this ) ) return FALSE;
	if( !ppmTitle->SetCallBackTitleUIOpen( CallBack_RecvTitleOpenUI, this ) ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;
	if( !pcmCharacter->SetCallbackSetSelfCharacter( CallBack_OnRecvSelfCharacter, this ) ) return FALSE;
	if( !pcmCharacter->SetCallbackSelfUpdatePosition( CallBack_AutoCloseByCharacterMove, this ) ) return FALSE;

	// UI CallBack
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	for( int nCount = 0 ; nCount < TITLELIST_CATEGOGRY_ITEMCOUNT_MAX ; nCount++ )
	{
		m_nCategoryIndex[ nCount ] = nCount;
	}

	m_pUserDataCategoryIndex = pcmUIManager->AddUserData( "UserData_Title_CategoryIndex", &m_nCategoryIndex[ 0 ], sizeof( int ), TITLELIST_CATEGOGRY_ITEMCOUNT_MAX, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pUserDataCategoryIndex ) return FALSE;

	for( int nCount = 0 ; nCount < TITLELIST_TITLE_ITEMCOUNT_MAX ; nCount++ )
	{
		m_nTitleIndex[ nCount ] = nCount;
	}

	m_pUserDataTitleIndex = pcmUIManager->AddUserData( "UserData_Title_TitleIndex", &m_nTitleIndex[ 0 ], sizeof( int ), TITLELIST_TITLE_ITEMCOUNT_MAX, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pUserDataTitleIndex ) return FALSE;

	for( int nCount = 0 ; nCount < AGPDTITLE_MAX_TITLE_QUEST_NUMBER ; nCount++ )
	{
		m_nTitleIndexCurrent[ nCount ] = nCount;
	}

	m_pUserDataTitleIndexCurrent = pcmUIManager->AddUserData( "UserData_Title_TitleIndexCurrent", &m_nTitleIndexCurrent[ 0 ], sizeof( int ), AGPDTITLE_MAX_TITLE_QUEST_NUMBER, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pUserDataTitleIndexCurrent ) return FALSE;

	for( int nCount = 0 ; nCount < AGPDTITLE_MAX_TITLE_QUEST_NUMBER ; nCount++ )
	{
		m_nTitleIndexMini[ nCount ] = nCount;
	}

	m_pUserDataTitleIndexMini = pcmUIManager->AddUserData( "UserData_Title_TitleIndexMini", &m_nTitleIndexMini[ 0 ], sizeof( int ), AGPDTITLE_MAX_TITLE_QUEST_NUMBER, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pUserDataTitleIndexMini ) return FALSE;

	if( !pcmUIManager->AddDisplay( this, "UiDialog_Title_DisplayCategoryName", 0, CallBack_DisplayCategoryName, AGCDUI_USERDATA_TYPE_INT32 ) ) return FALSE;
	if( !pcmUIManager->AddDisplay( this, "UiDialog_Title_DisplayTitleName", 0, CallBack_DisplayTitleName, AGCDUI_USERDATA_TYPE_INT32 ) ) return FALSE;
	if( !pcmUIManager->AddDisplay( this, "UiDialog_Title_DisplayTitleNameCurrent", 0, CallBack_DisplayTitleNameCurrent, AGCDUI_USERDATA_TYPE_INT32 ) ) return FALSE;
	if( !pcmUIManager->AddDisplay( this, "UiDialog_Title_DisplayTitleNameMini", 0, CallBack_DisplayTitleNameMini, AGCDUI_USERDATA_TYPE_INT32 ) ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnOpenUIAll", CallBack_OnOpenUIAll, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnOpenUICurrent", CallBack_OnOpenUICurrent, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnOpenUIMini", CallBack_OnOpenUIMini, 0 ) ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickCategory", CallBack_OnClickCategoryItem, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickTitle", CallBack_OnClickTitleItem, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickTitleCurrent", CallBack_OnClickTitleItemCurrent, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickTitleMini", CallBack_OnClickTitleItemMini, 0 ) ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickPagePrev", CallBack_OnClickPagePrev, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickPageNext", CallBack_OnClickPageNext, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickPage", CallBack_OnClickPage, 0 ) ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickSetTargetTitle", CallBack_OnClickSetTargetTitle, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickActivateTitle", CallBack_OnClickActivateTitle, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickSurrenderTitleCurrent", CallBack_OnClickSurrenderTitleCurrent, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickSurrenderTitleMini", CallBack_OnClickSurrenderTitleMini, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UiDialog_Title_OnClickCompleteTitle", CallBack_OnClickCompleteTitle, 0 ) ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_GetDialogControls( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUITitleAll = pcmUIManager->GetUI( "UI_TitleAll" );
	if( pcdUITitleAll )
	{
		// 타이틀 카테고리 리스트의 아이템 갯수를 카테고리 수로 지정한다.
		AgcdUIControl* pcdControl_ListCategory = pcmUIManager->GetControl( pcdUITitleAll, "List_TitleCategory" );
		if( pcdControl_ListCategory )
		{
			AcUIList* pList = ( AcUIList* )pcdControl_ListCategory->m_pcsBase;
			if( pList )
			{
				int nCategoryCount = m_mapCategory.GetSize();
				pList->SetListItemWindowTotalNum( nCategoryCount );
			}
		}

		m_pEditTitlePage = pcmUIManager->GetControl( pcdUITitleAll, "Edit_TitlePage" );
		m_pEditTitleName = pcmUIManager->GetControl( pcdUITitleAll, "Edit_TitleName" );
		m_pEditTitleInfo = pcmUIManager->GetControl( pcdUITitleAll, "Edit_TitleInfo" );
		m_pEditTitleEffect = pcmUIManager->GetControl( pcdUITitleAll, "Edit_TitleEffect" );

		for( int nCount = 0 ; nCount < TITLELIST_BUTTON_PAGE_COUNT ; nCount++ )
		{
			char strBtnName[ 64 ] = { 0, };
			sprintf_s( strBtnName, sizeof( char ) * 64, "Button_ListPage%d", nCount + 1 );
			m_pBtnListPage[ nCount ] = pcmUIManager->GetControl( pcdUITitleAll, strBtnName );
		}

		m_pBtnSetTargetTitle = pcmUIManager->GetControl( pcdUITitleAll, "Button_SetTargetTitle" );
		m_pBtnActivateTitle = pcmUIManager->GetControl( pcdUITitleAll, "Button_ActivateTitle" );
		m_pBtnCompleteTitle = pcmUIManager->GetControl( pcdUITitleAll, "Button_CompleteTitle" );
	}

	AgcdUI* pcdUITitleCurrent = pcmUIManager->GetUI( "UI_TitleCurrent" );
	if( pcdUITitleCurrent )
	{
		m_pEditTitleNameCurrent = pcmUIManager->GetControl( pcdUITitleCurrent, "Edit_TitleName" );
		m_pEditTitleInfoCurrent = pcmUIManager->GetControl( pcdUITitleCurrent, "Edit_TitleInfo" );
		m_pEditTitleEffectCurrent = pcmUIManager->GetControl( pcdUITitleCurrent, "Edit_TitleEffect" );
	}

	AgcdUI* pcdUITitleMini = pcmUIManager->GetUI( "UI_TitleMini" );
	if( pcdUITitleMini )
	{
	}

	return TRUE;
}

BOOL AgcmTitle::_LoadAllTitleCategoryInfo( void )
{
	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	if( !ppmTitle ) return FALSE;

	VectorTitleCategory::iterator Iter = ppmTitle->m_stAgpaTitleCategory.Begin();
	int nCategoryCount = ppmTitle->m_stAgpaTitleCategory.TitleCategoryCount;
	for( int nCount = 0 ; nCount < nCategoryCount ; nCount++ )
	{
		AgpdTitleCategory* pCategory = ( AgpdTitleCategory* )&*Iter;
		if( pCategory )
		{
			stTitleCategory NewCategory;

			strcpy_s( NewCategory.m_strName, sizeof( char ) * STRING_LENGTH_TITLE_NAME, pCategory->m_szTitleClass );
			NewCategory.m_nCategoryID = pCategory->m_nFirstCategory;

			vectorTitleStringTemplate vecTitle = ppmTitle->m_stAgpaTitleStringTemplate.FindByCategoryID( pCategory->m_nFirstCategory );

			vectorTitleStringTemplate::iterator IterTitle = vecTitle.begin();
			while( IterTitle != vecTitle.end() )
			{
				AgpdTitleStringTemplate* pTitleString = ( AgpdTitleStringTemplate* )&*IterTitle;
				if( pTitleString )
				{
					stTitleEntry NewTitle;

					NewTitle.m_nTitleID = pTitleString->m_nTitleTid;
					NewTitle.m_eState = TitleState_Ready;

					AgpdTitleTemplate* ppdTitleITemplate = ppmTitle->m_stAgpaTitleTemplate.Get( NewTitle.m_nTitleID );
					if( ppdTitleITemplate )
					{
						if( ppdTitleITemplate->m_vtTitleCheck[ 0 ].m_nTitleCheckType != 0 )
						{
							NewTitle.m_nCounterMax = ppdTitleITemplate->m_vtTitleCheck[ 0 ].m_nTitleCheckValue;
						}
					}

					strcpy_s( NewTitle.m_strName, sizeof( char ) * STRING_LENGTH_TITLE_NAME, pTitleString->m_szTitleName );
					strcpy_s( NewTitle.m_strDesc, sizeof( char ) * STRING_LENGTH_TITLE_DESC, pTitleString->m_szTitleNecessaryConditionDesc );
					strcpy_s( NewTitle.m_strDescEffect, sizeof( char ) * STRING_LENGTH_TITLE_DESC, pTitleString->m_szTitleEffectDesc );

					NewCategory.m_mapTitle.Add( NewTitle.m_nTitleID, NewTitle );

#ifdef _DEBUG
					char strDebug[ 256 ] = { 0, };
					sprintf_s( strDebug, 256, "--- Add Title : CategoryID = %d, CatetoryName = %s, TitleID = %d, TitleName = %s\n",
						NewCategory.m_nCategoryID, NewCategory.m_strName, NewTitle.m_nTitleID, NewTitle.m_strName );
					OutputDebugString( strDebug );
#endif
				}

				IterTitle++;
			}

			m_mapCategory.Add( NewCategory.m_nCategoryID, NewCategory );

#ifdef _DEBUG
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, 256, "--- Add TitleCategory : CategoryID = %d, CatetoryName = %s\n", NewCategory.m_nCategoryID, NewCategory.m_strName );
			OutputDebugString( strDebug );
#endif
		}

		Iter = ppmTitle->m_stAgpaTitleCategory.Next();
	}

	return TRUE;
}

stTitleCategory* AgcmTitle::_GetCategoryByID( int nCategoryID )
{
	return m_mapCategory.Get( nCategoryID );
}

stTitleCategory* AgcmTitle::_GetCategoryByName( char* pCategoryName )
{
	if( !pCategoryName || strlen( pCategoryName ) <= 0 ) return NULL;

	int nCategoryCount = m_mapCategory.GetSize();
	for( int nCount = 0 ; nCount < nCategoryCount ; nCount++ )
	{
		stTitleCategory* pCategory = m_mapCategory.GetByIndex( nCount );
		if( pCategory && strcmp( pCategory->m_strName, pCategoryName ) == 0 )
		{
			return pCategory;
		}
	}

	return NULL;
}

stTitleCategory* AgcmTitle::_GetCategoryByIndex( int nIndex )
{
	return m_mapCategory.GetByIndex( nIndex );
}

stTitleEntry* AgcmTitle::_GetTitleByID( int nTitleID )
{
	int nCategoryCount = m_mapCategory.GetSize();
	for( int nCount = 0 ; nCount < nCategoryCount ; nCount++ )
	{
		stTitleCategory* pCategory = m_mapCategory.GetByIndex( nCount );
		if( pCategory )
		{
			stTitleEntry* pTitle = _GetTitleByID( pCategory, nTitleID );
			if( pTitle )
			{
				return pTitle;
			}
		}
	}

	return NULL;
}

stTitleEntry* AgcmTitle::_GetTitleByID( stTitleCategory* pCategory, int nTitleID )
{
	if( !pCategory ) return NULL;
	return pCategory->m_mapTitle.Get( nTitleID );
}

stTitleEntry* AgcmTitle::_GetTitleByName( stTitleCategory* pCategory, char* pTitleName )
{
	if( !pTitleName || strlen( pTitleName ) <= 0 ) return NULL;

	int nTitleCount = pCategory->m_mapTitle.GetSize();
	for( int nCount = 0 ; nCount < nTitleCount ; nCount++ )
	{
		stTitleEntry* pTitle = pCategory->m_mapTitle.GetByIndex( nCount );
		if( pTitle && strcmp( pTitle->m_strName, pTitleName ) == 0 )
		{
			return pTitle;
		}
	}

	return NULL;
}

stTitleEntry* AgcmTitle::_GetTitleByIndex( stTitleCategory* pCategory, int nIndex )
{
	if( !pCategory ) return NULL;
	return pCategory->m_mapTitle.GetByIndex( nIndex );
}

stTitleEntry* AgcmTitle::_GetTitleCurrentByIndex( int nIndex )
{
	stTitleRunningEntry* pRunningEntry = m_vecRunningTitle.Get( nIndex );
	if( !pRunningEntry ) return NULL;
	return pRunningEntry->m_pTitle;
}

void AgcmTitle::_ParseTimeStamp( unsigned int nTime, int* pYear, int* pMonth, int* pDay, int* pHour, int* pMinute, int* pSecond )
{
	time_t TimeValue;
	struct tm* pTime = NULL;

	TimeValue = ( time_t )nTime;
	pTime = localtime( &TimeValue );
	if( !pTime ) return;

	if( pYear )		*pYear		= pTime->tm_year;
	if( pMonth )	*pMonth		= pTime->tm_mon + 1;
	if( pDay )		*pDay		= pTime->tm_mday;
	if( pHour )		*pHour		= pTime->tm_hour;
	if( pMinute )	*pMinute	= pTime->tm_min;
	if( pSecond )	*pSecond	= pTime->tm_sec;
}

unsigned int AgcmTitle::_GetLastActivateTime( char* pCharacterName )
{
	if( !pCharacterName || strlen( pCharacterName ) <= 0 ) return 0;

	stTitleActivationLog* pLog = m_mapActivateLog.Get( pCharacterName );
	if( !pLog ) return 0;

	return pLog->m_nActivateTime;
}

void AgcmTitle::_SaveTitleActivateLog( char* pCharacterName, unsigned int nTimeStamp )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return;
	if( !pCharacterName || strlen( pCharacterName ) <= 0 ) return;

	stTitleActivationLog* pLog = m_mapActivateLog.Get( pCharacterName );
	if( !pLog )
	{
		stTitleActivationLog NewLog;

		strcpy_s( NewLog.m_strCharacterName, sizeof( char ) * 64, pCharacterName );
		NewLog.m_nActivateTime = nTimeStamp;

		m_mapActivateLog.Add( pCharacterName, NewLog );
	}
	else
	{
		if( pLog->m_nActivateTime < nTimeStamp )
		{
			pLog->m_nActivateTime = nTimeStamp;
		}		
	}
}

BOOL AgcmTitle::_UpdateCategoryList( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;
	
	if( m_pUserDataCategoryIndex )
	{
		m_pUserDataCategoryIndex->m_bUpdateList = TRUE;
		pcmUIManager->RefreshUserData( m_pUserDataCategoryIndex, TRUE );
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateTitleList( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			// 리스트 갱신
			if( m_pUserDataTitleIndex )
			{
				pcmUIManager->RefreshUserData( m_pUserDataTitleIndex, TRUE );
			}

			_UpdateTitleListPage();
		}
		break;

	case TitleUi_ByHotKey :
		{
			// 리스트 갱신
			if( m_pUserDataTitleIndexCurrent )
			{
				pcmUIManager->RefreshUserData( m_pUserDataTitleIndexCurrent, TRUE );
			}
		}
		break;

	case TitleUi_Mini :
		{
			// 리스트 갱신
			if( m_pUserDataTitleIndexMini )
			{
				pcmUIManager->RefreshUserData( m_pUserDataTitleIndexMini, TRUE );
			}
		}
		break;
	}
	
	return TRUE;
}

BOOL AgcmTitle::_UpdateTitleListPage( void )
{
	if( !m_pEditTitlePage ) return FALSE;

	AcUIEdit* pEditPage = ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitlePage )->m_pcsBase;
	if( !pEditPage ) return FALSE;

	if( !m_pCurrentSelectCategory )
	{
		// 선택된 카테고리가 없으면 초기화한다.
		pEditPage->SetText( "" );
		return TRUE;
	}

	// 페이지 번호 갱신
	int nTotalCount = m_pCurrentSelectCategory->m_mapTitle.GetSize();

	float fMaxPageCount = ( float )nTotalCount / ( float )TiTLELIST_TITLE_ITEMCOUNT_PER_PAGE;
	float fFloatingValue = fMaxPageCount - ( float )( ( int )fMaxPageCount );

	int nMaxPageCount = ( int )fMaxPageCount;
	if( fFloatingValue > 0.0f )
	{
		nMaxPageCount += 1;
	}

	char strText[ 64 ] = { 0, };
	sprintf_s( strText, 64, "%d / %d", m_nCurrentTitleListPage, nMaxPageCount );
	pEditPage->SetText( strText );

	// 각 버튼들의 페이지 번호 갱신
	int nModValue = m_nCurrentTitleListPage % 5;
	if( nModValue == 0 )
	{
		nModValue = 5;
	}

	AcUIButton* pBtnPage[ TITLELIST_BUTTON_PAGE_COUNT ] = { NULL, };
	char strBtnText[ 16 ] = { 0, };

	for( int nCount = 0 ; nCount < TITLELIST_BUTTON_PAGE_COUNT ; nCount++ )
	{
		if( m_pBtnListPage[ nCount ] )
		{
			pBtnPage[ nCount ] = ( AcUIButton* )( ( AgcdUIControl* )m_pBtnListPage[ nCount ] )->m_pcsBase;
			if( pBtnPage[ nCount ] )
			{
				int nPageNumber = m_nCurrentTitleListPage - nModValue + ( nCount + 1 );
				if( nPageNumber <= nMaxPageCount )
				{
					memset( strBtnText, 0, sizeof( char ) * 16 );
					sprintf_s( strBtnText, sizeof( char ) * 16, "%d", nPageNumber );

					pBtnPage[ nCount ]->SetStaticString( strBtnText );
					pBtnPage[ nCount ]->SetButtonEnable( TRUE );
				}
				else
				{
					pBtnPage[ nCount ]->SetStaticString( "" );
					pBtnPage[ nCount ]->SetButtonEnable( FALSE );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmTitle::_AddRunningTitle( stTitleEntry* pTitle )
{
	if( !pTitle ) return FALSE;

	stTitleRunningEntry* pEntry = _GetRunningTitleByID( pTitle->m_nTitleID );
	if( pEntry ) return TRUE;

	// 현재 진행중인 타이틀 목록에 추가한다.
	stTitleRunningEntry NewEntry;
	NewEntry.m_pTitle = pTitle;

	m_vecRunningTitle.Add( NewEntry );

	OnClearTitleSelection( TitleUi_ByHotKey );
	OnClearTitleDescription( TitleUi_ByHotKey );
	OnClearTitleSelection( TitleUi_Mini );
	return TRUE;
}

stTitleRunningEntry* AgcmTitle::_GetRunningTitleByID( int nTitleID )
{
	int nRunningTitleCount = m_vecRunningTitle.GetSize();
	for( int nCount = 0 ; nCount < nRunningTitleCount ; nCount++ )
	{
		stTitleRunningEntry* pEntry = m_vecRunningTitle.Get( nCount );
		if( pEntry && pEntry->m_pTitle )
		{
			if( pEntry->m_pTitle->m_nTitleID == nTitleID )
			{
				return pEntry;
			}
		}
	}

	return NULL;
}

BOOL AgcmTitle::_DeleteRunningTitle( int nTitleID )
{
	int nRunningTitleCount = m_vecRunningTitle.GetSize();
	for( int nCount = 0 ; nCount < nRunningTitleCount ; nCount++ )
	{
		stTitleRunningEntry* pEntry = m_vecRunningTitle.Get( nCount );
		if( pEntry && pEntry->m_pTitle )
		{
			if( pEntry->m_pTitle->m_nTitleID == nTitleID )
			{
				OnClearTitleSelection( TitleUi_ByHotKey );
				OnClearTitleDescription( TitleUi_ByHotKey );
				OnClearTitleSelection( TitleUi_Mini );

				m_vecRunningTitle.Delete( nCount );
				return TRUE;
			}
		}
	}

	return TRUE;
}

BOOL AgcmTitle::OnClearTitleIDBoard( void )
{
	// IDBoard 초기화
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( pcmCharacter )
	{
		AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
		if( ppdCharacter )
		{
			OnUpdateTitleTextBoard( ppdCharacter, "", 0xFFFFFFFF, TRUE );
		}
	}

	return TRUE;
}

BOOL AgcmTitle::OnClearTitleStateAll( void )
{
	// 현재 장착중인 타이틀 초기화
	m_pCurrentActivateTitle = NULL;

	// 활성타이틀 목록을 초기화
	m_mapCharacterTitleActivate.Clear();

	// 타이틀들의 상태값을 초기값으로 되돌린다.
	m_vecRunningTitle.Clear();

	int nCategoryCount = m_mapCategory.GetSize();
	for( int nCountCategory = 0 ; nCountCategory < nCategoryCount ; nCountCategory++ )
	{
		stTitleCategory* pCategory = m_mapCategory.GetByIndex( nCountCategory );
		if( pCategory )
		{
			int nTitleCount = pCategory->m_mapTitle.GetSize();
			for( int nCountTitle = 0 ; nCountTitle < nTitleCount ; nCountTitle++ )
			{
				stTitleEntry* pTitle = pCategory->m_mapTitle.GetByIndex( nCountTitle );
				if( pTitle )
				{
					pTitle->m_eState = TitleState_Ready;
					pTitle->m_nCounterCur = 0;
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmTitle::OnClearTitleCategorySelection( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_TitleAll" );
	if( !pcdUI ) return FALSE;

	AgcdUIControl* pcdControl = pcmUIManager->GetControl( pcdUI, "List_TitleCategory" );
	if( !pcdControl ) return FALSE;

	AcUIList* pList = ( AcUIList* )pcdControl->m_pcsBase;
	if( !pList ) return FALSE;

	pList->SelectItem( -1 );
	m_pCurrentSelectCategory = NULL;
	return TRUE;
}

BOOL AgcmTitle::OnClearTitleSelection( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = NULL;
	switch( eType )
	{
	case TitleUi_ByNPC :	pcdUI = pcmUIManager->GetUI( "UI_TitleAll" );		break;
	case TitleUi_ByHotKey :	pcdUI = pcmUIManager->GetUI( "UI_TitleCurrent" );	break;
	case TitleUi_Mini :		pcdUI = pcmUIManager->GetUI( "UI_TitleMini" );		break;
	}

	if( !pcdUI ) return FALSE;

	AgcdUIControl* pcdControl = pcmUIManager->GetControl( pcdUI, "List_TitleList" );
	if( !pcdControl ) return FALSE;

	AcUIList* pList = ( AcUIList* )pcdControl->m_pcsBase;
	if( !pList ) return FALSE;

	pList->SelectItem( -1 );

	switch( eType )
	{
	case TitleUi_ByNPC :	m_pCurrentSelectTitle = NULL;			break;
	case TitleUi_ByHotKey :	m_pCurrentSelectTitleCurrent = NULL;	break;
	case TitleUi_Mini :		m_pCurrentSelectTitleMini = NULL;		break;
	}

	return TRUE;
}

BOOL AgcmTitle::OnClearTitleDescription( eTitleUiType eType )
{
	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			AcUIEdit* pEditName = m_pEditTitleName ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleName )->m_pcsBase : NULL;
			if( pEditName )
			{
				pEditName->ClearText();
			}

			AcUIEdit* pEditInfo = m_pEditTitleInfo ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleInfo )->m_pcsBase : NULL;
			if( pEditInfo )
			{
				pEditInfo->ClearText();
			}

			AcUIEdit* pEditEffect = m_pEditTitleEffect ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleEffect )->m_pcsBase : NULL;
			if( pEditEffect )
			{
				pEditEffect->ClearText();
			}
		}
		break;

	case TitleUi_ByHotKey :
		{
			AcUIEdit* pEditName = m_pEditTitleNameCurrent ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleNameCurrent )->m_pcsBase : NULL;
			if( pEditName )
			{
				pEditName->ClearText();
			}

			AcUIEdit* pEditInfo = m_pEditTitleInfoCurrent ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleInfoCurrent )->m_pcsBase : NULL;
			if( pEditInfo )
			{
				pEditInfo->ClearText();
			}

			AcUIEdit* pEditEffect = m_pEditTitleEffectCurrent ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleEffectCurrent )->m_pcsBase : NULL;
			if( pEditEffect )
			{
				pEditEffect->ClearText();
			}
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateTitleInfo( eTitleUiType eType, stTitleEntry* pTitle )
{
	if( !pTitle ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AcUIEdit* pEditName = NULL;
	AcUIEdit* pEditInfo = NULL;
	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			pEditName = m_pEditTitleName ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleName )->m_pcsBase : NULL;
			pEditInfo = m_pEditTitleInfo ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleInfo )->m_pcsBase : NULL;
		}
		break;

	case TitleUi_ByHotKey :
		{
			pEditName = m_pEditTitleNameCurrent ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleNameCurrent )->m_pcsBase : NULL;
			pEditInfo = m_pEditTitleInfoCurrent ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleInfoCurrent )->m_pcsBase : NULL;
		}
		break;
	}

	if( !pEditName || !pEditInfo ) return FALSE;

	// 타이틀명
	pEditName->SetText( pTitle->m_strName );

	// 라인피드 태그설정
	pEditInfo->SetLineDelimiter( "</n>" );

	if( pTitle->m_nCounterMax > 0 )
	{
		switch( pTitle->m_eState )
		{
		case TitleState_Running :
		case TitleState_Complete :
			{
				// 진행상태가 있는 경우
				char strText[ STRING_LENGTH_TITLE_DESC ] = { 0, };
				sprintf_s( strText, sizeof( char ) * STRING_LENGTH_TITLE_DESC, "%s</n>(%d/%d)", pTitle->m_strDesc, pTitle->m_nCounterCur, pTitle->m_nCounterMax );
				pEditInfo->SetText( strText, 0xFFFFFFFF );
			}
			break;

		default :
			{
				pEditInfo->SetText( pTitle->m_strDesc );
			}
			break;
		}
	}
	else
	{
		pEditInfo->SetText( pTitle->m_strDesc );
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateTitleEffect( eTitleUiType eType, stTitleEntry* pTitle )
{
	if( !pTitle ) return FALSE;

	AcUIEdit* pEdit = NULL;
	switch( eType )
	{
	case TitleUi_ByNPC :	pEdit = m_pEditTitleEffect ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleEffect )->m_pcsBase : NULL;				break;
	case TitleUi_ByHotKey :	pEdit = m_pEditTitleEffectCurrent ? ( AcUIEdit* )( ( AgcdUIControl* )m_pEditTitleEffectCurrent )->m_pcsBase : NULL;	break;
	}

	if( !pEdit ) return FALSE;

	// 라인피드 태그설정
	pEdit->SetLineDelimiter( "</n>" );
	pEdit->SetText( pTitle->m_strDescEffect );
	return TRUE;
}

BOOL AgcmTitle::_UpdateDialogButtonState( eTitleUiType eType )
{
	// 초기화 한번 하고서 시작하자..
	if( !_ReSetDialogButtonState( eType ) ) return FALSE;

	// 현재 선택된 타이틀이 없다면 여기서 스톱
	if( !m_pCurrentSelectTitle ) return FALSE;

	switch( m_pCurrentSelectTitle->m_eState )
	{
	case TitleState_Ready :		return _UpdateDialogButtonStateReady( eType );		break;
	case TitleState_Running :	return _UpdateDialogButtonStateRunning( eType );	break;
	case TitleState_Complete :	return _UpdateDialogButtonStateComplete( eType );	break;
	case TitleState_Enable :	return _UpdateDialogButtonStateEnable( eType );		break;
	case TitleState_Activate :	return _UpdateDialogButtonStateActivate( eType );	break;
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateDialogButtonStateReady( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			AcUIButton* pBtnSetTarget = m_pBtnSetTargetTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnSetTargetTitle )->m_pcsBase : NULL;
			if( !pBtnSetTarget ) return FALSE;

			// 획득목표로 설정 버튼만 활성화
			pBtnSetTarget->SetButtonEnable( TRUE );
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateDialogButtonStateRunning( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			// 현재까지는 초기화 상태에서 변경해야 할 것은 없음.. 포기하기 버튼이 다른 곳에 있으니...
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateDialogButtonStateComplete( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			AcUIButton* pBtnComplete = m_pBtnCompleteTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnCompleteTitle )->m_pcsBase : NULL;
			if( !pBtnComplete ) return FALSE;

			// 완료 버튼 활성화
			pBtnComplete->SetButtonEnable( TRUE );
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateDialogButtonStateEnable( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			AcUIButton* pBtnSetTarget = m_pBtnSetTargetTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnSetTargetTitle )->m_pcsBase : NULL;
			AcUIButton* pBtnActivate = m_pBtnActivateTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnActivateTitle )->m_pcsBase : NULL;
			if( !pBtnSetTarget || !pBtnActivate ) return FALSE;

			// 획득 목표로 설정 버튼을 삭제 버튼으로 변경
			char* pBtnText = pcmUIManager->GetUIMessage( "TitleBtnText_DeleteTitle" );
			pBtnSetTarget->SetStaticString( pBtnText );

			// 삭제 버튼과 장착 버튼 활성화
			pBtnSetTarget->SetButtonEnable( TRUE );
			pBtnActivate->SetButtonEnable( TRUE );

		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::_UpdateDialogButtonStateActivate( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			AcUIButton* pBtnSetTarget = m_pBtnSetTargetTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnSetTargetTitle )->m_pcsBase : NULL;
			AcUIButton* pBtnActivate = m_pBtnActivateTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnActivateTitle )->m_pcsBase : NULL;
			if( !pBtnSetTarget || !pBtnActivate ) return FALSE;

			// 획득 목표로 설정 버튼을 삭제 버튼으로 변경
			char* pBtnText = pcmUIManager->GetUIMessage( "TitleBtnText_DeleteTitle" );
			pBtnSetTarget->SetStaticString( pBtnText );

			// 장착 버튼을 장착해제 버튼으로 변경
			pBtnText = pcmUIManager->GetUIMessage( "TitleBtnText_DeActivateTitle" );
			pBtnActivate->SetStaticString( pBtnText );

			// 장착해제 버튼만 활성화
			pBtnActivate->SetButtonEnable( TRUE );
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::OnUpdateTitleTextBoard( void* pCharacter, char* pTitleName, DWORD dwColor, BOOL bIsSelfCharacter )
{
	AgcmTextBoardMng* pcmTextBoardMng = ( AgcmTextBoardMng* )g_pEngine->GetModule( "AgcmTextBoardMng" );
	if( pcmTextBoardMng )
	{
		AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
		if( ppdCharacter )
		{
			sBoardDataPtr pBoard = pcmTextBoardMng->GetBoard( ppdCharacter );
			if( pBoard && pBoard->pHPBar )
			{
				pBoard->pHPBar->SetTitle( pTitleName && strlen( pTitleName ) > 0 ? TRUE : FALSE );
				pBoard->pHPBar->SetTitleNameText( pTitleName ? pTitleName : "" );
				pBoard->pHPBar->SetColorTitleName( ARGB_TO_ABGR( dwColor ) );
				pBoard->pHPBar->ReCalulateOffsetX();
			}
			else if( pBoard && pBoard->pIDBoard )
			{
				pBoard->pIDBoard->SetTitle( pTitleName && strlen( pTitleName ) > 0 ? TRUE : FALSE );
				pBoard->pIDBoard->SetTitleNameText( pTitleName ? pTitleName : "" );
				pBoard->pIDBoard->SetColorTitleName( ARGB_TO_ABGR( dwColor ) );
				pBoard->pIDBoard->ReCalulateOffsetX();
			}
		}
	}

	return TRUE;
}

BOOL AgcmTitle::_ReSetDialogButtonState( eTitleUiType eType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	switch( eType )
	{
	case TitleUi_ByNPC :
		{
			AcUIButton* pBtnSetTarget = m_pBtnSetTargetTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnSetTargetTitle )->m_pcsBase : NULL;
			AcUIButton* pBtnActivate = m_pBtnActivateTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnActivateTitle )->m_pcsBase : NULL;
			AcUIButton* pBtnComplete = m_pBtnCompleteTitle ? ( AcUIButton* )( ( AgcdUIControl* )m_pBtnCompleteTitle )->m_pcsBase : NULL;
			if( !pBtnSetTarget || !pBtnActivate || !pBtnComplete ) return FALSE;

			// 획득목표로 설정 버튼을 획득목표로 설정 버튼으로 변경
			char* pBtnText = pcmUIManager->GetUIMessage( "TitleBtnText_SetTarget" );
			pBtnSetTarget->SetStaticString( pBtnText );

			// 장착 버튼을 장착 버튼으로 변경
			pBtnText = pcmUIManager->GetUIMessage( "TitleBtnText_ActivateTitle" );
			pBtnActivate->SetStaticString( pBtnText );

			// 모든 버튼을 비활성화
			pBtnSetTarget->SetButtonEnable( FALSE );
			pBtnActivate->SetButtonEnable( FALSE );
			pBtnComplete->SetButtonEnable( FALSE );
		}
		break;
	}

	return TRUE;
}

BOOL AgcmTitle::_CheckTitleExchangeTimeLimit( char* pCharacterName )
{
	if( !pCharacterName || strlen( pCharacterName ) <= 0 ) return FALSE;

	unsigned int nCurrTime = AuTimeStamp::GetCurrentTimeStamp();
	unsigned int nLastTime = _GetLastActivateTime( pCharacterName );

	time_t TimeValue = ( time_t )nLastTime;
	struct tm* pTime = localtime( &TimeValue );
	if( !pTime ) return FALSE;

	int nLimitYear		= pTime->tm_year	+ m_stTitleTimeLimit.m_nYear;
	int nLimitMonth		= pTime->tm_mon		+ m_stTitleTimeLimit.m_nMonth;
	int nLimitDay		= pTime->tm_mday	+ m_stTitleTimeLimit.m_nDay;
	int nLimitHour		= pTime->tm_hour	+ m_stTitleTimeLimit.m_nHour;
	int nLimitMinute	= pTime->tm_min		+ m_stTitleTimeLimit.m_nMinute;
	int nLimitSecond	= pTime->tm_sec		+ m_stTitleTimeLimit.m_nSecond;

	TimeValue = ( time_t )nCurrTime;
	pTime = localtime( &TimeValue );
	if( !pTime ) return FALSE;

	int nCurrYear	= pTime->tm_year;
	int nCurrMonth	= pTime->tm_mon;
	int nCurrDay	= pTime->tm_mday;
	int nCurrHour	= pTime->tm_hour;
	int nCurrMinute	= pTime->tm_min;
	int nCurrSecond	= pTime->tm_sec;

	if( nCurrYear > nLimitYear ) return TRUE;
	else if( nCurrYear == nLimitYear )
	{
		if( nCurrMonth > nLimitMonth )	return TRUE;
		else if( nCurrMonth == nLimitMonth )
		{
			if( nCurrDay > nLimitDay ) return TRUE;
			else if( nCurrDay == nLimitDay )
			{
				if( nCurrHour > nLimitHour ) return TRUE;
				else if( nCurrHour == nLimitHour )
				{
					if( nCurrMinute	> nLimitMinute ) return TRUE;
					else if( nCurrMinute == nLimitMinute )
					{
						if( nCurrSecond	> nLimitSecond ) return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL AgcmTitle::_CheckIsMaxRunningTitle( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 현재 진행중인 타이틀의 갯수가 지정된 최대 갯수에 도달하였으면 실패다.. 에러메세지 출력하고 리턴
	int nCurrentCount = GetTitleCountByState( TitleState_Running );
	if( nCurrentCount >= AGPDTITLE_MAX_TITLE_QUEST_NUMBER )
	{
		char* pErrorMsg = pcmUIManager->GetUIMessage( "CannotRunMoreTitle" );
		if( pErrorMsg && strlen( pErrorMsg ) > 0 )
		{
			pcmUIManager->ActionMessageOKDialog( pErrorMsg );
		}

		return FALSE;
	}

	return TRUE;
}

BOOL AgcmTitle::_CheckIsMaxEnableTitle( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 현재 사용가능한 타이틀의 갯수가 지정된 최대 갯수에 도달하였으면 실패다.. 에러메세지 출력하고 리턴
	int nEnableCount = GetTitleCountByState( TitleState_Enable ) + GetTitleCountByState( TitleState_Activate );
	if( nEnableCount >= AGPDTITLE_TITLE_NUMBER_MAX )
	{
		char* pErrorMsg = pcmUIManager->GetUIMessage( "CannotTryMoreTitle" );
		if( pErrorMsg && strlen( pErrorMsg ) > 0 )
		{
			pcmUIManager->ActionMessageOKDialog( pErrorMsg );
		}

		return FALSE;
	}

	return TRUE;
}

BOOL AgcmTitle::_CheckIsMaxEnableAndRunningTitle( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 현재 사용가능한 타이틀의 갯수가 지정된 최대 갯수에 도달하였으면 실패다.. 에러메세지 출력하고 리턴
	int nEnableCount = GetTitleCountByState( TitleState_Enable ) + GetTitleCountByState( TitleState_Activate ) + GetTitleCountByState( TitleState_Running );;
	if( nEnableCount >= AGPDTITLE_TITLE_NUMBER_MAX )
	{
		char* pErrorMsg = pcmUIManager->GetUIMessage( "CannotTryMoreTitle" );
		if( pErrorMsg && strlen( pErrorMsg ) > 0 )
		{
			pcmUIManager->ActionMessageOKDialog( pErrorMsg );
		}

		return FALSE;
	}

	return TRUE;
}


BOOL AgcmTitle::_CheckIsValidTitleRequirement( void )
{
	if( !m_pCurrentSelectTitle ) return FALSE;

	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmTitle || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	AgpdTitleTemplate* ppdTitleTemplate = ppmTitle->GetTitleTemplate( m_pCurrentSelectTitle->m_nTitleID );
	if( !ppdTitleTemplate ) return FALSE;

	BOOL bIsValid = _CheckIsValidRequireArchlord( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireArchlord" );

	bIsValid = _CheckIsValidRequireGuildMaster( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireGuildMaster" );

	bIsValid = _CheckIsValidRequireClassType( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireClassType" );

	bIsValid = _CheckIsValidRequireLevel( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireLevel" );

	bIsValid = _CheckIsValidRequireCon( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireCon" );

	bIsValid = _CheckIsValidRequireStr( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireStr" );

	bIsValid = _CheckIsValidRequireInt( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireInt" );

	bIsValid = _CheckIsValidRequireDex( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireDex" );

	bIsValid = _CheckIsValidRequireWis( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireWis" );

	bIsValid = _CheckIsValidRequireCharisma( ppdCharacter, ppdTitleTemplate);
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireCharisma" );

	bIsValid = _CheckIsValidRequireGheld( ppdCharacter, ppdTitleTemplate);
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireGheld" );

	bIsValid = _CheckIsValidRequireCash( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireCash" );

	bIsValid = _CheckIsValidRequireItem( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireItem" );

	bIsValid = _CheckIsValidRequireTitle( ppdCharacter, ppdTitleTemplate );
	if( !bIsValid ) return _ShowErrorMessageBoxOK( "TitleErrorMsg_RequireTitle" );

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireLevel( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	int nCharacterLevel = ppmCharacter->GetLevel( ppdCharacter );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireLevel > nCharacterLevel ) return FALSE;
	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireGuildMaster( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	if( !ppmTitle ) return FALSE;

	BOOL bIsGuildMaster = ppmTitle->IsGuildMaster( ppdCharacter );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireGuildMaster && !bIsGuildMaster ) return FALSE;
	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireArchlord( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	BOOL bIsArchlord = ppmCharacter->IsArchlord( ppdCharacter );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireClassArchlord && !bIsArchlord ) return FALSE;
	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireClassType( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	AuRaceType eRace = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_csFactor );

	switch( ppdTitleTemplate->m_stRequireCondition.m_nRequireClass )
	{
	case AGPMTITLE_REQUIRE_CLASS_ALL :			return TRUE;																					break;
	case AGPMTITLE_REQUIRE_CLASS_KNIGHT :		return eRace == AURACE_TYPE_HUMAN && eClass == AUCHARCLASS_TYPE_KNIGHT ? TRUE : FALSE;			break;
	case AGPMTITLE_REQUIRE_CLASS_ARCHER :		return eRace == AURACE_TYPE_HUMAN && eClass == AUCHARCLASS_TYPE_RANGER ? TRUE : FALSE;			break;
	case AGPMTITLE_REQUIRE_CLASS_WIZARD :		return eRace == AURACE_TYPE_HUMAN && eClass == AUCHARCLASS_TYPE_MAGE ? TRUE : FALSE;			break;
	case AGPMTITLE_REQUIRE_CLASS_BERSERKER :	return eRace == AURACE_TYPE_ORC && eClass == AUCHARCLASS_TYPE_KNIGHT ? TRUE : FALSE;			break;
	case AGPMTITLE_REQUIRE_CLASS_HUNTER :		return eRace == AURACE_TYPE_ORC && eClass == AUCHARCLASS_TYPE_RANGER ? TRUE : FALSE;			break;
	case AGPMTITLE_REQUIRE_CLASS_SORCERER :		return eRace == AURACE_TYPE_ORC && eClass == AUCHARCLASS_TYPE_MAGE ? TRUE : FALSE;				break;
	case AGPMTITLE_REQUIRE_CLASS_SWASHBUCKLER :	return eRace == AURACE_TYPE_MOONELF && eClass == AUCHARCLASS_TYPE_KNIGHT ? TRUE : FALSE;		break;
	case AGPMTITLE_REQUIRE_CLASS_RANGER :		return eRace == AURACE_TYPE_MOONELF && eClass == AUCHARCLASS_TYPE_RANGER ? TRUE : FALSE;		break;
	case AGPMTITLE_REQUIRE_CLASS_ELEMENTALER :	return eRace == AURACE_TYPE_MOONELF && eClass == AUCHARCLASS_TYPE_MAGE ? TRUE : FALSE;			break;
	case AGPMTITLE_REQUIRE_CLASS_SCION :		return eRace == AURACE_TYPE_DRAGONSCION ? TRUE : FALSE;											break;
	case AGPMTITLE_REQUIRE_CLASS_SLAYER :		return eRace == AURACE_TYPE_DRAGONSCION ? TRUE : FALSE;											break;
	case AGPMTITLE_REQUIRE_CLASS_ORBITER :		return eRace == AURACE_TYPE_DRAGONSCION ? TRUE : FALSE;											break;
	case AGPMTITLE_REQUIRE_CLASS_SUMMONER :		return eRace == AURACE_TYPE_DRAGONSCION ? TRUE : FALSE;											break;
	}

	return FALSE;
}

BOOL AgcmTitle::_CheckIsValidRequireCon( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValue = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireCon > nValue ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireStr( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValue = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireStr > nValue ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireInt( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValue = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireInt > nValue ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireDex( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValue = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireDex > nValue ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireWis( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValue = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireWis > nValue ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireCharisma( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValue = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireCha > nValue ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireGheld( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	__int64 nGheld = 0;
	ppmCharacter->GetMoney( ppdCharacter, &nGheld );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireGheld > nGheld ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireCash( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	__int64 nCash = ppmCharacter->GetCash( ppdCharacter );
	if( ppdTitleTemplate->m_stRequireCondition.m_nRequireCash > nCash ) return FALSE;

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireItem( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
	if( !ppmTitle ) return FALSE;

	for( int nCount = 0 ; nCount < AGPDTITLE_MAX_TITLE_REQUIRE_ITEM ; nCount++ )
	{
		int nRequireItemTID = ppdTitleTemplate->m_stRequireCondition.m_nRequireItemTid[ nCount ][ 0 ];
		int nRequireItemCount = ppdTitleTemplate->m_stRequireCondition.m_nRequireItemTid[ nCount ][ 1 ];
		if( nRequireItemTID <= 0 || nRequireItemCount <= 0 ) continue;

		if( !ppmTitle->CheckItemCount( ppdCharacter, nRequireItemTID, nRequireItemCount ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmTitle::_CheckIsValidRequireTitle( void* pCharacter, void* pTitleTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdTitleTemplate* ppdTitleTemplate = ( AgpdTitleTemplate* )pTitleTemplate;
	if( !ppdCharacter || !ppdTitleTemplate ) return FALSE;

	for( int nCount = 0 ; nCount < AGPDTITLE_MAX_TITLE_REQUIRE_TITLE ; nCount++ )
	{
		if( ppdTitleTemplate->m_stRequireCondition.m_nRequireTitleTid[ nCount ] != 0 )
		{
			if( ppdCharacter->m_csTitle )
			{
				int nRequireTitleTID = ppdTitleTemplate->m_stRequireCondition.m_nRequireTitleTid[ nCount ];
				if( !ppdCharacter->m_csTitle->IsHaveTitle( nRequireTitleTID ) )
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmTitle::_ShowErrorMessageBoxOK( char* pMessageKeyString )
{
	// 이 함수는 무조건 FALSE 를 리턴합니다.. 에러메세지 출력용이니까요.. ㄲㄲ
	if( !pMessageKeyString || strlen( pMessageKeyString ) <= 0 ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pErrorMsg = pcmUIManager->GetUIMessage( pMessageKeyString );
	if( !pErrorMsg || strlen( pErrorMsg ) <= 0 ) return FALSE;

	pcmUIManager->ActionMessageOKDialog( pErrorMsg );
	return FALSE;
}

char* AgcmTitle::GetCategoryNameByIndex( int nIndex )
{
	stTitleCategory* pCategory = _GetCategoryByIndex( nIndex );
	if( !pCategory ) return NULL;
	return pCategory->m_strName;
}

char* AgcmTitle::GetTitleNameByID( int nTitleID )
{
	int nCategoryCount = m_mapCategory.GetSize();
	for( int nCountCategory = 0 ; nCountCategory < nCategoryCount ; nCountCategory++ )
	{
		stTitleCategory* pCategory = m_mapCategory.GetByIndex( nCountCategory );
		if( pCategory )
		{
			stTitleEntry* pTitle = pCategory->m_mapTitle.Get( nTitleID );
			if( pTitle )
			{
				return pTitle->m_strName;
			}
		}
	}

	return NULL;
}

char* AgcmTitle::GetTitleNameByIndex( int nIndex )
{
	stTitleEntry* pTitle = _GetTitleByIndex( m_pCurrentSelectCategory, nIndex );
	if( !pTitle ) return NULL;
	return pTitle->m_strName;
}

char* AgcmTitle::GetTitleNameByIndexWithPage( int nIndex )
{
	int nFindIndex = nIndex + ( TiTLELIST_TITLE_ITEMCOUNT_PER_PAGE * ( m_nCurrentTitleListPage - 1 ) );
	stTitleEntry* pTitle = _GetTitleByIndex( m_pCurrentSelectCategory, nFindIndex );
	if( !pTitle ) return NULL;
	return pTitle->m_strName;
}

eTitleStateType AgcmTitle::GetTitleStateByIndexWidthPage( int nIndex )
{
	int nFindIndex = nIndex + ( TiTLELIST_TITLE_ITEMCOUNT_PER_PAGE * ( m_nCurrentTitleListPage - 1 ) );
	stTitleEntry* pTitle = _GetTitleByIndex( m_pCurrentSelectCategory, nFindIndex );
	if( !pTitle ) return TitleState_UnKnown;
	return pTitle->m_eState;
}

eTitleStateType AgcmTitle::GetCurrTitleStateByIndex( int nIndex )
{
	stTitleEntry* pTitle = _GetTitleCurrentByIndex( nIndex );
	if( !pTitle ) return TitleState_UnKnown;
	return pTitle->m_eState;
}

char* AgcmTitle::GetCurrTitleNameByIndexWithPage( int nIndex )
{
	stTitleEntry* pTitle = _GetTitleCurrentByIndex( nIndex );
	if( !pTitle ) return NULL;
	return pTitle->m_strName;
}

int AgcmTitle::GetTitleCountByState( eTitleStateType eState )
{
	int nFindCount = 0;

	int nCategoryCount = m_mapCategory.GetSize();
	for( int nCountCategory = 0 ; nCountCategory < nCategoryCount ; nCountCategory++ )
	{
		stTitleCategory* pCategory = m_mapCategory.GetByIndex( nCountCategory );
		if( pCategory )
		{
			int nTitleCount = pCategory->m_mapTitle.GetSize();
			for( int nCountTitle = 0 ; nCountTitle < nTitleCount ; nCountTitle++ )
			{
				stTitleEntry* pTitle = pCategory->m_mapTitle.GetByIndex( nCountTitle );
				if( pTitle && pTitle->m_eState == eState )
				{
					nFindCount++;
				}
			}
		}
	}

	return nFindCount;
}

int AgcmTitle::GetTitleCountCurrentCategory( void )
{
	if( !m_pCurrentSelectCategory ) return 0;
	return m_pCurrentSelectCategory->m_mapTitle.GetSize();
}

void AgcmTitle::SetTitlePageIndex( int nPageIndex )
{
	if( !m_pCurrentSelectCategory ) return;
	if( nPageIndex <= 0 ) return;
	
	int nTitleCount = GetTitleCountCurrentCategory();
	int nTotalCount = m_pCurrentSelectCategory->m_mapTitle.GetSize();

	float fMaxPageCount = ( float )nTotalCount / ( float )TiTLELIST_TITLE_ITEMCOUNT_PER_PAGE;
	float fFloatingValue = fMaxPageCount - ( float )( ( int )fMaxPageCount );

	int nMaxPageCount = ( int )fMaxPageCount;
	if( fFloatingValue > 0.0f )
	{
		nMaxPageCount += 1;
	}

	if( nPageIndex > nMaxPageCount )
	{
		nPageIndex = nMaxPageCount;
	}

	m_nCurrentTitleListPage = nPageIndex;

	_UpdateTitleListPage();
	_UpdateTitleList( TitleUi_ByNPC );
}

int AgcmTitle::GetCurrentActivateTitleID( void )
{
	if( !m_pCurrentActivateTitle ) return -1;
	return m_pCurrentActivateTitle->m_nTitleID;
}

char* AgcmTitle::GetCurrentActivateTitleName( char* pCharacterName )
{
	if( !pCharacterName || strlen( pCharacterName ) <= 0 ) return NULL;

	stCharacterTitleActivate* pCharacterTitle = m_mapCharacterTitleActivate.Get( pCharacterName );
	if( !pCharacterTitle ) return NULL;
	return GetTitleNameByID( pCharacterTitle->m_nTitleID );
}

DWORD AgcmTitle::GetTitleFontColor( eTitleStateType eType )
{
	stTitleFontColorEntry* pEntry = m_mapTitleFontColor.Get( eType );
	if( !pEntry ) return 0xFFFFFFFF;
	return pEntry->m_dwColor;
}

BOOL AgcmTitle::IsOpenUI( eTitleUiType eUiType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = NULL;
	switch( eUiType )
	{
	case TitleUi_ByNPC :	pcdUI = pcmUIManager->GetUI( "UI_TitleAll" );		break;
	case TitleUi_ByHotKey :	pcdUI = pcmUIManager->GetUI( "UI_TitleCurrent" );	break;
	case TitleUi_Mini :		pcdUI = pcmUIManager->GetUI( "UI_TitleMini" );		break;
	}

	if( !pcdUI ) return FALSE;
	if( pcdUI->m_eStatus == AGCDUI_STATUS_OPENED || pcdUI->m_eStatus == AGCDUI_STATUS_OPENING ) return TRUE;

	return FALSE;
}

BOOL AgcmTitle::CallBack_RecvTitleAdd( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE* pPacket = ( PACKET_AGPPTITLE* )pCustData;
	if( !pPacket ) return FALSE;
	if( pPacket->nParam != AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_ADD_RESULT* pResult = ( PACKET_AGPPTITLE_ADD_RESULT* )pPacket;
	if( pResult->bAddResult )
	{
		pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Enable, ppdCharacter );

		// 시스템메세지 창에 이 타이틀을 습득했다고 찍어준다.
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
		AgcmChatting2* pcmChatting = ( AgcmChatting2* )g_pEngine->GetModule( "AgcmChatting2" );
		if( pcmUIManager && pcmChatting )
		{
			char* pMessageFormat = pcmUIManager->GetUIMessage( "NotifyGetNewTitle" );
			if( pMessageFormat && strlen( pMessageFormat ) > 0 )
			{
				char* pTitleName = pcmTitle->GetTitleNameByID( pResult->nTitleID );
				if( pTitleName && strlen( pTitleName ) > 0 )
				{
					char strMessage[ 256 ] = { 0, };
					sprintf_s( strMessage, sizeof( char ) * 256, pMessageFormat, pTitleName );
					pcmChatting->AddSystemMessage( strMessage );
				}
			}
		}
	}
	
	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleUse( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE* pPacket = ( PACKET_AGPPTITLE* )pCustData;
	if( !pPacket ) return FALSE;
	if( pPacket->nParam != AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_USE_RESULT* pResult = ( PACKET_AGPPTITLE_USE_RESULT* )pPacket;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	if(!pResult->bUseResult)
	{
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
		if( !pcmUIManager ) return FALSE;

		switch(pResult->lFailReason)
		{
		case AGPMTITLE_USE_SUCCESS:
			break;
		case AGPMTITLE_USE_FAIL_DONT_HAVE_TITLE:
			break;
		case AGPMTITLE_USE_FAIL_ALREADY_USE:
			break;
		case AGPMTITLE_USE_FAIL_NOT_ENOUGH_TIME:
			{
				char* pErrMessageFormat = pcmUIManager->GetUIMessage( "TitleErrorMsg_CannotExChangeTitleIn2Hour" );
				if( pErrMessageFormat && strlen( pErrMessageFormat ) > 0 )
				{
					int nMonth = 0;
					int nDay = 0;
					int nHour = 0;
					int nMinute = 0;

					unsigned int nLastActivateTime = pcmTitle->_GetLastActivateTime( ppdCharacter->m_szID );
					pcmTitle->_ParseTimeStamp( nLastActivateTime, NULL, &nMonth, &nDay, &nHour, &nMinute, NULL );

					char strErrorMsg[ 256 ] = { 0, };
					sprintf_s( strErrorMsg, sizeof( char ) * 256, pErrMessageFormat, nMonth, nDay, nHour, nMinute );
					pcmUIManager->ActionMessageOKDialog( strErrorMsg );
				}
			}
			break;
		case AGPMTITLE_USE_FAIL_DONT_KNOW_REASON:
			break;
		}

		return FALSE;
	}

	BOOL bIsSelfCharacter = pcmCharacter->GetSelfCharacter() == ppdCharacter ? TRUE : FALSE;
	pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Activate, ppdCharacter, bIsSelfCharacter, pResult->bUse );
	pcmTitle->OnUpdateTitleTimeStamp( pResult->strCharName, pResult->nTitleID, pResult->lTimeStamp );
	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleList( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE* pPacket = ( PACKET_AGPPTITLE* )pCustData;
	if( !pPacket ) return FALSE;
	if( pPacket->nParam != AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_LIST_RESULT* pResult = ( PACKET_AGPPTITLE_LIST_RESULT* )pPacket;
	if( pResult->bListResult )
	{
		pcmTitle->OnUpdateTitleTimeStamp( pResult->strCharName, pResult->nTitleID, pResult->lTimeStamp );
		pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Enable, ppdCharacter );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleDelete( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE* pPacket = ( PACKET_AGPPTITLE* )pCustData;
	if( !pPacket ) return FALSE;
	if( pPacket->nParam != AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_DELETE_RESULT* pResult = ( PACKET_AGPPTITLE_DELETE_RESULT* )pPacket;
	if( pResult->bTitleDeleteResult )
	{
		pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Ready, ppdCharacter );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleQuestRequest( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE_QUEST* pPacket = ( PACKET_AGPPTITLE_QUEST* )pCustData;
	if( !pPacket ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_QUEST_REQUEST_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_QUEST_REQUEST_RESULT* pResult = ( PACKET_AGPPTITLE_QUEST_REQUEST_RESULT* )pPacket;
	if( pResult->nOperation != AGPMTITLE_QUEST_REQUEST_RESULT ) return FALSE;

	if( pResult->bQuestRequestResult )
	{
		pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Running, ppdCharacter, TRUE, pResult->bQuestRequestResult, 0 );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleQuestCheck( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE_QUEST* pPacket = ( PACKET_AGPPTITLE_QUEST* )pCustData;
	if( !pPacket ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_QUEST_CHECK_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_QUEST_CHECK_RESULT* pResult = ( PACKET_AGPPTITLE_QUEST_CHECK_RESULT* )pPacket;
	if( pResult->nOperation != AGPMTITLE_QUEST_CHECK_RESULT ) return FALSE;

	if( pResult->bCheckResult )
	{
		pcmTitle->OnUpdateTitleCounter( pResult->nTitleID, pResult->nTitleCurrentValue[ 0 ] );

		// 이 타이틀의 완료조건을 모두 채웠는지 검사한다.
		AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
		AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
		if( pcmCharacter && ppmTitle )
		{
			AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
			if( ppdCharacter )
			{
				if( ppmTitle->SatisfyTitleQuestCompleteCondition( ppdCharacter, pResult->nTitleID ) )
				{
					// 완료조건을 모두 채웠다면 해당 타이틀의 상태를 Complete 상태로 변경한다.
					pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Complete, ppdCharacter );
				}
			}
		}
	}

	// UI 업데이트
	pcmTitle->OnUpateUI( TitleUi_ByNPC );
	pcmTitle->OnUpateUI( TitleUi_ByHotKey );
	pcmTitle->OnUpateUI( TitleUi_Mini );
	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleQuestComplete( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE_QUEST* pPacket = ( PACKET_AGPPTITLE_QUEST* )pCustData;
	if( !pPacket ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_QUEST_COMPLETE_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT* pResult = ( PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT* )pPacket;
	if( pResult->nOperation != AGPMTITLE_QUEST_COMPLETE_RESULT ) return FALSE;

	if( pResult->bCompleteResult )
	{
		// NPC 로부터 퀘스트 완료처리가 되었으므로 이 타이틀은 이제부터 사용가능한 상태가 된다.
		pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Enable, ppdCharacter );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleQuestList( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !pcmTitle || !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE_QUEST* pPacket = ( PACKET_AGPPTITLE_QUEST* )pCustData;
	if( !pPacket ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pPacket->strCharName );
	if( !ppdCharacter ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, "Operation = AGPMTITLE_QUEST_LIST_RESULT, 캐릭터 = %s, 타이틀ID = %d\n", pPacket->strCharName, pPacket->nTitleID );
	OutputDebugString( strDebug );
#endif

	PACKET_AGPPTITLE_QUEST_LIST_RESULT* pResult = ( PACKET_AGPPTITLE_QUEST_LIST_RESULT* )pPacket;
	if( pResult->nOperation != AGPMTITLE_QUEST_LIST_RESULT ) return FALSE;

	if( pResult->bTitleQuestListResult )
	{
		if( pResult->bComplete )
		{
			// 퀘스트가 완료된 것은 사용가능한 상태가 된다.
			pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Enable, ppdCharacter );
		}
		else
		{
			// 퀘스트가 아직 진행중인 것들이다..
			pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Running, ppdCharacter, TRUE, pResult->bComplete, pResult->nTitleCurrentValue[ 0 ] );

			// 이 타이틀의 완료조건을 모두 채웠는지 검사한다.
			AgpmTitle* ppmTitle = ( AgpmTitle* )g_pEngine->GetModule( "AgpmTitle" );
			if( ppmTitle )
			{
				if( ppmTitle->SatisfyTitleQuestCompleteCondition( ppdCharacter, pResult->nTitleID ) )
				{
					// 완료조건을 모두 채웠다면 해당 타이틀의 상태를 Complete 상태로 변경한다.
					pcmTitle->OnChangeTitleState( pResult->nTitleID, TitleState_Complete, ppdCharacter );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleUseNear( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	PACKET_AGPPTITLE* pPacket = ( PACKET_AGPPTITLE* )pCustData;
	if( !pPacket ) return FALSE;
	if( pPacket->nParam != AGPMTITLE_PACKET_OPERATION_TITLE_USE_NEAR ) return FALSE;

	PACKET_AGPPTITLE_USE_NEAR* pResult = ( PACKET_AGPPTITLE_USE_NEAR* )pPacket;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pData;
	if( !ppdCharacter ) return FALSE;

	if( pResult->bUse )
	{
		pcmTitle->OnActivateTitle( pResult->nTitleID, ppdCharacter, FALSE );
	}
	else
	{
		pcmTitle->OnDeActivateTitle( pResult->nTitleID, ppdCharacter, FALSE );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_RecvTitleOpenUI( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	pcmTitle->OnOpenUI( TitleUi_ByNPC );
	pcmTitle->OnOpenUI( TitleUi_Mini );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnRecvSelfCharacter( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	return pcmTitle->OnSetSelfCharacter( ppdCharacter );
}

BOOL AgcmTitle::CallBack_AutoCloseByCharacterMove( void* pData, void* pClass, void* pCustData )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	pcmTitle->OnCloseUI( TitleUi_ByNPC );
	pcmTitle->OnCloseUI( TitleUi_Mini );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnOpenUIAll( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	pcmTitle->OnOpenUI( TitleUi_ByNPC );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnOpenUICurrent( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	pcmTitle->OnOpenUI( TitleUi_ByHotKey );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnOpenUIMini( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	pcmTitle->OnOpenUI( TitleUi_Mini );
	return TRUE;
}

BOOL AgcmTitle::CallBack_DisplayCategoryName( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pData ) return FALSE;

	int nIndex = *( int* )pData;

	char* pCategoryName = pcmTitle->GetCategoryNameByIndex( nIndex );
	if( pCategoryName && strlen( pCategoryName ) > 0 )
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, pCategoryName );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_DisplayTitleName( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pData ) return FALSE;

	int nIndex = *( int* )pData;

	char* pTitleName = pcmTitle->GetTitleNameByIndexWithPage( nIndex );
	if( pTitleName && strlen( pTitleName ) > 0 )
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, pTitleName );

		if( pControl )
		{
			eTitleStateType eTitleState = pcmTitle->GetTitleStateByIndexWidthPage( nIndex );
			pControl->m_stDisplayMap.m_stFont.m_ulColor = pcmTitle->GetTitleFontColor( eTitleState );
		}
	}
	else
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, "" );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_DisplayTitleNameCurrent( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pData ) return FALSE;

	int nIndex = *( int* )pData;

	char* pTitleName = pcmTitle->GetCurrTitleNameByIndexWithPage( nIndex );
	if( pTitleName && strlen( pTitleName ) > 0 )
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, pTitleName );

		if( pControl )
		{
			eTitleStateType eTitleState = pcmTitle->GetCurrTitleStateByIndex( nIndex );
			pControl->m_stDisplayMap.m_stFont.m_ulColor = pcmTitle->GetTitleFontColor( eTitleState );
		}
	}
	else
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, "" );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_DisplayTitleNameMini( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pData ) return FALSE;

	int nIndex = *( int* )pData;

	char* pTitleName = pcmTitle->GetCurrTitleNameByIndexWithPage( nIndex );
	if( pTitleName && strlen( pTitleName ) > 0 )
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, pTitleName );

		if( pControl )
		{
			DWORD dwTextColor = 0xFFFFFFFF;
			eTitleStateType eTitleState = pcmTitle->GetCurrTitleStateByIndex( nIndex );
			pControl->m_stDisplayMap.m_stFont.m_ulColor = pcmTitle->GetTitleFontColor( eTitleState );
		}
	}
	else
	{
		strcpy_s( pDisplay, STRING_LENGTH_TITLE_NAME, "" );
	}

	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickCategoryItem( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pControl ) return FALSE;
	if( !pControl->m_pcsParentControl ) return FALSE;

	AcUIList* pList = ( AcUIList* )pControl->m_pcsParentControl->m_pcsBase;
	if( !pList ) return FALSE;

	AcUIListItem* pSeletedItem = ( AcUIListItem* )pControl->m_pcsBase;
	if( !pSeletedItem ) return FALSE;

	pcmTitle->OnSelectCategoryByIndex( pSeletedItem->m_lItemIndex );
	pList->SelectItem( pSeletedItem->m_lItemIndex );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickTitleItem( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pControl ) return FALSE;

	AcUIListItem* pSeletedItem = ( AcUIListItem* )pControl->m_pcsBase;
	pcmTitle->OnSelectTitleByIndex( pSeletedItem->m_lItemIndex );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickTitleItemCurrent( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pControl ) return FALSE;

	AcUIListItem* pSeletedItem = ( AcUIListItem* )pControl->m_pcsBase;
	pcmTitle->OnSelectTitleCurrentByIndex( pSeletedItem->m_lItemIndex );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickTitleItemMini( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pControl ) return FALSE;

	AcUIListItem* pSeletedItem = ( AcUIListItem* )pControl->m_pcsBase;
	pcmTitle->OnSelectTitleMiniByIndex( pSeletedItem->m_lItemIndex );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickPagePrev( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	int nPage = pcmTitle->GetTitlePageIndex() - 5;
	pcmTitle->SetTitlePageIndex( nPage );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickPageNext( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	int nPage = pcmTitle->GetTitlePageIndex() + 5;
	pcmTitle->SetTitlePageIndex( nPage );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickPage( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pControl ) return FALSE;

	AcUIButton* pBtn = ( AcUIButton* )pControl->m_pcsBase;
	if( !pBtn || !pBtn->m_szStaticString ) return FALSE;

	int nPageNumber = 0;
	if( strlen( pBtn->m_szStaticString ) > 0 )
	{
		nPageNumber = atoi( pBtn->m_szStaticString );
	}

	pcmTitle->SetTitlePageIndex( nPageNumber );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickSetTargetTitle( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	pcmTitle->OnSendSetTargetTitle();
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickActivateTitle( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;
	if( !pControl ) return FALSE;

	AcUIButton* pcdUIButton = ( AcUIButton* )pControl->m_pcsBase;
	if( !pcdUIButton ) return FALSE;

	pcmTitle->OnSendTitleActivate();	
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickSurrenderTitleCurrent( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	pcmTitle->OnSendSurrenderTitle( TitleUi_ByHotKey );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickSurrenderTitleMini( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	pcmTitle->OnSendSurrenderTitle( TitleUi_Mini );
	return TRUE;
}

BOOL AgcmTitle::CallBack_OnClickCompleteTitle( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmTitle* pcmTitle = ( AgcmTitle* )pClass;
	if( !pcmTitle ) return FALSE;

	pcmTitle->OnSendCompleteTitle();
	return TRUE;
}

BOOL AgcmTitle::LoadXMLFileToDocument( void* pXmlDoc, char* pFileName, BOOL bIsEncrypt )
{
	TiXmlDocument* pDoc = ( TiXmlDocument* )pXmlDoc;
	if( !pDoc ) return FALSE;
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	BOOL bIsResult = FALSE;
	if( bIsEncrypt )
	{
		HANDLE hFile = ::CreateFile( pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ) return FALSE;

		// 마지막에 NULL 문자열을 추가해야 하니까 파일사이즈 + 1 해서 초기화한다.
		DWORD dwBufferSize = ::GetFileSize( hFile, NULL ) + 1;
		char* pBuffer = new char[ dwBufferSize ];
		memset( pBuffer, 0, sizeof( char ) * dwBufferSize );

		DWORD dwReadByte = 0;
		if( ::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadByte, NULL ) )
		{
			AuMD5Encrypt Cryptor;
#ifdef _AREA_CHINA_
			if( Cryptor.DecryptString( MD5_HASH_KEY_STRING, pBuffer, dwReadByte ) )
#else
			if( Cryptor.DecryptString( "1111", pBuffer, dwReadByte ) )
#endif
			{
				pDoc->Parse( pBuffer );
				if( !pDoc->Error() )
				{
					bIsResult = TRUE;
				}
			}
		}

		delete[] pBuffer;
		pBuffer = NULL;

		::CloseHandle( hFile );
		hFile = NULL;
	}
	else
	{
		if( pDoc->LoadFile( pFileName ) )
		{
			bIsResult = TRUE;
		}
	}

	return bIsResult;
}