// XTPCommandBars.cpp : implementation of the CXTPCommandBars class.
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
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPIntel80Helpers.h"
#include "Common/XTPPropExchange.h"
#include "Common/XTPToolTipContext.h"
#include "TabManager/XTPTabManager.h"

#include "XTPDockBar.h"
#include "XTPMenuBar.h"
#include "XTPShortcutManager.h"
#include "XTPPaintManager.h"
#include "XTPHookManager.h"
#include "XTPMouseManager.h"
#include "XTPControls.h"
#include "XTPControlComboBox.h"
#include "XTPCustomizeTools.h"
#include "XTPCommandBars.h"
#include "XTPSoundManager.h"
#include "XTPControlExt.h"
#include "XTPTabClientWnd.h"
#include "XTPReBar.h"

#ifdef _XTP_INCLUDE_RIBBON
#include "Ribbon/XTPRibbonBar.h"
#endif



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRuntimeClass* CXTPCommandBars::m_pToolBarClass = RUNTIME_CLASS(CXTPToolBar);
CRuntimeClass* CXTPCommandBars::m_pMenuBarClass = RUNTIME_CLASS(CXTPMenuBar);
CRuntimeClass* CXTPCommandBars::m_pPopupBarClass = RUNTIME_CLASS(CXTPPopupBar);
CRuntimeClass* CXTPCommandBars::m_pPopupToolBarClass = RUNTIME_CLASS(CXTPPopupToolBar);
CRuntimeClass* CXTPCommandBars::m_pDockBarClass = RUNTIME_CLASS(CXTPDockBar);


//////////////////////////////////////////////////////////////////////////

CXTPCommandBarsContextMenus::CXTPCommandBarsContextMenus(CXTPCommandBars* pCommandBars)
	: CXTPCommandBarList(pCommandBars)
{
}

CXTPCommandBar* CXTPCommandBarsContextMenus::Add(CXTPCommandBar* pPopupBar)
{
	ASSERT_KINDOF(CXTPPopupBar, pPopupBar);
	m_arrBars.Add(pPopupBar);
	((CXTPPopupBar*)pPopupBar)->m_bContextMenu = TRUE;
	return pPopupBar;
}

CXTPCommandBar* CXTPCommandBarsContextMenus::Add(UINT nIDBar, LPCTSTR strCaption, CMenu* pSubMenu)
{
	CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(m_pCommandBars);
	m_arrBars.Add(pPopupBar);

	pPopupBar->LoadMenu(pSubMenu);
	pPopupBar->SetBarID(nIDBar);

	pPopupBar->SetTitle(strCaption);

	pPopupBar->GetControls()->CreateOriginalControls();
	pPopupBar->m_bContextMenu = TRUE;

	return pPopupBar;
}

CXTPCommandBar* CXTPCommandBarsContextMenus::Add(UINT nIDBar)
{
	CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(m_pCommandBars);
	m_arrBars.Add(pPopupBar);

	CMenu menu;
	if (!menu.LoadMenu(nIDBar))
		return pPopupBar;

	CMenu* pSubMenu = menu.GetSubMenu(0);
	if (!pSubMenu)
		return pPopupBar;

	pPopupBar->LoadMenu(pSubMenu);
	pPopupBar->SetBarID(nIDBar);

	CString strCaption;
	menu.GetMenuString(0, strCaption, MF_BYPOSITION);
	CXTPDrawHelpers::StripMnemonics(strCaption);

	pPopupBar->SetTitle(strCaption);

	pPopupBar->GetControls()->CreateOriginalControls();
	pPopupBar->m_bContextMenu = TRUE;

	return pPopupBar;
}



//////////////////////////////////////////////////////////////////////////
// CXTPCommandBarsOptions

CXTPCommandBarsOptions::CXTPCommandBarsOptions()
{
	bAlwaysShowFullMenus = TRUE;
	bShowFullAfterDelay = TRUE;
	bToolBarScreenTips = TRUE;
	bToolBarAccelTips = FALSE;
	bLargeIcons = FALSE;
	bShowExpandButtonAlways = TRUE;

	szIcons = szLargeIcons = CSize(0);
	szPopupIcons = CSize(0, 0);

	bSyncFloatingBars = TRUE;
	bDblClickFloat = FALSE;
	bShowTextBelowIcons = FALSE;

	animationType = xtpAnimateWindowsDefault;
	bAltDragCustomization = TRUE;

	bAutoHideUnusedPopups = TRUE;

	bDirtyState = FALSE;
	bShowPopupBarToolTips = FALSE;
	bAutoUpdateShortcuts = TRUE;

	keyboardCuesUse = xtpKeyboardCuesUseAll;
	keyboardCuesShow = xtpKeyboardCuesShowAlways;
	bFlyByStatus = TRUE;

	bUseAltNumPadKeys = FALSE;
	bUseSystemSaveBitsStyle = TRUE;

	bDisableCommandIfNoHandler = TRUE;

	bWrapLargePopups = FALSE;

	bShowKeyboardTips = FALSE;
	m_pCommandBars = 0;

	EnableAutomation();

}

#ifndef SPI_GETMENUUNDERLINES
#define SPI_GETMENUUNDERLINES 0x100A
#endif

void CXTPCommandBarsOptions::ShowKeyboardCues(XTPKeyboardCuesShow keyboardCues)
{
	keyboardCuesShow = keyboardCues;

	m_pCommandBars->m_bUseKeyboardCues = FALSE;
	m_pCommandBars->m_bKeyboardCuesVisible = keyboardCues != xtpKeyboardCuesShowNever;

	if (keyboardCues == xtpKeyboardCuesShowWindowsDefault)
	{
		BOOL bRetVal = TRUE;
		if (::SystemParametersInfo(SPI_GETMENUUNDERLINES, 0, &bRetVal, 0))
		{
			if (!bRetVal)
			{
				m_pCommandBars->m_bUseKeyboardCues = TRUE;
				m_pCommandBars->m_bKeyboardCuesVisible = FALSE;
			}
		}
	}
}

XTPKeyboardCuesShow CXTPCommandBarsOptions::GetShowKeyboardCues() const
{
	return keyboardCuesShow;
}



CXTPCommandBars* CXTPCommandBars::CreateCommandBars()
{
	CXTPCommandBars* pCommandBars = (CXTPCommandBars*)CXTPCommandBars::CreateObject();
	return pCommandBars;
}


// CXTPCommandBars

IMPLEMENT_DYNCREATE(CXTPCommandBars, CCmdTarget)

CXTPCommandBars::CXTPCommandBars()
	: m_pFrame (0)
	, m_nDefaultMenuID(0)
{
	ZeroMemory(m_pDocks, 4 * sizeof(CXTPDockBar*));
	m_pDragSelected = NULL;

	m_bQuickCustomizeMode = m_bCustomizeMode = FALSE;
	m_bCustomizeAvail = FALSE;
	m_pDropSource = new CXTPCustomizeDropSource(this);

	m_pPaintManager = NULL;
	m_pImageManager = NULL;
	m_pMouseManager = NULL;
	m_pKeyboardManager = NULL;

	m_pOptions = new CXTPCommandBarsOptions;
	m_pOptions->m_pCommandBars = this;

	m_pDesignerControls = NULL;

	m_bIgnoreShiftMenuKey = FALSE;

	EnableAutomation();

	m_bRecalcLayout = FALSE;
	m_bRightToLeft = FALSE;

	m_pToolTipContext = new CXTPToolTipContext;
	m_pToolTipContext->ModifyToolTipStyle(TTS_NOPREFIX, 0);
	m_bUseKeyboardCues = FALSE;
	m_bKeyboardCuesVisible = TRUE;

	m_pShortcutManager = new CXTPShortcutManager(this);

	m_nIDHelpTracking = 0;

	m_bDesignerMode = FALSE;

	m_keyboardTips.pBar = NULL;
	m_keyboardTips.nLevel = 0;
	m_keyboardTips.nKey = 0;


	m_pActions = new CXTPControlActions(this);
	m_bEnableActions = FALSE;

	m_pContextMenus = new CXTPCommandBarsContextMenus(this);

}

CXTPCommandBars::~CXTPCommandBars()
{
	CMDTARGET_RELEASE(m_pDragSelected);
	CMDTARGET_RELEASE(m_pDesignerControls);

	m_pContextMenus->RemoveAll();
	CMDTARGET_RELEASE(m_pContextMenus);

	RemoveAll();

	for (int i = 0; i < 4; i++)
	{
		if (m_pDocks[i])
		{
			m_pDocks[i]->DestroyWindow();
			delete m_pDocks[i];
		}
	}

	delete m_pDropSource;

	CMDTARGET_RELEASE(m_pPaintManager);
	CMDTARGET_RELEASE(m_pImageManager);
	CMDTARGET_RELEASE(m_pToolTipContext);
	CMDTARGET_RELEASE(m_pShortcutManager);

	CMDTARGET_RELEASE(m_pOptions);

	XTPSoundManager()->StopThread();

	if (m_pActions)
	{
		m_pActions->RemoveAll();
		CMDTARGET_RELEASE(m_pActions);
	}
}

