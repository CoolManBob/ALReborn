/*=================================================================

	AuODBC.cpp

=================================================================*/

#include <tchar.h>
#include <stdio.h>
#include "ApBase.h"

#include "AuDatabaseODBC.h"
#include "ApMemoryTracker.h"


#pragma warning (disable : 4127)		// disable ASSERT macro warning
#pragma comment (lib, "ODBC32")


/****************************************************/
/*		The Implementation of AuEnvODBC	class		*/
/****************************************************/
//
AuEnvODBC::AuEnvODBC(eAUDB_VENDER eVender)
	: AuEnvironment(eVender)
	{
	m_hEnvironment = SQL_NULL_HENV;
	}


AuEnvODBC::~AuEnvODBC()
	{
	Close();
	}


//	Open/Close
//=========================================
//
BOOL AuEnvODBC::Open()
	{
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnvironment);
	if (SQL_NULL_HENV == m_hEnvironment)
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_ENV, m_hEnvironment);
		TRACE("!!! Error : Environment handle allocation failed in AuEnvODBC::Open()\n");
		return FALSE;
		}
	
	if (SQL_SUCCESS != SQLSetEnvAttr(m_hEnvironment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_ENV, m_hEnvironment);
		TRACE("!!! Error : Set ODBC version failed in AuEnvODBC::Open()\n");
		return FALSE;
		}
	return TRUE;
	}


void AuEnvODBC::Close()
	{
	if (SQL_NULL_HENV != m_hEnvironment)
		{
		if (!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvironment)))
			TRACE("!!! Error : Environment handle deallocation failed in AuEnvODBC::Close()\n");
		m_hEnvironment = SQL_NULL_HENV;
		}
	}


//	Connection Handle Method
//===========================================
//
//	Allocate/Deallocate Connection Handle Only(doesn't real connect)
//
SQLHDBC	AuEnvODBC::_AllocConn()
	{
	//if (!IsOpen())
	if (SQL_NULL_HENV == m_hEnvironment)
		{
		TRACE("!!! Error : Connection handle allocation is not allowed before environment handle allocated in AuEnvODBC::AllocConn()\n");
		return SQL_NULL_HDBC;
		}

	SQLHDBC dbc = SQL_NULL_HDBC;
	if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, m_hEnvironment, &dbc)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_ENV, m_hEnvironment);
		TRACE("!!!Error : Connection handle allocation failed in AuEnvODBC::AllocConn()\n");
		}
	return dbc;
	}


void AuEnvODBC::_FreeConn(SQLHDBC hConnection)
	{
	if (!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_DBC, hConnection)))
		{
		TRACE("!!! Error : Connection handle deallocation failed in AuEnvODBC::FreeConn()\n");
		}
	}




/********************************************************/
/*		The Implementation of AuDatabaseODBC class		*/
/********************************************************/
//
AuDatabaseODBC::AuDatabaseODBC(AuEnvironment& Environment)
	{
	m_pEnvironment = (AuEnvODBC*) &Environment;
	m_hConnection = SQL_NULL_HDBC;
	}


AuDatabaseODBC::~AuDatabaseODBC()
	{
	Close();
	}


//	Open
//===============================================
//
//	Open(Connect)/Close(Disconnect)
//
BOOL AuDatabaseODBC::Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword)
	{
	m_hConnection = m_pEnvironment->_AllocConn();
	if (SQL_NULL_HDBC == m_hConnection)
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_ENV, m_pEnvironment->m_hEnvironment);
		return FALSE;
		}

	switch (SQLConnect(m_hConnection, (SQLTCHAR *) pszDatasource, SQL_NTS,
										(SQLTCHAR *) pszUser, SQL_NTS,
										(SQLTCHAR *) pszPassword, SQL_NTS ))
		{
		case SQL_SUCCESS_WITH_INFO:
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
			TRACE("!!! Warning : Connection succeeded with information in AuDatabaseODBC::Open()\n");
		case SQL_SUCCESS:
			break;
		default:
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
			TRACE("!!! Error : Connection failed in AuDatabaseODBC::Open()\n");
			m_pEnvironment->_FreeConn(m_hConnection);
			m_hConnection = SQL_NULL_HDBC;
			return FALSE;
		}

	TRACE("Info : Connection succeeded in ODBCConnection::Open()\n");
	return TRUE;
	}


