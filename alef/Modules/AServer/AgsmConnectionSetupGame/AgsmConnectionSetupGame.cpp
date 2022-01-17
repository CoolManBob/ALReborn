/******************************************************************************
Module:  AgsmConnectionSetupGame.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 19
******************************************************************************/

#include <stdio.h>

#include "AgsmConnectionSetupGame.h"

AgsmConnectionSetupGame::AgsmConnectionSetupGame()
{
	SetModuleName("AgsmConnectionSetupGame");

	EnableIdle2(TRUE);

	SetPacketType(AGSMSERVER_PACKET_TYPE);

	// flag length = 16bits
	m_csPacket.SetFlagLength(sizeof(INT8));

	// ##### modified by raska
  #ifdef _AGSM_SERVER_MANAGER2_
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_CHAR,	23,		// server ip
							AUTYPE_INT8,	1,		// server status
							AUTYPE_END,		0
							);
  #else
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_CHAR,	12,		// server name
							AUTYPE_INT8,	1,		// server status
							AUTYPE_END,		0
							);
  #endif
}

AgsmConnectionSetupGame::~AgsmConnectionSetupGame()
{
}

BOOL AgsmConnectionSetupGame::OnAddModule()
{
	m_pagsmAOIFilter		= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pagsmServerManager	= (AgsmServerManager *) GetModule("AgsmServerManager2");

	if (!m_pagsmAOIFilter ||
		!m_pagsmServerManager)
		return FALSE;

	if (!m_pagsmServerManager->SetCallbackReconnectServer(CBReconnectServer, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmConnectionSetupGame::OnInit()
{
	return TRUE;
}

BOOL AgsmConnectionSetupGame::OnDestroy()
{
	return TRUE;
}

BOOL AgsmConnectionSetupGame::ConnectServer(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	// ##### modified by raska
  #ifdef _AGSM_SERVER_MANAGER2_
	pcsServer->m_nidServer = AgsModule::ConnectServer(pcsServer->m_szIP);
  #else
	pcsServer->m_nidServer = AgsModule::ConnectServer(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
  #endif
	if (pcsServer->m_nidServer >= 0)
	{
		pcsServer->m_bIsConnected = TRUE;

		if (!SendConnectInfo(pcsServer))
		{
			DisconnectServer(pcsServer->m_nidServer);

			pcsServer->m_bIsConnected = FALSE;

			return FALSE;
		}

		SendSetServerFlag(pcsServer);
	}
	else
	{
		pcsServer->m_bIsConnected = FALSE;

		return FALSE;
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::ConnectAll()
{
	if (!ConnectGameServers())
		return FALSE;

	if (!ConnectRelayServers())
		return FALSE;

	return ConnectLoginServers();
}

BOOL AgsmConnectionSetupGame::ReconnectServer(INT32 lServerID)
{
	if (lServerID == 0)
		return FALSE;

	AgsdServer	*pcsServer		= m_pagsmServerManager->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	return ReconnectServer(pcsServer);
}

BOOL AgsmConnectionSetupGame::ReconnectServer(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	pcsServer->m_bIsAccepted	= FALSE;
	pcsServer->m_bIsAuth		= FALSE;
	pcsServer->m_bIsConnected	= FALSE;

	DisconnectServer(pcsServer->m_nidServer);
	DestroyClient(pcsServer->m_dpnidServer);

	if (AddIdleEvent2(GetClockCount() + 10000, pcsServer->m_lServerID, this, CheckAccept, NULL))
	{
		printf(" CheckConnect : AddIdleEvent() - 4 Success \n");
	}
	else
	{
		printf(" CheckConnect : AddIdleEvent() - 4 Failed \n");
	}

	if (!ConnectServer(pcsServer))
	{
		if (AddIdleEvent2(GetClockCount() + 10000, pcsServer->m_lServerID, this, CheckConnect, NULL))
		{
			printf(" ReconnectServer : AddIdleEvent() Success \n");
		}
		else
		{
			printf(" ReconnectServer : AddIdleEvent() Failed \n");
		}
	}
	else
	{
		if (IsAllGameServerReady())
			m_pagsmServerManager->UpdateServerStatus(m_pagsmServerManager->GetThisServer(), GF_SERVER_START);
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::ConnectGameServers()
{
	BOOL	bConnectAll = TRUE;

	AgsdServer	*pcsThisServer = m_pagsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	do {

		Sleep(3000);

		bConnectAll = TRUE;

		INT16 lIndex = 0;

		// 게임 서버들을 몽땅 가져와 연결한다.
		AgsdServer *pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
		while (pcsServer)
		{
			if (pcsThisServer->m_lServerID != pcsServer->m_lServerID && !pcsServer->m_bIsConnected)
			{
				bConnectAll &= ConnectServer(pcsServer);
			}

			pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
		}

	} while (!bConnectAll);

	// 접속이 다 되었으니 접속해온 넘들을 검사한다.

	BOOL	bAcceptAll = TRUE;
	do {

		Sleep(3000);

		bAcceptAll = TRUE;

		INT16 lIndex = 0;

		AgsdServer *pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
		while (pcsServer)
		{
			if (pcsThisServer->m_lServerID != pcsServer->m_lServerID && !pcsServer->m_bIsAccepted)
			{
				bAcceptAll = FALSE;

				break;
			}

			pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
		}

	} while (!bAcceptAll);

	SetServerStatus(GF_SERVER_COMPLETE_CONNECTION);

	return TRUE;
}

BOOL AgsmConnectionSetupGame::ConnectLoginServers()
{
	INT16 lIndex = 0;

	AgsdServer *pcsServer = m_pagsmServerManager->GetLoginServers(&lIndex);
	while (pcsServer)
	{
		if (!ConnectServer(pcsServer))
		{
			if (AddIdleEvent2(GetClockCount() + 10000, pcsServer->m_lServerID, this, CheckConnect, NULL))
			{
				printf(" ConnectLoginServers : AddIdleEvent() \n");
			}
			else
			{
				printf(" ConnectLoginServers : AddIdleEvent() Failed \n");
			}
		}

		pcsServer = m_pagsmServerManager->GetLoginServers(&lIndex);
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::ConnectRelayServers()
{
	AgsdServer *pcsServer = m_pagsmServerManager->GetRelayServer();
	if (!pcsServer)
		return FALSE;

	if (!ConnectServer(pcsServer))
	{
		AddIdleEvent2(GetClockCount() + 10000, pcsServer->m_lServerID, this, CheckConnect, NULL);
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::CheckConnect(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	AgsmConnectionSetupGame	*pThis = (AgsmConnectionSetupGame *) pClass;

	AgsdServer			*pcsServer = pThis->m_pagsmServerManager->GetServer(lCID);
	if (!pcsServer)
		return FALSE;

	printf(" CheckConnect() \n");

	pcsServer->m_Mutex.WLock();

	if (pcsServer->m_bIsConnected == FALSE)
	{
		printf(" CheckCount : connect try.... \n");

		pThis->ConnectServer(pcsServer);

		if (pcsServer->m_bIsConnected == FALSE)
		{
			printf(" Connect Failed. Retry \n");

			if (pThis->AddIdleEvent2(pThis->GetClockCount() + 10000, pcsServer->m_lServerID, pThis, CheckConnect, NULL))
			{
				printf(" CheckConnect : AddIdleEvent() Success \n");
			}
			else
			{
				printf(" CheckConnect : AddIdleEvent() Failed \n");
			}
		}
		else
		{
			printf(" Connect Success... OK \n");
		}
	}
	else
	{
		if (pcsServer->m_bIsAccepted == FALSE)
		{
		}
		else
			printf(" CheckCount : Server is already connected \n");
	}

	pcsServer->m_Mutex.Release();

	if (pThis->IsAllGameServerReady())
		pThis->m_pagsmServerManager->UpdateServerStatus(pThis->m_pagsmServerManager->GetThisServer(), GF_SERVER_START);

	return TRUE;
}

BOOL AgsmConnectionSetupGame::CheckAccept(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	AgsmConnectionSetupGame	*pThis = (AgsmConnectionSetupGame *) pClass;

	AgsdServer			*pcsServer = pThis->m_pagsmServerManager->GetServer(lCID);
	if (!pcsServer)
		return FALSE;

	printf(" CheckAccept() \n");

	pcsServer->m_Mutex.WLock();

	if (pcsServer->m_bIsAccepted == FALSE)
	{
		printf(" CheckCount : connect try.... \n");

		pThis->ReconnectServer(pcsServer);

		if (pcsServer->m_bIsConnected == TRUE)
		{
			pThis->AddIdleEvent2(pThis->GetClockCount() + 10000, pcsServer->m_lServerID, pThis, CheckAccept, NULL);
		}
	}

	pcsServer->m_Mutex.Release();

	if (pThis->IsAllGameServerReady())
		pThis->m_pagsmServerManager->UpdateServerStatus(pThis->m_pagsmServerManager->GetThisServer(), GF_SERVER_START);

	return TRUE;
}

BOOL AgsmConnectionSetupGame::SendConnectInfo(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pagsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_CONNECT;
	INT8	cServerStatus = (INT8) m_pagsmServerManager->GetServerStatus();

	// ##### modified by raska
  #ifdef _AGSM_SERVER_MANAGER2_
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											pcsThisServer->m_szIP,
											&cServerStatus);
  #else
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szName,
											&cServerStatus);
  #endif

	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, pcsServer->m_nidServer))
	{
		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmConnectionSetupGame::SendSetServerFlag(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pagsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_SET_SERVER_FLAG;
	INT8	cServerStatus = (INT8) m_pagsmServerManager->GetServerStatus();

	// ##### modified by raska
  #ifdef _AGSM_SERVER_MANAGER2_
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											pcsThisServer->m_szIP,
											&cServerStatus);
  #else
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szName,
											&cServerStatus);
  #endif

	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, pcsServer->m_nidServer))
	{
		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmConnectionSetupGame::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	if (ulType != AGSMSERVER_PACKET_TYPE || !pvPacket || !nSize)
		return FALSE;

	INT8		cOperation = (-1);
	CHAR*		szServerName = NULL;
	INT8		cServerStatus = (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&szServerName,
						&cServerStatus);

	switch (cOperation)	{
	case AGSMSERVER_PACKET_OPERATION_CONNECT:
		{
			if (!szServerName)
			{
				DestroyClient(ulNID);
				return FALSE;
			}

		  #ifdef _AGSM_SERVER_MANAGER2_
			AgsdServer	*pcsServer = m_pagsmServerManager->GetServer(szServerName);
		  #else
			AgsdServer	*pcsServer = m_pagsmServerManager->GetServerName(szServerName);
		  #endif
			if (!pcsServer)
			{
				DestroyClient(ulNID);
				return FALSE;
			}

			pcsServer->m_Mutex.WLock();

			pcsServer->m_bIsAccepted = TRUE;
			pcsServer->m_dpnidServer = ulNID;

			m_pagsmServerManager->UpdateServerStatus(pcsServer, (eSERVERSTATUS) cServerStatus);

			SetIDToPlayerContext(pcsServer->m_lServerID, ulNID);

			// ##### modified by raska, 2004.05.11
		  #ifdef _AGSM_SERVER_MANAGER2_
			if (pcsServer->m_cType == AGSMSERVER_TYPE_GAME_SERVER)
			{
				m_pagsmAOIFilter->AddPlayerToGroup(m_pagsmServerManager->GetGameServersDPNID(), pcsServer->m_dpnidServer);
			}
			else if (pcsServer->m_cType == AGSMSERVER_TYPE_DEAL_SERVER ||
				pcsServer->m_cType == AGSMSERVER_TYPE_RECRUIT_SERVER)
			{
				pcsServer->m_nidServer = AgsModule::ConnectServer(pcsServer->m_szIP);
				if (pcsServer->m_nidServer < 0)
				{
					pcsServer->m_Mutex.Release();

					return FALSE;
				}

				if (!SendConnectInfo(pcsServer))
				{
					DisconnectServer(pcsServer->m_nidServer);

					pcsServer->m_bIsConnected = FALSE;

					pcsServer->m_Mutex.Release();

					return FALSE;
				}
			}
			CHAR	szBuffer[128];
			sprintf(szBuffer, "accept connection request from server(%s)", pcsServer->m_szIP);
			WriteLog(AS_LOG_RELEASE, szBuffer);
		  #else
			if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType == AGSMSERVER_TYPE_GAME_SERVER)
			{
				m_pagsmAOIFilter->AddPlayerToGroup(m_pagsmServerManager->GetGameServersDPNID(), pcsServer->m_dpnidServer);
			}
			else if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType == AGSMSERVER_TYPE_DEAL_SERVER ||
				((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType == AGSMSERVER_TYPE_RECRUIT_SERVER)
			{
				pcsServer->m_nidServer = AgsModule::ConnectServer(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
				if (pcsServer->m_nidServer < 0)
				{
					pcsServer->m_Mutex.Release();

					return FALSE;
				}

				if (!SendConnectInfo(pcsServer))
				{
					DisconnectServer(pcsServer->m_nidServer);

					pcsServer->m_bIsConnected = FALSE;

					pcsServer->m_Mutex.Release();

					return FALSE;
				}
			}
			CHAR	szBuffer[128];
			sprintf(szBuffer, "accept connection request from server(%s)", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
			WriteLog(AS_LOG_RELEASE, szBuffer);
		  #endif

			pcsServer->m_Mutex.Release();
		}
		break;

	case AGSMSERVER_PACKET_OPERATION_SET_SERVER_FLAG:
		{
			if (!szServerName)
			{
				DestroyClient(ulNID);
				return FALSE;
			}
			// ##### modified by raska, 2004.05.11
		  #ifdef _AGSM_SERVER_MANAGER2_
			AgsdServer	*pcsServer = m_pagsmServerManager->GetServer(szServerName);
		  #else
			AgsdServer	*pcsServer = m_pagsmServerManager->GetServerName(szServerName);
		  #endif
			if (!pcsServer)
			{
				DestroyClient(ulNID);
				return FALSE;
			}

			pcsServer->m_Mutex.WLock();

			SetServerFlagToPlayerContext(ulNID);

			pcsServer->m_Mutex.Release();

			// 2004.07.06. steeple
			// Connection 이 정상적으로 종료 되면, 한번 불러준다.
			EnumCallback(AGSMCONNECTION_SETUPGAME_CB_CONNECT_COMPLETE, szServerName, NULL);
		}
		break;
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::DisconnectAll()
{
	INT16 lIndex = 0;

	AgsdServer *pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
	while (pcsServer)
	{
		if (pcsServer->m_bIsConnected)
			DisconnectServer(pcsServer->m_nidServer);

		if (pcsServer->m_bIsAccepted)
			DestroyClient(pcsServer->m_dpnidServer);

		pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
	}

	lIndex = 0;

	pcsServer = m_pagsmServerManager->GetLoginServers(&lIndex);
	while (pcsServer)
	{
		if (pcsServer->m_bIsConnected)
			DisconnectServer(pcsServer->m_nidServer);

		if (pcsServer->m_bIsAccepted)
			DestroyClient(pcsServer->m_dpnidServer);

		pcsServer = m_pagsmServerManager->GetLoginServers(&lIndex);
	}

	pcsServer = m_pagsmServerManager->GetRelayServer();
	if (pcsServer)
	{
		if (pcsServer->m_bIsConnected)
			DisconnectServer(pcsServer->m_nidServer);

		if (pcsServer->m_bIsAccepted)
			DestroyClient(pcsServer->m_dpnidServer);
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::IsAllGameServerReady()
{
	INT16	lIndex = 0;

	AgsdServer *pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
	while (pcsServer)
	{
		if (pcsServer->m_nStatus != GF_SERVER_START)
			return FALSE;

		pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
	}

	return TRUE;
}

BOOL AgsmConnectionSetupGame::CBReconnectServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmConnectionSetupGame		*pThis			= (AgsmConnectionSetupGame *)	pClass;
	AgsdServer					*pcsServer		= (AgsdServer *)				pData;

	return pThis->ReconnectServer(pcsServer);
}

BOOL AgsmConnectionSetupGame::SetCallbackConnectComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCONNECTION_SETUPGAME_CB_CONNECT_COMPLETE, pfCallback, pClass);
}