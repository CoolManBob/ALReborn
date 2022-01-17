/*=============================================================

	zzThread.cpp

=============================================================*/
#include "ApBase.h"
#include "zzThread.h"
#include "ApMemoryTracker.h"
/****************************************************/
/*		The Implementation of Helper Functions		*/
/****************************************************/
//
BOOL zzIsValidHandle(HANDLE hHandle)
	{
	return (NULL != hHandle && INVALID_HANDLE_VALUE != hHandle);
	}

BOOL zzWaitSucceeded(DWORD dwWaitResult, DWORD dwHandleCount)	
	{
	return BOOL(((dwWaitResult >= WAIT_OBJECT_0) && (dwWaitResult <= WAIT_OBJECT_0 + dwHandleCount -1)));
	}

BOOL zzWaitAbandoned(DWORD dwWaitResult, DWORD dwHandleCount)
	{
	return BOOL(((dwWaitResult >= WAIT_ABANDONED_0) && (dwWaitResult <= WAIT_ABANDONED_0 + dwHandleCount -1)));
	}

DWORD zzWaitSucceededIndex(DWORD dwWaitResult)
	{
	return (dwWaitResult - WAIT_OBJECT_0);
	}

DWORD zzWaitAbandonedIndex(DWORD dwWaitResult)
	{
	return (dwWaitResult - WAIT_ABANDONED_0);
	}

BOOL zzWaitTimedOut(DWORD dwWaitResult)
	{
	return BOOL(WAIT_TIMEOUT == dwWaitResult);
	}

BOOL zzWaitFailed(DWORD dwWaitResult)
	{
	return BOOL(WAIT_FAILED == dwWaitResult);
	}

/************************************************************/
/*		The Implementation of the CriticalSection class		*/
/************************************************************/
//
zzCriticalSection::zzCriticalSection()
	{
	::InitializeCriticalSection(&m_CS);
	}

zzCriticalSection::~zzCriticalSection()
	{
	::DeleteCriticalSection(&m_CS);
	}

void zzCriticalSection::Enter()
	{
	::EnterCriticalSection(&m_CS);
	}

void zzCriticalSection::Leave()
	{
	::LeaveCriticalSection(&m_CS);
	}

CRITICAL_SECTION* zzCriticalSection::GetCS()
	{
	return &m_CS;
	}


/****************************************************/
/*		The Implementation of the Kernel class		*/
/****************************************************/
//
zzKernel::zzKernel()
	{
	m_hHandle = NULL;
	m_dwStatus = ERROR_INVALID_HANDLE;
	}

zzKernel::~zzKernel()
	{
	if (zzIsValidHandle(m_hHandle))
		{
		::CloseHandle(m_hHandle);
		m_hHandle = NULL;
		}
	}

DWORD zzKernel::Status() const
	{
	return m_dwStatus;
	}

DWORD zzKernel::Wait(DWORD dwMilliseconds)
	{
	return ::WaitForSingleObject(m_hHandle, dwMilliseconds);
	}

DWORD zzKernel::WaitForTwo(zzKernel *pKernel, BOOL fWaitAll, DWORD dwMilliseconds)
	{
	if (pKernel == NULL)
		{
		return WAIT_FAILED;
		}
	HANDLE hHandle[2];
	hHandle[0] = m_hHandle;
	hHandle[1] = pKernel->GetHandle();

	return ::WaitForMultipleObjects(2, hHandle, fWaitAll, dwMilliseconds);
	}

DWORD zzKernel::WaitForTwo(HANDLE hKernel, BOOL fWaitAll, DWORD dwMilliseconds)
	{
	if (hKernel == NULL)
		{
		return WAIT_FAILED;
		}
	HANDLE hHandle[2];
	hHandle[0] = m_hHandle;
	hHandle[1] = hKernel;

	return ::WaitForMultipleObjects(2, hHandle, fWaitAll, dwMilliseconds);
	}

HANDLE zzKernel::GetHandle() const
	{
		return m_hHandle;
	}

