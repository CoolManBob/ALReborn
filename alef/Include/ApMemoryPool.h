/*
	Notices: Copyright (c) NHN Studio 2002
	Created by: Bryan Jeong (2003/11/21)
*/

#if !defined(__APMEMORYPOOL_H__)
#define __APMEMORYPOOL_H__

#include "ApMemoryManager.h"

/**************************************************************************
ApMemory와 같이 클래스로 선언하여 컴파일 타임에서 사이즈를 확인할수 없는 경우
사용할수 있다.

  Initialize를 호출하여 lDataSize와 lArraySize를 호출하여 사용한다.
  Initialize를 하지 앟으면 Alloc, Free를 진행할수 없다.
**************************************************************************/

class ApMemoryPool
{
private:
	ApCriticalSection	m_Mutex;
	INT32				m_lTypeIndex;
	
public:
	ApMemoryPool() : m_lTypeIndex(-1) {}
	virtual ~ApMemoryPool() {}

	VOID Initialize(INT32 lDataSize, INT32 lArraySize, CHAR* pTypeName = NULL)
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return;
		m_lTypeIndex = ApMemoryManager::GetInstance().AddType(lDataSize, lArraySize, pTypeName);
	}
	
	PVOID Alloc()
	{
		if (-1 == m_lTypeIndex) return NULL;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return NULL;
		return ApMemoryManager::GetInstance().NewObject(m_lTypeIndex);
	}

	VOID Free(PVOID pObject)
	{
		if (-1 == m_lTypeIndex) return;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return;
		ApMemoryManager::GetInstance().DeleteObject(m_lTypeIndex, pObject);
	}

	INT32 GetAllocatedCount()
	{
		if (-1 == m_lTypeIndex) return -1;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return 0;
		return ApMemoryManager::GetInstance().GetAllocatedCount(m_lTypeIndex);
	}

	BOOL IsEmptyPool()
	{
		if (-1 == m_lTypeIndex) return FALSE;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;

		return (ApMemoryManager::GetInstance().m_lAllocCount[m_lTypeIndex] >= ApMemoryManager::GetInstance().m_lArraySize[m_lTypeIndex]);
	}

	INT32 GetRemainCount()
	{
		if (-1 == m_lTypeIndex) return FALSE;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return 0;

		return ApMemoryManager::GetInstance().m_lArraySize[m_lTypeIndex] - ApMemoryManager::GetInstance().m_lAllocCount[m_lTypeIndex];
	}
	
	inline INT32 GetTypeIndex()
	{
		return m_lTypeIndex;
	}
};

#endif	// __APMEMORYPOOL_H__