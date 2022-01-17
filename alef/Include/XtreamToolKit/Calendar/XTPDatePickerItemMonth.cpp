// XTPDatePickerItemMonth.cpp: implementation of the CXTPDatePickerItemMonth class.
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
#include "XTPDatePickerItemMonth.h"
#include "XTPDatePickerControl.h"
#include "XTPDatePickerPaintManager.h"
#include "XTPDatePickerItemDay.h"
#include "XTPDatePickerDaysCollection.h"
#include "XTPDatePickerNotifications.h"

#include "Common/XTPDrawHelpers.h"
#include "XTPCalendarUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerItemMonth

CXTPDatePickerItemMonth::CXTPDatePickerItemMonth(CXTPDatePickerControl* pControl,
												 COleDateTime dtMonth,
												 int nRow, int nCol)
	: m_nRow(nRow), m_nColumn(nCol), m_pControl(pControl), m_dtMonth(dtMonth)
{
	m_rcHeader.SetRectEmpty();
	m_rcDaysOfWeek.SetRectEmpty();
	m_rcWeekNumbers.SetRectEmpty();
	m_rcDaysArea.SetRectEmpty();
	m_rcLeftScroll.SetRectEmpty();
	m_rcRightScroll.SetRectEmpty();

	m_bShowDaysBefore = FALSE;
	m_bShowDaysAfter = FALSE;
	m_bShowLeftScroll = FALSE;
	m_bShowRightScroll = FALSE;

	PopulateDays();
}

CXTPDatePickerItemMonth::~CXTPDatePickerItemMonth()
{
	ClearDays();
}

void CXTPDatePickerItemMonth::PopulateDays()
{
	if (m_dtMonth.GetStatus() != COleDateTime::valid)
	{
		ASSERT(FALSE);
		m_dtMonth = CXTPCalendarUtils::ResetTime(COleDateTime::GetCurrentTime());
	}

	// init days from 1st to last
	COleDateTime dtDay(m_dtMonth.GetYear(), m_dtMonth.GetMonth(), 1, 0, 0, 0);
	COleDateTimeSpan spDay(1, 0, 0, 0);

	// adjust first day of the week
	int nOleFirstDayOfWeek = m_pControl->GetFirstDayOfWeek();

	while (dtDay.GetDayOfWeek() != nOleFirstDayOfWeek)
	{
		dtDay -= spDay;
	}

	// populate all grid days
	for (int nWeek = 0; nWeek < XTP_MAX_WEEKS; nWeek++)
	{
		for (int nDay = 0; nDay < XTP_WEEK_DAYS; nDay++)
		{
			// create and add new day item
			CXTPDatePickerItemDay* pDay = new CXTPDatePickerItemDay(m_pControl, this, dtDay);
			m_arrDays.Add(pDay);
			// move next day
			dtDay += spDay;
		}
	}
}

