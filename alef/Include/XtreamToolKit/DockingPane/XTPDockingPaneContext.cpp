// XTPDockingPaneContext.cpp : implementation of the CXTPDockingPaneContext class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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
#include "Common/XTPResourceManager.h"
#include "TabManager/XTPTabManager.h"

#include "XTPDockingPaneContext.h"
#include "XTPDockingPane.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneTabbedContainer.h"
#include "XTPDockingPaneSplitterContainer.h"
#include "XTPDockingPaneMiniWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void AFX_CDECL CXTPDockingPaneContext::AdjustRectangle(CRect& rect, CPoint pt)
{
	int nXOffset = (pt.x < rect.left) ? (pt.x - rect.left) :
					(pt.x > rect.right) ? (pt.x - rect.right) : 0;
	int nYOffset = (pt.y < rect.top) ? (pt.y - rect.top) :
					(pt.y > rect.bottom) ? (pt.y - rect.bottom) : 0;
	rect.OffsetRect(nXOffset, nYOffset);
}

void AFX_CDECL CXTPDockingPaneContext::AdjustCursor(CPoint& pt)
{
	CRect rc = XTPMultiMonitor()->GetWorkArea(pt);
	if (pt.x < rc.left) pt.x = rc.left;
	if (pt.x > rc.right) pt.x = rc.right;
	if (pt.y < rc.top) pt.y = rc.top;
	if (pt.y > rc.bottom) pt.y = rc.bottom;
}

typedef BOOL (WINAPI *PFNSETLAYEREDWINDOWATTRIBUTES) (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

#ifndef LWA_ALPHA
#define LWA_ALPHA               0x00000002
#endif

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED           0x00080000
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContextAlphaWnd

CXTPDockingPaneContextAlphaWnd::CXTPDockingPaneContextAlphaWnd()
{
}

CXTPDockingPaneContextAlphaWnd::~CXTPDockingPaneContextAlphaWnd()
{
}


BEGIN_MESSAGE_MAP(CXTPDockingPaneContextAlphaWnd, CWnd)
	//{{AFX_MSG_MAP(CXTPDockingPaneContextAlphaWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContextAlphaWnd message handlers

BOOL CXTPDockingPaneContextAlphaWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPDockingPaneContextAlphaWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	dc.FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT));
}

//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContextStickerWnd

CXTPDockingPaneContextStickerWnd::CXTPDockingPaneContextStickerWnd(CXTPDockingPaneContext* pContext)
	: m_pContext(pContext)
{
	m_typeSticker = m_selectedSticker = xtpPaneStickerNone;
}

CXTPDockingPaneContextStickerWnd::~CXTPDockingPaneContextStickerWnd()
{
}


BEGIN_MESSAGE_MAP(CXTPDockingPaneContextStickerWnd, CWnd)
	//{{AFX_MSG_MAP(CXTPDockingPaneContextStickerWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContextAlphaWnd message handlers

BOOL CXTPDockingPaneContextStickerWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

struct CXTPDockingPaneContextStickerWnd::SPRITEINFO
{
	SPRITEINFO(int x, int y, int left, int top, int cx, int cy)
	{
		ptDest = CPoint(x, y);
		rcSrc.SetRect(left, top, left + cx, top + cy);
	}
	CPoint ptDest;
	CRect rcSrc;
};

#define SPRITE_STICKER_TOP              0
#define SPRITE_STICKER_LEFT             1
#define SPRITE_STICKER_BOTTOM           2
#define SPRITE_STICKER_RIGHT            3
#define SPRITE_STICKER_TOP_SELECTED     4
#define SPRITE_STICKER_LEFT_SELECTED    5
#define SPRITE_STICKER_BOTTOM_SELECTED  6
#define SPRITE_STICKER_RIGHT_SELECTED   7

#define SPRITE_STICKER_CENTER           8
#define SPRITE_STICKER_CENTER_SELECTED  9
#define SPRITE_STICKER_CLIENT           10

static CXTPDockingPaneContextStickerWnd::SPRITEINFO arrSpritesStyckerWidbey[] =
{
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(25, 0, 0, 0, 43, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 25, 30, 33, 30, 43),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(25, 63, 43, 0, 43, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(63, 25, 0, 33, 30, 43),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(25, 0, 0, 76, 43, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 25, 90, 33, 30, 43),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(25, 63, 43, 76, 43, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(63, 25, 60, 33, 30, 43),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 30, 86, 0, 33, 33),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 30, 86, 76, 33, 33),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(25, 25, 0, 0, 43, 43),
};

static CXTPDockingPaneContextStickerWnd::SPRITEINFO arrSpritesStyckerVisualStudio2005[] =
{
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 0, 61, 29, 29, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 30, 90, 30, 30, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 59, 91, 0, 29, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(60, 30, 62, 0, 30, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 0, 61, 29 + 61, 29, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 30, 90, 30 + 61, 30, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 59, 91, 0 + 61, 29, 30),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(60, 30, 62, 0 + 61, 30, 29),

	CXTPDockingPaneContextStickerWnd::SPRITEINFO(30, 30, 120, 82, 28, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(24, 24, 120, 41, 41, 41),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(24, 24, 120, 0, 41, 41),

	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 0, 29, 29, 32),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 29, 32, 32, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 32, 0, 29, 32),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 0, 0, 32, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 0, 29 + 61, 29, 32),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 29, 32 + 61, 32, 29),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 32, 0 + 61, 29, 32),
	CXTPDockingPaneContextStickerWnd::SPRITEINFO(0, 0, 0, 0 + 61, 32, 29),

};


void CXTPDockingPaneContextStickerWnd::DrawTransparent(CDC* pDC , const CPoint& ptDest, const CSize& sz, CBitmap* pBitmap)
{
	CImageList il;
	il.Create(sz.cx, sz.cy, ILC_COLOR24 | ILC_MASK, 0, 1);
	il.Add(pBitmap, RGB(0, 0xFF, 0));

	il.Draw(pDC, 0, ptDest, ILD_NORMAL);
}

void CXTPDockingPaneContextStickerWnd::DrawSprite(CDC* pDC, UINT nID, SPRITEINFO* pSpriteInfo, BOOL bClientBitmap)
{
	CBitmap bmp;
	VERIFY(XTPResourceManager()->LoadBitmap(&bmp, nID));

	CSize sz(pSpriteInfo->rcSrc.Width(), pSpriteInfo->rcSrc.Height());

	CBitmap bmpSprite;
	bmpSprite.CreateCompatibleBitmap(pDC, sz.cx, sz.cy);

	if (bmpSprite.GetSafeHandle())
	{
		CXTPCompatibleDC dcSprite(pDC, &bmpSprite);
		CXTPCompatibleDC dc(pDC, &bmp);
		dcSprite.BitBlt(0, 0, sz.cx, sz.cy, &dc, pSpriteInfo->rcSrc.left, pSpriteInfo->rcSrc.top, SRCCOPY);
	}

	CPoint ptDest = bClientBitmap ? pSpriteInfo->ptDest : CPoint(0, 0);

	DrawTransparent(pDC, ptDest, sz, &bmpSprite);
}


