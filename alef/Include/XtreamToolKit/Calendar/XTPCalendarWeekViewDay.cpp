// XTPCalendarMonthViewDay.cpp: implementation of the CXTPCalendarMonthViewDay class.
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
#include "Resource.h"

#include "XTPCalendarWeekViewDay.h"
#include "XTPCalendarWeekView.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarPaintManager.h"
#include "XTPCalendarTheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CXTPCalendarWeekViewGroup::CXTPCalendarWeekViewGroup(CXTPCalendarWeekViewDay* pViewDay)
	: TBase(pViewDay)
{

}
CXTPCalendarWeekViewGroup::~CXTPCalendarWeekViewGroup()
{
}

void CXTPCalendarWeekViewGroup::FillHitTestEx(XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pHitTest)
{
	ASSERT(pHitTest && GetViewDay());
	if (pHitTest && GetViewDay())
	{
		GetViewDay()->FillHitTestEx(pHitTest);
		pHitTest->pViewGroup = this;
	}
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarWeekViewDay, CXTPCalendarViewDay)

CXTPCalendarWeekViewDay::CXTPCalendarWeekViewDay(CXTPCalendarWeekView* pWeekView)
		: TBase(pWeekView)
{}

CXTPCalendarWeekViewDay::~CXTPCalendarWeekViewDay()
{
}

void CXTPCalendarWeekViewDay::FillHitTestEx(XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pInfo)
{
	ASSERT(pInfo);
	if (!pInfo)
	{
		return;
	}
	pInfo->dt = m_dtDate;
	pInfo->pViewDay = GetPThis();
	pInfo->uHitCode = xtpCalendarHitTestEvent_Mask;
}

BOOL CXTPCalendarWeekViewDay::HitTestEx(CPoint pt, XTP_CALENDAR_HITTESTINFO_WEEK_VIEW* pHitTest)
{
	if (!pHitTest) {
		ASSERT(FALSE);
		return FALSE;
	}

	if (m_Layout.m_rcExpandSign.PtInRect(pt))
	{
		if (XTP_SAFE_GET4(GetView(), GetTheme(), GetWeekViewPart(), GetDayPart(),
			HitTestExpandDayButton(this, &pt), 0))
		{
			FillHitTestEx(pHitTest);
			pHitTest->uHitCode = xtpCalendarHitTestDayExpandButton;
			return TRUE;
		}
	}

	return TBase::HitTestEx(pt, pHitTest);
}

void CXTPCalendarWeekViewDay::AdjustLayout(CDC* pDC, const CRect& rcDay)
{
	if (!GetView() || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	int nHeaderHeight = GetView()->GetDayHeaderHeight();

	m_Layout.m_rcDay.CopyRect(&rcDay);
	m_Layout.m_rcDayHeader.CopyRect(&rcDay);
	m_Layout.m_rcDayHeader.bottom = m_Layout.m_rcDayHeader.top + nHeaderHeight;
	m_Layout.m_rcDayHeader.DeflateRect(1, 1, 1, 0);

	CRect rcDayEvents = rcDay;
	rcDayEvents.DeflateRect(1, nHeaderHeight, 1, 1);

	//-------------------------------------
	int nGroups = GetViewGroupsCount();
	ASSERT(nGroups == 1);

	CXTPCalendarViewGroup* pViewGroup = nGroups ? GetViewGroup_(0) : NULL;
	ASSERT(pViewGroup);

	if (pViewGroup)
	{
		pViewGroup->AdjustLayout(pDC, rcDayEvents);
	}
}
void CXTPCalendarWeekViewDay::AdjustLayout2(CDC* pDC, const CRect& rcDay)
{
	TBase::AdjustLayout(pDC, rcDay);

	XTP_SAFE_CALL4(GetView(), GetTheme(), GetWeekViewPart(), GetDayPart(), AdjustLayout(this, pDC, rcDay));
}

void CXTPCalendarWeekViewDay::Draw(CDC* pDC)
{
	//-- Draw Events (Group) -----------------
	int nGroups = GetViewGroupsCount();
	ASSERT(nGroups == 1);

	CXTPCalendarViewGroup* pViewGroup = nGroups ? GetViewGroup_(0) : NULL;
	ASSERT(pViewGroup);

	if (pViewGroup)
	{
		pViewGroup->Draw(pDC);
	}

	if (NoAllEventsAreVisible())
	{
		XTP_SAFE_CALL2(GetView(), GetPaintManager(),
			DrawBitmap(XTP_IDB_CALENDAR_EXPANDSIGNDOWN, pDC, GetExpandSignRect()) );
	}
}

CRect CXTPCalendarWeekViewDay::GetDayEventsRect() const
{
	int nColHeaderHeight = XTP_SAFE_GET1(GetView(), GetRowHeight(), 0);
	CRect rcDayEvents = m_Layout.m_rcDay;
	//rcDayEvents.bottom -= min(2, rcDayEvents.Height());
	rcDayEvents.top += min(nColHeaderHeight + 0, rcDayEvents.Height());
	int nBotSpace = XTP_SAFE_GET5(GetView(), GetCalendarControl(), GetTheme(),
		GetWeekViewPart(), GetDayPart(), GetExpandButtonHeight(), 0);

	rcDayEvents.bottom -= min(nBotSpace + 2, rcDayEvents.Height());


	return rcDayEvents;
}

BOOL CXTPCalendarWeekViewDay::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!GetView() || !GetCalendarControl())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (GetView()->GetTheme())
	{
		if (XTP_SAFE_GET4(GetView(), GetTheme(), GetWeekViewPart(), GetDayPart(),
			OnLButtonDown(this, nFlags, point), FALSE))
		{
			return TRUE;
		}
	}
	else if (m_Layout.m_rcExpandSign.PtInRect(point))
	{
		if (UserAction_OnExpandDay(xtpCalendarExpandDayButton_WeekView))
			return TRUE;

		XTP_SAFE_CALL1(GetCalendarControl(), QueueDayViewSwitch(GetDayDate()));
		return TRUE;
	}

	return TBase::OnLButtonDown(nFlags, point);
}

void CXTPCalendarWeekViewDay::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetView() && GetView()->GetTheme() && GetView()->GetTheme()->GetWeekViewPart() &&
		GetView()->GetTheme()->GetWeekViewPart()->GetDayPart())
		GetView()->GetTheme()->GetWeekViewPart()->GetDayPart()->OnMouseMove(this, nFlags, point);

	TBase::OnMouseMove(nFlags, point);
}
