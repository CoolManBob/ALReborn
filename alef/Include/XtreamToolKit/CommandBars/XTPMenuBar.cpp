// XTPMenuBar.cpp : implementation of the CXTPMenuBar class.
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
#include "Common/XTPImageManager.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPIntel80Helpers.h"

#include "XTPMenuBar.h"
#include "XTPControls.h"
#include "XTPControl.h"
#include "XTPHookManager.h"
#include "XTPControlButton.h"
#include "XTPControlPopup.h"
#include "XTPPaintManager.h"
#include "XTPCommandBars.h"
#include "XTPMouseManager.h"
#include "XTPPopupBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TID_REFRESH 12223

#ifndef OIC_WINLOGO
#define OIC_WINLOGO         32517
#endif


CXTPMenuBarMDIMenuInfo::CXTPMenuBarMDIMenuInfo(CXTPCommandBars* pCommandBars, UINT nIDResource)
{
	ASSERT(pCommandBars);
	ASSERT(nIDResource != 0);

	m_pControls = new CXTPControls;
	m_pControls->SetCommandBars(pCommandBars);

	m_nIDResource = nIDResource;
	m_bChanged = FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(nIDResource);

	#pragma warning(disable:4510)
	#pragma warning(disable:4610)

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);

		USES_PROTECTED_ACCESS(CXTPMenuBarMDIMenuInfo, CDocTemplate, UINT, m_nIDResource);
		UINT nID = PROTECTED_ACCESS(CDocTemplate, pDocTemplate, m_nIDResource);

		if (nIDResource == nID)
		{
			pDocTemplate->GetDocString(m_strTitle, CDocTemplate::docName);
			pDocTemplate->GetDocString(m_strDescription, CDocTemplate::regFileTypeName);
			break;
		}
	}
}

CXTPMenuBarMDIMenuInfo::~CXTPMenuBarMDIMenuInfo()
{
	m_pControls->InternalRelease();
}

//////////////////////////////////////////////////////////////////////////

CXTPMenuBarMDIMenus::CXTPMenuBarMDIMenus(CXTPMenuBar* pMenuBar)
	: m_pMenuBar(pMenuBar)
{
}

CXTPMenuBarMDIMenus::~CXTPMenuBarMDIMenus()
{
	RemoveAll();
}

void CXTPMenuBarMDIMenus::RemoveAll()
{
	POSITION pos = m_mapMenus.GetStartPosition();
	while (pos)
	{
		UINT nIDCommand;
		CXTPMenuBarMDIMenuInfo* pInfo;
		m_mapMenus.GetNextAssoc(pos, nIDCommand, pInfo);
		pInfo->InternalRelease();
	}
	m_mapMenus.RemoveAll();
}

void CXTPMenuBarMDIMenus::Remove(UINT nIDResource)
{
	CXTPMenuBarMDIMenuInfo* pInfo = Lookup(nIDResource);
	if (pInfo)
	{
		m_mapMenus.RemoveKey(nIDResource);
		pInfo->InternalRelease();
	}
}

void CXTPMenuBarMDIMenus::SetAt(UINT nIDResource, CXTPMenuBarMDIMenuInfo* pInfo)
{
	ASSERT(pInfo);
	ASSERT(!Lookup(nIDResource));

	m_mapMenus.SetAt(nIDResource, pInfo);
}

CXTPMenuBarMDIMenuInfo* CXTPMenuBarMDIMenus::Lookup(UINT nIDResource) const
{
	CXTPMenuBarMDIMenuInfo* pInfo;
	if (!m_mapMenus.Lookup(nIDResource, pInfo))
		return NULL;

	return pInfo;
}

int CXTPMenuBarMDIMenus::GetCount() const
{
	return (int)m_mapMenus.GetCount();
}

POSITION CXTPMenuBarMDIMenus::GetStartPosition() const
{
	return m_mapMenus.GetStartPosition();
}

