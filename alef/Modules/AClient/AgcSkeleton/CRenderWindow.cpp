#include "CRenderWindow.h"



CRenderWindow::CRenderWindow( void )
{
	m_hWnd = NULL;
	m_hInstance = NULL;

	m_WndClassEx.cbSize = 0;

	m_nWidth = 0;
	m_nHeight = 0;
	m_nDepth = 0;

	memset( m_strWindowClassName, 0, sizeof( char ) * STRING_LENGTH_WINDOW_CLASSNAME );
	memset( m_strWindowCaption, 0, sizeof( char ) * STIRNG_LENGTH_WINDOW_CAPTION );

	m_bIsFullScreen = FALSE;
	m_bIsActivate = FALSE;

	m_fnMessageProc = NULL;
}

CRenderWindow::~CRenderWindow( void )
{
	if( m_bIsActivate )
	{
		OnDestroy();
	}
}

BOOL CRenderWindow::OnCreate( HINSTANCE hInstance, int nWidth, int nHeight, int nDepth, BOOL bIsFullScreen, WNDPROC fnProc )
{
	m_hInstance = hInstance;
	if( !m_hInstance ) return FALSE;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = nDepth;

	m_bIsFullScreen = bIsFullScreen;
	m_fnMessageProc = fnProc;

	m_hWnd = _MakeWindow();
	if( !m_hWnd ) return FALSE;
	
	return TRUE;
}

BOOL CRenderWindow::OnDestroy( void )
{
	if( !m_hWnd ) return TRUE;
	
	::DestroyWindow( m_hWnd );
	m_hWnd = NULL;

	if( m_WndClassEx.cbSize )
	{
		::UnregisterClass( m_strWindowClassName, m_hInstance );
	}

	return TRUE;
}

BOOL CRenderWindow::OnShow( void )
{
	if( !m_hWnd ) return FALSE;

	::ShowWindow( m_hWnd, SW_SHOW );
	::UpdateWindow( m_hWnd );

	return TRUE;
}

BOOL CRenderWindow::OnHide( void )
{
	if( !m_hWnd ) return FALSE;

	::ShowWindow( m_hWnd, SW_HIDE );
	::UpdateWindow( m_hWnd );

	return TRUE;
}

BOOL CRenderWindow::OnMessage( unsigned int nMsg, WPARAM wParam, LPARAM lParam )
{
	if( m_fnMessageProc )
	{
		return m_fnMessageProc( m_hWnd, nMsg, wParam, lParam );
	}

	return ::DefWindowProc( m_hWnd, nMsg, wParam, lParam );
}

HWND CRenderWindow::_MakeWindow( void )
{
	// 윈도우 클래스 작성
	m_WndClassEx.cbSize        = sizeof( WNDCLASSEX );
	m_WndClassEx.style         = CS_CLASSDC;
	m_WndClassEx.lpfnWndProc   = m_fnMessageProc;
	m_WndClassEx.cbClsExtra    = 0;
	m_WndClassEx.cbWndExtra    = 0;
	m_WndClassEx.hInstance     = m_hInstance;
	m_WndClassEx.hIcon         = LoadIcon( m_hInstance, MAKEINTRESOURCE( 101 ) );
	m_WndClassEx.hIconSm       = LoadIcon( m_hInstance, MAKEINTRESOURCE( 101 ) );
	m_WndClassEx.hCursor       = ::SetCursor( ::LoadCursor( NULL, NULL ) ); // IDC_ARROW
	m_WndClassEx.hbrBackground = NULL; 
	m_WndClassEx.lpszMenuName  = NULL;
	m_WndClassEx.lpszClassName = ( LPCTSTR )m_strWindowClassName;

	if( !::RegisterClassEx( &m_WndClassEx ) ) return NULL;

	HWND hHandle = NULL;
	if( m_bIsFullScreen )
	{
		hHandle = ::CreateWindow( m_strWindowClassName, m_strWindowCaption, WS_POPUP | WS_VISIBLE, 0, 0, m_nWidth, m_nHeight, NULL, NULL, m_hInstance, NULL );
		if( !hHandle ) return NULL;

		::SetWindowPos( hHandle, HWND_TOPMOST, 0, 0, m_nWidth, m_nHeight, SWP_NOSIZE );
	}
	else
	{
		hHandle = ::CreateWindow( m_strWindowClassName, m_strWindowCaption, WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_VISIBLE, 0, 0, m_nWidth, m_nHeight + 25, NULL, NULL, m_hInstance, NULL );
		if( !hHandle ) return NULL;

		::ShowWindow( hHandle, SW_SHOW );
		::UpdateWindow( hHandle );
	}

	return hHandle;
}
