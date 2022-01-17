// XTPCalendarWeekViewDay.h: interface for the CXTPCalendarWeekViewDay class.
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
#if !defined(__XTPCALENDARWEEKVIEWDAY_H_)
#define __XTPCALENDARWEEKVIEWDAY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

#include "XTPCalendarWeekViewEvent.h"
#include "XTPCalendarViewDay.h"

class CXTPCalendarWeekView;


//===========================================================================
// Summary:
//     This class represents a single resource view portion of the Calendar
//     Week View.
// Remarks:
//     It represents a specific view of the CalendarView's associated events
//     data grouped by one day and one resource group. And provides basic
//     functionality on this data using user input through keyboard and mouse.
//
// See Also: CXTPCalendarWeekViewDay
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarWeekViewGroup : public CXTPCalendarViewGroupT<
											CXTPCalendarWeekViewDay,
											CXTPCalendarWeekViewEvent,
											XTP_CALENDAR_HITTESTINFO_WEEK_VIEW,
											CXTPCalendarWeekViewGroup >
{
public:
	//------------------------------------------------------------------------
	// Summary:
	//     Base class type definition.
	//------------------------------------------------------------------------
	typedef CXTPCalendarViewGroupT<     CXTPCalendarWeekViewDay,
										CXTPCalendarWeekViewEvent,
										XTP_CALENDAR_HITTESTINFO_WEEK_VIEW,
										CXTPCalendarWeekViewGroup >  TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//     Construct CXTPCalendarWeekViewGroup object.
	// Parameters:
	//     pViewDay - Pointer to CXTPCalendarWeekViewDay object.
	//-----------------------------------------------------------------------
	CXTPCalendarWeekViewGroup(CXTPCalendarWeekViewDay* pViewDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles class members deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarWeekViewGroup();

protected:
	// Summary:
	//     This member function is used to fill a XTP_CALENDAR_HITTESTINFO_WEEK_VIEW structure.
	// Parameters:
	//     pInfo - A pointer to a XTP_CALENDAR_HITTESTINFO_WEEK_VIEW struct.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the week view group.
	// See Also: XTP_CALENDAR_HITTESTINFO_WEEK_VIEW
	//-----------------------------------------------------------------------
	virtual void FillHitTestEx(XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pHitTest);

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to itself.
	// Remarks:
	//     Call this member function to get the "this" pointer.
	// Returns:
	//     Pointer to a CXTPCalendarWeekViewGroup.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarWeekViewGroup* GetPThis();

};

AFX_INLINE CXTPCalendarWeekViewGroup* CXTPCalendarWeekViewGroup::GetPThis() {
	return this;
}


//===========================================================================
// Summary:
//     This class represents a day view portion of the CalendarWeekView.
// Remarks:
//     It represents a specific view of the CalendarView's associated events
//     data grouped by one day and provides basic functionality on this
//     data using user input through the keyboard and mouse.
//
//          CXTPCalendarWeekViewDay is based on the CXTPCalendarViewDayT template
//          class. CXTPCalendarWeekViewDay inherits basic behavior from the
//          CXTPCalendarViewDayT class. Furthermore, class CXTPCalendarWeekViewDayand
//          overlaps and adds some functionality for its own behavior and look.
//
// See Also: CXTPCalendarViewDay, CXTPCalendarViewDayT
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarWeekViewDay : public CXTPCalendarViewDayT<
										CXTPCalendarWeekView,
										CXTPCalendarWeekViewGroup,
										XTP_CALENDAR_HITTESTINFO_WEEK_VIEW,
										CXTPCalendarWeekViewDay >
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarWeekViewEvent;
	DECLARE_DYNAMIC(CXTPCalendarWeekViewDay)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//------------------------------------------------------------------------
	// Summary:
	//     Base class type definition.
	//------------------------------------------------------------------------
	typedef CXTPCalendarViewDayT<
									CXTPCalendarWeekView,
									CXTPCalendarWeekViewGroup,
									XTP_CALENDAR_HITTESTINFO_WEEK_VIEW,
									CXTPCalendarWeekViewDay>    TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor. Construct CXTPCalendarWeekViewDay
	//     object.
	// Parameters:
	//     pWeekView - Pointer to CXTPCalendarWeekView object.
	// See Also: ~CXTPCalendarWeekViewDay()
	//-----------------------------------------------------------------------
	CXTPCalendarWeekViewDay(CXTPCalendarWeekView* pWeekView);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarWeekViewDay();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill a XTP_CALENDAR_HITTESTINFO_WEEK_VIEW structure.
	// Parameters:
	//     pInfo - A pointer to a XTP_CALENDAR_HITTESTINFO_WEEK_VIEW struct.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the week view.
	// See Also: XTP_CALENDAR_HITTESTINFO_WEEK_VIEW
	//-----------------------------------------------------------------------
	virtual void FillHitTestEx(XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine which view item,
	//     if any, is at the specified position index, and returns additional
	//     info in the _THitTest template object.
	// Parameters:
	//     pt       - A CPoint that contains the coordinates of the point test.
	//     pHitTest - A pointer to a _THitTest structure.
	// Remarks:
	//     Implements standard functionality for the HitTestEx method.
	// Returns:
	//     A BOOL. TRUE if the item is found. FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO
	//-----------------------------------------------------------------------
	virtual BOOL HitTestEx(CPoint pt, XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pHitTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout depending
	//     on the provided rectangle and also calls AdjustLayout() for all
	//     sub-items.
	// Parameters:
	//     rcDay - A CRect object that contains the rectangle coordinates
	//             used to draw the day view.
	// Remarks:
	//     Call Populate(COleDateTime dtDayDate) prior to calling AdjustLayout().
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcDay);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcDay); //<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view content using the
	//     specified device context.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before calling Draw().
	// See Also: AdjustLayout(CRect rcDay)
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function is used to obtain a portion of a day rectangle
	//     used to draw event views.
	// Returns:
	//     A CRect object that contains the rectangle coordinates.
	//-----------------------------------------------------------------------
	CRect GetDayEventsRect() const;

protected:

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
	//     This member function is used to obtain a pointer to itself.
	// Returns:
	//     A pointer to a CXTPCalendarWeekViewDay object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarWeekViewDay* GetPThis();
};
////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPCalendarWeekViewDay* CXTPCalendarWeekViewDay::GetPThis() {
	return this;
}

#endif // !defined(__XTPCALENDARWEEKVIEWDAY_H_)
