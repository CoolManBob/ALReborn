// XTPDatePickerItemDay.h: interface for the CXTPDatePickerItemDay class.
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
#if !defined(_XTPDATEPICKERITEMDAY_H__)
#define _XTPDATEPICKERITEMDAY_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPDatePickerControl;
class CXTPDatePickerItemMonth;

//===========================================================================
// Summary:
//     Class CXTPDatePickerItemDay represents one displayed day item in
//     the calendar.
// Remarks:
//     To create the object, call the constructor and provide pointers
//     to the parent CXTPDatePickerControl and CXTPDatePickerItemMonth
//     objects and a date value corresponding to its displayed day.
// See Also: CXTPDatePickerControl, CXTPDatePickerItemMonth
//===========================================================================
class _XTP_EXT_CLASS CXTPDatePickerItemDay : public CXTPCmdTarget
{
public:
	// ----------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// Parameters:
	//     pControl :  Pointer to the DatePicker control.
	//     pMonth :    Pointer to the corresponding Month item.
	//     dtDay :     Date value.
	// Remarks:
	//     Handles initial initialization.
	// ----------------------------------------------------
	CXTPDatePickerItemDay(CXTPDatePickerControl* pControl, CXTPDatePickerItemMonth* pMonth, COleDateTime dtDay);

	// -------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member item deallocation.
	// -------------------------------------
	virtual ~CXTPDatePickerItemDay();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a day item.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	// -----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the day's rectangle area.
	// Parameters:
	//     rcDay :  Day rectangle in client CXTPDatePickerControl coordinates.
	// See Also:
	//     GetRect()
	// -----------------------------------------------------------------------
	void SetRect(const CRect& rcDay);

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the day's rectangle area.
	// Returns:
	//     A CRect that contains the rectangle coordinates of the day item in
	//     client coordinates of the parent's CXTPDatePickerControl object.
	// See Also:
	//     SetRect(const CRect& rcDay)
	// ----------------------------------------------------------------------
	CRect GetRect();

	// ---------------------------------------------------------
	// Summary:
	//     Call this member function to get the date of the day.
	// Returns:
	//     The date of the day shown.
	// See Also:
	//     CXTPDatePickerItemDay::CXTPDatePickerItemDay
	// ---------------------------------------------------------
	COleDateTime GetDate();

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the day is visible.
	// Returns:
	//     Boolean value that determines if the day item is visible. TRUE if
	//     the day is visible. FALSE otherwise.
	// ---------------------------------------------------------------------
	BOOL IsVisible();


	// -----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the day is the
	//     "Today" date.
	// Parameters:
	//     dt :  Reference to the COleDateTime object representing the date to
	//           check.
	// Returns:
	//     A BOOL that indicates if the day is the "Today" date. TRUE if the
	//     day is the "Today" date. Otherwise FALSE.
	// -----------------------------------------------------------------------
	BOOL IsToday(COleDateTime &dt);

	// -----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get a parent month object.
	// Returns:
	//     A pointer to CXTPDatePickerItemMonth object which is an owner of this
	//     day object.
	// -----------------------------------------------------------------------
	CXTPDatePickerItemMonth* GetMonth() const;

	// -----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get a parent date picker object.
	// Returns:
	//     A pointer to CXTPDatePickerControl parent object.
	// -----------------------------------------------------------------------
	CXTPDatePickerControl* GetDatePickerControl();

// Attributes
protected:
	CXTPDatePickerControl* m_pControl;  // -----------------------------------------------------------
	                                    // This member variable is a pointer to the parent date picker
	                                    // control.
	                                    // -----------------------------------------------------------
	CXTPDatePickerItemMonth* m_pMonth;  // -----------------------------------------------------------
	                                    // This member variable is a pointer to the parent month item.
	                                    // -----------------------------------------------------------
	COleDateTime m_dtDay;               // -------------------------------------------------------------
	                                    // This member variable is used to specify the visible day date,
	                                    // represented by this Day item.
	                                    // -------------------------------------------------------------

	CRect m_rcDay;   // ------------------------------------------------------------------
	                 // This member variable is used to specify the coordinates of the day
	                 // spot.
	                 // ------------------------------------------------------------------
	BOOL m_bVisible; // -----------------------------------------------------------------
	                 // This member variable is used as a flag to specify if the day item
	                 // is visible.The flag values are TRUE if the day item is drawn, otherwise FALSE.
	                 // -----------------------------------------------------------------

protected:
	friend class CXTPDatePickerControl;
};

AFX_INLINE void CXTPDatePickerItemDay::SetRect(const CRect& rcDay) {
	m_rcDay = rcDay;
}

AFX_INLINE CRect CXTPDatePickerItemDay::GetRect() {
	return m_rcDay;
}

AFX_INLINE COleDateTime CXTPDatePickerItemDay::GetDate() {
	return m_dtDay;
}

AFX_INLINE BOOL CXTPDatePickerItemDay::IsVisible() {
	return m_bVisible;
}

AFX_INLINE CXTPDatePickerItemMonth* CXTPDatePickerItemDay::GetMonth() const {
	return m_pMonth;
}

AFX_INLINE CXTPDatePickerControl* CXTPDatePickerItemDay::GetDatePickerControl()
{
	return m_pControl;
}

#endif // !defined(_XTPDATEPICKERITEMDAY_H__)
