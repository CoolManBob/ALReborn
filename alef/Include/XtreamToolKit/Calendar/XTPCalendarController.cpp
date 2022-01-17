// XTPCalendarController.cpp : implementation file
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
#include "Common/XTPMacros.h"

#include "Common/XTPNotifyConnection.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarView.h"
#include "XTPCalendarWeekView.h"
#include "XTPCalendarMonthView.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarData.h"
#include "XTPCalendarResource.h"
#include "XTPCalendarNotifications.h"

#include "XTPDatePickerItemMonth.h"
#include "XTPDatePickerItemDay.h"
#include "XTPDatePickerControl.h"
#include "XTPDatePickerPaintManager.h"
#include "XTPDatePickerNotifications.h"

#include "XTPCalendarController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// DBG
#define DBG_TRACE_DP_NF
#define DBG_TRACE_DP_TIMER

//#define DBG_TRACE_DP_NF  TRACE
//#define DBG_TRACE_DP_TIMER     TRACE
// DBG

/////////////////////////////////////////////////////////////////////////////
#define XTP_CC_DONT_SCROLL_DP 1234567890

#define XTP_CC_REFRESH_DAYS_STEP_TIMER_MS 120
#define XTP_CC_REFRESH_DAYS_STEP_DAYS     3

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarController

const int nXTPMaxDayViewDays = 6;

////////////////////////////////////////////////////////////////////////////
CXTPCalendarController::CXTPDayInfoCache::CXTPMapTimers CXTPCalendarController::CXTPDayInfoCache::m_mapTimers;

IMPLEMENT_DYNCREATE(CXTPCalendarController, CCmdTarget)
//////////////////////////////////////////////////////////////////////////
CXTPCalendarController::CXTPDayInfoCache::CXTPDayInfoCache()
{
	// 37, 53, 79 , 101, 127, 199, 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007, 16001
	m_mapDaysInfo.InitHashTable(503, FALSE);
	m_mapDaysToRefresh.InitHashTable(503, FALSE);

	m_pOwner = NULL;

	m_uTimerID = 0;
	m_dwLastRedrawTime = 0;
	m_dwLastSelfClearTime = 0;
	m_dwWaitingDataTime = 0;

	m_uActivePriority = 1;
}

CXTPCalendarController::CXTPDayInfoCache::~CXTPDayInfoCache()
{
	KillTimer();
}

void CXTPCalendarController::CXTPDayInfoCache::KillTimer()
{
	if (m_uTimerID)
	{
		::KillTimer(NULL, m_uTimerID);
		m_mapTimers.RemoveKey(m_uTimerID);
		m_uTimerID = 0;
	}
}

BOOL CXTPCalendarController::CXTPDayInfoCache::HasEvents(DATE dtDay)
{
	XTPDayInfo tmpDI;
	if (m_mapDaysInfo.Lookup((long)dtDay, tmpDI))
	{
		UpdateDayInfo(dtDay, tmpDI.bHasEvents);
		return tmpDI.bHasEvents;
	}
	else
	{
		_RequestToRefreshDays(dtDay, dtDay, m_uActivePriority);
	}
	return FALSE;
}

void CXTPCalendarController::CXTPDayInfoCache::Init(CXTPCalendarController* pOwner)
{
	ASSERT(pOwner);
	m_pOwner = pOwner;
}

void CXTPCalendarController::CXTPDayInfoCache::Clear()
{
	m_mapDaysInfo.RemoveAll();
	m_mapDaysToRefresh.RemoveAll();
	m_uActivePriority = 1;

	DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - CLEAR \n"));
}

UINT CXTPCalendarController::CXTPDayInfoCache::UpActivePriority()
{
	return ++m_uActivePriority;
}

void CXTPCalendarController::CXTPDayInfoCache::UpdateDayInfo(DATE dtDay, BOOL bHasEvents)
{
	XTPDayInfo tmpDI = {bHasEvents, (DATE)CXTPCalendarUtils::GetCurrentTime()};
	m_mapDaysInfo[(long)dtDay] = tmpDI;
}

void CXTPCalendarController::CXTPDayInfoCache::ClearDays(COleDateTime dtDayFrom, COleDateTime dtDayTo)
{
	long nDay = min((long)dtDayFrom, (long)dtDayTo);
	long nDay2 = max((long)dtDayFrom, (long)dtDayTo);
	for (; nDay <= nDay2; nDay++)
	{
		m_mapDaysInfo.RemoveKey(nDay);
	}

	DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - Clear days (%s - %s) \n"), (LPCTSTR)dtDayFrom.Format(), (LPCTSTR)dtDayTo.Format());
}

