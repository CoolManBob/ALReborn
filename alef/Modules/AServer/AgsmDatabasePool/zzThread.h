/*========================================================================

	zzThread.h

========================================================================*/

#ifndef _ZZ_LIBRARY_THREAD_H_
	#define _ZZ_LIBRARY_THREAD_H_

#include "ApBase.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define		ZZ_NUM_MAXIMUM_WAIT_OBJECT		200

/************************************************/
/*		The Definition of Helper functions		*/
/************************************************/
//
BOOL	zzIsValidHandle(HANDLE hHandle);
BOOL	zzWaitSucceeded(DWORD dwWaitResult, DWORD dwHandleCount);
BOOL	zzWaitAbandoned(DWORD dwWaitResult, DWORD dwHandleCount);
DWORD	zzWaitSucceededIndex(DWORD dwWaitResult);
DWORD	zzWaitAbandonedIndex(DWORD dwWaitResult);
BOOL	zzWaitTimedOut(DWORD dwWaitResult);
BOOL	zzWaitFailed(DWORD dwWaitResult);

/********************************************************/
/*		The Definition of the CriticalSection class		*/
/********************************************************/
//
class zzCriticalSection
	{
	protected:
		CRITICAL_SECTION	m_CS;

	public:
		zzCriticalSection();
		virtual ~zzCriticalSection();

		void	Enter();
		void	Leave();

		CRITICAL_SECTION* GetCS();
	};


/************************************************/
/*		The Definition of the Kernel class		*/
/************************************************/
//
class zzKernel
	{
	protected:
		HANDLE	m_hHandle;
		DWORD	m_dwStatus;

	protected:
		zzKernel();

	public:
		virtual ~zzKernel();

		DWORD	Status() const;
		DWORD	Wait(DWORD dwMilliseconds);
		DWORD	WaitForTwo(zzKernel *rKernel, BOOL fWaitAll, DWORD dwMilliseconds);
		DWORD	WaitForTwo(HANDLE hKernel, BOOL fWaitAll, DWORD dwMilliseconds);
		HANDLE	GetHandle() const;
	};

/************************************************/
/*		The Definition of the Mutex class		*/
/************************************************/
//
class zzMutex : public zzKernel
	{
	public:
		// ... Create
		zzMutex(BOOL fInitialOwner = FALSE, LPCTSTR lpszName = NULL,
			    LPSECURITY_ATTRIBUTES lpAttribute = NULL);
		// ... Open
		zzMutex(LPCTSTR lpszName, BOOL fInheritHandle = FALSE,
				DWORD dwDesiredAccess = MUTEX_ALL_ACCESS);

		BOOL	Release();
	};

/********************************************************/
/*		The Definition of the CriticalSectionEx class		*/
/********************************************************/
//
class zzCriticalSectionEx
	{
	protected:
		zzMutex*		m_pKernel;

	public:
		zzCriticalSectionEx();
		virtual ~zzCriticalSectionEx();

		void	Enter();
		void	Leave();

	};
/****************************************************/
/*		The Definition of the Semaphore class		*/
/****************************************************/
//
class zzSemaphore : public zzKernel
	{
	public:
		// ... Create
		zzSemaphore(LONG nInitCount, LONG nMaximumCount,
					LPCTSTR lpszName = NULL, 
					LPSECURITY_ATTRIBUTES lpAttributes = NULL);
		// ... Open
		zzSemaphore(LPCTSTR lpszName, BOOL fInheritHandle = FALSE,
					DWORD dwDesiredAccess = SEMAPHORE_ALL_ACCESS);

		BOOL	Release(LONG lReleaseCount, LONG * plPreviousCount = NULL);
	};


/************************************************/
/*		The Definition of the Event class		*/
/************************************************/
//
class zzEvent : public zzKernel
	{
	public:
		// ... create
		zzEvent(BOOL fManualReset = FALSE, BOOL fInitialState = FALSE,
				LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpAttributes = NULL);
		// ... open
		zzEvent(LPCTSTR lpszName, BOOL fInheritHandle = FALSE, DWORD dwDesiredAccess = EVENT_ALL_ACCESS);

		BOOL	Set();
		BOOL	Reset();
		BOOL	Pulse();
	};

/****************************************************************/
/*		The Definition of the Waitable Colllection class		*/
/****************************************************************/
//
class zzWaitableCollection
	{
	private:
		HANDLE	m_hHandles[ZZ_NUM_MAXIMUM_WAIT_OBJECT];
		LONG	m_lNumObjects;

	public:
		zzWaitableCollection();
		zzWaitableCollection(zzWaitableCollection &rWC);
		virtual ~zzWaitableCollection();

		LONG	GetCount() const;
		BOOL	Add(const zzKernel *pObject);
		BOOL	Add(const zzKernel &rObject);
		BOOL	Add(const HANDLE hHandle);
		BOOL	AddCollection(const zzWaitableCollection &rWC);
		DWORD	Wait(BOOL fWaitAll, DWORD dwMiliseconds) const;

		zzWaitableCollection & operator= (zzWaitableCollection &rWC);
	};