BOOL AuDatabaseODBC::Open(LPCTSTR pszConnectionString)
	{
	m_hConnection = m_pEnvironment->_AllocConn();
	if (SQL_NULL_HDBC == m_hConnection)
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_ENV, m_pEnvironment->m_hEnvironment);
		return FALSE;
		}

	TCHAR szStringOut[512];
	SQLSMALLINT nStringIn = (SQLSMALLINT) _tcslen(pszConnectionString);
	SQLSMALLINT nStringOut;

	switch (SQLDriverConnect(m_hConnection, NULL, (SQLTCHAR *)pszConnectionString, nStringIn,
							 (SQLTCHAR *)szStringOut, sizeof(szStringOut),
							 &nStringOut, SQL_DRIVER_NOPROMPT))
		{
		case SQL_SUCCESS_WITH_INFO:
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
			TRACE("!!! Warning : Connection succeeded with information in AuDatabaseODBC::Open()\n");
		case SQL_SUCCESS:
			break;
		default:
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
			TRACE("!!! Error : Connection failed in AuDatabaseODBC::Open()\n");
			m_pEnvironment->_FreeConn(m_hConnection);
			m_hConnection = SQL_NULL_HDBC;
			return FALSE;
		}

	TRACE("Info : Connection succeeded in ODBCConnection::Open()\n");
	return TRUE;
	}


void AuDatabaseODBC::Close()
	{
	if (SQL_NULL_HDBC == m_hConnection)
		{
		TRACE("!!! Warning : Connection isn't opened before in AuDatabaseODBC::Close()\n");
		return;
		}

	switch (SQLDisconnect(m_hConnection))
		{
		case SQL_SUCCESS_WITH_INFO :
		case SQL_SUCCESS :
			break;
		default:
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
			TRACE("!!! Error : Disconnection failed in AuDatabaseODBC::Close()\n");
			break;
		}

	m_pEnvironment->_FreeConn(m_hConnection);
	m_hConnection = SQL_NULL_HDBC;
	}


//	Transaction Method
//========================================================
//
BOOL AuDatabaseODBC::BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation)
	{
	UNUSED_PARA(eIsolation);

	// turn off auto-commit mode
	if (!SQL_SUCCEEDED(_Set(SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, 0)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
		TRACE("!!! Error : Begin Transaction failed in AuDatabaseODBC::BeginTransaction()\n");
		return FALSE;
		}
	return TRUE;
	}


BOOL AuDatabaseODBC::EndTransaction(BOOL bCommit)
	{
	if (!SQL_SUCCEEDED(SQLEndTran(SQL_HANDLE_DBC, m_hConnection, (short) (bCommit ? SQL_COMMIT : SQL_ROLLBACK))))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_hConnection);
		TRACE("!!! Error : End Transaction failed in AuDatabaseODBC::EndTransaction()\n");
		return FALSE;
		}

	// restore auto-commit mode
	_Set(SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);
	return TRUE;
	}


//	Status Method
//=====================================================
//
BOOL AuDatabaseODBC::IsAlive()
	{
	SQLUINTEGER Dead = SQL_CD_FALSE;
	_Get(SQL_ATTR_CONNECTION_DEAD, &Dead, 0, 0);
	return (SQL_CD_FALSE == Dead);
	}


//	Attribute Method
//=====================================================
//
SQLRETURN AuDatabaseODBC::_Get(SQLINTEGER Attribute, SQLPOINTER pValue, SQLINTEGER BufferLength, SQLINTEGER* pStringLength)
	{
	return SQLGetConnectAttr(m_hConnection, Attribute, pValue, BufferLength, pStringLength);
	}


SQLRETURN AuDatabaseODBC::_Set(SQLINTEGER Attribute, SQLPOINTER pValue, SQLINTEGER StringLength)
	{
	return SQLSetConnectAttr(m_hConnection, Attribute, pValue, StringLength);
	}


