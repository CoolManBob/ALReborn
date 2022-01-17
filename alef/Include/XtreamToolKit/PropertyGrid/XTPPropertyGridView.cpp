// XTPPropertyGridView.cpp : implementation of the CXTPPropertyGridView class.
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
#include "Common/Resource.h"

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridDefines.h"
#include "XTPPropertyGridPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const UINT PGV_HIT_SPLITTER = 0x100;

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridToolTip

CXTPPropertyGridToolTip::CXTPPropertyGridToolTip()
{
	m_pGrid = NULL;
}

BEGIN_MESSAGE_MAP(CXTPPropertyGridToolTip, CWnd)
	//{{AFX_MSG_MAP(CXTPPropertyGridView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCHITTEST_EX()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LRESULT CXTPPropertyGridToolTip::OnNcHitTest(CPoint /*point*/)
{
	return (LRESULT)HTTRANSPARENT;
}
void CXTPPropertyGridToolTip::Create(CXTPPropertyGridView* pParentWnd)
{
	CWnd::CreateEx(0, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), _T(""), WS_POPUP, CXTPEmptyRect(), pParentWnd, 0);
	m_pGrid = pParentWnd;

}

void CXTPPropertyGridToolTip::Activate(BOOL bActive, CXTPPropertyGridItem* pItem, BOOL bValuePart)
{
	if (bActive)
	{
		ASSERT(pItem);
		m_fnt.DeleteObject();
		LOGFONT lf;
		m_pGrid->GetPaintManager()->GetItemFont(pItem, bValuePart)->GetLogFont(&lf);

		m_fnt.CreateFontIndirect(&lf);

		CString strText;
		GetWindowText(strText);
		CWindowDC dc(this);
		CXTPFontDC font(&dc, &m_fnt);
		CXTPWindowRect rc (this);
		rc.right = rc.left + dc.GetTextExtent(strText).cx + 8;

		CRect rcWork = XTPMultiMonitor()->GetWorkArea();

		if (rc.right > rcWork.right)
			rc.OffsetRect(rcWork.right - rc.right, 0);

		if (rc.left < rcWork.left)
			rc.OffsetRect(rcWork.left - rc.left, 0);

		MoveWindow(rc);
		ShowWindow(SW_SHOWNOACTIVATE);
	}
	else
	{
		DestroyWindow();
	}
}

BOOL CXTPPropertyGridToolTip::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CXTPPropertyGridToolTip::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rc(this);

	COLORREF clrText = GetXtremeColor(COLOR_INFOTEXT);
	dc.FillSolidRect(rc, GetXtremeColor(COLOR_INFOBK));
	dc.Draw3dRect(rc, clrText, clrText);

	CString strText;
	GetWindowText(strText);
	dc.SetTextColor(clrText);
	dc.SetBkMode(TRANSPARENT);

	CXTPFontDC font(&dc, &m_fnt);
	CRect rcText(rc);
	rcText.left += 4;

	dc.DrawText(strText, rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

}

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridView

CXTPPropertyGridView::CXTPPropertyGridView()
{
	m_properetySort = xtpGridSortCategorized;

	m_dDivider = .5;
	m_bAutoDivider = TRUE;

	m_bTracking = FALSE;

	m_bVariableSplitterPos = TRUE;

	m_pSelected = NULL;
	m_pCategories = new CXTPPropertyGridItems();

	m_hCursor = XTPResourceManager()->LoadCursor(XTP_IDC_HSPLITBAR);
	m_nLockUpdate = 0;
	m_pGrid = NULL;
	m_nItemHeight = 0;

	m_pFocusedButton = NULL;
	m_pHotButton = NULL;

	EnableAutomation();
}

CXTPPropertyGridView::~CXTPPropertyGridView()
{
	m_wndTip.DestroyWindow();
	DestroyWindow();

	m_pCategories->Clear();
	m_pCategories->InternalRelease();

}

IMPLEMENT_DYNAMIC(CXTPPropertyGridView, CListBox)

BEGIN_MESSAGE_MAP(CXTPPropertyGridView, CListBox)
	//{{AFX_MSG_MAP(CXTPPropertyGridView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelectionChanged)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()

	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()

	ON_WM_CHAR()
	ON_WM_GETDLGCODE()

	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()

	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()

	ON_MESSAGE(WM_GETOBJECT, OnGetObject)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridView message handlers
