// XTPCalendarControl.cpp : implementation file
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

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPSystemHelpers.h"

#include "XTPTopLevelWndMsgNotifier.h"
#include "XTPCalendarUtils.h"

#include "XTPCalendarPaintManager.h"
#include "XTPCalendarNotifications.h"

#include "XTPCalendarDayView.h"
#include "XTPCalendarWeekView.h"
#include "XTPCalendarMonthView.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarResource.h"

#include "XTPCalendarData.h"
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarDatabaseDataProvider.h"
#include "XTPCalendarMAPIDataProvider.h"
#include "XTPCalendarCustomDataProvider.h"

#include "XTPCalendarRemindersManager.h"
#include "XTPCalendarOccurSeriesChooseDlg.h"

#include "XTPCalendarTheme.h"
#include "XTPCalendarThemeOffice2007.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarControl

#define XTP_CALENDAR_POPULATE_REQUEST_TIMEOUT   200
#define XTP_CALENDAR_REDRAW_REQUEST_TIMEOUT     200

#define XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID   1
#define XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMEOUT   250

#define XTP_CALENDAR_FIRST_DYN_TIMERID          10

//#ifdef _DEBUG
//  const COleDateTimeSpan cspRemindersUpdatePeriod(0, 0,15,0);
//#else
	const COleDateTimeSpan cspRemindersUpdatePeriod(0, 2,0,0);
//#endif

/////////////////////////////////////////////////////////////////////////////
BOOL IsEventExists(CXTPCalendarData* pData, CXTPCalendarEvent* pEvent)
{
	if (pEvent->GetRecurrenceState() == xtpCalendarRecurrenceNotRecurring)
	{
		DWORD dwEventID = pEvent->GetEventID();
		CXTPCalendarEventPtr ptrEv = pData->GetEvent(dwEventID);
		return ptrEv != NULL;
	}
	else
	{
		DWORD dwPatternID = pEvent->GetRecurrencePatternID();
		CXTPCalendarRecurrencePatternPtr ptrPat = pData->GetRecurrencePattern(dwPatternID);
		return ptrPat != NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarControl::CXTPCalendarControl()
{
	RegisterWindowClass();

	m_pConnect = new CXTPNotifyConnection();

	m_bChanged = TRUE;
	m_nLockUpdateCount = m_nLockAdjustCount = 0;
	m_bAdjustScrollBar = FALSE;

	m_pActiveView = NULL;

	m_lcidActiveLocale = CXTPCalendarUtils::GetActiveLCID();

	m_pPaintManager = new CXTPCalendarPaintManager();
	if (m_pPaintManager)
	{
		m_pPaintManager->SetControl(this);
	}
	m_pTheme = NULL;

	m_mouseMode = xtpCalendarMouseNothing;
	m_nRowsPerWheel = 1;

	m_uNextTimerID = XTP_CALENDAR_FIRST_DYN_TIMERID;
	//---------------------------------------------------------------------------
	m_strUndoUIText = _T("&Undo\tAlt+BackSpace, Ctrl+Z");
	m_strRedoUIText = _T("&Redo\tAlt+BackSpace, Ctrl+Z");
	//---------------------------------------------------------------------------

	m_pMonthView = 0;
	m_pWeekView = 0;
	m_pDayView = 0;

	m_bDeleteOnFinalRelease = FALSE;
	m_bDisableRedraw = FALSE;

	m_bEnableToolTips = TRUE;
	m_bEnableSendNotifications = TRUE;

	m_uPopulateRequest_TimerID = 0;
	m_uRedrawRequest_TimerID = 0;

	m_bUpdateWhenEventChangedNotify = TRUE;

	m_pRemindersManager = NULL;
	m_cnidOnReminders = 0;
	m_spRemindersUpdatePeriod = cspRemindersUpdatePeriod;

	m_pOptions = new CXTPCalendarOptions();
	m_pResourcesNf = new CXTPCalendarResourcesNf();

	//By default add one resource with memory DataProvider
	SetDataProvider(xtpCalendarDataProviderMemory);
	ASSERT(GetDataProvider()->IsOpen());

	//EnableReminders(TRUE);

	//-----------------------------------------------------------------------
	if (XTPGetTopLevelWndMsgNotifier())
	{
		XTPGetTopLevelWndMsgNotifier()->Advise(this, WM_TIMECHANGE);
	}

	// test
//  CXTPCalendarThemeOffice2007* pTheme2007 = new CXTPCalendarThemeOffice2007();
//  SetTheme(pTheme2007);

	//*** TEST of customization ***

	// EXAMPLE: customize root settings

	//  pTheme2007->GetEventPartX()->m_fcsetSelected.clrBorder = RGB(0, 230, 0);
	//  pTheme2007->GetEventPartX()->m_fcsetSelected.fcsetSubject.clrColor = RGB(255, 0, 0);
	//  pTheme2007->GetEventPartX()->m_fcsetSelected.fcsetLocation.clrColor = RGB(0, 255, 0);

	//  // customize particular settings
	//  pTheme2007->GetDayViewPartX()->GetDayPartX()->GetGroupPartX()->GetSingleDayEventPartX()->m_fcsetSelected.clrBorder = RGB(255, 100, 230);
	/*

	LOGFONT lf;
	pTheme2007->GetDayViewPartX()->GetHeaderPartX()->m_TextLeft.fcsetTodaySelected.fntFont->GetLogFont(&lf);
	lf.lfHeight *= 2;
	//pTheme2007->GetDayViewPartX()->GetHeaderPartX()->m_TextLeft.fcsetTodaySelected.fntFont.SetCustomValue(&lf);

	lf.lfHeight = lf.lfHeight * 3 / 2;
	lf.lfItalic = 1;
	//pTheme2007->GetDayViewPartX()->GetDayPartX()->GetGroupPartX()->GetSingleDayEventPartX()->m_fcsetNormal.fcsetSubject.fntFont.SetCustomValue(&lf);

	//pTheme2007->GetDayViewPartX()->GetTimeScalePart()->m_fcsetAMPM.clrColor = RGB(200, 33, 12);

	pTheme2007->RefreshMetrics();
	*/
	// end EXAMPLE.
}

CXTPCalendarControl::~CXTPCalendarControl()
{
	//---------------------------------------------------------------------------
	if (XTPGetTopLevelWndMsgNotifier())
	{
		XTPGetTopLevelWndMsgNotifier()->Unadvise(this);
	}

	m_cnidOnReminders = 0;
	//m_Sink_DP.UnadviseAll();

	CMDTARGET_RELEASE(m_pResourcesNf);

	CMDTARGET_RELEASE(m_pActiveView);

	CMDTARGET_RELEASE(m_pDayView);
	CMDTARGET_RELEASE(m_pWeekView);
	CMDTARGET_RELEASE(m_pMonthView);

	CMDTARGET_RELEASE(m_pPaintManager);
	CMDTARGET_RELEASE(m_pTheme);

	CMDTARGET_RELEASE(m_pConnect);
	CMDTARGET_RELEASE(m_pRemindersManager);

	CMDTARGET_RELEASE(m_pOptions);

	if (::IsWindow(m_wndTip.GetSafeHwnd()))
	{
		m_wndTip.DestroyWindow();
	}

	if (::IsWindow(GetSafeHwnd()))
	{
		DestroyWindow();
	}
}

CScrollBar* CXTPCalendarControl::GetScrollBarCtrl(int nBar) const
{
	if (DYNAMIC_DOWNCAST(CView, GetParent()))
		return GetParent()->GetScrollBarCtrl(nBar);

	return 0;
}


void CXTPCalendarControl::OnDestroy()
{
	OnBeforeDestroy();

	CWnd::OnDestroy();
}

void CXTPCalendarControl::OnBeforeDestroy()
{
	if (m_pActiveView)
	{
		m_pActiveView->OnBeforeDestroy();
	}

	if (m_pDayView)
	{
		m_pDayView->OnBeforeDestroy();
	}
	if (m_pWeekView)
	{
		m_pWeekView->OnBeforeDestroy();
	}
	if (m_pMonthView)
	{
		m_pMonthView->OnBeforeDestroy();
	}
}

BOOL CXTPCalendarControl::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	return XTPDrawHelpers()->RegisterWndClass(hInstance,
			XTPCALENDARCTRL_CLASSNAME, CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW);
}

BOOL CXTPCalendarControl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CWnd::Create(XTPCALENDARCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	//-----------------------------------------------------------------------
	SetActiveView(GetDayView());

	CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetDayView());
	if (!pDayView)
	{
		return FALSE;
	}

	COleDateTime dtStartDate = CXTPCalendarUtils::GetCurrentTime();
	pDayView->ResetSelection();
	pDayView->ShowDay(dtStartDate);

	pDayView->ScrollToWorkDayBegin();

	return TRUE;
}

void CXTPCalendarControl::LockUpdate()
{
	m_nLockUpdateCount++;
}

void CXTPCalendarControl::UnlockUpdate()
{
	m_nLockUpdateCount--;

	if (m_nLockUpdateCount == 0 && m_bChanged)
	{
		RedrawControl();
	}
}

void CXTPCalendarControl::BeginUpdate()
{
	m_nLockUpdateCount++;
}

void CXTPCalendarControl::EndUpdate()
{
	m_bChanged = TRUE;
	m_nLockUpdateCount--;

	if (m_nLockUpdateCount == 0)
	{
		RedrawControl();
	}
}

void CXTPCalendarControl::BeginAdjust(BOOL bWithScrollBar)
{
	if (m_nLockAdjustCount == 0)
	{
		m_bAdjustScrollBar = FALSE;
	}
	m_nLockAdjustCount++;
	m_bAdjustScrollBar |= bWithScrollBar;
}

void CXTPCalendarControl::EndAdjust(BOOL bWithScrollBar)
{
	m_bChanged = TRUE;
	m_nLockAdjustCount--;
	m_bAdjustScrollBar |= bWithScrollBar;

	if (m_nLockAdjustCount == 0)
	{
		AdjustLayout();
		if (m_bAdjustScrollBar)
		{
			AdjustScrollBar();
		}

		m_bAdjustScrollBar = FALSE;
	}
}

BEGIN_MESSAGE_MAP(CXTPCalendarControl, CWnd)
//{{AFX_MSG_MAP(CXTPCalendarControl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_GETDLGCODE()
	ON_WM_ENABLE()

	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()

	ON_WM_TIMECHANGE()
	ON_WM_SYSCOLORCHANGE()

	ON_REGISTERED_MESSAGE(xtp_wm_SwitchView, OnSwitchView)
	ON_REGISTERED_MESSAGE(xtp_wm_UserAction, OnUserAction)

	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)

	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)

//  ON_CONTROL_REFLECT(ID_EDIT_UNDO, OnUndo)
//  ON_UPDATE_COMMAND_UI_REFLECT(OnUpdateUndo)
//}}AFX_MSG_MAP

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarControl message handlers

void CXTPCalendarControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	if (rc.IsRectEmpty())
		return;

	// Check cached bitmap
	if ((!m_bChanged || m_bDisableRedraw) && m_bmpCache.GetSafeHandle() != 0)
	{
		CXTPCompatibleDC memDC(&dc, &m_bmpCache);
		dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);
	}
	else
	{
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		if (!m_bmpCache.m_hObject)
		{
			m_bmpCache.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
		}

		CBitmap* pOldBitmap = memDC.SelectObject(&m_bmpCache);
		memDC.FillSolidRect(rc, 0xFF);

		OnDraw(&memDC);

		if (!IsWindowEnabled())
		{
			XTPImageManager()->DisableBitmap(memDC, rc, XTP_CALENDAR_DISABLED_COLOR_LIGHT, XTP_CALENDAR_DISABLED_COLOR_DARK);
		}

		dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBitmap);

		// update flag
		m_bChanged = FALSE;
	}
}



BOOL CXTPCalendarControl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPCalendarControl::OnSize(UINT nType, int cx, int cy)
{
	if (m_bmpCache.m_hObject)
	{
		m_bmpCache.DeleteObject();
	}

	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(GetSafeHwnd()) && cx > 0 && cy > 0)
	{
		AdjustLayout();
		AdjustScrollBar();
	}

}

void CXTPCalendarControl::AdjustScrollBar(int nSBType)
{
	if (!::IsWindow(GetSafeHwnd()) || !m_pActiveView)
		return;

	BeginUpdate();

	if (nSBType == SB_VERT || nSBType == -1)
	{
		AdjustScrollBarEx(SB_VERT);
	}

	if (nSBType == SB_HORZ || nSBType == -1)
	{
		AdjustScrollBarEx(SB_HORZ);
	}

	AdjustLayout();

	EndUpdate();
}

void CXTPCalendarControl::AdjustScrollBarEx(int nSBType)
{
	ASSERT(nSBType == SB_VERT || nSBType == SB_HORZ);

	if (!m_pActiveView)
	{
		return;
	}

	SCROLLINFO  si ;
	si.cbSize = sizeof(SCROLLINFO);
	si.nMin = 0 ;
	si.nMax = 1;
	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nPage = 1;
	si.nPos = 0;

	BOOL bEnabled = FALSE;
	if (nSBType == SB_VERT)
	{
		bEnabled = m_pActiveView->GetScrollBarInfoV(&si);
	}
	else if (nSBType == SB_HORZ)
	{
		bEnabled = m_pActiveView->GetScrollBarInfoH(&si);
	}
	else
	{
		ASSERT(FALSE);
		return;
	}

	if (si.nPos > si.nMax + 1 - (int)si.nPage)
	{
		int nPos_raw = si.nPos;

		si.nPos = si.nMax + 1 - (int)si.nPage;
		if (nSBType == SB_VERT)
		{
			m_pActiveView->ScrollV(si.nPos, nPos_raw);
		}
		else if (nSBType == SB_HORZ)
		{
			m_pActiveView->ScrollH(si.nPos, nPos_raw);
		}
		else
		{
			ASSERT(FALSE);
			return;
		}
	}

	if (bEnabled)
	{
		SetScrollInfo(nSBType, &si) ;
	}
	EnableScrollBarCtrl(nSBType, bEnabled);

	::EnableScrollBar(m_hWnd, nSBType, (bEnabled && IsWindowEnabled()) ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
}


void CXTPCalendarControl::AdjustLayout()
{
	if (!::IsWindow(GetSafeHwnd()))
		return;

	if (m_lcidActiveLocale != CXTPCalendarUtils::GetActiveLCID())
	{
		m_lcidActiveLocale = CXTPCalendarUtils::GetActiveLCID();

		XTP_SAFE_CALL1(m_pPaintManager, RefreshMetrics());
		XTP_SAFE_CALL1(m_pTheme, RefreshMetrics());
	}

	CClientDC dc(this);
	CXTPClientRect rcView(this);

	// horizontal scrolling support.
	SCROLLINFO si;
	if (m_pActiveView && m_pActiveView->GetScrollBarInfoH(&si))
	{
		ASSERT(m_pActiveView == m_pDayView);
		if (m_pActiveView == m_pDayView)
		{
			rcView.left -= si.nPos; //m_pDayView->m_nScrollOffsetX;
			rcView.right = rcView.left + si.nMax + 1 + m_pDayView->_GetTimeScaleWith();
		}
	}

	AdjustLayout(&dc, rcView);
}

void CXTPCalendarControl::AdjustLayout(CDC* pDC, const CRect& rcView)
{
	if (m_pActiveView && !rcView.IsRectEmpty())
	{
		if (GetTheme())
		{
			m_pActiveView->AdjustLayout2(pDC, rcView, TRUE);
		}
		else
		{
			m_pActiveView->AdjustLayout(pDC, rcView, TRUE);
		}
	}
}

void CXTPCalendarControl::RedrawControl(BOOL bForce)
{
	m_bChanged = TRUE;

	if (GetSafeHwnd() &&
		(bForce || (m_nLockUpdateCount == 0)))
	{
		Invalidate(FALSE);
		if (bForce)
			UpdateWindow();
	}
}

void CXTPCalendarControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);

	if (IsToolTipVisible())
	{
		HideToolTip();
	}

	SetCapture();
	SetFocus();
	if (m_pActiveView)
	{
		m_pActiveView->OnLButtonDown(nFlags, point);
	}

	CWnd::OnLButtonDown(nFlags, point);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_CALENDARLBUTTONDOWN, dwPoint, 0);
}

void CXTPCalendarControl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);

	CWnd::OnRButtonDown(nFlags, point);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_CALENDARRBUTTONDOWN, dwPoint, 0);
}

void CXTPCalendarControl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_CALENDARRBUTTONUP, dwPoint, 0);

	CWnd::OnRButtonUp(nFlags, point); // OnContextMenu will be called.
}

void CXTPCalendarControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);

	if (GetCapture() == this)
		ReleaseCapture();

	if (m_pActiveView)
	{
		m_pActiveView->OnLButtonUp(nFlags, point);
	}

	CWnd::OnLButtonUp(nFlags, point);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_CALENDARLBUTTONUP, dwPoint, 0);
}

void CXTPCalendarControl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);

	if (m_pActiveView)
	{
		m_pActiveView->OnLButtonDblClk(nFlags, point);
	}

	CWnd::OnLButtonDblClk(nFlags, point);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_CALENDARLBUTTONDBLCLICK, dwPoint, 0);
}

void CXTPCalendarControl::OnMouseMove(UINT nFlags, CPoint point)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);

	m_mouseMode = xtpCalendarMouseNothing;

	if (m_pActiveView)
	{
		m_pActiveView->OnMouseMove(nFlags, point);
	}

	CWnd::OnMouseMove(nFlags, point);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_CALENDARMOUSEMOVE, dwPoint, 0);
}

void CXTPCalendarControl::OnCaptureChanged(CWnd* pWnd)
{
	CWnd::OnCaptureChanged(pWnd);
}

void CXTPCalendarControl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);
	CViewChangedContext viewChanged(this, xtpCalendarViewChangedLock);

	if (pScrollBar != NULL)
	{
		CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	if (!m_pActiveView)
	{
		return;
	}

	SCROLLINFO si;
	m_pActiveView->GetScrollBarInfoV(&si);

	int nCurPos = si.nPos;
	int nCurPos_raw = si.nPos;
	int nLimit = GetScrollLimit(SB_VERT);

	// decide what to do for each different scroll event
	switch (nSBCode)
	{
	case SB_TOP:
		nCurPos = nCurPos_raw = 0;
		break;
	case SB_BOTTOM:
		nCurPos = nCurPos_raw = nLimit;
		break;
	case SB_LINEUP:
		nCurPos = max(nCurPos - 1, 0);
		nCurPos_raw--;
		break;
	case SB_LINEDOWN:
		nCurPos = min(nCurPos + 1, nLimit);
		nCurPos_raw++;
		break;
	case SB_PAGEUP:
		nCurPos = max(nCurPos - (int)si.nPage, 0);
		nCurPos_raw -= (int)si.nPage;
		break;
	case SB_PAGEDOWN:
		nCurPos = min(nCurPos + (int)si.nPage, nLimit);
		nCurPos_raw += (int)si.nPage;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nCurPos = nCurPos_raw = nPos;
		break;
	}

	m_pActiveView->ScrollV(nCurPos, nCurPos_raw);

	SetScrollPos(SB_VERT, nCurPos, FALSE);
	AdjustScrollBar();


	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXTPCalendarControl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);
	CViewChangedContext viewChanged(this, xtpCalendarViewChangedLock);

	if (pScrollBar != NULL)
	{
		CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	if (!m_pActiveView)
	{
		return;
	}

	SCROLLINFO si;
	int nScrollStep = 1;
	m_pActiveView->GetScrollBarInfoH(&si, &nScrollStep);

	int nCurPos = si.nPos;
	int nCurPos_raw = si.nPos;
	int nLimit = GetScrollLimit(SB_HORZ);

	// decide what to do for each different scroll event
	switch (nSBCode)
	{
	case SB_LEFT:
		nCurPos = nCurPos_raw = 0;
		break;
	case SB_RIGHT:
		nCurPos = nCurPos_raw = nLimit;
		break;
	case SB_LINELEFT:
		nCurPos = max(nCurPos - nScrollStep, 0);
		nCurPos_raw--;
		break;
	case SB_LINERIGHT:
		nCurPos = min(nCurPos + nScrollStep, nLimit);
		nCurPos_raw++;
		break;
	case SB_PAGELEFT:
		nCurPos = max(nCurPos - (int)si.nPage, 0);
		nCurPos_raw -= (int)si.nPage;
		break;
	case SB_PAGERIGHT:
		nCurPos = min(nCurPos + (int)si.nPage, nLimit);
		nCurPos_raw += (int)si.nPage;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nCurPos = nCurPos_raw = nPos;
		break;
	}

	m_pActiveView->ScrollH(nCurPos, nCurPos_raw);

	SetScrollPos(SB_HORZ, nCurPos, FALSE);
	AdjustScrollBar();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CXTPCalendarControl::OnDraw(CDC* pDC)
{
	ASSERT(m_pActiveView && pDC);
	if (m_pActiveView && pDC)
		m_pActiveView->OnDraw(pDC);
}

void CXTPCalendarControl::Populate()
{
	if (IsToolTipVisible())
	{
		HideToolTip();
	}

	if (!m_pActiveView)
		return;

	if (::IsWindow(GetSafeHwnd()))
	{
		CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);
		CViewChangedContext viewChanged(this, xtpCalendarViewChangedLock);

		CUpdateContext updateContext2(this, xtpCalendarUpdateAll);

		m_pActiveView->Populate();
	}
	else
	{
		m_pActiveView->Populate();
	}
}

BOOL CXTPCalendarControl::UpdateMouseCursor()
{
	if (!AfxGetApp())
	{
		return FALSE;
	}

	switch (m_mouseMode)
	{
	case xtpCalendarMouseEventPreResizeV:
	case xtpCalendarMouseEventResizingV:
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		TRACE_DRAGGING(_T("UpdateMouseCursor():: ResizingV\n"));
		break;
	case xtpCalendarMouseEventPreResizeH:
	case xtpCalendarMouseEventResizingH:
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		TRACE_DRAGGING(_T("UpdateMouseCursor():: ResizingH\n"));
		break;
	case xtpCalendarMouseEventPreDrag:
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		TRACE_DRAGGING(_T("UpdateMouseCursor():: PreDrag\n"));
		break;
	case xtpCalendarMouseEventDragCopy:
		SetCursor(XTPResourceManager()->LoadCursor(XTP_IDC_CALENDAR_DRAGCOPY));
		TRACE_DRAGGING(_T("UpdateMouseCursor():: DragCopy\n"));
		break;
	case xtpCalendarMouseEventDragMove:
		SetCursor(XTPResourceManager()->LoadCursor(XTP_IDC_CALENDAR_DRAGMOVE));
		TRACE_DRAGGING(_T("UpdateMouseCursor():: DragCopy\n"));
		break;
	case xtpCalendarMouseEventDraggingOut:
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		TRACE_DRAGGING(_T("UpdateMouseCursor():: Dragging OUT\n"));

		break;
	case xtpCalendarMouseNothing:
	default:
		TRACE_DRAGGING(_T("UpdateMouseCursor():: Nothing\n"));
		return FALSE;
	}
	return TRUE;
}

BOOL CXTPCalendarControl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		if (UpdateMouseCursor())
			return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CXTPCalendarControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	UINT uiMsg;
	int nScrollsCount = 0;
	// calculate what should be sent
	if (m_nRowsPerWheel == -1)
	{
		// A m_nRowsPerWheel value less than 0 indicates that the mouse wheel scrolls whole pages, not just lines.
		int nPagesScrolled = zDelta / 120;
		uiMsg = nPagesScrolled > 0 ? SB_PAGEUP : SB_PAGEDOWN;
		nScrollsCount = nPagesScrolled > 0 ? nPagesScrolled : -nPagesScrolled;
	}
	else
	{
		int nRowsScrolled = m_nRowsPerWheel * zDelta / 120;
		uiMsg = nRowsScrolled > 0 ? SB_LINEUP : SB_LINEDOWN;
		nScrollsCount = nRowsScrolled > 0 ? nRowsScrolled : -nRowsScrolled;
	}
	// send scroll messages
	for (int i = 0; i < nScrollsCount; i++)
	{
		PostMessage(WM_VSCROLL, uiMsg, 0);
	}

	// prevent parent from scrolling while calendar scroll pos is not min or max.
	//
	if (m_pActiveView)
	{
		SCROLLINFO  si;
		::ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(SCROLLINFO);

		BOOL bScrollEnabled = m_pActiveView->GetScrollBarInfoV(&si);

		if (bScrollEnabled)
		{
			int nViewType = m_pActiveView->GetViewType();

			if (nViewType == xtpCalendarDayView ||
				nViewType == xtpCalendarWorkWeekView)
			{
				INT nPosMin, nPosMax;
				GetScrollRange(SB_VERT, &nPosMin, &nPosMax);
				nPosMax = GetScrollLimit(SB_VERT);

				int nPos = GetScrollPos(SB_VERT);

				if (zDelta > 0 && nPos > nPosMin || zDelta < 0 && nPos < nPosMax)
				{
					return TRUE;
				}
			}
			else
			{
				return TRUE;
			}
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

//////////////////////////////////////////////////////////////////////////
// View related
//
CXTPCalendarView* CXTPCalendarControl::GetActiveView()
{
	return m_pActiveView;
}

CXTPCalendarView* CXTPCalendarControl::GetDayView()
{
	if (m_pDayView == 0)
	{
		m_pDayView = new CXTPCalendarDayView(this);
	}

	return m_pDayView;
}

CXTPCalendarView* CXTPCalendarControl::GetWeekView()
{
	if (m_pWeekView == 0)
	{
		m_pWeekView = new CXTPCalendarWeekView(this);
	}

	return m_pWeekView;

}

CXTPCalendarView* CXTPCalendarControl::GetMonthView()
{
	if (m_pMonthView == 0)
	{
		m_pMonthView = new CXTPCalendarMonthView(this);
	}

	return m_pMonthView;
}


void CXTPCalendarControl::SetActiveView(CXTPCalendarView* pView)
{
	CViewChangedContext viewChanged(this);

	CMDTARGET_ADDREF(pView);

	CXTPCalendarView* pInactiveView = m_pActiveView;
	if (pInactiveView)
	{
		pInactiveView->OnActivateView(FALSE, pView, pInactiveView);
	}

	m_pActiveView = pView;

	if (m_pActiveView)
	{
		m_pActiveView->OnActivateView(TRUE, m_pActiveView, pInactiveView);
	}

	CMDTARGET_RELEASE(pInactiveView);

}

void CXTPCalendarControl::SwitchActiveView(XTPCalendarViewType eViewType)
{
	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);
	CViewChangedContext viewChanged(this, xtpCalendarViewChangedLock);

	CUpdateContext updateContext2(this, xtpCalendarUpdateRedraw | xtpCalendarUpdateLayout);

	CXTPCalendarView* pPrevView = GetActiveView();
	CXTPCalendarViewEventsPtr ptrSelEvents;

	COleDateTime dtSelStartDate, dtSelDay, dtSelDay2, dtSelEvent;
	COleDateTimeSpan spSelDayDuration(0.);
	BOOL bSelDay = FALSE;
	BOOL bSelAllDay = FALSE;

	if (pPrevView)
	{
		bSelDay = pPrevView->GetSelection(&dtSelDay, &dtSelDay2, &bSelAllDay);
		if (bSelDay)
		{
			spSelDayDuration = CXTPCalendarUtils::ResetTime(dtSelDay2) - CXTPCalendarUtils::ResetTime(dtSelDay);
			if (bSelAllDay && spSelDayDuration.GetTotalDays() >= 1)
			{
				spSelDayDuration -= COleDateTimeSpan(1, 0, 0, 0);
			}
		}

		ptrSelEvents = pPrevView->GetSelectedEvents();

		dtSelEvent.SetDate(9999, 1, 1);
		int nCount = ptrSelEvents ? ptrSelEvents->GetCount() : 0;
		for (int i = 0; i < nCount; i++)
		{
			COleDateTime dtEvent = XTP_SAFE_GET3(ptrSelEvents, GetAt(i), GetEvent(), GetStartTime(), (DATE)0);
			dtSelEvent = min(dtSelEvent, dtEvent);
		}

		dtSelStartDate = nCount ? dtSelEvent : (bSelDay ? dtSelDay : COleDateTime::GetCurrentTime());
	}

	COleDateTime dtWDStartTime = XTP_SAFE_GET1(GetCalendarOptions(), dtWorkDayStartTime, (DATE)0);

	if (eViewType == xtpCalendarDayView)
	{
		SetActiveView(GetDayView());

		CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetDayView());
		if (!pDayView)
		{
			return;
		}

		pDayView->ResetSelection();
		pDayView->ShowDay(dtSelStartDate);

		COleDateTime dtSelBegin = CXTPCalendarUtils::UpdateTime(dtSelStartDate, dtWDStartTime);
		COleDateTime dtSelEnd = dtSelBegin + pDayView->GetCellDuration();

		pDayView->SetSelection(dtSelBegin, dtSelEnd, FALSE);
	}
	else if (eViewType == xtpCalendarWorkWeekView)
	{
		SetActiveView(GetDayView());

		CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetDayView());
		if (!pDayView)
		{
			return;
		}

		pDayView->ShowWorkingDays(dtSelStartDate);
		pDayView->ResetSelection();

		COleDateTime dtSelBegin = CXTPCalendarUtils::UpdateTime(pDayView->GetViewDayDate(0), dtWDStartTime);
		COleDateTime dtSelEnd = dtSelBegin + pDayView->GetCellDuration();

		pDayView->SetSelection(dtSelBegin, dtSelEnd, FALSE);
	}
	else if (eViewType == xtpCalendarWeekView)
	{
		SetActiveView(GetWeekView());

		CXTPCalendarWeekView* pWeekView = DYNAMIC_DOWNCAST(CXTPCalendarWeekView, GetWeekView());
		if (!pWeekView)
		{
			return;
		}

		pWeekView->ResetSelection();
		pWeekView->ShowDay(dtSelStartDate);

		COleDateTime dtSel = CXTPCalendarUtils::ResetTime(dtSelStartDate);
		COleDateTime dtSel2 = dtSel + spSelDayDuration;

		pWeekView->SetSelection(dtSel, dtSel2, TRUE);
	}
	else if (eViewType == xtpCalendarMonthView)
	{
		SetActiveView(GetMonthView());

		CXTPCalendarMonthView* pMonthView = DYNAMIC_DOWNCAST(CXTPCalendarMonthView, GetMonthView());
		if (!pMonthView || !pMonthView->GetGrid())
		{
			return;
		}
		COleDateTime dtFirstOfMonth(dtSelStartDate.GetYear(), dtSelStartDate.GetMonth(), 1, 0, 0, 0);

		pMonthView->ResetSelection();
		pMonthView->ShowDay(dtFirstOfMonth);

		int nWCount = pMonthView->GetGrid()->GetWeeksCount();
		if (nWCount < 5)
		{
			pMonthView->GetGrid()->SetWeeksCount(5);
		}

		COleDateTime dtMaxDate = pMonthView->GetViewDayDate(pMonthView->GetViewDayCount()-1);

		COleDateTime dtSel = CXTPCalendarUtils::ResetTime(dtSelStartDate);
		if (dtSel > dtMaxDate)
		{
			dtSel = pMonthView->GetViewDayDate(0);
		}
		COleDateTime dtSel2 = dtSel + spSelDayDuration;

		pMonthView->SetSelection(dtSel, dtSel2, TRUE);
	}
	else
	{
		ASSERT(FALSE);
		return;
	}

	CXTPCalendarView* pActiveView = GetActiveView();
	if (pActiveView)
	{
		pActiveView->UnselectAllEvents();
	}

	//------------------------------------------------------------------------
	Populate();

	if (::IsWindow(GetSafeHwnd()) && IsWindowVisible())
	{
		AdjustScrollBar();
	}
	//------------------------------------------------------------------------
	if (pActiveView && ptrSelEvents)
	{
		int nCount = ptrSelEvents->GetCount();
		for (int i = 0; i < nCount; i++)
		{
			if (ptrSelEvents->GetAt(i))
			{
				pActiveView->SelectEvent(ptrSelEvents->GetAt(i)->GetEvent());
			}
		}
	}
}

