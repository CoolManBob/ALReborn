#pragma once

class CResourceTree : public CTreeCtrlEx
{
// Construction
public:
	CResourceTree(CWnd *pcsParent, RECT &stInitRect);

	//@{ 2006/05/04 burumal
	void		SetTreeDNF_Flag(HTREEITEM hSelected, INT32 nNationalCode, INT32 nDNF);
	//@}

protected:
	BOOL		TrackDefaultPopupMenu();
	BOOL		TrackCharacterPopupMenu(HTREEITEM hSelectedItem);
	BOOL		TrackItemPopupMenu(HTREEITEM hSelectedItem);
	BOOL		TrackObjectPopupMenu(HTREEITEM hSelectedItem);
	BOOL		TrackSkillPopupMenu(HTREEITEM hSelectedItem);

	BOOL		_TrackPopupMenu(CMenu *pMenu);
	BOOL		_AppendMenu(CMenu *pMenu, UINT32 ulIDCommand);
	BOOL		_AddSeparator(CMenu *pMenu);

	HTREEITEM	OpenTree(DWORD dwData);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourceTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResourceTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResourceTree)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCommandResourceTreeContextMenuEditLabel();
	afx_msg void OnCommandResourceTreeContextMenuDeleteLabel();
	afx_msg void OnCommandResourceTreeContextMenuInsertLabel();
	afx_msg void OnCommandResourceTreeContextMenuCopyItem();
	afx_msg void OnCommandResourceTreeContextMenuDeleteItem();
	afx_msg void OnCommandResourceTreeContextMenuMoveItem();
	afx_msg void OnCommandResourceTreeContextMenuUpdateResource();
	afx_msg void OnCommandResourceTreeContextMenuTrunkOnly();
	afx_msg void OnCommandResourceTreeContextMenuSave();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
