// WorldMapSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "WorldMapSelectDlg.h"
#include "WorldMapEditDlg.h"

// CWorldMapSelectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorldMapSelectDlg, CDialog)
CWorldMapSelectDlg::CWorldMapSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldMapSelectDlg::IDD, pParent) 
{
	m_nMapIndex	= -1;
	m_bListGetChanged = false;
}

CWorldMapSelectDlg::~CWorldMapSelectDlg()
{
}

void CWorldMapSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	DDX_Control(pDX, IDC_MAP_DISPLAY, m_ctlDisplay);
}


BEGIN_MESSAGE_MAP(CWorldMapSelectDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_SAVE_MAP_LIST, OnBnClickedSaveMapList)
	ON_LBN_SELCHANGE(IDC_LIST, OnLbnSelchangeList)
END_MESSAGE_MAP()


// CWorldMapSelectDlg 메시지 처리기입니다.

BOOL CWorldMapSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 어레이 복사해둠.
	// 어레이는 소팅 되어 있어야함.
	m_arrayWorldMap = g_pcsApmMap->m_arrayWorldMap;

	UpdateList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWorldMapSelectDlg::OnBnClickedOk()
{
	if( IsListChanged() )
	{
		switch( MessageBox( "월드탭 리스트에 변경점이 있는데 저장하시겠어요?" , "월드맵" , MB_YESNOCANCEL ) )
		{
		case IDYES:
			OnBnClickedSaveMapList();

			// 리컬시브 콜
			OnBnClickedOk(); 
			return;

		case IDNO:
			// do nothing
			break;

		case IDCANCEL:
			// 작업 멈춤.
			return;
		}
	}

	int	nIndex;
	nIndex	= m_ctlList.GetCurSel();

	if( nIndex >= 0 )
	{
		CString	str;
		m_ctlList.GetText( nIndex , str );

		int	nWorldMap = atoi( (LPCTSTR) str );
		
		m_nMapIndex = nWorldMap;
	}

	OnOK();
}

void CWorldMapSelectDlg::UpdateList()
{
	g_pcsApmMap->m_arrayWorldMap = m_arrayWorldMap;

	m_ctlList.ResetContent();

	CString	str;
	ApmMap::WorldMap	* pWorldMap;
	for( int i = 0 ; i < ( int ) m_arrayWorldMap.size() ; i ++ )
	{
		pWorldMap = &m_arrayWorldMap[ i ];

		str.Format( SKY_TEMPLATE_FORMAT , pWorldMap->nMID , pWorldMap->strComment ); 
		m_ctlList.AddString( str );
	}
}

void CWorldMapSelectDlg::OnBnClickedAdd()
{
	ApmMap::WorldMap	* pWorldMap;

	int	nNewMapIndex = 0;
	for( int i = 1 ; i < 9999 ; i ++ )
	{
		int j;
		for( j = 0 ; j < ( int ) m_arrayWorldMap.size() ; j ++ )
		{
			pWorldMap = &m_arrayWorldMap[ j ];

			if( pWorldMap->nMID == i )
			{
				// 중복된게 있음.
				break;
			}
		}

		if( j == m_arrayWorldMap.size() )
		{
			// 이놈이거덩~
			nNewMapIndex = i;
			break;
		}
	}

	if( 0 == nNewMapIndex )
	{
		MessageBox( "월드맵의 갯수가 9999개를 초과했거나 다른 문제가 있음둥!" );
		return;
	}

	ApmMap::WorldMap	stWorldMap;

	stWorldMap.nMID	= nNewMapIndex;
	m_arrayWorldMap.push_back( stWorldMap );

	SetListChange();

	UpdateList();

	pWorldMap =  &m_arrayWorldMap[ m_arrayWorldMap.size() - 1 ];

	// 수정 메뉴를 부른다.
	CWorldMapEditDlg	dlg;
	dlg.m_pstWorldMap	= pWorldMap;
	if( IDOK == dlg.DoModal() )
	{
		// 성공..

		/*
		pWorldMap->xStart		= dlg.m_fStartX	;
		pWorldMap->zStart		= dlg.m_fStartZ	;
		pWorldMap->xEnd			= dlg.m_fEndX	;
		pWorldMap->zEnd			= dlg.m_fEndZ	;
		pWorldMap->nMapItemID	= dlg.m_nIndex	;

		strncpy( pWorldMap->strComment , (LPCTSTR) dlg.m_strComment , WORLDMAP_COMMENT_LENGTH - 1 ); 
		*/
		UpdateList();
	}
}

ApmMap::WorldMap * CWorldMapSelectDlg::GetMap( int nMap )
{
	ApmMap::WorldMap	* pWorldMap;

	for( int j = 0 ; j < ( int ) m_arrayWorldMap.size() ; j ++ )
	{
		pWorldMap = &m_arrayWorldMap[ j ];

		if( pWorldMap->nMID == nMap )
		{
			// 중복된게 있음.
			return pWorldMap;
		}
	}

	return NULL;
}

