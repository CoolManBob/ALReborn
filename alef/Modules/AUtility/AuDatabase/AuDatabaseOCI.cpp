/*=================================================================

	AuDatabaseOCI.cpp

=================================================================*/

#include <tchar.h>
#include <stdio.h>
#include "ApBase.h"
#include "AuDatabaseOCI.h"
#include "ApMemoryTracker.h"

#ifdef ORACLE_THROUGH_OCI

#pragma comment (lib, "oci.lib")


/****************************************************/
/*		The Implementation of AuEnvOCI	class		*/
/****************************************************/
//
AuEnvOCI::AuEnvOCI(eAUDB_VENDER eVender)
	: AuEnvironment(eVender)
	{
	m_bCaching = TRUE;
	}


AuEnvOCI::~AuEnvOCI()
	{
	Close();
	}


//	Open/Close
//=========================================
//
BOOL AuEnvOCI::Open()
	{
	// initialize
	if (OCI_SUCCESS != OCIInitialize((ub4) OCI_THREADED/*OCI_THREADED|OCI_SHARED*/, NULL, NULL, NULL, NULL))
		{
		return FALSE;
		}

	return TRUE;
	}


void AuEnvOCI::Close()
	{
	}


//
//=========================================
//
BOOL AuEnvOCI::_AllocEnv(OCIEnv **pphEnvironment, OCIError **pphError)
	{	
	if (OCI_SUCCESS != OCIEnvInit((OCIEnv **) pphEnvironment, (ub4) OCI_DEFAULT,
									(size_t) 0, (dvoid **) 0)
		)
		{
		return FALSE;
		}									

	if (OCI_SUCCESS != OCIHandleAlloc((dvoid *) *pphEnvironment, (dvoid **) pphError,
										(ub4) OCI_HTYPE_ERROR, (size_t) 0,
										(dvoid **) 0)
		)
		{
		OCIHandleFree((dvoid *) *pphEnvironment, (ub4) OCI_HTYPE_ENV);
		*pphEnvironment = NULL;
		return FALSE;
		}

	return TRUE;
	}


void AuEnvOCI::_FreeEnv(OCIEnv *phEnvironment, OCIError *phError)
	{
	if (phError)
		{
		OCIHandleFree((dvoid *) phError, (ub4) OCI_HTYPE_ERROR);	
		phError = NULL;
		}
	
	if (phEnvironment)
		{
		OCIHandleFree((dvoid *) phEnvironment, (ub4) OCI_HTYPE_ERROR);
		phEnvironment = NULL;
		}
	}




/********************************************************/
/*		The Implementation of AuDatabaseOCI class		*/
/********************************************************/
//
AuDatabaseOCI::AuDatabaseOCI(AuEnvironment& Environment)
	{
	m_pEnvironment = (AuEnvOCI *) &Environment;
	m_phEnvironment = NULL;
	m_phError = NULL;
	m_phServer = NULL;
	m_phServiceContext = NULL;
	m_phSession = NULL;
	m_phError = NULL;
	}


AuDatabaseOCI::~AuDatabaseOCI()
	{
	Close();
	}


//	Pre-connection
//===========================================
//
BOOL AuDatabaseOCI::_AllocServer(CHAR *pszDatasource)
	{
	// allocate server
	if (OCI_SUCCESS != OCIHandleAlloc((dvoid *) m_phEnvironment, (dvoid **) &m_phServer, (ub4) OCI_HTYPE_SERVER,
										(size_t) 0, (dvoid **) 0)
		)
		{
		return FALSE;
		}

	// attach
	if (OCI_SUCCESS != OCIServerAttach(m_phServer, m_phError, (text *)pszDatasource, (sb4)strlen(pszDatasource), OCI_DEFAULT))
		{
		OCIHandleFree((dvoid *) m_phServer, (ub4) OCI_HTYPE_SERVER);
		m_phServer = NULL;
		return FALSE;
		}
	
	return TRUE;
	}


