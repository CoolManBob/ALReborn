#pragma once
#include "sdlogindb.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","AdoEOF")
class PTSDK_STUFF CSQLServerLoginDB : public CSDLoginDB
{
public:
	CSQLServerLoginDB(void);
	~CSQLServerLoginDB(void);

	virtual bool IsOpen();
	virtual bool Open(const char * dbServer = NULL, const char * dbName = NULL, const char * userName = NULL, const char * userPwd = NULL, int port = 0);
	virtual void CloseDB2();

	virtual void SaveUserInfo( S_UserInfo & userInfo);	
	virtual int  GetUserInfo(int idtype, const char *id, S_UserInfo & userInfo);

protected:
	CWithLock m_ConnectionLock;
	_ConnectionPtr m_Connection;
};
