// XTPCalendarViewEvent.cpp: implementation of the CXTPCalendarViewEvent class.
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

#include "XTPCalendarViewEvent.h"
#include "XTPCalendarView.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarViewDay.h"
#include "XTPCalendarTheme.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//class CXTPCalendarViewEventSubjectEditor : public CEdit

IMPLEMENT_DYNAMIC(CXTPCalendarViewEvent, CXTPCalendarWMHandler)

BEGIN_MESSAGE_MAP(CXTPCalendarViewEventSubjectEditor, CEdit)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

CXTPCalendarViewEventSubjectEditor::CXTPCalendarViewEventSubjectEditor(CXTPCalendarControl* pOwner)
{
	ASSERT(pOwner);
	m_pOwner = pOwner;
}

CXTPCalendarViewEventSubjectEditor::~CXTPCalendarViewEventSubjectEditor()
{
}

BOOL CXTPCalendarViewEventSubjectEditor::OnWndMsg(UINT message, WPARAM wParam,
												  LPARAM lParam, LRESULT* pResult)
{
	ASSERT(m_pOwner);

	if (m_pOwner)
	{
		if(message == WM_MOUSEMOVE)
		{
			CPoint pt(lParam);
			MapWindowPoints(m_pOwner, &pt, 1);
			lParam = MAKELPARAM(pt.x, pt.y);
		}

		BOOL bHandled = m_pOwner->OnWndMsg_Children(message, wParam, lParam, pResult);
		if (bHandled)
		{
			return TRUE;
		}
	}
	return CEdit::OnWndMsg(message, wParam, lParam, pResult);
}

UINT CXTPCalendarViewEventSubjectEditor::OnGetDlgCode()
{
	return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTALLKEYS;
}

//////////////////////////////////////////////////////////////////////
CXTPCalendarViewEvent::CXTPCalendarViewEvent(CXTPCalendarEvent* pEvent, CXTPCalendarViewGroup* pViewGroup)
{
	UNUSED_ALWAYS(pViewGroup);

	m_bShowingToolTip = FALSE;

	ASSERT(pEvent);
	m_ptrEvent = pEvent;
	CMDTARGET_ADDREF(m_ptrEvent);

	m_pMasterView = 0;

	m_bSelected = FALSE;

	m_nMultiDayEventFlags = xtpCalendarMultiDayUnknown;

	m_nEventPlaceNumber = 0;

	m_nTimerID_ShowToolTip = 0;

	m_pSubjectEditor = NULL;

	//m_rcFirstClock.SetRect(0, 0, 0, 0);
	//m_rcLastClock.SetRect(0, 0, 0, 0);
	//m_rcStartTime.SetRect(0, 0, 0, 0);
	//m_rcEndTime.SetRect(0, 0, 0, 0);
	//  m_rcGlyphs.SetRect(0, 0, 0, 0);
	m_rcText.SetRect(0, 0, 0, 0);
	m_szText.cx = 0;
	m_szText.cy = 0;
	m_bMultyDayTextAlign = FALSE;

	m_bTextOutOfBorders = TRUE;

}

CXTPCalendarViewEvent::~CXTPCalendarViewEvent()
{
	//---------------------------------------------------------------------------
	if (m_pSubjectEditor)
	{
		ASSERT(FALSE);
		delete m_pSubjectEditor;
		m_pSubjectEditor = NULL;
	}

	CMDTARGET_RELEASE(m_ptrEvent);
	CMDTARGET_RELEASE(m_pMasterView);

	ASSERT(m_nTimerID_ShowToolTip == 0);
}

void CXTPCalendarViewEvent::OnBeforeDestroy()
{
	//---------------------------------------------------------------------------

	if (IsEditingSubject())
	{
		XTP_SAFE_CALL3(GetViewGroup_(), GetViewDay_(), GetView_(), EndEditSubject(xtpCalendarEditSubjectCommit, FALSE) );
	}
	//---------------------------------------------------------------------------
	if (m_nTimerID_ShowToolTip)
	{
		KillTimer(m_nTimerID_ShowToolTip);
		m_nTimerID_ShowToolTip = 0;
	}

	if (IsShowingToolTip())
	{
		HideToolTip();
	}
}

