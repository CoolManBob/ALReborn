// XTPCalendarDefines.h: interface for the CXTPCalendarEvent class.
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
#if !defined(_XTPCALENDARDEFINES_H__)
#define _XTPCALENDARDEFINES_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE

//#define DBG_TRACE_TIMER TRACE
//#define DBG_DATA_ASSERT ASSERT
//#define DBG_DATA_VERIFY VERIFY

//---------------
#ifndef DBG_TRACE_TIMER
#define DBG_TRACE_TIMER
#endif

#ifndef DBG_DATA_ASSERT
#define DBG_DATA_ASSERT
#endif

#ifndef DBG_DATA_VERIFY
#define DBG_DATA_VERIFY
#endif
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////
class CXTPCalendarEvent;
class CXTPCalendarEvents;
class CXTPCalendarRecurrencePattern;

class CXTPCalendarEventLabel;
class CXTPCalendarEventLabels;

#include "XTPCalendarUtils.h"

typedef long XTP_DATE_VALUE;

//===========================================================================
// Summary:
//     Define unknown event ID.
//===========================================================================
#define XTP_CALENDAR_UNKNOWN_EVENT_ID 0

//===========================================================================
// Summary:
//     Define an unknown recurrence pattern ID.
//===========================================================================
#define XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID 0

//===========================================================================
// Summary:
//     Define unknown schedule ID.
//===========================================================================
#define XTP_CALENDAR_UNKNOWN_SCHEDULE_ID 0

//===========================================================================
// Summary:
//     Define maximum text length for calendar event subject, location or body
//     which will be drawn on the screen.
//     Used to optimize drawing performance.
//===========================================================================
#define XTP_CALENDAR_DRAW_EVENTITEMTEXT_MAX 255

//===========================================================================
// Summary:
//     Define light and dark colors which are used by
//     CXTPImageManager::DisableBitmap to draw disabled calendar window content.
//===========================================================================
#define XTP_CALENDAR_DISABLED_COLOR_LIGHT   RGB(250, 250, 250)
#define XTP_CALENDAR_DISABLED_COLOR_DARK    RGB(128, 128, 128)  //<COMBINE XTP_CALENDAR_DISABLED_COLOR_LIGHT>

//===========================================================================
// Summary:
//     Enumerates busy status values.
// Example:
// <code>
// CXTPCalendarData* pData;
// CXTPCalendarEventPtr ptrEvent = pData->CreateNewEvent();
// ptrEvent->SetBusyStatus(xtpCalendarBusyStatusTentative);
// VERIFY(pCalendarData->AddEvent(ptrEvent));
// </code>
// See Also: CXTPCalendarEvent::SetBusyStatus
//===========================================================================
enum XTPCalendarEventBusyStatus
{
	xtpCalendarBusyStatusUnknown        = -1,   // Typed unknown value.
	xtpCalendarBusyStatusFree           = 0,    // Free event status.
	xtpCalendarBusyStatusTentative      = 1,    // Tentative event status.
	xtpCalendarBusyStatusBusy           = 2,    // Busy event status.
	xtpCalendarBusyStatusOutOfOffice    = 3     // OutOfOffice event status.
};

//===========================================================================
// Summary:
//     Enumerates importance level values.
// Example:
// <code>
// CXTPCalendarData* pData;
// CXTPCalendarEventPtr ptrEvent = pData->CreateNewEvent();
// ptrEvent->SetImportance(xtpCalendarImportanceHigh);
// VERIFY(pCalendarData->AddEvent(ptrEvent));
// </code>
// See Also: CXTPCalendarEvent::SetImportance
//===========================================================================
enum XTPCalendarEventImportance
{
	xtpCalendarImportanceUnknown        = -1,   // Typed unknown value.
	xtpCalendarImportanceLow            = 0,    // Low importance level.
	xtpCalendarImportanceNormal         = 1,    // Normal importance level.
	xtpCalendarImportanceHigh           = 2     // High importance level.
};

//===========================================================================
// Summary:
//     Enumerates recurrence states.
//===========================================================================
enum XTPCalendarEventRecurrenceState
{
	xtpCalendarRecurrenceNotRecurring   = 0,    // Event is not recurring.
	xtpCalendarRecurrenceMaster         = 1,    // Event is a Master (template to generate occurrences).
	xtpCalendarRecurrenceOccurrence     = 2,    // Event is Occurrence.
	xtpCalendarRecurrenceException      = 3     // Event is Exception (changed occurrence).
};

//===========================================================================
// Summary:
//     Type definition for an array of dates.
//===========================================================================
typedef CArray<COleDateTime, COleDateTime&> CXTPCalendarDatesArray;

//===========================================================================
// Summary:
//     Specify the end of the recurrence pattern type.
//===========================================================================
enum XTPCalendarPatternEnd
{
	xtpCalendarPatternEndDate              , // End of the recurrence pattern specified by the date.
	xtpCalendarPatternEndAfterOccurrences  , // End of the recurrence pattern specified by the occurrences amount.
	xtpCalendarPatternEndNoDate              // Recurrence pattern has no End.
};

//===========================================================================
// Summary:
//     Enumerates Recurrence Pattern Types.
//===========================================================================
enum XTPCalendarEventRecurrenceType
{
	xtpCalendarRecurrenceUnknown   = 0,     // Typed unknown value.

