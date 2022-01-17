// UITileList_TileTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MapTool.h"
#include "rwcore.h"
#include "rpworld.h"
//#include "AcuRpDWSector.h"
#include "MainWindow.h"
#include "UITileList_TileTabWnd.h"
#include "UITileList_EditTileDlg.h"
#include "MainFrm.h"
#include "ApUtil.h"


#include "TileEditCategoryDlg.h"
#include "TileNameEditDlg.h"
#include "ConvertTextureDlg.h"

#include "MyEngine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern MainWindow			g_MainWindow	;
extern CMainFrame *			g_pMainFrame	;
extern MyEngine				g_MyEngine		;

#define	IDT_CURSOR	13034
/////////////////////////////////////////////////////////////////////////////
// CUITileList_TileTabWnd

CUITileList_TileTabWnd::CUITileList_TileTabWnd()
{
	m_pSelectedCategory	= NULL					;
	m_nSelectedLayer	= TILELAYER_BACKGROUND	;
	m_pSelectedTile		= NULL					;
}

CUITileList_TileTabWnd::~CUITileList_TileTabWnd()
{
}


BEGIN_MESSAGE_MAP(CUITileList_TileTabWnd, CWnd)
	//{{AFX_MSG_MAP(CUITileList_TileTabWnd)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_TILEWINDOW_ADDCATEGORY, OnTilewindowAddcategory)
	ON_COMMAND(IDM_TILEWINDOW_DELETE_CATEGORY, OnTilewindowDeleteCategory)
	ON_COMMAND(IDM_TILEWINDOW_EDIT_CATEGORY, OnTilewindowEditCategory)
	ON_WM_DROPFILES()
	ON_COMMAND(IDM_TILEWINDOW_EDIT_TILE, OnTilewindowEditTile)
	ON_COMMAND(IDM_TILEWINDOW_DELETE_TILE, OnTilewindowDeleteTile)
	ON_COMMAND(IDM_TILEPOPUP_CHANGETILE, OnTilewindowChangeTile)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileList_TileTabWnd message handlers

void CUITileList_TileTabWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	CDC	memDC;
	memDC.CreateCompatibleDC( &dc );

	// 바닥타일..;;
	m_MemDC.FillRect( rect , &m_backgroundbrush );

	m_MemDC.SetBkMode( TRANSPARENT );

