// XTPPropertyGridView.h interface for the CXTPPropertyGridView class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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
#if !defined(__XTPPROPERTYGRIDVIEW_H__)
#define __XTPPROPERTYGRIDVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPPropertyGridPaintManager.h"
#include "Common/XTPSystemHelpers.h"

class CXTPPropertyGridView;
class CXTPPropertyGridPaintManager;
class CXTPPropertyGrid;
class CXTPImageManager;

//===========================================================================
// Summary:
//     CXTPPropertyGridToolTip is a CWnd derived class.
//     It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridToolTip : public CWnd
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridToolTip object
	//-----------------------------------------------------------------------
	CXTPPropertyGridToolTip();

	//-----------------------------------------------------------------------
	// Summary:
	//     Activate Tooltip control.
	// Parameters:
	//     bActive - TRUE to activate.
	//     pItem   - Item which tooltip to show
	//     bValuePart - TRUE if tooltip of value part will be visible
	//-----------------------------------------------------------------------
	void Activate(BOOL bActive, CXTPPropertyGridItem* pItem, BOOL bValuePart);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates Tooltip control.
	// Parameters:
	//     pParentWnd - Points to a CXTPPropertyGridView object.
	//-----------------------------------------------------------------------
	void Create(CXTPPropertyGridView* pParentWnd);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPPropertyGridView)
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	CXTPPropertyGridView* m_pGrid;      // Parent window.
	CFont m_fnt;                        // Font used to display the tool tip

	friend class CXTPPropertyGridView;
};

AFX_INLINE BOOL CXTPPropertyGridToolTip::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


