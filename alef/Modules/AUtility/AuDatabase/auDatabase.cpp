/*====================================================================

	AuDatabase.cpp

====================================================================*/


#include <tchar.h>
#include <stdio.h>
#include "ApBase.h"

#include "AuDatabaseOLEDB.h"
#include "AuDatabaseODBC.h"
#include "AuDatabaseOCI.h"
#include "AuDatabase.h"
#include "ApMemoryTracker.h"
#include "AuCircularBuffer.h"

#pragma warning (disable : 4127)


#ifdef ORACLE_THROUGH_OCI
	BOOL	g_bNotUseOCI = FALSE;
#else
	BOOL	g_bNotUseOCI = TRUE;
#endif


/****************************************/
/*		The Implementation of class		*/
/****************************************/
//
BOOL AuEnvironment::m_bCaching = FALSE;


AuEnvironment::AuEnvironment(eAUDB_VENDER eVender)
	{
	m_eVender = eVender;
	ZeroMemory(m_szError, sizeof(m_szError));
	}


AuDatabase::AuDatabase()
	{
	ZeroMemory(m_szError, sizeof(m_szError));
	}


AuStatement::AuStatement()
	{
	ZeroMemory(m_szError, sizeof(m_szError));
	m_lAffectedRows = 0;
	}


INT32 AuStatement::GetAffectedRows()
	{
	return (INT32)m_lAffectedRows;
	}




/************************************************/
/*		The Implementation of Column class		*/
/************************************************/
//
/*
LPCTSTR AuColumn::Name()
	{
	return m_szName;
	}


eAuDatabaseType AuColumn::Type()
	{
	return m_eType;
	}


eAuDatabaseType AuColumn::BindType()
	{
	return m_eBindType;
	}


INT32 AuColumn::Precision()
	{
	return m_nPrecision;
	}


INT32 AuColumn::Scale()
	{
	return m_nScale;
	}
*/




/****************************************************/
/*		The Implementation of Rowset Base class		*/
/****************************************************/
//
AuRowset::AuRowset()
	: m_cRef(1), m_pStatement(NULL)
	{
	AuRowset::Close();
	}


AuRowset::~AuRowset()
	{
	}


UINT8 AuRowset::AddRef()
	{
	return ++m_cRef;
	}


UINT8 AuRowset::Release()
	{
	UINT8 ui = --m_cRef;
	if (0 == m_cRef)
		delete this;
	return ui;
	}


void AuRowset::Close()
	{
	m_Buffer.Reset();
	m_ulRows = 0;
	m_ulCols = 0;
	m_ulRowBufferSize = 0;
	m_bEnd = FALSE;
	}


stColumn& AuRowset::GetColumnInfo(UINT32 ulCol)
	{
	ASSERT(m_ulCols > ulCol);
	return m_stColumns[ulCol];
	}


void AuRowset::Set(AuStatement& Statement)
	{
	m_pStatement = &Statement;
	m_Buffer.Reset();
	}


void AuRowset::InitializeBuffer(DWORD dwSizeK)
	{
	dwSizeK *= 1024;
	m_Buffer.Alloc(dwSizeK);
	}


PVOID AuRowset::GetHeadPtr()
	{
	return m_Buffer.GetHead();
	}




/****************************************************/
/*		The Implementation of Heap Buffer class		*/
/****************************************************/
//
//AuCircularBuffer g_AuCircularBufferDB;


#ifdef _DEBUG
	DWORD AuDatabaseBuffer::m_dwInitialized = 0;
#endif


BOOL AuDatabaseBuffer::Initialize(DWORD dwSize)
	{
	dwSize *= (1024*1024);	// ... M-byte

	/*if(!g_AuCircularBufferDB.Init(dwSize))
	{
		printf("g_AuCircularBufferDB.Init(%d) fail \n", dwSize);
		return FALSE;
	}*/

  #ifdef _DEBUG
	m_dwInitialized = dwSize;
  #endif

	TRACE("Info : Succeeded to create Heap(Size:%ld) in AuDatabaseBuffer::Initialize()\n", dwSize);
	return TRUE;
	}


void AuDatabaseBuffer::Cleanup()
	{
	//g_AuCircularBufferDB.Destroy();
	}


AuDatabaseBuffer::AuDatabaseBuffer()
	{
	m_pBuffer = NULL;
	m_dwAlloc = 0;
	m_dwUsed = 0;

  #ifdef _DEBUG
	m_dwAllocCount = 0;
  #endif
  	}


AuDatabaseBuffer::~AuDatabaseBuffer()
	{
	Free();
	}

		
void AuDatabaseBuffer::Free()
	{
	if (NULL != m_pBuffer)
		{
			//g_AuCircularBufferDB.Free(m_pBuffer);
			delete[] m_pBuffer;
		}

	m_pBuffer = NULL;
	m_dwAlloc = 0;

  //#ifdef _DEBUG
	//printf("Info : Free. Count allocated = [%d]\n", m_dwAllocCount);
  //#endif

	}


