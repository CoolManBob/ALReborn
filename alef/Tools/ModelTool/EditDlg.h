#pragma once

class CEditDlg : public CDialog
{
public:
	CEditDlg(CHAR *szEdit, CHAR *szCaption = NULL, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CEditDlg)
	enum { IDD = IDD_EDIT };
	CString	m_strEdit;
	//}}AFX_DATA

protected:
	CHAR m_szCaption[AMT_MAX_STR];
	CHAR *m_pszEdit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};