BOOL IsToolBarVisible(CXTPToolBar* pCommandBar)
{
	return pCommandBar&& pCommandBar->IsWindowVisible();
}


void CXTPCommandBars::EnableActions()
{
	ASSERT(!m_bEnableActions);
	ASSERT(m_pActions->GetCount() == 0);
	m_bEnableActions = TRUE;
}

CXTPControlAction* CXTPCommandBars::CreateAction(int nId)
{
	return m_pActions->Add(nId);
}

CXTPControlAction* CXTPCommandBars::FindAction(int nId) const
{
	return this == NULL ? NULL : m_pActions->FindAction(nId);
}


CXTPMenuBar* CXTPCommandBars::GetMenuBar() const
{
	return DYNAMIC_DOWNCAST(CXTPMenuBar, GetToolBar(XTP_IDR_MENUBAR));
}

CXTPMenuBar* CXTPCommandBars::SetMenu(LPCTSTR strName, UINT nID)
{
	CXTPMenuBar* pMenuBar = (CXTPMenuBar*) m_pMenuBarClass->CreateObject();
	ASSERT(pMenuBar);
	if (!pMenuBar)
		return NULL;

	pMenuBar->m_pCommandBars = this;

	pMenuBar->SetTitle(strName);
	pMenuBar->m_nBarID = XTP_IDR_MENUBAR;

	if (!pMenuBar->SetPosition(xtpBarTop) ||
		!pMenuBar->LoadMenuBar(nID))
	{
		delete pMenuBar;
		return NULL;
	}

	if (nID > 0)
	{
		pMenuBar->m_pControls->CreateOriginalControls();
		m_nDefaultMenuID = nID;
	}

	m_arrBars.Add(pMenuBar);

	return pMenuBar;
}

void CXTPCommandBars::RecalcFrameLayout(BOOL bDelay)
{
	if (m_bRecalcLayout)
		return;

	if (!m_pFrame->GetSafeHwnd())
		return;

	CFrameWnd* pFrame = m_pFrame->IsFrameWnd() ? (CFrameWnd*)m_pFrame : NULL;
	if (pFrame)
	{
		if (bDelay)
			pFrame->DelayRecalcLayout(FALSE);
		else
			pFrame->RecalcLayout(FALSE);
	}
	else
	{
		CXTPClientRect rc(m_pFrame);
		m_pFrame->SendMessage(WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()));
	}
}

void CXTPCommandBars::IdleRecalcLayout()
{
	CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, GetSite());
	if (pFrame)
	{
		if (pFrame->m_nIdleFlags & CFrameWnd::idleLayout)
		{
			pFrame->RecalcLayout(FALSE);
			pFrame->m_nIdleFlags &= ~CFrameWnd::idleLayout;
		}
	}
}

void CXTPCommandBars::OnToolBarAdded(CXTPToolBar* pToolBar)
{
	ASSERT(pToolBar);

	CXTPReBar* pReBar = GetFrameReBar();
	if (pReBar)
	{
		pReBar->AddToolBar(pToolBar, RBBS_GRIPPERALWAYS | RBBS_BREAK);
	}
}

void CXTPCommandBars::OnToolBarRemoved(CXTPToolBar* pToolBar)
{
	ASSERT(pToolBar);

	CXTPReBar* pReBar = GetFrameReBar();
	if (pReBar)
	{
		pReBar->DeleteToolBar(pToolBar);
	}
}

CXTPReBar* CXTPCommandBars::GetFrameReBar() const
{
	if (!m_pFrame->GetSafeHwnd())
		return NULL;

	return DYNAMIC_DOWNCAST(CXTPReBar, m_pFrame->GetDlgItem(AFX_IDW_REBAR));
}

CXTPToolBar* CXTPCommandBars::AddCustomBar(LPCTSTR lpcstrCaption, UINT nID, BOOL bTearOffBar)
{
	ASSERT(nID != 0);

	CXTPToolBar* pCommandBar = (CXTPToolBar*) m_pToolBarClass->CreateObject();
	pCommandBar->m_pCommandBars = this;

	XTP_COMMANDBARS_CREATEBAR cs;
	ZeroMemory(&cs, sizeof(cs));
	cs.bCustomBar = TRUE;
	cs.bTearOffBar = bTearOffBar;
	cs.lpcstrCaption = lpcstrCaption;
	cs.pCommandBar = pCommandBar;
	cs.nID = nID;

	m_pFrame->SendMessage(WM_XTP_CREATECOMMANDBAR, 0, (LPARAM)&cs);

	ASSERT(pCommandBar == cs.pCommandBar);

	ASSERT(pCommandBar);
	pCommandBar->m_strTitle = lpcstrCaption;

	if (!pCommandBar->SetPosition(bTearOffBar ? xtpBarFloating : xtpBarTop))
	{
		delete pCommandBar;
		return NULL;
	}

	m_arrBars.Add(pCommandBar);

	pCommandBar->SetBarID(nID);
	pCommandBar->m_bBuiltIn = FALSE;
	pCommandBar->m_bTearOff = bTearOffBar;

	if (!bTearOffBar) RecalcFrameLayout();

	OnToolBarAdded(pCommandBar);

	return pCommandBar;
}

CXTPShortcutManager* CXTPCommandBars::GetShortcutManager() const
{
	return m_pShortcutManager;
}

void CXTPCommandBars::SetShortcutManager(CXTPShortcutManager* pShortcutManager)
{
	CMDTARGET_RELEASE(m_pShortcutManager);
	m_pShortcutManager = pShortcutManager;
}


CXTPToolBar* CXTPCommandBars::Add(LPCTSTR strName, XTPBarPosition xtpPosition, CRuntimeClass* pToolbarClass)
{
	if (!pToolbarClass)
		pToolbarClass = m_pToolBarClass;

	ASSERT(pToolbarClass && pToolbarClass->IsDerivedFrom(RUNTIME_CLASS(CXTPToolBar)));
	ASSERT(IsDockingPosition(xtpPosition) || xtpPosition == xtpBarFloating);

	CXTPToolBar* pCommandBar = (CXTPToolBar*) pToolbarClass->CreateObject();
	pCommandBar->m_pCommandBars = this;

	ASSERT(pCommandBar);
	pCommandBar->m_strTitle = strName;

	if (!pCommandBar->SetPosition(xtpPosition))
	{
		delete pCommandBar;
		return NULL;
	}

	m_arrBars.Add(pCommandBar);


	return pCommandBar;
}

void CXTPCommandBars::EnableDocking()
{
	const DWORD dwDockBarMap[4][2] =
	{
		{ AFX_IDW_DOCKBAR_TOP, CBRS_TOP   },
		{ AFX_IDW_DOCKBAR_BOTTOM, CBRS_BOTTOM },
		{ AFX_IDW_DOCKBAR_LEFT, CBRS_LEFT   },
		{ AFX_IDW_DOCKBAR_RIGHT, CBRS_RIGHT  },
	};

	CWnd* pParentFrame = GetSite();
	ASSERT(pParentFrame);

	for (int i = 0; i < 4; i++)
	{
		ASSERT(!m_pDocks[i]);

		CXTPDockBar* pDockBar = (CXTPDockBar*)m_pDockBarClass->CreateObject();
		pDockBar->m_pCommandBars = this;

		if (!pDockBar->Create(pParentFrame,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE |
			dwDockBarMap[i][1], dwDockBarMap[i][0]))
		{
			AfxThrowResourceException();
		}
		m_pDocks[i] = pDockBar;
	}
}

CXTPDockBar* CXTPCommandBars::CanDock(CPoint pt, CXTPDockBar* pMainDock) const
{
	CRect rc;
	if (pMainDock)
	{
		pMainDock->GetWindowRect(&rc);
		if (pMainDock->m_dwStyle & CBRS_ORIENT_HORZ &&
			rc.top - 20 < pt.y && rc.bottom + 20 > pt.y && rc.left - 20 < pt.x && rc.right + 20 > pt.x)
			return pMainDock;
		if (pMainDock->m_dwStyle & CBRS_ORIENT_VERT &&
			rc.left - 20 < pt.x && rc.right + 20 > pt.x && rc.top - 20 < pt.y && rc.bottom + 20 > pt.y)
			return pMainDock;
	}
	for (int i = 0; i < 4; i++)
	{
		m_pDocks[i]->GetWindowRect(&rc);
		if (m_pDocks[i]->m_dwStyle & CBRS_ORIENT_HORZ &&
			rc.top - 20 < pt.y && rc.bottom + 20 > pt.y && rc.left - 20 < pt.x && rc.right + 20 > pt.x)
			return m_pDocks[i];
		if (m_pDocks[i]->m_dwStyle & CBRS_ORIENT_VERT &&
			rc.left - 20 < pt.x && rc.right + 20 > pt.x && rc.top - 20 < pt.y && rc.bottom + 20 > pt.y)
			return m_pDocks[i];
	}
	return NULL;
}