void CXTPPropertyGridView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetScrollBarCtrl(SB_VERT) == pScrollBar)
	{
		OnScrollControl();
	}
	else
	{
		CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}

struct CXTPPropertyGridView::WNDRECT
{
	HWND hWnd;
	RECT rc;
};

LRESULT CXTPPropertyGridView::OnScrollControl()
{
	if (GetCount() == 0)
		return 0;

	CArray<WNDRECT, WNDRECT&> arrRects;

	SetRedraw(FALSE);

	RECT rcTopOrig = {0, 0, 0, 0};
	GetItemRect(0, &rcTopOrig);

	HWND hWnd = ::GetWindow(m_hWnd, GW_CHILD);
	while (hWnd)
	{
		if (GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE)
		{
			RECT rc;
			::GetWindowRect(hWnd, &rc);

			WNDRECT wndRect;
			wndRect.hWnd = hWnd;
			wndRect.rc = rc;
			arrRects.Add(wndRect);
		}

		hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	}

	LRESULT lResult = Default();

	SetRedraw(TRUE);

	RECT rcTopDest = {0, 0, 0, 0};
	GetItemRect(0, &rcTopDest);
	int nOffset = rcTopDest.top - rcTopOrig.top;

	for (int i = 0; i < (int)arrRects.GetSize(); i++)
	{
		WNDRECT& wndRect = arrRects[i];
		RECT rc = wndRect.rc;
		ScreenToClient(&rc);

		::OffsetRect(&rc, 0, nOffset);
		::SetWindowPos(wndRect.hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}

	RedrawWindow(0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ALLCHILDREN);

	return lResult;
}

BOOL CXTPPropertyGridView::OnMouseWheel(UINT /*nFlags*/, short /*zDelta*/, CPoint /*pt*/)
{
	return (BOOL)OnScrollControl();
}


void CXTPPropertyGridView::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nItemHeight;

	GetPaintManager()->MeasureItem(lpMeasureItemStruct);
}


void CXTPPropertyGridView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpDrawItemStruct->itemData;
	CXTPPropertyGridPaintManager* pPaintManager = GetPaintManager();

	if (pItem == NULL)
		return;

	if ((int)lpDrawItemStruct->itemID == GetCount() - 1)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CXTPClientRect rc(this);
		rc.top = lpDrawItemStruct->rcItem.bottom;
		pDC->FillSolidRect(rc, pPaintManager->GetItemMetrics()->m_clrBack);
	}

	if (!((m_pGrid->GetStyle() & XTP_PGS_OWNERDRAW) &&
		(SendNotifyMessage(XTP_PGN_DRAWITEM, (LPARAM)lpDrawItemStruct) == TRUE)))
	{
		pPaintManager->DrawItem(lpDrawItemStruct);
	}
}

BOOL CXTPPropertyGridView::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CXTPPropertyGridView::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rc(this);
	CXTPBufferDC buffer(dc, rc);

	ASSERT(m_pGrid);
	GetPaintManager()->FillPropertyGridView(&buffer);

	CWnd::DefWindowProc(WM_PAINT, (WPARAM)buffer.m_hDC, 0);
}

void CXTPPropertyGridView::OnNcPaint()
{
	Default();

	CWindowDC dc(this);

	CXTPWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());

	ASSERT(m_pGrid);
	GetPaintManager()->DrawPropertyGridBorder(&dc, rc, FALSE);
}

void CXTPPropertyGridView::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	GetPaintManager()->DrawPropertyGridBorder(NULL, lpncsp->rgrc[0], TRUE);

	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

CXTPPropertyGridItem* CXTPPropertyGridView::InsertCategory(int nIndex, LPCTSTR strCaption, CXTPPropertyGridItem* pCategory)
{
	ASSERT(nIndex >= 0 && nIndex <= m_pCategories->GetCount());

	if (nIndex < 0 || nIndex > m_pCategories->GetCount())
		nIndex = m_pCategories->GetCount();

	if (pCategory == NULL)
		pCategory = new CXTPPropertyGridItem(strCaption);

	SetPropertySort(xtpGridSortCategorized);

	pCategory->m_pGrid = this;
	pCategory->m_bCategory = TRUE;
	pCategory->m_nFlags = 0;


	m_pCategories->InsertAt(nIndex, pCategory);

	if (m_hWnd)
	{
		int nInsertAt = (nIndex >= m_pCategories->GetCount() - 1) ? GetCount() : m_pCategories->GetAt(nIndex + 1)->m_nIndex;
		InsertItem(pCategory, nInsertAt);
	}
	_RefreshIndexes();


	return pCategory;

}