// 마고자 (2004-05-30 오후 4:47:28) : 유명 무실의 레이어 일단 제거..
//	// 레이어 출력
//	{
//		CRect	rectLayer;
//		rectLayer.left	= 0					;
//		rectLayer.top	= 0					;
//		rectLayer.right	= rect.Width()		;
//		rectLayer.bottom= TILELAYER_HEIGHT	;
//
//		memDC.SelectObject( m_aBitmapLayer[ m_nSelectedLayer ] );
//		m_MemDC.BitBlt( rectLayer.left , rectLayer.top , rectLayer.Width() , rectLayer.Height() , &memDC , 0 , 0 , SRCCOPY );
//	}

	// 카테고리 출력.
	{
		AuNode< stTileCategory * > *	pNode = m_listCategory.GetHeadNode();
		stTileCategory *				pRef			;
		int								count = 0		;
		CRect							rectCategory	;

		while( pNode )
		{
			pRef = pNode->GetData();

			// Rect 설정.
			rectCategory.left	= 0										;
			rectCategory.top	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * count			;
			rectCategory.right	= TILECATEGORY_WIDTH					;
			rectCategory.bottom	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * ( count + 1 )	;

			// 박스표시.
			if( pRef == m_pSelectedCategory )
			{
				// 선택됀것;
				/* dc.FillSolidRect	( rectCategory , TILECATEGORY_COLOR_SELECTEDBOX	);*/
				memDC.SelectObject( m_bitmapCategoryBoxSelected );
				m_MemDC.BitBlt( rectCategory.left , rectCategory.top , rectCategory.Width() , rectCategory.Height() , &memDC , 0 , 0 , SRCCOPY );

				m_MemDC.SetTextColor		( TILECATEGORY_COLOR_SELECTEDTEXT				);
				m_MemDC.DrawText			( pRef->name , rectCategory , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
			}
			else
			{
				// 선택돼지 않은것.
				/* dc.FillSolidRect	( rectCategory , TILECATEGORY_COLOR_DEFAULTBOX	);*/
				memDC.SelectObject( m_bitmapCategoryBoxUnselect );
				m_MemDC.BitBlt( rectCategory.left , rectCategory.top , rectCategory.Width() , rectCategory.Height() , &memDC , 0 , 0 , SRCCOPY );
				m_MemDC.SetTextColor		( TILECATEGORY_COLOR_DEFAULTTEXT				);
				m_MemDC.DrawText			( pRef->name , rectCategory , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
			}

			m_listCategory.GetNext( pNode );
			count++;
		}
	}

	// 카테고리 선택됀게 있으면
	// 
	if( m_pSelectedCategory )
	{
		// 출력함.. 
	
		AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
		stTileInfo	* pTileInfo;

		int		count		;
		int		line		;
		int		offset		;
		CRect	rectTileBox	;
		CRect	rectOutLine	;
		CRect	rectTmp		;

		count	= 0;
		line	= 0;
		offset	= 0;

		while( pNode )
		{
			pTileInfo = & pNode->GetData();

			// 라인 설정.
			line	= count / TILECATEGORY_COUNTPERLINE;
			offset	= count % TILECATEGORY_COUNTPERLINE;

			// Rect 설정.
			rectTileBox.SetRect(
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset		),
				TILELAYER_HEIGHT + 		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line		),
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset	+ 1	),
				TILELAYER_HEIGHT + 		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line	+ 1	)
				);

			// 정보 출력.
			// 아웃라인 박스를 구함..
			rectOutLine.left	= rectTileBox.left		- TILECATEGORY_TILEWMARGIN;
			rectOutLine.top		= rectTileBox.top		- TILECATEGORY_TILEHMARGIN;
			rectOutLine.right	= rectTileBox.right		+ TILECATEGORY_TILEWMARGIN;
			rectOutLine.bottom	= rectTileBox.bottom	+ TILECATEGORY_TILEHMARGIN;

			// 셀렉션 표시.
			if( m_pSelectedTile == pTileInfo )
			{	
				static bool bToggle = false;
				if( bToggle )
				{
					memDC.SelectObject( m_bitmapTileSelect );
				}
				else
				{
					memDC.SelectObject( m_bitmapTileSelect2 );
				}
				bToggle = ! bToggle;
				m_MemDC.BitBlt( rectOutLine.left , rectOutLine.top , rectOutLine.Width() , rectOutLine.Height() , &memDC , 0 , 0 , SRCCOPY );

				m_rectCursor	= rectOutLine;

				// dc.FillSolidRect( rectOutLine , TILECATEGORY_COLOR_SELECTEDBOX );
			}
			else
			{
				// do nothing;
				AuNode< int > * pNode = g_pMainFrame->m_Document.m_listSelectedTileIndex.GetHeadNode();
				int	nTile;
				while( pNode )
				{
					nTile = pNode->GetData();

					if( pTileInfo->index == nTile )
					{
						memDC.SelectObject( m_bitmapTileSelect );
						m_MemDC.BitBlt( rectOutLine.left , rectOutLine.top , rectOutLine.Width() , rectOutLine.Height() , &memDC , 0 , 0 , SRCCOPY );

						m_rectCursor	= rectOutLine;
						break;
					}

					pNode = pNode->GetNextNode();
				}
			}

			// 이미지 출력
			// dc.FillSolidRect( rectTileBox , RGB( 255 , 255 , 255 ) );

			LoadBitmap( pTileInfo );
			if( pTileInfo->pBmp )
			{
				pTileInfo->pBmp->Draw( m_MemDC.GetSafeHdc() , rectTileBox.left , rectTileBox.top );
			}

			// 텍스트 출력
			rectTmp			= rectOutLine;
			rectTmp.left	+= 1;
			rectTmp.top		+= 1;
			rectTmp.right	+= 1;
			rectTmp.bottom	+= 1;
			
			m_MemDC.SetTextColor	( TILECATEGORY_TILEINFOTEXTBKCOLOR );
			m_MemDC.DrawText		( pTileInfo->name , rectTmp , DT_LEFT | DT_TOP | DT_SINGLELINE );

			if( IsUsedTileName( pTileInfo ) )
				m_MemDC.SetTextColor	( TILECATEGORY_TILEINFOTEXTCOLOR_USED );
			else
				m_MemDC.SetTextColor	( TILECATEGORY_TILEINFOTEXTCOLOR_UNUSED );

			m_MemDC.DrawText		( pTileInfo->name , rectOutLine , DT_LEFT | DT_TOP | DT_SINGLELINE );

			m_pSelectedCategory->list.GetNext( pNode );
			count ++;
		}
	}
		
	// 세로 구분선 긋기.
	m_MemDC.MoveTo( TILECATEGORY_WIDTH , TILELAYER_HEIGHT);
	m_MemDC.LineTo( TILECATEGORY_WIDTH , rect.Height()	);
	

	// 마고자 (2004-05-29 오후 10:16:52) : 디멘젼 표시~
	CRect	rectDimension = rect;
	rectDimension.top = rectDimension.bottom - 20;
	m_MemDC.FillSolidRect( rectDimension , RGB( 211, 135, 231 ) );
	
	AuNode< int > * pNode = g_pMainFrame->m_Document.m_listSelectedTileIndex.GetHeadNode();
	if( pNode )
	{
		int * pData = &pNode->GetData();
		CString	str;
		
		AcTextureList::TextureInfo * pInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( * pData );
		if( pInfo )
		{
			str.Format( "%s - 타일 %dx%d 모드~" , __RemovePath( pInfo->strFilename ) , GET_TEXTURE_DIMENSION( *pData ) , GET_TEXTURE_DIMENSION( *pData ) );
		}
		else
		{
			str.Format( "타일 %dx%d 모드~" , GET_TEXTURE_DIMENSION( *pData ) , GET_TEXTURE_DIMENSION( *pData ) );
		}

		m_MemDC.SetTextColor( RGB( 255 , 255 , 255 ) );
		m_MemDC.DrawText( str , rectDimension , DT_VCENTER | DT_CENTER | DT_SINGLELINE );
	}

	dc.BitBlt( 0 , 0 , rect.Width() , rect.Height() , & m_MemDC , 0 , 0 , SRCCOPY );
	// Do not call CWnd::OnPaint() for painting messages
}

