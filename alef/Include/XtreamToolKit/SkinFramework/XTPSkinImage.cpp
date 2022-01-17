// XTPSkinImage.cpp: implementation of the CXTPSkinImage class.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/tmschema.h"
#include "Common/XTPVC80Helpers.h"

#include "XTPSkinImage.h"
#include "XTPSkinManagerResource.h"
#include "XTPSkinManager.h"
#include "XTPSkinDrawTools.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TILE_VERTICAL     1
#define TILE_HORIZONTAL   2
#define TILE_ALL          3


//////////////////////////////////////////////////////////////////////
// CXTPSkinImage

CXTPSkinImage::CXTPSkinImage()
{
	m_hBitmap = NULL;
	m_bAlpha = FALSE;
	m_bFiltered = FALSE;
	m_bInvert = FALSE;
	m_bMirrorImage = TRUE;
}

CXTPSkinImage::~CXTPSkinImage()
{
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
	}
}

void CXTPSkinImage::SetBitmap(HBITMAP hBitmap, BOOL bAlpha /*= FALSE*/)
{
	ASSERT(m_hBitmap == NULL);

	m_hBitmap = hBitmap;
	m_bAlpha = bAlpha;
}

#define FREE(hHandle) if (hHandle) { free(hHandle); hHandle = 0; }

void CXTPSkinImage::FilterImage(COLORREF clrTransparent)
{
	if (!m_hBitmap || m_bFiltered)
		return;

	m_bFiltered = TRUE;

	CXTPSkinManager* pManager =  XTPSkinManager();

	if (!pManager->IsColorFilterExists())
		return;

	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);

	if (!m_bAlpha)
	{
		BITMAPINFO bmi;
		HBITMAP hbm;
		LPBYTE pBits;
		BITMAP hbmInfo;
		GetObject(m_hBitmap, sizeof(BITMAP), &hbmInfo);

		// Initialize header to 0s.
		ZeroMemory(&bmi, sizeof(bmi));

		// Fill out the fields you care about.
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = hbmInfo.bmWidth;
		bmi.bmiHeader.biHeight = hbmInfo.bmHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		// Create the surface.
		hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)&pBits, NULL, 0);

		{
			CXTPCompatibleDC dc(NULL, hbm);
			dc.DrawState(CPoint(0, 0), CSize(hbmInfo.bmWidth, hbmInfo.bmHeight), m_hBitmap, 0, NULL);
		}

		DeleteObject(m_hBitmap);
		m_hBitmap = hbm;
	}

	PBYTE pBits = 0;
	PBITMAPINFO pBitmapInfo = 0;
	HBITMAP hBitmapAlpha = 0;

	TRY
	{
		UINT nSize;
		if (!CXTPImageManagerIcon::GetBitmapBits(dcSrc, m_hBitmap, pBitmapInfo, (LPVOID&)pBits, nSize))
			return;

		BYTE* pDest = NULL;

		hBitmapAlpha = CreateDIBSection(dcSrc, pBitmapInfo, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if (pDest == NULL || hBitmapAlpha == NULL)
			AfxThrowMemoryException();

		MEMCPY_S(pDest, pBits, nSize);

		for (UINT i = 0; i < nSize; i += 4)
		{
			COLORREF clr = RGB(pDest[i + 2], pDest[i + 1], pDest[i + 0]);

			if (clr != clrTransparent)
			{
				pManager->ApplyColorFilter(clr);

				pDest[i + 0] = GetBValue(clr);
				pDest[i + 1] = GetGValue(clr);
				pDest[i + 2] = GetRValue(clr);
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

	DeleteObject(m_hBitmap);
	m_hBitmap = hBitmapAlpha;
}

BOOL CXTPSkinImage::LoadFile(LPCTSTR lpszFileName)
{
	m_bAlpha = FALSE;

	BOOL bAlpha = FALSE;
	HBITMAP hBmp = CXTPImageManagerIcon::LoadBitmapFromFile(lpszFileName, &bAlpha);

	if (!hBmp)
		return FALSE;

	if (bAlpha)
	{
		if (XTPSkinManager()->GetSchema()->m_bPreMultiplyImages)
		{
			m_hBitmap = CXTPImageManagerIcon::PreMultiplyAlphaBitmap(hBmp, &m_bAlpha);
			DeleteObject(hBmp);
		}
		else
		{
			m_hBitmap = hBmp;
			m_bAlpha = TRUE;
		}
	}
	else
	{
		m_hBitmap = hBmp;
	}

	return TRUE;
}

BOOL CXTPSkinImage::LoadFile(HMODULE hModule, LPCTSTR lpszBitmapFileName)
{
	if (!hModule)
		return FALSE;

	m_bAlpha = FALSE;

	ASSERT(m_hBitmap == NULL);

	BOOL bAlpha = FALSE;
	HBITMAP hBmp = XTPSkinFrameworkLoadBitmap(hModule, lpszBitmapFileName, bAlpha);

	if (!hBmp)
		return FALSE;

	if (bAlpha)
	{
		if (XTPSkinManager()->GetSchema()->m_bPreMultiplyImages)
		{
			m_hBitmap = CXTPImageManagerIcon::PreMultiplyAlphaBitmap(hBmp, &m_bAlpha);
			DeleteObject(hBmp);
		}
		else
		{
			m_hBitmap = hBmp;
			m_bAlpha = TRUE;
		}
	}
	else
	{
		m_hBitmap = hBmp;
	}

	return m_hBitmap != NULL;
}

BOOL CXTPSkinImage::DrawImagePart(CDC* pDCDest, const CRect& rcDest, CDC* pDCSrc, const CRect& rcSrc) const
{
	if (rcDest.Width() <= 0 || rcDest.Height() <= 0 || rcSrc.Width() <= 0 || rcSrc.Height() <= 0)
		return TRUE;

	BOOL bResult = TRUE;

	if (m_bAlpha)
	{
		bResult = XTPImageManager()->AlphaBlend(pDCDest->GetSafeHdc(), rcDest, pDCSrc->GetSafeHdc(), rcSrc);
	}
	else if ((rcSrc.Width() == rcDest.Width()) && (rcSrc.Height() == rcDest.Height()))
	{
		bResult = pDCDest->BitBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
			pDCSrc, rcSrc.left, rcSrc.top, SRCCOPY);
	}
	else
	{
		bResult = pDCDest->StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
			pDCSrc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), SRCCOPY);
	}

	return bResult;
}


