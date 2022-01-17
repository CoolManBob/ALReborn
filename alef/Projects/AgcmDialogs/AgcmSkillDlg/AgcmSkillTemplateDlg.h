#pragma once

class AgcmSkillTemplateDlg : public CDialog
{
public:
	AgcmSkillTemplateDlg(AgpdSkillTemplate *pcsAgpdSkillTemplate, AgcdSkillTemplate *pcsAgcdSkillTemplate, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(AgcmSkillTemplateDlg)
	enum { IDD = IDD_SKILL_TEMPLATE };
	CString	m_strSmallSkillTexture;
	CString	m_strSkillTexture;
	CString	m_strName;
	CString	m_strUnableSkillTexture;
	//}}AFX_DATA

protected:
	AgpdSkillTemplate	*m_pcsAgpdSkillTemplate;
	AgcdSkillTemplate	*m_pcsAgcdSkillTemplate;


protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	//{{AFX_MSG(AgcmSkillTemplateDlg)
	afx_msg void OnButtonSetSkillTexture();
	afx_msg void OnButtonSetSmallSkillTexture();
	
	afx_msg void OnButtonDeleteSkillTexture();
	afx_msg void OnButtonDeleteSmallSkillTexture();
	afx_msg void OnButtonDeleteUnableSkillTexture();
	afx_msg void OnButtonSetUnableSkillTexture();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
