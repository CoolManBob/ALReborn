// XTPCalendarView.cpp : implementation file
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

#include "Common/XTPPropExchange.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarView.h"
#include "XTPCalendarViewEvent.h"
#include "XTPCalendarViewDay.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarData.h"
#include <math.h>

#pragma warning(disable: 4571) // warning C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_CLIPBOARD_EVENTS_DATA_VER 2

//===========================================================================
IMPLEMENT_DYNAMIC(CXTPCalendarWMHandler, CCmdTarget)
IMPLEMENT_DYNAMIC(CXTPCalendarView, CXTPCalendarWMHandler)

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarView

CXTPCalendarView::CXTPCalendarView(CXTPCalendarControl* pCalendarControl) :
		m_pControl(pCalendarControl)
{
	m_rcView.SetRectEmpty();

	m_Layout.m_nRowHeight = 18;

	m_eDraggingMode = xtpCalendaDragModeUnknown;
	m_ptStartDragging.x = m_ptStartDragging.y = LONG_MAX;
	m_bStartedClickInside = FALSE;

	m_nTimerID_StartEditSubject = 0;

	m_eUndoMode = xtpCalendarUndoModeUnknown;
	m_bResetUndoBuffer = FALSE;

	m_nKeyStateTimerID = 0;

	m_pSelectedEvents = new CXTPCalendarViewEvents();
	m_pResources = NULL;

	_CalculateEventTimeFormat();

	m_bScrollV_Disabled = FALSE;
	m_bScrollH_Disabled = FALSE;

}

CXTPCalendarView::~CXTPCalendarView()
{
	CMDTARGET_RELEASE(m_pSelectedEvents);
	CMDTARGET_RELEASE(m_pResources);


}

void CXTPCalendarView::OnBeforeDestroy()
{
	//---------------------------------------------------------------------------
	if (IsEditingSubject())
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(commit) [CXTPCalendarView::OnBeforeDestroy]"));
		EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}

	if (m_ptrDraggingEventNew)
	{
		BOOL bChanged = CommitDraggingEvent();
		TRACE_DRAGGING(_T("Dragging - CommitDraggingEvent() = %d. [CXTPCalendarView::OnBeforeDestroy] \n"), bChanged);
	}
	//---------------------------------------------------------------------------

	if (m_pSelectedEvents)
	{
		m_pSelectedEvents->RemoveAll();
	}

	//===========================================================================
	TBase::OnBeforeDestroy();
}

void CXTPCalendarView::OnActivateView(BOOL bActivate,
										CXTPCalendarView* pActivateView,
										CXTPCalendarView* pInactiveView)
{
	UNREFERENCED_PARAMETER(pActivateView);
	UNREFERENCED_PARAMETER(pInactiveView);

	if (!bActivate && IsEditingSubject())
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(commit) [CXTPCalendarView::OnActivateView(FALSE)] \n"));
		EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}
}

void CXTPCalendarView::RedrawControl()
{
	ASSERT(m_pControl);
	if (m_pControl)
	{
		m_pControl->RedrawControl();
	}
}

void CXTPCalendarView::Populate()
{
	if (IsEditingSubject())
	{
		EndEditSubject(xtpCalendarEditSubjectCommit);
	}

	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewDay* pViewDay = GetViewDay_(i);
		COleDateTime dtDay = GetViewDayDate(i);
		ASSERT(pViewDay);
		if (pViewDay)
		{
			pViewDay->Populate(dtDay);
		}
	}

	_ReSelectSelectEvents();
}

BOOL CXTPCalendarView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CSelectionChangedContext selChanged(this);

	TRACE_MOUSE(_T("OnLButtonDown - [CXTPCalendarView] (nFlags =%x, point.x =%d, point.y =%d) \n"), nFlags, point.x, point.y);
	m_bStartedClickInside = TRUE;

	TBase::OnLButtonDown(nFlags, point);

	//= kill timeout request to edit subject=====================================
	if (m_nTimerID_StartEditSubject)
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - KillTimer() [CXTPCalendarView::OnLButtonDown(%d, %d)] \n"), point.x, point.y);

		KillTimer(m_nTimerID_StartEditSubject);
		m_nTimerID_StartEditSubject = 0;
		m_ptrEditingViewEvent = NULL;
	}

	//---------------------------------------------------------------------------
	XTP_CALENDAR_HITTESTINFO hitInfo;
	if (HitTest(point, &hitInfo))
	{
		if (hitInfo.uHitCode & xtpCalendarHitTestDay_Mask)
		{
			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);
			ProcessDaySelection(&hitInfo, nFlags);
		}
		else
			if (hitInfo.uHitCode & xtpCalendarHitTestEvent_Mask)
			{
				CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);
				ProcessEventSelection(&hitInfo, nFlags);

				// set request to start dragging when OnMouseMove()
				m_ptStartDragging = point;

				TRACE_DRAGGING(_T("Dragging REQUEST point(%d, %d) \n"), point.x, point.y);
			}
	}

	//---------------------------------------------------------------------------
	BOOL bResetUndoLocal = TRUE;
	if (IsEditingSubject())
	{
		if (!GetSubjectEditorRect().PtInRect(point))
		{
			TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(commit) [CXTPCalendarView::OnLButtonDown(%d, %d)] \n"), point.x, point.y);

			bResetUndoLocal = m_eDraggingMode != xtpCalendaDragModeEditSubject;
			EndEditSubject(xtpCalendarEditSubjectCommit);
		}
	}
	m_bResetUndoBuffer = bResetUndoLocal;

	return TRUE;
}

BOOL CXTPCalendarView::OnLButtonUp(UINT nFlags, CPoint point)
{
	TRACE_MOUSE(_T("OnLButtonUp - [CXTPCalendarView] (nFlags =%x, point.x =%d, point.y =%d) \n"), nFlags, point.x, point.y);
	{
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
		CSelectionChangedContext selChanged(this);

		TBase::OnLButtonUp(nFlags, point);

		//---------------------------------------------------------------------------
		m_ptStartDragging.x = m_ptStartDragging.y = LONG_MAX;

		XTP_CALENDAR_HITTESTINFO hitInfo;
		BOOL bHitTest = HitTest(point, &hitInfo);
		int nDragMode = m_eDraggingMode;

		if (!OnEndDragging(point, &hitInfo))
		{
			TRACE_DRAGGING(_T("Dragging ENDED. point(%d, %d). [CXTPCalendarView::OnLButtonUp] \n"), point.x, point.y);

			if (bHitTest)
			{
				UINT uESmask = xtpCalendarHitTestEventTextArea | xtpCalendarHitTestEventResizeArea_Mask;
				if ((hitInfo.uHitCode & uESmask) && !IsEditingSubject() &&
					hitInfo.pViewEvent && hitInfo.pViewEvent->IsSelected())
				{
					BOOL bEnabled = XTP_SAFE_GET2(GetCalendarControl(), GetCalendarOptions(), bEnableInPlaceEditEventSubject_ByMouseClick, FALSE);
					bEnabled = bEnabled && IsSingleEventSelected();
					if (bEnabled)
					{
						bEnabled = !OnBeforeEditOperationNotify(xtpCalendarEO_EditSubject_ByMouseClick, hitInfo.pViewEvent);
					}
					if (bEnabled)
					{
						ASSERT(!m_ptrEditingViewEvent);

						m_ptrEditingViewEvent = hitInfo.pViewEvent;
						m_ptrEditingViewEvent->InternalAddRef();

						if (m_nTimerID_StartEditSubject == 0)
						{
							UINT uTimeOut_ms = GetStartEditSubjectTimeOut();
							m_nTimerID_StartEditSubject = SetTimer(uTimeOut_ms);
							ASSERT(m_nTimerID_StartEditSubject);
							DBG_TRACE_TIMER(_T("SET Timer: ID =%d, m_nTimerID_StartEditSubject, CXTPCalendarView.OnLButtonUp() \n"), m_nTimerID_StartEditSubject);

							TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - StartEditSubject REQUEST. [CXTPCalendarView::OnLButtonUp(%d, %d)] \n"), point.x, point.y);
						}
					}
				}
			}
		}

		//---------------------------------------------------------------------------
		if (_IsDragModeCopyMove(nDragMode) && bHitTest && hitInfo.pViewDay)
		{
			UnselectAllEvents();
			SelectDay(hitInfo.pViewDay, hitInfo.nGroup);
		}

		if (GetCalendarControl())
		{
			GetCalendarControl()->m_mouseMode = xtpCalendarMouseNothing;
		}
		m_bStartedClickInside = FALSE;
	}
	OnMouseMove(nFlags, point);

	return TRUE;
}

void CXTPCalendarView::OnMouseMove(UINT nFlags, CPoint point)
{

	TRACE_MOUSE(_T("OnMouseMove - [CXTPCalendarView] (point.x =%d, point.y =%d) \n"),
		point.x, point.y);

	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CSelectionChangedContext selChanged(this);

	TBase::OnMouseMove(nFlags, point);

	//---------------------------------------------------------------------------
	XTP_CALENDAR_HITTESTINFO hitInfo;
	BOOL bHitTest = HitTest(point, &hitInfo);

	if (bHitTest && hitInfo != m_LastHitInfo)
	{
		m_LastHitInfo = hitInfo;

		if (nFlags & MK_LBUTTON && m_bStartedClickInside)
		{
			const int cEpsilon = 2;
			if (m_ptStartDragging.x == LONG_MAX || m_ptStartDragging.y == LONG_MAX ||
					abs(point.x - m_ptStartDragging.x) >= cEpsilon ||
					abs(point.y - m_ptStartDragging.y) >= cEpsilon)
			{
				if (!m_ptrDraggingEventNew && !m_ptrDraggingEventOrig &&
						m_eDraggingMode == xtpCalendaDragModeUnknown)
				{
					XTP_CALENDAR_HITTESTINFO hitInfoSD;
					if (HitTest(m_ptStartDragging, &hitInfoSD))
					{
						if (hitInfoSD.uHitCode & xtpCalendarHitTestEvent_Mask)
						{
							int eDragMode = _GetDraggingMode(&hitInfoSD);
							if (!IsEditOperationDisabledNotify(_DragMod2Operation(eDragMode), hitInfoSD.pViewEvent))
							{
								TRACE_DRAGGING(_T("Dragging will START. point(%d, %d). [CXTPCalendarView::OnMouseMove] \n"), point.x, point.y);

								OnStartDragging(m_ptStartDragging, &hitInfoSD);
							}
						}
					}
				}
				m_ptStartDragging.x = m_ptStartDragging.y = LONG_MAX;

				BOOL bDragging = m_ptrDraggingEventNew && m_eDraggingMode != xtpCalendaDragModeEditSubject;

				if (bDragging)
				{
					CXTPCalendarEventPtr ptrDragEventPrev = m_ptrDraggingEventNew->CloneEvent();
					BOOL bDragged = OnDragging(point, &hitInfo);

					if (bDragged)
					{
						XTP_EVENT_DRAG_OPERATION_PARAMS opParamsDrag;
						::ZeroMemory(&opParamsDrag, sizeof(opParamsDrag));

						opParamsDrag.eOperation = _DragMod2Operation(m_eDraggingMode);
						opParamsDrag.pDraggingEvent = ptrDragEventPrev;
						opParamsDrag.pDraggingEventNew = m_ptrDraggingEventNew;

						BOOL bHandled = FALSE;
						if (m_pControl)
						{
							m_pControl->SendNotification(XTP_NC_CALENDAR_BEFORE_EVENT_EDIT_OPERATION,
								(WPARAM)&opParamsDrag, (LPARAM)&bHandled);
						}

						if (bHandled)
						{
							//rollback
							m_ptrDraggingEventNew = ptrDragEventPrev;
						}
						else
						{
							TRACE_DRAGGING(_T("Dragging OnDragging change event. point(%d, %d). [CXTPCalendarView::OnMouseMove] \n"), point.x, point.y);
						}

						XTP_SAFE_CALL1(GetCalendarControl(), Populate());
						CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateLayout | xtpCalendarUpdateRedraw);

						if (m_ptrDraggingEventNew && m_ptrDraggingEventOrig &&
							(!m_ptrDraggingEventNew->IsEqualStartEnd(m_ptrDraggingEventOrig) ||
							 m_ptrDraggingEventNew->GetScheduleID() != m_ptrDraggingEventOrig->GetScheduleID())
							)
						{
							SelectEvent(m_ptrDraggingEventNew);
						}
					}
				}
			}

			if (m_eDraggingMode == xtpCalendaDragModeUnknown &&
				hitInfo.uHitCode & (xtpCalendarHitTestDay_Mask | xtpCalendarHitTestEvent_Mask))

			{
				if ((hitInfo.uHitCode & xtpCalendarHitTestEvent_Mask) &&
					hitInfo.pViewEvent && hitInfo.pViewEvent->IsSelected())
				{}
				else
				{
					CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

					ProcessDaySelection(&hitInfo, nFlags | MK_SHIFT);
				}
			}
		}
	}
	else if (!bHitTest)
	{
		XTP_CALENDAR_HITTESTINFO hitInfoEmpty;
		m_LastHitInfo = hitInfoEmpty;
	}

	//===========================================================================
	if (GetCalendarControl())
	{
		if (_IsDragModeCopyMove(m_eDraggingMode) &&
			GetCalendarControl()->m_mouseMode != xtpCalendarMouseEventDraggingOut)
		{
			GetCalendarControl()->m_mouseMode = m_eDraggingMode == xtpCalendaDragModeCopy ?
					xtpCalendarMouseEventDragCopy : xtpCalendarMouseEventDragMove;

			GetCalendarControl()->UpdateMouseCursor();
		}
	}
}

void CXTPCalendarView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CSelectionChangedContext selChanged(this);

	TRACE_KEYBOARD(_T("OnChar - [CXTPCalendarView] (nChar =%d, nRepCnt =%d, nFlags =%x) \n"), nChar, nRepCnt, nFlags);

	TBase::OnChar(nChar, nRepCnt, nFlags);

	//========================================================================
	BOOL bEnable = XTP_SAFE_GET2(GetCalendarControl(), GetCalendarOptions(), bEnableInPlaceCreateEvent, FALSE);
	if (nChar >= _T(' ') && bEnable)
	{
		CString strInitialSubj((TCHAR)nChar);
		if (!OnInPlaceCreateEvent(strInitialSubj))
		{
			StartEditNewEventInplace(strInitialSubj);
		}
	}
}

