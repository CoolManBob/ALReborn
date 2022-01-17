// XTPDatePickerControl.cpp: implementation of the CXTPDatePickerControl class.
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

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPSystemHelpers.h"

#include "XTPTopLevelWndMsgNotifier.h"
#include "XTPCalendarUtils.h"
#include "XTPDatePickerItemMonth.h"
#include "XTPDatePickerPaintManager.h"
#include "XTPDatePickerDaysCollection.h"
#include "XTPDatePickerList.h"
#include "XTPDatePickerItemDay.h"
#include "XTPDatePickerControl.h"
#include "XTPDatePickerNotifications.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable: 4571) // warning C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions

// Custom class name for the Date Picker control window
const TCHAR XTP_DATEPICKERCTRL_CLASSNAME[] = _T("XTPDatePicker");

//////////////////////////////////////////////////////////////////////////
// CXTPDatePickerButtons

int CXTPDatePickerButtons::GetVisibleButtonCount() const
{
	int nCount = 0;
	for (int i = 0; i < (int)GetSize(); i++)
	{
		if (GetAt(i)->m_bVisible) nCount++;
	}
	return nCount;
}
CXTPDatePickerButton* CXTPDatePickerButtons::Find(int nID) const
{
	for (int i = 0; i < (int)GetSize(); i++)
	{
		if (GetAt(i)->m_nID == nID)
			return GetAt(i);
	}
	return NULL;
}
CXTPDatePickerButton* CXTPDatePickerButtons::HitTest(CPoint point) const
{
	for (int i = 0; i < (int)GetSize(); i++)
	{
		if (GetAt(i)->m_bVisible && GetAt(i)->m_rcButton.PtInRect(point))
			return GetAt(i);
	}
	return NULL;

}

//////////////////////////////////////////////////////////////////////////
// XTP_DAYITEM_METRICS

XTP_DAYITEM_METRICS::XTP_DAYITEM_METRICS()
{

	clrForeground = 0;
	clrBackground = 0;
}

CFont* XTP_DAYITEM_METRICS::GetFont()
{
	return &m_fntText;
}

void XTP_DAYITEM_METRICS::SetFont(CFont* pFont)
{
	ASSERT_VALID(pFont);
	if (!pFont)
		return;
	// set new font
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_fntText.DeleteObject();
	m_fntText.CreateFontIndirect(&lf);
}


CXTPDatePickerButton::CXTPDatePickerButton()
{
	m_rcButton.SetRectEmpty();
	m_bVisible = TRUE;
	m_bPressed = FALSE;
	m_bHighlight = FALSE;
	m_nID = 0;
}

CString CXTPDatePickerButton::GetCaption()
{
	if (m_strCaption.IsEmpty())
		return CXTPCalendarUtils::LoadString(m_nID);

	return m_strCaption;
}

void CXTPDatePickerButton::SetCaption(LPCTSTR pcszCaption)
{
	m_strCaption = pcszCaption;
}

//////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerControl

CXTPDatePickerControl::CXTPDatePickerControl()
{
	m_pConnect = new CXTPNotifyConnection;

	RegisterWindowClass();

	m_pfnCallback = NULL;
	m_pCallbackParam = NULL;
	m_nLockUpdateCount = 0;

	m_lcidActiveLocale = CXTPCalendarUtils::GetActiveLCID();

	m_pPaintManager = NULL;
	SetTheme(NULL);// set default paint manager
	//SetTheme(new CXTPDatePickerThemeOffice2007());

	m_bAutoSize = TRUE;
	m_mouseMode = mouseNothing;
	m_nTimerID = 0;
	m_bIsModal = FALSE;
	m_nMaxSelectionDays = XTP_SELECTION_INFINITE;
	m_bSelectWeek = FALSE;
	m_bRightToLeft = FALSE;

	m_bShowWeekNumbers = FALSE;
	m_nRows = 1;
	m_nColumns = 1;
	m_nFirstDayOfWeek = 2;
	m_nFirstWeekOfYearDays = 1;
	m_nMonthDelta = 0;
	m_bHighlightToday = TRUE;
	m_bChanged = TRUE;
	m_dtToday = COleDateTime::GetCurrentTime();
	m_dtFirstMonth.SetDate(m_dtToday.GetYear(), m_dtToday.GetMonth(), 1);
	m_dtMinRange.SetStatus(COleDateTime::null);
	m_dtMaxRange.SetStatus(COleDateTime::null);

	m_rcControl.SetRectEmpty();
	m_rcGrid.SetRectEmpty();
	m_bShowNonMonthDays = TRUE;
	m_borderStyle = xtpDatePickerBorder3D;

	m_pButtonCaptured = NULL;

	AddButton(XTP_IDS_DATEPICKER_TODAY);
	AddButton(XTP_IDS_DATEPICKER_NONE);

	m_pListControl = NULL;

	m_pSelectedDays = new CXTPDatePickerDaysCollection(this);

	m_arMonthNames = new CString[12];
	m_arDayOfWeekNames = new CString[7];

	m_bAllowNoncontinuousSelection = TRUE;

	m_bDeleteOnFinalRelease = FALSE;

	InitNames();

	ClearFocus();

	Populate();
	//-----------------------------------------------------------------------
	XTPGetTopLevelWndMsgNotifier()->Advise(this, WM_TIMECHANGE);
}

CXTPDatePickerControl::~CXTPDatePickerControl()
{
	XTPGetTopLevelWndMsgNotifier()->Unadvise(this);

	ClearMonths();

	if (m_pPaintManager)
		m_pPaintManager->SetControl(NULL);
	CMDTARGET_RELEASE(m_pPaintManager);

	if (m_pListControl)
	{
		m_pListControl->DestroyWindow();
		delete m_pListControl;
	}

	CMDTARGET_RELEASE(m_pSelectedDays);

	if (m_nTimerID != 0 && m_hWnd)
		KillTimer(m_nTimerID);

	DestroyWindow();

	for (int i = 0; i < GetButtonCount(); i++)
		delete m_arrButtons[i];

	delete[] m_arDayOfWeekNames;
	delete[] m_arMonthNames;

	CMDTARGET_RELEASE(m_pConnect);
}

void CXTPDatePickerControl::OnDestroy()
{
	if (m_bIsModal)
	{
		if (ContinueModal()) // skip if EndModalLoop has already called
		{
			EndModalLoop(IDABORT);
		}
	}
	CWnd::OnDestroy();
}

void CXTPDatePickerControl::AddButton(UINT nID)
{
	CXTPDatePickerButton* pButton = new CXTPDatePickerButton();

	pButton->m_nID = nID;

	m_arrButtons.Add(pButton);
}

BOOL CXTPDatePickerControl::IsTodayButtonVisible()
{
	CXTPDatePickerButton* pButton = m_arrButtons.Find(XTP_IDS_DATEPICKER_TODAY);
	return pButton ? pButton->m_bVisible : FALSE;
}

BOOL CXTPDatePickerControl::IsNoneButtonVisible()
{
	CXTPDatePickerButton* pButton = m_arrButtons.Find(XTP_IDS_DATEPICKER_NONE);
	return pButton ? pButton->m_bVisible : FALSE;
}

void CXTPDatePickerControl::SetTheme(CXTPDatePickerPaintManager* pPaintManager)
{
	// set default
	if (!pPaintManager)
		pPaintManager = new CXTPDatePickerPaintManager();

	if (m_pPaintManager)
		m_pPaintManager->SetControl(NULL);

	CMDTARGET_RELEASE(m_pPaintManager);
	m_pPaintManager = pPaintManager;

	if (m_pPaintManager)
	{
		m_pPaintManager->SetControl(this);
		m_pPaintManager->RefreshMetrics();
	}

	_RedrawControl(FALSE);
}

void CXTPDatePickerControl::RedrawControl()
{
	_RedrawControl(TRUE);
}

void CXTPDatePickerControl::_RedrawControl(BOOL bUpdateNow)
{
	m_bChanged = TRUE;

	if (GetSafeHwnd() && (m_nLockUpdateCount == 0))
	{
		Invalidate(FALSE);
		if (bUpdateNow)
		{
			UpdateWindow();
		}
	}
}

BEGIN_MESSAGE_MAP(CXTPDatePickerControl, CWnd)
	//{{AFX_MSG_MAP(CXTPDatePickerControl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()
	ON_WM_CANCELMODE()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_TIMECHANGE()
	ON_WM_SIZE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()

	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_DESTROY()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerControl message handlers

BOOL CXTPDatePickerControl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