void AuDatabaseOCI::_FreeServer()
	{
	if (m_phServer)
		{
		OCIServerDetach(m_phServer, m_phError, OCI_DEFAULT);
		OCIHandleFree((dvoid *) m_phServer, (ub4) OCI_HTYPE_SERVER);
		m_phServer = NULL;
		}
	}


BOOL AuDatabaseOCI::_AllocContext()
	{
	// service context
	if (OCI_SUCCESS != OCIHandleAlloc((dvoid *) m_phEnvironment, (dvoid **) &m_phServiceContext,
										(ub4) OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0))
		return FALSE;

	// Set the server handle in service handle
	if (OCI_SUCCESS != _Set(m_phServiceContext, OCI_HTYPE_SVCCTX, m_phServer, 0, OCI_ATTR_SERVER, m_phError))
		{
		_FreeContext();
		return FALSE;
		}
	
	return TRUE;
	}


void AuDatabaseOCI::_FreeContext()
	{
	if (m_phServiceContext)
		{
		OCIHandleFree(m_phServiceContext, (ub4) OCI_HTYPE_SVCCTX);
		m_phServiceContext = NULL;
		}
	}


BOOL AuDatabaseOCI::_AllocSession()
	{
	// session
	if (OCI_SUCCESS != OCIHandleAlloc((dvoid *) m_phEnvironment, (dvoid **) &m_phSession, (ub4) OCI_HTYPE_SESSION,
										(size_t) 0, (dvoid **) 0))
		return FALSE;

	return TRUE;
	}


void AuDatabaseOCI::_FreeSession()
	{
	// session
	if (m_phSession)
		{
		OCIHandleFree((dvoid *) m_phSession, (ub4) OCI_HTYPE_SESSION);
		m_phSession = NULL;
		}
	}


//	Connection method
//===============================================
//
BOOL AuDatabaseOCI::Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword)
	{
	// environment
	if (!m_pEnvironment->_AllocEnv(&m_phEnvironment, &m_phError))
		return FALSE;
	
	// server
	if (!_AllocServer((CHAR *) pszDatasource) ||
		!_AllocContext() ||
		!_AllocSession()
		)
		{
		AuGetErrorInfoOCI(m_phError, m_szError, sizeof(m_szError));
		return FALSE;
		}
	
	// user
	if (OCI_SUCCESS != _Set(m_phSession, OCI_HTYPE_SESSION, (dvoid *)pszUser, (sb4)strlen(pszUser), OCI_ATTR_USERNAME, m_phError))
		{
		return FALSE;
		}
		
	// password
    if (OCI_SUCCESS != _Set(m_phSession, OCI_HTYPE_SESSION, (dvoid *)pszPassword, (sb4)strlen(pszPassword), OCI_ATTR_PASSWORD, m_phError))
		{
		return FALSE;
		}

	// check statement caching
	ub4 ulMode = OCI_DEFAULT;
	if (AuEnvironment::IsCaching())
		ulMode = OCI_DEFAULT | OCI_STMT_CACHE;
	
	// session begin
	if (OCI_SUCCESS != OCISessionBegin(m_phServiceContext, m_phError, m_phSession, OCI_CRED_RDBMS, ulMode))
		{
		return FALSE;
		}

	// set user session to context
	if (OCI_SUCCESS != OCIAttrSet(m_phServiceContext, OCI_HTYPE_SVCCTX, m_phSession, (ub4) 0, OCI_ATTR_SESSION, m_phError))
		{
		return FALSE;
		}

	TRACE(_T("Info : Connection succeeded in AuDatabaseOCI::Open()\n"));
	return TRUE;
	}


BOOL AuDatabaseOCI::Open(LPCTSTR pszConnectionString)
	{
	ASSERT(!_T("!!! Error : not implemented\n"));
	return FALSE;
	}


void AuDatabaseOCI::Close()
	{
	if (m_phSession)
		OCISessionEnd(m_phServiceContext, m_phError, m_phSession, (ub4) 0);
	
	_FreeSession();
	_FreeContext();
	_FreeServer();
	m_pEnvironment->_FreeEnv(m_phEnvironment, m_phError);
	}


