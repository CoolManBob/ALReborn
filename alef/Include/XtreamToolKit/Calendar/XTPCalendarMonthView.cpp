// XTPCalendarMonthView.cpp: implementation of the CXTPCalendarMonthView class.
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

#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarMonthView.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarTheme.h"
#include "XTPCalendarNotifications.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_CAL_MIDDLE_SCROLL_POS 49

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarMonthView, CXTPCalendarView)

CXTPCalendarMonthView::CXTPCalendarMonthView(CXTPCalendarControl* pCalendarControl) :
		TBase(pCalendarControl, xtpCalendarMonthView)
{
	m_strLongDateFormat = _T("MMMM d");
	m_strSmallDateFormat = _T("d");

	m_nEventCaptionFormat = xtpCalendarCaptionFormatSubject;
	m_nEventTimeWidth = 0;

	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	COleDateTime dtFirst(dtNow.GetYear(), dtNow.GetMonth(), 1, 0, 0, 0);

	m_pGrid = new CMonthViewGrid(this);
	if (m_pGrid)
	{
		m_pGrid->SetWeeksCount(5);
		m_pGrid->SetBeginDate(dtFirst);

		m_pGrid->AdjustFirstDayOfWeek();
		m_pGrid->SetBeginDate(dtFirst);
	}

}

void CXTPCalendarMonthView::ShowDay(const COleDateTime& date, BOOL bSelect)
{
	if (!m_pGrid)
	{
		return;
	}

	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateAll);

	m_pGrid->SetBeginDate(date);
	Populate();

	UnselectAllEvents();
	if (bSelect)
		SelectDay(CXTPCalendarUtils::ResetTime(date));
}

CXTPCalendarMonthView::~CXTPCalendarMonthView()
{
	if (m_pGrid)
	{
		delete m_pGrid;
		m_pGrid = NULL;
	}
}

void CXTPCalendarMonthView::Populate()
{
	ASSERT(m_pGrid);
	if (m_pGrid)
	{
		m_pGrid->AdjustFirstDayOfWeek();
		TBase::Populate();
	}
}

void CXTPCalendarMonthView::AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	ASSERT(pDC);
	if (!m_pGrid || !pDC)
	{
		return;
	}

	TBase::AdjustLayout(pDC, rcView, FALSE);

	_CalculateDateFormats(pDC);
	_CalculateEventCaptionFormat(pDC);

	m_pGrid->AdjustLayout(pDC);

	//---------------------------------------------------------------------------
	if (bCallPostAdjustLayout)
	{
		OnPostAdjustLayout();
	}
}

void CXTPCalendarMonthView::AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	if (!GetTheme())
	{
		ASSERT(FALSE);
		return;
	}
	TBase::AdjustLayout2(pDC, rcView, bCallPostAdjustLayout);

	GetTheme()->GetMonthViewPart()->AdjustLayout(pDC, rcView, bCallPostAdjustLayout);
}

void CXTPCalendarMonthView::ScrollV(int /*nPos*/, int nPos_raw)
{
	if (!m_pGrid || m_bScrollV_Disabled)
	{
		return;
	}

	if (IsEditingSubject())
	{
		EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}

	COleDateTime dtFirst0 = m_pGrid->GetBeginDate();

	m_pGrid->ScrollV(nPos_raw);

	COleDateTime dtFirst1 = m_pGrid->GetBeginDate();

	if (GetSelection())
	{
		COleDateTimeSpan spShift = dtFirst1 - dtFirst0;
		m_selectedBlock.dtBegin += spShift;
		m_selectedBlock.dtEnd += spShift;

		CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
	}
	UnselectAllEvents();

	Populate();
}