BOOL CXTPCommandBars::DockCommandBar(CXTPToolBar* pBar, LPRECT lpRect, CXTPDockBar* pDockBar)
{
	ASSERT(pDockBar);

	if (!pBar->Create(m_pFrame, FALSE))
		return FALSE;

	pDockBar->DockCommandBar(pBar, lpRect);
	pBar->m_barPosition = pDockBar->GetPosition();

	return TRUE;
}

BOOL CXTPCommandBars::DockCommandBar(CXTPToolBar* pBar, XTPBarPosition xtpPosition)
{
	CXTPDockBar* pDockBar = GetDockBar(xtpPosition);
	ASSERT(pDockBar);

	return DockCommandBar(pBar, NULL, pDockBar);
}

BOOL CXTPCommandBars::FloatCommandBar(CXTPToolBar* pBar)
{
	if (!pBar->Create(m_pFrame, TRUE))
	{
		return FALSE;
	}
	if (pBar->m_pDockBar != NULL)
	{
		pBar->m_pDockBar->RemoveCommandBar(pBar);
		pBar->m_pDockBar = NULL;

	}
	pBar->m_barPosition = xtpBarFloating;

	return TRUE;
}

void CXTPCommandBars::DelayRedrawCommandBars()
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);
		if (pBar->IsVisible() && pBar->GetSafeHwnd())
		{
			pBar->DelayRedraw();
		}
	}
}

void CXTPCommandBars::InvalidateCommandBars()
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);
		if (pBar->IsVisible() && pBar->GetSafeHwnd())
		{
			pBar->Redraw();
		}
	}
}

void CXTPCommandBars::RedrawCommandBars()
{
	m_bRecalcLayout = TRUE;

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);

		for (int j = 0; j < pBar->GetControlCount(); j++)
		{
			pBar->GetControl(j)->OnThemeChanged();
		}

		if (pBar->IsVisible())
		{
			pBar->OnRecalcLayout();
		}
	}

	CMDIFrameWnd* pMDIFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, GetSite());
	if (pMDIFrame)
	{
		CXTPTabClientWnd* pClient = DYNAMIC_DOWNCAST(CXTPTabClientWnd, CWnd::FromHandle(pMDIFrame->m_hWndMDIClient));
		if (pClient)
			pClient->Refresh();
	}

	CWnd* pWnd = GetSite()->GetDlgItem(AFX_IDW_STATUS_BAR);
	if (pWnd)
	{
		pWnd->Invalidate(FALSE);
	}

	m_bRecalcLayout = FALSE;
	RecalcFrameLayout();

	GetSite()->SendMessage(WM_NCPAINT);
}

BOOL CXTPCommandBar::IsKeyboardCuesVisible() const
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (!pCommandBars)
		return TRUE;

	return pCommandBars->m_bKeyboardCuesVisible;
}


CXTPCommandBarKeyboardTip::CXTPCommandBarKeyboardTip(CXTPCommandBars* pCommandBars, CCmdTarget* pOwner, LPCTSTR lpszCaption, CPoint pt, DWORD dwAlign, BOOL bEnabled)
{
	m_pOwner = pOwner;
	m_strCaption = lpszCaption;
	m_pCommandBars = pCommandBars;
	m_pt = pt;
	m_strTip.Empty();
	m_bEnabled = bEnabled;
	m_dwAlign = dwAlign;
}

BEGIN_MESSAGE_MAP(CXTPCommandBarKeyboardTip, CWnd)
	ON_WM_PAINT()
	ON_WM_NCHITTEST_EX()
END_MESSAGE_MAP()

LRESULT CXTPCommandBarKeyboardTip::OnNcHitTest(CPoint /*point*/)
{
	return (LRESULT)HTTRANSPARENT;
}

void CXTPCommandBarKeyboardTip::OnPaint()
{
	CPaintDC dc(this);
	m_pCommandBars->GetPaintManager()->DrawKeyboardTip(&dc, this, FALSE);
}

BOOL CXTPCommandBar::IsKeyboardTipsVisible() const
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars && pCommandBars->m_keyboardTips.pBar == this && pCommandBars->m_keyboardTips.nLevel > 0)
		return pCommandBars->m_keyboardTips.nLevel;

	return FALSE;
}

void CXTPCommandBar::CreateKeyboardTips()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (!pCommandBars)
		return;

	for (int i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl->IsVisible() || pControl->GetCaption().IsEmpty() || (pControl->GetFlags() & xtpFlagSkipFocus))
			continue;

		if (pControl->GetType() == xtpControlGallery)
			continue;

		CPoint pt(pControl->GetRect().left + 12, pControl->GetRect().bottom - 11);

		CXTPCommandBarKeyboardTip* pWnd = new CXTPCommandBarKeyboardTip(pCommandBars, pControl, pControl->GetCaption(), pt, DT_TOP, pControl->GetEnabled());

		if (pControl->GetAction() && !pControl->GetAction()->GetKeyboardTip().IsEmpty())
		{
			pWnd->m_strTip = pControl->GetAction()->GetKeyboardTip();
		}

		pCommandBars->m_keyboardTips.arr.Add(pWnd);
	}

	if (GetParentCommandBar() == 0)
	{
		for (int j = 0; j < pCommandBars->GetCount(); j++)
		{
			CXTPToolBar* pToolBar = pCommandBars->GetAt(j);
			if (pToolBar == this || !IsToolBarVisible(pToolBar))
				continue;
			CXTPWindowRect rc(pToolBar);

			CPoint pt(rc.left, rc.top);
			CXTPCommandBarKeyboardTip* pWnd = new CXTPCommandBarKeyboardTip(pCommandBars, pToolBar, pToolBar->GetTitle(), pt, DT_TOP | DT_LEFT, TRUE);

			pCommandBars->m_keyboardTips.arr.Add(pWnd);
		}
	}
}