CXTPCalendarControl* CXTPCalendarViewEvent::GetCalendarControl()
{
	return XTP_SAFE_GET2(GetViewGroup_(), GetViewDay_(), GetCalendarControl(), NULL);
}

void CXTPCalendarViewEvent::AdjustLayout(CDC* pDC, const CRect& rcEventMax, int nEventPlaceNumber)
{
	UNREFERENCED_PARAMETER(pDC);

	m_rcEvent.CopyRect(&rcEventMax);
	m_rcEventMax.CopyRect(&rcEventMax);

	m_rcSubjectEditor.CopyRect(&rcEventMax);
	m_rcToolTip.CopyRect(&rcEventMax);
	m_rcText.CopyRect(&rcEventMax);
	m_rcStartTime.CopyRect(m_rcEvent);
	m_rcStartTime.DeflateRect(0, 0, m_rcStartTime.Width(), m_rcStartTime.Height());
	m_rcEndTime.CopyRect(m_rcStartTime);

	m_nEventPlaceNumber = nEventPlaceNumber;
}

void CXTPCalendarViewEvent::OnMouseMove(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);

	CXTPCalendarView* pView =  XTP_SAFE_GET2(GetViewGroup_(), GetViewDay_(), GetView_(), NULL);

	if (!pView || !GetCalendarControl())
	{
		ASSERT(FALSE);
		return;
	}

	XTP_CALENDAR_HITTESTINFO hitInfo;
	BOOL bIsNoDragging = pView->GetDraggingMode() == xtpCalendaDragModeUnknown;
	BOOL bHideTooltip = TRUE;

	BOOL bHit = HitTest(point, &hitInfo);

	// to check that mouse is not over some topmost element (like Expand/Scroll button)
	if (bHit)
	{
		//bHit = XTP_SAFE_GET2(GetViewGroup_(), GetViewDay_(), HitTest(point, &hitInfo), FALSE) &&
		bHit = XTP_SAFE_GET3(GetViewGroup_(), GetViewDay_(), GetView_(), HitTest(point, &hitInfo), FALSE) &&
				((hitInfo.uHitCode & xtpCalendarHitTestEvent_Mask) && hitInfo.pViewEvent == this);
	}

	if (bHit)
	{
		int eDragMode = pView->_GetDraggingMode(&hitInfo);
		BOOL bDragEnabled = !pView->IsEditOperationDisabledNotify(pView->_DragMod2Operation(eDragMode), this);

		if (bDragEnabled && hitInfo.uHitCode & xtpCalendarHitTestEventResizeHArea)
		{
			GetCalendarControl()->m_mouseMode = xtpCalendarMouseEventPreResizeH;
		}
		else if (bDragEnabled && hitInfo.uHitCode & xtpCalendarHitTestEventResizeVArea)
		{
			GetCalendarControl()->m_mouseMode = xtpCalendarMouseEventPreResizeV;
		}
		else if (bDragEnabled && hitInfo.uHitCode & xtpCalendarHitTestEventDragArea)
		{
			GetCalendarControl()->m_mouseMode = xtpCalendarMouseEventPreDrag;
		}
		else if (hitInfo.uHitCode & xtpCalendarHitTestEventTextArea)
		{
			bHideTooltip = !!(nFlags & MK_LBUTTON);
			BOOL bShow = !IsMultidayEvent() || XTP_SAFE_GET2(pView, GetResources(), GetCount(), 0) > 1;

			if (bShow && bIsNoDragging && !IsEditingSubject() && !pView->IsEditingSubject() &&
				!IsShowingToolTip() && !(nFlags&MK_LBUTTON) && m_bTextOutOfBorders)
			{
				if (m_nTimerID_ShowToolTip == 0)
				{
					UINT uTimeOut_ms = GetShowToolTipTimeOut();
					m_nTimerID_ShowToolTip = SetTimer(uTimeOut_ms);
					ASSERT(m_nTimerID_ShowToolTip);

					DBG_TRACE_TIMER(_T("SET Timer: ID =%d, m_nTimerID_ShowToolTip, CXTPCalendarViewEvent::OnMouseMove() \n"), m_nTimerID_ShowToolTip);
				}
			}
		}
	}
	else
	{
		if (m_nTimerID_ShowToolTip)
		{
			KillTimer(m_nTimerID_ShowToolTip);
			m_nTimerID_ShowToolTip = 0;
		}
	}

	if (bHideTooltip && IsShowingToolTip())
	{
		HideToolTip();
	}
}

