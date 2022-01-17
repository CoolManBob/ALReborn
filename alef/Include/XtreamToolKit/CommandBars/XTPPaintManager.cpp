// XTPPaintManager.cpp : implementation of the CXTPPaintManager class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPPaintManager.h"
#include "XTPControlButton.h"
#include "XTPControlPopup.h"
#include "XTPControlGallery.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPPopupBar.h"
#include "XTPControlComboBox.h"
#include "XTPControlEdit.h"
#include "XTPControls.h"
#include "XTPStatusBar.h"

#ifdef _XTP_INCLUDE_RIBBON
#include "XTPOffice2007Theme.h"
#include "Ribbon/XTPRibbonTheme.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace XTPPaintThemes;

CXTPPaintManager* CXTPPaintManager::s_pInstance = NULL;

//////////////////////////////////////////////////////////////////////
// CXTPPaintManager::CXTPPaintManagerDestructor

static class CXTPPaintManager::CPaintManagerDestructor
{
public:
	~CPaintManagerDestructor()
	{
		CXTPPaintManager::Done();
	}

} __xtpPaintManagerDestructor;


///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPanePaintManager::CLogFont

CXTPPaintManager::CLogFont::CLogFont()
{
	::ZeroMemory(this, sizeof(LOGFONT));
}

///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPanePaintManager::CNonClientMetrics

CXTPPaintManager::CNonClientMetrics::CNonClientMetrics()
{
	::ZeroMemory(this, sizeof(NONCLIENTMETRICS));
	cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), this, 0));
}

CXTPPaintManager::CXTPPaintManager()
{
	m_bFlatToolBar = FALSE;
	m_bFlatMenuBar = FALSE;

	m_nTextHeight = 0;
	m_bUseOfficeFont = FALSE;
	m_strOfficeFont = _T("Tahoma");
	m_bUseStandardFont = TRUE;
	m_bShowShadow = TRUE;
	m_bEmbossedDisabledText = TRUE;
	m_nShadowOptions = 0;

	m_bThemedStatusBar = FALSE;
	m_bThemedCheckBox = FALSE;

	m_iconsInfo.bUseFadedIcons = FALSE;
	m_iconsInfo.bIconsWithShadow = FALSE;
	m_iconsInfo.bUseDisabledIcons = FALSE;
	m_iconsInfo.bOfficeStyleDisabledIcons = FALSE;

	m_nAnimationSteps = 10;
	m_nAnimationTime = 100;
	m_bThickCheckMark = FALSE;
	m_bClearTypeTextQuality = FALSE;

	m_systemTheme = xtpSystemThemeDefault;
	m_bShowKeyboardCues = -1;

	m_nSplitButtonDropDownWidth = 11;
	m_nPopupBarTextPadding = 4;
	m_nSplitButtonPopupWidth = 20;

	m_bSelectImageInPopupBar = FALSE;
	m_bEnableAnimation = FALSE;

	m_bOffice2007Padding = FALSE;

	m_bOffsetPopupLabelText = TRUE;

	m_nEditHeight = 0;
	m_bAutoResizeIcons = FALSE;

	m_bWrapCaptionBelowText = TRUE;


	XTPImageManager()->RefreshAll();

	m_pGalleryPaintManager = new CXTPControlGalleryPaintManager(this);


	m_pfnSetLayeredWindowAttributes = NULL;
	//init layered function  (for Win98 compatible)
	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (hLib)
	{
		m_pfnSetLayeredWindowAttributes = (PVOID) ::GetProcAddress(hLib, "SetLayeredWindowAttributes");
	}
}

XTPCurrentSystemTheme CXTPPaintManager::GetCurrentSystemTheme()
{
	if (m_systemTheme == xtpSystemThemeDefault)
		return XTPColorManager()->IsLunaColorsDisabled() ? xtpSystemThemeUnknown :
			XTPColorManager()->GetCurrentSystemTheme();

	if (m_systemTheme == xtpSystemThemeAuto)
		return XTPColorManager()->GetWinThemeWrapperTheme();

	return m_systemTheme;
}
void CXTPPaintManager::SetLunaTheme(XTPCurrentSystemTheme systemTheme)
{
	m_systemTheme = systemTheme;
	RefreshMetrics();

}

COLORREF CXTPPaintManager::GetXtremeColor(UINT nIndex)
{
	if (nIndex > XPCOLOR_LAST) return nIndex;
	return m_arrColor[nIndex];
}

void CXTPPaintManager::SetColors(int cElements, CONST INT* lpaElements, CONST COLORREF* lpaRgbValues)
{
	for (int i = 0; i < cElements; i++)
		m_arrColor[lpaElements[i]].SetStandardValue(lpaRgbValues[i]);
}


CXTPPaintManager::~CXTPPaintManager()
{
	delete m_pGalleryPaintManager;
}

XTP_COMMANDBARS_ICONSINFO* CXTPPaintManager::GetIconsInfo()
{
	return &m_iconsInfo;
}

void CXTPPaintManager::RefreshMetrics()
{
	UpdateFonts();

	RefreshXtremeColors();

	CXTPColorManager* pColorManager = XTPColorManager();

	for (int i = 0; i <= XPCOLOR_LAST; i++)
		m_arrColor[i].SetStandardValue(pColorManager->GetColor(i));

	m_clrShadowFactor = 0;
	m_clrStatusTextColor = GetXtremeColor(COLOR_BTNTEXT);

	m_clrFloatingGripper.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	m_clrFloatingGripperText.SetStandardValue(GetXtremeColor(COLOR_3DHIGHLIGHT));

	m_clrDisabledIcon.SetStandardValue(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_3DSHADOW));

	m_themeStatusBar.CloseTheme();

	if (m_bThemedStatusBar)
	{
		m_themeStatusBar.OpenThemeData(NULL, L"STATUS");

		if (m_themeStatusBar.IsAppThemed())
			m_themeStatusBar.GetThemeColor(SP_PANE, 0, TMT_TEXTCOLOR, &m_clrStatusTextColor);
	}

	m_themeButton.CloseTheme();
	if (m_bThemedCheckBox)
	{
		m_themeButton.OpenTheme(NULL, L"BUTTON");
	}

	m_pGalleryPaintManager->RefreshMetrics();
}

int CXTPPaintManager::GetControlHeight() const
{
	return max(22, m_nTextHeight);
}

CXTPPaintManager* CXTPPaintManager::CreateTheme(XTPPaintTheme paintTheme)
{
	CXTPPaintManager* pInstance = NULL;

	if (paintTheme == xtpThemeOfficeXP) pInstance = new CXTPOfficeTheme();
	else if (paintTheme == xtpThemeOffice2003) pInstance = new CXTPOffice2003Theme();
	else if (paintTheme == xtpThemeNativeWinXP) pInstance = new CXTPNativeXPTheme();
	else if (paintTheme == xtpThemeWhidbey) pInstance = new CXTPWhidbeyTheme();
#ifdef _XTP_INCLUDE_RIBBON
	else if (paintTheme == xtpThemeOffice2007) pInstance = new CXTPOffice2007Theme();
	else if (paintTheme == xtpThemeRibbon) pInstance = new CXTPRibbonTheme();
#endif
	else pInstance = new CXTPDefaultTheme();

	ASSERT(pInstance);

	pInstance->m_themeCurrent = paintTheme;

	return pInstance;
}

void CXTPPaintManager::SetTheme(XTPPaintTheme paintTheme)
{
	SetCustomTheme(CreateTheme(paintTheme));
	s_pInstance->m_themeCurrent = paintTheme;
}

void CXTPPaintManager::SetCustomTheme(CXTPPaintManager* pTheme)
{
	if (s_pInstance != NULL)
		s_pInstance->InternalRelease();

	s_pInstance = pTheme;

	s_pInstance->m_themeCurrent = xtpThemeCustom;
	s_pInstance->RefreshMetrics();
}


void CXTPPaintManager::Done()
{
	if (s_pInstance != NULL)
		s_pInstance->InternalRelease();
	s_pInstance = NULL;
}



void CXTPPaintManager::Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clr)
{
	CXTPPenDC pen (*pDC, clr);
	CXTPBrushDC brush (*pDC, clr);

	CPoint pts[3];
	pts[0] = pt0;
	pts[1] = pt1;
	pts[2] = pt2;
	pDC->Polygon(pts, 3);
}

void CXTPPaintManager::Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush)
{
	if (nBrush != -1) pDC->FillSolidRect(rc, GetXtremeColor(nBrush));
	if (nPen != nBrush && nPen != -1) pDC->Draw3dRect(rc, GetXtremeColor(nPen), GetXtremeColor(nPen));
}

void CXTPPaintManager::Draw3dRect(CDC* pDC, CRect rc, int nTopLeft, int nBottomRight)
{
	pDC->Draw3dRect(&rc, GetXtremeColor(nTopLeft), GetXtremeColor(nBottomRight));
}

void CXTPPaintManager::_DrawCheckMark(CDC* pDC, CRect rc, COLORREF clr, BOOL bLayoutRTL /*= FALSE*/)
{
	CRect rcCheck(CPoint(rc.left + rc.Width()/2 - 10, rc.top + rc.Height()/2 - 10), CSize(20, 20));

	CXTPPenDC pen(*pDC, clr);

	if (bLayoutRTL)
	{
		pDC->MoveTo(rcCheck.left + 6, rcCheck.top + 7);
		pDC->LineTo(rcCheck.left + 10, rcCheck.top + 11);
		pDC->LineTo(rcCheck.left + 13, rcCheck.top + 8);

		pDC->MoveTo(rcCheck.left + 6, rcCheck.top + 8);
		pDC->LineTo(rcCheck.left + 10, rcCheck.top + 12);
		pDC->LineTo(rcCheck.left + 13, rcCheck.top + 9);
	}
	else
	{
		pDC->MoveTo(rcCheck.left + 6, rcCheck.top + 9);
		pDC->LineTo(rcCheck.left + 8, rcCheck.top + 11);
		pDC->LineTo(rcCheck.left + 13, rcCheck.top + 6);

		pDC->MoveTo(rcCheck.left + 6, rcCheck.top + 10);
		pDC->LineTo(rcCheck.left + 8, rcCheck.top + 12);
		pDC->LineTo(rcCheck.left + 13, rcCheck.top + 7);
	}
}

