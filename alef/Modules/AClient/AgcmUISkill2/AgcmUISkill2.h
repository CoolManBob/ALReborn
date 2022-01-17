#ifndef	__AGCMUISKILL2_H__
#define	__AGCMUISKILL2_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUISkill2D" )
#else
#pragma comment ( lib , "AgcmUISkill2" )
#endif
#endif

#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmSkill.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmUIManager2.h"
#include "AgcmUICharacter.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgcmEventEffect.h"
#include "AgpmBillInfo.h"
#include "AgcmUICashInven.h"
#include "AgcmUISkillDragonScion.h"
#include "AgcmHeroicSkill.h"

#define	UI_MESSAGE_ID_SKILL_MAIN_DIALOG_MESSAGE		"SkillMainDialogMsg"
#define	UI_MESSAGE_ID_SKILL_LEVEL					"SkillLevel"
#define	UI_MESSAGE_ID_SKILL_BUY						"SkillBuy"
#define	UI_MESSAGE_ID_SKILL_UPGRADE					"SkillUpgrade"
#define	UI_MESSAGE_ID_SKILL_FAIL_INITIALIZE			"Skill_FailInitialize"
#define	UI_MESSAGE_ID_SKILL_CONFIRM_INITIALIZE		"Skill_ConfirmInitialize"
#define	UI_MESSAGE_ID_TIME_UNIT_DAY					"Time_Unit_Day"					//	2005.10.04. steeple
#define	UI_MESSAGE_ID_TIME_UNIT_NIGHT				"Time_Unit_Night"				//	2005.10.04. steeple
#define UI_MESSAGE_ID_ARROW_NOT_COST_KATARIYA		"Item_Arrow_Not_Cost_Katariya"	//	2005.10.05. steeple

#define UI_MESSAGE_TOOLTIP_TIME						"Tooltip_Time"
#define UI_MESSAGE_TOOLTIP_COMPLETETIME				"Tooltip_CompleteTime"
#define UI_MESSAGE_TOOLTIP_DURATIONTIME				"Tooltip_DurationTime"

#define	AGCMUISKILL2_MAX_BLINK_TIME					5000			// msec
#define	AGCMUISKILL2_PROCESS_IDLE_INTERVAL			1000			// msec

#define	AGCMUISKILL2_TOOLTIP_OFFSET					140

#define	AGCMUISKILL2_CLOSE_UI_DISTANCE				150

enum	eSkillTooltipColor
{
	AGCMUISKILL2_TOOLTIP_COLOR_NORMAL		,
	AGCMUISKILL2_TOOLTIP_COLOR_NEXT_LEVEL	,
	AGCMUISKILL2_TOOLTIP_COLOR_DISABLE		,
	AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY	,

	AGCMUISKILL2_TOOLTIP_COLOR_COUNT		,
};

typedef enum	_AgcmUISkill2_Display_ID {
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT					= 0,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_NAME,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL_2,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_PRICE,
	AGCMUI_SKILL2_DISPLAY_ID_INVEN_MONEY,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL_EXP,			// ## Product
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_EXP_DEGREE,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_EXP_DEGREE_MAX,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_TRAINER_CLASS,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_MAIN_DIALOG,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_BUY_BUTTON,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_SELECTED_NAME,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_0,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_1,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_2,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_3,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_4,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_5,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_6,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_7,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_8,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_9,
	AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_10,
} AgcmUISkill2_Display_ID;

typedef enum	_AgcmUISkill2_TooltipType {
	AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL						= 0,
	AGCMUI_SKILL2_TOOLTIP_TYPE_BUY,
	AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE,
} AgcmUISkill2_TooltipType;

class AgcmUISkill2 : public AgcModule {
private:
	AgpmGrid				*m_pcsAgpmGrid;
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmItem				*m_pcsAgcmItem;
	AgcmSkill				*m_pcsAgcmSkill;
	AgcmEventSkillMaster	*m_pcsAgcmEventSkillMaster;
	AgcmUIManager2			*m_pcsAgcmUIManager2;
	AgcmUICharacter			*m_pcsAgcmUICharacter;
	AgcmUIMain				*m_pcsAgcmUIMain;
	AgcmUIItem				*m_pcsAgcmUIItem;
	AgcmUIControl			*m_pcsAgcmUIControl;
	AgcmEventEffect			*m_pcsAgcmEventEffect;
	AgpmBillInfo			*m_pcsAgpmBillInfo;
	AgcmUICashInven			*m_pcsAgcmUICashInven;

