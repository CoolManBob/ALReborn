// CXTPReportDragDrop.cpp : implementation of the CXTPReportHeaderDropWnd class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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

#include "XTPReportDefines.h"
#include "XTPReportHeader.h"
#include "XTPReportPaintManager.h"
#include "XTPReportColumn.h"
#include "XTPReportDragDrop.h"
#include "XTPReportControl.h"
#include "XTPReportColumn.h"
#include "XTPReportColumns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPReportHeaderDragWnd

CXTPReportHeaderDragWnd::CXTPReportHeaderDragWnd()
{
	m_pHeader = NULL;
	m_pPaintManager = NULL;
	m_pColumn = NULL;
}

CXTPReportHeaderDragWnd::~CXTPReportHeaderDragWnd()
{
}


BEGIN_MESSAGE_MAP(CXTPReportHeaderDragWnd, CWnd)
	//{{AFX_MSG_MAP(CXTPReportHeaderDragWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPReportHeaderDragWnd message handlers

BOOL CXTPReportHeaderDragWnd::Create(CRect rect, CXTPReportHeader* pHeader, CXTPReportPaintManager* pPaintManager, CXTPReportColumn* pColumn)
{
	m_pHeader = pHeader;
	m_pPaintManager = pPaintManager;
	m_pColumn = pColumn;
	if (rect.Height() == 0)
		rect.bottom = rect.top + pPaintManager->GetHeaderHeight();

	DWORD dwStyle = WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	BOOL bCreated = CreateEx(dwExStyle, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, dwStyle,
		rect.left, rect.top, rect.Width(), rect.Height(),
		NULL, NULL, NULL);

	if (bCreated && m_pHeader && m_pHeader->GetControl() &&
		m_pHeader->GetControl()->m_bHScrollBarVisible )
	{
		SetTimer(1, 100, NULL);
	}
	return bCreated;
}


void CXTPReportHeaderDragWnd::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rc(this);
	CXTPBufferDC memDC(dc, rc);
	OnDraw(&memDC, rc);
}

BOOL CXTPReportHeaderDragWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPReportHeaderDragWnd::OnDraw(CDC* pDC, CRect rcClient)
{
	if (!m_pPaintManager)
		return;

	// draw background
	m_pPaintManager->FillHeaderControl(pDC, rcClient);
	// draw column header
	m_pPaintManager->DrawColumn(pDC, m_pColumn, m_pHeader, rcClient, TRUE);
}

void CXTPReportHeaderDragWnd::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	delete this;
}

void CXTPReportHeaderDragWnd::OnTimer(UINT_PTR nIDEvent)
{
	UNREFERENCED_PARAMETER(nIDEvent);

	if (!m_pHeader || !m_pHeader->GetControl())
	{
		return;
	}
	CXTPReportControl* pControl = m_pHeader->GetControl();

	ASSERT(pControl->m_bHScrollBarVisible);

	CRect rcHeaderScreen = m_pHeader->m_rcHeader;
	pControl->ClientToScreen(&rcHeaderScreen);

	CRect rcControlScreen(0,0,0,0);
	CRect rcDragScreen(0,0,0,0);

	pControl->GetWindowRect(&rcControlScreen);
	GetWindowRect(&rcDragScreen);


	CPoint ptMouseScreen(0, 0);
	if (!GetCursorPos(&ptMouseScreen) ||
		ptMouseScreen.y < rcHeaderScreen.top || ptMouseScreen.y > rcHeaderScreen.bottom)
	{
		return;
	}

	BOOL bScrolled = FALSE;
	BOOL bLayoutRTL = pControl->GetExStyle() & WS_EX_LAYOUTRTL;

	int nDiffRight = bLayoutRTL ? rcControlScreen.left - rcDragScreen.left :
		rcDragScreen.right - rcControlScreen.right;

	if (nDiffRight > 0)
	{
		int nStep = max(min(7, nDiffRight/10), CXTPReportHeader::s_nMinAutoScrollStep);
		UINT uTimer = max(10, 100 - nDiffRight);
		SCROLLINFO scrollInfo;

		BOOL bRes = pControl->GetScrollInfo(SB_HORZ, &scrollInfo);
		if (bRes)
		{
			if (scrollInfo.nPos < scrollInfo.nMax)
			{
				pControl->SetLeftOffset(scrollInfo.nPos + nStep);
				SetTimer(1, uTimer, NULL);
				bScrolled = TRUE;
			}
		}
	}
	else
	{
		int nLeftBorderX = bLayoutRTL ? rcControlScreen.right : rcControlScreen.left;

		int nFreezCols = pControl->GetFreezeColumnsCount();
		if (nFreezCols && pControl->GetColumns())
		{
			CXTPReportColumn* pLastFCol = pControl->GetColumns()->GetVisibleAt(nFreezCols - 1);
			if (pLastFCol)
			{
				CRect rcLastFCol = pLastFCol->GetRect();
				pControl->ClientToScreen(&rcLastFCol);

				int nLastFCol_middleX = rcLastFCol.left + rcLastFCol.Width() / 2;
				if (rcDragScreen.left < rcLastFCol.right &&
					ptMouseScreen.x > nLastFCol_middleX )
				{
					nLeftBorderX = rcLastFCol.right;
				}
			}
		}

		int nDiffLeft = bLayoutRTL ? rcDragScreen.right - nLeftBorderX : nLeftBorderX - rcDragScreen.left;

		if (nDiffLeft > 0)
		{
			int nStep = max(min(7, nDiffLeft/10), CXTPReportHeader::s_nMinAutoScrollStep);
			UINT uTimer = max(10, 100 - nDiffLeft);
			SCROLLINFO scrollInfo;

			BOOL bRes = pControl->GetScrollInfo(SB_HORZ, &scrollInfo);
			if (bRes)
			{
				if (scrollInfo.nPos > scrollInfo.nMin)
				{
					pControl->SetLeftOffset(scrollInfo.nPos - nStep);
					SetTimer(1, uTimer, NULL);
					bScrolled = TRUE;
				}
			}
		}
	}

	//-------------------------------------------------------------------------
	if (bScrolled)
	{
		CPoint ptMouse = ptMouseScreen;
		pControl->ScreenToClient(&ptMouse);

		pControl->RedrawControl();
		pControl->UpdateWindow();

		m_pHeader->OnMouseMove(0, ptMouse);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTPReportHeaderDropWnd

CXTPReportHeaderDropWnd::CXTPReportHeaderDropWnd(COLORREF crColor)
	: m_clr(crColor)
{
	m_nHeight = 0;
}

CXTPReportHeaderDropWnd::~CXTPReportHeaderDropWnd()
{
}


BEGIN_MESSAGE_MAP(CXTPReportHeaderDropWnd, CWnd)
	//{{AFX_MSG_MAP(CXTPReportHeaderDropWnd)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPReportHeaderDropWnd message handlers

BOOL CXTPReportHeaderDropWnd::Create(int nHeight)
{
	m_nHeight = nHeight + 20;

	DWORD dwStyle = WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW ;

	BOOL bResult = CreateEx(dwExStyle, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, dwStyle,
		0, 0, 12, m_nHeight,
		NULL, NULL, NULL);

	CRgn rgn1, rgn2;
	POINT ptArrow[7];

	ptArrow[0].x = 8;   ptArrow[0].y = 0;
	ptArrow[1].x = 8;   ptArrow[1].y = 4;
	ptArrow[2].x = 11;  ptArrow[2].y = 4;
	ptArrow[3].x = 6;   ptArrow[3].y = 9;
	ptArrow[4].x = 1;   ptArrow[4].y = 4;
	ptArrow[5].x = 4;   ptArrow[5].y = 4;
	ptArrow[6].x = 4;   ptArrow[6].y = 0;
	rgn1.CreatePolygonRgn(ptArrow, 7, ALTERNATE);


	ptArrow[0].x = 4;   ptArrow[0].y = m_nHeight;
	ptArrow[1].x = 4;   ptArrow[1].y = m_nHeight-4;
	ptArrow[2].x = 0;   ptArrow[2].y = m_nHeight-4;
	ptArrow[3].x = 6;   ptArrow[3].y = m_nHeight-10;
	ptArrow[4].x = 12;  ptArrow[4].y = m_nHeight-4;
	ptArrow[5].x = 8;   ptArrow[5].y = m_nHeight-4;
	ptArrow[6].x = 8;   ptArrow[6].y = m_nHeight;
	rgn2.CreatePolygonRgn(ptArrow, 7, ALTERNATE);

	m_rgn.CreateRectRgn(0, 0, 12, nHeight);
	m_rgn.CombineRgn(&rgn1, &rgn2, RGN_OR);
	SetWindowRgn(m_rgn, FALSE);

	rgn1.DeleteObject();
	rgn2.DeleteObject();

	return bResult;
}


void CXTPReportHeaderDropWnd::PostNcDestroy()
{
	m_rgn.DeleteObject();

	CWnd::PostNcDestroy();
	delete this;
}

BOOL CXTPReportHeaderDropWnd::OnEraseBkgnd(CDC* pDC)
{
	pDC->FillSolidRect(0, 0, 12, m_nHeight, m_clr);
	return TRUE;
}

void CXTPReportHeaderDropWnd::SetWindowPos(int x, int y)
{
	CWnd::SetWindowPos(&wndTop,
		x - 6, y - (m_nHeight / 2), 0, 0,
		SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
}
