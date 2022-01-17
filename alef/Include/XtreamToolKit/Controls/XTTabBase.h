// XTTabBase.h : header file
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#ifndef __XTTABCTRLBASE_H__
#define __XTTABCTRLBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXTTabCtrlButtons;

#include "XTTabBaseTheme.h"
#include "XTThemeManager.h"

// ----------------------------------------------------------------------
// Summary:
//     Enumeration used to determine navigation button display for a
//     CXTTabCtrl object.
// Remarks:
//     XTNavBtnState type defines the constants used by the
//     CXTTabCtrl class to determine which navigation buttons to display.
// See Also:
//     CXTTabCtrl
//
// <KEYWORDS xtNavBtnArrows, xtNavBtnClose, xtNavBtnShowAll>
// ----------------------------------------------------------------------
enum XTNavBtnState
{
	xtNavBtnArrows  = 1, // Display arrow buttons.
	xtNavBtnClose   = 2, // Display close button.
	xtNavBtnShowAll = 3  // Display arrow and close buttons.
};

//===========================================================================
// Summary:
//     CXTTabBase is a stand alone base class. It is used to draw an XP
//     style tab control.
//===========================================================================
class _XTP_EXT_CLASS CXTTabBase : public CXTThemeManagerStyleHost
{
	DECLARE_THEME_HOST(CXTTabBase)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTTabBase object
	//-----------------------------------------------------------------------
	CXTTabBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTTabBase object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTTabBase();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a pointer to the associated tab control.
	// Remarks:
	//     This member function is called by the base class to manage a
	//     pointer for the derived class CTabCtrl object.
	// Returns:
	//     A pointer to a CTabCtrl object if successful, otherwise NULL.
	//-----------------------------------------------------------------------
	CTabCtrl* GetTabCtrlImpl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a pointer to the tab control navigation button object.
	// Remarks:
	//     This member function is called to retrieve a to the tab control
	//     buttons used for navigating and closing tab windows.
	// Returns:
	//     A pointer to a CXTTabCtrlButtons object if successful, otherwise NULL.
	//-----------------------------------------------------------------------
	CXTTabCtrlButtons* GetButtons();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a pointer to the tab controls paint manager.
	// Remarks:
	//     This member function is called to return a pointer to the tab control
	//     paint manager. The paint manager is responsible for the visual appearance
	//     of the tab control.
	// Returns:
	//     A pointer to a CXTTabBaseTheme object if successful, otherwise NULL.
	//-----------------------------------------------------------------------
	CXTTabBaseTheme* GetPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTTabBase class to
	//     perform initialization when the window is created or sub-classed.
	// Returns:
	//     TRUE if the window was successfully initialized, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual bool Init();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function copies the child coordinates of the CTabCtrl client
	//     area into the object referenced by 'rcChild'. The client coordinates
	//     specify the upper-left and lower-right corners of the client area.
	// Parameters:
	//     rcChild - A reference to a CRect object to receive the client coordinates.
	//-----------------------------------------------------------------------
	virtual void GetChildRect(CRect& rcChild) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set visibility of the navigation buttons. These
	//     buttons are used in place of the default forward and back buttons that are
	//     displayed when the tab control is not wide enough to display all tabs. You can
	//     also define a close button to be used to close the active tab. This will give
	//     the tab control a VS.NET style tabbed interface.
	// Parameters:
	//     dwFlags - The value can be one or more of the values listed in the Remarks section.
	// Remarks:
	//     Styles to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:
	//          * <b>xtNavBtnArrows</b> To show arrow buttons.
	//          * <b>xtNavBtnClose</b> To show close button.
	//          * <b>xtNavBtnShowAll</b> To show arrow and close buttons.
	//-----------------------------------------------------------------------
	void ShowNavButtons(DWORD dwFlags);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to associate the tab control with this object.
	// Parameters:
	//     pTabCtrl - Points to a valid tab control object.
	//-----------------------------------------------------------------------
	void ImplAttach(CTabCtrl* pTabCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the tab control to add padding to a
	//     tab label for use with XP style tabs.
	// Parameters:
	//     strLabelText - Tab label to add padding to.
	//-----------------------------------------------------------------------
	virtual void OnAddPadding(CXTStringHelper& strLabelText);

public:
	bool m_bBoldFont;       // true to set the selected tab font to bold.
	bool m_bXPBorder;       // true to draw an XP border around the tab child window.
	BOOL m_bAutoCondensing; // TRUE for auto-condensing tabs.

protected:
	bool               m_bPreSubclassInit; // true when initializing from PreSubclassWindow.
	CTabCtrl*          m_pTabCtrl;         // Pointer to the tab control associated with this object.
	CXTTabCtrlButtons* m_pNavBtns;         // Arrow buttons.

//{{AFX_CODEJOCK_PRIVATE
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSysColorChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//}}AFX_CODEJOCK_PRIVATE
};

//---------------------------------------------------------------------------

AFX_INLINE CTabCtrl* CXTTabBase::GetTabCtrlImpl() {
	return m_pTabCtrl;
}
AFX_INLINE CXTTabCtrlButtons* CXTTabBase::GetButtons() {
	return m_pNavBtns;
}

//===========================================================================

//{{AFX_CODEJOCK_PRIVATE
#define DECLATE_TABCTRL_BASE(ClassName, Tab, Base)\
class _XTP_EXT_CLASS ClassName : public Tab, public Base\
{\
protected:\
	virtual void PreSubclassWindow() {\
		Tab::PreSubclassWindow();\
		Base::PreSubclassWindow();\
	}   \
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) {\
		if (!Tab::PreCreateWindow(cs))\
			return FALSE;\
		return Base::PreCreateWindow(cs);\
	}   \
	afx_msg void OnPaint() {\
		Base::OnPaint();\
	}   \
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) {\
		return Base::OnEraseBkgnd(pDC);\
	}   \
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection) {\
		Base::OnSettingChange(uFlags, lpszSection);\
	}   \
	afx_msg void OnSysColorChange() {\
		Base::OnSysColorChange();\
	}   \
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) {\
		if (Tab::OnCreate(lpCreateStruct) == -1)\
			return -1;\
		return Base::OnCreate(lpCreateStruct);\
	}   \
};

