/*=============================================================

	AgcmUICooldown.h

=============================================================*/

#ifndef _AGCM_UI_COOLDOWN_H_
	#define _AGCM_UI_COOLDOWN_H_

#include "AgcModule.h"
#include "AgcmUIManager2.h"
//#include "AgcmCharacter.h"
#include "AgcmUIControl.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGCMUICOOLDOWN_EVENT
	{
	AGCMUICOOLDOWN_EVENT_OPEN = 0,
	AGCMUICOOLDOWN_EVENT_CLOSE,
	AGCMUICOOLDOWN_EVENT_SOUND_ITEM,
	AGCMUICOOLDOWN_EVENT_SOUND_QUSET,
	AGCMUICOOLDOWN_EVENT_SOUND_SKILL,
	AGCMUICOOLDOWN_EVENT_MAX,
	};

enum eAGCMUICOOLDOWN_DISPLAY_ID
	{
	AGCMUICOOLDOWN_DISPLAY_ID_TITLE = 0,
	AGCMUICOOLDOWN_DISPLAY_ID_BAR_CUR,
	AGCMUICOOLDOWN_DISPLAY_ID_BAR_MAX,
	};

#define AGCMUICOOLDOWN_TIMESLICE			300			// ms	
#define AGCMUICOOLDOWN_MAX					50
#define AGCMUICOOLDOWN_MAX_TITLE_LENGTH		64
#define AGCMUICOOLDOWN_INVALID_ID			(-1)

struct AgcdUICooldown
	{
	//public:
		INT32						m_lID;
		PVOID						m_pClass;
		UINT32						m_ulDelay;		// cooldown time
		CHAR						m_szTitle[AGCMUICOOLDOWN_MAX_TITLE_LENGTH + 1];
		UINT32						m_ulStart;		// start time
		ApModuleDefaultCallBack		m_pCallback;	// after cooldown callback
		BOOL						m_bNeedUI;		// 
		PVOID						m_pvCustom;		// custom data ptr.
		
		void Init()
			{
			m_pClass = NULL;
			m_ulDelay = 0;
			memset(m_szTitle, 0, sizeof(m_szTitle));
			m_ulStart = 0;
			m_pCallback = NULL;
			m_bNeedUI = FALSE;
			m_pvCustom = NULL;
			}

	};

/****************************************************/
/*		The Definition of AgcmUICooldown class		*/
/****************************************************/
//
class AgcmUICooldown : public AgcModule
	{
	public:
		//	Event
		static CHAR s_szEvents[AGCMUICOOLDOWN_EVENT_MAX][30];
	
	private:
		//	Related modules
		AgpmCharacter			*m_pAgpmCharacter;
//		AgcmCharacter			*m_pAgcmCharacter;
		AgcmUIManager2			*m_pAgcmUIManager2;
		AgcmUIControl			*m_pAgcmUIControl;

		//	UI events
		ApSafeArray<INT32, AGCMUICOOLDOWN_EVENT_MAX>	m_lEvents;

		//	User data
		AgcdUIUserData			*m_pUDDummy;
		
		//	Cooldown data
		INT32					m_lCount;
		INT32					m_lCurrentID;
		AgcdUICooldown			m_Cooldowns[AGCMUICOOLDOWN_MAX];
		
		// Bar data
		UINT32					m_ulElapsed;
		
	protected:
		BOOL AddEvent();
		BOOL AddFunction();
		BOOL AddDisplay();
		BOOL AddUserData();

		void ProcessCooldown(UINT32 ulClockCount);
		void OnEnd();
		
	public:
		AgcmUICooldown();
		virtual ~AgcmUICooldown();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		BOOL	OnIdle(UINT32 ulClockCount);

		//	Operation
		INT32	RegisterCooldown(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	StartCooldown(INT32 lID, UINT32 ulDelay, CHAR *pszTitle = NULL, BOOL bNeedUI = TRUE, PVOID pvCustom = NULL, eAGCMUICOOLDOWN_EVENT pSoundType = AGCMUICOOLDOWN_EVENT_OPEN);
		void	CancelCooldown(INT32 lID);

		//	Functions
		static BOOL CBOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Display callbacks
		static BOOL	CBDisplayTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayProgress(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		//static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
