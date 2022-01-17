// XTPDatePickerPaintManager.cpp: implementation of the CXTPDatePickerPaintManager class.
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

#include "Resource.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPResourceManager.h"

#include "XTPCalendarUtils.h"
#include "XTPDatePickerPaintManager.h"
#include "XTPDatePickerControl.h"
#include "XTPDatePickerItemMonth.h"
#include "XTPDatePickerItemDay.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPDatePickerPaintManager::CXTPDatePickerPaintManager()
{
	m_pControl = NULL;

	m_bEnableTheme = TRUE;

	// initialize default fonts
	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));
	SetFontIndirect(&lfIcon);

	// init metrics
	RefreshMetrics();

	// init cursor
	m_hCursorBack = XTPResourceManager()->LoadCursor(XTP_IDC_DATEPICKER_BACKARROW);
}

void CXTPDatePickerPaintManager::SetFontIndirect(LOGFONT* lFont)
{
		// initialize default day item font
	m_fontDay.DeleteObject();
	VERIFY(m_fontDay.CreateFontIndirect(lFont));

	LOGFONT lfBold = *lFont;
	lfBold.lfWeight = FW_BOLD;
	VERIFY(m_fontDayBold.CreateFontIndirect(&lfBold));

	// initialize default non month day item font
	m_fontNonMonthDay.DeleteObject();
	VERIFY(m_fontNonMonthDay.CreateFontIndirect(lFont));
	// initialize default header font
	m_fontHeader.DeleteObject();
	VERIFY(m_fontHeader.CreateFontIndirect(lFont));
	// initialize default days of week font
	m_fontDaysOfWeek.DeleteObject();
	VERIFY(m_fontDaysOfWeek.CreateFontIndirect(lFont));
	// initialize default popup list control font
	m_fontListControl.DeleteObject();
	VERIFY(m_fontListControl.CreateFontIndirect(lFont));
	// initialize default button font
	m_fontButton.DeleteObject();
	VERIFY(m_fontButton.CreateFontIndirect(lFont));
	// initialize default week numbers font
	m_fontWeekNumbers.DeleteObject();
	lFont->lfHeight -= lFont->lfHeight / 3;
	VERIFY(m_fontWeekNumbers.CreateFontIndirect(lFont));
}


CXTPDatePickerPaintManager::~CXTPDatePickerPaintManager()
{
	//destructor
}

void CXTPDatePickerPaintManager::RefreshMetrics()
{
	RefreshXtremeColors();

	// get system params
	m_clr3DLight = GetSysColor(COLOR_3DLIGHT);
	m_clrBtnText = GetSysColor(COLOR_BTNTEXT);
	m_clr3DShadow = GetSysColor(COLOR_3DSHADOW);
	m_clrBtnFace = GetSysColor(COLOR_BTNFACE);


	// initialize default date picker paint manager settings
	m_clrControlBack = GetSysColor(COLOR_WINDOW);
	m_clrDayBack = m_clrControlBack;
	m_clrDayText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrNonMonthDayBack = m_clrDayBack;

	m_clrSelectedDayBack = GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED);
	m_clrSelectedDayText = m_clrDayText;

	m_clrStaticBorder = GetXtremeColor(XPCOLOR_STATICFRAME);

	m_clrHeaderBack = m_clrBtnFace;
	m_clrHeaderText = m_clrDayText;
	m_clrDaysOfWeekBack = m_clrDayBack;
	m_clrDaysOfWeekText = m_clrDayText;
	m_clrWeekNumbersBack = m_clrDayBack;
	m_clrWeekNumbersText = m_clrDayText;
	m_clrListControlBack = m_clrControlBack;
	m_clrListControlText = m_clrDayText;
	m_clrWeekNumbersText = m_clrHeaderText;

	m_clrHighlightToday = GetSysColor(COLOR_HIGHLIGHT);


	m_clrNonMonthDayText = m_clr3DShadow;


	m_themeButton.CloseTheme();

	if (m_bEnableTheme && !XTPColorManager()->IsLunaColorsDisabled())
	{
		switch (XTPColorManager()->GetCurrentSystemTheme())
		{
			case xtpSystemThemeBlue:
				m_clrSelectedDayBack = RGB(0xFB, 0xE6, 0x94);
				m_clrHighlightToday = RGB(0xBB, 0x55, 0x03);
				m_clrHeaderBack = RGB(0x9E, 0xBE, 0xF5);
				break;
			case xtpSystemThemeOlive:
				m_clrSelectedDayBack = RGB(251, 230, 148);
				m_clrHighlightToday = RGB(187, 85, 3);
				m_clrHeaderBack = RGB(217, 217, 167);
				break;
			case xtpSystemThemeSilver:
				m_clrSelectedDayBack = RGB(251, 230, 148);
				m_clrHighlightToday = RGB(187, 85, 3);
				m_clrHeaderBack = RGB(215, 215, 229);
				break;
		}
		m_themeButton.OpenTheme(0, L"BUTTON");
	}
}

void CXTPDatePickerPaintManager::DrawBackground(CDC* pDC, CRect rcClient)
{
	pDC->FillSolidRect(rcClient, GetControlBackColor());
}


