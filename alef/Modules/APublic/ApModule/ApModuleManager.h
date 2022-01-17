/******************************************************************************
Module:  ApModuleManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 23
******************************************************************************/

#if !defined(__APMODULEMANAGER_H__)
#define __APMODULEMANAGER_H__

#include "ApBase.h"
#include "ApModule.h"
#include "ApMutualEx.h"
#include "AuTickCounter.h"
#include "AuPacket.h"

typedef enum
{
	APMM_PACKET_CLIENT_CODE_UNKNOWN	= 0,
	APMM_PACKET_CLIENT_CODE_EXIT,
	APMM_PACKET_CLIENT_CODE_CRASH
} ApMMClientCode;

typedef struct ApModuleList
{
	ApModule*			pModule;
	ApModuleList*		next;
} ApModuleList;

class ApModuleManager {
protected:
	ApModuleList *	m_listModule;
	ApModuleList *	m_listModuleIdle;
	ApModuleList *	m_listModuleIdle2;
	ApModuleList *	m_listModuleIdle3;
	
	ApMutualEx		m_hMutex;
	UINT32			m_ulClockCount;

	AuTickCounter	m_csTickCounter;

	AuPacket		m_csSystemPacket;

public:
	ApModule*		m_pModuleDisp[APPACKET_MAX_PACKET_TYPE];
	
	// 2004.05.21. steeple
public:
	PVOID m_pSlowIdleLogClass;
	ApModuleDefaultCallBack m_pfSlowIdleLogCB;
	BOOL WriteSlowIdleLog(CHAR* szModuleName, INT32 lTick);

	PVOID m_pSlowDispatchLogClass;
	ApModuleDefaultCallBack m_pfSlowDispatchLogCB;
	BOOL WriteSlowDispatchLog(CHAR* szModuleName, INT32 lTick);


public:
	ApModuleManager();
	~ApModuleManager();

	BOOL		Initialize();

	BOOL		AddModule(ApModule* pModule);
	BOOL		RemoveModule(CHAR* szModuleName);

	ApModule*	GetModule(CHAR* szModuleName);
	ApModule*	GetModule(INT32	index);			// 0-based index·Î Á¢±Ù
	INT32		GetModuleCount();

	BOOL		Dispatch(CHAR* szData, INT16 nSize, UINT32 ulNID = 0, DispatchArg *pstCheckArg = NULL);

	VOID		EnableModuleIdle(ApModule* pModule, BOOL bUseIdle);
	VOID		EnableModuleIdle2(ApModule* pModule, BOOL bUseIdle);
	VOID		EnableModuleIdle3(ApModule* pModule, BOOL bUseIdle);
	BOOL		Idle(UINT32 ulClockCount);
	BOOL		Idle2(UINT32 ulClockCount);
	BOOL		Idle3(UINT32 ulClockCount);
	BOOL		Destroy();

	UINT32		GetClockCount();
	UINT32		GetPrevClockCount();

	virtual BOOL OnAddModule() { return TRUE; }

	virtual BOOL OnPostDispatch() { return TRUE; }

	virtual BOOL SendPacket(PVOID pvPacket, INT16 nLength, UINT32 ulNID = 0, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 nFlag = APMODULE_SENDPACKET_PLAYER) { return TRUE; }

	virtual BOOL SetSelfCID(INT32 lCID) { return TRUE; }

	virtual BOOL PacketMonitoring(PVOID pvPacket, INT16 nLength, BOOL bIsSendPacket) { return TRUE; }

	// System Packet
	PVOID		MakeSPClientExit(INT16 *pnSize);
	PVOID		MakeSPClientCrash(INT16 *pnSize, CHAR *szReason);
	inline VOID	FreeSystemPacket(PVOID pvPacket)	{ m_csSystemPacket.FreePacket(pvPacket); }

	virtual BOOL OnSPReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	virtual BOOL OnSPClientExit(UINT32 ulNID)	{ return TRUE; }
	virtual BOOL OnSPClientCrash(UINT32 ulNID, CHAR *szReason)	{ return TRUE; }
	
	// Report
	BOOL		ReportAll();
	BOOL		ReportModule(CHAR *pszModuleName, CHAR *pszFileName = NULL);
};

#endif //__APMODULEMANAGER_H__