//	Statement Handle Method
//====================================================
//
SQLHSTMT AuDatabaseODBC::_AllocSTMT()
	{
	SQLHSTMT hStatement = SQL_NULL_HSTMT;
	if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, m_hConnection, &hStatement)))
		{
		TRACE("!!! Error : Statement handle allocation failed in AuDatabaseODBC::AllocSTMT()\n");
		}
	return hStatement;
	}


void AuDatabaseODBC::_FreeSTMT(SQLHDBC hStatement)
	{
	if (!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_STMT, hStatement)))
		{
		TRACE("!!! Warning : in AuDatabaseODBC::FreeSTMT()\n");
		}
	}




/********************************************************/
/*		The Implementationof ODBC Statement class		*/
/********************************************************/
//
AuStmtODBC::AuStmtODBC(AuDatabase& Database)
	{
	m_hStatement = SQL_NULL_HSTMT;
	m_pDatabase = (AuDatabaseODBC *)&Database;
	m_ulCols = 0;
	}


AuStmtODBC::~AuStmtODBC()
	{
	Close();
	}


//	Open/Close
//===========================================================
//
BOOL AuStmtODBC::Open()
	{
	ASSERT(NULL != m_pDatabase);
	if (NULL == m_pDatabase || SQL_NULL_HSTMT == (m_hStatement = m_pDatabase->_AllocSTMT()))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_DBC, m_pDatabase->m_hConnection);
		return FALSE;
		}
	return TRUE;
	}


void AuStmtODBC::Close()
	{
	if (SQL_NULL_HSTMT != m_hStatement)
		{
		SQLFreeStmt(m_hStatement, SQL_CLOSE);
		m_pDatabase->_FreeSTMT(m_hStatement);
		m_hStatement= SQL_NULL_HSTMT;
		}
	m_ulCols = 0;
	}


//	Execute
//==========================================================
//
//	Execute Query. if DML can get count of affected rows
//
BOOL AuStmtODBC::Execute(LPCTSTR pszQuery)
	{
	ASSERT (NULL != pszQuery);
	ResetParameters();
	SQLFreeStmt(m_hStatement, SQL_CLOSE);

	if (SQL_SUCCESS != SQLExecDirect(m_hStatement, (SQLTCHAR *) pszQuery, SQL_NTS))
		{	
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : Statement execution failed in AuStmtODBC::Execute()\n");
		TRACE("\t\tQuery = [%s]\n", pszQuery ? pszQuery : _T(""));
		return FALSE;
		}

	if (!SQL_SUCCEEDED(SQLRowCount(m_hStatement, &m_lAffectedRows)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : SQLRowCount failed in AuStmtODBC::Execute()\n");
		return FALSE;
		}

	SQLSMALLINT nCols;
	if (!SQL_SUCCEEDED(SQLNumResultCols(m_hStatement, &nCols)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : Column count failed in AuRowsetODBC::Bind()\n");
		return FALSE;
		}

	m_ulCols = nCols;

	return TRUE;
	}


//	Prepare
//=====================================================================
//
//	Prepare query for dynamic parameter binding
//
BOOL AuStmtODBC::Prepare(LPCTSTR pszQuery)
	{
	if (NULL == pszQuery || _T('\0') == *pszQuery)
		return FALSE;

	ResetParameters();
	SQLFreeStmt(m_hStatement, SQL_CLOSE);
	TRACE("Query=[%s]\n", pszQuery);

	// prepare
	if (SQL_SUCCESS != SQLPrepare(m_hStatement, (SQLTCHAR *) pszQuery, SQL_NTS))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : Statement preparation failed in AuStmtODBC::Prepare()\n");
		TRACE("\t\tQuery = [%s]\n", pszQuery ? pszQuery : _T(""));
		return FALSE;
		}

	// get no. of required param.
	m_nParams = 0;
	if (!SQL_SUCCEEDED(SQLNumParams(m_hStatement, &m_nParams)))
		{
		TRACE("!!! Error : Param count failed in AuStmtODBC::Prepare()\n");
		return FALSE;
		}
	TRACE("Info : Param count succeeded. params=[%d] in AuStmtODBC::Prepare()\n", m_nParams);
	
	if (FALSE == _DescribeParams())
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : Statement preparation failed in AuStmtODBC::Prepare()\n");
		TRACE("\t\tQuery = [%s]\n", pszQuery ? pszQuery : _T(""));	
		return FALSE;
		}
	
	return TRUE;
	}


