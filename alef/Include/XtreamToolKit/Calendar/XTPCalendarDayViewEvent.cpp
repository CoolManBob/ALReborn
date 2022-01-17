// XTPCalendarViewEvent.cpp: implementation of the CXTPCalendarDayViewEvent class.
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

#include "XTPCalendarViewPart.h"
#include "XTPCalendarPaintManager.h"
#include "XTPCalendarUtils.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarDayViewDay.h"
#include "XTPCalendarDayViewEvent.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarData.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarRecurrencePattern.h"

#include "XTPCalendarTheme.h"
#include "XTPCalendarThemeOffice2007.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarDayViewEvent, CXTPCalendarViewEvent)

CXTPCalendarDayViewEvent::CXTPCalendarDayViewEvent(CXTPCalendarEvent* pEvent,
												   CXTPCalendarDayViewGroup* pViewGroup) :
		TBase(pEvent, pViewGroup)
{
	ASSERT(pViewGroup);

	m_bHideEventTime = TRUE;
	m_rcTimeframe.SetRectEmpty();
}

CXTPCalendarDayViewEvent::~CXTPCalendarDayViewEvent()
{
}

BOOL CXTPCalendarDayViewEvent::IsMultidayEvent()
{
	if (!m_ptrEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (m_ptrEvent->IsAllDayEvent())
		return TRUE;

	int nHours = m_ptrEvent->GetDurationMinutes()/60;
	return nHours >= 24;
}

void CXTPCalendarDayViewEvent::AdjustSubjectEditor()
{
	AdjustSubjectEditorEx(TRUE);
}

void CXTPCalendarDayViewEvent::AdjustSubjectEditorEx(BOOL bUpdateView)
{
	if (IsEditingSubject() && m_pSubjectEditor)
	{
		if (!XTP_SAFE_GET2(GetViewGroup(), GetViewDay(), GetView(), NULL))
		{
			ASSERT(FALSE);
			return;
		}

		CRect rcEvents = GetViewGroup()->m_LayoutX.m_rcDayDetails;

		m_pSubjectEditor->MoveWindow(&m_rcSubjectEditor, FALSE);

		if (!IsMultidayEvent())
		{
			if (m_rcSubjectEditor.top >= rcEvents.top && m_rcSubjectEditor.top < rcEvents.bottom)
			{
				m_pSubjectEditor->ShowWindow(SW_SHOW);
				m_pSubjectEditor->SetFocus();
			}
			else
			{
				if (bUpdateView && GetViewGroup()->GetViewDay()->GetView()->IsEditingSubject())
				{
					GetViewGroup()->GetViewDay()->GetView()->OnUpdateEditingSubject(GetEditingSubjectFromEditor());
				}

				XTP_SAFE_CALL1(GetCalendarControl(), SetFocus());
				XTP_SAFE_CALL1(m_pSubjectEditor, ShowWindow(SW_HIDE));
			}
		}
	}
}

void CXTPCalendarDayViewEvent::AdjustLayout(CDC* pDC, const CRect& rcEventMax, int nEventPlaceNumber)
{
	TBase::AdjustLayout(pDC, rcEventMax, nEventPlaceNumber);

	m_nMultiDayEventFlags = 0;
	m_bHideEventTime = TRUE;

	CXTPCalendarEvent* pEvent = GetEvent();
	if (!pEvent  || !pDC ||
		!XTP_SAFE_GET2(GetViewGroup(), GetViewDay(), GetView(), NULL))
	{
		ASSERT(FALSE);
		return;
	}

	// all-day event
	if (IsMultidayEvent())
	{
		//calculate icons rect

		CString strLoc = GetItemTextLocation();
		CString strEventText = GetItemTextSubject();
		if (strLoc.GetLength() > 0)
		{
			strEventText += _T(" (") + strLoc + _T(")");
		}

		CSize szText = XTP_SAFE_GET5(GetViewGroup(), GetViewDay(), GetView(), GetPaintManager(), GetDayViewEventPart(), GetTextExtent(pDC, strEventText), CSize(0));
		m_szText.cx = szText.cx;
		m_szText.cy = szText.cy;

		AdjustMultidayEvent(rcEventMax, nEventPlaceNumber);

		if (!pEvent->IsAllDayEvent() &&
				m_nMultiDayEventFlags & xtpCalendarMultiDayLast
		  )
		{
			COleDateTimeSpan spDay(1, 0, 0, 0);
			COleDateTime dtDay = XTP_SAFE_GET2(GetViewGroup_(), GetViewDay_(), GetDayDate(), (DATE)0);
			if (dtDay != pEvent->GetEndTime() - spDay)
			{
				CSize szClock = XTP_SAFE_GET4(GetViewGroup(), GetViewDay(), GetView(), GetPaintManager(), GetClockSize(), CSize(0));
				m_rcLastClock.CopyRect(m_rcEvent);
				m_rcLastClock.top -= 1;
				m_rcLastClock.bottom -= 1;
				m_rcLastClock.left = m_rcLastClock.right - szClock.cx;
				m_rcText.right = m_rcLastClock.left;
			}
		}

		return;
	}

	m_nMultiDayEventFlags |= xtpCalendarMultiDayNoMultiDay;

	// non all-day event
	int nPace = GetEventPlacePos();
	int nPaceCount = max(1, GetEventPlaceCount());

	ASSERT(GetEventPlaceCount() >= 1);
	ASSERT(nPace >= 0 && nPace < nPaceCount);

	m_rcEvent.CopyRect(GetViewGroup()->m_LayoutX.m_rcDayDetails);
	int nEventWidth = m_rcEvent.Width() / nPaceCount;

	// skip event views if they are not fit even by 5 pixel bar width
	// and correct some first event views heights
	if (nEventWidth <= 5)
	{
		while (nPaceCount > 1 && nEventWidth <= 5)
		{
			nPaceCount--;
			nEventWidth = m_rcEvent.Width() / nPaceCount;
		}
		if (nPace > nPaceCount)
		{
			m_rcEvent.SetRectEmpty();
			return;
		}
	}

	// calc other rect coordinates
	m_rcEvent.left += nEventWidth * nPace;
	m_rcEvent.right = m_rcEvent.left + nEventWidth;

	COleDateTime dtStartTime(pEvent->GetStartTime());
	COleDateTime dtEndTime(pEvent->GetEndTime());
	COleDateTimeSpan spEnd(0.0);
	if (CXTPCalendarUtils::IsZeroTime(dtEndTime) &&
		!CXTPCalendarUtils::IsEqual(dtStartTime, dtEndTime))
	{
		spEnd.SetDateTimeSpan(0, 0, 0, 1);
	}
	int nTopCell = XTP_SAFE_GET1(GetViewGroup(), GetDateTimeCell(dtStartTime, FALSE), 0);
	int nBottomCell = XTP_SAFE_GET1(GetViewGroup(), GetDateTimeCell(dtEndTime - spEnd, TRUE), 0);

	// is event's Begin/End on cells borders
	COleDateTime dtStartCellTime = GetViewGroup()->GetViewDay()->GetView()->GetCellTime(nTopCell);
	COleDateTime dtEndCellTime = GetViewGroup()->GetViewDay()->GetView()->GetCellTime(nBottomCell);
	dtEndCellTime += GetViewGroup()->GetViewDay()->GetView()->GetCellDuration();

	COleDateTime dtStart_TimeOnly = CXTPCalendarUtils::ResetDate(dtStartTime);
	COleDateTime dtEnd_TimeOnly = CXTPCalendarUtils::ResetDate(dtEndTime - spEnd);
	dtEnd_TimeOnly += spEnd;

	m_bHideEventTime = CXTPCalendarUtils::IsEqual(dtStartCellTime, dtStart_TimeOnly) &&
						(CXTPCalendarUtils::IsEqual(dtEndCellTime, dtEnd_TimeOnly, TRUE) ||
						 CXTPCalendarUtils::IsEqual(dtStartCellTime, dtEnd_TimeOnly) );

	CXTPCalendarEventPtr ptrDragEventOrig(GetViewGroup()->GetViewDay()->GetView()->GetDraggingEventOrig(), TRUE);

	if (ptrDragEventOrig && ptrDragEventOrig->IsEqualIDs(GetEvent()) &&
		!ptrDragEventOrig->IsEqualStartEnd(GetEvent()))
	{
		m_bHideEventTime = TRUE;
	}
	//---------------------------------------------------------------------------

	if (nBottomCell < nTopCell)
	{
		if (pEvent->GetEventPeriodDays() <= 1)
		{
			nBottomCell = nTopCell;
		}
		else
			if (pEvent->GetEndTime().GetDay() != GetViewGroup()->GetCellDateTime(0).GetDay())
			{
				nBottomCell = GetViewGroup()->GetRowCount() - 1;
			}
			else
			{
				nTopCell = 0;
			}
	}
	ASSERT(nBottomCell >= nTopCell);

	CRect rcTopCell = GetViewGroup()->GetCellRect(nTopCell);
	CRect rcBottomCell = GetViewGroup()->GetCellRect(nBottomCell);

	m_rcEvent.top = rcTopCell.top-1;
	m_rcEvent.bottom = rcBottomCell.bottom-2;
	m_rcEvent.right -= 5;

	m_rcEventMax.top = max(m_rcEvent.top-5, GetViewGroup()->m_LayoutX.m_rcDayDetails.top);
	m_rcEventMax.bottom = min(m_rcEvent.bottom + 5, GetViewGroup()->m_LayoutX.m_rcDayDetails.bottom);

	// adjust subject area
	m_rcText.CopyRect(m_rcEvent);
	m_rcText.left += 10; // correct to busy area
	m_rcText.left += CalcIconsRect(m_rcText);
	m_rcText.left += 4;

	m_rcSubjectEditor.CopyRect(&m_rcEvent);
	m_rcSubjectEditor.top += 1;
	m_rcSubjectEditor.left += 8;
	m_rcSubjectEditor.right -= 1;

	// Calculate Real time frame rect
	double dSecondsPerPixel = GetViewGroup()->GetViewDay()->GetView()->GetCellDuration().GetTotalSeconds()
			/ max(1, GetViewGroup()->GetRowHeight());
	COleDateTime dtTopTime = GetViewGroup()->GetCellDateTime(nTopCell);
	COleDateTime dtBottomTime = GetViewGroup()->GetCellDateTime(nBottomCell)
			+ GetViewGroup()->GetViewDay()->GetView()->GetCellDuration();

	if (CXTPCalendarUtils::IsEqual(dtStartTime, dtEndTime))
	{
		m_rcTimeframe.top = m_rcTimeframe.bottom = LONG_MAX;
	}
	else
	{
		double dShift = max(0, (dtStartTime - dtTopTime).GetTotalSeconds());
		dShift /= dSecondsPerPixel;
		m_rcTimeframe.top = m_rcEvent.top + (LONG)dShift;

		dShift = max(0, (dtBottomTime - dtEndTime).GetTotalSeconds());
		dShift = min(m_rcEvent.Height(), dShift/dSecondsPerPixel);
		m_rcTimeframe.bottom = m_rcEvent.bottom - (LONG)dShift;
	}

	m_rcTimeframe.left = m_rcEvent.left + 1;
	m_rcTimeframe.right = m_rcTimeframe.left + 4;

	// set expanded signs for day view
	CRect rcEvents = GetViewGroup()->m_LayoutX.m_rcDayDetails;

	if (m_rcEvent.top >= rcEvents.bottom-10)
		GetViewGroup()->SetExpandDown();
	if (m_rcEvent.bottom <= rcEvents.top + 10)
		GetViewGroup()->SetExpandUp();
}

void CXTPCalendarDayViewEvent::AdjustLayout2(CDC* pDC, const CRect& rcEventMax, int nEventPlaceNumber)
{
	TBase::AdjustLayout(pDC, rcEventMax, nEventPlaceNumber);

	m_nMultiDayEventFlags = 0;
	m_bHideEventTime = TRUE;

	CXTPCalendarEvent* pEvent = GetEvent();
	if (!pEvent  || !pDC ||
		!XTP_SAFE_GET2(GetViewGroup(), GetViewDay(), GetView(), NULL) ||
		!XTP_SAFE_GET3(GetViewGroup(), GetViewDay(), GetView(), GetTheme(), NULL) )
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarTheme* pTheme = GetViewGroup()->GetViewDay()->GetView()->GetTheme();

	// all-day event
	if (IsMultidayEvent())
	{
		// to calculate multiday-flags
		AdjustMultidayEvent(rcEventMax, nEventPlaceNumber);

		pTheme->GetDayViewPart()->GetDayPart()->GetGroupPart()->GetMultiDayEventPart()->AdjustLayout(this, pDC, rcEventMax, nEventPlaceNumber);
		return;

	}

	pTheme->GetDayViewPart()->GetDayPart()->GetGroupPart()->GetSingleDayEventPart()->AdjustLayout(this, pDC, rcEventMax, nEventPlaceNumber);

}

void CXTPCalendarDayViewEvent::OnPostAdjustLayout()
{
	CXTPCalendarViewEvent::OnPostAdjustLayout();

	CRect rcEvents = GetEventsRect();

	if (m_rcToolTip.top < rcEvents.top)
	{
		m_rcToolTip.top = rcEvents.top;

		if (m_rcToolTip.top >= m_rcToolTip.bottom)
		{
			m_rcToolTip.bottom = m_rcToolTip.top + 10;
		}
	}
}

CRect CXTPCalendarDayViewEvent::GetEventsRect()
{
	if (!GetViewGroup() || !GetViewGroup()->GetViewDay())
	{
		ASSERT(FALSE);
		return CRect(0, 0, 0, 0);
	}
	if (IsMultidayEvent())
	{
		return GetViewGroup()->m_LayoutX.m_rcAllDayEvents;
	}
	return GetViewGroup()->m_LayoutX.m_rcDayDetails;
}

BOOL CXTPCalendarDayViewEvent::IsVisible()
{
	return IsVisibleEx(TRUE, TRUE, FALSE);
}

BOOL CXTPCalendarDayViewEvent::IsVisibleEx(BOOL bCheckTop, BOOL bCheckBottom,
		BOOL bTopAndBottom)
{
	// at least one of bCheckTop or bCheckBottom must be TRUE (or both)
	ASSERT((!bCheckTop && !bCheckBottom) == FALSE);

	if (!GetViewGroup() || !GetViewGroup()->GetViewDay())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bVisible = FALSE;

	if (IsMultidayEvent())
	{
		CRect rcEvents = GetViewGroup()->m_LayoutX.m_rcAllDayEvents;
		bVisible = m_rcEvent.bottom <= rcEvents.bottom;
	}
	else
	{
		CRect rcEvents = GetViewGroup()->m_LayoutX.m_rcDayDetails;
		BOOL bETopVisible = m_rcEvent.top <= rcEvents.bottom &&
							m_rcEvent.top >= rcEvents.top-2;

		BOOL bEBottomVisible = m_rcEvent.bottom <= rcEvents.bottom &&
							   m_rcEvent.bottom >= rcEvents.top-2;

		BOOL bEBodyVisible = !(m_rcEvent.top < rcEvents.top-2 &&
								m_rcEvent.bottom < rcEvents.top-2
								||
								m_rcEvent.top > rcEvents.bottom &&
								m_rcEvent.bottom > rcEvents.bottom);

		if (bTopAndBottom)
		{
			bETopVisible |= !bCheckTop;
			bEBottomVisible |= !bCheckBottom;

			bVisible = bETopVisible && bEBottomVisible;
		}
		else
		{
			bVisible = bETopVisible || bEBottomVisible || bEBodyVisible;
		}

	}
	return bVisible;
}

void CXTPCalendarDayViewEvent::ChangeEventPlace(int nNewPlace)
{
	CXTPCalendarTheme* pTheme = XTP_SAFE_GET3(GetViewGroup_(), GetViewDay_(), GetView_(), GetTheme(), NULL);

	if (pTheme)
	{
		CRect rcEventNew = m_rcEvent;
		pTheme->GetDayViewPart()->GetDayPart()->GetGroupPart()->GetMultiDayEventPart()->CalcEventYs(rcEventNew, nNewPlace);

		int nY0 = m_rcEvent.top;
		int nY1 = rcEventNew.top;

		_ChangeEventPlace(nNewPlace, nY0, nY1);
	}
	else
	{
		TBase::ChangeEventPlace(nNewPlace);
	}
}

void CXTPCalendarDayViewEvent::Draw(CDC* pDC)
{
	BOOL bNoVisibleMDEvent = IsMultidayEvent() && !IsVisible();

	if (m_nMultiDayEventFlags & xtpCalendarMultiDaySlave || bNoVisibleMDEvent)
	{
		return;// was drawn in the Master (or out of events rect)
	}
	//ASSERT((m_nMultiDayEventFlags & xtpCalendarMultiDayMaster) || m_nMultiDayEventFlags == xtpCalendarMultiDayNoMultiDay);

	XTP_SAFE_CALL5(GetViewGroup(), GetViewDay(), GetView(), GetPaintManager(), GetDayViewEventPart(), OnDraw(pDC, this) );
}

BOOL CXTPCalendarDayViewEvent::HitTestEx(CPoint point, XTP_CALENDAR_HITTESTINFO_DAY_VIEW* pInfo)
{
	if (!pInfo || !GetViewGroup() || !GetViewGroup()->GetViewDay())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CRect rcEvents = GetEventsRect();

	CRect rcEventMax2(m_rcEventMax);

	if (IsSelected() && !IsMultidayEvent())
	{
		rcEventMax2.top = rcEventMax2.top - 5;
		rcEventMax2.bottom = rcEventMax2.bottom + 5;
	}

	BOOL bVisible = IsVisibleEx(TRUE, TRUE, FALSE);
	if (!rcEventMax2.PtInRect(point) || !rcEvents.PtInRect(point) || !bVisible)
	{
		return FALSE;
	}
	//-----------------------------------------
	XTP_CALENDAR_HITTESTINFO_DAY_VIEW tmpInfo, tmpInfo2;
	tmpInfo.pt = tmpInfo2.pt = point;

	tmpInfo.pViewDay = GetViewGroup()->GetViewDay();
	tmpInfo.pViewGroup = GetViewGroup();
	tmpInfo.pViewEvent = this;
	tmpInfo.uHitCode = xtpCalendarHitTestUnknown;

	if (GetViewGroup()->HitTestDateTime(&tmpInfo2))
	{
		tmpInfo.dt = tmpInfo2.dt;
		tmpInfo.bTimePartValid = tmpInfo2.bTimePartValid;
		tmpInfo.uHitCode |= (tmpInfo2.uHitCode & xtpCalendarHitTestDayView_Mask);
	}
	else
	{
		tmpInfo.dt = GetViewGroup()->GetViewDay()->GetDayDate();
		tmpInfo.bTimePartValid = FALSE;
	}
	//-----------------------------------------

	if (IsMultidayEvent())
	{
		BOOL bIsFirst = (m_nMultiDayEventFlags & xtpCalendarMultiDayFirst);
		BOOL bIsLast = (m_nMultiDayEventFlags & xtpCalendarMultiDayLast);
		//BOOL bIsMiddle = (m_nMultiDayEventFlags & xtpCalendarMultiDayMiddle);

		CRect rcBegin = m_rcEventMax;
		rcBegin.right = min(m_rcEvent.left + 5, m_rcEvent.right);

		CRect rcEnd = m_rcEventMax;
		rcEnd.left = max(m_rcEvent.right - 5, m_rcEvent.left);

		if (bIsFirst && rcBegin.PtInRect(point))
		{
			tmpInfo.uHitCode = xtpCalendarHitTestEventResizeHArea | xtpCalendarHitTestEventResizeBegin;
			*pInfo = tmpInfo;
			return TRUE;
		}

		if (bIsLast && rcEnd.PtInRect(point))
		{
			tmpInfo.uHitCode = xtpCalendarHitTestEventResizeHArea | xtpCalendarHitTestEventResizeEnd;
			*pInfo = tmpInfo;
			return TRUE;
		}

		if (m_rcEvent.PtInRect(point))
		{
			tmpInfo.uHitCode = xtpCalendarHitTestEventTextArea;
			*pInfo = tmpInfo;
			return TRUE;
		}
		return FALSE;
	}

	CRect rcTime = m_rcEvent;
	rcTime.right = m_rcTimeframe.right + 1;

	CRect rcText = m_rcEvent;
	rcText.left = rcTime.right;

	CRect rcBegin = m_rcEvent;
	rcBegin.bottom = min(m_rcEvent.top + 5, m_rcEvent.bottom);

	CRect rcEnd = m_rcEvent;
	rcEnd.top = max(m_rcEvent.bottom - 5, m_rcEvent.top);

	if (IsSelected())
	{
		rcBegin.top = m_rcEvent.top - 5;
		rcEnd.bottom = m_rcEvent.bottom + 5;
	}

	if (rcTime.PtInRect(point))
	{
		tmpInfo.uHitCode = xtpCalendarHitTestEventDragArea;
		*pInfo = tmpInfo;
		return TRUE;
	}

	if (rcBegin.PtInRect(point))
	{
		tmpInfo.uHitCode = xtpCalendarHitTestEventResizeVArea | xtpCalendarHitTestEventResizeBegin;
		*pInfo = tmpInfo;
		return TRUE;
	}

	if (rcEnd.PtInRect(point))
	{
		tmpInfo.uHitCode = xtpCalendarHitTestEventResizeVArea | xtpCalendarHitTestEventResizeEnd;
		*pInfo = tmpInfo;
		return TRUE;
	}

	if (rcText.PtInRect(point))
	{
		tmpInfo.uHitCode = xtpCalendarHitTestEventTextArea;
		*pInfo = tmpInfo;
		return TRUE;
	}

	//-----------------------------------------
	return FALSE;
}

CFont* CXTPCalendarDayViewEvent::GetSubjectEditorFont()
{
	CFont* pFont = &XTP_SAFE_GET5(GetViewGroup(), GetViewDay(), GetView(), GetPaintManager(), GetDayViewEventPart(), GetTextFont(), *((CFont*)(NULL)));
	ASSERT(pFont);
	return pFont;
}

CXTPCalendarViewEvent* CXTPCalendarDayViewEvent::StartEditSubject()
{
	XTP_SAFE_CALL3(GetViewGroup(), GetViewDay(), GetView(), EnsureVisibleH(this));

	CXTPCalendarViewEvent* pEV = TBase::StartEditSubject();

	AdjustSubjectEditorEx(FALSE);

	return pEV;
}
