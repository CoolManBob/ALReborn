#include "AgcmUIAccountCheck.h"
#include "AgcmUIManager2.h"
#include "AgcmUILogin.h"
#include "AgcmWorld.h"
#include "AuStrTable.h"

#ifndef USE_MFC
	#include "AuJapaneseClientAuth.h"
#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
#endif
#endif



AgcmUIAccountCheck::AgcmUIAccountCheck( void )
: m_pcmUILogin( NULL ), m_pChallengeString( NULL ), m_pcdUIUserDataKeyChallenge( NULL ),
	m_nEventKeyOpen( 0 ), m_nEventKeyClose( 0 ), m_nEventNotProtected( 0 ), m_nRetryCount( 0 )
{
	OnInitialize( NULL );
}

AgcmUIAccountCheck::~AgcmUIAccountCheck( void )
{
	OnInitialize( NULL );
}

BOOL AgcmUIAccountCheck::OnInitialize( void* pUILogin )
{
	if( !pUILogin ) return FALSE;
	m_pcmUILogin = pUILogin;
	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddEvent( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( pcmUIManager ) return FALSE;

	m_nEventKeyOpen = pcmUIManager->AddEvent( "LG_OpenEKey" );
	if( m_nEventKeyOpen < 0 ) return FALSE;

	m_nEventKeyClose = pcmUIManager->AddEvent( "LG_CloseEKey" );
	if( m_nEventKeyClose < 0 ) return FALSE;

	m_nEventNotProtected = pcmUIManager->AddEvent( "LG_NotIsProtected" );
	if( m_nEventNotProtected < 0 ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordOK",		CB_OnSubmit,	2, "ID_EDIT", "PASSWORD_EDIT" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordCancel",	CB_OnCancel,	0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordEditTab",	CB_OnKeyTab,	2, "Pre_EDIT", "Next_EDIT" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_EKeyOK",				CB_OnOpenKey,	1, "EKey_EDIT" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_EKeyCancel",			CB_OnCloseKey,	0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_LoginToServer",		CB_OnSendLogin, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordInit",		CB_OnClear,		2, "Id_Edit", "Password_Edit" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "EXIT_ApplicationQUIT",	CB_OnExitGame,	0 ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_pcdUIUserDataKeyChallenge	= pcmUIManager->AddUserData( "EKEY_Challenge", m_pChallengeString, sizeof( CHAR* ), 5, AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pcdUIUserDataKeyChallenge ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	if( !pcmUIManager->AddDisplay( this, "EKEY_Display_Challenge", 0, CB_OnDisplayChallenge, AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddCallBack( void* pLogin )
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )pLogin;
	if( !pcmLogin ) return FALSE;

	if( !pcmLogin->SetCallbackEKeyActive( CB_OnActiveKey, this ) ) return FALSE;
	if( !pcmLogin->SetCallbackSignOnSuccess( CB_OnLoginOK, this ) ) return FALSE;

	if( !pcmLogin->SetCallbackConnectLoginServer( CB_OnConnect, this ) ) return FALSE;
	if( !pcmLogin->SetCallbackDisconnectLoginServer( CB_OnDisConnect, this ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddLoginRetryCount( void* pUIManager )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	++m_nRetryCount;
	if( m_nRetryCount < 5 )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_PASSWORD_INCORRECT ] );
	}
	else
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_PASSWORD_FALIED ] );
	}

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnClear( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcdUIControl* pEditAccount = ( AgcdUIControl* )pData1;
	AgcdUIControl* pEditPassword = ( AgcdUIControl* )pData2;
	if( !pEditAccount || !pEditPassword ) return FALSE;
	if( pEditAccount->m_lType != AcUIBase::TYPE_EDIT || pEditPassword->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	( ( AcUIEdit* )pEditAccount->m_pcsBase )->ClearText();
	( ( AcUIEdit* )pEditPassword->m_pcsBase )->ClearText();

	if( g_pEngine )
	{
		g_pEngine->ReleaseMeActiveEdit( NULL );
	}

	( ( AcUIEdit* )pEditAccount->m_pcsBase )->SetMeActiveEdit();
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnSubmit( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
#ifdef _AREA_JAPAN_
	if( g_jAuth.GetAutoLogin() )	
		return FALSE;
#elif defined(_AREA_KOREA_) && !defined( USE_MFC )
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( pWebzenAuth && pWebzenAuth->IsAutoLogin() )	return FALSE;
#endif

	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmUIManager || !pcmLogin ) return FALSE;

	pcmUILogin->ConnectLoginServer();

	AgcdUIControl* pEditAccount = ( AgcdUIControl* )pData1;
	AgcdUIControl* pEditPassword = ( AgcdUIControl* )pData2;
	if( !pEditAccount || !pEditPassword ) return FALSE;
	if( pEditAccount->m_lType != AcUIBase::TYPE_EDIT || pEditPassword->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	TCHAR* pTextAccount = ( CHAR* )( ( AcUIEdit* )pEditAccount->m_pcsBase )->GetText();
	TCHAR* pTextPassword = ( CHAR* )( ( AcUIEdit* )pEditPassword->m_pcsBase )->GetText();

	if( !pTextAccount || strlen( pTextAccount ) <= 0 ) return FALSE;
	if( !pTextPassword || strlen( pTextPassword ) <= 0 ) return FALSE;

	pcmLogin->SetIDPassword( pTextAccount, pTextPassword );

	
/*
	pcmLogin->ConnectLoginServer();

	if( g_pEngine )
	{
		g_pEngine->WaitingDialog( NULL, pcmUIManager->GetUIMessage( "LOGIN_WAITING" ) );
	}
	*/

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnCancel( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnKeyTab( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcdUIControl* pEditAccount = ( AgcdUIControl* )pData1;
	AgcdUIControl* pEditPassword = ( AgcdUIControl* )pData2;
	if( !pEditAccount || !pEditPassword ) return FALSE;
	if( pEditAccount->m_lType != AcUIBase::TYPE_EDIT || pEditPassword->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	( ( AcUIEdit* )( pEditAccount->m_pcsBase ) )->ReleaseMeActiveEdit();
	( ( AcUIEdit* )( pEditPassword->m_pcsBase ) )->SetMeActiveEdit();
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnSendLogin( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	pcmLogin->ConnectLoginServer();
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnExitGame( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	RsEventHandler( rsQUITAPP, NULL );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnOpenKey( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	AgcdUIControl* pEdit = ( AgcdUIControl* )pData1;
	if( !pEdit || pEdit->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	TCHAR* pKeyString = ( CHAR* )( ( AcUIEdit* )( pEdit->m_pcsBase ) )->GetText();
	pcmLogin->SendEKey( pKeyString, pcmLogin->m_nNID );

	( ( AcUIEdit* )( pEdit->m_pcsBase ) )->SetText( "" );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnCloseKey( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( pThis->m_nEventKeyClose );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnConnect( void* pData, void* pClass, void* pCustomData )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	if( !pData )
	{
		if( pcmLogin->GetLoginMode() == AGCMLOGIN_MODE_ID_PASSWORD_INPUT )
		{
			if( g_pEngine )
			{
				// Login서버 재접속이 더이상 불가능하면 종료한다
				if( !pThis->ReConnectLoginServer() )
				{
					g_pEngine->WaitingDialogEnd();
					pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CONNECT_FAILED_LOGIN_SERVER ] );
					g_pEngine->OnTerminate();
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnDisConnect( void* pData, void* pClass, void* pCustomData )
{
	if( g_pEngine )
	{
		g_pEngine->WaitingDialogEnd();
	}

	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	BOOL* pbIsDestroyNormal = ( BOOL* )pCustomData;
	if( !pbIsDestroyNormal ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmUILogin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	if( !( *pbIsDestroyNormal ) && _tcslen( pcmCharacter->m_szGameServerAddress ) < 1 )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER ] );
	}

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnActiveKey( void* pData, void* pClass, void* pCustomData )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	if( g_pEngine )
	{
		g_pEngine->WaitingDialogEnd();
	}

	pThis->m_pChallengeString = ( CHAR* )pData;
	pcmUIManager->SetUserDataRefresh( ( AgcdUIUserData* )pThis->GetUserDataKeyChallenge() );
	pcmUIManager->ThrowEvent( pThis->GetEKeyOpen() );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnLoginOK( void* pData, void* pClass, void* pCustomData )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	AgcmWorld* pcmWorld = ( AgcmWorld* )pcmUILogin->GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;
	if( !pcmWorld->SendPacketGetWorld( NULL, pcmLogin->m_nNID ) ) return FALSE;

	static bool bIsFirstCall = true;

	if( g_eServiceArea == AP_SERVICE_AREA_CHINA )
	{
		if( pcmLogin->m_lIsProtected == 0 && bIsFirstCall )
		{
			pcmLogin->m_lIsLimited = 0;
			pcmUIManager->ThrowEvent( pThis->GetENotProtected() );
		}
	}

	pcmWorld->SendPacketGetCharCount( pcmLogin->m_szAccount, pcmLogin->m_nNID );
	pcmUILogin->SetLoginMode( AGCMLOGIN_MODE_SERVER_SELECT );
	return pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_CLOSE ] );
}

BOOL AgcmUIAccountCheck::CB_OnDisplayChallenge( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pDisplay ) return FALSE;

	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	if( pThis->GetChallengeString() )
	{
		INT32 nChallenge = atoi( pThis->GetChallengeString() );
		CHAR strTemp[ 12 ] = { 0, };

		if( nChallenge < 1000 )
		{
			strcpy( strTemp, "XXXX" );
		}
		else
		{
			sprintf( strTemp, "%d", nChallenge );
		}

		sprintf( pDisplay, ClientStr().GetStr( STI_INPUT_EKEY ), strTemp );
	}
	else
	{
		sprintf( pDisplay, "" );
	}

	return TRUE;
}