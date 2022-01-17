// XTPReportNavigator.h: interface for the CXTPReportNavigator class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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
#if !defined(__XTPREPORTNAVIGATOR_H__)
#define __XTPREPORTNAVIGATOR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPReportDefines.h"

class CXTPReportControl;
class CXTPReportRow;
class CXTPReportHeader;
class CXTPReportColumn;

//===========================================================================
// Summary:
//     Utility class, handling Report Control items' navigation activities.
// Remarks:
//
// See Also: CXTPReportControl overview
//===========================================================================
class _XTP_EXT_CLASS CXTPReportNavigator : public CXTPCmdTarget
{
	friend class CXTPReportControl;
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default navigator constructor, handles properties initialization.
	// See Also: RefreshMetrics
	//-----------------------------------------------------------------------
	CXTPReportNavigator(CXTPReportControl* pReportControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default navigator destructor, handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportNavigator();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to start edit currently focused item.
	// Remarks:
	//     Call this method to start edit currently focused item.
	//-----------------------------------------------------------------------
	void BeginEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row down.
	// Parameters:
	//     bSelectBlock - TRUE when selecting a new focused row, FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused row down by 1.
	// See Also: MoveUp, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MoveDown(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row up.
	// Parameters:
	//     bSelectBlock     - TRUE when selecting a new focused row, FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused row up by 1.
	// See Also: MoveDown, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MoveUp(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row down at one page.
	// Parameters:
	//     bSelectBlock     - TRUE when selecting rows up to new focused row,
	//                        FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move currently
	//     focused row down by 1 page (an count of rows visible in a view).
	// See Also: MoveDown, MoveUp, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MovePageDown(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row up at one page.
	// Parameters:
	//     bSelectBlock - TRUE when selecting rows up to new focused row,
	//                    FALSE otherwise.
	//     bIgnoreSelection - TRUE to ignore the current rows selection.
	// Remarks:
	//     Call this member function if you want to move currently
	//     row up by 1 page (and count of rows visible in a view).
	// See Also: MoveDown, MoveUp, MovePageDown, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MovePageUp(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row to the beginning of the report view.
	// Parameters:
	//     bSelectBlock     - TRUE when selecting rows up to new focused row,
	//                        FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused row to the top of the rows list.
	// See Also: MoveDown, MoveUp, MovePageDown, MovePageUp, MoveLast
	//-----------------------------------------------------------------------
	void MoveFirstRow(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row to the end of the report view.
	// Parameters:
	//     bSelectBlock     - TRUE when selecting rows up to new focused row,
	//                        FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused row to the bottom of the rows list.
	// See Also: MoveDown, MoveUp, MovePageDown, MovePageUp, MoveFirst
	//-----------------------------------------------------------------------
	void MoveLastRow(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused row to the specified row index.
	// Parameters:
	//     nRowIndex        - Index of the row to move focus.
	//     bSelectBlock     - TRUE when selecting rows up to new focused row,
	//                        FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused row to the the specified row index.
	// See Also: MoveDown, MoveUp, MovePageDown, MovePageUp, MoveFirst
	//-----------------------------------------------------------------------
	void MoveToRow(int nRowIndex, BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused column to the beginning of the row.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused column to the beginning of the currently selected row.
	// See Also: MoveLastColumn
	//-----------------------------------------------------------------------
	void MoveFirstColumn();

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused column to the end of the row.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused column to the end of the currently selected row.
	// See Also: MoveFirstColumn
	//-----------------------------------------------------------------------
	void MoveLastColumn();

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused column left.
	// Parameters:
	//     bSelectBlock - TRUE when selecting a new focused row, FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused column left by 1.
	// See Also: MoveUp, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MoveLeft(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused column left.
	// Parameters:
	//     bSelectBlock     - TRUE when selecting a new focused row, FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused column right by 1.
	// See Also: MoveDown, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MoveRight(BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused column to the specified index.
	// Parameters:
	//     nColumnIndex         - Index of the column to focus.
	//     bClearIfNonFocusable - Used when column the focused row specified
	//                            by nColumnIndex is not focusable:
	//                            if TRUE column focus will be cleared (no
	//                            column focused), otherwise focused column will
	//                            not be changed.
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused column to the specified index.
	// See Also: MoveDown, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MoveToColumn(int nColumnIndex, BOOL bClearIfNonFocusable = FALSE);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Moves the focused column left or right.
	// Parameters:
	//     bSelectBlock     - TRUE when selecting a new focused row, FALSE otherwise.
	//     bIgnoreSelection - TRUE if ignore current rows selection.
	//     bBack - TRUE to move left/ FALSE to move right
	// Remarks:
	//     Call this member function if you want to move the currently
	//     focused column
	// See Also: MoveDown, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void MoveLeftRight(BOOL bBack, BOOL bSelectBlock = FALSE, BOOL bIgnoreSelection = FALSE);

protected:
	CXTPReportControl* m_pReportControl; // Associated report control.

};


#endif //#if !defined(__XTPREPORTNAVIGATOR_H__)
