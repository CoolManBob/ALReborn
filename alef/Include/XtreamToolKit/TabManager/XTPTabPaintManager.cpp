// XTPTabPaintManager.cpp: implementation of the CXTPTabPaintManager class.
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
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPSystemHelpers.h"

#include "XTPTabPaintManager.h"
#include "XTPTabManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"
#include "XTPTabPaintManager.inl"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CXTPTabPaintManager


CXTPTabPaintManager::CXTPTabPaintManager()
{
	m_bUseStandardFont = TRUE;

	m_pColorSet = NULL;
	m_pAppearanceSet = NULL;

	SetAppearance(xtpTabAppearancePropertyPage);

	m_bHotTracking = FALSE;
	m_bBoldNormal = m_bBoldSelected = FALSE;
	m_bShowIcons = TRUE;
	m_bSelectOnButtonDown = TRUE;

	m_bDisableLunaColors = FALSE;
	m_bOneNoteColors = FALSE;

	m_tabPosition = xtpTabPositionTop;
	m_tabLayout = xtpTabLayoutAutoSize;

	m_clientFrame = xtpTabFrameBorder;
	m_bStaticFrame = FALSE;

	m_bInvertGradient = FALSE;
	m_bFillBackground = TRUE;
	m_bDrawFocusRect = TRUE;

	m_rcClientMargin.SetRect(0, 0, 0, 0);
	m_rcControlMargin.SetRect(0, 0, 0, 0);
	m_rcButtonMargin.SetRect(0, 0, 0, 0);

	m_nButtonExtraLength = 0;
	m_szIcon = CSize(16, 16);

	m_nFixedTabWidth = 70;
	m_nMinTabWidth = m_nMaxTabWidth = 0;


	m_bDrawTextEndEllipsis = TRUE;
	m_bDrawTextPathEllipsis = FALSE;
	m_bDrawTextNoPrefix = TRUE;
	m_bDrawTextHidePrefix = FALSE;

	m_bMultiRowFixedSelection = FALSE;
	m_bMultiRowJustified = TRUE;

	m_toolBehaviour = xtpTabToolTipShrinkedOnly;
	m_bSelectOnDragOver = TRUE;
	m_nItemColor = 0;

	m_bClearTypeTextQuality = FALSE;
	m_bRotateImageOnVerticalDraw = FALSE;

	m_rcButtonTextPadding.SetRect(6, 0, 6, 0);

	m_bClipHeader = TRUE;

	m_nDrawTextFormat = DT_LEFT | DT_VCENTER;

	m_szNavigateButton = CSize(14, 15);

	EnableAutomation();



	RefreshMetrics();
}

CXTPTabPaintManager::~CXTPTabPaintManager()
{
	delete m_pColorSet;
	delete m_pAppearanceSet;

}

CXTPTabPaintManager::CColorSet* CXTPTabPaintManager::SetColor(XTPTabColorStyle tabColor)
{
	CColorSet* pColorSet = NULL;

	m_tabColor = tabColor;
	if (tabColor == xtpTabColorVisualStudio) pColorSet = new CColorSetVisualStudio();
	else if (tabColor == xtpTabColorOffice2003) pColorSet = new CColorSetOffice2003();
	else if (tabColor == xtpTabColorWinXP) pColorSet = new CColorSetWinXP();
	else if (tabColor == xtpTabColorWhidbey) pColorSet = new CColorSetWhidbey();
	else if (tabColor == xtpTabColorOffice2007) pColorSet = new CColorSetOffice2007();
	else pColorSet = new CColorSetDefault();

	return SetColorSet(pColorSet);
}
CXTPTabPaintManager::CColorSet* CXTPTabPaintManager::SetColorSet(CColorSet* pColorSet)
{
	if (pColorSet)
	{
		if (m_pColorSet) delete m_pColorSet;
		m_pColorSet = pColorSet;

		pColorSet->m_pPaintManager = this;
		pColorSet->RefreshMetrics();
	}
	OnPropertyChanged();

	return pColorSet;
}

CXTPTabPaintManager::CAppearanceSet* CXTPTabPaintManager::SetAppearance(XTPTabAppearanceStyle tabAppearance)
{
	CAppearanceSet* pAppearanceSet = NULL;

	m_tabAppearance = tabAppearance;
	if (tabAppearance == xtpTabAppearanceVisualStudio) pAppearanceSet = new CAppearanceSetVisualStudio();
	else if (tabAppearance == xtpTabAppearanceExcel) pAppearanceSet = new CAppearanceSetExcel();
	else if (tabAppearance == xtpTabAppearancePropertyPageFlat) pAppearanceSet = new CAppearanceSetPropertyPageFlat();
	else if (tabAppearance == xtpTabAppearanceFlat) pAppearanceSet = new CAppearanceSetFlat();
	else if (tabAppearance == xtpTabAppearanceStateButtons) pAppearanceSet = new CAppearanceSetStateButtons();
	else if (tabAppearance == xtpTabAppearancePropertyPageSelected) pAppearanceSet = new CAppearanceSetPropertyPageSelected();
	else if (tabAppearance == xtpTabAppearancePropertyPage2003) pAppearanceSet = new CAppearanceSetPropertyPage2003();
	else if (tabAppearance == xtpTabAppearanceVisio) pAppearanceSet = new CAppearanceSetVisio();
	else if (tabAppearance == xtpTabAppearanceVisualStudio2005) pAppearanceSet = new CAppearanceSetVisualStudio2005();
	else if (tabAppearance == xtpTabAppearancePropertyPage2007) pAppearanceSet = new CAppearanceSetPropertyPage2007();
	else pAppearanceSet = new CAppearanceSetPropertyPage();

	return SetAppearanceSet(pAppearanceSet);

}
CXTPTabPaintManager::CAppearanceSet* CXTPTabPaintManager::SetAppearanceSet(CAppearanceSet* pAppearanceSet)
{
	if (pAppearanceSet)
	{
		if (m_pAppearanceSet) delete m_pAppearanceSet;
		m_pAppearanceSet = pAppearanceSet;

		pAppearanceSet->m_pPaintManager = this;
		pAppearanceSet->RefreshMetrics();

		SetColor(pAppearanceSet->GetDefaultColorSet());
	}
	OnPropertyChanged();

	return pAppearanceSet;

}


