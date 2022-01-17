/*===========================================================================

	AgcmUIRemission.h
	
===========================================================================*/

#ifndef _AGCM_UI_REMISSION_H_
	#define _AGCM_UI_REMISSION_H_

#include "AgcModule.h"
#include "AgpmRemission.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmUIMain.h"
#include "AgcmEventManager.h"




/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGCMUIREMISSION_EVENT
	{
	AGCMUIREMISSION_EVENT_OPEN = 0,
	AGCMUIREMISSION_EVENT_CLOSE,
	AGCMUIREMISSION_EVENT_SUCCESS,
	AGCMUIREMISSION_EVENT_MAX
	};


const enum eAGCMUIREMISSION_MESSAGE
	{
	AGCMUIREMISSION_MESSAGE_NONE = 0,
	AGCMUIREMISSION_MESSAGE_DESCRIPTION,
	AGCMUIREMISSION_MESSAGE_CURRENT_POINT,
	AGCMUIREMISSION_MESSAGE_REMITABLE_POINT,
	AGCMUIREMISSION_MESSAGE_INDUL_COUNT,

	AGCMUIREMISSION_MESSAGE_NOT_TIME_POINT,
	AGCMUIREMISSION_MESSAGE_CONFIRM_INDUL,
	AGCMUIREMISSION_MESSAGE_SUCCESS,
	
	AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV1,
	AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV2,
	AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV3,
	
	AGCMUIREMISSION_MESSAGE_NEED_INDUL_COUNT,
	
	AGCMUIREMISSION_MESSAGE_MAX,
	};




/****************************************************/
/*		The Definition of AgcmUIRemission class		*/
/****************************************************/
//
class AgcmUIRemission : public AgcModule
	{
	private :
		AgpmRemission	*m_pAgpmRemission;
		AgpmCharacter	*m_pAgpmCharacter;
		
		AgcmCharacter	*m_pAgcmCharacter;
		AgcmUIManager2	*m_pAgcmUIManager2;
		AgcmUIControl	*m_pAgcmUIControl;
		AgcmUIMain		*m_pAgcmUIMain;
		AgcmUICooldown	*m_pAgcmUICooldown;
		AgcmEventManager	*m_pAgcmEventManager;
		
		// Selected remission type
		INT8			m_cType;
	
		// UI position
		AuPOS			m_stOpenPos;
		
		// Event
		INT32			m_lEvent[AGCMUIREMISSION_EVENT_MAX];
		static CHAR		s_szEvent[AGCMUIREMISSION_EVENT_MAX][30];
	
		// User data
		AgcdUIUserData	*m_pstDummy;
		AgcdUIUserData	*m_pstIndulEnable;
		INT32			m_lIndumEnable;

		// Message key
		static CHAR		s_szMessage[AGCMUIREMISSION_MESSAGE_MAX][30];
		
		// ETC
		INT32						m_lCooldownID;			// cooldown(progress bar)
		INT32						m_lOldLevel;

	public :
		AgcmUIRemission();
		virtual ~AgcmUIRemission();
		
		//	ApModule inherited
		BOOL	OnAddModule();
		
		//	Window
		BOOL	Open();
		void	Close();
		
		//	Result
		BOOL	OnTest(INT32 lResult);
		BOOL	OnResult(AgpdRemitArg *pArg);
		
		//	Event callback
		static BOOL	CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Result callback
		static BOOL	CBRemit(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Main UI callback
		static BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	UI Function callback
		static BOOL	CBOpenWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickButtonIndul(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Cooldown callback
		static BOOL CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Display callback
		static BOOL	CBDisplayDescription(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayCurrentPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayRemitablePoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayIndulCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayNeedIndulCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		
		//	Boolean callback
		static BOOL CBIsActiveIndulButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

		//	Packet
		BOOL	SendRemit(AgpdCharacter *pAgpdCharacter, INT8 cType);
		BOOL	SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent);

	protected :
		//	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		BOOL	AddBoolean();
		
		//	helper
		CHAR*	GetMessageTxt(eAGCMUIREMISSION_MESSAGE eMessage);
	};


#endif