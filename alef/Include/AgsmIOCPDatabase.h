// AgsmIOCPDatabase.h: interface for the AgsmIOCPDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMIOCPDATABASE_H__B77808DC_F3F8_47F4_95DC_464952B86111__INCLUDED_)
#define AFX_AGSMIOCPDATABASE_H__B77808DC_F3F8_47F4_95DC_464952B86111__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApIOCP.h"
#include "ApModule.h"
#include "auDatabase.h"
#include "ApMemory.h"
#include "AgsmDBPoolEvents.h"

typedef enum _eAgsmIOCPDatabaseApi
{
	AGSMIOCPDATABASE_API_OLEDB = 0,
	AGSMIOCPDATABASE_API_ODBC,
} eAgsmIOCPDatabaseApi;

// m_pfCallback, 생성자, 소멸자 추가 - 2004.03.04. steeple
class SQLBuffer : public ApMemory<SQLBuffer, 200000>	// 메모리 풀링 개수 20000 (20만) 개로 늘림 (기존 1000개) 2004.05.23. steeple
{
public:
	CHAR Buffer[1024 * 2];	// 최대 2K의 SQL문을 사용할수 있다.

	ApModuleDefaultCallBack m_pfCallback;
	PVOID	m_pClass;
	PVOID	m_pData;
	UINT32	m_ulNID;

public:
	SQLBuffer() { Init(); }
	virtual ~SQLBuffer() { Init(); }

	inline void Init() { memset(Buffer, 0, 1024*2); m_pfCallback = NULL; m_pClass = NULL; m_pData = NULL; }
	inline void SetCallbackParam(ApModuleDefaultCallBack pfCallback, PVOID pClass, PVOID pData)
	{
		m_pfCallback = pfCallback; m_pClass = pClass; m_pData = pData;
	}
};

class AgsmIOCPDatabase : public ApIOCP
{
private:
	friend class AgsmDBPool;
	AuDatabaseManager	m_DBManager;

	BOOL CreateDBConnection(AuDatabase** ppConnection,  AuStatement** ppStatement, AuRowset** ppRowset);
	BOOL PostSQL(SQLBuffer* pSQLBuffer);

	LPCTSTR m_szDataSource;
	LPCTSTR m_szUser;
	LPCTSTR m_szPassword;
	
public:
	AgsmDBPoolEvents	m_DBEvents;
	
public:
	AgsmIOCPDatabase();
	AgsmIOCPDatabase(eAgsmIOCPDatabaseApi eApi);
	virtual ~AgsmIOCPDatabase();

	BOOL InitConnectParam();
	BOOL SetConnectParam(LPCTSTR szDataSource, LPCTSTR szUser, LPCTSTR szPassword);

	BOOL Create(INT32 lDBConnectionCount);
	BOOL ExecuteSQL(AuStatement* pStatement, AuRowset* pRowset, SQLBuffer* pSQLBuffer);

	static UINT WINAPI DBWorkerThread(PVOID pParam);
};

#endif // !defined(AFX_AGSMIOCPDATABASE_H__B77808DC_F3F8_47F4_95DC_464952B86111__INCLUDED_)
