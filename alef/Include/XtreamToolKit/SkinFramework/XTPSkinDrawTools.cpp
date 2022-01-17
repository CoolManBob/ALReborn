// XTPSkinDrawTools.cpp
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

#include "stdafx.h"

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPImageManager.h"

#include "XTPSkinDrawTools.h"
#include "XTPSkinObject.h"
#include "XTPSkinManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL XTPFillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
{
	COLORREF clrOld = ::SetBkColor(hdc, clr);
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, CRect(x, y, x + cx, y + cy), NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
	return TRUE;
}

BOOL XTPFillSolidRect(HDC hdc, LPRECT lprc, COLORREF clr)
{
	COLORREF clrOld = ::SetBkColor(hdc, clr);
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
	return TRUE;
}

void XTPFillSolidRect(HDC hdc, int x, int y, int cx, int cy, HBRUSH hBrush)
{
	::FillRect(hdc, CRect(x, y, x + cx, y + cy), hBrush);
}


void AFX_CDECL XTPSkinFrameworkDrawFrame(HDC hdc, LPRECT lprc, int nSize, COLORREF clr)
{
	XTPFillSolidRect(hdc, lprc->left, lprc->top, lprc->right - lprc->left - nSize, nSize, clr);
	XTPFillSolidRect(hdc, lprc->left, lprc->top, nSize, lprc->bottom - lprc->top - nSize, clr);
	XTPFillSolidRect(hdc, lprc->right, lprc->top, -nSize, lprc->bottom - lprc->top, clr);
	XTPFillSolidRect(hdc, lprc->left, lprc->bottom, lprc->right - lprc->left, -nSize, clr);
}

void AFX_CDECL XTPSkinFrameworkDrawFrame(HDC hdc, LPRECT lprc, int nSize, HBRUSH hBrush)
{
	XTPFillSolidRect(hdc, lprc->left, lprc->top, lprc->right - lprc->left - nSize, nSize, hBrush);
	XTPFillSolidRect(hdc, lprc->left, lprc->top, nSize, lprc->bottom - lprc->top - nSize, hBrush);
	XTPFillSolidRect(hdc, lprc->right - nSize, lprc->top, nSize, lprc->bottom - lprc->top, hBrush);
	XTPFillSolidRect(hdc, lprc->left, lprc->bottom - nSize, lprc->right - lprc->left, nSize, hBrush);
}


DWORD DrawDiagonalLine(
	HDC    hdc,
	LPRECT lprc,
	int    iDirection,
	int    iThickness,
	UINT   flags,
	COLORREF clr)
{
	RECT    rc;
	LPLONG  py;
	int     cx;
	int     cy;
	int     dx;
	int     dy;
	LPINT   pc;

	CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

	if (IsRectEmpty(lprc))
		return 0L;

	rc = *lprc;
	iThickness--;

	cy = rc.bottom - rc.top;
	cx = rc.right - rc.left;

	if (!flags && (cy != cx))
		cy -= iThickness * pMetrics->m_cyBorder;

	if (cy >= cx)
	{
		cy /= cx;
		pc = &cy;

		cx = pMetrics->m_cxBorder;

	}
	else
	{
		cx /= cy;
		pc = &cx;

		cy = pMetrics->m_cyBorder;
	}

	dx = cx;
	dy = iDirection * cy;

	*pc = (*pc + iThickness) * pMetrics->m_cyBorder;

	rc.right  -= cx;
	rc.bottom -= cy;

	py = ((iDirection < 0) ? &rc.top : &rc.bottom);

	while ((rc.left <= rc.right) && (rc.top <= rc.bottom))
	{
		if (!(flags & BF_MIDDLE))
		{
			XTPFillSolidRect(hdc, rc.left, *py, cx, cy, clr);
		}
		else
		{
			if (cy > pMetrics->m_cyBorder)
			{
				if (flags & BF_LEFT)
					XTPFillSolidRect(hdc, rc.left, lprc->top, cx, *py - lprc->top + cy, clr);
				else
					XTPFillSolidRect(hdc, rc.left, *py, cx, lprc->bottom - *py, clr);
			}
			else
			{
				if (flags & BF_TOP)
					XTPFillSolidRect(hdc, rc.left, *py, lprc->right - rc.left, cy, clr);
				else

					XTPFillSolidRect(hdc, lprc->left, *py, rc.left - lprc->left + cx, cy, clr);
			}
		}

		rc.left += dx;
		*py     -= dy;
	}

	return MAKELONG(cx, cy);
}


