// XTPCalendarDayView.h: interface for the CXTPCalendarDayView class.
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
#if !defined(_XTPCALENDARDAYVIEW_H__)
#define _XTPCALENDARDAYVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPCalendarDayViewTimeScale;

#include "XTPCalendarView.h"
#include "XTPCalendarDayViewDay.h"
#include "XTPCalendarTimeZoneHelper.h"

//===========================================================================
// Summary:
//     This class implements a specific view portion of the Calendar
//     control - called <b>Day view</b>.
// Remarks:
//          The Day view consists of one or more days, placed in a
//          row. Each day is stretched down by a number of fixed time
//          intervals. Each day is represented by a CXTPCalendarDayViewDay class.
//
//          The Day view also contains two time scales defining those intervals
//          at the left of the view, which is represented by the
//          CXTPCalendarDayViewTimeScale class. One time scale is the main and
//          is visible by default, second one is the alternative and can be
//          enabled programmatically. Time scale interval can be changed
//          using the SetScaleInterval member function. The content of the view
//          refreshes automatically.
//
// See Also: CXTPCalendarView, CXTPCalendarWeekView, CXTPCalendarMonthView,
//          CXTPCalendarDayViewDay, CXTPCalendarDayViewEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarDayView : public CXTPCalendarViewT<CXTPCalendarDayViewDay,
													 XTP_CALENDAR_HITTESTINFO_DAY_VIEW>
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarDayViewDay;
	friend class CXTPCalendarDayViewGroup;
	friend class CXTPCalendarDayViewTimeScale;

	friend class CXTPCalendarControl;
	friend class CXTPCalendarControlView;
	friend class CXTPCalendarTheme;

	DECLARE_DYNAMIC(CXTPCalendarDayView)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//------------------------------------------------------------------------
	// Remarks:
	//     Base class type definition
	//------------------------------------------------------------------------
	typedef CXTPCalendarViewT< CXTPCalendarDayViewDay,
								XTP_CALENDAR_HITTESTINFO_DAY_VIEW> TBase;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pCalendarControl - Pointer to CXTPCalendarControl object.
	// See Also: ~CXTPCalendarEvent()
	//-----------------------------------------------------------------------
	CXTPCalendarDayView(CXTPCalendarControl* pCalendarControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles class members deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarDayView();


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the view type flag.
	// Returns:
	//     An XTPCalendarViewType object that contains the view type flag.
	// See Also: XTPCalendarViewType
	//-----------------------------------------------------------------------
	virtual XTPCalendarViewType GetViewType();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout
	//     depending on the window's client dimensions and calls.
	// Parameters:
	//     rcView - A CRect that contains the bounding rectangle dimensions
	//              of the view.
	//     bCallPostAdjustLayout - TRUE for calling additional post-adjustments
	// Remarks:
	//     Call Populate() prior adjusting layout. Adjust layout of all
	//     sub-items.
	//     AdjustLayout2 is called by calendar control instead of AdjustLayout
	//     when theme is set.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout = TRUE);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout = TRUE); //<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to retrieve information about a
	//     scroll bar's state.
	// Parameters:
	//     pSI - Pointer to scrollbar information structure.
	// Returns: A BOOL.
	//          TRUE if the visible row count is not the same as the actual row count.
	//          FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL GetScrollBarInfoV(SCROLLINFO* pSI);
	virtual BOOL GetScrollBarInfoH(SCROLLINFO* pSI, int* pnScrollStep = NULL); //<COMBINE GetScrollBarInfoV@SCROLLINFO*>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the content of a view.
	// Parameters:
	//     nPos     - Position value.
	//     nPos_raw - Raw position value. (Not used at this time.)
	//-----------------------------------------------------------------------
	virtual void ScrollV(int nPos, int nPos_raw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the content of a view.
	// Parameters:
	//     nPos     - Position value.
	//     nPos_raw - Raw position value. (Not used at this time.)
	//-----------------------------------------------------------------------
	virtual void ScrollH(int nPos, int nPos_raw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns values which interpreted as a minimum
	//     column width, where column is a group view object.
	// Remarks:
	//     The returned number may be one of the following values:
	//      [ul]
	//      [li] 0 means disabled;
	//      [li] -1 use some default width for multiresources only;
	//      [li] -N use for multiresources only (internally converted to +N);
	//      [li] +N used always (in single and multi resources mode).
	//      [/ul]
	//
	// Returns:
	//     Minimum column width in pixels (may be negative) or 0 or -1.
	// See Also: SetMinColumnWidth
	//-----------------------------------------------------------------------
	virtual int GetMinColumnWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function used to set minimum column width, where column
	//     is a group view object.
	// Parameters:
	//     nWidth - [in] Minimum column width in pixels (may be negative) or 0 or -1.
	//              See remarks section.
	// Remarks:
	//     nWidth number may be one of the following values:
	//      [ul]
	//      [li] 0 means disabled;
	//      [li] -1 use some default width for multiresources only;
	//      [li] -N use for multiresources only (internally converted to +N);
	//      [li] +N used always (in single and multi resources mode).
	//      [/ul]
	//
	// See Also: GetMinColumnWidth
	//-----------------------------------------------------------------------
	virtual void SetMinColumnWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function performs all of the drawing logic of the
	//     calendar view.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view content using the
	//     specified device context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before Draw()
	//     Draw2 is called by calendar control instead of Draw when theme
	//     is set.
	// See Also: AdjustLayout
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);
	virtual void Draw2(CDC* pDC); //<COMBINE Draw>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process left mouse button events.
	// Parameters:
	//     nFlags  - Indicates whether various virtual keys are down.
	//     point   - Specifies the x- and y- coordinate of the cursor.
	//     These coordinates are always relative to the upper-left corner
	//     of the window.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     presses the left mouse button.
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to processes left mouse button events.
	// Parameters:
	//     nFlags  - A UINT that is used to indicate whether various virtual keys are down.
	//     point   - A CPoint that specifies the x- and y- coordinates of the cursor.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     releases the left mouse button.
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process mouse move events.
	// Parameters:
	//     nFlags  - Indicates whether various virtual keys are down.
	//     point   - Specifies the x- and y- coordinate of the cursor.
	//     These coordinates are always relative to the upper-left
	//     corner of the window.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     moves the mouse cursor or stylus.
	//-----------------------------------------------------------------------
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process keyboard events.
	// Parameters:
	//     nChar   - Virtual key code of the given key.
	//     nRepCnt - Number of times the keystroke is repeated as a result
	//               of the user holding down the key.
	//     nFlags  - Scan code, key-transition code, previous key state,
	//               and context code.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     presses keys on the keyboard.
	//-----------------------------------------------------------------------
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the date of a day view by the
	//     day's index.
	// Parameters:
	//     nIndex  - Day view index in the view collection.
	// Returns:
	//     COleDateTime object with the day view date and time.
	// Remarks:
	//     Index number starts at 0 and cannot be negative.
	// See Also: GetViewDayCount()
	//-----------------------------------------------------------------------
	virtual COleDateTime GetViewDayDate(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine which view item,
	//     if any, is at a specified position index, and returns
	//     additional info in a XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pt       - Coordinates of point to test.
	//     pHitTest - Pointer to a XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Returns:
	//     TRUE if item found. FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	//-----------------------------------------------------------------------
	virtual BOOL HitTestEx(CPoint pt, XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the visible row count
	//     on the view.
	// Returns:
	//     An int containing the count of the visible rows on the view.
	//-----------------------------------------------------------------------
	int GetVisibleRowCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the top visible row on the view.
	// Returns:
	//     An int that contains the number of the top visible row on the view.
	//-----------------------------------------------------------------------
	int GetTopRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the total row count on the view.
	// Returns:
	//     An int containing the total count of rows on the view.
	//-----------------------------------------------------------------------
	int GetRowCount() const;

	/////////////////////////////////////////////////////////////////////////
	// time scale related

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the time scale item interval.
	// Returns:
	//     A COleDateTimeSpan object that contains the time interval of
	//     the time scale item.
	//-----------------------------------------------------------------------
	COleDateTimeSpan GetScaleInterval() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the time scale item interval.
	// Parameters:
	//     spScaleInterval - A COleDateTimeSpan object that contains the
	//                       new time scale item interval.
	//-----------------------------------------------------------------------
	void SetScaleInterval(const COleDateTimeSpan spScaleInterval);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the text label for the
	//     default time scale.
	// Parameters:
	//     strText - A CString that contains the new text label for the
	//               default time label.
	//-----------------------------------------------------------------------
	void SetScaleText(LPCTSTR strText);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the text label of the
	//     default time scale.
	// Returns:
	//     A CString object that contains the default time scale text label.
	//-----------------------------------------------------------------------
	CString GetScaleText();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the text label for the
	//     alternative time scale.
	// Parameters:
	//     strText - A CString that contains the new text label.
	//-----------------------------------------------------------------------
	void SetScale2Text(LPCTSTR strText);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the text label of the
	//     alternative time scale.
	// Returns:
	//     A CString object that contains the alternative time scale text label.
	//-----------------------------------------------------------------------
	CString GetScale2Text();


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the time zone information for
	//     the alternative time scale.
	// Parameters:
	//     pTzInfo - A pointer to time zone information structure.
	// See Also:
	//     MSDN Articles:
	//          INFO: Retrieving Time-Zone Information
	//          KB115231, Q115231
	//
	//          HOWTO: Change Time Zone Information Using Visual Basic
	//          KB221542, Q221542
	//
	//-----------------------------------------------------------------------
	void SetScale2TimeZone(const TIME_ZONE_INFORMATION* pTzInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get the time zone information for
	//     the alternative time scale.
	// Returns:
	//     A reference to time zone information structure.
	//-----------------------------------------------------------------------
	const TIME_ZONE_INFORMATION& GetScale2TimeZone();

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
	//      Get full information about additional time scale time zone.
	// Remarks:
	//      Retrieve additional information from the registry.
	// Returns:
	//      A smart pointer to CXTPCalendarTimeZone object.
	// See Also:
	//      GetTimeZoneInformation(), CXTPCalendarTimeZone::GetTimeZoneInfo()
	//-----------------------------------------------------------------------
	CXTPCalendarTimeZonePtr GetScale2TimeZoneInfo();  // scale 2

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to Show or hide the alternative
	//     time scale.
	// Parameters:
	//     bShow - A BOOL that contains the new visibility flag value.
	//             TRUE - Default value, shows the alternative time scale.
	//             FALSE - Do not show the default value.
	//-----------------------------------------------------------------------
	void ShowScale2(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the visibility flag of
	//     the alternative time scale.
	// Returns:
	//     A BOOL that contains the visibility flag value.
	//     TRUE - If the alternative time scale is visible.
	//     FALSE - If the alternative time scale is not visible.
	//-----------------------------------------------------------------------
	BOOL IsScale2Visible();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the Expand Up glyph
	//     flag value.
	// Returns:
	//     A BOOL that contains the Expand Up glyph flag value.
	//         TRUE  - If the Expand Up glyph is visible.
	//         FALSE - If the Expand Up glyph is not visible.
	//-----------------------------------------------------------------------
	BOOL IsExpandUp();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the Expand Down glyph
	//     flag value.
	// Returns:
	//     A BOOL that contains the Expand Down flag value.
	//     TRUE  - If the Expand Down glyph is visible.
	//     FALSE - If the Expand Down glyph is not visible.
	//-----------------------------------------------------------------------
	BOOL IsExpandDown();

	/////////////////////////////////////////////////////////////////////////
	// cells related

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the time value for
	//     the specified cell.
	// Parameters:
	//     nCell - An int that contains a cell number.
	// Returns:
	//     A COleDateTime object that contains the specific time value
	//     for the cell's beginning coordinate.
	//-----------------------------------------------------------------------
	COleDateTime GetCellTime(int nCell) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the current cell duration.
	// Returns:
	//     A COleDateTimeSpan object that contains the amount of time between
	//     a cell's start coordinates and the next cell's starting coordinates.
	//-----------------------------------------------------------------------
	COleDateTimeSpan GetCellDuration() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the cell number for
	//     the specified time value.
	// Parameters:
	//     dtTime - A COleDateTime object that contains a time
	//              value to check for.
	//     bForEndTime - A BOOL that contains the value that indicates if
	//                   we should consider a split line between cells to
	//                   belong to the end of the previous
	//                   cell or to the beginning of the next cell.
	// Returns:
	//     Call this member function when you want to retrieve which cell
	//     contains a specified time value.
	//-----------------------------------------------------------------------
	int GetCellNumber(COleDateTime dtTime, BOOL bForEndTime) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the cell number for
	//     the specified time value.
	// Parameters:
	//     nHour - An int that contains the hour part of the time value to check for.
	//     nMin - An int that contains the minute part of the time value to check for.
	//     nSec - An int that contains the second part of the time value to check for.
	//     bForEndTime - A BOOL that contains the value that indicates if
	//                   we should consider a split line between cells to
	//                   belong to the end of the previous
	//                   cell or to the beginning of the next cell.
	// Returns:
	//     Call this member function when you want to retrieve which cell
	//     contains a specified time value.
	//-----------------------------------------------------------------------
	int GetCellNumber(int nHour, int nMin, int nSec, BOOL bForEndTime) const;

	/////////////////////////////////////////////////////////////////////////
	// selection related

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the view selection.
	// Parameters:
	//     pBegin                 - A pointer to the COleDateTime object
	//                              that contains the beginning of the selection.
	//     pEnd                   - A pointer to the COleDateTime object
	//                              that contains the ending of the selection.
	//     pbAllDayEvent          - A pointer to the BOOL variable
	//                              that contains the AllDayEvent attribute of
	//                              the selection (used in the day view only).
	//     pspSelectionResolution - A pointer to the COleDateTimeSpan
	//                              object that contains the selection unit
	//                              length. (i.e. 1 day, 1 Hour, ... 5 min)
	// Remarks:
	//     Call this member function to fill provided COleDateTime
	//     objects with the values of the selection beginning, ending,
	//     and selection unit length.
	// Returns:
	//     TRUE - If the selection is returned successfully.
	//     FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL GetSelection(COleDateTime* pBegin = NULL, COleDateTime* pEnd = NULL,
							  BOOL* pbAllDayEvent = NULL, int* pnGroupIndex = NULL,
							  COleDateTimeSpan* pspSelectionResolution = NULL);

	/////////////////////////////////////////////////////////////////////////
	// day manipulations

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to show the specified day in the
	//     calendar day view.
	// Parameters:
	//     date - A COleDateTime object that contains the date to show.
	//     bSelect - TRUE to select the specified day after showing,
	//               FALSE otherwise. TRUE is the default value.
	// Remarks:
	//     Call this member function to show only the specified day in the
	//     calendar day view.
	//-----------------------------------------------------------------------
	virtual void ShowDay(const COleDateTime& date, BOOL bSelect = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to show the specified day interval
	//     in the calendar day view.
	// Parameters:
	//     dtBegin - A COleDateTime object that contains the beginning
	//               interval date to show.
	//     dtEnd - A COleDateTime object that contains the ending interval
	//             date to show.
	// Remarks:
	//     Call this member function to show all days from the dtBegin
	//     up to dtEnd inclusive.
	//-----------------------------------------------------------------------
	virtual void ShowDays(const COleDateTime& dtBegin, const COleDateTime& dtEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to show that working week
	//     in the calendar day view, which includes the specified day.
	// Parameters:
	//     dtDay - A COleDateTime object that contains the date to show.
	// Remarks:
	//     Call this member function to show working week
	//     which includes dtDay.
	//-----------------------------------------------------------------------
	virtual void ShowWorkingDays(const COleDateTime& dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to add a specified date to the
	//     dates set showing in the current calendar day view.
	// Parameters:
	//     date - A COleDateTime object that contains the date to display.
	// Remarks:
	//     Call this member function to display a specific day in the
	//     calendar day view in addition to the days already visible.
	//-----------------------------------------------------------------------
	virtual void AddDay(const COleDateTime& date);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to ensure that an event view item
	//     is visible.
	// Parameters:
	//     pViewEvent - A CXTPCalendarViewEvent object.  A pointer to the
	//                  event view item being made visible.
	// Remarks:
	//     If necessary, the function scrolls the day view so that the
	//     event view item is visible.
	// Returns:
	//     Returns TRUE if the system scrolled the day view in the
	//     Calendar control to ensure that the specified event view item
	//     is visible. Otherwise, the return value is FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL EnsureVisible(CXTPCalendarViewEvent* pViewEvent);
	virtual BOOL EnsureVisibleH(CXTPCalendarViewEvent* pViewEvent); //<COMBINE EnsureVisible>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to start view from the work day starting time.
	// Remarks:
	//     Call this method after set new data provider because new data
	//     provider store other time scale resolution value.
	//-----------------------------------------------------------------------
	virtual void ScrollToWorkDayBegin();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine whether resource group
	//     header should be visible.
	// Remarks:
	//     This method returns FALSE when resource groups count is zero and
	//     there is only one schedule used.
	// Returns:
	//     TRUE when visible; FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsGroupHeaderVisible();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get main or additional time scale
	//     object.
	// Parameters:
	//     nNumber - A time scale object number:
	//                  1 - main time scale,
	//                  2 - additional time scale.
	// Remarks:
	//   By default number 1 (main time scale) is used.
	// Returns:
	//     Pointer to the CXTPCalendarDayViewTimeScale object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarDayViewTimeScale* GetTimeScale(int nNumber = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the CXTPCalendarDayViewDay object
	//     by the day's date.
	// Parameters:
	//     dtDay  - A day date to find.
	// Remarks:
	//     Returns NULL if no day with the specified date in the days collection.
	// Returns:
	//     A pointer to a CXTPCalendarViewDay object.
	// See Also: GetViewDayCount(), GetViewDay_, GetViewDay
	//-----------------------------------------------------------------------
	virtual CXTPCalendarDayViewDay* GetDay(COleDateTime dtDay);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the view back a
	//     specified number of days.
	// Parameters:
	//     nScrollDaysCount - An int that is used to specify the number
	//                        of days to scroll.
	// Remarks:
	//     Call this member function when you want to switch the
	//     number of days back from the current day.
	// See Also: ScrollDaysToNext
	//-----------------------------------------------------------------------
	virtual void ScrollDaysToPrev(int nScrollDaysCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the view forward a
	//     specified number of days.
	// Parameters:
	//     nScrollDaysCount - An int that is used to specify the number
	//                        of days to scroll.
	// Remarks:
	//     Call this member function to switch the number of days forward
	//     from the current date.
	// See Also: ScrollDaysToPrev
	//-----------------------------------------------------------------------
	virtual void ScrollDaysToNext(int nScrollDaysCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to stop vertical scrolling.
	// Remarks:
	//     If there is a vertical scroll event in process, then calling
	//     this member function stops the vertical scrolling.
	//-----------------------------------------------------------------------
	virtual void StopVertEventScroll();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is used to change the mouse cursor.
	// Parameters:
	//     bOutOfArea : TRUE means out (FALSE - inside) of drag area.
	// Remarks:
	//     Call this member function to changes the mouse cursor depending on
	//     which drag mode is currently active. Note: The drag mode is
	//     contained in the XTPCalendarDraggingMode m_eDraggingMode member
	//     variable.
	// ----------------------------------------------------------------------
	virtual void SetMouseOutOfDragArea(BOOL bOutOfArea);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the point is in
	//     the drag able rectangle.
	// Parameters:
	//     pnt      - A CPoint object that contains the point to test.
	//     pHitInfo - Pointer to the XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure,
	//                which helps to determine the client rectangle.
	// Returns:
	//     TRUE if the point is out of the drag able rectangle, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsOutOfClientRect(CPoint pnt, XTP_CALENDAR_HITTESTINFO_DAY_VIEW *pHitInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine the scroll direction.
	// Parameters:
	//     pnt - Test mouse point.
	// Remarks:
	//     This function returns an XTPCalendarDayViewScrollDirection enumeration which defines the
	//     scroll direction.
	// Returns:
	//     xtpCalendarDayViewScrollUp/xtpCalendarDayViewScrollDown if scrolling up/down is needed.
	//     xtpCalendarDayViewScrollNotNeeded if scrolling is not needed.
	//-----------------------------------------------------------------------
	virtual XTPCalendarDayViewScrollDirection GetNeededScrollDirection(CPoint pnt);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to vertically scroll an event
	//     in the current view.
	// Parameters:
	//     bUp - A BOOL that contains the direction to scroll:
	//           TRUE if up.
	//           FALSE if down.
	// Returns:
	//     TRUE, if scrolled successfully. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL VertEventScroll(BOOL bUp);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to return the minimum event
	//     duration so that the event is visible in the current view.
	// Returns:
	//     A COleDateTimeSpan object that specifies the minimum duration
	//     of the event.
	//-----------------------------------------------------------------------
	virtual COleDateTimeSpan GetEventDurationMin() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to select or un-select the provided
	//     day, including all of the day's events.
	// Parameters:
	//     pDay    - A pointer to a CXTPCalendarViewDay object.
	// See Also: CXTPCalendarViewDay, SelectDay(COleDateTime dtSelDay, BOOL bSelect)
	//-----------------------------------------------------------------------
	virtual void SelectDay(CXTPCalendarViewDay* pDay);

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts Time Zones from both timescales to the minutes shift
	//      of the second time scale in comparison with the first one.
	// See Also: CXTPCalendarViewDay, SelectDay(COleDateTime dtSelDay, BOOL bSelect)
	//-----------------------------------------------------------------------
	virtual void AdjustScale2TimeZone();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the content of a view.
	// Parameters:
	//     nIndex :    Position value.
	//     nPos_raw :  Raw position value. (Not used at this time.)
	//-----------------------------------------------------------------------
	virtual void _ScrollV(int nIndex, int nPos_raw);

private:
	virtual void OnStartDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* pHitTest);
	virtual BOOL OnDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* pHitTest);
	virtual BOOL OnEndDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* pHitInfo);

	virtual void ClearDays();

	virtual void _ScrollDays(int nScrollDaysCount, BOOL bPrev);
	virtual void ProcessCellSelection(COleDateTime dtNewSelBegin, BOOL bFixSelEnd,
		BOOL bAllDayEventSel, int nGroupIndex);

	virtual void ProcessDaySelection(XTP_CALENDAR_HITTESTINFO* pInfo, UINT nFlags);

	virtual CXTPCalendarViewEvent* FindEventToEditByTAB(COleDateTime dtMinStart,
														BOOL bReverse,
														CXTPCalendarEvent* pAfterEvent = NULL);
	virtual COleDateTime GetNextTimeEditByTAB();
	virtual void UpdateNextTimeEditByTAB(COleDateTime dtNext, BOOL bReverse,
										 BOOL bReset = FALSE);

	virtual BOOL OnTimer(UINT_PTR uTimerID);
	virtual void OnActivateView(BOOL bActivate, CXTPCalendarView* pActivateView,
								CXTPCalendarView* pInactiveView);

	void _AddDay(const COleDateTime& date);
	int CalculateHeaderFormatAndHeight(CDC* pDC, int nCellWidth);

	virtual CXTPCalendarData* _GetDataProviderByConnStr(LPCTSTR pcszConnStr, BOOL bCompareNoCase = TRUE);

public:
	//{{AFX_CODEJOCK_PRIVATE
	virtual void AdjustAllDayEvents();

	virtual CRect GetDayHeaderRectangle();
	virtual CRect GetAllDayEventsRectangle();

	virtual int GetAllDayEventsMaxCount();
	virtual int GetTotalGroupsCount();
	virtual int RecalcMinColumnWidth();

	struct XTP_DAY_VIEW_LAYOUT
	{
		int m_nVisibleRowCount; // Visible rows count on the day view.

		CRect m_rcDayHeader;
		CRect m_rcAllDayEvents;

		int  m_nRowCount;        // Total rows count on the day view.
		int  m_nTopRow;          // Top visible row on the day view.

		int  m_nAllDayEventsCountMax; // Reserved amount of all day events area to avoid area 'blinking' when dragging.
		int  m_nAllDayEventHeight;    // The height of one event in all day events area.
	};
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual XTP_DAY_VIEW_LAYOUT& GetLayout();
	virtual BOOL IsUseCellAlignedDraggingInTimeArea();
	//}}AFX_CODEJOCK_PRIVATE
private:

	XTP_DAY_VIEW_LAYOUT m_LayoutX; // Layout data.

	CXTPCalendarDayViewTimeScale* m_pTimeScaleHeader;   // Pointer to the main time scale object.
	CXTPCalendarDayViewTimeScale* m_pTimeScaleHeader2;  // Pointer to the alternative time scale object.

	int  m_nAllDayEventsCountMin_WhenDrag;


	COleDateTimeSpan m_spDraggingStartOffset_Time;

	COleDateTime m_dtSelectionStart;

	DWORD m_dwScrollingEventTimerID;
	BOOL m_bScrollingEventUp;
	BOOL m_bMouseOutOfDragArea;
	COleDateTime m_dtDraggingStartPoint;
	CPoint m_ptLastMousePos;
	CPoint m_ptLBtnDownMousePos;

	DWORD m_dwRedrawNowLineTimerID;
	COleDateTime m_dtLastRedrawTime;

protected:

	int m_nScrollOffsetX;   // Stores horizontal left offset of visible day view part.

	int m_nMinColumnWidth;  //  0 means disabled;
	                        // -1 use some default width for multiresources only;
	                        // -N use for multiresources only (internally converted to +N);
	                        // +N used always (in single and multi resources mode).

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get time scales area width.
	// Returns:
	//     Time scales area width in pixels.
	//-----------------------------------------------------------------------
	int _GetTimeScaleWith();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get horizontal scroll area rect.
	// Returns:
	//     Horizontal scroll area rect.
	//-----------------------------------------------------------------------
	CRect _GetScrollRectClient();

	BOOL _EnsureVisibleH(CXTPCalendarViewEvent* pViewEvent); //<COMBINE EnsureVisible>
	BOOL _EnsureVisibleV(CXTPCalendarViewEvent* pViewEvent); //<COMBINE EnsureVisible>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to ensure that an selection is visible.
	// Remarks:
	//     If necessary, the function scrolls the day view horizontally so that
	//     the selection is visible.
	// Returns:
	//     Returns TRUE if the system scrolled the day view, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL _EnsureVisibleSelectionH();

};

//================================================================

AFX_INLINE CRect CXTPCalendarDayView::GetDayHeaderRectangle() {
	return m_LayoutX.m_rcDayHeader;
}

AFX_INLINE CRect CXTPCalendarDayView::GetAllDayEventsRectangle() {
	return m_LayoutX.m_rcAllDayEvents;
}

AFX_INLINE int CXTPCalendarDayView::GetRowCount() const {
	return m_LayoutX.m_nRowCount;
}

AFX_INLINE int CXTPCalendarDayView::GetVisibleRowCount() const {
	return m_LayoutX.m_nVisibleRowCount;
}

AFX_INLINE int CXTPCalendarDayView::GetTopRow() const {
	return m_LayoutX.m_nTopRow;
}

AFX_INLINE void CXTPCalendarDayView::ProcessDaySelection(XTP_CALENDAR_HITTESTINFO* /*pInfo*/, UINT /*nFlags*/) {
	UnselectAllEvents();
}

AFX_INLINE void CXTPCalendarDayView::SelectDay(CXTPCalendarViewDay* /*pDay*/) {
	// Do nothing for day view.
}

AFX_INLINE CXTPCalendarDayViewTimeScale* CXTPCalendarDayView::GetTimeScale(int nNumber) {
	ASSERT(nNumber == 1 || nNumber == 2);
	return nNumber <= 1 ? m_pTimeScaleHeader : m_pTimeScaleHeader2;
}

AFX_INLINE CXTPCalendarDayView::XTP_DAY_VIEW_LAYOUT& CXTPCalendarDayView::GetLayout() {
	return m_LayoutX;
}

AFX_INLINE int CXTPCalendarDayView::GetMinColumnWidth() const {
	return m_nMinColumnWidth;
}

AFX_INLINE void CXTPCalendarDayView::SetMinColumnWidth(int nWidth) {
	m_nMinColumnWidth = nWidth;
}

#endif // !defined(_XTPCALENDARDAYVIEW_H__)
