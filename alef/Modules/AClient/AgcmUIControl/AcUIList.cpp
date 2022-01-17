#include "AcUIList.h"
#include "AcUIEdit.h"

#include "ApMemoryTracker.h"

VOID AcUIListItem::OnWindowRender( void )
{
	AcUIBase::OnWindowRender();
	if( !m_pcsList ) return;
	if( m_pcsList->m_lSelectedIndex	!= m_lItemIndex ) return;

	RwTexture* pTexture = _GetTextureByID( m_pcsList->m_lSelectedItemTextureID );
	if( !pTexture || !pTexture->raster ) return;

	INT32 nAbsolute_x = 0;
	INT32 nAbsolute_y = 0;
	ClientToScreen( &nAbsolute_x, &nAbsolute_y );

	FLOAT fPosX = ( FLOAT )nAbsolute_x + m_lImageOffsetX;
	FLOAT fPosY = ( FLOAT )nAbsolute_y + m_lImageOffsetY;
	UINT8 nAlpha = ( UINT8 )( m_stRenderInfo.cRenderAlpha * ( m_pfAlpha ? *m_pfAlpha : 1 ) );

	if( FALSE != m_bShrinkMode )
	{
		g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, w, h, 
			m_stRenderInfo.fStartU, m_stRenderInfo.fStartV, m_stRenderInfo.fEndU, m_stRenderInfo.fEndV,
			m_stRenderInfo.lRenderColor, nAlpha );
	}
	else if( m_bClipImage )
	{
		INT32 lWidth = RwRasterGetWidth( pTexture->raster );
		INT32 lHeight = RwRasterGetHeight( pTexture->raster );

		FLOAT fU = 1.0f;
		FLOAT fV = 1.0f;

		if( lWidth * m_stRenderInfo.fScale > w )
		{
			fU = w / ( lWidth * m_stRenderInfo.fScale );
			lWidth = w;
		}

		if( lHeight * m_stRenderInfo.fScale > h )
		{
			fV = h / ( lHeight * m_stRenderInfo.fScale );
			lHeight = h;
		}

		g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, ( FLOAT )lWidth, ( FLOAT )lHeight,
			0.0f, 0.0f, fU, fV, m_stRenderInfo.lRenderColor, nAlpha );
			
	}
	else
	{
		FLOAT fWidth = ( FLOAT )( RwRasterGetWidth( pTexture->raster ) ) * m_stRenderInfo.fScale;
		FLOAT fHeight = ( FLOAT )( RwRasterGetHeight( pTexture->raster ) ) * m_stRenderInfo.fScale;

		g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, fWidth, fHeight, 
			m_stRenderInfo.fStartU, m_stRenderInfo.fStartV, m_stRenderInfo.fEndU, m_stRenderInfo.fEndV,
			m_stRenderInfo.lRenderColor, nAlpha );
	}
}

RwTexture* AcUIListItem::_GetTextureByID( INT32 nTextureID )
{
	if( !m_pcsList ) return NULL;
	return m_pcsList->m_csTextureList.GetImage_ID( m_pcsList->m_lSelectedItemTextureID );
}