void CXTPMenuBarMDIMenus::GetNextMenu(POSITION& pos, CXTPMenuBarMDIMenuInfo*& pInfo) const
{
	UINT nIDResource = 0;
	m_mapMenus.GetNextAssoc(pos, nIDResource, pInfo);
}


//////////////////////////////////////////////////////////////////////////

class CXTPMenuBar::CControlMDIButton : public CXTPControlButton
{
public:
	CControlMDIButton()
	{
		m_dwFlags |= xtpFlagRightAlign | xtpFlagSkipFocus | xtpFlagManualUpdate | xtpFlagNoMovable;
	}
	CSize GetSize(CDC* /*pDC*/)
	{
		return CSize(16, 16);
	};
	void SetBeginGroup(BOOL /*bBeginGroup*/)
	{
	}
	void Draw(CDC* pDC);
	void OnExecute()
	{
		HWND hWndChild = ((CXTPMenuBar*)m_pParent)->GetActiveMdiChildWnd();
		ASSERT(hWndChild);

		UINT nId = m_nId == XTP_ID_MENUBAR_CLOSE ? SC_CLOSE :
			m_nId == XTP_ID_MENUBAR_RESTORE ? SC_RESTORE : SC_MINIMIZE;

		::PostMessage(hWndChild, WM_SYSCOMMAND, nId, 0);
	}
};

class CXTPMenuBar::CControlMDISysMenuPopup : public CXTPControlPopup
{
	class CXTPSysMenuPopupBar : public CXTPPopupBar
	{
		void UpdateShortcuts()
		{
		}

	};

public:
	CControlMDISysMenuPopup(HWND hWndActiveChild, HMENU hDocMenu)
	{
		m_dwFlags |= xtpFlagManualUpdate | xtpFlagNoMovable;
		m_controlType = xtpControlPopup;
		m_strCaption = _T("-");
		m_hIcon = 0;

		m_pCommandBar = new CXTPSysMenuPopupBar();
		m_pCommandBar->EnableCustomization(FALSE);

		SetMDISysMenu(hWndActiveChild, hDocMenu);

	}
	void SetMDISysMenu(HWND hWndActiveChild, HMENU hDocMenu)
	{
		m_hDocMenu = hDocMenu;

		HICON hIcon = (HICON)::SendMessage(hWndActiveChild, WM_XTP_GETTABICON, 0, 0);

		if (hIcon == NULL) hIcon = (HICON)::SendMessage(hWndActiveChild, WM_GETICON, ICON_SMALL, 0);
		if (hIcon == NULL) hIcon = (HICON)::SendMessage(hWndActiveChild, WM_GETICON, ICON_BIG, 0);
		if (hIcon == NULL) hIcon = (HICON)(ULONG_PTR)::GetClassLongPtr(hWndActiveChild, GCLP_HICONSM);
		if (hIcon == NULL) hIcon = AfxGetApp()->LoadOEMIcon(OIC_WINLOGO);

		if (m_hIcon)
		{
			DestroyIcon(m_hIcon);
			m_hIcon = 0;
		}

		if (hIcon)
			m_hIcon = CXTPImageManagerIcon::ScaleToFit(hIcon, CXTPImageManagerIcon::GetExtent(hIcon), 16);

		m_pCommandBar->GetControls()->RemoveAll();

		CMenu* pMenu = CMenu::FromHandle(hDocMenu);

		BOOL bSeparator = FALSE;
		for (int i = 0; i < (int)pMenu->GetMenuItemCount(); i++)
		{
			UINT nItemID = pMenu->GetMenuItemID(i);
			if (nItemID == 0)
				bSeparator = TRUE;
			else
			{
				CXTPControl* pControl = NULL;
				if (nItemID >= 0xF000 && nItemID < 0xF1F0)
				{
					pControl = (CXTPControl*)CXTPControlButton::CreateObject();
					pControl->SetFlags(xtpFlagManualUpdate);
					m_pCommandBar->GetControls()->Add(pControl, nItemID);

					CString strCaption;
					pMenu->GetMenuString(i, strCaption, MF_BYPOSITION);
					pControl->SetCaption(strCaption);

					CString strDescription;
					strDescription.LoadString(ID_COMMAND_FROM_SC(nItemID));
					pControl->SetDescription(strDescription);

					pControl->SetItemDefault((int)::GetMenuDefaultItem(pMenu->m_hMenu, TRUE, GMDI_USEDISABLED) == i);


					pControl->SetEnabled(!(pMenu->GetMenuState (i, MF_BYPOSITION) & MF_DISABLED));
				}
				else
				{
					pControl = m_pCommandBar->GetControls()->AddMenuItem(pMenu, i);

				}

				if (pControl && bSeparator)
				{
					pControl->SetBeginGroup(TRUE);
					bSeparator = FALSE;
				}
			}
		}


	}
	~CControlMDISysMenuPopup()
	{
		if (m_hIcon) DestroyIcon(m_hIcon);
	}
	CSize GetSize(CDC* /*pDC*/)
	{
		return CSize(16, 16);
	};
	void Draw(CDC* pDC)
	{
		if (m_hIcon)
		{
			pDC->DrawState (GetRect().TopLeft(), CSize(16, 16), m_hIcon, (UINT)DSS_NORMAL, HBRUSH(0));
		}
	}
	void UpdateShadow()
	{

	}
	void AdjustExcludeRect(CRect& rc, BOOL /*bVertical*/)
	{
		rc.InflateRect(1, 1);
	}

public:
	HMENU m_hDocMenu;
	HICON m_hIcon;
};

