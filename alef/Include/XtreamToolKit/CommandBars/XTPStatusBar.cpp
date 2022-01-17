// XTPStatusBar.cpp : implementation of the CXTPStatusBar class.
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
#include "Common/XTPColorManager.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPToolTipContext.h"

#include "XTPStatusBar.h"
#include "XTPPaintManager.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SBPF_UPDATE 0x0001  // pending update of text
#define CX_PANE_BORDER 6    // 3 pixels on each side of each pane

#define WM_GETTHEMED_STATUSBAR (WM_USER + 9300)

//////////////////////////////////////////////////////////////////////////
// CXTPStatusBarPane

IMPLEMENT_DYNAMIC(CXTPStatusBarPane, CCmdTarget)

CXTPStatusBarPane::CXTPStatusBarPane()
{
	m_nID = 0;
	m_cxText = 0;
	m_nStyle = 0;
	m_nFlags = 0;

	m_nIndex = -1;
	m_nStatusIndex = -1;
	m_pStatusBar = NULL;

	m_hWndPane = NULL;
	m_bAutoDeleteWnd = FALSE;
	m_bVisible = TRUE;

	m_clrText = (COLORREF)-1;
	m_clrBackground = (COLORREF)-1;

	m_nIconIndex = -1;
	m_nAlignment = DT_LEFT;

	m_rcMargins.SetRect(2, 1, 2, 1);


}

CXTPStatusBarPane::~CXTPStatusBarPane()
{
	CWnd* pWnd = CWnd::FromHandlePermanent(m_hWndPane);

	if (pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->DestroyWindow();

		if (m_bAutoDeleteWnd)
		{
			delete pWnd;
		}
	}
}

CRect CXTPStatusBarPane::GetMargins() const
{
	return m_rcMargins;
}

void CXTPStatusBarPane::SetMargins(int nLeft, int nTop, int nRight, int nBottom)
{
	m_rcMargins.SetRect(nLeft, nTop, nRight, nBottom);
}

void CXTPStatusBarPane::SetText(LPCTSTR lpszText)
{
	ASSERT(m_pStatusBar);
	m_pStatusBar->SetPaneText(m_nIndex, lpszText);
}

CString CXTPStatusBarPane::GetText() const
{
	return m_strText;
}

void CXTPStatusBarPane::SetWidth(int nWidth)
{
	m_pStatusBar->SetPaneWidth(m_nIndex, nWidth);
}

int CXTPStatusBarPane::GetWidth() const
{
	return m_cxText;
}

void CXTPStatusBarPane::SetStyle(int nStyle)
{
	m_pStatusBar->SetPaneStyle(m_nIndex, nStyle);

}
int CXTPStatusBarPane::GetStyle() const
{
	return m_nStyle;
}

UINT CXTPStatusBarPane::GetID() const
{
	return m_nID;
}

int CXTPStatusBarPane::GetIndex() const
{
	return m_nIndex;
}

CXTPStatusBar* CXTPStatusBarPane::GetStatusBar() const
{
	return m_pStatusBar;
}

void CXTPStatusBarPane::Remove()
{
	m_pStatusBar->RemoveAt(m_nIndex);
}

void CXTPStatusBarPane::SetVisible(BOOL bVisible)
{
	if (m_bVisible != bVisible)
	{
		m_bVisible = bVisible;

		if (m_hWndPane)
			::ShowWindow(m_hWndPane, m_bVisible ? SW_SHOW : SW_HIDE);

		m_pStatusBar->UpdateAllPanes(TRUE, FALSE);
	}
}

BOOL CXTPStatusBarPane::IsVisible() const
{
	return m_bVisible;
}

void CXTPStatusBarPane::OnDraw(CDC* pDC, CRect rcItem)
{
	CXTPPaintManager* pPaintManager = m_pStatusBar->GetPaintManager();

	if (pPaintManager)
	{
		pPaintManager->DrawStatusBarPaneEntry(pDC, rcItem, this);
	}
}

void CXTPStatusBarPane::BestFit()
{
	CClientDC dc(m_pStatusBar);
	CXTPFontDC font(&dc, GetTextFont());

	m_cxText = dc.GetTextExtent(m_strText).cx + m_rcMargins.left + m_rcMargins.right;

	CXTPImageManagerIcon* pIcon = GetImage();
	if (pIcon) m_cxText += pIcon->GetWidth() + (!m_strText.IsEmpty() ? 2 : 0);
}

void CXTPStatusBarPane::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
	SAFE_INVALIDATE(m_pStatusBar);
}

COLORREF CXTPStatusBarPane::GetTextColor() const
{
	return m_clrText;
}

void CXTPStatusBarPane::SetBackgroundColor(COLORREF clrBackground)
{
	m_clrBackground = clrBackground;
	SAFE_INVALIDATE(m_pStatusBar);
}

COLORREF CXTPStatusBarPane::GetBackgroundColor() const
{
	return m_clrBackground;
}

CFont* CXTPStatusBarPane::GetTextFont()
{
	ASSERT(m_pStatusBar);
	return (m_fntText.m_hObject == NULL) ?
		&m_pStatusBar->m_fontStatus : &m_fntText;
}

void CXTPStatusBarPane::SetTextFont(CFont* pFntText)
{
	ASSERT(pFntText);

	LOGFONT lf;
	pFntText->GetLogFont(&lf);

	SetTextFont(&lf);
}

void CXTPStatusBarPane::SetTextFont(PLOGFONT pLogfText)
{
	ASSERT(pLogfText);

	m_fntText.DeleteObject();
	m_fntText.CreateFontIndirect(pLogfText);

	SAFE_INVALIDATE(m_pStatusBar);
}

