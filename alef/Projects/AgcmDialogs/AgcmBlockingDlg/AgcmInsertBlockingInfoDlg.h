#pragma once

class AgcmInsertBlockingInfoDlg : public CDialog
{
public:
	AgcmInsertBlockingInfoDlg(UINT16 unNewIndex, CWnd* pParent = NULL);

	//{{AFX_DATA(AgcmInsertBlockingInfoDlg)
	enum { IDD = IDD_INSERT_BLOCKING_INFO };
	CComboBox	m_csBlockingType;
	UINT	m_unNewIndex;
	//}}AFX_DATA

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};