void CXTPCalendarMonthView::Draw2(CDC* pDC)
{
	if (!GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	GetTheme()->GetMonthViewPart()->Draw(pDC);
}

void CXTPCalendarMonthView::Draw(CDC* pDC)
{
	ASSERT(pDC);
	if (!pDC || !m_pGrid)
	{
		return;
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(GetViewRect(), GetSysColor(COLOR_3DFACE));

	m_pGrid->Draw(pDC);

	//-- Draw Days ------------------------------
	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarMonthViewDay* pViewDay = GetViewDay(i);
		ASSERT(pViewDay);
		if (pViewDay)
		{
			pViewDay->Draw(pDC);
		}
	}
}

void CXTPCalendarMonthView::_SplitDateFormat(const CString& strDateFormat, CStringArray& rarTokens)
{
	CString strSeparators = _T(" dDmMyYgG");
	CString strToken;

	CString strCHprev;
	BOOL bIsPrevSeparator = FALSE;

	int nLength = strDateFormat.GetLength();
	for (int i = 0; i < nLength; i++)
	{
		CString strCH(strDateFormat[i]);

		if (strCH == _T("'"))
		{
			strToken += strCH;
			for (i++; i < nLength; i++)
			{
				strCH = strDateFormat[i];
				CString strCH2((i + 1 < nLength) ? strDateFormat[i+1] : _T('\0'));

				strToken += strCH;

				if (strCH == _T("'") && strCH2 == _T("'"))
				{
					strToken += strCH2;
					i++;
					continue;
				}

				if (strCH == _T("'"))
				{
					break;
				}
			}
			rarTokens.Add(strToken);
			strToken.Empty();
			strCHprev.Empty();

			continue;
		}

		BOOL bIsSeparator = (strSeparators.Find(strCH) >= 0);

		if (bIsPrevSeparator != bIsSeparator ||
			bIsPrevSeparator && bIsSeparator && strCH.CompareNoCase(strCHprev)
		  )
		{
			if (!strToken.IsEmpty())
			{
				rarTokens.Add(strToken);
			}
			strToken.Empty();
		}
		strToken += strCH;

		strCHprev = strCH;
		bIsPrevSeparator = bIsSeparator;
	}

	if (!strToken.IsEmpty())
	{
		rarTokens.Add(strToken);
	}
}

void CXTPCalendarMonthView::_ReadDefaultDateFormats()
{
	const CString strSpecials(_T(" ,.-_=+|\?/<>~`!@#$%^&*()"));

	CString strLongDateFormat;
	CString strSmallDateFormat;

	CString strLocaleFormat = CXTPCalendarUtils::GetLocaleString(LOCALE_SLONGDATE, 81);

	CStringArray arTokens;
	_SplitDateFormat(strLocaleFormat, arTokens);

	BOOL bDayAdded = FALSE;
	BOOL bMonthAdded = FALSE;
	BOOL bDateFormatTokenWas = FALSE;

	int nCount = (int)arTokens.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString strToken = arTokens[i];

		if (!bDayAdded && (strToken == _T("d") || strToken == _T("dd")))
		{
			strLongDateFormat += _T("d");
			strSmallDateFormat += _T("d");

			bDayAdded = TRUE;
			if (i + 1 < nCount)
			{
				strLongDateFormat += arTokens[i + 1];
				strSmallDateFormat += arTokens[i + 1];
			}
		}

		if (!bMonthAdded && (strToken == _T("M") || strToken == _T("MM") ||
			strToken == _T("MMM") || strToken == _T("MMMM")))
		{
			strLongDateFormat += strToken;
			bMonthAdded = TRUE;
			if (i + 1 < nCount)
			{
				strLongDateFormat += arTokens[i + 1];
			}
		}

		if (!bDateFormatTokenWas)
		{
			if (strToken == _T("ddd") || strToken == _T("dddd") || strToken == _T("gg") ||
				strToken == _T("y") || strToken == _T("yy") || strToken == _T("yyyy"))
			{
				bDateFormatTokenWas = TRUE;
				if (!bDayAdded && !bMonthAdded)
				{
					strLongDateFormat.Empty();
				}
				if (!bDayAdded)
				{
					strSmallDateFormat.Empty();
				}
			}
		}

		if (!bDateFormatTokenWas && !bDayAdded && !bMonthAdded)
		{
			strLongDateFormat += strToken;
			strSmallDateFormat += strToken;
		}
	}

	TRIMRIGHT_S(strLongDateFormat, strSpecials);
	TRIMRIGHT_S(strSmallDateFormat, strSpecials);

	if (!bDayAdded && !bMonthAdded)
	{
		strLongDateFormat = _T("M-d");
		strSmallDateFormat = _T("d");
	}
	else if (!bDayAdded || !bMonthAdded)
	{
		int nLen = strLongDateFormat.GetLength();
		CString strCHlast = nLen ? strLongDateFormat.Right(1) : _T("");
		if (strCHlast.FindOneOf(strSpecials) < 0)
		{
			strLongDateFormat += _T("-");
		}

		if (!bDayAdded)
		{
			strLongDateFormat += _T("d");
			strSmallDateFormat = _T("d");
		}
		else
		{
			strLongDateFormat += _T("M");
		}
	}

	m_strDayHeaderFormatDefaultLong = strLongDateFormat;
	m_strDayHeaderFormatDefaultMiddle = m_strDayHeaderFormatDefaultLong;
	REPLACE_S(m_strDayHeaderFormatDefaultMiddle, _T("MMMM"), _T("MMM"));

	m_strDayHeaderFormatDefaultShort = m_strDayHeaderFormatDefaultMiddle;
	REPLACE_S(m_strDayHeaderFormatDefaultShort, _T("MMM"), _T("MM"));

	m_strDayHeaderFormatDefaultShortest = strSmallDateFormat;
}

