#pragma once
#include "afxwin.h"

// CCharAnimAttachedSoundDataDlg dialog

class CCharAnimAttachedSoundDataDlg : public CDialog
{
	DECLARE_DYNCREATE(CCharAnimAttachedSoundDataDlg)

public:
	CCharAnimAttachedSoundDataDlg(AEE_CharAnimAttachedData *pcsAEE, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCharAnimAttachedSoundDataDlg();

// Dialog Data
	enum { IDD = IDD_CHAR_ANIM_ATTACHED_SOUND_DATA };
	CListBox	m_csListSound;
	CComboBox	m_csComboCondition;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnClose();
	afx_msg void OnLbnSelchangeListSound();
	afx_msg void OnCbnSelchangeComboCondition();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

public:
	AEE_CharAnimAttachedData *m_pcsAEE;
};