AFX_INLINE COLORREF Mix(CDC* pDC, int x, int y, COLORREF clrBorder, COLORREF clrFace, double a)
{
	COLORREF clr = clrFace;

	if (a < 0)
		a = -a;
	else
	{
		clr = pDC->GetPixel(x, y);
	}
	int r = int(GetRValue(clrBorder) + a * double(GetRValue(clr) -  GetRValue(clrBorder)));
	int g = int(GetGValue(clrBorder) + a * double(GetGValue(clr) -  GetGValue(clrBorder)));
	int b = int(GetBValue(clrBorder) + a * double(GetBValue(clr) -  GetBValue(clrBorder)));

	return RGB(r, g, b);
}

void CXTPPaintManager::AlphaEllipse(CDC* pDC, CRect rc, COLORREF clrBorder, COLORREF clrFace)
{
	ASSERT(rc.Width() == rc.Height());
	double x0 = double(rc.left + rc.right - 1) / 2;
	double y0 = double(rc.top + rc.bottom - 1) / 2;
	double radius = double(rc.right - rc.left - 1) / 2 - 0.25;

	for (int x = rc.left; x < rc.right; x++)
	for (int y = rc.top; y < rc.bottom; y++)
	{
		double distance = sqrt(pow(x0 - x, 2) + pow(y0 - y, 2));

		if (distance >= radius - 1 && distance <= radius + 1)
		{
			pDC->SetPixel(x, y, Mix(pDC, x, y, clrBorder, clrFace, distance - radius));
		}
		else if (distance < radius - 1)
		{
			pDC->SetPixel(x, y, clrFace);
		}
	}
}

void CXTPPaintManager::DrawRadioMark(CDC* pDC, CRect rcCheck, COLORREF clr)
{
	CPoint pt(rcCheck.CenterPoint());
	CRect rcRadio(pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);

	AlphaEllipse(pDC, rcRadio, clr, clr);
}

void CXTPPaintManager::DrawCheckMark(CDC* pDC, CRect rc, COLORREF clr)
{
	BOOL bLayoutRTL = CXTPDrawHelpers::IsContextRTL(pDC);

	_DrawCheckMark(pDC, rc, clr, bLayoutRTL);
	if (m_bThickCheckMark)
	{
		rc.OffsetRect(0, 1);
		_DrawCheckMark(pDC, rc, clr, bLayoutRTL);
	}
}

void CXTPPaintManager::DrawComboExpandMark(CDC* pDC, CRect rc, COLORREF clr)
{
	CPoint pt(rc.CenterPoint());
	int nWidth = max(2, rc.Width() / 2 - 4);

	Triangle(pDC, CPoint(pt.x - nWidth, pt.y - nWidth/2), CPoint(pt.x + nWidth, pt.y - nWidth/2), CPoint (pt.x , pt.y - nWidth/2 + nWidth), clr);
}

void CXTPPaintManager::HorizontalLine(CDC* pDC, int x0, int y, int x1, COLORREF clrPen)
{
	pDC->FillSolidRect(x0, y, x1 - x0, 1, clrPen);
}

void CXTPPaintManager::VerticalLine(CDC* pDC, int x, int y0, int y1, COLORREF clrPen)
{
	pDC->FillSolidRect(x, y0, 1, y1 - y0, clrPen);
}

void CXTPPaintManager::Line(CDC* pDC, CPoint p0, CPoint p1)
{
	pDC->MoveTo(p0);
	pDC->LineTo(p1);
}
void CXTPPaintManager::Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen)
{
	CXTPPenDC pen(*pDC, GetXtremeColor(nPen));
	pDC->MoveTo(x0, y0);
	pDC->LineTo(x1, y1);
}
void CXTPPaintManager::Pixel(CDC* pDC, int x, int y, int nPen)
{
	pDC->SetPixel(x, y, GetXtremeColor(nPen));
}

void CXTPPaintManager::StripMnemonics(CString& strClear)
{
	CXTPDrawHelpers::StripMnemonics(strClear);
}


void CXTPPaintManager::SplitString(const CString& str, CString& strFirstRow, CString& strSecondRow)
{
	strFirstRow = str;
	strSecondRow.Empty();

	int nIndex = str.Find(_T(' '));
	if (nIndex < 1)
		return;

	int nCenter = str.GetLength() / 2;
	for (int i = 0; i < nCenter - 1; i++)
	{
		ASSERT(nCenter - i >= 0 && nCenter + i + 1 < str.GetLength());

		if (str[nCenter - i] == _T(' '))
		{
			strFirstRow = str.Left(nCenter - i);
			strSecondRow = str.Mid(nCenter - i + 1);
			return;
		}

		if (str[nCenter + i + 1] == _T(' '))
		{
			strFirstRow = str.Left(nCenter + i + 1);
			strSecondRow = str.Mid(nCenter + i + 2);
			return;
		}
	}

}

#define XTP_DTT_COMPOSITED      (1UL << 13)     // Draws text with antialiased alpha (needs a DIB section)
#define XTP_DTT_TEXTCOLOR       (1UL << 0)

void DrawTextApi(CDC* pDC, const CString& str, LPRECT lpRect, UINT format, BOOL bComposited)
{
	if (!bComposited)
	{
		pDC->DrawText(str, lpRect, format);
	}
	else
	{
		USES_CONVERSION;

		CXTPWinThemeWrapper wrapper;
		wrapper.OpenTheme(0, L"GLOBALS");

		if (!wrapper.IsAppThemed())
		{
			pDC->DrawText(str, lpRect, format);
			return;
		}

		CRect rcBuffer(lpRect);

		HDC hDC = 0;
		HPAINTBUFFER pb = wrapper.BeginBufferedPaint(pDC->GetSafeHdc(), rcBuffer, XTP_BPBF_TOPDOWNDIB, 0, &hDC);

		if (hDC != NULL)
		{
			::BitBlt(hDC, rcBuffer.left, rcBuffer.top, rcBuffer.Width(), rcBuffer.Height(), pDC->GetSafeHdc(), rcBuffer.left, rcBuffer.top, SRCCOPY);

			XTP_UX_DTTOPTS op;
			op.dwSize = sizeof(op);
			op.dwFlags = XTP_DTT_COMPOSITED | XTP_DTT_TEXTCOLOR;
			op.crText = pDC->GetTextColor();


			CXTPFontDC font(CDC::FromHandle(hDC), pDC->GetCurrentFont());

			wrapper.DrawThemeTextEx(hDC, 0, 0, T2CW((LPTSTR)(LPCTSTR)str), -1,
				format,  rcBuffer, &op);
		}

		wrapper.EndBufferedPaint(pb, TRUE);
	}
}


void CXTPPaintManager::DrawControlText3(CDC* pDC, CXTPControl* pButton, const CString& str, const CString& strClear, CRect rcText, BOOL bVert, BOOL bCentered)
{
	BOOL bEmbossed = !pButton->GetEnabled() && m_bEmbossedDisabledText;
	BOOL bShowKeyboardCues = pButton->GetParent()->IsKeyboardCuesVisible();

	if (m_bShowKeyboardCues > -1)
		bShowKeyboardCues = m_bShowKeyboardCues;

	int nFormat = DT_SINGLELINE | (bCentered ? DT_CENTER : DT_LEFT);

	if (!bVert)
	{
		if (bEmbossed)
		{
			COLORREF clrText = pDC->GetTextColor();
			pDC->SetTextColor(GetXtremeColor(COLOR_3DHIGHLIGHT));
			pDC->DrawText(strClear, rcText + CPoint(1, 1), nFormat | DT_VCENTER | DT_NOPREFIX);
			pDC->SetTextColor(clrText);
		}

		if (bShowKeyboardCues)
			DrawTextApi(pDC, str, &rcText, nFormat | DT_VCENTER, pButton->HasDwmCompositedRect());
		else
			DrawTextApi(pDC, strClear, &rcText, nFormat | DT_VCENTER | DT_NOPREFIX, pButton->HasDwmCompositedRect());
	}
	else
	{
		if (bEmbossed)
		{
			COLORREF clrText = pDC->GetTextColor();
			pDC->SetTextColor(GetXtremeColor(COLOR_3DHIGHLIGHT));
			pDC->DrawText(strClear, rcText + CPoint(1, 1), nFormat | DT_NOPREFIX | DT_NOCLIP);
			pDC->SetTextColor(clrText);
		}

		pDC->DrawText(strClear, &rcText, nFormat | DT_NOPREFIX | DT_NOCLIP);
	}
}

void CXTPPaintManager::DrawDropDownGlyph(CDC* pDC, CXTPControl* /*pControl*/, CPoint pt, BOOL bSelected, BOOL bPopuped, BOOL bEnabled, BOOL bVert)
{
	COLORREF clr = GetRectangleTextColor(bSelected, FALSE, bEnabled, FALSE, bPopuped, xtpBarTypeNormal, xtpBarTop);

	if (bVert)
	{
		Triangle(pDC, CPoint(pt.x + 2 , pt.y - 2), CPoint(pt.x + 2, pt.y + 2), CPoint (pt.x, pt.y), clr);
	}
	else
	{
		Triangle(pDC, CPoint(pt.x - 2 , pt.y - 1), CPoint(pt.x + 2, pt.y - 1), CPoint (pt.x, pt.y + 1), clr);
	}
}

