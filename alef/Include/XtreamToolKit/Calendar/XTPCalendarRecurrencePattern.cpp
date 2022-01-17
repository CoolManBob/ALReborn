// XTPCalendarRecurrencePattern.cpp: implementation of the CXTPCalendarRecurrencePattern class.
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

#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarEvents.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarUtils.h"
#include "XTPCalendarData.h"
#include "XTPCalendarCustomProperties.h"
#include "XTPCalendarRemindersManager.h"

static LPCTSTR cszRPatt_PatternID = _T("PatternID");
static LPCTSTR cszRPatt_StartTime = _T("StartTime");

static LPCTSTR cszRPatt_DurationMinutes = _T("DurationMinutes");
static LPCTSTR cszRPatt_RecurrenceType = _T("RecurrenceType");
static LPCTSTR cszRPatt_RecurrenceOpt_Data1 = _T("RecurrenceOpt_Data1");
static LPCTSTR cszRPatt_RecurrenceOpt_Data2 = _T("RecurrenceOpt_Data2");
static LPCTSTR cszRPatt_RecurrenceOpt_Data3 = _T("RecurrenceOpt_Data3");

static LPCTSTR cszRPatt_PatternStartDate = _T("PatternStartDate");
static LPCTSTR cszRPatt_UseEndMethod = _T("UseEndMethod");
static LPCTSTR cszRPatt_PatternEndDate = _T("PatternEndDate");
static LPCTSTR cszRPatt_EndAfterOccurrences = _T("EndAfterOccurrences");

static LPCTSTR cszRPatt_MasterEventID = _T("MasterEventID");

//===========================================================================
static const LPCTSTR cszOccRMD_Count        = _T("*xtp*OccRMD_Count");

static const LPCTSTR cszOccRMD_StartTime    = _T("*xtp*OccRMD_Start");
static const LPCTSTR cszOccRMD_EndTime      = _T("*xtp*OccRMD_End");

static const LPCTSTR cszOccRMD_IsReminder                 = _T("*xtp*OccRMD_IsReminder");
static const LPCTSTR cszOccRMD_NextReminderTime_Snoozed = _T("*xtp*OccRMD_NextReminderTime_Snoozed");
//===========================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////
#define XTP_RECPATT_PROP_SET_DATA_VER 1
////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CXTPCalendarRecurrencePattern, CCmdTarget)

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarRecurrencePattern::CXTPCalendarRecurrencePattern(CXTPCalendarData* pDataProvider)
{
	ASSERT(pDataProvider);

	m_ptrDataProvider = pDataProvider;
	CMDTARGET_ADDREF(m_ptrDataProvider);

	m_dwPatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID;
	m_dwMasterEventID = XTP_CALENDAR_UNKNOWN_EVENT_ID;

	m_dtStartTime = (DATE)0; //.SetStatus(COleDateTime::null);
	m_nDurationMinutes = 0;

	m_Options.m_nRecurrenceType = xtpCalendarRecurrenceUnknown;

	m_dtPatternStartDate = (DATE)0; //.SetStatus(COleDateTime::null);

	m_pCustomProperties = new CXTPCalendarCustomProperties;

}

CXTPCalendarRecurrencePattern::~CXTPCalendarRecurrencePattern()
{
	CMDTARGET_RELEASE(m_ptrDataProvider);
	CMDTARGET_RELEASE(m_pCustomProperties);
}

COleDateTime CXTPCalendarRecurrencePattern::GetStartOccurrenceTime(COleDateTime dtOccurrenceStartDate) const
{
	COleDateTime dtStartOcc = GetStartTime();
	dtStartOcc = CXTPCalendarUtils::UpdateDate(dtStartOcc, dtOccurrenceStartDate);

	return dtStartOcc;
}

COleDateTime CXTPCalendarRecurrencePattern::GetEndOccurrenceTime(COleDateTime dtOccurrenceStartDate) const
{
	COleDateTimeSpan spDuration = GetDuration();
	COleDateTime dtEndOcc = GetStartOccurrenceTime(dtOccurrenceStartDate);

	dtEndOcc += spDuration;

	return dtEndOcc;
}

void CXTPCalendarRecurrencePattern::_GetOccurrenceStartEnd(const COleDateTime& dtOccStartDate, COleDateTime& rdtOccStart, COleDateTime& rdtOccEnd) const
{
	rdtOccStart = GetStartTime();
	rdtOccStart = CXTPCalendarUtils::UpdateDate(rdtOccStart, dtOccStartDate);

	COleDateTimeSpan spDuration = GetDuration();
	rdtOccEnd = rdtOccStart + spDuration;
}

void CXTPCalendarRecurrencePattern::SetPatternStartDate(COleDateTime dtStart)
{
	m_dtPatternStartDate = CXTPCalendarUtils::ResetTime(dtStart);

	if (m_dtPatternStartDate.GetStatus() == COleDateTime::valid &&
			m_PatternEnd.m_dtPatternEndDate.GetStatus() == COleDateTime::valid &&
			m_dtPatternStartDate > m_PatternEnd.m_dtPatternEndDate)
	{
		m_PatternEnd.m_dtPatternEndDate = m_dtPatternStartDate;
	}
}

void CXTPCalendarRecurrencePattern::SetPatternEndDate(COleDateTime dtEnd)
{
	m_PatternEnd.m_dtPatternEndDate = CXTPCalendarUtils::ResetTime(dtEnd);
	m_PatternEnd.m_nUseEnd = xtpCalendarPatternEndDate;

	if (m_dtPatternStartDate.GetStatus() == COleDateTime::valid &&
			m_PatternEnd.m_dtPatternEndDate.GetStatus() == COleDateTime::valid &&
			m_PatternEnd.m_dtPatternEndDate < m_dtPatternStartDate)
	{
		m_dtPatternStartDate = m_PatternEnd.m_dtPatternEndDate;
	}
}

