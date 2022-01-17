#ifndef		_MAGOJA_BMP_GDI_CLASS__
#define		_MAGOJA_BMP_GDI_CLASS__

// Magoja 2001/03/22
// License to Jae-wook Jung ( magoja@magoja.com )
// You can use this code freely.
// in one hour for programming & debugging :>
// this is good for simple app , with bmp.

// Magoja's Note
// ......;
#include <string>

class  CBmp
{
public:
	CBmp(						);
	CBmp( LPSTR filename		);				// Creator
	CBmp( const CBmp & param	);				// Copy constructor

	~CBmp( );									// Destructor

	// Loading functions.
	BOOL Create( int width , int height );

	BOOL LoadBitmap( LPSTR	filename , std::string * pError );
	BOOL Load( LPSTR	filename , std::string * pError ) { return LoadBitmap( filename , pError ); }

	BOOL Save( LPSTR	filename );

	void Release();

	BOOL IsEmpty() const;// Determine this class is empty


	// Drawing Funcions
	BOOL Draw( HDC	hdc ) const;
	BOOL Draw( HDC hdc , int x , int y , DWORD rop = SRCCOPY ) const;
	BOOL Draw( HDC hdc , int x , int y , int dx , int dy , DWORD rop = SRCCOPY) const;
	BOOL Draw( HDC hdc , int x , int y , int dx , int dy , int sx , int sy , DWORD rop = SRCCOPY ) const;

	// Attribute Functions
	int	 GetXSize	() const { return m_nWidth; }
	int  GetYSize	() const { return m_nHeight; }
	int	 GetWidth	() const { return m_nWidth; }
	int  GetHeight	() const { return m_nHeight; }


	// Get the HDC Handle for GDI functions
	// DO NOT Release or Delete this handle
	HDC	GetDC		() const { return m_MemDC; }

protected:
	int		m_nWidth		;
	int		m_nHeight		;
	HBITMAP	m_hBitmap		;
	HBITMAP	m_hPrevBitmap	;
	HDC		m_MemDC			;

public:
	// Operators
	CBmp & operator= ( const CBmp & param );
};


#endif // _MAGOJA_BMP_GDI_CLASS__