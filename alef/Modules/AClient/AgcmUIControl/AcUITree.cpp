// AcUITree.cpp: implementation of the AcUITree class.
//
//////////////////////////////////////////////////////////////////////

#include "AcUITree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcUITree::AcUITree( void )
{
	m_nType				= TYPE_TREE;

	m_lItemDepthMargin	= 10;
	m_pastItemInfo		= NULL;

	m_bNeedRefresh		= FALSE;

	m_lTotalVisibleItem	= 0;
	m_lStartRowIndex	= 0;
}

AcUITree::~AcUITree( void )
{
}

VOID AcUITree::OnWindowRender( void )
{
	if( m_bNeedRefresh )
	{
		RefreshList();
		m_bNeedRefresh = FALSE;
	}

	AcUIList::OnWindowRender();
}

AcUIListItem* AcUITree::OnNewListItem( INT32 lIndex )
{
	if( !m_pastItemInfo || lIndex >= m_lTotalListItemNum ) return NULL;
	INT32 lCurrentDepth = m_pastItemInfo[ lIndex ].m_lDepth;

	// Parent Item이 Open 되어있나 검사.
	// Close된 것이 하나라도 있다면, return NULL
	for( INT32 nItemInfoCount = lIndex - 1; nItemInfoCount >= 0; --nItemInfoCount )
	{
		AcUITreeItemInfo* pItemInfo = GetTreeItemInfo( nItemInfoCount );
		if( pItemInfo )
		{
			if( lCurrentDepth <= pItemInfo->m_lDepth ) continue;
			if( !pItemInfo->m_bItemOpened ) return NULL;
			lCurrentDepth = pItemInfo->m_lDepth;
		}
	}

	return ( AcUIListItem* )new AcUITreeItem( this, GetTreeItemInfo( lIndex ) );
}

VOID AcUITree::OnChangeTotalNum( void )
{
	if( m_pastItemInfo )
	{
		delete[] m_pastItemInfo;
	}

	m_pastItemInfo = new AcUITreeItemInfo[ m_lTotalListItemNum ];
	if( m_ppListItem )
	{
		for( INT32 nItemCount = 0; nItemCount < m_lVisibleListItemRow * m_lItemColumn; ++nItemCount )
		{
			if( m_ppListItem[ nItemCount ] )
			{
				AcUITreeItemInfo* pItemInfo = GetTreeItemInfo( m_ppListItem[ nItemCount ]->m_lItemIndex );
				( ( AcUITreeItem* )m_ppListItem[ nItemCount ] )->SetTreeInfo( this, pItemInfo );
			}
		}
	}
}

BOOL AcUITree::IsValidListItem( AcUIListItem *pListItem, INT32 lIndex )
{
	if( !m_pastItemInfo || !pListItem || lIndex >= m_lTotalListItemNum ) return FALSE;

	AcUITreeItemInfo* pItemInfo = GetTreeItemInfo( lIndex );
	if( !pItemInfo ) return FALSE;

	INT32 lCurrentDepth = pItemInfo->m_lDepth;

	// Parent Item이 Open 되어있나 검사.
	// Close된 것이 하나라도 있다면, return NULL
	for( INT32 nItemInfoCount = lIndex - 1; nItemInfoCount >= 0; --nItemInfoCount )
	{
		pItemInfo = GetTreeItemInfo( nItemInfoCount );
		if( pItemInfo )
		{
			if( lCurrentDepth <= pItemInfo->m_lDepth ) continue;
			if( !pItemInfo->m_bItemOpened ) return FALSE;
			lCurrentDepth = pItemInfo->m_lDepth;
		}
	}

	pListItem->m_lItemIndex = lIndex;
	( ( AcUITreeItem* )pListItem )->SetTreeInfo( this, GetTreeItemInfo( lIndex ) );
	( ( AcUITreeItem* )pListItem )->RefreshItem();

	return TRUE;
}

