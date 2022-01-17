#pragma once
#include "sdkconfig.h"
#include "serverinterface.h"
#include "TCPConnection.h"
#include "DispatchSvr.h"
#include "ClientAgent.h"
#include "SDLoginDB.h"
#include "SDPacketEncrypt.h"
#include "AuthProcess.h"
#include "InfoCast.h"
#include "IniConfig.h"

class PTSDK_STUFF CAccountServer :
	public CServerInterface, private CInfoNode, public CInfoViewer
{
private:
	CAccountServer(void);
	~CAccountServer(void);
	
	//void myPTAuthen( 
	//	CSessionPtr & p_session,			// 交互会话上下文，需置入"ClientIP"
	//	const char * ptAccount,				// PT账号
	//	const char * password,				// 密码
	//	const char * pwdEncryptCode = NULL	// 密码加密方式
	//	);

public:
	static CAccountServer * NewInstance();
	static void DeleteInstance(CAccountServer * p_as);
	CInfoNode * GetInfoNode();
	void Init(const char * iniFileName);
	void Init(CConfigInterface & config);

	void SetClientInterface(CClientInterface * p_client);

	// 认证PT账号和密码的正确性
	void PTAuthen( 
		CSessionPtr & p_session,			// 交互会话上下文，需置入"ClientIP"
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

protected:

	CSDPacketEncrypt	m_Encrypter,
						m_Decrypter;

	CSDMsgPipe			m_ASPipe;

	vector<CTCPConnection*>	m_TCPConnections;

	CDispatchSvr		m_DispatchSvr;

	CAuthProcess		m_PreProcess;

	CSDLoginDB *		m_pLoginDB;

	static void ConnectionCheck(void * parameter);	
	CThreadMaster m_KeepConnection;

	vector< pair<string, int> > m_StaticAS;

	void Clear();

	bool ConnectToAS( CTCPConnection &connection);
	void MakeFirstPacket(CSDPacket & packet);
	virtual void GotInfo(int intInfo, const char * strInfo);
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
