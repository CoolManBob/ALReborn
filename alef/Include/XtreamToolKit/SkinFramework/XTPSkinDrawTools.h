// XTPSkinDrawTools.h
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSKINDRAWTOOLS_H__)
#define __XTPSKINDRAWTOOLS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE

// Private function

BOOL WINAPI XTRSkinFrameworkDrawEdge(HDC hdc, LPRECT lprc, UINT edge, UINT flags);
BOOL WINAPI XTRSkinFrameworkDrawFrameControl(HDC hdc, LPRECT lprc,  UINT uType,  UINT uState);
int AFX_CDECL XTPGetParentDCClipBox(HWND   pwnd, HDC    hdc, LPRECT lprc);
void AFX_CDECL XTPSkinFrameworkGetIconSize(HICON hImage, int* pcx, int* pcy);
HBITMAP AFX_CDECL XTPSkinFrameworkLoadBitmap(HMODULE hModule, LPCTSTR lpszResourceName, BOOL& bAlpha);
void AFX_CDECL XTPSkinFrameworkDrawFrame(HDC hdc, LPRECT lprc, int nSize, COLORREF clr);
void AFX_CDECL XTPSkinFrameworkDrawFrame(HDC hdc, LPRECT lprc, int nSize, HBRUSH hBrush);

//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTPSKINDRAWTOOLS_H__)
