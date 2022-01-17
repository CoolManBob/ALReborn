/******************************************************************************
Module:  AgsmConnectionSetupRelay.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2004. 01. 26
******************************************************************************/

#if !defined(__AGSMCONNECTIONSETUPRELAY_H__)
#define __AGSMCONNECTIONSETUPRELAY_H__

#include "ApBase.h"

#include "AuPacket.h"

#include "AsDefine.h"
#include "AgsEngine.h"

#include "AgsmServerManager2.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmConnectionSetupRelayD" )
#else
#pragma comment ( lib , "AgsmConnectionSetupRelay" )
#endif


class AgsmConnectionSetupRelay : public AgsModule {
private:
	AgsmServerManager		*m_pagsmServerManager;

	BOOL					SendConnectInfo(AgsdServer *pcsServer);
	BOOL					SendSetServerFlag(AgsdServer *pcsServer);

	//BOOL					IsAllGameServerReady();

	//BOOL					ConnectServer(AgsdServer *pcsServer);

public:
	AuPacket				m_csPacket;

public:
	AgsmConnectionSetupRelay();
	~AgsmConnectionSetupRelay();

	BOOL					OnAddModule();

	BOOL					OnInit();
	BOOL					OnDestroy();

	//BOOL					ConnectGameServers();

	BOOL					DisconnectAll();

	BOOL					OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

	BOOL					OnDisconnectSvr(INT32 lServerID);

	//static BOOL				CheckConnect(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);
};

#endif	//__AGSMCONNECTIONSETUPRELAY_H__