void CXTPCalendarController::CXTPDayInfoCache::RequestToRefreshDays(COleDateTime dtDayFrom, COleDateTime dtDayTo)
{
	m_uActivePriority++;

	DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - RequestToRefreshDays (%s - %s). ActivePriority = %d \n"),
					   (LPCTSTR)dtDayFrom.Format(), (LPCTSTR)dtDayTo.Format(), m_uActivePriority);

	_RequestToRefreshDays(dtDayFrom, dtDayTo, m_uActivePriority);
}

void CXTPCalendarController::CXTPDayInfoCache::_RequestToRefreshDays(COleDateTime dtDayFrom, COleDateTime dtDayTo, UINT uPriority)
{
	ASSERT(m_pOwner);
	if (!m_pOwner)
	{
		return;
	}

	long nDay = min((long)dtDayFrom, (long)dtDayTo);
	long nDay2 = max((long)dtDayFrom, (long)dtDayTo);

	// WARNING! - Ooo... The days range is too large!
	ASSERT(nDay2 - nDay < 10*1000);

	for (; nDay <= nDay2; nDay++)
	{
		m_mapDaysToRefresh[nDay] = uPriority;
	}

	if (m_mapDaysToRefresh.GetCount() && m_uTimerID == 0)
	{
		UINT uTimeOut = (UINT)m_pOwner->m_nBoldDaysIdleStepTime_ms;

		m_uTimerID = ::SetTimer(NULL, 0, uTimeOut, OnTimerCallback);
		m_mapTimers[m_uTimerID] = this;

		DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - _RequestToRefreshDays, SetTimer (ID = %d) \n"), m_uTimerID);
	}
}

void CXTPCalendarController::CXTPDayInfoCache::OnRefreshDays(int nDaysCountToRefresh)
{
	ASSERT(m_pOwner);

	if (!XTP_SAFE_GET1(m_pOwner, m_pResourcesNf, NULL))
	{
		if (!m_dwWaitingDataTime)
		{
			m_dwWaitingDataTime = GetTickCount();
			DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnRefreshDays, Start Wait for data\n"));
		}

		if (abs((long)(GetTickCount() - m_dwWaitingDataTime)) >= 5000)
		{
			m_dwWaitingDataTime = 0;

			if (m_uTimerID)
			{
				DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnRefreshDays, KillTimer (ID = %d) \n"), m_uTimerID);
				DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnRefreshDays, end Wait for data\n"));

				KillTimer();

				OnSelfClearOld();
			}
		}
		return;
	}
	else
	{
		if (m_dwWaitingDataTime)
		{
			DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnRefreshDays, end Wait for data\n"));
		}
		m_dwWaitingDataTime = 0;
	}

	//***************************************************
	for (int i = 0; i < nDaysCountToRefresh; i++)
	{
		long nDay_min = LONG_MAX;
		UINT uPriority_max = 0;

		POSITION pos = m_mapDaysToRefresh.GetStartPosition();
		while (pos)
		{
			long nDay = 0;
			UINT uPriority = 0;
			m_mapDaysToRefresh.GetNextAssoc(pos, nDay, uPriority);

			if (uPriority == uPriority_max && nDay < nDay_min ||
				uPriority > uPriority_max)
			{
				uPriority_max = uPriority;
				nDay_min = nDay;
			}
		}

		if (nDay_min < LONG_MAX)
		{
			m_mapDaysToRefresh.RemoveKey(nDay_min);

			COleDateTime dtDay((DATE)nDay_min);
			BOOL bHasEvents = XTP_SAFE_GET1(m_pOwner, _HasEvents(dtDay), FALSE);

			UpdateDayInfo(dtDay, bHasEvents);
		}
	}
	//***************************************************

	if (m_mapDaysToRefresh.GetCount() == 0 && m_uTimerID)
	{
		DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnRefreshDays, KillTimer (ID = %d) \n"), m_uTimerID);
		DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - Days In Cache %d \n"), m_mapDaysInfo.GetCount());

		KillTimer();

		OnSelfClearOld();
	}

	if (abs((long)(GetTickCount() - m_dwLastRedrawTime)) >= 500 || m_uTimerID == 0)
	{
		XTP_SAFE_CALL2(m_pOwner, m_pDatePickerCtrl, RedrawControl());
		m_dwLastRedrawTime = GetTickCount();
	}
}
void CXTPCalendarController::CXTPDayInfoCache::OnSelfClearOld()
{

#ifdef _DEBUG
	int nUpdateTime_ms = 20 * 1000;
	COleDateTimeSpan spCachePeriod(0, 0, 1, 0); // 1 min
#else
	int nUpdateTime_ms = 5 * 60 * 1000; // 5 min
	COleDateTimeSpan spCachePeriod(0, 1, 0, 0); // 1 hour
#endif

	if (abs((long)(GetTickCount() - m_dwLastSelfClearTime)) < nUpdateTime_ms)
	{
		return;
	}
	m_dwLastSelfClearTime = ::GetTickCount();

	COleDateTime dtOldTime = CXTPCalendarUtils::GetCurrentTime();
	dtOldTime -= spCachePeriod;

	DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnSelfClear, Start. Days In Cache %d \n"), m_mapDaysInfo.GetCount());

	POSITION pos = m_mapDaysInfo.GetStartPosition();
	while (pos)
	{
		long nDay = 0;
		XTPDayInfo tmpDI;

		m_mapDaysInfo.GetNextAssoc(pos, nDay, tmpDI);
		if (tmpDI.dtLastAccessTime < dtOldTime)
		{
			m_mapDaysInfo.RemoveKey(nDay);
		}
	}

	DBG_TRACE_DP_TIMER(_T("XTPCalendarController::DayInfoCache - OnSelfClear, End. Days In Cache %d \n"), m_mapDaysInfo.GetCount());
}

