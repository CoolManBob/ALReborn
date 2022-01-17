// XTPCalendarWeekViewEvent.h: interface for the CXTPCalendarWeekViewEvent class.
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
#if !defined(__XTPCALENDARWEEKVIEWEVENT_H_)
#define __XTPCALENDARWEEKVIEWEVENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

#include "XTPCalendarViewEvent.h"
//#include "XTPCalendarWeekViewDay.h"

class CXTPCalendarControl;
class CXTPCalendarWeekView;
class CXTPCalendarWeekViewDay;
class CXTPCalendarWeekViewGroup;
class CXTPCalendarEvent;
struct XTP_CALENDAR_HITTESTINFO_WEEK_VIEW;

//===========================================================================
// Summary:
//     This class represents an event view portion of the CalendarWeekViewDay.
// Remarks:
//     It represents a specific view of the event associated data according
//     to various view types and display settings and provides basic
//     functionality on this data using user input through the keyboard and mouse.
//
//          CXTPCalendarWeekViewEvent is based on CXTPCalendarViewEventT template
//          class. CXTPCalendarWeekViewEvent inherits basic behavior from CXTPCalendarViewEventT.
//          Furthermore, CXTPCalendarWeekViewEvent overlaps some functionality in
//          CXTPCalendarViewEventT and also adds some new functionality for its
//          own behavior and look.
//
// See Also: CXTPCalendarViewEvent, CXTPCalendarViewEventT
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarWeekViewEvent : public CXTPCalendarViewEventT<
										CXTPCalendarWeekViewGroup,
										XTP_CALENDAR_HITTESTINFO_WEEK_VIEW >
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarWeekViewEvent)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//------------------------------------------------------------------------
	// Summary:
	//     Base class type definition.
	//------------------------------------------------------------------------
	typedef CXTPCalendarViewEventT< CXTPCalendarWeekViewGroup,
									XTP_CALENDAR_HITTESTINFO_WEEK_VIEW> TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pViewDay - A pointer to a CXTPCalendarWeekViewDay object.
	//     pEvent   - A pointer to a CXTPCalendarEvent.
	// See Also: ~CXTPCalendarWeekViewDay()
	//-----------------------------------------------------------------------
	CXTPCalendarWeekViewEvent(CXTPCalendarEvent* pEvent, CXTPCalendarWeekViewGroup* pViewGroup);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarWeekViewEvent();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill a XTP_CALENDAR_HITTESTINFO_WEEK_VIEW structure.
	// Parameters:
	//     point - A CPoint object that contains the point to test.
	//     pInfo - A pointer to a XTP_CALENDAR_HITTESTINFO_WEEK_VIEW struct.
	// Remarks:
	//     Call this member function to gather hit test information from
	//     the day view.
	// See Also: XTP_CALENDAR_HITTESTINFO_WEEK_VIEW
	//-----------------------------------------------------------------------
	virtual BOOL HitTestEx(CPoint point, XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust rectangles used to draw
	//     event view icons.
	// Parameters:
	//     rc - A CRect that contains the rectangle coordinates used to
	//          draw the view.
	// Remarks:
	//     Call this member function to calculate rectangles to draw event
	//     view icons. Depends on what glyphs should be drawn based on the
	//     event view rect.
	//-----------------------------------------------------------------------
	virtual int CalcIconsRect(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the view's layout
	//     depending on the provided rectangle and also calls AdjustLayout()
	//     for all sub-items.
	// Parameters:
	//     rcEventMax          - An int that contains the rectangle coordinates
	//                           used to draw the view.
	//     nEventPlaceNumber   - An int that contains the sequential place number.
	// Remarks:
	//     Call Populate(COleDateTime dtDayDate) prior calling AdjustLayout().
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcEventMax, int nEventPlaceNumber);
	virtual void AdjustLayout2(CDC* pDC, const CRect& rcEventMax, int nEventPlaceNumber); //<COMBINE AdjustLayout>

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
	//     Call this member function to determine if the "view visible" flag is set.
	// Returns:
	//     A BOOL. TRUE if the "view visible" is set. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsVisible();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the day view rectangle coordinates.
	// Returns:
	//     A CRect object with the day rectangle coordinates.
	// See Also: SetDayRect(CRect rcDay)
	//-----------------------------------------------------------------------
	CRect GetViewRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the value of the "show end time" flag.
	// Returns:
	//     A BOOL. TRUE if "show end time" flag is set. FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsShowEndTime();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the value of the "show time as clock" flag.
	// Returns:
	//     A BOOL. TRUE if the "show time as clock" flag is set. FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsTimeAsClock();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to perform additional adjustments.
	// Remarks:
	//     Call this member function to perform additional adjustments after
	//     all adjustment activities are completed.
	//-----------------------------------------------------------------------
	virtual void OnPostAdjustLayout();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the editor window font.
	// Returns:
	//     A pointer to a CFont object that contains the editor window font.
	//-----------------------------------------------------------------------
	virtual CFont* GetSubjectEditorFont();
};
////////////////////////////////////////////////////////////////////////////

AFX_INLINE CRect CXTPCalendarWeekViewEvent::GetViewRect() {
	return m_rcEvent;
}

#endif // !defined(__XTPCALENDARWEEKVIEWEVENT_H_)