void CXTPTabPaintManager::SetFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardFont /*= FALSE*/)
{
	m_bUseStandardFont = bUseStandardFont;

	if (!pLogFont)
		return;

	if (m_bClearTypeTextQuality && XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		pLogFont->lfQuality = 5;
	}

	m_fntNormal.DeleteObject();
	m_fntBold.DeleteObject();
	m_fntVerticalNormal.DeleteObject();
	m_fntVerticalBold.DeleteObject();

	pLogFont->lfWeight = m_bBoldNormal ? FW_BOLD : FW_NORMAL;
	VERIFY(m_fntNormal.CreateFontIndirect(pLogFont));

	pLogFont->lfWeight = FW_BOLD;
	VERIFY(m_fntBold.CreateFontIndirect(pLogFont));

	STRCPY_S(pLogFont->lfFaceName, LF_FACESIZE, CXTPDrawHelpers::GetVerticalFontName(FALSE));
	pLogFont->lfOrientation = 900;
	pLogFont->lfEscapement = 2700;
	pLogFont->lfHeight = pLogFont->lfHeight < 0 ? __min(-11, pLogFont->lfHeight) : pLogFont->lfHeight;
	pLogFont->lfWeight = m_bBoldNormal ? FW_BOLD : FW_NORMAL;
	VERIFY(m_fntVerticalNormal.CreateFontIndirect(pLogFont));

	pLogFont->lfWeight = FW_BOLD;
	VERIFY(m_fntVerticalBold.CreateFontIndirect(pLogFont));
}

void CXTPTabPaintManager::RefreshMetrics()
{
	if (m_bUseStandardFont)
	{
		LOGFONT lfIcon;
		VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));

		SetFontIndirect(&lfIcon, TRUE);
	}

	m_pColorSet->RefreshMetrics();
	m_pAppearanceSet->RefreshMetrics();
}

void CXTPTabPaintManager::DisableLunaColors(BOOL bDisableLunaColors)
{
	m_bDisableLunaColors = bDisableLunaColors;
	RefreshMetrics();
	OnPropertyChanged();
}
BOOL CXTPTabPaintManager::IsLunaColorsDisabled() const
{
	return m_bDisableLunaColors || XTPColorManager()->IsLunaColorsDisabled();
}

COLORREF CXTPTabPaintManager::GetOneNoteColor(XTPTabOneNoteColor tabColor)
{
	const COLORREF clrTable[] =
	{
		RGB(138, 168, 228), // xtpTabColorBlue    : Blue tab color used when OneNote colors enabled.
		RGB(255, 219, 117), // xtpTabColorYellow  : Yellow tab color used when OneNote colors enabled.
		RGB(189, 205, 159), // xtpTabColorGreen   : Green tab color used when OneNote colors enabled.
		RGB(240, 158, 159), // xtpTabColorRed     : Red tab color used when OneNote colors enabled.
		RGB(186, 166, 225), // xtpTabColorPurple  : Purple tab color used when OneNote colors enabled.
		RGB(154, 191, 180), // xtpTabColorCyan    : Cyan tab color used when OneNote colors enabled.
		RGB(247, 182, 131), // xtpTabColorOrange  : Orange tab color used when OneNote colors enabled.
		RGB(216, 171, 192)  // xtpTabColorMagenta : Magenta tab color used when OneNote colors enabled.
	};

	return clrTable[tabColor-xtpTabColorBlue];
}

void CXTPTabPaintManager::SetOneNoteColors(BOOL bOneNoteColors /* = TRUE */)
{
	m_bOneNoteColors = bOneNoteColors;
	RefreshMetrics();
}

void CXTPTabPaintManager::DrawNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc)
{
	FillNavigateButton(pDC, pButton, rc);

	COLORREF clr = pDC->GetTextColor();
	CXTPPenDC pen(*pDC, clr);

	if (pButton->IsEnabled())
	{
		CXTPBrushDC brush(*pDC, clr);
		pButton->DrawEntry(pDC, rc);
	}
	else
	{
		CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		pButton->DrawEntry(pDC, rc);
		pDC->SelectObject(pOldBrush);
	}

}

void CXTPTabPaintManager::FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc)
{
	m_pColorSet->FillNavigateButton(pDC, pButton, rc);
}

void CXTPTabPaintManager::DrawTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient)
{
	m_pAppearanceSet->DrawTabControl(pTabManager, pDC, rcClient);
}

void CXTPTabPaintManager::DrawRowItems(CXTPTabManager* pTabManager, CDC* pDC, const CRect& rcClipBox, int nItemRow)
{
	CXTPTabManagerItem* pSelected = NULL;

	CXTPTabManager::ROW_ITEMS* pRowItems = pTabManager->m_pRowIndexer->GetRowItems();

	if (!pRowItems)
		return;

	if (nItemRow >= pTabManager->GetRowCount())
		return;

	int nFirstItem = pRowItems[nItemRow].nFirstItem;
	int nLastItem = pRowItems[nItemRow].nLastItem;
	BOOL bDrawRow = TRUE;

	if (m_pAppearanceSet->m_bButtonsReverseZOrder)
	{
		bDrawRow = FALSE;

		for (int i = nFirstItem; i <= nLastItem; i++)
		{
			CXTPTabManagerItem* pItem = pTabManager->GetItem(i);
			if (!pItem)
				return;

			if (pItem->m_nItemRow != nItemRow)
				break;

			if (pItem->IsSelected())
			{
				nFirstItem = i;
				bDrawRow = TRUE;
				break;
			}

			if (pItem->IsVisible() && CRect().IntersectRect(rcClipBox, m_pAppearanceSet->GetButtonDrawRect(pItem)))
			{
				m_pAppearanceSet->DrawSingleButton(pDC, pItem);
			}
		}
	}

	if (bDrawRow)
	{
		for (int i = nLastItem; i >= nFirstItem; i--)
		{
			CXTPTabManagerItem* pItem = pTabManager->GetItem(i);
			if (!pItem)
				return;

			if (pItem->m_nItemRow != nItemRow)
				break;

			if (pItem->IsVisible() && CRect().IntersectRect(rcClipBox, m_pAppearanceSet->GetButtonDrawRect(pItem)))
			{
				if (pItem->IsSelected())
					pSelected = pItem;
				else
					m_pAppearanceSet->DrawSingleButton(pDC, pItem);
			}
		}
	}


	if (pSelected)
	{
		m_pAppearanceSet->DrawSingleButton(pDC, pSelected);
	}


}

