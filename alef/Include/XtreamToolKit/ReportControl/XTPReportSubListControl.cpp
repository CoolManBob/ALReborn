// XTPReportSubListControl.cpp : implementation of the CXTPReportSubListControl class.
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

#include "XTPReportControl.h"
#include "XTPReportColumn.h"
#include "XTPReportColumns.h"
#include "XTPReportDefines.h"
#include "XTPReportDragDrop.h"
#include "XTPReportSubListControl.h"
#include "XTPReportHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPReportSubListControl, CListBox)
/////////////////////////////////////////////////////////////////////////////
// CXTPReportSubListControl

CXTPReportSubListControl::CXTPReportSubListControl()
{
	m_pReportCtrl = NULL;
	m_pDragWnd = NULL;
	m_bSortAscending = FALSE;
	m_nDropIndex = 0;
}

CXTPReportSubListControl::~CXTPReportSubListControl()
{
}


BEGIN_MESSAGE_MAP(CXTPReportSubListControl, CListBox)
	//{{AFX_MSG_MAP(CXTPReportSubListControl)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPReportSubListControl attributes

BOOL CXTPReportSubListControl::SetReportCtrl(CXTPReportControl* pReportCtrl)
{
	if (pReportCtrl == NULL)
		return FALSE;

	ASSERT_KINDOF(CXTPReportControl, pReportCtrl);

	if (m_pReportCtrl != pReportCtrl)
	{
		ResetContent();
		m_pReportCtrl = pReportCtrl;
	}

	UpdateList();

	return TRUE;
}

