// XTPRibbonBar.h: interface for the CXTPRibbonBar class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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
#if !defined(__XTPRIBBONBAR_H__)
#define __XTPRIBBONBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXTPRibbonTheme;
class CXTPRibbonQuickAccessControls;
class CXTPRibbonTab;
class CXTPRibbonControlTab;
class CXTPTabPaintManager;
class CXTPRibbonBar;
class CXTPRibbonGroup;
class CXTPOffice2007FrameHook;
class CXTPRibbonTabContextHeaders;
class CXTPRibbonGroups;

#include "CommandBars/XTPControls.h"
#include "CommandBars/XTPMenuBar.h"

class CXTPRibbonScrollableBar
{
private:
	class CControlGroupsScroll;

protected:
	void InitScrollableBar(CXTPCommandBar* pParent);

	void ShowScrollableRect(CXTPRibbonGroups* pGroups, CRect rc);

public:
	void EnableGroupsScroll(BOOL bScrollLeft, BOOL bScrollRight);
	virtual void OnGroupsScroll(BOOL bScrollLeft) = 0;
	virtual CRect GetGroupsRect() const = 0;

	void CreateGroupKeyboardTips(CXTPRibbonTab* pSelectedTab);

public:
	int m_nGroupsScrollPos;         // Groups scroll position

protected:
	CXTPCommandBar* m_pParent;
	CXTPControl* m_pControlScrollGroupsLeft;    // Control to draw left scroll
	CXTPControl* m_pControlScrollGroupsRight;   // Control to draw right scroll
};

CXTPRibbonScrollableBar* AFX_CDECL GetScrollableBar(CXTPCommandBar* pCommandBar);