//===========================================================================
// Summary:
//     CXTPPropertyGridView is a CListBox derived class.
//     It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridView : public CListBox, public CXTPAccessible
{
	struct WNDRECT;
	DECLARE_DYNAMIC(CXTPPropertyGridView)

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridView object
	//-----------------------------------------------------------------------
	CXTPPropertyGridView();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridView object, handles cleanup and
	//     deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridView();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the position of the divider.
	// Returns:
	//     Position of the divider
	//-----------------------------------------------------------------------
	int GetDividerPos() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the position of the divider.
	// Parameters:
	//     nDivider - Position of the divider
	//-----------------------------------------------------------------------
	void SetDividerPos(int nDivider);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to disable auto calculating divider position
	//-----------------------------------------------------------------------
	void LockDivider();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines where a point lies in a specified item.
	// Parameters:
	//     point - Specifies the point to be tested.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* ItemFromPoint(CPoint point) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to refresh items in the list.
	//-----------------------------------------------------------------------
	void Refresh();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all items in the list.
	//-----------------------------------------------------------------------
	void ResetContent();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves an item with the specified index.
	// Parameters:
	//     nIndex - Specifies the zero-based index of the item to retrieve.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetItem(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a new category to a Property
	//     Grid Control.
	// Parameters:
	//     strCaption - Name of the category to add.
	//     pCategory  - Points to a CXTPPropertyGridItem object.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* AddCategory(LPCTSTR strCaption, CXTPPropertyGridItem* pCategory = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to insert a new category to the Property Grid control.
	// Parameters:
	//     strCaption - Name of the category to add.
	//     nIndex     - Index of category to be inserted
	//     pCategory  - Points to a CXTPPropertyGridItem object.
	// Returns:
	//     The pointer to the item object of the newly inserted category.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* InsertCategory(int nIndex, LPCTSTR strCaption, CXTPPropertyGridItem* pCategory = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the currently used control's Paint Manager.
	// Remarks:
	//     Call this member function to get the paint manager object used
	//     for drawing a property grid control window.
	// Returns:
	//     Pointer to the paint manager object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves Image manager associated with property grid
	// Returns:
	//     Pointer to the image manager object that stores the images in
	//     the property grid.
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sends the specified message to the owner window.
	// Parameters:
	//     wParam - Specifies additional message-dependent information.
	//     lParam - Specifies additional message-dependent information
	// Returns:
	//     The result of the message processing; its value depends on the message sent.
	//-----------------------------------------------------------------------
	LRESULT SendNotifyMessage(WPARAM wParam = 0, LPARAM lParam = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get parent property grid class
	// Returns:
	//     Returns parent CXTPPropertyGrid object.
	//-----------------------------------------------------------------------
	CXTPPropertyGrid* GetPropertyGrid() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to move focus to in-place button
	// Parameters:
	//     pButton - Button to move focus
	//-----------------------------------------------------------------------
	void FocusInplaceButton(CXTPPropertyGridInplaceButton* pButton);

protected:


//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPPropertyGridView)
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT) { return 0;}
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPPropertyGridView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelectionChanged();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCntr, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus (CWnd* pNewWnd);
	afx_msg LRESULT OnGetObject(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	// System accessibility support.
	virtual HRESULT GetAccessibleParent(IDispatch** ppdispParent);
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual CCmdTarget* GetAccessible();
	DECLARE_INTERFACE_MAP()
//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPPropertyGridItem* GetSelectedItem();
	void SetPropertySort(XTPPropertyGridSortOrder sort, BOOL bRrefresh = FALSE, BOOL bSetRedraw = TRUE);
	int InsertItem(CXTPPropertyGridItem* pItem, int nIndex);
	int _DoExpand(CXTPPropertyGridItem* pItem, int nIndex);
	void _DoCollapse(CXTPPropertyGridItem* pItem);
	void _RefreshIndexes();
	void SwitchExpandState(int nItem);
	int HitTest(CPoint point) const;

	void ShowToolTip(CPoint pt);
	void _ShowToolTip(CRect rcBound, CRect rcText, CXTPPropertyGridItem* pItem, BOOL bValuePart);
	CSize _GetTextExtent(const CString& str, CXTPPropertyGridItem* pItem, BOOL bValuePart);
	void RelayToolTipEvent(UINT message);
	LRESULT OnScrollControl();

protected:
	CXTPPropertyGrid*             m_pGrid;              // Parent CXTPPropertyGrid class.
	BOOL                          m_bTracking;          // TRUE if grid in tracking mode.
	CRect                         m_rcToolTip;          // Position of the Tooltip.
	double                        m_dDivider;           // Divider position.
	BOOL                          m_bAutoDivider;       // TRUE to auto calculate divider
	HCURSOR                       m_hCursor;            // Handle of the cursor.
	CString                       m_strTipText;         // Tooltip text.
	XTPPropertyGridSortOrder      m_properetySort;      // Current sort order.
	CXTPPropertyGridItem*         m_pSelected;          // Current selected item.
	CXTPPropertyGridItems*        m_pCategories;        // Collection of the categories.
	CXTPPropertyGridToolTip       m_wndTip;             // Tooltip control.
	int                           m_nLockUpdate;        // TRUE to lock update.
	UINT                          m_nItemHeight;        // Default height of the item
	BOOL                          m_bVariableSplitterPos;   // TRUE to allow the user to resize the splitter with the mouse.

	CXTPPropertyGridInplaceButton* m_pFocusedButton;    // Currently focused button
	CXTPPropertyGridInplaceButton* m_pHotButton;        // Currently highlighted button

private:

	friend class CXTPPropertyGridItem;
	friend class CXTPPropertyGridItems;
	friend class CXTPPropertyGrid;
	friend class CXTPPropertyGridInplaceEdit;
	friend class CPropertyGridCtrl;
	friend class CXTPPropertyGridToolTip;
	friend class CXTPPropertyGridInplaceButton;
};

AFX_INLINE CXTPPropertyGrid* CXTPPropertyGridView::GetPropertyGrid() const {
	return  m_pGrid;
}

#endif // #if !defined(__XTPPROPERTYGRIDVIEW_H__)
