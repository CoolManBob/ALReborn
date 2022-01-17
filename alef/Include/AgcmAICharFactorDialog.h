#if !defined(AFX_AgcmAICharFactorDIALOG_H__A7B2843F_A41A_4329_93C8_030D83FD94B9__INCLUDED_)
#define AFX_AgcmAICharFactorDIALOG_H__A7B2843F_A41A_4329_93C8_030D83FD94B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAICharFactorDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharFactorDialog dialog

class AgcmAICharFactorDialog : public CDialog
{
// Construction
public:
	AgcmAICharFactorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAICharFactorDialog)
	enum { IDD = IDD_DIALOG_AI_FACTOR };
	int		m_lCompare;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAICharFactorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAICharFactorDialog)
	afx_msg void OnAIFactorEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AgcmAICharFactorDIALOG_H__A7B2843F_A41A_4329_93C8_030D83FD94B9__INCLUDED_)
