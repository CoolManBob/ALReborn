// XTPReportRow.cpp : implementation of the CXTPReportRow class.
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
#include "Resource.h"

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPToolTipContext.h"

#include "XTPReportRecord.h"
#include "XTPReportControl.h"
#include "XTPReportPaintManager.h"
#include "XTPReportRow.h"
#include "XTPReportColumns.h"
#include "XTPReportColumn.h"
#include "XTPReportRecordItem.h"
#include "XTPReportRecordItemText.h"
#include "XTPReportTip.h"
#include "XTPReportInplaceControls.h"
#include "XTPReportHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPReportRow, CCmdTarget)
/////////////////////////////////////////////////////////////////////////////
// CXTPReportRow


CXTPReportRow::CXTPReportRow()
	: m_pParentRow(NULL), m_pRecord(NULL), m_pControl(NULL), m_pParentRows(NULL)
{
	m_nGroupLevel = m_nRowLevel = 0;


	m_bVisible = FALSE;
	m_pChilds = NULL;
	m_bExpanded = TRUE;

	m_rcRow.SetRectEmpty();
	m_rcCollapse.SetRectEmpty();
	m_nChildIndex = m_nIndex = -1;

	m_nPreviewHeight = 0;

}

void CXTPReportRow::InitRow(CXTPReportControl* pControl, CXTPReportRecord* pRecord)
{
	ASSERT(pRecord || IsGroupRow());

	m_pControl = pControl;

	if (pRecord)
	{
		m_pRecord = pRecord;
		m_bExpanded = pRecord->m_bExpanded;
		m_pRecord->InternalAddRef();
	}
}

void CXTPReportRow::InitRow(CXTPReportRow* pRow)
{
	ASSERT(pRow->m_pRecord);
	ASSERT(pRow->m_pParentRow == NULL);

	m_pControl = pRow->m_pControl;
	m_pRecord = pRow->m_pRecord;
	m_pRecord->InternalAddRef();
	m_rcRow = pRow->m_rcRow;
	m_nIndex = pRow->m_nIndex;
	m_nPreviewHeight = pRow->m_nPreviewHeight;
	m_nRowLevel = pRow->m_nRowLevel;
	m_nGroupLevel = pRow->m_nGroupLevel;
}


CXTPReportRows* CXTPReportRow::GetChilds()
{
	if (!m_pChilds)
	{
		m_pChilds = new CXTPReportRows();
	}
	return m_pChilds;
}



CXTPReportRow::~CXTPReportRow()
{
	if (m_pChilds)
	{
		m_pChilds->InternalRelease();
	}

	if (m_pRecord)
	{
		m_pRecord->InternalRelease();
	}
}

int CXTPReportRow::GetHeight(CDC* pDC, int nWidth)
{
	ASSERT(m_pControl);
	int nHeight = m_pControl->GetPaintManager()->GetRowHeight(pDC, this, nWidth);

	if (!IsGroupRow() && !IsItemsVisible())
		nHeight = 0;

	if (pDC->IsPrinting())
		return nHeight;

	m_nPreviewHeight = 0;

	if (IsPreviewVisible())
	{
		CXTPReportRecordItemPreview* pItem = GetRecord()->GetItemPreview();

		m_nPreviewHeight = pItem->GetPreviewHeight(pDC, this, m_pControl->GetReportHeader()->GetWidth());

		return nHeight + m_nPreviewHeight;
	}


	return nHeight;
}


BOOL CXTPReportRow::IsFocused() const
{
	return m_pControl->m_nFocusedRow == m_nIndex;
}

BOOL CXTPReportRow::IsSelected() const
{
	return m_pControl->GetSelectedRows()->Contains(this);
}

void CXTPReportRow::SetSelected(BOOL bSelected)
{
	if (bSelected)
	{
		if (!m_pControl->IsMultipleSelection())
			m_pControl->GetSelectedRows()->Clear();

		m_pControl->GetSelectedRows()->Add(this);
	}
	else
	{
		m_pControl->GetSelectedRows()->Remove(this);
	}

	m_pControl->RedrawControl();
}