CXTPPropertyGridItem* CXTPPropertyGridView::AddCategory(LPCTSTR strCaption, CXTPPropertyGridItem* pCategory)
{
	return InsertCategory(m_pCategories->GetCount(), strCaption, pCategory);
}

void CXTPPropertyGridView::Refresh()
{
	_RefreshIndexes();
	SetPropertySort(m_properetySort, TRUE);

	SAFE_INVALIDATE(m_pGrid);
}

void CXTPPropertyGridView::ResetContent()
{
	if (m_pSelected)
	{
		m_pSelected->OnDeselect();
		m_pSelected = NULL;
		m_pGrid->OnSelectionChanged(NULL);
	}

	if (m_hWnd)
	{
		for (int i = 0; i < GetCount(); i++)
		{
			CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)GetItemDataPtr(i);
			ASSERT(pItem);
			if (!pItem)
				continue;
			pItem->SetVisible(FALSE);
		}

		CListBox::ResetContent();

		CFont* pFont = &GetPaintManager()->GetItemMetrics()->m_fontNormal;

		SetFont(pFont, FALSE);

		CWindowDC dc(this);
		CXTPFontDC font(&dc, pFont);
		m_nItemHeight = dc.GetTextExtent(_T(" "), 1).cy + 4;

		SetItemHeight(0, m_nItemHeight);
	}
}

void CXTPPropertyGridView::SetPropertySort(XTPPropertyGridSortOrder sort, BOOL bRrefresh, BOOL bSetRedraw)
{
	if (m_nLockUpdate > 0)
	{
		m_properetySort = sort;
		return;
	}

	if (sort == m_properetySort && !bRrefresh)
		return;

	if (!m_hWnd)
		return;

	if (bSetRedraw) SetRedraw(FALSE);

	CXTPPropertyGridItem* pTopItem = bRrefresh ? GetItem(GetTopIndex()) : NULL;
	CXTPPropertyGridItem* pSelected = GetSelectedItem();

	ResetContent();

	if (sort == xtpGridSortCategorized)
	{
		for (int i = 0; i < m_pCategories->GetCount(); i++)
		{
			CXTPPropertyGridItem* pCategory = m_pCategories->GetAt(i);
			InsertItem(pCategory, GetCount());
		}
	}
	else if (sort == xtpGridSortAlphabetical || sort == xtpGridSortNoSort)
	{
		CXTPPropertyGridItems lstItems;
		int i;
		for (i = 0; i < m_pCategories->GetCount(); i++)
		{
			CXTPPropertyGridItem* pCategory = m_pCategories->GetAt(i);
			if (!pCategory->IsHidden())
				lstItems.AddTail(pCategory->m_pChilds);
		}

		if (sort != xtpGridSortNoSort)
			lstItems.Sort();

		for (i = 0; i < lstItems.GetCount(); i++)
		{
			CXTPPropertyGridItem* pItem = lstItems.GetAt(i);
			InsertItem(pItem, GetCount());
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	_RefreshIndexes();

	if (pTopItem && pTopItem->IsVisible())
	{
		SetTopIndex(pTopItem->m_nIndex);
	}

	if (pSelected)
	{
		pSelected->Select();
	}
	OnSelectionChanged();

	if (bSetRedraw) SetRedraw(TRUE);

	if (sort != m_properetySort)
	{
		m_properetySort = sort;
		m_pGrid->OnSortChanged();
	}
}

void CXTPPropertyGridView::SetDividerPos(int nDivider)
{
	double dWidth = (double)CXTPWindowRect(this).Width();

	if (!m_bAutoDivider)
	{
		m_dDivider = nDivider;
		m_dDivider = __max(m_dDivider, 1.0);

		if (dWidth != 0)
		{
			m_dDivider = __min(m_dDivider, dWidth);
		}
	}
	else
	{
		m_dDivider = dWidth == 0 ? .5 : (double)nDivider / dWidth;

		m_dDivider = __max(m_dDivider, .1);
		m_dDivider = __min(m_dDivider, .85);
	}
}

int CXTPPropertyGridView::GetDividerPos() const
{
	return int(m_dDivider * (m_bAutoDivider ? CXTPWindowRect(this).Width() : 1));
}

void CXTPPropertyGridView::LockDivider()
{
	m_dDivider = GetDividerPos();
	m_bAutoDivider = FALSE;
}

int  CXTPPropertyGridView::InsertItem(CXTPPropertyGridItem* pItem, int nIndex)
{
	if (m_nLockUpdate > 0) return 0;
	if (!m_hWnd) return 0;

	if (pItem->IsHidden())
		return 0;

	pItem->OnBeforeInsert();
	nIndex = (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LPARAM)pItem);

	ASSERT(nIndex != -1);
	SetItemDataPtr(nIndex, pItem);

	if (GetStyle() & LBS_OWNERDRAWVARIABLE)
	{
		MEASUREITEMSTRUCT measureItemStruct =
		{
			ODT_LISTBOX, 0, nIndex, 0, m_nItemHeight, (ULONG_PTR)pItem
		};
		MeasureItem(&measureItemStruct);
		SetItemHeight(nIndex, measureItemStruct.itemHeight);
	}

	pItem->SetVisible(TRUE);

	int nItemsInserted = 1;

	if (pItem->m_bExpanded)
	{
		nItemsInserted += _DoExpand(pItem, nIndex);
	}

	return nItemsInserted;
}

int CXTPPropertyGridView::_DoExpand(CXTPPropertyGridItem* pItem, int nIndex)
{
	int nStart = nIndex;

	for (int i = 0; i < pItem->GetChilds()->GetCount(); i++)
	{
		CXTPPropertyGridItem* pChild = pItem->GetChilds()->GetAt(i);

		nIndex += InsertItem(pChild, nIndex + 1);

	}
	return nIndex - nStart;
}

void CXTPPropertyGridView::_RefreshIndexes()
{
	if (m_hWnd)
	{
		for (int i = 0; i < GetCount(); i++)
		{
			CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)GetItemDataPtr(i);
			ASSERT(pItem);
			if (!pItem)
				continue;

			if (pItem->m_nIndex != i)
			{
				pItem->m_nIndex = i;
				pItem->OnIndexChanged();
			}
		}
	}
}

void CXTPPropertyGridView::_DoCollapse(CXTPPropertyGridItem* pItem)
{
	ASSERT(pItem);
	if (!pItem)
		return;
	ASSERT(pItem->m_bExpanded);
	int nIndex = pItem->m_nIndex + 1;

	while (nIndex < GetCount())
	{
		CXTPPropertyGridItem* pChild = (CXTPPropertyGridItem*)GetItemDataPtr(nIndex);
		ASSERT(pChild);
		if (!pChild || !pChild->HasParent(pItem))
			break;

		pChild->SetVisible(FALSE);
		DeleteString(nIndex);
	}
	_RefreshIndexes();
}

CXTPPropertyGridItem* CXTPPropertyGridView::GetItem(int nIndex) const
{
	if (nIndex < 0 || nIndex >= GetCount())
		return 0;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)GetItemDataPtr(nIndex);

	if ((ULONG_PTR)pItem == (ULONG_PTR)(-1))
		return NULL;

	ASSERT(pItem);

	return pItem;
}

void CXTPPropertyGridView::SwitchExpandState(int nItem)
{
	CXTPPropertyGridItem* pItem = GetItem(nItem);
	if (!pItem)
		return;

	if (pItem->m_bExpanded)
		pItem->Collapse();
	else
		pItem->Expand();
}

void CXTPPropertyGridView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (HitTest(point) == PGV_HIT_SPLITTER)
	{
		SetFocus();
		SetCapture();
		if (m_pSelected) m_pSelected->OnValidateEdit();

		m_bTracking = TRUE;
		return;
	}


	CXTPPropertyGridItem* pItem = ItemFromPoint(point);
	if (pItem)
	{
		SetFocus();

		if (GetFocus() != this)
			return;

		if ((ItemFromPoint(point) == pItem) && pItem->OnLButtonDown(nFlags, point))
			return;
	}

	CListBox::OnLButtonDown(nFlags, point);
}

