/******************************************************************************
Module:  AsCommonLib.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 19
******************************************************************************/

#if !defined(__ASCOMMONLIB_H__)
#define __ASCOMMONLIB_H__

#include "ApBase.h"
#include "ApMutualEx.h"

#include "AsCMDQueue.h"
#include "AsDebugOutput.h"
#include "AspActionValidCheck.h"

#include "AuProfileManager.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AsCommonLibD" )
#else
#pragma comment ( lib , "AsCommonLib" )
#endif
#endif


// stIOCPOBJECT structure
///////////////////////////////////////////////////////////////////////////////
typedef struct _stIOCPOBJECT {
	HANDLE		hCompPort;
	UINT32		ulThreadID;
} stIOCPOBJECT, *pstIOCPOBJECT;

//	AsQueueLIFO class
//		- LIFO Queue를 생성 관리한다.
///////////////////////////////////////////////////////////////////////////////
class AsQueueLIFO {
private:
	INT16		m_nQueueSize;
	INT16		m_nDataSize;
	PVOID		m_pQueueData;

	INT16		m_nNumData;

	ApMutualEx	m_Mutex;

public:
	AsQueueLIFO();
	~AsQueueLIFO();

	BOOL Initialize(INT16 nQueueSize, INT16 nDataSize);

	BOOL PopQueue(PVOID pBuffer);
	BOOL PushQueue(PVOID pBuffer);
	BOOL ResetQueue();
};

/*
//	AsIOCP class
//		- IOCP를 생성하고 IOCP 관련 함수들을 재정의해 파라미터를 줄인다.
///////////////////////////////////////////////////////////////////////////////
class AsIOCP {
private:
	HANDLE		m_hCompPort;

public:
	AsIOCP();
	~AsIOCP();

	UINT32 CreateIoCompletionPort(DWORD dwNumberOfConcurrentThreads);
	UINT32 AssociateDeviceWithCompletionPort(HANDLE hDevice, DWORD dwCompKey);
	UINT32 GetQueuedCompletionStatus(LPDWORD lpNumberOfBytesTransferred, 
				LPDWORD lpCompletionKey, LPOVERLAPPED *lpOverlaped);
	UINT32 PostQueuedCompletionStatus(DWORD dwNumBytes, ULONG_PTR CompKey, OVERLAPPED* pOverlapped);
};
*/

BOOL IsObjectInArea(AuPOS stStartPos, AuPOS stEndPos, AuPOS stObjectPos);

#endif //__ASCOMMONLIB_H__