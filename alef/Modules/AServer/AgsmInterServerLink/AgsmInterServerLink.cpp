/*=============================================================

	AgsmInterServerLink.cpp

=============================================================*/

#include <stdio.h>
#include "AgsmInterServerLink.h"

#include <process.h>

//#define AGSMISL_TRACE	TRACE
#define AGSMISL_TRACE	TRACE

/************************************************************/
/*		The Implementation of AgsmInterServerLink class		*/
/************************************************************/
//
AgsmInterServerLink::AgsmInterServerLink()
	{
	m_pAgsmServerManager = NULL;
	m_pAgsmAOIFilter = NULL;
	
	m_ulPreviousClockReconnect	= 0;
	m_ulPreviousClockNotify		= 0;

	SetModuleName("AgsmInterServerLink");
	SetModuleType(APMODULE_TYPE_SERVER);
	//EnableIdle2(TRUE);		// for notify and idle event

	SetPacketType(AGSMSERVER_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,	1,		// operation
							AUTYPE_CHAR,	23,		// server ip
							AUTYPE_INT8,	1,		// server status
							AUTYPE_INT16,	1,		// No. of players
							AUTYPE_END,		0
							);

	m_lReplyCount	= 0;

	m_bDisconnectLoginServer	= FALSE;
	m_bDestroyThread	= FALSE;

	}

AgsmInterServerLink::~AgsmInterServerLink()
	{
	}


//	ApModule Inherited
//=============================================
//
BOOL AgsmInterServerLink::OnAddModule()
	{
	m_pAgsmServerManager = (AgsmServerManager *) GetModule("AgsmServerManager2");
	m_pAgsmAOIFilter = (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	if (!m_pAgsmServerManager || !m_pAgsmAOIFilter)
		return FALSE;

	return TRUE;
	}

BOOL AgsmInterServerLink::OnInit()
	{
	m_bDestroyThread	= FALSE;

	m_stSendThread.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsmInterServerLink::ConnectServerThread, 
													this, 0, &m_stSendThread.ulThreadID);

	if ((!m_stSendThread.hThread) || (!m_stSendThread.ulThreadID)) 
	{
		// Thread 생성 실패
		DebugBreak();
		return FALSE;
	}

	SetThreadName(m_stSendThread.ulThreadID, "ConnectServerThread");

	return TRUE;
	}

BOOL AgsmInterServerLink::OnDestroy()
	{
	m_bDestroyThread	= TRUE;

	::WaitForSingleObject(m_stSendThread.hThread, INFINITE);

	return TRUE;
	}

BOOL AgsmInterServerLink::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize,
									UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (AGSMSERVER_PACKET_TYPE  != ulType || !pvPacket || !nSize)
		return FALSE;

	INT8		cOperation = (-1);
	CHAR*		pszServerIP = NULL;
	INT8		cServerStatus = (-1);
	INT16		nPlayers = 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pszServerIP,
						&cServerStatus,
						&nPlayers);

	if (!pszServerIP)
		{
		AGSMISL_TRACE("!!!Error : pszServerIP of NID[%d] is NULL in OnReceive()\n"
					  "Destroying client...\n", ulNID);
		DestroyClient(ulNID);
		return FALSE;
		}

	AgsdServer	*pcsServer = m_pAgsmServerManager->GetServer(pszServerIP);
	if (!pcsServer)
		{
		AGSMISL_TRACE("!!!Error : Cannot find server of IP[%s], NID(%d) is NULL in OnReceive()\n"
					  "Destroying client...\n", pszServerIP, ulNID);
		DestroyClient(ulNID);
		return FALSE;
		}

	switch (cOperation)
		{
		case AGSMSERVER_PACKET_OPERATION_CONNECT:
			_OnConnect(pcsServer, ulNID);
			break;

		case AGSMSERVER_PACKET_OPERATION_NOTIFY_STATUS:
			_OnNotify(pcsServer, cServerStatus, nPlayers);
			break;

		case AGSMSERVER_PACKET_OPERATION_REPLY_CONNECT:
			pcsServer->m_bIsReplyConnect	= TRUE;
			break;

		default:
			break;
		}

	return TRUE;
	}

