/******************************************************************************
Module:  AsCommonLib.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 19
******************************************************************************/

#include "AsCommonLib.h"

//	AsQueueLIFO class member functions
///////////////////////////////////////////////////////////////////////////////

//		constructor
//	Functions
//		- none
///////////////////////////////////////////////////////////////////////////////
AsQueueLIFO::AsQueueLIFO()
{
}

//		destructor
//	Functions
//		- 처음 큐에 할당된 메모리를 해제한다.
///////////////////////////////////////////////////////////////////////////////
AsQueueLIFO::~AsQueueLIFO()
{
	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}
}

//		Initialize
//	Functions
//		- initialize queue member variables
//		- memory allocate
//	Arguments
//		- nQueueSize : 
//		- nDataSize :
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::Initialize(INT16 nQueueSize, INT16 nDataSize)
{
	//m_Mutex.WLock();

	m_nQueueSize = nQueueSize;
	m_nDataSize = nDataSize;

	m_pQueueData = (PVOID) GlobalAlloc(GMEM_FIXED, nDataSize);
	if (m_pQueueData == NULL)
	{
		// memory allocation error. log error
		return FALSE;
	}

	m_nNumData = 0;

	//m_Mutex.Release();

	return TRUE;
}

//		PopQueue
//	Functions
//		- 큐에있는 데이타중 젤 뒤에 (나중에 들어온놈) 있는 넘을 꺼낸다.
//	Arguments
//		- pBuffer : 꺼낸 데이타 저장할 포인터
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::PopQueue(PVOID pBuffer)
{
	PROFILE("AsQueueLIFO::PopQueue");

	m_Mutex.WLock();

	if (m_nNumData == 0)
	{
		// queue is empty
		m_Mutex.Release();
		return FALSE;
	}

	CopyMemory(pBuffer, ((CHAR *) m_pQueueData) + (m_nDataSize * m_nNumData), m_nDataSize);

	m_nNumData--;

	m_Mutex.Release();

	return TRUE;
}

//		PushQueue
//	Functions
//		- queue에 데이타를 집어 넣는다.
//	Arguments
//		- pBuffer : 집어넣을 데이터
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::PushQueue(PVOID pBuffer)
{
	PROFILE("AsQueueLIFO::PushQueue");

	m_Mutex.WLock();

	if (m_nNumData == m_nQueueSize)
	{
		// queue is full
		m_Mutex.Release();
		return FALSE;
	}

	m_nNumData++;

	CopyMemory(((CHAR *) m_pQueueData) + (m_nDataSize * m_nNumData), pBuffer, m_nDataSize);

	m_Mutex.Release();

	return TRUE;
}

//		ResetQueue
//	Functions
//		- 큐를 초기화한다.
//	Arguments
//		- none
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::ResetQueue()
{
	m_Mutex.WLock();

	m_nQueueSize = 0;
	m_nDataSize = 0;

	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}

	m_nNumData = 0;

	m_Mutex.Release();

	return TRUE;
}

/*
//	AsIOCP class member functions
///////////////////////////////////////////////////////////////////////////////

//		constructor
//	Functions
//		- i/o completion port 생성
///////////////////////////////////////////////////////////////////////////////
AsIOCP::AsIOCP()
{
}

//		destructor
//	Functions
//		- 생성된 completion port 삭제
///////////////////////////////////////////////////////////////////////////////
AsIOCP::~AsIOCP()
{
	if (m_hCompPort != NULL)
	{
		CloseHandle(m_hCompPort);
	}
}

//		CreateIoCompletionPort (wrap function)
//	Functions
//		- CreateIoCompletionPort()를 호출해 생성된 핸들을 m_hCompPort에 넣는다.
//	Arguments
//		- dwNumberOfConcurrentThreads : 생성할 worker thread 수 ('0'이면 CPU 갯수만큼 생성)
//	Return value
//		- TRUE
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::CreateIoCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	m_hCompPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);

	return TRUE;
}

//		CreateIoCompletionPort (wrap function)
//	Functions
//		- CreateIoCompletionPort()를 호출해 생성된 핸들을 m_hCompPort에 넣는다.
//	Arguments
//		- dwNumberOfConcurrentThreads : 생성할 worker thread 수 ('0'이면 CPU 갯수만큼 생성)
//	Return value
//		- TRUE
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::AssociateDeviceWithCompletionPort(HANDLE hDevice, DWORD dwCompKey)
{
	HANDLE hTemp = ::CreateIoCompletionPort(hDevice, m_hCompPort, dwCompKey, 0);

	return TRUE;
}

//		GetQueuedCompletionStatus (wrap function)
//	Functions
//		- GetQueuedCompletionStatus()를 호출한 후 validate check
//	Arguments
//		- lpNumberOfBytesTransTransferred : 전송된 바이트 수
//		- lpCompletionKey : 전송된 키 데이타
//		- lpOverlapped : 전송된 데이타 포인터
//	Return value
//		- TRUE : 성공
//		- dwError : 각종 에러들... 실패...
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::GetQueuedCompletionStatus(LPDWORD lpNumberOfBytesTransferred, 
				LPDWORD lpCompletionKey, LPOVERLAPPED *lpOverlapped)
{
	BOOL fOK = ::GetQueuedCompletionStatus(m_hCompPort, lpNumberOfBytesTransferred, 
						lpCompletionKey, lpOverlapped, INFINITE);
	DWORD dwError = GetLastError();

	if (fOK)
	{
		MessageBox(NULL, "Ok... now process", "AsIOCP", 1);
		return TRUE;
	}
	else
	{
		if (lpOverlapped != NULL) {
			MessageBox(NULL, "Process a failed completed I/O request. dwError contains the reason for failure", "AsIOCP", 1);
		} else {
			if (dwError == WAIT_TIMEOUT) {
				MessageBox(NULL, "Time-out while waiting for completed I/O entry", "AsIOCP", 1);
			} else {
				MessageBox(NULL, "Bad call to GetQueuedCompletionStatus. dwError contails the reason for the bad call", "AsIOCP", 1);
			}
		}

		return dwError;
	}
}

//		PostQueuedCompletionStatus (wrap function)
//	Functions
//		- PostQueuedCompletionStatus()를 호출한 후 result return
//	Arguments
//		- dwNumBytes : 
//	Return value
//		- PostQueuedCompletionStatus return value
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::PostQueuedCompletionStatus(DWORD dwNumBytes, ULONG_PTR CompKey, OVERLAPPED* pOverlapped)
{
	return (::PostQueuedCompletionStatus(m_hCompPort, dwNumBytes, CompKey, pOverlapped));
}
*/

//		IsObjectInArea
//	Functions
//		- object가 어떤 범위 안에 있는지 검사한다.
//	Arguments
//		- stStartPos : 범위 시작점
//		- stEndPos : 범위 종료점
//		- stObjectPos : object 위치
//	Return value
//		- TRUE : 범위안에 존재
//		- FALSE : 범위 밖에 존재
///////////////////////////////////////////////////////////////////////////////
BOOL IsObjectInArea(AuPOS stStartPos, AuPOS stEndPos, AuPOS stObjectPos)
{
	if ((stStartPos.x <= stObjectPos.x) && (stObjectPos.x <= stEndPos.x) &&
		(stStartPos.y <= stObjectPos.y) && (stObjectPos.y <= stEndPos.y))
		return TRUE;
	else
		return FALSE;
}