void CXTPMenuBar::CControlMDIButton::Draw(CDC* pDC)
{
	CFont font;
	font.CreatePointFont(MulDiv(80, 96, pDC->GetDeviceCaps(LOGPIXELSX)), _T("Marlett"));
	CFont* pFont = pDC->SelectObject(&font);
	GetPaintManager()->DrawControlEntry(pDC, this);
	pDC->SetTextColor(GetPaintManager()->GetControlTextColor(this));
	pDC->DrawText(m_strParameter, m_rcControl, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	pDC->SelectObject(pFont);
}


IMPLEMENT_XTP_COMMANDBAR(CXTPMenuBar, CXTPToolBar)

CXTPMenuBar::CXTPMenuBar()
{
	m_barType = xtpBarTypeMenuBar;
	m_bMdiApp = FALSE;
	m_hWndActiveChild = 0;
	m_nIDResource = 0;
	m_nIDResourceCurrent = 0;

	m_bDirtyMenu = FALSE;
	m_hMenuDefault = NULL;
	m_bShowExpandButton = FALSE;

	m_bCloseable = FALSE;

	m_nBarID = XTP_IDR_MENUBAR;
	m_dwFlags = xtpFlagAlignAny | xtpFlagFloating | xtpFlagStretched;

	m_pMDIMenus = new CXTPMenuBarMDIMenus(this);

	EnableAutomation();
}

CXTPMenuBar::~CXTPMenuBar()
{
	FreeMDIMenus();

	CMDTARGET_RELEASE(m_pMDIMenus);

	if (m_hMenuDefault)
	{
		DestroyMenu(m_hMenuDefault);
	}
}


void CXTPMenuBar::SetupHook()
{
	m_bMdiApp = GetSite()->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd));


	CWnd* pSite = GetSite();
	XTPHookManager()->SetHook(pSite->GetSafeHwnd(), this);

	CMDIFrameWnd* pMDIFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, pSite);
	if (pMDIFrame)
	{
		XTPHookManager()->SetHook(pMDIFrame->m_hWndMDIClient, this);
	}
}

