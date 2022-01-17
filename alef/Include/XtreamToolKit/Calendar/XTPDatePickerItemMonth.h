// XTPDatePickerItemMonth.h: interface for the CXTPDatePickerItemMonth class.
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
#if !defined(_XTPDATEPICKERITEMMONTH_H__)
#define _XTPDATEPICKERITEMMONTH_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE
// Maximum amount of weeks shown in one month area.
const UINT XTP_MAX_WEEKS = (UINT)6;

// Number of days in the week.
const UINT XTP_WEEK_DAYS = (UINT)7;
//}}AFX_CODEJOCK_PRIVATE

class CXTPDatePickerControl;
class CXTPDatePickerItemDay;

//===========================================================================

//-----------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerItemMonth represents one month inside the
//     date picker month grid.
// Remarks:
//     Call the constructor to create the object and provide a pointer
//     to the DatePicker control. The date value corresponds to the first
//     day of the month and the grid coordinates. It is not necessary to
//     add day items to the month item after creation. The month item
//     constructor calls the day item constructor and fills the day items.
// See Also: CXTPDatePickerItemDay
//===========================================================================
class _XTP_EXT_CLASS CXTPDatePickerItemMonth : public CXTPCmdTarget
{
//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPDatePickerControl;
	friend class CXTPDatePickerPaintManager;
	friend class CXTPDatePickerTheme;
//}}AFX_CODEJOCK_PRIVATE

public:
	// ------------------------------------------------------------------------
	// Summary:
	//     Default CXTPDatePickerItemMonth object constructor.
	// Parameters:
	//     pControl :  A CXTPDatePickerControl pointer to the parent DatePicker
	//                 control.
	// dtMonth :   A COleDateTime object date of the first day of the month.
	// nRow :      An int that contains the vertical coordinate of the month
	//             in the control's months grid.
	// nCol :      An int that contains the horizontal coordinate of the month
	//             in the control's months grid.
	// Remarks:
	//     Handles initial initialization, populates the month item with the
	//     corresponding day items.
	// See Also:
	//     PopulateDays(), CXTPDatePickerItemDay
	// ------------------------------------------------------------------------
	CXTPDatePickerItemMonth(CXTPDatePickerControl* pControl, COleDateTime dtMonth, int nRow, int nCol);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member items deallocation.
	// See Also: ClearDays()
	//-----------------------------------------------------------------------
	virtual ~CXTPDatePickerItemMonth();

