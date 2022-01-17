// UITileList_Others.cpp : implementation file
//

#include "stdafx.h"
#include "atlbase.h"
#include "maptool.h"
#include "UITileList_Others.h"
#include "Plugin_TextureInfo.h"
#include "Plugin_Blocking.h"
#include "Plugin_NPC.h"
#include "Plugin_Grass.h"
#include "Plugin_BitmapExport.h"
#include "Plugin_CameraWalk.h"
//@{ Jaewon 20050113
#include "Plugin_AmbOcclMap.h"
//@} Jaewon
#include "Plugin_NatureEffect.h"
#include "Plugin_GeometryEffect.h"
#include "Plugin_PointLight.h"
#include "Plugin_BossSpawn.h"
#include "plugin_loadblock.h"
#include "plugin_LineBlock.h"
#include "Plugin_NoLayer.h"


const	int _gnMenuCountPerLine = 5;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// CUITileList_Others

CUITileList_Others::CUITileList_Others()
{
	m_pCurrentPlugin	= NULL;
}

CUITileList_Others::~CUITileList_Others()
{
}


BEGIN_MESSAGE_MAP(CUITileList_Others, CWnd)
	//{{AFX_MSG_MAP(CUITileList_Others)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileList_Others message handlers

void CUITileList_Others::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 99 , 99 , 99 ) );

	DrawMenu( &dc );
	// Do not call CWnd::OnPaint() for painting messages
}

void CUITileList_Others::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	int nIndex = GetMenuIndex( point );

	if( nIndex  < m_listPlugin.GetCount() )
	{
		int	nOffset = 0;
		AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
		while( pNode )
		{
			if( nOffset++ == nIndex )
			{
				break;
			}

			m_listPlugin.GetNext( pNode );
		}

		if( m_pCurrentPlugin == pNode )
		{
			TRACE( "같은서 선택..\n" );
			return;
		}

		OnChangePlugin( &m_pCurrentPlugin->GetData() , &pNode->GetData() );

		// 윈도우 숨기고 보이게 하는 과정..

		Plugin *  pPlugin;
		if( m_pCurrentPlugin )
		{
			pPlugin = &m_pCurrentPlugin->GetData();

			pPlugin->pWnd->ShowWindow( SW_HIDE );
		}

		if( pNode )
		{
			CRect	rect;
			pPlugin = &pNode->GetData();
			GetClientRect( rect );

			rect.top = GetPluginTop( m_listPlugin.GetCount() - 1 ) + UITILELIST_TEXT_HEIGHT + UITILELIST_TEXT_HEIGHT;

			pPlugin->pWnd->MoveWindow( rect , FALSE );
			pPlugin->pWnd->ShowWindow( SW_SHOW );
		}

		m_pCurrentPlugin = pNode;

		Invalidate();
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

int CUITileList_Others::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CUITileList_PluginBase * pPluginWnd;
	CUITileList_PluginBase * pPluginWndBlocking;
	pPluginWnd	= new CPlugin_Water;

	AddPlugin( pPluginWnd , "워터 플로그인" , RGB( 0, 0 , 0 ) );

//	pPluginWnd	= new CPlugin_TextureInfo;
//
//	AddPlugin( pPluginWnd , "Texture Infomation(시험중)" , RGB( 0 , 0 , 0 ) );

	pPluginWndBlocking = pPluginWnd	= new CPlugin_Blocking;

	AddPlugin( pPluginWnd , "Blocking Plugin" , RGB( 0 , 0 , 0 ) );

	pPluginWnd	= new CPlugin_NPC;

	AddPlugin( pPluginWnd , "NPC Plugin(시험중)" , RGB( 0 , 0 , 0 ) );

	pPluginWnd	= new CPlugin_Grass;
	
	AddPlugin( pPluginWnd , "Grass Plugin" , RGB( 0 , 0 , 0 ) );

	pPluginWnd	= new CPlugin_BitmapExport;

	AddPlugin( pPluginWnd , "Bitmap Export Plugin Test" , RGB( 0 , 0 , 0 ) );

	pPluginWnd	= new CPlugin_CameraWalk;

	AddPlugin( pPluginWnd , "Camera Walking Plugin" , RGB( 0 , 0 , 0 ) );

	//@{ Jaewon 20050113
	pPluginWnd = new CPlugin_AmbOcclMap;

	AddPlugin( pPluginWnd , "Ambient Occlusion Map Plugin" , RGB( 0 , 0 , 0 ) );
	//@} Jaewon

	pPluginWnd = new CPlugin_GeometryEffect;
	AddPlugin( pPluginWnd , "Geometry Effect Plugin" , RGB( 0 , 0 , 0 ) );

	pPluginWnd = new CPlugin_PointLight;
	AddPlugin( pPluginWnd , "Point Light Plugin" , RGB( 0 , 0 , 0 ) );

	CPlugin_BossSpawn::GetClass();
	pPluginWnd = CPlugin_BossSpawn::m_spThis;
	AddPlugin( pPluginWnd , "Boss Spawn Plugin" , RGB( 0 , 0 , 0 ) );

	pPluginWnd = new CPlugin_LoadBlock;
	AddPlugin( pPluginWnd , "Load Block Plugin" , RGB( 0 , 0 , 0 ) );

	pPluginWnd = new CPlugin_NatureEffect;
	AddPlugin( pPluginWnd , "Nature Effect Plugin" , RGB( 0 , 0 , 0 ) );

	pPluginWnd = new CPlugin_LineBlock;
	AddPlugin( pPluginWnd , "Line Blocking Plugin" , RGB( 0 , 0 , 0 ) );

	( ( CPlugin_LineBlock * ) pPluginWnd )->m_pPluginBlocking = pPluginWndBlocking;

	pPluginWnd = new CPlugin_NoLayer;
	AddPlugin( pPluginWnd , "No Layer Plugin" , RGB( 0 , 0 , 0 ) );

	// 레지스트리 얻어내는 부분..
	Plugin				* pPlugin	;
	AuNode< Plugin >	* pNode		= m_listPlugin.GetHeadNode();
	BOOL				bSave		;
	char				strQuery[ 1024 ];
	DWORD				bInRegistry;

	CRegKey reg;

	CString str;
	str.Format( "Software\\%s" , "AlefMapTool" );
	reg.Create( HKEY_LOCAL_MACHINE , str );

	while( pNode )
	{
		pPlugin = & pNode->GetData();

		bSave	= pPlugin->pWnd->OnQuerySaveData( strQuery );
		if( bSave )
		{
			// 레지스트리 얻어냄..
			if( reg.QueryValue( bInRegistry , strQuery ) != ERROR_SUCCESS )
			{
				bInRegistry			= TRUE;
			}

			// 에헤헤..
			pPlugin->pWnd->m_bSave	= bInRegistry;
		}

		m_listPlugin.GetNext( pNode );
	}
	
	
	return 0;
}

