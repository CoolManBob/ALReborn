// XTPScrollBar.cpp : implementation of the CXTPScrollBar class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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

#include "XTPCommandBars.h"
#include "XTPPaintManager.h"
#include "XTPControlGallery.h"
#include "XTPScrollBar.h"
#include "XTPMouseManager.h"

//////////////////////////////////////////////////////////////////////////
//  CXTPScrollBase

#define IDSYS_SCROLL 23345


CXTPScrollBase::CXTPScrollBase()
{
	ZeroMemory(&m_spi, sizeof(m_spi));
	m_spi.fVert = TRUE;

	m_pSBTrack = NULL;

	m_scrollBarStyle = xtpScrollStyleDefault;
}

CXTPScrollBase::~CXTPScrollBase()
{

}

void CXTPScrollBase::CalcScrollBarInfo(LPRECT lprc, SCROLLBARPOSINFO* pSBInfo, SCROLLINFO* pSI)
{
	int cpx;
	DWORD dwRange;
	int denom;
	BOOL fVert = pSBInfo->fVert;

	pSBInfo->rc = *lprc;
	pSBInfo->pxPopup = 0;

	if (fVert)
	{
		pSBInfo->pxTop = lprc->top;
		pSBInfo->pxBottom = lprc->bottom;
		pSBInfo->pxLeft = lprc->left;
		pSBInfo->pxRight = lprc->right;
		pSBInfo->cpxThumb = GetGalleryPaintManager()->m_cyVScroll;
	}
	else
	{
		pSBInfo->pxTop = lprc->left;
		pSBInfo->pxBottom = lprc->right;
		pSBInfo->pxLeft = lprc->top;
		pSBInfo->pxRight = lprc->bottom;
		pSBInfo->cpxThumb = GetGalleryPaintManager()->m_cxHScroll;
	}

	pSBInfo->pos = pSI->nPos;
	pSBInfo->page = pSI->nPage;
	pSBInfo->posMin = pSI->nMin;
	pSBInfo->posMax = pSI->nMax;

	dwRange = ((DWORD)(pSBInfo->posMax - pSBInfo->posMin)) + 1;

	cpx = min((pSBInfo->pxBottom - pSBInfo->pxTop) / 2, pSBInfo->cpxThumb);

	pSBInfo->pxUpArrow  = pSBInfo->pxTop    + cpx;
	pSBInfo->pxDownArrow = pSBInfo->pxBottom - cpx;

	if ((pSBInfo->page != 0) && (dwRange != 0))
	{
		int i = MulDiv(pSBInfo->pxDownArrow - pSBInfo->pxUpArrow,
			pSBInfo->page, dwRange);

		pSBInfo->cpxThumb = max(pSBInfo->cpxThumb / 2, i);
	}

	pSBInfo->pxMin = pSBInfo->pxTop + cpx;
	pSBInfo->cpx = pSBInfo->pxBottom - cpx - pSBInfo->cpxThumb - pSBInfo->pxMin;

	denom = dwRange - (pSBInfo->page ? pSBInfo->page : 1);
	if (denom)
		pSBInfo->pxThumbTop = MulDiv(pSBInfo->pos - pSBInfo->posMin,
		pSBInfo->cpx, denom) + pSBInfo->pxMin;
	else
		pSBInfo->pxThumbTop = pSBInfo->pxMin - 1;

	pSBInfo->pxThumbBottom = pSBInfo->pxThumbTop + pSBInfo->cpxThumb;
}

void CXTPScrollBase::SetupScrollInfo()
{
	CRect rc = GetScrollBarRect();

	SCROLLINFO si;
	GetScrollInfo(&si);

	CalcScrollBarInfo(&rc, &m_spi, &si);
}

int CXTPScrollBase::HitTestScrollBar(POINT pt)
{
	int px = m_spi.fVert ? pt.y : pt.x;

	if (!::PtInRect(&m_spi.rc, pt))
		return HTNOWHERE;

	if (px < m_spi.pxUpArrow)
		return XTP_HTSCROLLUP;

	if (m_spi.pxPopup > 0 && px > m_spi.pxPopup)
		return XTP_HTSCROLLPOPUP;

	if (px >= m_spi.pxDownArrow)
		return XTP_HTSCROLLDOWN;

	if (px < m_spi.pxThumbTop)
		return XTP_HTSCROLLUPPAGE;

	if (px < m_spi.pxThumbBottom)
		return XTP_HTSCROLLTHUMB;

	if (px < m_spi.pxDownArrow)
		return XTP_HTSCROLLDOWNPAGE;

	return HTERROR;
}


//////////////////////////////////////////////////////////////////////////
// Scrolling

void CXTPScrollBase::EndScroll(BOOL fCancel)
{
	SCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack)
	{
		pSBTrack->cmdSB = 0;
		ReleaseCapture();

		if (pSBTrack->bTrackThumb)
		{
			if (fCancel)
			{
				pSBTrack->posOld = pSBTrack->pSBInfo->pos;
			}

			DoScroll(SB_THUMBPOSITION, pSBTrack->posOld);

			RedrawScrollBar();
		}
		else
		{

			if (pSBTrack->hTimerSB != 0)
			{
				::KillTimer(pSBTrack->hWndTrack, pSBTrack->hTimerSB);
				pSBTrack->hTimerSB = 0;
			}
		}


		DoScroll(SB_ENDSCROLL, 0);
	}
}

int SBPosFromPx(CXTPScrollBase::SCROLLBARPOSINFO*  pSBInfo, int px)
{
	if (px < pSBInfo->pxMin)
	{
		return pSBInfo->posMin;
	}

	if (px >= pSBInfo->pxMin + pSBInfo->cpx)
	{
		return (pSBInfo->posMax - (pSBInfo->page ? pSBInfo->page - 1 : 0));
	}

	if (pSBInfo->cpx)
		return (pSBInfo->posMin + MulDiv(pSBInfo->posMax - pSBInfo->posMin -
		(pSBInfo->page ? pSBInfo->page - 1 : 0),
		px - pSBInfo->pxMin, pSBInfo->cpx));
	else
		return (pSBInfo->posMin - 1);
}


void CXTPScrollBase::MoveThumb(int px)
{
	SCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;

	if ((pSBTrack == NULL) || (px == pSBTrack->pxOld))
		return;

	SCROLLBARPOSINFO* pSBInfo = m_pSBTrack->pSBInfo;

pxReCalc:

	pSBTrack->posNew = SBPosFromPx(pSBInfo, px);

	if (pSBTrack->posNew != pSBTrack->posOld)
	{
		DoScroll(SB_THUMBTRACK, pSBTrack->posNew);

		pSBTrack->posOld = pSBTrack->posNew;

		if (px >= pSBInfo->pxMin + pSBInfo->cpx)
		{
			px = pSBInfo->pxMin + pSBInfo->cpx;
			goto pxReCalc;
		}

	}

	pSBInfo->pxThumbTop = px;
	pSBInfo->pxThumbBottom = pSBInfo->pxThumbTop + pSBInfo->cpxThumb;
	pSBTrack->pxOld = px;

	RedrawScrollBar();
}

