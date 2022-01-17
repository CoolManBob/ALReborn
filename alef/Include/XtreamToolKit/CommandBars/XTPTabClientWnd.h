// XTPTabClientWnd.h : interface for the CXTPTabClientWnd class.
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
#if !defined(__TABCLIENTWNDEX_H__)
#define __TABCLIENTWNDEX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPControlButton.h"

//-----------------------------------------------------------------------
// Summary:
//     The WM_XTP_PRETRANSLATEMOUSEMSG message is sent to owner window to
//     pre-translate mouse messages.
// Parameters:
//     point  - CPoint((DWORD)lParam) - mouse cursor position
//     wParam - mouse notification.
// Returns:
//     If the application is to process this message, the return value
//     should be TRUE.
// Example:
//     Here is an example of how an application would process the WM_XTP_PRETRANSLATEMOUSEMSG
//     message.
// <code>
// BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
//     //{{AFX_MSG_MAP(CMainFrame)
//     ON_MESSAGE(WM_XTP_PRETRANSLATEMOUSEMSG, OnTabbarMouseMsg)
//     //}}AFX_MSG_MAP
// END_MESSAGE_MAP()
//
// LRESULT CMainFrame::OnTabbarMouseMsg(WPARAM wParam, LPARAM lParam)
// {
//     CPoint point = CPoint((DWORD)lParam);
//
//     CXTPTabManagerItem* pItem =  m_MTIClientWnd.HitTest(point);
//
//     if (pItem)
//     {
//          if (wParam == WM_RBUTTONDOWN)
//          {
//
//              CWnd* pFrame = CWnd::FromHandle(pItem->GetHandle());
//              MDIActivate(pFrame);
//
//              m_MTIClientWnd.Refresh();
//
//              CMenu menuPopup;
//              VERIFY(menuPopup.LoadMenu(IDR_WORKSPACE_POPUP));
//
//              m_MTIClientWnd.WorkspaceToScreen(&point);
//              CXTPCommandBars::TrackPopupMenu(menuPopup.GetSubMenu(0), 0, point.x, point.y, this);
//
//              m_MTIClientWnd.Refresh();
//
//              return TRUE;
//          }
//      }
//
//      return FALSE;
// }
// </code>
// See Also: CXTPTabClientWnd
//-----------------------------------------------------------------------
const UINT WM_XTP_PRETRANSLATEMOUSEMSG = (WM_XTP_COMMANDBARS_BASE + 20);

//-----------------------------------------------------------------------
// Summary:
//     The WM_XTP_GETWINDOWTEXT message is sent to MDI child window to retrieve text will be used for
//     tabbed interface.
// Remarks:
//     If application returns 0, CDocument::GetTitle will be used. You can override
//     CXTPTabClientWnd::GetItemText instead of process the message.
// Returns:
//     Points to a null-terminated string that specifies the text to be used.
// Example:
//     Here is an example of how an application would process the WM_XTP_GETWINDOWTEXT
//     message.
// <code>
// BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
//     //{{AFX_MSG_MAP(CChildFrame)
//     ON_MESSAGE(WM_XTP_GETWINDOWTEXT, OnGetTabText)
//     //}}AFX_MSG_MAP
// END_MESSAGE_MAP()
//
// LRESULT CChildFrame::OnGetTabText(WPARAM /*wParam*/, LPARAM /*lParam*/)
// {
//     return (LPCTSTR)m_strCaption;
// }
// </code>
// See Also: CXTPTabClientWnd, WM_XTP_GETTABICON, WM_XTP_GETWINDOWTOOLTIP
//-----------------------------------------------------------------------
const UINT WM_XTP_GETWINDOWTEXT = (WM_XTP_COMMANDBARS_BASE + 21);

//-----------------------------------------------------------------------
// Summary:
//     The WM_XTP_GETWINDOWTOOLTIP message is sent to MDI child window to retrieve tooltips for CXTPTabClientWnd's tabs.
// Remarks:
//     You must call CXTPTabClientWnd::EnableToolTips to enable tooltips.
// Returns:
//     Points to a null-terminated string that specifies the tooltip to be used. <p/>
//     If application returns 0, CDocument::GetPathName will be used.
// Example:
//     Here is an example of how an application would process the WM_XTP_GETWINDOWTOOLTIP
//     message.
// <code>
// BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
//     //{{AFX_MSG_MAP(CChildFrame)
//     ON_MESSAGE(WM_XTP_GETWINDOWTOOLTIP, OnGetTabTip)
//     //}}AFX_MSG_MAP
// END_MESSAGE_MAP()
//
// LRESULT CChildFrame::OnGetTabTip(WPARAM /*wParam*/, LPARAM /*lParam*/)
// {
//     return (LPCTSTR)m_strToolTip;
// }
// </code>
// See Also: CXTPTabClientWnd, WM_XTP_GETTABICON, WM_XTP_GETWINDOWTEXT
//-----------------------------------------------------------------------
const UINT WM_XTP_GETWINDOWTOOLTIP = (WM_XTP_COMMANDBARS_BASE + 23);