CXTPImageManagerIcon* CXTPStatusBarPane::GetImage() const
{
	if (m_nIconIndex != -1)
		return m_pStatusBar->GetImageManager()->GetImage(m_nIconIndex, 0);

	return NULL;
}

int CXTPStatusBarPane::GetIconIndex() const
{
	return m_nIconIndex;
}

void CXTPStatusBarPane::SetIconIndex(int nIconIndex)
{
	m_nIconIndex = nIconIndex;
	m_pStatusBar->UpdateAllPanes(TRUE, FALSE);
}

void CXTPStatusBarPane::SetTextAlignment(int nAlign)
{
	m_nAlignment = nAlign;
	SAFE_INVALIDATE(m_pStatusBar);
}

int CXTPStatusBarPane::GetTextAlignment() const
{
	return m_nAlignment;
}

void CXTPStatusBarPane::SetTooltip(LPCTSTR lpszTooltip)
{
	m_strToolTip = lpszTooltip;
}

CString CXTPStatusBarPane::GetTooltip() const
{
	return m_strToolTip;
}



//////////////////////////////////////////////////////////////////////////
// CXTPStatusBar

IMPLEMENT_DYNAMIC(CXTPStatusBar, CControlBar)

CXTPStatusBar::CXTPStatusBar()
{
	// setup default border/margin depending on type of system
	m_cyTopBorder = 2;
	if (IsWin4())
	{
		m_cxLeftBorder = 0;
		m_cxRightBorder = 0;
		m_cyBottomBorder = 0;
	}
	else
	{
		m_cxLeftBorder = 2;
		m_cxRightBorder = 2;
		m_cyBottomBorder = 1;
	}
	// minimum height set with SB_SETMINHEIGHT is cached
	m_nMinHeight = 0;

	m_pCommandBars = 0;
	m_bCommandBarsTheme = TRUE;
	m_bDrawDisabledText = TRUE;
	m_pToolTipContext = new CXTPToolTipContext;

	CXTPPaintManager::CNonClientMetrics ncm;
	m_fontStatus.CreateFontIndirect(&ncm.lfStatusFont);

}

BOOL CXTPStatusBar::IsWin4()
{
#if (_MSC_VER >= 1300)
	return TRUE;
#else
	DWORD dwVersion = ::GetVersion();
	return (BYTE)dwVersion >= 4;
#endif
}


CXTPStatusBar::~CXTPStatusBar()
{
	RemoveAll();

	CMDTARGET_RELEASE(m_pToolTipContext);

}

CXTPImageManager* CXTPStatusBar::GetImageManager() const
{
	if (m_pCommandBars)
		return m_pCommandBars->GetImageManager();

	return XTPImageManager();
}

CXTPStatusBarPane* CXTPStatusBar::HitTest(CPoint pt, CRect* lpRect) const
{
	for (int i = 0; i < GetPaneCount(); i++)
	{
		CXTPStatusBarPane* pPane = GetPane(i);
		if (!pPane->IsVisible())
			continue;

		CRect rc(0, 0, 0, 0);
		GetItemRect(i, &rc);

		if (rc.PtInRect(pt))
		{
			if (lpRect)
				*lpRect = rc;

			return pPane;
		}
	}

	return NULL;
}

void CXTPStatusBar::RemoveAll()
{
	for (int i = 0; i < (int)m_arrPanes.GetSize(); i++)
	{
		m_arrPanes[i]->InternalRelease();
	}
	m_arrPanes.RemoveAll();

	UpdateAllPanes(TRUE, TRUE);
}

CXTPStatusBarPane* CXTPStatusBar::GetPane(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < GetPaneCount());

	return nIndex >= 0 && nIndex < GetPaneCount() ? m_arrPanes[nIndex]: NULL;
}

CXTPStatusBarPane* CXTPStatusBar::FindPane(UINT nID) const
{
	for (int i = 0; i < GetPaneCount(); i++)
	{
		if (GetPane(i)->m_nID == nID)
			return GetPane(i);
	}
	return NULL;
}

int CXTPStatusBar::GetVisiblePaneCount() const
{
	int nCount = 0;

	for (int i = 0; i < GetPaneCount(); i++)
	{
		if (m_arrPanes[i]->IsVisible()) nCount++;
	}

	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar creation, etc



BOOL CXTPStatusBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	return CreateEx(pParentWnd, 0, dwStyle, nID);
}

void CXTPStatusBar::SetFont(CFont* pFont, BOOL bRedraw /* = TRUE */)
{
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	m_fontStatus.DeleteObject();
	m_fontStatus.CreateFontIndirect(&lf);

	CWnd::SetFont(&m_fontStatus, bRedraw);
}

BOOL CXTPStatusBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent

	// save the style (some of these style bits are MFC specific)
	m_dwStyle = (dwStyle & CBRS_ALL);

	// translate MFC style bits to windows style bits
	dwStyle &= ~CBRS_ALL;
	dwStyle |= CCS_NOPARENTALIGN | CCS_NOMOVEY | CCS_NODIVIDER | CCS_NORESIZE;
	if (pParentWnd->GetStyle() & WS_THICKFRAME)
		dwStyle |= SBARS_SIZEGRIP;
	dwStyle |= dwCtrlStyle;

#if _MSC_VER <= 1100
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTLS_REG));
#else
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_BAR_REG));
#endif

	// create the HWND
	CRect rect;
	rect.SetRectEmpty();
	return CWnd::Create(STATUSCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID);
}

