#pragma once

#ifndef PTSDK_DLL
#	define PTSDK_DLL
#endif
#include "ServerInterface.h"
#include "ConfigInterface.h"
#include "InfoCast.h"

class PTSDK_STUFF CAccountServer :
	public CServerInterface/*, private CInfoNode*/, private CInfoViewer
{
private:
	CAccountServer(void);
	~CAccountServer(void);
public:
	static CAccountServer * NewInstance();
	static void DeleteInstance(CAccountServer * p_as);
	CInfoNode * GetInfoNode();
	void Init(const char * iniFileName);
	void Init(CConfigInterface & config);

	void SetClientInterface(CClientInterface * p_client);

	// 认证PT账号和密码的正确性
	void PTAuthen( 
		CSessionPtr & p_session,			// 交互会话上下文
		const char * ptAccount,				// PT账号
		const char * password,				// 密码
		const char * pwdEncryptCode = NULL	// 密码加密方式
		);	

	// 使用游戏账号进行认证
	void GMAuthen(
		CSessionPtr & p_session,			// 交互会话上下文
		const char * gmAccount,				// 游戏账户
		const char * password,				// 密码
		const char * pwdEncryptCode = NULL	// 密码加密方式
		);

	(operator CInfoNode &)();
	(operator CServerInterface&)();
};

class PTSDK_STUFF CAccountServerPtr
{
public:
	CAccountServerPtr();
	~CAccountServerPtr();

	CAccountServer * GetPoint();
	CAccountServer& operator*(void);
	CAccountServer * operator->(void);
	CInfoNode	   * GetInfoNode();

private:
	CAccountServer * m_pAS;
};
