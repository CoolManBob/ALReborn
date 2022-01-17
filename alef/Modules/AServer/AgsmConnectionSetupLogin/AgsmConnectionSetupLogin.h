/******************************************************************************
Module:  AgsmConnectionSetupLogin.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 19
******************************************************************************/

#if !defined(__AGSMCONNECTIONSETUPLOGIN_H__)
#define __AGSMCONNECTIONSETUPLOGIN_H__

#include "ApBase.h"

#include "AuPacket.h"

#include "AsDefine.h"
#include "AgsEngine.h"

#include "AgsmServerManager2.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmConnectionSetupLoginD" )
#else
#pragma comment ( lib , "AgsmConnectionSetupLogin" )
#endif


class AgsmConnectionSetupLogin : public AgsModule {
private:
	AgsmServerManager		*m_pagsmServerManager;

	BOOL					SendConnectInfo(AgsdServer *pcsServer);
	BOOL					SendSetServerFlag(AgsdServer *pcsServer);

public:
	AuPacket				m_csPacket;

public:
	AgsmConnectionSetupLogin();
	~AgsmConnectionSetupLogin();

	BOOL					OnAddModule();

	BOOL					OnInit();
	BOOL					OnDestroy();

	BOOL					DisconnectAll();

	BOOL					OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

	BOOL					OnDisconnectSvr(INT32 lServerID);
};

#endif	//__AGSMCONNECTIONSETUPLOGIN_H__