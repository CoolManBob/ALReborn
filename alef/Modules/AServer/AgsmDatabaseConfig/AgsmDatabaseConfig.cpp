/*===================================================================

	AgsmDatabaseConfig.cpp
		- Implements
			AgsdQueryConfig class
			AgsaQuery class
			AgsmDatabaseConfig class

====================================================================*/


#include "AgsmDatabaseConfig.h"


/************************************************************/
/*		The Implementation of AgsmDatabaseConfig class		*/
/************************************************************/
//
AgsmDatabaseConfig::AgsmDatabaseConfig()
	{
	m_pAgsmServerManager = NULL;
	m_nDatabase = 0;
	m_nLogPooIndex = -1;
	SetModuleName("AgsmDatabaseConfig");
	SetModuleType(APMODULE_TYPE_SERVER);
	SetModuleData(sizeof(AgsdQueryConfig), AGSMDATABASECONFIG_DATA_TYPE_QUERYCONFIG);
	}

	
AgsmDatabaseConfig::~AgsmDatabaseConfig()
	{
	}




//	ApModule inherited
//==============================================
//
BOOL AgsmDatabaseConfig::OnAddModule()
	{
	m_pAgsmServerManager = (AgsmServerManager *) GetModule("AgsmServerManager2");
	
	if (NULL == m_pAgsmServerManager)
		return FALSE;
	
	return TRUE;
	}


BOOL AgsmDatabaseConfig::OnInit()
	{
	m_DatabaseConfig.MemSetAll();
	
	// query map initialize
	m_csQueryMap.SetCount(255);
	if (!m_csQueryMap.InitializeObject(sizeof (AgsdQueryConfig *), m_csQueryMap.GetCount(), NULL, NULL, this))
		return FALSE;

	return TRUE;	
	}


BOOL AgsmDatabaseConfig::OnDestroy()
	{
	// destroy module data
	INT32 lIndex = 0;
	AgsdQueryConfig **ppQueryConfig = NULL;
	while (NULL != (ppQueryConfig  = (AgsdQueryConfig **) m_csQueryMap.GetObjectSequence(&lIndex)))
		{
		if (*ppQueryConfig)
			_DestroyQueryConfig(*ppQueryConfig);
		}

	// destroy map
	m_csQueryMap.RemoveObjectAll();
	return TRUE;
	}




//	Module data
//==============================================
//
AgsdQueryConfig* AgsmDatabaseConfig::_CreateQueryConfig()
	{
	AgsdQueryConfig *pcsQuery = (AgsdQueryConfig *) CreateModuleData(AGSMDATABASECONFIG_DATA_TYPE_QUERYCONFIG);
	pcsQuery->Init();
	return pcsQuery;
	}


BOOL AgsmDatabaseConfig::_DestroyQueryConfig(AgsdQueryConfig *pcsQuery)
	{
	return DestroyModuleData((PVOID) pcsQuery, AGSMDATABASECONFIG_DATA_TYPE_QUERYCONFIG);
	}




//	ReadConfig
//==============================================
//
BOOL AgsmDatabaseConfig::ReadConfig(CHAR *pszDatabase, CHAR *pszQuery)
	{
	m_nDatabase = 0;
	if (_ReadDatabase(pszDatabase) && _ReadQuery(pszQuery))
		return TRUE;
	return FALSE;
	}




#define _ctextend _T('\0')

