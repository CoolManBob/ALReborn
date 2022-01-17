/*==============================================================

	AgcmUICooldown.cpp

==============================================================*/

#include "AgcmUICooldown.h"


CHAR AgcmUICooldown::s_szEvents[AGCMUICOOLDOWN_EVENT_MAX][30] = 
	{
	"UI_COOLDOWN_OPEN",
	"UI_COOLDOWN_CLOSE",
	"UI_COOLDOWN_ITEM",
	"UI_COOLDOWN_QUEST",
	"UI_COOLDOWN_SKILL"
	};

/********************************************************/
/*		The Implementation of AgcmUICooldown class		*/
/********************************************************/
//
AgcmUICooldown::AgcmUICooldown()
	{
	SetModuleName("AgcmUICooldown");
	EnableIdle(TRUE);

	m_pAgpmCharacter	= NULL;
//	m_pAgcmCharacter	= NULL;
	m_pAgcmUIManager2	= NULL;
	m_pAgcmUIControl	= NULL;

	// user data
	m_pUDDummy			= NULL;
	
	m_lEvents.MemSetAll();
	m_lCount = 0;
	m_lCurrentID = AGCMUICOOLDOWN_INVALID_ID;	

	for (INT32 i=0; i<5; ++i)
		m_Cooldowns[i].Init();
	
	m_ulElapsed = 0;
	}

AgcmUICooldown::~AgcmUICooldown()
	{
	}


//	ApModule inherited
//===============================================
//
BOOL AgcmUICooldown::OnInit()
	{
	return TRUE;
	}

BOOL AgcmUICooldown::OnDestroy()
	{
	return TRUE;
	}

BOOL AgcmUICooldown::OnAddModule()
	{
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
//	m_pAgcmCharacter	= (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmUIManager2 = (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	m_pAgcmUIControl	= (AgcmUIControl *) GetModule("AgcmUIControl");

	if (!m_pAgpmCharacter ||
//		!m_pAgcmCharacter ||
		!m_pAgcmUIManager2 || !m_pAgcmUIControl
		)
		return FALSE;
	
	if (!AddEvent() ||
		!AddFunction() ||
		!AddDisplay() ||
		!AddUserData()
		)
		return FALSE;


	return TRUE;
	}

BOOL AgcmUICooldown::OnIdle(UINT32 ulClockCount)
	{
		PROFILE("AgcmUICoolDown::OnIdle");

		ProcessCooldown(ulClockCount);	
		return TRUE;
	}

BOOL AgcmUICooldown::AddEvent()
	{
	for (INT32 i=0; i < AGCMUICOOLDOWN_EVENT_MAX; ++i)
		{
		m_lEvents[i] = m_pAgcmUIManager2->AddEvent(s_szEvents[i]);
		if (m_lEvents[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}

BOOL AgcmUICooldown::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, "COOLDOWN_OPEN", CBOpen, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "COOLDOWN_CLOSE", CBClose, 0))
		return FALSE;

	return TRUE;
	}

BOOL AgcmUICooldown::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, "COOLDOWN_DISP_TITLE", AGCMUICOOLDOWN_DISPLAY_ID_TITLE, CBDisplayTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COOLDOWN_DISP_BARCUR", AGCMUICOOLDOWN_DISPLAY_ID_BAR_CUR, CBDisplayProgress, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COOLDOWN_DISP_BARMAX", AGCMUICOOLDOWN_DISPLAY_ID_BAR_MAX, CBDisplayProgress, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}

BOOL AgcmUICooldown::AddUserData()
	{
	m_pUDDummy = m_pAgcmUIManager2->AddUserData("COOLDOWN_UD_DUMMY", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pUDDummy)
		return FALSE;

	return TRUE;
	}


//	Operation
//======================================================
//
INT32 AgcmUICooldown::RegisterCooldown(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	if (m_lCount >= AGCMUICOOLDOWN_MAX)
		return -1;
	
	INT32 i = m_lCount;	
	m_Cooldowns[i].m_ulStart = 0;
	m_Cooldowns[i].m_pClass = pClass;
	m_Cooldowns[i].m_pCallback = pfCallback;

	m_lCount++;
	return i;
	}
	
BOOL AgcmUICooldown::StartCooldown(INT32 lID, UINT32 ulDelay, CHAR *pszTitle, BOOL bNeedUI, PVOID pvCustom, eAGCMUICOOLDOWN_EVENT pSoundType)
	{
	if (AGCMUICOOLDOWN_INVALID_ID >= lID || lID >= m_lCount)
		return FALSE;
	
	if (AGCMUICOOLDOWN_INVALID_ID != m_lCurrentID)
		return FALSE;
		
	m_Cooldowns[lID].m_ulDelay = ulDelay;
	m_Cooldowns[lID].m_bNeedUI = bNeedUI;
	if (pszTitle)
		strncpy(m_Cooldowns[lID].m_szTitle, pszTitle, AGCMUICOOLDOWN_MAX_TITLE_LENGTH);
	m_Cooldowns[lID].m_pvCustom = pvCustom;
	m_Cooldowns[lID].m_ulStart = 0;//GetClockCount();
	
	m_ulElapsed = 0;

	m_lCurrentID = lID;
	if (m_Cooldowns[lID].m_bNeedUI)
	{
		m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUICOOLDOWN_EVENT_OPEN]);
	}
	if( pSoundType > 1)
	{
		m_pAgcmUIManager2->ThrowEvent(m_lEvents[pSoundType]);
	}
		
	return TRUE;
	}
	