//	DescribeParams
//=====================================================================
//
//	get parameter info. form DB
//
BOOL AuStmtODBC::_DescribeParams()
	{
	// describe parameters
	for (INT16 i=0; i < m_nParams; i++)
		{
		m_Params[i].Set(m_hStatement, i);
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
BOOL AuStmtODBC::SetParam(INT16 nIndex, PVOID pBuffer, INT32 lBufferSize, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pBuffer, lBufferSize, SQL_C_CHAR, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, CHAR* pszBuffer, INT32 lBufferSize, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pszBuffer, lBufferSize, SQL_C_CHAR, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, INT8* pcBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pcBuffer, sizeof(INT8), SQL_C_STINYINT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, INT16* pnBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pnBuffer, sizeof(INT16), SQL_C_SSHORT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, INT32* plBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(plBuffer, sizeof(INT32), SQL_C_SLONG, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, INT64* plBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(plBuffer, sizeof(INT64), SQL_C_SBIGINT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, UINT8* pucBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pucBuffer, sizeof(UINT8), SQL_C_UTINYINT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, UINT16* punBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(punBuffer, sizeof(UINT16), SQL_C_USHORT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, UINT32* pulBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pulBuffer, sizeof(UINT32), SQL_C_ULONG, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, UINT64* plBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(plBuffer, sizeof(UINT64), SQL_C_UBIGINT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, FLOAT* pfBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pfBuffer, sizeof(FLOAT), SQL_C_FLOAT, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


BOOL AuStmtODBC::SetParam(INT16 nIndex, double* pfBuffer, BOOL bOut)
	{
	ASSERT(nIndex < m_nParams);
	return m_Params[nIndex].Bind(pfBuffer, sizeof(double), SQL_C_DOUBLE, bOut ? SQL_PARAM_OUTPUT : SQL_PARAM_INPUT);
	}


//	ResetParameters
//=====================================================================
//
BOOL AuStmtODBC::ResetParameters()
	{
	if (!SQL_SUCCEEDED(SQLFreeStmt(m_hStatement, SQL_RESET_PARAMS)))
		{
		TRACE(_T("!!! Error : Reset Parameter failed in AuParamODBC::ReetParameters()\n"));
		return FALSE;
		}
	
	return TRUE;
	}


//	Execute
//=====================================================================
//
//	!!! statement must prepared before
//
BOOL AuStmtODBC::Execute()
	{
	if (SQL_SUCCESS != SQLExecute(m_hStatement))
		{	
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : Statement execution failed in AuStmtODBC::Execute()\n");
		return FALSE;
		}

	if (!SQL_SUCCEEDED(SQLRowCount(m_hStatement, &m_lAffectedRows)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : SQLRowCount failed in AuStmtODBC::Execute()\n");
		return FALSE;
		}

	SQLSMALLINT nCols;
	if (!SQL_SUCCEEDED(SQLNumResultCols(m_hStatement, &nCols)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_hStatement);
		TRACE("!!! Error : Column count failed in AuStmtODBC::Execute()\n");
		return FALSE;
		}

	m_ulCols = nCols;

	return TRUE;
	}


//	HasResult
//===========================================================
//
BOOL AuStmtODBC::HasResult()
	{
	return (m_ulCols > 0);
	}




/****************************************************/
/*		The Implementation of ODBC Parameter		*/
/****************************************************/
//
void AuParamODBC::_Initialize()
	{
	m_hStatement		= SQL_NULL_HANDLE;
	m_nIndex			= 0;
	m_nType				= 12;//SQL_UNKNOWN_TYPE;
	m_nSize				= 999;
	m_nDecimalDigits	= 0;
	m_nInd				= SQL_NTS;
	m_bNull				= FALSE;
	}


void AuParamODBC::Set(SQLHSTMT hStatement, SQLUSMALLINT nIndex)
	{
	_Initialize();
	m_hStatement = hStatement;
	m_nIndex = (SQLUSMALLINT)(nIndex + 1);
	}


BOOL AuParamODBC::Describe()
	{
	SQLSMALLINT nNull = 0;

	if (SQL_SUCCESS != SQLDescribeParam(m_hStatement, m_nIndex, &m_nType, &m_nSize, &m_nDecimalDigits, &nNull))
		{
		TRACE("!!! Error : SQLDescribeParam failed in AuParamODBC::Describe()\n");
		return FALSE;
		}

	m_bNull = (nNull == SQL_NULLABLE);
	return TRUE;
	}


BOOL AuParamODBC::Bind(PVOID pBuffer, INT32 lBufferSize, INT16 nValueType, INT16 nInOut)
	{
	if (SQL_SUCCESS != SQLBindParameter(m_hStatement, m_nIndex, nInOut, nValueType, m_nType, m_nSize,
										m_nDecimalDigits, pBuffer, lBufferSize, &m_nInd))
		{
		TRACE("!!! Error : SQLBindParameter failed in AuParamODBC::Bind()\n");
		return FALSE;
		}
	return TRUE;
	}




/************************************************/
/*		The Implementation of AuRowsetODBC		*/
/************************************************/
//
AuRowsetODBC::AuRowsetODBC()
	{
	m_pStatement = NULL;
	m_pDBColumnInfo = NULL;
	m_pBindings = NULL;
	}


AuRowsetODBC::AuRowsetODBC(AuStatement& Statement)
	{
	m_pDBColumnInfo = NULL;
	m_pBindings = NULL;
	m_pStatement = (AuStmtODBC *) &Statement;
	}


AuRowsetODBC::~AuRowsetODBC()
	{
	Close();
	}


void AuRowsetODBC::Close()
	{
	if (NULL != m_pStatement)
		{
		SQLFreeStmt(m_pStatement->m_hStatement, SQL_UNBIND);
		m_pStatement = NULL;
		}
	m_pDBColumnInfo = NULL;
	m_pBindings = NULL;
	
	AuRowset::Close();
	}


//	Bind
//===============================================================
//
//	Get column info and set bindding info.
//
BOOL AuRowsetODBC::Bind(BOOL bBindToString)
	{
	UNUSED_PARA(bBindToString);

	if (NULL == m_pStatement || SQL_NULL_HSTMT == m_pStatement->m_hStatement)
		return FALSE;
	
	m_ulCols = m_pStatement->m_ulCols;

	// Get column info.
	//m_pDBColumnInfo = new AuColumnODBC[m_ulCols];
	m_pDBColumnInfo = m_ColumnODBC;
	for (short i = 0; i < (short)m_ulCols; i++)
		{
		if (!m_pDBColumnInfo[i].GetInfo(m_pStatement->m_hStatement, (short)(i+1)))
			{
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_pStatement->m_hStatement);
			TRACE("!!! Error : Get column info. failed in awRowsetODBC::Bind()\n");
			return FALSE;
			}
		_tcscpy(m_stColumns[i].m_szName, m_pDBColumnInfo[i].Name());
		}

	// set offset
	//m_pBindings = new AuBindingODBC[m_ulCols];
	m_pBindings = m_BindingODBC;
	ULONG ulOffset = 0;
	for (short i = 0; i < (short) m_ulCols; i++)
		{
		ulOffset += m_pBindings[i].Set(&m_pDBColumnInfo[i], ulOffset);
		}

	m_ulRowBufferSize = ulOffset;
	if (!SQL_SUCCEEDED(SQLSetStmtAttr(m_pStatement->m_hStatement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)UintToPtr(m_ulRowBufferSize), 0)))
		{
		AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_pStatement->m_hStatement);
		}

	return TRUE;
	}


//	Fetch
//======================================================
//
//	Bind column value to buffer and fetch rows
//
BOOL AuRowsetODBC::Fetch(UINT16 uiFetchCount)
	{
	SQLULEN ulFetched = 0;
	
	ULONG ulFetchBufferSize = m_ulRowBufferSize * uiFetchCount;

	//printf("Fetch : %d\n", ulFetchBufferSize);
	if(!m_Buffer.Alloc(ulFetchBufferSize))
	{
		printf("Fetch : %d\n", ulFetchBufferSize);
		printf( " Alloc fail -1 !!!!!!!!! \n");
		return FALSE;
	}
	LPBYTE pBuffer = m_Buffer.GetTail();

	if(!pBuffer)
	{
		printf( " Alloc fail -2 !!!!!!!!! \n");
		return FALSE;
	}

	ZeroMemory(pBuffer, ulFetchBufferSize);

	for (unsigned short i = 0; i < m_ulCols; i++)
		{
		if (!SQL_SUCCEEDED(SQLBindCol(m_pStatement->m_hStatement, (unsigned short)(i+1),
									m_pBindings[i].Type(),				// target type
									m_pBindings[i].Value(pBuffer),		// target value ptr
									m_pBindings[i].MaxLen(),			// buffer length
									m_pBindings[i].Ind(pBuffer)			// ... 
									)))
			{
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_pStatement->m_hStatement);
			TRACE("!!! Error : Binding failed in AuRowsetODBC::()\n");
			return FALSE;
			}
		}

	SQLUSMALLINT* pRowStatusArray = new SQLUSMALLINT[uiFetchCount];
	if(!pRowStatusArray)
	{
		printf("pRowStatusArray error \n");
		return FALSE;
	}

	// Block Fetch, if supported
	SQLRETURN hr = SQLExtendedFetch(m_pStatement->m_hStatement, SQL_FETCH_NEXT, uiFetchCount, &ulFetched, pRowStatusArray);
	BOOL bRet;
	switch (hr)
		{
		case SQL_NO_DATA :
			m_bEnd = TRUE;
			bRet = TRUE;
			break;

		case SQL_SUCCESS_WITH_INFO:
			TRACE("!!! Warning : Succeeded with info. in AuRowsetODBC::Fetch()\n");

		case SQL_SUCCESS :
			m_Buffer.AddUsed((DWORD)ulFetched * m_ulRowBufferSize);
			m_ulRows += (ULONG)ulFetched;
			bRet = TRUE;
			break;

		default:
			// Error
			AuGetErrorInfoODBC(m_szError, SQL_HANDLE_STMT, m_pStatement->m_hStatement);
			TRACE("!!! Error : Fetch failed in AuRowsetODBC::Fetch()\n");
			bRet = FALSE;
			break;
			}

	delete [] pRowStatusArray;
	return bRet;
	}


BOOL AuRowsetODBC::IsEnd()
	{
	return m_bEnd;
	}


UINT32 AuRowsetODBC::GetOffset(UINT32 ulCol)
	{
	return m_pBindings[ulCol].ValueOffset();
	}


//	Value method
//================================================
//
PVOID AuRowsetODBC::Get(UINT32 ulRow, UINT32 ulCol)
	{
	//ASSERT(m_ulRows > ulRow);
	//ASSERT(m_ulCols > ulCol);
	if (ulRow >= m_ulRows || ulCol > m_ulCols)
		return NULL;

	return 
		m_Buffer.GetHead() + (ulRow * m_ulRowBufferSize + /* column offset */m_pBindings[ulCol].ValueOffset());
	}


//	Extra 
//==================================================
//
//	set(reset) statement to preallocated rowset
//
void AuRowsetODBC::Set(AuStatement& Statement)
	{
	Close();	// reset
	m_pStatement = (AuStmtODBC *) &Statement;
	}




/********************************************************/
/*		The Implementation of AuColumnODBC class		*/
/********************************************************/
//
AuColumnODBC::AuColumnODBC()
	{
	Initialize();
	}


void AuColumnODBC::Initialize()
	{
	ZeroMemory(m_szName, sizeof(m_szName));
	m_nType = SQL_UNKNOWN_TYPE;
	m_nColumnSize = 0;
	m_nDecimalDigits = 0;
	m_bIsRequired = FALSE;
	}


//	AuColumnODBC::GetInfo
//=========================================================
//
//	nIndex : 1-based index
//
BOOL AuColumnODBC::GetInfo(SQLHSTMT hStatement, short nIndex)
	{

	SQLSMALLINT Size = 0;
	SQLSMALLINT nNullable = 0;

	if (SQL_SUCCESS != SQLDescribeCol(hStatement, nIndex,
								(SQLCHAR *) m_szName,			// column name 
								SQL_STRING_LENGTH,				// column name buffer length
								&Size,							// column name length(out)
								&m_nType,						// column type
								&m_nColumnSize,					// column length
								&m_nDecimalDigits,				// decimal digits
								&nNullable						// Nullable
								))
		{
		TRACE("!!! Error : DescribeCol failed in AuColumnODBC::GetInfo()");
		return FALSE;
		}

	m_bIsRequired = (nNullable != SQL_NULLABLE);
	return TRUE;
	}


LPCTSTR AuColumnODBC::Name()
	{
	return m_szName;
	}


SQLSMALLINT AuColumnODBC::Type()
	{
	return m_nType;
	}


SQLINTEGER AuColumnODBC::ColumnSize()
	{
	return (SQLINTEGER)m_nColumnSize;
	}


SQLSMALLINT AuColumnODBC::DecimalDigits()
	{
	return m_nDecimalDigits;
	}




/********************************************************/
/*		The Implementation of AuBindingODBC class		*/
/********************************************************/
//
ULONG AuBindingODBC::Set(AuColumnODBC* pColumn, ULONG ulOffset)
	{
	m_ulOffset = ulOffset;
	m_TargetType = SQL_C_TCHAR;

	switch (pColumn->Type())
		{
		case SQL_CHAR :
		case SQL_WCHAR :
		case SQL_VARCHAR :
		case SQL_WVARCHAR :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize() + 1);
			break;

		case SQL_DECIMAL :		
		case SQL_NUMERIC :

		case SQL_TINYINT :
		case SQL_SMALLINT :
		case SQL_INTEGER :
		case SQL_BIGINT :
		
		case SQL_REAL :
		case SQL_FLOAT :
		case SQL_DOUBLE :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_NUMERIC_SIZE);
			break;

		//case SQL_DATE :		// equal to SQL_DATETIME
		//case SQL_TIME :		// equal to SQL_INTERVAL
		case SQL_DATETIME :
		case SQL_INTERVAL :
		case SQL_TIMESTAMP :
		case SQL_TYPE_DATE :
		case SQL_TYPE_TIME :
		case SQL_TYPE_TIMESTAMP :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_DATETIME_SIZE);
			break;

		case SQL_BIT :
			m_ulMaxLen = sizeof(TCHAR);
			break;

		case SQL_LONGVARCHAR :
		case SQL_WLONGVARCHAR :
		case SQL_BINARY :
		case SQL_VARBINARY :
		case SQL_LONGVARBINARY :
		default :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_COLUMN_VALUE_SIZE);
			break;
		}
	return (m_ulMaxLen + sizeof(LONG));
	}


