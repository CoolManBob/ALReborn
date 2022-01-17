// XTPCalendarPaintManager.cpp : implementation file
//
// This file is a part of the XTREME CALENDAR MFC class library.
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
#include "Resource.h"

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarPaintManager.h"
#include "XTPCalendarDayViewEvent.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarEventLabel.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarWeekView.h"
#include "XTPCalendarMonthView.h"
#include "XTPCalendarData.h"
#include "XTPCalendarDayViewTimeScale.h"

static const double dXTPMaxAlpha = 75.;

static const LPCTSTR cszGlyphsFontName = _T("MS Outlook");

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CXTPCalendarPaintManager::CDayViewEventPart::OnDraw(CDC* pDC, CXTPCalendarDayViewEvent* pViewEvent)
{
	if (!pDC || !pViewEvent || !pViewEvent->GetEvent() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcEvent(pViewEvent->GetEventRect());
	if (rcEvent.IsRectEmpty())
	{
		return;
	}

	// fill event background
	CXTPCalendarEventLabel* ptrLabel = pViewEvent->GetEvent()->GetLabel();
	COLORREF crBackground = ptrLabel ? ptrLabel->m_clrColor : GetBackgroundColor();

	BOOL bSelected = pViewEvent->IsSelected();
	int nMDEFlags = pViewEvent->GetMultiDayEventFlags();

	BOOL bMultiDay = (nMDEFlags & xtpCalendarMultiDayFMLmask) != 0;

	CString strLoc = pViewEvent->GetItemTextLocation();
	CString strText = pViewEvent->GetItemTextSubject();
	if (strLoc.GetLength() > 0)
	{
		strText += _T(" (") + strLoc + _T(")");
	}
	CString strTime;

	if (!bMultiDay && !pViewEvent->HideEventTime())
	{
		COleDateTime dtStart = pViewEvent->GetEvent()->GetStartTime();
		COleDateTime dtEnd = pViewEvent->GetEvent()->GetEndTime();

		strTime = pViewEvent->FormatEventTime(dtStart);

		if (!CXTPCalendarUtils::IsEqual(dtStart, dtEnd))
		{
			strTime += _T("-") + pViewEvent->FormatEventTime(dtEnd);
		}
	}

	CRect rcBorder(rcEvent);
	rcBorder.left += 1;

	pDC->FillSolidRect(rcBorder, crBackground);
	pDC->FillSolidRect(rcBorder.left, rcBorder.top, rcBorder.Width(), 1, m_pPaintManager->m_clrWindowText);
	pDC->FillSolidRect(rcBorder.left - 1, rcBorder.bottom - (bMultiDay ? 1 : 0),
		rcBorder.Width() + 1, 1, m_pPaintManager->m_clrWindowText);

	if (!bMultiDay)
	{
		pDC->FillSolidRect(rcBorder.right, rcBorder.top, 1, rcBorder.Height()+1, m_pPaintManager->m_clrWindowText);
	}

	if (bSelected && bMultiDay)
	{
		pDC->FillSolidRect(rcEvent.left, rcEvent.top + 1, rcEvent.Width(), rcEvent.Height() - 2, m_pPaintManager->m_clrButtonFace);
		pDC->FillSolidRect(rcEvent.left, rcEvent.top - 1, rcEvent.Width()+1, 1, m_pPaintManager->m_clrWindowText);
		pDC->FillSolidRect(rcEvent.left, rcEvent.bottom, rcEvent.Width()+1, 1, m_pPaintManager->m_clrWindowText);
	}

	if (nMDEFlags & xtpCalendarMultiDayFirst)
	{
		pDC->FillSolidRect(rcEvent.left, rcEvent.top, 1, rcEvent.Height(), m_pPaintManager->m_clrWindowText);
		rcEvent.left++;
		if (pViewEvent->GetFirstClockRect().Width() > 0)
		{
			m_pPaintManager->DrawClock(
				pDC,
				pViewEvent->GetEvent()->GetStartTime(),
				pViewEvent->GetFirstClockRect(),
				crBackground,
				xtpCalendarClockAlignLeft);
		}
	}
	if (nMDEFlags & xtpCalendarMultiDayLast)
	{
		pDC->FillSolidRect(rcEvent.right, rcEvent.top, 1, rcEvent.Height(), m_pPaintManager->m_clrWindowText);
		rcEvent.right -= 2;
		if (pViewEvent->GetLastClockRect().Width() > 0)
		{
			m_pPaintManager->DrawClock(
				pDC,
				pViewEvent->GetEvent()->GetEndTime(),
				pViewEvent->GetLastClockRect(),
				crBackground,
				xtpCalendarClockAlignLeft);
		}

	}

	UINT uAlign = 0;
	CRect rcText(pViewEvent->GetTextRect());
	CRect rcTime(rcText);
	BOOL bTimeOutOfBorders = FALSE;

	//  draw busy border
	if (!bMultiDay)
	{
		rcText.top += min(2, rcText.Height());
		rcTime.top = rcText.top;

		int nBusyStatus = pViewEvent->GetEvent()->GetBusyStatus();

		// fill busy area
		CRect rcBusyArea(rcEvent);
		//  draw border
		rcBusyArea.right = rcBusyArea.left + 7;
		//rcText.left = rcBusyArea.right + 3;
		pDC->FillSolidRect(rcBusyArea, m_pPaintManager->m_clrWindowText);
		//  draw background
		rcBusyArea.DeflateRect(1, 1, 1, 0);
		pDC->FillSolidRect(rcBusyArea, m_pPaintManager->m_clrWindow);

		if (pViewEvent->GetTimeframeRect().top == LONG_MAX ||
				pViewEvent->GetTimeframeRect().bottom == LONG_MAX)
		{
			POINT arBusyPoints[] =
			{
				{rcEvent.left, rcEvent.top},
				{rcEvent.left + 7, rcEvent.top},
				{rcEvent.left + 7, rcEvent.top + 7},
				{rcEvent.left, rcEvent.top}
			};
			const int cBPCount = sizeof(arBusyPoints) / sizeof(arBusyPoints[0]);
			CRgn rgnBusy;
			BOOL bRgn = rgnBusy.CreatePolygonRgn(arBusyPoints, cBPCount, ALTERNATE);
			ASSERT(bRgn);

			if (bRgn)
			{
				m_pPaintManager->DrawBusyStatus(pDC, rgnBusy, nBusyStatus);
			}
		}
		else
		{
			//  draw busy border
			rcBusyArea.top = pViewEvent->GetTimeframeRect().top;
			rcBusyArea.bottom = pViewEvent->GetTimeframeRect().bottom;
			pDC->FillSolidRect(rcBusyArea, m_pPaintManager->m_clrWindowText);
			//  draw busy color
			rcBusyArea.DeflateRect(0, 1, 0, 1);

			m_pPaintManager->DrawBusyStatus(pDC, rcBusyArea, nBusyStatus);
		}

		// draw shadow
		if (!bSelected)
			m_pPaintManager->DrawShadow(pDC, pViewEvent->GetEventRect());

		// draw border
		if (bSelected)
		{
			CBrush brushBorder(m_pPaintManager->m_clrWindowText);

			CRect rcBorder2(pViewEvent->GetEventRect());
			rcBorder2.right += 1;

			CRect rcBorderTop(rcBorder2);
			rcBorderTop.bottom = rcBorderTop.top + 1;
			rcBorderTop.top -= 5;

			m_pPaintManager->DrawBusyStatus(pDC, rcBorderTop, nBusyStatus);
			pDC->FrameRect(&rcBorderTop, &brushBorder);

			CRect rcBorderBottom(rcBorder2);
			rcBorderBottom.top = rcBorderBottom.bottom;
			rcBorderBottom.bottom += 6;

			m_pPaintManager->DrawBusyStatus(pDC, rcBorderBottom, nBusyStatus);
			pDC->FrameRect(&rcBorderBottom, &brushBorder);
		}
		rcText.right -= 4;

		uAlign |= DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL;

		//---------------------------------------------------------------------------
		if (!strTime.IsEmpty())
		{
			CSize szTime = GetTextExtent(pDC, strTime);
			if (szTime.cx <= rcText.Width())
			{
				strText = strTime + _T(" ") + strText;
				strTime.Empty();
			}
			else
			{
				bTimeOutOfBorders = TRUE;
				rcTime.bottom = rcTime.top + szTime.cy;
				rcText.top = rcTime.bottom;
			}
		}
	}
	else
	{
		rcText.left += 3;
		CSize szText = GetTextExtent(pDC, strText);
		if (pViewEvent->IsMultyDayTextAlign() || szText.cx + 2 > rcText.Width())
		{
			uAlign |= DT_LEFT;
		}
		else
		{
			uAlign |= DT_CENTER;
		}
		uAlign |= DT_SINGLELINE;
	}

	m_pPaintManager->DrawIcons(pDC, pViewEvent);

	// draw event time, if need
	if (!strTime.IsEmpty())
	{
		DrawText(pDC, strTime, rcTime, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	// draw event text
	DrawText(pDC, strText, rcText, uAlign | DT_VCENTER);

	// update OutOfBorders flag

	int nX_need = 0, nY_need = 0;
	if (uAlign & DT_SINGLELINE)
	{
		CSize szText = GetTextExtent(pDC, strText);
		nX_need = szText.cx;
	}
	else
	{
		CRect rcNeeded(0, 0, rcText.Width(), 0);
		DrawText(pDC, strText, rcNeeded, DT_CALCRECT | DT_WORDBREAK);
		nY_need = rcNeeded.Height();
	}

	BOOL bOutOfBorders = nY_need > rcText.Height() ||
						 nX_need > rcText.Width() ||
						 bTimeOutOfBorders;
	pViewEvent->SetTextOutOfBorders(bOutOfBorders);
}

AFX_INLINE int CXTPCalendarPaintManager::CheckValue(int iValue)
{
	return ((iValue > 255) ? 255 : ((iValue < 0) ? 0 : iValue));
}

AFX_INLINE COLORREF CXTPCalendarPaintManager::AlphaPixel(const COLORREF crPixel, const UINT i)
{
	return RGB(
			   CheckValue(GetRValue(crPixel)-i),
			   CheckValue(GetGValue(crPixel)-i),
			   CheckValue(GetBValue(crPixel)-i));
}

void CXTPCalendarPaintManager::DrawShadow(CDC* pDC, const CRect& rcRect)
{
	ASSERT(pDC);
	if (!pDC || pDC->IsPrinting())
	{
		return;
	}

	CRect rcVShadow(rcRect);
	rcVShadow.left = rcVShadow.right;
	rcVShadow.right += 5;
	rcVShadow.top += 5;
	DrawShadowRectPart(pDC, rcVShadow, xtpShadowLR);

	CRect rcHShadow(rcRect);
	rcHShadow.left += 7;
	rcHShadow.top = rcHShadow.bottom + 1;
	rcHShadow.bottom += 6;
	DrawShadowRectPart(pDC, rcHShadow, xtpShadowTB);

	CRect rcTRShadow(rcVShadow);
	rcTRShadow.bottom = rcTRShadow.top;
	rcTRShadow.top -= 5;
	DrawShadowRectPart(pDC, rcTRShadow, xtpShadowBR);

	CRect rcBRShadow(rcVShadow);
	rcBRShadow.top = rcBRShadow.bottom;
	rcBRShadow.bottom += 5;
	DrawShadowRectPart(pDC, rcBRShadow, xtpShadowTR);

	CRect rcBLShadow(rcRect);
	rcBLShadow.right = rcBLShadow.left + 7;
	rcBLShadow.top = rcBLShadow.bottom + 1;
	rcBLShadow.bottom += 6;
	DrawShadowRectPart(pDC, rcBLShadow, xtpShadowBL);
}

void CXTPCalendarPaintManager::DrawShadowRectPart(CDC* pDC, const CRect& rcShadow, const XTPShadowPart part)
{
	ASSERT(pDC);
	if (!pDC || !pDC->GetSafeHdc())
	{
		return;
	}

	// start processing
	HDC hdc;               // handle of the DC we will create
	HBITMAP hbitmap;       // bitmap handle
	BITMAPINFO bmi;        // bitmap header
	ULONG   ulBitmapWidth, ulBitmapHeight;      // bitmap width/height
	VOID *pvBits;          // pointer to DIB section

	// create a DC for our bitmap -- the source DC for AlphaBlend
	hdc = ::CreateCompatibleDC(pDC->GetSafeHdc());
	if (!hdc)
		return;

	// setup bitmap info
	// zero the memory for the bitmap info
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	// set the bitmap width and height. Later on, the blending will occur in the center of each of the three areas.
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = ulBitmapWidth = max(rcShadow.Width(), 0);
	bmi.bmiHeader.biHeight = ulBitmapHeight = max(rcShadow.Height(), 0);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = ulBitmapWidth * ulBitmapHeight * 4;
	// create our DIB section and select the bitmap into the dc
	if ((ulBitmapWidth > 0) && (ulBitmapHeight > 0))
	{
		hbitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
		if (hbitmap)
		{
			HGDIOBJ hOld = SelectObject(hdc, hbitmap);
			// Copy old picture values to the created bitmap
			if (::BitBlt(hdc, 0, 0, ulBitmapWidth, ulBitmapHeight,
				pDC->GetSafeHdc(), rcShadow.left, rcShadow.top, SRCCOPY))
			{

				// Adjust all pixels of the picture
				switch (part)
				{
				case xtpShadowLR:
					ApplyShadowLR((UINT*)pvBits, ulBitmapWidth, ulBitmapHeight);
					break;
				case xtpShadowTB:
					ApplyShadowTB((UINT*)pvBits, ulBitmapWidth, ulBitmapHeight);
					break;
				case xtpShadowTR:
					ApplyShadowTR((UINT*)pvBits, ulBitmapWidth, ulBitmapHeight);
					break;
				case xtpShadowBR:
					ApplyShadowBR((UINT*)pvBits, ulBitmapWidth, ulBitmapHeight);
					break;
				case xtpShadowBL:
					ApplyShadowBL((UINT*)pvBits, ulBitmapWidth, ulBitmapHeight);
					break;
				default:
					break;
				}

				// Copy modified picture to the old DC
				if (pvBits)
				{
					pDC->BitBlt(rcShadow.left, rcShadow.top, ulBitmapWidth, ulBitmapHeight, CDC::FromHandle(hdc), 0, 0, SRCCOPY);
				}
			}
			// do cleanup
			SelectObject(hdc, hOld);
			DeleteObject(hbitmap);
		}
	}
	// do cleanup
	DeleteDC(hdc);
}

void CXTPCalendarPaintManager::ApplyShadowLR(UINT* pBitmap, const ULONG ulBitmapWidth, const ULONG ulBitmapHeight)
{
	if (!pBitmap)
		return;

	UINT nAlpha = 0;
	const double dPercentsPerPixel = dXTPMaxAlpha / (double)ulBitmapWidth;
	double dAlpha = dXTPMaxAlpha;

	for (ULONG x = 0; x < ulBitmapWidth; x++)
	{
		nAlpha = (UINT)dAlpha;
		if (nAlpha != 0)
		{
			for (ULONG y = 0; y < ulBitmapHeight; y++)
			{
				UINT uiPixel = pBitmap[x + y * ulBitmapWidth];
				pBitmap[x + y * ulBitmapWidth] = AlphaPixel(uiPixel, nAlpha);
			}
		}
		dAlpha -= dPercentsPerPixel;
	}
}

void CXTPCalendarPaintManager::ApplyShadowTB(UINT* pBitmap, const ULONG ulBitmapWidth, const ULONG ulBitmapHeight)
{
	if (!pBitmap)
		return;

	UINT nAlpha = 0;
	const double dPercentsPerPixel = dXTPMaxAlpha / (double)ulBitmapHeight;
	double dAlpha = 0;

	for (ULONG y = 0; y < ulBitmapHeight; y++)
	{
		nAlpha = (UINT)(dAlpha);
		if (nAlpha != 0)
		{
			for (ULONG x = 0; x < ulBitmapWidth; x++)
			{
				UINT uiPixel = pBitmap[x + y * ulBitmapWidth];
				pBitmap[x + y * ulBitmapWidth] = AlphaPixel(uiPixel, nAlpha);
			}
		}
		dAlpha += dPercentsPerPixel;
	}
}

void CXTPCalendarPaintManager::ApplyShadowTR(UINT* pBitmap, const ULONG ulBitmapWidth, const ULONG ulBitmapHeight)
{
	if (!pBitmap)
		return;

	UINT nAlpha = 0;
	const double dPercentsPerPixel = dXTPMaxAlpha / (double)min(ulBitmapWidth, ulBitmapHeight);

	for (ULONG y = 0; y < ulBitmapHeight; y++)
	{
		for (ULONG x = 0; x < ulBitmapWidth; x++)
		{
			nAlpha = (UINT)(dXTPMaxAlpha - sqrt((double)(ulBitmapHeight - y)*(x)) * dPercentsPerPixel);
			nAlpha = min(nAlpha, (UINT)((ulBitmapWidth - x) * dPercentsPerPixel));
			nAlpha = min(nAlpha, (UINT)((y + 1) * dPercentsPerPixel));
			if (nAlpha != 0)
			{
				UINT uiPixel = pBitmap[x + y * ulBitmapWidth];
				pBitmap[x + y * ulBitmapWidth] = AlphaPixel(uiPixel, nAlpha);
			}
		}
	}
}

void CXTPCalendarPaintManager::ApplyShadowBR(UINT* pBitmap, const ULONG ulBitmapWidth, const ULONG ulBitmapHeight)
{
	if (!pBitmap)
		return;

	UINT nAlpha = 0;
	const double dPercentsPerPixel = dXTPMaxAlpha / (double)min(ulBitmapHeight, ulBitmapWidth);

	for (ULONG y = 0; y < ulBitmapHeight; y++)
	{
		for (ULONG x = 0; x < ulBitmapWidth; x++)
		{
			nAlpha = (UINT)(dXTPMaxAlpha - sqrt((double)y * x) * dPercentsPerPixel);
			nAlpha = min(nAlpha, (UINT)((ulBitmapWidth - x) * dPercentsPerPixel));
			nAlpha = min(nAlpha, (UINT)((ulBitmapHeight - y) * dPercentsPerPixel));
			if (nAlpha != 0)
			{
				UINT uiPixel = pBitmap[x + y * ulBitmapWidth];
				pBitmap[x + y * ulBitmapWidth] = AlphaPixel(uiPixel, nAlpha);
			}
		}
	}
}

void CXTPCalendarPaintManager::ApplyShadowBL(UINT* pBitmap, const ULONG ulBitmapWidth, const ULONG ulBitmapHeight)
{
	if (!pBitmap)
		return;

	UINT nAlpha = 0;
	const double dPercentsPerPixel = dXTPMaxAlpha / (double)min(ulBitmapHeight, ulBitmapWidth);

	for (ULONG y = 0; y < ulBitmapHeight; y++)
	{
		for (ULONG x = 0; x < ulBitmapWidth; x++)
		{
			nAlpha = (UINT)(dXTPMaxAlpha - sqrt((double)(ulBitmapHeight - y)*(ulBitmapWidth - x)) * dPercentsPerPixel);
			nAlpha = min(nAlpha, (UINT)(x * dPercentsPerPixel));
			nAlpha = min(nAlpha, (UINT)((y) * dPercentsPerPixel));
			if (nAlpha != 0)
			{
				UINT uiPixel = pBitmap[x + y * ulBitmapWidth];
				pBitmap[x + y * ulBitmapWidth] = AlphaPixel(uiPixel, nAlpha);
			}
		}
	}
}

void CXTPCalendarPaintManager::CDayViewTimeScaleCellPart::RefreshMetrics()
{
	m_nHourWidth = m_nMinWidth = 0;
	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));
	STRCPY_S(lfIcon.lfFaceName, LF_FACESIZE, _T("Tahoma"));
	m_fntTimeText.SetStandardValue(&lfIcon);
}

void CXTPCalendarPaintManager::CDayViewTimeScaleCellPart::CalcWidth(CDC* pDC,
					const CString& strHour, const CString& strMin,
					int nHourHeight, int& nWidth)
{
	ASSERT(pDC);
	if (!pDC || !m_pPaintManager)
		return;

	CSize szMin(0, 0), szHour(0, 0);

	{
		CXTPFontDC fnt(pDC, &GetTextFont());
		szMin = pDC->GetTextExtent(strMin);
	}

	{
		CRect rcDefaultCell(0, 0, 100, nHourHeight);
		AdjustTimeFont(pDC, rcDefaultCell);

		CXTPFontDC fnt(pDC, m_fntTimeText);
		szHour = pDC->GetTextExtent(strHour);
		// check also widest hours number
		CSize szHour2 = pDC->GetTextExtent(_T("88"), 2);
		if (szHour2.cx > szHour.cx)
		{
			szHour = szHour2;
		}
	}

	m_nHourWidth = szHour.cx;
	m_nMinWidth = szMin.cx;
	nWidth = max(42, szHour.cx + szMin.cx + 17);
}

void CXTPCalendarPaintManager::CDayViewTimeScaleCellPart::AdjustTimeFont(CDC* pDC, CRect rcCell)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}
	rcCell.DeflateRect(5, 6, 1, 7);

	LOGFONT lfIcon;
	((CFont*)m_fntTimeText)->GetLogFont(&lfIcon);
	CSize szText;
	{
		CXTPFontDC fontBig(pDC, m_fntTimeText);
		szText = pDC->GetTextExtent(_T("88"), 2);
	}

	int nParam1 = rcCell.Width() * szText.cy;
	int nParam2 = rcCell.Height() * szText.cx;
	if (nParam1 > nParam2)
	{
		lfIcon.lfHeight = ::MulDiv(lfIcon.lfHeight, rcCell.Height(), szText.cy);
	}
	else
	{
		lfIcon.lfHeight = ::MulDiv(lfIcon.lfHeight, rcCell.Width(), szText.cx);
	}

	m_fntTimeText.SetStandardValue(&lfIcon);
}

