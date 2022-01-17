// XTPDockingPaneManager.cpp : implementation of the CXTPDockingPaneManager class.
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
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPToolTipContext.h"
#include "Common/XTPPropExchange.h"
#include "TabManager/XTPTabManager.h"

#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneLayout.h"
#include "XTPDockingPaneBase.h"
#include "XTPDockingPane.h"
#include "XTPDockingPaneSplitterContainer.h"
#include "XTPDockingPaneTabbedContainer.h"
#include "XTPDockingPaneMiniWnd.h"
#include "XTPDockingPanePaintManager.h"
#include "XTPDockingPaneLayout.h"
#include "XTPDockingPaneAutoHidePanel.h"
#include "XTPDockingPaneContext.h"
#include "XTPDockingPaneKeyboardHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CXTPDockingPaneManager::CXTPDockingPaneManager()
{
	m_pSite = 0;
	m_pLayout = 0;

	m_nSplitterGap = 22;
	m_nClientMargin = 0;

	m_dwDefaultPaneOptions = 0;

	m_bAttachingPane = FALSE;

	m_bHideClient = FALSE;
	m_bUseSplitterTracker = TRUE;

	m_pPaintManager = NULL;
	SetTheme(xtpPaneThemeDefault);

	m_pImageManager = new CXTPImageManager();

	m_bLockSplitters = FALSE;

	m_bThemedFloatingFrames = FALSE;

	m_pDockingContext = new CXTPDockingPaneContext();
	m_pDockingContext->m_pManager = this;

	m_bAlphaDockingContext = FALSE;
	m_bShowDockingContextStickers = FALSE;
	m_nDockingContextStickerStyle = xtpPaneStickerStyleWhidbey;

	m_bCloseGroupOnButtonClick = FALSE;
	m_bHideGroupOnButtonClick = TRUE;
	m_pPreviewLayout = 0;
	m_bSyncActiveFloatingFrames = TRUE;

	m_captionDirection = xtpPaneCaptionHorizontal;

	m_pToolTipContext = new CXTPToolTipContext();

	m_pActivePane = 0;
	m_bShowMaximizeButton = FALSE;
	m_bLayoutCreated = FALSE;

	m_bShowContentsWhileDragging = FALSE;

	m_bShowSizeCursorWhileDragging = FALSE;
	m_bInitialUpdateCalled = FALSE;

	m_ptMinClientSize = CPoint(60, 60);

	m_hwndLastFocus = NULL;

	m_bStickyFloatingFrames = FALSE;
	m_nStickyGap = 15;
	m_nFloatingFramesOpacity = 255;

	m_bKeyboardEnabled = xtpPaneKeyboardUnused;

	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (hLib)
	{
		m_pfnSetLayeredWindowAttributes = (PFNSETLAYEREDWINDOWATTRIBUTES)::GetProcAddress(hLib, "SetLayeredWindowAttributes");
	}

	XTPDrawHelpers()->RegisterWndClass(0, _T("XTPDockingPaneSplitter"), 0);
	XTPDrawHelpers()->RegisterWndClass(0, _T("XTPDockingPaneTabbedContainer"), CS_DBLCLKS);
	XTPDrawHelpers()->RegisterWndClass(0, _T("XTPDockingPaneManager"), 0);
	XTPDrawHelpers()->RegisterWndClass(0, _T("XTPDockingPaneAutoHidePanel"), 0);
	XTPDrawHelpers()->RegisterWndClass(0, _T("XTPDockingPaneMiniWnd"), CS_DBLCLKS);

	m_pWindowSelectClass = RUNTIME_CLASS(CXTPDockingPaneWindowSelect);

	OleInitialize(NULL);

	EnableAutomation();
}

CXTPDockingPaneManager::~CXTPDockingPaneManager()
{
	CMDTARGET_RELEASE(m_pLayout);

	CMDTARGET_RELEASE(m_pActivePane);

	CMDTARGET_RELEASE(m_pPaintManager);

	CMDTARGET_RELEASE(m_pToolTipContext);

	delete m_pDockingContext;

	CMDTARGET_RELEASE(m_pImageManager);

	OleUninitialize();
}

void CXTPDockingPaneManager::HideClient(BOOL bHide)
{
	RecalcFrameLayout(NULL);

	m_bHideClient = bHide;
	m_pLayout->m_pTopContainer->OnParentContainerChanged(m_pLayout->m_pTopContainer);

	RedrawPanes();
}

BEGIN_MESSAGE_MAP(CXTPDockingPaneManager, CWnd)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE_VOID(WM_INITIALUPDATE, OnInitialUpdate)
	ON_MESSAGE(WM_GETOBJECT, OnGetObject)
END_MESSAGE_MAP()

void CXTPDockingPaneManager::OnSysColorChange()
{
	RefreshXtremeColors();

	m_pPaintManager->RefreshMetrics();


	_Redraw();
}

void CXTPDockingPaneManager::OnSettingChange(UINT /*uFlags*/, LPCTSTR /*lpszSection*/)
{
	m_pPaintManager->RefreshMetrics();
	_Redraw();
}

void CXTPDockingPaneManager::SetFloatingFramesOpacity(int nOpacity)
{
	m_nFloatingFramesOpacity = nOpacity;

	POSITION pos = GetPaneStack().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = GetPaneStack().GetNext(pos);

		if (pPane->GetType() == xtpPaneTypeMiniWnd)
		{
			((CXTPDockingPaneMiniWnd*)pPane)->UpdateWindowOpacity();
		}
	}
}

void CXTPDockingPaneManager::UpdatePanes()
{
	HWND hWndFocus = ::GetFocus();

	if (m_hwndLastFocus == hWndFocus)
		return;

	m_hwndLastFocus = hWndFocus;

	POSITION pos = GetPaneStack().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = GetPaneStack().GetNext(pos);

		if (pPane->GetType() == xtpPaneTypeTabbedContainer || pPane->GetType() == xtpPaneTypeMiniWnd)
		{
			pPane->OnFocusChanged();
		}
	}
}