ULONG AuBindingODBC::Set2(AuColumnODBC* pColumn, ULONG ulOffset)
	{
	m_ulOffset = ulOffset;
	m_TargetType = SQL_C_TCHAR;

	switch (pColumn->Type())
		{
		// String
		case SQL_CHAR :
		case SQL_WCHAR :
		case SQL_VARCHAR :
		case SQL_WVARCHAR :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize() + 1);
			break;

		// Integer
		case SQL_TINYINT :
			m_ulMaxLen = sizeof(INT8);
			break;
		case SQL_SMALLINT :
			m_ulMaxLen = sizeof(INT16);
			break;
		case SQL_INTEGER :
			m_ulMaxLen = sizeof(INT32);
			break;
		case SQL_BIGINT :
			m_ulMaxLen = sizeof(INT64);
			break;

		// Floating point
		case SQL_REAL :
		case SQL_FLOAT :
			m_ulMaxLen = sizeof(FLOAT);
			break;
		case SQL_DOUBLE :
			m_ulMaxLen = sizeof(double);
			break;

		// Decimal
		case SQL_DECIMAL :		
		case SQL_NUMERIC :
			m_ulMaxLen = sizeof(TCHAR) * (pColumn->ColumnSize() + 1);
			break;

		// Datetime
		//case SQL_DATE :		// equal to SQL_DATETIME
		//case SQL_TIME :		// equal to SQL_INTERVAL
		case SQL_DATETIME :
		case SQL_INTERVAL :
		case SQL_TIMESTAMP :
		case SQL_TYPE_DATE :
		case SQL_TYPE_TIME :
		case SQL_TYPE_TIMESTAMP :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_DATETIME_SIZE);
			break;

		case SQL_BIT :
			m_ulMaxLen = sizeof(TCHAR);
			break;

		case SQL_LONGVARCHAR :
		case SQL_WLONGVARCHAR :
		case SQL_BINARY :
		case SQL_VARBINARY :
		case SQL_LONGVARBINARY :
		default :
			m_ulMaxLen = sizeof(TCHAR) * (AUDB_MAX_COLUMN_VALUE_SIZE);
			break;
		}
	return (m_ulMaxLen + sizeof(LONG));
	}


