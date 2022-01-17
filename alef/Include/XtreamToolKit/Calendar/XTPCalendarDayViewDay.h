// XTPCalendarDayViewDay.h: interface for the CXTPCalendarDayViewDay class.
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
#if !defined(_XTPCALENDARDAYVIEWDAY_H__)
#define _XTPCALENDARDAYVIEWDAY_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarDayViewEvent.h"
#include "XTPCalendarViewDay.h"

class CXTPCalendarDayView;
class CXTPCalendarViewPart;
struct XTP_CALENDAR_HITTESTINFO_DAY_VIEW;

//===========================================================================
// Summary:
//     This class represents a single resource view portion of the Calendar
//     Day View.
// Remarks:
//     It represents a specific view of the CalendarView's associated events
//     data grouped by one day and one resource and provides basic
//     functionality on this data using user input through keyboard and mouse.
//
// See Also: CXTPCalendarDayViewDay
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarDayViewGroup : public CXTPCalendarViewGroupT<
											CXTPCalendarDayViewDay,
											CXTPCalendarDayViewEvent,
											XTP_CALENDAR_HITTESTINFO_DAY_VIEW,
											CXTPCalendarDayViewGroup>
{

	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarDayViewEvent;
	friend class CXTPCalendarTheme;

	DECLARE_DYNAMIC(CXTPCalendarDayViewGroup)
	//}}AFX_CODEJOCK_PRIVATE

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Base class type definition.
	//-----------------------------------------------------------------------
	typedef CXTPCalendarViewGroupT<
									CXTPCalendarDayViewDay,
									CXTPCalendarDayViewEvent,
									XTP_CALENDAR_HITTESTINFO_DAY_VIEW,
									CXTPCalendarDayViewGroup >
		TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//     Construct CXTPCalendarDayViewGroup object.
	// Parameters:
	//     pViewDay - Pointer to CXTPCalendarDayViewDay object.
	// See Also: ~CXTPCalendarDayViewGroup()
	//-----------------------------------------------------------------------
	CXTPCalendarDayViewGroup(CXTPCalendarDayViewDay* pViewDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarDayViewGroup();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout depending
	//     on the provided bounding rectangle. and calls AdjustLayout() for
	//     all sub items.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     rcGroup - A CRect object containing the dimensions of the bounding
	//               rectangle for the DayViewGroup area.
	// Remarks:
	//     AdjustLayout2 is called by calendar control instead of AdjustLayout
	//     when theme is set.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcGroup);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcGroup); //<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view content utilizing
	//     the specified device context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before Draw()
	// See Also: AdjustLayout
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill the
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pt - A CPoint object that contains the point to test.
	//     pHitTest - Pointer to an XTP_CALENDAR_HITTESTINFO_DAY_VIEW struct.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the day view.
	// See Also: XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	//-----------------------------------------------------------------------
	virtual BOOL HitTestEx(CPoint pt, XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the busy status for the
	//     specified time.
	// Parameters:
	//     dtTime - A COleDateTime object that contains the time value for
	//              which to check the busy status.
	// Returns:
	//     XTPCalendarEventBusyStatus value.
	//-----------------------------------------------------------------------
	virtual int GetBusyStatus(COleDateTime dtTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the drawing area of all day
	//     events.
	// Returns:
	//     A CRect object containing the coordinates of the all day events
	//     drawing area.
	//-----------------------------------------------------------------------
	virtual CRect GetAllDayEventsRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Adjusts layout of day events items.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//-----------------------------------------------------------------------
	virtual void AdjustDayEvents(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the height of a single cell.
	// Returns:
	//     An int that contains the height, in pixels, of a single cell.
	//-----------------------------------------------------------------------
	int GetRowHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of rows
	//     visible in the view.
	// Returns:
	//     An int that contains the number of rows visible in the view.
	//-----------------------------------------------------------------------
	int GetVisibleRowCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of the top
	//     visible row in the view.
	// Returns:
	//     An int that contains the number of the top visible row in the view.
	//-----------------------------------------------------------------------
	int GetTopRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the total number of
	//     rows in the view.
	// Returns:
	//     An int that contains the total number of rows in the view.
	//-----------------------------------------------------------------------
	int GetRowCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the beginning time value
	//     for the specified cell.
	// Parameters:
	//     nCell - An int that contains the cell index.
	// Returns:
	//     A COleDateTime object that contains the beginning time value
	//     for the specified cell.
	//-----------------------------------------------------------------------
	COleDateTime GetCellDateTime(int nCell) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the cell number for
	//     the specified time value.
	// Parameters:
	//     dtTime - A COleDateTime object that contains the time value to
	//              search for.
	//     bForEndTime - A BOOL object that determines if a split line
	//                   between cells is needed.  The split line is used to
	//                   indicate if the date to search for belongs to the
	//                   end of the previous cell or to the beginning of the
	//                   next cell.
	// Returns:
	//     Call this member function to retrieve the cell that contains
	//     the specified time value.
	//-----------------------------------------------------------------------
	int GetDateTimeCell(const COleDateTime& dtTime, BOOL bForEndTime) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the rect for the specified
	//     cell.
	// Parameters:
	//     nCell - An int that contains the cell index.
	// Returns:
	//     A CRect object that contains the cell rect.
	//-----------------------------------------------------------------------
	CRect GetCellRect(int nCell) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to check is the specified date time
	//     work or non-work.
	// Parameters:
	//     dtDateTime - A COleDateTime object that contains the date time
	//                  value to check.
	// Returns:
	//     TRUE if the specified date time is work, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsWorkDateTime(const COleDateTime& dtDateTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine is the group header
	//     visible or not.
	// Returns:
	//     TRUE if the group header is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsGroupHeaderVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set ExpandUp sign visible.
	// Remarks:
	//     Can be useful to implement visualization themes.
	//-----------------------------------------------------------------------
	void SetExpandUp();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set ExpandDown sign visible.
	// Remarks:
	//     Can be useful to implement visualization themes.
	//-----------------------------------------------------------------------
	void SetExpandDown();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine is ExpandUp sign visible.
	// Remarks:
	//     Can be useful to implement visualization themes.
	// Returns:
	//     TRUE if ExpandUp sign visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsExpandUp();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine is ExpandDown sign visible.
	// Remarks:
	//     Can be useful to implement visualization themes.
	// Returns:
	//     TRUE if ExpandDown sign visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsExpandDown();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to find top and bottom events in group.
	// Parameters:
	//     rpMin - [out] Reference to store pointer to a top event.
	//     rpMax - [out] Reference to store pointer to a bottom event.
	//-----------------------------------------------------------------------
	void FindMinMaxGroupDayEvents(CXTPCalendarDayViewEvent*& rpMin, CXTPCalendarDayViewEvent*& rpMax);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function used to process user action xtpCalendarUserAction_OnScrollDay.
	// Parameters:
	//      eButton - [in] a button from which this action comes.
	// Remarks:
	//      Implementation send a XTP_NC_CALENDAR_USERACTION notification.
	//
	// Returns:
	//      TRUE if action was handled by user and default processing must  be
	//      skipped, FALSE otherwise.
	// See Also: XTP_NC_CALENDAR_USERACTION
	//-----------------------------------------------------------------------
	BOOL UserAction_OnScrollDay(XTPCalendarScrollDayButton eButton);
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
	//     This member function is used to obtain the view part of the
	//     specified cell.
	// Parameters:
	//     dtDateTime - A COleDateTime object that contains the cell's
	//                  date to return the view part for.
	// Returns: A pointer to a CXTPCalendarViewPart object.
	//          The CXTPCalendarViewPart object contains the Day View
	//          Working Cell part if dtDateTime is the same as the Work Date
	//          Time.
	//          Otherwise the CXTPCalendarViewPart object contains the
	//          Non Work Cell part.
	// See Also: CXTPCalendarViewPart overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewPart* GetCellViewPart(const COleDateTime& dtDateTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the events collection
	//     which belongs to the specified cell.
	// Parameters:
	//     nCell       - An int that contains the cell number for the events
	//                   collection.
	//     pViewEvents - Pointer to an event views collection. This pointer
	//                   is used populate the views collection with data.
	//     rdtMinEventTime - A COleDateTime object used to contain the minimum
	//                       time for events in the returned collection.
	//     rdtMaxEventTime - A COleDateTime object used to contain the maximum
	//                       time for events in the returned collection.
	// See Also: TViewEventsCollection
	//-----------------------------------------------------------------------
	virtual void GetCellEvents(int nCell,
		CXTPCalendarDayViewGroup::TBase::TViewEventsCollection* pViewEvents,
		COleDateTime& rdtMinEventTime, COleDateTime& rdtMaxEventTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill an
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pHitTest - A pointer to an XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the day view.
	//     It uses pHitTest->pt point for gathering information.
	// Returns: A BOOL.
	//          TRUE if pHitTest->pt point is on a valid part of the Day View.
	//          FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	//-----------------------------------------------------------------------
	virtual BOOL HitTestDateTime(XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest);

	// -----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill an
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pInfo :  A pointer to an XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Remarks:
	//     Call this member function to gather hit test information from the
	//     day view.
	// See Also:
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	// -----------------------------------------------------------------------
	virtual void FillHitTestEx(XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest);

public:
	//{{AFX_CODEJOCK_PRIVATE
	struct XTP_DAY_VIEW_GROUP_LAYOUT
	{
		CRect m_rcAllDayEvents; // The coordinates of the all day area.
		CRect m_rcDayDetails;   // The coordinates of days details area.

		BOOL  m_bShowHeader;    // TRUE to show resource group header; FALSE otherwise.

		// for office 2007 theme
		int   m_nHotState;  // Last Items Hot state.

		enum XTPEnumHotItem
		{
			xtpHotHeader        = 0x001,
			xtpHotScrollUp      = xtpCalendarHitTestDayViewScrollUp,
			xtpHotScrollDown    = xtpCalendarHitTestDayViewScrollDown,

			//xtpHotADScrollUp      = 0x010,
			//xtpHotADScrollDown    = 0x020,

		};
	};
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual XTP_DAY_VIEW_GROUP_LAYOUT& GetLayout();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	XTP_DAY_VIEW_GROUP_LAYOUT m_LayoutX; // Store group view layout.

	BOOL m_bExpandUP;           // Store is ExpandUP sign visible.
	BOOL m_bExpandDOWN;         // Store is ExpandDOWN sign visible.

private:
	TViewEventsCollection m_arRegionViewEvents;

	void _AddInRegion_IfNeed(CXTPCalendarDayViewEvent* pViewEvent);
	void _OnRegionEnded(CDC* pDC, int nRegionMaxSize);

	virtual CXTPCalendarDayViewGroup* GetPThis();
};

AFX_INLINE CXTPCalendarDayViewGroup* CXTPCalendarDayViewGroup::GetPThis() {
	return this;
}


//===========================================================================
// Summary:
//     This class represents a day view portion of the Calendar Day View.
// Remarks:
//     It represents a specific view of the CalendarView's associated events
//     data grouped by one day and provides basic functionality on this
//     data using user input through keyboard and mouse.
//
//          CXTPCalendarDayViewDay is based on CXTPCalendarViewDayT template
//          class. It inherits basic behavior from its and overlaps and add some
//          to get its own behavior and look.
//
// See Also: CXTPCalendarViewDay, CXTPCalendarViewDayT
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarDayViewDay : public CXTPCalendarViewDayT<
											CXTPCalendarDayView,
											CXTPCalendarDayViewGroup,
											XTP_CALENDAR_HITTESTINFO_DAY_VIEW,
											CXTPCalendarDayViewDay >
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarDayView;
	friend class CXTPCalendarDayViewGroup;
	friend class CXTPCalendarDayViewEvent;

	DECLARE_DYNAMIC(CXTPCalendarDayViewDay)
	//}}AFX_CODEJOCK_PRIVATE

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Base class type definition.
	//-----------------------------------------------------------------------
	typedef CXTPCalendarViewDayT<CXTPCalendarDayView,
								 CXTPCalendarDayViewGroup,
								 XTP_CALENDAR_HITTESTINFO_DAY_VIEW,
								 CXTPCalendarDayViewDay >    TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pDayView - Pointer to parent CXTPCalendarDayView object.
	//     dtDate - A date for which the day view is created.
	// Remarks:
	//     Construct CXTPCalendarDayViewDay object.
	// See Also: ~CXTPCalendarDayViewDay()
	//-----------------------------------------------------------------------
	CXTPCalendarDayViewDay(CXTPCalendarDayView* pDayView, COleDateTime dtDate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarDayViewDay();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view content utilizing
	//     the specified device context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before Draw()
	// See Also: AdjustLayout(CRect rcDay)
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill the
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pt - A CPoint object that contains the point to test.
	//     pHitTest - Pointer to an XTP_CALENDAR_HITTESTINFO_DAY_VIEW struct.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the day view.
	// See Also: XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	//-----------------------------------------------------------------------
//  virtual BOOL HitTestEx(CPoint pt, XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout depending
	//     on the provided bounding rectangle. and calls AdjustLayout() for
	//     all sub items.
	// Parameters:
	//     rcDay   - A CRect object containing the dimensions of the bounding
	//               rectangle for the control.
	// Remarks:
	//     AdjustLayout2 is called by calendar control instead of AdjustLayout
	//     when theme is set.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcDay);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcDay);//<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data
	//     for all items contained in the view.
	//-----------------------------------------------------------------------
	virtual void Populate(COleDateTime dtDayDate);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a single day's view caption.
	// Returns:
	//     A CString object that contains the day's view caption.
	//-----------------------------------------------------------------------
	virtual CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill an
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Parameters:
	//     pInfo :  A pointer to an XTP_CALENDAR_HITTESTINFO_DAY_VIEW structure.
	// Remarks:
	//     Call this member function to gather hit test information from the
	//     day view.
	// See Also:
	//     XTP_CALENDAR_HITTESTINFO_DAY_VIEW
	//-----------------------------------------------------------------------
	virtual void FillHitTestEx(XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pInfo);

protected:
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
private:
	virtual CXTPCalendarDayViewDay* GetPThis();
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE CRect CXTPCalendarDayViewGroup::GetAllDayEventsRect() const {
	return m_LayoutX.m_rcAllDayEvents;
}

AFX_INLINE BOOL CXTPCalendarDayViewGroup::IsGroupHeaderVisible() const {
	return m_LayoutX.m_bShowHeader;
}


AFX_INLINE CXTPCalendarDayViewGroup::XTP_DAY_VIEW_GROUP_LAYOUT&
										CXTPCalendarDayViewGroup::GetLayout() {
	return m_LayoutX;
}


AFX_INLINE CXTPCalendarDayViewDay* CXTPCalendarDayViewDay::GetPThis() {
	return this;
}

AFX_INLINE void CXTPCalendarDayViewGroup::SetExpandUp() {
	m_bExpandUP = TRUE;
}

AFX_INLINE void CXTPCalendarDayViewGroup::SetExpandDown() {
	m_bExpandDOWN = TRUE;
}

AFX_INLINE BOOL CXTPCalendarDayViewGroup::IsExpandUp() {
	return m_bExpandUP;
}

AFX_INLINE BOOL CXTPCalendarDayViewGroup::IsExpandDown() {
	return m_bExpandDOWN;
}

#endif // !defined(AFX_XTPCALENDARDAYVIEWDAY_H__7E299FBF_C671_4648_8919_8ACAFDF38A99__INCLUDED_)
