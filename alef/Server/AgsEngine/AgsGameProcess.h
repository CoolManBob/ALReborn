/******************************************************************************
Module:  AgsClientProcess.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 18
******************************************************************************/

#if !defined(__AGSCLIENTPROCESS_H__)
#define __AGSCLIENTPROCESS_H__

#include "ApBase.h"
#include "AsDefine.h"
#include "ApModuleManager.h"
#include "ApIOCPDispatcher.h"
#include "AsIOCPServer.h"

class AgsGameProcess;

// Game Thread를 구동시키는데 필요한 것들
struct ParamGameThread
{
	AgsGameProcess		*pcsGameProcess;
	ApIOCPDispatcher	*pcsIOCPDispatcher;
};

// Idle Thread를 구동시키는데 필요한 것들
struct ParamIdleThread
{
	AgsGameProcess		*pcsGameProcess;
	INT32				nmsecClockInterval;
};

class AgsGameProcess {
private:
	ApModuleManager*	m_pmmModuleManager;
	INT16*				m_pnServerStatus;

public:
	AgsGameProcess(INT16 *pnServerStatus, ApModuleManager *pmmModuleManager);
	~AgsGameProcess();

	static BOOL GameThread(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);

	static UINT WINAPI IdleThread(PVOID pvParam);
	static UINT WINAPI ProtectIdleThread(PVOID pvParam);

	static UINT WINAPI IdleThread2(PVOID pvParam);
	static UINT WINAPI ProtectIdleThread2(PVOID pvParam);

	static UINT WINAPI IdleThread3(PVOID pvParam);
	static UINT WINAPI ProtectIdleThread3(PVOID pvParam);

	static BOOL CBDisconnect(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
	static BOOL ExceptionFilter(LPEXCEPTION_POINTERS pException);

	BOOL	ProcessCommand(pstCOMMAND pstCommand);

private:
	BOOL	CmdClient(pstCOMMAND pstCommand);
	BOOL	CmdServer(pstCOMMAND pstCommand);

	BOOL	AddNewClient(INT32 dpnidPlayer);
	BOOL	RemoveClient(INT32 dpnidPlayer);

	BOOL	AddNewServer(INT32 dpnidServer);
	BOOL	RemoveServer(INT32 dpnidServer);
};

#endif //__AGSCLIENTPROCESS_H__