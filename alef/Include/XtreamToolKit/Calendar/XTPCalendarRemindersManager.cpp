// XTPCalendarRemindersManager.cpp: implementation of the
// CXTPCalendarRemindersManager class.
//
// This file is a part of the XTREME CALENDAR MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPCalendarRemindersManager.h"
#include "XTPCalendarData.h"
#include "XTPCalendarNotifications.h"
#include "XTPCalendarCustomProperties.h"
#include "XTPCalendarResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
//  #define DBG_REMINDERS_TRACE TRACE
#endif

#ifndef DBG_REMINDERS_TRACE
	#define DBG_REMINDERS_TRACE
#endif

#define TIMERID_RMD_REFRESH 1
#define XTP_CALENDAR_RMD_REFRESH_TIMEOUT 3*1000

#define TIMERID_RMD_NOTIFY_CHANGED 2
#define XTP_CALENDAR_RMD_NOTIFY_CHANGED_TIMEOUT 500

static LPCTSTR XTP_CALENDAR_REMINDERS_MANAGER_WND_NAME = _T("XTPCalendarRemindersManagerWnd");

////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarReminder, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPCalendarRemindersManager, CCmdTarget)

CXTPCalendarReminder::CXTPCalendarReminder(CXTPCalendarRemindersManager* pOwnerMan)
{
	m_pOwnerMan = pOwnerMan;
	m_pEventDataProvider = NULL;

	m_dtEventStartTime = (DATE)0;
	m_dtNextReminderTime = (DATE)0;
	m_nMinutesBeforeStart = 0;

	m_eEventType = evtNormal;
	::ZeroMemory(&m_RecurrenceEventInfo, sizeof(m_RecurrenceEventInfo));
	m_dwNormalEventID = XTP_CALENDAR_UNKNOWN_EVENT_ID;

}

CXTPCalendarReminder::~CXTPCalendarReminder()
{
	CMDTARGET_RELEASE(m_pEventDataProvider);
}

CXTPCalendarEventPtr CXTPCalendarReminder::GetEvent()
{
	if (!m_pOwnerMan || !m_pEventDataProvider)
	{
		ASSERT(FALSE);
		return NULL;
	}

	if (m_eEventType == evtNormal)
	{
		return m_pEventDataProvider->GetEvent(m_dwNormalEventID);
	}
	if (m_eEventType != evtRecurrence)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarEventPtr ptrMaster = m_pEventDataProvider->GetEvent(m_RecurrenceEventInfo.m_dwMasterEventID);
	CXTPCalendarRecurrencePatternPtr ptrPattern = ptrMaster ? ptrMaster->GetRecurrencePattern() : NULL;

	if (!ptrMaster || !ptrPattern)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarEvent* pExc = ptrPattern->FindException(m_RecurrenceEventInfo.m_dtOccurrenceStartTime,
									m_RecurrenceEventInfo.m_dtOccurrenceEndTime);
	if (pExc)
	{
		return pExc->CloneEvent();
	}

	return ptrMaster->CloneForOccurrence(m_RecurrenceEventInfo.m_dtOccurrenceStartTime,
										 m_RecurrenceEventInfo.m_dtOccurrenceEndTime);
}

void CXTPCalendarReminder::SetEvent(CXTPCalendarEvent* pEvent)
{
	if (!pEvent || !pEvent->GetDataProvider())
	{
		ASSERT(FALSE);
		return;
	}

	// data provider
	CMDTARGET_RELEASE(m_pEventDataProvider);

	m_pEventDataProvider = pEvent->GetDataProvider();
	CMDTARGET_ADDREF(m_pEventDataProvider);

	//-----
	m_dtEventStartTime = pEvent->GetStartTime();

	int nRState = pEvent->GetRecurrenceState();
	if (nRState == xtpCalendarRecurrenceNotRecurring)
	{
		m_eEventType = evtNormal;
		m_dwNormalEventID = pEvent->GetEventID();
		return;
	}

	CXTPCalendarRecurrencePatternPtr ptrPattern = pEvent->GetRecurrencePattern();

	if (nRState != xtpCalendarRecurrenceOccurrence &&
		nRState != xtpCalendarRecurrenceException || !ptrPattern)
	{
		ASSERT(FALSE);
		return;
	}

	m_eEventType = evtRecurrence;
	m_RecurrenceEventInfo.m_dwMasterEventID = ptrPattern->GetMasterEventID();

	if (nRState == xtpCalendarRecurrenceOccurrence)
	{
		m_RecurrenceEventInfo.m_dtOccurrenceStartTime = pEvent->GetStartTime();
		m_RecurrenceEventInfo.m_dtOccurrenceEndTime = pEvent->GetEndTime();
	}
	else if (nRState == xtpCalendarRecurrenceException)
	{
		m_RecurrenceEventInfo.m_dtOccurrenceStartTime = pEvent->GetRException_StartTimeOrig();
		m_RecurrenceEventInfo.m_dtOccurrenceEndTime = pEvent->GetRException_EndTimeOrig();
	}
	else
	{
		ASSERT(FALSE);
	}
}

