// XTPReportNavigator.cpp : implementation of the CXTPReportNavigator class.
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

#include "Common/XTPResourceManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPVC80Helpers.h"

#include "XTPReportNavigator.h"
#include "XTPReportControl.h"
#include "XTPReportRecord.h"
#include "XTPReportRecordItem.h"
#include "XTPReportColumn.h"
#include "XTPReportColumns.h"
#include "XTPReportInplaceControls.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CXTPReportNavigator

CXTPReportNavigator::CXTPReportNavigator(CXTPReportControl* pReportControl)
	: m_pReportControl(pReportControl)
{

}

CXTPReportNavigator::~CXTPReportNavigator()
{
}

void CXTPReportNavigator::MoveDown(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetNext(
			m_pReportControl->GetFocusedRow(),
			m_pReportControl->m_bSkipGroupsFocus),
		bSelectBlock,
		bIgnoreSelection);
}

void CXTPReportNavigator::MoveUp(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetPrev(
			m_pReportControl->GetFocusedRow(),
			m_pReportControl->m_bSkipGroupsFocus),
		bSelectBlock,
		bIgnoreSelection);
}

void CXTPReportNavigator::MovePageDown(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	int nCurrentRowIndex = m_pReportControl->m_nFocusedRow != -1 ? m_pReportControl->m_nFocusedRow : 0;
	nCurrentRowIndex = min(
		m_pReportControl->m_pRows->GetCount() - 1,
		nCurrentRowIndex + m_pReportControl->GetReportAreaRows(nCurrentRowIndex, true));

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetAt(nCurrentRowIndex),
		bSelectBlock,
		bIgnoreSelection);
}

void CXTPReportNavigator::MovePageUp(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	int nCurrentRowIndex = m_pReportControl->m_nFocusedRow != -1 ? m_pReportControl->m_nFocusedRow : 0;
	nCurrentRowIndex = max(0, nCurrentRowIndex - m_pReportControl->GetReportAreaRows(nCurrentRowIndex, false));

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetAt(nCurrentRowIndex),
		bSelectBlock,
		bIgnoreSelection);
}

void CXTPReportNavigator::MoveFirstRow(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetAt(0),
		bSelectBlock,
		bIgnoreSelection);
}

void CXTPReportNavigator::MoveLastRow(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetAt(m_pReportControl->m_pRows->GetCount() - 1),
		bSelectBlock,
		bIgnoreSelection);
}

void CXTPReportNavigator::MoveToRow(int nRowIndex, BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	int nCurrentRowIndex = max(0, nRowIndex);
	nCurrentRowIndex = min(nCurrentRowIndex, m_pReportControl->m_pRows->GetCount() - 1);
	if (nCurrentRowIndex < 0)
	{
		return;
	}

	m_pReportControl->SetFocusedRow(
		m_pReportControl->m_pRows->GetAt(nCurrentRowIndex),
		bSelectBlock,
		bIgnoreSelection);
}


void CXTPReportNavigator::BeginEdit()
{
	if  (!m_pReportControl)
		return;

	m_pReportControl->AdjustScrollBars();
	m_pReportControl->RedrawControl();
	m_pReportControl->UpdateWindow();

	CXTPReportRow* pFocusedRow = m_pReportControl->GetFocusedRow();

	if (m_pReportControl->m_pFocusedColumn &&
		pFocusedRow && pFocusedRow->GetRecord())
	{
		XTP_REPORTRECORDITEM_ARGS itemArgs(m_pReportControl, pFocusedRow, m_pReportControl->m_pFocusedColumn);

		if (itemArgs.pItem && itemArgs.pItem->IsAllowEdit(&itemArgs))
		{
			if (!m_pReportControl->IsVirtualMode())
			{
				m_pReportControl->EnsureVisible(pFocusedRow);
			}

			m_pReportControl->EditItem(&itemArgs);

			if (m_pReportControl->GetInplaceEdit()->GetSafeHwnd() &&
				m_pReportControl->GetInplaceEdit()->GetItem() == itemArgs.pItem)
			{
				CString str;
				m_pReportControl->GetInplaceEdit()->GetWindowText(str);
				m_pReportControl->GetInplaceEdit()->SetSel(str.GetLength(), str.GetLength());
			}
		}
	}
}

