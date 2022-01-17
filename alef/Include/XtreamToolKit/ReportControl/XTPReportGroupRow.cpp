// XTPReportGroupRow.cpp : implementation of the CXTPReportGroupRow class.
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

#include "XTPReportControl.h"
#include "XTPReportPaintManager.h"
#include "XTPReportRecordItemText.h"
#include "XTPReportInplaceControls.h"
#include "XTPReportGroupRow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
// CXTPReportGroupItem


CXTPReportGroupRow::CXTPReportGroupRow()
{
	m_bExpanded = TRUE;

}

void CXTPReportGroupRow::SetCaption(LPCTSTR lpszCaption)
{
	m_strGroupText = lpszCaption;
}


CString CXTPReportGroupRow::GetCaption()
{
	return m_strGroupText;
}

void CXTPReportGroupRow::Draw(CDC* pDC, CRect rcRow, int nLeftOffset)
{
	CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();

	pDC->SetBkMode(TRANSPARENT);

	m_rcRow = rcRow;
	if (GetControl()->m_nFreezeColumnsCount == 0)
	{
		m_rcRow.left -= nLeftOffset;
		m_rcRow.right -= nLeftOffset;
	}

	XTP_REPORTRECORDITEM_DRAWARGS drawArgs;
	drawArgs.pDC = pDC;
	drawArgs.nTextAlign = DT_LEFT;

	drawArgs.pControl = m_pControl;
	drawArgs.pRow = this;
	drawArgs.pColumn = NULL;
	drawArgs.pItem = NULL;
	drawArgs.rcItem = m_rcRow;

	XTP_REPORTRECORDITEM_METRICS* pDrawMetrics = new XTP_REPORTRECORDITEM_METRICS;
	pDrawMetrics->strText = GetCaption();

	pPaintManager->FillGroupRowMetrics(this, pDrawMetrics, pDC->IsPrinting());

	ASSERT(m_pControl);
	if (m_pControl)
	{
		m_pControl->GetItemMetrics(&drawArgs, pDrawMetrics);
	}

	pPaintManager->DrawGroupRow(pDC, this, m_rcRow, pDrawMetrics);

	pDrawMetrics->InternalRelease();
}

void CXTPReportGroupRow::OnClick(CPoint ptClicked)
{
	// expand/collapse on single click at the collapse bitmap
	if (m_rcCollapse.PtInRect(ptClicked))
	{
		SetExpanded(!IsExpanded());
	}
}

void CXTPReportGroupRow::OnDblClick(CPoint ptClicked)
{
	// do not expand if double clicked on the collapse bitmap
	if (!m_rcCollapse.PtInRect(ptClicked))
	{
		// otherwise expand on double click
		SetExpanded(!IsExpanded());
	}
	// process parent
	CXTPReportRow::OnDblClick(ptClicked);
}

INT_PTR CXTPReportGroupRow::OnToolHitTest(CPoint /*point*/, TOOLINFO* pTI)
{
	INT_PTR nHit = (INT_PTR)this;
	CString strTip = GetTooltip();

	if (strTip.IsEmpty())
		return -1;

	CXTPToolTipContext::FillInToolInfo(pTI, m_pControl->m_hWnd, m_rcRow,
		nHit, strTip);

	return nHit;

}

