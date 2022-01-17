// XTPPropertyGridPaintManager.cpp : implementation of the CXTPPropertyGridPaintManager class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"

#include "XTPPropertyGridPaintManager.h"
#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGrid.h"

#include "XTPPropertyGridItemBool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CXTPPropertyGridItemMetrics::CXTPPropertyGridItemMetrics(CXTPPropertyGridPaintManager* pPaintManager)
	: m_pPaintManager(pPaintManager)
{
	m_nImage = -1;
	m_uDrawTextFormat = DT_VCENTER | DT_SINGLELINE;
	m_nMaxLength = 0;

}
void CXTPPropertyGridItemMetrics::SetDefaultValues()
{
	m_clrHelpBack.SetDefaultValue();
	m_clrHelpFore.SetDefaultValue();
	m_clrLine.SetDefaultValue();
	m_clrBack.SetDefaultValue();
	m_clrFore.SetDefaultValue();
	m_clrReadOnlyFore.SetDefaultValue();
	m_clrCategoryFore.SetDefaultValue();
	m_clrVerbFace.SetDefaultValue();
}

COLORREF CXTPPropertyGridPaintManager::GetItemTextColor(CXTPPropertyGridItem* pItem, BOOL bValuePart)
{
	ASSERT(pItem);
	if (!pItem)
		return m_pMetrics->m_clrFore;

	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(bValuePart, FALSE);
	if (pMetrics && !pMetrics->m_clrFore.IsDefaultValue())
		return pMetrics->m_clrFore;

	return pItem->IsCategory() ? m_pMetrics->m_clrCategoryFore :
		pItem->GetReadOnly() ? m_pMetrics->m_clrReadOnlyFore : m_pMetrics->m_clrFore;
}

COLORREF CXTPPropertyGridPaintManager::GetItemBackColor(CXTPPropertyGridItem* pItem, BOOL bValuePart)
{
	ASSERT(pItem);
	if (!pItem)
		return m_pMetrics->m_clrBack;

	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(bValuePart, FALSE);
	if (pMetrics && !pMetrics->m_clrBack.IsDefaultValue())
		return pMetrics->m_clrBack;

	return m_pMetrics->m_clrBack;
}

CFont* CXTPPropertyGridPaintManager::GetItemFont(CXTPPropertyGridItem* pItem, BOOL bValuePart)
{
	ASSERT(pItem);
	if (!pItem)
		return &m_pMetrics->m_fontNormal;

	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(bValuePart, FALSE);
	if (pMetrics && pMetrics->m_fontNormal.GetSafeHandle())
		return &pMetrics->m_fontNormal;

	if (pItem->IsCategory())
		return &m_pMetrics->m_fontBold;

	if (bValuePart && m_pGrid->IsHighlightChangedItems() && pItem->IsValueChanged())
		return &m_pMetrics->m_fontBold;

	return &m_pMetrics->m_fontNormal;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPPropertyGridPaintManager::CXTPPropertyGridPaintManager(CXTPPropertyGrid* pGrid)
	: m_pGrid(pGrid)
{
	m_buttonsStyle = xtpGridButtonsDefault;

	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));
	lfIcon.lfWeight = FW_NORMAL;
	lfIcon.lfItalic = FALSE;

	VERIFY(m_fntVerbNormal.CreateFontIndirect(&lfIcon));
	lfIcon.lfUnderline = TRUE;
	VERIFY(m_fntVerbUnderline.CreateFontIndirect(&lfIcon));

	m_pMetrics = new CXTPPropertyGridItemMetrics(this);
}

CXTPPropertyGridPaintManager::~CXTPPropertyGridPaintManager()
{
	CMDTARGET_RELEASE(m_pMetrics);
}

void CXTPPropertyGridPaintManager::RefreshMetrics()
{
	RefreshXtremeColors();

	m_clrFace = GetXtremeColor(COLOR_3DFACE);
	m_clrShadow = GetXtremeColor(COLOR_3DSHADOW);

	m_pMetrics->m_clrVerbFace = GetXtremeColor(COLOR_ACTIVECAPTION);
	m_pMetrics->m_clrHelpBack.SetStandardValue(m_clrFace);
	m_pMetrics->m_clrHelpFore.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_pMetrics->m_clrLine.SetStandardValue(GetSysColor(COLOR_SCROLLBAR));
	m_pMetrics->m_clrCategoryFore.SetStandardValue(GetXtremeColor(COLOR_GRAYTEXT));
	m_pMetrics->m_clrBack.SetStandardValue(GetXtremeColor(COLOR_WINDOW));
	m_pMetrics->m_clrFore.SetStandardValue(GetXtremeColor(COLOR_WINDOWTEXT));
	m_pMetrics->m_clrReadOnlyFore.SetStandardValue(GetXtremeColor(COLOR_GRAYTEXT));

	HWND hWnd = AfxGetMainWnd() ? AfxGetMainWnd()->GetSafeHwnd() : 0;
	m_themeTree.OpenThemeData(hWnd, L"TREEVIEW");
	m_themeButton.OpenThemeData(hWnd, L"BUTTON");
	m_themeCombo.OpenThemeData(hWnd, L"COMBOBOX");
}

