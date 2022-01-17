// MapSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "ApmMap.h"
#include "MapSelectDlg.h"
#include ".\mapselectdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapSelectDlg dialog

#define MAP_SELECT_DLG_MARGIN	8
#define MAP_SELECT_DLG_MARGIN_Y	25

CMapSelectDlg::CMapSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapSelectDlg::IDD, pParent) ,
	m_nX_array( 0 ),
	m_nY_array( 0 ),
	m_nX2_array( 0 ),
	m_nY2_array( 0 )
{
	//{{AFX_DATA_INIT(CMapSelectDlg)
	m_nX		= 0				;
	m_nY		= 0				;
	m_nX2		= 0				;
	m_nY2		= 0				;
	m_strInfo	= _T("")		;
	m_bUseEffect = FALSE;
	//}}AFX_DATA_INIT

	m_nType			= LOAD_NORMAL	;
}


void CMapSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapSelectDlg)
	DDX_Control(pDX, IDC_LOADTYPECOMBO, m_ctlLoadTypeCombo);
	DDX_Control(pDX, IDC_MAP_SELECT_CONTROL, m_MapSelectStatic);
	DDX_Text(pDX, IDC_MAP_X, m_nX);
	DDX_Text(pDX, IDC_MAP_Y, m_nY);
	DDX_Text(pDX, IDC_MAP_X2, m_nX2);
	DDX_Text(pDX, IDC_MAP_Y2, m_nY2);
	DDX_Text(pDX, IDC_INFO, m_strInfo);
	DDX_Check(pDX, IDC_EFFECT, m_bUseEffect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CMapSelectDlg)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_LOADALL, OnLoadall)
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TOGGLEFIELD, OnTogglefield)
	ON_BN_CLICKED(IDC_EXPORTMODE, OnExportmode)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SAVESELECTION, OnBnClickedSaveselection)
	ON_BN_CLICKED(IDC_LOADSELECTION, OnBnClickedLoadselection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapSelectDlg message handlers

BOOL CMapSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_MapSelectStatic.Init();

	CRect	rectButton;
	
	CRect	rectMap;
	rectMap.left	= MAP_SELECT_DLG_MARGIN;
	rectMap.top		= MAP_SELECT_DLG_MARGIN;
	rectMap.right	= rectMap.left + 768;
	rectMap.bottom	= rectMap.top + 768;
	m_MapSelectStatic.MoveWindow( rectMap.left , rectMap.top , 768 , 768 );

	// 마고자 (2004-06-28 오후 5:47:06) : 버튼의 크기를 구함..
	// 마고자 (2004-06-28 오후 6:08:26) : 이거 널이 나오는 경우가 있을까 -_-;
	// 그냥 에러처리 안함..
	GetDlgItem( IDOK )->GetClientRect( rectButton );

	CRect	rectWindow;
	CRect	rectClient;
	CRect	rectButtonNew;

	GetWindowRect( rectWindow	);
	GetClientRect( rectClient	);

	// 프레임 크기 계산 .
	rectWindow.right	-= rectClient.Width	();
	rectWindow.bottom	-= rectClient.Height();

	// 왼쪽 마진..
	rectWindow.right	+= MAP_SELECT_DLG_MARGIN	;
	rectWindow.bottom	+= MAP_SELECT_DLG_MARGIN	;

	// 맵 셀렉트 창 크기 넣음..
	rectWindow.right	+= rectMap.Width	() + MAP_SELECT_DLG_MARGIN	;
	rectWindow.bottom	+= rectMap.Height	() + MAP_SELECT_DLG_MARGIN_Y;

	rectButtonNew.left	= rectWindow.right;

	// 버튼 크기 넣음..
	rectWindow.right	+= rectButton.Width	() + MAP_SELECT_DLG_MARGIN	;
	
	MoveWindow( rectWindow , FALSE );

	CWnd * pWnd;

#define CONTROL_UPDATE( nControl ) \
	pWnd = GetDlgItem( nControl );\
	pWnd->GetWindowRect( rectButton );\
	ScreenToClient( rectButton );\
	rectButtonNew.top		= rectButton.top;\
	rectButtonNew.right		= rectButtonNew.left + rectButton.Width();\
	rectButtonNew.bottom	= rectButton.top + rectButton.Height();\
	pWnd->MoveWindow( rectButtonNew );

	CONTROL_UPDATE( IDOK				);
	CONTROL_UPDATE( IDC_EXPORT			);
	CONTROL_UPDATE( IDCANCEL			);
	CONTROL_UPDATE( IDC_TOGGLEFIELD		);
	CONTROL_UPDATE( IDC_SAVESELECTION	);
	CONTROL_UPDATE( IDC_LOADSELECTION	);

	CONTROL_UPDATE( IDC_EFFECT			);

	if( m_nX_array	== 0 && 
		m_nY_array	== 0 && 
		m_nX2_array	== 0 && 
		m_nY2_array	== 0
		)
	{
		m_nX_array	= 0									;
		m_nY_array	= 0									;
		m_nX2_array	= 0 + ALEF_PREVIEW_MAP_SELECT_SIZE	;
		m_nY2_array	= 0 + ALEF_PREVIEW_MAP_SELECT_SIZE	;
	}

	CButton	* pButton = ( CButton * ) GetDlgItem( IDC_TOGGLEFIELD );
	ASSERT( NULL != pButton );
	if( NULL == pButton )
	{
		MessageBox( "-_-;;;;;;;;;;;;;;;;;;" );
		return 0;
	}

	switch( m_nMode )
	{
	case	NORMAL_LOADING_SELECT	:
		{
			m_MapSelectStatic.m_nSelectedIndexX = m_nX_array;
			m_MapSelectStatic.m_nSelectedIndexY = m_nY_array;			

			GetDlgItem( IDC_SAVESELECTION )->EnableWindow( FALSE );
			GetDlgItem( IDC_LOADSELECTION )->EnableWindow( FALSE );
		}
		break;
	}

	m_MapSelectStatic.SetMode( m_nMode );

	int	nMap = m_MapSelectStatic.GetMapFromPos( m_nX_array , m_nY_array );

	m_MapSelectStatic.SetMap( nMap );

	switch( m_MapSelectStatic.GetMap() )
	{
	case CMapSelectStatic::WORLD_MAP	:	pButton->SetWindowText( "Dungeon Field" );	break;
	//case CMapSelectStatic::DUNGEON_MAP	:	pButton->SetWindowText( "World Field" );	break;
	default:
		break;
	}

	m_nX		= (int)m_nX_array	- ( int ) ALEF_MAX_HORI_SECTOR / 2	;
	m_nY		= (int)m_nY_array	- ( int ) ALEF_MAX_VERT_SECTOR / 2	;
	m_nX2		= (int)m_nX2_array	- ( int ) ALEF_MAX_HORI_SECTOR / 2	;
	m_nY2		= (int)m_nY2_array	- ( int ) ALEF_MAX_VERT_SECTOR / 2	;

	m_ctlLoadTypeCombo.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CMapSelectDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message )
	{
	case WM_MAPFOCUSCHANGED:
		{
			m_nX_array	= wParam										;
			m_nY_array	= lParam										;
			m_nX2_array	= wParam + ALEF_PREVIEW_MAP_SELECT_SIZE			;
			m_nY2_array	= lParam + ALEF_PREVIEW_MAP_SELECT_SIZE			;

			m_nX		= (int)m_nX_array	- ( int ) ALEF_MAX_HORI_SECTOR / 2	;
			m_nY		= (int)m_nY_array	- ( int ) ALEF_MAX_VERT_SECTOR / 2	;
			m_nX2		= (int)m_nX2_array	- ( int ) ALEF_MAX_HORI_SECTOR / 2	;
			m_nY2		= (int)m_nY2_array	- ( int ) ALEF_MAX_VERT_SECTOR / 2	;

			UpdateData( FALSE );
		}
		break;
	case WM_MAPSELECTED:
		{
			m_nX_array	= wParam										;
			m_nY_array	= lParam										;
			m_nX2_array	= wParam + ALEF_PREVIEW_MAP_SELECT_SIZE			;
			m_nY2_array	= lParam + ALEF_PREVIEW_MAP_SELECT_SIZE			;
				
			m_nX		= ( int ) m_nX_array	- ( int ) ALEF_MAX_HORI_SECTOR / 2	;
			m_nY		= ( int ) m_nY_array	- ( int ) ALEF_MAX_VERT_SECTOR / 2	;
			m_nX2		= ( int ) m_nX2_array	- ( int ) ALEF_MAX_HORI_SECTOR / 2	;
			m_nY2		= ( int ) m_nY2_array	- ( int ) ALEF_MAX_VERT_SECTOR / 2	;
			UpdateData( FALSE );
			OnOK();
		}
		break;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CMapSelectDlg::OnExport() 
{
	/*
	CButton	* pButton = ( CButton * ) GetDlgItem( IDC_EXPORT );
	ASSERT( NULL != pButton );
	if( NULL == pButton )
	{
		MessageBox( "-_-;;;;;;;;;;;;;;;;;;" );
		return;
	}

	m_nType	= ( m_nType + 1 ) % 2;

	switch( m_nType )
	{
	case LOAD_NORMAL	:	pButton->SetWindowText( "To Event Mode" );	break;
	case LOAD_EXPORT	:	pButton->SetWindowText( "To NormalMode" );	break;
	default:
		break;
	}

	Invalidate( FALSE );
	*/
}

void CMapSelectDlg::OnLoadall() 
{
//	m_nType		= LOAD_ALL				;
//	m_nX_array	= 0						;
//	m_nY_array	= 0						;
//	m_nX2_array	= ALEF_MAX_HORI_SECTOR	;
//	m_nY2_array	= ALEF_MAX_VERT_SECTOR	;
//
//	OnOK();
}

BOOL CMapSelectDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_MapSelectStatic.ChangeSelectSize( ( int ) zDelta );
	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CMapSelectDlg::OnOK() 
{
//	switch( m_ctlLoadTypeCombo.GetCurSel() )
//	{
//	case 0:
//		// Normal
//		break;
//	case 1:
//		// Export
//		m_nType		= LOAD_EXPORT			;
//		break;
//	}
	
	CDialog::OnOK();
}

void CMapSelectDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CBrush		*pBrush = NULL;
	CString		strType	= "Default";

	pBrush = &ALEF_BRUSH_LOAD_NORMAL		;

	/*
	switch( m_nType )
	{
	case LOAD_NORMAL	:
		{
			pBrush = &ALEF_BRUSH_LOAD_NORMAL		;
			strType = "Normal Mode( Enable full functions accessing )";
		}
		break;
	case LOAD_EXPORT	:
		{
			pBrush = &ALEF_BRUSH_LOAD_EXPORT		;
			strType = "Event Mode ( Disable for geometry editing )";
		}
		break;
	default:
		break;
	}
	*/

	CRect	rect;
	GetClientRect( rect );
	dc.FillRect( rect , pBrush );

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 0 ) );
	dc.TextOut( 10 , rect.Height() - 25 , strType );
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CMapSelectDlg::OnTogglefield() 
{
	/*
	m_MapSelectStatic.SetMap( !m_MapSelectStatic.GetMap() );

	CButton	* pButton = ( CButton * ) GetDlgItem( IDC_TOGGLEFIELD );
	ASSERT( NULL != pButton );
	if( NULL == pButton )
	{
		MessageBox( "-_-;;;;;;;;;;;;;;;;;;" );
		return;
	}

	switch( m_MapSelectStatic.GetMap() )
	{
	case CMapSelectStatic::WORLD_MAP	:	pButton->SetWindowText( "Dungeon Field" );	break;
	case CMapSelectStatic::DUNGEON_MAP	:	pButton->SetWindowText( "World Field" );	break;
	default:
		break;
	}

	*/
}