void CXTPDockingPaneContextStickerWnd::OnDraw(CDC* pDC)
{
	XTPCurrentSystemTheme theme = XTPColorManager()->GetCurrentSystemTheme();
	BOOL bClient = (m_typeSticker & xtpPaneStickerClient) == xtpPaneStickerClient;

	if (m_pContext->GetStickerStyle() == xtpPaneStickerStyleWhidbey)
	{
		UINT nIDBitmap = theme == xtpSystemThemeOlive ? XTP_IDB_DOCKINGPANE_STICKERS_OLIVE :
			theme == xtpSystemThemeSilver ? XTP_IDB_DOCKINGPANE_STICKERS_SILVER : XTP_IDB_DOCKINGPANE_STICKERS_BLUE;

		SPRITEINFO* sprites = arrSpritesStyckerWidbey;

		if (bClient)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKER_CLIENT, &sprites[SPRITE_STICKER_CLIENT]);
		if (m_typeSticker & xtpPaneStickerTop)
			DrawSprite(pDC, nIDBitmap, &sprites[m_selectedSticker == xtpPaneStickerTop ? SPRITE_STICKER_TOP_SELECTED : SPRITE_STICKER_TOP], bClient);
		if (m_typeSticker & xtpPaneStickerLeft)
			DrawSprite(pDC, nIDBitmap, &sprites[m_selectedSticker == xtpPaneStickerLeft ? SPRITE_STICKER_LEFT_SELECTED : SPRITE_STICKER_LEFT], bClient);
		if (m_typeSticker & xtpPaneStickerBottom)
			DrawSprite(pDC, nIDBitmap, &sprites[m_selectedSticker == xtpPaneStickerBottom ? SPRITE_STICKER_BOTTOM_SELECTED : SPRITE_STICKER_BOTTOM], bClient);
		if (m_typeSticker & xtpPaneStickerRight)
			DrawSprite(pDC, nIDBitmap, &sprites[m_selectedSticker == xtpPaneStickerRight ? SPRITE_STICKER_RIGHT_SELECTED : SPRITE_STICKER_RIGHT], bClient);
		if (m_typeSticker & xtpPaneStickerCenter)
			DrawSprite(pDC, nIDBitmap, &sprites[m_selectedSticker == xtpPaneStickerCenter ? SPRITE_STICKER_CENTER_SELECTED : SPRITE_STICKER_CENTER]);
	}
	else
	{
		SPRITEINFO* sprites = arrSpritesStyckerVisualStudio2005;
		int nClientSprite = bClient ? 0 : 11;

		if (bClient)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKERS_2005, &sprites[m_selectedSticker == xtpPaneStickerCenter ? SPRITE_STICKER_CENTER_SELECTED : SPRITE_STICKER_CLIENT]);
		if (m_typeSticker & xtpPaneStickerTop)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKERS_2005, &sprites[nClientSprite + (m_selectedSticker == xtpPaneStickerTop ? SPRITE_STICKER_TOP_SELECTED : SPRITE_STICKER_TOP)], bClient);
		if (m_typeSticker & xtpPaneStickerLeft)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKERS_2005, &sprites[nClientSprite + (m_selectedSticker == xtpPaneStickerLeft ? SPRITE_STICKER_LEFT_SELECTED : SPRITE_STICKER_LEFT)], bClient);
		if (m_typeSticker & xtpPaneStickerBottom)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKERS_2005, &sprites[nClientSprite + (m_selectedSticker == xtpPaneStickerBottom ? SPRITE_STICKER_BOTTOM_SELECTED : SPRITE_STICKER_BOTTOM)], bClient);
		if (m_typeSticker & xtpPaneStickerRight)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKERS_2005, &sprites[nClientSprite + (m_selectedSticker == xtpPaneStickerRight ? SPRITE_STICKER_RIGHT_SELECTED : SPRITE_STICKER_RIGHT)], bClient);
		if (m_typeSticker & xtpPaneStickerCenter)
			DrawSprite(pDC, XTP_IDB_DOCKINGPANE_STICKERS_2005, &sprites[SPRITE_STICKER_CENTER]);
	}
}


void CXTPDockingPaneContextStickerWnd::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	CXTPBufferDC dc(dcPaint, CXTPClientRect(this));

	OnDraw(&dc);
}

XTPDockingPaneStickerType CXTPDockingPaneContextStickerWnd::HitTest(CPoint pt)
{
	CXTPClientRect rc(this);
	ScreenToClient(&pt);

	if (!rc.PtInRect(pt))
		return xtpPaneStickerNone;

	CClientDC dcClient(this);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dcClient, rc.Width(), rc.Height());

	CXTPCompatibleDC dc(&dcClient, &bmp);
	dc.FillSolidRect(rc, 0);

	UINT nIDBitmap = m_pContext->GetStickerStyle() == xtpPaneStickerStyleVisualStudio2005 ?
		XTP_IDB_DOCKINGPANE_STICKERS_2005 : XTP_IDB_DOCKINGPANE_STICKERS_BLUE;
	UINT nIDBitmapClient = m_pContext->GetStickerStyle() == xtpPaneStickerStyleVisualStudio2005 ?
		XTP_IDB_DOCKINGPANE_STICKERS_2005 : XTP_IDB_DOCKINGPANE_STICKER_CLIENT;

	BOOL bClient = (m_typeSticker & xtpPaneStickerClient) == xtpPaneStickerClient;

	SPRITEINFO* sprites = m_pContext->GetStickerStyle() == xtpPaneStickerStyleWhidbey ?
		arrSpritesStyckerWidbey : arrSpritesStyckerVisualStudio2005;

	if (m_typeSticker & xtpPaneStickerTop)
	{
		DrawSprite(&dc, nIDBitmap, &sprites[SPRITE_STICKER_TOP], bClient);
		if (dc.GetPixel(pt) != 0)
			return xtpPaneStickerTop;
	}
	if (m_typeSticker & xtpPaneStickerLeft)
	{
		DrawSprite(&dc, nIDBitmap, &sprites[SPRITE_STICKER_LEFT], bClient);
		if (dc.GetPixel(pt) != 0)
			return xtpPaneStickerLeft;
	}
	if (m_typeSticker & xtpPaneStickerBottom)
	{
		DrawSprite(&dc, nIDBitmap, &sprites[SPRITE_STICKER_BOTTOM], bClient);
		if (dc.GetPixel(pt) != 0)
			return xtpPaneStickerBottom;
	}
	if (m_typeSticker & xtpPaneStickerRight)
	{
		DrawSprite(&dc, nIDBitmap, &sprites[SPRITE_STICKER_RIGHT], bClient);
		if (dc.GetPixel(pt) != 0)
			return xtpPaneStickerRight;
	}
	if (m_typeSticker & xtpPaneStickerCenter)
	{
		DrawSprite(&dc, nIDBitmapClient, &sprites[SPRITE_STICKER_CLIENT]);
		if (dc.GetPixel(pt) != 0)
			return xtpPaneStickerCenter;
	}

	return xtpPaneStickerNone;
}

//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContext

CXTPDockingPaneContext::CXTPDockingPaneContext()
{
	m_bUseAlphaContext = FALSE;
	m_bUseDockingStickers = FALSE;
	m_bDragKeyboard = FALSE;

	m_pStickerPane = NULL;
	m_pLastStickerPane = NULL;
	m_pManager = NULL;
	m_pPane = NULL;
	m_pContainer = NULL;
	m_bAttachLast = FALSE;
	m_bAttach = FALSE;
	m_bFloatable = TRUE;
	m_bDockable = TRUE;
	m_bDitherLast = FALSE;
	m_containDirection = (XTPDockingPaneDirection)-1;
	m_pDC = NULL;

	m_pfnSetLayeredWindowAttributes = NULL;

	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (hLib)
	{
		m_pfnSetLayeredWindowAttributes = (PVOID) ::GetProcAddress(hLib, "SetLayeredWindowAttributes");
	}

	m_bResetDC = FALSE;
}

CXTPDockingPaneContext::~CXTPDockingPaneContext()
{

}


void CXTPDockingPaneContext::Drag(CXTPDockingPaneBase* pPane, CPoint pt)
{
	m_bDragKeyboard = (pt == CPoint(-1, -1));
	CRect rect = pPane->GetPaneWindowRect();

	if (m_bDragKeyboard)
	{
		pt.x = rect.CenterPoint().x;
		pt.y = rect.top + 15;
		SetCursorPos(pt.x, pt.y);
	}

	m_ptSticky = m_ptLast = pt;

	AdjustRectangle(rect, pt);

	m_pPane = pPane;
	m_rectDragFrameScreen = m_rectDragFrame = rect;
	m_pDC = 0;
	m_pContainer = 0;
	m_bAttach = m_bAttachLast = FALSE;
	m_bFloatable = TRUE;
	m_bDockable = TRUE;

	m_bUseAlphaContext = m_pManager->IsAlphaDockingContext() && (m_pfnSetLayeredWindowAttributes != NULL);
	m_bUseDockingStickers = m_pManager->IsShowDockingContextStickers() && m_bUseAlphaContext;


	CXTPDockingPaneBaseList lst;
	pPane->FindPane(xtpPaneTypeDockingPane, &lst);
	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pListPane = (CXTPDockingPane*)lst.GetNext(pos);

		if (m_pManager->_OnAction(xtpPaneActionFloating, pListPane))
			m_bFloatable = FALSE;

		if (pListPane->GetOptions() & xtpPaneNoFloatable)
			m_bFloatable = FALSE;

		if (pListPane->GetOptions() & xtpPaneNoDockable)
			m_bDockable = FALSE;
	}

	if (m_pManager->m_bShowSizeCursorWhileDragging)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
	}


	InitLoop();

	Track();
}


void CXTPDockingPaneContext::InitLoop()
{
	m_rectLast.SetRectEmpty();
	m_sizeLast.cx = m_sizeLast.cy = 0;
	m_bDitherLast = FALSE;
	m_rectStickerPane.SetRectEmpty();
	m_pStickerPane = NULL;
	m_pLastStickerPane = NULL;
	m_bResetDC = FALSE;

	if (!m_bUseAlphaContext)
	{
		// handle pending WM_PAINT messages
		MSG msg;
		while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
				return;
			DispatchMessage(&msg);
		}

		// lock window update while dragging
		ASSERT(m_pDC == NULL);
		CWnd* pWnd = CWnd::GetDesktopWindow();
		if (pWnd->LockWindowUpdate())
			m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
		else
			m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE);
		ASSERT(m_pDC != NULL);
	}
}
void CXTPDockingPaneContext::CancelLoop()
{
	ReleaseCapture();

	if (m_bUseAlphaContext)
	{
		m_wndContext.DestroyWindow();
		m_wndAttachedTab.DestroyWindow();

		DestroyDockingStickers();

		POSITION pos = m_rgnStickers.GetStartPosition();
		while (pos)
		{
			UINT nKey;
			CRgn* pRgn;
			m_rgnStickers.GetNextAssoc(pos, nKey, pRgn);
			delete pRgn;
		}
		m_rgnStickers.RemoveAll();

	}
	else
	{
		DrawFocusRect(TRUE);    // gets rid of focus rect

		CWnd* pWnd = CWnd::GetDesktopWindow();
		pWnd->UnlockWindowUpdate();
		if (m_pDC != NULL)
		{
			pWnd->ReleaseDC(m_pDC);
			m_pDC = NULL;
		}
	}
}

void CXTPDockingPaneContext::_CreateRectangleRgn(CRgn& rgnResult, CRect rc)
{
	CRgn rgnOutside, rgnInside;
	rgnOutside.CreateRectRgnIndirect(&rc);
	CRect rect = rc;
	rect.DeflateRect(4, 4);
	rect.IntersectRect(rect, rc);
	rgnInside.CreateRectRgnIndirect(rect);
	rgnResult.CreateRectRgn(0, 0, 0, 0);
	rgnResult.CombineRgn(&rgnOutside, &rgnInside, RGN_XOR);
}

void CXTPDockingPaneContext::_CreateRgn(CRgn& rgnResult, CRect rc, BOOL bTabbedRgn, BOOL bRemove)
{
	if (bRemove)
	{
		rgnResult.CreateRectRgn(0, 0, 0, 0);
		return;
	}

	if (bTabbedRgn)
	{
		CSize szTab(min(50, rc.Width() - 5), min(20, rc.Height() / 2));
		CRect rcIntersect, rcTop(rc.left, rc.top, rc.right, rc.bottom - szTab.cy),
			rcBottom(rc.left + 5, rc.bottom - szTab.cy - 4, rc.left + 5 + szTab.cx, rc.bottom);

		CRgn rgnTop, rgnBottom, rgnIntersect;

		rcIntersect.IntersectRect(rcTop, rcBottom);
		rcIntersect.DeflateRect(4, 0);

		_CreateRectangleRgn(rgnTop, rcTop);
		_CreateRectangleRgn(rgnBottom, rcBottom);
		_CreateRectangleRgn(rgnIntersect, rcIntersect);

		rgnResult.CreateRectRgn(0, 0, 0, 0);
		rgnResult.CombineRgn(&rgnBottom, &rgnTop, RGN_OR);
		rgnResult.CombineRgn(&rgnResult, &rgnIntersect, RGN_XOR);
	}
	else
	{
		_CreateRectangleRgn(rgnResult, rc);
	}
}

void CXTPDockingPaneContext::DrawFocusRect(BOOL bRemoveRect)
{
	if (m_pManager->m_bShowContentsWhileDragging && m_pContainer == NULL &&
		m_bFloatable && !m_bAttach && !m_rectDragFrame.IsRectEmpty())
	{
		CRect rect = m_rectDragFrame;

		m_wndAttachedTab.DestroyWindow();
		m_wndContext.DestroyWindow();

		if (!m_bUseAlphaContext && !m_bResetDC)
		{
			ASSERT(m_pDC != NULL);
			// determine new rect and size

			rect.SetRectEmpty();

			// first, determine the update region and select it
			CRgn rgnNew, rgnLast, rgnUpdate;

			_CreateRgn(rgnNew, rect, m_bAttach, TRUE);
			_CreateRgn(rgnLast, m_rectLast, m_bAttachLast);

			rgnUpdate.CreateRectRgn(0, 0, 0, 0);
			rgnUpdate.CombineRgn(&rgnLast, &rgnNew, RGN_XOR);

			// draw into the update/new region
			m_pDC->SelectClipRgn(&rgnUpdate);
			m_pDC->GetClipBox(&rect);
			CBrush* pBrushOld = m_pDC->SelectObject(CDC::GetHalftoneBrush());
			m_pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
			// cleanup DC

			if (pBrushOld != NULL)
				m_pDC->SelectObject(pBrushOld);

			m_pDC->SelectClipRgn(NULL);

			CWnd* pWnd = CWnd::GetDesktopWindow();
			pWnd->UnlockWindowUpdate();
			if (m_pDC != NULL)
			{
				pWnd->ReleaseDC(m_pDC);
				m_pDC = NULL;
			}
			m_bResetDC = TRUE;
		}

		if (m_pPane->GetType() == xtpPaneTypeSplitterContainer)
		{
			if (m_rectDragFrame != m_rectLast)
			{
				m_pPane->GetDockingSite()->MoveWindow(m_rectDragFrame);
			}
		}
		else
		{
			XTPDockingPaneAction actionNotify = xtpPaneActionFloated;

			CXTPDockingPaneBaseList lst;
			m_pPane->FindPane(xtpPaneTypeDockingPane, &lst);

			if (m_rectDragFrame.top < 0)
				m_rectDragFrame.OffsetRect(0, -m_rectDragFrame.top);

			CXTPDockingPaneMiniWnd* pMiniWnd = m_pManager->FloatPane(m_pPane, m_rectDragFrame);

			m_pManager->RecalcFrameLayout(m_pPane);
			SAFE_CALLPTR(pMiniWnd, RecalcLayout());

			m_pPane = pMiniWnd->GetTopPane();

			POSITION pos = lst.GetHeadPosition();
			while (pos)
			{
				CXTPDockingPane* pPane = (CXTPDockingPane*)lst.GetNext(pos);
				m_pManager->_OnAction(actionNotify, pPane);
			}

			AfxGetThread()->PumpMessage();
			AfxGetThread()->OnIdle(0);
		}

		m_rectLast = rect;
		m_bAttachLast = FALSE;

		return;
	}

	if (m_bResetDC)
	{
		ASSERT(m_pDC == NULL);
		ASSERT(m_bUseAlphaContext == FALSE);

		// handle pending WM_PAINT messages
		MSG msg;
		while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
				return;
			DispatchMessage(&msg);
		}

		// lock window update while dragging
		CWnd* pWnd = CWnd::GetDesktopWindow();
		if (pWnd->LockWindowUpdate())
			m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
		else
			m_pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE);
		ASSERT(m_pDC != NULL);
		m_bResetDC = FALSE;

		m_sizeLast.cx = m_sizeLast.cy = 0;
		m_bDitherLast = FALSE;
		m_rectLast.SetRectEmpty();
	}

	if (m_bUseAlphaContext)
	{
		ASSERT(bRemoveRect == FALSE);

		CRect rect = (m_pContainer == 0) ? m_rectDragFrame : m_rectContainer;

		m_rectLast = rect;

		if (!m_bFloatable && m_pContainer == 0)
		{
			rect.SetRectEmpty();
		}

		if (m_bAttach != m_bAttachLast)
		{
			m_bAttachLast = m_bAttach;
			if (m_bAttach)
				CreateContextWindow(&m_wndAttachedTab);
			else
				m_wndAttachedTab.DestroyWindow();
		}

		if (m_bAttach)
		{
			CSize szTab(min(50, rect.Width() - 5), min(20, rect.Height() / 2));
			CRect rcTab;

			if (m_pManager->GetPaintManager()->GetTabPaintManager()->GetPosition() == xtpTabPositionTop
				&& m_pContainer && m_pContainer->GetType() == xtpPaneTypeTabbedContainer)
			{
				if (((CXTPDockingPaneTabbedContainer*)m_pContainer)->IsTitleVisible())
				{
					rect.top += m_pManager->GetPaintManager()->GetCaptionHeight() + 3;
				}

				rect.top += szTab.cy;
				rcTab = CRect(rect.left + 5, rect.top - szTab.cy, rect.left + 5 + szTab.cx, rect.top);
			}
			else
			{
				rect.bottom -= szTab.cy;
				rcTab = CRect(rect.left + 5, rect.bottom, rect.left + 5 + szTab.cx, rect.bottom + szTab.cy);
			}

			m_wndAttachedTab.SetWindowPos(0, rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
		}

		if (m_wndContext.GetSafeHwnd() == 0 || CXTPWindowRect(&m_wndContext).Size() != rect.Size())
		{
			m_wndContext.DestroyWindow();
			CreateContextWindow(&m_wndContext);
		}

		m_wndContext.SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
	else
	{
		ASSERT(m_pDC != NULL);
		// determine new rect and size
		CRect rect = (m_pContainer == 0) ? m_rectDragFrame : m_rectContainer;

		if (!m_bFloatable && m_pContainer == 0)
			rect.SetRectEmpty();

		// first, determine the update region and select it
		CRgn rgnNew, rgnLast, rgnUpdate;

		_CreateRgn(rgnNew, rect, m_bAttach, bRemoveRect);
		_CreateRgn(rgnLast, m_rectLast, m_bAttachLast);

		rgnUpdate.CreateRectRgn(0, 0, 0, 0);
		rgnUpdate.CombineRgn(&rgnLast, &rgnNew, RGN_XOR);

		m_rectLast = rect;
		m_bAttachLast = m_bAttach;

		// draw into the update/new region
		m_pDC->SelectClipRgn(&rgnUpdate);
		m_pDC->GetClipBox(&rect);
		CBrush* pBrushOld = m_pDC->SelectObject(CDC::GetHalftoneBrush());
		m_pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
		// cleanup DC

		if (pBrushOld != NULL)
			m_pDC->SelectObject(pBrushOld);

		m_pDC->SelectClipRgn(NULL);
	}
}

BOOL CXTPDockingPaneContext::CanDock(CRect rc, CPoint pt, CXTPDockingPaneBase* pPane, BOOL bInside)
{
	double dSize = 20.0;

	double dRatio = 1.0;

	if (bInside && !m_bFloatable)
	{
		if (rc.Width() > 1 && rc.Height() > 1) dRatio = (double)rc.Height() / (double)rc.Width();
		dSize = 32000.0;
	}

	if (rc.Height() == 0 || rc.Width() == 0)
		return FALSE;

	if (bInside && !rc.PtInRect(pt))
		return FALSE;

	if (!bInside && !CRect(rc.left - 20, rc.top - 20, rc.right + 20, rc.bottom + 20).PtInRect(pt))
		return FALSE;

	int nInside = bInside ? 1 : -1;
	BOOL bFound = FALSE;


	if ((double)abs(pt.y - rc.top) < dSize && nInside * (pt.y - rc.top) >= 0)
	{
		m_containDirection = xtpPaneDockTop;
		dSize = (double)abs(pt.y - rc.top);
		bFound = TRUE;
	}

	if ((double)abs(rc.bottom - pt.y) < dSize && nInside * (rc.bottom - pt.y) >= 0)
	{
		m_containDirection = xtpPaneDockBottom;
		dSize = (double)abs(rc.bottom - pt.y);
		bFound = TRUE;
	}

	if ((double)abs(pt.x - rc.left) * dRatio < dSize && nInside * (pt.x - rc.left) >= 0)
	{
		m_containDirection = xtpPaneDockLeft;
		dSize = (double)abs(pt.x - rc.left) * dRatio;
		bFound = TRUE;
	}

	if ((double)abs(rc.right - pt.x) * dRatio < dSize && nInside * (rc.right- pt.x) >= 0)
	{
		m_containDirection = xtpPaneDockRight;
		bFound = TRUE;
	}

	if (bFound)
	{
		if (!IsAllowDockingTo(pPane, m_containDirection))
			return FALSE;

		m_rectContainer = m_pManager->_CalculateResultDockingRect(m_pPane, m_containDirection, pPane);
		m_pContainer = pPane;
	}
	return bFound;
}

BOOL CXTPDockingPaneContext::IsAllowDockingTo(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction)
{
	CXTPDockingPaneBaseList lst;
	m_pPane->FindPane(xtpPaneTypeDockingPane, &lst);
	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* p = (CXTPDockingPane*)lst.GetNext(pos);

		if (m_pManager->_OnAction(xtpPaneActionDocking, p, pPane, direction))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CXTPDockingPaneContext::IsAllowAttachTo(CXTPDockingPaneBase* pPane)
{

	CXTPDockingPaneBaseList lst;
	m_pPane->FindPane(xtpPaneTypeDockingPane, &lst);
	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* p = (CXTPDockingPane*)lst.GetNext(pos);

		if (m_pManager->_OnAction(xtpPaneActionAttaching, p, pPane))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CXTPDockingPaneContext::IsBehind(CXTPDockingPaneBase* pPaneBase, CXTPDockingPaneBase* pPaneTest)
{
	CWnd* pFrameBase = pPaneBase->GetDockingSite();
	if (!pFrameBase || !pFrameBase->IsKindOf(RUNTIME_CLASS(CXTPDockingPaneMiniWnd)))
		return TRUE;

	CWnd* pFrameTest = pPaneTest->GetDockingSite();
	if (!pFrameTest || !pFrameTest->IsKindOf(RUNTIME_CLASS(CXTPDockingPaneMiniWnd)))
		return FALSE;

	HWND hWndNext = ::GetWindow(pFrameBase->GetSafeHwnd(), GW_HWNDNEXT);

	while (hWndNext)
	{
		if (hWndNext == pFrameTest->GetSafeHwnd())
			return FALSE;

		hWndNext = ::GetWindow(hWndNext, GW_HWNDNEXT);
	}

	return TRUE;
}

void CXTPDockingPaneContext::FindContainer(CPoint pt)
{
	CXTPDockingPaneInfoList* pList = &m_pManager->GetPaneList();

	CXTPDockingPaneBase* pFloatingPane = NULL;

	POSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pNextPane = pList->GetNext(pos);

		CXTPDockingPaneBase* pPane = pNextPane->GetContainer();
		if (pPane == NULL)
			continue;

		ASSERT(pPane->GetType() == xtpPaneTypeTabbedContainer);

		if (pPane->GetContainer() == 0 || pPane->GetContainer()->GetType() != xtpPaneTypeSplitterContainer)
			continue;

		if (! (::GetWindowLong(((CXTPDockingPaneTabbedContainer*)pPane)->GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE))
			continue;

		if (m_pPane->ContainPane(pPane))
			continue;

		CRect rcClient = pPane->GetPaneWindowRect();

		if (rcClient.PtInRect(pt) && pNextPane->IsFloating())
		{
			if (pFloatingPane == NULL || IsBehind(pFloatingPane, pPane))
			{
				pFloatingPane = pPane;
			}
		}
	}

	pos = pList->GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pNextPane = pList->GetNext(pos);
		if (pNextPane->GetOptions() & xtpPaneNoDockable)
			continue;

		CXTPDockingPaneBase* pPane = pNextPane->GetContainer();
		if (pPane == NULL)
			continue;

		if (pFloatingPane != NULL && pFloatingPane != pPane)
			continue;

		ASSERT(pPane->GetType() == xtpPaneTypeTabbedContainer);

		if (pPane->GetContainer() == 0 || pPane->GetContainer()->GetType() != xtpPaneTypeSplitterContainer)
			continue;

		if (! (::GetWindowLong(((CXTPDockingPaneTabbedContainer*)pPane)->GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE))
			continue;

		CRect rcClient = m_rectContainer = pPane->GetPaneWindowRect();

		if (m_pPane->ContainPane(pPane))
			continue;

		if (((CXTPDockingPaneTabbedContainer*)pPane)->CanAttach(rcClient, pt))
		{
			if (IsAllowAttachTo(pPane))
			{
				m_pContainer = pPane;
				m_bAttach = TRUE;
				m_pStickerPane = 0;
				m_rectStickerPane.SetRectEmpty();
				return;
			}
			else
			{
				rcClient = m_rectContainer;
			}
		}

		if (m_bUseDockingStickers)
		{
			if (rcClient.PtInRect(pt))
			{
				if (m_pStickerPane == NULL || IsBehind(m_pStickerPane, pPane))
				{
					m_rectStickerPane = m_rectContainer;
					m_pStickerPane = pPane;
				}
			}
		}
		else
		{
			if (CanDock(rcClient, pt, pPane))
				return;
		}
	}

	if (pFloatingPane)
		return;

	if (m_bUseDockingStickers)
	{
		if (m_pStickerPane)
		{
			return;
		}

		if (m_rectStickerPane.IsRectEmpty())
		{
			CXTPDockingPaneBase* pPane = m_pManager->GetClientPane();
			if (pPane->GetPaneWindowRect().PtInRect(pt))
			{
				m_rectStickerPane = pPane->GetPaneWindowRect();
				m_pStickerPane = pPane;
				return;
			}

			pPane = m_pManager->GetTopPane();
			if (pPane->GetPaneWindowRect().PtInRect(pt))
			{
				m_rectStickerPane = pPane->GetPaneWindowRect();
			}
		}
	}
	else
	{
		CXTPDockingPaneBase* pPane = m_pManager->GetClientPane();
		m_rectContainer = pPane->GetPaneWindowRect();
		if (CanDock(pPane->GetPaneWindowRect(), pt, pPane))
			return;

		pPane = m_pManager->GetTopPane();
		m_rectContainer = pPane->GetPaneWindowRect();
		if (CanDock(pPane->GetPaneWindowRect(), pt, pPane, FALSE))
			return;
	}
}

void CXTPDockingPaneContext::EnsureVisible(CRect& rectDragFrame)
{
	CRect rcWork = XTPMultiMonitor()->GetWorkArea(rectDragFrame);
	int nGap = 10;

	if (rcWork.bottom - rectDragFrame.top < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.bottom - rectDragFrame.top - nGap);
	}
	if (rectDragFrame.bottom - rcWork.top < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.top - rectDragFrame.bottom + nGap);
	}
	if (rcWork.right - rectDragFrame.left < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.right - rectDragFrame.left - nGap, 0);
	}
	if (rectDragFrame.right - rcWork.left < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.left - rectDragFrame.right + nGap, 0);
	}
}