UINT CXTPCalendarControl::SetTimer(UINT uTimeOut_ms)
{
	UINT uTimerID = m_uNextTimerID;

	if (!::IsWindow(GetSafeHwnd()))
		return 0;

	UINT nRes = (UINT)CWnd::SetTimer(uTimerID, uTimeOut_ms, NULL);

	if (!nRes)
	{
		ASSERT(FALSE);
		return 0;
	}
	m_uNextTimerID++;

	if (m_uNextTimerID < XTP_CALENDAR_FIRST_DYN_TIMERID)
	{
		m_uNextTimerID = XTP_CALENDAR_FIRST_DYN_TIMERID;
	}

	return uTimerID;
}

void CXTPCalendarControl::OnTimer(UINT_PTR uTimerID)
{
	ASSERT(m_pActiveView);

	if (uTimerID == m_uPopulateRequest_TimerID)
	{
		KillTimer(m_uPopulateRequest_TimerID);
		m_uPopulateRequest_TimerID = 0;

		Populate();

		return;
	}
	else if (uTimerID == m_uRedrawRequest_TimerID)
	{
		KillTimer(m_uRedrawRequest_TimerID);
		m_uRedrawRequest_TimerID = 0;

		CUpdateContext updateContext(this, xtpCalendarUpdateRedraw);

		return;
	}
	else if (uTimerID == XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID)
	{
		KillTimer(XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID);

		CUpdateContext updateContext(this, xtpCalendarUpdateAll);

		if (GetTheme())
		{
			GetTheme()->RefreshMetrics();
		}
		else if (m_pPaintManager)
		{
			m_pPaintManager->RefreshMetrics();
		}
		return;
	}


	if (m_pActiveView)
	{
		m_pActiveView->OnTimer(uTimerID);
		DBG_TRACE_TIMER(_T("on Timer: ID=%d  \n"), uTimerID);
	}
}

void CXTPCalendarControl::OnOptionsChanged(int nOptionRelation)
{
	SendNotification(XTP_NC_CALENDAROPTIONSWASCHANGED, nOptionRelation, 0);

	KillTimer(XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID);
	CWnd::SetTimer(XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID, XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMEOUT, NULL);
}

///////////////////////////////////////////////////////////////////////////
// Data part
//

CString CXTPCalendarControl::DataSourceFromConStr(LPCTSTR lpszConnectionString)
{
	ASSERT(lpszConnectionString);

	if (!lpszConnectionString || 0 == _tcslen(lpszConnectionString))
	{
		return _T("");
	}

	CString strConnStr_lower = lpszConnectionString;
	strConnStr_lower.MakeLower();

	int nIndex = strConnStr_lower.Find(_T("data source="));
	if (nIndex != -1)
	{
		CString strDataSource = strConnStr_lower.Mid(nIndex + 12);
		if (strDataSource.IsEmpty())
			return _T("");

		if ((strDataSource[0] == _T('\'') || strDataSource[0] == _T('"')) && strDataSource.GetLength() > 2)
		{
			TCHAR chQuot = strDataSource[0];

			DELETE_S(strDataSource, 0);

			int nQuot2Idx = FIND_S(strDataSource, chQuot, 0);

			if (nQuot2Idx >= 0)
			{
				DELETE_S(strDataSource, nQuot2Idx, strDataSource.GetLength() - nQuot2Idx);
			}
		}

		return strDataSource;
	}

	return strConnStr_lower;
}

XTPCalendarDataProvider CXTPCalendarControl::DataProviderTypeFromConStr(
								LPCTSTR lpszConnectionString,
								XTPCalendarDataProvider eDPDefault)
{
	ASSERT(lpszConnectionString);

	if (!lpszConnectionString)
	{
		return eDPDefault;
	}

	CString strConnStr_lower(lpszConnectionString);
	strConnStr_lower.MakeLower();

	BOOL bProviderCustom = strConnStr_lower.Find(_T("provider=custom")) != -1;
	if (bProviderCustom)
	{
		return xtpCalendarDataProviderCustom;
	}

	BOOL bProviderMAPI   = strConnStr_lower.Find(_T("provider=mapi"))   != -1;
	if (bProviderMAPI)
	{
		return xtpCalendarDataProviderMAPI;
	}

	BOOL bProviderXML   = strConnStr_lower.Find(_T("provider=xml")) != -1;
	if (bProviderXML)
	{
		return xtpCalendarDataProviderMemory;
	}

	BOOL bProvider = strConnStr_lower.Find(_T("provider=")) != -1;
	BOOL bDSN      = strConnStr_lower.Find(_T("dsn="))      != -1;
	if (bProvider || bDSN)
	{
		return xtpCalendarDataProviderDatabase;
	}

	return eDPDefault;
}

