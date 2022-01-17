// XTPDockingPaneTabbedContainer.cpp : implementation of the CXTPDockingPaneTabbedContainer class.
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

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPToolTipContext.h"
#include "TabManager/XTPTabManager.h"

#include "XTPDockingPaneTabbedContainer.h"
#include "XTPDockingPane.h"
#include "XTPDockingPaneContext.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPanePaintManager.h"
#include "XTPDockingPaneLayout.h"
#include "XTPDockingPaneAutoHidePanel.h"
#include "XTPDockingPaneMiniWnd.h"
#include "XTPDockingPaneSplitterContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
// CTabManagerDropTarget

class CXTPDockingPaneTabbedContainer::CContainerDropTarget : public COleDropTarget
{
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* /*pDataObject*/, DWORD /*dwKeyState*/, CPoint point)
	{
		CXTPDockingPaneTabbedContainer* pControl = (CXTPDockingPaneTabbedContainer*)pWnd;
		ASSERT_VALID(pControl);

		if (!pControl->GetPaintManager()->m_bSelectOnDragOver)
			return DROPEFFECT_NONE;

		CXTPTabManagerItem* pItem = pControl->CXTPTabManager::HitTest(point);

		if (pItem)
		{
			CXTPDockingPane* pPane = pControl->GetItemPane(pItem->GetIndex());
			if (pControl->GetSelected() != pPane)
			{
				pControl->SelectPane(pPane, FALSE, FALSE);
			}
		}

		return DROPEFFECT_NONE;
	}
};

// CXTPDockingPaneTabbedContainer

void CXTPDockingPaneTabbedContainer::RedrawControl(LPCRECT lpRect, BOOL /*bAnimate*/)
{
	if (!m_bLockReposition)
	{
		if (GetSafeHwnd()) InvalidateRect(lpRect, FALSE);
	}
}
void CXTPDockingPaneTabbedContainer::Reposition()
{
	if (!m_bLockReposition)
	{
		GetDockingPaneManager()->RedrawPanes();
	}

}
CXTPTabPaintManager* CXTPDockingPaneTabbedContainer::GetPaintManager() const
{
	return CXTPDockingPaneBase::GetPaintManager()->GetTabPaintManager();
}

void CXTPDockingPaneTabbedContainer::SetPaintManager(CXTPTabPaintManager* /*pPaintManager*/)
{

}

BOOL CXTPDockingPaneTabbedContainer::DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const
{
	if (!pItem)
		return TRUE;

	if (!CXTPDockingPaneBase::GetPaintManager()->GetTabPaintManager()->m_bShowIcons)
		return FALSE;

	CXTPImageManagerIcon* pImage = ((CXTPDockingPane*)pItem->GetData())->GetIcon(szIcon.cx);

	if (!pImage)
		return FALSE;

	if (!bDraw)
	{
		return TRUE;
	}

	pItem->DrawImage(pDC, CRect(pt, szIcon), pImage);

	return TRUE;
}



IMPLEMENT_DYNAMIC(CXTPDockingPaneTabbedContainer, CWnd)

CXTPDockingPaneTabbedContainer::CXTPDockingPaneTabbedContainer(CXTPDockingPaneLayout* pLayout)
	: CXTPDockingPaneBaseContainer(xtpPaneTypeTabbedContainer, pLayout)
{

	m_pSelectedPane = 0;
	m_pTrackingPane = 0;

	m_bLockReposition = FALSE;

	m_bActive = FALSE;
	m_bTitleVisible = TRUE;
	m_bDelayRedraw = FALSE;

	m_pCaptionButtons = new CXTPDockingPaneCaptionButtons();
	m_pCaptionButtons->Add(new CXTPDockingPaneCaptionButton(XTP_IDS_DOCKINGPANE_CLOSE, this));
	m_pCaptionButtons->Add(new CXTPDockingPaneCaptionButton(XTP_IDS_DOCKINGPANE_MAXIMIZE, this));
	m_pCaptionButtons->Add(new CXTPDockingPaneCaptionButton(XTP_IDS_DOCKINGPANE_RESTORE, this));
	m_pCaptionButtons->Add(new CXTPDockingPaneCaptionButton(XTP_IDS_DOCKINGPANE_AUTOHIDE, this));
	m_pCaptionButtons->Add(new CXTPDockingPaneCaptionButton(XTP_IDS_DOCKINGPANE_MENU, this));

	m_pDropTarget = new CContainerDropTarget();
	m_bMaximized = FALSE;

	EnableAutomation();

}

CXTPDockingPaneCaptionButton* CXTPDockingPaneTabbedContainer::GetCloseButton() const
{
	return FindCaptionButton(XTP_IDS_DOCKINGPANE_CLOSE);
}
CXTPDockingPaneCaptionButton* CXTPDockingPaneTabbedContainer::GetPinButton() const
{
	return FindCaptionButton(XTP_IDS_DOCKINGPANE_AUTOHIDE);
}


void CXTPDockingPaneTabbedContainer::Init(CXTPDockingPane* pPane, CWnd* pFrame)
{
	ASSERT(pPane);
	if (!pPane)
		return;

	m_pDockingSite = pFrame;
	m_szDocking = pPane->m_szDocking;
	m_rcWindow = pPane->m_rcWindow;

	_InsertPane(pPane);
	SelectPane(pPane);
}

