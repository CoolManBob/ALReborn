// SoundPackerDlg.h : header file
//

#if !defined(AFX_SOUNDPACKERDLG_H__67E94DF1_BBD1_47FA_9B50_400B461CA07B__INCLUDED_)
#define AFX_SOUNDPACKERDLG_H__67E94DF1_BBD1_47FA_9B50_400B461CA07B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mss.h"

/////////////////////////////////////////////////////////////////////////////
// CSoundPackerDlg dialog

class CSoundPackerDlg : public CDialog
{
// Construction
public:
	CSoundPackerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSoundPackerDlg)
	enum { IDD = IDD_SOUNDPACKER_DIALOG };
	CString	m_strResult;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundPackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	BOOL SetDirectoryFiles(char*	szDirectory);
	void PrintInfo(char*	szMessage);

	HDIGDRIVER			m_drvDigital;
	HPROVIDER			m_hProvider;

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSoundPackerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonPack();
	afx_msg void OnClose();
	afx_msg void OnButtonUnpack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDPACKERDLG_H__67E94DF1_BBD1_47FA_9B50_400B461CA07B__INCLUDED_)