void CXTPPropertyGridPaintManager::FillPropertyGrid(CDC* pDC)
{
	CXTPClientRect rc(m_pGrid);

	pDC->FillSolidRect(rc, m_clrFace);
	pDC->SetBkMode(TRANSPARENT);

	if (m_pGrid->IsHelpVisible())
	{
		CXTPPropertyGridItem* pItem = m_pGrid->GetSelectedItem();

		CRect rcHelp(rc);
		rcHelp.top = rc.bottom - m_pGrid->m_nHelpHeight;

		pDC->FillSolidRect(rcHelp, m_pMetrics->m_clrHelpBack);
		DrawPropertyGridBorder(pDC, rcHelp, FALSE);


		CXTPFontDC fontCaption(pDC, &m_pMetrics->m_fontBold);

		if (pItem)
		{
			pDC->SetTextColor(m_pMetrics->m_clrHelpFore);

			pDC->SetBkMode(TRANSPARENT);
			CRect rcCaption(rcHelp);
			rcCaption.DeflateRect(6, 3);
			pDC->DrawText(pItem->GetCaption(), rcCaption, DT_SINGLELINE | DT_NOPREFIX);

			CString strDesc = pItem->GetDescription();
			if (!strDesc.IsEmpty())
			{
				CXTPFontDC fontDescription(pDC, &m_pMetrics->m_fontNormal);
				CXTPEmptyRect rcCaptionHeight;
				pDC->DrawText(pItem->GetCaption(), rcCaptionHeight, DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);
				CRect rcDesc(rcHelp);
				rcDesc.DeflateRect(6, 3 + rcCaptionHeight.Height() + 3, 6, 3);
				pDC->DrawText(strDesc, rcDesc, DT_WORDBREAK | DT_NOPREFIX);
			}
		}

		rc.bottom -= m_pGrid->m_nHelpHeight + 3;
	}

	if (m_pGrid->IsVerbsVisible())
	{
		CRect rcVerbs(rc);
		rcVerbs.top = rc.bottom - m_pGrid->m_nVerbsHeight;

		pDC->FillSolidRect(rcVerbs, m_pMetrics->m_clrHelpBack);
		DrawPropertyGridBorder(pDC, rcVerbs, FALSE);

		HitTestVerbs(pDC, rcVerbs, CPoint(0));
	}
}

int CXTPPropertyGridPaintManager::HitTestVerbs(CDC* pDC, CRect rcVerbs, CPoint pt)
{
	#define DRAWTEXTPART(strCaption)\
		CRect rcText(CRect(x, y, min(x + nTextWidth, rcVerbs.right), min(y + nTextHeight, rcVerbs.bottom)));\
		if (bDraw)\
		{\
			pVerb->SetPart(rcText);\
			pDC->SetTextColor(m_pMetrics->m_clrVerbFace);\
			pDC->DrawText(strCaption, rcText, DT_SINGLELINE | DT_NOPREFIX);\
			if (pVerb->IsFocused())\
			{\
				pDC->SetTextColor(m_pMetrics->m_clrHelpBack);\
				pDC->SetBkColor(0);\
				rcText.bottom++;\
				pDC->DrawFocusRect(rcText);\
			}\
		}\
		else if (rcText.PtInRect(pt)) return i;

	BOOL bDraw = (pt == CPoint(0));

	CXTPFontDC font(pDC, &m_fntVerbUnderline);

	rcVerbs.DeflateRect(6, 4);

	int nCount = m_pGrid->GetVerbs()->GetCount();

	int x = rcVerbs.left, y = rcVerbs.top, nWidth = rcVerbs.Width();
	int nTextHeight = pDC->GetTextExtent(_T("XXX"), 3).cy;

	for (int i = 0; i < nCount; i++)
	{
		CXTPPropertyGridVerb* pVerb = m_pGrid->GetVerbs()->GetAt(i);
		CString strCaption = pVerb->GetCaption();

		for (;;)
		{
			if (strCaption.IsEmpty())
				break;

			int nTextWidth = pDC->GetTextExtent(strCaption).cx;

			if (x + nTextWidth < nWidth)
			{
				DRAWTEXTPART(strCaption);
				x += nTextWidth;
				break;
			}
			else
			{
				int nStartIndex = (strCaption.GetLength() - 1) * (nWidth - x) / nTextWidth;
				int nIndex = nStartIndex;

				if ((nStartIndex < 1) && (x == rcVerbs.left))
				{
					i = nCount - 1;
					break;
				}

				for (; nIndex >= 0; nIndex--)
				{
					if (strCaption[nIndex] == _T(' ') || strCaption[nIndex] == _T(',') || strCaption[nIndex] == _T('-'))
						break;
				}

				if (nIndex > 0 || (x == rcVerbs.left))
				{
					if (nIndex <= 0) nIndex = nStartIndex;

					CString strLeft = strCaption.Left(nIndex + 1);
					CString strRight = strCaption.Mid(nIndex + 1);

					nTextWidth = pDC->GetTextExtent(strLeft).cx;
					DRAWTEXTPART(strLeft);

					x = rcVerbs.left;
					y += nTextHeight;
					strCaption = strRight;
				}
				else
				{
					y += nTextHeight;
					x = rcVerbs.left;
				}
			}
			if (y >= rcVerbs.bottom)
				break;
		}

		if ((y >= rcVerbs.bottom) || (i == nCount - 1))
			break;

		if (bDraw)
		{
			font.SetFont(&m_fntVerbNormal);
			pDC->SetTextColor(m_pMetrics->m_clrHelpFore);
			pDC->DrawText(_T(","), 1, CRect(x, y, x + 8, min(y + nTextHeight, rcVerbs.bottom)), DT_SINGLELINE | DT_NOPREFIX);
			font.SetFont(&m_fntVerbUnderline);
		}
		x += 8;
	}
	return -1;
}

void CXTPPropertyGridPaintManager::DrawPropertyGridBorder(CDC* pDC, RECT& rc, BOOL bAdjustRect)
{
	XTPPropertyGridBorderStyle borderStyle = m_pGrid->GetBorderStyle();

	if (pDC)
	{
		switch (borderStyle)
		{
		case xtpGridBorderStaticEdge:
			pDC->Draw3dRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				m_clrShadow, m_clrShadow);
			break;

		case xtpGridBorderFlat:
			pDC->Draw3dRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				GetXtremeColor(COLOR_WINDOWFRAME), GetXtremeColor(COLOR_WINDOWFRAME));
			break;

		case xtpGridBorderClientEdge:
			pDC->Draw3dRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
			pDC->Draw3dRect(rc.left + 1, rc.top + 1, rc.right - rc.left - 2, rc.bottom - rc.top - 2,
				GetXtremeColor(COLOR_3DDKSHADOW), GetXtremeColor(COLOR_3DFACE));
			break;
		}
	}

	if (bAdjustRect)
	{
		int nSize = borderStyle == xtpGridBorderClientEdge ? 2 :
			borderStyle == xtpGridBorderStaticEdge || borderStyle == xtpGridBorderFlat ? 1 : 0;

		rc.left   += nSize;
		rc.top    += nSize;
		rc.right  -= nSize;
		rc.bottom -= nSize;
	}
}

