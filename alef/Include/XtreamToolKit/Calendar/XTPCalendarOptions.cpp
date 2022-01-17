// XTPCalendarOptions.cpp: implementation of the CXTPCalendarOptions class.
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
#include "Common/XTPNotifyConnection.h"

#include "XTPCalendarOptions.h"
#include "XTPCalendarNotifications.h"

#include "XTPCalendarData.h"
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarDatabaseDataProvider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// void CXTPCalendarOptions
IMPLEMENT_DYNAMIC(CXTPCalendarOptions, CCmdTarget)

CXTPCalendarOptions::CXTPCalendarOptions()
{
	// default settings
	nWorkWeekMask = xtpCalendarDayMo_Fr;
	nFirstDayOfTheWeek = xtpCalendarDayMonday;
	dtWorkDayStartTime.SetTime(8, 0, 0);
	dtWorkDayEndTime.SetTime(17, 0, 0);

	bEnableInPlaceEditEventSubject_ByF2 = TRUE;
	bEnableInPlaceEditEventSubject_ByMouseClick = TRUE;
	bEnableInPlaceEditEventSubject_ByTab = TRUE;
	bEnableInPlaceEditEventSubject_AfterEventResize = TRUE;

	bEnableInPlaceCreateEvent = TRUE;
	bUseOutlookFontGlyphs = FALSE;

	nDayView_ScaleInterval = 30;
	bDayView_AutoResetBusyFlag = TRUE;

	bDayView_Scale2Visible = FALSE;
	//strDayView_ScaleLabel;
	//strDayView_Scale2Label;
	::ZeroMemory(&tziDayView_Scale2TimeZone, sizeof(tziDayView_Scale2TimeZone));
	VERIFY( ::GetTimeZoneInformation(&tziDayView_Scale2TimeZone) != TIME_ZONE_ID_INVALID);

	nDayView_CurrentTimeMarkVisible = xtpCalendarCurrentTimeMarkVisibleForToday;
	bDayView_TimeScaleShowMinutes = FALSE;

	bMonthView_CompressWeekendDays = TRUE;
	bMonthView_ShowEndDate = FALSE;
	bMonthView_ShowTimeAsClocks = FALSE;

	bWeekView_ShowEndDate = FALSE;
	bWeekView_ShowTimeAsClocks = FALSE;

	m_pDataProvider = NULL;

}

CXTPCalendarOptions::~CXTPCalendarOptions()
{
	//CMDTARGET_RELEASE(m_pDataProvider);
}

void CXTPCalendarOptions::DoPropExchange(CXTPPropExchange* pPX)
{
	long nSchema = 3;
	PX_Long(pPX, _T("Version"), nSchema, 3);

	PX_Int(pPX, _T("WorkWeekMask"), nWorkWeekMask, xtpCalendarDayMo_Fr);
	PX_Int(pPX, _T("FirstDayOfTheWeek"), nFirstDayOfTheWeek, xtpCalendarDayMonday);
	PX_DateTime(pPX, _T("WorkDayStartTime"), dtWorkDayStartTime);
	PX_DateTime(pPX, _T("WorkDayEndTime"), dtWorkDayEndTime);

	PX_Bool(pPX, _T("EnableInPlaceEditEventSubject_ByF2"), bEnableInPlaceEditEventSubject_ByF2, TRUE);
	PX_Bool(pPX, _T("EnableInPlaceEditEventSubject_ByMouseClick"), bEnableInPlaceEditEventSubject_ByMouseClick, TRUE);
	PX_Bool(pPX, _T("EnableInPlaceEditEventSubject_ByTab"), bEnableInPlaceEditEventSubject_ByTab, TRUE);
	PX_Bool(pPX, _T("EnableInPlaceEditEventSubject_AfterEventResize"), bEnableInPlaceEditEventSubject_AfterEventResize, TRUE);

	PX_Bool(pPX, _T("EnableInPlaceCreateEvent"), bEnableInPlaceCreateEvent, TRUE);

	if (nSchema >= 2)
	{
		PX_Bool(pPX, _T("UseOutlookFontGlyphs"), bUseOutlookFontGlyphs, FALSE);
	}
	else if (pPX->IsLoading())
	{
		bUseOutlookFontGlyphs = FALSE;
	}

	PX_Bool(pPX, _T("DayView_AutoResetBusyFlag"), bDayView_AutoResetBusyFlag, TRUE);
	PX_Int(pPX, _T("DayView_ScaleInterval"), nDayView_ScaleInterval, 30);
	PX_String(pPX, _T("DayView_ScaleLabel"), strDayView_ScaleLabel, _T(""));
	PX_String(pPX, _T("DayView_Scale2Label"), strDayView_Scale2Label, _T(""));
	PX_Bool(pPX, _T("DayView_Scale2Visible"), bDayView_Scale2Visible, FALSE);

	if (nSchema >= 3)
	{
		PX_Int(pPX, _T("DayView_CurrentTimeMarkVisible"), nDayView_CurrentTimeMarkVisible, xtpCalendarCurrentTimeMarkVisibleForToday);
	}
	else if (pPX->IsLoading())
	{
		nDayView_CurrentTimeMarkVisible = xtpCalendarCurrentTimeMarkVisibleForToday;
	}

	PX_Bool(pPX, _T("DayView_TimeScaleShowMinutes"), bDayView_TimeScaleShowMinutes, FALSE);

	BYTE* pTZIdata = (BYTE*)&tziDayView_Scale2TimeZone;
	DWORD dwBytes = sizeof(tziDayView_Scale2TimeZone);
	PX_Blob(pPX, _T("DayView_Scale2TimeZone"), pTZIdata, dwBytes);

	PX_Bool(pPX, _T("MonthView_CompressWeekendDays"), bMonthView_CompressWeekendDays, TRUE);
	PX_Bool(pPX, _T("MonthView_ShowEndDate"), bMonthView_ShowEndDate, FALSE);
	PX_Bool(pPX, _T("MonthView_ShowTimeAsClocks"), bMonthView_ShowTimeAsClocks, FALSE);

	PX_Bool(pPX, _T("WeekView_ShowEndDate"), bWeekView_ShowEndDate, FALSE);
	PX_Bool(pPX, _T("WeekView_ShowTimeAsClocks"), bWeekView_ShowTimeAsClocks, FALSE);
}

CXTPCalendarTimeZonePtr CXTPCalendarOptions::GetCurrentTimeZoneInfo()
{
	TIME_ZONE_INFORMATION tziCurrent;
	::ZeroMemory(&tziCurrent, sizeof(tziCurrent));

	if (::GetTimeZoneInformation(&tziCurrent) == TIME_ZONE_ID_INVALID)
	{
		ASSERT(FALSE);
		return NULL;
	}

	return CXTPCalendarTimeZone::GetTimeZoneInfo(&tziCurrent);
}

void CXTPCalendarOptions::OnOptionsChanged()
{
	XTP_SAFE_CALL2(m_pDataProvider, GetConnection(), SendEvent(XTP_NC_CALENDAROPTIONSWASCHANGED, (WPARAM)-1, 0));
}

///////////////////////////////////////////////////////////////////////////
// Data part
//

void CXTPCalendarOptions::SetDataProvider(CXTPCalendarData* pDataProvider)
{
	// free old data provider
	if (m_pDataProvider)
	{
		m_pDataProvider->SetOptionsToUpdate(NULL);
	}

	// set a new one
	if (pDataProvider)
	{
		m_pDataProvider = pDataProvider;

		if (m_pDataProvider)
		{
			m_pDataProvider->SetOptionsToUpdate(this);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

