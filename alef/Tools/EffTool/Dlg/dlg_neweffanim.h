#pragma once

class CDlg_NewEffAnim : public CDialog
{
public:
	CDlg_NewEffAnim(AgcdEffBase* pBase, CWnd* pParent = NULL);   // standard constructor

	//AFX_DATA
	enum { IDD = IDD_DLG_NEWEFFANIM };
	int		m_nSelBtn;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel()		{	CDialog::OnCancel();	}

	// Message
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	const char* bGetFName(void)	{	return (const char*)m_strFName.GetBuffer();	}

private:
	void	vValidateChk(void);

private:
	AgcdEffBase*	m_pEffBase;
	CString			m_strFName;
};