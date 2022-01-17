// AgsmDBPool.h: interface for the AgsmDBPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMDBPOOL_H__EED31C63_6B3E_4076_9806_07BF54A0AB87__INCLUDED_)
#define AFX_AGSMDBPOOL_H__EED31C63_6B3E_4076_9806_07BF54A0AB87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgsmIOCPDatabase.h"
#include "AgsEngine.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmDBPoolD" )
#else
#pragma comment ( lib , "AgsmDBPool" )
#endif
#endif

// 2004.02.29. steeple
// 외부에서 세팅 가능하게 변경
#define AGSMDBPOOL_DATASOURCE_STRING_LENGTH	32
#define AGSMDBPOOL_USER_STRING_LENGTH		32
#define AGSMDBPOOL_PASSWORD_STRING_LENGTH		32

class AgsmDBPool : public AgsModule
{
private:
	AgsmIOCPDatabase*	m_pIOCPDatabase;
	
	CHAR m_szDataSource[AGSMDBPOOL_DATASOURCE_STRING_LENGTH+1];
	CHAR m_szUser[AGSMDBPOOL_USER_STRING_LENGTH+1];
	CHAR m_szPassword[AGSMDBPOOL_PASSWORD_STRING_LENGTH+1];

public:
	AgsmDBPool();
	virtual ~AgsmDBPool();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();

	BOOL SetConnectParam(LPCTSTR szDataSource, LPCTSTR szUser, LPCTSTR szPassword);
	BOOL CreateIOCPDatabase(eAgsmIOCPDatabaseApi eApi = AGSMIOCPDATABASE_API_OLEDB);

	BOOL Create(INT32 lDBConnectionCount);
	INT32 GetDBConnectionCount();
	BOOL ExecuteSQL(SQLBuffer* pSQLBuffer);
};

#endif // !defined(AFX_AGSMDBPOOL_H__EED31C63_6B3E_4076_9806_07BF54A0AB87__INCLUDED_)
