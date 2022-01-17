// XTPTaskPanel.cpp : implementation of the CXTPTaskPanel class.
//
// This file is a part of the XTREME TASKPANEL MFC class library.
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

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPToolTipContext.h"
#include "Common/XTPResourceManager.h"

#if !defined(_XTP_ACTIVEX) && defined(_XTP_INCLUDE_COMMANDBARS)
#include "CommandBars/XTPCommandBar.h"
#include "CommandBars/XTPMouseManager.h"
#endif

#include "XTPTaskPanelItems.h"
#include "XTPTaskPanelGroup.h"
#include "XTPTaskPanel.h"
#include "XTPTaskPanelPaintManager.h"
#include "XTPTaskPanelGroupItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SPI_GETMOUSEHOVERTIME
#define SPI_GETMOUSEHOVERTIME 102
#endif

#define TID_REPOSITION 200
#define TID_SCROLLBARS 201
#define TID_SCROLLGROUP 202
#define TID_GROUPHOVER 203

#define IsItemLink(pItem) (pItem && pItem->GetType() == xtpTaskItemTypeLink)
#define IsItemGroup(pItem) (pItem && pItem->GetType() == xtpTaskItemTypeGroup)

CLIPFORMAT CXTPTaskPanel::m_cfItem = (CLIPFORMAT)::RegisterClipboardFormat(_T("TaskPanelGroupItem"));

BOOL CXTPTaskPanel::GetMessageExt(MSG& msg)
{
	while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
	{
		if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
			break;
		DispatchMessage(&msg);
	}

	if (!::GetMessage(&msg, NULL, 0, 0))
	{
		AfxPostQuitMessage((int)msg.wParam);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CPanelDropTarget
class CXTPTaskPanel::CPanelDropTarget : public COleDropTarget
{
public:
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		CXTPTaskPanel* pPanel = DYNAMIC_DOWNCAST(CXTPTaskPanel, pWnd);
		if (!pPanel)
			return DROPEFFECT_NONE;

		return pPanel->OnDragOver(pDataObject, dwKeyState, point);
	}

	virtual void OnDragLeave(CWnd* pWnd)
	{
		CXTPTaskPanel* pPanel = DYNAMIC_DOWNCAST(CXTPTaskPanel, pWnd);

		if (pPanel)
		{
			pPanel->OnDragOver(NULL, 0, CPoint(-1, -1));
		}
	}
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
	{
		CXTPTaskPanel* pPanel = DYNAMIC_DOWNCAST(CXTPTaskPanel, pWnd);

		if (pPanel)
		{
			return pPanel->OnDrop(pDataObject, dropEffect, point);
		}
		return FALSE;
	}
};

//////////////////////////////////////////////////////////////////////////
// CXTPTaskPanelScrollButton

CXTPTaskPanelScrollButton::CXTPTaskPanelScrollButton(BOOL bScrollUp)
{
	this->bScrollUp = bScrollUp;
	bEnabled = FALSE;
	bPressed = FALSE;
	bHot = FALSE;
	rcButton.SetRectEmpty();
	pGroupScroll = pGroupCaption = NULL;
}

BOOL CXTPTaskPanelScrollButton::IsVisible() const
{
	if (rcButton.IsRectEmpty())
		return FALSE;
	if (pGroupScroll == NULL)
		return FALSE;
	if (!bEnabled && !pGroupScroll->GetTaskPanel()->GetPaintManager()->m_bCaptionScrollButton)
		return FALSE;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// CXTPTaskPanelEditItem

class CXTPTaskPanelEditItem : public CEdit
{
public:
	CXTPTaskPanelEditItem();

public:
	BOOL Create(CXTPTaskPanelItem* pItem);

protected:
	virtual void PostNcDestroy();

protected:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

protected:
	BOOL m_bEscapeKey;
	CXTPTaskPanelItem* m_pItem;

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CXTPTaskPanelEditItem)

};

AFX_INLINE BOOL CXTPTaskPanelEditItem::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE BOOL CXTPTaskPanelEditItem::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CEdit::Create(dwStyle, rect, pParentWnd, nID);
}

IMPLEMENT_DYNAMIC(CXTPTaskPanelEditItem, CEdit)

CXTPTaskPanelEditItem::CXTPTaskPanelEditItem()
{
	m_bEscapeKey = FALSE;
	m_pItem = NULL;
}

BEGIN_MESSAGE_MAP(CXTPTaskPanelEditItem, CEdit)
	//{{AFX_MSG_MAP(CXTPTaskPanelEditItem)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT CXTPTaskPanelEditItem::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

void CXTPTaskPanelEditItem::OnKillFocus(CWnd* /*pNewWnd*/)
{
	PostMessage(WM_CLOSE);

	if (!m_bEscapeKey && m_pItem)
	{
		CString str;
		GetWindowText(str);

		if (!str.IsEmpty())
		{
			m_pItem->GetTaskPanel()->OnEndLabelEdit(m_pItem, str);
		}
		m_pItem = NULL;
	}
}

void CXTPTaskPanelEditItem::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		m_bEscapeKey = (nChar == VK_ESCAPE);
		OnKillFocus(NULL);

		return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CXTPTaskPanelEditItem::PostNcDestroy()
{
	delete this;
}

BOOL CXTPTaskPanelEditItem::Create(CXTPTaskPanelItem* pItem)
{
	m_pItem = pItem;

	CXTPTaskPanel* pPanel = pItem->GetTaskPanel();
	CFont* pFont = m_pItem->GetItemFont();

	UINT nFormat = 0;
	CRect rc = pItem->GetTextRect(&nFormat);
	rc.OffsetRect(0, -pPanel->GetScrollOffset());

	CString strText = pItem->GetCaption();
	UINT nCenter = nFormat & DT_CENTER ? ES_CENTER : 0;

	if (!CEdit::Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | nCenter, rc, pPanel, 0))
		return FALSE;

	SetFont(pFont);
	SetMargins(0, 0);
	SetWindowText(strText);
	SetFocus();
	SetSel(0, -1);

	return TRUE;
}

void CXTPTaskPanel::RenameItem(CXTPTaskPanelItem* pItem)
{
	Reposition(FALSE);
	EnsureVisible(pItem);
	UpdateWindow();

	CXTPTaskPanelEditItem* pEditItem = new CXTPTaskPanelEditItem;
	pEditItem->Create(pItem);
}

void CXTPTaskPanel::OnEndLabelEdit(CXTPTaskPanelItem* pItem, LPCTSTR str)
{
	XTP_TPNENDLABELEDIT ele;
	ele.pItem = pItem;
	ele.strNewString = str;

	if (NotifyOwner(XTP_TPN_ENDLABELEDIT, (LPARAM)&ele) == XTP_ACTION_CANCEL)
		return;

	pItem->SetCaption(ele.strNewString);
}


/////////////////////////////////////////////////////////////////////////////
// CXTPTaskPanel

IMPLEMENT_DYNAMIC(CXTPTaskPanel, CWnd)

CXTPTaskPanel::CXTPTaskPanel()
{
	RegisterWindowClass();

	m_pPaintManager = NULL;
	SetTheme(xtpTaskPanelThemeNativeWinXP);

	m_pGroups = new CXTPTaskPanelGroups(this);
	m_pGroups->m_pOwner = this;

	m_nClientHeight = m_nOldClientHeight = 0;

	m_rcMargins = CXTPTaskPanelPaintManager::rectDefault;
	m_nGroupSpacing = CW_USEDEFAULT;

	m_szItemIcon = CSize(16, 16);
	m_szGroupIcon = CSize(32, 32);

	m_bGrabbingFocus = FALSE;
	m_pItemHot = 0;
	m_pItemFocused = 0;
	m_pItemPressed = 0;

	m_nAnimateTimer = 0;
	m_bScrollVisible = FALSE;

	m_hHandCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));

	if (m_hHandCursor == 0)
		m_hHandCursor = XTPResourceManager()->LoadCursor(XTP_IDC_HAND);

	m_bExpandable = TRUE;

	SetAnimation(xtpTaskPanelAnimationYes);

	m_nLockRedraw = 0;
	m_nOLEDropMode = 0;

	m_bAcceptFocus = TRUE;
	m_bDrawFocusRect = FALSE;

	m_pImageManager = new CXTPImageManager();

	m_itemLayout = xtpTaskItemLayoutDefault;
	m_hotTrackStyle = xtpTaskPanelHighlightDefault;

	m_panelBehaviour = xtpTaskPanelBehaviourExplorer;

	m_pActiveGroup = NULL;
	m_bSelectItemOnFocus = FALSE;
	m_bSingleSelection = FALSE;

	m_nAllowDrag = xtpTaskItemAllowDragNone;
	m_bAllowDrop = TRUE;

	m_ptPressed = CPoint(0, 0);

	m_pItemDragging = m_pItemDragOver = NULL;

	OleInitialize(NULL);
	m_pDropTarget = new CPanelDropTarget();

	m_pScrollButton[TRUE] = new CXTPTaskPanelScrollButton(TRUE);
	m_pScrollButton[FALSE] = new CXTPTaskPanelScrollButton(FALSE);

	m_bTimerGroupHover = FALSE;

	m_pToolTipContext = new CXTPToolTipContext;

	m_bMultiColumn = FALSE;
	m_nColumnWidth = 100;
	m_nGroupMinClientHeight = 0;

	m_bInUpdateScrollBar = FALSE;

	m_bTabItems = FALSE;

	EnableAutomation();
}