void AgcmUICooldown::CancelCooldown(INT32 lID)
	{
	if (AGCMUICOOLDOWN_INVALID_ID == m_lCurrentID)
		return;
		
	if (m_lCurrentID != lID)
		return;
	
	if (m_Cooldowns[m_lCurrentID].m_bNeedUI)
		m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUICOOLDOWN_EVENT_CLOSE]);

	m_lCurrentID = AGCMUICOOLDOWN_INVALID_ID;
	}

void AgcmUICooldown::ProcessCooldown(UINT32 ulClockCount)
	{
	if (AGCMUICOOLDOWN_INVALID_ID == m_lCurrentID)
		return;

	if (0 == m_Cooldowns[m_lCurrentID].m_ulStart)
		{
		m_ulElapsed = 0;
		m_Cooldowns[m_lCurrentID].m_ulStart = ulClockCount;
		}
	else
		m_ulElapsed = ulClockCount - m_Cooldowns[m_lCurrentID].m_ulStart;
	if (m_ulElapsed >= m_Cooldowns[m_lCurrentID].m_ulDelay)
		{
		m_ulElapsed = m_Cooldowns[m_lCurrentID].m_ulDelay;
		// refresh
		if (m_Cooldowns[m_lCurrentID].m_bNeedUI)
			{
			m_pAgcmUIManager2->SetUserDataRefresh(m_pUDDummy);
			m_pAgcmUIManager2->RefreshUserData(m_pUDDummy);
			}
		OnEnd();
		}
	
	// refresh
	if (m_Cooldowns[m_lCurrentID].m_bNeedUI)
		{
		m_pAgcmUIManager2->SetUserDataRefresh(m_pUDDummy);
		m_pAgcmUIManager2->RefreshUserData(m_pUDDummy);
		}
	}

void AgcmUICooldown::OnEnd()
	{
	INT32 i = m_lCurrentID;
	m_lCurrentID = AGCMUICOOLDOWN_INVALID_ID;

	if (m_Cooldowns[i].m_bNeedUI)
		m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUICOOLDOWN_EVENT_CLOSE]);
	
	if (m_Cooldowns[i].m_pCallback)
		(m_Cooldowns[i].m_pCallback)((PVOID) i , m_Cooldowns[i].m_pClass, m_Cooldowns[i].m_pvCustom);
	
	m_Cooldowns[i].m_ulStart = 0;
	}

//	Functions
//=========================================================
//
BOOL AgcmUICooldown::CBOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	AgcmUICooldown	*pThis = (AgcmUICooldown *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;

	pThis->m_pAgcmUIManager2->RefreshUserData(pThis->m_pUDDummy);

	return TRUE;
	}

BOOL AgcmUICooldown::CBClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICooldown	*pThis = (AgcmUICooldown *) pClass;
	
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUICOOLDOWN_EVENT_CLOSE]);

	return TRUE;
	}


//	Display callbacks
//====================================================
//
BOOL AgcmUICooldown::CBDisplayTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pData || !pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUICooldown *pThis = (AgcmUICooldown *) pClass;

	// 현재 진행중인 넘을 찾아 그넘의 타이틀을 뿌린다.
	if (AGCMUICOOLDOWN_INVALID_ID == pThis->m_lCurrentID)
		return FALSE;
		
	strcpy(szDisplay, pThis->m_Cooldowns[pThis->m_lCurrentID].m_szTitle);
	
	return TRUE;	
	}

BOOL AgcmUICooldown::CBDisplayProgress(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !szDisplay || !plValue)
		return FALSE;

	AgcmUICooldown		*pThis = (AgcmUICooldown *) pClass;

	// 현재 진행중인 넘을 찾아 그넘의 진행상태를 뿌린다.
	if (AGCMUICOOLDOWN_INVALID_ID == pThis->m_lCurrentID)
		return FALSE;
		
	switch (lID)
		{	
		case AGCMUICOOLDOWN_DISPLAY_ID_BAR_CUR:
			*plValue = (INT32) pThis->m_ulElapsed;
			sprintf(szDisplay, "%d", *plValue);
			break;

		case AGCMUICOOLDOWN_DISPLAY_ID_BAR_MAX:
			*plValue = (INT32) pThis->m_Cooldowns[pThis->m_lCurrentID].m_ulDelay;
			sprintf(szDisplay, "%d", *plValue);
			break;

		default:
			break;
		}	

	return TRUE;
	}

/*
BOOL AgcmUICooldown::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUICooldown *pThis = (AgcmUICooldown *) pClass;
	
	pThis->m_bIsWindowOpen = FALSE;
	pThis->m_bHoldOn = FALSE;

	return TRUE;
	}
*/