int CXTPDatePickerControl::OnCreate(LPCREATESTRUCT lp)
{
	int nRet = CWnd::OnCreate(lp);

	CXTPClientRect rc(this);
	AdjustLayout(rc);

	if (m_bRightToLeft)
	{
		SetLayoutRTL(TRUE);
	}

	return nRet;
}

void CXTPDatePickerControl::PreSubclassWindow()
{
	CXTPClientRect rc(this);
	AdjustLayout(rc);
}

BOOL CXTPDatePickerControl::GoModal(const RECT& rect, CWnd* pParentWnd)
{
	if (!pParentWnd)
		pParentWnd = AfxGetMainWnd();

	CRect rcScreen(rect);

	if (!::IsWindow(m_hWnd) && !CreateEx(WS_EX_TOOLWINDOW, XTP_DATEPICKERCTRL_CLASSNAME, NULL, pParentWnd ? WS_CHILD : WS_POPUP, rcScreen, pParentWnd, 0))
		return FALSE;

	// Enable this window
	EnableWindow(TRUE);

	CWnd* pFocusWnd = SetFocus();

	if (pParentWnd)
	{
		SetWindowLongPtr(m_hWnd, GWLP_HWNDPARENT, 0);
		ModifyStyle(WS_CHILD, WS_POPUP);
		SetWindowLongPtr(m_hWnd, GWLP_HWNDPARENT, (LONG_PTR)pParentWnd->m_hWnd);
	}

	SetWindowPos(&CWnd::wndTopMost, rcScreen.left, rcScreen.top, rcScreen.Width(), rcScreen.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	SetCapture();

	SendNotification(XTP_NC_DATEPICKERBEFOREGOMODAL);
	SendMessageToParent(XTP_NC_DATEPICKERBEFOREGOMODAL);

	m_bIsModal = TRUE;

	m_nFlags |= WF_CONTINUEMODAL;
	int nResult = m_nModalResult;
	if (ContinueModal())
	{
		// enter modal loop
		DWORD dwFlags = MLF_SHOWONIDLE;
		if (GetStyle() & DS_NOIDLEMSG)
			dwFlags |= MLF_NOIDLEMSG;
		nResult = RunModalLoop(dwFlags);
	}

	ReleaseCapture();
	DestroyWindow();

	m_bIsModal = FALSE;

	if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
		pFocusWnd->SetFocus();

	return (nResult == IDOK);
}

CSize CXTPDatePickerControl::SetGridDimentions(CRect rcClient)
{
	CWindowDC dc(GetOwner());
	CSize szMonth(m_pPaintManager->CalcMonthRect(&dc));

	szMonth.cy += 2;

	m_nRows = max(1, rcClient.Height() / szMonth.cy);
	m_nColumns = max(1, rcClient.Width() / szMonth.cx);

	if (m_dtMaxRange.GetStatus() == COleDateTime::valid)
	{
		int nMaxRangeMonths = (m_dtMaxRange.GetYear() * 12 + m_dtMaxRange.GetMonth()) -
			(m_dtFirstMonth.GetYear() * 12 + m_dtFirstMonth.GetMonth()) + 1;

		if (m_dtMinRange.GetStatus() == COleDateTime::valid)
		{
			int nTotalMaxRangeMonths = (m_dtMaxRange.GetYear() * 12 + m_dtMaxRange.GetMonth()) -
				(m_dtMinRange.GetYear() * 12 + m_dtMinRange.GetMonth()) + 1;
			while (m_nRows * m_nColumns > nTotalMaxRangeMonths)
			{
				if (m_nRows > 1)
					m_nRows--;
				else if (m_nColumns > 1)
					m_nColumns--;
				else
					break;
			}
		}

		if (m_nRows * m_nColumns > nMaxRangeMonths)
		{
			ShiftDate(m_dtFirstMonth, nMaxRangeMonths - m_nRows * m_nColumns);
		}

		if (m_dtMinRange.GetStatus() == COleDateTime::valid &&
			m_dtFirstMonth < m_dtMinRange)
		{
			m_dtFirstMonth = m_dtMinRange;
			while (m_nRows * m_nColumns > nMaxRangeMonths)
			{
				if (m_nRows > 1)
					m_nRows--;
				else if (m_nColumns > 1)
					m_nColumns--;
				else
					break;
			}
		}
	}
	return szMonth;
}

void CXTPDatePickerControl::AdjustLayout(CRect rcClient)
{
	if (!GetSafeHwnd())
		return;

	if (m_lcidActiveLocale != CXTPCalendarUtils::GetActiveLCID())
	{
		m_lcidActiveLocale = CXTPCalendarUtils::GetActiveLCID();

		InitNames();
	}

	m_dtToday = COleDateTime::GetCurrentTime();

	m_rcControl = rcClient;

	CSize szMonth(0, 0);
	CRect rcMonth(0, 0, 0, 0);

	//get button size
	CSize szButton(CalcButtonSize());

	m_rcGrid.CopyRect(&m_rcControl);

	m_pPaintManager->DrawBorder(0, this, m_rcGrid, FALSE);

	if (m_arrButtons.GetVisibleButtonCount() > 0)
		m_rcGrid.bottom -= szButton.cy;

	if (!m_bAutoSize)
	{
		szMonth.cx = m_rcGrid.Width() / m_nColumns;
		szMonth.cy = m_rcGrid.Height() / m_nRows;
	}
	else
	{
		szMonth = SetGridDimentions(m_rcGrid);
		CSize szGrid (szMonth.cx * m_nColumns, szMonth.cy * m_nRows);

		int nXOffset = max(0, (m_rcGrid.left + m_rcGrid.right - szGrid.cx) / 2);
		int nYOffset = max(0, (m_rcGrid.top + m_rcGrid.bottom - szGrid.cy) / 2);

		m_rcGrid = CRect(CPoint(nXOffset, nYOffset), szGrid);

		ClearMonths();
		CreateMonthArray();

	} // else auto end

	CalcButtonBandRect();
	int nIndex = 0;
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		for (int nCol = 0; nCol < m_nColumns; nCol++)
		{
			rcMonth = CRect(CPoint(m_rcGrid.left + nCol * szMonth.cx, m_rcGrid.top + nRow * szMonth.cy), szMonth);
			rcMonth.DeflateRect(1, 1);

			// get next month item
			CXTPDatePickerItemMonth* pMonth = m_arrMonths.GetAt(nIndex);
			nIndex++;
			// adjust internal month layout
			pMonth->AdjustLayout(rcMonth, !m_bAutoSize);
		}
	}

	m_bChanged = TRUE;
}

void CXTPDatePickerControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CXTPClientRect rc(this);

	// Check cached bitmap
	if (!m_bChanged && m_bmpCache.GetSafeHandle() != 0)
	{
		CXTPCompatibleDC memDC(&dc, &m_bmpCache);
		dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);
	}
	else
	{
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		m_bmpCache.DeleteObject();
		m_bmpCache.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

		CBitmap* pOldBitmap = memDC.SelectObject(&m_bmpCache);

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

void CXTPDatePickerControl::OnDraw(CDC* pDC)
{
	CXTPClientRect rcClient(this);

	// draw background
	m_pPaintManager->DrawBackground(pDC, rcClient);

	// draw all month items in the collection
	int nMonthCount = (int)m_arrMonths.GetSize();
	for (int nIndex = 0; nIndex < nMonthCount; nIndex++)
	{
		// get next month item
		CXTPDatePickerItemMonth* pMonth = m_arrMonths.GetAt(nIndex);
		// draw it
		pMonth->Draw(pDC);
	}

	// draw today/none buttons
	DrawButtons(pDC);

	// draw border
	m_pPaintManager->DrawBorder(pDC, this, rcClient, TRUE);
}

UINT CXTPDatePickerControl::OnGetDlgCode()
{
	return DLGC_WANTARROWS /*| DLGC_WANTTAB | DLGC_WANTALLKEYS*/;
}

void CXTPDatePickerControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef XTP_DATEPICKER_SITENOTIFY_KEY
	XTP_DATEPICKER_SITENOTIFY_KEY(this, TRUE, nChar)
#endif

	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		if (m_bIsModal)
		{
			if (ContinueModal()) // skip if EndModalLoop has already called
			{
				EndModalLoop(nChar == VK_RETURN ? IDOK : IDCANCEL);
			}
			return;
		}
	}
	else if (nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP ||
			nChar == VK_DOWN || nChar == VK_PRIOR || nChar == VK_NEXT ||
			nChar == VK_HOME || nChar == VK_END ||
			nChar == VK_SPACE)
	{
		CXTPSelectionHelper _selector(this);

		BOOL bIsShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
		BOOL bIsCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
		XTPDrawHelpers()->KeyToLayout(this, nChar);

		if (nChar == VK_RIGHT)
		{
			_selector.MoveFocus(stepDay, dirNext, bIsShift, bIsCtrl);
		}
		else if (nChar == VK_DOWN)
		{
			_selector.MoveFocus(stepWeek, dirNext, bIsShift, bIsCtrl);
		}
		else if (nChar == VK_NEXT && !bIsCtrl)
		{
			_selector.MoveFocus(stepMonth, dirNext, FALSE, FALSE);
		}
		else if (nChar == VK_NEXT && bIsCtrl)
		{
			_selector.MoveFocus(stepYear, dirNext, FALSE, FALSE);
		}
		//*****
		else if (nChar == VK_LEFT)
		{
			_selector.MoveFocus(stepDay, dirPrev, bIsShift, bIsCtrl);
		}
		else if (nChar == VK_UP)
		{
			_selector.MoveFocus(stepWeek, dirPrev, bIsShift, bIsCtrl);
		}
		else if (nChar == VK_PRIOR && !bIsCtrl)
		{
			_selector.MoveFocus(stepMonth, dirPrev, FALSE, FALSE);
		}
		else if (nChar == VK_PRIOR && bIsCtrl)
		{
			_selector.MoveFocus(stepYear, dirPrev, FALSE, FALSE);
		}
		//***
		else if (nChar == VK_HOME && !bIsCtrl)
		{
			_selector.MoveFocus(stepWeekBE, dirPrev, FALSE, FALSE);
		}
		else if (nChar == VK_HOME && bIsCtrl)
		{
			_selector.MoveFocus(stepMonthBE, dirPrev, FALSE, FALSE);
		}
		//***
		else if (nChar == VK_END && !bIsCtrl)
		{
			_selector.MoveFocus(stepWeekBE, dirNext, FALSE, FALSE);
		}
		else if (nChar == VK_END && bIsCtrl)
		{
			_selector.MoveFocus(stepMonthBE, dirNext, FALSE, FALSE);
		}
		//***
		else if (nChar == VK_SPACE && bIsCtrl)
		{
			_selector.SelUnselFocus();
		}

		//***
		_selector._TmpSaveFocus();
		m_nLockUpdateCount++;

		// fire selection changed
		SendNotification(XTP_NC_DATEPICKERSELECTIONCHANGED);
		SendMessageToParent(XTP_NC_DATEPICKER_SELECTION_CHANGED);
		//***
		_selector._TmpRestoreFocus();
		m_nLockUpdateCount--;

		//-------------------
		EnsureVisibleFocus();

		//--------------
		RedrawControl();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPDatePickerControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bIsModal && !m_rcControl.PtInRect(point))
	{
		if (ContinueModal()) // skip if EndModalLoop has already called
		{
			EndModalLoop(IDCANCEL);
		}
		return;
	}
	if (!m_bIsModal)
	{
		SetCapture();
	}
	SetFocus();

	CXTPDatePickerItemMonth* pMonth = HitTest(point);
	if (pMonth)
	{
		pMonth->OnLButtonDown(nFlags, point);
	}

	// process buttons
	m_pButtonCaptured = m_arrButtons.HitTest(point);

	ProcessButtons(point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CXTPDatePickerControl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CXTPDatePickerItemMonth* pMonth = HitTest(point);
	if (pMonth)
	{
		pMonth->OnLButtonDown(nFlags, point);
	}

	m_pButtonCaptured = m_arrButtons.HitTest(point);
	ProcessButtons(point);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CXTPDatePickerControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// kill timer
	if (m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	// logic message processing
	if (m_mouseMode == mouseTrackingHeaderList)
	{
		// reset mouse mode
		m_mouseMode = mouseNothing;

		if (m_pListControl)
		{
			int nDelta = m_pListControl->GetMonthInterval();
			// destroy list control
			m_pListControl->DestroyWindow();
			delete m_pListControl;
			m_pListControl = NULL;
			// scroll on selected months count
			if (nDelta < 0)
				ScrollLeft(-nDelta);
			else if (nDelta > 0)
				ScrollRight(nDelta);
		}
	}
	else
	{
		// forward message to appropriate month
		CXTPDatePickerItemMonth* pMonth = HitTest(point);
		if (pMonth)
		{
			pMonth->OnLButtonUp(nFlags, point);
		}

		// Update selection
		BOOL bSelecting = m_mouseMode == mouseSelecting;
		BOOL bDeselecting = m_mouseMode == mouseDeselecting;
		if (bSelecting || bDeselecting)
		{
			int nDay = (m_dtLastClicked - m_dtFirstClicked).GetDays();
			for (; abs(nDay) >= 0; nDay += nDay > 0 ? -1 : 1)
			{
				COleDateTime dtSelect(m_dtFirstClicked);
				dtSelect += nDay;
				if (bDeselecting)
				{
					m_pSelectedDays->Remove(dtSelect);
				}
				else if (bSelecting && IsSelected(dtSelect))
				{
					m_pSelectedDays->Add(dtSelect);
				}

				if (nDay == 0)
					break;
			}

			// reset mouse mode
			m_mouseMode = mouseNothing;

			// fire selection changed
			SendNotification(XTP_NC_DATEPICKERSELECTIONCHANGED);
			SendMessageToParent(XTP_NC_DATEPICKER_SELECTION_CHANGED);
			_EndModalIfNeed();
		}
	}

	//release resources
	if (!m_bIsModal)
		ReleaseCapture();


	// process buttons
	m_pButtonCaptured = NULL;
	ProcessButtons(point);


	// reset mouse mode
	m_mouseMode = mouseNothing;

	CWnd::OnLButtonUp(nFlags, point);
}

void CXTPDatePickerControl::OnCaptureChanged(CWnd* pWnd)
{
	// kill timer
	if (m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	// logic message processing
	if (m_mouseMode == mouseTrackingHeaderList)
	{
		// reset mouse mode
		m_mouseMode = mouseNothing;

		if (m_pListControl)
		{
			if (m_pListControl->GetSafeHwnd())
				m_pListControl->DestroyWindow();
			delete m_pListControl;
			m_pListControl = NULL;
		}
	}
	m_pButtonCaptured = NULL;

	if (m_bIsModal)
	{
		if (ContinueModal()) // skip if EndModalLoop has already called
		{
			EndModalLoop(IDOK);
		}
	}

	CWnd::OnCaptureChanged(pWnd);
}

void CXTPDatePickerControl::OnCancelMode()
{
	CWnd::OnCancelMode();
}

void CXTPDatePickerControl::OnMouseLeave()
{
	TRACKMOUSEEVENT tre;
	tre.dwFlags = TME_CANCEL;
	tre.hwndTrack = this->m_hWnd;
	tre.dwHoverTime = HOVER_DEFAULT;
	tre.cbSize = sizeof(tre);

	_TrackMouseEvent(&tre);

	ProcessButtons(CPoint(-1, -1));
}


void CXTPDatePickerControl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_mouseMode == mouseTrackingHeaderList)
	{
		if (m_pListControl)
		{
			m_pListControl->OnMouseMove(nFlags, point);
		}
	}
	else
	{
		// forward message to appropriate month
		CXTPDatePickerItemMonth* pMonth = HitTest(point);
		if (pMonth)
			pMonth->OnMouseMove(nFlags, point);
	}

	ProcessButtons(point);


	// standard processing
	CWnd::OnMouseMove(nFlags, point);

	TRACKMOUSEEVENT tre;
	tre.dwFlags = TME_LEAVE;
	tre.hwndTrack = this->m_hWnd;
	tre.dwHoverTime = HOVER_DEFAULT;
	tre.cbSize = sizeof(tre);

	_TrackMouseEvent(&tre);

	// fire mouse move
	DWORD dwPoint = MAKELONG(point.x, point.y);
	SendNotification(XTP_NC_DATEPICKERMOUSEMOVE, dwPoint);
}

BOOL CXTPDatePickerControl::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	WNDCLASS wndcls;
	if (hInstance == NULL) hInstance = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInstance, XTP_DATEPICKERCTRL_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInstance;
		wndcls.hIcon = NULL;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = XTP_DATEPICKERCTRL_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CXTPDatePickerControl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	if (!CWnd::Create(XTP_DATEPICKERCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	return TRUE;
}

void CXTPDatePickerControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CXTPClientRect rc(this);

	CRect rcClient(0, 0, 0, 0);
	this->GetClientRect(&rcClient);

	AdjustLayout(rcClient);
}

