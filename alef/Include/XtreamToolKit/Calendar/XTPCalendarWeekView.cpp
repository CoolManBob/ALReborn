// XTPCalendarWeekView.cpp: implementation of the CXTPCalendarWeekView class.
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
#include "XTPCalendarWeekView.h"
#include "XTPCalendarPaintManager.h"
#include "XTPCalendarWeekViewEvent.h"
#include "XTPCalendarWeekViewDay.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarTheme.h"
#include "XTPCalendarNotifications.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarWeekView, CXTPCalendarView)

CXTPCalendarWeekView::CXTPCalendarWeekView(CXTPCalendarControl* pCalendarControl) :
		TBase(pCalendarControl, xtpCalendarWeekView)
{
	m_LayoutX.m_nGridColumns = 2;

	m_nCurrPos = 180;//365;

	m_LayoutX.m_nDayHeaderHeight = 18;
	m_LayoutX.m_nDayWidth = 1;
	m_LayoutX.m_nDayHeidht = 1;

	m_nFirstDayOfWeekIndex = 0;     // 1 = Sunday, 2 = Monday ...

	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	SetBeginDate(dtNow);

	AdjustFirstDayOfWeek();

	SetBeginDate(dtNow);

	//---------------------------------------------------------------------------
	for (int i = 0; i < 7; i++)
	{
		CXTPCalendarWeekViewDay* pWDayView = new CXTPCalendarWeekViewDay(this);
		m_arDays.Add(pWDayView);
	}

}

CXTPCalendarWeekView::~CXTPCalendarWeekView()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////


void CXTPCalendarWeekView::ShowDay(const COleDateTime& date, BOOL bSelect)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CXTPCalendarControl::CViewChangedContext viewChanged(this);

	CXTPCalendarControl::CUpdateContext updateContext2(m_pControl, xtpCalendarUpdateAll);

	SetBeginDate(date);
	Populate();

	if (bSelect)
		SelectDay(CXTPCalendarUtils::ResetTime(date));
}

void CXTPCalendarWeekView::Populate()
{
	AdjustFirstDayOfWeek();

	TBase::Populate();
}

void CXTPCalendarWeekView::AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	if (!GetPaintManager() || !GetPaintManager()->GetWeekViewPart() ||
		!GetPaintManager()->GetWeekViewEventPart() || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	TBase::AdjustLayout(pDC, rcView, FALSE);

	AdjustFirstDayOfWeek();

	CXTPCalendarViewPart* pWVPart = GetPaintManager()->GetWeekViewPart();

	m_LayoutX.m_nDayHeaderHeight = pWVPart->GetTextExtent(pDC, _T("A")).cy + 7;
	m_Layout.m_nRowHeight = GetPaintManager()->GetWeekViewEventPart()->GetTextExtent(pDC, _T("(")).cy + 6;

	// Adjust Week's Grid

	CRect rcView2 = rcView;

	rcView2.DeflateRect(-1, -1, -1, -1);
	int nRows = 6/m_LayoutX.m_nGridColumns;
	ASSERT(6 % m_LayoutX.m_nGridColumns == 0);

	m_LayoutX.m_nDayWidth = rcView2.Width() / m_LayoutX.m_nGridColumns;
	m_LayoutX.m_nDayHeidht = rcView2.Height() / nRows;

	CalculateHeaderFormat(pDC, m_LayoutX.m_nDayWidth, pWVPart);

	int nDayIndex = 0;
	CRect rc(0, 0, 0, 0);

	for (int nDayCol = 0; nDayCol < m_LayoutX.m_nGridColumns; nDayCol++)
	{
		for (int nDayRow = 0; nDayRow < nRows; nDayRow++)
		{
			CXTPCalendarWeekViewDay* pDay = GetViewDay(nDayIndex);
			ASSERT(pDay);
			if (!pDay)
			{
				continue;
			}

			COleDateTime dtDayDate = GetViewDayDate(nDayIndex);

			rc.SetRect( rcView2.left + m_LayoutX.m_nDayWidth * nDayCol,
						rcView2.top + m_LayoutX.m_nDayHeidht* nDayRow,
						rcView2.left + m_LayoutX.m_nDayWidth + m_LayoutX.m_nDayWidth * nDayCol,
						rcView2.top + m_LayoutX.m_nDayHeidht + m_LayoutX.m_nDayHeidht * nDayRow);

			int nWday = dtDayDate.GetDayOfWeek();

			ASSERT(nWday != 1);

			if (nWday == 7)
			{
				CRect rc2 = rc;
				rc2.bottom = rc.top + rc.Height()/2;

				pDay->AdjustLayout(pDC, rc2);

				nDayIndex++;
				pDay = GetViewDay(nDayIndex);
				rc.top = rc2.bottom;
			}

			ASSERT(pDay);
			if (pDay)
			{
				pDay->AdjustLayout(pDC, rc);
			}

			nDayIndex++;
		}
	}

	//---------------------------------------------------------------------------
	if (bCallPostAdjustLayout)
	{
		OnPostAdjustLayout();
	}
}

void CXTPCalendarWeekView::AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	if (!GetTheme())
	{
		ASSERT(FALSE);
		return;
	}
	TBase::AdjustLayout(pDC, rcView, FALSE);
	AdjustFirstDayOfWeek();

	XTP_SAFE_CALL2(GetTheme(), GetWeekViewPart(), AdjustLayout(pDC, rcView, bCallPostAdjustLayout));
}

void CXTPCalendarWeekView::AdjustFirstDayOfWeek()
{
	if (!GetCalendarControl())
	{
		return;
	}

	int nFDPrev = m_nFirstDayOfWeekIndex;

	//- Adjust First day of Week ------------------------------------------
	m_nFirstDayOfWeekIndex = GetCalendarControl()->GetFirstDayOfWeek();
	if (m_nFirstDayOfWeekIndex == 1)
	{
		m_nFirstDayOfWeekIndex = 2;
	}

	int nDShift = m_nFirstDayOfWeekIndex - nFDPrev;
	COleDateTime dtBeginDate_prev = GetBeginDate();
	COleDateTime dtBeginDate = dtBeginDate_prev + COleDateTimeSpan(nDShift);

	SetBeginDate(dtBeginDate);

	dtBeginDate = GetBeginDate();
//  if (!CXTPCalendarUtils::IsEqual(dtBeginDate_prev, dtBeginDate))
//  {
//      CXTPCalendarControl::CViewChangedContext viewChanged(this);
//  }
}

COleDateTime CXTPCalendarWeekView::ShiftDateToCell_00(COleDateTime dtDate) const
{
	int nShift = (dtDate.GetDayOfWeek() - m_nFirstDayOfWeekIndex + 7) % 7;
	if (nShift)
	{
		COleDateTimeSpan spShift(nShift, 0, 0, 0);
		dtDate -= spShift;
	}
	return dtDate;
}

void CXTPCalendarWeekView::SetBeginDate(COleDateTime dtNewBeginDate)
{
	m_dtBeginDate = ShiftDateToCell_00(CXTPCalendarUtils::ResetTime(dtNewBeginDate));
}

void CXTPCalendarWeekView::Draw(CDC* pDC)
{
	XTP_SAFE_CALL2(GetPaintManager(), GetWeekViewPart(), OnDraw(pDC, this));

	// Draw events

	int nDays = m_arDays.GetCount();
	for (int nDay = 0; nDay < nDays; nDay++)
	{
		XTP_SAFE_CALL1(m_arDays.GetAt(nDay), Draw(pDC));
	}
}

