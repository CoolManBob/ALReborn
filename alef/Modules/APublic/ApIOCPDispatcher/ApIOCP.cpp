// ApIOCP.cpp: implementation of the ApIOCP class.
//
//////////////////////////////////////////////////////////////////////

#include "ApIOCP.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ApIOCP::ApIOCP()
{
	Initialize();
}

ApIOCP::~ApIOCP()
{
	Destroy();
}

VOID ApIOCP::Initialize()
{
	m_hIOCP = NULL;
	m_lThreadCount = 0;
//	::ZeroMemory(m_lWorkerThreadIDs, sizeof(UINT) * MAX_THREAD_COUNT);
//	::ZeroMemory(m_hWorkerThreadHandles, sizeof(HANDLE) * MAX_THREAD_COUNT);

	m_lWorkerThreadIDs.MemSetAll();
	m_hWorkerThreadHandles.MemSetAll();
}

HANDLE ApIOCP::GetHandle()
{
	return m_hIOCP;
}

INT32 ApIOCP::GetThreadCount()
{
	return m_lThreadCount;
}

BOOL ApIOCP::Create(INT32 lThreadCount, ThreadProc WalkerThread, PVOID pvParam, CHAR* ThreadName)
{
	// IOCP 생성
	m_lThreadCount = lThreadCount;
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, lThreadCount);
	if (!m_hIOCP) return FALSE;

	CHAR szBuffer[256];

	// Worker thread 생성
	for (INT32 i = 0; i < lThreadCount; ++i)
	{
		::ZeroMemory(szBuffer, sizeof(szBuffer));
		m_hWorkerThreadHandles[i] = (HANDLE)_beginthreadex(NULL, 0, WalkerThread, pvParam, 0, &m_lWorkerThreadIDs[i]);
		if ((!m_hWorkerThreadHandles[i]) || (!m_lWorkerThreadIDs[i])) return FALSE;

		sprintf(szBuffer, "%s_%d", ThreadName, m_lWorkerThreadIDs[i]);
		SetThreadName(m_lWorkerThreadIDs[i], szBuffer);
	}

	return TRUE;
}

VOID ApIOCP::Destroy()
{
	// IOCP handle 종료
	if (NULL != m_hIOCP) ::CloseHandle(m_hIOCP);

	// Worker thread가 없으면 리턴
	if (0 == m_lThreadCount) return;

	// Worker thread 종료
	for(INT32 i = 0; i < m_lThreadCount; ++i)
	{
//		::TerminateThread(m_hWorkerThreadHandles[i], 1);
		::CloseHandle(m_hWorkerThreadHandles[i]);	// 핸들을 닫음
	}
	
	// 모든 쓰레드가 종료되기를 기다림
	::WaitForMultipleObjects(m_lThreadCount, &m_hWorkerThreadHandles[0], TRUE, INFINITE);
	
	Initialize();
}

BOOL ApIOCP::AttachHandle(HANDLE hHandle, ULONG_PTR dwKey)
{
	return (::CreateIoCompletionPort(hHandle, m_hIOCP, dwKey, 0) == m_hIOCP);
}

BOOL ApIOCP::PostStatus(ULONG_PTR dwKey, DWORD dwBytes, LPOVERLAPPED lpOverlapped)
{
	return ::PostQueuedCompletionStatus(m_hIOCP, dwBytes, dwKey, lpOverlapped);
}

BOOL ApIOCP::GetStatus(ULONG_PTR *pdwKey, DWORD *pdwBytes, LPOVERLAPPED *plpOverlapped, DWORD dwTime)
{
	return ::GetQueuedCompletionStatus(m_hIOCP, pdwBytes, pdwKey, plpOverlapped, dwTime);
}