BOOL AgsmInterServerLink::OnDisconnectSvr(INT32 lServerID, UINT32 ulNID)
	{
	AgsdServer *pcsServer = m_pAgsmServerManager->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	if (pcsServer->m_dpnidServer != ulNID && pcsServer->m_dpnidServer != 0)
		return TRUE;

	_OnDisconnect(pcsServer, 0);

	return TRUE;
	}

BOOL AgsmInterServerLink::OnIdle2(UINT32 ulClockCount)
	{
	/*
	// connnect disconnected servers
	_IdleReconnect(ulClockCount);

	//_IdleNotify(ulClockCount);
	
	if (m_ulPreviousClockNotify + AGSMINTERSERVERLINK_TERM_NOTIFY < ulClockCount)
	{
		if (AGSMSERVER_TYPE_GAME_SERVER == m_pAgsmServerManager->GetThisServerType())
		{
			INT16		lIndex		= 0;
			AgsdServer	*pcsServer	= m_pAgsmServerManager->GetLoginServers(&lIndex);
			while (pcsServer)
			{
				_Notify(pcsServer);

				pcsServer	= m_pAgsmServerManager->GetLoginServers(&lIndex);
			}

			AgsdServer	*pcsRelayServer	= m_pAgsmServerManager->GetRelayServer();
			if (pcsRelayServer)
			{
				_RequestReply(pcsRelayServer);
			}
		}

		m_ulPreviousClockNotify	= ulClockCount;
	}
	*/

	return TRUE;
	}

UINT WINAPI AgsmInterServerLink::ConnectServerThread(PVOID pvParam)
{
	if (!pvParam)
		return FALSE;

	AgsmInterServerLink	*pThis	= (AgsmInterServerLink *)	pvParam;

	while (TRUE)
	{
		UINT32 ulClockCount	= pThis->GetClockCount();

		// connnect disconnected servers
		pThis->_IdleReconnect(ulClockCount);

		//_IdleNotify(ulClockCount);
		
		if (pThis->m_ulPreviousClockNotify + AGSMINTERSERVERLINK_TERM_NOTIFY < ulClockCount)
		{
			if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType())
			{
				INT16		lIndex		= 0;
				AgsdServer	*pcsServer	= pThis->m_pAgsmServerManager->GetLoginServers(&lIndex);
				while (pcsServer)
				{
					pThis->_Notify(pcsServer);

					pcsServer	= pThis->m_pAgsmServerManager->GetLoginServers(&lIndex);
				}

				AgsdServer	*pcsRelayServer	= pThis->m_pAgsmServerManager->GetRelayServer();
				if (pcsRelayServer)
				{
					pThis->_RequestReply(pcsRelayServer);
				}
			}

			pThis->m_ulPreviousClockNotify	= ulClockCount;
		}

		// 2008.03.12. steeple
		// 로그인 서버 끊는 걸 한 쓰레드에서 처리하기 위해 이렇게 함.
		// 기존에는 ServerStatus 쓰레드에서 처리하다가 데드락 걸림.
		// 여기서는 끊기만 하면 된다. 다음 틱 때 리커넥드 됨.
		if (pThis->m_bDisconnectLoginServer)
		{
			pThis->DisconnectLoginServers();
			pThis->SetDisconnectLoginServer(FALSE);
		}

		if (pThis->m_bDestroyThread)
			break;

		Sleep(5000);
	}

	return FALSE;
}


//	Connect/Disconnect method
//===============================================
//
BOOL AgsmInterServerLink::ConnectAll(BOOL bExceptRelay)
	{
	ASSERT((AGSMSERVER_TYPE_GAME_SERVER == m_pAgsmServerManager->GetThisServerType()) &&
		   "게임서버도 아닌데 왜 이걸써! 두글라고 --+");

	BOOL bResult = ConnectGameServers();
	if (!bExceptRelay)
		bResult &= ConnectRelayServer();
	bResult &= ConnectLoginServers();

	return bResult;
	}

BOOL AgsmInterServerLink::DisconnectAll()
	{
	AgsdServer	*pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	AGSMISL_TRACE("Disconnecting all servers...\n");

	INT16 lIndex = 0;
	AgsdServer *pcsServer = NULL;

	// if game server, disconnect all connected login, relay servers
	if (AGSMSERVER_TYPE_GAME_SERVER == pcsThisServer->m_cType)
		{
		while (NULL != (pcsServer = m_pAgsmServerManager->GetLoginServers(&lIndex)))
			{
			_Disconnect(pcsServer);
			}

		pcsServer = m_pAgsmServerManager->GetRelayServer();
		if (pcsServer)
			_Disconnect(pcsServer);
		}

	// disconnect all connected game servers
	while (NULL != (pcsServer = m_pAgsmServerManager->GetGameServersInGroup(&lIndex)))
		{
		_Disconnect(pcsServer);
		}

	return TRUE;
	}

