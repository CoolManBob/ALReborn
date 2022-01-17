/*=======================================================================

	AuDatabaseOLEDB.cpp

=======================================================================*/


#include <tchar.h>
#include <stdio.h>
#include "ApBase.h"

#include <oledb.h>
#include <oledberr.h>	// OLEDB Errors
#include <msdasc.h>		// OLEDB ServiceComponents
#include <msdaguid.h>	// CLSID_OLEDB_ENUMERATOR
#include <msdasql.h>	// MSDASQL - Provider for ODBC
#include <msdaora.h>    // MSDAORA - Oracle provider 
#include <msdadc.h>		// DataConversion library

#include "AuDatabaseOLEDB.h"

#include "ApMemoryTracker.h"

#pragma warning (disable : 4127)

#ifdef _DEBUG
	#define AUDBTRACE				TRACE
#else
	#define AUDBTRACE(p, x)			x
#endif

const CLSID my_CLSID_MSDAORA			= {0xE8CC4CBE,0xFDFF,0x11D0,{0xB8,0x65,0x00,0xA0,0xC9,0x08,0x1C,0x1D}};
const CLSID my_CLSID_MSDASQL			= {0xC8B522CBL,0x5CF3,0x11CE,{0xAD,0xE5,0x00,0xAA,0x00,0x44,0x77,0x3D}};
const CLSID my_CLSID_OraOLEDB			= {0x3F63C36E,0x51A3,0x11D2,{0xBB,0x7D,0x00,0xC0,0x4F,0xA3,0x00,0x80}};


/********************************************/
/*		Transaction Isolation Level Map		*/
/********************************************/
//
ISOLEVEL AUDB_ISOLATION_LEVEL_OLEDB[AUDB_ISOLATION_LEVEL_NUM] = 
	{
	ISOLATIONLEVEL_UNSPECIFIED,
	ISOLATIONLEVEL_CHAOS,
	ISOLATIONLEVEL_READUNCOMMITTED,
	ISOLATIONLEVEL_READCOMMITTED,
	ISOLATIONLEVEL_REPEATABLEREAD,
	ISOLATIONLEVEL_SERIALIZABLE,
	ISOLATIONLEVEL_ISOLATED
	};


/************************************************************/
/*		The Implementation of OLEDB Environment class		*/
/************************************************************/
//
AuEnvOLEDB::AuEnvOLEDB(eAUDB_VENDER eVender)
	: AuEnvironment(eVender), m_pIEnumerator(NULL), m_pISourcesRowset(NULL)
	{
	}


AuEnvOLEDB::~AuEnvOLEDB()
	{
	Close();
	}


BOOL AuEnvOLEDB::Open()
	{
	/*
	// Enumerator로 m_eVender에 해당하는 넘을 거시기한다.
	//		지금은 하드코딩되어 있으니 나중에 하자.
	if (S_OK != CoCreateInstance(CLSID_OLEDB_ENUMERATOR, NULL, CLSCTX_INPROC_SERVER,
							 IID_IUnknown, (void **)&m_pIEnumerator))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}
	m_pIEnumerator->QueryInterface(IID_ISourcesRowset, (void **)&m_pISourcesRowset);
	*/
	return TRUE;
	}


void AuEnvOLEDB::Close()
	{
	if (m_pISourcesRowset)
		{
		m_pISourcesRowset->Release();
		m_pISourcesRowset = NULL;
		}
	if (m_pIEnumerator)
		{
		m_pIEnumerator->Release();
		m_pIEnumerator = NULL;
		}
	}


IDBInitialize* AuEnvOLEDB::AllocConn()
	{
	//	Hardcoded.
	IDBInitialize* pIDBInitialize = NULL;

	switch (m_eVender)
		{
		case AUDB_VENDER_ORACLE :
			CoCreateInstance(my_CLSID_MSDAORA, NULL, CLSCTX_INPROC_SERVER, IID_IDBInitialize, (void **)&pIDBInitialize);
			//CoCreateInstance(my_CLSID_OraOLEDB, NULL, CLSCTX_INPROC_SERVER, IID_IDBInitialize, (void **)&pIDBInitialize);
			break;
		default :
			CoCreateInstance(my_CLSID_MSDASQL, NULL, CLSCTX_INPROC_SERVER, IID_IDBInitialize, (void **)&pIDBInitialize);			
			break;
		}
	return pIDBInitialize;
	}


void AuEnvOLEDB::FreeConn(IUnknown* pIDBInitialize)
	{
	if (pIDBInitialize)
		AUDBTRACE("Release IDBInitialize[%d] in AuEnvOLEDB::FreeConn()\n", pIDBInitialize->Release());
	}


//	Attribute(Property)
//=======================================================
//
HRESULT Set(IUnknown* pIUnknown, ULONG cPropertySets, DBPROPSET rgPropertySets[])
	{
	HRESULT hr = E_FAIL;
	IDBProperties*	pIDBProperties = NULL;

	pIUnknown->QueryInterface(IID_IDBProperties, (void **)&pIDBProperties);
	if (NULL != pIDBProperties)
		{
		hr = pIDBProperties->SetProperties(cPropertySets, rgPropertySets);
		AUDBTRACE("Release IDBProperties[%d] in Set()\n", pIDBProperties->Release());
		}
	return hr;
	}


HRESULT Get(IUnknown* pIUnknown, ULONG cPropertyIDSets, const DBPROPIDSET rgPropertyIDSets[],
										 ULONG* pcPropertySets, DBPROPSET** prgPropertySets)
	{
	HRESULT hr = E_FAIL;
	IDBProperties*	pIDBProperties = NULL;

	pIUnknown->QueryInterface(IID_IDBProperties, (void **)&pIDBProperties);
	if (NULL != pIDBProperties)
		{
		hr = pIDBProperties->GetProperties(cPropertyIDSets, rgPropertyIDSets, pcPropertySets, prgPropertySets);
		AUDBTRACE("Release IDBProperties[%d] in Get()\n", pIDBProperties->Release());
		}
	return hr;
	}