void CXTPDockingPaneContext::UpdateStickyFrame(CRect& rectDragFrame, CWnd* pHost)
{
	int nGap = m_pManager->m_nStickyGap;
	CXTPWindowRect rcWork(pHost);

	if (rectDragFrame.bottom < rcWork.top - nGap || rectDragFrame.top > rcWork.bottom + nGap)
		return;

	if (rectDragFrame.right < rcWork.left - nGap || rectDragFrame.left > rcWork.right + nGap)
		return;

	if (abs(rcWork.bottom - rectDragFrame.top) < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.bottom - rectDragFrame.top);
	}
	if (abs(rectDragFrame.bottom - rcWork.top) < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.top - rectDragFrame.bottom);
	}
	if (abs(rcWork.left - rectDragFrame.right) < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.left - rectDragFrame.right, 0);
	}
	if (abs(rectDragFrame.left - rcWork.right) < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.right - rectDragFrame.left, 0);
	}
	if (abs(rectDragFrame.left - rcWork.left) < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.left - rectDragFrame.left, 0);
	}
	if (abs(rectDragFrame.right - rcWork.right) < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.right - rectDragFrame.right, 0);
	}
	if (abs(rectDragFrame.bottom - rcWork.bottom) < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.bottom - rectDragFrame.bottom);
	}
	if (abs(rectDragFrame.top - rcWork.top) < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.top - rectDragFrame.top);
	}
}

void CXTPDockingPaneContext::UpdateStickyFrame(CRect& rectDragFrame)
{
	CRect rcWork = XTPMultiMonitor()->GetWorkArea(rectDragFrame);
	int nGap = m_pManager->m_nStickyGap;

	if (abs(rcWork.top - rectDragFrame.top) < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.top - rectDragFrame.top);
	}
	if (abs(rectDragFrame.bottom - rcWork.bottom) < nGap)
	{
		rectDragFrame.OffsetRect(0, rcWork.bottom - rectDragFrame.bottom);
	}
	if (abs(rcWork.right - rectDragFrame.right) < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.right - rectDragFrame.right, 0);
	}
	if (abs(rectDragFrame.left - rcWork.left) < nGap)
	{
		rectDragFrame.OffsetRect(rcWork.left - rectDragFrame.left, 0);
	}

	if ((m_pManager->GetSite()->GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)) == 0)
		UpdateStickyFrame(rectDragFrame, m_pManager->GetSite());

	CXTPDockingPaneBaseList* pList = &m_pManager->GetPaneStack();
	POSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pNextPane = pList->GetNext(pos);
		if (pNextPane->GetType() != xtpPaneTypeMiniWnd)
			continue;

		CXTPDockingPaneMiniWnd* pWnd = (CXTPDockingPaneMiniWnd*)pNextPane;
		if (pWnd->GetSafeHwnd() && pWnd->IsWindowVisible() && m_pPane->GetDockingSite() != pWnd)
		{
			UpdateStickyFrame(rectDragFrame, pWnd);
		}
	}
}

void CXTPDockingPaneContext::UpdateSizingStickyFrame(UINT nSide, CRect& rectDragFrame, CWnd* pHost)
{
	int nGap = m_pManager->m_nStickyGap;
	CXTPWindowRect rcWork(pHost);

	if (rectDragFrame.bottom < rcWork.top - nGap || rectDragFrame.top > rcWork.bottom + nGap)
		return;

	if (rectDragFrame.right < rcWork.left - nGap || rectDragFrame.left > rcWork.right + nGap)
		return;

	if (abs(rectDragFrame.top - rcWork.bottom) < nGap && (nSide == WMSZ_TOP || nSide == WMSZ_TOPLEFT || nSide == WMSZ_TOPRIGHT))
	{
		rectDragFrame.top = rcWork.bottom;
	}
	if (abs(rectDragFrame.bottom - rcWork.top) < nGap && (nSide == WMSZ_BOTTOM || nSide == WMSZ_BOTTOMLEFT || nSide == WMSZ_BOTTOMRIGHT))
	{
		rectDragFrame.bottom = rcWork.top;
	}
	if (abs(rectDragFrame.left - rcWork.left) < nGap && (nSide == WMSZ_LEFT || nSide == WMSZ_TOPLEFT || nSide == WMSZ_BOTTOMLEFT))
	{
		rectDragFrame.left = rcWork.left;
	}
	if (abs(rectDragFrame.left - rcWork.right) < nGap && (nSide == WMSZ_LEFT || nSide == WMSZ_TOPLEFT || nSide == WMSZ_BOTTOMLEFT))
	{
		rectDragFrame.left = rcWork.right;
	}
	if (abs(rectDragFrame.right - rcWork.left) < nGap  && (nSide == WMSZ_RIGHT || nSide == WMSZ_TOPRIGHT|| nSide == WMSZ_BOTTOMRIGHT))
	{
		rectDragFrame.right = rcWork.left;
	}
	if (abs(rectDragFrame.right - rcWork.right) < nGap  && (nSide == WMSZ_RIGHT || nSide == WMSZ_TOPRIGHT|| nSide == WMSZ_BOTTOMRIGHT))
	{
		rectDragFrame.right = rcWork.right;
	}
}

void CXTPDockingPaneContext::OnSizingFloatingFrame(CXTPDockingPaneMiniWnd* pMiniWnd, UINT nSide, LPRECT lpRect)
{
	if (!m_pManager->IsStickyFloatingFrames())
		return;

	CRect rectDragFrame(lpRect);

	if ((m_pManager->GetSite()->GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)) == 0)
		UpdateSizingStickyFrame(nSide, rectDragFrame, m_pManager->GetSite());

	CXTPDockingPaneBaseList* pList = &m_pManager->GetPaneStack();
	POSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pNextPane = pList->GetNext(pos);
		if (pNextPane->GetType() != xtpPaneTypeMiniWnd)
			continue;

		CXTPDockingPaneMiniWnd* pWnd = (CXTPDockingPaneMiniWnd*)pNextPane;
		if (pWnd->GetSafeHwnd() && pWnd->IsWindowVisible() && pMiniWnd != pWnd)
		{
			UpdateSizingStickyFrame(nSide, rectDragFrame, pWnd);
		}
	}

	*lpRect = rectDragFrame;
}