void CXTPDatePickerItemMonth::AutoAdjustLayout(CRect rcClient)
{
	m_rcMonth = rcClient;
	// calc header rect
	m_rcHeader = m_rcMonth;
	m_rcHeader.bottom = m_rcHeader.top + 18; // TODO: change to font height

	// calc days of week rect
	m_rcDaysOfWeek = m_rcMonth;
	m_rcDaysOfWeek.top = m_rcHeader.bottom;
	m_rcDaysOfWeek.bottom = m_rcDaysOfWeek.top + 18; // TODO: change to font height

	// draw week numbers
	m_rcWeekNumbers = m_rcMonth;
	m_rcWeekNumbers.top = m_rcDaysOfWeek.bottom;
	m_rcWeekNumbers.right = m_rcWeekNumbers.left + 18; // TODO: change to font width

	// calc triangles rect
	m_rcLeftScroll = m_rcHeader;
	m_rcRightScroll = m_rcHeader;
	int nScrollWidth = min(m_rcWeekNumbers.Width(), m_rcHeader.Width() / 8);
	m_rcHeader.left += nScrollWidth;
	m_rcHeader.right -= nScrollWidth;
	m_rcLeftScroll.right = m_rcHeader.left;
	m_rcRightScroll.left = m_rcHeader.right;

	// calc days display params
	m_rcDaysArea = m_rcMonth;
	m_rcDaysArea.top = m_rcDaysOfWeek.bottom;
	m_rcDaysArea.left = m_rcWeekNumbers.right;
	m_rcDaysArea.right -= m_rcWeekNumbers.Width();

	// calc simple day rect sizes
	CSize szDay(m_rcDaysArea.Width() / XTP_WEEK_DAYS,
				m_rcDaysArea.Height() / XTP_MAX_WEEKS);

	// calc rects for all days
	CRect rcDay;
	int nIndex = 0;
	for (int nWeek = 0; nWeek < XTP_MAX_WEEKS; nWeek++)
	{
		for (int nDay = 0; nDay < XTP_WEEK_DAYS; nDay++)
		{
			CXTPDatePickerItemDay* pDay = GetDay(nIndex);
			nIndex++;

			rcDay.left = m_rcDaysArea.left + nDay * szDay.cx + 1;
			rcDay.top = m_rcDaysArea.top + nWeek * szDay.cy;
			rcDay.right = rcDay.left + szDay.cx;
			rcDay.bottom = rcDay.top + szDay.cy;
			pDay->SetRect(rcDay);
		}
	}
}

void CXTPDatePickerItemMonth::ByFontAdjustLayout(CRect rcClient)
{
	CWindowDC dc(CWnd::GetDesktopWindow());
	CSize szDay(m_pControl->m_pPaintManager->CalcDayRect(&dc));
	CSize szHeader(m_pControl->m_pPaintManager->CalcMonthHeaderRect(&dc));
	CSize szWeekText(m_pControl->m_pPaintManager->CalcDayOfWeekRect(&dc));
	CSize szWeekNumber(m_pControl->m_pPaintManager->CalcWeekNumbersRect(&dc));

	m_rcMonth = rcClient;
	// calc header rect
	m_rcHeader = m_rcMonth;
	m_rcHeader.bottom = m_rcHeader.top + szHeader.cy;
	//m_rcHeader.right = m_rcHeader.left + szHeader.cx;

	// calc days of week rect
	m_rcDaysOfWeek = m_rcMonth;
	m_rcDaysOfWeek.top = m_rcHeader.bottom;
	//m_rcDaysOfWeek.bottom = m_rcDaysOfWeek.top + 18; // TODO: change to font height
	m_rcDaysOfWeek.bottom = m_rcDaysOfWeek.top + szWeekText.cy;

	// draw week numbers
	m_rcWeekNumbers = m_rcMonth;
	m_rcWeekNumbers.top = m_rcDaysOfWeek.bottom;
	//m_rcWeekNumbers.right = m_rcWeekNumbers.left + 18; // TODO: change to font width
	m_rcWeekNumbers.right = m_rcWeekNumbers.left + szWeekNumber.cx;

	// calc triangles rect
	m_rcLeftScroll = m_rcHeader;
	m_rcRightScroll = m_rcHeader;
	m_rcLeftScroll.right = m_rcHeader.left+ szWeekNumber.cx + 3;
	m_rcRightScroll.left = m_rcHeader.right - szWeekNumber.cx - 3;

	// calc days display params
	m_rcDaysArea = m_rcMonth;
	m_rcDaysArea.top = m_rcDaysOfWeek.bottom;
	m_rcDaysArea.left = m_rcWeekNumbers.right;
	m_rcDaysArea.right -= m_rcWeekNumbers.Width();

	CRect rcDay;
	int nIndex = 0;
	for (int nWeek = 0; nWeek < XTP_MAX_WEEKS; nWeek++)
	{
		for (int nDay = 0; nDay < XTP_WEEK_DAYS; nDay++)
		{
			CXTPDatePickerItemDay* pDay = GetDay(nIndex);
			nIndex++;

			rcDay.left = m_rcDaysArea.left + nDay * szDay.cx + 1;
			rcDay.top = m_rcDaysArea.top + nWeek * szDay.cy;
			rcDay.right = rcDay.left + szDay.cx ;
			rcDay.bottom = rcDay.top + szDay.cy;
			pDay->SetRect(rcDay);
		}
	}

}

