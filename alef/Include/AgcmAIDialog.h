#if !defined(AFX_AGCMAIDIALOG_H__5325E8C3_B757_4D26_8C23_55AAD2103FFB__INCLUDED_)
#define AFX_AGCMAIDIALOG_H__5325E8C3_B757_4D26_8C23_55AAD2103FFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAIDialog.h : header file
//

#include "AgpmAI.h"

#include "AgcmAICharFactorDialog.h"
#include "AgcmAICharStatusDialog.h"
#include "AgcmAICharItemDialog.h"

#include "AgcmAIUseItemEdit.h"
#include "AgcmAIUseSkillEdit.h"
#include "AgcmAIScreamEditDlg.h"

class AgcmAIDlg;

/////////////////////////////////////////////////////////////////////////////
// AgcmAIDialog dialog

class AgcmAIDialog : public CDialog
{
private:
	AgpmAI *			m_pcsAgpmAI;
	AgcmAIDlg *			m_pcsAgcmAIDlg;

	AgpdAITemplate *	m_pstTemplate;
	AgpdAIFactor *		m_pstAIFactor;
	AgpdAIAction *		m_pstAction;

	AgcmAICharFactorDialog	m_dlgCharFactor;
	AgcmAICharStatusDialog	m_dlgCharStatus;
	AgcmAICharItemDialog	m_dlgCharItem;

	ApModuleDefaultCallBack	m_pfnCBClose;

// Construction
public:
	VOID SetModule(AgpmAI *pcsAgpmAI, AgcmAIDlg *pcsAgcmDlg, AgpmCharacter *pcsAgpmCharacter, AgpmItem *pcsAgpmItem );
	BOOL Init();
	BOOL InitData(AgpdAITemplate *pstTemplate);
	BOOL InitDataFactor(AgpdAIFactor *pstAIFactor);
	BOOL InitDataAction(AgpdAIAction *pstAction);
	AgcmAIDialog(CWnd* pParent = NULL);   // standard constructor

	AgcmAIUseItemEdit   m_csAgcmAIUseItemEditDlg;
	AgcmAIUseSkillEdit   m_csAgcmAIUseSkillEditDlg;
	AgcmAIScreamEditDlg   m_csAgcmAIScreamEditDlg;

// Dialog Data
	//{{AFX_DATA(AgcmAIDialog)
	enum { IDD = IDD_DIALOG_AI };
	CComboBox	m_csActionType;
	CComboBox	m_csTargetBase;
	CListCtrl	m_csAIActions;
	CComboBox	m_csAIFactorType;
	CListCtrl	m_csAIFactors;
	long	m_lAITID;
	CString	m_szName;
	float	m_fVisibility;
	long	m_lInterval;
	long	m_lAIFactorID;
	CString	m_szAIFactorCondition;
	long	m_lTargetID;
	float	m_fTargetPosX;
	float	m_fTargetPosY;
	float	m_fTargetPosZ;
	float	m_fTargetRadius;
	BOOL	m_bTargetAggressive;
	BOOL	m_bTargetFriendly;
	BOOL	m_bTargetAgro;
	BOOL	m_bTargetDamage;
	BOOL	m_bTargetBuffer;
	BOOL	m_bTargetStrong;
	BOOL	m_bTargetWeak;
	BOOL	m_bTargetBoss;
	BOOL	m_bTargetFollower;
	BOOL	m_bTargetSelf;
	BOOL	m_bTargetOther;
	long	m_lTargetAggressive;
	long	m_lTargetFriendly;
	long	m_lTargetAgro;
	long	m_lTargetDamage;
	long	m_lTargetBuffer;
	long	m_lTargetNear;
	long	m_lTargetFar;
	long	m_lTargetStrong;
	long	m_lTargetWeak;
	long	m_lTargetBoss;
	long	m_lTargetFollower;
	long	m_lTargetSelf;
	long	m_lTargetOther;
	long	m_lActionRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAIDialog)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAIDialog)
	afx_msg void OnAIEditCondition();
	afx_msg void OnAIFactorAdd();
	afx_msg void OnAIFactorDelete();
	afx_msg void OnAIActionAddUpdate();
	afx_msg void OnAIActionDelete();
	afx_msg void OnItemchangedAIFactors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedAIActions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeAIVisibility();
	afx_msg void OnChangeAIInterval();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAiUseitem();
	afx_msg void OnAiScream();
	afx_msg void OnAiUseskill();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAIDIALOG_H__5325E8C3_B757_4D26_8C23_55AAD2103FFB__INCLUDED_)
