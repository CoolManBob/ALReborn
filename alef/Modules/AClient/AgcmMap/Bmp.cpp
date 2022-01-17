// Magoja 2001/03/22
// License to Jae-wook Jung ( magoja@magoja.com )
// You can use this code freely.
// in one hour for programming & debugging :>
// this is good for simple app , with bmp.

// Magoja's Note
// ......;
#include "ApBase.h"
#include <windows.h>
#include <windowsx.h>

#include "bmp.h"

CBmp::CBmp( LPSTR filename )
:m_nWidth( 0 ),m_nHeight( 0 ),m_hBitmap( NULL ),m_MemDC( NULL ), m_hPrevBitmap( NULL )// Creator
{
	LoadBitmap( filename , NULL );
}

CBmp::CBmp():m_nWidth( 0 ),m_nHeight( 0 ),m_hBitmap( NULL ),m_MemDC( NULL ), m_hPrevBitmap( NULL )
{

}

CBmp::~CBmp( )
{
	Release();
}

BOOL CBmp::LoadBitmap( LPSTR	filename , std::string * pError )
{
	//  Checking File Name
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( filename, drive, dir, fname, ext );

	// Make Upper
	int		textlen = ( int ) strlen( ext );
	for( int i = 0 ; i < textlen ; ++i ) ext[ i ] = ( char ) toupper( ext[ i ] );

	if( strncmp( ext , ".BMP" , 5 ) )
	{
		// Extension name is different
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::LoadBitmap - Not a BMP File - '%s'\n" , filename );
		OutputDebugString( strDebug );
#endif
		if( pError )
		{
			char strDebug[ 256 ];
			wsprintf( strDebug , "Not a BMP File - '%s'\n" , filename );
			*pError = strDebug;
		}
		return FALSE;
	}

	if( !IsEmpty() )
	{
		// If this class already has image
		Release(); // Release it first
	}

	m_hBitmap = (HBITMAP)LoadImage(NULL, filename,
		IMAGE_BITMAP, 0 , 0 ,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE );

	if (m_hBitmap == NULL)
	{
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::LoadBitmap File loading failure - '%s'\n" , filename );
		OutputDebugString( strDebug );
		GetCurrentDirectory( 256 , strDebug );
#endif
		if( pError )
		{
			DWORD	uError = GetLastError();
			LPVOID lpMsgBuf;
			::FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				uError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			// Process any inserts in lpMsgBuf.
			// ...
			*pError = (LPCTSTR)lpMsgBuf;

			// Free the buffer.
			LocalFree( lpMsgBuf );
		}
		return FALSE;
	}
	
	// Get size of the bitmap
	BITMAP bmp;
	GetObject(m_hBitmap, sizeof(bmp), &bmp);

	// Store the size
	m_nWidth = bmp.bmWidth;
	m_nHeight = bmp.bmHeight;

	// Prepare memory dc for drawing
	HDC	hdc = ::GetDC( NULL );
	if( hdc == NULL )
	{
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::LoadBitmap File loading failure ( GetDC return Fail ) - '%s'\n" , filename );
		OutputDebugString( strDebug );
#endif
		Release();

		if( pError )
		{
			char strDebug[ 256 ];
			wsprintf( strDebug , "GetDC() Failed\n" );
			*pError = strDebug;
		}
		return FALSE;
	}

	m_MemDC			= CreateCompatibleDC( hdc );
	m_hPrevBitmap	= ( HBITMAP ) SelectObject( m_MemDC , m_hBitmap );
	ReleaseDC( NULL , hdc );

	return TRUE;
}

BOOL CBmp::Draw( HDC hdc ) const
{
	return Draw( hdc , 0 , 0 , m_nWidth , m_nHeight , SRCCOPY );
}

BOOL CBmp::Draw( HDC hdc , int x , int y , DWORD rop ) const
{
	return Draw( hdc , x , y , m_nWidth , m_nHeight , rop );
}

