// AgcmUIMain.h: interface for the AgcmUIMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMUIMAIN_H__E027077D_0A43_44A3_825F_BFB6ED319711__INCLUDED_)
#define AFX_AGCMUIMAIN_H__E027077D_0A43_44A3_825F_BFB6ED319711__INCLUDED_

#include "AgcModule.h"
#include "AgcmUIManager2.h"
#include "AgpmFactors.h"
#include "AgcmCharacter.h"
#include "AgcmSkill.h"
#include "AgcmItem.h"
#include "AgcmEff2.h"
#include "AgcmLogin.h"
#include "AgcmUIMainSystemGridTooltip.h"
#include <list>

#define AGCMUIMAIN_MAX_QUICKBELT	4

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum
{
	AGCM_UI_MAIN_EXP_STEP_REMAIN	= 0,
	AGCM_UI_MAIN_EXP_STEP_PERCENTAGE,
	AGCM_UI_MAIN_EXP_STEP_CURRENT,
	AGCM_UI_MAIN_EXP_STEP_MAX,

	AGCM_UI_MAIN_DISPLAY_INT32,

	AGCM_UI_MAIN_DISPLAY_QBELT_LAYER,
} AgcmUIMainDisplay;

const int	AGCMUI_RCLICK_DELAY_TIME		= 200;		// milisecond
const int	AGCMUI_UPDATE_ONLINE_TIME_INTERVAL = 5000;

// define function names
/////////////////////////////////////////////////////////////
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_1			"QuickBeltUse_1"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_2			"QuickBeltUse_2"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_3			"QuickBeltUse_3"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_4			"QuickBeltUse_4"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_5			"QuickBeltUse_5"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_6			"QuickBeltUse_6"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_7			"QuickBeltUse_7"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_8			"QuickBeltUse_8"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_9			"QuickBeltUse_9"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_10			"QuickBeltUse_10"

#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_HPPOTION	"QuickBeltUseHPPotion"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_MPPOTION	"QuickBeltUseMPPotion"

#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_LAYER_UP		"QuickBeltLayerUp"
#define	AGCMUI_MAIN_FUNCTION_QUICK_BELT_LAYER_DOWN		"QuickBeltLayerDown"

#define	AGCMUI_MAIN_FUNCTION_SET_FOCUS_MOVABLE_ITEM		"SetFocusMovableItem"
#define	AGCMUI_MAIN_FUNCTION_KILL_FOCUS_MOVABLE_ITEM	"KillFocusMovableItem"

#define AGCMUI_MAIN_MAX_SYSTEM_GRID_COLUMN				6

typedef enum	_AgcmUIMainCB {
	AGCMUI_MAIN_CB_OPEN_TOOLTIP				= 0,
	AGCMUI_MAIN_CB_CLOSE_TOOLTIP,
	AGCMUI_MAIN_CB_CLOSE_ALL_UI_TOOLTIP,
	AGCMUI_MAIN_CB_UPDATE_ALARM_GRID,
	AGCMUI_MAIN_CB_RBUTTON_SKILL_CAST,
	AGCMUI_MAIN_CB_KEYDOWN_CLOSE_ALL_UI_EXCEPT_MAIN,

	AGCMUI_MAIN_CB_ADD_SHORTCUT_ITEM,
	AGCMUI_MAIN_CB_REMOVE_SHORTCUT_ITEM,
	AGCMUI_MAIN_CB_UPDATE_HP_POTION,
	AGCMUI_MAIN_CB_UPDATE_MP_POTION,
	AGCMUI_MAIN_CB_KEYDOWN_ESC,

	AGCMUI_MAIN_CB_UPDATE_HPPOINT_GAGE,
	AGCMUI_MAIN_CB_UPDATE_MPPOINT_GAGE,
} AgcmUIMainCB;

const int	AGCMUI_MAIN_PROCESS_IDLE_INTERVAL_MSEC		= 500;


struct stSystemGridControlEntry
{
	AgpdGridItem*						m_ppdGridItem;
	ApModuleDefaultCallBack				m_fnCallBack;
	void*								m_pClass;

	stSystemGridControlEntry( void )
	{
		m_ppdGridItem = NULL;
		m_fnCallBack = NULL;
		m_pClass = NULL;
	}
};

class AgcmUIMain : public AgcModule  
{
private:
	typedef		std::list< stSystemGridControlEntry >				ListSystemGridEntry;
	typedef		std::list< stSystemGridControlEntry >::iterator	ListSystemGridEntryIter;

	AgcmUIManager2	*	m_pcsAgcmUIManager2	;
	AgpmGrid		*	m_pcsAgpmGrid		;
	AgpmFactors		*	m_pcsAgpmFactors	;
	AgpmCharacter	*	m_pcsAgpmCharacter	;
	AgpmItem		*	m_pcsAgpmItem		;
	AgpmSkill		*	m_pcsAgpmSkill		;
	AgcmCharacter	*	m_pcsAgcmCharacter	;
	AgcmSkill		*	m_pcsAgcmSkill		;
	AgcmItem		*	m_pcsAgcmItem		;
	ApmMap			*	m_pcsApmMap			;
	AgcmEff2		*	m_pcsAgcmEff2		;
	AgcmLogin		*	m_pcsAgcmLogin		;

	AgcdUIUserData *	m_apstQuickBelt[2];
//	AgcdUIUserData *	m_apstVQuickBelt[2];
//	AgcdUIUserData *	m_pstSkill;

	AgcdUIUserData *	m_pstHPPotionGrid;
	AgcdUIUserData *	m_pstMPPotionGrid;

	AgcdUIUserData *	m_pstSystemMessageGrid;

	AgcdUIUserData *	m_apstQBLayer[2];

	AcUIToolTip			m_csQuickBeltToolTip;

	AgpdGrid			m_csAlarmGrid;
	AgcdUIUserData *	m_pcsAlarmGrid;

	AgcdUIUserData *	m_pcsEnableExceptJapan;
	BOOL				m_bEnableExceptJapan;

	AcUIEdit*			m_pcsOnlineTimeEdit;
	AcUIEdit*			m_pcsAddictStatusEdit;
	AgcdUIUserData		*m_pcsOnlineTimeAddictStatus;
	UINT32				m_ulLastUpdateOnlineTime;
	DWORD				m_ulStartOnlineTime;
	
	//	Event ID 
	INT32				m_lEventIDAddInvalidItem	;
	INT32				m_lEventIDRemoveItem;
	INT32				m_lEventGetPotionScrollBarControl;
	INT32				m_lEventOpenMediaRate;
	INT32				m_lEventOpenMediaRate18;
	INT32				m_lEventCloseMediaRate;
	INT32				m_lEventGetOnlineTimeEdit;
	INT32				m_lEventGetAddictStatusEdit;
	
	UINT32				m_ulPrevProcessClockCount;

	BOOL				m_bIsVerticalQuickBelt;

	UINT32				m_ulRClickDownTMSec;

	INT32				m_lDragCursorID;
	INT32				m_lPrevCursor;

	BOOL				RemoveGridLayer(AgpdGrid *pcsGrid, AgpdGridItem *pcsGridItem, INT32 lLayer);

	AgcdUIControl		*m_pcsHPPointScrollBarControl;
	AgcdUIControl		*m_pcsMPPointScrollBarControl;

	BOOL				m_bMediaRateWindow_Show;
	UINT32				m_ulMediaRateWindow_RemainSec;
	UINT32				m_ulMediaRateWindow_LastTick;

	MTRand				m_csRand;
	
public:
	INT32				m_lHPPointScrollBarValue;
	INT32				m_lMPPointScrollBarValue;

public:
	AgpdGrid			m_astQuickBeltGrid[2];
//	AgpdGrid			m_astVQuickBeltGrid[2];
//	AgpdGrid			m_stSkillGrid;
	INT32				m_alCurrentLayer[2];

	//@{ 2006/07/05 burumal
	BOOL				m_bQuickBeltSwitchButton[2];
	INT32				m_nSavedCurrentQuickBeltLayer;
	//@}

	AgpdGrid			m_stHPPotionGrid;
	AgpdGrid			m_stMPPotionGrid;

	AgpdGrid			m_stSystemMessageGrid;

	INT32				m_lTotalHPPotionCount;
	INT32				m_lTotalMPPotionCount;
	INT32				m_lTotalArrowCount;
	INT32				m_lTotalBoltCount;

	AgpdGridItem *		m_pcsHPPotionItem;
	AgpdGridItem *		m_pcsMPPotionItem;

	AgpdGridItem *		m_pcsArrowItem;
	AgpdGridItem *		m_pcsBoltItem;

	INT32				m_lEventMainUIAllClose;		// Main UI 관련 창들을 모두 닫음

	CHAR				m_szGameServerName[129];
	
	BOOL				m_bIsNC17;

	AgcmUIMainSystemGridTooltip		m_SystemGridTooltip;

public:
	AgcmUIMain();
	virtual ~AgcmUIMain();

	BOOL			OnAddModule();
	BOOL			OnDestroy();
	BOOL			OnInit();
	BOOL			OnInitQuickBeltGrid( void );

	BOOL			OnIdle(UINT32 ulClockCount);

	BOOL			AddShortcut(AgpdGrid *pcsGrid, AgpdGridItem *pcsGridItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsThrowEvent = TRUE);
	BOOL			RemoveShortcut(AgpdGrid *pcsGrid, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsThrowEvent = TRUE);

	BOOL			AddHPPotion(INT32 lHPPotionTID);
	BOOL			RemoveHPPotion();
	BOOL			AddMPPotion(INT32 lMPPotionTID);
	BOOL			RemoveMPPotion();

	BOOL			SetHPPointScrollBarValue(INT32 lHPPoint);
	BOOL			SetMPPointScrollBarValue(INT32 lMPPoint);

	BOOL			AddSystemMessageGridItem(AgpdGridItem *pcsGridItem, ApModuleDefaultCallBack pfUseCallback = NULL, PVOID pvCallbackClass = NULL , BOOL bFrontAdd = FALSE );
	BOOL			RemoveSystemMessageGridItem(AgpdGridItem *pcsGridItem);
	BOOL			ClearSystemMessageGridItem( VOID );

	BOOL			SetArrow();
	BOOL			ResetArrow();
	BOOL			SetBolt();
	BOOL			ResetBolt();

	BOOL			SetOnlineTimeAddictStatus(UINT32 ulClockCount);