void	CUITileList_Others::OnChangePlugin		( Plugin * pPrev , Plugin * pNext )
{
	if( pPrev ) pPrev->pWnd->OnDeSelectedPlugin();
	if( pNext ) pNext->pWnd->OnSelectedPlugin();
}

BOOL CUITileList_Others::LButtonDownGeometry	( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnLButtonDownGeometry( pPos );
	}
	else return TRUE;
}

BOOL CUITileList_Others::LButtonUpGeometry		( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnLButtonUpGeometry( pPos );
	}
	else return TRUE;
}

BOOL CUITileList_Others::LButtonDblClkGeometry	( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnLButtonDblClkGeometry( pPos );
	}
	else return TRUE;
}

BOOL CUITileList_Others::RButtonDownGeometry	( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnRButtonDownGeometry( pPos );
	}
	else return TRUE;
}
BOOL CUITileList_Others::RButtonUpGeometry		( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnRButtonUpGeometry( pPos );
	}
	else return TRUE;
}

BOOL CUITileList_Others::RButtonDblClkGeometry	( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnRButtonDblClkGeometry( pPos );
	}
	else return TRUE;
}

BOOL CUITileList_Others::MouseMoveGeometry		( RwV3d * pPos )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnMouseMoveGeometry( pPos );
	}
	else return TRUE;
}

BOOL CUITileList_Others::OnWindowRender			()
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnWindowRender();
	}
	else return TRUE;
}

BOOL CUITileList_Others::OnIdle				( UINT32 ulClockCount )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnIdle( ulClockCount );
	}
	else return TRUE;
}

BOOL CUITileList_Others::AddPlugin				( CUITileList_PluginBase * pPlugin , CString name , COLORREF color )
{
	ASSERT( NULL != pPlugin );

	// 리스트에 삽입~

	Plugin	plugin;
	plugin.color	= color		;
	plugin.name		= name		;
	plugin.pWnd		= pPlugin	;

	if (!pPlugin->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE( "윈도우생성실패\n" );
		return FALSE;
	}

	pPlugin->ShowWindow( FALSE );
	
	m_listPlugin.AddTail( plugin );

	return TRUE;
}