#ifndef LWA_ALPHA
#define LWA_ALPHA               0x00000002
#endif
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED           0x00080000
#endif
typedef BOOL (WINAPI *PFNSETLAYEREDWINDOWATTRIBUTES) (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

void CXTPCommandBars::ShowKeyboardTips(CXTPCommandBar* pCommandBar, int nLevel)
{
	HideKeyboardTips();

	if (!m_pOptions->bShowKeyboardTips)
		return;

	if (!pCommandBar->GetSafeHwnd())
		return;

	pCommandBar->UpdateWindow();

	m_keyboardTips.pBar = pCommandBar;
	m_keyboardTips.nLevel = nLevel;
	m_keyboardTips.nKey = 0;

	pCommandBar->CreateKeyboardTips();

	if (m_keyboardTips.arr.GetSize() == 0)
	{
		m_keyboardTips.pBar = NULL;
		return;
	}

	CXTPPaintManager* pPaintManager = GetPaintManager();
	CClientDC dc(pCommandBar);
	CXTPFontDC font(&dc, &pPaintManager->m_fontToolTip);

	int i;
	for (i = (int)m_keyboardTips.arr.GetSize() - 1; i >= 0; i--)
	{
		CXTPCommandBarKeyboardTip* pWnd = m_keyboardTips.arr[i];

		if (!pWnd->m_strTip.IsEmpty())
			continue;

		if (pWnd->m_strCaption.IsEmpty())
		{
			pWnd->DestroyWindow();
			delete pWnd;
			m_keyboardTips.arr.RemoveAt(i);
			continue;
		}
		pWnd->m_strCaption.MakeUpper();

		int nAmdIndex = pWnd->m_strCaption.Find(_T('&'));

		pWnd->m_strTip = pWnd->m_strCaption.GetAt(nAmdIndex + 1); // nAmdIndex can be -1.
	}

	int nCount = (int)m_keyboardTips.arr.GetSize();

	for (i = 0; i < nCount; i++)
	{
		CXTPCommandBarKeyboardTip* pWndBase = m_keyboardTips.arr[i];

		CString strTip = pWndBase->m_strTip;
		CArray<CXTPCommandBarKeyboardTip*, CXTPCommandBarKeyboardTip*> arr;
		arr.Add(pWndBase);
		int j;

		for ( j = i + 1; j < nCount; j++)
		{
			CXTPCommandBarKeyboardTip* pWnd = m_keyboardTips.arr[j];
			if (pWnd->m_strTip == strTip)
			{
				arr.Add(pWnd);
			}
		}

		if (arr.GetSize() < 2)
			continue;

		CString strUsed = _T("& ");
		int k = 1;

		for (j = 0; j < (int)arr.GetSize(); j++)
		{
			CXTPCommandBarKeyboardTip* pWnd = arr[j];
			TCHAR chAccel = 0;

			for (int n = 0; n < pWnd->m_strCaption.GetLength(); n++)
			{
				if (strUsed.Find(pWnd->m_strCaption[n]) == -1)
				{
					chAccel = pWnd->m_strCaption[n];
					strUsed += chAccel;
					break;
				}
			}
			if (chAccel == 0)
			{
				CString str;
				str.Format(_T("%i"), k);
				chAccel = str[0];
				k++;
			}

			arr[j]->m_strTip += chAccel;
		}
	}

	BOOL bLayoutRTL = IsLayoutRTL();

	for (i = 0; i < nCount; i++)
	{
		CXTPCommandBarKeyboardTip* pWnd = m_keyboardTips.arr[i];

		CSize sz = dc.GetTextExtent(pWnd->m_strTip);
		sz = CSize(max(sz.cx + 6, 16), max(sz.cy + 2, 15));

		CPoint pt = pWnd->m_pt;
		if (DYNAMIC_DOWNCAST(CWnd, pWnd->m_pOwner) == 0)
			pCommandBar->ClientToScreen(&pt);

		CRect rc(CPoint(pt.x - (pWnd->m_dwAlign & DT_CENTER ?  sz.cx / 2 : bLayoutRTL || (pWnd->m_dwAlign & DT_RIGHT) ? sz.cx : 0),
			pt.y - (pWnd->m_dwAlign & DT_VCENTER ?  sz.cy / 2 : pWnd->m_dwAlign & DT_BOTTOM ? sz.cy : 0)), sz);

		BOOL bLayered = pPaintManager->m_pfnSetLayeredWindowAttributes && !pWnd->m_bEnabled;

		pWnd->CreateEx((bLayered ? WS_EX_LAYERED : 0) | WS_EX_TOOLWINDOW, AfxRegisterWndClass(CS_SAVEBITS), 0, WS_POPUP, rc, GetSite(), 0);

		pWnd->SetWindowPos(&CWnd::wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(),  SWP_NOACTIVATE | SWP_NOOWNERZORDER);

		pPaintManager->DrawKeyboardTip(0, pWnd, TRUE);

		if (bLayered)
		{
			((PFNSETLAYEREDWINDOWATTRIBUTES)pPaintManager->m_pfnSetLayeredWindowAttributes)(pWnd->m_hWnd, 0x00, 150, LWA_ALPHA);
		}
	}

	for (i = 0; i < nCount; i++)
	{
		CXTPCommandBarKeyboardTip* pWnd = m_keyboardTips.arr[i];
		pWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
	}
}

void CXTPCommandBars::HideKeyboardTips()
{
	if (m_keyboardTips.arr.GetSize() == 0)
		return;

	for (int i = 0; i < (int)m_keyboardTips.arr.GetSize(); i++)
	{
		m_keyboardTips.arr[i]->DestroyWindow();
		delete m_keyboardTips.arr[i];
	}

	m_keyboardTips.arr.RemoveAll();
	m_keyboardTips.pBar = 0;
	m_keyboardTips.nLevel = 0;
	m_keyboardTips.nKey = 0;
}

void CXTPCommandBars::ShowKeyboardCues(BOOL bShow)
{
	if (m_bCustomizeMode)
		bShow = TRUE;

	if (m_bUseKeyboardCues && m_bKeyboardCuesVisible != bShow)
	{
		m_bKeyboardCuesVisible = bShow;
		InvalidateCommandBars();
	}
}

void CXTPCommandBars::OnTrackingModeChanged(CXTPCommandBar* pCommandBar, int bMode)
{
	pCommandBar;

	CXTPMouseManager* pMouseManager = GetMouseManager();

	if (bMode == FALSE && m_bUseKeyboardCues && !pMouseManager->IsTrackedLock(0))
	{
		ShowKeyboardCues(FALSE);
	}

	if (m_keyboardTips.nLevel > 0 && m_pOptions->bShowKeyboardTips)
	{
		HideKeyboardTips();

		if (pMouseManager->IsTrackedLock(0))
		{
			CXTPCommandBar* pCommandBarTop = pMouseManager->GetTrackArray().GetAt(pMouseManager->GetTrackArray().GetSize() - 1);
			if (pCommandBarTop->m_bKeyboardSelect)
			{
					pCommandBarTop->PostMessage(WM_TIMER, XTP_TID_SHOWKEYBOARDTIPS);
			}
		}
	}


}

CXTPMouseManager* CXTPCommandBars::GetMouseManager() const
{
	if (this == NULL)
		return XTPMouseManager();

	if (m_pMouseManager != NULL)
		return m_pMouseManager;

	return m_pMouseManager = XTPMouseManager();
}

CXTPKeyboardManager* CXTPCommandBars::GetKeyboardManager() const
{
	if (this == NULL)
		return XTPKeyboardManager();

	if (m_pKeyboardManager != NULL)
		return m_pKeyboardManager;

	return m_pKeyboardManager = XTPKeyboardManager();
}


CXTPControl* CXTPCommandBars::_GetNextControl(CXTPControls*& pControls, int nIndex) const
{
	int nNextIndex = pControls->GetNext(nIndex, +1);

	if (m_pOptions->keyboardCuesUse & xtpKeyboardCuesUseAll
		&& nNextIndex <= nIndex)
	{
		CXTPToolBar* pToolBar = (CXTPToolBar*)pControls->GetParent();
		CXTPToolBar* pStartBar = pToolBar;

		for (;;)
		{
			pToolBar = GetAt(GetNextVisible(FindIndex(pToolBar), +1));
			ASSERT(pToolBar);

			pControls = pToolBar->GetControls();
			nNextIndex = pControls->GetNext(-1, +1);

			if (nNextIndex != -1)
				break;

			if (pToolBar == pStartBar)
				return NULL;
		}
	}

	return pControls->GetAt(nNextIndex);
}

CXTPControl* CXTPCommandBars::FindAccel(CXTPCommandBar* pCommandBar, UINT chAccel, BOOL& bSelectOnly) const
{
	ASSERT(pCommandBar->IsVisible());

	CXTPControls* pControls = pCommandBar->GetControls();

	CXTPControl* pControl = _GetNextControl(pControls, pCommandBar->m_nSelected);
	if (pControl == NULL)
		return NULL;

	bSelectOnly = FALSE;

	CXTPControl* pStart = pControl, *pAmpCondidate = NULL, *pCondidate = NULL;

	do
	{
		ASSERT(pControl);

		CString strCaption = pControl->IsCaptionVisible() ? pControl->GetCaption() : _T("");
		if (!strCaption.IsEmpty() && pControl->GetEnabled())
		{
			int nAmpIndex = strCaption.Find(_T('&'));
			BOOL bFound = nAmpIndex > -1 && nAmpIndex < strCaption.GetLength() - 1;

			if (CXTPShortcutManager::CompareAccelKey(strCaption[bFound ? nAmpIndex + 1 : 0], chAccel) &&
				(bFound || ((m_pOptions->keyboardCuesUse & xtpKeyboardCuesUseAmpersandOnly) == 0)))
			{
				if (bFound)
				{
					if (pAmpCondidate == NULL)
						pAmpCondidate = pControl;
					else
					{
						bSelectOnly = TRUE;
						return pAmpCondidate;
					}
				}
				else if (pControl->GetParent()->GetType() == xtpBarTypeMenuBar || pControl->GetParent()->IsTrackingMode())
				{
					if (pCondidate == NULL)
						pCondidate = pControl;
					else
					{
						bSelectOnly = TRUE;
					}
				}
			}
		}

		pControl = _GetNextControl(pControls, pControl->GetIndex());

		ASSERT(pControl != 0);
		if (pControl == NULL)
			break;
	}
	while (pControl != pStart);

	return pAmpCondidate ? pAmpCondidate : pCondidate;
}

void CXTPCommandBars::ClosePopups() const
{
	CXTPMouseManager* pMouseManage = this == NULL ? XTPMouseManager() : GetMouseManager();
	pMouseManage->SendTrackLost();
}

BOOL CXTPCommandBars::OnFrameAccel(UINT chAccel)
{
	if (m_pOptions->keyboardCuesUse == xtpKeyboardCuesUseNone)
		return FALSE;

	if ((chAccel >= VK_NUMPAD0 && chAccel <= VK_NUMPAD9) && !m_pOptions->bUseAltNumPadKeys)
		return FALSE;

	if (MapVirtualKey(chAccel, 2) == 0)
		return FALSE;

	CXTPToolBar* pMenuBar = GetMenuBar();
	CXTPToolBar* pCommandBar = IsToolBarVisible(pMenuBar) ? pMenuBar : NULL;

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pToolBar = GetAt(i);

		if (pToolBar->IsTrackingMode())
		{
			pCommandBar = pToolBar;
			break;
		}

		if (m_pOptions->keyboardCuesUse & xtpKeyboardCuesUseAll && pCommandBar == NULL && IsToolBarVisible(pToolBar))
			pCommandBar = pToolBar;
	}
	if (!pCommandBar)
		return FALSE;

	if (IsToolBarVisible(pCommandBar))
	{
		if (pCommandBar->PreviewAccel(chAccel))
		{
			return TRUE;
		}

		BOOL bSelectOnly;
		CXTPControl* pControl = FindAccel(pCommandBar, chAccel, bSelectOnly);
		if (pControl)
		{
			int nIndex = pControl->GetIndex();
			pCommandBar = (CXTPToolBar*)pControl->GetParent();

			if (!pCommandBar->IsTrackingMode())
				ClosePopups();

			ShowKeyboardCues(TRUE);

			m_keyboardTips.nLevel = 1;

			pCommandBar->SetTrackingMode(TRUE, FALSE, TRUE);
			pCommandBar->SetSelected(nIndex, TRUE);

			if (!bSelectOnly)
			{
				pControl->OnUnderlineActivate();
			}

			return TRUE;
		}
	}
	return FALSE;
}

