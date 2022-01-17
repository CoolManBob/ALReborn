// XTPCalendarEvent.cpp: implementation of the CXTPCalendarEvent class.
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

#include "Common/XTPPropExchange.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"

#include "XTPCalendarEvent.h"
#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarUtils.h"
#include "XTPCalendarData.h"
#include "XTPCalendarEventLabel.h"
#include "XTPCalendarCustomProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR cszEventProp_EventID = _T("EventID");
static LPCTSTR cszEventProp_ScheduleID= _T("ScheduleID");
static LPCTSTR cszEventProp_StartTime = _T("StartTime");
static LPCTSTR cszEventProp_EndTime = _T("EndTime");

static LPCTSTR cszEventProp_Subject = _T("Subject");
static LPCTSTR cszEventProp_Location = _T("Location");
static LPCTSTR cszEventProp_Body                    = _T("Body");

// OLD. used to read old data files
static LPCTSTR cszEventPropOld_Description          = _T("Description");

static LPCTSTR cszEventProp_AllDayEvent = _T("AllDayEvent");
static LPCTSTR cszEventProp_LabelID = _T("LabelID");
static LPCTSTR cszEventProp_BusyStatus = _T("BusyStatus");
static LPCTSTR cszEventProp_Importance = _T("Importance");

static LPCTSTR cszEventProp_ReminderMinutesBeforeStart = _T("ReminderMinutesBeforeStart");
static LPCTSTR cszEventProp_ReminderSoundFile = _T("ReminderSoundFile");

static LPCTSTR cszEventProp_CreationTime = _T("CreationTime");
static LPCTSTR cszEventProp_LastModificationTime = _T("LastModificationTime");

static LPCTSTR cszEventProp_RecurrenceState = _T("RecurrenceState");

static LPCTSTR cszEventProp_IsMeeting = _T("IsMeeting");
static LPCTSTR cszEventProp_IsPrivate = _T("IsPrivate");
static LPCTSTR cszEventProp_IsReminder              = _T("IsReminder");

//## Recurrence Event properties
static LPCTSTR cszEventProp_RecurrencePatternID = _T("RecurrencePatternID");

//## Recurrence Event Exception properties
static LPCTSTR cszEventProp_RExceptionDeleted = _T("RExceptionDeleted");
static LPCTSTR cszEventProp_RException_StartTimeOrig= _T("RException_StartTimeOrig");
static LPCTSTR cszEventProp_RException_EndTimeOrig = _T("RException_EndTimeOrig");

/////////////////////////////////////////////////////////////////////////////
#define XTP_EVENT_PROP_SET_DATA_VER_1 1
#define XTP_EVENT_PROP_SET_DATA_VER 2

IMPLEMENT_DYNAMIC(CXTPCalendarIconIDs, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPCalendarEvent, CCmdTarget)

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
CXTPCalendarEvent::CXTPCalendarEvent(CXTPCalendarData* pDataProvider)
{
	ASSERT(pDataProvider);
	m_pDataProvider = pDataProvider;

	if (m_pDataProvider)
	{
		m_pDataProvider->InternalAddRef();
	}

	m_dwEventID = XTP_CALENDAR_UNKNOWN_EVENT_ID;
	m_nLabelID = XTP_CALENDAR_DEFAULT_LABEL_ID;
	m_uScheduleID = XTP_CALENDAR_UNKNOWN_SCHEDULE_ID;

	m_nBusyStatus = xtpCalendarBusyStatusFree;
	m_nImportance = xtpCalendarImportanceNormal;

	m_dwRecurrencePatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID;
	m_nRecurrenceState = xtpCalendarRecurrenceNotRecurring;
	m_bRExceptionDeleted = FALSE;


	m_bAllDayEvent = FALSE;
	m_dtStartTime = (DATE)0; //.SetStatus(COleDateTime::null);
	m_dtEndTime = (DATE)0; //.SetStatus(COleDateTime::null);

	m_dtCreationTime = m_dtLastModificationTime = CXTPCalendarUtils::GetCurrentTime();

	m_bReminder = FALSE;
	m_nReminderMinutesBeforeStart = XTP_CALENDAR_DEFAULT_REMINDER_MINUTES;

	m_dtRException_StartTimeOrig = (DATE)0; // .SetStatus(COleDateTime::null);
	m_dtRException_EndTimeOrig = (DATE)0; // .SetStatus(COleDateTime::null);

	m_bMeeting = FALSE;
	m_bPrivate = FALSE;

	m_dwRecurrencePatternID_ToUse = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID;

	m_pCustomProperties = new CXTPCalendarCustomProperties;
	m_pCustomIconsIDs = new CXTPCalendarCustomIconIDs;


}