void CXTPCalendarView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CSelectionChangedContext selChanged(this);

	TRACE_KEYBOARD(_T("OnKeyDown - [CXTPCalendarView] (nChar =%d, nRepCnt =%d, nFlags =%x) \n"), nChar, nRepCnt, nFlags);

	TBase::OnKeyDown(nChar, nRepCnt, nFlags);

	//---------------------------------------------------------------------------
	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		if (IsEditingSubject())
		{
			TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(). [CXTPCalendarView::OnKeyDown] \n"));

			CXTPCalendarEventPtr ptrEditingEvent(m_ptrDraggingEventOrig, TRUE);

			EndEditSubject((nChar == VK_RETURN) ? xtpCalendarEditSubjectCommit : xtpCalendarEditSubjectCancel);

			if (nChar == VK_ESCAPE && ptrEditingEvent)
			{
				_Undo(ptrEditingEvent);
				m_UndoBuffer.Set(m_PrevUndoBuffer);
			}
		}

		if (nChar == VK_ESCAPE && m_ptrDraggingEventNew)
		{
			CXTPCalendarEventPtr ptrEditingEvent(m_ptrDraggingEventOrig, TRUE);

			CancelDraggingEvent();

			BOOL bControlUpdated = FALSE;
			if (ptrEditingEvent && !m_bResetUndoBuffer)
			{
				bControlUpdated = _Undo(ptrEditingEvent);
				m_UndoBuffer.Set(m_PrevUndoBuffer);
			}

			if (!bControlUpdated && GetCalendarControl())
			{
				CXTPCalendarControl::CUpdateContext updateContext(GetCalendarControl(), xtpCalendarUpdateLayout | xtpCalendarUpdateRedraw);
				GetCalendarControl()->Populate();
			}
		}
	}
	else if (nChar == VK_F2)
	{
		if (IsEditingSubject())
		{
			return;
		}

		CXTPCalendarViewEvent* pViewEvent = GetLastSelectedViewEvent();
		BOOL bEnabled = XTP_SAFE_GET2(GetCalendarControl(), GetCalendarOptions(), bEnableInPlaceEditEventSubject_ByF2, FALSE);
		bEnabled = bEnabled && m_eDraggingMode == xtpCalendaDragModeUnknown && pViewEvent;

		if (bEnabled)
		{
			bEnabled = !OnBeforeEditOperationNotify(xtpCalendarEO_EditSubject_ByF2, pViewEvent);
		}
		if (bEnabled)
		{
			ASSERT(!m_ptrDraggingEventOrig && !m_ptrDraggingEventNew && !m_ptrEditingViewEvent);

			if (pViewEvent)
			{
				CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

				EmptyUndoBuffer();

				StartEditSubject(pViewEvent);
			}
		}
	}
	else if (nChar == VK_DELETE)
	{
		if (m_eDraggingMode == xtpCalendaDragModeUnknown && !m_ptrEditingViewEvent)
		{
			ASSERT(!m_ptrDraggingEventOrig && !m_ptrDraggingEventNew && !m_ptrEditingViewEvent);

			CXTPCalendarViewEvent* pViewEvent = GetLastSelectedViewEvent();

			if (m_selectedBlock.dtEnd.GetStatus() == COleDateTime::valid &&
				GetCalendarControl() && pViewEvent && pViewEvent->IsSelected())
			{
				CXTPCalendarControl::CUpdateContext updateContext(GetCalendarControl(), xtpCalendarUpdateRedraw | xtpCalendarUpdateLayout);

				VERIFY(GetCalendarControl()->DoDeleteSelectedEvents(pViewEvent));

				GetCalendarControl()->Populate();
			}
		}
	}
	else if (nChar == VK_TAB)
	{
		if (m_eDraggingMode == xtpCalendaDragModeUnknown ||
			m_eDraggingMode == xtpCalendaDragModeEditSubject)
		{
			BOOL bShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;

			CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);
			if (IsEditingSubject())
			{
				EndEditSubject(xtpCalendarEditSubjectCommit);
			}

			COleDateTime dtMinStart = GetNextTimeEditByTAB();

			CXTPCalendarViewEvent* pViewEvent = GetLastSelectedViewEvent();
			CXTPCalendarEvent* pLastEvent = pViewEvent ? pViewEvent->GetEvent() : NULL;

			const int cLoopMax = 5000;
			int i;
			for (i = 0; i < cLoopMax; i++) // to prevent infinite looping
			{
				pViewEvent = FindEventToEditByTAB(dtMinStart, bShift, pLastEvent);
				pLastEvent = pViewEvent ? pViewEvent->GetEvent() : NULL;
				if (!pViewEvent || pViewEvent && pViewEvent->IsVisible())
				{
					break;
				}
			}
			ASSERT(i < cLoopMax); // probably infinite looping ???


			UnselectAllEvents();
			if (pViewEvent && pViewEvent->IsVisible() && pViewEvent->GetEvent())
			{
				dtMinStart = pViewEvent->GetEvent()->GetStartTime();
				UpdateNextTimeEditByTAB(dtMinStart, bShift);

				EmptyUndoBuffer();

				BOOL bEnabled = XTP_SAFE_GET2(GetCalendarControl(), GetCalendarOptions(), bEnableInPlaceEditEventSubject_ByTab, FALSE);
				if (bEnabled)
				{
					bEnabled = !OnBeforeEditOperationNotify(xtpCalendarEO_EditSubject_ByTab, pViewEvent);
				}

				if (bEnabled)
				{
					StartEditSubject(pViewEvent);
				}
				else
				{
					SelectEvent(pViewEvent->GetEvent());
				}

				SelectDay(XTP_SAFE_GET2(pViewEvent, GetViewGroup_(), GetViewDay_(), NULL),
						  XTP_SAFE_GET2(pViewEvent, GetViewGroup_(), GetGroupIndex(), -1));
			}
			else
			{
				dtMinStart = GetViewDayDate(bShift ? GetViewDayCount()-1 : 0);
				UpdateNextTimeEditByTAB(dtMinStart, bShift, TRUE);
			}
		}
	}
}

BOOL CXTPCalendarView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedrawIfNeed);
	CSelectionChangedContext selChanged(this);

	TRACE_KEYBOARD(_T("OnSysKeyDown - [CXTPCalendarView] (nChar =%d, nRepCnt =%d, nFlags =%x) \n"), nChar, nRepCnt, nFlags);

	BOOL bHandled = TBase::OnSysKeyDown(nChar, nRepCnt, nFlags);

	//---------------------------------------------------------------------------
	UINT uAltBit = (((UINT)1) << 13);
	if (nChar == VK_BACK && (nFlags & uAltBit))
	{
		bHandled = TRUE;
		if (CanUndo())
		{
			Undo();
		}
		else if (CanRedo())
		{
			Redo();
		}
	}

	return bHandled;
}

BOOL CXTPCalendarView::SetDraggingEvent(CXTPCalendarEvent* pEventOrig)
{
	if (m_ptrDraggingEventOrig || m_ptrDraggingEventNew || !pEventOrig)
	{
		ASSERT(FALSE); // Dragging Event already set,
		return FALSE;  // or CommitDraggingEvent() was not called.
	}                  // or invalid parameter

	TRACE_DRAGGING(_T("Dragging - SetDraggingEvent. ptr =%x. [CXTPCalendarView::SetDraggingEvent] \n"), pEventOrig);

	pEventOrig->InternalAddRef();
	m_ptrDraggingEventOrig = pEventOrig;
	m_ptrDraggingEventNew = pEventOrig->CloneEvent();

	if (m_ptrDraggingEventNew &&
		m_ptrDraggingEventNew->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence)
	{
		VERIFY(m_ptrDraggingEventNew->MakeAsRException());
	}

	return TRUE;
}

CXTPCalendarEvent* CXTPCalendarView::GetDraggingEventOrig()
{
	return m_ptrDraggingEventOrig;
}

CXTPCalendarEvent* CXTPCalendarView::GetDraggingEventNew()
{
	return m_ptrDraggingEventNew;
}

BOOL CXTPCalendarView::CommitDraggingEvent()
{
	ASSERT(m_ptrDraggingEventNew);

	if (!GetCalendarControl())
	{
		return FALSE;
	}

	CXTPAutoResetValue<BOOL> autoSet_UpdateCtrlNf(GetCalendarControl()->m_bUpdateWhenEventChangedNotify);
	autoSet_UpdateCtrlNf = FALSE;

	if (m_ptrDraggingEventNew && m_ptrDraggingEventOrig)
	{
		BOOL bDPdiff = m_ptrDraggingEventNew->GetDataProvider() != m_ptrDraggingEventOrig->GetDataProvider();

		BOOL bChanged = !m_ptrDraggingEventNew->IsEqualStartEnd(m_ptrDraggingEventOrig);
		bChanged = bChanged || m_ptrDraggingEventNew->GetSubject() != m_ptrDraggingEventOrig->GetSubject();
		bChanged = bChanged || m_ptrDraggingEventNew->IsAllDayEvent() != m_ptrDraggingEventOrig->IsAllDayEvent();
		bChanged = bChanged || m_ptrDraggingEventNew->GetScheduleID() != m_ptrDraggingEventOrig->GetScheduleID();
		bChanged = bChanged || bDPdiff;

		if (bChanged || m_eDraggingMode == xtpCalendaDragModeCopy)
		{
			if (!m_UndoBuffer.IsInUndoBuffer(m_ptrDraggingEventOrig) || m_bResetUndoBuffer)
			{
				m_PrevUndoBuffer.Set(m_UndoBuffer);
				EmptyUndoBuffer();
			}

			CXTPCalendarData* pDataNew = m_ptrDraggingEventNew->GetDataProvider(); // _GetDataProviderForDraggingEvent(m_ptrDraggingEventNew);
			CXTPCalendarData* pDataOrig = m_ptrDraggingEventOrig->GetDataProvider();

			if (m_eDraggingMode == xtpCalendaDragModeCopy)
			{
				if (m_ptrDraggingEventNew->GetRecurrenceState() != xtpCalendarRecurrenceNotRecurring)
				{
					VERIFY(m_ptrDraggingEventNew->RemoveRecurrence() );
				}
				if (pDataNew)
				{
					DBG_DATA_VERIFY(pDataNew->AddEvent(m_ptrDraggingEventNew) );
				}
			}
			else
			{
				m_UndoBuffer.AddUndoAction(m_ptrDraggingEventOrig, m_ptrDraggingEventNew);
				m_eUndoMode = xtpCalendarUndoModeUndo;
				m_bResetUndoBuffer = FALSE;

				if (pDataNew)
				{
					if (!bDPdiff)
					{
						// restore event ID for case when m_ptrDraggingEventNew was recreated
						// during dragging between providers
						m_ptrDraggingEventNew->SetEventID(m_ptrDraggingEventOrig->GetEventID());

						DBG_DATA_VERIFY(pDataNew->ChangeEvent(m_ptrDraggingEventNew) );
					}
					else
					{
						if (m_ptrDraggingEventNew->GetRecurrenceState() != xtpCalendarRecurrenceNotRecurring)
						{
							VERIFY(m_ptrDraggingEventNew->RemoveRecurrence() );
						}
						if (pDataOrig)
						{
							DBG_DATA_VERIFY(pDataOrig->DeleteEvent(m_ptrDraggingEventOrig) );
						}

						DBG_DATA_VERIFY(pDataNew->AddEvent(m_ptrDraggingEventNew));
					}
				}

				// for test only
				//VERIFY(pData->DeleteEvent(m_ptrDraggingEventOrig));
				//VERIFY(pData->AddEvent(m_ptrDraggingEventNew));
			}
		}

		TRACE_DRAGGING(_T("Dragging - CommitDraggingEvent() = %d. ptr =%x. [CXTPCalendarView] \n"), bChanged, (CXTPCalendarEvent*)m_ptrDraggingEventOrig);

		m_ptrDraggingEventOrig = NULL;
		m_ptrDraggingEventNew = NULL;

		return bChanged;
	}
	else if (m_ptrDraggingEventNew)
	{
		if (!m_UndoBuffer.IsInUndoBuffer(m_ptrDraggingEventNew) || m_bResetUndoBuffer)
		{
			m_PrevUndoBuffer.Set(m_UndoBuffer);
			EmptyUndoBuffer();
		}

		CXTPCalendarData* pData = m_ptrDraggingEventNew->GetDataProvider(); //_GetDataProviderForDraggingEvent(m_ptrDraggingEventNew);

		ASSERT(m_ptrDraggingEventNew->GetRecurrenceState() == xtpCalendarRecurrenceNotRecurring);

		if (pData)
		{
			DBG_DATA_VERIFY( pData->AddEvent(m_ptrDraggingEventNew) );
		}

		m_UndoBuffer.AddUndoAction(NULL, m_ptrDraggingEventNew);
		m_eUndoMode = xtpCalendarUndoModeUndo;
		m_bResetUndoBuffer = FALSE;

		m_ptrDraggingEventOrig = NULL;
		m_ptrDraggingEventNew = NULL;

		return TRUE;
	}

	return FALSE;
}

void CXTPCalendarView::CancelDraggingEvent()
{
	ASSERT(m_ptrDraggingEventNew);
	m_eDraggingMode = xtpCalendaDragModeUnknown;
	if (GetCalendarControl())
	{
		GetCalendarControl()->m_mouseMode = xtpCalendarMouseNothing;
		GetCalendarControl()->UpdateMouseCursor();
	}

	m_ptrDraggingEventOrig = NULL;
	m_ptrDraggingEventNew = NULL;

	m_ptStartDragging.x = m_ptStartDragging.y = LONG_MAX;
	m_bStartedClickInside = FALSE;
}

CXTPCalendarEventsPtr CXTPCalendarView::RetrieveDayEvents(CXTPCalendarResource* pRC,
														  COleDateTime dtDay)
{
	ASSERT(pRC);

	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	CXTPCalendarEventsPtr ptrEvents = pRC ? pRC->RetrieveDayEvents(dtDay) : new CXTPCalendarEvents();

	if (!m_ptrDraggingEventNew || !ptrEvents || !pRC)
	{
		return ptrEvents;
	}

	BOOL bIsDragCopy = m_eDraggingMode == xtpCalendaDragModeCopy;

	COleDateTime dtDay1 = CXTPCalendarUtils::ResetTime(m_ptrDraggingEventNew->GetStartTime());
	COleDateTime dtDay2 = CXTPCalendarUtils::ResetTime(m_ptrDraggingEventNew->GetEndTime());

	UINT uSchID = m_ptrDraggingEventNew->GetScheduleID();
	BOOL bNewEventScheduleVisible =
			m_ptrDraggingEventNew->GetDataProvider() == pRC->GetDataProvider() &&
			pRC->ExistsScheduleID(uSchID);

	if (!bIsDragCopy)
	{
		int nCount = ptrEvents->GetCount();
		for (int i = nCount-1; i >= 0; i--)
		{
			CXTPCalendarEvent* pEvent = ptrEvents->GetAt(i, FALSE);
			if (!pEvent)
			{
				continue;
			}

			if (m_ptrDraggingEventOrig &&
				m_ptrDraggingEventOrig->GetDataProvider() == pEvent->GetDataProvider() &&
				m_ptrDraggingEventOrig->IsEqualIDs(pEvent) &&
				m_ptrDraggingEventOrig->GetScheduleID() == pEvent->GetScheduleID()
			   )
			{
				ptrEvents->RemoveAt(i);
				//break;
			}
		}
	}

	if (dtDay >= dtDay1 && dtDay <= dtDay2 && bNewEventScheduleVisible)
	{
		ptrEvents->Add(m_ptrDraggingEventNew);
	}

	return ptrEvents;
}