BOOL CXTPCalendarReminder::IsEqualID(const CXTPCalendarReminder* pR2) const
{
	ASSERT(pR2);
	if (!pR2)
	{
		return FALSE;
	}

	if (m_pEventDataProvider != pR2->m_pEventDataProvider)
	{
		return FALSE;
	}
	if (m_eEventType != pR2->m_eEventType)
	{
		return FALSE;
	}
	if (m_eEventType == evtNormal)
	{
		return m_dwNormalEventID == pR2->m_dwNormalEventID;
	}
	if (m_eEventType != evtRecurrence)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if (m_RecurrenceEventInfo.m_dwMasterEventID != pR2->m_RecurrenceEventInfo.m_dwMasterEventID)
	{
		return FALSE;
	}
	if (!CXTPCalendarUtils::IsEqual(m_RecurrenceEventInfo.m_dtOccurrenceStartTime,
		pR2->m_RecurrenceEventInfo.m_dtOccurrenceStartTime) )
	{
		return FALSE;
	}

	if (!CXTPCalendarUtils::IsEqual(m_RecurrenceEventInfo.m_dtOccurrenceEndTime,
		pR2->m_RecurrenceEventInfo.m_dtOccurrenceEndTime) )
	{
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
int CXTPCalendarReminders::Find(const CXTPCalendarReminder* pReminder) const
{
	if (!pReminder)
	{
		ASSERT(FALSE);
		return -1;
	}

	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarReminder* pRmdI = GetAt(i, FALSE);
		ASSERT(pRmdI);

		if (pRmdI && pRmdI->IsEqualID(pReminder))
		{
			return i;
		}
	}
	return -1;
}

int CXTPCalendarReminders::Find(CXTPCalendarEvent* pEvent) const
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return -1;
	}

	CXTPCalendarReminder eventRmd;
	eventRmd.SetEvent(pEvent);

	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarReminder* pRmdI = GetAt(i, FALSE);
		ASSERT(pRmdI);

		if (pRmdI && eventRmd.IsEqualID(pRmdI))
		{
			return i;
		}
	}
	return -1;
}

int CXTPCalendarReminders::Find(DWORD dwEventID) const
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarReminder* pRmdI = GetAt(i, FALSE);
		ASSERT(pRmdI);
		if (!pRmdI)
		{
			continue;
		}

		if (pRmdI->m_eEventType == CXTPCalendarReminder::evtNormal &&
			pRmdI->m_dwNormalEventID == dwEventID
			||
			pRmdI->m_eEventType == CXTPCalendarReminder::evtRecurrence &&
			pRmdI->m_RecurrenceEventInfo.m_dwMasterEventID == dwEventID)
		{
			return i;
		}
	}
	return -1;
}

BOOL CXTPCalendarReminders::RemoveDataForEvent(CXTPCalendarEvent* pEvent)
{
	ASSERT(pEvent);
	if (!pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	BOOL bChanged = FALSE;

	if (pEvent->GetRecurrenceState() == xtpCalendarRecurrenceMaster)
	{
		bChanged = _RemoveDataForMasterEvent(pEvent);
	}
	else
	{
		int nFIdx = Find(pEvent);
		if (nFIdx >= 0)
		{
			RemoveAt(nFIdx);
			bChanged = TRUE;

			ASSERT(Find(pEvent) < 0);
		}
	}

	//---------------------------------
	int nFIdx = Find(pEvent->GetEventID());
	if (nFIdx >= 0)
	{
		RemoveAt(nFIdx);
		bChanged = TRUE;
	}
	return bChanged;
}

void CXTPCalendarReminders::Sort()
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		for (int j = i+1; j < nCount; j++)
		{
			TObjectPtr ptrE1 = GetAt(i, TRUE);
			TObjectPtr ptrE2 = GetAt(j, TRUE);

			if (ptrE1->m_dtEventStartTime > ptrE2->m_dtEventStartTime)
			{
				SetAt(j, ptrE1.Detach());
				SetAt(i, ptrE2.Detach());
			}
		}
	}
}

