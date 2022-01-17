#include "AgsmGKforPCRoom.h"

AgsmGK::AgsmGK()
{
	SetModuleName("AgsmGK");

	//EnableIdle2(TRUE);

	m_pcsAgsmCharacter			= NULL;
	m_pcsAgsmServerManager2		= NULL;

	m_pcsAgsdLKServer			= NULL;
}

AgsmGK::~AgsmGK()
{
}

BOOL AgsmGK::OnAddModule()
{
	m_pcsAgsmCharacter			= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmServerManager2		= (AgsmServerManager2 *)	GetModule("AgsmServerManager2");

	if (!m_pcsAgsmCharacter ||
		!m_pcsAgsmServerManager2)
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackEnterGameworld(CBEnterGameWorld, this))
		return FALSE;
	if (!m_pcsAgsmCharacter->SetCallbackExitGameWorld(CBExitGameWorld, this))
		return FALSE;

	if (!m_pcsAgsmServerManager2->SetCallbackAddServer(CBAddServerInfo, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmGK::OnInit()
{
	m_csPacketMemoryPool.Initialize(1024, 20, _T("AgsmGK_PacketMemory"));

	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetCBLKServer(this, DispatchGK, DisconnectGK);
}

BOOL AgsmGK::OnDestroy()
{
	return TRUE;
}

BOOL AgsmGK::DispatchGK(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	if (!pvPacket || !pvParam || !pvSocket)
		return FALSE;

	AgsmGK			*pThis			= (AgsmGK *)			pvParam;
	AsServerSocket	*pSocket		= (AsServerSocket *)	pvSocket;

	return pThis->OnReceive(pvPacket, pSocket->GetIndex());
}

BOOL AgsmGK::DisconnectGK(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	if (!pvParam || !pvSocket)
		return FALSE;

	AgsmGK			*pThis			= (AgsmGK *)			pvParam;
	AsServerSocket	*pSocket		= (AsServerSocket *)	pvSocket;

	AgsmGKReturn	eReturn	= pThis->ConnectLKServer();
	if (eReturn == AGSMGK_RETURN_CONNECT_SUCCESS ||
		eReturn == AGSMGK_RETURN_NEED_WAIT)
		return TRUE;

	return FALSE;
}

BOOL AgsmGK::OnReceive(PVOID pvPacket, UINT32 ulNID)
{
	if (!pvPacket || ulNID == 0)
		return FALSE;

	switch (::ntohs(((PLK_PACKET_HEADER) pvPacket)->unHeaderMsg))
	{
	case GK_PACKET_COMMAND_AUTH:
		{
			// ((PGK_PACKET_AUTH) pvPacket)
		}
		break;

	case GK_PACKET_COMMAND_AUTH_RESULT:
		{
			PVOID	pvBuffer[2];
			pvBuffer[0]	= UintToPtr(::ntohs(((PGK_PACKET_AUTH_RESULT) pvPacket)->unResult));
			pvBuffer[1] = UintToPtr(ulNID);

			EnumCallback(AGSMGK_RECEIVE_AUTH_RESULT,
						 ((PGK_PACKET_AUTH_RESULT) pvPacket)->szID,
						 pvBuffer);
		}
		break;

	case GK_PACKET_COMMAND_LOGOUT:
		{
			// ((PGK_PACKET_LOGOUT) pvPacket)
		}
		break;

	case GK_PACKET_COMMAND_LOGOUT_RESULT:
		{
			// ((PGK_PACKET_LOGOUT_RESULT) pvPacket)
		}
		break;

	case GK_PACKET_COMMAND_BILL_AUTH:
		{
			// ((PGK_PACKET_BILL_AUTH) pvPacket)
		}
		break;

	case GK_PACKET_COMMAND_BILL_AUTH_RESULT:
		{
			PVOID	pvBuffer[2];
			pvBuffer[0]	= UintToPtr(::ntohs(((PGK_PACKET_BILL_AUTH_RESULT) pvPacket)->unResult));
			pvBuffer[1] = UintToPtr(ulNID);

			EnumCallback(AGSMGK_RECEIVE_BILL_AUTH_RESULT,
						 ((PGK_PACKET_BILL_AUTH_RESULT) pvPacket)->szID,
						 pvBuffer);
		}
		break;

	case GK_PACKET_COMMAND_WARN_NOTICE:
		{
			// ((PGK_PACKET_WARN_NOTICE) pvPacket)
		}
		break;

	case GK_PACKET_COMMAND_CUT_IP:
		{
			// ((PGK_PACKET_CUT_IP) pvPacket)
		}
		break;

	case GK_PACKET_COMMAND_PING_AUTH:
		{
			PGK_PACKET_KEEP_ALIVE packet	= (PGK_PACKET_KEEP_ALIVE) m_csPacketMemoryPool.Alloc();
			if (!packet)
				break;

			CopyMemory(packet, pvPacket, sizeof(GK_PACKET_KEEP_ALIVE));

			return SendPacket(packet, sizeof(GK_PACKET_KEEP_ALIVE), ulNID);
			break;
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL AgsmGK::SendPacketAuth(CHAR *pszAccountID, UINT32 ulNID)
{
	if (!pszAccountID || strlen(pszAccountID) > 34 || ulNID == 0)
		return FALSE;

	CHAR	*pszIPAddr	= GetPlayerIPAddress(ulNID);

	if (!pszIPAddr || strlen(pszIPAddr) > 16)
		return FALSE;

	PGK_PACKET_AUTH	packet	= (PGK_PACKET_AUTH)	m_csPacketMemoryPool.Alloc();
	if (!packet)
		return FALSE;

	ZeroMemory(packet, sizeof(GK_PACKET_AUTH));

	packet->stHeader.unLen			= ::htons(sizeof(GK_PACKET_AUTH) - sizeof(LK_PACKET_HEADER));
	packet->stHeader.unHeaderMsg	= ::htons(GK_PACKET_COMMAND_AUTH);
	//packet->stHeader.ulSeqID		= ;

	strcpy(packet->szIP, pszIPAddr);
	strcpy(packet->szID, pszAccountID);

	BOOL	bSendResult	= SendPacket(packet, sizeof(GK_PACKET_AUTH), m_pcsAgsdLKServer->m_dpnidServer);

	m_csPacketMemoryPool.Free(packet);

	return	bSendResult;
}

BOOL AgsmGK::SendPacketLogout(CHAR *pszAccountID, UINT32 ulNID)
{
	if (!pszAccountID || strlen(pszAccountID) > 34 || ulNID == 0)
		return FALSE;

	CHAR	*pszIPAddr	= GetPlayerIPAddress(ulNID);

	if (!pszIPAddr || strlen(pszIPAddr) > 16)
		return FALSE;

	PGK_PACKET_LOGOUT	packet	= (PGK_PACKET_LOGOUT)	m_csPacketMemoryPool.Alloc();
	if (!packet)
		return FALSE;

	ZeroMemory(packet, sizeof(GK_PACKET_LOGOUT));

	packet->stHeader.unLen			= ::htons(sizeof(GK_PACKET_LOGOUT) - sizeof(LK_PACKET_HEADER));
	packet->stHeader.unHeaderMsg	= ::htons(GK_PACKET_COMMAND_LOGOUT);
	//packet->stHeader.ulSeqID		= ;

	strcpy(packet->szIP, pszIPAddr);
	strcpy(packet->szID, pszAccountID);

	SYSTEMTIME	stCurrent;
	::GetLocalTime(&stCurrent);

	sprintf(packet->szOuttime, "%4d%2d%2d%2d%2d%4d", stCurrent.wYear,
													 stCurrent.wMonth,
													 stCurrent.wDay,
													 stCurrent.wHour,
													 stCurrent.wMinute,
													 stCurrent.wSecond);

	BOOL	bSendResult	= SendPacket(packet, sizeof(GK_PACKET_LOGOUT), m_pcsAgsdLKServer->m_dpnidServer);

	m_csPacketMemoryPool.Free(packet);

	return	bSendResult;
}

BOOL AgsmGK::SendPacketBillAuth(CHAR *pszAccountID, UINT32 ulNID)
{
	if (!pszAccountID || strlen(pszAccountID) > 34 || ulNID == 0)
		return FALSE;

	CHAR	*pszIPAddr	= GetPlayerIPAddress(ulNID);

	if (!pszIPAddr || strlen(pszIPAddr) > 16)
		return FALSE;

	PGK_PACKET_BILL_AUTH	packet	= (PGK_PACKET_BILL_AUTH)	m_csPacketMemoryPool.Alloc();
	if (!packet)
		return FALSE;

	ZeroMemory(packet, sizeof(GK_PACKET_BILL_AUTH));

	packet->stHeader.unLen			= ::htons(sizeof(GK_PACKET_BILL_AUTH) - sizeof(LK_PACKET_HEADER));
	packet->stHeader.unHeaderMsg	= ::htons(GK_PACKET_COMMAND_BILL_AUTH);
	//packet->stHeader.ulSeqID		= ;

	strcpy(packet->szIP, pszIPAddr);
	strcpy(packet->szID, pszAccountID);

	BOOL	bSendResult	= SendPacket(packet, sizeof(GK_PACKET_BILL_AUTH), m_pcsAgsdLKServer->m_dpnidServer);

	m_csPacketMemoryPool.Free(packet);

	return	bSendResult;
}

BOOL AgsmGK::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmGK			*pThis			= (AgsmGK *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (!pThis->GetLKServer())
		return TRUE;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	return pThis->SendPacketAuth(pcsAgsdCharacter->m_szAccountID, pcsAgsdCharacter->m_dpnidCharacter);
}

BOOL AgsmGK::CBExitGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmGK			*pThis			= (AgsmGK *)		pClass;

	if (!pThis->GetLKServer())
		return TRUE;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	return pThis->SendPacketLogout(pcsAgsdCharacter->m_szAccountID, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmGK::CBAddServerInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmGK			*pThis			= (AgsmGK *)		pClass;
	AgsdServer		*pcsServer		= (AgsdServer *)	pData;

	if (pcsServer->m_cType == AGSMSERVER_TYPE_LK_SERVER)
	{
		pThis->m_pcsAgsdLKServer	= pcsServer;
	}

	return TRUE;
}

AgsmGKReturn AgsmGK::ConnectLKServer()
{
	if (!m_pcsAgsdLKServer)
		return AGSMGK_RETURN_NOT_EXIST_LKSERVER;

	m_pcsAgsdLKServer->m_dpnidServer = AgsModule::ConnectServer(m_pcsAgsdLKServer->m_szIP, FALSE);
	if (m_pcsAgsdLKServer->m_dpnidServer > 0)
	{
		m_pcsAgsdLKServer->m_bIsConnected = TRUE;
		SetIDToPlayerContext(m_pcsAgsdLKServer->m_lServerID, m_pcsAgsdLKServer->m_dpnidServer);

		SetLKServerSocket(m_pcsAgsdLKServer->m_dpnidServer);

		return AGSMGK_RETURN_CONNECT_SUCCESS;
	}
	else
	{
		m_pcsAgsdLKServer->m_bIsConnected = FALSE;

		if (!AgsModule::AddTimer(5000, m_pcsAgsdLKServer->m_lServerID, this, CheckConnectLKServer, m_pcsAgsdLKServer))
			return AGSMGK_RETURN_CONNECT_FAIL;
	}
					
	return AGSMGK_RETURN_NEED_WAIT;
}

BOOL AgsmGK::WaitForConnectServer()
{
	if (!m_pcsAgsdLKServer)
		return FALSE;

	while (!m_pcsAgsdLKServer->m_bIsConnected)
	{
		Sleep(1000);
	}

	return TRUE;
}

BOOL AgsmGK::CheckConnectLKServer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass)
		return FALSE;

	AgsmGK			*pThis			= (AgsmGK *)		pClass;

	if (!pThis->m_pcsAgsdLKServer)
		return FALSE;

	if (pThis->m_pcsAgsdLKServer->m_bIsConnected == FALSE)
	{
		AgsmGKReturn	eReturn	= pThis->ConnectLKServer();
		if (eReturn == AGSMGK_RETURN_CONNECT_SUCCESS ||
			eReturn == AGSMGK_RETURN_NEED_WAIT)
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmGK::SetCallbackReceiveAuthResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGK_RECEIVE_AUTH_RESULT, pfCallback, pClass);
}

BOOL AgsmGK::SetCallbackReceiveBillAuthResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGK_RECEIVE_BILL_AUTH_RESULT, pfCallback, pClass);
}