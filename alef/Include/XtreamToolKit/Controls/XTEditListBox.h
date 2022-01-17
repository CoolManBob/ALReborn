// XTEditListBox.h interface for the CXTEditListBox class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#if !defined(__XTEDITLISTBOX_H__)
#define __XTEDITLISTBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------
// Summary:
//     CXTEditListBoxToolBar is a CStatic derived class. It used by the
//     CXTEditListBox class to create a toolbar above the edit list box
//     to display icons for editing.
// Remarks:
//     CXTEditListBoxToolBar can be used for other classes by
//     setting the notify window in Initialize. This window will receive
//     notification messages whenever the new, delete, up, and down
//     buttons are pressed. You can handle these messages by adding an
//     ON_BN_CLICKED handler for each of the buttons XT_IDC_BTN_NEW,
//     XT_IDC_BTN_DELETE, XT_IDC_BTN_UP and XT_IDC_BTN_DOWN.
// ---------------------------------------------------------------------
class _XTP_EXT_CLASS CXTEditListBoxToolBar : public CStatic
{
	DECLARE_DYNAMIC(CXTEditListBoxToolBar)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTEditListBoxToolBar object
	//-----------------------------------------------------------------------
	CXTEditListBoxToolBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTEditListBoxToolBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTEditListBoxToolBar();


public:

	// ------------------------------------------------------------------------
	// Summary:
	//     Initializes the CXTEditListBoxToolBar control.
	// Parameters:
	//     bAutoFont -  True to enable automatic font initialization.
	// Remarks:
	//     Call this member function to initialize the edit group control. This
	//     method should be called directly after creating or sub-classing the
	//     control.
	// ------------------------------------------------------------------------
	virtual void Initialize(bool bAutoFont = true);

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function returns a reference to the new button of the
	//     edit group.
	// Returns:
	//     A reference to a CXTButton object.
	// ---------------------------------------------------------------------
	CXTButton& GetNewButton();

	// --------------------------------------------------------------------
	// Summary:
	//     This member function returns a reference to the delete button of
	//     the edit group.
	// Returns:
	//     A reference to a CXTButton object.
	// --------------------------------------------------------------------
	CXTButton& GetDeleteButton();

	// --------------------------------------------------------------------
	// Summary:
	//     This member function returns a reference to the up button of the
	//     edit group.
	// Returns:
	//     A reference to a CXTButton object.
	// --------------------------------------------------------------------
	CXTButton& GetUpButton();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function returns a reference to the down button of the
	//     edit group.
	// Returns:
	//     A reference to a CXTButton object.
	// ----------------------------------------------------------------------
	CXTButton& GetDownButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will enable or disable editing.
	// Parameters:
	//     bEnable - True to enable editing.
	//-----------------------------------------------------------------------
	void EnableEdit(bool bEnable);

