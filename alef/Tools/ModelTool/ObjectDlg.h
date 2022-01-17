#pragma once

#define OBJECGT_DLG_TREE_ITEM_NOT_SELECT	1
#define OBJECGT_DLG_TREE_ITEM_SELECT		2

typedef BOOL (*ObjectDlgCallback)(PVOID pvClass, PVOID pvCustClass);

/////////////////////////////////////////////////////////////////////////////
// CObjectDlg dialog

class CObjectDlg : public CDialog
{
// Construction
public:
	CObjectDlg(ObjectDlgCallback pfCallback, PVOID pvCustClass, CHAR *pszDest, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectDlg)
	enum { IDD = IDD_OBJECT };
	CTreeCtrl	m_csObjectList;
	//}}AFX_DATA

protected:
	PVOID				m_pvCustClass;
	ObjectDlgCallback	m_pfCallback;
	CHAR				*m_pszDest;

public:
	HTREEITEM	_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, DWORD dwData = OBJECGT_DLG_TREE_ITEM_NOT_SELECT);
	HTREEITEM	_FindItem(LPCSTR lpszItem);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};