void CXTPTabPaintManager::DrawTabControlEx(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient)
{
	if (pTabManager->IsDrawStaticFrame())
	{
		pDC->Draw3dRect(rcClient, GetColorSet()->m_clrFrameBorder, GetColorSet()->m_clrFrameBorder);
		CAppearanceSet::DeflateRectEx(rcClient, CRect(1, 1, 1, 1), pTabManager->GetPosition());
		pDC->Draw3dRect(rcClient, GetColorSet()->m_clrControlFace, GetColorSet()->m_clrControlFace);
		CAppearanceSet::DeflateRectEx(rcClient, CRect(1, 0, 1, 1), pTabManager->GetPosition());
	}

	if (m_bFillBackground)
	{
		pDC->FillSolidRect(rcClient, GetColorSet()->m_clrControlFace);
	}

	CAppearanceSet::DeflateRectEx(rcClient, m_rcControlMargin, pTabManager->GetPosition());

	pDC->SetBkMode(TRANSPARENT);


	if (m_bFillBackground)
	{
		m_pAppearanceSet->FillTabControl(pTabManager, pDC, rcClient);
	}

	int i;

	CRect rcClipBox;
	pDC->GetClipBox(rcClipBox);

	if (m_bClipHeader)
		rcClipBox.IntersectRect(rcClipBox, pTabManager->m_rcHeaderRect);

	if (!rcClipBox.IsRectEmpty())
	{
		CRgn rgn;
		if (m_bClipHeader)
		{
			rgn.CreateRectRgnIndirect(rcClipBox);
			pDC->SelectClipRgn(&rgn);

			m_pColorSet->SelectClipRgn(pDC, rcClient, pTabManager);
		}

		int nRowCont = pTabManager->GetRowCount();

		for (int j = nRowCont - 1; j >= 0; j--)
		{
			DrawRowItems(pTabManager, pDC, rcClipBox, j);
		}

		if (m_bClipHeader)
		{
			pDC->SelectClipRgn(NULL);
		}
	}

	for (i = pTabManager->GetNavigateButtonCount() - 1; i >= 0; i--)
		pTabManager->GetNavigateButton(i)->Draw(pDC);

}
int CXTPTabPaintManager::_GetButtonLength(int nValue, int nMin, int nMax)
{
	nValue += m_rcButtonMargin.left +  m_rcButtonMargin.right;
	if (nMin > 0) nValue = max(nMin, nValue);
	if (nMax > 0) nValue = min(nMax, nValue);
	return nValue;
}

void CXTPTabPaintManager::StripMnemonics(CString& strClear)
{
	CXTPDrawHelpers::StripMnemonics(strClear);
}

int CXTPTabPaintManager::GetPrefixTextExtent(CDC* pDC, const CString& str, UINT uFormat)
{
	if (uFormat & DT_NOPREFIX)
		return pDC->GetTextExtent(str).cx;

	CString strClear = str;
	StripMnemonics(strClear);

	return pDC->GetTextExtent(strClear).cx;
}

void CXTPTabPaintManager::DrawTextPathEllipsis(CDC* pDC, const CString& strItem, CRect rcItem, UINT uFormat)
{
	if (strItem.Find(_T('\\')) != -1)
	{
		pDC->DrawText(strItem, rcItem, uFormat | DT_PATH_ELLIPSIS);
		return;
	}

	LPCTSTR strEllipsis = _T("...");
	int cxEllipsis = pDC->GetTextExtent(strEllipsis, 3).cx;
	int cxItem = GetPrefixTextExtent(pDC, strItem, uFormat);

	if (cxItem <= rcItem.Width())
	{
		pDC->DrawText(strItem, rcItem, uFormat);
		return;
	}
	if (rcItem.Width() <= cxEllipsis)
	{
		pDC->DrawText(strEllipsis, 3, rcItem, uFormat);
		return;
	}
	int nWidth = rcItem.Width() - cxEllipsis;

	int nResultLength = strItem.GetLength() * nWidth / cxItem;
	CString strResultLeft = strItem.Left(nResultLength - nResultLength / 2);
	CString strResultRight = strItem.Right(nResultLength / 2);

	while (GetPrefixTextExtent(pDC, strResultLeft, uFormat) + GetPrefixTextExtent(pDC, strResultRight, uFormat) < nWidth)
	{
		nResultLength++;
		strResultLeft = strItem.Left(nResultLength - nResultLength / 2);
		strResultRight = strItem.Right(nResultLength / 2);
	}

	while (GetPrefixTextExtent(pDC, strResultLeft, uFormat) + GetPrefixTextExtent(pDC, strResultRight, uFormat) > nWidth)
	{
		nResultLength--;
		strResultLeft = strItem.Left(nResultLength - nResultLength / 2);
		strResultRight = strItem.Right(nResultLength / 2);
	}

	pDC->DrawText(strResultLeft + strEllipsis + strResultRight, rcItem, uFormat);
}

void CXTPTabPaintManager::DrawFocusRect(CDC* pDC, CXTPTabManagerItem* /*pItem*/, CRect rcItem)
{
	if (!m_bDrawFocusRect)
		return;

	rcItem.DeflateRect(3, 3, 3, 3);
	pDC->SetTextColor(0);
	pDC->SetBkColor(0xFFFFFF);
	pDC->DrawFocusRect(rcItem);
}

