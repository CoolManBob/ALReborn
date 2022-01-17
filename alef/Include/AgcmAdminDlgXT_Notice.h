#if !defined(AFX_AGCMADMINDLGXT_NOTICE_H__4C6958E5_B904_4551_B8EB_D0D8A3C9F796__INCLUDED_)
#define AFX_AGCMADMINDLGXT_NOTICE_H__4C6958E5_B904_4551_B8EB_D0D8A3C9F796__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Notice.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Notice dialog

class AgcmAdminDlgXT_Notice : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Notice(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Notice)
	enum { IDD = IDD_NOTICE };
	CEdit	m_csNoticeEdit;
	CXTButton	m_csNoticeBtn;
	CString	m_szNotice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Notice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;
	
	ADMIN_CB m_pfCBNotice;

	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	// Callback Function Regist
	BOOL SetCBNotice(ADMIN_CB pfCBNotice);

	void Lock();
	void Unlock();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Notice)
	virtual BOOL OnInitDialog();
	afx_msg void OnBNotice();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_NOTICE_H__4C6958E5_B904_4551_B8EB_D0D8A3C9F796__INCLUDED_)
