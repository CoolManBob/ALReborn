#include "AcUIGrid.h"
#include "AgcmUIControl.h"
#include "AcUIScroll.h"
#include "AuMath.h"
#include "ApMemoryTracker.h"

#define	ACUIGRID_ITEM_BASE_CONTROL_ID							1000000

My2DVertex AcUIGridItem::m_vAlphaFan[ 7 ] = 
{
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0.5f, 0.5f }
};

AcUIGrid::AcUIGrid( void )
{
	m_nType						= TYPE_GRID;
	m_pAgpdGrid					= NULL;
	m_apGridItemWindow			= NULL;

	m_lGridItemStart_x			= 0;
	m_lGridItemStart_y			= 0;
	m_lGridItemGap_x			= 0;
	m_lGridItemGap_y			= 0;

	m_lGridItemWindowNum		= 0;
	m_lNowLayer					= 0;
	m_lGridItemWidth			= 0;
	m_lGridItemHeight			= 0;

	m_bDragDropMessageExist		= FALSE;
	m_bDrawAreas				= FALSE;
	m_clProperty.bRenderBlank	= FALSE;
	m_bMoveItemCopy				= FALSE;
	m_bGridItemMovable			= TRUE;
	m_bDrawImageForeground		= FALSE;

	m_lItemToolTipX				= 0;
	m_lItemToolTipY				= 0;
	m_lItemToolTipID			= AP_INVALID_IID;
	m_pToolTipAgpdGridItem		= NULL;
	
	m_bGridItemBottomCountWrite	= TRUE;
	m_lReusableDisplayTextureID	= 0;

	m_pcsGridScroll				= NULL;
	m_lGridItemStartRow			= 0;
	m_lGridItemVisibleRow		= 0;
}

AcUIGrid::~AcUIGrid( void )
{
	DeleteAllChild();
}

