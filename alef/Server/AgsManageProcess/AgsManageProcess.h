/******************************************************************************
Module:  AgsManageProcess.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 19
******************************************************************************/

#if !defined(__AGSMANAGEPROCESS_H__)
#define __AGSMANAGEPROCESS_H__

#include "windows.h"

#include "ApDefine.h"
#include "AsDefine.h"
#include "AsDP8.h"
#include "AsDP8Client.h"
#include "AgsGameProcess.h"
//#include "AgsNotiProcess.h"
#include "ApModuleManager.h"

//#include "AgsSystemStart.h"


#ifdef	_DEBUG
#pragma comment ( lib , "AgsManageProcessD" )
#else
#pragma comment ( lib , "AgsManageProcess" )
#endif


class AgsManageProcess {
private:
	INT16*				m_pnServerStatus;
	ApModuleManager*	m_pmmModuleManager;
	AsDP8*				m_pDPModule;
//	AsCMDQueue*			m_pqueueResult;
//	AsCMDQueue*			m_pqueueResultSvr;
	AsCMDQueue*			m_pqueueRecv;
	AsCMDQueue*			m_pqueueRecvSvr;

	//INT16				m_nNumDPClient;
	//AsDP8Client*		m_pDPClientModule;

private:
	stTHREADOBJECT		m_stGameProcPool[AGSMP_POOL_GAMEPROC];
//	stTHREADOBJECT		m_stNotiProcPool[AGSMP_POOL_NOTIPROC];

	INT16				m_nDefaultGameProc;
//	INT16				m_nDefaultNotiProc;

	INT16				m_nNumGameProc;
//	INT16				m_nNumNotiProc;

	INT16				m_nNumProcessClient[AGSMP_POOL_GAMEPROC];

	INT16				m_nNumTotalProc;

	HANDLE				m_hWaitHandles[AGSMP_POOL_GAMEPROC + AGSMP_POOL_NOTIPROC];

	//BOOL				MessageHandler(MSG *pMsg);

	//BOOL				SelectThreadForNewPlayer(UINT32 *pulThreadID);

	UINT32				m_ulClockCount;

	INT16				m_nClockInterval;

public:
	AgsManageProcess(INT16 *pnServerStatus, AsDP8 *pDPModule, ApModuleManager *pmmModuleManager, 
					 AsCMDQueue* pqueueRecv, AsCMDQueue* pqueueRecvSvr, /*AsCMDQueue* pqueueResult, AsCMDQueue* pqueueResultSvr,*/
					 INT16 nClockInterval, INT16 nDefaultGameProc/*, INT16 nDefaultNotiProc*/);
	~AgsManageProcess();

	static DWORD WINAPI Process(PVOID pvParam);
	static VOID APIENTRY TimerAPCRoutine(PVOID pvArgToCompletionRoutine,
							DWORD dwTimerLowValue, DWORD dwTimerHighValue);

	static DWORD WINAPI StartGameProcess(PVOID pvParam);
	static DWORD WINAPI StartNotiProcess(PVOID pvParam);

	//static DWORD WINAPI ConnectServers(PVOID pvParam);

	BOOL ResetClockCount(UINT32 ulNewClockCount);
	BOOL AddClockCount();
	UINT32 GetClockCount();

	BOOL Start();
	BOOL Stop();

	//BOOL SystemStart();
};

#endif //__AGSMANAGEPROCESS_H__