CSize CXTPPaintManager::DrawControlText2(CDC* pDC, CXTPControl* pButton, CRect rcText, BOOL bDraw, BOOL bVert, BOOL bTrangled)
{
	CString str = pButton->GetCaption();
	CString strClear(str);
	StripMnemonics(strClear);

	CString strFirstRow, strSecondRow;
	if (m_bWrapCaptionBelowText)
		SplitString(str, strFirstRow, strSecondRow);
	else strFirstRow = str;

	CString strFirstRowClear(strFirstRow), strSecondRowClear(strSecondRow);
	StripMnemonics(strFirstRowClear);
	StripMnemonics(strSecondRowClear);


	if (!bDraw)
	{
		CSize szFirstRow = pDC->GetTextExtent(strClear.IsEmpty() ? _T(" ") : strFirstRow);
		CSize szSecondRow = pDC->GetTextExtent(strClear.IsEmpty() ? _T(" ") : strSecondRow);

		if (bTrangled) szSecondRow.cx += 8;
		CSize sz(max(szFirstRow.cx, szSecondRow.cx), 24);
		return bVert ? CSize(sz.cy, sz.cx) : sz;
	}

	if (!bVert)
	{
		CRect rcFirstRowText(rcText.left, rcText.top, rcText.right, rcText.CenterPoint().y + 1);
		CRect rcSecondRowText(rcText.left, rcText.CenterPoint().y, rcText.right, rcText.bottom);

		DrawControlText3(pDC, pButton, strFirstRow, strFirstRowClear, rcFirstRowText, bVert, TRUE);

		int nWidth = pDC->GetTextExtent(strSecondRowClear).cx + (bTrangled ? 9 : 0);
		rcSecondRowText.left += (rcText.Width() - nWidth) /2;
		DrawControlText3(pDC, pButton, strSecondRow, strSecondRowClear, rcSecondRowText, bVert, FALSE);

		if (bTrangled)
		{
			CPoint pt = CPoint(strSecondRow.IsEmpty() ? rcText.CenterPoint().x - 1:
				rcSecondRowText.left + nWidth - 4, rcSecondRowText.CenterPoint().y);
			DrawDropDownGlyph(pDC, pButton, pt, pButton->GetSelected(), pButton->GetPopuped(), pButton->GetEnabled(), bVert);
		}
	}
	else
	{
		CRect rcSecondRowText(rcText.left + 1, rcText.top, rcText.CenterPoint().x + 1, rcText.bottom);
		CRect rcFirstRowText(rcText.CenterPoint().x, rcText.top, rcText.right - 1, rcText.bottom);

		CSize szText = pDC->GetTextExtent (strFirstRowClear);
		rcFirstRowText.left = rcFirstRowText.right - (rcFirstRowText.Width() - szText.cy + 1) / 2;
		rcFirstRowText.top = rcFirstRowText.top + (rcFirstRowText.Height() - szText.cx + 1) / 2;
		DrawControlText3(pDC, pButton, strFirstRow, strFirstRowClear, rcFirstRowText, bVert, FALSE);

		szText = pDC->GetTextExtent (strSecondRowClear);
		szText.cx += (bTrangled ? 8 : 0);
		CRect rcSecondRow(rcSecondRowText);
		rcSecondRowText.left = rcSecondRowText.right - (rcSecondRowText.Width() - szText.cy + 1) / 2;
		rcSecondRowText.top = rcSecondRowText.top + (rcSecondRowText.Height() - szText.cx + 1) / 2;
		DrawControlText3(pDC, pButton, strSecondRow, strSecondRowClear, rcSecondRowText, bVert, FALSE);

		if (bTrangled)
		{
			CPoint pt = CPoint(rcSecondRow.CenterPoint().x - 2, rcSecondRowText.top + szText.cx - 4);
			DrawDropDownGlyph(pDC, pButton, pt, pButton->GetSelected(), pButton->GetPopuped(), pButton->GetEnabled(), bVert);
		}
	}
	return 0;
}

CSize CXTPPaintManager::DrawControlText(CDC* pDC, CXTPControl* pButton, CRect rcText, BOOL bDraw, BOOL bVert, BOOL bCentered, BOOL bTriangled)
{
	CString str = pButton->GetCaption();

	CString strClear(str);
	StripMnemonics(strClear);

	if (!bDraw)
	{
		CSize sz = pDC->GetTextExtent(strClear.IsEmpty() ? _T(" ") : strClear);
		if (bTriangled) sz.cx += m_bOffice2007Padding ? 8 : 10;
		return bVert ? CSize(sz.cy, sz.cx) : sz;
	}

	if (!bVert)
	{
		int nWidth = pDC->GetTextExtent(strClear).cx + (bTriangled ? 5 : 0);

		if (bCentered)
		{
			rcText.left += (rcText.Width() - nWidth) /2;
		}

		DrawControlText3(pDC, pButton, str, strClear, rcText, bVert, FALSE);

		if (bTriangled)
		{
			CPoint pt = CPoint(rcText.left + nWidth, rcText.CenterPoint().y);
			DrawDropDownGlyph(pDC, pButton, pt, pButton->GetSelected(), pButton->GetPopuped(), pButton->GetEnabled(), bVert);
		}
	}
	else
	{
		CRect rc(rcText);
		CSize szText = pDC->GetTextExtent (strClear);
		szText.cx += (bTriangled ? 5 : 0);
		rcText.left = rcText.right - (rcText.Width() - szText.cy + 1) / 2;
		if (bCentered)
		{
			rcText.top = rcText.top + (rcText.Height() - szText.cx + 1) / 2;
		}

		DrawControlText3(pDC, pButton, str, strClear, rcText, bVert, FALSE);

		if (bTriangled)
		{
			CPoint pt = CPoint(rc.CenterPoint().x, rcText.top + szText.cx - 1);
			DrawDropDownGlyph(pDC, pButton, pt, pButton->GetSelected(), pButton->GetPopuped(), pButton->GetEnabled(), bVert);
		}
	}
	return 0;
}

void CXTPPaintManager::UseOfficeFont(bool bUseOfficeFont)
{
	m_bUseOfficeFont = bUseOfficeFont;
	RefreshMetrics();
}

BOOL CXTPPaintManager::FontExists(LPCTSTR strFaceName)
{
	return CXTPDrawHelpers::FontExists(strFaceName);
}

void CXTPPaintManager::SetCommandBarsFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardFont /*= FALSE*/)
{
	m_bUseStandardFont = bUseStandardFont;

	if (!pLogFont)
		return;

	if (m_bClearTypeTextQuality && XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		pLogFont->lfQuality = 5;
	}

	m_fontRegular.DeleteObject();
	m_fontRegularBold.DeleteObject();
	m_fontVert.DeleteObject();
	m_fontVertBold.DeleteObject();
	m_fontIconBold.DeleteObject();

	// see if the Tahoma font was found.
	BOOL bTahomaExists = FontExists(m_strOfficeFont);

	// see if the system font is set to the Tahoma font, if so set the office font flag to true.
	bool bTahomaLF = m_strOfficeFont.CompareNoCase(pLogFont->lfFaceName) == 0;
	if (bTahomaLF)
	{
		m_bUseOfficeFont = true;
	}

	// set the office font if applicable.
	//BOOL bUseSystemFont = (pLogFont->lfCharSet > SYMBOL_CHARSET);
	if (bUseStandardFont)
	{
		if (m_bUseOfficeFont && bTahomaExists)
			STRCPY_S(pLogFont->lfFaceName, LF_FACESIZE, m_strOfficeFont);
	}

	m_fontRegular.CreateFontIndirect (pLogFont);

	int nWeight = pLogFont->lfWeight;

	pLogFont->lfWeight = FW_BOLD;
	m_fontRegularBold.CreateFontIndirect (pLogFont);


	pLogFont->lfOrientation = 900;
	pLogFont->lfEscapement = 2700;
	pLogFont->lfWeight = nWeight;
	STRCPY_S(pLogFont->lfFaceName, LF_FACESIZE, CXTPDrawHelpers::GetVerticalFontName(m_bUseOfficeFont));

	m_fontVert.CreateFontIndirect (pLogFont);

	pLogFont->lfWeight = FW_BOLD;
	m_fontVertBold.CreateFontIndirect (pLogFont);

	CWindowDC dc (NULL);
	CXTPFontDC font(&dc, &m_fontRegular);
	m_nTextHeight = dc.GetTextExtent(_T(" "), 1).cy + 6;
}

void CXTPPaintManager::UpdateFonts()
{

	CNonClientMetrics ncm;

	if (m_bUseStandardFont)
	{
		CLogFont lf;

		lf.lfHeight = ncm.lfMenuFont.lfHeight < 0 ? min(-11, ncm.lfMenuFont.lfHeight) : ncm.lfMenuFont.lfHeight;
		lf.lfWeight = ncm.lfMenuFont.lfWeight;
		lf.lfItalic = ncm.lfMenuFont.lfItalic;
		lf.lfCharSet = ncm.lfMenuFont.lfCharSet;

		STRCPY_S(lf.lfFaceName, LF_FACESIZE, ncm.lfMenuFont.lfFaceName);

		if ((_tcsicmp(lf.lfFaceName, _T("Segoe UI")) == 0) &&
			XTPSystemVersion()->IsClearTypeTextQualitySupported() && FontExists(lf.lfFaceName))
		{
			lf.lfQuality = 6;
		}

		SetCommandBarsFontIndirect(&lf, TRUE);
	}

	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));

	if (m_bClearTypeTextQuality && XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		lfIcon.lfQuality = 6;
		ncm.lfSmCaptionFont.lfQuality = 6;
		ncm.lfStatusFont.lfQuality = 6;
	}

	m_fontSmCaption.SetStandardFont(&ncm.lfSmCaptionFont);

	VERIFY(m_fontIcon.SetStandardFont(&lfIcon));

	lfIcon.lfWeight = FW_BOLD;
	VERIFY(m_fontIconBold.SetStandardFont(&lfIcon));

	m_fontToolTip.SetStandardFont(&ncm.lfStatusFont);
}

void CXTPPaintManager::DrawControlEntry(CDC* pDC, CXTPControl* pButton)
{
	DrawRectangle(pDC, pButton->GetRect(), pButton->GetSelected(), pButton->GetPressed(), pButton->GetEnabled(), pButton->GetChecked(),
		pButton->GetPopuped(), pButton->GetParent()->GetType(), pButton->GetParent()->GetPosition());
}

COLORREF CXTPPaintManager::GetControlTextColor(CXTPControl* pButton)
{
	BOOL bEnabled = pButton->GetType() == xtpControlSplitButtonPopup ? (pButton->GetStyle() != xtpButtonIconAndCaptionBelow ?
		IsControlCommandEnabled(pButton->GetEnabled()) : IsControlDropDownEnabled(pButton->GetEnabled())) : pButton->GetEnabled();

	return GetRectangleTextColor(pButton->GetSelected(), pButton->GetPressed(), bEnabled, pButton->GetChecked(),
		pButton->GetPopuped(), pButton->GetParent()->GetType(), pButton->GetParent()->GetPosition());
}

int CXTPPaintManager::GetPopupBarGripperWidth(CXTPCommandBar* pBar)
{
	if (pBar->GetType() != xtpBarTypePopup)
		return 0;

	int nDoubleGripper = pBar->IsPopupBar() && ((CXTPPopupBar*)pBar)->IsDoubleGripper() ? 2 : 1;
	CSize szImage(GetPopupBarImageSize(pBar));

	return szImage.cx * nDoubleGripper + 1;
}

COLORREF CXTPPaintManager::GetControlEditBackColor(CXTPControl* pControl)
{
	return GetXtremeColor(pControl->GetEnabled() ? COLOR_WINDOW : COLOR_3DFACE);
}