void CXTPDatePickerPaintManager::DrawBorder(CDC* pDC, const CXTPDatePickerControl* pControl, CRect& rcClient, BOOL bDraw)
{
	XTPDatePickerBorderStyle borderStyle = pControl->GetBorderStyle ();
	BOOL bThemed = m_themeButton.IsAppThemed();

	if (bDraw)
	{
		if (borderStyle == xtpDatePickerBorder3D)
		{
			pDC->Draw3dRect(0, 0, rcClient.Width(), rcClient.Height(), m_clrBtnFace, m_clrBtnText);
			pDC->Draw3dRect(1, 1, rcClient.Width()-2, rcClient.Height()-2, m_clrControlBack, m_clr3DShadow);
			pDC->Draw3dRect(2, 2, rcClient.Width()-4, rcClient.Height()-4, m_clrBtnFace, m_clrBtnFace);
			pDC->Draw3dRect(3, 3, rcClient.Width()-6, rcClient.Height()-6, m_clr3DShadow, m_clrControlBack);
		}
		if ((borderStyle == xtpDatePickerBorderStatic) || (borderStyle == xtpDatePickerBorderOffice && bThemed))
		{
			pDC->Draw3dRect(0, 0, rcClient.Width(), rcClient.Height(), m_clrStaticBorder, m_clrStaticBorder);
		}
		if (borderStyle == xtpDatePickerBorderOffice && !bThemed)
		{
			pDC->Draw3dRect(0, 0, rcClient.Width(), rcClient.Height(), m_clrBtnFace, m_clrBtnText);
			pDC->Draw3dRect(1, 1, rcClient.Width()-2, rcClient.Height()-2, m_clrControlBack, m_clr3DShadow);
			pDC->Draw3dRect(2, 2, rcClient.Width()-4, rcClient.Height()-4, m_clrBtnFace, m_clrBtnFace);
		}
	}
	else
	{
		if (borderStyle == xtpDatePickerBorder3D)
		{
			rcClient.DeflateRect(4, 4, 4, 4);
		}
		if ((borderStyle == xtpDatePickerBorderStatic) || (borderStyle == xtpDatePickerBorderOffice && bThemed))
		{
			rcClient.DeflateRect(1, 1, 1, 1);
		}
		if (borderStyle == xtpDatePickerBorderOffice && !bThemed)
		{
			rcClient.DeflateRect(3, 3, 3, 3);
		}
	}
}

void CXTPDatePickerPaintManager::SetButtonFontIndirect(LOGFONT* lFont)
{
	m_fontButton.DeleteObject();
	m_fontButton.CreateFontIndirect(lFont);
}

void CXTPDatePickerPaintManager::SetButtonFont(CFont* fontButton)
{
	LOGFONT lFont;
	fontButton->GetLogFont(&lFont);
	SetButtonFontIndirect(&lFont);
}

void CXTPDatePickerPaintManager::Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2)
{
	CXTPPenDC pen (*pDC, 0);
	CXTPBrushDC brush (*pDC, 0);

	CPoint pts[3];
	pts[0] = pt0;
	pts[1] = pt1;
	pts[2] = pt2;
	pDC->Polygon(pts, 3);
}

void CXTPDatePickerPaintManager::DrawScrollTriangle(CDC* pDC, CRect rcSpot, BOOL bLeftDirection)
{
	CPoint ptCenter(rcSpot.CenterPoint());

	if (bLeftDirection)
		Triangle(pDC, ptCenter, CPoint(ptCenter.x + 4, ptCenter.y - 4), CPoint(ptCenter.x + 4, ptCenter.y + 4));
	else
		Triangle(pDC, ptCenter, CPoint(ptCenter.x - 4, ptCenter.y - 4), CPoint(ptCenter.x - 4, ptCenter.y + 4));

}