CXTPReportControl* CXTPReportSubListControl::GetReportCtrl()
{
	return m_pReportCtrl;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPReportSubListControl operations

void CXTPReportSubListControl::SortAscending(BOOL bSort)
{
	m_bSortAscending = bSort;
	if (GetSafeHwnd()) ResetContent();
	UpdateList();
}

BOOL CXTPReportSubListControl::UpdateList()
{
	if (m_pReportCtrl == NULL)
		return FALSE;

	if (GetSafeHwnd() == NULL)
		return FALSE;

	CClientDC dc(this);
	CXTPFontDC fnt(&dc, &m_pReportCtrl->GetPaintManager()->m_fontCaption);
	int nHeight = dc.GetTextExtent(_T(" "), 1).cy + 5;

	if (GetItemHeight(0) != nHeight) SetItemHeight(0, nHeight);


	CXTPReportColumns* pColumns = m_pReportCtrl->GetColumns();
	int nColumnCount = pColumns->GetCount();

	BOOL bChanged = FALSE;
	// delete all visible items from the list
	for (int i = GetCount() - 1; i >= 0; i--)
	{
		int nItemIndex = (int)GetItemData(i);
		CXTPReportColumn* pColumn = pColumns->Find(nItemIndex);
		if (!pColumn || pColumn->IsVisible() || !pColumn->IsShowInFieldChooser())
		{
			DeleteString(i);
			bChanged = TRUE;
		}
	}

	// add the rest of invisible items
	for (int nColumn = 0; nColumn < nColumnCount; nColumn++)
	{
		CXTPReportColumn* pColumn = pColumns->GetAt(nColumn);
		if (pColumn && !pColumn->IsVisible() && pColumn->IsShowInFieldChooser())
		{
			int nListItem = FindItemData(pColumn->GetItemIndex());
			if (nListItem < 0)
			{
				int nItem = -1;
				if (m_bSortAscending)
				{
					for (nItem = 0; nItem < GetCount(); nItem++)
					{
						CString str = GetItemCaption(nItem);

						if (str > pColumn->GetCaption())
							break;
					}
					nItem = InsertString(nItem, pColumn->GetCaption());
				}
				else
				{
					nItem = AddString(pColumn->GetCaption());
				}

				if (nItem >= 0)
				{
					SetItemData(nItem, pColumn->GetItemIndex());
					bChanged = TRUE;
				}
			}
		}
	}
	EnableWindow(GetCount() > 0);

	if (bChanged)
		Invalidate();

	return TRUE;
}

CXTPReportColumn* CXTPReportSubListControl::GetItemColumn(int nIndex)
{
	if (!m_pReportCtrl || GetSafeHwnd() == 0)
		return NULL;

	int nItemData = (int)GetItemData(nIndex);
	if (nItemData == -1)
		return NULL;

	CXTPReportColumn* pColumn = m_pReportCtrl->GetColumns()->Find(nItemData);

	return pColumn;
}

CString CXTPReportSubListControl::GetItemCaption(int nIndex)
{
	if (!m_pReportCtrl || GetSafeHwnd() == 0)
		return _T("");

	if (GetStyle() & LBS_HASSTRINGS)
	{
		CString str;
		GetText(nIndex, str);
		return str;
	}

	int nItemData = (int)GetItemData(nIndex);
	if (nItemData == -1)
		return _T("");

	CXTPReportColumn* pColumn = m_pReportCtrl->GetColumns()->Find(nItemData);

	if (pColumn == NULL)
		return _T("");

	return pColumn->GetCaption();

}


int CXTPReportSubListControl::FindItemData(int nItemData)
{
	for (int i = GetCount() - 1; i >= 0; i--)
	{
		int nItemIndex = (int)GetItemData(i);
		if (nItemData == nItemIndex)
			return i;
	}
	return -1;
}

BOOL CXTPReportSubListControl::Include(int /*iSubItem*/)
{
	return TRUE;
}

BOOL CXTPReportSubListControl::Disable(int /*iSubItem*/)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPReportSubListControl message handlers

void CXTPReportSubListControl::PreSubclassWindow()
{
	CListBox::PreSubclassWindow();
}

void CXTPReportSubListControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rcItem(lpDrawItemStruct->rcItem);

	CXTPReportPaintManager* pPaintManager = m_pReportCtrl ? m_pReportCtrl->GetPaintManager() : NULL;
	COLORREF clrBack = pPaintManager ? pPaintManager->m_clrControlBack : ::GetXtremeColor(COLOR_WINDOW);


	if (GetCount() > 0 && m_pReportCtrl && pPaintManager)
	{
		BOOL bDisable = Disable((int)lpDrawItemStruct->itemData);

		CXTPFontDC fnt(pDC, &pPaintManager->m_fontCaption);

		if (GetExStyle() & WS_EX_STATICEDGE)
		{
			pDC->Draw3dRect(rcItem, pPaintManager->m_clrHighlightText, pPaintManager->m_clrControlDark);
			rcItem.DeflateRect(1, 1);
			pDC->FillSolidRect(rcItem, pPaintManager->m_clrControlBack);
			rcItem.DeflateRect(1, 1);
		}
		else
		{
			pDC->FillSolidRect(rcItem, pPaintManager->m_clrHeaderControl);
			pDC->Draw3dRect(rcItem, pPaintManager->m_clrBtnFace, GetXtremeColor(COLOR_3DDKSHADOW));
			rcItem.DeflateRect(1, 1);
			pDC->Draw3dRect(rcItem, pPaintManager->m_clrControlBack, pPaintManager->m_clrControlDark);
			rcItem.DeflateRect(1, 1);

		}

		pDC->SetBkMode(TRANSPARENT);

		if (lpDrawItemStruct->itemState&ODS_SELECTED)
		{
			pDC->FillSolidRect(rcItem, GetXtremeColor(COLOR_3DFACE));
			pDC->InvertRect(rcItem);
			pDC->SetTextColor(bDisable ? ::GetXtremeColor(COLOR_BTNFACE) : ::GetXtremeColor(COLOR_3DHIGHLIGHT));
		}
		else
		{
			if (bDisable)
			{
				pDC->SetTextColor(::GetXtremeColor(COLOR_3DHIGHLIGHT));

				CRect rect = rcItem;
				rect.OffsetRect(1, 1);

				pDC->DrawText(GetItemCaption((int)lpDrawItemStruct->itemID), rect,
					DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS | DT_LEFT);

				pDC->SetTextColor(::GetXtremeColor(COLOR_3DSHADOW));
			}
			else
				pDC->SetTextColor(pPaintManager->m_clrCaptionText);
		}

		pDC->DrawText(GetItemCaption((int)lpDrawItemStruct->itemID), rcItem,
			DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS | DT_LEFT);

	}
	else
		pDC->FillSolidRect(rcItem, clrBack);
}

BOOL CXTPReportSubListControl::BeginDrag(int nItem)
{
	if (GetCount() <= 0)
		return FALSE;

	int nSubItem = (int)GetItemData(nItem);

	if (Disable(nSubItem))
	{
		return FALSE;
	}

	GetClientRect(m_rcDragWnd);
	m_rcDragWnd.bottom = m_rcDragWnd.top + GetItemHeight(0);

	CXTPReportHeader* pHeader = m_pReportCtrl->GetReportHeader();
	CXTPReportColumn* pColumn = m_pReportCtrl->GetColumns()->Find(nSubItem);
	pHeader->StartDragging(pColumn);
	pHeader->m_bDragGroupBox = FALSE;

	//pHeader->m_nDragIndex = m_nSubItem;
	m_pDragWnd = new CXTPReportHeaderDragWnd;
	if (m_pDragWnd)
		m_pDragWnd->Create(m_rcDragWnd, pHeader, m_pReportCtrl->GetPaintManager(), pColumn);
	m_pReportCtrl->SetMouseMode(xtpReportMouseDraggingColumn);

	GetWindowRect(m_rcDropTarget1);
	m_rcDropTarget2 = m_pReportCtrl->m_rcHeaderArea;

	if (pColumn->IsGroupable())
		m_rcDropTarget2.UnionRect(&m_rcDropTarget2, &m_pReportCtrl->m_rcGroupByArea);

	m_pReportCtrl->ClientToScreen(m_rcDropTarget2);

	m_nDropIndex = -1;

	return TRUE;
}

