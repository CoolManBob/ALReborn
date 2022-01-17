// XTFlatControlsTheme.cpp: implementation of the CXTFlatControlsTheme class.
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

#include "XTFlatControlsTheme.h"

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTFlatComboBox.h"

#include "XTHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_THEME_FACTORY(CXTFlatComboBoxTheme)

/////////////////////////////////////////////////////////////////////////////
// CXTFlatComboBoxTheme class
//

CXTFlatComboBoxTheme::CXTFlatComboBoxTheme()
{
}

void CXTFlatComboBoxTheme::RefreshMetrics()
{
	CXTThemeManagerStyle ::RefreshMetrics();

	// get system metrics for border, 3D edge and drop arrow box.
	m_sizeThumb.cx = ::GetSystemMetrics(SM_CXHTHUMB);
	m_sizeThumb.cy = ::GetSystemMetrics(SM_CYVTHUMB);
	m_sizeBorder.cx = ::GetSystemMetrics(SM_CXBORDER);
	m_sizeBorder.cy = ::GetSystemMetrics(SM_CYBORDER);
	m_sizeEdge.cx = ::GetSystemMetrics(SM_CXEDGE);
	m_sizeEdge.cy = ::GetSystemMetrics(SM_CYEDGE);

	// define sizes for office arrow and max height.
	m_cxArrow = GetDropArrowWidth();
}

void CXTFlatComboBoxTheme::DrawFlatComboBox(CDC* pDC, CXTFlatComboBox* pCombo, int eState)
{
	CRect rcItem;
	pCombo->GetClientRect(&rcItem);

	CRect rcBack(rcItem);
	rcBack.DeflateRect(m_sizeEdge);

	if (pCombo->IsWindowEnabled())
	{
		// erase background.
		pDC->FillSolidRect(rcBack, GetXtremeColor(COLOR_WINDOW));

		CRect rFillColor(rcBack);
		rFillColor.DeflateRect(m_sizeBorder);
		rFillColor.right -= m_cxArrow;

		if (eState == xtMouseHover || eState == xtMouseSelect)
		{
			rFillColor.right--;
			pDC->Draw3dRect(&rcItem, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHILIGHT));
		}

		if (pCombo->GetEditSel() == (DWORD)CB_ERR)
			pDC->FillSolidRect(rFillColor, pCombo->GetBackColor());
	}
	else
	{
		pDC->FillSolidRect(rcBack, GetXtremeColor(XPCOLOR_3DFACE));

		pDC->Draw3dRect(&rcItem, GetXtremeColor(XPCOLOR_3DSHADOW),
			GetXtremeColor(XPCOLOR_3DSHADOW));
	}

	DrawDownArrow(pDC, rcBack, eState, pCombo);
}

void CXTFlatComboBoxTheme::DrawDownArrow(CDC* pDC, const CRect& rcItem, int eState, CXTFlatComboBox* pCombo)
{
	// determine background size.
	CRect rcArrow(rcItem);
	DrawArrowBack(pDC, rcArrow, eState, pCombo);

	CPoint pt;
	pt.x = (rcArrow.left + rcArrow.right - 4) / 2;
	pt.y = (rcArrow.top + rcArrow.bottom - 2) / 2;

	if (eState == xtMouseSelect)
		pt.Offset(1, 1);

	COLORREF cr = pCombo->IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOWTEXT) : GetXtremeColor(COLOR_3DSHADOW);

	pDC->SetPixel(pt.x, pt.y, cr);
	pDC->SetPixel(pt.x+1, pt.y, cr);
	pDC->SetPixel(pt.x+2, pt.y, cr);
	pDC->SetPixel(pt.x+3, pt.y, cr);
	pDC->SetPixel(pt.x+4, pt.y, cr);
	pDC->SetPixel(pt.x+1, pt.y+1, cr);
	pDC->SetPixel(pt.x+2, pt.y+1, cr);
	pDC->SetPixel(pt.x+3, pt.y+1, cr);
	pDC->SetPixel(pt.x+2, pt.y+2, cr);
}

int CXTFlatComboBoxTheme::GetDropArrowWidth() const
{
	TEXTMETRIC tm;

	CWindowDC dc(NULL);
	CXTPFontDC fontDC(&dc, &XTAuxData().font);
	dc.GetTextMetrics(&tm);

	int nThumbWidth = __max(15, tm.tmAveCharWidth * 2 + 4);

	return min(nThumbWidth, GetSystemMetrics(SM_CXVSCROLL) + 1);

}

