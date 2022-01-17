// XTPCalendarEventRecurrenceDlg.cpp: implementation of the CXTPCalendarEventPropertiesDlg.
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
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarEventRecurrenceDlg.h"
#include "XTPCalendarEventPropertiesDlg.h"

#include "XTPCalendarEvent.h"
#include "XTPCalendarRecurrencePattern.h"

#define XTP_TIMER_ID_UPDATE_TIMEOUT 100

#define XTP_TIMER_ID_START_CB_CHANGED       0x01
#define XTP_TIMER_ID_END_CB_CHANGED         0x02
#define XTP_TIMER_ID_DURATION_CB_CHANGED    0x04

#define XTP_TIMER_ID_ALL_CB_CHANGED         0x07

IMPLEMENT_DYNAMIC(CXTPCalendarEventRecurrenceDlg, CDialog)

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarEventRecurrenceDlg::CXTPCalendarEventRecurrenceDlg(CXTPCalendarEvent* pMasterEvent,
															   CWnd* pParent)
	:
	m_nOccurNum(10),
	m_dtEndDate(COleDateTime::GetCurrentTime()),
	m_nDayInterval(1),
	m_nWeeklyInterval(1),
	m_bMonday(TRUE),
	m_bTuesday(FALSE),
	m_bWednesday(FALSE),
	m_bThursday(FALSE),
	m_bFriday(FALSE),
	m_bSaturday(FALSE),
	m_bSunday(FALSE),
	m_MonthDate(1),
	m_nMonthInterval(1),
	m_nMonthInterval2(1),
	m_nDayOfMonth(m_dtEndDate.GetDay()),
	m_ptrMasterEvent(pMasterEvent, TRUE),
	m_bDisableRemove(FALSE)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(IDD), pParent);

	ASSERT(m_ptrMasterEvent);

	m_ptrPattern = m_ptrMasterEvent->GetRecurrencePattern();
	m_dtStart = m_ptrPattern->GetStartTime();
	m_dtEnd = m_dtStart + m_ptrPattern->GetDuration();

	m_dtStartDate = m_ptrPattern->GetPatternStartDate();
}

CXTPCalendarEventRecurrenceDlg::~CXTPCalendarEventRecurrenceDlg()
{
}

#if _MSC_VER < 1200
AFX_INLINE void DDX_DateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
		CXTPDateTimeHelper::GetDateTimeCtrlTime(hWndCtrl, value);
	else
		CXTPDateTimeHelper::SetDateTimeCtrlTime(hWndCtrl, value);
}
#endif

void CXTPCalendarEventRecurrenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, XTP_IDC_COMBO_SCHEDULEID, m_wndCbStart);
	DDX_Control(pDX, XTP_IDC_COMBO_REC_END, m_wndCbEnd);
	DDX_Control(pDX, XTP_IDC_COMBO_REC_DURATION, m_wndCbDuration);
	DDX_DateTimeCtrl(pDX, XTP_IDC_DATETIMEPICKER_START, m_dtStartDate);
	DDX_DateTimeCtrl(pDX, XTP_IDC_DATETIMEPICKER_ENDDATE, m_dtEndDate);
	DDX_Text(pDX, XTP_IDC_EDIT_OCCUR, m_nOccurNum);
	DDX_Text(pDX, XTP_IDC_EDIT_DAYINTERVAL, m_nDayInterval);
	DDX_Text(pDX, XTP_IDC_EDIT_WEEKLY_INTERVAL, m_nWeeklyInterval);
	DDX_Check(pDX, XTP_IDC_CHECK_MONDAY, m_bMonday);
	DDX_Check(pDX, XTP_IDC_CHECK_TUESDAY, m_bTuesday);
	DDX_Check(pDX, XTP_IDC_CHECK_WEDNESDAY, m_bWednesday);
	DDX_Check(pDX, XTP_IDC_CHECK_THURSDAY, m_bThursday);
	DDX_Check(pDX, XTP_IDC_CHECK_FRIDAY, m_bFriday);
	DDX_Check(pDX, XTP_IDC_CHECK_SATURDAY, m_bSaturday);
	DDX_Check(pDX, XTP_IDC_CHECK_SUNDAY, m_bSunday);
	DDX_Text(pDX, XTP_IDC_EDIT_MONTH_DATE, m_MonthDate);
	DDX_Text(pDX, XTP_IDC_EDIT_MONTH_MONTH, m_nMonthInterval);
	DDX_Text(pDX, XTP_IDC_EDIT_MONTH_MONTH2, m_nMonthInterval2);
	DDX_Control(pDX, XTP_IDC_COMBO_MONTH_DAY, m_wndCbMonthDay);
	DDX_Control(pDX, XTP_IDC_COMBO_MONTH_CRIT, m_wndCbMonthWhich);
	DDX_Text(pDX, XTP_IDC_EDIT_YEAR_DATE, m_nDayOfMonth);
	DDX_Control(pDX, XTP_IDC_COMBO_YEAR_MONTH, m_wndYearMonth);
	DDX_Control(pDX, XTP_IDC_COMBO_YEAR_MONTH2, m_wndComboYearMonth2);
	DDX_Control(pDX, XTP_IDC_COMBO_YEAR_CRIT, m_wndCbYearWhich);
	DDX_Control(pDX, XTP_IDC_COMBO_YEAR_DAY, m_wndCbYearDay);
}