BOOL CXTPCalendarRecurrencePattern::GetOccurrences(CXTPCalendarEvents* pEventsArray,
		COleDateTime dtStart, COleDateTime dtEnd,
		CXTPCalendarEvent* pMasterEvent)
{
	if (!pMasterEvent || !pEventsArray)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//------
	COleDateTime dtStartOcc, dtEndOcc;
	_GetOccurrenceStartEnd(dtStart, dtStartOcc, dtEndOcc);

	int nOccDays = CXTPCalendarUtils::GetEventPeriodDays(dtStartOcc, dtEndOcc);
	ASSERT(nOccDays > 0);
	nOccDays = max(1, nOccDays);

	COleDateTime dtStartCheckOcc = dtStart;

	if (dtStart == dtEnd)
	{
		//- check for previous multiday occurrences ---
		if (nOccDays > 1)
		{
			COleDateTimeSpan spCheckMDOcc(nOccDays - 1, 0, 0, 0);
			dtStartCheckOcc = dtStart - spCheckMDOcc;
		}
	}
	CXTPCalendarDatesArray arDates;
	GetOccurrencesDates(arDates, dtStartCheckOcc, dtEnd);

	COleDateTime dtI;
	int nCount = (int)arDates.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		dtI = arDates[i];

		_GetOccurrenceStartEnd(dtI, dtStartOcc, dtEndOcc);

		ASSERT(!(CXTPCalendarUtils::ResetTime(dtEndOcc) < CXTPCalendarUtils::ResetTime(dtStart) ||
			CXTPCalendarUtils::ResetTime(dtStartOcc) > CXTPCalendarUtils::ResetTime(dtEnd)));

		CXTPCalendarEventPtr ptrOccurrenceEvent(FindException(dtStartOcc, dtEndOcc), TRUE);

		if (ptrOccurrenceEvent)
		{
			continue;
		}
		else
		{
			ptrOccurrenceEvent = pMasterEvent->CloneForOccurrence(dtStartOcc, dtEndOcc);

			if (ptrOccurrenceEvent && ptrOccurrenceEvent->GetStartTime() <
				pMasterEvent->GetLastModificationTime())
			{
				ptrOccurrenceEvent->SetReminder(FALSE);
			}
		}

		if (ptrOccurrenceEvent)
		{
			m_arOccReminders.UpdateOccIfNeed(ptrOccurrenceEvent, this);

			pEventsArray->Add(ptrOccurrenceEvent);
		}
	}

	FindExceptions(pEventsArray, dtStart, dtEnd);

	return TRUE;
}

