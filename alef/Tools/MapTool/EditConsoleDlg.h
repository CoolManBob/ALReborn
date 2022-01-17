#if !defined(AFX_EDITCONSOLEDLG_H__72C50C03_6E54_40F6_A9C1_306122586131__INCLUDED_)
#define AFX_EDITCONSOLEDLG_H__72C50C03_6E54_40F6_A9C1_306122586131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditConsoleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditConsoleDlg dialog
#include "ApUtil.h"

class CEditConsoleDlg : public CDialog
{
public:
	static CStringList	m_sCommandHistory;

	CGetArg2	m_Argument	;
	int		m_nCommandType	;
	POINT	m_Pos			;

	int		GetCommandIndex( CString str );

// Construction
public:
	CEditConsoleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditConsoleDlg)
	enum { IDD = IDD_COMMANDCONSOLE };
	CString	m_strCommand;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditConsoleDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditConsoleDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCONSOLEDLG_H__72C50C03_6E54_40F6_A9C1_306122586131__INCLUDED_)
