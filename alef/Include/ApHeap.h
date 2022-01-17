// ApHeap.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2008.05.05.
//
// 아크로드 프로젝트에 특화될 수 있게끔 매니저 역할도 한다.

#ifndef _APHEAP_H_
#define _APHEAP_H_

#include "ApBase.h"
#include <map>

const int APHEAP_DEFAULT_SIZE = 128 * 128 * 8 * 5;		// 대략 655Kb

struct ApHeapData
{
	DWORD m_dwThreadID;

	HANDLE m_hHeap;

	// For Report
	SIZE_T m_llHeapSize;
	SIZE_T m_llAllocCount;
	SIZE_T m_llAllocBytes;

	ApHeapData() : m_dwThreadID(0), m_hHeap(0), m_llHeapSize(0), m_llAllocCount(0), m_llAllocBytes(0)
	{}
};

class ApHeap
{
public:
	enum EunmApHeapMode
	{
		APHEAP_MODE_AUTO_CREATE_ON_ALLOC = 0,
		APHEAP_MODE_DONT_USE_AUTO_CREATION,
	};

private:
	typedef std::map<DWORD, ApHeapData>				HeapMap;		// ThreadID, ApHeapData
	typedef std::map<DWORD, ApHeapData>::iterator	HeapIter;

	HeapMap	m_MapHeap;
	EunmApHeapMode m_eMode;

public:
	ApHeap(EunmApHeapMode eMode = APHEAP_MODE_AUTO_CREATE_ON_ALLOC);
	virtual ~ApHeap();

	BOOL Create(SIZE_T llHeapSize);

	LPVOID Alloc(SIZE_T llSize);
	BOOL Free(LPVOID lpMem);

	BOOL Destroy();
	BOOL DestroyAll();

	void Report();
};

#endif//_APHEAP_H_
