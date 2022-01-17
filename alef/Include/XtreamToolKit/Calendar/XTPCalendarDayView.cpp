// XTPCalendarDayView.cpp: implementation of the CXTPCalendarDayView class.
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

#include "XTPCalendarData.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarDayViewTimeScale.h"
#include "XTPCalendarUtils.h"

#include "XTPCalendarPaintManager.h"
#include "XTPCalendarTheme.h"
#include "XTPCalendarThemeOffice2007.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarNotifications.h"

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC50Helpers.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define XTP_SCROLL_TIMER_RESOLUTION_MS  30
#define XTP_REDRAW_TIMER_RESOLUTION_MS  2000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarDayView, CXTPCalendarView)

CXTPCalendarDayView::CXTPCalendarDayView(CXTPCalendarControl* pCalendarControl) :
		TBase(pCalendarControl, xtpCalendarDayView),
		m_ptLastMousePos(0, 0)
{
	m_pTimeScaleHeader = new CXTPCalendarDayViewTimeScale(this);
	m_pTimeScaleHeader2 = new CXTPCalendarDayViewTimeScale(this, FALSE, FALSE);

	m_LayoutX.m_nRowCount = 0;
	m_LayoutX.m_nVisibleRowCount = 0;
	m_LayoutX.m_nTopRow = 0;

	m_LayoutX.m_rcAllDayEvents.SetRectEmpty();
	m_LayoutX.m_rcDayHeader.SetRectEmpty();

	m_LayoutX.m_nAllDayEventsCountMax = 0;
	m_LayoutX.m_nAllDayEventHeight = 0;

	m_nAllDayEventsCountMin_WhenDrag = 0;

	m_spDraggingStartOffset_Time = 0.;

	m_nScrollOffsetX = 0;
	m_nMinColumnWidth = -1; // 0 - means disabled; -1 use auto for multiresources; -X use for multiresources only; +X used always (in single and multi resources mode).

	// add today to the day view by default
	COleDateTime dtNow(CXTPCalendarUtils::GetCurrentTime());
	m_arDays.Add(new CXTPCalendarDayViewDay(this, dtNow));

	// go to the beginning of the work day
	int nHour = 0, nMin = 0, nSec = 0;
	XTP_SAFE_CALL1(m_pControl, GetWorkDayStartTime(nHour, nMin, nSec));
	int nPos = GetCellNumber(nHour, nMin, nSec, FALSE);
	_ScrollV(nPos, nPos);

	m_dwScrollingEventTimerID = 0;
	m_bMouseOutOfDragArea = FALSE;
	m_dtDraggingStartPoint = COleDateTime(0, 0, 0, 0, 0, 0);

	m_dwRedrawNowLineTimerID = 0;
	m_dtLastRedrawTime = CXTPCalendarUtils::GetCurrentTime();

	m_ptLBtnDownMousePos = CPoint(0,0);
}

CXTPCalendarDayView::~CXTPCalendarDayView()
{
	CMDTARGET_RELEASE(m_pTimeScaleHeader);
	CMDTARGET_RELEASE(m_pTimeScaleHeader2);

	ClearDays();
}

XTPCalendarViewType CXTPCalendarDayView::GetViewType()
{
	int nWWMask = m_pControl ? m_pControl->GetWorkWeekMask() : xtpCalendarDayMo_Fr;
	int nDaysCount = GetViewDayCount();

	int nWWCount = CXTPCalendarUtils::GetDayOfWeekCount(nWWMask);
	if (nDaysCount != nWWCount)
	{
		return xtpCalendarDayView;
	}

	for (int i = 0; i < nDaysCount; i++)
	{
		COleDateTime dtDay = GetViewDayDate(i);
		int nDayOfWeek = dtDay.GetDayOfWeek();
		int nDayMask = CXTPCalendarUtils::GetDayOfWeekMask(nDayOfWeek);

		if ((nWWMask & nDayMask) == 0)
		{
			return xtpCalendarDayView;
		}
	}

	return xtpCalendarWorkWeekView;
}

void CXTPCalendarDayView::ClearDays()
{
	m_arDays.RemoveAll();
}

COleDateTime CXTPCalendarDayView::GetCellTime(int nCell) const
{
	double dblTime = (long)nCell * (double)GetCellDuration();

	return (DATE)dblTime;
}

COleDateTimeSpan CXTPCalendarDayView::GetCellDuration() const
{
	return m_pTimeScaleHeader ? m_pTimeScaleHeader->GetScaleInterval() : (double)0;
}

int CXTPCalendarDayView::GetCellNumber(COleDateTime dtTime, BOOL bForEndTime) const
{
	double dblTime = (double)CXTPCalendarUtils::ResetDate(dtTime);

	double dblEpsilon = XTP_HALF_SECOND * (bForEndTime ? -1 : 1);
	double dblCellDur = (double)GetCellDuration();
	if (dblCellDur <= XTP_HALF_SECOND)
	{
		dblCellDur = XTP_HALF_SECOND * 2 * 60 * 30;
	}
	int nCell = int(dblTime / dblCellDur + dblEpsilon);
	return nCell;
}

int CXTPCalendarDayView::GetCellNumber(int nHour, int nMin, int nSec, BOOL bForEndTime) const
{
	COleDateTime dtTime;
	dtTime.SetTime(nHour, nMin, nSec);
	return GetCellNumber(dtTime, bForEndTime);
}

void CXTPCalendarDayView::ScrollV(int nIndex, int nPos_raw)
{
	if (m_bScrollV_Disabled)
	{
		return;
	}
	_ScrollV(nIndex, nPos_raw);
}

void CXTPCalendarDayView::_ScrollV(int nIndex, int /*nPos_raw*/)
{
	if (nIndex + m_LayoutX.m_nVisibleRowCount >= m_LayoutX.m_nRowCount)
	{
		nIndex = m_LayoutX.m_nRowCount - m_LayoutX.m_nVisibleRowCount;
	}

	if (nIndex == m_LayoutX.m_nTopRow)
		return;

	m_LayoutX.m_nTopRow = nIndex;
}

void CXTPCalendarDayView::ScrollH(int nPos, int nPos_raw)
{
	SCROLLINFO si;
	if (!GetScrollBarInfoH(&si))
	{
		m_nScrollOffsetX = 0;
	}

	int nXmax = si.nMax - si.nPage + 1;

	m_nScrollOffsetX = min(max(nPos, si.nMin), nXmax);

	if (!m_bScrollH_Disabled)
	{
		if (nPos_raw < 0)
		{
			ScrollDaysToPrev();
		}
		else if (nPos_raw > nXmax)
		{
			ScrollDaysToNext();
		}
	}
}

BOOL CXTPCalendarDayView::GetScrollBarInfoV(SCROLLINFO* pSI)
{
	ASSERT(pSI);
	if (!pSI || m_bScrollV_Disabled)
	{
		return FALSE;
	}

	pSI->nPos = m_LayoutX.m_nTopRow;
	pSI->nMax = m_LayoutX.m_nRowCount - 1;
	pSI->nPage = m_LayoutX.m_nVisibleRowCount;

	return m_LayoutX.m_nVisibleRowCount != m_LayoutX.m_nRowCount;
}

BOOL CXTPCalendarDayView::GetScrollBarInfoH(SCROLLINFO* pSI, int* pnScrollStep)
{
	ASSERT(pSI);
	if (!pSI)
		return FALSE;

	int nMinColumnWidth = RecalcMinColumnWidth();
	if (nMinColumnWidth <= 0)
		return FALSE;
	int nColumnsCount = GetTotalGroupsCount();

	CRect rcClient = _GetScrollRectClient();

	int nFullWidth = nColumnsCount * nMinColumnWidth;

	if (nFullWidth <= rcClient.Width())
		return FALSE;

	pSI->nPos = min(m_nScrollOffsetX, nFullWidth);
	pSI->nMin = 0;
	pSI->nMax = nFullWidth-1;
	pSI->nPage = rcClient.Width();

	if (pnScrollStep)
		*pnScrollStep = 15;

	return TRUE;
}

int CXTPCalendarDayView::GetTotalGroupsCount()
{
	int nGroupsCount = 0;

	int nDays = GetViewDayCount();
	for (int i = 0; i < nDays; i++)
	{
		CXTPCalendarDayViewDay* pDay = GetViewDay(i);
		ASSERT(pDay);
		if (pDay)
			nGroupsCount += pDay->GetViewGroupsCount();
	}
	return nGroupsCount;
}

int CXTPCalendarDayView::RecalcMinColumnWidth()
{
	int nMinColumnWidth = m_nMinColumnWidth;
	if (nMinColumnWidth < 0)
	{
		// automatically use horizontal scrolling for multi-resources mode
		if (GetResources() && GetResources()->GetCount() <= 1)
			return -1;

		if (nMinColumnWidth == -1)
			nMinColumnWidth = ::GetSystemMetrics(SM_CXFULLSCREEN) / 6;
		else
			nMinColumnWidth = abs(nMinColumnWidth);
	}
	return nMinColumnWidth;
}

int CXTPCalendarDayView::GetAllDayEventsMaxCount()
{
	int nAllDayMax = 0;
	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewDay* pViewDay = GetViewDay(i);
		ASSERT(pViewDay);
		int nGroupsCount = pViewDay->GetViewGroupsCount();
		for (int g = 0; g < nGroupsCount; g++)
		{
			int nAllDay = 0;

			CXTPCalendarDayViewGroup* pViewGroup = pViewDay->GetViewGroup(g);
			ASSERT(pViewGroup);
			if (!pViewGroup)
			{
				continue;
			}
			int nECount = pViewGroup->GetViewEventsCount();
			for (int j = 0; j < nECount; j++)
			{
				CXTPCalendarDayViewEvent* pViewEvent = pViewGroup->GetViewEvent(j);
				ASSERT(pViewEvent);
				if (pViewEvent && pViewEvent->IsMultidayEvent())
				{
					nAllDay++;
				}
			}

			nAllDayMax = max(nAllDayMax, nAllDay);
		}
	}
	return nAllDayMax;
}

void CXTPCalendarDayView::AdjustAllDayEvents()
{
	if (GetResources() && GetResources()->GetCount() > 1)
	{
		return;
	}

	int nDaysCount = GetViewDayCount();
	for (int nDayIdx = 0; nDayIdx < nDaysCount; nDayIdx++)
	{
		CXTPCalendarDayViewDay* pViewDay = GetViewDay(nDayIdx);
		if (!pViewDay)
		{
			ASSERT(FALSE);
			continue;
		}
		// go through all days to look for multiday events
		int nGroupsCount = pViewDay->GetViewGroupsCount();
		for (int g = 0; g < nGroupsCount; g++)
		{
			CXTPCalendarDayViewGroup* pViewGroup = pViewDay->GetViewGroup(g);
			ASSERT(pViewGroup);
			if (!pViewGroup)
			{
				continue;
			}
			int nEventsCount = pViewGroup->GetViewEventsCount();
			for (int nEvent = 0; nEvent < nEventsCount; nEvent++)
			{
				CXTPCalendarViewEvent* pViewEvent = pViewGroup->GetViewEvent(nEvent);
				if (!pViewEvent)
				{
					ASSERT(FALSE);
					continue;
				}

				int nMDEFlags = pViewEvent->GetMultiDayEventFlags();

				if ((nMDEFlags & xtpCalendarMultiDayMSmask) || !(nMDEFlags & xtpCalendarMultiDayFMLmask))
				{
					continue; // already marked or not multi day
				}
				//------------------------

				pViewEvent->SetMultiDayEvent_MasterSlave(TRUE, NULL);
				int nMasterPlace = pViewEvent->GetEventPlaceNumber();

				if ((nMDEFlags & xtpCalendarMultiDayLast) == 0)
				{
					// find other visible instances and mark them as Slave
					for (int i = nDayIdx + 1; i < nDaysCount; i++)
					{
						CXTPCalendarViewDay* pViewDay2 = GetViewDay(i);
						if (XTP_SAFE_GET1(pViewDay2, GetViewGroupsCount(), 0) != 1)
						{
							ASSERT(FALSE);
							break;
						}
						CXTPCalendarViewEvent* pViewEvent2 = XTP_SAFE_GET2(pViewDay2,
							GetViewGroup_(0), GetViewEventByEvent_(pViewEvent->GetEvent()), NULL);

						if (!pViewEvent2)
						{
							break;
						}
						pViewEvent->AddMultiDayEvent_Slave(pViewEvent2, nMasterPlace);
					}
				}
			}
		}

	}
}