VOID AcUITree::RefreshList( void )
{
	INT32 lOpenedDepth = -1;
	AcUIList::RefreshList();

	m_lTotalVisibleItem = 0;
	m_lStartRowIndex = 0;

	for( INT32 nItemRowCount = 0; nItemRowCount < m_lTotalItemRow; ++nItemRowCount )
	{
		AcUITreeItemInfo* pItemInfo = GetTreeItemInfo( nItemRowCount );
		if( pItemInfo )
		{
			// 현재 오픈되있는Depth보다 1 크다는 것은 보인다는 얘기
			if( lOpenedDepth + 1 >= pItemInfo->m_lDepth )
			{
				++m_lTotalVisibleItem;
			}

			// 현재 Depth가 Open된 Depth 바로 밑이고 Open되어있다면, OpenedDepth Update
			if( lOpenedDepth + 1 == pItemInfo->m_lDepth && pItemInfo->m_bItemOpened )
			{
				lOpenedDepth = pItemInfo->m_lDepth;
			}
			// 현재 Depth가 Open된 Depth보다 작거나 같고 Close되어있다면, OpenedDepth 한칸 올리기
			else if( lOpenedDepth >= pItemInfo->m_lDepth && !pItemInfo->m_bItemOpened )
			{
				lOpenedDepth = pItemInfo->m_lDepth - 1;
			}

			if( m_lListItemStartRow == nItemRowCount )
			{
				m_lStartRowIndex = m_lTotalVisibleItem - 1;
				if( m_lStartRowIndex < 0 )
				{
					m_lStartRowIndex = 0;
				}
			}
		}
	}

	UpdateScroll();
}

VOID AcUITree::UpdateStartRowByScroll( void )
{
	if( m_pcsScroll )
	{
		if( m_lTotalVisibleItem <= m_lVisibleListItemRow ) return;

		FLOAT fValue = m_pcsScroll->GetScrollValue();
		INT32 lValue = ( INT32 )( ( m_lTotalVisibleItem - m_lVisibleListItemRow ) * fValue );

		UpdateStartRow( lValue );
	}
}

VOID AcUITree::UpdateStartRow( INT32 lStartRow )
{
	INT32 nItemRowCount = 0;
	INT32 lTotalVisibleItem = 0;
	INT32 lOpenedDepth = -1;

	for( nItemRowCount = 0; nItemRowCount < m_lTotalItemRow; ++nItemRowCount )
	{
		AcUITreeItemInfo* pItemInfo = GetTreeItemInfo( nItemRowCount );

		if( lOpenedDepth + 1 >= pItemInfo->m_lDepth )
		{
			++lTotalVisibleItem;
		}

		// 현재 Depth가 Open된 Depth 바로 밑이고 Open되어있다면, OpenedDepth Update
		if( lOpenedDepth + 1 == pItemInfo->m_lDepth && pItemInfo->m_bItemOpened )
		{
			lOpenedDepth = pItemInfo->m_lDepth;
		}
		// 현재 Depth가 Open된 Depth보다 작거나 같고 Close되어있다면, OpenedDepth 한칸 올리기
		else if( lOpenedDepth >= pItemInfo->m_lDepth && !pItemInfo->m_bItemOpened )
		{
			lOpenedDepth = pItemInfo->m_lDepth - 1;
		}

		if( lTotalVisibleItem - 1 == lStartRow ) break;
	}

	if( nItemRowCount != m_lListItemStartRow )
	{
		SetListItemWindowStartRow( nItemRowCount, FALSE, FALSE );
	}
}

VOID AcUITree::UpdateScroll( void )
{
	if( m_pcsScroll )
	{
		if( m_lTotalItemRow > m_lVisibleListItemRow )
		{
			m_pcsScroll->SetScrollButtonInfo( m_pcsScroll->m_bVScroll, m_pcsScroll->m_lMinPosition, m_pcsScroll->m_lMaxPosition,
				1.0f / ( FLOAT )( m_lTotalVisibleItem - m_lVisibleListItemRow ), m_pcsScroll->m_bMoveByUnit );

			m_pcsScroll->SetScrollValue( m_lStartRowIndex / ( FLOAT )( m_lTotalVisibleItem - m_lVisibleListItemRow ), FALSE );
		}
		else
		{
			m_pcsScroll->m_fScrollUnit = 0.0f;
			m_pcsScroll->SetScrollValue( 0.0f, FALSE );
		}
	}
}

VOID AcUITree::SetItemDepth( INT32 lIndex, INT32 lDepth )
{
	if( !m_pastItemInfo || lIndex >= m_lTotalListItemNum || lDepth < 0 ) return;
	if( lIndex > 0 )
	{
		if( m_pastItemInfo[ lIndex - 1 ].m_lDepth < lDepth )
		{
			m_pastItemInfo[ lIndex - 1 ].m_bHasChild = TRUE;
		}
		else
		{
			m_pastItemInfo[ lIndex - 1 ].m_bHasChild = FALSE;
		}
	}

	m_pastItemInfo[ lIndex ].m_lDepth = lDepth;
}