int CXTPTabPaintManager::DrawSingleButtonIconAndText(CDC* pDC, CXTPTabManagerItem* pItem, CRect rcItem, BOOL bDraw)
{
	CXTPTabManager* pManager = pItem->GetTabManager();

	XTPTabLayoutStyle layout = pManager->GetLayout();

	BOOL bVertical = !pManager->IsHorizontalPosition();
	if (layout == xtpTabLayoutRotated) bVertical = !bVertical;

	CString strItem = pItem->GetCaption();
	CXTPFontDC fnt(pDC, m_bBoldSelected && pItem->IsSelected() && pManager->IsActive() ? GetBoldFont(bVertical) : GetFont(bVertical));


	CSize szIcon(m_szIcon);
	BOOL bDrawIcon = pManager->DrawIcon(pDC, 0, pItem, FALSE, szIcon);
	BOOL bDrawText = layout != xtpTabLayoutCompressed || pItem->IsSelected();

	int nNavigateButtonsLength = 0;
	if (bDrawText && pItem->GetNavigateButtons()->GetSize() > 0)
	{
		nNavigateButtonsLength = 0;
		for (int i = 0; i < (int)pItem->GetNavigateButtons()->GetSize(); i++)
		{
			CXTPTabManagerNavigateButton* pButton = pItem->GetNavigateButtons()->GetAt(i);
			if ((pButton->GetFlags() == xtpTabNavigateButtonAlways) ||
				((pButton->GetFlags() == xtpTabNavigateButtonAutomatic) && pItem->IsSelected()))
			{
				CSize sz = pButton->GetSize();
				nNavigateButtonsLength += bVertical ? sz.cy : sz.cx;
			}
		}

		if (nNavigateButtonsLength > 0) nNavigateButtonsLength += 3;
	}


	if (bDraw)
	{
		m_pColorSet->SetTextColor(pDC, pItem);

		if (nNavigateButtonsLength > 0)
		{
			if (bVertical) rcItem.bottom -= 3; else rcItem.right -= 3;

			for (int i = (int)pItem->GetNavigateButtons()->GetSize() - 1; i >= 0; i--)
			{
				CXTPTabManagerNavigateButton* pButton = pItem->GetNavigateButtons()->GetAt(i);
				if ((pButton->GetFlags() == xtpTabNavigateButtonAlways) ||
					((pButton->GetFlags() == xtpTabNavigateButtonAutomatic) && pItem->IsSelected()))
				{
					CSize szButton = pButton->GetSize();
					if (!bVertical)
					{
						pButton->SetRect(CRect(rcItem.right - szButton.cx,
							rcItem.CenterPoint().y + szButton.cy / 2 - szButton.cy, rcItem.right,
							rcItem.CenterPoint().y + szButton.cy / 2));
						rcItem.right -= szButton.cx;
					}
					else
					{
						pButton->SetRect(CRect(rcItem.CenterPoint().x - szButton.cx / 2,
							rcItem.bottom - szButton.cy, rcItem.CenterPoint().x - szButton.cx / 2 + szButton.cx, rcItem.bottom));
						rcItem.bottom -= szButton.cy;
					}
					pItem->GetNavigateButtons()->GetAt(i)->Draw(pDC);
				}
			}
		}
		CRect rcFocus(rcItem);

		if (bVertical)
		{
			int nAvailLength = pItem->GetButtonLength() - pItem->GetContentLength();
			if (nAvailLength > 0)
			{
				rcItem.DeflateRect(0, nAvailLength / 2);
			}

			rcItem.top += m_rcButtonTextPadding.left + m_rcButtonMargin.left;
			if (pManager->GetPosition() == xtpTabPositionRight)
				rcItem.DeflateRect(m_rcButtonMargin.bottom, 0, m_rcButtonMargin.top, 0);
			else
				rcItem.DeflateRect(m_rcButtonMargin.top, 0, m_rcButtonMargin.bottom, 0);

			if (bDrawIcon)
			{
				CPoint pt(rcItem.CenterPoint().x - szIcon.cx/2, rcItem.top - 1);
				if (rcItem.Height() > szIcon.cy) pManager->DrawIcon(pDC, pt, pItem, TRUE, szIcon);
				rcItem.top += szIcon.cy + 2;
			}
			if (bDrawText && rcItem.bottom > rcItem.top + 8)
			{
				CSize szText = pDC->GetTextExtent(strItem);
				rcItem.left = rcItem.right - (rcItem.Width() - szText.cy - (pManager->GetPosition() == xtpTabPositionRight ? 1 : 0)) / 2;
				rcItem.right = rcItem.left + rcItem.Height();
				rcItem.bottom -= m_rcButtonMargin.right + max(0, m_rcButtonTextPadding.right - 4);

				if (!m_bDrawTextNoPrefix)
					StripMnemonics(strItem);

				if (m_bDrawTextPathEllipsis)
				{
					DrawTextPathEllipsis(pDC, strItem, rcItem, DT_SINGLELINE | DT_NOCLIP | DT_NOPREFIX);
				}
				else
				{
					pDC->DrawText(strItem, &rcItem, DT_SINGLELINE | DT_NOCLIP |
						DT_NOPREFIX | (m_bDrawTextEndEllipsis ? DT_END_ELLIPSIS : 0));
				}
			}

		}
		else
		{
			int nAvailLength = pItem->GetButtonLength() - pItem->GetContentLength();
			if (nAvailLength > 0)
			{
				rcItem.DeflateRect(nAvailLength / 2, 0);
			}

			rcItem.left += m_rcButtonTextPadding.left + m_rcButtonMargin.left;

			if (pManager->GetPosition() == xtpTabPositionBottom)
				rcItem.DeflateRect(0, m_rcButtonMargin.bottom, 0, m_rcButtonMargin.top);
			else
				rcItem.DeflateRect(0, m_rcButtonMargin.top, 0, m_rcButtonMargin.bottom);

			if (bDrawIcon)
			{
				CPoint pt(rcItem.left - 1, rcItem.CenterPoint().y - szIcon.cy/2);
				if (rcItem.Width() > szIcon.cx) pManager->DrawIcon(pDC, pt, pItem, TRUE, szIcon);
				rcItem.left += szIcon.cx + 2;
			}

			if (bDrawText && rcItem.right > rcItem.left)
			{
				rcItem.right -= m_rcButtonTextPadding.right + m_rcButtonMargin.right - (bDrawIcon ? 2 : 0);
				if (m_bDrawTextPathEllipsis)
				{
					DrawTextPathEllipsis(pDC, strItem, rcItem, DT_SINGLELINE | m_nDrawTextFormat | (m_bDrawTextNoPrefix ? DT_NOPREFIX : 0));
				}
				else
				{
					if (m_bDrawTextHidePrefix)
						StripMnemonics(strItem);

					pDC->DrawText(strItem, rcItem, DT_SINGLELINE | m_nDrawTextFormat |
						(m_bDrawTextHidePrefix || m_bDrawTextNoPrefix ? DT_NOPREFIX : 0) | (m_bDrawTextEndEllipsis ? DT_END_ELLIPSIS : 0));
				}
			}
		}

		if (pItem->IsFocused())
		{
			DrawFocusRect(pDC, pItem, rcFocus);
		}
		return 0;
	}
	else
	{
		if (layout == xtpTabLayoutFixed)
		{
			return _GetButtonLength(m_nFixedTabWidth, m_nMinTabWidth, m_nMaxTabWidth);
		}

		if (layout == xtpTabLayoutCompressed)
		{
			if (!bDrawText)
				return _GetButtonLength(8 + (bVertical ? szIcon.cy : szIcon.cx), m_nMinTabWidth, m_nMaxTabWidth) ;

			CXTPFontDC fntItems(pDC, m_bBoldSelected && pManager->IsActive() ? GetBoldFont(bVertical) : GetFont(bVertical));

			int nLength = 0;

			for (int i = 0; i < pManager->GetItemCount(); i++)
			{
				CRect rcItemText(0, 0, 0, 0);
				pDC->DrawText(pManager->GetItem(i)->GetCaption(), rcItemText, DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP | (m_bDrawTextNoPrefix ? DT_NOPREFIX : 0));
				nLength = max(nLength, rcItemText.Width());
			}
			return _GetButtonLength(nNavigateButtonsLength + nLength + m_rcButtonTextPadding.left + m_rcButtonTextPadding.right +
				(!bDrawIcon ? 0: bVertical ? szIcon.cy : szIcon.cx), m_nMinTabWidth, m_nMaxTabWidth);
		}

		pDC->DrawText(strItem, rcItem, DT_SINGLELINE | DT_CALCRECT | DT_NOCLIP | (m_bDrawTextNoPrefix ? DT_NOPREFIX : 0));
		return _GetButtonLength(nNavigateButtonsLength + rcItem.Width() + m_rcButtonTextPadding.left + m_rcButtonTextPadding.right +
			(!bDrawIcon ? 0: bVertical ? szIcon.cy : szIcon.cx), m_nMinTabWidth, m_nMaxTabWidth);
	}
}


void CXTPTabPaintManager::AdjustClientRect(CXTPTabManager* pTabManager, CRect& rcClient)
{
	if (pTabManager->IsDrawStaticFrame())
		CAppearanceSet::DeflateRectEx(rcClient, CRect(2, 1, 2, 2), pTabManager->GetPosition());

	if (pTabManager->GetItemCount() > 0)
	{
		m_pAppearanceSet->AdjustClientRect(pTabManager, rcClient);
	}
}

CRect CXTPTabPaintManager::RepositionNavigateButtons(CXTPTabManager* pTabManager, CRect rcClient)
{
	CRect rcNavigateButtons = m_pAppearanceSet->GetHeaderRect(rcClient, pTabManager);

	if (pTabManager->IsHorizontalPosition())
		rcNavigateButtons.right -= 1;
	else
		rcNavigateButtons.bottom -= 1;

	for (int i = pTabManager->GetNavigateButtonCount() - 1; i >= 0; i--)
		pTabManager->GetNavigateButton(i)->Reposition(rcNavigateButtons);

	return rcNavigateButtons;
}

void CXTPTabPaintManager::RepositionTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient)
{
	pTabManager->m_rcHeaderRect.SetRectEmpty();
	pTabManager->m_rcClient = pTabManager->m_rcControl = rcClient;

	pTabManager->m_pRowIndexer->CreateIndexer(1);

	for (int i = 0; i < pTabManager->GetItemCount(); i++)
	{
		pTabManager->GetItem(i)->m_nItemRow = 0;
	}

	m_pAppearanceSet->RepositionTabControl(pTabManager, pDC, rcClient);
	AdjustClientRect(pTabManager, pTabManager->m_rcClient);
}



BOOL CXTPTabPaintManager::_CreateMultiRowIndexerBestFit(CXTPTabManager* pTabManager, int nWidth, int nRow, int nTotalLength)
{
	const int nRowCount = pTabManager->GetRowCount();
	const int nLengthPerRow = !m_bMultiRowJustified ? nWidth : min(nTotalLength / (nRowCount - nRow), nWidth);

	CXTPTabManager::ROW_ITEMS* pRowItems = pTabManager->m_pRowIndexer->GetRowItems();
	int x = 0;

	for (int i = pRowItems[nRow].nLastItem; i < pTabManager->GetItemCount(); i++)
	{
		CXTPTabManagerItem* pItem = pTabManager->GetItem(i);
		int nLength = pItem->GetButtonLength();

		if (x + nLength > nLengthPerRow && x != 0 && nLength > 0)
		{
			if (nRow == nRowCount - 1)
				return FALSE;

			pRowItems[nRow + 1].nFirstItem = pRowItems[nRow + 1].nLastItem = i;

			if (_CreateMultiRowIndexerBestFit(pTabManager, nWidth, nRow + 1, nTotalLength - x))
				return TRUE;

			if (x + nLength > nWidth)
				return FALSE;
		}
		pRowItems[nRow].nLastItem = i;
		pItem->m_nItemRow = nRow;
		x += nLength;
	}
	return TRUE;
}

void CXTPTabPaintManager::CreateMultiRowIndexer(CXTPTabManager* pTabManager, CDC* pDC, int nWidth)
{
	int x = 0;
	int nRowCount = 1;
	int i;
	int nTotalLength = 0;
	int nItemCount = pTabManager->GetItemCount();
	CXTPTabManagerItem* pSelectedItem = 0;

	for (i = 0; i < nItemCount; i++)
	{
		CXTPTabManagerItem* pItem = pTabManager->GetItem(i);

		if (pItem->IsSelected())
		{
			pSelectedItem = pItem;
		}

		int nLength = pItem->m_nContentLength = pItem->m_nButtonLength =
			pItem->IsVisible() ? m_pAppearanceSet->GetButtonLength(pDC, pItem) : 0;

		if (x + nLength > nWidth && x != 0)
		{
			x = 0;
			nRowCount++;
		}
		x += nLength;
		nTotalLength += nLength;
	}

	CXTPTabManager::ROW_ITEMS* pRowItems = pTabManager->m_pRowIndexer->CreateIndexer(nRowCount);

	if (nRowCount == 1)
		return;

	pRowItems[0].nFirstItem = pRowItems[0].nLastItem = 0;
	VERIFY(_CreateMultiRowIndexerBestFit(pTabManager, nWidth, 0, nTotalLength));

	if (!m_bMultiRowFixedSelection && pSelectedItem)
	{
		int nSelectedRow = pSelectedItem->GetItemRow();

		CXTPTabManager::ROW_ITEMS selectedRow = pRowItems[nSelectedRow];
		pRowItems[nSelectedRow] = pRowItems[0];
		pRowItems[0] = selectedRow;
	}
}

