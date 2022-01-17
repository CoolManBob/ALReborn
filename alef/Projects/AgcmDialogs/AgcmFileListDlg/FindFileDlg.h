#pragma once

class CFindFileDlg : public CDialog
{
public:
	CFindFileDlg(CHAR *pszDest, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CFindFileDlg)
	enum { IDD = IDD_FILE_LIST_FIND };
	CString	m_strFindFile;
	//}}AFX_DATA

	CHAR	*m_pszDest;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};