int CXTPReportRow::GetLastChildRow(CXTPReportRows* pChilds) const
{
	CXTPReportRow* pRow = pChilds->GetAt(pChilds->GetCount() - 1);
	return pRow->HasChildren() && pRow->IsExpanded() ? GetLastChildRow(pRow->GetChilds()) : pRow->GetIndex();
}

void CXTPReportRow::SelectChilds()
{
	if (!(m_pControl->IsMultipleSelection() && HasChildren() && IsExpanded() && m_nIndex != -1))
		return;

	m_pControl->BeginUpdate();

	int nIndexBegin = m_nIndex + 1;
	int nIndexEnd = GetLastChildRow(GetChilds());

	m_pControl->GetSelectedRows()->AddBlock(nIndexBegin, nIndexEnd);

	m_pControl->EndUpdate();
}

BOOL CXTPReportRow::IsItemsVisible() const
{
	return TRUE;
}


BOOL CXTPReportRow::IsPreviewVisible() const
{
	return !IsGroupRow() && m_pRecord && m_pControl->IsPreviewMode() && m_pRecord->GetItemPreview() != NULL;
}

void CXTPReportRow::Draw(CDC* pDC, CRect rcRow, int nLeftOffset)
{
	ASSERT(m_pControl);

	BOOL bControlFocused = m_pControl->HasFocus();
	int nFreezeCols = m_pControl->m_nFreezeColumnsCount;
	CRect rcClipBox = m_pControl->GetReportRectangle();

	m_rcRow = rcRow;
	m_rcRow.left -= nLeftOffset;
	m_rcRow.right -= nLeftOffset;

	if (nFreezeCols == 0)
	{
		rcRow = m_rcRow;
	}

	XTP_REPORTRECORDITEM_DRAWARGS drawArgs;
	drawArgs.pDC = pDC;
	drawArgs.pControl = m_pControl;
	drawArgs.pRow = this;
	int nIndentWidth = m_pControl->GetHeaderIndent();
	CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();

	CXTPReportColumns arrVisibleColumns(m_pControl);
	m_pControl->GetColumns()->GetVisibleColumns(arrVisibleColumns);
	int nVisColCount = arrVisibleColumns.GetCount();
	nFreezeCols = min(nFreezeCols, nVisColCount);

	// paint row background
	pPaintManager->FillRow(pDC, this, m_rcRow);

	CRect rcItem(m_rcRow.left, m_rcRow.top, m_rcRow.right, m_rcRow.bottom - m_nPreviewHeight);

	CRect rcIndent(nFreezeCols ? rcRow : m_rcRow);
	rcIndent.right = rcIndent.left + nIndentWidth;

	if (m_pRecord) // if drawing record, not group
	{
		int xMinCol_0 = rcRow.left + nIndentWidth;
		// paint record items
		for (int nColumn = nVisColCount-1; nColumn >= 0; nColumn--)
		{
			BOOL bFreezeCol = nColumn < nFreezeCols;
			int nColIdx = bFreezeCol ? nFreezeCols - 1 - nColumn : nColumn;

			CXTPReportColumn* pColumn = arrVisibleColumns.GetAt(nColIdx);
			ASSERT(pColumn && pColumn->IsVisible());

			if (pColumn && IsItemsVisible())
			{
				rcItem.left = pColumn->GetRect().left;
				if (nColIdx == 0)
				{
					rcItem.left = max(xMinCol_0, rcItem.left);
				}
				rcItem.right = pColumn->GetRect().right;

				if (!CRect().IntersectRect(rcClipBox, rcItem))
					continue;

				if (bFreezeCol)
				{
					pDC->FillSolidRect(rcItem, pPaintManager->GetControlBackColor(GetControl()));
					pPaintManager->FillRow(pDC, this, rcItem);
				}

				CRect rcGridItem(rcItem);
				rcGridItem.left--;

				// draw shade background if sorted by this row
				if (pColumn->IsSorted())
				{
					pPaintManager->FillItemShade(pDC, rcItem);
				}

				CXTPReportRecordItem* pItem = m_pRecord->GetItem(pColumn);

				if (pItem)
				{
					// draw item
					drawArgs.pColumn = pColumn;
					drawArgs.rcItem = rcItem;
					drawArgs.nTextAlign = pColumn->GetAlignment();
					drawArgs.pItem = pItem;
					// draw item
					int nItemTextWidth = pItem->Draw(&drawArgs);

					pColumn->m_nMaxItemWidth = max(pColumn->m_nMaxItemWidth, nItemTextWidth);
				}

				pPaintManager->DrawGrid(pDC, TRUE, rcGridItem);

				if (nColIdx == nFreezeCols - 1)
					pPaintManager->DrawFreezeColsDivider(pDC, rcGridItem, GetControl(), this);
			}
		}

		if (IsPreviewVisible())
		{
			CRect rcPreviewItem(m_rcRow);
			rcPreviewItem.DeflateRect(nIndentWidth, rcPreviewItem.Height() - m_nPreviewHeight, 0, 0);

			drawArgs.rcItem = rcPreviewItem;
			drawArgs.nTextAlign = DT_LEFT;
			drawArgs.pItem = m_pRecord->GetItemPreview();
			drawArgs.pColumn = NULL;

			drawArgs.pItem->Draw(&drawArgs);
		}

		if (nIndentWidth > 0)
		{
			// draw indent column
			pPaintManager->FillIndent(pDC, rcIndent);
		}
	}

	BOOL bGridVisible = pPaintManager->IsGridVisible(FALSE);

	//CRect rcFocus(m_rcRow.left + nIndentWidth, m_rcRow.top, m_rcRow.right, m_rcRow.bottom - (bGridVisible ? 1 : 0));
	CRect rcFocus(rcIndent.right, m_rcRow.top, m_rcRow.right, m_rcRow.bottom - (bGridVisible ? 1 : 0));

	if (IsFocused() && bControlFocused)
		pPaintManager->DrawFocusedRow(pDC, rcFocus);


	if (m_nIndex < m_pControl->GetRows()->GetCount() - 1 && nIndentWidth > 0)
	{
		CXTPReportRow* pNextRow = m_pControl->GetRows()->GetAt(m_nIndex + 1);
		ASSERT(pNextRow);
		rcFocus.left = rcIndent.left + min(nIndentWidth, pPaintManager->m_nTreeIndent * pNextRow->GetTreeDepth());
	}
	else
	{
		rcFocus.left = m_rcRow.left;
	}

	pPaintManager->DrawGrid(pDC, FALSE, rcFocus);

}