CXTPCalendarData* CXTPCalendarControl::CreateDataProvider(LPCTSTR lpszConnectionString)
{
	ASSERT(lpszConnectionString);

	if (!lpszConnectionString)
	{
		return NULL;
	}

	XTPCalendarDataProvider eDPType = DataProviderTypeFromConStr(lpszConnectionString, xtpCalendarDataProviderMemory);

	CXTPCalendarData* pDataProvider = CreateDataProvider(eDPType);

	if (pDataProvider)
	{
		pDataProvider->SetConnectionString(lpszConnectionString);
	}
	return pDataProvider;
}

CXTPCalendarData* CXTPCalendarControl::CreateDataProvider(XTPCalendarDataProvider eDataProvider)
{
	CXTPCalendarData* pDataProvider = NULL;
	switch (eDataProvider)
	{
	case xtpCalendarDataProviderMemory:
		pDataProvider = new CXTPCalendarMemoryDataProvider();
		break;
	case xtpCalendarDataProviderDatabase:
		pDataProvider = new CXTPCalendarDatabaseDataProvider();
		break;
	case xtpCalendarDataProviderMAPI:
		pDataProvider = new CXTPCalendarMAPIDataProvider();
		break;
	case xtpCalendarDataProviderCustom:
		pDataProvider = new CXTPCalendarCustomDataProvider();
		break;
	default:
		ASSERT(FALSE);
	}

	return pDataProvider;
}

void CXTPCalendarControl::_SetDataProvider(CXTPCalendarData* pDataProvider,
										  BOOL bCloseDataProviderWhenDestroy)
{
	if (m_pRemindersManager)
	{
		VERIFY( m_pRemindersManager->StopMonitoring() );
	}

	CMDTARGET_ADDREF(pDataProvider);

	if (m_pOptions)
	{
		m_pOptions->SetDataProvider(pDataProvider);
	}

	// set current DP
	if (m_pResourcesNf)
	{
		m_pResourcesNf->RemoveAll();

		CXTPCalendarResource* pRC = new CXTPCalendarResource(this);
		if (pRC)
		{
			m_pResourcesNf->Add(pRC);
			pRC->SetDataProvider(pDataProvider, bCloseDataProviderWhenDestroy);
		}
	}
	CMDTARGET_RELEASE(pDataProvider);
}

void CXTPCalendarControl::SetDataProvider(CXTPCalendarData* pDataProvider,
										  BOOL bCloseDataProviderWhenDestroy)
{
	_SetDataProvider(pDataProvider, bCloseDataProviderWhenDestroy);

	AdviseToDataProvider();
}

void CXTPCalendarControl::SetDataProvider(LPCTSTR lpszConnectionString)
{
	ASSERT(lpszConnectionString);
	if (!lpszConnectionString)
	{
		return;
	}

	CXTPCalendarData* pDataProvider = CreateDataProvider(lpszConnectionString);
	if (pDataProvider)
	{
		_SetDataProvider(pDataProvider, TRUE);
		CMDTARGET_RELEASE(pDataProvider);
	}
	AdviseToDataProvider();
}

void CXTPCalendarControl::SetDataProvider(
	XTPCalendarDataProvider eDataProvider, LPCTSTR lpszConnectionString)
{
	CXTPCalendarData* pDataProvider = CreateDataProvider(eDataProvider);
	if (pDataProvider)
	{
		if (lpszConnectionString)
		{
			pDataProvider->SetConnectionString(lpszConnectionString);
		}
		if (eDataProvider == xtpCalendarDataProviderMemory && !lpszConnectionString)
		{
			VERIFY(pDataProvider->Open());
		}

		_SetDataProvider(pDataProvider, TRUE);
		CMDTARGET_RELEASE(pDataProvider);
	}
	AdviseToDataProvider();
}

CXTPCalendarData* CXTPCalendarControl::GetDataProvider()
{
	ASSERT(this);

	if (this && m_pResourcesNf && m_pResourcesNf->GetCount() && m_pResourcesNf->GetAt(0))
	{
		return m_pResourcesNf->GetAt(0)->GetDataProvider();
	}
	return NULL;
}

CXTPCalendarResources* CXTPCalendarControl::GetResources()
{
	ASSERT(this);
	return this ? m_pResourcesNf : NULL;
}

void CXTPCalendarControl::SetResources(CXTPCalendarResources* pResources,
									   CXTPCalendarData* pOptionsDataProvider)
{
	if (!m_pResourcesNf || !pResources || pResources->GetCount() == 0)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pOptionsDataProvider)
	{
		pOptionsDataProvider = XTP_SAFE_GET2(pResources, GetAt(0), GetDataProvider(), NULL);
	}
	if (m_pOptions)
	{
		m_pOptions->SetDataProvider(pOptionsDataProvider);
	}

	// copy data to allow using self as input parameter
	CXTPCalendarResources arRes;
	arRes.Append(pResources);

	m_pResourcesNf->RemoveAll();
	m_pResourcesNf->Append(&arRes);

	// reset view's configurations
	XTP_SAFE_CALL1(m_pDayView,   SetResources(NULL));
	XTP_SAFE_CALL1(m_pMonthView, SetResources(NULL));
	XTP_SAFE_CALL1(m_pWeekView,  SetResources(NULL));

	AdviseToDataProvider();
}


void CXTPCalendarControl::AdviseToDataProvider()
{
	m_Sink_DP.UnadviseAll();
	m_cnidOnReminders = 0;

	if (m_pResourcesNf)
	{
		CXTPNotifyConnection* pDPConn = m_pResourcesNf->GetConnection();
		ASSERT(pDPConn);
		if (pDPConn)
		{
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAROPTIONSWASCHANGED, &CXTPCalendarControl::OnEvent_FromDataProvider);

			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAREVENTWASADDED, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAREVENTWASDELETED, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAREVENTWASCHANGED, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDARPATTERNWASADDED, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDARPATTERNWASDELETED, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDARPATTERNWASCHANGED, &CXTPCalendarControl::OnEvent_FromDataProvider);

			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoRetrieveDayEvents, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoRemoveAllEvents, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoRead_Event, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoRead_RPattern, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoCreate_Event, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoUpdate_Event, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoDelete_Event, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoCreate_RPattern, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoUpdate_RPattern, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoDelete_RPattern, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoGetAllEvents_raw, &CXTPCalendarControl::OnEvent_FromDataProvider);
			m_Sink_DP.Advise(pDPConn, XTP_NC_CALENDAR_DoGetUpcomingEvents, &CXTPCalendarControl::OnEvent_FromDataProvider);
		}
		m_pResourcesNf->ReBuildInternalData();
	}

	_AdviseToReminders_StartMonitoring();

	SendNotification(XTP_NC_CALENDAR_RESOURCES_WHERE_CHANGED, 0, 0);
}

//////////////////////////////////////////////////////////////////////////
// Reminders

BOOL CXTPCalendarControl::IsRemindersEnabled() const
{
	return m_pRemindersManager != NULL;
}

void CXTPCalendarControl::EnableReminders(BOOL bEnable)
{

	if (bEnable && m_pRemindersManager)
	{
		// Already enabled. skip.
		return;
	}

	if (!bEnable && !m_pRemindersManager)
	{
		// Already disabled. skip.
		return;
	}

	// 1. Enable.
	if (bEnable)
	{
		ASSERT(!m_pRemindersManager);
		CMDTARGET_RELEASE(m_pRemindersManager);

		m_pRemindersManager = new CXTPCalendarRemindersManager();

		_AdviseToReminders_StartMonitoring();
	}

	// 2. Disable
	if (!bEnable)
	{
		ASSERT(m_pRemindersManager);
		if (m_pRemindersManager)
		{
			VERIFY( m_pRemindersManager->StopMonitoring() );

			if (m_cnidOnReminders)
			{
				m_Sink_DP.Unadvise(m_cnidOnReminders);
				m_cnidOnReminders = 0;
			}
		}

		CMDTARGET_RELEASE(m_pRemindersManager);
	}
}

void CXTPCalendarControl::_AdviseToReminders_StartMonitoring()
{
	if (m_pRemindersManager && m_pResourcesNf)
	{
		CXTPNotifyConnection* pRMConn = m_pRemindersManager->GetConnection();
		ASSERT(pRMConn);
		if (pRMConn)
		{
			m_cnidOnReminders = m_Sink_DP.Advise(pRMConn, XTP_NC_CALENDAR_ON_REMINDERS, &CXTPCalendarControl::OnEvent_Reminders);
			ASSERT(m_cnidOnReminders);

			VERIFY(m_pRemindersManager->StartMonitoring(m_pResourcesNf,
					m_spRemindersUpdatePeriod) );
		}
	}
}

void CXTPCalendarControl::OnEnable(BOOL bEnable)
{
	UNREFERENCED_PARAMETER(bEnable);

	AdjustScrollBar();
}

UINT CXTPCalendarControl::OnGetDlgCode()
{
	return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTALLKEYS;
}

void CXTPCalendarControl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);
	CViewChangedContext viewChanged(this, xtpCalendarViewChangedLock);

	if (m_pActiveView)
	{
		m_pActiveView->OnChar(nChar, nRepCnt, nFlags);
	}
}

void CXTPCalendarControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedrawIfNeed);
	CViewChangedContext viewChanged(this, xtpCalendarViewChangedLock);

#ifdef XTP_CALENDAR_SITENOTIFY_KEY
	if (!XTP_CALENDAR_SITENOTIFY_KEY(this, TRUE, nChar))
		return;
