// XTCaptionTheme.cpp: implementation of the CXTCaptionTheme class.
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
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPWinThemeWrapper.h"

#include "XTThemeManager.h"
#include "XTButtonTheme.h"
#include "XTDefines.h"
#include "XTButton.h"
#include "XTCaption.h"
#include "XTCaptionTheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_THEME_FACTORY(CXTCaptionTheme)

/////////////////////////////////////////////////////////////////////////////
// CXTCaptionTheme

CXTCaptionTheme::CXTCaptionTheme()
{
}

void CXTCaptionTheme::RefreshMetrics()
{
	CXTThemeManagerStyle ::RefreshMetrics();

	m_clrEdgeShadow.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	m_clrEdgeLight.SetStandardValue(GetXtremeColor(COLOR_3DHILIGHT));
}

void CXTCaptionTheme::DrawCaptionBack(CDC* pDC, CXTCaption* pCaption, CRect& rcItem)
{
	CRect rc = rcItem;

	// Set the background mode to transparent.
	pDC->SetBkMode(TRANSPARENT);

	// repaint the background.
	pDC->FillSolidRect(rc, pCaption->m_clrBorder);

	// draw the caption border.
	if (pCaption->m_dwExStyle & CPWS_EX_GROOVE_EDGE)
	{
		pDC->Draw3dRect(rc, m_clrEdgeShadow, m_clrEdgeLight);
		rc.DeflateRect(1, 1);
		pDC->Draw3dRect(rc, m_clrEdgeLight, m_clrEdgeShadow);

		if (pCaption->m_nBorder == 0)
		{
			rc.DeflateRect(1, 1);
		}
	}

	else
	if (pCaption->m_dwExStyle & CPWS_EX_RAISED_EDGE)
	{
		pDC->Draw3dRect(rc, m_clrEdgeLight, m_clrEdgeShadow);

		if (pCaption->m_nBorder == 0)
		{
			rc.DeflateRect(1, 1);
		}
	}

	// draw the caption banner border.
	rc.DeflateRect(pCaption->m_nBorder, pCaption->m_nBorder);
	pDC->FillSolidRect(rc, pCaption->m_clrFace);
}

