#pragma once

#include "afxcmn.h"
#include "AgcmCharacter.h"

#define D_CAD_TEMPLATE_NAME_ADD				"Add"
#define D_CAD_TEMPLATE_NAME_REMOVE			"Remove"
#define D_CAD_TEMPLATE_NAME_FLAGS			"Flags"
#define D_CAD_TEMPLATE_NAME_DATA			"Data"
#define D_CAD_TEMPLATE_NAME_START			"Play"

class CCharacterAnimationDlg : public CDialog
{
DECLARE_DYNAMIC(CCharacterAnimationDlg)

	class CMyTreeCtrl : public CTreeCtrl
	{
	public :
		CMyTreeCtrl() {}

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	};
	
public :
	enum
	{
		E_CAD_TREE_ITEM_ANIM_TYPE	= 1,
		E_CAD_TREE_ITEM_ANIM_TYPE2,
	};

	CCharacterAnimationDlg(AgcdCharacterTemplate *m_pstAgcdCharacterTemplate, CWnd* pParent = NULL);
	virtual ~CCharacterAnimationDlg();

// Dialog Data
	enum { IDD = IDD_CHARACTER_ANIMATION };

protected :
	AgcdCharacterTemplate*	m_pstAgcdCharacterTemplate;
	CMyTreeCtrl				m_csTreeAnimation;

protected :
	HTREEITEM _InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD dwData);
	AgcdCharacterAnimation* GetCharacterAnimation(HTREEITEM hAnimType2);

	CMenu*	_CreatePopupMenu();
	BOOL	_AppendString(CMenu *pcsMenu, UINT32 ulIDCommand, CHAR *pszTextItem);
	BOOL	_AppendSeparator(CMenu *pcsMenu);
	BOOL	_TrackPopupMenu(CMenu *pcsMenu);

	afx_msg VOID	OnCommandAdd();
	afx_msg VOID	OnCommandRemove();
	afx_msg VOID	OnCommandRemoveAll();
	afx_msg VOID	OnCommandFlags();
	afx_msg VOID	OnCommandData();
	afx_msg VOID	OnCommandStart();

public:
	virtual BOOL Create(AgcdCharacterTemplate *pstAgcdCharacterTemplate, CWnd* pParentWnd = NULL);

protected :
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnClose();
	afx_msg void OnNMRclickTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);	

	DECLARE_MESSAGE_MAP()
};
