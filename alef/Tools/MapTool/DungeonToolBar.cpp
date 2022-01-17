// DungeonToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "MainFrm.h"
#include "MyEngine.h"
#include "DungeonToolBar.h"
#include "DungeonWnd.h"

extern CMainFrame *		g_pMainFrame;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *		g_pMainFrame;
extern COLORREF	__ColorTable[ MAX_DUNGEON_HEIGHT_COUNT + 1 ];

#define TOOL_START_Y		100
#define TOOL_ITEM_HEIGHT	30

#define TOOL_ITEM_COLOR				RGB( 25 , 25 , 25 )
#define TOOL_ITEM_COLOR_SELECTED	RGB( 255 , 25 , 25 )

/////////////////////////////////////////////////////////////////////////////
// CDungeonToolBar

CDungeonToolBar::CDungeonToolBar()
{
	m_uCurrentTemplateIndex	= -1;
	m_nToolItemCount	= 0;
	m_nCurrentToolItem	= 1;
	m_nCurrentLayer		= 0;
}

CDungeonToolBar::~CDungeonToolBar()
{
}


BEGIN_MESSAGE_MAP(CDungeonToolBar, CWnd)
	//{{AFX_MSG_MAP(CDungeonToolBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDungeonToolBar message handlers

void CDungeonToolBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 172 , 172 , 255 ) );

	CRect	rectItem;

	rectItem.left	= 0;
	rectItem.right	= rect.right;

	// 마고자 (2004-09-09 오후 4:12:28) : 템플릿 표시..
	rectItem.top	= DTM_COMBO_HEIGHT		;
	rectItem.bottom	= DTM_COMBO_HEIGHT + 30	;

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	// 템플릿 정보 긁어 오기 .. 화면에 표시..
	// 
	CString	str;
	CDungeonTemplate			* pTemplate = NULL;

	if( m_uCurrentTemplateIndex <= 0 )
	{
		str.Format( "No Template" );
	}
	else
	{
		pTemplate = g_pMainFrame->m_pDungeonWnd->GetTemplate( m_uCurrentTemplateIndex );

		if( pTemplate )
		{
			str.Format( pTemplate->strName );
		}
		else
		{
			str.Format( "에러-_-" );
		}
	}

	dc.DrawText( str , rectItem , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	
	for ( int i = 0 ; i < ( int ) m_nToolItemCount ; ++ i )
	{
		rectItem.top	= TOOL_START_Y + i * TOOL_ITEM_HEIGHT			;
		rectItem.bottom	= TOOL_START_Y + ( i + 1 ) * TOOL_ITEM_HEIGHT	;

		if( i == m_nCurrentToolItem )
		{
			dc.FillSolidRect( rectItem , TOOL_ITEM_COLOR_SELECTED );
		}
		else
		{
			dc.FillSolidRect( rectItem , TOOL_ITEM_COLOR );
		}

		dc.DrawText( m_aToolItem[ i ].strName , rectItem , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}

	// 레이어 표시..

	if( pTemplate )
	{
		m_rectLayerSelect.left		= 5											;
		m_rectLayerSelect.top		= rectItem.bottom + 10						;
		m_rectLayerSelect.right		= rectItem.right - 5						;
		m_rectLayerSelect.bottom	= m_rectLayerSelect.top + DUNGEON_LAYER_HEIGHT * MAX_DUNGEON_HEIGHT_COUNT;
		
		dc.FillSolidRect( m_rectLayerSelect , RGB( 0 , 0 , 20 ) );

		CRect	rectBox = m_rectLayerSelect;
		CString	str;
		dc.SetBkMode( TRANSPARENT );

		for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			rectBox.top		= m_rectLayerSelect.top + DUNGEON_LAYER_HEIGHT * i;
			rectBox.bottom	= m_rectLayerSelect.top + DUNGEON_LAYER_HEIGHT * ( i + 1 );

			if( m_nCurrentLayer == i )
			{
				dc.FillSolidRect( rectBox , RGB( 255 , 255  , 255 ) );
				CRect	rect2 = rectBox;
				rect2.DeflateRect( 3 , 3 , 3 , 3 );
				dc.FillSolidRect( rect2 , RGB( 0 , 0 , 0 ) );
			}
			else
			{
				dc.FillSolidRect( rectBox , __ColorTable[ i ] );
			}

			str.Format( "%.0fM" , pTemplate->afSampleHeight[ i ] / 100.0f );
			dc.DrawText( str , rectBox , DT_CENTER | DT_VCENTER | DT_SINGLELINE );			
		}

	}
	// Do not call CWnd::OnPaint() for painting messages

	DrawButtons( &dc );
}

void CDungeonToolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// 세로 옵셋 계산해서 메뉴 찾아냄..
	// 해당 옵션 선택 & 화면 갱신..


	INT32 nButton = GetPressedButton( point.x , point.y );
	PressButton( nButton );

	if( nButton >= 0 )
	{
		switch( nButton )
		{
		case IDC_TEMPLATE_BUTTON:
			{
				CDungeonTemplateSelectDlg	dlg;
				if( IDOK == dlg.DoModal() )
				{
					// 템플릿 리스트 업데이트,
					// 커런트 템플릿 인덱스 업데이트

					m_uCurrentTemplateIndex	= dlg.m_nTemplateID;

					if( g_pMainFrame->m_pDungeonWnd->SaveTemplate() )
					{
						// do nothing
					}
					else
					{
						MD_SetErrorMessage( "던젼 템플릿 저장에 실패했어요" );
					}

					Invalidate( FALSE );

					// 높이 구하기..
					UpdateDungeonWallHeight( dlg.m_nTemplateID , TRUE );
				}

				ReleaseButton();
			}
			break;

		case IDC_APPLY_DUNGEON:
			{
				switch( MessageBox( "던젼 바닥 높이 조정을 하시겠습니까?" , "던전적용" , MB_YESNOCANCEL ) )
				{
				case IDYES	:	g_pMainFrame->m_pDungeonWnd->PostMessage( WM_DUNGEON_UPDATE , TRUE	); break;
				case IDNO	:	g_pMainFrame->m_pDungeonWnd->PostMessage( WM_DUNGEON_UPDATE , FALSE	); break;
				default:
					break;
				}
			}
			break;
		}

		CWnd::OnLButtonDown(nFlags, point);
		return;
	}
	
	if( point.y < TOOL_START_Y ) return ;

	int nHeight = point.y - TOOL_START_Y;
	UINT32	uOffset = ( UINT32 ) nHeight / TOOL_ITEM_HEIGHT;

	if( uOffset < m_nToolItemCount )
	{
		SelectItem( uOffset );
	}
	else
	{
		// 레이어 선택..
		if( m_rectLayerSelect.PtInRect( point ) )
		{
			uOffset = ( point.y - m_rectLayerSelect.top ) / ( DUNGEON_LAYER_HEIGHT );

			if( uOffset >= 0 && uOffset < MAX_DUNGEON_HEIGHT_COUNT )
			{
				SetLayer( uOffset );
			}
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

BOOL	CDungeonToolBar::SelectItem( UINT32 uItem )
{
	ASSERT( uItem < m_nToolItemCount );

	if( m_nCurrentToolItem == uItem )
	{
		switch( m_aToolItem[ uItem ].uItemIndex )
		{
		case TBM_DOME4:
			{
				BOOL bDisplayDome = g_pMainFrame->m_Document.m_uAtomicFilter & CAlefMapDocument::AF_OBJECT_DOME;

				if( bDisplayDome )
				{
					g_pMainFrame->m_Document.m_uAtomicFilter &= ~CAlefMapDocument::AF_OBJECT_DOME;
				}
				else
				{
					g_pMainFrame->m_Document.m_uAtomicFilter |= CAlefMapDocument::AF_OBJECT_DOME;
				}

				TRACE( "천장 표시 토글\n" );
			}
			break;
		default:
			break;
		}
	}
	else
	{
		if( uItem >= m_nToolItemCount	) return FALSE;
		if( uItem == m_nCurrentToolItem	) return FALSE;

		UINT32	nPrev = m_nCurrentToolItem;
		m_nCurrentToolItem = uItem;

		// 마고자 (2004-09-09 오후 2:38:03) : 각 아이템 초기화 과정..
		switch( m_aToolItem[ uItem ].uItemIndex )
		{
		case	TBM_FLOOR	:	// 바닥
		case	TBM_WALL2	:	// 벽
		case	TBM_WALL4	:	// 벽
		case	TBM_FENCE	:	// 난간
		case	TBM_STAIRS	:	// 계단
		case	TBM_SLOPE	:	// 경사
		case	TBM_PILLAR	:	// 기둥
		case	TBM_SELECT	:	// 선택 또는 맥스값..
		default:
			break;
		}

		Invalidate( FALSE );

		// 마고자 (2004-09-09 오후 2:41:46) : Parent Notify
		g_pMainFrame->m_pDungeonWnd->PostMessage( WM_DUNGEONTOOLBAR_ITEM_CHANAGE , m_aToolItem[ nPrev ].uItemIndex , m_aToolItem[ uItem ].uItemIndex );
		g_pMainFrame->m_pDungeonWnd->Invalidate( FALSE );

		return TRUE;
	}

	return FALSE;
}

int CDungeonToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// 툴이미지를 리소스로 부터 로딩..

	if( FALSE ) // m_bmpTool.LoadBitmap( IDB_DUNGEON_TOOL_ITEM ) )
	{
		// do nothing..
	}
	else
	{
		// 
	}

	// 메뉴등록..
	//VERIFY( AddDungeonToolItem( TBM_SELECT	, "선택"	) );
	VERIFY( AddDungeonToolItem( TBM_FLOOR	, "바닥"	) );
	VERIFY( AddDungeonToolItem( TBM_WALL2	, "벽"		) );
	//VERIFY( AddDungeonToolItem( TBM_WALL4	, "벽4"		) );
	VERIFY( AddDungeonToolItem( TBM_STAIRS	, "계단"	) );
	VERIFY( AddDungeonToolItem( TBM_PILLAR	, "기둥"	) );
	//VERIFY( AddDungeonToolItem( TBM_FENCE	, "난간"	) );
	//VERIFY( AddDungeonToolItem( TBM_SLOPE	, "경사"	) );
	VERIFY( AddDungeonToolItem( TBM_DOME	, "작은천장") );
	VERIFY( AddDungeonToolItem( TBM_DOME4	, "큰천장"	) );

	// 초기메뉴 설정..

	AddButton( IDC_TEMPLATE_BUTTON , CRect( 0, DTM_COMBO_H_START, DTM_TOOL_MENU_WIDTH, DTM_COMBO_HEIGHT ) , "템플릿" );
	AddButton( IDC_APPLY_DUNGEON , CRect( 0, 0, DTM_TOOL_MENU_WIDTH, DTM_COMBO_H_START ) , "Apply" );
	
	// 템플릿 리스트 등록..
	// 페어런트를 통해서 , 등록된 템플릿을 모두 넣는다..
	
	return 0;
}

BOOL	CDungeonToolBar::AddDungeonToolItem( UINT32 uItemIndex , CString strName )
{
	ASSERT( uItemIndex <= TBM_SELECT );
	ASSERT( m_nToolItemCount < 8 );

	if( m_nToolItemCount >= 8 )
	{
		TRACE( "개수초과\n" );
		return FALSE;
	}

	m_aToolItem[ m_nToolItemCount ].uItemIndex	= uItemIndex;
	m_aToolItem[ m_nToolItemCount ].strName		= strName	;

	m_nToolItemCount++;
	return TRUE;
}

LRESULT CDungeonToolBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
//	#define	CW_CLICKED_BUTTON( uID )	if( uID == LOWORD( wParam ) && BN_CLICKED == HIWORD( wParam ) )
//	if( WM_COMMAND == message )
//	{
//		CW_CLICKED_BUTTON( IDC_TEMPLATE_BUTTON )
//		{
//			CDungeonTemplateSelectDlg	dlg;
//			if( IDOK == dlg.DoModal() )
//			{
//				// 템플릿 리스트 업데이트,
//				// 커런트 템플릿 인덱스 업데이트
//
//				m_uCurrentTemplateIndex	= dlg.m_nTemplateID;
//
//				if( g_pMainFrame->m_pDungeonWnd->SaveTemplate() )
//				{
//					// do nothing
//				}
//				else
//				{
//					MD_SetErrorMessage( "던젼 템플릿 저장에 실패했어요" );
//				}
//
//				Invalidate( FALSE );
//			}
//		}
//		CW_CLICKED_BUTTON( IDC_APPLY_DUNGEON )
//		{
//			// 변화량 적용..
//			g_pMainFrame->m_pDungeonWnd->PostMessage( WM_DUNGEON_UPDATE );
//		}
//	}
//	
	return CWnd::WindowProc(message, wParam, lParam);
}