//	Transaction Method
//========================================================
//
BOOL AuDatabaseOCI::BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation)
	{
	return TRUE;
	/*
	OCITrans *m_phTransaction = NULL;
	OCIHandleAlloc(m_phEnvironment, (dvoid **) &m_phTransaction, OCI_HTYPE_TRANS, 0, 0);
	OCIAttrSet(m_phServiceContext, OCI_HTYPE_SVCCTX, m_phTransaction, 0, OCI_ATTR_TRANS, m_phError);

	XID gxid;

	gxid.formatID = 1000;
	gxid.gtrid_length = 3;
	gxid.data[0] = 1; gxid.data[1] = 2; gxid.data[2] = 3;
	gxid.bqual_length = 1;
	gxid.data[3] = 1;
	OCIAttrSet(m_phTransaction, OCI_HTYPE_TRANS, NULL, 0, OCI_ATTR_XID, m_phError);

	
	if (OCI_SUCCESS == OCITransStart(m_phServiceContext, m_phError, 60, OCI_TRANS_NEW))
		return TRUE;
	return FALSE;
	*/
	}


BOOL AuDatabaseOCI::EndTransaction(BOOL bCommit)
	{
	sword r;
	if (bCommit)
		r = OCITransCommit(m_phServiceContext, m_phError, OCI_DEFAULT);
	else
		r = OCITransRollback(m_phServiceContext, m_phError, OCI_DEFAULT);
	
	return (OCI_SUCCESS == r);
	}


//	Status Method
//=====================================================
//
BOOL AuDatabaseOCI::IsAlive()
	{
	ub4 ulStatus = 0;
	_Get((dvoid *)m_phServer, OCI_HTYPE_SERVER, (dvoid *)&ulStatus, (ub4 *)0, OCI_ATTR_SERVER_STATUS, m_phError);
	if (OCI_SERVER_NORMAL == ulStatus)
		return TRUE;

	return FALSE;
	}


//	Statement Handle Method
//====================================================
//
BOOL AuDatabaseOCI::_AllocSTMT(OCIStmt **pphStatement)
	{
	if (AuEnvironment::IsCaching())
		return TRUE;

	if (OCI_SUCCESS != OCIHandleAlloc((dvoid *) m_phEnvironment, (dvoid **) pphStatement,
					(ub4) OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0))
		return FALSE;
	
	return TRUE;
	}


void AuDatabaseOCI::_FreeSTMT(OCIStmt *phStatement)
	{
	OCIHandleFree((dvoid *) phStatement, (ub4) OCI_HTYPE_STMT);
	}




/********************************************************/
/*		The Implementationof OCI Statement class		*/
/********************************************************/
//
AuStmtOCI::AuStmtOCI(AuDatabase& Database)
	{
	m_pDatabase = (AuDatabaseOCI *)&Database;
	m_phStatement = NULL;
	m_phError = NULL;
	m_ulMode = OCI_DEFAULT;
	m_ulCols = 0;
	m_nParams = 0;
	}


AuStmtOCI::~AuStmtOCI()
	{
	Close();
	}


//	Open/Close
//===========================================================
//
BOOL AuStmtOCI::Open()
	{
	ASSERT(NULL != m_pDatabase);
	if (!m_pDatabase->_AllocSTMT(&m_phStatement))
		{
		AuGetErrorInfoOCI(m_pDatabase->m_phError, m_szError, sizeof(m_szError));
		return FALSE;
		}

	m_phError = m_pDatabase->m_phError;

	return TRUE;
	}


void AuStmtOCI::Close()
	{
	if (m_phStatement)
		{
		m_pDatabase->_FreeSTMT(m_phStatement);
		m_phStatement = NULL;
		}
	}


//	Execute
//==========================================================
//
//	Execute Query. if DML can get count of affected rows
//
BOOL AuStmtOCI::Execute(LPCTSTR pszQuery)
	{
	if (Prepare(pszQuery) && Execute())
		return TRUE;

	return FALSE;
	}


