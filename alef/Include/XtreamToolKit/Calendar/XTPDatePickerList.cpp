// XTPDatePickerList.cpp: implementation of the CXTPDatePickerList class.
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

#include "Common/XTPDrawHelpers.h"
#include "XTPDatePickerPaintManager.h"
#include "XTPDatePickerItemMonth.h"
#include "XTPDatePickerItemDay.h"
#include "XTPDatePickerControl.h"

#include "XTPDatePickerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerList

CXTPDatePickerList::CXTPDatePickerList(CXTPDatePickerControl* pControl, COleDateTime dtStartMonth)
	: m_pControl(pControl), m_dtStartMonth(dtStartMonth), m_dtSelMonth(dtStartMonth), m_dtMiddleMonth(dtStartMonth)
{
	m_nItemsAbove = 3;
	m_nItemsBelow = 3;

	m_rcListControl.SetRectEmpty();
}

CXTPDatePickerList::~CXTPDatePickerList()
{
}


BEGIN_MESSAGE_MAP(CXTPDatePickerList, CWnd)
	//{{AFX_MSG_MAP(CXTPDatePickerList)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerList message handlers

BOOL CXTPDatePickerList::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPDatePickerList::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

	CXTPClientRect rcClient(this);
	CXTPBufferDC dcMem(dcPaint, rcClient);

	m_dtSelMonth.SetStatus(COleDateTime::null);
	int nMonth = m_dtMiddleMonth.GetMonth();
	int nYear = m_dtMiddleMonth.GetYear();

	ShiftMonthDown(nYear, nMonth, m_nItemsAbove);

	CXTPDatePickerPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// prepare DC
	CFont* pOldFont = dcMem.SelectObject(pPaintManager->GetListControlTextFont());
	int iPosY = 0;

	// fill background
	dcMem.FillSolidRect(rcClient, pPaintManager->GetListControlBackColor());
	dcMem.SetTextColor(pPaintManager->GetListControlTextColor());
	dcMem.SetBkMode(TRANSPARENT);

	CString strDisplayText;
	for (int nItem = 0; nItem < m_nItemsAbove + 1 + m_nItemsBelow; nItem++)
	{
		CRect rcItem;

		if (m_pControl)
			strDisplayText.Format(_T("%s %d"), (LPCTSTR)m_pControl->GetMonthName(nMonth), nYear);

		CSize szItem = dcMem.GetTextExtent(_T(" "), 1);
		szItem.cy += 3;

		rcItem.SetRect(0, iPosY, m_rcListControl.Width(), iPosY + szItem.cy);

		dcMem.DrawText(strDisplayText, rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		if (IsSelected(nItem, rcItem))
		{
			dcMem.InvertRect(rcItem);
			m_dtSelMonth.SetDate(nYear, nMonth, 1);
			m_dtSelMonth.SetStatus(COleDateTime::valid);
		}

		// next month
		ShiftMonthUp(nYear, nMonth, 1);

		iPosY += szItem.cy;
	}

	dcMem.SelectObject(pOldFont);
}

void CXTPDatePickerList::OnMouseMove(UINT /*nFlags*/, CPoint /*point*/)
{
	Invalidate();
}

void CXTPDatePickerList::OnTimer(UINT_PTR /*nIDEvent*/)
{
	// determine cursor position
	DWORD dwPos = ::GetMessagePos();
	POINTS ptsPos = MAKEPOINTS(dwPos);
	CPoint ptPos;
	ptPos.x = ptsPos.x;
	ptPos.y = ptsPos.y;

	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);

	if (ptPos.y < rcClient.top)
	{
		int nDiff = rcClient.top - ptPos.y;
		int nScroll = nDiff / rcClient.Height() + 1;
		ScrollUp(nScroll);

		Invalidate();
	}
	else if (ptPos.y > rcClient.bottom)
	{
		int nDiff = ptPos.y - rcClient.bottom;
		int nScroll = nDiff / rcClient.Height() + 1;
		ScrollDown(nScroll);

		Invalidate();
	}
}

BOOL CXTPDatePickerList::IsSelected(int nX, CRect rcItem)
{
	BOOL bReturn = FALSE;

	// determine cursor position
	DWORD dwPos = ::GetMessagePos();
	POINTS ptsPos = MAKEPOINTS(dwPos);
	CPoint ptPos;
	ptPos.x = ptsPos.x;
	ptPos.y = ptsPos.y;

	ScreenToClient(&ptPos);

	CRect rcClient;
	GetClientRect(rcClient);

	if (ptPos.x < rcClient.left || ptPos.x > rcClient.right ||
		ptPos.y < rcClient.top || ptPos.y > rcClient.bottom)
		return FALSE;

	if (ptPos.y < 0 && nX == 0)
		bReturn = TRUE;
	else if (ptPos.y > rcClient.Height() && nX == (m_nItemsBelow + m_nItemsAbove))
		bReturn = TRUE;
	else if (ptPos.y >= rcItem.top && ptPos.y <= rcItem.bottom-1)
		bReturn = TRUE;

	return bReturn;
}

CSize CXTPDatePickerList::GetListSize()
{
	ASSERT(m_pControl);

	CSize szMaxItem(0, 0);
	CClientDC dc(m_pControl);

	CXTPDatePickerPaintManager* pPaintManager = m_pControl->GetPaintManager();

	CFont* pOldFont = dc.SelectObject(pPaintManager->GetListControlTextFont());

	szMaxItem.cy = dc.GetTextExtent(_T(" "), 1).cy + 3;

	for (int nMonth = 1; nMonth <= 12; nMonth++)
	{
		CString strText;

		if (m_pControl)
			strText.Format(_T("%s 0000"), (LPCTSTR)m_pControl->GetMonthName(nMonth));

		int iWidth = dc.GetTextExtent(strText).cx;

		if (iWidth > szMaxItem.cx)
			szMaxItem.cx = iWidth;
	}

	dc.SelectObject(pOldFont);

	// calculate rectangle
	CSize szCalendar(szMaxItem.cx + 20, szMaxItem.cy * (m_nItemsAbove + 1 + m_nItemsBelow) + 2);

	return szCalendar;
}

void CXTPDatePickerList::ShiftMonthUp(int& nYear, int& nMonth, const int nCount)
{
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		nMonth++;
		if (nMonth > 12)
		{
			nMonth = 1;
			nYear++;
		}
	}
}

void CXTPDatePickerList::ShiftMonthDown(int& nYear, int& nMonth, const int nCount)
{
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		nMonth--;
		if (nMonth < 1)
		{
			nMonth = 12;
			nYear--;
		}
	}
}

void CXTPDatePickerList::ScrollUp(int nCount)
{
	int nMonth = m_dtMiddleMonth.GetMonth();
	int nYear = m_dtMiddleMonth.GetYear();

	ShiftMonthDown(nYear, nMonth, nCount);

	COleDateTime dtMinRange, dtMaxRange;
	DWORD dwRes = m_pControl->GetRange(&dtMinRange, &dtMaxRange);
	if (dwRes & GDTR_MIN)
	{
		int nTopMonth = nMonth;
		int nTopYear = nYear;
		ShiftMonthDown(nTopYear, nTopMonth, m_nItemsAbove);
		COleDateTime dtTmp(nTopYear, nTopMonth, m_dtMiddleMonth.GetDay(), 0, 0, 0);
		if (dtTmp < dtMinRange)
		{
			nYear = dtMinRange.GetYear();
			nMonth = dtMinRange.GetMonth();
			ShiftMonthUp(nYear, nMonth, m_nItemsAbove);
		}
	}

	m_dtMiddleMonth.SetDate(nYear, nMonth, 1);
}

void CXTPDatePickerList::ScrollDown(int nCount)
{
	int nMonth = m_dtMiddleMonth.GetMonth();
	int nYear = m_dtMiddleMonth.GetYear();

	ShiftMonthUp(nYear, nMonth, nCount);

	COleDateTime dtMinRange, dtMaxRange;
	DWORD dwRes = m_pControl->GetRange(&dtMinRange, &dtMaxRange);
	if (dwRes & GDTR_MAX)
	{
		int nBottomMonth = nMonth;
		int nBottomYear = nYear;
		ShiftMonthUp(nBottomYear, nBottomMonth, m_nItemsBelow);
		COleDateTime dtTmp(nBottomYear, nBottomMonth, m_dtMiddleMonth.GetDay(), 0, 0, 0);
		if (dtTmp > dtMaxRange)
		{
			nYear = dtMaxRange.GetYear();
			nMonth = dtMaxRange.GetMonth();
			ShiftMonthDown(nYear, nMonth, m_nItemsBelow);
		}
	}

	m_dtMiddleMonth.SetDate(nYear, nMonth, 1);

	ScrollUp(0);
}

int CXTPDatePickerList::GetMonthInterval()
{
	if (m_dtSelMonth.GetStatus() == COleDateTime::valid)
	{
		return (m_dtSelMonth.GetYear() * 12 + m_dtSelMonth.GetMonth()) -
				(m_dtStartMonth.GetYear() * 12 + m_dtStartMonth.GetMonth());
	}
	return 0;
}

BOOL CXTPDatePickerList::Create(CRect rcList)
{
	// create list control
	DWORD dwStyle = WS_POPUP | WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_BORDER;

	m_pControl->ClientToScreen(rcList);

	if (!CreateEx(0, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, dwStyle, rcList, m_pControl, 0, 0))
		return FALSE;

	CSize szCalendar = GetListSize();

	m_rcListControl = rcList;
	m_rcListControl.left = m_rcListControl.CenterPoint().x - szCalendar.cx / 2 - 1;
	m_rcListControl.right = m_rcListControl.left + szCalendar.cx;
	m_rcListControl.top = m_rcListControl.CenterPoint().y - szCalendar.cy / 2 + 1;
	m_rcListControl.bottom = m_rcListControl.top + szCalendar.cy;

	SetWindowPos(NULL, m_rcListControl.left, m_rcListControl.top, m_rcListControl.Width(), m_rcListControl.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	ScrollDown(0);
	RedrawWindow();

	return TRUE;
}
