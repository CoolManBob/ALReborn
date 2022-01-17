// XTPRibbonBar.cpp : implementation file
//
// This file is a part of the XTREME RIBBON MFC class library.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPPropExchange.h"
#include "Common/XTPToolTipContext.h"

#include "CommandBars/resource.h"
#include "CommandBars/XTPShortcutManager.h"
#include "CommandBars/XTPCommandBars.h"
#include "CommandBars/XTPPopupBar.h"
#include "CommandBars/XTPControlExt.h"
#include "CommandBars/XTPMouseManager.h"
#include "CommandBars/XTPControlPopup.h"
#include "CommandBars/XTPControlButton.h"
#include "CommandBars/XTPOffice2007FrameHook.h"

#include "XTPRibbonTab.h"
#include "XTPRibbonTheme.h"
#include "XTPRibbonQuickAccessControls.h"
#include "XTPRibbonGroups.h"
#include "XTPRibbonGroup.h"
#include "XTPRibbonControlTab.h"
#include "XTPRibbonBar.h"
#include "XTPRibbonSystemButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_XTP_COMMANDBAR(CXTPRibbonBar, CXTPMenuBar)

#define IDSYS_SCROLL 17821

class CXTPRibbonBar::CControlQuickAccessCommand : public CXTPControlButton
{
public:
	CControlQuickAccessCommand(CXTPControls* pQuickAccessControls, CXTPControl* pControlOriginal)
	{
		m_pControlOriginal = pControlOriginal;
		m_pQuickAccessControls = pQuickAccessControls;
		m_pQuickAccessControl = 0;
	}

	void SetQuickAccessControl(CXTPControl* pQuickAccessControl)
	{
		m_pQuickAccessControl = pQuickAccessControl;
		SetChecked(TRUE);
	}

	void OnExecute()
	{
		CXTPControlButton::OnExecute();

		if (m_pQuickAccessControl)
		{
			m_pQuickAccessControls->Remove(m_pQuickAccessControl);
		}
		else
		{
			CXTPControl* pControl = m_pQuickAccessControls->AddClone(m_pControlOriginal, -1, TRUE);
			pControl->SetHideFlag(xtpHideCustomize, FALSE);
			pControl->DelayLayoutParent();
		}
	}

protected:
	CXTPControl* m_pControlOriginal;
	CXTPControl* m_pQuickAccessControl;
	CXTPControls* m_pQuickAccessControls;
};

class CXTPRibbonScrollableBar::CControlGroupsScroll : public CXTPControlButton
{
public:
	CControlGroupsScroll(BOOL bScrollLeft)
	{
		SetFlags(xtpFlagSkipFocus | xtpFlagNoMovable);
		m_bScrollLeft = bScrollLeft;
	}

	void Draw(CDC* pDC)
	{
		((CXTPRibbonTheme*)GetParent()->GetPaintManager())->DrawRibbonScrollButton(pDC, this, m_bScrollLeft);
	}

	virtual void OnClick(BOOL /*bKeyboard  = FALSE */, CPoint /*pt  = CPoint */)
	{
		if (GetKeyState(VK_LBUTTON) >= 0)
			return;

		m_bPressed = TRUE;

		CXTPRibbonScrollableBar* pScrollableBar = GetScrollableBar(m_pParent);

		pScrollableBar->OnGroupsScroll(m_bScrollLeft);

		int cmsTimer = GetDoubleClickTime() * 4 / 5;
		m_pParent->SetTimer(IDSYS_SCROLL, cmsTimer, NULL);
		BOOL bFirst = TRUE;

		m_pParent->SetCapture();

		while (::GetCapture() == m_pParent->GetSafeHwnd() && IsVisible())
		{
			MSG msg;

			if (!::GetMessage(&msg, NULL, 0, 0))
			{
				AfxPostQuitMessage((int)msg.wParam);
				break;
			}

			if (msg.message == WM_TIMER && msg.wParam == IDSYS_SCROLL)
			{
				pScrollableBar->OnGroupsScroll(m_bScrollLeft);

				if (bFirst)
				{
					cmsTimer = GetDoubleClickTime() / 10;
					m_pParent->SetTimer(IDSYS_SCROLL, cmsTimer, NULL);
				}
				bFirst = FALSE;
			}
			else if (msg.message == WM_LBUTTONUP)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		ReleaseCapture();
		m_pParent->KillTimer(IDSYS_SCROLL);

		m_bPressed = FALSE;
		if (!IsVisible()) m_bSelected = FALSE;
		RedrawParent(FALSE);
	}

protected:
	BOOL m_bScrollLeft;
};

class CXTPRibbonBar::CControlCaptionButton : public CXTPControlButton
{
public:
	CControlCaptionButton(CXTPOffice2007FrameHook* pFrame)
	{
		m_pFrame = pFrame;
		SetFlags(xtpFlagNoMovable | xtpFlagSkipFocus | xtpFlagManualUpdate);
	}
protected:
	void Draw(CDC* pDC)
	{
		((CXTPRibbonBar*)GetParent())->GetRibbonPaintManager()->
			DrawRibbonFrameCaptionButton(pDC, GetRect(), GetID(), GetSelected(), GetPressed(), m_bEnabled && m_pFrame->IsFrameActive());
	}

	void OnExecute()
	{
		GetParent()->GetSite()->PostMessage(WM_SYSCOMMAND, GetID(), 0);
	}

protected:
	CXTPOffice2007FrameHook* m_pFrame;
};

class CXTPRibbonBarControlQuickAccessPopup: public CXTPControlPopup
{
	DECLARE_XTP_CONTROL(CXTPRibbonBarControlQuickAccessPopup)

public:
	CXTPRibbonBarControlQuickAccessPopup()
	{
		SetFlags(xtpFlagNoMovable | xtpFlagManualUpdate);
	}
	CSize GetSize(CDC* /*pDC*/)
	{
		CXTPRibbonTheme* pPaintManager = (CXTPRibbonTheme*)m_pParent->GetPaintManager();
		return CSize(pPaintManager->GetEditHeight() * 13 / 22, pPaintManager->GetEditHeight());
	}
	void Draw(CDC* pDC)
	{
		((CXTPRibbonBar*)GetParent()->GetRootParent())->GetRibbonPaintManager()->DrawRibbonQuickAccessButton(pDC, this);
	}

	BOOL OnSetPopup(BOOL bPopup)
	{
		if (bPopup)
		{
			if (m_pCommandBar)
				m_pCommandBar->InternalRelease();

			CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetParent()->GetRootParent();

			m_pCommandBar = pRibbonBar->CreateContextMenu(this);
		}

		return CXTPControlPopup::OnSetPopup(bPopup);
	}

};

IMPLEMENT_XTP_CONTROL(CXTPRibbonBarControlQuickAccessPopup, CXTPControlPopup)

class CXTPRibbonBar::CMorePopupToolBar : public CXTPPopupToolBar
{
public:
	CMorePopupToolBar(CXTPRibbonBar* pRibbonBar)
	{
		m_rcBorders = CRect(3, 3, 3, 3);
		m_pRibbonBar = pRibbonBar;
	}

	void FillCommandBarEntry(CDC* pDC)
	{
		CXTPRibbonTheme* pPaintManager = (CXTPRibbonTheme*)GetPaintManager();
		pPaintManager->FillMorePopupToolBarEntry(pDC, this);
	}

	CSize GetIconSize() const
	{
		return m_pRibbonBar->GetIconSize();
	}
	CSize GetButtonSize() const
	{
		return m_pRibbonBar->GetButtonSize();
	}
protected:
	CXTPRibbonBar* m_pRibbonBar;
};

class CXTPRibbonBar::CControlQuickAccessMorePopup: public CXTPControlPopup
{
public:
	CControlQuickAccessMorePopup()
	{
		SetFlags(xtpFlagNoMovable | xtpFlagManualUpdate);
	}
	CSize GetSize(CDC* /*pDC*/)
	{
		CXTPRibbonTheme* pPaintManager = (CXTPRibbonTheme*)m_pParent->GetPaintManager();
		return CSize(pPaintManager->GetEditHeight() * 13 / 22, pPaintManager->GetEditHeight());
	}
	void Draw(CDC* pDC)
	{
		((CXTPRibbonBar*)GetParent())->GetRibbonPaintManager()->DrawRibbonQuickAccessMoreButton(pDC, this);
	}