int CXTPCalendarDayView::CalculateHeaderFormatAndHeight(CDC* pDC, int nCellWidth)
{
	ASSERT(pDC);

	const int cnMinHeight = 19;

	CXTPCalendarViewPart* pPart = XTP_SAFE_GET1(GetPaintManager(), GetDayViewHeaderPart(), NULL);
	ASSERT(pPart);
	if (!pPart || !pDC)
	{
		return cnMinHeight;
	}

	CalculateHeaderFormat(pDC, nCellWidth, pPart);

	CString strHearderTest = _T("QW(");

	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarDayViewDay* pDay = GetViewDay(i);
		ASSERT(pDay);
		if (pDay)
		{
			strHearderTest += pDay->GetCaption();
		}
	}

	CXTPFontDC fnt(pDC, &pPart->GetTextFont());

	CSize sizeH = pDC->GetTextExtent(strHearderTest);
	int nHeight = max(cnMinHeight, sizeH.cy + 6);

	return nHeight;
}

CXTPCalendarData* CXTPCalendarDayView::_GetDataProviderByConnStr(LPCTSTR pcszConnStr, BOOL bCompareNoCase)
{
	UNREFERENCED_PARAMETER(pcszConnStr);
	UNREFERENCED_PARAMETER(bCompareNoCase);

	return NULL;
}

BOOL CXTPCalendarDayView::IsGroupHeaderVisible()
{
	BOOL bHideHeader = XTP_SAFE_GET1(GetResources(), GetCount(), 0) == 1 &&
			XTP_SAFE_GET2(GetResources(), GetAt(0), IsSchedulesSetEmpty(), FALSE);
	return !bHideHeader;
}

void CXTPCalendarDayView::AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	ASSERT(pDC);
	if (!pDC)
	{
		return;
	}

	XTP_SAFE_CALL1(m_pTimeScaleHeader, SetCaption(GetScaleText()) );
	XTP_SAFE_CALL1(m_pTimeScaleHeader2, SetCaption(GetScale2Text()) );
	XTP_SAFE_CALL1(m_pTimeScaleHeader2, SetVisible(IsScale2Visible()) );

	TBase::AdjustLayout(pDC, rcView, FALSE);

	COleDateTimeSpan spDay(1, 0, 0, 0);

	double dRows = spDay / max((double)m_pTimeScaleHeader->GetScaleInterval(), XTP_HALF_SECOND * 2);
	m_LayoutX.m_nRowCount = (int)(dRows + XTP_HALF_SECOND); // to prevent loosing 1 :  (int)(47.99999999999) = 47; // need 48 !

	//CString str; str.Format(_T("RowsCount[%d] = %.15e (%.15e)"), m_nRowCount, dRows, (dRows + XTP_HALF_SECOND));
	//AfxMessageBox(str);

	CRect rcView2 = rcView;
	// alternative time scale
	if (IsScale2Visible())
	{
		int nTimeScale2Width = XTP_SAFE_GET1(m_pTimeScaleHeader2, CalcWidth(pDC), 0);

		m_pTimeScaleHeader2->m_Layout.m_rcHeader.SetRect(rcView2.left, rcView2.top, rcView2.left + nTimeScale2Width, rcView2.bottom);
		rcView2.left += nTimeScale2Width;

		AdjustScale2TimeZone();
	}

	// regular time scale
	if (m_pTimeScaleHeader)
	{
		int nTimeScaleWidth = XTP_SAFE_GET1(m_pTimeScaleHeader, CalcWidth(pDC), 0);
		m_pTimeScaleHeader->m_Layout.m_rcHeader.SetRect(rcView2.left, rcView2.top, rcView2.left + nTimeScaleWidth, rcView2.bottom);
		rcView2.left += nTimeScaleWidth;
	}

	CXTPCalendarViewPart* pPart = XTP_SAFE_GET1(GetPaintManager(), GetDayViewHeaderPart(), NULL);
	int nDaysCount = GetViewDayCount();
	if (nDaysCount <= 0 || !pPart)
	{
		return;
	}

	int nColumnsCount = GetTotalGroupsCount();

	// Adjust Header
	int nCellWidth = rcView2.Width() / max(nDaysCount, 1);
	int nDayHeaderHeight = CalculateHeaderFormatAndHeight(pDC, nCellWidth);

	int nMinRowHeight = nDayHeaderHeight;
	m_LayoutX.m_nAllDayEventsCountMax = max(1, GetAllDayEventsMaxCount());

	if (m_eDraggingMode == xtpCalendaDragModeResizeBegin || m_eDraggingMode == xtpCalendaDragModeResizeEnd
		|| _IsDragModeCopyMove(m_eDraggingMode))
	{
		m_LayoutX.m_nAllDayEventsCountMax = max(m_LayoutX.m_nAllDayEventsCountMax, m_nAllDayEventsCountMin_WhenDrag);
		m_nAllDayEventsCountMin_WhenDrag = m_LayoutX.m_nAllDayEventsCountMax;
	}

	BOOL bGroupHeaderVisible = IsGroupHeaderVisible();
	int nDGHeadersHeight = nDayHeaderHeight * (bGroupHeaderVisible ? 2 : 1);

	// day view part
	int nRowCount = m_LayoutX.m_nRowCount;
	int nTotalRows = nRowCount + m_LayoutX.m_nAllDayEventsCountMax + (bGroupHeaderVisible ? 2 : 1);

	m_Layout.m_nRowHeight = max(nMinRowHeight, rcView2.Height() / max(nTotalRows, 1));

	double dVisibleRowCountMax = (double)(rcView2.Height()) / (double)(max(m_Layout.m_nRowHeight, 1));
	double dVisibleRowCount = dVisibleRowCountMax - (0.2 + m_LayoutX.m_nAllDayEventsCountMax) -
								(bGroupHeaderVisible ? 2 : 1);

	m_LayoutX.m_nVisibleRowCount = max((int)dVisibleRowCount, max(0, (int)dVisibleRowCountMax/2));
	m_LayoutX.m_nVisibleRowCount = max(0, min(nRowCount, m_LayoutX.m_nVisibleRowCount));

	if (m_LayoutX.m_nTopRow + m_LayoutX.m_nVisibleRowCount >= m_LayoutX.m_nRowCount)
	{
		m_LayoutX.m_nTopRow = m_LayoutX.m_nRowCount - m_LayoutX.m_nVisibleRowCount;
	}

	int nAllDayEventsHeight = max(0, rcView2.Height() - (m_LayoutX.m_nVisibleRowCount * m_Layout.m_nRowHeight + nDGHeadersHeight));

	m_LayoutX.m_rcDayHeader.SetRect(rcView2.left, rcView2.top, rcView2.right, rcView2.top + nDayHeaderHeight);

	m_LayoutX.m_rcAllDayEvents.SetRect(rcView2.left, rcView2.top + nDGHeadersHeight,
							 rcView2.right, rcView2.top + nAllDayEventsHeight + nDGHeadersHeight);

	int nCols = 0;
	for (int i = 0; i < nDaysCount; i++)
	{
		CXTPCalendarDayViewDay* pDay = GetViewDay(i);
		if (!pDay)
		{
			ASSERT(FALSE);
			continue;
		}

		//int nWidth = (i == nDaysCount - 1) ? rcView2.Width() : rcView2.Width() / (nColumnsCount - nCols);
		int nWidth = (rcView2.Width() / (nColumnsCount - nCols)) * pDay->GetViewGroupsCount();
		if (i == nDaysCount - 1)
			nWidth = rcView2.Width();


		CRect rcDay(rcView2.left, rcView2.top, rcView2.left + nWidth, rcView2.bottom);
		pDay->AdjustLayout(pDC, rcDay);

		rcView2.left += nWidth;
		nCols += pDay->GetViewGroupsCount();
	}

	//----------------------------------------------
	if (nDaysCount && XTP_SAFE_GET1(GetViewDay(0), GetViewGroupsCount(), 0) &&
		GetViewDay(0)->GetViewGroup(0) )
	{
		CRect rcAllDayEvents = GetViewDay(0)->GetViewGroup(0)->GetAllDayEventsRect();
		ASSERT(rcAllDayEvents.Height() == m_LayoutX.m_rcAllDayEvents.Height());

		m_LayoutX.m_rcAllDayEvents.top = rcAllDayEvents.top;
		m_LayoutX.m_rcAllDayEvents.bottom = rcAllDayEvents.bottom;
	}

	AdjustAllDayEvents();

	//----------------------------------------------------------
	XTP_SAFE_CALL1(m_pTimeScaleHeader, AdjustLayout(pDC) );
	XTP_SAFE_CALL1(m_pTimeScaleHeader2, AdjustLayout(pDC) );

	//---------------------------------------------------------------------------
	int nDVCount = GetViewDayCount();
	if (nDVCount)
	{
		COleDateTime dtFirstDay = GetViewDayDate(0);
		COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);
		BOOL bSelChanged = FALSE;

		if (m_selectedBlock.dtEnd.GetStatus() != COleDateTime::valid ||
				m_selectedBlock.dtBegin.GetStatus() != COleDateTime::valid)
		{
			int nSelRow = GetTopRow() + min(2, GetVisibleRowCount());

			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateTime(dtFirstDay, GetCellTime(nSelRow));
			m_selectedBlock.dtEnd = m_selectedBlock.dtBegin + GetCellDuration();
			m_selectedBlock.bAllDayEvent = FALSE;
			m_selectedBlock.nGroupIndex = 0;

			bSelChanged = TRUE;
		}

		COleDateTime dtSelBeginDay = CXTPCalendarUtils::ResetTime(m_selectedBlock.dtBegin);
		COleDateTime dtSelEndDay = CXTPCalendarUtils::ResetTime(m_selectedBlock.dtEnd-COleDateTimeSpan(0, 0, 0, 1));

		// compare with the first day
		if (dtSelBeginDay < dtFirstDay)
		{
			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtBegin, dtFirstDay);
			bSelChanged = TRUE;
		}
		if (dtSelEndDay < dtFirstDay)
		{
			m_selectedBlock.dtEnd = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtEnd, dtFirstDay);
			bSelChanged = TRUE;
		}
		// compare with the last day
		if (dtSelEndDay > dtLastDay)
		{
			m_selectedBlock.dtEnd = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtEnd, dtLastDay);
			bSelChanged = TRUE;
		}
		if (dtSelBeginDay > dtLastDay)
		{
			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtBegin, dtLastDay);
			bSelChanged = TRUE;
		}

		if (bSelChanged)
		{
			CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
		}
	}
	//---------------------------------------------------------------------------
	if (bCallPostAdjustLayout)
	{
		OnPostAdjustLayout();
	}
}

/*
	AdjustLayout2
		<Pre-Theme Adjust Layout step>
		Theme->AdjustLayout
				   <Adjust Layout for theme step>
				<- <call AdjustLayout2 for children>

		<After-Theme Adjust Layout step>

		OnPostAdjustLayout2
			<Pre-Theme Post Adjust Layout step>
			Theme->OnPostAdjustLayout

			<After-Theme Post Adjust Layout step>
*/

void CXTPCalendarDayView::AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	if (!pDC || !GetTheme())
	{
		ASSERT(FALSE);
		return;
	}
	TBase::AdjustLayout2(pDC, rcView, bCallPostAdjustLayout);

	//*** Pre-Theme Adjust Layout step
	XTP_SAFE_CALL1(m_pTimeScaleHeader, SetCaption(GetScaleText()) );
	XTP_SAFE_CALL1(m_pTimeScaleHeader2, SetCaption(GetScale2Text()) );
	XTP_SAFE_CALL1(m_pTimeScaleHeader2, SetVisible(IsScale2Visible()) );

	if (IsScale2Visible())
	{
		AdjustScale2TimeZone();
	}

	COleDateTimeSpan spDay(1, 0, 0, 0);

	double dRows = spDay / max((double)m_pTimeScaleHeader->GetScaleInterval(), XTP_HALF_SECOND * 2);
	m_LayoutX.m_nRowCount = (int)(dRows + XTP_HALF_SECOND); // to prevent loosing 1 :  (int)(47.99999999999) = 47; // need 48 !

	m_LayoutX.m_nAllDayEventsCountMax = max(1, GetAllDayEventsMaxCount());

	if (m_eDraggingMode == xtpCalendaDragModeResizeBegin || m_eDraggingMode == xtpCalendaDragModeResizeEnd
		|| _IsDragModeCopyMove(m_eDraggingMode))
	{
		m_LayoutX.m_nAllDayEventsCountMax = max(m_LayoutX.m_nAllDayEventsCountMax, m_nAllDayEventsCountMin_WhenDrag);
		m_nAllDayEventsCountMin_WhenDrag = m_LayoutX.m_nAllDayEventsCountMax;
	}


	//***
	GetTheme()->GetDayViewPart()->AdjustLayout(pDC, rcView);

	//*** Post-Theme Adjust Layout step
	//---------------------------------------------------------------------------
	int nDVCount = GetViewDayCount();
	if (nDVCount)
	{
		COleDateTime dtFirstDay = GetViewDayDate(0);
		COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);
		BOOL bSelChanged = FALSE;

		if (m_selectedBlock.dtEnd.GetStatus() != COleDateTime::valid ||
			m_selectedBlock.dtBegin.GetStatus() != COleDateTime::valid)
		{
			int nSelRow = GetTopRow() + min(2, GetVisibleRowCount());

			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateTime(dtFirstDay, GetCellTime(nSelRow));
			m_selectedBlock.dtEnd = m_selectedBlock.dtBegin + GetCellDuration();
			m_selectedBlock.bAllDayEvent = FALSE;
			m_selectedBlock.nGroupIndex = 0;

			bSelChanged = TRUE;
		}

		COleDateTime dtSelBeginDay = CXTPCalendarUtils::ResetTime(m_selectedBlock.dtBegin);
		COleDateTime dtSelEndDay = CXTPCalendarUtils::ResetTime(m_selectedBlock.dtEnd-COleDateTimeSpan(0, 0, 0, 1));

		// compare with the first day
		if (dtSelBeginDay < dtFirstDay)
		{
			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtBegin, dtFirstDay);
			bSelChanged = TRUE;
		}
		if (dtSelEndDay < dtFirstDay)
		{
			m_selectedBlock.dtEnd = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtEnd, dtFirstDay);
			bSelChanged = TRUE;
		}
		// compare with the last day
		if (dtSelEndDay > dtLastDay)
		{
			m_selectedBlock.dtEnd = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtEnd, dtLastDay);
			bSelChanged = TRUE;
		}
		if (dtSelBeginDay > dtLastDay)
		{
			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateDate(m_selectedBlock.dtBegin, dtLastDay);
			bSelChanged = TRUE;
		}

		if (bSelChanged)
		{
			CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
		}
	}
	//---------------------------------------------------------------------------
	if (bCallPostAdjustLayout)
	{
		OnPostAdjustLayout();

		GetTheme()->GetDayViewPart()->OnPostAdjustLayout();
	}
}

void CXTPCalendarDayView::Draw2(CDC* pDC)
{
	if (!GetTheme())
	{
		ASSERT(FALSE);
		return;
	}

	GetTheme()->GetDayViewPart()->Draw(pDC);

	m_dtLastRedrawTime = CXTPCalendarUtils::GetCurrentTime();
}

void CXTPCalendarDayView::OnDraw(CDC* pDC)
{
	SCROLLINFO si;
	if (!GetScrollBarInfoH(&si) || pDC->IsPrinting())
	{
		TBase::OnDraw(pDC);
		return;
	}

	//===================================
	CRect rcView = GetViewRect();

	CDC memDC;
	CBitmap bmpCache;
	memDC.CreateCompatibleDC(pDC);

	VERIFY( bmpCache.CreateCompatibleBitmap(pDC, rcView.Width(), rcView.Height()) );

	CXTPBitmapDC autoDCbmp(&memDC, &bmpCache);
	memDC.FillSolidRect(rcView, 0xFF);

	memDC.SetViewportOrg(m_nScrollOffsetX, 0);

	//------------------------------------------------
	if (m_pTimeScaleHeader)
		m_pTimeScaleHeader->MoveTo(m_nScrollOffsetX);

	if (m_pTimeScaleHeader2 && IsScale2Visible())
		m_pTimeScaleHeader2->MoveTo(m_nScrollOffsetX);

	//************************************************
	TBase::OnDraw(&memDC);
	//************************************************

	//int nTSWidth = _GetTimeScaleWith();
	CXTPClientRect rcClient(GetCalendarControl());

	pDC->BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
				&memDC, 0, 0, SRCCOPY);

}

void CXTPCalendarDayView::Draw(CDC* pDC)
{
	pDC->SetBkMode(TRANSPARENT);

	// days
	int nCount = GetViewDayCount();
	for (int i = nCount - 1; i >= 0; i--)
	{
		XTP_SAFE_CALL1(GetViewDay(i), Draw(pDC));
	}

	// alternative time scale
	if (IsScale2Visible() && m_pTimeScaleHeader2)
		m_pTimeScaleHeader2->Draw(pDC);

	// regular time scale
	XTP_SAFE_CALL1(m_pTimeScaleHeader, Draw(pDC));

	m_dtLastRedrawTime = CXTPCalendarUtils::GetCurrentTime();
}

BOOL CXTPCalendarDayView::HitTestEx(CPoint pt, XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitTest)
{
	if (!pHitTest)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pHitTest->pt = pt;

	if (m_pTimeScaleHeader2 && m_pTimeScaleHeader2->HitTestDateTime(pHitTest))
	{
		return TRUE;
	}

	if (m_pTimeScaleHeader && m_pTimeScaleHeader->HitTestDateTime(pHitTest))
	{
		return TRUE;
	}

	return TBase::HitTestEx(pt, pHitTest);
}

BOOL CXTPCalendarDayView::IsUseCellAlignedDraggingInTimeArea()
{
	if (GetTheme() && GetTheme()->GetDayViewPart())
	{
		return GetTheme()->GetDayViewPart()->IsUseCellAlignedDraggingInTimeArea();
	}
	return CXTPCalendarView::IsUseCellAlignedDraggingInTimeArea();
}

void CXTPCalendarDayView::OnStartDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* pHitTest)
{
	if (!pHitTest || !m_pControl)
	{
		ASSERT(FALSE);
		return;
	}

	m_dtDraggingStartPoint = pHitTest->dt;
	m_spDraggingStartOffset_Time = 0.;

	m_nAllDayEventsCountMin_WhenDrag = max(1, GetAllDayEventsMaxCount());

	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

	TBase::OnStartDragging(point, pHitTest);

	if (double(m_spDraggingStartOffset) != 0.0
		&& IsUseCellAlignedDraggingInTimeArea())
	{
		// Align to cell begin time
		m_spDraggingStartOffset = pHitTest->dt - pHitTest->pViewEvent->GetEvent()->GetStartTime();
		int nTmpCell = GetCellNumber((DATE)(double)m_spDraggingStartOffset, FALSE);
		m_spDraggingStartOffset = (double)GetCellTime(nTmpCell);
	}

	m_selectedBlock.dtEnd = m_selectedBlock.dtBegin;
	m_selectedBlock.bAllDayEvent = FALSE;
	m_selectedBlock.nGroupIndex = pHitTest->nGroup;

	CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
}

BOOL CXTPCalendarDayView::OnDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* pHitTest)
{
	if (!pHitTest)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!m_ptrDraggingEventNew || m_eDraggingMode == xtpCalendaDragModeEditSubject ||
		!GetCalendarControl())
	{
		return FALSE;
	}

	TRACE_DRAGGING(_T("OnDragging - [CXTPCalendarDayView] (point.x =%d, point.y =%d) \n"),
		point.x, point.y);

	XTP_CALENDAR_HITTESTINFO hitInfo2 = *pHitTest;
	XTP_CALENDAR_HITTESTINFO* pHitTest2 = &hitInfo2;

	BOOL bChanged = FALSE;

	XTP_CALENDAR_HITTESTINFO_DAY_VIEW hitTestInfo;

	 // TODO:
	//if (IsOutOfDraggingRect(point, &hitTestInfo))
	//{
	//  return FALSE;
	//}

	if (!HitTestEx(point, &hitTestInfo))
	{
		return FALSE;
	}

	//========================================================================
	BOOL bScrolled = FALSE;
	XTPCalendarDayViewScrollDirection scrollDir = GetNeededScrollDirection(point);

	if (scrollDir != xtpCalendarDayViewScrollNotNeeded)
	{
		if (scrollDir == xtpCalendarDayViewScrollUp)
		{
			TRACE_DRAGGING(_T("OnDragging::ScrollV up\n"));
			bScrolled = VertEventScroll(TRUE);//scroll up
			SetMouseOutOfDragArea(FALSE);

			if (bScrolled)
			{
				if (HitTestEx(point, &hitTestInfo))
				{
					pHitTest2->dt = hitTestInfo.dt;
				}
			}
		}
		else if (scrollDir == xtpCalendarDayViewScrollDown)
		{
			TRACE_DRAGGING(_T("OnDragging::ScrollV down\n"));
			bScrolled = VertEventScroll(FALSE);//scroll down
			SetMouseOutOfDragArea(FALSE);

			if (bScrolled)
			{
				if (HitTestEx(point, &hitTestInfo))
				{
					pHitTest2->dt = hitTestInfo.dt;
				}
			}
		}
	}

//  if (IsOutOfDraggingRect(point, &hitTestInfo))
//  {
//      TRACE_DRAGGING("OnDragging::OutOfDragArea\n");
//          SetMouseOutOfDragArea(TRUE);
//          StopVertEventScroll();
//          bScrolled = TRUE;
//          return TRUE;
//  }

	// TODO:
	if (IsOutOfClientRect(point, &hitTestInfo))
	{
		TRACE_DRAGGING(_T("OnDragging::OutOfDragArea\n"));
			SetMouseOutOfDragArea(TRUE);
			StopVertEventScroll();
			bScrolled = TRUE;
			return TRUE;
		}

	if (!bScrolled)
	{
		TRACE_DRAGGING(_T("OnDragging::In DragArea\n"));
		SetMouseOutOfDragArea(FALSE);
		StopVertEventScroll();
	}

	// check drag on TimeScale:

	TRACE_DRAGGING(_T("OnDragging - [CXTPCalendarDayView] (hour =%d, minute =%d) \n"),
					pHitTest->dt.GetHour(), pHitTest->dt.GetMinute());

	if (_IsDragModeCopyMove(m_eDraggingMode) && (
		m_pTimeScaleHeader2 && m_pTimeScaleHeader2->HitTestDateTime(&hitTestInfo) ||
		(m_pTimeScaleHeader && m_pTimeScaleHeader->HitTestDateTime(&hitTestInfo))))
	{
		COleDateTime dtNewTime = pHitTest->dt;
		dtNewTime = COleDateTime(m_dtDraggingStartPoint.GetYear(),
			m_dtDraggingStartPoint.GetMonth(), m_dtDraggingStartPoint.GetDay(),
			dtNewTime.GetHour(), dtNewTime.GetMinute(), dtNewTime.GetSecond());

		m_ptrDraggingEventNew->MoveEvent(dtNewTime);

		return TRUE;
	}

	if (!m_ptrDraggingEventOrig)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bAllDayEventArea = 0 != (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewAllDayEvent);

	BOOL bAllDayEventNew_prev = m_ptrDraggingEventNew->IsAllDayEvent();
	BOOL bAllMultiDayEvent = bAllDayEventNew_prev || m_ptrDraggingEventNew->GetDurationMinutes() > 24 * 60;

	BOOL bAllMultiDayEventOrig = m_ptrDraggingEventOrig->IsAllDayEvent();
	bAllMultiDayEventOrig |= m_ptrDraggingEventOrig->GetDurationMinutes() > 24 * 60;

	bChanged = bAllDayEventArea != bAllMultiDayEvent;

	if (bChanged)
	{
		if (!bAllDayEventArea && bAllMultiDayEventOrig && _IsDragModeCopyMove(m_eDraggingMode))
		{
			m_ptrDraggingEventNew->SetDuration(GetCellDuration());
		}
		else if (bAllDayEventArea && !bAllMultiDayEventOrig && _IsDragModeCopyMove(m_eDraggingMode))
		{
			m_ptrDraggingEventNew->SetStartTime(CXTPCalendarUtils::ResetTime(m_ptrDraggingEventOrig->GetStartTime()));
			m_ptrDraggingEventNew->SetEndTime(m_ptrDraggingEventNew->GetStartTime()+COleDateTimeSpan(1, 0, 0, 0));
		}
		else
		{
			m_ptrDraggingEventNew->SetStartTime(m_ptrDraggingEventOrig->GetStartTime());
			m_ptrDraggingEventNew->SetEndTime(m_ptrDraggingEventOrig->GetEndTime());
		}

		if (_IsDragModeCopyMove(m_eDraggingMode))
		{
			if (bAllDayEventArea == bAllMultiDayEventOrig)
			{
				m_spDraggingStartOffset = m_spDraggingStartOffset_Time;
				m_spDraggingStartOffset_Time = 0.;
			}
			else
			{
				m_spDraggingStartOffset_Time = m_spDraggingStartOffset;
				m_spDraggingStartOffset = 0.;
			}

			if (bAllDayEventArea)
			{
				if (GetCalendarControl()->DayView_IsAutoResetBusyFlag())
				{
					if (!bAllDayEventNew_prev &&
						xtpCalendarBusyStatusBusy == m_ptrDraggingEventNew->GetBusyStatus()
						&&
						!(bAllMultiDayEventOrig &&
						xtpCalendarBusyStatusBusy == m_ptrDraggingEventOrig->GetBusyStatus()))
					{
						m_ptrDraggingEventNew->SetBusyStatus(xtpCalendarBusyStatusFree);
					}
				}
			}
			else
			{
				if (GetCalendarControl()->DayView_IsAutoResetBusyFlag())
				{
					if (bAllMultiDayEvent &&
						xtpCalendarBusyStatusFree == m_ptrDraggingEventNew->GetBusyStatus()
						&&
						!(!bAllMultiDayEventOrig &&
						xtpCalendarBusyStatusFree == m_ptrDraggingEventOrig->GetBusyStatus()))
					{
						m_ptrDraggingEventNew->SetBusyStatus(xtpCalendarBusyStatusBusy);
					}
				}
			}
		}
	}

	//========================================================================
	if (m_eDraggingMode == xtpCalendaDragModeResizeEnd)
	{
		if (pHitTest2->bTimePartValid)
		{
			pHitTest2->dt += GetCellDuration();
		}
	}

	BOOL bChangedBase = TBase::OnDragging(point, pHitTest2);

	//-------------------------------------------------------------------------
	// multi-resources movement

	if (_IsDragModeCopyMove(m_eDraggingMode))
	{
		UINT uScheduleID = XTP_SAFE_GET2(pHitTest2, pViewGroup, GetScheduleID(), 0);
		if (uScheduleID != m_ptrDraggingEventNew->GetScheduleID())
		{
			if (uScheduleID != XTP_CALENDAR_UNKNOWN_SCHEDULE_ID)
			{
				m_ptrDraggingEventNew->SetScheduleID(uScheduleID);
				bChanged = TRUE;
			}
			else if (m_ptrDraggingEventNew->GetScheduleID() != m_ptrDraggingEventOrig->GetScheduleID())
			{
				m_ptrDraggingEventNew->SetScheduleID(m_ptrDraggingEventOrig->GetScheduleID());
				bChanged = TRUE;
			}
		}
	}
	//---
	CXTPCalendarData* pHitData = XTP_SAFE_GET2(pHitTest2, pViewGroup, GetDataProvider(), NULL);
	if (pHitData && pHitData != m_ptrDraggingEventNew->GetDataProvider())
	{
		CXTPCalendarEventPtr ptrHitEventNew = pHitData->CreateNewEvent();
		if (ptrHitEventNew)
		{
			if (m_ptrDraggingEventNew->CloneEventTo(ptrHitEventNew))
			{
				m_ptrDraggingEventNew = ptrHitEventNew;
				bChanged = TRUE;
			}
		}
	}

	//------------------------------------------------------------------------
	//COleDateTimeSpan spDurationNew = m_ptrDraggingEventNew->GetEndTime() -
	//                                  m_ptrDraggingEventNew->GetStartTime();

	//  TRACE(_T("bAllDayEventArea =%d, Orig->IsAllDayEvent =%d, bAllMultiDayEventOrig =%d, New->GetDurationMinutes=%d [%s - %s]\n"),
	//      bAllDayEventArea, m_ptrDraggingEventOrig->IsAllDayEvent(),
	//bAllMultiDayEventOrig, (int)spDurationNew.GetTotalMinutes(),
	//m_ptrDraggingEventNew->GetStartTime().Format(),
	//m_ptrDraggingEventNew->GetEndTime().Format());

	BOOL bIsResizing = m_eDraggingMode == xtpCalendaDragModeResizeBegin ||
						m_eDraggingMode == xtpCalendaDragModeResizeEnd;

	BOOL bIsZeroLen = CXTPCalendarUtils::IsEqual(m_ptrDraggingEventNew->GetStartTime(),
						m_ptrDraggingEventNew->GetEndTime());

	if (bAllDayEventArea && (bIsResizing && !bIsZeroLen || !bIsResizing) &&
		(m_ptrDraggingEventOrig->IsAllDayEvent() || !bAllMultiDayEventOrig
		 || m_ptrDraggingEventNew->IsAllDayEvent()))
	{
		m_ptrDraggingEventNew->SetAllDayEvent(TRUE);
	}
	else
	{
		m_ptrDraggingEventNew->SetAllDayEvent(FALSE);
	}
	//------------------------------------------------------------------------

	return bChanged || bChangedBase;
}

XTPCalendarDayViewScrollDirection CXTPCalendarDayView::GetNeededScrollDirection(CPoint pnt)
{
	static const int nVScrollZone = 20;
	CRect rcCtrl;

	if (!m_pControl)
	{
		return xtpCalendarDayViewScrollNotNeeded;
	}

	m_pControl->GetWindowRect(rcCtrl);
	CRect rcAllDayEvents = GetAllDayEventsRectangle();
	SCROLLINFO si;
	GetScrollBarInfoV(&si);

	int nCurPos = si.nPos;
	int nLimit = m_pControl->GetScrollLimit(SB_VERT);

	BOOL bWidtdhOK = (rcAllDayEvents.left <= pnt.x &&
		rcAllDayEvents.right >= pnt.x);

	if (!bWidtdhOK)
		return xtpCalendarDayViewScrollNotNeeded;

	if ((pnt.y >= rcAllDayEvents.bottom) &&
		(pnt.y - rcAllDayEvents.bottom <= nVScrollZone) && nCurPos > 0 &&
		m_ptLBtnDownMousePos.y - pnt.y > 2)
	{
		// Scroll up
		return xtpCalendarDayViewScrollUp;
	}

	if ((pnt.y + 3 < rcCtrl.Height()) &&
		(rcCtrl.Height() - (pnt.y + 3) <= nVScrollZone) && nCurPos < nLimit &&
		pnt.y - m_ptLBtnDownMousePos.y > 2)
	{
		return xtpCalendarDayViewScrollDown;
	}

	return xtpCalendarDayViewScrollNotNeeded;
}

BOOL CXTPCalendarDayView::IsOutOfClientRect(CPoint pnt, XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pHitInfo)
{
	if (!m_pControl)
	{
		return FALSE;
	}

	CRect rcCtrl;
	m_pControl->GetWindowRect(rcCtrl);

	if (m_pTimeScaleHeader2 && m_pTimeScaleHeader2->HitTestDateTime(pHitInfo))
	{
		return FALSE;
	}

	if (m_pTimeScaleHeader && m_pTimeScaleHeader->HitTestDateTime(pHitInfo))
	{
		return FALSE;
	}


	//bottom
	if (pnt.y + 3 >= rcCtrl.Height())
		return TRUE;
	//top
	CRect rcAllDayEvents = GetAllDayEventsRectangle();
	if (pnt.y <= rcAllDayEvents.top)
		return TRUE;

	//left
	if (pnt.x < rcAllDayEvents.left)
		return TRUE;

	//right
	if (pnt.x > rcAllDayEvents.right)
		return TRUE;

	if (pHitInfo->uHitCode & xtpCalendarHitTestDayViewTimeScale)
		return TRUE;

	return FALSE;
}


void CXTPCalendarDayView::StopVertEventScroll()
{
	if (m_dwScrollingEventTimerID)
	{
		KillTimer(m_dwScrollingEventTimerID);
		m_dwScrollingEventTimerID = 0;
	}
}

void CXTPCalendarDayView::SetMouseOutOfDragArea(BOOL bOutOfArea)
{

	BOOL bChanged = m_bMouseOutOfDragArea != bOutOfArea;
	m_bMouseOutOfDragArea = bOutOfArea;

	BOOL bNeedUpd = FALSE;

	if (m_bMouseOutOfDragArea)
	{
		if (m_ptrDraggingEventNew)
		{
			m_ptrDraggingEventNew->MoveEvent(m_dtDraggingStartPoint);
		}
		if (m_pControl && m_pControl->m_mouseMode != xtpCalendarMouseEventDraggingOut)
		{
			m_pControl->m_mouseMode = xtpCalendarMouseEventDraggingOut;
			TRACE_DRAGGING(_T("SetMouseOutOfDragArea OUT\n"));
			bNeedUpd = TRUE;
		}
	}


	if (bChanged && !m_bMouseOutOfDragArea)
	{
		TRACE_DRAGGING(_T("SetMouseOutOfDragArea IN\n"));

		XTPCalendarMouseMode mouseMode = xtpCalendarMouseNothing;
		switch (m_eDraggingMode)
		{
			case xtpCalendaDragModeUnknown:
			case xtpCalendaDragModeEditSubject:
				mouseMode = xtpCalendarMouseNothing;
				break;
			case xtpCalendaDragModeCopy:
				mouseMode = xtpCalendarMouseEventDragCopy;
				break;
			case xtpCalendaDragModeMove:
				mouseMode = xtpCalendarMouseEventDragMove;
				break;
			case xtpCalendaDragModeResizeBegin:
			case xtpCalendaDragModeResizeEnd:
				mouseMode = xtpCalendarMouseEventResizingV;
				break;
		}

		XTP_SAFE_SET1(m_pControl, m_mouseMode, mouseMode);
		bNeedUpd = TRUE;
	}

	if (bNeedUpd && m_pControl)
		m_pControl->UpdateMouseCursor();
}

BOOL CXTPCalendarDayView::VertEventScroll(BOOL bUp)
{
	if (!m_pControl)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	SCROLLINFO si;
	if (!GetScrollBarInfoV(&si))
	{
		return FALSE;
	}

	int nCurPos = si.nPos;
	int nCurPos_raw = si.nPos;
	int nLimit = m_pControl->GetScrollLimit(SB_VERT);


	if (bUp && nCurPos > 0)
	{
		//scroll up
		nCurPos = max(nCurPos - 1, 0);
		nCurPos_raw--;
		m_bScrollingEventUp = TRUE;
	}
	else if (!bUp && nCurPos < nLimit)
	{
		//scroll down
		nCurPos = min(nCurPos + 1, nLimit);
		nCurPos_raw++;
		m_bScrollingEventUp = FALSE;
	}
	else
	{
		//limit is reached
		StopVertEventScroll();
		return FALSE;//not handled
	}


	ScrollV(nCurPos, nCurPos_raw);
	m_pControl->SetScrollPos(SB_VERT, nCurPos, FALSE);
	m_pControl->AdjustScrollBar();

	if (!m_dwScrollingEventTimerID)
	{
		//Start timer
		m_dwScrollingEventTimerID = SetTimer(XTP_SCROLL_TIMER_RESOLUTION_MS);
		DBG_TRACE_TIMER(_T("SET Timer: ID =%d, m_dwScrollingEventTimerID, CXTPCalendarDayView::VertEventScroll() \n"), m_dwScrollingEventTimerID);
	}

	return TRUE;
}

void CXTPCalendarDayView::OnActivateView(BOOL bActivate, CXTPCalendarView* pActivateView,
										CXTPCalendarView* pInactiveView)
{
	TBase::OnActivateView(bActivate, pActivateView, pInactiveView);

	if (!bActivate)
	{
		if (m_dwRedrawNowLineTimerID)
		{
			KillTimer(m_dwRedrawNowLineTimerID);
			m_dwRedrawNowLineTimerID = 0;
			//TRACE(_T("DayView: Auto redraw timer was KILLED. \n"));
		}

		if (m_dwScrollingEventTimerID)
		{
			KillTimer(m_dwScrollingEventTimerID);
			m_dwScrollingEventTimerID = 0;
		}
	}

	if (bActivate && m_dwRedrawNowLineTimerID == 0)
	{
		m_dwRedrawNowLineTimerID = SetTimer(XTP_REDRAW_TIMER_RESOLUTION_MS);
		DBG_TRACE_TIMER(_T("SET Timer: ID =%d, m_dwRedrawNowLineTimerID, CXTPCalendarDayView::OnActivateView() \n"), m_dwRedrawNowLineTimerID);
		//TRACE(_T("DayView: Auto redraw timer was SET. \n"));
	}

}

BOOL CXTPCalendarDayView::OnTimer(UINT_PTR uTimerID)
{
	if (uTimerID == m_dwScrollingEventTimerID)
	{
		OnMouseMove(MK_LBUTTON, m_ptLastMousePos);
		return TRUE;
	}
	else if (uTimerID == m_dwRedrawNowLineTimerID)
	{
		COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();

		//COleDateTimeSpan spDiff = dtNow - m_dtLastRedrawTime;
		//if ((spDiff.GetTotalSeconds() >= 15 || dtNow.GetSecond()%15 == 0) &&
		//  dtNow.GetSecond() != m_dtLastRedrawTime.GetSecond())

		if (dtNow.GetMinute() != m_dtLastRedrawTime.GetMinute())
		{
			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);
			//TRACE(_T("DayView: Auto REDRAW (%d:%d:%d). \n"), (int)dtNow.GetHour(),
			//  (int)dtNow.GetMinute(), (int)dtNow.GetSecond());
		}
		return TRUE;
	}

	return TBase::OnTimer(uTimerID);
}


BOOL CXTPCalendarDayView::OnEndDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* pHitInfo)
{
	if (!m_pControl || !pHitInfo)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CRect rcCtrl;
	m_pControl->GetWindowRect(rcCtrl);
	BOOL bReturnBack = point.y + 3 >= rcCtrl.Height();

	if (m_dwScrollingEventTimerID)
	{
		if (bReturnBack)
		{
			pHitInfo->dt = m_dtDraggingStartPoint;
			OnDragging(point, pHitInfo);
		}
		else
		{
			OnDragging(point, pHitInfo);
		}
	}

	StopVertEventScroll();


	m_dtDraggingStartPoint = COleDateTime(0, 0, 0, 0, 0, 0);

	if (_IsDragModeCopyMove(m_eDraggingMode) && m_ptrDraggingEventNew && m_ptrDraggingEventOrig)
	{
		if (!m_ptrDraggingEventNew->IsEqualStartEnd(m_ptrDraggingEventOrig))
		{
			COleDateTime dtS = m_ptrDraggingEventNew->GetStartTime();
			COleDateTime dtE = m_ptrDraggingEventNew->GetEndTime();
			COleDateTime dtS0 = CXTPCalendarUtils::ResetDate(dtS);
			COleDateTime dtE0 = CXTPCalendarUtils::ResetDate(dtE);

			m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateTime(dtS, GetCellTime(GetCellNumber(dtS0, FALSE)));
			m_selectedBlock.dtEnd = GetCellTime(GetCellNumber(dtE0, dtS0 == dtE0 ? FALSE : TRUE)) + GetCellDuration();
			m_selectedBlock.dtEnd = CXTPCalendarUtils::UpdateTime(dtE, m_selectedBlock.dtEnd);

			if (m_ptrDraggingEventNew->IsAllDayEvent() ||
					m_ptrDraggingEventNew->GetDurationMinutes() > 24 * 60)
			{
				m_selectedBlock.bAllDayEvent = TRUE;
			}
			m_selectedBlock.nGroupIndex = pHitInfo->nGroup;
			CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
		}
	}

	m_nAllDayEventsCountMin_WhenDrag = 0;
	{
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
	}

	return TBase::OnEndDragging(point, pHitInfo);
}

BOOL CXTPCalendarDayView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_ptLBtnDownMousePos = CPoint(0, 0);

	return TBase::OnLButtonUp(nFlags, point);
}

BOOL CXTPCalendarDayView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSelectionChangedContext selChanged(this);

	TBase::OnLButtonDown(nFlags, point);

	m_ptLBtnDownMousePos = point;

	if (m_eDraggingMode != xtpCalendaDragModeUnknown)
	{
		return FALSE;
	}

	XTP_CALENDAR_HITTESTINFO_DAY_VIEW hitTestInfo;

	if (HitTestEx(point, &hitTestInfo))
	{
		if (hitTestInfo.dt.GetStatus() != COleDateTime::valid)
			return FALSE;

		ASSERT(hitTestInfo.dt.GetStatus() == COleDateTime::valid);

		BOOL bEvent = (hitTestInfo.uHitCode & xtpCalendarHitTestEvent_Mask) != 0;

		if (bEvent)
		{
			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

			m_selectedBlock.dtEnd = m_selectedBlock.dtBegin = hitTestInfo.dt;
			m_dtSelectionStart = hitTestInfo.dt;
			m_selectedBlock.bAllDayEvent = FALSE;
			m_selectedBlock.nGroupIndex = hitTestInfo.nGroup;

			CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
		}
		else
		{
			BOOL bFixBegin = (nFlags & MK_SHIFT);
			BOOL bAllDay = 0 != (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewAllDayEvent);
			COleDateTime dtNewEnd = m_selectedBlock.dtEnd;

			if (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewTimeScale)
			{
				dtNewEnd = CXTPCalendarUtils::UpdateTime(dtNewEnd, hitTestInfo.dt + GetCellDuration());
				hitTestInfo.nGroup = max(0, m_selectedBlock.nGroupIndex);
			}
			else if (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewCell)
			{
				dtNewEnd = hitTestInfo.dt + GetCellDuration();
			}
			else if (bAllDay)
			{
				dtNewEnd = hitTestInfo.dt + COleDateTimeSpan(1, 0, 0, 0);
			}

			ProcessCellSelection(dtNewEnd, bFixBegin, bAllDay, hitTestInfo.nGroup);
		}
		return TRUE;
	}
	return FALSE;
}

void CXTPCalendarDayView::OnMouseMove(UINT nFlags, CPoint point)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CSelectionChangedContext selChanged(this);

	m_ptLastMousePos = point;

	TBase::OnMouseMove(nFlags, point);

	if (m_eDraggingMode != xtpCalendaDragModeUnknown)
	{
		return;
	}

	// process selection mode
	if ((nFlags & MK_LBUTTON) == 0)
	{
		return;
	}
	else if (!m_bStartedClickInside)
	{
		return;
	}

	XTP_CALENDAR_HITTESTINFO_DAY_VIEW hitTestInfo;
	if (HitTestEx(point, &hitTestInfo))
	{
		COleDateTime dtEnd = m_selectedBlock.dtEnd;
		BOOL bAllDayEvent = m_selectedBlock.bAllDayEvent;


		BOOL bScrolled = FALSE;
		XTPCalendarDayViewScrollDirection direction = GetNeededScrollDirection(point);
		if (direction != xtpCalendarDayViewScrollNotNeeded &&
			hitTestInfo.dt.GetStatus() == COleDateTime::valid)
		{
			if (direction == xtpCalendarDayViewScrollUp)
			{
				bScrolled = VertEventScroll(TRUE);//scroll up
				hitTestInfo.dt -= GetCellDuration();
			}
			else if (direction == xtpCalendarDayViewScrollDown)
			{
				bScrolled = VertEventScroll(FALSE);
				hitTestInfo.dt += GetCellDuration();
			}

		}
		if (!bScrolled)
		{
			StopVertEventScroll();
		}


		//-------------------------------------------------------------------
		// selection processing

		if (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewTimeScale)
		{
			m_selectedBlock.bAllDayEvent = FALSE;
			if (bAllDayEvent)
			{
				m_selectedBlock.dtBegin = m_dtSelectionStart;
			}

			m_selectedBlock.dtEnd = CXTPCalendarUtils::UpdateTime(m_selectedBlock.dtEnd, hitTestInfo.dt);
			if (m_selectedBlock.dtEnd >= m_selectedBlock.dtBegin)
			{
				m_selectedBlock.dtEnd += GetCellDuration();
			}
		}
		else if (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewCell)
		{
			m_selectedBlock.bAllDayEvent = FALSE;
			if (bAllDayEvent)
			{
				m_selectedBlock.dtBegin = m_dtSelectionStart;
			}

			m_selectedBlock.dtEnd = hitTestInfo.dt;
			if (m_selectedBlock.dtEnd >= m_selectedBlock.dtBegin)
			{
				m_selectedBlock.dtEnd += GetCellDuration();
			}
		}
		else if (hitTestInfo.uHitCode & xtpCalendarHitTestDayViewAllDayEvent)
		{
			if (!bAllDayEvent)
			{
				m_dtSelectionStart = m_selectedBlock.dtBegin;
				m_selectedBlock.dtBegin = CXTPCalendarUtils::ResetTime(m_selectedBlock.dtBegin);
			}
			m_selectedBlock.dtEnd = hitTestInfo.dt;
			if (m_selectedBlock.dtEnd >= m_selectedBlock.dtBegin)
			{
				m_selectedBlock.dtEnd += COleDateTimeSpan(1, 0, 0, 0);
			}
			m_selectedBlock.bAllDayEvent = TRUE;
		}

		//---------------------------------------------------------------------------
		if (m_selectedBlock.dtEnd != dtEnd || m_selectedBlock.bAllDayEvent != bAllDayEvent)
		{
			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);
			CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
		}
	}
}

BOOL CXTPCalendarDayView::GetSelection(COleDateTime* pBegin, COleDateTime* pEnd,
									   BOOL* pbAllDayEvent, int* pnGroupIndex,
									   COleDateTimeSpan* pspSelectionResolution)
{
	if (!m_selectedBlock.IsValid())
		return FALSE;

	COleDateTime dtBegin = m_selectedBlock.dtBegin, dtEnd = m_selectedBlock.dtEnd;

	BOOL bAllDayEvent = m_selectedBlock.bAllDayEvent;

	if (pbAllDayEvent)
	{
		*pbAllDayEvent = bAllDayEvent;
	}

	if (bAllDayEvent)
	{
		if (dtBegin > dtEnd)
		{
			COleDateTime dt = dtEnd;
			dtEnd = dtBegin + COleDateTimeSpan(1, 0, 0, 0);

			dtBegin = dt;
		}

		dtBegin = CXTPCalendarUtils::ResetTime(dtBegin);
		if (!CXTPCalendarUtils::IsZeroTime(dtEnd))
		{
			dtEnd += COleDateTimeSpan(1, 0, 0, 0);
		}
		dtEnd = CXTPCalendarUtils::ResetTime(dtEnd);

	}
	else
	{
		if (dtBegin >= dtEnd)
		{
			COleDateTime dt = dtEnd;
			dtEnd = dtBegin + GetCellDuration();
			dtBegin = dt;
		}

		bAllDayEvent = CXTPCalendarUtils::IsZeroTime(dtBegin) &&
					   CXTPCalendarUtils::IsZeroTime(dtEnd);
	}

	if (pBegin)
		*pBegin = dtBegin;

	if (pEnd)
		*pEnd = dtEnd;

	if (pnGroupIndex)
	{
		*pnGroupIndex = m_selectedBlock.nGroupIndex;
	}

	if (pspSelectionResolution)
	{
		if (bAllDayEvent)
		{
			*pspSelectionResolution = COleDateTimeSpan(1, 0, 0, 0);
		}
		else
		{
			*pspSelectionResolution = GetCellDuration();
		}
	}

	return TRUE;
}

void CXTPCalendarDayView::ShowDay(const COleDateTime& date, BOOL bSelect)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl);
	ClearDays();
	AddDay(date);

	if (bSelect)
	{
		CXTPCalendarDayViewDay* pDay = m_arDays.GetCount() > 0 ? m_arDays.GetAt(0) : NULL;
		if (pDay)
			SelectDay(pDay);
	}
}

void CXTPCalendarDayView::AddDay(const COleDateTime& date)
{
	CXTPCalendarControl::CViewChangedContext viewChanged(this);

	_AddDay(date);
}

void CXTPCalendarDayView::_AddDay(const COleDateTime& date)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl);
	CXTPCalendarDayViewDay* pDay = new CXTPCalendarDayViewDay(this, date);
	if (pDay)
	{
		m_arDays.Add(pDay);
		pDay->Populate(date);
	}
}

void CXTPCalendarDayView::ShowDays(const COleDateTime& dtBegin, const COleDateTime& dtEnd)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl);

	CXTPCalendarControl::CViewChangedContext viewChanged(this);

	ClearDays();

	COleDateTimeSpan spDay(1, 0, 0, 0);
	COleDateTimeSpan spDaysRange = CXTPCalendarUtils::ResetTime(dtEnd) - CXTPCalendarUtils::ResetTime(dtBegin);

	int nDays = GETTOTAL_DAYS_DTS(spDaysRange) + 1;
	COleDateTime dtDay = CXTPCalendarUtils::ResetTime(dtBegin);

	nDays = max(1, nDays); // to be sure that at leas one day will be added
	for (int i = 0; i < nDays; i++)
	{
		_AddDay(dtDay);
		dtDay += spDay;
	}
}

void CXTPCalendarDayView::ShowWorkingDays(const COleDateTime& dtDay)
{
	if (!m_pControl)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarControl::CUpdateContext updateContext(m_pControl);
	CXTPCalendarControl::CViewChangedContext viewChanged(this);

	COleDateTime dtWeekDay(dtDay);

	int nFirstWeekDay = m_pControl->GetFirstDayOfWeek();
	// adjust beginning of the week iteration period to the FirstDayOfWeek
	while (dtWeekDay.GetDayOfWeek() != nFirstWeekDay)
	{
		dtWeekDay -= COleDateTimeSpan(1);
	}

	// add all working days during the week to the collection
	ClearDays();
	for (int nWeekDay = 0; nWeekDay < 7; nWeekDay++)
	{
		int nWWMask = m_pControl->GetWorkWeekMask();
		int nDayOfWeek = dtWeekDay.GetDayOfWeek();
		int nDayMask = CXTPCalendarUtils::GetDayOfWeekMask(nDayOfWeek);
		if (nWWMask & nDayMask)
		{
			_AddDay(dtWeekDay);
		}
		dtWeekDay += COleDateTimeSpan(1);
	}

	if (GetViewDayCount() == 0)
	{
		_AddDay(CXTPCalendarUtils::ResetTime(COleDateTime::GetCurrentTime()));
	}
}

COleDateTimeSpan CXTPCalendarDayView::GetScaleInterval() const
{
	int nSI_min = XTP_SAFE_GET2(m_pControl, GetCalendarOptions(), nDayView_ScaleInterval, 30);
	return COleDateTimeSpan(0, 0, nSI_min, 0);
}

void CXTPCalendarDayView::SetScaleInterval(const COleDateTimeSpan spScaleInterval)
{
	BOOL bValidSVal = GETTOTAL_MINUTES_DTS(spScaleInterval) >= 1;
	ASSERT(bValidSVal);

	if (m_pTimeScaleHeader && bValidSVal)
	{
		int nOldScaleInterval = XTP_SAFE_GET2(m_pControl, GetCalendarOptions(), nDayView_ScaleInterval, 30);
		int nScaleInterval = (int)GETTOTAL_MINUTES_DTS(spScaleInterval);

		if (nScaleInterval != nOldScaleInterval)
		{
			XTP_SAFE_SET2(m_pControl, GetCalendarOptions(), nDayView_ScaleInterval, nScaleInterval);

			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateAll);

			SendNotification(XTP_NC_CALENDAROPTIONSWASCHANGED, xtpCalendarDayView, 0);
			SendNotification(XTP_NC_CALENDARVIEWTIMESCALEWASCHANGED, 0, 0);
		}
	}
}

void CXTPCalendarDayView::SetScaleText(LPCTSTR strText)
{
	XTP_SAFE_CALL1(m_pTimeScaleHeader, SetCaption(strText));

	XTP_SAFE_SET2(m_pControl, GetCalendarOptions(), strDayView_ScaleLabel, strText);

	SendNotification(XTP_NC_CALENDAROPTIONSWASCHANGED, xtpCalendarDayView, 0);

	SendNotification(XTP_NC_CALENDARVIEWTIMESCALEWASCHANGED, 0, 0);
}
CString CXTPCalendarDayView::GetScaleText()
{
	return XTP_SAFE_GET2(m_pControl, GetCalendarOptions(), strDayView_ScaleLabel, _T(""));
}

void CXTPCalendarDayView::SetScale2Text(LPCTSTR strText)
{
	XTP_SAFE_CALL1(m_pTimeScaleHeader2, SetCaption(strText));

	XTP_SAFE_SET2(m_pControl, GetCalendarOptions(), strDayView_Scale2Label, strText);

	SendNotification(XTP_NC_CALENDAROPTIONSWASCHANGED, xtpCalendarDayView, 0);
	SendNotification(XTP_NC_CALENDARVIEWTIMESCALEWASCHANGED, 0, 0);
}
CString CXTPCalendarDayView::GetScale2Text()
{
	return XTP_SAFE_GET2(m_pControl, GetCalendarOptions(), strDayView_Scale2Label, _T(""));
}

void CXTPCalendarDayView::SetScale2TimeZone(const TIME_ZONE_INFORMATION* pTzInfo)
{
	if (!pTzInfo || !m_pControl || !m_pControl->GetCalendarOptions())
	{
		ASSERT(FALSE);
		return;
	}

	m_pControl->GetCalendarOptions()->tziDayView_Scale2TimeZone = *pTzInfo;

	AdjustScale2TimeZone();

	SendNotification(XTP_NC_CALENDAROPTIONSWASCHANGED, xtpCalendarDayView, 0);
	SendNotification(XTP_NC_CALENDARVIEWTIMESCALEWASCHANGED, 0, 0);
}

void CXTPCalendarDayView::AdjustScale2TimeZone()
{
	TIME_ZONE_INFORMATION tziCurrent;
	::ZeroMemory(&tziCurrent, sizeof(tziCurrent));

	if (::GetTimeZoneInformation(&tziCurrent) == TIME_ZONE_ID_INVALID)
	{
		ASSERT(FALSE);
		return;
	}
	TIME_ZONE_INFORMATION tziScale2 = GetScale2TimeZone();

	SYSTEMTIME systm00, systmScale1, systmScale2;
	ZeroMemory(&systm00, sizeof(systm00));
	ZeroMemory(&systmScale1, sizeof(systmScale1));
	ZeroMemory(&systmScale2, sizeof(systmScale2));

	::GetLocalTime(&systm00);
	systm00.wHour = systm00.wMinute = systm00.wSecond = 0;

	if (!CXTPCalendarUtils::SystemTimeToTzSpecificLocalTime(&tziCurrent,
												&systm00, &systmScale1))
	{
		ASSERT(FALSE);
		return;
	}
	if (!CXTPCalendarUtils::SystemTimeToTzSpecificLocalTime(&tziScale2,
												&systm00, &systmScale2))
	{
		ASSERT(FALSE);
		return;
	}

	COleDateTime dtScale1(systmScale1), dtScale2(systmScale2);
	COleDateTimeSpan spScale2Offset = dtScale2 - dtScale1;
	ASSERT((GETTOTAL_SECONDS_DTS(spScale2Offset) % 60) == 0);

	int nTimeShift2_min = GETTOTAL_MINUTES_DTS(spScale2Offset);

	XTP_SAFE_CALL1(m_pTimeScaleHeader2, SetTimeshift(nTimeShift2_min));
}

const TIME_ZONE_INFORMATION& CXTPCalendarDayView::GetScale2TimeZone()
{
	static TIME_ZONE_INFORMATION s_tzEmty = {0};

	return XTP_SAFE_GET2(m_pControl, GetCalendarOptions(), tziDayView_Scale2TimeZone, s_tzEmty);
}


void CXTPCalendarDayView::ShowScale2(BOOL bShow)
{
	if (!m_pTimeScaleHeader2 || !m_pControl)
	{
		ASSERT(FALSE);
		return;
	}

	m_pTimeScaleHeader2->SetVisible(bShow);

	m_pControl->GetCalendarOptions()->bDayView_Scale2Visible = bShow;
	m_pControl->AdjustLayout();

	SendNotification(XTP_NC_CALENDAROPTIONSWASCHANGED, xtpCalendarDayView, 0);
	SendNotification(XTP_NC_CALENDARVIEWTIMESCALEWASCHANGED, 0, 0);
}
BOOL CXTPCalendarDayView::IsScale2Visible()
{
	return XTP_SAFE_GET2(m_pControl, GetCalendarOptions(), bDayView_Scale2Visible, FALSE);
}

COleDateTime CXTPCalendarDayView::GetViewDayDate(int nIndex)
{
	CXTPCalendarDayViewDay* pViewDay = GetViewDay(nIndex);
	ASSERT(pViewDay);
	return pViewDay ? pViewDay->m_dtDate :
				CXTPCalendarUtils::ResetTime(CXTPCalendarUtils::GetCurrentTime());
}

void CXTPCalendarDayView::_ScrollDays(int nScrollDaysCount, BOOL bPrev)
{
	ASSERT(nScrollDaysCount >= 1 && nScrollDaysCount <= 14);
	nScrollDaysCount = min(14, max(1, nScrollDaysCount));

	int nCount = GetViewDayCount();
	if (!nCount)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarControl::CUpdateContext updateContext(m_pControl);
	CXTPCalendarControl::CViewChangedContext viewChanged(this);

	COleDateTimeSpan spDay(bPrev ? -1 : 1, 0, 0, 0);
	COleDateTime dtNewDay = GetViewDayDate(bPrev ? 0 : nCount-1);

	for (int i = 0; i < nScrollDaysCount; i++)
	{
		dtNewDay += spDay;

		CXTPCalendarDayViewDay* pDay = new CXTPCalendarDayViewDay(this, dtNewDay);
		if (pDay)
		{
			m_arDays.RemoveAt(bPrev ? nCount-1 : 0);
			m_arDays.InsertAt(bPrev ? 0 : nCount-1, pDay);

			pDay->Populate(dtNewDay);
		}
	}
}

