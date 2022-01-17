#pragma once

#include "AgpmCashMall.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmCharManager.h"
#include "AgsmBilling.h"

class AgsmSystemMessage;
class CCallBackFunc;

class AgsmCashMall : public AgsModule {
	friend CCallBackFunc;

private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCashMall	*m_pcsAgpmCashMall;
	AgpmBillInfo	*m_pcsAgpmBillInfo;

	AgsmCharacter	*m_pcsAgsmCharacter;
	AgsmCharManager	*m_pcsAgsmCharManager;
	AgsmItem		*m_pcsAgsmItem;
	AgsmItemManager	*m_pcsAgsmItemManager;
	AgsmBilling		*m_pcsAgsmBilling;

	AgsmSystemMessage *m_pcsAgsmSystemMessage;
	
public:
	AgsmCashMall();
	virtual ~AgsmCashMall();

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();

	BOOL	SendBuyResult(AgpdCharacter *pcsCharacter, INT32 lResult);

	static BOOL		CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRequestMallProductList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRequestBuyItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRefreshCash(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCheckListVersion(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBResultBuyItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBResultCashMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBDisableForaWhile(PVOID pData, PVOID pClass, PVOID pCustData);
	//JK_À¥Á¨ºô¸µ
	static BOOL		CBRequestBuyItemWebzen(PVOID pData, PVOID pClass, PVOID pCustData);
	//JK_À¥Á¨ºô¸µ
	static BOOL		CBRequestUseStorage(PVOID pData, PVOID pClass, PVOID pCustData);
	//JK_À¥Á¨ºô¸µ
	static BOOL		CBRequestInquireStorageList(PVOID pData, PVOID pClass, PVOID pCustData);




private:
	BOOL	CheckBuyRequirement(AgpdCharacter *pcsCharacter, INT32 lProductID, UINT8 ucMallListVersion);
	BOOL	ProcessBuyItem(AgpdCharacter *pcsCharacter, INT32 lProductID, UINT8 ucMallListVersion);
	BOOL	ProcessBuyItemResult(AgpdCharacter *pcsCharacter, AgsdBillingItem *pcsBilling);
	BOOL	SendMallProductList(AgpdCharacter *pcsCharacter, INT32 lTab);
	//JK_À¥Á¨ºô¸µ
	BOOL	ProcessBuyItemWebzen(AgpdCharacter *pcsCharacter, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq);
	//JK_À¥Á¨ºô¸µ
	BOOL	ProcessUseStorage(AgpdCharacter *pcsCharacter, INT32 StorageSeq, INT32 StorageItemSeq);
	//JK_À¥Á¨ºô¸µ
	BOOL	ProcessInquireStorageList(AgpdCharacter *pcsCharacter, INT32 NowPage);



};