void CUITileList_Others::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// 플러그인 데이타 제거..
	// 레지스트리 얻어내는 부분..
	BOOL				bSave		;
	char				strQuery[ 1024 ];

	CRegKey reg;

	CString str;
	str.Format( "Software\\%s" , "AlefMapTool" );
	reg.Create( HKEY_LOCAL_MACHINE , str );

	AuNode< Plugin >	* pNode ;
	Plugin	* pPlugin;

	while( pNode = m_listPlugin.GetHeadNode() )
	{
		pPlugin	= &pNode->GetData();

		bSave	= pPlugin->pWnd->OnQuerySaveData( strQuery );
		if( bSave )
		{
			// 레지스트리 기록함..
			reg.SetValue( pPlugin->pWnd->m_bSave , strQuery	);
		}

		pPlugin->pWnd->DestroyWindow();
		delete pPlugin->pWnd;

		m_listPlugin.RemoveHead();
	}
}

void CUITileList_Others::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if( m_pCurrentPlugin )
	{
		CRect	rect;
		Plugin	* pPlugin;

		pPlugin = &m_pCurrentPlugin->GetData();
		GetClientRect( rect );

		rect.top = GetPluginTop( m_listPlugin.GetCount() ) + UITILELIST_TEXT_HEIGHT;

		pPlugin->pWnd->MoveWindow( rect , FALSE );
		pPlugin->pWnd->ShowWindow( SW_SHOW );
	}
}

BOOL CUITileList_Others::SelectObject( RpClump * pClump )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->OnSelectObject( pClump );
	}
	else return FALSE;
}


