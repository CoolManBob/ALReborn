// XTTabBaseTheme.cpp: implementation of the CXTTabBaseTheme class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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

#include "Common/XTPIntel80Helpers.h"
#include "Common/XTPWinThemeWrapper.h"

#include "XTTabBaseTheme.h"
#include "XTDefines.h"
#include "XTUtil.h"
#include "XTGlobal.h"
#include "XTVC50Helpers.h"
#include "XTTabBase.h"
#include "XTTabCtrlButtons.h"

#define DB_LEFT        0x0002
#define DB_RIGHT       0x0004
#define DB_CROSS       0x0008

#define DB_ENABLED     0x0000
#define DB_DISABLED    0x0100
#define DB_PRESSED     0x1000
#define DB_OVER        0x4000

#define DB_ISENABLED(Style) (!((Style)&DB_DISABLED))
#define DB_ISPRESSED(Style) (((Style) & (DB_PRESSED)) != 0)
#define DB_ISOVER(Style) (((Style)&DB_OVER) == DB_OVER)


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_THEME_FACTORY(CXTTabBaseTheme)
//===========================================================================
// CXTTabBaseTheme class
//===========================================================================

CXTTabBaseTheme::CXTTabBaseTheme()
	: m_bUseWinThemes(TRUE)
	, m_bSystemDrawing(FALSE)
{

}

CXTTabBaseTheme::~CXTTabBaseTheme()
{

}

void CXTTabBaseTheme::RefreshMetrics()
{
	CXTThemeManagerStyle ::RefreshMetrics();

	m_clrNormalText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrSelectedText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_iEdge = ::GetSystemMetrics(SM_CYEDGE);

	m_wrapTheme.OpenTheme(0, L"TAB");
}

void CXTTabBaseTheme::DrawBorders(CDC* pDC, const CRect& rcClient)
{
	CRect rcBorder = rcClient;

	if (m_bUseWinThemes && m_wrapTheme.IsAppThemeReady())
	{
	}
	else
	{
		pDC->Draw3dRect(&rcBorder, GetXtremeColor(COLOR_3DHILIGHT),
			GetXtremeColor(COLOR_3DDKSHADOW));
		rcBorder.DeflateRect(1, 1);
		pDC->Draw3dRect(&rcBorder, GetXtremeColor(COLOR_3DFACE),
			GetXtremeColor(COLOR_3DSHADOW));
	}
}

void CXTTabBaseTheme::DrawTabCtrl(CDC* pDC, CXTTabBase* pTabCtrlBase)
{
	CTabCtrl* pTabCtrl = pTabCtrlBase->GetTabCtrlImpl();

	CXTPClientRect rcClient(pTabCtrl);
	pDC->FillSolidRect(rcClient, GetBackColor());

	if ((m_enumTheme == xtThemeDefault) && (IsTop(GetTabStyle(pTabCtrl)) || !m_wrapTheme.IsAppThemed()))
	{
		ShowButtons(pTabCtrlBase);
		PROTECTED_DEFWINDOWPROC_CALL(CXTTabBaseTheme, pTabCtrlBase->GetTabCtrlImpl(), WM_PAINT, (WPARAM)pDC->m_hDC, 0);
		return;
	}

	if (m_bSystemDrawing)
	{
		ShowButtons(pTabCtrlBase);
		PROTECTED_DEFWINDOWPROC_CALL(CXTTabBaseTheme, pTabCtrlBase->GetTabCtrlImpl(), WM_PAINT, (WPARAM)pDC->m_hDC, 0);
		return;
	}

	DrawBorders(pDC, rcClient);

	if (pTabCtrl->GetItemCount() != 0)
	{
		FillHeader(pDC, pTabCtrlBase, GetHeaderRect(pTabCtrlBase));

		// draw remaining tabs
		for (int iItem = 0; iItem < pTabCtrl->GetItemCount(); ++iItem)
		{
			DrawTab(pDC, pTabCtrlBase, iItem);
		}

		DrawTab(pDC, pTabCtrlBase, pTabCtrl->GetCurSel());

		ShowButtons(pTabCtrlBase);

		if (pTabCtrlBase->m_bXPBorder)
		{
			CRect rcChild;
			pTabCtrlBase->GetChildRect(rcChild);
			rcChild.InflateRect(1, 1);
			DrawBorder(pDC, rcChild);
		}
	}
	else
	{
		ShowButtons(pTabCtrlBase);
	}
}

void CXTTabBaseTheme::FillHeader(CDC* pDC, CXTTabBase* pTabCtrlBase, CRect rcHeader)
{
	pDC->FillSolidRect(rcHeader, GetBackColor());
	DWORD dwStyle = GetTabStyle(pTabCtrlBase->GetTabCtrlImpl());

	if (m_bUseWinThemes && m_wrapTheme.IsAppThemeReady())
	{
		CXTPClientRect rc(pTabCtrlBase->GetTabCtrlImpl());

		if (IsRight(dwStyle))
		{
			rc.right = rcHeader.left + 1;
		}
		else if (IsLeft(dwStyle))
		{
			rc.left = rcHeader.right;
		}
		else if (IsBottom(dwStyle))
		{
			rc.bottom = rcHeader.top + 1;
		}
		else
		{
			rc.top = rcHeader.bottom;
		}

		m_wrapTheme.DrawThemeBackground(pDC->GetSafeHdc(), TABP_PANE,
			0, &rc, 0);
		return;
	}

	// draw a line to separate tab area.

	if (IsRight(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.left-2, rcHeader.top,
			1, rcHeader.Height()-1, GetXtremeColor(COLOR_3DSHADOW));
		pDC->FillSolidRect(rcHeader.left-1, rcHeader.top,
			1, rcHeader.Height()-1, GetXtremeColor(COLOR_3DDKSHADOW));
	}
	else if (IsLeft(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.right, rcHeader.top,
			1, rcHeader.Height(), GetXtremeColor(COLOR_3DHILIGHT));
	}
	else if (IsBottom(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.left+1, rcHeader.top-2,
			rcHeader.Width()-2, 1, GetXtremeColor(COLOR_3DSHADOW));
		pDC->FillSolidRect(rcHeader.left, rcHeader.top-1,
			rcHeader.Width(), 1, GetXtremeColor(COLOR_3DDKSHADOW));
	}
	else
	{
		pDC->FillSolidRect(rcHeader.left, rcHeader.bottom,
			rcHeader.Width()-1, 1, GetXtremeColor(COLOR_3DHILIGHT));
	}
}

