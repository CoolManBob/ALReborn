/*===================================================================

	IOCPDatabase.cpp

===================================================================*/

#include "IOCPDatabase.h"
#include <memory>
#include "ApMemoryTracker.h"



//#define AGSMTRACE	TRACE
#define AGSMTRACE	printf

#define BUFFERING_RESERVE_COUNT		550
#define BUFFERING_COMMIT_COUNT		500


/****************************************************/
/*		The Implementation of DBWorker class		*/
/****************************************************/
//
DBWorker::DBWorker()
	:m_pManager(NULL), m_pConnection(NULL), m_pStatement(NULL), m_pRowset(NULL), m_bStop(TRUE)
	{
	m_bIgnoreFail = FALSE;
	strcpy(m_szName, "AgsmDB");

	m_bBufferingCommit = FALSE;
	m_lNowBufferingCount = 0;
	m_lMaxBufferingCount = 0;
	}


DBWorker::~DBWorker()
	{
	// Buffering Commit Mode 라면 Critical Section 해제 한다.
	if(m_bBufferingCommit)
		DeleteBufferingLock();

	Disconnect();

	}


void DBWorker::Stop()
	{
	m_pIOCP->PostStatus(NULL, 9999);
	}




//	zzThread inherited
//====================================================
//
BOOL DBWorker::OnCreate()
	{
	CoInitialize(NULL);
	m_bIgnoreFail = m_pManager->m_bIgnoreFail;
	
	if (FALSE == Connect())
		return FALSE;

	m_bStop = FALSE;
	return TRUE;
	}


void DBWorker::OnTerminate()
	{
	// release database object (when abnormal termination)
	Disconnect();

	CoUninitialize();
	}




//	Main worker function
//===============================================
//
DWORD DBWorker::Do()
	{
	AgsdQuery		*pQuery;
	DWORD			dwTransferred;
	LPOVERLAPPED	pOverlapped;
	LONG			lExecuted = 0;
	LONG			lFailed = 0;
	LONG			lCommitted = 0;
	SYSTEMTIME		st;
	CHAR			szLog[MAX_PATH];

	// 2008.01.06. steeple
	// Buffering Commit 모드라면, Ciritical Section 초기화 한다.
	if(m_bBufferingCommit)
	{
		InitializeBufferingLock();
		
		// Vector 크기를 잡아놓는다.
		m_BufferingQueryVector.reserve(BUFFERING_RESERVE_COUNT);
	}

	while (FALSE == m_bStop)
		{
		if (m_pIOCP->GetStatus((PULONG_PTR)&pQuery, &dwTransferred, &pOverlapped, INFINITE))
			{
			if (9999 == dwTransferred)	// 9999 is termination signal
				{
				lExecuted = m_pIOCP->Executed();
				return 1;
				}

			if (99999 == dwTransferred && m_bBufferingCommit)	// 99999 is flush buffering commit
				{
				ProcessBufferingQuery(NULL, TRUE);
				continue;
				}

			// 2008.01.21. steeple
			if (m_bBufferingCommit)
			{
				ProcessBufferingQuery(pQuery);
				continue;
			}

			ExecuteQuery(pQuery);
			
			lExecuted = m_pIOCP->Executed();

			// print query count
			if (1 == (lExecuted % 100))
				AGSMTRACE("[%05d][%08d] Query Executed [%d] Committed[%d]\n", m_nIndex, GetId(), lExecuted, lCommitted);
			}
		else
			{
			GetLocalTime(&st);
			sprintf(szLog, "LOG\\RelayServer_QueueFail-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : GetStatus failed in DBWorker::Do()");
			AuLogFile_s(szLog, strCharBuff);
			}
		}
	
	return 0;
	}

#define DB_QUERY_TIMER