//-----------------------------------------------------------------------
// Summary:
//     The WM_XTP_NEWTABITEM message is sent to owner window when new tab was created
// Parameters:
//     pNewItem - (CXTPTabManagerItem*)wParam. Pointer to tab was created.
// Example:
//     Here is an example of how an application would process the WM_XTP_NEWTABITEM
//     message.
// <code>
// BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
//     //{{AFX_MSG_MAP(CMainFrame)
//     ON_MESSAGE(WM_XTP_NEWTABITEM, OnTabbarNewItem)
//     //}}AFX_MSG_MAP
// END_MESSAGE_MAP()
//
// LRESULT CMainFrame::OnTabbarNewItem(WPARAM wParam, LPARAM /*lParam*/)
// {
//     CXTPTabManagerItem* pItem = (CXTPTabManagerItem*)wParam;
//     return 0;
// }
// </code>
// See Also: CXTPTabClientWnd
//-----------------------------------------------------------------------
const UINT WM_XTP_NEWTABITEM = (WM_XTP_COMMANDBARS_BASE + 24);

//-----------------------------------------------------------------------
// Summary:
//     Workspace buttons enumerator, used to indicate which tab navigation
//     buttons to display in the TabWorkspace.
// Example:
//     <code>m_MTIClientWnd.SetFlags(xtpWorkspaceHideClose);</code>
// See Also: CXTPTabClientWnd::SetFlags
//
// <KEYWORDS xtpWorkspaceHideArrows, xtpWorkspaceHideClose, xtpWorkspaceHideAll>
//-----------------------------------------------------------------------
enum XTPWorkspaceButtons
{
	xtpWorkspaceHideArrows = 1,             // To hide arrow buttons.
	xtpWorkspaceHideClose = 2,              // To hide close button.
	xtpWorkspaceHideAll = 3,                // To hide arrow and close buttons.
	xtpWorkspaceShowActiveFiles = 4,        // To show active file button.
	xtpWorkspaceHideArrowsAlways = 1 + 8,   // To hide arrow buttons always
	xtpWorkspaceShowCloseTab = 16,          // To show close button for all tabs.
	xtpWorkspaceShowCloseSelectedTab = 32,  // To show close button for selected tabs.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPWorkspaceNewTabPosition enumerator, used to determine where new tab will appear
// Example:
//     <code>m_MTIClientWnd.SetNewTabPosition(xtpWorkspaceNewTabLeftMost);</code>
// See Also: CXTPTabClientWnd::SetNewTabPosition
//
// <KEYWORDS xtpWorkspaceNewTabRightMost, xtpWorkspaceNewTabLeftMost, xtpWorkspaceNewTabNextToActive>
//-----------------------------------------------------------------------
enum XTPWorkspaceNewTabPosition
{
	xtpWorkspaceNewTabRightMost,             // New tab will be created after all tabs
	xtpWorkspaceNewTabLeftMost,              // New tab will be created before all tabs
	xtpWorkspaceNewTabNextToActive           // New tab will be created after active tab
};

//-----------------------------------------------------------------------
// Summary:
//     XTPWorkspaceActivateTab enumerator, used to determine which tab will
//      become active after the currently active tab is closed.
// See Also: CXTPTabClientWnd.SetAfterCloseActiveTab
//
// <KEYWORDS xtpWorkspaceActivateNextToClosed, xtpWorkspaceActivateTopmost>
//-----------------------------------------------------------------------
enum XTPWorkspaceActivateTab
{
	xtpWorkspaceActivateNextToClosed,       // Activate tab next to the closed tab.
	xtpWorkspaceActivateTopmost             // Activate the left-most tab.
};



//{{AFX_CODEJOCK_PRIVATE
class CXTPTabPaintManager;
class CXTPCommandBars;
class CXTPMDIFrameWnd;
class CXTPToolTipContext;
//}}AFX_CODEJOCK_PRIVATE


//===========================================================================
// Summary:
//     CXTPTabClientWnd is a CWnd derived class. It represents a tabbed
//     workspace.
//===========================================================================
class _XTP_EXT_CLASS CXTPTabClientWnd : public CWnd, public CXTPTabManagerAtom
{
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPTabClientWnd)

protected:
	class CTabClientDropTarget;
	class CNavigateButtonActiveFiles;
//}}AFX_CODEJOCK_PRIVATE

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     CWorkspace is a CXTPTabManager derived class. internal used.
	//-----------------------------------------------------------------------
	class _XTP_EXT_CLASS CWorkspace : public CXTPTabManager
	{

	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CWorkspace object
		//-----------------------------------------------------------------------
		CWorkspace();