HWND CXTPMenuBar::GetActiveMdiChildWnd(BOOL* bMaximized)
{
	BOOL bMax = FALSE;
	if (!m_bMdiApp) return NULL;
	CMDIFrameWnd * pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, GetSite());
	ASSERT_VALID(pFrame);
	HWND hWndMdiArea = pFrame->m_hWndMDIClient;

	ASSERT(hWndMdiArea && ::IsWindow(hWndMdiArea));

	HWND hWnd = (HWND) ::SendMessage(hWndMdiArea, WM_MDIGETACTIVE, 0, (LPARAM)&bMax);
	ASSERT(hWnd == NULL || ::IsWindow(hWnd));
	if (bMaximized) *bMaximized = bMax;
	return hWnd;
}


BOOL CXTPMenuBar::IsOleDocumentActive(HWND hWndActiveChild) const
{
#if _MSC_VER >= 1200 // MFC 6.0
	CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, CWnd::FromHandle(hWndActiveChild));

	COleDocument* pActiveOleDoc =  DYNAMIC_DOWNCAST(COleDocument,
		pFrame && pFrame->GetActiveView() ? pFrame->GetActiveView()->GetDocument() : NULL);

	if (!pActiveOleDoc)
		return FALSE;

	COleDocObjectItem* pItem = DYNAMIC_DOWNCAST(COleDocObjectItem,
		pActiveOleDoc->GetInPlaceActiveItem(pFrame->GetActiveView()));

	return  pItem != NULL;
#else
	UNREFERENCED_PARAMETER(hWndActiveChild);
	return FALSE;
#endif
}

void CXTPMenuBar::SyncActiveMdiChild()
{
	HWND hWndActiveChild = GetActiveMdiChildWnd();
	if (m_hWndActiveChild == hWndActiveChild)
		return;

	if (IsOleDocumentActive(hWndActiveChild))
		return;

	if (m_hWndActiveChild != NULL)
		XTPHookManager()->RemoveHook(m_hWndActiveChild, this);
	m_hWndActiveChild = hWndActiveChild;
	if (m_hWndActiveChild != NULL)
		XTPHookManager()->SetHook(m_hWndActiveChild, this);
}

void CXTPMenuBar::RefreshMenu()
{
	SyncActiveMdiChild();

	if (IsVisible())
	{
		RefreshSysButtons();
	}
}


void CXTPMenuBar::AddSysButton(CXTPControl* pControl, int nId, LPCTSTR strParameter, int nBefore /*= -1*/)
{
	m_pControls->Add(pControl, nId, strParameter, nBefore, TRUE);
}

void CXTPMenuBar::RefreshSysButtons()
{
	BOOL bMax = FALSE;
	HWND hWndActiveChild = GetActiveMdiChildWnd(&bMax);
	DWORD dwStyle = hWndActiveChild ? GetWindowLong(hWndActiveChild, GWL_STYLE) : 0;
	CXTPControl*  pButton;


	pButton = m_pControls->FindControl(XTP_ID_MENUBAR_SYSMENU);

	if (bMax && (m_dwFlags & xtpFlagAddMDISysPopup))
	{
		HMENU hDocMenuButton = pButton ? ((CControlMDISysMenuPopup*)pButton)->m_hDocMenu : 0;

		HMENU hDocMenu =
			::GetSystemMenu(hWndActiveChild, FALSE);
		if (hDocMenu && ::IsMenu(hDocMenu))
		{
			if (hDocMenuButton != hDocMenu)
			{
				if (pButton)
				{
					((CControlMDISysMenuPopup*)pButton)->SetMDISysMenu(hWndActiveChild, hDocMenu);
					DelayRedraw();
				}
				else
				{
					AddSysButton(new CControlMDISysMenuPopup(hWndActiveChild, hDocMenu), XTP_ID_MENUBAR_SYSMENU, _T(""), 0);
				}
			}
		}
		else
		{
			if (pButton) m_pControls->Remove(pButton);
		}
	}
	else if (pButton) m_pControls->Remove(pButton);


	pButton = (CXTPControlButton*)m_pControls->FindControl(XTP_ID_MENUBAR_MINIMIZE);
	if (!pButton && bMax && (dwStyle & WS_MINIMIZEBOX) && (!(m_dwFlags & xtpFlagHideMinimizeBox))) AddSysButton(new CControlMDIButton(), XTP_ID_MENUBAR_MINIMIZE, _T("0"));
	else if (pButton && !bMax) m_pControls->Remove(pButton);

	pButton = (CXTPControlButton*)m_pControls->FindControl(XTP_ID_MENUBAR_RESTORE);
	if (!pButton && bMax && (dwStyle & WS_MAXIMIZEBOX) && (!(m_dwFlags & xtpFlagHideMaximizeBox))) AddSysButton(new CControlMDIButton(), XTP_ID_MENUBAR_RESTORE, _T("2"));
	else if (pButton && !bMax) m_pControls->Remove(pButton);

	pButton = (CXTPControlButton*)m_pControls->FindControl(XTP_ID_MENUBAR_CLOSE);
	if (!pButton && bMax && (!(m_dwFlags & xtpFlagHideClose))) AddSysButton(new CControlMDIButton(), XTP_ID_MENUBAR_CLOSE, _T("r"));
	else if (pButton && !bMax) m_pControls->Remove(pButton);
}