void CXTPDockingPaneManager::OnInitialUpdate()
{
	if (m_pSite && m_pSite->IsFrameWnd() && ((CFrameWnd*)m_pSite)->m_nIdleFlags & CFrameWnd::idleLayout)
	{
		RecalcFrameLayout(NULL);
	}

	OnIdleUpdateCmdUI(TRUE, 0L);


	if (m_pLayout)
	{
		POSITION pos = m_pLayout->m_lstStack.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPaneBase* pPane = m_pLayout->m_lstStack.GetNext(pos);
			if (pPane->GetType() == xtpPaneTypeMiniWnd)
			{
				CXTPDockingPaneMiniWnd* pMiniFrame = (CXTPDockingPaneMiniWnd*)pPane;

				if (pMiniFrame->GetSafeHwnd())
				{
					if (pMiniFrame->m_nIdleFlags & CFrameWnd::idleLayout)
					{
						pMiniFrame->RecalcLayout();
					}

					pMiniFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);
				}
			}
		}
	}

	m_bInitialUpdateCalled = TRUE;
}


LRESULT CXTPDockingPaneManager::OnIdleUpdateCmdUI(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	UpdatePanes();
	return 0;
}

void CXTPDockingPaneManager::SyncPanesState()
{
	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = GetPaneList().GetNext(pos);
		CXTPDockingPaneBase* pContainer = info.pPane->m_pParentContainer;

		if (info.pPane->GetDockingSite() && pContainer)
		{
			if (pContainer->m_pParentContainer == 0 || pContainer->m_pParentContainer->GetType() != xtpPaneTypeAutoHidePanel)
			{

				if (DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, info.pPane->GetDockingSite()))
				{
					info.pLastHolder = info.pFloatingHolder = pContainer;
				}
				else
				{
					info.pLastHolder = info.pDockingHolder = pContainer;
				}
			}
		}
	}
}

LRESULT CXTPDockingPaneManager::OnSizeParent(WPARAM, LPARAM lParam)
{
	if (GetTopPane() != NULL)
	{
		AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

		m_pLayout->OnSizeParent(m_pSite, lpLayout->rect, lpLayout);

		SyncPanesState();

		m_bLayoutCreated = TRUE;
	}
	return 0;
}

CXTPDockingPaneBase* CXTPDockingPaneManager::_Clone(CXTPDockingPaneBase* pPane)
{
	return (CXTPDockingPaneBase*)pPane->Clone(m_pLayout, 0);
}

BOOL CXTPDockingPaneManager::_OnAction(XTPDockingPaneAction action, CXTPDockingPane* pPane, CXTPDockingPaneBase* pDockContainer /*= NULL*/, XTPDockingPaneDirection dockDirection /*= xtpPaneDockLeft*/)
{
	XTP_DOCKINGPANE_ACTION dpAction(action);
	dpAction.pPane = pPane;
	dpAction.pDockContainer = pDockContainer;
	dpAction.dockDirection = dockDirection;

	if ((pPane->GetEnabled() & xtpPaneEnableActions) == 0)
		dpAction.bCancel = TRUE;

	NotifyOwner(XTP_DPN_ACTION, (LPARAM)&dpAction);

	return dpAction.bCancel;
}

void CXTPDockingPaneManager::LockSplitters(BOOL bLock /*= TRUE*/)
{
	m_bLockSplitters = bLock;

	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = GetPaneList().GetNext(pos);
		if (pPane->GetContainer())
			SAFE_CALLPTR(pPane->GetContainer()->GetContainer(), OnChildContainerChanged(pPane));
	}
}


CXTPDockingPaneMiniWnd* CXTPDockingPaneManager::FloatPane(CXTPDockingPaneBase* pPane, CRect rc)
{
	ASSERT(pPane->GetType() == xtpPaneTypeDockingPane || pPane->GetType() == xtpPaneTypeTabbedContainer);

	if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		_RemovePane(pPane);
	}
	else
	{
		pPane = pPane->Clone(m_pLayout, 0);
	}

	CXTPDockingPaneMiniWnd* pMiniFrame = (CXTPDockingPaneMiniWnd*)OnCreatePane(xtpPaneTypeMiniWnd, m_pLayout);
	pMiniFrame->Init(pPane, rc);

	SyncPanesState();
	m_hwndLastFocus = 0;

	return pMiniFrame;
}

CXTPDockingPane* CXTPDockingPaneManager::CreatePane(UINT nID, CRect rc, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour)
{
	ASSERT(GetTopPane());
	CXTPDockingPane* pPane = (CXTPDockingPane*)OnCreatePane(xtpPaneTypeDockingPane, m_pLayout);
	ASSERT(pPane);
	if (!pPane)
		return NULL;

	pPane->SetID(nID);
	pPane->SetWindowRect(rc);


	XTP_DOCKINGPANE_INFO pane(pPane);
	GetPaneList().AddTail(pane);

	_InsertPane(pPane, direction, pNeighbour);

	SyncPanesState();

	return pPane;
}


CXTPDockingPaneBase* CXTPDockingPaneManager::OnCreatePane(XTPDockingPaneType type, CXTPDockingPaneLayout* pLayout)
{
	CXTPDockingPaneBase* pPane = NULL;
	switch (type)
	{
		case xtpPaneTypeDockingPane:
			pPane = new CXTPDockingPane(pLayout);
			break;

		case xtpPaneTypeSplitterContainer:
			pPane = new CXTPDockingPaneSplitterContainer(pLayout);
			break;

		case xtpPaneTypeTabbedContainer:
			pPane = new CXTPDockingPaneTabbedContainer(pLayout);
			break;

		case xtpPaneTypeClient:
			pPane = new CXTPDockingPaneClientContainer(pLayout);
			break;

		case xtpPaneTypeMiniWnd:
			pPane = new CXTPDockingPaneMiniWnd(pLayout);
			break;

		case xtpPaneTypeAutoHidePanel:
			pPane = new CXTPDockingPaneAutoHidePanel(pLayout);
			break;
	}
	ASSERT(pPane);
	return pPane;
}


void CXTPDockingPaneManager::DockPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour)
{
	if (pPane->GetType() == xtpPaneTypeSplitterContainer)
	{
		CXTPDockingPaneBaseList lst;
		pPane->FindPane(xtpPaneTypeTabbedContainer, &lst);

		if (lst.GetCount() == 1)
		{
			pPane = lst.GetHead();
		}
	}

	if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		_RemovePane(pPane);
	}
	else
	{
		pPane = _Clone((CXTPDockingPaneBase*)pPane);
	}
	_InsertPane(pPane, direction, pNeighbour);

	m_pLayout->_FreeEmptyPanes();
	m_hwndLastFocus = 0;
}

void CXTPDockingPaneManager::_RemovePane(CXTPDockingPaneBase* pPane)
{
	if (!pPane)
		return;

	ASSERT(pPane->m_pParentContainer);
	if (!pPane->m_pParentContainer)
		return;

	pPane->m_pParentContainer->RemovePane(pPane);
}

void CXTPDockingPaneManager::AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour)
{
	_AttachPane(pPane, pNeighbour);
	SyncPanesState();
}

void CXTPDockingPaneManager::_AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour)
{
	ASSERT(pNeighbour);
	if (!pNeighbour)
		return;

	if (pNeighbour->GetType() == xtpPaneTypeDockingPane)
	{
		pNeighbour = pNeighbour->m_pParentContainer;
		ASSERT(pNeighbour);
	}
	ASSERT(pNeighbour->GetType() == xtpPaneTypeTabbedContainer);

	if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		_RemovePane(pPane);
		((CXTPDockingPaneTabbedContainer*)pNeighbour)->_InsertPane((CXTPDockingPane*)pPane);
	}
	else
	{
		ASSERT(pPane->GetType() == xtpPaneTypeSplitterContainer || pPane->GetType() == xtpPaneTypeTabbedContainer);

		CXTPDockingPaneBaseList lst;
		((CXTPDockingPaneBase*)pPane)->FindPane(xtpPaneTypeDockingPane, &lst);

		ASSERT(lst.GetCount() > 0);
		POSITION pos = lst.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPaneBase* pListPane = lst.GetNext(pos);
			ASSERT(pListPane->GetType() == xtpPaneTypeDockingPane);

			_RemovePane(pListPane);
			((CXTPDockingPaneTabbedContainer*)pNeighbour)->_InsertPane((CXTPDockingPane*)pListPane);
		}

	}
	m_hwndLastFocus = NULL;
}

CRect CXTPDockingPaneManager::_CalculateResultDockingRect(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour)
{
	if (pNeighbour == NULL)
	{
		pNeighbour = GetTopPane();
	}

	ASSERT(pNeighbour);
	if (!pNeighbour)
		return CRect(0, 0, 0, 0);

	BOOL bHoriz = (direction == xtpPaneDockLeft || direction == xtpPaneDockRight);

	if (pNeighbour == GetTopPane())
	{
		CXTPDockingPaneSplitterContainer* pSplitter = (CXTPDockingPaneSplitterContainer*)pNeighbour;

		return CXTPDockingPaneSplitterContainer::_CalculateResultDockingRectChild(pSplitter, pPane, direction);
	}
	else
	{
		ASSERT(pNeighbour->m_pParentContainer);
		if (!pNeighbour->m_pParentContainer)
			return CRect(0, 0, 0, 0);

		if (pNeighbour->m_pParentContainer->GetType() == xtpPaneTypeTabbedContainer)
		{
			pNeighbour = pNeighbour->m_pParentContainer;
			ASSERT(pNeighbour->m_pParentContainer != NULL);
			if (!pNeighbour->m_pParentContainer)
				return CRect(0, 0, 0, 0);
		}

		ASSERT(pNeighbour->m_pParentContainer);
		ASSERT(pNeighbour->m_pParentContainer->GetType() == xtpPaneTypeSplitterContainer);

		CXTPDockingPaneSplitterContainer* pSplitter = (CXTPDockingPaneSplitterContainer*)pNeighbour->m_pParentContainer;

		if (pSplitter->m_bHoriz == bHoriz)
			return pSplitter->_CalculateResultDockingRectSelf(pPane, direction, pNeighbour);

		return CXTPDockingPaneSplitterContainer::_CalculateResultDockingRectChild(pNeighbour, pPane, direction);
	}
}

void CXTPDockingPaneManager::_InsertPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour)
{
	ASSERT(pPane->m_pParentContainer == NULL);

	if (pNeighbour == NULL || (pNeighbour->GetType() == xtpPaneTypeDockingPane &&
		(IsPaneHidden((CXTPDockingPane*)pNeighbour) || IsPaneClosed((CXTPDockingPane*)pNeighbour))))
	{
		pNeighbour = GetTopPane();
	}

	ASSERT(pNeighbour);
	if (!pNeighbour)
		return;

	CWnd* pParentFrame = pNeighbour->GetDockingSite();
	ASSERT(pParentFrame != NULL);

	BOOL bHoriz = (direction == xtpPaneDockLeft || direction == xtpPaneDockRight);
	BOOL bAfter = (direction == xtpPaneDockRight || direction == xtpPaneDockBottom);

	if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		CXTPDockingPaneTabbedContainer* pContainer = (CXTPDockingPaneTabbedContainer*)OnCreatePane(xtpPaneTypeTabbedContainer, m_pLayout);
		pContainer->Init((CXTPDockingPane*)pPane, pParentFrame);

		pPane = pContainer;
	}

	if (pNeighbour == GetTopPane())
	{
		CXTPDockingPaneSplitterContainer* pSplitter = (CXTPDockingPaneSplitterContainer*)pNeighbour;

		if (pSplitter->m_bHoriz == bHoriz)
			pSplitter->_InsertPane(pPane, NULL, bAfter);
		else
		{
			CXTPDockingPaneSplitterContainer* pChildSplitter = (CXTPDockingPaneSplitterContainer*)OnCreatePane(xtpPaneTypeSplitterContainer, m_pLayout);
			pChildSplitter->Init(pSplitter, bHoriz, pParentFrame);

			pChildSplitter->_InsertPane(pPane, NULL, bAfter);
			m_pLayout->m_pTopContainer = pChildSplitter;
		}
	}
	else
	{
		ASSERT(pNeighbour->m_pParentContainer);
		if (!pNeighbour->m_pParentContainer)
			return;

		if (pNeighbour->m_pParentContainer->GetType() == xtpPaneTypeTabbedContainer)
		{
			pNeighbour = pNeighbour->m_pParentContainer;
			ASSERT(pNeighbour->m_pParentContainer != NULL);
			if (!pNeighbour->m_pParentContainer)
				return;
		}

		ASSERT(pNeighbour->m_pParentContainer);
		ASSERT(pNeighbour->m_pParentContainer->GetType() == xtpPaneTypeSplitterContainer);

		CXTPDockingPaneSplitterContainer* pSplitter = (CXTPDockingPaneSplitterContainer*)pNeighbour->m_pParentContainer;

		if (pSplitter->m_bHoriz == bHoriz)
		{
			pSplitter->_InsertPane(pPane, pNeighbour, bAfter);
		}
		else
		{
			CXTPDockingPaneSplitterContainer* pChildSplitter = (CXTPDockingPaneSplitterContainer*)OnCreatePane(xtpPaneTypeSplitterContainer, m_pLayout);
			pChildSplitter->Init(pNeighbour, bHoriz, pParentFrame);

			pChildSplitter->_InsertPane(pPane, pNeighbour, bAfter);
			pSplitter->_Replace(pNeighbour, pChildSplitter);
		}


	}
	RecalcFrameLayout(pNeighbour, TRUE);
}

