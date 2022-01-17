// XTPOffice2007FrameHook.cpp : implementation file
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

#include "XTPCommandBars.h"
#include "XTPControlButton.h"

#include "XTPOffice2007Theme.h"
#include "XTPOffice2007FrameHook.h"

#include "Ribbon/XTPRibbonBar.h"

#define XTP_TID_MOUSELEAVE  0xACB1

#ifndef WM_NCMOUSELEAVE
#define WM_NCMOUSELEAVE     0x02A2
#endif



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CXTPOffice2007FrameHook::CControlCaptionButton : public CXTPControl
{
public:
	CControlCaptionButton(CXTPOffice2007FrameHook* pFrame)
	{
		m_pFrame = pFrame;
	}

public:
	virtual int GetSelected() const
	{
		return m_pFrame->m_pSelected == (CXTPControl*)this;
	}
	virtual BOOL GetPressed() const
	{
		return m_pFrame->m_pSelected == (CXTPControl*)this && m_pFrame->m_bButtonPressed;
	}

protected:
	CXTPOffice2007FrameHook* m_pFrame;
};

void CXTPOffice2007FrameHook::AddCaptionButton(int nId, int nHit)
{
	CXTPControl* pButton = new CControlCaptionButton(this);
	pButton->SetID(nId);
	pButton->SetHelpId(nHit);
	m_pCaptionButtons->Add(pButton);
}



CXTPOffice2007FrameHook::CXTPOffice2007FrameHook()
{
	m_pCommandBars = NULL;
	m_pRibbonBar = NULL;
	m_pPaintManager = NULL;

	m_bActive = TRUE;
	m_szFrameRegion = CSize(0, 0);
	m_hwndSite = NULL;
	m_pSelected = NULL;
	m_bDelayReclalcLayout = FALSE;
	m_bButtonPressed = FALSE;
	m_bOffice2007FrameEnabled = FALSE;
	m_nSkipNCPaint = 0;

	m_nFrameBorder = 0;
	m_bInUpdateFrame = FALSE;
	m_bLockNCPaint = FALSE;

	m_nMsgUpdateSkinState = RegisterWindowMessage(_T("WM_SKINFRAMEWORK_UPDATESTATE"));
	m_nMsgQuerySkinState = RegisterWindowMessage(_T("WM_SKINFRAMEWORK_QUERYSTATE"));


	m_bDwmEnabled = FALSE;

	m_pCaptionButtons = new CXTPControls();
	AddCaptionButton(SC_CLOSE, HTCLOSE);
	AddCaptionButton(SC_MAXIMIZE, HTMAXBUTTON);
	AddCaptionButton(SC_RESTORE, HTMAXBUTTON);
	AddCaptionButton(SC_MINIMIZE, HTMINBUTTON);
}

void CXTPOffice2007FrameHook::EnableOffice2007Frame(CWnd* pSite, CXTPPaintManager* pPaintManager)
{
	ASSERT(pSite);
	ASSERT(pPaintManager);

	m_pCommandBars = NULL;
	m_pRibbonBar = NULL;
	m_pPaintManager = pPaintManager;
	m_hwndSite = pSite->GetSafeHwnd();

	m_bActive = TRUE;
	m_szFrameRegion = CSize(0, 0);

	m_pSelected = NULL;

	m_bDelayReclalcLayout = FALSE;
	m_bButtonPressed = FALSE;

	m_bOffice2007FrameEnabled = TRUE;

	m_bDwmEnabled = pSite->GetStyle() & WS_CHILD ? FALSE : CXTPWinDwmWrapper().IsCompositionEnabled();

	XTPHookManager()->SetHook(m_hwndSite, this);
	SendMessage(m_hwndSite, m_nMsgUpdateSkinState, 0, 0);

	if (!m_bDwmEnabled)
	{
		CWnd::ModifyStyle(m_hwndSite, WS_CAPTION, WS_BORDER, SWP_FRAMECHANGED);
	}

	::SetWindowPos(m_hwndSite, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_DRAWFRAME);
}