/********************************************************/
/*		The Implementation of OLEDB Database class		*/
/********************************************************/
//
AuDatabaseOLEDB::AuDatabaseOLEDB(AuEnvironment& Environment)
	: m_pIDBInitialize(NULL), m_pIDBProperties(NULL), m_pISession(NULL), m_pITransaction(NULL), m_bOpen(FALSE)
	{
	m_pEnvironment = (AuEnvOLEDB*) &Environment;
	m_pIDBInitialize = m_pEnvironment->AllocConn();
	}


AuDatabaseOLEDB::~AuDatabaseOLEDB()
	{
	Close();
	m_pEnvironment->FreeConn(m_pIDBInitialize);
	}


void AuDatabaseOLEDB::Close()
	{
	if (m_pITransaction)
		{
		AUDBTRACE("Release ITransaction[%d] in AuDatabaseOLEDB::Close()\n", m_pITransaction->Release());
		m_pITransaction = NULL;
		}
	if (m_pISession)
		{
		AUDBTRACE("Release ISession[%d] in AuDatabaseOLEDB::Close()\n", m_pISession->Release());
		m_pISession = NULL;
		}
	if (m_pIDBInitialize)
		{
		AUDBTRACE("HR[%ld] IDBInitialize::Uninitialize in AuDatabaseOLEDB::Close()\n", m_pIDBInitialize->Uninitialize());
		}
	m_bOpen = FALSE;
	}


