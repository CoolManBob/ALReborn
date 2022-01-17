#pragma once

class CSSConditionListDlg : public CDialog
{
public:
	CSSConditionListDlg(UINT32 *plCondition, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CSSConditionListDlg)
	enum { IDD = IDD_EFFECT_SS_CONDITION };
	UINT	m_unCondition;
	BOOL	m_bSSAir;
	BOOL	m_bSSAirTarget;
	BOOL	m_bSSEarth;
	BOOL	m_bSSEarthTarget;
	BOOL	m_bSSFire;
	BOOL	m_bSSFireTarget;
	BOOL	m_bSSIce;
	BOOL	m_bSSIceTarget;
	BOOL	m_bSSLightening;
	BOOL	m_bSSLighteningTarget;
	BOOL	m_bSSMagic;
	BOOL	m_bSSMagicTarget;
	BOOL	m_bSSPoison;
	BOOL	m_bSSPoisonTarget;
	BOOL	m_bSSWater;
	BOOL	m_bSSWaterTarget;
	//}}AFX_DATA

protected:
	UINT32 *m_plCondition;

public:
	VOID	UpdateConditionDlg(UINT32 *plCondition);
	VOID	EnableApplyButton(BOOL bEnable = TRUE);

protected:
	VOID	InitializeConditionButton();

	VOID	UpdateConditionButton(BOOL bUpdateData = TRUE);
	VOID	UpdateCondition();


public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CSSConditionListDlg)
	afx_msg void OnDestroy();
	afx_msg void OnButtonInput();
	afx_msg void OnButtonOutput();
	afx_msg void OnButtonApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