BOOL DBWorker::Execute(AgsdQuery* pQuery)
	{
	
#ifdef DB_QUERY_TIMER
	// 2008.01.06. steeple
	// DB Query Timer 추가. 10초 이상 걸린 녀석들 로그 남긴다.
	CHAR szFileName[255];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFileName, "LongTimeQuery_%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

	std::stringstream title;
	title << "Query Index : %d" << pQuery->m_nIndex;
	AuStopWatch2(szFileName, title.str(), 10000);	// 10초 이상 걸린 녀석들 로그 남긴다.
#endif	

	// DB와의 연결이 끊어지거나 해서 reset되었다.
	if (NULL == m_pStatement)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "DBWorker::Execute(), m_pStatement is NULL\n");
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}

	if(!pQuery->Get())
	{
		printf("Query = %d, %s\n", pQuery->m_nIndex, pQuery->Get());
		return FALSE;
	}
	
	BOOL bResult = TRUE;
	if (FALSE == m_pStatement->Prepare(pQuery->Get()))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "DBWorker::Execute(), m_pStatement->Prepare() returned FALSE. QueryIndex:%d\n", pQuery->m_nIndex);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);

		pQuery->Dump();
		if(pQuery->m_pfCallback)
			pQuery->m_pfCallbackFail(NULL, pQuery->m_pClass, pQuery->m_pData);

		return FALSE;
		}

	if (FALSE == pQuery->BindParam(m_pStatement) || FALSE == m_pStatement->Execute())
		{
		pQuery->Dump();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "DBWorker::Execute(), m_pStatement->BindParam() or m_pStatement->Execute() returned FALSE. QueryIndex:%d\n", pQuery->m_nIndex);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		if(pQuery->m_pfCallbackFail)
			pQuery->m_pfCallbackFail(NULL, pQuery->m_pClass, pQuery->m_pData);

		return FALSE;
		}
	
	if (m_pStatement->HasResult())
		{
		// bind
		m_pRowset->Set(*m_pStatement);
		m_pRowset->Bind(TRUE);

		// fetch
		while (!m_pRowset->IsEnd())
		{
			if(!m_pRowset->Fetch())
			{
				printf("Fetch error %d \n", pQuery->GetIndex());
				m_pRowset->Close();
				return FALSE;
			}
		}
	}
	else
		{
		if (0 == m_pStatement->GetAffectedRows())
			{
			// exclude stored procedure(index 100 ~ 200).
			// 403 is item deletion procedure
			// 317 is first login time of today update
			if (403 != pQuery->m_nIndex &&
				200 <= pQuery->m_nIndex &&
				317 != pQuery->m_nIndex )
				{
				pQuery->Dump();
				AGSMTRACE("!!! Warning : [%05d][%08d] No Affected rows.\n\t Query=[%s]\n",
							m_nIndex, GetId(), pQuery->Get());
				}
			}
		}
	
	// callback
	if(pQuery->m_pfCallback)
		pQuery->m_pfCallback(m_pRowset, pQuery->m_pClass, pQuery->m_pData);

	m_pRowset->Close();

	return TRUE;
	}

