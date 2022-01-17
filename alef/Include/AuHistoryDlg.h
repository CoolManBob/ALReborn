#if !defined(AFX_AUHISTORYDLG_H__9685BBAF_4B70_4C70_A0F9_456E885C0789__INCLUDED_)
#define AFX_AUHISTORYDLG_H__9685BBAF_4B70_4C70_A0F9_456E885C0789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuHistoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AuHistoryDlg dialog

class AuHistoryDlg : public CDialog
{
private:
	INT32 m_iLastClickIndex;

public:
	CString m_strFileName;
	CList<FileInfo, FileInfo> m_csList;

// Construction
public:
	AuHistoryDlg(CWnd* pParent = NULL);   // standard constructor

	void InitListCtrl();
	void InitHistory();
	void SetHistoryList(CList<FileInfo, FileInfo>* pList);	// 2004.11.25. steeple
	void AddListItem(LPCTSTR lpVer, LPCTSTR lpUser, LPCTSTR lpDate);

// Dialog Data
	//{{AFX_DATA(AuHistoryDlg)
	enum { IDD = IDD_HISTORY };
	CListCtrl	m_ctrlHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AuHistoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AuHistoryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetFile();
	afx_msg void OnClickHistory(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUHISTORYDLG_H__9685BBAF_4B70_4C70_A0F9_456E885C0789__INCLUDED_)
