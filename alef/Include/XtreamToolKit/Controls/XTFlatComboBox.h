// XTFlatComboBox.h interface for the CXTFlatComboBox class.
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
#if !defined(__XTFLATCOMBOBOX_H__)
#define __XTFLATCOMBOBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTFlatComboBoxTheme;
class CXTFlatEditTheme;

#include "XTThemeManager.h"

//===========================================================================
// Summary:
//     CXTFlatComboBox is a CComboBox derived class. The CXTFlatComboBox class
//     can easily give your application's standard CComboBox control the Flat
//     Look that is seen in many of the Microsoft(r) Office(tm) products.
// Remarks:
//     To use the control, use Class Wizard to add the CComboBox member variable (
//     Class Wizard -> Member Variables Tab -> Add Variable) and rename CComboBox
//     to CXTFlatComboBox.
//===========================================================================
class _XTP_EXT_CLASS CXTFlatComboBox : public CComboBox, public CXTThemeManagerStyleHostBase
{
	DECLARE_DYNAMIC(CXTFlatComboBox)
	DECLARE_THEME_HOST(CXTFlatComboBox)
	DECLARE_THEME_REFRESH(CXTFlatComboBox)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatComboBox object
	//-----------------------------------------------------------------------
	CXTFlatComboBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTFlatComboBox object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTFlatComboBox();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will disable the flat look for the CXTFlatComboBox.
	// Parameters:
	//     bDisable - TRUE to disable flat appearance.
	//-----------------------------------------------------------------------
	void DisableFlatLook(BOOL bDisable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the appearance of the
	//     CXTFlatComboBox control is flat.
	// Returns:
	//     TRUE if the control is flat, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsFlat();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function enables or disables auto completion.
	// Parameters:
	//     bEnable - TRUE to enable auto completion, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual void EnableAutoCompletion(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the mouse is within the control's
	//     rect.
	// Returns:
	//     TRUE if the mouse is over the CXTFlatComboBox, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL PointInRect();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the control has the focus.
	// Returns: TRUE if the control has the focus. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary      : Call this member function to set the background color
	//                for the CXTFlatComboBox control.
	// Parameters   : crBack - RGB value representing background color.
	// See Also     : SetTextColor, GetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	virtual void SetBackColor(COLORREF crBack) {
		m_crBack = crBack;
	}
	//-----------------------------------------------------------------------
	// Summary      : Call this member function to retrieve the background color
	//                for the CXTFlatComboBox.
	// Returns      : An RGB color value representing the background color for
	//                the CXTFlatComboBox.
	// See Also     : SetTextColor, SetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const {
		return (m_crBack == COLORREF_NULL) ? GetXtremeColor(COLOR_WINDOW) : m_crBack;
	}
	//-----------------------------------------------------------------------
	// Summary      : Call this member function to set the text color
	//                for the CXTFlatComboBox control.
	// Parameters   : crText - RGB value representing text color.
	// See Also     : SetBackColor, GetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	virtual void SetTextColor(COLORREF crText) {
		m_crText = crText;
	}
	//-----------------------------------------------------------------------
	// Summary      : Call this member function to retrieve the text color
	//                for the CXTFlatComboBox.
	// Returns      : An RGB color value representing the text color for
	//                the CXTFlatComboBox.
	// See Also     : SetTextColor, SetBackColor, GetBackColor
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const {
		return (m_crText == COLORREF_NULL) ? GetXtremeColor(COLOR_WINDOWTEXT) : m_crText;
	}

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTFlatComboBox whenever a paint
	//     operation should occur.
	// Parameters:
	//     pDC    - Points to the current device context.
	//     eState - Current state of the CXTFlatComboBox, either xtMouseNormal, xtMouseHover, or xtMouseSelect.
	// See Also:
	//     XTMouseState
	//-----------------------------------------------------------------------
	virtual void DrawCombo(CDC* pDC, XTMouseState eState);

	//-----------------------------------------------------------------------
	// Summary:
	//     A helper for rendering the control appearance.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//-----------------------------------------------------------------------
	virtual void DoPaint(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine the actual client area
	//      for the CXTFlatComboBox control. The actual client area is the display area
	//      excluding borders and drop arrow box.
	// Parameters:
	//      rc - Reference to a CRect object to receive client coordinates.
	//-----------------------------------------------------------------------
	virtual void CalcClientRect(CRect& rc);

protected:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTFlatComboBox)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTFlatComboBox)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetFocus();
	afx_msg BOOL OnKillFocus();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEditUpdate();
	afx_msg BOOL OnEndSel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnPrintClient(WPARAM wp, LPARAM lp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE

public:
	BOOL m_bAutoComp;   // Used by Auto completing.
	BOOL m_bFlatLook;   // TRUE if the control is flat.
	BOOL m_bDisableAC;  // TRUE when auto complete is disabled internally.

protected:
	BOOL m_bPainted;    // Used during paint operations.
	BOOL m_bHasFocus;   // TRUE if the control has the focus.
	DWORD m_nStyle;     // Stores the standard window styles for the control.
	DWORD m_nStyleEx;   // Stores the extended window styles for the control.
	COLORREF m_crBack;  // RGB color value representing background color.
	COLORREF m_crText;  // RGB color value representing text color.
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTFlatComboBox::EnableAutoCompletion(BOOL bEnable/*=TRUE*/) {
	m_bAutoComp = bEnable;
}
AFX_INLINE BOOL CXTFlatComboBox::IsFlat() {
	return m_bFlatLook;
}
AFX_INLINE BOOL CXTFlatComboBox::IsFocused() const {
	return m_bHasFocus;
}

//===========================================================================
// Summary:
//     CXTEdit is a CEdit derived class. The CXTEdit class provides the functionality
//     of a Windows edit control. An edit control is a rectangular child window
//     in which the user can enter text.
// Remarks:
//     You can create an edit control either from a dialog template or directly
//     in your code. In both cases, first call the constructor CXTEdit to construct
//     the CXTEdit object, then call the Create member function to create the
//     Windows edit control and attach it to the CXTEdit object.
//
//     Construction can be a one-step process in a class derived from CXTEdit.
//     Write a constructor for the derived class and call Create from within
//     the constructor.
//
//     CXTEdit inherits significant functionality from CWnd. To set and retrieve
//     text from a CXTEdit object, use the CWnd member functions SetWindowText
//     and GetWindowText, which either set or get the entire contents of an edit control
//     even if it is a multiline control. Also, if an edit control is multiline,
//     get and set part of the control's text by calling the CXTEdit member
//     functions GetLine, SetSel, GetSel, and ReplaceSel.
//
//     If you want to handle Windows notification messages sent by an edit
//     control to its parent (usually a class derived from CDialog), add a
//     message-map entry and message-handler member function to the parent
//     class for each message.
//
//     Each message-map entry takes the following form:
//
// <code>
// ON_Notification(id, memberFxn)
// </code>
//
//          where 'id' specifies the child window ID of the edit control sending the
//          notification, and 'memberFxn' is the name of the parent member function
//          you have written to handle the notification.
//
//          The parent's function prototype is as follows:
//
// <code>
// afx_msg void memberFxn();
// </code>
//
//          Following is a list of potential message-map entries and a description
//          of the cases in which they would be sent to the parent:
//
//        * <b>ON_EN_CHANGE</b> The user has taken an action that may have altered
//          text in an edit control. Unlike the EN_UPDATE notification message,
//          this notification message is sent after Windows updates the display.
//        * <b>ON_EN_ERRSPACE</b> The edit control cannot allocate enough memory
//          to meet a specific request.
//        * <b>ON_EN_HSCROLL</b> The user clicks an edit control's horizontal scroll
//          bar. The parent window is notified before the screen is updated.
//        * <b>ON_EN_KILLFOCUS</b> The edit control loses the input focus.
//        * <b>ON_EN_MAXTEXT</b> The current insertion has exceeded the specified
//          number of characters for the edit control and has been truncated.
//          Also sent when an edit control does not have the ES_AUTOHSCROLL
//          style and the number of characters to be inserted would exceed the
//          width of the edit control. Also sent when an edit control does not
//          have the ES_AUTOVSCROLL style and the total number of lines resulting
//          from a text insertion would exceed the height of the edit control.
//        * <b>ON_EN_SETFOCUS</b> Sent when an edit control receives the input focus.
//        * <b>ON_EN_UPDATE</b> The edit control is about to display altered text.
//          Sent after the control has formatted the text but before it screens
//          the text so that the window size can be altered, if necessary.
//        * <b>ON_EN_VSCROLL</b> The user clicks an edit control's vertical scroll
//          bar. The parent window is notified before the screen is updated.
//
//     If you create a CXTEdit object within a dialog box, the CXTEdit object
//     is automatically destroyed when the user closes the dialog box.
//
//     If you create a CXTEdit object from a dialog resource using the dialog
//     editor, the CXTEdit object is automatically destroyed when the user
//     closes the dialog box.
//
//     If you create a CXTEdit object within a window, you may also need to
//     destroy it. If you create the CXTEdit object on the stack, it is destroyed
//     automatically. If you create the CXTEdit object on the heap by using
//     the new function, you must call delete on the object to destroy it when
//     the user terminates the Windows edit control. If you allocate any memory
//     in the CXTEdit object, override the CXTEdit destructor to dispose of
//     the allocations.
//===========================================================================
class _XTP_EXT_CLASS CXTEdit : public CEdit
{
	DECLARE_DYNAMIC(CXTEdit)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTEdit object
	//-----------------------------------------------------------------------
	CXTEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTEdit object, handles clean up and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTEdit();

public:
	// ------------------------------------------------------------------------
	// Summary:
	//     Determines if the clipboard contains valid information.
	// Returns:
	//     TRUE if the Clipboard contains data in a format which this edit view
	//     can accept, otherwise it returns FALSE.
	// Remarks:
	//     Call this member function to determine if the Clipboard contains
	//     information that can be pasted into this edit view.
	// ------------------------------------------------------------------------
	virtual BOOL CanPaste();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if a selection has been made.
	// Returns:
	//     TRUE if a selection has been made, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL SelectionMade();

	// ----------------------------------------------------------------------
	// Summary:
	//     Initializes the CXTEdit control.
	// Remarks:
	//     Call this member function to initialize edit control. You should
	//     always call this after you have created or subclassed your edit
	//     control.
	// Parameters:
	//     pParentWnd -  Pointer to the parent of the edit control.
	// Returns:
	//     True if the control was successfully initialized, otherwise false.
	// ----------------------------------------------------------------------
	virtual bool Initialize(CWnd* pParentWnd);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTEdit)
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CWnd* m_pParentWnd; // Pointer to the parent of the edit control.

};