void CXTPCalendarRecurrencePattern::GetOccurrencesDates(CXTPCalendarDatesArray& rArDates,
		COleDateTime dtStart, COleDateTime dtEnd) const
{
	COleDateTime dtStart0 = CXTPCalendarUtils::ResetTime(dtStart);
	COleDateTime dtEnd0 = CXTPCalendarUtils::SetTime_235959(dtEnd);

	int nMaxOccCount = INT_MAX;
	int nOccCount = 0;
	COleDateTime dtI;
	COleDateTime dtE1(9999, 1, 1, 0, 0, 0);
	COleDateTime dtS1 = max(dtStart0, m_dtPatternStartDate);

	switch (m_PatternEnd.m_nUseEnd)
	{
		case xtpCalendarPatternEndDate:
			dtE1 = min(m_PatternEnd.m_dtPatternEndDate, dtEnd0);
			break;
		case xtpCalendarPatternEndAfterOccurrences:
			nMaxOccCount = m_PatternEnd.m_nEndAfterOccurrences;
			dtE1 = dtEnd0;
			break;
		case xtpCalendarPatternEndNoDate:
			dtE1 = dtEnd0;
			break;
		default:
			ASSERT(FALSE);
			return;
	}

	switch (m_Options.m_nRecurrenceType)
	{
		case xtpCalendarRecurrenceDaily:
		{
			//------------------------------------------------------
			// 1. Every N days:
			//      m_nInterval = N,
			//      m_nDayOfWeekMask = xtpCalendarDayAllWeek
			//------------------------------------------------------
			// 2. Every week day (Mo-Fr):
			//      m_nInterval = 1,
			//      m_nDayOfWeekMask = xtpCalendarDayMo_Fr
			//------------------------------------------------------

			int nDays = m_Options.m_Daily.nIntervalDays;

			if (m_Options.m_Daily.bEveryWeekDayOnly)
			{
				ASSERT(nDays == 1);
				nDays = 1;
			}

			COleDateTimeSpan spStep(nDays, 0, 0, 0);

			if (m_dtPatternStartDate < dtStart0)
			{
				//--- shift start date --------------
				COleDateTimeSpan spDiff = dtStart0 - m_dtPatternStartDate;
				int nDiff_Days = (int)GETTOTAL_DAYS_DTS(spDiff);
				ASSERT(nDiff_Days >= 1);

				int nJump_Days = (nDiff_Days / nDays) * nDays;
				spDiff.SetDateTimeSpan(nJump_Days, 0, 0, 0);

				dtS1 = m_dtPatternStartDate + spDiff;
				if (dtS1 < dtStart0)
				{
					dtS1 += spStep;
				}

				//--- calculate occurrences ----------
				if (nMaxOccCount != INT_MAX)
				{
					nOccCount = nDiff_Days / nDays;

					if (m_Options.m_Daily.bEveryWeekDayOnly)
					{
						int nWDstart = m_dtPatternStartDate.GetDayOfWeek();

						int nBasis1_days = (nDiff_Days - (7 - nWDstart));
						int nSatDays = nBasis1_days / 7 + ((nBasis1_days % 7) ? 1 : 0);

						int nBasis6_days = (nDiff_Days - (6 - nWDstart));
						int nSunDays = nBasis6_days / 7 + ((nBasis6_days % 7) ? 1 : 0);

						nOccCount -= nSatDays + nSunDays;
					}
				}
			}
			else
			{
				dtS1 = m_dtPatternStartDate;
			}

			for (dtI = dtS1; dtI <= dtE1 && nOccCount < nMaxOccCount; dtI += spStep)
			{
				if (m_Options.m_Daily.bEveryWeekDayOnly)
				{
					int nWD = dtI.GetDayOfWeek(); // 1-Sunday, 2 - Monday, ...
					if (nWD == 1 || nWD == 7)
					{
						continue;
					}
				}
				ASSERT(dtI >= dtStart0 && dtI <= dtE1);
				rArDates.Add(dtI);
				nOccCount++;
			}
		}
		break;

		case xtpCalendarRecurrenceWeekly:
		{
			//------------------------------------------------------
			// 1. Every N weeks:
			//      m_nInterval = N,
			//      m_nDayOfWeekMask = <*MASK*>
			//------------------------------------------------------

			int nWeeksStep = m_Options.m_Weekly.nIntervalWeeks;
			if (nWeeksStep <= 0)
			{
				ASSERT(FALSE);
				nWeeksStep = 1;
			}
			COleDateTimeSpan spStep(1, 0, 0, 0);
			COleDateTimeSpan spStepWeeks((nWeeksStep)*7, 0, 0, 0);

			//================================================================
			int nFirstDayOfWeekIndex = 2; // Always Monday (as in MS Outlook)

			COleDateTimeSpan spWeek(7, 0, 0, 0);
			COleDateTime dtPatSW = CXTPCalendarUtils::ShiftDateToWeekBegin(m_dtPatternStartDate, nFirstDayOfWeekIndex);
			dtPatSW = CXTPCalendarUtils::ResetTime(dtPatSW);

			COleDateTime dtOccFrame1 = dtPatSW;
			COleDateTime dtOccFrame2 = dtOccFrame1 + spWeek;

			if (m_dtPatternStartDate < dtStart0)
			{
				COleDateTime dtStart0SW = CXTPCalendarUtils::ShiftDateToWeekBegin(dtStart0, nFirstDayOfWeekIndex);

				COleDateTimeSpan spDiff = dtStart0SW - dtPatSW;

				int nDiff_Weeks = (int)spDiff.GetTotalDays() / 7;
				ASSERT((int)spDiff.GetTotalDays() % 7 == 0);

				int nJump_Weeks = (nDiff_Weeks / nWeeksStep) * nWeeksStep;
				spDiff.SetDateTimeSpan(nJump_Weeks * 7, 0, 0, 0);

				dtOccFrame1 = dtPatSW + spDiff;
				dtOccFrame2 = dtOccFrame1 + spWeek;

				nOccCount = CXTPCalendarUtils::GetDayOfWeekCount(m_Options.m_Weekly.nDayOfWeekMask);
				nOccCount *= nJump_Weeks / nWeeksStep;

				//------------------------------------------------------------
				int nSkippedMask = CXTPCalendarUtils::MakeDaysOfWeekMask_Mo_(m_dtPatternStartDate.GetDayOfWeek());
				nOccCount -= CXTPCalendarUtils::GetDayOfWeekCount(nSkippedMask & m_Options.m_Weekly.nDayOfWeekMask);
				//------------------------------------------------------------
				int nAddedMask = CXTPCalendarUtils::MakeDaysOfWeekMask_Mo_(dtS1.GetDayOfWeek());
				nOccCount += CXTPCalendarUtils::GetDayOfWeekCount(nAddedMask & m_Options.m_Weekly.nDayOfWeekMask);
			}
			//================================================================

			for (dtI = dtS1; dtI <= dtE1 && nOccCount < nMaxOccCount; dtI += spStep)
			{
				if (nWeeksStep > 1 && !(dtOccFrame1 <= dtI && dtI < dtOccFrame2))
				{
					dtOccFrame1 += spStepWeeks;
					dtOccFrame2 = dtOccFrame1 + spWeek;

					dtI = dtOccFrame1 - spStep;
					continue;
				}

				int nWD = dtI.GetDayOfWeek(); // 1-Sunday, 2 - Monday, ...
				int nWDMask = CXTPCalendarUtils::GetDayOfWeekMask(nWD);

				if (m_Options.m_Weekly.nDayOfWeekMask & nWDMask)
				{
					ASSERT(dtI >= dtStart0 && dtI <= dtE1);
					rArDates.Add(dtI);
					nOccCount++;
				}
			}
		}
		break;

		case xtpCalendarRecurrenceMonthly:
		{
			//------------------------------------------------------
			// 1. Day N of every M month(s):
			//      m_nDayOfMonth = N,
			//      m_nInterval = M
			//------------------------------------------------------

			//================================================================
			int nMInterval = m_Options.m_Monthly.nIntervalMonths;
			if (nMInterval <= 0)
			{
				ASSERT(FALSE);
				nMInterval = 1;
			}

			COleDateTime dtOccFrame1 = CXTPCalendarUtils::ResetTime(m_dtPatternStartDate);
			CXTPCalendarUtils::UpdateMonthDay(dtOccFrame1, 1);

			if (m_dtPatternStartDate < dtStart0)
			{
				int nPS_Year = m_dtPatternStartDate.GetYear();
				int nPS_Month = m_dtPatternStartDate.GetMonth();
				int nPS_GlobalMonth = nPS_Year * 12 + nPS_Month;

				int nS0_Year = dtStart0.GetYear();
				int nS0_Month = dtStart0.GetMonth();
				int nS0_GlobalMonth = nS0_Year * 12 + nS0_Month;

				int nMonthDiff = nS0_GlobalMonth - nPS_GlobalMonth;
				int nJump_Month = (nMonthDiff / nMInterval) * nMInterval;

				CXTPCalendarUtils::ShiftDate_Month(dtOccFrame1, nJump_Month, 1);

				nOccCount = nJump_Month / nMInterval;

				//------------------------------------------------------------
				if (m_dtPatternStartDate.GetDay() > m_Options.m_Monthly.nDayOfMonth)
				{
					nOccCount--;
				}
				//------------------------------------------------------------
				if (dtS1.GetDay() > m_Options.m_Monthly.nDayOfMonth)
				{
					nOccCount++;
				}
			}
			//================================================================

			COleDateTime dtOcc(dtOccFrame1);
			VERIFY(CXTPCalendarUtils::UpdateMonthDay(dtOcc, m_Options.m_Monthly.nDayOfMonth));
			if (dtOcc >= dtS1)
			{
				dtS1 = dtOcc;
			}
			else
			{
				dtS1 = dtOcc;
				CXTPCalendarUtils::ShiftDate_Month(dtS1, m_Options.m_Monthly.nIntervalMonths,
					m_Options.m_Monthly.nDayOfMonth);
			}

			for (dtI = dtS1; dtI <= dtE1 && nOccCount < nMaxOccCount;)
			{
				ASSERT(dtI >= dtStart0 && dtI <= dtE1);
				rArDates.Add(dtI);
				nOccCount++;

				CXTPCalendarUtils::ShiftDate_Month(dtI, m_Options.m_Monthly.nIntervalMonths,
					m_Options.m_Monthly.nDayOfMonth);
			}
		}
		break;

		case xtpCalendarRecurrenceMonthNth:
		{
			//------------------------------------------------------
			//~ The I:{First, second, ..., Last>
			//~         W:{Day, WeekDay, WeekEndDay, Su, Mo, ...}
			//~             of every M month(s):
			//------------------------------------------------------

			//================================================================
			int nMInterval = m_Options.m_MonthNth.nIntervalMonths;
			if (nMInterval <= 0)
			{
				ASSERT(FALSE);
				nMInterval = 1;
			}

			COleDateTime dtOccFrame1 = CXTPCalendarUtils::ResetTime(m_dtPatternStartDate);
			CXTPCalendarUtils::UpdateMonthDay(dtOccFrame1, 1);

			if (m_dtPatternStartDate < dtStart0)
			{
				int nPS_Year = m_dtPatternStartDate.GetYear();
				int nPS_Month = m_dtPatternStartDate.GetMonth();
				int nPS_GlobalMonth = nPS_Year * 12 + nPS_Month;

				int nS0_Year = dtStart0.GetYear();
				int nS0_Month = dtStart0.GetMonth();
				int nS0_GlobalMonth = nS0_Year * 12 + nS0_Month;

				int nMonthDiff = nS0_GlobalMonth - nPS_GlobalMonth;
				int nJump_Month = (nMonthDiff / nMInterval) * nMInterval;

				CXTPCalendarUtils::ShiftDate_Month(dtOccFrame1, nJump_Month, 1);

				nOccCount = nJump_Month / nMInterval;

				//------------------------------------------------------------
				COleDateTime dtTestMD(m_dtPatternStartDate);
				CXTPCalendarUtils::CalcDayOfMonth(dtTestMD,
					m_Options.m_MonthNth.nWhichDay,
					m_Options.m_MonthNth.nWhichDayMask);

				if (m_dtPatternStartDate.GetDay() > dtTestMD.GetDay())
				{
					nOccCount--;
				}
				//------------------------------------------------------------
				dtTestMD = dtS1;
				CXTPCalendarUtils::CalcDayOfMonth(dtTestMD,
					m_Options.m_MonthNth.nWhichDay,
					m_Options.m_MonthNth.nWhichDayMask);

				if (dtS1.GetDay() > dtTestMD.GetDay())
				{
					nOccCount++;
				}
			}
			//================================================================

			COleDateTime dtOcc(dtOccFrame1);

			BOOL bRes = CXTPCalendarUtils::CalcDayOfMonth(dtOcc, m_Options.m_MonthNth.nWhichDay,
				m_Options.m_MonthNth.nWhichDayMask);
			if (!bRes)
			{
				return;
			}

			if (dtOcc >= dtS1)
			{
				dtS1 = dtOcc;
			}
			else
			{
				dtS1 = dtOcc;
				CXTPCalendarUtils::ShiftDate_Month(dtS1, nMInterval);
				bRes = CXTPCalendarUtils::CalcDayOfMonth(dtS1, m_Options.m_MonthNth.nWhichDay,
					m_Options.m_MonthNth.nWhichDayMask);
				if (!bRes)
				{
					return;
				}
			}

			for (dtI = dtS1; dtI <= dtE1 && nOccCount < nMaxOccCount;)
			{
				ASSERT(dtI >= dtStart0 && dtI <= dtE1);
				rArDates.Add(dtI);
				nOccCount++;

				CXTPCalendarUtils::ShiftDate_Month(dtI, nMInterval);
				bRes = CXTPCalendarUtils::CalcDayOfMonth(dtI, m_Options.m_MonthNth.nWhichDay,
					m_Options.m_MonthNth.nWhichDayMask);
				if (!bRes)
				{
					ASSERT(FALSE);
					break;
				}
			}
		}
		break;

		case xtpCalendarRecurrenceYearly:
		{
			//------------------------------------------------------
			// Every M:{month} D:{Day}
			//      m_nMonthOfYear = M
			//      m_nDayOfMonth = D
			//------------------------------------------------------

			COleDateTime dtOcc(dtS1.GetYear(), m_Options.m_Yearly.nMonthOfYear,
							   1, 0, 0, 0);
			CXTPCalendarUtils::UpdateMonthDay(dtOcc, m_Options.m_Yearly.nDayOfMonth);

			if (dtOcc >= dtS1)
			{
				dtS1 = dtOcc;
			}
			else
			{
				dtS1 = dtOcc;
				CXTPCalendarUtils::ShiftDate_Year(dtS1, 1, m_Options.m_Yearly.nDayOfMonth);
			}

			for (dtI = dtS1; dtI <= dtE1 && nOccCount < nMaxOccCount;
				 CXTPCalendarUtils::ShiftDate_Year(dtI, 1, m_Options.m_Yearly.nDayOfMonth))
			{
				ASSERT(dtI >= dtStart0 && dtI <= dtE1);
				rArDates.Add(dtI);
				nOccCount++;
			}
		}
		break;

		case xtpCalendarRecurrenceYearNth:
		{
			//------------------------------------------------------
			//~ The I:{First, second, ..., Last>
			//~         W:{Day, WeekDay, WeekEndDay, Su, Mo, ...}
			//~             of M:{month}
			//------------------------------------------------------
			COleDateTime dtOcc(dtS1.GetYear(), m_Options.m_YearNth.nMonthOfYear,
				1, 0, 0, 0);

			BOOL bRes = CXTPCalendarUtils::CalcDayOfMonth(dtOcc,
				m_Options.m_YearNth.nWhichDay,
				m_Options.m_YearNth.nWhichDayMask);
			if (!bRes)
			{
				return;
			}

			if (dtOcc >= dtS1)
			{
				dtS1 = dtOcc;
			}
			else
			{
				dtS1 = dtOcc;
				CXTPCalendarUtils::ShiftDate_Year(dtS1, 1);
				bRes = CXTPCalendarUtils::CalcDayOfMonth(dtS1, m_Options.m_YearNth.nWhichDay,
					m_Options.m_YearNth.nWhichDayMask);
				if (!bRes)
				{
					return;
				}
			}

			for (dtI = dtS1; dtI <= dtE1 && nOccCount < nMaxOccCount;)
			{
				ASSERT(dtI >= dtStart0 && dtI <= dtE1);
				rArDates.Add(dtI);
				nOccCount++;

				CXTPCalendarUtils::ShiftDate_Year(dtI, 1);
				bRes = CXTPCalendarUtils::CalcDayOfMonth(dtI, m_Options.m_YearNth.nWhichDay,
					m_Options.m_YearNth.nWhichDayMask);
				if (!bRes)
				{
					ASSERT(FALSE);
					break;
				}
			}
		}
	}
}


