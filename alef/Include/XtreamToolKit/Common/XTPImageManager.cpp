// XTPImageManager.cpp : implementation of the CXTPImageManager class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#include <math.h>

#include "XTPVC80Helpers.h"
#include "XTPColorManager.h"
#include "XTPDrawHelpers.h"
#include "XTPSystemHelpers.h"
#include "XTPMacros.h"
#include "XTPPropExchange.h"
#include "XTPImageManager.h"

#ifdef _XTP_INCLUDE_GRAPHICLIBRARY
#include "GraphicLibrary/XTPGraphicBitmapPng.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DESTROYICON(hIcon) if (hIcon) { ::DestroyIcon(hIcon); hIcon = 0; }
#define FREE(hHandle) if (hHandle) { free(hHandle); hHandle = 0; }

double CXTPImageManager::m_dDisabledAlphaFactor = 1.0;
double CXTPImageManager::m_dDisabledBrightnessFactor = 0.5;
BOOL CXTPImageManager::m_bAutoResample = FALSE;
CLIPFORMAT CXTPImageManager::m_nImageClipFormat = (CLIPFORMAT)RegisterClipboardFormat(_T("Xtreme ToolBar Image"));
CLIPFORMAT CXTPImageManager::m_nAlphaClipFormat = (CLIPFORMAT)RegisterClipboardFormat(_T("Alpha Bitmap Image"));

#ifndef NOMIRRORBITMAP
#define NOMIRRORBITMAP      (DWORD)0x80000000 /* Do not Mirror the bitmap in this call */
#endif

//---------------------------------------------------------------------------
//    Should be a prime number:
// 37, 53, 79 , 101, 127, 199, 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007, 16001, 32003, 48017, 64007
#define XTP_IMAGEMAN_HASH_TABLE_SIZE 127

//////////////////////////////////////////////////////////////////////////
// Common

BOOL CXTPImageManager::BitmapsCompatible(LPBITMAP lpbm1, LPBITMAP lpbm2) const
{
	if (lpbm1->bmBitsPixel != lpbm2->bmBitsPixel)
		return FALSE;

	if (lpbm1->bmPlanes != lpbm2->bmPlanes)
		return FALSE;

	if (lpbm1->bmWidth != lpbm2->bmWidth)
		return FALSE;

	if (lpbm1->bmHeight != lpbm2->bmHeight)
		return FALSE;

	return TRUE;
}

BOOL CXTPImageManager::McTransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
	int nWidthDest, int nHeightDest, HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
	int nHeightSrc, UINT crTransparent) const
{

	BOOL bResult = FALSE;

	if (!hdcDest || !hdcSrc)
		return FALSE;

	HDC hdcMask = NULL;
	HDC hdcTmpSrc = NULL;
	HBITMAP hbmTransMask = NULL;
	HBITMAP oldhbmTransMask = NULL;
	HBITMAP hbmTmpSrc = NULL;
	HBITMAP oldhbmTmpSrc = NULL;


	COLORREF OldBkColor = ::SetBkColor(hdcDest, RGB(255, 255, 255));
	COLORREF OldTextColor = ::SetTextColor(hdcDest, RGB(0, 0, 0));

	if ((hdcMask = ::CreateCompatibleDC(hdcDest)) == NULL)
	{
		goto ClearUp;
	}

	if ((hdcTmpSrc = ::CreateCompatibleDC(hdcSrc))==NULL)
	{
		goto ClearUp;
	}

	if ((hbmTmpSrc = ::CreateCompatibleBitmap(hdcDest,nWidthSrc,nHeightSrc))==NULL)
	{
		goto ClearUp;
	}

	oldhbmTmpSrc = (HBITMAP)::SelectObject(hdcTmpSrc,hbmTmpSrc);

	if (!::BitBlt(hdcTmpSrc, 0, 0, nWidthSrc, nHeightSrc, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY))
	{
		goto ClearUp;
	}

	if ((hbmTransMask = ::CreateBitmap(nWidthSrc, nHeightSrc, 1, 1, NULL)) == NULL)
	{
		goto ClearUp;
	}

	oldhbmTransMask = (HBITMAP)::SelectObject(hdcMask,hbmTransMask);

	::SetBkColor(hdcTmpSrc,crTransparent);
	if (!::BitBlt(hdcMask, 0, 0, nWidthSrc, nHeightSrc, hdcTmpSrc, 0, 0, SRCCOPY))
	{
		goto ClearUp;
	}

	if (crTransparent != RGB(0, 0, 0))
	{
		::SetBkColor(hdcTmpSrc, RGB(0, 0, 0));
		::SetTextColor(hdcTmpSrc, RGB(255, 255, 255));

		if (!::BitBlt(hdcTmpSrc, 0, 0, nWidthSrc, nHeightSrc, hdcMask, 0, 0, SRCAND))
		{
			goto ClearUp;
		}
	}

	if ((nWidthDest == nWidthSrc) && (nHeightDest == nHeightSrc))
	{
		if (!::BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthSrc, nHeightSrc, hdcMask, 0, 0, SRCAND))
		{
			goto ClearUp;
		}
		if (!::BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthSrc, nHeightSrc, hdcTmpSrc, 0, 0, SRCPAINT))
		{
			goto ClearUp;
		}
	}
	else
	{
		if (!::StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcMask, 0, 0, nWidthSrc, nHeightSrc,SRCAND))
		{
			goto ClearUp;
		}
		if (!::StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcTmpSrc, 0, 0,  nWidthSrc, nHeightSrc,SRCPAINT))
		{
			goto ClearUp;
		}
	}
	bResult = TRUE;

ClearUp:
	if (hdcMask)
	{
		if (oldhbmTransMask)
			::SelectObject(hdcMask,oldhbmTransMask);
		::DeleteDC(hdcMask);
	}

	if (hbmTmpSrc)
	{
		if (oldhbmTmpSrc)
			::SelectObject(hdcTmpSrc,oldhbmTmpSrc);
		::DeleteObject(hbmTmpSrc);
	}

	if (hdcTmpSrc)
		::DeleteDC(hdcTmpSrc);

	if (hbmTransMask)
		::DeleteObject(hbmTransMask);

	::SetBkColor(hdcDest, OldBkColor);
	::SetTextColor(hdcDest,OldTextColor);

	return bResult;
}

BOOL CXTPImageManager::IsWindow2000() const
{
	return XTPSystemVersion()->IsWin2KOrGreater();
}

BOOL CXTPImageManager::TransparentBlt(HDC hdcDest, const CRect& rcDest,HDC hdcSrc,
	const CRect& rcSrc, UINT crTransparent) const
{
	if (IsWindow2000() && XTPImageManager()->m_pfnTransparentBlt)
	{
		return XTPImageManager()->m_pfnTransparentBlt(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
			hdcSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), crTransparent);
	}

	return McTransparentBlt(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
		hdcSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), crTransparent);
}

BOOL CXTPImageManager::BlendImages(HBITMAP hbmSrc1, BOOL bRTL1, HBITMAP hbmSrc2, BOOL bRTL2, HBITMAP hbmDst) const
{
	BITMAP bmSrc1, bmSrc2, bmDst;
	RGBQUAD *lprgbSrc1, *lprgbSrc2, *lprgbDst;
	DWORD dwWidthBytes;
	int x, y;

	// Get weighting value for second source image.

	// Get information about the surfaces you were passed.
	if (!GetObject(hbmSrc1, sizeof(BITMAP), &bmSrc1)) return FALSE;
	if (!GetObject(hbmSrc2, sizeof(BITMAP), &bmSrc2)) return FALSE;
	if (!GetObject(hbmDst,  sizeof(BITMAP), &bmDst))  return FALSE;

	// Make sure you have data that meets your requirements.
	if (!BitmapsCompatible(&bmSrc1, &bmSrc2)) return FALSE;
	if (!BitmapsCompatible(&bmSrc1, &bmDst))  return FALSE;
	if (bmSrc1.bmBitsPixel != 32) return FALSE;
	if (bmSrc1.bmPlanes != 1) return FALSE;
	if (!bmSrc1.bmBits || !bmSrc2.bmBits || !bmDst.bmBits) return FALSE;

	dwWidthBytes = bmDst.bmWidthBytes;

	// Initialize the surface pointers.
	lprgbSrc1 = (RGBQUAD*)bmSrc1.bmBits;
	lprgbSrc2 = (RGBQUAD*)bmSrc2.bmBits;
	lprgbDst  = (RGBQUAD*)bmDst.bmBits;

	#define F(a)  BYTE(gbSrc1.##a + (255 - gbSrc1.rgbReserved) * gbSrc2.##a / 255)

	for (y = 0; y < bmDst.bmHeight; y++)
	{
		for (x = 0; x < bmDst.bmWidth; x++)
		{
			RGBQUAD& gbSrc1 = bRTL1 ? lprgbSrc1[bmDst.bmWidth - 1 - x] : lprgbSrc1[x];
			RGBQUAD& gbSrc2 = bRTL2 ? lprgbSrc2[bmDst.bmWidth - 1 - x] : lprgbSrc2[x];

			lprgbDst[x].rgbRed   = F(rgbRed);
			lprgbDst[x].rgbGreen = F(rgbGreen);
			lprgbDst[x].rgbBlue  = F(rgbBlue);
		}

		// Move to next scan line.
		lprgbSrc1 = (RGBQUAD *)((LPBYTE)lprgbSrc1 + dwWidthBytes);
		lprgbSrc2 = (RGBQUAD *)((LPBYTE)lprgbSrc2 + dwWidthBytes);
		lprgbDst  = (RGBQUAD *)((LPBYTE)lprgbDst  + dwWidthBytes);
	}

	return TRUE;
}

HBITMAP AFX_CDECL CXTPImageManager::Create32BPPDIBSection(HDC hDC, int iWidth, int iHeight, LPBYTE* lpBits)
{
	BITMAPINFO bmi;
	HBITMAP hbm;
	LPBYTE pBits;

	// Initialize header to 0s.
	ZeroMemory(&bmi, sizeof(bmi));

	// Fill out the fields you care about.
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = iWidth;
	bmi.bmiHeader.biHeight = iHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Create the surface.
	hbm = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID*)&pBits, NULL, 0);

	if (lpBits)
	{
		*lpBits = pBits;
	}

	return(hbm);
}

BOOL CXTPImageManager::DoAlphaBlend(HDC hdcDest,                 // Handle to destination DC.
	int nXOriginDest,            // X-coord of upper-left corner.
	int nYOriginDest,            // Y-coord of upper-left corner.
	int nWidthDest,              // Destination width.
	int nHeightDest,             // Destination height.
	HDC hdcSrc,                  // Handle to source DC.
	int nXOriginSrc,             // X-coord of upper-left corner.
	int nYOriginSrc,             // Y-coord of upper-left corner.
	int nWidthSrc,               // Source width.
	int nHeightSrc              // Source height.
) const
{
	HDC      hdcSrc1 = NULL;
	HDC      hdcSrc2 = NULL;
	HBITMAP  hbmSrc1 = NULL, hbmOldSrc1 = NULL;
	HBITMAP  hbmSrc2 = NULL, hbmOldSrc2 = NULL;
	HBITMAP  hbmDst  = NULL;
	BOOL     bReturn = FALSE;
	BOOL     bRTLSrc    = XTPDrawHelpers()->IsContextRTL(hdcSrc);
	BOOL     bRTLDest   = XTPDrawHelpers()->IsContextRTL(hdcDest);

	// Create surfaces for sources and destination images.
	hbmSrc1 = CXTPImageManager::Create32BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
	if (!hbmSrc1) goto HANDLEERROR;

	hbmSrc2 = CXTPImageManager::Create32BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
	if (!hbmSrc2) goto HANDLEERROR;

	hbmDst  = CXTPImageManager::Create32BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
	if (!hbmDst) goto HANDLEERROR;

	// Create HDCs to hold our surfaces.
	hdcSrc1 = CreateCompatibleDC(hdcSrc);
	if (!hdcSrc1) goto HANDLEERROR;

	hdcSrc2 = CreateCompatibleDC(hdcSrc);
	if (!hdcSrc2) goto HANDLEERROR;

	XTPDrawHelpers()->SetContextRTL(hdcSrc2, FALSE);
	XTPDrawHelpers()->SetContextRTL(hdcSrc1, FALSE);
	
	// Prepare the surfaces for drawing.
	hbmOldSrc1 = (HBITMAP)SelectObject(hdcSrc1, hbmSrc1);
	hbmOldSrc2 = (HBITMAP)SelectObject(hdcSrc2, hbmSrc2);
	SetStretchBltMode(hdcSrc1, COLORONCOLOR);
	SetStretchBltMode(hdcSrc2, COLORONCOLOR);

	
	// Capture a copy of the source area.
	if (!StretchBlt(hdcSrc1, 0,0,nWidthDest,nHeightDest,
		hdcSrc,  nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc,
		SRCCOPY))
		goto HANDLEERROR;
	
	
	// Capture a copy of the destination area.
	if (!BitBlt(hdcSrc2, 0, 0, nWidthDest, nHeightDest,
		hdcDest, nXOriginDest, nYOriginDest, SRCCOPY))
		goto HANDLEERROR;



	SelectObject(hdcSrc1, hbmOldSrc1); hbmOldSrc1 = NULL;
	SelectObject(hdcSrc2, hbmOldSrc2); hbmOldSrc2 = NULL;

	// Blend the two source areas to create the destination image.
	bReturn = BlendImages(hbmSrc1, bRTLSrc, hbmSrc2, bRTLDest, hbmDst);


	// Display the blended (destination) image to the target HDC.
	if (bReturn)
	{
		hbmOldSrc1 = (HBITMAP)SelectObject(hdcSrc1, hbmDst);
	
		BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
			hdcSrc1, 0,0, SRCCOPY | (bRTLDest ? NOMIRRORBITMAP : 0));
		
		SelectObject(hdcSrc1, hbmOldSrc1);
		hbmOldSrc1 = NULL;
	}

	// Clean up the rest of the objects you created.

HANDLEERROR:

	if (hbmOldSrc1) SelectObject(hdcSrc1, hbmOldSrc1);
	if (hbmOldSrc2) SelectObject(hdcSrc2, hbmOldSrc2);
	if (hdcSrc1) DeleteDC(hdcSrc1);
	if (hdcSrc2) DeleteDC(hdcSrc2);
	if (hbmSrc1) DeleteObject(hbmSrc1);
	if (hbmSrc2) DeleteObject(hbmSrc2);
	if (hbmDst) DeleteObject(hbmDst);

	return bReturn;
}