CXTPCalendarEvent::~CXTPCalendarEvent()
{
	CMDTARGET_RELEASE(m_pDataProvider);
	CMDTARGET_RELEASE(m_pCustomProperties);
	CMDTARGET_RELEASE(m_pCustomIconsIDs);
}

CXTPCalendarEventLabel* CXTPCalendarEvent::GetLabel() const
{
	CXTPCalendarEventLabel* pResultLab = NULL;

	if (m_pDataProvider && m_pDataProvider->GetLabelList())
	{
		pResultLab = m_pDataProvider->GetLabelList()->Find(m_nLabelID);
	}

	return pResultLab;
}

void CXTPCalendarEvent::SetStartTime(COleDateTime dtStart)
{
	if (m_nRecurrenceState == xtpCalendarRecurrenceOccurrence ||
		m_nRecurrenceState == xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE);
		return;
	}

	m_dtStartTime = dtStart;

	if (m_dtStartTime.GetStatus() == COleDateTime::valid &&
		m_dtEndTime.GetStatus() == COleDateTime::valid &&
		(m_dtStartTime > m_dtEndTime ||
		 CXTPCalendarUtils::IsEqual(m_dtEndTime, m_dtStartTime)))
	{
		m_dtEndTime = m_dtStartTime;
	}
}

void CXTPCalendarEvent::SetEndTime(COleDateTime dtEnd)
{
	if (m_nRecurrenceState == xtpCalendarRecurrenceOccurrence ||
		m_nRecurrenceState == xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE);
		return;
	}

	m_dtEndTime = dtEnd;

	if (m_dtStartTime.GetStatus() == COleDateTime::valid &&
		m_dtEndTime.GetStatus() == COleDateTime::valid &&
		(m_dtEndTime < m_dtStartTime ||
		 CXTPCalendarUtils::IsEqual(m_dtEndTime, m_dtStartTime)))
	{
		m_dtStartTime = m_dtEndTime;
	}
}

void CXTPCalendarEvent::SetDuration(COleDateTimeSpan spDuration, BOOL bKeepStart)
{
	if (bKeepStart)
	{
		SetEndTime(m_dtStartTime + spDuration);
	}
	else
	{
		SetStartTime(m_dtEndTime - spDuration);
	}
}

void CXTPCalendarEvent::MoveEvent(COleDateTime dtNewStartTime)
{
	COleDateTimeSpan spDuration = m_dtEndTime - m_dtStartTime;
	SetStartTime(dtNewStartTime);
	SetEndTime(dtNewStartTime + spDuration);
}

int CXTPCalendarEvent::GetDurationMinutes() const
{
	return CXTPCalendarUtils::GetDurationMinutes(m_dtStartTime, m_dtEndTime, m_bAllDayEvent);
}

COleDateTimeSpan CXTPCalendarEvent::GetDuration() const
{
	COleDateTimeSpan spDuration;

	if (m_bAllDayEvent)
	{
		spDuration = CXTPCalendarUtils::ResetTime(m_dtEndTime) -
					 CXTPCalendarUtils::ResetTime(m_dtStartTime);
	}
	else
	{
		spDuration = m_dtEndTime - m_dtStartTime;
	}
	return spDuration;
}


CXTPCalendarRecurrencePatternPtr CXTPCalendarEvent::GetRecurrencePattern() const
{
	if (m_ptrRPattern)
	{
		ASSERT(m_nRecurrenceState == xtpCalendarRecurrenceMaster);

		CXTPCalendarRecurrencePatternPtr ptrPattern = m_ptrRPattern->ClonePattern();
		return ptrPattern;
	}

	if (!m_pDataProvider)
	{
		ASSERT(FALSE);
		return NULL;
	}
	CXTPCalendarRecurrencePatternPtr ptrPattern = m_pDataProvider->GetRecurrencePattern(m_dwRecurrencePatternID);
	return ptrPattern;
}