BOOL AuDatabaseOLEDB::Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword)
	{
	ASSERT(NULL != pszDatasource);
	ASSERT(NULL != pszUser);
	ASSERT(NULL != pszPassword);

	BSTR	bstrDatasource;
	BSTR	bstrUser;
	BSTR	bstrPassword;
	
  #ifndef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, pszDatasource, -1, m_wszDatasource, AUDB_MAX_DSN_SIZE);
	MultiByteToWideChar(CP_ACP, 0, pszUser, -1, m_wszUser, AUDB_MAX_DSN_SIZE);
	MultiByteToWideChar(CP_ACP, 0, pszPassword, -1, m_wszPassword, AUDB_MAX_DSN_SIZE);

	bstrDatasource = SysAllocString(m_wszDatasource);
	bstrUser = SysAllocString(m_wszUser); 
	bstrPassword = SysAllocString(m_wszPassword);
  #else
	// Is Proper?
	bstrDatasource = SysAllocString(pszDatasource);
	bstrUser = SysAllocString(pszUser); 
	bstrPassword = SysAllocString(pszPassword);		
  #endif	

	DBPROP			Properties[5];
	DBPROPSET		PropertySet[1];
	
	for (int i=0; i<5; i++)
		{
		VariantInit(&Properties[i].vValue);
		Properties[i].dwOptions = DBPROPOPTIONS_REQUIRED;
		Properties[i].colid = DB_NULLID;
		}

	// Prompt off
	Properties[0].dwPropertyID = DBPROP_INIT_PROMPT;
	Properties[0].vValue.vt = VT_I2;
	Properties[0].vValue.iVal = DBPROMPT_NOPROMPT;
	// Datasource
	Properties[3].dwPropertyID = DBPROP_INIT_DATASOURCE;
	Properties[3].vValue.vt = VT_BSTR;
	Properties[3].vValue.bstrVal = bstrDatasource;
	// User
	Properties[1].dwPropertyID = DBPROP_AUTH_USERID;
	Properties[1].vValue.vt = VT_BSTR;
	Properties[1].vValue.bstrVal = bstrUser;
	// Password
	Properties[2].dwPropertyID = DBPROP_AUTH_PASSWORD;
	Properties[2].vValue.vt = VT_BSTR;
	Properties[2].vValue.bstrVal = bstrPassword;
	// R/W
	Properties[4].dwPropertyID = DBPROP_INIT_MODE;
	Properties[4].vValue.vt = VT_I4;
	Properties[4].vValue.lVal = DB_MODE_READWRITE;
	
	// Property set
	PropertySet[0].guidPropertySet = DBPROPSET_DBINIT;
	PropertySet[0].cProperties = 5;
	PropertySet[0].rgProperties = Properties;

	// Set PropertySet
	Set(m_pIDBInitialize, 1, PropertySet);

	if (FAILED(m_pIDBInitialize->Initialize()))
		{
		::SysFreeString(bstrDatasource);
		::SysFreeString(bstrUser);
		::SysFreeString(bstrPassword);
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	::SysFreeString(bstrDatasource);
	::SysFreeString(bstrUser);
	::SysFreeString(bstrPassword);

	// Get IDBCreateSession Interface
	IDBCreateSession* pIDBCreateSession;
	if (FAILED(m_pIDBInitialize->QueryInterface(IID_IDBCreateSession, (void**)&pIDBCreateSession)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (FAILED(pIDBCreateSession->CreateSession(NULL, IID_IDBCreateCommand, (IUnknown **)&m_pISession)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		pIDBCreateSession->Release();
		return FALSE;
		}

	AUDBTRACE("Release IDBCreateSession[%d] in AuDatabaseOLEDB::Open()\n", pIDBCreateSession->Release());

	m_bOpen = TRUE;
	return TRUE;
	}


//	Transaction
//=============================================
//
BOOL AuDatabaseOLEDB::BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation)
	{
	HRESULT hr;
	// get interface
	if (FAILED(hr = m_pISession->QueryInterface(IID_ITransactionLocal, (void **)&m_pITransaction)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	// start transaction
	if (FAILED(m_pITransaction->StartTransaction(AUDB_ISOLATION_LEVEL_OLEDB[eIsolation], 0, NULL, NULL)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}
	return TRUE;
	}


BOOL AuDatabaseOLEDB::EndTransaction(BOOL bCommit)
	{
	if (NULL == m_pITransaction)
		{
		TRACE("!!! Warning : Transaction is NULL in AuConnOLEDB::EndTransaction()\n");
		return FALSE;
		}

	HRESULT hr;
	if (bCommit)
		hr = m_pITransaction->Commit(FALSE, XACTTC_SYNC, 0);
	else
		hr = m_pITransaction->Abort(NULL, FALSE, FALSE);

	if (FAILED(hr))
		{
		AuGetErrorInfoOLEDB(m_szError);
		AUDBTRACE("Release ITransaction[%d] in EndTransaction()\n", m_pITransaction->Release());
		m_pITransaction = NULL;
		return FALSE;
		}

	AUDBTRACE("Release ITransaction[%d] in EndTransaction()\n", m_pITransaction->Release());
	m_pITransaction = NULL;
	return TRUE;
	}


//	Status
//============================================
//
BOOL AuDatabaseOLEDB::IsAlive()
	{
	DBPROPID		PropertyID[1];			// in
	DBPROPIDSET		PropertySet[1];
	ULONG			ulPropertySet = 0;		// out
	DBPROPSET*		pRetPropertySet = NULL;

	PropertyID[0] = DBPROP_CONNECTIONSTATUS;
	PropertySet[0].guidPropertySet = DBPROPSET_DATASOURCEINFO;
	PropertySet[0].cPropertyIDs = 1;
	PropertySet[0].rgPropertyIDs = PropertyID;

	if (FAILED(Get(m_pIDBInitialize, 1, PropertySet, &ulPropertySet, &pRetPropertySet)))
		{
		TRACE("!!! Warning : Failed to get prop(DBPROP_CONNECTIONSTATUS). No interface of IDBProperty... may be in AuDatabaseOLEDB::IsAlive()\n");
		return m_bOpen;
		}

	BOOL bRet = FALSE;
	if (pRetPropertySet)
		{
		bRet = (DBPROPVAL_CS_INITIALIZED == pRetPropertySet->rgProperties->vValue.iVal);
		CoTaskMemFree(pRetPropertySet[0].rgProperties);
		CoTaskMemFree(pRetPropertySet);
		}
	return bRet;
	}




/********************************************************/
/*		The Implementation of OLEDB Statement class		*/
/********************************************************/
//
AuStmtOLEDB::AuStmtOLEDB(AuDatabase& Database)
	: m_pISession(NULL)
	{
	m_pIRowset = NULL;
	m_pICommandWithParameters = NULL;
	m_hAccessor = NULL;
	m_pIAccessor = NULL;
	m_nParams = 0;
	m_pDatabase = (AuDatabaseOLEDB *)&Database;
	m_pISession = (IDBCreateCommand* )m_pDatabase->m_pISession;
	AUDBTRACE("AddRef ISession[%d] in AuStmtOLEDB::AuStmtOLEDB()\n", m_pISession->AddRef());	// 받은거니까 AddRef해준다.
	}


AuStmtOLEDB::~AuStmtOLEDB()
	{
	Close();
	}


BOOL AuStmtOLEDB::Open()
	{
	return TRUE;
	}


void AuStmtOLEDB::Close()
	{
	if (NULL != m_pIRowset)
		{
		AUDBTRACE("Release IRowset[%d] in AuStmtOLEDB::Close()\n", m_pIRowset->Release());
		m_pIRowset = NULL;
		}
	if (NULL != m_pISession)
		{
		AUDBTRACE("Release ISession[%d] in AuStmtOLEDB::Close()\n", m_pISession->Release());
		m_pISession = NULL;
		}
	}


BOOL AuStmtOLEDB::Execute(LPCTSTR pszQuery)
	{
	LPWSTR pwszCommand;

	if (NULL != m_pIRowset)
		{
		AUDBTRACE("Release IRowset[%d] in AuStmtOLEDB::Execute()\n", m_pIRowset->Release());
		m_pIRowset = NULL;
		}

	if (FAILED(m_pISession->CreateCommand(NULL, IID_ICommandText, (IUnknown **)&m_pICommandText)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}
	
  #ifndef _UNICODE
	pwszCommand = m_wszCommand;
	MultiByteToWideChar(CP_ACP, 0, pszQuery, -1, pwszCommand, AUDB_MAX_QUERY_SIZE);
  #else
	pwszCommand = (LPWSTR)pszQuery;
  #endif

	if (FAILED(m_pICommandText->SetCommandText(DBGUID_DEFAULT, pwszCommand)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (FAILED(m_pICommandText->Execute(NULL, IID_IRowset, NULL, &m_lAffectedRows, (IUnknown **)&m_pIRowset)))
		{
		AUDBTRACE("Release ICommandText[%d] in AuStmtOLEDB::Execute()\n", m_pICommandText->Release());
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	AUDBTRACE("Release ICommandText[%d] in AuStmtOLEDB::Execute()\n", m_pICommandText->Release());

	return TRUE;
	}


BOOL AuStmtOLEDB::Prepare(LPCTSTR pszQuery)
	{
	if (NULL == pszQuery || _T('\0') == *pszQuery)
		return FALSE;
	
	LPWSTR pwszCommand;

	if (NULL != m_pIRowset)
		{
		AUDBTRACE("Release IRowset[%d] in AuStmtOLEDB::Execute()\n", m_pIRowset->Release());
		m_pIRowset = NULL;
		}

	if (FAILED(m_pISession->CreateCommand(NULL, IID_ICommandText, (IUnknown **)&m_pICommandText)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

  #ifndef _UNICODE
	pwszCommand = m_wszCommand;
	MultiByteToWideChar(CP_ACP, 0, pszQuery, -1, pwszCommand, AUDB_MAX_QUERY_SIZE);
  #else
	pwszCommand = (LPWSTR)pszQuery;
  #endif

	if (FAILED(m_pICommandText->SetCommandText(DBGUID_DEFAULT, pwszCommand)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (FAILED(m_pICommandText->QueryInterface(IID_ICommandWithParameters,  (void **) &m_pICommandWithParameters)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}
	
	m_nParams = 0;

	return TRUE;
	}


BOOL AuStmtOLEDB::Execute()
	{
	if (NULL == m_pICommandWithParameters)
		{
		m_nParams = 0;
		return FALSE;
		}

	DB_UPARAMS		cParams;
	DB_UPARAMS		rgParamOrdinal[AUDB_MAX_COLUMN_SIZE];
	DBPARAMBINDINFO rgParamBindInfo[AUDB_MAX_COLUMN_SIZE];

	cParams = m_nParams;
	for (ULONG ul = 0; ul < cParams; ul++)
		{
		rgParamOrdinal[ul] = ul + 1;

		rgParamBindInfo[ul].pwszName			= NULL;
		rgParamBindInfo[ul].ulParamSize			= m_ParamInfo[ul].m_ulParamSize;
		rgParamBindInfo[ul].bPrecision			= m_ParamInfo[ul].m_bPrecision;
		rgParamBindInfo[ul].bScale				= m_ParamInfo[ul].m_bScale;
		rgParamBindInfo[ul].pwszDataSourceType	= m_ParamInfo[ul].m_pwszDataSourceType;
		rgParamBindInfo[ul].dwFlags				= DBPARAMFLAGS_ISINPUT;
		}

	if (FAILED(m_pICommandWithParameters->SetParameterInfo(cParams, rgParamOrdinal, rgParamBindInfo)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		m_nParams = 0;
		return FALSE;
		}

	DBCOUNTITEM	cBindings = cParams;
	DBBINDING	rgBindings[AUDB_MAX_COLUMN_SIZE];
	DBLENGTH	dwOffset = 0;

	// Setup the Bindings
	for (ULONG ul = 0; ul < cBindings; ul++)
		{
		rgBindings[ul].iOrdinal		= ul + 1;
		rgBindings[ul].obStatus		= dwOffset;
		rgBindings[ul].obLength		= dwOffset;
		rgBindings[ul].obValue		= dwOffset;
		rgBindings[ul].pTypeInfo	= NULL;
		rgBindings[ul].pBindExt		= NULL;
		rgBindings[ul].dwPart		= DBPART_VALUE;
		rgBindings[ul].dwMemOwner	= DBMEMOWNER_CLIENTOWNED;
		rgBindings[ul].eParamIO		= DBPARAMIO_INPUT;
		rgBindings[ul].dwFlags		= 0;
		rgBindings[ul].bPrecision	= rgParamBindInfo[ul].bPrecision;
		rgBindings[ul].bScale		= rgParamBindInfo[ul].bScale;
		rgBindings[ul].pObject		= NULL;
		rgBindings[ul].wType		= m_ParamInfo[ul].m_wType;
		rgBindings[ul].cbMaxLen		= m_ParamInfo[ul].m_cbMaxLen;
		dwOffset += rgBindings[ul].cbMaxLen;
		}

	if (FAILED(m_pICommandWithParameters->QueryInterface(IID_IAccessor, (void**) &m_pIAccessor)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		m_nParams = 0;
		return FALSE;
		}

	if (FAILED(m_pIAccessor->CreateAccessor(DBACCESSOR_PARAMETERDATA, cBindings, rgBindings, dwOffset, &m_hAccessor, NULL))) 
		{
		AuGetErrorInfoOLEDB(m_szError);
		m_nParams = 0;
		return FALSE;
		}

	for (ULONG ul = 0; ul < cBindings; ul++)
		{
		m_Params[ul].cParamSets = 1;
		m_Params[ul].hAccessor = m_hAccessor;
		}

	if (FAILED(m_pICommandText->Execute(NULL, IID_IRowset, m_Params, &m_lAffectedRows, (IUnknown **)&m_pIRowset)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		AUDBTRACE("Release ICommandText[%d] in AuStmtOLEDB::Execute()\n", m_pICommandText->Release());
		m_nParams = 0;
		return FALSE;
		}

	AUDBTRACE("Release ICommandText[%d] in AuStmtOLEDB::Execute()\n", m_pICommandText->Release());
	m_nParams = 0;

	return TRUE;
	}


//	SetParam
//=====================================================================
//
//	set parameter data ptr.
//
BOOL AuStmtOLEDB::SetParam(INT16 nIndex, PVOID pBuffer, INT32 lBufferSize, BOOL bOut)
	{
	UNUSED_PARA(nIndex);
	UNUSED_PARA(pBuffer);
	UNUSED_PARA(lBufferSize);
	UNUSED_PARA(bOut);
	return FALSE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, CHAR* pszBuffer, INT32 lBufferSize, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= lBufferSize;
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_VARCHAR";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_STR;
	m_ParamInfo[m_nParams].m_cbMaxLen				= strlen(pszBuffer) + 1;

	m_Params[m_nParams].pData = pszBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, INT8* pcBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(INT16);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_I1";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_I1;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(INT8);

	m_Params[m_nParams].pData = pcBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, INT16* pnBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(INT16);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_I2";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_I2;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(INT16);

	m_Params[m_nParams].pData = pnBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, INT32* plBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(INT32);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_I4";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_I4;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(INT32);

	m_Params[m_nParams].pData = plBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, INT64* plBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(INT64);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_I8";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_I8;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(INT64);

	m_Params[m_nParams].pData = plBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, UINT8* pucBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(UINT8);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_VARCHAR";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_UI1;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(UINT8);

	m_Params[m_nParams].pData = pucBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, UINT16* punBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(UINT16);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_UI2";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_UI2;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(UINT16);

	m_Params[m_nParams].pData = punBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, UINT32* pulBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(UINT32);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_I4";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_UI4;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(UINT32);

	m_Params[m_nParams].pData = pulBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, UINT64* plBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(UINT64);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_UI8";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_UI8;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(UINT64);

	m_Params[m_nParams].pData = plBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::SetParam(INT16 nIndex, FLOAT* pfBuffer, BOOL bOut)
	{
	UNUSED_PARA(bOut);
	ASSERT(nIndex == m_nParams);
	m_ParamInfo[m_nParams].m_ulParamSize			= sizeof(FLOAT);
	m_ParamInfo[m_nParams].m_bPrecision				= 0;
	m_ParamInfo[m_nParams].m_bScale					= 0;
	m_ParamInfo[m_nParams].m_pwszDataSourceType		= L"DBTYPE_R4";
	m_ParamInfo[m_nParams].m_wType					= DBTYPE_R4;
	m_ParamInfo[m_nParams].m_cbMaxLen				= sizeof(FLOAT);

	m_Params[m_nParams].pData = pfBuffer;

	m_nParams++;
	return TRUE;
	}


BOOL AuStmtOLEDB::HasResult()
	{
	return (NULL != m_pIRowset);
	}




/********************************************************/
/*		The Implementation of OLEDB Rowset class		*/
/********************************************************/
//
AuRowsetOLEDB::AuRowsetOLEDB()
	{
	m_pIRowset = NULL;
	m_pDBColumnInfo = NULL;
	m_pIAccessor = NULL;
	m_hAccessor = NULL;
	}


AuRowsetOLEDB::AuRowsetOLEDB(AuStatement& Statement)
	{
	m_pStatement = &Statement;
	m_pIRowset = ((AuStmtOLEDB *)m_pStatement)->m_pIRowset;
	//AUDBTRACE("AddRef IRowset[%d] in AuRowsetOLEDB::AuRowsetOLEDB()", m_pIRowset->AddRef());
	m_pDBColumnInfo = NULL;
	m_pIAccessor = NULL;
	m_hAccessor = NULL;
	}


AuRowsetOLEDB::~AuRowsetOLEDB()
	{
	Close();
	}


void AuRowsetOLEDB::Close()
	{
	if (NULL != m_pDBColumnInfo)
		{
		CoTaskMemFree(m_pDBColumnInfo);
		m_pDBColumnInfo = NULL;
		}
	if (NULL != m_pIAccessor)
		{
		m_pIAccessor->ReleaseAccessor(m_hAccessor, NULL);
		AUDBTRACE("Release IAccessor[%d] in AuRowsetOLEDB::Close()\n", m_pIAccessor->Release());
		m_pIAccessor = NULL;
		}
	if (NULL != m_pIRowset)
		{
		AUDBTRACE("Release IRowset[%d] in AuRowsetOLEDB::Close()\n", m_pIRowset->Release());
		m_pIRowset = NULL;
		((AuStmtOLEDB *)m_pStatement)->m_pIRowset = NULL;
		}

	AuRowset::Close();
	}


//	Bind and fetch method
//================================================
//
BOOL AuRowsetOLEDB::Bind(BOOL bBindToString)
	{
	IColumnsInfo	*pIColumnsInfo = NULL;
	WCHAR			*pStringBuffer = NULL;

	// Get Column Info.
	if (FAILED(m_pIRowset->QueryInterface(IID_IColumnsInfo, (void **)&pIColumnsInfo)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}
	if (FAILED(pIColumnsInfo->GetColumnInfo((DBORDINAL*)&m_ulCols, &m_pDBColumnInfo, &pStringBuffer)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (NULL != pIColumnsInfo)
		AUDBTRACE("Release IColumnsInfo[%d] in AuRowsetOLEDB::Close()\n", pIColumnsInfo->Release());

	// Binding
	if (bBindToString)
		BindToString();
	else
		BindPerType();

	CoTaskMemFree(pStringBuffer);

	if (FAILED(m_pIRowset->QueryInterface(IID_IAccessor, (void**)&m_pIAccessor)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (FAILED(m_pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, m_ulCols, m_Bindings, 0, &m_hAccessor, NULL)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	return TRUE;
	}


BOOL AuRowsetOLEDB::Fetch(UINT16 uiFetchCount)
	{
	ASSERT(NULL != m_hAccessor);
	ASSERT(AUDB_MAX_FETCH_COUNT >= uiFetchCount);

	HROW*	pRows = m_Rows;
	ULONG	ulFetched;

	if (FAILED(m_pIRowset->GetNextRows(DB_NULL_HCHAPTER, 0, uiFetchCount, (DBCOUNTITEM*)&ulFetched, &(pRows))))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (0 == ulFetched)
		{
		m_bEnd = TRUE;
		return TRUE;
		}

	ULONG ulFetchedBufferSize = m_ulRowBufferSize * ulFetched;
	m_Buffer.Alloc(ulFetchedBufferSize);

	LPBYTE pByte = m_Buffer.GetTail();// + m_ulTotalFetchedSize;
	for (ULONG i=0; i<ulFetched; i++)
		{
		if (FAILED(m_pIRowset->GetData(pRows[i], m_hAccessor, pByte + (i * m_ulRowBufferSize))))
			{
			AuGetErrorInfoOLEDB(m_szError);
			return FALSE;
			}
		}

	m_Buffer.AddUsed(ulFetchedBufferSize);
	m_ulRows += ulFetched;

	return TRUE;
	}


void AuRowsetOLEDB::BindToString()
	{
	ULONG ulOffset = 0;
	
	for (ULONG i = 0; i<m_ulCols; i++)
		{
		m_Bindings[i].iOrdinal = i+1;
		m_Bindings[i].obValue = ulOffset;
		m_Bindings[i].obLength = 0;
		m_Bindings[i].obStatus = 0;
		m_Bindings[i].pTypeInfo = NULL;
		m_Bindings[i].pObject = NULL;
		m_Bindings[i].pBindExt = NULL;
		m_Bindings[i].dwPart = DBPART_VALUE;
		m_Bindings[i].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
		m_Bindings[i].eParamIO = DBPARAMIO_NOTPARAM;
		m_Bindings[i].dwFlags = 0;

		// column info
	  #ifdef _UNICODE
		_tcscpy(m_stColumns[i].m_szName, m_pDBColumnInfo[i].pwszName);
	  #else
		WideCharToMultiByte(CP_ACP, 0, m_pDBColumnInfo[i].pwszName, -1, m_stColumns[i].m_szName, AUDB_MAX_COLUMN_NAME_SIZE, NULL, NULL);
	  #endif
		m_stColumns[i].m_eBindType = AUDB_TYPE_STRING;

		switch (m_pDBColumnInfo[i].wType)
			{
			case DBTYPE_NULL:
			case DBTYPE_EMPTY:
			case DBTYPE_I1:
			case DBTYPE_I2:
			case DBTYPE_I4:
			case DBTYPE_UI1:
			case DBTYPE_UI2:
			case DBTYPE_UI4:
			case DBTYPE_R4:
			case DBTYPE_BOOL:
			case DBTYPE_I8:
			case DBTYPE_UI8:
			case DBTYPE_R8:
			case DBTYPE_CY:
			case DBTYPE_ERROR:
				m_Bindings[i].cbMaxLen = (25 + 1) * sizeof(TCHAR);
				break;
			// 이것도 좀 그렇다. prec, scale로 거시기 할까?


			case DBTYPE_DECIMAL:
			case DBTYPE_NUMERIC:
			case DBTYPE_VARNUMERIC:
				m_Bindings[i].cbMaxLen = (AUDB_MAX_NUMERIC_SIZE) * sizeof(TCHAR);
				break;

			case DBTYPE_DATE:
			case DBTYPE_DBDATE:
			case DBTYPE_DBTIMESTAMP:
			case DBTYPE_GUID:
				m_Bindings[i].cbMaxLen = AUDB_MAX_DATETIME_SIZE * sizeof(TCHAR);
				break;

			case DBTYPE_BYTES:
				m_Bindings[i].cbMaxLen =
					(m_pDBColumnInfo[i].ulColumnSize * 2 + 1) * sizeof(TCHAR);
				break;

			case DBTYPE_STR:
			case DBTYPE_WSTR:
			case DBTYPE_BSTR:
				m_Bindings[i].cbMaxLen = 
				   (m_pDBColumnInfo[i].ulColumnSize + 1) * sizeof(TCHAR);
				break;

			default:
				
				m_Bindings[i].cbMaxLen = AUDB_MAX_COLUMN_VALUE_SIZE;
				break;
			}

		// bind all types to string
	  #ifndef _UNICODE
		m_Bindings[i].wType = DBTYPE_STR;
	  #else
		m_Bindings[i].wType = DBTYPE_WSTR;
	  #endif

		m_Bindings[i].bPrecision = m_pDBColumnInfo[i].bPrecision;
		m_Bindings[i].bScale = m_pDBColumnInfo[i].bScale;

		// WSTR : 2, STR : 1
		//ulOffset += (m_pDBColumnInfo[i].ulColumnSize * sizeof(TCHAR));
		ulOffset += (ULONG)m_Bindings[i].cbMaxLen;
		}

	m_ulRowBufferSize  = ulOffset;
	}


void AuRowsetOLEDB::BindPerType()
	{
	ULONG ulOffset = 0;

	for (ULONG i = 0; i<m_ulCols; i++)
		{
		m_Bindings[i].iOrdinal = i+1;
		m_Bindings[i].obValue = ulOffset;
		m_Bindings[i].obLength = 0;
		m_Bindings[i].obStatus = 0;
		m_Bindings[i].pTypeInfo = NULL;
		m_Bindings[i].pObject = NULL;
		m_Bindings[i].pBindExt = NULL;
		m_Bindings[i].dwPart = DBPART_VALUE;
		m_Bindings[i].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
		m_Bindings[i].eParamIO = DBPARAMIO_NOTPARAM;
		m_Bindings[i].dwFlags = 0;

		switch (m_pDBColumnInfo[i].wType)
			{
			// Integer
			case DBTYPE_I1:
				m_Bindings[i].cbMaxLen = sizeof(INT8);
				m_Bindings[i].wType = DBTYPE_I1;
				m_stColumns[i].m_eBindType = AUDB_TYPE_INT8;
				break;
			case DBTYPE_I2:
				m_Bindings[i].cbMaxLen = sizeof(INT16);
				m_Bindings[i].wType = DBTYPE_I2;
				m_stColumns[i].m_eBindType = AUDB_TYPE_INT16;
				break;
			case DBTYPE_I4:
				m_Bindings[i].cbMaxLen = sizeof(INT32);
				m_Bindings[i].wType = DBTYPE_I1;
				m_stColumns[i].m_eBindType = AUDB_TYPE_INT32;
				break;
			case DBTYPE_I8:
				m_Bindings[i].cbMaxLen = sizeof(INT64);
				m_Bindings[i].wType = DBTYPE_I8;
				m_stColumns[i].m_eBindType = AUDB_TYPE_INT64;
				break;
			case DBTYPE_UI1:
				m_Bindings[i].cbMaxLen = sizeof(UINT8);
				m_Bindings[i].wType = DBTYPE_UI1;
				m_stColumns[i].m_eBindType = AUDB_TYPE_UINT8;
				break;
			case DBTYPE_UI2:
				m_Bindings[i].cbMaxLen = sizeof(UINT16);
				m_Bindings[i].wType = DBTYPE_UI2;
				m_stColumns[i].m_eBindType = AUDB_TYPE_UINT16;
				break;
			case DBTYPE_UI4:
				m_Bindings[i].cbMaxLen = sizeof(UINT32);
				m_Bindings[i].wType = DBTYPE_UI4;
				m_stColumns[i].m_eBindType = AUDB_TYPE_UINT32;
				break;
			case DBTYPE_UI8:
				m_Bindings[i].cbMaxLen = sizeof(UINT64);
				m_Bindings[i].wType = DBTYPE_UI8;
				m_stColumns[i].m_eBindType = AUDB_TYPE_UINT64;
				break;

			// Decimal, Numeric
			case DBTYPE_DECIMAL:
			case DBTYPE_NUMERIC:
			case DBTYPE_VARNUMERIC:
			if (0 == m_pDBColumnInfo[i].bScale)
					{
					/*
					if (m_pDBColumnInfo[i].bPrecision <= 3)		// int8
						{
						m_Bindings[i].cbMaxLen = sizeof(INT8);
						m_Bindings[i].wType = DBTYPE_I1;
						}
					else if (m_pDBColumnInfo[i].bPrecision <= 5)
						{
						m_Bindings[i].cbMaxLen = sizeof(INT16);
						m_Bindings[i].wType = DBTYPE_I2;
						}
					else if (m_pDBColumnInfo[i].bPrecision <= 10)
						{
						m_Bindings[i].cbMaxLen = sizeof(INT32);
						m_Bindings[i].wType = DBTYPE_I4;
						}
					else if (m_pDBColumnInfo[i].bPrecision <= 20)
						{
						m_Bindings[i].cbMaxLen = sizeof(INT64);
						m_Bindings[i].wType = DBTYPE_I8;
						}
					*/
					m_Bindings[i].cbMaxLen = sizeof(INT64);
					m_Bindings[i].wType = DBTYPE_I8;
					m_stColumns[i].m_eBindType = AUDB_TYPE_INT64;
					}
				else
					{
					m_Bindings[i].cbMaxLen = sizeof(double);
					m_Bindings[i].wType = DBTYPE_R8;
					m_stColumns[i].m_eBindType = AUDB_TYPE_DOUBLE;
					}
				break;

			// Floating Point
			case DBTYPE_R4:
				m_Bindings[i].cbMaxLen = sizeof(float);
				m_Bindings[i].wType = DBTYPE_R4;
				m_stColumns[i].m_eBindType = AUDB_TYPE_FLOAT;
			case DBTYPE_R8:
				m_Bindings[i].cbMaxLen = sizeof(double);
				m_Bindings[i].wType = DBTYPE_R8;
				m_stColumns[i].m_eBindType = AUDB_TYPE_DOUBLE;
				break;

			// Date. Bind to String
			case DBTYPE_DATE:
			case DBTYPE_DBDATE:
			case DBTYPE_DBTIMESTAMP:
				m_Bindings[i].cbMaxLen = AUDB_MAX_DATETIME_SIZE * sizeof(TCHAR);
			  #ifndef _UNICODE
				m_Bindings[i].wType = DBTYPE_STR;
			  #else
				m_Bindings[i].wType = DBTYPE_WSTR;
			  #endif
				m_stColumns[i].m_eBindType = AUDB_TYPE_STRING;
				break;

			// String
			case DBTYPE_STR:
			case DBTYPE_WSTR:
			case DBTYPE_BSTR:
				m_Bindings[i].cbMaxLen = 
				   (m_pDBColumnInfo[i].ulColumnSize + 1) * sizeof(TCHAR);
			  #ifndef _UNICODE
				m_Bindings[i].wType = DBTYPE_STR;
			  #else
				m_Bindings[i].wType = DBTYPE_WSTR;
			  #endif
				m_stColumns[i].m_eBindType = AUDB_TYPE_STRING;
				break;

			// 모르는 넘 자질구레한 넘은 다 스트링이다 ㅋㅋㅋ 
			case DBTYPE_NULL:
			case DBTYPE_EMPTY:
			case DBTYPE_BOOL:
			case DBTYPE_CY:
			case DBTYPE_ERROR:
			case DBTYPE_BYTES:
			case DBTYPE_GUID:
			default:
				TRACE("Warning : Unknown(Unsupported) Datatype Binded\n");
				m_Bindings[i].cbMaxLen = AUDB_MAX_COLUMN_VALUE_SIZE;
			  #ifndef _UNICODE
				m_Bindings[i].wType = DBTYPE_STR;
			  #else
				m_Bindings[i].wType = DBTYPE_WSTR;
			  #endif
				m_stColumns[i].m_eBindType = AUDB_TYPE_STRING;
				break;
			}
		m_Bindings[i].bPrecision = m_pDBColumnInfo[i].bPrecision;
		m_Bindings[i].bScale = m_pDBColumnInfo[i].bScale;

		ulOffset += (ULONG)m_Bindings[i].cbMaxLen;
		}

	m_ulRowBufferSize  = ulOffset;
	}


//	Info. method
//================================================
//
BOOL AuRowsetOLEDB::IsEnd()
	{
	return m_bEnd;
	}


UINT32 AuRowsetOLEDB::GetOffset(UINT32 ulCol)
	{
	return (UINT32)m_Bindings[ulCol].obValue;
	}


//	Value method
//================================================
//
PVOID AuRowsetOLEDB::Get(UINT32 ulRow, UINT32 ulCol)
	{
	//ASSERT(m_ulRows > ulRow);
	//ASSERT(m_ulCols > ulCol);
	if (ulRow >= m_ulRows || ulCol > m_ulCols)
		return NULL;

	return // (LPCTSTR)
		m_Buffer.GetHead() + (ulRow * m_ulRowBufferSize + m_Bindings[ulCol].obValue);
	}


void AuRowsetOLEDB::Set(AuStatement& Statement)
	{
	Close();	// reset
	m_pStatement = &Statement;
	m_pIRowset = ((AuStmtOLEDB *)m_pStatement)->m_pIRowset;
	m_pIRowset->AddRef();
	}




/********************************************************/
/*		The Implementation of GetErrorInfo Function		*/
/********************************************************/
//
void AuGetErrorInfoOLEDB(LPTSTR pszError)
	{
	// General
	WCHAR			wszError[AUDB_DEFAULT_ERROR_STRING_SIZE];
	WCHAR*			pwszError;
	HRESULT			hr;
	IErrorInfo*		pIErrorInfo = NULL;
	BSTR			bstrDescription = NULL;
	BSTR			bstrSource = NULL;

	// Error Records
	IErrorRecords* pIErrorRecords = NULL;
	IErrorInfo*		pIErrorInfoRecord = NULL;
	ULONG			ulRecords = 0;
	static LCID		lcid = GetUserDefaultLCID();
	
	// SQL Error
	/*
	ERRORINFO		ErrorInfo;
	ISQLErrorInfo*	pISQLErrorInfo = NULL;
	BSTR			bstrSQLState = NULL;			
	LONG			lNativeError;
	*/

	wszError[0] = _T('\0');
	pwszError = wszError;
	hr = GetErrorInfo(0, &pIErrorInfo);
	if (SUCCEEDED(hr) && pIErrorInfo)
		{
		hr = pIErrorInfo->QueryInterface(IID_IErrorRecords, (void **)&pIErrorRecords);
		if (SUCCEEDED(hr))
			{
			pIErrorRecords->GetRecordCount(&ulRecords);
			for (ULONG i=0; i<ulRecords; i++)
				{
				pIErrorRecords->GetErrorInfo(i, lcid, &pIErrorInfoRecord);
				pIErrorInfoRecord->GetDescription(&bstrDescription);
				pIErrorInfoRecord->GetSource(&bstrSource);
				pwszError += swprintf(pwszError, L"[%s] %s", bstrSource, bstrDescription);

				/*	인터페이스가 거의 안걸린다. 위의걸로도 충분하니 걍 넘어가자. ㅋㅋ
				pIErrorRecords->GetCustomErrorObject(i, IID_ISQLErrorInfo, (IUnknown**)&pISQLErrorInfo);
				if (pISQLErrorInfo)
					{
					pISQLErrorInfo->GetSQLInfo(&bstrSQLState, &lNativeError);
					swprintf(wszError, L"\t%s", bstrSQLState);
					OutputDebugStringW(wszError);

					// add
					SysFreeString(bstrSQLState);
					}
				*/

				SysFreeString(bstrDescription);
				SysFreeString(bstrSource);
				}
			}
		else
			{
			pIErrorInfo->GetDescription(&bstrDescription);
			pIErrorInfo->GetSource(&bstrSource);

			pwszError += swprintf(pwszError, L"[%s] %s", bstrSource, bstrDescription);

			SysFreeString(bstrDescription);
			SysFreeString(bstrSource);
			}
		}
	OutputDebugStringW(wszError);

  #ifndef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, wszError, -1, pszError, AUDB_DEFAULT_ERROR_STRING_SIZE, NULL, NULL);
  #else
	_tcscpy(pszError, wszError);
  #endif

	if( pIErrorInfo )
		pIErrorInfo->Release();
	if( pIErrorRecords )
		{
		pIErrorRecords->Release();
		if (pIErrorInfoRecord)
			pIErrorInfoRecord->Release();
		}
	}



/*		Backup
eAUDB_TYPE_ AuRowsetOLEDB::GetColType(ULONG ulCol)
	{
	eAUDB_TYPE_ eType;

	switch (m_pDBColumnInfo[ulCol].wType)
		{
		// Integer
		case DBTYPE_I1:
			eType = AuDatabaseColumnTypeINT8;
			break;
		case DBTYPE_I2:
			eType = AuDatabaseColumnTypeINT16;
			break;
		case DBTYPE_I4:
			eType = AuDatabaseColumnTypeINT32;
			break;
		case DBTYPE_I8:
			eType = AuDatabaseColumnTypeINT64;
			break;
		case DBTYPE_UI1:
			eType = AuDatabaseColumnTypeUINT8;
			break;
		case DBTYPE_UI2:
			eType = AuDatabaseColumnTypeUINT16;
			break;
		case DBTYPE_UI4:
			eType = AuDatabaseColumnTypeUINT32;
			break;
		case DBTYPE_UI8:
			eType = AuDatabaseColumnTypeUINT64;
			break;

		// Decimal
		case DBTYPE_DECIMAL:
		case DBTYPE_NUMERIC:
		case DBTYPE_VARNUMERIC:
			if (0 == m_pDBColumnInfo[ulCol].bScale)
				eType = AuDatabaseColumnTypeINT64;
			else
				eType = AuDatabaseColumnTypeDOUBLE;
			break;

		// Floating Point
		case DBTYPE_R4:
			eType = AuDatabaseColumnTypeFLOAT;
			break;
		case DBTYPE_R8:
			eType = AuDatabaseColumnTypeDOUBLE;
			break;

		// Date. Bind to String
		case DBTYPE_DATE:
		case DBTYPE_DBDATE:
		case DBTYPE_DBTIMESTAMP:
			eType = AuDatabaseColumnTypeDATE;
			break;

		// String
		case DBTYPE_STR:
		case DBTYPE_WSTR:
		case DBTYPE_BSTR:
			eType = AuDatabaseColumnTypeSTRING;
			break;

		// 모르는 넘 자질구레한 넘은 다 스트링이다 ㅋㅋㅋ 
		case DBTYPE_NULL:
		case DBTYPE_EMPTY:
		case DBTYPE_BOOL:
		case DBTYPE_CY:
		case DBTYPE_ERROR:
		case DBTYPE_BYTES:
		case DBTYPE_GUID:
		default:
			TRACE("Warning : Unknown(Unsupported) Datatype Binded\n");
			eType = AuDatabaseColumnTypeUNSUPPORT;
			break;
		}
	return eType;
	}
*/

/*
	ULONG cParams = 1;
	DB_UPARAMS ulParamOrdinal = 0;
	DBPARAMBINDINFO dbParamBindInfo;

	ulParamOrdinal = 1;
	dbParamBindInfo.pwszName = NULL;
	dbParamBindInfo.ulParamSize = 16;
	dbParamBindInfo.bPrecision = 0;
	dbParamBindInfo.bScale = 0;
	dbParamBindInfo.pwszDataSourceType = L"DBTYPE_VARCHAR";
	dbParamBindInfo.dwFlags = DBPARAMFLAGS_ISINPUT;

	if (FAILED(pICommandWithParameters->SetParameterInfo(cParams, &ulParamOrdinal, &dbParamBindInfo)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}


	ULONG cBindings = 0;
	DBBINDING	rgBindings[1];
	ULONG		cbRowSize;
	DBLENGTH dwOffset = 0;

	//SetUp the Bindings
	rgBindings[0].iOrdinal	= 1;
	rgBindings[0].obStatus	= dwOffset;
	rgBindings[0].obLength	= dwOffset;
	rgBindings[0].obValue	= dwOffset;
	rgBindings[0].pTypeInfo = NULL;
	rgBindings[0].pBindExt  = NULL;
	rgBindings[0].dwPart	= DBPART_VALUE;
	rgBindings[0].dwMemOwner= DBMEMOWNER_CLIENTOWNED;
	rgBindings[0].eParamIO	= DBPARAMIO_NOTPARAM;
	rgBindings[0].eParamIO	= DBPARAMIO_INPUT;
	rgBindings[0].dwFlags	= 0;
	rgBindings[0].bPrecision= dbParamBindInfo.bPrecision;
	rgBindings[0].bScale	= dbParamBindInfo.bScale;
	rgBindings[0].pObject	= NULL;
	rgBindings[0].wType		= DBTYPE_STR;
	rgBindings[0].cbMaxLen	= 16;
	dwOffset += rgBindings[0].cbMaxLen;
	cBindings++;

	HACCESSOR hAccessor = NULL;
	IAccessor *pIAccessor = NULL;

	if (FAILED(pICommandWithParameters->QueryInterface(IID_IAccessor, (void**) &pIAccessor)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	if (FAILED(pIAccessor->CreateAccessor(DBACCESSOR_PARAMETERDATA, cBindings, rgBindings, dwOffset, &hAccessor, NULL))) 
		{
		AuGetErrorInfoOLEDB(m_szError);
		return FALSE;
		}

	DBPARAMS Params;
	TCHAR szData[16] = "raska";

	Params.cParamSets = 1;
	Params.hAccessor = hAccessor;
	Params.pData = szData;

	if (FAILED(m_pICommandText->Execute(NULL, IID_IRowset, &Params, &m_lAffectedRows, (IUnknown **)&m_pIRowset)))
		{
		AuGetErrorInfoOLEDB(m_szError);
		AUDBTRACE("Release ICommandText[%d] in AuStmtOLEDB::Execute()\n", m_pICommandText->Release());
		return FALSE;
		}

	AUDBTRACE("Release ICommandText[%d] in AuStmtOLEDB::Execute()\n", m_pICommandText->Release());
	
	return TRUE;
	}
*/




