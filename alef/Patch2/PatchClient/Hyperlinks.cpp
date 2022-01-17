// Hyperlinks.cpp
//
// Copyright 2002 Neal Stublen
// All rights reserved.
//
// http://www.awesoftware.com
//

#include "stdafx.h"
#include "Resource.h"
#include <windows.h>

#include "Hyperlinks.h"


#define PROP_ORIGINAL_FONT		TEXT("_Hyperlink_Original_Font_")
#define PROP_ORIGINAL_PROC		TEXT("_Hyperlink_Original_Proc_")
#define PROP_STATIC_HYPERLINK	TEXT("_Hyperlink_From_Static_")
#define PROP_UNDERLINE_FONT		TEXT("_Hyperlink_Underline_Font_")

#ifdef _KOR

//#define URL_HOME		"http://archlord.hangame.com/"
#define URL_HOME		"http://archlord.webzen.co.kr/"
#define URL_REGISTER	"http://member.hangame.com/register/index.nhn"
#define URL_CUSTOMER	"http://archlord.hangame.com/support/faq.nhn"
#define URL_DRIVER		"http://archlord.hangame.com/data/drivers.nhn"

#elif _ENG

#define URL_HOME		"http://www.archlordgame.com"
#define URL_REGISTER	"https://cogaccounts.codemasters.com"
#define URL_CUSTOMER	"http://www.archlordgame.com/support"
#define URL_DRIVER		"http://archlord.naver.com/data/drivers.nhn"

#elif _JPN

#define URL_HOME		"http://www.archlord.jp"
#define URL_REGISTER	""
#define URL_CUSTOMER	"http://customer.hangame.co.jp/helpQAList.nhn?layer=2&category=245"
#define URL_DRIVER		""

#else

#define URL_HOME		""
#define URL_REGISTER	""
#define URL_CUSTOMER	""
#define URL_DRIVER		""

#endif


LRESULT CALLBACK _HyperlinkParentProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pfnOrigProc = (WNDPROC) GetProp(hwnd, PROP_ORIGINAL_PROC);

	switch (message)
	{
	case WM_COMMAND:
		{			
			//. 2005. 10. 18. Nonstopdj
			//. 추가된 새로운 하이퍼링크
			if( LOWORD(wParam) == IDC_HOMEPAGE)
			{
				ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), URL_HOME, NULL, SW_SHOW);
			}
			else if(LOWORD(wParam) == IDC_JOIN)
			{
#ifndef _JPN
				ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), URL_REGISTER, NULL, SW_SHOW);
#endif
			}
			else if(LOWORD(wParam) == IDC_CUSTOMER)
			{
				ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), URL_CUSTOMER, NULL, SW_SHOW);
			}
			else if(LOWORD(wParam) == IDC_DRIVER)
			{
#ifdef _KOR
				ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), URL_DRIVER, NULL, SW_SHOW);
