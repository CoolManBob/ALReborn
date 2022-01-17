#pragma once

class AgcmEffectStatusDlg : public CDialog
{
public:
	AgcmEffectStatusDlg(AgcdUseEffectSetData *pcsData, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmEffectStatusDlg)
	enum { IDD = IDD_EFFECT_STATUS };
	BOOL	m_bDirection;
	BOOL	m_bHitEffect;
	BOOL	m_bOnlyTarget;
	//}}AFX_DATA

	AgcdUseEffectSetData	*m_pcsData;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