	// ----------------------------------------------------------------------------
	// Summary:
	//     Call this member function to adjust the layout of all sub-items.
	// Parameters:
	//     rcClient :  A CRect object that contains the coordinates of the control.
	//     bIsAuto :   A BOOL that determines the method that is used to adjust the
	//                 layout\: If TRUE \- The layout is adjusted depending on the
	//                 given control client area rectangle. If FALSE \- The layout
	//                 calculations depend on the current display settings of all
	//                 included items (day items, header items) and the current
	//                 device context.
	// Remarks:
	//     Adjusts the layout of all sub-items in two different ways
	//     depending on the bIsAuto parameter. If bIsAuto = TRUE, then the
	//     layout is adjusted depending on the given control's client area
	//     rectangle. In this case the given count of month items is adjusted
	//     to fit the client area. If bIsAuto = FALSE, then the layout
	//     calculation depends on the current display settings of all
	//     included items (day items, header items) and the current device
	//     context. In this case the size of one month item is calculated
	//     first, this then determines the count of the month items that fit
	//     into the control's client rectangle. Note: The second method is
	//     the default mode.
	// ----------------------------------------------------------------------------
	void AdjustLayout(CRect rcClient, BOOL bIsAuto = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to perform drawing logic.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// Remarks:
	//     This function performs all drawing logic.
	//     This member function is used to display the month item. It does not
	//     perform any adjustments or size corrections. You must call
	//     AdjustLayout(CRect rcClient, BOOL bIsAuto = FALSE) first to
	//     handle all adjustments.
	// See Also: AdjustLayout(CRect rcClient, BOOL bIsAuto = FALSE)
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the month date.
	// Remarks:
	//     Use this member function to identify the month by date. Use the
	//     date that corresponds to the first day of the month.
	// Returns:
	//     A ColeDateTime object that contains the month date for which this
	//     item represents.
	// ---------------------------------------------------------------------
	COleDateTime GetMonth();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the days before a given
	//     month are displayed.
	// Remarks:
	//     This option allows changing the behavior of the control in the
	//     following way: If the first day of the month is Wednesday you may
	//     wish to leave places for Monday and Tuesday items empty to display
	//     the last days from previous month item.
	// Returns:
	//     Boolean value that determines if the days before this month are
	//     visible. TRUE if the days before this month are visible. Otherwise
	//     FALSE.
	// See Also:
	//     SetShowDaysBefore(BOOL bShow), SetShowDaysAfter(BOOL bShow),
	//     GetShowDaysAfter()
	// ----------------------------------------------------------------------
	BOOL GetShowDaysBefore();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine whether to show the days
	//     after this month.
	// Remarks:
	//     This option allows changing the behavior of the control in the
	//     following way: If the last day of the month is Wednesday you may
	//     wish to leave places for the remaining day items empty or display
	//     the beginning days from the next month item.
	// Returns:
	//     Boolean value determines whether to show the days after this
	//     month. TRUE - If the days after this month are visible. FALSE - If
	//     the days after this month are not visible.
	// See Also:
	//     GetShowDaysBefore()
	// ----------------------------------------------------------------------
	BOOL GetShowDaysAfter();

	// ---------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set whether days before this month
	//     are shown.
	// Parameters:
	//     bShow :  Boolean value that determines if the days before this month are
	//              displayed.<p/>TRUE if the days before this month are shown.<p/>FALSE
	//              if the days before this month are not shown.
	// Remarks:
	//     This option allows changing the behavior of the control in the
	//     following way: If the first day of the month is Wednesday you may
	//     leave places for Monday and Tuesday items empty or display the
	//     last days from the previous month item.
	// See Also:
	//     GetShowDaysBefore()
	// ---------------------------------------------------------------------------------
	void SetShowDaysBefore(BOOL bShow);

	// ----------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set whether days after this month are
	//     shown.
	// Parameters:
	//     bShow :  Boolean value that determines if the days after this month are
	//              displayed.<p/>TRUE if the days after this month are visible.<p/>FALSE
	//              if the days after this month are not visible.
	// Remarks:
	//     This option allows changing the behavior of the control in the
	//     following way: If the last day of month is Wednesday you may wish
	//     to leave places for remaining day items empty or display the
	//     beginning days from next month item.
	// See Also:
	//     GetShowDaysAfter()
	// ----------------------------------------------------------------------------------
	void SetShowDaysAfter(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the left scroll
	//     triangle is visible.
	// Remarks:
	//     This option allows using scrolling facilities to find and work
	//     with month items that currently are not shown in the control.
	//     ScrollV to desired month item using the scroll triangle
	//     icon.
	// Returns:
	//     Boolean value determines if the left scroll triangle is visible.
	//     TRUE - If the left scroll triangle is visible.
	//     FALSE - If the left scroll triangle is not visible.
	// See Also: GetShowRightScroll, SetShowScrolling
	//-----------------------------------------------------------------------
	BOOL GetShowLeftScroll();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the right scroll
	//     triangle is visible.
	// Remarks:
	//     This option allows scrolling facilities to find and work with
	//     month items that currently are not shown in the control. ScrollV to
	//     desired month item using the scroll triangle icon.
	// Returns:
	//     Boolean value that determines if the right scroll triangle is
	//     visible.
	//     TRUE  - If the right scroll triangle is visible.
	//     FALSE - If the right scroll triangle is not visible.
	// See Also:
	//     GetShowLeftScroll, SetShowScrolling
	// ----------------------------------------------------------------------
	BOOL GetShowRightScroll();

	// ------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to add the left and the right scrolling
	//     triangles.
	// Parameters:
	//     bLeftScroll :   Boolean value determines if the left scroll triangle is
	//                     visible.<p/>TRUE Left scrolling triangle is visible. Right
	//                     scrolling triangle is visible.<p/>FALSE Left scrolling
	//                     triangle is not visible. Right scrolling triangle is not
	//                     visible.
	// bRightScroll :  Boolean value determines if the right scroll triangle is
	//                 visible.
	// Remarks:
	//     This option allows scrolling facilities to find and work with
	//     month items that currently aren't shown in the control. ScrollV to
	//     the desired month item using the scroll triangle icon. In some
	//     cases scrolling is not needed, and you can hide one or both
	//     scrolling triangles.
	// See Also:
	//     GetShowLeftScroll, GetShowRightScroll
	// ------------------------------------------------------------------------------
	void SetShowScrolling(BOOL bLeftScroll, BOOL bRightScroll);

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the day item by its index in
	//     the day collection of the month.
	// Parameters:
	//     nIndex :  An int that contains the zero\-based index value.
	// Remarks:
	//     If the index value is less than 0 and greater then the value
	//     returned by GetDayCount() the function returns NULL.
	// Example:
	// <code>
	// // Enumerate the month array.
	// CPoint point (100, 100);
	// int nDayCount = GetDayCount();
	// for (int nDay = 0; nDay \< nDayCount; nDay++)
	// {
	//     CXTPDatePickerItemDay* pDay = GetDay(nDay);
	//     if (pDay && pDay-\>GetRect().PtInRect(point) && pDay-\>IsVisible())
	//     return pDay;
	// }
	// </code>
	// Returns:
	//     A pointer to a CXTPDatePickerItemDay object or NULL if the index
	//     is invalid.
	// See Also:
	//     GetDayCount()
	// ----------------------------------------------------------------------
	CXTPDatePickerItemDay* GetDay(int nIndex);

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of day items in the
	//     month collection.
	// Returns:
	//     An int that contains the number of items stored in the collection.
	// See Also:
	//     GetDay(int nIndex)
	// ----------------------------------------------------------------------
	int GetDayCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to return the day where the mouse cursor was
	//     located when the left mouse button was clicked.
	// Parameters:
	//     point :  A CPoint object that specifies the x\- and y\- coordinates
	//              of the cursor.
	//     bCheckVisible : TRUE when check visibility flag of the day item.
	//              If this parameter is FALSE, the method would return pointer
	//              to the corresponding day item even if this day is hidden
	//              (for example days from previous month).
	//              It is TRUE by default.
	// Remarks:
	//     This member function is heavily used by all event handlers to
	//     determine the item that was disturbed by the mouse pointer. If
	//     item was found, then the function returns a pointer to item,
	//     \otherwise NULL is returned.
	// Returns:
	//     \Returns a pointer to the affected day item. Returns NULL if the
	//     left mouse button was clicked outside any day area.
	//-----------------------------------------------------------------------
	CXTPDatePickerItemDay* HitTest(CPoint point, BOOL bCheckVisible = TRUE);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to cleanup the days array.
	// Remarks:
	//     Handles cleanup of days collection, including each day item.
	//-----------------------------------------------------------------------
	void ClearDays();

	// ----------------------------------------------------------
	// Summary:
	//     Call this member function to populate the day objects.
	// Remarks:
	//     Creates days objects based on the current settings.
	// ----------------------------------------------------------
	void PopulateDays();

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function is called to process left button down mouse
	//     messages.
	// Parameters:
	//     nFlags :  A UINT that is used to indicate whether various virtual
	//               keys are down.
	// point :   A CPoint object that specifies the x\- and y\- coordinates
	//           of the cursor.
	// Remarks:
	//     This function is called from control event handlers.
	// ---------------------------------------------------------------------
	void OnLButtonDown(UINT nFlags, CPoint point);

	// --------------------------------------------------------------------
	// Summary:
	//     This member function is called to process left button up mouse
	//     messages.
	// Parameters:
	//     nFlags :  A UINT that indicates whether various virtual keys are
	//               down.
	// point :   A CPoint object that specifies the x\- and y\- coordinates
	//           of the cursor.
	// Remarks:
	//     This function is called from control event handlers.
	// --------------------------------------------------------------------
	void OnLButtonUp(UINT nFlags, CPoint point);

	// --------------------------------------------------------------------
	// Summary:
	//     This function is called to process mouse move messages.
	// Parameters:
	//     nFlags :  A UINT that indicates whether various virtual keys are
	//               down.
	// point :   A CPoint object that specifies the x\- and y\- coordinates
	//               of the cursor.
	// Remarks:
	//     This function is called from control event handlers.
	// --------------------------------------------------------------------
	void OnMouseMove(UINT nFlags, CPoint point);

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function is called to process changing cursor message.
	// Parameters:
	//     point : A CPoint object that specifies the x\- and y\- coordinates
	//             of the cursor.
	// Remarks:
	//     This function is called from control event handlers.
	// Returns:
	//     Nonzero to halt further processing, or 0 to continue.
	// ---------------------------------------------------------------------
	BOOL OnSetCursor(CPoint point);

	// ------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to calculate sizes and counts by font
	//     metrics.
	// Parameters:
	//     rcClient :  A CRect object that contains the coordinates of the control and
	//                 the location where the control is drawn.
	// Remarks:
	//     This member function implements one of the branches of
	//     AdjustLayout(). If FALSE - The calculated layout depends on
	//     correct display settings of all included items (day items, header
	//     items) and current device context. In this case, the size of one
	//     month item is calculated first, then it determines the count of
	//     month items that fit into the control client rectangle.
	// See Also:
	//     AdjustLayout()
	// ------------------------------------------------------------------------------
	void ByFontAdjustLayout(CRect rcClient);

	// ---------------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the size of the header,
	//     days of week, week numbers, and the scrolling triangles bounding
	//     rectangles.
	// Parameters:
	//     rcClient :  A CRect object that contains the coordinates that specifies
	//                 where the control is drawn.
	// Remarks:
	//     This member function implements one of the branches of
	//     AdjustLayout(). If TRUE, then the layout is adjusted depending on
	//     the given control client area rectangle. In this case, the given
	//     count of month items is adjusted to fit the client area.
	// See Also:
	//     AdjustLayout()
	// ---------------------------------------------------------------------------
	void AutoAdjustLayout(CRect rcClient);

// Attributes
protected:
	CXTPDatePickerControl* m_pControl;  // -----------------------------------------------------------------
	                                    // This member variable is a CXTPDatePickerControl object pointer to
	                                    // the parent control
	                                    // -----------------------------------------------------------------
	int m_nRow;                         // -------------------------------------------------------------
	                                    // This member variable is an int that is used for the row month
	                                    // index.
	                                    // -------------------------------------------------------------
	int m_nColumn;                      // ----------------------------------------------------------------
	                                    // This member variable is an int that is used for the column month
	                                    // index.
	                                    // ----------------------------------------------------------------
	COleDateTime m_dtMonth;             // ---------------------------------------------------------------
	                                    // This member variable is COleDateTime object that is used as the
	                                    // date with the month definition for the month item.
	                                    // ---------------------------------------------------------------
	BOOL m_bShowDaysBefore;             // ------------------------------------------------------------------
	                                    // This member variable is a BOOL that is used to determine if the
	                                    // "days before this month" are visible. The valid values are TRUE if
	                                    // the "days before this month" are visible, otherwise false.
	                                    // ------------------------------------------------------------------
	BOOL m_bShowDaysAfter;              // -----------------------------------------------------------------
	                                    // This member variable is a BOOL that is used to indicate if the
	                                    // "days after this month" are visible. The valid values are TRUE if
	                                    // the "days after this month" are visible, otherwise FALSE.
	                                    // -----------------------------------------------------------------
	BOOL m_bShowLeftScroll;             // ---------------------------------------------------------------
	                                    // This member variable is a BOOL that is used to determine if the
	                                    // "left scroll triangle" is visible. The valid values are TRUE if
	                                    // the "left scroll triangle" is visible, otherwise false.
	                                    // ---------------------------------------------------------------
	BOOL m_bShowRightScroll;            // ----------------------------------------------------------------
	                                    // This member variable is a BOOL that is used to determine if the
	                                    // "right scroll triangle" is visible. The valid values are TRUE if
	                                    // the "right scroll triangle" is visible, otherwise false.
	                                    // ----------------------------------------------------------------

	CRect m_rcMonth;                    // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the total month's area.
	                                    // --------------------------------------------------------
	CRect m_rcHeader;                   // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the month's header area.
	                                    // --------------------------------------------------------
	CRect m_rcDaysOfWeek;               // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the month's "week days" area.
	                                    // --------------------------------------------------------
	CRect m_rcWeekNumbers;              // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the month's week numbers area.
	                                    // --------------------------------------------------------
	CRect m_rcDaysArea;                 // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the month's "all days" area.
	                                    // --------------------------------------------------------
	CRect m_rcLeftScroll;               // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the left scroll triangle area.
	                                    // --------------------------------------------------------
	CRect m_rcRightScroll;              // --------------------------------------------------------
	                                    // This member variable is a CRect object that contains the
	                                    // coordinates of the right scroll triangle area.
	                                    // --------------------------------------------------------

	CArray<CXTPDatePickerItemDay*, CXTPDatePickerItemDay*> m_arrDays;   // -----------------------------------------------------------
	                                                                    // This member variable is a CArray\<CXTPDatePickerItemDay*,
	                                                                    // CXTPDatePickerItemDay*\> object that is used as an internal
	                                                                    // storage area for the day items.
	                                                                    // -----------------------------------------------------------
private:
//{{AFX_CODEJOCK_PRIVATE
	static void Swap(COleDateTime& dtFirst, COleDateTime& dtSecond);
//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE COleDateTime CXTPDatePickerItemMonth::GetMonth() {
	return m_dtMonth;
}

AFX_INLINE BOOL CXTPDatePickerItemMonth::GetShowDaysBefore() {
	return m_bShowDaysBefore;
}
AFX_INLINE BOOL CXTPDatePickerItemMonth::GetShowDaysAfter() {
	return m_bShowDaysAfter;
}
AFX_INLINE void CXTPDatePickerItemMonth::SetShowDaysBefore(BOOL bShow) {
	m_bShowDaysBefore = bShow;
}
AFX_INLINE void CXTPDatePickerItemMonth::SetShowDaysAfter(BOOL bShow) {
	m_bShowDaysAfter = bShow;
}
AFX_INLINE BOOL CXTPDatePickerItemMonth::GetShowLeftScroll() {
	return m_bShowLeftScroll;
}
AFX_INLINE BOOL CXTPDatePickerItemMonth::GetShowRightScroll() {
	return m_bShowRightScroll;
}
AFX_INLINE void CXTPDatePickerItemMonth::SetShowScrolling(BOOL bLeftScroll, BOOL bRightScroll) {
	m_bShowLeftScroll = bLeftScroll;
	m_bShowRightScroll = bRightScroll;
}
AFX_INLINE void CXTPDatePickerItemMonth::Swap(COleDateTime& dtFirst, COleDateTime& dtSecond) {
	COleDateTime dtTemp = dtFirst;
	dtFirst = dtSecond;
	dtSecond = dtTemp;
}
#endif // !defined(_XTPDATEPICKERITEMMONTH_H__)