VOID CALLBACK CXTPCalendarController::CXTPDayInfoCache::OnTimerCallback(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(dwTime);

	 CXTPDayInfoCache* pThis = m_mapTimers[idEvent];
	 ASSERT(pThis && pThis->m_pOwner);

	 if (pThis && pThis->m_pOwner)
	 {
		//***********************
		SAFE_MANAGE_STATE(pThis->m_pOwner->m_pModuleState);
		//***********************

		 pThis->OnRefreshDays(pThis->m_pOwner->m_nBoldDaysPerIdleStep);
	 }
}


//////////////////////////////////////////////////////////////////////////

CXTPCalendarController::CXTPCalendarController()
{
	m_pCalendarCtrl = NULL;
	m_pDatePickerCtrl = NULL;
	m_bAdjustingView = FALSE;

	m_bBoldDaysWithEvents = TRUE;

	m_bBoldDaysOnIdle = TRUE;
	m_nBoldDaysPerIdleStep = XTP_CC_REFRESH_DAYS_STEP_DAYS;
	m_nBoldDaysIdleStepTime_ms = XTP_CC_REFRESH_DAYS_STEP_TIMER_MS;

	m_bUseActiveViewResoucesForBold = FALSE;

	m_pResourcesNf = NULL;
	m_eLastActiveView = -1;

	m_cnidEventAdded = 0;
	m_cnidEventChanged = 0;
	m_cnidEventDeleted = 0;

	m_DayInfoCache.Init(this);
}

CXTPCalendarController::~CXTPCalendarController()
{
	CMDTARGET_RELEASE(m_pCalendarCtrl);
	CMDTARGET_RELEASE(m_pDatePickerCtrl);
	CMDTARGET_RELEASE(m_pResourcesNf);
}


/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarController message handlers