BOOL CXTPStatusBar::PreCreateWindow(CREATESTRUCT& cs)
{
	// in Win4, status bars do not have a border at all, since it is
	//  provided by the client area.
	if (IsWin4() &&
		(m_dwStyle & (CBRS_ALIGN_ANY | CBRS_BORDER_ANY)) == CBRS_BOTTOM)
	{
		m_dwStyle &= ~(CBRS_BORDER_ANY | CBRS_BORDER_3D);
	}
	return CControlBar::PreCreateWindow(cs);
}

BOOL CXTPStatusBar::SetIndicators(const UINT* lpIDArray, int nIDCount)
{
	ASSERT_VALID(this);
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(UINT) * nIDCount, FALSE));
	ASSERT(::IsWindow(m_hWnd));

	RemoveAll();

	// copy initial data from indicator array
	BOOL bResult = TRUE;

	if (lpIDArray != NULL)
	{
		for (int i = 0; i < nIDCount; i++)
		{
			CXTPStatusBarPane* pPane = new CXTPStatusBarPane();
			m_arrPanes.Add(pPane);

			pPane->m_pStatusBar = this;
			pPane->m_nID = *lpIDArray++;
			pPane->m_nFlags |= SBPF_UPDATE;

			if (pPane->m_nID != 0)
			{
				if (!pPane->m_strText.LoadString(pPane->m_nID))
				{
					TRACE1("Warning: failed to load indicator string 0x%04X.\n",
						pPane->m_nID);
					bResult = FALSE;
					break;
				}

				pPane->BestFit();
			}
			else
			{
				// no indicator (must access via index)
				// default to 1/4 the screen width (first pane is stretchy)
				pPane->m_cxText = ::GetSystemMetrics(SM_CXSCREEN)/4;
				if (i == 0)
					pPane->m_nStyle |= (SBPS_STRETCH | SBPS_NOBORDERS);
			}
		}
	}
	UpdateAllPanes(TRUE, TRUE);

	return bResult;
}

void CXTPStatusBar::CalcInsideRect(CRect& rect, BOOL bHorz) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(bHorz);  // vertical status bar not supported

	// subtract standard CControlBar borders
	CControlBar::CalcInsideRect(rect, bHorz);

	// subtract size grip if present
	if ((GetStyle() & SBARS_SIZEGRIP) && !::IsZoomed(::GetParent(m_hWnd)))
	{
		// get border metrics from common control
		int rgBorders[3];
		CXTPStatusBar* pBar = (CXTPStatusBar*)this;
		pBar->DefWindowProc(SB_GETBORDERS, 0, (LPARAM)&rgBorders);

		// size grip uses a border + size of scrollbar + cx border
		rect.right -= rgBorders[0] + ::GetSystemMetrics(SM_CXVSCROLL) +
			::GetSystemMetrics(SM_CXBORDER) * 2;
	}
}

void CXTPStatusBar::UpdateAllPanes(BOOL bUpdateRects, BOOL bUpdateText)
{
	if (!GetSafeHwnd())
		return;

	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// update the status pane locations
	if (bUpdateRects)
	{
		// get border information and client work area
		CRect rect;
		GetWindowRect(rect);
		rect.OffsetRect(-rect.left, -rect.top);
		CalcInsideRect(rect, TRUE);
		int rgBorders[3];
		VERIFY((BOOL)DefWindowProc(SB_GETBORDERS, 0, (LPARAM)&rgBorders));

		// determine extra space for stretchy pane
		int cxExtra = rect.Width() + rgBorders[2];
		int nStretchyCount = 0, i, nVisibleCount = 0;

		for (i = 0; i < GetPaneCount(); i++)
		{
			CXTPStatusBarPane* pPane = GetPane(i);
			pPane->m_nIndex = i;

			if (!pPane->IsVisible())
				continue;

			pPane->m_nStatusIndex = nVisibleCount;
			nVisibleCount++;

			if (pPane->m_nStyle & SBPS_STRETCH)
				++nStretchyCount;
			cxExtra -= (pPane->m_cxText + CX_PANE_BORDER + rgBorders[2]);
		}

		// determine right edge of each pane
		int* rgRights = (int*)_alloca(nVisibleCount * sizeof(int));
		int right = rgBorders[0], j = 0;

		for (i = 0; i < GetPaneCount(); i++)
		{
			CXTPStatusBarPane* pPane = GetPane(i);
			if (!pPane->IsVisible())
				continue;

			// determine size of the pane
			ASSERT(pPane->m_cxText >= 0);
			right += pPane->m_cxText + CX_PANE_BORDER;
			if ((pPane->m_nStyle & SBPS_STRETCH) && cxExtra > 0)
			{
				ASSERT(nStretchyCount != 0);
				int cxAddExtra = cxExtra / nStretchyCount;
				right += cxAddExtra;
				--nStretchyCount;
				cxExtra -= cxAddExtra;
			}
			rgRights[j] = right;
			j++;

			// next pane
			right += rgBorders[2];
		}

		// set new right edges for all panes
		DefWindowProc(SB_SETPARTS, nVisibleCount, (LPARAM)rgRights);

		PositionControls();
	}

	// update text in the status panes if specified
	if (bUpdateText)
	{
		for (int i = 0; i < GetPaneCount(); i++)
		{
			CXTPStatusBarPane* pPane = GetPane(i);
			if (!pPane->IsVisible())
				continue;

			if (pPane->m_nFlags & SBPF_UPDATE)
				SetPaneText(i, pPane->m_strText);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar attribute access

int CXTPStatusBar::CommandToIndex(UINT nIDFind) const
{
	ASSERT_VALID(this);

	if (GetPaneCount() <= 0)
		return -1;

	for (int i = 0; i < GetPaneCount(); i++)
	{
		if (GetPane(i)->m_nID == nIDFind)
			return i;
	}

	return -1;
}

UINT CXTPStatusBar::GetItemID(int nIndex) const
{
	ASSERT_VALID(this);

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);

	if (!pPane)
		return 0;

	return pPane->m_nID;
}

void CXTPStatusBar::GetItemRect(int nIndex, LPRECT lpRect) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	if (!pPane)
	{
		::SetRectEmpty(lpRect);
		return;
	}

	CXTPStatusBar* pBar = (CXTPStatusBar*)this;
	if (!pBar->DefWindowProc(SB_GETRECT, pPane->m_nStatusIndex, (LPARAM)lpRect))
		::SetRectEmpty(lpRect);

	// WINBUG: CStatusBar::GetItemRect(...) does not return the correct size
	// of the last item in the status bar in most cases, so we need to calculate
	// the width ourself...

	if (pPane->m_nStatusIndex == GetVisiblePaneCount() - 1)
	{
		if ((pPane->m_nStyle & SBPS_STRETCH) == 0)
		{
			lpRect->right = lpRect->left + pPane->m_cxText + (::GetSystemMetrics(SM_CXEDGE)* 3);
		}
		else
		{
			lpRect->right = CXTPClientRect(pBar).right;

			if ((GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP)
			{
				lpRect->right -= (::GetSystemMetrics(SM_CXSMICON) + ::GetSystemMetrics(SM_CXEDGE));
			}
		}
	}
}

UINT CXTPStatusBar::GetPaneStyle(int nIndex) const
{
	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);

	if (!pPane)
		return 0;

	return pPane->m_nStyle;
}

void CXTPStatusBar::SetPaneStyle(int nIndex, UINT nStyle)
{
	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);

	if (pPane && pPane->m_nStyle != nStyle)
	{
		// if the pane is changing SBPS_STRETCH, then...
		if ((pPane->m_nStyle ^ nStyle) & SBPS_STRETCH)
		{
			// ... we need to re-layout the panes
			pPane->m_nStyle = nStyle;
			UpdateAllPanes(TRUE, FALSE);
		}

		// use SetPaneText, since it updates the style and text
		pPane->m_nStyle = nStyle;
		pPane->m_nFlags |= SBPF_UPDATE;
		SetPaneText(nIndex, pPane->m_strText);
	}
}

