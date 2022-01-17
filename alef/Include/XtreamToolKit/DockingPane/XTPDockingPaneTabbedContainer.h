// XTPDockingPaneTabbedContainer.h : interface for the CXTPDockingPaneTabbedContainer class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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
#if !defined(__XTPDOCKINGPANETABBEDCONTAINER_H__)
#define __XTPDOCKINGPANETABBEDCONTAINER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPDockingPaneBaseContainer.h"
#include "XTPDockingPanePaintManager.h"
#include "Common/XTPSystemHelpers.h"


class CXTPDockingPaneAutoHidePanel;
class CXTPDockingPaneCaptionButton;

//===========================================================================
// Summary:
//     CXTPDockingPaneTabbedContainer is a multiple inheritance class derived from
//     CWnd and CXTPDockingPaneBase. It represents a tabbed container for Docking
//     Panes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneTabbedContainer : public CWnd, public CXTPDockingPaneBaseContainer, public CXTPTabManager, public CXTPAccessible
{
	DECLARE_DYNAMIC(CXTPDockingPaneTabbedContainer)
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneTabbedContainer object
	//     Protected constructor. Internally constructed only.
	// Parameters:
	//     pLayout - Points to a CXTPDockingPaneLayout object.
	//-----------------------------------------------------------------------
	CXTPDockingPaneTabbedContainer(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneTabbedContainer object, handles cleanup and
	//     deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPaneTabbedContainer();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to activate a child docking pane.
	// Parameters:
	//     pPane     - Pane to be activated.
	//     bSetFocus - TRUE to set focus to child docking pane.
	//     bDelayRedraw - TRUE to redraw caption delayed.
	//-----------------------------------------------------------------------
	virtual void SelectPane(CXTPDockingPane* pPane, BOOL bSetFocus = TRUE, BOOL bDelayRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get child pane state.
	// Returns:
	//     TRUE if a child pane is active.
	//-----------------------------------------------------------------------
	BOOL IsActive() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get selected pane.
	// Returns:
	//     The selected child pane.
	//-----------------------------------------------------------------------
	CXTPDockingPane* GetSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to access title's close button.
	// Returns:
	//     A reference to the title's close button.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton* GetCloseButton() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to access title's pin button.
	// Returns:
	//     A reference to the title's pin button.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton* GetPinButton() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the title is visible.
	// Returns:
	//     TRUE if the title is visible.
	//-----------------------------------------------------------------------
	BOOL IsTitleVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if caption drawn vertically
	// Returns:
	//     TRUE if the caption drawn vertically
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionVertical() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the tab bar is visible.
	// Returns:
	//     TRUE if the tab bar is visible.
	//-----------------------------------------------------------------------
	virtual BOOL IsTabsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves safe window handle.
	// Returns: Safe window handle.
	//-----------------------------------------------------------------------
	virtual HWND GetPaneHwnd() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the tabbed container is
	//     auto-hidden.  If one pane in a group of panes is auto-hidden,
	//     then the entire group will be auto-hidden.
	// Returns:
	//     TRUE if the tabbed container is auto-hidden, FALSE if the
	//     tab group is visible.
	//-----------------------------------------------------------------------
	BOOL IsHidden() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called by the framework when the left mouse
	//     button is clicked while the mouse pointer is positioned over
	//     a pane caption.  This will give the pane focus and begin the
	//     pane dragging process (If the left button is held down).
	// Parameters:
	//     point - Cursor location on the pane's caption that was clicked.
	//-----------------------------------------------------------------------
	virtual void OnCaptionLButtonDown(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to check if docking pane can be attached to tabbed container.
	// Parameters:
	//     rcClient - Client rectangle of container.
	//     pt       - Point to attach.
	// Returns:
	//     TRUE if pane can be attached as new tab to tabbed container.
	//-----------------------------------------------------------------------
	virtual BOOL CanAttach(CRect& rcClient, CPoint pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to fill the pMinMaxInfo structure with the
	//     minimum and maximum width and height of the container.
	//     This will look at the MinMaxInfo for each pane in the
	//     container and set the appropriate minimum and maximum width
	//     and height for the container.
	// Parameters:
	//     pMinMaxInfo - Pointer to MINMAXINFO structure
	//-----------------------------------------------------------------------
	void GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get pane by its index.
	// Parameters:
	//     nIndex - Index of pane need to retrieve
	// Returns:
	//     CXTPDockingPane pointer corresponded with nIndex
	//-----------------------------------------------------------------------
	CXTPDockingPane* GetItemPane(int nIndex) const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to determine if a specific caption button
	//     is visible.
	// Parameters:
	//     pButton - Button to check to see if it is visible.
	// Returns:
	//     TRUE if the caption button specified is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionButtonVisible(CXTPDockingPaneCaptionButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called by the framework when a caption button
	//     is clicked.
	// Parameters:
	//     pButton - Caption button that was clicked.
	//-----------------------------------------------------------------------
	virtual void OnCaptionButtonClick(CXTPDockingPaneCaptionButton* pButton);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneTabbedContainer object
	//     Protected constructor. Internally constructed only.
	// Parameters:
	//     pPane   - Child pane.
	//     pFrame  - Parent Frame.
	//     pLayout - Points to a CXTPDockingPaneLayout object.
	//-----------------------------------------------------------------------
	void Init(CXTPDockingPane* pPane, CWnd* pFrame);

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies a CXTPDockingPaneTabbedContainer object
	// Parameters:
	//     pClone  - Points to a CXTPDockingPaneTabbedContainer object
	//     pMap    - Points to a CXTPPaneToPaneMap object
	//     dwIgnoredOptions - Options that must be skipped.
	// See Also:
	//     CXTPPaneToPaneMap
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPDockingPaneTabbedContainer* pClone, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to recalculate the positions of
	//     tab groups/Items.
	//-----------------------------------------------------------------------
	virtual void Reposition();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to redraw the tabbed control, I.e. When
	//     an Icon has changed.
	// Parameters:
	//     lpRect - The rectangular area of the window that is invalid.
	//     bAnimate - TRUE to animate changes in bounding rectangle.
	//-----------------------------------------------------------------------
	virtual void RedrawControl(LPCRECT lpRect, BOOL bAnimate);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to access the visual elements of the docking
	//     pane tabs.  I.e. Tab colors, styles, etc...
	// Returns:
	//     Pointer to CXTPTabPaintManager that contains the visual elements
	//     of the docking pane tabs.
	//-----------------------------------------------------------------------
	virtual CXTPTabPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called in a derived class to set a pointer
	//     the tab paint manager.
	// Parameters:
	//     pPaintManager - Pointer to a CXTPTabPaintManager object.
	//-----------------------------------------------------------------------
	virtual void SetPaintManager(CXTPTabPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the icon of the tab in the tabbed
	//     container needs to be drawn.  I.e. On mouse over.
	// Parameters:
	//     pDC    - Pointer to the destination device context.
	//     pt     - Specifies the location of the image.
	//     pItem  - CXTPTabManagerItem object to draw icon on.
	//     bDraw  - TRUE if the icon needs to be drawn, I.e. the icon size
	//                 changed.  FALSE if the icon does not need to be
	//                 drawn or redrawn.
	//     szIcon - Size of the tab icon.
	// Returns:
	//     TRUE if the icon was successfully drawn, FALSE if the icon
	//     was not drawn.
	//-----------------------------------------------------------------------
	virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to save/restore the settings of the pane.
	// Parameters:
	//     pPX - Points to a CXTPPropExchange object.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to when tabs was changed.
	//-----------------------------------------------------------------------
	virtual void OnTabsChanged();

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual void OnSizeParent(CWnd* pParent, CRect rect, LPVOID lParam);
	virtual void OnFocusChanged();
	virtual void SetDockingSite(CWnd* pFrame);
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	virtual void OnParentContainerChanged(CXTPDockingPaneBase* pContainer);
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	void ShowTitle(BOOL bShow);
	CString GetTitle() const;
	void Show(BOOL bSetFocus);      // ksa
	void _Swap(CXTPDockingPane* p1, CXTPDockingPane* p2);
	void _InsertPane(CXTPDockingPane* pPane, BOOL bSetFocus = TRUE);
	int HitTest(CPoint point) const;
	void AdjustMinMaxInfoClientRect(LPMINMAXINFO pMinMaxInfo, BOOL bCaptionOnly = FALSE) const;

	void InvalidatePane(BOOL bSelectionChanged);
	void _RestoreFocus();
	BOOL OnCaptionButtonDown(CXTPDockingPaneCaptionButton* pButton);
	CXTPDockingPaneCaptionButton* HitTestCaptionButton(CPoint point) const;
	void DeletePane();
	CXTPTabManagerItem* GetPaneTab(CXTPDockingPane* pPane) const;


	BOOL IsPaneRestored() const;
	BOOL IsAllowMaximize() const;
	BOOL IsPaneMaximized() const;
	BOOL IsPaneMinimized() const;
	void Restore();
	void Maximize();
	void NormalizeDockingSize();
//}}AFX_CODEJOCK_PRIVATE


protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPDockingPaneTabbedContainer)
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	LRESULT OnHelpHitTest(WPARAM, LPARAM);
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPDockingPaneTabbedContainer)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
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
	BOOL m_bActive;
	BOOL m_bTitleVisible;
	BOOL m_bLockReposition;
	BOOL m_bDelayRedraw;
	CXTPDockingPane*      m_pSelectedPane;
	CXTPDockingPane*      m_pTrackingPane;
	CArray<CRect, CRect&> m_lstRects;
	COleDropTarget* m_pDropTarget;
	BOOL m_bMaximized;


private:
	class CContainerDropTarget;

private:
	friend class CXTPDockingPaneManager;
	friend class CXTPDockingPane;
	friend class CXTPDockingPaneContext;
	friend class CXTPDockingPaneMiniWnd;
	friend class CXTPDockingPaneLayout;
	friend class CXTPDockingPaneAutoHidePanel;
	friend class CXTPDockingPaneAutoHideWnd;
	friend class CXTPDockingPaneCaptionButton;
	friend class CContainerDropTarget;
};

AFX_INLINE BOOL CXTPDockingPaneTabbedContainer::IsActive() const {
	return m_bActive;
}
AFX_INLINE CXTPDockingPane* CXTPDockingPaneTabbedContainer::GetSelected() const {
	return m_pSelectedPane;
}
AFX_INLINE HWND CXTPDockingPaneTabbedContainer::GetPaneHwnd() const {
	return CWnd::GetSafeHwnd();
}

#endif // #if !defined(__XTPDOCKINGPANETABBEDCONTAINER_H__)