		//-----------------------------------------------------------------------
		// Summary:
		//     Destroys a CWorkspace object, handles cleanup and deallocation.
		//-----------------------------------------------------------------------
		virtual ~CWorkspace();

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     Retrieves the paint manager.
		// Returns:
		//     A pointer to a CXTPTabPaintManager object.
		//-----------------------------------------------------------------------
		virtual CXTPTabPaintManager* GetPaintManager() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member is called when a tab navigation button is clicked.
		// Parameters:
		//     pButton - the tab navigation button that was clicked.
		//-----------------------------------------------------------------------
		virtual void OnNavigateButtonClick(CXTPTabManagerNavigateButton* pButton);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member is called when a tab button is clicked.  This
		//     will select the tab that was clicked.
		// Parameters:
		//     pItem - Pointer to the CXTPTabManagerItem tab that was clicked.
		//-----------------------------------------------------------------------
		virtual void OnItemClick(CXTPTabManagerItem* pItem);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called before item click
		// Parameters:
		//     pItem - Pointer to item is about to be clicked
		// Returns:
		//     TRUE if cancel process
		//-----------------------------------------------------------------------
		virtual BOOL OnBeforeItemClick(CXTPTabManagerItem* pItem);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member recalculates the layout of the tab manager and
		//     then repositions itself.  This member must be overridden in
		//     derived classes.
		//-----------------------------------------------------------------------
		virtual void Reposition();

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
		//     For example, on mouseover.  This member is overridden by its
		//     descendants.  This member must be overridden in
		//     derived classes.
		// Returns:
		//     TRUE if the icon was successfully drawn, FALSE if the icon
		//     was not drawn.
		//-----------------------------------------------------------------------
		virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     Initiates redrawing of the control
		// Remarks:
		//     Call this member function if you want to initialize redrawing
		//     of the control. The control will be redrawn taking into account
		//     its latest state.
		// Parameters:
		//     lpRect - The rectangular area of the window that is invalid.
		//     bAnimate - TRUE to animate changes in bounding rectangle.
		//-----------------------------------------------------------------------
		virtual void RedrawControl(LPCRECT lpRect, BOOL bAnimate);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this member to select the specified item.
		// Parameters:
		//     pItem - Points to a CXTPTabManagerItem object.
		//-----------------------------------------------------------------------
		virtual void SetSelectedItem(CXTPTabManagerItem* pItem);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member is called when a CXTPTabManagerItem is clicked
		//     and dragged within the tab header.  This will reorder the
		//     selected tab to the location it is dragged to.
		// Parameters:
		//     hWnd  - Handle to the CWnd object beneath the mouse cursor.
		//     pt - CPoint object specifies xy coordinates.
		//     pItem - Points to a CXTPTabManagerItem object
		//-----------------------------------------------------------------------
		virtual void ReOrder(HWND hWnd, CPoint pt, CXTPTabManagerItem* pItem);

		//-----------------------------------------------------------------------
		// Summary:
		//     Checks to see if the mouse is locked.
		// Returns:
		//     TRUE if locked; otherwise returns FALSE.
		//-----------------------------------------------------------------------
		virtual BOOL IsMouseLocked() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     Returns the tooltip associated with the specified item.
		// Parameters:
		//     pItem - Points to a CXTPTabManagerItem object.
		// Returns:
		//     Associated tooltip
		//-----------------------------------------------------------------------
		virtual CString GetItemTooltip(const CXTPTabManagerItem* pItem) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     Returns whether or not reorder is allowed.
		// Returns:
		//     TRUE when reorder is allowed; FALSE otherwise.
		//-----------------------------------------------------------------------
		virtual BOOL IsAllowReorder() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     Allows/disallows reorder.
		// Parameters:
		//     bAllowReorder - TRUE for allowing reorder; FALSE for disallowing.
		//-----------------------------------------------------------------------
		virtual void SetAllowReorder(BOOL bAllowReorder);

		//-----------------------------------------------------------------------
		// Summary:
		//     Determines if frame must be drawn.
		//-----------------------------------------------------------------------
		virtual BOOL IsDrawStaticFrame() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to get window handle of workspace
		//-----------------------------------------------------------------------
		virtual CWnd* GetWindow() const;

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     Finds the tab item that corresponds to the specified window
		// Parameters:
		//     hWnd - Handle of MDI child window to find.
		// Returns:
		//     Returns a pointer to the specified tab if it was found.
		//-----------------------------------------------------------------------
		CXTPTabManagerItem* FindItem(const HWND hWnd) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     Adds new tab item for the specified window
		// Parameters:
		//     pChildWnd - Pointer to MDI child window to add.
		// Returns:
		//     Returns a pointer to the newly added tab.
		//-----------------------------------------------------------------------
		CXTPTabManagerItem* AddItem(const CWnd* pChildWnd);

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function gets the index for the current icon.
		// Parameters:
		//     pItem - Pointer to a CXTPTabManagerItem.
		// Returns:
		//     An icon index for the current item.
		//-----------------------------------------------------------------------
		HICON GetItemIcon(const CXTPTabManagerItem* pItem) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function returns the RGB value for the referenced
		//     item text or (COLORREF)-1, if the color was not set.
		// Parameters:
		//     pItem - Pointer to a CXTPTabManagerItem.
		// Returns:
		//     The RGB value for the referenced item text, or (COLORREF)-1,
		//     if the color was not set
		//-----------------------------------------------------------------------
		COLORREF GetItemColor(const CXTPTabManagerItem* pItem) const;

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to recalculate layout of tab client
		//-------------------------------------------------------------------------
		virtual void OnRecalcLayout();

