/******************************************************************************
Module:  AsDP8Client.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 07. 19
******************************************************************************/

#include "AsDP8Client.h"

AsDP8Client::AsDP8Client()
{
	m_lSelfCID		= -1;
}

AsDP8Client::~AsDP8Client()
{
}

BOOL AsDP8Client::Initialize(INT32 lMaxInstance)
{
	return m_SocketManager.Initialize(lMaxInstance);
}

BOOL AsDP8Client::Stop()
{
	m_SocketManager.DisconnectAll();

    return TRUE;
}

BOOL AsDP8Client::IsConnected(INT32 nServerID)
{
	if (ASSERVER_STATE_ALIVE == m_SocketManager[nServerID]->GetState())
		return TRUE;
	else 
		return FALSE;
}

INT32 AsDP8Client::Open(LPCTSTR lpServerInfo, BOOL IsSendConnectInfo)
{
	INT32 lPort = 0;

    // pszServerInfo Format :  "xxx.xxx.xxx.xxx:port"
	char *strPort = (char*)strchr(lpServerInfo, ':');
	char strIP[23] = {0,};
    if( NULL != strPort )
    {
		// 포트 번호 추출
		lPort = atoi(strPort + 1);
		strncpy(strIP, lpServerInfo, strPort - lpServerInfo);
    } 
	else 
		return AP_INVALID_SERVERID;

	return Open(strIP, lPort, IsSendConnectInfo);
}

INT32 AsDP8Client::Open(LPCTSTR lpAddress, INT32 lPort, BOOL IsSendConnectInfo)
{
	if ((!lpAddress) || (0 == lPort))
		return AP_INVALID_SERVERID;

	for (INT32 i = AP_INVALID_SERVERID + 1; i<ASDP8CLIENT_MAX_DPCLIENT_INSTANCE; i++)
	{
		if (ASSERVER_STATE_INIT != m_SocketManager[i]->GetState()) continue;
		if (m_SocketManager[i]->m_bIsProcessReuse) continue;

		if (!m_SocketManager[i]->Connect(lpAddress, lPort))
		{
			printf("AsDP8Client::Open() Socket Index = [%d]\n",i);
			return AP_INVALID_SERVERID;
		}

		CHAR strBuff[512] = { 0, };
		sprintf_s(strBuff, sizeof(strBuff), "AsDP8Client::Open(%s:%d[%d]) - connected\n", lpAddress, lPort, i);
		OutputDebugString(strBuff);

		m_SocketManager[i]->m_bCallback	= TRUE;
		m_SocketManager[i]->SetState(ASSERVER_STATE_ALIVE);

		if (IsSendConnectInfo)
		{
			char Buffer[128];
			::ZeroMemory(Buffer, 128);
			((PPACKET_HEADER) Buffer)->bGuardByte		= APPACKET_FRONT_GUARD_BYTE;
			((PPACKET_HEADER) Buffer)->unPacketLength	= (UINT16)sizeof(PACKET_HEADER) + (UINT16)strlen(SERVER_STRING_ID) + 2;
			*((BYTE *) Buffer + ((PPACKET_HEADER) Buffer)->unPacketLength - 1)	= APPACKET_REAR_GUARD_BYTE;
			CopyMemory((CHAR *) Buffer + sizeof(PACKET_HEADER), SERVER_STRING_ID, strlen(SERVER_STRING_ID));
			m_SocketManager[i]->AsyncSend((CHAR*)Buffer, ((PPACKET_HEADER) Buffer)->unPacketLength, PACKET_PRIORITY_1);
		}

		return i;
	}

	return AP_INVALID_SERVERID;
}

BOOL AsDP8Client::Close(INT32 nServerID)
{
	if (nServerID >= ASDP8CLIENT_MAX_DPCLIENT_INSTANCE || nServerID <= AP_INVALID_SERVERID)
		return FALSE;

	if(!m_SocketManager[nServerID]->IsActive())
		return FALSE;

	CHAR strAddress[MAX_IP_ADDRESS_STRING+1] = { 0, };
	m_SocketManager[nServerID]->GetRemoteIPAddress(strAddress, sizeof(strAddress));

	CHAR strBuff[512] = { 0, };
	sprintf_s(strBuff, sizeof(strBuff), "AsDP8Client::Close(%s:%d[%d]) - disconnected\n", strAddress, /*lPort*/ 0, nServerID);
	OutputDebugString(strBuff);

	// 내가 끊은건 콜백 받지 말자....
	//m_SocketManager[nServerID]->m_bCallback = FALSE;
	m_SocketManager[nServerID]->Remove();

	return TRUE;
}

EnumAsyncSendResult AsDP8Client::Send(CHAR* pData, INT32 nDataSize, INT32 nServerID, PACKET_PRIORITY ePriority)
{
	if (nServerID >= ASDP8CLIENT_MAX_DPCLIENT_INSTANCE || nServerID <= AP_INVALID_SERVERID)
		return ASYNC_SEND_FAIL;

	if (SOCKET_TYPE_ARCHLORD == m_SocketManager[nServerID]->GetSocketType())
	{
		// server id setting
		((PPACKET_HEADER) pData)->lOwnerID	= m_lSelfCID;
	}

	return m_SocketManager[nServerID]->AsyncSend((CHAR*)pData, nDataSize, ePriority);
}

EnumAsyncSendResult AsDP8Client::SendRawData(CHAR* pData, INT32 nDataSize, INT32 nServerID)
{
	if (nServerID >= ASDP8CLIENT_MAX_DPCLIENT_INSTANCE || nServerID <= AP_INVALID_SERVERID)
		return ASYNC_SEND_FAIL;

	return m_SocketManager[nServerID]->AsyncSend((CHAR*)pData, nDataSize, PACKET_PRIORITY_NONE);
}

BOOL AsDP8Client::SetIDToPlayerContext(INT32 lID, INT32 lNID)
{
	if (AP_INVALID_SERVERID == lNID)
		return FALSE;

	PlayerData	*pPlayerData;

	pPlayerData = GetPlayerDataForID(lNID);
	if (!pPlayerData)
	{
//		WriteLog(AS_LOG_DEBUG, "AsDP9 : SetIDToPlayerContext() 에서 GetPlayerDataForID() 실패. NULL 리턴");
		return FALSE;
	}

	pPlayerData->lID = lID;

	return TRUE;
}

PlayerData* AsDP8Client::GetPlayerDataForID(INT32 lNID)
{
	if (AP_INVALID_SERVERID == lNID) return NULL;

	return m_SocketManager[lNID]->GetPlayerData();
}

BOOL AsDP8Client::SetServerFlagToPlayerContext(INT32 lNID)
{
	if (lNID == AP_INVALID_SERVERID)
		return FALSE;

	PlayerData	*pPlayerData	= GetPlayerDataForID(lNID);
	if (!pPlayerData)
	{
//		WriteLog(AS_LOG_DEBUG, "AsDP9 : SetServerFlagToPlayerContext() 에서 GetPlayerDataForID() 실패. NULL 리턴");
		return FALSE;
	}

	pPlayerData->bServer	= TRUE;

	return TRUE;
}