CXTPPropertyGridItem* CXTPPropertyGridView::ItemFromPoint(CPoint point) const
{
	BOOL bOutside = FALSE;
	int nIndex = CListBox::ItemFromPoint(point, bOutside);
	if (nIndex != -1 && !bOutside)
	{
		return GetItem(nIndex);
	}
	return NULL;
}

void CXTPPropertyGridView::FocusInplaceButton(CXTPPropertyGridInplaceButton* pButton)
{
	if (pButton != m_pFocusedButton)
	{
		if (pButton) SetFocus();
		m_pFocusedButton = pButton;
		Invalidate(FALSE);
	}
}

void CXTPPropertyGridView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		SetDividerPos(point.x);

		if (m_pSelected) m_pSelected->OnSelect();
		Invalidate(FALSE);
		return;
	}

	CXTPPropertyGridItem* pItem = ItemFromPoint(point);
	CXTPPropertyGridInplaceButton* pButton = NULL;

	if (pItem)
		pButton = pItem->GetInplaceButtons()->HitTest(point);

	if (pButton != m_pHotButton)
	{
		m_pHotButton = pButton;
		Invalidate(FALSE);
		TRACKMOUSEEVENT tme =
		{
			sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0
		};
		_TrackMouseEvent (&tme);
	}

	ShowToolTip(point);

	CListBox::OnMouseMove(nFlags, point);
}

void CXTPPropertyGridView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = FALSE;
	}

	CXTPPropertyGridItem* pItem = ItemFromPoint(point);

	if (pItem)
	{
		pItem->OnLButtonUp(nFlags, point);
	}

	CListBox::OnLButtonUp(nFlags, point);
}

void CXTPPropertyGridView::OnCaptureChanged(CWnd* pWnd)
{
	m_bTracking = FALSE;

	CListBox::OnCaptureChanged(pWnd);
}

void CXTPPropertyGridView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDblClk(nFlags, point);

	CXTPPropertyGridItem* pItem = ItemFromPoint(point);

	if (pItem)
	{
		pItem->OnLButtonDblClk(nFlags, point);


		if (pItem == ItemFromPoint(point))
		{
			SendNotifyMessage(XTP_PGN_DBLCLICK, (LPARAM)pItem);
		}
	}
}

void CXTPPropertyGridView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnRButtonDown(nFlags, point);

	CXTPPropertyGridItem* pItem = ItemFromPoint(point);

	if (pItem)
	{
		pItem->OnRButtonDown(nFlags, point);


		if (pItem == ItemFromPoint(point))
		{
			SendNotifyMessage(XTP_PGN_RCLICK, (LPARAM)pItem);
		}
	}
}

LRESULT CXTPPropertyGridView::SendNotifyMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pGrid);
	if (!m_pGrid)
		return 0;

	return m_pGrid->SendNotifyMessage(wParam, lParam);
}

CXTPPropertyGridItem* CXTPPropertyGridView::GetSelectedItem()
{
	return m_hWnd ? GetItem(GetCurSel()) : 0;
}

void CXTPPropertyGridView::OnChar(UINT nChar, UINT nRepCntr, UINT nFlags)
{
	if (m_bTracking) return;

	if (m_pFocusedButton && nChar != VK_TAB)
		return;

	if (::GetFocus() != m_hWnd)
		return;

	CXTPPropertyGridItem* pItem = GetSelectedItem();

	if (nChar == '+' && pItem && pItem->HasChilds() && !pItem->m_bExpanded)
	{
		pItem->Expand();
		return;
	}
	if (nChar == '-' && pItem && pItem->HasChilds() && pItem->m_bExpanded)
	{
		pItem->Collapse();
		return;
	}
	if (nChar == VK_TAB)
	{
		if (m_pFocusedButton)
		{
			BOOL bForward = GetKeyState(VK_SHIFT) >= 0;
			m_pGrid->OnNavigate(bForward ? xtpGridUIInplaceEdit : xtpGridUIViewNext, bForward, pItem);
		}
		else
		{
			m_pGrid->OnNavigate(xtpGridUIView, GetKeyState(VK_SHIFT) >= 0, pItem);
		}
		return;
	}
	if (pItem && (nChar != VK_RETURN || !pItem->HasChilds()))
	{
		if (pItem->OnChar(nChar))
			return;
	}

	CWnd::OnChar(nChar, nRepCntr, nFlags);
}