void CXTPDatePickerControl::ClearMonths()
{
	// cleanup old month array
	int nOldMonthCount = (int)m_arrMonths.GetSize();
	for (int nMonth = 0; nMonth < nOldMonthCount; nMonth++)
	{
		CXTPDatePickerItemMonth* pMonth = m_arrMonths.GetAt(nMonth);
		pMonth->InternalRelease();
	}
	m_arrMonths.RemoveAll();
}

void CXTPDatePickerControl::CreateMonthArray()
{
	COleDateTime dtNextMonth(m_dtFirstMonth);
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		for (int nCol = 0; nCol < m_nColumns; nCol++)
		{
			//int nIndex = nRow * m_nRows + nCol;
			// create next month item
			CXTPDatePickerItemMonth* pMonth = new CXTPDatePickerItemMonth(this, dtNextMonth, nRow, nCol);

			// add it to the array
			m_arrMonths.Add(pMonth);

			// go to the next month
			if (dtNextMonth.GetMonth() < 12)
				dtNextMonth.SetDate(dtNextMonth.GetYear(), dtNextMonth.GetMonth() + 1, 1);
			else
				dtNextMonth.SetDate(dtNextMonth.GetYear() + 1, 1, 1);
		}
	}

	// set first month and last month defaults
	int nMonthCount = (int)m_arrMonths.GetSize();
	if (nMonthCount > 0)
	{
		CXTPDatePickerItemMonth* pFirstMonth = m_arrMonths.GetAt(0);
		CXTPDatePickerItemMonth* pLastMonth = m_arrMonths.GetAt(nMonthCount - 1);
		// by default set show previous days for first month and show following days for last month
		pFirstMonth->SetShowDaysBefore(m_bShowNonMonthDays);
		pLastMonth->SetShowDaysAfter(m_bShowNonMonthDays);
		// set triangles showing
		pFirstMonth->SetShowScrolling(TRUE, pFirstMonth->GetShowRightScroll());
		CXTPDatePickerItemMonth* pLastMonthTopRow = m_arrMonths.GetAt(m_nColumns - 1);
		pLastMonthTopRow->SetShowScrolling(pLastMonthTopRow->GetShowLeftScroll(), TRUE);
	}

}

void CXTPDatePickerControl::Populate()
{
	// cleanup old month array
	ClearMonths();

	// add all month items in the grid to the array
	// and set first month and last month defaults
	CreateMonthArray();

	// redraw control image
	if (!m_rcControl.IsRectEmpty())
		AdjustLayout(m_rcControl);
	_RedrawControl(FALSE);

	SendNotification(XTP_NC_DATEPICKERMONTHCHANGED);
	SendMessageToParent(XTP_NC_DATEPICKERMONTHCHANGED);
}

void CXTPDatePickerControl::GetDayMetrics(COleDateTime& dtDay, XTP_DAYITEM_METRICS* pDayMetics)
{
	WPARAM wprmDay = (XTP_DATE_VALUE)(DATE)dtDay;
	SendNotification(XTP_NC_DATEPICKERGETDAYMETRICS, wprmDay, (LPARAM)pDayMetics);

	if (m_pfnCallback)
	{
		try
		{
			m_pfnCallback(this, dtDay, pDayMetics, m_pCallbackParam);
		}
		catch(...)
		{
			ASSERT(FALSE);
		}
	}
}

void CXTPDatePickerControl::SetGridSize(int nRows, int nCols)
{
	m_nRows = nRows;
	m_nColumns = nCols;
	Populate();
}

void CXTPDatePickerControl::SetHighlightToday(BOOL bValue)
{
	m_bHighlightToday = bValue;
	_RedrawControl(FALSE);
}

void CXTPDatePickerControl::SetShowWeekNumbers(BOOL bValue)
{
	m_bShowWeekNumbers = bValue;
	AdjustLayout(m_rcControl);
	_RedrawControl(FALSE);
}

CSize CXTPDatePickerControl::CalcButtonSize() const
{
	// get button size
	CWindowDC dc(GetDesktopWindow());
	CXTPFontDC fnt(&dc, m_pPaintManager->GetButtonFont());

	CSize szButton(0, 0);

	for (int i = 0; i < GetButtonCount(); i++)
	{
		CSize sz = dc.GetTextExtent(GetButton(i)->GetCaption());

		szButton.cx = max(szButton.cx, sz.cx + 12);
		szButton.cy = max(szButton.cy, sz.cy + 6);
	}

	return szButton;
}

void CXTPDatePickerControl::SetButtonRect()
{
	CSize szButton(CalcButtonSize());

	int nGap = 10;

	int nButtonLen = szButton.cx + nGap;
	int nVisibleCount = m_arrButtons.GetVisibleButtonCount();

	int nLeft = m_rcGrid.CenterPoint().x - (nButtonLen * nVisibleCount - nGap)/2;

	for (int i = 0; i < GetButtonCount(); i++)
	{
		CXTPDatePickerButton* pButton = GetButton(i);
		if (!pButton->m_bVisible) continue;

		pButton->m_rcButton = CRect(CPoint(nLeft, m_rcGrid.bottom + 1), szButton);
		nLeft += nButtonLen;
	}
}

void CXTPDatePickerControl::CalcButtonBandRect()
{
	SetButtonRect();
}

void CXTPDatePickerControl::DrawButtons(CDC* pDC)
{
	for (int i = 0; i < GetButtonCount(); i++)
	{
		CXTPDatePickerButton* pButton = GetButton(i);
		if (pButton->m_bVisible)
			m_pPaintManager->DrawButton(pDC, pButton->m_rcButton, pButton->GetCaption(), pButton->m_bPressed, pButton->m_bHighlight);
	}
}

CXTPDatePickerItemMonth* CXTPDatePickerControl::HitTest(CPoint ptMouse)
{
	// enumerate all month items in the collection
	int nMonthCount = (int)m_arrMonths.GetSize();
	for (int nIndex = 0; nIndex < nMonthCount; nIndex++)
	{
		CXTPDatePickerItemMonth* pMonth = m_arrMonths.GetAt(nIndex);
		if (pMonth && pMonth->m_rcMonth.PtInRect(ptMouse))
			return pMonth;
	}
	return NULL;
}

void CXTPDatePickerControl::ScrollLeft(int nMonthCount)
{
	int nYearNew = m_dtFirstMonth.GetYear() - nMonthCount / 12;
	int nMonthNew = m_dtFirstMonth.GetMonth() - nMonthCount % 12;
	if (nMonthNew < 1)
	{
		nMonthNew += 12;
		nYearNew--;
	}
	ASSERT(nMonthNew >= 1 && nMonthNew <= 12);

	if (m_dtMinRange.GetStatus() == COleDateTime::valid)
	{
		if (nYearNew < m_dtMinRange.GetYear())
		{
			nYearNew = m_dtMinRange.GetYear();
			nMonthNew = m_dtMinRange.GetMonth();
		}

		if (m_dtMinRange.GetYear() == nYearNew)
		{
			if (nMonthNew < m_dtMinRange.GetMonth())
				nMonthNew = m_dtMinRange.GetMonth();
		}
	}

	m_dtFirstMonth.SetDate(nYearNew, nMonthNew, 1);

	Populate();
}

void CXTPDatePickerControl::ScrollRight(int nMonthCount)
{
	int nYearNew = m_dtFirstMonth.GetYear() + nMonthCount / 12;
	int nMonthNew = m_dtFirstMonth.GetMonth() + nMonthCount % 12;
	if (nMonthNew > 12)
	{
		nMonthNew -= 12;
		nYearNew++;
	}

	if (m_dtMaxRange.GetStatus() == COleDateTime::valid)
	{
		int nLeftMonth = m_dtMaxRange.GetMonth() - m_nRows * m_nColumns + 1;
		int nLeftYear = m_dtMaxRange.GetYear();
		if (nLeftMonth < 1)
		{
			nLeftYear -= (-nLeftMonth) / 12 + 1;
			nLeftMonth = -(((-nLeftMonth) % 12) - 12);
		}

		if (nYearNew > nLeftYear)
		{
			nYearNew = nLeftYear;
			nMonthNew = nLeftMonth;
		}

		if (nLeftYear == nYearNew)
		{
			if (nMonthNew > nLeftMonth)
				nMonthNew = nLeftMonth;
		}
	}

	m_dtFirstMonth.SetDate(nYearNew, nMonthNew, 1);

	ScrollLeft(0); // adjust also scrolling left before calling Populate();
}


void CXTPDatePickerControl::OnTimeChange()
{
	CWnd::OnTimeChange();
	m_dtToday = CXTPCalendarUtils::GetCurrentTime();
	_RedrawControl(FALSE);
}