void CWorldMapSelectDlg::OnBnClickedEdit()
{
	// 선택된거 ID 알아내고..

	int	nIndex;
	nIndex	= m_ctlList.GetCurSel();

	CString	str;
	m_ctlList.GetText( nIndex , str );

	int	nWorldMap = atoi( (LPCTSTR) str );

	ApmMap::WorldMap	* pWorldMap = GetMap( nWorldMap );

	ASSERT( NULL != pWorldMap );

	// 수정 메뉴를 부른다.
	CWorldMapEditDlg	dlg;
	dlg.m_pstWorldMap	= pWorldMap;
	if( IDOK == dlg.DoModal() )
	{
		// 성공..

		UpdateList();
		SetListChange();

		m_ctlDisplay.SetWorldMap( nWorldMap , pWorldMap->xStart , pWorldMap->zStart , pWorldMap->xEnd , pWorldMap->zEnd );
	}
}

void CWorldMapSelectDlg::OnBnClickedRemove()
{
	int	nIndex;
	nIndex	= m_ctlList.GetCurSel();

	CString	str;
	m_ctlList.GetText( nIndex , str );

	int	nWorldMap = atoi( (LPCTSTR) str );

	ApmMap::WorldMap	* pWorldMap = GetMap( nWorldMap );

	ASSERT( NULL != pWorldMap );

	m_arrayWorldMap.erase( pWorldMap );

	UpdateList();

	m_ctlList.SetCurSel( -1 );

	SetListChange();
}

void CWorldMapSelectDlg::OnBnClickedSaveMapList()
{
	// 복사해넣기..
	g_pcsApmMap->m_arrayWorldMap = m_arrayWorldMap;
	// 월드맵 인포 저장.
	g_pcsApmMap->SaveWorldMap( "Ini\\" WORLDMAPTEMPLATE );
	MessageBox( "저장 OK~" );

	m_bListGetChanged = false;
}

void CWorldMapSelectDlg::OnLbnSelchangeList()
{
	int nIndex = m_ctlList.GetCurSel();
	int nCount = m_ctlList.GetCount();
	if ((nIndex != LB_ERR) && (nCount > 1))
	{
		// 월드맵 업데이트..
		CString	str;
		m_ctlList.GetText( nIndex , str );
		int	nWorldMap = atoi( (LPCTSTR) str );

		ApmMap::WorldMap	* pWorldMap = GetMap( nWorldMap );

		// 월드맵 로딩.
		m_ctlDisplay.SetWorldMap( nWorldMap , pWorldMap->xStart , pWorldMap->zStart , pWorldMap->xEnd , pWorldMap->zEnd );
	}
}
// C:\Alef\Tools\RegionTool\WorldMapSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "WorldMapSelectDlg.h"

// CWorldMapDisplayStatic

IMPLEMENT_DYNAMIC(CWorldMapDisplayStatic, CStatic)
CWorldMapDisplayStatic::CWorldMapDisplayStatic():m_nWorldMap( -1 ),m_fStepSize( 1.0f ), m_pImage( NULL )
{
}

CWorldMapDisplayStatic::~CWorldMapDisplayStatic()
{
	ReleaseImage();
}