#endif
			}
			break;
		}
	case WM_CTLCOLORSTATIC:
		{			
			HDC hdc = (HDC) wParam;
			HWND hwndCtl = (HWND) lParam;

			if( GetProp( hwndCtl, PROP_STATIC_HYPERLINK ) )
			{
				LRESULT lResult = CallWindowProc( pfnOrigProc, hwnd, message, wParam, lParam );
				SetTextColor( hdc, RGB( 137, 137, 137 ) );
				return lResult;
			}
			break;
		}
	case WM_DESTROY:
		{			
			SetWindowLong( hwnd, GWL_WNDPROC, (LONG) pfnOrigProc );
			RemoveProp( hwnd, PROP_ORIGINAL_PROC );
			break;
		}
	}

	return CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK _HyperlinkProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pfnOrigProc = (WNDPROC) GetProp(hwnd, PROP_ORIGINAL_PROC);

	switch (message)
	{
	case WM_DESTROY:
		{			
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG) pfnOrigProc);
			RemoveProp(hwnd, PROP_ORIGINAL_PROC);

			HFONT hOrigFont = (HFONT) GetProp(hwnd, PROP_ORIGINAL_FONT);
			SendMessage(hwnd, WM_SETFONT, (WPARAM) hOrigFont, 0);
			RemoveProp(hwnd, PROP_ORIGINAL_FONT);

			HFONT hFont = (HFONT) GetProp(hwnd, PROP_UNDERLINE_FONT);
			DeleteObject(hFont);
			RemoveProp(hwnd, PROP_UNDERLINE_FONT);

			RemoveProp(hwnd, PROP_STATIC_HYPERLINK);
			break;
		}
	case WM_MOUSEMOVE:
		{			
			if (GetCapture() != hwnd)
			{
				HFONT hFont = (HFONT) GetProp(hwnd, PROP_UNDERLINE_FONT);
				SendMessage(hwnd, WM_SETFONT, (WPARAM) hFont, FALSE);
				InvalidateRect(hwnd, NULL, FALSE);
				SetCapture(hwnd);
			}
			else
			{				
				RECT rect;
				GetWindowRect(hwnd, &rect);

				POINT pt = { LOWORD(lParam), HIWORD(lParam) };
				ClientToScreen(hwnd, &pt);

				if (!PtInRect(&rect, pt))
				{
					HFONT hFont = (HFONT) GetProp(hwnd, PROP_ORIGINAL_FONT);
					SendMessage(hwnd, WM_SETFONT, (WPARAM) hFont, FALSE);
					InvalidateRect(hwnd, NULL, FALSE);
					ReleaseCapture();

					//. 2005. 10. 18. Nonstopdj
					//. Hyperlink static Text의 rect을 Invalidate한다.
					RECT rcControl;
					AfxGetMainWnd( )->GetWindowRect(&rcControl);
					rcControl.left	= rect.left - rcControl.left;
					rcControl.top	= rect.top - rcControl.top;
					rcControl.right	= rcControl.left + 254;
					rcControl.bottom= rcControl.top	+ 40;
					AfxGetMainWnd( )->InvalidateRect(&rcControl , FALSE);
				}
			}
			break;
		}
	case WM_SETCURSOR:
		{			
			// Since IDC_HAND is not available on all operating systems,
			// we will load the arrow cursor if IDC_HAND is not present.
			HCURSOR hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
			if (NULL == hCursor)
			{
				hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
			}
			SetCursor(hCursor);
			return TRUE;
		}
	}

	return CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
}

BOOL ConvertStaticToHyperlink(HWND hwnd)
{
	// Subclass the parent so we can color the controls as we desire.

	HWND hwndParent = GetParent(hwnd);
	if( hwndParent )
	{
		WNDPROC pfnOrigProc = (WNDPROC) GetWindowLong( hwndParent, GWL_WNDPROC );
		if (pfnOrigProc != _HyperlinkParentProc)
		{
			SetProp( hwndParent, PROP_ORIGINAL_PROC, (HANDLE)pfnOrigProc);
			SetWindowLong( hwndParent, GWL_WNDPROC, (LONG) (WNDPROC) _HyperlinkParentProc );
		}
	}

	// Make sure the control will send notifications.
	DWORD dwStyle = GetWindowLong( hwnd, GWL_STYLE );
	SetWindowLong(hwnd, GWL_STYLE, dwStyle | SS_NOTIFY);

	// Subclass the existing control.
	WNDPROC pfnOrigProc = (WNDPROC) GetWindowLong(hwnd, GWL_WNDPROC);
	SetProp( hwnd, PROP_ORIGINAL_PROC, (HANDLE) pfnOrigProc );
	SetWindowLong( hwnd, GWL_WNDPROC, (LONG)(WNDPROC)_HyperlinkProc );

	// Create an updated font by adding an underline.
	HFONT hOrigFont = (HFONT)SendMessage( hwnd, WM_GETFONT, 0, 0 );
	SetProp( hwnd, PROP_ORIGINAL_FONT, (HANDLE) hOrigFont );

	LOGFONT lf;
	GetObject(hOrigFont, sizeof(lf), &lf);
	lf.lfUnderline = TRUE;

	HFONT hFont = CreateFontIndirect(&lf);
	SetProp( hwnd, PROP_UNDERLINE_FONT, (HANDLE) hFont );

	// Set a flag on the control so we know what color it should be.
	SetProp( hwnd, PROP_STATIC_HYPERLINK, (HANDLE)1 );

	return TRUE;
}

BOOL ConvertStaticToHyperlink(HWND hwndParent, UINT uiCtlId )
{
	return ConvertStaticToHyperlink( GetDlgItem( hwndParent, uiCtlId ) );
}