//void CXTPCalendarViewEvent::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//  UNREFERENCED_PARAMETER(nChar);
//  UNREFERENCED_PARAMETER(nRepCnt);
//  UNREFERENCED_PARAMETER(nFlags);
//}
//
//void CXTPCalendarViewEvent::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//  UNREFERENCED_PARAMETER(nChar);
//  UNREFERENCED_PARAMETER(nRepCnt);
//  UNREFERENCED_PARAMETER(nFlags);
//}
//
//BOOL CXTPCalendarViewEvent::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//  UNREFERENCED_PARAMETER(nChar);
//  UNREFERENCED_PARAMETER(nRepCnt);
//  UNREFERENCED_PARAMETER(nFlags);
//  return FALSE;
//}


void CXTPCalendarViewEvent::AddMultiDayEvent_Slave(CXTPCalendarViewEvent* pSlaveView, int nMasterEventPlace)
{
	if (!pSlaveView || !pSlaveView->GetViewGroup_() || !GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	pSlaveView->GetViewGroup_()->ExchangeEventPlace(pSlaveView, nMasterEventPlace);
	pSlaveView->SetMultiDayEvent_MasterSlave(FALSE, this);

	if (!pSlaveView->IsVisible())
	{
		return;
	}

//  ASSERT(m_rcEvent.top == pSlaveView->m_rcEvent.top);
//  ASSERT(m_rcEventMax.bottom == pSlaveView->m_rcEventMax.bottom);

	m_rcEvent.right = pSlaveView->m_rcEvent.right;
	m_rcEventMax.right = pSlaveView->m_rcEventMax.right;

	// correct right margins
	m_rcText.right = m_rcEvent.right;
	m_rcLastClock.CopyRect(m_rcText);
	m_rcLastClock.left = m_rcLastClock.right;
	m_rcLastClock.bottom = m_rcLastClock.top;

	//pSlaveView->SetMultiDayEvent_MasterSlave(FALSE, this);

	int nMDEFlags = pSlaveView->GetMultiDayEventFlags();

	if (nMDEFlags & xtpCalendarMultiDayLast)
	{
		m_nMultiDayEventFlags |= xtpCalendarMultiDayLast;
		COleDateTime dtEE(GetEvent()->GetEndTime());
		if (dtEE == CXTPCalendarUtils::ResetTime(dtEE) || GetEvent()->IsAllDayEvent())
		{
			m_rcLastClock.left = m_rcLastClock.right;
		}
		else
		{
			// correct last clock rect
			CSize szClock = XTP_SAFE_GET4(GetViewGroup_(), GetViewDay_(), GetView_(), GetPaintManager(), GetClockSize(), CSize(0) );
			m_rcLastClock.left = m_rcLastClock.right - szClock.cx -1;
			m_rcLastClock.bottom = m_rcLastClock.top + szClock.cy;
			m_rcText.right -= szClock.cx;
		}
	}
}

BOOL CXTPCalendarViewEvent::IsMultidayEvent()
{
	if (!m_ptrEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (m_ptrEvent->IsAllDayEvent())
	{
		return TRUE;
	}

	int nDays = m_ptrEvent->GetEventPeriodDays();
	return nDays > 1;
}

void CXTPCalendarViewEvent::OnPostAdjustLayout()
{
	if (IsMultidayEvent())
	{
		m_rcSubjectEditor.CopyRect(&m_rcEvent);
		m_rcSubjectEditor.left += 1;

		m_rcText.CopyRect(m_rcEvent);
		m_rcText.left += min(m_rcText.Width()-1, m_rcFirstClock.Width() + 4);
		m_rcText.right -= min(m_rcText.Width()-1, m_rcLastClock.Width() + 4);

		int nGlyphs = CalcIconsRect(m_rcEvent);
		CRect rcGlyphs(max(m_rcText.CenterPoint().x - (m_szText.cx/2 + nGlyphs + 3), m_rcText.left),
					   m_rcText.top,
					   m_rcText.right,
					   m_rcText.bottom
					 );
		if (m_rcText.CenterPoint().x <= m_rcText.left + nGlyphs + m_szText.cx/2) //
		{
			m_rcText.left = min(m_rcText.left + CalcIconsRect(rcGlyphs), m_rcText.right);

			m_bMultyDayTextAlign = TRUE;
		}
		else
		{
			m_bMultyDayTextAlign = FALSE;
			CalcIconsRect(rcGlyphs);
		}
	}

	m_rcToolTip = m_rcEvent;

	AdjustSubjectEditor();
}

void CXTPCalendarViewEvent::AdjustMultidayEvent(CRect rcEventMax, int nEventPlaceNumber)
{
	if (!XTP_SAFE_GET3(GetViewGroup_(), GetViewDay_(), GetView_(), GetPaintManager(), NULL)
		|| !m_ptrEvent)
	{
		ASSERT(FALSE);
		return;
	}

	int nRowHeight = GetViewGroup_()->GetViewDay_()->GetView_()->GetRowHeight();

	m_nMultiDayEventFlags = 0;

	m_rcEventMax.top = rcEventMax.top + nRowHeight * nEventPlaceNumber + 1;
	m_rcEventMax.bottom = m_rcEventMax.top + nRowHeight - 2;

	m_rcEvent.CopyRect(&m_rcEventMax);
	m_rcEvent.left += 4;
	m_rcEvent.right -= 4;

	if (!IsMultidayEvent())
	{
		m_nMultiDayEventFlags = xtpCalendarMultiDayNoMultiDay;
		return;
	}

	COleDateTime dtDay = GetViewGroup_()->GetViewDay_()->GetDayDate();

	COleDateTime dtStartDay = CXTPCalendarUtils::ResetTime(m_ptrEvent->GetStartTime());
	COleDateTime dtEndDay = CXTPCalendarUtils::RetriveEventEndDate(m_ptrEvent);

	//TRACE(dtStartDay.Format(_T("%A, %B %d, %Y %H %M %s\n")));
	//TRACE(dtEndDay.Format(_T("%A, %B %d, %Y %H %M %s\n")));
	ASSERT(dtDay == dtStartDay || dtDay == dtEndDay || dtDay > dtStartDay && dtDay < dtEndDay);

	m_rcText.CopyRect(m_rcEvent);
	m_rcFirstClock.CopyRect(m_rcEvent);
	m_rcFirstClock.right = m_rcFirstClock.left;
	m_rcFirstClock.bottom = m_rcFirstClock.top;
	CSize szClock(GetViewGroup_()->GetViewDay_()->GetView_()->GetPaintManager()->GetClockSize());

	if (dtDay == dtStartDay)
	{
		m_nMultiDayEventFlags |= xtpCalendarMultiDayFirst;
		if (!m_ptrEvent->IsAllDayEvent() && dtDay != m_ptrEvent->GetStartTime())
		{
			m_rcFirstClock.right = m_rcFirstClock.left + szClock.cx;
			m_rcFirstClock.bottom = m_rcFirstClock.top + szClock.cy;
			m_rcText.left += m_rcFirstClock.right + 4;
		}
	}

	if (dtDay == dtEndDay)
	{
		m_nMultiDayEventFlags |= xtpCalendarMultiDayLast;
	}

	COleDateTimeSpan spDay(1, 0, 0, 0);
	if (dtDay == m_ptrEvent->GetEndTime() - spDay)
	{
		m_nMultiDayEventFlags |= xtpCalendarMultiDayLast;
	}
	else if (dtDay > dtStartDay && dtDay < dtEndDay)
	{
		m_nMultiDayEventFlags |= xtpCalendarMultiDayMiddle;
	}

	//m_rcGlyphs.CopyRect(rcEventMax);
	m_rcStartTime.CopyRect(m_rcEventMax);
	m_rcStartTime.right = m_rcStartTime.left;
	m_rcEndTime.CopyRect(m_rcEventMax);
	m_rcStartTime.left = m_rcStartTime.right;
}

void CXTPCalendarViewEvent::_MoveRectY(int nYbasePrev, int nYbaseNew, CRect& rRC)
{
	int nOffset = rRC.top - nYbasePrev;
	rRC.bottom = rRC.Height() + nYbaseNew + nOffset;
	rRC.top = nYbaseNew + nOffset;
}

void CXTPCalendarViewEvent::ChangeEventPlace(int nNewPlace)
{
	int nEventHeight = XTP_SAFE_GET3(GetViewGroup_(), GetViewDay_(), GetView_(), GetRowHeight(), 1);

	int nY0 = m_rcEvent.top;
	int nEventsTop = m_rcEvent.top - m_nEventPlaceNumber * nEventHeight - 1;
	int nY1 = nEventsTop + nNewPlace * nEventHeight + 1;

	_ChangeEventPlace(nNewPlace, nY0, nY1);
}

void CXTPCalendarViewEvent::_ChangeEventPlace(int nNewPlace, int nYbasePrev, int nYbaseNew)
{
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcFirstClock);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcLastClock);

	_MoveRectY(nYbasePrev, nYbaseNew, m_rcStartTime);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcEndTime);

	_MoveRectY(nYbasePrev, nYbaseNew, m_rcReminderGlyph);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcRecurrenceGlyph);

	_MoveRectY(nYbasePrev, nYbaseNew, m_rcMeetingGlyph);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcPrivateGlyph);

	_MoveRectY(nYbasePrev, nYbaseNew, m_rcEvent);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcEventMax);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcSubjectEditor);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcToolTip);
	_MoveRectY(nYbasePrev, nYbaseNew, m_rcText);

	m_nEventPlaceNumber = nNewPlace;
}

