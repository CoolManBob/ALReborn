// XTColorSelectorCtrlTheme.cpp: implementation of the XTColorSelectorCtrlTheme class.
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
#include "Common/XTPColorManager.h"
#include "XTColorSelectorCtrl.h"
#include "XTColorSelectorCtrlTheme.h"
#include "XTGlobal.h"
#include "XTFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CXTColorSelectorCtrlTheme

IMPLEMENT_THEME_FACTORY(CXTColorSelectorCtrlTheme)

CXTColorSelectorCtrlTheme::CXTColorSelectorCtrlTheme()
: m_crBorderHilight(COLORREF_NULL)
, m_crBorderShadow(COLORREF_NULL)
, m_crBack(COLORREF_NULL)
, m_crPressed(COLORREF_NULL)
, m_crHilight(COLORREF_NULL)
, m_crChecked(COLORREF_NULL)
{
}

void CXTColorSelectorCtrlTheme::RefreshMetrics()
{
	CXTThemeManagerStyle::RefreshMetrics();

	m_crBack = GetXtremeColor(COLOR_3DFACE);
	m_crBorderHilight = GetXtremeColor(COLOR_3DHILIGHT);
	m_crBorderShadow = GetXtremeColor(COLOR_3DSHADOW);
	m_crText = GetXtremeColor(COLOR_BTNTEXT);
}

void CXTColorSelectorCtrlTheme::FillBackground(CDC* pDC, const CRect& rect, CXTColorSelectorCtrl* /*pOwner*/)
{
	pDC->FillSolidRect(&rect, m_crBack);
}

void CXTColorSelectorCtrlTheme::DrawColorCell(CXTColorSelectorCtrl::COLOR_CELL* pColorCell, CDC* pDC, BOOL bHilite, BOOL bPressed)
{
	pDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pDC->SelectObject(&XTAuxData().font);

	CRect rcItem = pColorCell->rect;

	bool bChecked = pColorCell->bChecked;

	if (bChecked || bHilite || bPressed)
	{
		if (bPressed)
		{
			if (m_crPressed != COLORREF_NULL)
			{
				pDC->FillSolidRect(&rcItem, m_crPressed);
			}
		}
		else if (bHilite)
		{
			if (m_crHilight != COLORREF_NULL)
			{
				pDC->FillSolidRect(&rcItem, m_crHilight);
			}
		}
		else if (bChecked)
		{
			if (m_crChecked != COLORREF_NULL)
			{
				pDC->FillSolidRect(&rcItem, m_crChecked);
			}
			else
			{
				XTFuncDrawShadedRect(pDC, rcItem);
			}
		}

		pDC->Draw3dRect(rcItem, (bPressed || bChecked) ? m_crBorderShadow : m_crBorderHilight,
			(bPressed || bChecked) ? m_crBorderHilight : m_crBorderShadow);
	}

	int nLen = (int)_tcslen(pColorCell->szText);
	if (nLen)
	{
		pDC->SetTextColor(m_crText);
		pDC->DrawText(pColorCell->szText, nLen, rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	if ((pColorCell->dwStyle & CPS_XT_DEFAULT) == 0)
	{
		rcItem.DeflateRect(3, 3);

		if (pColorCell->nIndex != 0)
			pDC->Draw3dRect(rcItem, m_crBorderShadow, m_crBorderShadow);

		if ((pColorCell->dwStyle & CPS_XT_PICKBOX) == 0)
		{
			if ((pColorCell->dwStyle & CPS_XT_NOFILL) == 0)
			{
				rcItem.DeflateRect(3, 3);
				rcItem.right = rcItem.left + 12;
				rcItem.bottom = rcItem.top + 11;
				rcItem.OffsetRect(1, 0);
				pDC->Draw3dRect(rcItem, m_crBorderShadow, m_crBorderShadow);
				rcItem.DeflateRect(1, 1);
				pDC->FillSolidRect(rcItem, pColorCell->clr);
			}
		}
		else
		{
			rcItem.DeflateRect(1, 1);
			pDC->FillSolidRect(rcItem, pColorCell->clr);
		}
	}
	else
	{
		if (pColorCell->bChecked)
		{
			rcItem = pColorCell->rect;
			rcItem.DeflateRect(5, 6);
			rcItem.left = rcItem.right - 12;
			pDC->Draw3dRect(rcItem, m_crBorderShadow, m_crBorderShadow);
			rcItem.DeflateRect(1, 1);
			pDC->FillSolidRect(rcItem, pColorCell->clr);
		}
	}

	pDC->SelectObject(pOldFont);
}

//////////////////////////////////////////////////////////////////////
// CXTColorSelectorCtrlThemeOfficeXP

CXTColorSelectorCtrlThemeOfficeXP::CXTColorSelectorCtrlThemeOfficeXP()
{
}

void CXTColorSelectorCtrlThemeOfficeXP::RefreshMetrics()
{
	CXTColorSelectorCtrlTheme::RefreshMetrics();

	m_crBorderHilight = GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER);
	m_crBorderShadow = GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER);
	m_crBack = GetXtremeColor(XPCOLOR_MENUBAR_FACE);
	m_crPressed = GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED);
	m_crHilight = GetXtremeColor(XPCOLOR_HIGHLIGHT);
	m_crChecked = GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED);
}

//////////////////////////////////////////////////////////////////////
// CXTColorSelectorCtrlThemeOffice2003

CXTColorSelectorCtrlThemeOffice2003::CXTColorSelectorCtrlThemeOffice2003()
{
}

void CXTColorSelectorCtrlThemeOffice2003::RefreshMetrics()
{
	CXTColorSelectorCtrlThemeOfficeXP::RefreshMetrics();

	m_crBorderHilight = GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER);
	m_crBorderShadow = GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER);
	m_crBack = GetMSO2003Color(XPCOLOR_MENUBAR_FACE);
	m_crPressed = GetMSO2003Color(XPCOLOR_HIGHLIGHT_PUSHED);
	m_crHilight = GetMSO2003Color(XPCOLOR_HIGHLIGHT);
	m_crChecked = GetMSO2003Color(XPCOLOR_HIGHLIGHT_CHECKED);
}
