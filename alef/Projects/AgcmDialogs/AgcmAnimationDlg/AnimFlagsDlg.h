#pragma once

#include "AgcaAnimation.h"
#include "AgcdCharacter.h"

// CAnimFlagsDlg dialog

class CAnimFlagsDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnimFlagsDlg)

public:
	CAnimFlagsDlg( AgcdAnimationFlag *pstFlags, ACA_AttachedData *pcsACA, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimFlagsDlg();

// Dialog Data
	enum { IDD = IDD_ANIM_FLAGS };
	BOOL	m_bLoop;
	BOOL	m_bBlend;
	BOOL	m_bLink;
	int		m_lPreference;
	CString	m_cstrPoint;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedOk();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
	
protected:
	AgcdAnimationFlag*	m_pstFlags;
	ACA_AttachedData*	m_pcsACA;
};