BOOL CXTPTaskPanel::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	return XTPDrawHelpers()->RegisterWndClass(hInstance, _T("XTPTaskPanel"), CS_DBLCLKS);
}

CXTPTaskPanel::~CXTPTaskPanel()
{
	CMDTARGET_RELEASE(m_pItemHot);

	CMDTARGET_RELEASE(m_pItemFocused);

	CMDTARGET_RELEASE(m_pActiveGroup);

	if (m_pGroups)
	{
		m_pGroups->Clear(FALSE);
		CMDTARGET_RELEASE(m_pGroups);
	}

	CMDTARGET_RELEASE(m_pImageManager);

	CMDTARGET_RELEASE(m_pToolTipContext);

	delete m_pPaintManager;
	delete m_pDropTarget;

	delete m_pScrollButton[TRUE];
	delete m_pScrollButton[FALSE];

	OleUninitialize();
}

void CXTPTaskPanel::AllowDrag(long nAllowDrag /* = xtpTaskItemAllowDragDefault */)
{
	m_nAllowDrag = nAllowDrag;
}

void CXTPTaskPanel::SetImageList(CImageList* pImageList, CSize szItemIcon)
{
	if (pImageList)
	{
		m_pImageManager->SetIcons(*pImageList, 0, 0, 0, xtpImageNormal);
	}

	m_szItemIcon = szItemIcon;
}

void CXTPTaskPanel::SetGroupImageList(CImageList* pImageList, CSize szGroupIcon /* = CSize */)
{
	if (pImageList)
	{
		m_pImageManager->SetIcons(*pImageList, 0, 0, 0, xtpImageNormal);
	}

	m_szGroupIcon = szGroupIcon;
}
CXTPImageManager* CXTPTaskPanel::GetImageManager() const
{
	return m_pImageManager ? m_pImageManager : XTPImageManager();
}

void CXTPTaskPanel::SetImageManager(CXTPImageManager* pImageManager)
{
	if (m_pImageManager)
		m_pImageManager->InternalRelease();

	m_pImageManager = pImageManager;

	Reposition();
}


void CXTPTaskPanel::SetHotItem(CXTPTaskPanelItem* pItem)
{
	if (m_pItemHot == pItem)
		return;

	if (m_pItemHot)
		m_pItemHot->InternalRelease();

	m_pItemHot = pItem;

	if (m_pItemHot)
		m_pItemHot->InternalAddRef();

	if (m_pItemHot)
	{
		TRACKMOUSEEVENT tme =
		{
			sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd
		};
		_TrackMouseEvent(&tme);
	}

	Invalidate(FALSE);

	NotifyOwner(XTP_TPN_HOTITEMCHANGED, 0);

}

void CXTPTaskPanel::EnsureVisibleRect(const CRect& rc)
{
	CXTPClientRect rcClient(this);
	if (rc.bottom > rcClient.bottom + GetScrollOffset())
	{
		OnSrollChanged(min(rc.top, rc.bottom - rcClient.Height()));
	}
	else if (rc.top < GetScrollOffset())
	{
		OnSrollChanged(rc.top);
	}
}


void CXTPTaskPanel::EnsureVisible(CXTPTaskPanelItem* pItem)
{
	if (!pItem || !pItem->IsVisible())
		return;

	CXTPTaskPanelGroup* pGroup = pItem->GetItemGroup();
	if (!pGroup)
		return;

	if (pItem->IsGroup())
	{
		CRect rc = pGroup->GetCaptionRect();
		EnsureVisibleRect(rc);
	}
	else
	{
		if (!pGroup->IsExpanded())
			pGroup->SetExpanded(TRUE);

		CRect rc = pGroup->GetTargetClientRect();

		if (IsExplorerBehaviour())
		{
			CRect rcItem(((CXTPTaskPanelGroupItem*)pItem)->GetItemRect());
			rcItem.OffsetRect(0, rc.top);

			EnsureVisibleRect(rcItem);
		}
		else
		{
			rc.top -= pGroup->GetTargetCaptionRect().Height();
			EnsureVisibleRect(rc);

			pGroup->EnsureVisible((CXTPTaskPanelGroupItem*)pItem);
		}
	}

}

void CXTPTaskPanel::GrabFocus()
{
	if (m_bAcceptFocus && ::GetFocus() != m_hWnd && m_hWnd)
	{
		m_bGrabbingFocus = TRUE;
		SetFocus();
		m_bGrabbingFocus = FALSE;
	}

}

void CXTPTaskPanel::SetFocusedItem(CXTPTaskPanelItem* pItem, BOOL bDrawFocusRect)
{
	if (bDrawFocusRect)
		m_bDrawFocusRect = m_bAcceptFocus;

	GrabFocus();

	if (m_pItemFocused == pItem)
		return;

	if (m_bSelectItemOnFocus && IsItemLink(pItem) && !m_bSingleSelection)
	{
		pItem->GetItemGroup()->SetSelectedItem(((CXTPTaskPanelGroupItem*)pItem));
	}

	if (m_pItemFocused)
		m_pItemFocused->InternalRelease();

	m_pItemFocused = pItem;

	if (m_pItemFocused)
		m_pItemFocused->InternalAddRef();

	if (!GetSafeHwnd())
		return;

	EnsureVisible(pItem);

	Invalidate(FALSE);

	NotifyOwner(XTP_TPN_FOCUSEDITEMCHANGED, 0);

	if (m_pItemFocused)
	{
		AccessibleNotifyWinEvent(EVENT_OBJECT_FOCUS, m_hWnd, OBJID_CLIENT, GetAccessibleIndex(m_pItemFocused));
	}

	if (m_pItemFocused && m_pItemFocused->GetType() == xtpTaskItemTypeControl)
	{
		::SetFocus(((CXTPTaskPanelGroupItem*)m_pItemFocused)->GetControlHandle());
	}
}

int CXTPTaskPanel::GetGroupCount() const
{
	return m_pGroups->GetCount();
}

BOOL CXTPTaskPanel::IsDirty() const
{
	for (int i = 0; i < GetGroupCount(); i++)
	{
		if (GetAt(i)->IsDirty())
			return TRUE;
	}
	return FALSE;
}

void CXTPTaskPanel::SetMargins(long nLeft, long nTop, long nRight, long nBottom, long nMiddle)
{
	m_rcMargins.SetRect(nLeft, nTop, nRight, nBottom);
	m_nGroupSpacing = nMiddle;
	Reposition();
}


BOOL CXTPTaskPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CreateEx(0, _T("XTPTaskPanel"), NULL, dwStyle, rect, pParentWnd, nID))
	{
		return FALSE;
	}

	m_pDropTarget->Register(this);
	m_nAnimateTimer = 0;

	return TRUE;
}


CXTPTaskPanelGroup* CXTPTaskPanel::AddGroup(UINT nID, int nImage)
{
	CXTPTaskPanelGroup* pGroup = (CXTPTaskPanelGroup*)m_pGroups->Add(new CXTPTaskPanelGroup(this), nID);
	pGroup->SetIconIndex(nImage);
	return pGroup;
}

CXTPTaskPanelGroup* CXTPTaskPanel::FindGroup(UINT nID) const
{
	return (CXTPTaskPanelGroup*)m_pGroups->Find(nID);
}

CXTPTaskPanelGroupItem* CXTPTaskPanel::FindItem(UINT nID) const
{
	for (int i = 0; i < GetGroupCount(); i++)
	{
		CXTPTaskPanelGroupItem* pItem = GetAt(i)->FindItem(nID);
		if (pItem)
		{
			return pItem;
		}
	}
	return NULL;
}

CXTPTaskPanelGroup* CXTPTaskPanel::GetAt(int nIndex) const
{
	return (CXTPTaskPanelGroup*)m_pGroups->GetAt(nIndex);
}