	// user data
	///////////////////////////////////////////////////////////////
	//AgcdUIUserData			*m_pcsUserDataSkillMastery;
	AgcdUIUserData			*m_pcsUserDataSkillGrid;
	AgcdUIUserData			*m_pcsUserDataSkillBuyGrid;
	AgcdUIUserData			*m_pcsUserDataSkillUpgradeGrid;

	AgcdUIUserData			*m_pcsUserDataSkillPoint;

	AgcdUIUserData			*m_pcsUserDataInvenMoney;

	AgcdUIUserData			*m_pcsUserDataActiveBuyButton;
	AgcdUIUserData			*m_pcsUserDataActiveUpgradeButton;

	AgcdUIUserData			*m_pcsUserDataActiveSkillTab;
	AgcdUIUserData			*m_pcsUserDataActiveBuySkillTab;
	AgcdUIUserData			*m_pcsUserDataActiveUpgradeSkillTab;

	AgcdUIUserData			*m_pcsUserDataMainDialogMsg;

	AgcdUIUserData			*m_pcsUserDataIsActiveInitialize;

	AgcdUIUserData			*m_pcsUserDataSkillHighLevel[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	AgcdUIUserData			*m_pcsUserDataSkillHighLevelPoint[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];

	// event index
	///////////////////////////////////////////////////////////////
	INT32					m_lEventOpenSkillNPCDialogUI;
	INT32					m_lEventCloseSkillNPCDialogUI;

	INT32					m_lEventOpenSkillUI;
	INT32					m_lEventCloseSkillUI;
	INT32					m_lEventOpenSkillBuyUI;
	INT32					m_lEventCloseSkillBuyUI;

	INT32					m_lEventOpenSkillHighLevelUI;
	INT32					m_lEventCloseSkillHighLevelUI;
	INT32					m_lEventOpenSkillHighLevelUpgradeUI;
	INT32					m_lEventCloseSkillHighLevelUpgradeUI;

	INT32					m_lEventOpenSkillHeroicUI;
	INT32					m_lEventCloseSkillHeroicUI;
	INT32					m_lEventOpenSkillHeroicUpgradeUI;
	INT32					m_lEventCloseSkillHeroicUpgradeUI;

	INT32					m_lEventCastResultFailed;
	INT32					m_lEventCastResultInvalidTarget;
	INT32					m_lEventCastResultNeedRequirement;
	INT32					m_lEventCastResultNotEnoughHP;
	INT32					m_lEventCastResultNotEnoughSP;
	INT32					m_lEventCastResultNotEnoughMP;
	INT32					m_lEventCastResultNotEnoughArrow;
	INT32					m_lEventCastResultAlreadyUse;
	INT32					m_lEventCastResultNotReadyCast;
	INT32					m_lEventCastResultMiss;
	INT32					m_lEventCastResultInvalidTargetToGather;
	INT32					m_lEventCastResultNotEnoughProduct;

	INT32					m_lEventSetMasteryTab5;				// Product

	INT32					m_lEventSelectedItem;
	INT32					m_lEventUnSelectedItem;

	INT32					m_lEventActiveItem;
	INT32					m_lEventDisableItem;

	INT32					m_lEventBuySelectedItem;
	INT32					m_lEventBuyUnSelectedItem;

	INT32					m_lEventBuyActiveItem;
	INT32					m_lEventBuyDisableItem;

	INT32					m_lEventUpgradeSelectedItem;
	INT32					m_lEventUpgradeUnSelectedItem;

	INT32					m_lEventUpgradeActiveItem;
	INT32					m_lEventUpgradeDisableItem;

	INT32					m_lEventHighLevelSelectedItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	INT32					m_lEventHighLevelUnSelectedItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	INT32					m_lEventUpgradeHighLevelSelectedItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	INT32					m_lEventUpgradeHighLevelUnSelectedItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	
	INT32					m_lEventHighLevelActiveItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	INT32					m_lEventHighLevelDisableItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	INT32					m_lEventUpgradeHighLevelActiveItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];
	INT32					m_lEventUpgradeHighLevelDisableItem[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];

