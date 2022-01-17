/*====================================================================

	AgcmUIGamble.h
	
====================================================================*/


#ifndef _AGCM_UI_GAMBLE_H_
	#define _AGCM_UI_GAMBLE_H_


#include "AgpmGamble.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmUIMain.h"
#include "AgcmUICharacter.h"
#include "AgcmEventManager.h"
#include "AgcmChatting2.h"
#include "AgcmUIConsole.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const INT32		AGCMUIGAMBLE_CLOSE_UI_DISTANCE			=	1600;
const INT32		AGCMUIGAMBLE_GRID_LAYER					=	1;				// never change this
const INT32		AGCMUIGAMBLE_GRID_ROW					=	30;
const INT32		AGCMUIGAMBLE_GRID_COLUMN				=	4;


const enum eAGCMUIGAMBLE_MESSAGE
	{
	AGCMUIGAMBLE_MESSAGE_SUCCESS = 0,
	AGCMUIGAMBLE_MESSAGE_FULL_INVEN,
	AGCMUIGAMBLE_MESSAGE_NA_RACE,
	AGCMUIGAMBLE_MESSAGE_NA_CLASS,
	AGCMUIGAMBLE_MESSAGE_NOT_ENOUGH_LEVEL,
	AGCMUIGAMBLE_MESSAGE_NOT_ENOUGH_POINT,
	AGCMUIGAMBLE_MESSAGE_CONFIRM,
	AGCMUIGAMBLE_MESSAGE_MAX
	};


const enum eAGCMUIGAMBLE_EVENT
	{
	AGCMUIGAMBLE_EVENT_OPEN = 0,
	AGCMUIGAMBLE_EVENT_CLOSE,
	AGCMUIGAMBLE_EVENT_MAX
	};




/****************************************************/
/*		The Definition of AgcmUIGamble class		*/
/****************************************************/
//
class AgcmUIGamble : public ApModule
	{
	private:
		//	Related modules
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmItem			*m_pAgpmItem;
		AgpmGrid			*m_pAgpmGrid;
		AgpmGamble			*m_pAgpmGamble;
		AgcmCharacter		*m_pAgcmCharacter;
		AgcmItem			*m_pAgcmItem;
		AgcmUIManager2		*m_pAgcmUIManager2;
		AgcmUIControl		*m_pAgcmUIControl;
		AgcmUIMain			*m_pAgcmUIMain;
		AgcmEventManager	*m_pAgcmEventManager;
		AgcmChatting2		*m_pAgcmChatting2;
		AgcmUICharacter		*m_pAgcmUICharacter;

		//	Message ID
		static CHAR			s_szMessage[AGCMUIGAMBLE_MESSAGE_MAX][30];
		
		//	Event
		INT32				m_lEvent[AGCMUIGAMBLE_EVENT_MAX];
		static CHAR			s_szEvent[AGCMUIGAMBLE_EVENT_MAX][30];
		
		//	user-specific(by race and class) gamble template list
		ApSafeArray<INT32, 200>		m_alTemplateList;
		
		//	UI Grid related
		AgpdGrid			m_AgpdGrid_TemplateList;
		AgcdUIUserData		*m_pAgcdUIUserData_Grid;
		
		//	UI Open Position
		AuPOS				m_stOpenPos;
		
		//	Tooltip
		AcUIToolTip			m_Tooltip;
		
	public:
		AgcmUIGamble();
		virtual ~AgcmUIGamble();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		
		//	Window
		BOOL	Open();
		void	Close();
		
	  #ifdef _DEBUG
		void	TestOpen();
	  #endif
		
		//	Packet Send
		BOOL	SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent);
		BOOL	SendGamble(AgpdCharacter *pAgpdCharacter, INT32 lGambleTID);

		//	Module Callback
		static BOOL	CBResultGamble(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Event Callback
		static BOOL	CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Main UI callback
		static BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	UI Function Callback
		static BOOL	CBClickClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBGridSetFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBGridKillFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBRightClickGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	protected :
		//	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		
		//	helper
		BOOL	OnResultGamble(INT32 lResult, INT32 lTID);
		BOOL	PrepareGambleGridItem(AgpdCharacter *pAgpdCharacter);
		void	RemoveGambleGridItem();
		void	OpenTooltip(INT32 lTID, INT32 x, INT32 y);
		void	CloseTooltip();
		CHAR*	GetMessageTxt(eAGCMUIGAMBLE_MESSAGE eMessage);
		void	AddSystemMessage(CHAR *pszMessage);
	};


#endif