void CXTPDatePickerItemMonth::AdjustLayout(CRect rcClient, BOOL bIsAuto)
{
	if (bIsAuto)
		AutoAdjustLayout(rcClient);
	else
		ByFontAdjustLayout(rcClient);
}

CXTPDatePickerItemDay* CXTPDatePickerItemMonth::GetDay(int nIndex)
{
	if (nIndex < GetDayCount())
		return m_arrDays[nIndex];
	return NULL;
}

int CXTPDatePickerItemMonth::GetDayCount() const
{
	return (int)m_arrDays.GetSize();
}


void CXTPDatePickerItemMonth::Draw(CDC* pDC)
{
	// draw header
	CXTPDatePickerPaintManager* pPaintManager = m_pControl->GetPaintManager();
	pPaintManager->DrawMonthHeader(pDC, this);

	// draw days of week
	pPaintManager->DrawDaysOfWeek(pDC, this);

	//  draw all days
	int nDaysCount = GetDayCount();
	for (int nIndex = 0; nIndex < nDaysCount; nIndex++)
	{
		CXTPDatePickerItemDay* pDay = GetDay(nIndex);
		pDay->Draw(pDC);
	}

	// draw week numbers
	pPaintManager->DrawWeekNumbers(pDC, this);
}

// TODO: remove
//void CXTPDatePickerItemMonth::DrawHeader(CDC* pDC)
//{
//  if (m_rcHeader.IsRectEmpty())
//      return;
//
//  // get current paint manager
//  CXTPDatePickerPaintManager* pPaintManager = m_pControl->GetPaintManager();
//
//  // fill background
//  pDC->FillSolidRect(m_rcHeader, pPaintManager->GetHeaderBackColor());
//
//  // draw header text
//  pDC->SetBkColor(pPaintManager->GetHeaderBackColor());
//  pDC->SetTextColor(pPaintManager->GetHeaderTextColor());
//  CXTPFontDC fnt(pDC, pPaintManager->GetHeaderTextFont());
//  CString strText;
//  strText.Format(_T("%s %d"), (LPCTSTR)m_pControl->GetMonthName(m_dtMonth.GetMonth()), m_dtMonth.GetYear());
//  pDC->DrawText(strText, &m_rcHeader, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
//
//  // draw left scroll triangle
//  if (!m_rcLeftScroll.IsRectEmpty())
//  {
//      // fill background
//      pDC->FillSolidRect(m_rcLeftScroll, pPaintManager->GetHeaderBackColor());
//      // draw left triangle itself
//      if (m_bShowLeftScroll)
//          pPaintManager->DrawScrollTriangle(pDC, m_rcLeftScroll, TRUE);
//  }
//
//  // draw right scroll triangle
//  if (!m_rcRightScroll.IsRectEmpty())
//  {
//      // fill background
//      pDC->FillSolidRect(m_rcRightScroll, pPaintManager->GetHeaderBackColor());
//      // draw right triangle itself
//      if (m_bShowRightScroll)
//          pPaintManager->DrawScrollTriangle(pDC, m_rcRightScroll, FALSE);
//  }
//}
//
//void CXTPDatePickerItemMonth::DrawDaysOfWeek(CDC* pDC)
//{
//  if (m_rcDaysOfWeek.IsRectEmpty())
//      return;
//
//  // get current paint manager
//  CXTPDatePickerPaintManager* pPaintManager = m_pControl->GetPaintManager();
//
//  // fill background
//  pDC->FillSolidRect(m_rcDaysOfWeek, pPaintManager->GetDaysOfWeekBackColor());
//
//  // draw days of week text
//  pDC->SetBkColor(pPaintManager->GetDaysOfWeekBackColor());
//  pDC->SetTextColor(pPaintManager->GetDaysOfWeekTextColor());
//  CXTPFontDC fnt(pDC, pPaintManager->GetDaysOfWeekTextFont());
//  int nMaxX = 0;
//  for (int nDayDelta = 0; nDayDelta < XTP_WEEK_DAYS; nDayDelta++)
//  {
//      // calc item rect
//      CXTPDatePickerItemDay* pDay = GetDay(nDayDelta);
//      CRect rcItem(pDay->GetRect());
//      rcItem.top = m_rcDaysOfWeek.top;
//      rcItem.bottom = m_rcDaysOfWeek.bottom - 2;
//      nMaxX = rcItem.right;
//      // get item text
//      CString strText(m_pControl->GetDayOfWeekName(pDay->GetDate().GetDayOfWeek()));
//      pDC->DrawText(strText.Left(1), &rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
//  }
//
//  // draw bottom line on days of the week
//  CRect rcBottomLine(m_rcDaysOfWeek);
//  rcBottomLine.bottom--;
//  rcBottomLine.top = rcBottomLine.bottom - 1;
//  rcBottomLine.left = m_rcWeekNumbers.right - 1;
//  rcBottomLine.right = nMaxX;
//  pDC->FillSolidRect(rcBottomLine, pPaintManager->GetNonMonthDayTextColor());
//}
//
//void CXTPDatePickerItemMonth::DrawWeekNumbers(CDC* pDC)
//{
//  if (!m_pControl->GetShowWeekNumbers() || m_rcWeekNumbers.IsRectEmpty())
//      return;
//
//  // get current paint manager
//  CXTPDatePickerPaintManager* pPaintManager = m_pControl->GetPaintManager();
//
//  // fill background
//  pDC->FillSolidRect(m_rcWeekNumbers, pPaintManager->GetWeekNumbersBackColor());
//
//  // draw week numbers
//  pDC->SetBkColor(pPaintManager->GetWeekNumbersBackColor());
//  pDC->SetTextColor(pPaintManager->GetWeekNumbersTextColor());
//  CXTPFontDC fnt(pDC, pPaintManager->GetWeekNumbersTextFont());
//
//  // draw right line on week days
//  CRect rcLine(m_rcWeekNumbers);
//
//  for (int nWeek = 0; nWeek < XTP_MAX_WEEKS; nWeek++)
//  {
//      // calculate item rectangle (get week middle day)
//      CXTPDatePickerItemDay* pDayFirst = GetDay(nWeek * XTP_WEEK_DAYS);
//      CXTPDatePickerItemDay* pDayLast = GetDay((nWeek + 1) * XTP_WEEK_DAYS - 1);
//
//      if (pDayFirst && (pDayFirst->IsVisible() || nWeek < 4) && pDayLast)
//      {
//          CRect rcItem(pDayFirst->GetRect());
//          rcItem.left = m_rcWeekNumbers.left;
//          rcItem.right = m_rcWeekNumbers.right - 3;
//          // get item text
//          CString strText;
//          int nWeekNumber = ((pDayLast->GetDate() - COleDateTimeSpan(m_pControl->GetFirstWeekOfYearDays() - 1)).GetDayOfYear() - 1) / 7 + 1;
//          strText.Format(_T("%d"), nWeekNumber);
//          pDC->DrawText(strText, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
//
//          rcLine.bottom = rcItem.bottom;
//      }
//  }
//
//  pDC->FillSolidRect(rcLine.right - 1, rcLine.top - 1, 1, rcLine.Height(), pPaintManager->GetNonMonthDayTextColor());
//}

