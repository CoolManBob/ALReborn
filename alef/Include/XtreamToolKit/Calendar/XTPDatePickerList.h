// XTPDatePickerItemList.h: interface for the CXTPDatePickerItemList class.
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
#if !defined(_XTPDATEPICKERITEMLIST_H__)
#define _XTPDATEPICKERITEMLIST_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPDatePickerControl;

//==========================================================================
// Summary:
//     This class provides the functionality of a list control which
//     displays month names.
// Remarks:
//     The CXTPDatePickerList provides a functionality of a list control
//     which displays a collection of month names in a pop-up list window,
//     allowing the user to scroll up and down and to choose any specific
//     month for further selecting in the date picker control.
// See Also: CMonthCalCtrl
//===========================================================================
class _XTP_EXT_CLASS CXTPDatePickerList : public CWnd
{
	friend class CXTPDatePickerControl;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDatePickerList object.
	// Parameters:
	//     pControl     - Pointer to the parent CXTPDatePickerControl object
	//     dtStartMonth - A date containing the month which will be created
	//                    as the central item in the list.
	// Remarks:
	//     Construct a CXTPDatePickerList object in two steps.
	//     First, call the constructor CXTPDatePickerList and then
	//     call Create method, which initializes the window.
	// Example:
	// <code>
	// // Declare a dynamic CXTPDatePickerList object.
	// pListControl = new CXTPDatePickerList(pDatePickerWnd, dtMonthToShow);
	//
	// // create control
	// pListControl->Create(rcHeader);
	// </code>
	// See Also: Create
	//-----------------------------------------------------------------------
	CXTPDatePickerList(CXTPDatePickerControl* pControl, COleDateTime dtStartMonth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDatePickerList object,
	//     handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDatePickerList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates the List Window with position and sizes.
	// Parameters:
	//     rcList - Rectangle determining list window position and size.
	// Remarks:
	//     Construct a CXTPDatePickerList object in two steps.
	//     First, call the constructor CXTPDatePickerList and then
	//     call Create method, which initializes the window.
	// Example: For the example, see CXTPDatePickerList::CXTPDatePickerList.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// See Also: CXTPDatePickerList::CXTPDatePickerList
	//-----------------------------------------------------------------------
	BOOL Create(CRect rcList);

// Operations
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Returns an interval between start month and selected month.
	// Remarks:
	//     Call this member function at any time to determine which
	//     month is currently selected in the list.
	// Returns:
	//     A number of months between start month and selected one.
	//-----------------------------------------------------------------------
	int GetMonthInterval();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Implements scroll down functionality.
	// Parameters:
	//     nCount - determines how many positions will be scrolled.
	// Remarks:
	//     Call this member function to scroll the list with months down
	//     by a specified amount of items.
	// See Also: ScrollUp
	//-----------------------------------------------------------------------
	void ScrollDown(int nCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Implements scroll up functionality
	// Parameters:
	//     nCount - Determines how many positions will be scrolled.
	// Remarks:
	//     Call this member function to scroll the list with months up
	//     by a specified amount of items.
	// See Also: ScrollDown
	//-----------------------------------------------------------------------
	void ScrollUp(int nCount = 1);

private:
	static void ShiftMonthUp(int& nYear, int& nMonth, const int nCount);
	static void ShiftMonthDown(int& nYear, int& nMonth, const int nCount);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates size of list by font and control sizes.
	// Returns:
	//     A size of the list to create.
	//-----------------------------------------------------------------------
	CSize GetListSize();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines whether an item is selected.
	// Parameters:
	//     nX      - Item zero-based numeric index from the beginning of the list.
	//     rcItem  - Coordinates of the item to check.
	// Returns:
	//     TRUE if item is selected, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsSelected(int nX, CRect rcItem);

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	// {{AFX_MSG(CXTPDatePickerList)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// }}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	DISABLE_WNDCREATE()

	// Attributes
protected:
	CXTPDatePickerControl* m_pControl;  // Pointer to the parent control.
	COleDateTime m_dtStartMonth;        // The month to start selection from (will be placed in the middle of the list for the beginning).
	COleDateTime m_dtMiddleMonth;       // The month currently in center.
	COleDateTime m_dtSelMonth;          // Currently selected month in the list.

	int m_nItemsAbove;  // How many items are from the top of the middle item on the list.
	int m_nItemsBelow;  // How many items are from the bottom of the middle item on the list.

	CRect m_rcListControl;  // Coordinates of list control in screen coordinates.
};


#endif // !defined(_XTPDATEPICKERITEMLIST_H__)