void CXTPMenuBar::FreeMDIMenus()
{
	m_pMDIMenus->RemoveAll();
}

BOOL CXTPMenuBar::IsMDIMenusChanged()
{
	m_bDirtyMenu = FALSE;

	if (m_pMDIMenus->GetCount() > 0)
	{
		SwitchMDIMenu(GetDefaultMenuResource());
		m_bDirtyMenu = TRUE;
	}

	BOOL bChanged = FALSE;

	POSITION pos = m_pMDIMenus->GetStartPosition();
	while (pos)
	{
		CXTPMenuBarMDIMenuInfo* pInfo;
		m_pMDIMenus->GetNextMenu(pos, pInfo);

		pInfo->m_bChanged = pInfo->m_pControls->IsChanged();
		bChanged = bChanged || pInfo->m_bChanged;
	}
	return bChanged;
}


CXTPMenuBarMDIMenuInfo* CXTPMenuBar::AddMDIMenu(UINT nIDResource)
{
	CXTPMenuBarMDIMenuInfo* pInfo = m_pMDIMenus->Lookup(nIDResource);
	if (pInfo)
		return pInfo;

	pInfo = new CXTPMenuBarMDIMenuInfo(GetCommandBars(), nIDResource);
	m_pMDIMenus->SetAt(nIDResource, pInfo);

	CMenu menu;
	VERIFY(menu.LoadMenu(nIDResource));

	pInfo->m_pControls->LoadMenu(&menu);
	pInfo->m_pControls->CreateOriginalControls();

	return pInfo;
}

void CXTPMenuBar::SwitchActiveMenu()
{
	CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, GetSite());
	if (pFrame && pFrame->GetSafeHwnd()) pFrame->OnUpdateFrameMenu(0);
	m_bDirtyMenu = FALSE;
}

