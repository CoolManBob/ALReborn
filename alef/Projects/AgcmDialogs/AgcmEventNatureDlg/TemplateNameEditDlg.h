#if !defined(AFX_TEMPLATENAMEEDITDLG_H__54284D05_E7C3_4E87_A2BF_C32094F24D73__INCLUDED_)
#define AFX_TEMPLATENAMEEDITDLG_H__54284D05_E7C3_4E87_A2BF_C32094F24D73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateNameEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateNameEditDlg dialog

class CTemplateNameEditDlg : public CDialog
{
// Construction
public:
	CTemplateNameEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateNameEditDlg)
	enum { IDD = IDD_TEMPLATE_NAME_DLG };
	CString	m_strName;
	BOOL	m_bUseCloud;
	BOOL	m_bUseEffect;
	BOOL	m_bUseFog;
	BOOL	m_bUseLight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateNameEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateNameEditDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATENAMEEDITDLG_H__54284D05_E7C3_4E87_A2BF_C32094F24D73__INCLUDED_)
