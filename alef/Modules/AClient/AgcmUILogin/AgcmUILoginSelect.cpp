#include "AgcmUILoginSelect.h"
#include "AcUIEventButton.h"
#include "CWebzenAuth.h"

AgcmUILoginSelect::AgcmUILoginSelect( VOID )
{
}

AgcmUILoginSelect::~AgcmUILoginSelect( VOID )
{
}

BOOL	AgcmUILoginSelect::Initialize( AgcmUIManager2* pManager , AgcmLogin*	pcmLogin )
{
	if( !pManager )
		return FALSE;

	m_pUIManager	=	pManager;
	m_pcmLogin		=	pcmLogin;

	// Login 윈도우를 만든다.
	m_csLoginSelectUI.m_Property.bTopmost		=	TRUE;
	m_csLoginSelectUI.m_Property.bModal			=	TRUE;
	m_csLoginSelectUI.ShowWindow( FALSE );

	// Archlord.ini를 읽어온다.
	m_ArchlordLoginInfo.LoadFile( FALSE );

	// Login 윈도우에 Button과 이벤트 등록을 한다
	_LoginSelectWindowInit();

	return TRUE;
}

VOID	AgcmUILoginSelect::OpenLoginSelect( VOID )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	AgcmLogin* pcmLogin = ( AgcmLogin* )g_pEngine->GetModule( "AgcmLogin" );
	Initialize( pcmUIManager, pcmLogin );
	INT	nGroupCount	=	m_ArchlordLoginInfo.GetGroupCount();

	// Group이 존재하면 선택창을 띄운다.
	if( nGroupCount > 0 )
	{
		g_pEngine->m_pCurrentFullUIModule->AddChild((AgcWindow*)&m_csLoginSelectUI);
		m_csLoginSelectUI.ShowWindow( TRUE );
		m_csLoginSelectUI.SetModal();
	}

	// Group이 없을 경우에는 레지스트리의 정보로 접속시도한다.
	else
	{
		AgcmLogin* pcmLogin = ( AgcmLogin* )g_pEngine->GetModule( "AgcmLogin" );
		if( !pcmLogin ) return;

		if( pcmLogin->ConnectLoginServer() >= 0 )
		{
			if( g_pEngine )
			{
				g_pEngine->WaitingDialog( NULL, m_pUIManager->GetUIMessage( "LOGIN_WAITING" ) );
			}
		}
	}

}

VOID	AgcmUILoginSelect::CloseLoginSelect( VOID )
{
	g_pEngine->m_pCurrentFullUIModule->DeleteChild((AgcWindow*)&m_csLoginSelectUI);
	m_csLoginSelectUI.ShowWindow( FALSE );
	m_csLoginSelectUI.ReleaseModal();
}