void CUITileList_TileTabWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( point.x < TILECATEGORY_WIDTH )
	{
		// 팝업 메뉴를 띄운다.

		// 카테고리냐 타일이냐 구분
		stTileCategory * pRef;
		pRef = GetCategoryFromPoint( point );
		if( pRef )
		{
			// 까꿍..;
			m_pSelectedCategory = pRef;
			Invalidate( FALSE );

			CMenu *pMenu;
			CMenu menu;

			menu.LoadMenu(IDR_TILEMENU);
			pMenu = menu.GetSubMenu(0);

			ClientToScreen( &point );

			TrackPopupMenu( pMenu->GetSafeHmenu() , TPM_TOPALIGN | TPM_LEFTALIGN ,
				point.x , point.y , 0 , GetSafeHwnd() , NULL );
		}
	
	}
	else
	{
		stTileInfo * pTileInfo;
		pTileInfo = GetTileFromPoint( point );
		if( pTileInfo )
		{
			if( pTileInfo != m_pSelectedTile )
			{
				// 다중선택..

				// 타일 선택이 바뀌었음을 메인윈도우에게 알려준다.
				g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( pTileInfo->index );
			}
			else
			{
				m_pSelectedTile = pTileInfo;

				// 타일 선택이 바뀌었음을 메인윈도우에게 알려준다.
				g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()				;
				g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( pTileInfo->index );
				g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= pTileInfo->applyalpha		;
			}

			Invalidate( FALSE );

			switch( GET_TEXTURE_TYPE( pTileInfo->index ) )
			{
			case TT_FLOORTILE	:
			case TT_ONE			:
			case TT_ALPHATILE	:
				{
					// FLOOR 모드로 변경한다.
					ChangeLayer( TILELAYER_BACKGROUND );
				}
				break;

			case TT_UPPERTILE	:
				{
					// Float 모드로 변경한다.
					ChangeLayer( TILELAYER_UPPER );
				}
				break;

			}

			CMenu *pMenu;
			CMenu menu;

			menu.LoadMenu(IDR_TILEMENU);
			pMenu = menu.GetSubMenu(1);

			ClientToScreen( &point );

			TrackPopupMenu( pMenu->GetSafeHmenu() , TPM_TOPALIGN | TPM_LEFTALIGN ,
				point.x , point.y , 0 , GetSafeHwnd() , NULL );
		}
	}

	
	CWnd::OnRButtonDown(nFlags, point);
}

int CUITileList_TileTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( NULL == AGCMMAP_THIS ) return 0;

	// 이전에 저장됀 카테고리 정보를 로딩함.
	LoadScript();
	
	// 분류됀 카테고리 정보를 이용해서 로딩돼어있는 타일을 분류한다.
	
	CreateTextureList( &AGCMMAP_THIS->m_TextureList );

	// 처음 등록된 타입을 선택한다.
	if( m_listCategory.GetHeadNode() )
		m_pSelectedCategory = m_listCategory.GetHeadNode()->GetData()	;
	else
		m_pSelectedCategory = NULL										;

	DragAcceptFiles();
	
	m_backgroundbitmap.LoadBitmap			( IDB_TILE_BACKTILE		);
	m_backgroundbrush.CreatePatternBrush	( & m_backgroundbitmap	);

	m_aBitmapLayer[ TILELAYER_BACKGROUND	].LoadBitmap( IDB_TILE_LAYER_BACKGROUND	);
	m_aBitmapLayer[ TILELAYER_TONGMAP		].LoadBitmap( IDB_TILE_LAYER_TONGMAP	);
	m_aBitmapLayer[ TILELAYER_ALPHATILEING	].LoadBitmap( IDB_TILE_LAYER_ALPHA		);
	m_aBitmapLayer[ TILELAYER_UPPER			].LoadBitmap( IDB_TILE_LAYER_UPPER		);
	
	m_bitmapCategoryBoxSelected.	LoadBitmap( IDB_TILE_CATEGORY_SELECTED	);
	m_bitmapCategoryBoxUnselect.	LoadBitmap( IDB_TILE_CATEGORY_UNSELECT	);
	m_bitmapTileSelect.				LoadBitmap( IDB_TILE_TILESELECT			);
	m_bitmapTileSelect2.			LoadBitmap( IDB_TILE_TILESELECT2		);

	SetTimer( IDT_CURSOR , 1500 , NULL );

	CDC	*pDC = GetDC();
	m_MemDC.CreateCompatibleDC( pDC );
	ReleaseDC( pDC );

	CFont	font;
	font.CreatePointFont( 90 , "굴림" );
	m_MemDC.SelectObject( font );

	return 0;
}

void CUITileList_TileTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	GetClientRect( rect );

// 마고자 (2004-05-30 오후 4:48:04) : 유명무실 레이어 일단 제거.
	// 레이어냐 아냐니 구분
//	if( point.y < TILELAYER_HEIGHT )
//	{
//		// 레이어 번경이다.
//
//		int layer = point.x / ( rect.Width() / TILELAYER_COUNT );
//		ChangeLayer( layer );
//
//		return;
//	}


	// 카테고리냐 타일이냐 구분
	stTileCategory * pRef;
	pRef = GetCategoryFromPoint( point );
	if( pRef )
	{
		// 까꿍..;
		m_pSelectedCategory = pRef;
		Invalidate( FALSE );
		return;
	}

	stTileInfo * pTileInfo;
	pTileInfo = GetTileFromPoint( point );
	if( pTileInfo )
	{
		m_pSelectedTile = pTileInfo;

		// 타일 선택이 바뀌었음을 메인윈도우에게 알려준다.
		
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()				;
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( pTileInfo->index );
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= pTileInfo->applyalpha		;

		Invalidate( FALSE );

		switch( GET_TEXTURE_TYPE( pTileInfo->index ) )
		{
		case TT_FLOORTILE	:
		case TT_ONE			:
		case TT_ALPHATILE	:
			{
				// FLOOR 모드로 변경한다.
				ChangeLayer( TILELAYER_BACKGROUND );
			}
			break;

		case TT_UPPERTILE	:
			{
				// Float 모드로 변경한다.
				ChangeLayer( TILELAYER_UPPER );
			}
			break;

		}


		return;
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CUITileList_TileTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);
}


