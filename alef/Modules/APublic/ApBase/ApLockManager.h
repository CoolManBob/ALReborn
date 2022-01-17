/*
	Notices: Copyright (c) NHN Studio 2004
	Created by: Bryan Jeong (2004/03/24)
*/

/*
	수정 내역
	1. 2004.5.10 (정남영)
		- ApLockManager에 중복 Lock이 가능하도록 수정
		- ApMultipleLock 삭제
*/

// ApLockManager.h: interface for the ApLockManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APLOCKMANAGER_H__030AC40A_EE83_4C11_8942_2BCD27809B51__INCLUDED_)
#define AFX_APLOCKMANAGER_H__030AC40A_EE83_4C11_8942_2BCD27809B51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

const INT32 MAX_LOCKTABLE_COUNT = 50;

class ApMultipleLock;
class ApLockManager
{
private:
	struct stLockElement
	{
		ApMutualEx*	pMutex;
		LONG		lCount;

		stLockElement(ApMutualEx* Mutex = NULL) : pMutex(Mutex), lCount(0) {}

		bool operator == (const stLockElement& lhs)
		{
			return ((PVOID)lhs.pMutex == (PVOID)pMutex);
		}
	};

	class FtrCompare
	{
	public:
		bool operator () (const stLockElement& lhs, const stLockElement& rhs)
		{
			return ((PVOID)lhs.pMutex < (PVOID)rhs.pMutex);
		}
	};

private:
	stLockElement	m_LockTable[MAX_LOCKTABLE_COUNT];
	size_t			m_lLockTableCount;

	INT32			m_RecursionBackupCount[MAX_LOCKTABLE_COUNT];
	INT32			m_lRecursionCount;
//	friend class ApMultipleLock;

private:
	ApLockManager();	// singleton을 위해 private에 위치

	BOOL Lock(stLockElement *pLockElement);
	BOOL Unlock(stLockElement *pLockElement);

	void SequenceLock(stLockElement *pLockElement);
	void SequenceUnlock(stLockElement *pLockElement);

	void RecursionLock(stLockElement *pLockElement, INT32 lCount);
	void RecursionUnlock(stLockElement *pLockElement);

	void SequenceUnlockforMultiple(PVOID pValue);
	void SequenceLockforMultiple(PVOID pValue);

	void RemoveCheck(stLockElement *pLockElement);
//	INT32 Insert(ApMutualEx* pValue);

public:
	// TLS singleton
	__declspec (thread) static ApLockManager* pThis;

public:
	virtual ~ApLockManager();
	static ApLockManager* Instance();
	static void DeleteInstance();

	INT32 GetCount();

	BOOL InsertLock(ApMutualEx* pValue);
	void RemoveUnlock(ApMutualEx* pValue);
	void SafeRemoveUnlock(ApMutualEx* pValue);

	BOOL IsExistData();
	void AllUnlock();
};
/*
// 중복 Lock 기능을 추가하면서 ApMultipleLock도 수정되어야 한다.
// 하지만 중복 Lock 기능을 지원하려면 추가적인 오버헤드가 있어서
// ApMultipleLock의 효용성에 문제가 발생하여 우선은 제외하기로 결정
// 2004.5.10 정남영
class ApMultipleLock 
{
private:
	// 최소값 기법과 정렬 기법 모두다 사용 가능한 fuctor
	class FtrSort
	{
	public:
		bool operator () (const ApMultipleLockArray& lhs, const ApMultipleLockArray& rhs)
		{
			return ((PVOID)&lhs->m_Mutex) < ((PVOID)&rhs->m_Mutex);
		}
	};

public:
	ApMultipleLock( ApMultipleLockArray* pArray, INT32 lCount )
	{
		// 정렬 기법
//		sort(pArray, pArray + lCount, FtrSort());
//		PVOID Min = (PVOID) &pArray[0]->m_Mutex;

		// 최소값 기법
		ApMultipleLockArray *pMin = std::min_element(pArray, pArray + lCount, FtrSort());
		PVOID Min = (PVOID)&((*pMin)->m_Mutex);

		// Min뒤에 있는 동기화 객체들을 Release 시킨다.
		ApLockManager::Instance()->SequenceUnlockforMultiple(Min);

		// LockManager에 삽입
		for (INT32 i = 0; i < lCount; ++i)
			ApLockManager::Instance()->Insert( (PVOID) &pArray[i]->m_Mutex );

		// Min뒤에 있는 동기화 객체들을 Lock 시킨다.
		ApLockManager::Instance()->SequenceLockforMultiple(Min);
	}
};
*/
#endif // !defined(AFX_APLOCKMANAGER_H__030AC40A_EE83_4C11_8942_2BCD27809B51__INCLUDED_)
