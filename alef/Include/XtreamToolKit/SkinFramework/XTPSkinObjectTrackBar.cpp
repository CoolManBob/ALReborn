// XTPSkinObjectTrackBar.cpp: implementation of the CXTPSkinObjectTrackBar class.
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

#include "XTPSkinObjectTrackBar.h"
#include "XTPSkinManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

inline void swap(long& _X, long& _Y)
{
	long _Tmp = _X;
	_X = _Y;
	_Y = _Tmp;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectTrackBar, CXTPSkinObjectFrame)

CXTPSkinObjectTrackBar::CXTPSkinObjectTrackBar()
{
	m_strClassName = _T("TRACKBAR");

	m_nTickFreq = 1;
	m_bHotThumb = FALSE;

	ZeroMemory(&m_drawRect, sizeof(m_drawRect));
}

CXTPSkinObjectTrackBar::~CXTPSkinObjectTrackBar()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectTrackBar, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectTrackBar)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(TBM_SETTICFREQ, OnSetTicFreq)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectTrackBar message handlers


BOOL CXTPSkinObjectTrackBar::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

#define TICKHEIGHT 3
#define BORDERSIZE 2
#define MIN_THUMB_HEIGHT (2 * GetSystemMetrics(SM_CXEDGE))


void PatRect(CDC* pDC,int x,int y,int dx,int dy, BOOL bVert, COLORREF clr)
{
	if (bVert)
	{
		pDC->FillSolidRect(y, x, dy, dx, clr);
	}
	else
	{
		pDC->FillSolidRect(x, y, dx, dy, clr);
	}
}

void CXTPSkinObjectTrackBar::DrawTic(CDC* pDC, int x, int y, int dir, COLORREF clr)
{
	if (dir == -1) y -= TICKHEIGHT;
	PatRect(pDC,x,y,1,TICKHEIGHT, GetStyle() & TBS_VERT, clr);
}

int CXTPSkinObjectTrackBar::LogToPhys(DWORD dwPos)
{
	const RECT& rc = m_drawRect.rc;

	int lLogMin = (int)SendMessage(TBM_GETRANGEMIN);
	int lLogMax = (int)SendMessage(TBM_GETRANGEMAX);
	int iSizePhys = max(1, rc.right - rc.left);

	int x;
	x = rc.left;
	if (lLogMax == lLogMin)
		return x;

	return (int)MulDiv(dwPos - lLogMin, iSizePhys - 1, lLogMax - lLogMin) + x;
}

void ValidateThumbHeight(DWORD dwStyle, int& iThumbHeight, int& iThumbWidth)
{
	if (iThumbHeight < MIN_THUMB_HEIGHT)
		iThumbHeight = MIN_THUMB_HEIGHT;

	iThumbWidth = iThumbHeight / 2;
	iThumbWidth |= 0x01;

	if (dwStyle & TBS_ENABLESELRANGE)
	{
		if (dwStyle & TBS_FIXEDLENGTH)
		{
			iThumbWidth = (iThumbHeight * 9) / 20;
			iThumbWidth |= 0x01;
		}
		else
		{
			iThumbHeight += (iThumbWidth * 2) / 9;
		}
	}
}

void CXTPSkinObjectTrackBar::FillDrawRect()
{
	CXTPClientRect rc(this);
	DWORD dwStyle = GetStyle();

	if (dwStyle & TBS_VERT)
	{
		swap(rc.left, rc.top);
		swap(rc.right, rc.bottom);
	}
	int iThumbHeight = 0, iThumbWidth = 0;

	if (dwStyle & TBS_FIXEDLENGTH)
	{
		iThumbHeight = (int)SendMessage(TBM_GETTHUMBLENGTH);
		ValidateThumbHeight(dwStyle, iThumbHeight, iThumbWidth);
	}
	else
	{
		iThumbHeight = (GetSystemMetrics(SM_CYHSCROLL) * 4) / 3;

		if ((iThumbHeight > MIN_THUMB_HEIGHT) && (rc.bottom < (int)iThumbHeight))
		{
			iThumbHeight = rc.bottom - 3 * GetSystemMetrics(SM_CYEDGE); // top, bottom, and tic
			if (dwStyle & TBS_ENABLESELRANGE)
				iThumbHeight = iThumbHeight * 3 / 4;

			ValidateThumbHeight(dwStyle, iThumbHeight, iThumbWidth);
		}


		ValidateThumbHeight(dwStyle, iThumbHeight, iThumbWidth);
	}


	if (dwStyle & (TBS_BOTH | TBS_TOP) && !(GetStyle() & TBS_NOTICKS))
		rc.top += TICKHEIGHT + BORDERSIZE + 3;

	rc.top   += BORDERSIZE;
	rc.bottom  = rc.top + iThumbHeight;
	rc.left   += (iThumbWidth + BORDERSIZE);
	rc.right  -= (iThumbWidth + BORDERSIZE);

	m_drawRect.rc = rc;
	m_drawRect.iThumbHeight = iThumbHeight;
	m_drawRect.iThumbWidth = iThumbWidth;
	m_drawRect.iSizePhys = max(1, rc.right - rc.left);
	m_drawRect.dwStyle = dwStyle;
}

