// XTPControlEdit.h : interface for the CXTPControlEdit class.
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
#if !defined(__XTPCONTOLEDIT_H__)
#define __XTPCONTOLEDIT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPControl.h"
#include "XTPControlComboBox.h"

class CXTPControlEdit;

const UINT XTP_FN_SPINUP     = 0x1010;
const UINT XTP_FN_SPINDOWN   = 0x1011;

struct NMXTPUPDOWN : public NMXTPCONTROL
{
	int   iDelta;
};

//===========================================================================
// Summary:
//     Inplace Edit control of CXTPControlEdit.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlEditCtrl : public CXTPEdit
{
public:
	CXTPControlEditCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the parent CXTPControlEdit object.
	// Returns:
	//     Pointer to parent CXTPControlEdit.
	//-----------------------------------------------------------------------
	CXTPControlEdit* GetControlEdit() const;

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh char format of edit control
	//-------------------------------------------------------------------------
	void UpdateCharFormat();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPControlEditCtrl)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg LRESULT OnWindowFromPoint(WPARAM, LPARAM);
	afx_msg void OnEditChanged();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPControlEdit* m_pControl;            // Parent edit control

private:
	friend class CXTPControlEdit;

};

//===========================================================================
// Summary:
//     CXTPControlEdit is a CXTPControl derived class.
//     It represents an edit control.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlEdit : public CXTPControl
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlEdit object
	//-----------------------------------------------------------------------
	CXTPControlEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlEdit object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControlEdit();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method creates an edit control. Override it to use inherited
	//     edit control.
	// Returns:
	//     A pointer to the newly created CXTPControlEditCtrl.
	//-----------------------------------------------------------------------
	virtual CXTPControlEditCtrl* CreateEditControl();

