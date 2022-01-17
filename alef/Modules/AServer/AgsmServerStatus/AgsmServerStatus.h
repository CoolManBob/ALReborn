// AgsmServerStatus.h
// (C) NHN - ArchLord Development Team
// steeple, 2005. 03. 31.
//
// 서버 상태를 보내주는 서버를 작성

#ifndef __AGSMSERVERSTATUS_H__
#define __AGSMSERVERSTATUS_H__

#include "ApServerMonitor.h"
#include "ApServerMonitorPacket.h"

#include "AgsmServerStatus_IOCPServer.h"
#include "AgsmServerManager2.h"
#include "AgsmAdmin.h"
#include "AgpmConfig.h"
#include "AgsmConfig.h"
#include "AgsmInterServerLink.h"
#include "AgsmDatabasePool.h"
#include "AgpmSiegeWar.h"

#include "WISMApiDll.h"
#pragma comment(lib, "WISMApiWRS64.lib")




#define ASSM_MAX_THREAD				1
#define ASSM_MAX_CONNECTION			5

class AgsmServerStatus : public AgsModule
{
private:
	AgsmServerStatus_IOCPServer m_csIOCPServer;

	AgsmServerManager* m_pagsmServerManager;
	AgsmAdmin* m_pagsmAdmin;
	AgsmAccountManager* m_pagsmAccountManager;

	AgpmConfig	*m_pagpmConfig;
	AgsmConfig	*m_pagsmConfig;

	AgsmInterServerLink *	m_pAgsmInterServerLink;
	AgsmCharacter		*	m_pAgsmCharacter;
	
	AgsmDatabasePool	*	m_pAgsmDatabasePool;

	AgpmSiegeWar		*	m_pAgpmSiegeWar;

	// Server Type
	INT32 m_lServerType;
	INT16 m_nPort;

	INT32 m_lConnectedSocketIndex;
	INT32 m_lLastSendClockCount;
	INT32 m_lLastSendServerMonitorInfo;

	CHAR m_szProcessFileLastModifiedTime[ASSM_MAX_TIME_STRING+1];

	INT32 m_lLiveTick;

	INT32	m_lPatchVersion;

	WISM::CWISMNotification m_WISMObj;

public:
	PVOID m_pServerMainClass;
	AsTimerCallBack m_pfServerShutdown;
	
public:
	ApServerMonitorPacket m_csPacket;

public:
	AgsmServerStatus();
	virtual ~AgsmServerStatus();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	INT32 GetConnectedSocketIndex() {return m_lConnectedSocketIndex;}

	BOOL SetServerTypeLoginServer();
	BOOL SetServerTypeGameServer();
	BOOL SetServerTypeRelayServer();
	BOOL SetServerTypePatchServer();
	BOOL SetServerType(INT32 lServerType, INT16 nPort);

	BOOL Start();
	BOOL Stop();

	static BOOL DispatchThread(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
	static BOOL DisconnectCB(PVOID pData, PVOID pvParam, PVOID pvSocket);

	BOOL ParsePacket(PVOID pvPacket, INT16 nSize, INT32 lIndex);
	BOOL ParseServerInfoPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex);
	BOOL ParseServerControlPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex);

	BOOL ServerControlStop(INT32 lSocketIndex);

	PVOID MakeServerInfoPacket(INT16* pnPacketSize);
	PVOID MakeServerInfoPacketForGameServer(INT16* pnPacketLength);
	PVOID MakeServerInfoPacketForRelayServer(INT16* pnPacketLength);
	PVOID MakeServerInfoPacketForLoginServer(INT16* pnPacketLength);
	PVOID MakeServerInfoPacketForPatchServer(INT16* pnPacketLength);

	BOOL MakeAndSendGameInfoPacket(INT32 lIndex, CHAR* _KillCharacterName, CHAR* _KillRace, CHAR* _KilledCharacter, CHAR* _KilledRace);

	BOOL SendServerInfoPacket(INT32 lIndex = -1);
	BOOL SendServerControlPacket(INT32 lIndex = -1);
	BOOL SendGeneralParamStringPacket(INT32 lIndex, char *pszConsoleIP, char *pszString);

	INT32 GetServerStatusForSM();

	//JK_WISM연동
	INT32 GetServerSystemID();


	VOID SetPatchVersion(INT32 lPatchVersion)
	{
		m_lPatchVersion = lPatchVersion;
	}

	static BOOL CBEnumNotice(PVOID pvData, PVOID pvClass, PVOID pvCustData);
};

#endif // __AGSMSERVERSTATUS_H__