BOOL CXTPCalendarEvent::SetRecurrenceState_Master(CXTPCalendarRecurrencePattern* pPattern)
{
	if (!pPattern || m_nRecurrenceState != xtpCalendarRecurrenceNotRecurring)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	ASSERT(!m_ptrRPattern);

	m_nRecurrenceState = xtpCalendarRecurrenceMaster;

	m_ptrRPattern = pPattern;
	m_ptrRPattern->InternalAddRef();
	m_dwRecurrencePatternID = m_ptrRPattern->GetPatternID();

	m_ptrRPattern->SetMasterEventID(m_dwEventID);

	BOOL bRes = CalcStartEndDatesFromPattern();
	ASSERT(bRes);

	return bRes;
}

BOOL CXTPCalendarEvent::SetRecurrenceState_ExceptionOccurrence(XTPCalendarEventRecurrenceState nState, DWORD dwNewPatternID)
{
	ASSERT(nState == xtpCalendarRecurrenceOccurrence || nState == xtpCalendarRecurrenceException);

	if (m_ptrRPattern || m_nRecurrenceState == xtpCalendarRecurrenceMaster)
	{
		ASSERT(FALSE); //set new recur. state to master event. strange! why ???
		return FALSE;
	}

	// update original Start/End times for exception event
	if (nState == xtpCalendarRecurrenceException)
	{
		// You should change state to exception only for occurrence or new event (event copy).
		// !!! Otherwise this may not work correctly !!!
		//ASSERT(m_nRecurrenceState == xtpCalendarRecurrenceOccurrence);
		ASSERT(m_nRecurrenceState == xtpCalendarRecurrenceNotRecurring ||
			   m_nRecurrenceState == xtpCalendarRecurrenceOccurrence ||
			   m_nRecurrenceState == xtpCalendarRecurrenceException);

		COleDateTime dtTmpOrig = GetStartTime();
		SetRException_StartTimeOrig(dtTmpOrig);

		dtTmpOrig = GetEndTime();
		SetRException_EndTimeOrig(dtTmpOrig);
	}

	m_nRecurrenceState = nState;

	if (dwNewPatternID != XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID)
	{
		m_dwRecurrencePatternID = dwNewPatternID;
	}
	//ASSERT(m_dwRecurrencePatternID != XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

	return TRUE;
}

BOOL CXTPCalendarEvent::SetRecurrenceState_NotRecurring()
{
	if (m_ptrRPattern)
	{
		m_ptrRPattern->SetMasterEventID(XTP_CALENDAR_UNKNOWN_EVENT_ID);
	}
	m_dwRecurrencePatternID_ToUse = m_dwRecurrencePatternID;

	m_nRecurrenceState = xtpCalendarRecurrenceNotRecurring;
	m_ptrRPattern = NULL;
	m_dwRecurrencePatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID;

	return TRUE;
}

BOOL CXTPCalendarEvent::CalcStartEndDatesFromPattern()
{
	if (m_nRecurrenceState != xtpCalendarRecurrenceMaster || !m_ptrRPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	COleDateTime dtS = m_ptrRPattern->GetPatternStartDate();
	COleDateTime dtE(9999, 1, 1, 0, 0, 0);

	if (m_ptrRPattern->GetUseEndMethod() == xtpCalendarPatternEndDate ||
			m_ptrRPattern->GetUseEndMethod() == xtpCalendarPatternEndAfterOccurrences &&
			m_ptrRPattern->GetEndAfterOccurrences() < 4000)
	{
		CXTPCalendarDatesArray arDates;
		m_ptrRPattern->GetOccurrencesDates(arDates, dtS, dtE);
		int nCount = (int)arDates.GetSize();
		if (nCount)
		{
			dtS = arDates[0];
			dtE = arDates[nCount-1];

			//SetStartTime(m_ptrRPattern->GetStartOccurrenceTime(dtS));
			//SetEndTime(m_ptrRPattern->GetEndOccurrenceTime(dtS));
			m_dtStartTime = m_ptrRPattern->GetStartOccurrenceTime(dtS);
			m_dtEndTime = m_ptrRPattern->GetEndOccurrenceTime(dtS);

			ASSERT(GetEventPeriodDays() > 0);
			int nDays = max(1, GetEventPeriodDays());

			dtE += COleDateTimeSpan(nDays-1, 0, 0, 0);
		}
		else
		{
			dtE = dtS;
		}
	}

	//SetStartTime(dtS);
	//SetEndTime(dtE);

	m_dtStartTime = dtS;
	m_dtEndTime = dtE;

	UpdateStartEndDatesFromPatternExceptions();

	return TRUE;
}

BOOL CXTPCalendarEvent::UpdateStartEndDatesFromPatternExceptions()
{
	COleDateTime dtS = GetStartTime();
	COleDateTime dtE = GetEndTime();

	BOOL bRes = FALSE;
	//---------------------------------------------------------------------------
	CXTPCalendarEventsPtr ptrExceptions = m_ptrRPattern->GetExceptions();
	int nCount = ptrExceptions ? ptrExceptions->GetCount() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pExcept = ptrExceptions->GetAt(i);
		if (pExcept->GetStartTime() < dtS)
		{
			dtS = pExcept->GetStartTime();
			bRes = TRUE;
		}
		if (pExcept->GetEndTime() > dtE)
		{
			dtE = pExcept->GetEndTime();
			bRes = TRUE;
		}
	}
	//---------------------------------------------------------------------------

	if (bRes)
	{
		//SetStartTime(dtS);
		//SetEndTime(dtE);

		m_dtStartTime = dtS;
		m_dtEndTime = dtE;
	}

	return bRes;
}

