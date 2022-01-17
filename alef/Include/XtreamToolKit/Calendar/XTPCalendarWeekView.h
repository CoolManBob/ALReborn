// XTPCalendarWeekView.h: interface for the CXTPCalendarWeekView class.
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
#if !defined(_XTPCALENDARWEEKVIEW_H__)
#define _XTPCALENDARWEEKVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarView.h"
#include "XTPCalendarWeekViewDay.h"

class CXTPCalendarWeekView;
class CXTPCalendarContorl;



//===========================================================================
// Summary:
//     This class implements a specific view portion of the Calendar
//     control - called <b>Week view</b>.
// Remarks:
//     The Week view can be shown as one week, consisting of
//     7 days, as a normal week contains.
//     Weekend days of the week are drawn compressed, which means
//     that 2 weekend days will be drawn in a rectangle of a usual day.
//     Each day is represented by a CXTPCalendarWeekViewDay class.
//
// See Also: CXTPCalendarView, CXTPCalendarDayView, CXTPCalendarMonthView,
//          CXTPCalendarWeekViewDay, CXTPCalendarWeekViewEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarWeekView : public CXTPCalendarViewT<
										CXTPCalendarWeekViewDay,
										XTP_CALENDAR_HITTESTINFO_WEEK_VIEW>
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarWeekView)

	friend class CXTPCalendarWeekViewEvent;
	friend class CXTPCalendarTheme;
	//}}AFX_CODEJOCK_PRIVATE
public:

	//------------------------------------------------------------------------
	// Summary:
	//     Base class type definition.
	//------------------------------------------------------------------------
	typedef CXTPCalendarViewT<  CXTPCalendarWeekViewDay,
								XTP_CALENDAR_HITTESTINFO_WEEK_VIEW> TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pCalendarControl - Pointer to CXTPCalendarControl object.
	// See Also: ~CXTPCalendarEvent()
	//-----------------------------------------------------------------------
	CXTPCalendarWeekView(CXTPCalendarControl* pCalendarControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarWeekView();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data
	//     for all items contained in the view.
	//-----------------------------------------------------------------------
	virtual void Populate();

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
	//     This member function is used to adjust the view's layout depending
	//     on the window's client size and also calls AdjustLayout() for
	//     all sub-items.
	// Parameters:
	//     rcView                - A CRect that contains the rectangle
	//                             coordinates used to draw the view.
	//     bCallPostAdjustLayout - A BOOL. Flag used to call additional adjustments.
	// Remarks:
	//     Call Populate() prior calling adjust AdjustLayout().
	//     AdjustLayout2 is called by calendar control instead of AdjustLayout
	//     when theme is set.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout = TRUE);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout = TRUE); //<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to retrieve information about a
	//     scroll bar state.
	// Parameters:
	//     pSI - A pointer to a scrollbar information structure.
	// Returns:
	//     A BOOL. This function should return TRUE to indicate success.
	//     FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL GetScrollBarInfoV(SCROLLINFO* pSI);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to scroll the contents of a view.
	// Parameters:
	//     nPos     - An int that contains the position value.
	//     nPos_raw - An int that contains the raw position value.
	// Remarks:
	//     ScrollV internally uses Populate() to fill the new view data.
	// See Also: Populate().
	//-----------------------------------------------------------------------
	virtual void ScrollV(int nPos, int nPos_raw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view contents using
	//     the specified device context.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before calling Draw().
	//     Draw2 is called by calendar control instead of Draw when theme
	//     is set.
	// See Also: AdjustLayout().
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);
	virtual void Draw2(CDC* pDC); //<COMBINE Draw>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process keyboard on-key-down events.
	// Parameters:
	//     nChar   - A UINT that contains the virtual key code of the given key.
	//     nRepCnt - A UINT that contains the number of times the keystroke is
	//               repeated as a result of the user holding down the key.
	//     nFlags  - A UINT that contains the scan code, key-transition code,
	//               previous key state, and the context code.
	// Remarks:
	//     This method is called by the CalendarControl when the user
	//     presses keys on the keyboard.
	//-----------------------------------------------------------------------
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the date of a day view by
	//     the day's index.
	// Parameters:
	//     nIndex  - An int that contains the day view index in the view
	//               collection.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	// Returns:
	//     A COleDateTime object that contains the day view date and time.
	// See Also: GetViewDayCount()
	//-----------------------------------------------------------------------
	virtual COleDateTime GetViewDayDate(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the header height of a
	//     day.
	// Returns:
	//     An int that contains the height of a row required to display
	//     a single event.
	//-----------------------------------------------------------------------
	int GetDayHeaderHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the date of a week's Monday.
	// Parameters:
	//     dtNewBeginDate  - A COleDateTime object that contains the date of Monday.
	// Remarks:
	//     Call this member function to set the date of a week to the Monday of
	//     the current week.
	// See Also: GetBeginDate()
	//-----------------------------------------------------------------------
	void SetBeginDate(COleDateTime dtNewBeginDate);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the date of the current
	//     week's Monday.
	// Returns:
	//     A COleDateTime object that contains the current week's Monday date.
	// See Also: SetBeginDate()
	//-----------------------------------------------------------------------
	COleDateTime GetBeginDate() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the width of the weeks
	//     day rectangle.
	// Returns:
	//     An int that contains the width of the day rectangle.
	//-----------------------------------------------------------------------
	int GetDayWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the height of a week's
	//     day rectangle.
	// Returns:
	//     An int that contains the height of a day rectangle.
	//-----------------------------------------------------------------------
	int GetDayHeidht() const;

protected: // member function

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to adjust the first day of the week.
	// Remarks:
	//     The first day of the week depends on regions.
	//-----------------------------------------------------------------------
	virtual void AdjustFirstDayOfWeek();

private:
	COleDateTime ShiftDateToCell_00(COleDateTime dtDate) const;

public:

	struct XTP_WEEK_VIEW_LAYOUT
	{
		int m_nGridColumns;     // Number of columns in a week day grid.
		int m_nDayHeaderHeight; // Height of a day header.

		int m_nDayWidth;        // Width of day rectangle.
		int m_nDayHeidht;       // Height of day rectangle.
	};

protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual XTP_WEEK_VIEW_LAYOUT& GetLayout();
	//}}AFX_CODEJOCK_PRIVATE

protected: // data members

	XTP_WEEK_VIEW_LAYOUT m_LayoutX; // Layout data.
	int m_nCurrPos;         // Current scrollbar position.

	COleDateTime m_dtBeginDate; // First date of week (Monday's date).
	int m_nFirstDayOfWeekIndex; // Index for first weekday.



};

AFX_INLINE CXTPCalendarWeekView::XTP_WEEK_VIEW_LAYOUT& CXTPCalendarWeekView::GetLayout() {
	return m_LayoutX;
}

AFX_INLINE int CXTPCalendarWeekView::GetDayHeaderHeight() const {
	return m_LayoutX.m_nDayHeaderHeight;
}

AFX_INLINE COleDateTime CXTPCalendarWeekView::GetBeginDate() const {
	return m_dtBeginDate;
}

AFX_INLINE int CXTPCalendarWeekView::GetDayWidth() const {
	return m_LayoutX.m_nDayWidth;
}

AFX_INLINE int CXTPCalendarWeekView::GetDayHeidht() const {
	return m_LayoutX.m_nDayHeidht;
}

#endif // !defined(_XTPCALENDARWEEKVIEW_H__)