BOOL CXTPSkinImage::DrawImageTile(CDC* pDCDest, const CRect& rcDest, CDC* pDCSrc, const CRect& rcSrc, BOOL bTile) const
{
	if (rcDest.Width() <= 0 || rcDest.Height() <= 0 || rcSrc.Width() <= 0 || rcSrc.Height() <= 0)
		return TRUE;

	if (!bTile)
	{
		return DrawImagePart(pDCDest, rcDest, pDCSrc, rcSrc);
	}

	if (rcSrc.Width() == 1 && (rcSrc.Height() == 1 || rcSrc.Height() == rcDest.Height()))
	{
		return DrawImagePart(pDCDest, rcDest, pDCSrc, rcSrc);
	}

	if (rcSrc.Height() == 1 && (rcSrc.Width() == 1 || rcSrc.Width() == rcDest.Width()))
	{
		return DrawImagePart(pDCDest, rcDest, pDCSrc, rcSrc);
	}

	if (rcSrc.right <= rcSrc.left || rcDest.right <= rcDest.left)
		return TRUE;

	if (rcSrc.bottom <= rcSrc.top || rcDest.bottom <= rcDest.top)
		return TRUE;

	int nHeight = bTile & TILE_VERTICAL? min(rcSrc.Height(), rcDest.Height()): rcDest.Height();
	int nWidth = bTile & TILE_HORIZONTAL? min(rcSrc.Width(), rcDest.Width()): rcDest.Width();

	for (int x = rcDest.left; x < rcDest.right; x += nWidth)
	{
		for (int y = rcDest.top; y < rcDest.bottom; y += nHeight)
		{
			CRect rcDestTile(x, y, x + nWidth, y + nHeight);
			CRect rcSrcTile(rcSrc);

			if (bTile & TILE_VERTICAL)
			{
				if (rcSrcTile.Height() > rcDestTile.Height())
					rcSrcTile.bottom = rcSrcTile.top + rcDestTile.Height();

				if (rcDestTile.bottom > rcDest.bottom)
				{
					rcDestTile.bottom = rcDest.bottom;
					rcSrcTile.bottom = rcSrcTile.top + rcDestTile.Height();
				}
			}

			if (bTile & TILE_HORIZONTAL)
			{
				if (rcSrcTile.Width() > rcDestTile.Width())
					rcSrcTile.right = rcSrcTile.left + rcDestTile.Width();

				if (rcDestTile.right > rcDest.right)
				{
					rcDestTile.right = rcDest.right;
					rcSrcTile.right = rcSrcTile.left + rcDestTile.Width();
				}
			}

			DrawImagePart(pDCDest, rcDestTile, pDCSrc, rcSrcTile);
		}
	}
	return TRUE;
}