BOOL CXTPImageManager::AlphaBlend(HDC hdcDest, const CRect& rcDest, HDC hdcSrc, const CRect& rcSrc) const
{
	if (XTPDrawHelpers()->IsContextRTL(hdcDest) && XTPSystemVersion()->IsWin9x())
	{
		return DoAlphaBlend(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
			hdcSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
	}
	
	if (XTPImageManager()->m_pfnAlphaBlend && (IsWindow2000() || (rcSrc.top == 0 && rcSrc.left == 0)))
	{
		BLENDFUNCTION bf;
		ZeroMemory(&bf, sizeof(BLENDFUNCTION));

		bf.AlphaFormat  = 0x01;
		bf.SourceConstantAlpha = 255;
		bf.BlendOp = AC_SRC_OVER;

		return XTPImageManager()->m_pfnAlphaBlend(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
			hdcSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), bf);

	}
	return DoAlphaBlend(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
		hdcSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
}



//////////////////////////////////////////////////////////////////////////
// CXTPImageManagerImageList

CXTPImageManagerImageList::CXTPImageManagerImageList()
{
	m_hImageList = NULL;
	m_bDestroyImageList = FALSE;
	m_nBaseCommand = 0;
	m_pManager = NULL;
}

CXTPImageManagerImageList::~CXTPImageManagerImageList()
{
	if (m_hImageList && m_bDestroyImageList)
	{
		ImageList_Destroy(m_hImageList);
	}

	RemoveAll();
}

void CXTPImageManagerImageList::RemoveAll()
{
	UINT nCommand;
	CXTPImageManagerIcon* pIcon;

	POSITION pos = m_mapIcons.GetStartPosition();
	while (pos)
	{
		m_mapIcons.GetNextAssoc(pos, nCommand, pIcon);
		pIcon->InternalRelease();
	}

	m_mapIcons.RemoveAll();
}

BOOL CXTPImageManagerImageList::Lookup(UINT nCommand)
{
	return ((nCommand >= m_nBaseCommand) &&
		(nCommand < m_nBaseCommand + ImageList_GetImageCount(m_hImageList)));
}

CSize CXTPImageManagerImageList::GetIconSize() const
{
	int nWidth = 0, nHeight = 0;
	VERIFY(ImageList_GetIconSize(m_hImageList, &nWidth, &nHeight));

	return CSize(nWidth, nHeight);
}

CXTPImageManagerIcon* CXTPImageManagerImageList::GetIcon(UINT nCommand)
{
	CXTPImageManagerIcon* pImage = NULL;

	if (m_mapIcons.Lookup(nCommand, pImage))
	{
		return pImage;
	}

	CSize sz = GetIconSize();

	pImage = new CXTPImageManagerIcon(nCommand, sz.cx, sz.cy, this);
	m_mapIcons.SetAt(nCommand, pImage);

	return pImage;
}

void CXTPImageManagerImageList::Draw(CDC* pDC, CPoint pt, CXTPImageManagerIcon* pIcon, CSize szIcon)
{
	ImageList_DrawEx(m_hImageList, pIcon->GetID() - m_nBaseCommand, pDC->GetSafeHdc(),
		pt.x, pt.y, szIcon.cx, szIcon.cy, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_NORMAL);
}

//////////////////////////////////////////////////////////////////////////
// CXTPImageManagerIcon


struct CXTPImageManagerIcon::ICONIMAGE
{
	BITMAPINFOHEADER    icHeader;   // DIB header
	RGBQUAD             icColors[1];// Color table
	BYTE                icXOR[1];   // DIB bits for XOR mask
	BYTE                icAND[1];   // DIB bits for AND mask
};

#pragma pack(push, 2)
// resources are WORD-aligned

struct CXTPImageManagerIcon::ICONDIRENTRY
{
	BYTE        bWidth;             // Width, in pixels, of the image
	BYTE        bHeight;            // Height, in pixels, of the image
	BYTE        bColorCount;        // Number of colors in image (0 if >= 8bpp)
	BYTE        bReserved;          // Reserved (must be 0)
	WORD        wPlanes;            // Color Planes
	WORD        wBitCount;          // Bits per pixel
	DWORD       dwBytesInRes;       // How many bytes in this resource ?
	DWORD       dwImageOffset;      // Where in the file is this image ?
};

struct CXTPImageManagerIcon::ICONDIRHEADER
{
	WORD            idReserved;     // Reserved (must be 0)
	WORD            idType;         // Resource Type (1 for icons)
	WORD            idCount;        // How many images ?
};

struct CXTPImageManagerIcon::GRPICONDIRENTRY
{
	BYTE   bWidth;                  // Width, in pixels, of the image
	BYTE   bHeight;                 // Height, in pixels, of the image
	BYTE   bColorCount;             // Number of colors in image (0 if >= 8bpp)
	BYTE   bReserved;               // Reserved
	WORD   wPlanes;                 // Color Planes
	WORD   wBitCount;               // Bits per pixel
	DWORD  dwBytesInRes;            // how many bytes in this resource ?
	WORD   nID;                     // the ID
};

struct CXTPImageManagerIcon::GRPICONDIR
{
	WORD              idReserved;   // Reserved (must be 0)
	WORD              idType;       // Resource type (1 for icons)
	WORD              idCount;      // How many images ?
	GRPICONDIRENTRY   idEntries[1]; // The entries for each image
};

#pragma pack(pop)

BOOL CXTPImageManagerIcon::IsAlphaBitmapFile(LPCTSTR pszFileName)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwBytesRead;

	BITMAPFILEHEADER fileHeader;

	if (!ReadFile(hFile, &fileHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, 0) || dwBytesRead != sizeof(BITMAPFILEHEADER))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	BITMAPINFOHEADER infoHeader;

	if (!ReadFile(hFile, &infoHeader, sizeof(BITMAPINFOHEADER), &dwBytesRead, 0) || dwBytesRead != sizeof(BITMAPINFOHEADER))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	BOOL bResult = infoHeader.biBitCount == 32;

	CloseHandle(hFile);

	return bResult;
}

BOOL CXTPImageManagerIcon::IsPngBitmapFile(LPCTSTR pszFileName)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwBytesRead;

	BYTE fileHeader[4];
	BYTE png_signature[4] = {137, 80, 78, 71};

	if (!ReadFile(hFile, &fileHeader, sizeof(fileHeader), &dwBytesRead, 0) || dwBytesRead != sizeof(fileHeader))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	BOOL bResult = memcmp(&fileHeader, &png_signature, 4) == 0;

	CloseHandle(hFile);

	return bResult;
}


HBITMAP AFX_CDECL CXTPImageManagerIcon::LoadBitmapFromFile(LPCTSTR lpszFileName, BOOL* lbAlphaBitmap)
{
#ifdef _XTP_INCLUDE_GRAPHICLIBRARY
	BOOL bPngBitmap = CXTPImageManagerIcon::IsPngBitmapFile(lpszFileName);

	if (bPngBitmap)
	{
		CXTPGraphicBitmapPng bmp;
		if (!bmp.LoadFromFile(lpszFileName))
			return NULL;

		if (lbAlphaBitmap)
		{
			*lbAlphaBitmap = bmp.IsAlpha();
		}
		return (HBITMAP)bmp.Detach();
	}
#endif

	BOOL bAlphaBitmap = CXTPImageManagerIcon::IsAlphaBitmapFile(lpszFileName);

	HBITMAP hBmp = (HBITMAP)LoadImage(0, lpszFileName,
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | (bAlphaBitmap ? LR_CREATEDIBSECTION : 0) | LR_LOADFROMFILE);

	if (lbAlphaBitmap)
	{
		*lbAlphaBitmap = bAlphaBitmap;
	}

	return hBmp;
}


