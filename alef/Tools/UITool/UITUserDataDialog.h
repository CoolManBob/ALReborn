#if !defined(AFX_UITUSERDATADIALOG_H__7AB7ADDD_329D_446D_8958_940CFB109A7E__INCLUDED_)
#define AFX_UITUSERDATADIALOG_H__7AB7ADDD_329D_446D_8958_940CFB109A7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITUserDataDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITUserDataDialog dialog

class UITUserDataDialog : public CDialog
{
private:
	AgcdUIControl *		m_pcsControl;

// Construction
public:
	UITUserDataDialog(AgcdUIControl *pcsControl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UITUserDataDialog)
	enum { IDD = IDD_UIT_USER_DATA };
	CComboBox	m_csUserDataList;
	long	m_lUserDataIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITUserDataDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITUserDataDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITUSERDATADIALOG_H__7AB7ADDD_329D_446D_8958_940CFB109A7E__INCLUDED_)
