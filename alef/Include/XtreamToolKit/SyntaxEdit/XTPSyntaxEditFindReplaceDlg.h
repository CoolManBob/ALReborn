// XTPSyntaxEditFindReplaceDlg.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPSYNTAXEDITFINDREPLACEDLG_H__)
#define __XTPSYNTAXEDITFINDREPLACEDLG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditView;


//===========================================================================
// Summary:
//      This class implements a Find and Replace dialogs for a SyntaxEdit
//      control.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditFindReplaceDlg : public CDialog
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// Parameters:
	//      pParentWnd - pointer to a parent window.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditFindReplaceDlg(CWnd* pParentWnd = NULL);

	int m_nSearchDirection;  // Store Search Direction: 0-up, 1-down.
	BOOL m_bMatchWholeWord;  // Store Match Whole Words option.
	BOOL m_bMatchCase;       // Store Match Case option.
	CString m_csFindText;    // Store text to find.
	CString m_csReplaceText; // Store text to replace.

	//{{AFX_DATA(CXTPSyntaxEditFindReplaceDlg)
	//{{AFX_CODEJOCK_PRIVATE
	enum { IDD = XTP_IDD_EDIT_SEARCH_REPLACE };

	CButton m_btnFindNext;
	CButton m_btnRadioUp;
	CButton m_btnRadioDown;
	CButton m_btnReplace;
	CButton m_btnReplaceAll;
	CComboBox m_wndFindCombo;
	CComboBox m_wndReplaceCombo;
	//}}AFX_CODEJOCK_PRIVATE
	//}}AFX_DATA

	//-----------------------------------------------------------------------
	// Summary:
	//      This method used to show find or replace dialog. If window is not
	//      created this method loads a dialog template from resources and
	//      created modeless dialog.
	// Parameters:
	//      pEditView   - A pointer to CXTPSyntaxEditView.
	//      bReplaceDlg - Set TRUE to create replace dialog or FALSE to create
	//                    find dialog.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: ShowWindow
	//-----------------------------------------------------------------------
	BOOL ShowDialog(CXTPSyntaxEditView* pEditView, BOOL bReplaceDlg=FALSE);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      This function used to move or add string at the top of history
	//      items (item 0).
	// Parameters:
	//      csText    - Text to move or add.
	//      arHistory - History array.
	//      wndCombo  - History combobox.
	// See Also:
	//-----------------------------------------------------------------------
	void UpdateHistoryCombo(const CString& csText, CStringArray& arHistory);
	void UpdateHistoryCombo(const CString& csText, CStringArray& arHistory, CComboBox& wndCombo); //<COMBINE CXTPSyntaxEditFindReplaceDlg::UpdateHistoryCombo@const CString&@CStringArray&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to initialize combobox from strings array.
	// Parameters:
	//      arHistory - Array of strings.
	//      wndCombo  - A combobox control to add strings from array.
	// See Also: LoadHistory
	//-----------------------------------------------------------------------
	void InitHistoryCombo(CStringArray& arHistory, CComboBox& wndCombo);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to load from application profile history for
	//      find and replace comboboxes and state for other dialog controls.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: SaveHistory
	//-----------------------------------------------------------------------
	BOOL LoadHistory();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to save to application profile history for
	//      find and replace comboboxes and state for other dialog controls.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: LoadHistory
	//-----------------------------------------------------------------------
	BOOL SaveHistory();

	//-----------------------------------------------------------------------
	// Summary:
	//      This method update controls state (enable or disable) for buttons:
	//      Find, Replace and ReplaceAll.
	//-----------------------------------------------------------------------
	void EnableControls();

	//{{AFX_VIRTUAL(CXTPSyntaxEditFindReplaceDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSyntaxEditFindReplaceDlg)
	afx_msg void OnEditChangeComboFind();
	afx_msg void OnSelendOkComboFind();
	afx_msg void OnChkMatchWholeWord();
	afx_msg void OnChkMatchCase();
	afx_msg void OnRadioUp();
	afx_msg void OnRadioDown();
	afx_msg void OnBtnFindNext();
	afx_msg void OnBtnReplace();
	afx_msg void OnBtnReplaceAll();
	afx_msg void OnEditChangeComboReplace();
	afx_msg void OnSelendOkComboReplace();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

	BOOL                m_bReplaceDlg;      // Stored dialog type.
	CPoint              m_ptWndPos;         // Stored window position.
	CXTPSyntaxEditView* m_pEditView;        // Stored attached view.
};


#endif // !defined(__XTPSYNTAXEDITFINDREPLACEDLG_H__)