	INT32					m_lEventSkillBuySuccess;
	INT32					m_lEventSkillLearnSuccess;
	INT32					m_lEventSkillUpgradeSuccess;

	INT32					m_lEventNotEnoughMoney;
	INT32					m_lEventInvalidClass;
	INT32					m_lEventLowLevel;
	INT32					m_lEventNotEnoughSkillPoint;
	INT32					m_lEventNotSelectBuySkill;
	INT32					m_lEventNotSelectUpgradeSkill;
	INT32					m_lEventAlreadyLearnSkill;

	INT32					m_lEventNotEnoughHeroicPoint;
	INT32					m_lEventNotEnoughCharismaPoint;

	INT32					m_lEventConfirmBuySkill;
	INT32					m_lEventConfirmLearnSkill;
	INT32					m_lEventConfirmUpgradeSkill;

	INT32					m_lEventConfirmSkillInitialize;
	INT32					m_lEventFailSkillInitialize;

	INT32					m_lEventSkillInitializeSuccess;
	INT32					m_lEventSkillInitializeFailed;
	INT32					m_IEventSkillInitializeMoneyFull;

	INT32					m_lEventSkillRollbackConfirm;

	AgpdGrid				m_stSkillListGrid[AGPMEVENT_SKILL_MAX_MASTERY_SKILL];
	AgpdGrid				m_stSkillBuyListGrid[AGPMEVENT_SKILL_MAX_MASTERY_SKILL];
	AgpdGrid				m_stSkillUpgradeListGrid[AGPMEVENT_SKILL_MAX_MASTERY_SKILL];

	AgpdGrid				m_stSkillHighLevelGrid[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];

	INT32					m_lSkillPoint;
	INT32					m_lSkillPointHighLevel[AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL];

	INT32					m_lSkillMasteryList[AGPMEVENT_SKILL_MAX_MASTERY][AGPMEVENT_SKILL_MAX_MASTERY_SKILL];

	AgpdCharacter			*m_pcsSelfCharacter;

	ApdEvent				*m_pcsEvent;

	AuRaceType				m_eRaceType;
	AuCharClassType			m_eClassType;

	INT64					m_llInvenMoney;

	INT32					m_lSelectedItemIndex;
	INT32					m_lSelectedBuyItemIndex;
	INT32					m_lSelectedUpgradeItemIndex;

	INT32					m_lSelectedHighLevelSkillTID;
	INT32					m_lSelectedUpgradeHighLevelSkillTID;

	BOOL					m_bIsOpenSkillHighLevelUI;
	BOOL					m_bIsOpenSkillUpgradeHighLevelUI;

	BOOL					m_bIsOpenSkillHeroicUI;
	BOOL					m_bIsOpenSkillUpgradeHeroicUI;

	AgpdItem				*m_pcsUseSkillBook;

	INT32					m_lDummyData;
	BOOL					m_bIsActiveInitialize;

	BOOL					m_bIsOpenSkillEventUI;
	AuPOS					m_stOpenSkillUIPos;

	BOOL					m_bIsOpenSkillUI;
	BOOL					m_bIsOpenSkillProductUI;

	INT32					m_lLastSetMasteryIndex;
	INT32					m_lTooltipLineIndex;

	BOOL					m_bIsDestroy;

	BOOL			AddFunction();
	BOOL			AddEvent();
	BOOL			AddUserData();
	BOOL			AddDisplay();
	BOOL			AddBoolean();

	BOOL			SetMastery(INT32 lMasteryIndex);
	BOOL			SetBuyMastery(INT32 lMasteryIndex);
	BOOL			SetUpgradeMastery(INT32 lMasteryIndex);
	BOOL			SetMasteryArchlord(INT32 lMasteryIndex);
	BOOL			SetMasteryHighLevel(INT32 lMasteryIndex);
	BOOL			SetMasteryHeroic(INT32 lMasteryIndex);

	BOOL			SetUpgradeMasteryHighLevel();
	BOOL			SetUpgradeMasteryHeroic();

	BOOL			AddHighLevelSkill(AgpdSkill* pcsSkill);
	BOOL			RemoveHighLevelSkill(AgpdSkill* pcsSkill);

	BOOL			AddArchlordSkill(AgpdSkill* pcsSkill);
	BOOL			RemoveArchlordSkill(AgpdSkill* pcsSkill);

	BOOL			AddHeroicSkill(AgpdSkill* pcsSkill);
	BOOL			RemoveHeroicSkill(AgpdSkill* pcsSkill);

	// tooltip member
	//////////////////////////////////////////////////////////////////////
	AcUIToolTip				m_csSkillToolTip;
	AcUIToolTip				m_csMasteryToolTip;
	AcUIToolTip				m_csAddSPToolTip;

public:
	BOOL					OpenSkillToolTip(AgcdUIControl *pcsSourceControl, AgcmUISkill2_TooltipType eTooltipType);
	BOOL					CloseSkillToolTip( VOID );
	BOOL					OnShowMsgBox( CHAR* pMsgString );

	ApdEvent*				GetApdEvent( VOID )				{	return m_pcsEvent;	}
	INT32					GetSelectCharacterTID ( VOID )	{	return m_pcsAgpmEventSkillMaster->GetCharTID(m_eRaceType, m_eClassType);	}

	AgcmHeroicSkill			m_HeroicSkill;
private:
	BOOL					SetSkillToolTipInfo(INT32 lSkillID, AgcmUISkill2_TooltipType eTooltipType);
	BOOL					SetSkillTemplateToolTipInfo(INT32 lSkillTID, AgcmUISkill2_TooltipType eTooltipType);

	BOOL					SetSkillToolTipName(AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive);
	BOOL					SetSkillToolTipAttribute(AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive);
	BOOL					SetSkillToolTipDescription(AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive);
	BOOL					SetSkillToolTipCurrentLevel(AgpdSkill *pcsSkill, AgcmUISkill2_TooltipType eTooltipType);
	BOOL					SetSkillToolTipNextLevel(AgpdSkill *pcsSkill, AgpdSkillTemplate *pcsSkillTemplate, AgcmUISkill2_TooltipType eTooltipType);
	BOOL					SetSkillToolTipUpgradeInfo(AgpdSkill *pcsSkill);

	BOOL					SetSkillToolTipInfo(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12);