void CXTPTaskPanel::SetTheme(XTPTaskPanelPaintTheme paintTheme)
{
	if (m_pPaintManager)
	{
		delete m_pPaintManager;
	}

	if (paintTheme == xtpTaskPanelThemeOfficeXPPlain) m_pPaintManager = new CXTPTaskPanelOfficeXPThemePlain();
	else if (paintTheme == xtpTaskPanelThemeToolbox) m_pPaintManager = new CXTPTaskPanelToolboxTheme();
	else if (paintTheme == xtpTaskPanelThemeToolboxWhidbey) m_pPaintManager = new CXTPTaskPanelToolboxWhidbeyTheme();
	else if (paintTheme == xtpTaskPanelThemeListView) m_pPaintManager = new CXTPTaskPanelListViewTheme();
	else if (paintTheme == xtpTaskPanelThemeListViewOfficeXP) m_pPaintManager = new CXTPTaskPanelListViewOfficeXPTheme();
	else if (paintTheme == xtpTaskPanelThemeListViewOffice2003) m_pPaintManager = new CXTPTaskPanelListViewOffice2003Theme();
	else if (paintTheme == xtpTaskPanelThemeOffice2000Plain) m_pPaintManager = new CXTPTaskPanelPaintManagerPlain();
	else if (paintTheme == xtpTaskPanelThemeOffice2003Plain) m_pPaintManager = new CXTPTaskPanelOffice2003ThemePlain();
	else if (paintTheme == xtpTaskPanelThemeNativeWinXPPlain) m_pPaintManager = new CXTPTaskPanelExplorerThemePlain();
	else if (paintTheme == xtpTaskPanelThemeOffice2000) m_pPaintManager = new CXTPTaskPanelPaintManager();
	else if (paintTheme == xtpTaskPanelThemeOffice2003) m_pPaintManager = new CXTPTaskPanelOffice2003Theme();
	else if (paintTheme == xtpTaskPanelThemeShortcutBarOffice2003) m_pPaintManager = new CXTPTaskPanelShortcutBarOffice2003Theme();
	else m_pPaintManager = new CXTPTaskPanelExplorerTheme();

	m_paintTheme = paintTheme;

	m_pPaintManager->RefreshMetrics();

	Reposition();

}
void CXTPTaskPanel::SetCustomTheme(CXTPTaskPanelPaintManager* pPaintManager)
{
	delete m_pPaintManager;
	m_pPaintManager = pPaintManager;

	if (m_pPaintManager)
	{
		m_pPaintManager->RefreshMetrics();
	}

	m_paintTheme = xtpTaskPanelThemeCustom;

	Reposition();
}



BEGIN_MESSAGE_MAP(CXTPTaskPanel, CWnd)
	//{{AFX_MSG_MAP(CXTPTaskPanel)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_GETDLGCODE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PRINT, OnPrint)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrint)
	ON_MESSAGE(WM_GETOBJECT, OnGetObject)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPTaskPanel message handlers

BOOL CXTPTaskPanel::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPTaskPanel::OnPaint()
{
	CPaintDC dcPaint(this);
	CXTPClientRect rc(this);
	CXTPBufferDC dc(dcPaint, rc);

	dc.SetWindowOrg(0, GetScrollOffset()) ;
	OnDraw(&dc);
	dc.SetWindowOrg(0, 0);
}

LRESULT  CXTPTaskPanel::OnPrint(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC)
	{
		if (lParam & PRF_NONCLIENT)
		{
			Default();
		}

		CPoint pt = pDC->GetViewportOrg();
		pDC->SetViewportOrg(pt.x, pt.y - GetScrollOffset()) ;
		OnDraw(pDC);

		if (lParam & PRF_CHILDREN)
		{
			CWnd* pWnd = GetWindow(GW_CHILD);
			while (pWnd)
			{
				if (pWnd->IsWindowVisible())
				{
					CRect rc;
					pWnd->GetWindowRect(&rc);
					ScreenToClient(&rc);

					pDC->SetViewportOrg(rc.left, rc.top);
					pWnd->Print(pDC, (DWORD)lParam);
				}
				pWnd = pWnd->GetWindow(GW_HWNDNEXT);
			}
		}
	}

	return TRUE;
}

void CXTPTaskPanel::OnDraw(CDC* pDC)
{

	m_pPaintManager->FillTaskPanel(pDC, this);

	for (int i = 0; i < GetGroupCount(); i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);
		if (!pGroup->IsVisible())
			continue;

		pGroup->OnPaint(pDC);
	}

	m_pPaintManager->DrawScrollButton(pDC, m_pScrollButton[TRUE]);
	m_pPaintManager->DrawScrollButton(pDC, m_pScrollButton[FALSE]);
}


void CXTPTaskPanel::SetLockRedraw(BOOL bLockRedraw)
{
	m_nLockRedraw += bLockRedraw ? +1 : -1;
	ASSERT(m_nLockRedraw >= 0);

	if (m_nLockRedraw == 0)
		Reposition();
}

void CXTPTaskPanel::UpdateScrollButtons()
{

	m_pScrollButton[TRUE]->bEnabled = m_pScrollButton[TRUE]->pGroupScroll ? m_pScrollButton[TRUE]->pGroupScroll->IsScrollButtonEnabled(TRUE) : FALSE;
	m_pScrollButton[FALSE]->bEnabled = m_pScrollButton[FALSE]->pGroupScroll ? m_pScrollButton[FALSE]->pGroupScroll->IsScrollButtonEnabled(FALSE) : FALSE;
}

void CXTPTaskPanel::OnClickScrollButton(CXTPTaskPanelScrollButton* pScrollButton)
{
	if (!pScrollButton->bEnabled)
		return;

	pScrollButton->bPressed = TRUE;

	SetCapture();

	CXTPTaskPanelGroup* pActiveGroup = m_pActiveGroup;

	int nScrollOffset = (pScrollButton->bScrollUp ? -1 : +1) * pActiveGroup->GetItemsInRow();
	pActiveGroup->Scroll(nScrollOffset);

	Invalidate(FALSE);

	SetTimer(TID_SCROLLGROUP, 500, NULL);
	BOOL bQuick = FALSE;

	while (CWnd::GetCapture() == this && pScrollButton->bEnabled)
	{
		MSG msg;

		if (!GetMessageExt(msg))
			break;

		if (msg.message == WM_TIMER && msg.wParam == TID_SCROLLGROUP)
		{
			pActiveGroup->Scroll(nScrollOffset);
			Invalidate(FALSE);

			if (!bQuick)
			{
				KillTimer(TID_SCROLLGROUP);
				SetTimer(TID_SCROLLGROUP, 50, NULL);
				bQuick = TRUE;
			}
		}
		else if (msg.message == WM_LBUTTONUP) break;
		else if (msg.message == WM_MOUSEMOVE)
		{
		}
		else if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_ESCAPE)
				break;
		}
		else
			DispatchMessage(&msg);

	}
	pScrollButton->bPressed = FALSE;

	ReleaseCapture();
	Invalidate(FALSE);
}


void CXTPTaskPanel::RepositionScrollButtons()
{
	m_pScrollButton[FALSE]->rcButton.SetRectEmpty();
	m_pScrollButton[TRUE]->rcButton.SetRectEmpty();
	m_pScrollButton[TRUE]->pGroupCaption = m_pScrollButton[FALSE]->pGroupCaption = NULL;

	if (IsExplorerBehaviour() || !m_pActiveGroup || !m_pActiveGroup->IsVisible())
		return;

	m_pScrollButton[TRUE]->pGroupScroll = m_pScrollButton[FALSE]->pGroupScroll = m_pActiveGroup;

	if (GetPaintManager()->m_bCaptionScrollButton)
	{
		CXTPClientRect rc(this);
		rc.bottom = max(rc.bottom, m_nClientHeight);
		CRect rcMargins = GetPaintManager()->GetControlMargins(this);

		m_pScrollButton[TRUE]->pGroupCaption = m_pActiveGroup;

		CRect rcCaption = m_pActiveGroup->GetCaptionRect();
		int nRight = rc.right - rcMargins.right - GetPaintManager()->GetGroupOuterMargins(m_pActiveGroup).right;

		int nHeight = rcCaption.Height();
		m_pScrollButton[TRUE]->rcButton.SetRect(nRight - nHeight, rcCaption.top, nRight, rcCaption.bottom);

		CXTPTaskPanelGroup* pGroupNext = GetNextVisibleGroup(m_pActiveGroup->GetIndex());
		if (pGroupNext == 0)
		{
			m_pScrollButton[FALSE]->rcButton.SetRect(nRight - nHeight, rc.bottom - rcMargins.bottom - nHeight,
				nRight, rc.bottom - rcMargins.bottom);
		}
		else
		{
			m_pScrollButton[FALSE]->pGroupCaption = pGroupNext;

			CRect rcNextCaption = pGroupNext->GetCaptionRect();
			int nButtonRight = rc.right - rcMargins.right - GetPaintManager()->GetGroupOuterMargins(pGroupNext).right;

			m_pScrollButton[FALSE]->rcButton.SetRect(nButtonRight - rcNextCaption.Height(), rcNextCaption.top, nButtonRight, rcNextCaption.bottom);
		}
	}
	else
	{
		CRect rc = m_pActiveGroup->GetClientRect();
		rc.DeflateRect(4, 4);
		int nHeight = 16;

		m_pScrollButton[TRUE]->rcButton.SetRect(rc.right - nHeight, rc.top, rc.right, rc.top + nHeight);
		m_pScrollButton[FALSE]->rcButton.SetRect(rc.right - nHeight, rc.bottom - nHeight, rc.right, rc.bottom);

	}

	UpdateScrollButtons();
}

