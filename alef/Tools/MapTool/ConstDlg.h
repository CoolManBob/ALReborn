#if !defined(AFX_CONSTDLG_H__0078580C_E5CE_4E53_BE20_E718EE2361CB__INCLUDED_)
#define AFX_CONSTDLG_H__0078580C_E5CE_4E53_BE20_E718EE2361CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConstDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConstDlg dialog

class CConstDlg : public CDialog
{
// Construction
public:
	CConstDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConstDlg)
	enum { IDD = IDD_CONST };
	int		m_nDepth		;
	int		m_nHoriCount	;
	float	m_fSectorWidth	;
	int		m_nVertCount	;
	//}}AFX_DATA
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConstDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSTDLG_H__0078580C_E5CE_4E53_BE20_E718EE2361CB__INCLUDED_)