BEGIN_MESSAGE_MAP(CXTPCalendarEventRecurrenceDlg, CDialog)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_SCHEDULEID, OnStartComboChanged)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_REC_END, OnEndComboChanged)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_REC_DURATION, OnDurationComboChanged)

	ON_CBN_KILLFOCUS(XTP_IDC_COMBO_SCHEDULEID, OnStartComboChanged)
	ON_CBN_KILLFOCUS(XTP_IDC_COMBO_REC_END, OnEndComboChanged)
	ON_CBN_KILLFOCUS(XTP_IDC_COMBO_REC_DURATION, OnDurationComboChanged)

	ON_CBN_EDITCHANGE(XTP_IDC_COMBO_SCHEDULEID, OnStartComboEdited)
	ON_CBN_EDITCHANGE(XTP_IDC_COMBO_REC_END, OnEndComboEdited)
	ON_CBN_EDITCHANGE(XTP_IDC_COMBO_REC_DURATION, OnDurationComboEdited)

	ON_BN_CLICKED(XTP_IDC_BUTTON_REMOVE_RECURRENCE, OnBnClickedButtonRemoveRecurrence)
	ON_CONTROL_RANGE(BN_CLICKED, XTP_IDC_RADIO_DAILY, XTP_IDC_RADIO_YEARLY, OnBnClickedRadioDailyYearly)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CXTPCalendarEventRecurrenceDlg::InitStartEndCB()
{
	BOOL bInListStart = FALSE;
	BOOL bInListEnd = FALSE;

	CString strTimeFormat_HrMin = CXTPCalendarUtils::GetTimeFormatString_HrMin();
	CString strTime;

	// initializing m_wndCbStart and m_wndCbEnd combo boxes
	for (int i = 0; i < 24*60; i += 30)
	{
		int nHours = i / 60;
		int nMinutes = i % 60;

		strTime = CXTPCalendarUtils::GetTimeFormat(nHours, nMinutes, strTimeFormat_HrMin);

		int nIndex = m_wndCbStart.AddString(strTime);

		if (m_dtStart.GetHour() == nHours && m_dtStart.GetMinute() == nMinutes)
		{
			m_wndCbStart.SetCurSel(nIndex);
			bInListStart = TRUE;
		}

		nIndex = m_wndCbEnd.AddString(strTime);

		if (m_dtEnd.GetHour() == nHours && m_dtEnd.GetMinute() == nMinutes)
		{
			m_wndCbEnd.SetCurSel(nIndex);
			bInListEnd = TRUE;
		}
	}

	if (!bInListStart)
	{
		strTime = CXTPCalendarUtils::GetTimeFormat(m_dtStart.GetHour(), m_dtStart.GetMinute(), strTimeFormat_HrMin);
		m_wndCbStart.SetWindowText(strTime);
	}

	if (!bInListEnd)
	{
		strTime = CXTPCalendarUtils::GetTimeFormat(m_dtEnd.GetHour(), m_dtEnd.GetMinute(), strTimeFormat_HrMin);
		m_wndCbEnd.SetWindowText(strTime);
	}
}

int CXTPCalendarEventRecurrenceDlg::_AddString(CComboBox& wndCB, UINT nStrResID, DWORD_PTR dwItemData)
{
	int nIndex = wndCB.AddString(CXTPCalendarUtils::LoadString(nStrResID));
	wndCB.SetItemData(nIndex, dwItemData);
	return nIndex;
}

int CXTPCalendarEventRecurrenceDlg::_AddLocaleString(CComboBox& wndCB, LCTYPE lcidStr, DWORD_PTR dwItemData)
{
	int nIndex = wndCB.AddString(CXTPCalendarUtils::GetLocaleString(lcidStr, 255));
	wndCB.SetItemData(nIndex, dwItemData);
	return nIndex;
}

void CXTPCalendarEventRecurrenceDlg::InitDurationCB()
{
	//initializing combo m_wndCbDuration combobox
	int nDuration_min = m_ptrPattern->GetDurationMinutes();

	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_MINUTES_0, 0);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_MINUTES_1, 1);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_MINUTES_5, 5);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_MINUTES_10, 10);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_MINUTES_15, 15);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_MINUTES_30, 30);

	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_1, 1 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_2, 2 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_3, 3 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_4, 4 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_5, 5 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_6, 6 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_7, 7 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_8, 8 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_9, 9 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_10, 10 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_11, 11 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_DAYS_0_5, 12 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_HOURS_18, 18 * 60);

	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_DAYS_1, 1 * 24 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_DAYS_2, 2 * 24 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_DAYS_3, 3 * 24 * 60);

	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_WEEKS_1, 1 * 7 * 24 * 60);
	_AddString(m_wndCbDuration, XTP_IDS_CALENDAR_WEEKS_2, 2 * 7 * 24 * 60);

	int nCount = m_wndCbDuration.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		if (nDuration_min == (int)m_wndCbDuration.GetItemData(i))
		{
			m_wndCbDuration.SetCurSel(i);
			return;
		}
	}

	CString strDuration = CXTPCalendarUtils::FormatTimeDuration(nDuration_min, FALSE);
	m_wndCbDuration.SetWindowText(strDuration);
}

void CXTPCalendarEventRecurrenceDlg::InitDayOfWeekCBs(CComboBox& wndCB)
{
	_AddLocaleString(wndCB, LOCALE_SDAYNAME1, xtpCalendarDayMonday);
	_AddLocaleString(wndCB, LOCALE_SDAYNAME2, xtpCalendarDayTuesday);
	_AddLocaleString(wndCB, LOCALE_SDAYNAME3, xtpCalendarDayWednesday);
	_AddLocaleString(wndCB, LOCALE_SDAYNAME4, xtpCalendarDayThursday);
	_AddLocaleString(wndCB, LOCALE_SDAYNAME5, xtpCalendarDayFriday);
	_AddLocaleString(wndCB, LOCALE_SDAYNAME6, xtpCalendarDaySaturday);
	_AddLocaleString(wndCB, LOCALE_SDAYNAME7, xtpCalendarDaySunday);

	wndCB.SetCurSel(0);
}