void CXTPDockingPaneContext::Move(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;

	m_rectDragFrameScreen.OffsetRect(ptOffset);

	if (CRect().IntersectRect(m_rectDragFrameScreen, XTPMultiMonitor()->GetWorkArea(m_rectDragFrame)))
	{
		m_rectDragFrame = m_rectDragFrameScreen;
	}

	EnsureVisible(m_rectDragFrame);

	if (!m_rectDragFrame.IsRectEmpty() && m_pManager->m_bStickyFloatingFrames)
	{
		UpdateStickyFrame(m_rectDragFrame);
	}


	m_pContainer = 0;
	m_bAttach = FALSE;
	m_rectStickerPane.SetRectEmpty();
	m_pStickerPane = NULL;

	if (GetKeyState(VK_CONTROL) >= 0 && m_bDockable)
		FindContainer(pt);

	m_ptLast = pt;

	if (m_bUseDockingStickers)
	{
		UpdateDockingStickers();

		BOOL bFound = FALSE;
		POSITION pos = m_lstStickers.GetTailPosition();
		while (pos)
		{

			CXTPDockingPaneContextStickerWnd* pSticker = m_lstStickers.GetPrev(pos);
			XTPDockingPaneStickerType selectedSticker = xtpPaneStickerNone;

			if (!bFound)
			{
				XTPDockingPaneStickerType ht = pSticker->HitTest(pt);
				if (ht != xtpPaneStickerNone)
				{
					if ((pSticker->m_typeSticker & xtpPaneStickerClient) == xtpPaneStickerClient)
					{
						m_pContainer = m_pStickerPane;
						m_rectContainer = m_rectStickerPane;
					}
					else
					{
						m_pContainer = m_pManager->GetTopPane();
						m_rectContainer = m_pContainer->GetPaneWindowRect();
					}

					switch (ht)
					{
					case xtpPaneStickerTop:
						m_rectContainer.bottom = m_rectContainer.CenterPoint().y;
						m_containDirection = xtpPaneDockTop;
						break;
					case xtpPaneStickerBottom:
						m_rectContainer.top = m_rectContainer.CenterPoint().y;
						m_containDirection = xtpPaneDockBottom;
						break;
					case xtpPaneStickerLeft:
						m_rectContainer.right = m_rectContainer.CenterPoint().x;
						m_containDirection = xtpPaneDockLeft;
						break;
					case xtpPaneStickerRight:
						m_rectContainer.left = m_rectContainer.CenterPoint().x;
						m_containDirection = xtpPaneDockRight;
						break;
					case xtpPaneStickerCenter:
						m_bAttach = TRUE;
						break;
					}

					BOOL bAllow = TRUE;

					if ((ht != xtpPaneStickerCenter) && !IsAllowDockingTo(m_pContainer, m_containDirection))
					{
						m_pContainer = NULL;
						bAllow = FALSE;
					}

					if ((ht == xtpPaneStickerCenter) && !IsAllowAttachTo(m_pContainer))
					{
						m_pContainer = NULL;
						m_bAttach = FALSE;
						bAllow = FALSE;
					}

					if (bAllow)
					{
						if (ht != xtpPaneStickerCenter)
							m_rectContainer = m_pManager->_CalculateResultDockingRect(m_pPane, m_containDirection, m_pContainer);

						selectedSticker = ht;
					}

					bFound = TRUE;
				}
			}
			if (pSticker->m_selectedSticker != selectedSticker)
			{
				pSticker->m_selectedSticker = selectedSticker;
				pSticker->Invalidate(FALSE);
			}
		}
	}


	DrawFocusRect();
}

void CXTPDockingPaneContext::Track()
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return;

	CWnd* pCapture = m_pManager->GetSite();
	ASSERT(pCapture);
	if (!pCapture)
		return;
	// set capture to the window which received this message
	pCapture->SetCapture();
	ASSERT(pCapture == CWnd::GetCapture());

	BOOL bAccept = FALSE;
	// get messages until capture lost or cancelled/accepted
	while (CWnd::GetCapture() == pCapture)
	{
		MSG msg;

		// handle pending WM_PAINT messages
		while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
				return;
			DispatchMessage(&msg);
			continue;
		}


		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		if (msg.message == WM_LBUTTONUP)
		{
			bAccept = TRUE;
			break;
		}
		else if (msg.message == WM_MOUSEMOVE)
		{
			CSize sz = m_ptSticky - CPoint(msg.pt);
			if (abs(sz.cx) > 4 || abs(sz.cy) > 4)
			{
				Move(msg.pt);
				m_ptSticky = CPoint(0, 0);
			}
		}
		else if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
		{
			if (m_bDragKeyboard && msg.message == WM_KEYDOWN)
			{
				if (msg.wParam == VK_RETURN)
				{
					bAccept = TRUE;
					break;
				}
				if (msg.wParam >= VK_LEFT && msg.wParam <= VK_DOWN)
				{
					CPoint pt(m_ptLast);
					if (msg.wParam == VK_LEFT)
						pt.x -= 10;
					else if (msg.wParam == VK_RIGHT)
						pt.x += 10;
					else if (msg.wParam == VK_UP)
						pt.y -= 10;
					else if (msg.wParam == VK_DOWN)
						pt.y += 10;

					AdjustCursor(pt);
					SetCursorPos(pt.x, pt.y);
					Move(pt);
					m_ptSticky = CPoint(0, 0);
				}
			}
			if (msg.wParam == VK_CONTROL)
			{
				Move(m_ptLast);
				m_ptSticky = CPoint(0, 0);
			}
			if (msg.wParam == VK_ESCAPE)
			{
				break;
			}
		}
		else
			DispatchMessage(&msg);
	}

	CancelLoop();

	if (bAccept && m_ptSticky == CPoint(0, 0))
	{
		XTPDockingPaneAction actionNotify = xtpPaneActionFloated;

		CXTPDockingPaneBaseList lst;
		m_pPane->FindPane(xtpPaneTypeDockingPane, &lst);

		if (m_bAttach)
		{
			ASSERT(m_pContainer->GetType() == xtpPaneTypeTabbedContainer);
			m_pManager->AttachPane(m_pPane, m_pContainer);
			actionNotify = xtpPaneActionAttached;
		}
		else
		{
			if (m_pContainer)
			{
				m_pManager->DockPane(m_pPane, m_pManager->GetRTLDirection(m_containDirection), m_pContainer);
				m_pManager->EnsureVisible(m_pContainer);
				actionNotify = xtpPaneActionDocked;
			}
			else if (m_bFloatable)
			{
				CRect rcWork = XTPMultiMonitor()->GetWorkArea(m_rectDragFrame);

				if (m_rectDragFrame.top < rcWork.top)
					m_rectDragFrame.OffsetRect(0, rcWork.top - m_rectDragFrame.top);

				if (m_pPane->GetType() == xtpPaneTypeSplitterContainer)
					m_pPane->GetDockingSite()->MoveWindow(m_rectDragFrame);
				else
					m_pManager->FloatPane(m_pPane, m_rectDragFrame);
			}
			else
			{
				bAccept = FALSE;
			}
		}

		if (bAccept)
		{
			POSITION pos = lst.GetHeadPosition();
			while (pos)
			{
				CXTPDockingPane* pPane = (CXTPDockingPane*)lst.GetNext(pos);
				m_pManager->_OnAction(actionNotify, pPane, m_pContainer, m_containDirection);
			}
		}
	}
}


void CXTPDockingPaneContext::DestroyDockingStickers()
{
	while (!m_lstStickers.IsEmpty())
	{
		CWnd* pWnd = m_lstStickers.RemoveHead();
		pWnd->DestroyWindow();
		delete pWnd;
	}
}

void CXTPDockingPaneContext::IncludeRgnPart(CRgn* pRgn, int x1, int y, int x2)
{
	if (x1 < x2)
	{
		CRgn rgnExclude;
		rgnExclude.CreateRectRgn(x1, y, x2, y + 1);
		pRgn->CombineRgn(pRgn, &rgnExclude, RGN_OR);
	}
}

void CXTPDockingPaneContext::RegionFromBitmap(CRgn* pRgn, CDC* pDC, CRect rc)
{
	CSize sz = rc.Size();

	pRgn->CreateRectRgn(0, 0, 0, 0);

	for (int y = 0; y < sz.cy; y++)
	{
		int nStart = 0, x = 0;
		BOOL bTransparent = TRUE;

		while (x < sz.cx)
		{
			BOOL bTransparentPixel = pDC->GetPixel(x, y) == 0;

			if (bTransparent && !bTransparentPixel)
			{
				nStart = x;
				bTransparent = FALSE;
			}
			else if (!bTransparent && bTransparentPixel)
			{
				IncludeRgnPart(pRgn, nStart, y, x);
				bTransparent = TRUE;
			}
			x++;
		}
		if (!bTransparent)
		{
			IncludeRgnPart(pRgn, nStart, y, x);
		}
	}
}