CSize CXTPPaintManager::DrawControlEdit(CDC* pDC, CXTPControlEdit* pControlEdit, BOOL bDraw)
{
	if (bDraw)
	{
		if (pControlEdit->GetParent()->GetType() == xtpBarTypePopup)
		{
			DrawControlPopupParent(pDC, pControlEdit, TRUE);
		}
		else
		{
			CRect rc = pControlEdit->GetRect();
			CRect rcLabelText(rc);
			rcLabelText.left += 2;
			BOOL bEnabled = pControlEdit->GetEnabled();

			if (pControlEdit->IsImageVisible())
			{
				CSize szIcon = pControlEdit->GetIconSize();

				CXTPImageManagerIcon* pImage = pControlEdit->GetImage(szIcon.cx);
				szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();

				CPoint pt = CPoint(rc.left + 3, rc.CenterPoint().y - szIcon.cy/2);
				DrawImage(pDC, pt, szIcon, pImage, FALSE, FALSE, bEnabled, FALSE, FALSE);

				rcLabelText.left = rc.left + szIcon.cx + 6;
			}

			if (pControlEdit->IsCaptionVisible())
			{
				CXTPFontDC fontLabel(pDC, GetRegularFont());

				pDC->SetTextColor(GetRectangleTextColor(FALSE, FALSE, bEnabled, FALSE, FALSE, pControlEdit->GetParent()->GetType(), pControlEdit->GetParent()->GetPosition()));
				DrawControlText(pDC, pControlEdit, rcLabelText, TRUE, FALSE, FALSE, FALSE);
			}
		}
		return 0;
	}

	BOOL bPopupBar = pControlEdit->GetParent()->GetType() == xtpBarTypePopup;
	CString strCaption(pControlEdit->GetCaption());

	StripMnemonics(strCaption);

	CXTPFontDC font(pDC, GetIconFont());
	int nHeight = m_nEditHeight > 0 ? m_nEditHeight : pDC->GetTextExtent(_T(" "), 1).cy + 7;

	font.SetFont(GetRegularFont());

	BOOL bCaptionVisible = pControlEdit->IsCaptionVisible();

	if (bCaptionVisible && m_nEditHeight == 0)
		nHeight = max(nHeight, pDC->GetTextExtent(_T(" "), 1).cy + 7);

	int nCaptionWidth = pDC->GetTextExtent(strCaption).cx + 5;

	if (bPopupBar)
	{
		int nGripperWidth = GetPopupBarGripperWidth(pControlEdit->GetParent());
		pControlEdit->SetLabelWidth(nCaptionWidth + nGripperWidth + m_nPopupBarTextPadding);

		return CSize(pControlEdit->GetWidth() + nGripperWidth, nHeight);
	}

	BOOL bImageVisible = pControlEdit->IsImageVisible();

	int nLabelWidth = (bImageVisible ? pControlEdit->GetIconSize().cx  + 4 : 0) +
		(bCaptionVisible ? nCaptionWidth : 0);

	if (bImageVisible)
		nHeight = max(nHeight, pControlEdit->GetButtonSize().cy);

	pControlEdit->SetLabelWidth(nLabelWidth);

	return CSize(pControlEdit->GetWidth(), nHeight);
}

CSize CXTPPaintManager::DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pControlCombo, BOOL bDraw)
{
	if (bDraw)
	{
		if (pControlCombo->GetParent()->GetType() == xtpBarTypePopup)
		{
			DrawControlPopupParent(pDC, pControlCombo, TRUE);
		}
		else
		{
			CRect rc = pControlCombo->GetRect();
			CRect rcLabelText(rc);
			rcLabelText.left += 2;
			BOOL bEnabled = pControlCombo->GetEnabled();

			if (pControlCombo->IsImageVisible())
			{
				CSize szIcon = pControlCombo->GetIconSize();

				CXTPImageManagerIcon* pImage = pControlCombo->GetImage(szIcon.cx);
				szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();

				CPoint pt = CPoint(rc.left + 3, rc.CenterPoint().y - szIcon.cy/2);
				DrawImage(pDC, pt, szIcon, pImage, FALSE, FALSE, bEnabled, FALSE, FALSE);

				rcLabelText.left = rc.left + szIcon.cx + 6;
			}

			if (pControlCombo->IsCaptionVisible())
			{
				CXTPFontDC fontLabel(pDC, GetRegularFont());

				pDC->SetTextColor(GetRectangleTextColor(FALSE, FALSE, bEnabled, FALSE, FALSE, pControlCombo->GetParent()->GetType(), pControlCombo->GetParent()->GetPosition()));
				DrawControlText(pDC, pControlCombo, rcLabelText, TRUE, FALSE, FALSE, FALSE);
			}
		}
		return 0;
	}

	BOOL bPopupBar = pControlCombo->GetParent()->GetType() == xtpBarTypePopup;
	CString strCaption(pControlCombo->GetCaption());

	StripMnemonics(strCaption);

	CXTPFontDC font(pDC, GetIconFont());
	int nHeight = m_nEditHeight > 0 ? m_nEditHeight : max(20, pDC->GetTextExtent(_T(" "), 1).cy + 7);

	pControlCombo->SetThumbWidth(max(15, MulDiv(nHeight, 9, 14) + 1));

	font.SetFont(GetRegularFont());

	BOOL bCaptionVisible = pControlCombo->IsCaptionVisible();

	if (bCaptionVisible && m_nEditHeight == 0)
		nHeight = max(nHeight, pDC->GetTextExtent(_T(" "), 1).cy + 7);

	int nCaptionWidth = pDC->GetTextExtent(strCaption).cx + 5;

	if (bPopupBar)
	{
		int nGripperWidth = GetPopupBarGripperWidth(pControlCombo->GetParent());
		pControlCombo->SetLabelWidth(nCaptionWidth + nGripperWidth + m_nPopupBarTextPadding);

		return CSize(pControlCombo->GetWidth() + nGripperWidth, nHeight);
	}

	BOOL bImageVisible = pControlCombo->IsImageVisible();

	int nLabelWidth = (bImageVisible ? pControlCombo->GetIconSize().cx  + 4 : 0) +
		(bCaptionVisible ? nCaptionWidth : 0);

	if (bImageVisible)
		nHeight = max(nHeight, pControlCombo->GetParent()->GetButtonSize().cy);

	pControlCombo->SetLabelWidth(nLabelWidth);

	return CSize(pControlCombo->GetWidth(), nHeight);
}

CSize CXTPPaintManager::DrawControl(CDC* pDC, CXTPControl* pButton, BOOL bDraw)
{
	if (pButton->GetType() == xtpControlEdit)
	{
		return DrawControlEdit(pDC, (CXTPControlEdit*)pButton, bDraw);
	}

	if (pButton->GetType() == xtpControlComboBox)
	{
		return DrawControlComboBox(pDC, (CXTPControlComboBox*)pButton, bDraw);
	}

	return (pButton->GetParent()->GetType() == xtpBarTypePopup) ?
		DrawControlPopupParent(pDC, pButton, bDraw): DrawControlToolBarParent(pDC, pButton, bDraw);
}

void CXTPPaintManager::FillDockBar(CDC* pDC, CXTPDockBar* pBar)
{
	pDC->FillSolidRect(CXTPClientRect((CWnd*)pBar), GetXtremeColor(COLOR_3DFACE));
}

void CXTPPaintManager::FillStatusBar(CDC* pDC, CXTPStatusBar* pBar)
{
	if (m_bThemedStatusBar && m_themeStatusBar.IsAppThemed())
	{
		m_themeStatusBar.DrawThemeBackground(pDC->GetSafeHdc(), 0, 0, CXTPClientRect((CWnd*)pBar), 0);
	}
	else
	{
		pDC->FillSolidRect(CXTPClientRect(pBar), GetXtremeColor(COLOR_3DFACE));
	}
}

CFont* CXTPPaintManager::GetCommandBarFont(CXTPCommandBar* pBar, BOOL bBold)
{
	return (pBar->GetPosition() == xtpBarLeft || pBar->GetPosition() == xtpBarRight) ? (bBold ? &m_fontVertBold: &m_fontVert) : (bBold ? &m_fontRegularBold : &m_fontRegular);

}