BOOL AFX_CDECL CXTPImageManagerIcon::IsPngBitmapResource(HMODULE hModule, LPCTSTR lpBitmapName)
{
	BYTE png_signature[4] = {137, 80, 78, 71};

	HRSRC hResource = ::FindResource(hModule, lpBitmapName, _T("PNG"));

	if (hResource == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	if (hGlobal == NULL)
		return FALSE;

	LPBYTE pBitmapInfoHeader = (LPBYTE)::LockResource(hGlobal);
	BOOL bPngBitmap = memcmp(pBitmapInfoHeader, &png_signature, 4) == 0;

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return (bPngBitmap);
}

BOOL AFX_CDECL CXTPImageManagerIcon::IsAlphaBitmapResource(HMODULE hModule, LPCTSTR lpBitmapName)
{
	HRSRC hResource = ::FindResource(hModule, lpBitmapName, RT_BITMAP);

	if (hResource == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	if (hGlobal == NULL)
		return FALSE;

	LPBITMAPINFOHEADER pBitmapInfoHeader = (LPBITMAPINFOHEADER)::LockResource(hGlobal);
	ASSERT(pBitmapInfoHeader != NULL);
	if (!pBitmapInfoHeader)
		return FALSE;

	BOOL bAlpahBitmap = pBitmapInfoHeader->biBitCount == 32;
	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return (bAlpahBitmap);
}

HBITMAP AFX_CDECL CXTPImageManagerIcon::LoadBitmapFromResource(LPCTSTR lpszResource, BOOL* lbAlphaBitmap)
{
	HMODULE hModule;

#ifdef _XTP_INCLUDE_GRAPHICLIBRARY
	hModule = AfxFindResourceHandle(lpszResource, _T("PNG"));

	BOOL bPngBitmap = IsPngBitmapResource(hModule, lpszResource);
	if (bPngBitmap)
	{
		CXTPGraphicBitmapPng bmpIcons;

		HRSRC hResource = ::FindResource(hModule, lpszResource, _T("PNG"));

		if (!bmpIcons.LoadFromResource(hModule, hResource))
			return NULL;

		if (lbAlphaBitmap)
		{
			*lbAlphaBitmap = bmpIcons.IsAlpha();
		}

		return (HBITMAP)bmpIcons.Detach();
	}
#endif

	hModule = AfxFindResourceHandle(lpszResource, RT_BITMAP);

	CBitmap bmpIcons;
	BOOL bAlphaBitmap = IsAlphaBitmapResource(hModule, lpszResource);

	if (lbAlphaBitmap)
	{
		*lbAlphaBitmap = bAlphaBitmap;
	}

	if (bAlphaBitmap)
	{
		return LoadAlphaBitmap(hModule, lpszResource);
	}

	return (HBITMAP)LoadImage(hModule, lpszResource, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
}

HBITMAP AFX_CDECL CXTPImageManagerIcon::LoadBitmapFromResource(HMODULE hModule, LPCTSTR lpszResource, BOOL* lbAlphaBitmap)
{
#ifdef _XTP_INCLUDE_GRAPHICLIBRARY

	BOOL bPngBitmap = IsPngBitmapResource(hModule, lpszResource);
	if (bPngBitmap)
	{
		CXTPGraphicBitmapPng bmpIcons;

		HRSRC hResource = ::FindResource(hModule, lpszResource, _T("PNG"));

		if (!bmpIcons.LoadFromResource(hModule, hResource))
			return NULL;

		if (lbAlphaBitmap)
		{
			*lbAlphaBitmap = bmpIcons.IsAlpha();
		}

		return (HBITMAP)bmpIcons.Detach();
	}
#endif

	CBitmap bmpIcons;
	BOOL bAlphaBitmap = IsAlphaBitmapResource(hModule, lpszResource);

	if (lbAlphaBitmap)
	{
		*lbAlphaBitmap = bAlphaBitmap;
	}

	if (bAlphaBitmap)
	{
		return LoadAlphaBitmap(hModule, lpszResource);
	}

	return (HBITMAP)LoadImage(hModule, lpszResource, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
}

HBITMAP CXTPImageManagerIcon::LoadAlphaBitmap(UINT nIDResource)
{
	return LoadAlphaBitmap(AfxFindResourceHandle(MAKEINTRESOURCE(nIDResource), RT_BITMAP),
		MAKEINTRESOURCE(nIDResource));
}

HBITMAP CXTPImageManagerIcon::LoadAlphaBitmap(HMODULE hModule, LPCTSTR lpszResource)
{
	HRSRC hResource = ::FindResource(hModule, lpszResource, RT_BITMAP);
	if (hResource == NULL)
		return NULL;

	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	if (hGlobal == NULL)
		return NULL;

	LPBITMAPINFO pResourceInfo = (LPBITMAPINFO)::LockResource(hGlobal);
	ASSERT(pResourceInfo != NULL);
	if (!pResourceInfo)
		return NULL;

	int biSizeImage = pResourceInfo->bmiHeader.biHeight * pResourceInfo->bmiHeader.biWidth * 4;

	HBITMAP hbmResult = NULL;

	if (pResourceInfo->bmiHeader.biBitCount == 32 &&
		SizeofResource(hModule, hResource) >= int(biSizeImage + sizeof(BITMAPINFOHEADER)))
	{
		CDC dcSrc;
		dcSrc.CreateCompatibleDC(NULL);

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
		FREE(pBitmapInfo);
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return hbmResult;

}



HBITMAP CXTPImageManagerIcon::LoadAlphaIcon(LPCTSTR pszFileName, int nWidth)
{

	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	ICONDIRHEADER idh;

	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, &idh, sizeof(ICONDIRHEADER) , &dwBytesRead, NULL) || dwBytesRead != sizeof(ICONDIRHEADER))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (idh.idType != 1 || idh.idCount == 0)
	{
		CloseHandle(hFile);
		return FALSE;
	}


	ICONDIRENTRY* pEntries = (ICONDIRENTRY*)malloc(sizeof(ICONDIRENTRY) * idh.idCount);

	if (pEntries == NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (!ReadFile(hFile, pEntries, idh.idCount * sizeof(ICONDIRENTRY), &dwBytesRead, NULL)
		|| dwBytesRead != idh.idCount * sizeof(ICONDIRENTRY))
	{
		free(pEntries);
		CloseHandle(hFile);
		return FALSE;
	}

	HBITMAP hBitmap = 0;

	for (int i = 0; i < idh.idCount; i++)
	{
		if (pEntries[i].wBitCount == 32 && (((int)pEntries[i].bWidth == nWidth) || (nWidth == 0)))
		{
			if (!SetFilePointer(hFile, pEntries[i].dwImageOffset,
				NULL, FILE_BEGIN))
				continue;

			ICONIMAGE* pIconImage = (ICONIMAGE*)malloc(pEntries[i].dwBytesInRes);
			if (pIconImage == NULL)
				continue;

			dwBytesRead = 0;
			BOOL bResult = ReadFile(hFile, pIconImage, pEntries[i].dwBytesInRes,
				&dwBytesRead, NULL);

			if (!bResult || (dwBytesRead != pEntries[i].dwBytesInRes) || (pIconImage->icHeader.biBitCount != 32))
			{
				free(pIconImage);
				continue;
			}

			CDC dcSrc;
			dcSrc.CreateCompatibleDC(NULL);

			pIconImage->icHeader.biHeight /= 2;
			pIconImage->icHeader.biSizeImage = pIconImage->icHeader.biHeight * pIconImage->icHeader.biWidth * 4;

			BYTE* pDest = NULL;
			HBITMAP hBmp = CreateDIBSection(dcSrc, (BITMAPINFO*)&pIconImage->icHeader, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);

			if (pDest != NULL && hBmp != NULL)
			{
				MEMCPY_S(pDest, &pIconImage->icColors, pIconImage->icHeader.biSizeImage);
				hBitmap = hBmp;

				free(pIconImage);
				break;
			}

			free(pIconImage);
		}
	}
	free(pEntries);
	CloseHandle(hFile);

	return hBitmap;

}

BOOL CXTPImageManager::SetIconFromIcoFile(LPCTSTR pszFileName, UINT nIDCommand, CSize szIcon, XTPImageState imageState)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	CXTPImageManagerIcon::ICONDIRHEADER idh;

	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, &idh, sizeof(CXTPImageManagerIcon::ICONDIRHEADER) , &dwBytesRead, NULL) || dwBytesRead != sizeof(CXTPImageManagerIcon::ICONDIRHEADER))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (idh.idType != 1 || idh.idCount == 0)
	{
		CloseHandle(hFile);
		return FALSE;
	}


	CXTPImageManagerIcon::ICONDIRENTRY* pEntries = (CXTPImageManagerIcon::ICONDIRENTRY*)malloc(sizeof(CXTPImageManagerIcon::ICONDIRENTRY) * idh.idCount);

	if (pEntries == NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (!ReadFile(hFile, pEntries, idh.idCount * sizeof(CXTPImageManagerIcon::ICONDIRENTRY), &dwBytesRead, NULL)
		|| dwBytesRead != idh.idCount * sizeof(CXTPImageManagerIcon::ICONDIRENTRY))
	{
		free(pEntries);
		CloseHandle(hFile);
		return FALSE;
	}

	BOOL bResult = FALSE;

	for (int i = 0; i < idh.idCount; i++)
	{
		CSize szResourceIcon((int)pEntries[i].bWidth, (int)pEntries[i].bHeight);
		CXTPImageManagerIconHandle hIcon;

		if ((szIcon == szResourceIcon) || (szIcon == CSize(0, 0)))
		{
			if (!SetFilePointer(hFile, pEntries[i].dwImageOffset,
				NULL, FILE_BEGIN))
				continue;

			CXTPImageManagerIcon::ICONIMAGE* pIconImage = (CXTPImageManagerIcon::ICONIMAGE*)malloc(pEntries[i].dwBytesInRes);
			if (pIconImage == NULL)
				continue;

			dwBytesRead = 0;
			BOOL bReadFile = ReadFile(hFile, pIconImage, pEntries[i].dwBytesInRes,
				&dwBytesRead, NULL);

			if (!bReadFile || (dwBytesRead != pEntries[i].dwBytesInRes))
			{
				free(pIconImage);
				continue;
			}

			if (pIconImage->icHeader.biBitCount == 32)
			{

				CDC dcSrc;
				dcSrc.CreateCompatibleDC(NULL);

				pIconImage->icHeader.biHeight /= 2;
				pIconImage->icHeader.biSizeImage = pIconImage->icHeader.biHeight * pIconImage->icHeader.biWidth * 4;

				BYTE* pDest = NULL;
				HBITMAP hBitmap = CreateDIBSection(dcSrc, (BITMAPINFO*)&pIconImage->icHeader, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);

				if (pDest != NULL && hBitmap != NULL)
				{
					MEMCPY_S(pDest, &pIconImage->icColors, pIconImage->icHeader.biSizeImage);

					hIcon = hBitmap;
				}
			}
			else
			{
				hIcon = CreateIconFromResourceEx((PBYTE)pIconImage, pEntries[i].dwBytesInRes, TRUE,
					0x00030000, szResourceIcon.cx, szResourceIcon.cy, LR_DEFAULTCOLOR);
			}

			if (!hIcon.IsEmpty())
			{
				SetIcon(hIcon, nIDCommand, szResourceIcon, imageState);
				bResult = TRUE;
			}

			free(pIconImage);
		}
	}
	free(pEntries);
	CloseHandle(hFile);

	return bResult;
}


//////////////////////////////////////////////////////////////////////////
// CXTPImageManagerIconHandle

CXTPImageManagerIconHandle::CXTPImageManagerIconHandle()
{
	m_hIcon = 0;
	m_hBitmap = m_hAlphaBits = 0;
	m_bClearHandles = TRUE;
}
CXTPImageManagerIconHandle::CXTPImageManagerIconHandle(HICON hIcon)
{
	m_hIcon = hIcon;
	m_hBitmap = m_hAlphaBits = 0;
	m_bClearHandles = FALSE;
}
CXTPImageManagerIconHandle::CXTPImageManagerIconHandle(HBITMAP hBitmap)
{
	m_hIcon = 0;
	m_hBitmap = hBitmap;
	m_hAlphaBits = 0;
	m_bClearHandles = FALSE;
}
CXTPImageManagerIconHandle::CXTPImageManagerIconHandle(const CXTPImageManagerIconHandle& hHandle)
{
	m_hIcon = hHandle.m_hIcon;
	m_hBitmap = hHandle.m_hBitmap;

	m_bClearHandles = FALSE;
	m_hAlphaBits = 0;
}

void CXTPImageManagerIconHandle::CopyHandle(HBITMAP hBitmap)
{
	Clear();
	m_hBitmap = CXTPImageManagerIcon::CopyAlphaBitmap(hBitmap);

	m_bClearHandles = TRUE;
}
void CXTPImageManagerIconHandle::CopyHandle(CXTPImageManagerIconHandle& hHandle)
{
	Clear();

	if (hHandle.m_hIcon) m_hIcon = CopyIcon(hHandle.m_hIcon);
	if (hHandle.m_hBitmap) m_hBitmap = CXTPImageManagerIcon::CopyAlphaBitmap(hHandle.m_hBitmap);

	m_bClearHandles = TRUE;
}

const CXTPImageManagerIconHandle& CXTPImageManagerIconHandle::operator=(const HICON hIcon)
{
	Clear();
	m_hIcon = hIcon;
	m_bClearHandles = TRUE;

	return *this;
}
const CXTPImageManagerIconHandle& CXTPImageManagerIconHandle::operator=(const HBITMAP hBitmap)
{
	Clear();
	m_hBitmap = hBitmap;
	m_bClearHandles = TRUE;
	return *this;
}

CXTPImageManagerIconHandle::~CXTPImageManagerIconHandle()
{
	Clear();
}

BOOL CXTPImageManagerIconHandle::IsEmpty() const
{
	return m_hIcon == 0 && m_hBitmap == 0;
}

BOOL CXTPImageManagerIconHandle::IsAlpha() const
{
	return m_hBitmap != 0;
}
HBITMAP CXTPImageManagerIconHandle::GetBitmap() const
{
	ASSERT(IsAlpha());
	return m_hBitmap;
}
HICON CXTPImageManagerIconHandle::GetIcon() const
{
	return m_hIcon;
}

void CXTPImageManagerIconHandle::Clear()
{
	if (m_bClearHandles)
	{
		if (m_hIcon)
		{
			DestroyIcon(m_hIcon);
		}
		if (m_hBitmap)
		{
			DeleteObject(m_hBitmap);
		}
	}

	if (m_hAlphaBits)
	{
		DeleteObject(m_hAlphaBits);
	}

	m_hIcon = 0;
	m_hBitmap = 0;
	m_hAlphaBits = 0;
}

CSize CXTPImageManagerIconHandle::GetExtent() const
{
	if (m_hIcon)
		return CXTPImageManagerIcon::GetExtent(m_hIcon);

	if (m_hBitmap)
	{
		BITMAP bmpinfo;
		if (::GetObject(m_hBitmap, sizeof(bmpinfo), &bmpinfo))
		{
			return CSize((int)bmpinfo.bmWidth, (int)bmpinfo.bmHeight);
		}
	}
	return 0;

}

BOOL CXTPImageManagerIconHandle::CreateIconFromResource(LPCTSTR lpszResourceName, CSize szIcon, BOOL bGroupResource)
{
	if (bGroupResource)
	{
		HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_GROUP_ICON);
		if (hInst)
			return CreateIconFromResource(hInst, lpszResourceName, szIcon, TRUE);
	}

	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_ICON);
	if (hInst)
		return CreateIconFromResource(hInst, lpszResourceName, szIcon, FALSE);

	return FALSE;

}

BOOL CXTPImageManagerIconHandle::CreateIconFromResource(HINSTANCE hInst, LPCTSTR lpszResourceName, CSize szIcon, BOOL bGroupResource)
{
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_GROUP_ICON);

	if (hRsrc != NULL && bGroupResource)
	{
		// Load and Lock to get a pointer to a GRPICONDIR
		HGLOBAL hGlobal = LoadResource(hInst, hRsrc);

		CXTPImageManagerIcon::GRPICONDIR* lpGrpIconDir = (CXTPImageManagerIcon::GRPICONDIR*)LockResource(hGlobal);

		for (int i = (int)lpGrpIconDir->idCount - 1; i >= 0; i--)
		{
			if (lpGrpIconDir->idEntries[i].bWidth == szIcon.cx && lpGrpIconDir->idEntries[i].bHeight == szIcon.cy)
			{
				if (CreateIconFromResource(hInst, MAKEINTRESOURCE(lpGrpIconDir->idEntries[i].nID), szIcon, FALSE))
					return TRUE;
			}
		}

		return FALSE;
	}

	hRsrc = ::FindResource(hInst, lpszResourceName, RT_ICON);

	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	CXTPImageManagerIcon::ICONIMAGE* lpIconImage = (CXTPImageManagerIcon::ICONIMAGE*)LockResource(hGlobal);

	if (lpIconImage == NULL)
		return FALSE;

	DWORD dwResourceSize = SizeofResource(hInst, hRsrc);

	if (lpIconImage->icHeader.biBitCount == 32)
	{
		CDC dcSrc;
		dcSrc.CreateCompatibleDC(0);

		BITMAPINFOHEADER* pBitmapInfo = (BITMAPINFOHEADER*)_alloca(sizeof(BITMAPINFOHEADER) + sizeof(COLORREF) * 3);
		*pBitmapInfo = lpIconImage->icHeader;

		pBitmapInfo->biHeight /= 2;
		pBitmapInfo->biSizeImage = pBitmapInfo->biHeight * pBitmapInfo->biWidth * 4;

		BYTE* pDest = NULL;
		HBITMAP hBitmap = CreateDIBSection(dcSrc, (BITMAPINFO*)pBitmapInfo, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);

		if (pDest == NULL || hBitmap == NULL)
			return FALSE;

		ASSERT(pBitmapInfo->biSizeImage <= dwResourceSize - sizeof(BITMAPINFOHEADER));

		MEMCPY_S(pDest, &lpIconImage->icColors, pBitmapInfo->biSizeImage);

		*this = hBitmap; // NOTE: it call DeleteObejct in destructor

	}
	else
	{
		*this = CreateIconFromResourceEx((PBYTE)lpIconImage, dwResourceSize, TRUE,
			0x00030000, szIcon.cx, szIcon.cy, LR_DEFAULTCOLOR);
	}

	return !IsEmpty();
}


BOOL CXTPImageManagerIconHandle::PreMultiply()
{
	if (m_hAlphaBits != 0)
		return TRUE;

	if (m_hIcon != 0)
	{
		ICONINFO info;
		if (GetIconInfo(m_hIcon, &info))
		{

			m_hAlphaBits = CXTPImageManagerIcon::PreMultiplyAlphaBitmap(info.hbmColor);

			::DeleteObject(info.hbmColor);
			::DeleteObject(info.hbmMask);
		}
	}
	else if (m_hBitmap != 0)
	{
		m_hAlphaBits = CXTPImageManagerIcon::PreMultiplyAlphaBitmap(m_hBitmap);
	}
	return m_hAlphaBits != 0;
}

BOOL CXTPImageManagerIcon::IsAlpha() const
{
	return m_hIcon.IsAlpha();
}

HBITMAP CXTPImageManagerIcon::InvertAlphaBitmap(HBITMAP hBitmap)
{

	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	DWORD* pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;

	HBITMAP hBitmapAlpha = 0;

	TRY
	{
		UINT nSize;
		if (!GetBitmapBits(dcSrc, hBitmap, pBitmapInfo, (LPVOID&)pBits, nSize))
			return 0;

		DWORD* pDest = NULL;

		hBitmapAlpha = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if (pDest == NULL || hBitmapAlpha == NULL)
			AfxThrowMemoryException();

		int cy = pBitmapInfo->bmiHeader.biHeight;
		int cx = pBitmapInfo->bmiHeader.biWidth;

		for (int y = 0; y < cy; y++)
		{
			DWORD* pBitsRow = &pBits[y * cx + cx - 1];

			for (int x = 0; x < cx; x++)
			{
				*pDest++ = *pBitsRow--;
			}
		}
	}
	CATCH (CMemoryException, e)
	{
		TRACE(_T("Failed -- Memory exception thrown."));
	}
	END_CATCH

	FREE(pBits);
	FREE(pBitmapInfo);

	return hBitmapAlpha;
}

HBITMAP CXTPImageManagerIcon::PreMultiplyAlphaBitmap(HBITMAP hBitmap, BOOL* pbAlpha)
{

	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	PBYTE pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;

	HBITMAP hBitmapAlpha = 0;

	TRY
	{
		UINT nSize;
		if (!GetBitmapBits(dcSrc, hBitmap, pBitmapInfo, (LPVOID&)pBits, nSize))
			return 0;

		BYTE* pDest = NULL;

		hBitmapAlpha = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if (pDest == NULL || hBitmapAlpha == NULL)
			AfxThrowMemoryException();

		MEMCPY_S(pDest, pBits, nSize);

		BOOL bAlpha0Found = FALSE;

		for (UINT i = 0; i < nSize; i += 4)
		{
			int nAlpha = pBits[i + 3];
			pDest[i + 0] = BYTE(pBits[i + 0] * nAlpha / 255);
			pDest[i + 1] = BYTE(pBits[i + 1] * nAlpha / 255);
			pDest[i + 2] = BYTE(pBits[i + 2] * nAlpha / 255);

			if (pbAlpha)
			{
				if (nAlpha == 0)
					bAlpha0Found = TRUE;

				if ((nAlpha != 255 && nAlpha != 0) || (nAlpha == 255 && bAlpha0Found))
					*pbAlpha = TRUE;
			}
		}

		if (pbAlpha && (*pbAlpha == FALSE))
		{
			MEMCPY_S(pDest, pBits, nSize);
		}

	}
	CATCH (CMemoryException, e)
	{
		TRACE(_T("Failed -- Memory exception thrown."));
	}
	END_CATCH

	FREE(pBits);
	FREE(pBitmapInfo);

	return hBitmapAlpha;
}