int CXTPReportRow::GetTreeDepth() const
{
	return m_nRowLevel;
}

INT_PTR CXTPReportRow::OnToolHitTest(CPoint point, TOOLINFO* pTI)
{
	CRect rcItem;
	CXTPReportRecordItem* pItem = HitTest(point, &rcItem);

	if (!pItem)
		return -1;

	INT_PTR nHit = (INT_PTR)pItem;
	CString strTip = pItem->GetTooltip();

	m_pControl->OnGetToolTipInfo(this, pItem, strTip);

	if (strTip.IsEmpty() || strTip == _T(" "))
		return -1;

	CXTPToolTipContext::FillInToolInfo(pTI, m_pControl->m_hWnd, rcItem,
		nHit, strTip);

	return nHit;
}

BOOL CXTPReportRow::OnLButtonDown(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs)
{
	return (BOOL)m_pControl->SendMessageToParent(this, pClickArgs->pItem, pClickArgs->pColumn, XTP_NM_REPORT_LBUTTONDOWN, &pClickArgs->ptClient);
}

void CXTPReportRow::OnClick(CPoint ptClicked)
{
	XTP_REPORTRECORDITEM_CLICKARGS clickArgs;
	clickArgs.pControl = m_pControl;
	clickArgs.pRow = this;
	clickArgs.ptClient = ptClicked;
	clickArgs.pColumn = NULL;

	// find clicked item
	clickArgs.pItem = HitTest(ptClicked, &clickArgs.rcItem, &clickArgs.pColumn);

	if (HasChildren() && m_rcCollapse.PtInRect(ptClicked)
		&& clickArgs.pColumn && clickArgs.pColumn->IsTreeColumn())
	{
		SetExpanded(!IsExpanded());
		return;
	}

	// notify item if found
	if (!clickArgs.pItem)
		return;

	clickArgs.pItem->OnClick(&clickArgs);
}

void CXTPReportRow::OnDblClick(CPoint ptClicked)
{
	XTP_REPORTRECORDITEM_CLICKARGS clickArgs;
	clickArgs.pControl = m_pControl;
	clickArgs.pRow = this;
	clickArgs.ptClient = ptClicked;
	clickArgs.pColumn = NULL;

	// find clicked item
	clickArgs.pItem = HitTest(ptClicked, &clickArgs.rcItem, &clickArgs.pColumn);

	// notify item if found
	if (clickArgs.pItem != NULL)
	{
		clickArgs.pItem->OnDblClick(&clickArgs);
	}
	else
	{
		// just notify parent
		m_pControl->SendMessageToParent(this, NULL, clickArgs.pColumn, NM_DBLCLK, &ptClicked, -1);
	}
}

CRect CXTPReportRow::GetItemRect(CXTPReportRecordItem* pItem)
{
	if (!IsItemsVisible() || !pItem || !m_pRecord)
		return CRect(0, 0, 0, 0);

	if (IsPreviewVisible() && pItem == GetRecord()->GetItemPreview())
	{
		return CRect(m_rcRow.left + m_pControl->GetHeaderIndent(),
			m_rcRow.bottom - m_nPreviewHeight, m_rcRow.right, m_rcRow.bottom);
	}

	int x = m_rcRow.left + m_pControl->GetHeaderIndent();

	CXTPReportColumns* pColumns = m_pControl->GetColumns();
	int nColumnCount = pColumns->GetCount();

	CRect rcItem(0, m_rcRow.top, 0, m_rcRow.bottom - m_nPreviewHeight);

	for (int nColumn = 0; nColumn < nColumnCount; nColumn++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(nColumn);
		if (pColumn && pColumn->IsVisible())
		{
			rcItem.left = x;
			x = rcItem.right = pColumn->GetRect().right;

			if (m_pRecord->GetItem(pColumn) == pItem)
			{
				ShiftTreeIndent(rcItem, pColumn);
				return rcItem;
			}
		}
	}
	return CRect(0, 0, 0, 0);
}

void CXTPReportRow::ShiftTreeIndent(CRect& rcItem, CXTPReportColumn* pColumn) const
{
	if (pColumn->IsTreeColumn())
	{
		int nTreeDepth = GetTreeDepth() - m_nGroupLevel;
		if (nTreeDepth > 0)
			nTreeDepth++;
		rcItem.left += m_pControl->GetIndent(nTreeDepth);

		CRect rcBitmap(rcItem);
		int nIndent = m_pControl->GetPaintManager()->DrawCollapsedBitmap(NULL, this, rcBitmap);

		rcItem.left += nIndent + 1;
	}
}

CXTPReportRecordItem* CXTPReportRow::HitTest(CPoint ptPoint, CRect* pRectItem, CXTPReportColumn** ppColumn) const
{
	if (!m_pRecord)
		return NULL;

	// find record item
	int x = m_rcRow.left + m_pControl->GetHeaderIndent();

	CXTPReportColumns* pColumns = m_pControl->GetColumns();
	int nColumnCount = pColumns->GetCount();


	// if hittes for Preview item
	if (IsPreviewVisible())
	{
		CXTPReportRecordItemPreview* pPreviewItem = GetRecord()->GetItemPreview();
		if (pPreviewItem)
		{
			CRect rcItem(x, m_rcRow.bottom - m_nPreviewHeight, m_rcRow.right, m_rcRow.bottom);
			if (rcItem.PtInRect(ptPoint))
			{
				if (pRectItem)
				{
					*pRectItem = rcItem;
				}
				return pPreviewItem;
			}
		}
	}

	CRect rcItem(0, m_rcRow.top, 0, m_rcRow.bottom - m_nPreviewHeight);

	if (!IsItemsVisible())
		return NULL;

	for (int nColumn = 0; nColumn < nColumnCount; nColumn++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(nColumn);
		if (pColumn && pColumn->IsVisible())
		{
			rcItem.left = x;
			x = rcItem.right = pColumn->GetRect().right;

			if (rcItem.PtInRect(ptPoint) && ppColumn)
			{
				*ppColumn = pColumn;
			}
			// make tooltip shift if tree view (see also Draw function)
			ShiftTreeIndent(rcItem, pColumn);

			// check point
			if (rcItem.PtInRect(ptPoint))
			{
				if (pRectItem)
				{
					*pRectItem = rcItem;
				}

				return m_pRecord->GetItem(pColumn);
			}
		}
	}

	return NULL;
}

