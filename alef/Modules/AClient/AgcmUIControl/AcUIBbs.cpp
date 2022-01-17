#include "AcUIBbs.h"



extern AgcEngine *	g_pEngine;



/*****************************************************************
*   Function : AcUIBbs	
*   Comment  : AcUIBbs class Constructor
*   Date&Time : 2003-02-26, 오후 2:07
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcUIBbs::AcUIBbs()
{
	m_bInitSetting							= FALSE;
	m_pstCellContent						= NULL;
	m_stBbsInfo.m_cColumnNum				= 0;
	m_stBbsInfo.m_v2dCellStart.x			= 0.0f;
	m_stBbsInfo.m_v2dCellStart.y			= 0.0f;
	m_stBbsInfo.m_nCellHeight				= 0;
	m_stBbsInfo.m_cRowNum					= 0;
	m_stBbsInfo.m_nAllPageNum				= 0;	
	m_stBbsInfo.m_nNowPageNum				= 0;
	m_stBbsInfo.m_nGapColumnCell			= 0;
	m_stBbsInfo.m_nColumnGap				= 0;
	
	m_stCommandMessageInfo.lControlID		= 0;
	m_stCommandMessageInfo.lSelectedColumn	= 0;
	m_stCommandMessageInfo.lSelectedIndex	= 0;
	m_stCommandMessageInfo.lSelectedRow		= 0;

	for( int nColCount = 0 ; nColCount < ACUIBBS_COLUMN_NUM_MAX ; ++nColCount )
	{
		memset( m_stColumnInfo->szColumnName, 0, sizeof( char ) * ACUIBBS_COLUMN_NAME_MAX );
		m_stColumnInfo->nColumnWidth =	0;
		m_stColumnInfo->eArrayType = HANFONT_LEFTARRANGE;	
	}

	for( int nPageCount = 0 ; nPageCount < ACUIBBS_PAGEMANAGER_NUM ; ++nPageCount )
	{
		memset( m_stPageManager[ nPageCount ].szPageIndex, 0, sizeof( char ) * 10 );
		m_stPageManager[ nPageCount ].lPageIndex =	0;
	}
}

/*****************************************************************
*   Function : ~AcUIBbs
*   Comment  : AcUIBbs class Destructor
*   Date&Time : 2003-02-26, 오후 2:07
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcUIBbs::~AcUIBbs()
{
}

/*****************************************************************
*   Function : OnInit
*   Comment  : OnInit virtual function
*   Date&Time : 2003-03-12, 오후 2:24
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIBbs::OnInit()			
{
	AcUIBase::OnInit();
	AddChild( &m_clContentWindow );
	m_clContentWindow.ShowWindow( FALSE );
	return TRUE;
}

/*****************************************************************
*   Function : InitInfoInput
*   Comment  : 
*   Date&Time : 2003-02-26, 오후 2:37
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::InitInfoInput( stAcUIBbsInfo *pstBbsInfo )
{
	m_stBbsInfo = *( pstBbsInfo );
	_CreateCellContentMemory();
		
	m_bInitSetting = TRUE;
	PageManagerSetting();
}

/*****************************************************************
*   Function : Destroy
*   Comment  : Destroy
*   Date&Time : 2003-02-26, 오후 5:20
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIBbs::Destroy()	
{
	_DestroyCellContentMemory();
	return TRUE;
}

/*****************************************************************
*   Function : SetCellContentMemory
*   Comment  : Set Memory of Content Memory
*   Date&Time : 2003-02-26, 오후 5:04
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::_CreateCellContentMemory()								
{
	if( m_pstCellContent )
	{
		_DestroyCellContentMemory();
	}

	m_pstCellContent = ( stAgcUIBbsCellContent* )malloc( sizeof( stAgcUIBbsCellContent ) * ( m_stBbsInfo.m_cColumnNum * m_stBbsInfo.m_cRowNum ) );
	if( m_pstCellContent )
	{
		// Cell초기화 
		ClearAllCellText();
	}

	return;
}

/*****************************************************************
*   Function : DestroyCellContentMemory
*   Comment  : Destroy Cell Content Memory
*   Date&Time : 2003-02-26, 오후 5:09
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::_DestroyCellContentMemory()
{
	if( m_pstCellContent )
	{
		free( m_pstCellContent );
		m_pstCellContent = NULL;
	}

	return;
}

/*****************************************************************
*   Function : SetColumn
*   Comment  : Set Column
*   Date&Time : 2003-02-26, 오후 5:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIBbs::SetColumnInfo( UINT8 cColumnIndex, stAgcUIBbsColumnInfo *pstBbsColumnInfo )
{
	if( cColumnIndex >= ACUIBBS_COLUMN_NUM_MAX ) return FALSE;
	memcpy( &( m_stColumnInfo[cColumnIndex ] ), pstBbsColumnInfo, sizeof( stAgcUIBbsColumnInfo ) );

	/*strncpy( m_stColumnInfo[cColumnIndex].szColumnName , pszColumnName, ACUIBBS_COLUMN_NAME_MAX );
	m_stColumnInfo[cColumnIndex].nColumnWidth = nColumnWidth;
	m_stColumnInfo[cColumnIndex].eArrayType = eArrayType;*/
	return TRUE;
}

