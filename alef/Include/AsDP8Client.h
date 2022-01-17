/******************************************************************************
Module:  AsDP8Client.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 07. 19
******************************************************************************/

#if !defined(__ASDP8CLIENT_H__)
#define __ASDP8CLIENT_H__

#include "ApBase.h"
#include "AsIOCPServer.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AsDP8ClientD" )
#else
#pragma comment ( lib , "AsDP8Client" )
#endif
#endif

#define ASDP8CLIENT_MAX_DPCLIENT_INSTANCE	300
#define SERVER_STRING_ID "THIS IS SERVER"

///////////////////////////////////////////////////////////////////////////////
class AsDP8Client {
public:
	AsServerSocketManager	m_SocketManager;
	INT32					m_lSelfCID;		// server id

public:
	AsDP8Client();
	~AsDP8Client();

	BOOL	Initialize(INT32 lMaxInstance = ASDP8CLIENT_MAX_DPCLIENT_INSTANCE);
	BOOL	Stop();

	INT32	Open(LPCTSTR lpServerInfo /* IPAddress : Port */, BOOL IsSendConnectInfo);
	INT32	Open(LPCTSTR lpAddress, INT32 lPort, BOOL IsSendConnectInfo);
	BOOL	Close(INT32 nServerID);

	BOOL	IsConnected(INT32 nServerID);
	EnumAsyncSendResult	Send(CHAR* pData, INT32 nDataSize, INT32 nServerID, PACKET_PRIORITY ePriority);
	EnumAsyncSendResult	SendRawData(CHAR* pData, INT32 nDataSize, INT32 nServerID);

	PlayerData*	GetPlayerDataForID(INT32 lNID);
	BOOL	SetIDToPlayerContext(INT32 lID, INT32 lNID);
	BOOL	SetServerFlagToPlayerContext(INT32 lNID);
};

#endif //__ASDP8CLIENT_H__