	protected:
		CXTPTabClientWnd* m_pTabClientWnd;  // Parent tabbed client.
		double m_dHeight;                   // Height of the workspace
		CRect m_rcSplitter;                 // Splitter position.

		friend class CXTPTabClientWnd;
	};

	//===========================================================================
	// Summary:
	//     CSingleWorkspace is a CWorkspace derived class. internal used.
	//===========================================================================
	class _XTP_EXT_CLASS CSingleWorkspace : public CWnd, public CWorkspace
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CSingleWorkspace object, handles cleanup and deallocation
		//-------------------------------------------------------------------------
		~CSingleWorkspace();

		//-----------------------------------------------------------------------
		// Summary:
		//     Initiates redrawing of the control
		// Parameters:
		//     lpRect - The rectangular area of the window that is invalid.
		//     bAnimate - TRUE to animate changes in bounding rectangle.
		// Remarks:
		//     Call this member function if you want to initialize redrawing
		//     of the control. The control will be redrawn taking into account
		//     its latest state.
		//-----------------------------------------------------------------------
		virtual void RedrawControl(LPCRECT lpRect, BOOL bAnimate);

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     The framework calls this member function to determine whether a
		//     point is in the bounding rectangle of the specified tool.
		// Parameters:
		//     point - Specifies the x- and y-coordinate of the cursor. These
		//             coordinates are always relative to the upper-left corner of the window
		//     pTI   - A pointer to a TOOLINFO structure.
		// Returns:
		//     If the tooltip control was found, the window control ID. If
		//     the tooltip control was not found, -1.
		//-----------------------------------------------------------------------
		INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to get window handle of workspace
		//-----------------------------------------------------------------------
		virtual CWnd* GetWindow() const;

	//{{AFX_CODEJOCK_PRIVATE
		DECLARE_MESSAGE_MAP()

		virtual BOOL PreTranslateMessage(MSG* pMsg);
		BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

