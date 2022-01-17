// XTPCalendarUtils.cpp: implementation of the CXTPCalendarUtils class.
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
#include "resource.h"
#include <math.h>

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPSystemHelpers.h"

#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarData.h"
#include "XTPCalendarUtils.h"





#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////
BOOL CXTPCalendarUtils::s_bUseResourceFileLocale = FALSE;

BOOL CXTPCalendarUtils::IsUseResourceFileLocale()
{
	return s_bUseResourceFileLocale;
}

void CXTPCalendarUtils::SetUseResourceFileLocale(BOOL bUseResourceFileLocale)
{
	s_bUseResourceFileLocale = bUseResourceFileLocale;
}

LCID CXTPCalendarUtils::GetActiveLCID()
{
	LCID lcidCurr = LOCALE_USER_DEFAULT;

	if (s_bUseResourceFileLocale)
		lcidCurr = MAKELCID(XTPResourceManager()->GetResourcesLangID(), SORT_DEFAULT);

	return lcidCurr;
}

////////////////////////////////////////////////////////////////////////////
CXTPCalendarUtils::CXTPCalendarUtils()
{
}

int CXTPCalendarUtils::GetLocaleLong(LCTYPE LCType)
{
	TCHAR szResult[5];

	int nResult = ::GetLocaleInfo(GetActiveLCID(), LCType , szResult, 4);
	ASSERT(nResult == 2);
	return _ttoi(szResult);
}

CString CXTPCalendarUtils::GetLocaleString(LCTYPE LCType, int nMaxLength)
{
	CString strResult;

	int nResult = ::GetLocaleInfo(GetActiveLCID(), LCType , strResult.GetBufferSetLength(nMaxLength), nMaxLength);
	strResult.ReleaseBuffer();
	ASSERT(strResult.GetLength() == nResult - 1);

	return strResult;
}

CString CXTPCalendarUtils::GetDateFormat(CONST SYSTEMTIME* lpDate, LPCTSTR lpFormat)
{
	CString strResult;

	int nResult = ::GetDateFormat(GetActiveLCID(), 0, lpDate, lpFormat, strResult.GetBufferSetLength(255), 255);
	strResult.ReleaseBuffer();
	ASSERT(strResult.GetLength() == nResult - 1 || strResult.GetLength() == 0);

	return strResult;
}

CString CXTPCalendarUtils::GetTimeFormat(CONST SYSTEMTIME* lpTime, LPCTSTR lpFormat)
{
	CString strResult;

	int nResult = ::GetTimeFormat(GetActiveLCID(), 0, lpTime, lpFormat, strResult.GetBufferSetLength(255), 255);
	strResult.ReleaseBuffer();
	ASSERT(strResult.GetLength() == nResult - 1);

	return strResult;
}

CString CXTPCalendarUtils::GetDateFormat(CONST COleDateTime& dtDate, LPCTSTR lpFormat)
{
	SYSTEMTIME stDate;
	GETASSYSTEMTIME_DT(dtDate, stDate);
	return GetDateFormat(&stDate, lpFormat);
}

CString CXTPCalendarUtils::GetTimeFormat(CONST COleDateTime& dtTime, LPCTSTR lpFormat)
{
	SYSTEMTIME stTime;
	GETASSYSTEMTIME_DT(dtTime, stTime);
	return GetTimeFormat(&stTime, lpFormat);
}

CString CXTPCalendarUtils::GetTimeFormat(int nHour, int nMinutes, LPCTSTR lpFormat)
{
	SYSTEMTIME stTime;
	::ZeroMemory(&stTime, sizeof(stTime));

	stTime.wHour = (WORD)nHour;
	stTime.wMinute = (WORD)nMinutes;

	return GetTimeFormat(&stTime, lpFormat);
}

///***

COleDateTime CXTPCalendarUtils::RetriveEventEndDate(const CXTPCalendarEvent* pEvent)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return COleDateTime((DATE)0);
	}

	COleDateTime dtStartDay = ResetTime(pEvent->GetStartTime());
	COleDateTime dtEndDay = ResetTime(pEvent->GetEndTime());

	if (IsZeroTime(pEvent->GetEndTime()) && dtStartDay < dtEndDay)
	{
		dtEndDay -= COleDateTimeSpan(1, 0, 0, 0);
	}
	return dtEndDay;
}

