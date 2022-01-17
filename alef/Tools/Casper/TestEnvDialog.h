#if !defined(AFX_TESTENVDIALOG_H__C20FC4F8_AD5C_42CD_B156_B1BB308FEC71__INCLUDED_)
#define AFX_TESTENVDIALOG_H__C20FC4F8_AD5C_42CD_B156_B1BB308FEC71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestEnvDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestEnvDialog dialog
class CTestEnvDialog : public CDialog
{
// Construction
public:
	CTestEnvDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTestEnvDialog)
	enum { IDD = IDD_TEST_ENV };
	CString	m_strHigherID;
	int		m_lID_Begin;
	int		m_lID_End;
	CString	m_strServerIP;
	UINT	m_ulServerPort;
	UINT	m_lClientNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestEnvDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestEnvDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTENVDIALOG_H__C20FC4F8_AD5C_42CD_B156_B1BB308FEC71__INCLUDED_)