BOOL CXTPImageManagerIcon::GetBitmapBits(CDC& dcSrc, HBITMAP hBitmap, PBITMAPINFO& pBitmapInfo, LPVOID& pBits, UINT& nSize)
{
	if (hBitmap == 0)
		return FALSE;

	BITMAPINFO  bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);


	if (GetDIBits(dcSrc, hBitmap, 0, 0, NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) == 0)
		return FALSE;

	if (bmi.bmiHeader.biBitCount != 32)
		return FALSE;

	nSize = bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 4;
	pBits = (PBYTE)malloc(nSize);
	if (pBits == NULL)
		return FALSE;

	if ((pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + 3 * sizeof(COLORREF))) == NULL)
	{
		FREE(pBits);
		return FALSE;
	}

	MEMCPY_S(pBitmapInfo, &bmi, sizeof(BITMAPINFOHEADER));

	if (GetDIBits(dcSrc, hBitmap, 0, bmi.bmiHeader.biHeight, pBits, pBitmapInfo, DIB_RGB_COLORS) == NULL)
	{
		FREE(pBits);
		FREE(pBitmapInfo);
		return FALSE;
	}
	return TRUE;
}

HBITMAP CXTPImageManagerIcon::CopyAlphaBitmap(HBITMAP hBitmap)
{
	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	PBYTE pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;
	UINT nSize;

	if (!GetBitmapBits(dcSrc, hBitmap, pBitmapInfo, (LPVOID&)pBits, nSize))
		return 0;

	BYTE* pDest = NULL;
	HBITMAP hBmp = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);

	if (pDest != NULL && hBmp != NULL)
	{
		MEMCPY_S(pDest, pBits, nSize);
	}

	FREE(pBits);
	FREE(pBitmapInfo);

	return hBmp;

}




//////////////////////////////////////////////////////////////////////////
// CXTPImageManagerIcon
CXTPImageManagerIcon::CXTPImageManagerIcon(UINT nID, int nWidth, int nHeight, CXTPImageManagerIconSet* pIconSet)
	: m_nID(nID)
	, m_nWidth(nWidth)
	, m_nHeight(nHeight)
	, m_pIconSet(pIconSet)
	, m_pImageList(NULL)
{
	m_bScaled = FALSE;
	m_bDrawComposited = FALSE;

}

CXTPImageManagerIcon::CXTPImageManagerIcon(UINT nID, int nWidth, int nHeight, CXTPImageManagerImageList* pImageList)
	: m_nID(nID)
	, m_nWidth(nWidth)
	, m_nHeight(nHeight)
	, m_pIconSet(NULL)
	, m_pImageList(pImageList)
{
	m_bScaled = FALSE;

}


CXTPImageManagerIcon::~CXTPImageManagerIcon()
{
	Clear(TRUE);
}

UINT CXTPImageManagerIcon::GetID() const
{
	return m_nID;
}

int CXTPImageManagerIcon::GetHeight() const
{
	return m_nHeight;
}

int CXTPImageManagerIcon::GetWidth() const
{
	return m_nWidth;
}

BOOL CXTPImageManagerIcon::IsScaled() const
{
	return m_bScaled;
}

void CXTPImageManagerIcon::Clear(BOOL bIcon /* = FALSE */)
{
	if (bIcon)
	{
		m_hIcon.Clear();
	}
	m_hFaded.Clear();
	m_hShadow.Clear();
	m_hHot.Clear();
	m_hChecked.Clear();
	m_hPressed.Clear();
	m_hDisabled.Clear();
	m_hDisabledAuto.Clear();
}

