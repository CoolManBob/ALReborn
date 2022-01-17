/*=================================================================

	AuDatabase2.cpp


=================================================================*/


#include "ApBase.h"
#include "AuDatabase2.h"
#include "ApMemoryTracker.h"


/********************************************************/
/*		The Implementation of Simple Database class		*/
/********************************************************/
//
AuDatabase2::AuDatabase2()
	{
	//Init
	m_pManager = NULL;
	m_pDatabase = NULL;
	m_pStatement = NULL;
	m_pRowset = NULL;
	
	m_ulCurrentRow = 0;
	
	m_hSemaphore = NULL;
	}


AuDatabase2::~AuDatabase2()
	{
	Terminate();
	}




//	Init/Term
//================================================
//
BOOL AuDatabase2::Initialize(AuDatabaseManager *pManager)
	{
	if (!pManager)
		return FALSE;

	m_pManager = pManager;
	m_pDatabase = m_pManager->CreateDatabase();
	if (!m_pDatabase)
		return FALSE;
		
	return TRUE;
	}


BOOL AuDatabase2::Connect(CHAR *pszUser, CHAR *pszPassword, CHAR *pszDatasource)
	{
	// connect to database
	if (!m_pDatabase->Open(pszDatasource, pszUser, pszPassword))
		return FALSE;

	// alloc statement	
	m_pStatement = m_pManager->CreateStatement(m_pDatabase);
	if (!m_pStatement || !m_pStatement->Open())
		return FALSE;

	// pre-alloc rowset	
	m_pRowset = m_pManager->CreateRowset(m_pStatement);
	if (!m_pRowset)
		return FALSE;

	m_pRowset->InitializeBuffer(64);		// 64k pre-alloc	
	
	return TRUE;
	}


void AuDatabase2::Terminate()
	{
	m_ulCurrentRow = 0;
	
	//release all
	if (m_pRowset)
		{
		m_pRowset->Release();
		m_pRowset = NULL;
		}
	
	if (m_pStatement)
		{
		delete m_pStatement;
		m_pStatement = NULL;
		}
		
	if (m_pDatabase)
		{
		delete m_pDatabase;
		m_pDatabase = NULL;
		}
	
	if (m_hSemaphore)
		{
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
		}
	}




//	Query
//======================================
//
BOOL AuDatabase2::SetQueryText(CHAR *pszQuery)
	{
	return (m_pStatement && m_pStatement->Prepare(pszQuery));
	}


INT8 AuDatabase2::ExecuteQuery()
	{
	m_ulCurrentRow = 0;
	// execute
	if (!m_pStatement->Execute())
		return AUDATABASE2_QR_FAIL;
		
	if (m_pStatement->HasResult())
		{
		m_pRowset->Set(*m_pStatement);
		m_pRowset->Bind(TRUE);
		while (!m_pRowset->IsEnd())
			{
			if (!m_pRowset->Fetch())
				return AUDATABASE2_QR_FAIL;
			}

		if (0 == m_pRowset->GetRowCount())
			return AUDATABASE2_QR_NO_RESULT;
		else
			return AUDATABASE2_QR_SUCCESS;
		}
	else
		{
		if (0 > m_pStatement->GetAffectedRows())
			return AUDATABASE2_QR_NO_ROW_UPDATE;
		else
			return AUDATABASE2_QR_SUCCESS;
		}
	}


BOOL AuDatabase2::GetNextRow()
	{
	++m_ulCurrentRow;
	return (m_ulCurrentRow < m_pRowset->GetRowCount());
	}


CHAR* AuDatabase2::GetQueryResult(UINT32 ulColumn)	
	{
	if (!m_pRowset)
		return FALSE;
	
	CHAR *psz = NULL;
	
	if (m_ulCurrentRow < m_pRowset->GetRowCount() && ulColumn < m_pRowset->GetColCount())
		psz = (CHAR *) m_pRowset->Get(m_ulCurrentRow, ulColumn);
	
	return psz;
	}

BOOL AuDatabase2::GetQueryINT32(UINT32 ulColumn, INT32* desc)
{
	CHAR *psz = GetQueryResult(ulColumn);

	if(!psz)
		return FALSE;
	
	*desc = (INT32) atoi(psz);

	return TRUE;
}

BOOL AuDatabase2::GetQueryUINT32(UINT32 ulColumn, UINT32* desc)
{
	CHAR *psz = GetQueryResult(ulColumn);

	if(!psz)
		return FALSE;

	*desc = (UINT32)atoi(psz);

	return TRUE;
}

BOOL AuDatabase2::GetQueryDWORD(UINT32 ulColumn, DWORD* desc)
{
	CHAR *psz = GetQueryResult(ulColumn);

	if(!psz)
		return FALSE;

	*desc = (DWORD) atol(psz);

	return TRUE;
}

UINT32 AuDatabase2::GetCurrentColumns()
	{
	return m_pRowset ?	m_pRowset->GetColCount() : 0;
	}


UINT32 AuDatabase2::GetReadRows()
	{
	return m_pRowset ?	m_pRowset->GetRowCount() : 0;
	}


void AuDatabase2::EndQuery()
	{
	if(!m_pRowset || !m_pStatement)
		return;
	
	m_ulCurrentRow = 0;
	m_pRowset->Close();
	m_pStatement->ResetParameters();
	}




//	Transaction
//==================================
//
BOOL AuDatabase2::StartTranaction()
	{
	return (m_pDatabase && m_pDatabase->BeginTransaction());
	}


BOOL AuDatabase2::CommitTransaction()
	{
	return (m_pDatabase && m_pDatabase->EndTransaction(TRUE));
	}


BOOL AuDatabase2::AbortTransaction()
	{
	return (m_pDatabase && m_pDatabase->EndTransaction(FALSE));
	}




//	Bind Variable
//=============================================
//
BOOL AuDatabase2::SetParam(INT16 nIndex, CHAR *psz, INT32 lSize, BOOL bOut)
	{
	return (m_pStatement && m_pStatement->SetParam(nIndex, psz, lSize, bOut));
	}


BOOL AuDatabase2::SetParam(INT16 nIndex, UINT32 *pul, BOOL bOut)
	{
	return (m_pStatement && m_pStatement->SetParam(nIndex, pul, bOut));
	}


BOOL AuDatabase2::SetParam(INT16 nIndex, INT32 *pl, BOOL bOut)
	{
	return (m_pStatement && m_pStatement->SetParam(nIndex, pl, bOut));
	}

BOOL AuDatabase2::SetParam(INT16 nIndex, INT64 *pn, BOOL bOut)
{
	return (m_pStatement && m_pStatement->SetParam(nIndex, pn, bOut));
}


BOOL AuDatabase2::SetParam(INT16 nIndex, INT8 *pc, BOOL bOut)
	{
	return (m_pStatement && m_pStatement->SetParam(nIndex, pc, bOut));
	}


BOOL AuDatabase2::SetParam(INT16 nIndex, INT16 *pn, BOOL bOut)
	{
	return (m_pStatement && m_pStatement->SetParam(nIndex, pn, bOut));
	}




//
//====================================
//
void AuDatabase2::InitializeSemaphore()
	{
	m_hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	}


void AuDatabase2::ReleaseSemaphore()
	{
	if (m_hSemaphore)
		::ReleaseSemaphore(m_hSemaphore, 1, NULL);
	}




//
//====================================
//
eAUDB_VENDER AuDatabase2::GetVender()
	{
	if (m_pManager)
		return m_pManager->m_eVender;
	
	return AUDB_VENDER_ORACLE;
	}