BOOL CXTPCalendarReminders::_RemoveDataForMasterEvent(CXTPCalendarEvent* pEvent)
{
	if (!pEvent || pEvent->GetRecurrenceState() != xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	BOOL bChanged = FALSE;

	DWORD dwMasterEventID = pEvent->GetEventID();

	int nCount = GetCount();
	for (int i = nCount-1; i >= 0 ; i--)
	{
		CXTPCalendarReminder* pRmdI = GetAt(i, FALSE);
		ASSERT(pRmdI);
		if (!pRmdI)
		{
			continue;
		}

		if (pRmdI->m_eEventType == CXTPCalendarReminder::evtRecurrence &&
			pRmdI->m_RecurrenceEventInfo.m_dwMasterEventID == dwMasterEventID)
		{
			RemoveAt(i);
			bChanged = TRUE;
		}
	}
	return bChanged;
}

////////////////////////////////////////////////////////////////////////////
CXTPCalendarRemindersManager::CXTPCalendarRemindersManager()
{
	m_dtLastUpdateTime = (DATE)0;

	m_pConnection = new CXTPNotifyConnection();
	m_pResourcesNf = new CXTPCalendarResourcesNf();

	m_bSkipOnEventChanged = FALSE;
	m_bMonitoringRunning = FALSE;

}

CXTPCalendarRemindersManager::~CXTPCalendarRemindersManager()
{
	//RemoveAll();

	CMDTARGET_RELEASE(m_pConnection);
	CMDTARGET_RELEASE(m_pResourcesNf);
}

BEGIN_MESSAGE_MAP(CXTPCalendarRemindersManager, CWnd)
	ON_WM_TIMER()
	ON_WM_TIMECHANGE()
END_MESSAGE_MAP()

void CXTPCalendarRemindersManager::OnFinalRelease()
{
	StopMonitoring();

	CWnd::OnFinalRelease();
	CCmdTarget::OnFinalRelease();
}

BOOL CXTPCalendarRemindersManager::_CreateWnd()
{
	LPCTSTR pcszSimpleWndClass = AfxRegisterWndClass(0);
	CRect rcEmpty(0,0,0,0);

	BOOL bCreated = CreateEx(0, pcszSimpleWndClass,
					XTP_CALENDAR_REMINDERS_MANAGER_WND_NAME,
					WS_POPUP, rcEmpty, NULL, 0);

	return bCreated;
}

BOOL CXTPCalendarRemindersManager::StartMonitoring(CXTPCalendarResources* pResources,
												   COleDateTimeSpan spPeriod2Cache)
{
	if (!GetSafeHwnd())
	{
		if (!_CreateWnd())
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	//***********************************
	if (IsMonitoringRunning())
	{
		StopMonitoring();
	}

	//***********************************
	RemoveAll();
	XTP_SAFE_CALL1(m_pResourcesNf, RemoveAll());
	m_Sink.UnadviseAll();

	//-----------------------------------
	ASSERT(pResources);
	ASSERT(GETTOTAL_SECONDS_DTS(spPeriod2Cache) > 0);

	if (!pResources || !m_pResourcesNf)
	{
		return FALSE;
	}

	DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::StartMonitoring(). period = %d min \n"),
						(LPCTSTR)CXTPCalendarUtils::GetCurrentTime().Format(),
						(int)spPeriod2Cache.GetTotalMinutes());

	m_pResourcesNf->Append(pResources);
	m_spPeriod2Cache = spPeriod2Cache;

	//------------------------------------
	// Advise to Data Provider notifications
	CXTPNotifyConnection* pConnRC = m_pResourcesNf->GetConnection();
	ASSERT(pConnRC);

	if (pConnRC)
	{
		m_Sink.Advise(pConnRC, XTP_NC_CALENDAREVENTWASADDED, &CXTPCalendarRemindersManager::OnEventChanged);
		m_Sink.Advise(pConnRC, XTP_NC_CALENDAREVENTWASDELETED, &CXTPCalendarRemindersManager::OnEventChanged);
		m_Sink.Advise(pConnRC, XTP_NC_CALENDAREVENTWASCHANGED, &CXTPCalendarRemindersManager::OnEventChanged);
	}
	m_pResourcesNf->ReBuildInternalData();

	//------------------------------------
	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	UpdateDataFromDP(dtNow, m_spPeriod2Cache);

	SetTimer(TIMERID_RMD_REFRESH, XTP_CALENDAR_RMD_REFRESH_TIMEOUT, NULL);

	//--------------------------------------------------------
	m_bMonitoringRunning = TRUE;
	NotifyReminders(xtpCalendarRemindersMonitoringStarted);

	return TRUE;
}

BOOL CXTPCalendarRemindersManager::StopMonitoring()
{
	if (IsWindow(m_hWnd))
	{
		KillTimer(TIMERID_RMD_REFRESH);
	}

	m_Sink.UnadviseAll();

	RemoveAll();

	m_bMonitoringRunning = FALSE;
	NotifyReminders(xtpCalendarRemindersMonitoringStopped);

	DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::STOP Monitoring().  \n"),
						CXTPCalendarUtils::GetCurrentTime().Format());

	return TRUE;
}