//	Prepare
//=====================================================================
//
//	Prepare query for dynamic parameter binding
//
/*
BOOL AuStmtOCI::Prepare(LPCTSTR pszQuery)
	{
	if (OCI_SUCCESS != OCIStmtPrepare(m_phStatement, m_phError, (text *) pszQuery, (ub4) strlen(pszQuery),
										(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
		{
		AuGetErrorInfoOCI(m_phError, m_szError, sizeof(m_szError));
		return FALSE;
		}
	
	// no. of params
	sb4	lParams = 0;
	OCIStmtGetBindInfo(m_phStatement, m_phError, AUDB_MAX_COLUMN_SIZE, 1, &lParams,
					   m_pszNames, m_ucNameSizes, m_pszInd, m_ucIndSizes, m_ucDupl, m_phBinds);

	m_nParams = (INT16) lParams;
	
	return _DescribeParams();
	}
*/


BOOL AuStmtOCI::Prepare(LPCTSTR pszQuery)
	{
	if (NULL == pszQuery || _T('\0') == *pszQuery)
		return FALSE;
	
	if (!m_pDatabase)
		return FALSE;

	if (m_phStatement)
		{
		OCIStmtRelease(m_phStatement, m_phError, NULL, 0, OCI_DEFAULT);
		m_phStatement = NULL;
		}

	if (OCI_SUCCESS != OCIStmtPrepare2(m_pDatabase->m_phServiceContext, &m_phStatement, m_phError,
									  (OraText *) pszQuery, (ub4) strlen(pszQuery),
									  //(OraText *) pszKey, (ub4) strlen(pszKey),
									  NULL, 0,
									  (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
		{
		AuGetErrorInfoOCI(m_phError, m_szError, sizeof(m_szError));
		return FALSE;
		}
	

	// no. of params
	sb4	lParams = 0;
	OCIStmtGetBindInfo(m_phStatement, m_phError, AUDB_MAX_COLUMN_SIZE, 1, &lParams,
					   m_pszNames, m_ucNameSizes, m_pszInd, m_ucIndSizes, m_ucDupl, m_phBinds);

	m_nParams = (INT16) lParams;
	
	if (!_DescribeParams())
		{
		AuGetErrorInfoOCI(m_phError, m_szError, sizeof(m_szError));
		return FALSE;
		}
		
	return TRUE;
	}


//	DescribeParams
//=====================================================================
//
//	get parameter info. from DB
//
BOOL AuStmtOCI::_DescribeParams()
	{
	for (INT16 i=0; i < m_nParams; i++)
		{
		m_Params[i].Set(m_phStatement, m_phError, i);
		if (!m_Params[i].Describe())
			return FALSE;
		}
	
	return TRUE;
	}


//	SetParam
//=====================================================================
//
//	set parameter data ptr.
//
BOOL AuStmtOCI::SetParam(INT16 nIndex, PVOID pBuffer, INT32 lBufferSize, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(lBufferSize > 0);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, lBufferSize, SQLT_CHR);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, CHAR* pBuffer, INT32 lBufferSize, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(lBufferSize > 0);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, lBufferSize, SQLT_STR);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, INT8* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(INT8), SQLT_INT);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, INT16* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(INT16), SQLT_INT);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, INT32* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(INT32), SQLT_INT);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, INT64* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(!"Not supported");
	return FALSE;
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, UINT8* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(UINT8), SQLT_UIN);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, UINT16* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(UINT16), SQLT_UIN);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, UINT32* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(UINT32), SQLT_UIN);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, UINT64* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(!"Not supported");
	return FALSE;
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, FLOAT* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(FLOAT), SQLT_FLT);
	}


BOOL AuStmtOCI::SetParam(INT16 nIndex, double* pBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, sizeof(double), SQLT_FLT);
	}


//	ResetParameters
//=====================================================================
//
BOOL AuStmtOCI::ResetParameters()
	{
	return TRUE;
	}


