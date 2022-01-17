#pragma once

/////////////////////////////////////////////////////////////////////////////
// CAnimationDlg dialog
class CAnimationDlg : public CDialog
{
// Construction
public:
	CAnimationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimationDlg)
	enum { IDD = IDD_ANIMATION };
	UINT	m_unCurTime;
	int		m_nSliderAnimTime;
	BOOL	m_bAnimPlay;
	BOOL	m_bAnimStop;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationDlg)
	public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	VOID	UpdateAnimTime(INT32 lTime);
	VOID	SetAnimRange(INT32 lMin, INT32 lMax);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimationDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAnimPlay();
	afx_msg void OnCheckAnimStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};