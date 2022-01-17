// XTPShortcutBar.h interface for the CXTPShortcutBar class.
//
// This file is a part of the XTREME SHORTCUTBAR MFC class library.
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
#if !defined(__XTPSHORTCUTBAR_H__)
#define __XTPSHORTCUTBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPShortcutBarPaintManager.h"

//-----------------------------------------------------------------------
// Summary:
//     XTP_SBN_SELECTION_CHANGED is used to indicate that the selection
//     has changed in the shortcut bar.
// Remarks:
//     XTP_SBN_SELECTION_CHANGED is sent in the XTPWM_SHORTCUTBAR_NOTIFY
//     message to the owner window when the selection has changed.
//
//     The selection changed when a user clicks on a shortcut bar item or
//     the items are navigated with the arrow keys.
// Example:
//   See example of XTPWM_SHORTCUTBAR_NOTIFY
// See Also:
//     XTPWM_SHORTCUTBAR_NOTIFY, XTP_SBN_RCLICK
//-----------------------------------------------------------------------
const UINT XTP_SBN_SELECTION_CHANGING  = 1;

//-----------------------------------------------------------------------
// Summary:
//     XTP_SBN_SELECTION_CHANGING is used to indicate that value for the selection
//     is currently changing.
// Remarks:
//     XTP_SBN_SELECTION_CHANGING is sent in the XTPWM_SHORTCUTBAR_NOTIFY
//     message to the owner window when the selection is in the process of changing.
//
//     The selection changed when a user clicks on a shortcut bar item or
//     the items are navigated with the arrow keys.
// See Also:
//     XTPWM_SHORTCUTBAR_NOTIFY, XTP_SBN_RCLICK
//-----------------------------------------------------------------------
const UINT XTP_SBN_SELECTION_CHANGED = 3;

//-----------------------------------------------------------------------
// Summary:
//     XTP_SBN_RCLICK is used to indicate that the user has pressed the
//     right mouse button on a shortcut bar item.
// Remarks:
//     XTP_SBN_RCLICK is sent in the XTPWM_SHORTCUTBAR_NOTIFY message to the
//     owner window when the user has right clicked on a shortcut bar item.
// Example:
//   See example of XTPWM_SHORTCUTBAR_NOTIFY
// See Also:
//     XTPWM_SHORTCUTBAR_NOTIFY, XTP_SBN_SELECTION_CHANGING
//-----------------------------------------------------------------------
const UINT XTP_SBN_RCLICK = 2;


//-----------------------------------------------------------------------
// Summary:
//     The XTPWM_SHORTCUTBAR_NOTIFY message is sent to the CXTPShortcutBar owner window
//     whenever an action occurs within the CXTPShortcutBar
// Parameters:
//     nAction -  Value of wParam specifies a ShortcutBar value that indicates the user's
//                request.
//     pItem    - The value of lParam points to an CXTPShortcutBarItem object that contains information for the
//                specified item. This pointer should <b>never</b> be NULL.
// Remarks:
//     nAction parameter can be one of the following values:
//         * <b>XTP_SBN_SELECTION_CHANGING</b> Indicates the selection has changed in the shortcut bar.
//         * <b>XTP_SBN_RCLICK</b> Indicates the user pressed the right mouse button on the shortcut bar item.
//
// Returns:
//     If the application is to process this message, the return value should be TRUE, otherwise the
//     return value is FALSE.
// Example:
//     Here is an example of how an application would process the XTPWM_SHORTCUTBAR_NOTIFY
//     message.
// <code>
//
// BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//     //{{AFX_MSG_MAP(CMainFrame)
//     ON_MESSAGE(XTPWM_SHORTCUTBAR_NOTIFY, OnShortcutBarNotify)
//     //}}AFX_MSG_MAP
// END_MESSAGE_MAP()
//
// LRESULT CMainFrame::OnShortcutBarNotify(WPARAM wParam, LPARAM lParam)
// {
//      switch (wParam)
//      {
//          case XTP_SBN_SELECTION_CHANGING:
//              {
//                  CXTPShortcutBarItem* pItem = (CXTPShortcutBarItem*)lParam;
//                  TRACE(_T("Selection Changing. Item.Caption = %s\n"), pItem->GetCaption());
//
//                  // TODO: You can return -1 to ignore changing
//
//              }
//              return TRUE;
//          case XTP_SBN_RCLICK:
//              {
//                  CPoint point(lParam);
//                  CXTPShortcutBarItem* pItem = m_wndShortcutBar.HitTest(point);
//                  if (pItem)
//                  {
//                      TRACE(_T("RClick. Item.ID = %i\n"), pItem->GetID());
//
//                      CMenu mnu;
//                      mnu.LoadMenu(IDR_POPUP_MENU);
//
//                      m_wndShortcutBar.ClientToScreen(&point);
//
//                      CXTPCommandBars::TrackPopupMenu(mnu.GetSubMenu(0), 0, point.x, point.y, AfxGetMainWnd());
//
//                  }
//              }
//              return TRUE;
//
//
//      }
//      return 0;
//  }
// </code>
// See Also:
//     XTP_SBN_SELECTION_CHANGING, XTP_SBN_RCLICK, CXTPShortcutBar
//-----------------------------------------------------------------------
const UINT XTPWM_SHORTCUTBAR_NOTIFY = (WM_USER + 9190 + 1);