void CXTPDatePickerPaintManager::DrawButton(CDC* pDC, const CRect& rcButton, const CString& strButton,
						BOOL bIsDown, BOOL bIsHighLight, BOOL bDrawText)
{
	pDC->SetBkMode(TRANSPARENT);

	CRect rcButtonX = rcButton;
	rcButtonX.DeflateRect(1, 1, 1, 1);

	if (!m_themeButton.IsAppThemed())
	{

		pDC->FillSolidRect(rcButton, m_clrBtnFace);

		if (bIsDown)
		{
			pDC->Draw3dRect(rcButton, m_clrBtnText, m_clrBtnFace);
			pDC->Draw3dRect(rcButtonX, m_clr3DShadow, m_clrControlBack);
		}
		else
		{
			pDC->Draw3dRect(rcButton, m_clrBtnFace, m_clrBtnText);
			pDC->Draw3dRect(rcButtonX, m_clrControlBack, m_clr3DShadow);
		}
	}
	else
	{
		m_themeButton.DrawThemeBackground(pDC->GetSafeHdc(), BP_PUSHBUTTON,
			bIsDown ? PBS_PRESSED: bIsHighLight ? PBS_HOT : PBS_NORMAL,
			&rcButton, NULL);
	}

	if (bDrawText)
	{
		CXTPFontDC autoFont(pDC, &m_fontButton, m_clrBtnText);
		pDC->DrawText(strButton, rcButtonX, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}

BOOL CXTPDatePickerPaintManager::DrawDay(CDC* pDC, CXTPDatePickerItemDay* pDay)
{
	if (!pDC || !pDay)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// check this day item
	BOOL bThisMonthItem = FALSE;
	BOOL bNextMonthItem = FALSE;
	BOOL bPreviousMonthItem = FALSE;

	CXTPDatePickerControl* pControl = pDay->GetDatePickerControl();
	CXTPDatePickerItemMonth* pMonth = pDay->GetMonth();

	COleDateTime dtDay = pDay->GetDate();
	CRect rcDay = pDay->GetRect();

	if (dtDay.GetYear() > pMonth->GetMonth().GetYear())
		bNextMonthItem = TRUE;
	else if (dtDay.GetYear() < pMonth->GetMonth().GetYear())
		bPreviousMonthItem = TRUE;
	else // years the same
	{
		if (dtDay.GetMonth() > pMonth->GetMonth().GetMonth())
			bNextMonthItem = TRUE;
		else if (dtDay.GetMonth() < pMonth->GetMonth().GetMonth())
			bPreviousMonthItem = TRUE;
		else // months the same
			bThisMonthItem = TRUE;
	}

	if (bPreviousMonthItem && !pMonth->GetShowDaysBefore())
		return FALSE;

	if (bNextMonthItem && !pMonth->GetShowDaysAfter())
		return FALSE;

	// set default drawing metrics depending on item month position
	XTP_DAYITEM_METRICS* pMetrics = new XTP_DAYITEM_METRICS;
	pMetrics->clrBackground = bThisMonthItem ?
		GetDayBackColor() : GetNonMonthDayBackColor();
	pMetrics->clrForeground = bThisMonthItem ?
		GetDayTextColor() : GetNonMonthDayTextColor();
	pMetrics->SetFont(bThisMonthItem ?
		GetDayTextFont() : GetNonMonthDayTextFont());

	// call callback function for changing parameters if needed
	pControl->GetDayMetrics(dtDay, pMetrics);

	// change default drawing metrics if day is selected
	if (pControl->IsSelected(dtDay))
	{
		pMetrics->clrBackground = GetSelectedDayBackColor();
		pMetrics->clrForeground = GetSelectedDayTextColor();
	}


	// apply settings to the DC
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(pMetrics->clrForeground);

	// fill background
	pDC->FillSolidRect(rcDay, pMetrics->clrBackground);

	// make a text for drawing
	CString strText;
	strText.Format(_T("%i"), dtDay.GetDay());

	// draw item content
	{
		// draw item text
		CXTPFontDC fntdc(pDC, pMetrics->GetFont());
		pDC->DrawText(strText, &rcDay, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOCLIP);
	}

	CRect rcFocus = rcDay;
	BOOL bToday = FALSE;

	// highlight today item
	if (pControl->GetHighlightToday() && pDay->IsToday(dtDay))
	{
		pDC->Draw3dRect(rcDay, GetHighlightTodayColor(), GetHighlightTodayColor());
		bToday = TRUE;
	}

	if (pControl->IsFocused(dtDay) && pControl->GetSafeHwnd() == ::GetFocus())
	{
		if (!bToday)
		{
			pDC->SetTextColor(0);
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->DrawFocusRect(&rcFocus);
		}
		else
		{
			COLORREF clrFocus = GetHighlightTodayColor();
			clrFocus = clrFocus ^ RGB(255, 128, 128);

			rcFocus.DeflateRect(0, 0, 1, 1);

			CPen pen1(PS_DOT, 1, clrFocus);
			CXTPPenDC autoPen(pDC, &pen1);

			pDC->MoveTo(rcFocus.TopLeft());
			pDC->LineTo(rcFocus.right, rcFocus.top);
			pDC->LineTo(rcFocus.right, rcFocus.bottom);
			pDC->LineTo(rcFocus.left, rcFocus.bottom);
			pDC->LineTo(rcFocus.left, rcFocus.top);
		}
	}

	pMetrics->InternalRelease();

	return TRUE;
}

void CXTPDatePickerPaintManager::DrawMonthHeader(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	if (pMonth->m_rcHeader.IsRectEmpty())
		return;

	CXTPDatePickerControl* pControl = pMonth->m_pControl;

	COleDateTime dtMonth = pMonth->GetMonth();
	CRect rcHeader = pMonth->m_rcHeader;
	CRect rcLeftScroll = pMonth->m_rcLeftScroll;
	CRect rcRightScroll = pMonth->m_rcRightScroll;

	// fill background
	pDC->FillSolidRect(rcHeader, GetHeaderBackColor());

	// draw header text
	pDC->SetBkColor(GetHeaderBackColor());
	pDC->SetTextColor(GetHeaderTextColor());
	CXTPFontDC fnt(pDC, GetHeaderTextFont());
	CString strText;
	strText.Format(_T("%s %d"), (LPCTSTR)pControl->GetMonthName( dtMonth.GetMonth()), dtMonth.GetYear());
	pDC->DrawText(strText, &rcHeader, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	// draw left scroll triangle
	if (!rcLeftScroll.IsRectEmpty())
	{
		// fill background
		pDC->FillSolidRect(rcLeftScroll, GetHeaderBackColor());
		// draw left triangle itself
		if (pMonth->GetShowLeftScroll())
			DrawScrollTriangle(pDC, rcLeftScroll, TRUE);
	}

	// draw right scroll triangle
	if (!rcRightScroll.IsRectEmpty())
	{
		// fill background
		pDC->FillSolidRect(rcRightScroll, GetHeaderBackColor());
		// draw right triangle itself
		if (pMonth->GetShowRightScroll())
			DrawScrollTriangle(pDC, rcRightScroll, FALSE);
	}
}

void CXTPDatePickerPaintManager::DrawDaysOfWeek(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	if (pMonth->m_rcDaysOfWeek.IsRectEmpty())
		return;

	CXTPDatePickerControl* pControl = pMonth->m_pControl;

	// fill background
	pDC->FillSolidRect(pMonth->m_rcDaysOfWeek, GetDaysOfWeekBackColor());

	// draw days of week text
	pDC->SetBkColor(GetDaysOfWeekBackColor());
	pDC->SetTextColor(GetDaysOfWeekTextColor());
	CXTPFontDC fnt(pDC, GetDaysOfWeekTextFont());
	int nMaxX = 0;
	for (int nDayDelta = 0; nDayDelta < XTP_WEEK_DAYS; nDayDelta++)
	{
		// calc item rect
		CXTPDatePickerItemDay* pDay = pMonth->GetDay(nDayDelta);
		CRect rcItem(pDay->GetRect());
		rcItem.top = pMonth->m_rcDaysOfWeek.top;
		rcItem.bottom = pMonth->m_rcDaysOfWeek.bottom - 2;
		nMaxX = rcItem.right;
		// get item text
		CString strText(pControl->GetDayOfWeekName(pDay->GetDate().GetDayOfWeek()));
		pDC->DrawText(strText.Left(1), &rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}

	// draw bottom line on days of the week
	CRect rcBottomLine(pMonth->m_rcDaysOfWeek);
	rcBottomLine.bottom--;
	rcBottomLine.top = rcBottomLine.bottom - 1;
	rcBottomLine.left = pMonth->m_rcWeekNumbers.right - 1;
	rcBottomLine.right = nMaxX;
	pDC->FillSolidRect(rcBottomLine, GetNonMonthDayTextColor());
}

void CXTPDatePickerPaintManager::DrawWeekNumbers(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	CXTPDatePickerControl* pControl = pMonth->m_pControl;

	if (!pControl->GetShowWeekNumbers() || pMonth->m_rcWeekNumbers.IsRectEmpty())
		return;

	// fill background
	pDC->FillSolidRect(pMonth->m_rcWeekNumbers, GetWeekNumbersBackColor());

	// draw week numbers
	pDC->SetBkColor(GetWeekNumbersBackColor());
	pDC->SetTextColor(GetWeekNumbersTextColor());
	CXTPFontDC fnt(pDC, GetWeekNumbersTextFont());

	// draw right line on week days
	CRect rcLine(pMonth->m_rcWeekNumbers);

	for (int nWeek = 0; nWeek < XTP_MAX_WEEKS; nWeek++)
	{
		// calculate item rectangle (get week middle day)
		CXTPDatePickerItemDay* pDayFirst = pMonth->GetDay(nWeek * XTP_WEEK_DAYS);
		CXTPDatePickerItemDay* pDayLast = pMonth->GetDay((nWeek + 1) * XTP_WEEK_DAYS - 1);

		if (pDayFirst && (pDayFirst->IsVisible() || nWeek < 4) && pDayLast)
		{
			CRect rcItem(pDayFirst->GetRect());
			rcItem.left = pMonth->m_rcWeekNumbers.left;
			rcItem.right = pMonth->m_rcWeekNumbers.right - 3;
			// get item text
			CString strText;
			int nWeekNumber = ((pDayLast->GetDate() - COleDateTimeSpan(pControl->GetFirstWeekOfYearDays() - 1)).GetDayOfYear() - 1) / 7 + 1;
			strText.Format(_T("%d"), nWeekNumber);
			pDC->DrawText(strText, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

			rcLine.bottom = rcItem.bottom;
		}
	}

	pDC->FillSolidRect(rcLine.right - 1, rcLine.top - 1, 1, rcLine.Height(), GetNonMonthDayTextColor());
}

CSize CXTPDatePickerPaintManager::CalcDayRect(CDC* pDC)
{
	CFont* fontOld = pDC->SelectObject(&m_fontDay);
	CSize szDay = pDC->GetTextExtent(_T("30"), 2);
	szDay.cx += 4;
	szDay.cy += 2;
	pDC->SelectObject(fontOld);

	return szDay;
}

CSize CXTPDatePickerPaintManager::CalcMonthHeaderRect(CDC* pDC)
{
	CFont* fontOld = pDC->SelectObject(&m_fontHeader);
	CSize szHeader = pDC->GetTextExtent(_T(" "), 1);
	szHeader.cy += 4;
	pDC->SelectObject(fontOld);
	return szHeader;
}

CSize CXTPDatePickerPaintManager::CalcDayOfWeekRect(CDC* pDC)
{
	CFont* fontOld = pDC->SelectObject(&m_fontDaysOfWeek);
	CSize szWeekText = pDC->GetTextExtent(_T(" "), 1);
	szWeekText.cy += 4;
	pDC->SelectObject(fontOld);
	return szWeekText;
}

CSize CXTPDatePickerPaintManager::CalcWeekNumbersRect(CDC* pDC)
{
	CFont* fontOld = pDC->SelectObject(&m_fontWeekNumbers);
	CSize szWeekNumber = pDC->GetTextExtent(_T("88"), 2);
	szWeekNumber.cx += 8;
	pDC->SelectObject(fontOld);
	return szWeekNumber;
}

CSize CXTPDatePickerPaintManager::CalcMonthRect(CDC* pDC)
{
	CSize szDay(CalcDayRect(pDC));
	CSize szHeader(CalcMonthHeaderRect(pDC));
	CSize szWeekText(CalcDayOfWeekRect(pDC));
	CSize szWeekNumber(CalcWeekNumbersRect(pDC));

	CSize szMonth(0, 0);
	szMonth.cx = (szDay.cx * 7 + szWeekNumber.cx * 2);
	szMonth.cy = (szDay.cy * 6 + szWeekText.cy + szHeader.cy);

	return szMonth;
}


//***************************************************************************
IMPLEMENT_DYNAMIC(CXTPDatePickerThemePart, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPDatePickerTheme, CCmdTarget)
IMPLEMENT_DYNCREATE(CXTPDatePickerThemeOffice2007, CXTPDatePickerTheme)

void CXTPDatePickerThemePart::Create(CXTPDatePickerTheme* pTheme)
{
	ASSERT(pTheme);
	ASSERT(!m_pTheme);
	m_pTheme = pTheme;

	if (m_pTheme)
	{
		m_pTheme->m_arMembers.Add(this);
	}
}

void CXTPDatePickerTheme::RefreshMetrics()
{
	TBase::RefreshMetrics(); //RefreshXtremeColors();

	m_clrBackground.SetStandardValue(GetSysColor(COLOR_WINDOW));

	m_clrLineTop.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_clrLineLeft.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
//  m_clrLineBottom.SetStandardValue(GetSysColor(COLOR_3DSHADOW));

	m_fntBaseFont.SetStandardValue(&m_fontDay);
	m_fntBaseFontBold.SetStandardValue(&m_fontDayBold);

	//-------------------------------------
	int nCount = m_arMembers.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPDatePickerThemePart* pPart = m_arMembers.GetAt(i);
		ASSERT(pPart);
		if (pPart)
			pPart->RefreshMetrics();
	}
}

void CXTPDatePickerTheme::CTODay::RefreshMetrics()
{
	ASSERT(m_pTheme);
	if (!m_pTheme)
		return;

	// text colors
	m_Normal.fcsetText.clrColor.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));
	m_Normal.fcsetTextBold.clrColor.SetStandardValue(m_Normal.fcsetText.clrColor);

	m_Normal.fcsetTextGrayed.clrColor.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_Normal.fcsetTextGrayedBold.clrColor.SetStandardValue(m_Normal.fcsetTextGrayed.clrColor);

	// Bk colors
	m_Normal.fcsetText.clrBkColor.SetStandardValue(m_pTheme->m_clrBackground);
	m_Normal.fcsetTextBold.clrBkColor.SetStandardValue(m_pTheme->m_clrBackground);
	m_Normal.fcsetTextGrayed.clrBkColor.SetStandardValue(m_pTheme->m_clrBackground);
	m_Normal.fcsetTextGrayedBold.clrBkColor.SetStandardValue(m_pTheme->m_clrBackground);

	// fonts
	m_Normal.fcsetText.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);
	m_Normal.fcsetTextGrayed.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);

	m_Normal.fcsetTextBold.fntFont.SetStandardValue(m_pTheme->m_fntBaseFontBold);
	m_Normal.fcsetTextGrayedBold.fntFont.SetStandardValue(m_pTheme->m_fntBaseFontBold);

	//----------------------------------------
	// Selected text colors
	m_Selected.fcsetText.clrColor.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));
	m_Selected.fcsetTextBold.clrColor.SetStandardValue(m_Normal.fcsetText.clrColor);

	m_Selected.fcsetTextGrayed.clrColor.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_Selected.fcsetTextGrayedBold.clrColor.SetStandardValue(m_Normal.fcsetTextGrayed.clrColor);

	// Selected Bk colors
	m_Selected.fcsetText.clrBkColor.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED));
	m_Selected.fcsetTextBold.clrBkColor.SetStandardValue(m_Selected.fcsetText.clrBkColor);
	m_Selected.fcsetTextGrayed.clrBkColor.SetStandardValue(m_Selected.fcsetText.clrBkColor);
	m_Selected.fcsetTextGrayedBold.clrBkColor.SetStandardValue(m_Selected.fcsetText.clrBkColor);

	// Selected fonts
	m_Selected.fcsetText.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);
	m_Selected.fcsetTextGrayed.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);

	m_Selected.fcsetTextBold.fntFont.SetStandardValue(m_pTheme->m_fntBaseFontBold);
	m_Selected.fcsetTextGrayedBold.fntFont.SetStandardValue(m_pTheme->m_fntBaseFontBold);

	//----------------------------------
	m_clrToodayFrame.SetStandardValue(GetSysColor(COLOR_HIGHLIGHT));
}