void CXTPReportNavigator::MoveLeftRight(BOOL bBack, BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	if  (!m_pReportControl)
		return;

	CXTPReportControl::CUpdateContext updateContext(m_pReportControl);

	CXTPReportRow* pFocusedRow = m_pReportControl->GetFocusedRow();
	CXTPReportColumn* pFocusedColumn = m_pReportControl->GetNextFocusableColumn(pFocusedRow,
		m_pReportControl->m_pFocusedColumn ? m_pReportControl->m_pFocusedColumn->GetIndex() : -1,
		bBack ? -1 : 1);

	if (pFocusedColumn)
	{
		m_pReportControl->SetFocusedColumn(pFocusedColumn);
	}
	else
	{
		CXTPReportRow* pRow = bBack ?
			m_pReportControl->m_pRows->GetPrev(pFocusedRow, FALSE) :
			m_pReportControl->m_pRows->GetNext(pFocusedRow, FALSE);
		if (pRow && pRow->GetIndex() != m_pReportControl->m_nFocusedRow)
		{
			m_pReportControl->SetFocusedRow(pRow, bSelectBlock, bIgnoreSelection);
			m_pReportControl->SetFocusedColumn(
				m_pReportControl->GetNextFocusableColumn(
					m_pReportControl->GetFocusedRow(),
					bBack ? m_pReportControl->m_pColumns->GetCount() : -1,
					bBack ? -1 : +1)
				);
		}
	}
}

void CXTPReportNavigator::MoveLeft(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	MoveLeftRight(TRUE, bSelectBlock, bIgnoreSelection);
}

void CXTPReportNavigator::MoveRight(BOOL bSelectBlock, BOOL bIgnoreSelection)
{
	MoveLeftRight(FALSE, bSelectBlock, bIgnoreSelection);
}

void CXTPReportNavigator::MoveFirstColumn()
{
	if  (!m_pReportControl)
		return;

	CXTPReportControl::CUpdateContext updateContext(m_pReportControl);
	CXTPReportRow* pFocusedRow = m_pReportControl->GetFocusedRow();
	CXTPReportColumn* pFocusedColumn = m_pReportControl->GetNextFocusableColumn(pFocusedRow, -1, +1);
	if (pFocusedColumn)
	{
		m_pReportControl->SetFocusedColumn(pFocusedColumn);
	}
}

void CXTPReportNavigator::MoveLastColumn()
{
	if  (!m_pReportControl)
		return;

	CXTPReportControl::CUpdateContext updateContext(m_pReportControl);
	CXTPReportRow* pFocusedRow = m_pReportControl->GetFocusedRow();
	CXTPReportColumn* pFocusedColumn = m_pReportControl->GetNextFocusableColumn(pFocusedRow, m_pReportControl->GetColumns()->GetCount(), -1);
	if (pFocusedColumn)
	{
		m_pReportControl->SetFocusedColumn(pFocusedColumn);
	}
}

void CXTPReportNavigator::MoveToColumn(int nColumnIndex, BOOL bClearIfNonFocusable)
{
	if (!m_pReportControl)
	{
		return;
	}

	nColumnIndex = max(0, nColumnIndex);
	nColumnIndex = min(nColumnIndex, m_pReportControl->GetColumns()->GetCount()-1);
	if (nColumnIndex < 0)
	{
		return;
	}

	CXTPReportControl::CUpdateContext updateContext(m_pReportControl);

	CXTPReportRow* pFocusedRow = m_pReportControl->GetFocusedRow();
	CXTPReportColumn* pColumn = m_pReportControl->GetColumns()->GetAt(nColumnIndex);
	if (!pColumn)
	{
		return;
	}

	CXTPReportRecordItem* pItem = pFocusedRow->GetRecord()->GetItem(pColumn);

	if (!pItem || !pItem->IsFocusable())
	{
		if (bClearIfNonFocusable)
		{
			pColumn = NULL;
		}
		else
		{
			return;
		}
	}

	m_pReportControl->SetFocusedColumn(pColumn);
}

//////////////////////////////////////////////////////////////////////////