BOOL DrawDiagonal(HDC hdc, LPRECT lprc, COLORREF clrTopLeft, COLORREF clrBottomRight, UINT flags)
{
	int nDirection = 1;

	switch (flags & (BF_RECT | BF_DIAGONAL))
	{
	case BF_DIAGONAL_ENDTOPLEFT:
	case BF_DIAGONAL_ENDBOTTOMRIGHT:
		nDirection = -1;
		break;
	}

	DWORD dAdjust = DrawDiagonalLine(hdc, lprc, nDirection, 1, (flags & ~BF_MIDDLE),
		((flags & BF_BOTTOM) ? clrBottomRight : clrTopLeft));

	if (flags & BF_TOP)
		lprc->left += LOWORD(dAdjust);
	else
		lprc->right -= LOWORD(dAdjust);

	if (flags & BF_RIGHT)
		lprc->top += HIWORD(dAdjust);
	else
		lprc->bottom -= HIWORD(dAdjust);

	return TRUE;
}

#ifndef BDR_OUTER
#define BDR_OUTER       0x0003
#endif

#ifndef BDR_INNER
#define BDR_INNER       0x000c
#endif



BOOL WINAPI XTRSkinFrameworkDrawEdge(HDC hdc, LPRECT lprc, UINT edge, UINT flags)
{
	COLORREF clrTopLeft;
	COLORREF clrBottomRight;

	BOOL fResult = TRUE;

	if (flags & BF_MONO)
		flags |= BF_FLAT;

	RECT rc = *lprc;

	UINT bdrType = (edge & BDR_OUTER);

	CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

	if (bdrType)
	{

DrawBorder:

		if (flags & BF_FLAT)
		{
			if (flags & BF_MONO)
				clrBottomRight = (bdrType & BDR_OUTER) ? GetSysColor(COLOR_WINDOWFRAME) : GetSysColor(COLOR_WINDOW);
			else
				clrBottomRight = (bdrType & BDR_OUTER) ? pMetrics->m_clrEdgeShadow : GetSysColor(COLOR_3DFACE);

			clrTopLeft = clrBottomRight;
		}
		else
		{
			switch (bdrType)
			{
			case BDR_RAISEDOUTER:
				clrTopLeft = ((flags & BF_SOFT) ? pMetrics->m_clrEdgeHighLight : GetSysColor(COLOR_3DLIGHT));
				clrBottomRight = pMetrics->m_clrEdgeDkShadow;
				break;

			case BDR_RAISEDINNER:
				clrTopLeft = ((flags & BF_SOFT) ? GetSysColor(COLOR_3DLIGHT) : pMetrics->m_clrEdgeHighLight);
				clrBottomRight = pMetrics->m_clrEdgeShadow;
				break;

			case BDR_SUNKENOUTER:
				clrTopLeft = ((flags & BF_SOFT) ? pMetrics->m_clrEdgeDkShadow : pMetrics->m_clrEdgeShadow);
				clrBottomRight = pMetrics->m_clrEdgeHighLight;
				break;

			case BDR_SUNKENINNER:
				clrTopLeft = ((flags & BF_SOFT) ? pMetrics->m_clrEdgeShadow : pMetrics->m_clrEdgeDkShadow);
				clrBottomRight = GetSysColor(COLOR_3DLIGHT);
				break;

			default:
				return FALSE;
			}
		}

		if (flags & BF_DIAGONAL)
		{
			fResult = DrawDiagonal(hdc, &rc, clrTopLeft, clrBottomRight, flags);
		}
		else
		{
			if (flags & BF_RIGHT)
			{
				rc.right -= pMetrics->m_cxBorder;
				XTPFillSolidRect(hdc, rc.right, rc.top, pMetrics->m_cxBorder, rc.bottom - rc.top, clrBottomRight);
			}
			if (flags & BF_BOTTOM)
			{
				rc.bottom -= pMetrics->m_cyBorder;
				XTPFillSolidRect(hdc, rc.left, rc.bottom,  rc.right - rc.left, pMetrics->m_cyBorder, clrBottomRight);
			}
			if (flags & BF_LEFT)
			{
				XTPFillSolidRect(hdc, rc.left, rc.top, pMetrics->m_cxBorder, rc.bottom - rc.top, clrTopLeft);
				rc.left += pMetrics->m_cxBorder;
			}
			if (flags & BF_TOP)
			{
				XTPFillSolidRect(hdc, rc.left, rc.top, rc.right - rc.left, pMetrics->m_cyBorder, clrTopLeft);
				rc.top += pMetrics->m_cyBorder;
			}
			fResult = TRUE;
		}
	}

	bdrType = (edge & BDR_INNER);

	if (bdrType)
	{
		edge &= ~BDR_INNER;
		goto DrawBorder;
	}

	if (flags & BF_MIDDLE)
	{
		if (flags & BF_DIAGONAL)
			fResult = FALSE; // TODO!
		else
			fResult = XTPFillSolidRect(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, ((flags & BF_MONO) ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_3DFACE)));
	}

	if (flags & BF_ADJUST)
		*lprc = rc;

	return fResult;
}