void CXTPDatePickerTheme::CTOMonthHeader::RefreshMetrics()
{
	ASSERT(m_pTheme);
	if (!m_pTheme)
		return;

	m_clrBackground.SetStandardValue(GetSysColor(COLOR_BTNFACE));

	m_Text.clrColor.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));

	m_Text.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);;
}

void CXTPDatePickerTheme::CTOWeekDay::RefreshMetrics()
{
	ASSERT(m_pTheme);
	if (!m_pTheme)
		return;

	m_Text.clrColor.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));

	m_Text.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);;
}

void CXTPDatePickerTheme::CTOWeekNumber::RefreshMetrics()
{
	ASSERT(m_pTheme);
	if (!m_pTheme)
		return;

	m_Text.clrColor.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));

	LOGFONT lFont;
	m_pTheme->m_fntBaseFont->GetLogFont(&lFont);

	lFont.lfHeight -= lFont.lfHeight / 3;

	m_Text.fntFont.SetStandardValue(&lFont);
}

void CXTPDatePickerTheme::CTOButton::RefreshMetrics()
{
	ASSERT(m_pTheme);
	if (!m_pTheme)
		return;

	m_Text.clrColor.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));

	m_Text.fntFont.SetStandardValue(m_pTheme->m_fntBaseFont);;
}

