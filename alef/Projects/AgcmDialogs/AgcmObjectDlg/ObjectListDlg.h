#pragma once

#define OBJECGT_LIST_DLG_TREE_ITEM_NOT_SELECT	1
#define OBJECGT_LIST_DLG_TREE_ITEM_SELECT		2

typedef BOOL (*ObjectListDlgCallback)(PVOID pvClass, PVOID pvCustClass);

class CObjectListDlg : public CDialog
{
public:
	CObjectListDlg(ObjectListDlgCallback pfInitCallback, ObjectListDlgCallback pfEndCallback, PVOID pvCustClass, CHAR *pszDest = NULL, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CObjectListDlg)
	enum { IDD = IDD_OBJECT_LIST };
	CTreeCtrl	m_csList;
	//}}AFX_DATA

protected:
	PVOID					m_pvCustClass;
	ObjectListDlgCallback	m_pfInitCallback;
	ObjectListDlgCallback	m_pfEndCallback;
	CHAR*					m_pszDest;
	BOOL					m_bListUpdated;

public:
	HTREEITEM	_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, DWORD dwData = OBJECGT_LIST_DLG_TREE_ITEM_NOT_SELECT, BOOL bEnsureVisible = FALSE);
	HTREEITEM	_FindItem(LPCSTR lpszItem);

	VOID		_EnableButton(BOOL bEnable = TRUE);

	CTreeCtrl*	_GetTreeCtrl()	{	return &m_csList;	}

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CObjectListDlg)
	afx_msg void OnButtonOlAdd();
	afx_msg void OnButtonOlRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
