#pragma once

#include "../resource.h"

class AgcmSkillTemplateListDlg : public CDialog
{
public:
	AgcmSkillTemplateListDlg(CHAR **ppszDest, CWnd* pParent = NULL);   // standard constructor
	AgcmSkillTemplateListDlg(INT32 *plTID, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(AgcmSkillTemplateListDlg)
	enum { IDD = IDD_SKILL_TEMPLATE_LIST };
	CListBox	m_csSkillTemplateList;
	//}}AFX_DATA

protected:
	BOOL IsLowEqual(CHAR *szCmp1, CHAR *szCmp2);
	VOID _ToUpper(CHAR *szDest, CHAR *szSrc);

	CHAR	**m_ppszDest;
	INT32	*m_plTID;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(AgcmSkillTemplateListDlg)
	afx_msg void OnDblclkListSkillTemplate();
	afx_msg void OnButtonStlFind();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};