void CXTPTaskPanel::Reposition(BOOL bRecalcOnly)
{
	if (!GetSafeHwnd())
		return;

	if (m_nLockRedraw > 0)
		return;

	CXTPClientRect rc(this);

	CRect rcMargins = GetPaintManager()->GetControlMargins(this);
	int nGroupSpacing = GetPaintManager()->GetGroupSpacing(this);

	int nCurPos = rcMargins.top, nGroupCount = GetGroupCount(), i;
	rc.DeflateRect(rcMargins.left, 0, rcMargins.right, 0);

	{
		CClientDC dc(this);
		for (i = 0; i < nGroupCount; i++)
		{
			CXTPTaskPanelGroup* pGroup = GetAt(i);
			if (!pGroup->IsVisible())
				continue;

			pGroup->OnCalcClientHeight(&dc, rc);
		}
	}


	BOOL bFirstVisbleGroup = TRUE;

	for (i = 0; i < nGroupCount; i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);
		if (!pGroup->IsVisible())
		{
			pGroup->OnAnimate(0);
			continue;
		}

		if (!bFirstVisbleGroup)
		{
			nCurPos += nGroupSpacing;
		}
		else bFirstVisbleGroup = FALSE;

		CRect rcGroup = pGroup->OnReposition(CRect(rc.left, nCurPos, rc.right, nCurPos), bRecalcOnly);

		nCurPos = rcGroup.bottom + GetPaintManager()->GetGroupOuterMargins(pGroup).bottom;
	}
	nCurPos += rcMargins.bottom;

	m_nClientHeight = nCurPos;

	RepositionScrollButtons();

	m_nAnimationStep = 10;

	BOOL bDirty = IsDirty();
	if (m_nAnimateTimer == 0 && bDirty)
	{
		m_nAnimateTimer = SetTimer(TID_REPOSITION, 30, NULL);
	}
	else if (m_nAnimateTimer != 0 && !bDirty)
	{
		KillTimer(m_nAnimateTimer);
		m_nAnimateTimer = 0;
	}

	UpdateScrollBar();

	if (!m_bAnimation)
		AnimateGroups(FALSE);

	Invalidate(FALSE);
}

void CXTPTaskPanel::OnClientHeightChanged()
{

}

void CXTPTaskPanel::UpdateScrollBar()
{
	if (m_bInUpdateScrollBar)
		return;

	m_bInUpdateScrollBar = TRUE;
	if (m_nClientHeight != m_nOldClientHeight)
	{
		m_nOldClientHeight = m_nClientHeight;
		OnClientHeightChanged();
	}


	CXTPClientRect rc(this);
	m_bScrollVisible = rc.Height() < m_nClientHeight;

	ShowScrollBar(SB_VERT, m_bScrollVisible);

	if (m_bScrollVisible)
	{
		SCROLLINFO  si ;

		si.cbSize = sizeof(SCROLLINFO) ;
		si.fMask = SIF_PAGE | SIF_RANGE ;

		GetScrollInfo(SB_VERT, &si, si.fMask);

		UINT nControlHeight = (UINT)rc.Height();

		if (si.nPage != nControlHeight || si.nMax != m_nClientHeight - 1 || si.nMin != 0)
		{
			si.nPage = nControlHeight ;
			si.nMax = m_nClientHeight - 1;
			si.nMin = 0 ;
			si.fMask = SIF_PAGE | SIF_RANGE ;
			SetScrollInfo(SB_VERT, &si) ;
			AnimateGroups(FALSE);
		}
		EnableScrollBarCtrl(SB_VERT, TRUE);
	}

	m_bInUpdateScrollBar = FALSE;
}

void CXTPTaskPanel::OnSize(UINT nType, int cx, int cy)
{
	Reposition(IsExplorerBehaviour());


	CWnd::OnSize(nType, cx, cy);

}

int CXTPTaskPanel::GetScrollOffset() const
{
	if (!m_bScrollVisible)
		return 0;

	return GetScrollPos(SB_VERT);
}

int CXTPTaskPanel::GetClientHeight() const
{
	return m_nClientHeight;
}

void CXTPTaskPanel::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// its horizontal scroll bar
	int nCurPos = GetScrollPos(SB_VERT);

	// decide what to do for each diffrent scroll event
	switch (nSBCode)
	{
		case SB_LEFT: nCurPos = 0; break;
		case SB_RIGHT: nCurPos = GetScrollLimit(SB_VERT); break;
		case SB_LINELEFT: nCurPos = max(nCurPos - 6, 0); break;
		case SB_LINERIGHT: nCurPos = min(nCurPos + 6, GetScrollLimit(SB_VERT)); break;
		case SB_PAGELEFT: nCurPos = max(nCurPos - CXTPClientRect(this).Height(), 0); break;
		case SB_PAGERIGHT: nCurPos = min(nCurPos + CXTPClientRect(this).Height(), GetScrollLimit(SB_VERT)); break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:  nCurPos = nPos; break;
	}

	OnSrollChanged(nCurPos);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CXTPTaskPanel::OnSrollChanged(int nCurPos)
{
	SetScrollPos(SB_VERT, nCurPos);
	AnimateGroups(FALSE);

	Invalidate(FALSE);
}

CXTPTaskPanelScrollButton* CXTPTaskPanel::HitTestScrollButton(CPoint point) const
{
	point.y += GetScrollOffset();

	for (int i = 0; i < 2; i++)
	{
		if (m_pScrollButton[i]->IsVisible() && m_pScrollButton[i]->rcButton.PtInRect(point))
			return m_pScrollButton[i];
	}

	return 0;
}

CXTPTaskPanelGroup* CXTPTaskPanel::HitTestGroup(CPoint point, BOOL* pbCaption) const
{
	if (HitTestScrollButton(point))
		return 0;

	point.y += GetScrollOffset();

	int nGroupCount = GetGroupCount();

	for (int i = 0; i < nGroupCount; i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);
		if (!pGroup->IsVisible())
			continue;

		if (pGroup->GetClientRect().PtInRect(point))
		{
			if (pbCaption) *pbCaption = FALSE;
			return pGroup;
		}
		if (pGroup->IsCaptionVisible() && pGroup->GetCaptionRect().PtInRect(point))
		{
			if (pbCaption) *pbCaption = TRUE;
			return pGroup;
		}

	}
	return NULL;
}

CXTPTaskPanelItem* CXTPTaskPanel::HitTestItem(CPoint point, BOOL bClientAvail) const
{
	BOOL bCaption;
	CXTPTaskPanelGroup* pGroup = HitTestGroup(point, &bCaption);

	if (!pGroup)
		return NULL;

	if (bCaption)
		return pGroup;

	CXTPTaskPanelGroupItem* pItem = pGroup->HitTest(point);
	if (pItem)
		return pItem;

	if (bClientAvail)
		return pGroup;

	return NULL;
}

void CXTPTaskPanel::OnClick(CXTPTaskPanelGroupItem* pItem)
{
	ASSERT(pItem);
	NotifyOwner(XTP_TPN_CLICK, (LPARAM)pItem);
}

void CXTPTaskPanel::OnRButtonDown(UINT nFlags, CPoint point)
{
	CXTPTaskPanelItem* pItem = HitTestItem(point);

	if (pItem && pItem->IsAcceptFocus())
	{
		SetFocusedItem(pItem);
	}

	if (pItem)
	{
		NotifyOwner(XTP_TPN_RCLICK, (LPARAM)pItem);
	}


	CWnd::OnRButtonDown(nFlags, point);
}

void CXTPTaskPanel::OnRButtonUp(UINT nFlags, CPoint point)
{

	CWnd::OnRButtonUp(nFlags, point);
}

void CXTPTaskPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPTaskPanelItem* pItem = HitTestItem(point);

	if (pItem && pItem->IsAcceptFocus())
	{
		SetFocusedItem(pItem);
	}
	else
	{
		GrabFocus();

	}

	if (pItem && pItem->IsAcceptFocus())
	{
		SetCapture();
		m_pItemPressed = pItem;
		m_ptPressed = point;
		Invalidate(FALSE);
	}

	CXTPTaskPanelScrollButton* pScrollButton = HitTestScrollButton(point);

	if (pScrollButton)
	{
		OnClickScrollButton(pScrollButton);
	}


	CWnd::OnLButtonDown(nFlags, point);
}

void CXTPTaskPanel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);

	if (m_pItemHot && m_pItemPressed == m_pItemHot && !m_pItemHot->IsGroup())
	{
		NotifyOwner(XTP_TPN_DBLCLICK, (LPARAM)(CXTPTaskPanelGroupItem*)m_pItemPressed);
	}
}

