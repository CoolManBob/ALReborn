// AsEchoServer.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 05. 07.
//
// 2004.05.10. - AsIOCPServer 를 이용해서 전면 수정함

// 2005.03.31. steeple
// 이놈은 4차 때부터 안쓰였고, 계속 안쓰이고 있음.

#ifndef __ASECHOSERVER__
#define __ASECHOSERVER__

#include "AsIOCPServer.h"
#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AsEchoServerD.lib")
#else
	#pragma comment(lib, "AsEchoServer.lib")
#endif
#endif

typedef BOOL (*AsEchoServerCallback)	(PVOID pData, PVOID pClass, PVOID pCustData);

#define ASECHOSERVER_MAX_THREAD		1
#define ASECHOSERVER_MAX_CONNECTION		100

#define ASECHOSERVER_PORT_LOGINSERVER	11500
#define ASECHOSERVER_PORT_GAMESERVER	11600
#define ASECHOSERVER_PORT_RELAYSERVER	11700

#define ASECHOSERVER_FILENAME_SLOW_IDLE_LOG		"slow_idle_"
#define ASECHOSERVER_FILENAME_SLOW_DISPATCH_LOG		"slow_dispatch_"

typedef enum _eAsEchoServerServerType
{
	ASECHOSERVER_SERVER_TYPE_LOGINSERVER = 0,
	ASECHOSERVER_SERVER_TYPE_GAMESERVER,
	ASECHOSERVER_SERVER_TYPE_RELAYSERVER,
} eAsEchoServerServerType;

typedef enum _eAsEchoServerPacketType
{
	ASECHOSERVER_PACKET_ECHO = 0x01,
	ASECHOSERVER_PACKET_SERVER_INFO = 0x02,
} eAsEchoServerPacketType;

class AsEchoServer
{
private:
	AsIOCPServer m_csIOCPServer;

	// Server Type
	eAsEchoServerServerType m_eServerType;
	INT16 m_nPort;
	
	// Server Info
	PVOID m_pGetUserCountClass;
	AsEchoServerCallback m_pfGetUserCountCB;	// User Count 를 구하는 함수

public:
	AuPacket m_csEchoPacket;
	AuPacket m_csServerInfoPacket;

public:
	AsEchoServer();
	virtual ~AsEchoServer();

	BOOL SetServerTypeLoginServer();
	BOOL SetServerTypeGameServer();
	BOOL SetServerTypeRelayServer();
	BOOL SetServerType(eAsEchoServerServerType eType, INT16 nPort);

	BOOL Start();
	BOOL Stop();

	static BOOL EchoThread(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);

	BOOL ParsePacket(PVOID pvPacket, INT16 nSize, INT32 lIndex);
	BOOL ParseEchoPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex);
	BOOL ParseServerInfoPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex);

	PVOID MakeEchoPacket(INT16* pnPacketSize, CHAR* szMessage, INT16 nMessageLength);
	PVOID MakeServerInfoPacket(INT16* pnPacketSize, INT32 lCurrentUserCount);

	BOOL SendEchoPacket(PVOID pvReceivedPacket, INT16 nReceivedPacketSize, INT32 lIndex);
	BOOL SendServerInfoPacket(INT32 lIndex);

	//////////////////////////////////////////////////////////////////////////
	// Log
	INT32 GetCurrentTimeStamp();
	INT32 GetCurrentTimeDate(CHAR* szTimeBuf);

	BOOL WriteSlowIdleLog(CHAR* szModuleName, INT32 lTick);
	BOOL WriteSlowDispatchLog(CHAR* szModuleName, INT32 lTick);

	//////////////////////////////////////////////////////////////////////////
	// Callback Registration Function
	BOOL SetCallbackGetUserCount(AsEchoServerCallback pfCallback, PVOID pClass);
	BOOL SetCallbackSlowIdle(AsEchoServerCallback pfCallback, PVOID pClass);
	BOOL SetCallbackSlowDispatch(AsEchoServerCallback pfCallback, PVOID pClass);

	//////////////////////////////////////////////////////////////////////////
	// Callback Function
	static BOOL CBSlowIdleLog(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSlowDispatchLog(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	// __ASECHOSERVER__