CXTPCalendarEventPtr CXTPCalendarEvent::CloneEvent()
{
	if (!m_pDataProvider)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarEventPtr ptrNewEvent = m_pDataProvider->CreateNewEvent(GetEventID());

	if (!ptrNewEvent)
	{
		ASSERT(FALSE);
		return NULL;
	}

	if (CloneEventTo(ptrNewEvent))
	{
		return ptrNewEvent;
	}
	ASSERT(FALSE);
	return NULL;
}

BOOL CXTPCalendarEvent::CloneEventTo(CXTPCalendarEvent* pEventDest)
{
	if (!m_pDataProvider || !pEventDest)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pEventDest->Clear();

	BOOL bRes = pEventDest->Update(this);
	if (!bRes)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	XTPCalendarEventRecurrenceState nRState = GetRecurrenceState();

	if (nRState == xtpCalendarRecurrenceMaster)
	{
		CXTPCalendarRecurrencePatternPtr ptrRPattern = GetRecurrencePattern();
		bRes = pEventDest->SetRecurrenceState_Master(ptrRPattern);
	}
	else
		if (nRState != xtpCalendarRecurrenceNotRecurring)
		{
			ASSERT(nRState == xtpCalendarRecurrenceException || nRState == xtpCalendarRecurrenceOccurrence);
			DWORD dwPatternID = GetRecurrencePatternID();
			bRes = pEventDest->SetRecurrenceState_ExceptionOccurrence(nRState, dwPatternID);
		}

	if (nRState == xtpCalendarRecurrenceException)
	{
		COleDateTime dtTmpOrig = GetRException_StartTimeOrig();
		pEventDest->SetRException_StartTimeOrig(dtTmpOrig);

		dtTmpOrig = GetRException_EndTimeOrig();
		pEventDest->SetRException_EndTimeOrig(dtTmpOrig);
	}
	pEventDest->m_dwRecurrencePatternID_ToUse = m_dwRecurrencePatternID_ToUse;

	ASSERT(bRes);
	return bRes;
}

BOOL CXTPCalendarEvent::Update(CXTPCalendarEvent* pNewData)
{
	if (pNewData == this)
	{
		return TRUE;
	}

	UINT uScheduleID = pNewData->GetScheduleID();
	SetScheduleID(uScheduleID);

	CString strTmp = pNewData->GetSubject();
	SetSubject(strTmp);

	strTmp = pNewData->GetLocation();
	SetLocation(strTmp);

	int nTmp = pNewData->GetLabelID();
	SetLabelID(nTmp);

	BOOL bTmp = pNewData->IsAllDayEvent();
	SetAllDayEvent(bTmp);

	COleDateTime dtTmp = pNewData->GetStartTime();
	m_dtStartTime = dtTmp;

	dtTmp = pNewData->GetEndTime();
	m_dtEndTime = dtTmp;

	bTmp = pNewData->IsReminder();
	SetReminder(bTmp);

	nTmp = pNewData->GetReminderMinutesBeforeStart();
	SetReminderMinutesBeforeStart(nTmp);

	strTmp = pNewData->GetReminderSoundFile();
	SetReminderSoundFile(strTmp);

	strTmp = pNewData->GetBody();
	SetBody(strTmp);

	nTmp = pNewData->GetBusyStatus();
	SetBusyStatus(nTmp);

	nTmp = pNewData->GetImportance();
	SetImportance(nTmp);

	dtTmp = pNewData->GetCreationTime();
	SetCreationTime(dtTmp);

	dtTmp = pNewData->GetLastModificationTime();
	SetLastModificationTime(dtTmp);

	nTmp = pNewData->IsMeeting();
	SetMeeting(nTmp);

	nTmp = pNewData->IsPrivate();
	SetPrivate(nTmp);

	bTmp = pNewData->IsRExceptionDeleted();
	SetRExceptionDeleted(bTmp);

	dtTmp = pNewData->GetRException_StartTimeOrig();
	SetRException_StartTimeOrig(dtTmp);

	dtTmp = pNewData->GetRException_EndTimeOrig();
	SetRException_EndTimeOrig(dtTmp);

	ASSERT(pNewData->GetCustomProperties());
	if (pNewData->GetCustomProperties())
	{
		pNewData->GetCustomProperties()->CopyTo(m_pCustomProperties);
	}

	if (GetCustomIcons() && pNewData->GetCustomIcons())
	{
		GetCustomIcons()->Copy(*pNewData->GetCustomIcons());
	}
	else
	{
		XTP_SAFE_CALL1(GetCustomIcons(), RemoveAll());
	}

	return TRUE;
}