BOOL CXTPCalendarUtils::CalcDayOfMonth(COleDateTime& rdtDate, int nWhichDay, int nWhichDayMask)
{
	ASSERT(nWhichDay >= xtpCalendarWeekFirst && nWhichDay <= xtpCalendarWeekLast);
	ASSERT((nWhichDayMask & (~xtpCalendarDayAllWeek)) == 0);

	COleDateTime dtYMD;
	dtYMD.SetDate(rdtDate.GetYear(), rdtDate.GetMonth(), 1);

	COleDateTimeSpan spStep(nWhichDay == xtpCalendarWeekLast ? -1 : 1, 0, 0, 0);

	if (nWhichDay == xtpCalendarWeekLast)
	{
		CXTPCalendarUtils::ShiftDate_Month(dtYMD, 1);
		COleDateTimeSpan spInitStep(-1, 0, 0, 0);
		dtYMD += spInitStep;
	}

	int nFCount = nWhichDay == xtpCalendarWeekLast ? xtpCalendarWeekLast-1 : xtpCalendarWeekFirst-1;

	for (int i = 0; i < 31; i++)
	{
		int nWD = dtYMD.GetDayOfWeek();
		int nWDMask = CXTPCalendarUtils::GetDayOfWeekMask(nWD);

		if (nWhichDayMask & nWDMask)
		{
			nFCount++;

			if (nFCount == nWhichDay)
			{
				rdtDate = dtYMD;
				return TRUE;
			}
		}
		dtYMD += spStep;
	}

	ASSERT(FALSE);
	return FALSE;
}

int CXTPCalendarUtils::GetWeekOfYear(COleDateTime dtDate, int nFirstDayOfWeekIndex)
{
	COleDateTime dtFirst(dtDate.GetYear(), 1, 1, 0, 0, 0);
	COleDateTime dtFirstWeekDate = ShiftDateToWeekBegin(dtFirst, nFirstDayOfWeekIndex);

	COleDateTimeSpan spDiff = dtDate - dtFirstWeekDate;

	int nDayOfYear = (int)spDiff.GetTotalDays();
	int nWeek = nDayOfYear/7 + 1;

	return nWeek;
}

BOOL CXTPCalendarUtils::IsWin9x()
{
	return XTPSystemVersion()->IsWin9x();
}

BOOL CXTPCalendarUtils::SystemTimeToTzSpecificLocalTime(
					const TIME_ZONE_INFORMATION* pTZI,
					const SYSTEMTIME* pUTime, SYSTEMTIME* pTzTime)
{
	if (!pTZI || !pUTime || !pTzTime)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!CXTPCalendarUtils::IsWin9x())
	{
		return ::SystemTimeToTzSpecificLocalTime((TIME_ZONE_INFORMATION*)pTZI,
												 (SYSTEMTIME*)pUTime, pTzTime);
	}

	//---------------------------------------------------------------------------
	COleDateTime dtUTime(*pUTime);
	COleDateTime dtTzTime = dtUTime;

	int nBias_min = pTZI->Bias;

	if (pTZI->StandardDate.wMonth)
	{
		nBias_min += pTZI->StandardBias;
	}

	BOOL bDayLight = FALSE;
	int nDLBias_min = 0;
	if (pTZI->DaylightDate.wMonth != 0 && pTZI->DaylightBias != 0)
	{
		bDayLight = TRUE;
		nDLBias_min = (pTZI->DaylightBias - pTZI->StandardBias);
	}

	dtTzTime = dtUTime - Minutes2Span(nBias_min);

	if (bDayLight && IsInDst(pTZI, dtTzTime))
	{
		dtTzTime -= Minutes2Span(nDLBias_min);
	}

	return GETASSYSTEMTIME_DT(dtTzTime, *pTzTime);
}

BOOL CXTPCalendarUtils::IsInDst(const TIME_ZONE_INFORMATION* pTZI,
								COleDateTime dtTime)
{
	if (pTZI->StandardDate.wMonth == 0 ||
		pTZI->DaylightDate.wMonth == 0 || pTZI->DaylightBias == 0)
	{
		return FALSE;
	}

	COleDateTime dtStandard(dtTime.GetYear(), pTZI->StandardDate.wMonth, 1,
						pTZI->StandardDate.wHour, pTZI->StandardDate.wMinute,
						pTZI->StandardDate.wSecond);

	COleDateTime dtDaylight(dtTime.GetYear(), pTZI->DaylightDate.wMonth, 1,
						pTZI->DaylightDate.wHour, pTZI->DaylightDate.wMinute,
						pTZI->DaylightDate.wSecond);

	BOOL bRes1 = CalcDayOfMonth(dtStandard, pTZI->StandardDate.wDay,
								GetDayOfWeekMask(pTZI->StandardDate.wDayOfWeek + 1));

	BOOL bRes2 = CalcDayOfMonth(dtDaylight, pTZI->DaylightDate.wDay,
								GetDayOfWeekMask(pTZI->DaylightDate.wDayOfWeek + 1));

	if (!bRes1 || !bRes2)
	{
		ASSERT(FALSE);
		return FALSE;
	}
//  TRACE(_T("dtStandard = %d-%d-%d || "), dtStandard.GetYear(), dtStandard.GetMonth(), dtStandard.GetDay());
//  TRACE(_T("dtDaylight = %d-%d-%d \n"), dtDaylight.GetYear(), dtDaylight.GetMonth(), dtDaylight.GetDay());
//  TRACE(_T("dtTime = %d-%d-%d \n"), dtTime.GetYear(), dtTime.GetMonth(), dtTime.GetDay());

	if (dtStandard < dtDaylight)
	{
		if (dtTime >= dtDaylight || dtTime < dtStandard)
		{
			return TRUE;
		}
	}
	else
	{
		if (dtTime >= dtDaylight && dtTime < dtStandard)
		{
			return TRUE;
		}
	}

	return FALSE;
}