void CXTPCalendarEventRecurrenceDlg::InitWhichDayCBs(CComboBox& wndCB)
{
	_AddString(wndCB, XTP_IDS_CALENDAR_FIRST,   xtpCalendarWeekFirst);
	_AddString(wndCB, XTP_IDS_CALENDAR_SECOND,  xtpCalendarWeekSecond);
	_AddString(wndCB, XTP_IDS_CALENDAR_THIRD,   xtpCalendarWeekThird);
	_AddString(wndCB, XTP_IDS_CALENDAR_FOURTH,  xtpCalendarWeekFourth);
	_AddString(wndCB, XTP_IDS_CALENDAR_LAST,    xtpCalendarWeekLast);

	wndCB.SetCurSel(0);
}

void CXTPCalendarEventRecurrenceDlg::InitMonthCBs(CComboBox& wndCB)
{
	for (int nMonth = 0; nMonth < 12; nMonth++)
	{
		_AddLocaleString(wndCB, LOCALE_SMONTHNAME1 + nMonth, nMonth + 1);
	}

	COleDateTime dtNow(COleDateTime::GetCurrentTime());
	wndCB.SetCurSel(dtNow.GetMonth() - 1);
}

// CXTPCalendarEventRecurrenceDlg message handlers
BOOL CXTPCalendarEventRecurrenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitStartEndCB();
	InitDurationCB();

	//initializing recurrence end properties
	int nRadio = XTP_IDC_RADIO_NOEND;
	XTPCalendarPatternEnd UseEnd = (XTPCalendarPatternEnd)
										 m_ptrPattern->GetUseEndMethod();
	switch (UseEnd)
	{
	case xtpCalendarPatternEndDate:
		nRadio = XTP_IDC_RADIO_ENDBY;
		m_dtEndDate = m_ptrPattern->GetPatternEndDate();
		break;
	case xtpCalendarPatternEndAfterOccurrences:
		nRadio = XTP_IDC_RADIO_ENDAFTER;
		m_nOccurNum = m_ptrPattern->GetEndAfterOccurrences();
		break;
	case xtpCalendarPatternEndNoDate:
		nRadio = XTP_IDC_RADIO_NOEND;
		break;
	}
	CheckRadioButton(XTP_IDC_RADIO_NOEND, XTP_IDC_RADIO_ENDBY, nRadio);

	if (!m_bDisableRemove)
	{
		::EnableWindow(GetDlgItem(XTP_IDC_BUTTON_REMOVE_RECURRENCE)->GetSafeHwnd(), TRUE);
	}

	// initializing day/week/month/year recurrence settings
	CheckRadioButton(XTP_IDC_RADIO_EVERYDAY, XTP_IDC_RADIO_EVERYWORKDAY, XTP_IDC_RADIO_EVERYDAY);
	CheckRadioButton(XTP_IDC_RADIO_MONTH_RECUR, XTP_IDC_RADIO_MONTH_IN, XTP_IDC_RADIO_MONTH_RECUR);
	CheckRadioButton(XTP_IDC_RADIO_YEAR_RECUR, XTP_IDC_RADIO_YEAR_IN, XTP_IDC_RADIO_YEAR_RECUR);

	InitDayOfWeekCBs(m_wndCbMonthDay);
	InitDayOfWeekCBs(m_wndCbYearDay);

	InitWhichDayCBs(m_wndCbMonthWhich);
	InitWhichDayCBs(m_wndCbYearWhich);

	InitMonthCBs(m_wndYearMonth);
	InitMonthCBs(m_wndComboYearMonth2);

	nRadio = XTP_IDC_RADIO_WEEKLY;
	XTP_CALENDAR_RECURRENCE_OPTIONS opt = m_ptrPattern->GetRecurrenceOptions();
	int nRecType = m_ptrPattern->GetRecurrenceType();
	int i;
	switch (nRecType)
	{
	case xtpCalendarRecurrenceDaily:
		{
			nRadio = XTP_IDC_RADIO_DAILY;
			int nRd = XTP_IDC_RADIO_EVERYDAY;
			if (opt.m_Daily.bEveryWeekDayOnly)
			{
				m_nDayInterval = 1;
				nRd = XTP_IDC_RADIO_EVERYWORKDAY;
			}
			else
			{
				m_nDayInterval = opt.m_Daily.nIntervalDays;
			}
			CheckRadioButton(XTP_IDC_RADIO_EVERYDAY, XTP_IDC_RADIO_EVERYWORKDAY, nRd);
		}
		break;
	case xtpCalendarRecurrenceWeekly:
		nRadio = XTP_IDC_RADIO_WEEKLY;
		m_nWeeklyInterval = opt.m_Weekly.nIntervalWeeks;
		m_bMonday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDayMonday) != 0;
		m_bTuesday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDayTuesday) != 0;
		m_bWednesday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDayWednesday) != 0;
		m_bThursday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDayThursday) != 0;
		m_bFriday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDayFriday) != 0;
		m_bSaturday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDaySaturday) != 0;
		m_bSunday = (opt.m_Weekly.nDayOfWeekMask & xtpCalendarDaySunday) != 0;
		break;
	case xtpCalendarRecurrenceMonthly:
		nRadio = XTP_IDC_RADIO_MONTHLY;
		CheckRadioButton(XTP_IDC_RADIO_MONTH_RECUR, XTP_IDC_RADIO_MONTH_IN,
						 XTP_IDC_RADIO_MONTH_RECUR);
		m_nMonthInterval = opt.m_Monthly.nIntervalMonths;
		m_MonthDate = opt.m_Monthly.nDayOfMonth;
		break;
	case xtpCalendarRecurrenceMonthNth:
		{
			nRadio = XTP_IDC_RADIO_MONTHLY;
			CheckRadioButton(XTP_IDC_RADIO_MONTH_RECUR, XTP_IDC_RADIO_MONTH_IN,
							 XTP_IDC_RADIO_MONTH_IN);
			m_nMonthInterval2 = opt.m_MonthNth.nIntervalMonths;

			for (i = 0; i < m_wndCbMonthDay.GetCount(); i++)
				if (opt.m_MonthNth.nWhichDayMask & m_wndCbMonthDay.GetItemData(i))
				{
					m_wndCbMonthDay.SetCurSel(i);
					break;
				}
			for (i = 0; i < m_wndCbMonthWhich.GetCount(); i++)
				if (opt.m_MonthNth.nWhichDay == (int) m_wndCbMonthWhich.GetItemData(i))
				{
					m_wndCbMonthWhich.SetCurSel(i);
					break;
				}
		}
		break;
	case xtpCalendarRecurrenceYearly:
		{
			nRadio = XTP_IDC_RADIO_YEARLY;
			CheckRadioButton(XTP_IDC_RADIO_YEAR_RECUR, XTP_IDC_RADIO_YEAR_IN,
							 XTP_IDC_RADIO_YEAR_RECUR);
			m_nDayOfMonth = opt.m_Yearly.nDayOfMonth;
			for (i = 0; i < m_wndYearMonth.GetCount(); i++)
			{
				if (opt.m_Yearly.nMonthOfYear == (int) m_wndYearMonth.GetItemData(i))
				{
					m_wndYearMonth.SetCurSel(i);
					break;
				}
			}
		}
		break;
	case xtpCalendarRecurrenceYearNth:
		nRadio = XTP_IDC_RADIO_YEARLY;
		CheckRadioButton(XTP_IDC_RADIO_YEAR_RECUR, XTP_IDC_RADIO_YEAR_IN, XTP_IDC_RADIO_YEAR_IN);

		for (i = 0; i < m_wndComboYearMonth2.GetCount(); i++)
			if (opt.m_YearNth.nMonthOfYear == (int) m_wndComboYearMonth2.GetItemData(i))
			{
				m_wndComboYearMonth2.SetCurSel(i);
				break;
			}

		for (i = 0; i < m_wndCbYearWhich.GetCount(); i++)
			if (opt.m_YearNth.nWhichDay == (int) m_wndCbYearWhich.GetItemData(i))
			{
				m_wndCbYearWhich.SetCurSel(i);
				break;
			}

		for (i = 0; i < m_wndCbYearDay.GetCount(); i++)
			if (opt.m_YearNth.nWhichDayMask & m_wndCbYearDay.GetItemData(i))
			{
				m_wndCbYearDay.SetCurSel(i);
				break;
			}
		break;
	default:
		ASSERT(nRecType == xtpCalendarRecurrenceUnknown);
	}
	CheckRadioButton(XTP_IDC_RADIO_DAILY, XTP_IDC_RADIO_YEARLY, nRadio);
	OnBnClickedRadioDailyYearly(nRadio);

	UpdateData(FALSE);

	_MoveControlsBy_Y();

	return TRUE;
}
void CXTPCalendarEventRecurrenceDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent & XTP_TIMER_ID_ALL_CB_CHANGED)
	{
		KillTimer(nIDEvent);
	}
	else
	{
		ASSERT(FALSE);
		CDialog::OnTimer(nIDEvent);
	}

	if (nIDEvent & XTP_TIMER_ID_START_CB_CHANGED)
	{
		//TRACE(_T("_OnStartChanged() \n"));
		_OnStartChanged();
	}
	if (nIDEvent & XTP_TIMER_ID_END_CB_CHANGED)
	{
		//TRACE(_T("_OnEndChanged() \n"));
		_OnEndChanged();
	}
	if (nIDEvent & XTP_TIMER_ID_DURATION_CB_CHANGED)
	{
		//TRACE(_T("_OnDurationChanged() \n"));
		_OnDurationChanged();
	}
}

