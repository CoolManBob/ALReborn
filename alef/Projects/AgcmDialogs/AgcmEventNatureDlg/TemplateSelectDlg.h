#if !defined(AFX_TEMPLATESELECTDLG_H__6281E1FD_B68B_45FF_AC3E_693A1C757305__INCLUDED_)
#define AFX_TEMPLATESELECTDLG_H__6281E1FD_B68B_45FF_AC3E_693A1C757305__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateSelectDlg dialog
#include "AgcmEventNatureDlg.h"

#define	TEMPLATE_LIST_FORMAT	"%03d , %s"

class CTemplateSelectDlg : public CDialog
{
protected:
	// Data members..
	AgcmEventNatureDlg *	m_csAgcmEventNatureDlg;

// Construction
public:
	void UpdateList();
	CTemplateSelectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateSelectDlg)
	enum { IDD = IDD_TEMPLATE_SELECT_DLG };
	CListBox	m_ctlTemplateList;
	BOOL	m_bTypeBGM;
	BOOL	m_bTypeEffect;
	BOOL	m_bTypeSky;
	//}}AFX_DATA

	UINT	DoModal( AgcmEventNatureDlg * pParent );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateSelectDlg)
	afx_msg void OnAddTemplate();
	afx_msg void OnEditTemplate();
	afx_msg void OnRemoveTemplate();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRenameTemplate();
	afx_msg void OnDblclkTemplateList();
	afx_msg void OnTypeSky();
	afx_msg void OnTypeEffect();
	afx_msg void OnTypeBgm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATESELECTDLG_H__6281E1FD_B68B_45FF_AC3E_693A1C757305__INCLUDED_)
