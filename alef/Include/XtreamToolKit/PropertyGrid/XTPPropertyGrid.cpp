// XTPPropertyGrid.cpp : implementation of the CXTPPropertyGrid class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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
#include "Common/Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPToolTipContext.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int SPLITTER_HEIGHT = 3;
const int TOOLBAR_HEIGHT = 25;

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridUpdateContext

CXTPPropertyGridUpdateContext::CXTPPropertyGridUpdateContext()
{
	m_nSelected = 0;
	m_nTopIndex = 0;
	m_propertySort = xtpGridSortAlphabetical;


}

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridVerb

CXTPPropertyGridVerb::CXTPPropertyGridVerb()
{
	m_nID = 0;
	m_nIndex = -1;
	m_pVerbs = 0;

}

//////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridVerbs

CXTPPropertyGridVerbs::CXTPPropertyGridVerbs()
{
	m_pGrid = 0;


}

CXTPPropertyGridVerbs::~CXTPPropertyGridVerbs()
{
	for (int i = 0; i < m_arrVerbs.GetSize(); i++)
		m_arrVerbs[i]->InternalRelease();
}

void CXTPPropertyGridVerbs::RemoveAll()
{
	if (IsEmpty())
		return;

	for (int i = 0; i < m_arrVerbs.GetSize(); i++)
		m_arrVerbs[i]->InternalRelease();

	m_arrVerbs.RemoveAll();

	m_pGrid->OnVerbsChanged();
}

int CXTPPropertyGridVerbs::GetCount() const
{
	return (int)m_arrVerbs.GetSize();
}

void CXTPPropertyGridVerbs::Add(LPCTSTR strCaption, UINT nID)
{
	CXTPPropertyGridVerb* pVerb = new CXTPPropertyGridVerb();

	pVerb->m_nID = nID;
	pVerb->m_strCaption = strCaption;
	pVerb->m_pVerbs = this;
	pVerb->m_rcPart.SetRectEmpty();
	pVerb->m_ptClick = CPoint(0);

	pVerb->m_nIndex = (UINT)m_arrVerbs.Add(pVerb);

	m_pGrid->OnVerbsChanged();
}


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridToolBar

CXTPPropertyGridToolBar::CXTPPropertyGridToolBar()
{
	m_cxLeftBorder = m_cxRightBorder = m_cyBottomBorder = m_cyTopBorder = 0;

}

CXTPPropertyGridToolBar::~CXTPPropertyGridToolBar()
{
}

BEGIN_MESSAGE_MAP(CXTPPropertyGridToolBar, CToolBar)
	//{{AFX_MSG_MAP(CToolBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXTPPropertyGridToolBar::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CXTPPropertyGridToolBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS* /*lpncsp*/)
{
}

void CXTPPropertyGridToolBar::OnPaint()
{
	CPaintDC dc(this);

	CXTPClientRect rc(this);
	CXTPBufferDC memDC(dc, rc);
	memDC.FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));

	CToolBar::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

void CXTPPropertyGridToolBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}

BOOL CXTPPropertyGridToolBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGrid

CXTPPropertyGrid::CXTPPropertyGrid()
: m_bHelpVisible(TRUE)
, m_bToolBarVisible(FALSE)
, m_nHelpHeight(58)
, m_bPreSubclassWindow(TRUE)
{
	RegisterWindowClass();

	m_pView = 0;

	m_hCursorSplit = XTPResourceManager()->LoadCursor(XTP_IDC_VSPLITBAR);

	m_hCursorHand = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));

	if (m_hCursorHand == 0)
		m_hCursorHand = XTPResourceManager()->LoadCursor(XTP_IDC_HAND);


	m_pPaintManager = new CXTPPropertyGridPaintManager(this);
	m_pPaintManager->RefreshMetrics();

	m_themeCurrent = xtpGridThemeDefault;

	m_pVerbs = new CXTPPropertyGridVerbs;
	m_pVerbs->m_pGrid = this;

	m_nVerbsHeight = 25;
	m_nFocusedVerb = -1;
	m_bVerbActivate = FALSE;
	m_bVerbsVisible = FALSE;

	m_bTabItems = FALSE;
	m_bVariableItemsHeight = FALSE;

	m_borderStyle = xtpGridBorderStaticEdge;

	m_bShowInplaceButtonsAlways = FALSE;

	LOGFONT lfIcon;
	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));
	lfIcon.lfWeight = FW_NORMAL;
	lfIcon.lfItalic = FALSE;
	VERIFY(m_pPaintManager->GetItemMetrics()->m_fontNormal.CreateFontIndirect(&lfIcon));

	lfIcon.lfWeight = FW_BOLD;
	VERIFY(m_pPaintManager->GetItemMetrics()->m_fontBold.CreateFontIndirect(&lfIcon));

	m_bHighlightChanged = FALSE;

	m_pImageManager = new CXTPImageManager;

	m_pInplaceEdit = new CXTPPropertyGridInplaceEdit;
	m_pInplaceListBox = new CXTPPropertyGridInplaceList;

	m_bVariableHelpHeight = TRUE;
}

BOOL CXTPPropertyGrid::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	return XTPDrawHelpers()->RegisterWndClass(hInstance, XTPPROPERTYGRID_CLASSNAME, 0);
}

CXTPPropertyGrid::~CXTPPropertyGrid()
{
	if (m_pView)
	{
		delete m_pView;
	}

	CMDTARGET_RELEASE(m_pVerbs);
	CMDTARGET_RELEASE(m_pImageManager);

	SAFE_DELETE(m_pInplaceEdit);
	SAFE_DELETE(m_pInplaceListBox);

	delete m_pPaintManager;
}

void CXTPPropertyGrid::SetInplaceEdit(CXTPPropertyGridInplaceEdit*  pInplaceEdit)
{
	ASSERT(pInplaceEdit);

	SAFE_DELETE(m_pInplaceEdit);
	m_pInplaceEdit = pInplaceEdit;
}

void CXTPPropertyGrid::SetInplaceList(CXTPPropertyGridInplaceList*  pInplaceList)
{
	ASSERT(pInplaceList);

	SAFE_DELETE(m_pInplaceListBox);
	m_pInplaceListBox = pInplaceList;
}


CXTPPropertyGridView* CXTPPropertyGrid::CreateView() const
{
	return new CXTPPropertyGridView();
}

CXTPPropertyGridView& CXTPPropertyGrid::GetGridView() const
{
	if (m_pView == 0)
	{
		m_pView = CreateView();
		m_pView->m_pGrid = (CXTPPropertyGrid*)this;
	}

	return *m_pView;
}

IMPLEMENT_DYNAMIC(CXTPPropertyGrid, CWnd)

BEGIN_MESSAGE_MAP(CXTPPropertyGrid, CWnd)
	//{{AFX_MSG_MAP(CXTPPropertyGrid)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(XTP_IDC_PROPERTYGRID_ALPHABETIC, OnSortAlphabetic)
	ON_COMMAND(XTP_IDC_PROPERTYGRID_CATEGORIZED, OnSortCategorized)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

UINT CXTPPropertyGrid::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

BOOL CXTPPropertyGrid::CreateGridView(DWORD dwListStyle)
{
	CXTPEmptyRect rect;

	if (dwListStyle == NULL)
	{
		dwListStyle = m_bVariableItemsHeight ? LBS_OWNERDRAWVARIABLE | LBS_NOINTEGRALHEIGHT :
			LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT;
	}

	// Must be owner draw.
	ASSERT((dwListStyle & (LBS_OWNERDRAWVARIABLE|LBS_OWNERDRAWFIXED)) != 0);

	m_bVariableItemsHeight = (dwListStyle & LBS_OWNERDRAWVARIABLE) != 0;

	if (!GetGridView().CreateEx(0, _T("LISTBOX"), NULL,
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE |
		LBS_NOTIFY | WS_VSCROLL | WS_TABSTOP | dwListStyle, rect, this, 0))
	{
		TRACE0("Error creating property grid view.\n");
		return FALSE;
	}

	GetGridView().ResetContent();
	SetStandardColors();

	return TRUE;
}

BOOL CXTPPropertyGrid::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwListStyle /*= LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT*/)
{
	if (!CreateEx(0, XTPPROPERTYGRID_CLASSNAME, _T(""),
		WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_GROUP, rect, pParentWnd, nID))
	{
		TRACE0("Error creating property grid.\n");
		return FALSE;
	}

	if (!CreateGridView(dwListStyle))
		return FALSE;

	Reposition(rect.right - rect.left, rect.bottom - rect.top);

	SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		SWP_SHOWWINDOW|SWP_NOZORDER);

	return TRUE;
}

BOOL CXTPPropertyGrid::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	//
	// PreCreateWindow is called when a control is dynamically
	// created. We want to set m_bPreSubclassWindow to FALSE
	// here so the control is initialized from CWnd::Create and
	// not CWnd::PreSubclassWindow.
	//

	m_bPreSubclassWindow = FALSE;

	return TRUE;
}

void CXTPPropertyGrid::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();

	if (m_bPreSubclassWindow)
	{
		// Initialize the control.
		VERIFY(CreateGridView(NULL));

		Reposition();
	}
}

void CXTPPropertyGrid::SetVariableItemsHeight(BOOL bVariable /*= TRUE*/)
{
	if (m_bVariableItemsHeight == bVariable)
		return;

	m_bVariableItemsHeight = bVariable;

	CXTPPropertyGridView& wndView = GetGridView();
	if (!wndView.GetSafeHwnd())
		return;

	wndView.ResetContent();
	wndView.DestroyWindow();

	DWORD dwListStyle = bVariable ? LBS_OWNERDRAWVARIABLE | LBS_NOINTEGRALHEIGHT : LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT;

	VERIFY(GetGridView().CreateEx(0, _T("LISTBOX"), NULL, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_TABSTOP | dwListStyle, CXTPEmptyRect(), this, 0));

	wndView.Refresh();

	Reposition();
}


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGrid message handlers

BOOL CXTPPropertyGrid::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CXTPPropertyGrid::OnPaint()
{
	CPaintDC dcPaint(this);
	CXTPClientRect rc(this);
	CXTPBufferDC dc(dcPaint, rc);

	m_pPaintManager->FillPropertyGrid(&dc);


}

CWnd* CXTPPropertyGrid::GetNextGridTabItem(BOOL bForward)
{
	CWnd* pThis = this;
	CWnd* pParent = pThis->GetParent();

	if (!pThis || !pParent)
	{
		ASSERT(FALSE);
		return 0 ;
	}

	if ((pParent->GetExStyle() & WS_EX_CONTROLPARENT) && (pParent->GetStyle() & WS_CHILD))
	{
		pParent = pParent->GetParent();
	}

	CWnd* pNextItem = pParent->GetNextDlgTabItem(pThis, bForward);

	if (pNextItem == pThis)
		return NULL;

	return pNextItem;
}

void CXTPPropertyGrid::OnNavigate(XTPPropertyGridUI nUIElement, BOOL bForward, CXTPPropertyGridItem* pItem)
{
	int nNextElement = bForward ? +1 : -1;
	int nUI = nUIElement + nNextElement;

	CXTPPropertyGridView& wndView = GetGridView();

	if (pItem == NULL)
		pItem = wndView.GetSelectedItem();

	while (nUI != nUIElement)
	{
		if (nUI == xtpGridUIViewPrev)
		{
			nUI += nNextElement;

			if (m_bTabItems)
			{
				int nIndex = !bForward && pItem ? pItem->m_nIndex - 1 : 0;
				if (nIndex >= 0)
				{
					wndView.SetFocus();
					wndView.SetCurSel(nIndex);
					wndView.OnSelectionChanged();

					if (bForward)
						return;

					nUI = xtpGridUIInplaceButton;
					pItem = wndView.GetSelectedItem();
				}
			}
		}
		if (nUI == xtpGridUIView)
		{
			wndView.SetFocus();
			wndView.FocusInplaceButton(NULL);
			return;
		}
		if (nUI == xtpGridUIInplaceEdit)
		{
			if (pItem && pItem->GetFlags() & xtpGridItemHasEdit)
			{
				pItem->OnSelect();
				pItem->SetFocusToInplaceControl();
				return;
			}
			nUI += nNextElement;
		}
		if (nUI == xtpGridUIInplaceControl)
		{
			if (pItem && pItem->GetInplaceControls()->GetCount() > 0)
			{
				CWnd* pWnd = pItem->GetInplaceControls()->GetAt(0);
				if (pWnd && pWnd->GetSafeHwnd() && pWnd->GetStyle() & WS_TABSTOP)
				{
					pWnd->SetFocus();
					return;
				}
			}
			nUI += nNextElement;
		}
		if (nUI == xtpGridUIInplaceButton)
		{
			if (pItem && pItem->IsInplaceButtonsVisible())
			{
				CXTPPropertyGridInplaceButtons* pButtons = pItem->GetInplaceButtons();

				if (wndView.m_pFocusedButton && wndView.m_pFocusedButton->GetItem() == pItem)
				{
					int nIndex = wndView.m_pFocusedButton->GetIndex();
					if (bForward && nIndex < pButtons->GetCount() -1)
					{
						wndView.FocusInplaceButton(pButtons->GetAt(nIndex + 1));
						return;
					}
					else if (!bForward && nIndex > 0)
					{
						wndView.FocusInplaceButton(pButtons->GetAt(nIndex - 1));
						return;
					}
				}
				else
				{
					wndView.FocusInplaceButton(pButtons->GetAt(bForward ? 0 : pButtons->GetCount() - 1));
					return;
				}
			}
			nUI += nNextElement;
		}
		if (nUI == xtpGridUIViewNext)
		{
			if (m_bTabItems)
			{
				int nIndex = bForward ? (pItem ? pItem->m_nIndex + 1 : 0) : wndView.GetCount() - 1;
				if (nIndex < wndView.GetCount())
				{
					wndView.SetFocus();
					wndView.SetCurSel(nIndex);
					wndView.OnSelectionChanged();
					if (bForward)
						return;
					pItem = wndView.GetSelectedItem();
				}
			}

			nUI += nNextElement;
		}
		if (nUI == xtpGridUIVerb)
		{
			if (IsVerbsVisible())
			{
				m_nFocusedVerb = bForward ? 0 : m_pVerbs->GetCount() - 1;
				m_bVerbActivate = TRUE;
				SetFocus();
				m_bVerbActivate = FALSE;
				Invalidate(FALSE);
				return;
			}
			nUI += nNextElement;
		}
		if (nUI == xtpGridUIParentNext || nUI == xtpGridUIParentPrev)
		{
			CWnd* pWndNext = GetNextGridTabItem(!bForward);

			if (pWndNext != NULL)
			{
				pWndNext->SetFocus();
				return;
			}

			nUI = (nUI == xtpGridUIParentNext) ? xtpGridUIViewPrev : xtpGridUIVerb;
		}
		if (nUI < xtpGridUIParentPrev || nUI > xtpGridUIParentNext)
		{
			ASSERT(FALSE);
			return;
		}
	}
}

void CXTPPropertyGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_nFocusedVerb != -1 && IsVerbsVisible())
	{
		if (m_nFocusedVerb >= m_pVerbs->GetCount())
			m_nFocusedVerb = m_pVerbs->GetCount() -1;

		BOOL bForward = GetKeyState(VK_SHIFT) >= 0;
		CXTPDrawHelpers::KeyToLayout(this, nChar);

		if ((nChar == VK_TAB) && bForward && (m_nFocusedVerb == m_pVerbs->GetCount() -1))
		{
			OnNavigate(xtpGridUIVerb, TRUE, NULL);
			return;

		}
		else if ((nChar == VK_TAB) && !bForward && (m_nFocusedVerb == 0))
		{
			OnNavigate(xtpGridUIVerb, FALSE, NULL);
			return;
		}
		else if (nChar == VK_RIGHT || nChar == VK_DOWN || ((nChar == VK_TAB) && bForward))
		{
			m_nFocusedVerb++;
			if (m_nFocusedVerb >= m_pVerbs->GetCount())
				m_nFocusedVerb = 0;
			Invalidate(FALSE);
		}
		else if (nChar == VK_LEFT || nChar == VK_UP || nChar == VK_TAB)
		{
			m_nFocusedVerb--;
			if (m_nFocusedVerb < 0)
				m_nFocusedVerb = m_pVerbs->GetCount() -1;
			Invalidate(FALSE);
		}
		else if (nChar == VK_RETURN)
		{
			CRect rcPart = m_pVerbs->GetAt(m_nFocusedVerb)->GetPart();
			OnVerbClick(m_nFocusedVerb, CPoint(rcPart.left, rcPart.bottom));
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGrid::OnKillFocus(CWnd* pNewWnd)
{
	if (m_nFocusedVerb != -1)
	{
		m_nFocusedVerb = -1;
		Invalidate(FALSE);
	}
	CWnd::OnKillFocus(pNewWnd);
}

void CXTPPropertyGrid::OnSetFocus(CWnd* pOldWnd)
{
	if (!m_bVerbActivate)
	{
		if (pOldWnd && pOldWnd == GetNextGridTabItem(FALSE) && GetKeyState(VK_SHIFT) < 0)
		{
			OnNavigate(xtpGridUIParentNext, FALSE, NULL);
		}
		else if (pOldWnd && pOldWnd == GetNextGridTabItem(TRUE) && GetKeyState(VK_SHIFT) >= 0)
		{
			OnNavigate(xtpGridUIParentPrev, TRUE, NULL);
		}
		else
		{
			GetGridView().SetFocus();
		}
	}
	else
	{
		CWnd::OnSetFocus(pOldWnd);
	}
}

void CXTPPropertyGrid::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!GetGridView().GetSafeHwnd())
		return;

	Reposition(cx, cy);
}

int CXTPPropertyGrid::HitTest(CPoint pt)
{
	CXTPClientRect rc(this);

	if (m_bHelpVisible)
	{
		CRect rcSplitter(CPoint(rc.left, rc.bottom - SPLITTER_HEIGHT - m_nHelpHeight), CSize(rc.Width(), SPLITTER_HEIGHT));

		if (m_bVariableHelpHeight && rcSplitter.PtInRect(pt))
			return xtpGridHitHelpSplitter;

		rc.bottom -= SPLITTER_HEIGHT + m_nHelpHeight;
	}
	if (IsVerbsVisible())
	{
		CRect rcSplitter(CPoint(rc.left, rc.bottom - SPLITTER_HEIGHT - m_nVerbsHeight), CSize(rc.Width(), SPLITTER_HEIGHT));

		if (rcSplitter.PtInRect(pt))
			return xtpGridHitVerbsSplitter;

		CRect rcVerbs(rc);
		rcVerbs.top = rc.bottom - m_nVerbsHeight;

		if (rcVerbs.PtInRect(pt))
		{
			CWindowDC dc(this);
			int nIndex = m_pPaintManager->HitTestVerbs(&dc, rcVerbs, pt);
			if (nIndex != -1)
			{
				return xtpGridHitFirstVerb + nIndex;
			}
		}
	}

	return xtpGridHitError;
}