void CXTCaptionTheme::DrawCaptionText(CDC* pDC, CXTCaption* pCaption)
{
		// if the caption text is empty, return.
	if (pCaption->GetCaptionText().IsEmpty())
	{
		return;
	}

	// Get the display area for the text.
	CRect rcText(pCaption->GetTextRect());

	// Set the text color and select the caption font.
	CXTPFontDC fontDC(pDC, pCaption->GetFont());
	pDC->SetTextColor(pCaption->m_clrText);

	// draw the text, and select the original font.
	pDC->DrawText(pCaption->GetCaptionText(), rcText,
		DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
}

void CXTCaptionTheme::DrawCaptionIcon(CDC* pDC, CXTCaption* pCaption, CRect& rcItem)
{
	// If no icon was defined, just return.
	if (pCaption->m_hIcon == NULL)
		return;

	CRect rc = rcItem;
	rc.left = (rc.right - pCaption->m_sizeIcon.cx)-(pCaption->m_nBorder + 2);

	// stop the icon at the left hand border
	if (rc.left < pCaption->m_nBorder)
	{
		rc.left = pCaption->m_nBorder;
	}

	rc.top = (rcItem.Height() - pCaption->m_sizeIcon.cy)/2;
	rc.right = rc.left + pCaption->m_sizeIcon.cx;
	rc.bottom = rc.top + pCaption->m_sizeIcon.cy;

	// Only draw the icon if it will fit into the space between the borders
	if (rc.right < rcItem.right - pCaption->m_nBorder)
	{
		// draw the icon associated with the caption.
		::DrawIconEx(pDC->GetSafeHdc(), rc.left, rc.top, pCaption->m_hIcon,
			rc.Width(), rc.Height(), NULL, (HBRUSH)NULL, DI_NORMAL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTCaptionThemeOfficeXP

void CXTCaptionThemeOfficeXP::DrawCaptionBack(CDC* pDC, CXTCaption* pCaption, CRect& rcItem)
{
	CRect rc = rcItem;

	pDC->SetBkMode(TRANSPARENT);

	// repaint the background.
	pDC->FillSolidRect(rc, pCaption->m_clrBorder);

	if (!pCaption->IsAppCaption())
	{
		pDC->Draw3dRect(rc, GetXtremeColor(COLOR_3DHILIGHT), GetXtremeColor(COLOR_3DSHADOW));
	}
	else
	{
		// draw the caption banner border.
		rc.DeflateRect(pCaption->m_nBorder, pCaption->m_nBorder);
		pDC->FillSolidRect(rc, pCaption->m_clrFace);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTCaptionThemeOffice2003

CXTCaptionThemeOffice2003::CXTCaptionThemeOffice2003()
{

}

CXTCaptionThemeOffice2003::CXTCaptionThemeOffice2003(BOOL /*bPrimaryCaption*/)
{
	XT_ERROR_MSG("CXTCaptionThemeOffice2003::CXTCaptionThemeOffice2003(BOOL bPrimaryCaption) has been deprecated.\n\n"
		"Use CXTCaption::SetAppCaption(bool bAppCaption) to set the application's primary caption.");
}

void CXTCaptionThemeOffice2003::DrawCaptionBack(CDC* pDC, CXTCaption* pCaption, CRect& rcItem)
{
	pDC->SetBkMode(TRANSPARENT);

	if (pCaption->IsAppCaption())
	{
		XTPDrawHelpers()->GradientFill(pDC, rcItem, XTPColorManager()->grcCaption, FALSE);
		pDC->Draw3dRect(rcItem, GetXtremeColor(XPCOLOR_FRAME), GetXtremeColor(XPCOLOR_FRAME));
	}
	else
	{
		XTPDrawHelpers()->GradientFill(pDC, rcItem, XTPColorManager()->grcToolBar, FALSE);
		pDC->FillSolidRect(rcItem.left, rcItem.bottom - 1, rcItem.Width(), 1, GetXtremeColor(XPCOLOR_FRAME));
	}
}

IMPLEMENT_THEME_FACTORY(CXTCaptionButtonTheme)

/////////////////////////////////////////////////////////////////////////////
// CXTCaptionButtonTheme

CXTCaptionButtonTheme::CXTCaptionButtonTheme()
{
	m_bOffsetHiliteText = FALSE;
}

BOOL CXTCaptionButtonTheme::DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton)
{
	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		SetColorFace(pCaption->m_clrFace);
	}

	return CXTButtonTheme::DrawButtonThemeBackground(lpDIS, pButton);
}

COLORREF CXTCaptionButtonTheme::GetTextColor(UINT /*nState*/, CXTButton* pButton)
{
	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		return pCaption->m_clrText;
	}

	return m_crText;
}

void CXTCaptionButtonTheme::DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton)
{
	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		if (pCaption->IsAppCaption() && !pCaption->GetCaptionText().IsEmpty())
		{
			// select font into device context.
			CXTPFontDC fontDC(pDC, GetThemeFont(pButton));
			pDC->SetTextColor(GetTextColor(nState, pButton));

			if (nState & ODS_SELECTED && m_bOffsetHiliteText)
				rcItem.OffsetRect(1, 1);

			// draw the text, and select the original font.
			pDC->DrawText(pCaption->GetCaptionText(), rcItem,
				DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTCaptionButtonThemeOfficeXP

CXTCaptionButtonThemeOfficeXP::CXTCaptionButtonThemeOfficeXP()
{
	m_bAnimateIcon = FALSE;
	m_bFadedIcon = FALSE;
	m_bOffsetHiliteText = FALSE;
}

COLORREF CXTCaptionButtonThemeOfficeXP::GetTextColor(UINT nState, CXTButton* pButton)
{
	if (nState & ODS_DISABLED)
		return GetXtremeColor(COLOR_GRAYTEXT);

	if (pButton->GetHilite() || (nState & ODS_SELECTED))
		return m_crTextHilite;

	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		return pCaption->m_clrText;
	}

	return m_crText;
}

BOOL CXTCaptionButtonThemeOfficeXP::DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton)
{
	// define some temporary variables.
	CDC*  pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rcItem = lpDIS->rcItem;
	int   nState = lpDIS->itemState;

	// Paint the background.
	if (pButton->GetHilite() || (nState & ODS_SELECTED))
	{
		pDC->FillSolidRect(rcItem, (nState & ODS_SELECTED) ?
			GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED) : GetXtremeColor(XPCOLOR_HIGHLIGHT));

		pDC->Draw3dRect(rcItem, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER),
			GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
	}
	else
	{
		CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
		if (::IsWindow(pCaption->GetSafeHwnd()))
		{
			m_crBack = pCaption->m_clrFace;
		}

		pDC->FillSolidRect(rcItem, m_crBack);

		if (m_bWordTheme)
		{
			pDC->Draw3dRect(rcItem,
				m_crBorderShadow, m_crBorderShadow);

		}
	}

	return TRUE;
}

void CXTCaptionButtonThemeOfficeXP::DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton)
{
	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		if (pCaption->IsAppCaption() && !pCaption->GetCaptionText().IsEmpty())
		{
			// select font into device context.
			CXTPFontDC fontDC(pDC, GetThemeFont(pButton));
			pDC->SetTextColor(GetTextColor(nState, pButton));

			if (nState & ODS_SELECTED && m_bOffsetHiliteText)
				rcItem.OffsetRect(1, 1);

			// draw the text, and select the original font.
			pDC->DrawText(pCaption->GetCaptionText(), rcItem,
				DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTCaptionButtonThemeOffice2003

BOOL CXTCaptionButtonThemeOffice2003::GetGradientValues(CRect& rButton, CXTCaption* pCaption, CXTPPaintManagerColorGradient& clr)
{
	if (!::IsWindow(pCaption->GetSafeHwnd()))
		return FALSE;

	// get caption and button coordinates.
	CXTPClientRect rCaption(pCaption);
	pCaption->ScreenToClient(&rButton);

	// get the caption's device context.
	CClientDC dc(pCaption);

	// create a memory device context to simulate caption background.
	CDC memDC;
	if (memDC.CreateCompatibleDC(&dc))
	{
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&dc, rCaption.Width(), rCaption.Height());
		CBitmap* pBmp = memDC.SelectObject(&bitmap);

		// draw a gradient to the memory device context that matches what should be drawn
		// on the caption background.
		XTPDrawHelpers()->GradientFill(&memDC, rCaption, pCaption->IsAppCaption() ?
			XTPColorManager()->grcCaption: XTPColorManager()->grcToolBar, FALSE);

		// determine the light color gradient.
		COLORREF crLight = memDC.GetPixel(rButton.TopLeft());

		// determine the dark color gradient.
		COLORREF crDark = memDC.GetPixel(rButton.BottomRight());

		// set the gradient to and from values.
		clr.SetStandardValue(crLight, crDark);

		// cleanup GDI
		memDC.SelectObject(pBmp);
		memDC.DeleteDC();
		bitmap.DeleteObject();

		return TRUE;
	}

	return FALSE;
}

BOOL CXTCaptionButtonThemeOffice2003::DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton)
{
	if (!::IsWindow(pButton->GetSafeHwnd()))
		return FALSE;

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc = lpDIS->rcItem;
	int nState = lpDIS->itemState;

	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		BOOL bHilite = pButton->GetHilite();
		BOOL bSelected = (nState & ODS_SELECTED) != 0;

		// Paint the background.
		if (bSelected || bHilite)
		{
			XTPDrawHelpers()->GradientFill(pDC, rc, bSelected ?
				XTPColorManager()->grcLunaPushed: XTPColorManager()->grcLunaSelected, FALSE);

			pDC->Draw3dRect(&rc, GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER),
				GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		}
		else
		{
			CXTPWindowRect rButton(pButton);

			CXTPPaintManagerColorGradient clr;
			GetGradientValues(rButton, pCaption, clr);

			XTPDrawHelpers()->GradientFill(pDC, rc, clr, FALSE);
		}

		return TRUE;
	}

	return CXTButtonThemeOffice2003::DrawButtonThemeBackground(lpDIS, pButton);
}

COLORREF CXTCaptionButtonThemeOffice2003::GetTextColor(UINT nState, CXTButton* pButton)
{
	if (nState & ODS_DISABLED)
		return GetXtremeColor(COLOR_GRAYTEXT);

	if (pButton->GetHilite() || (nState & ODS_SELECTED))
		return m_crTextHilite;

	CXTCaption* pCaption = ((CXTCaptionButton*)pButton)->GetCaption();
	if (::IsWindow(pCaption->GetSafeHwnd()))
	{
		return pCaption->m_clrText;
	}

	return m_crText;
}