	xtpCalendarRecurrenceDaily     = 1,     // Every N days. N = { 1, 2 .. }
	xtpCalendarRecurrenceWeekly    = 2,     // Every N weeks on specified 'DaysMask'.
	                                        //   N = { 1, 2 .. },
	                                        //   DaysMask = {see enum XTPCalendarWeekDay}
	xtpCalendarRecurrenceMonthly   = 3,     // D day of every N months.
	                                        //   D = { 1, 2 .. 31}
	                                        //   N = { 1, 2 .. }
	xtpCalendarRecurrenceMonthNth  = 4,     // The first, second, ... X of every N months.
	                                        //   first, second, ... = {see enum XTPCalendarWeek}
	                                        //   X = {see enums XTPCalendarWeekDay}
	                                        //   N = { 1, 2 .. }
	xtpCalendarRecurrenceYearly    = 5,     // Every month(M) day(D).
	                                        //   M = { 1, 2 .. 12}
	                                        //   D = { 1, 2 .. 31}
	xtpCalendarRecurrenceYearNth   = 6      // The first, second, ... X of month(M).
	                                        //   first, second, ... = {see enum XTPCalendarWeek}
	                                        //   X = {see enums XTPCalendarWeekDay}
	                                        //   M = { 1, 2 .. 12}
};

//===========================================================================
// Summary:
//     Enumerates Day of Weeks codes.
//===========================================================================
enum XTPCalendarWeekDay
{
	xtpCalendarDaySunday        = 0x001,        // Sunday
	xtpCalendarDayMonday        = 0x002,        // Monday
	xtpCalendarDayTuesday       = 0x004,        // Tuesday
	xtpCalendarDayWednesday     = 0x008,        // Wednesday
	xtpCalendarDayThursday      = 0x010,        // Thursday
	xtpCalendarDayFriday        = 0x020,        // Friday
	xtpCalendarDaySaturday      = 0x040,        // Saturday

	xtpCalendarDayAllWeek       = xtpCalendarDayMonday | xtpCalendarDayTuesday |
								  xtpCalendarDayWednesday | xtpCalendarDayThursday |
								  xtpCalendarDayFriday | xtpCalendarDaySaturday |
								  xtpCalendarDaySunday, // All week mask.

	xtpCalendarDaySaSu          = xtpCalendarDaySaturday | xtpCalendarDaySunday, // Saturday, Sunday mask.
	xtpCalendarDayMo_Fr         = xtpCalendarDayAllWeek ^ xtpCalendarDaySaSu     // Monday to Friday mask.
};

//===========================================================================
// Summary:
//     Enumerates a sequence numbers for days.
//===========================================================================
enum XTPCalendarWeek
{
	xtpCalendarWeekFirst    = 1, // First
	xtpCalendarWeekSecond   = 2, // Second
	xtpCalendarWeekThird    = 3, // Third
	xtpCalendarWeekFourth   = 4, // Fourth
	xtpCalendarWeekLast     = 5  // Last
};

//===========================================================================
// Summary:
//     Enumeration used to determine the calendar data provider in use.
// Remarks:
//     This enumeration contains all available data provider types.
// See Also:
//     CXTPCalendarData, CXTPCalendarMemoryDataProvider,
//     CXTPCalendarDatabaseDataProvider
//===========================================================================
enum XTPCalendarDataProvider
{
	xtpCalendarDataProviderUnknown  = 0,    // Unknown data provider

	xtpCalendarDataProviderMemory   = 1,    // Memory data provider - CXTPCalendarMemoryDataProvider.
	xtpCalendarDataProviderDatabase = 2,    // Database data provider - CXTPCalendarDatabaseDataProvider.
	xtpCalendarDataProviderMAPI     = 3,    // MAPI (Exchange server) data provider - CXTPCalendarMAPIDataProvider.
	xtpCalendarDataProviderCustom   = 4     // Custom data provider - CXTPCalendarCustomDataProvider.
};

//===========================================================================
// Summary:
//     Enumeration used to determine the calendar view type in use.
// Remarks:
//     This enumeration contains all available Calendar view types.
// See Also:
//     CXTPCalendarView, CXTPCalendarDayView, CXTPCalendarWeekView,
//     CXTPCalendarMonthView
//===========================================================================
enum XTPCalendarViewType
{
	xtpCalendarDayView,     // Day view - CXTPCalendarDayView
	xtpCalendarWorkWeekView, // Day view - CXTPCalendarDayView. Work week days are shown.
	xtpCalendarWeekView,    // Week view - CXTPCalendarWeekView
	xtpCalendarMonthView    // Month view - CXTPCalendarMonthView
};

//===========================================================================
// Summary:
//     Enumeration of operational mouse modes.
// Remarks:
//     Calendar Control has several
//     Mouse states that handled by control. This enumeration helps to
//     clearly identify each of these
//     - Sends Notifications:
//     - Sends Messages:
// See Also:
//===========================================================================
enum XTPCalendarMouseMode
{
	xtpCalendarMouseNothing,                // User is doing nothing - just watching to the control.
	xtpCalendarMouseEventPreResizeV,        // User is moving mouse under event view vertical "resize" area.
	xtpCalendarMouseEventPreResizeH,        // User is moving mouse under event view horizontal "resize" area.
	xtpCalendarMouseEventPreDrag,           // User is moving mouse under event view "drag" area.
	xtpCalendarMouseEventResizingV,         // User is resizing event vertically (holding mouse left button down).
	xtpCalendarMouseEventResizingH,         // User is resizing event horizontally (holding mouse left button down).
	xtpCalendarMouseEventDragCopy,          // User is dragging event view (holding left mouse button down).
	xtpCalendarMouseEventDragMove,          // User is dragging event view (holding left mouse button down and Control key is down).
	xtpCalendarMouseEventDraggingOut        // User is dragging event out of dragging area.
};

