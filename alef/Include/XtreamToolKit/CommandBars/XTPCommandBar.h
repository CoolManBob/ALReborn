// XTPCommandBar.h : interface for the CXTPCommandBar class.
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
#if !defined(__XTPCOMMANDBAR_H__)
#define __XTPCOMMANDBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/XTPSystemHelpers.h"
#include "XTPCommandBarsDefines.h"
#include "XTPHookManager.h"

class CXTPDockBar;
class CXTPControls;
class CXTPControl;
class CXTPControlPopup;
class CXTPCommandBars;
class CXTPCommandBar;
class CXTPCommandBarAnimation;
class CXTPPaintManager;
class CXTPImageManager;
class CXTPPropExchange;
struct XTP_COMMANDBARS_PROPEXCHANGE_PARAM;
class CXTPToolTipContext;
class CXTPCommandBarAnimation;
class CXTPCommandBarKeyboardTip;

//{{AFX_CODEJOCK_PRIVATE
#define XTP_TID_HOVER    112223
#define XTP_TID_EXPANDED_POPUP 112224
#define XTP_BOOL_DEFAULT 2
#define XTP_TID_SHOWKEYBOARDTIPS 0xCCCA
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPCommandBarList is a CList derived class. It is used as a collection
//     of CXTPCommandBar classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarList : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarList object
	// Parameters:
	//     pCommandBars - Parent CXTPCommandBars pointer
	//-----------------------------------------------------------------------
	CXTPCommandBarList(CXTPCommandBars* pCommandBars)
	{
		m_pCommandBars = pCommandBars;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBarList object, handles cleanup and
	//     deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPCommandBarList();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find the CommandBar with the specified
	//     identifier.
	// Parameters:
	//     nBarID - Identifier CommandBar to find.
	// Returns:
	//     A pointer to a CXTPCommandBar object.
	//-----------------------------------------------------------------------
	CXTPCommandBar* FindCommandBar(UINT nBarID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Commandbars object attached to list
	// Returns:
	//     Pointer to CXTPCommandBars parent object
	// See Also:
	//     CXTPCommandBars, CXTPCommandBar
	//-----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars() const { return m_pCommandBars; }

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns CommandBar in list by its index
	// Parameters:
	//     nIndex - Index of the commandbar to return
	// Returns:
	//     CommandBar in list by its index
	// See Also: GetCount
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Return count of commanbars in list
	// Returns:
	//     Commanbars count
	// See Also: GetAt
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add new commandbar to the list
	// Parameters:
	//      pCommandBar - CommandBar to be added
	// Returns:
	//      Pointer to just added CommandBar
	// See Also: GetAt, GetCount
	//-----------------------------------------------------------------------
	virtual CXTPCommandBar* Add(CXTPCommandBar* pCommandBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to check if CommandBar exists in the list
	// Parameters:
	//      pCommandBar - CommandBar to test
	// Returns:
	//      TRUE if CommandBar exists in the list
	//-----------------------------------------------------------------------
	BOOL Lookup(CXTPCommandBar* pCommandBar) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to clean the list
	// See Also: Remove
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove CommandBar from the list
	// Parameters:
	//     pCommandBar - CommandBar to be removed
	// See Also: RemoveAll
	//-----------------------------------------------------------------------
	void Remove(CXTPCommandBar* pCommandBar);

protected:
	CXTPCommandBars* m_pCommandBars;            // Parent CXTPCommandBars object
	CArray<CXTPCommandBar*, CXTPCommandBar*> m_arrBars;         // Array of CommandBars
};

//===========================================================================
// Summary:
//     CXTPCommandBar is a CWnd derived class. It represents the parent
//     class for command bars.
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBar : public CWnd, public CXTPHookManagerHookAble, public CXTPAccessible
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBar object
	//-----------------------------------------------------------------------
	CXTPCommandBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPCommandBar();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the tracking state.
	// Returns:
	//     TRUE if the command bar is in the tracking mode.
	//-----------------------------------------------------------------------
	int IsTrackingMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the controls collection.
	// Returns:
	//     A CXTPControls pointer.
	//-----------------------------------------------------------------------
	CXTPControls* GetControls() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to set controls collection to commandbar
	// Parameters:
	//     pControls - New controls to be set
	// See Also: GetControls
	//-----------------------------------------------------------------------
	void SetControls(CXTPControls* pControls);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the type of the command bar.
	// Returns:
	//     The type of the command bar. It can be one of the following:
	//     * <b>xtpBarTypeMenuBar</b> Indicates the command bar is menu bar (CXTPMenuBar)
	//     * <b>xtpBarTypeNormal</b> Indicates the command bar is simple toolbar (CXTPToolBar)
	//     * <b>xtpBarTypePopup</b> Indicates the command bar is popupbar (CXTPPopupBar)
	// See Also: XTPBarType, GetPosition
	//-----------------------------------------------------------------------
	XTPBarType GetType() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the position of the command bar.
	// Returns:
	//     The position of the command bar.
	// See Also: XTPBarPosition, GetType
	//-----------------------------------------------------------------------
	XTPBarPosition GetPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to recalculate the layout.
	//-----------------------------------------------------------------------
	void DelayLayout();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to redraw the command bar.
	//-----------------------------------------------------------------------
	virtual void DelayRedraw();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to change the tracking state.
	// Parameters:
	//     bMode        - TRUE to set the tracking mode; otherwise FALSE.
	//     bSelectFirst - TRUE to select the first item.
	//     bKeyboard    - TRUE if the item is popped by the keyboard.
	// Returns:
	//     TRUE if the method was successful.
	// See Also: IsTrackingMode.
	//-----------------------------------------------------------------------
	virtual BOOL SetTrackingMode(int bMode, BOOL bSelectFirst = TRUE, BOOL bKeyboard = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to convert menu items to command bar controls.
	// Parameters:
	//     pMenu           - Menu to be converted.
	//     bRemoveControls - TRUE to remove all controls first.
	// Returns:
	//     TRUE if the method was successful; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL LoadMenu(CMenu* pMenu, BOOL bRemoveControls = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the control at the specified index.
	// Parameters:
	//     nIndex - An integer index.
	// Returns:
	//     The CXTPControl pointer currently at this index.
	// See also: GetControls, CXTPControls::GetAt
	//-----------------------------------------------------------------------
	CXTPControl* GetControl(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the count of the controls
	// Returns:
	//     The count of the controls.
	//-----------------------------------------------------------------------
	int GetControlCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select the specified control.
	// Parameters:
	//     nSelected - An integer index of the item to be selected.
	//     bKeyboard - TRUE if the item was selected using the keyboard.
	// Returns:
	//     TRUE if the method was successful; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL SetSelected(int nSelected, BOOL bKeyboard = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to popup the specified control.
	// Parameters:
	//     nPopuped  - An integer index of the item to be popped.
	//     bKeyboard - TRUE if the item is popped by the keyboard.
	// Returns:
	//     TRUE if the method was successful; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetPopuped(int nPopuped, BOOL bKeyboard = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set a title for this command bar.
	// Parameters:
	//     lpszTitle - Title to be set.
	//-----------------------------------------------------------------------
	void SetTitle(LPCTSTR lpszTitle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a title for this command bar.
	// Returns:
	//     The title of the command bar.
	//-----------------------------------------------------------------------
	CString GetTitle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the command bar's object.
	// Returns:
	//     A CXTPCommandBars pointer (can be NULL).
	//-----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the command bar's object.
	// Parameters:
	//     pCommandBars - CXTPCommandBars to be set.
	//-----------------------------------------------------------------------
	void SetCommandBars(CXTPCommandBars* pCommandBars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the command bar's site.
	// Returns:
	//     Command bar's site (cannot be NULL).
	//-----------------------------------------------------------------------
	CWnd* GetSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the command bar's site as CFrameWnd.
	// Returns:
	//     The command bar's site (can be NULL).
	//-----------------------------------------------------------------------
	CFrameWnd* GetFrameSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the command bar's flags.
	// Parameters:
	//     dwFlagsAdd    - Flags to be added.
	//     dwFlagsRemove - Flags to be removed.
	// Remarks:
	//     dwFlagsAdd and dwFlagsRemove can be combination of XTPToolBarFlags and XTPMenuBarFlags<p/>
	//     Use the bitwise-OR (|) operator to combine the flags.<p/>
	//     * <b>xtpFlagAlignTop</b> -  Allows docking at the top of the client area.
	//     * <b>xtpFlagAlignBottom</b> -  Allows docking at the bottom of the client area.
	//     * <b>xtpFlagAlignLeft</b> -  Allows docking on the left side of the client area.
	//     * <b>xtpFlagAlignRight</b> -  Allows docking on the left side of the client area.
	//     * <b>xtpFlagAlignAny</b> -  Allows docking on any side of the client area.
	//     * <b>xtpFlagFloating</b> -  Allows floating.
	//     * <b>xtpFlagHideWrap</b> -  Allow to hide wrapped controls.
	//     * <b>xtpFlagStretched</b> -  Bar is stretched.
	//     * <b>xtpFlagStretchedShared</b> -  Bar is stretched, but not fill whole row
	//     * <b>xtpFlagSmartLayout</b> -  Smart layout is enabled
	//
	//     * <b>xtpFlagHideMinimizeBox</b> - To hide minimize box.
	//     * <b>xtpFlagHideMaximizeBox</b> - To hide maximize box.
	//     * <b>xtpFlagIgnoreSetMenuMessage</b> - To ignore MDI menus.
	//     * <b>xtpFlagHideClose</b> - To hide close button.
	//     * <b>xtpFlagHideMDIButtons</b> - Specifies to hide all MDI buttons displayed on the Command Bar.
	//     * <b>xtpFlagAddMDISysPopup</b> - To add system MDI popup bar.  Specifies to add system MDI popup menu to the Menu Bar.  This will appears as an icon to the far left of the Menu Bar and will display the MDI menu bar options when clicked.
	//
	// See Also: XTPMenuBarFlags, XTPToolBarFlags
	//-----------------------------------------------------------------------
	void SetFlags(DWORD dwFlagsAdd, DWORD dwFlagsRemove = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the controls from the button array.
	// Parameters:
	//     pButtons - Pointer to an array of command Ids. It cannot be NULL.
	//     nCount   - Number of elements in the array pointed to by pButtons.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// See Also: LoadToolBar, LoadMenu.
	//-----------------------------------------------------------------------
	BOOL SetButtons(UINT* pButtons, int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to load the toolbar specified by nIDResource.
	// Parameters:
	//     nIDResource - Resource ID of the toolbar to be loaded.
	//     bLoadIcons  - TRUE if the toolbar icons are to be used by CXTPImageManager.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// See Also: LoadMenu, SetButtons.
	//-----------------------------------------------------------------------
	virtual BOOL LoadToolBar(UINT nIDResource, BOOL bLoadIcons = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the size of the actual bitmapped
	//     images to be added to a toolbar.
	// Parameters:
	//     size - Width and height, in pixels, of the toolbar's icons.
	//-----------------------------------------------------------------------
	void SetIconSize(CSize size);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the size of a toolbar icon.
	// See Also:
	//     GetLargeIconSize
	//-----------------------------------------------------------------------
	virtual CSize GetIconSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get Large icon size of toolbar
	// Returns:
	//     CSize value contained size of large toolbar icons.
	// See Also: GetIconSize
	//-----------------------------------------------------------------------
	CSize GetLargeIconSize(BOOL bAutoSize) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the size of the buttons in the command bar.
	// Parameters:
	//     size - Width and height, in pixels, of the buttons.
	// Remarks:
	//     The button size must always be at least as large as the bitmap
	//     size it encloses. This function must be called only before adding
	//     any bitmaps to the command bar. If the application does not explicitly
	//     set the button size, it is set to the default size.
	//-----------------------------------------------------------------------
	void SetButtonSize(CSize size);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the size of the command bar button.
	// Returns:
	//     The width and height values of the command bar button.
	//-----------------------------------------------------------------------
	virtual CSize GetButtonSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to show the caption below control's icon.
	// Parameters:
	//     bTextBelow - TRUE to show the caption below the icon.
	//-----------------------------------------------------------------------
	void ShowTextBelowIcons(BOOL bTextBelow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a Boolean value that indicates whether the caption is
	//     below the control's icon.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL IsTextBelowIcons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to popup the command bar.
	// Parameters:
	//     pControlPopup - Points to a CXTPControlPopup object.
	//     bSelectFirst  - TRUE to select the first item.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the current focused control.
	// Returns:
	//     Window handle of the focused control.
	//-----------------------------------------------------------------------
	HWND GetTrackFocus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set focus to the window.
	// Parameters:
	//     hwnd - Window handle of the focused control
	//-----------------------------------------------------------------------
	void SetTrackFocus(HWND hwnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the visibility state of the control.
	// Parameters:
	//     bVisible - TRUE to set control visible.
	//-----------------------------------------------------------------------
	virtual void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a Boolean value that indicates whether the command bar
	//     is visible or hidden
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the position of the command bar
	// Parameters:
	//     barPosition - Position of the command bar
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL SetPosition(XTPBarPosition barPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the index of control with specified
	//     accelerator character.
	// Parameters:
	//     chAccel - Character to find.
	// Returns:
	//     Index of the control contained  character as accelerator.
	//-----------------------------------------------------------------------
	int FindAccel(TCHAR chAccel) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the index of the control with specified
	//     accelerator character.
	// Parameters:
	//     chAccel     - Character to find
	//     bSelectOnly - TRUE to select only.
	// Returns:
	//     Index of the control contained character as accelerator.
	//-----------------------------------------------------------------------
	int FindAccelEx(UINT chAccel, BOOL& bSelectOnly) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the customize mode of the command
	//     bars.
	// Returns:
	//     TRUE if command bars are in customized mode; otherwise returns
	//     FALSE
	//-----------------------------------------------------------------------
	BOOL IsCustomizeMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when a control is entered into the
	//     command bar.
	// Parameters:
	//     pDataObject - Points to a CXTPControl object
	//     point       - Point to test.
	// Returns:
	//     DROPEFFECT enumerator.
	//-----------------------------------------------------------------------
	virtual DROPEFFECT OnCustomizeDragEnter(CXTPControl* pDataObject, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a control leaves the command bar.
	//-----------------------------------------------------------------------
	virtual void OnCustomizeDragLeave();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when control is over the command bar.
	// Parameters:
	//     pDataObject - Points to a CXTPControl object
	//     point       - Point to test
	// Returns:
	//     DROPEFFECT enumerator.
	//-----------------------------------------------------------------------
	virtual DROPEFFECT OnCustomizeDragOver(CXTPControl* pDataObject, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when control is dropped to the command bar.
	// Parameters:
	//     pDataObject - Points to a CXTPControl object
	//     dropEffect  - DROPEFFECT enumerator.
	//     ptDrop      - Point where user drops the control.
	//     ptDrag      - Point where user starts drag the control.
	//-----------------------------------------------------------------------
	virtual void OnCustomizeDrop(CXTPControl* pDataObject, DROPEFFECT& dropEffect, CPoint ptDrop, CPoint ptDrag);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the customizable property of the
	//     command bar.
	// Returns:
	//     TRUE if the command bar is customizable; otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizable() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable/disable customization of the command bar.
	// Parameters:
	//     bEnable - TRUE to enable customization.
	//-----------------------------------------------------------------------
	void EnableCustomization(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to recalculate command bar layout.
	//-----------------------------------------------------------------------
	virtual void OnRecalcLayout();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove the tracking state of the command bar.
	//-----------------------------------------------------------------------
	void OnTrackLost();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to track the command bar.
	//-----------------------------------------------------------------------
	void OnTrackEnter();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the owner window of the command bar.
	// Returns:
	//     A pointer to a CWnd object
	//-----------------------------------------------------------------------
	CWnd* GetOwnerSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to redraw all parent command bars.
	//-----------------------------------------------------------------------
	void InvalidateParents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the current behavior modification
	//     flags of the command bar.
	// Returns:
	//     Flags of the command bar that are currently in effect.
	//-----------------------------------------------------------------------
	DWORD GetFlags() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the identifier of the command bar.
	// Returns:
	//     Identifier of the command bar.
	//-----------------------------------------------------------------------
	UINT GetBarID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the identifier.
	// Parameters:
	//     nID - Identifier to set.
	//-----------------------------------------------------------------------
	void SetBarID(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a child control becomes selected.
	// Parameters:
	//     bSelected - TRUE when control becomes selected.
	//     pControl  - Points to a CXTPControl object
	//-----------------------------------------------------------------------
	virtual void OnControlSelected(BOOL bSelected, CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set the description context.
	// Parameters:
	//     strDescription - Description string.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL SetStatusText(const CString& strDescription);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the command bar.
	// Parameters:
	//     dwData - Contains the new value to associate with the command bar.
	// See Also: GetCommandBarData
	//-----------------------------------------------------------------------
	void SetCommandBarData(DWORD_PTR dwData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value
	//     associated with the command bar.
	// Returns:
	//     The 32-bit value associated with the command bar.
	// See Also: SetCommandBarData
	//-----------------------------------------------------------------------
	DWORD_PTR GetCommandBarData() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the parent command bar.
	// Returns:
	//     A pointer to a CXTPCommandBar object
	//-----------------------------------------------------------------------
	virtual CXTPCommandBar* GetParentCommandBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the top-level parent command bar.
	// Returns:
	//     A pointer to the top-level CXTPCommandBar object
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetRootParent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the paint manager.
	// Returns:
	//     A pointer to a CXTPPaintManager object
	//-----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the image manager
	// Returns:
	//     A pointer to a CXTPImageManager object
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the width of the floated or popup toolbar.
	// Parameters:
	//     nWidth - Width of the floated or popuped toolbar.
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the width of the floated or popup toolbar.
	// Returns:
	//     The width, in pixels, of the floated or popup toolbar.
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the specified paint manager
	// Parameters:
	//     pPaintManager - Points to a CXTPPaintManager object  to be set
	//-----------------------------------------------------------------------
	void SetPaintManager(CXTPPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the specified image manager
	// Parameters:
	//     pImageManager - Points to a CXTPImageManager object to be set
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the CommandBar layout
	//     is Right-to-Left (RTL).
	// Returns:
	//     Returns TRUE if the Commandbar layout is RTL, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsLayoutRTL() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the CommandBar is a
	//     CXTPDialogBar.
	// Returns:
	//     Returns TRUE if the CommandBar is a CXTPDialogBar, otherwise FALSE
	// See Also: CXTPToolBar, CXTPDialogBar
	//-----------------------------------------------------------------------
	virtual BOOL IsDialogBar() const;

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
	//     Call this member function to determine if the CommandBar is a
	//     CXTPPopupBar.
	// Returns:
	//     Returns TRUE if the CommandBar is a CXTPPopupBar, otherwise FALSE
	// See Also: CXTPCommandBar, CXTPPopupBar
	//-----------------------------------------------------------------------
	virtual BOOL IsPopupBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if underlines is visible in command bar control.
	// Return:
	//     Returns TRUE if underlines is visible; otherwise FALSE;
	//-----------------------------------------------------------------------
	virtual BOOL IsKeyboardCuesVisible() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called internally to prevent command bar to grab edit focus.
	// Parameters:
	//     bGrabFocus - FALSE to prevent grab focus.
	//-----------------------------------------------------------------------
	void SetGrabFocus(BOOL bGrabFocus);

	//-------------------------------------------------------------------------
	// Summary:
	//     The framework calls this member function to redraw the CommandBars.
	// Parameters:
	//     lpRect - The rectangular area of the bar that is invalid.
	//     bAnimate - TRUE to animate changes in bounding rectangle.
	//-------------------------------------------------------------------------
	virtual void Redraw(LPCRECT lpRect = 0, BOOL bAnimate = TRUE);

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
	//     Call this method to enable animation for CommandBar
	// Parameters:
	//     bEnable - TRUE to enable animation; FALSE to disable
	//-----------------------------------------------------------------------
	void EnableAnimation(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable double buffer for toolbar drawing
	// Parameters:
	//     bEnable - True to enable double buffer for drawing.
	// See Also: EnableAnimation
	//-----------------------------------------------------------------------
	void EnableDoubleBuffer(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if animation is enabled for specified command bar
	// Returns:
	//     TRUE if animation is enabled
	// See Also: EnableAnimation
	//-----------------------------------------------------------------------
	BOOL IsAnimationEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by WindowProc, or is called during
	//     message reflection.
	// Parameters:
	//     hWnd     - Window handle message belongs to.
	//     nMessage - Specifies the message to be sent.
	//     wParam   - Specifies additional message-dependent information.
	//     lParam   - Specifies additional message-dependent information.
	//     lResult  - The return value of WindowProc. Depends on the message;
	//                may be NULL.
	//-----------------------------------------------------------------------
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to be sure control is visible on toolbar
	// Parameters:
	//     pControl - CXTPControl child that need to check
	//-----------------------------------------------------------------------
	virtual void EnsureVisible(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve default style of buttons of the commanbar
	// Returns:
	//     XTPButtonStyle represents default style of buttons
	// See Also: SetDefaultButtonStyle
	//-----------------------------------------------------------------------
	XTPButtonStyle GetDefaultButtonStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set default style for commanbar buttons.
	// Parameters:
	//     buttonStyle - Button style to set
	// See Also: GetDefaultButtonStyle
	//-----------------------------------------------------------------------
	void SetDefaultButtonStyle(XTPButtonStyle buttonStyle);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to freeze redrawing until UnlockRedraw call
	// See Also: UnlockRedraw
	//-------------------------------------------------------------------------
	void LockRedraw();

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to unfreeze redrawing.
	// See Also: LockRedraw
	//-------------------------------------------------------------------------
	void UnlockRedraw();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show gripper for the commandbar
	// Parameters:
	//     bShow - TRUE to show; FALSE to hide
	// See Also: GetShowGripper
	//-----------------------------------------------------------------------
	void SetShowGripper(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if gripper is visible for the commandbar
	// Returns:
	//     TRUE if gripper is visible
	// See Also: SetShowGripper
	//-----------------------------------------------------------------------
	BOOL GetShowGripper() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get borders margins of commandbar
	// Returns:
	//     Rectangle contains border margins
	//-----------------------------------------------------------------------
	virtual CRect GetBorders();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of command bar
	// Parameters:
	//     pDC - POinter to device context to draw
	// See Also: DrawCommandBar
	//-----------------------------------------------------------------------
	virtual void FillCommandBarEntry(CDC* pDC);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to check if keyboard tips are visible for the commandbar
	// Returns:
	//     TRUE if keyboard tips are visible
	// See Also: CreateKeyboardTips
	//-----------------------------------------------------------------------
	BOOL IsKeyboardTipsVisible() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to create keyboard tips collection for commandbar
	// See Also: IsKeyboardTipsVisible
	//-------------------------------------------------------------------------
	virtual void CreateKeyboardTips();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when user press key while keyboard tips are visible
	// Parameters:
	//     chAccel - Virtual key
	// See Also: OnKeyboardTip
	//-----------------------------------------------------------------------
	void OnKeyboardTipChar(UINT chAccel);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when keyboard tip was found for the commandbar
	// Parameters:
	//     pTip - Keyboard tip was found for commandbar
	// See Also: OnKeyboardTipChar
	//-----------------------------------------------------------------------
	virtual void OnKeyboardTip(CXTPCommandBarKeyboardTip* pTip);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to hide
	// See Also: IsKeyboardTipsVisible
	//-------------------------------------------------------------------------
	void HideKeyboardTips();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     The framework calls this member function when a non-system key
	//     is pressed.
	// Parameters:
	//     nChar - Specifies the virtual key code of the given key.
	//     lParam   - Specifies additional message-dependent information.
	// Returns:
	//     TRUE if key handled, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL OnHookKeyDown(UINT nChar, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get the size of the command bar.
	// Parameters:
	//      nLength - The requested dimension of the control bar, either horizontal or vertical, depending on dwMode.
	//      dwMode - see CControlBar::CalcDynamicLayout for list of supported flags.
	// Returns:
	//     Size of the command bar.
	//-----------------------------------------------------------------------
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to process special keys.
	// Parameters:
	//     key - Special keys enumerator. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     CXTPCommandBar maps key passed as parameter of OnKeyDown to XTPSpecialKey enumerator and call ProcessSpecialKey.<p/>
	//     key parameter can be one of the following:
	//         * <b>xtpKeyNext</b> Indicates the RIGHT or DOWN key was pressed  Next control must be selected
	//         * <b>xtpKeyPrev</b> Indicates the LEFT or UP key was pressed. Previous control must be selected.
	//         * <b>xtpKeyBack</b> Indicates the LEFT key was pressed for popup bar. Current submenu must be closed
	//         * <b>xtpKeyPopup</b> Indicates the RIGHT key was pressed for popup bar. Submenu must be opened.
	//         * <b>xtpKeyEscape</b> Indicates the ESCAPE key was pressed.
	//         * <b>xtpKeyReturn</b> Indicates the RETURN key was pressed
	//         * <b>xtpKeyHome</b> Indicates the HOME key was pressed. First control must be selected.
	//         * <b>xtpKeyEnd</b> Indicates the END key was pressed. Last control must be selected.
	// Returns:
	//     TRUE if key handled, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL ProcessSpecialKey(XTPSpecialKey key);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the controls array is changed.
	//-----------------------------------------------------------------------
	virtual void OnControlsChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method makes a copy of the command bar.
	// Parameters:
	//     pCommandBar - Command bar needed to be copied.
	//     bRecursive - TRUE to copy recursively.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine mouse hit code
	// Parameters:
	//     pt - Position to test
	// Returns:
	//     HTCLIENT if pointer belongs client area; HTNOWHERE otherwise.
	//-----------------------------------------------------------------------
	virtual int OnMouseHitTest(CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates the control's shortcuts.
	//-----------------------------------------------------------------------
	virtual void UpdateShortcuts();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves active MDI document template
	// Remarks:
	//     This method call CXTPCommandBars::GetActiveDocTemplate.
	// Returns:
	//     Active document template identifier.
	// See Also: CXTPCommandBars::GetActiveDocTemplate
	//-----------------------------------------------------------------------
	UINT GetActiveDocTemplate() const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to assign self identifiers for serialization process.
	// Parameters:
	//     nID             - Identifier to assign
	//     pCommandBarList - List of CommandBars.
	//     pParam          - Address of a XTP_COMMANDBARS_PROPEXCHANGE_PARAM structure.
	//-----------------------------------------------------------------------
	virtual void GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called in serialization process.to restore popups from list of command bars.
	// Parameters:
	//     pCommandBarList - List of CommandBars.
	//-----------------------------------------------------------------------
	virtual void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if parent frame in help mode
	//-----------------------------------------------------------------------
	BOOL IsHelpMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called then toolbar removed from commandbars collection
	//-----------------------------------------------------------------------
	virtual void OnRemoved();

protected:
//{{AFX_CODEJOCK_PRIVATE
	void CustomizeFindDropIndex(CXTPControl* pDataObject, const CPoint& point, CRect& rcMarker, int& nDropIndex, BOOL& bDropAfter);
	void CustomizeDrawMarker(CDC* pDC);
	void UpdateDocTemplateControls();
	BOOL IsMouseLocked() const;
	CSize GetAutoIconSize(BOOL bLarge) const;
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
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE

	//{{AFX_VIRTUAL(CXTPCommandBar)
	public:
	virtual void OnFinalRelease();
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	void FilterToolTipMessage(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreviewAccel(UINT chAccel);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCommandBar)
	afx_msg void OnUpdateCmdUI();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();
	afx_msg void OnPaint();
	afx_msg LRESULT OnPrint(WPARAM wParam, LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnGetObject(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

public:
	int              m_nStateFlags;         // State of the command bars.
	static int       s_nHoverDelay;         // Hover delay time
	static int       s_nExpandDelay;        // Expand delay time
	static int       s_nExpandHoverDelay;   // Expand hover delay
	BOOL             m_bComboBar;           // TRUE if it is combo box list

protected:
	BOOL             m_bGrabFocus;          // TRUE to grab current focus.
	int              m_nMRUWidth;           // Recent width.
	int              m_nSelected;           // Selected control index.
	int              m_nPopuped;            // Popuped control index.
	HWND             m_hwndFocus;           // The handle of the window that currently has focus.
	UINT             m_nBarID;              // Command bar's identifier.
	BOOL             m_bVisible;            // TRUE if the command bar is visible.
	BOOL             m_bTracking;           // TRUE if the command bar is in tracking mode.
	CWnd*            m_pSite;               // Command bar's site.
	DWORD            m_nIdleFlags;          // Idle flags.
	DWORD            m_dwFlags;             // Command bar's flags.
	DWORD            m_dwStyle;             // Command bar's styles.
	CString          m_strTitle;            // Command bar's caption.
	XTPBarType       m_barType;             // Type of the command bar.
	CXTPControls*    m_pControls;           // A pointer to controls collection.
	XTPBarPosition   m_barPosition;         // Position of the command bar.
	CXTPCommandBars* m_pCommandBars;        // parent command bars class.
	CWnd*            m_pOwner;              // Owner window.
	CSize            m_szIcons;             // Size of the command bar's icons.
	CSize            m_szButtons;           // Size of the command bar's icons.
	BOOL             m_bTextBelow;          // TRUE if text is shown below the CommandBar buttons.
	BOOL             m_bExecOnRButton;      // TRUE if the control button is executed on a right-click
	BOOL             m_bIgnoreUpdateHandler;// TRUE to skip update handler for all child controls
	BOOL             m_bRecursePopup;       // TRUE if popup bar was opened recursively. See TPM_RECURSE flag description.
	int              m_nLockRecurse;        // TRUE if another popup bar is opened recursively.
	BOOL             m_bKeyboardSelect;     // TRUE if last item was selected by keyboard.
	BOOL             m_bCloseable;          // TRUE if the CommandBar can be closed and\or Hidden
	BOOL             m_bCustomizable;       // TRUE if command bar is customizable.
	CRect            m_rcMarker;            // Marker position.
	BOOL             m_bVerticalMarker;     // TRUE to draw marker vertically.
	DWORD_PTR        m_dwData;              // The 32-bit value associated with the command bar.
	UINT*            m_pReturnCmd;          // Return command holder.
	BOOL             m_bCustomizeDialogPresent; // TRUE if the command bar is present in Customize dialog.
	BOOL             m_bAnimatePopup;       // TRUE to animate popup bar.
	BOOL             m_nLockRedraw;         // TRUE if all drawing routines skipped
	BOOL             m_bTrackOnHover;       // TRUE to select item if mouse stay over control
	CXTPPaintManager* m_pPaintManager;      // Selected paint manager for command bar
	CXTPImageManager* m_pImageManager;      // Selected image manager for command bar
	BOOL             m_bMultiLine;          // True if multi line commandbar
	XTPButtonStyle   m_buttonStyle;         // Default button Style

	CXTPToolTipContext* m_pToolTipContext;  // Tool tip Context.
	CXTPCommandBarAnimation* m_pAnimation;  // Animation context
private:
	int m_nClickedControl;

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()
	DECLARE_XTP_COMMANDBAR(CXTPCommandBar)

	class CCommandBarCmdUI;

	DECLARE_INTERFACE_MAP()


	friend class CXTPCommandBars;
	friend class CXTPDockBar;
	friend class CCommandBarCmdUI;
	friend class CXTPControl;
	friend class CXTPControls;
	friend class CXTPControlPopup;
	friend class CXTPToolBar;
	friend class CXTPMouseManager;
	friend class CXTPPopupBar;
	friend class CXTPCustomizeSheet;
	friend class CXTPPaintManager;

//}}AFX_CODEJOCK_PRIVATE
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE int CXTPCommandBar::IsTrackingMode() const{
	return m_bTracking;
}
AFX_INLINE CXTPControls* CXTPCommandBar::GetControls() const {
	return m_pControls;
}
AFX_INLINE XTPBarType CXTPCommandBar::GetType() const {
	return m_barType;
}
AFX_INLINE XTPBarPosition CXTPCommandBar::GetPosition() const {
	return m_barPosition;
}
AFX_INLINE void CXTPCommandBar::DelayLayout() {
	m_nIdleFlags |= xtpIdleLayout;
}

AFX_INLINE void CXTPCommandBar::DelayRedraw() {
	m_nIdleFlags |= xtpIdleRedraw;
}


AFX_INLINE void CXTPCommandBar::SetTitle(LPCTSTR lpszTitle) {
	m_strTitle = lpszTitle;
	Redraw();
}
AFX_INLINE CString CXTPCommandBar::GetTitle() const {
	return m_strTitle;
}
AFX_INLINE void CXTPCommandBar::SetCommandBars(CXTPCommandBars* pCommandBars){
	m_pCommandBars = pCommandBars;
}
AFX_INLINE HWND CXTPCommandBar::GetTrackFocus() const {
	return m_hwndFocus;
}
AFX_INLINE void CXTPCommandBar::SetTrackFocus(HWND hwnd) {
	m_hwndFocus = hwnd;
}
AFX_INLINE void CXTPCommandBar::SetVisible(BOOL /*bVisible*/) {
}
AFX_INLINE BOOL CXTPCommandBar::IsVisible() const {
	return FALSE;
}
AFX_INLINE BOOL CXTPCommandBar::SetPosition(XTPBarPosition /*barPosition*/) {
	return FALSE;
}
AFX_INLINE CSize CXTPCommandBar::CalcDynamicLayout(int, DWORD /*nMode*/) {
	return CSize(0, 0);
}
AFX_INLINE CXTPCommandBar* CXTPCommandBar::GetParentCommandBar() const {
	return NULL;
}
AFX_INLINE void CXTPCommandBar::OnRecalcLayout() {

}
AFX_INLINE BOOL CXTPCommandBar::IsCustomizable() const{
	return m_bCustomizable;
}
AFX_INLINE BOOL CXTPCommandBar::IsDialogBar() const {
	return FALSE;
}
AFX_INLINE void CXTPCommandBar::EnableCustomization(BOOL bEnable) {
	if (!IsDialogBar()) m_bCustomizable = bEnable;
}

AFX_INLINE void CXTPCommandBar::SetIconSize(CSize size) {
	m_szIcons = size;
}
AFX_INLINE void CXTPCommandBar::SetButtonSize(CSize size) {
	m_szButtons = size;
}

AFX_INLINE DWORD CXTPCommandBar::GetFlags() const {
	return m_dwFlags;
}

AFX_INLINE UINT CXTPCommandBar::GetBarID() const {
	return m_nBarID;
}
AFX_INLINE void CXTPCommandBar::SetCommandBarData(DWORD_PTR dwData) {
	m_dwData = dwData;
}
AFX_INLINE DWORD_PTR CXTPCommandBar::GetCommandBarData() const {
	return m_dwData;
}
AFX_INLINE void CXTPCommandBar::LockRedraw() {
	m_nLockRedraw ++;
}
AFX_INLINE void CXTPCommandBar::UnlockRedraw() {
	m_nLockRedraw --;
	if (m_nLockRedraw == 0 && (m_nIdleFlags & xtpIdleRedraw))
		Redraw();
}
AFX_INLINE void CXTPCommandBar::SetGrabFocus(BOOL bGrabFocus) {
	m_bGrabFocus = bGrabFocus;
}
AFX_INLINE BOOL CXTPCommandBar::PreviewAccel(UINT) {
	return FALSE;
}
AFX_INLINE BOOL CXTPCommandBar::IsRibbonBar() const {
	return FALSE;
}
AFX_INLINE BOOL CXTPCommandBar::IsPopupBar() const {
	return FALSE;
}
AFX_INLINE void CXTPCommandBar::SetBarID(UINT nID) {
	m_nBarID = nID;
}
AFX_INLINE XTPButtonStyle CXTPCommandBar::GetDefaultButtonStyle() const {
	return m_buttonStyle;
}

AFX_INLINE void CXTPCommandBar::SetDefaultButtonStyle(XTPButtonStyle buttonStyle) {
	m_buttonStyle = buttonStyle;
}

#endif //#if !defined(__XTPCOMMANDBAR_H__)
