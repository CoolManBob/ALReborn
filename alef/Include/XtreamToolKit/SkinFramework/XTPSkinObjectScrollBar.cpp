// XTPSkinObjectScrollBar.cpp: implementation of the CXTPSkinObjectScrollBar class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
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

#include "XTPSkinManager.h"
#include "XTPSkinObjectScrollBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define IDSYS_SCROLL 0xFA45

CRect CXTPSkinObjectFrame::GetScrollBarRect(int nBar)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if (nBar == SB_CTL)
	{
		return rcClient;
	}

	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(rc);

	if (nBar == SB_VERT)
	{
		rc.bottom = rcClient.bottom - rc.top;
		rc.top = rcClient.top - rc.top;

		if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
		{
			rc.left = rcClient.left - rc.left - GetMetrics()->m_cxVScroll;
		}
		else
		{
			rc.left = rcClient.right - rc.left;
		}
		rc.right = rc.left + GetMetrics()->m_cxVScroll;
	}

	if (nBar == SB_HORZ)
	{
		rc.right = rcClient.right - rc.left;
		rc.left = rcClient.left - rc.left;

		rc.top = rcClient.bottom - rc.top;
		rc.bottom = rc.top + GetMetrics()->m_cyHScroll;
	}

	return rc;
}

void CXTPSkinObjectFrame::SetupScrollInfo(XTP_SKINSCROLLBARPOSINFO* pSBInfo)
{
	CRect rc = GetScrollBarRect(pSBInfo->nBar);

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);

	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	::GetScrollInfo(m_hWnd, pSBInfo->nBar, &si);

	CalcScrollBarInfo(&rc, pSBInfo, &si);
}

void CXTPSkinObjectFrame::RedrawScrollBar(int nBar)
{
	ASSERT(nBar == SB_VERT || nBar == SB_HORZ);
	RedrawScrollBar(&m_spi[nBar]);

}

void CXTPSkinObjectFrame::RedrawScrollBar(XTP_SKINSCROLLBARPOSINFO* pSBInfo)
{
	if (!pSBInfo->fVisible )
		return;

	if (!IsWindow(m_hWnd))
		return;

	CDC* pDC = pSBInfo->nBar == SB_CTL ? (CDC*)new CClientDC(this) : (CDC*)new CWindowDC(this);

	if (pDC)
	{
		CRect rc = GetScrollBarRect(pSBInfo->nBar);

		CXTPBufferDCEx dcMem(*pDC, rc);

		if (!m_pSBTrack || !m_pSBTrack->bTrackThumb)
			SetupScrollInfo(pSBInfo);

		DrawScrollBar(&dcMem, pSBInfo);
	}

	delete pDC;

}

int CXTPSkinObjectFrame::HitTestScrollBar(XTP_SKINSCROLLBARPOSINFO* pSBInfo, POINT pt)
{
	if (!pSBInfo->fVisible)
		return HTNOWHERE;

	int px = pSBInfo->fVert ? pt.y : pt.x;

	if (!::PtInRect(&pSBInfo->rc, pt))
		return HTNOWHERE;

	if (px < pSBInfo->pxUpArrow)
		return XTP_HTSCROLLUP;

	if (px >= pSBInfo->pxDownArrow)
		return XTP_HTSCROLLDOWN;

	if (px < pSBInfo->pxThumbTop)
		return XTP_HTSCROLLUPPAGE;

	if (px < pSBInfo->pxThumbBottom)
		return XTP_HTSCROLLTHUMB;

	if (px < pSBInfo->pxDownArrow)
		return XTP_HTSCROLLDOWNPAGE;

	return HTERROR;
}


