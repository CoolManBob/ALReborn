#pragma once
#include "sdkconfig.h"
#include "SessionInfo.h"
#include "sdtalk\SDMessage.h"


class PTSDK_STUFF CClientInterface
{
public:
	enum RESULT{ failed = -1, sys_error = -2, time_out = -3, success = 0, 
		no_user, miss_pwd, refuse,
		no_bind_ekey, ekey_warp, need_reserve_pwd,
		user_existed, need_upgrade, sdid_existed, logined_otherwhere,
		gameid_existed, pt_unlogined, user_locked,
		need_ekey,
		last_Result
	};
	
	enum OPERATION{
		authen = 0,
		ekeyauth,
		reserveauth,
		regist,
		upgrade,
		checkid,
		changepwd,
		changeGameId,
		last_Operation
	};

	virtual void QuestEKey( CSessionPtr &p_session, const char *challengeNum) = 0 ;
	virtual void OperateResult( CSessionPtr &p_session, int operation, int result) = 0 ;
	virtual void RecvDownMessage(CSDMessage& v_msg) = 0 ;
};