	static	BOOL	CBAddShortcut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBRemoveShortcut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBAddHPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBAddMPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBRemovePotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBLayer1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBLayer2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBLayer3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBLayer4(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBDisplayCurrentLayer(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBDisplayExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBGetOnlineTimeEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBGetAddictStatusEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBDisplayDefault(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBDisplayInt32(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBQuickBeltUse_1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_4(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_5(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_6(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_7(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_8(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_9(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUse_10(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBQuickBeltUseHPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltUseMPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBQuickBeltLayerUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBQuickBeltLayerDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBCloseUIWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	// Control별 Action Function
	static	BOOL	CBSelectListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static  BOOL	CBEditControlInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static  BOOL	CBButtonControlDisable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static  BOOL	CBButtonControlEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBEditControlActive(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBLClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBRClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBRClickUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBPotionGridRClickUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBClickSystemGridNoAction(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBClickSystemGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBChangeHPPointScollBarValue(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBChangeMPPointScollBarValue(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBGetPotionScollBarControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// Main UI와 관련된 모든 창을 닫음
	static	BOOL	CBMainUIAllClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	BOOL			RemoveGridItemFromQuickBelt(AgpdGridItem *pcsGridItem);
	BOOL			RefreshQuickBeltGrid();

	BOOL			CheckQuickBeltItem(INT32 lItemID);

	// tool tip 관련 함수들
	static BOOL		CBOpenToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCloseToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCloseAllUIToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBOpenSystemGridToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCloseSystemGridToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	BOOL			SetCallbackOpenTooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackCloseTooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackCloseAllUITooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackUpdateAlarmGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRButtonSkillCast(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackKeydownCloseAllUIExceptMain(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackKeydownESC(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackAddShortcut(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRemoveShortcut(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackUpdateHPPotion(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackUpdateMPPotion(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackUpdateHPPointGage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackUpdateMPPointGage(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			UpdateAlarmGrid();
	BOOL			AddItemToAlarmGrid(AgpdGridItem *pcsGridItem);
	BOOL			ClearAlarmGrid();

	void			UpdateMediaRateWindow(UINT32 ulClockCount);

	// Application Quit 관련 
	INT32			m_lEventMainQuit			;		// Application 종료 여부를 Yes No 로 묻고 종료한다 
	static BOOL	    CBUIReturnAppQuit	(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )	;
	static BOOL		CBApplicationQuitEventCall(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// QuickBelt Toggle 시 좌표 변환을 위한 Function
	static BOOL		CBQuickBeltAdjustPos(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBQuickBeltVAdjustPos(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
//	AgpdGridItem*	GetSkillGridItem();

	static BOOL		CBDefaultBoolean(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSelfCharacterDead(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL		CBIsSelected(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL		CBSetFocusMovableItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBKillFocusMovableItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBUpdateActionStatus(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	BOOL			ProcessSkill(INT32 lSkillID);
	BOOL			UseBelt( INT32 nLayer , INT32 nIndex );

//	static BOOL		CBInitItem(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL		CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBAddItemToInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveItemFromInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateStackCount(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBGetRegisteredMPPotionTID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// 케릭터가 리젼을 넘어갈때 호출됨..
	static BOOL		CBChangeCharacterBindingRegion(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBIDPasswordOK(PVOID pData, PVOID pClass, PVOID pCustData);

	// 한국버전 Client인가
	static BOOL		CBBooleanIsKorea(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourcecontrol);

	BOOL			UseGridItem(AgpdGridItem *pcsGridItem, INT32 lQBGridIndex, INT32 lColumnIndex);
	BOOL			RefreshQBeltGridItemStackCount(INT32 lLayer = 0);

	// 중독방지 2007.07.06. steeple
	static BOOL		CBUpdateOnlineTime(PVOID pData, PVOID pClass, PVOID pCustData);

	// 이 컨트롤이 현재 UI 의 컨트롤인가.
	BOOL			IsCurrentUI( void* pControl );

public :
	ListSystemGridEntry			m_ListSystemGridControl;

public :
	BOOL			OnAddSystemGridControl				( AgpdGridItem* ppdGridItem, ApModuleDefaultCallBack fnCallBack, void* pClass , BOOL bFrontAdd = FALSE );
	BOOL			OnDeleteSystemGridControl			( AgpdGridItem* ppdGridItem );

	BOOL			OnClickSystemGridControl			( AgpdGridItem* ppdGridItem );

	BOOL			OnClearSystemGrid					( void );
	BOOL			OnRefreshSystemGrid					( void );

private:
	BOOL			_OpenSystemGridTooltip				( AgcdUIControl *pcsSourceControl );
	BOOL			_CloseSystemGridTooltip				( AgcdUIControl *pcsSourceControl );

	INT32			GetItemTotalCount					( AgpdItem * item );
	void			RefreshItemCount					( AgpdItem * item );
	INT32			RefreshItemCount					( INT32 & count, AgpdGridItem * grid, AgpdItem * item );
};

#endif // !defined(AFX_AGCMUIMAIN_H__E027077D_0A43_44A3_825F_BFB6ED319711__INCLUDED_)