	// --------------------------------------------------------------------------
	// Summary:
	//     This member function will set the display style for the toolbar
	//     buttons.
	// Parameters:
	//     dwxStyle -  Specifies the button style as defined in the Remarks
	//                 section.
	//     bRedraw -   Specifies whether the button is to be redrawn. A nonzero
	//                 value redraws the button. A zero value does not redraw the
	//                 button. The button is redrawn by default.
	// Remarks:
	//     Styles to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:<p/>
	//     * <b>BS_XT_FLAT</b> Draws a flat button.
	//     * <b>BS_XT_SEMIFLAT</b> Draws a semi-flat button.
	//     * <b>BS_XT_TWOROWS</b> Draws images and text that are
	//           centered.
	//     * <b>BS_XT_SHOWFOCUS</b> Draws a focus rect when the button
	//           has input focus.
	//     * <b>BS_XT_HILITEPRESSED</b> Highlights the button when
	//           pressed.
	//     * <b>BS_XT_XPFLAT</b> Draws a flat button ala Office XP.
	//     * <b>BS_XT_WINXP_COMPAT</b> Enables Windows XP themes if
	//           available.
	// Returns:
	//     The previous style that was set.
	// --------------------------------------------------------------------------
	virtual void SetXButtonStyle(DWORD dwxStyle, BOOL bRedraw = TRUE);

	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTEditListBoxToolBar)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTEditListBoxToolBar object to
	//     render text display for the control.
	// Parameters:
	//     pDC      - Pointer to a valid device context.
	//     rcClient - Area to draw text on.
	//-----------------------------------------------------------------------
	virtual void DrawText(CDC* pDC, CRect& rcClient);

	// ------------------------------------------------------------------
	// Summary:
	//     Recalculates the button layout within the CXTEditListBoxToolBar window.
	// Remarks:
	//     This member function is called by the CXTEditListBoxToolBar object to
	//     position the group bar buttons when the window is sized.
	// ------------------------------------------------------------------
	virtual void MoveButtons();

	// ---------------------------------------------------------------------
	// Summary:
	//     Sends notification to the owner window.
	// Parameters:
	//     nCmdID -  Command ID to send.
	// Remarks:
	//     This member function sends the command specified by <i>nCmdID</i>
	//     to the owner of the CXTEditListBoxToolBar object. The command is
	//     sent whenever a button is pressed on the group bar.
	// ---------------------------------------------------------------------
	virtual void SendCommand(UINT nCmdID);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTEditListBoxToolBar)
	afx_msg void OnButtonNew();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	bool m_bShowUpDownButtons;     // Controls whether of not the up.down buttons are shown.
	bool m_bShowNewDeleteButtons;     // Controls whether of not the up.down buttons are shown.
	bool          m_bEnableEdit;   // True if editing is enabled.
	CRect         m_arClipRect[4]; // Array of toolbar button sizes.
	CXTButton     m_arButton[4];   // Array of toolbar buttons.
	CXTIconHandle m_arIcon[4];     // Array of toolbar button icons.
	CToolTipCtrl  m_tooltip;       // Tooltip control for edit buttons.

	friend class CXTEditListBox;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CXTButton& CXTEditListBoxToolBar::GetNewButton() {
	return m_arButton[0];
}
AFX_INLINE CXTButton& CXTEditListBoxToolBar::GetDeleteButton() {
	return m_arButton[1];
}
AFX_INLINE CXTButton& CXTEditListBoxToolBar::GetUpButton() {
	return m_arButton[2];
}
AFX_INLINE CXTButton& CXTEditListBoxToolBar::GetDownButton() {
	return m_arButton[3];
}
AFX_INLINE void CXTEditListBoxToolBar::EnableEdit(bool bEnable) {
	m_bEnableEdit = bEnable;
}

// forwards

class CXTItemEdit;

const DWORD LBS_XT_DEFAULT         = 0x0000;  //<ALIAS CXTEditListBox::SetListEditStyle@UINT@DWORD>
const DWORD LBS_XT_CHOOSEDIR       = 0x0001;  //<ALIAS CXTEditListBox::SetListEditStyle@UINT@DWORD>
const DWORD LBS_XT_CHOOSEFILE      = 0x0002;  //<ALIAS CXTEditListBox::SetListEditStyle@UINT@DWORD>
const DWORD LBS_XT_NOTOOLBAR       = 0x0008;  //<ALIAS CXTEditListBox::SetListEditStyle@UINT@DWORD>
const DWORD LBS_XT_BROWSE          = 0x0010; // Browse button
const DWORD LBS_XT_HIDE_UP_DOWN    = 0x0020; // Hide Up/Down buttons
const DWORD LBS_XT_ONLY_UP_DOWN    = 0x0040; // Only Up/Down buttons
const DWORD LBS_XT_BROWSE_ONLY     = 0x0080; // Browse button

//===========================================================================
// Summary:
//     CXTEditListBox is a CXTListBox derived class. It is used to create an
//     editable list box. This list box can be configured to display a toolbar
//     for editing. You can define browse styles to search for files or folders.
//     Each entry is made editable with a double mouse click.
//===========================================================================
class _XTP_EXT_CLASS CXTEditListBox : public CXTListBox
{
	DECLARE_DYNAMIC(CXTEditListBox)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTEditListBox object
	//-----------------------------------------------------------------------
	CXTEditListBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTEditListBox object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTEditListBox();

public:

	// -----------------------------------------------------------------------------
	// Summary:
	//     Sets the edit style for the edit list box.
	// Parameters:
	//     lpszTitle -  NULL terminated string that represents the caption title.
	//     nTitle -     Resource ID of the string to load for the caption title.
	//     dwLStyle -   Style for the list edit control. Pass in LBS_XT_NOTOOLBAR
	//                  if you do not wish the caption edit navigation control bar
	//                  to be displayed.
	// Remarks:
	//     Call this member function to set the style and title for the edit
	//     list box. The style of the edit list box can be set to one or more
	//     of the following values:<p/>
	//
	//     * <b>LBS_XT_DEFAULT</b> Standard edit field.
	//     * <b>LBS_XT_CHOOSEDIR</b> Choose directory browse edit field.
	//     * <b>LBS_XT_CHOOSEFILE</b> Choose file browse edit field.
	//     * <b>LBS_XT_NOTOOLBAR</b> Do not display edit toolbar.
	// -----------------------------------------------------------------------------
	void SetListEditStyle(UINT nTitle, DWORD dwLStyle = LBS_XT_DEFAULT);
	void SetListEditStyle(LPCTSTR lpszTitle, DWORD dwLStyle = LBS_XT_DEFAULT); //<combine CXTEditListBox::SetListEditStyle@UINT@DWORD>

	// --------------------------------------------------------------------
	// Summary:
	//     Retrieves the current item index.
	// Returns:
	//     An integer value that represents the edit control index.
	// Remarks:
	//     Call this member function to get the current index for the edit
	//     control. Similar to GetCurSel; however, the current index is the
	//     index of the last item to be modified or added to the edit list
	//     box and not necessarily the selected item.
	// --------------------------------------------------------------------
	int GetCurrentIndex();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will enable editing for the list box item.
	// Parameters:
	//     iItem - Index of the item to edit.
	//-----------------------------------------------------------------------
	void EditItem(int iItem);

	// --------------------------------------------------------------------
	// Summary:
	//     Retrieves the edited item's text label.
	// Remarks:
	//     This member function is called to retrieve the text for the item
	//     that is being edited in the list box and save the value to
	//     m_strItemText.
	// --------------------------------------------------------------------
	virtual void GetEditItemText();

	// --------------------------------------------------------------------
	// Summary:
	//     This method is called to set inplace edit text
	// Parameters:
	//     pcszText - next text to set
	// --------------------------------------------------------------------
	void SetEditText(LPCTSTR pcszText);

	// --------------------------------------------------------------------
	// Summary:
	//     Returns a pointer to the CXTEditListBoxToolBar toolbar.
	// Returns:
	//     A reference to a CXTEditListBoxToolBar object.
	// Remarks:
	//     Call this member function to return a reference to the
	//     CXTEditListBoxToolBar control that is associated with the edit list box.
	// --------------------------------------------------------------------
	CXTEditListBoxToolBar& GetEditGroup();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the default filter for the
	//     file dialog.
	// Parameters:
	//     lpszFilter - Points to a NULL terminated string that represents
	//                 the file filter used by the file open dialog.
	//-----------------------------------------------------------------------
	void SetDlgFilter(LPCTSTR lpszFilter = NULL);

	// --------------------------------------------------------------------------
	// Summary:
	//     This member function sets the initial directory for the file dialog.
	// Parameters:
	//     lpszInitialDir -  [in] Points to a NULL terminated string the represents the
	//                  initial directory of the file open dialog..
	// --------------------------------------------------------------------------
	void SetDlgInitialDir(LPCTSTR lpszInitialDir);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the edit list has a toolbar.
	// Returns:
	//     true if the toolbar is turned on, otherwise returns false.
	//-----------------------------------------------------------------------
	bool HasToolbar();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will enable or disable editing.
	// Parameters:
	//     bEnable - True to enable editing.
	//-----------------------------------------------------------------------
	void EnableEdit(bool bEnable);

	// ---------------------------------------------------------------------
	// Summary:
	//     Initializes the CXTEditListBox control.
	// Parameters:
	//     bAutoFont -  True to enable automatic font initialization.
	// Remarks:
	//     Call this member function to initialize the list box. This method
	//     should be called directly after creating or sub-classing the
	//     control.
	// ---------------------------------------------------------------------
	virtual void Initialize(bool bAutoFont = true);

	// -------------------------------------------------------------------
	// Summary:
	//     Recalculates the toolbar layout for the CXTEditListBox.
	// Remarks:
	//     Call this member function to correctly reposition the edit list
	//     box toolbar. This will readjust the layout to correctly and
	//     position the toolbar in relation to the list.
	// -------------------------------------------------------------------
	virtual void RecalcLayout();

	// -------------------------------------------------------------
	// Summary:
	//     Sets the default text for new items.
	// Parameters:
	//     lpszItemDefaultText -  NULL terminated string.
	// Remarks:
	//     Call this member function to set the default text that is
	//     displayed when a new item is added to the edit list box.
	// -------------------------------------------------------------
	void SetNewItemDefaultText(LPCTSTR lpszItemDefaultText);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will create the edit group control.
	// Parameters:
	//     bAutoFont - True to enable automatic font initialization.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL CreateEditGroup(bool bAutoFont = true);

	// ----------------------------------------------------------------------
	// Summary:
	//     Enables editing for the currently selected item.
	// Parameters:
	//     bNewItem -  TRUE to add a new item.
	// Remarks:
	//     This member function will enable editing for the currently
	//     selected list box item. If 'bNewItem' is TRUE, a new item is added
	//     to the end of the list box.
	// ----------------------------------------------------------------------
	virtual void EditListItem(BOOL bNewItem);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTEditListBox)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual COLORREF GetBackColor();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTEditListBox)
	afx_msg void OnEndLabelEdit();
	afx_msg void OnItemBrowse();
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnMoveItemUp();
	afx_msg void OnMoveItemDown();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CWnd* m_pParentWnd;             // Pointer to the parent window.
	CString m_strItemDefaultText;   // Default text used when new items are created.
	int             m_nIndex;       // Current index when edit functions are performed.
	BOOL            m_bNewItem;     // TRUE if a new item is being entered into the list box.
	bool            m_bEnableEdit;  // True if editing is enabled.
	DWORD           m_dwLStyle;     // List edit styles.
	CString         m_strTitle;     // Caption area title.
	CString         m_strFilter;    // Default file filter.
	CString         m_strInitialDir;   // Initial Dir.
	CString         m_strItemText;  // Current text of a selected item during edit.
	CXTItemEdit*    m_pItemEdit;    // Points to the in-place edit item.
	CXTEditListBoxToolBar    m_editGroup;    // The edit group (toolbar) that appears above the list box.

};

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTEditListBox::GetCurrentIndex() {
	return m_nIndex;
}
AFX_INLINE CXTEditListBoxToolBar& CXTEditListBox::GetEditGroup() {
	return m_editGroup;
}
AFX_INLINE void CXTEditListBox::SetDlgFilter(LPCTSTR lpszFilter/*=NULL*/) {
	m_strFilter = lpszFilter;
}
AFX_INLINE void CXTEditListBox::SetDlgInitialDir(LPCTSTR lpszInitialDir/*=NULL*/) {
	m_strInitialDir = lpszInitialDir;
}
AFX_INLINE bool CXTEditListBox::HasToolbar() {
	return ((m_dwLStyle & LBS_XT_NOTOOLBAR) == 0);
}
AFX_INLINE void CXTEditListBox::EnableEdit(bool bEnable) {
	m_bEnableEdit = bEnable; m_editGroup.EnableEdit(bEnable);
}
AFX_INLINE void CXTEditListBox::SetNewItemDefaultText(LPCTSTR lpszItemDefaultText) {
	m_strItemDefaultText = lpszItemDefaultText;
}

#endif // #if !defined(__XTEDITLISTBOX_H__)