BOOL CBmp::Draw( HDC hdc , int x , int y , int dx , int dy , DWORD rop ) const
{
	if( IsEmpty() ) return FALSE;
	BitBlt( hdc , x , y , dx , dy ,m_MemDC , 0 , 0 , rop );
	return TRUE;
}

BOOL CBmp::Draw( HDC hdc , int x , int y , int dx , int dy , int sx , int sy , DWORD rop ) const
{
	if( IsEmpty() ) return FALSE;
	BitBlt( hdc , x , y , dx , dy ,m_MemDC , sx , sy , rop );
	return TRUE;
}

void CBmp::Release()
{
	// Clean Up
	m_nWidth	= 0;
	m_nHeight	= 0;

	if( m_hBitmap )
	{
		if( m_MemDC )
			SelectObject( m_MemDC , m_hPrevBitmap );

		DeleteObject( m_hBitmap );
		m_hBitmap = NULL;
	}

	if( m_MemDC )
	{
		DeleteDC( m_MemDC );
		m_MemDC = NULL;
	}
}

BOOL CBmp::IsEmpty() const
{
	if( m_hBitmap == NULL || m_MemDC == NULL ) return TRUE;
	else return FALSE;
}

CBmp::CBmp( const CBmp & param ) // Copy constructor
:m_nWidth( 0 ),m_nHeight( 0 ),m_hBitmap( NULL ),m_MemDC( NULL )
{
	if( param.IsEmpty() ) return;
	
	// Copy that Image;
	m_nWidth	= param.GetWidth();
	m_nHeight	= param.GetHeight();

	// Create Memory DC
	m_MemDC		= CreateCompatibleDC( param.GetDC() );
	
	if( m_MemDC == NULL )
	{
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::CBmp Copy Constructor - CreateDC Fail\n" );
		OutputDebugString( strDebug );
#endif
		Release();
		return;
	}

	m_hBitmap	= CreateCompatibleBitmap( m_MemDC , m_nWidth , m_nHeight );
	if( m_hBitmap == NULL )
	{
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::CBmp Copy Constructor - CreateBitmap Fail\n" );
		OutputDebugString( strDebug );
#endif
		Release();
		return;
	}

	// Copy the Image
	param.Draw( GetDC() );
}

CBmp & CBmp::operator= ( const CBmp & param )
{
	// Release First
	Release();

	if( param.IsEmpty() ) return *this;
	
	// Copy that Image;
	m_nWidth	= param.GetWidth();
	m_nHeight	= param.GetHeight();

	// Create Memory DC
	m_MemDC		= CreateCompatibleDC( param.GetDC() );
	
	if( m_MemDC == NULL )
	{
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::operator= - CreateDC Fail\n" );
		OutputDebugString( strDebug );
#endif
		Release();
		return *this;
	}

	m_hBitmap	= CreateCompatibleBitmap( m_MemDC , m_nWidth , m_nHeight );
	if( m_hBitmap == NULL )
	{
#ifdef _DEBUG
		char strDebug[ 256 ];
		wsprintf( strDebug , "CBmp::operator= - CreateBitmap Fail\n" );
		OutputDebugString( strDebug );
#endif
		Release();
		return *this;
	}

	// Copy the Image
	param.Draw( GetDC() );

	return *this;
}

BOOL CBmp::Create( int width , int height )
{
	Release();

	HDC	hdc = ::GetDC( NULL );
	m_MemDC			= CreateCompatibleDC( hdc );
	m_hBitmap		= CreateCompatibleBitmap( hdc , width , height );
	m_hPrevBitmap	= ( HBITMAP ) SelectObject( m_MemDC , m_hBitmap );
	ReleaseDC( NULL , hdc );
	
	m_nWidth		= width	;
	m_nHeight		= height;
	return TRUE;
}

PBITMAPINFO		CreateBitmapInfoStruct	( HBITMAP hBmp												);
BOOL			CreateBMPFile			( LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC	);