		//{{AFX_MSG(CSingleWorkspace)
		public:
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnMouseLeave();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		//}}AFX_MSG
	//}}AFX_CODEJOCK_PRIVATE

	};


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTabClientWnd object
	//-----------------------------------------------------------------------
	CXTPTabClientWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPTabClientWnd object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPTabClientWnd();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Attaches the tabbed workspace.
	// Parameters:
	//     pParentFrame - Points to a CMDIFrameWnd object
	//     bEnableGroups - TRUE to enable groups.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL Attach(CXTPMDIFrameWnd* pParentFrame,  BOOL bEnableGroups = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Detaches the tabbed workspace.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL Detach();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a tab workspace is attached.
	// Returns:
	//     TRUE if attached; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL IsAttached() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the parent frame of the control.
	//-----------------------------------------------------------------------
	CMDIFrameWnd* GetParentFrame() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if right-to-left mode was set.
	// Returns:
	//     TRUE if text is displayed using right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	BOOL IsLayoutRTL() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set right-to-left mode.
	// Parameters:
	//     bRightToLeft - TRUE to set right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	void SetLayoutRTL(BOOL bRightToLeft);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set where new tab must appear.
	// Parameters:
	//     tabPosition - Position of new tab regarding other tabs.
	//                   It can be one of the following
	//                      * <b>xtpWorkspaceNewTabRightMost</b> New tab will be created after all tabs
	//                      * <b>xtpWorkspaceNewTabLeftMost</b> New tab will be created before all tabs
	//                      * <b>xtpWorkspaceNewTabNextToActive</b> New tab will be created after active tab
	//     activeTab - Enumerator that determine which tab will be activated after closing another one.
	//                 It can be one of the following
	//                      * <b>xtpWorkspaceActivateNextToClosed</b> Activate tab next to the closed tab.
	//                      * <b>xtpWorkspaceActivateTopmost</b> Activate the top-most tab.
	// See Also: XTPWorkspaceNewTabPosition, GetNewTabPositon, XTPWorkspaceActivateTab
	//-----------------------------------------------------------------------
	void SetNewTabPosition(XTPWorkspaceNewTabPosition tabPosition);
	void SetAfterCloseActiveTab(XTPWorkspaceActivateTab activeTab); // <combine CXTPTabClientWnd::SetNewTabPosition@XTPWorkspaceNewTabPosition>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is call to determine where new tab must appear.
	// Returns:
	//     XTPWorkspaceNewTabPosition enumerator that show where new tab must appear.
	// See Also: XTPWorkspaceNewTabPosition, SetNewTabPosition
	//-----------------------------------------------------------------------
	XTPWorkspaceNewTabPosition GetNewTabPositon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Saves Position state.
	//-----------------------------------------------------------------------
	void SaveState()
	{
		AfxGetApp()->WriteProfileInt(_T("TabDocking"), _T("Position"), (int)m_pPaintManager->GetPosition());
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads previous docking state.
	//-----------------------------------------------------------------------
	void LoadState()
	{
		int nValue = AfxGetApp()->GetProfileInt(_T("TabDocking"), _T("Position"), xtpTabPositionTop);
		m_pPaintManager->SetPosition(XTPTabPosition(nValue));
		m_bForceToRecalc = TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Scan through all MDIChild windows and update corresponding
	//     tab items if any changes happened (e.g. window text or active MDIChild).
	//-----------------------------------------------------------------------
	virtual void UpdateContents();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create new CWorkspace class, you can override it for custom Workskspace.
	//-----------------------------------------------------------------------
	virtual CWorkspace* CreateWorkspace();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set flags of the tab workspace.
	// Parameters:
	//     dwButtons - Buttons to hide. Can be any of the values listed in the
	//                 remarks section.
	// Remarks:
	//     dwButtons can be one of the following:
	//     * <b>xtpWorkspaceHideArrows</b> Hides the arrow buttons.
	//     * <b>xtpWorkspaceHideClose</b> Hides the close button.
	//     * <b>xtpWorkspaceHideAll</b> Hides the arrow and close buttons.
	// See also: GetFlags, XTPWorkspaceButtons
	//-----------------------------------------------------------------------
	void SetFlags(DWORD dwButtons);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine which tab navigation buttons are
	//     currently displayed in the tab workspace.
	// Returns:
	//     Retrieves tab navigation flags of the tab workspace.
	// See Also: SetFlags, XTPWorkspaceButtons
	//-----------------------------------------------------------------------
	DWORD GetFlags() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to enable or disable tooltips show
	// Parameters:
	//     behaviour - Tooltips behaviour will be set. See remarks section for available flags.
	// Remarks:
	//     <i>behaviour<i> parameter can be one of the following:
	//     * <b>xtpTabToolTipNever</b> Show tooltips for tabs always
	//     * <b>xtpTabToolTipAlways</b> Doesn't show toltips for tabs
	//     * <b> xtpTabToolTipShrinkedOnly</b> Show tooltips only if tab was shrinked (see xtpTabLayoutSizeToFit layout)
	//-----------------------------------------------------------------------
	void EnableToolTips(XTPTabToolTipBehaviour behaviour = xtpTabToolTipAlways);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the paint manager.
	// Returns:
	//     A pointer to a CXTPTabPaintManager object.
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the specified paint manager.
	// Parameters:
	//     pPaintManager - Points to a CXTPTabPaintManager object.
	//-----------------------------------------------------------------------
	void SetPaintManager(CXTPTabPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify whether auto themes are used for the
	//     TabWorkspace.
	// Parameters:
	//     bAutoTheme - TRUE to use auto themes.
	// Remarks:
	//     By default, AutoTheme = TRUE.  When AutoTheme = TRUE, the theme
	//     of the TabWorkspace will change when the theme of the CommandBars
	//     control is changed.  Set AutoTheme = FALSE to keep the currently
	//     set theme even if the CommandBars theme is changed.
	//-----------------------------------------------------------------------
	void SetAutoTheme(BOOL bAutoTheme = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if auto themes are used to
	//     theme the TabWorkspace.
	// Returns:
	//     TRUE if auto themes are used to theme the TabWorkspace, FALSE
	//     if auto themes are not used.
	//-----------------------------------------------------------------------
	BOOL GetAutoTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find the item that corresponds to the
	//     specified window.
	// Parameters:
	//     hWnd - Handle of the window.
	// Returns:
	//     A pointer to a CXTPTabManagerItem object.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* FindItem(const HWND hWnd) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to determine which tab, if any, is at the
	//     specified screen position.
	// Parameters:
	//     pt - Point to be tested.
	// Returns:
	//     The zero-based index of the tab, or returns -1 if no tab is
	//     at the specified position.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* HitTest(CPoint pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the current number of workspaces.
	// Returns:
	//     An integer value that represents the number of workspaces.
	// Remarks:
	//     If tab groups are enabled, then there will be more than
	//     one workspace.  If tab groups are disabled, then there is only 1
	//     workspace with an index of 0.
	// See Also: GetWorkspace
	//-----------------------------------------------------------------------
	int GetWorkspaceCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the workspace at the specified index.
	// Parameters:
	//     nIndex - An integer index starting at 0.
	// Returns:
	//     A pointer to the workspace currently at this index.
	// Remarks:
	//     If tab groups are enabled, then there will be more than
	//     one workspace.  If tab groups are disabled, then there is only 1
	//     workspace with an index of 0.
	// See Also: GetWorkspaceCount
	//-----------------------------------------------------------------------
	CWorkspace* GetWorkspace(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the workspace command mode.
	// Parameters:
	//     nID    - Specifies the identifier of the workspace.
	//     ppItem - MDI active item.
	// Remarks:
	//     Call this member function if you want to determine whether or
	//     not workspace command is enabled.
	// Returns:
	//     TRUE when workspace command is enabled; FALSE when it is disabled.
	//-----------------------------------------------------------------------
	BOOL IsWorkspaceCommandEnabled(UINT nID, CXTPTabManagerItem** ppItem = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to execute special workspace command
	// Parameters:
	//     nID - Specifies the identifier of the workspace. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     nID can be one of the following:
	//     * <b>XTP_ID_WORKSPACE_NEWVERTICAL</b> Creates new vertical group.
	//     * <b>XTP_ID_WORKSPACE_NEWHORIZONTAL</b> Creates new horizontal group.
	//     * <b>XTP_ID_WORKSPACE_MOVEPREVIOUS</b> Moves item to previous group.
	//     * <b>XTP_ID_WORKSPACE_MOVENEXT</b> Moves item to next group.
	//-----------------------------------------------------------------------
	void OnWorkspaceCommand(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to refresh all the items.
	// Parameters:
	//     bRecalcLayout - true to recalculate layout of frame.
	//-----------------------------------------------------------------------
	void Refresh(BOOL bRecalcLayout = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts the workspace coordinates of a given point on the display to screen coordinates.
	// Parameters:
	//     lpPoint - Points to a POINT structure or CPoint object that contains the client coordinates to be converted.
	//-----------------------------------------------------------------------
	void WorkspaceToScreen(LPPOINT lpPoint) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts the screen coordinates of a given point or rectangle on the display to client coordinates.
	// Parameters:
	//     lpPoint - Points to a CPoint object or POINT structure that contains the screen coordinates to be converted.
	//-----------------------------------------------------------------------
	void ScreenToWorkspace(LPPOINT lpPoint) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to show or hide the workspace.
	// Parameters:
	//     bShow - TRUE to show the workspace, or FALSE to hide it.
	//-----------------------------------------------------------------------
	void ShowWorkspace(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom workspace control.
	// Parameters:
	//     pWorkspace - Custom workspace control
	//-----------------------------------------------------------------------
	void SetTabWorkspace(CWorkspace* pWorkspace);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the current reordering allowance.
	// Returns:
	//     TRUE when reordering is allowed; FALSE when it is disallowed.
	//-----------------------------------------------------------------------
	BOOL IsAllowReorder() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to allow or disallow reordering.
	// Parameters:
	//     bAllowReorder - TRUE for allowing reordering; FALSE for disallowing
	//                     reordering.
	//-----------------------------------------------------------------------
	void SetAllowReorder(BOOL bAllowReorder);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the background is filled.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//     rc  - Rectangle area to be filled.
	//-----------------------------------------------------------------------
	virtual void OnFillBackground(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the client is drawn.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//     rc  - Rectangle area to be drawn.
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tooltip context pointer.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to use trackers.
	// Parameters:
	//     bSplitterTracker - TRUE to use trackers.
	// Remarks:
	//     This member function will display the contents for child window
	//     while the splitter is resized if bSplitterTracker is set to FALSE.
	// See Also: IsSplitterTrackerUsed
	//-----------------------------------------------------------------------
	void UseSplitterTracker(BOOL bSplitterTracker);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the splitter tracker is used.
	// Returns:
	//     TRUE if the splitter tracker is used; otherwise returns FALSE.
	// See Also: UseSplitterTracker
	//-----------------------------------------------------------------------
	BOOL IsSplitterTrackerUsed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the command bar's object.
	// Returns:
	//     A CXTPCommandBars pointer (can be NULL).
	//-----------------------------------------------------------------------
	virtual CXTPCommandBars* GetCommandBars() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to tile child windows in workspace (only if EnableGroups enabled)
	// Parameters:
	//     bHorizontal - TRUE to tile horizontally.
	//-----------------------------------------------------------------------
	void MDITile(BOOL bHorizontal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns an item by its index.
	// Parameters:
	//     nIndex - Zero-based index of the item in the collection.
	// Remarks:
	//     You use this member function to get an item from collection by
	//     item index. If the given index is less than 0 or greater than
	//     the value returned by GetItemCount(), GetItem() returns NULL.
	// Returns:
	//     Pointer to the found item, if any, or NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* GetItem(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the number of items.
	// Returns:
	//     The number of items.
	//-----------------------------------------------------------------------
	int GetItemCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to reposition items.
	//-----------------------------------------------------------------------
	virtual void Reposition();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     If m_bAutoTheme is FALSE, then it checks to see if the theme
	//     set for the TabWorkspace is different from the theme set for
	//     the CommandBars.  If the themes are different, then the TabWorkSpace
	//     theme is set to the theme specified in m_themeCommandBars.
	//-----------------------------------------------------------------------
	virtual void CheckCommandBarsTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the current active MDI child window.
	// Returns:
	//     A pointer to the active MDI child window.
	//-----------------------------------------------------------------------
	CWnd* MDIGetActive();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to activate an MDI child window
	//     independently of the MDI frame window.
	// Parameters:
	//     pWnd - Pointer to a valid device context.
	//-----------------------------------------------------------------------
	void MDIActivate(CWnd* pWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the text of a specific tab.
	// Parameters:
	//     pChildWnd - A CWnd pointer that represents the child to be
	//                 displayed when the window is activated.
	// Returns:
	//     The text of a particular tab, or NULL if an error occurs.
	//-----------------------------------------------------------------------
	virtual CString GetItemText(const CWnd* pChildWnd) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the RGB value for the referenced
	//     item text or (COLORREF)-1, if the color was not set.
	// Parameters:
	//     pItem - A pointer to a CXTPTabManagerItem.
	// Returns:
	//     The RGB value for the referenced item text, or (COLORREF)-1,
	//     if the color was not set.
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemColor(const CXTPTabManagerItem* pItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the icon of a specific tab.
	// Parameters:
	//     pItem - A pointer to a CXTPTabManagerItem.
	// Returns:
	//     The icon of a particular tab, or NULL if an error occurs.
	//-----------------------------------------------------------------------
	virtual HICON GetItemIcon(const CXTPTabManagerItem* pItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the tooltip associated with the specified item.
	// Parameters:
	//     pItem - Points to a CXTPTabManagerItem object.
	// Returns:
	//     Associated tooltip
	//-----------------------------------------------------------------------
	virtual CString GetItemTooltip(const CXTPTabManagerItem* pItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a workspace.
	// Parameters:
	//     nIndex - Index of the workspace.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual CWorkspace*  AddWorkspace(int nIndex = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function adds a new tab item for the specified window.
	// Parameters:
	//     pChildWnd - A CWnd pointer that represents the child to be
	//                 displayed when the window is activated.
	// Returns:
	//     A pointer to a CXTPTabManagerItem object.
	//-----------------------------------------------------------------------
	virtual CXTPTabManagerItem* AddItem(CWnd* pChildWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find the specified index.
	// Parameters:
	//     pWorkspace - Pointer to a CXTPTabManger object.
	// Returns:
	//     The index of the matching item.
	//-----------------------------------------------------------------------
	int FindIndex(CXTPTabManager* pWorkspace) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions sets a workspace active.
	// Parameters:
	//     pWorkspace - Pointer to a CWorkspace object.
	//-----------------------------------------------------------------------
	void SetActiveWorkspace(CWorkspace* pWorkspace);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method shows the context menu for toolbars.
	// Parameters:
	//     pt - Position of the menu to show.
	//-----------------------------------------------------------------------
	void ContextMenu(CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to initialize a loop.
	//-----------------------------------------------------------------------
	void InitLoop();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called when a loop is canceled.
	//-----------------------------------------------------------------------
	void CancelLoop();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to update the status of the
	//     TabWorkspace navigation buttons.
	// Parameters:
	//     pWorkspace - Pointer to a CWorkspace object.
	// SeeAlso:  SetFlags, GetFlags
	//-----------------------------------------------------------------------
	virtual void UpdateFlags(CWorkspace* pWorkspace);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the selected item in the control.
	// Returns:
	//     A pointer to a CXTPTabManagerItem object.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* GetSelectedItem() const;


protected:
//{{AFX_CODEJOCK_PRIVATE
	void DoWorkspaceCommand(CXTPTabManagerItem* pItem, CWorkspace* pFocusWorkspace, int nAction);
	void DrawFocusRect(BOOL bRemoveRect = FALSE);
	void TrackSplitter(int nWorkspace, CPoint point);
	void RepositionWorkspaces(CRect rc, CRect rcAvail, CWorkspace* pWorkspaceFirst, CWorkspace* pWorkspaceSecond);
	void NormalizeWorkspaceSize();
	void ReorderWorkspace(CPoint pt, CXTPTabManagerItem* pItem);
	void ActivateNextItem(CXTPTabManagerItem* pItem);
	virtual BOOL OnBeforeItemClick(CXTPTabManagerItem* pItem);
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPTabClientWnd)
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTabClientWnd)
	afx_msg LRESULT OnMDIActivate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMDICreate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMDIDestroy(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMDINext(WPARAM wParam, LPARAM lParam);


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdateWorkspaceCommand(CCmdUI* pCmdUI);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	public:
	afx_msg void OnIdleUpdateCmdUI();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

public:
	BOOL m_bDelayLock;                      // TRUE to lock update with small delay
	BOOL m_bLockUpdate;                     // TRUE to lock update tabs.
	BOOL m_bForceToRecalc;                  // TRUE to recalculate in next idle event.
	BOOL m_bLockReposition;                 // TRUE to lock reposition.

protected:
	CArray<CWorkspace*, CWorkspace*> m_arrWorkspace;    // Array of workspaces.
	CWorkspace* m_pTabWorkspace;            // Custom workspace
	BOOL m_bUserWorkspace;                  // TRUE if custom workspace used

	CMDIFrameWnd* m_pParentFrame;           // pointer to the corresponding parent MDIFrame window
	BOOL m_bRefreshed;                      // TRUE if Refresh currently executed.

	CXTPTabPaintManager* m_pPaintManager;   // Current paint manager.
	XTPPaintTheme m_themeCommandBars;       // Theme set for the TabWorkspace (Can be different from CommandBars if m_bAutoTheme is FALSE)
	BOOL m_bAutoTheme;                      // TRUE to auto theme the TabWorkspace.

	CWorkspace* m_pActiveWorkspace;         // Active workspace.
	BOOL m_bHorizSplitting;                 // TRUE if groups devided horizontally

	HCURSOR m_hCursorHoriz;                 // Horizontal split cursor handle.
	HCURSOR m_hCursorVert;                  // Vertical split cursor handle.
	HCURSOR m_hCursorNew;                   // New item cursor handle
	HCURSOR m_hCursorDelete;                // Remove item cursor handle

	BOOL m_bThemedBackColor;                // TRUE if a themed backcolor will be used.  If TRUE, then the backcolor of the MDIClient is calculated from current theme, if FALSE the standard BackColor property will be used

	BOOL m_bEnableGroups;                   // TRUE if TabWorkspace groups are enabled.

	DWORD m_dwFlags;                        // Specifies which tab navigation buttons are displayed.
	BOOL m_bAllowReorder;                   // TRUE to allow the user to reorder tabs.
	BOOL m_bShowWorkspace;                  // TRUE to show the TabWorkspace, FALSE to hide the TabWorkspace.

	BOOL m_bUpdateContents;                 // TRUE if UpdateContents executed.
	CXTPToolTipContext* m_pToolTipContext;  // Tooltip Context.
	BOOL m_bUseSplitterTracker;             // If TRUE, splitter trackers are used.  When resizing a workspace, an outline of the workspace is drawn as the splitter is dragged.  If FALSE, the workspace will be resized in "real-time."
	int m_nSplitterSize;                    // Splitter size
	XTPWorkspaceNewTabPosition m_newTabPosition; // New tab position.
	XTPWorkspaceActivateTab m_afterCloseTabPosition;  // Tab to make active after the currently active tab is closed.

protected:
//{{AFX_CODEJOCK_PRIVATE
	CRect m_rcGroup;
	CTabClientDropTarget* m_pDropTarget;
	CWorkspace* m_pFocusWorkspace;
	int m_nFocusedAction;
	CRect m_rectLast;
	CSize m_sizeLast;
	CDC* m_pDC;
	BOOL m_bIgnoreFlickersOnActivate;
	BOOL m_bRightToLeft;
	UINT m_nMsgUpdateSkinState;
	UINT m_nMsgQuerySkinState;
//}}AFX_CODEJOCK_PRIVATE



	friend class CWorkspace;
	friend class CSingleWorkspace;
	friend class CTabClientDropTarget;
	friend class CNavigateButtonActiveFiles;
	friend class CXTPControlTabWorkspace;
};


class _XTP_EXT_CLASS CXTPControlTabWorkspace : public CXTPControlButton, public CXTPTabClientWnd::CWorkspace
{
	DECLARE_XTP_CONTROL(CXTPControlTabWorkspace)

public:
	CXTPControlTabWorkspace();
	~CXTPControlTabWorkspace();

public:
	virtual void RedrawControl(LPCRECT lpRect, BOOL bAnimate);
	virtual CWnd* GetWindow() const;
	CXTPTabPaintManager* GetPaintManager() const;
	virtual void OnRecalcLayout();
	XTPTabPosition GetPosition() const;
	virtual void OnRemoved();

protected:
	virtual void SetRect(CRect rcControl);
	virtual void Draw(CDC* pDC);
	virtual CSize GetSize(CDC* pDC);
	CString GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, INT_PTR* nHit = 0) const;

	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	void OnMouseMove(CPoint point);

protected:
	BOOL m_bForceRecalc;

};


/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPTabClientWnd::IsAttached() const {
	return (m_pParentFrame!=NULL ? TRUE : FALSE);
}

AFX_INLINE CMDIFrameWnd* CXTPTabClientWnd::GetParentFrame() const {
	return m_pParentFrame;
}
AFX_INLINE DWORD CXTPTabClientWnd::GetFlags() const {
	return m_dwFlags;
}
AFX_INLINE CXTPToolTipContext* CXTPTabClientWnd::GetToolTipContext() const {
	return m_pToolTipContext;
}
AFX_INLINE void CXTPTabClientWnd::UseSplitterTracker(BOOL bSplitterTracker) {
	m_bUseSplitterTracker = bSplitterTracker;
}
AFX_INLINE BOOL CXTPTabClientWnd::IsSplitterTrackerUsed() const {
	return m_bUseSplitterTracker;
}
AFX_INLINE void CXTPTabClientWnd::SetNewTabPosition(XTPWorkspaceNewTabPosition tabPosition) {
	m_newTabPosition = tabPosition;
}
AFX_INLINE XTPWorkspaceNewTabPosition CXTPTabClientWnd::GetNewTabPositon() const {
	return m_newTabPosition;
}
AFX_INLINE void CXTPTabClientWnd::SetAfterCloseActiveTab(XTPWorkspaceActivateTab activeTab) {
	m_afterCloseTabPosition = activeTab;
}

#endif// #if !defined(__XTPTABCLIENTWNDEX_H__)
