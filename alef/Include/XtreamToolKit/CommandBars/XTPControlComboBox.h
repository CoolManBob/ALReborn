// XTPControlComboBox.h : interface for the CXTPControlComboBox class.
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
#if !defined(__XTPCONTOLCOMBOBOX_H__)
#define __XTPCONTOLCOMBOBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPControl.h"
#include "XTPControlPopup.h"
#include "XTPPopupBar.h"

//{{AFX_CODEJOCK_PRIVATE

#ifndef SHACF_DEFAULT
#define SHACF_FILESYSTEM                0x00000001  // This includes the File System as well as the rest of the shell (Desktop\My Computer\Control Panel\)
#define SHACF_URLALL                    (SHACF_URLHISTORY | SHACF_URLMRU)  // Include the URL's in the users History and Recently Used lists. Equivalent to SHACF_URLHISTORY | SHACF_URLMRU.
#define SHACF_URLHISTORY                0x00000002  // URLs in the User's History
#define SHACF_URLMRU                    0x00000004  // URLs in the User's Recently Used list.
#define SHACF_FILESYS_ONLY              0x00000010  // Include only the file system. Do not include virtual folders such as Desktop or Control Panel.
#define SHACF_USETAB                    0x00000008  // Use the tab to move thru the autocomplete possibilities instead of to the next dialog/window control.
#endif

//}}AFX_CODEJOCK_PRIVATE


class CXTPControlComboBox;
class CXTPControlComboBoxAutoCompleteWnd;

//===========================================================================
// Summary:
//     CXTPEdit is a CEdit derived class. It is for internal usage only.
//===========================================================================
class _XTP_EXT_CLASS CXTPEdit : public CEdit
{
private:
	class _XTP_EXT_CLASS CRichEditContext
	{
	public:
		CRichEditContext();
		~CRichEditContext();

	public:
		HINSTANCE m_hInstance;
		CString m_strClassName;
		BOOL m_bRichEdit2;
	};


	DECLARE_DYNCREATE(CXTPEdit)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPEdit object
	//-----------------------------------------------------------------------
	CXTPEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Called by the framework to route and dispatch command messages
	//     and to handle the update of command user-interface objects.
	// Parameters:
	//     nID          - Contains the command ID.
	//     nCode        - Identifies the command notification code.
	//     pExtra       - Used according to the value of nCode.
	//     pHandlerInfo - If not NULL, OnCmdMsg fills in the pTarget and
	//                    pmf members of the pHandlerInfo structure instead
	//                    of dispatching the command. Typically, this parameter
	//                    should be NULL.
	// Returns:
	//     Nonzero if the message is handled; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     The framework calls this member function when the user selects
	//     an item from a menu, when a child control sends a notification
	//     message, or when an accelerator keystroke is translated.
	// Parameters:
	//     wParam - The low-order word of wParam identifies the command
	//              ID of the menu item, control, or accelerator. The
	//              high-order word of wParam specifies the notification
	//              message if the message is from a control. If the message
	//              is from an accelerator, the high-order word is 1. If
	//              the message is from a menu, the high-order word is 0.
	//     lParam   - Specifies additional message-dependent information.
	// Returns:
	//     An application returns nonzero if it processes this message;
	//     otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function displays a popup context menu.
	// Parameters:
	//     pControl - Pointer to a CXTPControl control.
	//     point - CPoint object specifies xy coordinates.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL ShowContextMenu(CXTPControl* pControl, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function displays a popup context menu.
	// Parameters:
	//     dwStyle - Specifies object's style flags.
	//     pParentWnd - Pointer to the parent window.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL CreateEdit(DWORD dwStyle, CWnd* pParentWnd);


	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines whether the specified character is intended for a edit. If it is, this method processes the message.
	// Parameters:
	//     nChar - Specifies the virtual key code of the given key.
	//     lParam   - Specifies additional message-dependent information.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsDialogCode(UINT nChar, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns window text of edit
	// Parameters:
	//     rString - String to return text
	//-----------------------------------------------------------------------
	void GetWindowTextEx(CString& rString);
	void SetWindowTextEx(LPCTSTR lpszString);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get rich edit version information.
	//-----------------------------------------------------------------------
	CRichEditContext& GetRichEditContext();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if edit command is enable,
	// Parameters:
	//     nID - Edit command
	// Returns:
	//     TRUE if edit command is enabled for control
	//-----------------------------------------------------------------------
	BOOL IsCommandEnabled(UINT nID);

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPEdit)
	afx_msg void OnImeStartComposition();
	afx_msg void OnImeEndComposition();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bImeMode;            // TRUE if IME editor currently enabled.
	BOOL m_bComposited;         // TRUE if control is AERO composited.
	BOOL m_bIgonoreEditChanged;

};