//	Execute
//=====================================================================
//
//	!!! statement must prepared before
//
BOOL AuStmtOCI::Execute()
	{
	ub4 ulMaxIter = 0;
	ub4 ulType = OCI_STMT_SELECT;
	
	_Get(m_phStatement, OCI_HTYPE_STMT, (dvoid *) &ulType, 0, OCI_ATTR_STMT_TYPE, m_phError);
	
	if (OCI_STMT_SELECT != ulType)
		{
		ulMaxIter = 1;
		}
	
    if (OCI_SUCCESS != OCIStmtExecute(m_pDatabase->m_phServiceContext, m_phStatement, m_phError,
									  ulMaxIter, 0, NULL, NULL, m_ulMode)
		)
		{
		AuGetErrorInfoOCI(m_phError, m_szError, sizeof(m_szError));
		return FALSE;
		}

	// affected rows
	ub4 ulAffected = 0;
	_Get(m_phStatement, OCI_HTYPE_STMT, (dvoid *) &ulAffected, 0, OCI_ATTR_ROW_COUNT, m_phError);
	m_lAffectedRows = ulAffected;	


	// get column count
	ub4 ulColumns;
	_Get(m_phStatement, OCI_HTYPE_STMT, (dvoid *) &ulColumns, 0, OCI_ATTR_PARAM_COUNT, m_phError);
	m_ulCols = ulColumns;

	return TRUE;
	}


//	HasResult
//===========================================================
//
BOOL AuStmtOCI::HasResult()
	{
	return (m_ulCols > 0);
	}


//	Extra
//============================================================
//
void AuStmtOCI::SetTransactionMode(BOOL bInstantCommit)
	{
	if (bInstantCommit)
		m_ulMode = OCI_COMMIT_ON_SUCCESS;
	else
		m_ulMode = OCI_DEFAULT;
	}




/****************************************************/
/*		The Implementation of OCI Rowset Class		*/
/****************************************************/
//
AuRowsetOCI::AuRowsetOCI()
	{
	m_pStatement = NULL;
	m_pDBColumnInfo = NULL;
	m_pBindings = NULL;
	}


AuRowsetOCI::AuRowsetOCI(AuStatement& Statement)
	{
	m_pStatement = (AuStmtOCI *) &Statement;
	m_pDBColumnInfo = NULL;
	m_pBindings = NULL;
	}


AuRowsetOCI::~AuRowsetOCI()
	{
	Close();
	}


void AuRowsetOCI::Close()
	{
	if (NULL != m_pStatement)
		{
		//SQLFreeStmt(m_pStatement->m_hStatement, SQL_UNBIND);
		m_pStatement = NULL;
		}
	m_pDBColumnInfo = NULL;
	m_pBindings = NULL;
	
	AuRowset::Close();
	}


//	Bind
//===============================================================
//
//	Get column info and set binding info.
//
BOOL AuRowsetOCI::Bind(BOOL bBindToString)
	{
	UNUSED_PARA(bBindToString);

	if (NULL == m_pStatement || NULL == m_pStatement->m_phStatement)
		return FALSE;
	
	m_ulCols = m_pStatement->m_ulCols;

	// get column info
	m_pDBColumnInfo = m_Columns;
	for (ub4 i = 0; i < m_ulCols; i++)
		{
		if (!m_pDBColumnInfo[i].GetInfo(m_pStatement->m_phStatement, 
										m_pStatement->m_phError,
										(i+1)
										)
			)
			{
			AuGetErrorInfoOCI(m_pStatement->m_phError, m_szError, sizeof(m_szError));
			TRACE("!!! Error : Column describe failed in AuRowsetOCI::Bind()\n");
			return FALSE;
			}
		_tcscpy(m_stColumns[i].m_szName, m_pDBColumnInfo[i].Name());
		}

	//	set offset
	m_pBindings = m_Bindings;
	ULONG ulOffset = 0;
	for (ub4 i = 0; i < (ub4) m_ulCols; i++)
		{
		ulOffset += m_pBindings[i].Set(&m_pDBColumnInfo[i], ulOffset);
		}

	m_ulRowBufferSize = ulOffset;

	return TRUE;
	}