BOOL CXTPCommandBars::PreTranslateFrameMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_SYSKEYDOWN) && (HIWORD(pMsg->lParam) & KF_ALTDOWN)
		&& !GetMouseManager()->IsMouseLocked() && !GetMouseManager()->IsTrackedLock(0)
		&& GetMouseManager()->IsTopParentActive(m_pFrame->GetSafeHwnd()))
	{
		if (pMsg->wParam == VK_MENU)
		{
			ShowKeyboardCues(TRUE);
			return FALSE;
		}

		if ((!m_bIgnoreShiftMenuKey || !(::GetKeyState(VK_SHIFT) & 0x8000)))
		{
			UINT chAccel = (UINT)pMsg->wParam;

			if (chAccel)
			{
				if (OnFrameAccel(chAccel))
					return TRUE;
			}
		}
	}

	if ((pMsg->message == WM_SYSKEYUP || pMsg->message == WM_KEYUP) && m_bUseKeyboardCues && m_bKeyboardCuesVisible &&
		!GetMouseManager()->IsTrackedLock(0))
	{
		ShowKeyboardCues(FALSE);
	}


	if ((pMsg->message == WM_CONTEXTMENU || pMsg->message == WM_SYSKEYUP)
		&& GetMouseManager()->IsTrackedLock(0))
	{
		XTPKeyboardManager()->ProcessKeyboardHooks(WM_CONTEXTMENU, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}

	if (pMsg->message == WM_MOUSEWHEEL)
	{
		if (XTPKeyboardManager()->IsKeyboardHooked())
		{
			return XTPKeyboardManager()->ProcessKeyboardHooks(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
		}
#ifdef _XTP_INCLUDE_RIBBON
		else
		{
			CXTPCommandBar* pMenuBar = GetMenuBar();
			CPoint pt;
			GetCursorPos(&pt);

			if (pMenuBar && pMenuBar->GetType() == xtpBarTypeRibbon && CXTPWindowRect(pMenuBar).PtInRect(pt))
			{
				((CXTPRibbonBar*)pMenuBar)->SelectNextTab((short)HIWORD(pMsg->wParam) <= 0);
				return TRUE;
			}
		}
#endif
	}

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (GetShortcutManager()->TranslateAccelerator(GetSite()->GetSafeHwnd(), pMsg))
			return TRUE;
	}


	return FALSE;
}

void CXTPCommandBars::UpdateCommandBars() const
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPCommandBar* pBar = GetAt(i);
		pBar->OnIdleUpdateCmdUI(TRUE, TRUE);
	}
}

BOOL CXTPCommandBars::OnFrameWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_IDLEUPDATECMDUI)
	{
		for (int i = 0; i < GetCount(); i++)
		{
			CXTPCommandBar* pBar = GetAt(i);

			if (pBar->GetPosition() == xtpBarFloating)
			{
				pBar->OnIdleUpdateCmdUI(TRUE, TRUE);
			}
		}
	}
	if (message == WM_SYSCOLORCHANGE)
	{
		XTPColorManager()->RefreshColors();
		GetPaintManager()->RefreshMetrics();
		GetImageManager()->RefreshAll();

		GetSite()->RedrawWindow(FALSE);
		RedrawCommandBars();
	}
	if (message == WM_SETTINGCHANGE)
	{
		GetPaintManager()->RefreshMetrics();

		GetSite()->RedrawWindow(FALSE);
		RedrawCommandBars();
	}
	if (message == WM_CLOSE)
	{
		if (m_pDropSource->m_pSheet != NULL)
		{
			return TRUE;
		}
	}
	if (message == WM_COMMAND)
	{
		CWnd* pWnd = CWnd::GetFocus();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CXTPEdit)) && ((CXTPEdit*)pWnd)->OnCommand(wParam, lParam))
		{
			if (pResult != NULL)
				*pResult = 1;
			return TRUE;
		}

		if (LOWORD(wParam) == ID_HELP && GetHelpTrackingId() > 0)
		{
			AFX_CMDHANDLERINFO info;
			info.pTarget = NULL;

			if (GetSite()->OnCmdMsg(ID_HELP, CN_COMMAND, NULL, &info))
			{
				GetSite()->SendMessage(WM_COMMANDHELP, 0, HID_BASE_COMMAND +  GetHelpTrackingId());
				return TRUE;
			}
		}
	}

	switch (message)
	{
	case WM_ACTIVATEAPP:
		if (!wParam)
			ClosePopups();
		return FALSE;

	case WM_SYSCOMMAND:
		if ((wParam == 0 || LOWORD(wParam) != 0) && wParam != SC_KEYMENU && !IsCustomizeMode())
			ClosePopups();
		return FALSE;
	}

	return FALSE;
}

void CXTPCommandBars::SetLayoutRTL(CWnd* pWnd, BOOL bRTLLayout)
{
	DWORD dwRTLStyle = WS_EX_LAYOUTRTL;

	if (pWnd->IsKindOf(RUNTIME_CLASS(CXTPEdit)))
	{
		pWnd->ModifyStyleEx(0, dwRTLStyle);
		return;
	}

	pWnd->ModifyStyleEx(bRTLLayout ? 0 : dwRTLStyle, !bRTLLayout ? 0 : dwRTLStyle);

	// walk through HWNDs to avoid creating temporary CWnd objects
	// unless we need to call this function recursively
	for (CWnd* pChild = pWnd->GetWindow(GW_CHILD); pChild != NULL;
		pChild = pChild->GetWindow(GW_HWNDNEXT))
	{

		// send to child windows after parent
		SetLayoutRTL(pChild, bRTLLayout);
	}
}


void CXTPCommandBars::SetLayoutRTL(BOOL bRightToLeft)
{
	if (!XTPSystemVersion()->IsLayoutRTLSupported())
		return;

	if (m_bRightToLeft == bRightToLeft)
		return;

	m_bRightToLeft = bRightToLeft;

	int i;

	for (i = 0; i < 4; i++)
	{
		if (m_pDocks[i])
		{
			SetLayoutRTL(m_pDocks[i], m_bRightToLeft);
		}
	}

	for (i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pToolBar = GetAt(i);
		if (pToolBar->GetPosition() == xtpBarFloating)
		{
			SetLayoutRTL(pToolBar, m_bRightToLeft);
		}
	}

	CWnd* pStatusBar = GetSite()->GetDlgItem(AFX_IDW_STATUS_BAR);
	if (pStatusBar)
	{
		SetLayoutRTL(pStatusBar, m_bRightToLeft);
	}

	GetImageManager()->DrawReverted(2);
	RedrawCommandBars();
}

BOOL CXTPCommandBars::IsLayoutRTL() const
{
	if (GetSite()->GetExStyle() & WS_EX_LAYOUTRTL)
		return TRUE;

	return m_bRightToLeft;
}

CXTPControl* CXTPCommandBars::FindControl(XTPControlType type, UINT nId, BOOL bVisible, BOOL bRecursive) const
{

	for (int i = GetCount() - 1; i >= 0; i--)
	{
		CXTPToolBar* pBar = GetAt(i);

		if (!bVisible || pBar->IsVisible())
		{
			CXTPControl* pControl = pBar->GetControls()->FindControl(type, nId, bVisible, bRecursive);
			if (pControl != NULL) return pControl;
		}
	}
	return NULL;
}

