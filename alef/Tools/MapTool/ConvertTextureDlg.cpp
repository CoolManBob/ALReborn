// ConvertTextureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ConvertTextureDlg.h"
#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConvertTextureDlg dialog


CConvertTextureDlg::CConvertTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertTextureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConvertTextureDlg)
	m_strName		= _T("");
	m_strFileName	= _T("");
	m_bShowGrid		= TRUE	;
	//}}AFX_DATA_INIT

	m_nType			= 0		;
	m_nDimension	= 2		;
	m_bDisableType	= FALSE	;
	m_bTextureUpdate= FALSE	;
	m_nProperty		= 0		;
}


void CConvertTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvertTextureDlg)
	DDX_Control(pDX, IDC_PROPERTY, m_ctlProperty);
	DDX_Control(pDX, IDC_DIMENSION, m_ctlComboDimension);
	DDX_Control(pDX, IDC_TYPE, m_ctlTypeCombo);
	DDX_Control(pDX, IDC_CATEGORY, m_ctlCategoryCombo);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	DDX_Check(pDX, IDC_SHOWGRID, m_bShowGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConvertTextureDlg, CDialog)
	//{{AFX_MSG_MAP(CConvertTextureDlg)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_WM_PAINT()
	ON_CBN_CLOSEUP(IDC_DIMENSION, OnCloseupDimension)
	ON_BN_CLICKED(IDC_SHOWGRID, OnShowgrid)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvertTextureDlg message handlers

BOOL CConvertTextureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 파일이름 분석 , 창 내용 정리.
	// 콤보 박스 정리..

	if( m_bDisableType )
	{
		m_ctlTypeCombo.EnableWindow( FALSE );
	}

	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( (LPCTSTR) m_strFileNameOrigin , drive, dir, fname, ext );

	m_strFileName.Format( "%s%s" , fname , ext );
	m_ctlTypeCombo		.SetCurSel( m_nType			);
	m_ctlComboDimension	.SetCurSel( m_nDimension - 1);
	UpdateData( FALSE );
	
	char	copyfilename[ 1024 ];
	wsprintf( copyfilename , "%s\\Map\\Tile\\%s%s" , ALEF_CURRENT_DIRECTORY , fname , ext );

	RwImage	* pImage;

	pImage = RwImageRead ( (LPCTSTR) copyfilename );

	BOOL	bImaged = FALSE;

	// 어쩔수 없다.. 에러 이미지 출력
	if( NULL != pImage )
	{
		// 생각없이 한개만 생성.
		// 로딩성공.
		char strFilename[ 1024 ];
		wsprintf( strFilename , "%s\\Map\\Temp\\__TMP__.bmp" , AGCMMAP_THIS->m_TextureList.m_strTextureDirectory );

		VERIFY( RwImageWrite( pImage , (LPCTSTR) strFilename ) );

		RwImageDestroy( pImage		);

		bImaged = m_bmpImage.Load( strFilename , NULL );
		
		DeleteFile( strFilename );
	}

	if( FALSE == bImaged )
	{
		HBRUSH	hBrush;

		m_bmpImage.Create( 50 , 50 );
		VERIFY( hBrush = CreateSolidBrush( RGB( 255 , 0 , 0 ) ) );

		RECT	rect;
		SetRect( &rect , 0 , 0 , 50 , 50 );
		::FillRect		( m_bmpImage.GetDC() , &rect , hBrush );
		::SetTextColor	( m_bmpImage.GetDC() , RGB( 0 , 0 , 0 ) );
		::SetBkMode		( m_bmpImage.GetDC() , TRANSPARENT );
		::TextOut		( m_bmpImage.GetDC() , 0 , 0 , "ERROR" , 5 );

		DeleteObject( hBrush );

		// 이미지를 못읽었으니 그리드는 표시하지 않음..
		m_bShowGrid	= FALSE;
		UpdateData( FALSE );
	}

	CWnd * pItem = GetDlgItem( IDC_DISPLAY );
	ASSERT( NULL != pItem );
	pItem->GetWindowRect( m_rectImage );
	pItem->DestroyWindow();
	ScreenToClient( m_rectImage );

	m_ctlProperty.SetCurSel( m_nProperty );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConvertTextureDlg::OnOK() 
{
	int	type;
	type = m_ctlTypeCombo.GetCurSel();

	if( type == -1 )
	{
		MessageBox( "타입을 무조건! 선택하셔야합니다." , "--+" , MB_ICONERROR | MB_OK );
		return;
	}

	// 결국 목적은 타일의 타입을 입력받는것.
	m_nType = type;

	m_nDimension = m_ctlComboDimension.GetCurSel() + 1 ;

	if( m_bTextureUpdate )
	{
		if( CopyFile( ( LPCTSTR ) m_strFileOriginalPath , ( LPCTSTR ) m_strFileNameOrigin , FALSE ) == FALSE )
		{
			MessageBox( "파일 카피 실패!" );
		}
	}

	// 프라퍼티..
	m_nProperty = m_ctlProperty.GetCurSel();

	CDialog::OnOK();
}