void CXTFlatComboBoxTheme::DrawArrowBack(CDC* pDC, CRect& rcArrow, int eState, CXTFlatComboBox* /*pCombo*/)
{
	rcArrow.left = rcArrow.right - (m_cxArrow - 4);

	if (eState == xtMouseHover)
	{
		pDC->Draw3dRect(&rcArrow, GetXtremeColor(COLOR_3DHILIGHT), GetXtremeColor(COLOR_3DSHADOW));
		rcArrow.DeflateRect(-1, -1);

		pDC->Draw3dRect(&rcArrow, GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_3DFACE));
		rcArrow.DeflateRect(1, 1);
	}

	if (eState == xtMouseSelect)
	{
		pDC->Draw3dRect(&rcArrow, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHILIGHT));
		rcArrow.DeflateRect(-1, -1);

		pDC->Draw3dRect(&rcArrow, GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_3DFACE));
		rcArrow.DeflateRect(1, 1);
	}

	// erase background.
	rcArrow.DeflateRect(1, 1);
	pDC->FillSolidRect(&rcArrow, GetXtremeColor(COLOR_3DFACE));
}

/////////////////////////////////////////////////////////////////////////////
// CXTFlatComboBoxThemeOfficeXP class
//

CXTFlatComboBoxThemeOfficeXP::CXTFlatComboBoxThemeOfficeXP()
{

}

void CXTFlatComboBoxThemeOfficeXP::DrawArrowBack(CDC* pDC, CRect& rcArrow, int eState, CXTFlatComboBox* /*pCombo*/)
{
	rcArrow.left = rcArrow.right - (m_cxArrow - 2);

	if (eState == xtMouseHover)
	{
		pDC->FillSolidRect(&rcArrow, GetXtremeColor(XPCOLOR_HIGHLIGHT));

		CXTPPenDC pen(*pDC, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
		pDC->MoveTo(rcArrow.left-1, rcArrow.top);
		pDC->LineTo(rcArrow.left-1, rcArrow.bottom);
	}

	else if (eState == xtMouseSelect)
	{
		pDC->FillSolidRect(&rcArrow, GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED));

		CXTPPenDC pen(*pDC, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
		pDC->MoveTo(rcArrow.left-1, rcArrow.top);
		pDC->LineTo(rcArrow.left-1, rcArrow.bottom);
	}

	else
	{
		rcArrow.DeflateRect(1, 1);
		pDC->FillSolidRect(&rcArrow, GetXtremeColor(COLOR_3DFACE));
	}
}

void CXTFlatComboBoxThemeOfficeXP::DrawDownArrow(CDC* pDC, const CRect& rcItem, int eState, CXTFlatComboBox* pCombo)
{
	// determine background size.
	CRect rcArrow(rcItem);
	DrawArrowBack(pDC, rcArrow, eState, pCombo);

	CPoint pt;
	pt.x = (rcArrow.left + rcArrow.right - 4) / 2;
	pt.y = (rcArrow.top + rcArrow.bottom - 2) / 2;

	COLORREF cr = pCombo->IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOWTEXT) : GetXtremeColor(COLOR_3DSHADOW);

	pDC->SetPixel(pt.x, pt.y, cr);
	pDC->SetPixel(pt.x+1, pt.y, cr);
	pDC->SetPixel(pt.x+2, pt.y, cr);
	pDC->SetPixel(pt.x+3, pt.y, cr);
	pDC->SetPixel(pt.x+4, pt.y, cr);
	pDC->SetPixel(pt.x+1, pt.y+1, cr);
	pDC->SetPixel(pt.x+2, pt.y+1, cr);
	pDC->SetPixel(pt.x+3, pt.y+1, cr);
	pDC->SetPixel(pt.x+2, pt.y+2, cr);
}

void CXTFlatComboBoxThemeOfficeXP::DrawFlatComboBox(CDC* pDC, CXTFlatComboBox* pCombo, int eState)
{
	CRect rcItem;
	pCombo->GetClientRect(&rcItem);

	CRect rcBack(rcItem);
	rcBack.DeflateRect(m_sizeBorder);

	if (pCombo->IsWindowEnabled())
	{
		pDC->FillSolidRect(rcBack, GetXtremeColor(COLOR_WINDOW));

		CRect rFillColor = rcBack;
		rFillColor.DeflateRect(m_sizeBorder);
		rFillColor.right -= m_cxArrow;

		if (eState == xtMouseHover || eState == xtMouseSelect)
		{
			rFillColor.right--;
			pDC->Draw3dRect(&rcItem, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER),
				GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
		}
		else
		{
			pDC->Draw3dRect(&rcItem, GetXtremeColor(XPCOLOR_3DFACE),
				GetXtremeColor(XPCOLOR_3DFACE));
		}

		if (pCombo->GetEditSel() == (DWORD)CB_ERR)
			pDC->FillSolidRect(rFillColor, pCombo->GetBackColor());
	}
	else
	{
		pDC->FillSolidRect(rcBack, GetXtremeColor(XPCOLOR_3DFACE));

		pDC->Draw3dRect(&rcItem, GetXtremeColor(XPCOLOR_3DSHADOW),
			GetXtremeColor(XPCOLOR_3DSHADOW));
	}

	DrawDownArrow(pDC, rcBack, eState, pCombo);
}

