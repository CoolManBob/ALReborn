// ApLockManager.cpp: implementation of the ApLockManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ApLockManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <algorithm>

using namespace std;

#define DISTANCE(_Ptr) (_Ptr - m_LockTable)
ApLockManager* ApLockManager::pThis = NULL;

ApLockManager::ApLockManager()
{
	ZeroMemory(m_LockTable, sizeof(stLockElement) * MAX_LOCKTABLE_COUNT);
	ZeroMemory(m_RecursionBackupCount, sizeof(INT32) * MAX_LOCKTABLE_COUNT);
	m_lLockTableCount = 0;
	m_lRecursionCount = 0;
}

ApLockManager::~ApLockManager()
{
//	if (pThis) delete pThis;
}

// return singleton instance
ApLockManager* ApLockManager::Instance()
{
	if (!pThis)
	{
		pThis = new ApLockManager;
	}

	return pThis;
}

void ApLockManager::DeleteInstance()
{
	if( pThis )
	{
		delete pThis;
		//@{ 2006/10/18 burumal
		pThis = NULL;
		//@}
	}
}


// 현재 lock table에 있는 동기화 객체의 카운트
INT32 ApLockManager::GetCount()
{
	return (INT32)m_lLockTableCount;
}

BOOL ApLockManager::Lock(stLockElement *pLockElement)
{
	EnterCriticalSection(&pLockElement->pMutex->m_csCriticalSection);
	++pLockElement->lCount;

	return TRUE;
}

BOOL ApLockManager::Unlock(stLockElement *pLockElement)
{
	LeaveCriticalSection(&pLockElement->pMutex->m_csCriticalSection);
	--pLockElement->lCount;

	return TRUE;	
}

// Insert를 이용하여 삽입후 Lock을 걸어준다.
BOOL ApLockManager::InsertLock(ApMutualEx* pValue)
{	
	//@{ 2007/01/02 burumal
	if ( !pValue )
		return FALSE;
	//@}

	stLockElement *pLockElement = lower_bound(m_LockTable, m_LockTable + m_lLockTableCount, pValue, FtrCompare());
	BOOL bResult = FALSE;

	if (NULL != (PVOID)pLockElement->pMutex)	
	{
		// pValue와 같거나 더 큰 값의 pValue가 LockTable에 존재한다.
		if ((PVOID)pLockElement->pMutex == (PVOID)pValue)
		{
			// 정확히 같은 ApMutualEx가 존재하는 경우
			bResult = Lock(pLockElement);	// 이미 Lock 되어 있는 상태라면 SequenceLock/Unlock이 필요하지 않다. 
		}
		else
		{
			// arycoat 2008.10.21
			if( m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1 )
			{
				//OutputDebugStr("m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1");
				return FALSE;
			}

			// pValue보다 큰 값의 ApMutualEx가 존재하는 경우 -> Sequence Lock/Unlock이 필요하다.
			size_t lDistance = DISTANCE(pLockElement);
			size_t lBytes = (m_lLockTableCount - lDistance);
			lBytes *= sizeof(ApLockManager::stLockElement);
			memmove(pLockElement + 1, pLockElement, lBytes);
			m_LockTable[lDistance] = stLockElement(pValue);
			++m_lLockTableCount;

			SequenceUnlock(pLockElement + 1);
			bResult = Lock(pLockElement);
			SequenceLock(pLockElement + 1);
		}
	}
	else
	{
		// arycoat 2008.10.21
		if( m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1 )
		{
			//OutputDebugStr("m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1");
			return FALSE;
		}

		// LockTable에 존재하지 않는 ApMutualEx이므로 마지막에 추가만한다.
		m_LockTable[m_lLockTableCount] = stLockElement(pValue);
		bResult = Lock(&m_LockTable[m_lLockTableCount]);
		++m_lLockTableCount;
	}

	return bResult;
}

// 해당 요소를 release를 하고 locktable에서 삭제한다.
void ApLockManager::RemoveUnlock(ApMutualEx* pValue)
{
	//@{ 2007/01/02 burumal
	if ( !pValue )
		return;
	//@}

	stLockElement *pLockElement = find(m_LockTable, m_LockTable + m_lLockTableCount, pValue);
	if (NULL == (PVOID)pLockElement->pMutex) return;

	// ASSERT가 발생하는 경우는 release를 했는데 또다시 release를 한 경우가 많다.
//	ASSERT(LockTable.end() != iter && "Remove를 위한 Mutex 객체가 잘못되었습니다.");

	Unlock(pLockElement);
	RemoveCheck(pLockElement);
}

