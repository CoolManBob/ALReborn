/******************************************************************************
Module:  AgsClientProcess.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 18
******************************************************************************/

#if !defined(__AGSCLIENTPROCESS_H__)
#define __AGSCLIENTPROCESS_H__

#include "windows.h"
#include "AsDefine.h"
#include "AsCMDQueue.h"
#include "ApModuleManager.h"

#include "AsDP8.h"

#include <dplay8.h>


#ifdef	_DEBUG
#pragma comment ( lib , "AgsGameProcessD" )
#else
#pragma comment ( lib , "AgsGameProcess" )
#endif


class AgsGameProcess {
private:
	AsCMDQueue*			m_pqueueRecv;
	AsCMDQueue*			m_pqueueRecvSvr;
	ApModuleManager*	m_pmmModuleManager;

	INT16*				m_pnServerStatus;

	AsDP8*				m_pDPModule;

public:
	INT16	m_nQueueIndex;

	HANDLE	m_hClockTimer;

private:
	INT16	m_nNumClient;						// 처리하고 있는 클라이언트 수

public:
	AgsGameProcess(INT16 *pnServerStatus, INT16 nQueueIndex, AsDP8 *pDPModule,
				   AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr, ApModuleManager *pmmModuleManager);
	~AgsGameProcess();

	INT16	GetNumClient() { return m_nNumClient; }

	BOOL	Process();

private:
	INT16	ProcessClient();
	INT16	ProcessServer();

	BOOL	AddNewClient(DPNID dpnidPlayer);
	BOOL	RemoveClient(DPNID dpnidPlayer);

	BOOL	AddNewServer(DPNID dpnidServer);
	BOOL	RemoveServer(DPNID dpnidServer);

	//static VOID APIENTRY UpdateAPCRoutine(PVOID pvArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue);

	//BOOL	SendResult();
};

#endif //__AGSCLIENTPROCESS_H__