BOOL CXTPPropertyGrid::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		GetCursorPos(&point);
		ScreenToClient(&point);

		int nHit = HitTest(point);

		if (nHit == xtpGridHitHelpSplitter || nHit == xtpGridHitVerbsSplitter)
		{
			SetCursor(m_hCursorSplit);
			return TRUE;
		}
		if ((nHit != -1) && (nHit >= xtpGridHitFirstVerb))
		{
			SetCursor(m_hCursorHand);
			return TRUE;
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CXTPPropertyGrid::OnInvertTracker(CRect rect)
{
	ASSERT_VALID(this);
	ASSERT(!rect.IsRectEmpty());
	ASSERT((GetStyle() & WS_CLIPCHILDREN) == 0);

	// pat-blt without clip children on
	CDC* pDC = GetDC();

	CBrush brush(GetXtremeColor(COLOR_3DFACE));
	CBrush* pBrush = (CBrush*)pDC->SelectObject(&brush);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
	pDC->SelectObject(pBrush);

	ReleaseDC(pDC);
}

void CXTPPropertyGrid::OnSelectionChanged(CXTPPropertyGridItem* pItem)
{
	if (!m_hWnd)
		return;

	CXTPClientRect rc(this);
	CRect rcHelp(rc);
	rcHelp.top = rc.bottom - m_nHelpHeight;

	if (pItem) SendNotifyMessage(XTP_PGN_SELECTION_CHANGED, (LPARAM)pItem);

	InvalidateRect(rcHelp, FALSE);
}

void CXTPPropertyGrid::OnVerbsChanged()
{
	if (m_bVerbsVisible != IsVerbsVisible() && GetGridView().m_nLockUpdate == 0)
	{
		Reposition();
	}
	else if (GetSafeHwnd() && GetGridView().m_nLockUpdate == 0)
	{
		Invalidate(FALSE);
	}
}

void CXTPPropertyGrid::OnVerbClick(int nIndex, CPoint pt)
{
	ClientToScreen(&pt);
	ASSERT(nIndex < m_pVerbs->GetCount());

	CXTPPropertyGridVerb* pVerb = m_pVerbs->GetAt(nIndex);
	pVerb->m_ptClick = pt;

	SendNotifyMessage(XTP_PGN_VERB_CLICK, (LPARAM)pVerb);
}

void CXTPPropertyGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ((m_nFocusedVerb != -1) && (m_nFocusedVerb == HitTest(point) - xtpGridHitFirstVerb))
	{
		OnVerbClick(m_nFocusedVerb, point);
	}
	else
	{
		CWnd::OnLButtonUp(nFlags, point);
	}

}

void CXTPPropertyGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nHitTest = HitTest(point);

	if ((nHitTest != -1) && (nHitTest >= xtpGridHitFirstVerb))
	{
		m_nFocusedVerb = nHitTest - xtpGridHitFirstVerb;
		m_bVerbActivate = TRUE;
		SetFocus();
		m_bVerbActivate = FALSE;
		Invalidate(FALSE);
		return;

	}
	SetFocus();

	if ((nHitTest == xtpGridHitHelpSplitter) || (nHitTest == xtpGridHitVerbsSplitter))
	{
		SetCapture();
		CXTPClientRect rc(this);
		BOOL bHelpHitTest = nHitTest == xtpGridHitHelpSplitter;

		CRect rcAvail = bHelpHitTest ? CRect(0, 20 + TOOLBAR_HEIGHT, rc.right, rc.bottom) :
			CRect(0, 20 + TOOLBAR_HEIGHT, rc.right, rc.bottom - (IsHelpVisible() ? m_nHelpHeight + SPLITTER_HEIGHT : 0));

		ModifyStyle(WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, FALSE);

		m_rectTracker.SetRect(0, point.y, rc.Width(), point.y + 3);

		OnInvertTracker(m_rectTracker);

		BOOL bAccept = FALSE;
		while (GetCapture() == this)
		{
			MSG msg;
			if (!GetMessage(&msg, NULL, 0, 0))
				break;

			if (msg.message == WM_MOUSEMOVE)
			{
				point = CPoint(msg.lParam);
				point.y = __min(point.y, rcAvail.bottom - 20);
				point.y = __max((int)point.y, int(rcAvail.top));

				if (m_rectTracker.top != point.y)
				{
					OnInvertTracker(m_rectTracker);
					m_rectTracker.OffsetRect(0, point.y - m_rectTracker.top);
					OnInvertTracker(m_rectTracker);
				}
			}
			else if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) break;
			else if (msg.message == WM_LBUTTONUP)
			{
				bAccept = TRUE;
				break;
			}
			else  ::DispatchMessage(&msg);
		}

		ReleaseCapture();

		if (bAccept)
		{
			if (bHelpHitTest) m_nHelpHeight = rcAvail.bottom - m_rectTracker.top -2; else
				m_nVerbsHeight = rcAvail.bottom - m_rectTracker.top -2;
			Reposition(rc.Width(), rc.Height());
		}
		Invalidate(FALSE);
		ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS, FALSE);
		return;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CXTPPropertyGrid::Reposition()
{
	if (GetSafeHwnd() == 0)
		return;

	CXTPClientRect rc(this);
	Reposition(rc.Width(), rc.Height());
}

