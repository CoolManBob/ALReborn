#include "AgcmReturnToLogin.h"
#include "AgcmNatureEffect.h"
#include "AgcmUILogin.h"
#include "AgcmUIGuild.h"

AgcmReturnToLogin::AgcmReturnToLogin()
{
	SetModuleName("AgcmReturnToLogin");

	m_lAuthKey	= 0;
}

AgcmReturnToLogin::~AgcmReturnToLogin()
{
}

BOOL AgcmReturnToLogin::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmReturnToLogin	= (AgpmReturnToLogin *)		GetModule("AgpmReturnToLogin");
	m_pcsAgcmCharacter		= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmConnectManager	= (AgcmConnectManager *)	GetModule("AgcmConnectManager");
	m_pcsAgcmLogin			= (AgcmLogin *)				GetModule("AgcmLogin");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmReturnToLogin ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmConnectManager ||
		!m_pcsAgcmLogin)
		return FALSE;

	if (!m_pcsAgpmReturnToLogin->SetCallbackRequestFailed(CBReceiveRequestFailed, this))
		return FALSE;
	if (!m_pcsAgpmReturnToLogin->SetCallbackSendKeyAddr(CBReceiveAuthKey, this))
		return FALSE;
	if (!m_pcsAgpmReturnToLogin->SetCallbackEndProcess(CBEndProcess, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmReturnToLogin::SendPacketRequest()
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketRequest(m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength);
}

BOOL AgcmReturnToLogin::SetCallbackRequestFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRETURN_CB_REQUEST_FAILED, pfCallback, pClass);
}

BOOL AgcmReturnToLogin::SetCallbackEndProcess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRETURN_CB_END_PROCESS, pfCallback, pClass);
}

BOOL AgcmReturnToLogin::SetCallBackReconnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRETURN_CB_RECONNECT_LOGINSERVER, pfCallback, pClass);
}

BOOL AgcmReturnToLogin::CBReceiveRequestFailed(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmReturnToLogin	*pThis	= (AgcmReturnToLogin *)	pClass;

	return pThis->EnumCallback(AGCMRETURN_CB_REQUEST_FAILED, NULL, NULL);
}

BOOL AgcmReturnToLogin::CBReceiveAuthKey(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmReturnToLogin	*pThis	= (AgcmReturnToLogin *)	pClass;
	INT32	lAuthKey	= *(INT32 *) pData;
	CHAR	*pszLoginServerAddr	= (CHAR *) pCustData;

	((AgcEngine *) pThis->GetModuleManager())->m_SocketManager.SetDestroyNormal();

	pThis->m_pcsAgpmCharacter->RemoveAllCharacters();

	AgcmNatureEffect * pcsAgcmNatureEffect = static_cast< AgcmNatureEffect * >( pThis->GetModule("AgcmNatureEffect") );
	if( pcsAgcmNatureEffect )
	{
		pcsAgcmNatureEffect->StopNatureEffect( TRUE );
	}

	AgcmUILogin	* pcsAgcmUILogin	= static_cast< AgcmUILogin	* >( pThis->GetModule("AgcmUILogin" ) );
	if( pcsAgcmUILogin )
	{
		pcsAgcmUILogin->OpenLoadingWindow( AURACE_TYPE_HUMAN );
		pcsAgcmUILogin->SetLoginMode(AGCMLOGIN_MODE_WAIT_MY_CHARACTER);
	}

	AgcmUIGuild*	pUIGuild = static_cast< AgcmUIGuild* >(pThis->GetModule( "AgcmUIGuild" ));
	pUIGuild->InitMemberUIData();

	return pThis->ReconnectLoginServer(lAuthKey, pszLoginServerAddr);
}

BOOL AgcmReturnToLogin::CBEndProcess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmReturnToLogin	*pThis	= (AgcmReturnToLogin *)	pClass;

	pThis->EnumCallback(AGCMRETURN_CB_END_PROCESS, NULL, NULL);

	pThis->m_lAuthKey	= 0;

	return pThis->m_pcsAgcmLogin->SendGetUnion(NULL, pThis->m_pcsAgcmLogin->m_szAccount);
}

BOOL AgcmReturnToLogin::ReconnectLoginServer(INT32 lAuthKey, CHAR *pszLoginServerAddr)
{
	//if (!pszLoginServerAddr || !pszLoginServerAddr[0])
	//	return FALSE;

	m_lAuthKey	= lAuthKey;

	CHAR strServerAddress[ 24 ] = { 0, };
	if( !pszLoginServerAddr || strlen( pszLoginServerAddr ) <= 0 )
	{
		AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
		if( !pcmLogin ) return -1;

		strncpy( strServerAddress, pcmLogin->m_szIPAddress, 23 );
	}
	else
	{
		strncpy( strServerAddress, pszLoginServerAddr, 23 );
	}

	return m_pcsAgcmConnectManager->Connect(strServerAddress, ACDP_SERVER_TYPE_LOGINSERVER, this, CBSocketOnConnect, NULL, NULL);
}

BOOL AgcmReturnToLogin::CBSocketOnConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmReturnToLogin	*pThis	= (AgcmReturnToLogin *)	pClass;

	//@{ 2006/09/21 burumal
	//pThis->EnumCallback(AGCMRETURN_CB_RECONNECT_LOGINSERVER, NULL, NULL);
	return pThis->EnumCallback(AGCMRETURN_CB_RECONNECT_LOGINSERVER, NULL, NULL);
	//@}

	//INT16	nPacketLength	= 0;
	//PVOID	pvPacket		= pThis->m_pcsAgpmReturnToLogin->MakePacketReconnectLoginServer(pThis->m_pcsAgcmLogin->m_szAccount, pThis->m_lAuthKey, &nPacketLength);

	//if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
	//	return FALSE;

	//return pThis->SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, pThis->m_pcsAgcmConnectManager->GetLoginServerNID());
}

BOOL AgcmReturnToLogin::SendReconnectLoginServer()
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketReconnectLoginServer(m_pcsAgcmLogin->m_szAccount, m_lAuthKey, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_pcsAgcmConnectManager->GetLoginServerNID());
}