void CXTPCalendarRemindersManager::RemoveAll()
{
	m_arWaitingReminders.RemoveAll();
	m_arActiveReminders.RemoveAll();
}

BOOL CXTPCalendarRemindersManager::UpdateDataFromDP(COleDateTime dtFrom,
													COleDateTimeSpan spPeriod)
{
	ASSERT(m_pResourcesNf);
	if (!m_pResourcesNf || 0 == m_pResourcesNf->GetCount())
	{
		m_dtLastUpdateTime = (DATE)0;
		return FALSE;
	}

	DWORD dwTime0 = ::GetTickCount();
	DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::start retrieve UpcomingEvents().  \n"),
						CXTPCalendarUtils::GetCurrentTime().Format());

	CWaitCursor _WC;

	int nWaitingCount = m_arWaitingReminders.GetCount();
	int nActiveCount = m_arActiveReminders.GetCount();
	// ???
	m_arWaitingReminders.RemoveAll();
	m_arActiveReminders.RemoveAll();

	CXTPCalendarEventsPtr ptrEvents;
	BOOL bDataAvailable = GetUpcomingEventsAll(dtFrom, spPeriod, ptrEvents);

	if (bDataAvailable)
	{
		int nCount = ptrEvents ? ptrEvents->GetCount() : 0;
		for (int i = 0; i < nCount; i++)
		{
			CXTPCalendarEvent* pEvent = ptrEvents->GetAt(i, FALSE);
			ASSERT(pEvent);

			ProcessNewEvent(pEvent, dtFrom, spPeriod);
		}
		m_arActiveReminders.Sort();

		m_dtLastUpdateTime = dtFrom;

		int nWaitingCount2 = m_arWaitingReminders.GetCount();

		//------------------------------------
		DWORD dwTime1 = ::GetTickCount();
		DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::STOP retrieve UpcomingEvents(). time=%d ms, count = %d \n"),
			CXTPCalendarUtils::GetCurrentTime().Format(), (dwTime1 - dwTime0), nWaitingCount2);

		//------------------------------------------------------------
		return (nWaitingCount != 0 || nWaitingCount2 != 0 || nActiveCount != 0);
	}

	//===================================================
	DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::SKIP  retrieve UpcomingEvents(). *** Some Data Closed. *** \n"),
						CXTPCalendarUtils::GetCurrentTime().Format());

	m_dtLastUpdateTime = (DATE)0;
	return FALSE;
}

void CXTPCalendarRemindersManager::OnEventChanged(XTP_NOTIFY_CODE Event,
												  WPARAM /*wParam*/, LPARAM lParam)
{
	if (m_bSkipOnEventChanged)
	{
		return;
	}

	if (!m_pResourcesNf || 0 == m_pResourcesNf->GetCount())
	{
		ASSERT(FALSE);
		return;
	}

	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	BOOL bChanged = FALSE;
	BOOL bChanged2 = FALSE;
	if (Event == XTP_NC_CALENDAREVENTWASADDED)
	{
		CXTPCalendarEvent* pEvent = (CXTPCalendarEvent*)lParam;
		ProcessNewEvent(pEvent, m_dtLastUpdateTime, m_spPeriod2Cache);

		bChanged = ProcessActiveReminders(dtNow);
	}
	else if (Event == XTP_NC_CALENDAREVENTWASDELETED)
	{
		CXTPCalendarEvent* pEvent = (CXTPCalendarEvent*)lParam;

		m_arWaitingReminders.RemoveDataForEvent(pEvent);

		bChanged = m_arActiveReminders.RemoveDataForEvent(pEvent);
	}
	else if (Event == XTP_NC_CALENDAREVENTWASCHANGED)
	{
		CXTPCalendarEvent* pEvent = (CXTPCalendarEvent*)lParam;
		bChanged = ProcessChangedEvent(pEvent);

		bChanged2 = ProcessActiveReminders(dtNow);
	}
	else
	{
		ASSERT(FALSE);
	}

	if (bChanged || bChanged2)
	{
		PostNotify_RemindersFire();

		DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::OnEventChanged(). data updated. \n"),
							CXTPCalendarUtils::GetCurrentTime().Format());

	}
}

