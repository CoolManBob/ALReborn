#pragma once

class CTreeDlg : public CDialog
{
public:
	CTreeDlg(DWORD *pdwData, CTreeCtrlEx *pcsSrcTree, CWnd* pParent = NULL);   // standard constructor

protected:
	CTreeCtrlEx*	m_pcsSrcTree;
	DWORD*			m_pdwData;

public:
	//{{AFX_DATA(CTreeDlg)
	enum { IDD = IDD_TREE };
	CTreeCtrlEx	m_csTreeCtrl;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnDblclkTreeItem(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};