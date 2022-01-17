#pragma once

class CStatusListDlg : public CDialog
{
public:
	CStatusListDlg(UINT32 *plStatus, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CStatusListDlg)
	enum { IDD = IDD_EFFECT_STATUS_LIST };
	BOOL	m_bDirection;
	BOOL	m_bHitEffect;
	BOOL	m_bOnlyTarget;
	UINT	m_unStatus;
	BOOL	m_bLoop;
	BOOL	m_bAnimLoop;
	BOOL	m_bFirstTarget;
	BOOL	m_bCalcDist;
	//}}AFX_DATA

protected:
	UINT32	*m_plStatus;

public:
	VOID	UpdateStatusDlg(UINT32 *plStatus);
	VOID	EnableApplyButton(BOOL bEnable = TRUE);

protected:
	VOID	InitializeStatusButton();
	VOID	UpdateStatusButton(BOOL bUpdateData = TRUE);
	VOID	UpdateStatus();

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CStatusListDlg)
	afx_msg void OnButtonInput();
	afx_msg void OnButtonOutput();
	afx_msg void OnDestroy();
	afx_msg void OnButtonApply();

	DECLARE_MESSAGE_MAP()
};