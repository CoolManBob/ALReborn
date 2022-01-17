// Plugin_BitmapExport.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "Mainfrm.h"
#include "Plugin_BitmapExport.h"

#include "dibapi.h"
#include "dibutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL	__DivisionSetHeightBitmapCallback ( DivisionInfo * pDivisionInfo , PVOID pData );

/////////////////////////////////////////////////////////////////////////////
// CPlugin_BitmapExport
struct __BitmapExportStructHeight
{
	CDC *	pDC	;
	BOOL	bGetRange;
	FLOAT	fMax;
	FLOAT	fMin;

	FLOAT	fFilter;
};

CPlugin_BitmapExport::CPlugin_BitmapExport()
{
	m_fHeightMin	= 0.0f		;
	m_fHeightMax	= 25500.0f	;
	m_fFilter		= 1.0f		;

	m_nGenerateIteration = 64	;
	m_fGenerateFilter	 = 0.4f	;

	m_strShortName = "Bitmap";
}

CPlugin_BitmapExport::~CPlugin_BitmapExport()
{
}


BEGIN_MESSAGE_MAP(CPlugin_BitmapExport, CWnd)
	//{{AFX_MSG_MAP(CPlugin_BitmapExport)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlugin_BitmapExport message handlers

void CPlugin_BitmapExport::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDown(nFlags, point);
}

int CPlugin_BitmapExport::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect	rect;

	rect.SetRect( 
		BITMAP_UI_MARGIN , 
		BITMAP_UI_MARGIN , 
		BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH , 
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT );
	m_buttonPasteIn.Create( "Paste In", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect , this,IDC_BUTTON_PASTEIN);
	
	rect.SetRect( 
		BITMAP_UI_MARGIN * 2 + BITMAP_BUTTONWIDTH   , 
		BITMAP_UI_MARGIN , 
		BITMAP_UI_MARGIN * 2 + BITMAP_BUTTONWIDTH * 2 ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT );
	m_buttonCopy.Create( "Copy", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect , this,IDC_BUTTON_COPY);

	rect.SetRect(
		BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN ,
		BITMAP_UI_MARGIN ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT );
	m_buttonVertexColorSet.Create( "페인트", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect , this,IDC_BUTTON_VERTEX_COLOR_SET);

	rect.SetRect(
		BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN,
		BITMAP_UI_MARGIN ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH + BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH,
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT );
	m_buttonVertexColorGet.Create( "칼라복사", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect , this,IDC_BUTTON_VERTEX_COLOR_GET);

	rect.SetRect(
		BITMAP_UI_MARGIN ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_DETAIL + BITMAP_UI_MARGIN ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONWIDTH ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_DETAIL + BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT );

	m_buttonSetting.Create( "Property", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect , this,IDC_BUTTON_PROPERTY);

	rect.left	= BITMAP_UI_MARGIN * 2 + BITMAP_BUTTONWIDTH		;
	rect.right	= BITMAP_UI_MARGIN * 2 + BITMAP_BUTTONWIDTH * 2	;

	m_buttonGenerate.Create( "Generate", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect , this,IDC_BUTTON_GENERATE);

	// 마고자 (2004-07-09 오후 4:41:49) : 초기 DC 생성..
	CDC	* pDC = GetDC();
	m_memDC.CreateCompatibleDC( pDC );
	m_memBitmap.CreateCompatibleBitmap( pDC , BITMAP_DETAIL , BITMAP_DETAIL );
	m_memDC.SelectObject( m_memBitmap );
	ReleaseDC( pDC );

	return 0;
}