void CXTPSkinObjectFrame::CalcScrollBarInfo(LPRECT lprc, XTP_SKINSCROLLBARPOSINFO* pSBInfo, SCROLLINFO* pSI)
{
	int cpx;
	DWORD dwRange;
	int denom;
	BOOL fVert = pSBInfo->fVert;

	pSBInfo->rc = *lprc;

	if (fVert)
	{
		pSBInfo->pxTop = lprc->top;
		pSBInfo->pxBottom = lprc->bottom;
		pSBInfo->pxLeft = lprc->left;
		pSBInfo->pxRight = lprc->right;
		pSBInfo->cpxThumb = GetMetrics()->m_cyVScroll;
	}
	else
	{
		pSBInfo->pxTop = lprc->left;
		pSBInfo->pxBottom = lprc->right;
		pSBInfo->pxLeft = lprc->top;
		pSBInfo->pxRight = lprc->bottom;
		pSBInfo->cpxThumb = GetMetrics()->m_cxHScroll;
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

void XTPSkinCalcTrackDragRect(XTP_SKINSCROLLBARTRACKINFO* pSBTrack)
{

	int     cx;
	int     cy;
	LPINT   pwX, pwY;

	pwX = pwY = (LPINT)&pSBTrack->rcTrack;

	if (pSBTrack->fTrackVert)
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

void CXTPSkinObjectFrame::DoScroll(HWND hwnd, HWND hWndSBNotify, int cmd, int pos, BOOL fVert)
{
	if (!hWndSBNotify)
		return;

	::SendMessage(hWndSBNotify, (UINT)(fVert ? WM_VSCROLL : WM_HSCROLL),
		MAKELONG(cmd, pos), (LPARAM)hwnd);

}

void CXTPSkinObjectFrame::EndScroll(BOOL fCancel)
{
	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;

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

			DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify,
				SB_THUMBPOSITION, pSBTrack->posOld, pSBTrack->fTrackVert);

			RedrawScrollBar(pSBTrack->pSBInfo);
		}
		else
		{

			if (pSBTrack->hTimerSB != 0)
			{
				KillTimer(pSBTrack->hTimerSB);
				pSBTrack->hTimerSB = 0;
			}
		}


		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify,
			SB_ENDSCROLL, 0, pSBTrack->fTrackVert);
	}
}

int SBPosFromPx(XTP_SKINSCROLLBARPOSINFO*  pSBInfo, int px)
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


void CXTPSkinObjectFrame::MoveThumb(int px)
{
	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;

	if ((pSBTrack == NULL) || (px == pSBTrack->pxOld))
		return;

	XTP_SKINSCROLLBARPOSINFO* pSBInfo = m_pSBTrack->pSBInfo;

pxReCalc:

	pSBTrack->posNew = SBPosFromPx(pSBInfo, px);

	if (pSBTrack->posNew != pSBTrack->posOld)
	{
		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, SB_THUMBTRACK, pSBTrack->posNew, pSBTrack->fTrackVert);

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

	RedrawScrollBar(pSBTrack->pSBInfo);
}

void CXTPSkinObjectFrame::TrackThumb(UINT message, CPoint pt)
{
	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;
	if (!pSBTrack)
		return;

	XTP_SKINSCROLLBARPOSINFO* pSBInfo = pSBTrack->pSBInfo;

	if (HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if (pSBInfo == NULL)
		return;

	int px;

	if (!PtInRect(&pSBTrack->rcTrack, pt))
		px = pSBInfo->pxStart;
	else
	{
		px = (pSBTrack->fTrackVert ? pt.y : pt.x) + pSBTrack->dpxThumb;
		if (px < pSBInfo->pxMin)
			px = pSBInfo->pxMin;
		else if (px >= pSBInfo->pxMin + pSBInfo->cpx)
			px = pSBInfo->pxMin + pSBInfo->cpx;
	}

	MoveThumb(px);

	pSBTrack->fHitOld = TRUE;

	if (message == WM_LBUTTONUP || GetKeyState(VK_LBUTTON) >= 0)
	{
		EndScroll(FALSE);
	}
}

void CXTPSkinObjectFrame::TrackBox(UINT message, CPoint point)
{
	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack == NULL)
		return;

	if (message != WM_NULL && HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if ((pSBTrack->cmdSB == SB_PAGEUP || pSBTrack->cmdSB == SB_PAGEDOWN))
	{
		int* pLength = (int *)&pSBTrack->rcTrack;

		if (pSBTrack->fTrackVert)
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
		RedrawScrollBar(pSBTrack->pSBInfo);
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
			pSBTrack->hTimerSB = SetTimer(IDSYS_SCROLL, cmsTimer, NULL);

			DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify,
				pSBTrack->cmdSB, 0, pSBTrack->fTrackVert);
		}
	}
}