COleDateTime CXTPCalendarUtils::GetCurrentTime()
{
	SYSTEMTIME st;
	::ZeroMemory(&st, sizeof(st));

	::GetLocalTime(&st);

	COleDateTime dtNow(st);

	return dtNow;
}

CString CXTPCalendarUtils::GetTimeFormatString_HrMin()
{
	////////////////////////////////////////////////////////////////////////////
	//  LOCALE_STIME    // time separator
	//
	//  LOCALE_ITLZERO  //leading zeros in time field
	//      0 No leading zeros for hours.
	//      1 Leading zeros for hours.
	//
	//  LOCALE_ITIME    // time format specifier
	//      0 AM / PM 12-hour format.
	//      1 24-hour format.
	//
	//  LOCALE_ITIMEMARKPOSN    // time marker position
	//      0 Use as suffix.
	//      1 Use as prefix.
	//
	//  LOCALE_S1159    // AM designator
	//  LOCALE_S2359    // PM designator
	////////////////////////////////////////////////////////////////////////////

	CString strTmp;

	CString strTimeSeparator = CXTPCalendarUtils::GetLocaleString(LOCALE_STIME, 16);

	strTmp = CXTPCalendarUtils::GetLocaleString(LOCALE_ITIME, 16);
	BOOL b24_HFormat = _ttoi(strTmp) == 1;

	strTmp = CXTPCalendarUtils::GetLocaleString(LOCALE_ITLZERO, 16);
	BOOL bH_LZero = _ttoi(strTmp) == 1;

	strTmp = CXTPCalendarUtils::GetLocaleString(LOCALE_ITIMEMARKPOSN, 16);
	BOOL bAM_PM_Prefix = _ttoi(strTmp) == 1;

	//---------------------------------------------------------------------------
	CString strTimeFormat;
	strTimeFormat += strTimeSeparator;
	strTimeFormat += _T("mm");

	if (b24_HFormat)
	{
		strTmp = bH_LZero ? _T("HH") : _T("H");
		strTimeFormat = strTmp + strTimeFormat;
	}
	else //12 AM/PM Format
	{
		strTmp = bH_LZero ? _T("hh") : _T("h");
		strTimeFormat = strTmp + strTimeFormat;

		if (bAM_PM_Prefix)
		{
			strTimeFormat = _T("tt ") + strTimeFormat;
		}
		else
		{
			strTimeFormat = strTimeFormat + _T(" tt");
		}
	}
	//---------------------------------------------------------------------------

	return strTimeFormat;
}