CXTPDockingPaneTabbedContainer::~CXTPDockingPaneTabbedContainer()
{
	if (m_hWnd) DestroyWindow();

	delete m_pDropTarget;
}

void CXTPDockingPaneTabbedContainer::SetDockingSite(CWnd* pFrame)
{
	m_pDockingSite = pFrame;

	if (m_hWnd)
	{
		if (GetParent() != pFrame) CWnd::SetParent(pFrame);
	}

	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)GetNext(pos);
		pPane->SetDockingSite(pFrame);
	}

}

BOOL CXTPDockingPaneTabbedContainer::IsTabsVisible() const
{
	return m_hWnd && !DYNAMIC_DOWNCAST(CXTPDockingPaneAutoHideWnd, GetParent())
		&& (GetItemCount() > 1 || CXTPDockingPaneBase::GetPaintManager()->m_bDrawSingleTab);
}

void CXTPDockingPaneTabbedContainer::InvalidatePane(BOOL bSelectionChanged)
{
	if (!GetSafeHwnd())
		return;

	if (m_pParentContainer == 0)
		return;

	if (m_bLockReposition)
		return;

	m_bLockReposition += 1;
	OnTabsChanged();
	m_bLockReposition -= 1;

	CRect rect = m_rcWindow;
	CXTPDockingPaneBase::GetPaintManager()->AdjustClientRect(this, rect, TRUE);

	if (bSelectionChanged)
	{
		POSITION pos = GetHeadPosition();
		while (pos)
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)GetNext(pos);
			pPane->ShowWindow(m_pSelectedPane == pPane);
			pPane->OnSizeParent(m_pDockingSite, rect, 0);
		}
	}
	Invalidate(FALSE);

	m_pParentContainer->InvalidatePane(bSelectionChanged);
}

void CXTPDockingPaneTabbedContainer::OnSizeParent(CWnd* pParent, CRect rect, LPVOID lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

	ASSERT(!IsEmpty());

	SetDockingSite(pParent);
	m_rcWindow = rect;

	if (lpLayout == 0 || lpLayout->hDWP != NULL)
	{
		CRect rectOld;
		::GetWindowRect(m_hWnd, rectOld);
		HWND hWndParent = ::GetParent(m_hWnd);
		::ScreenToClient(hWndParent, &rectOld.TopLeft());
		::ScreenToClient(hWndParent, &rectOld.BottomRight());
		if (rectOld != rect || m_bDelayRedraw)
		{
			SetWindowPos(&CWnd::wndBottom, rect.left, rect.top, rect.Width(), rect.Height(), 0);
			Invalidate(FALSE);
			m_bDelayRedraw = FALSE;
		}

		m_bLockReposition += 1;
		OnTabsChanged();
		m_bLockReposition -= 1;

		GetPinButton()->SetState(IsHidden() ? xtpPanePinVisible | xtpPanePinPushed: DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, pParent) == 0 ? xtpPanePinVisible : 0);

		CXTPDockingPaneBase::GetPaintManager()->AdjustClientRect(this, rect, TRUE);

		POSITION pos = GetHeadPosition();
		while (pos)
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)GetNext(pos);
			pPane->ShowWindow(m_pSelectedPane == pPane);
			pPane->OnSizeParent(pParent, rect, lParam);
		}

	}
}



void CXTPDockingPaneTabbedContainer::OnTabsChanged()
{
	if (!m_hWnd)
		return;

	m_bLockReposition += 1;

	DeleteAllItems();

	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)GetNext(pos);

		CXTPTabManagerItem* pItem = AddItem(GetItemCount());
		if (m_pSelectedPane == pPane) SetSelectedItem(pItem);

		pItem->SetCaption(pPane->GetShortTitle());
		pItem->SetColor(pPane->GetItemColor());
		pItem->SetTooltip(pPane->GetTitle());
		pItem->SetEnabled(pPane->GetEnabled() & xtpPaneEnableClient);

		pItem->SetData((DWORD_PTR)pPane);
	}
	//////////////////////////////////////////////////////////////////////////

	m_pCaptionButtons->CheckForMouseOver(CPoint(-1, -1));

	m_bLockReposition -= 1;
}

void CXTPDockingPaneTabbedContainer::_InsertPane(CXTPDockingPane* pPane, BOOL bSetFocus)
{
	ASSERT(m_pLayout);
	if (!m_pLayout)
		return;

	if (!m_hWnd && !m_pLayout->IsUserLayout())
	{
		Create(_T("XTPDockingPaneTabbedContainer"), _T(""), WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), GetDockingSite(), 0);
		m_pDropTarget->Register(this);
	}

	m_lstPanes.AddTail(pPane);

	pPane->SetParentContainer(this);
	pPane->SetDockingSite(GetDockingSite());

	m_bDelayRedraw = TRUE;
	GetDockingPaneManager()->RecalcFrameLayout(this, TRUE);

	SelectPane(pPane, bSetFocus);

	SAFE_CALLPTR(m_pParentContainer, OnChildContainerChanged(this));

}