void CXTPPropertyGridPaintManager::FillPropertyGridView(CDC* pDC)
{
	CXTPPropertyGridView* pView = &m_pGrid->GetGridView();

	CXTPClientRect rc(pView);
	pDC->FillSolidRect(rc, m_pMetrics->m_clrBack);

}

void CXTPPropertyGridPaintManager::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CXTPPropertyGridView* pView = &m_pGrid->GetGridView();
	if (pView->GetStyle() & LBS_OWNERDRAWVARIABLE)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpMeasureItemStruct->itemData;
		pItem->MeasureItem(lpMeasureItemStruct);
	}
}

void CXTPPropertyGridPaintManager::DrawCategoryCaptionBackground(CDC* pDC, CRect rc)
{
	pDC->FillSolidRect(rc, m_pMetrics->m_clrLine);
}

void CXTPPropertyGridPaintManager::DrawInplaceListItem(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint, CRect rc, BOOL bSelected)
{
	pDC->SetBkMode(TRANSPARENT);

	if (bSelected)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_HIGHLIGHT));
	}
	else
	{
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));
	}

	CRect rcText(rc.left + 2, rc.top, rc.right - 2, rc.bottom);

	CXTPImageManagerIcon* pImage = pConstraint->GetImage();

	if (pImage)
	{
		int nHeight = min(pImage->GetHeight(), rc.Height());
		int nWidth = pImage->GetWidth();

		CPoint pt(rcText.left, rcText.CenterPoint().y - nHeight/2);
		pImage->Draw(pDC, pt, CSize(nWidth, nHeight));

		rcText.left += nWidth + 5;
	}

	// Draw the text.
	pDC->DrawText(pConstraint->m_strConstraint, rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
}


void CXTPPropertyGridPaintManager::AdjustItemValueRect(CXTPPropertyGridItem* pItem, CRect& rcValue)
{
	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(TRUE, FALSE);
	if (pMetrics && pMetrics->m_nImage != -1)
	{
		CXTPImageManagerIcon* pImage = m_pGrid->GetImageManager()->GetImage(pMetrics->m_nImage, 0);

		if (pImage)
		{
			int nWidth = pImage->GetWidth();
			rcValue.left += nWidth + 5;
		}
	}
}

void CXTPPropertyGridPaintManager::AdjustItemCaptionRect(CXTPPropertyGridItem* pItem, CRect& rcCaption)
{
	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(FALSE, FALSE);
	if (pMetrics && pMetrics->m_nImage != -1)
	{
		CXTPImageManagerIcon* pImage = m_pGrid->GetImageManager()->GetImage(pMetrics->m_nImage, 0);

		if (pImage)
		{
			int nWidth = pImage->GetWidth();
			rcCaption.left += nWidth + 5;
		}
	}
}

void CXTPPropertyGridPaintManager::DrawItemValue(CDC* pDC, CXTPPropertyGridItem* pItem, CRect rcValue)
{
	pDC->SetTextColor(GetItemTextColor(pItem, TRUE));
	pDC->FillSolidRect(rcValue, GetItemBackColor(pItem, TRUE));

	rcValue.left += 4;

	if (pItem->OnDrawItemValue(*pDC, rcValue))
		return;

	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(TRUE, FALSE);
	if (pMetrics && pMetrics->m_nImage != -1)
	{
		CXTPImageManagerIcon* pImage = m_pGrid->GetImageManager()->GetImage(pMetrics->m_nImage, 0);

		if (pImage)
		{
			int nHeight = min(pImage->GetHeight(), rcValue.Height());
			int nWidth = pImage->GetWidth();

			CPoint pt(rcValue.left - 2, rcValue.CenterPoint().y - nHeight/2);
			pImage->Draw(pDC, pt, CSize(nWidth, nHeight));

			rcValue.left += nWidth + 5;

		}

	}
	rcValue.DeflateRect(0, 1);

	UINT nFormat = pMetrics ? pMetrics->m_uDrawTextFormat : DT_VCENTER | DT_SINGLELINE;

	if (pItem->IsMultiLine()) nFormat = (nFormat | DT_WORDBREAK) & ~DT_SINGLELINE;

	pDC->DrawText(pItem->GetViewValue(), rcValue, DT_NOPREFIX | nFormat);
}

void CXTPPropertyGridPaintManager::DrawItemCaption(CDC* pDC, CXTPPropertyGridItem* pItem, CRect rcCaption)
{
	CRect rcText(rcCaption);
	rcText.left = (pItem->GetIndent() + (pItem->IsCategory() ? 1 : 0)) * XTP_PGI_EXPAND_BORDER + 3;

	rcText.right = rcCaption.right - 1;
	rcText.bottom -= 1;

	CXTPPropertyGridItemMetrics* pMetrics = pItem->GetMetrics(FALSE, FALSE);
	if (pMetrics && pMetrics->m_nImage != -1)
	{
		CXTPImageManagerIcon* pImage = m_pGrid->GetImageManager()->GetImage(pMetrics->m_nImage, 0);

		if (pImage)
		{
			int nHeight = min(pImage->GetHeight(), rcText.Height());
			int nWidth = pImage->GetWidth();

			CPoint pt(rcText.left - 2, rcText.CenterPoint().y - nHeight/2);
			pImage->Draw(pDC, pt, CSize(nWidth, nHeight));

			rcText.left += nWidth + 5;

		}

	}

	UINT nFormat = pMetrics ? pMetrics->m_uDrawTextFormat : DT_VCENTER | DT_SINGLELINE;

	pDC->DrawText(pItem->GetCaption(), rcText, DT_NOPREFIX | nFormat);
}

