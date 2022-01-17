#if !defined(AFX_AGCMAIFACTORDIALOG_H__A7B2843F_A41A_4329_93C8_030D83FD94B9__INCLUDED_)
#define AFX_AGCMAIFACTORDIALOG_H__A7B2843F_A41A_4329_93C8_030D83FD94B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAIFactorDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmAIFactorDialog dialog

class AgcmAIFactorDialog : public CDialog
{
// Construction
public:
	AgcmAIFactorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAIFactorDialog)
	enum { IDD = IDD_DIALOG_AI_FACTOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAIFactorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAIFactorDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAIFACTORDIALOG_H__A7B2843F_A41A_4329_93C8_030D83FD94B9__INCLUDED_)
