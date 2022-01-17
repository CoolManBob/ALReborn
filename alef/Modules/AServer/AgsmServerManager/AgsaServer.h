/******************************************************************************
Module:  AgsaServer.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 10
******************************************************************************/

#if !defined(__AGSASERVER_H__)
#define __AGSASERVER_H__

#include "ApBase.h"
#include "ApAdmin.h"

#include "AsDefine.h"

#include "AgsdServer.h"

const int AGSMSERVER_MAX_GAME_SERVER		= 15;
const int AGSMSERVER_MAX_LOGIN_SERVER		= 15;

class AgsaServerTemplate : public ApAdmin {
public:
	AgsaServerTemplate();
	~AgsaServerTemplate();

	AgsdServerTemplate*	AddServerTemplate(AgsdServerTemplate *pcsServerTemplate, INT32 lTID);
	AgsdServerTemplate* GetServerTemplate(INT32 lTID);
	BOOL				RemoveServerTemplate(INT32 lTID);
};

class AgsaServer : public ApAdmin {
private:
	AgsdServer*			m_pcsLoginServers[AGSMSERVER_MAX_LOGIN_SERVER];
	AgsdServer*			m_pcsDealServer;
	AgsdServer*			m_pcsGameServers[AGSMSERVER_MAX_GAME_SERVER];

	INT16				m_nNumServers;
	INT16				m_nNumGameServers;
	INT16				m_nNumLoginServers;

public:
	AgsaServer();
	~AgsaServer();

	AgsdServer*			AddServer(AgsdServer *pcsServer, INT32 lServerID, INT16 nServerType);
	AgsdServer*			GetServer(INT32 lServerID);
	AgsdServer*			GetServer(CHAR *szServerName);
	BOOL				RemoveServer(INT32 lServerID);

	AgsdServer*			GetLoginServers(INT16 *pnIndex);
	AgsdServer*			GetDealServer();
	AgsdServer*			GetGameServers(INT16 *pnIndex);
};

#endif //__AGSASERVER_H__