void CXTPDatePickerControl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == XTP_DATEPICKER_TIMERID)
	{
		if (m_mouseMode == mouseScrollingLeft)
			ScrollLeft(GetMonthDelta());
		else if (m_mouseMode == mouseScrollingRight)
			ScrollRight(GetMonthDelta());

		if ((m_mouseMode == mouseTrackingHeaderList) &&
			(m_pListControl != NULL))
		{
			m_pListControl->OnTimer(nIDEvent);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CXTPDatePickerControl::ShowListHeader(CRect rcHeader, COleDateTime dtMonth)
{
	// make sure that the list is not already created
	ASSERT(!m_pListControl);
	if (m_pListControl)
	{
		m_pListControl->DestroyWindow();
		delete m_pListControl;
		m_pListControl = NULL;
	}

	if (!m_bIsModal)
		SetCapture();

	// create list
	m_pListControl = new CXTPDatePickerList(this, dtMonth);
	if (!m_pListControl)
		throw (new CMemoryException());

	// create control
	m_pListControl->Create(rcHeader);

	m_nTimerID = (UINT)SetTimer(XTP_DATEPICKER_TIMERID, 2 * XTP_DATEPICKER_TIMER_INTERVAL / 3, NULL);

	m_mouseMode = mouseTrackingHeaderList;
}

void CXTPDatePickerControl::Select(const COleDateTime& dtDay)
{
	ClearFocus();
	if (m_nMaxSelectionDays == XTP_SELECTION_INFINITE ||
		m_nMaxSelectionDays > m_pSelectedDays->GetSelectedDaysCount())
	{
		m_pSelectedDays->Add(dtDay);
	}
}

void CXTPDatePickerControl::Deselect(const COleDateTime& dtDay)
{
	ClearFocus();
	m_pSelectedDays->Remove(dtDay);
}

BOOL CXTPDatePickerControl::IsSelected(const COleDateTime& dtDay) const
{
	BOOL bSelected = m_pSelectedDays->Contains(dtDay);

	if (m_mouseMode == mouseSelecting ||
		m_mouseMode == mouseDeselecting)
	{
		BOOL bBetween = FALSE;
		if (m_dtFirstClicked <= m_dtLastClicked &&
			m_dtFirstClicked <= dtDay && m_dtLastClicked >= dtDay)
		{
			bBetween = TRUE;
		}
		if (m_dtFirstClicked >= m_dtLastClicked &&
			m_dtFirstClicked >= dtDay && m_dtLastClicked <= dtDay)
		{
			bBetween = TRUE;
		}
		if (bBetween && m_mouseMode == mouseSelecting)
		{
			COleDateTimeSpan spDays = dtDay - m_dtFirstClicked;
			int nDays = abs(spDays.GetDays());
			if (m_nMaxSelectionDays == XTP_SELECTION_INFINITE ||
				m_pSelectedDays->GetSelectedDaysCount() + nDays < m_nMaxSelectionDays)
			{
				bSelected = TRUE;
			}
		}
		if (bBetween && m_mouseMode == mouseDeselecting)
			bSelected = FALSE;
	}
	return bSelected;
}

BOOL CXTPDatePickerControl::IsFocused(const COleDateTime& dtDay) const
{
	if (m_dtFocused.GetStatus() != COleDateTime::valid)
	{
		return FALSE;
	}

	return CXTPCalendarUtils::IsEqual(CXTPCalendarUtils::ResetTime(m_dtFocused),
									  CXTPCalendarUtils::ResetTime(dtDay));
}

void CXTPDatePickerControl::ClearFocus()
{
	CXTPSelectionHelper _selector(this);
	_selector.RemoveFocus();
}

void CXTPDatePickerControl::OnButtonClick(UINT nID)
{
	XTP_NC_DATEPICKER_BUTTON nm;
	nm.nID = nID;

	SendNotification(XTP_NC_DATEPICKERBUTTONCLICKED, nID);
	SendMessageToParent(XTP_NC_DATEPICKER_BUTTON_CLICK, (NMHDR*)&nm);

	switch (nID)
	{
		case XTP_IDS_DATEPICKER_TODAY:
			{
				// behavior: change selection to today
				ClearFocus();
				m_pSelectedDays->Clear();
				COleDateTime dt;
				GetToday(dt);
				EnsureVisible(dt);
				Select(dt);
				SendNotification(XTP_NC_DATEPICKERSELECTIONCHANGED);
				SendMessageToParent(XTP_NC_DATEPICKER_SELECTION_CHANGED);
				_EndModalIfNeed();
			}
			break;

		case XTP_IDS_DATEPICKER_NONE:
			{
				// behavior: change selection
				ClearFocus();
				m_pSelectedDays->Clear();
				SendNotification(XTP_NC_DATEPICKERSELECTIONCHANGED);
				SendMessageToParent(XTP_NC_DATEPICKER_SELECTION_CHANGED);
				_EndModalIfNeed();
			}
			break;
	}
}

void CXTPDatePickerControl::ProcessButtons(CPoint point)
{
	for (int i = 0; i < GetButtonCount(); i++)
	{
		CXTPDatePickerButton* pButton = GetButton(i);
		BOOL bHighlight = pButton->m_rcButton.PtInRect(point);

		if (pButton == m_pButtonCaptured)
		{
			if (bHighlight != pButton->m_bPressed)
			{
				pButton->m_bPressed = bHighlight;
				_RedrawControl(FALSE);
			}
		}
		else if (pButton->m_bPressed)
		{
			pButton->m_bPressed = FALSE;
			 if (bHighlight)
				 OnButtonClick(pButton->m_nID);
			_RedrawControl(FALSE);
		}

		bHighlight = pButton->m_bHighlight;

		pButton->m_bHighlight = ((m_pButtonCaptured == pButton) ||
			(!m_pButtonCaptured && pButton->m_rcButton.PtInRect(point)));

		if (pButton->m_bHighlight != bHighlight)
		{
			_RedrawControl(FALSE);
		}
	}
}


LRESULT CXTPDatePickerControl::SendMessageToParent(int nMessage, NMHDR* pNMHDR)
{
	if (!IsWindow(m_hWnd))
		return 0;

	NMHDR nmhdr;
	if (pNMHDR == NULL)
		pNMHDR = &nmhdr;

	pNMHDR->hwndFrom = GetSafeHwnd();
	pNMHDR->idFrom = GetDlgCtrlID();
	pNMHDR->code = nMessage;

	CWnd *pOwner = GetOwner();
	LRESULT res = 0;
	if (pOwner && IsWindow(pOwner->m_hWnd))
		res = pOwner->SendMessage(WM_NOTIFY, pNMHDR->idFrom, (LPARAM)pNMHDR);

	return res;
}

void CXTPDatePickerControl::_EndModalIfNeed()
{
	BOOL bSingleDaySel = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	if (m_bIsModal && !bSingleDaySel)
	{
		if (ContinueModal()) // skip if EndModalLoop has already called
		{
			EndModalLoop(IDOK);
		}
	}

}

void CXTPDatePickerControl::SetButtonsVisible(BOOL bShowToday, BOOL bShowNone)
{
	m_arrButtons.Find(XTP_IDS_DATEPICKER_TODAY)->m_bVisible = bShowToday;
	m_arrButtons.Find(XTP_IDS_DATEPICKER_NONE)->m_bVisible = bShowNone;

	if (!m_rcControl.IsRectEmpty())
	{
		AdjustLayout(m_rcControl);
		_RedrawControl(FALSE);
	}
}

void CXTPDatePickerControl::EnsureVisible(const COleDateTime& dtDate)
{
	int nYear = dtDate.GetYear();
	int nMonth = dtDate.GetMonth();
	// enumerate all month items in the collection
	int nMonthCount = (int)m_arrMonths.GetSize();
	for (int nIndex = 0; nIndex < nMonthCount; nIndex++)
	{
		CXTPDatePickerItemMonth* pMonth = m_arrMonths.GetAt(nIndex);
		if (pMonth)
		{
			if (pMonth->GetMonth().GetMonth() == nMonth && pMonth->GetMonth().GetYear() == nYear)
				return;
		}
	}
	m_dtFirstMonth.SetDate(nYear, nMonth, 1);
	Populate();
}

AFX_INLINE int GetEqualLeftSymbols(const CString& str1, const CString& str2)
{
	int nCount = min(str1.GetLength(), str2.GetLength());

	for (int i = 0; i < nCount; i++)
	{
		if (str1.GetAt(i) != str2.GetAt(i))
			return i;
	}
	return nCount;
}

void CXTPDatePickerControl::InitNames()
{
	// initialize month names
	for (int nMonth = 0; nMonth < 12; nMonth++)
	{
		CString strMonth = CXTPCalendarUtils::GetLocaleString(LOCALE_SMONTHNAME1 + nMonth, 255) ;

		m_arMonthNames[nMonth] = strMonth;
	}

	int nEqualLeftSymbols = 255, nDay;

	// initialize day names
	for (nDay = 0; nDay < 7; nDay++)
	{
		CString strDayName = CXTPCalendarUtils::GetLocaleString(LOCALE_SABBREVDAYNAME1 + nDay, 255);

		int nOleDayOfWeek = (nDay + 1) % 7;
		m_arDayOfWeekNames[nOleDayOfWeek] = strDayName;

		if (nDay > 0 && nEqualLeftSymbols > 0)
		{
			nEqualLeftSymbols = min(nEqualLeftSymbols, GetEqualLeftSymbols(strDayName, m_arDayOfWeekNames[1]));
		}
	}

	// If first symbols equal, remove them.
	if (nEqualLeftSymbols > 0)
	{
		for (nDay = 0; nDay < 7; nDay++)
		{
			DELETE_S(m_arDayOfWeekNames[nDay], 0, nEqualLeftSymbols);
		}
	}
}

void CXTPDatePickerControl::SetShowNonMonthDays(BOOL bShow)
{
	if (bShow != m_bShowNonMonthDays)
	{
		m_bShowNonMonthDays = bShow;
		// set first month and last month defaults
		int nMonthCount = (int)m_arrMonths.GetSize();
		if (nMonthCount > 0)
		{
			CXTPDatePickerItemMonth* pFirstMonth = m_arrMonths.GetAt(0);
			CXTPDatePickerItemMonth* pLastMonth = m_arrMonths.GetAt(nMonthCount - 1);
			// by default set show previous days for first month and show following days for last month
			pFirstMonth->SetShowDaysBefore(bShow);
			pLastMonth->SetShowDaysAfter(bShow);
		}
		_RedrawControl(FALSE);
	}
}

void CXTPDatePickerControl::AllowNoncontinuousSelection(BOOL bAllow /*= TRUE*/)
{
	m_bAllowNoncontinuousSelection = bAllow;
	if (!m_bAllowNoncontinuousSelection && m_pSelectedDays->GetSelectedBlocksCount() > 1)
	{
		m_pSelectedDays->Clear();
		_RedrawControl(FALSE);
	}
}


void CXTPDatePickerControl::SetMaxSelCount(int nMax)
{
	m_nMaxSelectionDays = nMax;
	int nCurrentSelectedDays = m_pSelectedDays->GetSelectedDaysCount();

	// clear extra days
	if (nCurrentSelectedDays > m_nMaxSelectionDays)
	{
		m_pSelectedDays->Clear();
		_RedrawControl(FALSE);
	}
}

BOOL CXTPDatePickerControl::GetSelRange(COleDateTime& refMinRange, COleDateTime& refMaxRange) const
{
	return m_pSelectedDays->GetMinMaxRange(refMinRange, refMaxRange);
}

BOOL CXTPDatePickerControl::SetSelRange(const COleDateTime& dtMinRange, const COleDateTime& dtMaxRange)
{
	ClearFocus();
	m_pSelectedDays->SelectRange(dtMinRange, dtMaxRange);
	m_dtFirstClicked = dtMinRange;
	m_dtLastClicked = dtMaxRange;

	return TRUE;
}

BOOL CXTPDatePickerControl::SizeMinReq(BOOL bRepaint /* = TRUE */)
{
	CRect rect;
	BOOL bRetVal = FALSE;
	if (GetMinReqRect(rect))
	{
		DWORD dwFlags = SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOMOVE | SWP_NOACTIVATE;
		if (!bRepaint)
			dwFlags |= SWP_NOREDRAW;
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), dwFlags);
		bRetVal = TRUE;
	}

	return bRetVal;
}