/****************************************************/
/*		The Implementation of the Mutex class		*/
/****************************************************/
//
zzMutex::zzMutex(BOOL fInitialOwner, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpAttributes)
	{
	m_hHandle = ::CreateMutex(lpAttributes, fInitialOwner, lpszName);
	if (zzIsValidHandle(m_hHandle))
		{
		if (lpszName)
			m_dwStatus = GetLastError();
		else
			m_dwStatus = NO_ERROR;
		}
	else
		{
		m_dwStatus = GetLastError();
		}
	}

zzMutex::zzMutex(LPCTSTR lpszName, BOOL fInheritHandle, DWORD dwDesiredAccess)
	{
	m_hHandle = ::OpenMutex(dwDesiredAccess, fInheritHandle, lpszName);
	if (zzIsValidHandle(m_hHandle))
		m_dwStatus = NO_ERROR;
	else
		m_dwStatus = GetLastError();
	}

BOOL zzMutex::Release()
	{
	return ::ReleaseMutex(m_hHandle);
	}

/********************************************************/
/*		The Implementation of the CriticalSectionEx		*/
/********************************************************/
//
zzCriticalSectionEx::zzCriticalSectionEx()
	{
	m_pKernel = NULL;
	m_pKernel = new zzMutex;
	}

zzCriticalSectionEx::~zzCriticalSectionEx()
	{
	if (NULL != m_pKernel)
		delete m_pKernel;
	}

void zzCriticalSectionEx::Enter()
	{
	DWORD dwWait = m_pKernel->Wait(INFINITE);
	if (WAIT_OBJECT_0 == dwWait || WAIT_ABANDONED_0 == dwWait)
		return;
	}

void zzCriticalSectionEx::Leave()
	{
	m_pKernel->Release();
	}


/********************************************************/
/*		The Implementation of the Semaphore class		*/
/********************************************************/
//
zzSemaphore::zzSemaphore(LONG nInitCount, LONG nMaxCount, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpAttributes)
	{
	m_hHandle = ::CreateSemaphore(lpAttributes, nInitCount, nMaxCount, lpszName);
	if (zzIsValidHandle(m_hHandle))
		{
		if (lpszName)
			m_dwStatus = GetLastError();
		else
			m_dwStatus = NO_ERROR;
		}
	else
		{
		m_dwStatus = GetLastError();
		}
	}

zzSemaphore::zzSemaphore(LPCTSTR lpszName, BOOL fInheritHandle, DWORD dwDesiredAccess)
	{
	m_hHandle = ::OpenSemaphore(dwDesiredAccess, fInheritHandle, lpszName);
	if (zzIsValidHandle(m_hHandle))
		m_dwStatus = NO_ERROR;
	else
		m_dwStatus = GetLastError();
	}

BOOL zzSemaphore::Release(LONG lReleaseCount, LONG *plPreviousCount)
	{
	LONG lPreviousCount;
	BOOL fStatus = ::ReleaseSemaphore(m_hHandle, lReleaseCount, &lPreviousCount);
	if (fStatus && plPreviousCount)
		*plPreviousCount = lPreviousCount;
	return fStatus;
	}

/****************************************************/
/*		The Implementation of the Event class		*/
/****************************************************/
//
zzEvent::zzEvent(BOOL fManualReset, BOOL fInitialState, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpAttributes)
	{
	m_hHandle = ::CreateEvent(lpAttributes, fManualReset, fInitialState, lpszName);
	if (zzIsValidHandle(m_hHandle))
		{
		if (lpszName)
			m_dwStatus = GetLastError();
		else
			m_dwStatus = NO_ERROR;
		}
	else
		{
		m_dwStatus = GetLastError();
		}
	}

zzEvent::zzEvent(LPCTSTR lpszName, BOOL fInheritHandle, DWORD dwDesiredAccess)
	{
	m_hHandle = ::OpenEvent(dwDesiredAccess, fInheritHandle, lpszName);
	if (zzIsValidHandle(m_hHandle))
		{
		m_dwStatus = NO_ERROR;
		}
	else
		{
		m_dwStatus = GetLastError();
		}
	}

BOOL zzEvent::Set()
	{
	return ::SetEvent(m_hHandle);
	}

BOOL zzEvent::Reset()
	{
	return ::ResetEvent(m_hHandle);
	}