BOOL AcUIBbs::SetColumnInfo( UINT8 cColumnIndex, char *pszColumnName, UINT16 nColumnWidth, HANFONT_ARRANGETYPE eArrayType )
{
	if( cColumnIndex >= ACUIBBS_COLUMN_NUM_MAX ) return FALSE;

	strncpy( m_stColumnInfo[ cColumnIndex ].szColumnName, pszColumnName, ACUIBBS_COLUMN_NAME_MAX );
	m_stColumnInfo[ cColumnIndex ].nColumnWidth = nColumnWidth;
	m_stColumnInfo[ cColumnIndex ].eArrayType = eArrayType;

	return TRUE;
}

/*****************************************************************
*   Function : _DrawBbsContent
*   Comment  : Bbs에 Column명과 함께 Content를 써 넣는다. 
*   Date&Time : 2003-02-27, 오전 10:37
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::_DrawBbsContent()				
{
	if ( !m_bInitSetting ) return;

	int nDrawStart_x = ( int )( m_stBbsInfo.m_v2dCellStart.x );
	int nDrawStart_y = ( int )( m_stBbsInfo.m_v2dCellStart.y );

	ClientToScreen( &nDrawStart_x, &nDrawStart_y );

	RwRect rectDraw;
	rectDraw.h = m_stBbsInfo.m_nCellHeight;
	rectDraw.y = nDrawStart_y;

	//Draw Column Name 
	for ( int nColCount = 0 ; nColCount < m_stBbsInfo.m_cColumnNum ; ++nColCount )
	{
		rectDraw.x = nDrawStart_x; 
		rectDraw.w = m_stColumnInfo[ nColCount ].nColumnWidth;

		_DrawColumnName( &rectDraw, nColCount );
	}

	//Draw Cell Content
	for( int nRowCount = 0 ; nRowCount < m_stBbsInfo.m_cRowNum ; ++nRowCount )
	{
		nDrawStart_x = ( int )( m_stBbsInfo.m_v2dCellStart.x );
		nDrawStart_y = ( int )( m_stBbsInfo.m_v2dCellStart.y );
		ClientToScreen( &nDrawStart_x, &nDrawStart_y );

		for( int nColCount = 0 ; nColCount < m_stBbsInfo.m_cColumnNum ; ++nColCount )
		{
			rectDraw.x = nDrawStart_x; 
			rectDraw.y = nDrawStart_y + m_stBbsInfo.m_nCellHeight * nRowCount + m_stBbsInfo.m_nGapColumnCell;
			rectDraw.w = m_stColumnInfo[ nColCount ].nColumnWidth;

			_DrawCell( &rectDraw, nRowCount, nColCount );
			nDrawStart_x += m_stColumnInfo[ nColCount ].nColumnWidth + m_stBbsInfo.m_nColumnGap ;
		}
	}
}

/*****************************************************************
*   Function : _DrawPageManager
*   Comment  : Draw Page Manager 
*   Date&Time : 2003-03-13, 오후 3:08
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::_DrawPageManager()
{
	if( !_IsValidPageIndex() ) return;

	int nPageManagerGap = 20;
	int nPageManagerCharacterGap = 10;

	if ( h < nPageManagerGap ) return;
	
	RwRect rectDrawPageManager = { nPageManagerGap, h - nPageManagerGap, w, nPageManagerGap	};
	ClientToScreen( &rectDrawPageManager.x , &rectDrawPageManager.y );

	for( int nPageCount = 0 ; nPageCount < ACUIBBS_PAGEMANAGER_NUM ; ++nPageCount )
	{
		if( m_stPageManager[ nPageCount ].lPageIndex == 0 ) return;
		
		if( m_pAgcmFont )
		{
			m_stPageManager[ nPageCount ].rectPageSelectArea = m_pAgcmFont->DrawTextRectIM2D( &rectDrawPageManager, m_stPageManager[ nPageCount ].szPageIndex, 0, FF_LEFT );
		}
		
		rectDrawPageManager.x += ( m_stPageManager[ nPageCount ].rectPageSelectArea.w + nPageManagerCharacterGap );

		if ( m_stPageManager[ nPageCount ].lPageIndex == -1 ) return;
	}
}

/*****************************************************************
*   Function : OnPostRender
*   Comment  : World Render한 후 
*   Date&Time : 2003-02-27, 오전 10:39
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::OnPostRender( RwRaster* raster )			
{
/*	if( m_listImage.GetCount() > 0 )
	{
		int nAbsolute_x = 0;
		int nAbsolute_y = 0;
		ClientToScreen( &nAbsolute_x, &nAbsolute_y );

		RwRaster * pImageRaster = m_listImage.GetHead();

		ASSERT( NULL != pImageRaster );

		RwRasterPushContext	( raster				);
		RwRasterRender( pImageRaster, nAbsolute_x, nAbsolute_y	);
		_DrawBbsContent		( raster )				 ;
		_DrawPageManager		( raster )				 ;
		RwRasterPopContext	(						);
	}	*/
}

