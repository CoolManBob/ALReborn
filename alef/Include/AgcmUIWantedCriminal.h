/*====================================================================

	AgcmUIWantedCriminal.h
	
====================================================================*/


#ifndef _AGCM_UI_WANTEDCRIMINAL_H_
	#define _AGCM_UI_WANTEDCRIMINAL_H_


#include "AgpmWantedCriminal.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmUIMain.h"
#include "AgcmEventManager.h"
#include "AgcmChatting2.h"
#include "AgcmUIConsole.h"
#include "AgcmMinimap.h"
#include "AgcmUILogin.h"			// for UI Text(class description on board)


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGCMUIWANTEDCRIMINAL_MESSAGE
	{
	AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE = 0,
	AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE2,
	AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE3,
	AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE_CANCELED,
	AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE_SUCCEEDED,
	AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_BASE_MONEY,
	AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_MONEY,
	AGCMUIWANTEDCRIMINAL_MESSAGE_ALREADY_ARRANGED,
	AGCMUIWANTEDCRIMINAL_MESSAGE_RANK_TOOLTIP1,
	AGCMUIWANTEDCRIMINAL_MESSAGE_RANK_TOOLTIP2,
	AGCMUIWANTEDCRIMINAL_MESSAGE_KILL_WC,
	AGCMUIWANTEDCRIMINAL_MESSAGE_DEAD_WC,
	AGCMUIWANTEDCRIMINAL_MESSAGE_UNDER_BASE_MONEY,
	AGCMUIWANTEDCRIMINAL_MESSAGE_MAX
	};


const enum eAGCMUIWANTEDCRIMINAL_EVENT
	{
	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE1 = 0,
	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE2,
	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE3,
	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_RANKING_BOARD,
	AGCMUIWANTEDCRIMINAL_EVENT_CLOSE_RANKING_BOARD,
	AGCMUIWANTEDCRIMINAL_EVENT_CHAR_ONLINE,
	AGCMUIWANTEDCRIMINAL_EVENT_CHAR_OFFLINE,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_01,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_02,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_03,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_04,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_05,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_06,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_07,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_08,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_09,
	AGCMUIWANTEDCRIMINAL_EVENT_PAGE_10,
	AGCMUIWANTEDCRIMINAL_EVENT_MAX
	};


const UINT32	AGCMUIWANTEDCRIMINAL_UPDATE_RANK_INTERVAL	= (1000 * 30);		// 30 seconds
#define			AGCMUIWANTEDCRIMINAL_ALARM_ICON				"WantedAlarm"




/************************************************************/
/*		The Definition of AgcmUIWantedCriminal class		*/
/************************************************************/
//
class AgcmUIWantedCriminal : public ApModule
	{
	private:
		//	Related modules
		AgpmGrid			*m_pAgpmGrid;
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmWantedCriminal	*m_pAgpmWantedCriminal;
		AgcmCharacter		*m_pAgcmCharacter;
		AgcmUIManager2		*m_pAgcmUIManager2;
		AgcmUIControl		*m_pAgcmUIControl;
		AgcmUIMain			*m_pAgcmUIMain;
		AgcmEventManager	*m_pAgcmEventManager;
		AgcmChatting2		*m_pAgcmChatting2;

		//	Message ID
		static TCHAR	s_szMessage[AGCMUIWANTEDCRIMINAL_MESSAGE_MAX][30];
		
		//	Event
		INT32			m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_MAX];
		static TCHAR	s_szEvent[AGCMUIWANTEDCRIMINAL_EVENT_MAX][30];
		
		//	User Data
		AgcdUIUserData	*m_pstDummy;
		AgcdUIUserData	*m_pstBoard;
		INT32			m_lBoard[AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE];
		AgcdUIUserData	*m_pstArrangeEnable;
		INT32			m_lArrageEnable;

		//	Arrange
		INT64				m_llBounty;		// bounty(money) for arrange
		//	Board
		AuPOS				m_stOpenPos;
		ApSafeArray<AgpdWantedCriminal, AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE>	m_WantedCriminals;	// board item
		INT32				m_lPage;		// board page
		INT32				m_lList;		// list count of page
		//	Ranking
		INT32				m_lRank;			// my ranking of all wanted criminals
		INT64				m_llMyBounty;		// money for whom kills me
		UINT32				m_ulPreviousClock;	// for rank update
		AgpdGridItem		*m_pAgpdGridItemAlarm;


	public:
		AgcmUIWantedCriminal();
		virtual ~AgcmUIWantedCriminal();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnIdle(UINT32 ulClockCount);
		
		//	Operation
		BOOL	OpenBoard();
		BOOL	OpenArrange();
		BOOL	OnClickPage(INT32 lPage, BOOL bRefresh = FALSE);
		
	  #ifdef _DEBUG
		void	TestOpenBoard();
		void	TestOpenArrange();
	  #endif
		
		//	Packet Send
		BOOL	SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent);
		BOOL	SendArrange(AgpdCharacter *pAgpdCharacter, TCHAR *pszCharID, INT64 llMoney);
		BOOL	SendRequstList(AgpdCharacter *pAgpdCharacter, INT32 lPage);
		BOOL	SendRequestMyRank(AgpdCharacter *pAgpdCharacter);

		//	Module Callback
		static BOOL	CBResultArrange(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBResultList(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBResultMyRank(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBUpdateKiller(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBUpdateFlag(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBNotify(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBReleaseCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Event Callback
		static BOOL	CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Main UI callback
		static BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	UI Function Callback
		//	Arrange
		static BOOL	CBClickArrangeOnMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickArrange1OK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickArrange2OK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickArrange3OK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickArrangeCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		//	Board
		static BOOL	CBClickBoardLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage01(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage02(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage03(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage04(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage05(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage06(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage07(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage08(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage09(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardPage10(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardRefresh(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBoardClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		
		//	Display Callback
		//	Arrange
		static BOOL	CBDisplayArrangeDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayArrangeDesc2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayArrangeDesc3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayArrangeCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayArrangeTotalGheld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayArrangeBounty(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		//	Board
		static BOOL	CBDisplayBoardRank(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayBoardWanted(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayBoardBounty(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayBoardLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayBoardClass(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		
		//	Boolean Callback
		static BOOL CBIsActiveArrange(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	
	protected :
		//	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		
		//	helper
		TCHAR*	GetMessageTxt(eAGCMUIWANTEDCRIMINAL_MESSAGE eMessage);
		void	AddSystemMessage(TCHAR *pszMessage);
		void	ResetBoard();
	};


#endif