void CXTPTaskPanel::OnLButtonUp(UINT /*nFlags*/, CPoint point)
{
	UNREFERENCED_PARAMETER(point);

	if (m_pItemPressed == NULL)
	{
		return;
	}

	CXTPTaskPanelItem* pItemPressed = m_pItemPressed;
	m_pItemPressed = NULL;

	ReleaseCapture();
	Invalidate(FALSE);



	if (pItemPressed == m_pItemHot)
	{
		if (IsItemGroup(pItemPressed))
		{
			CXTPTaskPanelGroup* pGroup = (CXTPTaskPanelGroup*)pItemPressed;
			pGroup->SetExpanded(!pGroup->IsExpanded());
		}
		else
		{
			OnClick((CXTPTaskPanelGroupItem*)pItemPressed);
		}
	}
}

void CXTPTaskPanel::OnCaptureChanged(CWnd* pWnd)
{
	if (m_pItemPressed)
	{
		m_pItemPressed = NULL;
		Invalidate(FALSE);
	}

	CWnd::OnCaptureChanged(pWnd);
}

void CXTPTaskPanel::OnStartItemDrag(CXTPTaskPanelGroupItem* pItem)
{
	BOOL bAllowDrag = pItem->IsAllowDrag();
	if (bAllowDrag == xtpTaskItemAllowDragNone)
		return;

	ASSERT(pItem);
	ReleaseCapture();

	BOOL bAllowDragOutsideControl = (bAllowDrag & xtpTaskItemAllowDragMoveOutsideControl) ||
		(bAllowDrag & xtpTaskItemAllowDragCopyOutsideControl);

	if (NotifyOwner(XTP_TPN_STARTDRAG, (LPARAM)pItem) == XTP_ACTION_CANCEL)
		return;

	COleDataSource ds;
	if (!pItem->PrepareDrag(ds, bAllowDragOutsideControl))
		return;

	m_pItemDragging = pItem;

	DROPEFFECT dropEffectMask =
		((bAllowDrag & xtpTaskItemAllowDragCopyWithinGroup) ||
		(bAllowDrag & xtpTaskItemAllowDragCopy) ? DROPEFFECT_COPY : 0) +
		((bAllowDrag & xtpTaskItemAllowDragMoveWithinGroup) ||
		(bAllowDrag & xtpTaskItemAllowDragMove) ? DROPEFFECT_MOVE : 0);

	DROPEFFECT dropEffect = ds.DoDragDrop(dropEffectMask);

	if ((dropEffect == DROPEFFECT_MOVE) && (dropEffectMask & DROPEFFECT_MOVE))
	{
		m_pItemDragging->Remove();
	}

	m_pItemDragOver = m_pItemDragging = NULL;
}

DROPEFFECT CXTPTaskPanel::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropEffectMask = DROPEFFECT_COPY + DROPEFFECT_MOVE;

	BOOL bCaption;
	CXTPTaskPanelGroup* pGroup = HitTestGroup(point, &bCaption);

	CXTPTaskPanelItem* pItemDragOverOld = m_pItemDragOver;

	if (pGroup && pGroup->IsAllowDrop())
	{
		m_pItemDragOver = pGroup;

		if (!bCaption)
		{
			CRect rcItem;
			CXTPTaskPanelItem* pItem = pGroup->HitTest(point, &rcItem);

			if (pItem && pItem->IsAllowDrop())
			{
				if (!GetPaintManager()->m_bInvertDragRect)
				{
					pItem = rcItem.CenterPoint().y > point.y ? pItem : pGroup->GetNextVisibleItem(pItem->GetIndex());
				}

				if (pItem)
				{
					m_pItemDragOver = pItem;
				}
			}
		}
	}
	else
	{
		m_pItemDragOver = NULL;
	}

	DROPEFFECT dropEffect = DROPEFFECT_NONE;

	if (m_pItemDragOver && m_nOLEDropMode)
	{
		dropEffect = DROPEFFECT_MOVE;

		if (dropEffect == DROPEFFECT_NONE)
			m_pItemDragOver = 0;
	}
	else
	{
		if (!pDataObject || !pDataObject->IsDataAvailable(GetClipboardFormat()))
			m_pItemDragOver = NULL;

		if (pDataObject && !pDataObject->IsDataAvailable(CF_TEXT) && (m_pItemDragging == 0))
			m_pItemDragOver = NULL;


		if (m_pItemDragging)
		{
			BOOL bAllowDrag = m_pItemDragging->IsAllowDrag();

			if (((bAllowDrag & xtpTaskItemAllowDragMoveWithinGroup) == 0) &&
				((bAllowDrag & xtpTaskItemAllowDragMoveWithinControl) == 0))
				dropEffectMask &= ~DROPEFFECT_MOVE;

			if (((bAllowDrag & xtpTaskItemAllowDragCopyWithinGroup) == 0) &&
				((bAllowDrag & xtpTaskItemAllowDragCopyWithinControl) == 0))
				dropEffectMask &= ~DROPEFFECT_COPY;

			if (m_pItemDragOver && m_pItemDragging->GetItemGroup() != pGroup)
			{
				if (bAllowDrag & xtpTaskItemAllowDragMoveWithinGroup)
					dropEffectMask &= ~DROPEFFECT_MOVE;

				if (bAllowDrag & xtpTaskItemAllowDragCopyWithinGroup)
					dropEffectMask &= ~DROPEFFECT_COPY;
			}

			if (m_pItemDragOver)
			{
				if (((bAllowDrag & xtpTaskItemAllowDragMoveWithinControl) == 0) &&
					((bAllowDrag & xtpTaskItemAllowDragMoveWithinGroup) == 0))
					dropEffectMask &= ~DROPEFFECT_MOVE;

				if (((bAllowDrag & xtpTaskItemAllowDragCopyWithinControl) == 0) &&
					((bAllowDrag & xtpTaskItemAllowDragCopyWithinGroup) == 0))
					dropEffectMask &= ~DROPEFFECT_COPY;
			}

			if (dropEffectMask == 0)
				m_pItemDragOver = 0;
		}
	}


	if (pItemDragOverOld != m_pItemDragOver)
	{
		Invalidate(FALSE);
	}

	BOOL bGroupHover = IsItemGroup(m_pItemDragOver) && !((CXTPTaskPanelGroup*)m_pItemDragOver)->IsExpanded();

	if (bGroupHover && !m_bTimerGroupHover)
	{
		UINT nHoverTime = 500;
		SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &nHoverTime, 0);

		SetTimer(TID_GROUPHOVER, nHoverTime, NULL);
		m_bTimerGroupHover = TRUE;
	}
	if (!bGroupHover && m_bTimerGroupHover)
	{
		KillTimer(TID_GROUPHOVER);
		m_bTimerGroupHover = FALSE;
	}


	if (!m_pItemDragOver)
		return DROPEFFECT_NONE;

	if (dropEffect != DROPEFFECT_NONE)
		return dropEffect;

	if (dropEffectMask & DROPEFFECT_COPY)
	{
		if (((dwKeyState & MK_CONTROL) == MK_CONTROL) || ((dropEffectMask & DROPEFFECT_MOVE) == 0))
			return DROPEFFECT_COPY;
	}

	return dropEffectMask & DROPEFFECT_MOVE ? DROPEFFECT_MOVE : DROPEFFECT_NONE;
}

BOOL CXTPTaskPanel::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	point;

	if (!m_pItemDragOver)
		return FALSE;

	CXTPTaskPanelItem* pItemDragOver = m_pItemDragOver;
	m_pItemDragOver = NULL;

	Invalidate(FALSE);

	if (dropEffect != DROPEFFECT_COPY && dropEffect != DROPEFFECT_MOVE)
		return FALSE;

	if (m_nOLEDropMode && pDataObject)
	{

		return dropEffect != DROPEFFECT_NONE;
	}

	if (!pDataObject || !pDataObject->IsDataAvailable(GetClipboardFormat()))
		return FALSE;

	CXTPTaskPanelItem* pItemDrop = CXTPTaskPanelItem::CreateFromOleData(pDataObject);

	if (!pItemDrop)
		return FALSE;

	if (IsItemGroup(pItemDragOver))
	{
		CXTPTaskPanelItems* pItems = ((CXTPTaskPanelGroup*)pItemDragOver)->GetItems();
		pItems->InsertAt(pItems->GetCount(), pItemDrop);
	}
	else
	{
		CXTPTaskPanelItems* pItems = pItemDragOver->GetParentItems();

		int nIndex = pItemDragOver->GetIndex();

		if (GetPaintManager()->m_bInvertDragRect)
		{
			nIndex++;

			if (m_pItemDragging && m_pItemDragging->GetParentItems() == pItems)
			{
				if (m_pItemDragging->GetIndex() >= nIndex)
					nIndex--;
			}
		}

		pItems->InsertAt(nIndex, pItemDrop);
	}

	if (pItemDrop->IsAcceptFocus())
	{
		SetFocusedItem(pItemDrop);
	}

	NotifyOwner(XTP_TPN_ITEMDROP, (LPARAM)pItemDrop);
	return TRUE;
}