void CMapSelectDlg::OnExportmode() 
{
	/*
	m_nType	= LOAD_AUTOEXPORTER;

	OnOK();	
	*/
}

int CMapSelectDlg::DoModal( int nMode )
{
	m_nMode = nMode;
	return CDialog::DoModal();
}
void CMapSelectDlg::OnBnClickedSaveselection()
{
	// 셀렉션 저장..
	CString szFileName;
	CFileDialog dlg(FALSE,_T("INI"),_T("*.INI"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Export Division File 파일 (*.INI)|*.INI|죄다 (*.*)|*.*|"));

	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	if( IDOK == dlg.DoModal() )
	{
		szFileName=dlg.GetPathName();
		VERIFY( SaveSelection( szFileName ) );
	}

	SetCurrentDirectory( strDir );
}

BOOL	CMapSelectDlg::SaveSelection( CString strFilename )
{
	FILE	* pFile = fopen( ( LPCTSTR ) strFilename , "wt" );
	if( NULL == pFile )
	{
		// MessageBox( "파일 오픈실패~ 프로텍션 체크하시고 다시 시도하셈" );
		return FALSE;
	}
	else
	{
		vector <int>::iterator Iter;
		int nValue;

		for ( Iter = m_MapSelectStatic.m_vectorDivision.begin( ) ; Iter != m_MapSelectStatic.m_vectorDivision.end( ) ; Iter++ )
		{
			nValue = *Iter;

			fprintf( pFile , "[%04d]\n" , nValue );
		}

		fclose( pFile );

		return TRUE;
	}
}

BOOL	CMapSelectDlg::LoadSelection( CString strFilename )
{
	FILE	* pFile = fopen( ( LPCTSTR ) strFilename , "rt" );

	if( NULL == pFile )
	{
		return FALSE;
	}
	else
	{
		this->m_MapSelectStatic.m_vectorDivision.clear();

		INT32	nValue;
		while( EOF != fscanf( pFile , "[%04d]\n" , &nValue) )
		{
			AddSelection( nValue );
		}

		fclose( pFile );
		return TRUE;
	}
}

BOOL	CMapSelectDlg::AddSelection( INT32 nDivision )
{
	this->m_MapSelectStatic.m_vectorDivision.push_back( nDivision );
	return TRUE;
}

void CMapSelectDlg::OnBnClickedLoadselection()
{
	CString szFileName;
	CFileDialog dlg(TRUE,_T("INI"),_T("*.INI"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Export Division File 파일 (*.INI)|*.INI|죄다 (*.*)|*.*|"));

	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	if( IDOK == dlg.DoModal() )
	{
		if( LoadSelection( dlg.GetPathName() ) )
		{
			Invalidate( FALSE );
		}
		else
		{
			MessageBox( "파일 오픈실패~ 프로텍션 체크하시고 다시 시도하셈" );
		}
	}

	SetCurrentDirectory( strDir );
}
