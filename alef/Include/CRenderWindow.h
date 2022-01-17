#ifndef __CLASS_RENDER_WINDOW_H__
#define __CLASS_RENDER_WINDOW_H__




#include "Windows.h"
#include "ContainerUtil.h"


#define STRING_LENGTH_WINDOW_CLASSNAME		256
#define STIRNG_LENGTH_WINDOW_CAPTION		256


class CRenderWindow
{
private :
	HWND									m_hWnd;
	HINSTANCE								m_hInstance;
	WNDCLASSEX								m_WndClassEx;

	int										m_nWidth;
	int										m_nHeight;
	int										m_nDepth;

	char									m_strWindowClassName[ STRING_LENGTH_WINDOW_CLASSNAME ];
	char									m_strWindowCaption[ STIRNG_LENGTH_WINDOW_CAPTION ];

	BOOL									m_bIsFullScreen;
	BOOL									m_bIsActivate;

	WNDPROC									m_fnMessageProc;

public :
	CRenderWindow( void );
	virtual ~CRenderWindow( void );

public :
	virtual BOOL		OnCreate			( HINSTANCE hInstance, int nWidth, int nHeight, int nDepth, BOOL bIsFullScreen, WNDPROC fnProc );
	virtual BOOL		OnDestroy			( void );
	virtual BOOL		OnShow				( void );
	virtual BOOL		OnHide				( void );
	virtual BOOL		OnMessage			( unsigned int nMsg, WPARAM wParam, LPARAM lParam );

private :
	HWND				_MakeWindow			( void );

public :
	HWND				GetWindowHandle		( void ) { return m_hWnd; }
	HINSTANCE			GetWindowInstance	( void ) { return m_hInstance; }

	int					GetWidth			( void ) { return m_nWidth; }
	int					GetHright			( void ) { return m_nHeight; }
	int					GetDepth			( void ) { return m_nDepth; }

	char*				GetWindowClassName	( void ) { return m_strWindowClassName; }
	char*				GetWindowCaption	( void ) { return m_strWindowCaption; }

	BOOL				IsFullScreen		( void ) { return m_bIsFullScreen; }
	BOOL				IsActivate			( void ) { return m_bIsActivate; }

	WNDPROC				GetWndProc			( void ) { return m_fnMessageProc; }
	void				SetWndProc			( WNDPROC fnProc ) { m_fnMessageProc = fnProc; }
};




#endif