void CXTPTaskPanel::OnStartGroupDrag(CXTPTaskPanelItem* pItem)
{
	if (!pItem->IsAllowDrag())
		return;

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));

	m_pItemDragging = pItem;

	Invalidate(FALSE);

	while (CWnd::GetCapture() == this)
	{
		MSG msg;

		if (!GetMessageExt(msg))
			break;

		if (msg.message == WM_LBUTTONUP)
			break;
		else if (msg.message == WM_MOUSEMOVE)
		{
			CPoint pt(msg.pt);
			ScreenToClient(&pt);

			BOOL bCaption;
			CXTPTaskPanelGroup* pHTGroup = HitTestGroup(pt, &bCaption);

			if (pHTGroup && bCaption && pHTGroup != pItem)
			{
				m_pGroups->Move(pItem, pHTGroup->GetIndex());
			}
		}
		else if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_ESCAPE)
			{
				break;
			}
		}
		else
			DispatchMessage(&msg);

	}
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	m_pItemDragging = NULL;

	ReleaseCapture();
}

void CXTPTaskPanel::CheckScrollButtonMouseOver(CPoint point)
{
	CXTPTaskPanelScrollButton* pButtonHot = HitTestScrollButton(point);

	for (int i = 0; i < 2; i++)
	{
		BOOL bHot = (pButtonHot == m_pScrollButton[i]);

		if ((m_pScrollButton[i]->bHot && !bHot) || (!m_pScrollButton[i]->bHot && bHot))
		{
			m_pScrollButton[i]->bHot = bHot;
			Invalidate(FALSE);

			if (bHot)
			{
				TRACKMOUSEEVENT tme =
				{
					sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd
				};
				_TrackMouseEvent(&tme);
			}
		}
	}
}

void CXTPTaskPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);


#if !defined(_XTP_ACTIVEX) && defined(_XTP_INCLUDE_COMMANDBARS)
	if (XTPMouseManager()->IsTrackedLock(NULL) || XTPMouseManager()->IsMouseLocked())
		return;
#endif

	CXTPTaskPanelItem* pItem = HitTestItem(point);

	BOOL bAccept = pItem && pItem->IsAcceptFocus() && CXTPDrawHelpers::IsTopParentActive(m_hWnd) &&
		((m_pItemPressed == NULL) || (m_pItemPressed == pItem));

	SetHotItem(bAccept ? pItem : NULL);

	if (m_pItemPressed && (abs(m_ptPressed.x - point.x) + abs(m_ptPressed.y - point.y) > 4))
	{
		if (IsItemGroup(m_pItemPressed)) OnStartGroupDrag(m_pItemPressed);
			else OnStartItemDrag((CXTPTaskPanelGroupItem*)m_pItemPressed);
	}

	CheckScrollButtonMouseOver(point);
}


void CXTPTaskPanel::OnMouseLeave()
{
	SetHotItem(NULL);
	CheckScrollButtonMouseOver(CPoint(-1, -1));
}

BOOL CXTPTaskPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_pItemHot)
	{
		::SetCursor(m_hHandCursor);
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


void CXTPTaskPanel::OnDestroy()
{
	CWnd::OnDestroy();
}

BOOL CXTPTaskPanel::AnimateGroups(BOOL bCheckDirty)
{
	BOOL bDirty = FALSE;

	for (int i = 0; i < GetGroupCount(); i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);
		if (!bCheckDirty || pGroup->IsDirty())
		{
			bDirty = TRUE;
			pGroup->OnAnimate(m_nAnimationStep);
		}
	}
	RepositionScrollButtons();

	if (DYNAMIC_DOWNCAST(CXTPTaskPanelEditItem, GetFocus()) != 0)
		GrabFocus();

	return bDirty;
}

void CXTPTaskPanel::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case TID_REPOSITION:
			{
				BOOL bDirty = AnimateGroups();

				Invalidate(FALSE);

				m_nAnimationStep--;

				if (!bDirty)
				{
					KillTimer(m_nAnimateTimer);
					m_nAnimateTimer = 0;
				}
			}
			break;

		case TID_GROUPHOVER:
			if (IsItemGroup(m_pItemDragOver) && m_bTimerGroupHover)
			{
				((CXTPTaskPanelGroup*)m_pItemDragOver)->SetExpanded(TRUE);
			}
			m_bTimerGroupHover = FALSE;
			KillTimer(TID_GROUPHOVER);
			break;
	}

	CWnd::OnTimer(nIDEvent);
}

void CXTPTaskPanel::OnSysColorChange()
{
	m_pPaintManager->RefreshMetrics();
	Reposition();


	CWnd::OnSysColorChange();
}

#ifndef SPI_GETMENUANIMATION
#define SPI_GETMENUANIMATION 0x1002
#endif

void CXTPTaskPanel::SetAnimation(XTPTaskPanelAnimation panelAnimation)
{
	m_eAnimation = panelAnimation;

	if (m_eAnimation == xtpTaskPanelAnimationYes) m_bAnimation = TRUE;
	else if (m_eAnimation == xtpTaskPanelAnimationNo) m_bAnimation = FALSE;
	else
	{
		m_bAnimation = FALSE;
		::SystemParametersInfo(SPI_GETMENUANIMATION, 0, &m_bAnimation, 0);
	}
}

void CXTPTaskPanel::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CWnd::OnSettingChange(uFlags, lpszSection);

	if (m_eAnimation == xtpTaskPanelAnimationSystem)
		SetAnimation(xtpTaskPanelAnimationSystem);

	m_pPaintManager->RefreshMetrics();
	Reposition();
}


CXTPTaskPanelItem* CXTPTaskPanel::GetFocusedItem() const
{
	return m_pItemFocused;
}

BOOL CXTPTaskPanel::IsDrawFocusRect() const
{
	return m_bDrawFocusRect && (::GetFocus() == m_hWnd);
}


HWND CXTPTaskPanel::GetNextControl(BOOL bForward) const
{
	HWND hWnd = m_hWnd;
	HWND hWndParent = ::GetParent(hWnd);

	if (!hWnd || !hWndParent)
	{
		ASSERT(FALSE);
		return 0;
	}
	if ((GetWindowLong(hWndParent, GWL_EXSTYLE) & WS_EX_CONTROLPARENT) &&
		(GetWindowLong(hWndParent, GWL_STYLE) & WS_CHILD))
	{
		hWndParent = ::GetParent(hWndParent);
	}

	HWND hwndNextItem = ::GetNextDlgTabItem(hWndParent, hWnd, bForward);

	if (hwndNextItem == hWnd)
		return 0;

	return hwndNextItem;
}

void CXTPTaskPanel::CheckOldFocus(CWnd* pOldWnd)
{

	if (!m_bGrabbingFocus && pOldWnd)
	{
		BOOL bShiftPressed = GetKeyState(VK_SHIFT) < 0;

		if (pOldWnd->m_hWnd == GetNextControl(!bShiftPressed))
		{
			CXTPTaskPanelItem* pItem = GetNextItem(0, bShiftPressed ? -1 : 1, !m_bTabItems);
			if (pItem)
			{
				SetFocusedItem(pItem, TRUE);
			}
		}
	}
}

void CXTPTaskPanel::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	CheckOldFocus(pOldWnd);


	Invalidate(FALSE);


}

void CXTPTaskPanel::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	m_bDrawFocusRect = FALSE;

	Invalidate(FALSE);

}

CXTPTaskPanelGroup* CXTPTaskPanel::GetNextVisibleGroup(int nIndex, int nDirection) const
{
	return GetAt(m_pGroups->GetNextVisibleIndex(nIndex, nDirection));
}