void CXTPCalendarViewEvent::SetMultiDayEvent_MasterSlave(BOOL bMaster, CXTPCalendarViewEvent* pMasterView)
{
	m_nMultiDayEventFlags |= bMaster ? xtpCalendarMultiDayMaster : xtpCalendarMultiDaySlave;

	ASSERT(bMaster && !pMasterView || !bMaster && pMasterView);

	CMDTARGET_RELEASE(m_pMasterView);
	m_pMasterView = pMasterView;
	CMDTARGET_ADDREF(m_pMasterView);
}

UINT CXTPCalendarViewEvent::SetTimer(UINT uTimeOut_ms)
{
	return XTP_SAFE_GET1(GetViewGroup_(), SetTimer(uTimeOut_ms), 0);
}

void CXTPCalendarViewEvent::KillTimer(UINT uTimerID)
{
	XTP_SAFE_CALL1(GetViewGroup_(), KillTimer(uTimerID) );
}

BOOL CXTPCalendarViewEvent::OnTimer(UINT_PTR uTimerID)
{
	if (!uTimerID)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (uTimerID == m_nTimerID_ShowToolTip)
	{
		ShowToolTip();
		return TRUE;
	}
	return FALSE;
}

CXTPCalendarViewEvent* CXTPCalendarViewEvent::StartEditSubject()
{
	//- Forward editing process to master event ---------------------------------
	if (m_pMasterView)
	{
		if (!m_pMasterView->IsEditingSubject())
		{
			m_pMasterView->StartEditSubject();
		}
		return m_pMasterView;
	}

	// ensure that previous one is destroyed.
	if (m_pSubjectEditor)
	{
		ASSERT(FALSE);
		delete m_pSubjectEditor;
		m_pSubjectEditor = NULL;
	}

	CXTPCalendarControl* pControl = GetCalendarControl();
	if (!pControl)
	{
		ASSERT(FALSE);
		return NULL;
	}

	if (IsShowingToolTip())
	{
		HideToolTip();
	}

	// let theme to customize subject editor
	if (pControl->GetTheme())
	{
		m_pSubjectEditor = pControl->GetTheme()->StartEditSubject(this);
		if (m_pSubjectEditor)
		{
			return this;
		}
	}

	//- create and show text editor ---------------------------------------------
	m_pSubjectEditor = new CXTPCalendarViewEventSubjectEditor(pControl);
	if (!m_pSubjectEditor)
	{
		return NULL;
	}

	DWORD dwEditStyles = WS_CHILD | WS_VISIBLE | GetSubjectEditorStyles()
						 | ES_MULTILINE | ES_AUTOHSCROLL;

	m_pSubjectEditor->Create(dwEditStyles, m_rcSubjectEditor, pControl, 0);

	//m_pSubjectEditor->CreateEx(WS_EX_TRANSPARENT, _T("EDIT"), NULL,
	//                         dwEditStyles, m_rcSubjectEditor, pControl, 0);

	m_pSubjectEditor->SetFocus();
	CFont* pEditorFont = GetSubjectEditorFont();
	if (pEditorFont)
	{
		m_pSubjectEditor->SetFont(pEditorFont);
	}

	m_pSubjectEditor->SetWindowText(XTP_SAFE_GET1(m_ptrEvent, GetSubject(), _T("")));
	m_pSubjectEditor->SetSel(0, 0);

	//---------------------------------------------------------------------------
	return this;
}