void CXTPCalendarRecurrencePattern::SetRecurrenceOptions(const XTP_CALENDAR_RECURRENCE_OPTIONS& Options)
{
	m_Options = Options;

	if (m_Options.m_nRecurrenceType == xtpCalendarRecurrenceDaily &&
		m_Options.m_Daily.bEveryWeekDayOnly)
	{
		m_Options.m_Daily.nIntervalDays = 1;
	}
}


void CXTPCalendarRecurrencePattern::SetPatternEnd(const XTP_CALENDAR_PATTERN_END& patternEnd)
{
	m_PatternEnd = patternEnd;

	if (m_PatternEnd.m_nUseEnd == xtpCalendarPatternEndDate)
	{
		m_PatternEnd.m_dtPatternEndDate = CXTPCalendarUtils::ResetTime(m_PatternEnd.m_dtPatternEndDate);

		if (m_dtPatternStartDate.GetStatus() == COleDateTime::valid &&
			m_PatternEnd.m_dtPatternEndDate.GetStatus() == COleDateTime::valid &&
			m_PatternEnd.m_dtPatternEndDate < m_dtPatternStartDate)
		{
			m_dtPatternStartDate = m_PatternEnd.m_dtPatternEndDate;
		}
	}
}


CXTPCalendarRecurrencePatternPtr CXTPCalendarRecurrencePattern::ClonePattern()
{
	if (!m_ptrDataProvider)
	{
		ASSERT(FALSE);
		return NULL;
	}

	DWORD dwPatternID = GetPatternID();
	DWORD dwMasterID = GetMasterEventID();

	CXTPCalendarRecurrencePatternPtr ptrNew = m_ptrDataProvider->CreateNewRecurrencePattern();
	if (!ptrNew)
	{
		ASSERT(FALSE);
		return NULL;
	}
	ptrNew->SetPatternID(dwPatternID);
	ptrNew->SetMasterEventID(dwMasterID);

	if (!ptrNew->Update(this))
	{
		ASSERT(FALSE);
		return NULL;
	}

	return ptrNew;
}