CXTPDockingPaneBase* CXTPDockingPaneManager::_GetHolder(CXTPDockingPaneBase* pPane, BOOL bFloating)
{
	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = GetPaneList().GetNext(pos);
		if (info.pPane == pPane)
			return bFloating ? info.pFloatingHolder : info.pDockingHolder;
	}
	return NULL;
}

void CXTPDockingPaneManager::ToggleAutoHide(CXTPDockingPaneBase* pPane)
{
	if (pPane == NULL)
		return;

	if (pPane->IsHidden())
	{
		if (pPane->GetType() == xtpPaneTypeDockingPane && m_bHideGroupOnButtonClick)
		{
			pPane = pPane->m_pParentContainer;
		}

		ToggleDocking(pPane);
	}
	else
	{
		HidePane(pPane);
	}
}

void CXTPDockingPaneManager::ToggleDocking(CXTPDockingPaneBase* pPane)
{
	ASSERT(pPane);
	BOOL bFloating = DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, pPane->GetDockingSite()) != NULL;
	BOOL bHidden = pPane->IsHidden();
	BOOL bDocking = !bFloating && !bHidden;

	if (pPane->GetType() == xtpPaneTypeSplitterContainer || pPane->GetType() == xtpPaneTypeTabbedContainer)
	{

		CXTPDockingPaneBaseList lst;
		((CXTPDockingPaneBase*)pPane)->FindPane(xtpPaneTypeDockingPane, &lst);

		int nIDSelected = 0, nIDFocused = 0;

		POSITION pos = lst.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPane* pListPane = (CXTPDockingPane*)lst.GetNext(pos);
			if (pListPane->IsSelected()) nIDSelected = pListPane->GetID();
			if (pListPane->IsFocus()) nIDFocused = pListPane->GetID();
		}

		pos = lst.GetHeadPosition();
		CXTPDockingPaneBase* pLastHolder = NULL;
		while (pos)
		{
			CXTPDockingPane* pListPane = (CXTPDockingPane*)lst.GetNext(pos);

			CXTPDockingPaneTabbedContainer* pHolder = (CXTPDockingPaneTabbedContainer*)_GetHolder(pListPane, bDocking);
			if (!_ToggleDocking(pListPane, pHolder ? pHolder : pLastHolder))
			{
				if (pListPane->GetID() == nIDSelected) nIDSelected = 0;
				if (pListPane->GetID() == nIDFocused) nIDFocused = 0;
			}

			pLastHolder = pHolder ? pHolder : pListPane->GetContainer();
		}

		if (nIDFocused > 0)
			ShowPane(nIDFocused);
		else if (nIDSelected > 0)
			ShowPane(nIDSelected);
	}
	else if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		CXTPDockingPaneTabbedContainer* pHolder = (CXTPDockingPaneTabbedContainer*)_GetHolder(pPane, bDocking);
		_ToggleDocking((CXTPDockingPane*)pPane, pHolder);
	}
	else
	{
		ASSERT(FALSE);
	}


	m_pLayout->_FreeEmptyPanes();
	m_hwndLastFocus = 0;
}



BOOL CXTPDockingPaneManager::_ToggleDocking(CXTPDockingPane* pPane, CXTPDockingPaneBase* pHolder)
{
	BOOL bFloating = DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, pPane->GetDockingSite()) != NULL;
	BOOL bHidden = pPane->IsHidden();
	BOOL bDocking = !bFloating && !bHidden;


	if (bFloating && (pPane->GetOptions() & xtpPaneNoDockable))
		return FALSE;

	if (bDocking && (pPane->GetOptions() & xtpPaneNoFloatable))
		return FALSE;

	if (_OnAction(bHidden ? xtpPaneActionPinning : !bDocking ? xtpPaneActionDocking : xtpPaneActionFloating, pPane, pHolder))
		return FALSE;

	if (pHolder)
	{
		m_bAttachingPane = TRUE;
		AttachPane(pPane, pHolder);
		m_bAttachingPane = FALSE;
	}
	else
	{
		FloatPane(pPane, pPane->GetContainer()->GetPaneWindowRect());
	}

	_OnAction(bHidden ? xtpPaneActionPinned : !bDocking ? xtpPaneActionDocked : xtpPaneActionFloated, pPane);

	return TRUE;
}

void CXTPDockingPaneManager::EnableKeyboardNavigate(DWORD options)
{
	CXTPDockingPaneKeyboardHook::GetThreadState()->SetupKeyboardHook(this, options != xtpPaneKeyboardUnused);
	m_bKeyboardEnabled = options;
}

