// ApHeap.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2008.05.06.

#include "ApHeap.h"

//#define APHEAP_PROFILE
#define APHEAP_REPORT_FILE_NAME		"log\\ApHeap_Report.log"

ApHeap::ApHeap(EunmApHeapMode eMode)
{
	m_eMode = eMode;
}

ApHeap::~ApHeap()
{
	DestroyAll();
}

BOOL ApHeap::Create(SIZE_T llHeapSize)
{
	// 이미 map 에 있다면 하지 않아야 한다.
	HeapIter iter = m_MapHeap.find(::GetCurrentThreadId());
	if(iter != m_MapHeap.end())
		return FALSE;

	ApHeapData stData;

	stData.m_hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, llHeapSize, 0);
	if(!stData.m_hHeap)
	{
		ASSERT(!"Heap Create failed!!!\n");
		return FALSE;
	}

	stData.m_dwThreadID = ::GetCurrentThreadId();
	stData.m_llHeapSize = llHeapSize;

	// map 에 추가한다.
	m_MapHeap.insert(pair<DWORD, ApHeapData>(stData.m_dwThreadID, stData));
	
	return TRUE;
}

LPVOID ApHeap::Alloc(SIZE_T llSize)
{
#ifdef APHEAP_PROFILE
	std::string title = _T("ApHeap::Alloc");
	AuStopWatch csStopWatch(title);
#endif

	DWORD dwThreadID = ::GetCurrentThreadId();

	HeapIter iter = m_MapHeap.find(dwThreadID);
	if(iter == m_MapHeap.end())
	{
		if(m_eMode == APHEAP_MODE_AUTO_CREATE_ON_ALLOC)
		{
			if(Create(APHEAP_DEFAULT_SIZE) == FALSE)
				return NULL;

			iter = m_MapHeap.find(dwThreadID);
			if(iter == m_MapHeap.end())
				return NULL;
		}
		else
			return NULL;
	}

	LPVOID lpMem = ::HeapAlloc(iter->second.m_hHeap, HEAP_ZERO_MEMORY, llSize);
	if(!lpMem)
		return NULL;

	++iter->second.m_llAllocCount;
	iter->second.m_llAllocBytes += llSize;

	return lpMem;
}

BOOL ApHeap::Free(LPVOID lpMem)
{
#ifdef APHEAP_PROFILE
	std::string title = _T("ApHeap::Free");
	AuStopWatch csStopWatch(title);
#endif

	DWORD dwThreadID = ::GetCurrentThreadId();

	HeapIter iter = m_MapHeap.find(dwThreadID);
	if(iter == m_MapHeap.end())
		return FALSE;

	BOOL bResult = ::HeapFree(iter->second.m_hHeap, 0, lpMem);
	return bResult;
}

BOOL ApHeap::Destroy()
{
	DWORD dwThreadID = ::GetCurrentThreadId();

	HeapIter iter = m_MapHeap.find(dwThreadID);
	if(iter == m_MapHeap.end())
		return FALSE;

	BOOL bResult = ::HeapDestroy(iter->second.m_hHeap);

	// map 에서 지워준다.
	m_MapHeap.erase(dwThreadID);
	
	return bResult;
}

BOOL ApHeap::DestroyAll()
{
	HeapIter iter = m_MapHeap.begin();
	while(iter != m_MapHeap.end())
	{
		::HeapDestroy(iter->second.m_hHeap);
		++iter;
	}

	m_MapHeap.clear();

	return TRUE;
}

void ApHeap::Report()
{
	HeapIter iter = m_MapHeap.begin();
	while(iter != m_MapHeap.end())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "TID:%d\tHeapSize:%I64d\tAllocCount:%I64d\tAllocByes:%I64d\n",
					iter->second.m_dwThreadID,
					iter->second.m_llHeapSize,
					iter->second.m_llAllocCount,
					iter->second.m_llAllocBytes);
		AuLogFile_s(APHEAP_REPORT_FILE_NAME, strCharBuff);

		++iter;
	}
}