void CXTPDatePickerTheme::DrawBackground(CDC* pDC, CRect rcClient)
{
	pDC->FillSolidRect(rcClient, m_clrBackground);
}

void CXTPDatePickerTheme::DrawBorder(CDC* pDC, const CXTPDatePickerControl* pControl, CRect& rcClient, BOOL bDraw)
{
	// TODO: customize border
	TBase::DrawBorder(pDC, pControl, rcClient, bDraw);
}

void CXTPDatePickerTheme::DrawScrollTriangle(CDC* pDC, CRect rcSpot, BOOL bLeftDirection)
{
	// TODO: customize L/R buttons
	TBase::DrawScrollTriangle(pDC, rcSpot, bLeftDirection);
}

void CXTPDatePickerTheme::DrawButton(CDC* pDC, const CRect& rcButton, const CString& strButton,
									BOOL bIsDown, BOOL bIsHighLight, BOOL bDrawText)
{
	// TODO: customize
	TBase::DrawButton(pDC, rcButton, strButton, bIsDown, bIsHighLight, FALSE);

	if (bDrawText)
	{
		CRect rcButtonX = rcButton;
		rcButtonX.DeflateRect(1, 1, 1, 1);

		CXTPFontDC autoFont(pDC, GetButtonPart()->m_Text.fntFont, GetButtonPart()->m_Text.clrColor);
		pDC->DrawText(strButton, rcButtonX, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}

CSize CXTPDatePickerTheme::CalcDayRect(CDC* pDC)
{
	CArray<CFont*, CFont*> arFonts;
	arFonts.Add(GetDayPart()->m_Normal.fcsetText.fntFont);
	arFonts.Add(GetDayPart()->m_Normal.fcsetTextBold.fntFont);
	arFonts.Add(GetDayPart()->m_Normal.fcsetTextGrayed.fntFont);
	arFonts.Add(GetDayPart()->m_Normal.fcsetTextGrayedBold.fntFont);

	arFonts.Add(GetDayPart()->m_Selected.fcsetText.fntFont);
	arFonts.Add(GetDayPart()->m_Selected.fcsetTextBold.fntFont);
	arFonts.Add(GetDayPart()->m_Selected.fcsetTextGrayed.fntFont);
	arFonts.Add(GetDayPart()->m_Selected.fcsetTextGrayedBold.fntFont);

	CFont* pFontMax = CXTPCalendarUtils::GetMaxHeightFont(arFonts, pDC);
	ASSERT(pFontMax);
	if (!pFontMax)
		return CSize(10, 10);

	CXTPFontDC autoFont(pDC, pFontMax);
	CSize szDay = pDC->GetTextExtent(_T("30"), 2);
	szDay.cx += 4;
	szDay.cy += 2;

	return szDay;
}

CSize CXTPDatePickerTheme::CalcMonthHeaderRect(CDC* pDC)
{
	CXTPFontDC autoFont(pDC, GetMonthHeaderPart()->m_Text.fntFont);
	CSize szHeader = pDC->GetTextExtent(_T(" "), 1);
	szHeader.cy += 4;

	return szHeader;
}

CSize CXTPDatePickerTheme::CalcDayOfWeekRect(CDC* pDC)
{
	CXTPFontDC autoFont(pDC, GetWeekDayPart()->m_Text.fntFont);
	CSize szWeekText = pDC->GetTextExtent(_T(" "), 1);
	szWeekText.cy += 4;

	return szWeekText;
}

CSize CXTPDatePickerTheme::CalcWeekNumbersRect(CDC* pDC)
{
	CXTPFontDC autoFont(pDC, GetWeekNumberPart()->m_Text.fntFont);
	CSize szWeekNumber = pDC->GetTextExtent(_T("88"), 2);
	szWeekNumber.cx += 8;

	return szWeekNumber;
}

BOOL CXTPDatePickerTheme::DrawDay(CDC* pDC, CXTPDatePickerItemDay* pDay)
{
	return GetDayPart()->Draw(pDC, pDay);
}

void CXTPDatePickerTheme::DrawWeekNumbers(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	GetWeekNumberPart()->Draw(pDC, pMonth);
}
void CXTPDatePickerTheme::DrawDaysOfWeek(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	GetWeekDayPart()->Draw(pDC, pMonth);
}

void CXTPDatePickerTheme::DrawMonthHeader(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	GetMonthHeaderPart()->Draw(pDC, pMonth);
}

BOOL CXTPDatePickerTheme::CTODay::Draw(CDC* pDC, CXTPDatePickerItemDay* pDay)
{
	if (!pDC || !pDay)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// check this day item
	BOOL bThisMonthItem = FALSE;
	BOOL bNextMonthItem = FALSE;
	BOOL bPreviousMonthItem = FALSE;

	CXTPDatePickerControl* pControl = pDay->GetDatePickerControl();
	CXTPDatePickerItemMonth* pMonth = pDay->GetMonth();

	COleDateTime dtDay = pDay->GetDate();
	CRect rcDay = pDay->GetRect();

	if (dtDay.GetYear() > pMonth->GetMonth().GetYear())
		bNextMonthItem = TRUE;
	else if (dtDay.GetYear() < pMonth->GetMonth().GetYear())
		bPreviousMonthItem = TRUE;
	else // years the same
	{
		if (dtDay.GetMonth() > pMonth->GetMonth().GetMonth())
			bNextMonthItem = TRUE;
		else if (dtDay.GetMonth() < pMonth->GetMonth().GetMonth())
			bPreviousMonthItem = TRUE;
		else // months the same
			bThisMonthItem = TRUE;
	}

	if (bPreviousMonthItem && !pMonth->GetShowDaysBefore())
		return FALSE;

	if (bNextMonthItem && !pMonth->GetShowDaysAfter())
		return FALSE;

	// set default drawing metrics depending on item month position
	XTP_DAYITEM_METRICS* pMetrics = new XTP_DAYITEM_METRICS;

	CDayItem& setDI = pControl->IsSelected(dtDay) ? m_Selected : m_Normal;

	pMetrics->clrBackground = bThisMonthItem ?
		setDI.fcsetText.clrBkColor : setDI.fcsetTextGrayed.clrBkColor;
	pMetrics->clrForeground = bThisMonthItem ?
		setDI.fcsetText.clrColor : setDI.fcsetTextGrayed.clrColor;
	pMetrics->SetFont(bThisMonthItem ?
		setDI.fcsetText.fntFont : setDI.fcsetTextGrayed.fntFont);

	// call callback function for changing parameters if needed
	pControl->GetDayMetrics(dtDay, pMetrics);

	// apply settings to the DC
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(pMetrics->clrForeground);

	// fill background
	pDC->FillSolidRect(rcDay, pMetrics->clrBackground);

	// make a text for drawing
	CString strText;
	strText.Format(_T("%i"), dtDay.GetDay());

	// draw item content
	{
		// draw item text
		CXTPFontDC fntdc(pDC, pMetrics->GetFont());
		pDC->DrawText(strText, &rcDay, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOCLIP);
	}

	CRect rcFocus = rcDay;
	BOOL bToday = FALSE;

	// highlight today item
	if (pControl->GetHighlightToday() && pDay->IsToday(dtDay))
	{
		pDC->Draw3dRect(rcDay, m_clrToodayFrame, m_clrToodayFrame);
		bToday = TRUE;
	}

	if (pControl->IsFocused(dtDay) && pControl->GetSafeHwnd() == ::GetFocus())
	{
		if (!bToday)
		{
			pDC->SetTextColor(0);
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->DrawFocusRect(&rcFocus);
		}
		else
		{
			COLORREF clrFocus = m_clrToodayFrame;
			clrFocus = clrFocus ^ RGB(255, 128, 128);

			rcFocus.DeflateRect(0, 0, 1, 1);

			CPen pen1(PS_DOT, 1, clrFocus);
			CXTPPenDC autoPen(pDC, &pen1);

			pDC->MoveTo(rcFocus.TopLeft());
			pDC->LineTo(rcFocus.right, rcFocus.top);
			pDC->LineTo(rcFocus.right, rcFocus.bottom);
			pDC->LineTo(rcFocus.left, rcFocus.bottom);
			pDC->LineTo(rcFocus.left, rcFocus.top);
		}
	}

	pMetrics->InternalRelease();

	return TRUE;
}

void CXTPDatePickerTheme::CTOMonthHeader::Draw(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	class CXTPDatePickerItemMonth_helper : public CXTPDatePickerItemMonth {
		friend class CXTPDatePickerTheme::CTOMonthHeader;
	};
	CXTPDatePickerItemMonth_helper* pMonthX = (CXTPDatePickerItemMonth_helper*)pMonth;
	CXTPDatePickerControl* pControl = pMonthX->m_pControl;

	if (pMonthX->m_rcHeader.IsRectEmpty())
		return;

	COleDateTime dtMonth = pMonth->GetMonth();
	CRect rcHeader = pMonthX->m_rcHeader;
	CRect rcLeftScroll = pMonthX->m_rcLeftScroll;
	CRect rcRightScroll = pMonthX->m_rcRightScroll;

	pDC->SetBkMode(TRANSPARENT);

	// fill background
	pDC->FillSolidRect(rcHeader, m_clrBackground);

	// draw header text
	CXTPFontDC fnt(pDC, m_Text.fntFont, m_Text.clrColor);
	CString strText;
	strText.Format(_T("%s %d"), (LPCTSTR)pControl->GetMonthName( dtMonth.GetMonth()), dtMonth.GetYear());
	pDC->DrawText(strText, &rcHeader, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	// draw left scroll triangle
	if (!rcLeftScroll.IsRectEmpty())
	{
		// fill background
		pDC->FillSolidRect(rcLeftScroll, m_clrBackground);
		// draw left triangle itself
		if (pMonth->GetShowLeftScroll())
			m_pTheme->DrawScrollTriangle(pDC, rcLeftScroll, TRUE);
	}

	// draw right scroll triangle
	if (!rcRightScroll.IsRectEmpty())
	{
		// fill background
		pDC->FillSolidRect(rcRightScroll, m_clrBackground);
		// draw right triangle itself
		if (pMonth->GetShowRightScroll())
			m_pTheme->DrawScrollTriangle(pDC, rcRightScroll, FALSE);
	}
}

void CXTPDatePickerTheme::CTOWeekDay::Draw(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	class CXTPDatePickerItemMonth_helper : public CXTPDatePickerItemMonth {
		friend class CXTPDatePickerTheme::CTOWeekDay;
	};
	CXTPDatePickerItemMonth_helper* pMonthX = (CXTPDatePickerItemMonth_helper*)pMonth;
	CXTPDatePickerControl* pControl = pMonthX->m_pControl;

	if (pMonthX->m_rcDaysOfWeek.IsRectEmpty())
		return;

	pDC->SetBkMode(TRANSPARENT);

	// fill background
	//pDC->FillSolidRect(pMonthX->m_rcDaysOfWeek, m_Text.clrColor);

	// draw days of week text
	CXTPFontDC fnt(pDC, m_Text.fntFont, m_Text.clrColor);
	int nMaxX = 0;
	for (int nDayDelta = 0; nDayDelta < XTP_WEEK_DAYS; nDayDelta++)
	{
		// calc item rect
		CXTPDatePickerItemDay* pDay = pMonthX->GetDay(nDayDelta);
		CRect rcItem(pDay->GetRect());
		rcItem.top = pMonthX->m_rcDaysOfWeek.top;
		rcItem.bottom = pMonthX->m_rcDaysOfWeek.bottom - 2;
		nMaxX = rcItem.right;
		// get item text
		CString strText(pControl->GetDayOfWeekName(pDay->GetDate().GetDayOfWeek()));
		pDC->DrawText(strText.Left(1), &rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}

	// draw bottom line on days of the week
	CRect rcBottomLine(pMonthX->m_rcDaysOfWeek);
	rcBottomLine.bottom--;
	rcBottomLine.top = rcBottomLine.bottom - 1;
	rcBottomLine.left = pMonthX->m_rcWeekNumbers.right - 1;
	rcBottomLine.right = nMaxX;
	pDC->FillSolidRect(rcBottomLine, m_pTheme->m_clrLineTop);
}

void CXTPDatePickerTheme::CTOWeekNumber::Draw(CDC* pDC, CXTPDatePickerItemMonth* pMonth)
{
	class CXTPDatePickerItemMonth_helper : public CXTPDatePickerItemMonth {
		friend class CXTPDatePickerTheme::CTOWeekNumber;
	};
	CXTPDatePickerItemMonth_helper* pMonthX = (CXTPDatePickerItemMonth_helper*)pMonth;
	CXTPDatePickerControl* pControl = pMonthX->m_pControl;

	if (!pControl->GetShowWeekNumbers() || pMonthX->m_rcWeekNumbers.IsRectEmpty())
		return;

	pDC->SetBkMode(TRANSPARENT);

	// fill background
	//pDC->FillSolidRect(pMonthX->m_rcWeekNumbers, m_Text.clrColor);

	// draw week numbers
	CXTPFontDC fnt(pDC, m_Text.fntFont, m_Text.clrColor);

	// draw right line on week days
	CRect rcLine(pMonthX->m_rcWeekNumbers);

	for (int nWeek = 0; nWeek < XTP_MAX_WEEKS; nWeek++)
	{
		// calculate item rectangle (get week middle day)
		CXTPDatePickerItemDay* pDayFirst = pMonth->GetDay(nWeek * XTP_WEEK_DAYS);
		CXTPDatePickerItemDay* pDayLast = pMonth->GetDay((nWeek + 1) * XTP_WEEK_DAYS - 1);

		if (pDayFirst && (pDayFirst->IsVisible() || nWeek < 4) && pDayLast)
		{
			CRect rcItem(pDayFirst->GetRect());
			rcItem.left = pMonthX->m_rcWeekNumbers.left;
			rcItem.right = pMonthX->m_rcWeekNumbers.right - 3;
			// get item text
			CString strText;
			int nWeekNumber = ((pDayLast->GetDate() - COleDateTimeSpan(pControl->GetFirstWeekOfYearDays() - 1)).GetDayOfYear() - 1) / 7 + 1;
			strText.Format(_T("%d"), nWeekNumber);
			pDC->DrawText(strText, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

			rcLine.bottom = rcItem.bottom;
		}
	}

	pDC->FillSolidRect(rcLine.right - 1, rcLine.top - 1, 1, rcLine.Height(), m_pTheme->m_clrLineLeft);
}

CXTPDatePickerThemeOffice2007::CXTPDatePickerThemeOffice2007()
{
	m_pDayPart = new CTODay();
	m_pDayPart->Create(this);

	m_pMonthHeader = new CTOMonthHeader();
	m_pMonthHeader->Create(this);

	m_pWeekDay = new CTOWeekDay();
	m_pWeekDay->Create(this);

	m_pWeekNumberPart = new CTOWeekNumber();
	m_pWeekNumberPart->Create(this);

	m_pButtonPart = new CTOButton();
	m_pButtonPart->Create(this);

	RefreshMetrics();

}

void CXTPDatePickerThemeOffice2007::RefreshMetrics()
{
	TBase::RefreshMetrics();

	m_clrLineTop.SetStandardValue(RGB(172, 168, 153));
	m_clrLineLeft.SetStandardValue(m_clrLineTop);
}

void CXTPDatePickerThemeOffice2007::CTODay::RefreshMetrics()
{
	ASSERT(m_pTheme);
	if (!m_pTheme)
		return;

	TBase::RefreshMetrics();

	//----------------------------------------
	// Selected text colors
	m_Selected.fcsetTextGrayed.clrColor.SetStandardValue(m_Selected.fcsetText.clrColor);
	m_Selected.fcsetTextGrayedBold.clrColor.SetStandardValue(m_Selected.fcsetText.clrColor);

	// Selected Bk colors
	m_Selected.fcsetText.clrBkColor.SetStandardValue(RGB(251, 200, 79));
	m_Selected.fcsetTextBold.clrBkColor.SetStandardValue(m_Selected.fcsetText.clrBkColor);
	m_Selected.fcsetTextGrayed.clrBkColor.SetStandardValue(m_Selected.fcsetText.clrBkColor);
	m_Selected.fcsetTextGrayedBold.clrBkColor.SetStandardValue(m_Selected.fcsetText.clrBkColor);

	//----------------------------------
	m_clrToodayFrame.SetStandardValue(RGB(187, 85, 3));
}

void CXTPDatePickerThemeOffice2007::CTOMonthHeader::RefreshMetrics()
{
	TBase::RefreshMetrics();

	m_clrBackground.SetStandardValue(RGB(191, 219, 255));
}

