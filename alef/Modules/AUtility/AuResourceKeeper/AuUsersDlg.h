#if !defined(AFX_AUUSERSDLG_H__37DF80E2_0554_4315_B406_17DB829CA614__INCLUDED_)
#define AFX_AUUSERSDLG_H__37DF80E2_0554_4315_B406_17DB829CA614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuUsersDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AuUsersDlg dialog
#include "RKInclude.h"

class AuUsersDlg : public CDialog
{
private:
	INT32 m_iRClickIndex;
public:
	CRKUserManager m_UserManager;

// Construction
public:
	AuUsersDlg(CWnd* pParent = NULL);   // standard constructor
	void InitListCtrl();
	void InitUserList();
	void AddListItem(LPCTSTR lpUserID);
// Dialog Data
	//{{AFX_DATA(AuUsersDlg)
	enum { IDD = IDD_USERS };
	CListCtrl	m_ctrlUsers;
	CString	m_strUserID;
	CString	m_strPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AuUsersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AuUsersDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUserDelete();
	afx_msg void OnAddUser();
	afx_msg void OnClickUsersList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUUSERSDLG_H__37DF80E2_0554_4315_B406_17DB829CA614__INCLUDED_)