void CXTPSkinObjectFrame::TrackInit(CPoint point, XTP_SKINSCROLLBARPOSINFO* pSBInfo, BOOL bDirect)
{

	int px = (pSBInfo->fVert ? point.y : point.x);

	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = new XTP_SKINSCROLLBARTRACKINFO;
	memset(pSBTrack, 0, sizeof(XTP_SKINSCROLLBARTRACKINFO));

	pSBTrack->fTrackVert = pSBInfo->fVert;

	if (pSBInfo->nBar == SB_CTL)
	{
		pSBTrack->hWndSB = m_hWnd;
		pSBTrack->hWndSBNotify = ::GetParent(m_hWnd);
		pSBTrack->fNonClient = FALSE;
	}
	else
	{
		pSBTrack->hWndSB = NULL;
		pSBTrack->hWndSBNotify = m_hWnd;
		pSBTrack->fNonClient = TRUE;
	}

	pSBTrack->cmdSB = (UINT)-1;
	pSBTrack->nBar = pSBInfo->nBar;
	pSBTrack->bTrackThumb = FALSE;
	pSBTrack->pSBInfo = pSBInfo;

	m_pSBTrack = pSBTrack;


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
		XTPSkinCalcTrackDragRect(pSBTrack);

		pSBTrack->pxOld = pSBInfo->pxStart = pSBInfo->pxThumbTop;
		pSBTrack->posNew = pSBTrack->posOld = pSBInfo->pos;
		pSBTrack->dpxThumb = pSBInfo->pxStart - px;

		pSBTrack->bTrackThumb = TRUE;

		SetCapture();

		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify,
			SB_THUMBTRACK, pSBTrack->posOld, pSBTrack->fTrackVert);

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

	SetCapture();

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

	while (::GetCapture() == m_hWnd)
	{
		while (::GetCapture() == m_hWnd)
		{
			MSG msg;

			if (!::GetMessage(&msg, NULL, 0, 0))
			{
				AfxPostQuitMessage((int)msg.wParam);
				break;
			}

			UINT cmd = msg.message;

			if (cmd == WM_TIMER && msg.wParam == IDSYS_SCROLL)
			{
				ContScroll();
			}
			else if (cmd >= WM_MOUSEFIRST && cmd <= WM_MOUSELAST)
			{
				CPoint ptScreen = msg.pt;

				if (pSBTrack->fNonClient)
					ScreenToFrame(&ptScreen);
				else
					ScreenToClient(&ptScreen);

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
	}

	if (pSBTrack->hTimerSB != 0)
	{
		KillTimer(pSBTrack->hTimerSB);
	}

	pSBInfo->ht = 0;

	delete m_pSBTrack;
	m_pSBTrack = NULL;

	RedrawScrollBar(pSBInfo);
}


void CXTPSkinObjectFrame::ContScroll()
{
	XTP_SKINSCROLLBARTRACKINFO* pSBTrack = m_pSBTrack;
	ASSERT(pSBTrack);

	if (pSBTrack == NULL)
		return;

	CPoint pt;
	GetCursorPos(&pt);

	if (pSBTrack->fNonClient)
		ScreenToFrame(&pt);
	else
		ScreenToClient(&pt);

	TrackBox(WM_NULL, pt);

	if (pSBTrack->fHitOld)
	{
		pSBTrack->hTimerSB = SetTimer( IDSYS_SCROLL, GetDoubleClickTime() / 10, NULL);

		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify,
			pSBTrack->cmdSB, 0, pSBTrack->fTrackVert);
	}
}



void CXTPSkinObjectFrame::DrawScrollBar(CDC* pDC, XTP_SKINSCROLLBARPOSINFO* pSBInfo)
{
	GetSkinManager()->GetSchema()->DrawThemeScrollBar(pDC, this, pSBInfo);
}


//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectScrollBar
IMPLEMENT_DYNCREATE(CXTPSkinObjectScrollBar, CXTPSkinObjectFrame)