void CXTTabBaseTheme::SwapColors(COLORREF& clrLight, COLORREF& clrDark)
{
	COLORREF clr(clrLight);
	clrLight = clrDark;
	clrDark = clr;
}

void CXTTabBaseTheme::VerticalLine(CDC* pDC, int x, int y, int nLength, COLORREF clr)
{
	if (clr != COLORREF_NULL)
		pDC->FillSolidRect(x, y, 1, nLength, clr);
}

void CXTTabBaseTheme::HorizontalLine(CDC* pDC, int x, int y, int nLength, COLORREF clr)
{
	if (clr != COLORREF_NULL)
		pDC->FillSolidRect(x, y, nLength, 1, clr);
}

void CXTTabBaseTheme::Pixel(CDC* pDC, int x, int y, COLORREF clr)
{
	if (clr != COLORREF_NULL)
		pDC->SetPixel(x, y, clr);
}
void CXTTabBaseTheme::DrawRotatedBitsBottom(int cx, int cy, UINT* pSrcBits, UINT* pDestBits)
{
	UINT* pSrc = &pSrcBits[cx * (cy - 1)];
	for (int i = 0; i < cy; i++)
	{
		pSrcBits = pSrc;
		for (int j = 0; j < cx; j++)
		{
			*pDestBits++ = *pSrcBits++;
		}
		pSrc -= cx;
	}
}

void CXTTabBaseTheme::DrawRotatedBitsRight(int cx, int cy, UINT* pSrcBits, UINT* pDestBits)
{
	UINT* pSrc = pSrcBits;
	for (int i = 0; i < cy; i++)
	{
		pSrcBits = pSrc;
		for (int j = 0; j < cx; j++)
		{
			*pDestBits++ = *pSrcBits;
			pSrcBits += cy;
		}
		pSrc += 1;
	}
}

void CXTTabBaseTheme::DrawRotatedBitsLeft(int cx, int cy, UINT* pSrcBits, UINT* pDestBits)
{
	UINT* pSrc = pSrcBits;
	pDestBits += cx * cy - 1;
	for (int i = 0; i < cy; i++)
	{
		pSrcBits = pSrc;
		for (int j = 0; j < cx; j++)
		{
			*pDestBits-- = *pSrcBits;
			pSrcBits += cy;
		}
		pSrc += 1;
	}
}


void CXTTabBaseTheme::DrawRotatedButton(CDC* pDC, CRect rcItem, BOOL bSelected, BOOL bSwap, LPFNDRAWROTATEDBITS pfnRotatedProc)
{
	const int cx = rcItem.Width();
	const int cy = rcItem.Height();

	BITMAPINFOHEADER BMI;
	// Fill in the header info.
	ZeroMemory (&BMI, sizeof(BMI));
	BMI.biSize = sizeof(BITMAPINFOHEADER);
	BMI.biWidth = bSwap ? cy : cx;
	BMI.biHeight = bSwap ? cx : cy;
	BMI.biPlanes = 1;
	BMI.biBitCount = 32;
	BMI.biCompression = BI_RGB;   // No compression

	UINT* pSrcBits = NULL;
	HBITMAP hbmSrc = CreateDIBSection (NULL, (BITMAPINFO *)&BMI, DIB_RGB_COLORS, (void **)&pSrcBits, 0, 0l);

	BMI.biWidth = cx;
	BMI.biHeight = cy;
	UINT* pDestBits = NULL;
	HBITMAP hbmDest = CreateDIBSection (NULL, (BITMAPINFO *)&BMI, DIB_RGB_COLORS, (void **)&pDestBits, 0, 0l);


	CDC dc;
	dc.CreateCompatibleDC(NULL);
	HGDIOBJ hbmpOld = ::SelectObject(dc, hbmSrc);

	CRect rcDraw(0, 0, bSwap ? cy: cx, bSwap ? cx : cy);

	dc.FillSolidRect(rcDraw, GetBackColor());

	m_wrapTheme.DrawThemeBackground(dc.GetSafeHdc(), TABP_TABITEM,
		bSelected ? TIS_SELECTED : TIS_NORMAL, &rcDraw, 0);

	(*pfnRotatedProc)(cx, cy, pSrcBits, pDestBits);

	pDC->DrawState(rcItem.TopLeft(), rcItem.Size(), hbmDest, DST_BITMAP);

	::SelectObject(dc, hbmpOld);

	DeleteObject(hbmSrc);
	DeleteObject(hbmDest);
}



void CXTTabBaseTheme::FillTabFaceNativeWinXP(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected)
{
	DWORD dwStyle = GetTabStyle(pTabCtrl);


	if (IsRight(dwStyle))
	{
		rcItem.InflateRect(1, 0, 0, 0);
		if (bSelected)
		{
			rcItem.InflateRect(1, 2, 2, 2);
			pDC->ExcludeClipRect(rcItem.left, rcItem.top, rcItem.left + 1, rcItem.top + 1);
			pDC->ExcludeClipRect(rcItem.left, rcItem.bottom - 1, rcItem.left + 1, rcItem.bottom);
		}

		DrawRotatedButton(pDC, rcItem, bSelected, TRUE, DrawRotatedBitsRight);
	}
	else if (IsLeft(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(2, 2, 1, 2);
			pDC->ExcludeClipRect(rcItem.right - 1, rcItem.top, rcItem.right, rcItem.top + 1);
			pDC->ExcludeClipRect(rcItem.right - 1, rcItem.bottom - 1, rcItem.right, rcItem.bottom);
		}

		DrawRotatedButton(pDC, rcItem, bSelected, TRUE, DrawRotatedBitsLeft);
	}
	else if (IsBottom(dwStyle))
	{
		rcItem.InflateRect(0, 1, 0, 0);
		if (bSelected)
		{
			rcItem.InflateRect(2, 1, 2, 2);
			pDC->ExcludeClipRect(rcItem.left, rcItem.top, rcItem.left + 1, rcItem.top + 1);
			pDC->ExcludeClipRect(rcItem.right - 1, rcItem.top, rcItem.right, rcItem.top + 1);
		}

		DrawRotatedButton(pDC, rcItem, bSelected, FALSE, DrawRotatedBitsBottom);
	}
	else
	{
		if (bSelected)
		{
			rcItem.InflateRect(2, 2, 2, 1);
			pDC->ExcludeClipRect(rcItem.left, rcItem.bottom - 1, rcItem.left + 1, rcItem.bottom);
			pDC->ExcludeClipRect(rcItem.right - 1, rcItem.bottom - 1, rcItem.right, rcItem.bottom);
		}

		m_wrapTheme.DrawThemeBackground(pDC->GetSafeHdc(), TABP_TABITEM,
			bSelected ? TIS_SELECTED: TIS_NORMAL, &rcItem, 0);
	}

}