//===========================================================================
// Summary:
//     Contains bit-masks for adjusting the calendar control.
// Remarks:
//     This enumeration contains bit masks for using with
//     CXTPCalendarControl::CUpdateContext.
//     Depending on members of this enum there will be specific
//     redrawing functionality called on exiting adjuster's scope.
//===========================================================================
enum XTPCalendarUpdateOptions
{
	xtpCalendarUpdateLayout    = 0x0001,    // Adjust positions of control's elements.
	xtpCalendarUpdateScrollBar = 0x0002,    // Recalculate control's scrollbar positions.
	xtpCalendarUpdateRedraw    = 0x0004,    // Redraw control picture.
	xtpCalendarUpdateAll       = 0x000F,    // Redraw all portions of the control on exiting adjuster scope.
	xtpCalendarUpdateRedrawIfNeed      = 0x0010    // Redraw control's picture if needed.
};

//===========================================================================
// Summary:
//     This enumeration contains options for
//     CXTPCalendarControl::CViewChangedContext.
//===========================================================================
enum XTPCalendarViewChangedUpdateOptions
{
	xtpCalendarViewChangedSend  = 1, // Set send request for XTP_NC_CALENDARVIEWWASCHANGED notification and lock sending.
	xtpCalendarViewChangedLock  = 2  // Lock sending only, send request for XTP_NC_CALENDARVIEWWASCHANGED notification is not set.
};


//===========================================================================
// Summary:
//     Enumerates allowable hit test codes.
// Remarks:
//     ResizeV, ResizeH flags are used together with
//     ResizeBegin or ResizeEnd.
//===========================================================================
enum XTPCalendarHitTestCode
{
	xtpCalendarHitTestUnknown          = 0,        // The position is unknown or does not belong to our control.

	xtpCalendarHitTestDayArea          = 0x0001,   // The position is inside the days area.
	xtpCalendarHitTestDayHeader        = 0x0002,   // The position is inside control's header.
	xtpCalendarHitTestDayExpandButton  = 0x0004,   // The position is over Expand day button icon.
	xtpCalendarHitTestGroupArea        =  xtpCalendarHitTestDayArea, // The position is inside a group area (formerly day).
	xtpCalendarHitTestGroupHeader      = 0x0008,   // The position is inside Group (resource) header.
	xtpCalendarHitTestDay_Mask         = 0x000F,   // The mask for any position inside days area with the header.

	xtpCalendarHitTestEventDragArea    = 0x0010,   // The position is on Event drag area.
	xtpCalendarHitTestEventTextArea    = 0x0020,   // The position is on Event text view area.

	xtpCalendarHitTestEventResizeVArea = 0x0100,   // Event vertical resize area.
	xtpCalendarHitTestEventResizeHArea = 0x0200,   // Event horizontal resize area.
	xtpCalendarHitTestEventResizeArea_Mask = xtpCalendarHitTestEventResizeVArea | xtpCalendarHitTestEventResizeHArea, // The mask for both event resize areas.

	xtpCalendarHitTestEventResizeBegin = 0x0400,   // Event start resizing area.
	xtpCalendarHitTestEventResizeEnd   = 0x0800,   // Event end resizing area.

	xtpCalendarHitTestEvent_Mask       = 0x0FF0,   // The mask for event area at whole.

	xtpCalendarHitTestDayViewTimeScale  = 0x01000,   // Time scale part of the day view.
	xtpCalendarHitTestDayViewCell       = 0x02000,   // Day view cells - its general part.
	xtpCalendarHitTestDayViewAllDayEvent= 0x04000,  // All day zone on the day view.

	xtpCalendarHitTestDayViewScrollUp   = 0x10000,  // DayView Scroll Up button. (presents in office 2007 theme only)
	xtpCalendarHitTestDayViewScrollDown = 0x20000,  // DayView Scroll Down button. (presents in office 2007 theme only)

	xtpCalendarHitTestDayView_Mask      = 0xFF000    // The mask for all DayView areas.

};

//===========================================================================
// Summary:
//     Enumerates allowable dragging mode state codes.
//===========================================================================
enum XTPCalendarDraggingMode
{
	xtpCalendaDragModeUnknown     = 0, // No dragging mode.

	xtpCalendaDragModeCopy        = 1, // Copying event.
	xtpCalendaDragModeMove        = 2, // Moving event.

	xtpCalendaDragModeResizeBegin = 3, // Resizing event begin.
	xtpCalendaDragModeResizeEnd   = 4, // Resizing event end.

	xtpCalendaDragModeEditSubject = 5  // Editing event subject.
};

