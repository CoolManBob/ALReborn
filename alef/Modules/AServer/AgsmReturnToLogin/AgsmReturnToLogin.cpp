#include "AgsmReturnToLogin.h"

AgsmReturnToLogin::AgsmReturnToLogin()
{
	SetModuleName("AgsmReturnToLogin");

	m_csAuthKeyPoolAdmin.Initialize();

	EnableIdle(TRUE);
}

AgsmReturnToLogin::~AgsmReturnToLogin()
{
}

BOOL AgsmReturnToLogin::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmReturnToLogin	= (AgpmReturnToLogin *)		GetModule("AgpmReturnToLogin");
	m_pcsAgsmAccountManager	= (AgsmAccountManager *)	GetModule("AgsmAccountManager");
	m_pcsAgsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmServerManager2	= (AgsmServerManager2 *)	GetModule("AgsmServerManager2");
	//m_pcsAgsmLoginDB		= (AgsmLoginDB *)			GetModule("AgsmLoginDB");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmReturnToLogin ||
		!m_pcsAgsmAccountManager ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmServerManager2)
		return FALSE;

	if (!m_pcsAgpmReturnToLogin->SetCallbackRequest(CBRequest, this))
		return FALSE;
	if (!m_pcsAgpmReturnToLogin->SetCallbackRequestKey(CBRequestKey, this))
		return FALSE;
	if (!m_pcsAgpmReturnToLogin->SetCallbackResponseKey(CBResponseKey, this))
		return FALSE;
	if (!m_pcsAgpmReturnToLogin->SetCallbackReconnectLoginServer(CBReconnectLoginServer, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmReturnToLogin::OnIdle(UINT32 ulClockCount)
{
	m_csAuthKeyPoolAdmin.ProcessTimeout(ulClockCount, 120000);

	return TRUE;
}

BOOL AgsmReturnToLogin::CBRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmReturnToLogin	*pThis	= (AgsmReturnToLogin *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->ReturnToLoginServer(pcsCharacter);
}

BOOL AgsmReturnToLogin::CBRequestKey(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmReturnToLogin	*pThis	= (AgsmReturnToLogin *)	pClass;
	PVOID	*ppvBuffer		= (PVOID *)	pData;
	if (!ppvBuffer || !ppvBuffer[2])
		return FALSE;

	CHAR	*pszAccountName	= (CHAR *)		ppvBuffer[0];
	CHAR	*pszServerName	= (CHAR *)		ppvBuffer[1];
	UINT32	ulNID			= *(UINT32 *)	ppvBuffer[2];

	AgsdAuthKey	*pcsAgsdAuthKey	= pThis->m_csAuthKeyPoolAdmin.AddAuthKey(pszAccountName, pszServerName, pThis->GetClockCount());
	if (!pcsAgsdAuthKey)
	{
		pThis->SendPacketResponseKey(pszAccountName, (-1), ulNID);
		return FALSE;
	}

	return pThis->SendPacketResponseKey(pszAccountName, pcsAgsdAuthKey->GetAuthKey(), ulNID);
}

BOOL AgsmReturnToLogin::CBResponseKey(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmReturnToLogin	*pThis	= (AgsmReturnToLogin *)	pClass;
	CHAR	*pszAccountName	= (CHAR *)	pData;
	INT32	lAuthKey	= *(INT32 *)	pCustData;

	AgsdAccount	*pcsAgsdAccount	= pThis->m_pcsAgsmAccountManager->GetAccountLock(pszAccountName);
	if (!pcsAgsdAccount || pcsAgsdAccount->m_nNumChar < 1)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacterLock(pcsAgsdAccount->m_szCharName[0]);
	if (!pcsCharacter)
		return FALSE;

	BOOL	bResult				= TRUE;

	BOOL	bRemoveCharacter	= FALSE;

	if (lAuthKey > 0)
	{
		bResult	= pThis->SendPacketReceivedAuthKey(pcsCharacter, lAuthKey);
	}
	else
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "lAuthKey <= 0 \n");
		AuLogFile_s("LOG\\ReturnToLoginErr.txt", strCharBuff);

		pThis->SendPacketRequestFailed(pcsCharacter);

		bResult	= pThis->m_pcsAgpmCharacter->ClearActionBlockTime(pcsCharacter);
	}

	pcsCharacter->m_Mutex.Release();
	pcsAgsdAccount->m_Mutex.Release();

	if (!bResult)
	{
		//if (pThis->m_pcsAgpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
		//	AuLogFile("RemoveNPC.log", "Removed by CBResponseKey()\n");

		pThis->m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
	}

	return bResult;
}

