// XTPControlCustom.cpp : implementation of the CXTPControlCustom class.
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
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPPropExchange.h"

#include "XTPCommandBar.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPHookManager.h"
#include "XTPControlCustom.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




////////////////////////////


IMPLEMENT_XTP_CONTROL(CXTPControlCustom, CXTPControlButton)


CXTPControlCustom::CXTPControlCustom()
{
	m_hwndControl = NULL;
	m_rcBorders.SetRectEmpty();
	m_verticalOptions = xtpVerticalHide;
	m_bControlVisible = TRUE;
	m_dwFlags = xtpFlagNoMovable;
	m_nControlID = 0;

	EnableAutomation();
}

CXTPControlCustom::~CXTPControlCustom()
{
	XTPHookManager()->RemoveAll(this);
}

void CXTPControlCustom::SetControl(CWnd* pWnd)
{
	XTPHookManager()->RemoveAll(this);

	m_hwndControl = pWnd->GetSafeHwnd();

	if (m_hwndControl)
	{
		XTPHookManager()->SetHook(m_hwndControl, this);

		CXTPWindowRect rc(m_hwndControl);
		m_szControl = CSize(rc.Width(), rc.Height());
	}
}

int CXTPControlCustom::OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& /*lResult*/)
{
	if (nMessage == WM_MOUSEMOVE)
	{
		CPoint point(lParam);
		MapWindowPoints(hWnd, m_pParent->GetSafeHwnd(), &point, 1);
		GetParent()->OnMouseMove((UINT)wParam, point);
	}

	return FALSE;
}

void CXTPControlCustom::SetSize(CSize szControl)
{
	m_szControl = szControl;
}

CWnd* CXTPControlCustom::GetControl()
{
	return CWnd::FromHandle(m_hwndControl);
}


CSize CXTPControlCustom::GetSize(CDC* pDC)
{
	if (m_hwndControl && m_bControlVisible)
	{
		return CSize(m_szControl.cx + m_rcBorders.left + m_rcBorders.right, m_szControl.cy + m_rcBorders.top + m_rcBorders.bottom);

	}
	return CXTPControlButton::GetSize(pDC);
}


void CXTPControlCustom::SetRect(CRect rcControl)
{
	if (m_rcControl == rcControl && !(m_hwndControl && ::GetParent(m_hwndControl) != m_pParent->GetSafeHwnd()))
		return;

	m_rcControl = rcControl;

	CWnd* pControlWnd = CWnd::FromHandle(m_hwndControl);
	if (pControlWnd && pControlWnd->GetSafeHwnd() && ::IsWindow(pControlWnd->GetSafeHwnd()))
	{
		pControlWnd->SetParent(m_pParent);
		pControlWnd->ModifyStyle(WS_POPUP, WS_CHILD);
		rcControl.DeflateRect(m_rcBorders);
		pControlWnd->MoveWindow(rcControl);

		ShowHideChildControl();
	}
}

void CXTPControlCustom::SetEnabled(BOOL bEnabled)
{
	if (bEnabled != m_bEnabled)
	{
		m_bEnabled = bEnabled;
		if (m_hwndControl)
		{
			::EnableWindow(m_hwndControl, bEnabled);
		}

		RedrawParent();
	}
}

void CXTPControlCustom::ShowHideChildControl()
{
	if (m_pControls && m_pControls->IsOriginalControls())
		return;

	if (!m_pParent || (::GetParent(m_hwndControl) != m_pParent->GetSafeHwnd()))
		return;

	if (m_hwndControl)
	{
		::SetWindowPos(m_hwndControl, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE 
			| (m_pParent && m_bControlVisible && IsVisible() && m_pParent->GetSafeHwnd() ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

void CXTPControlCustom::OnRemoved()
{
	m_bControlVisible = FALSE;
	ShowHideChildControl();

	CXTPControlButton::OnRemoved();
}

void CXTPControlCustom::SetHideFlags(DWORD dwFlags)
{
	if (m_dwHideFlags != dwFlags)
	{
		m_dwHideFlags = dwFlags;

		ShowHideChildControl();
		DelayLayoutParent();
	}
}

void CXTPControlCustom::OnActionChanged(int nProperty)
{
	if (nProperty == 2 && m_hwndControl)
	{
		ShowHideChildControl();
	}
}

void CXTPControlCustom::OnCalcDynamicSize(DWORD dwMode)
{
	if (m_verticalOptions == xtpVerticalHide)
	{
		if (dwMode & LM_VERTDOCK) SetHideFlags(m_dwHideFlags | xtpHideDockingPosition);
		else SetHideFlags(m_dwHideFlags & ~xtpHideDockingPosition);
	}
	if (m_verticalOptions == xtpVerticalButton)
	{
		m_bControlVisible = (dwMode & LM_VERTDOCK) == 0;
		ShowHideChildControl();
	}
}

void CXTPControlCustom::Copy(CXTPControl* pControl, BOOL bRecursive)
{
	ASSERT(DYNAMIC_DOWNCAST(CXTPControlCustom, pControl));

	m_rcBorders = ((CXTPControlCustom*)pControl)->m_rcBorders;
	m_verticalOptions = ((CXTPControlCustom*)pControl)->m_verticalOptions;
	m_szControl = ((CXTPControlCustom*)pControl)->m_szControl;
	m_hwndControl = ((CXTPControlCustom*)pControl)->m_hwndControl;
	m_nControlID = ((CXTPControlCustom*)pControl)->m_nControlID;

	CXTPControl::Copy(pControl, bRecursive);
}

BOOL CXTPControlCustom::IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint /*point*/, DROPEFFECT& dropEffect)
{
	if (pCommandBar->GetType() != xtpBarTypePopup)
	{
		dropEffect = DROPEFFECT_NONE;
		return FALSE;
	}
	return TRUE;
}

void CXTPControlCustom::Draw(CDC* pDC)
{
	if (m_hwndControl && ::IsWindowVisible(m_hwndControl))
		return;

	CXTPControlButton::Draw(pDC);
}

void CXTPControlCustom::OnClick(BOOL bKeyboard, CPoint pt)
{
	if (m_hwndControl && ::IsWindowVisible(m_hwndControl) && !IsCustomizeMode())
		return;

	CXTPControlButton::OnClick(bKeyboard, pt);
}

void CXTPControlCustom::SetBorders(int cxRight, int cxTop, int cxLeft, int cxBottom)
{
	m_rcBorders.SetRect(cxRight, cxTop, cxLeft, cxBottom);
}

void CXTPControlCustom::SetVerticalPositionOption(XTPControlCustomVerticalOptions options)
{
	m_verticalOptions = options;
}

CWnd* CXTPControlCustom::FindChildWindow(CXTPCommandBars* pCommandBars, UINT nID)
{
	CWnd* pWndSite = pCommandBars->GetSite();

	if (pWndSite->GetDlgItem(nID) != NULL)
		return pWndSite->GetDlgItem(nID);

	for (int i = 0; i < pCommandBars->GetCount(); i++)
	{
		CXTPToolBar* pToolBar = pCommandBars->GetAt(i);

		if (pToolBar->GetDlgItem(nID) != NULL)
			return pToolBar->GetDlgItem(nID);
	}


	return NULL;
}

void CXTPControlCustom::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControlButton::DoPropExchange(pPX);

	PX_Rect(pPX, _T("Borders"), m_rcBorders, CRect(0, 0, 0, 0));
	PX_Size(pPX, _T("ControlSize"), m_szControl, CSize(0, 0));
	PX_Enum(pPX, _T("VerticalOptions"), m_verticalOptions, xtpVerticalHide);

	if (pPX->GetSchema() > _XTP_SCHEMA_861)
	{
		PX_Int(pPX, _T("ControlId"), m_nControlID, 0);
	}

	if (pPX->IsLoading() && m_nControlID != 0)
	{
		CXTPCommandBars* pCommandBars = ((XTP_COMMANDBARS_PROPEXCHANGE_PARAM*)pPX->m_dwData)->pCommandBars;
		ASSERT(pCommandBars->IsKindOf(RUNTIME_CLASS(CXTPCommandBars)));
		ASSERT(pCommandBars && pCommandBars->GetSite());

		CWnd* pWnd = FindChildWindow(pCommandBars, m_nControlID);

		if (pWnd)
		{
			m_hwndControl = pWnd->GetSafeHwnd();
		}

	}
}



CXTPControlCustom* CXTPControlCustom::CreateControlCustom(CWnd* pWndChild)
{
	CXTPControlCustom* pControlCustom = (CXTPControlCustom*)CreateObject();
	pControlCustom->SetControl(pWndChild);
	pControlCustom->m_nControlID = pWndChild->GetDlgCtrlID();

	return pControlCustom;
}




