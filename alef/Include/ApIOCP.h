/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// ApIOCP.h: interface for the ApIOCP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APIOCP_H__B8DA05C0_3595_467D_812B_9F81A29D72B9__INCLUDED_)
#define AFX_APIOCP_H__B8DA05C0_3595_467D_812B_9F81A29D72B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

typedef UINT (WINAPI *ThreadProc)(PVOID);
const INT32 MAX_THREAD_COUNT = 20;	// 최대 쓰레드 갯수정의

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApIOCPDispatcherD" )
#else
#pragma comment ( lib , "ApIOCPDispatcher" )
#endif
#endif

class ApIOCP  
{
private:
	HANDLE	m_hIOCP;
	//HANDLE	m_hWorkerThreadHandles[MAX_THREAD_COUNT];
	//UINT	m_lWorkerThreadIDs[MAX_THREAD_COUNT];
	ApSafeArray<HANDLE, MAX_THREAD_COUNT>	m_hWorkerThreadHandles;
	ApSafeArray<UINT, MAX_THREAD_COUNT>		m_lWorkerThreadIDs;
	INT32	m_lThreadCount;
	
public:
	ApIOCP();
	virtual ~ApIOCP();
	
	VOID	Initialize();

	HANDLE	GetHandle();
	INT32	GetThreadCount();

	BOOL	Create(INT32 lThreadCount, ThreadProc WalkerThread, PVOID pvParam, CHAR* ThreadName);
	VOID	Destroy();

	BOOL	AttachHandle(HANDLE hHandle, ULONG_PTR dwKey);
	BOOL	PostStatus(ULONG_PTR dwKey, DWORD dwBytes = 0, LPOVERLAPPED lpOverlapped = NULL);
	BOOL	GetStatus(ULONG_PTR *pdwKey, DWORD *pdwBytes, LPOVERLAPPED *plpOverlapped, DWORD dwTime = INFINITE);
};

#endif // !defined(AFX_APIOCP_H__B8DA05C0_3595_467D_812B_9F81A29D72B9__INCLUDED_)