BOOL AgsmReturnToLogin::CBReconnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmReturnToLogin	*pThis	= (AgsmReturnToLogin *)	pClass;
	PVOID	*ppvBuffer		= (PVOID *)	pData;
	if (!ppvBuffer || !ppvBuffer[1] || !ppvBuffer[2])
		return FALSE;

	//if (!pThis->m_pcsAgsmLoginDB)
	//	return FALSE;

	CHAR	*pszAccountName	= (CHAR *)		ppvBuffer[0];
	INT32	lAuthKey		= *(INT32 *)	ppvBuffer[1];
	UINT32	ulNID			= *(UINT32 *)	ppvBuffer[2];


	AgsdAuthKey	*pcsAgsdAuthKey	= pThis->m_csAuthKeyPoolAdmin.GetAuthKey(pszAccountName);
	if (!pcsAgsdAuthKey)
		return FALSE;

	if (pcsAgsdAuthKey->GetAuthKey() != lAuthKey)
		return FALSE;

	pThis->m_csAuthKeyPoolAdmin.RemoveAuthKey(pszAccountName);

	if (!pThis->EnumCallback(AGSMRETURN_CB_SET_RECONNECT_CLIENT, &ulNID, pszAccountName))
	//if (!pThis->m_pcsAgsmLoginDB->SetReconnectClient(ulNID, pszAccountName))
	{
		return FALSE;
	}

	return pThis->SendPacketEndProcess(ulNID);
}

BOOL AgsmReturnToLogin::ReturnToLoginServer(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
	m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

	// timer event에 등록한다.
	if (!((AgsEngine *) GetModuleManager())->m_AsIOCPServer.m_csTimer.AddTimer(10000, pcsCharacter->m_lID, this, CBTimeoutNotifySaveAll, NULL))
		return FALSE;

	m_pcsAgsmCharacter->BackupCharacterData(pcsCharacter);
	m_pcsAgsmCharacter->EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pcsCharacter, NULL);

	m_pcsAgpmCharacter->SetReturnToLoginServer(pcsCharacter);

	if (!EnumCallback(AGSMRETURN_CB_NOTIFY_SAVE_ALL, pcsCharacter, NULL))
	{
		m_pcsAgpmCharacter->ResetReturnToLoginServer(pcsCharacter);
		return FALSE;
	}

	m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus	= AGSDCHAR_RETURN_STATUS_WAIT_SAVE;

	m_pcsAgpmCharacter->SetActionBlockTime(pcsCharacter, 11000, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

	return TRUE;
}

