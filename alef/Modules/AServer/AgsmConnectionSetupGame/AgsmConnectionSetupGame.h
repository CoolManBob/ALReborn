/******************************************************************************
Module:  AgsmConnectionSetupGame.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 19
******************************************************************************/

#if !defined(__AGSMCONNECTIONSETUPGAME_H__)
#define __AGSMCONNECTIONSETUPGAME_H__

#include "ApBase.h"

#include "AuPacket.h"

#include "AsDefine.h"
#include "AgsEngine.h"

#include "AgsmAOIFilter.h"
#include "AgsmServerManager2.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmConnectionSetupGameD" )
#else
#pragma comment ( lib , "AgsmConnectionSetupGame" )
#endif

typedef enum _eAgsmConnectionSetupGameCallback
{
	AGSMCONNECTION_SETUPGAME_CB_CONNECT_COMPLETE = 0,
} eAgsmConnectionSetupGameCallback;


class AgsmConnectionSetupGame : public AgsModule {
private:
	AgsmAOIFilter			*m_pagsmAOIFilter;
	AgsmServerManager		*m_pagsmServerManager;

	BOOL					SendConnectInfo(AgsdServer *pcsServer);
	BOOL					SendSetServerFlag(AgsdServer *pcsServer);

public:
	AuPacket				m_csPacket;

public:
	AgsmConnectionSetupGame();
	~AgsmConnectionSetupGame();

	BOOL					OnAddModule();

	BOOL					OnInit();
	BOOL					OnDestroy();

	BOOL					ConnectAll();
	BOOL					ReconnectServer(INT32 lServerID);
	BOOL					ReconnectServer(AgsdServer *pcsServer);

	BOOL					ConnectGameServers();
	BOOL					ConnectLoginServers();
	BOOL					ConnectRelayServers();
	BOOL					DisconnectAll();

	BOOL					ConnectServer(AgsdServer *pcsServer);

	static BOOL				CheckConnect(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);
	static BOOL				CheckAccept(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);

	BOOL					IsAllGameServerReady();

	BOOL					OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

	static BOOL				CBReconnectServer(PVOID pData, PVOID pClass, PVOID pCustData);

	//BOOL					OnDisconnectSvr(INT32 lServerID);

	BOOL					SetCallbackConnectComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGSMCONNECTIONSETUPGAME_H__