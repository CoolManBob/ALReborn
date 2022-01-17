#pragma once

#include "AgcmCashMall.h"
#include "AgcmUIManager2.h"
#include "AgcmUIMain.h"
#include "CWebzenShop.h"

#define	AGCMUICASHMALL_MAX_DISPLAY_COUNT		6
#define	AGCMUICASHMALL_MAX_PAGE_COUNT			5
#define	AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT	9

#define	AGCMUIMESSAGE_CASHMALL_KEEP_CASH		"KeepCash"			// 보유로니
#define	AGCMUIMESSAGE_CASHMALL_UNIT				"Unit"				// 개
#define	AGCMUIMESSAGE_CASHMALL_USE_CASH			"UseCash"			// 사용할 로니
#define	AGCMUIMESSAGE_CASHMALL_BUY_RESULT		"BuyResult"			// 결제 후 잔액
#define	AGCMUIMESSAGE_CASHMALL_CHARGE_URL		"CashChargeURL"
#define	AGCMUIMESSAGE_CASHMALL_NOT_COMMIT_ON_DEAD	"NotCommitOnDead"

class AgcmUICashMall : public AgcModule
{
private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmGrid		*m_pcsAgpmGrid;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCashMall	*m_pcsAgpmCashMall;

	AgcmCharacter	*m_pcsAgcmCharacter;
	AgcmCashMall	*m_pcsAgcmCashMall;
	AgcmUIManager2	*m_pcsAgcmUIManager2;
	AgcmUIMain		*m_pcsAgcmUIMain;

	AgpdGrid		m_acsItemGrid[AGCMUICASHMALL_MAX_DISPLAY_COUNT];

	INT32			m_lCurrentTab;
	INT32			m_lStartPage;
	INT32			m_lCurrentPage;
	INT32			m_lCash;
	INT32			m_lCurrentPageItemCount;
	INT32			m_lMaxPage;
	INT32			m_lUserDataMaxPage;
	
	AgcdUIUserData	*m_pcsUserDataCurrentTab;
	AgcdUIUserData	*m_pcsUserDataCurrentPage;
	AgcdUIUserData	*m_pcsUserDataItemGrid;
	AgcdUIUserData	*m_pcsUserDataCash;
	AgcdUIUserData	*m_pcsUserDataBuyProduct;
	AgcdUIUserData	*m_pcsUserDataMaxPage;

	AcUIEdit		*m_pcsEditCash;
	AcUIList		*m_pcsListControl;

	INT32			m_lEventGetEditControl;
	INT32			m_lEventGetListControl;
	INT32			m_lEventOpenCashMallUI;
	INT32			m_lEventCloseCashMallUI;

	INT32			m_lEventNormalProduct;
	INT32			m_lEventNewProduct;
	INT32			m_lEventHotProduct;
	INT32			m_lEventEventProduct;
	INT32			m_lEventSaleProduct;

	INT32			m_lEventSPCProduct;
	INT32			m_lEventTPackProduct;

	INT32			m_lEventSelectPageSlot[AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT];

	INT32			m_lEventBuyProductConfirm;
	INT32			m_lEventNotEnoughCash;

	INT32			m_lEventBuyResultSuccess;
	INT32			m_lEventBuyResultNotEnoughCash;
	INT32			m_lEventBuyResultInventoryFull;
	INT32			m_lEventBuyResultNeedNewItemList;
	INT32			m_lEventBuyResultPCBangOnly;
	INT32			m_lEventBuyResultLimitedCount;
	INT32			m_lEventBuyResultNotTPCBang;
	INT32			m_lEventBuyResultNotSPCBang;

	INT32			m_lEventOpenRefreshCashUI;

	BOOL			m_bReceiveMallInfo;

	BOOL			m_bIsOpenedUI;

	AgpdItem		*m_pcsBuyItem;

public:
	AgcmUICashMall();
	virtual ~AgcmUICashMall();

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();

private:
	BOOL	AddFunction();
	BOOL	AddUserData();
	BOOL	AddDisplay();
	BOOL	AddEvent();
	BOOL	AddBoolean();

	BOOL	SetItemGrid(INT32 lTab, INT32 lStartPage, INT32 lSelectPage);

	BOOL	RefreshCashEdit();
	BOOL	SetSelectPageSlot(INT32 lSelectPage);

public:
	static BOOL	CBUpdateMallList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateCash(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResponseBuyResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);

public:
	static BOOL	CBGetEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetListControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCashCharge(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCheckCash(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBBuyProductConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBBuyProduct(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBOpenCashMallUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSelectTab(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBPageLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBPageRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGoPage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBIsShowTabButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsShowChargeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActivePage(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActiveBuy(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDisplayProductName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayProductDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayProductPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayTabName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayMaxPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailValuePrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailValueUseCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailValueRemainCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	
	// event return callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL CBReturnRefresh(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

#ifdef _AREA_KOREA_
public :
	CWebzenShopClient								m_WebzenShop;

public :
	CWebzenShopClient*		GetWebzenShop			( void ) { return &m_WebzenShop; }
#endif
};