#if !defined(AFX_AGCMAIUSESKILLEDIT_H__36BA3908_1825_42F5_9A5F_7F2AFB4FD1CC__INCLUDED_)
#define AFX_AGCMAIUSESKILLEDIT_H__36BA3908_1825_42F5_9A5F_7F2AFB4FD1CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAIUseSkillEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmAIUseSkillEdit dialog

#include "AgpmAI.h"
#include "AgpmSkill.h"

class AgcmAIUseSkillEdit : public CDialog
{
// Construction
public:
	AgcmAIUseSkillEdit(CWnd* pParent = NULL);   // standard constructor

	AgpmSkill		*m_pcsAgpmSkill;
	AgpdAITemplate	*m_pstTemplate;

// Dialog Data
	//{{AFX_DATA(AgcmAIUseSkillEdit)
	enum { IDD = IDD_DIALOG_AI_EDIT_USE_SKILL };
	CEdit	m_cRateEdit;
	CListCtrl	m_cSkillList;
	CEdit	m_cHPConEdit;
	CComboBox	m_cSkillListCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAIUseSkillEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAIUseSkillEdit)
	afx_msg void OnAiUseSkillAddButton();
	afx_msg void OnAiUseSkillOkButton();
	afx_msg void OnAiUseSkillCancelButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnAiUseSkillRemoveButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAIUSESKILLEDIT_H__36BA3908_1825_42F5_9A5F_7F2AFB4FD1CC__INCLUDED_)