BOOL CXTPCalendarRecurrencePattern::Update(CXTPCalendarRecurrencePattern* pNewData)
{
	if (pNewData == this)
	{
		return TRUE;
	}

	COleDateTime dtTmp = pNewData->GetStartTime();
	SetStartTime(dtTmp);

	int nTmp = pNewData->GetDurationMinutes();
	SetDurationMinutes(nTmp);

	const XTP_CALENDAR_RECURRENCE_OPTIONS& rOpt = pNewData->GetRecurrenceOptions();
	SetRecurrenceOptions(rOpt);

	dtTmp = pNewData->GetPatternStartDate();
	SetPatternStartDate(dtTmp);

	const XTP_CALENDAR_PATTERN_END& rPatternEnd = pNewData->GetPatternEnd();
	SetPatternEnd(rPatternEnd);

	RemoveAllExceptions();

	CXTPCalendarEventsPtr ptrExceptions = pNewData->GetExceptions();

	int nCount = XTP_SAFE_GET1(ptrExceptions, GetCount(), 0);
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pException = ptrExceptions->GetAt(i, FALSE);
		if (pException)
		{
			if (!SetException(pException))
			{
				ASSERT(FALSE);
			}
		}
	}

	ASSERT(pNewData->GetCustomProperties());
	pNewData->GetCustomProperties()->CopyTo(m_pCustomProperties);

	m_arOccReminders.RemoveAll();
	m_arOccReminders.Append(pNewData->m_arOccReminders);

	return TRUE;
}

BOOL CXTPCalendarRecurrencePattern::SetException(CXTPCalendarEvent* pException, CXTPCalendarEvent* pMasterEventToUpdate)
{
	if (!pException)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DWORD dwExcID = pException->GetEventID();
	CXTPCalendarEvent* pExc = m_arExceptionEvents.FindEvent(dwExcID);

	if (!pExc)
	{
		COleDateTime dtStartOcc = pException->GetRException_StartTimeOrig();
		COleDateTime dtEndOcc = pException->GetRException_EndTimeOrig();

		pExc = FindException(dtStartOcc, dtEndOcc);
	}

	BOOL bRes = TRUE;
	if (pExc)
	{
		pExc->SetEventID(pException->GetEventID());
		bRes = pExc->Update(pException);
		ASSERT(bRes);
	}
	else
	{
		CXTPCalendarEventPtr ptrExc = pException->CloneEvent();
		m_arExceptionEvents.Add(ptrExc);
		pExc = ptrExc;
	}
	pExc->SetRecurrencePatternID(m_dwPatternID);

	if (pMasterEventToUpdate)
	{
		pMasterEventToUpdate->UpdateStartEndDatesFromPatternExceptions();
	}

	return bRes;
}

BOOL CXTPCalendarRecurrencePattern::RemoveException(CXTPCalendarEvent* pException,
									CXTPCalendarEvent* pMasterEventToUpdate)
{
	if (!pException)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nRState = pException->GetRecurrenceState();
	ASSERT(nRState == xtpCalendarRecurrenceOccurrence || nRState == xtpCalendarRecurrenceException);

	COleDateTime dtStartOcc, dtEndOcc;

	if (nRState == xtpCalendarRecurrenceOccurrence)
	{
		dtStartOcc = pException->GetStartTime();
		dtEndOcc = pException->GetEndTime();
	}
	else
	{
		if (nRState == xtpCalendarRecurrenceException)
		{
			dtStartOcc = pException->GetRException_StartTimeOrig();
			dtEndOcc = pException->GetRException_EndTimeOrig();
		}
		else
		{
			return FALSE;
		}
	}

	int nCount = m_arExceptionEvents.GetCount();
	int nFExcIdx = FindExceptionIndex(dtStartOcc, dtEndOcc);

	if (nFExcIdx >= 0 && nFExcIdx < nCount)
	{
		m_arExceptionEvents.RemoveAt(nFExcIdx);

		if (pMasterEventToUpdate)
		{
			pMasterEventToUpdate->UpdateStartEndDatesFromPatternExceptions();
		}

		return TRUE;
	}

	return FALSE;
}