BOOL zzEvent::Pulse()
	{
	return ::PulseEvent(m_hHandle);
	}

/********************************************************************/
/*		The Implementation of the Waitable Colllection class		*/
/********************************************************************/
//
zzWaitableCollection::zzWaitableCollection()
	{
	m_lNumObjects = 0;
	}

zzWaitableCollection::zzWaitableCollection(zzWaitableCollection &rWC)
	{
	*this = rWC;
	}

zzWaitableCollection::~zzWaitableCollection()
	{
	return;
	}

zzWaitableCollection& zzWaitableCollection::operator= (zzWaitableCollection &rWC)
	{
	for (int i=0; i<rWC.m_lNumObjects; i++)
		m_hHandles[i] = rWC.m_hHandles[i];
	m_lNumObjects = rWC.m_lNumObjects;
	return *this;
	}

LONG zzWaitableCollection::GetCount() const
	{
	return m_lNumObjects;
	}

BOOL zzWaitableCollection::Add(const zzKernel *pObject)
	{
	if (ZZ_NUM_MAXIMUM_WAIT_OBJECT == m_lNumObjects)
		return FALSE;

	m_hHandles[m_lNumObjects++] = pObject->GetHandle();
	return TRUE;
	}

BOOL zzWaitableCollection::Add(const zzKernel &rObject)
	{
	if (ZZ_NUM_MAXIMUM_WAIT_OBJECT == m_lNumObjects)
		return FALSE;

	m_hHandles[m_lNumObjects++] = rObject.GetHandle();
	return TRUE;
	}

BOOL zzWaitableCollection::Add(const HANDLE hHandle)
	{
	if (ZZ_NUM_MAXIMUM_WAIT_OBJECT == m_lNumObjects)
		return FALSE;

	m_hHandles[m_lNumObjects++] = hHandle;
	return TRUE;
	}

BOOL zzWaitableCollection::AddCollection(const zzWaitableCollection &rWC)
	{
	for (int i=0; i<rWC.m_lNumObjects; i++)
		{
		if (!Add(rWC.m_hHandles[i]))
			return FALSE;
		}
	return TRUE;
	}

DWORD zzWaitableCollection::Wait(BOOL fWaitAll, DWORD dwMilliseconds) const
	{
	if (0 == m_lNumObjects)
		return WAIT_FAILED;

	return ::WaitForMultipleObjects(m_lNumObjects, m_hHandles, fWaitAll, dwMilliseconds);
	}


/************************************************************/
/*		The Implementation of the Shared Memory class		*/
/************************************************************/
//
zzSharedMemory::zzSharedMemory(LONG lSize, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpAttributes)
	: m_lpMemory(NULL), m_fIsCreator(FALSE), m_dwStatus(NO_ERROR), m_hMapping(NULL)
	{
	m_hMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, lpAttributes, PAGE_READWRITE, 0, lSize, lpszName);
	if (zzIsValidHandle(m_hMapping))
		{
		m_dwStatus = GetLastError();
		if (ERROR_ALREADY_EXISTS == m_dwStatus)
			m_fIsCreator = FALSE;
		else
			m_fIsCreator = TRUE;
		}
	else
		{
		m_dwStatus = ::GetLastError();
		return;
		}

	m_lpMemory = (LPVOID) ::MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == m_lpMemory)
		{
		m_dwStatus = ::GetLastError();
		}
	}

zzSharedMemory::zzSharedMemory(LPCTSTR lpszName)
	: m_lpMemory(NULL), m_fIsCreator(FALSE), m_dwStatus(NO_ERROR), m_hMapping(NULL)
	{
	m_hMapping = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, lpszName);

	if (zzIsValidHandle(m_hMapping))
		{
		m_dwStatus = NO_ERROR;
		}
	else
		{
		m_dwStatus = ::GetLastError();
		return;
		}

	m_lpMemory = (LPVOID) ::MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == m_lpMemory)
		{
		m_dwStatus = ::GetLastError();
		}
	}