CXTPOffice2007FrameHook::~CXTPOffice2007FrameHook()
{
	CMDTARGET_RELEASE(m_pCaptionButtons);

	if (m_bOffice2007FrameEnabled)
	{
		EnableOffice2007Frame(NULL);
	}
}

DWORD CXTPOffice2007FrameHook::GetSiteStyle(BOOL bExStyle) const
{
	return m_hwndSite ? (DWORD)GetWindowLong(m_hwndSite, bExStyle ? GWL_EXSTYLE : GWL_STYLE) : 0;
}

void CXTPOffice2007FrameHook::EnableOffice2007Frame(CXTPCommandBars* pCommandBars)
{
	m_pCommandBars = pCommandBars;
	m_pRibbonBar = NULL;
	m_pPaintManager = NULL;

	if (pCommandBars)
	{
		m_pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, pCommandBars->GetMenuBar());
		m_hwndSite = pCommandBars->GetSite()->GetSafeHwnd();
	}

	m_bActive = TRUE;
	m_szFrameRegion = CSize(0, 0);

	m_pSelected = NULL;

	m_bDelayReclalcLayout = FALSE;
	m_bButtonPressed = FALSE;

	m_bOffice2007FrameEnabled = pCommandBars != NULL;

	if (pCommandBars)
	{
		XTPHookManager()->SetHook(m_hwndSite, this);
		SendMessage(m_hwndSite, m_nMsgUpdateSkinState, 0, 0);

		m_bDwmEnabled = GetSiteStyle() & WS_CHILD ? FALSE : CXTPWinDwmWrapper().IsCompositionEnabled();

		if (!m_bDwmEnabled)
		{
			CWnd::ModifyStyle(m_hwndSite, WS_CAPTION, WS_BORDER, SWP_FRAMECHANGED);
		}

		UpdateFrameRegion();

		::SetWindowPos(m_hwndSite, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_DRAWFRAME);
	}
	else
	{
		XTPHookManager()->RemoveAll(this);

		if (m_hwndSite)
		{
			if (m_bDwmEnabled)
			{
				CXTPWinDwmWrapper().ExtendFrameIntoClientArea(m_hwndSite, 0, 0, 0, 0);
			}

			SendMessage(m_hwndSite, m_nMsgUpdateSkinState, 0, 0);

			CWnd::ModifyStyle(m_hwndSite, WS_BORDER, WS_CAPTION, SWP_FRAMECHANGED);
			SetWindowRgn(m_hwndSite, (HRGN)NULL, TRUE);
		}
		m_hwndSite = NULL;
		m_bDwmEnabled = FALSE;
	}
}


CXTPOffice2007Theme* CXTPOffice2007FrameHook::GetPaintManager() const
{
	return m_pCommandBars ? (CXTPOffice2007Theme*)m_pCommandBars->GetPaintManager() : (CXTPOffice2007Theme*)m_pPaintManager;
}

void CXTPOffice2007FrameHook::UpdateFrameRegion()
{
	if (!IsWindow(m_hwndSite))
		return;

	CXTPWindowRect rc(m_hwndSite);
	UpdateFrameRegion(rc.Size(), TRUE);
}