void	CUITileList_TileTabWnd::LoadScript()
{
	// 스크립트 파일 로딩..
	
	FILE	*pFile = fopen( TILECATEGORY_FILE , "rt" );
	if( pFile == NULL )
	{
		// 파일오느 실패.
		return ;
	}

	CGetArg2		arg;
	char			strbuffer[	1024	];
	//unsigned int	nRead = 0;

	int count = 0;
	char	categoryname[ 256 ];
	stTileCategory	*pCategory;
	while( fgets( strbuffer , 1024 , pFile ) )
	{
		// 읽었으면..

		arg.SetParam( strbuffer , "|\n" );

		if( arg.GetArgCount() < 2 )
		{
			// 갯수 이상
			continue;
		}

		pCategory			= new stTileCategory			;
		pCategory->category		= atoi( arg.GetParam( 0 ) )		;
		strncpy( categoryname , arg.GetParam( 1 ) , 256 )	;
		pCategory->name		= categoryname					;

		// 소팅하면서 저장함..
		AddCategory( pCategory );
		
		count ++;
	}

	fclose( pFile );

}

BOOL CUITileList_TileTabWnd::AddCategory( stTileCategory * pCategory )
{
	// 소팅해서 집어넣는다..( 타입별로.. )
	AuNode< stTileCategory * > *	pNode = m_listCategory.GetHeadNode();
	stTileCategory *				pRef;

	while( pNode )
	{
		pRef = pNode->GetData();

		if( pRef->category > pCategory->category )
		{
			m_listCategory.InsertBefore( pNode , pCategory );
			return TRUE;
		}

		m_listCategory.GetNext( pNode );
	}

	m_listCategory.AddTail( pCategory );

	return TRUE;
}

void CUITileList_TileTabWnd::OnDestroy() 
{
	CWnd::OnDestroy();

	// 카테고리 딜리트..
	AuNode< stTileCategory * > * pNode;
	stTileCategory *	pRef;

	while( pNode = m_listCategory.GetHeadNode() )
	{
		pRef = pNode->GetData();

		AuNode< stTileInfo > * pNode = pRef->list.GetHeadNode();
		stTileInfo	* pInsertedTileInfo;
		while( pNode )
		{
			pInsertedTileInfo = & pNode->GetData();

			// 비트맵 해제..
			if( pInsertedTileInfo->pBmp )
			{
				pInsertedTileInfo->pBmp->Release();
				delete pInsertedTileInfo->pBmp;
				pInsertedTileInfo->pBmp = NULL;
			}

			pRef->list.GetNext( pNode );
		}

		delete pRef;

		m_listCategory.RemoveHead();
	}

	KillTimer( IDT_CURSOR );
}

BOOL CUITileList_TileTabWnd::SaveScript()
{
	AuNode< stTileCategory * > * pNode = m_listCategory.GetHeadNode();
	stTileCategory *	pRef;

	char str[ 1024 ];
	GetCurrentDirectory( 1024 , str );
	FILE	*pFile = fopen( TILECATEGORY_FILE , "wt" );
	if( pFile == NULL )
	{
		// 파일오느 실패.
		return FALSE;
	}
	

	while( pNode )
	{
		pRef = pNode->GetData();
		wsprintf( str , "%d|%s\n" , pRef->category , (LPCTSTR) pRef->name );
		fputs( str , pFile );
		m_listCategory.GetNext( pNode );
	}

	fclose( pFile );
	return TRUE;
}

BOOL CUITileList_TileTabWnd::CreateTextureList(AcTextureList *pList)
{
	AuNode< AcTextureList::TextureInfo > * pNode = pList->TextureList.GetHeadNode();
	AcTextureList::TextureInfo *	pTi			;

	while( pNode )
	{
		pTi = & pNode->GetData();

		switch( GET_TEXTURE_TYPE( pTi->nIndex ) )
		{
		case TT_FLOORTILE	:
		case TT_ONE			:
		case TT_ALPHATILE	:
			{
				// 한개만 등록.
				AddTile( pTi->nIndex , pTi->strComment );
			}
			break;
		case TT_UPPERTILE	:
			{
				// 갯수별로 등록.

				// 지금은 잠시 - -;

				// 조만간 추가하게될거심.
				CString	str;
				for( int y = 0 ; y < 4 ; y ++ )
				{
					for( int x = 0 ; x < 4 ; x ++ )
					{
						str.Format( "%s %d" , pTi->strComment , y * 4 + x );
						AddTile( pTi->nIndex + y * 4 + x , (LPSTR) (LPCTSTR) str );
					}
				}
			}
			break;
		}

		pList->TextureList.GetNext( pNode );
	}

	return TRUE;
}

BOOL CUITileList_TileTabWnd::LoadBitmap			( stTileInfo  * pTileInfo , BOOL bForce )
{
	if( !bForce && pTileInfo->pBmp )
	{
		return TRUE;
	}
	else
	{
		if( pTileInfo->pBmp ) delete pTileInfo->pBmp;

		pTileInfo->pBmp = new CBmp;

		// 로딩성공.
		CString	filename;
		filename.Format( "%s\\Map\\Temp\\%d,%d.bmp" , ALEF_CURRENT_DIRECTORY ,
			GET_TEXTURE_CATEGORY( pTileInfo->index ) , GET_TEXTURE_INDEX( pTileInfo->index ) );

		AcTextureList::TextureInfo *	pTextureInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( pTileInfo->index	);
		if( pTextureInfo )
		{
			AGCMMAP_THIS->m_TextureList.MakeTempBitmap( pTextureInfo );
		}

		if( !pTileInfo->pBmp->Load( (LPSTR) (LPCTSTR) filename , NULL ) )
		{
			// 에러라면 딴걸 읽음..;
			pTileInfo->pBmp->Load( TILECATEGORY_ERROR_BITMAP , NULL );
		}
	}

	return TRUE;
}

