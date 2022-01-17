/*=============================================================

	AgcmUIProduct.h

=============================================================*/

#ifndef _AGCM_UI_PRODUCT_H_
	#define _AGCM_UI_PRODUCT_H_

#include "AgcModule.h"
#include "AgcmUIManager2.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgpmProduct.h"
#include "AgpmItem.h"
#include "AgpmFactors.h"
#include "AgcmItem.h"
#include "AgcmUIControl.h"
#include "AgpmSkill.h"
#include "AgcmSkill.h"
#include "AgcmProduct.h"
#include "AgcmChatting2.h"
#include "AgcmUIMain.h"
#include "AgcmUICooldown.h"
#include "AgcmUIProductSkill.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgcmUIProductD")
#else
	#pragma comment (lib , "AgcmUIProduct")
#endif
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_GRAY		0XFFAEA59C
#define COLOR_RED		0xFFFF0000

#define AGCMUIPRODUCT_MAX_LIST			100
#define AGCMUIPRODUCT_MAX_PAGE_LIST		13

const enum eAGCMUIPRODUCT_EVENT
	{
	AGCMUIPRODUCT_EVENT_OPEN = 0,
	AGCMUIPRODUCT_EVENT_CLOSE,
	AGCMUIPRODUCT_EVENT_SELECT_LIST,
	AGCMUIPRODUCT_EVENT_MAKE,
	AGCMUIPRODUCT_EVENT_SUCCESS,
	AGCMUIPRODUCT_EVENT_FAIL,
	AGCMUIPRODUCT_EVENT_MAX
	};

const enum eAGCMUIPRODUCT_MESSAGE
	{
	AGCMUIPRODUCT_MESSAGE_TITLE = 0,
	AGCMUIPRODUCT_MESSAGE_SELECT_EDIT,
	AGCMUIPRODUCT_MESSAGE_MAKE_TITLE,
	AGCMUIPRODUCT_MESSAGE_MAKE_NAME,
	AGCMUIPRODUCT_MESSAGE_MAKE_LEVEL,
	AGCMUIPRODUCT_MESSAGE_MAKE_PRICE,
	AGCMUIPRODUCT_MESSAGE_OK,
	AGCMUIPRODUCT_MESSAGE_CANCEL,
	AGCMUIPRODUCT_MESSAGE_MAX
	};


#define AGCMUIPRODUCT_MAX_LIST			100
#define AGCMUIPRODUCT_MAX_GRID			8

/****************************************************/
/*		The Definition of AgcmUIProduct class		*/
/****************************************************/
//
class AgcmUIProduct : public AgcModule
	{
	public:
		static CHAR s_szMessage[AGCMUIPRODUCT_MESSAGE_MAX][30];
		static CHAR s_szEvent[AGCMUIPRODUCT_EVENT_MAX][30];
		static CHAR* s_szL10NCategory[AGPMPRODUCT_CATEGORY_MAX];

	public:
		//	Related modules
		AgpmGrid				*m_pAgpmGrid;
		AgpmCharacter			*m_pAgpmCharacter;
		AgcmCharacter			*m_pAgcmCharacter;
		AgcmUIManager2			*m_pAgcmUIManager2;
		AgpmProduct				*m_pAgpmProduct;
		AgcmProduct				*m_pAgcmProduct;
		AgpmItem				*m_pAgpmItem;
		AgcmItem				*m_pAgcmItem;
		AgcmUIControl			*m_pAgcmUIControl;
		AgpmFactors				*m_pAgpmFactors;
		AgpmSkill				*m_pAgpmSkill;
		AgcmSkill				*m_pAgcmSkill;
		AgcmChatting2			*m_pAgcmChatting;
		AgcmUIMain				*m_pAgcmUIMain;
		AgcmUICooldown			*m_pAgcmUICooldown;
		AgcmUIProductSkill		*m_pAgcmUIProductSkill;

		//	Tooltip
		AcUIToolTip				m_csTooltip;

		//	Item Grid
		AgpdGrid				m_GridSourceItem;
		AgpdGridItem			*m_pGridItemList[AGCMUIPRODUCT_MAX_GRID];

		//	UI Events
		INT32					m_lEvent[AGCMUIPRODUCT_EVENT_MAX];

		//	User data
		AgcdUIUserData			*m_pstDummy;
		AgcdUIUserData			*m_pstSelectList;
		AgcdUIUserData			*m_pstMakeEdit;
		AgcdUIUserData			*m_pstMakeSourceItem;
		AgcdUIUserData			*m_pstActiveComposeButton;

		INT32					m_lComposeBtn;

		//	Product data
		INT32					m_eCategory;								// product category
		INT32					m_lSkillID;									// product skill
		INT32					m_lComposeList[AGCMUIPRODUCT_MAX_LIST];		// compose id(s) of given category
		INT32					m_lTotalCompose;							// total count of compose id of given category
		INT32					m_lSelectedComposeID;						// Selected Compose ID
		INT32					m_lReceipeID;								// Receipe ID
		

		BOOL					m_bHold;									// use action block
		BOOL					m_bIsWindowOpen;

		INT32					m_lCooldownID;
		

	protected:
		BOOL AddEvent();
		BOOL AddFunction();
		BOOL AddDisplay();
		BOOL AddUserData();
		BOOL AddBoolean();

	public:
		AgcmUIProduct();
		virtual ~AgcmUIProduct();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();

		//	Open
		BOOL	Open(INT32 eCategory, INT32 lSkillID);

		//	Functions
		static BOOL CBOpenWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickOkButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Display callbacks
		static BOOL CBDisplayTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplaySelectEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplaySelectList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMakeEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMakeOk(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMakeCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		//	Drag & Drop callback
		static BOOL CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Boolean callback
		static BOOL CBIsActiveComposeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

		//	Cooldown Callback
		static BOOL CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Operation
		BOOL	OnMake();
		BOOL	CastAnimation(AgpdCharacter *pAgpdCharacter, FLOAT *pfAnimDuration);

		//	Result
		BOOL	OnResult(AgpdSkill *pAgpdSkill, INT32 lResult, INT32 lItemTID, INT32 lExp, int nCount );
		void	OnPostResult();

		//	System driven UI close callback
		static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

		BOOL	AddSystemMessage(CHAR* pszMsg);
		
	protected:
		//	Display helper
		BOOL	DisplaySourceItem();
		BOOL	AddGridItem(AgpdItemTemplate *pItemTemplate, INT32 lCount, INT32 lIndex);
		BOOL	SetGridItemAttachedTexture(AgpdGridItem *pAgpdGridItem, AgpdItemTemplate *pAgpdItemTempalte);
		BOOL	RefreshSourceItemGrid();
		BOOL	ClearGrid();
		void	RefreshAll();
		
		//	Helper
		BOOL					IsComposable(AgpdComposeTemplate *pAgpdComposeTemplate);
		AgpdComposeTemplate*	GetComposeByIndex(INT32 lUserDataIndex);
		INT32	GetMessageTxt(INT32 eCategory, eAGCMUIPRODUCT_MESSAGE eMessage, CHAR *pszBuffer, INT32 lSize);
		CHAR*	GetMessageTxt(INT32 eCategory, eAGCMUIPRODUCT_MESSAGE eMessage);
	};

#endif
