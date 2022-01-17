#if !defined(AFX_AULOGINDLG_H__94082553_809A_4090_B5FE_F697460C5477__INCLUDED_)
#define AFX_AULOGINDLG_H__94082553_809A_4090_B5FE_F697460C5477__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuLoginDlg.h : header file
//
#include "RKInclude.h"

/////////////////////////////////////////////////////////////////////////////
// AuLoginDlg dialog
class AuLoginDlg : public CDialog
{
public:
	INT32	m_iLastClickIndex;
	BOOL	m_bLoginSuccess;
	CRKRepository m_Repository;
	CRKUserManager m_UserManager;

// Construction
public:
	AuLoginDlg(CWnd* pParent = NULL);   // standard constructor
	void InitListCtrl();
	BOOL InitConfig();
	void AddListItem(LPCTSTR lpName, LPCTSTR lpPath);
// Dialog Data
	//{{AFX_DATA(AuLoginDlg)
	enum { IDD = IDD_LOGIN };
	CListCtrl	m_ctrlList;
	CString	m_strPassword;
	CString	m_strUserID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AuLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AuLoginDlg)
	afx_msg void OnLogin();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickRepoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddRepository();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AULOGINDLG_H__94082553_809A_4090_B5FE_F697460C5477__INCLUDED_)