void CXTPReportRow::OnMouseMove(UINT nFlags, CPoint point)
{

	// find clicked item
	CRect rcItem;
	CXTPReportRecordItem* pItem = HitTest(point, &rcItem);

	// notify item if found
	if (pItem != NULL)
	{
		pItem->OnMouseMove(nFlags, point);
	}

}


void CXTPReportRow::FillMetrics(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem, XTP_REPORTRECORDITEM_METRICS* pMetrics)
{
	m_pControl->m_nLockUpdateCount++;
	XTP_REPORTRECORDITEM_DRAWARGS drawArgs;
	drawArgs.pDC = NULL;
	drawArgs.pColumn = pColumn;
	drawArgs.pControl = m_pControl;
	drawArgs.pRow = this;
	drawArgs.rcItem.SetRectEmpty();
	drawArgs.pItem = pItem;
	GetItemMetrics(&drawArgs, pMetrics);
	m_pControl->m_nLockUpdateCount--;
}

void CXTPReportRow::ShowToolTip(CPoint ptTip, CXTPReportTip* pTipWnd)
{
	CRect rcItem(0, 0, 0, 0);
	CXTPReportColumn* pColumn = NULL;
	CXTPReportRecordItem* pItem = HitTest(ptTip, &rcItem, &pColumn);
	// show tooltip
	if (!pItem || pItem->IsPreviewItem() || !pColumn)
	{
		pTipWnd->m_pItem = NULL;
		pTipWnd->m_nRowIndex = -1;
		return;
	}

	if (!CXTPDrawHelpers::IsTopParentActive(m_pControl->GetSafeHwnd()) || m_pControl->GetActiveItem())
	{
		return;
	}

	if ((pItem != pTipWnd->m_pItem) || (m_nIndex != pTipWnd->m_nRowIndex))
	{
		pTipWnd->m_pItem = pItem;
		pTipWnd->m_nRowIndex = m_nIndex;

		CString strTip = pItem->GetTooltip();
		m_pControl->OnGetToolTipInfo(this, pItem, strTip);

		if (!strTip.IsEmpty() || strTip == _T(" ") || pColumn->GetAlignment() & DT_WORDBREAK)
			return;

		XTP_REPORTRECORDITEM_METRICS* pMetrics = new XTP_REPORTRECORDITEM_METRICS();
		pMetrics->strText = pItem->GetCaption(pColumn);
		FillMetrics(pColumn, pItem, pMetrics);

		CString strText(pMetrics->strText);

		strText.TrimRight();

		if (strText.IsEmpty())
		{
			pMetrics->InternalRelease();
			return;
		}

		XTP_REPORTRECORDITEM_ARGS itemArgs;
		itemArgs.pControl = m_pControl;
		itemArgs.pRow = this;

		pItem->GetCaptionRect(&itemArgs, rcItem);

		m_pControl->ClientToScreen(&rcItem);

		if (!pTipWnd->GetSafeHwnd())
		{
			pTipWnd->Create(m_pControl);
		}


		CWindowDC dc(m_pControl);
		CXTPFontDC font(&dc, pMetrics->pFont);
		CSize sz = dc.GetTextExtent(strText);

		if (sz.cx > rcItem.Width() - 4)
		{
			rcItem.InflateRect(1, 1, 0, 0);

			pTipWnd->SetFont(pMetrics->pFont);
			pTipWnd->SetTooltipText(strText);
			pTipWnd->SetHoverRect(rcItem);
			pTipWnd->Activate(TRUE);

			TRACKMOUSEEVENT tme =
			{
				sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_pControl->GetSafeHwnd(), 0
			};
			_TrackMouseEvent (&tme);
		}

		pMetrics->InternalRelease();
	}
}

