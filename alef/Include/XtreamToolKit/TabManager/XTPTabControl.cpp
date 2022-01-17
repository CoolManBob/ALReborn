// XTPTabControl.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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

#include "Common/XTPToolTipContext.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"

#include "XTPTabControl.h"
#include "XTPTabPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
// CTabManagerDropTarget

class CXTPTabControl::CTabControlDropTarget : public COleDropTarget
{
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* /*pDataObject*/, DWORD /*dwKeyState*/, CPoint point)
	{
		CXTPTabControl* pControl = (CXTPTabControl*)pWnd;
		ASSERT_VALID(pControl);

		if (!pControl->GetPaintManager()->m_bSelectOnDragOver)
			return DROPEFFECT_NONE;

		CXTPTabManagerItem* pItem = pControl->HitTest(point);

		if (pItem)
		{
			pControl->SetSelectedItem(pItem);
		}

		return DROPEFFECT_NONE;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CXTPTabControl

CXTPTabControl::CXTPTabControl()
{
	m_pPaintManager = new CXTPTabPaintManager();
	m_pPaintManager->DisableLunaColors(TRUE);
	m_pPaintManager->m_bDrawTextNoPrefix = FALSE;

	m_hwndClient = 0;
	m_bAllowReorder = FALSE;

	m_pDropTarget = new CTabControlDropTarget();
	m_pImageManager = new CXTPImageManager();
	m_pToolTipContext = new CXTPToolTipContext;
}

CXTPTabControl::~CXTPTabControl()
{
	CMDTARGET_RELEASE(m_pPaintManager);
	CMDTARGET_RELEASE(m_pImageManager);
	CMDTARGET_RELEASE(m_pToolTipContext);

	delete m_pDropTarget;
}

BOOL CXTPTabControl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CWnd::Create(AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_pDropTarget->Register(this);

	return TRUE;
}

CXTPTabManagerItem* CXTPTabControl::InsertItem(int nItem, LPCTSTR lpszItem, HWND hwndChild /*= NULL*/, int nImage /*= -1*/)
{
	CXTPTabManagerItem* pItem = AddItem(nItem);
	pItem->SetCaption(lpszItem);
	pItem->SetHandle(hwndChild);
	pItem->SetImageIndex(nImage);

	if (hwndChild)
		::ShowWindow(hwndChild, SW_HIDE);

	if (nItem == 0 && m_pSelected == NULL)
	{
		SetCurSel(0);
	}

	return pItem;
}

void CXTPTabControl::RedrawControl(LPCRECT lpRect, BOOL /*bAnimate*/)
{
	if (GetSafeHwnd()) InvalidateRect(lpRect, FALSE);
}

void CXTPTabControl::Reposition()
{
	if (!GetSafeHwnd())
		return;

	CXTPClientRect rc(this);
	CClientDC dc(this);

	GetPaintManager()->RepositionTabControl(this, &dc, rc);

	if (m_hwndClient && ::IsWindow(m_hwndClient))
	{
		GetPaintManager()->AdjustClientRect(this, rc);
		::MoveWindow(m_hwndClient, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
	}

	Invalidate(FALSE);
}

void CXTPTabControl::OnItemClick(CXTPTabManagerItem* pItem)
{
	CXTPTabManager::OnItemClick(pItem);

	if (m_hwndClient)
	{
		if (::GetFocus() != m_hWnd && !::IsChild(m_hwndClient, ::GetFocus()))
		{
			HWND hWnd = ::GetNextDlgTabItem(m_hwndClient, NULL, FALSE);
			::SetFocus(hWnd ? hWnd : m_hwndClient);
		}
	}

}

void CXTPTabControl::SetSelectedItem(CXTPTabManagerItem* pItem)
{
	if (m_pSelected != pItem)
	{
		if (m_hwndClient)
			::ShowWindow(m_hwndClient, SW_HIDE);

		if (pItem)
		{
			m_hwndClient = pItem->GetHandle();
		}
		else
		{
			m_hwndClient = 0;
		}

		if (m_hwndClient)
		{
			::ShowWindow(m_hwndClient, SW_SHOW);
		}
	}

	CXTPTabManager::SetSelectedItem(pItem);


	NMHDR nmhdr;
	nmhdr.hwndFrom = GetSafeHwnd();
	nmhdr.idFrom = GetDlgCtrlID();
	nmhdr.code = TCN_SELCHANGE;

	CWnd* pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
	{
		pOwner->SendMessage(WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);
	}
}

void CXTPTabControl::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();

	Reposition();
}

void CXTPTabControl::SetImageList(CImageList* pImageList)
{
	if (pImageList)
	{
		m_pImageManager->SetIcons(*pImageList, 0, 0, 0, xtpImageNormal);
	}
	Reposition();
}

void CXTPTabControl::SetImageManager(CXTPImageManager* pImageManager)
{
	if (pImageManager)
	{
		m_pImageManager->InternalRelease();

		m_pImageManager = pImageManager;
	}
	Reposition();
}


BEGIN_MESSAGE_MAP(CXTPTabControl, CWnd)
	//{{AFX_MSG_MAP(CXTPTabControl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPTabControl message handlers

BOOL CXTPTabControl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

BOOL CXTPTabControl::DrawParentBackground(CDC* pDC, CRect rc)
{
	HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC,
		(WPARAM)pDC->GetSafeHdc(), (LPARAM)m_hWnd);
	if (hBrush)
	{
		::FillRect(pDC->GetSafeHdc(), rc, hBrush);
		return TRUE;
	}

	return FALSE;
}