LRESULT CPlugin_BitmapExport::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	#define ISPRESSEDBUTTON( nID )	if( ( nID ) == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam) )

	switch( message )
	{
	case WM_COMMAND:
		{
			ISPRESSEDBUTTON( IDC_BUTTON_PASTEIN )
			{
				TRACE( "페이스트~\n" );
				SetHeightInfo();
			}
			ISPRESSEDBUTTON( IDC_BUTTON_COPY )
			{
				TRACE( "카피~~\n" );
				GetHeightInfo();
				Invalidate( FALSE );
			}
			ISPRESSEDBUTTON( IDC_BUTTON_PROPERTY )
			{
				// 값 변경..
				CHeightmapPropteryDlg dlg;
				dlg.m_fGeometryFilter	= m_fFilter;
				dlg.m_nMaxHeight		= ( INT32 ) ( m_fHeightMax / 100.0f );
				dlg.m_nMinHeight		= ( INT32 ) ( m_fHeightMin / 100.0f );
				if( IDOK == dlg.DoModal() )
				{
					m_fFilter		= dlg.m_fGeometryFilter	; 
					m_fHeightMax	= ( FLOAT ) dlg.m_nMaxHeight * 100.0f;
					m_fHeightMin	= ( FLOAT ) dlg.m_nMinHeight * 100.0f;
					Invalidate( FALSE );
				}
			}
			ISPRESSEDBUTTON( IDC_BUTTON_GENERATE )
			{
				// 값 변경..
				CGenerateGeometryDlg dlg;
				dlg.m_fFilter		= m_fGenerateFilter		;
				dlg.m_nIteration	= m_nGenerateIteration	;
				if( IDOK == dlg.DoModal() )
				{
					m_fGenerateFilter		= dlg.m_fFilter		;
					m_nGenerateIteration	= dlg.m_nIteration	;
					// 어플라이 --;...

					dlg.m_stHeightMap.DrawDC( m_memDC.GetSafeHdc() , 0 , 0 );

					__BitmapExportStructHeight	stHeight;

					stHeight.pDC	= &m_memDC		;
					stHeight.fMax	= m_fHeightMax	;
					stHeight.fMin	= m_fHeightMin	;
					stHeight.fFilter= m_fFilter		;
					stHeight.bGetRange	= FALSE		;

					AGCMMAP_THIS->EnumLoadedDivision( __DivisionSetHeightBitmapCallback , ( PVOID ) &stHeight );

					Invalidate( FALSE );
				}
			}

			ISPRESSEDBUTTON( IDC_BUTTON_VERTEX_COLOR_GET )
			{
				GetVertexColorMap();
				Invalidate( FALSE );
			}
			ISPRESSEDBUTTON( IDC_BUTTON_VERTEX_COLOR_SET )
			{
				SetVertexColorMap();
				Invalidate( FALSE );
			}
		}
		break;
	default:
		break;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void CPlugin_BitmapExport::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 38 , 38 , 38 ) );

	dc.BitBlt( ( TILELIST_WIDTH - BITMAP_DETAIL ) / 2 , 30 , BITMAP_DETAIL , BITMAP_DETAIL , & m_memDC , 0 , 0 , SRCCOPY );

	CString	str;

	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	dc.SetBkMode( TRANSPARENT );

	str.Format( "최소 : %.0fm" , m_fHeightMin / 100.0f );
	dc.TextOut(
		BITMAP_UI_MARGIN ,
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_DETAIL + BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN ,
		str );

	str.Format( "최대 : %.0fm" , m_fHeightMax / 100.0f );
	dc.TextOut( 
		BITMAP_UI_MARGIN , 
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_DETAIL + BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT ,
		str );
	
	str.Format( "필터 : %.1f" , m_fFilter );
	dc.TextOut( 
		BITMAP_UI_MARGIN , 
		BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_DETAIL + BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT + BITMAP_UI_MARGIN + BITMAP_BUTTONHEIGHT * 2 ,
		str );
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL	__DivisionGetHeightBitmapCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	__BitmapExportStructHeight	* pstHeight = ( __BitmapExportStructHeight * ) pData;

	CDC	* pDC = pstHeight->pDC;

	ASSERT( NULL != pDC );

	ApWorldSector * pSector;
	int x , z;
	int nSegmentX , nSegmentZ;

	FLOAT	fHeight;
	FLOAT	fHeightMax = pstHeight->fMax , fHeightMin = pstHeight->fMin;
	BOOL	bFirst = TRUE;

	if( pstHeight->bGetRange )
	{
		// 마고자 (2004-07-09 오후 4:58:29) : 최대 최소 높이 구함.
		for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
		{
			for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
			{
				pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

				if( NULL == pSector ) continue;

				for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
				{
					for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
					{
						fHeight = pSector->D_GetHeight2( nSegmentX , nSegmentZ );

						if( bFirst )
						{
							bFirst = FALSE;
							fHeightMin = fHeight;
							fHeightMax = fHeight;
						}

						if( fHeight < fHeightMin ) fHeightMin = fHeight;
						if( fHeightMax < fHeight ) fHeightMax = fHeight;
					}
				}
			}
		}

		pstHeight->fMax	= fHeightMax;
		pstHeight->fMin	= fHeightMin;
	}

	BYTE		value;

	// 마고자 (2004-07-09 오후 4:58:44) : 이미지 생성..
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
			{
				for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
				{
					fHeight = pSector->D_GetHeight2( nSegmentX , nSegmentZ );

					if( fHeight < fHeightMin ) fHeight = fHeightMin;
					if( fHeightMax < fHeight ) fHeight = fHeightMax;

					value = ( BYTE ) ( ( fHeight - fHeightMin ) / ( fHeightMax - fHeightMin ) * 255.0f );

					pDC->SetPixel( ( x - pDivisionInfo->nX ) * 16 + nSegmentX , ( z - pDivisionInfo->nZ ) * 16 + nSegmentZ , RGB( value , value , value ) );					
				}
			}
		}
	}

	return FALSE;
}

BOOL	__DivisionSetHeightBitmapCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	__BitmapExportStructHeight	* pstHeight = ( __BitmapExportStructHeight * ) pData;

	CDC	* pDC = pstHeight->pDC;

	ASSERT( NULL != pDC );

	ApWorldSector	* pSector	;

	int x , z;
	int nSegmentX , nSegmentZ;

	FLOAT	fHeight;
	FLOAT	fHeightMax = pstHeight->fMax , fHeightMin = pstHeight->fMin;

	BYTE		value;
	COLORREF	color;

	CProgressDlg	dlgProgress;
	INT32			nProgress	= 0;
	dlgProgress.StartProgress( "Updating Geometry" , 256 * 3 , g_pMainFrame );

	// 마고자 (2004-07-09 오후 4:58:44) : 이미지 생성..
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			dlgProgress.SetProgress( nProgress++ );

			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
			{
				for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
				{
					color = pDC->GetPixel( ( x - pDivisionInfo->nX ) * 16 + nSegmentX , ( z - pDivisionInfo->nZ ) * 16 + nSegmentZ );
					value = GetRValue( color );

					fHeight = ( FLOAT ) value / 255.0f * ( fHeightMax - fHeightMin ) + fHeightMin;

					AGCMMAP_THIS->D_SetHeight( pSector , SECTOR_HIGHDETAIL , nSegmentX , nSegmentZ , fHeight , TRUE , FALSE );
				}
			}
		}
	}

	// 사포 한방 때림..
	float height , original , applyheight;

	FLOAT fAdjustRate = pstHeight->fFilter;

	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			dlgProgress.SetProgress( nProgress++ );

			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
			{
				for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
				{
					height = pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX - 1 ),
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ - 1 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX + 0 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ - 1 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX + 1 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ - 1 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX - 1 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ + 0 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX + 1 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ + 0 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX - 1 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ + 1 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX + 0 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ + 1 ) );
					height += pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX + 1 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ + 1 ) );
					height += original = pSector->D_GetHeight(	pSector->GetXStart() + pSector->GetStepSizeX() * ( nSegmentX + 0 ) ,
															pSector->GetZStart() + pSector->GetStepSizeX() * ( nSegmentZ + 0 ) );
					height /= 9.0f;

					applyheight = -( original - height ) * fAdjustRate;

					AGCMMAP_THIS->D_SetHeight( pSector , SECTOR_HIGHDETAIL , nSegmentX , nSegmentZ , original + applyheight , TRUE , FALSE );
				}
			}
		}
	}

	
	// 여기서 전체적으로 한번 업데이트 해준다.
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			dlgProgress.SetProgress( nProgress++ );

			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );
			if( NULL == pSector ) continue;

			AGCMMAP_THIS->RecalcNormal( pSector , SECTOR_HIGHDETAIL );
			AGCMMAP_THIS->UnlockSectors( TRUE );
		}
	}

	dlgProgress.EndProgress();

	return FALSE;
}

