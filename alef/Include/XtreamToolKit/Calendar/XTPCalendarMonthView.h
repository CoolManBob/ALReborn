// XTPCalendarMonthView.h: interface for the CXTPCalendarMonthView class.
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
#if !defined(_XTPCALENDARMONTHVIEW_H__)
#define _XTPCALENDARMONTHVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

#include "XTPCalendarView.h"
#include "XTPCalendarMonthViewDay.h"

class CXTPCalendarMonthView;

//===========================================================================
// Summary:
//     Define minimum count of weeks which can be shown in the
//     Month View.
// See Also: CXTPCalendarMonthView::CMonthViewGrid::GetWeeksCount,
//           CXTPCalendarMonthView::CMonthViewGrid::SetWeeksCount,
//           XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX
//===========================================================================
#define XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN   2

//===========================================================================
// Summary:
//     Define maximum count of weeks which can be shown in the
//     Month View.
// See Also: CXTPCalendarMonthView::CMonthViewGrid::GetWeeksCount,
//           CXTPCalendarMonthView::CMonthViewGrid::SetWeeksCount,
//           XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN
//===========================================================================
#define XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX   6



//===========================================================================
// Summary:
//     This class implements a specific view portion of the Calendar
//     control - called <b>Month view</b>.
// Remarks:
//     The Month can contain one or more weeks, placed in
//     a column. Each week consist of 7 days, as a normal week.
//     Weekend days of the week can be drawn compressed, which means
//     that 2 weekend days will be drawn in a rectangle of a usual day.
//     Each day is represented by a CXTPCalendarMonthViewDay class.
//
// See Also: CXTPCalendarView, CXTPCalendarDayView, CXTPCalendarWeekView,
//          CXTPCalendarMonthViewDay, CXTPCalendarMonthViewEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarMonthView : public CXTPCalendarViewT<
										CXTPCalendarMonthViewDay,
										XTP_CALENDAR_HITTESTINFO_MONTH_VIEW>
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarMonthViewDay;
	friend class CXTPCalendarMonthViewEvent;

	DECLARE_DYNAMIC(CXTPCalendarMonthView)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//------------------------------------------------------------------------
	// Remarks:
	//     Base class type definition
	//------------------------------------------------------------------------
	typedef CXTPCalendarViewT< CXTPCalendarMonthViewDay,
								XTP_CALENDAR_HITTESTINFO_MONTH_VIEW> TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pCalendarControl - Pointer to a CXTPCalendarControl object.
	// See Also: ~CXTPCalendarMonthView()
	//-----------------------------------------------------------------------
	CXTPCalendarMonthView(CXTPCalendarControl* pCalendarControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member item deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarMonthView();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data
	//     for all items contained in the view.
	//-----------------------------------------------------------------------
	virtual void Populate();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout
	//     depending on the window's client size and then calls AdjustLayout()
	//     for all sub-items.
	// Parameters:
	//     rcView                - A CRect object that contains the
	//                             coordinates used to draw the view.
	//     bCallPostAdjustLayout - A BOOL.  Flag used to call for additional
	//                             adjustments.
	// Remarks:
	//     Call Populate() prior to AdjustLayout.
	//     AdjustLayout2 is called by calendar control instead of AdjustLayout
	//     when theme is set.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout = TRUE);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout = TRUE);//<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to show the specified day in the
	//     calendar view.
	// Parameters:
	//     date - A COleDateTime object that contains the date to show.
	//     bSelect - TRUE to select the specified day after showing,
	//               FALSE otherwise. TRUE is the default value.
	// Remarks:
	//     Call this member function to show only the specified day in the
	//     calendar view.
	//-----------------------------------------------------------------------
	virtual void ShowDay(const COleDateTime& date, BOOL bSelect = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to retrieve information about the
	//     current scroll bar state.
	// Parameters:
	//     pSI - A SCROLLINFO pointer. Pointer to a scrollbar information
	//           structure.
	//-----------------------------------------------------------------------
	virtual BOOL GetScrollBarInfoV(SCROLLINFO* pSI);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the contents of a view.
	// Parameters:
	//     nPos     - An int that contains the position value.
	//     nPos_raw - An int that contains the raw position value.
	// Remarks:
	//     ScrollV internally uses Populate() to fill new view data.
	//     Note: nPos is not used at this time.  It is reserved for future use.
	// See Also: Populate(), GetScrollBarInfoV.
	//-----------------------------------------------------------------------
	virtual void ScrollV(int nPos, int nPos_raw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view content using
	//     the specified device context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before Draw().
	//     Draw2 is called by calendar control instead of Draw when theme
	//     is set.
	// See Also: AdjustLayout().
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);
	virtual void Draw2(CDC* pDC); //<COMBINE Draw>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the event caption format flags.
	// Remarks:
	//     Call this member function to obtain event caption format flags.
	//     Event caption format flags are stored in a packed form. Use bitwise
	//     operations to determine each flag state.
	// Returns:
	//     Integer value containing flags.
	//-----------------------------------------------------------------------
	virtual int GetEventCaptionFormat() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the width necessary to display
	//     the event's time.
	// Returns:
	//     An int that contains the width as an integer value.
	//-----------------------------------------------------------------------
	virtual int GetEventTimeWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to processes mouse movement events.
	// Parameters:
	//     nFlags  - A UINT that is used to indicate whether various virtual
	//               keys are down.
	//     point   - A CPoint that specifies the x- and y- coordinates of the cursor.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     moves the mouse cursor or stylus.
	//-----------------------------------------------------------------------
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to processes left mouse button events.
	// Parameters:
	//     nFlags  - A UINT that is used to indicates whether various virtual
	//               keys are down.
	//     point   - A CPoint that specifies the x- and y- coordinates of the cursor.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     presses the left mouse button.
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process keyboard events.
	// Parameters:
	//     nChar   - A UINT. Virtual key code of the given key.
	//     nRepCnt - A UINT. Number of times the keystroke is repeated as
	//               a result of the user holding down the key.
	//     nFlags  - A UINT. Scan code, key-transition code, previous key state,
	//               and context code.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     presses keys on the keyboard.
	//-----------------------------------------------------------------------
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the view's day object by
	//     the given index.
	// Parameters:
	//     nIndex  - An int. Day view index in the view collection.
	// Remarks:
	//     Index numbers start with 0 and cannot be negative.
	// Returns:
	//     Pointer to a CXTPCalendarMonthViewDay object.
	// See Also: GetViewDayCount()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarMonthViewDay* GetViewDay(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the date of a day view by the
	//     day's index.
	// Parameters:
	//     nIndex - An int. Day view index in the view collection.
	// Remarks:
	//     Index numbers start with 0 and cannot be negative.
	// Returns:
	//     A COleDateTime object that contains the day view date and time.
	// See Also: GetViewDayCount()
	//-----------------------------------------------------------------------
	virtual COleDateTime GetViewDayDate(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain week day header text.
	// Parameters:
	//     pstrText - [in, out] Pointer to item text;
	//     nWeekDay - Week day number as: 1 - Sunday, 2 - Monday, ... 7 - Satyrday.
	// Remarks:
	//     If AskItemTextFlags has xtpCalendarItemText_MonthViewWeekDayHeader
	//     flag set - XTP_NC_CALENDAR_GETITEMTEXT notification is sent to
	//     customize standard text.
	// See Also:
	//     XTPCalendarGetItemText,XTPCalendarGetItemTextEx,
	//     XTP_NC_CALENDAR_GETITEMTEXT,
	//     CXTPCalendarControl::GetAskItemTextFlags,
	//     CXTPCalendarControlPaintManager::GetAskItemTextFlags,
	//     CXTPCalendarControlPaintManager::SetAskItemTextFlags,
	//     CXTPCalendarTheme::GetAskItemTextFlags,
	//     CXTPCalendarTheme::SetAskItemTextFlags
	//-----------------------------------------------------------------------
	virtual void GetWeekDayTextIfNeed(CString* pstrText, int nWeekDay);
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the format string of a
	//     day date.
	// Parameters:
	//     dtDay - A COleDateTime object that contains the date to be formatted.
	//     bLong - A BOOL. Determines whether to apply a long date format.
	// Remarks:
	//     Call this member function to obtain the date in a form appropriate
	//     to display in the day header text.
	// Returns:
	//     CString object with the formatted date.
	// See Also: GetViewDayCount()
	//-----------------------------------------------------------------------
	CString _FormatDayDate(COleDateTime dtDay, BOOL bLong);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the view's date
	//     format based on the current locale.
	//-----------------------------------------------------------------------
	void _CalculateDateFormats(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to build day header date format
	//      default strings.
	// Remarks:
	//      This member function read locale settings and fill
	//      m_strDayHeaderFormatDefaultXXX members.
	// See Also:
	//      _CalculateDateFormats(), CXTPCalendarView::_ReadDefaultHeaderFormats(),
	//      m_strDayHeaderFormatDefaultLong, m_strDayHeaderFormatDefaultMiddle,
	//      m_strDayHeaderFormatDefaultShort, m_strDayHeaderFormatDefaultShortest.
	//-----------------------------------------------------------------------
	void _ReadDefaultDateFormats();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to parse the format string into tokens.
	// Parameters:
	//     strDateFormat - A CString that contains the string to parse.
	//     rarTokens     - A CStringArray that contains the resultant token array.
	//-----------------------------------------------------------------------
	void _SplitDateFormat(const CString& strDateFormat, CStringArray& rarTokens);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the event view's date
	//     format based on the current locale.
	//-----------------------------------------------------------------------
	void _CalculateEventCaptionFormat(CDC* pDC);

	CString m_strLongDateFormat;    // String of the long date format.
	CString m_strSmallDateFormat;   // String of the short date format.
	int     m_nEventCaptionFormat;  // Event caption format flags are stored in packed form by bitwise operations.
	int     m_nEventTimeWidth;      // width of the area to display the event times.

	XTP_CALENDAR_MONTHVIEW_DAYPOS m_DayPOS_LastSelected; // Last selected day positions.
public:

	//=======================================================================
	// Remarks:
	//     This helper class implements a grid for the month view.
	// See Also: CXTPCalendarMonthView overview
	//=======================================================================
	class _XTP_EXT_CLASS CMonthViewGrid
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Default class constructor.
		// Parameters:
		//     pView - A CXTPCalendarMonthView pointer to the parent view class.
		//-----------------------------------------------------------------------
		CMonthViewGrid(CXTPCalendarMonthView* pView);

		//-----------------------------------------------------------------------
		// Summary:
		//     Default class destructor.
		// Remarks:
		//     Handles member item deallocation.
		//-----------------------------------------------------------------------
		virtual ~CMonthViewGrid();

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the date of the first
		//     cell (cell [0, 0]) in the grid.
		// Returns:
		//     A COleDateTime object that contains the date of the first cell
		//     (cell [0, 0]) in the grid.
		// See Also: SetBeginDate, ShiftDateToCell_00
		//-----------------------------------------------------------------------
		COleDateTime GetBeginDate() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to set the start view date.
		// Parameters:
		//     dtBeginDate - A COleDateTime object that contains the start view date.
		// Remarks:
		//     This date will be adjusted (shifted) to be the first cell
		//     (cell [0, 0]) date in the grid.
		// See Also: GetBeginDate, ShiftDateToCell_00
		//-----------------------------------------------------------------------
		void         SetBeginDate(COleDateTime dtBeginDate);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to shift the specified date to
		//     the first cell date in the grid.
		// Parameters:
		//     dtDate - A COleDateTime object that contains the date to shift.
		// Returns:
		//     A COleDateTime object that contains the date that was shifted
		//     to the first cell in the grid.
		// See Also: GetBeginDate, SetBeginDate
		//-----------------------------------------------------------------------
		COleDateTime ShiftDateToCell_00(COleDateTime dtDate);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the weeks (rows) count
		//     in the grid.
		// Returns:
		//     An int that contains the number of weeks (rows) in the grid.
		// See Also: SetWeeksCount
		//-----------------------------------------------------------------------
		int GetWeeksCount() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to set the number of weeks that are
		//     shown in the grid (Month View).
		// Parameters:
		//     nWeeks - An int that contains the number of weeks shown in the
		//              grid. Minimum and maximum values are defined as
		//              XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN and
		//              XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX.
		// See Also: GetWeeksCount, XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN,
		//           XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX
		//-----------------------------------------------------------------------
		void SetWeeksCount(int nWeeks);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to retrieve the specified day date.
		// Parameters:
		//     nWeekIndex    - An int that contains the index of the week.
		//                     Valid values are from 0 to GetWeeksCount()-1.
		//     nWeekDayIndex - An int that contains the index of the day cell
		//                      in the week. Valid values are from 0 to 6.
		// Returns:
		//     A COleDateTime object that contains the specified day date.
		// See Also: GetWeeksCount, SetWeeksCount
		//-----------------------------------------------------------------------
		COleDateTime GetDayDate(int nWeekIndex, int nWeekDayIndex) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the bounding rectangle
		//     of the day cell.
		// Parameters:
		//     nWeekIndex      - An int that contains the index of the week.
		//                       Valid values are from 0 to GetWeeksCount()-1.
		//     nWeekDayIndex   - An int that contains the index of the day cell
		//                       in the week. Valid values are from 0 to 6.
		//     bIncludeBorders - A BOOL. If TRUE, then borders between days are
		//                       included in the bounding rectangle coordinates.
		//                       If FALSE, then borders are not included in the
		//                       bounding rectangle coordinates.
		// Returns:
		//     A CRect object that contains the bounding rectangle coordinates
		//     of the day cell.
		// See Also: GetCelRect, GetWeeksCount, CRect
		//-----------------------------------------------------------------------
		CRect GetDayRect(int nWeekIndex, int nWeekDayIndex, BOOL bIncludeBorders = FALSE) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the bounding rectangle
		//     of the full grid cell.
		// Parameters:
		//     nWeekIndex      - An int that contains the index of the week.
		//                       Valid values are from 0 to GetWeeksCount()-1.
		//     nColIndex       - An int that contains the index of the cell in the grid.
		//                       Valid values are from 0 to GetColsCount()-1.
		//     bIncludeBorders - A BOOL. If TRUE, then borders between cells
		//                       are included in the bounding rectangle coordinates.
		//                       If FALSE, then the borders between cells are not
		//                       included in the bounding rectangle coordinates.
		// Remarks:
		//     Full grid cell means day cell for no compressed days or
		//     cell included 2 day cells for compressed Sat/Sun if such
		//     option is ON.
		// Returns:
		//     A CRect object that contains the bounding rectangle coordinates
		//     of the full grid cell.
		// See Also: GetDayRect, GetWeeksCount, GetColsCount, CRect
		//-----------------------------------------------------------------------
		CRect GetCelRect(int nWeekIndex, int nColIndex, BOOL bIncludeBorders = FALSE) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the column for the
		//     specified week day.
		// Parameters:
		//     nWeekDayIndex - An in that contains the index of the week day.
		//                     1-Sunday, 2-Monday, ....
		// Remarks:
		//     If the "compress weekend days" option is ON then, only one
		//     column number is returned for Saturday and Sunday.
		// Returns:
		//     An in that contains the column number for the specified week day.
		// See Also: GetColsCount, GetWeekDayForColIndex
		//-----------------------------------------------------------------------
		int GetWeekDayCol(int nWeekDayIndex) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the week day index for the
		//     specified column.
		// Parameters:
		//     nColIdx - An in that contains the index of the week day.
		//                     1-Sunday, 2-Monday, ....
		// Remarks:
		//     If the "compress weekend days" option is ON then,
		//     7 (Saturday) returned for the compressed column.
		// Returns:
		//     Index of the week day as: 1-Sunday, 2-Monday, ....
		// See Also: GetColsCount, GetWeekDayCol
		//-----------------------------------------------------------------------
		int GetWeekDayForColIndex(int nColIdx) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the number of columns
		//     that are displayed in the Month View.
		// Remarks:
		//     If the "compress weekend days" option is ON, then 6 is returned,
		//     otherwise 7.
		// Returns:
		//     An int that contains the number of columns that are displayed
		//     in the Month View.
		// See Also: GetWeekDayCol
		//-----------------------------------------------------------------------
		int GetColsCount() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the number of pixels in
		//     the height of the Month View week days columns header.
		// Returns:
		//     An int that contains the value of the columns header height in pixels.
		//-----------------------------------------------------------------------
		int GetColHeaderHeight() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to set the height of the Month View
		//     week days columns header.
		// Parameters:
		//     nHeight - A new height value in pixels.
		// Remarks:
		//     Should be used in visual themes AdjustLayout processing.
		//-----------------------------------------------------------------------
		void SetColHeaderHeight(int nHeight);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to scroll the contents of a grid.
		// Parameters:
		//     nPos - An int that contains the position value.
		// Remarks:
		//     This member function calculates the date time that corresponds
		//     with the current scroll bar position.  Next, the function adjusts
		//     the dates in the grid so that the date that is associated with the
		//     current scroll bar position is placed in the beginning cell of the
		//     grid.
		// See Also: GetScrollPos, CXTPCalendarMonthView::ScrollV,
		//           CXTPCalendarMonthView::GetScrollBarInfoV.
		//-----------------------------------------------------------------------
		void ScrollV(int nPos);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the current scrolling
		//     position.
		// Returns:
		//     An int that contains the current scrolling position.
		// See Also: ScrollV, CXTPCalendarMonthView::ScrollV,
		//           CXTPCalendarMonthView::GetScrollBarInfoV.
		//-----------------------------------------------------------------------
		int GetScrollPos();

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to adjust the grid's layout
		//     depending on the window's client size and then
		//     calls AdjustLayout() for all sub-items.
		// See Also: CXTPCalendarMonthView::AdjustLayout
		//-----------------------------------------------------------------------
		void AdjustLayout(CDC* pDC);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to draw the grid content using the
		//     specified device context.
		// Parameters:
		//     pDC - Pointer to a valid device context.
		// Remarks:
		//     Call CXTPCalendarMonthView::AdjustLayout before Draw().
		// See Also: CXTPCalendarMonthView::Draw,
		//           CXTPCalendarMonthView::AdjustLayout.
		//-----------------------------------------------------------------------
		void Draw(CDC* pDC);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to adjust the grid's columns
		//     depending on the FirstDayOfWeek calendar control option value.
		// See Also: CXTPCalendarControl::GetFirstDayOfWeek
		//-----------------------------------------------------------------------
		void AdjustFirstDayOfWeek();

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to adjust the grid's columns and rows.
		// See Also: AdjustDays
		//-----------------------------------------------------------------------
		void AdjustGrid(const CRect& rcRect);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to call AdjustLayout for each day in
		//     days collection with corresponding day rect.
		// See Also: AdjustEvents
		//-----------------------------------------------------------------------
		void AdjustDays(CDC* pDC);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to adjust the event views layout.
		// See Also: AdjustLayout, CXTPCalendarMonthView::AdjustLayout
		//-----------------------------------------------------------------------
		void AdjustEvents(CDC* pDC);

		//-----------------------------------------------------------------------
		// Summary:
		//     This function is used to obtain the view day object that is
		//     associated with the given indexes.
		// Parameters:
		//     nWeekIndex    - An int that contains the index of the week.
		//                     Valid values are from 0 to GetWeeksCount()-1.
		//     nWeekDayIndex - An int that contains the index of the day cell
		//                     in the week.
		// Returns:
		//     A pointer to a CXTPCalendarMonthViewDay object that contains the
		//     view day that is associated with the given indexes.
		// See Also: CXTPCalendarMonthView::GetViewDay
		//-----------------------------------------------------------------------
		CXTPCalendarMonthViewDay* GetViewDay(int nWeekIndex, int nWeekDayIndex);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the maximum number of
		//     available day positions.
		// Returns:
		//     A XTP_CALENDAR_MONTHVIEW_DAYPOS object that contains the
		//     maximum number of available day positions.
		// See Also: XTP_CALENDAR_MONTHVIEW_DAYPOS,
		//           XTP_CALENDAR_HITTESTINFO_MONTH_VIEW,
		//           CXTPCalendarMonthView::m_DayPOS_LastSelected
		//-----------------------------------------------------------------------
		XTP_CALENDAR_MONTHVIEW_DAYPOS GetMaxDayPOS();

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain the First Day Of Week index.
		// Returns:
		//     First Day Of Week index. (1-Sunday, 2-Monday ...)
		// See Also: GetWeekDayCol
		//-----------------------------------------------------------------------
		int GetFirstDayOfWeek() const;

	protected:
		COleDateTime m_dtBeginDate;     // First Cell (cell [0, 0]) date in the grid.

		int m_nWeeksCount;              // The number of weeks that are displayed.
		int m_nColHeaderHeight;         // Column header height in pixels.

		CStringArray m_arColHeaderText; // Array of columns (week days) headers names.

		CUIntArray m_arColsLeftX;       // Array of columns left borders positions.
		CUIntArray m_arRowsTopY;        // Array of rows top borders positions.

		int m_nFirstDayOfWeekIndex;     // First Day Of Week index: 1-Sunday, 2-Monday ...

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to adjust the grid's column header layout.
		// See Also: AdjustLayout, CXTPCalendarMonthView::AdjustLayout
		//-----------------------------------------------------------------------
		void AdjustHeader(CDC* pDC);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to calculate the date for a
		//     middle scrollbar position.
		// Returns:
		//     A COleDateTime object that contains the date for a middle
		//     scrollbar position.
		// See Also: ScrollV, GetScrollPos, CXTPCalendarMonthView::ScrollV,
		//           CXTPCalendarMonthView::GetScrollBarInfoV.
		//-----------------------------------------------------------------------
		COleDateTime GetMiddleScrollBeginDate();

	protected:
		CXTPCalendarMonthView* m_pView; // Pointer to the parent view class.
	};
	friend class CMonthViewGrid;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the Month View days
	//     grid object.
	// Returns:
	//     A pointer to a CMonthViewGrid object that contains the Month
	//     View days grid object.
	// See Also: CMonthViewGrid
	//-----------------------------------------------------------------------
	CMonthViewGrid* GetGrid();

protected:
	CMonthViewGrid* m_pGrid; // Pointer to the grid object.

};

//===========================================================================
//XTP_CALENDAR_MONTHVIEW_DAYPOS
//-----------------------------
AFX_INLINE int XTP_CALENDAR_MONTHVIEW_DAYPOS::GetPOS() {
	int nPOS = nWeekIndex * 7 + nWeekDayIndex;
	return nPOS;
}

AFX_INLINE void XTP_CALENDAR_MONTHVIEW_DAYPOS::SetPOS(int nWidx, int nWDidx) {
	nWeekIndex = nWidx;
	nWeekDayIndex = nWDidx;
}

AFX_INLINE void XTP_CALENDAR_MONTHVIEW_DAYPOS::SetPOS(int nPOS) {
	nWeekIndex = nPOS / 7;
	nWeekDayIndex = nPOS % 7;
}

AFX_INLINE BOOL XTP_CALENDAR_MONTHVIEW_DAYPOS::IsValid() {
	BOOL bValid = (nWeekIndex >= 0 || nWeekDayIndex >= 0);
	return bValid;
}

AFX_INLINE XTP_CALENDAR_MONTHVIEW_DAYPOS::operator int() {
	ASSERT(IsValid());
	return GetPOS();
}

AFX_INLINE const XTP_CALENDAR_MONTHVIEW_DAYPOS& XTP_CALENDAR_MONTHVIEW_DAYPOS::operator =(int nPOS) {
	SetPOS(nPOS);
	return *this;
}

AFX_INLINE XTP_CALENDAR_MONTHVIEW_DAYPOS& XTP_CALENDAR_MONTHVIEW_DAYPOS::operator ++() {
	ASSERT(IsValid());
	SetPOS(GetPOS() + 1);
	return *this;
}

AFX_INLINE XTP_CALENDAR_MONTHVIEW_DAYPOS XTP_CALENDAR_MONTHVIEW_DAYPOS::operator ++(int) {
	ASSERT(IsValid());
	XTP_CALENDAR_MONTHVIEW_DAYPOS retPOS = *this;
	SetPOS(GetPOS() + 1);
	return retPOS;
}

//===========================================================================
AFX_INLINE int  CXTPCalendarMonthView::CMonthViewGrid::GetWeeksCount() const {
	return m_nWeeksCount;
}

AFX_INLINE COleDateTime CXTPCalendarMonthView::CMonthViewGrid::GetBeginDate() const {
	return m_dtBeginDate;
}

AFX_INLINE XTP_CALENDAR_MONTHVIEW_DAYPOS CXTPCalendarMonthView::CMonthViewGrid::GetMaxDayPOS() {
	XTP_CALENDAR_MONTHVIEW_DAYPOS maxPOS = (int)(GetWeeksCount() * 7 - 1);
	return maxPOS;
}

AFX_INLINE CXTPCalendarMonthView::CMonthViewGrid* CXTPCalendarMonthView::GetGrid() {
	return m_pGrid;
}

AFX_INLINE int CXTPCalendarMonthView::CMonthViewGrid::GetFirstDayOfWeek() const {
	return m_nFirstDayOfWeekIndex;
}
////////////////////////////////////////////////////////////////////////////
AFX_INLINE int CXTPCalendarMonthView::GetEventCaptionFormat() const {
	return m_nEventCaptionFormat;
}

AFX_INLINE int CXTPCalendarMonthView::GetEventTimeWidth() const {
	return m_nEventTimeWidth;
}

#endif // !defined(_XTPCALENDARMONTHVIEW_H__)