void CXTPPropertyGridPaintManager::DrawInplaceButtons(CDC* pDC, CXTPPropertyGridItem* pItem, CRect rcValue)
{
	if (!pItem->IsInplaceButtonsVisible())
		return;

	int nRight = rcValue.right;
	for (int i = pItem->GetInplaceButtons()->GetCount() - 1; i >= 0; i--)
	{
		CXTPPropertyGridInplaceButton* pButton = pItem->GetInplaceButtons()->GetAt(i);

		int nWidth = pButton->GetWidth();
		CRect rcButton(nRight - nWidth, rcValue.top, nRight, rcValue.bottom);
		pButton->OnDraw(pDC, rcButton);

		nRight -= nWidth;
	}
}

void CXTPPropertyGridPaintManager::DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct)
{
	CXTPPropertyGridView* pView = &m_pGrid->GetGridView();
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpDrawItemStruct->itemData;

	CXTPWindowRect rcWindow(pView);
	CRect rc = lpDrawItemStruct->rcItem;

	CXTPBufferDC dc(lpDrawItemStruct->hDC, rc);
	CXTPFontDC font(&dc, GetItemFont(pItem, TRUE));

	dc.FillSolidRect(rc, GetItemBackColor(pItem, FALSE));
	dc.SetBkMode(TRANSPARENT);


	CRect rcCaption(rc.left, rc.top, rc.left + pView->GetDividerPos(), rc.bottom);

	if (pItem->IsCategory())
	{
		DrawCategoryCaptionBackground(&dc, rc);

		font.SetFont(GetItemFont(pItem, FALSE));

		dc.SetTextColor(GetItemTextColor(pItem, FALSE));
		CXTPEmptyRect rcDrawText;
		dc.DrawText(pItem->GetCaption(), rcDrawText, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_CALCRECT);

		CRect rcText(CPoint(((pItem->GetIndent() + 1) * XTP_PGI_EXPAND_BORDER) - 1, rc.top + 1) , CSize(rcDrawText.Width() + 4, rc.Height() - 3));

		dc.DrawText(pItem->GetCaption(), rcText, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_RIGHT);

		if (lpDrawItemStruct->itemState & ODS_FOCUS)
		{
			dc.SetTextColor(0);
			dc.DrawFocusRect(rcText);
		}
	}
	else
	{

		CRect rcValue(rcCaption.right, rc.top, rc.right, rc.bottom - 1);
		DrawItemValue(&dc, pItem, rcValue);

		dc.SetTextColor(GetItemTextColor(pItem, FALSE));
		font.SetFont(GetItemFont(pItem, FALSE));


		BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL bFocused = bSelected && CWnd::GetFocus() && ((CWnd::GetFocus() == pView) || (CWnd::GetFocus()->GetParent() == pView) || (CWnd::GetFocus()->GetOwner() == pView));
		if (bFocused)
		{
			dc.SetTextColor(::GetXtremeColor(COLOR_HIGHLIGHTTEXT));
			dc.FillSolidRect(rcCaption, ::GetXtremeColor(COLOR_HIGHLIGHT));
		}
		else if (bSelected)
		{
			dc.FillSolidRect(rcCaption, m_pMetrics->m_clrLine);
		}

		CXTPPropertyGridItem* pCategory = pItem->GetParentItem();
		while (pCategory != NULL)
		{
			if (pCategory->IsCategory())
				dc.FillSolidRect(rc.left + pCategory->GetIndent() * XTP_PGI_EXPAND_BORDER, rc.top, XTP_PGI_EXPAND_BORDER, rc.Height(), m_pMetrics->m_clrLine);

			pCategory = pCategory->GetParentItem();
		}

		CXTPPenDC pen(dc, m_pMetrics->m_clrLine);
		dc.MoveTo(0, rc.bottom - 1); dc.LineTo(rc.right, rc.bottom - 1);
		dc.MoveTo(rcCaption.right, rc.top); dc.LineTo(rcCaption.right, rc.bottom - 1);

		DrawItemCaption(&dc, pItem, rcCaption);

		if (bSelected || m_pGrid->GetShowInplaceButtonsAlways()) DrawInplaceButtons(&dc, pItem, rcValue);
	}

	CXTPPropertyGridItem* pNext = pView->GetItem(lpDrawItemStruct->itemID + 1);
	if (pNext && pNext->IsCategory() && pNext->IsVisible())
	{
		dc.FillSolidRect(pNext->GetIndent() * XTP_PGI_EXPAND_BORDER, rc.bottom - 1, rc.Width(), 1, m_clrFace);
	}



	if (pItem->HasVisibleChilds())
	{
		CRgn rgn;
		if (!pItem->IsCategory())
		{
			rgn.CreateRectRgnIndirect(&rcCaption);
			dc.SelectClipRgn(&rgn);
		}

		DrawExpandButton(dc, pItem, rcCaption);

		dc.SelectClipRgn(NULL);
	}
}

void CXTPPropertyGridPaintManager::DrawExpandButton(CDC& dc, CXTPPropertyGridItem* pItem, CRect rcCaption)
{
	CRect rcSign(CPoint(XTP_PGI_EXPAND_BORDER / 2 - 5, rcCaption.CenterPoint().y - 4), CSize(9, 9));

	if (pItem->GetIndent() > 0)
		rcSign.OffsetRect((pItem->GetIndent() - (pItem->IsCategory() ? 0 : 1)) * XTP_PGI_EXPAND_BORDER, 0);


	if (m_buttonsStyle == xtpGridButtonsThemed && m_themeTree.IsAppThemed())
	{
		m_themeTree.DrawThemeBackground(dc, TVP_GLYPH, pItem->IsExpanded() ? GLPS_OPENED : GLPS_CLOSED, &rcSign, 0);

	}
	else
	{

		CPoint pt = rcSign.CenterPoint();

		if (!pItem->IsCategory()) dc.FillSolidRect(rcSign, m_pMetrics->m_clrBack);
		dc.Draw3dRect(rcSign, m_pMetrics->m_clrFore, m_pMetrics->m_clrFore);
		CXTPPenDC pen (dc, m_pMetrics->m_clrFore);

		dc.MoveTo(pt.x - 2, pt.y);
		dc.LineTo(pt.x + 3, pt.y);

		if (!pItem->IsExpanded())
		{
			dc.MoveTo(pt.x, pt.y - 2);
			dc.LineTo(pt.x, pt.y + 3);
		}
	}


}

