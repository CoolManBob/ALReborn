#pragma once

class CFileListDlg : public CDialog
{
	static int	lastsel;

public:
	CFileListDlg(CHAR *szPathName1, CHAR *szPathName2 = NULL, CHAR *szPathName3 = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileListDlg)
	enum { IDD = IDD_FILE_LIST };
	CListBox	m_csFileList;
	//}}AFX_DATA

protected:
	CHAR m_szPathName1[256];
	CHAR m_szPathName2[256];
	CHAR m_szPathName3[256];

protected:
	VOID SetFindFile(CHAR *szCmp);
	BOOL IsLowEqual(CHAR *szCmp1, CHAR *szCmp2);
	VOID _ToUpper(CHAR *szDest, CHAR *szSrc);
	VOID ReCalcStatusHorizon();

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CFileListDlg)
	afx_msg void OnDblclkFileList();
	afx_msg void OnFileListFind();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};