void CXTPSkinImage::DrawImage(CDC* pDC, const CRect& rcDest, const CRect& rcSrc, const CRect& rcSizingMargins, COLORREF clrTransparent, int nSizingType, BOOL bBorderOnly)
{
	if (rcSizingMargins.top + rcSizingMargins.bottom > rcSrc.Height())
		return;

	if (clrTransparent == COLORREF_NULL || m_bAlpha)
	{
		DrawImage(pDC, rcDest, rcSrc, rcSizingMargins, nSizingType, bBorderOnly);
		return;
	}

	FilterImage(clrTransparent);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcDest.Width(), rcDest.Height());

	CXTPCompatibleDC dc(pDC, &bmp);

	CRect rcDestOrig(0, 0, rcDest.Width(), rcDest.Height());

	dc.FillSolidRect(rcDestOrig, clrTransparent);
	DrawImage(&dc, rcDestOrig, rcSrc, rcSizingMargins, nSizingType, bBorderOnly);

	XTPImageManager()->TransparentBlt(*pDC, rcDest, dc, rcDestOrig, clrTransparent);

}

void CXTPSkinImage::InvertBitmap()
{
	if (!m_bAlpha)
		return;

	HBITMAP hBitmap = m_hBitmap;
	m_hBitmap = CXTPImageManagerIcon::InvertAlphaBitmap(m_hBitmap);
	DeleteObject(hBitmap);

	m_bInvert = !m_bInvert;
}