zzSharedMemory::~zzSharedMemory()
	{
	if (m_lpMemory)
		{
		::UnmapViewOfFile(m_lpMemory);
		m_lpMemory = NULL;
		}
	::CloseHandle(m_hMapping);
	}

DWORD zzSharedMemory::Status() const
	{
	return m_dwStatus;
	}

LPVOID zzSharedMemory::GetPtr()
	{
	return m_lpMemory;
	}

BOOL zzSharedMemory::IsCreator()
	{
	return m_fIsCreator;
	}



/********************************************************/
/*		The Implementation of the IPC Queue class		*/
/********************************************************/
//
zzIPCQueue::zzIPCQueue(DWORD dwMaxDepth, DWORD dwMsgSize, LPCTSTR lpszName, LPSECURITY_ATTRIBUTES lpAttributes)
	: m_SharedMemory( (LONG)(dwMaxDepth * dwMsgSize + sizeof(IPCQueueHeader)), lpszName, lpAttributes),
	m_pHeader(NULL),
	m_pBase(NULL),
	m_pGuardMutex(NULL),
	m_pFreeCountSemaphore(NULL),
	m_pPendingCountSemaphore(NULL),
	m_dwStatus(NO_ERROR),
	m_fIsCreator(FALSE)
	{
	TCHAR szName[MAX_PATH];
	
	CreateGuardMutexName(szName, lpszName);
	m_pGuardMutex = new zzMutex(TRUE, szName, lpAttributes);
	// ... check

	if (ERROR_ALREADY_EXISTS == m_pGuardMutex->Status())
		{
		m_pGuardMutex->Wait(INFINITE);
		m_fIsCreator = FALSE;
		}
	else
		{
		m_fIsCreator = TRUE;
		}

	CreateFreeCountSemaphoreName(szName, lpszName);
	m_pFreeCountSemaphore = new zzSemaphore(dwMaxDepth, dwMaxDepth, szName, lpAttributes);
	// ... check

	CreatePendingCountSemaphoreName(szName, lpszName);
	m_pPendingCountSemaphore = new zzSemaphore(0, dwMaxDepth, szName, lpAttributes);
	// ... check

	m_pHeader = (IPCQueueHeader *) m_SharedMemory.GetPtr();
	m_pBase = (LPVOID) ((BYTE *) m_pHeader + sizeof(IPCQueueHeader));

	if (m_fIsCreator)
		{
		m_pHeader->dwMaxDepth = dwMaxDepth;
		m_pHeader->dwMsgSize = dwMsgSize;
		m_pHeader->dwBaseOffset = sizeof(IPCQueueHeader);
		m_pHeader->dwHeadIndex = 0;
		m_pHeader->dwTailIndex = 0;
		}

	m_pGuardMutex->Release();

	if (!m_fIsCreator)
		{
		if ((m_pHeader->dwMaxDepth != dwMaxDepth) || (m_pHeader->dwMsgSize != dwMsgSize))
			{
			m_dwStatus = ERROR_INVALID_PARAMETER;

			return;
			}
		}
	}


zzIPCQueue::zzIPCQueue(LPCTSTR lpszName)
	: m_SharedMemory(lpszName),
	m_pHeader(NULL),
	m_pBase(NULL),
	m_pGuardMutex(NULL),
	m_pFreeCountSemaphore(NULL),
	m_pPendingCountSemaphore(NULL),
	m_dwStatus(NO_ERROR),
	m_fIsCreator(FALSE)
	{
	TCHAR szName[MAX_PATH];

	CreateGuardMutexName(szName, lpszName);
	m_pGuardMutex = new zzMutex(szName);
	// ... check
	m_pGuardMutex->Wait(INFINITE);

	CreateFreeCountSemaphoreName(szName, lpszName);
	m_pFreeCountSemaphore = new zzSemaphore(szName);
	// ... check

	CreatePendingCountSemaphoreName(szName, lpszName);
	m_pPendingCountSemaphore = new zzSemaphore(szName);
	// ... check

	m_pHeader = (IPCQueueHeader *) m_SharedMemory.GetPtr();
	m_pBase = (LPVOID) ((BYTE *) m_pHeader + sizeof(IPCQueueHeader));
	m_pGuardMutex->Release();
	}

zzIPCQueue::~zzIPCQueue()
	{
	}

DWORD zzIPCQueue::Status() const
	{
	return m_dwStatus;
	}
BOOL zzIPCQueue::IsCreator()
	{
	return m_fIsCreator;
	}

BOOL zzIPCQueue::GetProperties(LPDWORD lpdwDepth, LPDWORD lpdwMsgSize)
	{
	if (m_pHeader)
		{
		*lpdwDepth = m_pHeader->dwMaxDepth;
		*lpdwMsgSize = m_pHeader->dwMsgSize;
		return TRUE;
		}
	else
		return FALSE;
	}

//	Put/Get method
//=======================================================
//
BOOL zzIPCQueue::Put(LPCVOID lpMsg, DWORD dwTimeout)
	{
	DWORD dwStatus = m_pFreeCountSemaphore->Wait(dwTimeout);
	if (zzWaitSucceeded(dwStatus, 1))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			CopyMemory(GetTailPtr(), lpMsg, m_pHeader->dwMsgSize);
			IncrementTail();
			m_pGuardMutex->Release();
			}
		m_pPendingCountSemaphore->Release(1);
		}

	return (zzWaitSucceeded(dwStatus, 1));
	}

LPVOID zzIPCQueue::Put2(DWORD dwTimeout)
	{
	DWORD dwStatus = m_pFreeCountSemaphore->Wait(dwTimeout);
	if (zzWaitSucceeded(dwStatus, 1))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			LPVOID lpvRet = GetTailPtr();
			IncrementTail();
			m_pGuardMutex->Release();
			return lpvRet;
			}
		}
	return NULL;
	}

void zzIPCQueue::ReleasePendingCount()
	{
	m_pPendingCountSemaphore->Release(1);
	}

BOOL zzIPCQueue::PutAlertable(LPCVOID lpMsg, zzEvent *pInterrupt, DWORD dwTimeout)
	{
	BOOL fStatus = FALSE;
	DWORD dwStatus = pInterrupt->WaitForTwo(m_pFreeCountSemaphore, FALSE, dwTimeout);
	if (zzWaitSucceeded(dwStatus, 2) && (WAIT_OBJECT_0 + 1 == zzWaitSucceededIndex(dwStatus)))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			CopyMemory(GetTailPtr(), lpMsg, m_pHeader->dwMsgSize);
			IncrementTail();
			m_pGuardMutex->Release();
			fStatus = TRUE;
			}
		m_pPendingCountSemaphore->Release(1);
		}
	return fStatus;
	}

DWORD zzIPCQueue::PutAlertable(LPCVOID lpMsg, zzWaitableCollection &rCollection, DWORD dwTimeout)
	{
	zzWaitableCollection Collection;
	Collection.Add(m_pFreeCountSemaphore);
	Collection.AddCollection(rCollection);
	int nObjects = Collection.GetCount();

	DWORD dwStatus = Collection.Wait(FALSE, dwTimeout);

	if (zzWaitSucceeded(dwStatus, nObjects) && (WAIT_OBJECT_0 == zzWaitSucceededIndex(dwStatus)))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			CopyMemory(GetTailPtr(), lpMsg, m_pHeader->dwMsgSize);
			IncrementTail();
			m_pGuardMutex->Release();
			dwStatus = WAIT_OBJECT_0;
			}
		m_pPendingCountSemaphore->Release(1);
		}
	return dwStatus;
	}

BOOL zzIPCQueue::Get(LPVOID lpMsg, DWORD dwTimeout)
	{
	DWORD dwStatus = m_pPendingCountSemaphore->Wait(dwTimeout);
	if (zzWaitSucceeded(dwStatus, 1))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			CopyMemory(lpMsg, GetHeadPtr(), m_pHeader->dwMsgSize);
			IncrementHead();
			m_pGuardMutex->Release();
			}
		m_pFreeCountSemaphore->Release(1);
		}
	return (zzWaitSucceeded(dwStatus, 1));
	}