//	Fetch
//======================================================
//
//	Bind column value to buffer and fetch rows
//
BOOL AuRowsetOCI::Fetch(UINT16 uiFetchCount)
	{
	ULONG ulFetched = 0;
	ULONG ulFetchBufferSize = m_ulRowBufferSize * uiFetchCount;

	m_Buffer.Alloc(ulFetchBufferSize);
	LPBYTE pBuffer = m_Buffer.GetTail();

	ZeroMemory(pBuffer, ulFetchBufferSize);


	for (ULONG i = 0; i < m_ulCols; i++)
		{
		if (OCI_SUCCESS != OCIDefineByPos(m_pStatement->m_phStatement, &(m_pBindings[i].m_pDefine),
										  m_pStatement->m_phError, (i+1),
										  (dvoid *) m_pBindings[i].Value(pBuffer),
										  m_pBindings[i].MaxLen(), m_pBindings[i].Type(),
										  m_pBindings[i].Ind(pBuffer), /*(dvoid *) 0,*/ (ub2 *) 0,
										  (ub2 *) 0, OCI_DEFAULT
										  )
			)
			{
			AuGetErrorInfoOCI(m_pStatement->m_phError, m_szError, sizeof(m_szError));
			TRACE("!!! Error : Binding failed in AuRowsetOCI::Bind()\n");
			return FALSE;
			}


		if (OCI_SUCCESS != OCIDefineArrayOfStruct(m_pBindings[i].m_pDefine, m_pStatement->m_phError,
												  m_ulRowBufferSize, // m_pBindings[i].MaxLen(),
												  m_ulRowBufferSize, // sizeof(sb2),
												  0, 0
												  )
			)
			{
			AuGetErrorInfoOCI(m_pStatement->m_phError, m_szError, sizeof(m_szError));
			TRACE("!!! Error : Binding failed in AuRowsetOCI::Bind()\n");
			return FALSE;
			}
			
		}

	/*
	for (i = 0; i < m_ulCols; i++)
		{
		if (OCI_SUCCESS != OCIDefineArrayOfStruct(m_pBindings[i].m_pDefine, m_pStatement->m_phError,
												  m_ulRowBufferSize, // m_pBindings[i].MaxLen(),
												  m_ulRowBufferSize, // sizeof(sb2),
												  0, 0
												  )
			)
			{
			AuGetErrorInfoOCI(m_pStatement->m_phError, m_szError, sizeof(m_szError));
			TRACE("!!! Error : Binding failed in AuRowsetOCI::Bind()\n");
			return FALSE;
			}
		}
	*/
	
	BOOL bRet = FALSE;
	sb4 lCode = OCIStmtFetch(m_pStatement->m_phStatement, m_pStatement->m_phError,
							 (ub4) uiFetchCount, OCI_FETCH_NEXT, OCI_DEFAULT
							 );

	// fetched rows
	ub4 ulFetchedSize = sizeof(ub4);
	_Get(m_pStatement->m_phStatement, (ub4) OCI_HTYPE_STMT, (dvoid *)&ulFetched,
		 &ulFetchedSize, (ub4) OCI_ATTR_ROWS_FETCHED, m_pStatement->m_phError);

	switch (lCode)
		{
		case OCI_NO_DATA :
			if (ulFetched > 0)
				{
				m_Buffer.AddUsed(ulFetched * m_ulRowBufferSize);
				m_ulRows += ulFetched;				
				}
			m_bEnd = TRUE;
			bRet = TRUE;
			break;

		case OCI_SUCCESS_WITH_INFO:
			TRACE("!!! Warning : Succeeded with info. in AuRowsetOCI::Fetch()\n");

		case OCI_SUCCESS :
			m_Buffer.AddUsed(ulFetched * m_ulRowBufferSize);
			m_ulRows += ulFetched;
			bRet = TRUE;
			break;

		default:
			//	error
			TRACE("!!! Error : Fetch failed in AuRowsetOCI::Fetch()\n");
			AuGetErrorInfoOCI(m_pStatement->m_phError, m_szError, sizeof(m_szError));
			bRet = FALSE;
			break;
			}

	return bRet;
	}


BOOL AuRowsetOCI::IsEnd()
	{
	return m_bEnd;
	}