int CXTPCalendarUtils::ParceTimeString_min(LPCTSTR pcszTime_HrMin)
{
	CString strTime = pcszTime_HrMin;
	strTime.TrimLeft(); strTime.TrimRight();

	CString strPM = CXTPCalendarUtils::GetLocaleString(LOCALE_S2359, 20);
	strPM.MakeUpper();
	strTime.MakeUpper();

	// By default Am, or 24 hr format.
	BOOL bPM = !strPM.IsEmpty() && strTime.Find(strPM) >= 0;

	// remove prefixed AmPm and other non-digits in the beginning
	CString strTmp;
	strTmp = strTime.SpanExcluding(_T("0123456789"));
	int nTmpLen = strTmp.GetLength();
	if (nTmpLen)
	{
		DELETE_S(strTime, 0, nTmpLen);
	}

	//-------------------------------------------------
	CString strHr, strMin;
	strHr = strTime.SpanIncluding(_T("0123456789"));
	int nHrLen = strHr.GetLength();
	if (!nHrLen)
	{
		return -1;
	}
	DELETE_S(strTime, 0, nHrLen);

	CString strTimeSeparator = CXTPCalendarUtils::GetLocaleString(LOCALE_STIME, 16);
	TCHAR ctTimeSep = strTimeSeparator.GetLength() ? (TCHAR)strTimeSeparator[0] : (TCHAR)0;

	if (strTime.IsEmpty() || (strTime[0] != ctTimeSep && strTime[0] != _T(':')))
	{
		return -1;
	}
	DELETE_S(strTime, 0);

	strMin = strTime.SpanIncluding(_T("0123456789"));
	TCHAR* pChar1 = NULL, *pChar2 = NULL;
	int nHr = _tcstol(strHr, &pChar1, 10);
	int nMin = _tcstol(strMin, &pChar2, 10);

	int nResult_min = nHr * 60 + nMin;

	if (bPM && nResult_min <= 12 * 60)
		nResult_min += 12 * 60;

	if (nResult_min > 24 * 60)
		nResult_min = nResult_min % (24 * 60);

	return nResult_min;
}

CFont* CXTPCalendarUtils::GetMaxHeightFont(CArray<CFont*, CFont*>& arFonts, CDC* pDC,
										   int* pnMaxHeight)
{
	CFont* pFontMax = NULL;

	int nMaxHeight = 0;
	int nCount = (int)arFonts.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CFont* pFont = arFonts[i];
		if (pFont && pFont->m_hObject)
		{
			if (pDC)
			{
				CXTPFontDC autoFont(pDC, pFont);
				CSize szFi = pDC->GetTextExtent(_T("Aq"), 2);

				if (szFi.cy > nMaxHeight)
				{
					pFontMax = pFont;
					nMaxHeight = szFi.cy;
				}
			}
			else
			{
				LOGFONT lf;
				int nRes = pFont->GetLogFont(&lf);
				ASSERT(nRes);
				if (nRes)
				{
					int nHeight = abs(lf.lfHeight) + (lf.lfWeight < FW_BOLD ? 0 : 1);
					if (nHeight > nMaxHeight)
					{
						pFontMax = pFont;
						nMaxHeight = nHeight;
					}
				}
			}
		}
	}

	if (pnMaxHeight)
	{
		*pnMaxHeight = nMaxHeight;
	}
	return pFontMax;
}

CXTPCalendarUtils::CXTPTimesResourceIDs::CXTPTimesResourceIDs()
{
	s_mapMinutes.SetAt(0, XTP_IDS_CALENDAR_MINUTES_0);
	s_mapMinutes.SetAt(1, XTP_IDS_CALENDAR_MINUTES_1);
	s_mapMinutes.SetAt(5, XTP_IDS_CALENDAR_MINUTES_5);
	s_mapMinutes.SetAt(10, XTP_IDS_CALENDAR_MINUTES_10);
	s_mapMinutes.SetAt(15, XTP_IDS_CALENDAR_MINUTES_15);
	s_mapMinutes.SetAt(30, XTP_IDS_CALENDAR_MINUTES_30);

	s_mapHours.SetAt(1, XTP_IDS_CALENDAR_HOURS_1);
	s_mapHours.SetAt(2, XTP_IDS_CALENDAR_HOURS_2);
	s_mapHours.SetAt(3, XTP_IDS_CALENDAR_HOURS_3);
	s_mapHours.SetAt(4, XTP_IDS_CALENDAR_HOURS_4);
	s_mapHours.SetAt(5, XTP_IDS_CALENDAR_HOURS_5);
	s_mapHours.SetAt(6, XTP_IDS_CALENDAR_HOURS_6);
	s_mapHours.SetAt(7, XTP_IDS_CALENDAR_HOURS_7);
	s_mapHours.SetAt(8, XTP_IDS_CALENDAR_HOURS_8);
	s_mapHours.SetAt(9, XTP_IDS_CALENDAR_HOURS_9);
	s_mapHours.SetAt(10, XTP_IDS_CALENDAR_HOURS_10);
	s_mapHours.SetAt(11, XTP_IDS_CALENDAR_HOURS_11);
	s_mapHours.SetAt(12, XTP_IDS_CALENDAR_DAYS_0_5);
	s_mapHours.SetAt(18, XTP_IDS_CALENDAR_HOURS_18);

	s_mapDays.SetAt(1, XTP_IDS_CALENDAR_DAYS_1);
	s_mapDays.SetAt(2, XTP_IDS_CALENDAR_DAYS_2);
	s_mapDays.SetAt(3, XTP_IDS_CALENDAR_DAYS_3);

	s_mapWeeks.SetAt(1, XTP_IDS_CALENDAR_WEEKS_1);
	s_mapWeeks.SetAt(2, XTP_IDS_CALENDAR_WEEKS_2);
}