void CXTPPropertyGridPaintManager::FillInplaceButton(CDC* pDC, CXTPPropertyGridInplaceButton* pButton)
{
	CRect rect(pButton->GetRect());
	CPoint ptDots(rect.CenterPoint().x , rect.CenterPoint().y + 3);

	if (m_buttonsStyle == xtpGridButtonsThemed && m_themeButton.IsAppThemed() && m_themeCombo.IsAppThemed())
	{
		pDC->FillSolidRect(rect, m_pMetrics->m_clrBack);

		if (pButton->GetID() != XTP_ID_PROPERTYGRID_COMBOBUTTON)
		{
			m_themeButton.DrawThemeBackground(*pDC, BP_PUSHBUTTON, pButton->IsPressed() && pButton->IsHot() ? PBS_PRESSED :
				pButton->IsHot() ? PBS_HOT: pButton->IsFocused() ? PBS_DEFAULTED : PBS_NORMAL, rect, 0);
		}
		else
		{
			m_themeCombo.DrawThemeBackground(*pDC, CP_DROPDOWNBUTTON,
				pButton->IsPressed() && pButton->IsHot() ? CBXS_PRESSED: pButton->IsHot() ? CBXS_HOT : CBXS_NORMAL, rect, 0);
		}
	}
	else
	{
		if (m_buttonsStyle == xtpGridButtonsOfficeXP)
		{
			pDC->FillSolidRect(rect, GetXtremeColor(pButton->IsPressed() && pButton->IsHot() ? XPCOLOR_HIGHLIGHT_PUSHED :
				pButton->IsHot() ? XPCOLOR_HIGHLIGHT : XPCOLOR_TOOLBAR_FACE));
			COLORREF clrBorder = GetXtremeColor(pButton->IsPressed() && pButton->IsHot() ? XPCOLOR_HIGHLIGHT_PUSHED_BORDER :
				pButton->IsHot() ? XPCOLOR_HIGHLIGHT_BORDER : COLOR_3DSHADOW);
			pDC->Draw3dRect(rect, clrBorder, clrBorder);
		}
		else
		{
			pDC->FillSolidRect(rect, GetXtremeColor(COLOR_3DFACE));

			if (pButton->IsPressed() && pButton->IsHot())
				pDC->Draw3dRect(rect, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DSHADOW));
			else
			{
				pDC->Draw3dRect(rect, GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_3DDKSHADOW));
				rect.DeflateRect(1, 1);
				pDC->Draw3dRect(rect, GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DSHADOW));
			}
		}

		if (pButton->GetID() == XTP_ID_PROPERTYGRID_COMBOBUTTON)
		{
			CPoint pt = rect.CenterPoint();
			CXTPDrawHelpers::Triangle(pDC, CPoint(pt.x -3, pt.y -1), CPoint(pt.x + 3, pt.y -1), CPoint(pt.x, pt.y +2), GetXtremeColor(COLOR_BTNTEXT));
		}
	}
	BOOL bCustom = FALSE;

	if (pButton->GetID() != XTP_ID_PROPERTYGRID_COMBOBUTTON)
	{
		CXTPImageManagerIcon* pIcon = pButton->GetImage();
		if (pIcon)
		{
			CSize sz(pIcon->GetWidth(), pIcon->GetHeight());
			CPoint pt((rect.left + rect.right - sz.cx) / 2, (rect.top + rect.bottom - sz.cy) / 2);
			pIcon->Draw(pDC, pt, sz);
			bCustom = TRUE;
		}
		else if (!pButton->GetCaption().IsEmpty())
		{
			CXTPFontDC font(pDC, GetItemFont(pButton->GetItem(), TRUE));
			pDC->SetTextColor(GetXtremeColor(COLOR_BTNTEXT));
			pDC->DrawText(pButton->GetCaption(), rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
			bCustom = TRUE;
		}
	}

	if (!bCustom && pButton->GetID() == XTP_ID_PROPERTYGRID_EXPANDBUTTON)
	{
		pDC->FillSolidRect(ptDots.x - 4, ptDots.y, 2, 2, GetXtremeColor(COLOR_BTNTEXT));
		pDC->FillSolidRect(ptDots.x, ptDots.y, 2, 2, GetXtremeColor(COLOR_BTNTEXT));
		pDC->FillSolidRect(ptDots.x + 4, ptDots.y, 2, 2, GetXtremeColor(COLOR_BTNTEXT));
	}



	if (pButton->IsFocused())
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_BTNTEXT));
		pDC->SetBkColor(GetXtremeColor(COLOR_3DFACE));
		rect.DeflateRect(2, 2);
		pDC->DrawFocusRect(rect);
	}
}

//////////////////////////////////////////////////////////////////////
//

CXTPPropertyGridNativeXPTheme::CXTPPropertyGridNativeXPTheme(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
	m_buttonsStyle = xtpGridButtonsThemed;
}

void CXTPPropertyGridNativeXPTheme::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();

	switch (systemTheme)
	{
		case xtpSystemThemeSilver:
		case xtpSystemThemeBlue:
			m_clrShadow = RGB(127, 157, 185);
			break;

		case xtpSystemThemeOlive:
			m_clrShadow = RGB(164, 185, 127);
			break;
	}
}

//////////////////////////////////////////////////////////////////////
//

CXTPPropertyGridOffice2003Theme::CXTPPropertyGridOffice2003Theme(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
	m_bLunaTheme = FALSE;
	m_buttonsStyle = xtpGridButtonsThemed;
}