	// 맨 끝 LineFeed 는 맨 마지막에 한줄을 더 띄울지 말지를 결정함.
	BOOL					SetSkillToolTipBasicAttribute(AgpdSkill *pcsSkill, AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipBasicEffect(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipCostAttribute(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipDamageAttribute(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipSpecialEffect(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipFactorPoint(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipFactorPercent(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipEADamageAttribute(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipBuyCost(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					SetSkillToolTipSpecificLevelUp(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);

	// 2005.09.28. steeple
	BOOL					SetSummonsSkillToolTip(AgpdSkillTemplate* pcsSkillTemplate, AgpdSkillTooltipTemplate* pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos = 12, BOOL bLineFeed = TRUE);
	BOOL					GetUpdateFactor(INT32 lTID, AgpdFactor *pcsFactorPoint, AgpdFactor *pcsFactorPercent);
	BOOL					SetRecastSkillTimeToGridItem(INT32 lSkillID, UINT32 ulDuration, UINT32 ulRecastDelay);

	BOOL					IsActiveBuyItem(INT32 lIndex);
	BOOL					IsActiveUpgradeItem(INT32 lIndex);
	BOOL					IsActiveUpgradeHighLevelItem(INT32 lSkillTID);

	BOOL					SetStatusActiveBuyItem(INT32 lIndex);
	BOOL					SetStatusActiveUpgradeItem(INT32 lIndex);

	BOOL					SetSkillBookTooltip(AgpdItem *pcsItem, AcUIToolTip *pcsItemTooltip);
	BOOL					SetSkillScrollTooltip(AgpdItem *pcsItem, AcUIToolTip *pcsItemTooltip);

	BOOL					RefreshUsableOwnSkill(AgpdCharacter *pcsCharacter);

	INT32					GetCostWithTax(AgpdCharacter* pcsCharacter, INT32 cost);

public:
	AgcmUISkill2();
	virtual ~AgcmUISkill2();

	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnDestroy();

	// normal callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL		CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBAddSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBUpdateCharSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateInvenMoney(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBResponseGrant(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBOpenQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCloseQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBUpdateCharFactorSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBCheckCastSkillResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReceiveSkillAction(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReceiveSkillActionResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBAddBuffedList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveBuffedList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateAlarmGrid(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL		CBRefreshUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBConfirmUseSkillBook(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBSetItemTooltip(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBBuySkillResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpgradeSkillResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBLearnSkillResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBUpdateCost(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBSkillInitializeResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBInitCoolTime(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBGetSkillLevel( PVOID pData , PVOID pClass , PVOID pCustData );

	// display callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL		CBDisplaySkillPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillTrainerClass(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL		CBDisplaySkillMainDialog(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayInvenMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL		CBDisplaySkillBuyButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	
	static BOOL		CBDisplaySelectedSkillName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL		CBDisplaySkillPointHighLevel0(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel6(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel7(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel8(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel9(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillPointHighLevel10(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	// ## Product
	static BOOL		CBDisplaySkillLevelExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplaySkillExpDegree(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	// function callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL		CBSetMastery0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetMastery1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetMastery2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetMastery3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetMastery4(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetMasteryHighLevel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetMasteryHeroic(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBSetBuyMastery0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetBuyMastery1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetBuyMastery2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetBuyMastery3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBSetUpgradeMastery0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetUpgradeMastery1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetUpgradeMastery2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetUpgradeMastery3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenSkillUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenSkillBuyUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenSkillUpgradeUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenSkillHighLevelUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenSkillHighLevelSkillUpgradeUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenSkillHeroicUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenSkillHeroicSkillUpgradeUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBCloseSkillHighLevelUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCloseSkillHeroicUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBSelectSkillGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSelectBuySkillGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSelectUpgradeSkillGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBBuySkill(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBUpgradeSkill(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenButtonSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenBuySkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenBuyButtonSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenUpgradeSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBOpenUpgradeButtonSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBCloseSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBUpdateListItemStatus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBUpdateBuyListItemStatus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBUpdateUpgradeListItemStatus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBCloseSkillEventUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBToggleSkillUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenSkillDescToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCloseSkillDescToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBSkillInitialize(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetFailInitializeText(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSetConfirmInitializeText(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSkillInitializeOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBSkillRollback(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// event return callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL		CBReturnConfirmBuySkill(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBReturnConfirmLearnSkill(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBReturnConfirmUpgradeSkill(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBReturnConfirmSkillRollback(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

	// boolean callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL		CBIsActiveBuyButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL		CBIsActiveUpgradeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL		CBIsActiveRollbackButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL		CBIsActiveRollbackButton2(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	BOOL			OpenSkillUI(INT32 lMasteryIndex);
	BOOL			IsUsableSkill( void* pSkill );
	BOOL			IsEnableSkill( void* pSkill );

	VOID			RefreshInitButton( VOID );

	DWORD			GetSkillTooltipColor( eSkillTooltipColor eType )	{ return m_SkillTooltipColor[ eType ];	}

private :
	BOOL					_IsUsableSkillForMyRaceAndClass		( void* pCharacter, void* pSkillTemplate );
	UINT32					_GetDurationTime					( UINT ulExpiredTime );
	stAgpmSkillBuffList*	_GetBuffedSkill						( stAgpmSkillBuffList*	pBUffList , INT32 lSkillTID );
	VOID					_InitSkillTooltipColor				( VOID );

	BOOL					_MoveSkillWindow					( BOOL bUpgradeSkill = FALSE );


	DWORD					m_SkillTooltipColor					[AGCMUISKILL2_TOOLTIP_COLOR_COUNT];

public :
	AgcmUISkillDragonScion			m_UIDragonScion;
};

#endif	//__AGCMUISKILL2_H__