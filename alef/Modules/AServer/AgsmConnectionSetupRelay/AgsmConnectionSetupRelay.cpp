/******************************************************************************
Module:  AgsmConnectionSetupRelay.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 19
******************************************************************************/

#include <stdio.h>

#include "AgsmConnectionSetupRelay.h"

AgsmConnectionSetupRelay::AgsmConnectionSetupRelay()
{
	SetModuleName("AgsmConnectionSetupRelay");

	SetPacketType(AGSMSERVER_PACKET_TYPE);

	// flag length = 16bits
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_CHAR,	12,		// server name
							AUTYPE_INT8,	1,		// server status
							AUTYPE_END,		0
							);
}

AgsmConnectionSetupRelay::~AgsmConnectionSetupRelay()
{
}

BOOL AgsmConnectionSetupRelay::OnAddModule()
{
	m_pagsmServerManager = (AgsmServerManager *) GetModule("AgsmServerManager2");

	if (!m_pagsmServerManager)
		return FALSE;

	return TRUE;
}

BOOL AgsmConnectionSetupRelay::OnInit()
{
	return TRUE;
}

BOOL AgsmConnectionSetupRelay::OnDestroy()
{
	return TRUE;
}

/*
BOOL AgsmConnectionSetupRelay::ConnectGameServers()
{
	INT16 lIndex = 0;

	AgsdServer *pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
	while (pcsServer)
	{
		if (!ConnectServer(pcsServer))
		{
			AddIdleEvent(GetClockCount() + 10000, pcsServer->m_lServerID, this, CheckConnect, NULL);
		}

		pcsServer = m_pagsmServerManager->GetGameServers(&lIndex);
	}

	return TRUE;
}
*/

BOOL AgsmConnectionSetupRelay::SendConnectInfo(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pagsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_CONNECT;
	INT8	cServerStatus = (INT8) m_pagsmServerManager->GetServerStatus();

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szName,
											&cServerStatus);

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

BOOL AgsmConnectionSetupRelay::SendSetServerFlag(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer = m_pagsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_SET_SERVER_FLAG;
	INT8	cServerStatus = (INT8) m_pagsmServerManager->GetServerStatus();

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szName,
											&cServerStatus);

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

BOOL AgsmConnectionSetupRelay::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
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

			AgsdServer	*pcsServer = m_pagsmServerManager->GetServer(szServerName);
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

			// 접속 해온 서버로 (아마 게임서버일거다) 접속한다. (크로스링크 시킨다)
			pcsServer->m_nidServer = ConnectServer(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
			if (pcsServer->m_nidServer >= 0)
			{
				pcsServer->m_bIsConnected = TRUE;
				SendConnectInfo(pcsServer);
				SendSetServerFlag(pcsServer);
			}
			else
			{
				CHAR	szBuffer[128];
				sprintf(szBuffer, "accepted connection request, but ConnectServer() failed.. server(%s)", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
				WriteLog(AS_LOG_RELEASE, szBuffer);

				pcsServer->m_Mutex.Release();

				return FALSE;
			}

			CHAR	szBuffer[128];
			sprintf(szBuffer, "accept connection request from server(%s)", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
			WriteLog(AS_LOG_RELEASE, szBuffer);

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

			AgsdServer	*pcsServer = m_pagsmServerManager->GetServer(szServerName);
			if (!pcsServer)
			{
				DestroyClient(ulNID);
				return FALSE;
			}

			pcsServer->m_Mutex.WLock();

			SetServerFlagToPlayerContext(ulNID);

			pcsServer->m_Mutex.Release();
		}
		break;
	}

	return TRUE;
}

BOOL AgsmConnectionSetupRelay::DisconnectAll()
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

	return TRUE;
}

BOOL AgsmConnectionSetupRelay::OnDisconnectSvr(INT32 lServerID)
{
	AgsdServer *pcsServer = m_pagsmServerManager->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	pcsServer->m_Mutex.WLock();

	pcsServer->m_bIsConnected	= FALSE;
	pcsServer->m_bIsAuth		= FALSE;
	pcsServer->m_bIsAccepted	= FALSE;
	pcsServer->m_nidServer		= 0;
	pcsServer->m_dpnidServer	= 0;

	pcsServer->m_Mutex.Release();

	return TRUE;
}

/*
BOOL AgsmConnectionSetupRelay::CheckConnect(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	AgsmConnectionSetupRelay	*pThis = (AgsmConnectionSetupRelay *) pClass;

	AgsdServer			*pcsServer = pThis->m_pagsmServerManager->GetServer(lCID);
	if (!pcsServer)
		return FALSE;

	pcsServer->m_Mutex.WLock();

	if (pcsServer->m_bIsConnected == FALSE)
	{
		pThis->ConnectServer(pcsServer);

		if (pcsServer->m_bIsConnected == FALSE)
			pThis->AddIdleEvent(pThis->GetClockCount() + 10000, pcsServer->m_lServerID, pThis, CheckConnect, NULL);
	}

	pcsServer->m_Mutex.Release();

	if (pThis->IsAllGameServerReady())
		pThis->m_pagsmServerManager->UpdateServerStatus(pThis->m_pagsmServerManager->GetThisServer(), GF_SERVER_START);

	return TRUE;
}

BOOL AgsmConnectionSetupRelay::IsAllGameServerReady()
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
*/

/*
BOOL AgsmConnectionSetupGame::ConnectServer(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	pcsServer->m_nidServer = AgsModule::ConnectServer(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
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
*/