void CXTPPropertyGridOffice2003Theme::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_bLunaTheme = FALSE;

	if (!XTPColorManager()->IsLunaColorsDisabled())
	{
		XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();

		switch (systemTheme)
		{
		case xtpSystemThemeBlue:
			m_clrFace = RGB(221, 236, 254);
			m_clrShadow = RGB(123, 164, 224);
			m_pMetrics->m_clrHelpBack.SetStandardValue(m_clrFace);
			m_pMetrics->m_clrLine.SetStandardValue(RGB(169, 199, 240));
			m_pMetrics->m_clrCategoryFore.SetStandardValue(0);

			m_bLunaTheme = TRUE;
			break;

		case xtpSystemThemeOlive:
			m_clrFace = RGB(243, 242, 231);
			m_clrShadow = RGB(188, 187, 177);
			m_pMetrics->m_clrHelpBack.SetStandardValue(m_clrFace);
			m_pMetrics->m_clrLine.SetStandardValue(RGB(197, 212, 159));
			m_pMetrics->m_clrCategoryFore.SetStandardValue(0);

			m_bLunaTheme = TRUE;
			break;

		case xtpSystemThemeSilver:
			m_clrFace = RGB(238, 238, 244);
			m_clrShadow = RGB(161, 160, 187);
			m_pMetrics->m_clrHelpBack.SetStandardValue(m_clrFace);
			m_pMetrics->m_clrLine.SetStandardValue(RGB(192, 192, 211));
			m_pMetrics->m_clrCategoryFore.SetStandardValue(0);

			m_bLunaTheme = TRUE;
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// CXTPPropertyGridCoolTheme

CXTPPropertyGridCoolTheme::CXTPPropertyGridCoolTheme(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
}

void CXTPPropertyGridCoolTheme::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_pMetrics->m_clrLine.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
}


void CXTPPropertyGridCoolTheme::DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct)
{
	CXTPPropertyGridView* pView = &m_pGrid->GetGridView();
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpDrawItemStruct->itemData;

	ASSERT(pItem != NULL);
	if (!pItem)
		return;

	CXTPWindowRect rcWindow(pView);
	CRect rc = lpDrawItemStruct->rcItem;

	CXTPBufferDC dc(lpDrawItemStruct->hDC, rc);
	CXTPFontDC font(&dc, GetItemFont(pItem, TRUE));

	dc.FillSolidRect(rc, GetItemBackColor(pItem, FALSE));
	dc.SetBkMode(TRANSPARENT);


	CRect rcCaption(rc.left, rc.top, rc.left + pView->GetDividerPos(), rc.bottom);

	COLORREF clrLine = m_pMetrics->m_clrLine;
	COLORREF clrShadow = GetXtremeColor(COLOR_3DSHADOW);

	if (pItem->IsCategory())
	{
		dc.FillSolidRect(rc, clrLine);

		font.SetFont(GetItemFont(pItem, FALSE));
		dc.SetTextColor(GetItemTextColor(pItem, FALSE));

		int nIndent = XTP_PGI_EXPAND_BORDER * (pItem->GetIndent() + 1);

		if (lpDrawItemStruct->itemState & ODS_FOCUS)
		{
			dc.SetTextColor(::GetXtremeColor(COLOR_HIGHLIGHTTEXT));
			dc.FillSolidRect(CRect(nIndent, rc.top , rc.right, rc.bottom), ::GetXtremeColor(COLOR_HIGHLIGHT));
		}

		CRect rcText(nIndent + 3, rc.top , rc.right, rc.bottom - 1);
		dc.DrawText(pItem->GetCaption(), rcText, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

		dc.FillSolidRect((pItem->GetIndent() + (pItem->IsExpanded() ? 1 : 0)) * XTP_PGI_EXPAND_BORDER - 1, rc.bottom - 1, rc.Width(), 1, clrShadow);

		CXTPPropertyGridItem* pCategory = pItem->GetParentItem();
		while (pCategory != NULL)
		{
			if (pCategory->IsCategory())
			{
				dc.FillSolidRect(rc.left + pCategory->GetIndent() * XTP_PGI_EXPAND_BORDER, rc.top, XTP_PGI_EXPAND_BORDER, rc.Height(), clrLine);
				dc.FillSolidRect(rc.left + (pCategory->GetIndent() + 1) * XTP_PGI_EXPAND_BORDER - 1, rc.top, 1, rc.Height() + 2, clrShadow);
			}
			pCategory = pCategory->GetParentItem();
		}
	}
	else
	{
		CRect rcValue(rcCaption.right, rc.top, rc.right, rc.bottom - 1);
		DrawItemValue(&dc, pItem, rcValue);

		dc.SetTextColor(GetItemTextColor(pItem, FALSE));
		font.SetFont(GetItemFont(pItem, FALSE));

		BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL bFocused = bSelected && CWnd::GetFocus() && ((CWnd::GetFocus() == pView) || (CWnd::GetFocus()->GetParent() == pView) || (CWnd::GetFocus()->GetOwner() == pView));
		if (bFocused)
		{
			dc.SetTextColor(::GetXtremeColor(COLOR_HIGHLIGHTTEXT));
			dc.FillSolidRect(rcCaption, ::GetXtremeColor(COLOR_HIGHLIGHT));
		}
		else if (bSelected)
		{
			dc.FillSolidRect(rcCaption, clrLine);
		}

		CXTPPenDC pen(dc, clrLine);
		dc.MoveTo(0, rc.bottom - 1);
		dc.LineTo(rc.right, rc.bottom - 1);
		dc.MoveTo(rcCaption.right, rc.top);
		dc.LineTo(rcCaption.right, rc.bottom - 1);

		CXTPPropertyGridItem* pCategory = pItem->GetParentItem();
		while (pCategory != NULL)
		{
			if (pCategory->IsCategory())
			{
				dc.FillSolidRect(rc.left + pCategory->GetIndent() * XTP_PGI_EXPAND_BORDER, rc.top, XTP_PGI_EXPAND_BORDER, rc.Height(), clrLine);
				dc.FillSolidRect(rc.left + (pCategory->GetIndent() + 1) * XTP_PGI_EXPAND_BORDER - 1, rc.top, 1, rc.Height() + 2, clrShadow);
			}
			pCategory = pCategory->GetParentItem();
		}

		DrawItemCaption(&dc, pItem, rcCaption);

		if (bSelected || m_pGrid->GetShowInplaceButtonsAlways()) DrawInplaceButtons(&dc, pItem, rcValue);
	}

	CXTPPropertyGridItem* pNext = (CXTPPropertyGridItem*)pView->GetItemDataPtr(lpDrawItemStruct->itemID + 1);
	if (((ULONG_PTR)pNext == (ULONG_PTR)(-1)))
		dc.FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, clrShadow);
	else if (pNext && pNext->IsCategory() && pNext->IsVisible())
	{
		dc.FillSolidRect(pNext->GetIndent() * XTP_PGI_EXPAND_BORDER, rc.bottom - 1, rc.Width(), 1, clrShadow);
	}

	CXTPPropertyGridItem* pPrev = pItem;
	if (!pPrev)
		return;

	CXTPPropertyGridItem* pCategory = pPrev->GetParentItem();
	while (pCategory != NULL)
	{
		if (pItem->HasChilds() && pItem->IsExpanded())
			break;

		if (pCategory->IsCategory() && pCategory->IsVisible() && pCategory->GetChilds()->GetAt(pCategory->GetChilds()->GetCount() - 1) == pPrev)
		{
			dc.FillSolidRect(pCategory->GetIndent() * XTP_PGI_EXPAND_BORDER, rc.bottom - 1, rc.Width(), 1, clrShadow);
		}
		else
			break;
		pPrev = pCategory;
		pCategory = pPrev->GetParentItem();
	}


	if (pItem->HasVisibleChilds())
	{
		CRgn rgn;
		if (!pItem->IsCategory())
		{
			rgn.CreateRectRgnIndirect(&rcCaption);
			dc.SelectClipRgn(&rgn);
		}

		DrawExpandButton(dc, pItem, rcCaption);

		dc.SelectClipRgn(NULL);
	}
}