void CXTPPropertyGrid::Reposition(int cx, int cy)
{
	if (GetGridView().m_nLockUpdate > 0)
		return;

	if (GetSafeHwnd() == 0)
		return;

	CRect rcView(0, 0, cx, cy);

	if (m_bHelpVisible)
	{
		rcView.bottom -= m_nHelpHeight + SPLITTER_HEIGHT;
	}

	if (IsVerbsVisible())
	{
		rcView.bottom -= m_nVerbsHeight + SPLITTER_HEIGHT;
	}

	m_bVerbsVisible = IsVerbsVisible();

	if (m_bToolBarVisible)
	{
		ASSERT(m_wndToolbar.GetSafeHwnd());
		CRect rcToolBar(1, 1, cx - 1, TOOLBAR_HEIGHT - 1);
		m_wndToolbar.MoveWindow(rcToolBar);

		rcView.top += TOOLBAR_HEIGHT;
	}

	GetGridView().MoveWindow(rcView);
	GetGridView().Invalidate(FALSE);
	Invalidate(FALSE);
}

void CXTPPropertyGrid::CreateToolbar()
{
#if _MSC_VER < 1200
	m_wndToolbar.Create(this, TBSTYLE_FLAT | WS_CHILD | WS_VISIBLE | CBRS_TOOLTIPS | WS_BORDER, 0);
#else
	m_wndToolbar.CreateEx(this, TBSTYLE_FLAT, WS_BORDER | WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
#endif

	VERIFY(XTPResourceManager()->LoadToolBar(&m_wndToolbar, XTP_IDR_PROPERTYGRID_TOOLBAR));
}

void CXTPPropertyGrid::ShowToolBar(BOOL bShow)
{
	if (bShow && !m_wndToolbar.GetSafeHwnd())
	{
		CreateToolbar();
	}
	else
	{
		if (m_wndToolbar.GetSafeHwnd())
			m_wndToolbar.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}

	m_bToolBarVisible = bShow;

	Reposition();
	RefreshToolBarButtons();
}

void CXTPPropertyGrid::RefreshToolBarButtons()
{
	if (m_bToolBarVisible)
	{
		m_wndToolbar.GetToolBarCtrl().SetState(XTP_IDC_PROPERTYGRID_CATEGORIZED, TBSTATE_ENABLED | (GetGridView().m_properetySort == xtpGridSortCategorized ? TBSTATE_CHECKED : 0));
		m_wndToolbar.GetToolBarCtrl().SetState(XTP_IDC_PROPERTYGRID_ALPHABETIC, TBSTATE_ENABLED | (GetGridView().m_properetySort == xtpGridSortAlphabetical ? TBSTATE_CHECKED : 0));
	}
}

void CXTPPropertyGrid::ShowHelp(BOOL bShow)
{
	m_bHelpVisible = bShow;

	Reposition();
}

void CXTPPropertyGrid::OnSortAlphabetic()
{
	SetPropertySort(xtpGridSortAlphabetical);
}

void CXTPPropertyGrid::OnSortCategorized()
{
	SetPropertySort(xtpGridSortCategorized);
}

void CXTPPropertyGrid::OnSortChanged()
{
	RefreshToolBarButtons();
	OnSelectionChanged(NULL);
	SendNotifyMessage(XTP_PGN_SORTORDER_CHANGED, (LPARAM)GetDlgCtrlID());
}

LRESULT CXTPPropertyGrid::SendNotifyMessage(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	return GetOwner()->SendMessage(XTPWM_PROPERTYGRID_NOTIFY, wParam, lParam);
}

void CXTPPropertyGrid::SetStandardColors()
{
	m_pPaintManager->GetItemMetrics()->SetDefaultValues();

	m_pPaintManager->RefreshMetrics();
	RedrawControl();
}

void CXTPPropertyGrid::RedrawControl()
{
	if (GetSafeHwnd())
	{
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ERASE);
	}
}

void CXTPPropertyGrid::SetCustomColors(
	COLORREF clrHelpBack, COLORREF clrHelpFore, COLORREF clrViewLine,
	COLORREF clrViewBack, COLORREF clrViewFore, COLORREF clrCategoryFore)
{
	CXTPPropertyGridItemMetrics* pMetrics = m_pPaintManager->GetItemMetrics();

	pMetrics->m_clrHelpBack.SetCustomValue(clrHelpBack);
	pMetrics->m_clrHelpFore.SetCustomValue(clrHelpFore);
	pMetrics->m_clrLine.SetCustomValue(clrViewLine);
	pMetrics->m_clrBack.SetCustomValue(clrViewBack);
	pMetrics->m_clrFore.SetCustomValue(clrViewFore);
	pMetrics->m_clrCategoryFore.SetCustomValue(clrCategoryFore);

	RedrawControl();
}

void CXTPPropertyGrid::ResetContent()
{
	m_pVerbs->RemoveAll();
	GetGridView().ResetContent();

	GetGridView().m_nLockUpdate++;
	GetGridView().m_pCategories->Clear();
	GetGridView().m_nLockUpdate--;
}

CXTPPropertyGridItems* CXTPPropertyGrid::GetCategories() const
{
	return GetGridView().m_pCategories;
}

CXTPPropertyGridItem* CXTPPropertyGrid::AddCategory(int nID)
{
	CString strCaption;
	VERIFY(strCaption.LoadString(nID));
	CXTPPropertyGridItem* pItem = GetGridView().AddCategory(strCaption);
	pItem->SetID(nID);
	return pItem;
}

