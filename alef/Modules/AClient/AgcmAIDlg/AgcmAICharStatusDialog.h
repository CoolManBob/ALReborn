#if !defined(AFX_AGCMAICHARSTATUSDIALOG_H__86F7629A_4513_4080_AAF4_0ED4936F21EE__INCLUDED_)
#define AFX_AGCMAICHARSTATUSDIALOG_H__86F7629A_4513_4080_AAF4_0ED4936F21EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAICharStatusDialog.h : header file
//

#include "AgpdCharacter.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharStatusDialog dialog

class AgcmAICharStatusDialog : public CDialog
{
// Construction
public:
	AgcmAICharStatusDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAICharStatusDialog)
	enum { IDD = IDD_DIALOG_AI_STATUS };
	CComboBox	m_csActionStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAICharStatusDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAICharStatusDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAICHARSTATUSDIALOG_H__86F7629A_4513_4080_AAF4_0ED4936F21EE__INCLUDED_)
