#if !defined(AFX_KEYREFERENCEDLG_H__03F2918E_3759_4FCC_91E9_7B582790DA6F__INCLUDED_)
#define AFX_KEYREFERENCEDLG_H__03F2918E_3759_4FCC_91E9_7B582790DA6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeyReferenceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKeyReferenceDlg dialog

class CKeyReferenceDlg : public CDialog
{
// Construction
public:
	CKeyReferenceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyReferenceDlg)
	enum { IDD = IDD_KEYREFERENCE };
	CString	m_strKeyRef;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyReferenceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpKeyreference();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYREFERENCEDLG_H__03F2918E_3759_4FCC_91E9_7B582790DA6F__INCLUDED_)