//{{AFX_CODEJOCK_PRIVATE

// CXTPControlComboBoxAutoCompleteWnd implementation.
// used internally in CXTPControlComboBox and CXTPControlEdit controls

class _XTP_EXT_CLASS CXTPControlComboBoxAutoCompleteWnd : public CXTPHookManagerHookAble
{
public:
	CXTPControlComboBoxAutoCompleteWnd();
	~CXTPControlComboBoxAutoCompleteWnd();

public:
	HRESULT ShellAutoComplete(HWND hEdit, DWORD dwFlags);
	void CloseWindow();
	void SetupMessageHook(BOOL bSetup);
	BOOL IsDialogCode(UINT nChar, LPARAM lParam);

private:
	static CXTPControlComboBoxAutoCompleteWnd* m_pWndMonitor;
	static LRESULT CALLBACK CallWndProc(int code, WPARAM wParam, LPARAM lParam);
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	void SetAutoCompeteHandle(HWND);

public:
	HWND m_hWndAutoComplete;

private:
	static HHOOK m_hHookMessage;
	HWND m_hWndEdit;
};

//}}AFX_CODEJOCK_PRIVATE


///===========================================================================
// Summary:
//     CXTPControlComboBoxPopupBar is a CXTPPopupBar derived class.
//     It represents base class for combo popups.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlComboBoxPopupBar : public CXTPPopupBar
{
	DECLARE_XTP_COMMANDBAR(CXTPControlComboBoxPopupBar)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlComboBoxPopupBar object
	//-----------------------------------------------------------------------
	CXTPControlComboBoxPopupBar();

public:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRUAL(CXTPControlComboBoxPopupBar)
	virtual int GetCurSel() const {
		return LB_ERR;
	}
	virtual int FindString(int /*nStartAfter*/, LPCTSTR /*lpszItem*/) const{
		return LB_ERR;
	}
	virtual int FindStringExact(int /*nIndexStart*/, LPCTSTR /*lpsz*/) const {
		return LB_ERR;
	}
	virtual int SetTopIndex(int /*nIndex*/) {
		return LB_ERR;
	}
	virtual void SetCurSel(int /*nIndex*/) {
	}
	virtual void GetText(int /*nIndex*/, CString& /*rString*/) const {
	}
	virtual BOOL ProcessHookKeyDown(CXTPControlComboBox* pComboBox, UINT nChar, LPARAM lParam);
	BOOL OnHookKeyDown(UINT nChar, LPARAM lParam);
	//}}AFX_VIRUAL
//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//     CXTPControlComboBoxList is a CXTPControlComboBoxPopupBar derived class.
//     It represents a list box of CXTPControlComboBox control.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlComboBoxList : public CXTPControlComboBoxPopupBar
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlButton object
	//-----------------------------------------------------------------------
	CXTPControlComboBoxList();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by WindowProc, or is called during
	//                message reflection.
	// Parameters:
	//     hWnd     - Window handle that the message belongs to.
	//     nMessage - Specifies the message to be sent.
	//     wParam   - Specifies additional message-dependent information.
	//     lParam   - Specifies additional message-dependent information.
	//     lResult  - The return value of WindowProc. Depends on the message;
	//                may be NULL.
	//-----------------------------------------------------------------------
	int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called, then PopupBar becomes visible.
	// Parameters:
	//     pControlPopup - Points to a CXTPControlPopup object
	//     bSelectFirst  - TRUE to select the first item.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to change the tracking state.
	// Parameters:
	//     bMode - TRUE to set the tracking mode; otherwise FALSE.
	//     bSelectFirst - TRUE to select the first item.
	//     bKeyboard    - TRUE if the item is popuped by the keyboard.
	// See Also: IsTrackingMode.
	// Returns:
	//     TRUE if the method was successful.
	//-----------------------------------------------------------------------
	virtual BOOL SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the command bar in the given context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//     rcClipBox - The rectangular area of the control that is invalid
	//-----------------------------------------------------------------------
	virtual void DrawCommandBar(CDC* pDC, CRect rcClipBox);

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
	//     Call this member to retrieve the customize mode of the command
	//     bars.
	// Returns:
	//     TRUE if command bars are in customized mode; otherwise returns
	//     FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizable() const { return FALSE; }

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

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
	//     Creates list box.
	//-----------------------------------------------------------------------
	virtual void CreateListBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves list box  window.
	//-----------------------------------------------------------------------
	CListBox* GetListBoxCtrl() const;

protected:

//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRUAL(CXTPControlComboBoxList)
	virtual int GetCurSel() const {
		return GetListBoxCtrl()->GetCurSel();
	}
	virtual int FindString(int nStartAfter, LPCTSTR lpszItem) const {
		return GetListBoxCtrl()->FindString(nStartAfter, lpszItem);
	}
	virtual int FindStringExact(int nIndexStart, LPCTSTR lpsz) const {
		return GetListBoxCtrl()->FindStringExact(nIndexStart, lpsz);
	}
	virtual int SetTopIndex(int nIndex) {
		return GetListBoxCtrl()->SetTopIndex(nIndex);
	}
	virtual void SetCurSel(int nIndex) {
		GetListBoxCtrl()->SetCurSel(nIndex);
	}
	virtual void GetText(int nIndex, CString& rString) const {
		GetListBoxCtrl()->GetText(nIndex, rString);
	}
	//}}AFX_VIRUAL
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPControlComboBoxList)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE


	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to process key down event
	// Parameters:
	//     pComboBox - Owner combo box pointer
	//     nChar - Specifies the virtual key code of the given key.
	//     lParam - keystroke-message information
	// Returns:
	//     TRUE if message was processed.
	//-----------------------------------------------------------------------
	BOOL ProcessHookKeyDown(CXTPControlComboBox* pComboBox, UINT nChar, LPARAM lParam);