//===========================================================================
// Summary:
//     CXTPRibbonBar is a CXTPMenuBar derived class, It represents Ribbon control from Office 2007.
// Example:
//     The following code sample demonstrates how to create CXTPRibbonBar:
// <code>
// CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("The Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
// </code>
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonBar : public CXTPMenuBar, public CXTPRibbonScrollableBar
{
private:
	class CControlCaptionButton;
	class CControlQuickAccessMorePopup;
	class CControlQuickAccessCommand;
	class CMorePopupToolBar;

private:
	DECLARE_XTP_COMMANDBAR(CXTPRibbonBar)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonBar object
	//-----------------------------------------------------------------------
	CXTPRibbonBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonBar();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to add new tab for ribbon bar
	// Parameters:
	//     lpszCaption - Caption of tab to be added
	//     nID         - Identifier of tab to be added
	// Returns:
	//     Pointer to new CXTPRibbonTab object
	// See Also: InsertTab
	//-----------------------------------------------------------------------
	CXTPRibbonTab* AddTab(LPCTSTR lpszCaption);
	CXTPRibbonTab* AddTab(int nID); // <combine CXTPRibbonBar::AddTab@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to insert new tab for ribbon bar to specified position
	// Parameters:
	//     nItem       - Position to insert new tab
	//     lpszCaption - Caption of tab to be added
	//     nID         - Identifier of tab
	// Returns:
	//     Pointer to new CXTPRibbonTab object
	// See Also: AddTab
	//-----------------------------------------------------------------------
	CXTPRibbonTab* InsertTab(int nItem, int nID);
	CXTPRibbonTab* InsertTab(int nItem, LPCTSTR lpszCaption, int nID = 0); // <combine CXTPRibbonBar::InsertTab@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to get current selected tab
	// Returns:
	//     Pointer to current selected CXTPRibbonTab object
	// See Also: SetCurSel
	//-----------------------------------------------------------------------
	CXTPRibbonTab* GetSelectedTab() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to get tab in specified position
	// Parameters:
	//     nIndex - Index of tab to retrieve
	// Returns:
	//     Pointer to CXTPRibbonTab object in specified position
	// See Also: GetTabCount
	//-----------------------------------------------------------------------
	CXTPRibbonTab* GetTab(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to find tab by its identifier
	// Parameters:
	//     nId - Identifier of tab to be found
	// Returns:
	//     Pointer to CXTPRibbonTab object with specified identifier
	// See Also: FindGroup
	//-----------------------------------------------------------------------
	CXTPRibbonTab* FindTab(int nId) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to find group by its identifier
	// Parameters:
	//     nId - Identifier of group to be found
	// Returns:
	//     Pointer to CXTPRibbonGroup object with specified identifier
	// See Also: FindTab
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* FindGroup(int nId) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the number of tabs in the ribbon bar.
	// Returns:
	//     Number of tabs in the ribbon bar.
	// See Also: GetTab
	//-----------------------------------------------------------------------
	int GetTabCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves ribbon paint manager.
	// Returns:
	//     Pointer to CXTPRibbonTheme object
	// See Also: AddTab
	//-----------------------------------------------------------------------
	CXTPRibbonTheme* GetRibbonPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to select tab.
	// Parameters:
	//     nIndex - Index of tab to be selected
	// See Also: GetSelectedTab
	//-----------------------------------------------------------------------
	void SetCurSel(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the size of the command bar button.
	// Returns:
	//     The width and height values of the command bar button.
	//-----------------------------------------------------------------------
	CSize GetButtonSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get select tab index.
	// See Also: SetCurSel
	//-----------------------------------------------------------------------
	int GetCurSel() const;

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
	CXTPTabPaintManager* GetTabPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get highlighted group
	// Returns:
	//     CXTPRibbonGroup object under mouse cursor
	// See Also: CXTPRibbonGroup
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* GetHighlightedGroup() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if groups part of ribbon bar is visible
	// Returns:
	//     TRUE if groups are visible
	// See Also: SetGroupsVisible
	//-----------------------------------------------------------------------
	BOOL IsGroupsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if tabs part of ribbon bar is visible
	// Returns:
	//     TRUE if tabs are visible
	// See Also: SetTabsVisible
	//-----------------------------------------------------------------------
	BOOL IsTabsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if ribbon bar is visible
	//-----------------------------------------------------------------------
	BOOL IsRibbonBarVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to show or hide the ribbon.
	// Parameters:
	//     bVisible - TRUE to show the ribbon; FALSE to hide.
	//-----------------------------------------------------------------------
	virtual void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to hide/show groups part of ribbon bar
	// Parameters:
	//     bVisible - TRUE to show groups
	// See Also: IsGroupsVisible
	//-----------------------------------------------------------------------
	void SetGroupsVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to hide/show tabs part of ribbon bar
	// Parameters:
	//     bVisible - TRUE to show tabs
	// See Also: IsTabsVisible
	//-----------------------------------------------------------------------
	void SetTabsVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get CXTPRibbonControlTab that represents ribbon tabs
	//-----------------------------------------------------------------------
	CXTPRibbonControlTab* GetControlTab() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set minimum width before ribbon bar disappear
	// Parameters:
	//     nMinVisibleWidth - Minimum width.
	// Remarks:
	//     Default value is 250 pixels
	// See Also: GetMinimumVisibleWidth
	//-----------------------------------------------------------------------
	void SetMinimumVisibleWidth(int nMinVisibleWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get minimum width before ribbon bar disappear
	// Returns:
	//     Minimum width before ribbon bar disappear.
	// See Also: SetMinimumVisibleWidth
	//-----------------------------------------------------------------------
	int GetMinimumVisibleWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves collection of Quick Access controls.
	// Returns:
	//     Pointer to CXTPRibbonQuickAccessControls contained Quick Access controls.
	//-----------------------------------------------------------------------
	CXTPRibbonQuickAccessControls* GetQuickAccessControls() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add ribbon top-left system button.
	// Parameters:
	//     nID - Identifier of system button
	// See Also: GetSystemButton
	//-----------------------------------------------------------------------
	CXTPControlPopup* AddSystemButton(int nID = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get ribbon top-left system button.
	// See Also: AddSystemButton
	//-----------------------------------------------------------------------
	CXTPControlPopup* GetSystemButton() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the CommandBar is a
	//     CXTPRibbonBar.
	// Returns:
	//     Returns TRUE if the CommandBar is a CXTPRibbonBar, otherwise FALSE
	// See Also: CXTPCommandBar, CXTPRibbonBar
	//-----------------------------------------------------------------------
	virtual BOOL IsRibbonBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove all tab
	// See Also: RemoveTab
	//-----------------------------------------------------------------------
	void RemoveAllTabs();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove single tab
	// Parameters:
	//     nIndex - Index of tab to remove
	// See Also: RemoveTab
	//-----------------------------------------------------------------------
	void RemoveTab(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to be sure control is visible on ribbon bar
	// Parameters:
	//     pControl - CXTPControl child that need to check
	//-----------------------------------------------------------------------
	virtual void EnsureVisible(CXTPControl* pControl);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable Office 2007 frame.
	// See Also: IsFrameThemeEnabled
	//-----------------------------------------------------------------------
	void EnableFrameTheme(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Office 2007 frame enabled
	// See Also: IsFrameThemeEnabled
	//-----------------------------------------------------------------------
	BOOL IsFrameThemeEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show Quick Access controls below ribbon bar
	// Parameters:
	//     bBelow - TRUE to show below ribbon; FALSE - above.
	// See Also: IsQuickAccessBelowRibbon
	//-----------------------------------------------------------------------
	void ShowQuickAccessBelowRibbon(BOOL bBelow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determine if Quick Access controls located below ribbon bar
	// See Also: ShowQuickAccessBelowRibbon
	//-----------------------------------------------------------------------
	BOOL IsQuickAccessBelowRibbon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show/hide quick access
	// Parameters:
	//     bShow - TRUE to show quick access; FALSE to hide
	// See Also: IsQuickAccessVisible, ShowQuickAccessBelowRibbon
	//-----------------------------------------------------------------------
	void ShowQuickAccess(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if quick access is visible
	// Returns:
	//     TRUE if quick access is visible
	// See Also: ShowQuickAccess
	//-----------------------------------------------------------------------
	BOOL IsQuickAccessVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow same controls for quick access
	// Parameters:
	//     bAllow - TRUE to allow add same controls for quick access
	// See Also: IsAllowQuickAccessDuplicates, ShowQuickAccessBelowRibbon
	//-----------------------------------------------------------------------
	void AllowQuickAccessDuplicates(BOOL bAllow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow customization for quick access
	// Parameters:
	//     bAllow - TRUE to allow customization for quick access
	// See Also: IsAllowQuickAccessDuplicates, ShowQuickAccessBelowRibbon
	//-----------------------------------------------------------------------
	void AllowQuickAccessCustomization(BOOL bAllow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if user can add 2 or more same controls to quick access
	// Returns: TRUE if duplicates allowed
	// See Also: AllowQuickAccessDuplicates
	//-----------------------------------------------------------------------
	BOOL IsAllowQuickAccessDuplicates() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to select next or previous tab
	// Parameters:
	//     bNext - TRUE to select next
	//-----------------------------------------------------------------------
	void SelectNextTab(BOOL bNext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to minimize Ribbon Bar
	// Parameters:
	//     bMinimized - TRUE to minimize Ribbon Bar.
	//-----------------------------------------------------------------------
	void SetRibbonMinimized(BOOL bMinimized);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if ribbon bar is minimized
	//-----------------------------------------------------------------------
	BOOL IsRibbonMinimized() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to disable minimize feature of Ribbon Bar
	// Parameters:
	//     bAllow - TRUE to allow minimize ribbon bar; FALSE to forbid
	//-----------------------------------------------------------------------
	void AllowMinimize(BOOL bAllow);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     When switching tabs, RebuildControls first deletes all the controls
	//     from the old tab groups.  Then controls are added to the tab groups
	//     of the tab to be selected.  Finally, the new tab is selected.
	// Parameters:
	//     pSelected - Tab to be selected.
	// See Also: CXTPRibbonControlTab::SetSelectedItem, CXTPRibbonTab::GetGroups
	//-----------------------------------------------------------------------
	void RebuildControls(CXTPRibbonTab* pSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bounding rectangle for the ribbon tabs.
	// Returns:
	//     Bounding rectangle for the ribbon tabs.
	// See Also: CXTPRibbonThemeOffice2007Theme::FillRibbonBar
	//-----------------------------------------------------------------------
	CRect GetTabControlRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bounding rectangle for the ribbon caption.
	// Returns:
	//     Bounding rectangle for the ribbon caption.
	// See Also: GetCaptionTextRect, GetQuickAccessRect, GetTabControlRect
	//-----------------------------------------------------------------------
	CRect GetCaptionRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bounding rectangle for the ribbon caption text.
	// Returns:
	//     Bounding rectangle for the ribbon caption text.
	// See Also: GetCaptionRect, GetQuickAccessRect, GetTabControlRect
	//-----------------------------------------------------------------------
	CRect GetCaptionTextRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bounding rectangle for the ribbon Quick Access controls.
	// Returns:
	//     Bounding rectangle for the ribbon  Quick Access controls.
	// See Also: GetCaptionRect, GetCaptionTextRect, GetTabControlRect
	//-----------------------------------------------------------------------
	CRect GetQuickAccessRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines which ribbon group,
	//     if any, is at a specified position.
	// Parameters:
	//     point  - A CPoint that contains the coordinates of the point to test.
	// Returns:
	//      The CXTPRibbonGroup that is at the specified point, if no group is
	//      at the point, then NULL is returned.
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* HitTestGroup(CPoint point) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves tabs area height
	// See Also: GetGroupsHeight
	//-----------------------------------------------------------------------
	virtual int GetTabsHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves Quick Access area height
	// See Also: GetGroupsHeight, GetTabsHeight
	//-----------------------------------------------------------------------
	int GetQuickAccessHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates Quick Access area height
	//-----------------------------------------------------------------------
	int CalcQuickAccessHeight();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves group area height;
	// See Also: GetTabsHeight
	//-----------------------------------------------------------------------
	virtual int GetGroupsHeight();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves Groups bounding rectangle
	//-----------------------------------------------------------------------
	CRect GetGroupsRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method calculates groups height using GetLargeIconSize method and height of captions
	// Returns: Height of ribbon groups
	// See Also: GetGroupsHeight
	//-----------------------------------------------------------------------
	virtual int CalcGroupsHeight();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set height of the ribbon groups
	// Parameters:
	//     nHeight - Height of groups to set
	// See Also: GetTabsHeight
	//-----------------------------------------------------------------------
	void SetGroupsHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get caption height of ribbon bar
	// Returns: Caption height in pixels
	// See Also: CalcGroupsHeight
	//-----------------------------------------------------------------------
	int GetCaptionHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves Context Headers collection;
	// Returns:
	//     Pointer to CXTPRibbonTabContextHeaders class contained
	//     CXTPRibbonTabContextHeader collection
	// See Also: CXTPRibbonTabContextHeader
	//-----------------------------------------------------------------------
	CXTPRibbonTabContextHeaders* GetContextHeaders() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if control belongs to quick access controls.
	// Parameters:
	//     pControl - Control to test
	// Returns:
	//     TRUE if control located in quick access area.
	//-----------------------------------------------------------------------
	BOOL IsQuickAccessControl(CXTPControl* pControl) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if control can be added to quick access controls.
	// Parameters:
	//     pControl - Control to test
	// Returns:
	//     TRUE if control can be added in quick access area.
	//-----------------------------------------------------------------------
	virtual BOOL IsAllowQuickAccessControl(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the command bar in the given context.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rcClipBox - The rectangular area of the control that is invalid
	//-----------------------------------------------------------------------
	virtual void DrawCommandBar(CDC* pDC, CRect rcClipBox);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns CXTPOffice2007FrameHook hook window used to skin frame.
	// See Also: EnableFrameTheme
	//-----------------------------------------------------------------------
	CXTPOffice2007FrameHook* GetFrameHook() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Vista Glass Effect enabled for Ribbon Bar.
	//-----------------------------------------------------------------------
	BOOL IsDwmEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom font height for ribbon bar
	// Parameters:
	//     nFontHeight - Font height to set
	//-----------------------------------------------------------------------
	void SetFontHeight(int nFontHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines font height used for Ribbon Bar
	//-----------------------------------------------------------------------
	int GetFontHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if caption part of ribbon is visible
	//-----------------------------------------------------------------------
	BOOL IsCaptionVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method allow show/hide caption if frame theme not enabled.
	//-----------------------------------------------------------------------
	void ShowCaptionAlways(BOOL bShowCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the size of a toolbar icon.
	// See Also:
	//     GetLargeIconSize
	//-----------------------------------------------------------------------
	virtual CSize GetIconSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get pointer to quick access button
	// Returns:
	//     Pointer to quick access button
	//-----------------------------------------------------------------------
	CXTPControl* GetControlQuickAccess() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This virtual method called when tab is about to be changed.
	// Parameters:
	//     pTab - Tab to be selected
	// Returns:
	//     returns TRUE to cancel tab changing, FALSE to allow.
	//-----------------------------------------------------------------------
	virtual BOOL OnTabChanging(CXTPRibbonTab* pTab);

	//-----------------------------------------------------------------------
	// Summary:
	//     This virtual method called when current tab is changed.
	// Parameters:
	//     pTab - New selected tab
	//-----------------------------------------------------------------------
	virtual void OnTabChanged(CXTPRibbonTab* pTab);

//{{AFX_CODEJOCK_PRIVATE
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to assign self identifiers for serialization process.
	// Parameters:
	//     nID             - Identifier to assign
	//     pCommandBarList - List of CommandBars.
	//     pParam          - Address of a XTP_COMMANDBARS_PROPEXCHANGE_PARAM structure.
	//-----------------------------------------------------------------------
	void GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called in serialization process.to restore popups from list of command bars.
	// Parameters:
	//     pCommandBarList - List of CommandBars.
	//-----------------------------------------------------------------------
	void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList);

	void DoPropExchange(CXTPPropExchange* pPX);
	void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);
	virtual CXTPPopupBar* CreateContextMenu(CXTPControl* pSelectedControl);
	virtual CXTPPopupBar* CreateMoreQuickAccessContextMenu();
	virtual BOOL ShouldSerializeBar();
	virtual void MergeToolBar(CXTPCommandBar* pCommandBar, BOOL bSilent);
	int HitTestCaption(CPoint point) const;
	void RepositionCaptionButtons();
	void RepositionContextHeaders();
	void AddCaptionButton(int nId, BOOL bAdd, BOOL bEnabled, CRect& rcCaption);
	BOOL ShrinkContextHeaders(int nLeft, int nRight);
	void ShowContextMenu(CPoint point, CXTPControl* pSelectedControl);
	virtual void CreateKeyboardTips();
	virtual void OnKeyboardTip(CXTPCommandBarKeyboardTip* pTip);
	void RefreshSysButtons();

protected:

	public:
	virtual CSize CalcDockingLayout(int nLength, DWORD dwMode, int nWidth = 0);
	void Reposition(int cx, int cy);
	void RepositionGroups(CDC* pDC, CRect rcGroups);
	virtual BOOL PreviewAccel(UINT chAccel);
	virtual BOOL SetTrackingMode(int bMode, BOOL bSelectFirst = TRUE, BOOL bKeyboard = FALSE);
	int OnHookMessage(HWND /*hWnd*/, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& /*lResult*/);
	void OnRemoved();
	void OnGroupsScroll(BOOL bScrollLeft);
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()
	//{{AFX_VIRTUAL(CXTPRibbonBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPRibbonBar)
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void OnSysColorChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCustomizePlaceQuickAccess(UINT nCommand);
	afx_msg LRESULT OnCustomizeCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);

	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:

protected:
	CRect m_rcTabControl;           // Bounding rectangle of Tabs
	CRect m_rcGroups;               // Groups bounding rectangle
	CRect m_rcCaption;              // Caption bounding rectangle
	CRect m_rcCaptionText;          // Caption text bounding rectangle
	CRect m_rcHeader;               // Header bounding rectangle
	CRect m_rcQuickAccess;          // Quick access bounding rectangle

	BOOL m_bRibbonBarVisible;       // TRUE if ribbon bar visible
	BOOL m_bGroupsVisible;          // TRUE if groups is visible
	BOOL m_bTabsVisible;            // TRUE if tabs are visible
	BOOL m_bShowQuickAccessBelow;   // TRUE to show quick access controls below ribbon bar
	BOOL m_bShowQuickAccess;        // TRUE to show quick access
	BOOL m_bAllowQuickAccessDuplicates;         // TRUE to allow quick access duplicates
	BOOL m_bAllowQuickAccessCustomization;      // TRUE to allow quick access customization
	BOOL m_bGroupReducedChanged;    // Some group appear in resize handler
	int m_nQuickAccessHeight;           // Quick Access height
	BOOL m_bAllowMinimize;          // TRUE to allow minimize ribbon

	CXTPRibbonQuickAccessControls* m_pQuickAccessControls;      // Quick AccessControls (not used now)
	CXTPRibbonControlTab* m_pControlTab;        // Control tab pointer
	CXTPControl* m_pControlQuickAccess;         // Quick Access menu control
	CXTPControl* m_pControlQuickAccessMore;     // Quick Access more control
	CXTPControlPopup* m_pControlSystemButton;          // System button control

	CXTPRibbonGroup* m_pHighlightedGroup;       // Currently highlighted group
	int m_nMinVisibleWidth;                     // Minimum width before ribbon bar disappears

	CXTPOffice2007FrameHook* m_pFrameHook;      // Office 2007 Frame hook

	CXTPRibbonTabContextHeaders* m_pContextHeaders;     // Context header collection
	int m_nGroupsHeight;            // Custom groups height

	BOOL m_bMinimized;              // TRUE if Ribbon currently minimized
	BOOL m_bShowCaptionAlways;      // TRUE to show caption even if EnableFrameTheme was not called.
	CString m_strCaptionText;       // Caption text

private:
	friend class CXTPRibbonControlTab;
	friend class CXTPRibbonBarControlQuickAccessPopup;
	friend class CControlQuickAccessMorePopup;
	friend class CXTPRibbonGroup;
	friend class CXTPRibbonControls;
	friend class CXTPRibbonGroups;
};


AFX_INLINE CRect CXTPRibbonBar::GetTabControlRect() const {
	return m_rcTabControl;
}
AFX_INLINE CXTPRibbonGroup* CXTPRibbonBar::GetHighlightedGroup() const {
	return m_pHighlightedGroup;
}
AFX_INLINE CXTPRibbonControlTab* CXTPRibbonBar::GetControlTab() const {
	return m_pControlTab;
}
AFX_INLINE void CXTPRibbonBar::SetMinimumVisibleWidth(int nMinVisibleWidth) {
	m_nMinVisibleWidth = nMinVisibleWidth;
}
AFX_INLINE int CXTPRibbonBar::GetMinimumVisibleWidth() const {
	return m_nMinVisibleWidth;
}
AFX_INLINE CRect CXTPRibbonBar::GetCaptionRect() const {
	return m_rcCaption;
}
AFX_INLINE CRect CXTPRibbonBar::GetCaptionTextRect() const {
	return m_rcCaptionText;
}
AFX_INLINE CRect CXTPRibbonBar::GetQuickAccessRect() const {
	return m_rcQuickAccess;
}
AFX_INLINE CXTPRibbonQuickAccessControls* CXTPRibbonBar::GetQuickAccessControls() const {
	return m_pQuickAccessControls;
}
AFX_INLINE CXTPControlPopup* CXTPRibbonBar::GetSystemButton() const {
	return m_pControlSystemButton;
}
AFX_INLINE BOOL CXTPRibbonBar::IsRibbonBarVisible() const {
	return m_bVisible && m_bRibbonBarVisible;
}
AFX_INLINE CXTPRibbonTabContextHeaders* CXTPRibbonBar::GetContextHeaders() const {
	return m_pContextHeaders;
}
AFX_INLINE BOOL CXTPRibbonBar::IsRibbonBar() const {
	return TRUE;
}
AFX_INLINE void CXTPRibbonBar::ShowQuickAccess(BOOL bShow) {
	m_bShowQuickAccess = bShow;
	OnRecalcLayout();
}
AFX_INLINE BOOL CXTPRibbonBar::IsQuickAccessVisible() const {
	return m_bShowQuickAccess;
}
AFX_INLINE void CXTPRibbonBar::AllowQuickAccessDuplicates(BOOL bAllow) {
	m_bAllowQuickAccessDuplicates = bAllow;
}
AFX_INLINE void CXTPRibbonBar::AllowQuickAccessCustomization(BOOL bAllow) {
	m_bAllowQuickAccessCustomization = bAllow;
}
AFX_INLINE BOOL CXTPRibbonBar::IsAllowQuickAccessDuplicates() const {
	return m_bAllowQuickAccessDuplicates;
}
AFX_INLINE void CXTPRibbonBar::SetGroupsHeight(int nHeight) {
	m_nGroupsHeight = nHeight;
}
AFX_INLINE CXTPOffice2007FrameHook* CXTPRibbonBar::GetFrameHook() const {
	return m_pFrameHook;
}
AFX_INLINE void CXTPRibbonBar::AllowMinimize(BOOL bAllow) {
	m_bAllowMinimize = bAllow;
}
AFX_INLINE CRect CXTPRibbonBar::GetGroupsRect() const {
	return m_rcGroups;
}
AFX_INLINE void CXTPRibbonBar::ShowCaptionAlways(BOOL bShowCaption) {
	m_bShowCaptionAlways = bShowCaption;
}
AFX_INLINE CXTPControl* CXTPRibbonBar::GetControlQuickAccess() const {
	return m_pControlQuickAccess;
}

#endif // !defined(__XTPRIBBONBAR_H__)
