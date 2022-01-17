#pragma once
#include "sdkconfig.h"
#include "SessionInfo.h"
#include "SessionManage.h"
#include "ClientInterface.h"
#include "SDMsgPipe.h"
#include "SDMsgProcessor.h"
#include "ThreadMaster.h"
#include "LoginDB.h"
#include "QueueWithLock.h"
#include "Lock\Lock.h"
using namespace CleverLock;

typedef map<unsigned int, CSDMessage*> MAP_MSGEX;

class PTSDK_STUFF CServerInterface: public CSDMsgProcessor, public CSessionOwner, public CWithLock
{
public:
	CServerInterface(void);
	~CServerInterface(void);

	enum IdType{sdid = 0, ptid = 1, gmid = 2};

	// 认证PT账号和密码的正确性
	void PTAuthen( 
		CSessionPtr & p_session,			// 交互会话上下文
		const char * ptAccount,				// PT账号
		const char * password,				// 密码
		const char * pwdEncryptCode = NULL	// 密码加密方式
											// "0",NULL: 不加密
											// "1": SHA1 对于SHA1中出现的0X00用0X01替换
											// "2": MD5  对于MD5后出现的0X00用0X01替换
											// "3": 3DES 加密后的口令使用BASE64编码
											// "4": 采用老的边锋加密算法(以转换成小写字符串后的游戏账号做为密钥对密码进行加密
		);	

	// 使用游戏账号进行认证
	void GMAuthen(
		CSessionPtr & p_session,			// 交互会话上下文
		const char * gmAccount,				// 游戏账户
		const char * password,				// 密码
		const char * pwdEncryptCode = NULL	// 密码加密方式
											// "0",NULL: 不加密
											// "1": SHA1 对于SHA1中出现的0X00用0X01替换
											// "2": MD5  对于MD5后出现的0X00用0X01替换
											// "3": 3DES 加密后的口令使用BASE64编码
											// "4": 采用老的边锋加密算法(以转换成小写字符串后的游戏账号做为密钥对密码进行加密
		);

	// 为密保验证提供密保,如果未经过静态密码论证，需要手动设置Session会话状态中的三个变量
	//“ptid”，“ekeyType”，"challenge"
	void GiveEkey (
		CSessionPtr & p_session,			// 交互会话上下文
		const char * ekey					// 用户输入的密保号
		);

	// 认证用户的保留密码,如果未经过密保论证,需手动设置Session会话状态中的变量"ptid"
	void GiveReservePwd	( 
		CSessionPtr & p_session,			// 交互会话上下文
		const char * reservePwd				// 用户的保留密码
		);

	// 用户修改密码
	void ChangePassword	( 
		CSessionPtr & p_session,			// 交互会话上下文
		const char * id,					// 数字ID或POPTANG ID
		const char * oldPassword,			// 老静态密码
		const char * newPassword,			// 新静态密码
		const char * ekey,					// 密保
		const char * challenge				// 挑战码
		);

	// 检查账号是否存在
	int CheckAccount( 
		CSessionPtr & p_session ,			// 交互会话上下文
		const char * ptAccount				// 待检查的账号
		);

	// 升级游戏账号为PT账号
	void UpgradeAccount	( 
		CSessionPtr & p_session,			// 交互会话上下文		
		const char * gmAccount,				// 游戏帐号
		const char * ptId,					// POPTANG ID
		const char * password,				// 密码
		const char * name,					// 姓名
		const char * question1,				// 密码提示问题1
		const char * answer1,				// 密码提示问题答案1
		const char * question2,				// 密码提示问题2
		const char * answer2,				// 密码提示问题答案2
		const char * peopleId,				// 身份证
		const char * email,					// 电子邮件
		const char * telnum,				// 电话
		const char * mobilenum,				// 手机号码
		const char * birthday,				// 生日
		const char * address,				// 地址
		const char * privateInfo,			// 个人信息
		const char * sex,					// 性别
		const char * ip,					// IP地址
		const char * otherInfo = NULL		// 其它信息,目前只有游戏的数字帐号
		);


	// 注册一个PT账号
	void RegistPT( 
		CSessionPtr & p_session,			// 交互会话上下文
		const char * ptId,					// POPTANG ID
		const char * password,				// 密码
		const char * name,					// 姓名
		const char * question1,				// 密码提示问题1
		const char * answer1,				// 密码提示问题答案1
		const char * question2,				// 密码提示问题2
		const char * answer2,				// 密码提示问题答案2
		const char * peopleId,				// 身份证
		const char * email,					// 电子邮件
		const char * telnum,				// 电话
		const char * mobilenum,				// 手机号码
		const char * birthday,				// 生日
		const char * address,				// 地址
		const char * privateInfo,			// 个人信息
		const char * sex,					// 性别
		const char * ip						// IP地址
		);

	// 通过pt帐号修改游戏帐号
	void ChangeGameID(
		CSessionPtr & p_session,			// 交互会话上下文
		const char * ptId,					// POPTANG ID
		const char * newGameID				// 新的游戏账号
		);

	void NewSession( CSessionPtr & p_session);
	void EndSession( CSessionPtr & p_session);

	static void MakeChallenge(string & challenge, int length);

	bool IsConnect();

	void SetLoginDB(CLoginDB * p_loginDB);
	void SetClientInterface( CClientInterface * p_client);
	void SetASPipe( CSDMsgPipe * p_asPipe);

	void SetWorkThreadCount(int count = 1);
	void SetSessionLife(int life);

	virtual void GotInfo(int intInfo, const char * strInfo)=0;

protected:

	int	m_GameType,		m_AreaNum;
	CLoginDB*			m_pLoginDB;
	CClientInterface*	m_pClientInterface;
	CSDMsgPipe*			m_pASPipe;
	CMutiThreadMaster	m_MsgProThread;
	CThreadMaster		m_ReboundMsgProThread;
	CThreadMaster		m_KeepAliveThread;
	//int	m_nLastSend;
	int m_nKeepAliveTimeout;

	CQueueWithLock<CSDMessage*> m_ComeMsgs;
	CQueueWithLock<CSDMessage*> m_ReboundMsgs;
	void Clear();

	void SetMsg( CSDMessage & msg);
	bool IsNum( const char * id);

	int  SendSDMessage( CSDMessage & msg );

	virtual int ProcessMsg(CSDMessage * p_msg, int flag = 0);
	static	void ProcessComeMsg(void * parameter);
	static	void ProcessReboundMsg(void * parameter);
	static	void KeepAliveThread(void * parameter);

	void LocalAuthen(CSessionPtr & p_session, int idType, const char *id, const char * password);
	int  LocalCheck(int idType, const char *id, const char * password, CLoginDB::S_UserInfo &userInfo);	
	void LocalReservePwdAuthen( CSessionPtr & p_session , const char * reservePwd);

    
	bool PtAuthResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool EkeyAuthResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool UpgradeResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool RegistResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool ChangePWDResult	( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool CheckAccountResult	( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool ChangeGameIDResult ( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool PreProcessComeMsg	(CSDMessage& sdmessage);

	virtual void SessionTimeOut(CSessionPtr &session);
	unsigned int	m_SessionId;
	CSessionManage	m_SessionManage;

private:
	MAP_MSGEX m_mapMsgEx;
	CWithLock m_lockMsgEx;
	void PushMsgEx(CSDMessage* v_msg);
	CSDMessage* PopMsgEx(unsigned int v_transid);
};
