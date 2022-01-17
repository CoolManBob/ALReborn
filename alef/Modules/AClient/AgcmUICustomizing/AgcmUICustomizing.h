#if !defined(__AGCMUICUSTOMIZING_H__)
#define	__AGCMUICUSTOMIZING_H__

#include "AgpmCharacter.h"

#include "AgcModule.h"
#include "AgcmCharacter.h"
#include "AgcmCustomizeRender.h"
#include "AgcmUIManager2.h"
#include "AgcmEventCharCustomize.h"
#include "AgcmUIItem.h"
#include "AgcmChatting2.h"
#include "AgcmUIChatting2.h"
#include "AgcmUIMain.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUICustomizingD" )
#else
#pragma comment ( lib , "AgcmUICustomizing" )
#endif
#endif

#define	AGCMUICUSTOMIZE_MAX_FACE				50
#define	AGCMUICUSTOMIZE_MAX_HAIR				100

#define	AGCMUIMESSAGE_CUSTOMIZE_USELEVEL		"Customize_UseLevel"
#define	AGCMUIMESSAGE_CUSTOMIZE_PRICEMONEY		"Customize_PriceMoney"
#define	AGCMUIMESSAGE_CUSTOMIZE_PRICESKULL		"Customize_PriceSkull"
#define	AGCMUIMESSAGE_CUSTOMIZE_HAIRSTYLE		"Customize_HairStyle"
#define	AGCMUIMESSAGE_CUSTOMIZE_FACESTYLE		"Customize_FaceStyle"
#define	AGCMUIMESSAGE_CUSTOMIZE_TOTALPRICEMONEY	"Customize_TotalPriceMoney"
#define	AGCMUIMESSAGE_CUSTOMIZE_TOTALPRICESKULL	"Customize_TotalPriceSkull"

#define	AGCMUIMESSAGE_CUSTOMIZE_BUY_PRICE_FMT	"Customize_BuyPriceFormat"
#define	AGCMUIMESSAGE_CUSTOMIZE_FACECHANGE_FMT	"Customize_FaceChangeFormat"
#define	AGCMUIMESSAGE_CUSTOMIZE_HAIRCHANGE_FMT	"Customize_HairChangeFormat"

class AgcmUICustomizing : public AgcModule {
private:
	AgpmGrid				*m_pcsAgpmGrid;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmEventCharCustomize	*m_pcsAgpmEventCharCustomize;

	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmUIManager2			*m_pcsAgcmUIManager2;
	AgcmCustomizeRender		*m_pcsAgcmCustomizeRender;
	AgcmEventCharCustomize	*m_pcsAgcmEventCharCustomize;
	AgcmChatting2			*m_pcsAgcmChatting2;
	AgcmUIChatting2			*m_pcsAgcmUIChatting2;
	AgcmUIMain				*m_pcsAgcmUIMain;

private:
	INT32	m_lEventUIOpen;
	INT32	m_lEventUIClose;

	INT32	m_lEventBuyConfirm;

	INT32	m_lEventNeedDismountRide;
	INT32	m_lEventNeedRestoreTransform;

	AcUIEdit	*m_pcsEditFace;
	AcUIEdit	*m_pcsEditHair;
	AcUIEdit	*m_pcsEditSummary;

	INT32	m_alFace[CHARCUSTOMIZE_MAX_LIST];
	INT32	m_alHair[CHARCUSTOMIZE_MAX_LIST];

	AgcdUIUserData	*m_pcsUserDataFaceList;
	AgcdUIUserData	*m_pcsUserDataHairList;

	AcUIBase	m_csCustomizePreview;

	CharCustomizeList	m_acsCustomizeFaceList[CHARCUSTOMIZE_MAX_LIST];
	CharCustomizeList	m_acsCustomizeHairList[CHARCUSTOMIZE_MAX_LIST];

	INT32	m_lNumFaceList;
	INT32	m_lNumHairList;

	BOOL	m_bIsActiveBuy;

	AgcdUIUserData	*m_pcsUserDataIsActiveBuy;

	INT32	m_lTotalPriceMoney;
	INT32	m_lTotalPriceSkull;

	AgcdUIUserData	*m_pcsUserDataPriceMoney;

	INT32	m_lAddTextureID;

	BOOL	m_bIsOpenedUI;

	AuPOS	m_stOpenedPosition;

	ApdEvent	*m_pcsEvent;

	INT32	m_lPrevFaceIndex;
	INT32	m_lPrevHairIndex;

	
private:
	BOOL	AddEvent();
	BOOL	AddFunction();
	BOOL	AddUserData();
	BOOL	AddDisplay();

	BOOL	InitializeList();

	BOOL	SetFaceDescription();
	BOOL	SetHairDescription();
	BOOL	SetSummaryDescription();

	BOOL	IsEnoughCost();

	INT64	PriceWithTax(INT64 originPrice);
	INT64	GetInvenMoney();

public:
	AgcmUICustomizing();
	virtual ~AgcmUICustomizing();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSelfUpdateCustomize(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);

	// function
	static BOOL	CBGetEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBBuy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBConfirmOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBSelectFace(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSelectHair(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBRotateLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBRotateRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBRotateEnd(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBZoomIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBZoomOut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBZoomEnd(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBInitialize(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBCloseUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// display
	static BOOL	CBDisplayFaceName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL	CBDisplayHairName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL	CBDisplayBuyPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
};

#endif	//__AGCMUICUSTOMIZING_H__