BOOL CXTPDatePickerControl::GetMinReqRect(RECT* pRect) const
{
	return GetMinReqRect(pRect, 1, 1);
}


BOOL CXTPDatePickerControl::GetMinReqRect(RECT* pRect, int nRows, int nCols) const
{
	CWindowDC dc(GetDesktopWindow());
	CSize szMonth(m_pPaintManager->CalcMonthRect(&dc));

	pRect->left = pRect->top = 0;
	pRect->right = szMonth.cx * nCols;
	pRect->bottom = szMonth.cy * nRows;

	CRect rcBorder(0, 0, 0, 0);
	m_pPaintManager->DrawBorder(0, this, rcBorder, FALSE);

	pRect->right += rcBorder.left - rcBorder.right;
	pRect->bottom += rcBorder.top - rcBorder.bottom;

	if (m_arrButtons.GetVisibleButtonCount() > 0)
	{
		CSize szButton(CalcButtonSize());
		pRect->bottom += szButton.cy;
	}

	return TRUE;
}

BOOL CXTPDatePickerControl::GetCurSel(COleDateTime& refDateTime) const
{
	return m_pSelectedDays->GetMinMaxRange(refDateTime, refDateTime);
}

BOOL CXTPDatePickerControl::SetCurSel(const COleDateTime& refDateTime)
{
	return SetSelRange(refDateTime, refDateTime);
}

void CXTPDatePickerControl::SetToday(const COleDateTime& refDateTime)
{
	m_dtToday = refDateTime;
	_RedrawControl(FALSE);
}

DWORD CXTPDatePickerControl::GetRange(COleDateTime* pMinRange, COleDateTime* pMaxRange) const
{
	DWORD dwRes = 0;
	if (pMinRange && m_dtMinRange.GetStatus() == COleDateTime::valid)
	{
		*pMinRange = m_dtMinRange;
		dwRes |= GDTR_MIN;
	}
	if (pMaxRange && m_dtMaxRange.GetStatus() == COleDateTime::valid)
	{
		*pMaxRange = m_dtMaxRange;
		dwRes |= GDTR_MAX;
	}
	return dwRes;
}

BOOL CXTPDatePickerControl::SetRange(const COleDateTime* pMinRange, const COleDateTime* pMaxRange)
{
	if (pMinRange)
	{
		m_dtMinRange = *pMinRange;
		ScrollLeft(0);
	}
	if (pMaxRange)
	{
		m_dtMaxRange = *pMaxRange;
		ScrollRight(0);
	}
	return TRUE;
}

int CXTPDatePickerControl::GetMonthRange(COleDateTime& refMinRange, COleDateTime& refMaxRange, DWORD dwFlags) const
{
	refMinRange = m_dtFirstMonth;
	refMaxRange = m_dtFirstMonth;
	int nMonthCount = m_nRows * m_nColumns;
	ShiftDate(refMaxRange, nMonthCount);

	//if (GMR_DAYSTATE == dwFlags)
		// do nothing

	if (GMR_VISIBLE == dwFlags)
	{
		nMonthCount = (int)m_arrMonths.GetSize();
		if (nMonthCount > 0)
		{
			CXTPDatePickerItemMonth* pLastMonth = m_arrMonths.GetAt(nMonthCount - 1);
			if (pLastMonth)
			{
				CXTPDatePickerItemDay* pLastDay = pLastMonth->m_arrDays.GetAt(XTP_MAX_WEEKS * XTP_WEEK_DAYS - 1);
				if (pLastDay->GetDate().GetMonth() != refMaxRange.GetMonth())
				{
					ShiftDate(refMaxRange, 1);
					nMonthCount++;
				}
			}

			CXTPDatePickerItemMonth* pFirstMonth = m_arrMonths.GetAt(0);
			if (pFirstMonth)
			{
				CXTPDatePickerItemDay* pFirstDay = pFirstMonth->m_arrDays.GetAt(0);
				if (pFirstDay->GetDate().GetMonth() != refMinRange.GetMonth())
				{
					ShiftDate(refMinRange, -1);
					nMonthCount++;
				}
			}
		}
	}
	return nMonthCount;
}

BOOL CXTPDatePickerControl::ShiftDate(COleDateTime &refDate, int nMonthCount)
{
	int nYearNew = refDate.GetYear();
	int nMonthNew = refDate.GetMonth();
	int nInc = abs(nMonthCount) / nMonthCount;
	for (int nItem = 0; nItem < abs(nMonthCount); nItem++)
	{
		nMonthNew += nInc;
		if (nMonthNew < 1)
		{
			nMonthNew = 12;
			nYearNew--;
		}
		if (nMonthNew > 12)
		{
			nMonthNew = 1;
			nYearNew++;
		}
	}
	return 0 == refDate.SetDate(nYearNew, nMonthNew, 1);
}

