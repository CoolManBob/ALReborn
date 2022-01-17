// XTPReportColumn.cpp : implementation of the CXTPReportColumn class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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
#include "Common/XTPPropExchange.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPReportColumn.h"
#include "XTPReportColumns.h"
#include "XTPReportSubListControl.h"
#include "XTPReportRecordItem.h"
#include "XTPReportControl.h"
#include "XTPReportInplaceControls.h"
#include "XTPReportHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////
// CXTPReportColumn

CXTPReportColumn::CXTPReportColumn(int nItemIndex, LPCTSTR strName, int nWidth,
									BOOL bAutoSize, int nIconID, BOOL bSortable, BOOL bVisible) :
	m_nItemIndex(nItemIndex), m_strName(strName), m_nIconID(nIconID),
	m_nMinWidth(10), m_bSortable(bSortable), m_bVisible(bVisible)
{
	m_bGroupable = TRUE;
	m_bAutoSortWhenGrouped = TRUE;

	m_bFiltrable = TRUE;
	m_bIsResizable = bAutoSize;
	m_rcColumn.SetRectEmpty();

	m_pColumns = NULL;
	m_bSortIncreasing = TRUE;

	m_bAllowRemove = TRUE;
	m_bAllowDrag = TRUE;

	m_nMaxItemWidth = 0;
	m_nAlignment = DT_LEFT;
	m_nHeaderAlignment = -1;
	m_nFooterAlignment = -1;

	m_bDrawFooterDivider = TRUE;

	m_bAutoSize = bAutoSize;

	m_nColumnStaticWidth = m_nColumnAutoWidth = nWidth;

	m_bShowInFieldChooser = TRUE;
	m_bEditable = TRUE;

	m_pEditOptions = new CXTPReportRecordItemEditOptions();
}

CXTPReportColumn::~CXTPReportColumn()
{
	if (m_pEditOptions)
	{
		m_pEditOptions->InternalRelease();
	}
}

void CXTPReportColumn::SetCaption(LPCTSTR strCaption)
{
	m_strName = strCaption;
	m_pColumns->GetReportHeader()->OnColumnsChanged();
}

void CXTPReportColumn::SetFooterText(LPCTSTR strFooter)
{
	m_strFooterText = strFooter;
	m_pColumns->GetReportHeader()->GetControl()->AdjustLayout();
	m_pColumns->GetReportHeader()->GetControl()->AdjustScrollBars();
}

void CXTPReportColumn::SetFooterFont(CFont* pFont)
{
	m_fontFooter.DeleteObject(); // set default

	CFont* pFontDefault = &m_pColumns->GetReportHeader()->GetControl()->GetPaintManager()->m_fontCaption;

	if (pFont && pFont->m_hObject && pFont->m_hObject != pFontDefault->m_hObject)
	{
		LOGFONT lfFooter;
		pFont->GetLogFont(&lfFooter);

		VERIFY(m_fontFooter.CreateFontIndirect(&lfFooter));
	}

	m_pColumns->GetReportHeader()->GetControl()->AdjustLayout();
	m_pColumns->GetReportHeader()->GetControl()->AdjustScrollBars();
}

void CXTPReportColumn::SetDrawFooterDivider(BOOL bSet)
{
	m_bDrawFooterDivider = bSet;

	m_pColumns->GetReportHeader()->GetControl()->RedrawControl();
}

CFont* CXTPReportColumn::GetFooterFont()
{
	if (m_fontFooter.m_hObject != NULL)
		return &m_fontFooter;

	return &m_pColumns->GetReportHeader()->GetControl()->GetPaintManager()->m_fontCaption;
}

int CXTPReportColumn::GetNormAlignment(int nAlignment) const
{
	if (!GetColumns()->GetReportHeader()->GetPaintManager()->m_bRevertAlignment)
		return nAlignment;

	return nAlignment & DT_RIGHT ? (nAlignment - DT_RIGHT) : (nAlignment + DT_RIGHT);

}

