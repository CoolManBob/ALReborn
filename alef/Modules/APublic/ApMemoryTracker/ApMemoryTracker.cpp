// ApMemoryTracker.cpp: implementation of the ApMemoryTracker class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "ApMemoryTracker.h"
#include "MagDebug.h"


#ifdef _AP_MEMORY_TRACKER_

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define APMEMORY_TRACKER_FILENAME "ApMemory_Tracking.log"

static CHAR* strReportStatus[] =
{
	"NONE",			// None
	"LEAKS",		// ReportLeaks를 이용한 Report	
	"CRASH",		// ReportViolatioinGuardBytes를 이용한 Report
	"DELETE"		// RemoveTracking시 GuardByte를 체크하여 문제가 발생했을때를 위한 Report 
};

ApMemoryTracker* ApMemoryTracker::m_pThis = NULL;

ApMemoryTracker::ApMemoryTracker()
{
	m_lSequenceNumber	= 0;
	m_lTotalLeakSize	= 0;
	m_lTotalLeakCount	= 0;
	ZeroMemory(m_szBuffer, MESSAGE_BUFFER_SIZE + 1);
}

ApMemoryTracker::~ApMemoryTracker()
{
	m_pThis = NULL;
//	DestroyInstance();
}

ApMemoryTracker& ApMemoryTracker::GetInstance()
{
	if (NULL == m_pThis)
	{
#ifdef new
#undef new
#endif
		m_pThis = new ApMemoryTracker;
		atexit(ApMemoryTracker::DestroyInstance);
#ifdef new
#undef new
#define new DEBUG_NEW
#endif
	}

	return *m_pThis;
}

void ApMemoryTracker::DestroyInstance()
{
	if (m_pThis)
	{
		ApMemoryTracker *	pThis = m_pThis;

		m_pThis = NULL;

		pThis->Destroy();

		delete pThis;
	}
}

BOOL ApMemoryTracker::Destroy()
{
	AuAutoLock Lock(m_CriticalSection);
	if (!Lock.Result()) return FALSE;

	HashMap_TrackingData::iterator iter = m_HashMapTracking.begin();
	while (iter != m_HashMapTracking.end())
	{
		delete (*iter).second;
		m_HashMapTracking.erase(iter++);
//		++iter;
	}

//	m_HashMapTracking.clear();

	return TRUE;
}

BOOL ApMemoryTracker::CheckStartGuardBytes(PVOID ptr)
{
	_CrtMemBlockHeader* pHead = pHdr(ptr);
	return (*(DWORD*)pHead->gap == CRT_GUARD_BYTES);
}

BOOL ApMemoryTracker::CheckEndGuardBytes(PVOID ptr)
{
	_CrtMemBlockHeader* pHead = pHdr(ptr);
	return (*(DWORD*)(pbData(pHead) + pHead->nDataSize) == CRT_GUARD_BYTES);
}	

void ApMemoryTracker::AddTracking(PVOID ptr, INT32 lSize, const char* szFilename, INT32 lLine)
{
	AuAutoLock Lock(m_CriticalSection);
	if (!Lock.Result()) return;

	ApMemoryTrackingData* pTrackingData = new ApMemoryTrackingData;
	pTrackingData->pAddress = ptr;
	pTrackingData->lSize = lSize;
	pTrackingData->lLineNumber = lLine;
	pTrackingData->szFilename = szFilename;
	pTrackingData->lSequenceNumber = InterlockedIncrement(&m_lSequenceNumber);

	m_HashMapTracking.insert(HashMap_TrackingData::value_type((DWORD)ptr, pTrackingData));	
}

BOOL	g_bAssertOK = FALSE;

void ApMemoryTracker::RemoveTracking(PVOID ptr)
{
	AuAutoLock Lock(m_CriticalSection);
	if (!Lock.Result()) return;

	ApMemoryTrackingData* pTrackingData = NULL;
	HashMap_TrackingData::iterator iter = m_HashMapTracking.find((DWORD)ptr);

	if (g_bAssertOK)
	{
		ASSERT(iter != m_HashMapTracking.end());
	}

	if (iter != m_HashMapTracking.end())
	{
		pTrackingData = (*iter).second;
		
		if (!CheckStartGuardBytes(pTrackingData->pAddress) || 
			!CheckEndGuardBytes(pTrackingData->pAddress))
		{
			ReportFormat(pTrackingData, REPORT_STATUS_DELETE);
			ReportDisplay(REPORT_MODE_FILE);
		}
		delete pTrackingData;
		m_HashMapTracking.erase(iter);
	}
}

