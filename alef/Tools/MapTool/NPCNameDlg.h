#if !defined(AFX_NPCNAMEDLG_H__7E7CB519_51CA_430F_A9EB_C6CC021A907C__INCLUDED_)
#define AFX_NPCNAMEDLG_H__7E7CB519_51CA_430F_A9EB_C6CC021A907C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NPCNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNPCNameDlg dialog

class CNPCNameDlg : public CDialog
{
// Construction
public:
	CNPCNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNPCNameDlg)
	enum { IDD = IDD_NPCNAMEEDIT };
	CString	m_strName;
	CString	m_strNameOrigin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNPCNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNPCNameDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NPCNAMEDLG_H__7E7CB519_51CA_430F_A9EB_C6CC021A907C__INCLUDED_)