BOOL CXTPCalendarRemindersManager::ProcessNewEvent(CXTPCalendarEvent* pEvent,
							   COleDateTime dtFrom, COleDateTimeSpan spPeriod)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if (pEvent->GetRecurrenceState() == xtpCalendarRecurrenceMaster)
	{
		return _ProcessNewMasterEvent(pEvent, dtFrom, spPeriod);
	}
	return _ProcessNewSingleEvent(pEvent, dtFrom, spPeriod);
}

BOOL CXTPCalendarRemindersManager::_ProcessNewSingleEvent(CXTPCalendarEvent* pEvent,
							  COleDateTime dtFrom, COleDateTimeSpan spPeriod)
{

	if (pEvent->GetRecurrenceState() == xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!pEvent->IsReminder())
	{
		return FALSE;
	}

	int nMinBeforeStart = pEvent->GetReminderMinutesBeforeStart();
	COleDateTimeSpan spRem = CXTPCalendarUtils::Minutes2Span(nMinBeforeStart);
	COleDateTime dtNextReminderTime = pEvent->GetStartTime() - spRem;

	//---------------------------------
	COleVariant varNextReminderTime_Snoozed;
	BOOL bSnoozed = pEvent->GetCustomProperties()->GetProperty(
			cszEventCustProp_NextReminderTime_Snoozed, varNextReminderTime_Snoozed);

	if (bSnoozed)
	{
		dtNextReminderTime = (DATE)_variant_t(varNextReminderTime_Snoozed);
	}

	//---------------------------------
	COleDateTime dtEndMonitoringPeriod = dtFrom + spPeriod;

	if (dtNextReminderTime > dtEndMonitoringPeriod)
	{
		return FALSE;
	}

	//=======================================================================
	CXTPCalendarReminder* pRmdData = new CXTPCalendarReminder(this);
	if (!pRmdData)
	{
		return FALSE;
	}

	pRmdData->SetEvent(pEvent);

	pRmdData->m_nMinutesBeforeStart = nMinBeforeStart;
	pRmdData->m_dtNextReminderTime = dtNextReminderTime;

	m_arWaitingReminders.Add(pRmdData, FALSE);

	return TRUE;
}

BOOL CXTPCalendarRemindersManager::ProcessChangedEvent(CXTPCalendarEvent* pEvent)
{
	ASSERT(pEvent);
	if (!pEvent)
	{
		return FALSE;
	}

	m_arWaitingReminders.RemoveDataForEvent(pEvent);

	BOOL bActiveChanged = m_arActiveReminders.RemoveDataForEvent(pEvent);

	////BOOL bChanged2 =
	ProcessNewEvent(pEvent, m_dtLastUpdateTime, m_spPeriod2Cache);

	return bActiveChanged;// || bChanged2;
}

