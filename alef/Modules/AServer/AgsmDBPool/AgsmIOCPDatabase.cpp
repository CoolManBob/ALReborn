// AgsmIOCPDatabase.cpp: implementation of the AgsmIOCPDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmIOCPDatabase.h"
#include <memory>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LONG g_lExecuteCount = 0L;

AgsmIOCPDatabase::AgsmIOCPDatabase()
{
	// DB Manager를 Oracle의 OLEDB로 초기화
	m_DBManager.Initialize(AUDB_API_OLEDB, AUDB_ORACLE);

	// select의 결과를 담을 버퍼를 2MB로 초기화
	m_DBManager.InitializeHeap(2);

	InitConnectParam();
}

AgsmIOCPDatabase::AgsmIOCPDatabase(eAgsmIOCPDatabaseApi eApi)
{
	BOOL bInitialized = FALSE;
	switch(eApi)
	{
		case AGSMIOCPDATABASE_API_OLEDB:
			m_DBManager.Initialize(AUDB_API_OLEDB, AUDB_ORACLE);
			bInitialized = TRUE;
			break;

		case AGSMIOCPDATABASE_API_ODBC:
			m_DBManager.Initialize(AUDB_API_ODBC, AUDB_MYSQL);
			bInitialized = TRUE;
			break;
		
		default:
			ASSERT(!"AgsmIOCPDatabase::m_DBManager 초기화 실패");
			break;
	}

	if(bInitialized)
		m_DBManager.InitializeHeap(2);

	InitConnectParam();
}

AgsmIOCPDatabase::~AgsmIOCPDatabase()
{
	m_DBManager.TerminateHeap();
	m_DBManager.Terminate();
}

BOOL AgsmIOCPDatabase::InitConnectParam()
{
	m_szDataSource = NULL;
	m_szUser = NULL;
	m_szPassword = NULL;

	return TRUE;
}

BOOL AgsmIOCPDatabase::SetConnectParam(LPCTSTR szDataSource, LPCTSTR szUser, LPCTSTR szPassword)
{
	if(!szDataSource || !szUser || !szPassword)
		return FALSE;

	m_szDataSource = szDataSource;
	m_szUser = szUser;
	m_szPassword = szPassword;

	return TRUE;
}

BOOL AgsmIOCPDatabase::Create(INT32 lDBConnectionCount)
{
	if (!m_DBEvents.Create(lDBConnectionCount)) return FALSE;

	// Worker Thread를 생성한다.
	ApIOCP::Create(lDBConnectionCount, DBWorkerThread, PVOID(this));

	// DB에 Connection이 모두 연결될때까지 기다린다.
	return m_DBEvents.Wait();
}

BOOL AgsmIOCPDatabase::PostSQL(SQLBuffer* pSQLBuffer)
{
	return PostStatus((DWORD)pSQLBuffer);
}

// 2004.2.29. 김태희 수정.
// 외부에서 Connect Param 을 세팅할 수 있게 변경
BOOL AgsmIOCPDatabase::CreateDBConnection(AuDatabase** ppConnection, AuStatement** ppStatement, AuRowset** ppRowset)
{
	// Connect Param 이 세팅 안되어 있으면 나간다.
	if(!m_szDataSource || !m_szUser || !m_szPassword)
		return FALSE;

	// DB connection 생성
	*ppConnection = m_DBManager.CreateDatabase();
	if (!(*ppConnection)->Open(m_szDataSource, m_szUser, m_szPassword)) return FALSE;

	// Statement 생성
	*ppStatement = m_DBManager.CreateStatement(*ppConnection);
	if (!(*ppStatement)->Open()) return FALSE;
	
	// Rowset을 미리 생성
	*ppRowset = m_DBManager.CreateRowset(NULL);
	(*ppRowset)->InitializeBuffer(4);	// 4K. pre-alloc

	return TRUE;
}

// auRowset 을 전달 받는 방법으로 변경. - 2004.03.04. steeple
BOOL AgsmIOCPDatabase::ExecuteSQL(AuStatement* pStatement, AuRowset* pRowset, SQLBuffer* pSQLBuffer)
{
	if (pStatement->Execute(pSQLBuffer->Buffer))
	{
		// 결과가 없으면 update, insert, delete에 의한 결과라는 의미
		if (!pStatement->HasResult()) 
			return TRUE;

		// 아래부터는 select결과에 대한 데이터 이므로 결과에 맞게 처리한다.
		pRowset->Set(*pStatement);		// 미리 생성한 rowset에 statement 할당.
		pRowset->Bind(TRUE);
		
		while (!pRowset->IsEnd())
			pRowset->Fetch();		// Fetch한 결과는 이전 결과에 축적되며
											// row count는 마지막 fetch된 넘 까지(쿼리의 총 결과수가 아님)

		// Callback 을 불러준다.
		if(pSQLBuffer->m_pfCallback)
			pSQLBuffer->m_pfCallback(pRowset, pSQLBuffer->m_pClass, pSQLBuffer->m_pData);

		pRowset->Close();
	}

	return FALSE;
}

UINT WINAPI AgsmIOCPDatabase::DBWorkerThread(PVOID pParam)
{
	AgsmIOCPDatabase	*pThis = (AgsmIOCPDatabase*)pParam;
	DWORD				dwTransferred	= 0;
	LPOVERLAPPED		pOverlapped		= NULL;
	SQLBuffer*			pSQLBuffer		= NULL;

	// DB에 관련된 변수들
	AuDatabase*		pConnection	= NULL;
	AuStatement*	pStatement	= NULL;
	AuRowset*		pRowset		= NULL;

	INT32 lEventIndex = pThis->m_DBEvents.GetIndex();
	ASSERT((INVALID_INDEX != lEventIndex) && "소유자가 비어있는 Index가 없거나 Event 생성에 실패했습니다.");

	// COM library는 각 쓰레드별로 초기화해줘야한당.
	CoInitialize(NULL);

	while (!pThis->CreateDBConnection(&pConnection, &pStatement, &pRowset))
	{
		// DB와의 연결이 완료되지 않으면 다시 연결 시도
	}

	printf("\n DB 연결 완료");

	// DB연결이 완료 되었음을 Event에 설정
	pThis->m_DBEvents.SetDBEvent(lEventIndex);
	
	while(TRUE)
	{
		if (pThis->GetStatus((PULONG_PTR)&pSQLBuffer, &dwTransferred, &pOverlapped, INFINITE))
		{
			// SQL 실행
			if (NULL == pSQLBuffer)
			{
				printf("\npSQL이 NULL이다.");
				continue;
			}

			std::auto_ptr<SQLBuffer> autoPtr(pSQLBuffer);
			printf("Execute Query Count : %d\n", InterlockedIncrement(&g_lExecuteCount));
			if (!pThis->ExecuteSQL(pStatement, pRowset, pSQLBuffer))
			{
				// DB와의 연결이 끊어진 경우인지 확인하고 만약 끊어졌다면 다시 연결을 재시도 한다.
				if (!pConnection->IsAlive())
					{
					/*
					pConnection->Close();
					pConnection->Open("", "", "");
					connection이 끊어졌으므로 현상태에서 pStatement는 invalid하다.
					delete하고 다시 CreateStatement(*pStatement)하던가
					로우셋과 유사하게 auDatabase에서 Set(pStatement)할 수 있도록 라이브러리를 수정해야한당..
					*/
					}
			}
		}
		else
		{
			printf("\nSQL 처리 에러 : %s", pSQLBuffer->Buffer);
		}
	}
	
	CoUninitialize();  // never reach... ^^;
	return TRUE;
}