int AFX_CDECL XTPGetParentDCClipBox(HWND   pwnd, HDC    hdc, LPRECT lprc)
{
	if (GetClipBox(hdc, lprc) == NULLREGION)
		return FALSE;

	if ((GetClassLongPtr(pwnd, GCL_STYLE) & CS_PARENTDC) == 0)
		return TRUE;

	RECT rc;
	::GetClientRect(pwnd, &rc);

	return IntersectRect(lprc, lprc, &rc);
}

BOOL WINAPI XTRSkinFrameworkDrawFrameControl(HDC hdc, LPRECT lprc,  UINT uType,  UINT uState)
{
	if (uType == DFC_SCROLL && uState == DFCS_SCROLLSIZEGRIP)
	{
		XTPFillSolidRect(hdc, lprc, GetSysColor(COLOR_3DFACE));

		CXTPSkinManagerClass* pClassScrollBar = XTPSkinManager()->GetSkinClass(_T("SCROLLBAR"));
		pClassScrollBar->DrawThemeBackground(CDC::FromHandle(hdc), SBP_SIZEBOX, SZB_RIGHTALIGN, lprc);

		return TRUE;
	}
	int nType = uState & 0xF;

	if (uType == DFC_BUTTON && ((uState | DFCS_PUSHED | BF_ADJUST)  == (DFCS_BUTTONPUSH | DFCS_PUSHED | BF_ADJUST)))
	{
		RECT rc = *lprc;

		XTRSkinFrameworkDrawEdge(hdc, &rc,
			(uState & (DFCS_PUSHED | DFCS_CHECKED)) ? EDGE_SUNKEN : EDGE_RAISED,
				(UINT)(BF_ADJUST | BF_RECT));

		XTPFillSolidRect(hdc, &rc, GetSysColor(COLOR_3DFACE));

		if (uState & BF_ADJUST)
			*lprc = rc;

		return TRUE;
	}

	if ((uType == DFC_SCROLL) && (nType == DFCS_SCROLLLEFT || nType == DFCS_SCROLLRIGHT ||
		nType == DFCS_SCROLLUP ||  nType == DFCS_SCROLLDOWN))
	{
		int nState = 1;
		if (uState & DFCS_PUSHED) nState = 3;
		if (uState & DFCS_INACTIVE) nState = 4;

		CXTPSkinManagerClass* pClassScrollBar = XTPSkinManager()->GetSkinClass(_T("SCROLLBAR"));
		pClassScrollBar->DrawThemeBackground(CDC::FromHandle(hdc), SBP_ARROWBTN, nType * 4 + nState, lprc);

		return TRUE;
	}

	return FALSE;
}

void AFX_CDECL XTPSkinFrameworkGetIconSize(HICON hImage, int* pcx, int* pcy)
{
	*pcx = 32;
	*pcy = 32;

	ICONINFO iconInfo;
	if (!GetIconInfo(hImage, &iconInfo))
		return;

	BITMAP bm;
	if (!GetObject(iconInfo.hbmMask, sizeof(BITMAP), &bm))
		return;

	*pcx = bm.bmWidth;
	*pcy = bm.bmHeight;

	if (!iconInfo.hbmColor)
	{
		*pcy /= 2;
	}

	if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
	if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
}