bool CXTPDockingPaneManager::InstallDockingPanes(CWnd* pSite, bool bClipChildren/*=true*/)
{
#ifdef _DEBUG
	// must be a valid frame window.
	ASSERT(::IsWindow(pSite->GetSafeHwnd()));

	// should not be initialized yet.
	ASSERT(m_pLayout == 0);
	ASSERT(m_pSite == 0);
#endif

	if (!::IsWindow(pSite->GetSafeHwnd()) || m_pSite != 0)
	{
		// if the docking site is not a valid frame window handle,
		// or the docking site is already initialized, return false.
		return false;
	}

	// Frame windows must have the WS_CLIPCHILDREN and WS_CLIPSIBLINGS
	// styles for panes to be drawn correctly when they are displayed
	// from a hidden state. WS_CLIPSIBLINGS should already be set by
	// default for most frames, however we will include it here just
	// in case.

	if (bClipChildren /*&& DYNAMIC_DOWNCAST(CMDIFrameWnd, pSite) == 0*/)
	{
		long lStyle = ::GetWindowLong(pSite->GetSafeHwnd(), GWL_STYLE);
		if ((lStyle & WS_CLIPSIBLINGS) == 0 || (lStyle & WS_CLIPCHILDREN) == 0)
		{
			::SetWindowLong(pSite->GetSafeHwnd(), GWL_STYLE,
				lStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
	}

	if (!Create(_T("XTPDockingPaneManager"), _T(""), WS_CHILD, CRect(0, 0, 0, 0), pSite, 0))
		return false;

	m_pSite = pSite;
	m_pLayout = CreateLayout();
	m_pLayout->m_bUserLayout = FALSE;


	CMenu* pMenu = m_pSite->GetSystemMenu(FALSE);
	if (pMenu)
	{
		pMenu->GetMenuString(SC_MOVE, m_strMove, MF_BYCOMMAND);
		if (m_strMove.IsEmpty()) m_strMove = _T("&Move");
	}

	return true;
}

CXTPDockingPane* CXTPDockingPaneManager::FindPane(int nID) const
{
	if (!m_pLayout)
		return NULL;

	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = GetPaneList().GetNext(pos);
		if (pPane->GetID() == nID)
			return pPane;
	}

	return NULL;
}

BOOL CXTPDockingPaneManager::SetIcons(UINT nIDResource, const int* nIDs, int nCount, COLORREF clrMask)
{
	CImageList il;
	il.Create(16, 16, ILC_COLOR24 | ILC_MASK, nCount, 1);

	CBitmap bmp;
	bmp.LoadBitmap(nIDResource);
	il.Add(&bmp, clrMask);

	ASSERT(il.GetImageCount() == nCount); // Icons must be 16x16.
	if (il.GetImageCount() != nCount)
		return FALSE;

	for (int i = 0; i < nCount; i++)
	{
		HICON hIcon = il.ExtractIcon(i);
		if (nIDs[i] > 0)
		{
			SetIcon(nIDs[i], hIcon);
		}
		DestroyIcon(hIcon);
	}

	return TRUE;
}

void CXTPDockingPaneManager::SetImageManager(CXTPImageManager* pImageManager)
{
	if (pImageManager)
	{
		m_pImageManager->InternalRelease();

		m_pImageManager = pImageManager;
	}
}

void CXTPDockingPaneManager::ShowPane(CXTPDockingPane* pPane, BOOL bSetFocus /*= TRUE*/)
{
	if (pPane == NULL || m_pLayout == NULL) return;

	if (pPane->m_pParentContainer == NULL)
	{
		POSITION pos = GetPaneList().GetHeadPosition();
		while (pos)
		{
			XTP_DOCKINGPANE_INFO& info = GetPaneList().GetNext(pos);
			if (info.pPane == pPane && info.pLastHolder != NULL)
			{
				((CXTPDockingPaneTabbedContainer*)info.pLastHolder)->_InsertPane(pPane, bSetFocus);
			}
		}
	}
	else
	{
		((CXTPDockingPaneTabbedContainer*)pPane->m_pParentContainer)->SelectPane(pPane, bSetFocus);
		((CXTPDockingPaneTabbedContainer*)pPane->m_pParentContainer)->Show(bSetFocus);
	}
	if (m_bLayoutCreated)
	{
		pPane->ShowWindow(TRUE);
	}

	if (bSetFocus)
	{
		pPane->SetFocus();
	}
}

LRESULT CXTPDockingPaneManager::NotifyOwner(UINT nCode, LPARAM lParam)
{
	CWnd* pOwner = m_hWndOwner != NULL ? GetOwner() : m_pSite;
	ASSERT_VALID(pOwner);

	return pOwner->SendMessage(XTPWM_DOCKINGPANE_NOTIFY,
		(WPARAM)nCode, lParam);
}

void CXTPDockingPaneManager::ClosePane(CXTPDockingPane* pPane)
{
	if (pPane == NULL)
		return;

	pPane->InternalAddRef();

	if (NotifyOwner(XTP_DPN_CLOSEPANE, (LPARAM)pPane) != XTP_ACTION_NOCLOSE)
	{
		SAFE_CALLPTR(pPane->m_pParentContainer, RemovePane(pPane));
	}

	pPane->InternalRelease();
}


void CXTPDockingPaneManager::HidePane(CXTPDockingPaneBase* pPane)
{
	if (pPane == NULL) return;

	if (pPane->GetType() == xtpPaneTypeDockingPane && m_bHideGroupOnButtonClick)
	{
		pPane = pPane->m_pParentContainer;
	}

	if (pPane == NULL || pPane->GetContainer() == NULL)
		return;

	if (pPane->GetContainer()->GetType() == xtpPaneTypeAutoHidePanel)
	{
		((CXTPDockingPaneAutoHidePanel*)pPane->GetContainer())->CloseActiveWindow();
		return;
	}

	m_pLayout->HidePane(pPane);

	RecalcFrameLayout(NULL, TRUE);
}

void CXTPDockingPaneManager::DestroyPane(CXTPDockingPane* pPane)
{
	if (m_pLayout && pPane)
	{
		m_pLayout->DestroyPane(pPane);
		RecalcFrameLayout(NULL);
	}
}

BOOL CXTPDockingPaneManager::IsPaneClosed(CXTPDockingPane* pPane) const
{
	return pPane == NULL || pPane->IsClosed();
}

BOOL CXTPDockingPaneManager::IsPaneHidden(CXTPDockingPane* pPane) const
{
	return pPane != NULL && pPane->IsHidden();
}

BOOL CXTPDockingPaneManager::IsPaneSelected(CXTPDockingPane* pPane) const
{
	return pPane != NULL && pPane->IsSelected();
}

void CXTPDockingPaneManager::_Redraw()
{
	if (!m_hWnd)
		return;

	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = GetPaneList().GetNext(pos);
		RecalcFrameLayout(pPane, TRUE);

		CWnd* pFrame = pPane->GetDockingSite();

		if (DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, pFrame))
		{
			((CXTPDockingPaneMiniWnd*)pFrame)->OnThemedChanged();
		}

		SAFE_INVALIDATE(((CXTPDockingPaneTabbedContainer*)pPane->GetContainer()));
	}
}