void CXTPCalendarController::AdviseToNotifications()
{
	m_Sink.UnadviseAll();

	m_cnidEventAdded = 0;
	m_cnidEventChanged = 0;
	m_cnidEventDeleted = 0;

	CMDTARGET_RELEASE(m_pResourcesNf);

	// Advise to Calendar notifications
	if (m_pCalendarCtrl)
	{
		CXTPNotifyConnection* ptrDPConn = m_pCalendarCtrl ? m_pCalendarCtrl->GetConnection() : NULL;
		ASSERT(ptrDPConn);

		if (!ptrDPConn)
		{
			return;
		}

		m_Sink.Advise(ptrDPConn, XTP_NC_CALENDARVIEWWASCHANGED, &CXTPCalendarController::OnEvent_CalendarView);
		m_Sink.Advise(ptrDPConn, XTP_NC_CALENDAROPTIONSWASCHANGED, &CXTPCalendarController::OnEvent_CalendarOptionsChanged);
		m_Sink.Advise(ptrDPConn, XTP_NC_CALENDAR_RESOURCES_WHERE_CHANGED, &CXTPCalendarController::OnEvent_CalendarResourcesChanged);
	}

	// Advise to Date Picker notifications
	if (m_pDatePickerCtrl)
	{
		CXTPNotifyConnection* ptrDPConn = m_pDatePickerCtrl ? m_pDatePickerCtrl->GetConnection() : NULL;
		ASSERT(ptrDPConn);

		if (!ptrDPConn)
		{
			return;
		}

		m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERBUTTONCLICKED, &CXTPCalendarController::OnEvent_DatePicker);
		m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERSELECTIONCHANGED, &CXTPCalendarController::OnEvent_DatePicker);
		m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERBEFOREGOMODAL, &CXTPCalendarController::OnEvent_DatePicker);
		m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERMONTHCHANGED, &CXTPCalendarController::OnEvent_DatePicker);
		m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERGETDAYMETRICS, &CXTPCalendarController::OnEvent_DtPickGetItemMetrics);
	}
}

void CXTPCalendarController::_AdviseToDataChanged()
{
	ASSERT(m_pResourcesNf);
	if (!m_pResourcesNf)
	{
		return;
	}

	CXTPNotifyConnection* ptrDPConn = m_pResourcesNf->GetConnection();
	ASSERT(ptrDPConn);

	if (!ptrDPConn)
	{
		return;
	}

	m_cnidEventAdded = m_Sink.Advise(ptrDPConn, XTP_NC_CALENDAREVENTWASADDED, &CXTPCalendarController::OnEvent_CalendarDataChanged);
	m_cnidEventChanged = m_Sink.Advise(ptrDPConn, XTP_NC_CALENDAREVENTWASCHANGED, &CXTPCalendarController::OnEvent_CalendarDataChanged);
	m_cnidEventDeleted = m_Sink.Advise(ptrDPConn, XTP_NC_CALENDAREVENTWASDELETED, &CXTPCalendarController::OnEvent_CalendarDataChanged);
}

void CXTPCalendarController::_UnadviseFromDataChanged()
{
	if (m_cnidEventAdded)
	{
		m_Sink.Unadvise(m_cnidEventAdded);
	}

	if (m_cnidEventChanged)
	{
		m_Sink.Unadvise(m_cnidEventChanged);
	}

	if (m_cnidEventDeleted)
	{
		m_Sink.Unadvise(m_cnidEventDeleted);
	}

	m_cnidEventAdded = 0;
	m_cnidEventChanged = 0;
	m_cnidEventDeleted = 0;
}

