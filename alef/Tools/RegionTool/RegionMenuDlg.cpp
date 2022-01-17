// RegionMenuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"

#include "RTMenuRegion.h"
#include "RTMenuSpawn.h"
#include "RTMenuSky.h"
#include "RTMenuDistance.h"
#include "RTMenuBlock.h"

#include "GoToDlg.h"

#include "UploadDataDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD	IDC_COSTUM_MENU_START = 33001;

/////////////////////////////////////////////////////////////////////////////
// CRegionMenuDlg dialog

CRegionMenuDlg * CRTMenuBase::m_pParent = NULL;
FLOAT	CRTMenuBase::m_fScale = 1.0f;

CRegionMenuDlg::CRegionMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegionMenuDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegionMenuDlg)
	//}}AFX_DATA_INIT

	m_nCurrentMenu = REGIONTOOLMODE_REGION;

	m_pMainDlg = NULL;
}


void CRegionMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionMenuDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegionMenuDlg, CDialog)
	//{{AFX_MSG_MAP(CRegionMenuDlg)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_GOTO, OnGoto)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BITMAPUPDATE, OnBitmapupdate)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionMenuDlg message handlers

void CRegionMenuDlg::OnClose() 
{
	// 공짜론 안닫힘..
	return;
	
	// CDialog::OnClose();
}

BOOL	CRegionMenuDlg::AddMenu( CRTMenuBase * pMenu )
{
	ASSERT( NULL != pMenu );

	// 중복체크는 따로 안함..
	// 버튼 생성..
	// 포인터 설정..
	// 텍스트 설정..
	// 벡터 추가 끝.

	CButton	* pButton = new CButton;
	CRect	rect( 0 , 0 , 0 , 0 );
	pButton->Create(
		pMenu->GetMenuName() ,
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_PUSHLIKE ,
		rect ,
		this ,
		IDC_COSTUM_MENU_START + m_vectorMenuWnd.size()
	);

	MenuInfo	menuInfo;
	menuInfo.pButton	= pButton	;
	menuInfo.pMenu		= pMenu		;

	m_vectorMenuWnd.push_back( menuInfo );
	return TRUE;
}

BOOL CRegionMenuDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 포인터 설정..
	m_pMainDlg = ( CRegionToolDlg * ) GetParent();
	CRTMenuBase::m_pParent = this;
	
	// 마고자 (2004-11-11 오후 4:05:05) : 
	// 메뉴의 이치를 저장해둠..
	CWnd * pWnd = GetDlgItem( IDC_MENUWND );
	CRect	rect;
	pWnd->GetClientRect( rect );
	pWnd->ClientToScreen( rect );
	ScreenToClient( rect );

	pWnd->DestroyWindow();

	// 서브 윈도우를 설치함..
	AddMenu( new CRTMenuRegion	);
	AddMenu( new CRTMenuSpawn	);
	AddMenu( new CRTMenuSky		);
	AddMenu( new CRTMenuDistance);
	AddMenu( new CRTMenuBlock	);
	
	{
		CProgressDlg	dlg;

		dlg.StartProgress( "데이타 로딩중..." , m_vectorMenuWnd.size() , this );
		
		for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
		{
			dlg.SetProgress( i );

			if( m_vectorMenuWnd[ i ].pMenu )
			{
				if( !m_vectorMenuWnd[ i ].pMenu->Create( &rect , this ) )
				{
					DWORD uError = GetLastError();
					TRACE( "%d\n" , uError );
				}

				// 데이타 로드..
				m_vectorMenuWnd[ i ].pMenu->OnLoadData();
			}
		}

		dlg.EndProgress();
	}

	SelectMenu( 0 );

	if( g_bTeleportOnlyMode )
	{
		GetDlgItem( IDOK				)->EnableWindow( FALSE );
		GetDlgItem( IDC_BITMAPUPDATE	)->EnableWindow( FALSE );
	}

	SetWindowControlPosition();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void	CRegionMenuDlg::SelectMenu( int nOffset )
{
	m_nCurrentMenu = nOffset;

	for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
	{
		if( m_vectorMenuWnd[ i ].pMenu )
		{
			m_vectorMenuWnd[ i ].pMenu->ShowWindow( SW_HIDE );
		}
		if( m_vectorMenuWnd[ i ].pButton )
		{
			m_vectorMenuWnd[ i ].pButton->SetCheck( FALSE );
			m_vectorMenuWnd[ i ].pMenu->OnDeActiveRegionMenu();
		}
	}

	if( m_vectorMenuWnd[ m_nCurrentMenu ].pMenu )
	{
		m_vectorMenuWnd[ m_nCurrentMenu ].pMenu->ShowWindow( SW_SHOW );
	}
	if( m_vectorMenuWnd[ m_nCurrentMenu ].pButton )
	{
		m_vectorMenuWnd[ m_nCurrentMenu ].pButton->SetCheck( TRUE );
		m_vectorMenuWnd[ m_nCurrentMenu ].pMenu->OnActiveRegionMenu();
	}

	GetParent()->PostMessage( WM_REGIONMENU_UPDATE );
	InvalidateRegionView();
}

void CRegionMenuDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
	{
		if( m_vectorMenuWnd[ i ].pMenu )
		{
			m_vectorMenuWnd[ i ].pMenu->DestroyWindow();
			delete m_vectorMenuWnd[ i ].pMenu;
			m_vectorMenuWnd[ i ].pMenu = NULL;
		}
		if( m_vectorMenuWnd[ i ].pButton )
		{
			m_vectorMenuWnd[ i ].pButton->DestroyWindow();
			delete m_vectorMenuWnd[ i ].pButton;
			m_vectorMenuWnd[ i ].pButton = NULL;
		}
	}

	m_vectorMenuWnd.clear();
}

void CRegionMenuDlg::OnGoto() 
{
	CGoToDlg dlg;

	if( IDOK == dlg.DoModal() )
	{
		m_pMainDlg->m_ctlRegion.MoveTo( ( int ) dlg.m_uDivision );
	}	
}

void CRegionMenuDlg::OnOK() 
{
	if( FALSE == g_bTeleportOnlyMode )
	{
		// SaveData

		for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
		{
			if( m_vectorMenuWnd[ i ].pMenu )
			{
				m_vectorMenuWnd[ i ].pMenu->OnSaveData();
			}
		}
	
		MessageBox( "Save Ok~" );
	}
}

BOOL CRegionMenuDlg::IsAvailableUnSavedData()
{
	BOOL	bUnsavedData = FALSE;
	for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
	{
		if( m_vectorMenuWnd[ i ].pMenu )
		{
			bUnsavedData = m_vectorMenuWnd[ i ].pMenu->OnQuerySaveData();

			if( bUnsavedData ) break;
		}
	}

	return bUnsavedData;
}
void CRegionMenuDlg::OnCancel() 
{
	if( FALSE == g_bTeleportOnlyMode )
	{
		if( IsAvailableUnSavedData() )
		{
			if( IDYES == MessageBox( "세이브돼지 않은 데이타가 있습니다. 종료하시겠습니까?" , "리젼툴" , MB_YESNOCANCEL ) )
			{
				GetParent()->DestroyWindow();
			}
		}
		else
		{
			// 으하하..
			GetParent()->DestroyWindow();
		}
	}
	else
	{
		// 으하하..
		CRegionToolDlg * pDlg = ( CRegionToolDlg * ) GetParent();
		pDlg->OnCancel();
	}
}

void	CRegionMenuDlg::OnDeleteKeyDown()
{
	if( FALSE == g_bTeleportOnlyMode )
	{
		if( m_vectorMenuWnd[ m_nCurrentMenu ].pMenu )
		{
			m_vectorMenuWnd[ m_nCurrentMenu ].pMenu->OnDeleteKeyDown();
		}
	}
}

