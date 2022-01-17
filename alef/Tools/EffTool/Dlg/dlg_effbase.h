#pragma once

class CDlg_EffBase : public CDialog
{
public:
	CDlg_EffBase(CWnd* pParent = NULL);   // standard constructor

	//AFX_DATA
	enum { IDD = IDD_DLG_EFFBASE };
	CComboBox	m_ctrlCombo_LoopOpt;
	INT			m_nLoopOpt;
	BOOL		m_bBillboard;
	BOOL		m_bBillboardY;
	UINT		m_ulDelay;
	UINT		m_ulLife;
	CString		m_szTitle;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK()			{	CDialog::OnOK();		}
	virtual void OnCancel()		{	CDialog::OnCancel();	}

	// Message
	afx_msg void OnCheckBillboard();
	afx_msg void OnCheckBillboardy();
	afx_msg void OnSelchangeComboLoopopt();
	DECLARE_MESSAGE_MAP()
};