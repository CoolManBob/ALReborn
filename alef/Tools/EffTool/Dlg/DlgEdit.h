#pragma once

class CDlgEdit : public CDialog
{
public:
	CDlgEdit(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DLG_EDIT };
	CEdit	m_ctrlWnd;
	CString	m_strBuff;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	void	bAddString(LPCTSTR szIn);
	void	bClear(void);
	void	bSetMargins(UINT nLeft, UINT nRight);

private:
	BOOL	m_bInit;
};