BOOL	CWorldMapDisplayStatic::SetWorldMap( int nWorldMapIndex , FLOAT fStartX , FLOAT fStartZ , FLOAT fEndX , FLOAT fEndZ )
{
	Invalidate( FALSE );

	m_pointOffset.SetPoint( 0 , 0 );

	// 이미지 로딩..
	if( m_nWorldMap != nWorldMapIndex )
	{
		// From AgcmMinimap.h
		#define AGCMMAP_WORLDMAP_TEST			"wmap%04d"
		#define AGCMMAP_WORLDMAP_FOLDER_DEFAULT	"texture\\worldmap\\"

		// 월드맵 파일 로디잉.
		// m_pImage->Clear();
		m_bmpMap.Release();
		ReleaseImage();
		AllocImage();

		CString	strFileName;
		strFileName.Format( AGCMMAP_WORLDMAP_TEST , nWorldMapIndex );
		CString	strFullPath;
		strFullPath.Format( "%s%s.png" , AGCMMAP_WORLDMAP_FOLDER_DEFAULT , strFileName );


		// Png 라이브러리가 완성될때까지...
		{
			CString	bmpPath;
			bmpPath.Format( "%s.bmp" , strFileName );
			if( m_bmpMap.Load( (LPSTR)(LPCTSTR) bmpPath , NULL ) )
			{
				m_nWorldMap = nWorldMapIndex;
				m_fStartX	= fStartX	;
				m_fStartZ	= fStartZ	;
				m_fEndX		= fEndX		;
				m_fEndZ		= fEndZ		;

				// 계산값..가로만 기준으로 계산함..
				m_fStepSize	= ( fEndX - fStartX ) / 800.0f;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		/*
		if( m_pImage->Load( strFullPath , CXIMAGE_FORMAT_PNG) )
		{
			m_nWorldMap = nWorldMapIndex;
			m_fStartX	= fStartX	;
			m_fStartZ	= fStartZ	;
			m_fEndX		= fEndX		;
			m_fEndZ		= fEndZ		;

			// 계산값..가로만 기준으로 계산함..
			m_fStepSize	= ( fEndX - fStartX ) / 800.0f;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		*/
	}
	else
	{
		m_fStartX	= fStartX	;
		m_fStartZ	= fStartZ	;
		m_fEndX		= fEndX		;
		m_fEndZ		= fEndZ		;
		return TRUE;
	}
}


BEGIN_MESSAGE_MAP(CWorldMapDisplayStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CWorldMapDisplayStatic 메시지 처리기입니다.


void CWorldMapDisplayStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	static CDC _sMemDC;
	static CBitmap _sMemBitmap;
	static BOOL _bFirst = TRUE;

	CRect	rect;
	GetClientRect( rect );

	if( _bFirst )
	{
		_bFirst = FALSE;
		_sMemBitmap.CreateCompatibleBitmap( &dc , rect.Width() , rect.Height() );
		_sMemDC.CreateCompatibleDC( &dc );
		_sMemDC.SelectObject( _sMemBitmap );
	}

	_sMemDC.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );

	_sMemDC.SetTextColor( RGB( 255 , 255 , 255 ) );
	_sMemDC.SetBkMode( TRANSPARENT );

	if( m_nWorldMap != -1 )
	{
		//m_pImage->Draw( _sMemDC.GetSafeHdc() , m_pointOffset.x , m_pointOffset.y );
		m_bmpMap.Draw( _sMemDC.GetSafeHdc() , m_pointOffset.x , m_pointOffset.y );
	}
	else
	{
		_sMemDC.TextOut( 0 , 0 , "해당 월드맵 파일이 없어요" );
	}

	CPen	pen;
	pen.CreatePen( PS_SOLID , 1 , RGB( 0 , 0 , 255 ) );
	_sMemDC.SelectObject( pen );

	BOOL	bShowGrid = TRUE;
	if( bShowGrid )
	{
		CRect	rectDivision;
		UINT32	uDivision;

		int	nSectorX;
		int	nSectorZ;
		FLOAT	fStartX;
		FLOAT	fStartZ;

		CString	str;

		int		nWidth = ( int ) ( ( MAP_SECTOR_WIDTH * ( FLOAT ) MAP_DEFAULT_DEPTH ) / m_fStepSize );
		for( int i = 0 ; i < MAX_DIVISION ; ++i )
		{
			for( int j = 0 ; j < MAX_DIVISION ; ++j )
			{
				uDivision = MakeDivisionIndex( i  , j );

				nSectorX = ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( uDivision ) );
				nSectorZ = ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( uDivision ) );

				fStartX	= GetSectorStartX( nSectorX );
				fStartZ	= GetSectorStartX( nSectorZ );

				rectDivision.left	= PosToScreenX( fStartX );
				rectDivision.top	= PosToScreenY( fStartZ );
				rectDivision.right	= rectDivision.left	+ nWidth;
				rectDivision.bottom	= rectDivision.top	+ nWidth;

				if( rectDivision.right	>= 0 &&
					rectDivision.bottom >= 0 &&
					rectDivision.left	<= rect.Width() &&
					rectDivision.top	<= rect.Height() )
				{
					_sMemDC.MoveTo( rectDivision.left , rectDivision.bottom );
					_sMemDC.LineTo( rectDivision.left , rectDivision.top );
					_sMemDC.LineTo( rectDivision.right , rectDivision.top );	

					str.Format( "%d" , uDivision );
					_sMemDC.DrawText( str , rectDivision , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
				}
			}
		}
	}

	dc.BitBlt( 0 , 0 , rect.Width() , rect.Height() , &_sMemDC , 0 , 0 , SRCCOPY );
}

int	CWorldMapDisplayStatic::PosToScreenX( FLOAT fX )
{
	//double	duStepSize = ( double ) ( m_fEndX - m_fStartX ) / 800.0;

	//return static_cast<int>( ( double ) ( fX - m_fStartX ) / duStepSize ) + m_pointOffset.x;

	return static_cast<int>(( fX - m_fStartX ) / m_fStepSize ) + m_pointOffset.x;
}
int	CWorldMapDisplayStatic::PosToScreenY( FLOAT fZ )
{
	//double	duStepSize = ( double ) ( m_fEndX - m_fStartX ) / 800.0;

	//return static_cast<int>( ( double ) ( fZ - m_fStartZ ) / duStepSize ) + m_pointOffset.y;

	return static_cast<int>(( fZ - m_fStartZ ) / m_fStepSize ) + m_pointOffset.y;
}

void CWorldMapDisplayStatic::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CStatic::OnRButtonDown(nFlags, point);
}

void CWorldMapDisplayStatic::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CStatic::OnRButtonUp(nFlags, point);
}

void CWorldMapDisplayStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint	_sPrevPoint = point;

	if( nFlags & MK_RBUTTON )
	{
		m_pointOffset -= ( _sPrevPoint - point );
		Invalidate( FALSE );
	}

	_sPrevPoint = point;

	CStatic::OnMouseMove(nFlags, point);
}