void CXTTabBaseTheme::FillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected)
{
	if (m_bUseWinThemes && m_wrapTheme.IsAppThemeReady())
	{
		FillTabFaceNativeWinXP(pDC, pTabCtrl, rcItem, bSelected);
		return ;

	}

	DWORD dwStyle = GetTabStyle(pTabCtrl);

	if (IsRight(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(2, 2, 1, 2);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			HorizontalLine(pDC, rcItem.left, rcItem.top, rcItem.Width()-1, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left, rcItem.top+1, rcItem.Width()-2, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.right-1, rcItem.top+1, GetXtremeColor(COLOR_3DHILIGHT));

			VerticalLine(pDC, rcItem.right, rcItem.top+2, rcItem.Height()-4, GetXtremeColor(COLOR_3DDKSHADOW));
			VerticalLine(pDC, rcItem.right-1, rcItem.top+2, rcItem.Height()-4, GetXtremeColor(COLOR_3DSHADOW));

			HorizontalLine(pDC, rcItem.left+1, rcItem.bottom-2, rcItem.Width()-1, GetXtremeColor(COLOR_3DSHADOW));
			HorizontalLine(pDC, rcItem.left+1, rcItem.bottom-1, rcItem.Width()-2, GetXtremeColor(COLOR_3DDKSHADOW));
			Pixel(pDC, rcItem.right-1, rcItem.bottom-2, GetXtremeColor(COLOR_3DDKSHADOW));
		}
		else
		{
			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			HorizontalLine(pDC, rcItem.left, rcItem.top, rcItem.Width()-2, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left, rcItem.top+1, rcItem.Width()-2, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.right-2, rcItem.top+1, GetXtremeColor(COLOR_3DHILIGHT));

			VerticalLine(pDC, rcItem.right-1, rcItem.top+2, rcItem.Height()-4, GetXtremeColor(COLOR_3DDKSHADOW));
			VerticalLine(pDC, rcItem.right-2, rcItem.top+2, rcItem.Height()-4, GetXtremeColor(COLOR_3DSHADOW));

			HorizontalLine(pDC, rcItem.left, rcItem.bottom-2, rcItem.Width()-2, GetXtremeColor(COLOR_3DSHADOW));
			HorizontalLine(pDC, rcItem.left, rcItem.bottom-1, rcItem.Width()-2, GetXtremeColor(COLOR_3DDKSHADOW));
			Pixel(pDC, rcItem.right-2, rcItem.bottom-2, GetXtremeColor(COLOR_3DDKSHADOW));
		}
	}
	else if (IsLeft(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(2, 2, 1, 1);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			HorizontalLine(pDC, rcItem.left+2, rcItem.top, rcItem.Width()-2, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left+2, rcItem.top+1, rcItem.Width()-2, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.left+1, rcItem.top+1, GetXtremeColor(COLOR_3DHILIGHT));

			VerticalLine(pDC, rcItem.left, rcItem.top+2, rcItem.Height()-3, GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left+1, rcItem.top+2, rcItem.Height()-3, GetXtremeColor(COLOR_3DFACE));

			HorizontalLine(pDC, rcItem.left+1, rcItem.bottom-1, rcItem.Width()-2, GetXtremeColor(COLOR_3DSHADOW));
			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom, rcItem.Width()-3, GetXtremeColor(COLOR_3DDKSHADOW));
			Pixel(pDC, rcItem.left+1, rcItem.bottom-1, GetXtremeColor(COLOR_3DDKSHADOW));
		}
		else
		{
			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			HorizontalLine(pDC, rcItem.left+2, rcItem.top, rcItem.Width()-1, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left+2, rcItem.top+1, rcItem.Width()-2, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.left+1, rcItem.top+1, GetXtremeColor(COLOR_3DHILIGHT));

			VerticalLine(pDC, rcItem.left, rcItem.top+2, rcItem.Height()-4, GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left+1, rcItem.top+2, rcItem.Height()-4, GetXtremeColor(COLOR_3DFACE));

			HorizontalLine(pDC, rcItem.left+1, rcItem.bottom-2, rcItem.Width()-1, GetXtremeColor(COLOR_3DSHADOW));
			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom-1, rcItem.Width()-2, GetXtremeColor(COLOR_3DDKSHADOW));
			Pixel(pDC, rcItem.left+1, rcItem.bottom-2, GetXtremeColor(COLOR_3DDKSHADOW));
		}
	}
	else if (IsBottom(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(2, 2, 2, 1);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			VerticalLine(pDC, rcItem.left, rcItem.top, rcItem.Height()-1, GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left+1, rcItem.top, rcItem.Height()-1, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.left+1, rcItem.bottom-1, GetXtremeColor(COLOR_3DHILIGHT));

			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom, rcItem.Width()-4, GetXtremeColor(COLOR_3DDKSHADOW));
			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom-1, rcItem.Width()-4, GetXtremeColor(COLOR_3DSHADOW));

			VerticalLine(pDC, rcItem.right-1, rcItem.top+1, rcItem.Height()-2, GetXtremeColor(COLOR_3DDKSHADOW));
			VerticalLine(pDC, rcItem.right-2, rcItem.top+1, rcItem.Height()-2, GetXtremeColor(COLOR_3DSHADOW));
			Pixel(pDC, rcItem.right-2, rcItem.bottom-1, GetXtremeColor(COLOR_3DDKSHADOW));
		}
		else
		{
			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			VerticalLine(pDC, rcItem.left, rcItem.top, rcItem.Height()-2, GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left+1, rcItem.top, rcItem.Height()-2, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.left+1, rcItem.bottom-2, GetXtremeColor(COLOR_3DHILIGHT));

			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom-1, rcItem.Width()-4, GetXtremeColor(COLOR_3DDKSHADOW));
			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom-2, rcItem.Width()-4, GetXtremeColor(COLOR_3DSHADOW));

			VerticalLine(pDC, rcItem.right-1, rcItem.top-1, rcItem.Height()-1, GetXtremeColor(COLOR_3DDKSHADOW));
			VerticalLine(pDC, rcItem.right-2, rcItem.top-1, rcItem.Height()-1, GetXtremeColor(COLOR_3DSHADOW));
			Pixel(pDC, rcItem.right-2, rcItem.bottom-2, GetXtremeColor(COLOR_3DDKSHADOW));
		}
	}
	else
	{
		if (bSelected)
		{
			rcItem.InflateRect(2, 2, 2, 1);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			VerticalLine(pDC, rcItem.left, rcItem.top+2, rcItem.Height()-1, GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left+1, rcItem.top+2, rcItem.Height()-1, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.left+1, rcItem.top+1, GetXtremeColor(COLOR_3DHILIGHT));

			HorizontalLine(pDC, rcItem.left+2, rcItem.top, rcItem.Width()-4, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left+2, rcItem.top+1, rcItem.Width()-4, GetXtremeColor(COLOR_3DFACE));

			VerticalLine(pDC, rcItem.right-1, rcItem.top+2, rcItem.Height()-3, GetXtremeColor(COLOR_3DDKSHADOW));
			VerticalLine(pDC, rcItem.right-2, rcItem.top+2, rcItem.Height()-3, GetXtremeColor(COLOR_3DSHADOW));
			Pixel(pDC, rcItem.right-2, rcItem.top+1, GetXtremeColor(COLOR_3DDKSHADOW));
		}
		else
		{
			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			VerticalLine(pDC, rcItem.left, rcItem.top+2, rcItem.Height()-2, GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left+1, rcItem.top+2, rcItem.Height()-2, GetXtremeColor(COLOR_3DFACE));
			Pixel(pDC, rcItem.left+1, rcItem.top+1, GetXtremeColor(COLOR_3DHILIGHT));

			HorizontalLine(pDC, rcItem.left+2, rcItem.top, rcItem.Width()-4, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left+2, rcItem.top+1, rcItem.Width()-4, GetXtremeColor(COLOR_3DFACE));

			VerticalLine(pDC, rcItem.right-1, rcItem.top+2, rcItem.Height()-2, GetXtremeColor(COLOR_3DDKSHADOW));
			VerticalLine(pDC, rcItem.right-2, rcItem.top+2, rcItem.Height()-2, GetXtremeColor(COLOR_3DSHADOW));
			Pixel(pDC, rcItem.right-2, rcItem.top+1, GetXtremeColor(COLOR_3DDKSHADOW));
		}
	}
}