CXTPPropertyGridItem* CXTPPropertyGrid::InsertCategory(int nIndex, int nID)
{
	CString strCaption;
	VERIFY(strCaption.LoadString(nID));
	CXTPPropertyGridItem* pItem = GetGridView().InsertCategory(nIndex, strCaption);
	pItem->SetID(nID);
	return pItem;
}


void CXTPPropertyGrid::SetFont(CFont* pFont)
{
	LOGFONT lf;
	if (pFont->GetLogFont(&lf))
	{
		CXTPPropertyGridItemMetrics* pMetrics = m_pPaintManager->GetItemMetrics();

		pMetrics->m_fontNormal.DeleteObject();
		pMetrics->m_fontBold.DeleteObject();

		lf.lfWeight = FW_NORMAL;
		VERIFY(pMetrics->m_fontNormal.CreateFontIndirect(&lf));

		lf.lfWeight = FW_BOLD;
		VERIFY(pMetrics->m_fontBold.CreateFontIndirect(&lf));

		if (m_pInplaceEdit->GetSafeHwnd())
		{
			m_pInplaceEdit->SetFont(&pMetrics->m_fontNormal);
		}

		Refresh();
	}
}

void CXTPPropertyGrid::Refresh()
{
	GetGridView().SetPropertySort(GetPropertySort(), TRUE);
}


void CXTPPropertyGrid::SaveExpandingState(CXTPPropertyGridItems* pItems, CXTPPropertyGridUpdateContext& context)
{
	int nTopIndex = GetGridView().GetTopIndex();

	for (int nItem = 0; nItem < pItems->GetCount(); nItem++)
	{
		CXTPPropertyGridItem* pItem = pItems->GetAt(nItem);

		if (pItem->GetID() != 0)
		{
			if (pItem->HasChilds())
			{
				BOOL bExpanded = pItem->IsExpanded();
				context.m_mapState.SetAt(pItem->GetID(), bExpanded);
			}

			if (pItem->IsSelected()) context.m_nSelected = pItem->GetID();
			if (pItem->GetIndex() == nTopIndex) context.m_nTopIndex= pItem->GetID();
		}
		SaveExpandingState(pItem->GetChilds(), context);
	}
}

void CXTPPropertyGrid::RestoreExpandingState(CXTPPropertyGridItems* pItems, CXTPPropertyGridUpdateContext& context)
{
	for (int nItem = 0; nItem < pItems->GetCount(); nItem++)
	{
		CXTPPropertyGridItem* pItem = pItems->GetAt(nItem);

		BOOL bExpanded;
		if (pItem->GetID() != 0 && context.m_mapState.Lookup(pItem->GetID(), bExpanded))
		{
			if (bExpanded)
				pItem->Expand();
			else
				pItem->Collapse();
		}
		RestoreExpandingState(pItem->GetChilds(), context);
	}
}

void CXTPPropertyGrid::BeginUpdate(CXTPPropertyGridUpdateContext& context)
{
	GetGridView().m_nLockUpdate = 1;
	GetGridView().SetRedraw(FALSE);

	context.m_nSelected = 0;
	context.m_nTopIndex = 0;

	SaveExpandingState(GetCategories(), context);
	context.m_propertySort = GetPropertySort();

	ResetContent();
}

void CXTPPropertyGrid::EndUpdate(CXTPPropertyGridUpdateContext& context)
{
	RestoreExpandingState(GetCategories(), context);
	GetGridView().m_nLockUpdate = 0;

	GetGridView().SetPropertySort(context.m_propertySort, TRUE, FALSE);
	if (context.m_nSelected > 0 || context.m_nTopIndex > 0)
	{
		for (int i = 0; i < GetGridView().GetCount(); i++)
		{
			CXTPPropertyGridItem* pItem = GetGridView().GetItem(i);

			if (context.m_nTopIndex > 0 && pItem && pItem->GetID() == context.m_nTopIndex)
			{
				SetTopIndex(pItem->GetIndex());
				context.m_nTopIndex = 0;
				if (context.m_nSelected == 0)
					break;
			}

			if (context.m_nSelected > 0 && pItem && pItem->GetID() == context.m_nSelected)
			{
				pItem->Select();
				break;
			}
		}
	}

	GetGridView().SetRedraw(TRUE);

	Reposition();
}

void CXTPPropertyGrid::SetVerbsHeight(int nHeight)
{
	ASSERT(nHeight > 0);
	m_nVerbsHeight = nHeight;

	Reposition();
}

void CXTPPropertyGrid::SetHelpHeight(int nHeight)
{

	ASSERT(nHeight > 0);
	m_nHelpHeight = nHeight;

	Reposition();
}

void CXTPPropertyGrid::SetViewDivider(double dDivider)
{
	ASSERT(dDivider > 0 && dDivider < 1);
	GetGridView().m_dDivider = dDivider;

	Reposition();
}

double CXTPPropertyGrid::GetViewDivider() const
{
	return GetGridView().m_dDivider;
}

void CXTPPropertyGrid::SetViewDividerPos(int nDivider, BOOL bLockDivider /*= FALSE*/)
{
	if (bLockDivider)
	{
		GetGridView().LockDivider();
	}

	GetGridView().SetDividerPos(nDivider);

	Reposition();
}