void CXTPCalendarMonthView::_CalculateDateFormats(CDC* pDC)
{
	if (!m_pGrid || !m_pControl || !GetPaintManager() || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	_ReadDefaultDateFormats();

	m_strLongDateFormat = _GetDayHeaderFormat(0);
	m_strSmallDateFormat = _GetDayHeaderFormat(3);

	CXTPCalendarViewPart* pPart = GetPaintManager()->GetMonthViewEventPart();
	if (!pPart)
	{
		return;
	}

	CXTPFontDC fnt(pDC, &pPart->GetTextFont());

	int nBorderX = pDC->GetTextExtent(_T("WW"), 2).cx;
	int arBorder[] = {nBorderX*3/2, nBorderX, nBorderX/2, 0};

	int nColsCount = max(1, m_pGrid->GetColsCount());
	int nCellWidth = GetViewRect().Width() / nColsCount;

	for (int i = 0; i <= 2; i++)
	{
		m_strLongDateFormat = _GetDayHeaderFormat(i);
		int nLen = _GetMaxWidth(pDC, m_strLongDateFormat);

		if (nLen < nCellWidth - arBorder[i])
		{
			return;
		}
	}
}

CString CXTPCalendarMonthView::_FormatDayDate(COleDateTime dtDay, BOOL bLong)
{
	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(st));

	CString strDate;
	int nDay = dtDay.GetDay();

	CString strFormat = (bLong || nDay == 1) ? m_strLongDateFormat : m_strSmallDateFormat;

	if (GETASSYSTEMTIME_DT(dtDay, st))
	{
		strDate = CXTPCalendarUtils::GetDateFormat(&st, strFormat);
	}
	if (strDate.IsEmpty())
	{
		ASSERT(FALSE);
		strDate.Format(_T("%d-%d"), dtDay.GetMonth(), dtDay.GetDay());
	}

	return strDate;
}

BOOL CXTPCalendarMonthView::GetScrollBarInfoV(SCROLLINFO* pSI)
{
	ASSERT(pSI);
	if (!pSI || m_bScrollV_Disabled)
	{
		return FALSE;
	}

	pSI->nPos = m_pGrid->GetScrollPos();
	pSI->nMax = 105;
	pSI->nPage = m_pGrid->GetWeeksCount();

	return TRUE;
}

CXTPCalendarMonthViewDay* CXTPCalendarMonthView::GetViewDay(int nIndex)
{
	if (!m_pGrid)
	{
		return NULL;
	}

	int nWeeksCount = m_pGrid->GetWeeksCount();
	int nCount = m_arDays.GetCount();
	ASSERT(nCount == nWeeksCount * 7);

	if (nIndex >= nWeeksCount * 7 || nIndex >= nCount)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CXTPCalendarMonthViewDay* pDView = m_arDays.GetAt(nIndex);

	if (!pDView)
	{
		int nWeekIndex = nIndex / 7;
		int nWeekDayIndex = nIndex % 7;

		pDView = new CXTPCalendarMonthViewDay(this, nWeekIndex, nWeekDayIndex);
		m_arDays.SetAt(nIndex, pDView);
	}

	return pDView;
}

COleDateTime CXTPCalendarMonthView::GetViewDayDate(int nIndex)
{
	int nWeekIndex = nIndex / 7;
	int nWeekDayIndex = nIndex % 7;

	return XTP_SAFE_GET1(m_pGrid, GetDayDate(nWeekIndex, nWeekDayIndex), (DATE)0);
}


void CXTPCalendarMonthView::_CalculateEventCaptionFormat(CDC* pDC)
{
	if (!m_pGrid || !m_pControl || !GetPaintManager() || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bShowEnd = m_pControl->MonthView_IsShowEndDate();
	BOOL bShowAsClocks = m_pControl->MonthView_IsShowTimeAsClocks();

	CXTPCalendarViewPart* pPart = GetPaintManager()->GetMonthViewEventPart();
	if (!pPart)
	{
		return;
	}

	CXTPFontDC fnt(pDC, &pPart->GetTextFont());

	COleDateTime dtTestTime;
	dtTestTime.SetTime(23, 59, 0);

	//--------------------
	CSize szW = pDC->GetTextExtent(_T(" "), 1);

	int nColsCount = max(1, m_pGrid->GetColsCount());
	int nCellWidth = GetViewRect().Width() / nColsCount;
	int nTimeWithMax = (nCellWidth - szW.cx * 2)/2;

	//--------------------
	m_nEventCaptionFormat = bShowEnd ? xtpCalendarCaptionFormatStartEndSubject : xtpCalendarCaptionFormatStartSubject;

	m_nEventTimeWidth = (GetPaintManager()->GetClockSize().cx + szW.cx);

	if (!bShowAsClocks)
	{
		CString strTime = FormatEventTime(dtTestTime);
		m_nEventTimeWidth = pDC->GetTextExtent(strTime).cx + szW.cx;
	}

	if (bShowEnd)
	{
		m_nEventTimeWidth *= 2;
	}

	if (m_nEventTimeWidth > nTimeWithMax && m_nEventCaptionFormat == xtpCalendarCaptionFormatStartEndSubject)
	{
		m_nEventCaptionFormat = xtpCalendarCaptionFormatStartSubject;
		m_nEventTimeWidth /= 2;
	}

	if (m_nEventTimeWidth > nTimeWithMax)
	{
		m_nEventCaptionFormat = xtpCalendarCaptionFormatSubject;
		m_nEventTimeWidth = 0;
	}
}

BOOL CXTPCalendarMonthView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetTheme() && GetTheme()->GetMonthViewPart())
		if (GetTheme()->GetMonthViewPart()->OnLButtonDown(this, nFlags, point))
			return TRUE;

	return TBase::OnLButtonDown(nFlags, point);
}

void CXTPCalendarMonthView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetTheme() && GetTheme()->GetMonthViewPart())
		GetTheme()->GetMonthViewPart()->OnMouseMove(this, nFlags, point);

	TBase::OnMouseMove(nFlags, point);
}

void CXTPCalendarMonthView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
		COleDateTimeSpan spPage(GetViewDayCount(), 0, 0, 0);

		int nDVCount = GetViewDayCount();
		BOOL bSelReseted = m_selectedBlock.dtEnd.GetStatus() != COleDateTime::valid;


		COleDateTime dtLastSelDay = GetLastSelectedDate();
		COleDateTime dtNewSelDay = dtLastSelDay;

		CXTPDrawHelpers::KeyToLayout(m_pControl, nChar);

		if (nChar == VK_LEFT)
		{
			dtNewSelDay -= spDay;
		}
		else
			if (nChar == VK_RIGHT)
			{
				dtNewSelDay += spDay;
			}
			else
				if (nChar == VK_UP)
				{
					dtNewSelDay -= spWeek;
				}
				else
					if (nChar == VK_DOWN)
					{
						dtNewSelDay += spWeek;
					}
					else
						if (nChar == VK_PRIOR)
						{
							dtNewSelDay -= spPage;
						}
						else
							if (nChar == VK_NEXT)
							{
								dtNewSelDay += spPage;
							}
		//---------------------------------------------------------------------------
		COleDateTime dtFirstDay = GetViewDayDate(0);
		COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);

		BOOL bNeedScroll = dtNewSelDay < dtFirstDay || dtNewSelDay > dtLastDay;
		if (bNeedScroll && !m_bScrollV_Disabled)
		{
			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateAll);

			if (m_pGrid)
			{
				m_pGrid->SetBeginDate(dtNewSelDay);
				Populate();
				bSelReseted = TRUE;
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
				UnselectAllEvents();
				SelectDay(dtNewSelDay);
			}
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
//class CXTPCalendarMonthView::CMonthViewGrid

CXTPCalendarMonthView::CMonthViewGrid::CMonthViewGrid(CXTPCalendarMonthView* pView)
{
	m_pView = pView;

	m_dtBeginDate.SetStatus(COleDateTime::null);

	m_nWeeksCount = 0;
	SetWeeksCount(5);

	m_nColHeaderHeight = 18;

	m_nFirstDayOfWeekIndex = 2;     // 1 = Sunday, 2 = Monday ...

}

CXTPCalendarMonthView::CMonthViewGrid::~CMonthViewGrid()
{
}


COleDateTime CXTPCalendarMonthView::CMonthViewGrid::GetMiddleScrollBeginDate()
{
	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();
	COleDateTime dtFirst(dtNow.GetYear(), dtNow.GetMonth(), 1, 0, 0, 0);
	COleDateTime dtMiddle = ShiftDateToCell_00(dtFirst);

	return dtMiddle;
}

int CXTPCalendarMonthView::CMonthViewGrid::GetScrollPos()
{
	COleDateTime dtMiddle = GetMiddleScrollBeginDate();
	COleDateTime dtBegin = GetBeginDate();

	COleDateTimeSpan spPos = dtBegin - dtMiddle;
	int nDays = (int)spPos.GetTotalDays();

	ASSERT(nDays % 7 == 0);

	int nPos = nDays/7 + XTP_CAL_MIDDLE_SCROLL_POS;

	return nPos;
}

void CXTPCalendarMonthView::CMonthViewGrid::ScrollV(int nPos)
{
	COleDateTimeSpan    spScroll((nPos - XTP_CAL_MIDDLE_SCROLL_POS)*7, 0, 0, 0);
	COleDateTime dtMiddle = GetMiddleScrollBeginDate();
	COleDateTime dtScrolled_00 = dtMiddle + spScroll;

	SetBeginDate(dtScrolled_00);
}

COleDateTime CXTPCalendarMonthView::CMonthViewGrid::ShiftDateToCell_00(COleDateTime dtDate)
{
	dtDate = CXTPCalendarUtils::ResetTime(dtDate);

	int nShift = (dtDate.GetDayOfWeek() - m_nFirstDayOfWeekIndex + 7) % 7;
	if (nShift)
	{
		COleDateTimeSpan spShift(nShift, 0, 0, 0);
		dtDate -= spShift;
	}
	return dtDate;
}

void CXTPCalendarMonthView::CMonthViewGrid::SetWeeksCount(int nWeeks)
{
	ASSERT(nWeeks >= XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN);
	ASSERT(nWeeks <= XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX);

	ASSERT(m_pView);
	if (!m_pView)
	{
		return;
	}

	if (m_nWeeksCount != nWeeks)
	{
		if (m_pView->IsEditingSubject())
		{
			m_pView->EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
		}
	}

	m_nWeeksCount = max(nWeeks, XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MIN);
	m_nWeeksCount = min(m_nWeeksCount, XTP_CALENDAR_MONTHVIEW_SHOW_WEEKS_MAX);

	int nDaysCount = m_nWeeksCount * 7;
	int nCount = (int)m_pView->m_arDays.GetCount();

	if (nDaysCount != nCount)
	{
		m_pView->m_arDays.SetSize(nDaysCount);

		CXTPCalendarControl::CViewChangedContext viewChanged(m_pView);
	}
}

COleDateTime CXTPCalendarMonthView::CMonthViewGrid::GetDayDate(int nWeekIndex, int nWeekDayIndex) const
{
	ASSERT(nWeekIndex >= 0 && nWeekIndex < m_nWeeksCount);
	ASSERT(nWeekDayIndex >= 0 && nWeekDayIndex < 7);

	int nDayIndex = nWeekIndex * 7 + nWeekDayIndex;
	COleDateTimeSpan spShift(nDayIndex, 0, 0, 0);
	COleDateTime dtDay = m_dtBeginDate + spShift;

	return dtDay;
}

CRect CXTPCalendarMonthView::CMonthViewGrid::GetDayRect(int nWeekIndex, int nWeekDayIndex, BOOL bIncludeBorders) const
{
	BOOL bCompressWeD = XTP_SAFE_GET2(m_pView, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);

	COleDateTime dtDate = GetDayDate(nWeekIndex, nWeekDayIndex);
	int nWDay = dtDate.GetDayOfWeek();

	int nColIdx = nWeekDayIndex;
	int nCompressedColIdx = 7 - m_nFirstDayOfWeekIndex;
	if (bCompressWeD && nColIdx > nCompressedColIdx)
	{
		nColIdx--;
	}

	CRect rcDay(0, 0, 0, 0);
	rcDay.left = m_arColsLeftX[nColIdx];
	rcDay.right = m_arColsLeftX[nColIdx + 1];

	int nHalfHeight = (m_arRowsTopY[nWeekIndex + 1] - m_arRowsTopY[nWeekIndex]) / 2;

	if (bCompressWeD && nWDay == 7)
	{
		rcDay.top = m_arRowsTopY[nWeekIndex];
		rcDay.bottom = rcDay.top + nHalfHeight;
	}
	else
		if (bCompressWeD && nWDay == 1)
		{
			rcDay.top = m_arRowsTopY[nWeekIndex] + nHalfHeight;
			rcDay.bottom = m_arRowsTopY[nWeekIndex + 1];
		}
		else
		{
			rcDay.top = m_arRowsTopY[nWeekIndex];
			rcDay.bottom = m_arRowsTopY[nWeekIndex + 1];
		}

	if (!bIncludeBorders)
	{
		rcDay.DeflateRect(1, 1, 0, 0);
	}


	return rcDay;
}

CRect CXTPCalendarMonthView::CMonthViewGrid::GetCelRect(int nWeekIndex, int nColIndex, BOOL bIncludeBorders) const
{
	CRect rcCell(0, 0, 0, 0);

	rcCell.left = m_arColsLeftX[nColIndex];
	rcCell.right = m_arColsLeftX[nColIndex + 1];

	rcCell.top = m_arRowsTopY[nWeekIndex];
	rcCell.bottom = m_arRowsTopY[nWeekIndex + 1];

	if (!bIncludeBorders)
	{
		rcCell.DeflateRect(1, 1, 0, 0);
	}


	return rcCell;
}

int CXTPCalendarMonthView::CMonthViewGrid::GetColsCount() const
{
	BOOL bCompressWeD = XTP_SAFE_GET2(m_pView, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);
	int nColsCount = bCompressWeD ? 6 : 7;
	return nColsCount;
}

int CXTPCalendarMonthView::CMonthViewGrid::GetWeekDayCol(int nWeekDayIndex) const
{
	BOOL bCompressWeD = XTP_SAFE_GET2(m_pView, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);
	int nCompressedColIdx = 7 - m_nFirstDayOfWeekIndex;

	int nColIdx = (nWeekDayIndex - m_nFirstDayOfWeekIndex + 7) % 7;

	if (bCompressWeD && nColIdx > nCompressedColIdx)
	{
		nColIdx--;
	}

	return nColIdx;
}

int CXTPCalendarMonthView::CMonthViewGrid::GetWeekDayForColIndex(int nColIdx) const
{
	BOOL bCompressWeD = XTP_SAFE_GET2(m_pView, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);
	int nCompressedColIdx = 7 - m_nFirstDayOfWeekIndex;

	if (bCompressWeD && nColIdx == nCompressedColIdx)
	{
		return 7;
	}

	if (bCompressWeD && nColIdx > nCompressedColIdx)
	{
		nColIdx++;
	}

	int nWeekDayIndex = (m_nFirstDayOfWeekIndex - 1 + nColIdx) % 7 + 1;

	return nWeekDayIndex;
}

int CXTPCalendarMonthView::CMonthViewGrid::GetColHeaderHeight() const
{
	return m_nColHeaderHeight;
}

void CXTPCalendarMonthView::CMonthViewGrid::SetColHeaderHeight(int nHeight)
{
	m_nColHeaderHeight = nHeight;
}

CXTPCalendarMonthViewDay* CXTPCalendarMonthView::CMonthViewGrid::GetViewDay(int nWeekIndex, int nWeekDayIndex)
{
	int nIdx = nWeekIndex * 7 + nWeekDayIndex;
	return XTP_SAFE_GET1(m_pView, GetViewDay(nIdx), NULL);
}

void CXTPCalendarMonthView::CMonthViewGrid::AdjustFirstDayOfWeek()
{
	BOOL bCompressWeD = XTP_SAFE_GET2(m_pView, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);

	int nFDPrev = m_nFirstDayOfWeekIndex;
	//- Adjust First day of Week ------------------------------------------
	m_nFirstDayOfWeekIndex = XTP_SAFE_GET2(m_pView, GetCalendarControl(), GetFirstDayOfWeek(), 2);
	if (bCompressWeD && m_nFirstDayOfWeekIndex == 1)
	{
		m_nFirstDayOfWeekIndex = 2;
	}

	int nDShift = m_nFirstDayOfWeekIndex - nFDPrev;
	COleDateTime dtBeginDate = GetDayDate(0, 0);
	dtBeginDate += COleDateTimeSpan(nDShift);

	SetBeginDate(dtBeginDate);
}

void CXTPCalendarMonthView::CMonthViewGrid::SetBeginDate(COleDateTime dtBeginDate)
{
	CXTPCalendarControl::CViewChangedContext viewChanged(m_pView);

	m_dtBeginDate = ShiftDateToCell_00(dtBeginDate);
}

void CXTPCalendarMonthView::GetWeekDayTextIfNeed(CString* pstrText, int nWeekDay)
{
	ASSERT(nWeekDay >= 1 && nWeekDay <= 7);
	if (!pstrText)
	{
		ASSERT(FALSE);
		return;
	}

	DWORD dwFlags = XTP_SAFE_GET1(GetCalendarControl(), GetAskItemTextFlags(), 0);

	if (dwFlags & xtpCalendarItemText_MonthViewWeekDayHeader)
	{
		XTP_CALENDAR_GETITEMTEXT_PARAMS objRequest;
		::ZeroMemory(&objRequest, sizeof(objRequest));

		objRequest.nItem = xtpCalendarItemText_MonthViewWeekDayHeader;
		objRequest.pstrText = pstrText;
		objRequest.nWeekDay = nWeekDay;

		XTP_SAFE_CALL1(GetCalendarControl(), SendNotificationAlways(XTP_NC_CALENDAR_GETITEMTEXT, (WPARAM)&objRequest, 0));
	}
}

