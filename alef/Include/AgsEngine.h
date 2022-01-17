/******************************************************************************
Module:  AgsEngine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 06
******************************************************************************/

#if !defined(__AGSENGINE_H__)
#define __AGSENGINE_H__

#include "ApBase.h"
#include "ApModuleManager.h"

#include "AsDefine.h"
#include "AsCommonLib.h"
#include "ApIOCPDispatcher.h"
//#include "AsDP9.h"
#include "AsObject.h"
#include "AsIOCPServer.h"
#include "AsDP8Client.h"
#include "AgsGameProcess.h"

#pragma warning( disable : 4786 )

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsEngineD" )
#else
#pragma comment ( lib , "AgsEngine" )
#endif
#endif

#define RELAY_ERROR_LOG		_T("LOG\\RelayError.log")

typedef enum
{
	AGS_DB_VENDER_ORACLE = 0,
	AGS_DB_VENDER_MSSQL,
} AgsDatabaseVender;

extern AgsDatabaseVender g_eAgsDatabaseVender;

typedef struct _stPacketStatistics {
	INT64		ullPacketSize;								// ÃÑ ÆÐÅ¶ »çÀÌÁî
	UINT32		ulPacketCount;								// ÃÑ ÆÐÅ¶ °¹¼ö

	UINT32		ulAveragePacketSize;

	UINT32		ulCurrentThroughputBPS;
	UINT32		ulTotalThroughputBPS;
	UINT32		ulPeakThroughputBPS;

	UINT32		ulLastProcessClockCount;
	INT64		ullLastProcessPacketSize;
	UINT32		ulLastPacketCount;
} stPacketStatistics, *pstPacketStatistics;


class AgsModule : public ApModule {
public:
	AgsModule();
	~AgsModule();

	virtual BOOL OnIdle5Times(UINT32 ulClockCount) { return TRUE; };
	virtual BOOL OnIdle50Times(UINT32 ulClockCount) { return TRUE; };

	//BOOL AddModule(PVOID pModuleManager, AsCMDQueue* pqueueResult, AsCMDQueue* pqueueResultSvr);

	INT16 GetServerStatus();
	BOOL SetServerStatus(INT16 nServerStatus);

	DWORD GetDPNID(PVOID pszData, INT16 nDataSize);

	BOOL SendPacketSvr(PVOID pvPacket, INT16 nLength, INT16 nServerNID);

	//BOOL Send(DPNID dpnid, PVOID pvCharacter, PVOID pszData, INT16 nDataSize, 
	//	UINT16 unType = AGSNP_CMDTYPE_SEND_CLIENT);
	//BOOL SendServer(DPNID dpnid, PVOID pvServer, PVOID pszData, INT16 nDataSize, 
	//	UINT16 unType = AGSNP_CMDTYPE_SEND_SERVER);

	virtual BOOL OnDisconnect(INT32 lCID, UINT32 ulNID) { return TRUE; };
	virtual BOOL OnDisconnect(CHAR *szAccountName, UINT32 ulNID) { return TRUE; };
	virtual BOOL OnDisconnectSvr(INT32 lServerID, UINT32 ulNID) { return TRUE; };