CXTPSkinObjectScrollBar::CXTPSkinObjectScrollBar()
{
	ZeroMemory(&m_spiCtl, sizeof(XTP_SKINSCROLLBARPOSINFO));

	m_strClassName = _T("SCROLLBAR");
}

CXTPSkinObjectScrollBar::~CXTPSkinObjectScrollBar()
{

}

BEGIN_MESSAGE_MAP(CXTPSkinObjectScrollBar, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectScrollBar)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_ENABLE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(SBM_SETSCROLLINFO, OnSetScrollInfo)
	ON_MESSAGE(SBM_GETSCROLLINFO, OnGetScrollInfo)
	ON_MESSAGE(SBM_SETPOS, OnSetScrollPos)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectScrollBar message handlers

int CXTPSkinObjectScrollBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	Invalidate(FALSE);

	return 0;
}

LRESULT CXTPSkinObjectScrollBar::OnPrintClient(WPARAM wParam, LPARAM /*lParam*/)
{
	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectScrollBar::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CXTPSkinObjectScrollBar::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);

	CXTPBufferDC dcMem(*pDC, rc);

	if (!fSize)
	{
		m_spiCtl.fVert = GetStyle() & SBS_VERT;
		m_spiCtl.fVisible = TRUE;
		m_spiCtl.nBar = SB_CTL;

		if (!m_pSBTrack || !m_pSBTrack->bTrackThumb)
			SetupScrollInfo(&m_spiCtl);

		DrawScrollBar(&dcMem, &m_spiCtl);

	}
	else
	{
		CXTPSkinManagerClass* pClassScrollBar = GetSkinManager()->GetSkinClass(_T("SCROLLBAR"));
		FillBackground(&dcMem, rc);

		if (GetStyle() & SBS_SIZEGRIP)
		{
			pClassScrollBar->DrawThemeBackground(&dcMem, SBP_SIZEBOX, SZB_RIGHTALIGN, rc);
		}
	}

}

void CXTPSkinObjectScrollBar::OnEnable(BOOL bEnable)
{
	CXTPSkinObjectFrame::OnEnable(bEnable);
	Invalidate(FALSE);
}

void CXTPSkinObjectScrollBar::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectScrollBar::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);

	if (fSize)
		return;

	int ht = HitTestScrollBar(&m_spiCtl, point);

	if (ht != m_spiCtl.ht)
	{
		m_spiCtl.ht = ht;
		Invalidate(FALSE);

		if (m_spiCtl.ht != HTNOWHERE)
		{
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
		}
	}
}


void CXTPSkinObjectScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);

	if (fSize)
	{
		CXTPSkinObjectFrame::OnLButtonDown(nFlags, point);
		return;
	}

	TrackInit(point, &m_spiCtl, (GetKeyState(VK_SHIFT) < 0) ? TRUE : FALSE);

}

void CXTPSkinObjectScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);
	if (fSize)
	{
		CXTPSkinObjectFrame::OnLButtonDblClk(nFlags, point);
		return;
	}

	OnLButtonDown(nFlags, point);
}

LRESULT CXTPSkinObjectScrollBar::OnSetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc(SBM_SETSCROLLINFO, FALSE, lParam);

	if (wParam && IsWindowVisible())
	{
		CClientDC dc(this);
		OnDraw(&dc);
	}

	return lResult;
}

LRESULT CXTPSkinObjectScrollBar::OnGetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LRESULT bResult = DefWindowProc(SBM_GETSCROLLINFO, wParam, lParam);

	LPSCROLLINFO lpsi = (LPSCROLLINFO)lParam;

	if (lpsi && (lpsi->fMask & SIF_TRACKPOS) && m_pSBTrack)
	{
		lpsi->nTrackPos = m_pSBTrack->posNew;
	}

	return bResult;
}

LRESULT CXTPSkinObjectScrollBar::OnSetScrollPos(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc(SBM_SETPOS, wParam, FALSE);

	if (lParam && IsWindowVisible())
	{
		CClientDC dc(this);
		OnDraw(&dc);
	}

	return lResult;
}

void CXTPSkinObjectScrollBar::OnSetFocus(CWnd* pOldWnd)
{
	pOldWnd;
}