BOOL CXTPCalendarRemindersManager::_ProcessNewMasterEvent(CXTPCalendarEvent* pEvent,
														 COleDateTime dtFrom,
														 COleDateTimeSpan spPeriod)
{
	if (!pEvent || !pEvent->GetCustomProperties())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (pEvent->GetRecurrenceState() != xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPCalendarEvents arOccurrences;

	CXTPCalendarRecurrencePatternPtr ptrPattern = pEvent->GetRecurrencePattern();
	if (!ptrPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nMaxMinutesBefore = 0;
	BOOL bReminderExists = _GetMaxExceptionReminder(ptrPattern, nMaxMinutesBefore);

	if (pEvent->IsReminder())
	{
		nMaxMinutesBefore = max(nMaxMinutesBefore, pEvent->GetReminderMinutesBeforeStart());
		bReminderExists = TRUE;
	}

	if (!bReminderExists)
	{
		return FALSE;
	}

	COleDateTimeSpan spMaxBeforePeriod = CXTPCalendarUtils::Minutes2Span(nMaxMinutesBefore);

	COleDateTime dtOccFrom = ptrPattern->GetPatternStartDate();
	COleDateTime dtOccTo = dtFrom + spPeriod + spMaxBeforePeriod;
	ptrPattern->GetOccurrences(&arOccurrences, dtOccFrom, dtOccTo, pEvent);

	int nCount = arOccurrences.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pOccExc = arOccurrences.GetAt(i, FALSE);
		_ProcessNewSingleEvent(pOccExc, dtFrom, spPeriod);
	}
	return TRUE;
}

BOOL CXTPCalendarRemindersManager::_GetMaxExceptionReminder(
									CXTPCalendarRecurrencePattern* pPattern,
									int& rnMinutes)
{
	ASSERT(pPattern);
	if (!pPattern)
	{
		return FALSE;
	}

	CXTPCalendarEventsPtr ptrExcAr = pPattern->GetExceptions();
	if (!ptrExcAr)
	{
		return FALSE;
	}

	BOOL bExists = FALSE;
	rnMinutes = 0;

	int nCount = ptrExcAr->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pExc = ptrExcAr->GetAt(i, FALSE);
		if (pExc && pExc->IsReminder())
		{
			rnMinutes = max(rnMinutes, pExc->GetReminderMinutesBeforeStart());
			bExists = TRUE;
		}
	}
	return bExists;
}

void CXTPCalendarRemindersManager::OnTimer(UINT_PTR uTimerID)
{
	if (!m_pResourcesNf || 0 == m_pResourcesNf->GetCount())
	{
		ASSERT(FALSE);
		return;
	}

	if (uTimerID == TIMERID_RMD_NOTIFY_CHANGED)
	{
		KillTimer(TIMERID_RMD_NOTIFY_CHANGED);

		NotifyReminders(xtpCalendarRemindersFire);

		DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::POST NotifyReminders(fier). \n"),
							CXTPCalendarUtils::GetCurrentTime().Format());
		return;
	}


	//===========================================================================
	if (uTimerID != TIMERID_RMD_REFRESH)
	{
		ASSERT(FALSE);
		return;
	}
	//===========================================================================
	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();

	//-(1) Update cached data if need
	COleDateTime dtNextUpdateTime = m_dtLastUpdateTime + m_spPeriod2Cache;

	BOOL bUpdated = FALSE;
	if (dtNextUpdateTime <= dtNow || 0 == (DATE)m_dtLastUpdateTime)
	{
		DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::Update from timer ==>>. \n"),
							CXTPCalendarUtils::GetCurrentTime().Format());

		bUpdated = UpdateDataFromDP(dtNow, m_spPeriod2Cache);
	}

	//---------------------------------------------------------------------------
	//-(2) Notify reminders
	BOOL bUpdated2 = ProcessActiveReminders(dtNow);
	if (bUpdated || bUpdated2)
	{
		DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::NotifyReminders(fier). \n"),
							CXTPCalendarUtils::GetCurrentTime().Format());

		NotifyReminders(xtpCalendarRemindersFire);
	}
	//---------------------------------------------------------------------------
}

void CXTPCalendarRemindersManager::OnTimeChange()
{
	DBG_REMINDERS_TRACE(_T("%s - CXTPCalendarRemindersManager::OnTimeChange(update will soon). \n"),
							CXTPCalendarUtils::GetCurrentTime().Format());

	m_dtLastUpdateTime = (DATE)0;

	SetTimer(TIMERID_RMD_REFRESH, XTP_CALENDAR_RMD_REFRESH_TIMEOUT, NULL);
}

BOOL CXTPCalendarRemindersManager::ProcessActiveReminders(COleDateTime dtTime)
{
	CXTPAutoResetValue<BOOL> autoReset(m_bSkipOnEventChanged, FALSE);
	m_bSkipOnEventChanged = TRUE;

	BOOL bChanged = FALSE;
	int nCount = m_arWaitingReminders.GetCount();
	for (int i = nCount-1; i >= 0 ; i--)
	{
		CXTPCalendarReminder* pRmdI = m_arWaitingReminders.GetAt(i, FALSE);

		if (pRmdI->m_dtNextReminderTime <= dtTime)
		{
			m_arActiveReminders.Add(pRmdI, TRUE);
			m_arWaitingReminders.RemoveAt(i);

			bChanged = TRUE;

			_RemoveSnoozeData(pRmdI);
		}
	}
	if (bChanged)
		m_arActiveReminders.Sort();

	return bChanged;
}


