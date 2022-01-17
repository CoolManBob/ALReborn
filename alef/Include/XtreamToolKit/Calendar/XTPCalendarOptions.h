// XTPCalendarOptions.h: interface for the CXTPCalendarOptions class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(_XTPCALENDAROPTIONS_H__)
#define _XTPCALENDAROPTIONS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "XTPCalendarTimeZoneHelper.h"
#include "XTPCalendarDefines.h"
//#include "XTPCalendarEvent.h"

class CXTPCalendarData;
class CXTPPropExchange;
//class CXTPCalendarRecurrencePattern;
//class CXTPCalendarEventLabels;
//class CXTPNotifyConnection;

//===========================================================================
// Summary:
//     Structure contains all calendar options.
// Remarks:
//     This structure contains all options which could be changed by the
//     user for CXTPCalendarControl.
// See Also: CXTPCalendarControl, XTPCalendarWeekDay, XTPCalendarWeekDay
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarOptions : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarOptions)
	//}}AFX_CODEJOCK_PRIVATE

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarOptions();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default Destructor.
	// Remarks:
	//     Handles all deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarOptions();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an storage.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	// Remarks:
	//     For the save case the options data from will be saved to archive.
	//     For the load a new data will be loaded from the specified
	//     archive and set to members.
	// See Also:
	//     CXTPPropExchange
	//-----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get full information about current time zone.
	// Remarks:
	//      Retrieve additional information from the registry.
	// Returns:
	//      A smart pointer to CXTPCalendarTimeZone object.
	// See Also:
	//      GetTimeZoneInformation(), CXTPCalendarTimeZone::GetTimeZoneInfo()
	//-----------------------------------------------------------------------
	CXTPCalendarTimeZonePtr GetCurrentTimeZoneInfo();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called when option was changed.
	//-----------------------------------------------------------------------
	void OnOptionsChanged();

	/////////////////////////////////////////////////////////////////////////
	// data provider related

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the custom data provider for the control.
	// Parameters:
	//     pDataProvider - Pointer to the custom data provider object.
	//     lpszConnectionString - String containing the name and type of the data provider.
	// Remarks:
	//     Call this member function to set the custom data provider
	//     that is currently used by this calendar control. Note that
	//     custom data provider must be a descendant of CXTPCalendarData.
	// See Also: CXTPCalendarData overview, GetDataProvider
	//-----------------------------------------------------------------------
	void SetDataProvider(CXTPCalendarData* pDataProvider);

public:
	//## Calendar work week
	int nWorkWeekMask;                  // This structure member represents week working days using XTPCalendarWeekDay enumeration.
	                                    // Each day is represented by the corresponding binary bit.
	int nFirstDayOfTheWeek;             // This member shows the first day of the week using XTPCalendarWeekDay enum.
	COleDateTime dtWorkDayStartTime;    // This member contains work day start time.
	COleDateTime dtWorkDayEndTime;      // This member contains work day end time.

	BOOL bEnableInPlaceEditEventSubject_ByF2;           // Set TRUE to enable in-place edit event subject by F2, FALSE otherwise.
	BOOL bEnableInPlaceEditEventSubject_ByMouseClick;   // Set TRUE to enable in-place edit event subject by Mouse Click, FALSE otherwise.
	BOOL bEnableInPlaceEditEventSubject_ByTab;          // Set TRUE to enable in-place edit event subject by TAB, FALSE otherwise.
	BOOL bEnableInPlaceEditEventSubject_AfterEventResize;// Set TRUE to enable in-place edit event subject after event resize, FALSE otherwise.

	BOOL bEnableInPlaceCreateEvent; // Set TRUE to enable in-place event creation, FALSE otherwise.
	BOOL bUseOutlookFontGlyphs;     // Set TRUE to use 'MS Outlook' font to display Glyps, otherwise bitmaps are used.

	//## Day View
	BOOL bDayView_AutoResetBusyFlag;    // If TRUE - 'Busy' event status will be automatically set to 'Free' when moving event from hours area to all day events area and vice versa. If FALSE - status flag is not changed automatically.
	int nDayView_ScaleInterval;         // DayView scale interval in minutes.

	CString strDayView_ScaleLabel;                   // Stores main time scale label (day view)
	CString strDayView_Scale2Label;                  // Stores secondary time scale label (day view)
	BOOL bDayView_Scale2Visible;                     // TRUE when secondary time scale is visible in day view, FALSE otherwise.
	TIME_ZONE_INFORMATION tziDayView_Scale2TimeZone; // Stores time zone information for the secondary time scale.

	int nDayView_CurrentTimeMarkVisible; // A set of flags which define when Current Time Mark on the timescale is visible. See Also XTPCalendarCurrentTimeMarkFlags. By default it is xtpCalendarCurrentTimeMarkVisibleForToday.
	BOOL bDayView_TimeScaleShowMinutes;  // If TRUE - minutes will be shown on time scale. FALSE by default.

	//## Month View
	BOOL bMonthView_CompressWeekendDays;// TRUE when compressing weekend days in month view, FALSE otherwise.
	BOOL bMonthView_ShowEndDate;        // TRUE when showing event end date in month view, FALSE otherwise.
	BOOL bMonthView_ShowTimeAsClocks;   // TRUE when showing event time as graphical clocks in month view, FALSE otherwise.

	//## Week View
	BOOL bWeekView_ShowEndDate;         // TRUE when showing event end date in week view, FALSE otherwise.
	BOOL bWeekView_ShowTimeAsClocks;    // TRUE when showing event time as graphical clocks in week view, FALSE otherwise.

protected:
	CXTPCalendarData* m_pDataProvider;  // A stored pointer to the owner data provider.

};

#endif // !defined(_XTPCALENDAROPTIONS_H__)