void CXTPDockingPaneManager::SyncActiveFloatingFrames(BOOL bSyncActiveFloatingFrames)
{
	if (m_bSyncActiveFloatingFrames == bSyncActiveFloatingFrames)
		return;

	m_bSyncActiveFloatingFrames = bSyncActiveFloatingFrames;

	POSITION pos = GetPaneStack().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = GetPaneStack().GetNext(pos);
		if (pPane->GetType() == xtpPaneTypeMiniWnd)
		{
			CXTPDockingPaneMiniWnd* pFrame = (CXTPDockingPaneMiniWnd*)pPane;
			if (pFrame->m_hWnd)
			{
				pFrame->ModifyStyle(bSyncActiveFloatingFrames ? 0: MFS_SYNCACTIVE, bSyncActiveFloatingFrames ? MFS_SYNCACTIVE : SWP_FRAMECHANGED);
			}
		}
	}
}


void CXTPDockingPaneManager::SetTheme(XTPDockingPanePaintTheme paintTheme)
{
	if (m_pPaintManager != NULL)
		m_pPaintManager->InternalRelease();

	if (paintTheme == xtpPaneThemeOffice) m_pPaintManager = new CXTPDockingPaneOfficeTheme();
	else if (paintTheme == xtpPaneThemeGrippered) m_pPaintManager = new CXTPDockingPaneGripperedTheme();
	else if (paintTheme == xtpPaneThemeVisio) m_pPaintManager = new CXTPDockingPaneVisioTheme();
	else if (paintTheme == xtpPaneThemeOffice2003) m_pPaintManager = new CXTPDockingPaneOffice2003Theme();
	else if (paintTheme == xtpPaneThemeNativeWinXP) m_pPaintManager = new CXTPDockingPaneNativeXPTheme();
	else if (paintTheme == xtpPaneThemeWhidbey) m_pPaintManager = new CXTPDockingPaneWhidbeyTheme();
	else if (paintTheme == xtpPaneThemeVisualStudio2005) m_pPaintManager = new CXTPDockingPaneVisualStudio2005Theme();
	else if (paintTheme == xtpPaneThemeShortcutBar2003) m_pPaintManager = new CXTPDockingPaneShortcutBar2003Theme();
	else if (paintTheme == xtpPaneThemeExplorer) m_pPaintManager = new CXTPDockingPaneExplorerTheme();
	else if (paintTheme == xtpPaneThemeOffice2007) m_pPaintManager = new CXTPDockingPaneOffice2007Theme();
	else m_pPaintManager = new CXTPDockingPaneDefaultTheme();

	m_pPaintManager->m_themeCurrent = paintTheme;
	m_pPaintManager->RefreshMetrics();

	_Redraw();

	if (paintTheme == xtpPaneThemeVisualStudio2005)
	{
		SetDockingContextStickerStyle(xtpPaneStickerStyleVisualStudio2005);
	}
}

void CXTPDockingPaneManager::SetCustomTheme(CXTPDockingPanePaintManager* pTheme)
{
	ASSERT(pTheme);
	if (!pTheme)
		return;

	if (m_pPaintManager != NULL)
		m_pPaintManager->InternalRelease();

	m_pPaintManager = pTheme;
	m_pPaintManager->m_themeCurrent = xtpPaneThemeCustom;
	m_pPaintManager->RefreshMetrics();

	_Redraw();
}


XTPDockingPanePaintTheme CXTPDockingPaneManager::GetCurrentTheme() const
{
	ASSERT(m_pPaintManager);
	return m_pPaintManager->m_themeCurrent;
}

CXTPDockingPaneLayout* CXTPDockingPaneManager::CreateLayout()
{
	return new CXTPDockingPaneLayout(this);
}


void CXTPDockingPaneManager::GetLayout(CXTPDockingPaneLayout* pLayout) const
{
	ASSERT(pLayout);
	if (!pLayout)
		return;

	pLayout->Copy(m_pLayout);
}

void CXTPDockingPaneManager::_DetachAll()
{
	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = GetPaneList().GetNext(pos);
		pPane->Detach();
	}
}

void CXTPDockingPaneManager::SetLayout(const CXTPDockingPaneLayout* pLayout)
{
	ASSERT(pLayout);

	if (!pLayout || !pLayout->IsValid())
		return;

	_DetachAll();

	m_pLayout->Copy(pLayout);
	_Redraw();
}

void CXTPDockingPaneManager::DestroyAll()
{
	if (m_pLayout)
	{
		m_pLayout->InternalRelease();
		m_pLayout = CreateLayout();
		m_pLayout->m_bUserLayout = FALSE;
	}
}
void CXTPDockingPaneManager::CloseAll()
{
	CXTPDockingPaneInfoList& paneList = GetPaneList();
	POSITION pos = paneList.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = paneList.GetNext(pos);
		pPane->Close();
	}
}


CXTPDockingPaneBaseList& CXTPDockingPaneManager::GetPaneStack() const
{
	return m_pLayout->m_lstStack;
}

CXTPDockingPaneInfoList& CXTPDockingPaneManager::GetPaneList() const
{
	return m_pLayout->m_lstPanes;
}

CXTPDockingPaneBase* CXTPDockingPaneManager::GetClientPane() const
{
	return m_pLayout ? m_pLayout->m_pClient : NULL;
}

CXTPDockingPaneBase* CXTPDockingPaneManager::GetTopPane() const
{
	return m_pLayout ? (CXTPDockingPaneBase*)m_pLayout->m_pTopContainer : NULL;
}

void CXTPDockingPaneManager::SetIcon(UINT nID, CXTPImageManagerIconHandle hIcon)
{
	m_pImageManager->SetIcon(hIcon, nID, 0, xtpImageNormal);
}

CXTPImageManagerIcon* CXTPDockingPaneManager::GetIcon(UINT nID, int nWidth) const
{
	return m_pImageManager->GetImage(nID, nWidth);
}

void CXTPDockingPaneManager::ClearIconMap()
{
	m_pImageManager->RemoveAll();
}

void CXTPDockingPaneManager::SetThemedFloatingFrames(BOOL bThemedFloatingFrames)
{
	m_bThemedFloatingFrames = bThemedFloatingFrames;

	if (!m_pLayout)
		return;

	POSITION pos = GetPaneList().GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = GetPaneList().GetNext(pos);
		CWnd* pFrame = pPane->GetDockingSite();

		if (DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, pFrame))
		{
			((CXTPDockingPaneMiniWnd*)pFrame)->OnThemedChanged();
		}
	}

}