BOOL zzIPCQueue::Get2(LPVOID* lpMsg, DWORD dwTimeout)
	{
	DWORD dwStatus = m_pPendingCountSemaphore->Wait(dwTimeout);
	if (zzWaitSucceeded(dwStatus, 1))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			*lpMsg = GetHeadPtr();
			IncrementHead();
			m_pGuardMutex->Release();
			}
		//m_pFreeCountSemaphore->Release(1);
		}
	return (zzWaitSucceeded(dwStatus, 1));
	}

void zzIPCQueue::ReleaseFreeCount()
	{
	m_pFreeCountSemaphore->Release(1);
	}

BOOL zzIPCQueue::GetAlertable(LPVOID lpMsg, zzEvent *pInterrupt, DWORD dwTimeout)
	{
	BOOL fStatus = FALSE;
	DWORD dwStatus = pInterrupt->WaitForTwo(m_pPendingCountSemaphore, FALSE, dwTimeout);
	if (zzWaitSucceeded(dwStatus, 2) && ((WAIT_OBJECT_0 + 1) == zzWaitSucceededIndex(dwStatus)))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			CopyMemory(lpMsg, GetHeadPtr(), m_pHeader->dwMsgSize);
			IncrementHead();
			m_pGuardMutex->Release();
			fStatus = TRUE;
			}
		m_pFreeCountSemaphore->Release(1);
		}
	return fStatus;
	}

DWORD zzIPCQueue::GetAlertable(LPVOID lpMsg, zzWaitableCollection &rCollection, DWORD dwTimeout)
	{
	zzWaitableCollection Collection;
	Collection.Add(m_pPendingCountSemaphore);
	Collection.AddCollection(rCollection);
	int nObjects = Collection.GetCount();

	DWORD dwStatus = Collection.Wait(FALSE, dwTimeout);
	if (zzWaitSucceeded(dwStatus, nObjects) && ((WAIT_OBJECT_0) == zzWaitSucceededIndex(dwStatus)))
		{
		dwStatus = m_pGuardMutex->Wait(dwTimeout);
		if (zzWaitSucceeded(dwStatus, 1) || zzWaitAbandoned(dwStatus, 1))
			{
			CopyMemory(lpMsg, GetHeadPtr(), m_pHeader->dwMsgSize);
			IncrementHead();
			m_pGuardMutex->Release();
			dwStatus = WAIT_OBJECT_0;
			}
		m_pFreeCountSemaphore->Release(1);
		}
	return dwStatus;
	}

void zzIPCQueue::IncrementHead()
	{
	m_pHeader->dwHeadIndex = (m_pHeader->dwHeadIndex + 1) % m_pHeader->dwMaxDepth;
	}

void zzIPCQueue::IncrementTail()
	{
	m_pHeader->dwTailIndex = (m_pHeader->dwTailIndex + 1) % m_pHeader->dwMaxDepth;
	}

LPVOID zzIPCQueue::GetHeadPtr()
	{
	return (LPVOID) ((BYTE *) m_pBase + (m_pHeader->dwHeadIndex * m_pHeader->dwMsgSize));
	}

LPVOID zzIPCQueue::GetTailPtr()
	{
	return (LPVOID) ((BYTE *) m_pBase + (m_pHeader->dwTailIndex * m_pHeader->dwMsgSize));
	}

void zzIPCQueue::CreateGuardMutexName(LPTSTR lpszName, LPCTSTR lpszBaseName)
	{
	wsprintf(lpszName, TEXT("%s::%s"), TEXT("zzIPCQGuardMutex"), lpszBaseName);
	}

void zzIPCQueue::CreateFreeCountSemaphoreName(LPTSTR lpszName, LPCTSTR lpszBaseName)
	{
	wsprintf(lpszName, TEXT("%s::%s"), TEXT("zzIPCQFreeCountSemaphore"), lpszBaseName);
	}

void zzIPCQueue::CreatePendingCountSemaphoreName(LPTSTR lpszName, LPCTSTR lpszBaseName)
	{
	wsprintf(lpszName, TEXT("%s::%s"), TEXT("zzIPCQPendingCountSemaphore"), lpszBaseName);
	}



