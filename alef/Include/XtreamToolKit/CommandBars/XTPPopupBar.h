// XTPPopupBar.h : interface for the CXTPPopupBar class.
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
#if !defined(__XTPPOPUPBAR_H__)
#define __XTPPOPUPBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPCommandBar.h"

class CXTPControlPopup;
class CXTPCommandBars;

//===========================================================================
// Summary:
//     CXTPPopupBar is a CXTPCommandBar derived class. It represents submenu of menu bar.
//===========================================================================
class _XTP_EXT_CLASS CXTPPopupBar : public CXTPCommandBar
{
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_XTP_COMMANDBAR(CXTPPopupBar)
//}}AFX_CODEJOCK_PRIVATE

private:
	class CControlExpandButton;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPopupBar object
	//-----------------------------------------------------------------------
	CXTPPopupBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPopupBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPPopupBar();


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a popup bar object.
	// Parameters:
	//     pCommandBars - Points to a CXTPCommandBars object
	// Returns:
	//     A pointer to a CXTPPopupBar object
	//-----------------------------------------------------------------------
	static CXTPPopupBar* AFX_CDECL CreatePopupBar(CXTPCommandBars* pCommandBars);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method adds the tear-off state to the popup bar.
	// Parameters:
	//     lpszCaption - Caption of the tear-off toolbar.
	//     nID        - Identifier of the toolbar to be created.
	//     nWidth     - Width of the toolbar.
	//-----------------------------------------------------------------------
	void SetTearOffPopup(LPCTSTR lpszCaption, UINT nID, int nWidth = 400);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Popupbar has tear-off state.
	// Parameters:
	//     strCaption - Caption of the tear-off toolbar.
	//     nID        - Identifier of the toolbar to be created.
	//     nWidth     - Width of the toolbar.
	// Returns:
	//     TRUE if tear-off; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL IsTearOffPopup(CString& strCaption, UINT& nID, int& nWidth);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the popup menu is currently visible\popped up.
	// Returns:
	//     TRUE is the popup menu is visible/displayed, FALSE if the popup menu
	//     is currently hidden.
	//-------------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method opens the popup bar.
	// Parameters:
	//     xPos          - Specifies the logical x-coordinate of the popup bar
	//     yPos          - Specifies the logical y-coordinate of the  position.
	//     rcExclude     - Excluded area.
	//     pControlPopup - Pointer to a CXTPControlPopup object
	//     bSelectFirst  - TRUE to select the first item.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL Popup(int xPos, int yPos, LPCRECT rcExclude);
	BOOL Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst = FALSE); // <combine CXTPPopupBar::Popup@int@int@LPCRECT>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the control's popup.
	// Returns:
	//     A pointer to a CXTPControlPopup object.
	//-----------------------------------------------------------------------
	CXTPControlPopup* GetControlPopup() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method expands the bar.
	//-----------------------------------------------------------------------
	void ExpandBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to destroy the window.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL DestroyWindow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to make popup bar looks like popup toolbar.
	// Parameters:
	//     bToolBarType - TRUE to make bar as popup toolbar
	//-----------------------------------------------------------------------
	void SetPopupToolBar(BOOL bToolBarType);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the default menu item for the specified popup bar.
	// Parameters:
	//     uItem  - Identifier or position of the new default menu item or
	//              - 1 for no default item. The meaning of this parameter
	//              depends on the value of fByPos.
	//     fByPos - Value specifying the meaning of uItem. If this parameter
	//              is FALSE, uItem is a menu item identifier. Otherwise,
	//              it is a menu item position.
	//-----------------------------------------------------------------------
	void SetDefaultItem(UINT uItem, BOOL fByPos = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines the default menu item on the specified popup bar.
	// Parameters:
	//     gmdiFlags - Reserved. Should be NULL.
	//     fByPos    - Value specifying whether to retrieve the menu item's
	//                 identifier or its position. If this parameter is
	//                 FALSE, the identifier is returned. Otherwise, the
	//                 position is returned.
	// Returns:
	//     If the function succeeds, the return value is the identifier
	//     or position of the menu item. If the function fails, the return
	//     value is - 1.
	//-----------------------------------------------------------------------
	UINT GetDefaultItem(UINT gmdiFlags, BOOL fByPos = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if popup bar has double gripper.
	// Returns:
	//     TRUE if popup bar has double gripper.
	//-----------------------------------------------------------------------
	BOOL IsDoubleGripper() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set double gripper for popup bar
	// Parameters:
	//     bDoubleGripper - TRUE to set double gripper for popup bar
	//-----------------------------------------------------------------------
	void SetDoubleGripper(BOOL bDoubleGripper = TRUE);

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
	//     Determines if popup bar was added as context menu.
	// Returns:
	//     TRUE if popup bar is context menu
	// See Also: CXTPCommandBarsContextMenus
	//-----------------------------------------------------------------------
	BOOL IsContextMenu() const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Redraw the popup bar.
	//-----------------------------------------------------------------------
	void DelayRedraw() { Redraw(); }

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get the size of the command bar.
	// Parameters:
	//      nLength - The requested dimension of the control bar, either horizontal or vertical, depending on dwMode.
	//      dwMode - see CControlBar::CalcDynamicLayout for list of supported flags.
	// Returns:
	//     Size of the command bar.
	//-----------------------------------------------------------------------
	CSize CalcDynamicLayout(int nLength, DWORD dwMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to change the tracking state.
	// Parameters:
	//     bMode        - TRUE to set the tracking mode; otherwise FALSE.
	//     bSelectFirst - TRUE to select the first item.
	//     bKeyboard    - TRUE if the item is popuped by the keyboard.
	// Returns:
	//     TRUE if the method was successful.
	// See Also: IsTrackingMode.
	//-----------------------------------------------------------------------
	BOOL SetTrackingMode(int bMode, BOOL bSelectFirst = TRUE, BOOL bKeyboard = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select the specified control.
	// Parameters:
	//     nSelected - An integer index of the item to be selected.
	//     bKeyboard - TRUE if the item was selected using the keyboard.
	// Returns:
	//     TRUE if the method was successful; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetSelected(int nSelected, BOOL bKeyboard = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates the popup bar.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL Create();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the PopupBar position.
	// Parameters:
	//     sz - Size of the popup bar.
	//-----------------------------------------------------------------------
	void UpdateLocation(CSize sz);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to calculate destination rectangle of popup bar
	// Parameters:
	//     sz - Size of popup bar
	// Returns:
	//     Rectangle where popup bar will be located.
	//-----------------------------------------------------------------------
	virtual CRect CalculatePopupRect(CSize sz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Update flags.
	//-----------------------------------------------------------------------
	virtual void UpdateFlags();

	//-----------------------------------------------------------------------
	// Summary:
	//     Update expanding state.
	//-----------------------------------------------------------------------
	void UpdateExpandingState();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the command bar in the given context.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rcClipBox - The rectangular area of the control that is invalid
	//-----------------------------------------------------------------------
	void DrawCommandBar(CDC* pDC, CRect rcClipBox);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the controls array is changed.
	//-----------------------------------------------------------------------
	virtual void OnControlsChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to recalculate the command bar layout.
	//-----------------------------------------------------------------------
	void OnRecalcLayout();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the parent command bar
	// Returns:
	//     A pointer to a CXTPCommandBar object
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetParentCommandBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method make a copy of the command bar.
	// Parameters:
	//     pCommandBar - Command bar needed to be copied.
	//     bRecursive - TRUE to copy recursively.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

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

	//-------------------------------------------------------------------------
	// Summary:
	//     Translates all messages in message queue.
	//-------------------------------------------------------------------------
	void PumpMessage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates rectangle to set position where popup bar become visible.
	// Parameters:
	//     rc - CRect object specifying size of exclude area.
	//-----------------------------------------------------------------------
	virtual void AdjustExcludeRect(CRect& rc, BOOL /*bVertical*/);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the toolbars borders.
	// Parameters:
	//     rcBorders - Borders of the toolbar.
	//-----------------------------------------------------------------------
	void SetBorders(CRect rcBorders);

	//-----------------------------------------------------------------------
	// Summary:
	//     Disables shadow use.
	//-----------------------------------------------------------------------
	void DisableShadow();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	virtual CRect GetBorders();

	//{{AFX_MSG(CXTPPopupBar)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnFloatStatus(WPARAM wParam, LPARAM);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	void AdjustScrolling(LPSIZE lpSize, BOOL bDown = FALSE, BOOL bInvalidate = FALSE);
	void TrackTearOff();
	void SwitchTearOffTracking(BOOL bShow, CPoint point);
	void _MakeSameWidth(int nStart, int nLast, int nWidth);

	void Animate();
	XTPAnimationType GetAnimationType() const;

public:
	static double m_dMaxWidthDivisor;   // Maximum available width of popup

protected:
	CXTPControlPopup* m_pControlPopup;  // Control that popuped the bar.
	CPoint m_ptPopup;                   // Position of the bar.
	CRect m_rcExclude;                  // Excluded area.
	int  m_popupFlags;                  // Flags of the bar.
	BOOL m_bDynamicLayout;              // TRUE if DynamicLayout routine executed.
	BOOL m_bCollapsed;                  // TRUE if popupbar is collapsed.
	BOOL m_bExpanded;                   // TRUE if popupbar is expanded.
	BOOL m_bExpanding;                  // TRUE if popupbar is expanding.

	BOOL m_bTearOffPopup;               // TRUE if popupbar has tear-off option.
	CRect m_rcTearOffGripper;           // Tear-off gripper position.
	BOOL m_bTearOffSelected;            // TRUE if gripper selected.
	UINT_PTR m_nTearOffTimer;           // Tear-off timer.
	BOOL m_bTearOffTracking;            // Tear-off tracking mode.
	CSize m_szTearOffBar;               // Size of the bar.
	CXTPToolBar* m_pTearOffBar;         // Tear-off toolbar pointer.
	CString m_strTearOffCaption;        // Caption of the Tear-off toolbar.
	UINT m_nTearOffID;                  // ID of the Tear-off toolbar.
	int m_nTearOffWidth;                // Width of the Tear-off toolbar.
	BOOL m_bShowShadow;                 // TRUE to show the shadow.
	CRect m_rcBorders;                  // Borders of the bar.
	BOOL m_bDoubleGripper;              // TRUE if popup is state popup type
	BOOL m_bContextMenu;                // TRUE if popup bar is context menu

private:

	struct SCROLLINFO
	{
		struct BTNSCROLL
		{
			void Init(CWnd* pParent, UINT nID)
			{
				m_pParent = pParent;
				m_nID = nID;
			}

			CRect rc;
			UINT nTimer;
			CWnd* m_pParent;
			UINT m_nID;

			void KillTimer() { m_pParent->KillTimer(nTimer); nTimer = 0;}
			void SetTimer() { if (nTimer == 0) nTimer = (UINT)m_pParent->SetTimer(m_nID, 80, 0); }
			BOOL OnMouseMove(CPoint point)
			{
				if (rc.PtInRect(point))
				{
					SetTimer();
					return TRUE;
				} else if (nTimer) KillTimer();
				return FALSE;
			}
		};

		BOOL bScroll;
		int nScrollFirst, nScrollLast;
		BTNSCROLL btnUp;
		BTNSCROLL btnDown;

		void Init(CWnd* pWnd, UINT nIDUp, UINT nIDDown)
		{
			btnUp.Init(pWnd, nIDUp);
			btnDown.Init(pWnd, nIDDown);
			bScroll = nScrollFirst = nScrollLast = 0;
		}
	} m_scrollInfo;



private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	friend class CControlExpandButton;
	friend class CXTPControlPopup;
	friend class CXTPCommandBars;
	friend class CXTPCommandBar;
	friend class CXTPCommandBarsContextMenus;
};

//===========================================================================
// Summary:
//     CXTPPopupToolBar is a CXTPPopupBar derived class.
//===========================================================================
class _XTP_EXT_CLASS CXTPPopupToolBar : public CXTPPopupBar
{
	DECLARE_XTP_COMMANDBAR(CXTPPopupToolBar)

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPopupToolBar object
	//-----------------------------------------------------------------------
	CXTPPopupToolBar();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a popup toolbar object.
	// Parameters:
	//     pCommandBars - Points to a CXTPCommandBars object
	// Returns:
	//     A pointer to a CXTPPopupToolBar object
	//-----------------------------------------------------------------------
	static CXTPPopupToolBar* AFX_CDECL CreatePopupToolBar(CXTPCommandBars* pCommandBars);


protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get the size of the command bar.
	// Parameters:
	//      nLength - The requested dimension of the control bar, either horizontal or vertical, depending on dwMode.
	//      dwMode - see CControlBar::CalcDynamicLayout for list of supported flags.
	// Returns:
	//     Size of the command bar.
	//-----------------------------------------------------------------------
	CSize CalcDynamicLayout(int nLength, DWORD dwMode);
};

//////////////////////////////////////////////////////////////////////////



AFX_INLINE void CXTPPopupBar::DisableShadow() {
	m_bShowShadow = FALSE;
}
AFX_INLINE void CXTPPopupBar::SetBorders(CRect rcBorders) {
	m_rcBorders = rcBorders;
}
AFX_INLINE BOOL CXTPPopupBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE BOOL CXTPPopupBar::IsDoubleGripper() const {
	return m_bDoubleGripper;
}
AFX_INLINE void CXTPPopupBar::SetDoubleGripper(BOOL bDoubleGripper /*= TRUE*/) {
	m_bDoubleGripper = bDoubleGripper;
}
AFX_INLINE CXTPControlPopup* CXTPPopupBar::GetControlPopup() const {
	return m_pControlPopup;
}
AFX_INLINE BOOL CXTPPopupBar::IsPopupBar() const {
	return TRUE;
}
AFX_INLINE BOOL CXTPPopupBar::IsContextMenu() const {
	return m_bContextMenu;
}

#endif // #if !defined(__XTPPOPUPBAR_H__)