void CXTPCalendarRecurrencePattern::RemoveAllExceptions()
{
	m_arExceptionEvents.RemoveAll();

	m_arOccReminders.RemoveAll();
}

CXTPCalendarEvent* CXTPCalendarRecurrencePattern::FindException(COleDateTime dtStartOrig, COleDateTime dtEndOrig) const
{
	int nCount = m_arExceptionEvents.GetCount();
	int nFExcIdx = FindExceptionIndex(dtStartOrig, dtEndOrig);

	if (nFExcIdx >= 0 && nFExcIdx < nCount)
	{
		CXTPCalendarEvent* pEvent = m_arExceptionEvents.GetAt(nFExcIdx, FALSE);
		return pEvent;
	}
	return NULL;
}

int CXTPCalendarRecurrencePattern::FindExceptionIndex(COleDateTime dtStartOrig, COleDateTime dtEndOrig) const
{
	COleDateTime dtTmpS, dtTmpE;

	int nCount = m_arExceptionEvents.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pEvent = m_arExceptionEvents.GetAt(i, FALSE);

		ASSERT(pEvent->GetRecurrenceState() == xtpCalendarRecurrenceException);

		dtTmpS = pEvent->GetRException_StartTimeOrig();
		dtTmpE = pEvent->GetRException_EndTimeOrig();

		if (CXTPCalendarUtils::IsEqual(dtTmpS, dtStartOrig) &&
				CXTPCalendarUtils::IsEqual(dtTmpE, dtEndOrig))
		{
			return i;
		}
	}
	return -1;
}

BOOL CXTPCalendarRecurrencePattern::FindExceptions(CXTPCalendarEvents* pEvents,
												   COleDateTime dtStart,
												   COleDateTime dtEnd) const
{
	dtStart = CXTPCalendarUtils::ResetTime(dtStart);
	dtEnd = CXTPCalendarUtils::ResetTime(dtEnd);

	COleDateTime dtTmpS, dtTmpE;

	int nCount = m_arExceptionEvents.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pEvent = m_arExceptionEvents.GetAt(i, FALSE);

		ASSERT(pEvent->GetRecurrenceState() == xtpCalendarRecurrenceException);

		dtTmpS = CXTPCalendarUtils::ResetTime(pEvent->GetStartTime());
		dtTmpE = CXTPCalendarUtils::ResetTime(pEvent->GetEndTime());

		if (dtStart <= dtTmpS && dtTmpS <= dtEnd ||
				dtStart <= dtTmpE && dtTmpE <= dtEnd ||
				dtTmpS <= dtStart && dtStart <= dtTmpE ||
				dtTmpS <= dtEnd && dtEnd <= dtTmpE)
		{
			if (!pEvent->IsRExceptionDeleted())
			{
				pEvents->Add(pEvent);
			}
		}
	}
	return TRUE;
}

CXTPCalendarEventsPtr CXTPCalendarRecurrencePattern::GetExceptions()
{
	CXTPCalendarEventsPtr ptrExceptions = new CXTPCalendarEvents();
	if (!ptrExceptions)
	{
		return NULL;
	}

	int nCount = m_arExceptionEvents.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pEvent = m_arExceptionEvents.GetAt(i, FALSE);
		ASSERT(pEvent);

		CXTPCalendarEventPtr ptrEventCopy = pEvent ? pEvent->CloneEvent() : NULL;
		if (ptrEventCopy)
		{
			ptrExceptions->Add(ptrEventCopy);
		}
	}
	return ptrExceptions;
}

CXTPCalendarEventPtr CXTPCalendarRecurrencePattern::GetMasterEvent() const
{
	if (!m_ptrDataProvider)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarEventPtr ptrMEvent = m_ptrDataProvider->GetEvent(m_dwMasterEventID);
	ASSERT(ptrMEvent);
	return ptrMEvent;
}

void CXTPCalendarRecurrencePattern::SetPatternID(DWORD dwID)
{
	m_dwPatternID = dwID;

	int nCount = m_arExceptionEvents.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEvent* pEvent = m_arExceptionEvents.GetAt(i, FALSE);
		ASSERT(pEvent);
		if (pEvent)
		{
			pEvent->SetRecurrencePatternID(m_dwPatternID);
		}
	}
}

void CXTPCalendarRecurrencePattern::DoPropExchange(CXTPPropExchange* pPX)
{
	long nVersion = XTP_RECPATT_PROP_SET_DATA_VER;
	PX_Long(pPX, _T("Version"), nVersion, XTP_RECPATT_PROP_SET_DATA_VER);

	PX_DWord(pPX, cszRPatt_PatternID, m_dwPatternID, XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);
	PX_DateTime(pPX, cszRPatt_StartTime, m_dtStartTime);
	PX_Int(pPX, cszRPatt_DurationMinutes, m_nDurationMinutes, 0);

	PX_Enum(pPX, cszRPatt_RecurrenceType, m_Options.m_nRecurrenceType, xtpCalendarRecurrenceUnknown);

	PX_Int(pPX, cszRPatt_RecurrenceOpt_Data1, m_Options.m_YearNth.nWhichDay, 0);
	PX_Int(pPX, cszRPatt_RecurrenceOpt_Data2, m_Options.m_YearNth.nWhichDayMask, 0);
	PX_Int(pPX, cszRPatt_RecurrenceOpt_Data3, m_Options.m_YearNth.nMonthOfYear, 0);

	PX_DateTime(pPX, cszRPatt_PatternStartDate, m_dtPatternStartDate);

	PX_Enum(pPX, cszRPatt_UseEndMethod, m_PatternEnd.m_nUseEnd, xtpCalendarPatternEndNoDate);

	if (m_PatternEnd.m_nUseEnd == xtpCalendarPatternEndDate)
	{
		PX_DateTime(pPX, cszRPatt_PatternEndDate, m_PatternEnd.m_dtPatternEndDate);
	}
	else if (m_PatternEnd.m_nUseEnd == xtpCalendarPatternEndAfterOccurrences)
	{
		PX_Int(pPX, cszRPatt_EndAfterOccurrences, m_PatternEnd.m_nEndAfterOccurrences, 1);
	}

	PX_DWord(pPX, cszRPatt_MasterEventID, m_dwMasterEventID, XTP_CALENDAR_UNKNOWN_EVENT_ID);

	//------------------------------------------------------------------------
	if (pPX->IsStoring())
	{
		VERIFY( m_arOccReminders.Save(m_pCustomProperties, this) );
	}

	//-------------------------------------------
	m_pCustomProperties->DoPropExchange(pPX);

	//-------------------------------------------
	if (pPX->IsLoading())
	{
		VERIFY( m_arOccReminders.Load(m_pCustomProperties) );
	}

	m_arOccReminders.ClearProperties(m_pCustomProperties);
}

