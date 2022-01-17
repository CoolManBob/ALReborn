/*=============================================================

	AgcmUIRefinery.h

=============================================================*/

#ifndef _AGCM_UI_REFINERY_H_
	#define _AGCM_UI_REFINERY_H_

#include "AgcModule.h"
#include "AgcmUIManager2.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgpmRefinery.h"
#include "AgpmItem.h"
#include "AgpmFactors.h"
#include "AgcmItem.h"
#include "AgcmUIControl.h"
#include "AgcmRefinery.h"
#include "AgcmEventRefinery.h"
#include "AgcmChatting2.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgcmUICooldown.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgcmUIRefineryD")
#else
	#pragma comment (lib , "AgcmUIRefinery")
#endif
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_GRAY		0XFFAEA59C
#define COLOR_RED		0xFFFF0000


const enum eAGCMUIREFINERY_EVENT
	{
	AGCMUIREFINERY_EVENT_OPEN = 0,
	AGCMUIREFINERY_EVENT_CLOSE,
	AGCMUIREFINERY_EVENT_REFINE,
	AGCMUIREFINERY_EVENT_RESET,
	AGCMUIREFINERY_EVENT_SUCCESS,
    AGCMUIREFINERY_EVENT_END,
    AGCMUIREFINERY_EVENT_REFINEMONEYMSGBOX_OPEN,
	AGCMUIREFINERY_EVENT_MAX
	};


const enum eAGCMUIREFINERY_MESSAGE
	{
	AGCMUIREFINERY_MESSAGE_MAIN_EDIT = 0,
	AGCMUIREFINERY_MESSAGE_SUB_TITLE,
	AGCMUIREFINERY_MESSAGE_SUB_NOTIFY,
	AGCMUIREFINERY_MESSAGE_RESULT_SUCCESS,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_ITEM,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_DIFFRENT_ITEM,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_MONEY,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_UNABLE_TO_LOAD,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_UNREFINABLE_ITEM,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_FULL_INVENTORY,
	AGCMUIREFINERY_MESSAGE_RESULT_ALREADY_SATISFY_QUANTITY,
	AGCMUIREFINERY_MESSAGE_RESULT_SAME_STONE_ATTRIBUTE,
	AGCMUIREFINERY_MESSAGE_RESULT_INSUFFICIENT_OPTION_ITEM,
	AGCMUIREFINERY_MESSAGE_REFINABLE,
	AGCMUIREFINERY_MESSAGE_RESULT_FAIL_UNREFINABLE_TO_NEXT,
	AGCMUIREFINERY_MESSAGE_MAX
	};




/****************************************************/
/*		The Definition of AgcmUIRefinery class		*/
/****************************************************/
//
class AgcmUIRefinery : public AgcModule
	{
	public:
		static CHAR s_szMessage[AGCMUIREFINERY_MESSAGE_MAX][30];
		static CHAR s_szEvent[AGCMUIREFINERY_EVENT_MAX][30];

	public:
		//	Related modules
		AgpmFactors				*m_pAgpmFactors;
		AgpmGrid				*m_pAgpmGrid;		
		AgpmCharacter			*m_pAgpmCharacter;
		AgpmItem				*m_pAgpmItem;
		AgpmRefinery			*m_pAgpmRefinery;
		AgcmCharacter			*m_pAgcmCharacter;
		AgcmItem				*m_pAgcmItem;
		AgcmRefinery			*m_pAgcmRefinery;
		AgcmEventRefinery		*m_pAgcmEventRefinery;
		AgcmChatting2			*m_pAgcmChatting;
		AgcmUIManager2			*m_pAgcmUIManager2;
		AgcmUIControl			*m_pAgcmUIControl;
		AgcmUIMain				*m_pAgcmUIMain;
		AgcmUIItem				*m_pAgcmUIItem;
		AgcmUICooldown			*m_pAgcmUICooldown;

		//	Tooltip
		AcUIToolTip				m_csToolTip;

		//	Grid
		AgpdGrid				m_AgpdGridSource;
		AgpdGrid				m_AgpdGridResult;
		AgpdGridItem			*m_pAgpdGridItemResult;
		
		//	UI events
		INT32					m_lEvent[AGCMUIREFINERY_EVENT_MAX];

		//	User data
		AgcdUIUserData			*m_pstActiveTab;
		AgcdUIUserData			*m_pstMainEdit;
		AgcdUIUserData			*m_pstSubTitle;
		AgcdUIUserData			*m_pstSubNotify;
		AgcdUIUserData			*m_pstPrice;
		AgcdUIUserData			*m_pstSourceItem;
		AgcdUIUserData			*m_pstResultItem;
		AgcdUIUserData			*m_pstActiveRefineButton;

		INT32					m_lDummy;				// dummy for tab
		INT32					m_lRefineButton;

		//			
		INT32					m_eCategory;			// category
		ApSafeArray<AgpdItem *, AGPMREFINERY_MAX_GRID>	m_pSources;
		
		AgpdRefineTemplate		*m_pAgpdRefineTemplate;	// selected refine template
		INT32					m_eResult;
		eAGCMUIREFINERY_MESSAGE	m_eNotifyMsg;
		CHAR					*m_pszNotify;
		
		BOOL					m_bHold;				// hold on until packet result processed
		BOOL					m_bIsOpen;
		AuPOS					m_stOpenPos;
		
		INT32					m_lCooldownID;

	protected:
		//	General helper
		void	Reset(BOOL bRefresh = TRUE);
		CHAR*	GetMessageTxt(INT32 eCategory, eAGCMUIREFINERY_MESSAGE eMessage);
		BOOL	AddSystemMessage(CHAR* pszMsg);
		//BOOL	IsValidRefineStone();

        //	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		//BOOL	AddBoolean();
		
		//	Grid helper
		BOOL	InitializeGrid();
		void	DestroyGrid();
		BOOL	AddItemToSourceGrid(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
		BOOL	RemoveItemFromSourceGrid(AgpdItem *pAgpdItem);
		BOOL	AddItemToResultGrid(AgpdItemTemplate *pItemTemplate, INT32 lCount);
		void	RefreshSourceGrid();
		void	RefreshResultGrid();
		BOOL	SetGridItemAttachedTexture(AgpdGridItem *pAgpdGridItem, AgpdItemTemplate *pAgpdItemTempalte);
		void	ClearSourceGrid();
		void	ClearResultGrid();

	public:
		AgcmUIRefinery();
		virtual ~AgcmUIRefinery();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();

		//	Operations
		BOOL	Open();
		BOOL	OnResult(INT32 eResult, INT32 lResultItemTID);
		void	OnSuccess(INT32 lResultItemTID);
		BOOL	OnMoveItem(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
		
		//	Functions
		static BOOL CBOpenWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
        static BOOL CBRefine(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
		static BOOL CBClickRefineButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickResetButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		//	Display callbacks
		static BOOL CBDisplayMainEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplaySubTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplaySubNotify(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		//	Boolean
		//static BOOL CBIsActiveRefineButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		
		//	Cooldown Callback
		static BOOL CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Result Callback
		static BOOL CBRefineResult(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Grant Callback
		static BOOL	CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Position
		static BOOL CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

		//	System driven UI close callback
		static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBRemoveItemFromInventory(PVOID pData, PVOID pClass, PVOID pCustData);

		// From AgcmItem
		static BOOL CBCheckUseItem(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