//===========================================================================
// Summary:
//     Enumerates allowable flags for driving subject editors.
//===========================================================================
enum XTPCalendarEndEditSubjectAction
{
	xtpCalendarEditSubjectUnknown    = 0,    // Action is undefined.
	xtpCalendarEditSubjectCommit     = 1,    // Apply and commit dragging.
	xtpCalendarEditSubjectCancel     = 2     // Cancel editing and dragging changes.
};

//===========================================================================
// Summary:
//     Enumerates flags to identify undo mode.
//===========================================================================
enum XTPCalendarUndoMode
{
	xtpCalendarUndoModeUnknown = 0,    // Undo mode is undefined.
	xtpCalendarUndoModeUndo    = 1,    // Undo action is pending.
	xtpCalendarUndoModeRedo    = 2     // Redo action is pending.
};

//===========================================================================
// Summary:
//     Enumerates additional flags used in clipboard operations.
//===========================================================================
enum XTPCalendarClipboardCopyFlags
{
	xtpCalendarCopyFlagROccurrence2RException = 0x0001, // Convert recurrence Occurrence to Exception before paste.
	xtpCalendarCopyFlagKeepRException         = 0x0002, // Keep recurrence Exception event state before paste.
	xtpCalendarCopyFlagClearCBAfterPaste      = 0x0004, // Clear clipboard after paste.

	xtpCalendarCopyFlagCutMask = xtpCalendarCopyFlagROccurrence2RException |
							xtpCalendarCopyFlagKeepRException         |
							xtpCalendarCopyFlagClearCBAfterPaste  // Default flags for Cut operation.

};

//===========================================================================
// Summary:
//     Enumerates parameters used in configuring reminders for recurrence
//     patterns occurrences.
//===========================================================================
enum XTPEnumCalendarReminderForOccurrenceParameters
{
	xtpCalendarRmdPrm_Default       = LONG_MAX, // Set default reminders value.
	xtpCalendarRmdPrm_DontChange    = LONG_MIN  // Do not change reminders time value.
};

//===========================================================================
// Summary:
//     Enumerates flags to CXTPCalendarOptions.nDayView_CurrentTimeMarkVisible
//     member.
//     This set of flags define when Current Time Mark on the timescale is
//     visible.
//===========================================================================
enum XTPCalendarCurrentTimeMarkFlags
{
	xtpCalendarCurrentTimeMarkNone              = 0, // Time Mark invisible.

	xtpCalendarCurrentTimeMarkVisibleForToday   = 1, // Time Mark visible if today day is visible.
	xtpCalendarCurrentTimeMarkVisibleAlways     = 2, // Time Mark always visible.
	xtpCalendarCurrentTimeMarkPrinted           = 4  // If this flag set - Time Mark is visible for printed calendar also, otherwise it is hidden when printing.
};

//===========================================================================
// Summary:
//     Icons IDs set to be used with calendar event custom icons to customize
//     standard event glyphs.
//===========================================================================
enum XTPCalendarEventIconIDs
{
	xtpCalendarEventIconIDReminder      = -1, // Reminder glyph ID.
	xtpCalendarEventIconIDOccurrence    = -2, // Recurrence glyph ID.
	xtpCalendarEventIconIDException     = -3, // Recurrence exception glyph ID.
	xtpCalendarEventIconIDMeeting       = -4, // Meeting glyph ID.
	xtpCalendarEventIconIDPrivate       = -5  // Private glyph ID.
};

class CXTPCalendarViewDay;
class CXTPCalendarViewGroup;
class CXTPCalendarViewEvent;

//===========================================================================
// Summary:
//     Structure contains the calendar day view selection block.
// Remarks:
//     This structure describes a selection bounds on the calendar day view.
// See Also: CXTPCalendarDayView
//===========================================================================
struct XTP_CALENDAR_VIEWSELECTION
{
//{{AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Remarks:
	//     Initializes a XTP_CALENDAR_VIEWSELECTION object to the empty
	//     selection bounds.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_VIEWSELECTION()
	{
		nGroupIndex = -1;
		bAllDayEvent = FALSE;
		dtBegin.SetStatus(COleDateTime::null);
		dtEnd.SetStatus(COleDateTime::null);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Copy operator.
	// Parameters:
	//     rSrc - The source object.
	// Remarks:
	//     Initialize members from the specified object.
	// Returns:
	//     Reference to the current object.
	//-----------------------------------------------------------------------
	const XTP_CALENDAR_VIEWSELECTION& operator=(const XTP_CALENDAR_VIEWSELECTION& rSrc)
	{
		nGroupIndex = rSrc.nGroupIndex;
		bAllDayEvent = rSrc.bAllDayEvent;

		if(rSrc.dtBegin.GetStatus() == COleDateTime::valid) {
			dtBegin = rSrc.dtBegin;
		}
		else
		{
			dtBegin.SetStatus(rSrc.dtBegin.GetStatus());
		}

		if(rSrc.dtEnd.GetStatus() == COleDateTime::valid) {
			dtEnd = rSrc.dtEnd;
		}
		else
		{
			dtEnd.SetStatus(rSrc.dtEnd.GetStatus());
		}
		return *this;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines are times members values valid.
	// Returns:
	//     TRUE if date time object members are valid, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsValid() const {
		return dtBegin.GetStatus() == COleDateTime::valid &&
			   dtEnd.GetStatus() == COleDateTime::valid &&
			   nGroupIndex >= 0;
	}

public:
//}}AFX_CODEJOCK_PRIVATE

	int nGroupIndex;        // Numeric index of the corresponding resource group.
	COleDateTime dtBegin;   // Selection start time.
	COleDateTime dtEnd;     // Selection finish time.
	BOOL bAllDayEvent;      // TRUE when all day is selected, FALSE otherwise.
};


//===========================================================================
// Summary:
//     Basic set of parameters transferred from calendar items to
//     mouse handlers.
// Remarks:
//     This structure groups together all essential parameters used in
//     items processing activities. It is a basic structure for all
//     particular calendar's structures.
// See Also: XTP_CALENDAR_HITTESTINFO_DAY_VIEW, XTP_CALENDAR_HITTESTINFO_WEEK_VIEW,
//          XTP_CALENDAR_HITTESTINFO_MONTH_VIEW
//===========================================================================
struct XTP_CALENDAR_HITTESTINFO
{
	COleDateTime            dt;             // Time stamp on the specified point.
	BOOL                    bTimePartValid; // Specifies whether date's time part is valid,
	                                        // i.e. if we hit on all day events area, bTimePartValid will be FALSE.
	int                     uHitCode;       // A bitwise combination of XTPCalendarHitTestCode constants.