void CXTPImageManagerIcon::Refresh()
{
	m_hFaded.Clear();
	m_hShadow.Clear();
	m_hDisabledAuto.Clear();
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetIcon()
{
	return m_hIcon;
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetIcon(XTPImageState imageState)
{
	return imageState == xtpImageDisabled ? GetDisabledIcon() : imageState == xtpImageHot? GetHotIcon() :
		imageState == xtpImageChecked? GetCheckedIcon() : imageState == xtpImagePressed? GetPressedIcon() : GetIcon();
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetFadedIcon()
{
	if (m_hFaded.IsEmpty())
	{
		CreateFadedIcon();
	}
	return m_hFaded;
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetShadowIcon()
{
	if (m_hShadow.IsEmpty())
	{
		CreateShadowIcon();
	}
	return m_hShadow;
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetHotIcon()
{
	return !m_hHot.IsEmpty() ? m_hHot : m_hIcon;
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetCheckedIcon()
{
	return !m_hChecked.IsEmpty() ? m_hChecked : GetHotIcon();
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetPressedIcon()
{
	return !m_hPressed.IsEmpty() ? m_hPressed : GetHotIcon();
}

BOOL CXTPImageManagerIcon::HasDisabledIcon() const
{
	if (!m_hDisabled.IsEmpty())
		return TRUE;

	if (!m_hDisabledAuto.IsEmpty())
		return TRUE;

	return FALSE;
}

CXTPImageManagerIconHandle& CXTPImageManagerIcon::GetDisabledIcon(BOOL bCreateIfNotExists /*= TRUE*/)
{
	if (!m_hDisabled.IsEmpty())
		return m_hDisabled;

	if (!m_hDisabledAuto.IsEmpty())
		return m_hDisabledAuto;

	if (bCreateIfNotExists)
	{
		CreateDisabledIcon();
	}

	return m_hDisabledAuto;
}

BOOL CXTPImageManagerIcon::SetIcon(CXTPImageManagerIconHandle iconHandle, XTPImageState imageState)
{
	switch(imageState)
	{
		case xtpImageNormal:
			SetNormalIcon(iconHandle);
			break;
		case xtpImageHot:
			SetHotIcon(iconHandle);
			break;
		case xtpImageChecked:
			SetCheckedIcon(iconHandle);
			break;
		case xtpImageDisabled:
			SetDisabledIcon(iconHandle);
			break;
		case xtpImagePressed:
			SetPressedIcon(iconHandle);
			break;
		default:
			return FALSE;
	}

	return TRUE;
}


BOOL CXTPImageManagerIcon::SetIcon(CXTPImageManagerIconHandle hIcon)
{
	ASSERT(!hIcon.IsEmpty());
	Clear(TRUE);
	m_hIcon.CopyHandle(hIcon);
	CSize sz = GetExtent();

	m_nHeight = sz.cy;
	m_nWidth = sz.cx;

	m_bScaled = FALSE;

	if (m_pIconSet)
		m_pIconSet->RefreshAll();

	return TRUE;
}

BOOL CXTPImageManagerIcon::SetIcon(UINT nIDResourceIcon, int nWidth, int nHeight)
{
	Clear(TRUE);
	m_bScaled = FALSE;

	ASSERT(nWidth != 0 && nHeight != 0);

	if (!m_hIcon.CreateIconFromResource(MAKEINTRESOURCE(nIDResourceIcon), CSize(nWidth, nHeight)))
		return FALSE;

	m_nHeight = nHeight;
	m_nWidth = nWidth;

	return TRUE;
}

void CXTPImageManagerIcon::SetNormalIcon(CXTPImageManagerIconHandle hIcon)
{
	ASSERT(!hIcon.IsEmpty());

	Refresh();
	m_hIcon.CopyHandle(hIcon);


#ifdef _DEBUG
	CSize sz = GetExtent();

	ASSERT((int)m_nHeight == sz.cy);
	ASSERT((int)m_nWidth == sz.cx);

#endif
}

void CXTPImageManagerIcon::SetHotIcon(CXTPImageManagerIconHandle hIcon)
{
	Refresh();
	m_hHot.CopyHandle(hIcon);
}

void CXTPImageManagerIcon::SetCheckedIcon(CXTPImageManagerIconHandle hIcon)
{
	Refresh();
	m_hChecked.CopyHandle(hIcon);
}

void CXTPImageManagerIcon::SetPressedIcon(CXTPImageManagerIconHandle hIcon)
{
	Refresh();
	m_hPressed.CopyHandle(hIcon);
}

void CXTPImageManagerIcon::SetDisabledIcon(CXTPImageManagerIconHandle hIcon)
{
	Refresh();
	m_hDisabled.CopyHandle(hIcon);
}


COLORREF CXTPImageManagerIcon::LightenColor(COLORREF clr, double factor)
{
	return RGB(
		factor * 255 + (1.0 - factor) * GetRValue(clr),
		factor * 255 + (1.0 - factor) * GetGValue(clr),
		factor * 255 + (1.0 - factor) * GetBValue(clr)) ;
}

void CXTPImageManagerIcon::CreateFadedIcon()
{
	ASSERT(m_hFaded.IsEmpty());

	ICONINFO info;

	if (!m_hIcon.IsAlpha())
	{
		if (GetIconInfo(m_hIcon.GetIcon(), &info))
		{
			if (!CXTPDrawHelpers::IsLowResolution())
			{
				CXTPCompatibleDC dc(NULL, CBitmap::FromHandle(info.hbmColor));
				CXTPCompatibleDC dcMask(NULL, CBitmap::FromHandle(info.hbmMask));

				BITMAP bmp;
				::GetObject(info.hbmColor, sizeof(BITMAP), &bmp);

				for (int i = 0; i < bmp.bmWidth; i++)
				for (int j = 0; j < bmp.bmHeight; j++)
				{
					COLORREF clr = dc.GetPixel(i, j);
					COLORREF clrMask = dcMask.GetPixel(i, j);
					if (clrMask == 0)
						dc.SetPixel(i, j, LightenColor(clr, .3));
				}
			}

			m_hFaded = CreateIconIndirect(&info);

			::DeleteObject(info.hbmColor);
			::DeleteObject(info.hbmMask);
		}
	}
	else
	{
		CDC dcSrc;
		dcSrc.CreateCompatibleDC(NULL);

		PBYTE pBits = 0;
		PBITMAPINFO pBitmapInfo = 0;

		m_hFaded.CopyHandle(m_hIcon);

		TRY
		{
			UINT nSize;
			if (!GetBitmapBits(dcSrc, m_hFaded.GetBitmap(), pBitmapInfo, (LPVOID&)pBits, nSize))
				AfxThrowMemoryException();

			for (UINT i = 0; i < nSize; i += 4)
			{
				pBits[i + 0] = BYTE(.3 * 255 + (1.0 - .3) * pBits[i + 0]);
				pBits[i + 1] = BYTE(.3 * 255 + (1.0 - .3) * pBits[i + 1]);
				pBits[i + 2] = BYTE(.3 * 255 + (1.0 - .3) * pBits[i + 2]);
			}

			SetDIBits(dcSrc, m_hFaded.GetBitmap(), 0, pBitmapInfo->bmiHeader.biHeight, pBits, pBitmapInfo, DIB_RGB_COLORS);

		}
		CATCH (CMemoryException, e)
		{
			TRACE(_T("Failed -- Memory exception thrown."));
		}
		END_CATCH

		FREE(pBits);
		FREE(pBitmapInfo);
	}
}

void CXTPImageManagerIcon::CreateShadowIcon()
{
	ASSERT(m_hShadow.IsEmpty());

	COLORREF clrBackground = GetXtremeColor(XPCOLOR_HIGHLIGHT);
	COLORREF clrShadow = RGB(GetRValue(clrBackground) * .75, GetGValue(clrBackground) * .75, GetBValue(clrBackground) * .75);

	if (!GetHotIcon().IsAlpha())
	{
		ICONINFO info;
		if (GetIconInfo(GetHotIcon().GetIcon(), &info))
		{
			{

				CXTPCompatibleDC dc(NULL, CBitmap::FromHandle(info.hbmColor));
				CXTPCompatibleDC dcMask(NULL, CBitmap::FromHandle(info.hbmMask));

				BITMAP bmp;
				::GetObject(info.hbmColor, sizeof(BITMAP), &bmp);

				for (int i = 0; i < bmp.bmWidth; i++)
				for (int j = 0; j < bmp.bmHeight; j++)
				{
					COLORREF clrMask = dcMask.GetPixel(i, j);
					if (clrMask == 0)
						dc.SetPixel(i, j, clrShadow);
				}
			}

			m_hShadow = CreateIconIndirect(&info);

			::DeleteObject(info.hbmMask);
			::DeleteObject(info.hbmColor);
		}
	}
	else
	{
		CDC dcSrc;
		dcSrc.CreateCompatibleDC(NULL);

		PBYTE pBits = 0;
		PBITMAPINFO pBitmapInfo = 0;

		m_hShadow.CopyHandle(GetHotIcon());

		TRY
		{
			UINT nSize;
			if (!GetBitmapBits(dcSrc, m_hShadow.GetBitmap(), pBitmapInfo, (LPVOID&)pBits, nSize))
				AfxThrowMemoryException();

			for (UINT i = 0; i < nSize; i += 4)
			{
				pBits[i + 0] = GetRValue(clrShadow);
				pBits[i + 1] = GetGValue(clrShadow);
				pBits[i + 2] = GetBValue(clrShadow);
			}

			SetDIBits(dcSrc, m_hShadow.GetBitmap(), 0, pBitmapInfo->bmiHeader.biHeight, pBits, pBitmapInfo, DIB_RGB_COLORS);
		}
		CATCH (CMemoryException, e)
		{
			TRACE(_T("Failed -- Memory exception thrown."));
		}
		END_CATCH

		FREE(pBits);
		FREE(pBitmapInfo);
	}
}

void CXTPImageManagerIcon::CreateDisabledIcon(COLORREF clrDisabledLight /*= (COLORREF)-1*/, COLORREF clrDisabledDark /*= (COLORREF)-1*/)
{
	if (!m_hDisabledAuto.IsEmpty())
		return;

	BOOL bOfficeDisabledIcons = clrDisabledLight != (COLORREF)-1 && clrDisabledDark != (COLORREF)-1;

	ICONINFO info;

	if (!m_hIcon.IsAlpha())
	{
		if (GetIconInfo(m_hIcon.GetIcon(), &info))
		{
			{
				CXTPCompatibleDC dc(NULL, CBitmap::FromHandle(info.hbmColor));
				CXTPCompatibleDC dcMask(NULL, CBitmap::FromHandle(info.hbmMask));

				BITMAP bmp;
				::GetObject(info.hbmColor, sizeof(BITMAP), &bmp);

				for (int i = 0; i < bmp.bmWidth; i++)
				for (int j = 0; j < bmp.bmHeight; j++)
				{
					COLORREF clrMask = dcMask.GetPixel(i, j);
					COLORREF clr = dc.GetPixel(i, j);

					if (clrMask == 0)
					{
						if (bOfficeDisabledIcons)
						{
							double dGray = (GetRValue(clr) * 0.299 + GetGValue(clr) * 0.587 + GetBValue(clr) * 0.114)/255;
							double dLight = 1.0 - dGray;

							clr = RGB(dLight * GetRValue(clrDisabledDark) + dGray * GetRValue(clrDisabledLight),
								dLight * GetGValue(clrDisabledDark) + dGray * GetGValue(clrDisabledLight),
								dLight * GetBValue(clrDisabledDark) + dGray * GetBValue(clrDisabledLight));

							dc.SetPixel(i, j, clr);
						}
						else
						{
							double dGray = GetRValue(clr) * 0.299 + GetGValue(clr) * 0.587 + GetBValue(clr) * 0.114;
							int nGray = (BYTE)(pow(dGray / 255.0, CXTPImageManager::m_dDisabledBrightnessFactor) * 255.0);

							dc.SetPixel(i, j, RGB(nGray, nGray, nGray));
						}
					}
				}
			}

			m_hDisabledAuto = CreateIconIndirect(&info);
			::DeleteObject(info.hbmMask);
			::DeleteObject(info.hbmColor);

		}
	}
	else
	{
		CDC dcSrc;
		dcSrc.CreateCompatibleDC(NULL);

		PBYTE pBits = 0;
		PBITMAPINFO pBitmapInfo = 0;

		m_hDisabledAuto.CopyHandle(m_hIcon);

		TRY
		{
			UINT nSize;
			if (!GetBitmapBits(dcSrc, m_hDisabledAuto.GetBitmap(), pBitmapInfo, (LPVOID&)pBits, nSize))
				AfxThrowMemoryException();

			for (UINT i = 0; i < nSize; i += 4)
			{
				if (bOfficeDisabledIcons)
				{
					double dGray = (pBits[i + 0] * 0.114 + pBits[i + 1] * 0.587 + pBits[i + 2] * 0.299)/255.0;
					double dLight = 1.0 - dGray;

					pBits[i + 0] = BYTE(dLight * GetBValue(clrDisabledDark) + dGray * GetBValue(clrDisabledLight));
					pBits[i + 1] = BYTE(dLight * GetGValue(clrDisabledDark) + dGray * GetGValue(clrDisabledLight));
					pBits[i + 2] = BYTE(dLight * GetRValue(clrDisabledDark) + dGray * GetRValue(clrDisabledLight));
				}
				else
				{
					double dGray = pBits[i + 0] * 0.114 + pBits[i + 1] * 0.587 + pBits[i + 2] * 0.299;
					pBits[i + 0] = pBits[i + 1] = pBits[i + 2] = (BYTE)(pow(dGray / 255.0, CXTPImageManager::m_dDisabledBrightnessFactor) * 255.0);
					pBits[i + 3] = BYTE(pBits[i + 3] / CXTPImageManager::m_dDisabledAlphaFactor);
				}
			}

			SetDIBits(dcSrc, m_hDisabledAuto.GetBitmap(), 0, pBitmapInfo->bmiHeader.biHeight, pBits, pBitmapInfo, DIB_RGB_COLORS);

		}
		CATCH (CMemoryException, e)
		{
			TRACE(_T("Failed -- Memory exception thrown."));
		}
		END_CATCH

		FREE(pBits);
		FREE(pBitmapInfo);
	}
}

CSize CXTPImageManagerIcon::GetExtent() const
{
	return m_hIcon.GetExtent();
}

CSize CXTPImageManagerIcon::GetExtent(HICON hIcon)
{
	ASSERT(hIcon);

	CSize extent(0);
	if (hIcon)
	{
		ICONINFO iconinfo;
		if (::GetIconInfo(hIcon, &iconinfo))
		{
			BITMAP bmpinfo;
			if (::GetObject(iconinfo.hbmMask, sizeof(bmpinfo), &bmpinfo))
			{
				extent.cx = (int)bmpinfo.bmWidth;
				extent.cy = (int)bmpinfo.bmHeight;
				if (!iconinfo.hbmColor)
				{
					// b/w icons have double size for XOR and AND masks
					extent.cy /= 2;
				}
			}
			// cleanup GDI
			::DeleteObject(iconinfo.hbmMask);
			::DeleteObject(iconinfo.hbmColor);
		}
	}
	return extent;
}


HICON CXTPImageManagerIcon::ScaleToFit(HICON hIcon, CSize szExtent, int nWidth)
{
	if (nWidth == 0 || szExtent == CSize(0))
	{
		// invalid arg
		return NULL;
	}
	if (nWidth == szExtent.cx)
		return ::CopyIcon(hIcon);

	CSize szDesiredExtent(nWidth, MulDiv(szExtent.cy, nWidth, szExtent.cx));

	// scale the icon
	CImageList images;
	VERIFY(images.Create(szDesiredExtent.cx, szDesiredExtent.cy, ILC_COLOR32 | ILC_MASK, 1, 1));
	images.Add(hIcon);
	return images.ExtractIcon(0);
}

CXTPImageManagerIcon* CXTPImageManagerIcon::Scale(int nWidth) const
{
	ASSERT(m_hIcon.GetIcon());

	if (!m_hIcon.GetIcon())
		return NULL;

	CSize szExtent = GetExtent();

	CXTPImageManagerIcon* pIcon = new CXTPImageManagerIcon(m_nID, nWidth, MulDiv(szExtent.cy, nWidth, szExtent.cx), m_pIconSet);

	if (m_hIcon.GetIcon())
	{
		HICON hIcon = ScaleToFit(m_hIcon.GetIcon(), szExtent, nWidth);
		pIcon->SetIcon(hIcon);
		::DestroyIcon(hIcon);
	}
	if (m_hDisabled.GetIcon())
	{
		HICON hIcon = ScaleToFit(m_hDisabled.GetIcon(), szExtent, nWidth);
		pIcon->SetDisabledIcon(hIcon);
		::DestroyIcon(hIcon);
	}
	if (m_hHot.GetIcon())
	{
		HICON hIcon = ScaleToFit(m_hHot.GetIcon(), szExtent, nWidth);
		pIcon->SetHotIcon(hIcon);
		::DestroyIcon(hIcon);
	}
	if (m_hChecked.GetIcon())
	{
		HICON hIcon = ScaleToFit(m_hChecked.GetIcon(), szExtent, nWidth);
		pIcon->SetCheckedIcon(hIcon);
		::DestroyIcon(hIcon);
	}
	if (m_hPressed.GetIcon())
	{
		HICON hIcon = ScaleToFit(m_hPressed.GetIcon(), szExtent, nWidth);
		pIcon->SetPressedIcon(hIcon);
		::DestroyIcon(hIcon);
	}

	return pIcon;
}


BOOL CXTPImageManagerIcon::GetDIBBitmap(HBITMAP hBitmap, PBYTE& pBits, UINT& nBitsSize, PBITMAPINFO& pBitmapInfo, UINT& nBitmapInfoSize)
{
	pBits = 0;
	pBitmapInfo = 0;

	BITMAPINFO  bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	TRY
	{
		if (GetDIBits(dcSrc, hBitmap, 0, 0, NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) == 0)
			AfxThrowMemoryException();

		nBitsSize = bmi.bmiHeader.biSizeImage != 0? bmi.bmiHeader.biSizeImage :
			bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 4;

		pBits = (PBYTE)malloc(nBitsSize);
		if (pBits == NULL)
			AfxThrowMemoryException();

		int nColorTableSize = bmi.bmiHeader.biBitCount == 4 ? 16 : bmi.bmiHeader.biBitCount == 8 ? 256 : 3;

		nBitmapInfoSize = sizeof(BITMAPINFOHEADER) + nColorTableSize * sizeof(COLORREF);
		if ((pBitmapInfo = (PBITMAPINFO)malloc(nBitmapInfoSize)) == NULL)
			AfxThrowMemoryException();

		MEMCPY_S(pBitmapInfo, &bmi, sizeof(BITMAPINFOHEADER));

		if (GetDIBits(dcSrc, hBitmap, 0, bmi.bmiHeader.biHeight, pBits, pBitmapInfo, DIB_RGB_COLORS) == NULL)
			AfxThrowMemoryException();
	}
	CATCH (CMemoryException, e)
	{
		TRACE(_T("Failed -- Memory exception thrown."));
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

HBITMAP CXTPImageManagerIcon::ReadDIBBitmap(CArchive& ar)
{

	PBITMAPINFO pBitmapInfo = 0;
	PBYTE pBits = 0;

	DWORD dwCount = (DWORD) ar.ReadCount();
	DWORD dwBitsCount = 0;

	if (dwCount > 0)
	{
		pBitmapInfo = (PBITMAPINFO)malloc(dwCount);
		ar.Read(pBitmapInfo, dwCount);

		dwBitsCount = (DWORD) ar.ReadCount();
		pBits = (PBYTE)malloc(dwBitsCount);
		ar.Read(pBits, dwBitsCount);
	}
	else
	{
		return NULL;
	}

	if (!pBitmapInfo || dwBitsCount == 0)
	{
		FREE(pBits);
		FREE(pBitmapInfo);
		return NULL;
	}

	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	if (pBitmapInfo->bmiHeader.biSizeImage == 0)
	{
		pBitmapInfo->bmiHeader.biSizeImage = dwBitsCount;
	}

	BYTE* pDest = NULL;
	HBITMAP hBmp = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);

	if (!pDest || !hBmp)
	{
		FREE(pBits);
		FREE(pBitmapInfo);
		return NULL;
	}

	MEMCPY_S(pDest, pBits, min(dwBitsCount, pBitmapInfo->bmiHeader.biSizeImage));

	FREE(pBits);
	FREE(pBitmapInfo);

	return hBmp;
}

void CXTPImageManagerIcon::WriteDIBBitmap(CArchive& ar, HBITMAP hBitmap)
{
	PBYTE pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;
	UINT nBitsSize;
	UINT nBitmapInfoSize;

	if (GetDIBBitmap(hBitmap, pBits, nBitsSize, pBitmapInfo, nBitmapInfoSize))
	{
		ar.WriteCount(nBitmapInfoSize);
		ar.Write(pBitmapInfo, nBitmapInfoSize);
		ar.WriteCount(nBitsSize);
		ar.Write(pBits, nBitsSize);
	}
	else
	{
		ar.WriteCount(0);
	}
	FREE(pBits);
	FREE(pBitmapInfo);
}

void CXTPImageManagerIcon::SerializeIcon(CXTPImageManagerIconHandle& hIcon, CArchive& ar)
{
	if (ar.IsStoring())
	{
		ICONINFO info;

		if (!hIcon.IsEmpty())
		{
			if (hIcon.IsAlpha())
			{
				WriteDIBBitmap(ar, hIcon.GetBitmap());
				ar.WriteCount(0);
			}
			else if (GetIconInfo(hIcon.GetIcon(), &info))
			{
				WriteDIBBitmap(ar, info.hbmColor);
				WriteDIBBitmap(ar, info.hbmMask);

				::DeleteObject(info.hbmColor);
				::DeleteObject(info.hbmMask);

			}
			else
			{
				ar.WriteCount(0);
				ar.WriteCount(0);
			}
		}
		else
		{
			ar.WriteCount(0);
			ar.WriteCount(0);
		}
	}
	else
	{
		hIcon.Clear();

		HBITMAP hbmColor = ReadDIBBitmap(ar);
		HBITMAP hbmMask = ReadDIBBitmap(ar);

		if (hbmColor)
		{
			if (hbmMask == 0)
			{
				hIcon = hbmColor;
				hbmColor = 0;
			}
			else
			{
				BITMAP bmp;
				::GetObject(hbmColor, sizeof(BITMAP), &bmp);

				CImageList il;
				il.Create(bmp.bmWidth, bmp.bmHeight, ILC_COLOR24 | ILC_MASK, 0, 1);
				il.Add(CBitmap::FromHandle(hbmColor), hbmMask == 0 ? (CBitmap*)NULL : CBitmap::FromHandle(hbmMask));
				hIcon = il.ExtractIcon(0);
			}

			if (hbmColor) ::DeleteObject(hbmColor);
			if (hbmMask) ::DeleteObject(hbmMask);
		}
	}

}

void CXTPImageManagerIcon::Serialize(CArchive& ar)
{
	BOOL nShema = _XTP_SCHEMA_CURRENT;

	if (ar.IsStoring())
	{
		ar << m_nHeight;
		ar << nShema;

	}
	else
	{
		ar >> m_nHeight;
		ar >> nShema;
	}
	SerializeIcon(m_hIcon, ar);
	SerializeIcon(m_hDisabled, ar);
	SerializeIcon(m_hHot, ar);

	if (nShema > _XTP_SCHEMA_875)
	{
		SerializeIcon(m_hChecked, ar);
	}
	if (nShema > _XTP_SCHEMA_98)
	{
		SerializeIcon(m_hPressed, ar);
	}
}

void CXTPImageManagerIcon::DrawAlphaBitmap(CDC* pDC, HBITMAP hBitmap, CPoint ptDest, CSize szDest, CPoint ptSrc, CSize szSrc)
{
	if (szSrc == CSize(0))
		szSrc = szDest;

	CXTPCompatibleDC dc(NULL, CBitmap::FromHandle(hBitmap));

	XTPImageManager()->AlphaBlend(pDC->GetSafeHdc(), CRect(ptDest, szDest),
		dc, CRect(ptSrc, szSrc));

}
CXTPImageManager* CXTPImageManagerIcon::GetImageManager() const
{
	if (m_pIconSet)
	{
		ASSERT(m_pIconSet->m_pManager);
		return m_pIconSet->m_pManager;
	}

	return XTPImageManager();
}

void CXTPImageManagerIcon::DrawMono(CDC* pDC, CPoint pt, CXTPImageManagerIconHandle& hIcon, CSize szIcon, COLORREF clrBrush)
{
	if (hIcon.IsAlpha())
		return;

	if (szIcon.cx == 0)
		szIcon.cx = m_nWidth;

	if (szIcon.cy == 0)
		szIcon.cy = m_nHeight * szIcon.cx / m_nWidth;

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, szIcon.cx, szIcon.cy);
	{
		CXTPCompatibleDC dc(pDC, &bmp);
		dc.FillSolidRect(0, 0, szIcon.cx, szIcon.cy, 0xFFFFFF);

		DrawIconEx(dc.GetSafeHdc(), 0, 0, hIcon, szIcon.cx, szIcon.cy, 0, 0, DI_NORMAL);
	}
	pDC->DrawState(pt, szIcon, bmp, (UINT)DSS_MONO, CBrush(clrBrush));
}

void CXTPImageManagerIcon::Draw(CDC* pDC, CPoint pt, CSize szIcon)
{
	Draw(pDC, pt, GetIcon(), szIcon);
}

void CXTPImageManagerIcon::Draw(CDC* pDC, CPoint pt)
{
	Draw(pDC, pt, GetIcon());
}

void CXTPImageManagerIcon::DrawIconComposited(CDC* pDC, CPoint pt, CSize szIcon, HICON hIcon)
{
	CRect rcDest;
	if (szIcon.cx < 0)
	{
		rcDest.SetRect(pt.x + szIcon.cx, pt.y, pt.x, pt.y + szIcon.cy);
	}
	else
	{
		rcDest.SetRect(pt.x, pt.y, pt.x + szIcon.cx, pt.y + szIcon.cy);
	}

	CSize sz(rcDest.Width(), rcDest.Height());
	CBitmap bmp;
	LPDWORD lpBits;
	bmp.Attach(CXTPImageManager::Create32BPPDIBSection(pDC->GetSafeHdc(), sz.cx, sz.cy, (LPBYTE*)&lpBits));

	CXTPCompatibleDC dc(pDC, &bmp);

	CRect rcDestOrig(0, 0, sz.cx, sz.cy);

	dc.FillSolidRect(rcDestOrig, 0xFF00FF);
	DrawIconEx(dc, szIcon.cx > 0 ? 0 : -szIcon.cx, 0, hIcon, szIcon.cx, szIcon.cy, 0, 0, DI_NORMAL);

	for (int i = 0; i < sz.cx * sz.cy; i++)
	{
		if (lpBits[0] == 0xFF00FF)
		{
			lpBits[0] = 0;
		}
		else
		{
			lpBits[0] |= 0xFF000000;
		}
		lpBits++;
	}

	XTPImageManager()->AlphaBlend(*pDC, rcDest, dc, rcDestOrig);
}

void CXTPImageManagerIcon::Draw(CDC* pDC, CPoint pt, CXTPImageManagerIconHandle& hIcon, CSize szIcon /*= 0*/)
{
	if (szIcon.cx == 0)
		szIcon.cx = m_nWidth;

	if (szIcon.cy == 0)
		szIcon.cy = m_nHeight * szIcon.cx / m_nWidth;

	if (m_pImageList)
	{
		m_pImageList->Draw(pDC, pt, this, szIcon);
		return;
	}

	if (hIcon.IsEmpty())
	{
		return;
	}

	if (!hIcon.IsAlpha())
	{
		BOOL bDrawReverted = FALSE;
		if (GetImageManager()->m_bDrawReverted == 1)
			bDrawReverted = TRUE;
		else if (GetImageManager()->m_bDrawReverted == 2)
			bDrawReverted = CXTPDrawHelpers::IsContextRTL(pDC);

		if (bDrawReverted)
		{
			pt.x += szIcon.cx;
			szIcon.cx = -szIcon.cx;
		}

		if (!m_bDrawComposited)
		{
			DrawIconEx(pDC->GetSafeHdc(), pt.x, pt.y, hIcon.GetIcon(), szIcon.cx, szIcon.cy, 0, 0, DI_NORMAL);
		}
		else
		{
			DrawIconComposited(pDC, pt, szIcon, hIcon.GetIcon());

		}
	}
	else if (hIcon.PreMultiply())
	{
		if (pt.y < 0 && (szIcon.cy == (int)m_nHeight))
		{
			if (pt.y + m_nHeight > 0)
			{
				DrawAlphaBitmap(pDC, hIcon.m_hAlphaBits, CPoint(pt.x, 0),
					CSize(szIcon.cx, m_nHeight + pt.y), CPoint(0, -pt.y), CSize(m_nWidth, m_nHeight + pt.y));
			}
		}
		else
		{
			DrawAlphaBitmap(pDC, hIcon.m_hAlphaBits, pt, szIcon, CPoint(0, 0), CSize(m_nWidth, m_nHeight));
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// CXTPImageManagerIconSet

CXTPImageManagerIconSet::CXTPImageManagerIconSet(UINT nID, CXTPImageManager* pManager)
	: m_nID(nID)
{
	ASSERT(pManager);
	m_pManager = pManager;

	EnableAutomation();
}

CXTPImageManagerIconSet::~CXTPImageManagerIconSet()
{
	RemoveAll();
}

void CXTPImageManagerIconSet::RemoveAll()
{
	UINT nWidth;
	CXTPImageManagerIcon* pIcon;

	POSITION pos = m_mapIcons.GetStartPosition();
	while (pos)
	{
		m_mapIcons.GetNextAssoc(pos, nWidth, pIcon);
		pIcon->InternalRelease();
	}
	m_mapIcons.RemoveAll();
}

CXTPImageManagerIcon* CXTPImageManagerIconSet::CreateIcon(UINT nWidth)
{
	CXTPImageManagerIcon* pIcon = 0;
	if (m_mapIcons.Lookup(nWidth, pIcon))
	{
		ASSERT(pIcon->m_nID == m_nID);
		ASSERT(pIcon->m_nWidth == nWidth);
	}
	else
	{
		pIcon = new CXTPImageManagerIcon(m_nID, nWidth, 15, this);
		m_mapIcons.SetAt(nWidth, pIcon);
	}

	ASSERT(pIcon);

	return pIcon;
}

void CXTPImageManagerIconSet::SetIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth)
{
	CXTPImageManagerIcon* pIcon = CreateIcon(nWidth);
	pIcon->SetIcon(hIcon);
}

void CXTPImageManagerIconSet::SetDisabledIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth)
{
	CXTPImageManagerIcon* pIcon = CreateIcon(nWidth);
	pIcon->SetDisabledIcon(hIcon);
}

void CXTPImageManagerIconSet::SetHotIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth)
{
	CXTPImageManagerIcon* pIcon = CreateIcon(nWidth);
	pIcon->SetHotIcon(hIcon);
}

void CXTPImageManagerIconSet::SetCheckedIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth)
{
	CXTPImageManagerIcon* pIcon = CreateIcon(nWidth);
	pIcon->SetCheckedIcon(hIcon);
}

void CXTPImageManagerIconSet::SetPressedIcon(CXTPImageManagerIconHandle hIcon, UINT nWidth)
{
	CXTPImageManagerIcon* pIcon = CreateIcon(nWidth);
	pIcon->SetPressedIcon(hIcon);
}

CXTPImageManagerIcon* CXTPImageManagerIconSet::GetIcon(UINT nWidth, BOOL bScaled)
{
	ASSERT(m_mapIcons.GetCount() > 0);

	CXTPImageManagerIcon* pIcon = NULL;
	BOOL bSpecial = (nWidth == ICON_BIG) || (nWidth == ICON_SMALL);

	if (!bSpecial)
	{
		if (m_mapIcons.Lookup(nWidth, pIcon))
		{
			return pIcon;
		}
		if (!bScaled)
			return NULL;
	}

	UINT nDelta = 0;

	POSITION pos = m_mapIcons.GetStartPosition();
	while (pos)
	{
		UINT nWidthScale;
		CXTPImageManagerIcon* pIconScale;

		m_mapIcons.GetNextAssoc(pos, nWidthScale, pIconScale);

		if (bSpecial)
		{
			if (!pIconScale->IsScaled() && ((nDelta == 0) || ((nWidth == ICON_SMALL && nWidthScale < nDelta)
				|| (nWidth == ICON_BIG && nWidthScale > nDelta))))
			{
				pIcon = pIconScale;
				nDelta = nWidthScale;
			}
		}
		else if (nDelta == 0 || (UINT)abs(int(nWidthScale - nWidth)) < nDelta)
		{
			pIcon = pIconScale;
			nDelta = (UINT)abs(int(nWidthScale - nWidth));
		}
	}

	ASSERT(pIcon != 0);
	if (!pIcon)
		return NULL;

	return pIcon;
}


void CXTPImageManagerIconSet::RefreshAll()
{
	POSITION pos = m_mapIcons.GetStartPosition();
	UINT nWidth;
	CXTPImageManagerIcon* pIcon;

	while (pos)
	{
		m_mapIcons.GetNextAssoc(pos, nWidth, pIcon);

		if (pIcon->m_bScaled)
		{
			m_mapIcons.RemoveKey(nWidth);
			pIcon->InternalRelease();

		}
		else
		{
			pIcon->Refresh();
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// CXTPImageManager

CXTPImageManager::CXTPImageManager()
{
	m_clrMask = (COLORREF)-1;

	m_nCustomID = 0x10AAA;

	m_mapImages.InitHashTable(XTP_IMAGEMAN_HASH_TABLE_SIZE, FALSE);

	m_hMsImgDll = ::LoadLibrary(_T("msimg32.dll"));
	m_pfnAlphaBlend = 0;
	m_pfnTransparentBlt = 0;

	if (m_hMsImgDll)
	{
		m_pfnAlphaBlend = (PFNALPHABLEND)::GetProcAddress(m_hMsImgDll, "AlphaBlend");
		m_pfnTransparentBlt = (PFNTRANSPARENTBLT)::GetProcAddress(m_hMsImgDll, "TransparentBlt");
	}
	EnableAutomation();

	m_bDrawReverted = 2;


}

CXTPImageManager* AFX_CDECL XTPImageManager()
{
	static CXTPImageManager s_managerInstance;
	return &s_managerInstance;
}

CXTPImageManager::~CXTPImageManager()
{
	RemoveAll();

	if (m_hMsImgDll)
	{
		::FreeLibrary(m_hMsImgDll);
	}
}

BOOL CXTPImageManager::LoadToolbar(UINT nIDResourceToolBar, UINT*& pItems, int& nCount, CSize& szIcon) const
{
	LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResourceToolBar);

	struct CToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;

		WORD* items()
		{
			return (WORD*)(this + 1);
		}
	};

	ASSERT(lpszResourceName != NULL);


	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;

	ASSERT(pData->wVersion == 1);

	pItems = new UINT[pData->wItemCount];

	nCount = 0;
	for (int i = 0; i < pData->wItemCount; i++)
		if (pData->items()[i]) pItems[nCount++] = pData->items()[i];

	szIcon = CSize(pData->wWidth, pData->wHeight);

	UnlockResource(hGlobal);
	FreeResource(hGlobal);
	return TRUE;
}

BOOL CXTPImageManager::SetIcons(UINT nIDResourceToolBar, CBitmap& bmpIcons, XTPImageState imageState)
{
	UINT* pItems = NULL;
	CSize szIcon(0);
	int nCount = 0;
	BOOL bResult = LoadToolbar(nIDResourceToolBar, pItems, nCount, szIcon);

	if (bResult && !SetIcons(bmpIcons, pItems, nCount, szIcon, imageState))
		bResult = FALSE;

	if (pItems)
	{
		delete[] pItems;
	}

	return bResult;
}
BOOL CXTPImageManager::SetIcons(UINT nIDResourceToolBar, UINT nIDResourceBitmap, XTPImageState imageState)
{
	UINT* pItems = NULL;
	CSize szIcon(0);
	int nCount = 0;
	BOOL bResult = LoadToolbar(nIDResourceToolBar, pItems, nCount, szIcon);

	if (bResult && !SetIcons(nIDResourceBitmap, pItems, nCount, CSize(0, 0), imageState))
		bResult = FALSE;

	if (pItems)
	{
		delete[] pItems;
	}

	return bResult;

}


BOOL CXTPImageManager::SetIcons(UINT nIDResourceToolBar, XTPImageState imageState)
{
	return SetIcons(nIDResourceToolBar, nIDResourceToolBar, imageState);
}

BOOL CXTPImageManager::SetIcons(UINT nIDResourceToolBar, CImageList& imlIcons, XTPImageState imageState)
{
	UINT* pItems = NULL;
	CSize szIcon(0);
	int nCount = 0;
	BOOL bResult = LoadToolbar(nIDResourceToolBar, pItems, nCount, szIcon);

	if (bResult && !SetIcons(imlIcons, pItems, nCount, szIcon, imageState))
		bResult = FALSE;

	if (pItems)
	{
		delete[] pItems;
	}

	return bResult;

}



BOOL CXTPImageManager::SetIcon(CXTPImageManagerIconHandle hIcon, UINT nIDCommand, CSize szIcon, XTPImageState imageState)
{
	if (hIcon.IsEmpty())
		return FALSE;

	if (szIcon == CSize(0))
	{
		szIcon = hIcon.GetExtent();
	}
	if (szIcon == CSize(0))
		return FALSE;


	CXTPImageManagerIconSet* pIconSet;
	if (m_mapImages.Lookup(nIDCommand, pIconSet))
	{
		if (imageState == xtpImageNormal) pIconSet->SetIcon(hIcon, szIcon.cx);
		if (imageState == xtpImageDisabled) pIconSet->SetDisabledIcon(hIcon, szIcon.cx);
		if (imageState == xtpImageHot) pIconSet->SetHotIcon(hIcon, szIcon.cx);
		if (imageState == xtpImageChecked) pIconSet->SetCheckedIcon(hIcon, szIcon.cx);
		if (imageState == xtpImagePressed) pIconSet->SetPressedIcon(hIcon, szIcon.cx);
	}
	else if (imageState == xtpImageNormal)
	{
		pIconSet = new CXTPImageManagerIconSet(nIDCommand, this);
		pIconSet->SetIcon(hIcon, szIcon.cx);
		m_mapImages.SetAt(nIDCommand, pIconSet);
	}
	else return FALSE;

	return TRUE;
}

BOOL CXTPImageManager::SetIcon(UINT nIDResourceIcon, UINT nIDCommand, CSize szIcon, XTPImageState imageState)
{
	LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResourceIcon);

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_GROUP_ICON);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_GROUP_ICON);
	if (hRsrc != NULL)
	{
		// Load and Lock to get a pointer to a GRPICONDIR
		HGLOBAL hGlobal = LoadResource(hInst, hRsrc);

		CXTPImageManagerIcon::GRPICONDIR* lpGrpIconDir = (CXTPImageManagerIcon::GRPICONDIR*)
			LockResource(hGlobal);

		for (WORD i = 0; i < lpGrpIconDir->idCount; i++)
		{
			CXTPImageManagerIconHandle hIcon;
			CSize szResourceIcon(lpGrpIconDir->idEntries[i].bWidth, lpGrpIconDir->idEntries[i].bHeight);

			if ((szIcon == szResourceIcon) || (szIcon == CSize(0, 0)))
			{
				if (!hIcon.CreateIconFromResource(hInst, MAKEINTRESOURCE(lpGrpIconDir->idEntries[i].nID),
					szResourceIcon, FALSE))
				{
					return FALSE;
				}

				if (!SetIcon(hIcon, nIDCommand, szResourceIcon, imageState))
				{
					return FALSE;
				}
			}
		}
	}
	else
	{
		CXTPImageManagerIconHandle hIcon;

		if (!hIcon.CreateIconFromResource(lpszResourceName, szIcon, FALSE))
		{
			return FALSE;
		}

		return SetIcon(hIcon, nIDCommand, szIcon, imageState);
	}
	return TRUE;
}


BOOL CXTPImageManager::SetIcons(CImageList& imlIcons, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState)
{
	if (!imlIcons.GetSafeHandle())
		return FALSE;

	if ((nCount == 0) && (pCommands == 0))
	{
		nCount = imlIcons.GetImageCount();
	}

	if (szIcon == CSize(0) && nCount > 0)
	{
		IMAGEINFO imageInfo;
		imlIcons.GetImageInfo(0, &imageInfo);
		szIcon = CRect(imageInfo.rcImage).Size();
	}

	BOOL bIncludeAll = imlIcons.GetImageCount() == nCount;
	int j = 0;

	for (int i = 0; i < nCount; i++)
	{
		if ((pCommands == NULL) || bIncludeAll || (pCommands[i] != 0))
		{
			HICON hIcon = imlIcons.ExtractIcon(j);
			if (hIcon)
			{
				SetIcon(hIcon, pCommands ? pCommands[i]: i, szIcon, imageState);
				DestroyIcon(hIcon);
			}
			else
			{
				return FALSE;
			}
			j++;
		}
	}
	return TRUE;
}


BOOL CXTPImageManager::SplitBitmap(HBITMAP hbmSource, int nCount, HBITMAP* pDest) const
{
	ASSERT(pDest != NULL);
	ASSERT(sizeof(int) == 4);

	if (!pDest)
		return FALSE;

	BITMAPINFO  bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	PINT pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;

	BOOL bResult = TRUE;
	int i;

	TRY
	{
		if (GetDIBits(dcSrc, hbmSource, 0, 0, NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) == 0)
			AfxThrowMemoryException();

		UINT nSize = bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 4;
		pBits = (PINT)malloc(nSize);
		if (pBits == NULL)
			AfxThrowMemoryException();

		if ((pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + 3 * sizeof(COLORREF))) == NULL)
			AfxThrowMemoryException();

		MEMCPY_S(pBitmapInfo, &bmi, sizeof(BITMAPINFOHEADER));

		if (GetDIBits(dcSrc, hbmSource, 0, bmi.bmiHeader.biHeight, pBits, pBitmapInfo, DIB_RGB_COLORS) == NULL)
			AfxThrowMemoryException();

		ASSERT(pBitmapInfo->bmiHeader.biWidth % nCount == 0);

		pBitmapInfo->bmiHeader.biWidth /= nCount;
		pBitmapInfo->bmiHeader.biSizeImage /= nCount;

		PINT* pDestBits = new PINT[nCount];
		for (i = 0; i < nCount; i++)
		{
			pDest[i] = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDestBits[i], NULL, 0);
			if (!pDest[i])
			{
				for (int j = 0; j < i; j++)
					DeleteObject(pDest[j]);

				delete[] pDestBits;

				AfxThrowMemoryException();
			}
		}

		PINT pBitsIter = pBits;

		for (int y = 0; y < pBitmapInfo->bmiHeader.biHeight; y++)
		{
			for (i = 0; i < nCount; i++)
			{
				for (int x = 0; x < pBitmapInfo->bmiHeader.biWidth; x++)
				{
					*pDestBits[i]++ = *pBitsIter++;
				}
			}
		}
		delete[] pDestBits;

	}
	CATCH (CMemoryException, e)
	{
		TRACE(_T("Failed -- Memory exception thrown."));
		bResult = FALSE;
	}
	END_CATCH

	FREE(pBits);
	FREE(pBitmapInfo);

	return bResult;

}

COLORREF CXTPImageManager::GetBitmapMaskColor(CBitmap& bmpIcons, CPoint pt)
{
	CXTPCompatibleDC dc(NULL, &bmpIcons);
	return dc.GetPixel(pt);
}

COLORREF CXTPImageManager::GetBitmapMaskColor(UINT nIDBitmap, CPoint pt)
{
	CBitmap bmp;
	VERIFY(bmp.LoadBitmap(nIDBitmap));
	return GetBitmapMaskColor(bmp, pt);
}


HBITMAP CXTPImageManager::ResampleAlphaLayer(UINT nIDBitmap, COLORREF clrMask)
{
	HBITMAP hbmAlpha = CXTPImageManagerIcon::LoadAlphaBitmap(nIDBitmap);
	if (!hbmAlpha)
		return 0;

	CBitmap bmp;
	bmp.Attach(hbmAlpha);

	return CXTPImageManager::ResampleAlphaLayer(bmp, clrMask);
}

HBITMAP CXTPImageManager::ResampleAlphaLayer(HBITMAP bmpAlpha, COLORREF clrMask)
{
	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	PBYTE pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;
	UINT nSize;

	if (!CXTPImageManagerIcon::GetBitmapBits(dcSrc, bmpAlpha, pBitmapInfo, (LPVOID&)pBits, nSize))
		return 0;

	PBYTE pTarget = NULL;
	PBYTE pSource = pBits;
	pBitmapInfo->bmiHeader.biBitCount = 24;
	pBitmapInfo->bmiHeader.biSizeImage = 0;
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;

	HBITMAP hBitmapResult = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pTarget, NULL, 0);

	if (pTarget && pSource && hBitmapResult)
	{
		BYTE byMaskRed = GetRValue(clrMask);
		BYTE byMaskGreen = GetGValue(clrMask);
		BYTE byMaskBlue = GetBValue(clrMask);

		int nCount = pBitmapInfo->bmiHeader.biHeight * pBitmapInfo->bmiHeader.biWidth;

		for (; nCount--;)
		{
			int iAlpha = pSource[3];

			if (iAlpha < 120)
			{
				pTarget[0] = byMaskRed;
				pTarget[1] = byMaskGreen;
				pTarget[2] = byMaskBlue;
			}
			else
			{
				pTarget[0] = (BYTE)((pSource[0] * (255 - iAlpha) + pSource[0] * iAlpha) >> 8);
				pTarget[1] = (BYTE)((pSource[1] * (255 - iAlpha) + pSource[1] * iAlpha) >> 8);
				pTarget[2] = (BYTE)((pSource[2] * (255 - iAlpha) + pSource[2] * iAlpha) >> 8);
			}
			pTarget += 3;
			pSource += 4;
		}
	}

	FREE(pBits);
	FREE(pBitmapInfo);

	return hBitmapResult;
}

BOOL CXTPImageManager::SetIcons(CBitmap& bmpIcons, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState, BOOL bAlpha)
{
	if (szIcon == CSize(0) && nCount > 0)
	{
		BITMAP bmpInfo;
		bmpIcons.GetBitmap(&bmpInfo);
		szIcon = CSize(bmpInfo.bmWidth / nCount, bmpInfo.bmHeight);
	}

	if (nCount == 0 && szIcon.cx > 0)
	{
		BITMAP bmpInfo;
		bmpIcons.GetBitmap(&bmpInfo);
		nCount = bmpInfo.bmWidth / szIcon.cx;
	}

	if (szIcon.cx == 0)
		return FALSE;

	if (bAlpha && !IsAlphaIconsSupported() && m_bAutoResample)
	{
		HBITMAP hBitmap = ResampleAlphaLayer(bmpIcons, m_clrMask == COLORREF(-1) ? RGB(0, 0xFF, 0) : m_clrMask);
		if (!hBitmap)
			return FALSE;

		CBitmap bmpResample;
		bmpResample.Attach(hBitmap);
		return SetIcons(bmpResample, pCommands, nCount, szIcon, imageState, FALSE);
	}

	if (bAlpha)
	{
		BITMAP bmpInfo;
		bmpIcons.GetBitmap(&bmpInfo);
		if (bmpInfo.bmWidth % szIcon.cx != 0)
			return FALSE;

		int nIconCount = bmpInfo.bmWidth / szIcon.cx;

		HBITMAP* pDestBitmap = new HBITMAP[nIconCount];

		if (!SplitBitmap(bmpIcons, nIconCount, pDestBitmap))
		{
			delete[] pDestBitmap;
			return FALSE;
		}
		BOOL bIgnoreNulls = (nIconCount == nCount);

		int j = 0;

		for (int i = 0; i < nCount; i++)
		{
			if ((!pCommands || bIgnoreNulls || pCommands[i]) && j < nIconCount)
			{

				SetIcon(pDestBitmap[j], pCommands ? pCommands[i]: i, szIcon, imageState);
				j++;
			}
		}

		for (j = 0; j < nIconCount; j++)
		{
			DeleteObject(pDestBitmap[j]);
		}

		delete[] pDestBitmap;


		return TRUE;

	}
	else
	{
		CImageList imgList;


		if (!imgList.Create(szIcon.cx, szIcon.cy, ILC_COLOR24 | ILC_MASK, 0, 1))
			return FALSE;

		COLORREF clrMask = m_clrMask != (COLORREF)-1 ? m_clrMask : GetBitmapMaskColor(bmpIcons);
		imgList.Add(&bmpIcons, clrMask);

		return SetIcons(imgList, pCommands, nCount, szIcon, imageState);
	}

}


BOOL CXTPImageManager::SetIcons(UINT nIDResourceBitmap, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState)
{
	return SetIcons(MAKEINTRESOURCE(nIDResourceBitmap), pCommands, nCount, szIcon, imageState);
}

BOOL CXTPImageManager::SetIcons(LPCTSTR lpszResourceBitmap, UINT* pCommands, int nCount, CSize szIcon, XTPImageState imageState)
{
	BOOL bAlphaBitmap = FALSE;
	CBitmap bmpIcons;

	HBITMAP hBmp = CXTPImageManagerIcon::LoadBitmapFromResource(lpszResourceBitmap, &bAlphaBitmap);

	if (!hBmp)
		return FALSE;

	bmpIcons.Attach(hBmp);

	return SetIcons(bmpIcons, pCommands, nCount, szIcon, imageState, bAlphaBitmap);
}


void CXTPImageManager::SetImageList(HIMAGELIST hImageList, int nBaseCommand, BOOL bDestoryImageList)
{
	CXTPImageManagerImageList* pImageList = new CXTPImageManagerImageList;
	pImageList->m_bDestroyImageList = bDestoryImageList;
	pImageList->m_hImageList = hImageList;
	pImageList->m_nBaseCommand = nBaseCommand;
	pImageList->m_pManager = this;

	m_arrImageList.Add(pImageList);
}



BOOL CXTPImageManager::IsPngBitmapResource(UINT nIDResourceBitmap)
{
	return CXTPImageManagerIcon::IsPngBitmapResource(AfxFindResourceHandle(MAKEINTRESOURCE(nIDResourceBitmap), _T("PNG")),
		MAKEINTRESOURCE(nIDResourceBitmap));
}



BOOL CXTPImageManager::IsAlphaBitmapResource(UINT nIDResourceBitmap)
{
	return CXTPImageManagerIcon::IsAlphaBitmapResource(AfxFindResourceHandle(MAKEINTRESOURCE(nIDResourceBitmap), RT_BITMAP),
		MAKEINTRESOURCE(nIDResourceBitmap));
}

UINT CXTPImageManager::AddCustomIcon(CXTPImageManagerIconHandle hIcon)
{
	CSize szIcon = hIcon.GetExtent();
	m_nCustomID++;

	CXTPImageManagerIconSet* pIconSet = new CXTPImageManagerIconSet(m_nCustomID, this);
	pIconSet->SetIcon(hIcon, szIcon.cx);
	m_mapImages.SetAt(m_nCustomID, pIconSet);

	return m_nCustomID;
}

void CXTPImageManager::CopyImage(UINT nCommand)
{
	CMemFile memFile;
	CArchive ar (&memFile, CArchive::store);

	Serialize(nCommand, ar);
	ar.Flush();

	DWORD dwCount = (DWORD)memFile.GetPosition();
	BYTE* pControls = memFile.Detach();

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwCount);
	if (hGlobal != NULL)
	{
		LPVOID lpData = GlobalLock(hGlobal);

		MEMCPY_S(lpData, pControls, dwCount);

		GlobalUnlock(hGlobal);
		SetClipboardData(m_nImageClipFormat, hGlobal);
	}

	ar.Close();
	memFile.Close();
	free(pControls);
}