int CXTPReportColumn::GetAlignment() const
{
	return GetNormAlignment(m_nAlignment);
}

int CXTPReportColumn::GetHeaderAlignment() const
{
	if (m_nHeaderAlignment != -1)
		return m_nHeaderAlignment;

	if (GetColumns()->GetReportHeader()->GetPaintManager()->m_bUseColumnTextAlignment)
		return GetAlignment();

	return GetNormAlignment(DT_LEFT);
}

int CXTPReportColumn::GetFooterAlignment() const
{
	if (m_nFooterAlignment != -1)
		return m_nFooterAlignment;

	if (GetColumns()->GetReportHeader()->GetPaintManager()->m_bUseColumnTextAlignment)
		return GetAlignment();

	return GetNormAlignment(DT_LEFT);
}

void CXTPReportColumn::SetFooterAlignment(int nAlignment)
{
	m_nFooterAlignment = nAlignment;

	m_pColumns->GetReportHeader()->GetControl()->AdjustLayout();
	m_pColumns->GetReportHeader()->GetControl()->AdjustScrollBars();
}

int CXTPReportColumn::GetWidth() const
{
	return m_nColumnStaticWidth + (!m_bIsResizable ? GetIndent() : 0);
}

CRect CXTPReportColumn::GetRect() const
{
	return m_rcColumn;
}


int CXTPReportColumn::SetWidth(int nNewWidth)
{
	int nOldWidth = m_nColumnStaticWidth;
	m_nColumnStaticWidth = m_nColumnAutoWidth = nNewWidth;

	m_pColumns->GetReportHeader()->OnColumnsChanged();
	return nOldWidth;
}

BOOL CXTPReportColumn::IsSortedIncreasing() const
{
	return m_bSortIncreasing;
}
BOOL CXTPReportColumn::IsSortedDecreasing() const
{
	return !m_bSortIncreasing;
}

BOOL CXTPReportColumn::IsSorted() const
{
	return (m_pColumns->GetSortOrder()->IndexOf(this) != -1);
}

void CXTPReportColumn::SetTreeColumn(BOOL bIsTreeColumn)
{
	if (bIsTreeColumn)
	{
		m_pColumns->m_pTreeColumn = this;
	}
	else if (IsTreeColumn())
	{
		m_pColumns->m_pTreeColumn = NULL;
	}
}

BOOL CXTPReportColumn::IsTreeColumn() const
{
	return m_pColumns->m_pTreeColumn == this;
}

BOOL CXTPReportColumn::IsVisible() const
{
	return m_bVisible;
}


void CXTPReportColumn::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_Bool(pPX, _T("SortIncreasing"), m_bSortIncreasing, TRUE);
	PX_Bool(pPX, _T("Visible"), m_bVisible, TRUE);
	PX_Int(pPX, _T("Alignment"), m_nAlignment, DT_LEFT);
	PX_Int(pPX, _T("StaticWidth"), m_nColumnStaticWidth, 0);
	PX_Int(pPX, _T("AutoWidth"), m_nColumnAutoWidth, 0);

	if (pPX->GetSchema() > _XTP_SCHEMA_1041)
	{
		PX_Int(pPX, _T("HeaderAlignment"), m_nHeaderAlignment, -1);
		PX_Int(pPX, _T("FooterAlignment"), m_nFooterAlignment, -1);
	}
}

BOOL CXTPReportColumn::HasSortTriangle() const
{
	if (m_pColumns->GetSortOrder()->IndexOf(this) != -1)
		return TRUE;

	if (m_pColumns->GetGroupsOrder()->IndexOf(this) != -1 && m_bAutoSortWhenGrouped)
		return TRUE;

	return FALSE;
}

int CXTPReportColumn::GetCaptionWidth(CDC* pDC) const
{
	return pDC->GetTextExtent(m_strName).cx;
}

BOOL CXTPReportColumn::IsDragging() const
{
	return m_pColumns && m_pColumns->GetReportHeader() && m_pColumns->GetReportHeader()->m_pDragColumn == this;
}