// 2008.01.21. steeple
BOOL DBWorker::ExecuteQuery(AgsdQuery* pQuery)
{
	if (!pQuery)
		return FALSE;

	LONG			lFailed = 0;
	SYSTEMTIME		st;
	CHAR			szLog[MAX_PATH];

	//printf("Query = %d, %s\n", pQuery->m_nIndex, pQuery->Get());

	if (FALSE == Execute(pQuery))
	{
		// check connection and re-post
		INT32 lFailed = m_pIOCP->Failed();
		AGSMTRACE("!!! Error : [%05d][%08d] Query Failed [%8d][%s]\n",
					m_nIndex, GetId(), lFailed, pQuery->Get() ? pQuery->Get() : "");
					
		GetLocalTime(&st);
		sprintf(szLog, "LOG\\RelayServer_QueryError-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);
		
		if (FALSE == m_pConnection->IsAlive())
		{
			GetLocalTime(&st);
			sprintf(szLog, "LOG\\RelayServer_Disconnect-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

			char strCharBuff2[512] = { 0, };
			sprintf_s(strCharBuff2, sizeof(strCharBuff2), "%02d:%02d:%02d [%s.%s] Database Connection dead\n", st.wHour, st.wMinute, st.wSecond, m_pManager->m_szDSN, m_pManager->m_szUser);
			AuLogFile_s(szLog, strCharBuff2);
			if (!m_bIgnoreFail)
			{
				char strCharBuff3[256] = { 0, };
				sprintf_s(strCharBuff3, sizeof(strCharBuff3), "%02d:%02d:%02d ExitProcess()\n", st.wHour, st.wMinute, st.wSecond);
				AuLogFile_s(szLog, strCharBuff3);
				ExitProcess(100);
			}
		}

		if (NULL == m_pConnection || FALSE == m_pConnection->IsAlive())
		{
			Connect();
		}
	}

	pQuery->Release();
	return TRUE;		
}

BOOL DBWorker::Connect()
	{
	ASSERT((NULL != m_pManager) && "Manager is NULL in DBWorker::Connect()");
	
	m_pConnection = m_pManager->CreateDatabase();
	if (0 == m_pManager->m_cType)	// DSN
		{
		if (!m_pConnection->Open(m_pManager->m_szDSN, m_pManager->m_szUser, m_pManager->m_szPwd))
			{
			AGSMTRACE("!!!Error : [%05d][%08d] Database open failed\n", m_nIndex, GetId());
			return FALSE;
			}
			
		}
	else							// Connection String
		{
		if (!m_pConnection->Open(m_pManager->m_szDSN))
			{
			AGSMTRACE("!!!Error : [%05d][%08d] Database open failed\n", m_nIndex, GetId());
			return FALSE;
			}
		}

	m_pStatement = m_pManager->CreateStatement(m_pConnection);
	if (!m_pStatement->Open())
		{
		AGSMTRACE("!!!Error : [%05d][%08d] Statement open failed\n", m_nIndex, GetId());
		return FALSE;
		}
	
	// if OCI, set transaction mode to commit on success
	if (AUDB_API_OCI == m_pManager->m_eApi)
		{
		// 2008.01.06. steeple
		// Buffering Commit 추가.
		// 그동안 기본적으로 OCI_COMMIT_ON_SUCCESS 였음.
		if(m_pManager->m_bBufferingCommit)
			m_pStatement->SetTransactionMode(FALSE);
		else
			m_pStatement->SetTransactionMode(TRUE);
		}

	m_pRowset = m_pManager->CreateRowset(NULL);
	m_pRowset->InitializeBuffer(2048);	// 16K. pre-alloc

	AGSMTRACE("[%05d][%08d] Database opened successfully\n", m_nIndex, GetId());

	return TRUE;
	}


void DBWorker::Disconnect()
	{
	ASSERT((NULL != m_pManager) && "Manager is NULL in DBWorker::Disconnect()");

	if (NULL != m_pRowset)
		{
		m_pRowset->Close();
		m_pRowset->Release();
		m_pRowset = NULL;
		}

	if (NULL != m_pStatement)
		{
		m_pStatement->Close();
		delete m_pStatement;
		m_pStatement = NULL;
		}

	if (NULL != m_pConnection)
		{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;
		}
	}

// 2008.01.21. steeple
// bFlush shows that buffering will be flushed.
BOOL DBWorker::ProcessBufferingQuery(AgsdQuery* pcsQuery, BOOL bFlush)
{
	LONG lExecuted = 0;
	LONG lCommitted = 0;

	BufferingLock();

	if(pcsQuery)
		m_BufferingQueryVector.push_back(pcsQuery);

	if(m_BufferingQueryVector.size() >= m_lMaxBufferingCount || bFlush)
	{
		BufferingQueryIter iter = m_BufferingQueryVector.begin();
		while(iter != m_BufferingQueryVector.end())
		{
			AgsdQuery* pQuery = *iter;
			if(!pQuery)
			{
				++iter;
				continue;
			}

			ExecuteQuery(pQuery);

			++iter;
		}

		m_pConnection->EndTransaction();

		// print query count
		lExecuted = m_pIOCP->Executed();
		AGSMTRACE("[%05d][%08d] Query Executed [%d] Committed[%d]\n", m_nIndex, GetId(), lExecuted, lCommitted);

		m_BufferingQueryVector.clear();
	}
	
	BufferingUnLock();

	return TRUE;
}

// 2008.01.07. steeple
BOOL DBWorker::FlushBufferingCommit()
{
	if(m_pConnection && m_pConnection->IsAlive())
		m_pIOCP->PostStatus(NULL, 99999);

	return TRUE;
}



/********************************************************/
/*		The Implementation of IOCPDatabase class		*/
/********************************************************/
//
IOCPDatabase::IOCPDatabase()
	:m_pWorkers(NULL)
	{
	m_cType = 0;
	m_szDSN[0] = _T('\0');
	m_szUser[0] = _T('\0');
	m_szPwd[0] = _T('\0');
	m_eApi = AUDB_API_ODBC;
	m_bIgnoreFail = FALSE;

	m_bBufferingCommit = FALSE;
	}


IOCPDatabase::~IOCPDatabase()
	{
	}


BOOL IOCPDatabase::Create(INT16 nWorkers)
	{
	Initialize(m_eApi);
	m_nWorkers = nWorkers;
	m_IOCP.Create(nWorkers);
	m_pWorkers = new DBWorker[nWorkers];
	
	for (INT16 i=0; i<nWorkers; i++)
		{
		m_pWorkers[i].m_nIndex = i;
		m_pWorkers[i].m_pManager = this;
		m_pWorkers[i].m_pIOCP = &m_IOCP;

		if(m_bBufferingCommit)
			{
			m_pWorkers[i].SetBufferingCommit(TRUE);
			m_pWorkers[i].SetMaxBufferingCount(BUFFERING_COMMIT_COUNT);	// 500 개 모아서 한방에 커밋
			m_pWorkers[i].InitializeBufferingLock();
			}

		m_pWorkers[i].Start();
		}

	return TRUE;
	}


void IOCPDatabase::Destroy()
	{
	// stop all workers(thread)
	for (INT16 i=0; i<m_nWorkers; i++)
		m_pWorkers[i].Stop();
	
	// wait thread for 3 seconds before delete
	zzWaitableCollection Workers;
	for (INT16 i=0; i<m_nWorkers; i++)
		Workers.Add(m_pWorkers[i]);
	Workers.Wait(TRUE, 3000);

	// 3초 동안 지둘렸는디 안끝나믄 걍 delete한다.
	// 내부적으론 thread가 terminate된당. 머 어쩔 수 없는거지 -_-;
	delete [] m_pWorkers;

	// IOCP
	m_IOCP.Destroy();

	// AuDabaseManager
	Terminate();
	}


BOOL IOCPDatabase::PostQuery(AgsdQuery* pQuery)
	{
	return m_IOCP.PostStatus((ULONG_PTR)pQuery);
	}


BOOL IOCPDatabase::PostStatus(INT32 lStatus)
	{
	return m_IOCP.PostStatus(NULL, lStatus);
	}




//	Status
INT32 IOCPDatabase::GetPosted()
	{
	return m_IOCP.GetPosted();
	}


INT32 IOCPDatabase::GetExecuted()
	{
	return m_IOCP.GetExecuted();
	}


INT32 IOCPDatabase::GetFailed()
	{
	return m_IOCP.GetFailed();
	}

BOOL IOCPDatabase::FlushAll()
{
	for(INT16 i = 0; i < m_nWorkers; ++i)
	{
		m_pWorkers[i].FlushBufferingCommit();
	}

	return TRUE;
}





/************************************************/
/*		The Implementation of IOCP class		*/
/************************************************/
//
zzIOCP::zzIOCP()
	: m_hIOCP(NULL), m_lPosted(0), m_lQueued(0), m_lExecuted(0), m_lFailed(0)
	{
	}


zzIOCP::~zzIOCP()
	{
	}


BOOL zzIOCP::Create(DWORD dwThreadCount)
	{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwThreadCount);
	if (NULL == m_hIOCP)
		return FALSE;

	return TRUE;
	}


VOID zzIOCP::Destroy()
	{
	if (NULL != m_hIOCP)
		{
		::CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
		}

	WriteStatus();
	}


BOOL zzIOCP::PostStatus(ULONG_PTR dwKey, DWORD dwBytes, LPOVERLAPPED pOverlapped)
	{
	ASSERT((NULL != m_hIOCP) && "Invalid IOCP Handle in zzIOCP::PostStatus()");
	LONG lPosted = ::InterlockedIncrement(&m_lPosted);

	// write status
	if (9999 == (lPosted % 10000))
		WriteStatus();

	if (::PostQueuedCompletionStatus(m_hIOCP, dwBytes, dwKey, pOverlapped))
		{
		::InterlockedIncrement(&m_lQueued);
		return TRUE;
		}

	return FALSE;
	}


BOOL zzIOCP::GetStatus(PULONG_PTR pdwKey, DWORD *pdwBytes, LPOVERLAPPED *ppOverlapped, DWORD dwTime)
	{
	ASSERT((NULL != m_hIOCP) && "Invalid IOCP Handle in zzIOCP::PostStatus()");
	if (::GetQueuedCompletionStatus(m_hIOCP, pdwBytes, pdwKey, ppOverlapped, dwTime))
		{
		::InterlockedDecrement(&m_lQueued);
		return TRUE;
		}
	return FALSE;
	}

LONG zzIOCP::Executed()
	{
	return ::InterlockedIncrement(&m_lExecuted);
	}


LONG zzIOCP::Failed()
	{
	return ::InterlockedIncrement(&m_lFailed);
	}


void zzIOCP::WriteStatus()
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf(szFile, "LOG\\RelayServer_Status-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d] Posted[%15d] Queued[%15d] Execed[%15d] Failed[%15d]\n",
					st.wHour, st.wMinute, st.wSecond, m_lPosted, m_lQueued, m_lExecuted, m_lFailed);
	AuLogFile_s(szFile, strCharBuff);
	}