void CXTPDockingPaneManager::SetAnimationDelay(double dAnimationDelay /*= -1*/, int nAnimationDuration /*= 128*/, int nAnimationInterval /*= 16*/)
{
	CXTPDockingPaneAutoHideWnd::m_dAnimationDelay = dAnimationDelay;
	CXTPDockingPaneAutoHideWnd::m_nAnimationDuration = nAnimationDuration;
	CXTPDockingPaneAutoHideWnd::m_nAnimationInterval = nAnimationInterval;
}
double CXTPDockingPaneManager::GetAnimationDelay(int* pAnimationDuration /*= NULL*/, int* pAnimationInterval /*= NULL*/) const
{
	if (pAnimationDuration) *pAnimationDuration = CXTPDockingPaneAutoHideWnd::m_nAnimationDuration;
	if (pAnimationInterval) *pAnimationInterval = CXTPDockingPaneAutoHideWnd::m_nAnimationInterval;

	return CXTPDockingPaneAutoHideWnd::m_dAnimationDelay;
}

BOOL CXTPDockingPaneManager::IsLayoutRTL() const
{
	return (GetSite()->GetExStyle() & WS_EX_LAYOUTRTL) != 0;
}

XTPDockingPaneDirection CXTPDockingPaneManager::GetRTLDirection(XTPDockingPaneDirection direction) const
{
	if (IsLayoutRTL())
	{
		if (direction == xtpPaneDockLeft) direction = xtpPaneDockRight;
		else if (direction == xtpPaneDockRight) direction = xtpPaneDockLeft;
	}
	return direction;
}

XTPDockingPaneDirection CXTPDockingPaneManager::GetPaneDirection(const CXTPDockingPaneBase* pPane) const
{
	ASSERT(m_pLayout);
	return m_pLayout->_GetPaneDirection(pPane);
}

void CXTPDockingPaneManager::SetDockingContext(CXTPDockingPaneContext* pDockingContext)
{
	delete m_pDockingContext;
	m_pDockingContext = pDockingContext;
	m_pDockingContext->m_pManager = this;
}



void CXTPDockingPaneManager::NormalizeSplitters()
{
	CXTPDockingPaneBaseList& lst = GetPaneStack();
	POSITION pos = lst.GetHeadPosition();

	while (pos)
	{
		CXTPDockingPaneBase* pPane = lst.GetNext(pos);

		if (pPane->GetType() == xtpPaneTypeSplitterContainer)
		{
			((CXTPDockingPaneSplitterContainer*)pPane)->NormalizeDockingSize();
		}
	}
}

void CXTPDockingPaneManager::RecalcFramesLayout()
{
	CXTPDockingPaneBaseList& lst = GetPaneStack();
	POSITION pos = lst.GetHeadPosition();

	while (pos)
	{
		CXTPDockingPaneBase* pPane = lst.GetNext(pos);

		if (pPane->GetType() == xtpPaneTypeMiniWnd)
		{
			CXTPDockingPaneMiniWnd* pMiniWnd = (CXTPDockingPaneMiniWnd*)pPane;

			if (pMiniWnd->m_hWnd && pMiniWnd->GetStyle() & WS_VISIBLE)
			{
				pMiniWnd->RecalcLayout(TRUE);
				pMiniWnd->MoveWindow(CXTPWindowRect(pMiniWnd));
			}
		}
	}
	RecalcFrameLayout(NULL);
}

void CXTPDockingPaneManager::EnsureVisible(CXTPDockingPaneBase* pPane)
{
	if (!pPane)
		return;

	if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		ShowPane((CXTPDockingPane*)pPane, FALSE);
		pPane = pPane->GetContainer();
	}

	if (pPane->GetType() == xtpPaneTypeTabbedContainer)
	{
		((CXTPDockingPaneTabbedContainer*)pPane)->Show(FALSE);
	}

	CWnd* pSite = pPane->GetDockingSite();
	if (DYNAMIC_DOWNCAST(CXTPDockingPaneMiniWnd, pSite))
	{
		pSite->BringWindowToTop();
	}
}

void CXTPDockingPaneManager::OnSetPreviewMode(BOOL bPreview)
{

// Toggle Docking Panes.
	if (bPreview)
	{
		ASSERT(m_pPreviewLayout == NULL);
		m_pPreviewLayout = CreateLayout();
		GetLayout(m_pPreviewLayout);
		CloseAll();
	}
	else
	{
		ASSERT(m_pPreviewLayout != NULL);
		SetLayout(m_pPreviewLayout);

		CMDTARGET_RELEASE(m_pPreviewLayout);
	}
}

void CXTPDockingPaneManager::OnActivatePane(BOOL bActive, CXTPDockingPane* pPane)
{
	if (bActive && m_pActivePane != pPane)
	{
		if (m_pActivePane)
		{
			_OnAction(xtpPaneActionDeactivated, m_pActivePane);
			m_pActivePane->InternalRelease();
		}

		m_pActivePane = pPane;

		if (m_pActivePane)
		{
			m_pActivePane->InternalAddRef();
			_OnAction(xtpPaneActionActivated, m_pActivePane);
		}
	}
	else if (!bActive && pPane && m_pActivePane == pPane)
	{
		m_pActivePane = 0;
		_OnAction(xtpPaneActionDeactivated, pPane);

		pPane->InternalRelease();
	}
}

BOOL CXTPDockingPaneManager::DoPropExchange(CXTPPropExchange* pPX)
{
	if (pPX->IsStoring())
	{
		CXTPDockingPaneLayout layoutNormal(this);
		GetLayout(&layoutNormal);
		layoutNormal.DoPropExchange(pPX);
	}
	else
	{
		CXTPDockingPaneLayout layoutNormal(this);
		if (!layoutNormal.DoPropExchange(pPX))
			return FALSE;
		SetLayout(&layoutNormal);
	}
	return TRUE;
}

void CXTPDockingPaneManager::RecalcFrameLayout(CXTPDockingPaneBase* pPane, BOOL bDelay)
{
	CWnd* pSite = pPane ? pPane->GetDockingSite() : GetSite();
	if (!pSite || !IsWindow(pSite->GetSafeHwnd()))
		return;

	CFrameWnd* pFrame = pSite->IsFrameWnd() ? (CFrameWnd*)pSite : NULL;

	if (pFrame)
	{
		if (bDelay)
		{
			pFrame->DelayRecalcLayout(FALSE);
			pFrame->PostMessage(WM_IDLEUPDATECMDUI);
		}
		else
			pFrame->RecalcLayout(FALSE);
	}
	else
	{
		CXTPClientRect rc(pSite);
		if (bDelay)
		{
			MSG msg;
			if (PeekMessage(&msg, pSite->GetSafeHwnd(), WM_SIZE, WM_SIZE, PM_NOREMOVE))
				return;

			pSite->PostMessage(WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()));
		}
		else
			pSite->SendMessage(WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()));
	}
}