UINT CXTPPropertyGridView::OnGetDlgCode()
{
	return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTCHARS;
}

void CXTPPropertyGridView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGridView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGridView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CXTPPropertyGridItem* pItem = GetSelectedItem();

	if (m_pFocusedButton && m_pFocusedButton->GetItem() == pItem)
	{
		m_pFocusedButton->OnKeyDown(nChar);
		return;
	}

	if ((GetKeyState(VK_CONTROL) < 0) && (nChar == VK_RIGHT || nChar == VK_LEFT))
	{
		CXTPDrawHelpers::KeyToLayout(this, nChar);

		SetDividerPos(GetDividerPos() + (nChar == VK_RIGHT ? 3 : -3));

		if (m_pSelected) m_pSelected->OnSelect();
		Invalidate(FALSE);

		return;
	}

	if (nChar == VK_RIGHT)
	{
		if (pItem && pItem->HasChilds() && !pItem->m_bExpanded)
		{
			pItem->Expand();
			return;
		}
	}
	else if (nChar == VK_LEFT)
	{
		if (pItem && pItem->HasChilds() && pItem->m_bExpanded)
		{
			pItem->Collapse();
			return;
		}
	}
	if (nChar == VK_RETURN)
	{
		SwitchExpandState(GetCurSel());
		return ;
	}
	if (nChar == VK_F4 && m_pSelected)
	{
		CXTPPropertyGridInplaceButton* pButton = m_pSelected->GetInplaceButtons()->Find(XTP_ID_PROPERTYGRID_COMBOBUTTON);
		if (pButton)
			m_pSelected->OnInplaceButtonDown(pButton);
	}

	if (m_pSelected && m_pSelected->OnKeyDown(nChar))
	{
		return;
	}


	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPPropertyGridView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	if ((nChar == VK_DOWN || nChar == VK_UP) && m_pSelected)
	{
		CXTPPropertyGridInplaceButton* pButton = m_pSelected->GetInplaceButtons()->Find(XTP_ID_PROPERTYGRID_COMBOBUTTON);
		if (pButton)
			m_pSelected->OnInplaceButtonDown(pButton);
	}

	CListBox::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CXTPPropertyGridView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	static BOOL bRelay = FALSE;
	if (m_wndTip.GetSafeHwnd() && m_wndTip.IsWindowVisible() && !bRelay)
	{
		bRelay = TRUE;
		RelayToolTipEvent(message);
		bRelay = FALSE;
	}

	if (message == WM_MOUSELEAVE && m_pHotButton)
	{
		m_pHotButton = NULL;
		Invalidate(FALSE);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}


void CXTPPropertyGridView::OnSelectionChanged()
{
	CXTPPropertyGridItem* pItem = GetSelectedItem();

	if (m_pSelected) m_pSelected->OnDeselect();
	if (pItem) pItem->OnSelect();

	m_pSelected = pItem;

	if (m_pSelected) m_pGrid->OnSelectionChanged(m_pSelected);
}

int CXTPPropertyGridView::HitTest(CPoint point) const
{
	if (!m_bVariableSplitterPos)
		return -1;

	int nDivider = GetDividerPos();

	if ((point.x > nDivider - 4 && point.x <= nDivider + 2) && (GetCount() > 0))
		return PGV_HIT_SPLITTER;

	return -1;
}

BOOL CXTPPropertyGridView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		GetCursorPos(&point);
		ScreenToClient(&point);

		if (HitTest(point) == PGV_HIT_SPLITTER)
		{
			SetCursor(m_hCursor);
			return TRUE;
		}
	}

	return CListBox::OnSetCursor(pWnd, nHitTest, message);
}

void CXTPPropertyGridView::OnSize(UINT nType, int cx, int cy)
{
	CListBox::OnSize(nType, cx, cy);

	OnSelectionChanged();

	Invalidate(FALSE);
}


// ToolTips routings

