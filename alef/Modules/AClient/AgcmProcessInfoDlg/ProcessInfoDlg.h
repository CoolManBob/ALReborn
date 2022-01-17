#if !defined(AFX_PROCESSINFODLG_H__C49F76AA_E1CB_4A62_94BD_65CC3107531A__INCLUDED_)
#define AFX_PROCESSINFODLG_H__C49F76AA_E1CB_4A62_94BD_65CC3107531A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProcessInfoDlg dialog

class CProcessInfoDlg : public CDialog
{
// Construction
public:
	CProcessInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcessInfoDlg)
	enum { IDD = IDD_PROCESS };
	CString	m_strInfo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessInfoDlg)
	public:
	virtual BOOL Create(CHAR *szInitInfo, CWnd* pParentWnd = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	VOID SetProcessInfo(CHAR *szInfo);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcessInfoDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSINFODLG_H__C49F76AA_E1CB_4A62_94BD_65CC3107531A__INCLUDED_)