/*****************************************************************
*   Function : OnLButtonDown
*   Comment  : On Click Left Button 
*   Date&Time : 2003-03-11, 오전 11:28
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIBbs::OnLButtonDown( RsMouseStatus *ms )
{
	if( !m_bInitSetting ) return FALSE;
	else if ( m_stBbsInfo.m_cColumnNum <= 0 ) return FALSE;

	int nMouse_x = ( int )ms->pos.x;
	int nMouse_y = ( int )ms->pos.y;
	ClientToScreen( &nMouse_x, &nMouse_y );

	INT32 nNewPageNumber = 0;
	m_stCommandMessageInfo.lControlID = GetControlID();

	// 페이지 선택영역에서 클릭된 곳이 있는지 찾는다.
	for( int nPageManagerCount = 0 ; nPageManagerCount < ACUIBBS_PAGEMANAGER_NUM ; ++nPageManagerCount )
	{
		if( m_stPageManager[ nPageManagerCount ].lPageIndex == 0 ) break;

		// 마우스 좌표가 PageManager 영역에 포함되면..
		if( _IsInAreaPageManager( &m_stPageManager[ nPageManagerCount ], nMouse_x, nMouse_y ) )
		{
			// 이 페이지의 인덱스를 검사해서..
			switch( m_stPageManager[ nPageManagerCount ].lPageIndex )
			{
			case -2 :	// PrePage 면..
				{
					nNewPageNumber = m_stBbsInfo.m_nNowPageNum > 10 ? m_stBbsInfo.m_nNowPageNum - 10 : 1;
				}
				break;

			case -1 :	// NextPage 면..
				{
					nNewPageNumber = m_stBbsInfo.m_nAllPageNum < ( m_stBbsInfo.m_nNowPageNum + 10 ) ? m_stBbsInfo.m_nAllPageNum : m_stBbsInfo.m_nNowPageNum + 10;
				}
				break;

			default :	// 그외에는..
				{
					nNewPageNumber = m_stPageManager[ nPageManagerCount ].lPageIndex;
				}
				break;
			}

			return _SendChangePage( nNewPageNumber );
		}

		if( m_stPageManager[ nPageManagerCount ].lPageIndex == -1 ) break;
	}

	// Cell 중에 클릭된 곳이 있는지 찾는다.
	for( int nRowCount = 0 ; nRowCount < m_stBbsInfo.m_cRowNum ; ++nRowCount )
	{
		for( int nColCount = 0 ; nColCount < m_stBbsInfo.m_cColumnNum ; ++nColCount )
		{
			stAgcUIBbsCellContent* pCell = _GetCell( nRowCount, nColCount );
			if( _IsInAreaCell( pCell, nMouse_x, nMouse_y ) )
			{
				_SendClickCell( pCell->lIndex, nRowCount, nColCount );
			}
		}
	}
	
	return TRUE;
}

/*****************************************************************
*   Function : OnClose
*   Comment  : OnClose virtual function
*   Date&Time : 2003-03-11, 오후 12:06
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/	
void AcUIBbs::OnClose( void )
{
	OnClose();
	Destroy();
	m_csTextureList.DestroyTextureList();
}

/*****************************************************************
*   Function : PageMangerSetting
*   Comment  : PageMangerSetting
*   Date&Time : 2003-03-13, 오후 3:22
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::PageManagerSetting( void )
{
	if( !_IsValidPageIndex() ) return;
	
	//Paga Manager Setting
	UINT16 nPageManagerStartNum = ( UINT16 )( m_stBbsInfo.m_nNowPageNum / 10 ) + 1;
	UINT16 nPageManagerEndNum = nPageManagerStartNum + 9;

	// 마지막번호가 최대 페이지수를 넘어가지 않도록 보정
	if( nPageManagerEndNum > m_stBbsInfo.m_nAllPageNum )
	{
		nPageManagerEndNum = m_stBbsInfo.m_nAllPageNum;
	}

	m_stPageManager[ 0 ].lPageIndex = -2;
	strncpy( m_stPageManager[ 0 ].szPageIndex, "<<", ACUIBBS_PAGEMANAGER_NUM );

	int nPageCount = 0;
	for( nPageCount = nPageManagerStartNum ; nPageCount < ( nPageManagerEndNum + 1 ) ; ++nPageCount )
	{
		m_stPageManager[ ( nPageCount - nPageManagerStartNum + 1 ) ].lPageIndex = nPageCount;

		char sztemp[ ACUIBBS_PAGEMANAGER_NUM ] = { 0, };
		sprintf( sztemp, "%d", nPageCount );

		strncpy( m_stPageManager[ ( nPageCount - nPageManagerStartNum + 1 ) ].szPageIndex, sztemp, ACUIBBS_PAGEMANAGER_NUM );
	}

	m_stPageManager[ nPageCount ].lPageIndex = -1;
	strncpy( m_stPageManager[ nPageCount ].szPageIndex, ">>", sizeof( char ) * ACUIBBS_PAGEMANAGER_NUM );
}

/*****************************************************************
*   Function : SetPageInfo
*   Comment  : SetPageInfo
*   Date&Time : 2003-03-13, 오후 4:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIBbs::SetPageInfo( UINT16 nAllPage,	UINT16 nNowPage )			
{
	m_stBbsInfo.m_nAllPageNum = nAllPage;
	m_stBbsInfo.m_nNowPageNum = nNowPage;
	PageManagerSetting();
}

/*****************************************************************
*   Function : SetContent
*   Comment  : Set Content String
*   Date&Time : 2003-03-14, 오전 11:42
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIBbs::SetCellText( UINT8 nRow, UINT8 nColumn, char* pszContent )	
{
	stAgcUIBbsCellContent* pContent = _GetCell( nRow, nColumn );
	if( pContent )
	{
		strncpy( pContent->szCellContent, pszContent, sizeof( char ) * ACUIBBS_CELL_CONTENT_LENGTH_MAX );
		return TRUE;
	}

	return FALSE;
}

BOOL AcUIBbs::_IsValidArrayIndex( UINT16 nArrayIndex )
{
	if( nArrayIndex < 0 ) return FALSE;
	if( nArrayIndex >= ( m_stBbsInfo.m_cRowNum * m_stBbsInfo.m_cColumnNum ) ) return FALSE;
	return TRUE;
}

BOOL AcUIBbs::_IsValidRowIndex( UINT8 nRow )
{
	if( nRow < 0 ) return FALSE;
	if( nRow >= m_stBbsInfo.m_cRowNum ) return FALSE;
	return TRUE;
}

BOOL AcUIBbs::_IsValidColumnIndex( UINT8 nCol )
{
	if( nCol < 0 ) return FALSE;
	if( nCol >= m_stBbsInfo.m_cColumnNum ) return FALSE;
	return TRUE;
}

BOOL AcUIBbs::_IsValidPageIndex( void )
{
	// 페이지가 최소한 하나라도 있어야 하고..
	if( m_stBbsInfo.m_nAllPageNum == 0 ) return FALSE;

	// 페이지 번호는 1부터 시작하므로 현재 페이지가 0 이면 안되고..
	if( m_stBbsInfo.m_nNowPageNum == 0 ) return FALSE;

	// 물론 현재 페이지수가 최대 페이지수를 넘어가도 안된다.
	if( m_stBbsInfo.m_nNowPageNum > m_stBbsInfo.m_nAllPageNum ) return FALSE;

	return TRUE;
}

BOOL AcUIBbs::_IsValidCallBackIndex( UINT32 nCallBackIndex )
{
	if( nCallBackIndex < 0 ) return FALSE;
	if( nCallBackIndex >= ACUIBBS_CALLBACKPOINT_MAX_NUM ) return FALSE;
	return TRUE;
}

BOOL AcUIBbs::_IsInAreaPageManager( stAgcUIBbsPageManager* pPageManager, INT32 nMouseX, INT32 nMouseY )
{
	if( !pPageManager ) return FALSE;

	// 판정기준 : 지정 마우스 좌표가 PageRect 영역 내에 포함되는가?
	INT32 nMinX = pPageManager->rectPageSelectArea.x;
	INT32 nMaxX = pPageManager->rectPageSelectArea.x + pPageManager->rectPageSelectArea.w;
	INT32 nMinY = pPageManager->rectPageSelectArea.y;
	INT32 nMaxY = pPageManager->rectPageSelectArea.y + pPageManager->rectPageSelectArea.h;

	if( nMouseX >= nMinX && nMouseX <= nMaxX && nMouseY >= nMinY && nMouseY <= nMaxY ) return TRUE;
	return FALSE;
}

BOOL AcUIBbs::_IsInAreaCell( stAgcUIBbsCellContent* pCell, INT32 nMouseX, INT32 nMouseY )
{
	if( !pCell ) return FALSE;
	
	// 판정기준 : 지정 마우스 좌표가 PageRect 영역 내에 포함되는가?
	INT32 nMinX = pCell->rectSelectArea.x;
	INT32 nMaxX = pCell->rectSelectArea.x + pCell->rectSelectArea.w;
	INT32 nMinY = pCell->rectSelectArea.y;
	INT32 nMaxY = pCell->rectSelectArea.y + pCell->rectSelectArea.h;

	if( nMouseX >= nMinX && nMouseX <= nMaxX && nMouseY >= nMinY && nMouseY <= nMaxY ) return TRUE;
	return FALSE;
}

stAgcUIBbsCellContent* AcUIBbs::_GetCell( UINT32 nRow, UINT32 nCol )
{
	UINT16 nArrayIndex = ( nRow * m_stBbsInfo.m_cColumnNum ) + nCol;
	return _GetCell( nArrayIndex );
}

stAgcUIBbsCellContent* AcUIBbs::_GetCell( UINT16 nArrayIndex )
{
	if( !_IsValidArrayIndex( nArrayIndex ) ) return NULL;

	if( m_pstCellContent )
	{
		return ( stAgcUIBbsCellContent* )( ( BYTE* )m_pstCellContent + ( sizeof( stAgcUIBbsCellContent ) * nArrayIndex ) );
	}

	return NULL;
}

BOOL AcUIBbs::_SendChangePage( INT32 nNewPageNumber )
{
	if( pParent )
	{
		m_stCommandMessageInfo.lSelectedIndex = nNewPageNumber;
		return pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BBS_CHANGE_PAGE, ( PVOID )&m_lControlID );	
	}

	return TRUE;
}

BOOL AcUIBbs::_SendClickCell( UINT32 lIndex, INT32 nRow, INT32 nColumn )
{
	if( pParent )
	{
		m_stCommandMessageInfo.lSelectedIndex = lIndex;
		m_stCommandMessageInfo.lSelectedRow = nRow;
		m_stCommandMessageInfo.lSelectedColumn = nColumn;
		return pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BBS_CLICK_CONTENT, ( PVOID )&m_lControlID );
	}

	return TRUE;
}

void AcUIBbs::_DrawColumnName( RwRect* pRect, INT32 nColCount )
{
	if( !pRect ) return;
	if( m_pAgcmFont )
	{
		m_pAgcmFont->DrawTextRectIM2D( pRect, m_stColumnInfo[ nColCount ].szColumnName, 0, m_stColumnInfo[ nColCount ].eArrayType );
	}
}

void AcUIBbs::_DrawCell( RwRect* pRect, INT32 nRowCount, INT32 nColCount )
{
	if( !pRect ) return;

	UINT16 nArrayIndex = m_stBbsInfo.m_cColumnNum * nRowCount + nColCount;
	if ( m_pAgcmFont )
	{
		m_pstCellContent[ nArrayIndex ].rectSelectArea = m_pAgcmFont->DrawTextRectIM2D( pRect, m_pstCellContent[ nArrayIndex ].szCellContent, 0, m_stColumnInfo[ nColCount ].eArrayType );
	}
}


/*****************************************************************
*   Function : ClearCellText_ByRow
*   Comment  : 특정 Row의 Content를 지운다 
*   Date&Time : 2003-07-03, 오후 5:24
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIBbs::ClearCellText_ByRow( UINT8 nRow )
{
	if( !_IsValidRowIndex( nRow ) ) return;

	for( int nColCount = 0 ; nColCount < m_stBbsInfo.m_cColumnNum ; ++nColCount )
	{
		ClearCellText( nRow, nColCount );
	}
}

void AcUIBbs::ClearCellText( UINT32 nRow, UINT32 nColumn )
{
	stAgcUIBbsCellContent* pContent = _GetCell( nRow, nColumn );
	if( pContent )
	{
		pContent->lIndex = 0;
		memset( pContent->szCellContent, 0, sizeof( char ) * ACUIBBS_CELL_CONTENT_LENGTH_MAX );
		memset( &pContent->rectMyArea, 0, sizeof( RwRect ) );
		memset( &pContent->rectSelectArea, 0, sizeof( RwRect ) );
	}
}

/*****************************************************************
*   Function : ClearAllCellText
*   Comment  : 모든 Cell의 Text를 지운다 
*   Date&Time : 2003-07-03, 오후 5:08
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIBbs::ClearAllCellText( void )
{
	for( int nRowCount = 0 ; nRowCount < m_stBbsInfo.m_cRowNum ; ++nRowCount )
	{
		for( int nColCount = 0 ; nColCount < m_stBbsInfo.m_cColumnNum ; ++nColCount )
		{
			ClearCellText( nRowCount, nColCount );
		}
	}
}

void AcUIBbs::OnWindowRender( void )
{
	PROFILE( "AcUIBbs::OnWindowRender" );

	if( m_csTextureList.GetCount() > 0 )
	{
		int nAbsX = 0;
		int nAbsY = 0;

		ClientToScreen( &nAbsX, &nAbsY );

		stTextureListInfo * pTextureListInfo = m_csTextureList.GetHeadTextureListInfo();
		ASSERT( NULL != pTextureListInfo );

		// 바닥 배경 텍스처 검사해서..
		RwTexture* pBGTexture = pTextureListInfo->pTexture;
		if( pBGTexture != NULL )
		{
			RwRaster* pBGRaster = RwTextureGetRaster( pBGTexture );
			if( pBGRaster != NULL )
			{
				// 바닥 배경을 그려준다.
				FLOAT fWidth = ( FLOAT )RwRasterGetWidth( pBGRaster );
				FLOAT fHeight = ( FLOAT )RwRasterGetHeight( pBGRaster );
				FLOAT fAlpha = 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1.0f );

				g_pEngine->DrawIm2D( pBGTexture, ( FLOAT )nAbsX, ( FLOAT )nAbsY, fWidth, fHeight, 0.0f, 0.0f, 1.0f, 1.0f,	0xffffffff, ( UINT8 )fAlpha );
			}
		}

		// 게시판 내용 그리고..
		_DrawBbsContent();

		// 페이지 매니저 그려준다.
		_DrawPageManager();
	}	
}