void CXTPMenuBar::SwitchMDIMenu(UINT nIDResource, CMenu* pMenu)
{
	if ((nIDResource == 0) || (m_nIDResourceCurrent == 0) || (m_nIDResourceCurrent == nIDResource))
		return;

	if (GetCommandBars()->IsCustomizeAvail() || m_pMDIMenus->Lookup(nIDResource))
	{
		if (pMenu == NULL && !m_pMDIMenus->Lookup(nIDResource))
			return;

		CXTPMenuBarMDIMenuInfo* pInfo = m_pMDIMenus->Lookup(m_nIDResourceCurrent);
		if (pInfo)
		{
			ASSERT(pInfo->m_pControls == m_pControls);
		}
		else
		{
			pInfo = new CXTPMenuBarMDIMenuInfo(GetCommandBars(), m_nIDResourceCurrent);
			pInfo->m_pControls->InternalRelease();

			pInfo->m_pControls = m_pControls;
			m_pControls->InternalAddRef();

			m_pMDIMenus->SetAt(m_nIDResourceCurrent, pInfo);
		}

		m_pControls->SetParent(0);
		m_pControls->InternalRelease();
		m_pControls = NULL;

		pInfo = m_pMDIMenus->Lookup(nIDResource);
		if (!pInfo)
		{
			pInfo = new CXTPMenuBarMDIMenuInfo(GetCommandBars(), nIDResource);
			m_pControls = pInfo->m_pControls;
			m_pControls->InternalAddRef();
			m_pControls->SetParent(this);

			ASSERT(pMenu);
			if (pMenu)
			{
				LoadMenu(pMenu);
			}
			m_pControls->CreateOriginalControls();

			ASSERT(nIDResource != m_nIDResource);

			m_pMDIMenus->SetAt(nIDResource, pInfo);
		}
		else
		{
			m_pControls = pInfo->m_pControls;
			m_pControls->SetParent(this);
			m_pControls->InternalAddRef();

			if (m_pControls->GetOriginalControls() == 0 && pMenu)
			{
				CXTPOriginalControls* pOriginalControls = new CXTPOriginalControls();
				pOriginalControls->LoadMenu(pMenu);
				m_pControls->SetOriginalControls(pOriginalControls);
			}

			if (nIDResource == m_nIDResource)
			{
				m_pMDIMenus->Remove(nIDResource);
			}
		}
	}
	else if (pMenu)
	{
		LoadMenu(pMenu);
	}

	OnRecalcLayout();

	m_nIDResourceCurrent = nIDResource;
}

void CXTPMenuBar::SwitchMDIMenu(HMENU hMenu)
{
	UINT nIDResource = GetActiveDocTemplate();

	if (hMenu == m_hMenuDefault)
	{
		nIDResource = m_nIDResource;
	}

	if ((nIDResource == 0) || (m_nIDResourceCurrent == 0) || (m_nIDResourceCurrent == nIDResource))
		return;

	CMenu* pMenu = CMenu::FromHandle(hMenu);
	if (!pMenu || pMenu->GetMenuItemCount() == 0)
		return;

	SwitchMDIMenu(nIDResource, pMenu);
}

int CXTPMenuBar::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{

	switch (nMessage)
	{
		case WM_SYSCOMMAND:
			if (IsWindowVisible() && !XTPMouseManager()->IsMouseLocked() && (GetKeyState(VK_SHIFT) >= 0) && !IsCustomizeMode())
			if (((wParam & 0xFFF0) == SC_KEYMENU) && (lParam == 0))
			{
				if (PreviewAccel((UINT)VK_MENU))
					return TRUE;

				CXTPCommandBars* pCommandBars = GetCommandBars();
				if (pCommandBars)
				{
					pCommandBars->SetToolBarFocus(this);
				}
				return TRUE;
			}
			break;

		case WM_MDISETMENU:
			if (IsVisible())
			{
				if (IsCustomizeMode())
				{
					GetSite()->SetMenu(0);
					wParam = 0;
					return TRUE;
				}

				BOOL bAccepMenus = ((m_dwFlags & xtpFlagIgnoreSetMenuMessage) == 0) && (wParam || m_hMenuDefault);

				if (bAccepMenus)
				{
					SwitchMDIMenu(wParam ? (HMENU)wParam : m_hMenuDefault);
				}

				GetSite()->SetMenu(0);
				RefreshMenu();

				wParam = 0;
				return TRUE;
			}
			else if (GetSite()->GetMenu() == 0)
			{
				wParam = 0;
				return TRUE;

			}
			break;

		case WM_MDICREATE:
		case WM_MDIDESTROY:
		case WM_MDIACTIVATE:
			PostMessage(WM_TIMER, TID_REFRESH, 0);
			break;

		case WM_SIZE:
		case WM_WINDOWPOSCHANGED:
			if (hWnd == m_hWndActiveChild)
				PostMessage(WM_TIMER, TID_REFRESH, 0);
			break;

	}
	return CXTPToolBar::OnHookMessage(hWnd, nMessage, wParam, lParam, lResult);
}