void CXTPCalendarPaintManager::CDayViewTimeScaleCellPart::DrawHourCell(CDC* pDC, CRect rc, CString strText, BOOL bFillBackground)
{
	if (!pDC || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	if (bFillBackground)
		pDC->FillSolidRect(rc, GetBackgroundColor());
	pDC->FillSolidRect(rc.right - 1, rc.top, 1, rc.Height(), m_pPaintManager->m_clr3DShadow);

	rc.DeflateRect(0, 0, 4, 0);

	DrawText(pDC, strText, rc, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

	rc.DeflateRect(5, 0, 0, 0);
	pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, m_pPaintManager->m_clr3DShadow);
}

void CXTPCalendarPaintManager::CDayViewTimeScaleCellPart::DrawBigHourCell(CDC* pDC, CRect rc, CString strHour, CString strMin, int nRowPerHour, BOOL bFillBackground)
{
	if (!pDC || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	// Fill background
	if (bFillBackground)
		pDC->FillSolidRect(rc, GetBackgroundColor());
	pDC->FillSolidRect(rc.right - 1, rc.top, 1, rc.Height(), m_pPaintManager->m_clr3DShadow);

	// Calculate coordinates
	CRect rcMinutesText(rc);
	rcMinutesText.bottom = rcMinutesText.top + rc.Height() / nRowPerHour;
	rcMinutesText.left += m_nHourWidth + 10;

	CRect rcHoursText(rc);
	rcHoursText.right = rcMinutesText.left;
	rcHoursText.DeflateRect(0, 4, 4, 7);

	// Draw hours text
	CXTPFontDC fontBig(pDC, m_fntTimeText, GetTextColor());
	pDC->DrawText(strHour, rcHoursText, DT_TOP | DT_RIGHT | DT_SINGLELINE | DT_NOCLIP);

	// draw minutes text
	DrawText(pDC, strMin, rcMinutesText, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);

	// draw small lines
	if (nRowPerHour > 2)
	{
		BOOL bShowMinutes = XTP_SAFE_GET3(m_pPaintManager, m_pControl, GetCalendarOptions(), bDayView_TimeScaleShowMinutes, FALSE);
		CString strMinEx;

		for (int i = 1; i < nRowPerHour; i++)
		{
			int y = rc.top + i * rc.Height() / nRowPerHour;
			pDC->FillSolidRect(rcMinutesText.left, y - 1, rcMinutesText.Width(), 1, m_pPaintManager->m_clr3DShadow);

			//------------------------------------------
			if (bShowMinutes)
			{
				int nMinEx = 60 / nRowPerHour * i;
				strMinEx.Format(_T("%02d"), nMinEx);

				CRect rcMinEx = rcMinutesText;
				rcMinEx.top = y;
				rcMinEx.bottom = y + rc.Height();

				DrawText(pDC, strMinEx, rcMinEx, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
			}
		}
	}

	// draw line below
	rc.DeflateRect(5, 0, 4, 0);
	pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, m_pPaintManager->m_clr3DShadow);
}

void CXTPCalendarPaintManager::CDayViewTimeScaleHeaderPart::OnDraw(CDC* pDC, CXTPCalendarDayView* /*pView*/, CRect rc, CString strText)
{
	if (!pDC || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->FillSolidRect(rc, GetBackgroundColor());
	pDC->FillSolidRect(rc.right - 1, rc.top, 1, rc.Height(), m_pPaintManager->m_clr3DShadow);

	rc.DeflateRect(1, 0, 4, 0);
	pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, m_pPaintManager->m_clr3DShadow);

	rc.DeflateRect(1, 2);
	DrawText(pDC, strText, rc, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
}

void CXTPCalendarPaintManager::CDayViewTimeScaleHeaderPart::DrawNowLine(CDC* pDC,
					CXTPCalendarDayView* pView, CRect rc, int y, BOOL bDrawBk)
{
	if (!pDC || !pView || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	int nHeight = GETTOTAL_MINUTES_DTS(pView->GetScaleInterval()) >= 60 ? 5 : 10;

	CRect rcNowLine = rc;
	rcNowLine.top = y - nHeight;
	rcNowLine.bottom = y + 1;
	rcNowLine.left += 5;
	rcNowLine.right -= 1;

	BOOL bXPTheme = FALSE;
	switch (m_pPaintManager->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
	case xtpSystemThemeOlive:
	case xtpSystemThemeSilver:
		bXPTheme = TRUE;
		break;
	}

	if (bDrawBk)
	{
		if (bXPTheme)
		{
			XTPDrawHelpers()->GradientFill(pDC, &rcNowLine, m_pPaintManager->m_grclrToday, FALSE);
		}
	}
	else
	{
		rcNowLine.top = rcNowLine.bottom - 1;
		pDC->FillSolidRect(&rcNowLine, m_pPaintManager->m_clrUnderLineHdr);
	}
}

void CXTPCalendarPaintManager::CDayViewTimeScaleHeaderPart::DrawExpandSigns(CDC* pDC,
					CXTPCalendarDayView* pView, const CRect& rcTSHours)
{
	if (!pDC || !pView || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	// draw expand signs
	CRect rcSignUp(rcTSHours);
	CRect rcSignDown(rcTSHours);
	CSize szSign(m_pPaintManager->GetExpandSignSize());

	rcSignUp.left = rcTSHours.right - szSign.cx;

	rcSignDown.left = rcTSHours.right - szSign.cx;
	rcSignDown.top = rcSignDown.bottom - szSign.cy;

	CImageList* pilExpSgn = &m_pPaintManager->m_ilExpandSigns;
	if (pView->IsExpandUp())
	{
		m_pPaintManager->DrawBitmap(pilExpSgn, pDC, rcSignUp,
									CXTPCalendarPaintManager::idxExpandSignUp);
	}
	if (pView->IsExpandDown())
	{
		m_pPaintManager->DrawBitmap(pilExpSgn, pDC, rcSignDown,
									CXTPCalendarPaintManager::idxExpandSignDown);
	}

}

void CXTPCalendarPaintManager::CDayViewCellPart::GetParams(CXTPCalendarDayViewGroup* pViewGroup,
														   XTP_CALENDAR_DAYVIEWCELL_PARAMS& rCellParams)
{
	if (!pViewGroup || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	rCellParams.clrBackgroundColor =  rCellParams.bSelected ? m_pPaintManager->m_clrHighlight : GetBackgroundColor();
}

void CXTPCalendarPaintManager::CDayViewCellPart::OnDraw(CDC* pDC,
									CXTPCalendarDayViewGroup* pViewGroup, CRect rc,
									const XTP_CALENDAR_DAYVIEWCELL_PARAMS& cellParams)
{
	if (!pDC || !pViewGroup || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}
	COleDateTime dtTime = cellParams.dtBeginTime;

	CRect rcBusyStatus(rc);
	rcBusyStatus.right = rcBusyStatus.left + 6;

	pDC->FillSolidRect(&rcBusyStatus, m_pPaintManager->m_clrWindow);
	int nDayBusyStatus = pViewGroup->GetBusyStatus(dtTime);
	if (nDayBusyStatus != xtpCalendarBusyStatusUnknown)
	{
		m_pPaintManager->DrawBusyStatus(pDC, rcBusyStatus, nDayBusyStatus);
	}

	if (XTP_SAFE_GET2(pViewGroup, GetViewDay(), GetDayIndex(), 0) != 0 ||
		pViewGroup->GetGroupIndex() != 0)
	{
		pDC->FillSolidRect(rc.left, rc.top, 1, rc.Height(), m_pPaintManager->m_clrWindowText);
	}
	int nDayDeviderX = 0;//pViewGroup->GetGroupIndex() ? -2 : 0;
	pDC->FillSolidRect(rc.left + 6 + nDayDeviderX, rc.top, 1, rc.Height(), m_pPaintManager->m_clrWindowText);

	rc.left += 7 + nDayDeviderX;

	pDC->FillSolidRect(rc, cellParams.clrBackgroundColor);

	if (dtTime.GetMinute() == 0 && dtTime.GetHour() != 0)
	{
		pDC->FillSolidRect(rc.left, rc.top - 1, rc.Width(), 1, m_clrHour);
	}

	pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, m_clrShadow);
}

void CXTPCalendarPaintManager::CDayViewAllDayEventsPart::OnDraw(CDC* pDC,
				CXTPCalendarDayViewGroup* pViewGroup, CRect rc, BOOL bSelected)
{
	if (!pDC || !pViewGroup || !pViewGroup->GetViewDay() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	if (bSelected)
	{
		pDC->FillSolidRect(rc, m_pPaintManager->m_clrWindow);
		pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_pPaintManager->m_clr3DShadow);
		pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, m_pPaintManager->m_clr3DShadow);
	}
	else
	{
		pDC->FillSolidRect(rc, GetBackgroundColor());
	}

	if (pViewGroup->GetViewDay()->GetDayIndex() || pViewGroup->GetGroupIndex())
	{
		pDC->FillSolidRect(rc.left, rc.top + 1, 1, rc.Height() - 2, 0);
	}
}

void CXTPCalendarPaintManager::CDayViewHeaderPart::OnDraw(CDC* pDC,
					CXTPCalendarDayViewDay* pViewDay, CRect rc, CString strText)
{
	if (!pDC || !pViewDay || !pViewDay->GetView() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bIsCurrent = FALSE;
	COleDateTime dtCurrent =  CXTPCalendarUtils::GetCurrentTime();
	dtCurrent = CXTPCalendarUtils::ResetTime(dtCurrent);

	COleDateTime dtDay;

	bIsCurrent = pViewDay->GetDayDate() == dtCurrent;

	pDC->FillSolidRect(rc, GetBackgroundColor());

	CRect rcHeader(rc);

	m_pPaintManager->DrawHeader(pDC, rcHeader, FALSE, bIsCurrent);

	BOOL bFirstDay = pViewDay->GetDayIndex() == 0;
	BOOL bLastDay = pViewDay->GetDayIndex() == pViewDay->GetView()->GetViewDayCount() - 1;

	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_clrTopLeftBorder);

	pDC->FillSolidRect(rc.left, rc.top + (bFirstDay ? 0: 2), 1, rc.Height() -  (bFirstDay ? 0 : 3), m_clrTopLeftBorder);
	pDC->FillSolidRect(rc.right - 1, rc.top + (bLastDay ? 0: 2), 1, rc.Height() - (bLastDay ? 0 : 3), m_pPaintManager->m_clr3DShadow);

	rc.DeflateRect(2, 0, 2, 0);
	DrawLine_CenterLR(pDC, strText, rc, DT_VCENTER);
}

void CXTPCalendarPaintManager::CDayViewGroupHeaderPart::OnDraw(CDC* pDC,
				CXTPCalendarDayViewGroup* pViewGroup, CRect rc, CString strText)
{
	if (!pDC || !pViewGroup || !pViewGroup->GetViewDay() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->FillSolidRect(rc, GetBackgroundColor());

	CRect rcHeader(rc);

	m_pPaintManager->DrawHeader(pDC, rcHeader, FALSE, FALSE);

	CXTPCalendarDayViewDay* pViewDay = pViewGroup->GetViewDay();

	BOOL bFirstGroup = pViewDay->GetDayIndex() == 0 &&
					   pViewGroup->GetGroupIndex() == 0;
	BOOL bLastGroup = pViewDay->GetDayIndex() == pViewDay->GetView()->GetViewDayCount() - 1 &&
			pViewGroup->GetGroupIndex() == pViewDay->GetViewGroupsCount() - 1;          ;

	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_clrTopLeftBorder);

	pDC->FillSolidRect(rc.left, rc.top + (bFirstGroup ? 0: 2), 1, rc.Height() -  (bFirstGroup ? 0 : 3), m_clrTopLeftBorder);
	pDC->FillSolidRect(rc.right - 1, rc.top + (bLastGroup ? 0: 2), 1, rc.Height() - (bLastGroup ? 0 : 3), m_pPaintManager->m_clr3DShadow);

	rc.DeflateRect(2, 0, 2, 0);

	DrawLine_CenterLR(pDC, strText, rc, DT_VCENTER);
}

void CXTPCalendarPaintManager::DrawHeader(CDC* pDC, CRect& rcHeader,
		BOOL bIsSelected, BOOL bIsCurrent)
{
	DrawHeader(pDC, rcHeader, bIsSelected, bIsCurrent, m_grclrToday,
					  m_clrUnderLineHdr);
}

void CXTPCalendarPaintManager::DrawHeader(CDC* pDC, CRect& rcHeader,
		BOOL bIsSelected, BOOL bIsCurrent, const CXTPPaintManagerColorGradient& grclr, COLORREF clrUnderLineHdr)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}

	if (!bIsSelected)
		pDC->FillSolidRect(rcHeader, m_clrButtonFace);

	if (bIsCurrent)
	{
		if (grclr.clrDark != grclr.clrLight)
		{
			XTPDrawHelpers()->GradientFill(pDC, &rcHeader, grclr, FALSE);
		}
		else
		{
			pDC->FillSolidRect(rcHeader, grclr.clrDark);
		}
		pDC->FillSolidRect(rcHeader.left, rcHeader.bottom -1 ,
			rcHeader.Width(), 1, clrUnderLineHdr);
	}

	if (bIsSelected)
	{
		pDC->FillSolidRect(rcHeader, m_clrHighlight);
	}
}


void CXTPCalendarPaintManager::CWeekViewPart::DrawDayHeader(CDC* pDC,
						CRect rcDay, int nHeaderHeight,
						CString strHeader, BOOL bIsCurrent, BOOL bIsSelected)
{
	if (!pDC || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcHeader(rcDay);

	// draw header top line
	m_pPaintManager->DrawHorizontalLine(pDC, rcDay.left + 1, rcDay.top,
					 rcDay.Width()+1);

	rcHeader.DeflateRect(2, 2, 1, rcHeader.Height() - nHeaderHeight - 1);
	CRect rcText(rcHeader);
	rcText.DeflateRect(1, 0, 3, 1);

	// draw header contents
	CRect rcHeader2(rcHeader);
	rcHeader2.bottom -= 1;
	if (bIsCurrent && bIsSelected)
	{
		m_pPaintManager->DrawHeader(pDC, rcHeader2, FALSE, bIsCurrent);
		CRect rcSel = CRect(rcHeader2);
		CSize  szText = GetTextExtent(pDC, strHeader);
		rcSel.left = rcSel.right - (szText.cx + 7);
		rcSel.bottom--;
		m_pPaintManager->DrawHeader(pDC, rcSel, bIsSelected, FALSE);
	}
	else
	{
		m_pPaintManager->DrawHeader(pDC, rcHeader2, bIsSelected, bIsCurrent);
	}

	// draw header bottom line
	if (!bIsCurrent)
	{
		CRect rcBottomLine(rcHeader);
		rcBottomLine.top = --rcBottomLine.bottom - 1;
		if (!bIsSelected)
			rcBottomLine.DeflateRect(2, 0, 2, 0);
		pDC->FillSolidRect(rcBottomLine, m_clrHeaderBottomLine);
	}

	// draw header text
	pDC->SetBkMode(TRANSPARENT);
	SetTextColor(bIsSelected ? m_clrTextHighLightColor : m_clrTextNormalColor);
	DrawText(pDC, strHeader, rcText, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

}

void CXTPCalendarPaintManager::CWeekViewPart::OnDraw(CDC* pDC, CXTPCalendarWeekView* pWeekView)
{
	if (!pDC || !pWeekView || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcView(pWeekView->GetViewRect());

	pDC->FillSolidRect(rcView, m_clrBackground);

	//draw grid
	int nDayWidth = pWeekView->GetDayWidth();

	m_pPaintManager->DrawVerticalLine(pDC, rcView.left + nDayWidth - 1, rcView.top, rcView.bottom);

	// draw headers
	BOOL bIsSelected = FALSE;
	BOOL bIsCurrent = FALSE;
	COleDateTime dtCurrent = CXTPCalendarUtils::ResetTime(COleDateTime::GetCurrentTime());

	CString strHeaderFormat = pWeekView->GetDayHeaderFormat();

	SYSTEMTIME st;
	COleDateTime dtDay;

	int nDayHeaderHeight = pWeekView->GetDayHeaderHeight();
	for (int nDay = 0; nDay < 7; nDay++)
	{
		CXTPCalendarWeekViewDay* pWDay = pWeekView->GetViewDay(nDay);
		ASSERT(pWDay);
		if (!pWDay)
		{
			continue;
		}
		dtDay = pWDay->GetDayDate();
		bIsSelected = pWDay->IsSelected();
		bIsCurrent = (dtDay == dtCurrent);

		GETASSYSTEMTIME_DT(dtDay, st);
		CString strDate = CXTPCalendarUtils::GetDateFormat(&st, strHeaderFormat);

		//-------------------------------------------------------------
		DWORD dwFlags = m_pPaintManager->GetAskItemTextFlags();

		if (dwFlags & xtpCalendarItemText_WeekViewDayHeader)
		{
			XTP_CALENDAR_GETITEMTEXT_PARAMS objRequest;
			::ZeroMemory(&objRequest, sizeof(objRequest));

			objRequest.nItem = (int)xtpCalendarItemText_WeekViewDayHeader;
			objRequest.pstrText = &strDate;
			objRequest.pViewDay = pWDay;

			XTP_SAFE_CALL2(pWeekView, GetCalendarControl(), SendNotificationAlways(
							XTP_NC_CALENDAR_GETITEMTEXT, (WPARAM)&objRequest, 0));
		}
		//-------------------------------------------------------------

		DrawDayHeader(pDC, pWDay->GetDayRect(), nDayHeaderHeight,
					  strDate, bIsCurrent, bIsSelected);
	}
}
void CXTPCalendarPaintManager::CWeekViewEventPart::DrawBorder(CDC* pDC,
						CRect rcView, CXTPCalendarWeekViewEvent* pWeekViewEvent)
{
	if (!pDC || !pWeekViewEvent || !pWeekViewEvent->GetEvent() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	int nBorderWidth = pWeekViewEvent->IsSelected() ? 2 : 1;

	int nMDEFlags = pWeekViewEvent->GetMultiDayEventFlags();
	BOOL bAllDay = pWeekViewEvent->GetEvent()->IsAllDayEvent();

	if (pWeekViewEvent->IsSelected() &&
			(nMDEFlags & xtpCalendarMultiDayFirst ||
			 nMDEFlags & xtpCalendarMultiDayLast ||
			 nMDEFlags & xtpCalendarMultiDayMiddle)
	 )
	{
		rcView.top--;
		rcView.bottom++;
	}

	if (bAllDay ||
			nMDEFlags & xtpCalendarMultiDayFirst ||
			nMDEFlags & xtpCalendarMultiDayLast ||
			nMDEFlags & xtpCalendarMultiDayMiddle)
	{
		pDC->FillSolidRect(rcView.left, rcView.top , rcView.Width(), nBorderWidth, m_clrTextNormalColor);
		pDC->FillSolidRect(rcView.left, rcView.bottom - nBorderWidth, rcView.Width(), nBorderWidth, m_clrTextNormalColor);
	}
	if (nMDEFlags & xtpCalendarMultiDayFirst)
	{
		pDC->FillSolidRect(rcView.left, rcView.top, 1, rcView.Height(), m_clrTextNormalColor);
	}
	if (nMDEFlags & xtpCalendarMultiDayLast)
	{
		pDC->FillSolidRect(rcView.right, rcView.top, 1, rcView.Height(), m_clrTextNormalColor);
	}
}

int CXTPCalendarPaintManager::CWeekViewEventPart::DrawTimes(CDC* pDC, CXTPCalendarWeekViewEvent* pWeekViewEvent)
{
	if (!pDC || !pWeekViewEvent || !pWeekViewEvent->GetEvent() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return 0;
	}

	COLORREF clrBackground = pWeekViewEvent->IsSelected() ? m_pPaintManager->m_clrHighlight : m_clrBackground;

	CXTPCalendarEventLabel* ptrLabel = pWeekViewEvent->GetEvent()->GetLabel();
	COLORREF clrLabel = ptrLabel ? ptrLabel->m_clrColor : clrBackground;
	clrBackground = clrLabel;

	CRect rcStart(pWeekViewEvent->GetStartTimeRect());
	CRect rcEnd(pWeekViewEvent->GetEndTimeRect());
	COleDateTime dtStart(pWeekViewEvent->GetEvent()->GetStartTime());

	if (pWeekViewEvent->IsTimeAsClock())
	{
		m_pPaintManager->DrawClock(pDC, dtStart, rcStart, clrBackground, xtpCalendarClockAlignCenter);
	}
	else
	{
		CString strStartTime = pWeekViewEvent->FormatEventTime(dtStart);//.Format(_T("%I:%M%p"));
		DrawText(pDC, strStartTime, &rcStart, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}

	if (pWeekViewEvent->IsShowEndTime())
	{
		COleDateTime dtEnd(pWeekViewEvent->GetEvent()->GetEndTime());
		if (pWeekViewEvent->IsTimeAsClock())
		{
			m_pPaintManager->DrawClock(pDC, dtEnd, rcEnd, clrBackground, xtpCalendarClockAlignCenter);
		}
		else
		{
			CString strEndTime = pWeekViewEvent->FormatEventTime(dtEnd);
			DrawText(pDC, strEndTime, &rcEnd, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
	}
	return 0;
}

void CXTPCalendarPaintManager::CWeekViewEventPart::DrawSubj(CDC* pDC,
									CXTPCalendarWeekViewEvent* pWeekViewEvent)
{
	if (!pDC || !pWeekViewEvent || !pWeekViewEvent->GetEvent() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcText(pWeekViewEvent->GetTextRect());
	CString strLoc = pWeekViewEvent->GetItemTextLocation();
	CString strEventText = pWeekViewEvent->GetItemTextSubject();
	if (strLoc.GetLength() > 0)
	{
		strEventText += _T(" (") + strLoc + _T(")");
	}

	int nDayEventFlags = pWeekViewEvent->GetMultiDayEventFlags();
	int nAlign = DT_CENTER;
	if (pWeekViewEvent->IsMultyDayTextAlign() || GetTextExtent(pDC, strEventText).cx >= rcText.Width())
		nAlign = DT_LEFT;

	//nAlign
	if (pWeekViewEvent->GetEvent()->IsAllDayEvent() ||
			nDayEventFlags & xtpCalendarMultiDayFirst ||
			nDayEventFlags & xtpCalendarMultiDayMiddle ||
			nDayEventFlags & xtpCalendarMultiDayLast
	  )
		DrawText(pDC, strEventText, &rcText, nAlign | DT_VCENTER | DT_SINGLELINE);
	else
		DrawText(pDC, strEventText, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void CXTPCalendarPaintManager::CWeekViewEventPart::OnDraw(CDC* pDC,
									CXTPCalendarWeekViewEvent* pWeekViewEvent)
{
	if (!pDC || !pWeekViewEvent || !pWeekViewEvent->GetEvent() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcView(pWeekViewEvent->GetViewEventRect());
	CRect rcViewMax(rcView);
	CString strEventText;

	BOOL bSelected = pWeekViewEvent->IsSelected();
	int nDayEventFlags = pWeekViewEvent->GetMultiDayEventFlags();
	BOOL bMday = (nDayEventFlags & xtpCalendarMultiDayFMLmask) != 0;
	BOOL bAllDay = pWeekViewEvent->GetEvent()->IsAllDayEvent();

	CXTPCalendarEventLabel* ptrLabel = pWeekViewEvent->GetEvent()->GetLabel();
	BOOL bLabel = (ptrLabel && ptrLabel->m_nLabelID != XTP_CALENDAR_NONE_LABEL_ID);

	COLORREF clrBackground = m_clrBackground;
	SetTextColor(m_clrTextNormalColor);
	if (bMday || bAllDay)
		clrBackground = RGB(255, 255, 255);
	if (bLabel && ptrLabel)
		clrBackground = ptrLabel->m_clrColor;
	if (bSelected)
	{
		rcViewMax.CopyRect(pWeekViewEvent->GetViewEventRectMax());
		clrBackground = m_pPaintManager->m_clrHighlight;
		SetTextColor(m_clrTextHighLightColor);
	}

	if (bMday || bAllDay)
		pDC->FillSolidRect(rcView, clrBackground);
	else
		pDC->FillSolidRect(rcViewMax, clrBackground);

	// Draw border for long event
	DrawBorder(pDC, rcView, pWeekViewEvent);

	// Draw Clock for Long Events
	if (nDayEventFlags & xtpCalendarMultiDayFirst)
	{
		m_pPaintManager->DrawClock(pDC,
								   pWeekViewEvent->GetEvent()->GetStartTime(),
								   pWeekViewEvent->GetFirstClockRect(),
								   clrBackground,
								   xtpCalendarClockAlignLeft);
	}
	else if (nDayEventFlags & xtpCalendarMultiDayLast)
	{
		m_pPaintManager->DrawClock(pDC,
								   pWeekViewEvent->GetEvent()->GetEndTime(),
								   pWeekViewEvent->GetLastClockRect(),
								   clrBackground,
								   xtpCalendarClockAlignRight);
	}

	// 3 Draw time start/end and  icons
	clrBackground = bSelected ? m_pPaintManager->m_clrHighlight : m_clrBackground;
	if (!bMday && !bAllDay)
	{
		DrawTimes(pDC, pWeekViewEvent);
	}
	m_pPaintManager->DrawIcons(pDC, pWeekViewEvent);

	// 5 Draw Text
	DrawSubj(pDC, pWeekViewEvent);
}

void CXTPCalendarPaintManager::CMonthViewGridPart::OnDrawGrid(CDC* pDC,
											CXTPCalendarMonthView* pMonthView)
{
	if (!pDC || !pMonthView || !pMonthView->GetGrid() ||
		!pMonthView->GetCalendarControl() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	COLORREF clrGridLine = RGB(0, 0, 0);
	BOOL bCompressWeD = pMonthView->GetCalendarControl()->MonthView_IsCompressWeekendDays();
	int nWeeksCount = pMonthView->GetGrid()->GetWeeksCount();

	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	int nCurrentMonth = dtNow.GetMonth();
	int nWhiteMonthColorIdx = nCurrentMonth % 2;

	for (int nWeek = 0; nWeek < nWeeksCount; nWeek++)
	{
		for (int nDayIdx = 0; nDayIdx < 7; nDayIdx++)
		{
			COleDateTime dtDay = pMonthView->GetGrid()->GetDayDate(nWeek, nDayIdx);
			CRect rcB = pMonthView->GetGrid()->GetDayRect(nWeek, nDayIdx, TRUE);

			//Draw Grid lines
			if (nWeek > 0)
			{
				pDC->FillSolidRect(rcB.left, rcB.top, rcB.Width(), 1, clrGridLine);
			}
			if (nWeek < nWeeksCount-1)
			{
				pDC->FillSolidRect(rcB.left, rcB.bottom, rcB.Width(), 1, clrGridLine);
			}
			int nWDay = dtDay.GetDayOfWeek();
			BOOL bCompressedCol_Su = bCompressWeD && nDayIdx == 1 && nWDay == 1;
			if (nDayIdx > 0 && !bCompressedCol_Su)
			{
				pDC->FillSolidRect(rcB.left, rcB.top, 1, rcB.Height(), clrGridLine);
			}

			//Fill Grid cells background
			CRect rcDay = pMonthView->GetGrid()->GetDayRect(nWeek, nDayIdx, FALSE);

			int nMonth = dtDay.GetMonth();
			COLORREF clrBackground = nMonth % 2 == nWhiteMonthColorIdx ? m_clrBackground : m_clrBackground2;
			if (nDayIdx == 0)
			{
				rcDay.left--;
			}
			pDC->FillSolidRect(&rcDay, clrBackground);
		}
	}
}

void CXTPCalendarPaintManager::CMonthViewHeaderPart::OnDraw(CDC* pDC,
									CXTPCalendarMonthView* pMonthView, CRect rc,
									int nCollIndex, CString strText)
{
	if (!pDC || !pMonthView || !pMonthView->GetGrid() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->FillSolidRect(rc, GetBackgroundColor());

	BOOL bFirstDay = nCollIndex == 0;
	BOOL bLastDay = nCollIndex == pMonthView->GetGrid()->GetColsCount() - 1;

	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_clrTopLeftBorder);

	pDC->FillSolidRect(rc.left, rc.top + (bFirstDay ? 0: 2), 1, rc.Height() -  (bFirstDay ? 1 : 3), m_clrTopLeftBorder);
	pDC->FillSolidRect(rc.right - 1, rc.top + (bLastDay ? 0: 2), 1, rc.Height() - (bLastDay ? 1 : 3), m_pPaintManager->m_clr3DShadow);

	pDC->FillSolidRect(rc.left, rc.bottom, rc.Width(), 1, m_pPaintManager->m_clr3DShadow);

	rc.DeflateRect(2, 0, 2, 0);

	DrawLine_CenterLR(pDC, strText, rc, DT_VCENTER);
}

void CXTPCalendarPaintManager::CMonthViewEventPart::OnDrawDayDate(CDC* pDC,
											CRect rc, BOOL bToday,
											BOOL bSelected, CString strText)
{
	if (!pDC || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	rc.DeflateRect(1, 1, 1, 1);


	BOOL bXPTheme = FALSE;
	switch (m_pPaintManager->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
	case xtpSystemThemeOlive:
	case xtpSystemThemeSilver:
		bXPTheme = TRUE;
		break;
	}

	if (bSelected && bToday)
	{
		CRect rcSel = CRect(rc);
		CRect rcUnSel = CRect(rc);
		CSize  szText = GetTextExtent(pDC, strText);
		rcSel.left = rcSel.right - (szText.cx + 7);
		rcUnSel.right = rcUnSel.left + rcUnSel.Width() - (szText.cx + 7);
		if (bXPTheme)
			m_pPaintManager->DrawHeader(pDC, rcUnSel, FALSE, bToday);
		else
			m_pPaintManager->DrawHeader(pDC, rcUnSel, FALSE, bToday,
										m_grclrClassicSelDay, m_clrClassicUnderline);

		m_pPaintManager->DrawHeader(pDC, rcSel, bSelected, FALSE);
	}
	else
		if (bSelected || bToday)
		{
			if (bXPTheme)
				m_pPaintManager->DrawHeader(pDC, rc, bSelected, bToday);
			else
				m_pPaintManager->DrawHeader(pDC, rc, bSelected, bToday,
											m_grclrClassicSelDay, m_clrClassicUnderline);

		}

	if (bSelected)
	{
		SetTextColor(m_clrTextHighLightColor);
	}

	rc.DeflateRect(0, 0, 5, 0);
	DrawText(pDC, strText, rc, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

	m_clrTextColor.SetDefaultValue();
}

void CXTPCalendarPaintManager::CMonthViewEventPart::OnDrawEvent(CDC* pDC,
		CXTPCalendarMonthViewEvent* pViewEvent)
{
	if (!pDC || !pViewEvent || !pViewEvent->GetEvent() || !m_pPaintManager)
	{
		ASSERT(FALSE);
		return;
	}

	CRect rcEvent = pViewEvent->GetEventRect();
	CRect rcText = pViewEvent->GetTextRect();
	CXTPCalendarEvent* prtEvent = pViewEvent->GetEvent();

	BOOL bSelected = pViewEvent->IsSelected();
	int nMDEFlags = pViewEvent->GetMultiDayEventFlags();

	CRect rcClockFirst(0, 0, 0, 0);
	CRect rcClockLast(0, 0, 0, 0);
	COleDateTime dtClockFirst;
	COleDateTime dtClockLast;
	COLORREF clrBackground;
	if (nMDEFlags & xtpCalendarMultiDayFirst)
	{
		rcClockFirst.CopyRect(pViewEvent->GetFirstClockRect());
		dtClockFirst = pViewEvent->GetEvent()->GetStartTime();
	}
	if (nMDEFlags & xtpCalendarMultiDayLast)
	{
		rcClockLast.CopyRect(pViewEvent->GetLastClockRect());
		dtClockLast = pViewEvent->GetEvent()->GetEndTime();
	}

	if (bSelected)
	{
		if (nMDEFlags & xtpCalendarMultiDayFMLmask)
		{
			rcEvent.top--;
			rcEvent.bottom++;
		}
		else
		{
			rcEvent.left = pViewEvent->m_rcEventMax.left + 2;
			rcEvent.right = pViewEvent->m_rcEventMax.right - 2;
		}
		SetTextColor(m_clrTextHighLightColor);
		clrBackground = m_pPaintManager->m_clrHighlight;
		pDC->FillSolidRect(&rcEvent, m_pPaintManager->m_clrHighlight);
		clrBackground = m_pPaintManager->m_clrHighlight;
	}
	else
	{
		CXTPCalendarEventLabel* ptrLabel = pViewEvent->GetEvent()->GetLabel();
		COLORREF clrLabel = ptrLabel ? ptrLabel->m_clrColor : RGB(255, 255, 255);
		BOOL bNoneLabel = ptrLabel ? (ptrLabel->m_nLabelID == XTP_CALENDAR_NONE_LABEL_ID) : TRUE;
		clrBackground = clrLabel;

		if (nMDEFlags & xtpCalendarMultiDayFMLmask || !bNoneLabel)
		{
			pDC->FillSolidRect(&rcEvent, clrLabel);
		}
	}

	UINT uAlign = DT_LEFT;

	CString strLoc = pViewEvent->GetItemTextLocation();
	CString strText = pViewEvent->GetItemTextSubject();
	if (strLoc.GetLength() > 0)
	{
		strText += _T(" (") + strLoc + _T(")");
	}

	if (nMDEFlags & xtpCalendarMultiDayFMLmask)
	{
		int nBY = bSelected ? 2 : 1;
		pDC->FillSolidRect(rcEvent.left, rcEvent.top, rcEvent.Width(), nBY, m_clrMultiDayEventFrameColor);
		pDC->FillSolidRect(rcEvent.left, rcEvent.bottom - nBY, rcEvent.Width(), nBY, m_clrMultiDayEventFrameColor);

		if (nMDEFlags & xtpCalendarMultiDayFirst)
		{
			pDC->FillSolidRect(rcEvent.left, rcEvent.top, 1, rcEvent.Height(), m_clrMultiDayEventFrameColor);
		}
		if (nMDEFlags & xtpCalendarMultiDayLast)
		{
			pDC->FillSolidRect(rcEvent.right, rcEvent.top, 1, rcEvent.Height(), m_clrMultiDayEventFrameColor);
		}

		//CSize szSpace = GetTextExtent(pDC, _T(" "));

		rcText.left += 1;//min(1, szSpace.cx/2);
		rcText.right -= 1;
		CSize szText = GetTextExtent(pDC, strText);

		if (pViewEvent->IsMultyDayTextAlign() || szText.cx > rcText.Width())
			uAlign = DT_LEFT;
		else
			uAlign = DT_CENTER;

	}

	// draw clocks for multyday events
	if ((nMDEFlags & xtpCalendarMultiDayFirst) || (nMDEFlags & xtpCalendarMultiDayLast))
	{
		if (rcClockFirst.Width() > 0)
			m_pPaintManager->DrawClock(
				pDC,
				dtClockFirst,
				rcClockFirst,
				clrBackground,
				xtpCalendarClockAlignLeft);

		if (rcClockLast.Width() > 0)
			m_pPaintManager->DrawClock(
				pDC,
				dtClockLast,
				rcClockLast,
				clrBackground,
				xtpCalendarClockAlignLeft);
	}

	//CRect rcText = pViewEvent->GetTextRect();
	if (nMDEFlags & xtpCalendarMultiDayNoMultiDay)
	{
		//IsTimeAsClock()
		COleDateTime dtStart(prtEvent->GetStartTime());
		COleDateTime dtEnd(prtEvent->GetEndTime());
		CString strStartTime = pViewEvent->FormatEventTime(dtStart);
		CString strEndTime = pViewEvent->FormatEventTime(dtEnd);
		CRect rcStart(pViewEvent->GetStartTimeRect());
		CRect rcEnd(pViewEvent->GetEndTimeRect());
		if (pViewEvent->IsTimeAsClock())
		{
			m_pPaintManager->DrawClock(pDC, dtStart, rcStart, clrBackground, xtpCalendarClockAlignCenter);
			m_pPaintManager->DrawClock(pDC, dtEnd, rcEnd, clrBackground, xtpCalendarClockAlignCenter);
		}
		else
		{
			DrawText(pDC, strStartTime, rcStart, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			DrawText(pDC, strEndTime, rcEnd, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}

	}

	DrawText(pDC, strText, rcText, uAlign | DT_VCENTER | DT_SINGLELINE);
	m_pPaintManager->DrawIcons(pDC, pViewEvent);

	m_clrTextColor.SetDefaultValue();
}


/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarPaintManager

#define ADDVIEWPART(theClass, parent)\
	m_p##theClass = (C##theClass*)AddViewPart(new C##theClass(m_p##parent));



CXTPCalendarPaintManager::CXTPCalendarPaintManager()
{
	m_pGlyphsFont = NULL;

	m_szClockRect.cx = 15;
	m_szClockRect.cy = 15;

	m_szExpandSign.cx = m_szExpandSign.cy = 0;

	m_pControl = NULL;

	m_bEnableTheme = TRUE;

	m_dwAskItemTextFlags = 0;

	m_pControlPart = (CControlPart*)AddViewPart(new CControlPart());

	ADDVIEWPART(DayViewTimeScaleHeaderPart, ControlPart);
	ADDVIEWPART(DayViewTimeScaleCellPart, DayViewTimeScaleHeaderPart);
	ADDVIEWPART(DayViewCellPart, ControlPart);
	ADDVIEWPART(DayViewWorkCellPart, DayViewCellPart);
	ADDVIEWPART(DayViewNonworkCellPart, DayViewCellPart);
	ADDVIEWPART(DayViewHeaderPart, ControlPart);
	ADDVIEWPART(DayViewGroupHeaderPart, ControlPart);
	ADDVIEWPART(DayViewAllDayEventsPart, ControlPart);
	ADDVIEWPART(DayViewEventPart, ControlPart);
	ADDVIEWPART(WeekViewPart, ControlPart);
	ADDVIEWPART(WeekViewEventPart, ControlPart);
	ADDVIEWPART(MonthViewGridPart, ControlPart);
	ADDVIEWPART(MonthViewHeaderPart, ControlPart);
	ADDVIEWPART(MonthViewEventPart, ControlPart);

	//-----------------------------------------------------------------------
	CBitmap bmpSignUp, bmpSignDown;

	VERIFY(XTPResourceManager()->LoadBitmap(&bmpSignUp, XTP_IDB_CALENDAR_EXPANDSIGNUP) );
	VERIFY(XTPResourceManager()->LoadBitmap(&bmpSignDown, XTP_IDB_CALENDAR_EXPANDSIGNDOWN) );

	CSize szSexpandSign = GetExpandSignSize();

	VERIFY(m_ilExpandSigns.Create(szSexpandSign.cx, szSexpandSign.cy, ILC_COLOR24 | ILC_MASK, 0, 1));

	COLORREF clrMask = RGB(255, 0, 255);
	m_ilExpandSigns.Add(&bmpSignUp, clrMask);
	m_ilExpandSigns.Add(&bmpSignDown, clrMask);

	//-----------------------------------------------------------------------
	VERIFY(m_ilGlyphs.Create(16, 11, ILC_COLOR24 | ILC_MASK, 0, 1));
	CBitmap bmp;
	VERIFY(XTPResourceManager()->LoadBitmap(&bmp, XTP_IDB_CALENDAR_EVENT_GLYPHS));
	m_ilGlyphs.Add(&bmp, RGB(255, 0, 255));

	RefreshMetrics();
}

CXTPCalendarPaintManager::~CXTPCalendarPaintManager()
{
	while (!m_lstViewParts.IsEmpty())
	{
		delete m_lstViewParts.RemoveHead();
	}

	SAFE_DELETE(m_pGlyphsFont);
}

void CXTPCalendarPaintManager::RefreshMetrics()
{
	RefreshXtremeColors();

	m_clrButtonFace.SetStandardValue(GetSysColor(COLOR_BTNFACE));
	m_clrButtonFaceText.SetStandardValue(GetSysColor(COLOR_BTNTEXT));
	m_clrWindow.SetStandardValue(GetSysColor(COLOR_WINDOW));
	m_clrWindowText.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));
	m_clr3DShadow.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_clrHighlight.SetStandardValue(GetSysColor(COLOR_HIGHLIGHT));

	InitBusyStatusDefaultColors();

	POSITION pos = m_lstViewParts.GetHeadPosition();
	while (pos)
	{
		CXTPCalendarViewPart* pPart = m_lstViewParts.GetNext(pos);
		ASSERT(pPart);
		if (pPart)
		{
			pPart->RefreshMetrics();
		}
	}

	m_grclrToday.SetStandardValue(m_clrButtonFace, m_clrButtonFace);

	m_clrUnderLineHdr = m_clrHighlight;

	m_CurrSystemTheme = xtpSystemThemeUnknown;

	if (m_bEnableTheme && !XTPColorManager()->IsLunaColorsDisabled())
	{
		m_CurrSystemTheme = XTPColorManager()->GetCurrentSystemTheme();
		switch (m_CurrSystemTheme)
		{
		case xtpSystemThemeBlue:
		case xtpSystemThemeOlive:
		case xtpSystemThemeSilver:
			m_grclrToday.SetStandardValue(m_clrButtonFace, RGB(250, 203, 91));
			m_clrUnderLineHdr = RGB(187, 85, 3);
			break;
		}
	}

	//-----------------------------------------------------------------------
	SAFE_DELETE(m_pGlyphsFont);

	BOOL bFontExists = XTPDrawHelpers()->FontExists(cszGlyphsFontName);
	if (bFontExists)
	{
		m_pGlyphsFont = new CFont();
		if (m_pGlyphsFont)
		{
			LOGFONT lfGlupth;
			::ZeroMemory(&lfGlupth, sizeof(lfGlupth));

			STRCPY_S(lfGlupth.lfFaceName, LF_FACESIZE, cszGlyphsFontName);
			lfGlupth.lfCharSet = SYMBOL_CHARSET;
			lfGlupth.lfHeight = -11;
			lfGlupth.lfWeight = FW_NORMAL;

			if (!m_pGlyphsFont->CreateFontIndirect(&lfGlupth))
			{
				SAFE_DELETE(m_pGlyphsFont);
			}
		}
	}

	if (!m_pGlyphsFont)
	{
		TRACE(_T("WARNING! Calendar PaintManeger: cannot create 'MS Outlook' font. Standard bitmaps are used. \n"));
	}
}


CXTPCalendarViewPart* CXTPCalendarPaintManager::AddViewPart(CXTPCalendarViewPart* pPart)
{
	ASSERT(pPart);
	if (pPart)
	{
		m_lstViewParts.AddTail(pPart);
		pPart->m_pPaintManager = this;
	}

	return pPart;
}

void CXTPCalendarPaintManager::DrawLine(CDC* pDC, int x, int y, int cx, int cy)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}

	pDC->MoveTo(x, y);
	pDC->LineTo(x + cx, y + cy);
}

void CXTPCalendarPaintManager::DrawHorizontalLine(CDC* pDC, int x, int y, int cx)
{
	DrawLine(pDC, x, y, cx, 0);
}
void CXTPCalendarPaintManager::DrawVerticalLine(CDC* pDC, int x, int y, int cy)
{
	DrawLine(pDC, x, y, 0, cy);
}

int CXTPCalendarPaintManager::DrawBitmap(CImageList* pImageList, CDC* pDC,
										 CRect rcView, int iIcon, UINT uFlags)
{
	ASSERT(pImageList && pDC);
	if (!pImageList || !pDC)
	{
		return 0;
	}

	int nImageBound = rcView.left;

	if (rcView.left >= rcView.right - 2)
		return nImageBound;

	POINT ptIcon;
	IMAGEINFO imgInf;

	if (!pImageList->GetImageInfo(iIcon, &imgInf))
		return nImageBound;

	CSize szImage(imgInf.rcImage.right - imgInf.rcImage.left, imgInf.rcImage.bottom - imgInf.rcImage.top);
	CSize szView(rcView.Size());
	ptIcon.x = rcView.left;
	ptIcon.y = rcView.top;

	szImage.cx = min(szImage.cx, szView.cx);

	if (uFlags & DT_VCENTER)
	{
		ptIcon.y = rcView.top + (szView.cy - szImage.cy) /2 + 1;
	}

	IMAGELISTDRAWINDIRECT_S(pImageList, pDC, iIcon, ptIcon, szImage);

	return szImage.cx;
}

int CXTPCalendarPaintManager::DrawBitmap(UINT nIDResource, CDC* pDC, CRect rcBitmap)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return 0;
	}

	CBitmap bmpIcon;

	if (!XTPResourceManager()->LoadBitmap(&bmpIcon, nIDResource))
		return 0;

	BITMAP bmpInfo;
	bmpIcon.GetBitmap(&bmpInfo);

	CImageList ilBitmap;
	VERIFY(ilBitmap.Create(bmpInfo.bmWidth, bmpInfo.bmHeight, ILC_COLOR24 | ILC_MASK, 0, 1));
	ilBitmap.Add(&bmpIcon, RGB(255, 0 , 255));

	CRect rcBitmap2 = rcBitmap;
	rcBitmap2.bottom = rcBitmap2.top + bmpInfo.bmHeight;

	DrawBitmap(&ilBitmap, pDC, rcBitmap2, 0);

// Don't work correctly for the printer
//
//  CXTPCompatibleDC dcMem(pDC, &bmpIcon);
//
	// Get the size of the bitmap
//  BITMAP bmpInfo;
//  bmpIcon.GetBitmap(&bmpInfo);

//  pDC->BitBlt(rcBitmap.left, rcBitmap.top,
//              bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMem,
//              0, 0, SRCCOPY);

	return bmpInfo.bmWidth;
}

CSize CXTPCalendarPaintManager::GetBitmapSize(UINT nIDResource)
{
	CBitmap bmpIcon;
	VERIFY(XTPResourceManager()->LoadBitmap(&bmpIcon, nIDResource));

	BITMAP bmpInfo;
	bmpIcon.GetBitmap(&bmpInfo);
	CSize szIcon(bmpInfo.bmWidth, bmpInfo.bmHeight);
	return szIcon;
}

const CSize CXTPCalendarPaintManager::GetExpandSignSize()
{
	// if sign size is not initialized, initialize it
	if (!(m_szExpandSign.cx | m_szExpandSign.cy))
	{
		m_szExpandSign = GetBitmapSize(XTP_IDB_CALENDAR_EXPANDSIGNDOWN);
	}
	return m_szExpandSign;
}

int CXTPCalendarPaintManager::DrawClock(CDC* pDC, COleDateTime dtClockTime,
	CRect rcView, COLORREF clrBackground, XTPCalendarClockAlignFlags cafAlign)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return 0;
	}

	static const CPoint arrHours[] =
		{
			CPoint(0, -6),
			CPoint(3, -5),
			CPoint(5, -3),
			CPoint(6, 0),
			CPoint(5, 3),
			CPoint(3, 5),
			CPoint(0, 6),
			CPoint(-3, 5),
			CPoint(-5, 3),
			CPoint(-6, 0),
			CPoint(-5, -3),
			CPoint(-3, -5),
			CPoint(0, -6)
		};

	static const CPoint arrHalfHours[] =
		{
			CPoint(1, -5),
			CPoint(3, -3),
			CPoint(4, -1),
			CPoint(5, 1),
			CPoint(3, 3),
			CPoint(1, 5),
			CPoint(-1, 5),
			CPoint(-3, 3),
			CPoint(-5, 1),
			CPoint(-5, -1),
			CPoint(-3, -3),
			CPoint(-1, -5),
			CPoint(1, -5)
		};

	if (rcView.Width() < m_szClockRect.cx || rcView.Height() < m_szClockRect.cy)
		return 0;

	CPoint ptClock(rcView.left, rcView.top);

	if (cafAlign == xtpCalendarClockAlignLeft)
		ptClock.Offset(1, (rcView.Height() - m_szClockRect.cy)/2 + 1);
	else if (cafAlign == xtpCalendarClockAlignRight)
		ptClock.Offset(rcView.Width() - m_szClockRect.cx - 1,
					   (rcView.Height() - m_szClockRect.cy)/2 + 1);
	else if (cafAlign == xtpCalendarClockAlignCenter)
		ptClock.Offset((rcView.Width() - m_szClockRect.cx)/2,
					   (rcView.Height() - m_szClockRect.cy)/2 + 1);

	CRect rcClock(0, 0, 0, 0);
	rcClock.SetRect(ptClock.x, ptClock.y,
					ptClock.x + m_szClockRect.cx,
					ptClock.y + m_szClockRect.cy);

	int nHour = dtClockTime.GetHour();
	int nMin = dtClockTime.GetMinute() /5;

	COLORREF clrFill;
	COLORREF clrItems;
	if (nHour >= 12)
	{
		clrFill = RGB(0, 0, 0);
		clrItems = clrBackground;
	}
	else
	{
		clrFill = clrBackground;
		clrItems = RGB(0, 0, 0);
	}


	nHour = nHour > 12 ? nHour - 12 : nHour;

	CXTPBrushDC brushClock(pDC->m_hDC, clrFill);
	CPoint ptCenter(rcClock.CenterPoint());
	pDC->Ellipse(rcClock);
	CXTPPenDC penItems(pDC->m_hDC, clrItems);
	int x = 0;
	int y = 0;

	for (int i = 0; i <= 12; i++)
	{
		pDC->SetPixel(ptCenter.x + arrHours[i].x,
					  ptCenter.y + arrHours[i].y, clrItems);
		if (nHour == i)
		{
			if (nMin >= 6)
			{
				x = arrHalfHours[i].x;
				y = arrHalfHours[i].y;
			}
			else
			{
				x = arrHours[i].x;
				y = arrHours[i].y;
				x = x > 0 ? x -1 : x == 0 ? 0 : x + 1;
				y = y > 0 ? y -1 : y == 0 ? 0 : y + 1;
			}
			pDC->MoveTo(ptCenter);
			pDC->LineTo(ptCenter.x+ x, ptCenter.y + y);
		}
		if (nMin == i)
		{
			pDC->MoveTo(ptCenter);
			pDC->LineTo(ptCenter.x + arrHours[i].x,
						ptCenter.y + arrHours[i].y);
		}
	}

	return m_szClockRect.cx;

}

void CXTPCalendarPaintManager::SetControl(CXTPCalendarControl* pControl)
{
	m_pControl = pControl;
}

int CXTPCalendarPaintManager::DrawIconChar(CDC* pDC, CRect rc, TCHAR ch)
{
	if (!pDC || !m_pGlyphsFont || !m_pGlyphsFont->m_hObject)
	{
		ASSERT(FALSE);
		return 0;
	}

	CFont* pPrevFnt = pDC->SelectObject(m_pGlyphsFont);
	pDC->SetTextColor(GetControlPart()->GetTextColor());
	TCHAR cszCh[2] = {ch, 0};

	UINT uFlags = DT_VCENTER | DT_LEFT; // DT_CENTER;

	//CSize szChar = pDC->GetTextExtent(szCh, 1);
	CRect rcNeed = rc;
	pDC->DrawText(cszCh, 1, rcNeed, uFlags | DT_CALCRECT);
	rc.bottom = rc.top + rcNeed.Height();

	pDC->DrawText(cszCh, 1, rc, uFlags);

	pDC->SelectObject(pPrevFnt);

	//return szChar.cx;
	return rcNeed.Width();
}

int CXTPCalendarPaintManager::DrawIcons(CDC* pDC, CXTPCalendarViewEvent* pViewEvent)
{
	if (!pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return 0;
	}


	BOOL bUseFont = XTP_SAFE_GET3(pViewEvent, GetCalendarControl(),
						GetCalendarOptions(), bUseOutlookFontGlyphs, FALSE);
	bUseFont = bUseFont && m_pGlyphsFont;

	int nWidth = 0;

	if (pViewEvent->GetEvent()->IsReminder() &&
		!pViewEvent->GetReminderGlyphRect().IsRectEmpty())
	{
		if (bUseFont)
		{
			nWidth += DrawIconChar(pDC, pViewEvent->GetReminderGlyphRect(), 0x41);
		}
		else
		{
			nWidth += DrawBitmap(&m_ilGlyphs, pDC,
							pViewEvent->GetReminderGlyphRect(), 0, DT_VCENTER);
		}
	}

	int nRecurrenceState = pViewEvent->GetEvent()->GetRecurrenceState();
	if (!pViewEvent->GetRecurrenceGlyphRect().IsRectEmpty())
	{
		if ((nRecurrenceState == xtpCalendarRecurrenceMaster) || (nRecurrenceState == xtpCalendarRecurrenceOccurrence))
		{
			if (bUseFont)
			{
				nWidth += DrawIconChar(pDC, pViewEvent->GetRecurrenceGlyphRect(), 0x43);
			}
			else
			{
				nWidth += DrawBitmap(&m_ilGlyphs, pDC,
							 pViewEvent->GetRecurrenceGlyphRect(), 1, DT_VCENTER);
			}
		}
		else if (nRecurrenceState == xtpCalendarRecurrenceException)
		{
			if (bUseFont)
			{
				nWidth += DrawIconChar(pDC, pViewEvent->GetRecurrenceGlyphRect(), 0x44);
			}
			else
			{
				nWidth += DrawBitmap(&m_ilGlyphs, pDC,
							pViewEvent->GetRecurrenceGlyphRect(), 2, DT_VCENTER);
			}
		}
	}

	if (pViewEvent->GetEvent()->IsMeeting() &&
		!pViewEvent->GetMeetingGlyphRect().IsRectEmpty())
	{
		if (bUseFont)
		{
			nWidth += DrawIconChar(pDC, pViewEvent->GetMeetingGlyphRect(), 0x45);
		}
		else
		{
			nWidth += DrawBitmap(&m_ilGlyphs, pDC,
							pViewEvent->GetMeetingGlyphRect(), 3, DT_VCENTER);
		}
	}

	if (pViewEvent->GetEvent()->IsPrivate() &&
		!pViewEvent->GetPrivateGlyphRect().IsRectEmpty())
	{
		if (bUseFont)
		{
			nWidth += DrawIconChar(pDC, pViewEvent->GetPrivateGlyphRect(), 0x42);
		}
		else
		{
			nWidth += DrawBitmap(&m_ilGlyphs, pDC,
							pViewEvent->GetPrivateGlyphRect(), 4, DT_VCENTER);
		}
	}

	return nWidth;
}

void CXTPCalendarPaintManager::InitBusyStatusDefaultColors()
{
	m_brushTentative.DeleteObject();
	m_bmpTentativePattern.DeleteObject();

	VERIFY(XTPResourceManager()->LoadBitmap(&m_bmpTentativePattern, XTP_IDB_CALENDAR_TENATIVE_PATTERN));
	VERIFY(m_brushTentative.CreatePatternBrush(&m_bmpTentativePattern));


	CXTPCalendarViewPartBrushValue* pBVal_Free = _GetAtGrow_BusyStatusBrushValue(xtpCalendarBusyStatusFree, TRUE);
	CXTPCalendarViewPartBrushValue* pBVal_Tentative = _GetAtGrow_BusyStatusBrushValue(xtpCalendarBusyStatusTentative, TRUE);
	CXTPCalendarViewPartBrushValue* pBVal_Busy = _GetAtGrow_BusyStatusBrushValue(xtpCalendarBusyStatusBusy, TRUE);
	CXTPCalendarViewPartBrushValue* pBVal_OutOO = _GetAtGrow_BusyStatusBrushValue(xtpCalendarBusyStatusOutOfOffice, TRUE);

	if (pBVal_Free)
	{
		pBVal_Free->SetStandardValue(new CBrush(RGB(0xFF, 0xFF, 0xFF)), TRUE);
	}
	if (pBVal_Tentative)
	{
		pBVal_Tentative->SetStandardValue(&m_brushTentative, FALSE);
	}
	if (pBVal_Busy)
	{
		pBVal_Busy->SetStandardValue(new CBrush(RGB(0, 0, 0xFF)), TRUE);
	}
	if (pBVal_OutOO)
	{
		pBVal_OutOO->SetStandardValue(new CBrush(RGB(132, 0, 132)), TRUE);
	}
}

void CXTPCalendarPaintManager::SetBusyStatusBrush(int eBusyStatus, CBrush* pBrush, BOOL bAutodeleteBrush)
{
	if (eBusyStatus < 0 || !pBrush)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarViewPartBrushValue* pBVal = _GetAtGrow_BusyStatusBrushValue(eBusyStatus, TRUE);
	ASSERT(pBVal);
	if (pBVal)
	{
		pBVal->SetCustomValue(pBrush, bAutodeleteBrush);
	}
}

CBrush* CXTPCalendarPaintManager::GetBusyStatusBrush(int nBusyStatus)
{
	CXTPCalendarViewPartBrushValue* pBVal = _GetAtGrow_BusyStatusBrushValue(nBusyStatus, TRUE);
	ASSERT(pBVal);
	if (pBVal)
	{
		CBrush* pBrush = pBVal->GetValue();
		return pBrush;
	}

	return NULL;
}

CXTPCalendarViewPartBrushValue* CXTPCalendarPaintManager::_GetAtGrow_BusyStatusBrushValue(
	int nBusyStatus, BOOL bGrow)
{
	if (nBusyStatus < 0)
	{
		//ASSERT(FALSE);
		nBusyStatus = 0;
	}

	int nCount = (int)m_arBusyStatusBrushes.GetSize();

	if (nBusyStatus >= nCount || m_arBusyStatusBrushes[nBusyStatus] == NULL)
	{
		if (!bGrow)
		{
			return NULL;
		}
		m_arBusyStatusBrushes.SetAtGrow(nBusyStatus, new CXTPCalendarViewPartBrushValue);
	}
	CXTPCalendarViewPartBrushValue* pBVal = m_arBusyStatusBrushes[nBusyStatus];
	return pBVal;
}

void CXTPCalendarPaintManager::DrawBusyStatus(CDC* pDC, CRect& rcRect, int eBusyStatus)
{
	CBrush* pBrush = GetBusyStatusBrush(eBusyStatus);
	if (!pBrush || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	pDC->FillRect(&rcRect, pBrush);
}

void CXTPCalendarPaintManager::DrawBusyStatus(CDC* pDC, CRgn& rgnBusy, int eBusyStatus)
{
	CBrush* pBrush = GetBusyStatusBrush(eBusyStatus);
	if (!pBrush || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	CBrush brBorder(m_clrWindowText);

	VERIFY(pDC->FillRgn(&rgnBusy, pBrush));
	VERIFY(pDC->FrameRgn(&rgnBusy, &brBorder, 1, 1));
}

//////////////////////////////////////////////////////////////////////////