BOOL AcUITree::OnMouseWheel( INT32 lDelta )
{
	if( m_pcsScroll )
	{
		if( m_lTotalVisibleItem <= m_lVisibleListItemRow )
		{
			if( m_lListItemStartRow != 0 )
			{
				SetListItemWindowStartRow( 0 );
			}

			return TRUE;
		}

		INT32 lStartRow = lDelta > 0 ? m_lStartRowIndex - 1 : m_lStartRowIndex + 1;

		if( lStartRow < 0 )
		{
			lStartRow = 0;
		}
		else if( lStartRow > m_lTotalVisibleItem - m_lVisibleListItemRow )
		{
			lStartRow = m_lTotalVisibleItem - m_lVisibleListItemRow;
		}

		if( m_lTotalVisibleItem == m_lVisibleListItemRow )
		{
			m_pcsScroll->SetScrollValue( 0 );
		}
		else
		{
			m_pcsScroll->SetScrollValue( lStartRow / ( m_lTotalVisibleItem - m_lVisibleListItemRow + 0.0f ), FALSE );
		}

		UpdateStartRow( lStartRow );
		return TRUE;
	}

	return TRUE;
}

BOOL AcUITree::OpenItem( INT32 lIndex )
{
	AcUITreeItemInfo* pstInfo = GetTreeItemInfo( lIndex );
	if( !pstInfo ) return FALSE;

	INT32 lCurrentDepth = pstInfo->m_lDepth;

	for( INT32 nItemRowCount = lIndex; nItemRowCount >= 0; --nItemRowCount )
	{
		AcUITreeItemInfo* pItemInfo = GetTreeItemInfo( nItemRowCount );
		if( pItemInfo )
		{
			if( lCurrentDepth > pItemInfo->m_lDepth )
			{
				pItemInfo->m_bItemOpened = TRUE;
				lCurrentDepth = pItemInfo->m_lDepth;
			}
		}
	}

	SetListItemWindowStartRow( lIndex, FALSE, TRUE );
	return TRUE;
}

AcUITreeItemInfo* AcUITree::GetTreeItemInfo( INT32 lIndex )
{
	if( !m_pastItemInfo || lIndex < 0 || lIndex >= m_lTotalListItemNum ) return NULL;
	return m_pastItemInfo + lIndex;
}





AcUITreeItem::AcUITreeItem( void )
{
	m_nType				= TYPE_TREE_ITEM;

	m_pcsButtonOpen		= NULL;
	m_pcsButtonClose	= NULL;

	SetTreeInfo( NULL, NULL );
}

AcUITreeItem::AcUITreeItem( AcUITree *pcsTree, AcUITreeItemInfo *pstItemInfo )
{
	m_nType				= TYPE_TREE_ITEM;

	m_pcsButtonOpen		= NULL;
	m_pcsButtonClose	= NULL;

	SetTreeInfo( pcsTree, pstItemInfo );
}

AcUITreeItem::~AcUITreeItem( void )
{
}

BOOL AcUITreeItem::OnLButtonDblClk( RsMouseStatus *ms )
{
	if( m_pcsTree )
	{
		m_pstItemInfo = m_pcsTree->GetTreeItemInfo( m_lItemIndex );
	}

	if( !m_pstItemInfo ) return FALSE;
	m_pstItemInfo->m_bItemOpened = !m_pstItemInfo->m_bItemOpened;

	RefreshItem();
	return TRUE;
}

VOID AcUITreeItem::OnMoveWindow( void )
{
	if( m_pcsTree )
	{
		m_pstItemInfo = m_pcsTree->GetTreeItemInfo( m_lItemIndex );
		if( !m_pstItemInfo ) return;

		INT32 lMargin = m_pcsTree->m_lItemDepthMargin * m_pstItemInfo->m_lDepth;
		AgcWindowNode* pstNode = m_listChild.head;

		while( pstNode )
		{
			if( pstNode->pWindow->x + pstNode->pWindow->w > w )
			{
				pstNode->pWindow->MoveWindow( pstNode->pWindow->x, pstNode->pWindow->y, pstNode->pWindow->w - lMargin, pstNode->pWindow->h );
			}

			pstNode = pstNode->next;
		}

		x += lMargin;
		w -= lMargin;
	}
}