void CXTPPopupBar::PumpMessage()
{

	while (IsTrackingMode())
	{
		MSG msg;
		if (::GetMessage(&msg, NULL, NULL, NULL))
		{
			if (!IsTrackingMode())
			{
				::PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
				break;
			}

			if (msg.message != WM_KICKIDLE && !AfxGetThread()->PreTranslateMessage(&msg))
			{
				// process this message
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
			break;
	}
}

BOOL CXTPCommandBars::TrackPopupMenu(UINT nBarID, UINT nFlags, int x, int y, LPCRECT rcExclude)
{
	CXTPPopupBar* pPopup = (CXTPPopupBar*)m_pContextMenus->FindCommandBar(nBarID);
	if (!pPopup)
		return FALSE;

	return TrackPopupMenu(pPopup, nFlags, x, y, GetSite(), rcExclude);

}

BOOL CXTPCommandBars::TrackPopupMenu(CXTPPopupBar* pPopup, UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT rcExclude, CWnd* pWndOwner)
{
	ASSERT(pPopup);
	if (!pPopup)
	{
		return FALSE;
	}

	const MSG& msg = AfxGetThreadState()->m_lastSentMsg;
	if (msg.message == WM_CONTEXTMENU && msg.lParam == MAKELPARAM(-1, -1) && pPopup->GetCommandBars())
		pPopup->GetCommandBars()->ShowKeyboardCues(TRUE);

	if (!pWnd)
		pWnd = pPopup->GetSite();

	pPopup->m_pSite = pWnd ;
	pPopup->m_pOwner = pWndOwner;

	if (pWnd && pWnd->GetExStyle() & (WS_EX_LAYOUTRTL | WS_EX_RIGHT))
		nFlags |= TPM_RIGHTALIGN;

	UINT nReturn = TRUE;
	pPopup->m_pReturnCmd = NULL;

	if (nFlags & TPM_RETURNCMD)
	{
		pPopup->m_pReturnCmd = &nReturn;
		nReturn = 0;
	}
	pPopup->m_bExecOnRButton = nFlags & TPM_RIGHTBUTTON;
	pPopup->m_bIgnoreUpdateHandler = nFlags & TPM_NONOTIFY;
	pPopup->m_popupFlags = (nFlags & TPM_RIGHTALIGN) ? xtpPopupLeft | xtpPopupDown : xtpPopupDown;

	BOOL bRecurse = nFlags & TPM_RECURSE;
	pPopup->m_bRecursePopup = bRecurse;

	CXTPMouseManager* pMouseManager = XTPMouseManager();


	if (!bRecurse)
	{
		pMouseManager->SendTrackLost();
		ReleaseCapture();
	}
	else
	{
		pMouseManager->SendTrackLostRecurse();
		pMouseManager->LockTrackRecurse(TRUE);
	}
	pMouseManager->IgnoreLButtonUp();

	if (!pPopup->Popup(x, y, rcExclude))
	{
		return FALSE;
	}

	pPopup->PumpMessage();

	if (bRecurse)
	{
		pMouseManager->LockTrackRecurse(FALSE);
	}

	return nReturn;
}

BOOL CXTPCommandBars::TrackPopupMenuEx(CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWndOwner, LPTPMPARAMS lptpm)
{
	CXTPPopupBar* pPopup = CXTPPopupBar::CreatePopupBar(this);

	pPopup->m_pSite = GetSite();
	pPopup->m_pCommandBars = this;

	CRect rcExclude;
	rcExclude.SetRectEmpty();
	if (lptpm) rcExclude = lptpm->rcExclude;

	if (!pPopup->LoadMenu(pMenu))
	{
		pPopup->InternalRelease();
		return FALSE;
	}

	BOOL bResult = TrackPopupMenu(pPopup, nFlags, x, y, GetSite(), rcExclude, pWndOwner);

	pPopup->InternalRelease();

	return bResult;
}

BOOL CXTPCommandBars::TrackPopupMenu(CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT rcExclude, CWnd* pWndOwner, CXTPCommandBars* pCommandBars)
{
	CXTPPopupBar* pPopup = CXTPPopupBar::CreatePopupBar(pCommandBars);

	pPopup->m_pSite = pWnd;
	pPopup->m_pOwner = pWndOwner;
	pPopup->m_pCommandBars = pCommandBars;


	if (!pPopup->LoadMenu(pMenu))
	{
		pPopup->InternalRelease();
		return FALSE;
	}

	if (pCommandBars)
	{
		const MSG& msg = AfxGetThreadState()->m_lastSentMsg;
		if (msg.message == WM_CONTEXTMENU && msg.lParam == MAKELPARAM(-1, -1))
			pCommandBars->ShowKeyboardCues(TRUE);
	}

	BOOL bResult = TrackPopupMenu(pPopup, nFlags, x, y, pWnd, rcExclude, pWndOwner);

	pPopup->InternalRelease();

	return bResult;
}




CXTPPopupBar* CXTPCommandBars::GetToolbarsPopup()
{
	CXTPPopupBar* pPopup = CXTPPopupBar::CreatePopupBar(this);

	CXTPControlToolbars::CreateToolbarMenu(pPopup, 0, ID_VIEW_TOOLBAR, TRUE);

	return pPopup;
}

void CXTPCommandBars::ContextMenu(CXTPToolBar* pToolBar, CPoint point)
{
	if (IsCustomizeMode())
		return;

	CXTPPopupBar* pPopup = GetToolbarsPopup();

	GetSite()->SendMessage(WM_XTP_TOOLBARCONTEXTMENU, (WPARAM)pToolBar, (LPARAM)pPopup);

	if (pPopup && pPopup->GetControlCount() > 0)
	{
		pPopup->m_popupFlags = IsLayoutRTL() ? xtpPopupLeft : xtpPopupRight;
		ClosePopups();

		pPopup->Popup(point.x, point.y, NULL);

		while (pPopup->IsTrackingMode())
		{
			AfxGetThread()->PumpMessage();
		}
	}

	CMDTARGET_RELEASE(pPopup);
}

CXTPToolBar* CXTPCommandBars::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < GetCount())
		return m_arrBars.GetAt(nIndex);

	return 0;
}

void CXTPCommandBars::ToggleVisible(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < GetCount());

	CXTPToolBar* pBar = GetAt(nIndex);
	if (pBar)
	{
		pBar->SetVisible(!pBar->IsVisible());
	}

	m_pOptions->bDirtyState = TRUE;
}

CXTPToolBar* CXTPCommandBars::GetToolBar(UINT nID) const
{
	if (nID == 0)
		return NULL;

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);

		ASSERT(pBar != NULL);
		if (pBar->m_nBarID == nID)
			return pBar;
	}
	return NULL;
}

void CXTPToolBar::_GetHiddenControls(CXTPCommandBar* pExpandBar)
{
	for (int j = 0; j < GetControlCount(); j++)
	{
		CXTPControl* pControl = GetControl(j);

		if (pControl->GetHideFlags() == xtpHideWrap)
		{
			CXTPControl* pControlClone = pExpandBar->GetControls()->AddClone(pControl);
			pControlClone->SetHideFlags(0);
			pControlClone->SetBeginGroup(FALSE);
		}
	}
}

void CXTPCommandBars::GetHiddenControls(CXTPToolBar* pToolBar, CXTPCommandBar* pExpandBar)
{
	if (pToolBar->m_pDockBar == 0) return;

	CXTPDockBar* pDock = pToolBar->m_pDockBar;
	int nPos = pDock->FindBar(pToolBar);
	if (nPos == -1) return;

	CXTPDockBar::CToolBarArray arrBars;
	pDock->GetVisibleToolbars(nPos, arrBars);
	for (int i = 0; i < arrBars.GetSize(); i++)
	{
		CXTPToolBar* pToolBarRow = arrBars[i];
		pToolBarRow->_GetHiddenControls(pExpandBar);
	}
}

void CXTPCommandBars::_GetAddOrRemovePopup(CXTPToolBar* pToolBar, CXTPCommandBar* pExpandBar)
{
	CXTPControlPopup* pButton = (CXTPControlPopup*)pExpandBar->GetControls()->Add(xtpControlPopup, 0);
	pButton->SetFlags(xtpFlagManualUpdate);
	pButton->SetCaption(CXTPControlWindowList::ConstructCaption(pToolBar->GetTitle(), 0));

	BOOL bEnabled = m_bCustomizeAvail && pToolBar->m_bBuiltIn && pToolBar->m_bCustomizable;

	pButton->SetEnabled(bEnabled);
	if (bEnabled)
	{
		pToolBar->BuildCustomizePopup(pButton->GetCommandBar());
		ASSERT_KINDOF(CXTPPopupBar, pButton->GetCommandBar());
		((CXTPPopupBar*)pButton->GetCommandBar())->SetDoubleGripper();
	}
}

void CXTPCommandBars::GetAddOrRemovePopup(CXTPToolBar* pToolBar, CXTPCommandBar* pExpandBar)
{
	if (pToolBar->m_pDockBar == 0)
	{
		_GetAddOrRemovePopup(pToolBar, pExpandBar);
	}
	else
	{
		CXTPDockBar* pDock = pToolBar->m_pDockBar;
		int nPos = pDock->FindBar(pToolBar);
		if (nPos == -1)
		{
			_GetAddOrRemovePopup(pToolBar, pExpandBar);
		}
		else
		{
			CXTPDockBar::CToolBarArray arrBars;
			pDock->GetVisibleToolbars(nPos, arrBars);
			for (int i = 0; i < arrBars.GetSize(); i++)
			{
				_GetAddOrRemovePopup(arrBars[i], pExpandBar);
			}
		}
	}

	if (m_bCustomizeAvail)
	{
		CXTPControl* pControlCustomize = pExpandBar->GetControls()->Add(xtpControlButton, XTP_ID_CUSTOMIZE);
		pControlCustomize->SetBeginGroup(TRUE);
	}
}