void CXTTabBaseTheme::DrawTabIcon(CDC* pDC, CTabCtrl* pTabCtrl, CRect& rcItem, int iItem)
{
	TC_ITEM tci;
	tci.mask = TCIF_IMAGE;

	if (pTabCtrl->GetItem(iItem, &tci))
	{
		CImageList* pImageList = pTabCtrl->GetImageList();
		if (pImageList && tci.iImage >= 0)
		{
			CPoint point = rcItem.TopLeft();

			DWORD dwStyle = GetTabStyle(pTabCtrl);

			if (IsHorz(dwStyle))
			{
				point.Offset(m_iEdge*2, IsTop(dwStyle) ? m_iEdge : 1);
			}
			else
			{
				point.Offset(IsLeft(dwStyle) ? m_iEdge : 1, m_iEdge*2);
			}

			// Draw any associated icons.
			pImageList->Draw(pDC, tci.iImage, point, ILD_TRANSPARENT);

			IMAGEINFO info;
			pImageList->GetImageInfo(tci.iImage, &info);
			CRect rcImage(info.rcImage);

			if (IsHorz(dwStyle))
			{
				rcItem.left += rcImage.Width() + m_iEdge;
			}
			else
			{
				rcItem.top += rcImage.Height() + m_iEdge;
			}
		}
	}
}

void CXTTabBaseTheme::DrawTabText(CDC* pDC, CTabCtrl* pTabCtrl, CRect& rcItem, int iItem, BOOL bSelected, BOOL bBoldFont)
{
	TCHAR szLabel[256];
	szLabel[0] = 0;

	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = szLabel;
	tci.cchTextMax = _countof(szLabel);

	if (pTabCtrl->GetItem(iItem, &tci))
	{
		int nLen = (int)_tcslen(szLabel);
		if (nLen > 0)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(bSelected ? m_clrSelectedText : m_clrNormalText);

			// Set the font for the tab label.
			DWORD dwStyle = GetTabStyle(pTabCtrl);
			CXTPFontDC font(pDC, (bBoldFont && bSelected) ? (IsHorz(dwStyle) ? &XTAuxData().fontBold : &XTAuxData().fontVertBold): (IsHorz(dwStyle) ? &XTAuxData().font : &XTAuxData().fontVert));

			// Draw the tab label.
			if (IsHorz(dwStyle))
			{
				pDC->DrawText(szLabel, nLen, rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS);
			}
			else
			{
				CSize sz = pDC->GetTextExtent(szLabel, nLen);
				rcItem.left = rcItem.right-(rcItem.Width()-sz.cy + 1)/ 2;
				rcItem.top = rcItem.top+(rcItem.Height()-sz.cx + 1)/ 2;

				pDC->DrawText(szLabel, nLen, &rcItem, DT_SINGLELINE | DT_NOCLIP);
			}
		}
	}
}

