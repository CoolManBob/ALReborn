#pragma once

class CDlg_EffRenderBase : public CDialog
{
public:
	CDlg_EffRenderBase(CWnd* pParent = NULL);   // standard constructor

	//AFX_DATA
	enum { IDD = IDD_DLG_RENDERBASE };
	CComboBox	m_ctrlCombo_Blend;
	float	m_fAngleZ;
	float	m_fAngleX;
	float	m_fAngleY;
	CString	m_szMask;
	float	m_fPoxX;
	float	m_fPoxY;
	float	m_fPoxZ;
	CString	m_szTex;
	INT		m_nBlendType;
	//}}AFX_DATA

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK()			{	CDialog::OnOK();		}
	virtual void OnCancel()		{	CDialog::OnCancel();	}

	afx_msg void OnButtonTex();
	afx_msg void OnButtonMask();
	afx_msg void OnSelchangeComboBlend();
	DECLARE_MESSAGE_MAP()
};