/////////////////////////////////////////////////////////////////////////////
// CXTFlatComboBoxThemeOffice2003 class
//

CXTFlatComboBoxThemeOffice2003::CXTFlatComboBoxThemeOffice2003()
{

}

void CXTFlatComboBoxThemeOffice2003::DrawArrowBack(CDC* pDC, CRect& rcArrow, int eState, CXTFlatComboBox* pCombo)
{
	rcArrow.left = rcArrow.right - (m_cxArrow - 2);

	if (eState == xtMouseHover)
	{
		XTPDrawHelpers()->GradientFill(pDC, &rcArrow,
			XTPColorManager()->grcLunaSelected, FALSE);

		CXTPPenDC pen(*pDC, GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		pDC->MoveTo(rcArrow.left-1, rcArrow.top);
		pDC->LineTo(rcArrow.left-1, rcArrow.bottom);
	}

	else if (eState == xtMouseSelect)
	{
		XTPDrawHelpers()->GradientFill(pDC, &rcArrow,
			XTPColorManager()->grcLunaPushed, FALSE);

		CXTPPenDC pen(*pDC, GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		pDC->MoveTo(rcArrow.left-1, rcArrow.top);
		pDC->LineTo(rcArrow.left-1, rcArrow.bottom);
	}

	else if (pCombo->IsWindowEnabled())
	{
		rcArrow.DeflateRect(1, 1);
		XTPDrawHelpers()->GradientFill(pDC, &rcArrow,
			XTPColorManager()->grcMenuItemPopup, FALSE);
	}
}

void CXTFlatComboBoxThemeOffice2003::DrawFlatComboBox(CDC* pDC, CXTFlatComboBox* pCombo, int eState)
{
	CRect rcItem;
	pCombo->GetClientRect(&rcItem);

	CRect rcBack(rcItem);
	rcBack.DeflateRect(m_sizeBorder);

	if (pCombo->IsWindowEnabled())
	{
		pDC->FillSolidRect(rcBack, GetXtremeColor(COLOR_WINDOW));

		CRect rFillColor = rcBack;
		rFillColor.DeflateRect(m_sizeBorder);
		rFillColor.right -= m_cxArrow;

		if (eState == xtMouseHover || eState == xtMouseSelect)
		{
			rFillColor.right--;
			pDC->Draw3dRect(&rcItem, GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER),
				GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		}

		if (pCombo->GetEditSel() == (DWORD)CB_ERR)
			pDC->FillSolidRect(rFillColor, pCombo->GetBackColor());
	}
	else
	{
		pDC->FillSolidRect(rcBack, GetXtremeColor(XPCOLOR_3DFACE));

		pDC->Draw3dRect(&rcItem, GetXtremeColor(XPCOLOR_3DSHADOW),
			GetXtremeColor(XPCOLOR_3DSHADOW));
	}

	DrawDownArrow(pDC, rcBack, eState, pCombo);
}

IMPLEMENT_THEME_FACTORY(CXTFlatEditTheme)

/////////////////////////////////////////////////////////////////////////////
// CXTFlatEditTheme class

CXTFlatEditTheme::CXTFlatEditTheme()
{

}

void CXTFlatEditTheme::RefreshMetrics()
{
	CXTThemeManagerStyle ::RefreshMetrics();
}

void CXTFlatEditTheme::DrawBorders(CDC* pDC, CXTFlatEdit* pEdit, const CRect& rWindow, int eState)
{
	if (pEdit->IsWindowEnabled())
	{
		// erase background.
		pDC->FillSolidRect(&rWindow, GetXtremeColor(COLOR_WINDOW));

		if (eState == xtMouseHover || eState == xtMouseSelect)
		{
			pDC->Draw3dRect(&rWindow, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHILIGHT));
		}
	}
	else
	{
		pDC->FillSolidRect(&rWindow, GetXtremeColor(XPCOLOR_3DFACE));

		pDC->Draw3dRect(&rWindow, GetXtremeColor(XPCOLOR_3DSHADOW),
			GetXtremeColor(XPCOLOR_3DSHADOW));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTFlatEditThemeOfficeXP class

CXTFlatEditThemeOfficeXP::CXTFlatEditThemeOfficeXP()
{

}

void CXTFlatEditThemeOfficeXP::RefreshMetrics()
{
	CXTFlatEditTheme::RefreshMetrics();

	m_clrHighlightText.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT));
	m_clrPushedText.SetStandardValue(GetXtremeColor(XPCOLOR_PUSHED_TEXT));
	m_clrHighlight.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT));
	m_clrPushed.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED));
	m_clrFrameHighlight.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
	m_clrFrameNormal.SetStandardValue(GetXtremeColor(COLOR_WINDOW));
	m_clrNormal.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
	m_clrFrameEdit.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
}