//===========================================================================
// Summary:
//     CXTFlatEdit is a CXTEdit derived class. The CXTFlatEdit class can easily
//     give your application's standard CEdit control the flat look that is
//     seen in many of the Microsoft(r) Office(tm) products.
// Remarks:
//     To use the control, use Class Wizard to add the CEdit member variable (Class Wizard ->
//     Member Variables Tab -> Add Variable) and rename CEdit to CXTFlatEdit.
//===========================================================================
class _XTP_EXT_CLASS CXTFlatEdit : public CXTEdit, public CXTThemeManagerStyleHostBase
{
	DECLARE_DYNAMIC(CXTFlatEdit)
	DECLARE_THEME_HOST(CXTFlatEdit)
	DECLARE_THEME_REFRESH(CXTFlatEdit)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatEdit object
	//-----------------------------------------------------------------------
	CXTFlatEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTFlatEdit object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTFlatEdit();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will disable the flat look for the edit control.
	// Parameters:
	//     bDisable - TRUE to disable the flat appearance.
	//-----------------------------------------------------------------------
	void DisableFlatLook(BOOL bDisable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the appearance of the edit
	//     control is flat.
	// Returns:
	//     TRUE if the control is flat, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsFlat();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the mouse is within the control's
	//     rect.
	// Returns:
	//     TRUE if the mouse is over the edit box, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL PointInRect();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the edit control whenever a paint
	//     operation should occur.
	// Parameters:
	//     pDC    - Points to the current device context.
	//     rWindow - Area to render.
	// See Also:
	//     XTMouseState
	//-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, const CRect& rWindow);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTFlatEdit)
	afx_msg void OnNcPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnPrintClient(WPARAM wp, LPARAM lp);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bPainted;  // Used during paint operations.
	BOOL m_bHasFocus; // TRUE if the control has focus.
	BOOL m_bFlatLook; // TRUE if the control is flat.
	DWORD m_nStyle;   // Stores the standard window styles for the control.
	DWORD m_nStyleEx; // Stores the extended window styles for the control.
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTFlatEdit::IsFlat() {
	return m_bFlatLook;
}

#endif // #if !defined(__XTFLATCOMBOBOX_H__)