//	ReadQuery
//==============================================
//
//	Read predefined query from INI
//
BOOL AgsmDatabaseConfig::_ReadQuery(CHAR *pszFile)
	{
	INT16	nIndex = -1;
	INT16	nDatabase = 0;

	// read ini
	CHAR sz[1024];
	GetPrivateProfileString("QUERY", NULL, "", sz, sizeof(sz), pszFile);
	CHAR *psz = sz;

	CHAR szKey[128];
	CHAR szValue[AGSMDBCONFIG_MAX_QUERY_STRING_LENGTH];
	CHAR *pszBuf = NULL;

	while (_ctextend != *psz)
		{
		pszBuf = szKey;
		while (_ctextend != *psz)
			*pszBuf++ = *psz++;
		*pszBuf = _ctextend;
		GetPrivateProfileString("QUERY", szKey, "", szValue, sizeof(szValue), pszFile);
		
		pszBuf = szKey;
		while (_ctextend != *pszBuf && ',' != *pszBuf)
			pszBuf++;

		CHAR *pszDB = NULL;		
		if (_ctextend != *pszBuf)
			{
			pszDB = pszBuf+1;		// next char * of ','
			*pszBuf = _ctextend;
			}
		
		nIndex = atoi(szKey);
		nDatabase = pszDB ? atoi(pszDB) : 0;
		if (!_AddQuery(nIndex, nDatabase, szValue))
			{
			return FALSE;
			}

		psz++;
		}

	return TRUE;
	}




//	ReadDatabase
//===================================================
//
//	Read Database(Connection) configuration from INI
//
BOOL AgsmDatabaseConfig::_ReadDatabase(CHAR *pszFile)
	{
	INT16 nPoolItem = 7;
	
	INT16 nConnection = 3;
	INT16 nConnectionItem = 1;
	INT16 nConnectionLog = 5;
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetThisServer();
	if (NULL == pAgsdServer)
		return FALSE;
	
	if ( 0 >= _tcslen(pAgsdServer->m_szDBUser)
		|| 0 >= _tcslen(pAgsdServer->m_szDBPwd)
		|| 0 >= _tcslen(pAgsdServer->m_szDBDSN)
		)
		return FALSE;

	// Game, default
	_tcscpy(m_DatabaseConfig[m_nDatabase].m_szDSN, pAgsdServer->m_szDBDSN);
	_tcscpy(m_DatabaseConfig[m_nDatabase].m_szUser, pAgsdServer->m_szDBUser);
	_tcscpy(m_DatabaseConfig[m_nDatabase].m_szPwd, pAgsdServer->m_szDBPwd);
	m_DatabaseConfig[m_nDatabase].m_nConnection = nConnection;
	m_DatabaseConfig[m_nDatabase].m_cType = AGSMDATABASECONFIG_DSN;
	m_DatabaseConfig[m_nDatabase].m_bForced = FALSE;
	if (0 == _tcslen(pAgsdServer->m_szDBVender) || 0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("ORACLE")))
	  #ifdef ORACLE_THROUGH_OCI
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("OCI"));
	  #else
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("ODBC"));
	  #endif
	else if (0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("MSSQL")))
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("ODBC"));
	else
		return FALSE;
	
	m_nDatabase++;
		
	// Game, item
	for (INT16 i=0; i<nPoolItem; ++i)
		{
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szDSN, pAgsdServer->m_szDBDSN);
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szUser, pAgsdServer->m_szDBUser);
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szPwd, pAgsdServer->m_szDBPwd);
		m_DatabaseConfig[m_nDatabase].m_nConnection = nConnectionItem;
		m_DatabaseConfig[m_nDatabase].m_cType = AGSMDATABASECONFIG_DSN;
		m_DatabaseConfig[m_nDatabase].m_bForced = TRUE;
		m_DatabaseConfig[m_nDatabase].m_bIsLogDB = FALSE;
		if (0 == _tcslen(pAgsdServer->m_szDBVender) || 0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("ORACLE")))
			_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("OCI"));
		else if (0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("MSSQL")))
			_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("ODBC"));
		else
			return FALSE;
		
		m_nDatabase ++;
		}
	
	// Log
	if ( 0 >= _tcslen(pAgsdServer->m_szLogDBUser)
		|| 0 >= _tcslen(pAgsdServer->m_szLogDBPwd)
		|| 0 >= _tcslen(pAgsdServer->m_szLogDBDSN)
		)
		return TRUE;

	m_nLogPooIndex = m_nDatabase;
	
	_tcscpy(m_DatabaseConfig[m_nDatabase].m_szDSN, pAgsdServer->m_szLogDBDSN);
	_tcscpy(m_DatabaseConfig[m_nDatabase].m_szUser, pAgsdServer->m_szLogDBUser);
	_tcscpy(m_DatabaseConfig[m_nDatabase].m_szPwd, pAgsdServer->m_szLogDBPwd);
	m_DatabaseConfig[m_nDatabase].m_nConnection = nConnectionLog;
	m_DatabaseConfig[m_nDatabase].m_cType = AGSMDATABASECONFIG_DSN;
	m_DatabaseConfig[m_nDatabase].m_bForced = FALSE;
	m_DatabaseConfig[m_nDatabase].m_bIsLogDB = TRUE;
	if (0 == _tcslen(pAgsdServer->m_szDBVender) || 0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("ORACLE")))
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("OCI"));
	else if (0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("MSSQL")))
		_tcscpy(m_DatabaseConfig[m_nDatabase].m_szLib, _T("ODBC"));
	else
		return FALSE;
	
	m_nDatabase++;
	
	return TRUE;
	}


