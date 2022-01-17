#if !defined(AFX_UITCUSTOMCONTROLDIALOG_H__383B0B35_7C42_4D1A_8F18_F48470286D46__INCLUDED_)
#define AFX_UITCUSTOMCONTROLDIALOG_H__383B0B35_7C42_4D1A_8F18_F48470286D46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITCustomControlDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITCustomControlDialog dialog

class UITCustomControlDialog : public CDialog
{
public:
	AgcdUICControl *	m_pcsCControl;

// Construction
public:
	UITCustomControlDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UITCustomControlDialog)
	enum { IDD = IDD_UIT_SELECT_CUSTOM };
	CComboBox	m_csCustomControls;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITCustomControlDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITCustomControlDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITCUSTOMCONTROLDIALOG_H__383B0B35_7C42_4D1A_8F18_F48470286D46__INCLUDED_)