UINT32 AuRowsetOCI::GetOffset(UINT32 ulCol)
	{
	return m_pBindings[ulCol].ValueOffset();
	}


//	Value method
//================================================
//
PVOID AuRowsetOCI::Get(UINT32 ulRow, UINT32 ulCol)
	{
	//ASSERT(m_ulRows > ulRow);
	//ASSERT(m_ulCols > ulCol);
	if (ulRow >= m_ulRows || ulCol > m_ulCols)
		return NULL;
	
	
	// 스트링 바인딩만 사용하므로 empty string을 넘겨준다.
	// column type에 따라 바인딩할 경우 return NULL 해줘야한다.
	LPBYTE p = m_Buffer.GetHead() + ulRow * m_ulRowBufferSize;
	if (-1 == *(m_pBindings[ulCol].Ind(p)))
		return _T("");

	return 
		m_Buffer.GetHead() + (ulRow * m_ulRowBufferSize + /* column offset */m_pBindings[ulCol].ValueOffset());
	}


//	Extra 
//==================================================
//
//	set(reset) statement to preallocated rowset
//
void AuRowsetOCI::Set(AuStatement& Statement)
	{
	Close();	// ... reset
	m_pStatement = (AuStmtOCI *) &Statement;
	}




/****************************************************/
/*		The Implementation of OCI Column class		*/
/****************************************************/
//
AuColumnOCI::AuColumnOCI()
	{
	Initialize();
	}


void AuColumnOCI::Initialize()
	{
	ZeroMemory(m_szName, sizeof(m_szName));
	}


//	AuColumnOCI::GetInfo
//=========================================================
//
//	nIndex : 1-based index
//
BOOL AuColumnOCI::GetInfo(OCIStmt *phStatement, OCIError *phError, ub4 lIndex)
	{
	if (OCI_SUCCESS != OCIParamGet(phStatement, OCI_HTYPE_STMT, phError, (dvoid **) &m_phColumn, lIndex))
		return FALSE;
	
	// type
	_Get(m_phColumn, OCI_DTYPE_PARAM, (dvoid *) &m_unType, 0, OCI_ATTR_DATA_TYPE, phError);
	
	// name
	ub4 ulName = sizeof(m_szName);
	text *pszName;
	_Get(m_phColumn, OCI_DTYPE_PARAM, (dvoid **) &pszName, &ulName, OCI_ATTR_NAME, phError);
	ulName = min(ulName, AUDB_MAX_COLUMN_NAME_SIZE-1);
	_tcsncpy(m_szName, (LPCTSTR) pszName, ulName);
	m_szName[ulName] = _T('\0');

	// data size
	_Get(m_phColumn, OCI_DTYPE_PARAM, (dvoid **) &m_unColumnSize, 0, OCI_ATTR_DATA_SIZE, phError);
	
	// precision
	_Get(m_phColumn, OCI_DTYPE_PARAM, (dvoid **) &m_ucPrecision, 0, OCI_ATTR_PRECISION, phError);
	
	// scale
	_Get(m_phColumn, OCI_DTYPE_PARAM, (dvoid **) &m_cScale, 0, OCI_ATTR_SCALE, phError);

	return TRUE;
	}


LPCTSTR AuColumnOCI::Name()
	{
	return m_szName;
	}


ub2 AuColumnOCI::Type()
	{
	return m_unType;
	}


ub2 AuColumnOCI::ColumnSize()
	{
	return m_unColumnSize;
	}


ub1 AuColumnOCI::Precision()
	{
	return m_ucPrecision;
	}


sb1 AuColumnOCI::Scale()
	{
	return m_cScale;
	}




