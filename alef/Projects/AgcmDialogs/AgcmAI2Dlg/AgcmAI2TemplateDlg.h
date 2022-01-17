#if !defined(AFX_AGCMAI2TEMPLATEDLG_H__6AE7B28F_0015_46A8_B80D_43B3BBF4BA8A__INCLUDED_)
#define AFX_AGCMAI2TEMPLATEDLG_H__6AE7B28F_0015_46A8_B80D_43B3BBF4BA8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAI2TemplateDlg.h : header file
//

#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmAI2.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAI2TemplateDlg dialog

class AgcmAI2TemplateDlg : public CDialog
{
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmAI2					*m_pcsAgpmAI2;

// Construction
public:
	AgcmAI2TemplateDlg(CWnd* pParent = NULL);   // standard constructor

	void InitData( AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2 );
	void SetTemplateCombo();

// Dialog Data
	//{{AFX_DATA(AgcmAI2TemplateDlg)
	enum { IDD = IDD_DIALOG_AI2_TEMPLATE };
	CEdit	m_AI2TemplateNameEdit;
	CComboBox	m_cAI2TemplateCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAI2TemplateDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAI2TemplateDlg)
	afx_msg void OnAI2TemplateSelectButton();
	afx_msg void OnAI2TemplateRemoveButton();
	afx_msg void OnAI2ExitButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnAI2TemplateAddButton();
	afx_msg void OnAI2TemplateUpdateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAI2TEMPLATEDLG_H__6AE7B28F_0015_46A8_B80D_43B3BBF4BA8A__INCLUDED_)