	BOOL OnSetPopup(BOOL bPopup)
	{
		if (bPopup)
		{
			if (m_pCommandBar)
				m_pCommandBar->InternalRelease();

			m_pCommandBar = ((CXTPRibbonBar*)GetParent())->CreateMoreQuickAccessContextMenu();
		}

		return CXTPControlPopup::OnSetPopup(bPopup);
	}

};

void CXTPRibbonScrollableBar::InitScrollableBar(CXTPCommandBar* pParent)
{
	m_pControlScrollGroupsRight = pParent->GetControls()->Add(new CControlGroupsScroll(FALSE));
	m_pControlScrollGroupsLeft = pParent->GetControls()->Add(new CControlGroupsScroll(TRUE));
	m_nGroupsScrollPos = 0;
	m_pParent = pParent;

}

/////////////////////////////////////////////////////////////////////////////
// CXTPRibbonBar

CXTPRibbonBar::CXTPRibbonBar()
{
	m_pQuickAccessControls = new CXTPRibbonQuickAccessControls;
	m_pQuickAccessControls->SetParent(this);

	m_pControls->InternalRelease();
	m_pControls = new CXTPRibbonControls;
	m_pControls->SetParent(this);

	m_bShowCaptionAlways = TRUE;

	m_pHighlightedGroup = NULL;

	m_dwStyle = 0;
	m_rcTabControl.SetRectEmpty();
	m_rcGroups.SetRectEmpty();
	m_rcCaption.SetRectEmpty();
	m_rcCaptionText.SetRectEmpty();
	m_rcHeader.SetRectEmpty();

	InitScrollableBar(this);

	m_pControlQuickAccess = m_pControls->Add(new CXTPRibbonBarControlQuickAccessPopup, XTP_ID_RIBBONCONTROLQUICKACCESS);
	m_pControlQuickAccessMore = m_pControls->Add(new CControlQuickAccessMorePopup, XTP_ID_RIBBONCONTROLQUICKACCESSMORE);

	m_pControlTab = (CXTPRibbonControlTab*)m_pControls->Add(new CXTPRibbonControlTab, XTP_ID_RIBBONCONTROLTAB);


	m_barType = xtpBarTypeRibbon;

	m_dwFlags = xtpFlagIgnoreSetMenuMessage | xtpFlagStretched;

	m_bCustomizable = FALSE;
	m_bGroupsVisible = TRUE;
	m_bTabsVisible = TRUE;

	m_nMinVisibleWidth = 250;
	m_bTrackOnHover = FALSE;

	m_bMinimized = FALSE;

	m_pFrameHook = NULL;
	m_pControlSystemButton = NULL;

	m_bRibbonBarVisible = TRUE;
	m_bGroupReducedChanged = FALSE;

	m_bShowQuickAccessBelow = FALSE;
	m_bShowQuickAccess = TRUE;

	m_pContextHeaders = new CXTPRibbonTabContextHeaders;

	m_bMultiLine = TRUE;

	m_bAllowQuickAccessDuplicates = FALSE;
	m_bAllowQuickAccessCustomization = TRUE;

	m_nGroupsHeight = 0;
	m_nQuickAccessHeight = 0;

	m_nGroupsScrollPos = 0;

	m_bAllowMinimize = TRUE;

	EnableDoubleBuffer();
	EnableAnimation();
}

CXTPRibbonTheme* CXTPRibbonBar::GetRibbonPaintManager() const
{
	CXTPPaintManager* pPaintManager = GetPaintManager();
	ASSERT(pPaintManager->BaseTheme() == xtpThemeRibbon);

	return (CXTPRibbonTheme*)pPaintManager;
}


CXTPRibbonBar::~CXTPRibbonBar()
{
	if (m_pControlTab)
	{
		m_pControlTab->DeleteAllItems();
	}

	if (m_pQuickAccessControls)
	{
		m_pQuickAccessControls->RemoveAll();
	}

	CMDTARGET_RELEASE(m_pQuickAccessControls);
	SAFE_DELETE(m_pFrameHook);
	SAFE_DELETE(m_pContextHeaders);
}

void CXTPRibbonBar::OnRemoved()
{
	SAFE_DELETE(m_pFrameHook);

	if (m_pQuickAccessControls)
	{
		m_pQuickAccessControls->RemoveAll();
	}
	if (m_pControlTab)
	{
		m_pControlTab->DeleteAllItems();
	}

	CXTPMenuBar::OnRemoved();
}


CXTPControlPopup* CXTPRibbonBar::AddSystemButton(int nID)
{
	if (m_pControlSystemButton)
		return m_pControlSystemButton;

	m_pControlSystemButton = (CXTPControlPopup*)GetControls()->InsertAt(new CXTPRibbonControlSystemButton, 0);

	if (nID > 0) m_pControlSystemButton->SetID(nID);

	CXTPPopupBar* pCommandBar = new CXTPRibbonSystemPopupBar();
	pCommandBar->SetCommandBars(GetCommandBars());

	m_pControlSystemButton->SetCommandBar(pCommandBar);
	pCommandBar->InternalRelease();

	return m_pControlSystemButton;
}

void CXTPRibbonBar::EnableFrameTheme(BOOL bEnable)
{
	CXTPOffice2007FrameHook* pFrameHook = m_pFrameHook;

	if (!bEnable)
	{
		if (pFrameHook)
		{
			m_pControls->Remove(m_pControls->FindControl(SC_CLOSE));
			m_pControls->Remove(m_pControls->FindControl(SC_RESTORE));
			m_pControls->Remove(m_pControls->FindControl(SC_MAXIMIZE));
			m_pControls->Remove(m_pControls->FindControl(SC_MINIMIZE));

			m_pFrameHook = NULL;
			SAFE_DELETE(pFrameHook);

			OnRecalcLayout();
		}
	}
	else
	{
		if (!pFrameHook)
		{
			m_pFrameHook = new CXTPOffice2007FrameHook();
			m_pFrameHook->EnableOffice2007Frame(GetCommandBars());
			ASSERT(m_pFrameHook->m_pRibbonBar == this);
		}
	}
}

BOOL CXTPRibbonBar::IsFrameThemeEnabled() const
{
	return m_pFrameHook != NULL;
}

CXTPRibbonTab* CXTPRibbonBar::GetSelectedTab() const
{
	if (m_pControlTab == NULL)
		return NULL;

	return (CXTPRibbonTab*)m_pControlTab->GetSelectedItem();
}

BOOL CXTPRibbonBar::OnTabChanging(CXTPRibbonTab* pTab)
{
	NMXTPTABCHANGE nm;
	nm.pTab = pTab;

	return (BOOL)m_pControlTab->NotifySite(TCN_SELCHANGING, &nm);
}

void CXTPRibbonBar::OnTabChanged(CXTPRibbonTab* pTab)
{
	NMXTPTABCHANGE nm;
	nm.pTab = pTab;

	m_pControlTab->NotifySite(TCN_SELCHANGE, &nm);
}

int CXTPRibbonBar::GetCurSel() const
{
	return m_pControlTab->GetCurSel();
}

void CXTPRibbonBar::SetCurSel(int nIndex)
{
	m_pControlTab->SetCurSel(nIndex);

}

CXTPRibbonTab* CXTPRibbonBar::InsertTab(int nItem, int nID)
{
	CString strCaption;
	strCaption.LoadString(nID);

	return InsertTab(nItem, strCaption, nID);
}

CXTPRibbonTab* CXTPRibbonBar::InsertTab(int nItem, LPCTSTR lpszCaption, int nID)
{
	CXTPRibbonTab* pItem = (CXTPRibbonTab*)m_pControlTab->AddItem(nItem, new CXTPRibbonTab());
	pItem->m_pRibbonBar = this;
	pItem->m_pParent = this;
	pItem->SetCaption(lpszCaption);
	pItem->SetID(nID);

	if (nItem == 0 && GetCurSel() == -1)
	{
		SetCurSel(0);
	}

	return pItem;
}

CXTPRibbonTab* CXTPRibbonBar::AddTab(int nID)
{
	return InsertTab(m_pControlTab->GetItemCount(), nID);
}

CXTPRibbonTab* CXTPRibbonBar::AddTab(LPCTSTR lpszCaption)
{
	return InsertTab(m_pControlTab->GetItemCount(), lpszCaption, 0);
}

void CXTPRibbonBar::RebuildControls(CXTPRibbonTab* pSelected)
{
	m_nGroupsScrollPos = 0;

	if (IsRibbonMinimized())
		pSelected = NULL;

	for (int i = GetControlCount() - 1; i >= 0; i--)
	{
		CXTPControl* pControl = m_pControls->GetAt(i);
		CXTPRibbonGroup* pGroup = pControl->GetRibbonGroup();

		if ((pGroup != NULL) && (pGroup->GetParentTab() != pSelected || !pGroup->m_bVisible))
		{
			pControl->SetHideFlag(xtpHideRibbonTab, TRUE);
		}
		else
		{
			pControl->SetHideFlag(xtpHideRibbonTab, FALSE);
		}
	}
}


BEGIN_MESSAGE_MAP(CXTPRibbonBar, CXTPMenuBar)
	//{{AFX_MSG_MAP(CXTPRibbonBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND_RANGE(XTP_ID_RIBBONCUSTOMIZE_QUICKACCESSBELOW, XTP_ID_RIBBONCUSTOMIZE_QUICKACCESSABOVE, OnCustomizePlaceQuickAccess)
	ON_MESSAGE(WM_XTP_COMMAND, OnCustomizeCommand)
	ON_WM_NCHITTEST_EX()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPRibbonBar message handlers

int CXTPRibbonBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPMenuBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetupHook();

	return 0;
}

void CXTPRibbonBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CXTPMenuBar::OnShowWindow(bShow, nStatus);

	if (IsFrameThemeEnabled() && bShow != m_bRibbonBarVisible)
	{
		m_bRibbonBarVisible = bShow;
		m_pFrameHook->RecalcFrameLayout();
	}
}

void CXTPRibbonBar::SetVisible(BOOL bVisible)
{
	BOOL bOldVisible = IsVisible();

	CXTPMenuBar::SetVisible(bVisible);

	if (IsFrameThemeEnabled() && bOldVisible != IsVisible())
	{
		m_pFrameHook->RecalcFrameLayout();
	}
}

int CXTPRibbonBar::GetQuickAccessHeight() const
{
	return m_nQuickAccessHeight;

}

int CXTPRibbonBar::CalcQuickAccessHeight()
{
	CClientDC dc(this);
	CXTPFontDC font(&dc, GetPaintManager()->GetCommandBarFont(this, FALSE));

	int nQuickAccessHeight = GetButtonSize().cy + 4;

	for (int i = 0; i < m_pQuickAccessControls->GetCount(); i++)
	{
		CXTPControl* pControl = m_pQuickAccessControls->GetAt(i);
		if (!pControl->IsVisible())
			continue;

		int nHeight = pControl->GetSize(&dc).cy + 4;
		if (nHeight > nQuickAccessHeight)
			nQuickAccessHeight = nHeight;
	}
	return nQuickAccessHeight;
}

CSize CXTPRibbonBar::CalcDockingLayout(int nLength, DWORD dwMode, int /*nWidth*/)
{
	BOOL bRibbonBarVisible = nLength >= m_nMinVisibleWidth;
	m_bGroupReducedChanged = FALSE;

	CXTPWindowRect rc(GetSite());
	if (rc.Height() < m_nMinVisibleWidth)
		bRibbonBarVisible = FALSE;

	if (bRibbonBarVisible != m_bRibbonBarVisible)
	{
		m_bRibbonBarVisible =  bRibbonBarVisible;

		if (IsFrameThemeEnabled())
		{
			m_pFrameHook->DelayRecalcFrameLayout();
		}
	}

	if (!bRibbonBarVisible)
	{
		return CSize(nLength, 0);
	}

	int nCaptionHeight = GetCaptionHeight();
	int nHeight = nCaptionHeight + GetTabsHeight() + GetGroupsHeight();

	m_nQuickAccessHeight = CalcQuickAccessHeight();

	if (IsQuickAccessBelowRibbon())
	{
		nHeight += m_nQuickAccessHeight + 1;
	}


	if (!IsGroupsVisible() && !IsQuickAccessBelowRibbon())
	{
		nHeight += 2;
	}

	if (dwMode & LM_COMMIT)
	{
		Reposition(nLength, nHeight);
	}



	if (m_bGroupReducedChanged)
	{
		OnUpdateCmdUI();
		m_bGroupReducedChanged = FALSE;
	}

	if ((m_nIdleFlags & xtpIdleLayout) && (dwMode & LM_COMMIT))
	{
		m_nIdleFlags &= ~xtpIdleLayout;
		Redraw();
	}

	return CSize(nLength, nHeight);
}

int CXTPRibbonBar::GetTabsHeight() const
{
	return m_bTabsVisible ? GetRibbonPaintManager()->m_nTabsHeight : 2;
}

int CXTPRibbonBar::CalcGroupsHeight()
{
	if (m_nGroupsHeight > 0)
		return m_nGroupsHeight;

	int nHeight = GetRibbonPaintManager()->GetEditHeight() * 3;
	int nCaptionHeight = GetRibbonPaintManager()->GetGroupCaptionHeight();

	return nHeight + nCaptionHeight + 10;
}

int CXTPRibbonBar::GetGroupsHeight()
{
	return IsGroupsVisible() ? CalcGroupsHeight() : 0;
}

BOOL CXTPRibbonBar::IsGroupsVisible() const
{
	return m_bGroupsVisible && GetSelectedTab() != NULL && !m_bMinimized;
}

void CXTPRibbonBar::SetGroupsVisible(BOOL bVisible)
{
	if (bVisible != m_bGroupsVisible)
	{
		m_bGroupsVisible = bVisible;
		OnRecalcLayout();
	}
}

BOOL CXTPRibbonBar::IsTabsVisible() const
{
	return m_bTabsVisible;
}

void CXTPRibbonBar::SetTabsVisible(BOOL bVisible)
{
	if (bVisible != m_bTabsVisible)
	{
		m_bTabsVisible = bVisible;
		OnRecalcLayout();
	}
}

BOOL CXTPRibbonBar::IsQuickAccessControl(CXTPControl* pControl) const
{
	if (pControl == m_pControlQuickAccess)
		return TRUE;

	if (pControl == m_pControlQuickAccessMore)
		return TRUE;

	if (pControl->GetRibbonGroup())
		return FALSE;

	if (m_pQuickAccessControls == NULL)
		return FALSE;

	for (int i = 0; i < m_pQuickAccessControls->GetCount(); i++)
	{
		if (m_pQuickAccessControls->GetAt(i) == pControl)
			return TRUE;
	}
	return FALSE;
}

int CXTPRibbonBar::GetCaptionHeight() const
{
	if (!IsFrameThemeEnabled() && !m_bShowCaptionAlways)
		return 0;

	int nBorder =  m_pFrameHook ? m_pFrameHook->GetFrameBorder() : 0;
	int nHeight = GetRibbonPaintManager()->GetRibbonCaptionHeight() + nBorder;

	if (!IsQuickAccessVisible() || IsQuickAccessBelowRibbon())
		return nHeight;

	int nQuickAccessHeight = GetQuickAccessHeight();
	nQuickAccessHeight = max(nQuickAccessHeight, GetTabsHeight() - 2);

	if (nHeight >= nQuickAccessHeight + nBorder)
		return nHeight;

	return nQuickAccessHeight + nBorder;
}

void CXTPRibbonBar::Reposition(int cx, int cy)
{
	CClientDC dc(this);
	int nTabsHeight = GetTabsHeight();
	int nGroupsHeight = GetGroupsHeight();
	BOOL bFrameThemeEnabled = IsFrameThemeEnabled();
	BOOL bShowCaption = bFrameThemeEnabled || m_bShowCaptionAlways;

	int nQuickAccessHeight = m_nQuickAccessHeight;

	int i;

	CRect rcTabControl(2, 0, cx - 2, nTabsHeight);
	CRect rcRibbonGroups(0, nTabsHeight, cx, nTabsHeight + nGroupsHeight);

	int nFrameBorder = 4;

	if (bShowCaption)
	{
		nFrameBorder = m_pFrameHook ? m_pFrameHook->GetFrameBorder() : 0;

		m_rcCaption.SetRect(-nFrameBorder, 0, cx + nFrameBorder, GetCaptionHeight());
		rcTabControl.OffsetRect(0, m_rcCaption.Height());
		rcRibbonGroups.OffsetRect(0, m_rcCaption.Height());

		m_rcCaptionText.SetRect(0, 0, cx, m_rcCaption.bottom);
		m_rcHeader.SetRect(0, 0, cx, m_rcCaption.bottom);

		if (!m_pFrameHook) m_rcCaptionText.SetRectEmpty();
	}
	else
	{
		m_rcCaption.SetRectEmpty();
		m_rcCaptionText.SetRectEmpty();
		m_rcHeader.SetRectEmpty();
	}

	CArray<CXTPControl*, CXTPControl*> arrFreeControls;
	CArray<CXTPControl*, CXTPControl*> arrQuickAccessControls;

	for (i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);

		if (IsQuickAccessControl(pControl))
		{
			arrQuickAccessControls.Add(pControl);
		}
		else
		{
			if (!pControl->IsVisible())
				continue;

			if (pControl->GetRibbonGroup() != NULL)
				continue;

			if (pControl->GetID() >= SC_MINIMIZE && pControl->GetID() <= SC_RESTORE)
				continue;

			if (pControl == m_pControlSystemButton || pControl == m_pControlScrollGroupsLeft ||
				pControl == m_pControlScrollGroupsRight)
				continue;

			if (pControl == m_pControlTab)
				continue;

			arrFreeControls.Add(pControl);
		}
	}

	int nCorner = nQuickAccessHeight + nTabsHeight - 3;

	if (m_bShowQuickAccess && m_bShowQuickAccessBelow)
	{
		m_rcQuickAccess.SetRect(0, rcRibbonGroups.bottom, cx, rcRibbonGroups.bottom + nQuickAccessHeight);
	}
	else
	{
		m_rcQuickAccess.SetRect(3 + nCorner, nFrameBorder - 2, m_rcCaption.right, nFrameBorder - 2 + nQuickAccessHeight - 1);
	}

	int nLeft = 2;
	int nRight = cx - 2;

	if (m_pControlSystemButton)
	{
		CSize szControl(nCorner, nCorner - 1);
		int nTop = m_rcCaption.top + nFrameBorder - 1;

		if (!bShowCaption)
		{
			szControl = m_pControlSystemButton->GetSize(&dc);
			nTop = rcTabControl.top + (nTabsHeight - szControl.cy) / 2;
		}
		else if (!m_bTabsVisible)
		{
			szControl = m_pControlSystemButton->GetSize(&dc);
			nTop = m_rcCaption.top + (m_rcCaption.Height() - szControl.cy) / 2;
			m_rcQuickAccess.left = 3 + szControl.cx + 12;
		}
		nLeft = 0;

		CRect rcControl(nLeft, nTop, nLeft + szControl.cx, nTop + szControl.cy);
		nLeft += szControl.cx;

		m_pControlSystemButton->SetHideWrap(!bShowCaption && !m_bTabsVisible);
		m_pControlSystemButton->SetRect(rcControl);
		m_pControlSystemButton->SetWrap(TRUE);
	}

	for (i = 0; i < (int)arrFreeControls.GetSize(); i++)
	{
		CXTPControl* pControl = arrFreeControls.GetAt(i);

		if (pControl->GetFlags() & xtpFlagRightAlign)
			continue;

		pControl->SetHideFlag(xtpHideDockingPosition, !m_bTabsVisible);
		if (!pControl->IsVisible())
			continue;

		CSize szControl = pControl->GetSize(&dc);
		int nTop = rcTabControl.top + (nTabsHeight - szControl.cy) / 2;

		if (pControl->GetBeginGroup())
			nLeft += 6;

		CRect rcControl(nLeft, nTop, nLeft + szControl.cx, nTop + szControl.cy);
		nLeft += szControl.cx;

		pControl->SetHideWrap(FALSE);
		pControl->SetRect(rcControl);
		pControl->SetRowRect(CRect(2, rcTabControl.top, cy - 2, rcTabControl.bottom));
		pControl->SetWrap(FALSE);
	}

	int nLeftTab = nLeft;
	BOOL bQuickAccessNearTabs = m_bShowQuickAccess && !m_bShowQuickAccessBelow && !bShowCaption;

	if (bQuickAccessNearTabs)
	{
		nLeft += 5;
		m_rcQuickAccess.left = nLeft;
		m_rcQuickAccess.top = rcTabControl.top;
		m_rcQuickAccess.bottom = rcTabControl.bottom;
	}
	else
	{
		nLeft = m_rcQuickAccess.left + (m_bShowQuickAccessBelow ? 2 : 0);
	}

	nRight = m_bShowQuickAccess && m_bShowQuickAccessBelow ? cx - 2 - 13 :
		IsDwmEnabled() ? cx - 115 : cx - m_rcCaption.Height() * 3;

	if (m_pControlQuickAccessMore) m_pControlQuickAccessMore->SetHideFlag(xtpHideGeneric, TRUE);
	BOOL bQuickAccessMoreVisible = FALSE;
	BOOL bQuickAccessFound = FALSE;

	for (i = 0; i < (int)arrQuickAccessControls.GetSize(); i++)
	{
		CXTPControl* pControl = arrQuickAccessControls.GetAt(i);

		if (!m_bShowQuickAccess)
			pControl->SetHideFlag(xtpHideWrap, TRUE);
		else if (pControl != m_pControlQuickAccessMore)
			pControl->SetHideFlag(xtpHideWrap, bQuickAccessMoreVisible);
		if (!bShowCaption && !m_bTabsVisible)
			pControl->SetHideFlag(xtpHideWrap, TRUE);

		if (pControl == m_pControlQuickAccess && bQuickAccessFound &&
			!bQuickAccessNearTabs && !m_bShowQuickAccessBelow)
			nLeft += 12;

		if (!pControl->IsVisible())
			continue;

		BOOL bSpecial = pControl == m_pControlQuickAccessMore || pControl == m_pControlQuickAccess;

		CSize szControl = pControl->GetSize(&dc);
		int nTop = m_rcQuickAccess.top + (m_rcQuickAccess.Height() - szControl.cy + 1) / 2;

		if (!bSpecial && !m_bShowQuickAccessBelow && !bQuickAccessNearTabs)
		{
			nTop += 2;
		}
		if (!bSpecial)
		{
			bQuickAccessFound = TRUE;
		}

		if (!bSpecial && (nLeft + szControl.cx > nRight))
		{
			bQuickAccessMoreVisible = TRUE;
			pControl->SetHideFlag(xtpHideWrap, bQuickAccessMoreVisible);
			m_pControlQuickAccessMore->SetHideFlag(xtpHideGeneric, FALSE);
			continue;
		}

		CRect rcControl(nLeft, nTop, nLeft + szControl.cx, nTop + szControl.cy);

		if (bQuickAccessNearTabs || !bSpecial)
			nLeft += szControl.cx;

		pControl->SetHideWrap(FALSE);
		pControl->SetRect(rcControl);
		pControl->SetRowRect(m_rcQuickAccess);
		pControl->SetWrap(FALSE);
	}
	if (!m_bShowQuickAccessBelow) m_rcQuickAccess.right = nLeft;

	if (!bQuickAccessNearTabs) nLeft = nLeftTab;
	nRight = cx;

	for (i = (int)arrFreeControls.GetSize() - 1; i >= 0; i--)
	{
		CXTPControl* pControl = arrFreeControls.GetAt(i);

		if ((pControl->GetFlags() & xtpFlagRightAlign) == 0)
			continue;

		pControl->SetHideFlag(xtpHideDockingPosition, !m_bTabsVisible);
		if (!pControl->IsVisible())
			continue;

		CSize szControl = pControl->GetSize(&dc);
		int nTop = rcTabControl.top + (nTabsHeight - szControl.cy) / 2;

		CRect rcControl = CRect(nRight - szControl.cx, nTop, nRight, nTop + szControl.cy);
		nRight -= szControl.cx;

		pControl->SetHideWrap(FALSE);
		pControl->SetRect(rcControl);
		pControl->SetRowRect(CRect(2, rcTabControl.top, cy - 2, rcTabControl.bottom));
		pControl->SetWrap(TRUE);
		pControl->SetBeginGroup(FALSE);
	}

	if (!m_bTabsVisible) rcTabControl.bottom = rcTabControl.top;

	if (nLeft + 6 > nRight)
		m_rcTabControl.SetRectEmpty();
	else
		m_rcTabControl = CRect(nLeft, rcTabControl.top, nRight, rcTabControl.bottom);

	if (m_pControlTab)
	{
		m_pControlTab->SetHideFlag(xtpHideGeneric, !m_bTabsVisible);

		m_pControlTab->SetRect(m_rcTabControl);
		GetTabPaintManager()->RepositionTabControl(m_pControlTab, &dc, m_rcTabControl);
	}

	RepositionGroups(&dc, rcRibbonGroups);

	RepositionCaptionButtons();

	if (bShowCaption)
	{
		RepositionContextHeaders();
	}
}