void CXTPCalendarRecurrencePattern::SetOccReminder(
					CXTPCalendarEvent* pOccEvent,
					int nbIsReminder,
					DATE dtNextReminderTime_Snoozed)
{
	ASSERT(pOccEvent);
	if (!pOccEvent)
	{
		return;
	}

	int nIndex = m_arOccReminders.Find(pOccEvent);
	if (nIndex < 0)
	{
		CXTPCalendarReminderForOccurrence tmpData;
		tmpData.m_dtOccurrenceStartTime = pOccEvent->GetStartTime();
		tmpData.m_dtOccurrenceEndTime = pOccEvent->GetEndTime();

		nIndex = (int)m_arOccReminders.Add(tmpData);
	}
	CXTPCalendarReminderForOccurrence* pOccRmdData;
	pOccRmdData = &m_arOccReminders[nIndex];

	if (nbIsReminder != xtpCalendarRmdPrm_DontChange)
	{
		pOccRmdData->m_nbIsReminder = nbIsReminder;
	}

	if (dtNextReminderTime_Snoozed != xtpCalendarRmdPrm_DontChange)
	{
		pOccRmdData->m_dtNextReminderTime_Snoozed = dtNextReminderTime_Snoozed;
	}

	m_arOccReminders.UpdateOccIfNeedEx(pOccEvent, pOccRmdData, this);

	if (pOccRmdData->m_nbIsReminder == xtpCalendarRmdPrm_Default &&
		pOccRmdData->m_dtNextReminderTime_Snoozed == xtpCalendarRmdPrm_Default)
	{
		m_arOccReminders.RemoveAt(nIndex);
	}
}


////////////////////////////////////////////////////////////////////////////
CXTPCalendarReminderForOccurrence::CXTPCalendarReminderForOccurrence()
{
	m_dtOccurrenceStartTime = 0;
	m_dtOccurrenceEndTime = 0;

	m_nbIsReminder = xtpCalendarRmdPrm_Default;
	m_dtNextReminderTime_Snoozed = (DATE)xtpCalendarRmdPrm_Default;
}

CXTPCalendarReminderForOccurrence::~CXTPCalendarReminderForOccurrence()
{
}


const CXTPCalendarReminderForOccurrence& CXTPCalendarReminderForOccurrence::operator=(
		const CXTPCalendarReminderForOccurrence& rSrc)
{
	m_dtOccurrenceStartTime = rSrc.m_dtOccurrenceStartTime;
	m_dtOccurrenceEndTime = rSrc.m_dtOccurrenceEndTime;

	m_nbIsReminder = rSrc.m_nbIsReminder;
	m_dtNextReminderTime_Snoozed = rSrc.m_dtNextReminderTime_Snoozed;

	return *this;
}

//===========================================================================
CXTPCalendarReminderForOccurrenceArray::CXTPCalendarReminderForOccurrenceArray()
{
}

CXTPCalendarReminderForOccurrenceArray::~CXTPCalendarReminderForOccurrenceArray()
{
}

int CXTPCalendarReminderForOccurrenceArray::Find(CXTPCalendarEvent* pOccEvent)
{
	ASSERT(pOccEvent);
	if (!pOccEvent)
	{
		return -1;
	}
	ASSERT(pOccEvent->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence);

	DATE dtOcctartTime = pOccEvent->GetStartTime();
	DATE dtOccEndTime = pOccEvent->GetEndTime();

	int nFIndex = Find(dtOcctartTime, dtOccEndTime);
	return nFIndex;
}

int CXTPCalendarReminderForOccurrenceArray::Find(DATE dtOccStartTime, DATE dtOccEndTime)
{
	int nCount = (int)GetSize();
	for (int i = 0; i < nCount; i++)
	{
		const CXTPCalendarReminderForOccurrence& rData = ElementAt(i);

		if (CXTPCalendarUtils::IsEqual(dtOccStartTime, rData.m_dtOccurrenceStartTime) &&
			CXTPCalendarUtils::IsEqual(dtOccEndTime, rData.m_dtOccurrenceEndTime))
		{
			return i;
		}
	}
	return -1;
}

int CXTPCalendarReminderForOccurrenceArray::UpdateOccIfNeed(
								CXTPCalendarEvent* pOccEvent,
								CXTPCalendarRecurrencePattern* pPattern)
{
	if (!pOccEvent || !pPattern)
	{
		ASSERT(FALSE);
		return 0;
	}

	int nFIndex = Find(pOccEvent);
	if (nFIndex < 0)
	{
		return 0;
	}

	CXTPCalendarReminderForOccurrence* pOccRmdData;
	pOccRmdData = &ElementAt(nFIndex);

	int nRes = UpdateOccIfNeedEx(pOccEvent, pOccRmdData, pPattern);
	return nRes;
}

