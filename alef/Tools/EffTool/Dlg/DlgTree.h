#pragma once

class CDlgTree : public CDialog
{
// Construction
public:
	CDlgTree(CWnd* pParent = NULL);

public:
	// Dialog Data
	//{{AFX_DATA(CDlgTree)
	enum { IDD = IDD_DLG_TREE };
	CTreeCtrl		m_ctrlTreeData;
	CEdit			m_ctrlEditInfo;
	//}}AFX_DATA
	
protected:
	BOOL			m_bInit;
	const CPoint	ptEdit;
	const CPoint	ptTree;	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTree)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK()		{		}
	virtual void OnCancel()	{		}
	virtual TV_ITEM tGetTV_ITEM(HTREEITEM hItem);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgTree)
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
