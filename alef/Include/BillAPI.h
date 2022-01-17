#pragma once

#include <windows.h>
#include <string>

#include "bsipfun.h"
#include "APIFunc.h"
#include "CallbackFunc.h"

#define BSIP_ERROR -1
#define BSIP_OK 0

class CBillAPI  
{
public:
	CBillAPI();
	virtual ~CBillAPI();

private:
	std::string m_fileName;
	HINSTANCE	m_hDll;

public:
	GSCallbackFuncDef m_funCallBack;
	GSBECallbackFuncDef m_funCallbackBEAlert;

public:
	int IntSockEnv();
	int LoadDll();
	int LoadDll(std::string fileName);
	int GetFunc();
	int SetCallBackFunc();
	int FreeDll();

public:
	FunGSInitializeEx						m_pGSInitializeEx;
	FunGSInitialize							m_pGSInitialize;
	FunUninitialize                         m_pUninitialize;
	FunSendAuthorRequest                    m_pSendAuthorRequest;
	FunSendAccountRequest                   m_pSendAccountRequest;
	FunSendAccountAuthenRequest             m_pSendAccountAuthenRequest;
	FunSendAccountLockRequest               m_pSendAccountLockRequest;
	FunSendAccountUnlockRequest             m_pSendAccountUnlockRequest;
	FunSendAccountLockExRequest             m_pSendAccountLockExRequest;
	FunSendAccountUnlockExRequest           m_pSendAccountUnlockExRequest;
	FunSendAwardAuthenRequest               m_pSendAwardAuthenRequest;
	FunSendAwardAck                         m_pSendAwardAck;
	FunSendConsignLockRequest               m_pSendConsignLockRequest;
	FunSendConsignUnlockRequest             m_pSendConsignUnlockRequest;
	FunSendConsignDepositRequest            m_pSendConsignDepositRequest;
	FunSendGoldDepositResponse              m_pSendGoldDepositResponse;
	FunSendGoldConsumeLockRequest           m_pSendGoldConsumeLockRequest;
	FunSendGoldConsumeUnlockRequest         m_pSendGoldConsumeUnlockRequest;
	FunSendDepositRequest                   m_pSendDepositRequest;
	FunSendDepositAck                       m_pSendDepositAck;
	FunSendNotifyRequest                    m_pSendNotifyRequest;

	FunPlayerLogin							m_pSendLoginRequest;
	FunPlayerLogout							m_pSendLogoutRequest;
	FunPlayerReLogin						m_pSendReLoginRequest;
	FunGetNewId                             m_pGetNewId;
	FunGetNewIdByParam                      m_pGetNewIdByParam;
};
