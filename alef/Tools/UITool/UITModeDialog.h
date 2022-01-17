#if !defined(AFX_UITMODEDIALOG_H__C8C598E2_7515_4CE9_94F0_E6486E458947__INCLUDED_)
#define AFX_UITMODEDIALOG_H__C8C598E2_7515_4CE9_94F0_E6486E458947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITModeDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITModeDialog dialog

class UITModeDialog : public CDialog
{
public:
	AgcdUIMode	m_eMode;

// Construction
public:
	UITModeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UITModeDialog)
	enum { IDD = IDD_UIT_UI_MODE };
	CComboBox	m_csModeList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITModeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITModeDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITMODEDIALOG_H__C8C598E2_7515_4CE9_94F0_E6486E458947__INCLUDED_)
