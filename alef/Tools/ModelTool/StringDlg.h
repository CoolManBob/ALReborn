#pragma once

class CStringDlg : public CDialog
{
public:
	CStringDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CStringDlg)
	enum { IDD = IDD_STRING };
	CString	m_strCur;
	//}}AFX_DATA
	
	VOID SetString(CHAR *szStr);

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};