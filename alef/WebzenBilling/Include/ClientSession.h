#pragma once

/**************************************************************************************************

작성일: 2008-07-10
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 클라이언트에게 세션관리를 할수 있는 인터페이스 객체

**************************************************************************************************/

#include <Net/ServerSession.h>
#include "NetworkManager.h"

class CClientSession : public WBANetwork::ServerSession
{
public:
	CClientSession(void);
	virtual ~CClientSession(void);

	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);
	//void Connect(TCHAR* ipAddress, unsigned short portNo);
	//void CloseSocket();
	bool SendData(void* buffer, DWORD size);
	

protected:
	virtual	void OnAccept() {};
	virtual	void OnSend( int size ) {};
	//virtual	void OnReceive( PBYTE buffer, int size ) {};
	virtual	void OnClose( DWORD error ) {};	

	CNetworkManager		m_NetworkManager;
};