BOOL CXTPMenuBar::LoadMenuBar(UINT nIDResource)
{
	ASSERT(m_pControls->GetCount() == 0);

	if (nIDResource != 0)
	{
		CMenu menu;
		if (!menu.LoadMenu(nIDResource))
			return FALSE;

		if (!LoadMenu(&menu))
			return FALSE;

		m_nIDResourceCurrent = m_nIDResource = nIDResource;
	}


	SetupHook();

	UINT nIDMenuBarIcons[] =
	{
		SC_RESTORE, SC_MINIMIZE, SC_MAXIMIZE, SC_CLOSE
	};

	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars)
		pCommandBars->GetImageManager()->SetIcons(XTP_IDB_MENUBAR_ICONS, nIDMenuBarIcons, 4, CSize(16, 16));

	m_hMenuDefault = ::GetMenu(GetSite()->GetSafeHwnd());
	GetSite()->SetMenu(0);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CXTPMenuBar, CXTPToolBar)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CXTPMenuBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TID_REFRESH)
		RefreshMenu();

	CXTPToolBar::OnTimer(nIDEvent);
}

void CXTPMenuBar::Reset(BOOL bShowWarningMessage)
{
	if (GetControls()->GetOriginalControls() != NULL)
	{
		CXTPToolBar::Reset(bShowWarningMessage);
	}
	else if (((m_pMDIMenus->GetCount() > 0) || (GetControls()->GetOriginalControls() == NULL)) && m_nIDResourceCurrent)
	{
		if (bShowWarningMessage)
		{
			CString strMessage, strTitle = GetTitle(), strReset;

			VERIFY(XTPResourceManager()->LoadString(&strReset, XTP_IDS_CONFIRM_RESET));
			strMessage.Format(strReset, (LPCTSTR)strTitle);

			CXTPPushRoutingFrame push(GetFrameSite());

			if (AfxMessageBox(strMessage, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
				return;
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(m_nIDResourceCurrent));
		LoadMenu(&menu);

		CWnd* pWnd = GetSite();
		if (pWnd && ::IsWindow(pWnd->m_hWnd)) pWnd->SendMessage(WM_XTP_CUSTOMIZATION_RESETTOOLBAR, 0, (LPARAM)this);
	}

	RefreshMenu();
	OnInitialUpdate();
	OnRecalcLayout();
}

void CXTPMenuBar::Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive)
{
	CXTPToolBar::Copy(pCommandBar, bRecursive);

	m_nIDResource = ((CXTPMenuBar*)pCommandBar)->m_nIDResource;

	POSITION pos = ((CXTPMenuBar*)pCommandBar)->m_pMDIMenus->GetStartPosition();
	while (pos)
	{
		CXTPMenuBarMDIMenuInfo* pInfo;
		((CXTPMenuBar*)pCommandBar)->m_pMDIMenus->GetNextMenu(pos, pInfo);

		CXTPMenuBarMDIMenuInfo* pInfoOrig = m_pMDIMenus->Lookup(pInfo->m_nIDResource);
		if (pInfoOrig)
		{
			pInfoOrig->m_pControls->InternalRelease();
			pInfoOrig->m_pControls = pInfo->m_pControls;
			pInfoOrig->m_pControls->InternalAddRef();
		}
		else
		{
			m_pMDIMenus->SetAt(pInfo->m_nIDResource, pInfo);
			pInfo->InternalAddRef();
		}
	}

}

BOOL CXTPMenuBar::ShouldSerializeBar()
{
	if (!IsCustomizable())
		return FALSE;

	BOOL bChanged = IsMDIMenusChanged();
	if (bChanged)
		return TRUE;

	if ((GetControls()->GetOriginalControls() && !GetControls()->IsChanged()))
		return FALSE;

	return TRUE;
}