// dir = direction multiplier (drawing up or down)
// yTic = where (vertically) to draw the line of tics
void CXTPSkinObjectTrackBar::DrawTicsOneLine(CDC* pDC, int dir, int yTic)
{
	const RECT& rc = m_drawRect.rc;

	BOOL bVert = GetStyle() & TBS_VERT;
	CXTPSkinManagerClass* pClass = GetSkinClass();

	COLORREF clrTick = pClass->GetThemeColor(bVert ? TKP_TICSVERT : TKP_TICS, 1,  TMT_COLOR, GetColor(COLOR_BTNTEXT));

	DrawTic(pDC, rc.left, yTic, dir, clrTick);
	DrawTic(pDC, rc.left, yTic + (dir * 1), dir, clrTick);
	DrawTic(pDC, rc.right-1, yTic, dir, clrTick);
	DrawTic(pDC, rc.right-1, yTic+ (dir * 1), dir, clrTick);

	PDWORD pTics = (PDWORD)SendMessage(TBM_GETPTICS);
	int    iPos;
	int    i;

	int nTics = pTics ? (int)LocalSize((HANDLE)pTics) / sizeof(DWORD) : 0;


	// those inbetween
	if (m_nTickFreq && pTics)
	{
		for (i = 0; i < nTics; ++i)
		{
			if (((i + 1) % m_nTickFreq) == 0)
			{
				iPos = LogToPhys(pTics[i]);
				DrawTic(pDC, iPos, yTic, dir, clrTick);
			}
		}
	}

	int lSelStart = (int)SendMessage(TBM_GETSELSTART);
	int lSelEnd = (int)SendMessage(TBM_GETSELEND);
	int lLogMin = (int)SendMessage(TBM_GETRANGEMIN);

	// draw the selection range (triangles)

	if ((GetStyle() & TBS_ENABLESELRANGE) &&
		(lSelStart < lSelEnd) && (lSelEnd >= lLogMin))
	{

		pDC->SetBkColor(clrTick);

		iPos = LogToPhys(lSelStart);

		for (i = 0; i < TICKHEIGHT; i++)
			PatRect(pDC,iPos-i,yTic+(dir==1 ? i : -TICKHEIGHT),
					1,TICKHEIGHT-i, GetStyle() & TBS_VERT, GetColor(COLOR_BTNTEXT));

		iPos = LogToPhys(lSelEnd);

		for (i = 0; i < TICKHEIGHT; i++)
			PatRect(pDC,iPos+i,yTic+(dir==1 ? i : -TICKHEIGHT),
					1,TICKHEIGHT-i, GetStyle() & TBS_VERT, GetColor(COLOR_BTNTEXT));
	}
}

void CXTPSkinObjectTrackBar::DrawTics(CDC* pDC)
{
	const RECT& rc = m_drawRect.rc;
	DWORD dwStyle = GetStyle();

	if (dwStyle & TBS_NOTICKS)
		return;

	if ((dwStyle & TBS_BOTH) || !(dwStyle & TBS_TOP))
	{
		DrawTicsOneLine(pDC, 1, rc.bottom + 1);
	}

	if ((dwStyle & (TBS_BOTH | TBS_TOP)))
	{
		DrawTicsOneLine(pDC, -1, rc.top - 1);
	}
}

