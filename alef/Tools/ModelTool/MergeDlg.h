#pragma once

#define CMERGEDLGSET_DATA_NUM	9

class CMergeDlgSet
{
public:
	CHAR m_szData[CMERGEDLGSET_DATA_NUM][AMT_MAX_STR];

	CMergeDlgSet()
	{
		memset(this, 0, AMT_MAX_STR * CMERGEDLGSET_DATA_NUM);
	}
};

/////////////////////////////////////////////////////////////////////////////
// CMergeDlg dialog

class CMergeDlg : public CDialog
{
// Construction
public:
	CMergeDlg(CMergeDlgSet *pcsSet, CHAR *pszFindFilePathName1, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMergeDlg)
	enum { IDD = IDD_MERGE };
	CString	m_strMerge1;
	CString	m_strMerge2;
	CString	m_strMerge3;
	CString	m_strMerge4;
	CString	m_strMerge5;
	CString	m_strMerge6;
	CString	m_strMerge7;
	CString	m_strMerge8;
	CString	m_strMerge9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	CMergeDlgSet	*m_pcsSet;
	CHAR			m_szFindFilePathName1[AMT_MAX_STR];

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMergeDlg)
	afx_msg void OnButtonSetMerge1();
	afx_msg void OnButtonSetMerge2();
	afx_msg void OnButtonSetMerge3();
	afx_msg void OnButtonSetMerge4();
	afx_msg void OnButtonSetMerge5();
	afx_msg void OnButtonSetMerge6();
	afx_msg void OnButtonSetMerge7();
	afx_msg void OnButtonSetMerge8();
	afx_msg void OnButtonSetMerge9();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};