void CXTPTabPaintManager::RepositionTabControlMultiRow(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient)
{

	pTabManager->m_rcHeaderRect = RepositionNavigateButtons(pTabManager, rcClient);
	pTabManager->m_nHeaderOffset = 0;

	if (pTabManager->GetItemCount() == 0)
		return;

	CRect rcHeaderMargin = m_pAppearanceSet->GetHeaderMargin();
	int nButtonHeight = m_pAppearanceSet->GetButtonHeight(pTabManager);

	if (pTabManager->IsHorizontalPosition())
	{
		int nWidth = pTabManager->m_rcHeaderRect.Width() - (rcHeaderMargin.left + rcHeaderMargin.right);
		CreateMultiRowIndexer(pTabManager, pDC, nWidth);

		int nRowCount = pTabManager->GetRowCount();
		int nTop = 0;

		if (pTabManager->GetPosition() == xtpTabPositionBottom)
		{
			nTop = rcClient.bottom - (nButtonHeight * nRowCount + m_pAppearanceSet->m_nRowMargin * (nRowCount - 1))
				- rcHeaderMargin.top;
		}
		else
		{
			nTop = rcClient.top +  (nButtonHeight * (nRowCount - 1) + m_pAppearanceSet->m_nRowMargin * (nRowCount - 1))
				+ rcHeaderMargin.top;
		}

		int nBottom = nTop + nButtonHeight;

		int nOffset = pTabManager->GetPosition() == xtpTabPositionBottom ?
			nButtonHeight + m_pAppearanceSet->m_nRowMargin: -(nButtonHeight + m_pAppearanceSet->m_nRowMargin);


		CXTPTabManager::ROW_ITEMS* pRowItems = pTabManager->m_pRowIndexer->GetRowItems();
		int i;

		BOOL bSizeToFit = nRowCount > 1 && m_bMultiRowJustified;

		for (int nRow = 0; nRow < nRowCount; nRow++)
		{
			int nTotalLength = 0;

			int nItemInRow = pRowItems[nRow].nLastItem - pRowItems[nRow].nFirstItem + 1;
			int nFirstItem = pRowItems[nRow].nFirstItem;

			if (bSizeToFit)
			{
				for (i = 0; i < nItemInRow; i++)
				{
					nTotalLength += pTabManager->GetItem(nFirstItem + i)->GetButtonLength();
				}
			}

			int nTotalWidth = nWidth;

			int x = pTabManager->m_rcHeaderRect.left + rcHeaderMargin.left;

			for (i = 0; i < nItemInRow; i++)
			{
				CXTPTabManagerItem* pItem = pTabManager->GetItem(nFirstItem + i);
				ASSERT(pItem);
				if (!pItem)
					continue;
				int nButtonLength = pItem->m_nButtonLength;

				if (bSizeToFit)
				{
					int nLength = nButtonLength + (nTotalWidth - nTotalLength) / (nItemInRow - i);
					nTotalWidth -= nLength;
					nTotalLength -= nButtonLength;

					pItem->m_nButtonLength = nButtonLength = nLength;
				}

				pItem->SetRect(CRect(x, nTop, x + nButtonLength, nBottom));
				pItem->m_nItemRow = nRow;


				x += nButtonLength;
			}
			nTop += nOffset;
			nBottom += nOffset;
		}
	}
	else
	{
		int nWidth = pTabManager->m_rcHeaderRect.Height() - (rcHeaderMargin.left + rcHeaderMargin.right);
		CreateMultiRowIndexer(pTabManager, pDC, nWidth);


		int nRowCount = pTabManager->GetRowCount();
		int nLeft = 0;

		if (pTabManager->GetPosition() == xtpTabPositionRight)
		{
			nLeft = rcClient.right - (nButtonHeight * nRowCount + m_pAppearanceSet->m_nRowMargin * (nRowCount - 1))
				- rcHeaderMargin.top;
		}
		else
		{
			nLeft = rcClient.left +  (nButtonHeight * (nRowCount - 1) + m_pAppearanceSet->m_nRowMargin * (nRowCount - 1))
				+ rcHeaderMargin.top;
		}

		int nRight = nLeft + nButtonHeight;

		int nOffset = pTabManager->GetPosition() == xtpTabPositionRight ?
			nButtonHeight + m_pAppearanceSet->m_nRowMargin: -(nButtonHeight + m_pAppearanceSet->m_nRowMargin);


		CXTPTabManager::ROW_ITEMS* pRowItems = pTabManager->m_pRowIndexer->GetRowItems();
		int i;

		BOOL bSizeToFit = nRowCount > 1 && m_bMultiRowJustified;

		for (int nRow = 0; nRow < nRowCount; nRow++)
		{
			int nTotalLength = 0;

			int nItemInRow = pRowItems[nRow].nLastItem - pRowItems[nRow].nFirstItem + 1;
			int nFirstItem = pRowItems[nRow].nFirstItem;

			if (bSizeToFit)
			{
				for (i = 0; i < nItemInRow; i++)
				{
					nTotalLength += pTabManager->GetItem(nFirstItem + i)->GetButtonLength();
				}
			}

			int nTotalWidth = nWidth;

			int y = pTabManager->m_rcHeaderRect.top + rcHeaderMargin.left;

			for (i = 0; i < nItemInRow; i++)
			{
				CXTPTabManagerItem* pItem = pTabManager->GetItem(nFirstItem + i);
				ASSERT(pItem);
				int nButtonLength = pItem->m_nButtonLength;

				if (bSizeToFit)
				{
					int nLength = nButtonLength + (nTotalWidth - nTotalLength) / (nItemInRow - i);
					nTotalWidth -= nLength;
					nTotalLength -= nButtonLength;

					pItem->m_nButtonLength = nButtonLength = nLength;
				}

				pItem->SetRect(CRect(nLeft, y, nRight, y + nButtonLength));
				pItem->m_nItemRow = nRow;


				y += nButtonLength;
			}
			nLeft += nOffset;
			nRight += nOffset;
		}
	}
	pTabManager->m_rcHeaderRect = RepositionNavigateButtons(pTabManager, rcClient);
}

