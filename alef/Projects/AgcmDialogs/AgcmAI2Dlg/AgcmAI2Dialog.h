#pragma once

#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmAI2.h"

class AgcmAI2Dialog : public CDialog
{
private:
	AgpmItem			*m_pcsAgpmItem;
	AgpmSkill			*m_pcsAgpmSkill;
	AgpmAI2				*m_pcsAgpmAI2;
	AgpdAI2Template		*m_pcsAgpdAI2Template;

public:
	void ActiveMobAI();
	void DeactiveMobAI();

	void InitData( AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2, AgpdAI2Template *pcsTemplate );
	void ClearControl();
	void GetTemplateDataFromContrl( AgpdAI2Template *pcsTemplate );
	void FillControlByTemplate( AgpdAI2Template *pcsTemplate );

public:
	AgcmAI2Dialog(CWnd* pParent = NULL);   // standard constructor

	//}}AFX_DATA
	enum { IDD = IDD_DIALOG_AI2 };
	CButton		m_cFixedNPCCheckButton;
	CButton		m_cNPCCheckButton;
	CEdit		m_cAggressiveEdit;
	CListBox	m_cUseSkillList;
	CListBox	m_cUseItemList;
	CEdit		m_cAdjustWarriorEdit;
	CEdit		m_cAdjustTryDebuffEdit;
	CEdit		m_cAdjustTryAttackEdit;
	CEdit		m_cAdjustMonkEdit;
	CEdit		m_cAdjustMageEdit;
	CEdit		m_cAdjustHealEdit;
	CEdit		m_cAdjustDamageEdit;
	CEdit		m_cAdjustDebuffEdit;
	CEdit		m_cAdjustBuffEdit;
	CEdit		m_cAdjustArcherEdit;

protected:
	//Override
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//MessageMap
	afx_msg void OnAI2AddUseItemButton();
	afx_msg void OnAI2UpdateUseItemButton();
	afx_msg void OnAI2DeleteUseItemButton();
	afx_msg void OnAI2AddUseSkillButton();
	afx_msg void OnAI2UpdateUseSkillButton();
	afx_msg void OnAI2DeleteUseSkillButton();
	afx_msg void OnAI2AdjustOKButton();
	afx_msg void OnAI2AdjustCancelButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckNPC();

	DECLARE_MESSAGE_MAP()
};