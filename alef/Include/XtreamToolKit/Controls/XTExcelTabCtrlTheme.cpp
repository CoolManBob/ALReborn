// XTExcelTabCtrlTheme.cpp: implementation of the CXTExcelTabCtrlTheme class.
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

#include "XTExcelTabCtrlTheme.h"
#include "XTDefines.h"
#include "XTUtil.h"
#include "XTGlobal.h"
#include "XTMemDC.h"
#include "XTVC50Helpers.h"
#include "XTExcelTabCtrl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_THEME_FACTORY(CXTExcelTabCtrlTheme)

/////////////////////////////////////////////////////////////////////////////
// CXTExcelTabCtrlTheme class
//

CXTExcelTabCtrlTheme::CXTExcelTabCtrlTheme()
{
}

CXTExcelTabCtrlTheme::~CXTExcelTabCtrlTheme()
{

}

void CXTExcelTabCtrlTheme::RefreshMetrics()
{
	CXTThemeManagerStyle ::RefreshMetrics();

	m_cy = ::GetSystemMetrics(SM_CYHSCROLL);
	m_cx = ::GetSystemMetrics(SM_CXHSCROLL)-1;

	m_clr3DFace.SetStandardValue(GetXtremeColor(COLOR_BTNFACE));
	m_clr3DFacePushed.SetStandardValue(GetXtremeColor(COLOR_BTNFACE));
	m_clr3DFaceHilite.SetStandardValue(GetXtremeColor(COLOR_BTNFACE));
	m_clrBtnText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrBtnTextPushed.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrBtnTextHilite.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrBtnTextGray.SetStandardValue(GetXtremeColor(COLOR_GRAYTEXT));
	m_clrWindow.SetStandardValue(GetXtremeColor(COLOR_WINDOW));
	m_clrWindowText.SetStandardValue(GetXtremeColor(COLOR_WINDOWTEXT));
	m_clr3DHilight.SetStandardValue(GetXtremeColor(COLOR_3DHILIGHT));
	m_clr3DShadow.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	m_clrBorder3DHilite.SetStandardValue(GetXtremeColor(COLOR_3DHILIGHT));
	m_clrBorder3DShadow.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
}

COLORREF CXTExcelTabCtrlTheme::GetTabBackColor(CXTTcbItem* pTcbItem) const
{
	return (pTcbItem->crTabBack == COLORREF_NULL) ? m_clr3DFace : pTcbItem->crTabBack;
}

COLORREF CXTExcelTabCtrlTheme::GetTabTextColor(CXTTcbItem* pTcbItem) const
{
	return (pTcbItem->crTabText == COLORREF_NULL) ? m_clrBtnText : pTcbItem->crTabText;
}

COLORREF CXTExcelTabCtrlTheme::GetSelTabBackColor(CXTTcbItem* pTcbItem) const
{
	return (pTcbItem->crTabSelBack == COLORREF_NULL) ? m_clrWindow : pTcbItem->crTabSelBack;
}

COLORREF CXTExcelTabCtrlTheme::GetSelTabTextColor(CXTTcbItem* pTcbItem) const
{
	return (pTcbItem->crTabSelText == COLORREF_NULL) ? m_clrWindowText : pTcbItem->crTabSelText;
}

//////////////////////////////////////////////////////////////////////////
//
//     Height ************************
//          \*| |                  |  *
//          *\| |xxxxxxxxxxxxxxxxxx|   *
//         *  | |         |        |    *
//        *---+-+         |        +-----*
//           |  \       Text
//         Base  \
//                Margin
//
//      Base = Height / 2
//      Margin = Base / 2
//

