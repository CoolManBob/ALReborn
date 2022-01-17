// XListCtrlTestDlg.h : header file
//

#ifndef XLISTCTRLTESTDLG_H
#define XLISTCTRLTESTDLG_H

#include "XListCtrl.h" 


///////////////////////////////////////////////////////////////////////////////
// CXListCtrlTestDlg dialog

class CXListCtrlTestDlg : public CDialog
{
// Construction
public:
	CXListCtrlTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXListCtrlTestDlg)
	enum { IDD = IDD_LISTCTRLTEST_DIALOG };
	CListBox	m_Log;
	CXListCtrl	m_List;
	BOOL	m_bGridlines;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrlTestDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	CStringArray m_sa1, m_sa2;
	BOOL	m_bRunning;
	static int m_nColWidths[];

	void FillListCtrl(CXListCtrl * pList);
	void InitListCtrl(CXListCtrl * pList);
	void _cdecl Log(LPCTSTR lpszFmt, ...);

	// Generated message map functions
	//{{AFX_MSG(CXListCtrlTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButton1();
	afx_msg void OnDestroy();
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClear();
	afx_msg void OnButton2();
	afx_msg void OnGridlines();
	//}}AFX_MSG
	afx_msg LRESULT OnComboSelection(WPARAM, LPARAM);
	afx_msg LRESULT OnCheckbox(WPARAM, LPARAM);
	afx_msg LRESULT OnEditEnd(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XLISTCTRLTESTDLG_H