BOOL AuDatabaseBuffer::Alloc(DWORD dwSize)
	{
	if (m_dwAlloc >= m_dwUsed + dwSize)
		return TRUE;
	else
		return RealAlloc(dwSize);
	}


BOOL AuDatabaseBuffer::RealAlloc(DWORD dwSize)
	{
	ASSERT((0 == m_dwInitialized) || (dwSize <= 0x7FFF8));

	TRACE("Info : RealAlloc. current requested size=[%ld], previous total=[%ld]\n", dwSize, m_dwAlloc);
	
	LPBYTE pBuffer = m_pBuffer;		// ... for ReAlloc
	if (NULL == m_pBuffer)
	{
		//m_pBuffer = (LPBYTE)g_AuCircularBufferDB.Alloc(dwSize);
		m_pBuffer = new BYTE[dwSize];
		if(m_pBuffer)
		{
			memset(m_pBuffer, 0, dwSize);
		}
	}
	else
	{
		//m_pBuffer = (LPBYTE)g_AuCircularBufferDB.Alloc(m_dwAlloc + dwSize);
		m_pBuffer = new BYTE[m_dwAlloc + dwSize];
		if(m_pBuffer)
		{
			memset(m_pBuffer, 0, m_dwAlloc + dwSize);
			memcpy(m_pBuffer, pBuffer, m_dwAlloc);
			//g_AuCircularBufferDB.Free(pBuffer);
			delete[] pBuffer;
		}
	}

	if (NULL == m_pBuffer)
		{
		m_pBuffer = pBuffer;	// ... restore
		TRACE("!!! Error : Failed to %s memory in AuDatabaseBuffer::Alloc()\n", m_pBuffer ? _T("reallocate") : _T("allocate"));
		return FALSE;
		}
	m_dwAlloc += dwSize;

  #ifdef _DEBUG
	m_dwAllocCount++;
  #endif

	return TRUE;
	}


LPBYTE AuDatabaseBuffer::GetHead()
	{
	return m_pBuffer;
	}


LPBYTE AuDatabaseBuffer::GetTail()
	{
	return m_pBuffer + m_dwUsed;
	}


void AuDatabaseBuffer::Reset()
	{
	m_dwUsed = 0;
	//if (m_pBuffer)
	//	ZeroMemory(m_pBuffer, m_dwAlloc);
	}


void AuDatabaseBuffer::AddUsed(DWORD dwUsed)
	{
	m_dwUsed += dwUsed;
	}




/************************************************************/
/*		The Implementation of AuDatabaseManager class		*/
/************************************************************/
//
AuDatabaseManager::AuDatabaseManager()
	: m_pEnvironment(NULL), m_bInit(FALSE)
	{
	}


AuDatabaseManager::~AuDatabaseManager()
	{
	if (m_bInit)
		Terminate();
	}


BOOL AuDatabaseManager::Initialize(eAUDB_API eApi, eAUDB_VENDER eVender)
	{
	m_eApi = eApi;
	m_eVender = eVender;

	if (m_bInit)
		{
		TRACE("!!! Warning : manager already initialized in AuDatabaseManager::Initialize()\n");
		return TRUE;
		}

	switch (m_eApi)
		{
		case AUDB_API_OLEDB :
			m_pEnvironment = new AuEnvOLEDB(eVender);
			if (NULL != m_pEnvironment && m_pEnvironment->Open())
				m_bInit = TRUE;
			break;
		case AUDB_API_ODBC :
			m_pEnvironment = new AuEnvODBC(eVender);
			if (NULL != m_pEnvironment && m_pEnvironment->Open())
				m_bInit = TRUE;
			break;
		case AUDB_API_OCI :
		  #ifdef ORACLE_THROUGH_OCI
			ASSERT(AUDB_VENDER_ORACLE == eVender);
			m_pEnvironment = new AuEnvOCI(eVender);
			if (NULL != m_pEnvironment && m_pEnvironment->Open())
				m_bInit = TRUE;
		  #endif
			break;
		}

	return m_bInit;
	}


void AuDatabaseManager::Terminate()
	{
	if (m_pEnvironment)
		{
		delete m_pEnvironment;
		m_pEnvironment = NULL;
		m_bInit = FALSE;
		}
	//TerminateHeap();
	}


BOOL AuDatabaseManager::InitializeHeap(DWORD dwSize)
	{
	return AuDatabaseBuffer::Initialize(dwSize);
	}


void AuDatabaseManager::TerminateHeap()
	{
	AuDatabaseBuffer::Cleanup();
	}


