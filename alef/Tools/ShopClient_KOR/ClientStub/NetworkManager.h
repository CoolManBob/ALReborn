#pragma once

/**************************************************************************************************

작성일: 2008-07-08
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 네트웍 메인 부분을 처리하는 객체 

**************************************************************************************************/

#include <Net/ServerNetwork.h>
#include "SessionMgr.h"

class CNetworkManager : public WBANetwork::Thread
{
public:
	CNetworkManager(void);
	virtual ~CNetworkManager(void);

	// 네트웍 작업을 하기 위한 초기화
	bool Initialize(WBANetwork::LPFN_ErrorHandler errhandler, WBANetwork::ServerSession* ClinetSession);

private:
	// WBA 네트웍 라이브러리 네트웍 관련 객체
	WBANetwork::ServerNetwork	m_Network;	
	// WBA 네트웍 라이브러리 세션관리 관련 객체
	CSessionMgr					m_SessionMgr;

protected:
	// 부모 클래스에서 선언된 메소드 재정의 (스레드 실행 프로시져)
	virtual void Run();

private:
	// 스레드의 종료 여부를 검새하는 이벤트 핸들
	HANDLE m_hTerminateThread;
	//초기화가 되어있는지 검사하는 플레그
	bool m_bIsInit;

	//클라이언트 종료 처리
	void terminate();
};