void CXTPRibbonBar::AddCaptionButton(int nId, BOOL bAdd, BOOL bEnabled, CRect& rcCaption)
{
	CXTPControl* pButton = m_pControls->FindControl(nId);

	if (!pButton && bAdd)
	{
		pButton = m_pControls->Add(new CControlCaptionButton(m_pFrameHook), nId, NULL, -1, TRUE);

		CString strCaption;
		CMenu* pMenu = GetSite()->GetSystemMenu(FALSE);
		if (pMenu)
		{
			pMenu->GetMenuString(nId, strCaption, MF_BYCOMMAND);
			int nIndex = strCaption.Find(_T('\t'));
			if (nIndex > 0)
			{
				strCaption = strCaption.Left(nIndex);
			}
		}
		if (pButton->GetAction())
		{
			pButton->GetAction()->SetCaption(_T(""));
			pButton->GetAction()->SetDescription(NULL);
		}

		pButton->SetDescription(NULL);
		pButton->SetCaption(_T(""));
		pButton->SetTooltip(strCaption);
	}
	else if (pButton && !bAdd)
	{
		m_pControls->Remove(pButton);
	}

	if (bAdd)
	{
		CRect rcButton(rcCaption.right - rcCaption.Height(), rcCaption.top, rcCaption.right, rcCaption.bottom);
		pButton->SetRect(rcButton);
		rcCaption.right -= rcButton.Width();
		pButton->SetEnabled(bEnabled);
	}
}

BOOL CXTPRibbonBar::ShrinkContextHeaders(int nLeft, int nRight)
{
	int i, nCount = m_pContextHeaders->GetCount();

	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonTabContextHeader* pHeader =  m_pContextHeaders->GetHeader(i);
		if (pHeader->m_rcRect.left < nLeft)
		{
			pHeader->m_rcRect.left = nLeft;
			if (pHeader->m_rcRect.Width()  < 40)
			{
				pHeader->m_rcRect.right = nLeft + 40;
			}
			nLeft = pHeader->m_rcRect.right;
		}
		else break;
	}
	for (i = nCount - 1; i >= 0; i--)
	{
		CXTPRibbonTabContextHeader* pHeader =  m_pContextHeaders->GetHeader(i);
		if (pHeader->m_rcRect.right > nRight)
		{
			pHeader->m_rcRect.right = nRight;
			if (pHeader->m_rcRect.Width()  < 40)
			{
				pHeader->m_rcRect.left = nRight - 40;
			}
			nRight = pHeader->m_rcRect.left;

			if (nRight < nLeft)
				return FALSE;
		}
		else break;
	}

	return TRUE;
}

BOOL CXTPRibbonBar::IsCaptionVisible() const
{
	return IsFrameThemeEnabled() || m_bShowCaptionAlways;
}

void CXTPRibbonBar::RepositionContextHeaders()
{
	m_pContextHeaders->RemoveAll();

	CXTPRibbonTabContextHeader* pPreviousHeader = NULL;

	if (!IsCaptionVisible())
		return;

	int nCount = GetTabCount();
	if (nCount == 0)
		return;

	if (!IsQuickAccessBelowRibbon() && IsQuickAccessVisible() && !m_pFrameHook)
		m_rcHeader.left = m_rcQuickAccess.right + (m_pControlQuickAccess ? m_pControlQuickAccess->GetRect().Width() + 2 : 12);

	int i;
	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonTab* pTab = GetTab(i);
		pTab->m_pContextHeader = NULL;
		if (!pTab->IsVisible())
			continue;

		if (pTab->GetContextColor() == xtpRibbonTabContextColorNone || pTab->GetContextCaption().IsEmpty())
		{
			pPreviousHeader = NULL;
			continue;
		}
		if (pPreviousHeader && pPreviousHeader->m_color == pTab->GetContextColor() &&
			pPreviousHeader->m_strCpation == pTab->GetContextCaption())
		{
			pPreviousHeader->m_rcRect.right = pTab->GetRect().right;
			pPreviousHeader->m_pLastTab = pTab;
			pTab->m_pContextHeader = pPreviousHeader;
		}
		else
		{
			CXTPRibbonTabContextHeader* pHeader = new CXTPRibbonTabContextHeader(pTab);

			pHeader->m_rcRect.SetRect(pTab->GetRect().left, m_rcCaption.top + 4, pTab->GetRect().right, m_rcCaption.bottom);

			m_pContextHeaders->Add(pHeader);

			pPreviousHeader = pHeader;
		}
	}

	nCount = m_pContextHeaders->GetCount();
	if (nCount == 0)
		return;

	if (!ShrinkContextHeaders(m_rcHeader.left, m_rcHeader.right))
	{
		m_pContextHeaders->RemoveAll();
		return;
	}

	CRect rcHeaders(m_pContextHeaders->GetHeader(0)->m_rcRect.left, m_rcCaption.top + 6,
		m_pContextHeaders->GetHeader(nCount - 1)->m_rcRect.right, m_rcCaption.bottom);

	int nCaptionLength = m_rcCaptionText.Width();
	CRect rcCaptionText(m_rcCaptionText);

	if (CRect().IntersectRect(rcCaptionText, rcHeaders))
	{
		if (rcCaptionText.CenterPoint().x > rcHeaders.CenterPoint().x)
		{
			if (m_rcHeader.right - rcHeaders.right < nCaptionLength)
			{
				if (ShrinkContextHeaders(m_rcHeader.left, m_rcHeader.right - nCaptionLength))
				{
					m_rcCaptionText.right = m_rcHeader.right;
					m_rcCaptionText.left = m_rcCaptionText.right - nCaptionLength ;
				}
				else
				{
					m_pContextHeaders->RemoveAll();
				}
			}
			else
			{
				m_rcCaptionText.left = rcHeaders.right;
				m_rcCaptionText.right = m_rcCaptionText.left + nCaptionLength;
			}
		}
		else
		{
			if (rcHeaders.left - m_rcHeader.left < nCaptionLength)
			{
				if (ShrinkContextHeaders(m_rcHeader.left + nCaptionLength, m_rcHeader.right))
				{
					m_rcCaptionText.left = m_rcHeader.left;
					m_rcCaptionText.right = m_rcHeader.left + nCaptionLength ;
				}
				else
				{
					m_pContextHeaders->RemoveAll();
				}
			}
			else
			{
				m_rcCaptionText.right = rcHeaders.left;
				m_rcCaptionText.left = m_rcCaptionText.right - nCaptionLength;
			}

		}
	}
}

