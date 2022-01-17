#if !defined(AFX_EDITDLG_H__E2927D2E_4F3C_44F7_8F80_7786477FCFC8__INCLUDED_)
#define AFX_EDITDLG_H__E2927D2E_4F3C_44F7_8F80_7786477FCFC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog

#include "../resource.h"

class CEditDlg : public CDialog
{
// Construction
public:
	CEditDlg(CHAR *pszDest, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditDlg)
	enum { IDD = IDD_EDIT_DLG };
	CString	m_strEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHAR *m_pszDest;

	// Generated message map functions
	//{{AFX_MSG(CEditDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITDLG_H__E2927D2E_4F3C_44F7_8F80_7786477FCFC8__INCLUDED_)