void CXTPSkinImage::DrawImage(CDC* pDC, const CRect& rcDest, const CRect& rcSrc, const CRect& rcSizingMargins, int nSizingType, BOOL bBorderOnly)
{
	if (m_hBitmap == NULL)
		return;

	if (rcSizingMargins.top + rcSizingMargins.bottom > rcSrc.Height())
		return;

	FilterImage(COLORREF_NULL);


	pDC->SetStretchBltMode(COLORONCOLOR);

	CRect rcDestSizingMargins = rcSizingMargins;
	BOOL bTile = nSizingType == ST_TILE? TILE_ALL: 0;

	if (rcDest.left >= rcDest.right || rcDest.top >= rcDest.bottom)
		return;

	BOOL bRtl = CXTPDrawHelpers::IsContextRTL(pDC);

	if (m_bAlpha && m_bMirrorImage)
	{
		if ((bRtl && !m_bInvert) || (!bRtl && m_bInvert))
		{
			InvertBitmap();
		}
	}

	CXTPCompatibleDC dcSrc(pDC, m_hBitmap);

	if (bRtl && !m_bAlpha && m_bMirrorImage)
	{
		CXTPDrawHelpers::SetContextRTL(&dcSrc, FALSE);
	}

	if (rcSizingMargins.IsRectNull())
	{
		DrawImageTile(pDC, rcDest, &dcSrc, rcSrc, bTile);
		return;
	}

	if ((rcDest.Height() <= rcSizingMargins.top + rcSizingMargins.bottom) || (rcSrc.Height() == rcSizingMargins.top + rcSizingMargins.bottom))
	{
		rcDestSizingMargins.top = MulDiv(rcDest.Height(), rcSizingMargins.top, (rcSizingMargins.top + rcSizingMargins.bottom));
		rcDestSizingMargins.bottom = rcDest.Height() - rcDestSizingMargins.top;

		if (rcDestSizingMargins.bottom > 0 && (rcDestSizingMargins.bottom % 2 != rcSizingMargins.bottom % 2))
			rcDestSizingMargins.bottom++;


		if (bTile) bTile -= TILE_VERTICAL;
	}

	if ((rcDest.Width() <= rcSizingMargins.left + rcSizingMargins.right) || (rcSrc.Width() == rcSizingMargins.left + rcSizingMargins.right))
	{
		rcDestSizingMargins.left = MulDiv(rcDest.Width(), rcSizingMargins.left, (rcSizingMargins.left + rcSizingMargins.right));
		rcDestSizingMargins.right = rcDest.Width() - rcDestSizingMargins.left;

		if (bTile) bTile -= TILE_HORIZONTAL;
	}

	/*if ((rcDestSizingMargins.top +  rcDestSizingMargins.bottom <
		rcSizingMargins.top +  rcSizingMargins.bottom)
		&& rcDest.Width() < rcSrc.Width() + 3)
	{
		DrawImagePart(pDC, CRect(rcDest.left, rcDest.top, rcDest.right, rcDest.top + rcDestSizingMargins.top),
			&dcSrc, CRect(rcSrc.left, rcSrc.top , rcSrc.right, rcSrc.top + rcSizingMargins.top));

		DrawImagePart(pDC, CRect(rcDest.left, rcDest.bottom - rcDestSizingMargins.bottom, rcDest.right, rcDest.bottom),
			&dcSrc, CRect(rcSrc.left, rcSrc.bottom - rcSizingMargins.bottom , rcSrc.right, rcSrc.bottom ));

		return;
	}*/

	DrawImagePart(pDC, CRect(rcDest.left, rcDest.top, rcDest.left + rcDestSizingMargins.left, rcDest.top + rcDestSizingMargins.top),
		&dcSrc, CRect(rcSrc.left, rcSrc.top, rcSrc.left + rcSizingMargins.left, rcSrc.top + rcSizingMargins.top ));
	DrawImageTile(pDC, CRect(rcDest.left + rcDestSizingMargins.left, rcDest.top, rcDest.right - rcDestSizingMargins.right, rcDest.top + rcDestSizingMargins.top),
		&dcSrc, CRect(rcSrc.left + rcSizingMargins.left, rcSrc.top, rcSrc.right - rcSizingMargins.right, rcSrc.top + rcSizingMargins.top ), bTile);
	DrawImagePart(pDC, CRect(rcDest.right - rcDestSizingMargins.right, rcDest.top, rcDest.right, rcDest.top + rcDestSizingMargins.top),
		&dcSrc, CRect(rcSrc.right - rcSizingMargins.right, rcSrc.top , rcSrc.right, rcSrc.top + rcSizingMargins.top ));

	DrawImageTile(pDC, CRect(rcDest.left, rcDest.top + rcDestSizingMargins.top, rcDest.left + rcDestSizingMargins.left, rcDest.bottom - rcDestSizingMargins.bottom),
		&dcSrc, CRect(rcSrc.left, rcSrc.top + rcSizingMargins.top , rcSrc.left + rcSizingMargins.left, rcSrc.bottom - rcSizingMargins.bottom ), bTile);

	if (!bBorderOnly)
	{
		DrawImageTile(pDC, CRect(rcDest.left + rcDestSizingMargins.left, rcDest.top + rcDestSizingMargins.top, rcDest.right - rcDestSizingMargins.right, rcDest.bottom - rcDestSizingMargins.bottom),
			&dcSrc, CRect(rcSrc.left + rcSizingMargins.left, rcSrc.top + rcSizingMargins.top , rcSrc.right - rcSizingMargins.right, rcSrc.bottom - rcSizingMargins.bottom ), bTile);
	}

	DrawImageTile(pDC, CRect(rcDest.right - rcDestSizingMargins.right , rcDest.top + rcDestSizingMargins.top, rcDest.right, rcDest.bottom - rcDestSizingMargins.bottom),
		&dcSrc, CRect(rcSrc.right - rcSizingMargins.right, rcSrc.top + rcSizingMargins.top , rcSrc.right, rcSrc.bottom - rcSizingMargins.bottom), bTile);

	DrawImagePart(pDC, CRect(rcDest.left, rcDest.bottom - rcDestSizingMargins.bottom, rcDest.left + rcDestSizingMargins.left, rcDest.bottom),
		&dcSrc, CRect(rcSrc.left, rcSrc.bottom - rcSizingMargins.bottom , rcSrc.left + rcSizingMargins.left, rcSrc.bottom ));
	DrawImageTile(pDC, CRect(rcDest.left + rcDestSizingMargins.left, rcDest.bottom - rcDestSizingMargins.bottom, rcDest.right - rcDestSizingMargins.right, rcDest.bottom),
		&dcSrc, CRect(rcSrc.left + rcSizingMargins.left, rcSrc.bottom - rcSizingMargins.bottom  , rcSrc.right - rcSizingMargins.right, rcSrc.bottom ), bTile);
	DrawImagePart(pDC, CRect(rcDest.right - rcDestSizingMargins.right, rcDest.bottom - rcDestSizingMargins.bottom, rcDest.right, rcDest.bottom),
		&dcSrc, CRect(rcSrc.right - rcSizingMargins.right, rcSrc.bottom - rcSizingMargins.bottom , rcSrc.right, rcSrc.bottom));

}