	BOOL GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnNumAddr);

	PlayerData* GetPlayerDataForID(INT32 lNID);
	BOOL SetIDToPlayerContext(INT32 lID, UINT32 dpnid);
	BOOL SetAccountNameToPlayerContext(CHAR *szAccountName, UINT32 dpnid);
	CHAR* GetPlayerIPAddress(UINT32 lNID);

	BOOL SetServerFlagToPlayerContext(UINT32 dpnid);

	BOOL SetCheckValidation(UINT32 dpnid);
	BOOL ResetCheckValidation(UINT32 dpnid);

	BOOL GetConnectionInfo(UINT32 dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS);

	UINT32 CreateGroup();
	BOOL DestroyGroup(UINT32 ulGroupID);
	BOOL AddPlayerToGroup(UINT32 ulGroupID, UINT32 ulAddNID);
	BOOL RemovePlayerFromGroup(UINT32 ulGroupID, UINT32 ulRemoveNID);

	//UINT32 GetClockCount();

	INT16 ConnectServer(CHAR *szAddress, BOOL IsSendConnectInfo = TRUE);
	BOOL DisconnectServer(INT16 nServerNID);

	BOOL DestroyClient(INT32 lNID, INT32 lDestroyReason = DISCONNNECT_REASON_CLIENT_OUT_OF_DATE);

	// packet monitoring functions
	/////////////////////////////////////////////////////////////////////////
	BOOL	ResetPacketStatistics();

	pstPacketStatistics	GetRecvPacketStatistic(INT32 lPacketType);
	pstPacketStatistics	GetSendPacketStatistic(INT32 lPacketType);

	UINT32	GetSendTotalThroughputBPS();
	UINT32	GetRecvTotalThroughputBPS();

	BOOL	ActiveSendBuffer(UINT32 ulNID);
	BOOL	DisableSendBuffer(UINT32 ulNID);

	BOOL	IsPCRoom(UINT32 ulNID);

	AsTimerEventData* AddTimer(UINT32 uDelay, INT32 lCID, PVOID pClass, AsTimerCallBack fpTimerCallBack, PVOID pvData);

	BOOL	SetLKServerSocket(UINT32 ulNID);
	BOOL	SetBillingSocket(UINT32 ulNID);
	BOOL	SetPatchSocket(UINT32 ulNID);

	BOOL SendPacketUser(PACKET_HEADER& pPacket, UINT32 ulNID);

	BOOL SendPacketAllUser(PVOID pvPacket, INT16 nLength, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketAllUser(PACKET_HEADER& pPacket, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
#ifdef __PROFILE__
	void SetProfileDelay(DWORD dwDelay);
#endif
};

class AgsEngine : public ApModuleManager 
{
	static AgsEngine* m_pInstance;

public:
	INT16			m_nServerStatus;
	AsSystemCommand m_SystemCommand;

	AsIOCPServer	m_AsIOCPServer;
	AsDP8Client		m_DPClientModule;

	INT32			m_lGameThreadCount;
	INT32			m_lConnectionCount;
	INT32			m_lMaxSendBufferMB;
	INT32			m_lPort;

	// game, noti process manager (thread id)
	///////////////////////////////////////////////////////////////////////////////
//	stTHREADOBJECT		m_stIdleThread;
	stTHREADOBJECT		m_stProtectIdleThread;
	stTHREADOBJECT		m_stProtectIdleThread2;
	stTHREADOBJECT		m_stProtectIdleThread3;
	AgsGameProcess		*m_pcsGameProcess;

	INT16				m_nmsecClockInterval;
	HANDLE				m_hStartProcessCompleteEvent;

	BOOL StartServer(INT32 lPort, INT32 lThreadCount, INT32 lMaxConnectionCount, char *pstrIPBlockingFileName = NULL, char *pstrPCRoomIPFileName = NULL, INT32 lMaxSendBufferMB = 10 );
	BOOL StartProcess();
	BOOL StartIdleThread();
	BOOL StartIdleThread2();
	BOOL StartIdleThread3();
	BOOL StartGameThread();

	BOOL Stop();
	BOOL StopProcess();

public:
	AgsEngine();
	virtual ~AgsEngine();

	static AgsEngine* GetInstance()
	{ 
		return m_pInstance; 
	};

	INT16 GetServerStatus();
	BOOL SetServerStatus(INT16 nServerStatus);

	BOOL StopEngine();
	BOOL WaitStopEngine();

	BOOL SendPacket(PVOID pvPacket, INT16 nLength, UINT32 ulNID = 0, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacket( PACKET_HEADER& pPacket, UINT32 ulNID = 0, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketSvr(PVOID pvPacket, INT16 nLength, INT16 nServerNID);
	BOOL SendPacketAllUser(PVOID pvPacket, INT16 nLength, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketAllUser(PACKET_HEADER& pPacket, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	virtual BOOL OnRegisterModule() { return TRUE; };
	virtual BOOL OnTerminate() { return TRUE; };

	BOOL Disconnect(INT32 lCID, UINT32 ulNID);
	BOOL Disconnect(CHAR *szAccountName, UINT32 ulNID);
	BOOL DisconnectSvr(INT32 lServerID, UINT32 ulNID);

	BOOL GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnNumAddr);

	PlayerData* GetPlayerDataForID(INT32 lNID);
	BOOL SetIDToPlayerContext(INT32 lID, UINT32 dpnid);
	BOOL SetAccountNameToPlayerContext(CHAR *szAccountName, UINT32 dpnid);

	BOOL SetServerFlagToPlayerContext(UINT32 dpnid);

	BOOL SetCheckValidation(UINT32 dpnid);
	BOOL ResetCheckValidation(UINT32 dpnid);

	BOOL GetConnectionInfo(UINT32 dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS);

	UINT32 CreateGroup();
	BOOL DestroyGroup(UINT32 ulGroupID);
	BOOL AddPlayerToGroup(UINT32 ulGroupID, UINT32 ulAddNID);
	BOOL RemovePlayerFromGroup(UINT32 ulGroupID, UINT32 ulRemoveNID);

	INT16 ConnectServer(CHAR *szAddress, BOOL IsSendConnectInfo = TRUE);
	BOOL DisconnectServer(INT16 nServerNID);

	BOOL SetSelfCID(INT32 lCID);

	BOOL DestroyClient(INT32 lNID, INT32 lDestroyReason = DISCONNNECT_REASON_CLIENT_OUT_OF_DATE);

	BOOL SetCBLoginDisconnect(PVOID pClass, AsDispatchCallBack fpDisconnect);
	BOOL SetCBLKServer(PVOID pClass, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);
	BOOL SetCBBilling(PVOID pClass, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);
	BOOL SetCBPatch(PVOID pClass, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);

	BOOL SetNumInitGameThreads(INT16 nCount);
	BOOL SetClockInterval(INT16 nClockInterval_msec);
	
	AsTimerEventData* AddTimer(UINT32 uDelay, INT32 lCID, PVOID pClass, AsTimerCallBack fpTimerCallBack, PVOID pvData);

// packet monitor functions
private:
	stPacketStatistics	m_stRecvPacketStatistics[APPACKET_MAX_PACKET_TYPE];
	stPacketStatistics	m_stSendPacketStatistics[APPACKET_MAX_PACKET_TYPE];

	UINT32	m_ulStartPacketStatisticsTime;

public:
	BOOL	PacketMonitoring(PVOID pvPacket, INT16 nLength, BOOL bIsSendPacket);

	BOOL	ResetPacketStatistics();

	pstPacketStatistics	GetRecvPacketStatistic(INT32 lPacketType);
	pstPacketStatistics	GetSendPacketStatistic(INT32 lPacketType);

	UINT32	GetSendTotalThroughputBPS();
	UINT32	GetRecvTotalThroughputBPS();

	BOOL	ActiveSendBuffer(UINT32 ulNID);
	BOOL	DisableSendBuffer(UINT32 ulNID);

	BOOL	IsPCRoom(UINT32 ulNID);

	LPTOP_LEVEL_EXCEPTION_FILTER _originalExceptionHandler;

	static LONG __stdcall xCrashExceptionHandler(EXCEPTION_POINTERS* pExPtrs);

	BOOL	SetCrashReportExceptionHandler();

	BOOL	SetLKServerSocket(UINT32 ulNID);
	BOOL	SetBillingSocket(UINT32 ulNID);
	BOOL	SetPatchSocket(UINT32 ulNID);
};

#endif //__AGSENGINE_H__