BOOL CXTPCalendarRemindersManager::_RemoveSnoozeData(CXTPCalendarReminder* pRmd)
{
	CXTPCalendarEventPtr ptrEvent = pRmd->GetEvent();
	if (ptrEvent && ptrEvent->GetCustomProperties())
	{
		COleVariant varTmp;

		BOOL bSnoozed = ptrEvent->GetCustomProperties()->GetProperty(
							cszEventCustProp_NextReminderTime_Snoozed, varTmp);
		if (bSnoozed)
		{
			if (ptrEvent->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence)
			{
				CXTPCalendarRecurrencePatternPtr ptrPattern = ptrEvent->GetRecurrencePattern();
				CXTPCalendarEventPtr ptrMaster = ptrPattern ? ptrPattern->GetMasterEvent() : NULL;
				if (ptrMaster)
				{
					ptrPattern->SetOccReminder(ptrEvent, xtpCalendarRmdPrm_Default, (DATE)xtpCalendarRmdPrm_Default);

					VERIFY( ptrMaster->UpdateRecurrence(ptrPattern) );
					VERIFY( XTP_SAFE_GET1(ptrMaster->GetDataProvider(), ChangeEvent(ptrMaster), FALSE) );
				}
			}
			else
			{
				ptrEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_NextReminderTime_Snoozed);
				VERIFY( XTP_SAFE_GET1(ptrEvent->GetDataProvider(), ChangeEvent(ptrEvent), FALSE) );
			}

			return TRUE;
		}
	}
	return FALSE;
}

void CXTPCalendarRemindersManager::NotifyReminders(int eAction, LPARAM lParam)
{
	if (m_pConnection)
	{
		m_pConnection->SendEvent(XTP_NC_CALENDAR_ON_REMINDERS, (WPARAM)eAction, lParam);
	}
}

void CXTPCalendarRemindersManager::PostNotify_RemindersFire()
{
	SetTimer(TIMERID_RMD_NOTIFY_CHANGED, XTP_CALENDAR_RMD_NOTIFY_CHANGED_TIMEOUT, NULL);
}

int CXTPCalendarRemindersManager::GetActiveRemindersCount()
{
	return m_arActiveReminders.GetCount();
}

void CXTPCalendarRemindersManager::GetActiveReminders(CXTPCalendarReminders& rarActiveReminders)
{
	rarActiveReminders.RemoveAll();
	rarActiveReminders.Append(&m_arActiveReminders);
}

BOOL CXTPCalendarRemindersManager::Snooze(CXTPCalendarReminder* pReminder,
										  int nMinutesAfterNow)
{
	const int cnMaxSnoozeMinutes = (60*24*365) * 1000; // 1000 years

	// WARNING. no sense to snooze before now.
	ASSERT(nMinutesAfterNow >= 1);

	// WARNING. do you really need to snooze so far.
	ASSERT(nMinutesAfterNow <= cnMaxSnoozeMinutes);

	nMinutesAfterNow = max(1, min(nMinutesAfterNow, cnMaxSnoozeMinutes));

	CXTPCalendarEventPtr ptrEvent = pReminder ? pReminder->GetEvent() : NULL;

	if (!pReminder || !ptrEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pReminder->m_dtNextReminderTime = CXTPCalendarUtils::GetCurrentTime();
	pReminder->m_dtNextReminderTime += CXTPCalendarUtils::Minutes2Span(nMinutesAfterNow);

	m_arWaitingReminders.Add(pReminder, TRUE);

	int nIdx = m_arActiveReminders.Find(pReminder);
	if (nIdx >= 0)
	{
		m_arActiveReminders.RemoveAt(nIdx);
	}

	CXTPAutoResetValue<BOOL> autoReset(m_bSkipOnEventChanged, FALSE);
	m_bSkipOnEventChanged = TRUE;

	if (ptrEvent->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence)
	{
		CXTPCalendarRecurrencePatternPtr ptrPattern = ptrEvent->GetRecurrencePattern();
		CXTPCalendarEventPtr ptrMaster = ptrPattern ? ptrPattern->GetMasterEvent() : NULL;
		if (ptrMaster)
		{
			ptrPattern->SetOccReminder(ptrEvent, xtpCalendarRmdPrm_DontChange,
										(DATE)pReminder->m_dtNextReminderTime);

			VERIFY( ptrMaster->UpdateRecurrence(ptrPattern) );
			VERIFY( XTP_SAFE_GET1(ptrMaster->GetDataProvider(), ChangeEvent(ptrMaster), FALSE) );
		}
	}
	else
	{
		VERIFY( ptrEvent->GetCustomProperties()->SetProperty(
				cszEventCustProp_NextReminderTime_Snoozed,
				(DATE)pReminder->m_dtNextReminderTime) );

		VERIFY( XTP_SAFE_GET1(ptrEvent->GetDataProvider(), ChangeEvent(ptrEvent), FALSE) );
	}

	NotifyReminders(xtpCalendarReminderSnoozed, (LPARAM)pReminder);

	return TRUE;
}

BOOL CXTPCalendarRemindersManager::Dismiss(CXTPCalendarReminder* pReminder)
{
	if (!pReminder)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nIdx = m_arActiveReminders.Find(pReminder);
	if (nIdx >= 0)
	{
		m_arActiveReminders.RemoveAt(nIdx);
	}

	BOOL bRes = _Dismiss(pReminder);

	if (bRes)
	{
		NotifyReminders(xtpCalendarReminderDismissed, (LPARAM)pReminder);
	}

	return bRes;
}

BOOL CXTPCalendarRemindersManager::DismissAll()
{
	int nCount = m_arActiveReminders.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(i, FALSE);
		ASSERT(pReminder);
		if (pReminder)
		{
			_Dismiss(pReminder);
		}
	}
	m_arActiveReminders.RemoveAll();

	if (nCount)
	{
		NotifyReminders(xtpCalendarReminderDismissedAll);
	}

	return nCount > 0;
}