void CXTPRibbonBar::RepositionCaptionButtons()
{
	if (!IsFrameThemeEnabled())
	{
		return;
	}

	CRect rcQuickAccess = m_rcQuickAccess;
	if (m_bShowQuickAccessBelow || !m_bShowQuickAccess)
	{
		m_rcHeader.left += 70;
	}
	else
	{
		rcQuickAccess.right += (m_pControlQuickAccess ? m_pControlQuickAccess->GetRect().Width() + 2 : 12);
		m_rcHeader.left = rcQuickAccess.right;
	}

	CWnd* pSite = GetSite();
	DWORD dwStyle = pSite->GetStyle();
	DWORD dwExStyle = pSite->GetExStyle();
	BOOL bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW;

	int nFrameBorder = m_pFrameHook->GetFrameBorder();

	m_rcHeader.DeflateRect(0, nFrameBorder, 0, 3);

	BOOL bDwmEnabled = m_pFrameHook->IsDwmEnabled();

	BOOL bEnableClose = TRUE;
	BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;
	BOOL bSysMenu = (dwStyle & WS_SYSMENU);
	BOOL bEnabledMaximize = ((dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX);
	BOOL bEnabledMinimize = ((dwStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX);
	BOOL bShowMinMaxButtons = !bToolWindow && bSysMenu && (bEnabledMaximize || bEnabledMinimize);
	BOOL bShowCloseButton = bSysMenu;
	if (bSysMenu && !bToolWindow)
	{
		CMenu* pMenu = GetSite()->GetSystemMenu(FALSE);
		if (pMenu && pMenu->GetMenuState(SC_CLOSE, MF_BYCOMMAND) & MF_DISABLED) bEnableClose = FALSE;
	}
	if (bMaximized && (dwStyle & WS_CHILD))
	{
		bShowMinMaxButtons = bShowCloseButton = FALSE;
	}

	AddCaptionButton(SC_CLOSE, !bDwmEnabled && bShowCloseButton, bEnableClose, m_rcHeader);
	AddCaptionButton(SC_RESTORE, !bDwmEnabled && bShowMinMaxButtons && bMaximized, bEnabledMaximize, m_rcHeader);
	AddCaptionButton(SC_MAXIMIZE, !bDwmEnabled && bShowMinMaxButtons && !bMaximized, bEnabledMaximize, m_rcHeader);
	AddCaptionButton(SC_MINIMIZE, !bDwmEnabled && bShowMinMaxButtons, bEnabledMinimize, m_rcHeader);

	if (bDwmEnabled)
		m_rcHeader.right -= 100;

	m_rcCaptionText.SetRect(m_rcCaptionText.left + 70, m_rcHeader.top, m_rcHeader.right, m_rcHeader.bottom);


	CClientDC dc(this);
	CXTPFontDC font(&dc, &GetRibbonPaintManager()->m_fontFrameCaption);

	GetSite()->GetWindowText(m_strCaptionText);

	int nCaptionLength = dc.GetTextExtent(m_strCaptionText).cx + 20;
	nCaptionLength = min(m_rcCaptionText.Width(), nCaptionLength);

	CRect rcCaptionText(m_rcCaptionText);
	rcCaptionText.left += (rcCaptionText.Width() - nCaptionLength) / 2;
	rcCaptionText.right = rcCaptionText.left + nCaptionLength;

	if (CRect().IntersectRect(rcQuickAccess, rcCaptionText))
	{
		nCaptionLength = min(m_rcHeader.Width(), nCaptionLength);
		rcCaptionText.left = m_rcHeader.left + (m_rcHeader.Width() - nCaptionLength) / 2;
		rcCaptionText.right = rcCaptionText.left + nCaptionLength;
	}

	m_rcCaptionText = rcCaptionText;

}

void CXTPRibbonBar::RepositionGroups(CDC* pDC, CRect rcGroups)
{
	m_rcGroups = rcGroups;

	CXTPRibbonTab* pTab = GetSelectedTab();
	if (!pTab)
		return;

	CXTPRibbonGroups* pGroups = pTab->GetGroups();

	pGroups->CalcDynamicSize(pDC, rcGroups.Width(), LM_COMMIT, CRect(rcGroups.left + 4, rcGroups.top + 3, 6, 3));
}

void CXTPRibbonScrollableBar::ShowScrollableRect(CXTPRibbonGroups* pGroups, CRect rc)
{
	int nGroupsScrollPos = pGroups->GetScrollPos();
	CRect rcGroups(GetGroupsRect());

	if (rc.left < 12 && m_pControlScrollGroupsLeft->IsVisible())
	{
		nGroupsScrollPos = nGroupsScrollPos + rc.left - 12;
		if (nGroupsScrollPos < 0) nGroupsScrollPos = 0;
	}
	else if (rc.right > rcGroups.right && m_pControlScrollGroupsRight->IsVisible())
	{
		nGroupsScrollPos = nGroupsScrollPos + rc.right - rcGroups.Width() + 12;
		if (nGroupsScrollPos < 0) nGroupsScrollPos = 0;
	}

	if (nGroupsScrollPos != m_nGroupsScrollPos)
	{
		m_nGroupsScrollPos = nGroupsScrollPos;
		pGroups->GetParentTab()->GetParent()->OnRecalcLayout();
	}
}

void CXTPRibbonBar::EnsureVisible(CXTPControl* pControl)
{
	if (pControl->GetRibbonGroup())
	{
		if (pControl->GetRibbonGroup()->GetParentTab() != GetSelectedTab())
		{
			pControl->GetRibbonGroup()->GetParentTab()->Select();
		}
	}

	if (GetSelectedTab() && pControl->GetRibbonGroup() &&
		(m_pControlScrollGroupsLeft->IsVisible() || m_pControlScrollGroupsRight->IsVisible()))
	{
		ShowScrollableRect(GetSelectedTab()->GetGroups(), pControl->GetRect());
	}
}

void CXTPRibbonBar::OnGroupsScroll(BOOL bScrollLeft)
{
	if (!GetSelectedTab())
		return;

	GetCommandBars()->ClosePopups();

	int nGroupsScrollPos = GetSelectedTab()->GetGroups()->m_nGroupsScrollPos;

	if (bScrollLeft)
	{
		nGroupsScrollPos -= 40;
		if (nGroupsScrollPos < 0)
			nGroupsScrollPos = 0;
	}
	else
	{
		nGroupsScrollPos += 40;
	}

	if (nGroupsScrollPos != m_nGroupsScrollPos)
	{
		m_nGroupsScrollPos = nGroupsScrollPos;
		OnRecalcLayout();
	}
}

void CXTPRibbonScrollableBar::EnableGroupsScroll(BOOL bScrollLeft, BOOL bScrollRight)
{
	ASSERT(m_pControlScrollGroupsRight);
	ASSERT(m_pControlScrollGroupsLeft);

	CRect rcGroups(GetGroupsRect());

	if (bScrollLeft)
	{
		m_pControlScrollGroupsLeft->SetHideFlag(xtpHideScroll, FALSE);
		m_pControlScrollGroupsLeft->SetRect(CRect(rcGroups.left - 1, rcGroups.top, rcGroups.left + 12, rcGroups.bottom));
	}
	else
	{
		m_pControlScrollGroupsLeft->SetHideFlag(xtpHideScroll, TRUE);
	}

	if (bScrollRight)
	{
		m_pControlScrollGroupsRight->SetHideFlag(xtpHideScroll, FALSE);
		m_pControlScrollGroupsRight->SetRect(CRect(rcGroups.right - 12, rcGroups.top, rcGroups.right + 1, rcGroups.bottom));
	}
	else
	{
		m_pControlScrollGroupsRight->SetHideFlag(xtpHideScroll, TRUE);
	}
}



CXTPTabPaintManager* CXTPRibbonBar::GetTabPaintManager() const
{
	return GetRibbonPaintManager()->GetTabPaintManager();
}

void CXTPRibbonBar::DrawCommandBar(CDC* pDC, CRect rcClipBox)
{
#if 0
	_int64 nPerfomanceEnd;
	_int64 nPerfomanceStart;

	QueryPerformanceCounter((LARGE_INTEGER*)&nPerfomanceStart);
#endif

	pDC->SetBkMode(TRANSPARENT);

	CXTPPaintManager* pPaintManager = GetPaintManager();

	GetRibbonPaintManager()->FillRibbonBar(pDC, this);

	if (IsFrameThemeEnabled() && CRect().IntersectRect(m_rcCaption, rcClipBox))
	{
		GetRibbonPaintManager()->DrawRibbonFrameCaption(pDC, this, m_pFrameHook->m_bActive);
	}
	else if (m_bShowCaptionAlways && CRect().IntersectRect(m_rcCaption, rcClipBox))
	{
		GetRibbonPaintManager()->DrawRibbonFrameCaptionBar(pDC, this);
	}

	CXTPRibbonTab* pSlectedTab = GetSelectedTab();

	if (IsGroupsVisible())
	{
		GetRibbonPaintManager()->FillGroupRect(pDC, pSlectedTab, m_rcGroups);
	}

	GetTabPaintManager()->m_bDrawTextHidePrefix = !IsKeyboardCuesVisible();

	if (m_pControlTab && CRect().IntersectRect(rcClipBox, m_rcTabControl))
	{
		GetTabPaintManager()->DrawTabControl(m_pControlTab, pDC, m_rcTabControl);
	}

	int i;

	if (pSlectedTab && IsGroupsVisible())
	{
		CXTPRibbonGroups* pGroups = pSlectedTab->GetGroups();
		for (i = 0; i < pGroups->GetCount(); i++)
		{
			CXTPRibbonGroup* pGroup = pGroups->GetAt(i);

			if (pGroup->IsVisible() && CRect().IntersectRect(rcClipBox, pGroup->GetRect()))
			{
				pGroup->Draw(pDC, rcClipBox);
			}
		}
	}

	BOOL bFirst = TRUE;

	for (i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl->IsVisible())
			continue;

		if (pControl->GetRibbonGroup() != NULL)
			continue;

		if (pControl->GetBeginGroup() && !bFirst)
		{
			pPaintManager->DrawCommandBarSeparator(pDC, this, pControl);
		}
		bFirst = FALSE;
		CRect rc(pControl->GetRect());

		if (CRect().IntersectRect(rcClipBox, rc))
		{
			CXTPFontDC font(pDC, pPaintManager->GetCommandBarFont(this, pControl->IsItemDefault()));
			pControl->Draw(pDC);
		}
	}


	CXTPCommandBars* pCommandBars = GetCommandBars();

	if (pCommandBars && pCommandBars->IsCustomizeMode())
	{
		if (pCommandBars->GetDragControl() && pCommandBars->GetDragControl()->GetParent() == this &&
			pCommandBars->GetDragControl()->IsVisible())
		{
			CRect rc = pCommandBars->GetDragControl()->GetRect();
			pDC->Draw3dRect(rc, 0, 0);
			rc.DeflateRect(1, 1);
			pDC->Draw3dRect(rc, 0, 0);
		}

		if (!m_rcMarker.IsRectEmpty())
		{
			CustomizeDrawMarker(pDC);
		}
	}
#if 0
	QueryPerformanceCounter((LARGE_INTEGER*)&nPerfomanceEnd);
	TRACE(_T("TotalCounter = %i \n"), int(nPerfomanceEnd - nPerfomanceStart));
#endif
}

void CXTPRibbonBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nHit = HitTestCaption(point);

	if ((nHit == HTNOWHERE) && IsFrameThemeEnabled() && m_pContextHeaders->HitTest(point))
		nHit = HTCAPTION;

	if (nHit == HTCAPTION)
	{
		CWnd* pSite = GetSite();
		UINT syscmd = 0;

		if ((pSite->GetStyle() & WS_MINIMIZE) || (pSite->GetStyle() & WS_MAXIMIZE))
		{
			syscmd = SC_RESTORE;
		}
		else if ((pSite->GetStyle() & WS_MAXIMIZEBOX))
		{
			syscmd = SC_MAXIMIZE;
		}

		if (syscmd != 0)
		{
			if (syscmd == SC_MAXIMIZE && pSite->GetStyle() & WS_SYSMENU)
			{
				if (::GetMenuState(::GetSystemMenu(pSite->GetSafeHwnd(), FALSE), syscmd, MF_BYCOMMAND) & MFS_GRAYED)
					return;
			}

			ClientToScreen(&point);
			pSite->SendMessage(WM_SYSCOMMAND, syscmd | HTCAPTION, MAKELPARAM(point.x, point.y));
		}
		return;
	}

	CXTPTabManagerItem* pItem = m_pControlTab->HitTest(point);
	if (pItem && pItem == GetSelectedTab() && m_bAllowMinimize)
	{
		if (IsRibbonMinimized())
		{
			SetRibbonMinimized(FALSE);
		}
		else
		{
			m_pControlTab->SetSelectedItem(NULL);
			SetRibbonMinimized(TRUE);
		}
	}

	CXTPMenuBar::OnLButtonDblClk(nFlags, point);
}

