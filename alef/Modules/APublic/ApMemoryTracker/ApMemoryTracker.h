// ApMemoryTracker.h: interface for the ApMemoryTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMEMORYTRACKER_H__B97246E2_4CB5_4BC2_9E8D_0D7B6E0D5242__INCLUDED_)
#define AFX_APMEMORYTRACKER_H__B97246E2_4CB5_4BC2_9E8D_0D7B6E0D5242__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// default로 Memory Tracker를 사용하지 않는다.
//#define _AP_MEMORY_TRACKER_ 
#ifdef _AP_MEMORY_TRACKER_

#include "ApMemory.h"
#include "ApCRTHeader.h"
#include <unordered_map>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApMemoryTrackerD" )
#else
#pragma comment ( lib , "ApMemoryTracker" )
#endif
#endif

// 일단은 disable시켜놓고 차후에 보완한다. - kermi
#pragma warning ( disable : 4291 )

#define MESSAGE_BUFFER_SIZE 512
#define INFORMATION_SIZE	40

enum EnumReportMode
{
	REPORT_MODE_NONE		= 0,
	REPORT_MODE_TRACE,
	REPORT_MODE_CONSOLE,
	REPORT_MODE_FILE,
};

enum EnumReportStatus
{
	REPORT_STATUS_NONE		= 0,
	REPORT_STATUS_LEAK,
	REPORT_STATUS_CRASH,
	REPORT_STATUS_DELETE,
};

struct ApMemoryTrackingData : ApMemory<ApMemoryTrackingData, 1000000>
{
	PVOID pAddress;				// 할당된 메모리 주소
	INT32 lSize;				// 사이즈
	INT32 lLineNumber;			// new를 호출한 소스의 라인넘버
	const char* szFilename;		// new를 호출한 소스의 파일이름
	LONG lSequenceNumber;		// requestNumber와 비슷한 new가 호출된 sequenceNumber
	CHAR szText[INFORMATION_SIZE];	// 추가적인 정보를 저장한다.

	ApMemoryTrackingData()
	{
		szText[0] = 0x00;
	}
};

class ApMemoryTracker  
{
private:
	typedef std::unordered_map<DWORD, ApMemoryTrackingData*> HashMap_TrackingData;
	HashMap_TrackingData	m_HashMapTracking;
	ApCriticalSection		m_CriticalSection;
	LONG					m_lSequenceNumber;
	CHAR					m_szBuffer[MESSAGE_BUFFER_SIZE + 1];

	LONG					m_lTotalLeakCount;
	LONG					m_lTotalLeakSize;

public:
	static ApMemoryTracker*	m_pThis;

private:
	ApMemoryTracker();

	BOOL CheckStartGuardBytes(PVOID ptr);
	BOOL CheckEndGuardBytes(PVOID ptr);

	void ReportFormat(ApMemoryTrackingData* pTrackingData, EnumReportStatus eReportStatus);
	void ReportDisplay(EnumReportMode eReportMode);
	void ReportMemoryTrackingInfo(EnumReportMode eReportMode);
	
public:
	virtual ~ApMemoryTracker();

	static ApMemoryTracker& GetInstance();
	static void DestroyInstance();

	BOOL Destroy();

	void AddTracking(PVOID ptr, INT32 lSize, const char* szFilename, INT32 lLine);
	void RemoveTracking(PVOID ptr);
	void ReportViolatioinGuardBytes(EnumReportMode eReportMode);
	void ReportLeaks(EnumReportMode eReportMode);
	void AddInformation(PVOID ptr, CHAR* szText);
};

#ifdef _DEBUG
inline void* __cdecl operator new(unsigned int size, const char* file, int line)
{
	PVOID Pointer = (PVOID)malloc(size);
	ApMemoryTracker::GetInstance().AddTracking(Pointer, size, file, line);
	return Pointer;
};

inline void __cdecl operator delete(void *ptr)
{
	if (!ApMemoryTracker::m_pThis)
		return;

	ApMemoryTracker::GetInstance().RemoveTracking(ptr);
	free(ptr);
};
//@{ Jaewon 20041124
inline void __cdecl operator delete[](void *ptr)
{
	if (!ApMemoryTracker::m_pThis)
		return;

	ApMemoryTracker::GetInstance().RemoveTracking(ptr);
	free(ptr);
};
//@} Jaewon
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#define new DEBUG_NEW

// DISABLE_MEMORY_TRACKING
//		#ifdef new
//		#undef new
//		#endif

// ENABLE_MEMORY_TRACKING
//		#ifdef new
//		#undef new
//		#define new DEBUG_NEW
//		#endif	

#endif

#endif // !defined(AFX_APMEMORYTRACKER_H__B97246E2_4CB5_4BC2_9E8D_0D7B6E0D5242__INCLUDED_)