CString CXTPCalendarUtils::LoadString(UINT nIDResource)
{
	CString str;
	VERIFY(XTPResourceManager()->LoadString(&str, nIDResource));
	return str;
}

BOOL CXTPCalendarUtils::LoadString2(CString& rstrString, UINT nIDResource)
{
	CString str;
	BOOL bFind = XTPResourceManager()->LoadString(&str, nIDResource);
	if (bFind)
	{
		rstrString = str;
	}
	return bFind;
}

CString CXTPCalendarUtils::FormatTimeDuration(int nMinutes, BOOL bRoundly)
{
	static CXTPTimesResourceIDs s_RCData;

	int nWeeks = nMinutes / (7*24*60);
	int nWeeks_r = nMinutes % (7*24*60);

	int nDays = nMinutes / (24*60);
	int nDays_r = nMinutes % (24*60);

	int nHours = nMinutes / 60;
	int nHours_r = nMinutes % 60;

	CString strDuration;
	UINT nStrID = 0;

	if ((bRoundly || nWeeks_r == 0) && nWeeks)
	{
		if (s_RCData.s_mapWeeks.Lookup(nWeeks, nStrID) && nStrID)
			strDuration = LoadString(nStrID);
		else
			strDuration.Format(_T("%d %s"), nWeeks, (LPCTSTR)LoadString(XTP_IDS_CALENDAR_WEEKS));
	}
	else if ((bRoundly  || nDays_r == 0) && nDays)
	{
		if (s_RCData.s_mapDays.Lookup(nDays, nStrID) && nStrID)
			strDuration = LoadString(nStrID);
		else
			strDuration.Format(_T("%d %s"), nDays, (LPCTSTR)LoadString(XTP_IDS_CALENDAR_DAYS));
	}
	else if ((bRoundly || nHours_r == 0) && nHours)
	{
		if (s_RCData.s_mapHours.Lookup(nHours, nStrID) && nStrID)
			strDuration = LoadString(nStrID);
		else
			strDuration.Format(_T("%d %s"), nHours, (LPCTSTR)LoadString(XTP_IDS_CALENDAR_HOURS));
	}
	else
	{
		if (s_RCData.s_mapMinutes.Lookup(nMinutes, nStrID) && nStrID)
			strDuration = LoadString(nStrID);
		else
			strDuration.Format(_T("%d %s"), nMinutes, (LPCTSTR)LoadString(XTP_IDS_CALENDAR_MINUTES));
	}

	return strDuration;
}

int CXTPCalendarUtils::ParceTimeDuration_min(LPCTSTR pcszTime)
{
	CString strTime = pcszTime;

	strTime.TrimLeft(); strTime.TrimRight();

	CString strValue, strScale;
	strValue = strTime.SpanIncluding(_T("-+0123456789,."));
	int nValueLen = strValue.GetLength();
	if (!nValueLen)
	{
		return INT_MIN;
	}
	DELETE_S(strTime, 0, nValueLen);

	strScale = strTime;
	strScale.TrimLeft();

	TCHAR* pChar = NULL;
	REPLACE_S(strValue, _T(","), _T("."));
	double dValue = _tcstod(strValue, &pChar);

	CStringArray arSIname;
	CStringArray arSIname2;
	CUIntArray   arSIvalue;

	arSIname.Add(LoadString(XTP_IDS_CALENDAR_TIME_PREFIX_MINUTES));
	arSIname2.Add(_T("min"));
	arSIvalue.Add(1);

	arSIname.Add(LoadString(XTP_IDS_CALENDAR_TIME_PREFIX_HOURS));
	arSIname2.Add(_T("h"));
	arSIvalue.Add(60);

	arSIname.Add(LoadString(XTP_IDS_CALENDAR_TIME_PREFIX_DAYS));
	arSIname2.Add(_T("d"));
	arSIvalue.Add(24*60);

	arSIname.Add(LoadString(XTP_IDS_CALENDAR_TIME_PREFIX_WEEKS));
	arSIname2.Add(_T("w"));
	arSIvalue.Add(7*24*60);

	int nCount = (int)arSIname.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		if (_tcsnicmp(strScale, arSIname[i], arSIname[i].GetLength()) == 0 ||
			_tcsnicmp(strScale, arSIname2[i], arSIname2[i].GetLength()) == 0)
		{
			return (int)(dValue * arSIvalue[i]);
		}
	}

	return (int)dValue; // minutes
}