int CXTPRibbonBar::HitTestCaption(CPoint point) const
{
	if (IsCustomizeMode())
		return HTNOWHERE;

	if (!IsFrameThemeEnabled())
		return HTNOWHERE;

	if (m_rcCaption.PtInRect(point) && m_pControls->HitTest(point) == 0)
	{
		if (m_pContextHeaders->HitTest(point))
			return HTNOWHERE;

		DWORD dwStyle = GetSite()->GetStyle();

		if ((point.y < m_rcCaption.top + 5) && (dwStyle & WS_SIZEBOX) && ((dwStyle & WS_MAXIMIZE) == 0))
			return HTTOP;

		return HTCAPTION;
	}
	return HTNOWHERE;
}

BOOL CXTPRibbonBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	int nHit = HitTestCaption(pt);

	if (nHit >= HTSIZEFIRST && nHit <= HTSIZELAST)
	{
		CWnd* pSite = GetSite();
		pSite->SendMessage(WM_SETCURSOR, (WPARAM)pSite->m_hWnd, MAKELPARAM(nHit, message));
		return TRUE;

	}

	return CXTPMenuBar::OnSetCursor(pWnd, nHitTest, message);
}

void CXTPRibbonBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPRibbonTabContextHeader* pContextHeader = m_pContextHeaders->HitTest(point);
	if (pContextHeader)
	{
		pContextHeader->m_pFirstTab->Select();
		return;
	}

	int nHit = HitTestCaption(point);

	if (nHit != HTNOWHERE)
	{
		CWnd* pSite = GetSite();
		ClientToScreen(&point);

		GetCommandBars()->ClosePopups();
		UpdateWindow();

		if (pSite->GetStyle() & WS_MAXIMIZE)
			return;

		if (nHit == HTCAPTION)
		{
			pSite->PostMessage(WM_SYSCOMMAND, SC_MOVE | HTCAPTION, MAKELPARAM(point.x, point.y));
		}
		else if (nHit >= HTSIZEFIRST && nHit <= HTSIZELAST)
		{
			if (pSite->GetStyle() & WS_SYSMENU)
			{
				if (::GetMenuState(::GetSystemMenu(pSite->GetSafeHwnd(), FALSE), SC_SIZE, MF_BYCOMMAND) & MFS_GRAYED)
					return;
			}

			pSite->PostMessage(WM_SYSCOMMAND,  SC_SIZE + (nHit - HTSIZEFIRST + 1), MAKELPARAM(point.x, point.y));
		}
		return;
	}

	if (m_rcTabControl.PtInRect(point))
	{
		GetCommandBars()->ClosePopups();
	}

	if (!m_pControlTab->PerformClick(m_hWnd, point))
	{
		CXTPMenuBar::OnLButtonDown(nFlags, point);
	}
}

BOOL CXTPRibbonBar::IsRibbonMinimized() const
{
	return m_bMinimized;
}

void CXTPRibbonBar::SetRibbonMinimized(BOOL bMinimized)
{
	if (m_bMinimized != bMinimized)
	{
		m_bMinimized = bMinimized;

		GetCommandBars()->ClosePopups();

		if (!m_bMinimized && GetSelectedTab() == NULL)
		{
			m_pControlTab->SetSelectedItem(m_pControlTab->FindNextFocusable(-1, +1));
		}
		else if (!m_bMinimized)
		{
			RebuildControls(GetSelectedTab());
		}
		else if (bMinimized)
		{
			m_pControlTab->SetSelectedItem(NULL);
		}

		OnRecalcLayout();
	}
}

void CXTPRibbonBar::OnRButtonUp(UINT nFlags, CPoint point)
{
	int nHit = HitTestCaption(point);

	if (nHit == HTCAPTION)
	{
		GetCommandBars()->ClosePopups();
		UpdateWindow();

		CWnd* pSite = GetSite();
		ClientToScreen(&point);
		pSite->SendMessage(0x0313, (WPARAM)pSite->GetSafeHwnd(), MAKELPARAM(point.x, point.y));
		return;
	}

	CXTPMenuBar::OnRButtonUp(nFlags, point);
}


void CXTPRibbonBar::OnSysColorChange()
{
	CXTPMenuBar::OnSysColorChange();

	GetRibbonPaintManager()->RefreshMetrics();
	m_pControlTab->Reposition();
}

void CXTPRibbonBar::RefreshSysButtons()
{
	CXTPMenuBar::RefreshSysButtons();

	if (IsFrameThemeEnabled())
	{
		CString strWindowText;
		GetSite()->GetWindowText(strWindowText);

		if (strWindowText != m_strCaptionText)
			DelayLayout();
	}
}

INT_PTR CXTPRibbonBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	INT_PTR nHit = m_pControlTab ? m_pControlTab->PerformToolHitTest(m_hWnd, point, pTI) : -1;
	if (nHit != -1)
		return nHit;

	nHit = CXTPMenuBar::OnToolHitTest(point, pTI);
	if (nHit == -1)
		return -1;

	if (pTI != NULL && pTI->cbSize == sizeof(XTP_TOOLTIP_TOOLINFO_EX) && m_pToolTipContext
		&& m_pToolTipContext->IsShowTitleAndDescription())
	{
		XTP_TOOLTIP_CONTEXT* tc = ((XTP_TOOLTIP_TOOLINFO_EX*)pTI)->pToolInfo;
		if (tc->pObject && ((CXTPControl*)tc->pObject)->GetRibbonGroup())
		{
			tc->rcExclude = m_rcGroups;
		}
	}

	return nHit;
}

CXTPRibbonGroup* CXTPRibbonBar::HitTestGroup(CPoint point) const
{
	CXTPRibbonTab* pTab = GetSelectedTab();
	if (!pTab)
		return NULL;

	if (!IsGroupsVisible())
		return NULL;

	return pTab->GetGroups()->HitTest(point);
}

BOOL CXTPRibbonBar::IsDwmEnabled() const
{
	return m_pFrameHook && m_pFrameHook->IsDwmEnabled();
}

LRESULT CXTPRibbonBar::OnNcHitTest(CPoint point)
{
	if (IsDwmEnabled())
	{
		LRESULT lResult = 0;
		CXTPWinDwmWrapper().DefWindowProc(GetSite()->GetSafeHwnd(), WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y), &lResult);

		if (lResult == HTMINBUTTON || lResult == HTMAXBUTTON || lResult == HTCLOSE || lResult == HTHELP)
		{
			return HTTRANSPARENT;
		}
	}

	return CXTPMenuBar::OnNcHitTest(point);
}

void CXTPRibbonBar::OnMouseMove(UINT nFlags, CPoint point)
{
	CXTPCommandBars* pCommandBars = GetCommandBars ();
	CXTPMouseManager* pMouseManager = pCommandBars->GetMouseManager();

	BOOL bMouseLocked = pMouseManager->IsMouseLocked() || !pMouseManager->IsTopParentActive(m_hWnd)
		|| pMouseManager->IsTrackedLock(this) || m_nPopuped != -1;

	CXTPRibbonGroup* pHighlightedGroup = bMouseLocked || !GetRibbonPaintManager()->m_bHotTrackingGroups ?
		NULL : HitTestGroup(point);

	if (pHighlightedGroup != m_pHighlightedGroup)
	{
		if (m_pHighlightedGroup)
		{
			CRect rcRedraw(m_pHighlightedGroup->GetRect());
			m_pHighlightedGroup = NULL;

			Redraw(rcRedraw);
		}

		m_pHighlightedGroup = pHighlightedGroup;

		if (m_pHighlightedGroup)
		{
			Redraw(m_pHighlightedGroup->GetRect());
		}

		if (m_pHighlightedGroup)
		{
			pMouseManager->TrackMouseLeave(*this);
		}
	}

	if (!bMouseLocked && m_pControlTab)
	{
		m_pControlTab->PerformMouseMove(m_hWnd, point);
	}

	CXTPMenuBar::OnMouseMove(nFlags, point);
}

void CXTPRibbonBar::OnMouseLeave()
{
	if (m_pControlTab) m_pControlTab->PerformMouseMove(m_hWnd, CPoint(-1, -1));

	if (m_pHighlightedGroup)
	{
		CRect rcRedraw(m_pHighlightedGroup->GetRect());
		m_pHighlightedGroup = NULL;

		Redraw(rcRedraw);
	}

	CXTPMenuBar::OnMouseLeave();
}

CXTPRibbonTab* CXTPRibbonBar::GetTab(int nIndex) const
{
	return (CXTPRibbonTab*)m_pControlTab->GetItem(nIndex);
}

int CXTPRibbonBar::GetTabCount() const
{
	return m_pControlTab->GetItemCount();
}