#endif

	if (nChar == 0)
	{
		return;
	}

	SendNotification(XTP_NC_CALENDARKEYDOWN, (DWORD)nChar, 0);

	if (m_pActiveView)
	{
		m_pActiveView->OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CXTPCalendarControl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

#ifdef XTP_CALENDAR_SITENOTIFY_KEY
	if (!XTP_CALENDAR_SITENOTIFY_KEY(this, TRUE, nChar))
		return;
#endif
	if (nChar == 0)
	{
		return;
	}

	if (m_pActiveView)
	{
		if (m_pActiveView->OnSysKeyDown(nChar, nRepCnt, nFlags))
		{
			return;
		}
	}

	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CXTPCalendarControl::OnWndMsg_Children(UINT message, WPARAM wParam, LPARAM lParam,
											LRESULT* /*pResult*/)
{
	ASSERT(m_pActiveView);
	if (!m_pActiveView)
		return FALSE;

	if (message == WM_KEYDOWN)
	{
		UINT uRepCnt = LOWORD(lParam);
		UINT uFlags = HIWORD(lParam);

		if (wParam == VK_RETURN || wParam == VK_ESCAPE)
		{
			OnKeyDown((UINT)wParam, uRepCnt, uFlags);
			return TRUE;
		}

		BOOL bEditSubject = m_pActiveView->m_eDraggingMode == xtpCalendaDragModeEditSubject;
		if (bEditSubject && wParam == VK_TAB)
		{
			OnKeyDown((UINT)wParam, uRepCnt, uFlags);
			return TRUE;
		}
	}
	else if (message == WM_MOUSEMOVE)
	{
		if (m_pActiveView->m_eDraggingMode == xtpCalendaDragModeEditSubject)
		{
			m_mouseMode = xtpCalendarMouseNothing;
			m_pActiveView->OnMouseMove((UINT)wParam, CPoint(lParam));
		}
	}

	//---------------------------------------------------------------------------
	return FALSE;
}

void CXTPCalendarControl::ShowToolTip(const CString& strText, const CRect rcToolTip, CFont* pFont)
{
	if (!m_bEnableToolTips || GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rcTip(rcToolTip);
	rcTip.OffsetRect(7, 0);
	if (!m_wndTip.GetSafeHwnd())
	{
		m_wndTip.Create(this);
	}

	m_wndTip.SetFont(pFont);
	m_wndTip.SetTooltipText(strText);

	CSize rcNeeded = m_wndTip.CalcToolTipRect();

	rcTip.right = rcTip.left + rcNeeded.cx;
	rcTip.bottom = rcTip.top + rcNeeded.cy;

	ClientToScreen(rcTip);

	m_wndTip.SetHoverRect(rcTip);

	BOOL bShow = !strText.IsEmpty();
	m_wndTip.Activate(bShow);

	TRACKMOUSEEVENT tme =
	{
		sizeof(TRACKMOUSEEVENT), TME_LEAVE, GetSafeHwnd(), 0
	};
	_TrackMouseEvent (&tme);
}

void CXTPCalendarControl::HideToolTip()
{
	if (!GetSafeHwnd() || !m_wndTip.GetSafeHwnd())
	{
		return;
	}

	m_wndTip.Activate(FALSE);
}

BOOL CXTPCalendarControl::IsToolTipVisible() const
{
	return (m_wndTip.GetSafeHwnd() != NULL) && m_wndTip.IsWindowVisible();
}

void CXTPCalendarControl::SetPaintManager(CXTPCalendarPaintManager* pPaintManager)
{
	ASSERT(pPaintManager);

	CUpdateContext updateContext(this, xtpCalendarUpdateRedraw);
	CMDTARGET_RELEASE(m_pPaintManager);
	m_pPaintManager = pPaintManager;
}

void CXTPCalendarControl::OnSysColorChange()
{
	CUpdateContext updateContext(this, xtpCalendarUpdateRedraw);

	if (m_pPaintManager)
	{
		m_pPaintManager->RefreshMetrics();
	}

	if (GetTheme())
	{
		GetTheme()->RefreshMetrics();
	}

	AdjustLayout();

	CWnd::OnSysColorChange();
}

void CXTPCalendarControl::OnTimeChange()
{
	CUpdateContext updateContext(this, xtpCalendarUpdateAll);
	CWnd::OnTimeChange();
}

void CXTPCalendarControl::OnEditCommand(UINT uCommandID)
{
	if (uCommandID == ID_EDIT_UNDO)
	{
		OnUndo();
	}
	else if (uCommandID == ID_EDIT_CUT)
	{
		OnCut();
	}
	else if (uCommandID == ID_EDIT_COPY)
	{
		OnCopy();
	}
	else if (uCommandID == ID_EDIT_PASTE)
	{
		OnPaste();
	}
}


void CXTPCalendarControl::OnUndo()
{
	if (m_pActiveView)
	{
		if (m_pActiveView->CanUndo())
		{
			m_pActiveView->Undo();
		}
		else if (m_pActiveView->CanRedo())
		{
			m_pActiveView->Redo();
		}
	}
}

void CXTPCalendarControl::OnCut()
{
	if (m_pActiveView)
	{
		CWaitCursor _WC;
		m_pActiveView->Cut();
	}
}

void CXTPCalendarControl::OnCopy()
{
	if (m_pActiveView)
	{
		CWaitCursor _WC;
		m_pActiveView->Copy();
	}
}

void CXTPCalendarControl::OnPaste()
{
	if (m_pActiveView)
	{
		CWaitCursor _WC;
		m_pActiveView->Paste();
	}
}
void CXTPCalendarControl::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID == ID_EDIT_UNDO)
	{
		OnUpdateUndo(pCmdUI);
	}
	else if (pCmdUI->m_nID == ID_EDIT_CUT)
	{
		OnUpdateCut(pCmdUI);
	}
	else if (pCmdUI->m_nID == ID_EDIT_COPY)
	{
		OnUpdateCopy(pCmdUI);
	}
	else if (pCmdUI->m_nID == ID_EDIT_PASTE)
	{
		OnUpdatePaste(pCmdUI);
	}
}

void CXTPCalendarControl::OnUpdateUndo(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID != ID_EDIT_UNDO || !m_pActiveView)
	{
		ASSERT(FALSE);
		return;
	}

	if (m_pActiveView->CanUndo())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(m_strUndoUIText);
	}
	else if (m_pActiveView->CanRedo())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(m_strRedoUIText);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetText(m_strUndoUIText);
	}
}

void CXTPCalendarControl::OnUpdateCut(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID != ID_EDIT_CUT || !m_pActiveView)
	{
		ASSERT(FALSE);
		return;
	}
	pCmdUI->Enable(m_pActiveView->CanCut());
}

void CXTPCalendarControl::OnUpdateCopy(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID != ID_EDIT_COPY || !m_pActiveView)
	{
		ASSERT(FALSE);
		return;
	}
	pCmdUI->Enable(m_pActiveView->CanCopy());
}
void CXTPCalendarControl::OnUpdatePaste(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID != ID_EDIT_PASTE || !m_pActiveView)
	{
		ASSERT(FALSE);
		return;
	}
	pCmdUI->Enable(m_pActiveView->CanPaste());
}

void CXTPCalendarControl::OnContextMenu(CWnd* /*pWnd*/, CPoint pos)
{
	if (m_mouseMode != xtpCalendarMouseNothing)
		return;

	CPoint ptClient = pos;
	ScreenToClient(&ptClient);

	// forward message to subscriber
	DWORD dwPoint = MAKELONG(ptClient.x, ptClient.y);
	SendNotification(XTP_NC_CALENDARCONTEXTMENU, dwPoint, 0);
}

LRESULT CXTPCalendarControl::OnUserAction(WPARAM dwParam1, LPARAM dwParam2)
{
	int nButtonMask = xtpCalendarScrollDayButton_DayViewUp | xtpCalendarScrollDayButton_DayViewDown;
	int eButton = (int)(dwParam1 & nButtonMask);

	if ((dwParam1 & xtpCalendarUserAction_OnScrollDay) && eButton && m_pDayView)
	{
		CXTPCalendarDayViewDay* pDVday = m_pDayView->GetViewDay(HIWORD(dwParam2));
		CXTPCalendarDayViewGroup* pDVGroup = pDVday ? pDVday->GetViewGroup(LOWORD(dwParam2)) : NULL;
		ASSERT(pDVGroup);
		if (!pDVGroup || pDVGroup->UserAction_OnScrollDay((XTPCalendarScrollDayButton)eButton))
			return 0;

		CXTPCalendarDayViewEvent* pEVFirst = NULL, *pEVLast = NULL;
		pDVGroup->FindMinMaxGroupDayEvents(pEVFirst, pEVLast);

		if (eButton == xtpCalendarScrollDayButton_DayViewUp)
		{
			ASSERT(pEVFirst);
			if (pEVFirst)
				m_pDayView->EnsureVisible(pEVFirst);
		}
		else if (eButton == xtpCalendarScrollDayButton_DayViewDown)
		{
			ASSERT(pEVLast);
			if (pEVLast)
				m_pDayView->EnsureVisible(pEVLast);
		}
		else {
			ASSERT(FALSE);
		}
	}
	else  if (dwParam1 & xtpCalendarUserAction_OnExpandDay)
	{
		COleDateTime dtDate = (DATE)(LONG)dwParam2;

		nButtonMask = xtpCalendarExpandDayButton_WeekView | xtpCalendarExpandDayButton_MonthView;
		eButton = (int)(dwParam1 & nButtonMask);

		CXTPCalendarViewDay* pActiveDay = XTP_SAFE_GET1(GetActiveView(), _GetViewDay(dtDate), NULL);
		ASSERT(pActiveDay);
		if (!pActiveDay)
			return 0;

		if (pActiveDay->UserAction_OnExpandDay((XTPCalendarExpandDayButton)eButton))
			return 0;

		// default processing
		SwitchActiveView(xtpCalendarDayView);

		CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetActiveView());
		if (pDayView)
		{
			pDayView->ResetSelection();
			pDayView->ShowDay(dtDate);
		}
	}
	return 0;
}