private:
	DECLARE_XTP_COMMANDBAR(CXTPControlComboBoxList)
	friend class CXTPControlComboBox;
};

//////////////////////////////////////////////////////////////////////////

//===========================================================================
// Summary:
//     Inplace Edit control of the combo.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlComboBoxEditCtrl : public CXTPEdit
{

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent CXTPControlComboBox object.
	// Returns:
	//     Pointer to parent CXTPControlComboBox.
	//-----------------------------------------------------------------------
	CXTPControlComboBox* GetControlComboBox() const;

protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh char format of edit control
	//-------------------------------------------------------------------------
	void UpdateCharFormat();

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	//{{AFX_MSG(CXTPControlComboBoxEditCtrl)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditChanged();
	afx_msg LRESULT OnWindowFromPoint(WPARAM, LPARAM);
	afx_msg void OnShellAutoCompleteStart();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPControlComboBox* m_pControl;        // Parent Combo Box.

private:
	friend class CXTPControlComboBox;
};


//-----------------------------------------------------------------------
// Summary:
//     CXTPControlComboBox is a CXTPControl derived class. It represents a combo box control.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPControlComboBox : public CXTPControlPopup
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlComboBox object
	//-----------------------------------------------------------------------
	CXTPControlComboBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlComboBox object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControlComboBox();