void CXTPDockingPaneManager::_TrackPopupContextMenu(CXTPDockingPane* pPane)
{
	CPoint pt(0, 0);
	CXTPDockingPaneBase* pContainer = pPane->GetContainer();

	if (pPane->IsFloating())
	{
		pPane->GetDockingSite()->ClientToScreen(&pt);
		pContainer = ((CXTPDockingPaneMiniWnd*)pPane->GetDockingSite())->GetTopPane();
	}
	else
	{
		CRect rc = pContainer->GetPaneWindowRect();
		GetPaintManager()->AdjustCaptionRect((CXTPDockingPaneTabbedContainer*)pContainer, rc);
		pt = rc.TopLeft();
	}

	CMenu menu;
	menu.CreatePopupMenu();
	menu.InsertMenu(0, MF_BYPOSITION | MF_STRING, SC_MOVE, m_strMove);

	BOOL nCommand = TrackPopupMenu(menu, TPM_RETURNCMD, pt.x, pt.y, 0, GetSite()->GetSafeHwnd(), NULL);

	if (nCommand == SC_MOVE)
	{
		CXTPDockingPaneContext* pContext = GetDockingContext();
		pContext->Drag(pContainer, CPoint(-1, -1));
	}
}

BOOL CXTPDockingPaneManager::ActivateNextPane(CXTPDockingPane* pPaneActive, BOOL bForward)
{
	if (!m_pLayout)
		return FALSE;

	CXTPDockingPaneInfoList& paneList = m_pLayout->GetPaneList();
	if (paneList.IsEmpty())
		return FALSE;

	BOOL bActive = (pPaneActive != NULL);

	POSITION posFirst = bForward ? paneList.GetHeadPosition() : paneList.GetTailPosition();
	POSITION pos = posFirst;

	while (pos)
	{
		CXTPDockingPane* pPane = bForward ? paneList.GetNext(pos) : paneList.GetPrev(pos);

		if (pos == NULL && bActive)
		{
			pos = posFirst;
			bActive = FALSE;
		}

		if (!pPaneActive)
		{
			if ((pPane->GetEnabled() & xtpPaneEnableClient) == 0)
				continue;

			ShowPane(pPane);
			return TRUE;
		}
		else if (pPane == pPaneActive)
		{
			pPaneActive = NULL;
		}
	}
	return FALSE;
}

BOOL CXTPDockingPaneManager::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_DESTROY && m_bKeyboardEnabled)
	{
		m_bKeyboardEnabled = FALSE;
		CXTPDockingPaneKeyboardHook::GetThreadState()->SetupKeyboardHook(this, FALSE);
	}

	if (message == WM_SYSCOMMAND && wParam == SC_KEYMENU && LOWORD(lParam) == TEXT('-'))
	{
		CXTPDockingPane* pPane = GetActivePane();
		if (pPane && !pPane->IsClosed() && !pPane->IsHidden())
		{
			_TrackPopupContextMenu(pPane);
			return TRUE;
		}
	}

	if (message == WM_SYSCOMMAND && (wParam == SC_NEXTWINDOW || wParam == SC_PREVWINDOW))
	{
		return ActivateNextPane(GetActivePane(), wParam == SC_NEXTWINDOW);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}


//////////////////////////////////////////////////////////////////////////
// Accessible

CCmdTarget* CXTPDockingPaneManager::GetAccessible()
{
	return this;
}

HRESULT CXTPDockingPaneManager::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	if (GetSafeHwnd())
	{
		return AccessibleObjectFromWindow(GetSafeHwnd(), OBJID_WINDOW, IID_IDispatch, (void**)ppdispParent);
	}
	return E_FAIL;
}

HRESULT CXTPDockingPaneManager::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
		return E_INVALIDARG;

	*pChildCount = (long)m_pLayout->GetPaneList().GetCount();

	return S_OK;
}

HRESULT CXTPDockingPaneManager::GetAccessibleChild(VARIANT varChild, IDispatch* FAR* ppdispChild)
{
	*ppdispChild = NULL;
	int nChild = GetChildIndex(&varChild);

	if (nChild > 0 && nChild <= m_pLayout->GetPaneList().GetCount())
	{
		CXTPDockingPane* pPane = m_pLayout->GetPaneList().GetAt(m_pLayout->GetPaneList().FindIndex(nChild - 1));
		if (pPane)
		{
			*ppdispChild = pPane->GetIDispatch(TRUE);
		}
	}

	return S_OK;
}

HRESULT CXTPDockingPaneManager::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF || nChild == -1)
	{
		*pszName = SysAllocString(L"XTPDockingPaneManager");
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPDockingPaneManager::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_PROPERTYPAGE;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPDockingPaneManager::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	int nChild = GetChildIndex(&varChild);

	if (nChild == CHILDID_SELF || nChild == -1)
	{
		pvarState->vt = VT_I4;
		pvarState->lVal = 0;
		return S_OK;
	}

	return E_INVALIDARG;

}


HRESULT CXTPDockingPaneManager::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT /*varChild*/)
{
	*pxLeft = *pyTop = 0;
	*pcxWidth = *pcyHeight = 10;

	return S_OK;
}

HRESULT CXTPDockingPaneManager::AccessibleHitTest(long /*xLeft*/, long /*yTop*/, VARIANT* /*pvarID*/)
{
	return S_FALSE;
}


LRESULT CXTPDockingPaneManager::OnGetObject(WPARAM wParam, LPARAM lParam)
{
	if (lParam != OBJID_CLIENT)
		return (LRESULT)Default();

	LPUNKNOWN lpUnknown = GetInterface(&IID_IAccessible);
	if (!lpUnknown)
		return E_FAIL;

	return LresultFromObject(IID_IAccessible, wParam, lpUnknown);
}

BEGIN_INTERFACE_MAP(CXTPDockingPaneManager, CCmdTarget)
	INTERFACE_PART(CXTPDockingPaneManager, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()
