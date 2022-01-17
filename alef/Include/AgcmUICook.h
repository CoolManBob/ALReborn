/*=============================================================

	AgcmUICook.h

=============================================================*/

#ifndef _AGCM_UI_COOK_H_
	#define _AGCM_UI_COOK_H_

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
#include "AgcmEventProduct.h"
#include "AgcmChatting2.h"
#include "AgcmUIMain.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgcmUICookD")
#else
	#pragma comment (lib , "AgcmUICook")
#endif
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_GRAY		0XFFAEA59C
#define COLOR_RED		0xFFFF0000

#define AGCMUICOOK_MAX_LIST			100
#define AGCMUICOOK_MAX_PAGE_LIST		13

const enum eAgcmUICookEvent
	{
	AGCMUICOOK_EVENT_SELECT_OPEN = 0,
	AGCMUICOOK_EVENT_SELECT_CLOSE,
	AGCMUICOOK_EVENT_SELECT_LIST,
	AGCMUICOOK_EVENT_MAKE_OPEN,
	AGCMUICOOK_EVENT_MAKE_CLOSE,
	AGCMUICOOK_EVENT_MAKE_SUCCESS,
	AGCMUICOOK_EVENT_MAKE_FAIL,
	AGCMUICOOK_EVENT_GATHER_SUCCESS,
	AGCMUICOOK_EVENT_GATHER_FAIL,
	AGCMUICOOK_EVENT_MAX
	};

extern CHAR g_szEvent[AGCMUICOOK_EVENT_MAX][30];

#define AGCMUICOOK_MESSAGE_SELECT					"UI_MESSAGE_COOK_SELECT"
#define AGCMUICOOK_MESSAGE_MAKE						"UI_MESSAGE_COOK_MAKE"
#define AGCMUICOOK_MESSAGE_MAKE2					"UI_MESSAGE_COOK_MAKE2"
#define AGCMUICOOK_MESSAGE_SUCCESS					"UI_MESSAGE_SKILL_PROD_SUCCESS"
#define AGCMUICOOK_MESSAGE_FAIL						"UI_MESSAGE_SKILL_PROD_FAIL"
#define AGCMUICOOK_MESSAGE_ITEM						"UI_MESSAGE_SKILL_PROD_ITEM"
#define AGCMUICOOK_MESSAGE_EXP						"UI_MESSAGE_SKILL_PROD_EXP"

#define AGCMUICOOK_MAX_LIST			100
#define AGCMUICOOK_MAX_GRID			8

/************************************************/
/*		The Definition of AgcmUICook class		*/
/************************************************/
//
class AgcmUICook : public AgcModule
	{
	public:
		// ... Related modules
		AgpmGrid				*m_pcsAgpmGrid;
		AgpmCharacter			*m_pcsAgpmCharacter;
		AgcmCharacter			*m_pcsAgcmCharacter;
		AgcmUIManager2			*m_pcsAgcmUIManager2;
		AgpmProduct				*m_pcsAgpmProduct;
		AgcmProduct				*m_pcsAgcmProduct;
		AgpmItem				*m_pcsAgpmItem;
		AgcmItem				*m_pcsAgcmItem;
		AgcmUIControl			*m_pcsAgcmUIControl;
		AgpmFactors				*m_pcsAgpmFactors;
		AgpmSkill				*m_pcsAgpmSkill;
		AgcmSkill				*m_pcsAgcmSkill;
		AgcmChatting2			*m_pcsAgcmChatting;
		AgcmUIMain				*m_pcsAgcmUIMain;

		// ... Tooltip
		AcUIToolTip				m_csToolTip;

		// ... Grid
		AgpdGrid				m_GridSourceItem;

		// ... UI events
		INT32					m_lEvent[AGCMUICOOK_EVENT_MAX];

		// 
		INT32					m_lSelectedCookID;
		INT32					m_lReceipeID;

		// ... User data
		AgcdUIUserData			*m_pstSelectList;
		AgcdUIUserData			*m_pstMakeEdit;
		AgcdUIUserData			*m_pstMakeSourceItem;
		AgcdUIUserData			*m_pstMakeOk;
		AgcdUIUserData			*m_pstMakeCancel;

		INT32					m_lCookCount;
		INT32					m_lCookList[AGCMUICOOK_MAX_LIST];
		INT32					m_lCookIndex;
		AgpdGridItem			*m_pGridItemList[AGCMUICOOK_MAX_GRID];

		AuPOS					m_stOpenPos;
	
		BOOL					m_bHoldOn;		// 패킷전송후 완료되기 전까지는 다시 패킷을 보내지 않는다.
		BOOL					m_bIsWindowOpen;

	protected:
		BOOL AddEvent();
		BOOL AddFunction();
		BOOL AddDisplay();
		BOOL AddUserData();

	public:
		AgcmUICook();
		virtual ~AgcmUICook();

		// ... ApModule inherited
		BOOL OnAddModule();
		BOOL OnInit();
		BOOL OnDestroy();

		// ... Functions
		static BOOL CBOpenSelectWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickSelectListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickSelectListPageup(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickSelectListPagedn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBOpenSelectTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseSelectTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBOpenMakeWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickMakeOkButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickMakeCancelButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		// ... Display callbacks
		static BOOL CBDisplaySelectList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMakeEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMakeOk(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMakeCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		// ... Drag & Drop callback
		static BOOL CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		// ... Result callback
		static BOOL CBProductResult(PVOID pData, PVOID pClass, PVOID pCustData);

		// ... Grant callback
		static BOOL	CBCastCook(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

	protected:
		// ... Display helper
		BOOL DisplaySourceItem();
		BOOL RefreshSourceItemGrid();
		BOOL ClearGrid();
		BOOL SetTooltipText(AgpdProduct* pProduct);
		BOOL SetGridItemAttachedTexture( AgpdGridItem* pcsAgpdGridItem, AgpdItemTemplate* pcsAgpdItemTempalte );
			
		// ... Helper
		BOOL AddGridItem(AgpdItemTemplate* pItemTemplate, INT32 lCount);
		AgpdProduct* GetProductByIndex(INT32 lUserDataIndex);
		
		// ... Message
		BOOL AddSystemMessage(CHAR* pszMsg);
	};

#endif
