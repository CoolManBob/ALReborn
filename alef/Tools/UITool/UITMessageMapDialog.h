#if !defined(AFX_UITMESSAGEMAPDIALOG_H__30037256_DF9B_4FC6_AAE8_003844AC977A__INCLUDED_)
#define AFX_UITMESSAGEMAPDIALOG_H__30037256_DF9B_4FC6_AAE8_003844AC977A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITMessageMapDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITMessageMapDialog dialog

class UITMessageMapDialog : public CDialog
{
private:
	AgcdUI *			m_pcsUI;
	AgcdUIControl *		m_pcsControl;
	AgcdUIMessageMap *	m_pstMessageMap;

// Construction
public:
	UITMessageMapDialog(AgcdUI *pcsUI, AgcdUIControl *pcsControl, CWnd* pParent = NULL);   // standard constructor
	VOID RefreshActionList();

// Dialog Data
	//{{AFX_DATA(UITMessageMapDialog)
	enum { IDD = IDD_UIT_MESSAGE_MAPS };
	CListCtrl	m_csActionList;
	CListCtrl	m_csMessageList;
	CString	m_strMessageName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITMessageMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITMessageMapDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedUITMessageList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnUITDeleteAction();
	afx_msg void OnUITAddAction();
	afx_msg void OnUITEditAction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITMESSAGEMAPDIALOG_H__30037256_DF9B_4FC6_AAE8_003844AC977A__INCLUDED_)