/*****************************************************************
*   Function : SetAgpdGridPointer
*   Comment  : SetAgpdGridPointer
*   Date&Time : 2003-09-16, 오후 3:54
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetAgpdGridPointer( AgpdGrid* pAgpdGrid )
{
	DeleteAllChild();

	m_pAgpdGrid = pAgpdGrid;
	if( m_pAgpdGrid )
	{
		m_lGridItemWindowNum = m_pAgpdGrid->m_nRow * m_pAgpdGrid->m_nColumn ;

		SetGridItemMemory();
		AddGridItemWindow();
		UpdateUIGrid();
	}
}

/*****************************************************************
*   Function : SetGridItemDrawInfo
*   Comment  : SetGridItemDrawInfo
*   Date&Time : 2003-09-17, 오후 5:22
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetGridItemDrawInfo( INT32 lStartX, INT32 lStartY, INT32 lGapX, INT32 lGapY, INT32 lGridItemWidth, INT32 lGridItemHeight )
{
	m_lGridItemStart_x	= lStartX;
	m_lGridItemStart_y	= lStartY;
	m_lGridItemGap_x	= lGapX;
	m_lGridItemGap_y	= lGapY;
	m_lGridItemWidth	= lGridItemWidth;
	m_lGridItemHeight	= lGridItemHeight;

	if( !m_pAgpdGrid ) return;
	MoveGridItemWindow();
}

/*****************************************************************
*   Function : MoveGridItemWindow
*   Comment  : Grid Item Draw Info를 가지고 배열한다
*   Date&Time : 2003-09-17, 오후 8:41
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::MoveGridItemWindow( void )
{
	if( !m_pAgpdGrid || !m_apGridItemWindow ) return;

	INT32 lIndex = 0;
	INT32 lY;
	BOOL bVisibleRow;
	m_lGridItemVisibleRow = 0;

	for( INT32 nRowCount = 0 ; nRowCount < m_pAgpdGrid->m_nRow ; ++nRowCount )
	{
		bVisibleRow = FALSE;
		for( INT32 nColCount = 0 ; nColCount < m_pAgpdGrid->m_nColumn ; ++nColCount )
		{
			lIndex = ( nRowCount * m_pAgpdGrid->m_nColumn ) + nColCount;
			if( !m_apGridItemWindow[ lIndex ] )	continue;

			lY = m_lGridItemStart_y + ( nRowCount - m_lGridItemStartRow ) * ( m_lGridItemGap_y + m_lGridItemHeight );
			m_apGridItemWindow[ lIndex ]->MoveWindow( m_lGridItemStart_x + nColCount * ( m_lGridItemGap_x + m_lGridItemWidth ), 
														lY, m_apGridItemWindow[ lIndex ]->w, m_apGridItemWindow[ lIndex ]->h );

			if( nRowCount < m_lGridItemStartRow || lY + m_lGridItemHeight > h )
			{
				m_apGridItemWindow[ lIndex ]->m_Property.bVisible = FALSE;
			}
			else
			{
				m_apGridItemWindow[ lIndex ]->m_Property.bVisible = TRUE;
				bVisibleRow = TRUE;
			}
		}

		if( bVisibleRow )
		{
			++m_lGridItemVisibleRow;
		}
	}
}

/*****************************************************************
*   Function : UpdateUIGrid
*   Comment  : UpdateUIGrid
*   Date&Time : 2003-09-16, 오후 3:54
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::UpdateUIGrid( void )
{
	// 원래 위치로 돌려준다 
	MoveGridItemWindow();
	SetGridItemTextureAndInfo();	
}

/*****************************************************************
*   Function : OnClose
*   Comment  : OnClose
*   Date&Time : 2003-09-16, 오후 3:54
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::OnClose( void )
{
	AcUIBase::OnClose();
	DeleteAllChild();
}

BOOL AcUIGrid::OnPostInit( void )
{
	// 마고자 그리드 스타트 초기화.
	m_lGridItemStartRow = 0;
	if( m_pcsGridScroll )
	{
		m_pcsGridScroll->SetScrollValue( 0 );
	}

	return TRUE;
}


/*****************************************************************
*   Function : DeleteAllChild
*   Comment  : DeleteAllChild
*   Date&Time : 2003-09-16, 오후 3:54
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::DeleteAllChild( void )
{
	if( m_apGridItemWindow )
	{
		for( INT32 lIndex = 0; lIndex < m_lGridItemWindowNum; ++lIndex )
		{
			if( m_apGridItemWindow[ lIndex ]->pParent )
			{
				DeleteChild( m_apGridItemWindow[ lIndex ], TRUE, TRUE );
			}
			else
			{
				delete m_apGridItemWindow[ lIndex ];
			}
		}

		delete[] m_apGridItemWindow;
		m_apGridItemWindow = NULL;
	}

	m_lGridItemWindowNum = 0;
	if( m_clCopyGridItem.pParent )
	{
		DeleteChild( &m_clCopyGridItem );
	}
}

/*****************************************************************
*   Function : SetGridItemMemory
*   Comment  : SetGridItemMemory
*   Date&Time : 2003-09-16, 오후 3:54
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGrid::SetGridItemMemory( void )
{
	if( m_lGridItemWindowNum )
	{
		m_apGridItemWindow = new AcUIGridItem * [ m_lGridItemWindowNum ];
		memset( m_apGridItemWindow, 0, sizeof( AcUIGridItem* ) * m_lGridItemWindowNum );

		for( INT32 lIndex = 0; lIndex < m_lGridItemWindowNum; ++lIndex )
		{
			m_apGridItemWindow[ lIndex ] = new AcUIGridItem;
		}

		// Grid Item Movable 여부 Setting 
		for( INT32 nGridItemCount = 0 ; nGridItemCount < m_lGridItemWindowNum ; ++nGridItemCount )
		{
			m_apGridItemWindow[ nGridItemCount ]->m_Property.bMovable	= m_bGridItemMovable;
		}
	}

	if( !m_apGridItemWindow ) return FALSE;
	else return TRUE;
}

/*****************************************************************
*   Function : AddGridItemWindow
*   Comment  : AddGridItemWindow
*   Date&Time : 2003-09-17, 오후 8:32
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::AddGridItemWindow( void )
{
	if( !m_apGridItemWindow || !m_pAgpdGrid ) return;

	for( INT32 nGridItemCount = 0 ; nGridItemCount < m_lGridItemWindowNum ; ++nGridItemCount )
	{
		if( !m_apGridItemWindow[ nGridItemCount ]->pParent)
		{
			AddChild( ( m_apGridItemWindow[ nGridItemCount ] ), ACUIGRID_ITEM_BASE_CONTROL_ID + nGridItemCount );
			m_apGridItemWindow[ nGridItemCount ]->m_pParentGrid = this;
			m_apGridItemWindow[ nGridItemCount ]->m_Property.bMovable = TRUE;
		}
	}

	m_clCopyGridItem.m_Property.bMovable = FALSE;
	m_clCopyGridItem.MoveWindow( 0, 0, 0, 0 );

	if( !m_clCopyGridItem.pParent )
	{
		AddChild( &m_clCopyGridItem );
	}
}

/*****************************************************************
*   Function : GetIndex
*   Comment  : Row, Column으로 
*   Date&Time : 2003-09-17, 오후 8:04
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
INT32 AcUIGrid::GetIndex( INT32 lRow, INT32 lColumn )
{
	// Valid Check
	if( !m_pAgpdGrid ) return -1;
	else if( lRow >= m_pAgpdGrid->m_nRow || lRow < 0 ) return -1;
	else if( lColumn >= m_pAgpdGrid->m_nColumn || lColumn < 0 )	return -1;
	
	// Return 
	INT32 lIndex = 0;
	lIndex += ( lRow * m_pAgpdGrid->m_nColumn );
	lIndex += lColumn;

	return lIndex;	
}

/*****************************************************************
*   Function : GetGridItemWindow
*   Comment  : Layer, Row, Column 에 맞는 Window를 Return 받는다 
*   Date&Time : 2003-09-17, 오후 8:27
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
AcUIGridItem* AcUIGrid::GetGridItemWindow( INT32 lRow, INT32 lColumn )
{
	INT32 lIndex = GetIndex( lRow, lColumn );
	
	// Valid Check 
	if( lIndex < 0 ) return NULL;
	else if( lIndex >= m_pAgpdGrid->m_lGridCount ) return NULL;
	else if( NULL == m_apGridItemWindow ) return NULL;

	// Return 
	return ( m_apGridItemWindow[ lIndex ] );
}

AcUIGridItem* AcUIGrid::GetGridItemWindowByPos( INT32 x, INT32 y )
{
	if( m_apGridItemWindow && m_lGridItemWindowNum )
	{
		for( INT32 nGridItemCount = 0 ; nGridItemCount < m_lGridItemWindowNum ; ++nGridItemCount )
		{
			if( m_apGridItemWindow[ nGridItemCount ]->HitTest( x, y ) )
			{
				return m_apGridItemWindow[ nGridItemCount ];
			}
		}
	}

	return NULL;
}

/*****************************************************************
*   Function : SetGridItemTexture
*   Comment  : SetGridItemTexture
*   Date&Time : 2003-09-17, 오후 8:51
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetGridItemTextureAndInfo( void )
{
	if( !m_pAgcmUIControl || !m_pAgpdGrid || !m_apGridItemWindow ) return;

	RwTexture* pTexture	= NULL;
	INT32 lIndex = 0;
	AgpdGridItem* pGridItem	= NULL;

	for( INT32 nRowCount = 0 ; nRowCount < m_pAgpdGrid->m_nRow ; ++nRowCount )
	{
		for( INT32 nColCount = 0 ; nColCount < m_pAgpdGrid->m_nColumn ; ++nColCount )
		{
			if( m_apGridItemWindow[ lIndex ] )
			{
				lIndex = GetIndex( nRowCount , nColCount );
				pTexture = m_pAgcmUIControl->GetGridItemTexture( m_pAgpdGrid, m_lNowLayer, nRowCount, nColCount );

				m_apGridItemWindow[ lIndex ]->SetTexture( pTexture );
				m_apGridItemWindow[ lIndex ]->MoveWindow( m_apGridItemWindow[ lIndex ]->x,
					m_apGridItemWindow[ lIndex ]->y, m_lGridItemWidth, m_lGridItemHeight );

				pGridItem = m_pAgcmUIControl->GetGridItemPointer( m_pAgpdGrid, m_lNowLayer, nRowCount, nColCount );
				if( pGridItem )
				{
					m_apGridItemWindow[ lIndex ]->m_ppdGridItem	= pGridItem;
					m_apGridItemWindow[ lIndex ]->m_Property.bMovable = m_bGridItemMovable && pGridItem->m_bMoveable;	
				}
				else 
				{
					m_apGridItemWindow[ lIndex ]->m_ppdGridItem = NULL;
					m_apGridItemWindow[ lIndex ]->m_Property.bMovable = FALSE;
				}
			}
		}
	}
}

/*****************************************************************
*   Function : SetNowLayer
*   Comment  : Setting Now Layer 
*   Date&Time : 2003-09-17, 오후 9:53
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetNowLayer( INT32 lNowLayer )
{
	m_lNowLayer = lNowLayer;	
	UpdateUIGrid();
}

/*****************************************************************
*   Function : OnEndDrag
*   Comment  : OnEndDrag
*   Date&Time : 2003-09-18, 오후 9:00
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGrid::OnDragDrop( PVOID pParam1, PVOID pParam2 )
{
	if( !m_pAgpdGrid ) return TRUE;

	// Status 검사 
	AcUIGridItem* pGridItem	= ( AcUIGridItem* )pParam2;

	if( !pGridItem || !pGridItem->m_ppdGridItem ) return FALSE;
	if( !( m_pAgpdGrid->m_lGridType & pGridItem->m_ppdGridItem->m_eType ) ) return TRUE;
	if( !pGridItem->m_ppdGridItem )	return TRUE;

	// 처리 
	RsMouseStatus clMouseStatus	= *( ( RsMouseStatus* )pParam1 );
	ScreenToClient( clMouseStatus );

	// Row Column에 들어오지 않았으므로 
	if( !GetGridRowColumn( &clMouseStatus, &( m_stDragDropMessageInfo.lGridRow ), &( m_stDragDropMessageInfo.lGridColumn ) ) ) return TRUE;

	m_stDragDropMessageInfo.pGridItem = pGridItem->m_ppdGridItem;
	m_bDragDropMessageExist	= TRUE;

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_DRAG_DROP, ( PVOID )&m_lControlID );
	}

	return TRUE;
}

/*****************************************************************
*   Function : GetGridRowColumn
*   Comment  : GetGridRowColumn
*   Date&Time : 2003-09-19, 오후 4:09
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGrid::GetGridRowColumn( RsMouseStatus* ms, INT32* lRow, INT32* lColumn )
{
	return GetGridRowColumn( ( INT32 )ms->pos.x, ( INT32 )ms->pos.y, lRow, lColumn );
}

BOOL AcUIGrid::GetGridRowColumn( INT32 lX, INT32 lY, INT32* lRow, INT32* lColumn )
{
	INT32 lTempRow = -1;
	INT32 lTempColumn =	-1;
	
	for( INT32 nColCount = 0 ; nColCount < m_pAgpdGrid->m_nColumn ; ++nColCount )
	{
		if( ( m_lGridItemStart_x + nColCount * ( m_lGridItemGap_x + m_lGridItemWidth ) ) <= lX &&
			( m_lGridItemStart_x + nColCount * ( m_lGridItemGap_x + m_lGridItemWidth ) + m_lGridItemWidth ) >= lX )
		{
			lTempColumn = nColCount ;
			break;
		}
	}

	for( INT32 nRowCount = m_lGridItemStartRow ; nRowCount < m_pAgpdGrid->m_nRow ; ++nRowCount )
	{
		if( ( m_lGridItemStart_y + ( nRowCount - m_lGridItemStartRow ) * ( m_lGridItemGap_y + m_lGridItemHeight ) ) <= lY &&
			( m_lGridItemStart_y + ( nRowCount - m_lGridItemStartRow ) * ( m_lGridItemGap_y + m_lGridItemHeight ) + m_lGridItemHeight ) >= lY )
		{
			lTempRow = nRowCount;
			break;
		}
	}
		
	if( lTempRow == -1 || lTempColumn == -1 ) return FALSE;

	*lRow = lTempRow;
	*lColumn = lTempColumn;
	return TRUE;
}

BOOL AcUIGrid::GetGridRowColumn( INT32 lIndex, INT32* lRow, INT32* lColumn )
{
	if( !m_pAgpdGrid ) return FALSE;

	*lRow = ( INT32 )( lIndex / m_pAgpdGrid->m_nColumn );
	*lColumn = ( INT32 )( lIndex % m_pAgpdGrid->m_nColumn );
	return TRUE;
}

/*****************************************************************
*   Function : GetDragDropMessageInfo
*   Comment  : GetDragDropMessageInfo 한번만 Return해준다 
*   Date&Time : 2003-09-19, 오후 5:49
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
AgpdGridSelectInfo*	AcUIGrid::GetDragDropMessageInfo( void )
{
	if( m_bDragDropMessageExist )
	{
		m_bDragDropMessageExist = FALSE;		
		return &m_stDragDropMessageInfo;
	}

	return NULL;
}

/*****************************************************************
*   Function : GetGridItemClickInfo
*   Comment  : GetGridItemClickInfo
*   Date&Time : 2003-10-10, 오후 5:37
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
AgpdGridSelectInfo* AcUIGrid::GetGridItemClickInfo( void )
{
	if( m_bGridItemClickInfoExist )
	{
		m_bGridItemClickInfoExist = FALSE;
		return &m_stGridItemClickInfo;
	}

	return NULL;
}

void AcUIGrid::OnWindowRender( void )
{
	PROFILE( "AcUIGrid::OnWindowRender" );
	if( !m_bDrawImageForeground )
	{
		AcUIBase::OnWindowRender();
	}

	//2003.11.25 이종석과장님께서 수정.
	// 엥... ㅡ.ㅡ.. 여기서 리턴할거면서 아래의 코드는 왜 남겨두셨...;;;
	// 아래의 코드는 그리드 영역을 반투명 흰색으로 칠해준다.
	return;

	INT32 lStartX = m_lGridItemStart_x;
	INT32 lStartY = m_lGridItemStart_y;
	INT32 lX;
	INT32 lY;

	ClientToScreen( &lStartX, &lStartY );

	for( lX = 0; lX < m_pAgpdGrid->m_nColumn; ++lX )
	{
		for( lY = 0; lY < m_pAgpdGrid->m_nRow; ++lY )
		{
			g_pEngine->DrawIm2D( NULL, ( FLOAT )( lStartX + lX * ( m_lGridItemGap_x + m_lGridItemWidth ) ),
						   ( FLOAT )( lStartY + lY * ( m_lGridItemGap_y + m_lGridItemHeight ) ),
						   ( FLOAT )m_lGridItemWidth, ( FLOAT )m_lGridItemHeight, 0.0f, 0.0f, 1.0f, 1.0f,
						   m_dwAreaColor, ( UINT8 )( ( float )m_ucAreaAlpha * ( m_pfAlpha ? *m_pfAlpha : 1) ) );
		}
	}
}

/*****************************************************************
*   Function : SetCopyGridItem
*   Comment  : SetCopyGridItem
*   Date&Time : 2003-10-06, 오후 5:14
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetCopyGridItem( INT32 lGridItemIndex )
{
	if( !m_apGridItemWindow || lGridItemIndex >= m_lGridItemWindowNum ) return;

	RwTexture* pTexture = m_apGridItemWindow[ lGridItemIndex ]->GetTexturePointer();
	if( !pTexture ) return;

	m_clCopyGridItem.SetTexture( pTexture );
	m_clCopyGridItem.MoveWindow( m_apGridItemWindow[ lGridItemIndex ]->x, m_apGridItemWindow[ lGridItemIndex ]->y,
									m_apGridItemWindow[ lGridItemIndex ]->w, m_apGridItemWindow[ lGridItemIndex ]->h );
}

/*****************************************************************
*   Function : ReleaseCopyGridItem
*   Comment  : ReleaseCopyGridItem
*   Date&Time : 2003-10-06, 오후 5:14
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::ReleaseCopyGridItem( void )
{
	m_clCopyGridItem.SetTexture( NULL );
	m_clCopyGridItem.MoveWindow( 0, 0, 0, 0 );
}

/*****************************************************************
*   Function : OnCommand
*   Comment  : Command virtual function overriding 
*   Date&Time : 2003-10-06, 오후 5:34
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGrid::OnCommand( INT32	nID, PVOID pParam )
{
	if( m_pAgpdGrid && m_pcsGridScroll && *( INT32* )pParam == m_pcsGridScroll->m_lControlID )
	{
		// Scroll이 움직였다.
		switch( nID )
		{
		case UICM_SCROLL_CHANGE_SCROLLVALUE :
			{
				FLOAT fValue = m_pcsGridScroll->GetScrollValue();

				m_lGridItemStartRow = ( INT32 )( ( m_pAgpdGrid->m_nMaxRowExistItem + 1 - m_lGridItemVisibleRow ) * fValue );
				if( m_lGridItemStartRow < 0 )
				{
					m_lGridItemStartRow = 0;
				}

				MoveGridItemWindow();
			}
			return AcUIBase::OnCommand( nID, pParam );

		case UICM_BASE_MESSAGE_CLOSE :
			{
				SetScroll( NULL );
			}
			return AcUIBase::OnCommand( nID, pParam );
		}
	}
	else if( pParam )
	{
		AgcWindow* pcsWindow = *( AgcWindow** )pParam;
		AcUIBase* pcsBase = ( AcUIBase* )pcsWindow;

		if( pcsWindow && pcsWindow->m_bUIWindow )
		{
			AcUIGridItem* pcsGridItem = ( AcUIGridItem* )pcsBase;
			INT32 lGridItemID;
			
			for( lGridItemID = 0 ; lGridItemID < m_lGridItemWindowNum ; ++lGridItemID )
			{
				if( pcsGridItem == m_apGridItemWindow[ lGridItemID ] ) break;
			}

			if( lGridItemID < m_lGridItemWindowNum )
			{
				switch( nID )
				{
				case UICM_GRIDITEM_DRAG_START :
					{
						if( m_bMoveItemCopy )
						{
							SetCopyGridItem( lGridItemID  );
						}
						
						// DRAG_START 시점에서 Click Info 추가 
						if( m_pAgpdGrid->m_nColumn > 0 )
						{
							m_stGridItemClickInfo.lGridRow = ( INT32 )( ( INT32 )lGridItemID / m_pAgpdGrid->m_nColumn );
							m_stGridItemClickInfo.lGridColumn = ( INT32 )( ( INT32 )lGridItemID % m_pAgpdGrid->m_nColumn );
							m_stGridItemClickInfo.pGridItem = m_apGridItemWindow[ lGridItemID ]->m_ppdGridItem;
							m_bGridItemClickInfoExist = TRUE;
						
							if( pParent )
							{
								pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_DRAG_START, ( PVOID )&m_lControlID );
							}
						}
					}
					break;

				case UICM_GRIDITEM_DRAG_END :
					{
						UpdateUIGrid();
						if( m_bMoveItemCopy )
						{
							ReleaseCopyGridItem();
						}

						// DRAG_END 시점에서 Click Info 추가
						if( m_pAgpdGrid->m_nColumn )
						{
							m_stGridItemClickInfo.lGridRow = ( INT32 )( ( INT32 )lGridItemID / m_pAgpdGrid->m_nColumn );
							m_stGridItemClickInfo.lGridColumn = ( INT32 )( ( INT32 )lGridItemID % m_pAgpdGrid->m_nColumn );
							m_stGridItemClickInfo.pGridItem	= m_apGridItemWindow[ lGridItemID ]->m_ppdGridItem;
							m_bGridItemClickInfoExist = TRUE;

							if( pParent )
							{
								pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_DRAG_END, ( PVOID )&m_lControlID );
							}
						}
					}
					break;

				case UICM_GRIDITEM_SET_FOCUS :
					{
						if( !m_apGridItemWindow[ lGridItemID ]->m_ppdGridItem )	return TRUE;

						m_lItemToolTipID = m_apGridItemWindow[ lGridItemID ]->m_ppdGridItem->m_lItemID;
						m_lItemToolTipX	= m_apGridItemWindow[ lGridItemID ]->x + m_lGridItemWidth + 5;
						m_lItemToolTipY	= m_apGridItemWindow[ lGridItemID ]->y + m_lGridItemHeight + 5;
						m_pToolTipAgpdGridItem = m_apGridItemWindow[ lGridItemID ]->m_ppdGridItem;
						
						ClientToScreen( &m_lItemToolTipX, &m_lItemToolTipY );

						if( pParent )
						{
							pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_GRIDITEM_SET_FOCUS, ( PVOID )&m_lControlID );
						}
					}
					break;

				case UICM_GRIDITEM_KILL_FOCUS :
					{
						m_lItemToolTipID = AP_INVALID_IID;
						m_lItemToolTipX = 0;
						m_lItemToolTipY = 0;
						m_pToolTipAgpdGridItem = NULL;

						if( pParent )
						{
							pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_GRIDITEM_KILL_FOCUS, ( PVOID )&m_lControlID );
						}
					}
					break;

				case UICM_BASE_MESSAGE_LDBLCLICK :
					{
						if( m_pAgpdGrid->m_nColumn > 0 )
						{
							m_stGridItemClickInfo.lGridRow = ( INT32 )( ( INT32 )lGridItemID / m_pAgpdGrid->m_nColumn );
							m_stGridItemClickInfo.lGridColumn = ( INT32 )( ( INT32 )lGridItemID % m_pAgpdGrid->m_nColumn );
							m_stGridItemClickInfo.pGridItem = m_apGridItemWindow[ lGridItemID ]->m_ppdGridItem;
							m_bGridItemClickInfoExist = TRUE;
							
							if( pParent )
							{
								pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BASE_MESSAGE_LDBLCLICK, ( PVOID )&m_lControlID );
							}
						}
					}
					break;
				}
			}
		}
	}

	return AcUIBase::OnCommand( nID, pParam );
}

BOOL AcUIGrid::OnLButtonUp( RsMouseStatus *ms )
{
	return AcUIBase::OnLButtonUp( ms );
}

BOOL AcUIGrid::OnLButtonDown( RsMouseStatus *ms )
{
	if( AcUIBase::OnLButtonDown( ms ) != FALSE ) return TRUE;
	if( !m_pAgpdGrid ) return FALSE;
	
	INT32 lRow = 0;
	INT32 lColumn = 0;

	if( !GetGridRowColumn( ms, &lRow, &lColumn ) ) return FALSE;

	AcUIGridItem* pGridItem = ( AcUIGridItem* )GetGridItemWindow( lRow, lColumn );
	
	// 존재하는 GridItem 을 선택한 것인가
	if( !pGridItem || !pGridItem->m_ppdGridItem	) return FALSE;
	
	m_stGridItemClickInfo.lGridRow = lRow;
	m_stGridItemClickInfo.lGridColumn = lColumn;
	m_stGridItemClickInfo.pGridItem = pGridItem->m_ppdGridItem;
	m_bGridItemClickInfoExist =	TRUE;

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_LBUTTON_ITEM_CLICK, ( PVOID )&m_lControlID );
	}

	return TRUE;
}

BOOL AcUIGrid::OnRButtonUp( RsMouseStatus *ms )
{
	if( !m_pAgpdGrid ) return FALSE;

	INT32 lRow = 0;
	INT32 lColumn = 0;

	if( !GetGridRowColumn( ms, &lRow, &lColumn ) ) return FALSE;

	AcUIGridItem* pGridItem = ( AcUIGridItem* )GetGridItemWindow( lRow, lColumn );	
	
	// 존재하는 GridItem 을 선택한 것인가
	if( !pGridItem || !pGridItem->m_ppdGridItem	) return FALSE;
	
	m_stGridItemClickInfo.lGridRow = lRow;
	m_stGridItemClickInfo.lGridColumn = lColumn;
	m_stGridItemClickInfo.pGridItem = pGridItem->m_ppdGridItem;
	m_bGridItemClickInfoExist =	TRUE;

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRID_RBUTTON_ITEM_CLICK, ( PVOID )&m_lControlID );
		return FALSE;
	}

	return FALSE;
}

BOOL AcUIGrid::OnRButtonDown( RsMouseStatus *ms )
{
	return FALSE;	
}

/*****************************************************************
*   Function : SetClickInfoTargetWindow
*   Comment  : SetClickInfoTargetWindow
*   Date&Time : 2003-10-13, 오후 6:01
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetClickInfoTargetWindow( AgcWindow* pWindow, INT32 lX, INT32 lY )
{
	m_stGridItemClickInfo.pTargetWindow = pWindow;
	m_stGridItemClickInfo.lX = lX;
	m_stGridItemClickInfo.lY = lY;
}

BOOL AcUIGrid::SetReusableDisplayImage( INT32 lImageID )
{
	m_lReusableDisplayTextureID	= lImageID;
	OnAddImage( m_csTextureList.GetImage_ID( lImageID ) );
	return TRUE;
}

BOOL AcUIGrid::GetReusableDisplayImage( void )
{
	return m_lReusableDisplayTextureID;
}

VOID AcUIGrid::SetScroll( AcUIScroll *pcsScroll )
{
	m_pcsGridScroll = pcsScroll;
}

/*****************************************************************
*   Function : SetGridItemMovable
*   Comment  : Grid Item의 Movabel여부를 Setting한다 
*				Grid Item Window 생성여부와 상관없이 어느때나 호출 가능 
*   Date&Time : 2003-11-05, 오전 10:47
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGrid::SetGridItemMovable( BOOL bMovable )
{
	m_bGridItemMovable = bMovable;
	if( m_apGridItemWindow && m_lGridItemWindowNum ) // Grid Item Window가 이미 생성되어 있다면 
	{
		for( INT32 nGridItemCount = 0 ; nGridItemCount < m_lGridItemWindowNum ; ++nGridItemCount )
		{
			m_apGridItemWindow[ nGridItemCount ]->m_Property.bMovable = m_bGridItemMovable; 
		}
	}
}

BOOL AcUIGrid::OnMouseWheel( INT32 lDelta )
{
	if( m_pcsGridScroll && m_pAgpdGrid )
	{
		if( lDelta > 0 ) --m_lGridItemStartRow;
		else ++m_lGridItemStartRow;

		if( m_lGridItemStartRow < 0 )
		{
			m_lGridItemStartRow = 0;
		}
		else if( m_lGridItemStartRow > m_pAgpdGrid->m_nMaxRowExistItem + 1 - m_lGridItemVisibleRow )
		{
			m_lGridItemStartRow = m_pAgpdGrid->m_nMaxRowExistItem + 1 - m_lGridItemVisibleRow;
			if( m_lGridItemStartRow < 0 )
			{
				m_lGridItemStartRow = 0;
			}
		}

		if( m_pAgpdGrid->m_nMaxRowExistItem + 1 == m_lGridItemVisibleRow )
		{
			m_pcsGridScroll->SetScrollValue( 0 );
		}
		else
		{
			m_pcsGridScroll->SetScrollValue( m_lGridItemStartRow / ( m_pAgpdGrid->m_nMaxRowExistItem + 1 - m_lGridItemVisibleRow + 0.0f ) );
		}

		MoveGridItemWindow();
	}
	else
		return FALSE;

	return TRUE;
}

/*****************************************************************
*   Function : OnLButtonUp
*   Comment  : OnLButtonUp
*   Date&Time : 2003-09-18, 오후 8:38
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGridItem::OnLButtonUp( RsMouseStatus *ms )
{
	if( m_bExistHandler )
	{
		if( m_bShrinkMode || CheckInHandlerArea( ms ) ) return OnLButtonUpForHandler( ms );
	}
	else 
	{
		if( m_bMoveMovableWindow )
		{
			MovableOff();

			// Drag중이었다 EndDragMessage를 날린다
			int lAbsoluteX = 0;
			int lAbsoluteY = 0;
			ClientToScreen( &lAbsoluteX, &lAbsoluteY );

			RsMouseStatus clAbsolutePos = *ms;
			clAbsolutePos.pos.x += lAbsoluteX;
			clAbsolutePos.pos.y += lAbsoluteY;

			// this가 안보여야 마우스 포인터가 가리키는 Window를 얻어낼수 있다 
			m_Property.bVisible = FALSE;
			AgcWindow* pWindow = g_pEngine->WindowFromPoint( ( INT32 )ms->pos.x + lAbsoluteX, ( INT32 )ms->pos.y + lAbsoluteY );			
			m_Property.bVisible = TRUE;
				
			// Target Window설정 
			if( m_pParentGrid && pWindow )
			{
				INT32 lX = ( INT32 )ms->pos.x + lAbsoluteX;
				INT32 lY = ( INT32 )ms->pos.y + lAbsoluteY;

				m_pParentGrid->ScreenToClient( &lX, &lY );
				m_pParentGrid->SetClickInfoTargetWindow( pWindow, lX, lY );
			}

			// MESSAGE 처리 
			if( pWindow )
			{
				pWindow->SendMessage( MESSAGE_DRAG_DROP, ( PVOID )&clAbsolutePos, ( PVOID )this );
			}

			if( pParent )
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRIDITEM_DRAG_END, ( PVOID )&m_lControlID );
			}

			return FALSE;
		}
	}

	return FALSE;
}

/*****************************************************************
*   Function : OnLButtonDown
*   Comment  : OnLButtonDown
*   Date&Time : 2003-10-06, 오후 5:00
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGridItem::OnLButtonDown( RsMouseStatus *ms )
{
	// Movable이면 Move시작
	if( m_Property.bMovable && m_ppdGridItem && m_ppdGridItem->m_bMoveable )
	{
		if( m_pParentGrid && !m_pParentGrid->m_bGridItemMovable ) return FALSE;

		MovableOn( ms );
		if( pParent )
		{
			pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRIDITEM_DRAG_START, ( PVOID )&m_lControlID );
		}

		// TRUE에서 FALSE루 바꿨다... 왜냐면... AcUIGrid에서 받아야 되니깐.. 일케 하니깐 이상해서 다시 TRUE로 바꿨다.
		return TRUE;
	}
	
	return FALSE;
}

/*****************************************************************
*   Function : OnEndDrag
*   Comment  : OnEndDrag
*   Date&Time : 2003-09-18, 오후 8:50
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUIGridItem::OnDragDrop( PVOID pParam1, PVOID pParam2 )
{
	if( pParent )
	{
		pParent->SendMessage( MESSAGE_DRAG_DROP, pParam1, pParam2 );
	}

	return FALSE;
}

/*****************************************************************
*   Function : OnWindowRender
*   Comment  : OnWindowRender
*   Date&Time : 2003-10-16, 오후 4:43
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIGridItem::OnWindowRender( void )
{
	PROFILE( "AcUIGridItem::OnWindowRender" );
	if( !g_pEngine ) return;

	int lAbsoluteX = 0;
	int lAbsoluteY = 0;
	ClientToScreen( &lAbsoluteX, &lAbsoluteY );

	UINT8 nAlpha = ( UINT8 )( ( float )m_cAlphaValue * ( m_pfAlpha ? *m_pfAlpha : 1.0f ) );
	
	// Twinkle ( 깜빡이는 모드인가 체크 )
	if( m_ppdGridItem && m_ppdGridItem->m_bTwinkleMode )
	{
		// Twinkle중 Render 되지 않아야 할때 
		if( ACUIGRID_TWINKLE_TIMEGAP < g_pEngine->GetPrevClockCount() % ( ACUIGRID_TWINKLE_TIMEGAP * 2 ) ) return;
	}

	if( m_pTexture && m_pTexture->raster )
	{
		g_pEngine->DrawIm2D( m_pTexture, ( float )lAbsoluteX, ( float )lAbsoluteY, w, h,
							//( float )( RwRasterGetWidth( m_pTexture->raster ) ),
							//( float )( RwRasterGetHeight( m_pTexture->raster ) ),
							0.0f, 0.0f, 1.0f, 1.0f, m_lColorValue, nAlpha );

		if( m_pParentGrid && m_pParentGrid->m_nType == TYPE_GRID )
		{
			RwTexture* pTexture	= m_pParentGrid->m_csTextureList.GetImage_ID( m_pParentGrid->GetReusableDisplayImage() );
			if( m_ppdGridItem->m_lItemTID == 1054 && m_ppdGridItem->m_ulRemainTime )
			{
				if( pTexture )
				{
					DrawCoolTime( lAbsoluteX, lAbsoluteY, nAlpha, pTexture, (float)(m_ppdGridItem->m_ulReuseIntervalTime - m_ppdGridItem->m_ulRemainTime) / (float)m_ppdGridItem->m_ulReuseIntervalTime );
				}
				else if( m_ppdGridItem->m_pcsTemplateGrid )
				{
					RwTexture* pTexture = m_pParentGrid->m_pAgcmUIControl->GetGridItemTexture( m_ppdGridItem->m_pcsTemplateGrid );
					if( pTexture && !IsBadReadPtr( pTexture, sizeof( RwTexture ) ) )
					{
						INT32 lY = ( INT32 )( h * (float)(m_ppdGridItem->m_ulReuseIntervalTime - m_ppdGridItem->m_ulRemainTime) / (float)m_ppdGridItem->m_ulReuseIntervalTime );
						g_pEngine->DrawIm2DPixel( pTexture, lAbsoluteX, lAbsoluteY + lY, w, h - lY, 0, lY, w,	h, m_lColorValue, nAlpha );
					}
				}
			}
			else if( g_pEngine->GetPrevClockCount() >= m_ppdGridItem->m_ulUseItemTime && m_ppdGridItem->m_ulReuseIntervalTime > 0 &&
					g_pEngine->GetPrevClockCount() < m_ppdGridItem->m_ulUseItemTime + m_ppdGridItem->m_ulReuseIntervalTime )
			{
				if( pTexture )
				{
					FLOAT fRemainPercent = 0.0f;
					if( m_ppdGridItem->m_ulPauseClockCount )
						fRemainPercent = ( m_ppdGridItem->m_ulPauseClockCount - m_ppdGridItem->m_ulUseItemTime ) / ( m_ppdGridItem->m_ulReuseIntervalTime + 0.0f );
					else
						fRemainPercent = ( g_pEngine->GetPrevClockCount() - m_ppdGridItem->m_ulUseItemTime ) / ( m_ppdGridItem->m_ulReuseIntervalTime + 0.0f );
				
					DrawCoolTime( lAbsoluteX, lAbsoluteY, nAlpha, pTexture, fRemainPercent );
				}
				else if( m_ppdGridItem->m_pcsTemplateGrid )
				{
					RwTexture* pTexture = m_pParentGrid->m_pAgcmUIControl->GetGridItemTexture( m_ppdGridItem->m_pcsTemplateGrid );
					if( pTexture && !IsBadReadPtr( pTexture, sizeof( RwTexture ) ) )
					{
						FLOAT fRemainPercent = ( FLOAT )( ( g_pEngine->GetPrevClockCount() - m_ppdGridItem->m_ulUseItemTime ) / ( m_ppdGridItem->m_ulReuseIntervalTime + 0.0 ) );
						INT32 lY = ( INT32 )( h * fRemainPercent );

						g_pEngine->DrawIm2DPixel( pTexture, lAbsoluteX, lAbsoluteY + lY, w, h - lY, 0, lY, w,	h, m_lColorValue, nAlpha );
					}
				}
			}
		}
	}

	m_lColor = 0xffffffff;

	// Left Bottom , Right Bottom 쓰기 
	if( m_ppdGridItem && m_pTexture && m_pTexture->raster && ( ( m_ppdGridItem->m_bForceWriteBottom ) || ( m_pParentGrid && m_pParentGrid->m_bGridItemBottomCountWrite ) ) )
	{
		if( m_ppdGridItem->m_bWriteLeftBottom && m_ppdGridItem->m_strLeftBottom )
		{
			DWORD dwColor = ARGB32_TO_DWORD( 255, ( m_lColor >> 16 ) & 0xff, ( m_lColor >> 8 ) & 0xff, ( m_lColor >> 0 ) & 0xff );
			m_pAgcmFont->FontDrawStart( m_lFontType );
			m_pAgcmFont->DrawTextIM2D( ( FLOAT )( lAbsoluteX ), ( FLOAT )( lAbsoluteY + h - 1 ), m_ppdGridItem->m_strLeftBottom, m_lFontType,
				( UINT8 )( ( m_lColor >> 24 ) & 0xff ) * ( m_pfAlpha ? *m_pfAlpha : 1 ), dwColor );
			m_pAgcmFont->FontDrawEnd();
		}

		if( m_ppdGridItem->m_bWriteRightBottom && m_ppdGridItem->m_strRightBottom )
		{
			INT32 lstrLen = strlen( m_ppdGridItem->m_strRightBottom );
			DWORD dwColor = ARGB32_TO_DWORD( 255, ( m_lColor >> 16 ) & 0xff, ( m_lColor >> 8 ) & 0xff, ( m_lColor >> 0 ) & 0xff );
			m_pAgcmFont->FontDrawStart( m_lFontType );
			m_pAgcmFont->DrawTextIM2D( ( FLOAT )( lAbsoluteX + w - 8 * lstrLen ), ( FLOAT )( lAbsoluteY + h - 12 ), m_ppdGridItem->m_strRightBottom, m_lFontType,
				( UINT8 )( ( m_lColor>>24 ) & 0xff ) * ( m_pfAlpha ? *m_pfAlpha : 1 ), dwColor );
			m_pAgcmFont->FontDrawEnd();
		}
	}

	if( m_ppdGridItem && m_pParentGrid && m_pParentGrid->m_bDrawImageForeground && m_pParentGrid->m_pAgpdGrid->m_pcsSelectedGridItem == m_ppdGridItem )
	{
		if( m_pParentGrid->m_csTextureList.GetCount() > 0 )
		{
			RwTexture* pTexture = m_pParentGrid->m_stRenderInfo.lRenderID ? m_pParentGrid->m_csTextureList.GetImage_ID( m_pParentGrid->m_stRenderInfo.lRenderID ) : NULL;
			if( pTexture && pTexture->raster )
			{
				// 그려 보자꾸나 
				if( m_bShrinkMode )
				{
					g_pEngine->DrawIm2D( pTexture, ( FLOAT )lAbsoluteX, ( FLOAT )lAbsoluteY, w, h, 
											m_pParentGrid->m_stRenderInfo.fStartU, 
											m_pParentGrid->m_stRenderInfo.fStartV, 
											m_pParentGrid->m_stRenderInfo.fEndU, 
											m_pParentGrid->m_stRenderInfo.fEndV,
											m_pParentGrid->m_stRenderInfo.lRenderColor,
											( UINT8 )( m_pParentGrid->m_stRenderInfo.cRenderAlpha * ( m_pfAlpha ? *m_pfAlpha : 1 ) )  );
				}
				else 
				{
					g_pEngine->DrawIm2D( pTexture, ( FLOAT )lAbsoluteX, ( FLOAT )lAbsoluteY,
											( FLOAT )( RwRasterGetWidth( pTexture->raster ) ) * m_pParentGrid->m_stRenderInfo.fScale, 
											( FLOAT )( RwRasterGetHeight( pTexture->raster ) ) * m_pParentGrid->m_stRenderInfo.fScale, 
											m_pParentGrid->m_stRenderInfo.fStartU, 
											m_pParentGrid->m_stRenderInfo.fStartV, 
											m_pParentGrid->m_stRenderInfo.fEndU, 
											m_pParentGrid->m_stRenderInfo.fEndV,
											m_pParentGrid->m_stRenderInfo.lRenderColor, 
											( UINT8 )( m_pParentGrid->m_stRenderInfo.cRenderAlpha * ( m_pfAlpha ? *m_pfAlpha : 1 ) ) );
				}
			}
		}
	}

	// 내구도가 0인 아이템은 위에 붉은 텍스쳐를 한장 더 그려준다.
	if( m_pParentGrid && m_ppdGridItem && m_ppdGridItem->IsDisabled() && m_pParentGrid->m_pAgcmUIControl->m_pDurabilityZero )
	{
		UINT8 uAlphaWindow = ( UINT8 )( ( ( m_lColor >> 24 ) & 0xff ) * ( m_pfAlpha ? *m_pfAlpha : 1.0f ) );
		g_pEngine->DrawIm2D( m_pParentGrid->m_pAgcmUIControl->m_pDurabilityZero, ( float )lAbsoluteX, ( float )lAbsoluteY,
								( float )( RwRasterGetWidth( m_pParentGrid->m_pAgcmUIControl->m_pDurabilityZero->raster ) ),
								( float )( RwRasterGetHeight( m_pParentGrid->m_pAgcmUIControl->m_pDurabilityZero->raster ) ),
								0.0f, 0.0f, 1.0f, 1.0f, m_lColor, uAlphaWindow );
	}
}

void AcUIGridItem::OnSetFocus( void )
{
	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRIDITEM_SET_FOCUS, ( PVOID )&m_lControlID );
	}
}

void AcUIGridItem::OnKillFocus( void )
{
	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_GRIDITEM_KILL_FOCUS, ( PVOID )&m_lControlID );
	}
}

BOOL AcUIGridItem::DrawCoolTime( int lAbsoluteX, int lAbsoluteY, UINT8 nAlpha, RwTexture* pTexture, float fPercent )
{
	if( !pTexture )		return FALSE;

	if( m_ppdGridItem->m_bBarMode )
	{
		INT32 lY = ( INT32 )( h * fPercent );
		TRACE( "Grid Reuse : %d , %d , %d\n", g_pEngine->GetPrevClockCount() - m_ppdGridItem->m_ulUseItemTime, lY, ( INT32 )( fPercent * 100 ) );

		// DrawIm2DPixel() 쓰면 간단하게 해결
		g_pEngine->DrawIm2DPixel( pTexture, lAbsoluteX,	lAbsoluteY + lY, w,	h - lY,	0, lY, w, h, m_lColorValue, nAlpha );
	}
	else
	{
		FLOAT fDegree =	AUMATH_PIF * 2 - fPercent * 2 * AUMATH_PIF;
		INT32 lNumTriangle = 1;
		FLOAT fHeight =	h *.5f;
		FLOAT fWidth = w *.5f;
		FLOAT fTemp = atan( w / ( h + 0.0f ) );

		m_vAlphaFan[ 0 ].x = lAbsoluteX + w *.5f;
		m_vAlphaFan[ 0 ].y = lAbsoluteY + h *.5f;

		m_vAlphaFan[ 1 ].x = m_vAlphaFan[ 0 ].x;
		m_vAlphaFan[ 1 ].y = ( float )lAbsoluteY;

		m_vAlphaFan[ 2 ].y = ( float )lAbsoluteY;
		if( fDegree < fTemp )
		{
			m_vAlphaFan[ 2 ].x = lAbsoluteX + fWidth - fHeight * tan( fDegree );
		}
		else
		{
			m_vAlphaFan[ 2 ].x = ( float )lAbsoluteX;

			++lNumTriangle;
			m_vAlphaFan[ 3 ].x = ( float )lAbsoluteX;
			if( fDegree < AUMATH_PI - fTemp )
			{
				if( fDegree == AUMATH_PI / 2.0f )
				{
					m_vAlphaFan[ 3 ].y = lAbsoluteY + fHeight;
				}
				else
				{
					m_vAlphaFan[ 3 ].y = lAbsoluteY + fHeight - fWidth / tan( fDegree );
				}
			}
			else
			{
				m_vAlphaFan[ 3 ].y = ( float )lAbsoluteY + h;

				++lNumTriangle;
				m_vAlphaFan[4].y = ( float )lAbsoluteY + h;
				if( fDegree < AUMATH_PI + fTemp )
				{
					m_vAlphaFan[ 4 ].x = lAbsoluteX + fWidth + fHeight * tan( fDegree );
				}
				else
				{
					m_vAlphaFan[ 4 ].x = ( float )lAbsoluteX + w;

					++lNumTriangle;
					m_vAlphaFan[ 5 ].x = ( float )lAbsoluteX + w;
					if( fDegree < 2.0f * AUMATH_PI - fTemp )
					{
						if( fDegree == AUMATH_PI / 2.0f )
						{
							m_vAlphaFan[ 5 ].y = lAbsoluteY + fHeight;
						}
						else
						{
							m_vAlphaFan[ 5 ].y = lAbsoluteY + fHeight + fWidth / tan( fDegree );
						}
					}
					else
					{
						m_vAlphaFan[ 5 ].y = ( float )lAbsoluteY;

						++lNumTriangle;
						m_vAlphaFan[ 6 ].y = m_vAlphaFan[ 1 ].y;
						m_vAlphaFan[ 6 ].x = ( float )lAbsoluteX + fWidth - fHeight * tan( fDegree );
					}
				}
			}
		}

		// 알파 변경
		DWORD color = m_lColorValue & 0x00ffffff | ( nAlpha << 24 );
		for( int nFanCount = 0 ; nFanCount < 7 ; nFanCount++ )
			m_vAlphaFan[ nFanCount ].color = color;

		RwD3D9SetTexture( pTexture, 0 );
		RwD3D9SetFVF( MY2D_VERTEX_FLAG );
		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN, lNumTriangle, m_vAlphaFan, SIZE_MY2D_VERTEX );
	}

	return TRUE;
}

VOID	AcUIGrid::SetFocus( VOID )
{
	if( m_pAgpdGrid )
	{
		if( m_pAgpdGrid->m_lGridType	==	AGPDGRID_ITEM_TYPE_SKILL )
			return;
	}

	AgcWindow::SetFocus();
}