void CXTTabBaseTheme::DrawTab(CDC* pDC, CXTTabBase* pTabCtrlBase, int iItem)
{
	// Get the current tab selection.
	CTabCtrl* pTabCtrl = pTabCtrlBase->GetTabCtrlImpl();
	int iCurSel = pTabCtrl->GetCurSel();

	// Get the tab item size.
	CRect rcItem;
	pTabCtrl->GetItemRect(iItem, &rcItem);

	// Draw the tab item.
	BOOL bSelected = (iItem == iCurSel);
	FillTabFace(pDC, pTabCtrl, rcItem, bSelected);
	DrawTabIcon(pDC, pTabCtrl, rcItem, iItem);
	DrawTabText(pDC, pTabCtrl, rcItem, iItem, bSelected, pTabCtrlBase->m_bBoldFont);
}

CRect CXTTabBaseTheme::GetHeaderRect(CXTTabBase* pTabCtrlBase)
{
	CTabCtrl* pTabCtrl = pTabCtrlBase->GetTabCtrlImpl();

	CRect rcHeader;
	pTabCtrl->GetClientRect(&rcHeader);

	// get the selected tab item rect.
	CRect rcItem;
	pTabCtrl->GetItemRect(pTabCtrl->GetCurSel(), &rcItem);

	DWORD dwStyle = GetTabStyle(pTabCtrlBase->GetTabCtrlImpl());

	if (IsRight(dwStyle))
	{
		rcHeader.left = rcItem.left;
	}
	else if (IsLeft(dwStyle))
	{
		rcHeader.right = rcItem.right;
	}
	else if (IsBottom(dwStyle))
	{
		rcHeader.top = rcItem.top;
	}
	else
	{
		rcHeader.bottom = (rcHeader.top + rcItem.Height()) * pTabCtrl->GetRowCount() + m_iEdge;
	}
	return rcHeader;
}

void CXTTabBaseTheme::DrawButton(CDC* /*pDC*/, CXTTabCtrlButton* /*pButton*/, COLORREF /*clrButton*/)
{
	ASSERT(FALSE);
}

COLORREF CXTTabBaseTheme::FillButtons(CDC* /*pDC*/, CXTTabCtrlButtons* /*pButtons*/)
{
	ASSERT(FALSE);
	return 0;
}

void CXTTabBaseTheme::ShowButtons(CXTTabBase* pTabCtrlBase)
{
	CXTTabCtrlButtons* pNavBtns = pTabCtrlBase->GetButtons();
	CTabCtrl* pTabCtrl = pTabCtrlBase->GetTabCtrlImpl();

	if (::IsWindow(pTabCtrl->GetSafeHwnd()) && pNavBtns)
	{
		const bool bVertical = (pTabCtrl->GetStyle() & TCS_VERTICAL) == TCS_VERTICAL;
		const bool bMultiLine = bVertical || ((pTabCtrl->GetStyle() & TCS_MULTILINE) == TCS_MULTILINE);

		if ((m_enumTheme != xtThemeDefault) && !bVertical && !bMultiLine && !pTabCtrlBase->m_bAutoCondensing)
		{
			CWnd* pWnd = pTabCtrl->GetDlgItem(1);
			pNavBtns->SubclassTabButtons(pWnd); // pWnd can be NULL
			pNavBtns->ShowButtons(TRUE);
		}
		else
		{
			pNavBtns->ShowButtons(FALSE);
		}
	}
}

CXTPPaintManagerColor CXTTabBaseTheme::GetBackColor() const
{
	return GetXtremeColor(COLOR_3DFACE);
}

void CXTTabBaseTheme::AdjustBorders(CXTTcbItem* pMember)
{
	if (pMember && ::IsWindow(pMember->pWnd->GetSafeHwnd()))
	{
		// add WS_BORDER, WS_EX_CLIENTEDGE and WS_EX_STATICEDGE window styles.
		pMember->pWnd->ModifyStyle(0, (pMember->dwStyle & WS_BORDER));
		pMember->pWnd->ModifyStyleEx(0, (pMember->dwExStyle & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)));

		// Force WM_NCCALCSIZE to be called.
		pMember->pWnd->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}
}

void CXTTabBaseTheme::DrawBorder(CDC* /*pDC*/, const CRect& /*rcChild*/)
{
}

//////////////////////////////////////////////////////////////////////////
// CXTTabBaseThemeOfficeXP

CXTTabBaseThemeOfficeXP::CXTTabBaseThemeOfficeXP()
{
}

void CXTTabBaseThemeOfficeXP::RefreshMetrics()
{
	CXTTabBaseTheme::RefreshMetrics();

	m_clrNormalText.SetStandardValue(GetXtremeColor(XPCOLOR_TAB_INACTIVE_TEXT));
	m_clrSelectedText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_iEdge *= 2;
}

void CXTTabBaseThemeOfficeXP::DrawBorders(CDC* /*pDC*/, const CRect& /*rcClient*/)
{
}

void CXTTabBaseThemeOfficeXP::DrawTabIcon(CDC* pDC, CTabCtrl* pTabCtrl, CRect& rcItem, int iItem)
{
	TC_ITEM tci;
	tci.mask = TCIF_IMAGE;

	if (pTabCtrl->GetItem(iItem, &tci))
	{
		CImageList* pImageList = pTabCtrl->GetImageList();
		if (pImageList && tci.iImage >= 0)
		{
			CPoint point = rcItem.TopLeft();

			DWORD dwStyle = GetTabStyle(pTabCtrl);

			if (IsHorz(dwStyle))
			{
				point.Offset(m_iEdge*2, m_iEdge/2);
			}
			else
			{
				point.Offset(IsRight(dwStyle) ? 1 : 2, m_iEdge*2);
			}

			// Draw any associated icons.
			pImageList->Draw(pDC, tci.iImage, point, ILD_TRANSPARENT);

			IMAGEINFO info;
			pImageList->GetImageInfo(tci.iImage, &info);
			CRect rcImage(info.rcImage);

			if (IsHorz(dwStyle))
			{
				rcItem.left += rcImage.Width() + m_iEdge;
			}
			else
			{
				rcItem.top += rcImage.Height() + m_iEdge;
			}
		}
	}
}