CString CXTPCalendarViewEvent::GetEditingSubjectFromEditor()
{
	if (!m_pSubjectEditor)
	{
		ASSERT(FALSE);
		return _T("");
	}

	CString strSubject;
	m_pSubjectEditor->GetWindowText(strSubject);

	REPLACE_S(strSubject, _T(" \r\r\n "), _T(" "));
	REPLACE_S(strSubject, _T(" \r\r\n"), _T(" "));
	REPLACE_S(strSubject, _T("\r\r\n "), _T(" "));

	REPLACE_S(strSubject, _T("\r\n"), _T(" "));
	REPLACE_S(strSubject, _T("\n"), _T(" "));
	REPLACE_S(strSubject, _T("\r"), _T(" "));

	return strSubject;
}

CString CXTPCalendarViewEvent::EndEditSubject()
{
	if (!m_pSubjectEditor)
	{
		ASSERT(FALSE);
		return _T("");
	}

	CString strSubject = GetEditingSubjectFromEditor();

	//---------------------------------------------------------------------------
	delete m_pSubjectEditor;
	m_pSubjectEditor = NULL;
	//---------------------------------------------------------------------------
	return strSubject;
}

BOOL CXTPCalendarViewEvent::IsShowingToolTip()
{
	return m_bShowingToolTip;
}