BOOL CXTPRibbonBar::PreviewAccel(UINT chAccel)
{
	if (GetKeyState(VK_CONTROL) < 0)
		return FALSE;

	int nIndex = m_pControlTab->GetIndex();

	CXTPCommandBars* pCommandBars = GetCommandBars();

	if (chAccel == (UINT)VK_MENU && GetTabCount() > 0)
	{
		pCommandBars->ClosePopups();
		pCommandBars->ShowKeyboardCues(TRUE);
		pCommandBars->m_keyboardTips.nLevel = 1;

		if (IsRibbonMinimized())
		{
			SetTrackingMode(TRUE, TRUE, TRUE);
		}
		else
		{
			SetTrackingMode(TRUE, FALSE, TRUE);

			SetSelected(nIndex, TRUE);
			m_pControlTab->SetFocused(TRUE);
		}

		return TRUE;
	}

	for (int i = 0; i < GetTabCount(); i++)
	{
		CXTPRibbonTab* pTab = GetTab(i);

		CString strCaption = pTab->GetCaption();
		if (!strCaption.IsEmpty() && pTab->IsEnabled() && pTab->IsVisible())
		{
			int nAmpIndex = strCaption.Find(_T('&'));
			BOOL bFound = nAmpIndex > -1 && nAmpIndex < strCaption.GetLength() - 1;

			if (bFound && CXTPShortcutManager::CompareAccelKey(strCaption[nAmpIndex + 1], chAccel))
			{

				if (!IsTrackingMode())
					pCommandBars->ClosePopups();

				pCommandBars->ShowKeyboardCues(TRUE);

				m_bKeyboardSelect = TRUE;
				pCommandBars->m_keyboardTips.nLevel = 1;
				SetCurSel(i);

				SetTrackingMode(TRUE, FALSE, TRUE);
				SetSelected(nIndex, TRUE);
				m_pControlTab->SetFocused(TRUE);

				if (!IsRibbonMinimized()) pCommandBars->ShowKeyboardTips(this, 2);

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CXTPRibbonBar::SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard)
{
	m_pControlTab->PerformMouseMove(m_hWnd, CPoint(-1, -1));

	if (m_pHighlightedGroup)
	{
		m_pHighlightedGroup = NULL;
		Redraw();
	}

	return CXTPMenuBar::SetTrackingMode(bMode, bSelectFirst, bKeyboard);
}

void CXTPRibbonBar::ShowContextMenu(CPoint point, CXTPControl* pSelectedControl)
{
	ClientToScreen(&point);

	CXTPPopupBar* pPopupBar = CreateContextMenu(pSelectedControl);
	if (!pPopupBar)
		return;

	CWnd* pSite = m_bShowQuickAccess ? this : GetOwnerSite();

	if (pPopupBar->GetControls()->GetVisibleCount() > 0)
	{
		CXTPCommandBars::TrackPopupMenu(pPopupBar, TPM_RIGHTBUTTON, point.x, point.y, pSite, NULL, pSite);
	}

	pPopupBar->InternalRelease();
}

CXTPPopupBar* CXTPRibbonBar::CreateMoreQuickAccessContextMenu()
{
	CXTPPopupBar* pPopupBar = new CMorePopupToolBar(this);

	for (int i = 0; i < m_pQuickAccessControls->GetCount(); i++)
	{
		CXTPControl* pControl = m_pQuickAccessControls->GetAt(i);
		if (pControl->GetHideFlags() & xtpHideWrap)
		{
			CXTPControl* pClone = pPopupBar->GetControls()->AddClone(pControl);
			pClone->SetHideFlag(xtpHideWrap, FALSE);
		}
	}

	CXTPControl* pClone = pPopupBar->GetControls()->AddClone(m_pControlQuickAccess);
	pClone->SetHideFlag(xtpHideWrap, FALSE);

	pPopupBar->SetWidth(300);

	return pPopupBar;
}


CXTPPopupBar* CXTPRibbonBar::CreateContextMenu(CXTPControl* pSelectedControl)
{
	if (!m_bShowQuickAccess)
	{
		CXTPPopupBar* pPopupBar = GetCommandBars()->GetToolbarsPopup();
		GetSite()->SendMessage(WM_XTP_TOOLBARCONTEXTMENU, (WPARAM)this, (LPARAM)pPopupBar);
		return pPopupBar;
	}

	if (!pSelectedControl)
		return NULL;

	BOOL bMoreMenu = pSelectedControl->GetID() == XTP_ID_RIBBONCONTROLQUICKACCESS;

	CMenu menu;
	if (!XTPResourceManager()->LoadMenu(&menu, bMoreMenu ? XTP_IDR_RIBBONCUSTOMIZEMENUMORE : XTP_IDR_RIBBONCUSTOMIZEMENU))
		return NULL;

	CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
	pPopupBar->SetOwner(this);
	pPopupBar->LoadMenu(menu.GetSubMenu(0));

	CXTPControl* pControlToolbars = pPopupBar->GetControls()->InsertAt(new CXTPControlToolbars, bMoreMenu ? 0 : 2);
	pControlToolbars->SetBeginGroup(TRUE);

	BOOL bQuickAccess = IsQuickAccessControl(pSelectedControl);

	BOOL bAllow = IsAllowQuickAccessControl(pSelectedControl);
	BOOL bSystem = pSelectedControl->GetFlags() & xtpFlagNoMovable;

	for (int i = 0; i < pPopupBar->GetControlCount(); i++)
	{
		CXTPControl* pControl = pPopupBar->GetControl(i);

		switch(pControl->GetID())
		{
			case XTP_ID_RIBBONCUSTOMIZE_REMOVE:
				pControl->SetVisible(bQuickAccess && !bSystem && m_bAllowQuickAccessCustomization);
				pControl->SetTag((DWORD_PTR)pSelectedControl);
				break;

			case XTP_ID_RIBBONCUSTOMIZE_ADD:
				pControl->SetVisible(!bQuickAccess && !bSystem && m_bAllowQuickAccessCustomization);
				pControl->SetEnabled(bAllow);
				pControl->SetTag((DWORD_PTR)pSelectedControl);
				break;

			case XTP_ID_RIBBONCUSTOMIZE:
				if (!m_bAllowQuickAccessCustomization)
					pControl->SetCaption(XTP_IDS_CUSTOMIZE);
				break;

			case XTP_ID_RIBBONCUSTOMIZE_QUICKACCESSBELOW:
				pControl->SetVisible((bQuickAccess || bSystem) && !m_bShowQuickAccessBelow);
				break;

			case XTP_ID_RIBBONCUSTOMIZE_QUICKACCESSABOVE:
				pControl->SetVisible((bQuickAccess || bSystem) && m_bShowQuickAccessBelow);
				break;

			case XTP_ID_RIBBONCUSTOMIZE_LABEL:
				pControl = pPopupBar->GetControls()->SetControlType(i, xtpControlLabel);
				pControl->SetItemDefault(TRUE);
				pControl->SetVisible(m_bAllowQuickAccessCustomization && GetQuickAccessControls()->GetOriginalControls());
				break;

			case XTP_ID_RIBBONCUSTOMIZE_MINIMIZE:
				pControl->SetVisible(m_bAllowMinimize);
				pControl->SetChecked(m_bMinimized);
				break;

			case XTP_ID_RIBBONCUSTOMIZE_COMMANDS:
				{
					pControl->SetHideFlags(xtpHideGeneric);
					CXTPControls* pCommands = GetQuickAccessControls()->GetOriginalControls();
					if (!pCommands)
						continue;

					if (!m_bAllowQuickAccessCustomization)
						continue;

					for (int j = 0; j < pCommands->GetCount(); j++)
					{
						CXTPControl* pControlOriginal = pCommands->GetAt(j);

						CControlQuickAccessCommand* pControlCommand = new CControlQuickAccessCommand(GetQuickAccessControls(), pControlOriginal);
						pPopupBar->GetControls()->Add(pControlCommand, 0, NULL, i, TRUE);
						pControlCommand->SetCaption(pControlOriginal->GetCaption());

						CXTPControl* pControlQuickAccess = GetQuickAccessControls()->FindDuplicate(pControlOriginal);
						if (pControlQuickAccess)
						{
							pControlCommand->SetQuickAccessControl(pControlQuickAccess);
						}

						i++;
					}

				}
				break;

			default:
				continue;
		}
		pControl->SetFlags(xtpFlagManualUpdate);
	}

	GetSite()->SendMessage(WM_XTP_TOOLBARCONTEXTMENU, (WPARAM)this, (LPARAM)pPopupBar);

	return pPopupBar;
}

void CXTPRibbonBar::SelectNextTab(BOOL bNext)
{
	m_pControlTab->PerformKeyDown(m_hWnd, !bNext ? VK_LEFT : VK_RIGHT);
}

int CXTPRibbonBar::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	if (IsTrackingMode() && nMessage == WM_CONTEXTMENU)
	{
		if (lParam != -1)
			return TRUE;

		CXTPControl* pControl = GetControl(m_nSelected);
		if (!pControl)
			return TRUE;

		ShowContextMenu(pControl->GetRect().TopLeft(), pControl);
		return TRUE;
	}

	if (IsTrackingMode() && nMessage == WM_MOUSEWHEEL && !GetCommandBars()->GetMouseManager()->IsMouseLocked())
	{
		SelectNextTab((short)HIWORD(wParam) <= 0);
		return TRUE;
	}

	return CXTPMenuBar::OnHookMessage(hWnd, nMessage, wParam, lParam, lResult);
}


void CXTPRibbonBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (!pCommandBars) return;

	if (IsCustomizeMode())
	{
		CXTPCommandBar::OnRButtonDown(nFlags, point);
		return;
	}

	SetTrackingMode(FALSE);
	SetSelected(-1);
	//Redraw();

	int nHit = HitTestCaption(point);
	if (nHit != HTNOWHERE)
		return;

	CXTPControl* pControl = m_pControls->HitTest(point);

	if (pControl && pControl->OnRButtonDown(point))
		return;

	if (pControl)
	{
		ShowContextMenu(point, pControl);
		return;
	}

	CXTPRibbonGroup* pGroup = HitTestGroup(point);
	if (pGroup && pGroup->GetRect().bottom - GetRibbonPaintManager()->m_nGroupCaptionHeight < point.y)
	{
		ShowContextMenu(point, pGroup->GetControlGroupPopup());
		return;
	}

}

void CXTPRibbonBar::OnCustomizePlaceQuickAccess(UINT nCommand)
{
	ShowQuickAccessBelowRibbon(nCommand == XTP_ID_RIBBONCUSTOMIZE_QUICKACCESSBELOW);
}

LRESULT CXTPRibbonBar::OnCustomizeCommand(WPARAM wParam, LPARAM lParam)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)lParam;
	CXTPControl* pControl = (CXTPControl*)tagNMCONTROL->pControl->GetTag();

	if (wParam == XTP_ID_RIBBONCUSTOMIZE_ADD)
	{
		m_bInRecalcLayout = TRUE;
		pControl = m_pQuickAccessControls->AddClone(pControl, -1, TRUE);

		pControl->SetBeginGroup(FALSE);
		pControl->SetVisible(TRUE);

		if (pControl->GetType() == xtpControlGallery)
			pControl->SetStyle(xtpButtonIcon);
		else
			pControl->SetStyle(xtpButtonAutomatic);

		m_bInRecalcLayout = FALSE;

		OnRecalcLayout();

		return TRUE;
	}

	if (wParam == XTP_ID_RIBBONCUSTOMIZE_REMOVE)
	{
		m_pQuickAccessControls->Remove(pControl);
		return TRUE;
	}

	if (wParam == XTP_ID_RIBBONCUSTOMIZE)
	{
		CWnd* pOwner = GetOwnerSite();

		if (pOwner->SendMessage(WM_XTP_COMMAND, wParam, lParam) == 0)
		{
			pOwner->SendMessage(WM_COMMAND, wParam);
		}

		return TRUE;
	}

	if (wParam == XTP_ID_RIBBONCUSTOMIZE_MINIMIZE)
	{
		SetRibbonMinimized(!IsRibbonMinimized());
		return TRUE;
	}


	return FALSE;
}

BOOL CXTPRibbonBar::ShouldSerializeBar()
{
	return TRUE;
}