UINT CXTPReportSubListControl::Dragging(CPoint pt)
{
	CPoint point = pt;
	point.Offset(-(m_rcDragWnd.Width() >> 1), -(m_rcDragWnd.Height() >> 1));

	if (m_pDragWnd != NULL)
	{
		m_pDragWnd->SetWindowPos(&wndTop,
			point.x, point.y, 0, 0,
			SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}

	if (m_pReportCtrl)
	{
		CPoint pointCtrl(pt);
		m_pReportCtrl->ScreenToClient(&pointCtrl);
		m_pReportCtrl->OnMouseMove(0, pointCtrl);
	}


	if (m_rcDropTarget1.PtInRect(pt))
		return DL_MOVECURSOR;

	if (m_rcDropTarget2.PtInRect(pt))
		return DL_MOVECURSOR;

	return DL_STOPCURSOR;
}

void CXTPReportSubListControl::CancelDrag()
{
	if (m_pDragWnd != NULL)
	{
		m_pDragWnd->DestroyWindow();
		m_pDragWnd = NULL;
	}

	CXTPReportHeader* pHeader = m_pReportCtrl->GetReportHeader();
	pHeader->SetHotDivider(-1);

	m_pReportCtrl->SetMouseMode(xtpReportMouseNothing);
}

void CXTPReportSubListControl::Dropped(CPoint pt)
{
	if (m_pDragWnd != NULL)
	{
		m_pDragWnd->DestroyWindow();
		m_pDragWnd = NULL;
	}

	if (m_pReportCtrl)
	{
		m_pReportCtrl->ScreenToClient(&pt);
		m_pReportCtrl->OnLButtonUp(0, pt);
	}
}

void CXTPReportSubListControl::OnPaint()
{
	CPaintDC dcPaint(this);
	CRect rc;
	GetClientRect(rc);

	CXTPBufferDC dc(dcPaint, rc);

	CXTPReportPaintManager* pPaintManager = m_pReportCtrl ? m_pReportCtrl->GetPaintManager() : NULL;
	COLORREF clrBack = pPaintManager ? pPaintManager->m_clrControlBack : GetXtremeColor(COLOR_WINDOW);

	dc.FillSolidRect(rc, IsWindowEnabled() ? clrBack : GetXtremeColor(COLOR_BTNFACE));

	if (GetCount() > 0 || !pPaintManager)
		CWnd::DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);
	else
	{
		dc.SetTextColor(GetXtremeColor(COLOR_3DSHADOW));
		dc.SetBkMode(TRANSPARENT);
		CXTPFontDC fnt(&dc, &pPaintManager->m_fontCaption);

		dc.DrawText(pPaintManager->m_strNoFieldsAvailable, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
	}
}

LRESULT CXTPReportSubListControl::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	CListBox::DefWindowProc(WM_ERASEBKGND, wParam, 0);
	return CListBox::DefWindowProc(WM_PRINTCLIENT, wParam, lParam);
}

BOOL CXTPReportSubListControl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}


void CXTPReportSubListControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	BOOL bOutside = TRUE;
	int nItem = ItemFromPoint(point, bOutside);
	if (nItem == LB_ERR)
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	SetCurSel(nItem);
	ParentNotify_SelChangd();

	BOOL bAccept = FALSE;
	BOOL bStartDrag = FALSE;

	HCURSOR hCursorNo = AfxGetApp()->LoadStandardCursor(IDC_NO), hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	SetCapture();

	while (CWnd::GetCapture() == this)
	{
		MSG msg;
		if (!GetMessage(&msg, NULL, 0, 0))
			break;

		if (msg.message == WM_MOUSEMOVE)
		{
			CPoint pt(msg.lParam);

			if (point != pt)
			{
				if (!bStartDrag && abs(point.x - pt.x) + abs(point.y - pt.y) < 3)
					continue;

				point = pt;

				if (!bStartDrag)
				{
					BeginDrag(nItem);
					bStartDrag = TRUE;
				}

				if (bStartDrag)
				{
					ClientToScreen(&pt);
					UINT nCursor = Dragging(pt);
					::SetCursor(nCursor == DL_STOPCURSOR ? hCursorNo : hCursorArrow);
				}
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

	if (GetCapture() == this)
	{
		ReleaseCapture();
	}

	if (bStartDrag)
	{
		if (bAccept)
		{
			ClientToScreen(&point);
			Dropped(point);

			ParentNotify_SelChangd();
		}
		else
		{
			CancelDrag();
		}
	}
}

void CXTPReportSubListControl::ParentNotify_SelChangd()
{
	if (GetParent() && (GetStyle() & LBS_NOTIFY))
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd);

}