void XTPGetAsSystemTime(const COleDateTime& dateTime, SYSTEMTIME& st)
{
#if _MSC_VER < 1200
	UNREFERENCED_PARAMETER(dateTime);
	UNREFERENCED_PARAMETER(st);
	ASSERT(FALSE);
#else
	dateTime.GetAsSystemTime(st);
#endif
}

DWORD CXTPDatePickerControl::HitTest(PMCHITTESTINFO pMCHitTest)
{
	// check structure
	if (!pMCHitTest)
		return 0;

	ASSERT(sizeof(MCHITTESTINFO) == pMCHitTest->cbSize);

	// default result
	pMCHitTest->uHit = MCHT_NOWHERE;

	// start checking
	CPoint ptHit(pMCHitTest->pt);

	CXTPDatePickerItemMonth* pHitMonth = HitTest(ptHit);
	if (pHitMonth)
	{
		CXTPDatePickerItemDay* pHitDay = pHitMonth->HitTest(ptHit);
		if (pHitDay)
		{
			// MCHT_CALENDARDATE
			pMCHitTest->uHit = MCHT_CALENDARDATE;
			XTPGetAsSystemTime(pHitDay->GetDate(), pMCHitTest->st);

			// MCHT_CALENDARDATEPREV
			if (pHitMonth->GetShowDaysBefore() &&
				pHitMonth->GetMonth().GetMonth() < pHitDay->GetDate().GetMonth())
			{
				pMCHitTest->uHit = MCHT_CALENDARDATEPREV;
			}
			else
			// MCHT_CALENDARDATENEXT
			if (pHitMonth->GetShowDaysAfter() &&
				pHitMonth->GetMonth().GetMonth() > pHitDay->GetDate().GetMonth())
			{
				pMCHitTest->uHit = MCHT_CALENDARDATENEXT;
			}
		}
		else
		{
			XTPGetAsSystemTime(pHitMonth->GetMonth(), pMCHitTest->st);

			// MCHT_CALENDARDAY
			if (pHitMonth->m_rcDaysOfWeek.PtInRect(ptHit))
			{
				pMCHitTest->uHit = MCHT_CALENDARDAY;
				// The SYSTEMTIME structure at lpMCHitTest>st is set to the corresponding date in the top row.
				CPoint ptTopRow(ptHit.x, pHitMonth->m_rcDaysOfWeek.bottom + 1);
				CXTPDatePickerItemDay* pDay = pHitMonth->HitTest(ptTopRow);
				if (pDay)
				{
					XTPGetAsSystemTime(pDay->GetDate(), pMCHitTest->st);
				}
			}
			else
			// MCHT_CALENDARWEEKNUM
			if (pHitMonth->m_rcWeekNumbers.PtInRect(ptHit))
			{
				pMCHitTest->uHit = MCHT_CALENDARWEEKNUM;
				// The SYSTEMTIME structure at lpMCHitTest>st is set to the corresponding date in the leftmost column
				CPoint ptLeftRow(pHitMonth->m_rcWeekNumbers.right + 1, ptHit.y);
				CXTPDatePickerItemDay* pDay = pHitMonth->HitTest(ptLeftRow);
				if (pDay)
				{
					XTPGetAsSystemTime(pDay->GetDate(), pMCHitTest->st);
				}
			}
			else
			// MCHT_TITLEMONTH
			if (pHitMonth->m_rcHeader.PtInRect(ptHit))
			{
				pMCHitTest->uHit = MCHT_TITLEMONTH;
			}
			else
			// MCHT_TITLEBTNNEXT
			if (pHitMonth->GetShowRightScroll() &&
				pHitMonth->m_rcRightScroll.PtInRect(ptHit))
			{
				pMCHitTest->uHit = MCHT_TITLEBTNNEXT;
			}
			else
			// MCHT_TITLEBTNPREV
			if (pHitMonth->GetShowLeftScroll() &&
				pHitMonth->m_rcLeftScroll.PtInRect(ptHit))
			{
				pMCHitTest->uHit = MCHT_TITLEBTNPREV;
			}
		}
	}
	else
	{
		// MCHT_CALENDARBK
		pMCHitTest->uHit = MCHT_CALENDARBK;
	}

	return pMCHitTest->uHit;
}

void CXTPDatePickerControl::SetBorderStyle(XTPDatePickerBorderStyle borderStyle)
{
	m_borderStyle = borderStyle;
	AdjustLayout(m_rcControl);
	_RedrawControl(FALSE);
}

void CXTPDatePickerControl::SetAutoSize(BOOL bAuto)
{
	m_bAutoSize = bAuto;
	AdjustLayout(m_rcControl);
	_RedrawControl(FALSE);
}

void CXTPDatePickerControl::OnSysColorChange()
{
	m_pPaintManager->RefreshMetrics();
	AdjustLayout(m_rcControl);
	_RedrawControl(FALSE);
}

BOOL CXTPDatePickerControl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;
		::GetCursorPos(&point);
		ScreenToClient(&point);
		CXTPDatePickerItemMonth* pMonth = HitTest(point);
		if (pMonth)
		{
			BOOL bRet = pMonth->OnSetCursor(point);
			if (bRet)
				return bRet;
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CXTPDatePickerControl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	RedrawControl();

#ifdef XTP_DATEPICKER_SITENOTIFY_ONFOCUS
	XTP_DATEPICKER_SITENOTIFY_ONFOCUS(this, this, TRUE)
#endif
}

void CXTPDatePickerControl::OnKillFocus (CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	RedrawControl();

#ifdef XTP_DATEPICKER_SITENOTIFY_ONFOCUS
	XTP_DATEPICKER_SITENOTIFY_ONFOCUS(this, this, FALSE)
#endif
}

void CXTPDatePickerControl::OnFinalRelease()
{
	CWnd::OnFinalRelease();

	if (m_bDeleteOnFinalRelease)
	{
		CCmdTarget::OnFinalRelease();
	}
}

void CXTPDatePickerControl::OnEnable(BOOL bEnable)
{
	UNREFERENCED_PARAMETER(bEnable);

	_RedrawControl(FALSE);
}

BOOL CXTPDatePickerControl::GetVisibleRange(COleDateTime& refFirstVisibleDay,
											COleDateTime& refLastVisibleDay) const
{
	int nCount = (int)m_arrMonths.GetSize();
	if (!nCount || !m_arrMonths[0] || !m_arrMonths[nCount-1])
	{
		return FALSE;
	}
	if (!m_arrMonths[0]->GetDay(0))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	refFirstVisibleDay = m_arrMonths[0]->GetDay(0)->GetDate();

	if (!m_arrMonths[0]->GetShowDaysBefore())
	{
		int nFirstMnDCount = m_arrMonths[0]->GetDayCount();
		for (int i = 1; i < nFirstMnDCount; i++)
		{
			if (refFirstVisibleDay.GetMonth() == m_arrMonths[0]->GetMonth().GetMonth())
			{
				break;
			}
			refFirstVisibleDay = m_arrMonths[0]->GetDay(i)->GetDate();
		}
		ASSERT(refFirstVisibleDay.GetMonth() == m_arrMonths[0]->GetMonth().GetMonth());
	}

	int nLastMnDCount = m_arrMonths[nCount-1]->GetDayCount();
	if (!nLastMnDCount || !m_arrMonths[nCount-1]->GetDay(nLastMnDCount-1))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	refLastVisibleDay = m_arrMonths[nCount-1]->GetDay(nLastMnDCount-1)->GetDate();

	if (!m_arrMonths[nCount-1]->GetShowDaysAfter())
	{
		for (int i = nLastMnDCount-2; i > 0 ; i--)
		{
			if (refLastVisibleDay.GetMonth() == m_arrMonths[nCount-1]->GetMonth().GetMonth())
			{
				break;
			}
			refLastVisibleDay = m_arrMonths[nCount-1]->GetDay(i)->GetDate();
		}
		ASSERT(refLastVisibleDay.GetMonth() == m_arrMonths[nCount-1]->GetMonth().GetMonth());
	}

	return TRUE;
}

