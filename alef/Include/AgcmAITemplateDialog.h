#if !defined(AFX_AGCMAITEMPLATEDIALOG_H__B728B79E_342E_48CA_BF3A_5516DB9909D2__INCLUDED_)
#define AFX_AGCMAITEMPLATEDIALOG_H__B728B79E_342E_48CA_BF3A_5516DB9909D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAITemplateDialog.h : header file
//

#include "AgpmAI.h"
#include "AgcmAIUseItemEdit.h"

class AgcmAIDlg;

/////////////////////////////////////////////////////////////////////////////
// AgcmAITemplateDialog dialog

class AgcmAITemplateDialog : public CDialog
{
private:
	AgpmAI*				m_pcsAgpmAI;
	AgcmAIDlg*			m_pcsAgcmAIDlg;

	AgpdAITemplate*	m_pstTemplate;

	BOOL				m_bSelect;

// Construction
public:
	VOID SetModule(AgpmAI *pcsAgpmAI, AgcmAIDlg *pcsAgcmAIDlg);
	BOOL InitData(BOOL bSelect);
	BOOL Init();
	//BOOL OpenTemplate(AgpdAITemplate *pstTemplate);
	AgcmAITemplateDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAITemplateDialog)
	enum { IDD = IDD_DIALOG_AI_TEMPLATES };
	CListCtrl	m_csAITemplates;
	long	m_lAITID;
	CString	m_szAITName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAITemplateDialog)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAITemplateDialog)
	afx_msg void OnAITemplateAdd();
	afx_msg void OnAITemplateEdit();
	afx_msg void OnAITemplateDelete();
	afx_msg void OnOK();
	afx_msg void OnAITemplateSelect();
	afx_msg void OnItemchangedAITemplates(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAITEMPLATEDIALOG_H__B728B79E_342E_48CA_BF3A_5516DB9909D2__INCLUDED_)
