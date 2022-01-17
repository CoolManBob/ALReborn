// XTPDockingPaneManager.h : interface for the CXTPDockingPaneManager class.
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
#if !defined(__XTPDOCKINGPANEMANAGER_H__)
#define __XTPDOCKINGPANEMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPDockingPaneDefines.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPSystemHelpers.h"

class CXTPDockingPane;
class CXTPDockingPaneBase;
class CXTPDockingPaneBase;
class CXTPDockingPaneManager;
class CXTPDockingPaneSplitterContainer;
class CXTPDockingPaneTabbedContainer;
class CXTPDockingPaneMiniWnd;
class CXTPDockingPaneContext;
class CXTPDockingPaneLayout;
class CXTPDockingPanePaintManager;
class CXTPImageManagerIcon;
class CXTPImageManager;
class CXTPToolTipContext;

//===========================================================================
// Summary:
//     CXTPDockingPaneManager is a CWnd derived class. It is used to manipulate
//     docking panes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneManager : public CWnd, public CXTPAccessible
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneManager object
	//-----------------------------------------------------------------------
	CXTPDockingPaneManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneManager object, handles cleanup and
	//     deallocation
	//-----------------------------------------------------------------------
	~CXTPDockingPaneManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to install docking panes.
	// Parameters:
	//     pParent       - Parent frame of the panes.
	//     bClipChildren - 'true' to add WS_CLIPCHILDREN and WS_CLIPSIBLING
	//                     styles to frame windows so panes are displayed
	//                     correctly displayed from a hidden state.
	// Returns:
	//     'true' if the Docking Pane manager was correctly initialized, otherwise
	//     returns 'false'.
	// Remarks:
	//     You must call this member function first to initialize the Docking Pane
	//     manager. This must be called first before any other member functions
	//     are called.
	//-----------------------------------------------------------------------
	bool InstallDockingPanes(CWnd* pParent, bool bClipChildren=true);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to create a docking pane.
	// Parameters:
	//     nID        - Unique identifier of the pane.
	//     rc         - Initial size of the pane.
	//     direction  - Docking direction. Can be any of the values listed in the Remarks section.
	//     pNeighbour - Pane's Neighbor. It can be NULL to use the frame as a neighbor.
	// Remarks:
	//     direction parameter can be one of the following:
	//     * <b>xtpPaneDockTop</b> Docks the pane to the top of the neighbor.
	//     * <b>xtpPaneDockLeft</b> Docks the pane to the left of the neighbor.
	//     * <b>xtpPaneDockRight</b> Docks the pane to the right of the neighbor.
	//     * <b>xtpPaneDockBottom</b> Docks the pane to the bottom of the neighbor.
	// Returns:
	//     The pointer to the created pane.
	//-----------------------------------------------------------------------
	CXTPDockingPane* CreatePane(UINT nID, CRect rc, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to dock an existing pane.
	// Parameters:
	//     pPane      - Pane to be docked.
	//     direction  - Docking direction. Can be any of the values listed in the Remarks section.
	//     pNeighbour - Pane's Neighbor.
	// Remarks:
	//     direction parameter can be one of the following:
	//     * <b>xtpPaneDockTop</b> Docks the pane to the top of the neighbor.
	//     * <b>xtpPaneDockLeft</b> Docks the pane to the left of the neighbor.
	//     * <b>xtpPaneDockRight</b> Docks the pane to the right of the neighbor.
	//     * <b>xtpPaneDockBottom</b> Docks the pane to the bottom of the neighbor.
	//-----------------------------------------------------------------------
	void DockPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to float an existing pane.
	// Parameters:
	//     pPane - Pane to be floated.
	//     rc    - Floating rectangle.
	//-----------------------------------------------------------------------
	CXTPDockingPaneMiniWnd* FloatPane(CXTPDockingPaneBase* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to attach a pane to another one.
	// Parameters:
	//     pPane      - Pane to be attached.
	//     pNeighbour - Pane's Neighbor.
	//-----------------------------------------------------------------------
	void AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the pane's position (docking/floating).
	// Parameters:
	//     pPane - Docking Pane.
	//-----------------------------------------------------------------------
	void ToggleDocking(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the pane's Auto hide (docking/auto hide).
	// Parameters:
	//     pPane - Docking Pane.
	//-----------------------------------------------------------------------
	void ToggleAutoHide(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to show (activate) a pane
	// Parameters:
	//     nID       - Pane's identifier.
	//     pPane     - Pane need to show
	//     bSetFocus - TRUE to set focus to selected pane
	//-----------------------------------------------------------------------
	void ShowPane(int nID, BOOL bSetFocus = TRUE);
	void ShowPane(CXTPDockingPane* pPane, BOOL bSetFocus = TRUE); // <COMBINE CXTPDockingPaneManager::ShowPane@int@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to close a pane
	// Parameters:
	//     nID - Pane's identifier.
	//     pPane - Pane need to close
	//-----------------------------------------------------------------------
	void ClosePane(int nID);
	void ClosePane(CXTPDockingPane* pPane); // <COMBINE CXTPDockingPaneManager::ClosePane@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to destroy a pane
	// Parameters:
	//     nID - Pane's identifier.
	//     pPane - Pane need to destroy
	// See Also: ClosePane
	//-----------------------------------------------------------------------
	void DestroyPane(int nID);
	void DestroyPane(CXTPDockingPane* pPane); // <COMBINE CXTPDockingPaneManager::DestroyPane@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide a pane.
	// Parameters:
	//     nID - Pane's identifier.
	//     pPane - Pane need to hide
	//-----------------------------------------------------------------------
	void HidePane(int nID);
	void HidePane(CXTPDockingPaneBase* pPane);// <COMBINE CXTPDockingPaneManager::HidePane@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find a pane by its identifier.
	// Parameters:
	//     nID - Pane's identifier.
	// Returns:
	//     A pointer to a CXTPDockingPane object.
	//-----------------------------------------------------------------------
	CXTPDockingPane* FindPane(int nID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is closed.
	// Parameters:
	//     nID - Pane's identifier.
	//     pPane - Pane need to close.
	// Returns:
	//     TRUE if the pane is closed.
	//-----------------------------------------------------------------------
	BOOL IsPaneClosed(int nID) const;
	BOOL IsPaneClosed(CXTPDockingPane* pPane) const; // <COMBINE CXTPDockingPaneManager::IsPaneClosed@int@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is hidden.
	// Parameters:
	//     nID - Pane's identifier.
	//     pPane - Points to a CXTPDockingPane object.
	// Returns:
	//     TRUE if the pane is hidden.
	//-----------------------------------------------------------------------
	BOOL IsPaneHidden(int nID) const;
	BOOL IsPaneHidden(CXTPDockingPane* pPane) const; // <COMBINE CXTPDockingPaneManager::IsPaneHidden@int@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is selected.
	// Parameters:
	//     nID - Pane's identifier.
	//     pPane - Points to a CXTPDockingPane object.
	// Returns:
	//     TRUE if the pane is selected.
	//-----------------------------------------------------------------------
	BOOL IsPaneSelected(int nID) const;
	BOOL IsPaneSelected(CXTPDockingPane* pPane) const; // <COMBINE CXTPDockingPaneManager::IsPaneSelected@int@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set icons to panes.
	// Parameters:
	//     nIDResource - Resource Identifier.
	//     nIDs        - Pointer to an array of pane Ids.
	//     nCount      - Number of elements in the array pointed to by nIDs.
	//     clrMask     - RGB value of transparent color.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetIcons(UINT nIDResource, const int* nIDs, int nCount, COLORREF clrMask = 0xC0C0C0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set an icon to the pane.
	// Parameters:
	//     nID   - Docking pane's identifier.
	//     hIcon - Icon handle.
	//-----------------------------------------------------------------------
	void SetIcon(UINT nID, CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get an icon of the pane.
	// Parameters:
	//     nID - Docking pane's identifier.
	//     nWidth - Width of icon to retrieve.
	// Returns:
	//     The docking pane's icon.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon(UINT nID, int nWidth = 16) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove all icons.
	//-----------------------------------------------------------------------
	void ClearIconMap();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the speed of animation.
	// Parameters:
	//     dAnimationDelay    - Must be -1 to use nAnimationDuration and  nAnimationInterval.
	//     nAnimationDuration - Total time of animation, in milliseconds
	//     nAnimationInterval - Amount of time to rest, in milliseconds, between
	//                          each step.
	// Remarks:
	//     To disable animation set nAnimationDuration = 0.
	// See Also: GetAnimationDelay
	//-----------------------------------------------------------------------
	void SetAnimationDelay(double dAnimationDelay = -1, int nAnimationDuration = 128, int nAnimationInterval = 16);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the animation speed.
	// Parameters:
	//     pAnimationDuration - Pointer to receive total time of animation, in milliseconds.
	//     pAnimationInterval - Pointer to receive amount of time to rest, in milliseconds, between
	//                          each step.
	// Returns:
	//     Animation delay.
	// See Also: SetAnimationDelay
	//-----------------------------------------------------------------------
	double GetAnimationDelay(int* pAnimationDuration = NULL, int* pAnimationInterval = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to allow user switch and move panes using keyboard.
	// Parameters:
	//     options - available keyboard keys that can be use to navigate items
	// Remarks:
	//     options can be combination of the following:
	//     * <b>xtpPaneKeyboardUseAltMinus</b> To use Alt+'-' to show context menu
	//     * <b>xtpPaneKeyboardUseAltF6</b> To use Alt+F6 to select next pane
	//     * <b>xtpPaneKeyboardUseAltF7</b> To use Alt+F7 to show window select dialog
	//     * <b>xtpPaneKeyboardUseCtrlTab</b> To use Ctrl+Tab to show window select dialog
	//     * <b>xtpPaneKeyboardUseAll</b> To use all keys
	//
	//     You can call SetKeyboardWindowSelectClass to set custom window for Ctrl-Tab and Alt_F7 operations
	// See Also: XTPDockingPaneKeyboardNavigate
	//-----------------------------------------------------------------------
	void EnableKeyboardNavigate(DWORD options = xtpPaneKeyboardUseAll);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Keyboard Navigate options enabled.
	// Returns:
	//     Returns combination of XTPDockingPaneKeyboardNavigate flags indicates which keys can be used
	//     to navigate panes
	// See Also: EnableKeyboardNavigate, XTPDockingPaneKeyboardNavigate
	//-----------------------------------------------------------------------
	BOOL IsKeyboardNavigateEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom CXTPDockingPaneWindowSelect window, that will use to show active panes and files.
	// Parameters:
	//     pWindowSelectClass - CRuntimeClass pointer of custom CXTPDockingPaneWindowSelect window.
	// See Also: EnableKeyboardNavigate, XTPDockingPaneKeyboardNavigate
	//-----------------------------------------------------------------------
	void SetKeyboardWindowSelectClass(CRuntimeClass* pWindowSelectClass);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines Runtime class for CXTPDockingPaneWindowSelect window, that shows active panes and files.
	// See Also: SetKeyboardWindowSelectClass, EnableKeyboardNavigate, XTPDockingPaneKeyboardNavigate
	//-----------------------------------------------------------------------
	CRuntimeClass* GetKeyboardWindowSelectClass() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to activate pane in cycle order
	// Parameters:
	//     pPane - Pane to start; NULL to select first pane
	//     bForward - TRUE to select next pane; FALSE to select previous pane
	//-----------------------------------------------------------------------
	BOOL ActivateNextPane(CXTPDockingPane* pPane, BOOL bForward);
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the list of the created panes.
	// Returns:
	//     A list of created panes.
	// See Also:
	//     GetPaneStack, CXTPDockingPaneInfoList
	//-----------------------------------------------------------------------
	CXTPDockingPaneInfoList& GetPaneList() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the list of the created panes.including virtual containers
	// Returns:
	//     A list of created panes with its containers.
	// See Also: GetPaneList
	//-----------------------------------------------------------------------
	CXTPDockingPaneBaseList& GetPaneStack() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the docking site of the manager.
	// Returns:
	//     The docking site.
	//-----------------------------------------------------------------------
	CWnd* GetSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the visual theme of the panes.
	// Parameters:
	//     theme - New visual theme. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     theme can be one of the following:
	//     * <b>xtpPaneThemeDefault</b> Enables the default theme.
	//     * <b>xtpPaneThemeOffice</b> Enables Visual Studio .NET style theme.
	//     * <b>xtpPaneThemeGrippered</b> Enables Visual Studio 6 style theme.
	//     * <b>xtpPaneThemeVisio</b> Enables Visio style theme.
	//     * <b>xtpPaneThemeOffice2003</b> Enables Office 2003 style theme.
	//     * <b>xtpPaneThemeNativeWinXP</b> Enables XP Theme.
	//     * <b>xtpPaneThemeWhidbey</b> Enables Whidbey theme.
	//-----------------------------------------------------------------------
	void SetTheme(XTPDockingPanePaintTheme theme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set a custom theme. See the DockingPane sample
	//     as a sample of creating a new theme.
	// Parameters:
	//     pTheme - New Theme.
	//-----------------------------------------------------------------------
	void SetCustomTheme(CXTPDockingPanePaintManager* pTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the current visual theme.
	// Returns:
	//     The current theme.
	//-----------------------------------------------------------------------
	XTPDockingPanePaintTheme GetCurrentTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves paint manager of docking panes.
	// Returns:
	//     Returns paint manager.
	//-----------------------------------------------------------------------
	CXTPDockingPanePaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to create a new layout.
	// Remarks:
	//     You must delete layout when it no longer used.
	// Returns:
	//     A pointer to a CXTPDockingPaneLayout object.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneLayout* CreateLayout();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the current layout.
	// Parameters:
	//     pLayout - Pointer to the existing layout.
	//-----------------------------------------------------------------------
	void GetLayout(CXTPDockingPaneLayout* pLayout) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the layout.
	// Parameters:
	//     pLayout - Pointer to the existing layout.
	//-----------------------------------------------------------------------
	void SetLayout(const CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to redraw all panes.
	//-----------------------------------------------------------------------
	void RedrawPanes();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve client pane.
	// Returns:
	//     A pointer to a CXTPDockingPaneBase object.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetClientPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to destroy all panes.
	//-----------------------------------------------------------------------
	void DestroyAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to close all panes.
	//-----------------------------------------------------------------------
	void CloseAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve Image Manager of Docking Panes.
	// Returns:
	//     Image Manager of Docking Panes.
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the specified image manager.
	// Parameters:
	//     pImageManager - Points to a CXTPImageManager object.
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide client area.
	// Parameters:
	//     bHide - TRUE to hide client.
	//-----------------------------------------------------------------------
	void HideClient(BOOL bHide);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks to see if the client is hidden.
	// Returns:
	//     TRUE if the client is hidden, FALSE if otherwise
	//-----------------------------------------------------------------------
	BOOL IsClientHidden() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add margins around panes
	// Parameters:
	//     nMargin - Width of margin for panes frame
	//-----------------------------------------------------------------------
	void SetClientMargin(int nMargin);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves width of margin around panes
	//-----------------------------------------------------------------------
	int GetClientMargin() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to use trackers.
	// Parameters:
	//     bSplitterTracker - TRUE to use trackers.
	// Remarks:
	//     This member function will display the contents for child pane
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
	//     Call this member to set direction of captions
	// Parameters:
	//     captionDirection - New direction to be set
	// Remarks:
	//     captionDirection parameter can be one of the following:
	//     * <b>xtpPaneCaptionHorizontal</b>
	//     * <b>xtpPaneCaptionVertical</b>
	//     * <b>xtpPaneCaptionAutoByPosition</b>
	//     * <b>xtpPaneCaptionAutoBySize</b>
	// Example:
	//     m_paneManager.GetPaintManager()->SetCaptionDirection(xtpPaneCaptionAutoSize);
	//-----------------------------------------------------------------------
	void SetCaptionDirection(XTPDockingPaneCaptionDirection captionDirection);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member determine the direction the tab buttons are displayed.
	// Remarks:
	//     Can return one of the following direction:
	//     * <b>xtpPaneCaptionHorizontal</b>
	//     * <b>xtpPaneCaptionVertical</b>
	//     * <b>xtpPaneCaptionAutoByPosition</b>
	//     * <b>xtpPaneCaptionAutoBySize</b>
	// Returns:
	//      A XTPDockingPaneCaptionDirection flag indicating which direction the
	//      pane caption is displayed.
	// See Also: SetCaptionDirection
	//-----------------------------------------------------------------------
	XTPDockingPaneCaptionDirection GetCaptionDirection() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to lock splitters.
	// Parameters:
	//     bLock - TRUE to forbid splitter window panes to
	//             be resized.
	//-----------------------------------------------------------------------
	void LockSplitters(BOOL bLock = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the splitters are locked.
	// Returns:
	//     TRUE if the splitters are locked; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsSplittersLocked() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set common caption for floating frames.
	// Parameters:
	//     lpszCaption - Caption to be set.
	//-----------------------------------------------------------------------
	void SetFloatingFrameCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to use themes for floating frames.
	// Parameters:
	//     bThemedFloatingFrames - TRUE to use themed floating panes, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetThemedFloatingFrames(BOOL bThemedFloatingFrames);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if themes are used for the floating frames.
	// Returns:
	//     TRUE if they are used; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsThemedFloatingFrames() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable sticky floating frames
	// Parameters:
	//     bSticky - TRUE to use sticky floating frames, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetStickyFloatingFrames(BOOL bSticky);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if sticky floating frames option enabled
	//-----------------------------------------------------------------------
	BOOL IsStickyFloatingFrames() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set opacity for inactive floating frames
	// Parameters:
	//     nOpacity - Alpha value. When nOpacity is 0, the window is completely transparent. When nOpacity is 255, the window is opaque.
	//-----------------------------------------------------------------------
	void SetFloatingFramesOpacity(int nOpacity);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines opacity for inactive floating frames
	//-----------------------------------------------------------------------
	int GetFloatingFramesOpacity() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to keep the activation of floating frames windows in sync with the activation of the top level frame window.
	// Parameters:
	//     bSyncActiveFloatingFrames - TRUE to synchronizes the activation of the mini-frame window
	//                                to the activation of its parent window.
	//-----------------------------------------------------------------------
	void SyncActiveFloatingFrames(BOOL bSyncActiveFloatingFrames);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the current layout is Right-to-Left (RTL).
	// Returns:
	//     TRUE if the current layout is Right-to-Left (RTL), FALSE if the
	//     layout is Left-to-Right.
	//-----------------------------------------------------------------------
	BOOL IsLayoutRTL() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the Right-to-Left (RTL) docking direction
	//     equivalent to the direction passed in if IsLayoutRTL returns TRUE.
	// Parameters:
	//     direction - Docking direction
	// Returns:
	//     Returns the Righ-to-Left docking direction of the supplied direction
	//     if IsLayoutRTL returns TRUE.  If IsLayoutRTL returns FALSE, then
	//     the direction passed in remains the same.
	//-----------------------------------------------------------------------
	XTPDockingPaneDirection GetRTLDirection(XTPDockingPaneDirection direction) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine the XTPDockingPaneDirection of pPane.
	// Parameters:
	//     pPane - Points to a CXTPDockingPaneBase object
	// Returns:
	//     The current XTPDockingPaneDirection of pPane.
	//-----------------------------------------------------------------------
	XTPDockingPaneDirection GetPaneDirection(const CXTPDockingPaneBase* pPane) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves top level virtual container for site window.
	// Remarks:
	//     Top level container always splitter container (xtpPaneTypeSplitterContainer)
	// Returns:
	//     Top level container.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetTopPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to use custom docking context.
	// Parameters:
	//     pDockingContext - Points to a CXTPDockingPaneContext object
	//-----------------------------------------------------------------------
	void SetDockingContext(CXTPDockingPaneContext* pDockingContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves current docking context pointer.
	// Returns:
	//     Pointer to current docking context.
	//-----------------------------------------------------------------------
	CXTPDockingPaneContext* GetDockingContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if Alpha Docking Context is used when
	//     panes are being dragged and dropped.
	// Returns:
	//     TRUE if AlphaDockingContext is enabled, FALSE if it is disabled.
	// Remarks:
	//     AlphaDockingContext must be TRUE if Docking Context Stickers will
	//     be used.
	// See Also: SetAlphaDockingContext, IsShowDockingContextStickers, SetShowDockingContextStickers
	//-----------------------------------------------------------------------
	BOOL IsAlphaDockingContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify whether an alpha docking context is
	//     used to indicate where a pane can be docked while dragging the pane.
	// Parameters:
	//     bAlphaDockingContext - TRUE to enable Alpha colorization while docking.
	// Remarks:
	//     Specifies whether the area that a docking pane can occupy is
	//     shaded in gray as the pane is dragged to its new location.  The
	//     shaded area indicates the area on the application the docking
	//     pane will occupy if docked in that location.
	//     AlphaDockingContext must be TRUE if Docking Context Stickers will
	//     be used.
	// See Also: IsAlphaDockingContext, IsShowDockingContextStickers, SetShowDockingContextStickers
	//-----------------------------------------------------------------------
	void SetAlphaDockingContext(BOOL bAlphaDockingContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine whether Docking Context Stickers are used.
	// Returns:
	//     TRUE if Docking Context Stickers are used, FALSE if they are not used.
	// See Also: SetAlphaDockingContext, IsAlphaDockingContext, SetShowDockingContextStickers
	//-----------------------------------------------------------------------
	BOOL IsShowDockingContextStickers() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify whether Visual Studio 2005 style
	//     Docking Context Stickers are used while dragging a docking pane.
	// Parameters:
	//     bShowDockingContextStickers - If TRUE, docking stickers are drawn
	//     on the screen indicating all the possible locations that the docking
	//     pane can be docked while the pane is dragged over the application.
	//     AlphaDockingContext must also be True to display the stickers.
	// Remarks:
	//     AlphaDockingContext must be TRUE if Docking Context Stickers will
	//     be used.
	// See Also: SetAlphaDockingContext, IsAlphaDockingContext, IsShowDockingContextStickers, SetDockingContextStickerStyle
	//-----------------------------------------------------------------------
	void SetShowDockingContextStickers(BOOL bShowDockingContextStickers);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set Docking Context Stickers style.
	// Parameters:
	//     style - Style of docking pane stickers.
	// Remarks:
	//     Call SetShowDockingContextStickers to enable Stickers.
	//     Style parameter can be one of the following:
	//     * <b>xtpPaneStickerStyleWhidbey</b> Whidbey stickers.
	//     * <b>xtpPaneStickerStyleVisualStudio2005</b> Visual Studio 2005 stickers.
	// See Also: SetShowDockingContextStickers, GetDockingContextStickerStyle
	//-----------------------------------------------------------------------
	void SetDockingContextStickerStyle(XTPDockingContextStickerStyle style);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get Docking Context Stickers style.
	// Returns:
	//     Style of docking pane stickers.
	// Remarks:
	//     Call SetShowDockingContextStickers to enable Stickers.
	// See Also: SetShowDockingContextStickers, SetDockingContextStickerStyle
	//-----------------------------------------------------------------------
	XTPDockingContextStickerStyle GetDockingContextStickerStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set "Show window contents while dragging" option
	// Parameters:
	//     bShow - TRUE to set show contents while dragging
	//-----------------------------------------------------------------------
	void SetShowContentsWhileDragging(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if "Show window contents while dragging" option is enabled
	// Returns:
	//     TRUE if contents is visible while dragging.
	//-----------------------------------------------------------------------
	BOOL GetShowContentsWhileDragging() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show Maximize/Restore buttons for docking panes
	// Parameters:
	//     bShowMaximizeButton - TRUE to show maximize buttons for panes
	// See Also: IsCaptionMaximizeButtonsVisible
	//-----------------------------------------------------------------------
	void ShowCaptionMaximizeButton(BOOL bShowMaximizeButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Maximize/Restore buttons for docking panes are visible
	// See Also: ShowCaptionMaximizeButton
	//-----------------------------------------------------------------------
	BOOL IsCaptionMaximizeButtonsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to save current splitter positions for all splitter containers
	// See Also: CXTPDockingPaneSplitterContainer
	//-----------------------------------------------------------------------
	void NormalizeSplitters();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to recalculate layout for all parent frames of panes.
	//-----------------------------------------------------------------------
	void RecalcFramesLayout();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method updates XTP_DOCKINGPANE_INFO structure for each panes filling
	//     its members.
	//-----------------------------------------------------------------------
	void SyncPanesState();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set default options for each pane
	// Parameters:
	//     dwOptions - Option applied for each pane. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     dwOptions parameter can be one or more of the following:
	//     * <b>xtpPaneNoCloseable</b> Indicates the pane cannot be closed.
	//     * <b>xtpPaneNoHideable</b> Indicates the pane cannot be hidden.
	//     * <b>xtpPaneNoFloatable</b> Indicates the pane cannot be floated.
	//     * <b>xtpPaneNoCaption</b> Indicates the pane has no caption..
	// See Also: GetDefaultPaneOptions, CXTPDockingPane::SetOptions, XTPDockingPaneOptions
	//-----------------------------------------------------------------------
	void SetDefaultPaneOptions(DWORD dwOptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves default panes options.
	// Returns:
	//     Default options used for each pane.
	// See Also: SetDefaultPaneOptions, XTPDockingPaneOptions
	//-----------------------------------------------------------------------
	DWORD GetDefaultPaneOptions() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the application's panes into and out of print-preview mode.
	// Parameters:
	//     bPreview - Specifies whether or not to place the application in print-preview mode. Set to TRUE to place in print preview, FALSE to cancel preview mode.
	//-----------------------------------------------------------------------
	void OnSetPreviewMode (BOOL bPreview);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get pane currently active (focused)
	//-----------------------------------------------------------------------
	CXTPDockingPane* GetActivePane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tooltip context pointer.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to save/restore the settings of the pane.
	// Parameters:
	//     pPX - Points to a CXTPPropExchange object.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to recalculate layout of pane's site.
	// Parameters:
	//     pPane - Pane which site need to recalculate
	//     bDelay - TRUE to recalculate after small delay
	//-----------------------------------------------------------------------
	void RecalcFrameLayout(CXTPDockingPaneBase* pPane, BOOL bDelay = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to update panes state
	//-----------------------------------------------------------------------
	void UpdatePanes();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to notify the parent window that an event
	//     has occurred in the docking pane.
	// Parameters:
	//     nCode -  Specifies which event has occurred.
	//     lParam  - Additional message-specific information.
	//-----------------------------------------------------------------------
	virtual LRESULT NotifyOwner(UINT nCode, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show and activate pane
	// Parameters:
	//     pPane - pane to show
	// See Also: ShowPane
	//-----------------------------------------------------------------------
	void EnsureVisible(CXTPDockingPaneBase* pPane);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a pane is created.
	// Parameters:
	//     type    - Type of pane that is created.
	//     pLayout - Points to a CXTPDockingPaneLayout object.
	// Returns:
	//     Created pane.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneBase* OnCreatePane(XTPDockingPaneType type, CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a pane activated/deactivated
	// Parameters:
	//     bActive - TRUE if pane activated, FALSE otherwise;
	//     pPane - Pane was activated/deactivated.
	//-----------------------------------------------------------------------
	virtual void OnActivatePane(BOOL bActive, CXTPDockingPane* pPane);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPDockingPaneManager)
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg void OnInitialUpdate();
	afx_msg LRESULT OnGetObject(WPARAM wParam, LPARAM lParam);
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
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
	CXTPDockingPaneLayout* GetCurrentLayout() const;

	CRect _CalculateResultDockingRect(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour);
	void _InsertPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour);
	void _RemovePane(CXTPDockingPaneBase* pPane);
	void _AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour);
	CXTPDockingPaneBase* _GetHolder(CXTPDockingPaneBase* pPane, BOOL bFloating);
	CXTPDockingPaneBase* _Clone(CXTPDockingPaneBase* pPane);

	void _TrackPopupContextMenu(CXTPDockingPane* pPane);
	void _Redraw();
	void _DetachAll();
	BOOL _ToggleDocking(CXTPDockingPane* pPane, CXTPDockingPaneBase* pHolder);
	BOOL _OnAction(XTPDockingPaneAction action, CXTPDockingPane* pPane, CXTPDockingPaneBase* pDockContainer = NULL, XTPDockingPaneDirection dockDirection = xtpPaneDockLeft);

public:
	int m_nSplitterGap;                             // Minimum available width of panes.
	BOOL m_bCloseGroupOnButtonClick;                // If TRUE, when the close button on a group of panes is clicked, then the entire group of panes is closed.  If FALSE, the only the currently visible pane in the group will be closed.
	BOOL m_bHideGroupOnButtonClick;                 // If TRUE, when the hide button on a group of panes is clicked, then the entire group of panes is hidden.  If FALSE, the only the currently visible pane in the group will be hidden.
	BOOL m_bShowSizeCursorWhileDragging;            // TRUE to show size cursor while panes are dragged
	CPoint m_ptMinClientSize;                       // Minimum client size
	int m_nStickyGap;                               // Sticky gap

protected:
	CWnd* m_pSite;                                  // Parent window of docking manager.
	CXTPDockingPaneLayout* m_pLayout;               // Selected layout.
	CXTPDockingPanePaintManager* m_pPaintManager;   // Current paint manager.
	CXTPDockingPane* m_pActivePane;                 // Current Active pane;

	int m_nClientMargin;                            // Client margins
	BOOL m_bStickyFloatingFrames;                   // TRUE to enable sticky option for floating panes

	CXTPImageManager* m_pImageManager;              // Image manager of docking panes.

	BOOL m_bHideClient;                             // If TRUE, the client area is hidden so that only the docking panes are visible and occupy the entire area.
	BOOL m_bUseSplitterTracker;                     // If TRUE, splitter trackers are used.  When resizing a docking pane, an outline of the pane is drawn as the splitter is dragged.  If FALSE, the docking pane will be resized in "real-time."
	BOOL m_bShowMaximizeButton;                     // Allow panes to be maximized.

	BOOL m_bLockSplitters;                          // If TRUE, you can not resize the panes when they are docked. However, panes can be resized via code and when they are floated.
	BOOL m_bAlphaDockingContext;                    // If TRUE, alpha docking context is used when dragging a pane, the shaded area indicates the panes new location of dropped.
	BOOL m_bShowDockingContextStickers;             // If TRUE, docking context stickers are drawn when the pane is being dragged and dropped. m_bAlphaDockingContext must be TRUE.
	BOOL m_bShowContentsWhileDragging;              // If TRUE, window contents is visible while dragging.

	BOOL m_bThemedFloatingFrames;                   // If TRUE, floating docking panes will use the currently set theme.
	BOOL m_bSyncActiveFloatingFrames;               // Synchronizes the activation of the mini-frame window to the activation of its parent window.
	BOOL m_nFloatingFramesOpacity;                  // Opacity of floating frames
	DWORD m_bKeyboardEnabled;                       // XTPDockingPaneKeyboardNavigate options

	DWORD m_dwDefaultPaneOptions;                   // Default Panes options.
	XTPDockingPaneCaptionDirection m_captionDirection; // Caption Direction

	CString m_strFloatingFrameCaption;              // The caption that is displayed in the title bar of a floating frame that has panes docked. This is the floating frame that contains other docking panes.

	XTPDockingContextStickerStyle m_nDockingContextStickerStyle;  // Sticker style.

	CXTPDockingPaneContext* m_pDockingContext;      // Docking context helper.
	CXTPDockingPaneLayout* m_pPreviewLayout;        // Preview mode layout.
	CXTPToolTipContext* m_pToolTipContext;          // Tooltip Context.

	typedef BOOL (WINAPI *PFNSETLAYEREDWINDOWATTRIBUTES) (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags); // Definition of SetLayeredWindowAttributes API function
	PFNSETLAYEREDWINDOWATTRIBUTES m_pfnSetLayeredWindowAttributes;          // POinter to SetLayeredWindowAttributes API function
	CRuntimeClass* m_pWindowSelectClass;            // Runtime class of WindowSelect

private:
	HWND m_hwndLastFocus;
	BOOL m_bAttachingPane;
	BOOL m_bLayoutCreated;
	BOOL m_bInitialUpdateCalled;
	CString m_strMove;

	friend class CXTPDockingPaneBase;
	friend class CXTPDockingPaneSplitterContainer;
	friend class CXTPDockingPaneTabbedContainer;
	friend class CXTPDockingPaneMiniWnd;
	friend class CXTPDockingPaneAutoHideWnd;
	friend class CXTPDockingPaneContext;
	friend class CXTPDockingPane;
	friend class CXTPDockingPaneLayout;
	friend class CXTPDockingPaneAutoHidePanel;
	friend class CDockingPaneSite;
	friend class CDockingPaneCtrl;
	friend class CDockingPaneOptions;

};

AFX_INLINE CWnd* CXTPDockingPaneManager::GetSite() const {
	return m_pSite;
}
AFX_INLINE CXTPDockingPaneLayout* CXTPDockingPaneManager::GetCurrentLayout() const {
	return m_pLayout;
}

AFX_INLINE void CXTPDockingPaneManager::RedrawPanes() {
	_Redraw();
}
AFX_INLINE CXTPDockingPanePaintManager* CXTPDockingPaneManager::GetPaintManager() const {
	return m_pPaintManager;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsClientHidden() const {
	return m_bHideClient;
}
AFX_INLINE void CXTPDockingPaneManager::UseSplitterTracker(BOOL bSplitterTracker) {
	m_bUseSplitterTracker = bSplitterTracker;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsSplitterTrackerUsed() const {
	return m_bUseSplitterTracker;
}
AFX_INLINE CXTPImageManager* CXTPDockingPaneManager::GetImageManager() const {
	return m_pImageManager;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsSplittersLocked() const {
	return m_bLockSplitters;
}
AFX_INLINE void CXTPDockingPaneManager::SetFloatingFrameCaption(LPCTSTR lpszCaption) {
	m_strFloatingFrameCaption = lpszCaption;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsThemedFloatingFrames() const {
	return m_bThemedFloatingFrames;
}
AFX_INLINE void CXTPDockingPaneManager::SetStickyFloatingFrames(BOOL bSticky) {
	m_bStickyFloatingFrames = bSticky;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsStickyFloatingFrames() const {
	return m_bStickyFloatingFrames;
}
AFX_INLINE CXTPDockingPaneContext* CXTPDockingPaneManager::GetDockingContext() const {
	return m_pDockingContext;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsAlphaDockingContext() const {
	return m_bAlphaDockingContext;
}
AFX_INLINE void CXTPDockingPaneManager::SetAlphaDockingContext(BOOL bAlphaDockingContext) {
	m_bAlphaDockingContext = bAlphaDockingContext;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsShowDockingContextStickers() const {
	return m_bShowDockingContextStickers;
}
AFX_INLINE void CXTPDockingPaneManager::SetShowDockingContextStickers(BOOL bShowDockingContextStickers) {
	m_bShowDockingContextStickers = bShowDockingContextStickers;
}
AFX_INLINE void CXTPDockingPaneManager::SetDefaultPaneOptions(DWORD dwOptions) {
	m_dwDefaultPaneOptions = dwOptions;
}
AFX_INLINE DWORD CXTPDockingPaneManager::GetDefaultPaneOptions() const {
	return m_dwDefaultPaneOptions;
}
AFX_INLINE void CXTPDockingPaneManager::HidePane(int nID) {
	HidePane((CXTPDockingPaneBase*)FindPane(nID));
}
AFX_INLINE void CXTPDockingPaneManager::ClosePane(int nID) {
	ClosePane(FindPane(nID));
}
AFX_INLINE void CXTPDockingPaneManager::ShowPane(int nID, BOOL bSetFocus) {
	ShowPane(FindPane(nID), bSetFocus);
}
AFX_INLINE void CXTPDockingPaneManager::DestroyPane(int nID) {
	DestroyPane(FindPane(nID));
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsPaneClosed(int nID) const {
	return IsPaneClosed(FindPane(nID));
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsPaneHidden(int nID) const {
	return IsPaneHidden(FindPane(nID));
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsPaneSelected(int nID) const {
	return IsPaneSelected(FindPane(nID));
}
AFX_INLINE CXTPDockingPane* CXTPDockingPaneManager::GetActivePane() const {
	return m_pActivePane;
}
AFX_INLINE CXTPToolTipContext* CXTPDockingPaneManager::GetToolTipContext() const {
	return m_pToolTipContext;
}
AFX_INLINE void CXTPDockingPaneManager::ShowCaptionMaximizeButton(BOOL bShowMaximizeButton) {
	m_bShowMaximizeButton = bShowMaximizeButton;
	RedrawPanes();
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsCaptionMaximizeButtonsVisible() const {
	return m_bShowMaximizeButton;
}
AFX_INLINE void CXTPDockingPaneManager::SetDockingContextStickerStyle(XTPDockingContextStickerStyle style) {
	m_nDockingContextStickerStyle = style;
}
AFX_INLINE XTPDockingContextStickerStyle CXTPDockingPaneManager::GetDockingContextStickerStyle() const {
	return m_nDockingContextStickerStyle;
}
AFX_INLINE void CXTPDockingPaneManager::SetCaptionDirection(XTPDockingPaneCaptionDirection captionDirection) {
	m_captionDirection = captionDirection;
	RedrawPanes();
}
AFX_INLINE XTPDockingPaneCaptionDirection CXTPDockingPaneManager::GetCaptionDirection() const {
	return m_captionDirection;
}
AFX_INLINE void CXTPDockingPaneManager::SetShowContentsWhileDragging(BOOL bShow) {
	m_bShowContentsWhileDragging = bShow;
}
AFX_INLINE BOOL CXTPDockingPaneManager::GetShowContentsWhileDragging() const {
	return m_bShowContentsWhileDragging;
}
AFX_INLINE void CXTPDockingPaneManager::SetClientMargin(int nMargin) {
	m_nClientMargin = nMargin;
	RecalcFrameLayout(NULL, TRUE);
}
AFX_INLINE int CXTPDockingPaneManager::GetClientMargin() const {
	return m_nClientMargin;
}
AFX_INLINE int CXTPDockingPaneManager::GetFloatingFramesOpacity() const {
	return m_nFloatingFramesOpacity;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsKeyboardNavigateEnabled() const {
	return m_bKeyboardEnabled;
}
AFX_INLINE void CXTPDockingPaneManager::SetKeyboardWindowSelectClass(CRuntimeClass* pWindowSelectClass) {
	m_pWindowSelectClass = pWindowSelectClass;
}
AFX_INLINE CRuntimeClass* CXTPDockingPaneManager::GetKeyboardWindowSelectClass() const {
	return m_pWindowSelectClass;
}


#endif //#if !defined(__XTPDOCKINGPANEMANAGER_H__)