int CXTPCalendarReminderForOccurrenceArray::UpdateOccIfNeedEx(
							CXTPCalendarEvent* pOccEvent,
							const CXTPCalendarReminderForOccurrence* pOccRmdData,
							CXTPCalendarRecurrencePattern* pPattern)
{
	if (!pOccEvent || !pOccRmdData || !pPattern)
	{
		ASSERT(FALSE);
		return 0;
	}

	// IsReminder
	if (pOccRmdData->m_nbIsReminder == xtpCalendarRmdPrm_Default)
	{
		CXTPCalendarEventPtr ptrMasterEvent = pPattern->GetMasterEvent();
		if (!ptrMasterEvent)
		{
			ASSERT(FALSE);
			return 0;
		}
		pOccEvent->SetReminder(ptrMasterEvent->IsReminder());
	}
	else if (pOccRmdData->m_nbIsReminder != xtpCalendarRmdPrm_DontChange)
	{
		pOccEvent->SetReminder(pOccRmdData->m_nbIsReminder != 0);
	}

	// MinutesBeforeStart_Snoozed
	if (pOccRmdData->m_dtNextReminderTime_Snoozed == xtpCalendarRmdPrm_Default)
	{
		pOccEvent->GetCustomProperties()->RemoveProperty(cszEventCustProp_NextReminderTime_Snoozed);
	}
	else if (pOccRmdData->m_dtNextReminderTime_Snoozed != xtpCalendarRmdPrm_DontChange)
	{
		VERIFY( pOccEvent->GetCustomProperties()->SetProperty(
							cszEventCustProp_NextReminderTime_Snoozed,
							pOccRmdData->m_dtNextReminderTime_Snoozed) );

	}
	return 1;
}

BOOL CXTPCalendarReminderForOccurrenceArray::Load(CXTPCalendarCustomProperties* pProps)
{
	ASSERT(pProps);
	if (!pProps)
	{
		return FALSE;
	}
	RemoveAll();

	COleVariant varCount;
	BOOL bExists = pProps->GetProperty(cszOccRMD_Count, varCount);
	if (!bExists)
	{
		return TRUE;
	}
	COleVariant varValue;
	CString strPropNameI;

	int nCount = (int)(long)_variant_t(varCount);
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarReminderForOccurrence rmdData;

		_PropName(cszOccRMD_StartTime, i, strPropNameI);
		if (pProps->GetProperty(strPropNameI, varValue))
		{
			rmdData.m_dtOccurrenceStartTime = (DATE)_variant_t(varValue);
		}

		_PropName(cszOccRMD_EndTime, i, strPropNameI);
		if (pProps->GetProperty(strPropNameI, varValue))
		{
			rmdData.m_dtOccurrenceEndTime = (DATE)_variant_t(varValue);
		}

		_PropName(cszOccRMD_IsReminder, i, strPropNameI);
		if (pProps->GetProperty(strPropNameI, varValue))
		{
			rmdData.m_nbIsReminder = (long)_variant_t(varValue);
		}

		_PropName(cszOccRMD_NextReminderTime_Snoozed, i, strPropNameI);
		if (pProps->GetProperty(strPropNameI, varValue))
		{
			rmdData.m_dtNextReminderTime_Snoozed = (DATE)_variant_t(varValue);
		}

		Add(rmdData);
	}
	return TRUE;
}

BOOL CXTPCalendarReminderForOccurrenceArray::Save(CXTPCalendarCustomProperties* pProps,
												 CXTPCalendarRecurrencePattern* pPattern)
{
	ASSERT(pProps);
	if (!pProps)
	{
		return FALSE;
	}
	ClearProperties(pProps);

	CString strPropNameI;
	int nCount = (int)GetSize();
	int i;

	//===========================================================================
	if (pPattern)
	{
		for (i = nCount-1; i >= 0; i--)
		{
			const CXTPCalendarReminderForOccurrence& rData = ElementAt(i);

			int nFExcIdx = pPattern->FindExceptionIndex(rData.m_dtOccurrenceStartTime,
														rData.m_dtOccurrenceEndTime);
			if (nFExcIdx >= 0)
			{
				RemoveAt(i);
			}
		}
	}

	//===========================================================================
	nCount = (int)GetSize();
	for (i = 0; i < nCount; i++)
	{
		const CXTPCalendarReminderForOccurrence& rData = ElementAt(i);

		_PropName(cszOccRMD_StartTime, i, strPropNameI);
		VERIFY( pProps->SetProperty(strPropNameI, COleDateTime(rData.m_dtOccurrenceStartTime)) );

		_PropName(cszOccRMD_EndTime, i, strPropNameI);
		VERIFY( pProps->SetProperty(strPropNameI, COleDateTime(rData.m_dtOccurrenceEndTime)) );

		_PropName(cszOccRMD_IsReminder, i, strPropNameI);
		VERIFY( pProps->SetProperty(strPropNameI, (long)rData.m_nbIsReminder) );

		if (rData.m_dtNextReminderTime_Snoozed != xtpCalendarRmdPrm_Default)
		{
			ASSERT(rData.m_dtNextReminderTime_Snoozed >= xtpCalendarDateTime_min && rData.m_dtNextReminderTime_Snoozed <= xtpCalendarDateTime_max);
			_PropName(cszOccRMD_NextReminderTime_Snoozed, i, strPropNameI);
			VERIFY( pProps->SetProperty(strPropNameI, COleDateTime(rData.m_dtNextReminderTime_Snoozed)) );
		}
	}

	BOOL bRes = pProps->SetProperty(cszOccRMD_Count, (long)nCount);
	ASSERT(bRes);

	return bRes;
}

void CXTPCalendarReminderForOccurrenceArray::ClearProperties(CXTPCalendarCustomProperties* pProps)
{
	ASSERT(pProps);
	if (!pProps)
	{
		return;
	}
	COleVariant varCount;
	BOOL bExists = pProps->GetProperty(cszOccRMD_Count, varCount);
	if (!bExists)
	{
		return;
	}
	CString strPropNameI;

	int nCount = (int)(long)_variant_t(varCount);
	for (int i = 0; i < nCount; i++)
	{
		_PropName(cszOccRMD_StartTime, i, strPropNameI);
		pProps->RemoveProperty(strPropNameI);

		_PropName(cszOccRMD_EndTime, i, strPropNameI);
		pProps->RemoveProperty(strPropNameI);

		_PropName(cszOccRMD_IsReminder, i, strPropNameI);
		pProps->RemoveProperty(strPropNameI);

		_PropName(cszOccRMD_NextReminderTime_Snoozed, i, strPropNameI);
		pProps->RemoveProperty(strPropNameI);
	}

	pProps->RemoveProperty(cszOccRMD_Count);
}
void CXTPCalendarReminderForOccurrenceArray::_PropName(LPCTSTR pcszProp, int nIndex, CString& rstrPropNameI)
{
	TCHAR szIndex[64];
	ITOT_S(nIndex, szIndex, 64, 10);

	rstrPropNameI = pcszProp;
	rstrPropNameI += _T("[");
	rstrPropNameI += szIndex;
	rstrPropNameI += _T("]");
}

