// SkyTemplatePreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "MyEngine.h"
#include "SkyTemplatePreviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkyTemplatePreviewDlg dialog

#define	MAX_TEMPLATE_COLOR_INDEX	20


COLORREF	g_ColorRefArray[ MAX_TEMPLATE_COLOR_INDEX ] = {
	RGB( 0		, 0		, 0		)	,
	RGB( 255	, 0		, 0		)	,
	RGB( 0		, 255	, 0		)	,
	RGB( 0		, 0		, 255	)	,
	RGB( 255	, 255	, 0		)	,
	RGB( 255	, 0		, 255	)	,
	RGB( 0		, 255	, 255	)	,
	RGB( 255	, 255	, 255	)	,
	RGB( 64		, 0		, 0		)	,
	RGB( 0		, 64	, 0		)	,
	RGB( 0		, 0		, 64	)	,
	RGB( 64		, 64	, 0		)	,
	RGB( 64		, 0		, 64	)	,
	RGB( 0		, 64	, 64	)	,
	RGB( 64		, 64	, 64	)	,
	RGB( 0		, 0		, 0		)	,
};

inline	COLORREF	__GetColorIndex( int nIndex )
{
	if( nIndex < MAX_TEMPLATE_COLOR_INDEX )
		return g_ColorRefArray[ nIndex ];
	else
	{
		TRACE( "칼라범위초과!!\n" );
		return g_ColorRefArray[ 0 ];
	}
}

CSkyTemplatePreviewDlg::CSkyTemplatePreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkyTemplatePreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkyTemplatePreviewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSkyTemplatePreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkyTemplatePreviewDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSkyTemplatePreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CSkyTemplatePreviewDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkyTemplatePreviewDlg message handlers

BOOL CSkyTemplatePreviewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd * pWnd = GetDlgItem( IDC_DISPLAY );
	ASSERT( NULL != pWnd );

	pWnd->GetWindowRect( m_Rect );
	ScreenToClient( m_Rect );

	pWnd->DestroyWindow();

	CDC	* pDC = GetDC();

	m_MemDC.CreateCompatibleDC( pDC );

	m_Bitmap.CreateCompatibleBitmap(
		pDC																									,
		m_Rect.right	= m_Rect.left	+ ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 )	* MAP_DEFAULT_DEPTH ,
		m_Rect.bottom	= m_Rect.top	+ ( ALEF_LOAD_RANGE_Y2 - ALEF_LOAD_RANGE_Y1 )	* MAP_DEFAULT_DEPTH	);

	m_MemDC.SelectObject( m_Bitmap );

	ReleaseDC( pDC );

	// 이미지 생성..
	TRACE( "이미지생성..시간이 좀 걸림..\n" );

	int	x,z;
	int	nSegmentX	, nSegmentZ;
	int	aTemplate[ SECTOR_MAX_COLLISION_OBJECT_COUNT ]	;
	int	nCount		;
	ApWorldSector	* pSector;

	for( z = ALEF_LOAD_RANGE_Y1 ; z <= ( int ) ALEF_LOAD_RANGE_Y2 ; ++z )
	{
		for( x = ALEF_LOAD_RANGE_X1 ; x <= ( int ) ALEF_LOAD_RANGE_X2 ; ++x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );
			if( pSector )
			{
				for( nSegmentZ = 0 ; nSegmentZ < MAP_DEFAULT_DEPTH ; ++nSegmentZ )
				{
					for( nSegmentX = 0 ; nSegmentX < MAP_DEFAULT_DEPTH ; ++nSegmentX )
					{
						nCount	= pSector->GetSkyObjectTemplateID( nSegmentX , nSegmentZ , aTemplate , SECTOR_MAX_COLLISION_OBJECT_COUNT );

						if( nCount == 2 )
						{
							// 헤헤 - -...
							m_MemDC.SetPixel(	( x - ALEF_LOAD_RANGE_X1 ) * MAP_DEFAULT_DEPTH + nSegmentX	,
												( z - ALEF_LOAD_RANGE_Y1 ) * MAP_DEFAULT_DEPTH + nSegmentZ	,
												__GetColorIndex( aTemplate[ 0 ] )							);
						}
					}
				}

			}
			else
			{
				m_MemDC.FillSolidRect(
					( x - ALEF_LOAD_RANGE_X1 ) * MAP_DEFAULT_DEPTH	,
					( z - ALEF_LOAD_RANGE_Y1 ) * MAP_DEFAULT_DEPTH	,
					MAP_DEFAULT_DEPTH								,
					MAP_DEFAULT_DEPTH								,
					__GetColorIndex( 0 )							);					

			}

		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSkyTemplatePreviewDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.BitBlt( m_Rect.left , m_Rect.top , m_Rect.Width() , m_Rect.Height() , &m_MemDC , 0 , 0 , SRCCOPY );
	// Do not call CDialog::OnPaint() for painting messages
}