void CXTPRibbonBar::GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	if (!pCommandBarList->Lookup(this))
	{
		pCommandBarList->Add(this);
		InternalAddRef();

		ASSERT(m_nBarID != 0);

		m_pQuickAccessControls->GenerateCommandBarList(nID, pCommandBarList, pParam);
	}
}

void CXTPRibbonBar::RestoreCommandBarList(CXTPCommandBarList* pCommandBarList)
{
	m_pQuickAccessControls->RestoreCommandBarList(pCommandBarList);
}

void CXTPRibbonBar::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_Bool(pPX, _T("ShowQuickAccessBelow"), m_bShowQuickAccessBelow, FALSE);

	if (pPX->GetSchema() > _XTP_SCHEMA_103)
		PX_Bool(pPX, _T("Minimized"), m_bMinimized, FALSE);

	CXTPPropExchangeSection secControls(pPX->GetSection(_T("QuickAccessControls")));
	m_pQuickAccessControls->DoPropExchange(&secControls);

}

void CXTPRibbonBar::Copy(CXTPCommandBar* pCommandBar, BOOL /*bRecursive = FALSE*/)
{
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBar;

	m_bShowQuickAccessBelow = pRibbonBar->m_bShowQuickAccessBelow;

	m_bMinimized = pRibbonBar->m_bMinimized;

	if (m_bMinimized)
	{
		m_pControlTab->SetSelectedItem(NULL);
	}

	m_pQuickAccessControls->RemoveAll();

	for (int i = 0; i < pRibbonBar->GetQuickAccessControls()->GetCount(); i++)
	{
		m_pQuickAccessControls->AddClone(pRibbonBar->GetQuickAccessControls()->GetAt(i), -1, FALSE);
	}
}

void CXTPRibbonBar::MergeToolBar(CXTPCommandBar* pCommandBar, BOOL /*bSilent*/)
{
	Copy(pCommandBar, FALSE);
}

CXTPRibbonTab* CXTPRibbonBar::FindTab(int nId) const
{
	return m_pControlTab->FindTab(nId);
}

CXTPRibbonGroup* CXTPRibbonBar::FindGroup(int nId) const
{
	for (int i = 0; i < GetTabCount(); i++)
	{
		CXTPRibbonTab* pTab = GetTab(i);
		CXTPRibbonGroup* pGroup = pTab->FindGroup(nId);

		if (pGroup)
			return pGroup;
	}
	return NULL;
}

void CXTPRibbonBar::ShowQuickAccessBelowRibbon(BOOL bBelow)
{
	m_bShowQuickAccessBelow = bBelow;

	OnRecalcLayout();

	if (IsDwmEnabled())
	{
		UpdateWindow();
		m_pFrameHook->UpdateFrameRegion();
	}
	else if (IsFrameThemeEnabled())
	{
		m_pFrameHook->GetSite()->SendMessage(WM_NCPAINT);
	}
}

BOOL CXTPRibbonBar::IsQuickAccessBelowRibbon() const
{
	return m_bShowQuickAccessBelow;
}

void CXTPRibbonBar::RemoveAllTabs()
{
	m_pControlTab->DeleteAllItems();
}

void CXTPRibbonBar::RemoveTab(int nIndex)
{
	CXTPRibbonTab* pTab = GetTab(nIndex);
	if (!pTab)
		return;

	pTab->Remove();
}

CSize CXTPRibbonBar::GetButtonSize() const
{
	if (m_szButtons != CSize(0)) return m_szButtons;

	int nHeight = GetPaintManager()->GetControlHeight();
	return CSize(nHeight, nHeight);
}

void CXTPRibbonBar::SetFontHeight(int nFontHeight)
{
	GetRibbonPaintManager()->SetFontHeight(nFontHeight);

	OnRecalcLayout();

	if (IsDwmEnabled())
	{
		UpdateWindow();
		m_pFrameHook->UpdateFrameRegion();
	}
	if (m_pFrameHook)
	{
		m_pFrameHook->RedrawFrame();
	}
}

int CXTPRibbonBar::GetFontHeight() const
{
	return GetRibbonPaintManager()->GetFontHeight();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CXTPRibbonBar::OnKeyboardTip(CXTPCommandBarKeyboardTip* pTip)
{
	CXTPControl* pControl = DYNAMIC_DOWNCAST(CXTPControl, pTip->m_pOwner);
	if (pControl)
	{
		pControl->OnUnderlineActivate();
		return;
	}

	CXTPRibbonTab* pTab = DYNAMIC_DOWNCAST(CXTPRibbonTab, pTip->m_pOwner);
	if (pTab)
	{
		SetCurSel(pTab->GetIndex());

		SetTrackingMode(TRUE, FALSE, TRUE);
		SetSelected(m_pControlTab->GetIndex(), TRUE);
		m_pControlTab->SetFocused(TRUE);

		if (!IsRibbonMinimized()) GetCommandBars()->ShowKeyboardTips(this, 2);

		return;
	}
}

void CXTPRibbonScrollableBar::CreateGroupKeyboardTips(CXTPRibbonTab* pSelectedTab)
{
	CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();
	CXTPRibbonTheme* pPaintManager = (CXTPRibbonTheme*)pCommandBars->GetPaintManager();
	CRect rcGroups = GetGroupsRect();

	for (int i = 0; i < m_pParent->GetControlCount(); i++)
	{
		CXTPControl* pControl = m_pParent->GetControl(i);
		if (!pControl->IsVisible() || pControl->GetCaption().IsEmpty())
			continue;

		if (pControl->GetRibbonGroup() == 0 || pControl->GetRibbonGroup()->GetParentTab() !=
			pSelectedTab || (pControl->GetFlags() & xtpFlagSkipFocus))
			continue;
		CRect rc(pControl->GetRect());

		CRect rcIntersect; rcIntersect.IntersectRect(rcGroups, rc);
		if (rcIntersect != rc)
			continue;

		CPoint pt(rc.left + 21, pControl->GetRect().bottom - 4);
		DWORD dwAlign = DT_TOP;

		CRect rcGroup = pControl->GetRibbonGroup()->GetRect();
		CRect rcCaption(rcGroup.left, rcGroup.bottom - pPaintManager->GetGroupCaptionHeight() + 2, rcGroup.right, rcGroup.bottom);
		rcGroup.bottom = rcCaption.top;

		if (CRect().IntersectRect(rc, rcCaption))
		{
			pt = CPoint(rc.CenterPoint().x, rcGroups.bottom - 6);
			dwAlign = DT_TOP | DT_CENTER;
		}
		else if (pControl->GetStyle() == xtpButtonIconAndCaptionBelow && rc.Height() > rcGroup.Height() / 2)
		{
			pt = CPoint(rc.CenterPoint().x, rcGroup.bottom - 2);
			dwAlign = DT_CENTER | DT_VCENTER;
		}
		else if (pControl->GetType() == xtpControlGallery)
		{
			pt = CPoint(rc.right, rcGroup.bottom - 2);
			dwAlign = DT_CENTER | DT_VCENTER;
		}
		else if (rc.CenterPoint().y < rcGroup.top + rcGroup.Height() * 1 / 3)
		{
			pt = CPoint(rc.left + 11, rcGroup.top + 2);
			dwAlign = DT_LEFT | DT_VCENTER;
		}
		else if (rc.CenterPoint().y > rcGroup.top + rcGroup.Height() * 2 / 3)
		{
			pt = CPoint(rc.left + 11, rcGroup.bottom - 2);
			dwAlign = DT_LEFT | DT_VCENTER;
		}
		else
		{
			pt = CPoint(rc.left + 11, rcGroup.CenterPoint().y);
			dwAlign = DT_LEFT | DT_VCENTER;
		}

		CXTPCommandBarKeyboardTip* pWnd = new CXTPCommandBarKeyboardTip(pCommandBars, pControl, pControl->GetCaption(), pt, dwAlign, pControl->GetEnabled());

		pCommandBars->m_keyboardTips.arr.Add(pWnd);
	}
}

void CXTPRibbonBar::CreateKeyboardTips()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (!pCommandBars)
		return;

	if (pCommandBars->m_keyboardTips.nLevel == 1)
	{
		int i;


		int k = 1;
		for (i = 0; i < m_pQuickAccessControls->GetCount(); i++)
		{
			CXTPControl* pControl = m_pQuickAccessControls->GetAt(i);
			if (!pControl->IsVisible())
				continue;

			CPoint pt(pControl->GetRect().CenterPoint().x, pControl->GetRect().bottom - 11);
			CString strCaption;
			if (k < 10)
			{
				strCaption.Format(_T("%i"), k);
			}
			else if (k < 19)
			{
				strCaption.Format(_T("0%i"), k - 9);
			}
			else break;
			k++;

			CXTPCommandBarKeyboardTip* pWnd = new CXTPCommandBarKeyboardTip(pCommandBars, pControl, strCaption, pt, DT_CENTER | DT_TOP, pControl->GetEnabled());

			pCommandBars->m_keyboardTips.arr.Add(pWnd);
		}

		for (i = 0; i < GetTabCount(); i++)
		{
			CXTPRibbonTab* pTab = GetTab(i);

			CString strCaption = pTab->GetCaption();
			if (!strCaption.IsEmpty() && pTab->IsEnabled() && pTab->IsVisible())
			{
				CPoint pt(pTab->GetRect().CenterPoint().x, pTab->GetRect().bottom - 9);

				CXTPCommandBarKeyboardTip* pWnd = new CXTPCommandBarKeyboardTip(pCommandBars, pTab, strCaption, pt, DT_CENTER | DT_TOP, TRUE);

				pCommandBars->m_keyboardTips.arr.Add(pWnd);
			}
		}

		if (m_pControlSystemButton)
		{
			CPoint pt(m_pControlSystemButton->GetRect().CenterPoint().x, m_pControlSystemButton->GetRect().CenterPoint().y);

			CXTPCommandBarKeyboardTip* pWnd = new CXTPCommandBarKeyboardTip(pCommandBars,
				m_pControlSystemButton, m_pControlSystemButton->GetCaption(), pt, DT_VCENTER | DT_CENTER, m_pControlSystemButton->GetEnabled());

			pCommandBars->m_keyboardTips.arr.Add(pWnd);

		}
	}

	if (pCommandBars->m_keyboardTips.nLevel == 2)
	{
		CreateGroupKeyboardTips(GetSelectedTab());
	}
}

CSize CXTPRibbonBar::GetIconSize() const
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	return m_szIcons != CSize(0) ? m_szIcons:
		(pCommandBars->GetCommandBarsOptions()->szIcons != CSize(0) ?
		pCommandBars->GetCommandBarsOptions()->szIcons : GetAutoIconSize(FALSE));
}



