/*===================================================================

	AgsmDatabasePool.cpp

===================================================================*/

#include "AgsmDatabasePool.h"
#include "ApMemoryTracker.h"


const INT32 g_lInstantCommitInverval = 1000;
const INT32 g_lBufferingCommitInterval = 1000 * 60 * 5;		// 5분


/************************************************************/
/*		The Implementation of AgsmDatabasePool class		*/
/************************************************************/
//
AgsmDatabasePool::AgsmDatabasePool()
	{
	SetModuleName("AgsmDatabasePool");
	EnableIdle(TRUE);

	m_pAgsmDatabaseConfig = NULL;
	m_nTotalForced = 0;
	m_nForcedStart = 0;

	m_ulLastFlushBufferingCommitClock = 0;
	}


AgsmDatabasePool::~AgsmDatabasePool()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgsmDatabasePool::OnAddModule()
	{
	m_pIOCPDatabases = NULL;
	m_pAgsmDatabaseConfig = (AgsmDatabaseConfig *) GetModule("AgsmDatabaseConfig");
	m_pAgpmConfig = (AgpmConfig *) GetModule("AgpmConfig");
	if (!m_pAgsmDatabaseConfig)
		return FALSE;

	return TRUE;
	}


BOOL AgsmDatabasePool::OnInit()
	{
	return TRUE;
	}

BOOL AgsmDatabasePool::OnIdle(UINT32 ulClockCount)
{
	// Buffering Commit 루틴
	if(ulClockCount - m_ulLastFlushBufferingCommitClock > g_lBufferingCommitInterval)
	{
		m_ulLastFlushBufferingCommitClock = ulClockCount;

		// Database Pool Thread 에서 Buffering Commit 인 녀석들을 Flush 시켜준다.
		INT16 nDatabase = m_pAgsmDatabaseConfig->GetDatabaseCount();
		for(INT16 i = 0; i < nDatabase; ++i)
		{
			if(m_pIOCPDatabases[i].m_bBufferingCommit)
				m_pIOCPDatabases[i].FlushAll();
		}
	}

	return TRUE;
}


BOOL AgsmDatabasePool::OnDestroy()
	{
	for (INT16 i=0; i<m_pAgsmDatabaseConfig->GetDatabaseCount(); i++)
		{
		if(m_pIOCPDatabases[i].m_bBufferingCommit)
			m_pIOCPDatabases[i].FlushAll();

		m_pIOCPDatabases[i].Destroy();
		}

	delete [] m_pIOCPDatabases;

	AuDatabaseBuffer::Cleanup();
	return TRUE;
	}




//	Create
//=======================================
//
CHAR g_szDatabaseLib[AUDB_API_NUM][10] =
	{
	"OLEDB",
	"ODBC",
	"OCI"
	};


BOOL AgsmDatabasePool::Create()
	{
	INT16 nDatabase = m_pAgsmDatabaseConfig->GetDatabaseCount();
	if (0 >= nDatabase)
		return FALSE;

	//AuDatabaseBuffer::Initialize(10);
	m_pIOCPDatabases = new IOCPDatabase[nDatabase];

	for (INT16 i=0; i<nDatabase; i++)
		{
		AgsdDatabaseConfig* pConfig = NULL;
		pConfig = m_pAgsmDatabaseConfig->GetDatabase(i);
		if (NULL == pConfig)
			return FALSE;

		if (AGSMDATABASECONFIG_DSN == pConfig->m_cType)
			{
			m_pIOCPDatabases[i].m_cType = 0;
			strncpy(m_pIOCPDatabases[i].m_szDSN, pConfig->m_szDSN, _MAX_CONN_PARAM_STRING_LENGTH - 1);
			strncpy(m_pIOCPDatabases[i].m_szUser, pConfig->m_szUser, _MAX_CONN_PARAM_STRING_LENGTH - 1);
			strncpy(m_pIOCPDatabases[i].m_szPwd, pConfig->m_szPwd, _MAX_CONN_PARAM_STRING_LENGTH - 1);
			}
		else if (AGSMDATABASECONFIG_CONNSTR == pConfig->m_cType)
			{
			m_pIOCPDatabases[i].m_cType = 1;
			strncpy(m_pIOCPDatabases[i].m_szDSN, pConfig->m_szDSN, _MAX_CONN_PARAM_STRING_LENGTH - 1);
			}

		if (pConfig->m_bForced)
			{
			++m_nTotalForced;
			if (0 == m_nForcedStart)
				m_nForcedStart = i;
			}

		if (pConfig->m_bIsLogDB
			&& m_pAgpmConfig
			&& m_pAgpmConfig->IsIgnoreLogFail())
			{
			m_pIOCPDatabases[i].m_bIgnoreFail = TRUE;
			}

		// 2008.01.06. steeple
		if (pConfig->m_bIsLogDB)
			m_pIOCPDatabases[i].m_bBufferingCommit = TRUE;

		for (INT16 x=0; x<AUDB_API_NUM; ++x)
			{
			if (0 == strnicmp(pConfig->m_szLib, g_szDatabaseLib[x], strlen(g_szDatabaseLib[x])))
				{
				m_pIOCPDatabases[i].m_eApi  = (eAUDB_API) x;
				break;
				}
			}

		m_pIOCPDatabases[i].Create(pConfig->m_nConnection);
		}

	if (0 == m_nTotalForced)
		m_nTotalForced = 1;

	return TRUE;
	}