void CXTPCalendarViewEvent::ShowToolTip()
{
	if (m_nTimerID_ShowToolTip)
	{
		KillTimer(m_nTimerID_ShowToolTip);
		m_nTimerID_ShowToolTip = 0;
	}
	BOOL bFocused = CWnd::GetFocus() == (CWnd*)GetCalendarControl();

	if (!bFocused)
	{
		return;
	}

	// create tooltip text
	CString strTip = GetToolText();

	// show tooltip window
	if (GetCalendarControl())
	{
		GetCalendarControl()->ShowToolTip(strTip, m_rcToolTip, GetSubjectEditorFont());
		m_bShowingToolTip = TRUE;
	}
}

void CXTPCalendarViewEvent::HideToolTip()
{
	if (GetCalendarControl())
	{
		GetCalendarControl()->HideToolTip();
	}
	m_bShowingToolTip = FALSE;
}

CString CXTPCalendarViewEvent::FormatEventTime(COleDateTime dtTime)
{
	return XTP_SAFE_GET3(GetViewGroup_(), GetViewDay_(), GetView_(), FormatEventTime(dtTime), _T("") );
}

CString CXTPCalendarViewEvent::GetToolText()
{
	if (!m_ptrEvent)
	{
		ASSERT(FALSE);
		return _T("");
	}

	CString strTime;

	if (!IsMultidayEvent())
	{
		COleDateTime dtStart = m_ptrEvent->GetStartTime();
		COleDateTime dtEnd = m_ptrEvent->GetEndTime();

		strTime = FormatEventTime(dtStart) + _T("-");

		if (!CXTPCalendarUtils::IsEqual(dtStart, dtEnd))
		{
			strTime += FormatEventTime(dtEnd);
		}
	}

	if (!strTime.IsEmpty())
	{
		strTime += _T(" ");
	}

	CString strTip = strTime + m_ptrEvent->GetSubject();

	//---------------------------------------------------------
	DWORD dwFlags = XTP_SAFE_GET1(GetCalendarControl(), GetAskItemTextFlags(), 0);

	if (dwFlags & xtpCalendarItemText_EventToolTipText)
	{
		XTP_CALENDAR_GETITEMTEXT_PARAMS objRequest;
		::ZeroMemory(&objRequest, sizeof(objRequest));

		objRequest.nItem = (int)xtpCalendarItemText_EventToolTipText;
		objRequest.pstrText = &strTip;
		objRequest.pViewEvent = this;

		XTP_SAFE_CALL1(GetCalendarControl(), SendNotificationAlways(XTP_NC_CALENDAR_GETITEMTEXT, (WPARAM)&objRequest, 0));
	}

	return strTip;
}


