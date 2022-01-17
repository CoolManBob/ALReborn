#if !defined(AFX_AGCMEVENTNPCDIALOGSELECTTEMPLATEDLG_H__2FF2E304_442E_47B1_B578_A34161663A8E__INCLUDED_)
#define AFX_AGCMEVENTNPCDIALOGSELECTTEMPLATEDLG_H__2FF2E304_442E_47B1_B578_A34161663A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmEventNPCDialogSelectTemplateDlg.h : header file
//

#include "../Resource.h"
#include "AgpmEventNPCDialog.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventNPCDialogSelectTemplateDlg dialog

class AgcmEventNPCDialogSelectTemplateDlg : public CDialog
{
// Construction
	AgpdEventNPCDialogData	*m_pcsNPCDialogData;

	ApmObject				*m_pcsApmObject;
	AgpmEventNPCDialog		*m_pcsAgpmEventNPCDialog;
	ApAdmin					m_csNPCDialogTemplate;

public:
	void InitData( ApmObject *pcsApmObject, AgpmEventNPCDialog *pcsAgpmEventNPCDialog, AgpdEventNPCDialogData *pcsEventData );

	AgcmEventNPCDialogSelectTemplateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmEventNPCDialogSelectTemplateDlg)
	enum { IDD = IDD_NPCDialogTemplateSelectDlg };
	CListBox	m_cDialogList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmEventNPCDialogSelectTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmEventNPCDialogSelectTemplateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	afx_msg void OnDialogSelect();
	afx_msg void OnDialogView();
	afx_msg void OnDialogReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTNPCDIALOGSELECTTEMPLATEDLG_H__2FF2E304_442E_47B1_B578_A34161663A8E__INCLUDED_)