CSize CXTPPaintManager::DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam)
{
	if (controlType == xtpButtonExpandToolbar)
	{
		if (!bDraw)
		{
			CSize szBar = *(CSize*)lpParam;
			if (!IsVerticalPosition(pBar->GetPosition()))
				pButton->SetRect(CRect(szBar.cx - (11 + 2), 2, szBar.cx - 2, szBar.cy - 2));
			else
				pButton->SetRect(CRect(2, szBar.cy - (11 + 2), szBar.cx - 2, szBar.cy - 2));
		}
		else
		{
			DrawControlEntry(pDC, pButton);
			COLORREF clrText = GetControlTextColor(pButton);

			BOOL bHiddenExists = *(BOOL*)lpParam;
			CRect rcButton = pButton->GetRect();
			if (!IsVerticalPosition(pBar->GetPosition()))
			{
				CPoint pt = CPoint(rcButton.left + rcButton.Width()/2, rcButton.bottom - 6);
				Triangle(pDC, CPoint(pt.x -2 , pt.y - 1), CPoint(pt.x + 2, pt.y - 1), CPoint (pt.x, pt.y + 1), clrText);
				if (bHiddenExists)
				{
					CXTPPenDC pen(*pDC, clrText);
					pDC->MoveTo(rcButton.left + 2, rcButton.top + 4); pDC->LineTo(rcButton.left + 4, rcButton.top + 6);     pDC->LineTo(rcButton.left + 1, rcButton.top + 9);
					pDC->MoveTo(rcButton.left + 3, rcButton.top + 4); pDC->LineTo(rcButton.left + 5, rcButton.top + 6);     pDC->LineTo(rcButton.left + 2, rcButton.top + 9);

					pDC->MoveTo(rcButton.left + 6, rcButton.top + 4); pDC->LineTo(rcButton.left + 8, rcButton.top + 6);     pDC->LineTo(rcButton.left + 5, rcButton.top + 9);
					pDC->MoveTo(rcButton.left + 7, rcButton.top + 4); pDC->LineTo(rcButton.left + 9, rcButton.top + 6);     pDC->LineTo(rcButton.left + 6, rcButton.top + 9);
				}
			}
			else
			{
				CPoint pt = CPoint(rcButton.left + 4, rcButton.top + 5);
				Triangle(pDC, CPoint(pt.x +2 , pt.y - 2), CPoint(pt.x + 2, pt.y + 2), CPoint (pt.x, pt.y), clrText);

				if (bHiddenExists)
				{
					CXTPPenDC pen(*pDC, clrText);
					pDC->MoveTo(rcButton.right - 8, rcButton.top + 2); pDC->LineTo(rcButton.right - 6, rcButton.top + 4);   pDC->LineTo(rcButton.right - 3, rcButton.top + 1);
					pDC->MoveTo(rcButton.right - 8, rcButton.top + 3); pDC->LineTo(rcButton.right - 6, rcButton.top + 5);   pDC->LineTo(rcButton.right - 3, rcButton.top + 2);

					pDC->MoveTo(rcButton.right - 8, rcButton.top + 6); pDC->LineTo(rcButton.right - 6, rcButton.top + 8);   pDC->LineTo(rcButton.right - 3, rcButton.top + 5);
					pDC->MoveTo(rcButton.right - 8, rcButton.top + 7); pDC->LineTo(rcButton.right - 6, rcButton.top + 9);   pDC->LineTo(rcButton.right - 3, rcButton.top + 6);
				}

			}
		}
	}
	else if (controlType == xtpButtonExpandFloating)
	{
		DrawControlEntry(pDC, pButton);
		CRect rcButton = pButton->GetRect();
		COLORREF clrText = GetControlTextColor(pButton);

		if (!pButton->GetSelected() && !pButton->GetPopuped())
			clrText = m_clrFloatingGripperText;
		CPoint pt = CPoint(rcButton.left + rcButton.Width()/2, rcButton.top + rcButton.Height()/2);
		Triangle(pDC, CPoint(pt.x -3 , pt.y - 2), CPoint(pt.x + 3, pt.y - 2), CPoint (pt.x, pt.y + 1), clrText);
	}
	else if (controlType == xtpButtonHideFloating)
	{
		CFont font;
		font.CreatePointFont(MulDiv(80, 96, pDC->GetDeviceCaps(LOGPIXELSX)), _T("Marlett"));
		CFont* pFont = pDC->SelectObject(&font);
		DrawControlEntry(pDC, pButton);
		if (!pButton->GetPressed() && !pButton->GetSelected())
			pDC->SetTextColor(m_clrFloatingGripperText);
		else
			pDC->SetTextColor(GetControlTextColor(pButton));

		pDC->DrawText(_T("r"), 1, pButton->GetRect(), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pDC->SelectObject(pFont);
	}
	else if (controlType == xtpButtonExpandMenu)
	{
		if (bDraw)
		{
			DrawControlEntry(pDC, pButton);
			CRect rcButton = pButton->GetRect();
			int nCenter = rcButton.CenterPoint().x;
			CXTPPenDC pen(pDC->GetSafeHdc(), 0);

			pDC->MoveTo(nCenter - 2, rcButton.top + 2); pDC->LineTo(nCenter, rcButton.top + 4);     pDC->LineTo(nCenter + 3, rcButton.top + 1);
			pDC->MoveTo(nCenter - 2, rcButton.top + 3); pDC->LineTo(nCenter, rcButton.top + 5);     pDC->LineTo(nCenter + 3, rcButton.top + 2);

			pDC->MoveTo(nCenter - 2, rcButton.top + 6); pDC->LineTo(nCenter, rcButton.top + 8);     pDC->LineTo(nCenter + 3, rcButton.top + 5);
			pDC->MoveTo(nCenter - 2, rcButton.top + 7); pDC->LineTo(nCenter, rcButton.top + 9);     pDC->LineTo(nCenter + 3, rcButton.top + 6);

		}
		return CSize(0, 12);
	}
	return 0;
}

CSize CXTPPaintManager::DrawControlText(CDC* pDC, CXTPControl* pButton, CRect rcText, BOOL bDraw, BOOL bTriangled, CSize szButton, BOOL bDrawImage)
{
	BOOL bVert = pButton->GetParent()->GetPosition() == xtpBarRight || pButton->GetParent()->GetPosition() == xtpBarLeft;

	if (!bVert)
	{
		if (bDrawImage)
		{
			rcText.left += m_bOffice2007Padding ? szButton.cx : szButton.cx - 1;

			CSize sz = DrawControlText(pDC, pButton, &rcText, bDraw, bVert, FALSE, bTriangled);

			return CSize((m_bOffice2007Padding ? 4 : 3) + sz.cx + szButton.cx, max(sz.cy + 6, szButton.cy));
		}
		else
		{
			CSize sz = DrawControlText(pDC, pButton, &rcText, bDraw, bVert, TRUE, bTriangled);
			return CSize(8 + sz.cx, max(sz.cy + 6, szButton.cy));
		}
	}
	else
	{
		if (bDrawImage)
		{
			rcText.top += szButton.cy;

			CSize sz = DrawControlText(pDC, pButton, &rcText, bDraw, bVert, FALSE, bTriangled);

			return CSize(max(sz.cx + 6, szButton.cx), 3 + sz.cy + szButton.cx);
		}
		else
		{
			CSize sz = DrawControlText(pDC, pButton, &rcText, bDraw, bVert, TRUE, bTriangled);
			return CSize(max(sz.cx + 6, szButton.cx), 8 + sz.cy);

		}
	}
}

CRect CXTPPaintManager::GetSplitButtonCommandRect(CXTPControl* pButton)
{
	CRect rc(pButton->GetRect());
	CXTPCommandBar* pParent = pButton->GetParent();

	if (pParent->GetType() != xtpBarTypePopup && pButton->GetStyle() == xtpButtonIconAndCaptionBelow)
	{
		int nSplitDropDownHeight = GetSplitDropDownHeight();

		BOOL bVert = pParent->GetPosition() == xtpBarLeft ||
			pParent->GetPosition() == xtpBarRight;

		return bVert ? CRect(rc.left + nSplitDropDownHeight, rc.top, rc.right, rc.bottom) :
			CRect(rc.left, rc.top, rc.right, rc.bottom - nSplitDropDownHeight);
	}

	int nSize = pParent->GetType() == xtpBarTypePopup ? m_nSplitButtonPopupWidth : m_nSplitButtonDropDownWidth + 1;
	return CRect(rc.left, rc.top, rc.right - nSize, rc.bottom);
}

CSize CXTPPaintManager::DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled)
{
	if (bDraw)
	{
		if (m_bThemedCheckBox && m_themeButton.IsAppThemed())
		{
			int nState = 0;
			if (!bEnabled)
				nState = 3;
			else if (bSelected && bPressed)
				nState = 2;
			else if (bSelected)
				nState = 1;

			if (bChecked == 1)
				nState += 4;

			if (bChecked == 2)
				nState += 8;

			m_themeButton.DrawThemeBackground(pDC->GetSafeHdc(), BP_CHECKBOX, nState + 1, rc, 0);

		}
		else
		{
			pDC->DrawFrameControl(rc, DFC_BUTTON, DFCS_BUTTONCHECK |
				(!bEnabled || bChecked == 2 ? DFCS_INACTIVE : 0) | (bChecked ? DFCS_CHECKED : 0) |
				(bPressed ? DFCS_PUSHED : 0));
		}
	}

	return CSize(13, 13);
}

CSize CXTPPaintManager::DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled)
{
	if (bDraw)
	{
		if (m_bThemedCheckBox && m_themeButton.IsAppThemed())
		{
			int nState = 0;
			if (!bEnabled)
				nState = 3;
			else if (bSelected && bPressed)
				nState = 2;
			else if (bSelected)
				nState = 1;

			if (bChecked == 1)
				nState += 4;

			if (bChecked == 2)
				nState += 8;

			m_themeButton.DrawThemeBackground(pDC->GetSafeHdc(), BP_RADIOBUTTON, nState + 1, rc, 0);

		}
		else
		{
			pDC->DrawFrameControl(rc, DFC_BUTTON, DFCS_BUTTONRADIO |
				(!bEnabled || bChecked == 2 ? DFCS_INACTIVE : 0) | (bChecked ? DFCS_CHECKED : 0) |
				(bPressed ? DFCS_PUSHED : 0));
		}
	}

	return CSize(13, 13);
}

CSize CXTPPaintManager::GetControlSize(CXTPControl* pControl, CSize sz, BOOL bVert)
{
	if (bVert)
	{
		if (pControl->GetWidth() > 0)
			sz.cy = pControl->GetWidth();

		if (pControl->GetHeight() > 0)
			sz.cx = pControl->GetHeight();
	}
	else
	{
		if (pControl->GetWidth() > 0)
			sz.cx = pControl->GetWidth();

		if (pControl->GetHeight() > 0)
			sz.cy = pControl->GetHeight();
	}

	return sz;
}

CSize CXTPPaintManager::GetPopupBarImageSize(CXTPCommandBar* pBar)
{
	CSize szIcon = pBar->GetIconSize();
	CSize szImage(max(szIcon.cx + 4, m_nTextHeight + 4), max(szIcon.cy + 6, m_nTextHeight));
	return szImage;
}

void CXTPPaintManager::DrawPopupBarText(CDC* pDC, const CString& strText, CRect& rcText, UINT nFlags, BOOL bDraw, BOOL bSelected, BOOL bEnabled)
{
	if (bDraw && !bEnabled && !bSelected && m_bEmbossedDisabledText)
	{
		COLORREF clrText = pDC->GetTextColor();
		pDC->SetTextColor(GetXtremeColor(COLOR_3DHIGHLIGHT));
		pDC->DrawText(strText, rcText + CPoint(1, 1), DT_SINGLELINE | DT_VCENTER | nFlags);
		pDC->SetTextColor(clrText);
	}

	pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_VCENTER | (!bDraw ? DT_CALCRECT : nFlags));
}