int CXTExcelTabCtrlTheme::DrawTab(CDC* pDC, CXTExcelTabCtrl* pTabCtrl, const CPoint& pt, bool bSelected, CXTTcbItem* pTcbItem)
{
	if (pDC == NULL || pTcbItem == NULL || !::IsWindow(pTabCtrl->GetSafeHwnd()))
		return -1;

	// Saves the current state of the device context, we will
	// restore the state when the method looses scope. This will
	// simplify selecting objects.
	const int nSavedDC = pDC->SaveDC();

	const int iHeight = m_cy;
	const int iBase = iHeight / 2;
	const int iWidth = iHeight + iHeight / 2; // 2 bases + 2 margins

	const bool bBottom = (pTabCtrl->GetTabStyle() & FTS_XT_BOTTOM) == FTS_XT_BOTTOM;

	pDC->SelectObject(pTabCtrl->GetTabFont(bSelected));
	const CSize szText = pDC->GetTextExtent(pTcbItem->szTabLabel);

	CRect rcText;
	rcText.left = pt.x + iBase + iBase / 2;
	rcText.top = pt.y + ((iHeight - szText.cy)/2)-1;
	rcText.right = rcText.left + szText.cx;
	rcText.bottom = rcText.top + szText.cy;

	COLORREF crBack = GetTabBackColor(pTcbItem);
	COLORREF crFore = GetTabTextColor(pTcbItem);

	if (bSelected)
	{
		crBack = GetSelTabBackColor(pTcbItem);
		crFore = GetSelTabTextColor(pTcbItem);
	}

	CPen penBack(PS_SOLID, 1, crBack);
	CPen penWindow(PS_SOLID, 1, m_clr3DHilight);
	CPen penShadow(PS_SOLID, 1, m_clr3DShadow);
	CPen penOutline(PS_SOLID, 1, m_clrBtnText);

	CBrush brush;
	brush.CreateSolidBrush(crBack);

	POINT points[] =
	{
		{ pt.x, pt.y + iHeight - 1 },
		{ pt.x + iBase - 1, pt.y               },
		{ pt.x + szText.cx + iWidth - iBase - 1, pt.y               },
		{ pt.x + szText.cx + iWidth - 1, pt.y + iHeight - 1 }
	};

	// swap vertical coordinates
	if (bBottom)
	{
		points[0].y = points[1].y;
		points[2].y = points[3].y;
		points[1].y = points[2].y;
		points[3].y = points[0].y;
	}

	pDC->SelectObject(&penOutline);
	pDC->SetBkColor(crBack);
	pDC->SelectObject(&brush);
	pDC->Polygon(points, 4);
	pDC->SetTextColor(crFore);
	pDC->DrawText(pTcbItem->szTabLabel, rcText, DT_CENTER);
	pDC->SelectObject(&penShadow);

	if (bSelected)
	{
		pDC->MoveTo(pt.x + iBase, points[1].y);
		pDC->LineTo(pt.x + iBase * 2 + szText.cx - 1, points[1].y);
		pDC->SelectObject(&penBack);
		pDC->MoveTo(pt.x + 1, points[0].y);
		pDC->LineTo(pt.x + szText.cx + iWidth - 1, points[0].y);
	}
	else
	{
		// highlight line on left
		pDC->SelectObject(&penWindow);
		pDC->MoveTo(pt.x + 1, points[0].y);
		pDC->LineTo(pt.x + 1 + iBase, points[0].y + iHeight);

		// shadow line on top
		pDC->SelectObject(&penShadow);
		pDC->MoveTo(pt.x, points[0].y);
		pDC->LineTo(pt.x + szText.cx + iWidth, points[0].y);

		// shadow line on bottom
		pDC->MoveTo(pt.x + iBase, points[1].y);
		pDC->LineTo(pt.x + szText.cx + iHeight - 1, points[1].y);
	}

	// Cleanup.
	pDC->RestoreDC(nSavedDC);
	brush.DeleteObject();

	return szText.cx + iWidth;
}

void CXTExcelTabCtrlTheme::DrawButtonBack(CDC* pDC, CRect& rect, CXTExcelTabCtrlButtonState& state)
{
	if (state.m_bPressed && state.m_bHilight)
	{
		pDC->Draw3dRect(rect, m_clrBorder3DShadow, m_clrBorder3DHilite);
	}
	else
	{
		pDC->Draw3dRect(rect, m_clrBorder3DHilite, m_clrBorder3DShadow);
	}

	rect.DeflateRect(1, 1);
	pDC->FillSolidRect(rect, m_clr3DFace);
}