XTPCalendarDraggingMode CXTPCalendarView::_GetDraggingMode(XTP_CALENDAR_HITTESTINFO* pHitTest) const
{
	if (!pHitTest)
	{
		ASSERT(FALSE);
		return xtpCalendaDragModeUnknown;
	}

	if (pHitTest->uHitCode & xtpCalendarHitTestEventResizeArea_Mask)
	{
		if (pHitTest->uHitCode & xtpCalendarHitTestEventResizeBegin)
		{
			return xtpCalendaDragModeResizeBegin;
		}
		else if (pHitTest->uHitCode & xtpCalendarHitTestEventResizeEnd)
		{
			return xtpCalendaDragModeResizeEnd;
		}
	}
	else
	{
		BOOL bIsCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
		return bIsCtrl ? xtpCalendaDragModeCopy : xtpCalendaDragModeMove;
	}

	return xtpCalendaDragModeUnknown;
}

void CXTPCalendarView::OnStartDragging(CPoint /*point*/, XTP_CALENDAR_HITTESTINFO* pHitTest)
{
	if (!pHitTest)
	{
		ASSERT(FALSE);
		return;
	}

	if ((pHitTest->uHitCode & xtpCalendarHitTestEvent_Mask) == 0)
	{
		return;
	}

	//---------------------------------------------------------------------------
	if (IsEditingSubject())
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(). [CXTPCalendarView::OnStartDragging] \n"));

		EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}
	//---------------------------------------------------------------------------

	m_spDraggingStartOffset.SetStatus(COleDateTimeSpan::null);
	m_eDraggingMode = xtpCalendaDragModeUnknown;

	if (!pHitTest->pViewEvent || !pHitTest->pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	m_eDraggingMode = _GetDraggingMode(pHitTest);
	ASSERT(m_eDraggingMode != xtpCalendaDragModeUnknown);

	if (_IsDragModeCopyMove(m_eDraggingMode))
	{
		if (pHitTest->bTimePartValid)
		{
			m_spDraggingStartOffset = pHitTest->dt - pHitTest->pViewEvent->GetEvent()->GetStartTime();
		}
		else
		{
			m_spDraggingStartOffset = CXTPCalendarUtils::ResetTime(pHitTest->dt) -
									  CXTPCalendarUtils::ResetTime(pHitTest->pViewEvent->GetEvent()->GetStartTime());
		}

		if (!m_nKeyStateTimerID)
		{
			m_nKeyStateTimerID = SetTimer(50);
			DBG_TRACE_TIMER(_T("SET Timer: ID =%d, m_nKeyStateTimerID, CXTPCalendarView::OnStartDragging() \n"), m_nKeyStateTimerID);
		}
	}

	//---------------------------------------------------------------------------
	VERIFY(SetDraggingEvent(pHitTest->pViewEvent->GetEvent()));
}

BOOL CXTPCalendarView::OnEndDragging(CPoint point, XTP_CALENDAR_HITTESTINFO* /*phitInfo*/)
{
	if (!m_ptrDraggingEventNew || !m_ptrDraggingEventOrig ||
			m_eDraggingMode == xtpCalendaDragModeEditSubject)
	{
		return FALSE;
	}

	CXTPCalendarEventPtr ptrEventToSelect = m_ptrDraggingEventNew;

	BOOL bChanged = CommitDraggingEvent();

	int nDragMode = m_eDraggingMode;
	m_eDraggingMode = xtpCalendaDragModeUnknown;

	TRACE_DRAGGING(_T("Dragging - CommitDraggingEvent() = %d. [CXTPCalendarView::OnEndDragging(%d, %d)] \n"), bChanged, point.x, point.y);

	if (bChanged && GetCalendarControl())
	{
		GetCalendarControl()->Populate();
	}
	UnselectAllEvents();

	//---------------------------------------------------------------------------
	if (_IsDragModeCopyMove(nDragMode))
	{
		EmptyUndoBuffer();
	}
	//---------------------------------------------------------------------------
	if (ptrEventToSelect && (nDragMode == xtpCalendaDragModeResizeBegin || nDragMode == xtpCalendaDragModeResizeEnd))
	{
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

		COleDateTime dtFVDate = CXTPCalendarUtils::ResetTime(ptrEventToSelect->GetStartTime());
		if (nDragMode == xtpCalendaDragModeResizeEnd)
		{
			dtFVDate = CXTPCalendarUtils::RetriveEventEndDate(ptrEventToSelect);
		}

		CXTPCalendarViewEvent* pViewEvent = FindViewEvent(ptrEventToSelect, dtFVDate);
		if (pViewEvent && pViewEvent->GetEvent())
		{
			SelectDay(XTP_SAFE_GET2(pViewEvent, GetViewGroup_(), GetViewDay_(), NULL),
					  XTP_SAFE_GET2(pViewEvent, GetViewGroup_(), GetGroupIndex(), -1));

			BOOL bEnabled = XTP_SAFE_GET2(GetCalendarControl(), GetCalendarOptions(), bEnableInPlaceEditEventSubject_AfterEventResize, FALSE);
			if (bEnabled)
			{
				bEnabled = !OnBeforeEditOperationNotify(xtpCalendarEO_EditSubject_AfterEventResize, pViewEvent);
			}

			if (bEnabled)
			{
				TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - StartEditSubject(). [CXTPCalendarView::OnEndDragging(%d, %d)] \n"), point.x, point.y);
				StartEditSubject(pViewEvent);
			}
			else
			{
				SelectEvent(pViewEvent->GetEvent());
			}
		}
	}

	SelectEvent(ptrEventToSelect);

	return TRUE;
}

CXTPCalendarViewEvent* CXTPCalendarView::FindViewEvent(CXTPCalendarEvent* pEvent, COleDateTime dtDay)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return NULL;
	}
	dtDay = CXTPCalendarUtils::ResetTime(dtDay);

	CXTPCalendarViewEvent* pFEView = NULL;
	BOOL bFDayProceessed = FALSE;

	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewDay* pViewDay = GetViewDay_(i);
		if (!pViewDay)
		{
			ASSERT(FALSE);
			continue;
		}

		COleDateTime dtDayDate = pViewDay->GetDayDate();

		int nGroupsCount = pViewDay->GetViewGroupsCount();
		for (int g = 0; g < nGroupsCount; g++)
		{
			CXTPCalendarViewGroup* pViewGroup = pViewDay->GetViewGroup_(g);
			ASSERT(pViewGroup);
			if (!pViewGroup)
			{
				continue;
			}
			int nECount = pViewGroup->GetViewEventsCount();
			for (int j = 0; j < nECount; j++)
			{
				CXTPCalendarViewEvent* pViewEvent = pViewGroup->GetViewEvent_(j);
				CXTPCalendarEvent* pEv = pViewEvent ? pViewEvent->GetEvent() : NULL;
				if (!pEv)
				{
					ASSERT(FALSE);
					continue;
				}
				if (pEvent->IsEqualIDs(pEv) && pEvent->IsEqualStartEnd(pEv))
				{
					if (pViewEvent->IsVisible())
					{
						pFEView = pViewEvent;
					}
					if (CXTPCalendarUtils::IsEqual(dtDay, dtDayDate))
					{
						bFDayProceessed = TRUE;
					}
					if (pFEView && bFDayProceessed)
					{
						return pFEView;
					}
				}
			}
		}
	}
	return NULL;
}

BOOL CXTPCalendarView::HasSelectedViewEvent()
{
	if (!m_pSelectedEvents)
	{
		return FALSE;
	}
	return m_pSelectedEvents->GetCount() > 0;
}

CXTPCalendarViewEvents* CXTPCalendarView::GetSelectedViewEvents()
{
	return m_pSelectedEvents;
}

BOOL CXTPCalendarView::IsSingleEventSelected()
{
	int nCount = m_pSelectedEvents ? m_pSelectedEvents->GetCount() : 0;

	if (nCount == 0)
	return FALSE;

	if (nCount == 1)
		return TRUE;

	CXTPCalendarEvent* pEvent = m_pSelectedEvents->GetAt(0)->GetEvent();

	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pViewEvent = m_pSelectedEvents->GetAt(i);
		ASSERT(pViewEvent);
		if (pViewEvent && !pViewEvent->GetEvent()->IsEqualIDs(pEvent))
			return FALSE;
	}

	return TRUE;
}

CXTPCalendarViewEventsPtr CXTPCalendarView::GetSelectedEvents()
{
	CXTPCalendarViewEventsPtr ptrEventsWithViews = new CXTPCalendarViewEvents();
	CXTPCalendarEvents arEvents;

	ASSERT(m_pSelectedEvents);
	if (!ptrEventsWithViews || !m_pSelectedEvents)
	{
		return NULL;
	}

	int nCount = m_pSelectedEvents->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pViewEvent = m_pSelectedEvents->GetAt(i);
		CXTPCalendarEvent* pEvent = pViewEvent ? pViewEvent->GetEvent() : NULL;

		if (pEvent && arEvents.Find(pEvent) < 0)
		{
			arEvents.Add(pEvent, TRUE);
			ptrEventsWithViews->Add(pViewEvent, TRUE);
		}
	}

	return ptrEventsWithViews;
}

BOOL CXTPCalendarView::GetSelection(COleDateTime* pBegin, COleDateTime* pEnd,
									BOOL* pbAllDayEvent, int* pnGroupIndex,
									COleDateTimeSpan* pspSelectionResolution)
{
	if (!m_selectedBlock.IsValid())
	{
		return FALSE;
	}

	if (pspSelectionResolution)
	{
		*pspSelectionResolution = COleDateTimeSpan(1, 0, 0, 0);
	}
	if (pbAllDayEvent)
	{
		*pbAllDayEvent = TRUE;
	}
	if (pnGroupIndex)
	{
		*pnGroupIndex = m_selectedBlock.nGroupIndex;
	}

	//---------------------------------------------------------------------------
	if (pBegin)
	{
		*pBegin = min(m_selectedBlock.dtBegin, m_selectedBlock.dtEnd);
	}
	if (pEnd)
	{
		*pEnd = max(m_selectedBlock.dtBegin, m_selectedBlock.dtEnd) + COleDateTimeSpan(1, 0, 0, 0);
	}

	return TRUE;

}

void CXTPCalendarView::SetSelection(const COleDateTime& dtBegin, const COleDateTime& dtEnd,
									BOOL bAllDayEvent, int nGroupIndex)
{
	m_selectedBlock.dtBegin = dtBegin;
	m_selectedBlock.dtEnd = dtEnd;
	m_selectedBlock.bAllDayEvent = bAllDayEvent;
	m_selectedBlock.nGroupIndex = nGroupIndex;

	CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
}

void CXTPCalendarView::ResetSelection()
{
	m_selectedBlock.dtBegin.SetStatus(COleDateTime::null);
	m_selectedBlock.dtEnd.SetStatus(COleDateTime::null);

	m_selectedBlock.bAllDayEvent = FALSE;
	m_selectedBlock.nGroupIndex = -1;

	CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
}

BOOL CXTPCalendarView::SelectionContains(COleDateTime date, int nGroupIndex)
{
	COleDateTime dtBegin;
	COleDateTime dtEnd;
	int nSelGroupIndex;

	if (GetSelection(&dtBegin, &dtEnd, NULL, &nSelGroupIndex))
	{
		if (nGroupIndex < 0 || nGroupIndex == nSelGroupIndex)
		{
			return (dtBegin <= date && (double)date <= (double)dtEnd - XTP_HALF_SECOND);
		}
	}

	return FALSE;
}


COleDateTime CXTPCalendarView::GetNextTimeEditByTAB()
{
	ASSERT(GetViewDayCount() != 0);

	COleDateTime dtDay;

	if (m_selectedBlock.dtEnd.GetStatus() == COleDateTime::valid)
	{
		dtDay = m_selectedBlock.dtEnd;
	}
	else
	{
		dtDay = GetViewDayDate(0);
	}

	dtDay = CXTPCalendarUtils::ResetTime(dtDay);
	return dtDay;
}

void CXTPCalendarView::UpdateNextTimeEditByTAB(COleDateTime dtNext, BOOL /*bReverse*/, BOOL /*bReset*/)
{
	COleDateTime dtNextDay = CXTPCalendarUtils::ResetTime(dtNext);
	COleDateTime dtCurrDay = CXTPCalendarUtils::ResetTime(GetNextTimeEditByTAB());
	if (dtNextDay != dtCurrDay)
	{
		SelectDay(dtNextDay);
	}
}