//////////////////////////////////////////////////////////////////////
// CXTPPropertyGridSimpleTheme

CXTPPropertyGridSimpleTheme::CXTPPropertyGridSimpleTheme(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
}

void CXTPPropertyGridSimpleTheme::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_pMetrics->m_clrLine.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
}



void CXTPPropertyGridSimpleTheme::DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct)
{
	CXTPPropertyGridView* pView = &m_pGrid->GetGridView();
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpDrawItemStruct->itemData;

	ASSERT(pItem != NULL);
	if (!pItem)
		return;

	CXTPWindowRect rcWindow(pView);
	CRect rc = lpDrawItemStruct->rcItem;

	CXTPBufferDC dc(lpDrawItemStruct->hDC, rc);
	CXTPFontDC font(&dc, GetItemFont(pItem, FALSE));

	dc.FillSolidRect(rc, GetItemBackColor(pItem, FALSE));
	dc.SetBkMode(TRANSPARENT);


	CRect rcCaption(rc.left, rc.top, rc.left + pView->GetDividerPos(), rc.bottom);

	COLORREF clrLine = m_pMetrics->m_clrLine;

	{
		if (!pItem->IsCategory())
		{
			CXTPFontDC fontValue(&dc, GetItemFont(pItem, TRUE));
			CRect rcValue(rcCaption.right, rc.top, rc.right, rc.bottom - 1);
			DrawItemValue(&dc, pItem, rcValue);
		}

		dc.SetTextColor(GetItemTextColor(pItem, FALSE));

		BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL bFocused = bSelected && CWnd::GetFocus() && ((CWnd::GetFocus() == pView) || (CWnd::GetFocus()->GetParent() == pView) || (CWnd::GetFocus()->GetOwner() == pView));
		if (bFocused)
		{
			dc.SetTextColor(::GetXtremeColor(COLOR_HIGHLIGHTTEXT));
			dc.FillSolidRect(CRect(XTP_PGI_EXPAND_BORDER, rcCaption.top, rcCaption.right, rcCaption.bottom), ::GetXtremeColor(COLOR_HIGHLIGHT));
		}
		else if (bSelected)
		{
			dc.FillSolidRect(CRect(XTP_PGI_EXPAND_BORDER, rcCaption.top, rcCaption.right, rcCaption.bottom), clrLine);
		}
		dc.FillSolidRect(XTP_PGI_EXPAND_BORDER - 1, rc.top, 1, rc.Height(), clrLine);

		CXTPPenDC pen(dc, clrLine);
		dc.MoveTo(XTP_PGI_EXPAND_BORDER, rc.bottom - 1);
		dc.LineTo(rc.right, rc.bottom - 1);
		dc.MoveTo(rcCaption.right, rc.top);
		dc.LineTo(rcCaption.right, rc.bottom - 1);


		DrawItemCaption(&dc, pItem, rcCaption);

		CRect rcValue(rcCaption.right, rc.top, rc.right, rc.bottom - 1);
		if (!pItem->IsCategory() && (bSelected || m_pGrid->GetShowInplaceButtonsAlways()))
			DrawInplaceButtons(&dc, pItem, rcValue);

	}

	if (pView->GetCount() == (int)lpDrawItemStruct->itemID + 1)
		dc.FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, clrLine);

	if (pItem->HasVisibleChilds())
	{
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rcCaption);
		dc.SelectClipRgn(&rgn);

		DrawExpandButton(dc, pItem, rcCaption);

		dc.SelectClipRgn(NULL);
	}
}



//////////////////////////////////////////////////////////////////////
// CXTPPropertyGridDelphiTheme

CXTPPropertyGridDelphiTheme::CXTPPropertyGridDelphiTheme(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
}