UINT32	CDungeonToolBar::GetItem()
{
	return m_aToolItem[ m_nCurrentToolItem ].uItemIndex;
}

INT32	CDungeonToolBar::SetLayer( INT32 nLayer )
{
	ASSERT( nLayer < MAX_DUNGEON_HEIGHT_COUNT );
	{
		m_nCurrentLayer = nLayer;

		Invalidate( FALSE );
	}
	return m_nCurrentLayer;
}

void CDungeonToolBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseButton();
	
	CWnd::OnLButtonUp(nFlags, point);
}

FLOAT	CDungeonToolBar::GetHeight()
{
	CDungeonTemplate * 	pTemplate = g_pMainFrame->m_pDungeonWnd->GetTemplate( m_uCurrentTemplateIndex );

	if( pTemplate )
	{
		return pTemplate->afSampleHeight[ GetLayer() ];
	}
	else
	{
		return 0.0f;
	}
}

FLOAT	CDungeonToolBar::UpdateDungeonWallHeight( UINT32 uDungeonTemplateID , BOOL bUpdateForce )
{
	CDungeonTemplate * pTemplate = g_pMainFrame->m_pDungeonWnd->GetTemplate( uDungeonTemplateID );

	if( pTemplate )
	{
		if( bUpdateForce )
		{

		}
		else
		{
			if( pTemplate->bDomeHeight ) return pTemplate->fDomeHeight;
		}

		pTemplate->bDomeHeight = TRUE;

		ApdObjectTemplate	*	pstApdObjectTemplate	;
		AgcdObjectTemplate	*	pstAgcdObjectTempte		;

		FLOAT	fHeight = 0.0f;
		pTemplate->fDomeHeight = 0.0f;
		RpClump	* pClump;

		pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate( pTemplate->uTemplateIndex[ TBM_WALL2 ][ 0 ].uIndex );
		if( pstApdObjectTemplate )
		{
			pstAgcdObjectTempte		= g_pcsAgcmObject->GetTemplateData( pstApdObjectTemplate );

			pClump = g_pcsAgcmObject->LoadClump( pstAgcdObjectTempte->m_stGroup.m_pstList->m_csData.m_pszDFFName , pstAgcdObjectTempte->m_szCategory );

			fHeight					= __GetClumpHeight( pClump );

			if( fHeight > pTemplate->fDomeHeight )
			{
				pTemplate->fDomeHeight = fHeight;
			}

			RpClumpDestroy( pClump );
		}

		pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate( pTemplate->uTemplateIndex[ TBM_WALL2 ][ 1 ].uIndex );
		if( pstApdObjectTemplate )
		{
			pstAgcdObjectTempte		= g_pcsAgcmObject->GetTemplateData( pstApdObjectTemplate );

			pClump = g_pcsAgcmObject->LoadClump( pstAgcdObjectTempte->m_stGroup.m_pstList->m_csData.m_pszDFFName , pstAgcdObjectTempte->m_szCategory );

			fHeight					= __GetClumpHeight( pClump );

			if( fHeight > pTemplate->fDomeHeight )
			{
				pTemplate->fDomeHeight = fHeight;
			}

			RpClumpDestroy( pClump );
		}

		pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate( pTemplate->uTemplateIndex[ TBM_WALL2 ][ 2 ].uIndex );
		if( pstApdObjectTemplate )
		{
			pstAgcdObjectTempte		= g_pcsAgcmObject->GetTemplateData( pstApdObjectTemplate );

			pClump = g_pcsAgcmObject->LoadClump( pstAgcdObjectTempte->m_stGroup.m_pstList->m_csData.m_pszDFFName , pstAgcdObjectTempte->m_szCategory );

			fHeight					= __GetClumpHeight( pClump );

			if( fHeight > pTemplate->fDomeHeight )
			{
				pTemplate->fDomeHeight = fHeight;
			}

			RpClumpDestroy( pClump );
		}

		return pTemplate->fDomeHeight;
	}
	else
	{
		return 0.0f;
	}

}