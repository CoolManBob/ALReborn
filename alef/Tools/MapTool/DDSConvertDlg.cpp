// DDSConvertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "MyEngine.h"
#include "Mainfrm.h"
#include "AcuTexture.h"
#include "DDSConvertDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDDSConvertDlg dialog


CDDSConvertDlg::CDDSConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDDSConvertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDDSConvertDlg)
	//}}AFX_DATA_INIT
}


void CDDSConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDDSConvertDlg)
	DDX_Control(pDX, IDC_FILELIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDDSConvertDlg, CDialog)
	//{{AFX_MSG_MAP(CDDSConvertDlg)
	ON_BN_CLICKED(IDC_CONVERT, OnConvert)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDDSConvertDlg message handlers

void CDDSConvertDlg::OnConvert() 
{
	if( 0 == m_ctlList.GetCount() )
	{
		MessageBox( "한개도없다니깐!" , "-_-+" , MB_OK );
		return;
	}
	else
	{
		// 변환작업..
		RwTexture	* pTexture;
		CString		strFilename;
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
		char	pFilename[ 1024 ];

		// 마고자 (2004-02-26 오전 11:26:57) : 포멧입력은 차후에..
		INT32	nFormat	= D3DFMT_DXT3;
		INT32	nCount = 0;

		for( int i = 0 ; i < m_ctlList.GetCount() ; ++ i )
		{
			m_ctlList.GetText( i , strFilename );

			pTexture = RwTextureRead( (LPSTR) (LPCTSTR) strFilename , NULL );
			if( pTexture )
			{
				_splitpath( (LPSTR) (LPCTSTR) strFilename , drive, dir, fname, ext );
				wsprintf( pFilename , "%s%s%s.dds" , drive , dir , fname );
				if( AcuTexture::RwD3D9DDSTextureWrite( pTexture , pFilename , nFormat , 1 , ACUTEXTURE_COPY_LOAD_LINEAR , NULL , FALSE , FALSE ) )
				{
					nCount++;
					DisplayMessage( RGB( 0 , 0 , 255 ) , "파일 변환성공! (%s)" , pFilename );
				}
				else
				{
					DisplayMessage( RGB( 255 , 0 , 0 ) , "파일을 쓰기실패! (%s)" , pFilename );
				}

				RwTextureDestroy( pTexture );
			}
			else
			{
				DisplayMessage( RGB( 255 , 0 , 0 ) , "파일을 읽을수가 없어요! (%s)" , strFilename );
			}
		}

		DisplayMessage( RGB( 0 , 255 , 0 ) , "파일변환 %d 개 파일중 %d 개 성공했습니다.! (%s)" , m_ctlList.GetCount() , nCount );
	}	
}

void CDDSConvertDlg::OnDropFiles(HDROP hDropInfo) 
{
	// 파일 삭제..
	m_ctlList.ResetContent();

	char	lpstr[ 1024 ];
	// 우선 몇개의 파일이 드래그 되는 지 확인한다.
	UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpstr , 1024 );

	if( dragcount > 0 )
	{
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

		// 모두다 검사 하려면
		for ( int i = 0 ; i < ( int )  dragcount ; i ++ )
		{
			::DragQueryFile( hDropInfo , i , lpstr , 1024 );
			TRACE( "드롭된 %d 번째 파일 -'%s'\n" , i + 1 , lpstr );

			_splitpath( lpstr , drive, dir, fname, ext );
			for( int nCount = 0 ; nCount < ( int ) strlen( ext ) ; nCount ++ ) ext[ nCount ] = toupper( ext[ nCount ] );

			if( !strcmp( ext , ".BMP" ) ||
				!strcmp( ext , ".JPG" ) ||
				!strcmp( ext , ".PNG" ) ||
				!strcmp( ext , ".TIF" ) )
			{
				m_ctlList.AddString( lpstr );
			}
		}
	}
	
	CDialog::OnDropFiles(hDropInfo);
}
