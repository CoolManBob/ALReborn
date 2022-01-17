#pragma once

#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmAI2.h"

typedef enum
{
	AGCMAI2DIALOG_EDITDLG_ADDITEM = 0,
	AGCMAI2DIALOG_EDITDLG_UPDATEITEM,
	AGCMAI2DIALOG_EDITDLG_ADDSKILL,
	AGCMAI2DIALOG_EDITDLG_UPDATESKILL,
} eAgcmAI2DialogEditStatus;


class AgcmAI2EditTableDlg : public CDialog
{
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmAI2					*m_pcsAgpmAI2;
	AgpdAI2Template			*m_pcsAgpdAI2Template;

	eAgcmAI2DialogEditStatus m_eEditStatus;
	void					*m_pvEditTable;

// Construction
public:
	AgcmAI2EditTableDlg(CWnd* pParent = NULL);   // standard constructor

	void InitData( AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2, AgpdAI2Template *pcsAI2Template, eAgcmAI2DialogEditStatus eStatus, void *pvData = NULL );
	void ClearControl();
	void FillControlByItem( AgpdAI2UseItem *pcsItem );
	void FillControlBySkill( AgpdAI2UseSkill *pcsSkill );
	void FillComboItem();
	void FillComboSkill();

	//}}AFX_DATA
	enum { IDD = IDD_Edit_ConditonTable };
	CEdit		m_cProbableEdit;
	CComboBox	m_cConditionCheck;
	CComboBox	m_cOperatorCombo;
	CEdit		m_cParameterEdit;
	CComboBox	m_cPercentCombo;
	CEdit		m_cTimerEdit;
	CComboBox	m_cTargetParameterCombo;
	CComboBox	m_cComboBox;

protected:
	// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// message map
	afx_msg void OnEditTableOKButton();
	afx_msg void OnEditTableCancelButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