HPALETTE XTPSkinFrameworkCreatePaletteFromDIBitmap(LPBITMAPINFO pbmi)
{
	DWORD nNumColors = pbmi->bmiHeader.biClrUsed;
	if (!nNumColors && pbmi->bmiHeader.biBitCount <= 8)
		nNumColors = 1 << pbmi->bmiHeader.biBitCount;

	if (!nNumColors)
		return NULL;

	PLOGPALETTE pLogPalette = (PLOGPALETTE)malloc(sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));

	pLogPalette->palNumEntries = (WORD)nNumColors;
	pLogPalette->palVersion = 0x300;

	for (WORD i = 0; i < nNumColors; i++)
	{
		pLogPalette->palPalEntry[i].peBlue = pbmi->bmiColors[i].rgbBlue;
		pLogPalette->palPalEntry[i].peRed = pbmi->bmiColors[i].rgbRed;
		pLogPalette->palPalEntry[i].peGreen = pbmi->bmiColors[i].rgbGreen;
		pLogPalette->palPalEntry[i].peFlags = 0;
	}

	HPALETTE hPalette = CreatePalette(pLogPalette);

	free(pLogPalette);

	return hPalette;
}

HBITMAP XTPSkinFrameworkCreateDDBFromPackedDIBitmap(LPBITMAPINFO pbmi, HPALETTE hPalette)
{
	// Find out how big the bmiColors is
	int nNumColors = (int)pbmi->bmiHeader.biClrUsed;
	if (!nNumColors && pbmi->bmiHeader.biBitCount <= 8)
		nNumColors = 1 << pbmi->bmiHeader.biBitCount;

	LPVOID lpBits = &pbmi->bmiColors[nNumColors];

	HDC hDC = GetDC(NULL);
	HPALETTE hOldPal = SelectPalette(hDC, hPalette, FALSE);

	HBITMAP hBitmap = CreateDIBitmap(hDC, &pbmi->bmiHeader, CBM_INIT, lpBits, pbmi, DIB_RGB_COLORS);

	SelectPalette(hDC, hOldPal, FALSE);
	ReleaseDC(NULL, hDC);

	return hBitmap;
}


HBITMAP AFX_CDECL XTPSkinFrameworkLoadBitmap(HMODULE hModule, LPCTSTR lpszResourceName, BOOL& bAlpha)
{
	if (CXTPImageManagerIcon::IsPngBitmapResource(hModule, lpszResourceName))
	{
		return CXTPImageManagerIcon::LoadBitmapFromResource(hModule, lpszResourceName, &bAlpha);
	}


	#define BitmapWidth(cx, bpp)  (((((cx) * (bpp)) + 31) & ~31) >> 3)

	HRSRC hResource = FindResource(hModule, lpszResourceName, RT_BITMAP);
	if (hResource == NULL)
		return NULL;

	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	if (hGlobal == NULL)
		return NULL;

	LPBITMAPINFO pResourceInfo = (LPBITMAPINFO)::LockResource(hGlobal);
	if (!pResourceInfo)
		return NULL;

	int biSizeImage = BitmapWidth(pResourceInfo->bmiHeader.biWidth,
		pResourceInfo->bmiHeader.biBitCount) * pResourceInfo->bmiHeader.biHeight;


	bAlpha = pResourceInfo->bmiHeader.biBitCount == 32;
	HBITMAP hbmResult = NULL;

	if (SizeofResource(hModule, hResource) >= int(biSizeImage + sizeof(BITMAPINFOHEADER)))
	{
		CDC dcSrc;
		dcSrc.CreateCompatibleDC(NULL);

		if (pResourceInfo->bmiHeader.biBitCount >= 24)
		{
			PBITMAPINFO pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + sizeof(COLORREF) * 3);
			ASSERT(pBitmapInfo != NULL);

			MEMCPY_S(pBitmapInfo, &pResourceInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
			pBitmapInfo->bmiHeader.biSizeImage = biSizeImage;

			BYTE* pDestBits = NULL;
			HBITMAP hBmp = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDestBits, NULL, 0);

			if (hBmp && pDestBits)
			{
				MEMCPY_S(pDestBits, &pResourceInfo->bmiColors, biSizeImage);
				hbmResult = hBmp;
			}
			free(pBitmapInfo);
		}
		else
		{
			HPALETTE hPalette = XTPSkinFrameworkCreatePaletteFromDIBitmap(pResourceInfo);

			hbmResult = XTPSkinFrameworkCreateDDBFromPackedDIBitmap(pResourceInfo, hPalette);

			if (hPalette)
			{
				DeleteObject(hPalette);
			}
		}
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return hbmResult ;
}