BOOL CPlugin_BitmapExport::SetClipBoard()
{
	if (::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		::GdiFlush();
		
		// borrowed these variables from the sample app
		HDIB     hDib    = NULL;
		HBITMAP  hBitmap = NULL;
		HPALETTE ghPal   = NULL;
		HANDLE hResult = NULL;
		
		//if (m_hBitmap)
		{
			// call the function that converts the bitmap to a DIB
			hDib = BitmapToDIB( ( HBITMAP ) m_memBitmap.GetSafeHandle(), ghPal);
			if (hDib)
			{
				// ahhh, the sweet smell of success
				hResult = ::SetClipboardData(CF_DIB, hDib);
				if (hResult == NULL)
				{
					//_ShowLastError();
				}
			}
			else
			{
				MessageBeep(0);
			}
		}
		::CloseClipboard();

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_BitmapExport::GetClipBoard()
{
    if (IsClipboardFormatAvailable(CF_DIB))
    {
        // a DIB is in the clipboard, draw it out
        GLOBALHANDLE    hGMem ;
        LPBITMAPINFO    lpBI ;
        void*            pDIBBits;
    
        OpenClipboard() ;
        hGMem = GetClipboardData(CF_DIB) ;
        lpBI = (LPBITMAPINFO)GlobalLock(hGMem) ;
        // point to DIB bits after BITMAPINFO object
        pDIBBits = (void*)(lpBI->bmiColors);

		if( lpBI->bmiHeader.biWidth		== BITMAP_DETAIL	&&
			lpBI->bmiHeader.biHeight	== BITMAP_DETAIL	)
		{
			::StretchDIBits( m_memDC.GetSafeHdc() ,
				0 , 0 , 
				lpBI->bmiHeader.biWidth , lpBI->bmiHeader.biHeight ,
				0, 0, lpBI->bmiHeader.biWidth, 
				lpBI->bmiHeader.biHeight,
				pDIBBits, lpBI, DIB_RGB_COLORS, SRCCOPY);
		}

        GlobalUnlock(hGMem) ;
        CloseClipboard() ;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CPlugin_BitmapExport::GetHeightInfo()
{
	__BitmapExportStructHeight	stHeight;

	stHeight.pDC	= &m_memDC		;
	stHeight.fMax	= m_fHeightMax	;
	stHeight.fMin	= m_fHeightMin	;
	stHeight.fFilter= m_fFilter		;
	stHeight.bGetRange	= FALSE		;

	AGCMMAP_THIS->EnumLoadedDivision( __DivisionGetHeightBitmapCallback , ( PVOID ) &stHeight );
	
	SetClipBoard();
}

void CPlugin_BitmapExport::SetHeightInfo()
{
	if( GetClipBoard() )
	{
		__BitmapExportStructHeight	stHeight;

		stHeight.pDC	= &m_memDC		;
		stHeight.fMax	= m_fHeightMax	;
		stHeight.fMin	= m_fHeightMin	;
		stHeight.fFilter= m_fFilter		;
		stHeight.bGetRange	= FALSE		;

		AGCMMAP_THIS->EnumLoadedDivision( __DivisionSetHeightBitmapCallback , ( PVOID ) &stHeight );

		Invalidate( FALSE );
    }
	else
	{
		MessageBox( "클립보드에 이미지가 없어요" );
	}
}

BOOL	CPlugin_BitmapExport::__DivisionSetVertexColorCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CDC	* pDC = ( CDC * ) pData;
	ASSERT( NULL != pDC );

	ApWorldSector	* pSector	;
	ApDetailSegment	* pSegment	;

	int x , z;
	int nSegmentX , nSegmentZ;

	COLORREF	color;
	ApRGBA		colorGet;

	CProgressDlg	dlgProgress;
	INT32			nProgress	= 0;
	dlgProgress.StartProgress( "Updating Geometry" , 256 , g_pMainFrame );

	// 마고자 (2004-07-09 오후 4:58:44) : 이미지 생성..
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			dlgProgress.SetProgress( nProgress++ );

			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
			{
				for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
				{
					pSegment = pSector->D_GetSegment( SECTOR_HIGHDETAIL , nSegmentX , nSegmentZ );

					if( pSegment )
					{
						color = pDC->GetPixel( ( x - pDivisionInfo->nX ) * 16 + nSegmentX , ( z - pDivisionInfo->nZ ) * 16 + nSegmentZ );

						colorGet.alpha	= 255;
						colorGet.red	= GetRValue( color );
						colorGet.green	= GetGValue( color );
						colorGet.blue	= GetBValue( color );

						AGCMMAP_THIS->D_SetValue( pSector , SECTOR_HIGHDETAIL , nSegmentX , nSegmentZ , colorGet );
					}
				}
			}
		}
	}

	dlgProgress.EndProgress();

	AGCMMAP_THIS->UnlockSectors();
	

	return TRUE;
}

BOOL	CPlugin_BitmapExport::__DivisionGetVertexColorCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CDC	* pDC = ( CDC * ) pData;
	ASSERT( NULL != pDC );

	ApWorldSector	* pSector	;
	ApDetailSegment	* pSegment	;

	int x , z;
	int nSegmentX , nSegmentZ;

	ApRGBA		colorGet;

	CProgressDlg	dlgProgress;
	INT32			nProgress	= 0;
	dlgProgress.StartProgress( "Updating Geometry" , 256 , g_pMainFrame );

	// 마고자 (2004-07-09 오후 4:58:44) : 이미지 생성..
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			dlgProgress.SetProgress( nProgress++ );

			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
			{
				for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
				{
					pSegment = pSector->D_GetSegment( SECTOR_HIGHDETAIL , nSegmentX , nSegmentZ );

					if( pSegment )
					{
						colorGet = pSegment->vertexcolor;
						pDC->SetPixel( ( x - pDivisionInfo->nX ) * 16 + nSegmentX , ( z - pDivisionInfo->nZ ) * 16 + nSegmentZ , RGB( colorGet.red , colorGet.green , colorGet.blue ) );
					}
				}
			}
		}
	}

	dlgProgress.EndProgress();

	return TRUE;
}

