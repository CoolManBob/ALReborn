#ifndef __AGSMBILLINGCHINA_H__
#define __AGSMBILLINGCHINA_H__

#include "AgsmCharacter.h"
#include "bsipmsg.h"

#include <map>

// **************************************************************************
class AgsdBilling;
class AgsmItemManager;
class AgsmCashMall;
class AgpmCashMall;
class AgpmItem;
class AgsmItem;
class AgsmBilling;

typedef enum BSIP_ERROR {
	BSIP_ERR_NONE					= 0,
	BSIP_ERR_LOAD_LIBRARY			= 500,
	BSIP_ERR_GET_FUNC_ADDR,
	BSIP_ERR_SET_CALLBACK_FUNC,
	BSIP_ERR_GS_INITIALIZE,
	BSIP_ERR_SUCCESS,
};

class CBillingBridge
{
private:
	class Lock
	{
	public:
		Lock(CRITICAL_SECTION& lock) : m_lock(lock) { EnterCriticalSection(&m_lock); }
		~Lock() { LeaveCriticalSection(&m_lock); }

	private:
		CRITICAL_SECTION& m_lock;
	};

public:
	CBillingBridge();
	~CBillingBridge();

	void SetCharManager(AgpmCharacter* pmChar, AgsmCharacter* smChar);
	void SetItemManager(AgpmItem* pmItem, AgsmItem* smItem, AgsmItemManager* itemManager);
	void SetCashMallManager(AgpmCashMall* pmMall, AgsmCashMall* smMall);
	void SetBilling(AgsmBilling* pBilling);

	void Add(char* uid, AgsdBilling* billing);
	void Remove(char* uid);

	AgsdBilling* Find(char* uid);
	
	AgsmCharacter* GetAgsmChar()	 { return m_pAgsmCharacter; }
	AgpmCharacter* GetAgpmChar()	 { return m_pAgpmCharacter; }
	AgsmCashMall*  GetAgsmCashMall() { return m_pAgsmCashMall; }
	AgpmCashMall*  GetAgpmCashMall() { return m_pAgpmCashMall; }
	AgsmBilling*   GetAgsmBilling()	 { return m_pAgsmBilling; }
	AgpmItem*	   GetAgpmItem()	 { return m_pAgpmItem; }
	AgsmItem*	   GetAgsmItem()	 { return m_pAgsmItem; }

	AgsmItemManager* GetAgsmItemManager() { return m_pAgsmItemManager; }

private:
	AgsmItemManager* m_pAgsmItemManager;

	AgsmBilling*	m_pAgsmBilling;
	AgsmCharacter*	m_pAgsmCharacter;
	AgpmCharacter*	m_pAgpmCharacter;

	AgsmCashMall*	m_pAgsmCashMall;
	AgpmCashMall*	m_pAgpmCashMall;
	
	AgpmItem*		m_pAgpmItem;
	AgsmItem*		m_pAgsmItem;

	typedef std::map<std::string, AgsdBilling*> StrBillingMap;
	StrBillingMap	m_map;

	CRITICAL_SECTION	m_cs;
};

extern CBillingBridge g_billingBridge;

// **************************************************************************
class CBillAPI;

class AgsmBillingChina
{
public:
	AgsmBillingChina();
	//AgsmBillingChina(CBillAPI& billAPI);
	~AgsmBillingChina();

	bool Init(ApModule *pModule = NULL);

	BSIP_ERROR	m_eBsipErr;
	int			m_err;
	UINT32		m_ulInitializedClock;

public:
	// 처음 로그인할때, 계정의 금액 정보를 가져온다.
	int SendAccountAuthen(char* account, char* ip, AgsdBilling *billing);
	// 아이템 구매, AccountUnlock과 쌍으로 불려야한다.
	int SendAccountLock(char* account, char* ip, int itemID, int itemPrice, AgsdBilling *billing);
	// 아이템 구매 완료
	int SendAccountUnlock(char* account, char* ip, int itemID, int itemPrice, char* order_id);

	void WriteErrorLog();
	void CheckConnection();

	// log
	void SomethingWrong(char* log);
	void AccountAuthenLog(char* account, char* ip, char* orderid);
	void AccountAuthenResLog(char* account, int balance);
	void AccountLockLog(char* account, char* ip, int itemID, int itemPrice, char* orderid);
	void AccountLockResLog(char* account, char* ip, int itemID, int itemPrice, char* orderid);
	void AccountUnlockLog(char* account, char* ip, int itemID, int itemPrice, char* orderid);

public:
	//BOOL SendGetCashMoney(AgpdCharacter *pAgpdCharacter);
	//BOOL SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR* /*pszDesc*/, INT64 llMoney, UINT64 /*ullListSeq*/);

private:
	CBillAPI&		m_billing;
	//AgsmCharacter	*m_pAgsmCharacter;

	// no assign/copy
	AgsmBillingChina(const AgsmBillingChina& rhs);
	const AgsmBillingChina& operator=(const AgsmBillingChina& rhs);

private:
	ApModule* m_pApModule;
	bool IsConnectedBSIP();

private:
	char	m_logFileName[MAX_PATH];
};

#endif