void CXTPStatusBar::GetPaneInfo(int nIndex, UINT& nID, UINT& nStyle,
	int& cxWidth) const
{
	ASSERT_VALID(this);

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	nID = pPane->m_nID;
	nStyle = pPane->m_nStyle;
	cxWidth = pPane->m_cxText;
}

void CXTPStatusBar::SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth)
{
	ASSERT_VALID(this);

	BOOL bChanged = FALSE;
	CXTPStatusBarPane* pPane = GetPane(nIndex);
	pPane->m_nID = nID;
	if (pPane->m_nStyle != nStyle)
	{
		if ((pPane->m_nStyle ^ nStyle) & SBPS_STRETCH)
			bChanged = TRUE;
		else
		{
			pPane->m_nStyle = nStyle;
			pPane->m_nFlags |= SBPF_UPDATE;
			SetPaneText(nIndex, pPane->m_strText);
		}
		pPane->m_nStyle = nStyle;
	}
	if (cxWidth != pPane->m_cxText)
	{
		// change width of one pane -> invalidate the entire status bar
		pPane->m_cxText = cxWidth;
		bChanged = TRUE;
	}
	if (bChanged)
		UpdateAllPanes(TRUE, FALSE);
}

void CXTPStatusBar::GetPaneText(int nIndex, CString& s) const
{
	ASSERT_VALID(this);

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);
	if (!pPane)
		return;

	s = pPane->m_strText;
}

CString CXTPStatusBar::GetPaneText(int nIndex) const
{
	ASSERT_VALID(this);

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);
	if (!pPane)
		return _T("");

	return pPane->m_strText;
}

void CXTPStatusBar::SetPaneWidth(int nIndex, int cxText)
{
	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);
	if (!pPane)
		return;

	if (cxText <= 0)
	{
		pPane->BestFit();
		UpdateAllPanes(TRUE, FALSE);
	}
	else if (pPane->m_cxText != cxText)
	{
		pPane->m_cxText = cxText;
		UpdateAllPanes(TRUE, FALSE);
	}
}

BOOL CXTPStatusBar::SetPaneText(int nIndex, const CString& lpszNewText, BOOL bUpdate)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	ASSERT(pPane);
	if (!pPane)
		return FALSE;

	if (!(pPane->m_nFlags & SBPF_UPDATE) && (lpszNewText == pPane->m_strText))
	{
		// nothing to change
		return TRUE;
	}

	pPane->m_strText = lpszNewText;

	if (!pPane->IsVisible())
		return FALSE;

	if (!bUpdate)
	{
		// can't update now, wait until later
		pPane->m_nFlags |= SBPF_UPDATE;
		return TRUE;
	}

	pPane->m_nFlags &= ~SBPF_UPDATE;
	DefWindowProc(SB_SETTEXT, ((WORD)pPane->m_nStyle) | pPane->m_nStatusIndex,
		(pPane->m_nStyle & SBPS_DISABLED) ? NULL :
		(LPARAM)(LPCTSTR)pPane->m_strText);

	Invalidate(FALSE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar implementation

CSize CXTPStatusBar::CalcFixedLayout(BOOL, BOOL bHorz)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// determine size of font being used by the status bar
	TEXTMETRIC tm;
	{
		CClientDC dc(NULL);
		CXTPFontDC font(&dc, &m_fontStatus);
		VERIFY(dc.GetTextMetrics(&tm));
	}

	// get border information
	CRect rect;
	rect.SetRectEmpty();
	CalcInsideRect(rect, bHorz);
	int rgBorders[3];
	DefWindowProc(SB_GETBORDERS, 0, (LPARAM)&rgBorders);

	// determine size, including borders
	CSize size;
	size.cx = 32767;
	size.cy = tm.tmHeight - tm.tmInternalLeading - 1
		+ rgBorders[1] * 2 + ::GetSystemMetrics(SM_CYBORDER) * 2
		- rect.Height();
	if (size.cy < m_nMinHeight)
		size.cy = m_nMinHeight;

	return size;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar message handlers

BEGIN_MESSAGE_MAP(CXTPStatusBar, CControlBar)
	//{{AFX_MSG_MAP(CXTPStatusBar)
	ON_WM_NCHITTEST_EX()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	ON_MESSAGE(SB_SETMINHEIGHT, OnSetMinHeight)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)

	ON_MESSAGE(WM_GETTHEMED_STATUSBAR, OnGetThemedStatusBar)
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CXTPStatusBar::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CControlBar::OnCreate(lpcs) == -1)
		return -1;

	SetFont(&m_fontStatus, FALSE);
	SendMessage (SB_SETMINHEIGHT, 20);

	return 0;
}

LRESULT CXTPStatusBar::OnNcHitTest(CPoint)
{
	LRESULT nResult = (LRESULT)Default();
	if (nResult == HTBOTTOMRIGHT)
		return (LRESULT)HTBOTTOMRIGHT;

	return (LRESULT)HTCLIENT;
}

LRESULT CXTPStatusBar::OnGetThemedStatusBar(WPARAM, LPARAM)
{
	CXTPPaintManager* pPaintManager = GetPaintManager();
	if (!pPaintManager)
		return FALSE;

	return pPaintManager->m_bThemedStatusBar;
}

void CXTPStatusBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS* lpncsp)
{
	// calculate border space (will add to top/bottom, subtract from right/bottom)
	CRect rect;
	rect.SetRectEmpty();
	CControlBar::CalcInsideRect(rect, TRUE);
	ASSERT(rect.top >= 2);

	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top - 2;
	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}

void CXTPStatusBar::OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle)
{
	if (m_hWnd != NULL &&
		((dwOldStyle & CBRS_BORDER_ANY) != (dwNewStyle & CBRS_BORDER_ANY)))
	{
		// recalc non-client area when border styles change
		SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
	}
}

void CXTPStatusBar::OnNcPaint()
{
	EraseNonClient();
}

void CXTPStatusBar::OnSize(UINT nType, int cx, int cy)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	CControlBar::OnSize(nType, cx, cy);

	// need to adjust pane right edges (because of stretchy pane)
	UpdateAllPanes(TRUE, FALSE);
	Invalidate(FALSE);
}

void CXTPStatusBar::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
	// not necessary to invalidate the borders
	DWORD dwStyle = m_dwStyle;
	m_dwStyle &= ~(CBRS_BORDER_ANY);
	CControlBar::OnWindowPosChanging(lpWndPos);
	m_dwStyle = dwStyle;
}

LRESULT CXTPStatusBar::OnSetText(WPARAM, LPARAM lParam)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	int nIndex = CommandToIndex(0);
	if (nIndex < 0)
		return -1;
	return SetPaneText(nIndex, (LPCTSTR)lParam) ? 0 : -1;
}

LRESULT CXTPStatusBar::OnGetText(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	int nMaxLen = (int)wParam;
	if (nMaxLen == 0)
		return 0;       // nothing copied
	LPTSTR lpszDest = (LPTSTR)lParam;

	int nLen = 0;
	int nIndex = CommandToIndex(0); // use pane with ID zero
	if (nIndex >= 0)
	{
		CXTPStatusBarPane* pPane = GetPane(nIndex);
		nLen = pPane->m_strText.GetLength();
		if (nLen > nMaxLen)
			nLen = nMaxLen - 1; // number of characters to copy (less term.)
		MEMCPY_S(lpszDest, (LPCTSTR)pPane->m_strText, nLen*sizeof(TCHAR));
	}
	lpszDest[nLen] = '\0';
	return nLen + 1;      // number of bytes copied
}

LRESULT CXTPStatusBar::OnGetTextLength(WPARAM, LPARAM)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	int nLen = 0;
	int nIndex = CommandToIndex(0); // use pane with ID zero
	if (nIndex >= 0)
	{
		CXTPStatusBarPane* pPane = GetPane(nIndex);
		nLen = pPane->m_strText.GetLength();
	}
	return nLen;
}