void CXTPPropertyGridDelphiTheme::DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct)
{
	CXTPPropertyGridView* pView = &m_pGrid->GetGridView();
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpDrawItemStruct->itemData;

	ASSERT(pItem != NULL);
	if (!pItem)
		return;

	CXTPWindowRect rcWindow(pView);
	CRect rc = lpDrawItemStruct->rcItem;

	CXTPBufferDC dc(lpDrawItemStruct->hDC, rc);
	CXTPFontDC font(&dc, GetItemFont(pItem, FALSE));

	dc.FillSolidRect(rc, GetItemBackColor(pItem, FALSE));
	dc.SetBkMode(TRANSPARENT);


	CRect rcCaption(rc.left, rc.top, rc.left + pView->GetDividerPos(), rc.bottom);

	COLORREF clrFore = m_pMetrics->m_clrFore;
	COLORREF clrShadow = GetXtremeColor(COLOR_3DSHADOW);
	COLORREF clrLight = GetXtremeColor(COLOR_BTNHIGHLIGHT);

	{
		if (!pItem->IsCategory())
		{
			CXTPFontDC fontValue(&dc, GetItemFont(pItem, TRUE));

			CRect rcValue(rcCaption.right, rc.top, rc.right, rc.bottom - 1);
			DrawItemValue(&dc, pItem, rcValue);
		}

		dc.SetTextColor(GetItemTextColor(pItem, FALSE));

		dc.Draw3dRect(rcCaption.right - 1, rc.top, 1, rc.Height(), clrShadow, clrShadow);
		dc.Draw3dRect(rcCaption.right, rc.top, 1, rc.Height(), clrLight, clrLight);

		BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL bFocused = bSelected && CWnd::GetFocus() && ((CWnd::GetFocus() == pView) || (CWnd::GetFocus()->GetParent() == pView) || (CWnd::GetFocus()->GetOwner() == pView));
		if (bFocused || bSelected)
		{
			dc.Draw3dRect(rc, clrFore, clrLight);
		}
		else
		{
			for (int i = 0; i < rc.right; i += 2)
				dc.SetPixel(i, rc.bottom - 1, clrShadow);
		}


		DrawItemCaption(&dc, pItem, rcCaption);

		CRect rcValue(rcCaption.right, rc.top, rc.right, rc.bottom - 1);
		if (!pItem->IsCategory() && (bSelected || m_pGrid->GetShowInplaceButtonsAlways()))
			DrawInplaceButtons(&dc, pItem, rcValue);
	}

	if (pItem->HasVisibleChilds())
	{
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rcCaption);
		dc.SelectClipRgn(&rgn);

		DrawExpandButton(dc, pItem, rcCaption);

		dc.SelectClipRgn(NULL);
	}
}

void CXTPPropertyGridDelphiTheme::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_pMetrics->m_clrBack.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
	m_pMetrics->m_clrFore.SetStandardValue(RGB(0, 0, 128));

}

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridWhidbeyTheme

CXTPPropertyGridOfficeXP::CXTPPropertyGridOfficeXP(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
	m_buttonsStyle = xtpGridButtonsOfficeXP;
}

void CXTPPropertyGridOfficeXP::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_pMetrics->m_clrCategoryFore.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_pMetrics->m_clrLine.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_FACE));
}

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridWhidbeyTheme

CXTPPropertyGridWhidbeyTheme::CXTPPropertyGridWhidbeyTheme(CXTPPropertyGrid* pGrid)
	: CXTPPropertyGridPaintManager(pGrid)
{
	m_buttonsStyle = xtpGridButtonsThemed;
}

void CXTPPropertyGridWhidbeyTheme::DrawCategoryCaptionBackground(CDC* pDC, CRect rc)
{
	pDC->FillSolidRect(rc, m_pMetrics->m_clrLine);

	XTPDrawHelpers()->GradientFill(pDC,
		CRect(rc.left, rc.bottom - 2, rc.right, rc.bottom - 1), m_pMetrics->m_clrLine, GetXtremeColor(COLOR_3DHIGHLIGHT), TRUE);

}

void CXTPPropertyGridWhidbeyTheme::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_pMetrics->m_clrLine.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
	m_pMetrics->m_clrCategoryFore.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrShadow = GetXtremeColor(XPCOLOR_STATICFRAME);
}

void CXTPPropertyGridWhidbeyTheme::DrawExpandButton(CDC& dc, CXTPPropertyGridItem* pItem, CRect rcCaption)
{

	CRect rcButton(CPoint(XTP_PGI_EXPAND_BORDER / 2 - 5, rcCaption.CenterPoint().y - 4), CSize(9, 9));

	if (pItem->GetIndent() > 0)
		rcButton.OffsetRect((pItem->GetIndent() - (pItem->IsCategory() ? 0 : 1)) * XTP_PGI_EXPAND_BORDER, 0);

	CDC* pDC = &dc;

	COLORREF clrButton = GetXtremeColor(COLOR_3DFACE);
	COLORREF clrFrame = GetXtremeColor(COLOR_3DSHADOW);
	pDC->FillSolidRect(rcButton.left + 1, rcButton.top + 1, 7, 7, clrButton);
	pDC->FillSolidRect(rcButton.left + 1, rcButton.top, 9 - 2, 1, clrFrame);
	pDC->FillSolidRect(rcButton.left + 1, rcButton.bottom - 1, 9 - 2, 1, clrFrame);
	pDC->FillSolidRect(rcButton.left, rcButton.top + 1, 1, 9 - 2, clrFrame);
	pDC->FillSolidRect(rcButton.right - 1, rcButton.top + 1, 1, 9 - 2, clrFrame);
	pDC->FillSolidRect(rcButton.left + 1, rcButton.top + 1, 9 - 2, 3, 0xFFFFFF);
	pDC->FillSolidRect(rcButton.left + 1, rcButton.top + 4, 9 - 4, 2, 0xFFFFFF);

	pDC->FillSolidRect(rcButton.left + 2, rcButton.top + 4, 9 - 4, 1, 0);
	if (!pItem->IsExpanded())
		pDC->FillSolidRect(rcButton.left + 4, rcButton.top + 2, 1, 9 - 4, 0);
}


