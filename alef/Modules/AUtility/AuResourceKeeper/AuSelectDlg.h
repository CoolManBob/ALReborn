#if !defined(AFX_AUSELECTDLG_H__1193B918_C415_4829_B3FC_E71CFA77140A__INCLUDED_)
#define AFX_AUSELECTDLG_H__1193B918_C415_4829_B3FC_E71CFA77140A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuSelectDlg.h : header file
//
#include <afxtempl.h>
#include "AuRK_API.h"

/////////////////////////////////////////////////////////////////////////////
// AuSelectDlg dialog

class AuSelectDlg : public CDialog
{
private:
	CList<FileInfo, FileInfo> *m_pUpdatedFileList;
	CList<FileInfo, FileInfo> *m_pSelectedFileList;

private:
	void SetAllItemCheck(BOOL bCheck);

// Construction
public:
	AuSelectDlg(CWnd* pParent = NULL);   // standard constructor
	void SetFileList(CList<FileInfo, FileInfo> *pUpdatedFileList, CList<FileInfo, FileInfo> *pSelectedFileList);
	void AddListItem(LPCTSTR lpText, LPCTSTR lpDate);
	void InitListItem();
	void InitListCtrl();

// Dialog Data
	//{{AFX_DATA(AuSelectDlg)
	enum { IDD = IDD_SELECT };
	CListCtrl	m_ctrlSelect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AuSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AuSelectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSelectAll();
	afx_msg void OnClean();
	afx_msg void OnGetFiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUSELECTDLG_H__1193B918_C415_4829_B3FC_E71CFA77140A__INCLUDED_)