void CXTTabBaseThemeOfficeXP::FillHeader(CDC* pDC, CXTTabBase* pTabCtrlBase, CRect rcHeader)
{
	pDC->FillSolidRect(rcHeader, GetXtremeColor(XPCOLOR_TAB_INACTIVE_BACK));

	// draw a line to separate tab area.
	DWORD dwStyle = GetTabStyle(pTabCtrlBase->GetTabCtrlImpl());

	if (IsRight(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.left-1, rcHeader.top,
			1, rcHeader.Height(), GetXtremeColor(COLOR_3DHILIGHT));
	}
	else if (IsLeft(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.right, rcHeader.top,
			1, rcHeader.Height(), GetXtremeColor(COLOR_3DHILIGHT));
	}
	else if (IsBottom(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.left, rcHeader.top,
			rcHeader.Width(), 1, GetXtremeColor(COLOR_WINDOWFRAME));
	}
	else
	{
		pDC->FillSolidRect(rcHeader.left, rcHeader.bottom-1,
			rcHeader.Width(), 1, GetXtremeColor(COLOR_3DFACE));

		pDC->FillSolidRect(rcHeader.left, rcHeader.bottom-2,
			rcHeader.Width(), 1, GetXtremeColor(COLOR_3DHILIGHT));
	}
}

void CXTTabBaseThemeOfficeXP::FillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected)
{
	DWORD dwStyle = GetTabStyle(pTabCtrl);

	if (IsRight(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(1, -3, -1, 1);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			HorizontalLine(pDC, rcItem.left, rcItem.top, rcItem.Width(), GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.right, rcItem.top, rcItem.Height() + 1, GetXtremeColor(COLOR_WINDOWFRAME));
			HorizontalLine(pDC, rcItem.left, rcItem.bottom, rcItem.Width(), GetXtremeColor(COLOR_WINDOWFRAME));
		}
		else
		{
			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom+3, rcItem.Width()-4, GetXtremeColor(COLOR_3DSHADOW));
		}
	}
	else if (IsLeft(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(1, -3, 1, 1);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			HorizontalLine(pDC, rcItem.left, rcItem.top, rcItem.Width(), GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.left, rcItem.top, rcItem.Height(), GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left, rcItem.bottom, rcItem.Width(), GetXtremeColor(COLOR_WINDOWFRAME));
		}
		else
		{
			HorizontalLine(pDC, rcItem.left+2, rcItem.bottom+3, rcItem.Width()-4, GetXtremeColor(COLOR_3DSHADOW));
		}
	}
	else if (IsBottom(dwStyle))
	{
		if (bSelected)
		{
			rcItem.InflateRect(-2, 0, 1, 1);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			VerticalLine(pDC, rcItem.left, rcItem.top+1, rcItem.Height()-1, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left, rcItem.bottom-1, rcItem.Width(), GetXtremeColor(COLOR_WINDOWFRAME));
			VerticalLine(pDC, rcItem.right, rcItem.top+1, rcItem.Height()-1, GetXtremeColor(COLOR_WINDOWFRAME));
		}
		else
		{
			VerticalLine(pDC, rcItem.right+1, rcItem.top+2, rcItem.Height()-3, GetXtremeColor(COLOR_3DSHADOW));
		}
	}
	else
	{
		if (bSelected)
		{
			rcItem.InflateRect(-2, 1, 2, 2);

			// paint the selected tab background.
			pDC->FillSolidRect(&rcItem, GetBackColor());

			VerticalLine(pDC, rcItem.left, rcItem.top+1, rcItem.Height()-2, GetXtremeColor(COLOR_3DHILIGHT));
			HorizontalLine(pDC, rcItem.left, rcItem.top, rcItem.Width(), GetXtremeColor(COLOR_3DHILIGHT));
			VerticalLine(pDC, rcItem.right, rcItem.top, rcItem.Height()-1, GetXtremeColor(COLOR_WINDOWFRAME));
		}
		else
		{
			VerticalLine(pDC, rcItem.right+2, rcItem.top+2, rcItem.Height()-3, GetXtremeColor(COLOR_3DSHADOW));
		}
	}
}

COLORREF CXTTabBaseThemeOfficeXP::FillButtons(CDC* pDC, CXTTabCtrlButtons* pButton)
{
	CXTPClientRect rcClient(pButton);

	pDC->FillSolidRect(rcClient, XTPColorManager()->GetColor(XPCOLOR_TAB_INACTIVE_BACK));

	if ((pButton->GetTabCtrl()->GetStyle() & TCS_BOTTOM) != 0)
	{
		pDC->FillSolidRect(0, 0, rcClient.right, 1, GetXtremeColor(COLOR_BTNTEXT));
	}
	else
	{
		pDC->FillSolidRect(0, rcClient.bottom - 1, rcClient.right, 1, GetXtremeColor(COLOR_3DHILIGHT));
	}
	return XTPColorManager()->GetColor(XPCOLOR_TAB_INACTIVE_TEXT);
}