int CXTPCalendarViewEvent::CalcIconsRect(CRect rc)
{
	rc.top += 2;
	rc.bottom += 2;

	m_rcReminderGlyph.CopyRect(rc);
	m_rcPrivateGlyph.CopyRect(rc);
	m_rcRecurrenceGlyph.CopyRect(rc);
	m_rcMeetingGlyph.CopyRect(rc);
	m_rcReminderGlyph.right = m_rcReminderGlyph.left;
	m_rcPrivateGlyph.right = m_rcPrivateGlyph.left;
	m_rcRecurrenceGlyph.right = m_rcRecurrenceGlyph.left;
	m_rcMeetingGlyph.right = m_rcMeetingGlyph.left;

	m_rcReminderGlyph.bottom = m_rcReminderGlyph.top + 11;
	m_rcPrivateGlyph.bottom = m_rcPrivateGlyph.top + 11;
	m_rcRecurrenceGlyph.bottom = m_rcRecurrenceGlyph.top + 11;
	m_rcMeetingGlyph.bottom = m_rcMeetingGlyph.top + 11;

	CPoint ptX(rc.left, rc.top);
	CPoint ptY(rc.left, rc.top);
	int nMinsize = 0;

	if (GetEvent() && GetEvent()->IsReminder())
	{
		m_rcReminderGlyph.right = min(m_rcReminderGlyph.right + 15, rc.right);
		ptX.Offset(15, 0);
		ptY.Offset(0, 11);
	}

	if (GetEvent() && GetEvent()->IsPrivate())
	{
		if (rc.bottom - (ptY.y + 11) > 11)
		{
			m_rcPrivateGlyph.left = min(ptY.x, rc.right);
			m_rcPrivateGlyph.top = min(ptY.y, rc.bottom);
			ptY.Offset(0, 11);
		}
		else
		{
			m_rcPrivateGlyph.left = min(ptX.x, rc.right);
			m_rcPrivateGlyph.top = min(ptX.y, rc.bottom);
			ptX.Offset(11, 0);

		}
		m_rcPrivateGlyph.right = min(m_rcPrivateGlyph.left + 11, rc.right);
		m_rcPrivateGlyph.bottom = min(m_rcPrivateGlyph.top + 11, rc.bottom);
		nMinsize = 15;
	}

	int nRecurrenceState = XTP_SAFE_GET1(GetEvent(), GetRecurrenceState(), xtpCalendarRecurrenceNotRecurring);
	if ((nRecurrenceState & xtpCalendarRecurrenceMaster) ||
			(nRecurrenceState == xtpCalendarRecurrenceOccurrence) ||
			(nRecurrenceState == xtpCalendarRecurrenceException)
	 )

	{
		if (rc.bottom - (ptY.y + 11) > 11)
		{
			m_rcRecurrenceGlyph.left = min(ptY.x, rc.right);
			m_rcRecurrenceGlyph.top = min(ptY.y, rc.bottom);
			ptY.Offset(0, 11);
		}
		else
		{
			m_rcRecurrenceGlyph.left = min(ptX.x, rc.right);
			m_rcRecurrenceGlyph.top = min(ptX.y, rc.bottom);
			ptX.Offset(15, 0);

		}
		m_rcRecurrenceGlyph.right = min(m_rcRecurrenceGlyph.left + 15, rc.right);
		m_rcRecurrenceGlyph.bottom = min(m_rcRecurrenceGlyph.top + 11, rc.bottom);
		nMinsize = 15;
	}

	if (GetEvent() && GetEvent()->IsMeeting())
	{
		if (rc.bottom - (ptY.y + 11) > 11)
		{
			m_rcMeetingGlyph.left = min(ptY.x, rc.right);
			m_rcMeetingGlyph.top = min(ptY.y, rc.bottom);
			ptY.Offset(0, 11);
		}
		else
		{
			m_rcMeetingGlyph.left = min(ptX.x, rc.right);
			m_rcMeetingGlyph.top = min(ptX.y, rc.bottom);
			ptX.Offset(15, 0);

		}
		m_rcMeetingGlyph.right = min(m_rcMeetingGlyph.left + 15, rc.right);
		m_rcMeetingGlyph.bottom = min(m_rcMeetingGlyph.top + 11, rc.bottom);
		nMinsize = 15;
	}

	return max((ptX.x)- rc.left, nMinsize);
}