void CXTPCalendarDayView::ScrollDaysToPrev(int nScrollDaysCount)
{
	_ScrollDays(nScrollDaysCount, TRUE);
}

void CXTPCalendarDayView::ScrollDaysToNext(int nScrollDaysCount)
{
	_ScrollDays(nScrollDaysCount, FALSE);
}

BOOL CXTPCalendarDayView::IsExpandDown()
{
	int nDays = GetViewDayCount();
	for (int nDay = 0; nDay < nDays; nDay++)
	{
		CXTPCalendarDayViewDay* pDay = GetViewDay(nDay);
		ASSERT(pDay);
		if (!pDay)
			continue;
		int nGroupsCount = pDay->GetViewGroupsCount();
		for (int i = 0; i < nGroupsCount; i++)
		{
			if (pDay->GetViewGroup(i) && pDay->GetViewGroup(i)->IsExpandDown())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CXTPCalendarDayView::IsExpandUp()
{
	int nDays = GetViewDayCount();
	for (int nDay = 0; nDay < nDays; nDay++)
	{
		CXTPCalendarDayViewDay* pDay = GetViewDay(nDay);
		ASSERT(pDay);
		if (!pDay)
			continue;
		int nGroupsCount = pDay->GetViewGroupsCount();
		for (int i = 0; i < nGroupsCount; i++)
		{
			if (pDay->GetViewGroup(i) && pDay->GetViewGroup(i)->IsExpandUp())
				return TRUE;
		}
	}
	return FALSE;
}

CXTPCalendarDayViewDay* CXTPCalendarDayView::GetDay(COleDateTime dtDay)
{
	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	int nDays = GetViewDayCount();
	for (int i = 0; i < nDays; i++)
	{
		COleDateTime dtDay_I = GetViewDayDate(i);

		if (CXTPCalendarUtils::IsEqual(dtDay, dtDay_I))
		{
			return GetViewDay(i);
		}
	}
	return NULL;
}

void CXTPCalendarDayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CSelectionChangedContext selChanged(this);

	TBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN ||
			nChar == VK_PRIOR || nChar == VK_NEXT)
	{
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

		UnselectAllEvents();

		BOOL bVKShift = !!(GetKeyState(VK_SHIFT) & 0x8000);
		COleDateTimeSpan spCell = GetCellDuration();
		COleDateTimeSpan spDay(1, 0, 0, 0);
		COleDateTime dtNewSelEnd = m_selectedBlock.dtEnd;

		BOOL bEndIsEnd = m_selectedBlock.dtEnd > m_selectedBlock.dtBegin || !bVKShift;
		int nSelCell = GetCellNumber(CXTPCalendarUtils::ResetDate(m_selectedBlock.dtEnd -
			COleDateTimeSpan(0, 0, 0, bEndIsEnd ? 1 : 0)), bEndIsEnd);

		CXTPDrawHelpers::KeyToLayout(m_pControl, nChar);

		int nDVCount = GetViewDayCount();
		COleDateTime dtFirstDay = GetViewDayDate(0);
		COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);

		if (nChar == VK_LEFT)
		{
			if (!m_bScrollH_Disabled || CXTPCalendarUtils::ResetTime(dtNewSelEnd) > dtFirstDay)
			{
				if (!bVKShift && m_selectedBlock.nGroupIndex > 0)
				{
					m_selectedBlock.nGroupIndex--;
				}
				else
				{
					dtNewSelEnd -= spDay;

					CXTPCalendarDayViewDay* pDay = GetDay(dtNewSelEnd);
					if (!bVKShift && pDay)
					{
						m_selectedBlock.nGroupIndex = pDay->GetViewGroupsCount() - 1;
					}
				}
			}
		}
		else
		if (nChar == VK_RIGHT)
		{
			if (!m_bScrollH_Disabled || CXTPCalendarUtils::ResetTime(dtNewSelEnd) < dtLastDay)
			{
				CXTPCalendarDayViewDay* pDay = GetDay(dtNewSelEnd);
				if (!bVKShift && pDay && m_selectedBlock.nGroupIndex < pDay->GetViewGroupsCount() - 1)
				{
					m_selectedBlock.nGroupIndex++;
				}
				else
				{
					dtNewSelEnd += spDay;
					m_selectedBlock.nGroupIndex = 0;
				}
			}
		}
		else
		if (nChar == VK_UP)
		{
			BOOL bRange = !m_bScrollV_Disabled || nSelCell > m_LayoutX.m_nTopRow;

			if (nSelCell > 0 && bRange)
			{
				dtNewSelEnd = CXTPCalendarUtils::UpdateTime(dtNewSelEnd, GetCellTime(nSelCell));
			}
			if (m_selectedBlock.bAllDayEvent)
			{
				m_selectedBlock.bAllDayEvent = FALSE;
				dtNewSelEnd -= spDay;
			}
		}
		else
		if (nChar == VK_DOWN)
		{
			BOOL bRange = !m_bScrollV_Disabled || nSelCell < m_LayoutX.m_nTopRow + m_LayoutX.m_nVisibleRowCount - 1;
			if (nSelCell + 1 < GetRowCount() && bRange)
			{
				dtNewSelEnd = CXTPCalendarUtils::UpdateTime(dtNewSelEnd, GetCellTime(nSelCell+1)) + spCell;
				if (nSelCell == GetRowCount()-2)
				{
					dtNewSelEnd -= COleDateTimeSpan(0, 0, 0, 1);
				}
			}
		}
		else
		if (nChar == VK_PRIOR)
		{
			if (!m_bScrollV_Disabled)
			{
				nSelCell = max(1, nSelCell - m_LayoutX.m_nVisibleRowCount + 1);
				m_LayoutX.m_nTopRow = max(0, m_LayoutX.m_nTopRow - m_LayoutX.m_nVisibleRowCount);
				dtNewSelEnd = CXTPCalendarUtils::UpdateTime(dtNewSelEnd, GetCellTime(nSelCell));

				CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
			}
		}
		else
		if (nChar == VK_NEXT)
		{
			if (!m_bScrollV_Disabled)
			{
				nSelCell = min(max(0, GetRowCount()-1), nSelCell + m_LayoutX.m_nVisibleRowCount);
				m_LayoutX.m_nTopRow += m_LayoutX.m_nVisibleRowCount;
				dtNewSelEnd = CXTPCalendarUtils::UpdateTime(dtNewSelEnd, GetCellTime(nSelCell)) + spCell;
				if (nSelCell == GetRowCount()-1)
				{
					dtNewSelEnd -= COleDateTimeSpan(0, 0, 0, 1);
				}

				CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
			}
		}

		ProcessCellSelection(dtNewSelEnd, bVKShift, m_selectedBlock.bAllDayEvent,
							 m_selectedBlock.nGroupIndex);
	}
}

void CXTPCalendarDayView::ProcessCellSelection(COleDateTime dtNewSelEnd,
				BOOL bFixSelBegin, BOOL bAllDayEventSel, int nGroupIndex)
{
	int nCount = GetViewDayCount();
	if (!nCount)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

	COleDateTimeSpan spDay(1, 0, 0, 0);
	COleDateTimeSpan spCell = GetCellDuration();
	COleDateTimeSpan spSelStep = bAllDayEventSel ? spDay : spCell;

	COleDateTime dtFirstDay = GetViewDayDate(0);

	COleDateTime dtSelBegin_prev = m_selectedBlock.dtBegin;
	COleDateTime dtSelEnd_prev = m_selectedBlock.dtEnd;

	if (CXTPCalendarUtils::IsEqual(m_selectedBlock.dtEnd, m_selectedBlock.dtBegin))
	{
		int nSelRow = GetTopRow() + min(2, GetVisibleRowCount());
		m_selectedBlock.dtBegin = CXTPCalendarUtils::UpdateTime(dtFirstDay, GetCellTime(nSelRow));
		m_dtSelectionStart = m_selectedBlock.dtBegin;

		m_selectedBlock.dtEnd = m_selectedBlock.dtBegin + spCell;
	}

	m_selectedBlock.dtEnd = dtNewSelEnd;

	if (!bFixSelBegin)
	{
		m_selectedBlock.dtBegin = m_selectedBlock.dtEnd - spSelStep;
		m_dtSelectionStart = m_selectedBlock.dtBegin;
	}
	else
		if (CXTPCalendarUtils::IsEqual(m_selectedBlock.dtEnd, m_selectedBlock.dtBegin) ||
			(!bAllDayEventSel && m_selectedBlock.dtEnd < m_selectedBlock.dtBegin &&
			 dtSelEnd_prev > dtSelBegin_prev)
		   )
		{
			m_selectedBlock.dtEnd -= spSelStep;
		}

	m_selectedBlock.bAllDayEvent = bAllDayEventSel;
	m_selectedBlock.nGroupIndex = nGroupIndex;

	//===========================================================================
	int nDVCount = GetViewDayCount();
	COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);
	COleDateTime& rdtSel1 = m_selectedBlock.dtEnd < m_selectedBlock.dtBegin ?
							m_selectedBlock.dtEnd : m_selectedBlock.dtBegin;
	COleDateTime& rdtSel2 = m_selectedBlock.dtEnd > m_selectedBlock.dtBegin ?
							m_selectedBlock.dtEnd : m_selectedBlock.dtBegin;

	COleDateTime dtSelDay1 = CXTPCalendarUtils::ResetTime(rdtSel1);
	COleDateTime dtSelDay2 = CXTPCalendarUtils::ResetTime(rdtSel2-COleDateTimeSpan(0, 0, 0, 1));

	//---------------------------------------------------------------------------
	if (!m_bScrollH_Disabled)
	{
		if (dtSelDay1 < dtFirstDay)
		{
			ScrollDaysToPrev();
		}
		else if (dtSelDay2 > dtLastDay)
		{
			ScrollDaysToNext();
		}
	}

	//---------------------------------------------------------------------------
	// update variables after scrolling
	nDVCount = GetViewDayCount();
	dtFirstDay = GetViewDayDate(0);
	dtLastDay = GetViewDayDate(nDVCount-1);

	if (dtSelDay1 < dtFirstDay || (!bFixSelBegin && dtSelDay2 >= dtLastDay))
	{
		if (bFixSelBegin)
		{
			rdtSel1 = CXTPCalendarUtils::UpdateTime(dtFirstDay, rdtSel1);
			rdtSel2 = rdtSel1 + spSelStep;
		}
	}
	//---------------------------------------------------------------------------
	if (dtSelDay2 > dtLastDay || (!bFixSelBegin && dtSelDay1 <= dtFirstDay))
	{
		rdtSel2 = rdtSel1 + spSelStep;
	}

	//---------------------------------------------------------------------------
	BOOL bEndIsEnd = m_selectedBlock.dtEnd > m_selectedBlock.dtBegin;
	int nSelEndCell = GetCellNumber(CXTPCalendarUtils::ResetDate(m_selectedBlock.dtEnd -
		COleDateTimeSpan(0, 0, 0, bEndIsEnd ? 1 : 0)), bEndIsEnd);

	if (nSelEndCell < m_LayoutX.m_nTopRow && !bAllDayEventSel)
	{
		if (!m_bScrollV_Disabled)
		{
			m_LayoutX.m_nTopRow = min(max(0, nSelEndCell), max(0, GetRowCount()-1));

			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
		}
	}
	else if (nSelEndCell >= m_LayoutX.m_nTopRow + m_LayoutX.m_nVisibleRowCount && !bAllDayEventSel)
	{
		if (!m_bScrollV_Disabled)
		{
			m_LayoutX.m_nTopRow = nSelEndCell - m_LayoutX.m_nVisibleRowCount + 1;
			m_LayoutX.m_nTopRow = min(max(0, m_LayoutX.m_nTopRow), max(0, GetRowCount() - 1));

			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
		}
	}

	if (_EnsureVisibleSelectionH())
	{
		GetCalendarControl()->AdjustLayout();
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
	}

	CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
}

