// XTPMenuBar.h : interface for the CXTPMenuBar class.
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
#if !defined(__XTPMENUBAR_H__)
#define __XTPMENUBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "XTPToolBar.h"
#include "XTPCommandBarsDefines.h"

#define XTP_IDR_MENUBAR                 1

class CXTPCommandBars;

//===========================================================================
// Summary:
//     CXTPMenuBarMDIMenuInfo structure used to describe parameters of application MDI menus.
//===========================================================================
class CXTPMenuBarMDIMenuInfo : public CXTPCmdTarget
{
public:
	CXTPMenuBarMDIMenuInfo(CXTPCommandBars* pCommandBars, UINT nIDResource);
	~CXTPMenuBarMDIMenuInfo();

public:
	CXTPControls* m_pControls;  // Menu controls
	CString m_strTitle;         // Title of MDI menu
	CString m_strDescription;   // Description of MDI menu
	HICON m_hIcon;              // Menu Icon
	UINT m_nIDResource;         // Template identifier
	BOOL m_bChanged;            // TRUE if menu was changed.

};

class CXTPMenuBar;


//===========================================================================
// Summary:
//     CXTPMenuBarMDIMenus represents collection of MDI menus for CXTPMenuBar
//===========================================================================
class _XTP_EXT_CLASS CXTPMenuBarMDIMenus : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMenuBarMDIMenus object.
	// Parameters:
	//     pMenuBar - Parent CXTPMenuBar
	//-----------------------------------------------------------------------
	CXTPMenuBarMDIMenus(CXTPMenuBar* pMenuBar);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMenuBarMDIMenus object, handles clean up and deallocation
	//-------------------------------------------------------------------------
	~CXTPMenuBarMDIMenus();

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns number of MDI menus in collection
	//-------------------------------------------------------------------------
	int GetCount() const;

//{{AFX_CODEJOCK_PRIVATE
	void SetAt(UINT nIDResource, CXTPMenuBarMDIMenuInfo* pInfo);
	CXTPMenuBarMDIMenuInfo* Lookup(UINT nIDResource) const;
	void Remove(UINT nIDResource);
	void RemoveAll();
	POSITION GetStartPosition() const;
	void GetNextMenu(POSITION& pos, CXTPMenuBarMDIMenuInfo*& pInfo) const;
//}}AFX_CODEJOCK_PRIVATE


protected:
	CMap<UINT, UINT, CXTPMenuBarMDIMenuInfo*, CXTPMenuBarMDIMenuInfo*> m_mapMenus;          // Menu map
	CXTPMenuBar* m_pMenuBar;            // Parent Menu Bar
};

//===========================================================================
// Summary:
//     CXTPMenuBar is a CXTPToolBar derived class. It represents menu bar of frame window.
//===========================================================================
class _XTP_EXT_CLASS CXTPMenuBar : public CXTPToolBar
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMenuBar object
	//-----------------------------------------------------------------------
	CXTPMenuBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMenuBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPMenuBar();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads the menu from resource.
	// Parameters:
	//     nIDResource - Menu identifier to load.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL LoadMenuBar(UINT nIDResource);

	//------------------------------------------------------------------------
	// Summary:
	//     Destroy all MDI menus.
	//------------------------------------------------------------------------
	void FreeMDIMenus();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will refresh the menus
	//-----------------------------------------------------------------------
	void RefreshMenu();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve identifier of currently visible menu
	//-----------------------------------------------------------------------
	UINT GetCurrentMenuResource() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve identifier of default menu
	//-----------------------------------------------------------------------
	UINT GetDefaultMenuResource() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves collection of application MDI menus
	//-----------------------------------------------------------------------
	CXTPMenuBarMDIMenus* GetMDIMenus() const;

	//-----------------------------------------------------------------------
	// Summary: This method is called to switch to MDI menu from MDI menus collection
	// Parameters:
	//     nIDMenu - Menu identifier to switch
	//     pMenu - Menu to switch
	// See Also: SwitchActiveMenu
	//-----------------------------------------------------------------------
	void SwitchMDIMenu(UINT nIDMenu, CMenu* pMenu = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to manually load MDI menu in application initialization
	// Parameters:
	//     nIDResource - Template identifier
	//-----------------------------------------------------------------------
	CXTPMenuBarMDIMenuInfo* AddMDIMenu(UINT nIDResource);

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to switch menu to active document menu.
	// See Also: GetActiveDocTemplate, SwitchMDIMenu
	//-------------------------------------------------------------------------
	void SwitchActiveMenu();


protected:
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
	int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	//-----------------------------------------------------------------------
	// Summary:
	//     Resets the controls to their original state.
	// Parameters:
	//     bShowWarningMessage - TRUE to show warning message
	//-----------------------------------------------------------------------
	virtual void Reset(BOOL bShowWarningMessage);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method makes a copy of the command bar.
	// Parameters:
	//     pCommandBar - Command bar needed to be copied.
	//     bRecursive - TRUE to copy recursively.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called create hooks for main window .
	//-------------------------------------------------------------------------
	void SetupHook();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves active MDI child window handle
	// Parameters:
	//     bMaximized -  If this parameter is not NULL, it is a pointer to a value that indicates the maximized state of the MDI child window.
	// Returns:
	//     The return value is the handle to the active MDI child window.
	//-----------------------------------------------------------------------
	HWND GetActiveMdiChildWnd(BOOL* bMaximized = NULL);

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to update active mdi window hooks.
	//-------------------------------------------------------------------------
	void SyncActiveMdiChild();

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to update system buttons of menu bar.
	//-------------------------------------------------------------------------
	virtual void RefreshSysButtons();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to switch current MDI menu.
	// Parameters:
	//     hMenu - Active menu need to select.
	//-----------------------------------------------------------------------
	void SwitchMDIMenu(HMENU hMenu);

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

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to check if Toolbar was changed and have to be saved
	// Returns:
	//     TRUE if toolbar have to be saved
	//-----------------------------------------------------------------------
	BOOL ShouldSerializeBar();

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPMenuBar)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE



private:
	void AddSysButton(CXTPControl* pControl, int nId, LPCTSTR lpszParameter, int nBefore = -1);
	BOOL IsMDIMenusChanged();
	BOOL IsOleDocumentActive(HWND hWndActiveChild) const;

protected:
	BOOL m_bMdiApp;             // TRUE if parent frame is CMDIFrameWnd derived.
	HWND m_hWndActiveChild;     // Active MDI window handle.
	HMENU m_hMenuDefault;       // Default menu handle.

	UINT m_nIDResource;         // Resource of default menu.
	UINT m_nIDResourceCurrent;  // Currently active menu.
	CXTPMenuBarMDIMenus* m_pMDIMenus;   // MDI menus.

private:
	BOOL m_bDirtyMenu;

private:
	DECLARE_XTP_COMMANDBAR(CXTPMenuBar)

	class CControlMDIButton;
	class CControlMDISysMenuPopup;

	friend class CXTPCommandBars;
	friend class CControlMDIButton;
	friend class CCommandBarsSite;
	friend class CCommandBarsCtrl;
};

AFX_INLINE UINT CXTPMenuBar::GetCurrentMenuResource() const {
	return m_nIDResourceCurrent;
}
AFX_INLINE UINT CXTPMenuBar::GetDefaultMenuResource() const {
	return m_nIDResource;
}
AFX_INLINE CXTPMenuBarMDIMenus* CXTPMenuBar::GetMDIMenus() const {
	return m_pMDIMenus;
}


#endif //#if !defined(__XTPMENUBAR_H__)
