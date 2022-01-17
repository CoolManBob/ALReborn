#pragma once
#include "afxwin.h"


// NatureEffectDlg 대화 상자입니다.

class NatureEffectDlg : public CDialog
{
	DECLARE_DYNAMIC(NatureEffectDlg)

public:
	NatureEffectDlg( INT32 nEffectID , CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~NatureEffectDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NATURE_EFFECT_SET };

	INT32	m_nEffectID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL	m_bDataChange;

	DWORD m_ID;
	CString m_strTextureFile;
	float m_fSizeMin;
	float m_fSizeMax;
	float m_fSpeedMin;
	float m_fSpeedMax;
	float m_fVibMin;
	float m_fVibMax;
	float m_fDensityMin;
	float m_fDensityMax;
	UINT m_iRedMin;
	UINT m_iRedMax;
	UINT m_iGreenMin;
	UINT m_iGreenMax;
	UINT m_iBlueMin;
	UINT m_iBlueMax;
	UINT m_iAlphaMin;
	UINT m_iAlphaMax;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedNeffectTextureButton();
	afx_msg void OnEnChangeNeffectSizeMin();
	afx_msg void OnEnChangeNeffectSizeMax();
	afx_msg void OnEnChangeNeffectSpeedMin();
	afx_msg void OnEnChangeNeffectSpeedMax();
	afx_msg void OnEnChangeNeffectVibMin();
	afx_msg void OnEnChangeNeffectVibMax();
	afx_msg void OnEnChangeNeffectDensityMin();
	afx_msg void OnEnChangeNeffectDensityMax();
	afx_msg void OnEnChangeNeffectRedMin();
	afx_msg void OnEnChangeNeffectRedMax();
	afx_msg void OnEnChangeNeffectGreenMin();
	afx_msg void OnEnChangeNeffectGreenMax();
	afx_msg void OnEnChangeNeffectBlueMin();
	afx_msg void OnEnChangeNeffectBlueMax();
	afx_msg void OnEnChangeNeffectAlphaMin();
	afx_msg void OnEnChangeNeffectAlphaMax();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNeffectOk();
	CComboBox m_ctrlComboBox;
	int m_iSelType;
	afx_msg void OnBnClickedNeffectCancel();
	afx_msg void OnCbnSelchangeNeffectCombo();
	afx_msg void OnApply();
	CComboBox m_ctlSkySetCombo;

	INT32	m_nSkySet;
};