BOOL CXTPCalendarController::IsFirstDayOfWeekShifted_FromSun2Mon()
{
	if (!m_pCalendarCtrl || !m_pCalendarCtrl->GetActiveView())
	{
		return FALSE;
	}

	int nViewType = m_pCalendarCtrl->GetActiveView()->GetViewType();
	if (nViewType == xtpCalendarMonthView || nViewType == xtpCalendarWeekView)
	{
		int nFirstDayOfWeekIndex = m_pCalendarCtrl->GetFirstDayOfWeek();

		int bCompress = (nViewType == xtpCalendarWeekView) ||
			m_pCalendarCtrl->MonthView_IsCompressWeekendDays();

		if (bCompress && nFirstDayOfWeekIndex == 1)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CXTPCalendarController::OnEvent_CalendarView(XTP_NOTIFY_CODE Event, WPARAM /*wParam*/, LPARAM lParam)
{
	if (!XTP_NC_CALENDARVIEWWASCHANGED == Event)
	{
		ASSERT(FALSE);
		return;
	}
	if (m_bAdjustingView)
	{
		return;
	}

	if (m_pDatePickerCtrl && m_pCalendarCtrl)
	{
		CXTPCalendarView* pView = m_pCalendarCtrl->GetActiveView();
		if (!pView)
			return;

		//--------------------------------------------------------
		int nViewTypeX = pView->GetViewType();
		if (nViewTypeX == xtpCalendarWorkWeekView)
		{
			nViewTypeX = xtpCalendarDayView;
		}

		if (m_eLastActiveView != nViewTypeX)
		{
			m_eLastActiveView = nViewTypeX;
			CMDTARGET_RELEASE(m_pResourcesNf);
			_UnadviseFromDataChanged();

			if (m_bBoldDaysOnIdle)
			{
				COleDateTime dtFirstVisibleDay, dtLastVisibleDay;
				if (m_pDatePickerCtrl->GetVisibleRange(dtFirstVisibleDay, dtLastVisibleDay))
				{
					m_DayInfoCache.RequestToRefreshDays(dtFirstVisibleDay, dtLastVisibleDay);
				}
			}
		}

		//--------------------------------------------------------
		COleDateTimeSpan spCompressCorrector(0 , 0, 0, 0);
		if (IsFirstDayOfWeekShifted_FromSun2Mon())
		{
			spCompressCorrector.SetDateTimeSpan(-1, 0, 0, 0);
		}

		int nCount = pView->GetViewDayCount();
		if (nCount > 0)
		{
			COleDateTime dtDay = m_pCalendarCtrl->GetActiveView()->GetViewDayDate(0);
			dtDay += spCompressCorrector;

			m_pDatePickerCtrl->SetSelRange(dtDay, dtDay);
		}

		for (int i = 1; i < nCount; i++)
		{
			COleDateTime dtDay = m_pCalendarCtrl->GetActiveView()->GetViewDayDate(i);
			dtDay += spCompressCorrector;
			m_pDatePickerCtrl->Select(dtDay);
		}

		if (lParam != XTP_CC_DONT_SCROLL_DP)
			m_pDatePickerCtrl->EnsureVisibleSelection();

		if (::IsWindow(m_pDatePickerCtrl->GetSafeHwnd()))
		{
			m_pDatePickerCtrl->RedrawControl();
			m_pDatePickerCtrl->UpdateWindow();
		}
	}
}

void CXTPCalendarController::OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM /*lParam*/)
{
	wParam;

	switch (Event)
	{
	case XTP_NC_DATEPICKERBEFOREGOMODAL:
		if (m_pDatePickerCtrl && m_pCalendarCtrl)
		{
			OnEvent_CalendarView(XTP_NC_CALENDARVIEWWASCHANGED, 0, 0);
		}
		break;
	case XTP_NC_DATEPICKERBUTTONCLICKED:
		DBG_TRACE_DP_NF(_T("OnEvent: DatePickerButtonClicked - ID = %u.     {CalendarController}. \n"), wParam);
		break;
	case XTP_NC_DATEPICKERMONTHCHANGED:
		m_DayInfoCache.UpActivePriority();
		break;
	case XTP_NC_DATEPICKERSELECTIONCHANGED:
		DBG_TRACE_DP_NF(_T("OnEvent: XTP_NC_DATEPICKERSELECTIONCHANGED.     {CalendarController}. \n"));
		if (m_pDatePickerCtrl && m_pCalendarCtrl && m_pCalendarCtrl->GetActiveView())
		{
			BOOL bChanged = FALSE;

			COleDateTime dtFrom;
			COleDateTime dtTo;
			if (m_pDatePickerCtrl->GetSelRange(dtFrom, dtTo))
			{
				int nFirstDayOfWeekIndex = m_pDatePickerCtrl->GetFirstDayOfWeek();

				CXTPCalendarControl::CUpdateContext updateContext(m_pCalendarCtrl);

				int nSelRangeDays = GETTOTAL_DAYS_DTS(dtTo - dtFrom)+1;
				// count selected days
				int nSelDays = 0;
				COleDateTimeSpan spDay(1, 0, 0, 0);
				for (COleDateTime dtDay = dtFrom; dtDay <= dtTo; dtDay += spDay)
				{
					if (m_pDatePickerCtrl->IsSelected(dtDay))
					{
						nSelDays++;
					}
				}

				// Day view
				if (nSelRangeDays <= nXTPMaxDayViewDays || nSelRangeDays != nSelDays ||
					dtFrom.GetDayOfWeek() != nFirstDayOfWeekIndex && nSelRangeDays <= 8
				 )
				{
					m_bAdjustingView = TRUE;

					// to save WorkWeek view mode
					int eViewMode = XTP_SAFE_GET2(m_pCalendarCtrl, GetActiveView(), GetViewType(), xtpCalendarDayView);

					if (eViewMode != xtpCalendarWorkWeekView || nSelDays != 1)
					{
						eViewMode = xtpCalendarDayView;
					}

					// WorkWeekView - set visible days
					if (eViewMode == xtpCalendarWorkWeekView)
					{
						CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, m_pCalendarCtrl->GetActiveView());
						if (pDayView)
						{
							pDayView->UnselectAllEvents();

							COleDateTime dtWWStart;
							m_pCalendarCtrl->GetWorkDayStartTime(dtWWStart);
							dtWWStart = CXTPCalendarUtils::UpdateDate(dtWWStart, dtFrom);

							pDayView->SetSelection(dtWWStart, dtWWStart + pDayView->GetCellDuration());
						}
					}

					//*** switch calendar view
					m_pCalendarCtrl->SwitchActiveView((XTPCalendarViewType)eViewMode);
					//***

					CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, m_pCalendarCtrl->GetActiveView());

					// DayView - set visible days
					if (pDayView && eViewMode == xtpCalendarDayView)
					{
						COleDateTime dtSelStart, dtSelEnd;
						BOOL bAllDay = FALSE;
						BOOL bSel = pDayView->GetSelection(&dtSelStart, &dtSelEnd, &bAllDay);
						if (bSel)
						{
							COleDateTimeSpan spSel = dtSelEnd - dtSelStart;
							COleDateTimeSpan spSelMin = pDayView->GetCellDuration();
							if (spSel < spSelMin)
							{
								bSel = FALSE;
							}
						}
						if (!bSel)
						{
							m_pCalendarCtrl->GetWorkDayStartTime(dtSelStart);
							dtSelEnd = dtSelStart + pDayView->GetCellDuration();
							bAllDay = FALSE;
						}
						dtSelStart = CXTPCalendarUtils::UpdateDate(dtSelStart, dtFrom);
						dtSelEnd = CXTPCalendarUtils::UpdateDate(dtSelEnd, dtFrom);

						pDayView->ShowDay(dtFrom);

						pDayView->SetSelection(dtSelStart, dtSelEnd, bAllDay);

						// add other selected days to calendar view
						for (COleDateTime dtDay(dtFrom + spDay); dtDay <= dtTo; dtDay += spDay)
						{
							if (m_pDatePickerCtrl->IsSelected(dtDay))
							{
								// add it to Calendar View
								pDayView->AddDay(dtDay);
							}
						}
						m_pCalendarCtrl->Populate();
					}

					m_bAdjustingView = FALSE;

					bChanged = TRUE;
				}
				// Week view
				else if (nSelRangeDays == 7 && nSelRangeDays == nSelDays)
				{
					COleDateTime dtBegin = dtFrom;
					if (IsFirstDayOfWeekShifted_FromSun2Mon())
					{
						dtBegin += spDay;
					}

					m_bAdjustingView = TRUE;

					m_pCalendarCtrl->SwitchActiveView(xtpCalendarWeekView);
					CXTPCalendarWeekView* pWeekView = DYNAMIC_DOWNCAST(CXTPCalendarWeekView, m_pCalendarCtrl->GetActiveView());
					if (pWeekView)
					{
						pWeekView->SetBeginDate(dtBegin);

						m_pCalendarCtrl->Populate();
					}

					m_bAdjustingView = FALSE;

					bChanged = TRUE;
				}
				// Month view
				else
				{
					ASSERT(nSelRangeDays == nSelDays && nSelRangeDays > 7);

					m_bAdjustingView = TRUE;

					m_pCalendarCtrl->SwitchActiveView(xtpCalendarMonthView);
					CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, m_pCalendarCtrl->GetActiveView());
					if (pMonthView)
					{
						CXTPCalendarControl::CViewChangedContext viewChanged(m_pCalendarCtrl, xtpCalendarViewChangedLock);

						COleDateTime dtBegin = dtFrom;
						if (IsFirstDayOfWeekShifted_FromSun2Mon())
						{
							dtBegin += spDay;
						}

						// Calculate weeks count
						COleDateTime dtWeekFromBegin = pMonthView->GetGrid()->ShiftDateToCell_00(dtFrom);
						COleDateTime dtWeekToEnd = pMonthView->GetGrid()->ShiftDateToCell_00(dtTo);
						dtWeekToEnd += COleDateTimeSpan(7, 0, 0, 0);
						int nSelectedWeeksRange = GETTOTAL_DAYS_DTS(dtWeekToEnd - dtWeekFromBegin);

						int nWeeks = nSelectedWeeksRange / 7;
						nWeeks = max(XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN, nWeeks);
						nWeeks = min(XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX, nWeeks);

						pMonthView->GetGrid()->SetBeginDate(dtBegin);
						pMonthView->GetGrid()->SetWeeksCount(nWeeks);

						m_pCalendarCtrl->Populate();
					}

					m_bAdjustingView = FALSE;

					bChanged = TRUE;
				}

				if (bChanged)
				{
					OnEvent_CalendarView(XTP_NC_CALENDARVIEWWASCHANGED, 0, XTP_CC_DONT_SCROLL_DP);

					if (m_pDatePickerCtrl->IsModal())
					{
						m_pCalendarCtrl->Invalidate(FALSE);
						m_pCalendarCtrl->RedrawControl();
					}
				}
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
}


void CXTPCalendarController::OnEvent_CalendarOptionsChanged(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM /*lParam*/)
{
	if (Event != XTP_NC_CALENDAROPTIONSWASCHANGED)
	{
		ASSERT(FALSE);
		return;
	}

	if ((int)wParam < 0)
	{
		SetFirstDayOfWeekToDatePicker();
	}
}

void CXTPCalendarController::OnEvent_CalendarResourcesChanged(XTP_NOTIFY_CODE Event, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (Event != XTP_NC_CALENDARDATAPROVIDERWASCHANGED)
	{
		ASSERT(FALSE);
		return;
	}

	CMDTARGET_RELEASE(m_pResourcesNf);
	_UnadviseFromDataChanged();

	m_DayInfoCache.Clear();
}

void CXTPCalendarController::OnEvent_CalendarDataChanged(XTP_NOTIFY_CODE Event, WPARAM /*wParam*/, LPARAM lParam)
{
	if (!m_pDatePickerCtrl)
	{
		return;
	}

	if (Event != XTP_NC_CALENDAREVENTWASADDED &&
		Event != XTP_NC_CALENDAREVENTWASCHANGED&&
		Event != XTP_NC_CALENDAREVENTWASDELETED)
	{
		ASSERT(FALSE);
		return;
	}

	//-------------------------------------
	COleDateTime dtFirstVisibleDay, dtLastVisibleDay;

	BOOL bRes = m_pDatePickerCtrl->GetVisibleRange(dtFirstVisibleDay, dtLastVisibleDay);
	if (!bRes)
	{
		return;
	}

	CXTPCalendarEvent* pEvent = (CXTPCalendarEvent*)lParam;
	if (!pEvent)
	{
		return;
	}

	if (Event == XTP_NC_CALENDAREVENTWASADDED ||
		Event == XTP_NC_CALENDAREVENTWASDELETED)
	{
		if (CXTPCalendarUtils::ResetTime(pEvent->GetStartTime()) > dtLastVisibleDay ||
			CXTPCalendarUtils::ResetTime(pEvent->GetEndTime()) < dtFirstVisibleDay )
		{
			return; // xtpCalendar_Skip;
		}

		if (m_bBoldDaysOnIdle)
		{
			m_DayInfoCache.ClearDays(pEvent->GetStartTime(), pEvent->GetEndTime());
		}
	}
	else
	{
		if (m_bBoldDaysOnIdle)
		{
			m_DayInfoCache.RequestToRefreshDays(dtFirstVisibleDay, dtLastVisibleDay);

			COleDateTime dtFom = max(dtFirstVisibleDay, pEvent->GetStartTime());
			COleDateTime dtTo = min(dtLastVisibleDay, pEvent->GetEndTime());
			m_DayInfoCache.RequestToRefreshDays(dtFom, dtTo);
		}
	}

	m_pDatePickerCtrl->RedrawControl();
}

void CXTPCalendarController::SetFirstDayOfWeekToDatePicker()
{
	if (m_pDatePickerCtrl && m_pCalendarCtrl)
	{
		int nFirstDayOfWeekIndex = m_pCalendarCtrl->GetFirstDayOfWeek();

		m_pDatePickerCtrl->SetFirstDayOfWeek(nFirstDayOfWeekIndex);
	}
}

void CXTPCalendarController::OnEvent_DtPickGetItemMetrics(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam)
{
	if (Event != XTP_NC_DATEPICKERGETDAYMETRICS)
	{
		ASSERT(FALSE);
		return;
	}
	if (!m_bBoldDaysWithEvents)
	{
		return;
	}

	const COleDateTime dtDay = (DATE)(XTP_DATE_VALUE)wParam;
	XTP_DAYITEM_METRICS* pDayItemMetrics = (XTP_DAYITEM_METRICS*)lParam;
	ASSERT(pDayItemMetrics);

	if (!pDayItemMetrics || !m_pCalendarCtrl || !m_pDatePickerCtrl)
	{
		return;
	}
	//--------------------------------------------------------------------
	if (!m_pResourcesNf)
	{
		m_pResourcesNf = new CXTPCalendarResourcesNf();
		if (!m_pResourcesNf)
		{
			return;
		}
		CXTPCalendarResources* pRC0 = m_pCalendarCtrl->GetResources();
		if (m_bUseActiveViewResoucesForBold)
		{
			pRC0 = m_pCalendarCtrl->GetActiveView()->GetResources();
		}

		m_pResourcesNf->Append(pRC0);

		_AdviseToDataChanged();

		m_pResourcesNf->ReBuildInternalData();
	}

	//--------------------------------------------------------------------
	BOOL bHasEvents = FALSE;

	if (m_bBoldDaysOnIdle)
	{
		bHasEvents = m_DayInfoCache.HasEvents(dtDay);
	}
	else
	{
		// check whether specified day has events
		bHasEvents = _HasEvents(dtDay);
	}

	// make the day with events as bold
	if (bHasEvents)
	{
		// set bold to day metrics
		pDayItemMetrics->SetFont(m_pDatePickerCtrl->GetPaintManager()->GetDayTextFontBold());
	}
}
BOOL CXTPCalendarController::_HasEvents(COleDateTime dtDay)
{
	ASSERT(m_pResourcesNf);
	if (!m_pResourcesNf)
	{
		return FALSE;
	}

	CXTPCalendarResources* pRCx = m_pResourcesNf->GetResourcesGroupedByDP();
	int nRCCount = pRCx ? pRCx->GetCount() : 0;

	for (int i = 0; i < nRCCount; i++)
	{
		CXTPCalendarResource* pRC = pRCx->GetAt(i);
		ASSERT(pRC);
		CXTPCalendarEventsPtr ptrEvents = pRC ? pRC->RetrieveDayEvents(dtDay) : NULL;
		if (ptrEvents && ptrEvents->GetCount() > 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}


void CXTPCalendarController::SetCalendar(CXTPCalendarControl* pCalendarCtrl)
{
	m_Sink.UnadviseAll();

	if (m_pCalendarCtrl)
	{
		m_pCalendarCtrl->InternalRelease();
	}
	m_pCalendarCtrl = pCalendarCtrl;
	if (m_pCalendarCtrl)
	{
		m_pCalendarCtrl->InternalAddRef();

		AdviseToNotifications();

		SetFirstDayOfWeekToDatePicker();
	}

	if (m_pCalendarCtrl && m_pDatePickerCtrl)
	{
		m_eLastActiveView = -1; // to ensure update
		OnEvent_CalendarView(XTP_NC_CALENDARVIEWWASCHANGED, 0, 0);
	}
}

void CXTPCalendarController::SetDatePicker(CXTPDatePickerControl* pDatePickerCtrl)
{
	m_Sink.UnadviseAll();

	if (m_pDatePickerCtrl)
	{
		m_pDatePickerCtrl->InternalRelease();
	}
	m_pDatePickerCtrl = pDatePickerCtrl;
	if (m_pDatePickerCtrl)
	{
		m_pDatePickerCtrl->InternalAddRef();

		m_pDatePickerCtrl->SetMaxSelCount(XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX * 7);

		AdviseToNotifications();

		SetFirstDayOfWeekToDatePicker();
	}

	if (m_pCalendarCtrl && m_pDatePickerCtrl)
	{
		m_eLastActiveView = -1; // to ensure update
		OnEvent_CalendarView(XTP_NC_CALENDARVIEWWASCHANGED, 0, 0);
	}
}

CXTPCalendarControl* CXTPCalendarController::GetCalendar() const
{
	return m_pCalendarCtrl;
}

CXTPDatePickerControl* CXTPCalendarController::GetDatePicker() const
{
	return m_pDatePickerCtrl;
}

void CXTPCalendarController::SetBoldDaysWithEvents(BOOL bBold)
{
	m_bBoldDaysWithEvents = bBold;

	if (m_pResourcesNf && !m_bBoldDaysWithEvents)
	{
		CMDTARGET_RELEASE(m_pResourcesNf);
		m_eLastActiveView = -1;
		_UnadviseFromDataChanged();

		m_DayInfoCache.Clear();
	}
}
