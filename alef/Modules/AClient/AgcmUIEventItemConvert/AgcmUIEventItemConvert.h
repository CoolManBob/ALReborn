#ifndef	__AGCMUIEVENTITEMCONVERT_H__
#define	__AGCMUIEVENTITEMCONVERT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIEventItemConvertD" )
#else
#pragma comment ( lib , "AgcmUIEventItemConvert" )
#endif
#endif

#include "AgcmCharacter.h"
#include "AgcmItemConvert.h"
#include "AgcmEventItemConvert.h"
#include "AgcmSound.h"

#include "AgcmUIManager2.h"

typedef enum	_eAgcmUIItemConvertType {
	AGCMUI_ITEMCONVERT_TYPE_NONE					= (-1),
	AGCMUI_ITEMCONVERT_TYPE_PHYSICAL,
	AGCMUI_ITEMCONVERT_TYPE_SOCKET,
	AGCMUI_ITEMCONVERT_TYPE_SPIRITSTONE,
	AGCMUI_ITEMCONVERT_TYPE_RUNE
} AgcmUIItemConvertType;

typedef enum	_AgcmUIItemConvert_Display_ID {
	AGCMUI_ITEMCONVERT_DISPLAY_ID_COST				= 0,
	AGCMUI_ITEMCONVERT_DISPLAY_ID_COST_PAY_RESULT,
//	AGCMUI_ITEMCONVERT_DISPLAY_ID_CONVERT_TITLE,
	AGCMUI_ITEMCONVERT_DISPLAY_ID_CONVERT_ITEM_NAME,
} AgcmUIItemConvert_Display_ID;


//#define	UI_MESSAGE_ID_CONV_TITLE					"Convert_Title"
//#define	UI_MESSAGE_ID_CONV_TITLE_PHYSICAL			"Convert_Title_Physical"
//#define	UI_MESSAGE_ID_CONV_TITLE_SOCKET				"Convert_Title_Socket"
//#define	UI_MESSAGE_ID_CONV_TITLE_RUNE				"Convert_Title_Rune"
//
//#define	UI_MESSAGE_ID_CONV_NPC_CONVERSATION			"Convert_NPCConversation"

//#define	UI_MESSAGE_ID_CONV_PHYSICAL					"Convert_Physical"
#define	UI_MESSAGE_ID_CONV_PHYSICAL_CONFIRM			"Convert_Physical_Confirm"
#define	UI_MESSAGE_ID_CONV_PHYSICAL_SUCCESS			"Convert_Physical_Success"
#define	UI_MESSAGE_ID_CONV_PHYSICAL_KEEPCURRENT		"Convert_Physical_KeepCurrent"
#define	UI_MESSAGE_ID_CONV_PHYSICAL_INITIALIZE_SAME	"Convert_Physical_Initialize_Same"
#define	UI_MESSAGE_ID_CONV_PHYSICAL_INITIALIZE		"Convert_Physical_Initialize"
#define	UI_MESSAGE_ID_CONV_PHYSICAL_DESTROY			"Convert_Physical_Destroy"

//#define	UI_MESSAGE_ID_CONV_RUNE						"Convert_Rune"
#define	UI_MESSAGE_ID_CONV_RUNE_CONFIRM				"Convert_Rune_Confirm"
#define	UI_MESSAGE_ID_CONV_RUNE_SUCCESS				"Convert_Rune_Success"
#define	UI_MESSAGE_ID_CONV_RUNE_KEEPCURRENT			"Convert_Rune_KeepCurrent"
#define	UI_MESSAGE_ID_CONV_RUNE_INITIALIZE_SAME		"Convert_Rune_Initialize_Same"
#define	UI_MESSAGE_ID_CONV_RUNE_INITIALIZE			"Convert_Rune_Initialize"
#define	UI_MESSAGE_ID_CONV_RUNE_DESTROY				"Convert_Rune_Destroy"

//#define	UI_MESSAGE_ID_CONV_SOCKET					"Convert_Socket"
#define	UI_MESSAGE_ID_CONV_SOCKET_CONFIRM			"Convert_Socket_Confirm"
#define	UI_MESSAGE_ID_CONV_SOCKET_SUCCESS			"Convert_Socket_Success"
#define	UI_MESSAGE_ID_CONV_SOCKET_KEEPCURRENT		"Convert_Socket_KeepCurrent"
#define	UI_MESSAGE_ID_CONV_SOCKET_INITIALIZE_SAME	"Convert_Socket_Initialize_Same"
#define	UI_MESSAGE_ID_CONV_SOCKET_INITIALIZE		"Convert_Socket_Initialize"
#define	UI_MESSAGE_ID_CONV_SOCKET_DESTROY			"Convert_Socket_Destroy"