BOOL AgsmInterServerLink::DisconnectLoginServers(BOOL bPushQueue)
{
	AGSMISL_TRACE("Disconnecting login servers...\n");
	
	INT16 lIndex = 0;
	AgsdServer *pcsServer = NULL;
	while (NULL != (pcsServer = m_pAgsmServerManager->GetLoginServers(&lIndex)))
	{
		if (pcsServer->m_bIsConnected)
		{
			_Disconnect(pcsServer);
		}

		if(bPushQueue)
		{
			_PushDisconnected(pcsServer);
		}
	}

	return TRUE;
}

// 2008.03.12. steeple
BOOL AgsmInterServerLink::SetDisconnectLoginServer(BOOL bSet)
{
	AuAutoLock csLock(m_DisconnectedServersCS);
	m_bDisconnectLoginServer = bSet;
	return TRUE;
}


//	Connect servers per type
//=========================================
//
BOOL AgsmInterServerLink::ConnectGameServers()
	{
	ASSERT(NULL != m_pAgsmServerManager);
	AGSMISL_TRACE("Connection game servers...\n");

	AgsdServer	*pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16 lIndex = 0;
	AgsdServer *pcsServer = NULL;
	while (NULL != (pcsServer = m_pAgsmServerManager->GetGameServersInGroup(&lIndex)))
		{
		if (pcsThisServer->m_lServerID != pcsServer->m_lServerID && !pcsServer->m_bIsConnected)
			{
			_ConnectEx(pcsServer);
			}
		}

	return TRUE;
	}

BOOL AgsmInterServerLink::ConnectLoginServers(BOOL bForceConnect)
	{
	AGSMISL_TRACE("Connecting login servers...\n");
	
	INT16 lIndex = 0;
	AgsdServer *pcsServer = NULL;
	while (NULL != (pcsServer = m_pAgsmServerManager->GetLoginServers(&lIndex)))
		{
		if (bForceConnect && pcsServer->m_bIsConnected)
			_Disconnect(pcsServer);

		_ConnectEx(pcsServer);
		}

	return TRUE;
	}

BOOL AgsmInterServerLink::ConnectRelayServer(BOOL bForceConnect)
	{
	AGSMISL_TRACE("Connecting relay servers...\n");

	AgsdServer *pcsServer = m_pAgsmServerManager->GetRelayServer();
	if (!pcsServer)
		return FALSE;

	if (bForceConnect && pcsServer->m_bIsConnected)
		_Disconnect(pcsServer);

	BOOL bResult = _ConnectEx(pcsServer);

	/*
	AgsdServer *pcsServer2 = NULL;
	INT32 lIndex = 0;
	CHAR* pszDatabase = m_pAgsmServerManager->GetThisServer()->m_szDatabase;

	while (NULL != (pcsServer2 = m_pAgsmServerManager->GetDealServers(&lIndex, pszDatabase)))
		{
		if (0 == pcsServer2->m_dpnidServer || pcsServer == pcsServer2)
			continue;

		if (bForceConnect && pcsServer2->m_bIsConnected)
			_Disconnect(pcsServer2);

		bResult = _ConnectEx(pcsServer2);
		}
	*/

	return bResult;
	}


//	Internal
//=============================================================
//
BOOL AgsmInterServerLink::_Connect(AgsdServer *pcsServer)
	{
	ASSERT(NULL != pcsServer);
	
	AuAutoLock Lock(pcsServer->m_Mutex);
	if (!Lock.Result()) return FALSE;

	CHAR* szIPdest = pcsServer->m_szPrivateIP;

	AGSMISL_TRACE("Connecting server[%s]...\n", szIPdest);

	if (pcsServer->m_bIsConnected)
		{
		AGSMISL_TRACE("Already connected. server[%s]\n", szIPdest);
		//return TRUE;
		}

	pcsServer->m_bIsReplyConnect	= FALSE;

	pcsServer->m_dpnidServer = AgsModule::ConnectServer(szIPdest);
	if (pcsServer->m_dpnidServer > 0)
		{
		if (_SendConnectInfo(pcsServer))
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
			AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

			pcsServer->m_bIsConnected = TRUE;
			//pcsServer->m_bIsAccepted = TRUE;
			// for OnDisconnectSvr()
			SetIDToPlayerContext(pcsServer->m_lServerID, pcsServer->m_dpnidServer);

			AddTimer(2000, pcsServer->m_lServerID, this, ConnectCheckCallback, NULL);

			//EnumCallback(AGSMINTERSERVERLINK_CB_CONNECT, pcsServer, NULL);
			return TRUE;
		}
		else
			DisconnectServer(pcsServer->m_dpnidServer);
		}

	pcsServer->m_bIsConnected = FALSE;
	pcsServer->m_dpnidServer = 0;

	return FALSE;
	}

BOOL AgsmInterServerLink::_ConnectEx(AgsdServer *pcsServer)
	{
	if (!_Connect(pcsServer))
		{
		_PushDisconnected(pcsServer);
		return FALSE;
		}
	
	if (AGSMSERVER_TYPE_LOGIN_SERVER == pcsServer->m_cType)
		_PushNotify(pcsServer);

	return TRUE;
	}

void AgsmInterServerLink::_Disconnect(AgsdServer *pcsServer)
	{
	ASSERT(NULL !=pcsServer);

	AuAutoLock Lock(pcsServer->m_Mutex);
	if (!Lock.Result()) return;

	printf("Disconnecting server[%s]...\n", pcsServer->m_szPrivateIP);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
	AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

	// 2007.07.30. steeple
	// Callback 에 넣어주기 위해서
	UINT32 ulNID = pcsServer->m_dpnidServer;

	DisconnectServer(pcsServer->m_dpnidServer);
	pcsServer->m_bIsConnected = FALSE;
	//DestroyClient(pcsServer->m_dpnidServer);
	pcsServer->m_dpnidServer = 0;
	pcsServer->m_lTime = 0;

	EnumCallback(AGSMINTERSERVERLINK_CB_DISCONNECT, pcsServer, &ulNID);
	}

BOOL AgsmInterServerLink::_Notify(AgsdServer *pcsServer)
	{
	if (NULL == pcsServer)
		return FALSE;
				
	AgsdServer *pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if (NULL == pcsThisServer)
		return FALSE;

	//INT32	lCurrentUser	= m_pAgsmCharacter.GetNumOfPlayers();
	INT32 lCurrentUser = 0;
	EnumCallback(AGSMINTERSERVERLINK_CB_GET_CURRENT_USER, &lCurrentUser, NULL);

	// get no. of players
	pcsThisServer->m_Mutex.WLock();	
	pcsThisServer->m_nNumPlayers = (INT16)lCurrentUser;	//m_pAgsmAOIFilter->GetPlayerCountFromAllPlayerGroup();
	pcsThisServer->m_Mutex.Release();

	if(_SendStatusInfo(pcsServer))
		printf("Notifying status to server[%s]...\n", pcsServer->m_szPrivateIP);

	return TRUE;
	}

void AgsmInterServerLink::_RequestReply(AgsdServer *pcsServer)
	{
	if (NULL == pcsServer)
		return;

	if (m_ReplyCS.WLock())
		{
		if (m_lReplyCount > AGSMINTERSERVERLINK_MAX_RETRY)
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
			AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);
			// Relay 맛갔다. 겜서버 죽인다.
			EnumCallback(AGSMINTERSERVERLINK_CB_REPLY_FAIL, pcsServer, NULL);

			return;
			}

		AGSMISL_TRACE("Request reply to relay server[%s]...\n", pcsServer->m_szPrivateIP);

		/*
		BOOL	bSendResult	= _SendRequestReply(pcsServer);
		if (bSendResult)
			m_lReplyCount++;
		*/

		m_ReplyCS.Release();
		}

	return;
	}

void AgsmInterServerLink::_ResponseReply(AgsdServer *pcsServer)
	{
	if (NULL == pcsServer)
		return;

	AGSMISL_TRACE("Response reply from relay server[%s]...\n", pcsServer->m_szPrivateIP);

	if (m_ReplyCS.WLock())
		{
		m_lReplyCount--;

		m_ReplyCS.Release();
		}

	return;
	}

void AgsmInterServerLink::_OnConnect(AgsdServer *pcsServer,  UINT32 ulNID)
	{
	ASSERT(NULL !=pcsServer);
	//AGSMISL_TRACE("OnConnect server[%s]...\n", pcsServer->m_szPrivateIP);

	AuAutoLock Lock(pcsServer->m_Mutex);
	if (!Lock.Result()) return;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - connected %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
	AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

	if (!pcsServer->m_bIsConnected)
	{
		pcsServer->m_dpnidServer = ulNID;
		pcsServer->m_dpnidServer = ulNID;
		pcsServer->m_bIsConnected = TRUE;
		pcsServer->m_lTime = time(NULL);
	}
	else
	{
		//AGSMISL_TRACE("Already connected. server[%s]\n", pcsServer->m_szPrivateIP);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - Already connected %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
		AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);
	}

	SetIDToPlayerContext(pcsServer->m_lServerID, ulNID);
	SetServerFlagToPlayerContext(ulNID);
	_SendReplyConnect(pcsServer);
	EnumCallback(AGSMINTERSERVERLINK_CB_CONNECT, pcsServer, &ulNID);
	}

void AgsmInterServerLink::_OnDisconnect(AgsdServer *pcsServer, UINT32 ulNID)
	{
	ASSERT(NULL !=pcsServer);
	AGSMISL_TRACE("OnDisconnect server[%s]...\n", pcsServer->m_szPrivateIP);

	_Disconnect(pcsServer);

	if (AGSMSERVER_TYPE_GAME_SERVER != m_pAgsmServerManager->GetThisServerType())
		return;

	// LK, 빌링 서버는 지가 알아서 거시기한다.
	if (AGSMSERVER_TYPE_LK_SERVER == pcsServer->m_cType ||
		AGSMSERVER_TYPE_BILLING_SERVER == pcsServer->m_cType)
		return;

	_PushDisconnected(pcsServer);
	}

void AgsmInterServerLink::_OnNotify(AgsdServer *pcsServer, INT8 nStatus, INT16 nNumPlayers)
	{
	ASSERT(NULL !=pcsServer);
	AGSMISL_TRACE("OnNotify server[%s]...\n", pcsServer->m_szPrivateIP);

	m_pAgsmServerManager->UpdateServerStatus(pcsServer, (eSERVERSTATUS)nStatus, nNumPlayers);
	EnumCallback(AGSMINTERSERVERLINK_CB_NOTIFY, pcsServer, NULL);
	}


//	OnIdle operation
//=========================================
//
// #####
// 처음시작시 m_ulPreviousClock값이 0이고 ulClockCount는 timeGetTime으로 얻어진
// 큰값이라 한번은 실행되게 되어있다.
// 모듈 초기화시 ulClockCount값을 받아서 설정할 수 있도록 되거나
// m_ulPreviousClock값이 0일때를 비교해 무시(매번 비교해서 우울하다)하도록 해야한다
// 지금은 어차피 큐에 들어가있는넘이 없으므로 그냥 넘어가니 냅둔다.
//
void AgsmInterServerLink::_IdleReconnect(UINT32 ulClockCount)
	{
	if (m_ulPreviousClockReconnect + AGSMINTERSERVERLINK_TERM_RECONNECT > ulClockCount)
		return;

	AgsdServer *pcsServer = _PopDisconnected();
	if (pcsServer)
		_ConnectEx(pcsServer);
	m_ulPreviousClockReconnect = ulClockCount;
	}

void AgsmInterServerLink::_IdleNotify(UINT32 ulClockCount)
	{
	if (AGSMSERVER_TYPE_GAME_SERVER != m_pAgsmServerManager->GetThisServerType())
		return;
			
	if (m_ulPreviousClockNotify + AGSMINTERSERVERLINK_TERM_NOTIFY > ulClockCount)
		return;

	// ##### 나중에 고쳐라.
	AgsdServer *pcsServer = _PopNotify();
	if (pcsServer)
		{
		_Notify(pcsServer);
		_PushNotify(pcsServer);
		}
	m_ulPreviousClockNotify = ulClockCount;
	}


//	Queue
//=========================================
//
void AgsmInterServerLink::_PushDisconnected(AgsdServer *pcsServer)
	{
	AuAutoLock Lock(m_DisconnectedServersCS);
	if (!Lock.Result()) return;
	m_DisconnectedServers.push_back(pcsServer);
	}

AgsdServer* AgsmInterServerLink::_PopDisconnected()
	{
	AuAutoLock Lock(m_DisconnectedServersCS);
	if (!Lock.Result()) return NULL;

	AgsdServer *pcsServer = NULL;
	if (m_DisconnectedServers.size() > 0)
		{
		pcsServer = m_DisconnectedServers.front();
		m_DisconnectedServers.pop_front();
		}
	return pcsServer;
	}

void AgsmInterServerLink::_PushNotify(AgsdServer *pcsServer)
	{
	AuAutoLock Lock(m_NotifyCS);
	if (!Lock.Result()) return;
	m_Notify.push_back(pcsServer);
	}

AgsdServer* AgsmInterServerLink::_PopNotify()
	{
	AuAutoLock Lock(m_NotifyCS);
	if (!Lock.Result()) return NULL;

	AgsdServer *pcsServer = NULL;
	if (m_Notify.size() > 0)
		{
		pcsServer = m_Notify.front();
		m_Notify.pop_front();
		}
	return pcsServer;
	}


//	Packet send
//=========================================
//
BOOL AgsmInterServerLink::_SendConnectInfo(AgsdServer *pcsServer)
	{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_CONNECT;
	INT8	cServerStatus = (INT8) m_pAgsmServerManager->GetServerStatus();
	INT16	nPlayers = 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE,
											 &cOperation,
											 pcsThisServer->m_szIP,
											 &cServerStatus,
											 &nPlayers);

	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, pcsServer->m_dpnidServer))
		{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
		}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
	}

BOOL AgsmInterServerLink::_SendStatusInfo(AgsdServer *pcsServer)
	{
	if (!pcsServer || !pcsServer->m_bIsConnected)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_NOTIFY_STATUS;
	INT8	cServerStatus = (INT8) pcsThisServer->m_nStatus;
	INT16	nPlayers = pcsThisServer->m_nNumPlayers;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE,
											 &cOperation,
											 pcsThisServer->m_szIP,
											 &cServerStatus,
											 &nPlayers);
	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, pcsServer->m_dpnidServer))
		{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
		}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
	}

BOOL AgsmInterServerLink::_SendReplyConnect(AgsdServer *pcsServer)
	{
	if (!pcsServer || !pcsServer->m_bIsConnected)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_REPLY_CONNECT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE,
											 &cOperation,
											 pcsThisServer->m_szIP,
											 NULL,
											 NULL);
	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, pcsServer->m_dpnidServer))
		{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
		}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
	}

//	Callback setting
//==========================================
//
BOOL AgsmInterServerLink::SetCallbackConnect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMINTERSERVERLINK_CB_CONNECT, pfCallback, pClass);
	}

BOOL AgsmInterServerLink::SetCallbackNotify(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMINTERSERVERLINK_CB_NOTIFY, pfCallback, pClass);
	}

BOOL AgsmInterServerLink::SetCallbackDisconnect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMINTERSERVERLINK_CB_DISCONNECT, pfCallback, pClass);
	}

BOOL AgsmInterServerLink::SetCallbackReplyFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMINTERSERVERLINK_CB_REPLY_FAIL, pfCallback, pClass);
	}

BOOL AgsmInterServerLink::SetCallbackGetCurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMINTERSERVERLINK_CB_GET_CURRENT_USER, pfCallback, pClass);
	}

BOOL AgsmInterServerLink::ConnectCheckCallback(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	AgsmInterServerLink	*pThis	= (AgsmInterServerLink *)	pClass;

	AgsdServer	*pcsServer	= pThis->m_pAgsmServerManager->GetServer(lCID);
	if (pcsServer)
	{
		if (pcsServer->m_bIsConnected)
		{
			if(pcsServer->m_bIsReplyConnect)
			{
				pThis->EnumCallback(AGSMINTERSERVERLINK_CB_CONNECT, pcsServer, NULL);
			}
			else
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
				AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

				pThis->_Disconnect(pcsServer);
				pThis->_ConnectEx(pcsServer);
			}
		}			
	}

	return TRUE;
}
