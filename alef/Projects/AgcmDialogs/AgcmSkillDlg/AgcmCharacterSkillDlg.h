#if !defined(AFX_AGCMSETSKILLDLG_H__FE79A649_63B7_4116_A555_BC2CD4A8AEB7__INCLUDED_)
#define AFX_AGCMSETSKILLDLG_H__FE79A649_63B7_4116_A555_BC2CD4A8AEB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmCharacterSkillDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmCharacterSkillDlg dialog
/*
class AgcmCharacterSkillDlg : public CDialog
{
// Construction
public:
	AgcmCharacterSkillDlg(AgpdSkillTemplateAttachData *pcsAgpdSkillData, AgcdSkillAttachTemplateData *pcsAgcdSkillData, CHAR *szFindAnimPathName1, CHAR *szFindAnimPathName2, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmCharacterSkillDlg)
	enum { IDD = IDD_CHARACTER_SKILL };
	int		m_nSkillDataIndex;
	CString	m_strSkillDataAnimationName;
	CString	m_strSkillName;
	int		m_nEquipType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmCharacterSkillDlg)
	public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHAR							m_szFindAnimPathName1[256];
	CHAR							m_szFindAnimPathName2[256];
	AgcdSkillAttachTemplateData		*m_pcsAgcdSkillData;
	AgcdSkillAttachTemplateData		m_csAgcdTempSkillData;
	AgpdSkillTemplateAttachData		*m_pcsAgpdSkillData;
	AgpdSkillTemplateAttachData		m_csAgpdTempSkillData;


	VOID ApplyData();

	// Generated message map functions
	//{{AFX_MSG(AgcmCharacterSkillDlg)
	afx_msg void OnButtonSkillDataSetAnimation();
	afx_msg void OnButtonSkillDataSetEffect();
	virtual void OnOK();
	afx_msg void OnSelchangeSkillDataIndex();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSkillDataSetSkillName();
	afx_msg void OnButtonSkillDataSetDefSkill();
	afx_msg void OnButtonApplySkill();
	afx_msg void OnButtonStartAnimation();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnButtonSkillDataRemove();
	afx_msg void OnSelchangeSkillDataLevelIndex();
	afx_msg void OnButtonSkillDataSetAnimProperty();
	afx_msg void OnButtonStartEffect();
	afx_msg void OnSelchangeComboEquipType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
*/
#endif // !defined(AFX_AGCMSETSKILLDLG_H__FE79A649_63B7_4116_A555_BC2CD4A8AEB7__INCLUDED_)
