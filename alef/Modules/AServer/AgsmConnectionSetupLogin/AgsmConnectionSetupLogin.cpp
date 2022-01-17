/******************************************************************************
Module:  AgsmConnectionSetupGame.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 19
******************************************************************************/

#include <stdio.h>
#include <time.h>

#include "AgsmConnectionSetupLogin.h"

AgsmConnectionSetupLogin::AgsmConnectionSetupLogin()
{
	SetModuleName("AgsmConnectionSetupLogin");

	SetPacketType(AGSMSERVER_PACKET_TYPE);

	// flag length = 16bits
	/*
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_CHAR,	12,		// server name
							AUTYPE_INT8,	1,		// server status
							AUTYPE_END,		0
							);
	*/
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_CHAR,	24,		// server ip
							AUTYPE_INT8,	1,		// server status
							AUTYPE_END,		0
							);
}

AgsmConnectionSetupLogin::~AgsmConnectionSetupLogin()
{
}

BOOL AgsmConnectionSetupLogin::OnAddModule()
{
	m_pagsmServerManager = (AgsmServerManager *) GetModule("AgsmServerManager2");

	if (!m_pagsmServerManager)
		return FALSE;

	return TRUE;
}

BOOL AgsmConnectionSetupLogin::OnInit()
{
	return TRUE;
}

BOOL AgsmConnectionSetupLogin::OnDestroy()
{
	return TRUE;
}

BOOL AgsmConnectionSetupLogin::SendConnectInfo(AgsdServer *pcsServer)
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
											pcsThisServer->m_szIP,
											&cServerStatus);
	/*
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szName,
											&cServerStatus);
	*/
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

BOOL AgsmConnectionSetupLogin::SendSetServerFlag(AgsdServer *pcsServer)
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
											pcsThisServer->m_szIP,
											&cServerStatus);
	/*
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szName,
											&cServerStatus);
	*/
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

BOOL AgsmConnectionSetupLogin::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
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

			// ##############################
			m_pagsmServerManager->UpdateServerStatus(pcsServer, (eSERVERSTATUS) cServerStatus, 0);

			SetIDToPlayerContext(pcsServer->m_lServerID, ulNID);

			// 접속 해온 서버로 (아마 게임서버일거다) 접속한다. (크로스링크 시킨다)
			//pcsServer->m_nidServer = ConnectServer(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
			pcsServer->m_nidServer = ConnectServer(pcsServer->m_szIP);
			if (pcsServer->m_nidServer >= 0)
			{
				pcsServer->m_bIsConnected = TRUE;

				//연결된 시간을 저장한다.
				pcsServer->m_lTime = time( NULL );

				SendConnectInfo(pcsServer);
				SendSetServerFlag(pcsServer);
			}
			else
			{
				CHAR	szBuffer[128];
				sprintf(szBuffer, "accepted connection request, but ConnectServer() failed.. server(%s)", pcsServer->m_szIP);
				WriteLog(AS_LOG_RELEASE, szBuffer);

				pcsServer->m_Mutex.Release();

				return FALSE;
			}

			CHAR	szBuffer[128];
			sprintf(szBuffer, "accept connection request from server(%s)", pcsServer->m_szIP);
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

BOOL AgsmConnectionSetupLogin::DisconnectAll()
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

BOOL AgsmConnectionSetupLogin::OnDisconnectSvr(INT32 lServerID)
{
	AgsdServer *pcsServer = m_pagsmServerManager->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	pcsServer->m_Mutex.WLock();

	pcsServer->m_bIsConnected	= FALSE;
	//pcsServer->m_bIsAuth		= FALSE;
	pcsServer->m_bIsAccepted	= FALSE;
	pcsServer->m_nidServer		= 0;
	pcsServer->m_dpnidServer	= 0;

	pcsServer->m_Mutex.Release();

	return TRUE;
}