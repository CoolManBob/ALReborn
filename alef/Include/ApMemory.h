/*
	Notices: Copyright (c) NHN Studio 2002
	Created by: Bryan Jeong (2003/11/21)
*/

// ApMemory.h: interface for the ApMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMEMORY_H__D97566A8_2F45_4D33_AF18_2BBBE224005E__INCLUDED_)
#define AFX_APMEMORY_H__D97566A8_2F45_4D33_AF18_2BBBE224005E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApMemoryAllocator.h"

/**************************************************************************
TObject		: 메모리 풀링을 할 클래스
lArraySize	: 풀링을 할 사이즈 (최대 갯수)

  class TestClass : public ApMemory<TestClass, 1000>
	...

이런 형태로 선언만 하게 되면 new, delete 연산자를 사용할때 메모리 풀링을 
자동으로 실행한다.
**************************************************************************/
template <class TObject, INT32 lArraySize = DEFAULT_ARRAY_SIZE>
class ApMemory  
{
private:
	static ApMemoryAllocator<TObject, lArraySize> m_csAllocator;

	static PVOID operator new [] (size_t lSize);
	static PVOID operator new [] (size_t lSize, const CHAR* szFileName, INT32 lLine);
	static VOID operator delete [] (PVOID pObject);
	static VOID operator delete [] (PVOID pObject, const CHAR* szFileName, INT32 lLine);
	
public:
	ApMemory() {}
	virtual ~ApMemory() {}

	// 메모리 할당
	static PVOID operator new(size_t lSize)
	{
		if (lSize == sizeof(TObject)) 
			return m_csAllocator.AllocatorNewMemory(lSize);
		else
			return NULL;
	}

	// for MFC
	static PVOID operator new(size_t lSize, const CHAR* szFileName, INT32 lLine)
	{
		if (lSize == sizeof(TObject)) 
			return m_csAllocator.AllocatorNewMemory(lSize);
		else
			return NULL;
	}

	// 메모리 삭제
	static VOID operator delete(PVOID pObject)
	{
		m_csAllocator.AllocatorDeleteMemory(pObject);
	}

	// for MFC
	static VOID operator delete(PVOID pObject, const CHAR* szFileName, INT32 lLine)
	{
		m_csAllocator.AllocatorDeleteMemory(pObject);
	}

	static BOOL SetEnable(BOOL bEnable)
	{
		return m_csAllocator.SetEnable(bEnable);
	}

	static BOOL GetEnable()
	{
		return m_csAllocator.GetEnable();
	}
};
template <class TObject, INT32 lArraySize> 
	ApMemoryAllocator<TObject, lArraySize> ApMemory<TObject, lArraySize>::m_csAllocator;


class ApGuardArray
{
private:
	INT32 m_lPageCount;
	INT32 m_lSize;
	BYTE* m_pPointer;

public:
	ApGuardArray(INT32 lSize) : m_lPageCount(0), m_pPointer(NULL), m_lSize(0)
	{
		m_lSize = lSize;
		m_lPageCount = (lSize / 4096) + 2;
		m_pPointer = (BYTE*)VirtualAlloc(NULL, 4096 * m_lPageCount, MEM_RESERVE, PAGE_READWRITE | PAGE_GUARD);
		VirtualAlloc(m_pPointer, 4096 * (m_lPageCount - 1), MEM_COMMIT, PAGE_READWRITE);
	}

	PVOID GetValidPointer()
	{
		return m_pPointer + (4096 * (m_lPageCount - 1)) - m_lSize;
	}

	~ApGuardArray()
	{
		if (m_pPointer)
		{
			VirtualFree(m_pPointer, 0, MEM_RELEASE);
		}
	}
};


#endif // !defined(AFX_APMEMORY_H__D97566A8_2F45_4D33_AF18_2BBBE224005E__INCLUDED_)