CXTPCalendarEventPtr CXTPCalendarEvent::CloneForOccurrence(COleDateTime dtStart,
										COleDateTime dtEnd, DWORD dwNewEventID)
{
	int eRecurrenceState = GetRecurrenceState();
	if (eRecurrenceState != xtpCalendarRecurrenceMaster || !m_pDataProvider)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarEventPtr ptrNewEvent = m_pDataProvider->CreateNewEvent(dwNewEventID);
	if (!ptrNewEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bRes = ptrNewEvent->Update(this);
	if (!bRes)
	{
		ASSERT(FALSE);
		return NULL;
	}

	ptrNewEvent->SetStartTime(dtStart);
	ptrNewEvent->SetEndTime(dtEnd);

	ptrNewEvent->SetRecurrenceState_ExceptionOccurrence(xtpCalendarRecurrenceOccurrence, m_dwRecurrencePatternID);

	ptrNewEvent->SetRExceptionDeleted(FALSE);

	return ptrNewEvent;
}

BOOL CXTPCalendarEvent::MakeEventAsRecurrence()
{
	if (m_nRecurrenceState != xtpCalendarRecurrenceNotRecurring || !m_pDataProvider)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	ASSERT(m_ptrRPattern == NULL);

	CXTPCalendarRecurrencePatternPtr ptrPattern;
	ptrPattern = m_pDataProvider->CreateNewRecurrencePattern(m_dwRecurrencePatternID_ToUse);

	if (!SetRecurrenceState_Master(ptrPattern))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL CXTPCalendarEvent::UpdateRecurrence(CXTPCalendarRecurrencePattern* pRPattern)
{
	if (m_nRecurrenceState != xtpCalendarRecurrenceMaster ||
			!m_pDataProvider || !m_ptrRPattern || !pRPattern)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//ASSERT(m_dwRecurrencePatternID == m_ptrRPattern->GetPatternID());
	//ASSERT(m_dwEventID == m_ptrRPattern->GetMasterEventID());

	//DWORD dwPatternID2 = pRPattern->GetPatternID();
	//DWORD dwMasterID2 = pRPattern->GetMasterEventID();

	//if (m_dwRecurrencePatternID != dwPatternID2 || m_dwEventID != dwMasterID2)
	//{
	//  ASSERT(FALSE);
	//  return FALSE;
	//}
	//------------------------------------------------
	BOOL bRes = m_ptrRPattern->Update(pRPattern);
	if (!bRes)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_dwRecurrencePatternID = m_ptrRPattern->GetPatternID();
	m_ptrRPattern->SetMasterEventID(m_dwEventID);

	bRes = CalcStartEndDatesFromPattern();
	ASSERT(bRes);

	return bRes;
}

BOOL CXTPCalendarEvent::RemoveRecurrence()
{
	COleDateTime dtStart, dtEnd;
	BOOL bIsMaster = GetRecurrenceState() == xtpCalendarRecurrenceMaster;

	if (bIsMaster && m_ptrRPattern)
	{
		dtStart = CXTPCalendarUtils::UpdateTime(m_ptrRPattern->GetPatternStartDate(),
					m_ptrRPattern->GetStartTime());

		dtEnd = dtStart + m_ptrRPattern->GetDuration();
	}
	else
	{
		bIsMaster = FALSE;
	}

	if (!SetRecurrenceState_NotRecurring())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (bIsMaster)
	{
		SetStartTime(dtStart);
		SetEndTime(dtEnd);
	}

	return TRUE;
}

CXTPCalendarRecurrencePattern* CXTPCalendarEvent::GetRPatternRef()
{
	ASSERT(GetRecurrenceState() == xtpCalendarRecurrenceMaster);

	return m_ptrRPattern;
}

int CXTPCalendarEvent::GetEventPeriodDays() const
{
	return CXTPCalendarUtils::GetEventPeriodDays(GetStartTime(), GetEndTime());
}

BOOL CXTPCalendarEvent::IsEqualStartEnd(CXTPCalendarEvent* pEvent) const
{
	COleDateTime dtStart2 = pEvent->GetStartTime();
	COleDateTime dtEnd2 = pEvent->GetEndTime();

	BOOL bRes = CXTPCalendarUtils::IsEqual(m_dtStartTime, dtStart2, TRUE) &&
				CXTPCalendarUtils::IsEqual(m_dtEndTime, dtEnd2, TRUE);
	return bRes;
}

BOOL CXTPCalendarEvent::IsEqualIDs(CXTPCalendarEvent* pEvent) const
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nRState2 = pEvent->GetRecurrenceState();

	BOOL bRes = FALSE;
	if (m_nRecurrenceState == xtpCalendarRecurrenceNotRecurring &&
			nRState2 == xtpCalendarRecurrenceNotRecurring)
	{
		DWORD dwEventID2 = pEvent->GetEventID();
		bRes = (m_dwEventID == dwEventID2);
	}
	else
	{
		DWORD dwPatternID2 = pEvent->GetRecurrencePatternID();

		// for xtpCalendarRecurrenceMaster & xtpCalendarRecurrenceOccurrence
		COleDateTime dtStart = m_dtStartTime;
		COleDateTime dtEnd = m_dtEndTime;

		COleDateTime dtStart2 = pEvent->GetStartTime();
		COleDateTime dtEnd2 = pEvent->GetEndTime();

		if (m_nRecurrenceState == xtpCalendarRecurrenceException)
		{
			dtStart = GetRException_StartTimeOrig();
			dtEnd = GetRException_EndTimeOrig();
		}

		if (pEvent->GetRecurrenceState() == xtpCalendarRecurrenceException)
		{
			dtStart2 = pEvent->GetRException_StartTimeOrig();
			dtEnd2 = pEvent->GetRException_EndTimeOrig();
		}

		bRes = m_dwRecurrencePatternID == dwPatternID2 &&
			   CXTPCalendarUtils::IsEqual(dtStart, dtStart2, TRUE) &&
			   CXTPCalendarUtils::IsEqual(dtEnd, dtEnd2, TRUE);
	}
	return bRes;
}

void CXTPCalendarEvent::Clear()
{
	CXTPCalendarEventPtr ptrEmptyEvent = m_pDataProvider->CreateNewEvent(GetEventID());
	if (!ptrEmptyEvent)
	{
		ASSERT(FALSE);
		return;
	}

	SetRecurrenceState_NotRecurring();
	Update(ptrEmptyEvent);
}

void CXTPCalendarEvent::DoPropExchange(CXTPPropExchange* pPX)
{
	long nVersion = XTP_EVENT_PROP_SET_DATA_VER;
	PX_Long(pPX, _T("Version"), nVersion, XTP_EVENT_PROP_SET_DATA_VER);

	// TODO:
	PX_DWord(pPX, cszEventProp_ScheduleID, (DWORD&)m_uScheduleID, 0);

	PX_DWord(pPX, cszEventProp_EventID, m_dwEventID, XTP_CALENDAR_UNKNOWN_EVENT_ID);
	PX_DateTime(pPX, cszEventProp_StartTime, m_dtStartTime);
	PX_DateTime(pPX, cszEventProp_EndTime, m_dtEndTime);

	PX_String(pPX, cszEventProp_Subject, m_strSubject, _T(""));
	PX_String(pPX, cszEventProp_Location, m_strLocation, _T(""));

	if (nVersion > XTP_EVENT_PROP_SET_DATA_VER_1)
	{
		PX_String(pPX, cszEventProp_Body, m_strBody, _T(""));
	}
	else
	{
		PX_String(pPX, cszEventPropOld_Description, m_strBody, _T(""));
	}

	PX_Bool(pPX, cszEventProp_AllDayEvent, m_bAllDayEvent, FALSE);
	PX_Int(pPX, cszEventProp_LabelID, m_nLabelID, XTP_CALENDAR_DEFAULT_LABEL_ID);

	PX_Int(pPX, cszEventProp_BusyStatus, m_nBusyStatus, xtpCalendarBusyStatusFree);
	PX_Int(pPX, cszEventProp_Importance, m_nImportance, xtpCalendarImportanceNormal);

	if (nVersion > XTP_EVENT_PROP_SET_DATA_VER_1)
	{
		PX_Bool(pPX, cszEventProp_IsReminder, m_bReminder, FALSE);
	}

	PX_Int(pPX, cszEventProp_ReminderMinutesBeforeStart, m_nReminderMinutesBeforeStart, 15);
	PX_String(pPX, cszEventProp_ReminderSoundFile, m_strReminderSoundFile, _T(""));

	PX_DateTime(pPX, cszEventProp_CreationTime, m_dtCreationTime);
	PX_DateTime(pPX, cszEventProp_LastModificationTime, m_dtLastModificationTime);

	PX_Bool(pPX, cszEventProp_IsMeeting, m_bMeeting, FALSE);
	PX_Bool(pPX, cszEventProp_IsPrivate, m_bPrivate, FALSE);

	if (pPX->IsStoring())
	{
		PX_Enum(pPX, cszEventProp_RecurrenceState, m_nRecurrenceState, xtpCalendarRecurrenceNotRecurring);
		PX_DWord(pPX, cszEventProp_RecurrencePatternID, m_dwRecurrencePatternID, XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

		if (m_nRecurrenceState == xtpCalendarRecurrenceException)
		{
			PX_Bool(pPX, cszEventProp_RExceptionDeleted, m_bRExceptionDeleted, FALSE);
			PX_DateTime(pPX, cszEventProp_RException_StartTimeOrig, m_dtRException_StartTimeOrig);
			PX_DateTime(pPX, cszEventProp_RException_EndTimeOrig, m_dtRException_EndTimeOrig);
		}
	}
	else
	{
		//===========================================================================
		XTPCalendarEventRecurrenceState nRecurrenceState = m_nRecurrenceState = xtpCalendarRecurrenceNotRecurring;
		PX_Enum(pPX, cszEventProp_RecurrenceState, nRecurrenceState, xtpCalendarRecurrenceNotRecurring);

		DWORD dwRecurrencePatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID;
		PX_DWord(pPX, cszEventProp_RecurrencePatternID, dwRecurrencePatternID, XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

		if (nRecurrenceState == xtpCalendarRecurrenceMaster)
		{
			if (!MakeEventAsRecurrence())
			{
				ASSERT(FALSE);
				return;
			}
		}
		else if (nRecurrenceState == xtpCalendarRecurrenceException || nRecurrenceState == xtpCalendarRecurrenceOccurrence)
		{
			SetRecurrenceState_ExceptionOccurrence(nRecurrenceState, dwRecurrencePatternID);

			if (nRecurrenceState == xtpCalendarRecurrenceException)
			{
				PX_Bool(pPX, cszEventProp_RExceptionDeleted, m_bRExceptionDeleted, FALSE);
				PX_DateTime(pPX, cszEventProp_RException_StartTimeOrig, m_dtRException_StartTimeOrig);
				PX_DateTime(pPX, cszEventProp_RException_EndTimeOrig, m_dtRException_EndTimeOrig);
			}
		}
	}

	//------------------------------------------------------------------------
	if (m_pCustomProperties)
		m_pCustomProperties->DoPropExchange(pPX);

	if (m_pCustomIconsIDs)
		m_pCustomIconsIDs->DoPropExchange(pPX);
}

void CXTPCalendarEvent::ClearIntermediateData()
{
	m_dwRecurrencePatternID_ToUse = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID;

	if (!IsReminder() && m_pCustomProperties)
	{
		// clear this property if reminder disabled
		m_pCustomProperties->RemoveProperty(cszEventCustProp_NextReminderTime_Snoozed);
	}

	if (!IsReminder() && m_ptrRPattern)
	{
		ASSERT(m_nRecurrenceState == xtpCalendarRecurrenceMaster);

		// clear reminders info for occurrences
		// if reminder disabled for whole series.
		m_ptrRPattern->GetOccReminders()->RemoveAll();
	}
}

void CXTPCalendarEvent::SetRecurrencePatternID(DWORD dwRecurrencePatternID)
{
	m_dwRecurrencePatternID = dwRecurrencePatternID;

	if (m_ptrRPattern)
	{
		ASSERT(m_nRecurrenceState == xtpCalendarRecurrenceMaster);
		m_ptrRPattern->SetPatternID(dwRecurrencePatternID);
	}
}

void CXTPCalendarEvent::SetEventID(DWORD dwEventID)
{
	m_dwEventID = dwEventID;

	if (m_nRecurrenceState == xtpCalendarRecurrenceMaster)
	{
		ASSERT(m_ptrRPattern);
		if (m_ptrRPattern)
		{
			m_ptrRPattern->SetMasterEventID(m_dwEventID);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
CXTPCalendarIconIDs::CXTPCalendarIconIDs()
{

}

CXTPCalendarIconIDs::~CXTPCalendarIconIDs()
{
}

int CXTPCalendarIconIDs::Find(UINT uID) const
{
	int nCount = GetSize();
	for (int i = 0; i < nCount; i++)
	{
		if (GetAt(i) == uID)
			return i;
	}
	return -1;
}

BOOL CXTPCalendarIconIDs::RemoveID(UINT uID)
{
	BOOL bResult = FALSE;

	int nCount = GetSize();
	for (int i = nCount - 1; i >= 0; i--)
	{
		if (GetAt(i) == uID)
		{
			RemoveAt(i);
			bResult = TRUE;
		}
	}

	return bResult;
}

void CXTPCalendarIconIDs::AddIfNeed(UINT uID)
{
	if (Find(uID) < 0)
	{
		Add(uID);
	}
}

CString CXTPCalendarIconIDs::SaveToString() const
{
	CString strData;

	TCHAR szNumber[34];

	int nCount = GetSize();
	for (int i = 0; i < nCount; i++)
	{
		UINT uElement = GetAt(i);

		ITOT_S((int)uElement, szNumber, _countof(szNumber));
		szNumber[33] = 0;

		if (!strData.IsEmpty())
		{
			strData += _T(",");
		}

		strData += szNumber;
	}

	return strData;
}

void CXTPCalendarIconIDs::LoadFromString(LPCTSTR pcszData)
{
	RemoveAll();

	CString strData = pcszData;

	while (!strData.IsEmpty())
	{
		CString strNumber = strData.SpanExcluding(_T(","));

		UINT uElement = (UINT)_ttoi(strNumber);
		Add(uElement);

		int nDelCount = strNumber.GetLength() + 1;
		nDelCount = min(nDelCount, strData.GetLength());
		DELETE_S(strData, 0, nDelCount);
	}
}

void CXTPCalendarIconIDs::DoPropExchange(CXTPPropExchange* pPX, LPCTSTR pcszSection, LPCTSTR pcszElement)
{
	if (!pPX || !pcszSection || !pcszElement)
	{
		ASSERT(FALSE);
		return;
	}

	long nVersion = 1;

	if (pPX->IsStoring())
	{
		CXTPPropExchangeSection secArray(pPX->GetSection(pcszSection));
		secArray->EmptySection();

		PX_Long(&secArray, _T("Version"), nVersion, 1);

		int nCount = GetSize();
		CXTPPropExchangeEnumeratorPtr pEnumerator(secArray->GetEnumerator(pcszElement));
		POSITION posStorage = pEnumerator->GetPosition(nCount);

		for (int i = 0; i < nCount; i++)
		{
			ULONG ulElement = (ULONG)GetAt(i);

			CXTPPropExchangeSection secElement(pEnumerator->GetNext(posStorage));
			PX_ULong(&secElement, _T("Value"), ulElement);
		}
	}
	else if (pPX->IsLoading())
	{
		RemoveAll();

		CXTPPropExchangeSection secArray(pPX->GetSection(pcszSection));

		PX_Long(&secArray, _T("Version"), nVersion, 1);

		if (nVersion != 1)
		{
			TRACE(_T("ERROR! CXTPCalendarIconIDs::DoPropExchange: Unsupported data version: %d \n"), nVersion);
			return;
		}

		CXTPPropExchangeEnumeratorPtr pEnumerator(secArray->GetEnumerator(pcszElement));
		POSITION posStorage = pEnumerator->GetPosition();

		while (posStorage)
		{
			ULONG ulElement = 0;

			CXTPPropExchangeSection secElement(pEnumerator->GetNext(posStorage));
			PX_ULong(&secElement, _T("Value"), ulElement, 0);

			Add((ULONG)ulElement);
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}



////////////////////////////////////////////////////////////////////////////