BOOL CUITileList_Others::Window_LButtonDown		( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnLButtonDown( ms );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_LButtonUp		( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnLButtonUp( ms );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_LButtonDblClk	( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnLButtonDblClk( ms );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_RButtonDown		( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnRButtonDown( ms );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_RButtonUp		( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnRButtonUp( ms );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_RButtonDblClk	( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnRButtonDblClk( ms );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_MouseWheel		( BOOL bForward )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnMouseWheel( bForward );
	}
	else return FALSE;
}

BOOL CUITileList_Others::Window_MouseMove		( RsMouseStatus *ms )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnMouseMove( ms );
	}
	else return FALSE;
}


BOOL CUITileList_Others::Window_KeyDown		( RsKeyStatus *ks )
{
	if( m_pCurrentPlugin )
	{
		Plugin *  pPlugin = &m_pCurrentPlugin->GetData();

		return pPlugin->pWnd->Window_OnKeyDown( ks );
	}
	else return FALSE;
}

BOOL CUITileList_Others::CleanUpData			()
{
	AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
	Plugin	* pPlugin;

	while( pNode )
	{
		pPlugin	= &pNode->GetData();

		pPlugin->pWnd->OnCleanUpData();

		pNode = pNode->GetNextNode();
	}

	return  TRUE;
}

BOOL CUITileList_Others::Export					( BOOL bServer )
{
	AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
	Plugin	* pPlugin;

	while( pNode )
	{
		pPlugin	= &pNode->GetData();

		pPlugin->pWnd->OnExport( bServer );

		pNode = pNode->GetNextNode();
	}

	return  TRUE;
}

void CUITileList_Others::ChangeSectorGeometry( ApWorldSector * pSector )
{
	// 마고자 (2004-06-22 오후 5:25:05) :  지형 변화를 플러그인에 통보함.

	AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
	Plugin				* pPlugin;
	while( pNode )
	{
		pPlugin = &pNode->GetData();

		if( pPlugin && pPlugin->pWnd )
			pPlugin->pWnd->OnChangeSectorGeometry( pSector );

		m_listPlugin.GetNext( pNode );
	}
}

int		CUITileList_Others::GetPluginTop( int nIndex )
{
	return UITILELIST_TEXT_HEIGHT * ( nIndex / _gnMenuCountPerLine );
}

int	CUITileList_Others::GetMenuIndex( CPoint point )
{
	CRect	rectWnd;
	GetClientRect( rectWnd );

	CRect	button;

	int	nIndexX , nIndexY;
	int	nWidth	= rectWnd.Width() / _gnMenuCountPerLine;
	int	nHeight	= UITILELIST_TEXT_HEIGHT;

	int	nOffset = 0;
	AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
	Plugin	* pPlugin;

	while( pNode )
	{
		pPlugin	= &pNode->GetData();

		// 인덱스 구하고..
		nIndexX	= nOffset % _gnMenuCountPerLine;
		nIndexY	= nOffset / _gnMenuCountPerLine;

		// 박스 크기 계산..
		button.left		= nWidth * nIndexX;
		button.right	= nWidth * ( nIndexX + 1 ) - 1;

		button.top		= nHeight * nIndexY;
		button.bottom	= nHeight * ( nIndexY + 1 ) - 1;

		if( button.PtInRect( point ) ) return nOffset;

		nOffset ++;
		m_listPlugin.GetNext( pNode );
	}

	return -1;
}

void CUITileList_Others::DrawMenu( CDC * pDC )
{
	CRect	rectWnd;
	GetClientRect( rectWnd );

	COLORREF	rgbBase		= RGB( 172 , 172 , 172 );
	COLORREF	rgbBright	= RGB( 225 , 225 , 225 );
	COLORREF	rgbDark		= RGB( 56 , 56 , 56 );

	CRect	rect , * pRect;
	CRect	button;

	pDC->SetBkMode( TRANSPARENT );

	int	nIndexX , nIndexY;
	int	nWidth	= rectWnd.Width() / _gnMenuCountPerLine;
	int	nHeight	= UITILELIST_TEXT_HEIGHT;

	int	nOffset = 0;
	AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
	Plugin	* pPlugin;

	while( pNode )
	{
		pPlugin	= &pNode->GetData();

		// 인덱스 구하고..
		nIndexX	= nOffset % _gnMenuCountPerLine;
		nIndexY	= nOffset / _gnMenuCountPerLine;

		// 박스 크기 계산..
		button.left		= nWidth * nIndexX;
		button.right	= nWidth * ( nIndexX + 1 ) - 1;

		button.top		= nHeight * nIndexY;
		button.bottom	= nHeight * ( nIndexY + 1 ) - 1;

		rect	= button;
		pRect	= &button;
		rect.DeflateRect( 2 , 2 , 2 , 2 );
		pDC->FillSolidRect( rect , rgbBase );

		if( pNode == m_pCurrentPlugin )
		{
			pDC->FillSolidRect( pRect->left , pRect->top , pRect->Width() , 2 , rgbDark);
			pDC->FillSolidRect( pRect->left , pRect->top , 2 , pRect->Height() , rgbDark);

			pDC->FillSolidRect( pRect->left , pRect->bottom - 2 , pRect->Width() , 2 , rgbBright);
			pDC->FillSolidRect( pRect->right - 2 , pRect->top , 2 , pRect->Height() , rgbBright);
		}
		else
		{
			pDC->FillSolidRect( pRect->left , pRect->top , pRect->Width() , 2 , rgbBright);
			pDC->FillSolidRect( pRect->left , pRect->top , 2 , pRect->Height() , rgbBright);

			pDC->FillSolidRect( pRect->left , pRect->bottom - 2 , pRect->Width() , 2 , rgbDark);
			pDC->FillSolidRect( pRect->right - 2 , pRect->top , 2 , pRect->Height() , rgbDark);
		}

		pDC->DrawText( pPlugin->pWnd->m_strShortName , pRect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );			

		nOffset ++;
		m_listPlugin.GetNext( pNode );
	}

	// 선택된 플러그인 세부 프로필 표시.
	if( m_pCurrentPlugin ) 
	{
		CRect	rectTitle;

		int	nTitleHeight = GetPluginTop( m_listPlugin.GetCount() - 1 ) + UITILELIST_TEXT_HEIGHT;
		rectTitle.SetRect(
			0 , nTitleHeight,
			rectWnd.Width() , nTitleHeight + UITILELIST_TEXT_HEIGHT );

		pDC->FillSolidRect( rectTitle , RGB( 255 , 0 , 0 ) );
		pDC->DrawText( m_pCurrentPlugin->GetData().name , rectTitle , DT_CENTER | DT_VCENTER | DT_SINGLELINE );			
	}

	/*
	int	nOffset = 0;
	AuNode< Plugin >	* pNode = m_listPlugin.GetHeadNode();
	Plugin	* pPlugin;

	dc.SetBkMode( TRANSPARENT );

	if( !pNode )
		dc.TextOut( 0 , 0 , "등록된 플러그인이 없습니다." );

	while( pNode )
	{
		pPlugin	= &pNode->GetData();

		if( pNode == m_pCurrentPlugin )
		{
			dc.FillSolidRect(
				0 , nOffset * UITILELIST_TEXT_HEIGHT,
				rect.Width() , UITILELIST_TEXT_HEIGHT ,
				RGB( 255 , 0 , 0 ) );
		}

		dc.SetTextColor( pPlugin->color );
		dc.TextOut( 0 , nOffset * UITILELIST_TEXT_HEIGHT,
			pPlugin->name	);

		nOffset ++;

		m_listPlugin.GetNext( pNode );
	}
	*/
}