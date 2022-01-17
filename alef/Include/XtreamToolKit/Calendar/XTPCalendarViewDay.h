// XTPCalendarViewDay.h: interface for the CXTPCalendarViewDay class.
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
#if !defined(_XTPCALENDARVIEWDAY_H__)
#define _XTPCALENDARVIEWDAY_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XTPCalendarPaintManager.h"
#include "XTPCalendarView.h"
#include "XTPCalendarResource.h"
#include "XTPCalendarControl.h"

#pragma warning(disable : 4786)

class CXTPCalendarView;
class CXTPCalendarViewDay;
class CXTPCalendarViewGroup;
class CXTPCalendarViewEvent;

class CXTPCalendarEvent;

struct XTP_CALENDAR_HITTESTINFO;

//===========================================================================
//
// Regular schema:
//    - CalendarView consists of CalendarView(s)Day
//    - CalendarViewDay consists of CalendarView(s)Resource
//    - CalendarViewGroup contains CalendarView(s)Event
//
// ViewGroup draws a single resource depending on the type of parent
// day view.
//
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarViewGroup : public CXTPCalendarWMHandler
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarView;
	friend class CXTPCalendarViewDay;
	friend class CXTPCalendarViewEvent;
	friend class CXTPCalendarTheme;

	DECLARE_DYNAMIC(CXTPCalendarViewGroup)
	//}}AFX_CODEJOCK_PRIVATE

	typedef CXTPCalendarWMHandler TBase;    // Base class shortcut
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor. Construct CXTPCalendarViewGroup object.
	// See Also: ~CXTPCalendarViewGroup()
	//-----------------------------------------------------------------------
	CXTPCalendarViewGroup();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member item deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewGroup();

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtain a pointer to collection of Calendar Resources.
	// Remarks:
	//     Use this method to retrieve a pointer to the Calendar Resources
	//     collection of this Group View.
	// Returns:
	//     CXTPCalendarResources pointer.
	// See Also:
	//     CXTPCalendarResources overview, SetResources, AddResource
	//-----------------------------------------------------------------------
	virtual CXTPCalendarResources* GetResources();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns data provider for this view group.
	// Returns:
	//     A pointer to the data provider object if it was previously set;
	//     otherwise, returns NULL.
	// See Also:
	//     CXTPCalendarData overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarData* GetDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a new Resource object into the collection of resources for
	//     this Group View.
	// Parameters:
	//     pResource   - Pointer to the added CXTPCalendarResource object.
	//     bWithAddRef - Set this value to TRUE to increment the reference
	//                   count of the inserted object,
	//                   FALSE to insert object without incrementing
	//                   reference count.
	// See Also:
	//     CXTPCalendarResources::Add
	//-----------------------------------------------------------------------
	virtual void AddResource(CXTPCalendarResource* pResource, BOOL bWithAddRef);

	//-----------------------------------------------------------------------
	// Summary:
	//     Assigns a new Resources collection to this Group View.
	// Parameters:
	//     pResources - Pointer to the new Resources collection to be copied
	//                  into internal collections of this Group View.
	// See Also:
	//     GetResources, AddResource
	//-----------------------------------------------------------------------
	virtual void SetResources(CXTPCalendarResources* pResources);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns default Schedule ID - the first existing one.
	// Returns:
	//     The first existing Schedule ID or
	//     XTP_CALENDAR_UNKNOWN_SCHEDULE_ID if not found.
	//-----------------------------------------------------------------------
	virtual UINT GetScheduleID();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data for
	//     all items contained in the view.
	// Parameters:
	//     dtDayDate - A COleDateTime object that contains the day's date.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	//-----------------------------------------------------------------------
	virtual void Populate(COleDateTime dtDayDate) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout depending
	//     on the provided rectangle and then calls AdjustLayout() for all
	//     of the sub-items.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     rcGroup - A CRect object that contains the rectangle coordinates
	//               that are used to draw the view.
	// Remarks:
	//     Call Populate(COleDateTime dtDayDate) prior to calling AdjustLayout().
	//     AdjustLayout2 is called by calendar control instead of AdjustLayout
	//     when theme is set.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcGroup);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcGroup); //<COMBINE AdjustLayout>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view contents using
	//     the specified device context.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before calling Draw().
	// See Also: AdjustLayout(CRect rcGroup)
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine which view item,
	//     if any, is at the specified position index, and returns additional
	//     info in an XTP_CALENDAR_HITTESTINFO struct.
	// Parameters:
	//     pt       - A CPoint object that contains the coordinates of
	//                the point to test.
	//     pHitTest - A pointer to an XTP_CALENDAR_HITTESTINFO struct that
	//                contains information about the point to test.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     A BOOL. TRUE if the item is found. FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO
	//-----------------------------------------------------------------------
	virtual BOOL HitTest(CPoint pt, XTP_CALENDAR_HITTESTINFO* pHitTest) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the group's rectangle area.
	// Returns:
	//     A CRect that contains the rectangle coordinates of the group view
	//     in client coordinates of the parent's CXTPDatePickerControl object.
	// See Also:
	//     GetHeaderRect
	//-----------------------------------------------------------------------
	virtual CRect GetRect()
	{
		return m_Layout.m_rcGroup;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the group's header rectangle
	//     area.
	// Returns:
	//     A CRect that contains the rectangle coordinates of the group view
	//     header part in client coordinates of the parent's
	//     CXTPDatePickerControl object.
	// See Also:
	//     GetRect
	//-----------------------------------------------------------------------
	virtual CRect GetHeaderRect()
	{
		return m_Layout.m_rcGroupHeader;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of event views
	//     in the day view collection.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     An int that contains the number of event views.
	//-----------------------------------------------------------------------
	virtual int GetViewEventsCount() = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to a
	//     CXTPCalendarViewEvent object associated with the event's index.
	// Parameters:
	//     nIndex  - An int that contains the event view index in the day
	//               view collection.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     A pointer to a CXTPCalendarViewEvent object.
	// See Also: GetViewEventsCount()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewEvent* GetViewEvent_(int nIndex) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the containing
	//     CXTPCalendarViewDay object.
	// Returns:
	//     A pointer to the CXTPCalendarViewDay object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewDay* GetViewDay_() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the index of the "this"
	//     pointer Group View in the Calendar Day View.
	// Returns:
	//     An int that contains the index of the "this" day in the Calendar
	//     Day View.
	//-----------------------------------------------------------------------
	virtual int GetGroupIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a group's view caption.
	// Returns:
	//     A CString object that contains the group's view caption.
	//-----------------------------------------------------------------------
	virtual CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of
	//     event views elements in this Group View.
	// Remarks:
	//     Call this method to retrieve the number of elements in the array
	//     of event views inside this Group View.
	//     Because indexes are zero-based, the size is 1 greater than
	//     the largest index.
	// Returns:
	//     A count of event views elements.
	// See Also:
	//     GetChildHandlerAt
	//-----------------------------------------------------------------------
	virtual int GetChildHandlersCount();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get a event view object at the
	//     specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetChildHandlersCount.
	// Remarks:
	//     Returns the array element at the specified index.
	// Returns:
	//     The pointer to the descendant of CXTPCalendarWMHandler element
	//     currently at this index.
	// See Also:
	//     GetChildHandlersCount
	//-----------------------------------------------------------------------
	virtual CXTPCalendarWMHandler* GetChildHandlerAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer a CXTPCalendarViewEvent
	//     object that is identified by the corresponding event object.
	// Parameters:
	//     pEvent  - Pointer to a CXTPCalendarEvent object.
	// Returns:
	//     A pointer to a CXTPCalendarViewEvent object.
	// See Also: CXTPCalendarEvent, CXTPCalendarViewEvent
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewEvent* GetViewEventByEvent_(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain an event view object identified
	//     by its event place number.
	// Parameters:
	//     nEventPlace - An int that contains the ViewEvent place identifier.
	// Returns:
	//     A pointer to a CXTPCalendarViewEvent object.
	// See Also: CXTPCalendarEvent, CXTPCalendarViewEvent
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewEvent* GetViewEventByPlace_(int nEventPlace);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to exchange event view positions.
	// Parameters:
	//     pViewEvent - A pointer to a CXTPCalendarViewEvent object.
	//     nNewPlace  - An int that contains the new event view position.
	// Remarks:
	//     Each event view has a unique position number for the day. In the
	//     simple case, this is the row number used to draw the event view.
	//     This function is used to avoid duplicating position numbers.
	// See Also: CXTPCalendarViewEvent::ChangeEventPlace,
	//           CXTPCalendarViewEvent::GetEventPlaceNumber,
	//           CXTPCalendarViewEvent::AddMultiDayEvent_Slave,
	//           CXTPCalendarViewEvent::AdjustLayout
	//-----------------------------------------------------------------------
	virtual void ExchangeEventPlace(CXTPCalendarViewEvent* pViewEvent, int nNewPlace);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to create a timer event.
	// Parameters:
	//     uTimeOut_ms - A UINT that contains the duration of the timer in
	//                   milliseconds.
	// Returns:
	//     A UINT that is the identifier of the timer.
	//-----------------------------------------------------------------------
	virtual UINT SetTimer(UINT uTimeOut_ms);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to destroy a timer event.
	// Parameters:
	//     uTimerID - A UINT that specifies the identifier of the timer.
	//-----------------------------------------------------------------------
	virtual void KillTimer(UINT uTimerID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to allow some customization before
	//     show events.
	// Parameters:
	//     pEvents - A pointer to events array to draw.
	// Remarks:
	//     Default implementation send XTP_NC_CALENDAR_PREPOPULATE notification.
	// See Also: XTP_NC_CALENDAR_PREPOPULATE
	//-----------------------------------------------------------------------
	virtual void OnPrePopulate(CXTPCalendarEvents* pEvents);

public:
	//{{AFX_CODEJOCK_PRIVATE
	// This structure describes coordinates of the group View.
	struct XTP_VIEW_GROUP_LAYOUT
	{
		CRect m_rcGroup;        // Group View rectangle.
		CRect m_rcGroupHeader;  // Group View header rectangle.
	};
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual XTP_VIEW_GROUP_LAYOUT& GetLayout_();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	XTP_VIEW_GROUP_LAYOUT m_Layout; // This group view client coordinates.

	CXTPCalendarResources*  m_pResources;   // An associated resource object.

};

//===========================================================================
// Summary:
//     This class represents the day view portion of the CalendarView.
// Remarks:
//     It represents a specific view of the CalendarView's associated events
//     data grouped by one day and provides basic functionality on this
//     data using user input through the keyboard and the mouse.
//
//          These are the predefined view implementations available
//          for the Calendar control:
//
//
//          * Day and work week day views - Using the CXTPCalendarDayViewDay class.
//          * Week day view               - Using the CXTPCalendarWeekViewDay class.
//          * Month day view              - Using the CXTPCalendarMonthViewDay class.
//
//
//          Furthermore, any user defined day view may also be implemented as
//          a descendant of the CXTPCalendarViewDay class.
//
//          A typical day view model consists of a collection of ViewEvent's for
//          the particular day, each of them represents a view of an CXTPCalendarEvent
//          object.
//
// See Also: CXTPCalendarViewDayT, CXTPCalendarDayViewDay,
//           CXTPCalendarWeekViewDay, CXTPCalendarMonthViewDay,
//           CXTPCalendarView, CXTPCalendarViewEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarViewDay : public CXTPCalendarWMHandler
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarView;
	friend class CXTPCalendarViewEvent;
	friend class CXTPCalendarTheme;

	DECLARE_DYNAMIC(CXTPCalendarViewDay)

	typedef CXTPCalendarWMHandler TBase;
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor. Construct CXTPCalendarViewDay object.
	// Parameters:
	//     pView - Pointer to CXTPCalendarView object.
	// See Also: ~CXTPCalendarViewDay()
	//-----------------------------------------------------------------------
	CXTPCalendarViewDay(CXTPCalendarView* pView);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member item deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewDay();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data for
	//     all items contained in the view.
	// Parameters:
	//     dtDayDate - A COleDateTime object that contains the day's date.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	//-----------------------------------------------------------------------
	virtual void Populate(COleDateTime dtDayDate) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout depending
	//     on the provided rectangle and then calls AdjustLayout() for all
	//     of the sub-items.
	// Parameters:
	//     rcDay   - A CRect object that contains the rectangle coordinates
	//               that are used to draw the view.
	// Remarks:
	//     Call Populate(COleDateTime dtDayDate) prior to calling AdjustLayout().
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the view contents using
	//     the specified device context.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	// Remarks:
	//     Call AdjustLayout() before calling Draw().
	// See Also: AdjustLayout(CRect rcDay)
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC) {
		UNREFERENCED_PARAMETER(pDC);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of
	//     resource group views elements in the collection.
	// Remarks:
	//     Call this method to retrieve the number of elements in the array.
	//     Because indexes are zero-based, the size is 1 greater than
	//     the largest index.
	// Returns:
	//     A count of resource group views elements.
	//-----------------------------------------------------------------------
	virtual int GetChildHandlersCount() { return GetViewGroupsCount(); }

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get a resource group view object at the
	//     specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetChildHandlersCount.
	// Remarks:
	//     Returns the array element at the specified index.
	// Returns:
	//     The pointer to the CXTPCalendarWMHandler element currently at this
	//     index.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarWMHandler* GetChildHandlerAt(int nIndex) {
		ASSERT_KINDOF(CXTPCalendarWMHandler, GetViewGroup_(nIndex));
		return GetViewGroup_(nIndex);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine which view item,
	//     if any, is at the specified position index, and returns additional
	//     info in an XTP_CALENDAR_HITTESTINFO struct.
	// Parameters:
	//     pt       - A CPoint object that contains the coordinates of
	//                the point to test.
	//     pHitTest - A pointer to an XTP_CALENDAR_HITTESTINFO struct that
	//                contains information about the point to test.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     A BOOL. TRUE if the item is found. FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO
	//-----------------------------------------------------------------------
	virtual BOOL HitTest(CPoint pt, XTP_CALENDAR_HITTESTINFO* pHitTest) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to a
	//     CXTPCalendarView object.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     A pointer to a CXTPCalendarView object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarView* GetView_() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to a
	//     CXTPCalendarControl object.
	// Returns:
	//     A pointer to a CXTPCalendarControl object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarControl* GetCalendarControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the associated
	//     Calendar Resources collection object.
	// Returns:
	//     A pointer to the CXTPCalendarResources collection object.
	// See Also:
	//     CXTPCalendarResources overview, CXTPCalendarResource overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarResources* GetResources();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the new Resources collection for the view day.
	// Parameters:
	//     pResources - A pointer to the CXTPCalendarResources collection object.
	// Remarks:
	//     Call this member function to programmatically set the new
	//     Resources collection for the view day.
	//-----------------------------------------------------------------------
	virtual void SetResources(CXTPCalendarResources* pResources);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of event views
	//     in the day view collection.
	// Remarks:
	//     This function left for for compatibility with previous versions
	//     and only for singe resource group mode.
	// Returns:
	//     An int that contains the number of event views.
	//-----------------------------------------------------------------------
	virtual int GetViewEventsCount();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to a
	//     CXTPCalendarViewEvent object associated with the event's index.
	// Parameters:
	//     nIndex  - An int that contains the event view index in the day
	//               view collection.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	//     This function left for for compatibility with previous versions
	//     and only for singe resource group mode.
	// Returns:
	//     A pointer to a CXTPCalendarViewEvent object.
	// See Also: GetViewEventsCount()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewEvent* GetViewEvent_(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of resource group
	//     views in the view day collection.
	// Remarks:
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     An int that contains the number of event views.
	//-----------------------------------------------------------------------
	virtual int GetViewGroupsCount() = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to a
	//     CXTPCalendarViewGroup object associated with the group's index.
	// Parameters:
	//     nIndex  - An int that contains the group view index in the day
	//               view collection.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	//     This is a pure virtual function. This function must be defined
	//     in the derived class.
	// Returns:
	//     A pointer to a CXTPCalendarViewGroup object.
	// See Also: GetViewGroupsCount()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewGroup* GetViewGroup_(int nIndex) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a day date value.
	// Returns:
	//     A COleDateTime object containing the day's date.
	// See Also: SetDayDate(COleDateTime dtDayDate)
	//-----------------------------------------------------------------------
	virtual COleDateTime GetDayDate() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the day date value.
	// Parameters:
	//     dtDayDate   - A COleDateTime object that contains the new day's
	//                   date value.
	// Returns:
	//     A COleDateTime object that contains the previous day's date value.
	// See Also: GetDayDate()
	//-----------------------------------------------------------------------
	virtual COleDateTime SetDayDate(COleDateTime dtDayDate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if all day's event
	//     views are visible.
	// Remarks:
	//     This function is used for determining if the expanding sings are
	//     drawn.
	// Returns:
	//     A BOOL. TRUE if there is at least one event that is completely
	//     invisible. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL _NoAllEventsAreVisible();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the day view is selected.
	// Returns:
	//     A BOOL. TRUE if the day view is selected. FALSE otherwise.
	// See Also: SetSelected(bSet)
	//-----------------------------------------------------------------------
	virtual BOOL IsSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the day view as selected
	// See Also: IsSelected()
	//-----------------------------------------------------------------------
	virtual void SetSelected();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the day view rectangle coordinates.
	// Returns:
	//     A CRect object containing the day view rectangle coordinates.
	// See Also: SetDayRect(CRect rcDay)
	//-----------------------------------------------------------------------
	virtual CRect GetDayRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the day view rectangle coordinates.
	// Parameters:
	//     rcDay - A CRect object containing the new rectangle coordinates.
	// See Also: GetDayRect()
	//-----------------------------------------------------------------------
	virtual void SetDayRect(CRect rcDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if there are event views
	//     invisible because the boundaries of the day view are hidden, and
	//     the day view should display the expand sign.
	// Returns:
	//     A BOOL. TRUE if there are invisible views. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsExpanded();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the expand sign rectangle.
	// Returns:
	//     A CRect object that contains the expand sign rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual CRect GetExpandSignRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to create a timer event.
	// Parameters:
	//     uTimeOut_ms - A UINT that contains the duration of the timer in
	//                   milliseconds.
	// Returns:
	//     A UINT that is the identifier of the timer.
	//-----------------------------------------------------------------------
	virtual UINT SetTimer(UINT uTimeOut_ms);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to destroy a timer event.
	// Parameters:
	//     uTimerID - A UINT that specifies the identifier of the timer.
	//-----------------------------------------------------------------------
	virtual void KillTimer(UINT uTimerID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the index of the "this"
	//     pointer day in the Calendar Day View.
	// Returns:
	//     An int that contains the index of the "this" day in the Calendar
	//     Day View.
	//-----------------------------------------------------------------------
	virtual int GetDayIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function used to process user action xtpCalendarUserAction_OnExpandDay.
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
	virtual BOOL UserAction_OnExpandDay(XTPCalendarExpandDayButton eButton);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to allow some customization before
	//     show events.
	// Remarks:
	//     Default implementation send XTP_NC_CALENDAR_PREPOPULATE_DAY notification.
	// See Also: XTP_NC_CALENDAR_PREPOPULATE
	//-----------------------------------------------------------------------
	virtual void OnPrePopulateDay();

public:
	//{{AFX_CODEJOCK_PRIVATE
	struct XTP_VIEW_DAY_LAYOUT
	{
		CRect m_rcDay;          // Day rectangle.
		CRect m_rcDayHeader;    // Day header rectangle.

		CRect m_rcExpandSign;       // Expand Sign rectangle.

		int   m_nHotState;  // Last Items Hot state.

		enum XTPEnumHotItem
		{
			xtpHotExpandButton  = 0x001,
			xtpHotDayHeader     = 0x002,
		};
	};
protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual XTP_VIEW_DAY_LAYOUT& GetLayout_();
	//}}AFX_CODEJOCK_PRIVATE
protected:
	XTP_VIEW_DAY_LAYOUT m_Layout; // Layout data.

	COleDateTime m_dtDate;          // Day date.

	CXTPCalendarResources*  m_pResources; // Resources array

protected:

};

//===========================================================================
// Summary:
//     This class template customizes, overrides and implements some
//     functionality for the CXTPCalendarViewDay base class.
// Remarks:
//     It is used as a part of the calendar control framework to build
//     <b><i>ViewDay</i></b> layer of <b>View->DayView->ViewDay</b>
//     typed objects hierarchy.
//     These are template parameters:
//     _TView      - Type of owner View object.
//     _TViewGroup - Type of View Resource objects stored in View Day.
//     _THitTest   - Type of HitTest struct, used as parameter in the
//                   member functions.
//     _TPThis     - The last derived class type in the class hierarchy.
//
//          You must provide all of the above parameters.
// See Also: CXTPCalendarViewDay
//===========================================================================
template<class _TView, class _TViewGroup, class _THitTest, class _TPThis >
class CXTPCalendarViewDayT : public CXTPCalendarViewDay
{
public:
	//------------------------------------------------------------------------
	// Remarks:
	//     Owner view class type definition.
	//------------------------------------------------------------------------
	typedef _TView TView;

	//------------------------------------------------------------------------
	// Remarks:
	//     Event views collection type definition.
	//------------------------------------------------------------------------
//  typedef CXTPCalendarPtrCollectionT< _TViewEvent > TViewEventsCollection;

	//------------------------------------------------------------------------
	// Remarks:
	//     Resource views collection type definition.
	//------------------------------------------------------------------------
	typedef CXTPCalendarPtrCollectionT< _TViewGroup > TViewGroupsCollection;
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pView - Pointer to owner view object.
	// See Also: ~CXTPCalendarViewDayT()
	//-----------------------------------------------------------------------
	CXTPCalendarViewDayT(_TView* pView) : CXTPCalendarViewDay((CXTPCalendarView*)pView)
	{
		m_pView = pView;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles class members deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewDayT() {
	};


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine which view item,
	//     if any, is at a specified position index, and returns additional
	//     info in a XTP_CALENDAR_HITTESTINFO struct.
	// Parameters:
	//     pt       - A CPoint object that contains the coordinates of
	//                the point to test.
	//     pHitTest - A pointer to a XTP_CALENDAR_HITTESTINFO struct that
	//                contains information about the point to test.
	// Remarks:
	//     Implements standard functionality for the HitTest method.
	// Returns:
	//     A BOOL. TRUE if the item is found. FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO
	//-----------------------------------------------------------------------
	virtual BOOL HitTest(CPoint pt, XTP_CALENDAR_HITTESTINFO* pHitTest)
	{
		_THitTest hitInfo;
		BOOL bRes = HitTestEx(pt, &hitInfo);
		if (bRes && pHitTest) {
			*pHitTest = (XTP_CALENDAR_HITTESTINFO)hitInfo;
		}
		return bRes;
	}

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
	virtual BOOL HitTestEx(CPoint pt, _THitTest* pHitTest)
	{
		if (!pHitTest) {
			ASSERT(FALSE);
			return FALSE;
		}

		if (!m_Layout.m_rcDay.PtInRect(pt)) {
			return FALSE;
		}

		if (m_Layout.m_rcDayHeader.PtInRect(pt)) {
			FillHitTestEx(pHitTest);
			pHitTest->uHitCode = xtpCalendarHitTestDayHeader;
			return TRUE;
		}

		if (m_Layout.m_rcExpandSign.PtInRect(pt) &&
			!XTP_SAFE_GET1(GetView_(), GetTheme(), NULL))
		{
			FillHitTestEx(pHitTest);
			pHitTest->uHitCode = xtpCalendarHitTestDayExpandButton;
			return TRUE;
		}

		int nCount = GetViewGroupsCount();
		for (int i = 0; i < nCount; i++)
		{
			_TViewGroup* pViewGroup = GetViewGroup(i);
			ASSERT(pViewGroup);
			if (pViewGroup && pViewGroup->HitTestEx(pt, pHitTest))
			{
				ASSERT(!pHitTest->pViewGroup || pHitTest->pViewGroup == pViewGroup);
				pHitTest->nGroup = i;
				pHitTest->pViewGroup = pViewGroup;
				return TRUE;
			}
		}

		FillHitTestEx(pHitTest);
		pHitTest->uHitCode = xtpCalendarHitTestDayArea;
		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill the pHitTest structure
	//     members with the default values for the current day view object.
	// Parameters:
	//     pHitTest - A Pointer to a _THitTest structure.
	// Remarks:
	//     This is a pure virtual function.  This function must be defined
	//     in the derived class.
	//-----------------------------------------------------------------------
	virtual void FillHitTestEx(_THitTest* pHitTest) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to the owner
	//     view object.
	// Returns:
	//     A CXTPCalendarView pointer to the owner view object.
	// See Also: GetView
	//-----------------------------------------------------------------------
	AFX_INLINE virtual CXTPCalendarView* GetView_() const {
		ASSERT(this);
		return this ? (CXTPCalendarView*)m_pView : NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the owner view object.
	// Returns:
	//     A _TView template pointer to the owner view object.
	// See Also: GetView_
	//-----------------------------------------------------------------------
	AFX_INLINE _TView* GetView() const {
		ASSERT(this);
		return this ? m_pView : NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of resource views
	//     in the day view collection.
	// Returns:
	//     An int that contains the number of resource views.
	//-----------------------------------------------------------------------
	virtual int GetViewGroupsCount() {
		return m_arViewGroups.GetCount();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the _TViewGroup
	//     object that corresponds to the resource's index.
	// Parameters:
	//     nIndex  - An int that contains the resource view index in the day
	//               view collection.
	// Returns:
	//     A pointer to a _TViewGroup template object.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	// See Also:
	//     GetViewGroupsCount(), GetViewGroup_()
	//-----------------------------------------------------------------------
	virtual _TViewGroup* GetViewGroup(int nIndex)
	{
		ASSERT(this);
		if (!this) {
			return NULL;
		}
		int nCount = m_arViewGroups.GetCount();
		ASSERT(nIndex >= 0 && nIndex < nCount);
		return (nIndex >= 0 && nIndex < nCount) ? m_arViewGroups.GetAt(nIndex) : NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the base CXTPCalendarViewGroup*
	//     object that corresponds to the resource's index.
	// Parameters:
	//     nIndex  - An int that contains the resource view index in the day
	//               view collection.
	// Returns:
	//     A pointer to a base of the _TViewGroup template object.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	// See Also:
	//     GetViewGroupsCount(), GetViewGroup()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewGroup* GetViewGroup_(int nIndex)
	{
		ASSERT(this);
		_TViewGroup* pViewGroup = this ? GetViewGroup(nIndex) : NULL;
		return pViewGroup;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data for
	//     all resource items contained in the view.
	// Parameters:
	//     dtDayDate - A COleDateTime object that contains the day's date.
	// Remarks:
	//     This function provides common functionality for all CXTPCalendarViewDay -
	//     derived classes.
	//-----------------------------------------------------------------------
	virtual void Populate(COleDateTime dtDayDate)
	{
		m_dtDate = CXTPCalendarUtils::ResetTime(dtDayDate);
		m_arViewGroups.RemoveAll();

		if (!GetView_()) {
			return;
		}

		OnPrePopulateDay();

		CXTPCalendarResources* pResources = GetResources();
		if (!pResources)
			return;

		_TViewGroup* pGroup = new _TViewGroup(GetPThis());
		if (!pGroup) {
			return;
		}
		m_arViewGroups.Add(pGroup);

		pGroup->SetResources(pResources);
		pGroup->Populate(m_dtDate);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout depending
	//     on the provided rectangle and then calls AdjustLayout() for all
	//     of the sub-items.
	// Parameters:
	//     rcDay   - A CRect object that contains the rectangle coordinates
	//               that are used to draw the view.
	// Remarks:
	//     Call Populate(COleDateTime dtDayDate) prior to calling AdjustLayout().
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcDay)
	{
		CXTPCalendarViewDay::AdjustLayout(pDC, rcDay);

		int nCount = GetViewGroupsCount();
		for (int i = 0; i < nCount; i++)
		{
			CXTPCalendarViewGroup* pViewGroup = GetViewGroup_(i);
			ASSERT(pViewGroup);
			if (pViewGroup) {
				pViewGroup->AdjustLayout(pDC, rcDay);
			}
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if all of the day's event
	//     views are not visible and to calculate the rectangle for the ExpandSign.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	// Returns:
	//     TRUE if there are invisible event views. FALSE otherwise.
	// See Also: CXTPCalendarViewDay::_NoAllEventsAreVisible
	//-----------------------------------------------------------------------
	virtual BOOL NoAllEventsAreVisible()
	{
		if (CXTPCalendarViewDay::_NoAllEventsAreVisible())
		{
			if (GetView_() && !GetView_()->GetTheme())
			{
				CSize szBm = XTP_SAFE_GET2(GetView_(), GetPaintManager(), GetExpandSignSize(), CSize(0) );
				m_Layout.m_rcExpandSign.SetRect(m_Layout.m_rcDay.right - szBm.cx, m_Layout.m_rcDay.bottom - szBm.cy,
					m_Layout.m_rcDay.right, m_Layout.m_rcDay.bottom);
			}
			return TRUE;
		}
		else
		{
			m_Layout.m_rcExpandSign.SetRect(m_Layout.m_rcDay.right, m_Layout.m_rcDay.bottom,
				m_Layout.m_rcDay.right, m_Layout.m_rcDay.bottom);
			return FALSE;
		}
	}

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the typed pointer to the
	//     last derived class in the class hierarchy.
	// Remarks: _TPThis - The last derived class type in the class hierarchy.
	//          This is a pure virtual function. This function must be defined
	//          in the derived class.
	// Returns:
	//     Pointer to this object as last derived class type.
	//-----------------------------------------------------------------------
	virtual _TPThis* GetPThis() = 0;


	TViewGroupsCollection m_arViewGroups; // Storage for views of groups.
private:
	_TView* m_pView; // Storage for a owner view.
};

////////////////////////////////////////////////////////////////////////////

AFX_INLINE COleDateTime CXTPCalendarViewDay::GetDayDate() const {
	return m_dtDate;
}
AFX_INLINE COleDateTime CXTPCalendarViewDay::SetDayDate(COleDateTime dtDayDate) {
	COleDateTime dtOld(m_dtDate);
	m_dtDate = CXTPCalendarUtils::ResetTime(dtDayDate);
	return dtOld;
}
AFX_INLINE CRect CXTPCalendarViewDay::GetDayRect() {
	return m_Layout.m_rcDay;
}
AFX_INLINE void CXTPCalendarViewDay::SetDayRect(CRect rcDay) {
	m_Layout.m_rcDay.CopyRect(rcDay);
}
AFX_INLINE BOOL CXTPCalendarViewDay::IsExpanded() {
	return m_Layout.m_rcExpandSign.Width() > 0 ? TRUE : FALSE;
}
AFX_INLINE CRect CXTPCalendarViewDay::GetExpandSignRect() {
	return m_Layout.m_rcExpandSign;
}

//===========================================================================
// Summary:
//     This class template customizes, overrides and implements some
//     functionality for the CXTPCalendarViewGroup base class.
// Remarks:
//     It is used as a part of the calendar control framework to build
//     <b><i>ViewGroup</i></b> layer of <b>View->DayView->ViewDay->ViewGroup</b>
//     typed objects hierarchy.
//     These are template parameters:
//     _TView      - Type of owner View object.
//     _TViewEvent - Type of View Event objects stored in Group View.
//     _THitTest   - Type of HitTest struct, used as parameter in the
//                   member functions.
//     _TPThis     - The last derived class type in the class hierarchy.
//
//          You must provide all of the above parameters.
// See Also: CXTPCalendarViewGroup
//===========================================================================
template<class _TViewDay, class _TViewEvent, class _THitTest, class _TPThis>
class CXTPCalendarViewGroupT : public CXTPCalendarViewGroup
{
public:
	//------------------------------------------------------------------------
	// Remarks:
	//     Owner view class type definition.
	//------------------------------------------------------------------------
	typedef _TViewDay TViewDay;

	//------------------------------------------------------------------------
	// Remarks:
	//     Event views collection type definition.
	//------------------------------------------------------------------------
	typedef CXTPCalendarPtrCollectionT< _TViewEvent > TViewEventsCollection;
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pView - Pointer to owner view object.
	// See Also: ~CXTPCalendarViewGroupT()
	//-----------------------------------------------------------------------
	CXTPCalendarViewGroupT(_TViewDay* pViewDay)
	{
		m_pViewDay = pViewDay;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles class members deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewGroupT() {
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine which view item,
	//     if any, is at a specified position index, and returns additional
	//     info in a XTP_CALENDAR_HITTESTINFO struct.
	// Parameters:
	//     pt       - A CPoint object that contains the coordinates of
	//                the point to test.
	//     pHitTest - A pointer to a XTP_CALENDAR_HITTESTINFO struct that
	//                contains information about the point to test.
	// Remarks:
	//     Implements standard functionality for the HitTest method.
	// Returns:
	//     A BOOL. TRUE if the item is found. FALSE otherwise.
	// See Also: XTP_CALENDAR_HITTESTINFO
	//-----------------------------------------------------------------------
	virtual BOOL HitTest(CPoint pt, XTP_CALENDAR_HITTESTINFO* pHitTest)
	{
		_THitTest hitInfo;
		BOOL bRes = HitTestEx(pt, &hitInfo);
		if (bRes && pHitTest) {
			*pHitTest = (XTP_CALENDAR_HITTESTINFO)hitInfo;
		}
		return bRes;
	}

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
	virtual BOOL HitTestEx(CPoint pt, _THitTest* pHitTest)
	{
		if (!pHitTest) {
			ASSERT(FALSE);
			return FALSE;
		}

		if (!m_Layout.m_rcGroup.PtInRect(pt)) {
			return FALSE;
		}

		if (m_Layout.m_rcGroupHeader.PtInRect(pt)) {
			FillHitTestEx(pHitTest);
			pHitTest->uHitCode = xtpCalendarHitTestGroupHeader;
			return TRUE;
		}

		int nCount = GetViewEventsCount();
		for (int i = 0; i < nCount; i++)
		{
			_TViewEvent* pViewEvent = GetViewEvent(i);
			ASSERT(pViewEvent);
			if (pViewEvent && pViewEvent->HitTestEx(pt, pHitTest))
			{
				ASSERT(!pHitTest->pViewEvent || pHitTest->pViewEvent == pViewEvent);
				pHitTest->nEvent = i;
				pHitTest->pViewEvent = pViewEvent;
				return TRUE;
			}
		}
		FillHitTestEx(pHitTest);
		pHitTest->uHitCode = xtpCalendarHitTestGroupArea;
		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill the pHitTest structure
	//     members with the default values for the current group view object.
	// Parameters:
	//     pHitTest - A Pointer to a _THitTest structure.
	// Remarks:
	//     This is a pure virtual function.  This function must be defined
	//     in the derived class.
	//-----------------------------------------------------------------------
	virtual void FillHitTestEx(_THitTest* pHitTest) = 0;

		//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to the owner
	//     view object.
	// Returns:
	//     A CXTPCalendarView pointer to the owner view object.
	// See Also: GetView
	//-----------------------------------------------------------------------
	AFX_INLINE virtual CXTPCalendarViewDay* GetViewDay_() const {
		ASSERT(this);
		return this ? (CXTPCalendarViewDay*)m_pViewDay : NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the owner view object.
	// Returns:
	//     A _TView template pointer to the owner view object.
	// See Also: GetView_
	//-----------------------------------------------------------------------
	AFX_INLINE _TViewDay* GetViewDay() const {
		ASSERT(this);
		return this ? m_pViewDay : NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of event views
	//     in the day view collection.
	// Returns:
	//     An int that contains the number of event views.
	//-----------------------------------------------------------------------
	virtual int GetViewEventsCount() {
		return m_arEvents.GetCount();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the _TViewEvent
	//     object that corresponds to the event's index.
	// Parameters:
	//     nIndex  - An int that contains the event view index in the day
	//               view collection.
	// Returns:
	//     A pointer to a _TViewEvent template object.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	// See Also:
	//     GetViewEventsCount(), GetViewEvent_()
	//-----------------------------------------------------------------------
	virtual _TViewEvent* GetViewEvent(int nIndex)
	{
		ASSERT(this);
		if (!this) {
			return NULL;
		}
		int nCount = m_arEvents.GetCount();
		ASSERT(nIndex >= 0 && nIndex < nCount);
		return (nIndex >= 0 && nIndex < nCount) ? m_arEvents.GetAt(nIndex) : NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the base CXTPCalendarViewEvent*
	//     object that corresponds to the event's index.
	// Parameters:
	//     nIndex  - An int that contains the event view index in the day
	//               view collection.
	// Returns:
	//     A pointer to a base of the _TViewEvent template object.
	// Remarks:
	//     Index number starts with 0 and cannot be negative.
	// See Also:
	//     GetViewEventsCount(), GetViewEvent()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarViewEvent* GetViewEvent_(int nIndex)
	{
		ASSERT(this);
		_TViewEvent* pViewEvent = this ? GetViewEvent(nIndex) : NULL;
		return pViewEvent;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to populate the view with data for
	//     all event items contained in the view.
	// Parameters:
	//     dtDayDate - A COleDateTime object that contains the day's date.
	// Remarks:
	//     This function provides common functionality for all CXTPCalendarViewGroup -
	//     derived classes.
	//-----------------------------------------------------------------------
	virtual void Populate(COleDateTime dtDayDate)
	{
		dtDayDate = CXTPCalendarUtils::ResetTime(dtDayDate);

		m_arEvents.RemoveAll();

		CXTPCalendarEventsPtr ptrEventsAr = new CXTPCalendarEvents();
		CXTPCalendarView* pView = XTP_SAFE_GET1(GetViewDay_(), GetView_(), NULL);
		if (!GetResources() || !pView || !ptrEventsAr) {
			return;
		}

		int nResourcesCount = GetResources()->GetCount();
		for (int nRc = 0; nRc < nResourcesCount; nRc++)
		{
			CXTPCalendarResource* pRC = GetResources()->GetAt(nRc);
			if (!pRC) {
				continue;
			}
			// to support dragging common mechanism "pView->RetrieveDayEvents" is used.
			CXTPCalendarEventsPtr ptrEvents = pView->RetrieveDayEvents(pRC, dtDayDate);
			if (!ptrEvents) {
				continue;
			}

			// append events
			ptrEventsAr->Append(ptrEvents);
		}

		// sort events by start time and some other criteria
		ptrEventsAr->Sort(CXTPCalendarEvents::CompareEvents_ForView);

		//---------------------
		CXTPCalendarEvent* pDragNew = XTP_SAFE_GET2(GetViewDay_(), GetView_(), GetDraggingEventNew(), NULL);
		CXTPCalendarEvent* pDragOrig = XTP_SAFE_GET2(GetViewDay_(), GetView_(), GetDraggingEventOrig(), NULL);

		// remove duplicated events
		int nCount = ptrEventsAr->GetCount();
		for (int nEvent = nCount - 2; nEvent >= 0; nEvent--)
		{
			CXTPCalendarEvent* pEvent0 = ptrEventsAr->GetAt(nEvent);
			CXTPCalendarEvent* pEvent1 = ptrEventsAr->GetAt(nEvent + 1);
			if (!pEvent0 || !pEvent1) {
				ASSERT(FALSE);
				continue;
			}
			if (pEvent0 == pDragNew || pEvent1 == pDragNew) {
				continue;
			}

			if (pEvent0->GetDataProvider() == pEvent1->GetDataProvider() &&
				pEvent0->IsEqualIDs(pEvent1))
			{
				ASSERT(pEvent0->GetScheduleID() == pEvent1->GetScheduleID());
				ptrEventsAr->RemoveAt(nEvent + 1);
			}
		}

		// dragging support
		if (pDragNew && !pDragOrig)
		{
			int nFIdx = ptrEventsAr->Find(pDragNew);
			if (nFIdx > 0)
			{
				ptrEventsAr->InsertAt(0, pDragNew, TRUE);
				ptrEventsAr->RemoveAt(nFIdx+1);
			}
		}

		// to allow some customization before show events
		OnPrePopulate(ptrEventsAr);

		// Create events views:
		nCount = ptrEventsAr->GetCount();
		for (int nEventId = 0; nEventId < nCount; nEventId++)
		{
			CXTPCalendarEvent* pEvent = ptrEventsAr->GetAt(nEventId);
			if (!pEvent) {
				ASSERT(FALSE);
				continue;
			}

			// the same checking is in the data provider.
			// check again for dragging event.
			if (!(pEvent->GetStartTime() < dtDayDate &&
				CXTPCalendarUtils::IsEqual(pEvent->GetEndTime(), dtDayDate)) )
			{
				m_arEvents.Add(new _TViewEvent(pEvent, GetPThis()));
			}

			if (dtDayDate < CXTPCalendarUtils::ResetTime(pEvent->GetStartTime()) ||
				dtDayDate > CXTPCalendarUtils::ResetTime(pEvent->GetEndTime()) )
			{
				ASSERT(FALSE);
				TRACE(_T("ERROR data: Day: %s  EVENT {%s - %s} \n"),
					(LPCTSTR)dtDayDate.Format(_T("#%Y-%m-%d#")),
					(LPCTSTR)CXTPCalendarUtils::ResetTime(pEvent->GetStartTime()).Format(_T("#%Y-%m-%d#")),
					(LPCTSTR)CXTPCalendarUtils::ResetTime(pEvent->GetEndTime()).Format(_T("#%Y-%m-%d#"))
					);
			}
		}
	}

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the typed pointer to the
	//     last derived class in the class hierarchy.
	// Remarks: _TPThis - The last derived class type in the class hierarchy.
	//          This is a pure virtual function. This function must be defined
	//          in the derived class.
	// Returns:
	//     Pointer to this object as last derived class type.
	//-----------------------------------------------------------------------
	virtual _TPThis* GetPThis() = 0;


	TViewEventsCollection m_arEvents; // Storage for events views.
private:
	_TViewDay* m_pViewDay; // Parent day view
};

////////////////////////////////////////////////////////////////////////////
AFX_INLINE CXTPCalendarViewDay::XTP_VIEW_DAY_LAYOUT& CXTPCalendarViewDay::GetLayout_() {
	return m_Layout;
}

AFX_INLINE CXTPCalendarResources* CXTPCalendarViewGroup::GetResources() {
	return m_pResources;
}

AFX_INLINE int CXTPCalendarViewGroup::GetChildHandlersCount() {
	return GetViewEventsCount();
}

AFX_INLINE CXTPCalendarViewGroup::XTP_VIEW_GROUP_LAYOUT& CXTPCalendarViewGroup::GetLayout_() {
	return m_Layout;
}

////////////////////////////////////////////////////////////////////////////
#endif // !defined(_XTPCALENDARVIEWDAY_H__)