AuDatabase* AuDatabaseManager::CreateDatabase()
	{
	ASSERT(NULL != m_pEnvironment);

	AuDatabase *pDatabase = NULL;
	switch (m_eApi)
		{
		case AUDB_API_OLEDB :
			pDatabase = CreateDatabaseOLEDB();
			break;
		case AUDB_API_ODBC :
			pDatabase = CreateDatabaseODBC();
			break;
		case AUDB_API_OCI :
		  #ifdef ORACLE_THROUGH_OCI
			pDatabase = CreateDatabaseOCI();
		  #endif
			break;
		}

	return pDatabase;
	}


AuStatement* AuDatabaseManager::CreateStatement(AuDatabase* pDatabase)
	{
	ASSERT(NULL != m_pEnvironment);
	
	AuStatement *pStatement = NULL;
	switch (m_eApi)
		{
		case AUDB_API_OLEDB :
			pStatement = CreateStatementOLEDB(pDatabase);
			break;
		case AUDB_API_ODBC :
			pStatement = CreateStatementODBC(pDatabase);
			break;
		case AUDB_API_OCI :
		  #ifdef ORACLE_THROUGH_OCI
			pStatement = CreateStatementOCI(pDatabase);
		  #endif
			break;
		}	

	return pStatement;
	}


AuRowset* AuDatabaseManager::CreateRowset(AuStatement* pStatement)
	{
	ASSERT(NULL != m_pEnvironment);
	
	AuRowset *pRowset = NULL;
	switch (m_eApi)
		{
		case AUDB_API_OLEDB :
			pRowset = CreateRowsetOLEDB(pStatement);
			break;
		case AUDB_API_ODBC :
			pRowset = CreateRowsetODBC(pStatement);
			break;
		case AUDB_API_OCI :
		  #ifdef ORACLE_THROUGH_OCI
			pRowset = CreateRowsetOCI(pStatement);
		  #endif
			break;
		}	

	return pRowset;
	}


//
//	==========	private	==========
//
AuDatabase* AuDatabaseManager::CreateDatabaseOLEDB()
	{
	ASSERT(NULL != m_pEnvironment);
	AuDatabase* pDatabase = new AuDatabaseOLEDB(*m_pEnvironment);
	return pDatabase;
	}


AuStatement* AuDatabaseManager::CreateStatementOLEDB(AuDatabase* pDatabase)
	{
	ASSERT(NULL != m_pEnvironment);
	AuStatement* pStatement = new AuStmtOLEDB(*pDatabase);
	return pStatement;
	}


AuRowset* AuDatabaseManager::CreateRowsetOLEDB(AuStatement* pStatement)
	{
	ASSERT(NULL != m_pEnvironment);
	AuRowset* pRowset ;
	if (NULL == pStatement)
		pRowset = new AuRowsetOLEDB();
	else
		pRowset = new AuRowsetOLEDB(*pStatement);
	return pRowset;
	}


AuDatabase* AuDatabaseManager::CreateDatabaseODBC()
	{
	ASSERT(NULL != m_pEnvironment);
	AuDatabase* pDatabase = new AuDatabaseODBC(*m_pEnvironment);
	return pDatabase;
	}


AuStatement* AuDatabaseManager::CreateStatementODBC(AuDatabase *pDatabase)
	{
	ASSERT(NULL != m_pEnvironment);
	AuStatement* pStatement = new AuStmtODBC(*pDatabase);
	return pStatement;
	}


AuRowset* AuDatabaseManager::CreateRowsetODBC(AuStatement *pStatement)
	{
	ASSERT(NULL != m_pEnvironment);
	AuRowset* pRowset ;
	if (NULL == pStatement)
		pRowset = new AuRowsetODBC();
	else
		pRowset = new AuRowsetODBC(*pStatement);
	return pRowset;
	}


AuDatabase* AuDatabaseManager::CreateDatabaseOCI()
	{
	ASSERT(NULL != m_pEnvironment);
	AuDatabase *pDatabase = NULL;
  #ifdef ORACLE_THROUGH_OCI
	pDatabase = new AuDatabaseOCI(*m_pEnvironment);
  #endif
	return pDatabase;
	}


AuStatement* AuDatabaseManager::CreateStatementOCI(AuDatabase *pDatabase)
	{
	ASSERT(NULL != m_pEnvironment);
	AuStatement *pStatement = NULL;
  #ifdef ORACLE_THROUGH_OCI	
	pStatement = new AuStmtOCI(*pDatabase);
  #endif
	return pStatement;
	}


AuRowset* AuDatabaseManager::CreateRowsetOCI(AuStatement *pStatement)
	{
	ASSERT(NULL != m_pEnvironment);
	AuRowset *pRowset = NULL;
  #ifdef ORACLE_THROUGH_OCI
	pRowset = new AuRowsetOCI(*pStatement);
  #endif
	return pRowset;
	}