void CXTPDockingPaneTabbedContainer::RemovePane(CXTPDockingPaneBase* pPane)
{
	ASSERT(pPane->GetContainer() == this);

	POSITION pos = m_lstPanes.Find((CXTPDockingPane*)pPane);
	ASSERT(pos);

	m_lstPanes.RemoveAt(pos);
	OnTabsChanged();

	pPane->SetParentContainer(NULL);

	m_pParentContainer->OnChildContainerChanged(this);

	if (m_lstPanes.IsEmpty())
	{
		if (m_bActive)
		{
			m_bActive = FALSE;
			GetDockingPaneManager()->OnActivatePane(FALSE, m_pSelectedPane);
		}
		DestroyWindow();
	}

	m_bDelayRedraw = TRUE;
	GetDockingPaneManager()->RecalcFrameLayout(this, TRUE);

	if (pPane == m_pSelectedPane)
	{
		SelectPane((CXTPDockingPane*)GetLastPane());
	}
}


BEGIN_MESSAGE_MAP(CXTPDockingPaneTabbedContainer, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_GETOBJECT, OnGetObject)
END_MESSAGE_MAP()



// CXTPDockingPaneTabbedContainer message handlers

void CXTPDockingPaneTabbedContainer::OnDestroy()
{
	CWnd::OnDestroy();
}

void CXTPDockingPaneTabbedContainer::DeletePane()
{
	InternalRelease();
}

void CXTPDockingPaneTabbedContainer::OnFinalRelease()
{
	if (m_hWnd != NULL)
		DestroyWindow();

	CCmdTarget::OnFinalRelease();
}

BOOL CXTPDockingPaneTabbedContainer::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (GetDockingPaneManager()->m_bShowSizeCursorWhileDragging)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);

		if (CXTPDockingPaneBase::GetPaintManager()->GetCaptionGripperRect(this).PtInRect(pt))
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
			return TRUE;
		}

	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CXTPDockingPaneTabbedContainer::OnPaint()
{
	CPaintDC dcPaint(this);
	CXTPClientRect rc(this);
	CXTPBufferDC dc(dcPaint);

	CXTPDockingPaneBase::GetPaintManager()->DrawPane(dc, this, rc);
}


void CXTPDockingPaneTabbedContainer::ShowTitle(BOOL bShow)
{
	m_bTitleVisible = bShow;

	m_bDelayRedraw = TRUE;
	GetDockingPaneManager()->RecalcFrameLayout(this, TRUE);
}

BOOL CXTPDockingPaneTabbedContainer::IsTitleVisible() const
{
	return m_bTitleVisible &&
		(m_pSelectedPane && ((m_pSelectedPane->GetOptions() & xtpPaneNoCaption) == 0)) &&
		CXTPDockingPaneBase::GetPaintManager()->m_bShowCaption;
}


CString CXTPDockingPaneTabbedContainer::GetTitle() const
{
	return m_pSelectedPane ? m_pSelectedPane->GetTitle() : _T("");
}

void CXTPDockingPaneTabbedContainer::OnFocusChanged()
{
	if (!GetSafeHwnd() || IsEmpty())
		return;

	CWnd* pFocus = GetFocus();
	BOOL bActive = (pFocus->GetSafeHwnd() && (pFocus == this || IsChild(pFocus) ||
		(pFocus->GetOwner()->GetSafeHwnd() && IsChild(pFocus->GetOwner()))));

	if (bActive != m_bActive)
	{
		m_bActive = bActive;

		GetDockingPaneManager()->OnActivatePane(bActive, m_pSelectedPane);

		Invalidate(FALSE);
	}
	else if (m_bActive && GetDockingPaneManager()->GetActivePane() != m_pSelectedPane)
	{
		GetDockingPaneManager()->OnActivatePane(bActive, m_pSelectedPane);
	}
}

#define DOCKINGPANE_HITCAPTION -2

int CXTPDockingPaneTabbedContainer::HitTest(CPoint point) const
{
	CXTPClientRect rc(this);

	if (IsTitleVisible())
	{
		if (rc.PtInRect(point))
		{
			CXTPDockingPaneBase::GetPaintManager()->AdjustCaptionRect(this, rc);
			if (!rc.PtInRect(point))
			{
				return DOCKINGPANE_HITCAPTION;
			}
		}
	}

	if (IsTabsVisible())
	{
		CXTPTabManagerItem* pItem = CXTPTabManager::HitTest(point);
		return pItem ? pItem->GetIndex() : -1;
	}
	return -1;
}

CXTPTabManagerItem* CXTPDockingPaneTabbedContainer::GetPaneTab(CXTPDockingPane* pPane) const
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetItemPane(i) == pPane)
			return GetItem(i);
	}
	return NULL;
}

void CXTPDockingPaneTabbedContainer::SelectPane(CXTPDockingPane* pPane, BOOL bSetFocus, BOOL bDelayRedraw)
{
	if (m_pSelectedPane != pPane)
	{
		m_pSelectedPane = pPane;

		if (bDelayRedraw)
		{
			m_bDelayRedraw = TRUE;
			GetDockingPaneManager()->RecalcFrameLayout(this, TRUE);
			SAFE_CALLPTR(m_pParentContainer, OnChildContainerChanged(this));
		}
		else
		{
			InvalidatePane(TRUE);
		}

		CXTPTabManager::EnsureVisible(GetPaneTab(m_pSelectedPane));
	}

	if (bSetFocus)
	{
		if (m_bActive) GetDockingPaneManager()->OnActivatePane(TRUE, m_pSelectedPane);
		SAFE_CALLPTR(pPane, SetFocus());
	}

}

void CXTPDockingPaneTabbedContainer::OnRButtonDown(UINT /*nFlags*/, CPoint point)
{
	int nHit = HitTest(point);
	if (nHit >= 0)
	{
		SelectPane(GetItemPane(nHit));
		GetDockingPaneManager()->RecalcFrameLayout(this);
	}

	if (m_pSelectedPane)
		m_pSelectedPane->SetFocus();
	else
		SetFocus();

	GetDockingPaneManager()->UpdatePanes();

	GetDockingPaneManager()->NotifyOwner(XTP_DPN_RCLICK, (LPARAM)(CXTPDockingPaneBase*)this);
}

void CXTPDockingPaneTabbedContainer::OnCaptionLButtonDown(CPoint point)
{
	_RestoreFocus();

	if (GetKeyState(VK_LBUTTON) < 0)
	{
		if (GetDockingPaneManager()->_OnAction(xtpPaneActionDragging, m_pSelectedPane))
			return;

		CXTPDockingPaneContext* pContext = GetDockingPaneManager()->GetDockingContext();
		pContext->Drag(this, point);
	}
}


void CXTPDockingPaneTabbedContainer::NormalizeDockingSize()
{
	XTPDockingPaneDirection direction = GetDockingPaneManager()->GetPaneDirection(this);
	BOOL bHoriz = (direction == xtpPaneDockLeft) || (direction == xtpPaneDockRight);

	if (bHoriz) m_szDocking.cx = m_rcWindow.Width();
	else m_szDocking.cy = m_rcWindow.Height();

	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = GetNext(pos);
		if (!pPane->IsEmpty())
		{
			if (bHoriz) pPane->m_szDocking.cx = m_szDocking.cx;
			else pPane->m_szDocking.cy = m_szDocking.cy;
		}
	}
}

void CXTPDockingPaneTabbedContainer::OnCaptionButtonClick(CXTPDockingPaneCaptionButton* pButton)
{
	CXTPDockingPane* pSelectedPane = m_pSelectedPane;

	CXTPDockingPaneManager* pManager = GetDockingPaneManager();

	switch (pButton->GetID())
	{
	case XTP_IDS_DOCKINGPANE_CLOSE:

		if (pManager->m_bCloseGroupOnButtonClick)
		{
			POSITION pos = m_lstPanes.GetTailPosition();
			while (pos)
			{
				CXTPDockingPane* pPane = (CXTPDockingPane*)m_lstPanes.GetPrev(pos);

				if ((pPane->GetOptions() & xtpPaneNoCloseable) != 0)
					continue;

				pPane->InternalAddRef();

				if (!pManager->_OnAction(xtpPaneActionClosing, pPane))
				{
					pPane->Close();
					pManager->_OnAction(xtpPaneActionClosed, pPane);
				}

				pPane->InternalRelease();
			}

		}
		else if (pSelectedPane)
		{
			pSelectedPane->InternalAddRef();
			if (!pManager->_OnAction(xtpPaneActionClosing, pSelectedPane))
			{
				pSelectedPane->Close();
				pManager->_OnAction(xtpPaneActionClosed, pSelectedPane);
			}
			pSelectedPane->InternalRelease();
		}
		break;


	case XTP_IDS_DOCKINGPANE_AUTOHIDE:

		if (!IsHidden())
		{
			if (!pManager->_OnAction(xtpPaneActionUnpinning, pSelectedPane))
			{
				GetDockingSite()->SetFocus();
				NormalizeDockingSize();
				pSelectedPane->Hide();
				pManager->_OnAction(xtpPaneActionUnpinned, pSelectedPane);
			}

		}
		else
		{
			GetDockingPaneManager()->ToggleDocking(pManager->m_bHideGroupOnButtonClick ?
				(CXTPDockingPaneBase*)this : (CXTPDockingPaneBase*)pSelectedPane);
		}

		break;

	case XTP_IDS_DOCKINGPANE_MAXIMIZE:
		Maximize();
		break;

	case XTP_IDS_DOCKINGPANE_RESTORE:
		Restore();
		break;

	}
}

BOOL CXTPDockingPaneTabbedContainer::OnCaptionButtonDown(CXTPDockingPaneCaptionButton* pButton)
{
	switch (pButton->GetID())
	{
	case XTP_IDS_DOCKINGPANE_MENU:
		if (m_pSelectedPane)
		{
			CXTPDockingPaneManager* pManager = GetDockingPaneManager();
			XTP_DOCKINGPANE_CLICK menu;

			menu.rcExclude = pButton->GetRect();
			ClientToScreen(&menu.rcExclude);

			menu.pt = CPoint(menu.rcExclude.left, menu.rcExclude.bottom);
			menu.pPane = m_pSelectedPane;
			pButton->m_bPressed = TRUE;
			Invalidate(FALSE);

			pManager->NotifyOwner(XTP_DPN_PANEMENUCLICK, (LPARAM)&menu);

			pButton->m_bPressed = FALSE;
			if (m_hWnd) Invalidate(FALSE);
		}
		return TRUE;

	}
	return FALSE;
}


void CXTPDockingPaneTabbedContainer::_RestoreFocus()
{
	if (m_pSelectedPane)
		m_pSelectedPane->SetFocus();
	else
		SetFocus();

	GetDockingPaneManager()->UpdatePanes();
}


