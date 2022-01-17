#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "afxwin.h"


// AlefAdminOption 대화 상자입니다.

class AlefAdminOption : public CDialog
{
	DECLARE_DYNAMIC(AlefAdminOption)

public:
	AlefAdminOption(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~AlefAdminOption();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPTION };

public:
	BOOL m_bUseAlpha, m_bUseAlpha2;
	INT32 m_lAlpha, m_lAlpha2;

	BOOL m_bSaveWhisper;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_csAlphaSlider;
	CButton m_csAlphaBtn;

	afx_msg void OnBnClickedChUseLayered();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	CButton m_csSaveWhisperCheckBox;
};