void CXTPScrollBase::TrackThumb(UINT message, CPoint pt)
{
	SCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;
	if (!pSBTrack)
		return;

	SCROLLBARPOSINFO* pSBInfo = pSBTrack->pSBInfo;

	if (HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if (pSBInfo == NULL)
		return;

	int px;

	if (!PtInRect(&pSBTrack->rcTrack, pt))
		px = pSBInfo->pxStart;
	else
	{
		px = (pSBTrack->pSBInfo->fVert ? pt.y : pt.x) + pSBTrack->dpxThumb;
		if (px < pSBInfo->pxMin)
			px = pSBInfo->pxMin;
		else if (px >= pSBInfo->pxMin + pSBInfo->cpx)
			px = pSBInfo->pxMin + pSBInfo->cpx;
	}

	MoveThumb(px);

	if (message == WM_LBUTTONUP || GetKeyState(VK_LBUTTON) >= 0)
	{
		EndScroll(FALSE);
	}
}

void CXTPScrollBase::TrackBox(UINT message, CPoint point)
{
	SCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack == NULL)
		return;

	if (message != WM_NULL && HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if ((pSBTrack->cmdSB == SB_PAGEUP || pSBTrack->cmdSB == SB_PAGEDOWN))
	{
		int* pLength = (int *)&pSBTrack->rcTrack;

		if (pSBTrack->pSBInfo->fVert)
			pLength++;

		if (pSBTrack->cmdSB == SB_PAGEUP)
			pLength[2] = pSBTrack->pSBInfo->pxThumbTop;
		else
			pLength[0] = pSBTrack->pSBInfo->pxThumbBottom;
	}

	BOOL fHit = PtInRect(&pSBTrack->rcTrack, point);

	BOOL fHitChanged = fHit != (BOOL)pSBTrack->fHitOld;

	if (fHitChanged)
	{
		pSBTrack->fHitOld = fHit;
		RedrawScrollBar();
	}

	int cmsTimer = GetDoubleClickTime() / 10;

	switch (message)
	{
	case WM_LBUTTONUP:
		EndScroll(FALSE);
		break;

	case WM_LBUTTONDOWN:
		pSBTrack->hTimerSB = 0;
		cmsTimer = GetDoubleClickTime() * 4 / 5;

		/*
		*** FALL THRU **
		*/

	case WM_MOUSEMOVE:
		if (fHit && fHitChanged)
		{
			pSBTrack->hTimerSB = SetTimer(m_pSBTrack->hWndTrack, IDSYS_SCROLL, cmsTimer, NULL);

			DoScroll(pSBTrack->cmdSB, 0);
		}
	}
}

void CXTPScrollBase::ContScroll()
{
	SCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;
	ASSERT(pSBTrack);

	if (pSBTrack == NULL)
		return;

	CPoint pt;
	GetCursorPos(&pt);

	ScreenToClient(m_pSBTrack->hWndTrack, &pt);

	TrackBox(WM_NULL, pt);

	if (pSBTrack->fHitOld)
	{
		pSBTrack->hTimerSB = SetTimer(m_pSBTrack->hWndTrack, IDSYS_SCROLL, GetDoubleClickTime() / 10, NULL);

		DoScroll(pSBTrack->cmdSB, 0);
	}
}

void CXTPScrollBase::CalcTrackDragRect(SCROLLBARTRACKINFO* pSBTrack) const
{

	int     cx;
	int     cy;
	LPINT   pwX, pwY;

	pwX = pwY = (LPINT)&pSBTrack->rcTrack;

	if (pSBTrack->pSBInfo->fVert)
	{
		cy = GetSystemMetrics(SM_CYVTHUMB);
		pwY++;
	}
	else
	{
		cy = GetSystemMetrics(SM_CXHTHUMB);
		pwX++;
	}

	cx = (pSBTrack->pSBInfo->pxRight - pSBTrack->pSBInfo->pxLeft) * 8;
	cy *= 2;

	*(pwX + 0) = pSBTrack->pSBInfo->pxLeft - cx;
	*(pwY + 0) = pSBTrack->pSBInfo->pxTop - cy;
	*(pwX + 2) = pSBTrack->pSBInfo->pxRight + cx;
	*(pwY + 2) = pSBTrack->pSBInfo->pxBottom + cy;
}

void CXTPScrollBase::PerformTrackInit(HWND hWnd, CPoint point, SCROLLBARPOSINFO* pSBInfo, BOOL bDirect)
{

	int px = m_spi.fVert ? point.y : point.x;

	SCROLLBARTRACKINFO* pSBTrack = new SCROLLBARTRACKINFO;
	memset(pSBTrack, 0, sizeof(SCROLLBARTRACKINFO));

	pSBTrack->cmdSB = (UINT)-1;
	pSBTrack->bTrackThumb = FALSE;
	pSBTrack->pSBInfo = pSBInfo;
	pSBTrack->hWndTrack = hWnd;

	m_pSBTrack = pSBTrack;

	CXTPMouseManager* pMouseManager = XTPMouseManager();

	RECT rcSB;
	LPINT pwX = (LPINT)&rcSB;
	LPINT pwY = pwX + 1;
	if (!pSBInfo->fVert)
		pwX = pwY--;

	*(pwX + 0) = pSBInfo->pxLeft;
	*(pwY + 0) = pSBInfo->pxTop;
	*(pwX + 2) = pSBInfo->pxRight;
	*(pwY + 2) = pSBInfo->pxBottom;


	if (px < pSBInfo->pxUpArrow)
	{
		pSBInfo->ht = XTP_HTSCROLLUP;
		pSBTrack->cmdSB = SB_LINEUP;
		*(pwY + 2) = pSBInfo->pxUpArrow;

	}
	else if (px >= pSBInfo->pxDownArrow)
	{
		pSBInfo->ht = XTP_HTSCROLLDOWN;
		pSBTrack->cmdSB = SB_LINEDOWN;
		*(pwY + 0) = pSBInfo->pxDownArrow;
	}
	else if (px < pSBInfo->pxThumbTop)
	{
		pSBInfo->ht = XTP_HTSCROLLUPPAGE;
		pSBTrack->cmdSB = SB_PAGEUP;
		*(pwY + 0) = pSBInfo->pxUpArrow;
		*(pwY + 2) = pSBInfo->pxThumbTop;
	}
	else if (px < pSBInfo->pxThumbBottom)
	{
		pSBInfo->ht = XTP_HTSCROLLTHUMB;
DoThumbPos:

		if (pSBInfo->pxDownArrow - pSBInfo->pxUpArrow <= pSBInfo->cpxThumb)
		{
			delete m_pSBTrack;
			m_pSBTrack = NULL;
			return;
		}

		pSBTrack->cmdSB = SB_THUMBPOSITION;
		CalcTrackDragRect(pSBTrack);

		pSBTrack->pxOld = pSBInfo->pxStart = pSBInfo->pxThumbTop;
		pSBTrack->posNew = pSBTrack->posOld = pSBInfo->pos;
		pSBTrack->dpxThumb = pSBInfo->pxStart - px;

		pSBTrack->bTrackThumb = TRUE;

		::SetCapture(hWnd);

		DoScroll(SB_THUMBTRACK, pSBTrack->posOld);

	}
	else if (px < pSBInfo->pxDownArrow)
	{
		pSBInfo->ht = XTP_HTSCROLLDOWNPAGE;
		pSBTrack->cmdSB = SB_PAGEDOWN;
		*(pwY + 0) = pSBInfo->pxThumbBottom;
		*(pwY + 2) = pSBInfo->pxDownArrow;
	}

	if ((bDirect && pSBTrack->cmdSB != SB_LINEUP && pSBTrack->cmdSB != SB_LINEDOWN))
	{
		if (pSBTrack->cmdSB != SB_THUMBPOSITION)
		{
			goto DoThumbPos;
		}
		pSBTrack->dpxThumb = -(pSBInfo->cpxThumb / 2);
	}

	::SetCapture(hWnd);

	if (pSBTrack->cmdSB != SB_THUMBPOSITION)
	{
		CopyRect(&pSBTrack->rcTrack, &rcSB);
	}

	if (!pSBTrack->bTrackThumb)
	{
		TrackBox(WM_LBUTTONDOWN, point);
	}
	else
	{
		TrackThumb(WM_LBUTTONDOWN, point);

	}

	pMouseManager->LockMouseMove();

	while (::GetCapture() == hWnd)
	{
			MSG msg;

			if (!::GetMessage(&msg, NULL, 0, 0))
			{
				AfxPostQuitMessage((int)msg.wParam);
				break;
			}

			if (!IsWindow(hWnd))
				break;

			UINT cmd = msg.message;

			if (cmd == WM_TIMER && msg.wParam == IDSYS_SCROLL)
			{
				ContScroll();
			}
			else if (cmd >= WM_MOUSEFIRST && cmd <= WM_MOUSELAST)
			{
				CPoint ptScreen = msg.pt;

				ScreenToClient(hWnd, &ptScreen);

				if (!pSBTrack->bTrackThumb)
				{
					TrackBox(cmd, ptScreen);
				}
				else
				{
					TrackThumb(cmd, ptScreen);
				}
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

	}
	pMouseManager->UnlockMouseMove();

	if (pSBTrack->hTimerSB != 0)
	{
		::KillTimer(hWnd, pSBTrack->hTimerSB);
	}

	delete m_pSBTrack;
	m_pSBTrack = NULL;

	if (IsWindow(hWnd))
	{
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		m_spi.ht = HitTestScrollBar(point);

		RedrawScrollBar();
	}
}

XTPScrollBarStyle CXTPScrollBase::GetScrollBarStyle() const
{
	if (m_scrollBarStyle != xtpScrollStyleDefault)
		return m_scrollBarStyle;

	switch (GetGalleryPaintManager()->GetPaintManager()->BaseTheme())
	{
		case xtpThemeOffice2000:
			return xtpScrollStyleFlat;

		case xtpThemeOffice2007:
		case xtpThemeRibbon:
			return xtpScrollStyleOffice2007Light;
	}

	return xtpScrollStyleSystem;
}


//////////////////////////////////////////////////////////////////////////
// CXTPScrollBar

CXTPScrollBar::CXTPScrollBar()
{
	m_pCommandBars = 0;
}

CXTPScrollBar::~CXTPScrollBar()
{

}

BEGIN_MESSAGE_MAP(CXTPScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CXTPStatusBar)
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(SBM_SETSCROLLINFO, OnSetScrollInfo)
	ON_MESSAGE(SBM_GETSCROLLINFO, OnGetScrollInfo)
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CRect CXTPScrollBar::GetScrollBarRect()
{
	return CXTPClientRect(this);
}

void CXTPScrollBar::GetScrollInfo(SCROLLINFO* psi)
{
	psi->cbSize = sizeof(SCROLLINFO);

	psi->fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	::GetScrollInfo(m_hWnd, SB_CTL, psi);
}

BOOL CXTPScrollBar::IsScrollBarEnabled() const
{
	return IsWindowEnabled();
}

CWnd* CXTPScrollBar::GetParentWindow() const
{
	return GetParent();
}

CXTPControlGalleryPaintManager* CXTPScrollBar::GetGalleryPaintManager() const
{
	if (m_pCommandBars)
		return m_pCommandBars->GetPaintManager()->GetGalleryPaintManager();

	return XTPPaintManager()->GetGalleryPaintManager();
}

void CXTPScrollBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	CXTPBufferDC dcMem(dc, rc);

	DefWindowProc(WM_PAINT, (WPARAM)dcMem.GetSafeHdc(), 0);

	m_spi.fVert = GetStyle() & SBS_VERT;

	if (!m_pSBTrack || !m_pSBTrack->bTrackThumb)
		SetupScrollInfo();

	GetGalleryPaintManager()->DrawScrollBar(&dcMem, this);
}

void CXTPScrollBar::DoScroll(int cmd, int pos)
{
	GetParent()->SendMessage((UINT)(m_spi.fVert ? WM_VSCROLL : WM_HSCROLL),
		MAKELONG(cmd, pos), (LPARAM)m_hWnd);
}

void CXTPScrollBar::RedrawScrollBar()
{
	Invalidate(FALSE);
}


void CXTPScrollBar::OnEnable(BOOL bEnable)
{
	CScrollBar::OnEnable(bEnable);
	Invalidate(FALSE);
}

void CXTPScrollBar::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPScrollBar::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	int ht = HitTestScrollBar(point);

	if (ht != m_spi.ht)
	{
		m_spi.ht = ht;
		Invalidate(FALSE);

		if (m_spi.ht != HTNOWHERE)
		{
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
		}
	}
}


void CXTPScrollBar::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	PerformTrackInit(m_hWnd, point, &m_spi, (GetKeyState(VK_SHIFT) < 0) ? TRUE : FALSE);
}

void CXTPScrollBar::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CScrollBar::OnContextMenu(pWnd, pos);
	Invalidate(FALSE);
}

void CXTPScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

LRESULT CXTPScrollBar::OnSetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LRESULT bResult = DefWindowProc(SBM_SETSCROLLINFO, FALSE, lParam);

	if (wParam)
	{
		Invalidate(FALSE);
		UpdateWindow();
	}

	return bResult;
}

LRESULT CXTPScrollBar::OnGetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LRESULT bResult = DefWindowProc(SBM_GETSCROLLINFO, wParam, lParam);

	LPSCROLLINFO lpsi = (LPSCROLLINFO)lParam;

	if (lpsi && (lpsi->fMask & SIF_TRACKPOS) && m_pSBTrack)
	{
		lpsi->nTrackPos = m_pSBTrack->posNew;
	}

	return bResult;
}


void CXTPScrollBar::OnSetFocus(CWnd* pOldWnd)
{
	pOldWnd;
}