BOOL CBmp::Save( LPSTR	filename )
{
	BITMAPINFO *bi;
	bi = CreateBitmapInfoStruct( (HBITMAP) m_hBitmap );
	BOOL ret = CreateBMPFile( filename , bi , m_hBitmap , m_MemDC );

	LocalFree( ( HLOCAL ) bi );
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////
PBITMAPINFO CreateBitmapInfoStruct( HBITMAP hBmp )
{     
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;
	// Retrieve the bitmap's color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
	{
        //errhandler("GetObject", hwnd); 
		return NULL;
	}
    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1; 
    else if (cClrBits <= 4)
		cClrBits = 4; 
    else if (cClrBits <= 8)
		cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;
	// Allocate memory for the BITMAPINFO structure. (This structure
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.)   
	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (1<< cClrBits)); 
	// There is no RGBQUAD array for the 24-bit-per-pixel format. 
	else          pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER)); 
    // Initialize the fields in the BITMAPINFO structure.
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits);
	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;
	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	// For Windows NT/2000, the width must be DWORD aligned unless
	// the bitmap is RLE compressed. This example shows this.
	// For Windows 95/98, the width must be WORD aligned unless the
	// bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8 
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;  
} 

BOOL CreateBMPFile( LPTSTR pszFile, PBITMAPINFO pbi,
                   HBITMAP hBMP, HDC hDC)  
{      
	HANDLE hf;
	// file handle
	BITMAPFILEHEADER hdr; 
	// bitmap file-header 
    PBITMAPINFOHEADER pbih;   
	// bitmap info-header   
	LPBYTE lpBits;            
	// memory pointer     
	DWORD dwTotal;           
	// total count of bytes 
    DWORD cb;            
	// incremental count of bytes
	BYTE *hp;             
	// byte pointer  
	DWORD dwTmp;  
	pbih = (PBITMAPINFOHEADER) pbi;
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage); 
	if (!lpBits)     
	{
		//errhandler("GlobalAlloc", hwnd); 
		return FALSE;
	}
    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB.    
	if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi,  
		DIB_RGB_COLORS))    
	{       
		// errhandler("GetDIBits", hwnd);   
		GlobalFree((HGLOBAL)lpBits);
		return FALSE;
	}    // Create the .BMP file.   
	hf = CreateFile(pszFile,        
		GENERIC_READ | GENERIC_WRITE,    
		(DWORD) 0,     
		NULL,         
		CREATE_ALWAYS,     
		FILE_ATTRIBUTE_NORMAL,   
		(HANDLE) NULL);  
	if (hf == INVALID_HANDLE_VALUE)   
	{
		//errhandler("CreateFile", hwnd); 
		GlobalFree((HGLOBAL)lpBits);
		return FALSE;
	}
    hdr.bfType = 0x4d42;
	// 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed   
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0;
	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +  
		pbih->biSize + pbih->biClrUsed      
		* sizeof (RGBQUAD);    
	// Copy the BITMAPFILEHEADER into the .BMP file.  
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL))    
	{       
		//errhandler("WriteFile", hwnd); 
		GlobalFree((HGLOBAL)lpBits);
		return FALSE;
    }    
	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)   
		+ pbih->biClrUsed * sizeof (RGBQUAD),    
		(LPDWORD) &dwTmp, ( NULL) ) )
	{
		// Error
		// errhandler("WriteFile", hwnd);    
		GlobalFree((HGLOBAL)lpBits);
		return FALSE;
	}
	// Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits;  
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
	{
		//errhandler("WriteFile", hwnd); 
		GlobalFree((HGLOBAL)lpBits);
		return FALSE;
	}
    // Close the .BMP file. 
	if (!CloseHandle(hf))  
	{
		//errhandler("CloseHandle", hwnd); 
	}
    // Free memory.
	GlobalFree((HGLOBAL)lpBits);

	return TRUE;
}