UINT CXTPImageManager::PasteCustomImage(COleDataObject& data)
{
#ifdef __AFXOLE_H__
	HGLOBAL hGlobal = data.GetGlobalData (m_nImageClipFormat);

	if (hGlobal == NULL) return 0;

	try
	{
		CSharedFile globFile;
		globFile.SetHandle(hGlobal, FALSE);
		CArchive ar (&globFile, CArchive::load);

		m_nCustomID++;
		Serialize(m_nCustomID, ar);

		ar.Close();

	}
	catch (COleException* pEx)
	{
		pEx->Delete ();
		return 0;
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		return 0;
	}
	return m_nCustomID;
#else
	return 0;
#endif
}

BOOL CXTPImageManager::IsPrimaryImageExists(UINT nCommand, int nWidth) const
{
	CXTPImageManagerIconSet* pIconSet = GetIconSet(nCommand);

	if (pIconSet)
	{
		CXTPImageManagerIcon* pIcon = pIconSet->GetIcon(nWidth, FALSE);
		if (pIcon && !pIcon->m_bScaled)
			return TRUE;
	}

	CXTPImageManagerImageList* pImageList = GetImageList(nCommand);
	if (pImageList)
	{
		return nWidth == 0 || pImageList->GetIconSize().cx == nWidth;
	}

	return FALSE;
}

