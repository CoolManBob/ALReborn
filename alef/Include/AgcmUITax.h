/*====================================================================

	AgcmUITax.h
	
====================================================================*/


#ifndef _AGCM_UI_TAX_H_
	#define _AGCM_UI_TAX_H_


#include "AgpmTax.h"
#include "AgpmGrid.h"
#include "AgpmSiegeWar.h"
#include "AgpmArchlord.h"
#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmUIMain.h"
#include "AgcmEventManager.h"
#include "AgcmChatting2.h"
#include "AgcmUIConsole.h"
#include "AgcmMinimap.h"
#include "AgcmUICharacter.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGCMUITAX_MESSAGE
	{
	AGCMUITAX_MESSAGE_STEWARD_INTRO = 0,
	AGCMUITAX_MESSAGE_MODIFY_CAPTION,
	AGCMUITAX_MESSAGE_VIEW_CAPTION,	
	AGCMUITAX_MESSAGE_MODIFY_RATIO_CONFIRM_REG,
	AGCMUITAX_MESSAGE_MODIFY_RATIO_CONFIRM_QUE,
	AGCMUITAX_MESSAGE_MODIFY_RATIO_SUCCESS,
	AGCMUITAX_MESSAGE_MODIFY_RATIO_FAIL_NOT_OWNER,
	AGCMUITAX_MESSAGE_MODIFY_RATIO_FAIL_NOT_TIME,
	AGCMUITAX_MESSAGE_MODIFY_RATIO_EXCEED_RANGE,
	AGCMUITAX_MESSAGE_MAX
	};


const enum eAGCMUITAX_EVENT
	{
	AGCMUITAX_EVENT_OPEN_STEWARD = 0,
	AGCMUITAX_EVENT_CLOSE_STEWARD,
	AGCMUITAX_EVENT_OPEN_RATIO,
	AGCMUITAX_EVENT_CLOSE_RATIO,
	AGCMUITAX_EVENT_OPEN_INCOME,
	AGCMUITAX_EVENT_CLOSE_INCOME,
	AGCMUITAX_EVENT_MAX
	};




/************************************************/
/*		The Definition of AgcmUITax class		*/
/************************************************/
//
class AgcmUITax : public ApModule
	{
	private:
		//	Related modules
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmTax				*m_pAgpmTax;
		AgpmSiegeWar		*m_pAgpmSiegeWar;
		AgpmArchlord		*m_pAgpmArchlord;
		AgcmCharacter		*m_pAgcmCharacter;
		AgcmUIManager2		*m_pAgcmUIManager2;
		AgcmUIControl		*m_pAgcmUIControl;
		AgcmUIMain			*m_pAgcmUIMain;
		AgcmEventManager	*m_pAgcmEventManager;
		AgcmChatting2		*m_pAgcmChatting2;
		AgcmUICharacter		*m_pAgcmUICharacter;

		//	Message ID
		static TCHAR	s_szMessage[AGCMUITAX_MESSAGE_MAX][30];
		
		//	Event
		INT32			m_lEvent[AGCMUITAX_EVENT_MAX];
		static TCHAR	s_szEvent[AGCMUITAX_EVENT_MAX][30];
		
		//	User Data
		AgcdUIUserData	*m_pstSteward;
		AgcdUIUserData	*m_pstModifyRatio;
		AgcdUIUserData	*m_pstViewIncome;
		
		//	UI Open Position
		AuPOS			m_stOpenPos;
		
		BOOL			m_bModifyWindow;

		INT32			m_lEventNPCID;

	public:
		AgcmUITax();
		virtual ~AgcmUITax();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		
		//	UI
		BOOL	OpenSteward();
		BOOL	OpenModifyRatio();
		BOOL	RequestIncome();
		BOOL	OpenViewIncome();
		void	CloseAll();
		
		//	Tax Data
		AgpdTax*		GetTax();
		AgpdRegionTax*	GetRegionTax(INT32 lIndex);
		BOOL			UpdateTax(AgpdTax *pAgpdTax);
		
	  #ifdef _DEBUG
		void	TestOpenSteward();
		void	TestOpenModifyRatio();
		void	TestRequestIncome();
	  #endif
		
		//	Packet Send
		BOOL	SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent);
		BOOL	SendModifyRatio(AgpdCharacter *pAgpdCharacter, AgpdTax *pAgpdTax);
		BOOL	SendViewIncome(AgpdCharacter *pAgpdCharacter, TCHAR *pszCastle);

		//	Module Callback
		static BOOL	CBResultModifyRatio(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBResultInquireTax(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBReleaseCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Event Callback
		static BOOL	CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Main UI callback
		static BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	UI Function Callback
		//	Steward
		static BOOL	CBClickModifyRatio(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickViewIncome(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		//	Modify Tax Ratio
		static BOOL	CBClickReset(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickBack(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Display Callback
		//	Steward
		static BOOL	CBDisplayStewardIntro(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		//	Common
		static BOOL	CBDisplayCaption(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRegion1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRegion2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRegion3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRegion4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRegion5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		//	Modify Ratio
		static BOOL	CBDisplayModifyRatio1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayModifyRatio2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayModifyRatio3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayModifyRatio4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		//	View Income
		static BOOL	CBDisplayViewRatio1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayViewRatio2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayViewRatio3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayViewRatio4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayViewRatio5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayIncome1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayIncome2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayIncome3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayIncome4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayIncome5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRecentIncome(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayTotalIncome(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayLatestTransferDate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		//	Boolean Callback
		static BOOL CBIsCastleOwner(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	protected :
		//	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		
		//	helper
		TCHAR*	GetMessageTxt(eAGCMUITAX_MESSAGE eMessage);
		void	AddSystemMessage(TCHAR *pszMessage);
	};


#endif