LRESULT CXTPStatusBar::OnSetMinHeight(WPARAM wParam, LPARAM)
{
	LRESULT lResult = Default();
	m_nMinHeight = (int)wParam;
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar idle update through CStatusCmdUI class

class CXTPStatusBar::CStatusCmdUI : public CCmdUI      // class private to this file!
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void CXTPStatusBar::CStatusCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CXTPStatusBar* pStatusBar = (CXTPStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CXTPStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pStatusBar->GetPaneStyle(m_nIndex) & ~SBPS_DISABLED;
	if (!bOn)
		nNewStyle |= SBPS_DISABLED;
	pStatusBar->SetPaneStyle(m_nIndex, nNewStyle);
}

void CXTPStatusBar::CStatusCmdUI::SetCheck(int nCheck) // "checking" will pop out the text
{
	CXTPStatusBar* pStatusBar = (CXTPStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CXTPStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pStatusBar->GetPaneStyle(m_nIndex) & ~SBPS_POPOUT;
	if (nCheck != 0)
		nNewStyle |= SBPS_POPOUT;
	pStatusBar->SetPaneStyle(m_nIndex, nNewStyle);
}

void CXTPStatusBar::CStatusCmdUI::SetText(LPCTSTR lpszText)
{
	CXTPStatusBar* pStatusBar = (CXTPStatusBar*)m_pOther;
	ASSERT(pStatusBar != NULL);
	ASSERT_KINDOF(CXTPStatusBar, pStatusBar);
	ASSERT(m_nIndex < m_nIndexMax);

	pStatusBar->SetPaneText(m_nIndex, lpszText);
}

void CXTPStatusBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CStatusCmdUI state;
	state.m_pOther = this;
	state.m_nIndexMax = (UINT)GetPaneCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = GetPane(state.m_nIndex)->m_nID;

		// allow the statusbar itself to have update handlers
		if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
			continue;

		// allow target (owner) to handle the remaining updates
		state.DoUpdate(pTarget, FALSE);
	}

	// update the dialog controls added to the status bar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar diagnostics

#ifdef _DEBUG
void CXTPStatusBar::AssertValid() const
{
	CControlBar::AssertValid();
}

void CXTPStatusBar::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);

	if (dc.GetDepth() > 0)
	{
		for (int i = 0; i < GetPaneCount(); i++)
		{
			dc << "\nstatus pane[" << i << "] = {";
			dc << "\n\tnID = " << GetPane(i)->m_nID;
			dc << "\n\tnStyle = " << GetPane(i)->m_nStyle;
			dc << "\n\tcxText = " << GetPane(i)->m_cxText;
			dc << "\n\tstrText = " << GetPane(i)->m_strText;
			dc << "\n\t}";
		}
	}
	dc << "\n";
}
#endif //_DEBUG


CXTPPaintManager* CXTPStatusBar::GetPaintManager() const
{
	if (!m_bCommandBarsTheme)
		return NULL;

	if (m_pCommandBars)
		return m_pCommandBars->GetPaintManager();
	return XTPPaintManager();
}

void CXTPStatusBar::DrawPaneEntry(CDC* pDC, int nIndex, CRect rcItem)
{
	CXTPStatusBarPane* pPane = GetPane(nIndex);

	if (pPane)
	{
		pPane->OnDraw(pDC, rcItem);
	}
}

void CXTPStatusBar::OnPaint ()
{
	UpdateAllPanes(FALSE, TRUE);

	CXTPPaintManager* pPaintManager = GetPaintManager();

	if (pPaintManager == NULL)
	{
		Default();
		return;
	}

	CPaintDC dcPaint (this);
	CXTPClientRect rcClient(this);
	CXTPBufferDC dc(dcPaint, rcClient);

	pPaintManager->FillStatusBar(&dc, this);

	int nVisibleCount = GetVisiblePaneCount();

	for (int i = 0; i < GetPaneCount(); i++)
	{
		CXTPStatusBarPane* pPane = GetPane(i);
		if (!pPane->IsVisible())
			continue;

		CRect rcItem;
		GetItemRect (i, rcItem);

		BOOL bGripperPane = (pPane->m_nStatusIndex == nVisibleCount - 1);

		if ((pPane->GetStyle() & SBPS_NOBORDERS) == 0)
			pPaintManager->DrawStatusBarPaneBorder(&dc, rcItem, bGripperPane);

		DrawPaneEntry(&dc, i, rcItem);
	}

	if (((GetParent()->GetStyle() & WS_THICKFRAME) != 0) &&
		(GetStyle() & SBARS_SIZEGRIP) && !::IsZoomed(::GetParent(m_hWnd)))
		DrawStretch(&dc, rcClient);
}

BOOL CXTPStatusBar::OnEraseBkgnd(CDC* /*pDC*/)
{
	CXTPPaintManager* pPaintManager = GetPaintManager();

	if (pPaintManager == NULL)
	{
		return (BOOL)Default();
	}

	return TRUE;
}

void CXTPStatusBar::DrawStretch(CDC* pDC, CRect& rcClient)
{
	GetPaintManager()->DrawStatusBarGripper(pDC, rcClient);
}



/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBar message handlers

BOOL CXTPStatusBar::RemoveIndicator(UINT nID)
{
	int nIndex = CommandToIndex(nID);
	if (nIndex == -1)
	{
		TRACE1("WARNING: Pane ID %d was not found in the status bar.\n", nID);
		return FALSE;
	}

	return RemoveAt(nIndex);
}

BOOL CXTPStatusBar::RemoveAt(int nIndex)
{

	CXTPStatusBarPane* pPane = GetPane(nIndex);
	if (!pPane)
		return FALSE;

	m_arrPanes.RemoveAt(nIndex);

	pPane->InternalRelease();

	UpdateAllPanes(TRUE, TRUE);

	return TRUE;
}