void CXTPDockingPaneTabbedContainer::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{

	CXTPDockingPaneCaptionButton* pButton = HitTestCaptionButton(point);

	if (pButton)
	{
		_RestoreFocus();

		if (m_pSelectedPane && OnCaptionButtonDown(pButton))
			return;

		if (pButton->Click(this, point) && m_pSelectedPane)
		{
			OnCaptionButtonClick(pButton);
		}

		return;
	}

	if (IsTabsVisible() && PerformClick(m_hWnd, point, TRUE))
		return;

	int nHit = HitTest(point);
	if (nHit == DOCKINGPANE_HITCAPTION && !IsHidden())
	{
		ClientToScreen(&point);
		OnCaptionLButtonDown(point);
	}
	else if (nHit >= 0)
	{
		CXTPDockingPane* pPane = GetItemPane(nHit);
		if (GetDockingPaneManager()->_OnAction(xtpPaneActionDragging, pPane))
		{
			SelectPane(pPane, TRUE, FALSE);
			PerformMouseMove(m_hWnd, point);
			Invalidate(FALSE);
			return;
		}

		m_lstRects.RemoveAll();
		for (int j = 0; j < GetItemCount(); j++)
		{
			CRect rc(GetItem(j)->GetRect());
			m_lstRects.Add(rc);
		}

		m_pTrackingPane = pPane;
		SelectPane(m_pTrackingPane, TRUE, FALSE);

		PerformMouseMove(m_hWnd, point);

		SetCapture();

		Invalidate(FALSE);
	}
	else
	{
		_RestoreFocus();
	}
}

CXTPDockingPane* CXTPDockingPaneTabbedContainer::GetItemPane(int nIndex) const
{
	return (CXTPDockingPane*)GetItem(nIndex)->GetData();
}

void CXTPDockingPaneTabbedContainer::OnLButtonDblClk(UINT /*nFlags*/, CPoint point)
{
	if (PerformClick(m_hWnd, point, TRUE))
		return;

	if (HitTestCaptionButton(point))
		return;

	int nHit = HitTest(point);
	if (nHit == DOCKINGPANE_HITCAPTION)
	{
		CXTPDockingPane* pSelected = GetSelected();
		if (IsHidden() &&pSelected && ((pSelected->GetOptions() & xtpPaneNoHideable) != 0))
			return;

		GetDockingPaneManager()->ToggleDocking(this);
	}
	else if (nHit >= 0)
	{
		CXTPDockingPane* pPane = GetItemPane(nHit);
		GetDockingPaneManager()->ToggleDocking(pPane);
	}

}


void CXTPDockingPaneTabbedContainer::_Swap(CXTPDockingPane* p1, CXTPDockingPane* p2)
{
	POSITION pos1 = m_lstPanes.Find(p1);
	POSITION pos2 = m_lstPanes.Find(p2);
	ASSERT(pos1 && pos2);

	m_lstPanes.SetAt(pos1, p2);
	m_lstPanes.SetAt(pos2, p1);
}

void CXTPDockingPaneTabbedContainer::OnMouseMove(UINT nFlags, CPoint point)
{
	PerformMouseMove(m_hWnd, point);

	if (m_pTrackingPane)
	{
		for (int i = 0; i < m_lstRects.GetSize(); i++)
		{
			if (GetItemPane(i) != m_pTrackingPane && m_lstRects[i].PtInRect(point))
			{
				_Swap(GetItemPane(i), m_pTrackingPane);

				InvalidatePane(TRUE);
				return;
			}
		}


		if (CXTPTabManager::HitTest(point) == NULL)
		{
			if (GetDockingPaneManager()->_OnAction(xtpPaneActionDetaching, m_pTrackingPane))
				return;

			ASSERT(m_pSelectedPane == m_pTrackingPane);
			m_pTrackingPane = NULL;
			ReleaseCapture();

			ClientToScreen(&point);

			CXTPDockingPaneContext* pContext = GetDockingPaneManager()->GetDockingContext();
			pContext->Drag(m_pSelectedPane, point);
		}
	}
	else
	{
		m_pCaptionButtons->CheckForMouseOver(point);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CXTPDockingPaneTabbedContainer::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPDockingPaneTabbedContainer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pTrackingPane)
	{
		m_pTrackingPane = NULL;
		if (GetCapture() == this) ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CXTPDockingPaneTabbedContainer::OnCaptureChanged(CWnd* pWnd)
{
	m_pTrackingPane = NULL;
	CWnd::OnCaptureChanged(pWnd);
}

void CXTPDockingPaneTabbedContainer::OnParentContainerChanged(CXTPDockingPaneBase* pContainer)
{
	m_pDockingSite = pContainer->m_pDockingSite;
	m_bTitleVisible = TRUE;
	if (pContainer->GetType() == xtpPaneTypeAutoHidePanel)
	{
		if (m_hWnd) ShowWindow(SW_HIDE); //?
	}
}

BOOL CXTPDockingPaneTabbedContainer::CanAttach(CRect& rcClient, CPoint pt) const
{
	CRect rcTitle(rcClient);
	BOOL bVertical = IsCaptionVertical();

	if (m_bTitleVisible)
	{
		if (bVertical)
		{
			rcClient.left += CXTPDockingPaneBase::GetPaintManager()->GetCaptionHeight() + 3;
		}
		else
		{
			rcClient.top += CXTPDockingPaneBase::GetPaintManager()->GetCaptionHeight() + 3;
		}
	}
	else
	{
		SAFE_CALLPTR(m_pDockingSite, GetWindowRect(rcTitle));
	}

	if (bVertical)
	{
		rcTitle.right = rcClient.left;
	}
	else
	{
		rcTitle.bottom = rcClient.top;
	}

	if (rcTitle.PtInRect(pt))
		return TRUE;

	if (IsTabsVisible())
	{
		CRect rcTabs(rcClient);
		rcClient.bottom -= CXTPDockingPaneBase::GetPaintManager()->GetTabsHeight();
		rcTabs.top = rcClient.bottom;

		if (rcTabs.PtInRect(pt))
			return TRUE;
	}

	return FALSE;
}

void CXTPDockingPaneTabbedContainer::Copy(CXTPDockingPaneTabbedContainer* pClone, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions)
{
	ASSERT(pClone);
	if (!pClone)
		return;

	m_bActive = FALSE;
	m_pDockingSite = GetDockingPaneManager()->GetSite();
	m_pSelectedPane = m_pTrackingPane = NULL;

	m_rcWindow = pClone->m_rcWindow;
	m_szDocking = pClone->m_szDocking;
	m_bTitleVisible = TRUE;
	m_bLockReposition = FALSE;

	CXTPDockingPane* pSelected = NULL;
	if (pMap)
	{
		POSITION pos = pClone->GetHeadPosition();
		while (pos)
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)pClone->GetNext(pos);
			CXTPDockingPane* pNewPane = (CXTPDockingPane*)pPane->Clone(m_pLayout, pMap);
			if (pClone->GetSelected() == pPane) pSelected = pNewPane;
			_InsertPane(pNewPane);
		}
	}
	else
	{

		POSITION pos = pClone->m_lstPanes.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)pClone->GetNext(pos);

			ASSERT(pPane->GetType() == xtpPaneTypeDockingPane);

			if ((((CXTPDockingPane*)pPane)->GetOptions() & dwIgnoredOptions) == 0)
			{
				if (pClone->GetSelected() == pPane && pSelected == NULL) pSelected = pPane;

				pPane->m_pParentContainer->RemovePane(pPane);
				_InsertPane(pPane);
			}
		}
	}
	if (pSelected) SelectPane(pSelected, FALSE);


}

CXTPDockingPaneBase* CXTPDockingPaneTabbedContainer::Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions)
{
	CXTPDockingPaneTabbedContainer* pPane = (CXTPDockingPaneTabbedContainer*)
		GetDockingPaneManager()->OnCreatePane(GetType(), pLayout);

	pPane->Copy(this, pMap, dwIgnoredOptions);

	SAFE_CALLPTR(pMap, SetAt(this, pPane));

	return pPane;

}

BOOL CXTPDockingPaneTabbedContainer::IsHidden() const
{
	return GetContainer() && GetContainer()->GetType() == xtpPaneTypeAutoHidePanel;
}

void CXTPDockingPaneTabbedContainer::Show(BOOL bSetFocus)
{
	if (m_pSelectedPane &&m_pParentContainer &&
		m_pParentContainer->GetType() == xtpPaneTypeAutoHidePanel)
	{
		((CXTPDockingPaneAutoHidePanel*)m_pParentContainer)->ShowPane(m_pSelectedPane, bSetFocus);
	}
	else if (DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, GetParentFrame()))
	{
		((CXTPDockingPaneMiniWnd*)GetParentFrame())->Expand();
	}

}

BOOL CXTPDockingPaneTabbedContainer::IsAllowMaximize() const
{
	if (!m_pParentContainer || m_pParentContainer->GetType() != xtpPaneTypeSplitterContainer)
		return FALSE;

	if (!GetDockingPaneManager()->IsCaptionMaximizeButtonsVisible())
		return FALSE;

	CXTPDockingPaneBaseList lst;
	((CXTPDockingPaneSplitterContainer*)m_pParentContainer)->FindChildPane(xtpPaneTypeTabbedContainer, &lst);

	return lst.GetCount() > 1;
}

void CXTPDockingPaneTabbedContainer::Maximize()
{
	if (!m_pDockingSite || !m_pParentContainer)
		return;

	CXTPDockingPaneBaseList lst;
	((CXTPDockingPaneSplitterContainer*)m_pParentContainer)->FindChildPane(xtpPaneTypeTabbedContainer, &lst);
	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneTabbedContainer* pTabbedContainer = (CXTPDockingPaneTabbedContainer*)lst.GetNext(pos);
		pTabbedContainer->m_bMaximized = pTabbedContainer == this;
	}

	GetDockingPaneManager()->RecalcFrameLayout(this);
}

void CXTPDockingPaneTabbedContainer::Restore()
{
	if (!m_pDockingSite || !m_pParentContainer)
		return;

	m_bMaximized = FALSE;
	GetDockingPaneManager()->RecalcFrameLayout(this);
}

BOOL CXTPDockingPaneTabbedContainer::IsPaneMaximized() const
{
	if (!IsAllowMaximize())
		return FALSE;

	return m_bMaximized;
}

BOOL CXTPDockingPaneTabbedContainer::IsPaneRestored() const
{
	if (!IsAllowMaximize())
		return FALSE;

	return !m_bMaximized;
}

BOOL CXTPDockingPaneTabbedContainer::IsPaneMinimized() const
{
	if (m_bMaximized)
		return FALSE;

	if (!m_pParentContainer || m_pParentContainer->GetType() != xtpPaneTypeSplitterContainer)
		return FALSE;

	if (!GetDockingPaneManager()->IsCaptionMaximizeButtonsVisible())
		return FALSE;

	CXTPDockingPaneBaseList lst;
	((CXTPDockingPaneSplitterContainer*)m_pParentContainer)->FindChildPane(xtpPaneTypeTabbedContainer, &lst);
	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneTabbedContainer* pTabbedContainer = (CXTPDockingPaneTabbedContainer*)lst.GetNext(pos);
		if (pTabbedContainer->m_bMaximized)
			return TRUE;
	}
	return FALSE;
}