void CXTPPropertyGridView::_ShowToolTip(CRect rcBound, CRect rcText, CXTPPropertyGridItem* pItem, BOOL bValuePart)
{
	if (!m_wndTip.GetSafeHwnd())
		return;

	m_wndTip.SetWindowText(m_strTipText);
	m_wndTip.MoveWindow(rcText);
	m_wndTip.Activate(TRUE, pItem, bValuePart);

	m_rcToolTip = rcBound;

	TRACKMOUSEEVENT tme =
	{
		sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0
	};
	_TrackMouseEvent (&tme);
}

CSize CXTPPropertyGridView::_GetTextExtent(const CString& str, CXTPPropertyGridItem* pItem, BOOL bValuePart)
{
	CWindowDC dc(this);
	CXTPFontDC font(&dc, GetPaintManager()->GetItemFont(pItem, bValuePart));
	return dc.GetTextExtent(str);
}

void CXTPPropertyGridView::ShowToolTip(CPoint pt)
{
	if (CXTPMouseMonitor::IsMouseHooked())
		return;

	if (!m_wndTip.GetSafeHwnd())
	{
		m_wndTip.Create(this);
	}

	if (m_pHotButton)
		return;

	CXTPPropertyGridItem* pItem = ItemFromPoint(pt);

	if (!pItem || pItem->IsCategory())
		return;

	int nDividerPos = GetDividerPos();

	CRect rc = pItem->GetItemRect();
	CRect rcCaption(rc.left, rc.top, rc.left + nDividerPos, rc.bottom);

	if (rcCaption.PtInRect(pt))
	{
		if (m_strTipText == pItem->GetCaption())
			return;

		m_strTipText = pItem->GetCaption();

		ClientToScreen(&rcCaption);

		CRect rcCaptionText(rcCaption);
		rcCaptionText.left += pItem->m_nIndent * XTP_PGI_EXPAND_BORDER + 3 - 3 -1;
		rcCaptionText.top--;
		GetPaintManager()->AdjustItemCaptionRect(pItem, rcCaptionText);

		int nTextExtent = _GetTextExtent(m_strTipText, pItem, FALSE).cx;

		if (nTextExtent + 3 > rcCaptionText.Width())
		{
			if (GetExStyle() & WS_EX_LAYOUTRTL)
			{
				rcCaptionText.left = rcCaption.right - nTextExtent - 7 - (pItem->m_nIndent * XTP_PGI_EXPAND_BORDER);

			}

			_ShowToolTip(rcCaption, rcCaptionText, pItem, FALSE);
		}
	}
	else
	{
		if (m_strTipText == pItem->GetViewValue())
			return;

		if (pItem->IsMultiLine())
			return;

		m_strTipText = pItem->GetViewValue();

		if (GetFocus() && GetFocus()->IsKindOf(RUNTIME_CLASS(CXTPPropertyGridInplaceEdit)))
		{
			if (((CXTPPropertyGridInplaceEdit*)(GetFocus()))->GetItem() == pItem)
				return;
		}

		CRect rcValue(rc.left + nDividerPos, rc.top, rc.right, rc.bottom);
		ClientToScreen(&rcValue);

		CRect rcValueText = pItem->GetValueRect();
		rcValueText.InflateRect(1, 2, 0, 1);
		ClientToScreen(&rcValueText);

		int nTextExtent = _GetTextExtent(m_strTipText, pItem, TRUE).cx;

		if (nTextExtent + 3 > rcValueText.Width())
		{
			if (GetExStyle() & WS_EX_LAYOUTRTL)
			{
				rcValueText.left = rcValueText.right - 8 - nTextExtent;

			}
			_ShowToolTip(rcValue, rcValueText, pItem, TRUE);
		}
	}
}


void CXTPPropertyGridView::RelayToolTipEvent(UINT message)
{
	if (m_wndTip.GetSafeHwnd() && m_wndTip.IsWindowVisible())
	{
		CRect rc;
		m_wndTip.GetWindowRect(rc);

		CPoint pt;
		GetCursorPos(&pt);

		if (!m_rcToolTip.PtInRect(pt) || m_pHotButton)
		{
			m_strTipText = "";
			m_wndTip.Activate(FALSE, 0, 0);
		}

		switch (message)
		{
			case WM_MOUSEWHEEL:
				m_strTipText = "";
				m_wndTip.Activate(FALSE, 0, 0);
				break;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_MOUSELEAVE:
				m_wndTip.Activate(FALSE, 0, 0);
				break;
		}
	}

}