CXTPStatusBarPane* CXTPStatusBar::AddIndicator(CXTPStatusBarPane* pPane, UINT nID, int nIndex)
{
	if (nIndex < 0 || nIndex > GetPaneCount())
	{
		TRACE0("WARNING: Pane index out of range.\n");
		nIndex = GetPaneCount();
	}

	if (CommandToIndex(nID) != -1)
	{
		TRACE1("WARNING: Pane ID %d already exists in the status bar.\n", nID);
	}

	m_arrPanes.InsertAt(nIndex, pPane);
	pPane->m_nID = nID;
	pPane->m_pStatusBar = this;

	if (nID != 0)
	{
		pPane->m_strText.LoadString(nID);
		pPane->BestFit();
	}
	else
	{
		// no indicator (must access via index)
		// default to 1/4 the screen width (first pane is stretchy)
		pPane->m_cxText = ::GetSystemMetrics(SM_CXSCREEN)/4;
		if (nIndex == 0)
			pPane->m_nStyle |= (SBPS_STRETCH | SBPS_NOBORDERS);
	}

	UpdateAllPanes(TRUE, TRUE);

	return pPane;
}

CXTPStatusBarPane* CXTPStatusBar::AddIndicator(UINT nID, int nIndex)
{
	if (nIndex < 0 || nIndex > GetPaneCount())
	{
		TRACE0("WARNING: Pane index out of range.\n");
		return NULL;
	}

	if (CommandToIndex(nID) != -1)
	{
		TRACE1("WARNING: Pane ID %d already exists in the status bar.\n", nID);
		return NULL;
	}

	return AddIndicator(new CXTPStatusBarPane(), nID, nIndex);
}

void CXTPStatusBar::PositionControls()
{
	CXTPClientRect rcClient(this);

	int cx = ::GetSystemMetrics(SM_CXEDGE);

	for (int i = 0; i < GetPaneCount(); i++)
	{
		CXTPStatusBarPane* pPane = GetPane(i);
		if (pPane->m_hWndPane == NULL || !IsWindow(pPane->m_hWndPane))
			continue;

		DWORD dwStyle = pPane->m_nStyle;

		CRect rcPane;
		GetItemRect(i, &rcPane);

		if ((dwStyle & SBPS_NOBORDERS) == 0)
		{
			rcPane.DeflateRect(cx, cx);
		}

		::MoveWindow(pPane->m_hWndPane, rcPane.left, rcPane.top, rcPane.Width(), rcPane.Height(), TRUE);
	}
}

BOOL CXTPStatusBar::AddControl(CWnd* pWnd, UINT nID, BOOL bAutoDelete)
{
	int nIndex = CommandToIndex (nID);

	if (nIndex == -1)
	{
		return FALSE;
	}

	GetPane(nIndex)->m_hWndPane = pWnd->GetSafeHwnd();
	GetPane(nIndex)->m_bAutoDeleteWnd = bAutoDelete;

	PositionControls();
	return TRUE;
}

void CXTPStatusBar::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CControlBar::OnWindowPosChanged(lpwndpos);

	CXTPPaintManager* pPaintManager = GetPaintManager();
	if (pPaintManager && pPaintManager->BaseTheme() == xtpThemeRibbon)
	{
		if ((lpwndpos->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)) != 0)
		{
			GetParentFrame()->SetWindowPos(0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		}
	}
}

LRESULT CXTPStatusBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	// handle delay hide/show
	BOOL bVis = GetStyle() & WS_VISIBLE;
	UINT swpFlags = 0;
	if ((m_nStateFlags & delayHide) && bVis)
		swpFlags = SWP_HIDEWINDOW;
	else if ((m_nStateFlags & delayShow) && !bVis)
		swpFlags = SWP_SHOWWINDOW;
	m_nStateFlags &= ~(delayShow | delayHide);
	if (swpFlags != 0)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, swpFlags |
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	// the style must be visible and if it is docked
	// the dockbar style must also be visible
	if ((GetStyle() & WS_VISIBLE) &&
		(m_pDockBar == NULL || (m_pDockBar->GetStyle() & WS_VISIBLE)))
	{
		// MFCBUG: removed check for IsFrameWnd so we can update statusbar
		// for dialog based applications as well as doc/view apps.
		CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
		if (pTarget == NULL)
			pTarget = (CFrameWnd*)GetParent();
		if (pTarget != NULL)
			OnUpdateCmdUI(pTarget, (BOOL)wParam);
	}
	return 0L;
}

INT_PTR CXTPStatusBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CControlBar
	INT_PTR nHit = CControlBar::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	CRect rcPane;
	CXTPStatusBarPane* pPane = HitTest(point, &rcPane);
	if (pPane)
	{
		nHit = pPane->GetID();

		CString strTip = pPane->GetTooltip();
		if (strTip.IsEmpty())
			return -1;

		CXTPToolTipContext::FillInToolInfo(pTI, m_hWnd, rcPane, nHit, strTip);

		return nHit;
	}
	return -1;
}


BOOL CXTPStatusBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{


	if (m_pToolTipContext)
	{
		m_pToolTipContext->FilterToolTipMessage(this, message, wParam, lParam);
	}

	return CControlBar::OnWndMsg(message, wParam, lParam, pResult);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBarPaneWnd
/////////////////////////////////////////////////////////////////////////////
CXTPStatusBarPaneWnd::CXTPStatusBarPaneWnd()
{
}

CXTPStatusBarPaneWnd::~CXTPStatusBarPaneWnd()
{
}

IMPLEMENT_DYNCREATE(CXTPStatusBarPaneWnd, CWnd)

BEGIN_MESSAGE_MAP(CXTPStatusBarPaneWnd, CWnd)
	//{{AFX_MSG_MAP(CXTPStatusBarPaneWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTPStatusBarPaneWnd::OnPaint()
{
	// background is already filled in gray
	CPaintDC dc(this);

	// Get the client rect.
	CXTPClientRect rcClient(this);

	CXTPBufferDC memDC(dc, rcClient);
	memDC.FillSolidRect(rcClient, GetXtremeColor(COLOR_3DFACE));

	// Draw the logo text
	DoPaint(&memDC, rcClient);
}

BOOL CXTPStatusBarPaneWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

BOOL CXTPStatusBarPaneWnd::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, DWORD dwStyle/*= WS_CHILD | WS_VISIBLE*/, UINT m_nID/*= 0xffff*/)
{
	CString wndClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_DBLCLKS);

	if (!CWnd::Create(wndClass, lpszWindowName, dwStyle,
		CRect(0, 0, 0, 0), pParentWnd, m_nID))
	{
		return FALSE;
	}

	m_strWindowText = lpszWindowName;

	return TRUE;
}