void CPlugin_BitmapExport::GetVertexColorMap()
{
	AGCMMAP_THIS->EnumLoadedDivision( __DivisionGetVertexColorCallback , ( PVOID ) &m_memDC );

	HANDLE hResult = NULL;
	
	SetClipBoard();
}

void CPlugin_BitmapExport::SetVertexColorMap()
{
	if( GetClipBoard() )
	{
		AGCMMAP_THIS->EnumLoadedDivision( __DivisionSetVertexColorCallback , ( PVOID ) &m_memDC );

		Invalidate( FALSE );
    }
	else
	{
		MessageBox( "클립보드에 이미지가 없어요" );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CHeightmapPropteryDlg dialog


CHeightmapPropteryDlg::CHeightmapPropteryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHeightmapPropteryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHeightmapPropteryDlg)
	m_fGeometryFilter	= 0.5f;
	m_nMaxHeight		= 255;
	m_nMinHeight		= 0;
	//}}AFX_DATA_INIT
}


void CHeightmapPropteryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHeightmapPropteryDlg)
	DDX_Text(pDX, IDC_GEOMETRYFILTER, m_fGeometryFilter);
	DDX_Text(pDX, IDC_MAXHEIGHT, m_nMaxHeight);
	DDX_Text(pDX, IDC_MINHEIGHT, m_nMinHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHeightmapPropteryDlg, CDialog)
	//{{AFX_MSG_MAP(CHeightmapPropteryDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeightmapPropteryDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CGenerateGeometryDlg dialog


CGenerateGeometryDlg::CGenerateGeometryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenerateGeometryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenerateGeometryDlg)
	m_fFilter		= 0.0f	;
	m_nIteration	= 0		;
	//}}AFX_DATA_INIT

	m_stHeightMap.Create( 256 );
}


void CGenerateGeometryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenerateGeometryDlg)
	DDX_Text(pDX, IDC_FILTER, m_fFilter);
	DDX_Text(pDX, IDC_ITERATION, m_nIteration);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenerateGeometryDlg, CDialog)
	//{{AFX_MSG_MAP(CGenerateGeometryDlg)
	ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenerateGeometryDlg message handlers

void CGenerateGeometryDlg::OnGenerate() 
{
	UpdateData( TRUE );
	m_stHeightMap.MakeTerrainFault( 256 , m_nIteration , 0 , 255 , m_fFilter );
	Invalidate( FALSE );	
}

void CGenerateGeometryDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	

	CRect	rect;
	GetClientRect( rect );

	m_stHeightMap.DrawDC( dc.GetSafeHdc() , rect.Width() - 256 , rect.Height() - 256 );
	// Do not call CDialog::OnPaint() for painting messages
}