void CXTPCalendarMonthView::CMonthViewGrid::AdjustHeader(CDC* pDC)
{
	CXTPCalendarViewPart* pPart = XTP_SAFE_GET2(m_pView, GetPaintManager(), GetMonthViewHeaderPart(), NULL);
	if (!m_pView || !m_pView->GetCalendarControl() || !pPart || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPFontDC fnt(pDC, &pPart->GetTextFont());

	int nColsCount = max(1, GetColsCount());
	int nCellWidth = m_pView->GetViewRect().Width() / nColsCount;

	//-- Calc header settings -----------------------------------------
	LCTYPE LC_WeekDAYNAME1 = LOCALE_SDAYNAME1;

	int nHWithMax = 0;
	int i;
	for (i = 0; i < 7; i++)
	{
		CString sWD = CXTPCalendarUtils::GetLocaleString(LC_WeekDAYNAME1 + i, 100);
		nHWithMax = max(nHWithMax, pDC->GetTextExtent(sWD).cx);
	}

	int nSpace = pDC->GetTextExtent(_T("Ww"), 2).cx * 2;
	if (nHWithMax + nSpace > nCellWidth)
	{
		LC_WeekDAYNAME1 = LOCALE_SABBREVDAYNAME1;
	}
	//---------------------------------------------------------------------------
	m_arColHeaderText.RemoveAll();
	m_arColHeaderText.SetSize(7);

	CString strHearderTest = _T("/");

	for (i = 0; i < 7; i++)
	{
		CString sWD = CXTPCalendarUtils::GetLocaleString(LC_WeekDAYNAME1 + i, 100);

		int nCol = GetWeekDayCol((i + 2) % 7);

		CString sVal = m_arColHeaderText[nCol];
		if (!sVal.IsEmpty())
		{
			// compressed col - always Sat/Sun
			CString sSat = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVDAYNAME6, 100);
			CString sSun = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVDAYNAME7, 100);

			m_arColHeaderText[nCol] = sSat + _T("/") + sSun;
			strHearderTest += sWD;
		}
		else
		{
			m_arColHeaderText[nCol] += sWD;
			strHearderTest += sWD;
		}
	}

	//-------------------------------------------
	CSize sizeH = pDC->GetTextExtent(strHearderTest);
	m_nColHeaderHeight = max(18, sizeH.cy + 6);

	m_pView->m_Layout.m_nRowHeight = m_nColHeaderHeight;
}

void CXTPCalendarMonthView::CMonthViewGrid::AdjustDays(CDC* pDC)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}

	int nWeeksCount = m_nWeeksCount;
	for (int nWeek = 0; nWeek < nWeeksCount; nWeek++)
	{
		for (int nDayIdx = 0; nDayIdx < 7; nDayIdx++)
		{
			CRect rcDay = GetDayRect(nWeek, nDayIdx, TRUE);

			CXTPCalendarMonthViewDay* pViewDay = GetViewDay(nWeek, nDayIdx);
			//----------------------------------------
			if (pViewDay)
				pViewDay->AdjustLayout(pDC, rcDay);
		}
	}
}

void CXTPCalendarMonthView::CMonthViewGrid::AdjustEvents(CDC* pDC)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}

	BOOL bCompressWeD = XTP_SAFE_GET2(m_pView, GetCalendarControl(), MonthView_IsCompressWeekendDays(), FALSE);
	int nSundayIdx = (7 - m_nFirstDayOfWeekIndex + 1) % 7;

	CXTPCalendarMonthViewDay* pViewDay;

	int nWeeksCount = m_nWeeksCount;
	for (int nWeek = 0; nWeek < nWeeksCount; nWeek++)
	{
		int nDayIdx;

		// Adjust Begin/End for multiday events
		for (nDayIdx = 0; nDayIdx < 7; nDayIdx++)
		{
			// go through all days to look for multiday events
			pViewDay = GetViewDay(nWeek, nDayIdx);
			ASSERT(pViewDay);
			if (!pViewDay)
			{
				continue;
			}

			ASSERT(pViewDay->GetViewGroupsCount() == 1);

			int nEventsCount = 0; //pViewDay->GetViewEventsCount();
			if (pViewDay->GetViewGroupsCount())
			{
				nEventsCount = XTP_SAFE_GET2(pViewDay, GetViewGroup_(0), GetViewEventsCount(), 0);
			}

			for (int nEvent = 0; nEvent < nEventsCount; nEvent++)
			{
				CXTPCalendarMonthViewEvent* pViewEvent = XTP_SAFE_GET2(pViewDay, GetViewGroup(0), GetViewEvent(nEvent), NULL );//pViewDay->GetViewEvent(nEvent);
				ASSERT(pViewEvent);
				if (!pViewEvent)
				{
					continue;
				}

				int nMDEFlags = pViewEvent->GetMultiDayEventFlags();

				if ((nMDEFlags & xtpCalendarMultiDayMSmask) || !(nMDEFlags & xtpCalendarMultiDayFMLmask))
				{
					continue; // already marked or not multiday
				}
				//------------------------
				BOOL bLast = (nMDEFlags & xtpCalendarMultiDayLast) != 0;
				pViewEvent->SetMultiDayEvent_MasterSlave(TRUE, NULL);
				int nMasterPlace = pViewEvent->GetEventPlaceNumber();

				if (!bLast && (nDayIdx != nSundayIdx || !bCompressWeD))
				{
					// find other visible instances and mark them as Slave
					for (int i = nDayIdx + 1; i < 7; i++)
					{
						if (i == nSundayIdx && bCompressWeD)
						{
							break; //continue; // skip compressed sunday
						}

						CXTPCalendarMonthViewDay* pViewDay2 = GetViewDay(nWeek, i);
						CXTPCalendarViewEvent* pViewEvent2 = NULL;

						ASSERT(XTP_SAFE_GET1(pViewDay2, GetViewGroupsCount(), 0) == 1);

						if (XTP_SAFE_GET1(pViewDay2, GetViewGroupsCount(), 0))
						{
							pViewEvent2 = XTP_SAFE_GET2(pViewDay2, GetViewGroup_(0),
								GetViewEventByEvent_(pViewEvent->GetEvent()), NULL);
						}
						if (!pViewEvent2)
						{
							break;
						}
						pViewEvent->AddMultiDayEvent_Slave(pViewEvent2, nMasterPlace);

						if (!pViewEvent2->IsVisible())
						{
							break;
						}
					}
				}
			}
		}
	}

}