BOOL CUITileList_TileTabWnd::AddTile	( int index , char * comment	)
{
	stTileCategory *				pCategory	;
	pCategory = FindCategory( GET_TEXTURE_CATEGORY( index ) );

	// 카테고리가 존재하지 않는것이라면
	// 타입을 기준으로하여 그냥 생성해버린다.
	if( pCategory == NULL )
	{
		pCategory = new stTileCategory;

		CString	str;
		str.Format( "%d번타입" , GET_TEXTURE_CATEGORY( index ) );
		pCategory->category	= GET_TEXTURE_CATEGORY( index );
		pCategory->name		= str;

		AddCategory( pCategory );
	}

	// 소팅해서 삽입.
	// 소팅해서 집어넣는다..( 타입별로.. )
	AuNode< stTileInfo > * pNode = pCategory->list.GetHeadNode();
	stTileInfo	* pInsertedTileInfo;
	stTileInfo	insertitem;

	insertitem.index		= index				;
	insertitem.name			= comment			;
	insertitem.pBmp			= NULL				;
	
	insertitem.applyalpha	= ALEF_DEFAULT_ALPHA;
	
	// 소트 시켜서 사이에 살짝 살짝 찔러 넣는다.
	while( pNode )
	{
		pInsertedTileInfo = & pNode->GetData();

		if( GET_TEXTURE_INDEX ( pInsertedTileInfo->index	) >
			GET_TEXTURE_INDEX ( index						)	)
		{
			
			pCategory->list.InsertBefore( pNode , insertitem );
			return TRUE;
		}

		pCategory->list.GetNext( pNode );
	}

	pCategory->list.AddTail( insertitem );

	return TRUE;
}

CUITileList_TileTabWnd::stTileCategory * CUITileList_TileTabWnd::FindCategory( int category )
{
	AuNode< stTileCategory * > * pNode = m_listCategory.GetHeadNode();
	stTileCategory * pCategory	;

	while( pNode )
	{
		pCategory = pNode->GetData();

		if( pCategory->category == category ) return pCategory;

		m_listCategory.GetNext( pNode );
	}

	return NULL;
}

void CUITileList_TileTabWnd::OnTilewindowAddcategory() 
{
	// 카테고리 추가함.
	// 카테고리 추가 Dialog 띄움.
	// 카테고리 넘버 ( 순차적으로 생성됨 , 임의로 수정가능 )
	// 카테고리 이름 받음.
	// 카테고리 넘버가 겹치는지 점검 , 겹친것의 정보를 표시해준다.

	stTileCategory * pCategory;
	
	pCategory			= new stTileCategory			;

	if( m_listCategory.GetTailNode() != NULL )
		pCategory->category		= 		m_listCategory.GetTail()->category + 1;	// 마지막에 들록된 타일 인덱스 에다 1을 더한값으로 사용함.
	else
		pCategory->category		=		1						;
	pCategory->name.Format( "%d번타입" , pCategory->category );
	
	ASSERT( pCategory->category < 16 );

	// 소팅하면서 저장함..
	AddCategory( pCategory );
	m_pSelectedCategory	= pCategory;

	Invalidate( FALSE );

	OnTilewindowEditCategory();
}

void CUITileList_TileTabWnd::OnTilewindowDeleteCategory() 
{
	if( IDNO == MessageBox( "카테고리 정말로 지우실꺼예요? -_-" , "맵툴" , MB_YESNO ) )
		return;

	// 현재 선택돼어있는 카테고리를 삭제함

	// 1, 내용이 들어 있으면 들어있는 타일들을 모두 디폴트 카테고리로 옮기고
	//     - 디폴트 카테고리의 빈자리가 현재 타일 갯수보다 모자라면 에러처리함.
	// 2, 현재 카테고리를 삭제한다.	

	DisplayMessage( "현재 카테고리를 삭제합니다. 들어있던 타일들 역시 모두 같이 삭제 됍니다." );

	// 1,해당 카테고리 삭제.
	// 2,들어있던 타일 정보 삭제..
	// 3,텍스쳐 리스트에서 제거..
	// 4,사용돼고 있는 맵데이타를 검사해서 , 디폴트 타일로 교환함.

	AuNode< stTileInfo > * pNode;
	stTileInfo	* pTileInfo;

	while( pNode = m_pSelectedCategory->list.GetHeadNode() )
	{
		pTileInfo = & pNode->GetData();

		// 비트맵 제거..
		if( pTileInfo->pBmp )
		{
			pTileInfo->pBmp->Release();
			delete pTileInfo->pBmp;
			pTileInfo->pBmp	= NULL;
		}

		// 맵에서 타일 정보 삭제.

			// 아직 하지 않음 -_-;

		// 텍스쳐 리스트에서 제거.
		AGCMMAP_THIS->m_TextureList.DeleteTexture( pTileInfo->index , TRUE );

		m_pSelectedCategory->list.RemoveHead();
	}

	// 카테고리 삭제..
	AuNode< stTileCategory * > *	pNodeCategory = m_listCategory.GetHeadNode();

	while( pNodeCategory )
	{
		if( pNodeCategory->GetData() == m_pSelectedCategory )
		{
			m_listCategory.RemoveNode( pNodeCategory );
			break;
		}
		m_listCategory.GetNext( pNodeCategory );
	}

	Invalidate( FALSE );

	SaveScript();
}

void CUITileList_TileTabWnd::OnTilewindowEditCategory() 
{
	// 카테고리 정보를 열어서 보여준다.
	// 

	if( m_pSelectedCategory == NULL )
	{
		DisplayMessage( AEM_ERROR , "EditCategory : 선택돼어있는 카테고리가 없어서 취소돼었습니다." );
		return;
	}

	CTileEditCategoryDlg dlg;
	dlg.m_nType		= m_pSelectedCategory->category;
	dlg.m_strName	= m_pSelectedCategory->name;

	if( dlg.DoModal() == IDOK )
	{
		m_pSelectedCategory->name = dlg.m_strName;
		Invalidate( FALSE );
	}

	SaveScript();
}