void CXTTabBaseThemeOfficeXP::DrawButton(CDC* pDC, CXTTabCtrlButton* pButton, COLORREF clrButton)
{
	ASSERT (pDC != NULL);

	CRect rc (pButton->GetRect());
	DWORD wStyle = pButton->m_wStyle;

	if (DB_ISENABLED(wStyle))
	{
		if (DB_ISPRESSED(wStyle))
			pDC->Draw3dRect(rc, RGB(0, 0, 0), GetSysColor(0x16));
		else if (DB_ISOVER(wStyle))
			pDC->Draw3dRect(rc, GetSysColor(0x16), RGB(0, 0, 0));

		if (DB_ISPRESSED(wStyle))
		{
			rc.OffsetRect(1, 1);
		}
	}

	CXTPPenDC pen(*pDC, clrButton);

	CBrush brush;
	CBrush* pOldBrush;

	if (DB_ISENABLED(wStyle))
	{
		brush.CreateSolidBrush(clrButton);
		pOldBrush = pDC->SelectObject(&brush);
	}
	else
	{
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	}

	if (wStyle & DB_CROSS)
	{
		pDC->MoveTo(rc.left + 3, rc.top + 3);
		pDC->LineTo(rc.right - 4, rc.bottom - 5);
		pDC->MoveTo(rc.left + 4, rc.top + 3);
		pDC->LineTo(rc.right - 3, rc.bottom - 5);

		pDC->MoveTo(rc.left + 3, rc.bottom - 6);
		pDC->LineTo(rc.right - 4, rc.top + 2);
		pDC->MoveTo(rc.left + 4, rc.bottom - 6);
		pDC->LineTo(rc.right - 3, rc.top + 2);
	}
	else if (wStyle & DB_LEFT)
	{
		CPoint pts[] =
		{
			CPoint(rc.right - 5, rc.top + 2),
			CPoint(rc.right - 9, rc.top + 6),
			CPoint(rc.right - 5, rc.top + 10)
		};
		pDC->Polygon(pts, 3);
	}
	else if (wStyle & DB_RIGHT)
	{
		CPoint pts[] =
		{
			CPoint(rc.left + 4, rc.top + 2),
			CPoint(rc.left + 8, rc.top + 6),
			CPoint(rc.left + 4, rc.top + 10)
		};
		pDC->Polygon(pts, 3);
	}

	pDC->SelectObject(pOldBrush);
}

void CXTTabBaseThemeOfficeXP::AdjustBorders(CXTTcbItem* pMember)
{
	if (pMember && ::IsWindow(pMember->pWnd->GetSafeHwnd()))
	{
		// remove WS_BORDER, WS_EX_CLIENTEDGE and WS_EX_STATICEDGE window styles.
		pMember->pWnd->ModifyStyle(WS_BORDER, 0);
		pMember->pWnd->ModifyStyleEx(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE, 0);

		// Force WM_NCCALCSIZE to be called.
		pMember->pWnd->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED);
	}
}

void CXTTabBaseThemeOfficeXP::DrawBorder(CDC* pDC, const CRect& rcChild)
{
	pDC->Draw3dRect(&rcChild,
		GetXtremeColor(XPCOLOR_3DSHADOW), GetXtremeColor(XPCOLOR_3DSHADOW));
}

//////////////////////////////////////////////////////////////////////////
// CXTTabBaseThemeOffice2003

CXTTabBaseThemeOffice2003::CXTTabBaseThemeOffice2003()
{
}

void CXTTabBaseThemeOffice2003::RefreshMetrics()
{
	CXTTabBaseThemeOfficeXP::RefreshMetrics();

	m_clrHeaderFace.SetStandardValue(GetBackColor(),
		XTPColorManager()->LightColor(GetBackColor(), GetXtremeColor(COLOR_WINDOW), 0xcd));
	m_clrShadow.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	m_clrHighlight.SetStandardValue(GetXtremeColor(COLOR_3DHIGHLIGHT));
	m_clrDarkShadow.SetStandardValue(GetXtremeColor(COLOR_3DDKSHADOW));
	m_clrButtonSelected.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED));
	m_clrButtonNormal.SetStandardValue(GetBackColor());

	m_clrNormalText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrSelectedText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
		{
			m_clrHeaderFace.SetStandardValue(RGB(196, 218, 250), RGB(252, 253, 254));
			m_clrDarkShadow.SetStandardValue(RGB(0, 53, 154));
			m_clrShadow.SetStandardValue(RGB(117, 166, 241));
			m_clrButtonSelected.SetStandardValue(RGB(255, 194, 115));
			m_clrButtonNormal.SetStandardValue(RGB(162, 192, 246));
		}
		break;
	case xtpSystemThemeOlive:
		{
			m_clrHeaderFace.SetStandardValue(RGB(242, 241, 228), RGB(252, 254, 254));
			m_clrDarkShadow.SetStandardValue(RGB(96, 119, 107));
			m_clrShadow.SetStandardValue(RGB(176, 194, 140));
			m_clrButtonNormal.SetStandardValue(RGB(208, 222, 170));
			m_clrButtonSelected.SetStandardValue(RGB(255, 194, 115));

		}
		break;
	case xtpSystemThemeSilver:
		{
			m_clrHeaderFace.SetStandardValue(RGB(243, 243, 247), RGB(252, 254, 254));
			m_clrDarkShadow.SetStandardValue(RGB(118, 116, 146));
			m_clrShadow.SetStandardValue(RGB(186, 185, 206));
			m_clrButtonNormal.SetStandardValue(RGB(217, 217, 230));
			m_clrButtonSelected.SetStandardValue(RGB(255, 194, 115));

		}
		break;
	}
}

void CXTTabBaseThemeOffice2003::FillHeader(CDC* pDC, CXTTabBase* pTabCtrlBase, CRect rcHeader)
{
	COLORREF clrLight = m_clrHeaderFace.clrLight, clrDark = m_clrHeaderFace.clrDark;

	DWORD dwStyle = GetTabStyle(pTabCtrlBase->GetTabCtrlImpl());
	BOOL bHoriz = !(dwStyle & TCS_VERTICAL);
	if ((dwStyle & TCS_BOTTOM) || (dwStyle & TCS_RIGHT))  SwapColors(clrLight, clrDark);

	XTPDrawHelpers()->GradientFill(pDC, rcHeader, clrLight, clrDark, !bHoriz);

	if (IsRight(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.left-1, rcHeader.top,
			1, rcHeader.Height(), m_clrDarkShadow);
	}
	else if (IsLeft(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.right, rcHeader.top,
			1, rcHeader.Height(), m_clrDarkShadow);
	}
	else if (IsBottom(dwStyle))
	{
		pDC->FillSolidRect(rcHeader.left, rcHeader.top-1,
			rcHeader.Width(), 1, m_clrDarkShadow);
	}
	else
	{
		pDC->FillSolidRect(rcHeader.left, rcHeader.bottom-2,
			rcHeader.Width(), 1, m_clrDarkShadow);
	}
}

