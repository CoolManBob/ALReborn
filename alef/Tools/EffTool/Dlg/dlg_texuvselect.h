#pragma once

class CDlg_TexUVSelect : public CDialog
{
public:
	CDlg_TexUVSelect(RwTexture* pTex, CWnd* pParent = NULL);   // standard constructor

	//AFX_DATA
	enum { IDD = IDD_DLG_TEXUV };
	CSpinButtonCtrl	m_ctrlSpinRow;
	CSpinButtonCtrl	m_ctrlSpinCol;
	UINT	m_nCol;
	UINT	m_nRow;
	UINT	m_nSelCol;
	UINT	m_nSelRow;

	UINT	m_nMaxCol;
	UINT	m_nMaxRow;

	BOOL	m_bMultiInput;
	UINT	m_nTimeDistance;
	UINT	m_nBeginIndex;
	UINT	m_nEndIndex;

protected:
	//}}AFX_DATA
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	// Message
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeEditCol();
	afx_msg void OnChangeEditRow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCheckMulti();
	afx_msg void OnEnChangeEditBeginIndex();
	afx_msg void OnEnChangeEditEndIndex();
	DECLARE_MESSAGE_MAP()

private:
	void	vChkValidIndex();

public:
	FLOAT		m_fLeft;
	FLOAT		m_fTop;
	FLOAT		m_fRight;
	FLOAT		m_fBottom;

private:
	RwTexture*	m_pTex;
	BOOL		m_bInit;
};