CXTPCalendarViewEvent* CXTPCalendarView::FindEventToEditByTAB(COleDateTime dtMinStart,
								BOOL bReverse, CXTPCalendarEvent* pAfterEvent)
{
	COleDateTime dtMinStartDay = CXTPCalendarUtils::ResetTime(dtMinStart);
	COleDateTime dtMinStart2 = dtMinStartDay + COleDateTimeSpan(1, 0, 0, 0);

	BOOL bAfterEventWas = pAfterEvent == NULL;
	if (pAfterEvent)
	{
		COleDateTime dtAEStartDay = CXTPCalendarUtils::ResetTime(pAfterEvent->GetStartTime());
		if (bReverse ? dtAEStartDay > dtMinStartDay : dtAEStartDay < dtMinStartDay)
		{
			bAfterEventWas = TRUE;
		}
	}

	int nDays = GetViewDayCount();
	for (int nDay = bReverse ? nDays-1 : 0; bReverse ? nDay >= 0 : nDay < nDays;
		bReverse ? nDay-- : nDay++)
	{
		CXTPCalendarViewDay* pVDay = GetViewDay_(nDay);
		if (!pVDay)
		{
			ASSERT(FALSE);
			continue;
		}

		COleDateTime dtDay = pVDay->GetDayDate();

		if (bReverse ? dtDay > dtMinStartDay : dtDay < dtMinStartDay)
		{
			continue;
		}

		int nGroupsCount = pVDay->GetViewGroupsCount();
		for (int nGroup = bReverse ? nGroupsCount - 1 : 0;
			 bReverse ? nGroup >= 0 : nGroup < nGroupsCount;
			 bReverse ? nGroup-- : nGroup++)
		{
			CXTPCalendarViewGroup* pViewGroup = pVDay->GetViewGroup_(nGroup);
			ASSERT(pViewGroup);
			if (!pViewGroup)
			{
				continue;
			}

			int nEvents = pViewGroup->GetViewEventsCount();
			for (int nEvent = bReverse ? nEvents - 1 : 0;
				 bReverse ? nEvent >= 0 : nEvent < nEvents;
				 bReverse ? nEvent-- : nEvent++)
			{
				CXTPCalendarViewEvent* pVEvent = pViewGroup->GetViewEvent_(nEvent);
				if (!pVEvent || !pVEvent->GetEvent())
				{
					ASSERT(FALSE);
					continue;
				}

				if (pAfterEvent)
				{
					if (pAfterEvent->IsEqualIDs(pVEvent->GetEvent()))
					{
						bAfterEventWas = TRUE;
						continue;
					}
				}
				COleDateTime dtES = pVEvent->GetEvent()->GetStartTime();
				COleDateTime dtESDay = CXTPCalendarUtils::ResetTime(dtES);

				if (bReverse ? (dtES > dtMinStart2 || dtESDay != dtDay) :
								(dtES < dtMinStart || dtESDay < dtDay))
				{
					continue;
				}

				if (bAfterEventWas)
				{
					return pVEvent;
				}
			}
		}
	}
	return NULL;
}

void CXTPCalendarView::UnselectAllEvents()
{
	if (!m_pSelectedEvents || !GetCalendarControl())
	{
		return;
	}

	int nCount = m_pSelectedEvents->GetCount();
	m_pSelectedEvents->RemoveAll();

	if (nCount)
	{
		CXTPCalendarControl::CUpdateContext updateContext(GetCalendarControl(), xtpCalendarUpdateRedraw);
		CSelectionChangedContext selChanged(this, xtpCalendarSelectionEvents);
	}
}

void CXTPCalendarView::_ReSelectSelectEvents()
{
	if (!m_pSelectedEvents)
	{
		return;
	}

	CXTPCalendarViewEventsPtr ptrSelEvents = GetSelectedEvents();

	m_pSelectedEvents->RemoveAll();

	if (ptrSelEvents)
	{
		int nCount = ptrSelEvents->GetCount();
		for (int i = 0; i < nCount; i++)
		{
			if (ptrSelEvents->GetAt(i))
			{
				SelectEvent(ptrSelEvents->GetAt(i)->GetEvent());
			}
		}
	}
}

void CXTPCalendarView::SelectDays(COleDateTime dtNewSel)
{
	CXTPCalendarControl::CUpdateContext updateContext(GetCalendarControl(), xtpCalendarUpdateRedraw);
	m_selectedBlock.dtEnd = dtNewSel;

	CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
}

void CXTPCalendarView::SelectDay(COleDateTime dtSelDay, int nGroupIndex)
{
	CXTPCalendarControl::CUpdateContext updateContext(GetCalendarControl(), xtpCalendarUpdateRedraw);

	m_selectedBlock.dtBegin = m_selectedBlock.dtEnd = dtSelDay;
	m_selectedBlock.nGroupIndex = nGroupIndex;

	CSelectionChangedContext selChanged(this, xtpCalendarSelectionDays);
}

void CXTPCalendarView::SelectDay(CXTPCalendarViewDay* pDay, int nGroupIndex)
{
	ASSERT(pDay);
	if (pDay)
	{
		SelectDay(pDay->GetDayDate(), nGroupIndex);
	}
}

void CXTPCalendarView::SelectViewEvent(CXTPCalendarViewEvent* pViewEvent, BOOL bSelect)
{
	if (!m_pSelectedEvents || !pViewEvent || !pViewEvent->GetEvent())
	{
		ASSERT(FALSE);
		return;
	}

	m_pSelectedEvents->Remove(pViewEvent->GetEvent());

	if (bSelect)
	{
		pViewEvent->InternalAddRef();
		m_pSelectedEvents->Add(pViewEvent);
	}
	CSelectionChangedContext selChanged(this, xtpCalendarSelectionEvents);
}

CXTPCalendarViewEvent* CXTPCalendarView::GetLastSelectedViewEvent()
{
	if (m_pSelectedEvents && m_pSelectedEvents->GetCount() > 0)
		return m_pSelectedEvents->GetAt(m_pSelectedEvents->GetCount() - 1);

	return NULL;
}

void CXTPCalendarView::SelectEvent(CXTPCalendarEvent* pEvent, BOOL bSelect)
{
	if (!pEvent)
	{
		ASSERT(FALSE);
		return;
	}
	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewDay* pViewDay = GetViewDay_(i);
		if (!pViewDay)
		{
			ASSERT(FALSE);
			continue;
		}

		int nGroupsCount = pViewDay->GetViewGroupsCount();
		for (int g = 0; g < nGroupsCount; g++)
		{
			CXTPCalendarViewGroup* pViewGroup = pViewDay->GetViewGroup_(g);
			ASSERT(pViewGroup);
			if (!pViewGroup)
			{
				continue;
			}

			int nECount = pViewGroup->GetViewEventsCount();
			for (int j = 0; j < nECount; j++)
			{
				CXTPCalendarViewEvent* pViewEvent = pViewGroup->GetViewEvent_(j);
				CXTPCalendarEvent* pEv = pViewEvent ? pViewEvent->GetEvent() : NULL;
				ASSERT(pEv);
				if (pEv && pEvent->IsEqualIDs(pEv) && pEvent->IsEqualStartEnd(pEv))
				{
					CXTPCalendarControl::CUpdateContext updateContext(GetCalendarControl(), xtpCalendarUpdateRedraw);
					SelectViewEvent(pViewEvent, bSelect);
				}
			}
		}
	}
}

BOOL CXTPCalendarView::OnDragging(CPoint /*point*/, XTP_CALENDAR_HITTESTINFO* pHitTest)
{
	ASSERT(pHitTest);
	if (!pHitTest || !m_ptrDraggingEventNew || m_eDraggingMode == xtpCalendaDragModeEditSubject)
	{
		return FALSE;
	}
	//---------------------------------------------------------------------------
	if (pHitTest->pViewDay)
	{
		if (m_eDraggingMode == xtpCalendaDragModeResizeBegin || m_eDraggingMode == xtpCalendaDragModeResizeEnd)
		{
			SelectDay(pHitTest->pViewDay, pHitTest->nGroup);
		}
	}
	//---------------------------------------------------------------------------
	COleDateTime dtNewTime;

	if (_IsDragModeCopyMove(m_eDraggingMode))
	{
		if (!pHitTest->bTimePartValid)
		{
			dtNewTime = CXTPCalendarUtils::UpdateDate(m_ptrDraggingEventNew->GetStartTime(), pHitTest->dt);
		}
		else
		{
			dtNewTime = pHitTest->dt;
		}
		dtNewTime -= m_spDraggingStartOffset;

		BOOL bChanged = FALSE;
		//------------------------------------
		if (!CXTPCalendarUtils::IsEqual(dtNewTime, m_ptrDraggingEventNew->GetStartTime()))
		{
			m_ptrDraggingEventNew->MoveEvent(dtNewTime);
			bChanged = TRUE;
		}

		//***
		if (bChanged)
		{
			return TRUE;
		}
	}
	else if (m_eDraggingMode == xtpCalendaDragModeResizeBegin)
	{
		dtNewTime = pHitTest->dt;
		COleDateTime dtEventEnd = m_ptrDraggingEventNew->GetEndTime();

		if (!CXTPCalendarUtils::IsEqual(dtNewTime, m_ptrDraggingEventNew->GetStartTime()))
		{
			if (dtNewTime > dtEventEnd)
			{
				dtNewTime = dtEventEnd;
			}

			m_ptrDraggingEventNew->SetStartTime(dtNewTime);

			BOOL bIsZeroStartEnd = CXTPCalendarUtils::IsZeroTime(m_ptrDraggingEventNew->GetStartTime()) &&
									CXTPCalendarUtils::IsZeroTime(m_ptrDraggingEventNew->GetEndTime());

			BOOL bIsZeroLen = CXTPCalendarUtils::IsEqual(m_ptrDraggingEventNew->GetStartTime(),
									m_ptrDraggingEventNew->GetEndTime());

			m_ptrDraggingEventNew->SetAllDayEvent(bIsZeroStartEnd && !bIsZeroLen);

			return TRUE;
		}
	}
	else if (m_eDraggingMode == xtpCalendaDragModeResizeEnd)
	{
		dtNewTime = pHitTest->dt;

		if (!pHitTest->bTimePartValid)
		{
			dtNewTime += COleDateTimeSpan(1, 0, 0, 0);
		}

		COleDateTime dtEventStart = m_ptrDraggingEventNew->GetStartTime();
		if (!CXTPCalendarUtils::IsEqual(dtNewTime, m_ptrDraggingEventNew->GetEndTime()))
		{
			if ((pHitTest->uHitCode & xtpCalendarHitTestDayViewTimeScale) &&
				(pHitTest->pViewDay == NULL))
			{
				dtNewTime.SetDateTime(
					dtEventStart.GetYear(), dtEventStart.GetMonth(), dtEventStart.GetDay(),
					dtNewTime.GetHour(), dtNewTime.GetMinute(), dtNewTime.GetSecond());
			}

			if (dtNewTime < dtEventStart)
			{
				dtNewTime = dtEventStart;
			}

			m_ptrDraggingEventNew->SetEndTime(dtNewTime);

			BOOL bIsZeroStartEnd = CXTPCalendarUtils::IsZeroTime(m_ptrDraggingEventNew->GetStartTime()) &&
									CXTPCalendarUtils::IsZeroTime(m_ptrDraggingEventNew->GetEndTime());

			BOOL bIsZeroLen = CXTPCalendarUtils::IsEqual(m_ptrDraggingEventNew->GetStartTime(),
									m_ptrDraggingEventNew->GetEndTime());

			m_ptrDraggingEventNew->SetAllDayEvent(bIsZeroStartEnd && !bIsZeroLen);

			return TRUE;
		}
	}

	return FALSE;
}

COleDateTime CXTPCalendarView::GetLastSelectedDate()
{
	if (m_selectedBlock.dtEnd.GetStatus() != COleDateTime::valid)
	{
		return GetViewDayDate(GetViewDayCount() / 2);
	}
	return CXTPCalendarUtils::ResetTime(m_selectedBlock.dtEnd);
}

int CXTPCalendarView::FindLastSelectedDay()
{
	COleDateTime dt = GetLastSelectedDate();

	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewDay* pViewDay = GetViewDay_(i);
		ASSERT(pViewDay);
		if (pViewDay && CXTPCalendarUtils::IsEqual(pViewDay->GetDayDate(), dt))
			return i;
	}
	return -1;
}

void CXTPCalendarView::ProcessDaySelection(XTP_CALENDAR_HITTESTINFO* pInfo, UINT nFlags)
{
	ASSERT(pInfo);
	if (!pInfo)
	{
		UnselectAllEvents();
		return;
	}

	if (nFlags & MK_SHIFT)
	{
		COleDateTime dtNewSelDay = GetViewDayDate(pInfo->nDay);
		SelectDays(dtNewSelDay);
	}
	else
	{
		UnselectAllEvents();

		ASSERT(pInfo->pViewDay);
		if (pInfo->pViewDay)
		{
			pInfo->pViewDay->SetSelected();
		}
	}
}

void CXTPCalendarView::ProcessEventSelection(XTP_CALENDAR_HITTESTINFO* pInfo, UINT nFlags)
{
	ASSERT(pInfo);
	if (!pInfo)
	{
		return;
	}

	BOOL bUnselectEvents = !(nFlags & (MK_SHIFT | MK_CONTROL));
	if (bUnselectEvents)
		UnselectAllEvents();

	ASSERT(pInfo->pViewDay);
	ASSERT(pInfo->pViewEvent);

	if (pInfo->pViewDay)
	{
		pInfo->pViewDay->SetSelected();
	}

	if (pInfo->pViewEvent)
	{

		BOOL bSelect = TRUE;
		if (nFlags & MK_CONTROL)
		{
			bSelect = !pInfo->pViewEvent->IsSelected();
		}
		SelectEvent(pInfo->pViewEvent->GetEvent(), bSelect);
	}
}

UINT CXTPCalendarView::SetTimer(UINT uTimeOut_ms)
{
	if (GetCalendarControl())
	{
		return GetCalendarControl()->SetTimer(uTimeOut_ms);
	}
	return 0;
}

void CXTPCalendarView::KillTimer(UINT uTimerID)
{
	if (GetCalendarControl())
	{
		VERIFY(GetCalendarControl()->KillTimer(uTimerID));
	}
}

BOOL CXTPCalendarView::OnTimer(UINT_PTR uTimerID)
{
	if (TBase::OnTimer(uTimerID))
	{
		return TRUE;
	}

	//---------------------------------------------------------------------------
	if (uTimerID == m_nTimerID_StartEditSubject)
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - KillTimer() [CXTPCalendarView::OnTimer(ID =%d)] \n"), uTimerID);

		KillTimer(m_nTimerID_StartEditSubject);
		m_nTimerID_StartEditSubject = 0;

		ASSERT(m_ptrEditingViewEvent);
		if (m_ptrEditingViewEvent)
		{
			TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - StartEditSubject(). [CXTPCalendarView::OnTimer(ID =%d)] \n"), uTimerID);

			EmptyUndoBuffer();
			StartEditSubject(m_ptrEditingViewEvent);
		}
		return TRUE;
	}
	else if (uTimerID == m_nKeyStateTimerID)
	{
		BOOL bIsCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
		XTPCalendarDraggingMode eDraggingMode = bIsCtrl ? xtpCalendaDragModeCopy : xtpCalendaDragModeMove;

		BOOL bPopulate = FALSE;

		if (!_IsDragModeCopyMove(m_eDraggingMode))
		{
			KillTimer(m_nKeyStateTimerID);
			m_nKeyStateTimerID = 0;
		}
		else if (m_eDraggingMode != eDraggingMode)
		{
			ASSERT(m_ptrDraggingEventNew);
			if (m_ptrDraggingEventNew)
			{
				CXTPCalendarViewEvent* pEventView = FindViewEvent(m_ptrDraggingEventNew, m_ptrDraggingEventNew->GetStartTime());
				if (!IsEditOperationDisabledNotify(_DragMod2Operation(eDraggingMode), pEventView))
				{
					m_eDraggingMode = eDraggingMode;
					bPopulate = TRUE;
				}
			}
		}

		if (bPopulate && GetCalendarControl())
		{
			GetCalendarControl()->Populate();

			GetCalendarControl()->m_mouseMode = m_eDraggingMode == xtpCalendaDragModeCopy ?
				xtpCalendarMouseEventDragCopy : xtpCalendarMouseEventDragMove;

			GetCalendarControl()->UpdateMouseCursor();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CXTPCalendarView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_nTimerID_StartEditSubject)
	{
		KillTimer(m_nTimerID_StartEditSubject);
		m_nTimerID_StartEditSubject = 0;
		m_ptrEditingViewEvent = NULL;
	}

	TRACE_MOUSE(_T("OnLButtonDblClk - [CXTPCalendarView] (nFlags =%x, point.x =%d, point.y =%d) \n"), nFlags, point.x, point.y);

	return TBase::OnLButtonDblClk(nFlags, point);
}

