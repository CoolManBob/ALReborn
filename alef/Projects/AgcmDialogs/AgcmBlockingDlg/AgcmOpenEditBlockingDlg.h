#pragma once

class AgcmOpenEditBlockingDlg : public CDialog
{
public:
	AgcmOpenEditBlockingDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(AgcmOpenEditBlockingDlg)
	enum { IDD = IDD_OPEN_EDIT_BLOCKING };
	CComboBox	m_csHasBlockingIndex;
	CButton	m_csBtNew;
	CButton	m_csBtOK;
	//}}AFX_DATA



protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//Message Map
	afx_msg void OnNewBlocking();
	
	DECLARE_MESSAGE_MAP()

protected:
	VOID InitializeControl();
	VOID SetBlockingIndex(INT16 nIndex);
};