void CConvertTextureDlg::OnSelchangeType() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	m_nType = m_ctlTypeCombo.GetCurSel();
	
	int default_dimension;
	switch( m_nType )
	{
	case	TT_FLOORTILE	:	default_dimension =	TT_FLOOR_DEPTH	;	break;
	case	TT_UPPERTILE	:	default_dimension =	TT_UPPER_DEPTH	;	break;
	case	TT_ONE			:	default_dimension =	TT_ONE_DEPTH	;	break;
	case	TT_ALPHATILE	:	default_dimension =	TT_ALPHA_DEPTH	;	break;
	default:
		ASSERT( !"-_-++++++++" );
		default_dimension =	0;
		break;
	}

	m_ctlComboDimension.SetCurSel( default_dimension - 1 );
}

void CConvertTextureDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	dc.SetBkMode( TRANSPARENT );

	dc.Rectangle( m_rectImage.left , m_rectImage.top , m_rectImage.left + 256 , m_rectImage.top + 256 );

	m_bmpImage.Draw( dc.GetSafeHdc() , m_rectImage.left , m_rectImage.top );

	if( m_bShowGrid )
	{
		// 마고자 (2004-05-30 오후 4:38:14) :  디테일 그리드 표시..
		CPen	pen;
		pen.CreatePen( PS_SOLID , 1 , RGB( 255 , 255 , 255 ) );
		dc.SelectObject( pen );

		UpdateData( TRUE );
		m_nDimension = m_ctlComboDimension.GetCurSel() + 1 ;

		for( int i = 0 ; i < m_nDimension + 1 ; ++ i )
		{
			// 가로선..
			dc.MoveTo( m_rectImage.left		, m_rectImage.top + m_bmpImage.GetHeight() / m_nDimension * i );
			dc.LineTo( m_rectImage.left + m_bmpImage.GetWidth(), m_rectImage.top + m_bmpImage.GetHeight() / m_nDimension * i );

			// 세로선..
			dc.MoveTo( m_rectImage.left + m_bmpImage.GetWidth() / m_nDimension * i , m_rectImage.top	);
			dc.LineTo( m_rectImage.left + m_bmpImage.GetWidth() / m_nDimension * i , m_rectImage.top + m_bmpImage.GetHeight()	);
		}
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CConvertTextureDlg::OnCloseupDimension() 
{
	Invalidate( FALSE );
}

void CConvertTextureDlg::OnShowgrid() 
{
	m_bShowGrid	= ! m_bShowGrid;

	UpdateData( FALSE );

	Invalidate( FALSE );
}

void CConvertTextureDlg::OnDropFiles(HDROP hDropInfo) 
{
	char	lpDroppedFilename[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	char	copyfilename[ 1024 ];
	int		count = 0;
	
	// 우선 몇개의 파일이 드래그 되는 지 확인한다.
	UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpDroppedFilename , 1024 );

	if( dragcount > 0 )
	{
		// 처음 한개만 받음.
		::DragQueryFile( hDropInfo , 0 , lpDroppedFilename , 1024 );

		// Tile 디렉토리에 같은 파일이 있는지 검사한다.
		// Tile 디렉토리로 카피해 넣는다.
		_splitpath( lpDroppedFilename , drive, dir, fname, ext );

		wsprintf( copyfilename , "%s\\Map\\Tile\\%s%s" , ALEF_CURRENT_DIRECTORY , fname , ext );

		RwImage	* pImage;

		pImage = RwImageRead ( lpDroppedFilename );

		BOOL	bImaged = FALSE;

		// 어쩔수 없다.. 에러 이미지 출력
		if( NULL != pImage )
		{
			// 생각없이 한개만 생성.
			// 로딩성공.
			char strFilename[ 1024 ];
			wsprintf( strFilename , "%s\\Map\\Temp\\__TMP__.bmp" , AGCMMAP_THIS->m_TextureList.m_strTextureDirectory );

			VERIFY( RwImageWrite( pImage , (LPCTSTR) strFilename ) );

			RwImageDestroy( pImage		);

			bImaged = m_bmpImage.Load( strFilename , NULL );
			
			DeleteFile( strFilename );
		}
		
		if( bImaged )
		{
			// 끝날때 텍스쳐 카피해 넣어야한다.
			m_bTextureUpdate = TRUE;

			this->m_strFileOriginalPath	= lpDroppedFilename	;
			this->m_strFileNameOrigin	= copyfilename		;

			m_strFileName.Format( "%s%s" , fname , ext );
			UpdateData( FALSE );
		}
		else
		{
			MessageBox( "이미지 로드 실패!" );
		}
	}

	Invalidate( FALSE );

	CDialog::OnDropFiles(hDropInfo);
}