void CXTPCalendarEventRecurrenceDlg::OnStartChanged()
{
	SetTimer(XTP_TIMER_ID_START_CB_CHANGED, XTP_TIMER_ID_UPDATE_TIMEOUT, NULL);
}

void CXTPCalendarEventRecurrenceDlg::OnEndChanged()
{
	SetTimer(XTP_TIMER_ID_END_CB_CHANGED, XTP_TIMER_ID_UPDATE_TIMEOUT, NULL);
}

void CXTPCalendarEventRecurrenceDlg::OnDurationChanged()
{
	SetTimer(XTP_TIMER_ID_DURATION_CB_CHANGED, XTP_TIMER_ID_UPDATE_TIMEOUT, NULL);
}

int CXTPCalendarEventRecurrenceDlg::GetChangedComboTimeInMin(CComboBox& wndCb)
{
	CString strCBtext;
	wndCb.GetWindowText(strCBtext);

	return CXTPCalendarUtils::ParceTimeString_min(strCBtext);
}

void CXTPCalendarEventRecurrenceDlg::OnStartComboEdited()
{
	int nMin = GetChangedComboTimeInMin(m_wndCbStart);
	if (nMin >= 0)
		OnStartChanged();
}

void CXTPCalendarEventRecurrenceDlg::OnEndComboEdited()
{
	int nMin = GetChangedComboTimeInMin(m_wndCbEnd);
	if (nMin >= 0)
		OnEndChanged();
}

void CXTPCalendarEventRecurrenceDlg::OnDurationComboEdited()
{
	int nMin = GetDurationComboInMin();
	if (nMin >= 0)
		OnDurationChanged();
}

void CXTPCalendarEventRecurrenceDlg::OnStartComboChanged()
{
	OnStartChanged();
}

