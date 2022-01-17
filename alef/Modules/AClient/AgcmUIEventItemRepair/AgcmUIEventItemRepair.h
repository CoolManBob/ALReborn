//	AgcmUIEventItemRepair.h
/////////////////////////////////////////////////////////////

#ifndef	__AGCMUIEVENTITEMREPAIR_H__
#define	__AGCMUIEVENTITEMREPAIR_H__

#include "AgcmCharacter.h"
#include "ApmEventManager.h"

#include "AgpmEventItemRepair.h"
#include "AgcmEventItemRepair.h"
#include "AgpmFactors.h"

#include "AgcmEventNPCTrade.h"

#include "AgcmUIManager2.h"
#include "AgcmUIItem.h"
#include "AgcmChatting2.h"
#include "AgcmSkill.h"

const INT32 AGCMUIEVENT_ITEMREPAIR_CLOSE_UI_DISTANCE = 100;

class AgcmUIEventItemRepair : public AgcModule 
{
private:
	// Modules
	AgpmCharacter*			m_pcsAgpmCharacter;
	AgcmCharacter*			m_pcsAgcmCharacter;
	AgcmUIManager2*			m_pcsAgcmUIManager2;

	ApmEventManager*		m_pcsApmEventManager;

	AgpmEventItemRepair*	m_pcsAgpmEventItemRepair;
	AgcmEventItemRepair*	m_pcsAgcmEventItemRepair;

	AgpmGrid*				m_pcsAgpmGrid;
	AgpmItem*				m_pcsAgpmItem;

	AgcmUIItem*				m_pcsAgcmUIItem;
	AgcmChatting2*			m_pcsAgcmChatting;
	AgpmFactors*			m_pcsAgpmFactors;
	AgcmSkill*				m_pcsAgcmSkill;

private:
	// properties
	INT32					m_lEventUIOpen;
	INT32					m_lEventUIClose;
	INT32					m_lEventInventoryUIOpen;
	INT32					m_lEventInventoryUIClose;

	INT32					m_lMessageAlreadyMax;
	INT32					m_lMessageInsufficientMoney;
	INT32					m_lMessageDontRepair;

	BOOL					m_bHold;
	BOOL					m_bIsUIOpen;

	INT64					m_RepairCost;
	INT64					m_InventoryMoney;

	AgcdUIUserData			*m_pcsUDMainGrid;
	AgcdUIUserData			*m_pcsUDInventoryMoney;
	AgcdUIUserData			*m_pcsUDRepairCost;

	AgpdGrid				m_RepairItemGrid;
	ApSafeArray<AgpdItem *, ITEM_REPAIR_GRID_MAX>	m_pSources;

	AuPOS					m_stOpenPos;

public:
	AgcmUIEventItemRepair();
	virtual ~AgcmUIEventItemRepair();
	
	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

	BOOL					AddFunctions();
	BOOL					AddEvents();
	BOOL					AddUserDatas();
	BOOL					AddDisplay();

	// Grid function
	BOOL					AddSystemMessage(CHAR* pszMsg);
	BOOL					ClearGridMainUI();
	BOOL					RefreshGrid();
	BOOL					RefreshInventoryMoney();
	BOOL					RefreshRepairCost();
	BOOL					OnMoveItem(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
	BOOL					AddItemRepairItemGrid(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
	BOOL					RemoveItemRepairItemGrid(AgpdItem *pAgpdItem);
	BOOL					FindItemRepairGrid(AgpdItem *pAgpdItem);

	void					ShowMessage(EnumAgpmEventItemRepairResultCode eResult);

	static BOOL				CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBItemRepairAck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBRideFailMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBRemoveInventoryGrid(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateMoney(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBDisplayInventoryMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL				CBDisplayRepairCost(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL				CBItemRepairUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBItemRepairReq(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//
	static BOOL				CBItemRepairRequestItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBItemRepairRequestAllItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBItemRepairRequestEquipItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBItemRepairRequestInvenItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBLClickInventory(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBResetMouseCursor(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBItemRepairResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBItemNeedRepair(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBAriseNPCTradeEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	// event return callback functions
	static BOOL				CBReturnRequestItem(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL				CBReturnRequestAllItem(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL				CBReturnRequestInvenItem(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL				CBReturnRequestEquipItem(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

//	BOOL					RefreshUserDataRepairCost();
};

#endif	//__AGCMUIEVENTITEMREPAIR_H__