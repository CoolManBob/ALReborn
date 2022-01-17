#pragma once

class CConditionListDlg : public CDialog
{
public:
	CConditionListDlg(UINT32 *plCondition, UINT32 *plSSCondition, CWnd* pParent = NULL);   // standard constructor

	static CConditionListDlg	*GetInstance();

// Dialog Data
	//{{AFX_DATA(CConditionListDlg)
	enum { IDD = IDD_EFFECT_CONDITION_LIST };
	UINT	m_unCondition;
	BOOL	m_bCharAnimAttack;
	BOOL	m_bCharAnimDead;
	BOOL	m_bCharAnimRun;
	BOOL	m_bCharAnimStruck;
	BOOL	m_bCharAnimWait;
	BOOL	m_bCharAnimWalk;
	BOOL	m_bTargetStruck;
	BOOL	m_bLevelUP;
	BOOL	m_bCommonInit;
	BOOL	m_bItemEquip;
	BOOL	m_bObjectWork;
	BOOL	m_bSkillTarget;
	BOOL	m_bItemSS;
	BOOL	m_bDestroy;
	BOOL	m_bCharResurrection;
	BOOL	m_bCharCreate;
	BOOL	m_bItemUse;
	BOOL	m_bTransform;
	BOOL	m_bCheckTime;
	//}}AFX_DATA

protected:
	UINT32	*m_plCondition;
	UINT32	*m_plSSCondition;

public:
	BOOL m_bSpiritStone;
	BOOL m_bCharAnimCast;

public:
	VOID	CloseSSConditionDlg();
	VOID	ApplySSCondition();

	VOID	UpdateConditionDlg(UINT32 *plCondition, UINT32 *plSSCondition);
	VOID	EnableApplyButton(BOOL bEnable = TRUE);

protected:
	VOID	InitializeConditionButton();
	VOID	UpdateConditionButton(BOOL bUpdateData = TRUE);
	VOID	UpdateCondition();

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

public:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CConditionListDlg)
	afx_msg void OnButtonInput();
	afx_msg void OnButtonOutput();
	afx_msg void OnDestroy();
	afx_msg void OnButtonOpenSsCondition();
	afx_msg void OnButtonApply();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