BOOL CXTPCalendarDayView::_EnsureVisibleSelectionH()
{
	if (!m_selectedBlock.IsValid())
		return FALSE;

	CXTPCalendarViewDay* pVDay = _GetViewDay(CXTPCalendarUtils::ResetTime(m_selectedBlock.dtEnd));
	ASSERT(pVDay);

	if (!pVDay)
		return FALSE;

	CRect rcGroup = pVDay->GetDayRect();
	{
		CXTPCalendarViewGroup* pVGroup = pVDay->GetViewGroup_(m_selectedBlock.nGroupIndex);
		ASSERT(pVGroup);
		if (pVGroup)
			rcGroup = pVGroup->GetRect();
	}

	CRect rcVisible = _GetScrollRectClient();

	int nNewScrollPos = m_nScrollOffsetX;

	if (rcGroup.left < rcVisible.left)
	{
		nNewScrollPos -= rcVisible.left - rcGroup.left;
		if (nNewScrollPos <= 2) nNewScrollPos = 0;
	}
	else if (rcGroup.right > rcVisible.right)
	{
		nNewScrollPos += rcGroup.right - rcVisible.right + 1;
	}

	if (nNewScrollPos != m_nScrollOffsetX)
	{
		ScrollH(nNewScrollPos, nNewScrollPos);
		return TRUE;
	}

	return FALSE;
}

BOOL CXTPCalendarDayView::_EnsureVisibleH(CXTPCalendarViewEvent* pViewEvent)
{
	ASSERT(pViewEvent);

	CXTPCalendarViewGroup* pVGroup = pViewEvent ? pViewEvent->GetViewGroup_() : NULL;
	ASSERT(pVGroup);

	if (!pVGroup)
		return FALSE;

	SCROLLINFO si;
	if (!GetScrollBarInfoH(&si))
		return FALSE;

	CRect rcVisible = _GetScrollRectClient();

	CRect rcObj = pViewEvent->GetEventRect();
	CRect rcGroup = pVGroup->GetRect();

	if (pViewEvent->IsMultidayEvent() && (pViewEvent->GetMultiDayEventFlags() & xtpCalendarMultiDayMaster))
	{
		rcObj.left = max(rcObj.left, rcVisible.left);
		rcObj.right = min(rcObj.right, rcVisible.right);

		rcGroup.right = rcGroup.left + rcVisible.Width();
	}

	int nNewScrollPos = m_nScrollOffsetX;

	if (rcObj.left < rcVisible.left)
	{
		nNewScrollPos -= rcVisible.left - rcGroup.left;
	}
	else if (rcObj.right > rcVisible.right)
	{
		nNewScrollPos += rcGroup.right - rcVisible.right + 1;
	}

	if (nNewScrollPos != m_nScrollOffsetX)
	{
		ScrollH(nNewScrollPos, nNewScrollPos);
		return TRUE;
	}

	return FALSE;
}

BOOL CXTPCalendarDayView::_EnsureVisibleV(CXTPCalendarViewEvent* pViewEvent)
{
	if (!pViewEvent || !pViewEvent->GetEvent() || !GetCalendarControl() ||
		!XTP_SAFE_GET2(pViewEvent, GetViewGroup_(), GetViewDay_(), NULL) )
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//---------------------------------------------------------------------------
	if (pViewEvent->IsMultidayEvent())
	{
		//return pViewEvent->IsVisible();
		return FALSE;
	}

	//---------------------------------------------------------------------------
	COleDateTime dtDay = XTP_SAFE_GET3(pViewEvent, GetViewGroup_(), GetViewDay_(), GetDayDate(), (DATE)0);
	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	COleDateTime dtStart = pViewEvent->GetEvent()->GetStartTime();
	COleDateTime dtEnd = pViewEvent->GetEvent()->GetEndTime();

	COleDateTime stSTime = CXTPCalendarUtils::ResetDate(dtStart);
	COleDateTime stETime = CXTPCalendarUtils::ResetDate(dtEnd);

	if (CXTPCalendarUtils::IsZeroTime(stETime))
		stETime = CXTPCalendarUtils::SetTime_235959(stETime);

	int nSCell = 0;
	int nECell = max(0, GetRowCount() - 1);

	if (CXTPCalendarUtils::ResetTime(dtStart) == dtDay)
		nSCell =  GetCellNumber(stSTime, FALSE);

	if (CXTPCalendarUtils::ResetTime(dtEnd) == dtDay)
		nECell = GetCellNumber(stETime, TRUE);

	if (nSCell < m_LayoutX.m_nTopRow)
	{
		m_LayoutX.m_nTopRow = min(max(0, nSCell-1), max(0, GetRowCount()-1));

		return TRUE;
	}
	else if (nECell >= m_LayoutX.m_nTopRow + m_LayoutX.m_nVisibleRowCount)
	{
		int nTopRowPrev = m_LayoutX.m_nTopRow;

		m_LayoutX.m_nTopRow = nECell - m_LayoutX.m_nVisibleRowCount + 1;
		m_LayoutX.m_nTopRow = min(max(0, m_LayoutX.m_nTopRow), max(0, GetRowCount()-1));
		m_LayoutX.m_nTopRow = min(max(0, nSCell-1), m_LayoutX.m_nTopRow);

		if (nTopRowPrev != m_LayoutX.m_nTopRow)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CXTPCalendarDayView::EnsureVisible(CXTPCalendarViewEvent* pViewEvent)
{
	ASSERT(pViewEvent);
	if (!pViewEvent)
		return FALSE;

	BOOL bChanged1 = _EnsureVisibleH(pViewEvent);
	BOOL bChanged2 = _EnsureVisibleV(pViewEvent);

	if (bChanged1 || bChanged2)
	{
		GetCalendarControl()->AdjustLayout();
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
	}

	ASSERT(pViewEvent->IsVisible());

	return TRUE;
}

BOOL CXTPCalendarDayView::EnsureVisibleH(CXTPCalendarViewEvent* pViewEvent)
{
	ASSERT(pViewEvent);
	if (!pViewEvent)
		return FALSE;

	BOOL bChanged1 = _EnsureVisibleH(pViewEvent);

	if (bChanged1)
	{
		GetCalendarControl()->AdjustLayout();
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateScrollBar);
	}

	ASSERT(pViewEvent->IsVisible());

	return TRUE;
}


CXTPCalendarViewEvent* CXTPCalendarDayView::FindEventToEditByTAB(COleDateTime dtMinStart,
								BOOL bReverse, CXTPCalendarEvent* pAfterEvent)
{
	CXTPCalendarViewEvent* pViewEvent = TBase::FindEventToEditByTAB(dtMinStart, bReverse, pAfterEvent);
	if (pViewEvent)
	{
		if (EnsureVisible(pViewEvent))
		{
			return pViewEvent;
		}
	}
	return NULL;
}

COleDateTime CXTPCalendarDayView::GetNextTimeEditByTAB()
{
	COleDateTime dtSel1 = min(m_selectedBlock.dtBegin, m_selectedBlock.dtEnd);
	COleDateTime dtSel2 = max(m_selectedBlock.dtBegin, m_selectedBlock.dtEnd);
	COleDateTime dtSel1Day = CXTPCalendarUtils::ResetTime(dtSel1);
	COleDateTime dtSel2Day = CXTPCalendarUtils::ResetTime(dtSel2);

	int nDVCount = GetViewDayCount();
	COleDateTime dtFirstDay = GetViewDayDate(0);
	COleDateTime dtLastDay = GetViewDayDate(nDVCount-1);

	if (dtSel1Day >= dtFirstDay && dtSel1Day <= dtLastDay)
	{
		return dtSel1;
	}
	else
		if (dtSel2Day >= dtFirstDay && dtSel2Day <= dtLastDay)
		{
			return dtSel2;
		}
	return TBase::GetNextTimeEditByTAB();
}

void CXTPCalendarDayView::UpdateNextTimeEditByTAB(COleDateTime dtNext, BOOL bReverse, BOOL bReset)
{
	m_selectedBlock.dtBegin = m_selectedBlock.dtEnd = GetNextTimeEditByTAB();
	m_dtSelectionStart = m_selectedBlock.dtBegin;

	COleDateTime dtNextDay = CXTPCalendarUtils::ResetTime(dtNext);
	COleDateTime dtCurrDay = CXTPCalendarUtils::ResetTime(GetNextTimeEditByTAB());

	if (dtNextDay != dtCurrDay || bReset)
	{
		m_selectedBlock.dtBegin = m_selectedBlock.dtEnd = dtNextDay;
		m_dtSelectionStart = m_selectedBlock.dtBegin;

		COleDateTime dtFirstDay = GetViewDayDate(bReverse ? GetViewDayCount()-1 : 0);
		if (dtNextDay == dtFirstDay || bReset)
		{
			if (bReverse)
			{
				m_selectedBlock.dtBegin = m_selectedBlock.dtEnd = dtNextDay + COleDateTimeSpan(1, 0, 0, 0);
				m_selectedBlock.dtBegin = m_selectedBlock.dtEnd - GetCellDuration();
			}
			else
			{
				m_selectedBlock.dtEnd = m_selectedBlock.dtBegin + GetCellDuration();
			}
			ProcessCellSelection(m_selectedBlock.dtEnd, FALSE, FALSE, 0);
		}
	}
}

COleDateTimeSpan CXTPCalendarDayView::GetEventDurationMin() const
{
	return GetCellDuration();
}

CXTPCalendarTimeZonePtr CXTPCalendarDayView::GetCurrentTimeZoneInfo()
{
	TIME_ZONE_INFORMATION tziCurrent;
	::ZeroMemory(&tziCurrent, sizeof(tziCurrent));

	if (::GetTimeZoneInformation(&tziCurrent) == TIME_ZONE_ID_INVALID)
	{
		ASSERT(FALSE);
		return NULL;
	}

	return CXTPCalendarTimeZone::GetTimeZoneInfo(&tziCurrent);
}

CXTPCalendarTimeZonePtr CXTPCalendarDayView::GetScale2TimeZoneInfo()
{
	return CXTPCalendarTimeZone::GetTimeZoneInfo(&GetScale2TimeZone());
}

void CXTPCalendarDayView::ScrollToWorkDayBegin()
{
	if (!m_pControl || !m_pControl->GetCalendarOptions())
	{
		ASSERT(FALSE);
		return;
	}

	COleDateTime dtStartDate = GetViewDayDate(0);
	COleDateTime dtSelBegin = CXTPCalendarUtils::UpdateTime(dtStartDate, m_pControl->GetCalendarOptions()->dtWorkDayStartTime);
	COleDateTime dtSelEnd = dtSelBegin + GetCellDuration();

	SetSelection(dtSelBegin, dtSelEnd, FALSE);

	int nWDStartCell = GetCellNumber(m_pControl->GetCalendarOptions()->dtWorkDayStartTime, FALSE);
	_ScrollV(nWDStartCell, nWDStartCell);
}

int CXTPCalendarDayView::_GetTimeScaleWith()
{
	int nWidth = XTP_SAFE_GET1(GetTimeScale(), GetWidth(), 0);
	if (IsScale2Visible())
		nWidth += XTP_SAFE_GET1(GetTimeScale(2), GetWidth(), 0);

	return nWidth;
}

CRect CXTPCalendarDayView::_GetScrollRectClient()
{
	CXTPClientRect rcClient(GetCalendarControl());
	rcClient.left += _GetTimeScaleWith();
	rcClient.left = min(rcClient.left, rcClient.right);

	return rcClient;
}

/////////////////////////////////////////////////////////////////////////////