void* AuBindingODBC::Value(LPBYTE pBuffer)
	{
	return (void *)(pBuffer + m_ulOffset);
	}


ULONG AuBindingODBC::ValueOffset()
	{
	return m_ulOffset;
	}


SQLLEN* AuBindingODBC::Ind(LPBYTE pBuffer)
	{
	return (SQLLEN *)(pBuffer + m_ulOffset + m_ulMaxLen);
	}


SQLSMALLINT	AuBindingODBC::Type()
	{
	return m_TargetType;
	}


ULONG AuBindingODBC::MaxLen()
	{
	return m_ulMaxLen;
	}




/********************************************************/
/*		The Implementation of GetErrorInfo Function		*/
/********************************************************/
//
void AuGetErrorInfoODBC(LPTSTR pszError, SQLSMALLINT HandleType, SQLHANDLE hHandle)
	{
	SQLRETURN		rc = SQL_SUCCESS;
	SQLSMALLINT		nLength;
	SQLSMALLINT		nRecordNo = 0;
	SQLINTEGER		lCode;
	SQLTCHAR		szState[5+1];
	SQLTCHAR		szMsg[AUDB_DEFAULT_ERROR_STRING_SIZE + 1];  // 128 + 1
	
	LPTSTR psz = pszError;
	*psz = _T('\0');

	do
		{
		lCode = 0;
		szState[0] = szMsg[0] = _T('\0');
		rc = SQLGetDiagRec(HandleType, hHandle, ++nRecordNo, szState, &lCode,
								szMsg, AUDB_DEFAULT_ERROR_STRING_SIZE, &nLength);

		if (SQL_SUCCESS == rc)
			{
			psz += _stprintf(psz, _T("[%s][%ld] %s\n"), szState, lCode, szMsg);
			}
		} while (SQL_SUCCESS == rc);


	if (SQL_NO_DATA == rc)
		{
		TRACE("!!! Error : %s\n", pszError);
		return;
		}

	switch (rc)
		{
		case SQL_SUCCESS_WITH_INFO:
			_stprintf(pszError, _T("Message buffer is too small."));
			break;
		case SQL_INVALID_HANDLE:
			_stprintf(pszError, _T("ODBC handle is invalid."));
			break;
		case SQL_ERROR:
			_stprintf(pszError, _T("Bufferlength is invalid."));
			break;
		case SQL_NO_DATA:
			_stprintf(pszError, _T("Invalid diagnostic record=[%d]."), nRecordNo);
			break;
		default:
			_stprintf(pszError, _T("Unknown return code=[%d]."), rc);
			break;
		}
	}

