// XTPCalendarCustomDataProvider.cpp: implementation of the
// CXTPCalendarCustomProperties class.
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
#include "XTPCalendarCustomDataProvider.h"
#include "XTPCalendarNotifications.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPCalendarCustomDataProvider, CXTPCalendarData)

CXTPCalendarCustomDataProvider::CXTPCalendarCustomDataProvider()
{
	m_typeProvider = xtpCalendarDataProviderCustom;

	SetCacheMode(xtpCalendarDPCacheModeOnRepeat);
}

CXTPCalendarCustomDataProvider::~CXTPCalendarCustomDataProvider()
{
}

void CXTPCalendarCustomDataProvider::Close()
{
	if (!IsOpen())
	{
		CXTPCalendarData::Open();
	}
	CXTPCalendarData::Close();
}

CXTPCalendarEventsPtr CXTPCalendarCustomDataProvider::DoRetrieveDayEvents(COleDateTime dtDay)
{
	CXTPCalendarEventsPtr ptrEvents;
	WPARAM wParam = (XTP_DATE_VALUE)(double)dtDay;
	LPARAM lParam = (LPARAM)&ptrEvents;

	SendNotification(XTP_NC_CALENDAR_DoRetrieveDayEvents, wParam, lParam);
	return ptrEvents;
}

void CXTPCalendarCustomDataProvider::DoRemoveAllEvents()
{
	SendNotification(XTP_NC_CALENDAR_DoRemoveAllEvents, 0, 0);
}

CXTPCalendarEventPtr CXTPCalendarCustomDataProvider::DoRead_Event(DWORD dwEventID)
{
	CXTPCalendarEventPtr ptrEvent;

	WPARAM wParam = (WPARAM)dwEventID;
	LPARAM lParam = (LPARAM)&ptrEvent;

	SendNotification(XTP_NC_CALENDAR_DoRead_Event, wParam, lParam);
	return ptrEvent;
}

CXTPCalendarRecurrencePatternPtr CXTPCalendarCustomDataProvider::DoRead_RPattern(DWORD dwPatternID)
{
	CXTPCalendarRecurrencePatternPtr ptrPattern;

	WPARAM wParam = (WPARAM)dwPatternID;
	LPARAM lParam = (LPARAM)&ptrPattern;

	SendNotification(XTP_NC_CALENDAR_DoRead_RPattern, wParam, lParam);
	return ptrPattern;
}

BOOL CXTPCalendarCustomDataProvider::DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID)
{
	BOOL bResult = -1;

	XTP_CALENDAR_WPARAM2 wParamX2;
	wParamX2.wParam1 = (WPARAM)pEvent;
	wParamX2.wParam2 = (WPARAM)&rdwNewEventID;

	WPARAM wParam = (WPARAM)&wParamX2;
	LPARAM lParam = (LPARAM)&bResult;

	SendNotification(XTP_NC_CALENDAR_DoCreate_Event, wParam, lParam);
	return bResult > 0;
}

BOOL CXTPCalendarCustomDataProvider::DoUpdate_Event (CXTPCalendarEvent* pEvent)
{
	BOOL bResult = -1;

	WPARAM wParam = (WPARAM)pEvent;
	LPARAM lParam = (LPARAM)&bResult;

	SendNotification(XTP_NC_CALENDAR_DoUpdate_Event, wParam, lParam);
	return bResult > 0;
}

BOOL CXTPCalendarCustomDataProvider::DoDelete_Event (CXTPCalendarEvent* pEvent)
{
	BOOL bResult = -1;

	WPARAM wParam = (WPARAM)pEvent;
	LPARAM lParam = (LPARAM)&bResult;

	SendNotification(XTP_NC_CALENDAR_DoDelete_Event, wParam, lParam);
	return bResult > 0;
}

BOOL CXTPCalendarCustomDataProvider::DoCreate_RPattern(CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID)
{
	BOOL bResult = -1;

	XTP_CALENDAR_WPARAM2 wParamX2;
	wParamX2.wParam1 = (WPARAM)pPattern;
	wParamX2.wParam2 = (WPARAM)&rdwNewPatternID;

	WPARAM wParam = (WPARAM)&wParamX2;
	LPARAM lParam = (LPARAM)&bResult;

	SendNotification(XTP_NC_CALENDAR_DoCreate_RPattern, wParam, lParam);
	return bResult > 0;
}


BOOL CXTPCalendarCustomDataProvider::DoUpdate_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	BOOL bResult = -1;

	WPARAM wParam = (WPARAM)pPattern;
	LPARAM lParam = (LPARAM)&bResult;

	SendNotification(XTP_NC_CALENDAR_DoUpdate_RPattern, wParam, lParam);
	return bResult > 0;
}

BOOL CXTPCalendarCustomDataProvider::DoDelete_RPattern(CXTPCalendarRecurrencePattern* pPattern)
{
	BOOL bResult = -1;

	WPARAM wParam = (WPARAM)pPattern;
	LPARAM lParam = (LPARAM)&bResult;

	SendNotification(XTP_NC_CALENDAR_DoDelete_RPattern, wParam, lParam);
	return bResult > 0;
}

CXTPCalendarEventsPtr CXTPCalendarCustomDataProvider::DoGetAllEvents_raw()
{
	CXTPCalendarEventsPtr ptrEvents;
	WPARAM wParam = 0;
	LPARAM lParam = (LPARAM)&ptrEvents;

	SendNotification(XTP_NC_CALENDAR_DoGetAllEvents_raw, wParam, lParam);
	return ptrEvents;
}

CXTPCalendarEventsPtr CXTPCalendarCustomDataProvider::DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod)
{
	CXTPCalendarEventsPtr ptrEvents;

	XTP_CALENDAR_WPARAM2 wParamX2;
	wParamX2.wParam1 = (WPARAM)&dtFrom;
	wParamX2.wParam2 = (WPARAM)&spPeriod;

	WPARAM wParam = (WPARAM)&wParamX2;
	LPARAM lParam = (LPARAM)&ptrEvents;

	SendNotification(XTP_NC_CALENDAR_DoGetUpcomingEvents, wParam, lParam);
	return ptrEvents;
}