CSize CXTPPaintManager::DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw)
{
	CXTPCommandBar* pParent = pButton->GetParent();

	CRect rcText(0, 0, 0, 0), rcShortcut(0, 0, 0, 0);
	CRect rcButton = pButton->GetRect();
	XTPControlType controlType = pButton->GetType();
	XTPButtonStyle buttonStyle = pButton->GetStyle();
	CString strCaption = pButton->GetCaption();

	BOOL bDrawImage = (buttonStyle == xtpButtonAutomatic && controlType != xtpControlPopup) ||
		(buttonStyle != xtpButtonCaption);

	CSize szImage(GetPopupBarImageSize(pParent));

	BOOL bDoubleGripper = pParent->IsPopupBar() ? ((CXTPPopupBar*)pParent)->IsDoubleGripper() : FALSE;
	int nGripperWidth = bDoubleGripper ? szImage.cx * 2 : szImage.cx;

	BOOL bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled();
	if (controlType == xtpControlSplitButtonPopup && bEnabled == TRUE_SPLITDROPDOWN)
		bEnabled = FALSE;

	if (pButton->GetType() == xtpControlLabel && !m_bOffsetPopupLabelText)
		nGripperWidth = 0;
	if (pButton->GetType() == xtpControlLabel && !bDrawImage)
		szImage.cy = m_nTextHeight;

	if (bDraw)
	{
		rcText.SetRect(rcButton.left + nGripperWidth + m_nPopupBarTextPadding, rcButton.top, rcButton.right, rcButton.bottom);
		rcShortcut.SetRect(rcButton.left + nGripperWidth + 8, rcButton.top, rcButton.right - 19, rcButton.bottom);

		if (controlType == xtpControlButton && pButton->GetShortcutText().GetLength() != 0)
			rcText.right -= pDC->GetTextExtent(pButton->GetShortcutText()).cx + 19;
	}

	BOOL bShowKeyboardCues = pParent->IsKeyboardCuesVisible();

	if (!bShowKeyboardCues)
		StripMnemonics(strCaption);

	if (buttonStyle == xtpButtonCaptionAndDescription && !pButton->GetDescription().IsEmpty())
	{
		{
			CXTPFontDC font(pDC, GetRegularBoldFont());
			if (bDraw)
			{
				rcShortcut.bottom = rcText.bottom = rcText.top + pDC->GetTextExtent(_T(" "), 1).cy + 10;
			}
			DrawPopupBarText(pDC, strCaption, rcText, DT_END_ELLIPSIS | (!bShowKeyboardCues ? DT_NOPREFIX : 0), bDraw, bSelected, bEnabled);

			if (controlType == xtpControlButton && pButton->GetShortcutText().GetLength() != 0)
				DrawPopupBarText(pDC, pButton->GetShortcutText(), rcShortcut, DT_RIGHT, bDraw, bSelected, bEnabled);
		}

		if (bDraw)
		{
			CRect rcDescription(rcText.left, rcText.bottom - 2, rcButton.right - 10, rcButton.bottom);
			pDC->DrawText(pButton->GetDescription(),rcDescription, DT_WORDBREAK | DT_TOP | DT_NOPREFIX);
		}
		else
		{
			CSize szDescription = pDC->GetTextExtent(pButton->GetDescription());
			CRect rcDescription(0, 0, (szDescription.cx + 20) / 2, 0);
			pDC->DrawText(_T("1\n2"), rcDescription, DT_CALCRECT | DT_WORDBREAK);

			int nWidth = nGripperWidth + m_nPopupBarTextPadding + rcText.Width() +  rcShortcut.Width() + 32;

			return GetControlSize(pButton, CSize(max(nWidth, nGripperWidth + m_nPopupBarTextPadding + szDescription.cx / 2 + 32),
				max(szImage.cy, rcDescription.Height() + rcText.Height() + 19)), FALSE);
		}
		return 0;
	}

	DrawPopupBarText(pDC, strCaption, rcText, DT_END_ELLIPSIS | (!bShowKeyboardCues ? DT_NOPREFIX : 0), bDraw, bSelected, bEnabled);

	if (controlType == xtpControlButton && pButton->GetShortcutText().GetLength() != 0)
		DrawPopupBarText(pDC, pButton->GetShortcutText(), rcShortcut, DT_RIGHT, bDraw, bSelected, bEnabled);

	return GetControlSize(pButton, CSize(nGripperWidth + m_nPopupBarTextPadding + rcText.Width() +  rcShortcut.Width() + 32, szImage.cy), FALSE);
}

CSize CXTPPaintManager::GetIconSize(CXTPControl* pButton)
{
	XTPButtonStyle buttonStyle = pButton->GetStyle();

	CSize szIcon = buttonStyle == xtpButtonIconAndCaptionBelow
		&& pButton->GetParent()->GetType() != xtpBarTypePopup ?
		pButton->GetParent()->GetLargeIconSize(TRUE) : pButton->GetIconSize();

	return szIcon;
}

int CXTPPaintManager::GetSplitDropDownHeight() const
{
	int nTextHeight = m_nEditHeight > 0 ? m_nTextHeight : max(22, m_nTextHeight + 3);
	int nSplitDropDownHeight = (nTextHeight * 2 - 17);

	return nSplitDropDownHeight;
}

CSize CXTPPaintManager::DrawControlToolBarParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw)
{
	CXTPFontDC font(pDC, GetCommandBarFont(pButton->GetParent(), pButton->IsItemDefault()));

	CRect rcButton = bDraw ? pButton->GetRect() : CXTPEmptyRect();
	XTPControlType controlType = pButton->GetType();
	XTPButtonStyle buttonStyle = pButton->GetStyle();

	CSize szIcon = GetIconSize(pButton);
	CSize szButton = pButton->GetButtonSize();

	BOOL bVert = pButton->GetParent()->GetPosition() == xtpBarRight || pButton->GetParent()->GetPosition() == xtpBarLeft;

	BOOL bDrawImage = (buttonStyle == xtpButtonAutomatic && controlType != xtpControlPopup) ||
		(buttonStyle == xtpButtonIcon) || (buttonStyle == xtpButtonIconAndCaption) || (buttonStyle == xtpButtonIconAndCaptionBelow);

	CXTPImageManagerIcon* pImage = (bDrawImage && pButton->GetIconId() != 0) ? pButton->GetImage(szIcon.cx) : NULL;
	bDrawImage = bDrawImage && (pImage != NULL);

	BOOL bDrawText = pButton->IsCaptionVisible();

	BOOL bPressed = pButton->GetPressed(), bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked(),
		bPopuped = pButton->GetPopuped();

	if (controlType == xtpControlSplitButtonPopup && bEnabled == TRUE_SPLITDROPDOWN)
		bEnabled = FALSE;

	if (controlType == xtpControlCheckBox || controlType == xtpControlRadioButton)
	{
		CSize szCheckBox = controlType == xtpControlRadioButton ?
			DrawControlRadioButtonMark(pDC, rcButton, FALSE, bSelected, bPressed, bChecked, bEnabled) :
			DrawControlCheckBoxMark(pDC, rcButton, FALSE, bSelected, bPressed, bChecked, bEnabled);

		if (bDraw)
		{
			pDC->SetTextColor(GetRectangleTextColor(FALSE, FALSE, bEnabled, FALSE, FALSE, pButton->GetParent()->GetType(), pButton->GetParent()->GetPosition()));

			CRect rcCheck;
			if (!bVert)
			{
				rcCheck = CRect(CPoint(rcButton.left + 3, rcButton.CenterPoint().y - szCheckBox.cy/2), szCheckBox);
				rcButton.left += szCheckBox.cx + 5;
			}
			else
			{
				rcCheck = CRect(CPoint(rcButton.CenterPoint().x - szCheckBox.cx / 2, rcButton.top + 3), szCheckBox);
				rcButton.top += szCheckBox.cy + 5;
			}

			if (controlType == xtpControlRadioButton)
				DrawControlRadioButtonMark(pDC, rcCheck, TRUE, bSelected, bPressed, bChecked, bEnabled);
			else
				DrawControlCheckBoxMark(pDC, rcCheck, TRUE, bSelected, bPressed, bChecked, bEnabled);

			DrawControlText(pDC, pButton, rcButton, bDraw, FALSE, szButton, FALSE);

			if (IsKeyboardSelected(bSelected))
			{
				pDC->SetTextColor(GetXtremeColor(COLOR_BTNTEXT));
				pDC->SetBkColor(GetXtremeColor(COLOR_BTNFACE));
				pDC->DrawFocusRect(rcButton);
			}

			return GetControlSize(pButton, szCheckBox, bVert);
		}
		else
		{
			CSize sz = DrawControlText(pDC, pButton, rcButton, bDraw, FALSE, szButton, FALSE);
			if (bVert)
				sz.cy += szCheckBox.cy + 5;
			else
				sz.cx += szCheckBox.cx + 5;
			return GetControlSize(pButton, sz, bVert);
		}
	}

	if (bDraw)
	{
		if (controlType != xtpControlSplitButtonPopup)
		{
			if (!pButton->IsTransparent())
			{
				DrawControlEntry(pDC, pButton);
			}
		}
		else
		{
			DrawSplitButtonFrame(pDC, pButton, rcButton);
		}
		pDC->SetTextColor(GetControlTextColor(pButton));
	}

	if (buttonStyle == xtpButtonIconAndCaptionBelow)
	{
		BOOL bTriangled = pButton->GetCommandBar() != NULL;

		int nTextHeight = GetControlHeight();
		int nSplitDropDownHeight = GetSplitDropDownHeight();

		CSize szButton(max(42, nTextHeight * 2 - 3),
			m_nEditHeight > 0 ? m_nEditHeight * 3 : nSplitDropDownHeight + szIcon.cy + 7);
		if (bVert) szButton = CSize(szButton.cy, szButton.cx);

		if (bDraw)
		{
			if (bDrawImage)
			{
				CPoint pt = bVert ? CPoint(rcButton.left + 4 + nSplitDropDownHeight, rcButton.CenterPoint().y - szIcon.cy / 2) :
					CPoint(rcButton.CenterPoint().x - szIcon.cx / 2, rcButton.top + 3);
				DrawImage(pDC, pt, CSize(szIcon.cx, 0), pImage, bSelected, bPressed, bEnabled, bChecked, bPopuped);
			}

			CRect rcText = rcButton;
			if (bVert)
				rcText.right = rcText.left + nSplitDropDownHeight;
			else
			{
				rcText.bottom -= 1;
				rcText.top = rcText.bottom - nSplitDropDownHeight;
			}

			DrawControlText2(pDC, pButton, rcText, TRUE, bVert, bTriangled);
		}
		else
		{
			CSize szText = DrawControlText2(pDC, pButton, CXTPEmptyRect(), FALSE, bVert, bTriangled);

			if (bVert)
			{
				szButton.cy = max(szButton.cy, szText.cy + 6);
			}
			else
			{
				szButton.cx = max(szButton.cx, szText.cx + 5);
			}
		}
		return GetControlSize(pButton, szButton, bVert);
	}

	if (pButton->GetParent()->IsTextBelowIcons() && bDrawImage)
	{
		CXTPFontDC fontHorizontal (pDC, pButton->IsItemDefault() ? &m_fontRegularBold : &m_fontRegular);
		BOOL bTriangled = controlType == xtpControlPopup;

		if (bDraw)
		{
			if (controlType == xtpControlSplitButtonPopup) rcButton.right -= m_nSplitButtonDropDownWidth;

			CPoint pt = CPoint(rcButton.CenterPoint().x - szIcon.cx / 2, rcButton.top + 4);
			DrawImage(pDC, pt, CSize(szIcon.cx, 0), pImage, bSelected, bPressed, bEnabled, bChecked, bPopuped);

			CRect rcText = rcButton;
			rcText.top += szButton.cy - 4;

			DrawControlText(pDC, pButton, rcText, TRUE, FALSE, TRUE, bTriangled);
		}
		else
		{
			CSize szText = DrawControlText(pDC, pButton, CXTPEmptyRect(), FALSE, FALSE, TRUE, bTriangled);

			szButton.cy += szText.cy;
			szButton.cx = max(szButton.cx, szText.cx + 10);

			if (pButton->GetType() == xtpControlSplitButtonPopup)
				szButton.cx += 11;

			szButton.cx = max(szButton.cx, szButton.cy);
		}
		return GetControlSize(pButton, szButton, bVert);
	}

	if (bDraw && pImage)
	{
		szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();

		CPoint pt = !bVert ? CPoint(rcButton.left + 3, rcButton.CenterPoint().y - szIcon.cy / 2) :
			CPoint(rcButton.CenterPoint().x - szIcon.cx / 2, rcButton.top + 3);

		if (controlType == xtpControlSplitButtonPopup) pt.x = rcButton.left + 3;
		if (controlType == xtpControlPopup && bDrawImage && !bDrawText) pt.x = rcButton.left + 3;
		if ((controlType == xtpControlButtonPopup || controlType == xtpControlButton) && bDrawImage && !bDrawText)
		{
			if (!bVert)
				pt.x = rcButton.CenterPoint().x - szIcon.cx / 2;
			else
				pt.y = rcButton.CenterPoint().y - szIcon.cy / 2;
		}

		pImage->m_bDrawComposited = pButton->HasDwmCompositedRect();
		DrawImage(pDC, pt, szIcon, pImage, bSelected, bPressed, bEnabled, bChecked, bPopuped);
		pImage->m_bDrawComposited = FALSE;
	}


	switch (controlType)
	{
	case xtpControlPopup:
		{
			if (!bDrawImage)
			{
				CSize sz = DrawControlText(pDC, pButton, rcButton, bDraw, bVert, TRUE, !bDraw || pButton->GetParent()->GetType() != xtpBarTypeMenuBar);
				return GetControlSize(pButton, CSize(sz.cx + 6, sz.cy + 6), bVert);
			}
			else
			{
				if (!bDrawText)
				{
					CPoint pt = CPoint(rcButton.right - 7, rcButton.CenterPoint().y);
					Triangle(pDC, CPoint(pt.x - 2 , pt.y - 1), CPoint(pt.x + 2, pt.y - 1), CPoint (pt.x, pt.y + 1), pDC->GetTextColor());
					return GetControlSize(pButton, CSize(szButton.cy + 7, szButton.cy), bVert);
				}
				return GetControlSize(pButton, DrawControlText(pDC, pButton, rcButton, bDraw, pButton->GetParent()->GetType() != xtpBarTypeMenuBar, szButton, bDrawImage), bVert);
			}
		}
	case xtpControlSplitButtonPopup:
		{
			if (!bDrawImage) szButton = CSize(22, 22);

			if (!bDrawText)
				return GetControlSize(pButton, CSize(szButton.cx + m_nSplitButtonDropDownWidth, szButton.cy), bVert);

			rcButton.right -= m_nSplitButtonDropDownWidth;

			CSize sz = DrawControlText(pDC, pButton, rcButton, bDraw, FALSE, szButton, bDrawImage);
			return GetControlSize(pButton, CSize(sz.cx + m_nSplitButtonDropDownWidth, sz.cy), bVert);

		}
	case xtpControlButtonPopup:
	case xtpControlButton:
	case xtpControlLabel:
	case xtpControlCheckBox:
	case xtpControlGallery:
		{
			if (!bDrawText)
				return GetControlSize(pButton, szButton, bVert);

			if (!bDrawImage) szButton = CSize(22, szButton.cy);

			return GetControlSize(pButton, DrawControlText(pDC, pButton, rcButton, bDraw, FALSE, szButton, bDrawImage), bVert);
		}
	}
	return 0;
}