void CXTPStatusBarPaneWnd::DoPaint(CDC* /*pDC*/, CRect& /*rcClient*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBarLogoPane
/////////////////////////////////////////////////////////////////////////////
CXTPStatusBarLogoPane::CXTPStatusBarLogoPane()
{
	CXTPPaintManager::CNonClientMetrics ncm;
	ncm.lfMenuFont.lfWeight = FW_BOLD;

	SetLogoFont(ncm.lfMenuFont);
}

CXTPStatusBarLogoPane::~CXTPStatusBarLogoPane()
{
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CXTPStatusBarLogoPane, CXTPStatusBarPaneWnd)
	//{{AFX_MSG_MAP(CXTPStatusBarLogoPane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTPStatusBarLogoPane::SetLogoFont(LPCTSTR lpszFontName, int nHeight/*= 24*/,
	int nWeight/*= FW_BOLD*/, BOOL bItalic/*= TRUE*/, BOOL bUnderline/*= FALSE*/)
{
	if (m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}

	m_Font.CreateFont(nHeight, 0, 0, 0, nWeight, (BYTE)bItalic,
		(BYTE)bUnderline, 0, 0, 0, 0, 0, 0, lpszFontName);
	SetTextSize();
}

void CXTPStatusBarLogoPane::SetLogoFont(LOGFONT& logFont)
{
	if (m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}

	m_Font.CreateFontIndirect(&logFont);
	SetTextSize();
}

void CXTPStatusBarLogoPane::SetTextSize()
{
	CWindowDC dc(NULL);
	CFont* pOldFont = dc.SelectObject(&m_Font);
	m_sizeText = dc.GetTextExtent(m_strWindowText);
	dc.SelectObject(pOldFont);
}

void CXTPStatusBarLogoPane::DoPaint(CDC* pDC, CRect& rcClient)
{
	// Determine the size of the logo text.
	CSize sizeText = pDC->GetTextExtent(m_strWindowText);

	// Calculate the top "y" location.
	int y = (rcClient.Height()-sizeText.cy)/2;

	// Resize the rect to reflect the actual text size.
	rcClient.top = y;
	rcClient.bottom = rcClient.top + sizeText.cy;

	// Set the text color.
	pDC->SetTextColor(GetXtremeColor(COLOR_3DSHADOW));

	// Draw text
	pDC->SetBkMode(TRANSPARENT);

	CXTPFontDC fnt(pDC, &m_Font);
	pDC->DrawText(m_strWindowText, &rcClient,
		DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_SINGLELINE);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPStatusBarIconPane
/////////////////////////////////////////////////////////////////////////////

CXTPStatusBarIconPane::CXTPStatusBarIconPane()
{
	m_hIcon = NULL;
	m_sizeIcon.cx = ::GetSystemMetrics(SM_CXSMICON);
	m_sizeIcon.cy = ::GetSystemMetrics(SM_CYSMICON);
}

CXTPStatusBarIconPane::~CXTPStatusBarIconPane()
{
	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
	}
}

BEGIN_MESSAGE_MAP(CXTPStatusBarIconPane, CXTPStatusBarPaneWnd)
	//{{AFX_MSG_MAP(CXTPStatusBarIconPane)
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTPStatusBarIconPane::DoPaint(CDC* pDC, CRect& rcClient)
{
	if (m_hIcon)
	{
		// determine the x-y location of the icon
		CPoint point;
		point.x = (rcClient.Width() -m_sizeIcon.cx)/2;
		point.y = (rcClient.Height()-m_sizeIcon.cy)/2;

		// draw the icon associated with this pane.
		if (IsWindowEnabled())
		{
			pDC->DrawState(point, m_sizeIcon, m_hIcon,
				DST_ICON | DSS_NORMAL, (HBRUSH)NULL);
		}
		else
		{
			CBrush brush(GetXtremeColor(COLOR_3DSHADOW));
			pDC->DrawState(point, m_sizeIcon, m_hIcon,
				DST_ICON | DSS_MONO, (HBRUSH)brush);
		}
	}
}

void CXTPStatusBarIconPane::SetPaneIcon(LPCTSTR lpszIconID)
{
	// Locate the resource handle for the icon id.
	HINSTANCE hInstance = AfxFindResourceHandle(lpszIconID, RT_GROUP_ICON);

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
	}

	// Load the image for the icon pane.
	m_hIcon = (HICON)::LoadImage(hInstance, lpszIconID, IMAGE_ICON,
		::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
}

void CXTPStatusBarIconPane::SetPaneIcon(int nIconID)
{
	SetPaneIcon(MAKEINTRESOURCE(nIconID));
}

void CXTPStatusBarIconPane::OnEnable(BOOL bEnable)
{
	CXTPStatusBarPaneWnd::OnEnable(bEnable);

	// Refresh the icon to indicate state.
	Invalidate();
	UpdateWindow();
}