CUITileList_TileTabWnd::stTileCategory *	CUITileList_TileTabWnd::GetCategoryFromPoint	( CPoint	point )
{
	// 화면 좌표로 카테고리를 찾는다.

	if( point.x < TILECATEGORY_WIDTH )
	{
		// 어느 카테고리가 선택 돼었느냐...
		AuNode< stTileCategory * > *	pNode = m_listCategory.GetHeadNode();
		stTileCategory *				pRef			;
		int								count = 0		;
		CRect							rectCategory	;

		while( pNode )
		{
			pRef = pNode->GetData();

			// Rect 설정.
			rectCategory.left	= 0										;
			rectCategory.top	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * count			;
			rectCategory.right	= TILECATEGORY_WIDTH					;
			rectCategory.bottom	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * ( count + 1 )	;

			if( rectCategory.PtInRect( point ) )
			{
				return pRef;
			}

			m_listCategory.GetNext( pNode );
			count++;
		}
	}

	return NULL;
}

CUITileList_TileTabWnd::stTileInfo *		CUITileList_TileTabWnd::GetTileFromPoint		( CPoint	point )
{
	// 화면 좌표로 타일 정보를 찾는다.
	if( point.x > TILECATEGORY_WIDTH && m_pSelectedCategory )
	{
		AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
		stTileInfo	* pTileInfo;

		int		count		;
		int		line		;
		int		offset		;
		CRect	rectTileBox	;
		CRect	rectOutLine	;
		CRect	rectTmp		;

		count	= 0;
		line	= 0;
		offset	= 0;

		while( pNode )
		{
			pTileInfo = & pNode->GetData();

			// 라인 설정.
			line	= count / TILECATEGORY_COUNTPERLINE;
			offset	= count % TILECATEGORY_COUNTPERLINE;

			// Rect 설정.
			rectTileBox.SetRect(
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset		),
				TILELAYER_HEIGHT +		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line		),
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset	+ 1	),
				TILELAYER_HEIGHT +		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line	+ 1	)
				);

			// 정보 출력.
			// 아웃라인 박스를 구함..
			rectOutLine.left	= rectTileBox.left		- TILECATEGORY_TILEWMARGIN;
			rectOutLine.top		= rectTileBox.top		- TILECATEGORY_TILEHMARGIN;
			rectOutLine.right	= rectTileBox.right		+ TILECATEGORY_TILEWMARGIN;
			rectOutLine.bottom	= rectTileBox.bottom	+ TILECATEGORY_TILEHMARGIN;

			if( rectOutLine.PtInRect( point ) )
			{
				return pTileInfo;
			}


			m_pSelectedCategory->list.GetNext( pNode );
			count ++;
		}
	}

	return NULL;
}

void CUITileList_TileTabWnd::OnDropFiles(HDROP hDropInfo) 
{
	char	lpDroppedFilename[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	char	copyfilename[ 1024 ];
	int		category , index;
	int		count = 0;
	stTileInfo	*pTi;
	AcTextureList::TextureInfo * pTLInfo;
	
	
	// 우선 몇개의 파일이 드래그 되는 지 확인한다.
	UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpDroppedFilename , 1024 );

	if( m_pSelectedCategory == NULL )
	{
		DisplayMessage( AEM_ERROR , "카테고리가 선택돼어있지 않으므로 작업을 계속 할 수 없습니다.");
		return;
	}

	// 현재 선택됀 카테고리 타입 이용.
	category	= m_pSelectedCategory->category;

	if( dragcount > 0 )
	{
		// 모두다 검사 하려면
		for ( int i = 0 ; i < ( int ) dragcount ; i ++ )
		{
			::DragQueryFile( hDropInfo , i , lpDroppedFilename , 1024 );

			// 여기서 드롭된 파일로 어떤식으로 클라이언트 데이타를 생성할것인지 정한다.
			// Dialog를 띄우고 
			// 텍스쳐 파일을 조합하여 이미지를 생성한다.

			TRACE( "드롭된 %d 번째 파일 -'%s'\n" , i + 1 , lpDroppedFilename );

			// Tile 디렉토리에 같은 파일이 있는지 검사한다.
			// Tile 디렉토리로 카피해 넣는다.
			_splitpath( lpDroppedFilename , drive, dir, fname, ext );

			wsprintf( copyfilename , "%s\\Map\\Tile\\%s%s" , ALEF_CURRENT_DIRECTORY , fname , ext );

			CConvertTextureDlg	dlg;
			dlg.m_strFileNameOrigin		= copyfilename		;
			dlg.m_strFileOriginalPath	= lpDroppedFilename	;
			dlg.m_strName				= fname				;
			if( dlg.DoModal() == IDOK )
			{
				// 다시 얻어냄..
				_splitpath( ( LPCTSTR ) dlg.m_strFileOriginalPath , drive, dir, fname, ext );

				if( CopyFile( ( LPCTSTR ) dlg.m_strFileOriginalPath , ( LPCTSTR )dlg.m_strFileNameOrigin , FALSE ) == FALSE )
				{
					// 파일 카피에러!..
					DisplayMessage( AEM_ERROR , "\"%s%s\" 파일이 이미 존재하므로 카피해 넣을 수 없습니다." , fname , ext );
					DisplayMessage( AEM_WARNNING , "다음 파일을 처리를 계속 합니다." );
					continue;
				}

				// 카테고리, 코멘트, 타입을 가지고 리스트에 추가한다.
				// 타입, 인덱스를 생성한다.
				// 빈 인덱스 얻기..
				{
					AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
					stTileInfo	*pTiNext;

					index = 1;
					while( pNode )
					{
						pTi		= & pNode->GetData();
						if( pNode->GetNextNode() )
						{
							pTiNext = &pNode->GetNextNode()->GetData();
							if( GET_TEXTURE_INDEX( pTi->index ) + 1 != GET_TEXTURE_INDEX( pTiNext->index ) )
							{
								// 빈칸이다!;;;
								index = GET_TEXTURE_INDEX( pTi->index ) + 1;
								break;
							}
						}
						else
						{
							index = GET_TEXTURE_INDEX( pTi->index ) + 1;
						}
						m_pSelectedCategory->list.GetNext( pNode );
					}
					// 끝장!
				}
				// 텍스쳐 리스트에 추가함.
				
				wsprintf( copyfilename , "Map\\Tile\\%s%s" , fname , ext );
				pTLInfo = AGCMMAP_THIS->m_TextureList.AddTexture( dlg.m_nDimension , category , index , dlg.m_nType , copyfilename , ( LPSTR ) ( LPCTSTR ) dlg.m_strName );
				
				// 현재 선택됀 카테고리에 타일을 추가한다.
				// 텍스쳐 리스트도 업데이트 시키고 프리뷰 이미지도 생성한다.
				switch( GET_TEXTURE_TYPE( pTLInfo->nIndex ) )
				{
				case TT_FLOORTILE	:
				case TT_ONE			:
				case TT_ALPHATILE	:
					{
						// 한개만 등록.
						AddTile( pTLInfo->nIndex , pTLInfo->strComment );
						pTLInfo->nProperty	= dlg.m_nProperty;
					}
					break;
				case TT_UPPERTILE	:
					{
						// 갯수별로 등록.

						// 지금은 잠시 - -;

						// 조만간 추가하게될거심.
						CString	str;
						for( int y = 0 ; y < 4 ; y ++ )
						{
							for( int x = 0 ; x < 4 ; x ++ )
							{
								str.Format( "%s %d" , pTLInfo->strComment , y * 4 + x );
								AddTile( pTLInfo->nIndex + y * 4 + x , (LPSTR) (LPCTSTR) str );
							}
						}					
					}
					break;
				}
				
				count ++;

				SaveSetChangeTileList();
			}
			else
			{
				// 카피한 파일 삭제..
				// 루프종료.
				DeleteFile( copyfilename );
				break;
			}
		}
	}

	// 다돼따
	DisplayMessage( AEM_NOTIFY , "%d개의 이미지를 타일로 추가하였습니다." , count );
	Invalidate( FALSE );

	CWnd::OnDropFiles(hDropInfo);
}