void CXTPCalendarMonthView::CMonthViewGrid::AdjustGrid(const CRect& rcRect)
{
	//-------------------------------------------
	int nColsCount = GetColsCount();

	int nWidth = rcRect.Width(); // m_pView->GetViewRect().Width();
	int nHeight = rcRect.Height(); //max(0, m_pView->GetViewRect().Height() - m_nColHeaderHeight);

	m_arRowsTopY.SetSize(m_nWeeksCount + 1);
	m_arColsLeftX.SetSize(nColsCount + 1);

	int nCurrPos = rcRect.top; //m_nColHeaderHeight;
	for (int nWeek = 0; nWeek < m_nWeeksCount; nWeek++)
	{
		m_arRowsTopY[nWeek] = nCurrPos;

		int nHeightRow = nHeight / (m_nWeeksCount - nWeek);
		nCurrPos += nHeightRow;
		nHeight -= nHeightRow;
	}
	m_arRowsTopY[m_nWeeksCount] = nCurrPos;

	nCurrPos = rcRect.left; //0;
	for (int nDayColl = 0; nDayColl < nColsCount; nDayColl++)
	{
		m_arColsLeftX[nDayColl] = nCurrPos;

		int nWidthRow = nWidth / (nColsCount - nDayColl);
		nCurrPos += nWidthRow;
		nWidth -= nWidthRow;
	}
	m_arColsLeftX[nColsCount] = nCurrPos;
}

void CXTPCalendarMonthView::CMonthViewGrid::AdjustLayout(CDC* pDC)
{
	ASSERT(pDC);
	ASSERT(m_pView);
	if (!m_pView || !pDC)
	{
		return;
	}

	AdjustHeader(pDC);

	//------------------------------------------------------------------
	CRect rcGrid = m_pView->GetViewRect();
	rcGrid.top = min(rcGrid.top + m_nColHeaderHeight, rcGrid.bottom);

	AdjustGrid(rcGrid);
	//------------------------------------------------------------------


	//------------------------------------------------------

	AdjustDays(pDC);

	AdjustEvents(pDC);
}

void CXTPCalendarMonthView::CMonthViewGrid::Draw(CDC* pDC)
{
	//-- Draw Header -------------------------------
	int nColsCount = GetColsCount();

	for (int nCol = 0; nCol < nColsCount; nCol++)
	{
		CRect rcCell = GetCelRect(0, nCol, TRUE);
		rcCell.top = m_pView->m_rcView.top;
		rcCell.bottom = rcCell.top + m_nColHeaderHeight;

		CString strWD = m_arColHeaderText[nCol];

		int nWeekDay = GetWeekDayForColIndex(nCol);
		m_pView->GetWeekDayTextIfNeed(&strWD, nWeekDay);

		XTP_SAFE_CALL3(m_pView, GetPaintManager(), GetMonthViewHeaderPart(),
						OnDraw(pDC, m_pView, rcCell, nCol, strWD));
	}

	//-- Draw Grid Lines and fill background -------
	XTP_SAFE_CALL3(m_pView, GetPaintManager(), GetMonthViewGridPart(),
					OnDrawGrid(pDC, m_pView));

}