void CXTPCalendarEventRecurrenceDlg::_OnStartChanged()
{
	int nMin = GetChangedComboTimeInMin(m_wndCbStart);
	if (nMin == -1)
	{
		return;
	}

	m_dtStart.SetTime(nMin/60%24, nMin%60, 0);

	int nDurationMin = GetDurationComboInMin();
	if (nDurationMin < 0)
	{
		nDurationMin = 0;
	}

	COleDateTimeSpan spDiff((double)nDurationMin/((double)24*60));
	m_dtEnd = m_dtStart + spDiff;
	m_dtEnd = CXTPCalendarUtils::ResetDate(m_dtEnd);

	CString strTimeFormat_HrMin = CXTPCalendarUtils::GetTimeFormatString_HrMin();
	CString strTmp = CXTPCalendarUtils::GetTimeFormat(m_dtEnd.GetHour(), m_dtEnd.GetMinute(), strTimeFormat_HrMin);
	m_wndCbEnd.SetWindowText(strTmp);
}

void CXTPCalendarEventRecurrenceDlg::OnEndComboChanged()
{
	OnEndChanged();
}

void CXTPCalendarEventRecurrenceDlg::_OnEndChanged()
{
	int nMin = GetChangedComboTimeInMin(m_wndCbEnd);
	if (nMin == -1)
	{
		return;
	}

	COleDateTime dtPrevEnd = m_dtEnd;
	m_dtEnd.SetTime(nMin/60%24, nMin%60, 0);

	COleDateTimeSpan spDiff = m_dtEnd - dtPrevEnd;

	int nDuration = GetDurationComboInMin();
	if (nDuration < 0)
	{
		nDuration = 0;
	}
	nDuration += GETTOTAL_MINUTES_DTS(spDiff);

	if (m_dtEnd < m_dtStart && nDuration < 0)
	{
		spDiff = m_dtStart - m_dtEnd;
		nDuration = 24*60 - GETTOTAL_MINUTES_DTS(spDiff);
	}

	CString strDuration = CXTPCalendarUtils::FormatTimeDuration(nDuration, FALSE);
	m_wndCbDuration.SetWindowText(strDuration);
}

int CXTPCalendarEventRecurrenceDlg::GetDurationComboInMin()
{
	CString strCBtext;
	m_wndCbDuration.GetWindowText(strCBtext);

	return CXTPCalendarUtils::ParceTimeDuration_min(strCBtext);
}

void CXTPCalendarEventRecurrenceDlg::OnDurationComboChanged()
{
	OnDurationChanged();
}

void CXTPCalendarEventRecurrenceDlg::_OnDurationChanged()
{
	int nDuration_min = GetDurationComboInMin();
	if (nDuration_min < 0)
	{
		return;
	}

	m_dtEnd = m_dtStart + CXTPCalendarUtils::Minutes2Span(nDuration_min);

	CString strTimeFormat_HrMin = CXTPCalendarUtils::GetTimeFormatString_HrMin();
	CString strTmp = CXTPCalendarUtils::GetTimeFormat(m_dtEnd.GetHour(), m_dtEnd.GetMinute(), strTimeFormat_HrMin);

	m_wndCbEnd.SetWindowText(strTmp);
}

void CXTPCalendarEventRecurrenceDlg::MsgBox_WrongValue(CWnd* pWnd)
{
	pWnd->SetFocus();
	XTPResourceManager()->ShowMessageBox(XTP_IDS_CALENDAR_WRONG_VALUE);
}

void CXTPCalendarEventRecurrenceDlg::MsgBox_WrongValueRange(UINT nCtrlID, int nMin, int nMax)
{
	if (GetDlgItem(nCtrlID))
		GetDlgItem(nCtrlID)->SetFocus();

	CString strFormat = CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_WRONG_VALUE_RANGE);
	CString strMsg, strVal1, strVal2;
	strVal1.Format(_T("%d"), nMin);
	strVal2.Format(_T("%d"), nMax);

	strMsg.Format(strFormat, (LPCTSTR)strVal1, (LPCTSTR)strVal2);

	AfxMessageBox(strMsg);
}