// for compatibility with previous versions
LRESULT CXTPCalendarControl::OnSwitchView(WPARAM nDate, LPARAM)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	SwitchActiveView(xtpCalendarDayView);
	CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, m_pActiveView);
	if (pDayView)
	{
		pDayView->ResetSelection();
		pDayView->ShowDay(COleDateTime((DATE)nDate));
	}
	return 0;
}

// for compatibility with previous versions
void CXTPCalendarControl::QueueDayViewSwitch(DATE dtDate)
{
	PostMessage(xtp_wm_SwitchView, (UINT)dtDate);
}

BOOL CXTPCalendarControl::DoDeleteSelectedEvents(CXTPCalendarViewEvent* pViewEvent)
{
	CXTPCalendarView* pView = XTP_SAFE_GET3(pViewEvent, GetViewGroup_(), GetViewDay_(), GetView_(), NULL);
	if (!pView)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//------------------------------------------------------------------------
	if (pView->IsEditingSubject())
	{
		pView->EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}

	//------------------------------------------------------------------------
	CXTPCalendarViewEventsPtr ptrSelEventViews = pView->GetSelectedEvents();
	BOOL bSelExist = ptrSelEventViews && ptrSelEventViews->GetCount();
	BOOL bHandled = FALSE;

	ASSERT(m_pActiveView == pView);
	if (bSelExist && m_pActiveView)
	{
		bHandled = m_pActiveView->OnBeforeEditOperationNotify(xtpCalendarEO_DeleteSelectedEvents, ptrSelEventViews);
	}
	if (bHandled)
	{
		return TRUE;
	}
	//------------------------------------------------------------------------
	if (!bSelExist)
	{
		return DoDeleteEvent(pViewEvent);
	}

	int nCount = ptrSelEventViews ? ptrSelEventViews->GetCount() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarViewEvent* pEV = ptrSelEventViews->GetAt(i, FALSE);

		if (!DoDeleteEvent(pEV))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CXTPCalendarControl::DoDeleteEvent(CXTPCalendarViewEvent* pViewEvent)
{
	CXTPCalendarView* pView = XTP_SAFE_GET3(pViewEvent, GetViewGroup_(), GetViewDay_(), GetView_(), m_pActiveView);
	if (!pView)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPCalendarEvent* pEvent = pViewEvent ? pViewEvent->GetEvent() : NULL;
	CXTPCalendarData* pData = pEvent ? pEvent->GetDataProvider() : NULL;
	if (!pEvent || !pData)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//------------------------------------------------------------------------
	if (pView->IsEditingSubject())
	{
		pView->EndEditSubject(xtpCalendarEditSubjectCommit, FALSE);
	}

	//- Check is Event Still Exist -------------------------------------------
	if (!::IsEventExists(pData, pEvent))
	{
		return TRUE;
	}

	//------------------------------------------------------------------------
	BOOL bHandled = FALSE;
	if (m_pActiveView)
	{
		bHandled = m_pActiveView->OnBeforeEditOperationNotify(xtpCalendarEO_DeleteEvent, pViewEvent);
	}

	if (bHandled)
	{
		return TRUE;
	}

	//------------------------------------------------------------------------
	CXTPAutoResetValue<BOOL> autoSet_UpdateCtrlNf(m_bUpdateWhenEventChangedNotify);
	autoSet_UpdateCtrlNf = FALSE;

	//------------------------------------------------------------------------
	if (pEvent->GetRecurrenceState() != xtpCalendarRecurrenceNotRecurring)
	{
		CXTPCalendarOccurSeriesChooseDlg dlgOccSer(this, XTP_IDS_CALENDAR_OCURR_SERIES_DELETE);
		dlgOccSer.SetEvent(pEvent);

		if (dlgOccSer.DoModal() != IDOK)
		{
			return TRUE;
		}

		if (!dlgOccSer.m_bOccur)
		{
			// delete series

			CXTPCalendarRecurrencePatternPtr ptrRecPatt = pEvent->GetRecurrencePattern();

			if (!ptrRecPatt)
			{
				ASSERT(FALSE);
				return FALSE;
			}
			CXTPCalendarEventPtr ptrMasterEvent = ptrRecPatt->GetMasterEvent();
			if (ptrMasterEvent)
			{
				return pData->DeleteEvent(ptrMasterEvent);
			}

			ASSERT(FALSE);
			return FALSE;
		}
	}

	return pData->DeleteEvent(pEvent);
}

void CXTPCalendarControl::OnSetFocus(CWnd* pOldWnd)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CWnd::OnSetFocus(pOldWnd);

#ifdef XTP_CALENDAR_SITENOTIFY_ONFOCUS
	XTP_CALENDAR_SITENOTIFY_ONFOCUS(this, this, TRUE)
#endif
}

void CXTPCalendarControl::OnKillFocus (CWnd* pNewWnd)
{
	CXTPSmartPtrInternalT<CCmdTarget> ptrThisLock(this, TRUE);

	CWnd::OnKillFocus(pNewWnd);

#ifdef XTP_CALENDAR_SITENOTIFY_ONFOCUS
	XTP_CALENDAR_SITENOTIFY_ONFOCUS(this, this, FALSE)
#endif
}

void CXTPCalendarControl::SetTheme(CXTPCalendarTheme* pTheme)
{
	if (m_pTheme)
	{
		m_pTheme->SetCalendarControl(NULL);
		CMDTARGET_RELEASE(m_pTheme)
	}

	m_pTheme = pTheme;

	if (m_pTheme)
	{
		m_pTheme->SetCalendarControl(this);
		m_pTheme->RefreshMetrics();
	}
	else
	{
		XTP_SAFE_CALL1(GetPaintManager(), RefreshMetrics());
	}
}

CXTPCalendarOptions* CXTPCalendarControl::GetCalendarOptions() const
{
	return m_pOptions;
}

// calendar settings
int CXTPCalendarControl::GetWorkWeekMask() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->nWorkWeekMask;
	}
	return xtpCalendarDayMo_Fr;
}

void CXTPCalendarControl::SetWorkWeekMask(const int nMask)
{
	if (!nMask) // Invalid mask
		return;

	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->nWorkWeekMask = nMask;
		OnOptionsChanged((int)-1);
	}
}

int CXTPCalendarControl::GetFirstDayOfWeek() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		int nFirstDayOfWeek = pOpt->nFirstDayOfTheWeek;
		ASSERT(nFirstDayOfWeek >= 1 && nFirstDayOfWeek <= 7);

		return max(min(7, nFirstDayOfWeek), 1);
	}
	return 1;
}

void CXTPCalendarControl::SetFirstDayOfWeek(const int nFirstDayOfWeek)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		ASSERT(nFirstDayOfWeek >= 1 && nFirstDayOfWeek <= 7);
		if (nFirstDayOfWeek >= 1 && nFirstDayOfWeek <= 7)
		{
			pOpt->nFirstDayOfTheWeek = nFirstDayOfWeek;
		}

		OnOptionsChanged((int)-1);
	}
}

void CXTPCalendarControl::GetWorkDayStartTime(int& nHour, int& nMin, int& nSec)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		nHour = pOpt->dtWorkDayStartTime.GetHour();
		nMin = pOpt->dtWorkDayStartTime.GetMinute();
		nSec = pOpt->dtWorkDayStartTime.GetSecond();
	}
}

void CXTPCalendarControl::GetWorkDayStartTime(COleDateTime& dtTime)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		dtTime = pOpt->dtWorkDayStartTime;
	}
}

void CXTPCalendarControl::GetWorkDayEndTime(int& nHour, int& nMin, int& nSec)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		nHour = pOpt->dtWorkDayEndTime.GetHour();
		nMin = pOpt->dtWorkDayEndTime.GetMinute();
		nSec = pOpt->dtWorkDayEndTime.GetSecond();
	}
}

void CXTPCalendarControl::GetWorkDayEndTime(COleDateTime& dtTime)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		dtTime = pOpt->dtWorkDayEndTime;
	}
}

void CXTPCalendarControl::SetWorkDayStartTime(int nHour, int nMin, int nSec)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->dtWorkDayStartTime.SetTime(nHour, nMin, nSec);

		OnOptionsChanged(xtpCalendarDayView);
	}
}

void CXTPCalendarControl::SetWorkDayStartTime(const COleDateTime& dtTime)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->dtWorkDayStartTime = dtTime;

		OnOptionsChanged(xtpCalendarDayView);
	}
}

void CXTPCalendarControl::SetWorkDayEndTime(int nHour, int nMin, int nSec)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->dtWorkDayEndTime.SetTime(nHour, nMin, nSec);

		OnOptionsChanged(xtpCalendarDayView);
	}
}

void CXTPCalendarControl::SetWorkDayEndTime(const COleDateTime& dtTime)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->dtWorkDayEndTime = dtTime;

		OnOptionsChanged(xtpCalendarDayView);
	}
}