CXTPImageManagerIcon* CXTPImageManager::GetImage(UINT nCommand, int nWidth) const
{
	CXTPImageManagerIconSet* pIconSet = GetIconSet(nCommand);

	if (pIconSet)
	{
		return pIconSet->GetIcon(nWidth);
	}

	CXTPImageManagerImageList* pImageList = GetImageList(nCommand);
	if (pImageList)
	{
		return pImageList->GetIcon(nCommand);
	}

	return NULL;
}

BOOL CXTPImageManager::Lookup(UINT nCommand) const
{
	return (GetIconSet(nCommand) != NULL) || (GetImageList(nCommand) != NULL);
}

CXTPImageManagerIconSet* CXTPImageManager::GetIconSet(UINT nCommand) const
{
	CXTPImageManagerIconSet* pIconSet;
	if (m_mapImages.Lookup(nCommand, pIconSet))
	{
		return pIconSet;

	}
	return NULL;
}

CXTPImageManagerImageList* CXTPImageManager::GetImageList(UINT nCommand) const
{
	for (int i = 0; i < (int)m_arrImageList.GetSize(); i++)
	{
		CXTPImageManagerImageList* pImageList = m_arrImageList[i];

		if (pImageList->Lookup(nCommand))
			return pImageList;
	}
	return NULL;
}