/****************************************************/
/*		The Implementation of OCI Binding class		*/
/****************************************************/
//
ULONG AuBindingOCI::Set(AuColumnOCI *pColumn, ULONG ulOffset)
	{
	m_ulOffset = ulOffset;
	m_unTargetType = SQLT_STR;
	
	switch (pColumn->Type())
		{
		case SQLT_CHR :
		case SQLT_AFC :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize() + 1);
			break;
		case SQLT_DAT :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_DATETIME_SIZE);
			break;
		case SQLT_INT :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize());
			break;
		case SQLT_UIN :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize());
			break;		
		
		
		//case SQLT_PDN :
		//case SQLT_BIN :
		case SQLT_NUM :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize());
			break;
		//case SQLT_NTY :
		//case SQLT_REF :

		case SQLT_FLT :
		default :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_COLUMN_VALUE_SIZE);
			break;
		}
			
	return (m_ulMaxLen + sizeof(sb2));	// OCI indicator var. is type of sb2
	}


ULONG AuBindingOCI::Set2(AuColumnOCI *pColumn, ULONG ulOffset)
	{
	m_ulOffset = ulOffset;
	m_unTargetType = SQLT_STR;

	switch (pColumn->Type())
		{
		case SQLT_CHR :
		case SQLT_DAT :
		case SQLT_INT :
		case SQLT_UIN :
		case SQLT_FLT :
		case SQLT_PDN :
		//case SQLT_BIN :
		case SQLT_NUM :
		//case SQLT_NTY :
		//case SQLT_REF :

		default :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_COLUMN_VALUE_SIZE);
			break;
		}

	return (m_ulMaxLen + sizeof(sb2));
	}


void* AuBindingOCI::Value(LPBYTE pBuffer)
	{
	return (void *)(pBuffer + m_ulOffset);
	}


ULONG AuBindingOCI::ValueOffset()
	{
	return m_ulOffset;
	}


sb2* AuBindingOCI::Ind(LPBYTE pBuffer)
	{
	return (sb2 *)(pBuffer + m_ulOffset + m_ulMaxLen);
	}


sb2	AuBindingOCI::Type()
	{
	return m_unTargetType;
	}


ULONG AuBindingOCI::MaxLen()
	{
	return m_ulMaxLen;
	}




/************************************************/
/*		The Implementation of OCI Parameter		*/
/************************************************/
//
void AuParamOCI::_Initialize()
	{
	m_phStatement = NULL;
	m_phError = NULL;
	m_phParam = NULL;
	m_nIndex = 0;
	}


void AuParamOCI::Set(OCIStmt *phStatement, OCIError *phError, INT16 nIndex)
	{
	m_phStatement = phStatement;
	m_phError = phError;
	m_nIndex = nIndex + 1;
	}


BOOL AuParamOCI::Describe()
	{
	return TRUE;
	}


BOOL AuParamOCI::Bind(PVOID pBuffer, sb4 lBufferSize, ub2 unType)
	{
	if (OCI_SUCCESS != OCIBindByPos(m_phStatement, &m_phParam, m_phError,
									(ub4) m_nIndex, (dvoid *) pBuffer, lBufferSize, unType,
									NULL, NULL, NULL, 0, NULL, OCI_DEFAULT)
		)
		{
		TCHAR sz[2048];
		AuGetErrorInfoOCI(m_phError, sz, sizeof(sz));
		return FALSE;
		}

	return TRUE;
	}




/****************************************************/
/*		The Implementation of Helper Function		*/
/****************************************************/
//
sword _Get(dvoid *phHandle, ub4 HandleType, dvoid *pAttribute, ub4 *pSize, ub4 Attribute, OCIError *phError)
	{
	return OCIAttrGet(phHandle, HandleType, pAttribute, pSize, Attribute, phError);
	}


sword _Set(dvoid *phHandle, ub4 HandleType, dvoid *pAttribute, ub4 Size, ub4 Attribute, OCIError *phError)
	{
	return OCIAttrSet(phHandle, HandleType, pAttribute, Size, Attribute, phError);
	}


void AuGetErrorInfoOCI(OCIError *phError, CHAR *pszError, size_t size)
	{
	sb4		ErrorCode = 0;
	OCIErrorGet((dvoid *) phError, (ub4) 1, (text *) NULL, &ErrorCode,
				(oratext *) pszError, (ub4) size, (ub4) OCI_HTYPE_ERROR);
	
	}


#endif		// ORACLE_THROUGH_OCI