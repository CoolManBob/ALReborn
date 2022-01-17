#pragma once

/**************************************************************************************************

작성일: 2008-07-08
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 세션 리스트를 관리 하는 객체

**************************************************************************************************/

#include <Net/SessionManager.h>
#include <Net/ServerSession.h>

class CSessionMgr : public WBANetwork::SessionManager
{
public:
	CSessionMgr(void);
	~CSessionMgr(void);

	void BindClient(WBANetwork::ServerSession* ClinetSession);

protected:	
	virtual	WBANetwork::ServerSession*	CreateSession();
	virtual	void						DeleteSession( WBANetwork::ServerSession* session );

private:
	WBANetwork::ServerSession* m_ClinetSession;
};
