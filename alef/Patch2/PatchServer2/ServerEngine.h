/******************************************************************************
Module:  ServerEngine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 10
******************************************************************************/

#if !defined(__SERVERENGINE_H__)
#define __SERVERENGINE_H__

#include "AgsEngine.h"

#include "AgsmServerStatus.h"
#include "AgsmPatchServer2.h"

#define PATCHSERVER_PORT			11000


class ServerEngine : public AgsEngine {
public:
	HANDLE	m_hEventStop;

	AgsmPatchServer2 *	m_pcsAgsmPatchServer2;
	AgsmServerStatus *	m_pcsAgsmServerStatus;

	BOOL	OnRegisterModule();

public:
	ServerEngine(GUID guidApp);
	~ServerEngine();

	static BOOL Shutdown(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
};

#endif //__SERVERENGINE_H__