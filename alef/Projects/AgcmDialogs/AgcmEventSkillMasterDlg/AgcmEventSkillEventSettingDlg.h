#if !defined(AFX_AGCMEVENTSKILLEVENTSETTINGDLG_H__FCE4DC0F_A2AD_41C5_BF38_2C929B55CCD5__INCLUDED_)
#define AFX_AGCMEVENTSKILLEVENTSETTINGDLG_H__FCE4DC0F_A2AD_41C5_BF38_2C929B55CCD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmEventSkillEventSettingDlg.h : header file
//

#include "ApmEventManager.h"
#include "AgpmEventSkillMaster.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSkillEventSettingDlg dialog

class AgcmEventSkillEventSettingDlg : public CDialog
{
// Construction
public:
	AgcmEventSkillEventSettingDlg(CWnd* pParent = NULL);   // standard constructor
	AgcmEventSkillEventSettingDlg(AgpmEventSkillMaster *pcsAgpmEventSkillMaster, ApdEvent *pcsEvent, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(AgcmEventSkillEventSettingDlg)
	enum { IDD = IDD_SKILL_MASTER_DIALOG };
	CComboBox	m_csRaceType;
	CComboBox	m_csClassType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmEventSkillEventSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmEventSkillEventSettingDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;
	ApdEvent				*m_pcsEvent;

public:
	BOOL	InitData(AgpmEventSkillMaster *pcsAgpmEventSkillMaster, ApdEvent *pcsEvent);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTSKILLEVENTSETTINGDLG_H__FCE4DC0F_A2AD_41C5_BF38_2C929B55CCD5__INCLUDED_)