/************************************************/
/*		The Implementation of Thread class		*/
/************************************************/
//
zzThread::zzThread(BOOL fStart, DWORD dwCreationFlags, LPSECURITY_ATTRIBUTES lpAttributes, SIZE_T dwStackSize)
	: m_dwCreationFlags(dwCreationFlags), m_dwStackSize(dwStackSize),
	  m_lpAttributes(lpAttributes), m_dwThreadId(0)
	{
	ZeroMemory(m_szName, sizeof(m_szName));
	if (fStart)
		{
		m_hHandle = CreateThread(m_lpAttributes, m_dwStackSize, ThreadFunc, (LPVOID)(zzThread *)this, m_dwCreationFlags, &m_dwThreadId);
		if (!zzIsValidHandle(m_hHandle))
			{
			m_dwStatus = ::GetLastError();
	//		TRACE0("Thred creation error\n");
			}
			SetThreadName(m_dwThreadId, m_szName);
		}
	}

zzThread::~zzThread()
	{
	DWORD dwExitCode;
	GetExitCode(&dwExitCode);
	if (STILL_ACTIVE == dwExitCode)
		{
		Terminate(10);
		}
	}

BOOL zzThread::Start()
	{
	if (NULL == m_hHandle)
		{
		m_hHandle = CreateThread(m_lpAttributes, m_dwStackSize, ThreadFunc, (LPVOID)(zzThread *)this, 0, &m_dwThreadId);
		if (!zzIsValidHandle(m_hHandle))
			{
			m_dwStatus = ::GetLastError();
			return FALSE;
			}
		}
		SetThreadName(m_dwThreadId, m_szName);
	return TRUE;
	}

DWORD zzThread::Suspend()
	{
	return ::SuspendThread(m_hHandle);
	}

DWORD zzThread::Resume()
	{
	return ::ResumeThread(m_hHandle);
	}

BOOL zzThread::Terminate(DWORD dwExitCode)
	{
	return ::TerminateThread(m_hHandle, dwExitCode);
	}

BOOL zzThread::GetExitCode(LPDWORD pdwExitCode)
	{
	return::GetExitCodeThread(m_hHandle, pdwExitCode);
	}

BOOL zzThread::SetPriority(int nPriority)
	{
	return ::SetThreadPriority(m_hHandle, nPriority);
	}

int zzThread::GetPriority()
	{
	return ::GetThreadPriority(m_hHandle);
	}

DWORD zzThread::GetId()
	{
	return m_dwThreadId;
	}

DWORD WINAPI zzThread::ThreadFunc(LPVOID lpvParam)
	{
	zzThread *pThis = (zzThread *) lpvParam;
	DWORD dwRet = 0;
	if (!pThis->OnCreate())	
		{
		dwRet = 11;
		return dwRet;
		}

	dwRet = pThis->Do();
	pThis->OnTerminate();
	return dwRet;
	}

BOOL zzThread::OnCreate()
	{
	TRACE(_T("zzThread::OnCreate()"));
	return TRUE;
	}

void zzThread::OnTerminate()
	{
	TRACE(_T("zzThread::OnTerminate()"));
	}

DWORD zzThread::Do()
	{
	TCHAR szMessage[1024]; 
	wsprintf(szMessage, "\
// THREAD INFORMATION\n\
// \tHandle = [0x%08x]\n\
// \tID        = [0x%08x]\n\
\n\
\n\
¾Ï°Íµµ ¾ÈÇÏÁö·Õ...virtual DWORD zzThread::Do()¸¦ »ó¼Ó¹Þ¾Æ¼­ ¸¸µé¾î¶û!!! Ä¼ÇÏÇÏ.\n\
\n\
\n\
\n\
                  ( -_-)«Î (((¡Ü~*  <ÆøÅº ¹Þ¾Æ¶ù~ ¾å!!>                        \n\
\n\
\n\
                                   _  _                                        \n\
                                 (¢Á.¢Á)                                       \n\
                                ¢Ñ ªÒ ¢Ð                                       \n\
                                                                               \n\
                                  ¸Þ~·Õ~                                       ",
	m_hHandle, m_dwThreadId);
	MessageBox(NULL, szMessage, _T("¸Ó¾ß?"), MB_OK);
	return 1;
	}