BOOL CXTPCalendarControl::DayView_IsAutoResetBusyFlag() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->bDayView_AutoResetBusyFlag;
	}
	return TRUE;
}

void CXTPCalendarControl::DayView_SetAutoResetBusyFlag(BOOL bAutoResetBusyFlag)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->bDayView_AutoResetBusyFlag = bAutoResetBusyFlag;

		OnOptionsChanged(xtpCalendarDayView);
	}
}

BOOL CXTPCalendarControl::MonthView_IsCompressWeekendDays() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->bMonthView_CompressWeekendDays;
	}
	return TRUE;
}

void CXTPCalendarControl::MonthView_SetCompressWeekendDays(BOOL bCompress)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->bMonthView_CompressWeekendDays = bCompress;

		OnOptionsChanged(xtpCalendarMonthView);
	}
}

BOOL CXTPCalendarControl::MonthView_IsShowEndDate() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->bMonthView_ShowEndDate;
	}
	return FALSE;
}

void CXTPCalendarControl::MonthView_SetShowEndDate(BOOL bShowEnd)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->bMonthView_ShowEndDate = bShowEnd;

		OnOptionsChanged(xtpCalendarMonthView);
	}
}

BOOL CXTPCalendarControl::MonthView_IsShowTimeAsClocks() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->bMonthView_ShowTimeAsClocks;
	}
	return FALSE;
}

void CXTPCalendarControl::MonthView_SetShowTimeAsClocks(BOOL bShowClocks)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->bMonthView_ShowTimeAsClocks = bShowClocks;

		OnOptionsChanged(xtpCalendarMonthView);
	}
}

BOOL CXTPCalendarControl::WeekView_IsShowEndDate() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->bWeekView_ShowEndDate;
	}
	return FALSE;
}

void CXTPCalendarControl::WeekView_SetShowEndDate(BOOL bShowEnd)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->bWeekView_ShowEndDate = bShowEnd;

		OnOptionsChanged(xtpCalendarWeekView);
	}
}

BOOL CXTPCalendarControl::WeekView_IsShowTimeAsClocks() const
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		return pOpt->bWeekView_ShowTimeAsClocks;
	}
	return FALSE;
}

void CXTPCalendarControl::WeekView_SetShowTimeAsClocks(BOOL bShowClocks)
{
	CXTPCalendarOptions* pOpt = GetCalendarOptions();
	ASSERT(pOpt);
	if (pOpt)
	{
		pOpt->bWeekView_ShowTimeAsClocks = bShowClocks;

		OnOptionsChanged(xtpCalendarWeekView);
	}
}

void CXTPCalendarControl::OnEvent_FromDataProvider(XTP_NOTIFY_CODE Event,
												WPARAM wParam, LPARAM lParam)
{
	ASSERT(Event == XTP_NC_CALENDAROPTIONSWASCHANGED ||
		Event == XTP_NC_CALENDAREVENTWASADDED ||
		Event == XTP_NC_CALENDAREVENTWASDELETED ||
		Event == XTP_NC_CALENDAREVENTWASCHANGED ||
		Event == XTP_NC_CALENDARPATTERNWASADDED ||
		Event == XTP_NC_CALENDARPATTERNWASDELETED ||
		Event == XTP_NC_CALENDARPATTERNWASCHANGED
		||
		Event == XTP_NC_CALENDAR_DoRetrieveDayEvents ||
		Event == XTP_NC_CALENDAR_DoRemoveAllEvents ||
		Event == XTP_NC_CALENDAR_DoRead_Event ||
		Event == XTP_NC_CALENDAR_DoRead_RPattern ||
		Event == XTP_NC_CALENDAR_DoCreate_Event ||
		Event == XTP_NC_CALENDAR_DoUpdate_Event ||
		Event == XTP_NC_CALENDAR_DoDelete_Event ||
		Event == XTP_NC_CALENDAR_DoCreate_RPattern ||
		Event == XTP_NC_CALENDAR_DoUpdate_RPattern ||
		Event == XTP_NC_CALENDAR_DoDelete_RPattern ||
		Event == XTP_NC_CALENDAR_DoGetUpcomingEvents ||
		Event == XTP_NC_CALENDAR_DoGetAllEvents_raw );

	SendNotification(Event, wParam, lParam);

	if (Event == XTP_NC_CALENDAROPTIONSWASCHANGED)
	{
		KillTimer(XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID);
		CWnd::SetTimer(XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMERID, XTP_CALENDAR_REFRESHMETRICS_REQUEST_TIMEOUT, NULL);
	}

	//-----------------------------------------------------------------------
	if (!m_bUpdateWhenEventChangedNotify)
	{
		return;
	}

	if (Event == XTP_NC_CALENDAREVENTWASADDED ||
		Event == XTP_NC_CALENDAREVENTWASDELETED ||
		Event == XTP_NC_CALENDAREVENTWASCHANGED)
	{
		CXTPCalendarEvent* pEvent = (CXTPCalendarEvent*)lParam;
		if (m_pActiveView)
		{
			int nUpdateType = m_pActiveView->OnEventChanged_InDataProvider(Event, pEvent);
			if (nUpdateType == xtpCalendar_Populate)
			{
				if (m_pActiveView->IsEditingSubject())
				{
					m_pActiveView->EndEditSubject(xtpCalendarEditSubjectCancel);
				}
				if (m_pActiveView->GetDraggingEventNew())
				{
					m_pActiveView->CancelDraggingEvent();
				}

				if (m_uPopulateRequest_TimerID)
				{
					KillTimer(m_uPopulateRequest_TimerID);
				}
				m_uPopulateRequest_TimerID = SetTimer(XTP_CALENDAR_POPULATE_REQUEST_TIMEOUT);
			}
			else if (nUpdateType == xtpCalendar_Redraw)
			{
				if (m_uRedrawRequest_TimerID)
				{
					KillTimer(m_uRedrawRequest_TimerID);
				}
				m_uRedrawRequest_TimerID = SetTimer(XTP_CALENDAR_REDRAW_REQUEST_TIMEOUT);
			}
		}
	}
}

void CXTPCalendarControl::OnEvent_Reminders(XTP_NOTIFY_CODE Event, WPARAM wParam , LPARAM lParam)
{
	ASSERT( Event == XTP_NC_CALENDAR_ON_REMINDERS );
	SendNotification(Event, wParam, lParam);
}

void CXTPCalendarControl::SendNotification(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam)
{
	if (this && m_pConnect && m_bEnableSendNotifications)
	{
		m_pConnect->SendEvent(EventCode, wParam, lParam);
	}
}

void CXTPCalendarControl::SendNotificationAlways(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam)
{
	if (this && m_pConnect)
	{
		m_pConnect->SendEvent(EventCode, wParam, lParam);
	}
}

void CXTPCalendarControl::OnFinalRelease()
{
	CWnd::OnFinalRelease();

	if (m_bDeleteOnFinalRelease)
	{
		CCmdTarget::OnFinalRelease();
	}
}

void CXTPCalendarControl::SetLayoutRTL(BOOL bRightToLeft)
{
	if (!XTPSystemVersion()->IsLayoutRTLSupported())
		return;

	if (!m_hWnd)
		return;

	ModifyStyleEx(bRightToLeft ? 0 : WS_EX_LAYOUTRTL, !bRightToLeft ? 0 : WS_EX_LAYOUTRTL);
	RedrawControl();
}

DWORD CXTPCalendarControl::GetAskItemTextFlags()
{
	DWORD dwFlags = 0;
	if (GetTheme())
	{
		dwFlags = GetTheme()->GetAskItemTextFlags();
	}
	else
	{
		dwFlags = XTP_SAFE_GET1(GetPaintManager(), GetAskItemTextFlags(), 0);
	}

	return dwFlags;
}

CXTPCalendarControl::CViewChanged_ContextData::CViewChanged_ContextData()
{
	m_nLockCount = 0;
	m_bRequest = FALSE;
}

CXTPCalendarControl::CViewChangedContext::CViewChangedContext(
											CXTPCalendarView* pView, int eType)
{
	if (!pView)
	{
		ASSERT(FALSE);
		return;
	}
	Init(pView->GetCalendarControl(), eType);
}

CXTPCalendarControl::CViewChangedContext::CViewChangedContext(CXTPCalendarControl* pControl, int eType)
{
	Init(pControl, eType);
}

void CXTPCalendarControl::CViewChangedContext::Init(CXTPCalendarControl* pControl, int eType)
{
	if (!pControl)
	{
		ASSERT(FALSE);
		return;
	}

	m_pControl = pControl;
	m_eType = eType;

	m_pControl->m_cntViewChanged.m_nLockCount++;
}

CXTPCalendarControl::CViewChangedContext::~CViewChangedContext()
{
	if (!m_pControl)
	{
		ASSERT(FALSE);
		return;
	}

	if (m_eType == xtpCalendarViewChangedSend)
	{
		m_pControl->m_cntViewChanged.m_bRequest = TRUE;
	}

	ASSERT(m_pControl->m_cntViewChanged.m_nLockCount > 0);

	m_pControl->m_cntViewChanged.m_nLockCount--;

	if (m_pControl->m_cntViewChanged.m_nLockCount == 0 &&
		m_pControl->m_cntViewChanged.m_bRequest)
	{
		m_pControl->m_cntViewChanged.m_bRequest = FALSE;

		m_pControl->SendNotification(XTP_NC_CALENDARVIEWWASCHANGED, 0, 0);
	}
}