BOOL CXTPDockingPaneTabbedContainer::IsCaptionButtonVisible(CXTPDockingPaneCaptionButton* pButton)
{
	if (pButton->GetID() == XTP_IDS_DOCKINGPANE_CLOSE)
		return m_pSelectedPane && ((m_pSelectedPane->GetOptions() & xtpPaneNoCloseable) == 0);

	if (pButton->GetID() == XTP_IDS_DOCKINGPANE_AUTOHIDE)
		return m_pSelectedPane && ((m_pSelectedPane->GetOptions() & xtpPaneNoHideable) == 0) && (pButton->GetState() & xtpPanePinVisible);

	if (pButton->GetID() == XTP_IDS_DOCKINGPANE_MAXIMIZE)
		return IsPaneRestored();

	if (pButton->GetID() == XTP_IDS_DOCKINGPANE_RESTORE)
		return IsPaneMaximized();

	if (pButton->GetID() == XTP_IDS_DOCKINGPANE_MENU)
		return m_pSelectedPane && (m_pSelectedPane->GetOptions() & xtpPaneHasMenuButton);

	return TRUE;
}


LRESULT CXTPDockingPaneTabbedContainer::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	CXTPDockingPane* pPane = GetSelected();
	CPoint point((DWORD)lParam);

	int nHit = HitTest(point);

	if (nHit >= 0)
	{
		pPane = GetItemPane(nHit);
	}

	if (!pPane)
		return 0;

	int nIDHelp = pPane->m_nIDHelp;

	if (nIDHelp == 0)
	{
		pPane->m_hwndChild ? (int)::GetDlgCtrlID(pPane->m_hwndChild): 0;
	}

	if (nIDHelp == 0)
	{
		nIDHelp = pPane->GetID();
	}

	return HID_BASE_RESOURCE + nIDHelp;
}

CXTPDockingPaneCaptionButton* CXTPDockingPaneTabbedContainer::HitTestCaptionButton(CPoint point) const
{
	if (IsTitleVisible())
	{
		for (int i = 0; i < m_pCaptionButtons->GetSize(); i++)
		{
			CXTPDockingPaneCaptionButton* pButton = m_pCaptionButtons->GetAt(i);
			if (pButton->PtInRect(point))
				return pButton->IsEnabled() ? pButton : NULL;
		}
	}

	return NULL;
}

INT_PTR CXTPDockingPaneTabbedContainer::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{

	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CControlBar
	INT_PTR nHit = CWnd::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	CXTPDockingPaneCaptionButton* pButton = HitTestCaptionButton(point);

	if (pButton)
	{
		nHit = (INT_PTR)pButton->GetID();
		CString strTip;
		XTPResourceManager()->LoadString(&strTip, (UINT)nHit);

		if (strTip.GetLength() == 0)
			return -1;

		CXTPToolTipContext::FillInToolInfo(pTI, m_hWnd, pButton->GetRect(), nHit, strTip);

		return nHit;
	}

	return PerformToolHitTest(m_hWnd, point, pTI);
}

void CXTPDockingPaneTabbedContainer::AdjustMinMaxInfoClientRect(LPMINMAXINFO pMinMaxInfo, BOOL bCaptionOnly) const
{
	CRect rc(0, 0, 1000, 1000);
	if (bCaptionOnly)
	{
		CXTPDockingPaneBase::GetPaintManager()->AdjustCaptionRect(this, rc);
	}
	else
	{
		CXTPDockingPaneBase::GetPaintManager()->AdjustClientRect((CXTPDockingPaneTabbedContainer*)this, rc, FALSE);
	}

	pMinMaxInfo->ptMinTrackSize.x += 1000 - rc.Width();
	pMinMaxInfo->ptMinTrackSize.y += 1000 - rc.Height();

	pMinMaxInfo->ptMaxTrackSize.x += 1000 - rc.Width();
	pMinMaxInfo->ptMaxTrackSize.y += 1000 - rc.Height();

}

void CXTPDockingPaneTabbedContainer::GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo) const
{
	CXTPDockingPaneBase::GetMinMaxInfo(pMinMaxInfo);

	if (IsEmpty())
		return;

	if (IsPaneMinimized())
	{
		if (((CXTPDockingPaneSplitterContainer*)m_pParentContainer)->IsHoriz())
			pMinMaxInfo->ptMaxTrackSize.x = 0;
		else
			pMinMaxInfo->ptMaxTrackSize.y = 0;

		AdjustMinMaxInfoClientRect(pMinMaxInfo, FALSE);
	}
	else
	{
		POSITION pos = GetHeadPosition();
		while (pos)
		{
			CXTPDockingPaneBase* pPane = GetNext(pos);

			MINMAXINFO info;
			pPane->GetMinMaxInfo(&info);

			pMinMaxInfo->ptMinTrackSize.x = max(pMinMaxInfo->ptMinTrackSize.x, info.ptMinTrackSize.x);
			pMinMaxInfo->ptMinTrackSize.y = max(pMinMaxInfo->ptMinTrackSize.y, info.ptMinTrackSize.y);

			pMinMaxInfo->ptMaxTrackSize.x = min(pMinMaxInfo->ptMaxTrackSize.x, info.ptMaxTrackSize.x);
			pMinMaxInfo->ptMaxTrackSize.y = min(pMinMaxInfo->ptMaxTrackSize.y, info.ptMaxTrackSize.y);
		}
		AdjustMinMaxInfoClientRect(pMinMaxInfo);
	}
}


