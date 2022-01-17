#pragma once

#include "ApMemoryManager.h"
#include "MagDebug.h"

template <class TObject, INT32 lArraySize = DEFAULT_ARRAY_SIZE>
class ApMemoryAllocator
{
private:
	static INT32		m_lTypeIndex;
	ApCriticalSection	m_Mutex;

public:
	ApMemoryAllocator()
	{
	}
	virtual ~ApMemoryAllocator() {}

	PVOID AllocatorNewMemory(SIZE_T lSize)
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return NULL;

		if (-1 == m_lTypeIndex)
		{
#ifdef _CPPRTTI
			m_lTypeIndex = ApMemoryManager::GetInstance().AddType(lSize, lArraySize, typeid(TObject).name());
#else
			m_lTypeIndex = ApMemoryManager::GetInstance().AddType(lSize, lArraySize);
#endif
		}

		if(m_lTypeIndex == -1)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "ApMemoryAllocator::AllocatorNewMemory() Fail : Size = %d, ArraySize = %d\n", lSize, lArraySize);
			AuLogFile_s("LOG\\AllocatorNewMemoryError.log", strCharBuff);
			return NULL;
		}

		return ApMemoryManager::GetInstance().NewObject(m_lTypeIndex);
	}

	VOID AllocatorDeleteMemory(PVOID pObject)
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return;
		ApMemoryManager::GetInstance().DeleteObject(m_lTypeIndex, pObject);
	}

	INT32 GetAllocatedCount()
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return 0;
		return ApMemoryManager::GetInstance().GetAllocatedCount(m_lTypeIndex);
	}

	BOOL SetEnable(BOOL bEnable)
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;
		return ApMemoryManager::GetInstance().SetEnable(m_lTypeIndex, bEnable);
	}

	BOOL GetEnable()
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;
		return ApMemoryManager::GetInstance().GetEnable(m_lTypeIndex);
	}
};

template <class TObject, INT32 lArraySize> 
	INT32 ApMemoryAllocator<TObject, lArraySize>::m_lTypeIndex = -1;