BOOL AgsmReturnToLogin::ReceiveNotifySaveAll(CHAR *pszName)
{
	if (!pszName || !pszName[0])
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(pszName);
	if (!pcsCharacter)
		return FALSE;

	if (m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus != AGSDCHAR_RETURN_STATUS_WAIT_SAVE)
		return TRUE;

	// timer event에 등록한다.
	if (!((AgsEngine *) GetModuleManager())->m_AsIOCPServer.m_csTimer.AddTimer(2000, pcsCharacter->m_lID, this, CBFinishSaveAll, NULL))
		return FALSE;

	m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus	= AGSDCHAR_RETURN_STATUS_FINISH_SAVE;

	m_pcsAgpmCharacter->SetActionBlockTime(pcsCharacter, 3000, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmReturnToLogin::CBTimeoutNotifySaveAll(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (lCID == 0 || !pClass)
		return FALSE;

	AgsmReturnToLogin	*pThis			= (AgsmReturnToLogin *)	pClass;

	AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return TRUE;

	if (pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus == AGSDCHAR_RETURN_STATUS_WAIT_SAVE)
	{
		pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus	= AGSDCHAR_RETURN_STATUS_NONE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "CBTimeoutNotifySaveAll() \n");
		AuLogFile_s("LOG\\ReturnToLoginErr.txt", strCharBuff);

		// client에 되돌아가기가 실패했다고 알려준다.
		pThis->SendPacketRequestFailed(pcsCharacter);

		pThis->m_pcsAgpmCharacter->ResetReturnToLoginServer(pcsCharacter);

		pThis->m_pcsAgpmCharacter->ClearActionBlockTime(pcsCharacter);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmReturnToLogin::CBFinishSaveAll(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (lCID == 0 || !pClass)
		return FALSE;

	AgsmReturnToLogin	*pThis			= (AgsmReturnToLogin *)	pClass;

	AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return TRUE;

	if (pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus == AGSDCHAR_RETURN_STATUS_FINISH_SAVE)
	{
		pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter)->m_eReturnStatus	= AGSDCHAR_RETURN_STATUS_NONE;

		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pcsCharacter, 6000000, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		// 여기서 이제 로그인 서버와의 통신을 시작하면 된다.
		pThis->SendPacketRequestKey(pcsCharacter);
	}
	else
	{
		// 뭔가 이상한 사항이다. 암튼,,, 블럭을 풀어주고 걍 게임서버에 냅둔다.
		pThis->m_pcsAgpmCharacter->ResetReturnToLoginServer(pcsCharacter);

		pThis->m_pcsAgpmCharacter->ClearActionBlockTime(pcsCharacter);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmReturnToLogin::SetCallbackNotifySaveAll(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRETURN_CB_NOTIFY_SAVE_ALL, pfCallback, pClass);
}

BOOL AgsmReturnToLogin::SetCallbackReconnectClient(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMRETURN_CB_SET_RECONNECT_CLIENT, pfCallback, pClass);
}

BOOL AgsmReturnToLogin::SendPacketRequestFailed(AgpdCharacter *pcsCharacter)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketRequestFailed(&nPacketLength);

	if (pvPacket && nPacketLength >= sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);

	return TRUE;
}

BOOL AgsmReturnToLogin::SendPacketRequestKey(AgpdCharacter *pcsCharacter)
{
	AgsdServer2* pagsdServer = m_pcsAgsmServerManager2->GetThisServer();
	if (!pagsdServer)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket = m_pcsAgpmReturnToLogin->MakePacketRequestKey(m_pcsAgsmCharacter->GetAccountID(pcsCharacter), pagsdServer->m_szWorld, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	INT16	nIndex	= 0;
	for( AgsdServer* pcsLoginServer = m_pcsAgsmCharacter->GetRecvCharServer(pcsCharacter);
		 pcsLoginServer;
		 pcsLoginServer = m_pcsAgsmServerManager2->GetLoginServers(&nIndex))
	{
		//pcsLoginServer = m_pcsAgsmServerManager2->GetServer("121.189.45.202:11002");

		if(!pcsLoginServer)
			return FALSE;

		if(!pcsLoginServer->m_bIsConnected)
			continue;

		SendPacketSvr(pvPacket, nPacketLength, pcsLoginServer->m_dpnidServer);
		
		return TRUE;
	}

	return TRUE;
}

BOOL AgsmReturnToLogin::SendPacketResponseKey(CHAR *pszAccountName, INT32 lAuthKey, UINT32 ulNID)
{
	if (!pszAccountName || !pszAccountName[0])
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketResponseKey(pszAccountName, lAuthKey, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_1);
}

BOOL AgsmReturnToLogin::SendPacketReceivedAuthKey(AgpdCharacter *pcsCharacter, INT32 lAuthKey)
{
	if (!pcsCharacter)
		return FALSE;

	INT16 nIndex = 0;
	for(AgsdServer2* pagsdServer = m_pcsAgsmCharacter->GetRecvCharServer(pcsCharacter);
		pagsdServer;
		pagsdServer = m_pcsAgsmServerManager2->GetLoginServers(&nIndex))
	{
		if(!pagsdServer)
			return FALSE;

		if(!pagsdServer->m_bIsConnected)
			continue;

		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketReceivedAuthKey(pcsCharacter->m_lID, lAuthKey, pagsdServer->m_szIP, &nPacketLength);
		//PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketReceivedAuthKey(pcsCharacter->m_lID, lAuthKey, "121.189.45.202:11002", &nPacketLength);

		if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
			return FALSE;

		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);

		DestroyClient(m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), DISCONNNECT_REASON_CLIENT_NORMAL);

		return TRUE;
	}

	return TRUE;
}

BOOL AgsmReturnToLogin::SendPacketEndProcess(UINT32 ulNID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmReturnToLogin->MakePacketEndProcess(&nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_1);
}