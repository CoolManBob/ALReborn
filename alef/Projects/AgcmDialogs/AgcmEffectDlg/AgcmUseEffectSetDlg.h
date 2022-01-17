#if !defined(AFX_AGCMUSEEFFECTSETDLG_H__D867EE56_E515_4F42_A9E0_6DD40F2FCE07__INCLUDED_)
#define AFX_AGCMUSEEFFECTSETDLG_H__D867EE56_E515_4F42_A9E0_6DD40F2FCE07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmUseEffectSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmUseEffectSetDlg dialog

#include "AgcdEffectData.h"
#include "AgcdEventEffect.h"

class AgcmUseEffectSetDlg : public CDialog
{
// Construction
public:
	AgcmUseEffectSetDlg(AgcdUseEffectSet *pstInitData, INT16 nMaxEffectNum, CWnd* pParent = NULL);   // standard constructor

	VOID InitializeData();

// Dialog Data
	//{{AFX_DATA(AgcmUseEffectSetDlg)
	enum { IDD = IDD_USE_EFFECT_SET };
	CComboBox	m_csIndex;
	UINT	m_unEID;
	UINT	m_unGap;
	float	m_fOffsetX;
	float	m_fOffsetY;
	float	m_fOffsetZ;
	float	m_fScale;
	CString	m_strSoundName;
	int		m_nParentNodeID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmUseEffectSetDlg)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	AgcdUseEffectSet	*m_pstAgcdEventEffect;
	INT16				m_nMaxEffectNum;
	INT16				m_nPreEIDIndex;

//protected:
//	AgcdUseEffectSet m_stUseEffectSet;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmUseEffectSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEidFind();
	afx_msg void OnUseEffectSetApply();
	afx_msg void OnClose();
	afx_msg void OnSelchangeComboUesIndex();
	afx_msg void OnButtonEidReset();
	afx_msg void OnButtonSetConditionFlag();
	afx_msg void OnButtonSetSound();
	afx_msg void OnButtonSetStatusFlags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMUSEEFFECTSETDLG_H__D867EE56_E515_4F42_A9E0_6DD40F2FCE07__INCLUDED_)