BOOL CRegionMenuDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( WM_KEYDOWN == pMsg->message )	
	{
		if( pMsg->wParam == VK_ESCAPE )
		{
			// 메인윈도로 포워딩시킴..
			m_pMainDlg->OnCancel();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void	CRegionMenuDlg::InvalidateRegionView()
{
	if( m_pMainDlg )
	{
		m_pMainDlg->m_ctlRegion.Invalidate( FALSE );
	}
}

void	CRegionMenuDlg::SetCaptureRegionView()
{
	if( m_pMainDlg )
	{
		m_pMainDlg->m_ctlRegion.SetCapture();
	}
}
void	CRegionMenuDlg::ReleaseCaptureRegionView()
{
	if( m_pMainDlg )
	{
		ReleaseCapture();
	}
}


void CRegionMenuDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch( nChar )
	{
	case VK_DELETE:
		{
			OnDeleteKeyDown();
		}
		break;
	case 'G':
		{
			CGoToDlg dlg;

			if( IDOK == dlg.DoModal() )
			{
				m_pMainDlg->m_ctlRegion.MoveTo( ( int ) dlg.m_uDivision );
			}	
		}
		break;
	/*
	case '1':
		OnTabRegion();
		break;
	case '2':
		OnTabSpawn();
		break;
	case '3':
		OnTABSky();
		break;
	*/

	default:
		if( '1' <= nChar && nChar <= '9' &&
			nChar - '1' < ( int ) m_vectorMenuWnd.size() )
		{
			SelectMenu( nChar - '1' );
		}
		break;
	}
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRegionMenuDlg::OnBitmapupdate() 
{
	/*
	CUploadDataDlg dlg;

	if( IDOK == dlg.DoModal( TRUE ) )
	{
		// 비트맵을 올린다..
		if( dlg.m_bCharacterTemplate	)	FileUpload( RM_RK_DIRECTORY_BIN "Ini\\CharacterTemplate.ini");
		if( dlg.m_bAI2Template			)	FileUpload( RM_RK_DIRECTORY_BIN "Ini\\AI2Template.ini");
		if( dlg.m_bObjectTemplate		)	FileUpload( RM_RK_DIRECTORY_BIN "Ini\\ObjectTemplate.ini");
		if( dlg.m_bSpawnGroup			)	FileUpload( RM_RK_DIRECTORY_BIN "Ini\\SpawnGroup.ini");
		if( dlg.m_bSkySet				)	FileUpload( RM_RK_DIRECTORY_BIN "Ini\\" AGPMNATURE_SKYSET_FILENAME );

		if( dlg.m_bRegionTool			)
		{
			FolderUpload( RM_RK_DIRECTORY_BIN "Regiontool" );
			FileUpload( RM_RK_DIRECTORY_BIN "Ini\\" REGIONTEMPLATEFILE );
		}

		if( dlg.m_bArtist				)	FolderUpload( RM_RK_DIRECTORY_BIN "Ini\\Object\\Artist" );
		if( dlg.m_bDesign				)	FolderUpload( RM_RK_DIRECTORY_BIN "Ini\\Object\\Design" );
	}
	*/
}

void CRegionMenuDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( this->GetSafeHwnd() )
	{
		SetWindowControlPosition();
	}
}

void	CRegionMenuDlg::SetWindowControlPosition()
{
	const int nMargin		= 5	;
	const int nButtonHeight = 20;

	CRect	rectWnd , rect;
	CRect	rectInfoWnd;
	GetClientRect( rectWnd );

	CWnd * pWnd;

	int	nCurrent = nMargin;

	// 위치수정..

	rectInfoWnd.left	= nMargin;
	rectInfoWnd.right	= rectWnd.Width() - nMargin;

	// 버튼들..
	{
		for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
		{
			pWnd = m_vectorMenuWnd[ i ].pButton;
			rect.SetRect( nMargin , nCurrent , rectWnd.Width() - nMargin , nCurrent + nButtonHeight );
			if( pWnd ) pWnd->MoveWindow( rect );

			nCurrent += nButtonHeight;
		}
		// 마진 추가..
		
		nCurrent += nMargin;

		// 탑 설정..
		rectInfoWnd.top = nCurrent;
	}

	// 아래버튼.. .. 밑에서 부터 올라간다.
	{
		nCurrent = rectWnd.Height() - nMargin;

		pWnd = GetDlgItem( IDCANCEL );
		rect.SetRect( nMargin , nCurrent - nButtonHeight , rectWnd.Width() - nMargin , nCurrent );
		if( pWnd ) pWnd->MoveWindow( rect );

		nCurrent -= nButtonHeight + nMargin;

		pWnd = GetDlgItem( IDOK );
		rect.SetRect( nMargin , nCurrent - nButtonHeight , rectWnd.Width() - nMargin , nCurrent );
		if( pWnd ) pWnd->MoveWindow( rect );

		nCurrent -= nButtonHeight;

		pWnd = GetDlgItem( IDC_GOTO );
		rect.SetRect( nMargin , nCurrent - nButtonHeight , rectWnd.Width() - nMargin , nCurrent );
		if( pWnd ) pWnd->MoveWindow( rect );

		nCurrent -= nButtonHeight;

		// 마진 추가..
		
		nCurrent -= nMargin;

		// 탑 설정..
		rectInfoWnd.bottom = nCurrent;
	}

	// 윈도우 이동..
	for( int i = 0 ; i < ( int ) m_vectorMenuWnd.size() ; i ++ )
	{
		if( m_vectorMenuWnd[ i ].pMenu )
		{
			m_vectorMenuWnd[ i ].pMenu->MoveWindow( rectInfoWnd );
		}
	}
}

LRESULT CRegionMenuDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_COMMAND )
	{
		int nControlID = LOWORD(wParam) - IDC_COSTUM_MENU_START;

		if( nControlID >= 0 && nControlID < ( int )m_vectorMenuWnd.size() )
		{
			SelectMenu( nControlID );
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