void CXTPCommandBars::SetQuickCustomizeMode(BOOL bMode)
{
	m_pOptions->bDirtyState = TRUE;
	m_bCustomizeMode = m_bQuickCustomizeMode = bMode;
	SetDragControl(NULL);

	ShowKeyboardCues(m_bCustomizeMode);
}

void CXTPCommandBars::SetCustomizeMode(BOOL bMode)
{
	m_pOptions->bDirtyState = TRUE;
	m_bCustomizeMode = bMode;
	SetDragControl(NULL);

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);

		pBar->DelayRedraw();
		pBar->OnIdleUpdateCmdUI(TRUE, TRUE);
	}
	RecalcFrameLayout();

	ShowKeyboardCues(m_bCustomizeMode);
}

void CXTPCommandBars::SetDragControl(CXTPControl* pControl)
{
	if (m_pDragSelected != pControl)
	{

		CXTPControl* pDragSelected = m_pDragSelected;
		m_pDragSelected = pControl;

		if (pDragSelected != NULL)
		{
			pDragSelected->RedrawParent();
			pDragSelected->InternalRelease();
		}

		if (m_pDragSelected)
		{
			m_pDragSelected->RedrawParent(FALSE);
			m_pDragSelected->InternalAddRef();
		}
	}

	CWnd* pWnd = m_pDropSource->m_pSheet ? (CWnd*)m_pDropSource->m_pSheet : m_pFrame;
	if (pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->SendMessage(WM_XTP_DRAGCONTROLCHANGED);
	}
}

void CXTPCommandBars::RemoveAll()
{
	m_bRecalcLayout = TRUE;

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pToolBar = GetAt(i);

		if (pToolBar->GetSafeHwnd())
		{
			pToolBar->ShowWindow(SW_HIDE);
			pToolBar->m_bVisible = FALSE;
		}

		if (pToolBar->m_pDockBar != NULL)
		{
			pToolBar->m_pDockBar->RemoveCommandBar(pToolBar);
			pToolBar->m_pDockBar = NULL;
		}

		OnToolBarRemoved(pToolBar);
		pToolBar->OnRemoved();

		pToolBar->InternalRelease();
	}
	m_arrBars.RemoveAll();

	m_bRecalcLayout = FALSE;
	RecalcFrameLayout(TRUE);

}

void CXTPCommandBars::Remove(CXTPToolBar* pToolBar)
{
	ASSERT(pToolBar);

	int i = FindIndex(pToolBar);
	if (i == -1)
		return;

	if (pToolBar->GetSafeHwnd())
	{
		pToolBar->ShowWindow(SW_HIDE);
	}
	pToolBar->m_bVisible = FALSE;

	if (pToolBar->m_pDockBar != NULL)
	{
		pToolBar->m_pDockBar->RemoveCommandBar(pToolBar);
		pToolBar->m_pDockBar = NULL;
		RecalcFrameLayout();
	}

	OnToolBarRemoved(pToolBar);
	pToolBar->OnRemoved();

	m_arrBars.RemoveAt(i);
	pToolBar->InternalRelease();
}

void CXTPCommandBars::SetSite(CWnd* pFrame)
{
	m_pFrame = pFrame;

	if (pFrame->GetSafeHwnd())
	{
		AFX_CMDHANDLERINFO info;
		info.pTarget = NULL;

		m_bCustomizeAvail = pFrame->CWnd::OnCmdMsg(XTP_ID_CUSTOMIZE, CN_COMMAND, this, &info);
	}

	m_bRightToLeft = m_pFrame->GetExStyle() & WS_EX_LAYOUTRTL;
}



void CXTPCommandBars::HideCommands(const UINT* pCommands, int nCount)
{
	ASSERT(pCommands);
	for (int i = 0; i < nCount; i++)
		HideCommand(pCommands[i]);
}

void CXTPCommandBars::HideCommand(UINT nCommand)
{
	m_pOptions->m_mapHiddenCommands.SetAt(nCommand, TRUE);
}


BOOL CXTPCommandBars::IsControlHidden(CXTPControl* pControl)
{
	BOOL bCommandHidden = IsCommandHidden(pControl->GetID());

	if (m_pOptions->bAutoHideUnusedPopups && !bCommandHidden && IsPopupControlType(pControl->GetType()))
	{
		CXTPCommandBar* pCommandBar = pControl->GetCommandBar();
		if (!pCommandBar)
			return FALSE;

		int nCount = pCommandBar->GetControlCount();

		if (nCount == 0)
			return FALSE;

		for (int i = 0; i < nCount; i++)
		{
			if (!IsControlHidden(pCommandBar->GetControl(i)))
				return FALSE;
		}

		return TRUE;

	}
	return bCommandHidden;
}

BOOL CXTPCommandBars::IsCommandHidden(UINT nCommand)
{
	BOOL bCommand;

	if (!m_pOptions->m_mapHiddenCommands.Lookup(nCommand, bCommand))
		return FALSE;

	if (m_pOptions->m_mapUsedCommands.Lookup(nCommand, bCommand))
		return FALSE;

	return TRUE;
}

void CXTPCommandBars::SetCommandUsed(UINT nCommand)
{
	if (IsCommandHidden(nCommand))
	{
		m_pOptions->m_mapUsedCommands.SetAt(nCommand, TRUE);
		m_pOptions->bDirtyState = TRUE;
	}
}
void CXTPCommandBars::ResetUsageData()
{
	m_pOptions->m_mapUsedCommands.RemoveAll();
	m_pOptions->bDirtyState = TRUE;
}


const TCHAR _xtpCommandBarOptionsSection[] = _T("%s-Options");

void PX_UintMap(CXTPPropExchange* pPX, LPCTSTR pszPropName, CMap<UINT, UINT, BOOL, BOOL>& mapValue)
{
	if (pPX->IsAllowBlobValues())
	{
		PX_Serialize(pPX, pszPropName, &mapValue);
		return;
	}

	CXTPPropExchangeSection secMap(pPX->GetSection(pszPropName));

	UINT nID;
	if (pPX->IsStoring())
	{
		secMap->WriteCount((DWORD)mapValue.GetCount());
		BOOL bValue;
		int i = 0;
		POSITION pos = mapValue.GetStartPosition();
		while (pos)
		{
			CString strSection;
			strSection.Format(_T("Item%i"), i++);

			mapValue.GetNextAssoc(pos, nID, bValue);

			PX_Long(&secMap, strSection, (long&)nID, 0);
		}
	}
	else
	{
		mapValue.RemoveAll();

		int nCount = secMap->ReadCount();
		for (int i = 0; i < nCount; i++)
		{
			CString strSection;
			strSection.Format(_T("Item%i"), i);
			PX_Long(&secMap, strSection, (long&)nID, 0);

			mapValue.SetAt(nID, TRUE);
		}
	}
}

void CXTPCommandBarsOptions::DoPropExchange(CXTPPropExchange* pPX, BOOL bUserOptionsOnly)
{
	pPX->UseDefaultOnLoad(FALSE);
	pPX->UseDefaultOnSave(FALSE);

	XTP_COMMANDBARS_ICONSINFO* pInfo = m_pCommandBars->GetPaintManager()->GetIconsInfo();
	DWORD dwSchema = _XTP_SCHEMA_CURRENT;

	PX_DWord(pPX, _T("Schema"), dwSchema, _XTP_SCHEMA_CURRENT);
	PX_Bool(pPX, _T("AlwaysShowFullMenus"), bAlwaysShowFullMenus, TRUE);
	PX_Bool(pPX, _T("ShowFullAfterDelay"), bShowFullAfterDelay, TRUE);
	PX_Bool(pPX, _T("ToolBarScreenTips"), bToolBarScreenTips, TRUE);
	PX_Bool(pPX, _T("ToolBarAccelTips"), bToolBarAccelTips, FALSE);
	PX_Bool(pPX, _T("LargeIcons"), bLargeIcons, FALSE);

	if ((dwSchema > _XTP_SCHEMA_875) && !(pPX->IsStoring() && pPX->IsAllowDefaultValues() && m_mapUsedCommands.IsEmpty()))
		PX_UintMap(pPX, _T("UsedCommands"), m_mapUsedCommands);

	if ((dwSchema > _XTP_SCHEMA_875) || bUserOptionsOnly)
		PX_Enum(pPX, _T("Animation"), animationType, xtpAnimateWindowsDefault);


	if (!bUserOptionsOnly)
	{
		PX_Bool(pPX, _T("SyncFloatingBars"), bSyncFloatingBars, TRUE);
		PX_Bool(pPX, _T("ShowExpandButtonAlways"), bShowExpandButtonAlways, TRUE);
		PX_Size(pPX, _T("IconsSize"), szIcons, CSize(0, 0));
		PX_Size(pPX, _T("LargeIconsSize"), szLargeIcons, CSize(0, 0));

		if (!(pPX->IsStoring() && pPX->IsAllowDefaultValues() && m_mapHiddenCommands.IsEmpty()))
			PX_UintMap(pPX, _T("HiddenCommands"), m_mapHiddenCommands);

		PX_Bool(pPX, _T("IconsWithShadow"), pInfo->bIconsWithShadow, FALSE);
		PX_Bool(pPX, _T("UseDisabledIcons"), pInfo->bUseDisabledIcons, TRUE);
		PX_Bool(pPX, _T("UseFadedIcons"), pInfo->bUseFadedIcons, TRUE);

		if (dwSchema > _XTP_SCHEMA_875)
		{
			PX_Bool(pPX, _T("SyncFloatingBars"), bSyncFloatingBars, TRUE);
			PX_Bool(pPX, _T("ShowExpandButtonAlways"), bShowExpandButtonAlways, TRUE);
			PX_Bool(pPX, _T("DblClickFloat"), bDblClickFloat, FALSE);
			PX_Bool(pPX, _T("ShowTextBelowIcons"), bShowTextBelowIcons, FALSE);
			PX_Bool(pPX, _T("AltDragCustomization"), bAltDragCustomization, TRUE);
			PX_Bool(pPX, _T("AutoHideUnusedPopups"), bAutoHideUnusedPopups, TRUE);
			PX_Size(pPX, _T("PopupIconsSize"), szPopupIcons, CSize(0, 0));
		}

		if (dwSchema > _XTP_SCHEMA_95)
		{
			PX_DWord(pPX, _T("KeyboardCuesUse"), keyboardCuesUse, xtpKeyboardCuesUseAll);
			PX_Enum(pPX, _T("KeyboardCuesShow"), keyboardCuesShow, xtpKeyboardCuesShowAlways);

			if (pPX->IsLoading()) ShowKeyboardCues(keyboardCuesShow);
		}
	}

}
void CXTPCommandBarsOptions::Serialize(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(&px);
}