CRect CXTPPaintManager::GetCommandBarBorders(CXTPCommandBar* /*pBar*/)
{
	return CRect(2, 2, 2, 2);
}

void CXTPPaintManager::GradientFill(CDC* pDC, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz, LPCRECT lpRectClip)
{
	XTPDrawHelpers()->GradientFill(pDC, lpRect, crFrom, crTo, bHorz, lpRectClip);

}

void CXTPPaintManager::DrawStatusBarGripper(CDC* pDC, CRect rcClient)
{
	if (m_bThemedStatusBar && m_themeStatusBar.IsAppThemed())
	{
		CRect rcGripper(rcClient.right - 18, rcClient.top, rcClient.right, rcClient.bottom);
		m_themeStatusBar.DrawThemeBackground(pDC->GetSafeHdc(), SP_GRIPPER, 0, &rcGripper, 0);
	}
	else
	{
		CXTPPenDC penBorder(*pDC, GetXtremeColor(COLOR_3DSHADOW));
		int i;
		for (i = 0; i < 3; i++)
		{

			pDC->MoveTo(rcClient.Width() - 3 -i * 4, rcClient.Height() - 2);
			pDC->LineTo(rcClient.Width() - 1, rcClient.Height() - 4 - i * 4);

			pDC->MoveTo(rcClient.Width() - 4 -i * 4, rcClient.Height() - 2);
			pDC->LineTo(rcClient.Width() - 1, rcClient.Height() - 4 - i * 4 - 1);
		}

		CXTPPenDC penBorderWhite(*pDC, GetXtremeColor(COLOR_BTNHIGHLIGHT));
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(rcClient.Width() - 5 -i * 4, rcClient.Height() - 2);
			pDC->LineTo(rcClient.Width() - 1 , rcClient.Height() - 4 - i * 4 - 2);
		}
	}
}

void CXTPPaintManager::AnimateExpanding(CXTPCommandBar* pCommandBar, CDC* pDestDC, CDC* pSrcDC, BOOL bExpandDown)
{
	CXTPClientRect rc(pCommandBar);
	int nWidth = rc.Width();
	int nHeight = rc.Height();

	int nSteps = m_nAnimationSteps;
	int nAnimationTime = m_nAnimationTime;

	CXTPControls* pControls = pCommandBar->GetControls();

	for (int i = 0; i < nSteps; ++i)
	{
		double dPercent = double(i + 1) / nSteps;

		int nTopDest = 0;
		int nTopSrc = 0;
		BOOL bPrevExpanded = FALSE;

		if (bExpandDown)
		{
			for (int j = 0; j < pControls->GetCount(); ++j)
			{
				CXTPControl* pControl = pControls->GetAt(j);
				if (!pControl->IsVisible())
					continue;

				BOOL bExpanded = pControl->GetExpanded();

				if (bExpanded != bPrevExpanded)
				{
					int nTopSrcCurrent = pControl->GetRect().top;
					int nHeightDstr = int((bPrevExpanded ? dPercent : 1) * (nTopSrcCurrent - nTopSrc));

					pDestDC->StretchBlt(0, nTopDest, nWidth, nHeightDstr, pSrcDC,
						0, nTopSrc, nWidth, nTopSrcCurrent - nTopSrc, SRCCOPY);

					nTopDest = nTopDest + nHeightDstr;
					nTopSrc = nTopSrcCurrent;
				}

				bPrevExpanded = bExpanded;
			}

			int nTopSrcCurrent = nHeight;
			int nHeightDstr = int((bPrevExpanded ? dPercent : 1) * (nTopSrcCurrent - nTopSrc));

			pDestDC->StretchBlt(0, nTopDest, nWidth, nHeightDstr, pSrcDC,
				0, nTopSrc, nWidth, nTopSrcCurrent - nTopSrc, SRCCOPY);
		}
		else
		{
			for (int j = pControls->GetCount() - 1; j >= 0; j--)
			{
				CXTPControl* pControl = pControls->GetAt(j);
				if (!pControl->IsVisible())
					continue;

				BOOL bExpanded = pControl->GetExpanded();

				if (bExpanded != bPrevExpanded)
				{
					int nTopSrcCurrent = nHeight - pControl->GetRect().bottom;
					int nHeightDstr = int((bPrevExpanded ? dPercent : 1) * (nTopSrcCurrent - nTopSrc));

					pDestDC->StretchBlt(0, nHeight - nTopDest - nHeightDstr, nWidth, nHeightDstr, pSrcDC,
						0, nHeight - nTopSrcCurrent, nWidth, nTopSrcCurrent - nTopSrc, SRCCOPY);

					nTopDest = nTopDest + nHeightDstr;
					nTopSrc = nTopSrcCurrent;
				}

				bPrevExpanded = bExpanded;
			}

			int nTopSrcCurrent = nHeight;
			int nHeightDstr = int((bPrevExpanded ? dPercent : 1) * (nTopSrcCurrent - nTopSrc));

			pDestDC->StretchBlt(0, nHeight - nTopDest - nHeightDstr, nWidth, nHeightDstr, pSrcDC,
				0, nHeight - nTopSrcCurrent, nWidth, nTopSrcCurrent - nTopSrc, SRCCOPY);

		}

		Sleep(nAnimationTime / nSteps);
	}


}

void CXTPPaintManager::AlphaBlendU(PBYTE pDest, PBYTE pSrcBack, int cx, int cy, PBYTE pSrc, BYTE byAlpha)
{
	const BYTE byDiff = (BYTE)(255 - byAlpha);

	for (int i = 0; i < cx * cy; i++)
	{
		pDest[0] = (BYTE)((pSrcBack[0] * byDiff + pSrc[0] * byAlpha) >> 8);
		pDest[1] = (BYTE)((pSrcBack[1] * byDiff + pSrc[1] * byAlpha) >> 8);
		pDest[2] = (BYTE)((pSrcBack[2] * byDiff + pSrc[2] * byAlpha) >> 8);

		pDest += 4;
		pSrcBack += 4;
		pSrc += 4;
	}
}


