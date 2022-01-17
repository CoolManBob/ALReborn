// AcUITree.h: interface for the AcUITree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUITREE_H__46136627_85DD_492B_B32D_0F1B338148BE__INCLUDED_)
#define AFX_ACUITREE_H__46136627_85DD_492B_B32D_0F1B338148BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcUIBase.h"
#include "AcUIList.h"

typedef struct AcUITreeItemInfo
{
	INT32													m_lDepth;
	BOOL													m_bHasChild;
	BOOL													m_bItemOpened;

	AcUITreeItemInfo( void )
	{
		m_lDepth = 0;
		m_bHasChild = 0;
		m_bItemOpened = 0;
	}
} AcUITreeItemInfo;

class AcUITree;

class AcUITreeItem : public AcUIListItem
{
private:

	AcUITree*												m_pcsTree;
	AcUITreeItemInfo*										m_pstItemInfo;

	AcUIButton*												m_pcsButtonClose;
	AcUIButton*												m_pcsButtonOpen;

public:
	AcUITreeItem( void );
	AcUITreeItem( AcUITree *pcsTree, AcUITreeItemInfo *pstItemInfo );
	virtual ~AcUITreeItem( void );

	virtual BOOL				OnLButtonDblClk				( RsMouseStatus *ms );
	virtual VOID				OnMoveWindow				( void );
	virtual VOID				OnWindowRender				( void );
	virtual	BOOL				OnCommand					( INT32	nID , PVOID pParam	);

	VOID						SetTreeInfo					( AcUITree *pcsTree, AcUITreeItemInfo *pstItemInfo );
	VOID						SetTreeButton				( AcUIButton *pcsButtonClose, AcUIButton *pcsButtonOpen );
	VOID						SetTreeButtonClose			( AcUIButton *pcsButtonClose );
	VOID						SetTreeButtonOpen			( AcUIButton *pcsButtonOpen );

	VOID						RefreshItem					( void );
};

class AcUITree : public AcUIList  
{
private:
	AcUITreeItemInfo*										m_pastItemInfo;

public:
	INT32													m_lItemDepthMargin;
	BOOL													m_bNeedRefresh;

	INT32													m_lTotalVisibleItem; // 현재 화면에 보이는 Item 의 수 (즉 닫혀서 보이지 않는 것 제외)
	INT32													m_lStartRowIndex;	 // 현재 화면에 보이는 Item에서 Start Row의 Index

public:
	AcUITree( void );
	virtual ~AcUITree( void );

	virtual VOID				OnWindowRender				( void );
	virtual AcUIListItem*		OnNewListItem				( INT32 lIndex );
	virtual VOID				OnChangeTotalNum			( void );
	virtual	BOOL				OnMouseWheel				( INT32	lDelta );

	virtual VOID				RefreshList					( void );
	virtual BOOL				IsValidListItem				( AcUIListItem *pListItem, INT32 lIndex	);

	virtual VOID				UpdateStartRowByScroll		( void );
	virtual VOID				UpdateScroll				( void );

	VOID						SetItemDepth				( INT32 lIndex, INT32 lDepth );
	VOID						UpdateStartRow				( INT32 lStartRow );

	AcUITreeItemInfo*			GetTreeItemInfo				( INT32 lIndex );
	BOOL						OpenItem					( INT32 lIndex );
};

#endif // !defined(AFX_ACUITREE_H__46136627_85DD_492B_B32D_0F1B338148BE__INCLUDED_)