void CXTExcelTabCtrlTheme::DrawButton(CDC* pDC, CXTExcelTabCtrl* pTabCtrl, CXTExcelTabCtrlButtonState& state)
{
	CRect r(state.m_rect);
	if (r.IsRectEmpty()) return;
	r.OffsetRect(0, -pTabCtrl->GetTabsRect().top);

	DrawButtonBack(pDC, r, state);

	COLORREF clrBtnText = m_clrBtnText;

	if (state.m_bEnabled)
	{
		if (state.m_bPressed)
		{
			clrBtnText = m_clrBtnTextPushed;
		}
		else if (state.m_bHilight)
		{
			clrBtnText = m_clrBtnTextHilite;
		}
	}
	else
	{
		clrBtnText = m_clrBtnTextGray;
	}

	CPen pen(PS_SOLID, 1, clrBtnText);

	CPen *pOldPen = pDC->SelectObject(&pen);

	int direction = 1 - ((state.m_IconType & 0xFF00) >> 8);
	int x = (direction < 0) ? r.right : r.left, y;

	const int arrow_cy = r.Width() / 2 + 2;
	const int arrow_cx = (arrow_cy + 1) / 2 + 1;
	const int top = r.top + (r.Height() - arrow_cy) / 2;

	x += (((r.Width() - arrow_cx) / 2) * direction);
	y = top;

	// draw arrow
	int height = arrow_cy;
	while (height > 0)
	{
		pDC->MoveTo(x, y);
		pDC->LineTo(x, y + height);
		y += 1;
		x += direction;
		height -= 2;
	}

	if (state.m_IconType & 0xFF)
	{
		x += (1 * direction);
		pDC->MoveTo(x, top);
		pDC->LineTo(x, top + arrow_cy);
	}

	pDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////
// CXTExcelTabCtrlThemeOfficeXP class
//

void CXTExcelTabCtrlThemeOfficeXP::RefreshMetrics()
{
	CXTExcelTabCtrlTheme::RefreshMetrics();

	m_clr3DFacePushed.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED));
	m_clr3DFaceHilite.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT));
	m_clrBtnText.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_TEXT));
	m_clrBtnTextPushed.SetStandardValue(GetXtremeColor(XPCOLOR_PUSHED_TEXT));
	m_clrBtnTextHilite.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT));
	m_clrBtnTextGray.SetStandardValue(GetXtremeColor(XPCOLOR_GRAYTEXT));
	m_clrBorder3DHilite.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
	m_clrBorder3DShadow.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
}

void CXTExcelTabCtrlThemeOfficeXP::DrawButtonBack(CDC* pDC, CRect& rect, CXTExcelTabCtrlButtonState& state)
{
	if (state.m_bEnabled && state.m_bHilight)
	{
		if (state.m_bPressed)
		{
			pDC->FillSolidRect(rect, m_clr3DFacePushed);
			pDC->Draw3dRect(rect, m_clrBorder3DHilite, m_clrBorder3DHilite);
		}
		else
		{
			pDC->FillSolidRect(rect, m_clr3DFaceHilite);
			pDC->Draw3dRect(rect, m_clrBorder3DHilite, m_clrBorder3DHilite);
		}
	}
	else
	{
		pDC->FillSolidRect(rect, m_clr3DFace);
		pDC->Draw3dRect(rect, m_clr3DFace, m_clr3DFace);
	}

	rect.DeflateRect(1, 1);
}

/////////////////////////////////////////////////////////////////////////////
// CXTExcelTabCtrlThemeOffice2003 class
//

void CXTExcelTabCtrlThemeOffice2003::RefreshMetrics()
{
	CXTExcelTabCtrlThemeOfficeXP::RefreshMetrics();

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
	case xtpSystemThemeOlive:
	case xtpSystemThemeSilver:
		m_clr3DFacePushed.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_PUSHED));
		m_clr3DFaceHilite.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT));
		m_clrBtnTextPushed.SetStandardValue(GetMSO2003Color(XPCOLOR_PUSHED_TEXT));
		m_clrBtnTextHilite.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_TEXT));
		m_clrBorder3DShadow.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		m_clrBorder3DHilite.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
		break;
	}
}

void CXTExcelTabCtrlThemeOffice2003::DrawButtonBack(CDC* pDC, CRect& rect, CXTExcelTabCtrlButtonState& state)
{
	if (state.m_bEnabled && state.m_bHilight)
	{
		if (state.m_bPressed)
		{
			XTPDrawHelpers()->GradientFill(pDC, &rect,
				XTPColorManager()->grcLunaPushed, FALSE);
		}
		else
		{
			XTPDrawHelpers()->GradientFill(pDC, &rect,
				XTPColorManager()->grcLunaSelected, FALSE);
		}

		pDC->Draw3dRect(&rect, m_clrBorder3DHilite, m_clrBorder3DHilite);
	}
	else
	{
		pDC->FillSolidRect(rect, m_clr3DFace);
		pDC->Draw3dRect(rect, m_clr3DFace, m_clr3DFace);
	}

	rect.DeflateRect(1, 1);
}