// ApMutualEx가 삭제될때 LockManager에 있으면 삭제하고 종료
void ApLockManager::SafeRemoveUnlock(ApMutualEx* pValue)
{
	//@{ 2007/01/02 burumal
	if ( !pValue )
		return;
	//@}

	stLockElement *pLockElement = find(m_LockTable, m_LockTable + m_lLockTableCount, pValue);
	if (NULL == (PVOID)pLockElement->pMutex) return;

	Unlock(pLockElement);
	RemoveCheck(pLockElement);
}

// RecursionCount가 0인 ApMutualEx만 LockTable에서 삭제한다.
void ApLockManager::RemoveCheck(stLockElement *pLockElement)
{
	if (0 >= pLockElement->lCount)
	{
		--m_lLockTableCount;
		size_t lDistance = DISTANCE(pLockElement);
		size_t lBytes = (m_lLockTableCount - lDistance);
		lBytes *= sizeof(ApLockManager::stLockElement);		
		memmove(pLockElement, pLockElement + 1, lBytes);
		memset(m_LockTable + m_lLockTableCount, 0x00, sizeof(ApLockManager::stLockElement));
	}
}

// pValue에서 뒤에 있는 모든 객체를 Lock 해준다. - multiple에서만 사용
void ApLockManager::SequenceLockforMultiple(PVOID pValue)
{
//	PtrIter iter = lower_bound(LockTable.begin(), LockTable.end(), pValue, FtrCompare());
//	for_each(iter, LockTable.end(), FtrLock());
}

// pValue에서 마지막까지 release 한다.
void ApLockManager::SequenceUnlockforMultiple(PVOID pValue)
{
//	PtrIter iter = lower_bound(LockTable.begin(), LockTable.end(), pValue, FtrCompare());
//	for_each(iter, LockTable.end(), FtrUnlock());
}

// iter에서 뒤에 있는 모든 객체를 RecursionCount만큼 lock을 해준다.
void ApLockManager::SequenceLock(stLockElement *pLockElement)
{
	size_t lDistance = DISTANCE(pLockElement);

	for (size_t i = lDistance; i < m_lLockTableCount; ++i)
	{
		//m_LockTable[i].lCount = m_RecursionBackupCount[i - lDistance];
		RecursionLock(&m_LockTable[i], m_RecursionBackupCount[i - lDistance]);
	}
}

// iter에서 마지막까지 release 한다.
void ApLockManager::SequenceUnlock(stLockElement *pLockElement)
{
	// LockCount를 Backup 한다.
	size_t lDistance = DISTANCE(pLockElement);
	for (size_t i = lDistance; i < m_lLockTableCount; ++i)
	{
		m_RecursionBackupCount[i - lDistance] = m_LockTable[i].lCount;
		++m_lRecursionCount;

		// LockCount만큼 unlock 한다.
		RecursionUnlock(&m_LockTable[i]);
	}
}

// 현재 unlock을 안한 객체가 있는지 확인한다.
BOOL ApLockManager::IsExistData()
{
//	if (m_lLockTableCount > 0)
//	{
//		printf("\nLockTable Count : %d", m_lLockTableCount);
//	}

	return (BOOL)m_lLockTableCount;
}

// release를 하지 않은 객체를 모두다 release해준다.
void ApLockManager::AllUnlock()
{
	INT32 lCount = (INT32)m_lLockTableCount;
	for (int i = lCount; i > 0; --i)
	{
		RecursionUnlock(&m_LockTable[i - 1]);
		RemoveCheck(&m_LockTable[i - 1]);
	}
//	for_each(LockTable.begin(), LockTable.end(), FtrRecursionUnlock());
//	LockTable.clear();
}

void ApLockManager::RecursionLock(stLockElement *pLockElement, INT32 lCount)
{
	for (int i = 0; i < lCount; ++i)
	{
		EnterCriticalSection(&pLockElement->pMutex->m_csCriticalSection);	
		++pLockElement->lCount;
	}
}

void ApLockManager::RecursionUnlock(stLockElement *pLockElement)
{
	INT32 lCount = pLockElement->lCount;
	for (int i = 0; i < lCount; ++i)
	{
		LeaveCriticalSection(&pLockElement->pMutex->m_csCriticalSection);
		pLockElement->lCount--;
	}
}
