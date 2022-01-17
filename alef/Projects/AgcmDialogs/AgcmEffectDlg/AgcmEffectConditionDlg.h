#pragma once

#include "AgcdEventEffect.h"

class AgcmEffectConditionDlg : public CDialog
{
public:
	AgcmEffectConditionDlg(AgcdUseEffectSet *pstEventEffect, AgcdUseEffectSetData *pstData, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmEffectConditionDlg)
	enum { IDD = IDD_EFFECT_CONDITION };
	BOOL	m_bCharAnimAttack;
	BOOL	m_bCharAnimDead;
	BOOL	m_bCharAnimRun;
	BOOL	m_bCharAnimStruck;
	BOOL	m_bCharAnimWait;
	BOOL	m_bCharAnimWalk;
	BOOL	m_bCharLogOn;
	BOOL	m_bCommonInit;
	BOOL	m_bItemEquip;
	BOOL	m_bObjectWork;
	BOOL	m_bSkillTarget;
	BOOL	m_bTargetStruck;
	BOOL	m_bLevelUP;
	BOOL	m_bSkillTargetStruck;
	BOOL	m_bSSAir;
	BOOL	m_bSSAirStruck;
	BOOL	m_bSSEarth;
	BOOL	m_bSSEarthStruck;
	BOOL	m_bSSFire;
	BOOL	m_bSSFireStruck;
	BOOL	m_bSSIce;
	BOOL	m_bSSIceStruck;
	BOOL	m_bSSLightening;
	BOOL	m_bSSLighteningStruck;
	BOOL	m_bSSMagic;
	BOOL	m_bSSMagicStruck;
	BOOL	m_bSSPoison;
	BOOL	m_bSSPoisonStruck;
	BOOL	m_bSSWater;
	BOOL	m_bSSWaterStruck;
	//}}AFX_DATA

protected:
	AgcdUseEffectSet		*m_pstParentData;
	AgcdUseEffectSetData	*m_pstData;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG(AgcmEffectConditionDlg)
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