void CXTPCalendarView::OnDraw(CDC* pDC)
{
	if (GetTheme())
	{
		Draw2(pDC);
	}
	else
	{
		Draw(pDC);
	}
}

void CXTPCalendarView::AdjustLayout(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	UNREFERENCED_PARAMETER(pDC);

//  #pragma  NOTE("for beginning development time. uncomment later.")
//  ASSERT(!GetTheme());

	m_rcView = rcView;
	_CalculateEventTimeFormat();

	if (bCallPostAdjustLayout)
	{
		OnPostAdjustLayout();
	}
}

void CXTPCalendarView::AdjustLayout2(CDC* pDC, const CRect& rcView, BOOL bCallPostAdjustLayout)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(bCallPostAdjustLayout);

	ASSERT(GetTheme());

	m_rcView = rcView;
	//_CalculateEventTimeFormat();

//  if (bCallPostAdjustLayout)
//  {
//      OnPostAdjustLayout();
//  }
}

CXTPCalendarViewDay* CXTPCalendarView::_GetViewDay(const COleDateTime& dt)
{
	COleDateTime dtDay = CXTPCalendarUtils::ResetTime(dt);
	int nDaysCount = GetViewDayCount();

	if (!nDaysCount || !GetViewDay_(0))
	{
		return NULL;
	}

	COleDateTime dtDay0 = GetViewDay_(0)->GetDayDate();

	COleDateTimeSpan spShift = dtDay - dtDay0;

	ASSERT((double)spShift == (double)((int)(double)spShift) );

	int nShift = GETTOTAL_DAYS_DTS(spShift); //(int)(double)spShift

	if (nShift < 0)
	{
		nShift = 0;
	}
	if (nShift >= nDaysCount)
	{
		nShift = nDaysCount - 1;
	}

	if (nShift >= 0 && nShift < nDaysCount)
	{
		return GetViewDay_(nShift);
	}

	return NULL;
}

void CXTPCalendarView::StartEditNewEventInplace(LPCTSTR pcszInitialSubject)
{
	if (m_ptrDraggingEventOrig || m_ptrDraggingEventNew || IsEditingSubject())
	{
		MessageBeep(0);
		return;
	}

	BOOL bAllDayEvent = FALSE;
	//---------------------------------------------------------------------------
	COleDateTime dtSelBegin, dtSelEnd;
	int nGroup = 0;
	if (!GetSelection(&dtSelBegin, &dtSelEnd, &bAllDayEvent, &nGroup))
	{
		return;
	}

	//---------------------------------------------------------------------------
	CXTPCalendarData* pData = NULL; //XTP_SAFE_GET1(GetCalendarControl(), GetDataProvider(), NULL);

	CXTPCalendarViewDay* pViewDay = _GetViewDay(dtSelBegin);
	if (!pViewDay)
	{
		return;
	}
	CXTPCalendarViewGroup* pViewGroup = NULL;
	if (pViewDay && nGroup < pViewDay->GetViewGroupsCount() )
	{
		pViewGroup = pViewDay->GetViewGroup_(nGroup);
		pData = pViewGroup ? pViewGroup->GetDataProvider() : NULL;
	}

	//--------------------
	m_ptrDraggingEventNew = pData ? pData->CreateNewEvent() : NULL;
	if (!m_ptrDraggingEventNew)
	{
		return;
	}

	//---------------------------------------------------------------------------
	UINT uScheduleID = XTP_SAFE_GET1(pViewGroup, GetScheduleID(), 0);
	m_ptrDraggingEventNew->SetScheduleID(uScheduleID);

	m_ptrDraggingEventNew->SetStartTime(min(dtSelBegin, dtSelEnd));
	m_ptrDraggingEventNew->SetEndTime(max(dtSelBegin, dtSelEnd));

	m_ptrDraggingEventNew->SetBusyStatus(bAllDayEvent ? xtpCalendarBusyStatusFree : xtpCalendarBusyStatusBusy);
	m_ptrDraggingEventNew->SetAllDayEvent(bAllDayEvent);

	//===========================================================================
	XTP_SAFE_CALL1(GetCalendarControl(), Populate());
	XTP_SAFE_CALL1(GetCalendarControl(), AdjustLayout());

	//---------------------------------------------------------------------------
	COleDateTime dtEventDay = m_ptrDraggingEventNew->GetStartTime();
	dtEventDay = CXTPCalendarUtils::ResetTime(dtEventDay);

	CXTPCalendarViewEvent* pViewEvent = FindViewEvent(m_ptrDraggingEventNew, dtEventDay);

	if (!pViewEvent)
	{
		CancelDraggingEvent();
		return;
	}

	StartEditSubject(pViewEvent);

	CEdit* pEditor = DYNAMIC_DOWNCAST(CEdit, pViewEvent->GetSubjectEditor());

	if (pEditor)
	{
		pEditor->SetWindowText(pcszInitialSubject);
		int nInitTxtLen = (int)_tcsclen(pcszInitialSubject);
		pEditor->SetSel(nInitTxtLen, nInitTxtLen);
	}
}

void CXTPCalendarView::StartEditSubject(CXTPCalendarViewEvent* pViewEvent)
{
	if (!pViewEvent)
	{
		ASSERT(FALSE);
		return;
	}
	TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - StartEditSubject(). [CXTPCalendarView] \n"));

	//---------------------------------------------------------------------------
	if (m_nTimerID_StartEditSubject)
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - KillTimer() [CXTPCalendarView::StartEditSubject] \n"));

		KillTimer(m_nTimerID_StartEditSubject);
		m_nTimerID_StartEditSubject = 0;
	}
	//---------------------------------------------------------------------------

	m_ptrEditingViewEvent = pViewEvent->StartEditSubject();

	if (!m_ptrEditingViewEvent)
	{
		ASSERT(FALSE);
		return;
	}
	CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateRedraw);

	m_ptrEditingViewEvent->InternalAddRef();
	m_eDraggingMode = xtpCalendaDragModeEditSubject;

	if (!m_ptrDraggingEventOrig && !m_ptrDraggingEventNew)
	{
		SetDraggingEvent(m_ptrEditingViewEvent->GetEvent());
	}

	SelectEvent(m_ptrDraggingEventNew);
}

void CXTPCalendarView::EndEditSubject(XTPCalendarEndEditSubjectAction eAction, BOOL bUpdateControl)
{

	TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(). [CXTPCalendarView] \n"));

	m_eDraggingMode = xtpCalendaDragModeUnknown;

	// kill timeout request to edit subject
	if (m_nTimerID_StartEditSubject)
	{
		ASSERT(m_ptrEditingViewEvent && !m_ptrDraggingEventOrig && !m_ptrDraggingEventNew);

		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - KillTimer() [CXTPCalendarView::EndEditSubject] \n"));

		KillTimer(m_nTimerID_StartEditSubject);
		m_nTimerID_StartEditSubject = 0;
		m_ptrEditingViewEvent = NULL;
		return;
	}

	ASSERT(m_ptrEditingViewEvent && m_ptrDraggingEventNew);

	CXTPCalendarEventPtr ptrEventToSelect(m_ptrDraggingEventNew, TRUE);

	BOOL bPopulate = FALSE;
	CString strNewSubject;

	//- end edit subject operation in Event view (kill edit wnd) ----------------
	if (m_ptrEditingViewEvent)
	{
		strNewSubject = m_ptrEditingViewEvent->EndEditSubject();
	}
	m_ptrEditingViewEvent = NULL;

	//---------------------------------------------------------------------------
	if (m_ptrDraggingEventNew)
	{
		if (eAction == xtpCalendarEditSubjectCommit)
		{
			m_ptrDraggingEventNew->SetSubject(strNewSubject);

			bPopulate = CommitDraggingEvent();
			TRACE_DRAGGING(_T("Dragging - CommitDraggingEvent() = %d. [CXTPCalendarView::EndEditSubject] \n"), bPopulate);

		}
		else
			if (eAction == xtpCalendarEditSubjectCancel)
			{
				TRACE_DRAGGING(_T("Dragging - CancelDraggingEvent. [CXTPCalendarView::EndEditSubject] \n"));

				ptrEventToSelect.SetPtr(m_ptrDraggingEventOrig, TRUE);

				CancelDraggingEvent();
				bPopulate = TRUE;
			}
			else
			{
				ASSERT(FALSE);
			}
	}

	if (bUpdateControl && bPopulate && GetCalendarControl())
	{
		GetCalendarControl()->Populate();

		if (ptrEventToSelect)
		{
			SelectEvent(ptrEventToSelect, TRUE);
		}
	}
}

void CXTPCalendarView::OnUpdateEditingSubject(CString strNewSubject)
{
	TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - OnUpdateEditingSubject('%s'). [CXTPCalendarView] \n"), strNewSubject);

	ASSERT(m_ptrEditingViewEvent && m_ptrDraggingEventNew);

	if (m_ptrEditingViewEvent && m_ptrDraggingEventNew)
	{
		m_ptrDraggingEventNew->SetSubject(strNewSubject);
	}
}

CRect CXTPCalendarView::GetSubjectEditorRect() const
{
	ASSERT(m_ptrEditingViewEvent);
	if (m_ptrEditingViewEvent)
	{
		return m_ptrEditingViewEvent->GetSubjectEditorRect();
	}

	return CRect(INT_MAX, INT_MAX, INT_MAX, INT_MAX);
}


void CXTPCalendarView::_CalculateEventTimeFormat()
{
	////////////////////////////////////////////////////////////////////////////
	//  LOCALE_STIME    // time separator
	//
	//  LOCALE_ITLZERO  //leading zeros in time field
	//      0 No leading zeros for hours.
	//      1 Leading zeros for hours.
	//
	//  LOCALE_ITIME    // time format specifier
	//      0 AM / PM 12-hour format.
	//      1 24-hour format.
	//
	//  LOCALE_ITIMEMARKPOSN    // time marker position
	//      0 Use as suffix.
	//      1 Use as prefix.
	//
	//  LOCALE_S1159    // AM designator
	//  LOCALE_S2359    // PM designator
	////////////////////////////////////////////////////////////////////////////

	CString strTmp;

	CString strTimeSeparator = CXTPCalendarUtils::GetLocaleString(LOCALE_STIME, 16);

	strTmp = CXTPCalendarUtils::GetLocaleString(LOCALE_ITIME, 16);
	BOOL b24_HFormat = _ttoi(strTmp) == 1;

	strTmp = CXTPCalendarUtils::GetLocaleString(LOCALE_ITLZERO, 16);
	BOOL bH_LZero = _ttoi(strTmp) == 1;

	strTmp = CXTPCalendarUtils::GetLocaleString(LOCALE_ITIMEMARKPOSN, 16);
	BOOL bAM_PM_Prefix = _ttoi(strTmp) == 1;

	//---------------------------------------------------------------------------
	CString strTimeFormat;
	strTimeFormat += strTimeSeparator;
	strTimeFormat += _T("mm");

	if (b24_HFormat)
	{
		strTmp = bH_LZero ? _T("HH") : _T("H");
		strTimeFormat = strTmp + strTimeFormat;
	}
	else //12 AM/PM Format
	{
		strTmp = bH_LZero ? _T("hh") : _T("h");
		strTimeFormat = strTmp + strTimeFormat;

		if (bAM_PM_Prefix)
		{
			strTimeFormat = _T("tt ") + strTimeFormat;
		}
		else
		{
			strTimeFormat = strTimeFormat + _T("tt");
		}
	}
	//---------------------------------------------------------------------------
	m_EventTimeFormat.strFormat = strTimeFormat;
	m_EventTimeFormat.b24_HFormat = b24_HFormat;
	m_EventTimeFormat.strAM = CXTPCalendarUtils::GetLocaleString(LOCALE_S1159, 20);
	m_EventTimeFormat.strPM = CXTPCalendarUtils::GetLocaleString(LOCALE_S2359, 20);
}

CString CXTPCalendarView::FormatEventTime(COleDateTime dtTime)
{
	CString strTime;

	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(st));

	if (GETASSYSTEMTIME_DT(dtTime, st))
	{
		strTime = CXTPCalendarUtils::GetTimeFormat(&st, m_EventTimeFormat.strFormat);
	}

	if (strTime.IsEmpty())
	{
		ASSERT(FALSE);
		strTime.Format(_T("%d:%02d"), dtTime.GetHour(), dtTime.GetMinute());
	}

	//- make AM/PM as lower case ---------------------------------------
	if (!m_EventTimeFormat.b24_HFormat)
	{
		CString strAM_lower = m_EventTimeFormat.strAM;
		CString strPM_lower = m_EventTimeFormat.strPM;

		strAM_lower.MakeLower();
		strPM_lower.MakeLower();

		REPLACE_S(strTime, m_EventTimeFormat.strAM, strAM_lower);
		REPLACE_S(strTime, m_EventTimeFormat.strPM, strPM_lower);
	}

	return strTime;
}

BOOL CXTPCalendarView::_Undo(CXTPCalendarEvent* pEvent)
{
	if (IsEditingSubject())
	{
		TRACE_EDIT_SUBJECT(_T("Edit SUBJECT - EndEditSubject(). [CXTPCalendarView::OnKeyDown] \n"));
		EndEditSubject(xtpCalendarEditSubjectCommit);
	}

	if (!GetCalendarControl())
	{
		return FALSE;
	}

	CXTPAutoResetValue<BOOL> autoSet_UpdateCtrlNf(GetCalendarControl()->m_bUpdateWhenEventChangedNotify);
	autoSet_UpdateCtrlNf = FALSE;

	//---------------------------------------------------------------------------
	CXTPCalendarEventPtr ptrEventNew;
	CXTPCalendarEventPtr ptrEventOld;

	if (pEvent)
	{
		ptrEventNew = m_UndoBuffer.UndoAllForEvent(pEvent);
		ptrEventOld = m_UndoBuffer.RedoAllForEvent(pEvent);
	}
	else
	{
		CXTPCalendarEventPtr ptrUndoEvent = m_UndoBuffer.GetFirstUndoEvent();
		if (ptrUndoEvent)
		{
			if (m_eUndoMode == xtpCalendarUndoModeUndo)
			{
				ptrEventNew = m_UndoBuffer.UndoAllForEvent(ptrUndoEvent);
				ptrEventOld = m_UndoBuffer.RedoAllForEvent(ptrUndoEvent);
				m_eUndoMode = xtpCalendarUndoModeRedo;
			}
			else
				if (m_eUndoMode == xtpCalendarUndoModeRedo)
				{
					ptrEventNew = m_UndoBuffer.RedoAllForEvent(ptrUndoEvent);
					ptrEventOld = m_UndoBuffer.UndoAllForEvent(ptrUndoEvent);
					m_eUndoMode = xtpCalendarUndoModeUndo;
				}
				else
				{
					ASSERT(FALSE);
					return FALSE;
				}
		}
	}

	//---------------------------------------------------------------------------
	if (ptrEventNew || ptrEventOld)
	{
		CXTPCalendarControl::CUpdateContext updateContext(m_pControl, xtpCalendarUpdateLayout | xtpCalendarUpdateRedraw);

		if (ptrEventNew && ptrEventOld && ptrEventNew->GetDataProvider())
		{
			VERIFY( ptrEventNew->GetDataProvider()->ChangeEvent(ptrEventNew) );
		}
		else if (ptrEventNew && !ptrEventOld && ptrEventNew->GetDataProvider())
		{
			VERIFY( ptrEventNew->GetDataProvider()->AddEvent(ptrEventNew) );
		}
		else if (!ptrEventNew && ptrEventOld && ptrEventOld->GetDataProvider())
		{
			VERIFY( ptrEventOld->GetDataProvider()->DeleteEvent(ptrEventOld) );
		}

		XTP_SAFE_CALL1(GetCalendarControl(), Populate());

		return TRUE;
	}
	return FALSE;
}

BOOL CXTPCalendarView::CanUndo()
{
	if (m_eUndoMode == xtpCalendarUndoModeUndo && m_UndoBuffer.GetCount())
	{
		return TRUE;
	}

	return _CanSubjectEditor(ID_EDIT_UNDO);
}

CWnd* CXTPCalendarView::_GetSubjectEditorWnd()
{
	if (m_ptrEditingViewEvent)
	{
		return m_ptrEditingViewEvent->GetSubjectEditor();
	}
	return NULL;
}

BOOL CXTPCalendarView::_CanSubjectEditor(UINT idEditCmd)
{
	CWnd* pSEditorWnd = _GetSubjectEditorWnd();
	if (!pSEditorWnd)
	{
		return FALSE;
	}
	CEdit* pSEditor = DYNAMIC_DOWNCAST(CEdit, pSEditorWnd);
	if (!pSEditor)
	{
		return FALSE;
	}

	BOOL bSEFocused = CWnd::GetFocus() &&
						CWnd::GetFocus()->GetSafeHwnd() == pSEditorWnd->GetSafeHwnd();
	if (!bSEFocused)
	{
		return FALSE;
	}

	int nSel1 = -1, nSel2 = -1;
	pSEditor->GetSel(nSel1, nSel2);

	switch (idEditCmd)
	{
	case ID_EDIT_UNDO:
		return pSEditor->CanUndo();
	case ID_EDIT_CUT:
		return nSel1 < nSel2;
	case ID_EDIT_COPY:
		return nSel1 < nSel2;
	case ID_EDIT_PASTE:
		return ::IsClipboardFormatAvailable(CF_TEXT);
	}

	return FALSE;
}

BOOL CXTPCalendarView::CanRedo()
{
	return m_eUndoMode == xtpCalendarUndoModeRedo && m_UndoBuffer.GetCount();
}

BOOL CXTPCalendarView::Undo()
{
	ASSERT(m_eUndoMode == xtpCalendarUndoModeUndo || _CanSubjectEditor(ID_EDIT_UNDO));
	return _Undo();
}

BOOL CXTPCalendarView::Redo()
{
	ASSERT(m_eUndoMode == xtpCalendarUndoModeRedo);
	return _Undo();
}

CString CXTPCalendarView::_GetMaxWidthFormat(CDC* pDC, CString strFormat, BOOL bMonth)
{
	if (!pDC)
	{
		ASSERT(FALSE);
		return _T("");
	}

	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(st));
	st.wDay = 20;
	st.wMonth = 1;
	st.wYear = (WORD)CXTPCalendarUtils::GetCurrentTime().GetYear();

	int nMax = 0;
	CString str;

	int nCount = bMonth ? 12 : 7;

	for (int i = 0; i < nCount; i++)
	{
		CString strResult = CXTPCalendarUtils::GetDateFormat(&st, strFormat);

		int nLen = pDC->GetTextExtent(strResult).cx;
		if (nMax < nLen)
		{
			str = strResult;
			nMax = nLen;
		}
		if (bMonth)
			st.wMonth++;
		else
			st.wDay++;
	}
	return str;
}

int CXTPCalendarView::_GetMaxWidth(CDC* pDC, CString strFormat)
{
	if (!pDC)
	{
		ASSERT(FALSE);
		return 1;
	}

	const TCHAR* chFormats[] =
		{
			_T("yyyy"), _T("yyy"), _T("yy"), _T("y"),
			_T("MMMM"), _T("MMM"),
			_T("dddd"), _T("ddd"), _T("dd"), _T("d")
		};

	for (int i = 0; i < _countof(chFormats); i++)
	{
		int nPos = strFormat.Find(chFormats[i]);
		if (nPos >= 0)
		{
			REPLACE_S(strFormat, chFormats[i], _GetMaxWidthFormat(pDC, chFormats[i], i < 2));
		}
	}


	return pDC->GetTextExtent(strFormat).cx;

}

void CXTPCalendarView::_RemoveYearPart(CString& strFormat)
{
	int nStartPos = -1, nEndPos = -1, i;
	BOOL bBracket = FALSE;
	for (i = 0; i < strFormat.GetLength(); i++)
	{
		TCHAR ch = strFormat[i];
		if (ch == _T('\''))
			bBracket = !bBracket;
		if (bBracket)
			continue;

		if (ch == _T('m') || ch == _T('M') || ch == _T('D') || ch == _T('d'))
		{
			if (nStartPos == -1)
				nStartPos = i;
			nEndPos = i;
		}
	}

	if (nStartPos != -1 && nEndPos > nStartPos)
	{
		strFormat = strFormat.Mid(nStartPos, nEndPos - nStartPos + 1);
	}
}

void CXTPCalendarView::_ReadDefaultHeaderFormats()
{
	m_strDayHeaderFormatDefaultLong = CXTPCalendarUtils::GetLocaleString(LOCALE_SLONGDATE, 81);

	_RemoveYearPart(m_strDayHeaderFormatDefaultLong);

	m_strDayHeaderFormatDefaultMiddle = m_strDayHeaderFormatDefaultLong;
	REPLACE_S(m_strDayHeaderFormatDefaultMiddle, _T("MMMM"), _T("MMM"));

	m_strDayHeaderFormatDefaultShort = m_strDayHeaderFormatDefaultMiddle;
	REPLACE_S(m_strDayHeaderFormatDefaultShort, _T("dddd"), _T("ddd"));

	m_strDayHeaderFormatDefaultShortest = CXTPCalendarUtils::GetLocaleString(LOCALE_SSHORTDATE, 81);
	_RemoveYearPart(m_strDayHeaderFormatDefaultShortest);
}

// nLevel = 0 - Long, 1 - Middle, 2 - Short, 3 - Shortest.
CString CXTPCalendarView::_GetDayHeaderFormat(int nLevel)
{
	switch (nLevel)
	{
	case 0:
		return m_strDayHeaderFormatLong.IsEmpty() ? m_strDayHeaderFormatDefaultLong : m_strDayHeaderFormatLong;
	case 1:
		return m_strDayHeaderFormatMiddle.IsEmpty() ? m_strDayHeaderFormatDefaultMiddle : m_strDayHeaderFormatMiddle;
	case 2:
		return m_strDayHeaderFormatShort.IsEmpty() ? m_strDayHeaderFormatDefaultShort : m_strDayHeaderFormatShort;
	case 3:
		return m_strDayHeaderFormatShortest.IsEmpty() ? m_strDayHeaderFormatDefaultShortest : m_strDayHeaderFormatShortest;
	}

	ASSERT(FALSE);
	return _T("");
}

void CXTPCalendarView::CalculateHeaderFormat(CDC* pDC, int nWidth, CXTPCalendarViewPart* pPart)
{
	if (!GetCalendarControl() || !pPart || !pDC)
	{
		ASSERT(FALSE);
		return;
	}

	_ReadDefaultHeaderFormats();

	CXTPFontDC fnt(pDC, &pPart->GetTextFont());
	int arBorder[] = {40, 35, 20, 0};

	for (int i = 0; i <= 3; i++)
	{
		m_strHeaderFormat = _GetDayHeaderFormat(i);
		int nLen = _GetMaxWidth(pDC, m_strHeaderFormat);

		if (nLen < nWidth - arBorder[i])
		{
			return;
		}
	}
}


COleDateTimeSpan CXTPCalendarView::GetEventDurationMin() const
{
	return COleDateTimeSpan(0.);
}

BOOL CXTPCalendarView::CanCut()
{
	return !IsEditingSubject() && HasSelectedViewEvent() ||
			IsEditingSubject() && _CanSubjectEditor(ID_EDIT_CUT);
}

BOOL CXTPCalendarView::CanCopy()
{
	return !IsEditingSubject() && HasSelectedViewEvent() ||
			IsEditingSubject() && _CanSubjectEditor(ID_EDIT_COPY);
}

BOOL CXTPCalendarView::CanPaste()
{
	if (!GetSelection())
	{
		return FALSE;
	}

	//- Register or get existing Clipboard Format ID -------------------------
	UINT uCF_Event = ::RegisterClipboardFormat(XTPCALENDARCTRL_CF_EVENT);

	BOOL bIsEventInClipboard = ::IsClipboardFormatAvailable(uCF_Event);

	return !IsEditingSubject() && bIsEventInClipboard ||
			IsEditingSubject() && _CanSubjectEditor(ID_EDIT_PASTE);
}


BOOL CXTPCalendarView::Cut()
{
	if (IsEditingSubject() && _GetSubjectEditorWnd())
	{
		if (CanCut())
		{
			_GetSubjectEditorWnd()->SendMessage(WM_CUT);
			return TRUE;
		}
		return FALSE;
	}

	//===========================================================================
	CXTPCalendarViewEventsPtr ptrSelViews = GetSelectedEvents();
	if (!ptrSelViews || !ptrSelViews->GetCount())
	{
		return FALSE;
	}

	if (OnBeforeEditOperationNotify(xtpCalendarEO_Cut, ptrSelViews))
	{
		return FALSE;
	}

	//------------------------------------------------------------------------
	DWORD dwCopyFlags = xtpCalendarCopyFlagCutMask;

	COleDataSource* pData = _CopySelectedEvents(dwCopyFlags);
	if (!pData)
	{
		return FALSE;
	}
	pData->SetClipboard();

	//------------------------------------------------------------------------
	int nCount = ptrSelViews->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pViewEvent = ptrSelViews->GetAt(i, FALSE);
		CXTPCalendarEvent* pEvent = pViewEvent ? pViewEvent->GetEvent() : NULL;

		VERIFY( XTP_SAFE_GET2(pEvent, GetDataProvider(), DeleteEvent(pEvent), FALSE) );
	}
	XTP_SAFE_CALL1(GetCalendarControl(), Populate());

	//---------------------------------------------------------------------------
	return TRUE;
}

BOOL CXTPCalendarView::Copy()
{
	if (IsEditingSubject() && _GetSubjectEditorWnd())
	{
		if (CanCopy())
		{
			_GetSubjectEditorWnd()->SendMessage(WM_COPY);
			return TRUE;
		}
		return FALSE;
	}

	//---------------------------------------------------------------------------
	CXTPCalendarViewEventsPtr ptrSelViews = GetSelectedEvents();
	if (!ptrSelViews || !ptrSelViews->GetCount())
	{
		return FALSE;
	}

	if (OnBeforeEditOperationNotify(xtpCalendarEO_Copy, ptrSelViews))
	{
		return FALSE;
	}

	//===========================================================================
	COleDataSource* pData = _CopySelectedEvents();
	if (!pData)
	{
		return FALSE;
	}

	pData->SetClipboard();
	return TRUE;
}

BOOL CXTPCalendarView::Paste()
{
	if (IsEditingSubject() && _GetSubjectEditorWnd())
	{
		if (CanPaste())
		{
			_GetSubjectEditorWnd()->SendMessage(WM_PASTE);
			return TRUE;
		}
		return FALSE;
	}

	//===========================================================================
	COleDateTime dtSelBegin, dtSelEnd;
	COleDateTimeSpan spSelectionResolution;
	if (!GetSelection(&dtSelBegin, &dtSelEnd, NULL, NULL, &spSelectionResolution))
	{
		return FALSE;
	}
	dtSelBegin = min(dtSelBegin, dtSelEnd);

	// Attach a COleDataObject to the clipboard and paste the data
	COleDataObject objData;
	if (!objData.AttachClipboard())
	{
		return FALSE;
	}

	DWORD dwCopyFlags = 0;
	CXTPCalendarEventsPtr ptrEventsFromCB = _ReadEventsFromClipboard(&objData, &dwCopyFlags);

	if (!ptrEventsFromCB)
	{
		return FALSE;
	}

	if (!GetCalendarControl())
	{
		return FALSE;
	}

	CXTPAutoResetValue<BOOL> autoSet_UpdateCtrlNf(GetCalendarControl()->m_bUpdateWhenEventChangedNotify);
	autoSet_UpdateCtrlNf = FALSE;

	//---------------------------------------------------------------------------
	COleDateTimeSpan spStartShift;

	int nCount = ptrEventsFromCB->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		BOOL bUseChangeEvent = FALSE;

		CXTPCalendarEvent* pNewEvent = ptrEventsFromCB->GetAt(i, FALSE);
		if (!pNewEvent)
		{
			ASSERT(FALSE);
			continue;
		}
		int nRState = pNewEvent->GetRecurrenceState();
		if (nRState == xtpCalendarRecurrenceMaster)
		{
			ASSERT(FALSE); // WARNING. Master event in the clipboard - ???
			VERIFY(pNewEvent->RemoveRecurrence());
		}

		//---------------------------------------------------------------------------
		if (nRState == xtpCalendarRecurrenceOccurrence &&
			dwCopyFlags & xtpCalendarCopyFlagROccurrence2RException)
		{
			VERIFY(pNewEvent->MakeAsRException());
			bUseChangeEvent = TRUE;
		}
		else
		{
			VERIFY(pNewEvent->RemoveRecurrence());
		}
		// Update recurrence state
		nRState = pNewEvent->GetRecurrenceState();
		//---------------------------------------------------------------------------
		if (nRState == xtpCalendarRecurrenceException &&
			dwCopyFlags & xtpCalendarCopyFlagKeepRException)
		{
			CXTPCalendarRecurrencePatternPtr ptrPattern = pNewEvent->GetRecurrencePattern();
			bUseChangeEvent = ptrPattern != NULL;
		}

		if (!bUseChangeEvent && nRState == xtpCalendarRecurrenceException)
		{
			VERIFY( pNewEvent->RemoveRecurrence() );
		}

		//---------------------------------------------------------------------------
		COleDateTime dtEventStart = pNewEvent->GetStartTime();

		if (i == 0)
		{
			spStartShift = dtSelBegin - dtEventStart;
			spStartShift += ((double)spStartShift < 0 ? -1 : 1) * XTP_HALF_SECOND;

			double dCells = (double)spStartShift / (double)spSelectionResolution;

			int nCells = (int)dCells; //+ (dCells >= 0 ? 1 : 0);

			double dCellDrobTime = fabs(dCells - (DWORD)dCells)*(double)spSelectionResolution;
			double dCellDrobTimeLimit = XTP_HALF_SECOND*2*10;

			if (dCells >= 0 && dCellDrobTime > dCellDrobTimeLimit)
			{
				nCells++;
			}
			spStartShift = (double)spSelectionResolution * (double)nCells;
		}
		dtEventStart += spStartShift;

		pNewEvent->MoveEvent(dtEventStart);

		//*******************************************************
		if (OnBeforePasteNotify(pNewEvent, bUseChangeEvent))
		{
			continue;
		}
		//*******************************************************

		//---------------------------------------------------------------------------
		if (bUseChangeEvent)
		{
			VERIFY( XTP_SAFE_GET2(pNewEvent, GetDataProvider(), ChangeEvent(pNewEvent), FALSE) );
		}
		else
		{
			VERIFY( XTP_SAFE_GET2(pNewEvent, GetDataProvider(), AddEvent(pNewEvent), FALSE) );
		}
	}

	//---------------------------------------------------------------------------
	if (dwCopyFlags & xtpCalendarCopyFlagClearCBAfterPaste)
	{
		if (::OpenClipboard(NULL))
		{
			::EmptyClipboard();
			::CloseClipboard();
		}
	}
	//---------------------------------------------------------------------------

	XTP_SAFE_CALL1(GetCalendarControl(), Populate());

	return TRUE;
}

CXTPCalendarEventsPtr CXTPCalendarView::_ReadEventsFromClipboard(COleDataObject* pData, DWORD* pdwCopyFlags)
{
	//- Register or get existing Clipboard Format ID -------------------------
	CLIPFORMAT uCF_Event = (CLIPFORMAT)::RegisterClipboardFormat(XTPCALENDARCTRL_CF_EVENT);

	ASSERT(pData);
	if (!pData || !pData->IsDataAvailable(uCF_Event))
	{
		return NULL;
	}

	CXTPCalendarEventsPtr ptrEvents = new CXTPCalendarEvents();
	if (!ptrEvents)
	{
		return NULL;
	}

	const int cErrTextSize = 1024;
	TCHAR szErrText[cErrTextSize + 1];

	try
	{
		HGLOBAL hData = pData->GetGlobalData(uCF_Event);
		if (!hData)
		{
			return NULL;
		}

		CSharedFile fileSahred;
		fileSahred.SetHandle(hData, FALSE);
		CArchive ar(&fileSahred, CArchive::load);

		CXTPPropExchangeArchive px(ar);

		long nSchema = 0;
		DWORD dwCopyFlags = 0;

		PX_Long(&px, _T("Version"), (long&)nSchema);
		px.ExchangeLocale();
		PX_DWord(&px, _T("CopyFlags"), (DWORD&)dwCopyFlags);

		if (nSchema != XTP_CLIPBOARD_EVENTS_DATA_VER)
		{
			ASSERT(FALSE);
			return NULL;
		}

		CXTPPropExchangeSection sec(px.GetSection(_T("Events")));
		CXTPPropExchangeEnumeratorPtr pEnumerator(sec->GetEnumerator(_T("Event")));
		POSITION pos = pEnumerator->GetPosition();

		UINT uScheduleID = 0;
		CXTPCalendarData* pDPsel = _GetDataProviderBySelection(&uScheduleID);

		while (pos)
		{
			CXTPPropExchangeSection secEvent(pEnumerator->GetNext(pos));

			CXTPCalendarData* pDPevent = NULL;

			//-- get data provider --------------------
			CString strDataProvider;
			PX_String(&secEvent, _T("DataProvider_ConnectionString"), strDataProvider, _T(""));

			if (!strDataProvider.IsEmpty())
			{
				pDPevent = _GetDataProviderByConnStr(strDataProvider);
			}
			CXTPCalendarData* pDataProvider = pDPevent ? pDPevent : pDPsel;
			ASSERT(pDataProvider);

			//-- create event and read its data ---------
			CXTPCalendarEventPtr ptrEvent = pDataProvider ? pDataProvider->CreateNewEvent() : NULL;
			if (!ptrEvent)
			{
				return NULL;
			}
			ptrEvent->DoPropExchange(&secEvent);

			//-----------------------------------
			if (!pDPevent || !_IsScheduleVisible(pDPevent, ptrEvent->GetScheduleID()))
			{
				ptrEvent->SetScheduleID(uScheduleID);
			}

			ptrEvents->Add(ptrEvent, TRUE);
		}
		ar.Close(); // detach from file

		//--------------------------------------------------------------------
		if (pdwCopyFlags)
		{
			*pdwCopyFlags = dwCopyFlags;
		}

		return ptrEvents;
	}
	catch(CArchiveException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPCalendarView::_ReadEventsFromClipboard() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPCalendarView::_ReadEventsFromClipboard() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(...)
	{
		TRACE(_T("EXCEPTION: CXTPCalendarView::_ReadEventsFromClipboard() - Unhandled Exception!\n"));
	}

	ASSERT(FALSE);
	return NULL;
}

COleDataSource* CXTPCalendarView::_CopySelectedEvents(DWORD dwCopyFlags)
{
	CXTPCalendarViewEventsPtr ptrSelViews = GetSelectedEvents();
	if (!ptrSelViews || !ptrSelViews->GetCount())
	{
		return NULL;
	}
	//------------------------------------------------------------------------
	const int cErrTextSize = 1024;
	TCHAR szErrText[cErrTextSize + 1];

	try
	{
		CSharedFile fileSahred(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
		CArchive ar(&fileSahred, CArchive::store);

		CXTPPropExchangeArchive px(ar);

		long nSchema = XTP_CLIPBOARD_EVENTS_DATA_VER;
		PX_Long(&px, _T("Version"), (long&)nSchema);
		px.ExchangeLocale();
		PX_DWord(&px, _T("CopyFlags"), (DWORD&)dwCopyFlags);

		int nCount = ptrSelViews->GetCount();

		CXTPPropExchangeSection sec(px.GetSection(_T("Events")));
		CXTPPropExchangeEnumeratorPtr pEnumerator(sec->GetEnumerator(_T("Event")));
		POSITION posEnum = pEnumerator->GetPosition((DWORD)nCount);

		for (int i = 0; i < nCount; i++)
		{
			CXTPCalendarViewEvent* pViewEvent = ptrSelViews->GetAt(i, FALSE);
			CXTPCalendarEvent* pEvent = pViewEvent->GetEvent();
			if (!pEvent)
			{
				ASSERT(FALSE);
				continue;
			}

			CXTPPropExchangeSection secEvent(pEnumerator->GetNext(posEnum));

			CString strDataProvider = XTP_SAFE_GET2(pEvent, GetDataProvider(), GetConnectionString(), _T(""));
			PX_String(&secEvent, _T("DataProvider_ConnectionString"), strDataProvider);

			pEvent->DoPropExchange(&secEvent);
		}
		ar.Close(); // perform Flush() and detach from file

		//- Prepare Global data ----------------------------------------------
		DWORD dwDataSize = (DWORD)fileSahred.GetLength();
		HGLOBAL hData = fileSahred.Detach();
		if (!hData)
		{
			ASSERT(FALSE);
			return NULL;
		}

		hData = ::GlobalReAlloc(hData, dwDataSize, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
		if (!hData)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//- Register Clipboard Format ----------------------------------------
		CLIPFORMAT uCF_Event = (CLIPFORMAT)::RegisterClipboardFormat(XTPCALENDARCTRL_CF_EVENT);

		// Cache data
		COleDataSource* pSource = new COleDataSource();
		if (pSource)
		{
			pSource->CacheGlobalData(uCF_Event, hData);
		}

		return pSource;
	}
	catch(CArchiveException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPCalendarView::_CopySelectedEvents() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(CFileException* pE)
	{
		if (pE->GetErrorMessage(szErrText, cErrTextSize))
		{
			TRACE(_T("EXCEPTION: CXTPCalendarView::_CopySelectedEvents() - %s\n"), szErrText);
		}
		pE->Delete();
	}
	catch(...)
	{
		TRACE(_T("EXCEPTION: CXTPCalendarView::_CopySelectedEvents() - Unhandled Exception!\n"));
	}

	ASSERT(FALSE);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////
CXTPCalendarPaintManager* CXTPCalendarView::GetPaintManager()
{
	ASSERT(m_pControl);
	return m_pControl ? m_pControl->GetPaintManager() : NULL;
}

CXTPCalendarTheme* CXTPCalendarView::GetTheme()
{
	ASSERT(m_pControl);
	return m_pControl ? m_pControl->GetTheme() : NULL;

}

void CXTPCalendarView::SendNotification(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam)
{
	if (m_pControl)
	{
		m_pControl->SendNotification(EventCode, wParam, lParam);
	}
}

BOOL CXTPCalendarView::IsEditOperationDisabledNotify(XTPCalendarEditOperation eOperation, CXTPCalendarViewEvent* pEventView)
{
	return _EditOperationNotify(XTP_NC_CALENDAR_IS_EVENT_EDIT_OPERATION_DISABLED,
								eOperation, pEventView);
}

BOOL CXTPCalendarView::OnBeforeEditOperationNotify(XTPCalendarEditOperation eOperation, CXTPCalendarViewEvent* pEventView)
{
	if (IsEditOperationDisabledNotify(eOperation, pEventView))
	{
		return TRUE;
	}

	return _EditOperationNotify(XTP_NC_CALENDAR_BEFORE_EVENT_EDIT_OPERATION,
								eOperation, pEventView);
}

BOOL CXTPCalendarView::OnBeforeEditOperationNotify(XTPCalendarEditOperation eOperation, CXTPCalendarViewEvents* pEventViews)
{
	if (_EditOperationNotify(XTP_NC_CALENDAR_IS_EVENT_EDIT_OPERATION_DISABLED,
							eOperation, NULL, pEventViews))
	{
		return TRUE;
	}

	return _EditOperationNotify(XTP_NC_CALENDAR_BEFORE_EVENT_EDIT_OPERATION,
			eOperation, NULL, pEventViews);
}

BOOL CXTPCalendarView::OnBeforePasteNotify(CXTPCalendarEvent* pEvent, BOOL bPasteWillChangeExistingEvent)
{
	XTP_EVENT_PASTE_OPERATION_PARAMS opPasteParams;
	::ZeroMemory(&opPasteParams, sizeof(opPasteParams));

	opPasteParams.eOperation = xtpCalendarEO_Paste;

	opPasteParams.pEvent = pEvent;
	opPasteParams.bPasteWillChangeExistingEvent = bPasteWillChangeExistingEvent;

	BOOL bDisabledHandled = FALSE;

	if (m_pControl)
	{
		m_pControl->SendNotification(XTP_NC_CALENDAR_IS_EVENT_EDIT_OPERATION_DISABLED,
								(WPARAM)&opPasteParams, (LPARAM)&bDisabledHandled);
	}

	if (bDisabledHandled)
	{
		return TRUE;
	}

	if (m_pControl)
	{
		m_pControl->SendNotification(XTP_NC_CALENDAR_BEFORE_EVENT_EDIT_OPERATION,
								 (WPARAM)&opPasteParams, (LPARAM)&bDisabledHandled);
	}

	return bDisabledHandled;
}

BOOL CXTPCalendarView::OnInPlaceCreateEvent(LPCTSTR pcszInitialSubject)
{
	XTP_EVENT_IN_PLACE_CREATE_EVENT_OPERATION_PARAMS opCrParams;
	::ZeroMemory(&opCrParams, sizeof(opCrParams));

	opCrParams.eOperation = xtpCalendarEO_InPlaceCreateEvent;
	opCrParams.pcszInitialSubject = pcszInitialSubject;

	BOOL bDisabledHandled = FALSE;

	if (m_pControl)
	{
		m_pControl->SendNotification(XTP_NC_CALENDAR_IS_EVENT_EDIT_OPERATION_DISABLED,
									(WPARAM)&opCrParams, (LPARAM)&bDisabledHandled);
	}

	if (bDisabledHandled)
	{
		return TRUE;
	}

	if (m_pControl)
	{
		m_pControl->SendNotification(XTP_NC_CALENDAR_BEFORE_EVENT_EDIT_OPERATION,
									(WPARAM)&opCrParams, (LPARAM)&bDisabledHandled);
	}

	return bDisabledHandled;
}

BOOL CXTPCalendarView::_EditOperationNotify(XTP_NOTIFY_CODE ncEvent,
							XTPCalendarEditOperation eOperation,
							CXTPCalendarViewEvent* pEventView,
							CXTPCalendarViewEvents* pEventViews)
{
	ASSERT(pEventView || pEventViews && !(pEventView && pEventViews));

	XTP_EVENT_EDIT_OPERATION_PARAMS opParams;
	::ZeroMemory(&opParams, sizeof(opParams));

	CXTPCalendarViewEvents arEvents;
	arEvents.Add(pEventView, TRUE);

	opParams.eOperation = eOperation;
	opParams.pEventViews = pEventView ? &arEvents : pEventViews;

	BOOL bDisabledHandled = FALSE;

	if (m_pControl)
	{
		m_pControl->SendNotification(ncEvent, (WPARAM)&opParams, (LPARAM)&bDisabledHandled);
	}

	return bDisabledHandled;
}

XTPEnumCalendarUpdateResult CXTPCalendarView::OnEventChanged_InDataProvider(
								XTP_NOTIFY_CODE nfCode, CXTPCalendarEvent* pEvent)
{
	ASSERT(pEvent);
	if (!pEvent)
	{
		return xtpCalendar_Skip;
	}

	if (nfCode != XTP_NC_CALENDAREVENTWASADDED &&
		nfCode != XTP_NC_CALENDAREVENTWASDELETED &&
		nfCode != XTP_NC_CALENDAREVENTWASCHANGED)
	{
		ASSERT(FALSE);
		return xtpCalendar_Skip;
	}

	int nDaysCount = GetViewDayCount();
	COleDateTime dtFirstDay = nDaysCount ?  XTP_SAFE_GET1(GetViewDay_(0), GetDayDate(), (DATE)0) : (DATE)0;
	COleDateTime dtLastDay = nDaysCount ?  XTP_SAFE_GET1(GetViewDay_(nDaysCount - 1), GetDayDate(), (DATE)0) : (DATE)0;

	if (nfCode == XTP_NC_CALENDAREVENTWASADDED ||
		nfCode == XTP_NC_CALENDAREVENTWASDELETED)
	{
		if (CXTPCalendarUtils::ResetTime(pEvent->GetStartTime()) > dtLastDay ||
			CXTPCalendarUtils::ResetTime(pEvent->GetEndTime()) < dtFirstDay)
		{
			return xtpCalendar_Skip;
		}

		return xtpCalendar_Populate;
	}

	return xtpCalendar_Populate;

	//  *****
	//  An Idea how changing event processing can be optimized
	//  from xtpCalendar_Populate to xtpCalendar_Redraw or even xtpCalendar_Skip.
//
//  int nRState_new = pEvent->GetRecurrenceState();
//  if (nRState_new == xtpCalendarRecurrenceMaster) {
//      return xtpCalendar_Populate;
//  }
//
//  //-----------------------------------------------------------------------
//  BOOL bUpdateEvent = FALSE;
//  int nDays = GetViewDayCount();
//  for (int nDay = 0; nDay < nDays; nDay++)
//  {
//      CXTPCalendarViewDay* pDay = GetViewDay_(nDay);
//      ASSERT(pDay);
//      if (!pDay)
//          continue;
//
//
//      int nEvents = pDay->GetViewEventsCount();
//      for (int nEvent = 0; nEvent < nEvents; nEvent++)
//      {
//          CXTPCalendarViewEvent* pEventView = pDay->GetViewEvent_(nEvent);
//          if (!pEventView && !pEventView->GetEvent())
//          {
//              ASSERT(FALSE);
//              continue;
//          }
//          CXTPCalendarEvent* pEvent_I = pEventView->GetEvent();
//          if (pEvent_I->GetEventID() == dwEventID)
//          {
//              if (!bUpdateEvent)
//              {
//                  if (pEvent_I->IsEqualStartEnd(pEvent))
//                      bUpdateEvent = TRUE;
//                  else
//                      return xtpCalendar_Populate;
//              }
//
//              if (bUpdateEvent)
//              {
//                  int nRState_I = pEvent_I->GetRecurrenceState();
//                  //int nRState_new = pEvent->GetRecurrenceState();
//
//                  if (nRState_I == xtpCalendarRecurrenceOccurrence)
//                      pEvent_I->SetRecurrenceState_ExceptionOccurrence(xtpCalendarRecurrenceException);
//
//                  pEvent_I->Update(pEvent);
//
//                  if (nRState_new == xtpCalendarRecurrenceOccurrence ||
//                      nRState_new == xtpCalendarRecurrenceException)
//                  {
//                      ASSERT(nRState_I == xtpCalendarRecurrenceOccurrence ||
//                              nRState_I == xtpCalendarRecurrenceException);
//
//                      pEvent_I->SetRecurrenceState_ExceptionOccurrence((XTPCalendarEventRecurrenceState)nRState_new);
//                  }
//              }
//          }
//      }
//
//  }
//
//  //-----------------------------------------------------------------------
//  if (!bUpdateEvent &&
//      !(CXTPCalendarUtils::ResetTime(pEvent->GetStartTime()) > dtLastDay ||
//        CXTPCalendarUtils::ResetTime(pEvent->GetEndTime()) < dtFirstDay)
//     )
//  {
//      return xtpCalendar_Populate;
//  }
//  return bUpdateEvent ? xtpCalendar_Redraw : xtpCalendar_Skip;
}

CXTPCalendarData* CXTPCalendarView::_GetDataProviderBySelection(UINT* puScheduleID)
{
	COleDateTime dtSelBegin;
	int nGroup = 0;
	if (!GetSelection(&dtSelBegin, NULL, NULL, &nGroup))
	{
		return NULL;
	}

	//---------------------------------------------------------------------------
	CXTPCalendarViewDay* pViewDay = _GetViewDay(dtSelBegin);
	if (!pViewDay)
	{
		return NULL;
	}
	CXTPCalendarData* pData = NULL;
	CXTPCalendarViewGroup* pViewGroup = NULL;

	if (pViewDay && nGroup < pViewDay->GetViewGroupsCount() )
	{
		pViewGroup = pViewDay->GetViewGroup_(nGroup);
		if (pViewGroup)
		{
			pData = pViewGroup->GetDataProvider();
			if (puScheduleID)
			{
				*puScheduleID = pViewGroup->GetScheduleID();
			}
		}
	}

	return pData;
}

CXTPCalendarData* CXTPCalendarView::_GetDataProviderByConnStr(LPCTSTR pcszConnStr, BOOL bCompareNoCase)
{
	CXTPCalendarViewDay* pViewDay = GetViewDay_(0);

	COleDateTime dtSelBegin;
	if (GetSelection(&dtSelBegin))
	{
		pViewDay = _GetViewDay(dtSelBegin);
	}

	//---------------------------------------------------------------------------
	if (!pViewDay)
	{
		return NULL;
	}

	int nGCount = pViewDay->GetViewGroupsCount();
	for (int g = 0; g < nGCount; g++)
	{
		CXTPCalendarViewGroup* pViewGroup = pViewDay->GetViewGroup_(g);
		if (pViewGroup)
		{
			CXTPCalendarData* pData = pViewGroup->GetDataProvider();
			if (pData)
			{
				CString strConn_i = pData->GetConnectionString();
				int nCmp = bCompareNoCase ? strConn_i.CompareNoCase(pcszConnStr) : strConn_i.Compare(pcszConnStr);
				if (nCmp == 0)
				{
					 return pData;
				}
			}
		}
	}

	return NULL;
}

BOOL CXTPCalendarView::_IsScheduleVisible(CXTPCalendarData* pDataProvider, UINT uScheduleID)
{
	CXTPCalendarViewDay* pViewDay = GetViewDay_(0);

	COleDateTime dtSelBegin;
	if (GetSelection(&dtSelBegin))
	{
		pViewDay = _GetViewDay(dtSelBegin);
	}

	//---------------------------------------------------------------------------
	if (!pViewDay)
	{
		return FALSE;
	}

	int nGCount = pViewDay->GetViewGroupsCount();
	for (int g = 0; g < nGCount; g++)
	{
		CXTPCalendarViewGroup* pViewGroup = pViewDay->GetViewGroup_(g);

		CXTPCalendarData* pData = pViewGroup ? pViewGroup->GetDataProvider() : NULL;
		if (pData && pData == pDataProvider)
		{

			int nRCCount = XTP_SAFE_GET2(pViewGroup, GetResources(), GetCount(), 0);
			for (int i = 0; i < nRCCount; i++)
			{
				if (XTP_SAFE_GET3(pViewGroup, GetResources(), GetAt(i), ExistsScheduleID(uScheduleID), FALSE))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


CXTPCalendarWMHandler* CXTPCalendarView::GetChildHandlerAt(int nIndex)
{
	ASSERT_KINDOF(CXTPCalendarWMHandler, GetViewDay_(nIndex));
	return GetViewDay_(nIndex);
}

CXTPCalendarResources* CXTPCalendarView::GetResources()
{
	ASSERT(this);
	if (m_pResources)
	{
		return m_pResources;
	}

	return GetCalendarControl() ? GetCalendarControl()->GetResources() : NULL;
}

void CXTPCalendarView::SetResources(CXTPCalendarResources* pResources)
{
	BOOL bChanged = TRUE;

	if (!pResources)
	{
		bChanged = m_pResources != NULL;
		CMDTARGET_RELEASE(m_pResources);
	}
	else
	{
		if (!m_pResources)
		{
			m_pResources = new CXTPCalendarResources();
		}
		if (!m_pResources)
		{
			return;
		}

		// copy data to allow using self as input parameter
		CXTPCalendarResources arRes;
		arRes.Append(pResources);

		m_pResources->RemoveAll();
		m_pResources->Append(&arRes);
	}

	if (bChanged)
	{
		SendNotification(XTP_NC_CALENDAR_RESOURCES_WHERE_CHANGED, 1, (LPARAM)this);
	}
}

BOOL CXTPCalendarView::IsTodayVisible()
{
	COleDateTime dtToday = CXTPCalendarUtils::GetCurrentTime();
	dtToday = CXTPCalendarUtils::ResetTime(dtToday);

	int nCount = GetViewDayCount();
	for (int i = 0; i < nCount; i++)
	{
		COleDateTime dtDay = GetViewDayDate(i);
		if (CXTPCalendarUtils::IsEqual(dtDay, dtToday))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarView::CSelectionChanged_ContextData::CSelectionChanged_ContextData()
{
	m_nLockCount = 0;
	m_nLockCount_Day = 0;
	m_nLockCount_Event = 0;

	m_bRequest_Day = FALSE;
	m_bRequest_Event = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//  class CSelectionChangedContext
//
CXTPCalendarView::CSelectionChangedContext::CSelectionChangedContext(CXTPCalendarView* pView, int eSelType)
{
	if (!pView)
	{
		ASSERT(FALSE);
		return;
	}
	m_pView = pView;
	m_eSelType = eSelType;

	m_pView->m_cntSelChanged.m_nLockCount += eSelType == xtpCalendarSelectionUnknown ? 1 : 0;
	m_pView->m_cntSelChanged.m_nLockCount_Day += eSelType == xtpCalendarSelectionDays ? 1 : 0;
	m_pView->m_cntSelChanged.m_nLockCount_Event += eSelType == xtpCalendarSelectionEvents ? 1 : 0;
}

CXTPCalendarView::CSelectionChangedContext::~CSelectionChangedContext()
{
	if (!m_pView)
	{
		ASSERT(FALSE);
		return;
	}
	if (m_eSelType == xtpCalendarSelectionUnknown)
	{
		m_pView->m_cntSelChanged.m_nLockCount--;
	}

	if (m_eSelType == xtpCalendarSelectionDays)
	{
		m_pView->m_cntSelChanged.m_nLockCount_Day--;

		if (m_pView->m_cntSelChanged.m_nLockCount_Day == 0)
		{
			m_pView->m_cntSelChanged.m_bRequest_Day = TRUE;
		}
	}
	if (m_eSelType == xtpCalendarSelectionEvents)
	{
		m_pView->m_cntSelChanged.m_nLockCount_Event--;

		if (m_pView->m_cntSelChanged.m_nLockCount_Event == 0)
		{
			m_pView->m_cntSelChanged.m_bRequest_Event = TRUE;
		}
	}

	if (m_pView->m_cntSelChanged.m_nLockCount == 0 && m_pView->m_cntSelChanged.m_bRequest_Day)
	{
		m_pView->m_cntSelChanged.m_bRequest_Day = FALSE;
		m_pView->SendNotification(XTP_NC_CALENDAR_SELECTION_CHANGED, (WPARAM)xtpCalendarSelectionDays);
	}
	if (m_pView->m_cntSelChanged.m_nLockCount == 0 && m_pView->m_cntSelChanged.m_bRequest_Event)
	{
		m_pView->m_cntSelChanged.m_bRequest_Event = FALSE;
		m_pView->SendNotification(XTP_NC_CALENDAR_SELECTION_CHANGED, (WPARAM)xtpCalendarSelectionEvents);
	}
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarWMHandler
CXTPCalendarWMHandler::CXTPCalendarWMHandler()
{
}

int CXTPCalendarWMHandler::GetChildHandlersCount()
{
	return 0;
}

CXTPCalendarWMHandler* CXTPCalendarWMHandler::GetChildHandlerAt(int nIndex)
{
	UNREFERENCED_PARAMETER(nIndex);
	return NULL;
}

BOOL CXTPCalendarWMHandler::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bHandled = FALSE;

	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild && pChild->OnLButtonDown(nFlags, point))
		{
			bHandled = TRUE;
			break;
		}
	}

	return bHandled;
}

BOOL CXTPCalendarWMHandler::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bHandled = FALSE;

	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild && pChild->OnLButtonUp(nFlags, point))
		{
			bHandled = TRUE;
			break;
		}
	}

	return bHandled;
}

BOOL CXTPCalendarWMHandler::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	BOOL bHandled = FALSE;

	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild && pChild->OnLButtonDblClk(nFlags, point))
		{
			bHandled = TRUE;
			break;
		}
	}

	return bHandled;
}

void CXTPCalendarWMHandler::OnMouseMove(UINT nFlags, CPoint point)
{
	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild)
		{
			pChild->OnMouseMove(nFlags, point);
		}
	}
}

void CXTPCalendarWMHandler::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild)
		{
			pChild->OnChar(nChar, nRepCnt, nFlags);
		}
	}
}

void CXTPCalendarWMHandler::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild)
		{
			pChild->OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}
}

BOOL CXTPCalendarWMHandler::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bHandled = FALSE;

	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild && pChild->OnSysKeyDown(nChar, nRepCnt, nFlags))
		{
			bHandled = TRUE;
			break;
		}
	}

	return bHandled;
}


BOOL CXTPCalendarWMHandler::OnTimer(UINT_PTR uTimerID)
{
	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild && pChild->OnTimer(uTimerID))
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CXTPCalendarWMHandler::OnPostAdjustLayout()
{
	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild)
		{
			pChild->OnPostAdjustLayout();
		}
	}
}

void CXTPCalendarWMHandler::OnFinalRelease()
{
	OnBeforeDestroy();
	CCmdTarget::OnFinalRelease();
}

void CXTPCalendarWMHandler::OnBeforeDestroy()
{
	int nCount = GetChildHandlersCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarWMHandler* pChild = GetChildHandlerAt(i);
		if (pChild)
		{
			pChild->OnBeforeDestroy();
		}
	}
}