#define ON_TABCTRL_REFLECT \
	ON_WM_PAINT() \
	ON_WM_ERASEBKGND() \
	ON_WM_SETTINGCHANGE() \
	ON_WM_SYSCOLORCHANGE() \
	ON_WM_CREATE
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTTabExBase is a stand alone base class. It is used to draw an XP
//     style tab control.
//===========================================================================
class _XTP_EXT_CLASS CXTTabExBase : public CXTTabBase
{
private:
	struct ITEMANDWIDTH;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTTabExBase object
	//-----------------------------------------------------------------------
	CXTTabExBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTTabExBase object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTTabExBase();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to initialize the font for the tab control
	//     associated with this view.
	//-----------------------------------------------------------------------
	virtual void InitializeFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the handle of the tooltip control associated
	//     with the tab control. The tab control creates a tooltip control if
	//     it has the TCS_TOOLTIPS style. You can also assign a tooltip control
	//     to a tab control by using the SetToolTips member function.
	// Returns:
	//     The handle of the tooltip control if successful; otherwise returns NULL.
	//-----------------------------------------------------------------------
	virtual CToolTipCtrl* GetTips();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to assign a tooltip control to the tab control.
	//     You can associate the tooltip control with a tab control by making
	//     a call to GetToolTips.
	// Parameters:
	//     pWndTip - Pointer to a tooltip control.
	//-----------------------------------------------------------------------
	virtual void SetTips(CToolTipCtrl* pWndTip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to register a tab with the tooltip control so
	//     that the information stored in the tooltip is displayed when the cursor
	//     is on the tab.
	// Parameters:
	//     nIDTab   - Index of the tab.
	//     lpszText - Pointer to the text for the tool.
	//-----------------------------------------------------------------------
	virtual void AddToolTip(UINT nIDTab, LPCTSTR lpszText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to update the tooltip text for the specified tab.
	// Parameters:
	//     nIDTab - Index of the tab.
	//     pViewClass - CRuntimeClass associated with the tab.
	//     lpszText - Pointer to the text for the tool.
	//-----------------------------------------------------------------------
	virtual void UpdateToolTip(int nIDTab, LPCTSTR lpszText);
	virtual void UpdateToolTip(CRuntimeClass* pViewClass, LPCTSTR lpszText); // <combine CXTTabExBase::UpdateToolTip@int@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to reset the values for the tooltip
	//     control based upon the information stored for each tab.
	//-----------------------------------------------------------------------
	virtual void ResetToolTips();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to enable or disable tooltip usage.
	// Parameters:
	//     bEnable - TRUE to enable tooltip usage.
	// Returns:
	//     TRUE if the tooltip control was found and updated, otherwise returns
	//     FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL EnableToolTipsEx(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a view to the tab control associated
	//     with this view.
	// Parameters:
	//     lpszLabel  - Pointer to the text for the tab associated with the view.
	//     pView      - An existing view to be added to the tab control.
	//     pViewClass - CView runtime class associated with the tab.
	//     pDoc       - CDocument associated with the view.
	//     pContext   - Create context for the view.
	//     iIndex     - -1 to add to the end.
	//     iIconIndex - Icon index for the tab. If -1, 'iIndex' is used to determine the index.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL AddView(LPCTSTR lpszLabel, CView* pView, int iIndex = -1, int iIconIndex = -1);
	virtual BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass* pViewClass, CDocument* pDoc = NULL, CCreateContext* pContext = NULL, int iIndex = -1, int iIconIndex = -1); // <combine CXTTabExBase::AddView@LPCTSTR@CView*@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to add a control to the tab control
	//     associated with this view.
	// Parameters:
	//     lpszLabel  - Pointer to the text for the tab associated with the view.
	//     pWnd       - CWnd object associated with the tab.
	//     iIndex     - Tab index of where to insert the new view. Default is -1 to add to
	//                  the end.
	//     iIconIndex - Icon index for the tab. If -1, 'iIndex' is used to determine the index.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL AddControl(LPCTSTR lpszLabel, CWnd* pWnd, int iIndex = -1, int iIconIndex = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns a pointer to a view from the specified
	//     runtime class.
	// Parameters:
	//     nView - Tab index.
	//     pViewClass - CView runtime class associated with the tab.
	// Returns:
	//     A pointer to a CView object, otherwise returns NULL.
	//-----------------------------------------------------------------------
	virtual CWnd* GetView(int nView);
	virtual CWnd* GetView(CRuntimeClass* pViewClass); // <combine CXTTabExBase::GetView@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns a pointer to the active view associated
	//     with the selected tab.
	// Returns:
	//     A pointer to the active view, otherwise returns NULL.
	//-----------------------------------------------------------------------
	virtual CWnd* GetActiveView();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to activate the specified view and deactivate
	//     all remaining views.
	// Parameters:
	//     pTabView - CWnd object to make active.
	//-----------------------------------------------------------------------
	virtual void ActivateView(CWnd* pTabView);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will set a view active based on the specified
	//     runtime class.
	// Parameters:
	//     nActiveTab - Tab index.
	//     pTabView - CWnd object to make active.
	//     pViewClass - CView runtime class associated with the tab.
	//-----------------------------------------------------------------------
	virtual void SetActiveView(int nActiveTab);
	virtual void SetActiveView(CWnd* pTabView); // <combine CXTTabExBase::SetActiveView@int>
	virtual void SetActiveView(CRuntimeClass* pViewClass); // <combine CXTTabExBase::SetActiveView@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will remove a view based on the specified
	//     tab index.
	// Parameters:
	//     nView - Tab index of the view.
	//     pView       - Points to the CWnd object associated with the tab.
	//     pViewClass - CView runtime class associated with the tab.
	//     bDestroyWnd - TRUE to destroy the list item.
	//-----------------------------------------------------------------------
	virtual void DeleteView(int nView, BOOL bDestroyWnd = TRUE);
	virtual void DeleteView(CWnd* pView, BOOL bDestroyWnd = TRUE); // <combine CXTTabExBase::DeleteView@int@BOOL>
	virtual void DeleteView(CRuntimeClass* pViewClass, BOOL bDestroyWnd = TRUE); // <combine CXTTabExBase::DeleteView@int@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will return the name for a view based on the tab
	//     index.
	// Parameters:
	//     nView - Tab index of the view.
	//     pViewClass - CView runtime class associated with the tab.
	// Returns:
	//     A NULL terminated string that represents the tab item text.
	//-----------------------------------------------------------------------
	virtual LPCTSTR GetViewName(int nView);
	virtual LPCTSTR GetViewName(CRuntimeClass* pViewClass); // <combine CXTTabExBase::GetViewName@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called when the tab control is resized.
	//     It is responsible for updating internal structures that are
	//     dependent on the control's size
	//-----------------------------------------------------------------------
	virtual void RecalcLayout();

	//-----------------------------------------------------------------------
	// Summary:
	//     Helper methods
	// Parameters:
	//     hDWP  - Handle to a multiple-window - position structure that contains
	//             size and position information for one or more windows.
	//     pView - A pointer to a CWnd object to be resized.
	//-----------------------------------------------------------------------
	virtual BOOL Defer(HDWP& hDWP, CWnd* pView);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to resize the tab view specified by 'pView'.
	// Parameters:
	//     pView - A pointer to a CWnd object to be resized.
	//-----------------------------------------------------------------------
	virtual void ResizeTabView(CWnd* pView);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used by the tab control bar to remove an item
	//     from the tab view list.
	// Parameters:
	//     pos         - The POSITION value of the item to be removed.
	//     bDestroyWnd - TRUE to destroy the list item.
	//-----------------------------------------------------------------------
	virtual void RemoveListItem(POSITION pos, BOOL bDestroyWnd = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove all the tabs, including all associated
	//     views.
	// Parameters:
	//     bDestroyWnd - TRUE to destroy the window associated with the tab item.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL RemoveAllTabs(BOOL bDestroyWnd = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the tab index from the current cursor
	//     position.
	// Parameters:
	//     point - Pointer to a CPoint object that contains the cursor screen coordinates.
	//             Use default for current cursor position.
	// Returns:
	//     An integer based index of the tab; or -1, if no tab is at the specified 'point'.
	//-----------------------------------------------------------------------
	virtual int GetTabFromPoint(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to see if the specified CWnd object is a
	//     child of the tab control.
	// Parameters:
	//     pView - A pointer to a CWnd object.
	// Returns:
	//     TRUE if the specified CWnd object is a child of the tab control, otherwise
	//     returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsChildView(CWnd* pView);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the text for the specified tab.
	// Parameters:
	//     nTab      - Index of the tab.
	//     pView     - CWnd object associated with the tab.
	//     pViewClass - CRuntimeClass of the CWnd associated with the tab.
	//     lpszLabel - New text for the tab label.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetTabText(int nTab, LPCTSTR lpszLabel);
	BOOL SetTabText(CWnd* pView, LPCTSTR lpszLabel); // <combine CXTTabExBase::SetTabText@int@LPCTSTR>
	BOOL SetTabText(CRuntimeClass* pViewClass, LPCTSTR lpszLabel); // <combine CXTTabExBase::SetTabText@int@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to activate the next view in the tab
	//     control.
	// Returns:
	//     A CWnd pointer to the newly activated view.
	//-----------------------------------------------------------------------
	CWnd* NextView();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to activate the previous view in the
	//     tab control.
	// Returns:
	//     A CWnd pointer to the newly activated view.
	//-----------------------------------------------------------------------
	CWnd* PrevView();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to enable or disable the tab auto-condensing
	//     mode. Auto-condensing mode affects the tab control's behavior when
	//     there is not enough room to fit all tabs. Without auto-condensation,
	//     the CXTTabCtrl control behaves like a standard tab control (i.e. it
	//     will display a slider control that allows the user to pan between tabs).
	//     With the auto-condensing mode enabled, CXTTabCtrl attempts to fit all
	//     tabs in the available space by trimming the tab label text. This behavior
	//     is similar to the behavior displayed by Visual C++'s Workspace View.
	//     For instance, you can see the FileView tab shrink if you shrink the
	//     Workspace View.
	// Parameters:
	//     bEnable - TRUE to enable auto-condense mode.
	//-----------------------------------------------------------------------
	void SetAutoCondense(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the state of the tab control's auto-condense
	//     mode. See SetAutoCondense() for a full explanation of this mode.
	// Returns:
	//     TRUE if auto-condense is enabled, or FALSE if it is disabled.
	//-----------------------------------------------------------------------
	BOOL GetAutoCondense();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will modify the style for the tab control associated
	//     with this view and set the appropriate font depending on the tab's
	//     orientation.
	// Parameters:
	//     dwRemove - Specifies window styles to be removed during style modification.
	//     dwAdd    - Specifies window styles to be added during style modification.
	//     nFlags   - Flags to be passed to SetWindowPos, or zero if SetWindowPos should
	//                not be called. The default is zero. See CWnd::ModifyStyle for more
	//                details.
	// Returns:
	//     Nonzero if the style was successfully modified, otherwise returns zero.
	//-----------------------------------------------------------------------
	virtual BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get the last known view that belongs
	//     to the frame.
	// Returns:
	//     A CView pointer to the last known view.
	//-----------------------------------------------------------------------
	CView* GetLastKnownChildView();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the resource ID for the popup menu
	//     used by the tab control.
	// Parameters:
	//     popupMenuID - ID for the tab control popup menu.
	//     nPos        - Index position in the menu resource.
	//-----------------------------------------------------------------------
	virtual void SetMenuID(UINT popupMenuID, int nPos = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the menu resource associated with the
	//     tab control.
	// Returns:
	//     The resource ID of the menu associated with the tab control.
	//-----------------------------------------------------------------------
	virtual UINT GetMenuID();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to allow WM_INITIALUPATE message to be sent
	//     to views after creation.
	// Parameters:
	//     bInitialUpdate - TRUE to send initial update message.
	//-----------------------------------------------------------------------
	virtual void SendInitialUpdate(BOOL bInitialUpdate);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called whenever the theme has changed.
	// See Also:
	//      CXTThemeManagerStyleHost::OnThemeChanged(), CXTHeaderCtrl::OnThemeChanged()
	//-----------------------------------------------------------------------
	virtual void OnThemeChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is a virtual method that is called to handle a
	//     TCN_SELCHANGING event. Override in your derived class to add additional
	//     functionality.
	//-----------------------------------------------------------------------
	virtual void OnSelChanging();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is a virtual method that is called to handle a
	//     TCN_SELCHANGE event. Override in your derived class to add additional
	//     functionality.
	//-----------------------------------------------------------------------
	virtual void OnSelChange();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the tooltip and tab text for
	//     the specified tab.
	// Parameters:
	//     nTab      - Index of the tab.
	//     pMember   - Address of an CXTTcbItem struct associated with the tab.
	//     lpszLabel - NULL terminated string that represents the new tab label.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL UpdateTabLabel(int nTab, CXTTcbItem* pMember, LPCTSTR lpszLabel);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function creates the CWnd object that is associated
	//     with a tab control item.
	// Parameters:
	//     pViewClass - CView runtime class to be created.
	//     pDocument  - CDocument associated with view.
	//     pContext   - Create context for the view.
	// Returns:
	//     A pointer to the newly created CWnd object, otherwise returns NULL.
	//-----------------------------------------------------------------------
	virtual CWnd* CreateTabView(CRuntimeClass *pViewClass, CDocument *pDocument, CCreateContext* pContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used internally by the tab control to calculate
	//     the width of a tab based on its label text.
	// Parameters:
	//     pDC      - Points to the current device context.
	//     sLabel   - Represents the tab label text.
	//     bHasIcon - Set to true if the tab item has an icon.
	// Returns:
	//     An integer value that represents the width of a tab.
	//-----------------------------------------------------------------------
	int CalculateTabWidth(CDC* pDC, CString& sLabel, bool bHasIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used internally by the tab control to shrink,
	//     or un-shrink, tabs based on the control's width and the state of the
	//     auto-condensation mode. See SetAutoCondense() for more information.
	//-----------------------------------------------------------------------
	void Condense();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTTabExBase class to
	//     perform initialization when the window is created or sub-classed.
	// Returns:
	//     TRUE if the window was successfully initialized, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual bool Init();

//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnPreWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	virtual void OnPostWindowPosChanged();
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
//}}AFX_CODEJOCK_PRIVATE

public:
	CList <CXTTcbItem*, CXTTcbItem*> m_tcbItems; // Template list containing tab information.

protected:
	int         m_nPos;             // Index of the popup menu contained in the menu.
	UINT        m_popupMenuID;      // Popup menu resource ID.
	BOOL        m_bInitialUpdate;   // TRUE to send initial update to views when created.
	CWnd*       m_pParentWnd;       // Points to the parent and will equal 'm_pParentFrame' in non-dialog applications.
	CView*      m_pLastActiveView;  // Points to the last active view that belongs to the main frame window.
	CFrameWnd*  m_pParentFrame;     // Points to the parent frame.

private:
	int        m_nOldIndex;
};

//---------------------------------------------------------------------------

AFX_INLINE CWnd* CXTTabExBase::GetActiveView() {
	return GetView(m_pTabCtrl->GetCurSel());
}
AFX_INLINE CView* CXTTabExBase::GetLastKnownChildView() {
	return m_pLastActiveView;
}
AFX_INLINE void CXTTabExBase::SetMenuID(UINT popupMenuID, int nPos) {
	m_popupMenuID = popupMenuID; m_nPos = nPos;
}
AFX_INLINE UINT CXTTabExBase::GetMenuID() {
	ASSERT(::IsWindow(m_pTabCtrl->GetSafeHwnd())); return m_popupMenuID;
}
AFX_INLINE void CXTTabExBase::SendInitialUpdate(BOOL bInitialUpdate) {
	m_bInitialUpdate = bInitialUpdate;
}

//===========================================================================

//{{AFX_CODEJOCK_PRIVATE
class _XTP_EXT_CLASS CXTTabCtrlBaseEx : public CXTTabExBase // CXTTabCtrlBaseEx deprecated, use CXTTabExBase instead (included for backward compatibility).
{
public:
	virtual BOOL EnableToolTipsImpl(BOOL bEnable) {
		return CXTTabExBase::EnableToolTipsEx(bEnable);
	}
protected:
	void OnRButtonDownImpl(UINT nFlags, CPoint point) {
		CXTTabExBase::OnRButtonDown(nFlags, point);
	}
	int OnCreateImpl_Post(LPCREATESTRUCT lpCreateStruct) {
		return CXTTabExBase::OnCreate(lpCreateStruct);
	}
	void OnDestroyImpl_Pre() {
		CXTTabExBase::OnDestroy();
	}
	void OnSelchangeImpl(NMHDR* pNMHDR, LRESULT* pResult) {
		CXTTabExBase::OnSelchanging(pNMHDR, pResult);
	}
	void OnSelchangingImpl(NMHDR* pNMHDR, LRESULT* pResult) {
		CXTTabExBase::OnSelchanging(pNMHDR, pResult);
	}
	void OnWindowPosChangedImpl_Pre(WINDOWPOS FAR* lpwndpos) {
		CXTTabExBase::OnPreWindowPosChanged(lpwndpos);
	}
	void OnWindowPosChangedImpl_Post(WINDOWPOS FAR*) {
		CXTTabExBase::OnPostWindowPosChanged();
	}
	BOOL PreTranslateMessageImpl(MSG* pMsg) {
		return CXTTabExBase::PreTranslateMessage(pMsg);
	}
	void PreSubclassWindowImpl_Post() {
		CXTTabExBase::PreSubclassWindow();
	}
	BOOL OnCmdMsgImpl_Pre(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
		return CXTTabExBase::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}
	LRESULT OnInitializeImpl(WPARAM, LPARAM) {
		ASSERT(0); return 0; // this is deprecated, tab is self initializing via Init().
	}
	void OnInitialUpdateImpl() {
		CXTTabExBase::Init();
	}
};
//{{AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
#define DECLATE_TABCTRLEX_BASE(ClassName, Tab, Base)\
class _XTP_EXT_CLASS ClassName : public Tab, public Base \
{ \
protected: \
	virtual void PreSubclassWindow() { \
		Tab::PreSubclassWindow(); \
		Base::PreSubclassWindow(); \
	} \
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) {\
		if (!Tab::PreCreateWindow(cs))\
			return FALSE;\
		return Base::PreCreateWindow(cs);\
	}   \
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) { \
		if (Base::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) \
			return TRUE; \
		return Tab::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo); \
	} \
	virtual BOOL PreTranslateMessage(MSG* pMsg) { \
		if (Base::PreTranslateMessage(pMsg)) \
			return TRUE; \
		return Tab::PreTranslateMessage(pMsg); \
	} \
	afx_msg void OnPaint() { \
		Base::OnPaint(); \
	} \
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { \
		return Base::OnEraseBkgnd(pDC); \
	} \
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection) { \
		Tab::OnSettingChange(uFlags, lpszSection); \
		Base::OnSettingChange(uFlags, lpszSection); \
	} \
	afx_msg void OnSysColorChange() { \
		Tab::OnSysColorChange(); \
		Base::OnSysColorChange(); \
	} \
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) { \
		if (Tab::OnCreate(lpCreateStruct) == -1) \
			return -1; \
		return Base::OnCreate(lpCreateStruct); \
	} \
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point) { \
		Base::OnRButtonDown(nFlags, point); \
	} \
	afx_msg void OnDestroy() { \
		Base::OnDestroy(); \
		Tab::OnDestroy(); \
	} \
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) { \
		Base::OnSelchange(pNMHDR, pResult); \
	} \
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) { \
		Base::OnSelchanging(pNMHDR, pResult); \
	} \
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) { \
		Base::OnPreWindowPosChanged(lpwndpos); \
		Tab::OnWindowPosChanged(lpwndpos); \
		Base::OnPostWindowPosChanged(); \
	} \
};

#define ON_TABCTRLEX_REFLECT \
	ON_WM_CREATE() \
	ON_WM_DESTROY() \
	ON_WM_RBUTTONDOWN() \
	ON_WM_WINDOWPOSCHANGED() \
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange) \
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging) \
	ON_TABCTRL_REFLECT
//}}AFX_CODEJOCK_PRIVATE

#endif // __XTTABCTRLBASE_H__