void CXTPSkinObjectTrackBar::DrawChannel(CDC* pDC)
{
	RECT rcChannel;
	::SendMessage(m_hWnd, TBM_GETCHANNELRECT, 0, (LPARAM)&rcChannel);

	BOOL bVert = m_drawRect.dwStyle & TBS_VERT;

	if (bVert)
	{
		swap(rcChannel.left, rcChannel.top);
		swap(rcChannel.right, rcChannel.bottom);
	}

	CXTPSkinManagerClass* pClass = GetSkinClass();

	pClass->DrawThemeBackground(pDC, bVert ? TKP_TRACKVERT: TKP_TRACK, 1, &rcChannel);
}

void CXTPSkinObjectTrackBar::DrawThumb(CDC* pDC)
{
	DWORD dwStyle =  m_drawRect.dwStyle;

	if (dwStyle & TBS_NOTHUMB)
		return;

	BOOL bVert = dwStyle & TBS_VERT;
	CXTPSkinManagerClass* pClass = GetSkinClass();

	RECT rcThumb;
	::SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)&rcThumb);

	int nPartId = bVert ? TKP_THUMBVERT: TKP_THUMB;

	if (!(dwStyle & TBS_BOTH))
	{
		if (!bVert)
			nPartId = dwStyle & TBS_TOP ? TKP_THUMBTOP : TKP_THUMBBOTTOM;
		else
			nPartId = dwStyle & TBS_TOP ? TKP_THUMBLEFT : TKP_THUMBRIGHT;
	}

	int nStateId = TUS_NORMAL;
	if (m_bHotThumb)
		nStateId = TUS_HOT;
	if (::GetCapture() == m_hWnd)
		nStateId = TUS_PRESSED;
	else if (::GetFocus() == m_hWnd && ((SendMessage(WM_QUERYUISTATE) & UISF_HIDEFOCUS) == 0))
		nStateId = TUS_FOCUSED;
	if (!IsWindowEnabled())
		nStateId = TUS_DISABLED;

	pClass->DrawThemeBackground(pDC, nPartId , nStateId, &rcThumb);
}

LRESULT CXTPSkinObjectTrackBar::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0)
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectTrackBar::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this);
	OnDraw(&dc);
}

void CXTPSkinObjectTrackBar::OnDraw(CDC* pDC)
{
	CXTPClientRect rc(this);

	CXTPBufferDC dcMem(*pDC, rc);

	NMCUSTOMDRAW nm;
	ZeroMemory(&nm, sizeof(NMCUSTOMDRAW));
	nm.hdc = dcMem.GetSafeHdc();
	nm.dwDrawStage = CDDS_PREPAINT;
	nm.hdr.code = NM_CUSTOMDRAW;
	nm.hdr.hwndFrom = GetSafeHwnd();
	nm.hdr.idFrom = GetDlgCtrlID();

	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&nm);

	FillBackground(&dcMem, rc);


	if (::GetFocus() == m_hWnd && ((SendMessage(WM_QUERYUISTATE) & UISF_HIDEFOCUS) == 0))
	{
		dcMem.SetBkColor(GetColor(COLOR_BTNHIGHLIGHT));

		DrawFocusRect(dcMem, &rc);
	}

	FillDrawRect();

	DrawTics(&dcMem);

	DrawChannel(&dcMem);

	DrawThumb(&dcMem);

}

LRESULT CXTPSkinObjectTrackBar::OnSetTicFreq(WPARAM wParam, LPARAM)
{
	m_nTickFreq = (int)wParam;

	return Default();
}


void CXTPSkinObjectTrackBar::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT rcThumb;
	::SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)&rcThumb);

	BOOL bHotThumb = ::PtInRect(&rcThumb, point);

	if (bHotThumb != m_bHotThumb)
	{
		m_bHotThumb = bHotThumb;
		Invalidate(FALSE);

		if (m_bHotThumb)
		{
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
		}
	}

	CXTPSkinObjectFrame::OnMouseMove(nFlags, point);
}

void CXTPSkinObjectTrackBar::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectTrackBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPSkinObjectFrame::OnLButtonDown(nFlags, point);

}

void CXTPSkinObjectTrackBar::OnLButtonUp(UINT nFlags, CPoint point)
{

	CXTPSkinObjectFrame::OnLButtonUp(nFlags, point);
}