void CXTPImageManager::RemoveIcon(UINT nIDCommand)
{
	CXTPImageManagerIconSet* pIconSet;
	if (m_mapImages.Lookup(nIDCommand, pIconSet))
	{
		m_mapImages.RemoveKey(nIDCommand);
		pIconSet->InternalRelease();
	}
}

void CXTPImageManager::RemoveAll()
{
	POSITION pos = m_mapImages.GetStartPosition();
	UINT nID;
	CXTPImageManagerIconSet* pIconSet;
	while (pos)
	{
		m_mapImages.GetNextAssoc(pos, nID, pIconSet);
		pIconSet->InternalRelease();
	}
	m_mapImages.RemoveAll();

	for (int i = 0; i < (int)m_arrImageList.GetSize(); i++)
	{
		delete m_arrImageList[i];
	}
	m_arrImageList.RemoveAll();

}

void CXTPImageManager::RefreshAll()
{
	POSITION pos = m_mapImages.GetStartPosition();
	UINT nID;
	CXTPImageManagerIconSet* pIconSet;

	while (pos)
	{
		m_mapImages.GetNextAssoc(pos, nID, pIconSet);
		pIconSet->RefreshAll();
	}

	for (int i = 0; i < (int)m_arrImageList.GetSize(); i++)
	{
		m_arrImageList[i]->RemoveAll();
	}
}

COLORREF CXTPImageManager::SetMaskColor(COLORREF clrMask)
{
	COLORREF clrReturn = m_clrMask;
	m_clrMask = clrMask;
	return clrReturn;
}
COLORREF CXTPImageManager::GetMaskColor() const
{
	return m_clrMask;
}

BOOL CXTPImageManager::IsAlphaIconsSupported() const
{
	return TRUE;

}

BOOL CXTPImageManager::IsAlphaIconsImageListSupported()
{
	DWORD dwComCtlVersion = XTPSystemVersion()->GetComCtlVersion();
	return HIWORD(dwComCtlVersion) > 5;
}

void CXTPImageManager::DoPropExchange(UINT nCommand, CXTPPropExchange* pPX)
{
	if (nCommand > m_nCustomID) m_nCustomID = nCommand;

	const LPCTSTR lpszCheck = _T("CXTPImageManagerIcons");
	CString strCheck = lpszCheck;

	if (pPX->IsStoring())
	{
		PX_String(pPX, _T("CheckSum"), strCheck, lpszCheck);

		ASSERT(nCommand > 0);

		CXTPImageManagerIconSet* pIconSet = GetIconSet(nCommand);
		ASSERT(pIconSet);

		if (!pIconSet)
		{
			CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Icon")));
			pEnumerator->GetPosition(0);
			return;
		}

		UINT nWidth;
		CXTPImageManagerIcon* pIcon;
		DWORD dwCount = 0;

		POSITION pos = pIconSet->m_mapIcons.GetStartPosition();
		while (pos)
		{
			pIconSet->m_mapIcons.GetNextAssoc(pos, nWidth, pIcon);
			if (pIcon->m_bScaled)
				continue;

			dwCount++;
		}

		CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Icon")));
		POSITION posEnum = pEnumerator->GetPosition(dwCount);

		pos = pIconSet->m_mapIcons.GetStartPosition();
		while (pos)
		{
			pIconSet->m_mapIcons.GetNextAssoc(pos, nWidth, pIcon);
			if (pIcon->m_bScaled)
				continue;

			CXTPPropExchangeSection secIcon(pEnumerator->GetNext(posEnum));

			PX_Long(&secIcon, _T("Width"), (long&)nWidth);
			PX_Serialize(&secIcon, _T("Data"), pIcon);
		}
	}
	else
	{
		PX_String(pPX, _T("CheckSum"), strCheck, lpszCheck);

		if (strCheck != lpszCheck)
			AfxThrowArchiveException(CArchiveException::badClass);


		CXTPImageManagerIconSet* pIconSet;
		if (m_mapImages.Lookup(nCommand, pIconSet))
		{
			pIconSet->RemoveAll();
		}
		else
		{
			pIconSet = new CXTPImageManagerIconSet(nCommand, this);
			m_mapImages.SetAt(nCommand, pIconSet);
		}

		CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Icon")));
		POSITION posEnum = pEnumerator->GetPosition();

		while (posEnum)
		{
			CXTPPropExchangeSection secIcon(pEnumerator->GetNext(posEnum));

			UINT nWidth = 16;
			PX_Long(&secIcon, _T("Width"), (long&)nWidth);

			CXTPImageManagerIcon* pIcon = new CXTPImageManagerIcon(nCommand, nWidth, 15, pIconSet);
			PX_Serialize(&secIcon, _T("Data"), pIcon);

			pIconSet->m_mapIcons.SetAt(nWidth, pIcon);
		}
	}
}

void CXTPImageManager::Serialize(UINT nCommand, CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(nCommand, &px);
}



void CXTPImageManager::DoPropExchange(CXTPPropExchange* pPX)
{
	CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*>* pIcons = GetImages();

	CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("IconSet")));
	POSITION posEnum = pEnumerator->GetPosition((DWORD)pIcons->GetCount());

	if (pPX->IsStoring())
	{
		UINT nIDCommand;
		CXTPImageManagerIconSet* pIconSet;

		POSITION pos = pIcons->GetStartPosition();

		while (pos)
		{
			pIcons->GetNextAssoc(pos, nIDCommand, pIconSet);
			CXTPPropExchangeSection secIconSet(pEnumerator->GetNext(posEnum));

			PX_Long(&secIconSet, _T("Id"), (long&)nIDCommand);
			DoPropExchange(nIDCommand, &secIconSet);
		}
	}
	else
	{
		RemoveAll();

		while (posEnum)
		{
			CXTPPropExchangeSection secIconSet(pEnumerator->GetNext(posEnum));

			UINT nIDCommand = 0;
			PX_Long(&secIconSet, _T("Id"), (long&)nIDCommand);

			DoPropExchange(nIDCommand, &secIconSet);
		}
	}

}
void CXTPImageManager::Serialize(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(&px);
}


void CXTPImageManager::AddIcons(CXTPImageManager* pImageManager)
{
	if (pImageManager == 0)
		return;


	POSITION pos = pImageManager->m_mapImages.GetStartPosition();
	UINT nID;
	CXTPImageManagerIconSet* pIconSet;

	while (pos)
	{
		pImageManager->m_mapImages.GetNextAssoc(pos, nID, pIconSet);

		if (!Lookup(nID))
		{
			m_mapImages.SetAt(nID, pIconSet);
			pIconSet->InternalAddRef();
		}
	}

}

BOOL CXTPImageManager::DoDisableBitmap(HBITMAP hBmp, COLORREF clrDisabledLight, COLORREF clrDisabledDark,
									   int nBlackAndWhiteContrast)
{
	ASSERT(nBlackAndWhiteContrast == -1 ||
		   (nBlackAndWhiteContrast >= 0 && nBlackAndWhiteContrast <= 255));

	BOOL bOfficeDisabledIcons = clrDisabledLight != (COLORREF)-1 && clrDisabledDark != (COLORREF)-1;

	BITMAP bmBitmap;

	// Get information about the surfaces you were passed.
	if (!GetObject(hBmp,  sizeof(BITMAP), &bmBitmap))  return FALSE;

	// Make sure you have data that meets your requirements.
	if (bmBitmap.bmBitsPixel != 32)
		return FALSE;

	if (bmBitmap.bmPlanes != 1)
		return FALSE;

	if (!bmBitmap.bmBits)
		return FALSE;

	DWORD dwWidthBytes = bmBitmap.bmWidthBytes;

	// Initialize the surface pointers.
	RGBQUAD *lprgbBitmap = (RGBQUAD*)bmBitmap.bmBits;

	//************************************************
	int x, y;
	for (y = 0; y < bmBitmap.bmHeight; y++)
	{
		for (x = 0; x < bmBitmap.bmWidth; x++)
		{
			if (bOfficeDisabledIcons)
			{
				double dGray = (lprgbBitmap[x].rgbBlue * 0.114 + lprgbBitmap[x].rgbGreen * 0.587 + lprgbBitmap[x].rgbRed * 0.299)/255.0;
				double dLight = 1.0 - dGray;

				lprgbBitmap[x].rgbBlue  = BYTE(dLight * GetBValue(clrDisabledDark) + dGray * GetBValue(clrDisabledLight));
				lprgbBitmap[x].rgbGreen = BYTE(dLight * GetGValue(clrDisabledDark) + dGray * GetGValue(clrDisabledLight));
				lprgbBitmap[x].rgbRed   = BYTE(dLight * GetRValue(clrDisabledDark) + dGray * GetRValue(clrDisabledLight));
			}
			else if (nBlackAndWhiteContrast != -1)
			{
				int nGray = (lprgbBitmap[x].rgbBlue * 114 + lprgbBitmap[x].rgbGreen * 587 + lprgbBitmap[x].rgbRed * 299) / 1000;
				nGray = nGray + (255 - nGray) * nBlackAndWhiteContrast / 255;
				nGray = min(nGray, 255);

				lprgbBitmap[x].rgbBlue = BYTE(nGray);
				lprgbBitmap[x].rgbGreen = BYTE(nGray);
				lprgbBitmap[x].rgbRed = BYTE(nGray);

				lprgbBitmap[x].rgbReserved = BYTE(lprgbBitmap[x].rgbReserved / CXTPImageManager::m_dDisabledAlphaFactor);
			}
			else
			{
				double dGray = (lprgbBitmap[x].rgbBlue * 0.114 + lprgbBitmap[x].rgbGreen * 0.587 + lprgbBitmap[x].rgbRed * 0.299)/255.0;

				lprgbBitmap[x].rgbBlue = (BYTE)(pow(dGray, CXTPImageManager::m_dDisabledBrightnessFactor) * 255.0);
				lprgbBitmap[x].rgbGreen = lprgbBitmap[x].rgbBlue;
				lprgbBitmap[x].rgbRed = lprgbBitmap[x].rgbBlue;

				lprgbBitmap[x].rgbReserved = BYTE(lprgbBitmap[x].rgbReserved / CXTPImageManager::m_dDisabledAlphaFactor);
			}
		}

		// Move to next scan line.
		lprgbBitmap = (RGBQUAD *)((LPBYTE)lprgbBitmap + dwWidthBytes);
	}

	return TRUE;
}

BOOL CXTPImageManager::DisableBitmap(HDC hDC, const CRect& rcRect, COLORREF clrDisabledLight, COLORREF clrDisabledDark)
{
	HDC      hdcDst  = NULL;
	HBITMAP  hbmDst  = NULL;

	BOOL    bReturn = FALSE;
	// Create surfaces for new image.
	hbmDst  = CXTPImageManager::Create32BPPDIBSection(hDC, rcRect.Width(), rcRect.Height());
	if (!hbmDst) goto HANDLEERROR;

	// Create HDCs to hold our surfaces.
	hdcDst  = CreateCompatibleDC(hDC);
	if (!hdcDst) goto HANDLEERROR;

	// Prepare the surfaces for drawing.
	SelectObject(hdcDst,  hbmDst);

	// Capture a copy of the source area.
	if (!BitBlt(hdcDst, 0, 0, rcRect.Width(), rcRect.Height(),
		 hDC, rcRect.left, rcRect.top, SRCCOPY) )
		goto HANDLEERROR;

	// Modify sourcepixels to create the destination image.
	bReturn = DoDisableBitmap(hbmDst, clrDisabledLight, clrDisabledDark);

	// Display the disabled image to the target HDC.
	if (bReturn)
	{
		BitBlt(hDC, rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height(),
			hdcDst, 0,0, SRCCOPY);
	}

HANDLEERROR:

	// Clean up the rest of the objects you created.
	DeleteDC(hdcDst);
	DeleteObject(hbmDst);

	return bReturn;
}

BOOL CXTPImageManager::BlackWhiteBitmap(HDC hDC, const CRect& rcRect, int nBlackAndWhiteContrast)
{
	HDC      hdcDst  = NULL;
	HBITMAP  hbmDst  = NULL;

	BOOL    bReturn = FALSE;
	// Create surfaces for new image.
	hbmDst  = CXTPImageManager::Create32BPPDIBSection(hDC, rcRect.Width(), rcRect.Height());
	if (!hbmDst) goto HANDLEERROR;

	// Create HDCs to hold our surfaces.
	hdcDst  = CreateCompatibleDC(hDC);
	if (!hdcDst) goto HANDLEERROR;

	// Prepare the surfaces for drawing.
	SelectObject(hdcDst,  hbmDst);

	// Capture a copy of the source area.
	if (!BitBlt(hdcDst, 0, 0, rcRect.Width(), rcRect.Height(),
		 hDC, rcRect.left, rcRect.top, SRCCOPY) )
		goto HANDLEERROR;

	// Modify sourcepixels to create the destination image.
	bReturn = DoDisableBitmap(hbmDst, (COLORREF)-1, (COLORREF)-1, nBlackAndWhiteContrast);

	// Display the disabled image to the target HDC.
	if (bReturn)
	{
		BitBlt(hDC, rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height(),
			hdcDst, 0,0, SRCCOPY);
	}

HANDLEERROR:

	// Clean up the rest of the objects you created.
	DeleteDC(hdcDst);
	DeleteObject(hbmDst);

	return bReturn;
}

BOOL CXTPImageManager::GetBitmapPixel(HBITMAP hBmp, const CPoint& pt, COLORREF& rclrPixel)
{
	BITMAP bmBitmap;

	// Get information about the surfaces you were passed.
	if (!GetObject(hBmp,  sizeof(BITMAP), &bmBitmap))
		return FALSE;

	// Make sure you have data that meets your requirements.
	if (bmBitmap.bmBitsPixel != 32)
		return FALSE;

	if (bmBitmap.bmPlanes != 1)
		return FALSE;

	if (!bmBitmap.bmBits)
		return FALSE;

	if (pt.x < 0 || pt.x >= bmBitmap.bmWidth ||
		pt.y < 0 || pt.y >= bmBitmap.bmHeight)
		return FALSE;

	// Initialize the surface pointers.
	DWORD *lprgbBitmap = (DWORD*)((BYTE*)bmBitmap.bmBits + bmBitmap.bmWidthBytes * pt.y);
	rclrPixel = lprgbBitmap[pt.x];

	return TRUE;
}