void CXTPDatePickerItemMonth::ClearDays()
{
	// cleanup old month array
	int nOldDayCount = GetDayCount();
	for (int nDay = 0; nDay < nOldDayCount; nDay++)
	{
		CXTPDatePickerItemDay* pDay = GetDay(nDay);
		pDay->InternalRelease();
	}
	m_arrDays.RemoveAll();
}

void CXTPDatePickerItemMonth::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_pControl)
		return;

	// check left scroll item
	if (m_bShowLeftScroll &&
		!m_rcLeftScroll.IsRectEmpty() && m_rcLeftScroll.PtInRect(point))
	{
		m_pControl->m_mouseMode = CXTPDatePickerControl::mouseScrollingLeft;
		m_pControl->m_nTimerID = (UINT)m_pControl->SetTimer(XTP_DATEPICKER_TIMERID, XTP_DATEPICKER_TIMER_INTERVAL, NULL);
		m_pControl->ScrollLeft(m_pControl->GetMonthDelta());
		return;
	}
	// check right scroll item
	if (m_bShowRightScroll &&
		!m_rcRightScroll.IsRectEmpty() && m_rcRightScroll.PtInRect(point))
	{
		m_pControl->m_mouseMode = CXTPDatePickerControl::mouseScrollingRight;
		m_pControl->m_nTimerID = (UINT)m_pControl->SetTimer(XTP_DATEPICKER_TIMERID, XTP_DATEPICKER_TIMER_INTERVAL, NULL);
		m_pControl->ScrollRight(m_pControl->GetMonthDelta());
		return;
	}
	// check header click
	if (!m_rcHeader.IsRectEmpty() && m_rcHeader.PtInRect(point))
	{
		// call month popup list
		m_pControl->ShowListHeader(m_rcHeader, m_dtMonth);
		return;
	}
	// check week numbers click
	m_pControl->m_bSelectWeek = !m_rcWeekNumbers.IsRectEmpty() && m_rcWeekNumbers.PtInRect(point);
	CPoint ptFirstWeekDay(point);
	ptFirstWeekDay.x = m_rcWeekNumbers.right + 3;

	// check day item
	CXTPDatePickerItemDay* pDay = HitTest(m_pControl->m_bSelectWeek ? ptFirstWeekDay : point, FALSE);
	if (pDay)
	{
		m_pControl->ClearFocus();

		BOOL bAllowNoncontinuousSelection = m_pControl->IsAllowNoncontinuousSelection();

		COleDateTime dtDay(pDay->GetDate());
		BOOL bSelected = m_pControl->IsSelected(dtDay);
		if (m_pControl->m_bSelectWeek)
		{
			// week is selected only when all week days are selected
			for (int i = 0; i < 7; i++)
			{
				COleDateTime dtDay_i = dtDay + COleDateTimeSpan(i, 0, 0, 0);
				bSelected &= m_pControl->IsSelected(dtDay_i);
			}
		}
		if (bSelected && (nFlags & MK_CONTROL) && bAllowNoncontinuousSelection)
			m_pControl->m_mouseMode = CXTPDatePickerControl::mouseDeselecting;
		else
			m_pControl->m_mouseMode = CXTPDatePickerControl::mouseSelecting;

		if (!(nFlags & MK_CONTROL) || !bAllowNoncontinuousSelection)
			m_pControl->m_pSelectedDays->Clear();

		if (nFlags & MK_SHIFT)
		{
			if (m_pControl->m_dtFirstClicked.GetStatus() == COleDateTime::valid)
			{
				m_pControl->m_dtLastClicked = dtDay;
			}
			else if (m_pControl->m_dtLastClicked.GetStatus() == COleDateTime::valid)
			{
				m_pControl->m_dtFirstClicked = m_pControl->m_dtLastClicked;
				m_pControl->m_dtLastClicked = dtDay;
			}
			else if (m_pControl->GetSelRange(m_pControl->m_dtFirstClicked, m_pControl->m_dtLastClicked))
			{
				if (m_pControl->m_dtFirstClicked < dtDay)
					m_pControl->m_dtLastClicked = dtDay;
				else
					m_pControl->m_dtFirstClicked = dtDay;
			}
			else
			{
				m_pControl->m_dtFirstClicked = dtDay;
				m_pControl->m_dtLastClicked = dtDay;
			}
		}
		else
		{
			m_pControl->m_dtFirstClicked = dtDay;
			m_pControl->m_dtLastClicked = m_pControl->m_bSelectWeek ?
				dtDay + COleDateTimeSpan(6, 0, 0, 0) : dtDay;
		}

		m_pControl->_RedrawControl(FALSE);
	}
}

