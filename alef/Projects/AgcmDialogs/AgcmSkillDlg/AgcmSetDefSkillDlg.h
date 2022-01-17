#if !defined(AFX_AGCMSETDEFSKILLDLG_H__097FF719_FED5_4428_8E80_B70DDB99ACD6__INCLUDED_)
#define AFX_AGCMSETDEFSKILLDLG_H__097FF719_FED5_4428_8E80_B70DDB99ACD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmSetDefSkillDlg.h : header file
//

#include "../resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmSetDefSkillDlg dialog

class AgcmSetDefSkillDlg : public CDialog
{
// Construction
public:
	AgcmSetDefSkillDlg(AgpdSkillTemplateAttachData	*pcsAgpdSkillData, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmSetDefSkillDlg)
	enum { IDD = IDD_DEF_SKILL };
	CString	m_strDefSkillName;
	int		m_nDefSkillIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmSetDefSkillDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	AgpdSkillTemplateAttachData		m_csAgpdTempSkillData;
	AgpdSkillTemplateAttachData		*m_pcsAgpdSkillData;

	// Generated message map functions
	//{{AFX_MSG(AgcmSetDefSkillDlg)
	afx_msg void OnSelchangeComboDefSkillIndex();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSkillDataSetDefaultSkillName();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMSETDEFSKILLDLG_H__097FF719_FED5_4428_8E80_B70DDB99ACD6__INCLUDED_)