COLORREF CXTTabBaseThemeOffice2003::GradientFillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected)
{
	COLORREF clrItem = bSelected ? m_clrButtonSelected : m_clrButtonNormal;
	COLORREF clrLight = XTPColorManager()->LightColor(clrItem, RGB(255, 255, 255), 300), clrDark = clrItem;

	DWORD dwStyle = GetTabStyle(pTabCtrl);
	BOOL bHoriz = !(dwStyle & TCS_VERTICAL);
	if ((dwStyle & TCS_BOTTOM) || (dwStyle & TCS_RIGHT))  SwapColors(clrLight, clrDark);

	XTPDrawHelpers()->GradientFill(pDC, rcItem, clrLight, clrDark, !bHoriz);

	return clrItem;
}

void CXTTabBaseThemeOffice2003::FillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected)
{
	CRect rc(rcItem);
	CRect rcClient;
	pTabCtrl->GetClientRect(&rcClient);

	DWORD dwStyle = GetTabStyle(pTabCtrl);

	if (IsRight(dwStyle))
	{
		COLORREF clr = GradientFillTabFace(pDC, pTabCtrl, CRect(rc.left-(bSelected ? 1 : 0), rc.top + 1, rc.right, rc.bottom), bSelected);

		HorizontalLine(pDC, rc.left, rc.top, rc.Width() - 1, m_clrDarkShadow);
		Pixel(pDC, rc.right - 1, rc.top + 1, m_clrDarkShadow);

		VerticalLine(pDC, rc.right - 0, rc.top + 2, rc.Height() - 3, m_clrDarkShadow);

		HorizontalLine(pDC, rc.left, rc.bottom, rc.Width() - 1, m_clrDarkShadow);
		Pixel(pDC, rc.right - 1, rc.bottom - 1, m_clrDarkShadow);

		if (bSelected)
		{
			VerticalLine(pDC, rc.left - 2, rcClient.top, rcClient.Height(), clr);
		}
	}
	else if (IsLeft(dwStyle))
	{
		COLORREF clr = GradientFillTabFace(pDC, pTabCtrl, CRect(rc.left + 1, rc.top + 1, rc.right + (bSelected ? 1 : 0), rc.bottom), bSelected);

		HorizontalLine(pDC, rc.left + 2, rc.top, rc.Width() - 2, m_clrDarkShadow);
		Pixel(pDC, rc.left + 1, rc.top + 1, m_clrDarkShadow);

		VerticalLine(pDC, rc.left, rc.top + 2, rc.Height() - 3, m_clrDarkShadow);

		HorizontalLine(pDC, rc.left + 2, rc.bottom, rc.Width() - 1, m_clrDarkShadow);
		Pixel(pDC, rc.left + 1, rc.bottom - 1, m_clrDarkShadow);

		if (bSelected)
		{
			VerticalLine(pDC, rc.right+1, rcClient.top, rcClient.Height(), clr);
		}
	}
	else if (IsBottom(dwStyle))
	{
		rc.InflateRect(0, 0, 0, 1);

		COLORREF clr = GradientFillTabFace(pDC, pTabCtrl, CRect(rc.left + 1, rc.top - (bSelected ? 1 : 0), rc.right, rc.bottom - 1), bSelected);

		VerticalLine(pDC, rc.left, rc.top, rc.Height() - 1, m_clrDarkShadow);
		Pixel(pDC, rc.left + 1, rc.top+rc.Height() - 1, m_clrDarkShadow);

		HorizontalLine(pDC, rc.left + 2, rc.bottom, rc.Width() - 3, m_clrDarkShadow);

		VerticalLine(pDC, rc.right, rc.top, rc.Height() - 1, m_clrDarkShadow);
		Pixel(pDC, rc.left + rc.Width() - 1, rc.top + rc.Height() - 1, m_clrDarkShadow);

		if (bSelected)
		{
			HorizontalLine(pDC, rcClient.left, rc.top-2, rcClient.Width(), clr);
		}
	}
	else
	{
		rc.InflateRect(0, 2, 0, 0);

		COLORREF clr = GradientFillTabFace(pDC, pTabCtrl, CRect(rc.left + 1, rc.top + 1, rc.right, rc.bottom + (bSelected ? 1 : 0)), bSelected);

		VerticalLine(pDC, rc.left, rc.top + 2, rc.Height() - 2, m_clrDarkShadow);
		Pixel(pDC, rc.left + 1, rc.top + 1, m_clrDarkShadow);

		HorizontalLine(pDC, rc.left + 2, rc.top, rc.Width() - 3, m_clrDarkShadow);

		VerticalLine(pDC, rc.right, rc.top + 2, rc.Height() - 2, m_clrDarkShadow);
		Pixel(pDC, rc.left + rc.Width() - 1, rc.top + 1, m_clrDarkShadow);

		if (bSelected)
		{
			HorizontalLine(pDC, rcClient.left, rc.bottom + 1, rcClient.Width(), clr);
		}
	}
}

void CXTTabBaseThemeOffice2003::DrawBorder(CDC* pDC, const CRect& rcChild)
{
	pDC->Draw3dRect(&rcChild,
		GetMSO2003Color(XPCOLOR_SEPARATOR), GetMSO2003Color(XPCOLOR_SEPARATOR));
}

CXTPPaintManagerColor CXTTabBaseThemeOffice2003::GetBackColor() const
{
	return XTPColorManager()->grcDockBar.clrDark;
}

COLORREF CXTTabBaseThemeOffice2003::FillButtons(CDC* pDC, CXTTabCtrlButtons* pButton)
{
	CXTPClientRect rcClient(pButton);

	COLORREF clrLight = m_clrHeaderFace.clrLight, clrDark = m_clrHeaderFace.clrDark;

	DWORD dwStyle = GetTabStyle(pButton->GetTabCtrl());
	BOOL bHoriz = !(dwStyle & TCS_VERTICAL);
	if ((dwStyle & TCS_BOTTOM) || (dwStyle & TCS_RIGHT))  SwapColors(clrLight, clrDark);

	XTPDrawHelpers()->GradientFill(pDC, rcClient, clrLight, clrDark, !bHoriz);

	return 0;
}