void CXTPDatePickerControl::EnsureVisibleSelection()
{
	COleDateTime dtFirstSelDay, dtLastSelDay;
	COleDateTime dtFirstVisibleDay, dtLastVisibleDay;

	BOOL bRes1 = GetVisibleRange(dtFirstVisibleDay, dtLastVisibleDay);
	BOOL bRes2 = GetSelRange(dtFirstSelDay, dtLastSelDay);
	if (bRes1 && bRes2)
	{
		if (dtFirstSelDay > dtLastVisibleDay)
		{
			int nMonths = CXTPCalendarUtils::GetDiff_Months(dtFirstSelDay, dtLastVisibleDay);
			nMonths += CXTPCalendarUtils::GetDiff_Months(dtLastSelDay, dtFirstSelDay);
			nMonths = max(nMonths, 1);
			ScrollRight(nMonths);
		}
		else if (dtLastSelDay > dtLastVisibleDay)
		{
			int nMonths = CXTPCalendarUtils::GetDiff_Months(dtLastSelDay, dtLastVisibleDay);
			nMonths = max(nMonths, 1);
			ScrollRight(nMonths);
		}

		//---------------------------
		bRes1 = GetVisibleRange(dtFirstVisibleDay, dtLastVisibleDay);
		bRes2 = GetSelRange(dtFirstSelDay, dtLastSelDay);
		if (bRes1 && bRes2)
		{
			if (dtFirstSelDay < dtFirstVisibleDay || dtLastSelDay > dtLastVisibleDay)
			{
				EnsureVisible(dtLastSelDay);
				EnsureVisible(dtFirstSelDay);
			}
		}
	}
}

void CXTPDatePickerControl::SetLayoutRTL(BOOL bRightToLeft)
{
	if (!XTPSystemVersion()->IsLayoutRTLSupported())
		return;

	m_bRightToLeft = bRightToLeft;

	if (!m_hWnd)
		return;

	ModifyStyleEx(bRightToLeft ? 0 : WS_EX_LAYOUTRTL, !bRightToLeft ? 0 : WS_EX_LAYOUTRTL);
	RedrawControl();
}



void CXTPDatePickerControl::EnsureVisibleFocus()
{
	if (m_dtFocused.GetStatus() != COleDateTime::valid)
	{
		return;
	}
	COleDateTime dtFirstVisibleDay, dtLastVisibleDay;

	BOOL bRes1 = GetVisibleRange(dtFirstVisibleDay, dtLastVisibleDay);
	if (bRes1)
	{
		if (m_dtFocused > dtLastVisibleDay)
		{
			int nMonths = CXTPCalendarUtils::GetDiff_Months(m_dtFocused, dtLastVisibleDay);
			nMonths = max(nMonths, 1);
			ScrollRight(nMonths);
		}

		//---------------------------
		bRes1 = GetVisibleRange(dtFirstVisibleDay, dtLastVisibleDay);
		if (bRes1)
		{
			if (m_dtFocused < dtFirstVisibleDay || m_dtFocused > dtLastVisibleDay)
			{
				EnsureVisible(m_dtFocused);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//class CXTPSelectionHelper
CXTPDatePickerControl::CXTPSelectionHelper::CXTPSelectionHelper(CXTPDatePickerControl* pControl)
{
	ASSERT(pControl);
	m_pDP = pControl;
}

void CXTPDatePickerControl::CXTPSelectionHelper::InitFocusIfNeed()
{
	if (m_pDP->m_dtFocused.GetStatus() != COleDateTime::valid)
	{
		COleDateTime dtSel0, dtSel1;
		BOOL bSelExists = m_pDP->GetSelectedDays()->GetMinMaxRange(dtSel0, dtSel1);
		if (!bSelExists)
		{
			dtSel0.SetDate(m_pDP->m_dtFirstMonth.GetYear(), m_pDP->m_dtFirstMonth.GetMonth(), 1);
		}
		m_pDP->m_dtFocused = dtSel0;

		if (m_pDP->m_dtFirstClicked.GetStatus() == COleDateTime::valid &&
			m_pDP->m_dtFirstClicked >= dtSel0 && m_pDP->m_dtFirstClicked <= dtSel1)
		{
			m_pDP->m_dtFSelBase = m_pDP->m_dtFirstClicked;
		}
		else
		{
			m_pDP->m_dtFSelBase = dtSel0;
		}
	}
}

void CXTPDatePickerControl::CXTPSelectionHelper::RemoveFocus()
{
	m_pDP->m_dtFocused.SetStatus(COleDateTime::null);
	m_pDP->m_dtFSelBase.SetStatus(COleDateTime::null);
}


void CXTPDatePickerControl::CXTPSelectionHelper::_TmpSaveFocus()
{
	m_dtFocusedTmp = m_pDP->m_dtFocused;
	m_dtFSelBaseTmp = m_pDP->m_dtFSelBase;
}

void CXTPDatePickerControl::CXTPSelectionHelper::_TmpRestoreFocus()
{
	m_pDP->m_dtFocused = m_dtFocusedTmp;
	m_pDP->m_dtFSelBase = m_dtFSelBaseTmp;
}

void CXTPDatePickerControl::CXTPSelectionHelper::MoveFocus(int eStep, int eDirection,
									BOOL bContinuouse, BOOL bSaveSel)
{
	ASSERT(eDirection == 1 || eDirection == -1);
	if (bContinuouse)
	{
		bSaveSel = FALSE;
	}

	InitFocusIfNeed();

	_MoveFocus(eStep, eDirection);

	if (bContinuouse)
	{
		COleDateTime dtSel0 = m_pDP->m_dtFocused;
		COleDateTime dtSel1 = m_pDP->m_dtFSelBase;

		BOOL bBackSel = dtSel0 > dtSel1;

		if (bBackSel)
		{
			dtSel1 = m_pDP->m_dtFocused;
			dtSel0 = m_pDP->m_dtFSelBase;
		}

		int nDays = (int)dtSel1 - (int)dtSel0;
		int nMaxSel = m_pDP->GetMaxSelCount();
		if (nMaxSel == XTP_SELECTION_INFINITE)
		{
			nMaxSel = INT_MAX;
		}

		if (nDays > nMaxSel)
		{
			if (bBackSel)
			{
				dtSel1 = (double)dtSel0 + nMaxSel;
			}
			else
			{
				dtSel0 = (double)dtSel1 - nMaxSel;
			}
		}

		m_pDP->GetSelectedDays()->Clear();
		m_pDP->GetSelectedDays()->SelectRange(dtSel0, dtSel1);
	}
	else
	{
		//int nSelDays = m_pDP->GetSelectedDays()->GetSelectedDaysCount();

		if (!bSaveSel)
		{
			m_pDP->GetSelectedDays()->Clear();
		}

		if (!bSaveSel /*|| nSelDays == 0*/)
		{
			m_pDP->GetSelectedDays()->Add(m_pDP->m_dtFocused);
		}

		m_pDP->m_dtFSelBase = m_pDP->m_dtFocused;
	}
}

void CXTPDatePickerControl::CXTPSelectionHelper::SelUnselFocus()
{
	InitFocusIfNeed();

	BOOL bSelected = m_pDP->GetSelectedDays()->Contains(m_pDP->m_dtFocused);

	if (bSelected)
	{
		m_pDP->GetSelectedDays()->Remove(m_pDP->m_dtFocused);
	}
	else
	{
		int nSelDays = m_pDP->GetSelectedDays()->GetSelectedDaysCount();
		int nMaxSel = m_pDP->GetMaxSelCount();
		if (nMaxSel == XTP_SELECTION_INFINITE)
		{
			nMaxSel = INT_MAX;
		}

		if (nSelDays < nMaxSel)
		{
			m_pDP->GetSelectedDays()->Add(m_pDP->m_dtFocused);
		}
	}
}

void CXTPDatePickerControl::CXTPSelectionHelper::_MoveFocus(int eStep, int eDirection)
{
	ASSERT(eDirection == 1 || eDirection == -1);

	InitFocusIfNeed();

	COleDateTime dtNew = m_pDP->m_dtFocused;

	if (eStep == stepDay || eStep == stepWeek)
	{
		dtNew = (double)dtNew + eDirection * (eStep == stepDay ? 1 : 7);
	}
	else if (eStep == stepMonth || eStep == stepYear)
	{
		int nMonths = eDirection * (eStep == stepMonth ? 1 : 12);
		CXTPCalendarUtils::ShiftDate_Month(dtNew, nMonths, dtNew.GetDay());
	}
	else if (eStep == stepWeekBE)
	{
		int nFWD = m_pDP->GetFirstDayOfWeek();

		int nShift = -1 * ((dtNew.GetDayOfWeek() - nFWD + 7) % 7);

		if (eDirection == dirNext)
		{
			nShift += 6;
		}

		dtNew = (double)dtNew + nShift;
	}
	else if (eStep == stepMonthBE)
	{
		CXTPCalendarUtils::UpdateMonthDay(dtNew, eDirection == dirPrev ? 1 : 31);
	}
	else
	{
		ASSERT(FALSE);
	}

	m_pDP->m_dtFocused = dtNew;
}
