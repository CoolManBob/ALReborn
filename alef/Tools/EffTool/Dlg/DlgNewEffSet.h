#pragma once

class CDlgNewEffSet : public CDialog
{
public:
	CDlgNewEffSet(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DLG_NEWEFFSET };
	UINT	m_dwEffSetID;
	CString	m_strTitle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