void CUITileList_TileTabWnd::OnTilewindowEditTile() 
{
	if( m_pSelectedTile )
	{
		// 타일 선택이 바뀌었음을 메인윈도우에게 알려준다.
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()						;
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( m_pSelectedTile->index )	;
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= m_pSelectedTile->applyalpha		;
		Invalidate( FALSE );
	}

	AcTextureList::TextureInfo * pInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( m_pSelectedTile->index );
	if( m_pSelectedTile && pInfo )
	{
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

		_splitpath( pInfo->strFilename , drive, dir, fname, ext );

		CConvertTextureDlg	dlg;
		dlg.m_strFileNameOrigin		= pInfo->strFilename;
		dlg.m_strFileOriginalPath	= pInfo->strFilename;
		dlg.m_strName				= pInfo->strComment	;

		dlg.m_nType					= GET_TEXTURE_TYPE		( pInfo->nIndex );
		dlg.m_nDimension			= GET_TEXTURE_DIMENSION	( pInfo->nIndex );
		dlg.m_bDisableType			= TRUE;
		dlg.m_nProperty				= pInfo->nProperty	;

		if( dlg.DoModal() == IDOK )
		{
			pInfo->nIndex = SET_TEXTURE_DIMENSION( pInfo->nIndex , dlg.m_nDimension );
			strncpy( pInfo->strComment , ( LPCTSTR ) dlg.m_strName , 256 );

			pInfo->nProperty		= dlg.m_nProperty;

			m_pSelectedTile->name	= dlg.m_strName;
			m_pSelectedTile->index	= pInfo->nIndex;

			g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()						;
			g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( m_pSelectedTile->index )	;
			g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= m_pSelectedTile->applyalpha		;

			if( dlg.m_bTextureUpdate )
			{
				char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
				_splitpath( ( LPCTSTR) dlg.m_strFileNameOrigin , drive , dir , fname , ext );
				wsprintf( pInfo->strFilename , "Map\\Tile\\%s%s" , fname , ext );
				// 프리뷰 다시 생성..
				AGCMMAP_THIS->m_TextureList.MakeTempBitmap	( pInfo );
				AGCMMAP_THIS->m_TextureList.LoadTexture		( pInfo , TRUE );

				LoadBitmap( m_pSelectedTile , TRUE );
			}
			Invalidate( FALSE );

			SaveSetChangeTileList();
		}
	}
	
}

void CUITileList_TileTabWnd::OnTilewindowDeleteTile() 
{
	if( m_pSelectedTile )
	{
		// 타일 선택이 바뀌었음을 메인윈도우에게 알려준다.
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()						;
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( m_pSelectedTile->index )	;
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= m_pSelectedTile->applyalpha		;
		Invalidate( FALSE );
	}

	// 타일 삭제함.
	if( m_pSelectedTile && IDYES == MessageBox( "타일 정말로 지우실꺼예요? -_-" , "맵툴" , MB_YESNO ) )
	{
		AcTextureList::TextureInfo * pInfo;

		pInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( m_pSelectedTile->index );

		// 파일 지움.
		// 마고자 (2004-05-31 오후 1:05:16) : 
		// 성진씨 요청으로 파일 삭제하지 않음.
//		char filename[ 256 ];
//		wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , pInfo->strFilename );
//		DeleteFile( filename );

		// 텍스쳐 리스트에서 삭제
		AGCMMAP_THIS->m_TextureList.DeleteTexture( m_pSelectedTile->index , TRUE );

		// 정보 삭제.

		AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
		stTileInfo	* pTi;
		while( pNode )
		{
			pTi = &pNode->GetData();

			if( pTi == m_pSelectedTile )
			{
				m_pSelectedCategory->list.RemoveNode( pNode );
				break;
			}

			m_pSelectedCategory->list.GetNext( pNode );
		}

		m_pSelectedTile = NULL;

		Invalidate( FALSE );

		DisplayMessage( AEM_NOTIFY , "타일을 삭제하였습니다." );

		SaveSetChangeTileList();
	}
}