void CXTPTabPaintManager::RepositionTabControlEx(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient)
{
	if (pTabManager->IsDrawStaticFrame())
	{
		CAppearanceSet::DeflateRectEx(rcClient, CRect(2, 1, 2, 2), pTabManager->GetPosition());
	}

	CAppearanceSet::DeflateRectEx(rcClient, m_rcControlMargin, pTabManager->GetPosition());

	if (pTabManager->GetLayout() == xtpTabLayoutMultiRow)
	{
		RepositionTabControlMultiRow(pTabManager, pDC, rcClient);
		return;
	}

	int i;

	for (i = 0; i < pTabManager->GetItemCount(); i++)
	{
		CXTPTabManagerItem* pItem = pTabManager->GetItem(i);

		pItem->m_nButtonLength = pItem->m_nContentLength =
			pItem->IsVisible() ? m_pAppearanceSet->GetButtonLength(pDC, pItem) : 0;

		if (pItem->IsVisible())
			pItem->m_nButtonLength += m_nButtonExtraLength;
	}

	pTabManager->m_rcHeaderRect = m_pAppearanceSet->GetHeaderRect(rcClient, pTabManager);

	if (pTabManager->GetItemCount() == 0)
	{
		RepositionNavigateButtons(pTabManager, rcClient);
		return;
	}

	CRect rcHeaderMargin = m_pAppearanceSet->GetHeaderMargin();
	int nButtonHeight = m_pAppearanceSet->GetButtonHeight(pTabManager);


	if (pTabManager->IsHorizontalPosition())
	{
		int nTop = rcClient.top + rcHeaderMargin.top;

		if (pTabManager->GetPosition() == xtpTabPositionBottom)
		{
			nTop = rcClient.bottom - nButtonHeight - rcHeaderMargin.top;
		}

		int nBottom = nTop + nButtonHeight;


		if (pTabManager->GetLayout() == xtpTabLayoutSizeToFit)
		{
			int nWidth = rcClient.Width() - (rcHeaderMargin.right + rcHeaderMargin.left);

			for (i = pTabManager->GetNavigateButtonCount() - 1; i >= 0; i--)
				pTabManager->GetNavigateButton(i)->AdjustWidth(nWidth);

			SizeToFit(pTabManager, nWidth);
		}

		pTabManager->m_rcHeaderRect = RepositionNavigateButtons(pTabManager, rcClient);

		if (pTabManager->m_nHeaderOffset < 0)
		{
			int nLength = pTabManager->GetItemsLength();
			int nNavigateButtonsWidth = pTabManager->m_rcHeaderRect.Width() - rcHeaderMargin.left - rcHeaderMargin.right;

			if (nLength + pTabManager->m_nHeaderOffset < nNavigateButtonsWidth)
			{
				pTabManager->m_nHeaderOffset = min(0, nNavigateButtonsWidth - nLength);
				pTabManager->m_rcHeaderRect = RepositionNavigateButtons(pTabManager, rcClient);
			}
		}

		int x = pTabManager->m_rcHeaderRect.left + rcHeaderMargin.left + pTabManager->GetHeaderOffset();

		if (pTabManager->GetLayout() == xtpTabLayoutRotated)
		{
			if (pTabManager->GetPosition() == xtpTabPositionTop)
			{
				nBottom = pTabManager->m_rcHeaderRect.bottom - rcHeaderMargin.bottom;
			}
			else
			{
				nTop = pTabManager->m_rcHeaderRect.top + rcHeaderMargin.bottom;

			}

			for (i = 0; i < pTabManager->GetItemCount(); i++)
			{
				CXTPTabManagerItem* pItem = pTabManager->GetItem(i);

				if (pTabManager->GetPosition() == xtpTabPositionTop)
				{
					pItem->SetRect(CRect(x, nBottom - pItem->m_nButtonLength, x + nButtonHeight, nBottom));
				}
				else
				{
					pItem->SetRect(CRect(x, nTop, x + nButtonHeight, nTop + pItem->m_nButtonLength));

				}
				x += nButtonHeight;
			}
		}
		else
		{

			for (i = 0; i < pTabManager->GetItemCount(); i++)
			{
				CXTPTabManagerItem* pItem = pTabManager->GetItem(i);

				pItem->SetRect(CRect(x, nTop, x + pItem->m_nButtonLength, nBottom));
				x += pItem->m_nButtonLength;
			}
		}

	}
	else
	{
		int nLeft = rcClient.left + rcHeaderMargin.top;

		if (pTabManager->GetPosition() == xtpTabPositionRight)
		{
			nLeft = rcClient.right - nButtonHeight - rcHeaderMargin.top;
		}

		int nRight = nLeft + nButtonHeight;

		if (pTabManager->GetLayout() == xtpTabLayoutSizeToFit)
		{
			int nWidth = rcClient.Height() - (rcHeaderMargin.right + rcHeaderMargin.left);

			for (i = pTabManager->GetNavigateButtonCount() - 1; i >= 0; i--)
				pTabManager->GetNavigateButton(i)->AdjustWidth(nWidth);

			SizeToFit(pTabManager, nWidth);
		}

		pTabManager->m_rcHeaderRect = RepositionNavigateButtons(pTabManager, rcClient);

		if (pTabManager->m_nHeaderOffset < 0)
		{
			int nLength = pTabManager->GetItemsLength();
			int nNavigateButtonsWidth = pTabManager->m_rcHeaderRect.Height() - rcHeaderMargin.left - rcHeaderMargin.right;

			if (nLength + pTabManager->m_nHeaderOffset < nNavigateButtonsWidth)
			{
				pTabManager->m_nHeaderOffset = min(0, nNavigateButtonsWidth - nLength);
				pTabManager->m_rcHeaderRect = RepositionNavigateButtons(pTabManager, rcClient);
			}
		}

		int y = pTabManager->m_rcHeaderRect.top + rcHeaderMargin.left + pTabManager->GetHeaderOffset();

		if (pTabManager->GetLayout() == xtpTabLayoutRotated)
		{
			if (pTabManager->GetPosition() == xtpTabPositionLeft)
			{
				nRight = pTabManager->m_rcHeaderRect.right - rcHeaderMargin.bottom;
			}
			else
			{
				nLeft = pTabManager->m_rcHeaderRect.left + rcHeaderMargin.bottom;

			}

			for (i = 0; i < pTabManager->GetItemCount(); i++)
			{
				CXTPTabManagerItem* pItem = pTabManager->GetItem(i);

				if (pTabManager->GetPosition() == xtpTabPositionLeft)
				{
					pItem->SetRect(CRect(nRight - pItem->m_nButtonLength, y, nRight, y + nButtonHeight));
				}
				else
				{
					pItem->SetRect(CRect(nLeft, y, nLeft + pItem->m_nButtonLength, y + nButtonHeight));

				}
				y += nButtonHeight;
			}
		}
		else
		{
			for (i = 0; i < pTabManager->GetItemCount(); i++)
			{
				CXTPTabManagerItem* pItem = pTabManager->GetItem(i);

				pItem->SetRect(CRect(nLeft, y, nRight, y + pItem->m_nButtonLength));
				y += pItem->m_nButtonLength;
			}
		}
	}
}