BOOL CXTPReportRow::HasParent(CXTPReportRow* pRow)
{
	if (m_pParentRow == NULL)
		return FALSE;
	if (pRow == m_pParentRow)
		return TRUE;
	return m_pParentRow->HasParent(pRow);
}

void CXTPReportRow::SetExpanded(BOOL bExpanded)
{
	if (bExpanded != m_bExpanded && HasChildren())
	{
		if (bExpanded)
		{
			m_pControl->_DoExpand(this);
		}
		else
		{
			m_pControl->_DoCollapse(this);
		}

		if (m_pRecord)
		{
			m_pRecord->m_bExpanded = bExpanded;
		}

		m_bExpanded = bExpanded;

		m_pControl->_RefreshIndexes();
		m_pControl->SendMessageToParent(this, NULL, NULL, XTP_NM_REPORT_ROWEXPANDED, NULL);
	}
	else
	{
		m_bExpanded = bExpanded;
	}

}

CXTPReportRow* CXTPReportRow::AddChild(CXTPReportRow* pRow)
{
	GetChilds()->Add(pRow);
	pRow->m_pParentRow = this;

	return pRow;
}

void CXTPReportRow::OnContextMenu(CPoint ptClick)
{
	CXTPReportColumn* pColumn = NULL;
	CXTPReportRecordItem* pItem = HitTest(ptClick, NULL, &pColumn);

	m_pControl->ClientToScreen(&ptClick);

	// send process notification to the user and wait for the reaction
	m_pControl->SendMessageToParent(this, pItem, pColumn, NM_RCLICK, &ptClick);
}

void CXTPReportRow::GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
{
	ASSERT(m_pRecord);
	ASSERT(pDrawArgs->pRow == this);
	ASSERT(pDrawArgs->pItem != NULL);

	CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

	pItemMetrics->pFont = &pPaintManager->m_fontText;
	pItemMetrics->clrForeground = pPaintManager->m_clrWindowText;
	pItemMetrics->clrBackground = XTP_REPORT_COLOR_DEFAULT;

	m_pRecord->GetItemMetrics(pDrawArgs, pItemMetrics);
	pDrawArgs->pItem->GetItemMetrics(pDrawArgs, pItemMetrics);

	m_pControl->GetItemMetrics(pDrawArgs, pItemMetrics);

	if (IsSelected() && (pDrawArgs->pDC && !pDrawArgs->pDC->IsPrinting()))
	{
		if (pDrawArgs->pColumn && IsFocused() && m_pControl->m_pFocusedColumn == pDrawArgs->pColumn)
			return;

		if (GetControl()->HasFocus())
		{
			pItemMetrics->clrForeground = pPaintManager->m_clrHighlightText;
			pItemMetrics->clrBackground = pPaintManager->m_clrHighlight;
		}
		else if (!pPaintManager->m_bHideSelection)
		{
			pItemMetrics->clrForeground = pPaintManager->m_clrSelectedRowText;
			pItemMetrics->clrBackground = pPaintManager->m_clrSelectedRow;
		}
	}
}

BOOL CXTPReportRow::HasChildren() const
{
	return m_pChilds && m_pChilds->GetCount() > 0;
}

int CXTPReportRow::GetIndex() const
{
	return m_nIndex;
}

BOOL CXTPReportRow::IsGroupRow() const
{
	return FALSE;
}

BOOL CXTPReportRow::IsExpanded() const
{
	return m_bExpanded;
}

BOOL CXTPReportRow::IsLastTreeRow() const
{
	if (!m_pParentRow)
		return FALSE;

	CXTPReportRows* pRows = m_pParentRow->GetChilds();

	return pRows->GetCount() > 0 && pRows->GetAt(pRows->GetCount() - 1) == this;
}

void CXTPReportRow::EnsureVisible()
{
	m_pControl->EnsureVisible(this);

}

CXTPReportRow* CXTPReportRow::GetNextSiblingRow() const
{
	if (!m_pParentRows)
		return 0;

	if (m_nChildIndex == -1)
		return 0;

	ASSERT(m_pParentRows->GetAt(m_nChildIndex) == this);

	if (m_nChildIndex < m_pParentRows->GetCount() - 1)
		return m_pParentRows->GetAt(m_nChildIndex + 1);

	return 0;
}