VOID AcUITreeItem::OnWindowRender( void )
{
	AcUIListItem::OnWindowRender();
}

BOOL AcUITreeItem::OnCommand( INT32	nID , PVOID pParam )
{
	PROFILE( "AcUIList::OnCommand" );

	if( m_pcsTree )
	{
		m_pstItemInfo = m_pcsTree->GetTreeItemInfo( m_lItemIndex );
	}

	if( m_pstItemInfo )
	{
		if( m_pcsButtonClose && *( INT32* )pParam == m_pcsButtonClose->m_lControlID )
		{
			// Close Button이 눌렸다.
			switch( nID )
			{
			case UICM_BUTTON_MESSAGE_CLICK :
				{
					m_pstItemInfo->m_bItemOpened = TRUE;
					RefreshItem();

					return AcUIBase::OnCommand( nID, pParam );
				}
				break;
			}
		}
		else if( m_pstItemInfo->m_bHasChild && m_pcsButtonOpen && *( INT32* )pParam == m_pcsButtonOpen->m_lControlID )
		{
			// Open Button이 눌렸다.
			switch( nID )
			{
			case UICM_BUTTON_MESSAGE_CLICK :
				{
					m_pstItemInfo->m_bItemOpened = FALSE;
					RefreshItem();

					return AcUIBase::OnCommand( nID, pParam );
				}
				break;
			}
		}
	}

	return AcUIBase::OnCommand( nID, pParam );
}

VOID AcUITreeItem::SetTreeInfo( AcUITree *pcsTree, AcUITreeItemInfo *pstItemInfo )
{
	m_pcsTree		= pcsTree;
	m_pstItemInfo	= pstItemInfo;
}

VOID AcUITreeItem::SetTreeButton( AcUIButton *pcsButtonClose, AcUIButton *pcsButtonOpen )
{
	SetTreeButtonClose( pcsButtonClose );
	SetTreeButtonOpen( pcsButtonOpen );
}

VOID AcUITreeItem::SetTreeButtonClose( AcUIButton *pcsButtonClose )
{
	m_pcsButtonClose = pcsButtonClose;
	if( !m_pcsButtonClose )	return;

	if( m_pcsTree )
	{
		m_pstItemInfo = m_pcsTree->GetTreeItemInfo( m_lItemIndex );
	}

	if( !m_pstItemInfo ) return;	

	if( m_pstItemInfo->m_bItemOpened || !m_pstItemInfo->m_bHasChild )
	{
		m_pcsButtonClose->m_Property.bVisible = FALSE;
	}
	else
	{
		m_pcsButtonClose->m_Property.bVisible = TRUE;
	}
}

VOID AcUITreeItem::SetTreeButtonOpen( AcUIButton *pcsButtonOpen )
{
	m_pcsButtonOpen = pcsButtonOpen;
	if( !m_pcsButtonOpen ) return;

	if( m_pcsTree )
	{
		m_pstItemInfo = m_pcsTree->GetTreeItemInfo( m_lItemIndex );
	}

	if( !m_pstItemInfo ) return;	

	if( m_pstItemInfo->m_bItemOpened && m_pstItemInfo->m_bHasChild )
	{
		m_pcsButtonOpen->m_Property.bVisible = TRUE;
	}
	else
	{
		m_pcsButtonOpen->m_Property.bVisible = FALSE;
	}
}

VOID AcUITreeItem::RefreshItem( void )
{
	if( m_pcsTree )
	{
		m_pstItemInfo = m_pcsTree->GetTreeItemInfo( m_lItemIndex );
	}

	if( !m_pstItemInfo ) return;

	if( m_pcsButtonClose )
	{
		m_pcsButtonClose->m_Property.bVisible = ( !m_pstItemInfo->m_bItemOpened && m_pstItemInfo->m_bHasChild );
	}

	if( m_pcsButtonOpen )
	{
		m_pcsButtonOpen->m_Property.bVisible = ( m_pstItemInfo->m_bItemOpened && m_pstItemInfo->m_bHasChild );
	}

	if( m_pcsTree )
	{
		m_pcsTree->m_bNeedRefresh = TRUE;
	}
}