CXTPTaskPanelItem* CXTPTaskPanel::GetNextItem(CXTPTaskPanelItem* pItem, int nDirection, BOOL bTab, BOOL bSkipRowItems) const
{
	if (GetGroupCount() == 0)
		return NULL;

	CXTPTaskPanelGroup* pGroupCheck = NULL;

	do
	{
		if (nDirection > 0)
		{
			if (pItem)
			{
				pGroupCheck = pItem->GetItemGroup();

				if (IsItemGroup(pItem))
					pItem = pGroupCheck->GetItems()->GetFirstVisibleItem();
				else
				{
					if (bTab) pItem = NULL; else pItem = pGroupCheck->GetNextVisibleItem(pItem->GetIndex(), bSkipRowItems ? pGroupCheck->GetItemsInRow() : 1);
				}
			}

			if (!pItem)
			{
				pItem = GetNextVisibleGroup(pGroupCheck ? pGroupCheck->GetIndex() : -1);
			}
		}
		else
		{
			if (!pItem && !pGroupCheck)
			{
				pGroupCheck = GetNextVisibleGroup(GetGroupCount(), - 1);
				pItem = pGroupCheck ? pGroupCheck->GetItems()->GetLastVisibleItem() : 0;
			}
			else if (IsItemGroup(pItem))
			{
				pGroupCheck = GetNextVisibleGroup(pItem->GetIndex(), - 1);
				pItem = pGroupCheck ? pGroupCheck->GetItems()->GetLastVisibleItem() : NULL;
			}
			else if (pItem)
			{
				pGroupCheck = pItem->GetItemGroup();
				if (bTab) pItem = NULL; else pItem = pGroupCheck->GetNextVisibleItem(pItem->GetIndex(), -(bSkipRowItems ? pGroupCheck->GetItemsInRow() : 1));

			}

			if (!pItem) pItem = pGroupCheck;
		}

		if (!(IsItemLink(pItem) && pGroupCheck && !pGroupCheck->IsExpanded()))
		if (pItem && pItem->IsAcceptFocus())
			return pItem;


	}
	while (pItem);


	return NULL;
}

void CXTPTaskPanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);


	CXTPDrawHelpers::KeyToLayout(this, nChar);

	switch (nChar)
	{
	case VK_HOME:
	case VK_END:
		if (m_pItemFocused)
		{
			CXTPTaskPanelItems* pItems = m_pItemFocused->GetParentItems();
			CXTPTaskPanelItem* pItem = nChar == VK_HOME ? pItems->GetFirstVisibleItem() : pItems->GetLastVisibleItem();
			if (pItem && pItem->IsAcceptFocus()) SetFocusedItem(pItem, TRUE);
		}
		break;

	case VK_DOWN:
	case VK_UP:
	case VK_RIGHT:
	case VK_LEFT:
		{
			BOOL bSkipRowItems = (nChar == VK_DOWN || nChar == VK_UP);
			int nDirection = (nChar == VK_DOWN || nChar == VK_RIGHT ? +1 : -1);

			if (!IsExplorerBehaviour() && GetKeyState(VK_CONTROL) < 0 && m_pActiveGroup)
			{
				CXTPTaskPanelGroup* pActiveGroup = GetNextVisibleGroup(m_pActiveGroup->GetIndex(), nDirection);
				if (pActiveGroup && pActiveGroup->IsAcceptFocus())
				{
					pActiveGroup->SetExpanded(TRUE);
					SetFocusedItem(pActiveGroup);
				}
				return;
			}

			CXTPTaskPanelItem* pItem = GetNextItem(m_pItemFocused, nDirection, FALSE, bSkipRowItems);
			if (pItem) SetFocusedItem(pItem, TRUE);
		}
		break;

	case VK_TAB:
		{
			BOOL bShiftPressed = GetKeyState(VK_SHIFT) < 0;

			CXTPTaskPanelItem* pItem = GetNextItem(m_pItemFocused, bShiftPressed ? -1 : +1, !m_bTabItems);
			if (pItem)
			{
				SetFocusedItem(pItem, TRUE);
			}
			else
			{
				HWND hWnd = GetNextControl(bShiftPressed);

				if (hWnd)
				{
					::SetFocus(hWnd);
				}
			}
		}
		break;

	case VK_RETURN:
	case VK_SPACE:
		if (m_pItemFocused && m_pItemFocused->IsAcceptFocus())
		{
			if (IsItemGroup(m_pItemFocused))
			{
				((CXTPTaskPanelGroup*)m_pItemFocused)->SetExpanded(!((CXTPTaskPanelGroup*)m_pItemFocused)->IsExpanded());
			}
			else if (IsItemLink(m_pItemFocused))
			{
				OnClick((CXTPTaskPanelGroupItem*)m_pItemFocused);
			}

		}
		break;
	}

}

