#pragma once

class CCharAnimAttachedDataDlg : public CDialog
{
	DECLARE_DYNCREATE(CCharAnimAttachedDataDlg)

public:
	CCharAnimAttachedDataDlg(AgcdAnimData2 *pcsAnimData, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCharAnimAttachedDataDlg();

// Dialog Data
	enum { IDD = IDD_CHAR_ANIM_ATTACHED_DATA };
	UINT	m_unCustType;
	UINT	m_unActiveRate;
	CString m_strPoint;
	CString m_strSubAnimation;
	CString m_strBlendingAnimation;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonCustomFlags();
	afx_msg void OnBnClickedButtonSetBlendingAnimation();
	afx_msg void OnBnClickedButtonSetSoundData();
	afx_msg void OnBnClickedButtonSetSubAnimation();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

public:
	BOOL	UpdateAnimData(AgcdAnimData2 **ppcsAnimData, CHAR *pszAnimation);
	BOOL	UpdateAnimData(AgcdAnimData2 **ppcsAnimData, CString strAnimation);

	ACA_AttachedData			*m_pcsACA;
	AEE_CharAnimAttachedData	*m_pcsAEE;
};