void CXTFlatEditThemeOfficeXP::DrawBorders(CDC* pDC, CXTFlatEdit* pEdit, const CRect& rWindow, int eState)
{
	if (pEdit->IsWindowEnabled())
	{
		pDC->FillSolidRect(&rWindow, GetXtremeColor(COLOR_WINDOW));

		if (eState == xtMouseHover || eState == xtMouseSelect)
		{
			pDC->Draw3dRect(&rWindow, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER),
				GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
		}
	}
	else
	{
		pDC->FillSolidRect(&rWindow, GetXtremeColor(XPCOLOR_3DFACE));

		pDC->Draw3dRect(&rWindow, GetXtremeColor(XPCOLOR_3DSHADOW),
			GetXtremeColor(XPCOLOR_3DSHADOW));
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTFlatEditThemeOffice2003

CXTFlatEditThemeOffice2003::CXTFlatEditThemeOffice2003()
{

}

void CXTFlatEditThemeOffice2003::RefreshMetrics()
{
	CXTFlatEditThemeOfficeXP::RefreshMetrics();

	XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();

	switch (systemTheme)
	{
	case xtpSystemThemeBlue:
		m_clrHighlightText.SetStandardValue(0);
		m_clrPushedText.SetStandardValue(0);
		m_clrHighlight.SetStandardValue(RGB(255, 238, 194));
		m_clrPushed.SetStandardValue(RGB(254, 128, 62));
		m_clrFrameHighlight.SetStandardValue(RGB(0, 0, 128));
		m_clrFrameNormal.SetStandardValue(RGB(127, 157, 185));
		m_clrNormal.SetStandardValue(RGB(169, 199, 240));
		break;

	case xtpSystemThemeOlive:
		m_clrHighlightText.SetStandardValue(0);
		m_clrPushedText.SetStandardValue(0);
		m_clrHighlight.SetStandardValue(RGB(255, 238, 194));
		m_clrPushed.SetStandardValue(RGB(254, 128, 62));
		m_clrFrameHighlight.SetStandardValue(RGB(63, 93, 56));
		m_clrFrameNormal.SetStandardValue(RGB(164, 185, 127));
		m_clrNormal.SetStandardValue(RGB(197, 212, 159));
		break;

	case xtpSystemThemeSilver:
		m_clrHighlightText.SetStandardValue(0);
		m_clrPushedText.SetStandardValue(0);
		m_clrHighlight.SetStandardValue(RGB(255, 238, 194));
		m_clrPushed.SetStandardValue(RGB(254, 128, 62));
		m_clrFrameHighlight.SetStandardValue(RGB(75, 75, 111));
		m_clrFrameNormal.SetStandardValue(RGB(165, 172, 178));
		m_clrNormal.SetStandardValue(RGB(192, 192, 211));
		break;
	default:
		m_clrFrameNormal.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	}
	m_clrFrameEdit.SetStandardValue(m_clrFrameNormal);
}

void CXTFlatEditThemeOffice2003::DrawBorders(CDC* pDC, CXTFlatEdit* pEdit, const CRect& rWindow, int eState)
{
	if (pEdit->IsWindowEnabled())
	{
		pDC->FillSolidRect(&rWindow, GetXtremeColor(COLOR_WINDOW));

		if (eState == xtMouseHover || eState == xtMouseSelect)
		{
			pDC->Draw3dRect(&rWindow, GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER),
				GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		}
	}
	else
	{
		pDC->FillSolidRect(&rWindow, GetXtremeColor(XPCOLOR_3DFACE));

		pDC->Draw3dRect(&rWindow, GetXtremeColor(XPCOLOR_3DSHADOW),
			GetXtremeColor(XPCOLOR_3DSHADOW));
	}
}