void CXTPOffice2007FrameHook::UpdateFrameRegion(CSize szFrameRegion, BOOL bUpdate)
{
	if (m_bInUpdateFrame)
		return;

	m_bInUpdateFrame = TRUE;

	if (m_szFrameRegion != szFrameRegion || bUpdate)
	{
		CRect rc(0, 0, szFrameRegion.cx, szFrameRegion.cy);
		AdjustWindowRectEx(rc, GetSiteStyle(FALSE), FALSE, GetSiteStyle(TRUE));

		m_nFrameBorder = -rc.left;

		if (!m_bDwmEnabled)
		{
			HRGN hRgn = 0;

			if (!IsMDIMaximized())
			{
				if (GetSiteStyle() & WS_MAXIMIZE)
				{
					hRgn = CreateRectRgn(m_nFrameBorder, m_nFrameBorder, szFrameRegion.cx - m_nFrameBorder, szFrameRegion.cy - m_nFrameBorder);
				}
				else
					hRgn = GetPaintManager()->CalcRibbonFrameRegion(this, szFrameRegion);
			}

			::SetWindowRgn(m_hwndSite, hRgn, TRUE);
		}
		else
		{
			if (!(GetSiteStyle() & WS_MAXIMIZE))
			{
				m_nFrameBorder = 4;
			}
			int cyTopHeight = IsCaptionVisible() ? 0 : GetCaptionHeight();
			CXTPWinDwmWrapper().ExtendFrameIntoClientArea(m_hwndSite, 0, cyTopHeight, 0, 0);
		}

		m_szFrameRegion = szFrameRegion;
	}

	m_bInUpdateFrame = FALSE;
}

BOOL CXTPOffice2007FrameHook::IsMDIMaximized() const
{
	if ((GetSiteStyle() & WS_MAXIMIZE) && (GetSiteStyle(TRUE) & WS_EX_MDICHILD))
		return TRUE;

	return FALSE;
}

BOOL CXTPOffice2007FrameHook::IsCaptionVisible() const
{
	if (IsMDIMaximized())
		return FALSE;

	if (!m_pRibbonBar)
		return TRUE;

	if (!(m_pRibbonBar->IsRibbonBarVisible() && m_pRibbonBar->GetPosition() == xtpBarTop))
		return TRUE;

	if ((GetSiteStyle() & (WS_CHILD | WS_MINIMIZE)) == (WS_CHILD | WS_MINIMIZE))
		return TRUE;

	return FALSE;
}

BOOL CXTPOffice2007FrameHook::IsFrameHasStatusBar(int* pnStatusHeight /*= NULL*/) const
{
	if (!GetPaintManager()->m_bFrameStatusBar)
		return FALSE;

	CWnd* pStatusBar = CWnd::FromHandle(::GetDlgItem(m_hwndSite, AFX_IDW_STATUS_BAR));
	if (!pStatusBar)
		return FALSE;

	if ((GetWindowLong(pStatusBar->m_hWnd, GWL_STYLE) & WS_VISIBLE) == 0)
		return FALSE;

	if (!pStatusBar->SendMessage(WM_USER + 9300))
		return FALSE;

	if (pnStatusHeight)
	{
		*pnStatusHeight = CXTPWindowRect(pStatusBar).Height();
	}

	return TRUE;
}


void CXTPOffice2007FrameHook::RedrawFrame()
{
	if (!m_bDwmEnabled)
	{
		CWindowDC dc(GetSite());
		GetPaintManager()->DrawRibbonFrame(&dc, this);
	}
}

void CXTPOffice2007FrameHook::RedrawRibbonBar()
{
	if (m_pRibbonBar)
	{
		m_pRibbonBar->Redraw();
		m_pRibbonBar->UpdateWindow();
	}
}

void CXTPOffice2007FrameHook::DelayRecalcFrameLayout()
{
	m_bDelayReclalcLayout = TRUE;
	::PostMessage(m_hwndSite, WM_IDLEUPDATECMDUI, 0, 0);
}

void CXTPOffice2007FrameHook::RecalcFrameLayout()
{
	m_bDelayReclalcLayout = FALSE;
	m_szFrameRegion = CSize(0);
	::SetWindowPos(m_hwndSite, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	CWnd* pSite = GetSite();

	CFrameWnd* pFrame = pSite->IsFrameWnd() ? (CFrameWnd*)pSite : NULL;
	if (pFrame)
	{
		pFrame->RecalcLayout(FALSE);
	}
	else
	{
		CXTPClientRect rc(pSite);
		pSite->SendMessage(WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()));
	}

	if (m_pRibbonBar)
	{
		m_pRibbonBar->Redraw();
	}
}

AFX_INLINE void SetVisibleAndEnabled(CXTPControl* pControl, BOOL bVisible, BOOL bEnabled)
{
	pControl->SetHideFlag(xtpHideGeneric, !bVisible);
	pControl->SetEnabled(bEnabled);
}

