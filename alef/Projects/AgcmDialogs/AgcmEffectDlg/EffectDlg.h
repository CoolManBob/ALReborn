#pragma once

#define D_EFFECT_DLG_ROOT_PRECISION		2
#define D_EFFECT_DLG_TREE_NAME_ROOT		"EFFECT_DIALOG"
#define D_EFFECT_DLG_TREE_ROOT_DATA		9999

class CEffectDlg : public CDialog
{
public:
	CEffectDlg(AgcdUseEffectSet *pstAgcdUseEffectSet, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CEffectDlg)
	enum { IDD = IDD_EFFECT };
	CTreeCtrl	m_csEffectTree;
	//}}AFX_DATA

protected:
	AgcdUseEffectSet*	m_pstAgcdUseEffectSet;
	INT32				m_lEffectIndex;

public:
	BOOL				InitializeDlg(AgcdUseEffectSet *pstAgcdUseEffectSet);

protected:
	VOID				InitializeEffectTree();
	HTREEITEM			InsertEffectTreeItem(CHAR *szName, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible = FALSE);

	VOID				UpdateAllDlg();

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CEffectDlg)
	afx_msg void OnDestroy();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnSelchangedTreeEffect(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonOpenConditionList();
	afx_msg void OnButtonOpenCustDataList();
	afx_msg void OnButtonOpenEffectList();
	afx_msg void OnButtonOpenStatusList();
	afx_msg void OnButtonOpenRotationDlg();

	DECLARE_MESSAGE_MAP()
};