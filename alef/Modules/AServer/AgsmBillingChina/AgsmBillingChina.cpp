#include "AgsmBillingChina.h"
#include "AgsmCashMall.h"
#include "BillAPI.h"
#include "AuTimeStamp.h"

#include <stdio.h>

// -========================================================================-
CBillingBridge g_billingBridge;

namespace
{
	CBillAPI billing;
}

// -========================================================================-

CBillingBridge::CBillingBridge()
{
	InitializeCriticalSection(&m_cs);
}

CBillingBridge::~CBillingBridge()
{
	DeleteCriticalSection(&m_cs);
}

void CBillingBridge::SetCharManager(AgpmCharacter* pmChar, AgsmCharacter* smChar)
{
	m_pAgpmCharacter = pmChar;
	m_pAgsmCharacter = smChar;
}

void CBillingBridge::SetItemManager(AgpmItem* pmItem, AgsmItem* smItem, AgsmItemManager* itemManager)
{
	m_pAgpmItem = pmItem;
	m_pAgsmItem = smItem;
	m_pAgsmItemManager = itemManager;
}

void CBillingBridge::SetCashMallManager(AgpmCashMall* pmMall, AgsmCashMall* smMall)
{
	m_pAgpmCashMall = pmMall;
	m_pAgsmCashMall = smMall;
}

void CBillingBridge::SetBilling(AgsmBilling* billing)
{
	m_pAgsmBilling = billing;
}

void CBillingBridge::Add(char* uid, AgsdBilling* billing)
{
	Lock lock(m_cs);
	
	StrBillingMap::iterator iter = m_map.find(uid);
	if (iter == m_map.end()) m_map.erase(uid);

	m_map[uid] = billing;
}

AgsdBilling* CBillingBridge::Find(char* uid)
{
	Lock lock(m_cs);

	StrBillingMap::iterator iter = m_map.find(uid);
	return iter == m_map.end() ? 0 : iter->second;
}

void CBillingBridge::Remove(char* uid)
{
	Lock lock(m_cs);
	m_map.erase(uid);
}

// -========================================================================-

