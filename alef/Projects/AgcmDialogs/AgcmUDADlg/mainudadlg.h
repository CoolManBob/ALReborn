#pragma once

class CMainUDADlg : public CDialog
{
public:
	CMainUDADlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CMainUDADlg)
	enum { IDD = IDD_MAIN_UDA };
	CComboBox m_csAtomicIndex;
	//}}AFX_DATA

	VOID	ApplyData();
	VOID	EnableControl(BOOL bEnable = TRUE);
	VOID	SetRenderTypePointer(INT32 *plRenderType, RpAtomic *pstAtomic);

protected:
	INT32*		m_plRenderType;
	RpAtomic*	m_pstAtomic;
	RpClump *	m_pstClump;

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CMainUDADlg)
	afx_msg void OnDestroy();
	afx_msg void OnButtonUdaApply();
	afx_msg void OnCbnSelchangeComboAtomicIndex();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