public:


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the edit control.
	// Returns:
	//     A pointer to the CEdit control.
	//-----------------------------------------------------------------------
	CEdit* GetEditCtrl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the edit control text.
	// Returns:
	//     The Edit control text.
	//-----------------------------------------------------------------------
	CString GetEditText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the edit control text.
	// Parameters:
	//     strText - New text of the edit control.
	//-----------------------------------------------------------------------
	void SetEditText(const CString&  strText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set grayed-out text displayed in the edit control
	//     that displayed a helpful description of what the control is used for.
	// Parameters:
	//     lpszEditHint - Edit hint to be set
	// Example:
	//     <code>pEdit->SetEditHint(_T("Click to find a contact");</code>
	// See Also: GetEditHint
	//-----------------------------------------------------------------------
	void SetEditHint(LPCTSTR lpszEditHint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get grayed-out text displayed in the edit control
	//     that displayed a helpful description of what the control is used for.
	// Returns:
	//     Edit hint of the control
	// See Also: SetEditHint
	//-----------------------------------------------------------------------
	CString GetEditHint() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function enables or disables shell auto completion.
	// Parameters:
	//     dwFlags - Flags that will be passed to SHAutoComplete function.
	// Remarks:
	//     Flags can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:
	//     * <b>SHACF_FILESYSTEM</b> This includes the File System as well as the rest of the shell (Desktop\My Computer\Control Panel\)
	//     * <b>SHACF_URLALL</b>  Include the URL's in the users History and Recently Used lists. Equivalent to SHACF_URLHISTORY | SHACF_URLMRU.
	//     * <b>HACF_URLHISTORY</b> URLs in the User's History
	//     * <b>SHACF_URLMRU</b> URLs in the User's Recently Used list.
	//     * <b>SHACF_FILESYS_ONLY</b> Include only the file system. Do not include virtual folders such as Desktop or Control Panel.
	// ---------------------------------------------------------------------------
	void EnableShellAutoComplete(DWORD dwFlags = SHACF_FILESYSTEM | SHACF_URLALL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to show the label of the control.
	// Parameters:
	//     bShow - TRUE to show the label.
	//-----------------------------------------------------------------------
	void ShowLabel(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the caption of the control is visible
	// Returns:
	//     TRUE if the caption is visible.
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if icon is visible for edit control
	// Returns:
	//     TRUE if control has icon
	//-----------------------------------------------------------------------
	BOOL IsImageVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the control has a visible label.
	// Returns:
	//     TRUE if the control has a visible label ; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL IsLabeled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show spin buttons for edit control
	// Parameters:
	//     bShow - TRUE to show spin buttons
	// See Also: IsSpinButtonsVisible
	//-----------------------------------------------------------------------
	void ShowSpinButtons(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if edit control has spin buttons
	// Returns:
	//     TRUE if control has spin buttons
	// See Also: ShowSpinButtons
	//-----------------------------------------------------------------------
	BOOL IsSpinButtonsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to enable or disable the control.
	//     If the control does not have flags xtpFlagManualUpdate, you must call
	//     the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// Parameters:
	//     bEnabled - TRUE if the control is enabled.
	// See Also: GetEnabled, SetChecked
	//-----------------------------------------------------------------------
	void SetEnabled(BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the style of edit control.
	// Returns:
	//     The style of the edit control.
	// See Also: SetEditStyle
	//-----------------------------------------------------------------------
	DWORD GetEditStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the style of edit control
	// Parameters:
	//     dwStyle - The style to be set
	// See Also: GetEditStyle
	//-----------------------------------------------------------------------
	void SetEditStyle(DWORD dwStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calls this function to set the read-only state of an edit control.
	// Parameters:
	//     bReadOnly - Specifies whether to set or remove the read-only state of the edit
	//                 control. A value of TRUE sets the state to read-only;
	//                 a value of FALSE sets the state to read/write.
	// See Also: GetReadOnly, SetEnabled
	//-----------------------------------------------------------------------
	void SetReadOnly(BOOL bReadOnly = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a boolean value indicating whether the edit control is read-only.
	// Returns:
	//     Returns TRUE if the control is read-only; otherwise, returns FALSE.
	// See Also: SetReadOnly
	//-----------------------------------------------------------------------
	BOOL GetReadOnly() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to compare controls.
	// Parameters:
	//     pOther - The control need compare with.
	// Returns:
	//     TRUE if the controls are identical.
	//-----------------------------------------------------------------------
	virtual BOOL Compare(CXTPControl* pOther);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get with of label.
	// Returns:
	//     Width of label of edit control.
	// See Also: SetLabelWidth, ShowLabel, IsLabeled
	//-----------------------------------------------------------------------
	int GetLabelWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set width of the label.
	// Parameters:
	//     nLabelWidth - Width of label to be set
	// See Also: GetLabelWidth, ShowLabel, IsLabeled
	//-----------------------------------------------------------------------
	void SetLabelWidth(int nLabelWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if edit control has focus
	//-----------------------------------------------------------------------
	BOOL HasFocus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set focus to the control.
	// Parameters:
	//     bFocused - TRUE to set focus
	//-----------------------------------------------------------------------
	virtual void SetFocused(BOOL bFocused);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the focused state of the control.
	// Returns:
	//     TRUE if the control has focus; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns spin buttons bounding rectangle.
	//-----------------------------------------------------------------------
	CRect GetSpinButtonsRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to hide the control.
	// Parameters:
	//     dwFlags - Reasons to hide.
	// See Also: XTPControlHideFlags
	//-----------------------------------------------------------------------
	virtual void SetHideFlags(DWORD dwFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when action property was changed
	// Parameters:
	//     nProperty - Property of the action
	// See Also: OnActionChanging
	//-----------------------------------------------------------------------
	virtual void OnActionChanged(int nProperty);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when action property is about to be changed
	// Parameters:
	//     nProperty - Property of the action
	// See Also: OnActionChanged
	//-----------------------------------------------------------------------
	virtual void OnActionChanging(int nProperty);

protected:

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to check if control accept focus
	// See Also: SetFocused
	//----------------------------------------------------------------------
	virtual BOOL IsFocusable() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Called after the mouse hovers over the control.
	//-----------------------------------------------------------------------
	virtual void OnMouseHover();

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the mouse cursor moves.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	//----------------------------------------------------------------------
	virtual void OnMouseMove(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called, then the edit control gets the focus.
	// Parameters:
	//     pOldWnd - Points to a CWnd object
	//-----------------------------------------------------------------------
	virtual void OnSetFocus(CWnd* pOldWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called, then the edit control loses the focus.
	//-----------------------------------------------------------------------
	virtual void OnKillFocus();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the control becomes selected.
	// Parameters:
	//     bSelected - TRUE if the control becomes selected.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL OnSetSelected(int bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the bounding rectangle of the control.
	// Parameters:
	//     rcControl - Bounding rectangle of the control.
	//-----------------------------------------------------------------------
	void SetRect(CRect rcControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called then edit control text was changed
	//-----------------------------------------------------------------------
	virtual void OnEditChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get default char format of rich edit text
	//-----------------------------------------------------------------------
	virtual CHARFORMAT2 GetDefaultCharFormat();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user clicks the control.
	// Parameters:
	//     bKeyboard - TRUE if the control is selected using the keyboard.
	//     pt - Mouse cursor position.
	//-----------------------------------------------------------------------
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user activate control using its underline.
	//----------------------------------------------------------------------
	virtual void OnUnderlineActivate();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to copy the control.
	// Parameters:
	//     pControl - Points to a source CXTPControl object
	//     bRecursive - TRUE to copy recursively.
	//-----------------------------------------------------------------------
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a non-system key is pressed.
	// Parameters:
	//     nChar - Specifies the virtual key code of the given key.
	//     lParam   - Specifies additional message-dependent information.
	// Returns:
	//     TRUE if key handled, otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL OnHookKeyDown(UINT nChar, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before recalculating the parent command
	//     bar size to calculate the dimensions of the control.
	// Parameters:
	//     dwMode - Flags used to determine the height and width of the
	//              dynamic command bar. See Remarks section for a list of
	//              values.
	// Remarks:
	//     The following predefined flags are used to determine the height and
	//     width of the dynamic command bar. Use the bitwise-OR (|) operator to
	//     combine the flags.<p/>
	//
	//     * <b>LM_STRETCH</b> Indicates whether the command bar should be
	//                stretched to the size of the frame. Set if the bar is
	//                not a docking bar (not available for docking). Not set
	//                when the bar is docked or floating (available for
	//                docking). If set, LM_STRETCH returns dimensions based
	//                on the LM_HORZ state. LM_STRETCH works similarly to
	//                the the bStretch parameter used in CalcFixedLayout;
	//                see that member function for more information about
	//                the relationship between stretching and orientation.
	//     * <b>LM_HORZ</b> Indicates that the bar is horizontally or
	//                vertically oriented. Set if the bar is horizontally
	//                oriented, and if it is vertically oriented, it is not
	//                set. LM_HORZ works similarly to the the bHorz
	//                parameter used in CalcFixedLayout; see that member
	//                function for more information about the relationship
	//                between stretching and orientation.
	//     * <b>LM_MRUWIDTH</b> Most Recently Used Dynamic Width. Uses the
	//                remembered most recently used width.
	//     * <b>LM_HORZDOCK</b> Horizontal Docked Dimensions. Returns the
	//                dynamic size with the largest width.
	//     * <b>LM_VERTDOCK</b> Vertical Docked Dimensions. Returns the dynamic
	//                size with the largest height.
	//     * <b>LM_COMMIT</b> Resets LM_MRUWIDTH to current width of
	//                floating command bar.
	//
	//     The framework calls this member function to calculate the dimensions
	//     of a dynamic command bar.<p/>
	//
	//     Override this member function to provide your own layout in classes
	//     you derive from CXTPControl. XTP classes derived from CXTPControl,
	//     such as CXTPControlComboBox, override this member function to provide
	//     their own implementation.
	// See Also:
	//     CXTPControlComboBox, CXTPControlCustom, CXTPControl,
	//     CXTPControlWindowList, CXTPControlWorkspaceActions, CXTPControlToolbars,
	//     CXTPControlOleItems, CXTPControlRecentFileList, CXTPControlSelector,
	//     CXTPControlListBox
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to assign a parent command bar object.
	// Parameters:
	//     pParent - Points to a CXTPCommandBar object
	//-----------------------------------------------------------------------
	void SetParent(CXTPCommandBar* pParent);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member checks if the user can resize control.
	// Returns:
	//     TRUE if resize available.
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizeResizeAllow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member returns the minimum width that the edit control
	//     can be sized by the user while in customization mode.
	// Returns:
	//     Width of label + 10 if the edit control's label is visible,
	//     if label is hidden, then it returns 10.
	//-----------------------------------------------------------------------
	virtual int GetCustomizeMinWidth() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called when control was removed from parent controls collection
	//-------------------------------------------------------------------------
	virtual void OnRemoved();

protected:
//{{AFX_CODEJOCK_PRIVATE
	void _SetEditText(const CString& strText);
	CString _GetEditText() const;
	void TrackSpinButton(CPoint pt);
	virtual void NotifySpinChanged(int increment, int direction);
	void OnThemeChanged();
	void ShowHideEditControl();
//}}AFX_CODEJOCK_PRIVATE

protected:

	CXTPControlEditCtrl* m_pEdit;       // Inplace edit control.
	BOOL m_bLabel;              // TRUE if label is visible.
	BOOL m_bReadOnly;           // TRUE if edit is readonly
	int m_nLabelWidth;          // Width of the label.
	BOOL m_bDelayReposition;    // Need to reposition control.
	BOOL m_bDelayDestroy;       // Need to reposition control.
	CString m_strEditHint;      // Edit hint of the controls
	BOOL m_bFocused;            // TRUE if edit has focus
	CString m_strLastText;      // Last entered text
	mutable CString m_strEditText;      // Edit text.
	mutable BOOL m_bEditChanged;        // TRUE if Edit Text was changed.
	CXTPControlComboBoxAutoCompleteWnd* m_pAutoCompleteWnd; // Auto complete window hook.
	DWORD m_dwShellAutoCompleteFlags;   // Shell auto complete flags.
	DWORD m_dwEditStyle;        // Edit style
	BOOL m_bShowSpinButtons;    // TRUE to show spsin buttons

public:

	DECLARE_XTP_CONTROL(CXTPControlEdit)
	friend class CXTPControlEditCtrl;
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE CEdit* CXTPControlEdit::GetEditCtrl() const {
	return m_pEdit;
}

AFX_INLINE void CXTPControlEdit::ShowLabel(BOOL bShow) {
	SetStyle(bShow ? xtpButtonCaption : xtpButtonAutomatic);
}
AFX_INLINE BOOL CXTPControlEdit::IsLabeled() const {
	return IsCaptionVisible();
}
AFX_INLINE BOOL CXTPControlEdit::IsCustomizeResizeAllow() const {
	return TRUE;
}
AFX_INLINE int CXTPControlEdit::GetCustomizeMinWidth() const {
	return m_nLabelWidth + 10;
}
AFX_INLINE CXTPControlEdit* CXTPControlEditCtrl::GetControlEdit() const {
	return m_pControl;
}
AFX_INLINE int CXTPControlEdit::GetLabelWidth() const {
	return m_nLabelWidth;
}
AFX_INLINE void CXTPControlEdit::SetLabelWidth(int nLabelWidth) {
	if (m_nLabelWidth != nLabelWidth)
	{
		m_nLabelWidth = nLabelWidth;
		m_bDelayReposition = TRUE;
	}
}
AFX_INLINE void CXTPControlEdit::OnThemeChanged() {
	m_bDelayReposition = TRUE;
}
AFX_INLINE void CXTPControlEdit::ShowSpinButtons(BOOL bShow) {
	if (m_bShowSpinButtons != bShow) {m_bShowSpinButtons = bShow; m_bDelayReposition = TRUE; DelayLayoutParent();}
}
AFX_INLINE BOOL CXTPControlEdit::IsSpinButtonsVisible() const {
	return m_bShowSpinButtons;
}

#endif //#if !defined(__XTPCONTOLEDIT_H__)
