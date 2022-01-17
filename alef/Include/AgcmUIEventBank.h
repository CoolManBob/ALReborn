#ifndef	__AGCMUIEVENTBANK_H__
#define	__AGCMUIEVENTBANK_H__


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIEventBankD" )
#else
#pragma comment ( lib , "AgcmUIEventBank" )
#endif
#endif


#include "AgpmItem.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmEventBank.h"
#include "AgcmUIManager2.h"
#include "AgcmUIItem.h"


typedef enum _AgcmUIEventBankDisplayID {
	AGCMUIEVENTBANK_DISPLAY_ID_BANK_MONEY			= 0,
	AGCMUIEVENTBANK_DISPLAY_ID_BANK_MONEY_CONFIRM,
	AGCMUIEVENTBANK_DISPLAY_ID_BUY_BANK_SLOT_TITLE,
} AgcmUIEventBankDisplayID;

#define	AGCMUIEVENTBANK_CLOSE_UI_DISTANCE			150

#define	UI_MESSAGE_ID_BANK_INPUT_MONEY_CONFIRM		"Bank_InputMoneyConfirm"
#define	UI_MESSAGE_ID_BANK_OUTPUT_MONEY_CONFIRM		"Bank_OutputMoneyConfirm"

#define	UI_MESSAGE_ID_ITEM_BUY_BANK_SLOT_TITLE		"Item_BuyBankSlotTitle"
#define	UI_MESSAGE_ID_ITEM_BUY_BANK_SLOT_PRICE		"Item_BuyBankSlotPrice"

class AgcmUIEventBank : public AgcModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;

	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmItem			*m_pcsAgcmItem;
	AgcmEventBank		*m_pcsAgcmEventBank;
	AgcmUIManager2		*m_pcsAgcmUIManager2;

	AgcdUIUserData		*m_pcsUserDataBank;
	AgcdUIUserData		*m_pcsUserDataBankMoney;

	AgcdUIUserData		*m_pcsUserDataDummy;

	AgcdUIUserData		*m_pcsBankLayerIndex;

	AgpdGrid			*m_pcsBankGrid;
	INT64				m_llBankMoney;

	BOOL				m_bIsInputMoneyToBank;

	INT32				m_lBankLayerIndex;
	INT32				m_lDummyData;

	BOOL				AddEvent();
	BOOL				AddFunction();
	BOOL				AddDisplay();
	BOOL				AddUserData();

	// event ids
	INT32				m_lEventOpenBankUI;
	INT32				m_lEventCloseBankUI;

	INT32				m_lEventBankUpdate;

	INT32				m_lEventConfirmBuyBankSlot;

	INT32				m_lEventNotEnoughBuyCost;

	AuPOS				m_stBankOpenPos;

	BOOL				m_bIsBankUIOpen;

public:
	AgcmUIEventBank();
	virtual ~AgcmUIEventBank();

	BOOL				OnAddModule();

	static BOOL			CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateBankUI(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateBank(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateBankMoney(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBAddItemToBank(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveItemFromBank(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBGrantBankEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBBankMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBInputMoneyToBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBOutputMoneyFromBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetInputMoneyToBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBSetOutputMoneyFromBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBDisplayBankMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayConfirmMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL	CBBuyBankSlot(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCancelBankSlot(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBRequestBuyBankSlot(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSetBankPrice(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBBankDragDropItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBBankUpdateLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBDisplayBuyBankSlotTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL	CBUpdateBankSize(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBIsActiveBuyBankSlot(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL	CBIsActiveBankSlot1(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL	CBIsActiveBankSlot2(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL	CBIsActiveBankSlot3(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
};

#endif	//__AGCMUIEVENTBANK_H__