//	Execute
//=======================================
//
BOOL AgsmDatabasePool::Execute(AgsdQuery* pQuery, INT16 nForcedIndex)
	{
	if (!m_pIOCPDatabases)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmDatabasePool::Execute(), !m_pIOCPDatabases. nForcedIndex:%d\n", nForcedIndex);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}
	
	INT16 nIndex = 0;
	if (0 != pQuery->m_nIndex)
		{
		// get query
		CHAR *pszQuery = m_pAgsmDatabaseConfig->GetQuery(pQuery->m_nIndex, &nIndex);
		if (NULL != pszQuery)
			pQuery->Set(pszQuery);
		else
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmDatabasePool::Execute(), pszQuery is NULL. QueryIndex:%d, nIndex:%d, nForcedIndex:%d\n",
										pQuery->m_nIndex, nIndex, nForcedIndex);
			AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
			}
		}

	// if forced, ignore index setting of configuration file
	if (0 != nForcedIndex)
		nIndex = nForcedIndex;

	if (nIndex >= m_pAgsmDatabaseConfig->GetDatabaseCount())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmDatabasePool::Execute(), nIndex >= m_pAgsmDatabaseConfig->GetDatabaseCount() QueryIndex:%d, nForcedIndex:%d, nIndex:%d, DatabasesCount:%d\n",
									pQuery->m_nIndex, nForcedIndex, nIndex, m_pAgsmDatabaseConfig->GetDatabaseCount());
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}

	BOOL bPosted = m_pIOCPDatabases[nIndex].PostQuery(pQuery);
	if(bPosted == FALSE)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmDatabasePool::Execute(), bPosted is FALSE. QueryIndex:%d, nForcedIndex:%d, nIndex:%d, DatabasesCount:%d\n",
									pQuery->m_nIndex, nForcedIndex, nIndex, m_pAgsmDatabaseConfig->GetDatabaseCount());
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		}
	return bPosted;
	}


//	Status
//==========================================
//
INT32 AgsmDatabasePool::GetPosted(INT16 nIndex)
	{
	INT32 lCount = 0;
	
	if (-1000 == nIndex)
		{
		for (INT16 i = 0; i < m_pAgsmDatabaseConfig->GetDatabaseCount(); i++)
			{
			lCount += m_pIOCPDatabases[i].GetPosted();
			}
		}
	else
		{
		if (nIndex >= 0 && nIndex < m_pAgsmDatabaseConfig->GetDatabaseCount())
			lCount = m_pIOCPDatabases[nIndex].GetPosted();
		}
	
	return lCount;
	}


INT32 AgsmDatabasePool::GetExecuted(INT16 nIndex)
	{
	INT32 lCount = 0;
	
	if (-1000 == nIndex)
		{
		for (INT16 i = 0; i < m_pAgsmDatabaseConfig->GetDatabaseCount(); i++)
			{
			lCount += m_pIOCPDatabases[i].GetExecuted();
			}
		}
	else
		{
		if (nIndex >= 0 && nIndex < m_pAgsmDatabaseConfig->GetDatabaseCount())
			lCount = m_pIOCPDatabases[nIndex].GetExecuted();
		}
	
	return lCount;
	}


INT32 AgsmDatabasePool::GetFailed(INT16 nIndex)
	{
	INT32 lCount = 0;
	
	if (-1000 == nIndex)
		{
		for (INT16 i = 0; i < m_pAgsmDatabaseConfig->GetDatabaseCount(); i++)
			{
			lCount += m_pIOCPDatabases[i].GetFailed();
			}
		}
	else
		{
		if (nIndex >= 0 && nIndex < m_pAgsmDatabaseConfig->GetDatabaseCount())
			lCount = m_pIOCPDatabases[nIndex].GetFailed();
		}
	
	return lCount;
	}