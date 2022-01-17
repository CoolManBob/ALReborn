// AgcmAdminDlgXT_IOCP.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 02.

#include "StdAfx.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmAdminDlgXT_IOCP::AgcmAdminDlgXT_IOCP()
{
	Initialize();
}

AgcmAdminDlgXT_IOCP::~AgcmAdminDlgXT_IOCP()
{
	Destroy();
}

VOID AgcmAdminDlgXT_IOCP::Initialize()
{
	m_hIOCP = NULL;
	m_lThreadCount = 0;
	::ZeroMemory(m_lWorkerThreadIDs, sizeof(UINT) * MAX_THREAD_COUNT);
	::ZeroMemory(m_hWorkerThreadHandles, sizeof(HANDLE) * MAX_THREAD_COUNT);
}

HANDLE AgcmAdminDlgXT_IOCP::GetHandle()
{
	return m_hIOCP;
}

INT32 AgcmAdminDlgXT_IOCP::GetThreadCount()
{
	return m_lThreadCount;
}

BOOL AgcmAdminDlgXT_IOCP::Create(INT32 lThreadCount, ThreadProc WalkerThread, PVOID pvParam)
{
	// IOCP 생성
	m_lThreadCount = lThreadCount;
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, lThreadCount);
	if (!m_hIOCP) return FALSE;

	// Worker thread 생성
	for (INT32 i = 0; i < lThreadCount; i++)
	{
		m_hWorkerThreadHandles[i] = (HANDLE)_beginthreadex(NULL, 0, WalkerThread, pvParam, 0, &m_lWorkerThreadIDs[i]);
		if ((!m_hWorkerThreadHandles[i]) || (!m_lWorkerThreadIDs[i])) return FALSE;
	}

	return TRUE;
}

VOID AgcmAdminDlgXT_IOCP::Destroy()
{
	// IOCP handle 종료
	if (NULL != m_hIOCP) ::CloseHandle(m_hIOCP);

	// Worker thread가 없으면 리턴
	if (0 == m_lThreadCount) return;

	// Worker thread 종료
	for(INT32 i = 0; i < m_lThreadCount; ++i)
	{
		::TerminateThread(m_hWorkerThreadHandles[i], 1);
		::CloseHandle(m_hWorkerThreadHandles[i]);	// 핸들을 닫음
	}
	
	// 모든 쓰레드가 종료되기를 기다림
	::WaitForMultipleObjects(m_lThreadCount, m_hWorkerThreadHandles, TRUE, INFINITE);

	Initialize();
}

BOOL AgcmAdminDlgXT_IOCP::AttachHandle(HANDLE hHandle, DWORD dwKey)
{
	return (::CreateIoCompletionPort(hHandle, m_hIOCP, dwKey, 0) == m_hIOCP);
}

BOOL AgcmAdminDlgXT_IOCP::PostStatus(DWORD dwKey, DWORD dwBytes, LPOVERLAPPED lpOverlapped)
{
	return ::PostQueuedCompletionStatus(m_hIOCP, dwBytes, dwKey, lpOverlapped);
}

BOOL AgcmAdminDlgXT_IOCP::GetStatus(DWORD *pdwKey, DWORD *pdwBytes, LPOVERLAPPED *plpOverlapped, DWORD dwTime)
{
	return ::GetQueuedCompletionStatus(m_hIOCP, pdwBytes, pdwKey, plpOverlapped, dwTime);
}