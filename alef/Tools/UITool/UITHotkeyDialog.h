#if !defined(AFX_UITHOTKEYDIALOG_H__CDFA0B56_955D_4989_A9CA_5BC399E93873__INCLUDED_)
#define AFX_UITHOTKEYDIALOG_H__CDFA0B56_955D_4989_A9CA_5BC399E93873__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITHotkeyDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITHotkeyDialog dialog

class UITHotkeyDialog : public CDialog
{
// Construction
private:
	AgcdUI *			m_pcsUI;
	AgcdUIHotkey *		m_pstHotkey;

public:
	UITHotkeyDialog(AgcdUI *pcsUI, CWnd* pParent = NULL);   // standard constructor
	BOOL RefreshHotkeyList();
	VOID RefreshActionList();

// Dialog Data
	//{{AFX_DATA(UITHotkeyDialog)
	enum { IDD = IDD_UIT_HOTKEY };
	CComboBox	m_csKeyCode;
	CComboBox	m_csKeyType;
	CListCtrl	m_csActionList;
	CListCtrl	m_csHotkeyList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITHotkeyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITHotkeyDialog)
	afx_msg void OnUITAddAction();
	afx_msg void OnUITEditAction();
	afx_msg void OnUITDeleteAction();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedUITEventList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedUITHotkeyList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUITAddHotkey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCustomizable;
public:
	CString m_strDescription;
public:
	afx_msg void OnBnClickedHotkeyCustomizable();
public:
	afx_msg void OnEnChangeHotkeyDescription();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITHOTKEYDIALOG_H__CDFA0B56_955D_4989_A9CA_5BC399E93873__INCLUDED_)
