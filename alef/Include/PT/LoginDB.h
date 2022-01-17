#pragma once
#include "sdkconfig.h"
#include <string>
using namespace std;

class PTSDK_STUFF CLoginDB
{
public:
///enum IdType{ptid = 0, gmid = 1, sdid = 2};
	enum { cantquery=0, nodata, gotdata };
	struct S_UserInfo {
			string ptid;
			int    areaNum;
			string gmid;
			string originSdid;
			string userSdid;
			string password; 
			string ekeyflag;
			string reservePwd;
			int status;
	};

	virtual void SaveUserInfo( S_UserInfo & userInfo) = 0;	
	virtual int  GetUserInfo(int idtype, const char *id,S_UserInfo & userInfo) = 0;
};