void CXTPDatePickerItemMonth::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
}

void CXTPDatePickerItemMonth::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	if ((m_pControl->m_mouseMode == CXTPDatePickerControl::mouseDeselecting ||
		 m_pControl->m_mouseMode == CXTPDatePickerControl::mouseSelecting) &&
		(m_rcDaysArea.PtInRect(point) || m_rcWeekNumbers.PtInRect(point)))
	{
		// check day item
		CPoint ptWeekBegin(m_rcWeekNumbers.right + 3, point.y);
		CXTPDatePickerItemDay* pDay = HitTest(m_pControl->m_bSelectWeek ? ptWeekBegin : point, FALSE);
		if (pDay)
		{
			// save old clicked dates
			COleDateTime dtOldFirstClicked = m_pControl->m_dtFirstClicked;
			COleDateTime dtOldLastClicked = m_pControl->m_dtLastClicked;

			// calculate new clicked dates
			COleDateTime dtDay(pDay->GetDate());
			if (m_pControl->m_bSelectWeek)
			{
				COleDateTime dtWeekEnd(dtDay + COleDateTimeSpan(6, 0, 0, 0));
				if (dtWeekEnd >= m_pControl->m_dtFirstClicked)
				{
					if (dtDay.GetDayOfWeek() != m_pControl->m_dtFirstClicked.GetDayOfWeek())
						m_pControl->m_dtFirstClicked -= COleDateTimeSpan(6, 0, 0, 0);
					m_pControl->m_dtLastClicked = dtWeekEnd;
				}
				if (dtDay < m_pControl->m_dtFirstClicked)
				{
					if (dtDay.GetDayOfWeek() == m_pControl->m_dtFirstClicked.GetDayOfWeek())
						m_pControl->m_dtFirstClicked += COleDateTimeSpan(6, 0, 0, 0);
					m_pControl->m_dtLastClicked = dtDay;
				}
			}
			else
			{
				m_pControl->m_dtLastClicked = dtDay;
			}

			// update control if needed
			if (((dtOldLastClicked - m_pControl->m_dtLastClicked).GetDays() != 0) ||
				 ((dtOldFirstClicked - m_pControl->m_dtFirstClicked).GetDays() != 0))
			{
				m_pControl->RedrawControl();
			}
		}
	}
}

CXTPDatePickerItemDay* CXTPDatePickerItemMonth::HitTest(CPoint point, BOOL bCheckVisible)
{
	// enumerate month array
	int nDayCount = GetDayCount();
	for (int nDay = 0; nDay < nDayCount; nDay++)
	{
		CXTPDatePickerItemDay* pDay = GetDay(nDay);
		if (pDay && pDay->GetRect().PtInRect(point) &&
			(pDay->IsVisible() || !bCheckVisible))
		{
			return pDay;
		}
	}
	return NULL;
}

BOOL CXTPDatePickerItemMonth::OnSetCursor(CPoint point)
{
	if (!m_rcWeekNumbers.IsRectEmpty() && m_rcWeekNumbers.PtInRect(point) &&
		m_pControl && m_pControl->GetPaintManager())
	{
		// week selection cursor
		SetCursor(m_pControl->GetPaintManager()->GetCursorBack());
		return TRUE;
	}
	return FALSE;
}