#define UI_MESSAGE_ID_CONVERT_INIT_TITLE			"Convert_Init_Title"

#define UI_MESSAGE_ID_CONVERT_PHYSICAL_IMPROPER_ITEM	"Convert_Physical_Improper_Item"
#define UI_MESSAGE_ID_CONVERT_PHYSICAL_ALREADY_FULL		"Convert_Physical_Already_Full"
#define UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST	"Convert_Physical_Cost_Catalyst"

#define UI_MESSAGE_ID_CONVERT_RUNE_NONE					"Convert_Rune_None"
#define UI_MESSAGE_ID_CONVERT_RUNE_IMPROPER_ITEM		"Convert_Rune_Improper_Item"
#define UI_MESSAGE_ID_CONVERT_RUNE_ALREADY_FULL			"Convert_Rune_Already_Full"
#define UI_MESSAGE_ID_CONVERT_RUNE_LOW_ITEM_LEVEL		"Convert_Rune_Low_Item_Level"
#define UI_MESSAGE_ID_CONVERT_RUNE_ALREADY_ANTI_CONV	"Convert_Rune_Already_Anti_Conv"
#define UI_MESSAGE_ID_CONVERT_RUNE_INVALID_RUNE			"Convert_Rune_Invalid_Rune"
#define UI_MESSAGE_ID_CONVERT_RUNE_COST					"Convert_Rune_Cost"

#define UI_MESSAGE_ID_CONVERT_SOCKET_IMPROPER_ITEM		"Convert_Socket_Improper_Item"
#define UI_MESSAGE_ID_CONVERT_SOCKET_ALREADY_FULL		"Convert_Socket_Already_Full"
#define UI_MESSAGE_ID_CONVERT_SOCKET_NOTENOUGH_MONEY	"Convert_Socket_NotEnoughMoney"

#define	AGCMUIEVENTITEMCONVERT_CLOSE_UI_DISTANCE			150

class AgcmUIEventItemConvert : public AgcModule {
private:
	AgpmGrid				*m_pcsAgpmGrid;
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmItemConvert			*m_pcsAgpmItemConvert;
	ApmEventManager			*m_pcsApmEventManager;

	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmItemConvert			*m_pcsAgcmItemConvert;
	AgcmEventItemConvert	*m_pcsAgcmEventItemConvert;
	AgcmUIManager2			*m_pcsAgcmUIManager2;

	AgcmSound				*m_pcsAgcmSound;

	AgcdUIUserData			*m_pcsUserDataGrid;
	AgcdUIUserData			*m_pcsUserDataGridSocket;
	AgcdUIUserData			*m_pcsUserDataConvertCost;

	AgcdUIUserData			*m_pcsUserDataConvertType;

	AgcdUIUserData			*m_pcsUserDataConvertItem;

	AgcdUIUserData			*m_pcsUserDataConvertResult;

	INT32					m_lEventOpenConvertMainUI;
	INT32					m_lEventCloseConvertUI;

	INT32					m_lEventOpenConvertUI;

	INT32					m_lEventConvertConfirm;

	INT32					m_lEventSpiritStoneConvertSuccess;
	INT32					m_lEventSpiritStoneConvertFail;
	INT32					m_lEventSpiritStoneConvertImproperItem;
	INT32					m_lEventSpiritStoneConvertAlreadyFull;

	INT32					m_lEventPhysicalConvertAlreadyFull;
	INT32					m_lEventPhysicalConvertImproperItem;

	INT32					m_lEventSocketConvertImproperItem;
	INT32					m_lEventSocketConvertNotEnoughMoney;
	INT32					m_lEventSocketConvertAlreadyFull;

	INT32					m_lEventSocketConvertNotEnoughInvenMoney;
	INT32					m_lEventSocketConvertEnoughInvenMoney;

	INT32					m_lEventRuneConvertNotEnoughSocket;

	INT32					m_lEventErrAlreadyFull;
	INT32					m_lEventErrNotExistItem;
	INT32					m_lEventErrInvalidCatalyst;
	INT32					m_lEventErrInvalidRune;
	INT32					m_lEventErrNotEnoughSocket;
	INT32					m_lEventErrLowCharLevel;
	INT32					m_lEventErrLowItemLevel;
	INT32					m_lEventErrImproperPart;
	INT32					m_lEventErrNotEnoughMoney;
	INT32					m_lEventErrTryEquipItem;
	INT32					m_lEventErrAlreadyAntiConvert;

	INT32					m_lEventUpdateNPCConv;
	INT32					m_lEventUpdateConvertInfo;
	INT32					m_lEventUpdateConvertConfirm;