void CUITileList_TileTabWnd::OnTilewindowChangeTile() 
{
	if( g_pMainFrame->m_Document.m_listSelectedTileIndex.GetCount() == 2 &&
		IDYES == MessageBox( "타일 교체는 타일 2개만 선택하셔야합니다 진행 할래요?" , "맵툴" , MB_YESNO ) )
	{
		UINT32	uTiled , uReplace;

		uTiled		= g_pMainFrame->m_Document.m_listSelectedTileIndex[ 0 ];
		uReplace	= g_pMainFrame->m_Document.m_listSelectedTileIndex[ 1 ];
		
		AcTextureList::TextureInfo * pInfoTiled		;
		AcTextureList::TextureInfo * pInfoReplace	;

		pInfoTiled		= AGCMMAP_THIS->m_TextureList.GetTextureInfo( uTiled	);
		pInfoReplace	= AGCMMAP_THIS->m_TextureList.GetTextureInfo( uReplace	);

		CString	str;
		str.Format( "'%s(%s)' 타일을 '%s(%s)'타일로 덮어써버립니다. 괜찮으시겠어요?" ,
			pInfoTiled->strComment		, pInfoTiled->strFilename	,
			pInfoReplace->strComment	, pInfoReplace->strFilename	);

		if( IDYES == MessageBox( str , "맵툴" , MB_YESNO ) )
		{
			AGCMMAP_THIS->SetChangeTile( uTiled , uReplace );
			AGCMMAP_THIS->UpdateAll();
			AGCMMAP_THIS->SetChangeTile( 0 , 0 );
		}

		Invalidate( FALSE );
	}
}

void CUITileList_TileTabWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown( nFlags , point );
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CUITileList_TileTabWnd::SelectTile(int tileindex)
{
	int	nonmaskindex;
	nonmaskindex = MAKE_TEXTURE_INDEX( GET_TEXTURE_DIMENSION( tileindex ) , GET_TEXTURE_CATEGORY( tileindex ) , GET_TEXTURE_INDEX( tileindex ) , GET_TEXTURE_TYPE( tileindex ) , 0 );

	AuNode< stTileCategory * >	*	pNode		= m_listCategory.GetHeadNode();
	AuNode< stTileInfo >		*	pNodeTile	;
	stTileCategory				*	pRef		;
	stTileInfo					*	pTileInfo	= NULL;

	while( pNode )
	{
		pRef = pNode->GetData();

		if( pRef->category == GET_TEXTURE_CATEGORY( nonmaskindex ) )
		{
			m_pSelectedCategory = pRef;

			pNodeTile = m_pSelectedCategory->list.GetHeadNode();
			while( pNodeTile )
			{
				pTileInfo = & pNodeTile->GetData();

				if( pTileInfo->index == nonmaskindex )
				{
					m_pSelectedTile	= pTileInfo;
					break;
				}
				m_pSelectedCategory->list.GetNext( pNodeTile );
			}
			break;
		}
		m_listCategory.GetNext( pNode );
	}

	if( pTileInfo )
	{
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll();
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( nonmaskindex );
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= pTileInfo->applyalpha	;
	}
}

void CUITileList_TileTabWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	InvalidateRect( m_rectCursor , FALSE );
	
	CWnd::OnTimer(nIDEvent);
}

void CUITileList_TileTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if( cx * cy > 0 )
	{
		CDC	*pDC = GetDC();
		m_MemBitmap.DeleteObject();
		m_MemBitmap.CreateCompatibleBitmap( pDC , cx , cy );
		m_MemDC.SelectObject( m_MemBitmap );

		ReleaseDC( pDC );
	}
}

void CUITileList_TileTabWnd::ChangeLayer( int layer )
{
	if( m_nSelectedLayer == layer ) return;

	g_MainWindow.OnTileLayerChange( layer , m_nSelectedLayer );
	
	m_nSelectedLayer = layer;
	g_pMainFrame->m_Document.m_nCurrentTileLayer	= m_nSelectedLayer	;
	Invalidate();
}

void CUITileList_TileTabWnd::UpdateDimensino	()
{
	// 마고자 (2004-05-29 오후 10:16:34) : 디멘젼 표시부분만 업데이트

	CRect	rect;
	GetClientRect( rect );

	rect.top = rect.bottom - 20;

	InvalidateRect( rect , FALSE );
}

BOOL CUITileList_TileTabWnd::IsUsedTileName	( stTileInfo * pTInfo )
{
	ASSERT( NULL != pTInfo );
	if( NULL == pTInfo ) return FALSE;

	RwTexDictionary *pTexDic = g_pcsAgcmResourceLoader->GetDefaultTexDict();
	ASSERT( NULL != pTexDic );

	if( pTexDic )
	{
		char	strExportName[ 1024 ];
		VERIFY( AGCMMAP_THIS->m_TextureList.GetExportFileName( strExportName , pTInfo->index ) );

		RwTexture * pTexture = RwTexDictionaryFindNamedTexture( pTexDic , strExportName );
	
		if( pTexture )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}