void CXTPPropertyGridView::OnSetFocus(CWnd* pOldWnd)
{
	m_pFocusedButton = NULL;

	CListBox::OnSetFocus(pOldWnd);

}
void CXTPPropertyGridView::OnKillFocus (CWnd* pNewWnd)
{
	m_pFocusedButton = NULL;

	CListBox::OnKillFocus(pNewWnd);

}


CXTPPropertyGridPaintManager* CXTPPropertyGridView::GetPaintManager() const
{
	return m_pGrid->GetPaintManager();
}

CXTPImageManager* CXTPPropertyGridView::GetImageManager() const
{
	return m_pGrid->GetImageManager();
}

//////////////////////////////////////////////////////////////////////////
// Accessible

LRESULT CXTPPropertyGridView::OnGetObject(WPARAM wParam, LPARAM lParam)
{
	if (lParam != OBJID_CLIENT)
		return (LRESULT)Default();

	LPUNKNOWN lpUnknown = GetInterface(&IID_IAccessible);
	if (!lpUnknown)
		return E_FAIL;

	return LresultFromObject(IID_IAccessible, wParam, lpUnknown);
}

BEGIN_INTERFACE_MAP(CXTPPropertyGridView, CCmdTarget)
	INTERFACE_PART(CXTPPropertyGridView, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()

CCmdTarget* CXTPPropertyGridView::GetAccessible()
{
	return this;
}

HRESULT CXTPPropertyGridView::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	*ppdispParent = NULL;

	if (GetSafeHwnd())
	{
		return AccessibleObjectFromWindow(GetSafeHwnd(), OBJID_WINDOW, IID_IDispatch, (void**)ppdispParent);
	}
	return E_FAIL;
}

HRESULT CXTPPropertyGridView::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
	{
		return E_INVALIDARG;
	}

	*pChildCount = CListBox::GetCount();
	return S_OK;
}

HRESULT CXTPPropertyGridView::GetAccessibleChild(VARIANT varChild, IDispatch* FAR* ppdispChild)
{
	*ppdispChild = NULL;
	int nChild = GetChildIndex(&varChild);

	if (nChild <= 0)
	{
		return E_INVALIDARG;
	}

	CXTPPropertyGridItem* pItem = GetItem(nChild - 1);
	if (!pItem)
	{
		return E_INVALIDARG;
	}

	*ppdispChild = pItem->GetIDispatch(TRUE);
	return S_OK;
}

HRESULT CXTPPropertyGridView::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF || nChild == -1)
	{
		*pszName = SysAllocString(L"Properties Window");
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPPropertyGridView::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_TABLE;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPPropertyGridView::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	pvarState->vt = VT_I4;
	pvarState->lVal = 0;
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF)
	{
		pvarState->lVal = STATE_SYSTEM_FOCUSABLE;
	}

	return S_OK;
}


HRESULT CXTPPropertyGridView::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	*pxLeft = *pyTop = *pcxWidth = *pcyHeight = 0;

	if (!GetSafeHwnd())
		return S_OK;

	CRect rc;
	GetWindowRect(&rc);

	int nChild = GetChildIndex(&varChild);

	if (nChild != CHILDID_SELF)
	{
		return E_INVALIDARG;
	}

	*pxLeft = rc.left;
	*pyTop = rc.top;
	*pcxWidth = rc.Width();
	*pcyHeight = rc.Height();

	return S_OK;
}

HRESULT CXTPPropertyGridView::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	if (pvarID == NULL)
		return E_INVALIDARG;

	pvarID->vt = VT_EMPTY;

	if (!GetSafeHwnd())
		return S_FALSE;

	if (!CXTPWindowRect(this).PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	pvarID->vt = VT_I4;
	pvarID->lVal = CHILDID_SELF;

	CPoint pt(xLeft, yTop);
	ScreenToClient(&pt);

	CXTPPropertyGridItem* pItem = ItemFromPoint(pt);

	if (pItem)
	{
		pvarID->vt = VT_DISPATCH;
		pvarID->pdispVal = pItem->GetIDispatch(TRUE);
	}

	return S_OK;
}