/*****************************************************************
*   Function : AcUIList
*   Comment  : AcUIList
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
AcUIList::AcUIList( void )
{
	m_nType							= TYPE_LIST;
	m_ppListItem					= NULL;

	m_pfConstructorCallback			= NULL;
	m_paConstructorClass			= NULL;
	m_pConstructorData				= NULL;

	m_pfDestructorCallback			= NULL;
	m_paDestructorClass				= NULL;
	m_pDestructorData				= NULL;

	m_pfRefreshCallback				= NULL;
	m_paRefreshClass				= NULL;
	m_pRefreshData					= NULL;

	m_lUseListItemInfoStringIndex	= 0;

	m_lTotalListItemNum				= 0;
	m_lVisibleListItemRow			= 0;
	m_lCurrentExistListItemRow		= 0;
	m_lListItemStartRow				= 0;

	m_lListItemWidth				= 0;	
	m_lListItemHeight				= 0;
	m_lListItemStartX				= 0;
	m_lListItemStartY				= 0;

	m_pcsScroll						= NULL;

	m_lItemColumn					= 1;
	m_lTotalItemRow					= 0;

	m_lSelectedItemTextureID		= 0;
	m_lSelectedIndex				= -1;

	memset( &m_csMouseMoveStatus, 0, sizeof( m_csMouseMoveStatus ) );
}

/*****************************************************************
*   Function : ~AcUIList
*   Comment  : ~AcUIList
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
AcUIList::~AcUIList( void )
{
	DeleteAllListItemInfo();
}

/*****************************************************************
*   Function : SetListItemWindowTotalNum
*   Comment  : SetListItemWindowTotalNum
*   Date&Time : 2003-10-01, 오후 2:07
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::SetListItemWindowTotalNum( INT32 lTotalNum )
{
	// 0개 밑으로는 설정 불가능
	if( lTotalNum < 0 )	return FALSE;

	// 현재 갯수와 같으면 안해도 그만..
	if( lTotalNum == m_lTotalListItemNum ) return TRUE;

	m_lTotalListItemNum = lTotalNum;
	if( !lTotalNum )
	{
		m_lTotalItemRow = 0;
	}
	else
	{
		m_lTotalItemRow = ( m_lTotalListItemNum - 1 ) / m_lItemColumn + 1;
	}

	UpdateScroll();
	OnChangeTotalNum();
	return TRUE;
}

/*****************************************************************
*   Function : SetListItemWindowVisibleRow
*   Comment  : SetListItemWindowVisibleRow
*   Date&Time : 2003-10-01, 오후 2:07
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::SetListItemWindowVisibleRow( INT32 lVisibleRow )
{
	PROFILE("AcUIList::SetListItemWindowVisibleRow");

	if( m_lVisibleListItemRow == lVisibleRow ) return TRUE;
	DeleteAllListItemInfo();

	m_lVisibleListItemRow =	lVisibleRow;
	BOOL bResult = SetMemoryppListItem();

	SetListItemWindowStartRow( m_lListItemStartRow );
	return bResult;
}

/*****************************************************************
*   Function : SetListItemWindowStartRow
*   Comment  : SetListItemWindowStartRow
*   Date&Time : 2003-10-01, 오전 11:27
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::SetListItemWindowStartRow( INT32 lStartRow, BOOL bDestroy, BOOL bChangeScroll )
{
	//	매개변수 bDestroy 를 쓰는데가 없다.. 예전에 쓰던걸까.. 지울까;;
	PROFILE("AcUIList::SetListItemWindowStartRow");
	if( lStartRow < 0 )	return;

	m_lListItemStartRow = lStartRow;
	m_lCurrentExistListItemRow = 0;

	if( m_lTotalItemRow > 0 && m_lListItemStartRow > m_lTotalItemRow - 1 )
	{
		m_lListItemStartRow = m_lTotalItemRow - 1;
	}

	RefreshList();

	if( m_pcsScroll && bChangeScroll )
	{
		UpdateScroll();
	}

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, (PVOID) UICM_LIST_SCROLL, (PVOID) &m_lControlID );
	}
}

VOID AcUIList::RefreshList( void )
{
	INT32 lCurrentListItemsTotalHeight	= 0;
	INT32 lMaxItemHeightInColumn		= 0;
	INT32 lListStart					= 0;
	INT32 lVisibleItemNum				= m_lVisibleListItemRow * m_lItemColumn;
	INT32 lTotalItemNum					= m_lTotalItemRow * m_lItemColumn;
	BOOL bFullHeight					= FALSE;

	INT32 lIndex;
	INT32 lIndex2;
	INT32 lItemIndex;

	if( !m_ppListItem ) return;

	//@{ 2006/07/01 burumal
	if( g_pEngine->GetCharCheckState() == FALSE && g_pEngine->m_pActiveEdit && g_pEngine->m_pActiveEdit->m_bActiveEdit )
	{
		AcUIEdit* pEdit = ( AcUIEdit* )g_pEngine->m_pActiveEdit;
		if( pEdit && pEdit->m_bForHotkey )
		{
			pEdit->ReleaseMeActiveEdit();
		}
	}
	//@}

	// ppListItem Setting
	for( lItemIndex = m_lListItemStartRow * m_lItemColumn, lIndex = 0 ; lIndex <  lVisibleItemNum && lItemIndex < m_lTotalListItemNum ; ++lItemIndex )
	{
		for( lIndex2 = 0 ; lIndex2 < lVisibleItemNum ; ++lIndex2 )
		{
			if( m_ppListItem[ lIndex2 ] && m_ppListItem[ lIndex2 ]->m_lItemIndex == lItemIndex )
			{
				AcUIListItem* pcsItemTemp = m_ppListItem[ lIndex2 ];
				m_ppListItem[ lIndex2 ] = m_ppListItem[ lIndex ];
				m_ppListItem[ lIndex ] = pcsItemTemp;
				break;
			}
		}

		if( !m_ppListItem[ lIndex ] )
		{
			m_ppListItem[ lIndex ] = NewListItemWidnow( lItemIndex, h - lCurrentListItemsTotalHeight, &bFullHeight );
		}
		else
		{
			if( !IsValidListItem( m_ppListItem[ lIndex ], lItemIndex ) )
			{
				continue;
			}

			RefreshListItemWindow( m_ppListItem[ lIndex ], lItemIndex, h - lCurrentListItemsTotalHeight, &bFullHeight );
		}

		if( m_ppListItem[ lIndex ] )
		{
			if( bFullHeight )
			{
				m_ppListItem[ lIndex ]->m_Property.bVisible = FALSE;
			}
			else
			{
				m_ppListItem[ lIndex ]->m_Property.bVisible = TRUE;

				if( lMaxItemHeightInColumn < m_ppListItem[ lIndex ]->m_lListItemWindowHeight )
				{
					lMaxItemHeightInColumn = m_ppListItem[ lIndex ]->m_lListItemWindowHeight;
				}

				// 해당 Row의 마지막 Column이면
				if( ( lItemIndex % m_lItemColumn ) == m_lItemColumn - 1 )
				{
					lCurrentListItemsTotalHeight += lMaxItemHeightInColumn;
					lMaxItemHeightInColumn = 0;

					++m_lCurrentExistListItemRow;
				}
			}

			++lIndex;
		}
	}

	for( ; lIndex < lVisibleItemNum ; ++lIndex )
	{
		if( m_ppListItem[ lIndex ] )
		{
			m_ppListItem[ lIndex ]->m_Property.bVisible = FALSE;
		}
	}

	// MoveWindow 
	MoveListItemWindow();

	// 여기에 어쩔 수 없이 WindowListUpdate()를 넣는다.
	WindowListUpdate();
}

AcUIListItem* AcUIList::GetListItemPointer_Index( INT32 lIndex )
{
	// Valid Check 
	if( 0 == m_lTotalListItemNum || 0 == m_lVisibleListItemRow || NULL == m_ppListItem ) return NULL;

	int	item_num = m_lVisibleListItemRow * m_lItemColumn;
	for( INT32 nItemCount = 0 ; nItemCount < item_num ; ++nItemCount )
	{
		if( NULL != m_ppListItem[ nItemCount ] )
		{
			if( lIndex == m_ppListItem[ nItemCount ]->m_lItemIndex )
			{
				return ( m_ppListItem[ nItemCount ] );
			}
		}
	}

	return NULL;
}

/*****************************************************************
*   Function : SetListItemWindowMoveInfo
*   Comment  : SetListItemWindowMoveInfo
*   Date&Time : 2003-10-01, 오전 11:27
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::SetListItemWindowMoveInfo( INT32 lItemWidth, INT32 lItemHeight, INT32 lItemStartX, INT32 lItemStartY )
{
	// Input 
	m_lListItemWidth = lItemWidth;
	m_lListItemHeight =	lItemHeight;
	m_lListItemStartX =	lItemStartX;
	m_lListItemStartY =	lItemStartY;

	if( 0 == m_lTotalListItemNum || NULL == m_ppListItem ) return;
	MoveListItemWindow();
	return;
}

/*****************************************************************
*   Function : SetCallbackConstructor
*   Comment  : SetCallbackConstructor
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::SetCallbackConstructor( AcUIListCallback pfConstructor, PVOID pClass, PVOID pData )
{
	m_pfConstructorCallback = pfConstructor;
	m_paConstructorClass = pClass;
	m_pConstructorData = pData;
}

/*****************************************************************
*   Function : SetCallbackDestructor
*   Comment  : SetCallbackDestructor
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::SetCallbackDestructor( AcUIListCallback pfDestructor, PVOID pClass, PVOID pData )
{
	m_pfDestructorCallback = pfDestructor;
	m_paDestructorClass	= pClass;	
	m_pDestructorData =	pData;
}

/*****************************************************************
*   Function : SetCallbackRefresh
*   Comment  : SetCallbackRefresh
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::SetCallbackRefresh( AcUIListCallback pfRefresh, PVOID pClass, PVOID pData )
{
	m_pfRefreshCallback	= pfRefresh;
	m_paRefreshClass = pClass;	
	m_pRefreshData = pData;
}

/*****************************************************************
*   Function : SetMemoryppListItem
*   Comment  : SetMemoryppListItem
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::SetMemoryppListItem( void )
{
	// Valid Check 
	if( 0 >= m_lVisibleListItemRow ) return FALSE;

	// Set Memory 
	m_ppListItem = new AcUIListItem*[ m_lVisibleListItemRow * m_lItemColumn ];
	if( NULL == m_ppListItem ) return FALSE;

	INT32 item_num = m_lVisibleListItemRow * m_lItemColumn;
	for( INT32 nItemCount = 0 ; nItemCount < item_num ; ++nItemCount )
	{
		m_ppListItem[ nItemCount ] = NULL;
	}

	RefreshList();
	return TRUE;
}	

/*****************************************************************
*   Function : NewListItemWidnow
*   Comment  : Memory에 ListItem 을 할당하고 Constructor Callback 을 호출한다 
*   Date&Time : 2003-10-01, 오전 11:31
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
AcUIListItem* AcUIList::NewListItemWidnow( INT32 lIndex, INT32 lHeightLeft, BOOL *pbFullHeight )
{
	PROFILE("AcUIList::NewListItemWidnow");

	AcUIListItem* pListItem	= OnNewListItem( lIndex );
	if( !pListItem ) return NULL;

	if( pbFullHeight )
	{
		*pbFullHeight = FALSE;
	}

	pListItem->m_lItemIndex = lIndex;
	if( m_pfConstructorCallback )
	{
		if( !m_pfConstructorCallback( this, pListItem, m_paConstructorClass, m_pConstructorData, &( pListItem->m_lListItemWindowHeight ) ) )
		{
			delete pListItem;
			return NULL;
		}			
	}

	if( m_lListItemHeight > pListItem->m_lListItemWindowHeight )
	{
		pListItem->m_lListItemWindowHeight = m_lListItemHeight;
	}

	pListItem->MoveWindow( pListItem->x, pListItem->y, pListItem->w, m_lListItemHeight );
	AddChild( pListItem );

	if( lHeightLeft < m_lListItemHeight && pbFullHeight )
	{
		*pbFullHeight = TRUE;
	}

	return pListItem;
}

/*****************************************************************
*   Function : RefreshListItemWidnow
*   Comment  : Memory에 할당된 ListItem 으로 Refresh Callback 을 호출한다 
*   Date&Time : 2003-10-01, 오전 11:31
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL	AcUIList::RefreshListItemWindow( AcUIListItem *pListItem, INT32 lIndex, INT32 lHeightLeft, BOOL *pbFullHeight )
{
	PROFILE("AcUIList::RefreshListItemWidnow");
	if ( !pListItem ) return FALSE;

	if( pbFullHeight )
	{
		*pbFullHeight = FALSE;
	}

	pListItem->m_lItemIndex = lIndex ;

	if( m_pfRefreshCallback )
	{
		if( !m_pfRefreshCallback( this, pListItem, m_paConstructorClass, m_pConstructorData, &( pListItem->m_lListItemWindowHeight ) ) )
		{
			return FALSE;
		}			
	}

	if( m_lListItemHeight > pListItem->m_lListItemWindowHeight )
	{
		pListItem->m_lListItemWindowHeight = m_lListItemHeight;
	}

	pListItem->MoveWindow( pListItem->x, pListItem->y, pListItem->w, m_lListItemHeight );

	if( lHeightLeft < m_lListItemHeight && pbFullHeight )
	{
		*pbFullHeight = TRUE;
	}

	return TRUE;
}

/*****************************************************************
*   Function : DeleteListItemWindow
*   Comment  : Memory에서 ListItem 을 지우고 Destructor Callback 을 호출한다 
*   Date&Time : 2003-10-01, 오전 11:32
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::DeleteListItemWindow( AcUIListItem* pListItem )
{
	PROFILE("AcUIList::DeleteListItemWindow");
	if( NULL == pListItem )	return FALSE;

	if( m_pfDestructorCallback )
	{
		m_pfDestructorCallback( this, pListItem, m_paDestructorClass, m_pDestructorData, NULL );
	}

	DeleteChild( pListItem, TRUE, TRUE );
	return TRUE;
}

/*****************************************************************
*   Function : SetListInfo_String
*   Comment  : SetListInfo_String
*   Date&Time : 2003-09-30, 오후 4:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::SetListInfo_String( CHAR* pszString )
{
	PROFILE("AcUIList::SetListInfo_String");
	if ( !pszString ) return FALSE;

	AcdUIListItemInfo* pclListItemInfo = new AcdUIListItemInfo;
	strncpy( pclListItemInfo->m_szListItemString, pszString, ( ACUILIST_LIST_ITEM_STRING_LENGTH - 1 ) );

	m_listItemInfo.AddTail( pclListItemInfo );
	return TRUE;
}

/*****************************************************************
*   Function : DeleteListInfo_String
*   Comment  : DeleteListInfo_String
*   Date&Time : 2003-09-30, 오후 9:03
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::DeleteListInfo_String( CHAR* pszString )
{
	PROFILE("AcUIList::DeleteListInfo_String");

	// Valid Check 
	if ( !pszString ) return FALSE;
	else if( m_listItemInfo.GetCount() == 0 ) return FALSE;

	INT32 lListItemInfoCount = m_listItemInfo.GetCount();

	AuNode< AcdUIListItemInfo* >* pNode = m_listItemInfo.GetHeadNode();
	AuNode< AcdUIListItemInfo* >* pPreNode = NULL;
	AcdUIListItemInfo* pData = NULL;
	INT32 lIndex = 0 ;

	while( pNode )
	{
		pPreNode = pNode;
		pData = m_listItemInfo.GetNext( pNode );
		
		// 같은 게 있으면 지워버리기..
		if( !strcmp( pData->m_szListItemString, pszString ) )
		{
			//지우기 
			delete pData;
			m_listItemInfo.RemoveNode( pPreNode );

			if( m_lUseListItemInfoStringIndex >= lIndex )
			{
				--m_lUseListItemInfoStringIndex;
			}
			
			if( m_lUseListItemInfoStringIndex < 0 )
			{
				m_lUseListItemInfoStringIndex = 0;
			}

			return TRUE;
		}

		++lIndex;
	}

	return FALSE;
}

/*****************************************************************
*   Function : OnClose
*   Comment  : OnClose
*   Date&Time : 2003-09-30, 오후 4:52
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::OnClose( void )
{
	AcUIBase::OnClose();
	DeleteAllListItemInfo();
}

BOOL AcUIList::OnCommand( INT32	nID, PVOID pParam )
{
	PROFILE("AcUIList::OnCommand");

	if( m_pcsScroll && *( INT32* )pParam == m_pcsScroll->m_lControlID )
	{
		// Scroll이 움직였다.
		switch( nID )
		{
		case UICM_SCROLL_CHANGE_SCROLLVALUE:	UpdateStartRowByScroll();		break;
		case UICM_BASE_MESSAGE_CLOSE:			SetListScroll( NULL );			break;
		}
	}
		
	return AcUIBase::OnCommand(nID, pParam);
}

/*****************************************************************
*   Function : GetListItemInfoString
*   Comment  : GetListItemInfoString
*   Date&Time : 2003-09-30, 오후 8:31
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
CHAR* AcUIList::GetListItemInfoString( INT32 lIndex )
{
	if( m_listItemInfo.GetCount() <= lIndex ) return NULL;
	else if( m_listItemInfo.GetCount() == 0 ) return NULL;
	
	return m_listItemInfo[ lIndex ]->m_szListItemString;
}

/*****************************************************************
*   Function : DeleteAllListItemInfo
*   Comment  : DeleteAllListItemInfo
*   Date&Time : 2003-09-30, 오후 9:00
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIList::DeleteAllListItemInfo( void )
{
	INT32 lListItemInfoCount = m_listItemInfo.GetCount();

	// 아이템 리스트정보 메모리 해제
	for( INT32 nItemInfoCount = 0 ; nItemInfoCount < lListItemInfoCount ; ++nItemInfoCount )
	{
		delete m_listItemInfo[ nItemInfoCount ];
	}

	// 실제 아이템 메모리 해제
	if( m_ppListItem )
	{
		int	max_num = m_lVisibleListItemRow * m_lItemColumn;
		for( INT32 nItemCount = 0 ; nItemCount < max_num ; ++nItemCount )
		{
			if ( m_ppListItem[ nItemCount ] )
			{
				DeleteListItemWindow( m_ppListItem[ nItemCount ] );
			}
		}

		delete[] m_ppListItem;
		m_ppListItem = NULL;
	}

	WindowListUpdate();
	m_listItemInfo.RemoveAll();
	return TRUE;
}

/*****************************************************************
*   Function : MoveListItemWindow
*   Comment  : MoveListItemWindow
*   Date&Time : 2003-10-01, 오후 1:47
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
VOID AcUIList::MoveListItemWindow( void )
{
	PROFILE("AcUIList::MoveListItemWindow");

	INT32 lWindowStart_y = 0;
	INT32 lItemNum = m_lVisibleListItemRow * m_lItemColumn;

	INT32 lIndex;
	INT32 lStartX;

	//m_lVisibleListItemRow로 바꿨다... Total에서...
	for( lIndex = 0 ; lIndex < lItemNum ; ++lIndex )
	{
		if( m_ppListItem[ lIndex ] )
		{
			lStartX = ( lIndex % m_lItemColumn ) * m_lListItemWidth;
			m_ppListItem[ lIndex ]->MoveWindow( m_lListItemStartX + lStartX, lWindowStart_y,
				m_lListItemWidth, m_ppListItem[ lIndex ]->m_lListItemWindowHeight );

			if( ( lIndex % m_lItemColumn ) == m_lItemColumn - 1 )
			{
				lWindowStart_y += m_ppListItem[ lIndex ]->m_lListItemWindowHeight;
			}
		}
	}
}

VOID AcUIList::GoNextListPage( void )
{
	PROFILE("AcUIList::GoNextListPage");

	if( m_lListItemStartRow + m_lCurrentExistListItemRow >= m_lTotalItemRow ) return;
	SetListItemWindowStartRow( m_lListItemStartRow + m_lCurrentExistListItemRow );
}
	
VOID AcUIList::GoPreListPage( void )
{
	PROFILE("AcUIList::GoPreListPage");

	INT32 lDestinationRow =	m_lListItemStartRow;
	INT32 lTotalHeight = 0;
	INT32 lMaxHeightInRow = 0;

	if( m_lListItemStartRow == 0 ) return;

	INT32 lEndIndex	= ( m_lListItemStartRow - m_lVisibleListItemRow ) * m_lItemColumn;
	if( lEndIndex < 0 )
	{
		lEndIndex = 0;
	}

	for( INT32 nItemCount = m_lListItemStartRow * m_lItemColumn - 1 ; nItemCount >= lEndIndex ; --nItemCount )
	{
		if( m_ppListItem[ nItemCount ] )
		{
			if( lMaxHeightInRow < m_ppListItem[ nItemCount ]->m_lListItemWindowHeight )
			{
				lMaxHeightInRow = m_ppListItem[ nItemCount ]->m_lListItemWindowHeight;
			}

			if( !( nItemCount % m_lItemColumn ) )
			{
				lTotalHeight += lMaxHeightInRow	;
			}
		}

		if( h >= lTotalHeight )
		{
			lDestinationRow = nItemCount / m_lItemColumn;
		}
	}

	SetListItemWindowStartRow( lDestinationRow );
}

VOID AcUIList::SetListItemColumn( INT32 lColumn )
{
	if( lColumn <= 0 ) return;

	INT32 lPrevVisibleRow = m_lVisibleListItemRow;
	SetListItemWindowVisibleRow( 0 );

	m_lItemColumn = lColumn;
	m_lTotalItemRow	= ( m_lTotalListItemNum - 1 ) / m_lItemColumn + 1;

	SetListItemWindowVisibleRow( lPrevVisibleRow );
}

AcUIListItem* AcUIList::OnNewListItem( INT32 lIndex )
{
	AcUIListItem* pcsItem = new AcUIListItem;
	pcsItem->m_pcsList = this;
	return pcsItem;
}

VOID AcUIList::UpdateStartRowByScroll( void )
{
	if( m_pcsScroll )
	{
		FLOAT fValue = m_pcsScroll->GetScrollValue();
		INT32 lValue = INT32( ( m_lTotalItemRow - m_lVisibleListItemRow) * fValue );

		if( lValue != m_lListItemStartRow )
		{
			SetListItemWindowStartRow( lValue, FALSE, FALSE );
		}
	}
}

VOID AcUIList::UpdateScroll( void )
{
	if( m_pcsScroll )
	{
		if( m_lTotalItemRow > m_lVisibleListItemRow )
		{
			m_pcsScroll->SetScrollButtonInfo( m_pcsScroll->m_bVScroll, m_pcsScroll->m_lMinPosition, m_pcsScroll->m_lMaxPosition,
				1.0f / ( FLOAT )( m_lTotalItemRow - m_lVisibleListItemRow ), m_pcsScroll->m_bMoveByUnit );

			m_pcsScroll->SetScrollValue( m_lListItemStartRow / ( FLOAT )( m_lTotalItemRow - m_lVisibleListItemRow ), FALSE );
		}
		else
		{
			m_pcsScroll->m_fScrollUnit = 0.0f;
			m_pcsScroll->SetScrollValue( 0.0f, FALSE );
		}
	}
}

BOOL AcUIList::OnMouseWheel( INT32 lDelta )
{
	if( m_pcsScroll )
	{
		if( m_lTotalItemRow <= m_lVisibleListItemRow )
		{
			if( m_lListItemStartRow != 0 )
			{
				SetListItemWindowStartRow( 0 );
			}

			return TRUE;
		}

		INT32 lStartRow = lDelta > 0 ? m_lListItemStartRow -1 : m_lListItemStartRow + 1;

		if( lStartRow < 0 )
		{
			lStartRow = 0;
		}
		else if( lStartRow > m_lTotalItemRow - m_lVisibleListItemRow )
		{
			lStartRow = m_lTotalItemRow - m_lVisibleListItemRow;
		}

		if( m_lTotalItemRow == m_lVisibleListItemRow )
		{
			m_pcsScroll->SetScrollValue( 0 );
		}
		else
		{
			SetListItemWindowStartRow( lStartRow, FALSE, TRUE );
		}

		return TRUE;
	}

	return TRUE;
}

BOOL AcUIList::OnMouseMove( RsMouseStatus *ms )
{
	//m_csMouseMoveStatus = *ms;

	//int nPosX = m_csMouseMoveStatus.pos.x;
	//int nPosY = m_csMouseMoveStatus.pos.y;

	//if( !m_ppListItem ) return FALSE;

	//int nListItemCount = m_lVisibleListItemRow * m_lItemColumn;
	//for( int nCount = 0 ; nCount < nListItemCount ; nCount++ )
	//{
	//	if( m_ppListItem[ nCount ] )
	//	{
	//		int nItemPosX = m_ppListItem[ nCount ]->x;
	//		int nItemPosY = m_ppListItem[ nCount ]->y;
	//		int nItemWidth = m_ppListItem[ nCount ]->w;
	//		int nItemHeight = m_ppListItem[ nCount ]->h;

	//		if( nPosX >= nItemPosX  && nPosX <  ( nItemPosX + nItemWidth ) &&
	//			nPosY >= nItemPosY  && nPosY <  ( nItemPosY + nItemHeight ) )
	//		{
	//			SelectItem( nCount );
	//		}
	//	}
	//}

	return FALSE;
}

BOOL AcUIList::OnLButtonUp( RsMouseStatus *ms	)
{
	m_csMouseMoveStatus = *ms;

	int nPosX = m_csMouseMoveStatus.pos.x;
	int nPosY = m_csMouseMoveStatus.pos.y;

	if( !m_ppListItem ) return FALSE;

	int nListItemCount = m_lVisibleListItemRow * m_lItemColumn;
	for( int nCount = 0 ; nCount < nListItemCount ; nCount++ )
	{
		if( m_ppListItem[ nCount ] )
		{
			int nItemPosX = m_ppListItem[ nCount ]->x;
			int nItemPosY = m_ppListItem[ nCount ]->y;
			int nItemWidth = m_ppListItem[ nCount ]->w;
			int nItemHeight = m_ppListItem[ nCount ]->h;

			if( nPosX >= nItemPosX  && nPosX <  ( nItemPosX + nItemWidth ) &&
				nPosY >= nItemPosY  && nPosY <  ( nItemPosY + nItemHeight ) )
			{
				if( m_ppListItem[ nCount ]->m_lStaticStringLength > 0 )
				{
					SelectItem( m_ppListItem[ nCount ]->m_lItemIndex );
				}
			}
		}
	}

	return FALSE;
}

VOID AcUIList::SetSelectedTexture( INT32 lTextureID )
{
	m_lSelectedItemTextureID = lTextureID;
}

VOID AcUIList::SelectItem( INT32 lSelectedIndex )
{
	m_lSelectedIndex = lSelectedIndex;
}

VOID AcUIList::OnWindowRender( void )
{
	if( m_lSelectedItemTextureID && m_lSelectedItemTextureID == m_stRenderInfo.lRenderID )
	{
		m_stRenderInfo.lRenderID = 0;
	}

	AcUIBase::OnWindowRender();
}


