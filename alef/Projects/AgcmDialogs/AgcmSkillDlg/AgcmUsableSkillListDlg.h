#if !defined(AFX_AGCMUSABLESKILLLISTDLG_H__B6B4E295_62CE_4E55_BCA0_FEDEE2DF46F5__INCLUDED_)
#define AFX_AGCMUSABLESKILLLISTDLG_H__B6B4E295_62CE_4E55_BCA0_FEDEE2DF46F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmUsableSkillListDlg.h : header file
//

#include "../resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmUsableSkillListDlg dialog

class AgcmUsableSkillListDlg : public CDialog
{
// Construction
public:
	AgcmUsableSkillListDlg(AgpdSkillTemplateAttachData	*pcsAgpdSkillData, CHAR *pszDest, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmUsableSkillListDlg)
	enum { IDD = IDD_USABLE_SKILL_LIST };
	CListBox	m_csUsableSkillList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmUsableSkillListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	AgpdSkillTemplateAttachData	*m_pcsAgpdSkillData;
	CHAR						*m_pszDest;

	BOOL IsLowEqual(CHAR *szCmp1, CHAR *szCmp2);
	VOID _ToUpper(CHAR *szDest, CHAR *szSrc);

	// Generated message map functions
	//{{AFX_MSG(AgcmUsableSkillListDlg)
	afx_msg void OnButtonFindUsableSkill();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListUsableSkill();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMUSABLESKILLLISTDLG_H__B6B4E295_62CE_4E55_BCA0_FEDEE2DF46F5__INCLUDED_)