BOOL	AgcmUILoginSelect::_LoginSelectWindowInit( VOID )
{
	INT GroupCount	=	m_ArchlordLoginInfo.GetGroupCount();


	for( INT i = 0 ; i < GroupCount ; ++i )
	{
		stLoginGroup*	pGroup	=	m_ArchlordLoginInfo.GetGroup( i );
		if( !pGroup )
			continue;

		// 버튼을 만들어서 붙인다
		AcUIEventButton*		pUIButton	=	new	AcUIEventButton;

		pUIButton->SetButtonEnable( TRUE );
		pUIButton->AddButtonImage( "Common_Button_Large_A.png"	, ACUIBUTTON_MODE_NORMAL	);
		pUIButton->AddButtonImage( "Common_Button_Large_B.png"	, ACUIBUTTON_MODE_ONMOUSE	);
		pUIButton->AddButtonImage( "Common_Button_Large_C.png"	, ACUIBUTTON_MODE_CLICK		);
		pUIButton->AddButtonImage( "Common_Button_Large_D.png"	, ACUIBUTTON_MODE_DISABLE	);

		pUIButton->SetStaticStringExt( (CHAR*)pGroup->m_strGroupName.c_str(), 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
		pUIButton->MoveWindow( 20 , i*25+30 , 158 , 17 );

		pUIButton->SetCallbackClickEvent( this , CBSelectLoginServer );
		pUIButton->SetCallbackSetFocusEvent( this, CBMouseOnLoginServer );

		m_csLoginSelectUI.AddChild( pUIButton );		
	}

	m_csLoginSelectUI.MoveWindow(		m_pUIManager->m_lWindowWidth / 2 - 178 / 2 
		,	m_pUIManager->m_lWindowHeight / 2 - (GroupCount*25 + 30) / 2
		,	198 , GroupCount*25 + 47	);

	return TRUE;	
}

stLoginGroup*	AgcmUILoginSelect::GetLoginGroup( CONST std::string strGroupName )
{
	stLoginGroup*	pGroup		=	NULL;
	INT				nGroupCount	=	m_ArchlordLoginInfo.GetGroupCount();
	for ( INT i = 0 ; i < nGroupCount ; ++i )
	{
		pGroup	=	m_ArchlordLoginInfo.GetGroup( i );

		if( pGroup->m_strGroupName.compare( strGroupName ) == 0 )
		{
			m_strLastSelectGroup	=	strGroupName;
			return pGroup;
		}
	}

	return NULL;
}

BOOL	AgcmUILoginSelect::CBSelectLoginServer( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton )
{
#ifndef USE_MFC
	AgcmUILoginSelect*	pThis			=	static_cast< AgcmUILoginSelect* >(pClass );
	AcUIEventButton*	pEventButton	=	static_cast< AcUIEventButton*	>(pButton);


	stLoginGroup*	pGroup		=	pThis->GetLoginGroup( pEventButton->m_szStaticString );
	if( !pGroup )
		return FALSE;

	srand( time(NULL) );
	INT nRand	=	rand() % pGroup->m_vecServerInfo.size();

	pThis->CloseLoginSelect();

	/*CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( pWebzenAuth && pWebzenAuth->IsAutoLogin() )
	{
		pThis->m_pcmLogin->SetIDPassword( ( char* )pWebzenAuth->GetGameID(), "autologin" );
	}*/


	pThis->m_pcmLogin->ConnectLoginServer( pGroup->m_vecServerInfo[ nRand ].m_strServerIP.c_str() );	
	pGroup->m_vecServerInfo[ nRand ].m_bTryConnect	=	TRUE;

	if( g_pEngine )
	{
		g_pEngine->WaitingDialog( NULL, pThis->m_pUIManager->GetUIMessage( "LOGIN_WAITING" ) );
	}
#endif

	return TRUE;

}

BOOL	AgcmUILoginSelect::CBMouseOnLoginServer( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton )
{
	AgcmUILoginSelect*	pThis			=	static_cast< AgcmUILoginSelect* >(pClass );
	if( !pThis ) return FALSE;

	pThis->m_pUIManager->ThrowEvent( pThis->m_pUIManager->m_nEventServerSelectMouseOverSound );

	return TRUE;
}

BOOL	AgcmUILoginSelect::IsRetryLoginServer( VOID )
{

	stLoginGroup*	pGroup	=	GetLoginGroup( m_strLastSelectGroup );
	if( pGroup )
	{
		// 접속 가능한 서버가 하나라도 남아 있으면 TRUE
		VecServerInfoIter	Iter	=	pGroup->m_vecServerInfo.begin();
		for( ; Iter != pGroup->m_vecServerInfo.end() ; ++Iter )
		{
			if( !Iter->m_bTryConnect )
				return TRUE;
		}
	}

	// 접속 가능한 서버가 없다
	return FALSE;
}

VOID	AgcmUILoginSelect::ReConnectLoginServer( VOID )
{
	std::string			strConnectIP;
	stLoginGroup*		pGroup	=	GetLoginGroup( m_strLastSelectGroup );
	VecServerInfoIter	Iter	=	pGroup->m_vecServerInfo.begin();
	for( ; Iter != pGroup->m_vecServerInfo.end() ; ++Iter )
	{
		if( Iter->m_bTryConnect	)
			continue;

		strConnectIP		=	Iter->m_strServerIP;
		Iter->m_bTryConnect	=	TRUE;
		break;
	}

	m_pcmLogin->ConnectLoginServer( strConnectIP.c_str() );

}