BOOL CXTPDockingPaneTabbedContainer::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CXTPDockingPaneManager* pManager = GetDockingPaneManager();

	if (pManager)
	{
		pManager->GetToolTipContext()->FilterToolTipMessage(this, message, wParam, lParam);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}


BOOL CXTPDockingPaneTabbedContainer::IsCaptionVertical() const
{
	CXTPDockingPaneManager* pManager = GetDockingPaneManager();

	switch (pManager->GetCaptionDirection())
	{
		case xtpPaneCaptionHorizontal:
			return FALSE;

		case xtpPaneCaptionAutoBySize:
			return m_rcWindow.Width() > m_rcWindow.Height();

		case xtpPaneCaptionAutoByPosition:
			XTPDockingPaneDirection direction = pManager->GetPaneDirection(this);
			return (direction == xtpPaneDockTop) || (direction == xtpPaneDockBottom);

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Accessible

CCmdTarget* CXTPDockingPaneTabbedContainer::GetAccessible()
{
	return this;
}

HRESULT CXTPDockingPaneTabbedContainer::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	*ppdispParent = NULL;

	if (GetSafeHwnd())
	{
		return AccessibleObjectFromWindow(GetSafeHwnd(), OBJID_WINDOW, IID_IDispatch, (void**)ppdispParent);
	}
	return E_FAIL;
}

HRESULT CXTPDockingPaneTabbedContainer::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
		return E_INVALIDARG;

	*pChildCount = GetItemCount() + 1;

	return S_OK;
}

HRESULT CXTPDockingPaneTabbedContainer::GetAccessibleChild(VARIANT varChild, IDispatch* FAR* ppdispChild)
{
	*ppdispChild = NULL;
	int nChild = GetChildIndex(&varChild);

	if (nChild == 1 && GetSelected())
		return AccessibleObjectFromWindow(GetSelected()->m_hwndChild, OBJID_WINDOW, IID_IDispatch, (void**)ppdispChild);

	if (nChild > 1 && nChild <= GetItemCount() + 1)
	{
		CXTPDockingPane* pPane = GetItemPane(nChild - 2);
		if (pPane)
		{
			*ppdispChild = pPane->GetIDispatch(TRUE);
		}
	}

	return S_OK;
}

HRESULT CXTPDockingPaneTabbedContainer::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF || nChild == -1)
	{
		*pszName = SysAllocString(L"XTPDockingPaneTabbedContainer");
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPDockingPaneTabbedContainer::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_PAGETABLIST;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPDockingPaneTabbedContainer::GetAccessibleState(VARIANT /*varChild*/, VARIANT* pvarState)
{
	pvarState->vt = VT_I4;
	pvarState->lVal = 0;

	return S_OK;
}


HRESULT CXTPDockingPaneTabbedContainer::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	*pxLeft = *pyTop = *pcxWidth = *pcyHeight = 0;

	if (!GetSafeHwnd())
		return S_OK;

	int nChild = GetChildIndex(&varChild);

	if (nChild != CHILDID_SELF)
		return E_INVALIDARG;

	CRect rc;
	GetWindowRect(&rc);

	*pxLeft = rc.left;
	*pyTop = rc.top;
	*pcxWidth = rc.Width();
	*pcyHeight = rc.Height();

	return S_OK;
}

HRESULT CXTPDockingPaneTabbedContainer::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	if (pvarID == NULL)
		return E_INVALIDARG;

	pvarID->vt = VT_EMPTY;

	if (!GetSafeHwnd())
		return S_FALSE;

	if (!CXTPWindowRect(this).PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	CPoint pt(xLeft, yTop);
	ScreenToClient(&pt);

	int nItem = HitTest(pt);
	if (nItem == DOCKINGPANE_HITCAPTION)
	{
		pvarID->vt = VT_I4;
		pvarID->lVal = 0;
		return S_OK;
	}

	if (nItem == -1)
	{
		pvarID->vt = VT_DISPATCH;
		if (GetSelected())
			return AccessibleObjectFromWindow(GetSelected()->m_hwndChild, OBJID_WINDOW, IID_IDispatch, (void**)&pvarID->pdispVal);

		return S_FALSE;
	}

	pvarID->vt = VT_DISPATCH;
	pvarID->pdispVal = GetItemPane(nItem)->GetIDispatch(TRUE);

	return S_OK;
}


LRESULT CXTPDockingPaneTabbedContainer::OnGetObject(WPARAM wParam, LPARAM lParam)
{
	if (lParam != OBJID_CLIENT)
		return (LRESULT)Default();

	LPUNKNOWN lpUnknown = GetInterface(&IID_IAccessible);
	if (!lpUnknown)
		return E_FAIL;

	return LresultFromObject(IID_IAccessible, wParam, lpUnknown);
}


BEGIN_INTERFACE_MAP(CXTPDockingPaneTabbedContainer, CCmdTarget)
	INTERFACE_PART(CXTPDockingPaneTabbedContainer, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()
