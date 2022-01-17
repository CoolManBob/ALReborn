#pragma once
#include "sdlogindb.h"
#include <winsock.h>
#include <mysql.h>
#include "QueueWithLock.h"

#ifdef MYSQL_ENABLE

class PTSDK_STUFF CMySqlLoginDB : public CSDLoginDB
{
public:
	CMySqlLoginDB(void);
	~CMySqlLoginDB(void);

	virtual bool IsOpen();
	virtual bool Open(const char * dbServer = NULL, const char * dbName = NULL, const char * userName = NULL, const char * userPwd = NULL, int port = 0);
	virtual void CloseDB2();

	virtual void SaveUserInfo( S_UserInfo & userInfo);	
	virtual int  GetUserInfo(int idtype, const char *id, S_UserInfo & userInfo);

protected:
	MYSQL * MakeMysql();
	MYSQL * GetMysql(int v_flag = 0);
	bool	GetQuerySql ( MYSQL  *);
	void	ReleaseMysql( MYSQL *);
	void	DestroyMysql( MYSQL *);

	CQueueWithLock<MYSQL *> m_MySqls;

	CWithLock m_CountLock;
	int m_MaxConCount, m_CurConCount;
	int m_nCount;
	bool m_bValid;

	vector<string> m_CommandVector;
};

#endif