BOOL AgsmDatabaseConfig::_ParseDatabaseString(INT16 nIndex, CHAR *psz)
	{
	if (nIndex >= AGSMDBCONFIG_MAX_DB ||
		nIndex < 0)
		return FALSE;

	CHAR szKey[32];
	CHAR szValue[AGSMDBCONFIG_MAX_DSN_LENGTH];
	CHAR *pszBuf = NULL;

	m_DatabaseConfig[nIndex].m_szDSN[0] = _ctextend;
	m_DatabaseConfig[nIndex].m_szUser[0] = _ctextend;
	m_DatabaseConfig[nIndex].m_szPwd[0] = _ctextend;
	m_DatabaseConfig[nIndex].m_nConnection = 0;
	m_DatabaseConfig[nIndex].m_cType = AGSMDATABASECONFIG_UNKNOWN;
	m_DatabaseConfig[nIndex].m_bForced = FALSE;
	m_DatabaseConfig[nIndex].m_szLib[0] = _ctextend;

	while (_ctextend != *psz)
		{
		// key
		pszBuf = szKey;
		while (_ctextend != *psz && _AGSMDATABASECONFIG_DELIM1 != *psz)
			*pszBuf++ = *psz++;
		if (_AGSMDATABASECONFIG_DELIM1 == *psz)
			psz++; // skip _AGSMDATABASECONFIG_DELIM1
		*pszBuf = _ctextend;
	
		// value
		pszBuf = szValue;
		while (_ctextend != *psz && _AGSMDATABASECONFIG_DELIM2 != *psz)
			*pszBuf++ = *psz++;
		if (_AGSMDATABASECONFIG_DELIM2 == *psz)
			psz++; // skip 
		*pszBuf = _ctextend;

		if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_DSN, strlen(_AGSMDATABASECONFIG_DSN)))
			{
			strncpy(m_DatabaseConfig[nIndex].m_szDSN, szValue, AGSMDBCONFIG_MAX_DSN_LENGTH - 1);
			m_DatabaseConfig[nIndex].m_cType = AGSMDATABASECONFIG_DSN;
			}
		else if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_USER, strlen(_AGSMDATABASECONFIG_USER)))
			strncpy(m_DatabaseConfig[nIndex].m_szUser, szValue, AGSMDBCONFIG_MAX_DSN_LENGTH - 1);
		else if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_PWD, strlen(_AGSMDATABASECONFIG_PWD)))
			strncpy(m_DatabaseConfig[nIndex].m_szPwd, szValue, AGSMDBCONFIG_MAX_DSN_LENGTH - 1);
		else if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_CONNECTION, strlen(_AGSMDATABASECONFIG_CONNECTION)))
			m_DatabaseConfig[nIndex].m_nConnection = atoi(szValue);
		else if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_CONNSTR, strlen(_AGSMDATABASECONFIG_CONNSTR)))
			{
			strncpy(m_DatabaseConfig[nIndex].m_szDSN, szValue, AGSMDBCONFIG_MAX_DSN_LENGTH - 1);
			m_DatabaseConfig[nIndex].m_cType = AGSMDATABASECONFIG_CONNSTR;
			}
		else if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_FORCED, strlen(_AGSMDATABASECONFIG_FORCED)))
			{
			if ('1' == szValue[0])
				m_DatabaseConfig[nIndex].m_bForced = TRUE;
			}
		else if (0 == strnicmp(szKey, _AGSMDATABASECONFIG_LIB, strlen(_AGSMDATABASECONFIG_LIB)))
			{
			strncpy(m_DatabaseConfig[nIndex].m_szLib, szValue, 9);
			m_DatabaseConfig[nIndex].m_szLib[9] = _ctextend;
			}
		}

	// valid check?
	if ((0 == m_DatabaseConfig[nIndex].m_nConnection) ||
		(AGSMDATABASECONFIG_UNKNOWN == m_DatabaseConfig[nIndex].m_cType))
		return FALSE;

	m_nDatabase++;
	return TRUE;
	}