class CXTPShortcutBar;
class CXTPImageManager;
class CXTPToolTipContext;

//===========================================================================
// Summary:
//     CXTPShortcutBarItem is the base class representing an item of the
//     ShortcutBar Control.
//===========================================================================
class _XTP_EXT_CLASS CXTPShortcutBarItem : public CXTPCmdTarget
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPShortcutBarItem object.
	// Parameters:
	//     pShortcutBar - Points to the parent ShortcutBar class.
	//     nID - Identifier of the item.
	//     pWnd - Client window of the item
	//     pShortcutBar - Points to the parent ShortcutBar class.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem(CXTPShortcutBar* pShortcutBar);
	CXTPShortcutBarItem(CXTPShortcutBar* pShortcutBar, UINT nID, CWnd* pWnd); // <combine CXTPShortcutBarItem::CXTPShortcutBarItem@CXTPShortcutBar*>

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the identifier of the item.
	// Parameters:
	//     nID - The new identifier of the item.
	//-----------------------------------------------------------------------
	void SetID(int nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the identifier of the item.
	// Returns:
	//     Identifier of the item.
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the icon's identifier.
	// Parameters:
	//     nId - Icon's identifier to be set.
	//-----------------------------------------------------------------------
	void SetIconId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon's identifier.
	// Returns:
	//     An icon's identifier of the item.
	//-----------------------------------------------------------------------
	int GetIconId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the caption of the item.
	// Parameters:
	//     strCaption - The new caption of the item.
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR strCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the caption of the item.
	// Returns:
	//     A CString object containing the caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the visibility state of the item.
	// Parameters:
	//     bVisible - TRUE if the item is visible; otherwise FALSE.
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the item is visible.
	// Returns:
	//     TRUE if the item is visible; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Associates ToolTip text with the item.
	// Parameters:
	//     strTooltip - The ToolTip text to display when the mouse cursor is over the item.
	//-----------------------------------------------------------------------
	void SetTooltip(LPCTSTR strTooltip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the ToolTip text associated with the item.
	// Returns:
	//     The ToolTip text for the item.
	//-----------------------------------------------------------------------
	CString GetTooltip() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the item is expanded.
	// Returns:
	//     TRUE if the item is expanded; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsExpanded() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the item is expand button.
	// Returns:
	//     TRUE if the item is expand button; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsItemExpandButton() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the parent ShortcutBar class.
	// Returns:
	//     A Pointer to the parent CXTPShortcutBar class.
	//-----------------------------------------------------------------------
	CXTPShortcutBar* GetShortcutBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the dimensions of the rectangle that bounds the item.
	// Returns:
	//     CRect object contains the bounding rectangle of the item.
	//-----------------------------------------------------------------------
	CRect GetItemRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the item.
	// Parameters:
	//     dwData - Contains the new value to associate with the item.
	//-----------------------------------------------------------------------
	void SetItemData(DWORD_PTR dwData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value associated with the item
	// Returns:
	//     The 32-bit value associated with the item
	//-----------------------------------------------------------------------
	DWORD_PTR GetItemData() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the image of this shortcut bar item.
	// Parameters:
	//     nWidth - Width of the icon to be retrieved. (CXTPImageManagerIcon
	//              can have multiple image sizes for the same item, the
	//              shortcut bar uses both 16x16 and 24x24 images depending on
	//              if the item is displayed in the extended shortcut list).
	//              If a 16x16 icon is to be retrieved, then pass in 16 for
	//              the width.
	// Returns:
	//     A pointer the CXTPImageManagerIcon object for this shortcut bar
	//     item.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetImage(int nWidth) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine is the ShortcutBarItem is hidden.
	//     An item is hidden when there are more items then can be displayed
	//     in the bottom shortcut bar.  As the shortcut bar is expanded, items
	//     that were hidden will become visible as more items are added to the
	//     expanded shortcut bar.
	// Returns:
	//     TRUE is the ShortcutBarItem is hidden, otherwise FALSE
	//-----------------------------------------------------------------------
	BOOL IsHidden() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the item is currently selected.
	//     Note that multiple items can be selected at one time, but only one
	//     can have focus.
	// Returns:
	//     TRUE if the item is currently selected.
	//-----------------------------------------------------------------------
	BOOL IsSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select this item.  Multiple items can have
	//     focus if CXTPShortcutBar::m_bSingleSelection is FALSE.
	// Parameters:
	//     bSelected - TRUE if the item will be selected, FALSE to no longer
	//                 select the item.
	//-----------------------------------------------------------------------
	void SetSelected(BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get child window of the item.
	// Returns:
	//     CWnd pointer to child window
	//-----------------------------------------------------------------------
	CWnd* GetClientWindow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set child window that will be visible
	//     in client part of the shortcutbar when item is selected.
	// Parameters:
	//     pWnd - New child window to be set.
	//-----------------------------------------------------------------------
	void SetClientWindow(CWnd* pWnd);

protected:
	CString m_strCaption;               // Caption for the item.
	CString m_strTooltip;               // Tooltip for the item.
	int m_nID;                          // Item's identifier.
	CRect m_rcItem;                     // Bounding rectangle of the item
	BOOL m_bVisible;                    // TRUE if the item is visible
	BOOL m_bExpanded;                   // TRUE if the item is expanded
	BOOL m_bHidden;                     // TRUE if the item is hidden
	HWND m_hwndChild;                   // Child window associated with the item
	BOOL m_bExpandButton;               // True if the item is expand button
	DWORD_PTR m_dwData;                 // The 32-bit value associated with the item
	CXTPShortcutBar* m_pShortcutBar;    // Parent CXTPShortcutBar class
	BOOL m_bSelected;                   // TRUE if item selected
	int m_nIconId;                      // Identifier of the item's image.

private:

	friend class CXTPShortcutBar;
};

//-----------------------------------------------------------------------
// Summary:
//     CXTPShortcutBar is a CWnd derived class. It is used to implement
//     an Outlook2003 ShortcutBar style control.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPShortcutBar : public CWnd
{
	//-----------------------------------------------------------------------
	// Summary:
	//     Array of CXTPShortcutBarItem objects
	//-----------------------------------------------------------------------
	typedef CArray<CXTPShortcutBarItem*, CXTPShortcutBarItem*> CShortcutArray;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPShortcutBar object.
	//-----------------------------------------------------------------------
	CXTPShortcutBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPShortcutBar object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPShortcutBar();


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method creates the ShortcutBar control
	// Parameters:
	//     dwStyle - Style for the ShortcutBar.
	//     rect -  Specifies the size and position of the ShortcutBar control.
	//     pParentWnd - Specifies the parent window of the ShortcutBar control.
	//     nID - Specifies the ShortcutBar control ID.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Register the window class if it has not already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     TRUE if the window class was successfully registered.
	//-----------------------------------------------------------------------
	BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve state information from the registry or .INI file.
	// Parameters:
	//     lpszProfileName - Points to a null-terminated string that specifies the name of a
	//     section in the initialization file or a key in the Windows registry where state
	//     information is stored.
	//-----------------------------------------------------------------------
	void LoadState(LPCTSTR lpszProfileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to save the state information to the registry or .INI file.
	// Parameters:
	//     lpszProfileName - Points to a null-terminated string that specifies the name of a
	//     section in the initialization file or a key in the Windows registry where state
	//     information is stored.
	//-----------------------------------------------------------------------
	void SaveState(LPCTSTR lpszProfileName);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add a new item.
	// Parameters:
	//     nID - Identifier of item that is to be added.
	//     pWnd - A Pointer to CWnd class associated with new item.
	// Returns:
	//     A pointer to a CXTPShortcutBarItem object.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* AddItem(UINT nID, CWnd* pWnd = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to find an item with the specified identifier.
	// Parameters:
	//     nID - Identifier of the item needed to find.
	// Returns:
	//     A pointer to a CXTPShortcutBarItem object.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* FindItem(int nID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to select the specified item.
	// Parameters:
	//     pItem - Item to be selected.
	//-----------------------------------------------------------------------
	void SelectItem(CXTPShortcutBarItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show/hide expand button
	// Parameters:
	//     bShow - TRUE to show; FALSE to hide
	//-----------------------------------------------------------------------
	void ShowExpandButton(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if expand button visible
	// Returns:
	//     TRUE if expand button is visible; otherwise FALSE;
	//-----------------------------------------------------------------------
	BOOL IsExpandButtonVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove the item.
	// Parameters:
	//     nID - Identifier of the item to delete.
	//-----------------------------------------------------------------------
	void RemoveItem(int nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all items.
	//-----------------------------------------------------------------------
	void RemoveAllItems();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves an item with specified index.
	// Parameters:
	//     nIndex - Specifies the zero-based index of the item to retrieve.
	// Returns:
	//     A pointer to a CXTPShortcutBarItem object.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* GetItem(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines where a point lies in a specified item.
	// Parameters:
	//     pt - Specifies the point to be tested.
	// Returns:
	//     A pointer to a CXTPShortcutBarItem item that occupies the specified point or NULL
	//     if no item occupies the point.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* HitTest(CPoint pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the minimum height of the client area in a ShortcutBar control.
	//     The default value is 200.
	// Parameters:
	//     nMinHeight - Specifies the minimum height in pixels of the client.
	//-----------------------------------------------------------------------
	void SetMinimumClientHeight(int nMinHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine gripper rectangle.
	// Returns:
	//     CRect object contains gripper bounding rectangle.
	//-----------------------------------------------------------------------
	CRect GetGripperRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get size of items
	// Returns:
	//     CSize object contains size of items
	//-----------------------------------------------------------------------
	CSize GetItemSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set size of items
	// Parameters:
	//     szItem - Size of the item.
	//-----------------------------------------------------------------------
	void SetItemSize(CSize szItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines the count of visible lines in the ShortcutBar control.
	// Returns:
	//     Count of visible lines
	// See Also:
	//     SetExpandedLinesCount
	//-----------------------------------------------------------------------
	int GetExpandedLinesCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines the height of visible lines in the ShortcutBar control.
	// Returns:
	//     Height of visible lines
	// See Also:
	//     SetExpandedLinesHeight
	//-----------------------------------------------------------------------
	int GetExpandedLinesHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the count of lines that are visible in the ShortcutBar control.
	// Parameters:
	//     nCount - Number of visible lines.
	// See Also:
	//     GetExpandedLinesCount
	//-----------------------------------------------------------------------
	void SetExpandedLinesCount(int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the the height of  lines that are visible in the ShortcutBar control.
	// Parameters:
	//     nHeight - Height of visible lines.
	// See Also:
	//     SetExpandedLinesCount, GetExpandedLinesCount
	//-----------------------------------------------------------------------
	void SetExpandedLinesHeight(int nHeight);


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable/disable resize client area
	// Parameters:
	//     bAllowResize - TRUE to allow resize; FALSE to disable
	// See Also:
	//     SetExpandedLinesCount
	//-----------------------------------------------------------------------
	void AllowGripperResize(BOOL bAllowResize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the hot item in the ShortcutBar control.
	// Returns:
	//     Pointer to the hot item.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* GetHotItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the selected item in the ShortcutBar control.
	// Returns:
	//     Pointer to the selected item.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* GetSelectedItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow only one item to be selected.
	// Parameters:
	//     bSingleSelection - TRUE to use single selection.
	// See Also: IsSingleSelection, SetSelectItemOnFocus
	//-----------------------------------------------------------------------
	void SetSingleSelection(BOOL bSingleSelection = TRUE);


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get Single Selection property of control.
	// See Also: SetSingleSelection, SetSelectItemOnFocus
	//-----------------------------------------------------------------------
	BOOL IsSingleSelection() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show active item on top of controls
	// Parameters:
	//     bActiveItemOnTop - TRUE to show active item on top of controls
	// See Also: IsShowActiveItemOnTop
	//-----------------------------------------------------------------------
	void ShowActiveItemOnTop(BOOL bActiveItemOnTop);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get ShowActiveItemOnTop property of control.
	// See Also: ShowActiveItemOnTop
	//-----------------------------------------------------------------------
	BOOL IsShowActiveItemOnTop() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow resize gripper by pixel
	// Parameters:
	//     bAllowFreeResize - TRUE to allow resize gripper by pixel
	// See Also: IsShowActiveItemOnTop
	//-----------------------------------------------------------------------
	void AllowFreeResize(BOOL bAllowFreeResize);

	// Summary:
	//     Determines if gripper allowed to resize by pixel.
	// See Also: ShowActiveItemOnTop
	//-----------------------------------------------------------------------
	BOOL IsAllowFreeResize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow collapse items
	// Parameters:
	//     bAllowCollapse - TRUE to allow collapse items
	//-----------------------------------------------------------------------
	void AllowCollapse(BOOL bAllowCollapse);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show gripper.
	// Parameters:
	//     bShowGripper - TRUE to show gripper; FALSE to hide
	//-----------------------------------------------------------------------
	void ShowGripper(BOOL bShowGripper);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the pressed item in the ShortcutBar control.
	// Returns:
	//     Pointer to the pressed item.
	//-----------------------------------------------------------------------
	CXTPShortcutBarItem* GetPressedItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines visibility of client pane.
	// Returns:
	//     TRUE if client pane is visible.
	//-----------------------------------------------------------------------
	BOOL IsClientPaneVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to show or hide client pane.
	// Parameters:
	//     bVisible - TRUE to show client pane; FALSE to hide.
	//-----------------------------------------------------------------------
	void SetClientPaneVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set new image manager.
	// Parameters:
	//     pImageManager - Points to a CXTPImageManager object to be set
	// Example:
	// <code>
	//     CXTPImageManager* pImageManager = new CXTPImageManager();
	//     pImageManager->SetIcons(IDR_MAINFRAME);
	//     m_wndShortcutBar.SetImageManager(pImageManager);
	// </code>
	// See Also:
	//     GetImageManager
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get a pointer to the image manager of shortcut
	//     bar control.
	// Returns:
	//     Pointer to the image manager of shortcut bar control.
	// Remarks:
	//     The image manager is used to hold all of the icons displayed in the
	//     shortcut bar control.
	// See Also:
	//     SetImageManager
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine how many CXTPShortcutBarItem(s) are int he
	//     shortcut bar.
	// Returns:
	//     Number of shortcut bar items in the shortcut bar.
	//-----------------------------------------------------------------------
	int GetItemCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set right-to-left mode.
	// Parameters:
	//     bRightToLeft - TRUE to set right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	void SetLayoutRTL(BOOL bRightToLeft);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves bounding rectangle of shortcut bar client
	// Returns:
	//     Bounding rectangle of shortcut bar client
	//-----------------------------------------------------------------------
	CRect GetClientPaneRect() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the visual theme of the ShortcutBar control.
	// Parameters:
	//     paintTheme - New visual theme. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     paintTheme can be one of the following:
	//     * <b>xtpShortcutThemeOffice2000</b> Enables Office 2000 style theme.
	//     * <b>xtpShortcutThemeOfficeXP</b> Enables Office XP style theme.
	//     * <b>xtpShortcutThemeOffice2003</b> Enables Office 2003 style theme.
	//
	// See Also:
	//     GetCurrentTheme, SetCustomTheme, XTPShortcutBarPaintTheme
	//-----------------------------------------------------------------------
	void SetTheme(XTPShortcutBarPaintTheme paintTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set a custom theme.
	// Parameters:
	//     pPaintManager - New paint manager.
	// See Also:
	//     GetCurrentTheme, SetTheme, XTPShortcutBarPaintTheme
	//-----------------------------------------------------------------------
	void SetCustomTheme(CXTPShortcutBarPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the current paint manager.
	// Returns:
	//     The current paint manager.
	//-----------------------------------------------------------------------
	CXTPShortcutBarPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the current visual theme.
	// Returns:
	//     The current theme.
	// See Also:
	//     SetTheme, SetCustomTheme, XTPShortcutBarPaintTheme
	//-----------------------------------------------------------------------
	XTPShortcutBarPaintTheme GetCurrentTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to redraw control.
	//-----------------------------------------------------------------------
	void RedrawControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tooltip context pointer.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine the number of visible items
	// Returns:
	//     Number of visible items
	//-----------------------------------------------------------------------
	int GetVisibleItemsCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine the number of collapsed items
	// Returns:
	//     Number of collapsed items
	//-----------------------------------------------------------------------
	int GetCollapsedItemsCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to reposition items.
	//-----------------------------------------------------------------------
	void Reposition();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when reposition is done.
	//-----------------------------------------------------------------------
	virtual void OnRepositionDone();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when expand button is pressed.
	// Parameters:
	//     pExpandButton - A pointer to expand button.
	//-----------------------------------------------------------------------
	virtual void OnExpandButtonDown(CXTPShortcutBarItem* pExpandButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set hot item.
	// Parameters:
	//     pItem - Item to be hot.
	//-----------------------------------------------------------------------
	void SetHotItem(CXTPShortcutBarItem* pItem);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPShortcutBar)
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	void PreSubclassWindow();
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPShortcutBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnMouseLeave();
	afx_msg void OnSysColorChange();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point) ;
	afx_msg LRESULT OnExpandPopupExecute(WPARAM wparam, LPARAM lParam);
	afx_msg void OnShowMoreButtons();
	afx_msg void OnShowFewerButtons();
	afx_msg void OnUpdateShowMoreButtons(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowFewerButtons(CCmdUI* pCmdUI);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	CXTPShortcutBarPaintManager* m_pPaintManager;   // Current paint manager.
	CXTPImageManager* m_pImageManager;      // Current image manager.
	int m_nExpandedLines;                   // Number of lines currently visible
	int m_nDesiredExpandedLinesHeight;      // Desired visible lines height height.
	int m_nMinClientHeight;                 // The height of client area.
	BOOL m_bAllowResize;                    // TRUE to allow resize
	XTPShortcutBarPaintTheme m_paintTheme;  // Current theme.
	CShortcutArray m_arrItems;              // Array of items.
	CSize m_szItem;                         // Size of items
	CRect m_rcClient;                       // Client's rectangle.
	CRect m_rcGripper;                      // Gripper's rectangle.
	HWND m_hwndClient;                      // Client window handle
	HCURSOR m_hSizeCursor;                  // Handle to the cursor displayed for the size icon.
	BOOL m_bTrackingSize;                   // TRUE if control in tracking mode.
	BOOL m_bShowActiveItemOnTop;            // TRUE to show active item on top
	BOOL m_bAllowFreeResize;                // TRUE to allow resize gripper by pixel;
	BOOL m_bAllowCollapse;                  // TRUE to allow collapse items.
	BOOL m_bShowGripper;                    // TRUE to show gripper.

	CXTPShortcutBarItem* m_pHotItem;        // Pointer to hot item.
	CXTPShortcutBarItem* m_pSelectedItem;   // Pointer to selected item.
	CXTPShortcutBarItem* m_pPressedItem;    // Pointer to pressed item.

	BOOL m_bClientPaneVisible;              // TRUE is client Pane in ShortcutBar is visible
	HCURSOR m_hHandCursor;                  // Hand cursor that is displayed when the cursor is positioned over a shortcut bar item.
	BOOL m_bSingleSelection;                // TRUE to use single selected items.
	CXTPToolTipContext* m_pToolTipContext;  // Tooltip Context.
	BOOL m_bPreSubclassWindow;              // True if PreSubclassWindow was called


	friend class CXTPShortcutBarItem;
	friend class CShortcutBarCtrl;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CString CXTPShortcutBarItem::GetCaption() const{
	return m_strCaption;
}
AFX_INLINE CString CXTPShortcutBarItem::GetTooltip() const{
	return m_strTooltip;
}
AFX_INLINE BOOL CXTPShortcutBarItem::IsItemExpandButton() const{
	return m_bExpandButton;
}
AFX_INLINE BOOL CXTPShortcutBarItem::IsExpanded() const {
	return m_bExpanded;
}
AFX_INLINE CXTPShortcutBar* CXTPShortcutBarItem::GetShortcutBar() const {
	return m_pShortcutBar;
}
AFX_INLINE CRect CXTPShortcutBarItem::GetItemRect() const {
	return m_rcItem;
}
AFX_INLINE void CXTPShortcutBarItem::SetItemData(DWORD_PTR dwData) {
	m_dwData = dwData;
}
AFX_INLINE DWORD_PTR CXTPShortcutBarItem::GetItemData() const {
	return m_dwData;
}
AFX_INLINE CWnd* CXTPShortcutBarItem::GetClientWindow() const {
	return CWnd::FromHandle(m_hwndChild);
}

AFX_INLINE CXTPShortcutBarItem* CXTPShortcutBar::GetHotItem() const {
	return m_pHotItem;
}
AFX_INLINE CXTPShortcutBarItem* CXTPShortcutBar::GetSelectedItem() const {
	return m_pSelectedItem;
}
AFX_INLINE CXTPShortcutBarItem* CXTPShortcutBar::GetPressedItem() const {
	return m_pPressedItem;
}
AFX_INLINE CRect CXTPShortcutBar::GetGripperRect() const {
	return m_rcGripper;
}
AFX_INLINE CSize CXTPShortcutBar::GetItemSize() const {
	return m_szItem;
}
AFX_INLINE void CXTPShortcutBar::SetItemSize(CSize szItem) {
	m_szItem = szItem;
}
AFX_INLINE int CXTPShortcutBar::GetExpandedLinesCount() const{
	return m_nExpandedLines;
}
AFX_INLINE BOOL CXTPShortcutBar::IsClientPaneVisible() const{
	return m_bClientPaneVisible;
}
AFX_INLINE void CXTPShortcutBar::SetClientPaneVisible(BOOL bVisible) {
	m_bClientPaneVisible = bVisible;
	Reposition();
}
AFX_INLINE CXTPShortcutBarPaintManager* CXTPShortcutBar::GetPaintManager() const{
	return m_pPaintManager;
}
AFX_INLINE void CXTPShortcutBar::SetMinimumClientHeight(int nMinHeight) {
	m_nMinClientHeight = nMinHeight;
}
AFX_INLINE XTPShortcutBarPaintTheme CXTPShortcutBar::GetCurrentTheme() const{
	return m_paintTheme;
}
AFX_INLINE BOOL CXTPShortcutBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE void CXTPShortcutBar::AllowGripperResize(BOOL bAllowResize) {
	m_bAllowResize = bAllowResize;
}
AFX_INLINE void CXTPShortcutBar::SetSingleSelection(BOOL bSingleSelection) {
	m_bSingleSelection = bSingleSelection;
	Reposition();
}
AFX_INLINE BOOL CXTPShortcutBar::IsSingleSelection() const{
	return m_bSingleSelection;
}
AFX_INLINE void CXTPShortcutBar::ShowActiveItemOnTop(BOOL bActiveItemOnTop) {
	m_bShowActiveItemOnTop = bActiveItemOnTop;
	Reposition();
}
AFX_INLINE BOOL CXTPShortcutBar::IsShowActiveItemOnTop() const {
	return m_bShowActiveItemOnTop;
}
AFX_INLINE void CXTPShortcutBar::AllowFreeResize(BOOL bAllowFreeResize) {
	m_bAllowFreeResize = bAllowFreeResize;
	Reposition();
}
AFX_INLINE BOOL CXTPShortcutBar::IsAllowFreeResize() const {
	return m_bAllowFreeResize;
}
AFX_INLINE void CXTPShortcutBar::AllowCollapse(BOOL bAllowCollapse) {
	m_bAllowCollapse = bAllowCollapse;
	Reposition();
}
AFX_INLINE void CXTPShortcutBar::ShowGripper(BOOL bShowGripper) {
	m_bShowGripper = bShowGripper;
	Reposition();
}
AFX_INLINE int CXTPShortcutBar::GetExpandedLinesHeight() const {
	return m_nDesiredExpandedLinesHeight;
}
AFX_INLINE CRect CXTPShortcutBar::GetClientPaneRect() const {
	return m_rcClient;
}

#endif // !defined(__XTPSHORTCUTBAR_H__)