/********************************************************/
/*		The Definition of the Shared Memory class		*/
/********************************************************/
//
class zzSharedMemory
	{
	private:
		HANDLE	m_hMapping;
		LPVOID	m_lpMemory;
		BOOL	m_fIsCreator;
		DWORD	m_dwStatus;

	public:
		// ... create
		zzSharedMemory(LONG lSize, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpAttributes = NULL);
		// ... open
		zzSharedMemory(LPCTSTR lpszName);
		virtual ~zzSharedMemory();

		DWORD	Status() const;
		LPVOID	GetPtr();
		BOOL	IsCreator();
	};


/****************************************************/
/*		The Definition of the IPC Queue class		*/
/****************************************************/
//
class zzIPCQueue
	{
	private:
		struct IPCQueueHeader
			{
			DWORD	dwMaxDepth;
			DWORD	dwMsgSize;
			DWORD	dwBaseOffset;
			DWORD	dwHeadIndex;
			DWORD	dwTailIndex;
			};

	private:
		zzSharedMemory		m_SharedMemory;
		IPCQueueHeader		*m_pHeader;
		LPVOID				m_pBase;
		zzMutex				*m_pGuardMutex;
		zzSemaphore			*m_pFreeCountSemaphore;
		zzSemaphore			*m_pPendingCountSemaphore;
		DWORD				m_dwStatus;
		BOOL				m_fIsCreator;

	public:
		// ... create
		zzIPCQueue(DWORD dwMaxDepth, DWORD dwMsgSize, LPCTSTR lpszName = NULL,
				   LPSECURITY_ATTRIBUTES lpAttributes = NULL);
		// ... open
		zzIPCQueue(LPCTSTR lpszName);
		
		virtual ~zzIPCQueue();

		DWORD	Status() const;
		BOOL	IsCreator();
		BOOL	GetProperties(LPDWORD lpdwDepth, LPDWORD lpdwMsgSize);

		BOOL	Put(LPCVOID lpMsg, DWORD dwTimeout = INFINITE);
		LPVOID	Put2(DWORD dwTimeout = INFINITE);
		void	ReleasePendingCount();
		BOOL	PutAlertable(LPCVOID lpMsg, zzEvent *pInterrupt, DWORD dwTimeout = INFINITE);
		DWORD	PutAlertable(LPCVOID lpMsg, zzWaitableCollection &rCollection, DWORD dwTimeout = INFINITE);

		BOOL	Get(LPVOID lpMsg, DWORD dwTimeout = INFINITE);
		BOOL	Get2(LPVOID* lpMsg, DWORD dwTimeout = INFINITE);
		void	ReleaseFreeCount();
		BOOL	GetAlertable(LPVOID lpMsg, zzEvent *pInterrupt, DWORD dwTimeout = INFINITE);
		DWORD	GetAlertable(LPVOID lpMsg, zzWaitableCollection &rCollection, DWORD dwTimeout = INFINITE);

	private:
		inline void IncrementHead();
		inline void IncrementTail();
		inline void *GetHeadPtr();
		inline void *GetTailPtr();
		void CreateGuardMutexName(LPTSTR lpszName, LPCTSTR lpszBaseName);
		void CreateFreeCountSemaphoreName(LPTSTR lpszName, LPCTSTR lpszBaseName);
		void CreatePendingCountSemaphoreName(LPTSTR lpszName, LPCTSTR lpszBaseName);
	};


/********************************************/
/*		The Definition of Thread class		*/
/********************************************/
//
//	- Object생성시 Thread는 시작되지 않는다(default)
//	- Thread를 무한 루프로 돌리거나 어떤 시그널된상황까지 돌리려면
//	  Stop method를 만들어 Thread가 끝나는 것을 확인하고
//	  Object를 delete해야 한다.
//	  그렇지 않은 경우는 Destructor에서 Terminate하므로
//	  비정상적으로 종료될 수 있다.
//
class zzThread : public zzKernel
	{
	protected:
		TCHAR		m_szName[32];
		DWORD		m_dwThreadId;
		DWORD		m_dwCreationFlags;
		LPSECURITY_ATTRIBUTES m_lpAttributes;
		SIZE_T		m_dwStackSize;

	public:
		zzThread(BOOL fStart = FALSE, DWORD dwCreationFlags = 0,
				 LPSECURITY_ATTRIBUTES lpAttributes = NULL,
				 SIZE_T dwStackSize = 0);
		virtual ~zzThread();

		// ... Control Method
		virtual BOOL	Start();
		DWORD	Suspend();
		DWORD	Resume();
		BOOL	Terminate(DWORD dwExitCode);
		BOOL	SetPriority(int nPriority);

		// ... Informational Method
		BOOL	GetExitCode(LPDWORD pdwExitCode);
		int		GetPriority();
		DWORD	GetId();
	
	protected:
		// ... Working Method
		virtual	DWORD	Do();
		virtual BOOL	OnCreate();
		virtual void	OnTerminate();

	private:
		static DWORD WINAPI ThreadFunc(LPVOID lpvParam);
	};

#endif