CXTPPropertyGridItem* CXTPPropertyGrid::GetSelectedItem() const
{
	return GetGridView().GetSelectedItem();
}

int CXTPPropertyGrid::GetTopIndex() const
{
	return GetGridView().GetTopIndex();
}

void CXTPPropertyGrid::SetTopIndex(int nIndex)
{
	GetGridView().SetTopIndex(nIndex);
}

void CXTPPropertyGrid::SetTheme(XTPPropertyGridPaintTheme paintTheme)
{
	if (paintTheme == xtpGridThemeDefault) SetCustomTheme(new CXTPPropertyGridPaintManager(this));
	else if (paintTheme == xtpGridThemeCool) SetCustomTheme(new CXTPPropertyGridCoolTheme(this));
	else if (paintTheme == xtpGridThemeDelphi) SetCustomTheme(new CXTPPropertyGridDelphiTheme(this));
	else if (paintTheme == xtpGridThemeNativeWinXP) SetCustomTheme(new CXTPPropertyGridNativeXPTheme(this));
	else if (paintTheme == xtpGridThemeOffice2003) SetCustomTheme(new CXTPPropertyGridOffice2003Theme(this));
	else if (paintTheme == xtpGridThemeSimple) SetCustomTheme(new CXTPPropertyGridSimpleTheme(this));
	else if (paintTheme == xtpGridThemeWhidbey) SetCustomTheme(new CXTPPropertyGridWhidbeyTheme(this));
	else if (paintTheme == xtpGridThemeOfficeXP) SetCustomTheme(new CXTPPropertyGridOfficeXP(this));
	else
	{
		ASSERT(FALSE);
	}

	m_themeCurrent = paintTheme;
}

void CXTPPropertyGrid::SetCustomTheme(CXTPPropertyGridPaintManager* pPaintManager)
{
	CXTPPropertyGridItemMetrics* pMetrics = m_pPaintManager->m_pMetrics;
	m_pPaintManager->m_pMetrics = 0;

	ASSERT(m_pPaintManager);
	delete m_pPaintManager;

	m_pPaintManager = pPaintManager;

	CMDTARGET_RELEASE(m_pPaintManager->m_pMetrics);
	m_pPaintManager->m_pMetrics = pMetrics;
	m_pPaintManager->RefreshMetrics();

	RedrawControl();
}

CXTPImageManager* CXTPPropertyGrid::GetImageManager() const
{
	return m_pImageManager ? m_pImageManager : XTPImageManager();
}

void CXTPPropertyGrid::SetImageManager(CXTPImageManager* pImageManager)
{
	if (m_pImageManager)
		m_pImageManager->InternalRelease();

	m_pImageManager = pImageManager;
}

void CXTPPropertyGrid::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	m_pPaintManager->RefreshMetrics();
}

BOOL CXTPPropertyGrid::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	CString strTipText;

	// don't handle the message if no string resource found
	if (!XTPResourceManager()->LoadString(&strTipText, (UINT)pNMHDR->idFrom))
		return FALSE;

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#endif
	*pResult = 0;

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);

	return TRUE;    // message was handled
}


BOOL CXTPPropertyGrid::IsLayoutRTL() const
{
	return GetExStyle() & WS_EX_LAYOUTRTL ? TRUE : FALSE;
}

void CXTPPropertyGrid::SetLayoutRTL(CWnd* pWnd, BOOL bRTLLayout)
{
	pWnd->ModifyStyleEx(bRTLLayout ? 0 : WS_EX_LAYOUTRTL, !bRTLLayout ? 0 : WS_EX_LAYOUTRTL);

	// walk through HWNDs to avoid creating temporary CWnd objects
	// unless we need to call this function recursively
	for (CWnd* pChild = pWnd->GetWindow(GW_CHILD); pChild != NULL;
		pChild = pChild->GetWindow(GW_HWNDNEXT))
	{

		// send to child windows after parent
		SetLayoutRTL(pChild, bRTLLayout);
	}
}


void CXTPPropertyGrid::SetLayoutRTL(BOOL bRightToLeft)
{
	if (!XTPSystemVersion()->IsLayoutRTLSupported())
		return;

	SetLayoutRTL(this, bRightToLeft);

	if (m_pInplaceEdit && m_pInplaceEdit->GetSafeHwnd())
	{
		m_pInplaceEdit->HideWindow();
		m_pInplaceEdit->m_bDelayCreate = TRUE;
	}

	if (GetSelectedItem()) GetSelectedItem()->OnSelect();

	Reposition();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ERASE);
}


void CXTPPropertyGrid::SetBorderStyle(XTPPropertyGridBorderStyle borderStyle)
{
	m_borderStyle = borderStyle;

	Reposition();

	if (GetGridView().GetSafeHwnd())
	{
		GetGridView().SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

XTPPropertyGridBorderStyle CXTPPropertyGrid::GetBorderStyle() const
{
	return m_borderStyle;
}