	int                     nDay;           // Number of the day.
	int                     nGroup;         // Number of the group.
	int                     nEvent;         // ID of the specified event.

	CXTPCalendarViewDay*    pViewDay;       // Pointer to affected CXTPCalendarViewDay object.
	CXTPCalendarViewGroup*  pViewGroup;     // Pointer to affected CXTPCalendarViewGroup object.
	CXTPCalendarViewEvent*  pViewEvent;     // Pointer to affected CXTPCalendarViewEvent object.

//{{AFX_CODEJOCK_PRIVATE
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Remarks:
	//     Initializes a XTP_CALENDAR_HITTESTINFO object to the empty
	//     and zero values.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_HITTESTINFO()
	{
		dt.SetStatus(COleDateTime::null);
		bTimePartValid = FALSE;
		uHitCode = xtpCalendarHitTestUnknown;
		pViewDay = NULL;
		pViewGroup = NULL;
		pViewEvent = NULL;
		nDay = -1;
		nGroup = -1;
		nEvent = -1;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Default destructor.
	//-----------------------------------------------------------------------
	virtual ~XTP_CALENDAR_HITTESTINFO()
	{
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Equality operator.
	// Parameters:
	//     hitInfo2 - Second XTP_CALENDAR_HITTESTINFO object to compare.
	// Remarks:
	//     Compare 2 XTP_CALENDAR_HITTESTINFO objects.
	// Returns:
	//     TRUE if specified objects are equal, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL operator==(const XTP_CALENDAR_HITTESTINFO& hitInfo2)
	{
		return  (dt.GetStatus() == hitInfo2.dt.GetStatus()) &&
					((dt.GetStatus() == COleDateTime::valid) &&
						(CXTPCalendarUtils::IsEqual(dt, hitInfo2.dt) ||
						(dt.GetStatus() != COleDateTime::valid))
					) &&
				(bTimePartValid == hitInfo2.bTimePartValid) &&
				(uHitCode == hitInfo2.uHitCode) &&
				(pViewDay == hitInfo2.pViewDay) &&
				(pViewGroup == hitInfo2.pViewGroup) &&
				(pViewEvent == hitInfo2.pViewEvent) &&
				(nDay == hitInfo2.nDay) &&
				(nGroup == hitInfo2.nGroup) &&
				(nEvent == hitInfo2.nEvent);
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Non-Equality operator.
	// Parameters:
	//     hitInfo2 - Second XTP_CALENDAR_HITTESTINFO object to compare.
	// Remarks:
	//     Compare 2 XTP_CALENDAR_HITTESTINFO objects.
	// Returns:
	//     TRUE if specified objects are not equal, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL operator!=(const XTP_CALENDAR_HITTESTINFO& hitInfo2) {
		return !(*this == hitInfo2);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Copy operator.
	// Parameters:
	//     hitInfo2 - The source object.
	// Remarks:
	//     Initialize members from the specified object.
	// Returns:
	//     Reference to the current object.
	//-----------------------------------------------------------------------
	const XTP_CALENDAR_HITTESTINFO& operator=(const XTP_CALENDAR_HITTESTINFO& hitInfo2)
	{
		dt = hitInfo2.dt;
		bTimePartValid = hitInfo2.bTimePartValid;
		uHitCode = hitInfo2.uHitCode;
		pViewDay = hitInfo2.pViewDay;
		pViewGroup = hitInfo2.pViewGroup;
		pViewEvent = hitInfo2.pViewEvent;
		nDay = hitInfo2.nDay;
		nGroup = hitInfo2.nGroup;
		nEvent = hitInfo2.nEvent;

		return *this;
	};
//}}AFX_CODEJOCK_PRIVATE
};


//===========================================================================
// Summary:
//     Enumeration used to determine the scroll direction.
// Remarks:
//     The enumeration determines if scrolling is necessary based on the
//     scroll mode(up, down, or NotNeeded.)
// See Also: CXTPCalendarDayView
//===========================================================================
enum XTPCalendarDayViewScrollDirection
{
	xtpCalendarDayViewScrollUp,         // ScrollV Up.
	xtpCalendarDayViewScrollDown,       // ScrollV Down.
	xtpCalendarDayViewScrollNotNeeded   // Scrolling not needed.
};

//===========================================================================
// Summary:
//     Enhanced set of parameters for hit test operations on
//     calendar day view area.
// Remarks:
//     This enumeration enhances XTP_CALENDAR_HITTESTINFO set of parameters
//     by specific calendar day view items.
// See Also: XTP_CALENDAR_HITTESTINFO
//===========================================================================
struct XTP_CALENDAR_HITTESTINFO_DAY_VIEW : public XTP_CALENDAR_HITTESTINFO
{
	POINT pt;   // Point where the mouse was clicked in client coordinates.

//{{AFX_CODEJOCK_PRIVATE
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Remarks:
	//     Initializes a XTP_CALENDAR_HITTESTINFO_DAY_VIEW object to the
	//     empty values.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_HITTESTINFO_DAY_VIEW()
	{
		pt.x = pt.y = -1;
	}
//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//     Set of parameters transferred from calendar items to
//     mouse handlers.
// See Also: XTP_CALENDAR_HITTESTINFO
//===========================================================================
struct XTP_CALENDAR_HITTESTINFO_WEEK_VIEW : public XTP_CALENDAR_HITTESTINFO
{
};


//===========================================================================
// Summary:
//     Defines a set of data and methods to easily manipulate with day
//     positions in month view. Used to convert Week/Day indexes pair to
//     sequential integer value and vice versa.
//===========================================================================
struct XTP_CALENDAR_MONTHVIEW_DAYPOS
{
	int nWeekIndex;     // Week index. From 0 to XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX
	int nWeekDayIndex;  // Week day cell index. From 0 to 6

//{{AFX_CODEJOCK_PRIVATE
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     nPOS - Initial sequential integer value.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_MONTHVIEW_DAYPOS(int nPOS = -1) {
		nWeekIndex = nWeekDayIndex  = -1;
		if (nPOS >= 0) {
			SetPOS(nPOS);
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the sequential integer value.
	// Remarks:
	//     This is the overloaded int operator.
	// Returns:
	//     Sequential integer value.
	// See Also: GetPOS
	//-----------------------------------------------------------------------
	operator int();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new Sequential integer value.
	// Parameters:
	//     nPOS - An int that contains the new value.
	// Remarks:
	//     This is the overloaded assignment operator.
	// Returns:
	//     An XTP_CALENDAR_MONTHVIEW_DAYPOS object reference.
	// See Also: SetPOS
	//-----------------------------------------------------------------------
	const XTP_CALENDAR_MONTHVIEW_DAYPOS& operator=(int nPOS);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to pre-increment a sequential
	//     position by 1.
	// Remarks:
	//     This is the overloaded pre-increment operator.(++i)
	// Returns:
	//     This object reference incremented by 1.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_MONTHVIEW_DAYPOS& operator++();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to post-increment a sequential
	//     position by 1.
	// Remarks:
	//     This is the overloaded post-increment operator.(i++)
	// Returns:
	//     Object value before incrementing.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_MONTHVIEW_DAYPOS operator++(int);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a sequential integer value.
	// Returns:
	//     An int that represents the sequential integer value.
	// See Also: SetPOS, operator int()
	//-----------------------------------------------------------------------
	int GetPOS();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new week/day index value.
	// Parameters:
	//     nWidx   - An int that contains the Week index.
	//               Valid values are from 0 to XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX
	//     nWDidx  - An int that contains the Week day cell index.
	//               Valid values are From 0 to 6.
	// See Also: operator=(int nPOS), SetPOS(int nPOS)
	//-----------------------------------------------------------------------
	void SetPOS(int nWidx, int nWDidx);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new sequential integer value.
	// Parameters:
	//     nPOS - An int that contains the new integer value.
	// See Also: operator=(int nPOS), SetPOS(int nWidx, int nWDidx)
	//-----------------------------------------------------------------------
	void SetPOS(int nPOS);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the WeekIndex and
	//     WeekDayIndex values are valid.
	// Remarks:
	//     The valid values are nWeekIndex >= 0 || nWeekDayIndex >= 0.
	// Returns:
	//     TRUE if values are valid. Otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsValid();

//}}AFX_CODEJOCK_PRIVATE
};


class CXTPCalendarMonthViewDay;
class CXTPCalendarMonthViewEvent;

//===========================================================================
// Summary:
//     Set of parameters transferred from calendar items to
//     mouse handlers.
// Remarks:
//     This structure groups together all essential parameters used in
//     items processing activities. It is a basic structure for all
//     particular calendar's structures.
// See Also: XTP_CALENDAR_HITTESTINFO, XTP_CALENDAR_MONTHVIEW_DAYPOS
//===========================================================================
struct XTP_CALENDAR_HITTESTINFO_MONTH_VIEW : public XTP_CALENDAR_HITTESTINFO
{
	XTP_CALENDAR_MONTHVIEW_DAYPOS dayPOS; // Day position.
};

//===========================================================================
// Summary:
//     Enumerates DB Provider cache modes.
//===========================================================================
enum XTPCalendarDataProviderCacheMode
{
	xtpCalendarDPCacheModeOff       = 0x000, // Cache is not used.

	xtpCalendarDPCacheModeOnRepeat  = 0x001, // Repeat all event manipulations in cache.
	xtpCalendarDPCacheModeOnClear   = 0x002, // Clear cache after every event manipulation.

	xtpCalendarDPCacheModeOnMask    = 0x00F, // Cache mode on mask.
};


//===========================================================================
// Summary:
//     Define unknown event label ID.
//===========================================================================
#define XTP_CALENDAR_UNKNOWN_LABEL_ID -1

//===========================================================================
// Summary:
//     Define event label ID which is used by default for a new event.
//     (Valid for default labels list only)
//===========================================================================
#define XTP_CALENDAR_DEFAULT_LABEL_ID 0

//===========================================================================
// Summary:
//     Define event label ID which signifies no label.
//     (Valid for default labels list only)
//===========================================================================
#define XTP_CALENDAR_NONE_LABEL_ID 0

//===========================================================================
// Summary:
//     Define default reminders value minutes which is for a new event.
//===========================================================================
#define XTP_CALENDAR_DEFAULT_REMINDER_MINUTES 15

//===========================================================================
// Summary:
//      The location in the registry that contains time zone information
//      for Windows NT family (NT/2000/XP).
// See also:
//      Microsoft KB221542
//===========================================================================
#define XTP_CALENDAR_TIMEZONESKEY_NT _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones")

//===========================================================================
// Summary:
//      The location in the registry that contains time zone information
//      for Windows 9x family.
// See also:
//      Microsoft KB221542
//===========================================================================
#define XTP_CALENDAR_TIMEZONESKEY_9X _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Time Zones")

//===========================================================================
// Summary:
//      Registry key name for a StandardName member value of
//      TIME_ZONE_INFORMATION.
// See also:
//      TIME_ZONE_INFORMATION WinAPI structure
//===========================================================================
#define XTP_CALENDAR_TZIRegValName_StandardName _T("Std")
// <COMBINE XTP_CALENDAR_TZIRegValName_StandardName>
#define XTP_CALENDAR_TZIRegValName_StandardNameW L"Std"

//===========================================================================
// Summary:
//      Registry key name for a DaylightName member value of
//      TIME_ZONE_INFORMATION.
// See also:
//      TIME_ZONE_INFORMATION WinAPI structure
//===========================================================================
#define XTP_CALENDAR_TZIRegValName_DaylightName _T("Dlt")
// <COMBINE XTP_CALENDAR_TZIRegValName_DaylightName>
#define XTP_CALENDAR_TZIRegValName_DaylightNameW L"Dlt"

//===========================================================================
// Summary:
//      Registry key name for a time zone display string value.
//      For example: "(GMT+02:00) Athens, Beirut, Istanbul, Minsk"
//===========================================================================
#define XTP_CALENDAR_TZIRegValName_DisplayStr   _T("Display")

//===========================================================================
// Summary:
//      Registry key name for a time zone order index value.
// Remarks:
//      This index value is used to sort time zones in the right order.
//===========================================================================
#define XTP_CALENDAR_TZIRegValName_Index        _T("Index")

//===========================================================================
// Summary:
//      Registry key name for some TIME_ZONE_INFORMATION structure members.
// See also:
//      Microsoft KB115231,
//      CXTPCalendarTimeZones::REGISTRY_TIMEZONE_INFORMATION
//===========================================================================
#define XTP_CALENDAR_TZIRegValName_DATA         _T("TZI")

//===========================================================================
//{{AFX_CODEJOCK_PRIVATE
#define XTP_EVENT_PLACE_POS_UNDEFINED 0xFFFF
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
#define XTP_SAFE_GET1(_ptr1, _member, _defVal) (_ptr1 ? _ptr1->_member : (_defVal))
#define XTP_SAFE_GET2(_ptr1, _ptr2, _member, _defVal) ((_ptr1 && _ptr1->_ptr2) ? _ptr1->_ptr2->_member : (_defVal) )
#define XTP_SAFE_GET3(_ptr1, _ptr2, _ptr3, _member, _defVal) ((_ptr1 && _ptr1->_ptr2 && _ptr1->_ptr2->_ptr3) ? _ptr1->_ptr2->_ptr3->_member : (_defVal) )
#define XTP_SAFE_GET4(_ptr1, _ptr2, _ptr3, _ptr4, _member, _defVal) ((_ptr1 && _ptr1->_ptr2 && _ptr1->_ptr2->_ptr3 && _ptr1->_ptr2->_ptr3->_ptr4) ? _ptr1->_ptr2->_ptr3->_ptr4->_member : (_defVal) )
#define XTP_SAFE_GET5(_ptr1, _ptr2, _ptr3, _ptr4, _ptr5, _member, _defVal) ((_ptr1 && _ptr1->_ptr2 && _ptr1->_ptr2->_ptr3 && _ptr1->_ptr2->_ptr3->_ptr4 && _ptr1->_ptr2->_ptr3->_ptr4->_ptr5) ? _ptr1->_ptr2->_ptr3->_ptr4->_ptr5->_member : (_defVal) )

#define XTP_SAFE_SET1(_ptr1, _member, _Val) \
	if (_ptr1){ \
		_ptr1->_member = _Val; \
	}

#define XTP_SAFE_SET2(_ptr1, _ptr2, _member, _Val) \
	if (_ptr1 && _ptr1->_ptr2){ \
		_ptr1->_ptr2->_member = _Val; \
	}

#define XTP_SAFE_CALL1(_ptr1, _member) \
	if (_ptr1){ \
		_ptr1->_member; \
	}

#define XTP_SAFE_CALL2(_ptr1, _ptr2, _member) \
	if (_ptr1 && _ptr1->_ptr2){ \
		_ptr1->_ptr2->_member; \
	}

#define XTP_SAFE_CALL3(_ptr1, _ptr2, _ptr3, _member) \
	if (_ptr1 && _ptr1->_ptr2 && _ptr1->_ptr2->_ptr3){ \
		_ptr1->_ptr2->_ptr3->_member; \
	}

#define XTP_SAFE_CALL4(_ptr1, _ptr2, _ptr3, _ptr4, _member) \
	if (_ptr1 && _ptr1->_ptr2 && _ptr1->_ptr2->_ptr3 && \
		_ptr1->_ptr2->_ptr3->_ptr4) \
	{ \
		_ptr1->_ptr2->_ptr3->_ptr4->_member; \
	}

#define XTP_SAFE_CALL5(_ptr1, _ptr2, _ptr3, _ptr4, _ptr5, _member) \
	if (_ptr1 && _ptr1->_ptr2 && _ptr1->_ptr2->_ptr3 && \
		_ptr1->_ptr2->_ptr3->_ptr4 && _ptr1->_ptr2->_ptr3->_ptr4->_ptr5) \
	{ \
		_ptr1->_ptr2->_ptr3->_ptr4->_ptr5->_member; \
	}
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      The WM_XTP_CALENDAR_BASE constant is used by Calendar Control to help
//      define private messages for use by private window classes, usually of
//      the form WM_XTP_CALENDAR_BASE+X, where X is an integer value.
// See Also:
//      WM_USER overview
//===========================================================================
const UINT WM_XTP_CALENDAR_BASE = (WM_USER + 9600);

static const LPCTSTR cszEventCustProp_NextReminderTime_Snoozed = _T("*xtp*NextReminderTime_Snoozed"); // String name of the corresponding property.
static const LPCTSTR cszEventCustProp_IconsIDs = _T("*xtp*CustomIcons"); // String name of the corresponding property.


//===========================================================================
// Summary:
//      This structure describes parameters of the end of the recurrence
//      pattern type.
//===========================================================================
struct XTP_CALENDAR_PATTERN_END
{
//{{AFX_CODEJOCK_PRIVATE
	//-----------------------------------------------------------------------
	// Summary:
	//    Default constructor of the structure.
	//    Handles members initialization.
	// See Also: ~XTP_CALENDAR_PATTERN_END
	//-----------------------------------------------------------------------
	XTP_CALENDAR_PATTERN_END()
	{
		m_nUseEnd = xtpCalendarPatternEndNoDate;

		m_dtPatternEndDate = (DATE)0;
		m_nEndAfterOccurrences = 1;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//    Default destructor of the structure.
	//    Handles members deallocation.
	// See Also: XTP_CALENDAR_PATTERN_END
	//-----------------------------------------------------------------------
	virtual ~XTP_CALENDAR_PATTERN_END(){};

	//-----------------------------------------------------------------------
	// Summary:
	//     This assignment operator is used to initialize a
	//     XTP_CALENDAR_PATTERN_END object.
	// Parameters:
	//     rSrc - Source value.
	// Returns:
	//     A reference to a valid XTP_CALENDAR_PATTERN_END object.
	//-----------------------------------------------------------------------
	const XTP_CALENDAR_PATTERN_END& operator=(const XTP_CALENDAR_PATTERN_END& rSrc)
	{
		m_nUseEnd = rSrc.m_nUseEnd;

		if (rSrc.m_dtPatternEndDate.GetStatus() == COleDateTime::valid)
		{
			m_dtPatternEndDate = rSrc.m_dtPatternEndDate;
		}
		else
		{
			m_dtPatternEndDate.SetStatus(rSrc.m_dtPatternEndDate.m_status);
		}

		m_nEndAfterOccurrences = rSrc.m_nEndAfterOccurrences;

		return *this;
	}
//}}AFX_CODEJOCK_PRIVATE

public:
	XTPCalendarPatternEnd   m_nUseEnd;              // End type.

	COleDateTime            m_dtPatternEndDate;     // End date.
	int                     m_nEndAfterOccurrences; // End number of occurrences.
};

//////////////////////////////////////////////////////////////////////////

// Custom class name for the Calendar control window
static const LPCTSTR XTPCALENDARCTRL_CLASSNAME = _T("XTPCalendar");

// for compatibility with previous versions
// see xtp_wm_UserAction
//
//{{AFX_CODEJOCK_PRIVATE
// String name of the calendar switch view custom message
static const LPCTSTR XTPCALENDARCTRL_SWITCHVIEWMSG = _T("XTPCalendarSwitchView");
// A message identifier of the calendar switch view custom message
const UINT xtp_wm_SwitchView = RegisterWindowMessage( XTPCALENDARCTRL_SWITCHVIEWMSG );
//}}AFX_CODEJOCK_PRIVATE

// String name of the calendar switch view custom message
static const LPCTSTR XTPCALENDARCTRL_USERACTION = _T("XTPCalendarUserAction");
const UINT xtp_wm_UserAction = RegisterWindowMessage( XTPCALENDARCTRL_USERACTION );

//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_XTPCALENDARDEFINES_H__)
