// XTPTabToolBar.h : interface for the CXTPTabToolBar class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#if !defined(__XTPTABTOOLBAR_H__)
#define __XTPTABTOOLBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPToolBar.h"
#include "TabManager/XTPTabManager.h"

//===========================================================================
// Summary:
//     CXTPTabToolBar is a CXTPToolBar derived class. It represents tabbable toolbar
//===========================================================================
class _XTP_EXT_CLASS CXTPTabToolBar : public CXTPToolBar, public CXTPTabManager
{
private:
	DECLARE_XTP_COMMANDBAR(CXTPTabToolBar)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTabToolBar object
	//-----------------------------------------------------------------------
	CXTPTabToolBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPTabToolBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPTabToolBar();


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add new tab to CXTPTabToolBar
	// Parameters:
	//     nItem - Index to insert category to
	//     lpszItem - Caption of new tab
	//     pItems - Control items
	//     nCount - Control items count
	//     bLoadIcons - TRUE to load icons
	//     nIDResource - Toolbar resource identifier
	// Returns:
	//     CXTPTabManagerItem class of new tab
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* InsertCategory(int nItem, LPCTSTR lpszItem, UINT* pItems, int nCount, BOOL bLoadIcons = TRUE);
	CXTPTabManagerItem* InsertCategory(int nItem, LPCTSTR lpszItem, UINT nIDResource, BOOL bLoadIcons = TRUE);// <combine CXTPTabToolBar::InsertCategory@int@LPCTSTR@UINT*@int@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to refresh controls on selected tab
	// Parameters:
	//     pItem - Selected item
	//-----------------------------------------------------------------------
	void UpdateTabs(CXTPTabManagerItem* pItem = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a pointer to the tab paint manager.
	//     The tab paint manager is used to customize the appearance of
	//     CXTPTabManagerItem objects and the tab manager.  I.e. Tab colors,
	//     styles, etc...  This member must be overridden in
	//     derived classes.
	// Returns:
	//     Pointer to CXTPTabPaintManager that contains the visual elements
	//     of the tabs.
	//-----------------------------------------------------------------------
	virtual CXTPTabPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set minimum available width of toolbar
	// Parameters:
	//      nMinWidth - Minimum width of toolbar
	//-----------------------------------------------------------------------
	void SetMinimumWidth(int nMinWidth);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to update position of TabManager.
	//-----------------------------------------------------------------------
	void Reposition();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the icon of the tab needs to be
	//     drawn.
	// Parameters:
	//     pDC    - Pointer to the destination device context.
	//     pt     - Specifies the location of the image.
	//     pItem  - CXTPTabManagerItem object to draw icon on.
	//     bDraw  - TRUE if the icon needs to be drawn, I.e. the icon size
	//              changed.  FALSE if the icon does not need to be
	//              drawn or redrawn.
	//     szIcon - Size of the tab icon.
	// Remarks:
	//     For example, on mouse-over.  This member is overridden by its
	//     descendants.  This member must be overridden in
	//     derived classes.
	// Returns:
	//     TRUE if the icon was successfully drawn, FALSE if the icon
	//     was not drawn.
	//-----------------------------------------------------------------------
	BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called when visual properties of the tabs are
	//     changed.  For example, color, mouse-over, and on-click. This member
	//     must be overridden in derived classes.
	// Parameters:
	//     lpRect - The rectangular area of the window that is invalid.
	//     bAnimate - TRUE to animate changes in bounding rectangle.
	//-------------------------------------------------------------------------
	void RedrawControl(LPCRECT lpRect, BOOL bAnimate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select a CXTPTabManagerItem tab.  A selected tab
	//     if the currently active tab.
	// Parameters:
	//     pItem - CXTPTabManagerItem tab to select.
	// Remarks:
	//     This member will will select the specified CXTPTabManagerItem.
	//
	//           SetCurSel will set the current CXTPTabManagerItem by its index
	//           within the collection of items.
	//
	//           A tab is selected when it has focus or is clicked.
	// See Also: CXTPTabManagerItem, GetCurSel, SetCurSel, GetSelectedItem
	//-----------------------------------------------------------------------
	void SetSelectedItem(CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before item click
	// Parameters:
	//     pItem - Pointer to item is about to be clicked
	// Returns:
	//     TRUE if cancel process
	//-----------------------------------------------------------------------
	BOOL OnBeforeItemClick(CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves borders of toolbar
	// Returns:
	//     Borders of toolbar
	//-----------------------------------------------------------------------
	CRect GetBorders();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the command bar in the given context.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rcClipBox - The rectangular area of the control that is invalid
	//-----------------------------------------------------------------------
	void DrawCommandBar(CDC* pDC, CRect rcClipBox);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method calculates the dimensions of a toolbar.
	// Parameters:
	//     nLength - Length of the bar.
	//     dwMode - Mode to dock.
	//     nWidth - Width of the bar.
	// Returns:
	//     Size of the docked toolbar.
	//-----------------------------------------------------------------------
	CSize CalcDockingLayout(int nLength, DWORD dwMode, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method calculates the dimensions of a toolbar.
	// Parameters:
	//     nLength - The requested dimension of the control bar, either horizontal or vertical, depending on dwMode.
	//     dwMode - Mode to dock.
	// Returns:
	//     Size of the docked toolbar.
	//-----------------------------------------------------------------------
	CSize CalcDynamicLayout(int nLength, DWORD dwMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Resets the controls to their original state.
	// Parameters:
	//     bShowWarningMessage - TRUE to show warning message
	//-----------------------------------------------------------------------
	void Reset(BOOL bShowWarningMessage);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when control is dropped to the command bar.
	// Parameters:
	//     pDataObject - Points to a CXTPControl object
	//     dropEffect  - DROPEFFECT enumerator.
	//     ptDrop      - Point where user drops the control.
	//     ptDrag      - Point where user starts drag the control.
	//-----------------------------------------------------------------------
	void OnCustomizeDrop(CXTPControl* pDataObject, DROPEFFECT& dropEffect, CPoint ptDrop, CPoint ptDrag);

protected:

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPTabToolBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTabToolBar)
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void OnSysColorChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

protected:
	CXTPTabPaintManager* m_pTabPaintManager;  // Tab PaintManager
	CRect m_rcTabControl;                     // Bounding rectangle of Tabs
	int m_nMinWidth;            // Minimum available width of toolbar

};

AFX_INLINE CXTPTabPaintManager* CXTPTabToolBar::GetPaintManager() const {
	return m_pTabPaintManager;
}
AFX_INLINE void CXTPTabToolBar::SetMinimumWidth(int nMinWidth) {
	m_nMinWidth = nMinWidth;
}

#endif // #if !defined(__XTPTABTOOLBAR_H__)