void CXTPCommandBars::SaveOptions(LPCTSTR lpszProfileName)
{
	TCHAR szSection[256];
	wsprintf(szSection, _xtpCommandBarOptionsSection, lpszProfileName);

	CXTPPropExchangeSection px(FALSE, szSection);
	px->EmptySection();
	m_pOptions->DoPropExchange(&px, TRUE);
}


void CXTPCommandBars::LoadOptions(LPCTSTR lpszProfileName)
{
	TCHAR szSection[256];
	wsprintf(szSection, _xtpCommandBarOptionsSection, lpszProfileName);

	CXTPPropExchangeSection px(TRUE, szSection);
	m_pOptions->DoPropExchange(&px, TRUE);

}

void CXTPCommandBars::SetTheme(XTPPaintTheme paintTheme)
{
	SetPaintManager(CXTPPaintManager::CreateTheme(paintTheme));
}


void CXTPCommandBars::SetPaintManager(CXTPPaintManager* pPaintManager)
{
	if (m_pPaintManager)
		m_pPaintManager->InternalRelease();

	m_pPaintManager = pPaintManager;

	if (m_pPaintManager)
	{
		m_pPaintManager->RefreshMetrics();
	}

	for (int i = 0; i < GetCount(); i++)
	{
		GetAt(i)->OnPaintManagerChanged();
	}

	GetImageManager()->RefreshAll();
	RedrawCommandBars();
}

void CXTPCommandBars::SetImageManager(CXTPImageManager* pImageManager)
{
	ASSERT(m_pImageManager == NULL);
	m_pImageManager = pImageManager;

	GetImageManager()->RefreshAll();
}

CXTPPaintManager* CXTPCommandBars::GetPaintManager() const
{
	if (m_pPaintManager)
		return m_pPaintManager;
	return XTPPaintManager();

}
CXTPImageManager* CXTPCommandBars::GetImageManager() const
{
	if (m_pImageManager)
		return m_pImageManager;
	return XTPImageManager();
}

void CXTPCommandBars::LoadDesignerBars(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	LoadDesignerBars(&px);
}

void CXTPCommandBars::LoadDesignerBars(CXTPPropExchange* pPX)
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		// Load Docking Position and Controls
		XTP_COMMANDBARS_PROPEXCHANGE_PARAM param;
		param.bSerializeControls = TRUE;
		param.bSerializeDesignerControls = TRUE;
		param.bSerializeImages = TRUE;
		param.bSerializeOptions = TRUE;
		param.bSerializeActions = TRUE;

		DoPropExchange(pPX, &param);

	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
	}
}


CDocTemplate* CXTPCommandBars::FindDocTemplate(CMDIChildWnd* pChild)
{
	USES_PROTECTED_ACCESS(CXTPCommandBars, CMDIChildWnd, HMENU, m_hMenuShared)

	CMultiDocTemplate* pTemplate = DYNAMIC_DOWNCAST(CMultiDocTemplate, pChild->GetActiveDocument()->GetDocTemplate());
	if (!pTemplate)
		return NULL;

	HMENU hMenuShared = PROTECTED_ACCESS(CMDIChildWnd, pChild, m_hMenuShared);

	if ((hMenuShared == 0) || (pTemplate->m_hMenuShared == hMenuShared))
		return pTemplate;

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();

	while (pos)
	{
		pTemplate = DYNAMIC_DOWNCAST(CMultiDocTemplate, AfxGetApp()->GetNextDocTemplate(pos));

		if (pTemplate && pTemplate->m_hMenuShared == hMenuShared)
			return pTemplate;
	}


	return NULL;
}


UINT CXTPCommandBars::GetActiveDocTemplate()
{
	#pragma warning(disable:4510)
	#pragma warning(disable:4610)

	USES_PROTECTED_ACCESS(CXTPCommandBars, CDocTemplate, UINT, m_nIDResource)
	USES_PROTECTED_ACCESS(CXTPCommandBars, CFrameWnd, UINT, m_nIDHelp)

	CMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, GetSite());

	if (!pFrame)
		return 0;

	UINT nIDResource = (WORD)m_nDefaultMenuID;

	CMDIChildWnd* pChild = pFrame->MDIGetActive();
	if (pChild)
	{
		if (pChild->GetActiveDocument())
		{
			CDocTemplate* pDocTemplate = FindDocTemplate(pChild);
			if (!pDocTemplate)
				return 0;
			nIDResource = PROTECTED_ACCESS(CDocTemplate, pDocTemplate, m_nIDResource);
		}
		else
		{
			nIDResource = PROTECTED_ACCESS(CFrameWnd, pChild, m_nIDHelp);
		}
	}

	return nIDResource;
}


void CXTPCommandBars::OnSetPreviewMode (BOOL bPreview)
{
	GetShortcutManager()->DisableShortcuts(bPreview);

	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPToolBar* pBar = GetAt(i);
		pBar->OnSetPreviewMode(bPreview);
	}
}

CXTPToolTipContext* CXTPCommandBars::GetToolTipContext() const
{
	return m_pToolTipContext;
}


int CXTPCommandBars::FindIndex(CXTPToolBar* pToolBar) const
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPToolBar* pBar = GetAt(i);
		if (pBar == pToolBar)
			return i;
	}
	return -1;
}

int CXTPCommandBars::GetNextVisible(long nIndex, int nDirection) const
{
	ASSERT(nDirection == +1 || nDirection == -1);
	if (GetCount() == 0) return -1;

	long nNext = nIndex + nDirection;

	if (nDirection == -1 && nIndex == -1)
	{
		nNext = GetCount() - 1;
	}

	BOOL bCircle = FALSE;

	while (nNext != nIndex)
	{
		if (nNext >= GetCount())
		{
			if ((nIndex == -1 && nDirection == +1) || bCircle) return -1;
			nNext = 0;
			bCircle = TRUE;
		}
		if (nNext < 0)
		{
			if ((nIndex == -1 && nDirection == -1) || bCircle) return -1;
			nNext = GetCount() - 1;
			bCircle = TRUE;
		}

		if (GetAt(nNext)->IsWindowVisible())
			return nNext;

		nNext += nDirection;
	}
	return nNext;
}

BOOL CXTPCommandBars::SetToolBarFocus(CXTPToolBar* pCommandBar, BOOL bNext /*= FALSE*/)
{
	if (!pCommandBar)
		return FALSE;

	if (bNext)
	{
		if (GetCount() < 2)
			return FALSE;

		int nIndex = FindIndex(pCommandBar);
		int nNextIndex = GetNextVisible(nIndex, bNext);
		if (nIndex == nNextIndex || nNextIndex == -1)
			return FALSE;

		pCommandBar = GetAt(nNextIndex);
	}
	ClosePopups();

	ShowKeyboardCues(TRUE);
	m_keyboardTips.nLevel = 1;

	pCommandBar->SetTrackingMode(TRUE, FALSE, TRUE);
	pCommandBar->SetSelected(pCommandBar->GetControls()->GetNext(-1, +1, TRUE, pCommandBar->GetType() == xtpBarTypeMenuBar), TRUE_KEYBOARD_NEXT);

	return TRUE;
}



