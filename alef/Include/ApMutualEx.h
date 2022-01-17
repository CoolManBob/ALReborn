/******************************************************************************
Module:  ApMutualEx.h
Notices: Copyright (c) 2002 netong (Copyright (c) 2000 Jeffrey Richter)
Purpose: 
Last Update: 2002. 04. 02
******************************************************************************/

/*
	Notices: Copyright (c) NHN Studio 2004
	Modified by: Bryan Jeong (2004/03/24)
*/

#if !defined(__APMUTUALEX_H__)
#define __APMUTUALEX_H__

#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
/******************************************************************************
Module name: Optex.h
Written by:  Jeffrey Richter
******************************************************************************/

#include "assert.h"

#pragma once

///////////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )

class ApCriticalSection
{
private:
	CRITICAL_SECTION	m_CriticalSection;

public:
	BOOL				m_bInit;

public:
	ApCriticalSection()
	{
		m_bInit = TRUE;
		InitializeCriticalSection(&m_CriticalSection);
	}

	~ApCriticalSection()
	{
		if( m_bInit )
			DeleteCriticalSection(&m_CriticalSection);
	}

	void Init()
	{
		if (m_bInit) return;

		m_bInit = TRUE;
		InitializeCriticalSection(&m_CriticalSection);
	}

	void Destroy()
	{
		if( m_bInit )
		{
			DeleteCriticalSection(&m_CriticalSection);
			m_bInit = FALSE;
		}
	}

	inline void Lock()
	{
		if( m_bInit )
			EnterCriticalSection(&m_CriticalSection);

		// test code gemani 
		//DWORD	cur_thread_id = GetCurrentThreadId();
		//if(cur_thread_id == g_ulMainThreadID)
		//{
		//	DWORD	start = timeGetTime();
		//	EnterCriticalSection(&m_CriticalSection);			
		//	DWORD	end = timeGetTime();

		//	if(end - start > 3)
		//	{
		//		start = 5;
		//	}
		//}
		//else
		//{
		//	EnterCriticalSection(&m_CriticalSection);
		//}
	}

	inline void Unlock()
	{
		if( m_bInit )
			LeaveCriticalSection(&m_CriticalSection);
	}

private: // 복사생성 및 대입연산 금지 - 2011.02.11 kdi
	ApCriticalSection( ApCriticalSection const & other );
	void operator=( ApCriticalSection const & other );
};

class ApMutualEx {
public:
	ApMutualEx();
	~ApMutualEx();

	VOID Init(PVOID pvParent = NULL);
	VOID Destroy();

	BOOL WLock();
	BOOL RLock();
	BOOL RemoveLock();
	VOID ResetRemoveLock();
	BOOL Release(BOOL bForce = FALSE);

	BOOL SafeRelease();

	BOOL SetNotUseLockManager();

	// 2006.05.04. steeple - 그냥 ThreadID 를 리턴해야함. 
	// 2010.07.20. supertj 
	// 우리프로젝트에서 해당 CS가 이미 소멸자가 불리어 사라졌는데 static 객체들이 멤버로 가지고 있는 것들이 있다.
	// 이때문에 해당 CS를 체크하는것을 넣을수 밖에 없네 쩝;;
	
	INT_PTR GetOwningThreadID() 
	{
		if( !IsBadReadPtr( (const VOID*)&m_csCriticalSection, sizeof(CRITICAL_SECTION) ) )
			return (INT_PTR)m_csCriticalSection.OwningThread; 
		else
			return NULL;
	};

	BOOL IsInit() { return m_bInit; };
	BOOL IsRemoveLock() { return m_bRemoveLock; };

private:
	CRITICAL_SECTION	m_csCriticalSection;

	BOOL	m_bInit;
	PVOID	m_pvParent;
	BOOL	m_bRemoveLock;

	BOOL	m_bNotUseLockManager;

	// ApLockManager에서 사용
	friend class ApLockManager;
	friend class ApLockTable;
	
	friend class FtrLock;
	friend class FtrUnlock;
	friend class FtrRecursionLock;
	friend class FtrRecursionUnlock;
};

// 2003.10.6 
// Jeong Nam-young
class AuAutoLock
{
private:
	ApCriticalSection*	m_pcsCriticalSection;
	ApMutualEx*			m_pcsMutualEx;
	BOOL				m_bAutoLock;
	BOOL				m_bMutualEx;

	BOOL				m_bIsLockResult;

public:
	AuAutoLock(ApCriticalSection& csCriticalSection, BOOL bAutoLock = TRUE) 
		: m_pcsCriticalSection(&csCriticalSection), m_bAutoLock(bAutoLock), m_bMutualEx(FALSE)
	{if(m_pcsCriticalSection) m_pcsCriticalSection->Lock(); m_bIsLockResult = TRUE;}

	AuAutoLock(ApMutualEx& csMutualEx, BOOL bAutoLock = TRUE)
		: m_pcsMutualEx(&csMutualEx), m_bAutoLock(bAutoLock), m_bMutualEx(TRUE)
	{if(m_pcsMutualEx) m_bIsLockResult = m_pcsMutualEx->WLock();}

	AuAutoLock() : m_pcsCriticalSection(NULL), m_pcsMutualEx(NULL), m_bAutoLock(FALSE), m_bMutualEx(FALSE)
	{}

	void SetMutualEx(ApMutualEx& csMutualEx, BOOL bAutoLock = TRUE)
	{m_pcsMutualEx=&csMutualEx; m_bAutoLock=bAutoLock; m_bMutualEx=TRUE;}

	void Lock() {if(m_pcsMutualEx) m_bIsLockResult = m_pcsMutualEx->WLock();}

	BOOL Result() {	assert(m_bIsLockResult); return m_bIsLockResult;}

	~AuAutoLock() 
	{
		if (m_bMutualEx)
		{
			if(m_pcsMutualEx)
				m_pcsMutualEx->Release();
		}
		else
		{
			if (m_pcsCriticalSection)
				m_pcsCriticalSection->Unlock();
		}
	}
};

// 2003. 11. 19
// 정남영
// ApArray가 ApAdmin에서는 Lock이 필요하지 않고 ApModule에서는 Lock이 필요함에 따라
// AutoLock을 스위칭 할수 있는 방법이 필요
// 그래서 AuAutoLock2가 탄생!! 
//
// --> VC++ 6.0의 컴파일러가 Template의 Select기법을 지원하지 않음에 따라
// 아래와 같은 Int2Type을 이용하여 해결

const INT32 DONT_USE_AUTOLOCK = 0;
const INT32 USE_AUTOLOCK = 1;

template <INT32 TUseAutoLock = USE_AUTOLOCK>
struct AuAutoLock2
{
private:
	template <INT32 v>
	struct Int2Type
	{
		enum { value = v };
	};
	
private:
    ApMutualEx &m_csMutalEx;

private:
	void Lock(Int2Type<USE_AUTOLOCK>) { m_csMutalEx.RLock();}
	void Lock(Int2Type<DONT_USE_AUTOLOCK>) {}

	void UnLock(Int2Type<USE_AUTOLOCK>) {m_csMutalEx.Release();}
	void UnLock(Int2Type<DONT_USE_AUTOLOCK>) {}

public:
	AuAutoLock2(ApMutualEx& csMutalEx) : m_csMutalEx(csMutalEx) { Lock(Int2Type<TUseAutoLock>()); }
	~AuAutoLock2() { UnLock(Int2Type<TUseAutoLock>());}
};

///////////////////////////////////////////////////////////////////////////////
#endif