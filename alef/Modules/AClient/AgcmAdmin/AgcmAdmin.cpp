#include "AgcmAdmin.h"
#include "AgcmUIManager2.h"



AgcmAdmin::AgcmAdmin()
{
	SetModuleName("AgcmAdmin");
	EnableIdle(TRUE);
	m_hDLL = NULL;
}

AgcmAdmin::~AgcmAdmin()
{
}

BOOL AgcmAdmin::OnAddModule()
{
	StartAdminClient();
	m_pagpmAdmin = (AgpmAdmin*)GetModule("AgpmAdmin");
	m_pagcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");

	if(!m_pagpmAdmin || !m_pagcmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgcmAdmin::OnInit()
{
	return TRUE;
}

BOOL AgcmAdmin::OnIdle(UINT32 ulClockCount)
{
	ADMIN_CB fnProc = GetDLLFunction( "ADMIN_Main_OnIdle" );
	if( fnProc )
	{
		return fnProc( NULL, NULL, NULL );
	}

	return TRUE;
}

BOOL AgcmAdmin::OnDestroy()
{
	// Admin DLL ÀÇ Instance »èÁ¦
	ADMIN_CB fnProc = GetDLLFunction( "ADMIN_Common_DeleteInstance" );
	if( fnProc )
	{
		fnProc( NULL, NULL, NULL );
	}

	if( m_hDLL )
	{
		FreeLibrary( m_hDLL );
		m_hDLL = NULL;
	}
	return TRUE;
}

// Dialog Message - 2004.01.12
BOOL AgcmAdmin::CBIsDialogMessage(PVOID pMSG, PVOID pClass)
{
	MSG* pMessage = ( MSG* )pMSG;
	AgcmAdmin* pThis = ( AgcmAdmin* )pClass;
	if( !pMessage || !pThis ) return FALSE;

	ADMIN_CB fnProc = pThis->GetDLLFunction( "ADMIN_Main_OnDlgMessageProc" );
	if( fnProc )
	{
		return fnProc( pMessage, NULL, NULL );
	}

	return TRUE;
}

BOOL AgcmAdmin::SendAdminClientRequest()
{
	AgpdCharacter* pstCharacter = m_pagcmCharacter->GetSelfCharacter();

	if(!pstCharacter)
		return FALSE;

	INT16 nAdminLevel = m_pagpmAdmin->GetAdminLevel(pstCharacter);

	if(nAdminLevel < AGPMADMIN_LEVEL_ADMIN)
		return FALSE;

	stAgpdAdminServerInfo stClientInfo;
	stClientInfo.m_lPort = 0;

	strncpy(stClientInfo.m_szServerIP, "", AGPMADMIN_MAX_IP_STRING);

	PVOID pvPacket = NULL;
	PVOID pvClientInfoPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_ADMIN_CLIENT_LOGIN;

	pvClientInfoPacket = m_pagpmAdmin->MakeServerInfoPacket(&nPacketLength, stClientInfo.m_szServerIP, &stClientInfo.m_lPort);

	if(!pvClientInfoPacket)
		return FALSE;

	pvPacket = m_pagpmAdmin->MakeAdminPacket(TRUE, &nPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, &pstCharacter->m_lID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvClientInfoPacket, NULL);

	m_pagpmAdmin->m_csPacket.FreePacket(pvClientInfoPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = m_pagpmAdmin->SendPacket(pvPacket, nPacketLength);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmAdmin::StartAdminClient( void )
{
	if( !m_hDLL )
	{
#ifdef _DEBUG
		m_hDLL = LoadLibrary( "AlefAdminDLLd.dll" );
#else
		m_hDLL = LoadLibrary( "AlefAdminDLL.dll" );
#endif
	}

	if( !m_hDLL ) return FALSE;

	ADMIN_CB fnProc = GetDLLFunction( "ADMIN_Common_SetAdminModule" );
	if( fnProc )
	{
		if( !fnProc( this, NULL, NULL ) ) return FALSE;
	}

	return TRUE;
}

CHAR* AgcmAdmin::GetUIMessageString( CHAR* pKeyString )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return NULL;

	return pcmUIManager->GetUIMessage( pKeyString );
}

ADMIN_CB AgcmAdmin::GetDLLFunction( CHAR* pFunctionName )
{
	if( !m_hDLL ) return NULL;
	return ( ADMIN_CB )GetProcAddress( m_hDLL, pFunctionName );
}