void CXTPTabControl::OnPaint()
{
	CPaintDC dcPaint(this);
	CXTPBufferDC dc(dcPaint);

	GetPaintManager()->DrawTabControl(this, &dc, m_rcControl);
}

LRESULT CXTPTabControl::OnPrintClient(WPARAM wParam, LPARAM /*lParam*/)
{
	CXTPClientRect rc(this);

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) GetPaintManager()->DrawTabControl(this, pDC, rc);

	return 1;
}

void CXTPTabControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	Reposition();
}


void CXTPTabControl::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	if ((GetStyle() & WS_TABSTOP) && m_pSelected && (m_pSelected == HitTest(point)))
	{
		SetFocus();
	}

	PerformClick(m_hWnd, point);
}

void CXTPTabControl::OnNavigateButtonClick(CXTPTabManagerNavigateButton* pButton)
{
	CXTPTabManager::OnNavigateButtonClick(pButton->GetID());

	CXTPTabManagerItem* pItem = pButton->GetItem() ? pButton->GetItem() : m_pSelected;

	if (pItem && (pButton->GetID() == xtpTabNavigateButtonClose) && pItem->IsClosable())
	{
		DeleteItem(pItem->GetIndex());
	}
}


void CXTPTabControl::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	RefreshXtremeColors();
	GetPaintManager()->RefreshMetrics();

	Reposition();

}

INT_PTR CXTPTabControl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	return PerformToolHitTest(m_hWnd, point, pTI);
}

void CXTPTabControl::OnMouseMove(UINT nFlags, CPoint point)
{
	PerformMouseMove(m_hWnd, point);

	CWnd::OnMouseMove(nFlags, point);
}

void CXTPTabControl::OnMouseLeave()
{
	PerformMouseMove(m_hWnd, CPoint(-1, -1));
}

BOOL CXTPTabControl::DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const
{
	if (!pItem)
		return GetPaintManager()->m_bShowIcons;

	if (pItem->GetImageIndex() == -1 || GetPaintManager()->m_bShowIcons == FALSE)
		return FALSE;

	CXTPImageManagerIcon* pImage = m_pImageManager->GetImage(pItem->GetImageIndex(), szIcon.cx);

	if (!pImage)
		return FALSE;

	if (!bDraw)
	{
		return TRUE;
	}

	pItem->DrawImage(pDC, CRect(pt, szIcon), pImage);

	return TRUE;
}

void CXTPTabControl::SetPaintManager(CXTPTabPaintManager* pPaintManager)
{
	delete m_pPaintManager;
	m_pPaintManager = pPaintManager;
	m_pPaintManager->RefreshMetrics();
	Reposition();
}

BOOL CXTPTabControl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	m_pToolTipContext->FilterToolTipMessage(this, message, wParam, lParam);

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

void CXTPTabControl::EnableToolTips(XTPTabToolTipBehaviour behaviour /*= xtpTabToolTipAlways*/)
{
	m_pPaintManager->EnableToolTips(behaviour);
}


void CXTPTabControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PerformKeyDown(m_hWnd, nChar);

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CXTPTabControl::OnGetDlgCode()
{
	return ::GetFocus() == m_hWnd ? DLGC_WANTARROWS : CWnd::OnGetDlgCode();
}

void CXTPTabControl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	InvalidateRect(m_rcHeaderRect, FALSE);
}

void CXTPTabControl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	InvalidateRect(m_rcHeaderRect, FALSE);
}

#ifndef WM_QUERYUISTATE
#define WM_QUERYUISTATE                 0x0129
#endif

#ifndef WM_CHANGEUISTATE
#define WM_CHANGEUISTATE                0x0127
#endif

#ifndef UISF_HIDEFOCUS
#define UISF_HIDEFOCUS                  0x1
#endif

#ifndef UIS_CLEAR
#define UIS_CLEAR                       2
#endif

void CXTPTabControl::SetFocusedItem(CXTPTabManagerItem* pItem)
{
	CXTPTabManager::SetFocusedItem(pItem);

	if ((::SendMessage(::GetParent(m_hWnd), WM_QUERYUISTATE, 0, 0) & UISF_HIDEFOCUS) != 0)
	{
		::SendMessage(::GetParent(m_hWnd), WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);

	}
}

BOOL CXTPTabControl::HeaderHasFocus() const
{
	return (GetStyle() & WS_TABSTOP) && (::GetFocus() == m_hWnd)
		&& ((::SendMessage(::GetParent(m_hWnd), WM_QUERYUISTATE, 0, 0) & UISF_HIDEFOCUS) == 0);
}

void CXTPTabControl::AdjustRect(BOOL bLarger, LPRECT lpRect)
{
	if (!lpRect)
		return;

	CRect rc(lpRect);
	GetPaintManager()->AdjustClientRect(this, rc);

	if (bLarger)
	{
		lpRect->top -= rc.top - lpRect->top;
		lpRect->left -= rc.left - lpRect->left;
		lpRect->bottom -= rc.bottom - lpRect->bottom;
		lpRect->right -= rc.right - lpRect->right;
	}
	else
	{
		*lpRect = rc;
	}
}