void CXTPOffice2007FrameHook::RepositionCaptionButtons()
{
	CXTPWindowRect rc(m_hwndSite);
	rc.OffsetRect(-rc.TopLeft());

	DWORD dwStyle = GetSiteStyle(FALSE);
	DWORD dwExStyle = GetSiteStyle(TRUE);

	BOOL bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW;
	BOOL bMinimized = dwStyle & WS_MINIMIZE;
	BOOL bEnableClose = TRUE;
	BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;
	BOOL bSysMenu = (dwStyle & WS_SYSMENU);
	BOOL bEnabledMaximize = ((dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX);
	BOOL bEnabledMinimize = ((dwStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX);
	BOOL bShowMinMaxButtons = !bToolWindow && bSysMenu && (bEnabledMaximize || bEnabledMinimize);
	if (bSysMenu && !bToolWindow)
	{
		CMenu* pMenu = CMenu::FromHandle(::GetSystemMenu(m_hwndSite, FALSE));
		if (pMenu && pMenu->GetMenuState(SC_CLOSE, MF_BYCOMMAND) & MF_DISABLED) bEnableClose = FALSE;
	}


	CRect rcCaption(rc.left, rc.top + m_nFrameBorder, rc.right - 4, rc.top + GetCaptionHeight() - 3);

	for (int i = 0; i < m_pCaptionButtons->GetCount(); i++)
	{
		CXTPControl* pControl = m_pCaptionButtons->GetAt(i);

		if (pControl->GetID() == SC_CLOSE)
		{
			SetVisibleAndEnabled(pControl, bSysMenu, bEnableClose);
		}
		if (pControl->GetID() == SC_MAXIMIZE)
		{
			SetVisibleAndEnabled(pControl, !bMaximized && bShowMinMaxButtons, bEnabledMaximize);
		}
		if (pControl->GetID() == SC_RESTORE)
		{
			pControl->SetHelpId(bMinimized? HTMINBUTTON : HTMAXBUTTON);

			if (bMinimized)
				SetVisibleAndEnabled(pControl, bShowMinMaxButtons, bEnabledMinimize);
			else
				SetVisibleAndEnabled(pControl, bMaximized && bShowMinMaxButtons, bEnabledMaximize);
		}
		if (pControl->GetID() == SC_MINIMIZE)
		{
			SetVisibleAndEnabled(pControl, !bMinimized && bShowMinMaxButtons, bEnabledMinimize);
		}
		if (!pControl->IsVisible())
			continue;

		CRect rcButton(rcCaption.right - rcCaption.Height(), rcCaption.top, rcCaption.right, rcCaption.bottom);
		pControl->SetRect(rcButton);

		rcCaption.right -= rcButton.Width();
	}
}

void CXTPOffice2007FrameHook::ScreenToFrame(LPPOINT lpPoint)
{
	CXTPDrawHelpers::ScreenToWindow(GetSite(), lpPoint);
}


void CXTPOffice2007FrameHook::TrackCaptionButton(CXTPControl* pSelected)
{
	CWnd* pSite = GetSite();
	pSite->SetCapture();
	BOOL bAccept = FALSE;
	m_bButtonPressed = TRUE;

	ASSERT(pSelected);

	RedrawFrame();

	while (::GetCapture() == m_hwndSite)
	{
		MSG msg;

		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		if (msg.message == WM_LBUTTONUP)
		{
			bAccept = m_bButtonPressed;
			break;
		}
		else if (msg.message == WM_MOUSEMOVE)
		{
			POINT point = msg.pt;
			ScreenToFrame(&point);

			BOOL bButtonPressed = pSelected->GetRect().PtInRect(point);

			if (bButtonPressed != m_bButtonPressed)
			{
				m_bButtonPressed = bButtonPressed;
				RedrawFrame();
			}
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	m_bButtonPressed = NULL;
	m_pSelected = NULL;

	ReleaseCapture();
	RedrawFrame();

	if (bAccept)
	{
		pSite->SendMessage(WM_SYSCOMMAND, pSelected->GetID());
	}
}

int CXTPOffice2007FrameHook::GetCaptionHeight() const
{
	if (IsCaptionVisible() || !m_pRibbonBar)
		return GetPaintManager()->GetFrameCaptionHeight() + m_nFrameBorder;

	return m_pRibbonBar->GetCaptionHeight();
}

#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED  0x031E
#endif

CXTPControl* CXTPOffice2007FrameHook::HitTestCaptionButton(CPoint point)
{
	CXTPControl* pCaptionButton = m_pCaptionButtons->HitTest(point);
	if (pCaptionButton && !pCaptionButton->GetEnabled()) pCaptionButton = NULL;

	return pCaptionButton;
}

int CXTPOffice2007FrameHook::OnHookMessage(HWND /*hWnd*/, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	if (!m_bOffice2007FrameEnabled)
	{
		return FALSE;
	}

	if (!m_bDwmEnabled && m_bLockNCPaint && (nMessage == WM_STYLECHANGING || nMessage == WM_STYLECHANGED
		|| nMessage == WM_WINDOWPOSCHANGED || nMessage == WM_WINDOWPOSCHANGING || nMessage == WM_NCPAINT))
	{
		if (nMessage == WM_WINDOWPOSCHANGING)
			((WINDOWPOS*)lParam)->flags &= ~SWP_FRAMECHANGED;

		return TRUE;
	}

	if (nMessage == WM_WINDOWPOSCHANGING)
	{
		WINDOWPOS* lpwndpos = (WINDOWPOS*)lParam;

		CSize szFrameRegion(lpwndpos->cx, lpwndpos->cy);

		if (((lpwndpos->flags & SWP_NOSIZE) ==  0) && (m_szFrameRegion != szFrameRegion) &&
			((GetSiteStyle() & WS_CHILD) == 0))
		{
			m_nSkipNCPaint = 1;
		}

		return FALSE;
	}

	if (nMessage == WM_WINDOWPOSCHANGED)
	{
		WINDOWPOS* lpwndpos = (WINDOWPOS*)lParam;
		if (lpwndpos->flags & SWP_FRAMECHANGED && !m_bInUpdateFrame)
		{
			UpdateFrameRegion();
		}
		return FALSE;
	}

	if (nMessage == WM_SIZE || nMessage == WM_STYLECHANGED)
	{
		CXTPWindowRect rc(m_hwndSite);

		CSize szFrameRegion(rc.Width(), rc.Height());

		if ((m_szFrameRegion != szFrameRegion) || (nMessage == WM_STYLECHANGED))
		{
			UpdateFrameRegion(szFrameRegion, (nMessage == WM_STYLECHANGED));
			RedrawFrame();
		}

		return FALSE;
	}

	if (nMessage == WM_NCRBUTTONUP && IsCaptionVisible()&& !m_bDwmEnabled)
	{
		if (m_pSelected)
		{
			m_pSelected = NULL;
			RedrawFrame();
		}

		CPoint point((DWORD)lParam);
		::SendMessage(m_hwndSite, 0x0313, (WPARAM)m_hwndSite, MAKELPARAM(point.x, point.y));
		return TRUE;
	}

	if (nMessage == WM_NCLBUTTONDOWN && IsCaptionVisible() && !m_bDwmEnabled)
	{
		CPoint point((DWORD)lParam);
		ScreenToFrame(&point);

		CXTPControl* pCaptionButton = HitTestCaptionButton(point);
		if (pCaptionButton)
		{
			TrackCaptionButton(pCaptionButton);
			return TRUE;
		}
	}

	if (nMessage == WM_NCMOUSEMOVE && IsCaptionVisible() && !m_bDwmEnabled)
	{
		CPoint point((DWORD)lParam);
		ScreenToFrame(&point);

		CXTPControl* pCaptionButton = HitTestCaptionButton(point);
		if (pCaptionButton != m_pSelected)
		{
			m_pSelected = pCaptionButton;
			RedrawFrame();

			if (m_pSelected)
			{
				::SetTimer (m_hwndSite, XTP_TID_MOUSELEAVE, 50, NULL);
			}
			else
			{
				KillTimer(m_hwndSite, XTP_TID_MOUSELEAVE);
			}
		}
	}

	if (nMessage == WM_TIMER && wParam == XTP_TID_MOUSELEAVE && !m_bDwmEnabled)
	{
		if (::GetCapture() != NULL)
			return TRUE;

		CPoint pt;
		::GetCursorPos (&pt);
		ScreenToFrame(&pt);

		if (!HitTestCaptionButton(pt))
		{
			if (m_pSelected)
			{
				m_pSelected = NULL;
				RedrawFrame();
			}

			KillTimer(m_hwndSite, XTP_TID_MOUSELEAVE);
		}

		return TRUE;
	}

	if (nMessage == WM_NCHITTEST  && !IsCaptionVisible() && !m_bDwmEnabled)
	{
		CPoint point((DWORD)lParam);
		ScreenToFrame(&point);

		lResult = XTPHookManager()->Default();

		if ((GetSiteStyle(TRUE) & WS_EX_CLIENTEDGE) && (GetSiteStyle() & WS_SIZEBOX)
			&& !(GetSiteStyle() & WS_MINIMIZE))
		{
			if (point.y == m_szFrameRegion.cy - 1 && lResult == HTBORDER)
			{
				lResult = HTBOTTOM;
			}

		}

		return TRUE;
	}

	if (nMessage == WM_NCHITTEST  && !IsCaptionVisible() && m_bDwmEnabled)
	{
		CPoint point((DWORD)lParam);
		ScreenToFrame(&point);

		lResult = XTPHookManager()->Default();
		if (point.y < GetCaptionHeight())
		{
			CXTPWinDwmWrapper().DefWindowProc(m_hwndSite, nMessage, wParam, lParam, &lResult);
		}
		return TRUE;

	}
	if (nMessage == WM_NCHITTEST && IsCaptionVisible() && !m_bDwmEnabled)
	{
		CPoint point((DWORD)lParam);
		ScreenToFrame(&point);

		lResult = XTPHookManager()->Default();

		CXTPControl* pCaptionButton = m_pCaptionButtons->HitTest(point);
		if (pCaptionButton)
		{
			lResult = pCaptionButton->GetHelpId();
			return TRUE;
		}

		if (lResult == HTNOWHERE &&  point.y < GetCaptionHeight())
		{
			if ((point.x < 7 + GetSystemMetrics(SM_CXSMICON)) && (GetSiteStyle() & WS_SYSMENU))
				lResult = HTSYSMENU;
			else
				lResult = HTCAPTION;
		}

		return TRUE;
	}

	if (nMessage == WM_SETTEXT)
	{
		lResult = XTPHookManager()->Default();

		if (IsCaptionVisible())
		{
			RedrawFrame();
		}
		else
		{
			if (m_pRibbonBar)
				RecalcFrameLayout();
		}
		return TRUE;
	}


	if (nMessage == WM_NCACTIVATE && !m_bDwmEnabled)
	{
		BOOL bActive = (BOOL)wParam;
		CWnd* pSite = GetSite();

		// stay active if WF_STAYACTIVE bit is on
		if (pSite->m_nFlags & WF_STAYACTIVE)
			bActive = TRUE;

		// but do not stay active if the window is disabled
		if (!pSite->IsWindowEnabled())
			bActive = FALSE;

		m_bLockNCPaint = TRUE;
		DWORD dwStyle = GetSiteStyle();
		if (dwStyle & WS_SIZEBOX)
		{
			CWnd::ModifyStyle(m_hwndSite, WS_SIZEBOX, 0, 0);
		}

		lResult = XTPHookManager()->Default(bActive, 0);

		if (dwStyle & WS_SIZEBOX)
		{
			CWnd::ModifyStyle(m_hwndSite, 0, WS_SIZEBOX, 0);
		}
		m_bLockNCPaint = FALSE;

		if (m_bActive != bActive)
		{
			m_bActive = bActive;

			RedrawFrame();
			RedrawRibbonBar();
		}


		return TRUE;
	}

	if (nMessage == WM_DWMCOMPOSITIONCHANGED)
	{
		BOOL bDwmEnabled = GetSiteStyle() & WS_CHILD ? FALSE : CXTPWinDwmWrapper().IsCompositionEnabled();
		if (bDwmEnabled != m_bDwmEnabled)
		{
			m_bDwmEnabled = bDwmEnabled;

			if (!m_bDwmEnabled)
			{
				CWnd::ModifyStyle(m_hwndSite, WS_CAPTION, WS_BORDER, SWP_FRAMECHANGED);
			}
			else
			{
				CWnd::ModifyStyle(m_hwndSite, WS_BORDER, WS_CAPTION, SWP_FRAMECHANGED);
				::SetWindowRgn(m_hwndSite, 0, TRUE);
			}
		}
		return FALSE;
	}

	if (nMessage == WM_NCCALCSIZE)
	{
		NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS FAR*)lParam;

		CRect rc(lpncsp->rgrc[0]);

		lResult = XTPHookManager()->Default();

		if (!IsMDIMaximized())
		{
			if (m_bDwmEnabled)
			{
				if (!IsCaptionVisible())
					lpncsp->rgrc[0].top = rc.top + 0;
			}
			else
			{
				lpncsp->rgrc[0].top = rc.top + (IsCaptionVisible() ? GetCaptionHeight() :  0);
				if (IsFrameHasStatusBar() && !(GetSiteStyle() & WS_MAXIMIZE))
				{
					int nBorderSize = max(rc.bottom - lpncsp->rgrc[0].bottom - 3, 1);
					lpncsp->rgrc[0].bottom = rc.bottom - nBorderSize;
				}
			}
		}

		return TRUE;
	}

	if (nMessage == m_nMsgQuerySkinState)
	{
		lResult = TRUE;
		return TRUE;
	}

	if (nMessage == WM_NCPAINT && !m_bDwmEnabled)
	{
		if (m_nSkipNCPaint)
		{
			m_nSkipNCPaint--;
			return TRUE;
		}
		if (!IsMDIMaximized())
		{
			RepositionCaptionButtons();

			CWindowDC dc(GetSite());
			GetPaintManager()->DrawRibbonFrame(&dc, this);
		}
		return TRUE;
	}

	if (nMessage == WM_IDLEUPDATECMDUI)
	{
		if (m_bDelayReclalcLayout)
		{
			RecalcFrameLayout();
		}
	}

	if (nMessage == WM_GETMINMAXINFO && !m_bDwmEnabled)
	{
		lResult = XTPHookManager()->Default();

		MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

		int yMin = GetCaptionHeight() + 5;
		int xMin = (int)3 * yMin;

		xMin += GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CXEDGE);

		lpMMI->ptMinTrackSize.x = max(lpMMI->ptMinTrackSize.x, xMin);
		lpMMI->ptMinTrackSize.y = max(lpMMI->ptMinTrackSize.y, yMin);

		if ((GetSiteStyle() & WS_CHILD) == 0)
		{
			CRect rcArea = XTPMultiMonitor()->GetWorkArea(GetSite());
			CRect rcScreen = XTPMultiMonitor()->GetScreenArea(GetSite());

			int nBorder = m_nFrameBorder;
			lpMMI->ptMaxSize.x = rcArea.Width() + nBorder * 2;
			lpMMI->ptMaxSize.y = rcArea.Height() + nBorder * 2;
			lpMMI->ptMaxPosition.y = rcArea.top - rcScreen.top - nBorder;
			lpMMI->ptMaxPosition.x = rcArea.left - rcScreen.left - nBorder;
		}

		return TRUE;
	}

	return FALSE;
}
