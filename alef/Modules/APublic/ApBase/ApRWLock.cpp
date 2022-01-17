// ApRWLock.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2007. 04. 23.

#include "ApRWLock.h"

//#define DONT_USE_RW_LOCK		1

ApRWLock::ApRWLock() : m_hNobodyIsReading(NULL),
						m_hWritingMutex(NULL),
						m_nReaders(0),
						m_bInit(0)
{
	Initialize();
}

ApRWLock::ApRWLock(const ApRWLock& param) : m_hNobodyIsReading(NULL),
											m_hWritingMutex(NULL),
											m_nReaders(0),
											m_bInit(0)
{
	// Copy Constructor 
	// 이놈도 역시 뮤텍스를 초기화 한다.
	// 이부분이 일어나는 곳이 ApWorldSector.cpp CreateDimension() 에서만
	// 일어나야 한다.

	// 처음 vector에 삽입할때 요게 불리면서
	// 초기화가 다시돼고 원본은 파기된다.
	Initialize();
}

void ApRWLock::Initialize()
{
#ifdef DONT_USE_RW_LOCK

	if(m_bInit)
		return;

	InitializeCriticalSection(&m_csReading);
	m_bInit = TRUE;
	
#else

	if( m_hNobodyIsReading || m_hWritingMutex ) return;

	// Reading Event
	m_hNobodyIsReading = CreateEvent(NULL, TRUE, true, NULL);

	// Writing Mutex
	m_hWritingMutex = CreateMutex(NULL, false, NULL);

	ASSERT(m_hNobodyIsReading && m_hWritingMutex);

	// to Manage m_nReaders
	InitializeCriticalSection(&m_csReading);
	
	m_nReaders = 0;

	m_bInit = TRUE;

#endif
}

ApRWLock::~ApRWLock()
{
#ifdef DONT_USE_RW_LOCK

	DeleteCriticalSection(&m_csReading);
	m_bInit = FALSE;

#else

	if(m_hNobodyIsReading || m_hWritingMutex )
	{
		CloseHandle(m_hNobodyIsReading);
		CloseHandle(m_hWritingMutex);
		DeleteCriticalSection(&m_csReading);

		ASSERT(m_nReaders == 0);

		m_bInit = FALSE;
	}

#endif
}

bool ApRWLock::LockReader(DWORD dwTimeOut)
{
#ifdef DONT_USE_RW_LOCK

	::EnterCriticalSection(&m_csReading);
	return true;

#else

	// Writer Lock 대기한다.
	if(WaitForSingleObject(m_hWritingMutex, dwTimeOut) != WAIT_OBJECT_0)
		return false;

	// Reader Lock Count 를 위한 CS
	::EnterCriticalSection(&m_csReading);
	{
		++m_nReaders;
		if(m_nReaders == 1)
		{
			// Reader Lock 걸렸다고 알려준다.
			ResetEvent(m_hNobodyIsReading);
		}
	}
	::LeaveCriticalSection(&m_csReading);

	// Writer Lock 풀어준다.
	ReleaseMutex(m_hWritingMutex);
	
	return true;

#endif
}

void ApRWLock::UnlockReader()
{
#ifdef DONT_USE_RW_LOCK

	::LeaveCriticalSection(&m_csReading);

#else

	::EnterCriticalSection(&m_csReading);
	{
		--m_nReaders;
		ASSERT(m_nReaders >= 0);
		if(m_nReaders == 0)
		{
			// Reader Lock 풀어준다.
			SetEvent(m_hNobodyIsReading);
		}
	}
	::LeaveCriticalSection(&m_csReading);

#endif
}

bool ApRWLock::LockWriter(DWORD dwTimeOut)
{
#ifdef DONT_USE_RW_LOCK

	::EnterCriticalSection(&m_csReading);
	return true;

#else

	// Writer Lock 체크
	if(WaitForSingleObject(m_hWritingMutex, dwTimeOut) != WAIT_OBJECT_0)
		return false;

	// Reader Lock 도 체크
	if(WaitForSingleObject(m_hNobodyIsReading, dwTimeOut) != WAIT_OBJECT_0)
	{
		// 이럴 땐 문제가 좀 있으니 Writer Lock 풀어준다.
		ReleaseMutex(m_hWritingMutex);
		return false;
	}
	
	return true;

#endif
}

void ApRWLock::UnlockWriter()
{
#ifdef DONT_USE_RW_LOCK

	::LeaveCriticalSection(&m_csReading);

#else

	// Writer Lock 풀어준다.
	ReleaseMutex(m_hWritingMutex);

#endif
}