CXTPDockingPaneContextStickerWnd* CXTPDockingPaneContext::CreateNewSticker(CRect rc, XTPDockingPaneStickerType typeSticker)
{
	if (typeSticker == xtpPaneStickerNone || typeSticker == xtpPaneStickerClient)
		return NULL;

	CXTPDockingPaneContextStickerWnd* pWnd = new CXTPDockingPaneContextStickerWnd(this);
	m_lstStickers.AddTail(pWnd);

	pWnd->CreateEx(WS_EX_TOOLWINDOW, AfxRegisterWndClass(NULL, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		0, WS_POPUP, CRect(0, 0, 0, 0), m_pManager->GetSite(), 0);

	pWnd->m_typeSticker = typeSticker;


	CRgn* pRgn = NULL;
	if (!m_rgnStickers.Lookup(typeSticker, pRgn))
	{
		pRgn = new CRgn;
		m_rgnStickers.SetAt(typeSticker, pRgn);
	}
	ASSERT(pRgn != NULL);
	if (!pRgn)
		return NULL;

	if (!pRgn->GetSafeHandle())
	{
		CClientDC dcClient(pWnd);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dcClient, rc.Width(), rc.Height());

		if (bmp.GetSafeHandle())
		{
			CXTPCompatibleDC dc(&dcClient, &bmp);
			dc.FillSolidRect(rc, RGB(0, 0, 0));
			pWnd->OnDraw(&dc);

			RegionFromBitmap(pRgn, &dc, rc);
		}
	}

	HRGN hRgn = ::CreateRectRgn(0, 0, 0, 0);
	::CombineRgn(hRgn, (HRGN)pRgn->GetSafeHandle(), NULL, RGN_COPY);

	pWnd->SetWindowRgn(hRgn, FALSE);
	pWnd->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

	return pWnd;
}

CSize CXTPDockingPaneContext::GetStickerSize(XTPDockingPaneStickerType type) const
{
	if (GetStickerStyle() == xtpPaneStickerStyleVisualStudio2005)
	{
		if (type == xtpPaneStickerTop) return CSize(29, 32);
		if (type == xtpPaneStickerLeft) return CSize(32, 29);
		if (type == xtpPaneStickerBottom) return CSize(29, 32);
		if (type == xtpPaneStickerRight) return CSize(32, 29);
		return CSize(89, 89);
	}
	else
	{
		if (type == xtpPaneStickerTop) return CSize(43, 30);
		if (type == xtpPaneStickerLeft) return CSize(30, 43);
		if (type == xtpPaneStickerBottom) return CSize(43, 30);
		if (type == xtpPaneStickerRight) return CSize(30, 43);
		return CSize(93, 93);
	}
}

XTPDockingContextStickerStyle CXTPDockingPaneContext::GetStickerStyle() const
{
	return m_pManager->GetDockingContextStickerStyle();

}

void CXTPDockingPaneContext::UpdateDockingStickers()
{
	if (m_rectStickerPane.IsRectEmpty())
	{
		m_pLastStickerPane = NULL;
		DestroyDockingStickers();
		return;
	}

	CSize sz(GetStickerSize(xtpPaneStickerClient));
	CRect rc(m_rectStickerPane.CenterPoint(), sz);
	rc.OffsetRect(- sz.cx / 2, - sz.cy / 2);

	if (m_pLastStickerPane != m_pStickerPane || m_lstStickers.IsEmpty())
	{
		DestroyDockingStickers();

		if (m_pStickerPane)
		{
			UINT allowStickers = xtpPaneStickerNone;
			if (IsAllowDockingTo(m_pStickerPane, xtpPaneDockLeft)) allowStickers = allowStickers + xtpPaneStickerLeft;
			if (IsAllowDockingTo(m_pStickerPane, xtpPaneDockRight)) allowStickers = allowStickers + xtpPaneStickerRight;
			if (IsAllowDockingTo(m_pStickerPane, xtpPaneDockBottom)) allowStickers = allowStickers + xtpPaneStickerBottom;
			if (IsAllowDockingTo(m_pStickerPane, xtpPaneDockTop)) allowStickers = allowStickers + xtpPaneStickerTop;

			CreateNewSticker(rc, m_pStickerPane == m_pManager->GetClientPane() || !IsAllowAttachTo(m_pStickerPane) ? XTPDockingPaneStickerType(xtpPaneStickerClient + allowStickers) : XTPDockingPaneStickerType(xtpPaneStickerClient + allowStickers + xtpPaneStickerCenter));
		}

		CXTPDockingPaneBase* pTopPane = m_pManager->GetTopPane();
		CRect rcWindow = pTopPane->GetPaneWindowRect();

		if (IsAllowDockingTo(pTopPane, xtpPaneDockTop))
			CreateNewSticker(CRect(CPoint(rcWindow.CenterPoint().x - GetStickerSize(xtpPaneStickerTop).cx / 2, rcWindow.top + 16), GetStickerSize(xtpPaneStickerTop)), xtpPaneStickerTop);
		if (IsAllowDockingTo(pTopPane, xtpPaneDockLeft))
			CreateNewSticker(CRect(CPoint(rcWindow.left + 16, rcWindow.CenterPoint().y - GetStickerSize(xtpPaneStickerLeft).cy/ 2), GetStickerSize(xtpPaneStickerLeft)), xtpPaneStickerLeft);
		if (IsAllowDockingTo(pTopPane, xtpPaneDockBottom))
			CreateNewSticker(CRect(CPoint(rcWindow.CenterPoint().x - GetStickerSize(xtpPaneStickerBottom).cx / 2, rcWindow.bottom - 16 - GetStickerSize(xtpPaneStickerBottom).cy), GetStickerSize(xtpPaneStickerBottom)), xtpPaneStickerBottom);
		if (IsAllowDockingTo(pTopPane, xtpPaneDockRight))
			CreateNewSticker(CRect(CPoint(rcWindow.right - GetStickerSize(xtpPaneStickerRight).cx - 16, rcWindow.CenterPoint().y - GetStickerSize(xtpPaneStickerRight).cy/ 2), GetStickerSize(xtpPaneStickerRight)), xtpPaneStickerRight);

		m_pLastStickerPane = m_pStickerPane;
	}

	ASSERT(m_lstStickers.GetCount() < 6);
}


void CXTPDockingPaneContext::CreateContextWindow(CXTPDockingPaneContextAlphaWnd* pWnd)
{
	ASSERT(m_bUseAlphaContext);

	if (pWnd->GetSafeHwnd())
		return;

	pWnd->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, AfxRegisterWndClass(NULL, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		0, WS_POPUP, CRect(0, 0, 0, 0), m_pManager->GetSite(), 0);

	if (m_pfnSetLayeredWindowAttributes)
	{
		((PFNSETLAYEREDWINDOWATTRIBUTES)m_pfnSetLayeredWindowAttributes)
			(pWnd->m_hWnd, 0, 100, LWA_ALPHA);
	}
}