BOOL AgsmDatabaseConfig::_AddQuery(INT16 nIndex, INT16 nDatabase, CHAR* pszQuery)
	{
	AgsdQueryConfig* pcsQuery = NULL;

	if (NULL != m_csQueryMap.GetQuery(nIndex))
		return FALSE;

	pcsQuery = _CreateQueryConfig();
	if (NULL == pcsQuery)
		return FALSE;

	strncpy(pcsQuery->m_szQuery, pszQuery, AGSMDBCONFIG_MAX_QUERY_STRING_LENGTH - 1);
	pcsQuery->m_nDatabase = nDatabase;

	if (NULL == m_csQueryMap.AddQuery(pcsQuery, nIndex))
		{
		_DestroyQueryConfig(pcsQuery);
		return FALSE;
		}
	return TRUE;	
	}

#undef _ctextend




//	Get
//=====================================================
//
CHAR* AgsmDatabaseConfig::GetQuery(INT32 lQueryIndex, INT16* pnDatabaseIndex)
	{
	AgsdQueryConfig* pQuery = m_csQueryMap.GetQuery(lQueryIndex);
	if (NULL == pQuery)
		return NULL;

	if (pnDatabaseIndex)
		*pnDatabaseIndex = pQuery->m_nDatabase;

	return pQuery->m_szQuery;
	}


AgsdQueryConfig* AgsmDatabaseConfig::GetQuery(INT32 lIndex)
	{
	return m_csQueryMap.GetQuery(lIndex);
	}


AgsdDatabaseConfig*	AgsmDatabaseConfig::GetDatabase(INT16 nIndex)
	{
	if (nIndex > m_nDatabase)
		return NULL;

	return &m_DatabaseConfig[nIndex];
	}


INT16 AgsmDatabaseConfig::GetDatabaseCount()
	{
	return m_nDatabase;
	}




/****************************************************/
/*		The Implementation of AgsaQuery class		*/
/****************************************************/
//
AgsaQuery::AgsaQuery()
	{
	m_nNumQuerys	= 0;
	}


AgsaQuery::~AgsaQuery()
	{
	}




//	Add/Remove
//===============================================
//
AgsdQueryConfig* AgsaQuery::AddQuery(AgsdQueryConfig *pcsQuery, INT32 lQueryID)
	{
	if (!pcsQuery || !AddObject((PVOID) &pcsQuery, lQueryID))
		return NULL;

	m_nNumQuerys++;

	return pcsQuery;
	}


BOOL AgsaQuery::RemoveQuery(INT32 lQueryID)
	{
	AgsdQueryConfig *pcsQuery = GetQuery(lQueryID);
	if (!pcsQuery)
		return FALSE;

	m_nNumQuerys--;
	return RemoveObject(lQueryID);
	}




//	GetQuery
//===============================================
//
AgsdQueryConfig* AgsaQuery::GetQuery(INT32 lQueryID)
	{
	AgsdQueryConfig **ppcsQuery = (AgsdQueryConfig **) GetObject(lQueryID);
	if (!ppcsQuery)
		return NULL;

	return *ppcsQuery;
	}