	AgpdGrid				m_stGrid;
	AgpdGrid				m_stGridSocket;
	INT64					m_llConvertCost;

	AcUIToolTip				m_csToolTip;

	AuPOS					m_stConvertOpenPos;

	AgcmUIItemConvertType	m_eConvertType;
	INT32					m_lConvertResult;

	AgpdItem				*m_pcsConvertItem;

	CHAR					m_szRuneName[64];

	INT32					m_lDummyData;

	AgcdUIControl			*m_pcsPhysicalEditControl;
	AgcdUIControl			*m_pcsRuneEditControl;
	AgcdUIControl			*m_pcsSocketEditControl;

	BOOL					AddEvent();
	BOOL					AddFunction();
	BOOL					AddDisplay();
	BOOL					AddUserData();

	BOOL					RefreshUserDataGrid();
	BOOL					RefreshUserDataGridSocket();
	BOOL					RefreshUserDataConvertCost();
	BOOL					RefreshUserDataConvertType();
	BOOL					RefreshUserDataConvertItem();
	BOOL					RefreshUserDataConvertResult();

	BOOL					RecalcConvertCost(AgpdItem *pcsItem);

	AuPOS					m_stEventPos;

	BOOL					m_bIsConvertUIOpen;

	UINT32					m_ulTimeToThrowEvent;
	INT32					m_lEventToThrow;

	BOOL					ThrowEventPhysicalConvertable(AgpdItemConvertResult eResult);
	BOOL					ThrowEventRuneConvertable(AgpdItemConvertRuneResult eResult);
	BOOL					ThrowEventSocketConvertable(AgpdItemConvertSocketResult eResult);

	BOOL					SetPhysicalResult(AgpdItemConvertResult eResult);
	BOOL					SetRuneResult(AgpdItemConvertRuneResult eResult);
	BOOL					SetSocketResult(AgpdItemConvertSocketResult eResult);

	INT32					m_lPrevNumPhysicalConvert;

	BOOL					m_bIsRequestConvert;
	UINT32					m_ulNextRequestConvertTime;

public:
	AgcmUIEventItemConvert();
	virtual ~AgcmUIEventItemConvert();

	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();
	BOOL					OnIdle(UINT32 ulClockCount);

	static BOOL				CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBReceiveGrant(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBResultPhysicalConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBResultSocketConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBResultSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBResultRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData);

	// ui function
//	static BOOL				CBOpenTooltipPhysical(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
//	static BOOL				CBOpenTooltipSocket(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
//	static BOOL				CBOpenTooltipRune(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
//	static BOOL				CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBAddConvertGridItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBAddConvertGridItemSocket(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBStartConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBConvertOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBConvertCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBSetNPCConv(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBGetPhysicalEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBGetRuneEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBGetSocketEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// display function
	static BOOL				CBDisplayConvertItemName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL				CBDisplayConvertCost(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL				CBDisplayConvertCostPayResult(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
//	static BOOL				CBDisplayConvertTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL				CBDisplayConvertPhysicalResult(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

//	static BOOL				CBUpdateNPCConv(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
//	static BOOL				CBUpdateConvertInfo(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBUpdateConvertConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// physical convert functions
	static BOOL				CBSetPhysicalConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	BOOL					InitPhysicalConvertUI();
	BOOL					CheckPhysicalConvert();
	BOOL					StartPhysicalConvert();
//	BOOL					UpdatePhysicalConvertInfo(AgcdUIControl *pcsEditControl);
	BOOL					UpdatePhysicalConvertConfirm(AgcdUIControl *pcsEditControl, AgpdItem *pcsConvertItem);

	// socket convert functions
	static BOOL				CBSetSocketConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	BOOL					InitSocketConvertUI();
	BOOL					CheckSocketConvert();
	BOOL					StartSocketConvert();
//	BOOL					UpdateSocketConvertInfo(AgcdUIControl *pcsEditControl);
	BOOL					UpdateSocketConvertConfirm(AgcdUIControl *pcsEditControl, AgpdItem *pcsConvertItem);

	// rune convert functions
	static BOOL				CBSetRuneConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	BOOL					InitRuneConvertUI();
	BOOL					CheckRuneConvert();
	BOOL					StartRuneConvert();
//	BOOL					UpdateRuneConvertInfo(AgcdUIControl *pcsEditControl);
	BOOL					UpdateRuneConvertConfirm(AgcdUIControl *pcsEditControl, AgpdItem *pcsConvertItem);

	BOOL					PlayEffectSuccess(BOOL bThrowEvent = TRUE);
	BOOL					PlayEffectFailed(BOOL bThrowEvent = TRUE);

	static BOOL				CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGCMUIEVENTITEMCONVERT_H__