#pragma once

typedef struct tagCUSTOM_TREEITEM
{
	HTREEITEM	hHandle;
	DWORD		dwColor;

	tagCUSTOM_TREEITEM()
	{
		hHandle = NULL;
		dwColor = RGB(0, 0, 0);
	}
} CUSTOM_TREEITEM;

typedef map<string, CUSTOM_TREEITEM*>	mapTREE_ITEM;
typedef mapTREE_ITEM::iterator			mapTREE_ITEMIter;
//@}

class CTreeCtrlEx : public CTreeCtrl
{	
// Construction
public:
	CTreeCtrlEx();
	VOID		_Create(CWnd *pcsParent, RECT &stInitRect, DWORD dwStyle = 0);

protected:
	VOID		SetDrag(HTREEITEM hSelected);

public:
	HTREEITEM	_FindItem(LPCSTR lpszItem);
	HTREEITEM	_FindItemEx(LPCSTR lpszName);	
	void		_DeleteItemEx(LPCSTR lpszName);
	void		_DeleteItemEx(HTREEITEM hDel);
	void		_DeleteAllItems();
	
	HTREEITEM	_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, DWORD dwData = 0, BOOL bEnsureVisible = FALSE, int nImage = -1, int nSelectedImage = -1);
	HTREEITEM	_MoveItem(HTREEITEM hMoveItem, HTREEITEM hNewParentItem, BOOL bDeleteMovedItem = TRUE);
	BOOL		_UpdateItem( HTREEITEM hItem, int nImage = -1, int nSelectedImage = -1);
	VOID		_ShowWindow();
	VOID		_HideWindow();

	BOOL		_Copy(CTreeCtrlEx *pSrc);

	VOID		InitializeMember();

	//@{ 2006/09/21 burumal
	void		SetItemBold(HTREEITEM hItem, BOOL bBold);
	BOOL		IsItemBold(HTREEITEM hItem);

	void		SetItemColor(HTREEITEM hItem, DWORD dwColor);
	BOOL		GetItemColor(HTREEITEM hItem, DWORD* pdwColor);
	//@}
	
protected:
	BOOL		m_bDrag;

	//@{ 2006/05/04 burumal
	CImageList		m_StateImageLIst;

	mapTREE_ITEM	m_mapTreeItem;
	//@}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlEx)
		// NOTE - the ClassWizard will add and remove member functions here.
		//afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CUSTOM_TREEITEM* FindCustomTreeItemInfo(HTREEITEM hItem);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};