CSize CXTPSkinImage::GetExtent() const
{
	if (!m_hBitmap)
		return CSize(0, 0);

	BITMAP bmpInfo;
	GetObject(m_hBitmap, sizeof(BITMAP), &bmpInfo);

	return CSize(bmpInfo.bmWidth, bmpInfo.bmHeight);
}

DWORD CXTPSkinImage::GetHeight() const
{
	return GetExtent().cy;
}

DWORD CXTPSkinImage::GetWidth() const
{
	return GetExtent().cx;
}

BOOL CXTPSkinImage::IsAlphaImage() const
{
	return m_bAlpha;
}

CRect CXTPSkinImage::GetSource(int nState, int nCount) const
{
	CSize szExtent = GetExtent();
	CRect rcImage(0, 0, szExtent.cx, szExtent.cy / nCount);
	rcImage.OffsetRect(0, nState * rcImage.Height());

	return rcImage;
}

//////////////////////////////////////////////////////////////////////////
//

CXTPSkinImages::CXTPSkinImages()
{

}

CXTPSkinImages::~CXTPSkinImages()
{
	RemoveAll();
}

void CXTPSkinImages::RemoveAll()
{
	POSITION pos = m_mapImages.GetStartPosition();
	CString strImageFile;
	CXTPSkinImage* pImage;
	while (pos != NULL)
	{
		m_mapImages.GetNextAssoc( pos, strImageFile, (void*&)pImage);
		delete pImage;
	}
	m_mapImages.RemoveAll();
}

CXTPSkinImage* CXTPSkinImages::LoadFile(CXTPSkinManagerResourceFile* pResourceFile, LPCTSTR lpszImageFile)
{
	if (!pResourceFile)
		return NULL;

	if (lpszImageFile == NULL)
		return NULL;

	CXTPSkinImage* pImage = 0;
	if (m_mapImages.Lookup(lpszImageFile, (void*&)pImage))
		return pImage;

	pImage = pResourceFile->LoadImage(lpszImageFile);
	if (!pImage)
		return NULL;

	m_mapImages.SetAt(lpszImageFile, pImage);
	return pImage;
}

CSize CXTPSkinImages::GetExtent(CXTPSkinManagerResourceFile* pResourceFile, LPCTSTR lpszImageFile)
{
	CXTPSkinImage* pImage = LoadFile(pResourceFile, lpszImageFile);

	if (!pImage)
		return CSize(0, 0);

	return pImage->GetExtent();
}