void CXTPCalendarWeekView::Draw2(CDC* pDC)
{
	if (!GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	GetTheme()->GetWeekViewPart()->Draw(pDC);
}

void CXTPCalendarWeekView::ScrollV(int /*nPos*/, int nPos_raw)
{
	if (nPos_raw == m_nCurrPos || m_bScrollV_Disabled)
		return;

	CXTPCalendarControl::CViewChangedContext viewChanged(this);

	if (IsEditingSubject())
	{
		EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}

	int nShift = nPos_raw - m_nCurrPos;
	COleDateTimeSpan spWeeks(nShift * 7, 0, 0, 0);

	m_dtBeginDate += spWeeks;

	m_nCurrPos = nPos_raw;

	if (GetSelection())
	{
		m_selectedBlock.dtBegin += spWeeks;
		m_selectedBlock.dtEnd += spWeeks;

		CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
	}
	UnselectAllEvents();

	Populate();
}

BOOL CXTPCalendarWeekView::GetScrollBarInfoV(SCROLLINFO* pSI)
{
	ASSERT(pSI);
	if (!pSI || m_bScrollV_Disabled)
	{
		return FALSE;
	}

	pSI->nPos = m_nCurrPos;
	pSI->nMax = 360;//730;
	pSI->nPage = 1;

	return TRUE;
}

COleDateTime CXTPCalendarWeekView::GetViewDayDate(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < 7);

	COleDateTimeSpan spDays(nIndex, 0, 0, 0);
	COleDateTime dtDayDate = m_dtBeginDate + spDays;

	return dtDayDate;
}

void CXTPCalendarWeekView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CSelectionChangedContext selChanged(this);

	TBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN ||
			nChar == VK_PRIOR || nChar == VK_NEXT)
	{
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

		BOOL bVKShift = !!(GetKeyState(VK_SHIFT) & 0x8000);
		COleDateTimeSpan spDay(1, 0, 0, 0);
		COleDateTimeSpan spWeek(7, 0, 0, 0);

		int nDVCount = GetViewDayCount();
		BOOL bSelReseted = !m_selectedBlock.dtEnd.GetStatus() == COleDateTime::valid;

		COleDateTime dtLastSelDay = GetLastSelectedDate();
		int nLastSelDayIdx = (dtLastSelDay.GetDayOfWeek() - m_nFirstDayOfWeekIndex + 7) % 7;

		COleDateTime dtNewSelDay = dtLastSelDay;

		int nSatDayIdx = 7 - m_nFirstDayOfWeekIndex;
		int nNewIdx = nLastSelDayIdx + 3;
		int nHalfShift = nNewIdx > nSatDayIdx && nLastSelDayIdx <= nSatDayIdx ||
						 nNewIdx > nSatDayIdx + 7 && nLastSelDayIdx <= nSatDayIdx + 7 ? 4 : 3;

		COleDateTimeSpan spHalfWeekPlus(nHalfShift, 0, 0, 0);
		COleDateTimeSpan spHalfWeekMinus(7 - nHalfShift, 0, 0, 0);

		CXTPDrawHelpers::KeyToLayout(m_pControl, nChar);

		if (nChar == VK_LEFT)
		{
			dtNewSelDay -= spHalfWeekMinus;
		}
		else
			if (nChar == VK_RIGHT)
			{
				dtNewSelDay += spHalfWeekPlus;
			}
			else
				if (nChar == VK_UP)
				{
					dtNewSelDay -= spDay;
				}
				else
					if (nChar == VK_DOWN)
					{
						dtNewSelDay += spDay;
					}
					else
						if (nChar == VK_PRIOR)
						{
							dtNewSelDay -= spWeek;
						}
						else
							if (nChar == VK_NEXT)
							{
								dtNewSelDay += spWeek;
							}
		//---------------------------------------------------------------------------
		COleDateTime dtFirstDay = GetViewDayDate(0);
		COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);

		BOOL bNeedScroll = dtNewSelDay < dtFirstDay || dtNewSelDay > dtLastDay;
		if (bNeedScroll && !m_bScrollV_Disabled)
		{
			if (dtNewSelDay < dtFirstDay)
			{
				ScrollV(m_nCurrPos-1, m_nCurrPos-1);
				bSelReseted = TRUE;
				CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
			}
			if (dtNewSelDay > dtLastDay)
			{
				ScrollV(m_nCurrPos + 1, m_nCurrPos + 1);
				bSelReseted = TRUE;
				CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
			}
		}

		//---------------------------------------------------------------------------
		if (!bNeedScroll || !m_bScrollV_Disabled)
		{
			if (bVKShift && !bSelReseted)
			{
				SelectDays(dtNewSelDay);
			}
			else
			{
				SelectDay(dtNewSelDay);
			}
		}
	}
}