BOOL CXTPCalendarRemindersManager::_Dismiss(CXTPCalendarReminder* pReminder)
{
	CXTPCalendarEventPtr ptrEvent = pReminder ? pReminder->GetEvent() : NULL;

	if (!pReminder || !ptrEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPAutoResetValue<BOOL> autoReset(m_bSkipOnEventChanged, FALSE);
	m_bSkipOnEventChanged = TRUE;

	if (ptrEvent->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence)
	{
		CXTPCalendarRecurrencePatternPtr ptrPattern = ptrEvent->GetRecurrencePattern();
		CXTPCalendarEventPtr ptrMaster = ptrPattern ? ptrPattern->GetMasterEvent() : NULL;
		if (ptrMaster)
		{
			ptrPattern->SetOccReminder(ptrEvent, FALSE);

			VERIFY( ptrMaster->UpdateRecurrence(ptrPattern) );
			VERIFY( XTP_SAFE_GET1(ptrMaster->GetDataProvider(), ChangeEvent(ptrMaster), FALSE) );
		}
	}
	else
	{
		ptrEvent->SetReminder(FALSE);
		//ptrEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_NextReminderTime_Snoozed);

		VERIFY( XTP_SAFE_GET1(ptrEvent->GetDataProvider(), ChangeEvent(ptrEvent), FALSE) );
	}

	return TRUE;
}

BOOL CXTPCalendarRemindersManager::GetUpcomingEventsAll(COleDateTime dtFrom,
							COleDateTimeSpan spPeriod, CXTPCalendarEventsPtr& rptrEvents)
{
	rptrEvents = NULL;

	if (!m_pResourcesNf || !m_pResourcesNf->GetResourcesGroupedByDP())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPCalendarResources* pRCgroups = m_pResourcesNf->GetResourcesGroupedByDP();

	int i;
	int nCount = pRCgroups->GetCount();

	//-- check is all data providers opened -------------
	for (i = 0; i < nCount; i++)
	{
		CXTPCalendarResource* pRC = pRCgroups->GetAt(i);
		ASSERT(pRC);
		CXTPCalendarData* pData = pRC ? pRC->GetDataProvider() : NULL;
		ASSERT(pData);

		if (!pData || !pData->IsOpen())
		{
			return FALSE;
		}
	}

	//-- read data ----------------
	for (i = 0; i < nCount; i++)
	{
		CXTPCalendarResource* pRC = pRCgroups->GetAt(i);
		ASSERT(pRC);
		CXTPCalendarData* pData = pRC ? pRC->GetDataProvider() : NULL;

		if (!pData || !pData->IsOpen())
		{
			ASSERT(FALSE);
			continue;
		}

		CXTPCalendarEventsPtr ptrEv = pData->GetUpcomingEvents(dtFrom, spPeriod);

		if (rptrEvents)
		{
			rptrEvents->Append(ptrEv);
		}
		else
		{
			rptrEvents = ptrEv;
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