void ApMemoryTracker::ReportFormat(ApMemoryTrackingData* pTrackingData, EnumReportStatus eReportStatus)
{
	INT32 lSize = sprintf(m_szBuffer, "%s\tSG[%s]\tEG[%s]\t[%s(%d)]\tSeq[%d]\tsize[%d]\tAddr[0x%.8x]\tInfo[%s]\n", 
						strReportStatus[eReportStatus],
						CheckStartGuardBytes(pTrackingData->pAddress) ? "O" : "X",
						CheckEndGuardBytes(pTrackingData->pAddress) ? "O" : "X",
						strrchr(pTrackingData->szFilename, '\\') + 1, 
						pTrackingData->lLineNumber, 
						pTrackingData->lSequenceNumber, 
						pTrackingData->lSize,
						pTrackingData->pAddress,
						pTrackingData->szText);

	ASSERT(lSize < MESSAGE_BUFFER_SIZE);
}

void ApMemoryTracker::ReportViolatioinGuardBytes(EnumReportMode eReportMode)
{
	AuAutoLock Lock(m_CriticalSection);
	if (!Lock.Result()) return;

	ReportMemoryTrackingInfo(eReportMode);
	HashMap_TrackingData::iterator iter = m_HashMapTracking.begin();
	while (iter != m_HashMapTracking.end())
	{
		if (FALSE == CheckStartGuardBytes( ((*iter).second)->pAddress) ||
			FALSE == CheckEndGuardBytes( ((*iter).second)->pAddress) )
		{
			ReportFormat((*iter).second, REPORT_STATUS_CRASH);
			ReportDisplay(eReportMode);
		}

		++iter;
	}	
}

void ApMemoryTracker::ReportLeaks(EnumReportMode eReportMode)
{
	AuAutoLock Lock(m_CriticalSection);
	if (!Lock.Result()) return;

	ReportMemoryTrackingInfo(eReportMode);
	HashMap_TrackingData::iterator iter = m_HashMapTracking.begin();
	while (iter != m_HashMapTracking.end())
	{
		ReportFormat((*iter).second, REPORT_STATUS_LEAK);
		ReportDisplay(eReportMode);

		m_lTotalLeakSize += (*iter).second->lSize;
		++m_lTotalLeakCount;

		++iter;
	}

	if (REPORT_MODE_FILE == eReportMode)
	{
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Total Leak Size : %d, Total Leak Count : %d", m_lTotalLeakSize, m_lTotalLeakCount);
		AuLogFile_s(APMEMORY_TRACKER_FILENAME, strCharBuff);
#endif
	}

	m_lTotalLeakSize	= 0;
	m_lTotalLeakCount	= 0;
}

void ApMemoryTracker::ReportDisplay(EnumReportMode eReportMode)
{
	switch (eReportMode)
	{
	case REPORT_MODE_TRACE:
		TRACE(m_szBuffer);
		break;

	case REPORT_MODE_CONSOLE:
		printf("%s", m_szBuffer);
		break;

	case REPORT_MODE_FILE:
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), m_szBuffer);
		AuLogFile_s(APMEMORY_TRACKER_FILENAME, strCharBuff);
#endif
		break;
	}
}

void ApMemoryTracker::ReportMemoryTrackingInfo(EnumReportMode eReportMode)
{
	INT32 lSize = sprintf(m_szBuffer, "Current Tracking Count %d, SequenceNumber : %d\n\n", 
							m_HashMapTracking.size(), 
							m_lSequenceNumber);

	ASSERT(lSize < MESSAGE_BUFFER_SIZE);

	ReportDisplay(eReportMode);
}

void ApMemoryTracker::AddInformation(PVOID ptr, CHAR* szText)
{
	ASSERT(INFORMATION_SIZE > strlen(szText));

	ApMemoryTrackingData* pTrackingData = NULL;
	HashMap_TrackingData::iterator iter = m_HashMapTracking.find((DWORD)ptr);

	if (iter != m_HashMapTracking.end())
	{
		pTrackingData = (*iter).second;
		strncpy(pTrackingData->szText, szText, INFORMATION_SIZE);
	}
}
#else
void DummyFunction1()
{
	// 이건 빌드 워닝 방지용
	// 더미 펑션

	DebugBreak();
}

#endif