void CXTPPaintManager::Animate(CDC* pDestDC, CDC* pSrcDC, CRect rc, XTPAnimationType animationType)
{
	int nSteps = m_nAnimationSteps;
	int nAnimationTime = m_nAnimationTime;

	switch (animationType)
	{
		case xtpAnimateSlide:
			{
				for (int i = 0; i < nSteps; ++i)
				{
					int nHeight = rc.Height() * (i + 1) / nSteps;

					pDestDC->BitBlt(0, 0, rc.Width(), nHeight, pSrcDC,
						0, rc.Height() - nHeight, SRCCOPY);

					Sleep(nAnimationTime / nSteps);
				}
			}
			break;

		case xtpAnimateUnfold:
			{

				for (int i = 0; i < nSteps; ++i)
				{
					CRect rcDest(rc.right - rc.Width() * (i + 1) / nSteps, rc.bottom - rc.Height() * (i + 1) / nSteps, rc.right, rc.bottom);

					pDestDC->BitBlt(0, 0, rcDest.Width(), rcDest.Height(), pSrcDC,
						rcDest.left, rcDest.top, SRCCOPY);

					Sleep(nAnimationTime / nSteps);
				}
			}
			break;

		case xtpAnimateFade:
			{
				const int cx = rc.Width();
				const int cy = rc.Height();

				BITMAPINFOHEADER BMI;
				// Fill in the header info.
				ZeroMemory (&BMI, sizeof(BMI));
				BMI.biSize = sizeof(BITMAPINFOHEADER);
				BMI.biWidth = cx;
				BMI.biHeight = cy;
				BMI.biPlanes = 1;
				BMI.biBitCount = 32;
				BMI.biCompression = BI_RGB;   // No compression

				BYTE * pSrcBits = NULL;
				HBITMAP hbmSrc = CreateDIBSection (NULL, (BITMAPINFO *)&BMI, DIB_RGB_COLORS, (void **)&pSrcBits, 0, 0l);

				BYTE * pSrcBackBits = NULL;
				HBITMAP hbmSrcBack = CreateDIBSection (NULL, (BITMAPINFO *)&BMI, DIB_RGB_COLORS, (void **)&pSrcBackBits, 0, 0l);

				BYTE * pDestBits = NULL;
				HBITMAP hbmDest = CreateDIBSection (NULL, (BITMAPINFO *)&BMI, DIB_RGB_COLORS, (void **)&pDestBits, 0, 0l);

				// Copy our source and destination bitmaps onto our DIBSections,
				// so we can get access to their bits using the BYTE *'s we passed into CreateDIBSection

				CDC dc;
				dc.CreateCompatibleDC(NULL);
				HBITMAP hbmpOld = (HBITMAP) ::SelectObject(dc, hbmSrc);
				::BitBlt(dc, 0, 0, cx, cy, pSrcDC->GetSafeHdc(), 0, 0, SRCCOPY);

				::SelectObject(dc, hbmSrcBack);
				::BitBlt(dc, 0, 0, cx, cy, pDestDC->GetSafeHdc (), 0, 0, SRCCOPY);

				DWORD dwTimePer = nAnimationTime / nSteps;

				::SelectObject(dc, hbmDest);
				for (int i = 1; i < nSteps; ++i)
				{
					DWORD dwTime = GetTickCount ();
					AlphaBlendU(pDestBits, pSrcBackBits, cx, cy, pSrcBits, (BYTE)(255 * i  / nSteps));
					pDestDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
					dwTime = GetTickCount () - dwTime;
					if (dwTime < dwTimePer)
					{
						Sleep(dwTimePer - dwTime);
					}
				}
				::SelectObject(dc, hbmpOld);
				DeleteObject(hbmSrc);
				DeleteObject(hbmSrcBack);
				DeleteObject(hbmDest);
			}

			break;
	}

}

void CXTPPaintManager::UpdateRTLCaption(CString& /*strCaption*/)
{
}


void CXTPPaintManager::DrawStatusBarPaneBorder(CDC* pDC, CRect rc, BOOL bGripperPane)
{
	if (m_bThemedStatusBar && m_themeStatusBar.IsAppThemed())
	{
		m_themeStatusBar.DrawThemeBackground(pDC->GetSafeHdc(), bGripperPane ? SP_GRIPPERPANE : SP_PANE, 0, rc, 0);
	}
	else
	{
		Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	}
}

void CXTPPaintManager::DrawStatusBarPaneEntry(CDC* pDC, CRect rcItem, CXTPStatusBarPane* pPane)
{
	BOOL bEnabled = !(pPane->GetStyle() & SBPS_DISABLED);
	BOOL bDrawText = bEnabled || pPane->GetStatusBar()->GetDrawDisabledText();

	if (pPane->GetBackgroundColor() != (COLORREF)-1)
	{
		CRect rcBackground(rcItem);

		if (!(pPane->GetStyle() & SBPS_NOBORDERS))
			rcBackground.DeflateRect(1, 1);

		pDC->FillSolidRect(rcBackground, pPane->GetBackgroundColor());
	}

	CXTPFontDC font(pDC, pPane->GetTextFont());
	rcItem.DeflateRect(pPane->GetMargins());
	CString str = pPane->GetText();

	CXTPImageManagerIcon* pIcon = pPane->GetImage();

	if (pPane->GetTextAlignment() != DT_LEFT)
	{
		int nWidth = pDC->GetTextExtent(str).cx;
		nWidth += pIcon ? pIcon->GetWidth() + (nWidth > 0 ? 2 : 0) : 0;

		if (rcItem.Width() > nWidth)
		{
			if (pPane->GetTextAlignment() == DT_RIGHT)
			{
				rcItem.left = rcItem.right - nWidth;
			}
			if (pPane->GetTextAlignment() == DT_CENTER)
			{
				rcItem.left = (rcItem.left + rcItem.right - nWidth) / 2;
			}
		}
	}

	if (pIcon)
	{
		if (rcItem.right >= rcItem.left + pIcon->GetWidth())
		{
			pIcon->Draw(pDC, CPoint(rcItem.left, (rcItem.top + rcItem.bottom - pIcon->GetHeight()) / 2), bEnabled ? pIcon->GetIcon() : pIcon->GetDisabledIcon());
		}
		rcItem.OffsetRect(pIcon->GetWidth() + 2, 0);
	}

	if (bDrawText)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(!bEnabled ? GetXtremeColor(COLOR_GRAYTEXT): pPane->GetTextColor() != (COLORREF)-1 ? pPane->GetTextColor() : m_clrStatusTextColor);
		pDC->DrawText(str, rcItem, DT_SINGLELINE | DT_VCENTER | DT_EXPANDTABS | DT_NOPREFIX);
	}
}


CSize CXTPPaintManager::DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw)
{
	if (pBar->GetPosition() == xtpBarFloating)
	{
		CRect rc;
		pBar->GetClientRect(&rc);

		ASSERT(pDC);
		CXTPFontDC font(pDC, GetSmCaptionFont());
		CSize sz = pDC->GetTextExtent(_T(" "), 1);

		rc.SetRect(3, 3, rc.right - 3, 3 + max(15, sz.cy));
		if (pDC && bDraw)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->FillSolidRect(rc, m_clrFloatingGripper);
			pDC->SetTextColor(m_clrFloatingGripperText);
			pDC->DrawText(pBar->GetTitle(), CRect(5, 3, rc.right - 2* rc.Size().cy, rc.bottom), DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
		}
		return rc.Size();

	}
	return 0;
}

void CXTPPaintManager::SetCommandBarRegion(CXTPCommandBar* /*pCommandBar*/)
{
}

BOOL CXTPPaintManager::IsFlatToolBar(CXTPCommandBar* pCommandBar)
{
	return pCommandBar->GetType() == xtpBarTypeMenuBar ? m_bFlatMenuBar : m_bFlatToolBar;
}

void CXTPPaintManager::DrawPopupBarGripper(CDC* /*pDC*/, int /*xPos*/, int /*yPos*/, int /*cx*/, int /*cy*/, BOOL /*bExpanded = FALSE*/)
{

}
void CXTPPaintManager::FillWorkspace(CDC* pDC, CRect rc, CRect /*rcExclude*/)
{
	pDC->FillSolidRect(rc, GetXtremeColor(COLOR_APPWORKSPACE));
}

void AFX_CDECL CXTPPaintManager::FillCompositeAlpha(CDC* pDC, CRect rc)
{
	CSize sz(rc.Width(), rc.Height());
	CBitmap bmp;
	LPDWORD lpBits;
	bmp.Attach(CXTPImageManager::Create32BPPDIBSection(pDC->GetSafeHdc(), sz.cx, sz.cy, (LPBYTE*)&lpBits));

	CXTPCompatibleDC dc(pDC, &bmp);

	CRect rcDestOrig(0, 0, sz.cx, sz.cy);

	dc.BitBlt(0, 0, sz.cx, sz.cy, pDC, rc.left, rc.top, SRCCOPY);

	for (int i = 0; i < sz.cx * sz.cy; i++)
	{
		lpBits[0] |= 0xFF000000;
		lpBits++;
	}

	pDC->BitBlt(rc.left, rc.top, sz.cx, sz.cy, &dc, 0, 0, SRCCOPY);
}

void CXTPPaintManager::DrawKeyboardTip(CDC* pDC, CXTPCommandBarKeyboardTip* pWnd, BOOL bSetRegion)
{
	if (!bSetRegion)
	{
		CXTPClientRect rc(pWnd);
		CXTPFontDC font(pDC, &m_fontToolTip);

		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_INFOBK));
		pDC->Draw3dRect(rc, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetXtremeColor(pWnd->m_bEnabled ? COLOR_INFOTEXT : COLOR_GRAYTEXT));
		pDC->DrawText(pWnd->m_strTip, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}

CSize CXTPPaintManager::GetAutoIconSize(BOOL bLarge) const
{
	if (m_bAutoResizeIcons)
	{
		int nHeight = GetControlHeight() - 4;
		if (bLarge) nHeight = nHeight * 2;

		nHeight = max(2, nHeight / 8);

		return CSize(nHeight * 8, nHeight * 8);
	}

	return bLarge ? CSize(32, 32) : CSize(16, 16);
}