void CXTPTaskPanel::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CXTPTaskPanel::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPTaskPanel::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CXTPTaskPanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_bScrollVisible)
	{
		// its horizontal scroll bar
		int nCurPos = GetScrollPos(SB_VERT);

		if (zDelta > 0) nCurPos = max(nCurPos - 10, 0);
		else nCurPos = min(nCurPos + 10, GetScrollLimit(SB_VERT));

		OnSrollChanged(nCurPos);

		Invalidate(FALSE);
	}
	else if (GetBehaviour() != xtpTaskPanelBehaviourExplorer && m_pActiveGroup)
	{
		BOOL bScrollButtonUp = zDelta > 0;

		for (int i = 0; i < 3; i++)
		{
			if (!m_pActiveGroup->IsScrollButtonEnabled(bScrollButtonUp))
				break;

			int nScrollOffset = (bScrollButtonUp ? -1 : +1) * m_pActiveGroup->GetItemsInRow();
			m_pActiveGroup->Scroll(nScrollOffset);
		}
		Invalidate(FALSE);
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

INT_PTR CXTPTaskPanel::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{

	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CControlBar
	INT_PTR nHit = CWnd::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

#if !defined(_XTP_ACTIVEX) && defined(_XTP_INCLUDE_COMMANDBARS)
	if (XTPMouseManager()->IsTrackedLock(NULL) || XTPMouseManager()->IsMouseLocked())
		return -1;
#endif

	if (m_pItemHot)
	{
			nHit = m_pItemHot->GetID();

			CString strTip = m_pItemHot->GetTooltip();
			if (strTip.GetLength() == 0)
				return -1;

			CXTPToolTipContext::FillInToolInfo(pTI, m_hWnd, m_pItemHot->GetHitTestRect(),
				nHit, strTip, m_pItemHot->GetCaption(), strTip, GetImageManager());

			return nHit;
	}
	return -1;
}

UINT CXTPTaskPanel::OnGetDlgCode()
{
	return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTCHARS;
}

LRESULT CXTPTaskPanel::NotifyOwner(WPARAM wParam, LPARAM lParam)
{
	if (!m_hWnd)
		return 0;

	return GetOwner()->SendMessage(XTPWM_TASKPANEL_NOTIFY, wParam, lParam);
}

void CXTPTaskPanel::ExpandGroup(CXTPTaskPanelGroup* pGroup, BOOL bExpanded)
{
	ASSERT(pGroup);

	if (IsExplorerBehaviour())
	{
		XTP_TPNGROUPEXPANDING NMGROUPEXPANDING;
		NMGROUPEXPANDING.bExpanding = bExpanded;
		NMGROUPEXPANDING.pGroup = pGroup;

		if (NotifyOwner(XTP_TPN_GROUPEXPANDING, (LPARAM)&NMGROUPEXPANDING) == XTP_ACTION_CANCEL)
			return;

		pGroup->m_bExpanded = bExpanded;
		pGroup->m_bExpanding = TRUE;
	}
	else if (bExpanded)
	{
		XTP_TPNGROUPEXPANDING NMGROUPEXPANDING;
		NMGROUPEXPANDING.bExpanding = TRUE;
		NMGROUPEXPANDING.pGroup = pGroup;

		if (NotifyOwner(XTP_TPN_GROUPEXPANDING, (LPARAM)&NMGROUPEXPANDING) == XTP_ACTION_CANCEL)
			return;

		if (m_pActiveGroup)
		{
			if (m_pActiveGroup->IsExpandable())
			{
				m_pActiveGroup->m_bExpanded = FALSE;
				m_pActiveGroup->m_bExpanding = TRUE;
			}
			m_pActiveGroup->InternalRelease();
		}

		m_pActiveGroup = pGroup;
		m_pActiveGroup->InternalAddRef();
		m_pActiveGroup->m_bExpanding = TRUE;
		m_pActiveGroup->m_bExpanded = TRUE;
	}

	Reposition();
}

BOOL CXTPTaskPanel::IsGroupExpanded(const CXTPTaskPanelGroup* pGroup) const
{
	return IsExplorerBehaviour() ? pGroup->m_bExpanded :
		(m_pActiveGroup == pGroup) || (pGroup->m_bExpanded && !pGroup->m_bExpandable);
}

XTPTaskPanelHotTrackStyle CXTPTaskPanel::GetHotTrackStyle() const
{
	if (m_hotTrackStyle != xtpTaskPanelHighlightDefault)
		return m_hotTrackStyle;

	if (m_panelBehaviour == xtpTaskPanelBehaviourExplorer)
		return xtpTaskPanelHighlightText;

	if (m_panelBehaviour == xtpTaskPanelBehaviourToolbox)
		return xtpTaskPanelHighlightItem;

	return xtpTaskPanelHighlightImage;
}

void CXTPTaskPanel::SetHotTrackStyle(XTPTaskPanelHotTrackStyle hotTrackStyle)
{
	m_hotTrackStyle = hotTrackStyle;
	Reposition();
}

XTPTaskPanelItemLayout CXTPTaskPanel::GetItemLayout() const
{
	if (m_itemLayout != xtpTaskItemLayoutDefault)
		return m_itemLayout;

	if (m_panelBehaviour == xtpTaskPanelBehaviourList)
		return xtpTaskItemLayoutImagesWithTextBelow;

	return xtpTaskItemLayoutImagesWithText;
}

void CXTPTaskPanel::SetItemLayout(XTPTaskPanelItemLayout itemLayout)
{
	m_itemLayout = itemLayout;
	Reposition();
}

void CXTPTaskPanel::SetBehaviour(XTPTaskPanelBehaviour panelBehaviour)
{
	m_panelBehaviour = panelBehaviour;
	Reposition();
}

XTPTaskPanelBehaviour CXTPTaskPanel::GetBehaviour() const
{
	return m_panelBehaviour;
}

void CXTPTaskPanel::OnItemRemoved(CXTPTaskPanelItem* pItem)
{
	ASSERT(pItem);

	if (m_pActiveGroup == pItem)
	{
		m_pActiveGroup->InternalRelease();
		m_pActiveGroup = NULL;
	}

	if (m_pItemHot == pItem)
	{
		m_pItemHot->InternalRelease();
		m_pItemHot = NULL;
	}

	if (m_pItemFocused == pItem)
	{
		m_pItemFocused->InternalRelease();
		m_pItemFocused = NULL;
	}


	if (m_pScrollButton[TRUE]->pGroupCaption == pItem)
		m_pScrollButton[TRUE]->pGroupCaption = NULL;

	if (m_pScrollButton[TRUE]->pGroupScroll == pItem)
		m_pScrollButton[TRUE]->pGroupScroll = NULL;

	if (m_pScrollButton[FALSE]->pGroupCaption == pItem)
		m_pScrollButton[FALSE]->pGroupCaption = NULL;

	if (m_pScrollButton[FALSE]->pGroupScroll == pItem)
		m_pScrollButton[FALSE]->pGroupScroll = NULL;

	NotifyOwner(XTP_TPN_ITEMREMOVED, (LPARAM)pItem);
}


BOOL CXTPTaskPanel::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (m_pToolTipContext)
	{
		m_pToolTipContext->FilterToolTipMessage(this, message, wParam, lParam);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

HWND CXTPTaskPanel::GetRootChild(HWND hWndTaskPanel, HWND hWnd)
{
	HWND hwndParent = ::GetParent(hWnd);

	while (hwndParent != hWndTaskPanel)
	{
		hWnd = hwndParent;
		hwndParent = ::GetParent(hWnd);

		if (hwndParent == NULL)
			return NULL;
	}

	return hWnd;
}

CXTPTaskPanelGroupItem* CXTPTaskPanel::FindControlItem(HWND hWnd)
{
	for (int i = 0; i < GetGroupCount(); i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);

		for (int j = 0; j < pGroup->GetItemCount(); j++)
		{
			if (pGroup->GetAt(j)->GetControlHandle() == hWnd)
				return pGroup->GetAt(j);
		}
	}

	return NULL;
}

BOOL CXTPTaskPanel::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && m_bTabItems)
	{
		HWND hWnd = ::GetFocus();

		if (hWnd && hWnd != m_hWnd && ::IsChild(m_hWnd, hWnd))
		{
			HWND hWndDlg = GetRootChild(m_hWnd, hWnd);
			if (!hWndDlg)
				return CWnd::PreTranslateMessage(pMsg);

			BOOL bShiftPressed = GetKeyState(VK_SHIFT) < 0;

			CXTPTaskPanelGroupItem* pItemFocused = FindControlItem(hWndDlg);
			if (pItemFocused)
			{
				CXTPTaskPanelItem* pItem = GetNextItem(pItemFocused, bShiftPressed ? -1 : +1, !m_bTabItems);
				if (pItem)
				{
					SetFocusedItem(pItem, TRUE);
				}
				else
				{
					HWND hWndNext = GetNextControl(bShiftPressed);

					if (hWndNext)
					{
						::SetFocus(hWndNext);
					}
				}
				return TRUE;
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

CXTPToolTipContext* CXTPTaskPanel::GetToolTipContext() const
{
	return m_pToolTipContext;
}

void CXTPTaskPanel::SetLayoutRTL(BOOL bRightToLeft)
{
	if (!XTPSystemVersion()->IsLayoutRTLSupported())
		return;

	if (!m_hWnd)
		return;

	ModifyStyleEx(bRightToLeft ? 0 : WS_EX_LAYOUTRTL, !bRightToLeft ? 0 : WS_EX_LAYOUTRTL);
	GetImageManager()->DrawReverted(bRightToLeft);

	Reposition();
}


LRESULT CXTPTaskPanel::OnGetObject(WPARAM wParam, LPARAM lParam)
{
	if (lParam != OBJID_CLIENT)
		return (LRESULT)Default();

	LPUNKNOWN lpUnknown = GetInterface(&IID_IAccessible);
	if (!lpUnknown)
		return E_FAIL;

	return LresultFromObject(IID_IAccessible, wParam, lpUnknown);
}

BEGIN_INTERFACE_MAP(CXTPTaskPanel, CCmdTarget)
	INTERFACE_PART(CXTPTaskPanel, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()

CCmdTarget* CXTPTaskPanel::GetAccessible()
{
	return this;
}

HRESULT CXTPTaskPanel::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	*ppdispParent = NULL;

	if (GetSafeHwnd())
	{
		return AccessibleObjectFromWindow(GetSafeHwnd(), OBJID_WINDOW, IID_IDispatch, (void**)ppdispParent);
	}
	return E_FAIL;
}

int CXTPTaskPanel::GetAccessibleIndex(CXTPTaskPanelItem* pItem)
{
	int nCount = 0;
	for (int i = 0; i < GetGroupCount(); i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);
		nCount++;

		if (pItem)
		{
			if (pGroup == pItem)
				return nCount;

			if (pItem->GetItemGroup() == pGroup)
				return nCount + pItem->GetIndex() + 1;
		}

		nCount += pGroup->GetItemCount();
	}

	return nCount;
}

CXTPTaskPanelItem* CXTPTaskPanel::GetAccessibleItem(int nIndex)
{
	for (int i = 0; i < GetGroupCount(); i++)
	{
		CXTPTaskPanelGroup* pGroup = GetAt(i);
		nIndex--;

		if (nIndex == 0)
			return pGroup;

		if (nIndex <= pGroup->GetItemCount())
			return pGroup->GetAt(nIndex - 1);

		nIndex -= pGroup->GetItemCount();
	}

	return NULL;
}

HRESULT CXTPTaskPanel::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
	{
		return E_INVALIDARG;
	}

	*pChildCount = GetAccessibleIndex(NULL);
	return S_OK;
}

HRESULT CXTPTaskPanel::GetAccessibleChild(VARIANT varChild, IDispatch* FAR* ppdispChild)
{
	*ppdispChild = NULL;
	int nChild = GetChildIndex(&varChild);

	if (nChild <= 0)
	{
		return E_INVALIDARG;
	}

	CXTPTaskPanelItem* pItem = GetAccessibleItem(nChild);
	if (!pItem)
	{
		return E_INVALIDARG;
	}

	*ppdispChild = pItem->GetIDispatch(TRUE);
	return S_OK;
}

HRESULT CXTPTaskPanel::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF || nChild == -1)
	{
		CString strCaption;
		GetWindowText(strCaption);
		*pszName = strCaption.AllocSysString();
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPTaskPanel::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_OUTLINE;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPTaskPanel::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
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


HRESULT CXTPTaskPanel::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	*pxLeft = *pyTop = *pcxWidth = *pcyHeight = 0;

	if (!GetSafeHwnd())
		return S_OK;

	CRect rc;
	GetWindowRect(&rc);

	int nChild = GetChildIndex(&varChild);

	if (nChild > 0)
	{
		CXTPTaskPanelItem* pItem = GetAccessibleItem(nChild);
		if (pItem)
		{
			CRect rcItem = pItem->GetHitTestRect();
			rc = CRect(CPoint(rc.left + rcItem.left, rc.top + rcItem.top), rcItem.Size());
		}

	}

	*pxLeft = rc.left;
	*pyTop = rc.top;
	*pcxWidth = rc.Width();
	*pcyHeight = rc.Height();

	return S_OK;
}

HRESULT CXTPTaskPanel::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	if (pvarID == NULL)
		return E_INVALIDARG;

	pvarID->vt = VT_EMPTY;

	if (!GetSafeHwnd())
		return S_FALSE;

	if (!CXTPWindowRect(this).PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	pvarID->vt = VT_I4;
	pvarID->lVal = 0;

	CPoint pt(xLeft, yTop);
	ScreenToClient(&pt);

	CXTPTaskPanelItem* pItem = HitTestItem(pt);

	if (pItem)
	{
		pvarID->vt = VT_DISPATCH;
		pvarID->pdispVal = pItem->GetIDispatch(TRUE);
	}

	return S_OK;
}
