// AgsmDBPool.cpp: implementation of the AgsmDBPool class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmDBPool.h"
#include "ApMemoryTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmDBPool::AgsmDBPool()
{
	SetModuleName("AgsmDBPool");
	
	m_pIOCPDatabase = NULL;

	memset(m_szDataSource, 0, AGSMDBPOOL_DATASOURCE_STRING_LENGTH+1);
	memset(m_szUser, 0, AGSMDBPOOL_USER_STRING_LENGTH+1);
	memset(m_szPassword, 0, AGSMDBPOOL_PASSWORD_STRING_LENGTH+1);
}

AgsmDBPool::~AgsmDBPool()
{
	if(m_pIOCPDatabase)
		delete m_pIOCPDatabase;
}

BOOL AgsmDBPool::OnAddModule()
{
	return TRUE;
}

BOOL AgsmDBPool::OnInit()
{
	return TRUE;
}

BOOL AgsmDBPool::OnDestroy()
{
	return TRUE;
}

BOOL AgsmDBPool::SetConnectParam(LPCTSTR szDataSource, LPCTSTR szUser, LPCTSTR szPassword)
{
	if(!szDataSource || !szUser || !szPassword)
		return FALSE;

	strncpy(m_szDataSource, szDataSource, AGSMDBPOOL_DATASOURCE_STRING_LENGTH);
	strncpy(m_szUser, szUser, AGSMDBPOOL_USER_STRING_LENGTH);
	strncpy(m_szPassword, szPassword, AGSMDBPOOL_PASSWORD_STRING_LENGTH);

	if(m_pIOCPDatabase)
		m_pIOCPDatabase->SetConnectParam(m_szDataSource, m_szUser, m_szPassword);

	return TRUE;
}

BOOL AgsmDBPool::CreateIOCPDatabase(eAgsmIOCPDatabaseApi eApi)
{
	m_pIOCPDatabase = new AgsmIOCPDatabase(eApi);
	return TRUE;
}

BOOL AgsmDBPool::ExecuteSQL(SQLBuffer* pSQLBuffer)
{
	if(!m_pIOCPDatabase)
		return FALSE;

	return m_pIOCPDatabase->PostSQL(pSQLBuffer);
}

INT32 AgsmDBPool::GetDBConnectionCount()
{
	if(!m_pIOCPDatabase)
		return FALSE;

	return m_pIOCPDatabase->GetThreadCount();
}

BOOL AgsmDBPool::Create(INT32 lDBConnectionCount)
{
	if(!m_pIOCPDatabase)
		return FALSE;

	return m_pIOCPDatabase->Create(lDBConnectionCount);
}