void CXTPTabPaintManager::EnsureVisible(CXTPTabManager* pTabManager, CXTPTabManagerItem* pItem)
{
	CRect rcHeader = pTabManager->m_rcHeaderRect;
	CRect rcItem = pItem->GetRect();

	if (rcHeader.IsRectEmpty() || rcItem.IsRectEmpty())
		return;

	if (GetLayout() == xtpTabLayoutSizeToFit)
		return;

	if (pTabManager->IsHorizontalPosition())
	{
		if (rcItem.left < rcHeader.left)
		{
			pTabManager->SetHeaderOffset(pTabManager->GetHeaderOffset() - rcItem.left + rcHeader.left + m_pAppearanceSet->GetHeaderMargin().left);
		}
		else if (rcItem.right > rcHeader.right)
		{
			pTabManager->SetHeaderOffset(pTabManager->GetHeaderOffset() - rcItem.right + rcHeader.right);
		}

	}
	else
	{
		if (rcItem.top < rcHeader.top)
		{
			pTabManager->SetHeaderOffset(pTabManager->GetHeaderOffset() - rcItem.top + rcHeader.top + m_pAppearanceSet->GetHeaderMargin().left);
		}
		else if (rcItem.bottom > rcHeader.bottom)
		{
			pTabManager->SetHeaderOffset(pTabManager->GetHeaderOffset() - rcItem.bottom + rcHeader.bottom);
		}

	}

}

class CXTPTabPaintManager::CSizeToFitSorter
{
public:

	static int _cdecl CompareRect(const void *arg1, const void *arg2)
	{
		int nLength1 = s_pTabManager->GetItem(*(int*)arg1)->GetButtonLength();
		int nLength2 = s_pTabManager->GetItem(*(int*)arg2)->GetButtonLength();
		if (nLength1 > nLength2) return -1;
		if (nLength1 < nLength2) return 1;

		return 0;
	}
	static CXTPTabManager* s_pTabManager;
};
CXTPTabManager* CXTPTabPaintManager::CSizeToFitSorter::s_pTabManager;

void CXTPTabPaintManager::SizeToFit(CXTPTabManager* pTabManager, int nWidth)
{
	#define GetItemLength(nIndex) pTabManager->GetItem(pIndex[nIndex])->m_nButtonLength

	int nLength = pTabManager->GetItemsLength();
	int nCount = pTabManager->GetItemCount();

	if (nWidth < nLength)
	{
		if (nCount == 1)
			pTabManager->GetItem(0)->m_nButtonLength = nWidth;
		else
		{
			int* pIndex = new int[nCount];
			for (int c = 0; c < nCount; c++)
			{
				pIndex[c] = c;
			}

			CSizeToFitSorter::s_pTabManager = pTabManager;
			qsort(pIndex, pTabManager->GetItemCount(), sizeof(int), CSizeToFitSorter::CompareRect);

			int nDelta = nLength - nWidth;

			for (int i = 0; i < nCount;)
			{
				int nItemWidth = GetItemLength(i);

				int k = i + 1;
				while (k < nCount && GetItemLength(k) == nItemWidth)
				{
					k++;
				}

				if (k < nCount)
				{
					int nNextWidth = GetItemLength(k);
					int nCanFree = k * (nItemWidth - nNextWidth);

					if (nCanFree >= nDelta)
					{
						for (int j = 0; j < k; j++)
						{
							GetItemLength(j) -= nDelta / k;
						}
						break;

					}
					else
					{
						nDelta -= nCanFree;
						for (int j = 0; j < k; j++)
						{
							GetItemLength(j) = nNextWidth;
						}
					}
				}
				else
				{
					for (int j = 0; j < nCount; j++)
					{
						GetItemLength(j) = nWidth / nCount;
					}
					break;
				}
				i = k;
			}

			delete[] pIndex;
		}
	}
}

XTPTabAppearanceStyle CXTPTabPaintManager::GetAppearance() const
{
	return m_tabAppearance;
}

XTPTabColorStyle CXTPTabPaintManager::GetColor() const
{
	return m_tabColor;
}

void CXTPTabPaintManager::SetPosition(XTPTabPosition tabPosition)
{
	m_tabPosition = tabPosition;
	OnPropertyChanged();

}

void CXTPTabPaintManager::SetLayout(XTPTabLayoutStyle tabLayout)
{
	m_tabLayout = tabLayout;
	OnPropertyChanged();
}

XTPTabPosition CXTPTabPaintManager::GetPosition() const
{
	return m_tabPosition;
}

XTPTabLayoutStyle CXTPTabPaintManager::GetLayout() const
{
	return m_tabLayout;
}


void CXTPTabPaintManager::OnPropertyChanged()
{
	for (int i = 0; i < m_arrObservers.GetSize(); i++)
	{
		m_arrObservers[i]->OnPropertyChanged();
	}
}


void CXTPTabPaintManager::AddObserver(CXTPTabManagerAtom* pObserver)
{
	for (int i = 0; i < m_arrObservers.GetSize(); i++)
	{
		if (m_arrObservers[i] == pObserver)
			return;
	}
	m_arrObservers.Add(pObserver);
}

//////////////////////////////////////////////////////////////////////////

