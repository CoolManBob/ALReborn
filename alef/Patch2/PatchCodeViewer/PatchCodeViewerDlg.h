// PatchCodeViewerDlg.h : header file
//

#pragma once


// CPatchCodeViewerDlg dialog
class CPatchCodeViewerDlg : public CDialog
{
// Construction
public:
	CPatchCodeViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PATCHCODEVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strPatchCodeFile;
	UINT m_ulPatchVersion;
	DWORD m_dwPatchCode;
	afx_msg void OnBnClickedSetPatchCodeFile();
	afx_msg void OnBnClickedGetCode();
};