void CXTPCalendarEventRecurrenceDlg::OnOK()
{
	int nMin = GetChangedComboTimeInMin(m_wndCbStart);
	if (nMin == -1)
	{
		MsgBox_WrongValue(&m_wndCbStart);
		return;
	}

	nMin = GetChangedComboTimeInMin(m_wndCbEnd);
	if (nMin == -1)
	{
		MsgBox_WrongValue(&m_wndCbEnd);
		return;
	}

	int nDuration_min = GetDurationComboInMin();
	if (nDuration_min < 0)
	{
		MsgBox_WrongValue(&m_wndCbDuration);
		return;
	}

	UpdateData();

	CXTPCalendarEventsPtr ptrExceptions = m_ptrPattern->GetExceptions();
	if (ptrExceptions && ptrExceptions->GetCount())
	{
		// Any exceptions associated with this recurring appointment will be lost. If any of the exceptions are meetings, the attendees will not be notified. Is this OK?
		int nRes = XTPResourceManager()->ShowMessageBox(XTP_IDS_CALENDAR_REMOVE_EXCEPTIONS, MB_OKCANCEL);
		if (nRes != IDOK)
		{
			return;
		}
		m_ptrPattern->RemoveAllExceptions();
	}

	m_dtStart = CXTPCalendarUtils::ResetDate(m_dtStart);
	m_ptrPattern->SetStartTime(m_dtStart);

	m_ptrPattern->SetDurationMinutes(nDuration_min);

	//----------------------------------------------------------
	XTP_CALENDAR_RECURRENCE_OPTIONS opt;

	int nRadio = GetCheckedRadioButton(XTP_IDC_RADIO_DAILY, XTP_IDC_RADIO_YEARLY);
	switch (nRadio)
	{
	case XTP_IDC_RADIO_DAILY:
		{
			if (m_nDayInterval < 1 || m_nDayInterval > 999)
			{
				MsgBox_WrongValueRange(XTP_IDC_EDIT_DAYINTERVAL, 1, 999);
				return;
			}

			opt.m_nRecurrenceType = xtpCalendarRecurrenceDaily;
			opt.m_Daily.bEveryWeekDayOnly = GetCheckedRadioButton(XTP_IDC_RADIO_EVERYDAY,
																  XTP_IDC_RADIO_EVERYWORKDAY) ==
											XTP_IDC_RADIO_EVERYWORKDAY;
			if (opt.m_Daily.bEveryWeekDayOnly)
				opt.m_Daily.nIntervalDays = 1;
			else
				opt.m_Daily.nIntervalDays = m_nDayInterval;
			break;
		}
	case XTP_IDC_RADIO_WEEKLY:

		if (m_nWeeklyInterval < 1 || m_nWeeklyInterval > 999)
		{
			MsgBox_WrongValueRange(XTP_IDC_EDIT_WEEKLY_INTERVAL, 1, 999);
			return;
		}

		opt.m_nRecurrenceType = xtpCalendarRecurrenceWeekly;
		opt.m_Weekly.nIntervalWeeks = m_nWeeklyInterval;
		opt.m_Weekly.nDayOfWeekMask = 0;
		if (m_bMonday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDayMonday;
		if (m_bTuesday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDayTuesday;
		if (m_bWednesday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDayWednesday;
		if (m_bThursday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDayThursday;
		if (m_bFriday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDayFriday;
		if (m_bSaturday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDaySaturday;
		if (m_bSunday)
			opt.m_Weekly.nDayOfWeekMask |= xtpCalendarDaySunday;

		break;
	case XTP_IDC_RADIO_MONTHLY:
		if (GetCheckedRadioButton(XTP_IDC_RADIO_MONTH_RECUR, XTP_IDC_RADIO_MONTH_IN) ==
			XTP_IDC_RADIO_MONTH_RECUR)
		{
			if (m_nMonthInterval < 1 || m_nMonthInterval > 999)
			{
				MsgBox_WrongValueRange(XTP_IDC_EDIT_MONTH_MONTH, 1, 999);
				return;
			}
			if (m_MonthDate < 1 || m_MonthDate > 31)
			{
				MsgBox_WrongValueRange(XTP_IDC_EDIT_MONTH_DATE, 1, 31);
				return;
			}

			opt.m_nRecurrenceType = xtpCalendarRecurrenceMonthly;
			opt.m_Monthly.nIntervalMonths = m_nMonthInterval;
			opt.m_Monthly.nDayOfMonth = m_MonthDate;
		}
		else
		{
			if (m_nMonthInterval2 < 1 || m_nMonthInterval2 > 999)
			{
				MsgBox_WrongValueRange(XTP_IDC_EDIT_MONTH_MONTH2, 1, 999);
				return;
			}

			opt.m_MonthNth.nIntervalMonths = m_nMonthInterval2;
			opt.m_nRecurrenceType = xtpCalendarRecurrenceMonthNth;
			opt.m_MonthNth.nWhichDayMask = (int)m_wndCbMonthDay.GetItemData(m_wndCbMonthDay.GetCurSel());

			opt.m_MonthNth.nWhichDay = (int)m_wndCbMonthWhich.GetItemData(m_wndCbMonthWhich.GetCurSel());
		}

		break;
	case XTP_IDC_RADIO_YEARLY:
		opt.m_nRecurrenceType = xtpCalendarRecurrenceYearly;
		if (GetCheckedRadioButton(XTP_IDC_RADIO_YEAR_RECUR, XTP_IDC_RADIO_YEAR_IN) ==
			XTP_IDC_RADIO_YEAR_RECUR)
		{
			if (m_nDayOfMonth < 1 || m_nDayOfMonth > 31)
			{
				MsgBox_WrongValueRange(XTP_IDC_EDIT_YEAR_DATE, 1, 31);
				return;
			}

			opt.m_Yearly.nDayOfMonth = m_nDayOfMonth;
			opt.m_Yearly.nMonthOfYear = (int)m_wndYearMonth.GetItemData(m_wndYearMonth.GetCurSel());
		}
		else
		{
			opt.m_nRecurrenceType = xtpCalendarRecurrenceYearNth;

			opt.m_YearNth.nMonthOfYear = (int)m_wndComboYearMonth2.GetItemData(m_wndComboYearMonth2.GetCurSel());

			opt.m_YearNth.nWhichDay = (int)m_wndCbYearWhich.GetItemData(m_wndCbYearWhich.GetCurSel());

			opt.m_YearNth.nWhichDayMask = (int)m_wndCbYearDay.GetItemData(m_wndCbYearDay.GetCurSel());
		}

		break;
	}

	m_ptrPattern->SetRecurrenceOptions(opt);

	//-----------------------------------------------------------------------
	m_dtStartDate = CXTPCalendarUtils::ResetTime(m_dtStartDate);
	m_ptrPattern->SetPatternStartDate(m_dtStartDate);

	nRadio = GetCheckedRadioButton(XTP_IDC_RADIO_NOEND, XTP_IDC_RADIO_ENDBY);
	switch (nRadio)
	{
	case XTP_IDC_RADIO_ENDBY:
		if (m_dtEndDate < m_dtStartDate)
		{
			MsgBox_WrongValue(GetDlgItem(XTP_IDC_DATETIMEPICKER_ENDDATE));
			return;
		}
		m_ptrPattern->SetPatternEndDate(m_dtEndDate);
		break;

	case XTP_IDC_RADIO_ENDAFTER:
		if (m_nOccurNum < 1 || m_nOccurNum > 999)
		{
			MsgBox_WrongValueRange(XTP_IDC_EDIT_OCCUR, 1, 999);
			return;
		}
		m_ptrPattern->SetEndAfterOccurrences(m_nOccurNum);
		break;
	case XTP_IDC_RADIO_NOEND:
		m_ptrPattern->SetNoEndDate();
		break;
	}

	//-----------------------------------------------------------------------
	VERIFY(m_ptrMasterEvent->UpdateRecurrence(m_ptrPattern));

	CDialog::OnOK();
}

void CXTPCalendarEventRecurrenceDlg::OnBnClickedButtonRemoveRecurrence()
{
	VERIFY(m_ptrMasterEvent->RemoveRecurrence());

	EndDialog(XTP_CALENDAR_DLGRESULT_REMOVE_RECURRENCE);
}

void CXTPCalendarEventRecurrenceDlg::OnBnClickedRadioDailyYearly(UINT nID)
{
	ShowWindow(XTP_IDC_RADIO_EVERYDAY, FALSE);
	ShowWindow(XTP_IDC_RADIO_EVERYWORKDAY, FALSE);
	ShowWindow(XTP_IDC_EDIT_DAYINTERVAL, FALSE);
	ShowWindow(XTP_IDC_STATIC_DAY, FALSE);

	ShowWindow(XTP_IDC_STATIC_WEEKLY_RECEVERY, FALSE);
	ShowWindow(XTP_IDC_EDIT_WEEKLY_INTERVAL, FALSE);
	ShowWindow(XTP_IDC_STATIC_WEEKLY_FOLLDAYS, FALSE);
	ShowWindow(XTP_IDC_CHECK_MONDAY, FALSE);
	ShowWindow(XTP_IDC_CHECK_TUESDAY, FALSE);
	ShowWindow(XTP_IDC_CHECK_WEDNESDAY, FALSE);
	ShowWindow(XTP_IDC_CHECK_THURSDAY, FALSE);
	ShowWindow(XTP_IDC_CHECK_FRIDAY, FALSE);
	ShowWindow(XTP_IDC_CHECK_SATURDAY, FALSE);
	ShowWindow(XTP_IDC_CHECK_SUNDAY, FALSE);

	ShowWindow(XTP_IDC_RADIO_MONTH_RECUR, FALSE);
	ShowWindow(XTP_IDC_RADIO_MONTH_IN, FALSE);
	ShowWindow(XTP_IDC_EDIT_MONTH_DATE, FALSE);
	ShowWindow(XTP_IDC_STATIC_MONTH_NUMEVERY, FALSE);
	ShowWindow(XTP_IDC_EDIT_MONTH_MONTH, FALSE);
	ShowWindow(XTP_IDC_STATIC_MONTH_MONTH, FALSE);
	ShowWindow(XTP_IDC_COMBO_MONTH_CRIT, FALSE);
	ShowWindow(XTP_IDC_COMBO_MONTH_DAY, FALSE);
	ShowWindow(XTP_IDC_STATIC_MONTH_EVERY, FALSE);
	ShowWindow(XTP_IDC_EDIT_MONTH_MONTH2, FALSE);
	ShowWindow(XTP_IDC_STATIC_MONTH_MONTH2, FALSE);

	ShowWindow(XTP_IDC_RADIO_YEAR_RECUR, FALSE);
	ShowWindow(XTP_IDC_RADIO_YEAR_IN, FALSE);
	ShowWindow(XTP_IDC_EDIT_YEAR_DATE, FALSE);
	ShowWindow(XTP_IDC_COMBO_YEAR_MONTH, FALSE);
	ShowWindow(XTP_IDC_COMBO_YEAR_CRIT, FALSE);
	ShowWindow(XTP_IDC_COMBO_YEAR_DAY, FALSE);
	ShowWindow(XTP_IDC_COMBO_YEAR_MONTH2, FALSE);
	ShowWindow(XTP_IDC_STATIC_YEAR_OF, FALSE);

	switch (nID)
	{
	case XTP_IDC_RADIO_DAILY:
		ShowWindow(XTP_IDC_RADIO_EVERYDAY);
		ShowWindow(XTP_IDC_RADIO_EVERYWORKDAY);
		ShowWindow(XTP_IDC_EDIT_DAYINTERVAL);
		ShowWindow(XTP_IDC_STATIC_DAY);
		break;
	case XTP_IDC_RADIO_WEEKLY:
		ShowWindow(XTP_IDC_STATIC_WEEKLY_RECEVERY);
		ShowWindow(XTP_IDC_EDIT_WEEKLY_INTERVAL);
		ShowWindow(XTP_IDC_STATIC_WEEKLY_FOLLDAYS);
		ShowWindow(XTP_IDC_CHECK_MONDAY);
		ShowWindow(XTP_IDC_CHECK_TUESDAY);
		ShowWindow(XTP_IDC_CHECK_WEDNESDAY);
		ShowWindow(XTP_IDC_CHECK_THURSDAY);
		ShowWindow(XTP_IDC_CHECK_FRIDAY);
		ShowWindow(XTP_IDC_CHECK_SATURDAY);
		ShowWindow(XTP_IDC_CHECK_SUNDAY);
		break;
	case XTP_IDC_RADIO_MONTHLY:
		ShowWindow(XTP_IDC_RADIO_MONTH_RECUR);
		ShowWindow(XTP_IDC_RADIO_MONTH_IN);
		ShowWindow(XTP_IDC_EDIT_MONTH_DATE);
		ShowWindow(XTP_IDC_STATIC_MONTH_NUMEVERY);
		ShowWindow(XTP_IDC_EDIT_MONTH_MONTH);
		ShowWindow(XTP_IDC_STATIC_MONTH_MONTH);
		ShowWindow(XTP_IDC_COMBO_MONTH_CRIT);
		ShowWindow(XTP_IDC_COMBO_MONTH_DAY);
		ShowWindow(XTP_IDC_STATIC_MONTH_EVERY);
		ShowWindow(XTP_IDC_EDIT_MONTH_MONTH2);
		ShowWindow(XTP_IDC_STATIC_MONTH_MONTH2);
		break;
	case XTP_IDC_RADIO_YEARLY:
		ShowWindow(XTP_IDC_RADIO_YEAR_RECUR);
		ShowWindow(XTP_IDC_RADIO_YEAR_IN);
		ShowWindow(XTP_IDC_EDIT_YEAR_DATE);
		ShowWindow(XTP_IDC_COMBO_YEAR_MONTH);
		ShowWindow(XTP_IDC_COMBO_YEAR_CRIT);
		ShowWindow(XTP_IDC_COMBO_YEAR_DAY);
		ShowWindow(XTP_IDC_COMBO_YEAR_MONTH2);
		ShowWindow(XTP_IDC_STATIC_YEAR_OF);
		break;
	}
}

CRect CXTPCalendarEventRecurrenceDlg::GetCtrlRect(int nID)
{
	ASSERT(GetDlgItem(nID));

	CRect rcRect(0, 0, 0, 0);

	if (GetDlgItem(nID))
		GetDlgItem(nID)->GetWindowRect(&rcRect);

	return rcRect;
}

void CXTPCalendarEventRecurrenceDlg::MoveWindow_Y(int nID, int nYOffset)
{
	if (GetDlgItem(nID))
	{
		CRect rcRect = GetCtrlRect(nID);
		ScreenToClient(&rcRect);

		rcRect.top += nYOffset;
		rcRect.bottom += nYOffset;

		GetDlgItem(nID)->MoveWindow(rcRect);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CXTPCalendarEventRecurrenceDlg::_MoveControlsBy_Y()
{
	CRect rcBase = GetCtrlRect(XTP_IDC_RADIO_DAILY);
	rcBase.top += 10;

	CRect rcBase2 = GetCtrlRect(XTP_IDC_RADIO_EVERYDAY);
	int nYOffset = rcBase.top - rcBase2.top;

	MoveWindow_Y(XTP_IDC_RADIO_EVERYDAY,    nYOffset);
	MoveWindow_Y(XTP_IDC_RADIO_EVERYWORKDAY,nYOffset);
	MoveWindow_Y(XTP_IDC_EDIT_DAYINTERVAL,  nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_DAY,        nYOffset);

	rcBase2 = GetCtrlRect(XTP_IDC_STATIC_WEEKLY_RECEVERY);
	nYOffset = rcBase.top - rcBase2.top;

	MoveWindow_Y(XTP_IDC_STATIC_WEEKLY_RECEVERY,nYOffset);
	MoveWindow_Y(XTP_IDC_EDIT_WEEKLY_INTERVAL,  nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_WEEKLY_FOLLDAYS,nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_MONDAY,          nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_TUESDAY,         nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_WEDNESDAY,       nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_THURSDAY,        nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_FRIDAY,          nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_SATURDAY,        nYOffset);
	MoveWindow_Y(XTP_IDC_CHECK_SUNDAY,          nYOffset);

	rcBase2 = GetCtrlRect(XTP_IDC_RADIO_MONTH_RECUR);
	nYOffset = rcBase.top - rcBase2.top;

	MoveWindow_Y(XTP_IDC_RADIO_MONTH_RECUR,     nYOffset);
	MoveWindow_Y(XTP_IDC_RADIO_MONTH_IN,        nYOffset);
	MoveWindow_Y(XTP_IDC_EDIT_MONTH_DATE,       nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_MONTH_NUMEVERY, nYOffset);
	MoveWindow_Y(XTP_IDC_EDIT_MONTH_MONTH,      nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_MONTH_MONTH,    nYOffset);
	MoveWindow_Y(XTP_IDC_COMBO_MONTH_CRIT,      nYOffset);
	MoveWindow_Y(XTP_IDC_COMBO_MONTH_DAY,       nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_MONTH_EVERY,    nYOffset);
	MoveWindow_Y(XTP_IDC_EDIT_MONTH_MONTH2,     nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_MONTH_MONTH2,   nYOffset);

	rcBase2 = GetCtrlRect(XTP_IDC_RADIO_YEAR_RECUR);
	nYOffset = rcBase.top - rcBase2.top;

	MoveWindow_Y(XTP_IDC_RADIO_YEAR_RECUR,  nYOffset);
	MoveWindow_Y(XTP_IDC_RADIO_YEAR_IN,     nYOffset);
	MoveWindow_Y(XTP_IDC_EDIT_YEAR_DATE,    nYOffset);
	MoveWindow_Y(XTP_IDC_COMBO_YEAR_MONTH,  nYOffset);
	MoveWindow_Y(XTP_IDC_COMBO_YEAR_CRIT,   nYOffset);
	MoveWindow_Y(XTP_IDC_COMBO_YEAR_DAY,    nYOffset);
	MoveWindow_Y(XTP_IDC_COMBO_YEAR_MONTH2, nYOffset);
	MoveWindow_Y(XTP_IDC_STATIC_YEAR_OF,    nYOffset);

	//------------------------------------------------------------------------
	CRect rcDialog;
	GetWindowRect(&rcDialog);

	CRect rcOK = GetCtrlRect(IDOK);
	rcDialog.bottom = rcOK.bottom + 10;

	SetWindowPos(NULL, 0, 0, rcDialog.Width(), rcDialog.Height(), SWP_NOMOVE | SWP_NOZORDER);
}