AgsmBillingChina::AgsmBillingChina()
:	m_billing(billing)
{
	m_eBsipErr				= BSIP_ERR_NONE;
	m_err					= 0;
	m_ulInitializedClock	= 0;
	m_pApModule				= NULL;

	memset(m_logFileName, 0, sizeof(m_logFileName));

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(m_logFileName, "LOG\\CashLog_%d%02d%02d_%02d%02d%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

AgsmBillingChina::~AgsmBillingChina()
{
	m_billing.m_pUninitialize(0);
	m_billing.FreeDll();
}

bool AgsmBillingChina::Init(ApModule *pModule)
{
	if (pModule)
		m_pApModule	= pModule;

	if (m_eBsipErr <= BSIP_ERR_LOAD_LIBRARY &&
		m_billing.LoadDll() != BSIP_OK)
	{
		printf("Can't load BSIP dll\n");
		m_eBsipErr	= BSIP_ERR_LOAD_LIBRARY;
		return false;
	}

	if (m_eBsipErr <= BSIP_ERR_GET_FUNC_ADDR &&
		m_billing.GetFunc() != BSIP_OK)
	{
		m_eBsipErr	= BSIP_ERR_GET_FUNC_ADDR;
		printf("Can't get func address\n");
		return false;
	}

	if (m_eBsipErr <= BSIP_ERR_SET_CALLBACK_FUNC &&
		m_billing.SetCallBackFunc() != BSIP_OK)
	{
		m_eBsipErr	= BSIP_ERR_SET_CALLBACK_FUNC;
		printf("Can't set callback func\n");
		return false;
	}

	if (m_eBsipErr < BSIP_ERR_GS_INITIALIZE)
		m_billing.IntSockEnv();

	if (m_eBsipErr <= BSIP_ERR_GS_INITIALIZE)
	{
		int ret = m_billing.m_pGSInitialize(&billing.m_funCallBack, "ini\\ConfigClient.ini", 0);
		if (BSIP_OK != ret)
		{
			m_eBsipErr	= BSIP_ERR_GS_INITIALIZE;
			m_err		= ret;
			printf("Can't initialize billing system, error code: %d\n", ret);
			return false;
		}
	}

	m_eBsipErr	= BSIP_ERR_SUCCESS;

	if (m_pApModule)
		m_ulInitializedClock	= m_pApModule->GetClockCount();

	/*
	printf("Waiting for connecting to BSIP\n");
	Sleep(5000);	// waiting 5s
	*/

	return true;
}


//bool AgsmBillingChina::Init()
//{
//}

int AgsmBillingChina::SendAccountAuthen(char* account, char* ip, AgsdBilling *billing)
{
	if (!IsConnectedBSIP())
		return (-1);

	GSBsipAccountAuthenReqDef AccountAuthenReq = {0, };

	strncpy(AccountAuthenReq.pt_id, account, MAX_PTID_LEN);
	strcpy(AccountAuthenReq.sd_id, "0");

	m_billing.m_pGetNewId(AccountAuthenReq.sess_id);
	m_billing.m_pGetNewId(AccountAuthenReq.unique_id);

	AccountAuthenReq.user_type = '1';			// always 1
	strcpy(AccountAuthenReq.ip , ip);
	
	//AccountAuthenReq.reserve_int = 0 ;
	//strcpy( AccountAuthenReq.reserve_char , "" );

	// save auth info
	g_billingBridge.Add(AccountAuthenReq.unique_id, billing);

	// log
	AccountAuthenLog(account, ip, AccountAuthenReq.unique_id);

	return m_billing.m_pSendAccountAuthenRequest( &AccountAuthenReq);
}

int AgsmBillingChina::SendAccountLock(char* account, char* ip, int itemID, int itemPrice, AgsdBilling* billing)
{
	if (!IsConnectedBSIP())
		return (-1);

	GSBsipAccountLockReqDef AccountLockReq = {0, };

	SYSTEMTIME st;
	GetLocalTime(&st);

	strncpy(AccountLockReq.pt_id, account, MAX_PTID_LEN);
	strcpy(AccountLockReq.sd_id, "0");
	
	m_billing.m_pGetNewId( AccountLockReq.sess_id );
	m_billing.m_pGetNewId( AccountLockReq.order_id );
	
	AccountLockReq.pay_type		= '3';	// 정확한 의미를
	AccountLockReq.app_type		= '2';	// 모르지만,
	AccountLockReq.status		= '2';	// 항상 이 값들을
	AccountLockReq.couple_type	= '1';	// 유지해야 한다.
	AccountLockReq.discount		= 100;	// 고정값이다.
	AccountLockReq.lock_period	= 0;	// 의미는 궁금해하지 말자.

	sprintf(AccountLockReq.log_time, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	strcpy(AccountLockReq.ip , ip);

	AccountLockReq.item_num = 1;
	
	AccountLockReq.item0_id = itemID;
	AccountLockReq.item0_num = 1;
	AccountLockReq.item0_price = itemPrice;

	// 한번에 item을 5개까지 구입할수 있도록 되어 있지만,
	// 우리겜에는 이런 기능이 없다.
	//
	//AccountLockReq.item1_id = 0;
	//AccountLockReq.item1_num = 0;
	//AccountLockReq.item1_price = 0;
	//AccountLockReq.item2_id = 0;
	//AccountLockReq.item2_num = 0;
	//AccountLockReq.item2_price = 0;
	//AccountLockReq.item3_id = 0;
	//AccountLockReq.item3_num = 0;
	//AccountLockReq.item3_price = 0;
	//AccountLockReq.item4_id = 0;
	//AccountLockReq.item4_num = 0;
	//AccountLockReq.item4_price = 0;

	// save auth info
	g_billingBridge.Add(AccountLockReq.order_id, billing);

	// log
	AccountLockLog(account, ip, itemID, itemPrice, AccountLockReq.order_id);
	
	return m_billing.m_pSendAccountLockRequest(&AccountLockReq);                                             
}

int AgsmBillingChina::SendAccountUnlock(char* account, char* ip, int itemID, int itemPrice, char* order_id)
{
	if (!IsConnectedBSIP())
		return (-1);

	GSBsipAccountUnlockReqDef AccountUnLockReq = {0, };

	m_billing.m_pGetNewId(AccountUnLockReq.sess_id);
	strcpy(AccountUnLockReq.order_id, order_id);

	strncpy(AccountUnLockReq.pt_id, account, MAX_PTID_LEN);
	strcpy(AccountUnLockReq.sd_id, "0");

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(AccountUnLockReq.log_time, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	AccountUnLockReq.confirm	 = '1';
	AccountUnLockReq.pay_type	 = '3';
	AccountUnLockReq.app_type	 = '2';
	AccountUnLockReq.status		 = '2';
	AccountUnLockReq.couple_type = '1';
	AccountUnLockReq.discount	 = 100;
	AccountUnLockReq.responsable = '1';

	strcpy(AccountUnLockReq.ip, ip);

	AccountUnLockReq.item_num = 1;

	AccountUnLockReq.item0_id	 = itemID;
	AccountUnLockReq.item0_num	 = 1;
	AccountUnLockReq.item0_price = itemPrice;
	
	//AccountUnLockReq.item1_id = 0;
	//AccountUnLockReq.item1_num = 0;
	//AccountUnLockReq.item1_price = 0;
	//AccountUnLockReq.item2_id = 0;
	//AccountUnLockReq.item2_num = 0;
	//AccountUnLockReq.item2_price = 0;
	//AccountUnLockReq.item3_id = 0;
	//AccountUnLockReq.item3_num = 0;
	//AccountUnLockReq.item3_price = 0;
	//AccountUnLockReq.item4_id = 0;
	//AccountUnLockReq.item4_num = 0;
	//AccountUnLockReq.item4_price = 0;

	return m_billing.m_pSendAccountUnlockRequest( &AccountUnLockReq);
}

void AgsmBillingChina::WriteErrorLog()
{
	CHAR	szBuffer[32];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	AuTimeStamp::GetCurrentTimeStampString(szBuffer, 32);

	switch (m_eBsipErr) {
		case BSIP_ERR_LOAD_LIBRARY:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't load BSIP dll\n", szBuffer);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
		case BSIP_ERR_GET_FUNC_ADDR:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't get func address\n", szBuffer);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
		case BSIP_ERR_SET_CALLBACK_FUNC:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't set callback func\n", szBuffer);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
		case BSIP_ERR_GS_INITIALIZE:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't initialize billing system, error code: %d\n", szBuffer, m_err);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
	}

	return;
}

void AgsmBillingChina::CheckConnection()
{
	if (IsConnectedBSIP())
		return;

	Init();

	return;
}

//BOOL AgsmBillingChina::SendGetCashMoney(AgpdCharacter *pAgpdCharacter)
//{
//	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
//	if (NULL == pAgsdCharacter)
//		return FALSE;
//
//	if (SendAccountAuthen(pAgsdCharacter->m_szAccountID, &pAgsdCharacter->m_strIPAddress[0], pAgpdCharacter) == 0)
//		return TRUE;
//	
//	return FALSE;	
//}
//
//BOOL AgsmBillingChina::SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR* /*pszDesc*/, INT64 llMoney, UINT64 /*ullListSeq*/)
//{
//	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
//	if (NULL == pAgsdCharacter)
//		return FALSE;
//
//	int ret = SendAccountLock(pAgsdCharacter->m_szAccountID,
//							  &pAgsdCharacter->m_strIPAddress[0],
//							  lProductID,
//							  (int)llMoney,
//							  pAgpdCharacter);
//	if (0 == ret)
//		return TRUE;
//
//	return FALSE;
//}

bool AgsmBillingChina::IsConnectedBSIP()
{
	UINT32	ulCurrentClockCount	= 0;
	if (m_pApModule)
		ulCurrentClockCount	= m_pApModule->GetClockCount();

	if (ulCurrentClockCount > 0)
	{
		if (m_eBsipErr == BSIP_ERR_SUCCESS &&
			m_ulInitializedClock + 5000 < ulCurrentClockCount)
			return true;
	}
	else
	{
		if (m_eBsipErr == BSIP_ERR_SUCCESS)
			return true;
	}

	return false;
}

void AgsmBillingChina::AccountAuthenLog(char* account, char* ip, char* orderid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountAuthen: %s, %s, %s\n", account, ip, orderid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountAuthenResLog(char* account, int balance)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountAutheRes: %s, %d\n", account, balance);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountLockLog(char* account, char* ip, int itemID, int itemPrice, char* orderid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountLock: %s, %s, %d, %d, %s\n", account, ip, itemID, itemPrice, orderid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountLockResLog(char* account, char* ip, int itemID, int itemPrice, char* orderid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountLockRes: %s, %s, %d, %d, %s\n", account, ip, itemID, itemPrice, orderid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountUnlockLog(char* account, char* ip, int itemID, int itemPrice, char* orderid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountUnlock: %s, %s, %d, %d, %s\n", account, ip, itemID, itemPrice, orderid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::SomethingWrong(char* log)
{
	char strCharBuff[512] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Error: %s\n", log);
	AuLogFile_s(m_logFileName, strCharBuff);
}