public:
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
	//     Call this member to insert\delete an edit box in the combo box
	//     when the control has focus.
	// Parameters:
	//     bSet - TRUE if the combo box has an edit control.
	// Remarks:
	//     If bSet is TRUE, when the combo box control is click an edit
	//     control is used to display the text and the user can edit or
	//     copy the text.
	// See Also: GetDropDownListStyle
	//-----------------------------------------------------------------------
	void SetDropDownListStyle(BOOL bSet = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the combo box has an edit control.
	// Returns:
	//     TRUE is the combo box has an edit control, FALSE otherwise.
	// See Also: SetDropDownListStyle
	//-----------------------------------------------------------------------
	BOOL GetDropDownListStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the width of the dropdown list.
	// Parameters:
	//     nWidth - The width of the dropdown list.
	//-----------------------------------------------------------------------
	void SetDropDownWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the count of items in the dropdown list.
	// Parameters:
	//     nDropDownItemCount - The count of items in the dropdown list.
	//-----------------------------------------------------------------------
	void SetDropDownItemCount(int nDropDownItemCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the dropdown list width.
	// Returns:
	//     Width of the dropdown list.
	//-----------------------------------------------------------------------
	int GetDropDownWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a string to a list box.
	// Parameters:
	//     lpsz - The string that is to be added.
	// Returns:
	//     The zero-based index of the string in the list box. The return value is LB_ERR
	//     if an error occurs.
	//-----------------------------------------------------------------------
	int AddString(LPCTSTR lpsz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the number of items in a list box.
	// Returns:
	//     The number of items in the list box, or LB_ERR if an error occurs.
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve a string from the list box of a combo box control.
	// Parameters:
	//     nIndex - Contains the zero-based index of the list-box string to be copied.
	//     str - A reference to a CString.
	//-----------------------------------------------------------------------
	void GetLBText(int nIndex, CString& str) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to removes all items from the list box of a combo box control.
	//-----------------------------------------------------------------------
	void ResetContent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call the FindStringExact member function to find the first list-box string
	//     (in a combo box) that matches the string specified in str.
	// Parameters:
	//     nIndexStart - Specifies the zero-based index of the item before the first item
	//     to be searched.
	//     lpsz - The string to search for.
	// Returns:
	//     The zero-based index of the matching item, or CB_ERR if the search was
	//     unsuccessful.
	// See Also: FindString
	//-----------------------------------------------------------------------
	int FindStringExact(int nIndexStart, LPCTSTR lpsz) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to insert a string into the list box of a combo box control
	// Parameters:
	//     nIndex - Contains the zero-based index to the position in the list box that will receive
	//     the string.
	//     lpsz - The string that is to be inserted.
	//-----------------------------------------------------------------------
	int InsertString(int nIndex, LPCTSTR lpsz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the currently selected item's text
	// Returns:
	//     The text that is currently selected.
	//-----------------------------------------------------------------------
	virtual CString GetListBoxText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the edit control of the combo box control.
	// Returns:
	//     A pointer to the CEdit control.
	//-----------------------------------------------------------------------
	CXTPControlComboBoxEditCtrl* GetEditCtrl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select a string in the list box of a combo box.
	// Parameters:
	//     nIndex - Specifies the zero-based index of the string to select.
	//-----------------------------------------------------------------------
	void SetCurSel(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine which item in the combo box is selected.
	// Returns:
	//     The zero-based index of the currently selected item in the list box of a combo box,
	//     or CB_ERR if no item is selected.
	//-----------------------------------------------------------------------
	int GetCurSel() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call the GetDroppedState member function to determine whether the list box of a drop-down
	//     combo box is visible (dropped down).
	// Returns:
	//     Nonzero if the list box is visible; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL GetDroppedState() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the edit text.
	// Returns:
	//     The Edit control text.
	//-----------------------------------------------------------------------
	CString GetEditText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to modify style of list box
	// Parameters:
	//     dwRemove - Styles to remove.
	//     dwAdd - Styles to add.
	//-----------------------------------------------------------------------
	void ModifyListBoxStyle(DWORD dwRemove, DWORD dwAdd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the edit control text.
	// Parameters:
	//     lpszText    - New text of the edit control.
	// See Also:
	//     FindStringExact, FindString
	//-----------------------------------------------------------------------
	void SetEditText(const CString& lpszText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set grayed-out text displayed in the edit control
	//     that displayed a helpful description of what the control is used for.
	// Parameters:
	//     lpszEditHint - Edit hint to be set
	// Example:
	//     <code>pCombo->SetEditHint(_T("Click to find a contact");</code>
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
	//     This method is called to get default char format of rich edit text
	//-----------------------------------------------------------------------
	virtual CHARFORMAT2 GetDefaultCharFormat();

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
	//     This member function enables or disables auto completion.
	// Parameters:
	//     bAutoComplete - TRUE to enable auto completion, otherwise FALSE.
	//-----------------------------------------------------------------------
	void EnableAutoComplete(BOOL bAutoComplete = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method finds the first string in a list box that contains the specified prefix,
	//     without changing the list-box selection
	// Parameters:
	//     nStartAfter - Contains the zero-based index of the item before the first item to be
	//                   searched. When the search reaches the bottom of the list box, it continues from the
	//                   top of the list box back to the item specified by nStartAfter. If nStartAfter is -1,
	//                   the entire list box is searched from the beginning.
	//     lpszItem    - Points to the null-terminated string that contains the prefix to search for.
	//                   The search is case independent, so this string may contain any combination of uppercase
	//                   and lowercase letters.
	// Returns:
	//     The zero-based index of the matching item, or LB_ERR if the search was unsuccessful.
	// See Also: FindStringExact
	//-----------------------------------------------------------------------
	int FindString(int nStartAfter, LPCTSTR lpszItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value associated with the
	//     specified combo box item.
	// Parameters:
	//     nIndex - Contains the zero-based index of an item in the combo box's list box.
	// Returns:
	//     The 32-bit value associated with the item, or CB_ERR if an error occurs.
	//-----------------------------------------------------------------------
	DWORD_PTR GetItemData(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the specified item in a combo box.
	// Parameters:
	//     nIndex     - Contains a zero-based index of the item to set.
	//     dwItemData - Contains the new value to associate with the item.
	// Returns:
	//     CB_ERR if an error occurs.
	//-----------------------------------------------------------------------
	int SetItemData(int nIndex, DWORD_PTR dwItemData);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to delete a string.
	// Parameters:
	//     nIndex - Contains a zero-based index of the item to delete.
	//-----------------------------------------------------------------------
	void DeleteItem(long nIndex);

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
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

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
	// See Also: SetLabelWidth, SetStyle, GetStyle
	//-----------------------------------------------------------------------
	int GetLabelWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set width of the label.
	// Parameters:
	//     nLabelWidth - Width of label to be set
	// See Also: GetLabelWidth, SetStyle, GetStyle
	//-----------------------------------------------------------------------
	void SetLabelWidth(int nLabelWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the caption of the control is visible
	// Returns:
	//     TRUE if the caption is visible.
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if icon is visible for combo box control
	// Returns:
	//     TRUE if control has icon
	//-----------------------------------------------------------------------
	BOOL IsImageVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get with of thumb button.
	// Returns:
	//     Width of thumb button of combo box.
	// See Also: SetThumbWidth
	//-----------------------------------------------------------------------
	int GetThumbWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set width of thumb button.
	// Parameters:
	//     nThumbWidth - Width of the thumb button to be set.
	// See Also: GetThumbWidth
	//-----------------------------------------------------------------------
	void SetThumbWidth(int nThumbWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if control has focus
	//-----------------------------------------------------------------------
	BOOL HasFocus() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method draw text of control if style is CBS_DROPDOWNLIST
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     rcText - Rectangle to draw.
	//----------------------------------------------------------------------
	virtual void DrawEditText(CDC* pDC, CRect rcText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves list box  window.
	//-----------------------------------------------------------------------
	CListBox* GetListBoxCtrl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns child popup bar
	//-----------------------------------------------------------------------
	CXTPControlComboBoxPopupBar* GetComboBoxPopupBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the edit icon's identifier.
	// Parameters:
	//     nId - Icon's identifier to be set.
	//-----------------------------------------------------------------------
	void SetEditIconId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the edit icon's identifier.
	//-----------------------------------------------------------------------
	int GetEditIconId() const;

	//{{AFX_CODEJOCK_PRIVATE

	// deprecated
	virtual CString GetText() const
	{
		return GetListBoxText();
	}
	//}}AFX_CODEJOCK_PRIVATE

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
	//     This method is called then edit control text was changed
	//-----------------------------------------------------------------------
	virtual void OnEditChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called, then the selected string is changed.
	//-----------------------------------------------------------------------
	virtual void OnSelChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the control is executed.
	//-----------------------------------------------------------------------
	virtual void OnExecute();

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
	//     This method create edit control. Override it to use inherited edit control.
	//-----------------------------------------------------------------------
	virtual CXTPControlComboBoxEditCtrl* CreateEditControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Called after the mouse hovers over the control.
	//-----------------------------------------------------------------------
	void OnMouseHover();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called, then edit control gets the focus.
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
	//     This method is called to get real rect of edit control of Combo Box
	// Parameters:
	//     rcControl - Rectangle of Combo Box area.
	//-----------------------------------------------------------------------
	virtual void DeflateEditRect(CRect& rcControl);

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
	//     This method is called to assign a parent command bar object.
	// Parameters:
	//     pParent - Points to a CXTPCommandBar object
	//-----------------------------------------------------------------------
	void SetParent(CXTPCommandBar* pParent);

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
	//     CXTPControl, CXTPControlCustom, CXTPControlEdit,
	//     CXTPControlWindowList, CXTPControlWorkspaceActions, CXTPControlToolbars,
	//     CXTPControlOleItems, CXTPControlRecentFileList, CXTPControlSelector,
	//     CXTPControlListBox
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to popup the control.
	// Parameters:
	//     bPopup - TRUE to set popup.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL OnSetPopup(BOOL bPopup);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member checks if the user can resize control.
	// Returns:
	//     TRUE if resize available.
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizeResizeAllow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member returns the minimum width that the combo box
	//     can be sized by the user while in customization mode.
	// Returns:
	//     Width of label + Width of Dropdown button + 5
	//-----------------------------------------------------------------------
	virtual int GetCustomizeMinWidth() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called when control was removed from parent controls collection
	//-------------------------------------------------------------------------
	virtual void OnRemoved();

//{{AFX_CODEJOCK_PRIVATE
protected:
	public:
	void UpdatePopupSelection();
	protected:
	BOOL IsValidList() const;
	void _SetEditText(const CString& lpszText);
	CString _GetEditText() const;
	virtual BOOL OnHookMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnThemeChanged();
	void ShowHideEditControl();
//}}AFX_CODEJOCK_PRIVATE


	DECLARE_XTP_CONTROL(CXTPControlComboBox)

protected:

	CXTPControlComboBoxEditCtrl* m_pEdit;   // Child edit control.
	BOOL m_bDropDown;           // TRUE if the combo is dropdown.
	XTPButtonStyle m_comboStyle; // Style of the combo box.
	int m_nLastSel;             // Last user selected index, (used during display of list box)
	CString m_strLastText;      // Last Text before user select focus and change it.
	BOOL m_bDelayDestroy;       // TRUE if need to recreate control.
	BOOL m_bDelayReposition;    // Need to reposition control.
	int m_nLabelWidth;          // Width of the label.
	int m_nThumbWidth;          // Width of the thumb area.
	CString m_strEditHint;      // Grayed-out text displayed in the edit control that displayed a helpful description
	BOOL m_bAutoComplete;       // TRUE if Auto Complete enabled
	BOOL m_bIgnoreAutoComplete; // TRUE to disable auto complete till next key event.
	DWORD m_dwShellAutoCompleteFlags;    // Shell auto complete flags.
	BOOL m_bFocused;            // TRUE if control is focused
	int m_nEditIconId;          // Edit Icon identifier
	BOOL m_bSelEndOk;           // TRUE if user selects a list item.
	int m_nDropDownItemCount;   // Maximum drop down items

	mutable CString m_strEditText;      // Edit text.
	mutable BOOL m_bEditChanged;        // TRUE if Edit Text was changed.

	CXTPControlComboBoxAutoCompleteWnd* m_pAutoCompleteWnd; // Auto Complete hook window.
	DWORD m_dwEditStyle;        // Edit style

private:
	int m_nCurSel;
	BOOL m_bIgnoreSelection;

	friend class CXTPControlComboBoxList;
	friend class CXTPControlComboBoxEditCtrl;

};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CListBox* CXTPControlComboBox::GetListBoxCtrl() const {
	return ((CListBox*)m_pCommandBar);
}
AFX_INLINE void CXTPControlComboBox::SetDropDownWidth(int nWidth) {
	m_pCommandBar->SetWidth(nWidth);
}
AFX_INLINE int CXTPControlComboBox::AddString(LPCTSTR lpsz) {
	return GetListBoxCtrl()->AddString(lpsz);
}
AFX_INLINE int CXTPControlComboBox::GetCount() const{
	return GetListBoxCtrl()->GetCount();
}
AFX_INLINE void CXTPControlComboBox::GetLBText(int nIndex, CString& str) const{
	GetComboBoxPopupBar()->GetText(nIndex, str);
}
AFX_INLINE void CXTPControlComboBox::ResetContent() {
	GetListBoxCtrl()->ResetContent();
}
AFX_INLINE int CXTPControlComboBox::FindStringExact(int nIndexStart, LPCTSTR lpsz) const {
	return GetComboBoxPopupBar()->FindStringExact(nIndexStart, lpsz);
}
AFX_INLINE int CXTPControlComboBox::InsertString(int nIndex, LPCTSTR lpsz) {
	return GetListBoxCtrl()->InsertString(nIndex, lpsz);
}
AFX_INLINE CXTPControlComboBoxEditCtrl* CXTPControlComboBox::GetEditCtrl() const {
	return m_pEdit;
}
AFX_INLINE int CXTPControlComboBox::FindString(int nStartAfter, LPCTSTR lpszItem) const {
	return GetComboBoxPopupBar()->FindString(nStartAfter, lpszItem);
}
AFX_INLINE DWORD_PTR CXTPControlComboBox::GetItemData(int nIndex) const {
	return (DWORD_PTR)GetListBoxCtrl()->GetItemData(nIndex);
}
AFX_INLINE int CXTPControlComboBox::SetItemData(int nIndex, DWORD_PTR dwItemData) {
	return GetListBoxCtrl()->SetItemData(nIndex, dwItemData);
}
AFX_INLINE void CXTPControlComboBox::DeleteItem(long nIndex) {
	if (nIndex < GetCount()) GetListBoxCtrl()->DeleteString(nIndex);
}
AFX_INLINE BOOL CXTPControlComboBox::IsCustomizeResizeAllow() const {
	return TRUE;
}
AFX_INLINE CXTPControlComboBox* CXTPControlComboBoxEditCtrl::GetControlComboBox() const {
	return m_pControl;
}
AFX_INLINE int CXTPControlComboBox::GetLabelWidth() const {
	return m_nLabelWidth;
}
AFX_INLINE void CXTPControlComboBox::SetLabelWidth(int nLabelWidth) {
	if (m_nLabelWidth != nLabelWidth)
	{
		m_nLabelWidth = nLabelWidth;
		m_bDelayReposition = TRUE;
	}
}
AFX_INLINE void CXTPControlComboBox::SetEditIconId(int nId) {
	if (m_nEditIconId != nId) {m_nEditIconId = nId; RedrawParent();m_bDelayReposition = TRUE;}
}
AFX_INLINE int CXTPControlComboBox::GetEditIconId() const{
	return m_nEditIconId;
}
AFX_INLINE int CXTPControlComboBox::GetThumbWidth() const {
	return m_nThumbWidth;
}
AFX_INLINE void CXTPControlComboBox::SetThumbWidth(int nThumbWidth) {
	if (m_nThumbWidth != nThumbWidth)
	{
		m_nThumbWidth = nThumbWidth;
		m_bDelayReposition = TRUE;
	}
}
AFX_INLINE void CXTPControlComboBox::OnThemeChanged() {
	m_bDelayReposition = TRUE;
}
AFX_INLINE void CXTPControlComboBox::SetDropDownItemCount(int nDropDownItemCount) {
	m_nDropDownItemCount = nDropDownItemCount;
}

#endif //#if !defined(__XTPCONTOLCOMBOBOX_H__)
