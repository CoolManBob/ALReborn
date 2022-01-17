// XTPStatusBar.h : interface for the CXTPStatusBar class.
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
#if !defined(__XTPSTATUSBAR_H__)
#define __XTPSTATUSBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXTPCommandBars;
class CXTPPaintManager;
class CXTPStatusBar;
class CXTPImageManagerIcon;
class CXTPImageManager;
class CXTPToolTipContext;

//===========================================================================
// Summary:
//     CXTPStatusBarPane is a CCmdTarget derived class. It represents single pane of status bar
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBarPane : public CXTPCmdTarget
{
private:
	DECLARE_DYNAMIC(CXTPStatusBarPane)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarPane object
	//-----------------------------------------------------------------------
	CXTPStatusBarPane();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBarPane object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPStatusBarPane();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the text caption that is displayed
	//     in this pane.
	// Parameters:
	//     lpszText - Text caption of the pane.
	// See Also: GetText
	//-----------------------------------------------------------------------
	void SetText(LPCTSTR lpszText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the text caption to the pane.
	// Returns:
	//     Text caption of pane.  This is the text displayed in the pane
	// See Also: SetText
	//-----------------------------------------------------------------------
	CString GetText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the width of this pane.
	// Parameters:
	//     nWidth - new width to be set
	// See Also: GetWidth
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the width of the pane.
	// Returns:
	//     Current width of the pane
	// See Also: SetWidth
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the style of a status bar's pane.
	//     A pane's style determines how the pane appears.
	// Parameters:
	//     nStyle - Style of the pane whose style is to be set
	//-----------------------------------------------------------------------
	void SetStyle(int nStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the style of a status bar's pane.
	// Returns:
	//     Style of the indicator pane.
	//-----------------------------------------------------------------------
	int GetStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the ID of a status bar's pane.
	// Returns:
	//     ID of the indicator pane.
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the index of a status bar's pane.
	// Returns:
	//     Index of the indicator pane.
	//-----------------------------------------------------------------------
	int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove the pane
	//-----------------------------------------------------------------------
	void Remove();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the pane is visible.
	// Returns:
	//     TRUE if the tab is visible, FALSE otherwise.
	// See Also: SetVisible
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide or show the pane.
	// Parameters:
	//     bVisible - TRUE to show the tab, FALSE to hide the tab.
	// See Also: IsVisible
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the color of an item text.
	// Parameters:
	//     clrText -  COLORREF object that represents the color of an item
	//     text.
	//-----------------------------------------------------------------------
	void SetTextColor(COLORREF clrText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the color of an item text.
	// Returns:
	//     A COLORREF object that represents the color of an item text.
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the color of an item background.
	// Parameters:
	//     clrBackground -  COLORREF object that represents the background color of an item
	//     text.
	//-----------------------------------------------------------------------
	void SetBackgroundColor(COLORREF clrBackground);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the background color of an item.
	// Returns:
	//     The background color of a item.
	//-----------------------------------------------------------------------
	COLORREF GetBackgroundColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the font of an item text.
	// Parameters:
	//     pFntText -  point to CFont object.
	//     pLogfText -  point to LOGFONT structure.
	//-----------------------------------------------------------------------
	void SetTextFont(CFont* pFntText);
	void SetTextFont(PLOGFONT pLogfText); // <combine CXTPStatusBarPane::SetTextFont@CFont* >

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the font of an item text.
	// Returns:
	//     Pointer to CFont object.
	//-----------------------------------------------------------------------
	virtual CFont* GetTextFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves icon index.
	// Returns:
	//     Icon index.
	// See Also: SetIconIndex
	//-----------------------------------------------------------------------
	int GetIconIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set icon index of item.
	// Parameters:
	//     nIconIndex - Icon index to be set.
	// See Also: GetIconIndex
	//-----------------------------------------------------------------------
	void SetIconIndex(int nIconIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the alignment of an item text.
	// Parameters:
	//     nAlign -  The alignment style of an item text.
	//-----------------------------------------------------------------------
	void SetTextAlignment(int nAlign);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the alignment of an item text.
	// Returns:
	//     The alignment style of an item text.
	//-----------------------------------------------------------------------
	int GetTextAlignment() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the tooltip for this pane.  This is the
	//     text that will be displayed when the mouse pointer is positioned
	//     over the pane.
	// Parameters: lpszTooltip - Tooltip to set for this tab.
	// See Also: GetTooltip
	//-----------------------------------------------------------------------
	void SetTooltip(LPCTSTR lpszTooltip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the tooltip displayed when the mouse
	//     pointer is positioned over the pane.
	// Returns:
	//     The tooltip displayed when the mouse if over the pane.
	// See Also: SetTooltip
	//-----------------------------------------------------------------------
	CString GetTooltip() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Fits the width of the pane to the its text width.
	//-----------------------------------------------------------------------
	void BestFit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get parent status bar object
	// Returns:
	//     Pointer to parent status bar object
	//-----------------------------------------------------------------------
	CXTPStatusBar* GetStatusBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get image of the pane
	// Returns:
	//     CXTPImageManagerIcon Pointer to be drawn.
	//-----------------------------------------------------------------------
	virtual CXTPImageManagerIcon* GetImage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns margins for pane entry
	// See Also: SetMargins
	//-----------------------------------------------------------------------
	CRect GetMargins() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set margins for pane
	// Parameters:
	//     nLeft - Left margin
	//     nTop - Top margin
	//     nRight - Right margin
	//     nBottom - Bottom margin
	// See Also: GetMargins
	//-----------------------------------------------------------------------
	void SetMargins(int nLeft, int nTop, int nRight, int nBottom);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw pane item. Override it to draw custom pane.
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     rcItem - Item rectangle
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC, CRect rcItem);

protected:
	UINT    m_nID;          // IDC of indicator: 0 => normal text area.
	int     m_cxText;       // Width of the string area in pixels. On both sides there is a 3 pixel gap and a 1 pixel border, making a pane 6 pixels wider.
	UINT    m_nStyle;       // Style flags (SBPS_*).
	UINT    m_nFlags;       // State flags (SBPF_*).
	CString m_strText;      // Text in the pane.
	BOOL m_bVisible;        // TRUE if pane is visible
	COLORREF m_clrText;     // Text color
	COLORREF m_clrBackground;       // Background color
	CString m_strToolTip;   // Tooltip text for the item..

	HWND m_hWndPane;        // Child handle
	BOOL m_bAutoDeleteWnd;  // TRUE to auto delete child.
	int m_nAlignment;       // Alignment of the item.

	CXTPStatusBar* m_pStatusBar;    // Parent StatusBar object
	int m_nIndex;           // Index of the item
	int m_nStatusIndex;     // Index in CStatusBarCtrl pane
	CFont m_fntText;        // Font of an item text
	int m_nIconIndex;       // Icon index of the pane
	CRect m_rcMargins;      // Margins of the pane


	friend class CXTPStatusBar;
};

//===========================================================================
// Summary:
//     CXTPStatusBar is a CStatusBar derived class.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBar : public CControlBar
{
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPStatusBar)
//}}AFX_CODEJOCK_PRIVATE

private:
	class CStatusCmdUI;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBar object
	//-----------------------------------------------------------------------
	CXTPStatusBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPStatusBar();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a status bar (a child window) and associates it with the CStatusBar object.
	//     Also sets the initial font and sets the status bar's height to a default value.
	// Parameters:
	//     pParentWnd - Pointer to the CWnd object whose Windows window is the parent of the status bar.
	//     dwStyle    - The status-bar style. In addition to the standard Windows styles, these styles are supported
	//     nID        - The toolbar's child-window ID
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Create(CWnd* pParentWnd,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
		UINT nID = AFX_IDW_STATUS_BAR);
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets each indicator's ID to the value specified by the corresponding
	//     element of the array lpIDArray, loads the string resource specified
	//     by each ID, and sets the indicator's text to the string.
	// Parameters:
	//     lpIDArray - Pointer to an array of IDs.
	//     nIDCount  - Number of elements in the array pointed to by lpIDArray.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL SetIndicators (const UINT* lpIDArray, int nIDCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the command bar's object.
	// Parameters:
	//     pCommandBars - CXTPCommandBars to be set.
	//-----------------------------------------------------------------------
	void SetCommandBars(CXTPCommandBars* pCommandBars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the paint manager.
	// Returns:
	//     A pointer to a CXTPPaintManager object.
	//-----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow status bar use global command bars theme
	// Parameters:
	//     bCommandBarsTheme - TRUE to allow.
	//-----------------------------------------------------------------------
	void UseCommandBarsTheme(BOOL bCommandBarsTheme);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the status bar to reposition pane
	//     item controls.
	//-----------------------------------------------------------------------
	void PositionControls();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a control to an indicator pane.
	// Parameters:
	//     pWnd        - Points to a control window.
	//     nID         - ID of the indicator pane.
	//     bAutoDelete - TRUE if the control is to be deleted when destroyed.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL AddControl(CWnd* pWnd, UINT nID, BOOL bAutoDelete = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the width for an indicator pane.
	// Parameters:
	//     nIndex - Index of the indicator pane.
	//     cxText - New width for the indicator pane.
	//-----------------------------------------------------------------------
	void SetPaneWidth(int nIndex, int cxText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add an indicator pane to the status bar.
	// Parameters:
	//     nID    - ID of the indicator pane.
	//     nIndex - Index of the indicator pane.
	//     pPane  - New CXTPStatusBarPane object
	// Returns:
	//     CXTPStatusBarPane pointer if successful, otherwise returns NULL.
	//-----------------------------------------------------------------------
	CXTPStatusBarPane* AddIndicator(UINT nID, int nIndex);
	CXTPStatusBarPane* AddIndicator(CXTPStatusBarPane* pPane, UINT nID, int nIndex);  // <combine CXTPStatusBar::AddIndicator@UINT@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove an indicator pane from the status bar.
	// Parameters:
	//     nID - ID of the indicator pane.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL RemoveIndicator(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the number of panes that are created
	//     for the status bar.
	// Returns:
	//     An integer value that represents the number of panes for the status bar.
	//-----------------------------------------------------------------------
	int GetPaneCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates total number of visible panes.
	// Returns:
	//     Total number of panes currently visible.
	//-----------------------------------------------------------------------
	int GetVisiblePaneCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the pane information for the given index.
	// Parameters:
	//     nIndex - Index of the indicator pane.
	// Returns:
	//     A pointer to an CXTPStatusBarPane class.
	//-----------------------------------------------------------------------
	CXTPStatusBarPane* GetPane(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to find the pane for the given Identifier.
	// Parameters:
	//     nID - Identifier of the indicator pane.
	// Returns:
	//     A pointer to an CXTPStatusBarPane class.
	//-----------------------------------------------------------------------
	CXTPStatusBarPane* FindPane(UINT nID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove all indicators.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove an indicator pane from the status bar.
	// Parameters:
	//     nIndex - Index of the indicator pane.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the image manager of the image manager.
	// Returns:
	//     A pointer to a CXTPImageManager object
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to determine which pane, if any, is at the
	//     specified screen position.
	// Parameters:
	//     pt - Point to be tested.
	//     lpRect - Returns rectangle of the item under point pt.
	// Returns:
	//     CXTPStatusBarPane pointer, or returns NULL if no pane is
	//     at the specified position.
	//-----------------------------------------------------------------------
	CXTPStatusBarPane* HitTest(CPoint pt, CRect* lpRect = NULL) const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw disabled text.
	// Parameters:
	//     bDraw - TRUE to draw disabled text, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetDrawDisabledText(BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if paint manager draw disabled text.
	//-----------------------------------------------------------------------
	BOOL GetDrawDisabledText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tooltip context pointer.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a single pane of the status bar.
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     nIndex - Index of the pane to draw.
	//     rcItem - Bounding rectangle of the pane
	//-----------------------------------------------------------------------
	virtual void DrawPaneEntry(CDC* pDC, int nIndex, CRect rcItem);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to create a status bar
	// Parameters:
	//     pParentWnd - Pointer to the CWnd object whose Windows window is the parent of the status bar.
	//     dwCtrlStyle - Additional styles for the creation of the embedded CStatusBarCtrl object.
	//     dwStyle - The status-bar style
	//     nID - The status bar's child-window ID.
	// Returns: Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
		UINT nID = AFX_IDW_STATUS_BAR);


	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the indicator index for a given ID. The index of the first indicator is 0
	// Parameters:
	//      nIDFind - String ID of the indicator whose index is to be retrieved
	// Returns:
	//      The index of the indicator if successful; -1 if not successful.
	// See Also: GetItemID
	//-----------------------------------------------------------------------
	int CommandToIndex(UINT nIDFind) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the ID of the indicator specified by nIndex
	// Parameters:
	//     nIndex - Index of the indicator whose ID is to be retrieved.
	// Returns:
	//     The ID of the indicator specified by nIndex.
	// See Also: CommandToIndex
	//-----------------------------------------------------------------------
	UINT GetItemID(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies the coordinates of the indicator specified by nIndex into the structure pointed to by lpRect.
	//     Coordinates are in pixels relative to the upper-left corner of the status bar.
	// Parameters:
	//     nIndex - Index of the indicator whose rectangle coordinates are to be retrieved.
	//     lpRect - Points to aRECT structure or a CRect object that will receive the coordinates of the indicator specified by nIndex.
	// See Also: GetPaneInfo, CommandToIndex
	//-----------------------------------------------------------------------
	void GetItemRect(int nIndex, LPRECT lpRect) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the text that appears in a status-bar pane.
	//     The second form of this member function fills a CString object with the string text.
	// Parameters:
	//     nIndex  - Index of the pane whose text is to be retrieved
	//     rString - A reference to a CString object that contains the text to be retrieved.
	// See Also: SetPaneText
	//-----------------------------------------------------------------------
	CString GetPaneText(int nIndex) const;
	void GetPaneText(int nIndex, CString& rString) const; // <combine CXTPStatusBar::GetPaneText@int@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the pane text to the string pointed to by lpszNewText.
	// Parameters:
	//     nIndex - Index of the pane whose text is to be set.
	//     lpszNewText - Pointer to the new pane text
	//     bUpdate - If TRUE, the pane is invalidated after the text is set.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// See Also: GetPaneText
	//-----------------------------------------------------------------------
	BOOL SetPaneText(int nIndex, const CString& lpszNewText, BOOL bUpdate = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets nID, nStyle, and cxWidth to the ID, style, and width of the indicator
	//     pane at the location specified by nIndex.
	// Parameters:
	//     nIndex  - Index of the pane whose information is to be retrieved.
	//     nID     - Reference to a UINT that is set to the ID of the pane
	//     nStyle  - Reference to a UINT that is set to the style of the pane.
	//     cxWidth - Reference to an integer that is set to the width of the pane
	// See Also: SetPaneInfo, GetItemRect
	//-----------------------------------------------------------------------
	void GetPaneInfo(int nIndex, UINT& nID, UINT& nStyle, int& cxWidth) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the specified indicator pane to a new ID, style, and width.
	// Parameters:
	//     nIndex  - Index of the indicator pane whose style is to be set.
	//     nID     - New ID for the indicator pane
	//     nStyle  - New style for the indicator pane
	//     cxWidth - New width for the indicator pane
	// See Also: GetPaneInfo
	//-----------------------------------------------------------------------
	void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the style of a status bar's pane. A pane's style determines how the pane appears.
	// Parameters:
	//     nIndex - Index of the pane whose style is to be retrieved.
	// Returns:
	//     The style of the status-bar pane specified by nIndex
	// See Also: SetPaneStyle
	//-----------------------------------------------------------------------
	UINT GetPaneStyle(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the style of a status bar's pane. A pane's style determines how the pane appears.
	// Parameters:
	//     nIndex - Index of the pane whose style is to be set.
	//     nStyle - Style of the pane whose style is to be set
	// See Also: GetPaneStyle
	//-----------------------------------------------------------------------
	void SetPaneStyle(int nIndex, UINT nStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function allows direct access to the underlying common control
	// Returns:
	//     Contains a reference to a CStatusBarCtrl object.
	//-----------------------------------------------------------------------
	CStatusBarCtrl& GetStatusBarCtrl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set font for status bar
	// Parameters:
	//      pFont - Font to set
	//      bRedraw - TRUE to redraw status bar
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);


//{{AFX_CODEJOCK_PRIVATE
#if _MSC_VER > 1100
	void SetBorders(LPCRECT lpRect);
	void SetBorders(int cxLeft = 0, int cyTop = 0, int cxRight = 0, int cyBottom = 0);
#endif
//}}AFX_CODEJOCK_PRIVATE

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the stretch gripper of the status bar.
	// Parameters:
	//     pDC      - Pointer to a valid device context.
	//     rcClient - Client rectangle.
	//-----------------------------------------------------------------------
	virtual void DrawStretch(CDC* pDC, CRect& rcClient);

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	void CalcInsideRect(CRect& rect, BOOL bHorz) const;
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	void EnableDocking(DWORD dwDockStyle);
#endif

	void UpdateAllPanes(BOOL bUpdateRects, BOOL bUpdateText);
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	//{{AFX_MSG(CXTPStatusBar)
	public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg LRESULT OnIdleUpdateCmdUI (WPARAM wParam, LPARAM lParam);
	protected:
	afx_msg LRESULT OnNcHitTest(CPoint);
	afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
	afx_msg void OnNcPaint();
	afx_msg void OnWindowPosChanging(LPWINDOWPOS);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMinHeight(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetThemedStatusBar(WPARAM, LPARAM);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	BOOL IsWin4();

private:
	DISABLE_WNDCREATE()
	DISABLE_WNDCREATEEX()


protected:
	CArray <CXTPStatusBarPane*, CXTPStatusBarPane*> m_arrPanes;  // Array of panes
	int m_nMinHeight;                                            // Minimum client height
	CFont m_fontStatus;                                          // Font of status bar
	BOOL m_bDrawDisabledText;                                    // TRUE to draw disabled text

private:
	CXTPCommandBars* m_pCommandBars;
	BOOL m_bCommandBarsTheme;
	CXTPToolTipContext* m_pToolTipContext;          // Tooltip Context.

	friend class CXTPStatusBarPane;
};

// --------------------------------------------------------------------
// Summary:
//     CXTPStatusBarPaneWnd is a CWnd derived class. It is the base class
//     used for creating CWnd objects to place in status bar panes that
//     do custom paint routines.
// --------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPStatusBarPaneWnd : public CWnd
{
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPStatusBarPaneWnd)
//}}AFX_CODEJOCK_PRIVATE

public:
	// -------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarPaneWnd object.
	// -------------------------------------------
	CXTPStatusBarPaneWnd();

	// -------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBarPaneWnd object, handles cleanup and
	//     deallocation
	// -------------------------------------------------------------
	virtual ~CXTPStatusBarPaneWnd();

public:
	// -----------------------------------------------------------------------
	// Summary:
	//     Call this member function to create a status bar pane object that
	//     performs custom draw routines.
	// Parameters:
	//     lpszWindowName -  [in] Text string to be associated with this pane.
	// pParentWnd -      [in] Pointer to the owner status bar window.
	// dwStyle -         [in] Window style.
	// nID -             [in] Control ID.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	// -----------------------------------------------------------------------
	virtual BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE, UINT nID = 0xffff);

protected:

	// -------------------------------------------------------------------
	// Summary:
	//     Override this virtual function in your derived class to perform
	//     your custom drawing routines.
	// Parameters:
	//     pDC -       [in] Pointer to the device context.
	// rcClient -  [in] Size of the client area to draw.
	// -------------------------------------------------------------------
	virtual void DoPaint(CDC* pDC, CRect& rcClient);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPStatusBarPaneWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	CString m_strWindowText;  // NULL terminated string that represents the pane text

};

//===========================================================================
// Summary:
//     CXTPStatusBarLogoPane is a CXTPStatusBarPaneWnd derived class. CXTPStatusBarLogoPane works with
//     CXTStatusBar and allows you to create a logo to place in your status
//     bar area.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBarLogoPane : public CXTPStatusBarPaneWnd
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarLogoPane object
	//-----------------------------------------------------------------------
	CXTPStatusBarLogoPane();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBarLogoPane object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPStatusBarLogoPane();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will set the text to be displayed in the logo
	//     pane.
	// Parameters:
	//     lpszLogoText - A NULL terminated string that represents the text to be displayed.
	//-----------------------------------------------------------------------
	virtual void SetLogoText(LPCTSTR lpszLogoText);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns a CString that represents the logo text.
	// Returns:
	//     A CString object that represents the text that is displayed
	//     in the logo pane.
	//-----------------------------------------------------------------------
	virtual CString GetLogoText();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the font to be displayed in the logo pane.
	// Parameters:
	//     lpszFontName - A NULL terminated string that represents the text to be displayed.
	//     nHeight      - Initial height for the font.
	//     nWeight      - Initial weight for the font.
	//     bItalic      - TRUE if the font is italic.
	//     bUnderline   - TRUE if the font is underlined.
	//     logFont - Address of a LOGFONT structure.
	//-----------------------------------------------------------------------
	virtual void SetLogoFont(LOGFONT& logFont);
	virtual void SetLogoFont(LPCTSTR lpszFontName, int nHeight = 24, int nWeight = FW_BOLD, BOOL bItalic = TRUE, BOOL bUnderline = FALSE); //<combine CXTPStatusBarLogoPane::SetLogoFont@LOGFONT&>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will return the size of the text displayed in
	//     the logo pane.
	// Returns:
	//     A CSize object that represents the current size of the logo pane text.
	//-----------------------------------------------------------------------
	CSize GetTextSize();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will calculate the size of the text that is displayed
	//     in the logo pane and initializes 'm_sizeText'.
	//-----------------------------------------------------------------------
	void SetTextSize();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this virtual function in your derived class to perform your
	//     custom drawing routines.
	// Parameters:
	//     pDC      - Pointer to the device context.
	//     rcClient - Size of the client area to draw.
	//-----------------------------------------------------------------------
	virtual void DoPaint(CDC* pDC, CRect& rcClient);

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE

protected:
	CFont m_Font;       // Font to be used
	CSize m_sizeText;   // Size of the text to be displayed
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CSize CXTPStatusBarLogoPane::GetTextSize() {
	SetTextSize(); m_sizeText.cx+=2; return m_sizeText;
}
AFX_INLINE void CXTPStatusBarLogoPane::SetLogoText(LPCTSTR lpszLogoText) {
	m_strWindowText = lpszLogoText;
}
AFX_INLINE CString CXTPStatusBarLogoPane::GetLogoText() {
	return m_strWindowText;
}

//===========================================================================
// Summary:
//     CXTPStatusBarIconPane is a CXTPStatusBarPaneWnd derived class. CXTPStatusBarIconPane works with
//     CXTStatusBar and allows you to create a logo to place in your status
//     bar area.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBarIconPane : public CXTPStatusBarPaneWnd
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarIconPane object
	//-----------------------------------------------------------------------
	CXTPStatusBarIconPane();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBarIconPane object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPStatusBarIconPane();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the pane icon for this object.
	// Parameters:
	//     nIconID - Resource ID of the icon to display.
	//     lpszIconID - Resource ID of the icon to display.
	//-----------------------------------------------------------------------
	void SetPaneIcon(int nIconID);
	void SetPaneIcon(LPCTSTR lpszIconID); //<combine CXTPStatusBarIconPane::SetPaneIcon@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this virtual function in your derived class to perform your
	//     custom drawing routines.
	// Parameters:
	//     pDC      - Pointer to the device context.
	//     rcClient - Size of the client area to draw.
	//-----------------------------------------------------------------------
	virtual void DoPaint(CDC* pDC, CRect& rcClient);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPStatusBarIconPane)
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CSize           m_sizeIcon; // Size of the icon to display.
	HICON           m_hIcon;    // Icon that is displayed in the status pane.
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPStatusBarPaneWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}



AFX_INLINE void CXTPStatusBar::SetCommandBars(CXTPCommandBars* pCommandBars) {
	m_pCommandBars = pCommandBars;
}
AFX_INLINE void CXTPStatusBar::UseCommandBarsTheme(BOOL bCommandBarsTheme) {
	m_bCommandBarsTheme = bCommandBarsTheme;
}
AFX_INLINE int CXTPStatusBar::GetPaneCount() const {
	return (int)m_arrPanes.GetSize();
}
AFX_INLINE CStatusBarCtrl& CXTPStatusBar::GetStatusBarCtrl() const{
	return *(CStatusBarCtrl*)this;
}

#if _MSC_VER > 1100
AFX_INLINE void CXTPStatusBar::SetBorders(int cxLeft, int cyTop, int cxRight, int cyBottom){
	ASSERT(cyTop >= 2); CControlBar::SetBorders(cxLeft, cyTop, cxRight, cyBottom);
}AFX_INLINE void CXTPStatusBar::SetBorders(LPCRECT lpRect) {
	SetBorders(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
}
#endif

#ifdef _DEBUG
// status bars do not support docking
AFX_INLINE void CXTPStatusBar::EnableDocking(DWORD) {
	ASSERT(FALSE);
}
#endif

AFX_INLINE void CXTPStatusBar::SetDrawDisabledText(BOOL bDraw) {
	m_bDrawDisabledText = bDraw;
}
AFX_INLINE BOOL CXTPStatusBar::GetDrawDisabledText() const {
	return m_bDrawDisabledText;
}
AFX_INLINE CXTPToolTipContext* CXTPStatusBar::GetToolTipContext() const {
	return m_pToolTipContext;
}


#endif //#if !defined(__XTPSTATUSBAR_H__)