int CXTPReportColumn::GetIndent() const
{
	if (!m_pColumns->GetReportHeader())
		return 0;

	if (m_pColumns->GetVisibleAt(0) == this)
		return GetControl()->GetHeaderIndent();

	return 0;
}

int CXTPReportColumn::GetMinWidth() const
{
	return (m_bIsResizable || m_bAutoSize ? m_nMinWidth : m_nColumnStaticWidth) + GetIndent();
}

void CXTPReportColumn::SetVisible(BOOL bVisible)
{
	if (bVisible != m_bVisible)
	{
		m_bVisible = bVisible;
		m_pColumns->GetReportHeader()->OnColumnsChanged();
	}
}

int CXTPReportColumn::GetItemIndex() const
{
	return m_nItemIndex;
}

int CXTPReportColumn::GetIndex() const
{
	return m_pColumns->IndexOf(this);
}

CXTPReportControl* CXTPReportColumn::GetControl() const
{
	return m_pColumns->GetReportHeader()->GetControl();
}

void CXTPReportColumn::SetShowInFieldChooser(BOOL bShow)
{
	if (bShow != m_bShowInFieldChooser)
	{
		m_bShowInFieldChooser = bShow;

		CXTPReportSubListControl* pSubList = m_pColumns->GetReportHeader()->GetSubListCtrl();
		if (pSubList)
		{
			pSubList->UpdateList();
		}
	}
}

BOOL CXTPReportColumn::IsHotTracking() const
{
	return m_pColumns->GetReportHeader()->GetHotTrackingColumn() == this;
}

BOOL CXTPReportColumn::IsShowInFieldChooser() const
{
	return m_bShowInFieldChooser;
}


int CXTPReportColumn::GetPrintWidth(int nTotalWidth) const
{
	CXTPReportColumns* pColumns = m_pColumns;

	int nColumnsWidth = 0;
	CXTPReportColumn* pLastAutoColumn = NULL;

	for (int nColumn = 0; nColumn < pColumns->GetCount(); nColumn++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(nColumn);
		if (!pColumn->IsVisible())
			continue;

		if (pColumn->IsAutoSize())
		{
			pLastAutoColumn = pColumn;
			nColumnsWidth += pColumn->GetWidth();
		}
		else
		{
			nTotalWidth -= pColumn->GetWidth();
		}
	}

	for (int i = 0; i < pColumns->GetCount(); i++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(i);
		if (!pColumn->IsVisible())
			continue;

		int nWidth = pColumn->GetWidth();

		if (pColumn->IsAutoSize())
		{
			if (pColumn == pLastAutoColumn)
			{
				nWidth = max(nTotalWidth, pColumn->GetMinWidth());
			}
			else
			{
				nColumnsWidth = max(1, nColumnsWidth);

				nWidth =
					max(int(pColumn->GetWidth() * nTotalWidth / nColumnsWidth), pColumn->GetMinWidth());

				nTotalWidth -= nWidth;
				nColumnsWidth -= pColumn->GetWidth();
			}
		}

		if (pColumn == this)
			return nWidth;
	}

	return 0;
}


int CXTPReportColumn::GetBestFitWidth() const
{
	CXTPReportControl* pControl = GetControl();
	CXTPReportPaintManager* pPaintManager = pControl->GetPaintManager();
	int nBestColumnWidth = 0;

	CClientDC dc(pControl);
	CXTPFontDC font(&dc, &pPaintManager->m_fontCaption);
	nBestColumnWidth = 6 + GetCaptionWidth(&dc);

	if (GetIconID() != XTP_REPORT_NOICON)
	{
		nBestColumnWidth += pPaintManager->DrawBitmap(NULL, pControl, GetRect(), nBestColumnWidth) + 2;
	}
	if (HasSortTriangle())
	{
		nBestColumnWidth += 27;
	}

	return max(nBestColumnWidth, m_nMaxItemWidth);
}