void CXTPCalendarViewEvent::AdjustSubjectEditor()
{
	if (IsEditingSubject() && m_pSubjectEditor)
	{
		m_pSubjectEditor->MoveWindow(&m_rcSubjectEditor, FALSE);
	}
}

BOOL CXTPCalendarViewEvent::IsSelected() const
{
	CXTPCalendarViewEvents* pSelEvents = XTP_SAFE_GET3(GetViewGroup_(), GetViewDay_(), GetView_(), GetSelectedViewEvents(), NULL);
	if (!pSelEvents)
	{
		return FALSE;
	}

	int nFIndex = pSelEvents->Find(m_ptrEvent);
	return nFIndex >= 0;
}

void CXTPCalendarViewEvent::SetSelected(BOOL bSet)
{
	XTP_SAFE_CALL3(GetViewGroup_(), GetViewDay_(), GetView_(), SelectViewEvent(this, bSet) );
}

CString CXTPCalendarViewEvent::_GetItemText(DWORD dwItemFlag)
{
	if (!GetCalendarControl() || !m_ptrEvent)
	{
		ASSERT(FALSE);
		return _T("");
	}

	DWORD dwFlags = GetCalendarControl()->GetAskItemTextFlags();

	CString strText;
	if (dwItemFlag & xtpCalendarItemText_EventSubject)
	{
		strText = m_ptrEvent->GetSubject();
	}
	else if (dwItemFlag & xtpCalendarItemText_EventLocation)
	{
		strText = m_ptrEvent->GetLocation();

	}
	else if (dwItemFlag & xtpCalendarItemText_EventBody)
	{
		strText = m_ptrEvent->GetBody();
	}
	else
	{
		ASSERT(FALSE);
		return _T("");
	}

	strText.Left(XTP_CALENDAR_DRAW_EVENTITEMTEXT_MAX);

	if (dwFlags & dwItemFlag)
	{
		XTP_CALENDAR_GETITEMTEXT_PARAMS objRequest;
		::ZeroMemory(&objRequest, sizeof(objRequest));

		objRequest.nItem = (int)dwItemFlag;
		objRequest.pstrText = &strText;
		objRequest.pViewEvent = this;

		GetCalendarControl()->SendNotificationAlways(XTP_NC_CALENDAR_GETITEMTEXT, (WPARAM)&objRequest, 0);
	}

	return strText;
}

//////////////////////////////////////////////////////////////////////////
//  CXTPCalendarViewEvents
//

CXTPCalendarViewEvents::CXTPCalendarViewEvents()
{
}

CXTPCalendarViewEvents::~CXTPCalendarViewEvents()
{
}

int CXTPCalendarViewEvents::Find(CXTPCalendarEvent* pEvent)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return -1;
	}

	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pViewEvent = GetAt(i);
		CXTPCalendarEvent* pEv = pViewEvent ? pViewEvent->GetEvent() : NULL;
		if (pEv && pEvent->IsEqualIDs(pEv) && pEvent->IsEqualStartEnd(pEv))
		{
			return i;
		}
	}

	return -1;
}

void CXTPCalendarViewEvents::Remove(CXTPCalendarEvent* pEvent)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return;
	}

	int nCount = GetCount();
	for (int i = nCount -1 ; i >= 0; i--)
	{
		CXTPCalendarViewEvent* pViewEvent = GetAt(i);
		CXTPCalendarEvent* pEv = XTP_SAFE_GET1(pViewEvent, GetEvent(), NULL);
		if (pEv && pEvent->IsEqualIDs(pEv) && pEvent->IsEqualStartEnd(pEv))
		{
			RemoveAt(i);
		}
	}
}

