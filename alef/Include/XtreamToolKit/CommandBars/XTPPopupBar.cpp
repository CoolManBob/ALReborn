// XTPPopupBar.cpp : implementation of the CXTPPopupBar class.
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

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPToolTipContext.h"

#include "XTPPopupBar.h"
#include "XTPPaintManager.h"
#include "XTPControls.h"
#include "XTPControl.h"
#include "XTPControlButton.h"
#include "XTPShortcutManager.h"
#include "XTPControlPopup.h"
#include "XTPShadowsManager.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPMouseManager.h"
#include "XTPSoundManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double CXTPPopupBar::m_dMaxWidthDivisor = 1.0/3.0;

#ifndef EVENT_SYSTEM_MENUPOPUPSTART
#define EVENT_SYSTEM_MENUPOPUPSTART     0x0006
#define EVENT_SYSTEM_MENUPOPUPEND       0x0007
#define OBJID_MENU 0xFFFFFFFD
#endif

#define TID_HOVERUP 5001
#define TID_HOVERDOWN 5002
#define TID_TEAROFFSELECTED 5003
#define TID_EXPANDHOVER 5004


class CXTPPopupBar::CControlExpandButton: public CXTPControlButton
{
public:
	CControlExpandButton()
	{
		m_controlType = xtpControlButton;
		m_dwFlags = xtpFlagManualUpdate | xtpFlagNoMovable | xtpFlagSkipFocus | xtpFlagShowPopupBarTip;
	}
	void Draw(CDC* pDC);
	virtual void OnExecute();
};

void CXTPPopupBar::CControlExpandButton::Draw(CDC* pDC)
{
	GetPaintManager()->DrawSpecialControl(pDC, xtpButtonExpandMenu, this, m_pParent, TRUE, NULL);
}

void CXTPPopupBar::CControlExpandButton::OnExecute()
{
	((CXTPPopupBar*)m_pParent)->ExpandBar();
}


IMPLEMENT_XTP_COMMANDBAR(CXTPPopupBar, CXTPCommandBar)

CXTPPopupBar::CXTPPopupBar()
{
	m_scrollInfo.Init(this, TID_HOVERUP, TID_HOVERDOWN);

	m_barType = xtpBarTypePopup;
	m_barPosition = xtpBarPopup;

	m_pControlPopup = 0;
	m_ptPopup = 0;
	m_rcExclude.SetRectEmpty();
	m_rcTearOffGripper.SetRectEmpty();
	m_popupFlags = xtpPopupRight;
	m_bDynamicLayout = FALSE;

	m_bTearOffPopup = FALSE;
	m_bTearOffTracking = FALSE;
	m_nTearOffID = 0;
	m_nTearOffWidth = 0;

	m_bShowShadow = TRUE;
	m_rcBorders = CRect(2, 4, 2, 4);
	m_bExpanding = FALSE;

	m_nMRUWidth = 0;
	m_bDoubleGripper = FALSE;

	m_pTearOffBar = NULL;
	m_szTearOffBar = CSize(0, 0);
	m_nTearOffTimer = 0;

	m_bContextMenu = FALSE;

}

CXTPPopupBar* CXTPPopupBar::CreatePopupBar(CXTPCommandBars* pCommandBars)
{
	CXTPPopupBar* pPopupBar = (CXTPPopupBar*)CXTPCommandBars::m_pPopupBarClass->CreateObject();
	pPopupBar->SetCommandBars(pCommandBars);
	return pPopupBar;
}

CXTPPopupBar::~CXTPPopupBar()
{
}

void CXTPPopupBar::SetTearOffPopup(LPCTSTR strCaption, UINT nID, int nWidth)
{
	if (nID == 0)
	{
		m_bTearOffPopup = FALSE;
	}
	else
	{
		m_bTearOffPopup = TRUE;
		m_strTearOffCaption = strCaption;
		m_nTearOffID = nID;
		m_nTearOffWidth = nWidth;
	}
}
BOOL CXTPPopupBar::IsTearOffPopup(CString& strCaption, UINT& nID, int& nWidth)
{
	strCaption = m_strTearOffCaption;
	nID = m_nTearOffID;
	nWidth = m_nTearOffWidth;

	return m_bTearOffPopup;
}

BOOL CXTPPopupBar::IsVisible() const
{
	return (m_hWnd != 0) && IsWindowVisible();
}

void CXTPPopupBar::ExpandBar()
{
	if (m_bExpanded == FALSE && m_bCollapsed == TRUE)
	{
		SAFE_CALLPTR(m_pToolTipContext, CancelToolTips());

		m_bExpanding = TRUE;
		m_bExpanded = TRUE;
		SetPopuped(-1);
		SetSelected(-1);
		OnRecalcLayout();
		XTPMouseManager()->SetForceExpanded(TRUE);
		XTPMouseManager()->IgnoreLButtonUp();
	}
}

BOOL CXTPPopupBar::Create()
{
	if (m_hWnd)
		return TRUE;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	UINT nClassStyle = !pCommandBars || pCommandBars->GetCommandBarsOptions()->bUseSystemSaveBitsStyle ? CS_SAVEBITS | CS_DBLCLKS : CS_DBLCLKS;

	CXTPDrawHelpers::RegisterWndClass(0, _T("XTPPopupBar"), nClassStyle);

	int nLayoutRTL = pCommandBars && pCommandBars->IsLayoutRTL() ? WS_EX_LAYOUTRTL : 0;
	if (!pCommandBars && GetImageManager()->IsDrawReverted() == 1) nLayoutRTL = WS_EX_LAYOUTRTL;

	if (!CreateEx(WS_EX_TOOLWINDOW | nLayoutRTL, _T("XTPPopupBar"), 0, MFS_SYNCACTIVE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), GetSite(), 0))
		return FALSE;

	SetWindowText(m_strTitle);

	return TRUE;
}

#ifndef SPI_GETMENUANIMATION
#define SPI_GETMENUANIMATION                0x1002
#endif

#ifndef SPI_GETMENUFADE
#define SPI_GETMENUFADE                     0x1012
#endif


XTPAnimationType CXTPPopupBar::GetAnimationType() const
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars)
	{
		if (GetWindow(GW_CHILD))
			return xtpAnimateNone;

		XTPAnimationType animationType = pCommandBars->GetCommandBarsOptions()->animationType;

		switch (animationType)
		{
			case xtpAnimateWindowsDefault:
			{
				BOOL bEnabled = FALSE;
				if (!SystemParametersInfo(SPI_GETMENUANIMATION, 0, &bEnabled, 0))
					return xtpAnimateNone;

				if (!bEnabled)
					return xtpAnimateNone;

				BOOL bFadeAnimation = FALSE;

				if (!SystemParametersInfo(SPI_GETMENUFADE, 0, &bFadeAnimation, 0))
					return xtpAnimateSlide;

				animationType = bFadeAnimation ? xtpAnimateFade : xtpAnimateSlide;
			}
			break;

			// choose any animation based on a random number
			case xtpAnimateRandom:
			{
				switch (RAND_S() % 3)
				{
					case 0:
						animationType = xtpAnimateFade;
						break;

					case 1:
						animationType = xtpAnimateSlide;
						break;

					default:
						animationType = xtpAnimateUnfold;
						break;
				}
			}
			break;
		}

		return animationType;
	}
	return xtpAnimateNone;

}

void CXTPPopupBar::OnControlsChanged()
{
	m_nSelected = -1;
	m_nPopuped = -1;

	OnRecalcLayout();
}

void CXTPPopupBar::OnRecalcLayout()
{
	if (!m_bDynamicLayout && GetSafeHwnd() && IsTrackingMode())
	{

		UpdateShortcuts();
		UpdateFlags();

		CRect rc = CalculatePopupRect(CalcDynamicLayout(0, 0));
		CXTPWindowRect rcWindow(this);

		if (!m_bExpanding && rc == rcWindow)
		{
			Redraw();
			return;
		}

		CXTPCommandBars* pCommandBars = GetCommandBars();
		BOOL bShowKeyboardTips = IsKeyboardTipsVisible();
		if (bShowKeyboardTips && pCommandBars) pCommandBars->HideKeyboardTips();

		XTPShadowsManager()->RemoveShadow(this);

		BOOL bChanged = FALSE;

		if (m_bExpanding)
		{
			BOOL bExpandDown = rcWindow.bottom < rc.bottom;

			XTPAnimationType animationType = GetAnimationType();
			if (IsAnimateType(animationType))
			{
				CDC* pDC = GetDesktopWindow()->GetDC();

				CBitmap bmp;
				bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
				CXTPCompatibleDC dcWindow(pDC, &bmp);

				dcWindow.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);
				GetDesktopWindow()->ReleaseDC(pDC);

				ShowWindow(SW_HIDE);
				GetSite()->UpdateWindow();

				SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOREDRAW);
				GetPaintManager()->SetCommandBarRegion(this);
				bChanged = TRUE;

				CWindowDC paintDC(this);
				CXTPClientRect rcClientPost(this);

				BOOL dwLayout = CXTPDrawHelpers::IsContextRTL(&paintDC);
				CXTPDrawHelpers::SetContextRTL(&paintDC, FALSE);
				paintDC.BitBlt(0, 0, rc.Width(), rc.Height(), &dcWindow, 0, 0, SRCCOPY);
				CXTPDrawHelpers::SetContextRTL(&paintDC, dwLayout);


				CXTPBufferDC animationDC(paintDC, rcClientPost);
				DrawCommandBar(&animationDC, rcClientPost);

				// Animation
				GetPaintManager()->AnimateExpanding(this, &paintDC, &animationDC, bExpandDown);
			}
			XTPSoundManager()->PlaySystemSound(xtpSoundMenuPopup);
		}

		if (!bChanged)
		{
			SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			GetPaintManager()->SetCommandBarRegion(this);
		}

		if (m_barPosition == xtpBarPopup && m_bShowShadow)
			XTPShadowsManager()->SetShadow(this, m_rcExclude);

		if (m_pControlPopup)
			m_pControlPopup->UpdateShadow();

		Redraw();

		if (bShowKeyboardTips)
		{
			PostMessage(WM_TIMER, XTP_TID_SHOWKEYBOARDTIPS);
		}

		m_bExpanding = FALSE;
	}
}

void CXTPPopupBar::_MakeSameWidth(int nStart, int nLast, int nWidth)
{
	for (int i = nStart; i < nLast; i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl || pControl->GetParent() != this || !pControl->IsVisible())
			continue;

		CRect rc = pControl->GetRect();
		pControl->SetRect(CRect(rc.left, rc.top, rc.left + nWidth, rc.bottom));
	}
}

CRect CXTPPopupBar::GetBorders()
{
	if (m_barType == xtpBarTypeNormal)
		return m_rcBorders;

	return GetPaintManager()->GetCommandBarBorders(this);
}

CSize CXTPPopupBar::CalcDynamicLayout(int nLength, DWORD /*nMode*/)
{
	m_bDynamicLayout = TRUE;

	CClientDC dc(this);
	CXTPFontDC font(&dc, GetPaintManager()->GetCommandBarFont(this));

	RECT rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWork, 0);

	CRect rcBorders = GetBorders();
	CSize szTearOffGripper(0);
	if (m_bTearOffPopup)
	{
		szTearOffGripper = GetPaintManager()->DrawTearOffGripper(&dc, 0, FALSE, FALSE);
		rcBorders.top += szTearOffGripper.cy;
	}

	if (m_barType == xtpBarTypeNormal)
	{
		UpdateShortcuts();

		CSize sz = m_pControls->CalcDynamicSize(&dc, GetWidth(), LM_COMMIT | LM_HORZ | LM_POPUP, rcBorders);

		m_rcTearOffGripper.SetRect(rcBorders.left, rcBorders.top - szTearOffGripper.cy, sz.cx - rcBorders.right, rcBorders.top);

		m_bDynamicLayout = FALSE;

		return sz;
	}

	if (nLength == 0)
	{
		for (int i = 0; i < GetControlCount(); i++)
		{
			CXTPControl* pControl = GetControl(i);
			if (!pControl || pControl->GetParent() != this)
				continue;

			pControl->OnCalcDynamicSize(0);
		}

		UpdateExpandingState();
		UpdateShortcuts();
	}


	CXTPControl* pButtonExpand = m_pControls->FindControl(XTP_ID_POPUPBAR_EXPAND);
	CSize szButtonExpand(0);
	if (m_bCollapsed)
	{
		szButtonExpand = GetPaintManager()->DrawSpecialControl(&dc, xtpButtonExpandMenu, NULL, this, FALSE, NULL);
		rcBorders.bottom += szButtonExpand.cy;

		if (!pButtonExpand)
			pButtonExpand = m_pControls->Add(new CControlExpandButton(), XTP_ID_POPUPBAR_EXPAND, _T(""), -1, TRUE);
		pButtonExpand->SetHideFlags(xtpHideGeneric);
	}
	else if (pButtonExpand) pButtonExpand->SetHideFlags(xtpHideGeneric);

	CSize sz = m_pControls->CalcPopupSize(&dc, nLength, rcBorders);

	if (m_bCollapsed)
	{
		pButtonExpand->SetHideFlags(xtpNoHide);
		pButtonExpand->SetRect(CRect(rcBorders.left, sz.cy - rcBorders.bottom, sz.cx - rcBorders.right, sz.cy - rcBorders.bottom + szButtonExpand.cy));
	}

	m_rcTearOffGripper.SetRect(rcBorders.left, rcBorders.top - szTearOffGripper.cy, sz.cx - rcBorders.right, rcBorders.top);

	m_bDynamicLayout = FALSE;

	return sz;
}

CRect CXTPPopupBar::CalculatePopupRect(CSize sz)
{
	RECT rcWork = m_rcExclude.IsRectEmpty() ? XTPMultiMonitor()->GetWorkArea(m_ptPopup) : XTPMultiMonitor()->GetWorkArea(m_rcExclude);
	RECT rcScreen = m_rcExclude.IsRectEmpty() ? XTPMultiMonitor()->GetScreenArea(m_ptPopup) : XTPMultiMonitor()->GetScreenArea(m_rcExclude);

	// If point is on the screen, but not in the work area, then the point
	// must be inside an AppBar.
	if (m_rcExclude.IsRectEmpty() && PtInRect(&rcScreen, m_ptPopup) && !PtInRect(&rcWork, m_ptPopup))
	{
		rcWork = rcScreen;
	}

	rcWork.bottom -= 5;  // for shadow

	CPoint ptCenterRect = m_rcExclude.CenterPoint();
	BOOL bScroll = FALSE, bWrap = FALSE;
	CXTPCommandBars* pCommandars = GetCommandBars();
	BOOL bNoScroll = (pCommandars ? pCommandars->GetCommandBarsOptions()->bWrapLargePopups : FALSE) || m_bMultiLine;


	if (m_rcExclude.Height() == 0 && m_ptPopup.y > rcWork.bottom)
	{
		m_ptPopup.y = m_ptPopup.y - sz.cy;
	}
	else if (m_ptPopup.y + sz.cy > rcWork.bottom)
	{
		if (m_rcExclude.Height() == 0)
		{
			m_ptPopup.y = (m_ptPopup.y < (rcWork.bottom - rcWork.top)/2 ? rcWork.bottom : m_ptPopup.y)- sz.cy;
		}
		else
		{
			if (m_popupFlags & xtpPopupDown)
			{
				m_ptPopup.y = m_rcExclude.top - sz.cy;

				if (m_ptPopup.y < rcWork.top)
				{
					if (bNoScroll)
					{
						m_popupFlags &= ~xtpPopupDown;
						if (m_pControlPopup && (m_pControlPopup->GetType() != xtpControlComboBox || GetParentCommandBar()->GetType() == xtpBarTypePopup))
						{
							m_rcExclude = m_pControlPopup->GetRect();

							AdjustExcludeRect(m_rcExclude, TRUE);
							GetParentCommandBar()->ClientToScreen(m_rcExclude);
						}

						if (m_popupFlags & xtpPopupLeft)
						{
							m_ptPopup.y = rcWork.bottom - sz.cy;
						}
						else
						{
							m_ptPopup.x = m_rcExclude.right;
							m_ptPopup.y = rcWork.bottom - sz.cy;
						}
					}
					else
					{
						bScroll = TRUE;
						if (ptCenterRect.y > (rcWork.bottom - rcWork.top)/2)
						{
							m_ptPopup.y = rcWork.top;
							sz.cy = m_rcExclude.top - rcWork.top;
						}
						else
						{
							m_ptPopup.y = m_rcExclude.bottom;
							sz.cy = rcWork.bottom - m_ptPopup.y;
						}
					}
				}
			}
			else
			{
				m_ptPopup.y = rcWork.bottom - sz.cy;
			}
		}

		if (m_ptPopup.y < rcWork.top) m_ptPopup.y = rcWork.top;

		if (m_ptPopup.y + sz.cy > rcWork.bottom)
		{
			ASSERT(m_ptPopup.y == rcWork.top);

			if (bNoScroll)
			{
				bWrap = TRUE;
			}
			else
			{
				sz.cy = rcWork.bottom - m_ptPopup.y;
				bScroll = TRUE;
			}
		}
	}

	if (bWrap && !m_bMultiLine && (GetType() == xtpBarTypePopup))
	{
		sz = CalcDynamicLayout(rcWork.bottom - rcWork.top, 0);
	}

	if (m_popupFlags == (xtpPopupLeft | xtpPopupDown) && m_rcExclude.Width() != 0)
		m_ptPopup.x = m_rcExclude.right - sz.cx;
	else if (m_popupFlags & xtpPopupLeft && m_rcExclude.Width() != 0)
		m_ptPopup.x = m_rcExclude.left - sz.cx;
	else if ((m_popupFlags & xtpPopupLeft) && m_rcExclude.Width() == 0)
		m_ptPopup.x -= sz.cx;

	if (m_ptPopup.x + sz.cx > rcWork.right)
	{
		if (m_rcExclude.Width() == 0 && m_ptPopup.x > rcWork.right)
			m_ptPopup.x = m_ptPopup.x - sz.cx;
		else if (m_rcExclude.Width() == 0)
			m_ptPopup.x = rcWork.right - sz.cx;
		else
		{
			m_ptPopup.x = (m_popupFlags & xtpPopupDown ? rcWork.right : m_rcExclude.left) - sz.cx ;
			m_popupFlags |= xtpPopupLeft;
		}
		if (m_ptPopup.x < rcWork.left)
			m_ptPopup.x = rcWork.left;
	}
	else if (m_ptPopup.x < rcWork.left)
	{
		if (m_rcExclude.Width() != 0)
		{
			m_ptPopup.x = m_popupFlags & xtpPopupDown ? rcWork.left : m_rcExclude.right;
			m_popupFlags &= ~xtpPopupLeft;
		}
		else if (m_popupFlags & xtpPopupLeft)
		{
			m_ptPopup.x = rcWork.left;
			m_popupFlags &= ~xtpPopupLeft;
		}
	}

	m_scrollInfo.bScroll = bScroll && (GetType() == xtpBarTypePopup);
	if (m_scrollInfo.bScroll)
	{
		m_scrollInfo.nScrollFirst = 0;
		AdjustScrolling(&sz);
	}

	return CRect(m_ptPopup, sz);
}

void CXTPPopupBar::UpdateLocation(CSize sz)
{
	CRect rc = CalculatePopupRect(sz);
	SetWindowPos(&CWnd::wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW | (IsWindowVisible() ? SWP_NOZORDER : 0));
}

void CXTPPopupBar::AdjustScrolling(LPSIZE lpSize, BOOL bDown, BOOL bInvalidate)
{
	CSize sz = lpSize ? *lpSize : CXTPWindowRect(this).Size();

	m_scrollInfo.btnDown.rc.SetRectEmpty();
	m_scrollInfo.btnUp.rc.SetRectEmpty();
	int nCount = GetControlCount();
	CRect rcBorders = GetBorders();

	int nSeparator = GetPaintManager()->DrawCommandBarSeparator(NULL, this, NULL, FALSE).cy;

	CSize szTearOffGripper(0);
	if (m_bTearOffPopup)
	{
		szTearOffGripper = GetPaintManager()->DrawTearOffGripper(NULL, 0, FALSE, FALSE);
		rcBorders.top += szTearOffGripper.cy;
	}

	CXTPControl* pButtonExpand = m_pControls->FindControl(XTP_ID_POPUPBAR_EXPAND);
	CSize szButtonExpand(0);

	if (m_bCollapsed)
	{
		szButtonExpand = GetPaintManager()->DrawSpecialControl(NULL, xtpButtonExpandMenu, NULL, this, FALSE, NULL);
		rcBorders.bottom += szButtonExpand.cy;

		if (!pButtonExpand)
			pButtonExpand = m_pControls->Add(new CControlExpandButton(), XTP_ID_POPUPBAR_EXPAND, _T(""), -1, TRUE);
		pButtonExpand->SetHideFlags(xtpHideGeneric);
	}
	else if (pButtonExpand) pButtonExpand->SetHideFlags(xtpHideGeneric);


	int nScrollHeight = 16;
	int i;

	if (!bDown)
	{
		if (m_scrollInfo.nScrollFirst == 1)
			m_scrollInfo.nScrollFirst = 0;

		int nPos = rcBorders.top;
		if (m_scrollInfo.nScrollFirst != 0)
		{
			nPos += nScrollHeight;
			m_scrollInfo.btnDown.rc.SetRect(rcBorders.left, rcBorders.top, sz.cx - rcBorders.right, nPos);
		}

		ASSERT(m_scrollInfo.nScrollFirst < nCount && m_scrollInfo.nScrollFirst >= 0);

		BOOL bFirst = TRUE;
		for (i = m_scrollInfo.nScrollFirst; i < nCount ; i++)
		{
			CXTPControl* pControl = GetControl(i);
			pControl->SetHideFlag(xtpHideScroll, FALSE);

			if (!pControl->IsVisible())
				continue;

			int nItemHeight = pControl->GetRect().Height();
			if (pControl->GetBeginGroup()) nPos += nSeparator;

			if (sz.cy - (nPos + nItemHeight) < nScrollHeight + rcBorders.bottom)
			{
				if (!bFirst)
					break;
				else
					nItemHeight = sz.cy - rcBorders.bottom - rcBorders.top;
			}

			bFirst = FALSE;
			pControl->SetRect(CRect(rcBorders.left, nPos, sz.cx - rcBorders.right, nPos + nItemHeight));

			nPos += nItemHeight;
		}
		for (int j = i; j < nCount; j++)
		{
			GetControl(j)->SetHideFlag(xtpHideScroll, TRUE);
		}

		m_scrollInfo.nScrollLast = i;

		if (i != nCount)
		{
			m_scrollInfo.btnUp.rc.SetRect(rcBorders.left, sz.cy - rcBorders.bottom - nScrollHeight, sz.cx - rcBorders.right, sz.cy - rcBorders.bottom);
		}
	}
	else
	{
		if (m_scrollInfo.nScrollLast == nCount - 1)
			m_scrollInfo.nScrollLast = nCount;

		int nPos = sz.cy - rcBorders.bottom;
		if (m_scrollInfo.nScrollLast != nCount)
		{
			nPos -= nScrollHeight;
			m_scrollInfo.btnUp.rc.SetRect(rcBorders.left, sz.cy - rcBorders.bottom - nScrollHeight, sz.cx - rcBorders.right, sz.cy - rcBorders.bottom);
		}

		ASSERT(m_scrollInfo.nScrollLast <= nCount && m_scrollInfo.nScrollLast > 0);

		for (i = m_scrollInfo.nScrollLast - 1 ; i >= 0 ; i--)
		{
			CXTPControl* pControl = GetControl(i);
			pControl->SetHideFlag(xtpHideScroll, FALSE);

			if (!pControl->IsVisible())
				continue;

			int nItemHeight = pControl->GetRect().Height();

			if (nPos - nItemHeight < nScrollHeight)
			{
				break;
			}
			pControl->SetRect(CRect(rcBorders.left, nPos - nItemHeight, sz.cx - rcBorders.right, nPos));

			nPos -= nItemHeight;
			if (pControl->GetBeginGroup()) nPos -= nSeparator;
		}

		for (int j = i; j >= 0; j--)
		{
			GetControl(j)->SetHideFlag(xtpHideScroll, TRUE);
		}

		m_scrollInfo.nScrollFirst = i + 1;

		if (i != -1)
		{
			m_scrollInfo.btnDown.rc.SetRect(rcBorders.left, rcBorders.top, sz.cx - rcBorders.right, rcBorders.top + nScrollHeight);
		}
	}
	if (m_bCollapsed)
	{
		pButtonExpand->SetHideFlags(xtpNoHide);
		pButtonExpand->SetRect(CRect(rcBorders.left, sz.cy - rcBorders.bottom, sz.cx - rcBorders.right, sz.cy - rcBorders.bottom +  szButtonExpand.cy));
	}
	m_rcTearOffGripper.SetRect(rcBorders.left, rcBorders.top - szTearOffGripper.cy, sz.cx - rcBorders.right, rcBorders.top);

	if (bInvalidate) Redraw();
}

void CXTPCommandBar::UpdateShortcuts()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (!pCommandBars)
		return;

	if (!pCommandBars->GetCommandBarsOptions()->bAutoUpdateShortcuts)
		return;

	for (int i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl || pControl->GetParent() != this || !pControl->IsVisible())
			continue;

		CString strShortcut;
		if (pControl->GetID() != 0)
		{
			pCommandBars->GetShortcutManager()->FindDefaultAccelerator(pControl->GetID(), strShortcut);
			pControl->m_strShortcutTextAuto = strShortcut;
		}
	}
}

void CXTPPopupBar::AdjustExcludeRect(CRect& rc, BOOL bVertical)
{
	m_pControlPopup->AdjustExcludeRect(rc, bVertical);
}

AFX_INLINE BOOL IsVerticalPosition(CXTPCommandBar* pCommandBar)
{
	return IsVerticalPosition(pCommandBar->GetPosition()) ||
		(pCommandBar->GetType() == xtpBarTypePopup && pCommandBar->GetPosition() == xtpBarFloating);
}

void CXTPPopupBar::UpdateFlags()
{
	if (m_pControlPopup)
	{
		BOOL bLayoutRTL = IsLayoutRTL();


		CXTPCommandBar* pParent = GetParentCommandBar();
		ASSERT(pParent);

		m_popupFlags = xtpPopupRight;

		if (m_pControlPopup->m_dwFlags & xtpFlagLeftPopup || bLayoutRTL) m_popupFlags |= xtpPopupLeft;

		if (pParent->GetFlags() & xtpFlagSmartLayout && pParent->GetPosition() == xtpBarPopup)
		{
			m_popupFlags |= xtpPopupDown;
		}
		else if (!IsVerticalPosition(pParent))
		{
			m_popupFlags |= xtpPopupDown;
		}
		else
		{
			if (pParent->GetPosition() == xtpBarRight || (pParent->GetType() == xtpBarTypePopup &&
				pParent->IsPopupBar() && ((CXTPPopupBar*)pParent)->m_popupFlags & xtpPopupLeft))
				m_popupFlags = xtpPopupLeft;
		}

		m_rcExclude = m_pControlPopup->GetRect();

		if (m_pControlPopup->GetType() == xtpControlComboBox && pParent->GetType() != xtpBarTypePopup)
		{
			m_popupFlags |= xtpPopupDown;
		}
		else AdjustExcludeRect(m_rcExclude, IsVerticalPosition(pParent));

		pParent->ClientToScreen(m_rcExclude);

		m_ptPopup.x = (m_popupFlags & xtpPopupDown) ? m_rcExclude.left : m_rcExclude.right;
		m_ptPopup.y = (m_popupFlags & xtpPopupDown) ? m_rcExclude.bottom : m_rcExclude.top;
	}
}


void CXTPPopupBar::UpdateExpandingState()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (!pCommandBars)
		return;

	int i;

	if (pCommandBars->GetCommandBarsOptions()->bAlwaysShowFullMenus)
	{
		for (i = 0; i < GetControlCount(); i++)
		{
			CXTPControl* pControl = GetControl(i);
			if (!pControl || pControl->GetParent() != this)
				continue;

			pControl->SetExpanded(FALSE);
		}
		m_bExpanded = TRUE;
		return;
	}

	m_bCollapsed = FALSE;

	BOOL bCollapsedExists = FALSE;
	for (i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl || pControl->GetParent() != this)
			continue;

		BOOL bVisible = pControl->IsVisible(xtpHideExpand | xtpHideScroll);

		if (pCommandBars->IsControlHidden(pControl))
		{
			pControl->SetExpanded(bVisible);
		}
		else
		{
			pControl->SetExpanded(FALSE);

			if (pControl->GetID() != XTP_ID_POPUPBAR_EXPAND && bVisible)
				bCollapsedExists = TRUE;
		}
	}

	for (i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl || pControl->GetParent() != this)
			continue;

		pControl->SetHideFlag(xtpHideExpand, FALSE);

		if (pControl->GetExpanded())
		{
			if (!m_bExpanded && bCollapsedExists)
			{
				pControl->SetHideFlag(xtpHideExpand, TRUE);
				m_bCollapsed = TRUE;
			}
		}
	}
}

void CXTPPopupBar::Animate()
{
	CXTPCommandBar* pParentCommandBar = GetParentCommandBar();

	BOOL bAnimate = pParentCommandBar ? pParentCommandBar->m_bAnimatePopup : TRUE;

	if (bAnimate)
	{
		if (IsCustomizeMode() || GetWindow(GW_CHILD))
			bAnimate = FALSE;
	}

	BOOL bRegionChanged = FALSE;


	if (bAnimate)
	{
		XTPAnimationType animationType = GetAnimationType();
		if (IsAnimateType(animationType))
		{

			if (m_pControlPopup)
			{
				m_pControlPopup->RedrawParent();
				m_pControlPopup->GetParent()->UpdateWindow();
			}

			GetSite()->UpdateWindow();

			CClientDC paintDC(this);
			CXTPClientRect rcClient(this);
			CXTPWindowRect rcWindow(this);

			CXTPBufferDC animationDC(paintDC, rcClient);
			DrawCommandBar(&animationDC, rcClient);

			CDC* pDC = GetDesktopWindow()->GetDC();

			BOOL dwLayout = CXTPDrawHelpers::IsContextRTL(&paintDC);
			CXTPDrawHelpers::SetContextRTL(&paintDC, FALSE);
			paintDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), pDC, rcWindow.left, rcWindow.top, SRCCOPY);
			GetDesktopWindow()->ReleaseDC(pDC);
			CXTPDrawHelpers::SetContextRTL(&paintDC, dwLayout);

			GetPaintManager()->SetCommandBarRegion(this);
			bRegionChanged = TRUE;
			// Animation
			GetPaintManager()->Animate(&paintDC, &animationDC, rcClient, animationType);
		}
	}

	if (pParentCommandBar) pParentCommandBar->m_bAnimatePopup = FALSE;
	if (!bRegionChanged) GetPaintManager()->SetCommandBarRegion(this);

}



BOOL CXTPPopupBar::Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst)
{
	m_pControlPopup = pControlPopup;

	if (!Create())
		return FALSE;

	if (m_pToolTipContext)
	{
		m_pToolTipContext->CancelToolTips();
	}

	LockRedraw();

	CWnd* pWndOwner = GetOwnerSite();
	if (pWndOwner) pWndOwner->SendMessage(WM_XTP_INITCOMMANDSPOPUP, 0, (LPARAM)this);

	m_bTearOffSelected = FALSE;

	m_bCollapsed = FALSE;
	m_bExpanded = IsCustomizeMode() || XTPMouseManager()->IsForceExpanded();

	UpdateFlags();

	UpdateLocation(CalcDynamicLayout(0, 0));

	if (!m_hWnd)
	{
		UnlockRedraw();
		return FALSE;
	}

	if (m_pControlPopup && m_pControlPopup->m_pParent->GetSafeHwnd() == NULL)
	{
		UnlockRedraw();
		DestroyWindow();
		return FALSE;
	}

	SetTrackingMode(TRUE, bSelectFirst, bSelectFirst);

	m_nIdleFlags &= ~xtpIdleLayout;
	OnIdleUpdateCmdUI(TRUE, 0L);

	if (!m_hWnd)
	{
		UnlockRedraw();
		return FALSE;
	}

	Animate();
	XTPSoundManager()->PlaySystemSound(xtpSoundMenuPopup);

	if (m_barPosition == xtpBarPopup && m_bShowShadow)
	{
		XTPShadowsManager()->RemoveShadow(this);
		XTPShadowsManager()->SetShadow(this, m_rcExclude);
	}

	m_nIdleFlags |= xtpIdleLayout;
	UnlockRedraw();

	return TRUE;
}

BOOL CXTPPopupBar::Popup(int x, int y, LPCRECT rcExclude)
{
	m_ptPopup = CPoint(x, y);
	if (rcExclude) m_rcExclude = *rcExclude;
	return Popup(NULL);
}

BEGIN_MESSAGE_MAP(CXTPPopupBar, CXTPCommandBar)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_NCACTIVATE()
	ON_WM_NCCREATE()
	ON_MESSAGE(WM_FLOATSTATUS, OnFloatStatus)
END_MESSAGE_MAP()

void CXTPPopupBar::SwitchTearOffTracking(BOOL bShow, CPoint point)
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	ASSERT(pCommandBars);

	ShowWindow(bShow ? SW_SHOWNA : SW_HIDE);

	if (!bShow)
	{
		if (m_pTearOffBar == NULL)
		{
			m_pTearOffBar = pCommandBars->GetToolBar(m_nTearOffID);
			if (m_pTearOffBar != NULL)
			{
				m_pTearOffBar->SetPosition(xtpBarFloating);
				m_pTearOffBar->SetVisible(TRUE);
				pCommandBars->RecalcFrameLayout();
			}
		}
		if (m_pTearOffBar == NULL)
		{

			m_pTearOffBar = pCommandBars->AddCustomBar(m_strTearOffCaption, m_nTearOffID, TRUE);
			m_pTearOffBar->m_nMRUWidth = m_nTearOffWidth;

			for (int i = 0; i < GetControlCount(); i++)
			{
				CXTPControl* pControl = GetControl(i);
				if (!pControl || pControl->GetParent() != this)
					continue;

				if (pControl->GetID() != XTP_ID_POPUPBAR_EXPAND)
				{
					CXTPControl* pControlClone = m_pTearOffBar->GetControls()->AddClone(pControl);
					pControlClone->SetHideFlags(pControlClone->GetHideFlags() & ~(xtpHideScroll | xtpHideExpand));
				}
			}

		}
		ASSERT(m_pTearOffBar);

		pCommandBars->GetSite()->SendMessage(WM_XTP_TEAROFFDONE, (WPARAM)m_pTearOffBar, (LPARAM)this);

		m_szTearOffBar = m_pTearOffBar->CalcDynamicLayout(-1, LM_MRUWIDTH | LM_HORZ | LM_COMMIT);
		CPoint pt(point.x - m_szTearOffBar.cx / 2, point.y - 10);
		m_pTearOffBar->MoveWindow(pt.x, pt.y, m_szTearOffBar.cx, m_szTearOffBar.cy);
		m_pTearOffBar->OnInitialUpdate();
		m_pTearOffBar->Redraw();


	}
	else
	{
		if (m_pTearOffBar)
		{
			m_pTearOffBar->SetVisible(FALSE);
			m_pTearOffBar = 0;
		}

	}
}

void CXTPPopupBar::TrackTearOff()
{
	m_pTearOffBar = NULL;
	CRect rcGripper = m_rcTearOffGripper;
	ClientToScreen(&rcGripper);

	BOOL bShow = TRUE;
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return;

	// set capture to the window which received this message
	SetCapture();
	ASSERT(this == CWnd::GetCapture());

	CPoint pt(0, 0);
	// get messages until capture lost or cancelled/accepted
	while (CWnd::GetCapture() == this)
	{
		MSG msg;

		while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
				break;
			DispatchMessage(&msg);
		}

		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		if (msg.message == WM_LBUTTONUP)
			break;
		else if (msg.message == WM_MOUSEMOVE && pt != msg.pt)
		{
			pt = msg.pt;
			BOOL bState = !(pt.x > rcGripper.right + 10 || pt.x < rcGripper.left - 10 ||
				pt.y > rcGripper.bottom + 10 || pt.y < rcGripper.top - 10);
			if (bState != bShow)
			{
				bShow = bState;
				SwitchTearOffTracking(bState, pt);
			}

			if (m_pTearOffBar != NULL)
			{
				CPoint point(pt.x - m_szTearOffBar.cx / 2, pt.y - 10);
				m_pTearOffBar->MoveWindow(point.x, point.y, m_szTearOffBar.cx, m_szTearOffBar.cy);

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

	ReleaseCapture();
	if (m_pTearOffBar)
	{
		GetCommandBars()->ClosePopups();
	}
}


void CXTPPopupBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bTearOffPopup && m_rcTearOffGripper.PtInRect(point) && !IsCustomizeMode())
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));

		XTPMouseManager()->LockMouseMove();
		m_bTearOffTracking = TRUE;
		TrackTearOff();
		m_bTearOffTracking = FALSE;
		XTPMouseManager()->UnlockMouseMove();

	}
	CXTPCommandBar::OnLButtonDown(nFlags, point);
}

BOOL CXTPPopupBar::DestroyWindow()
{
	if (!GetSafeHwnd())
		return CXTPCommandBar::DestroyWindow();

	CWnd* pWnd = GetWindow(GW_CHILD);
	if (!pWnd)
		return CXTPCommandBar::DestroyWindow();

	CWnd* pWndParent = GetSite();

	while (pWnd)
	{
		CWnd* pWndNext = pWnd->GetWindow(GW_HWNDNEXT);

		pWnd->ShowWindow(SW_HIDE);
		pWnd->SetParent(pWndParent);

		pWnd = pWndNext;
	}
	return CXTPCommandBar::DestroyWindow();
}

BOOL CXTPPopupBar::SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard)
{
	if (!CXTPCommandBar::SetTrackingMode(bMode, bSelectFirst, bKeyboard))
		return FALSE;

	CWnd* pWnd = GetSite();

	if (!bMode)
	{
		AccessibleNotifyWinEvent(EVENT_SYSTEM_MENUPOPUPEND, m_hWnd, OBJID_MENU, 0);

		DestroyWindow();

		CXTPCommandBar* pParentCommandBar = GetParentCommandBar();
		if (pParentCommandBar)
			pParentCommandBar->SetPopuped(-1);

		if (pWnd && ::IsWindow(pWnd->m_hWnd)) pWnd->SendMessage(WM_XTP_UNINITCOMMANDSPOPUP, 0, (LPARAM)this);

		Sleep(1);

		m_pControlPopup = 0;

	}
	else
	{
		AccessibleNotifyWinEvent(EVENT_SYSTEM_MENUPOPUPSTART, m_hWnd, OBJID_MENU, 0);
	}
	if (pWnd && ::IsWindow(pWnd->m_hWnd)) pWnd->UpdateWindow();

	return TRUE;
}


void CXTPPopupBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nLockRecurse > 0)
		return;

	if (GetParentCommandBar() != NULL && CXTPClientRect(this).PtInRect(point))
	{
		// restore selection
		GetParentCommandBar()->SetSelected(m_pControlPopup->GetIndex());
	}

	if (m_scrollInfo.bScroll)
	{
		if (m_scrollInfo.btnUp.OnMouseMove(point) ||
			m_scrollInfo.btnDown.OnMouseMove(point))
		{
			SetPopuped(-1);
		}
	}

	if (m_bTearOffPopup && !IsCustomizeMode() && m_rcTearOffGripper.PtInRect(point) && !m_bTearOffSelected)
	{
		SetSelected(-1);
		SetPopuped(-1);
		m_nTearOffTimer = SetTimer(TID_TEAROFFSELECTED, 80, NULL);
	}

	CXTPCommandBar::OnMouseMove(nFlags, point);
}

CXTPCommandBar* CXTPPopupBar::GetParentCommandBar() const
{
	return m_pControlPopup ? m_pControlPopup->GetParent() : NULL;
}

int CXTPPopupBar::OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	return MA_NOACTIVATE;
}

BOOL CXTPPopupBar::OnNcActivate(BOOL bActive)
{
	return bActive ? FALSE : CWnd::OnNcActivate(bActive);
}

BOOL CXTPPopupBar::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CXTPCommandBar::OnNcCreate(lpCreateStruct))
		return FALSE;

	if (GetStyle() & MFS_SYNCACTIVE)
	{
		// syncronize activation state with top level parent
		CWnd* pParentWnd = GetTopLevelParent();
		ASSERT(pParentWnd != NULL);
		CWnd* pActiveWnd = GetForegroundWindow();
		BOOL bActive = (pParentWnd == pActiveWnd) ||
			(pParentWnd->GetLastActivePopup() == pActiveWnd &&
			pActiveWnd->SendMessage(WM_FLOATSTATUS, FS_SYNCACTIVE) != 0);

		// the WM_FLOATSTATUS does the actual work
		SendMessage(WM_FLOATSTATUS, bActive ? FS_ACTIVATE : FS_DEACTIVATE);
	}


	return TRUE;
}


LRESULT CXTPPopupBar::OnFloatStatus(WPARAM wParam, LPARAM)
{
	// FS_SYNCACTIVE is used to detect MFS_SYNCACTIVE windows
	LRESULT lResult = ((GetStyle() & MFS_SYNCACTIVE) && (wParam & FS_SYNCACTIVE));
	return lResult;
}



void CXTPPopupBar::DrawCommandBar(CDC* pDC, CRect rcClipBox)
{
	CXTPCommandBar::DrawCommandBar(pDC, rcClipBox);

	if (m_scrollInfo.bScroll)
	{
		if (!m_scrollInfo.btnUp.rc.IsRectEmpty())
		{
			CPoint pt = m_scrollInfo.btnUp.rc.CenterPoint();
			GetPaintManager()->Triangle(pDC, CPoint(pt.x - 3, pt.y), CPoint(pt.x + 3, pt.y), CPoint(pt.x, pt.y + 3), GetXtremeColor(COLOR_BTNTEXT));
		}
		if (!m_scrollInfo.btnDown.rc.IsRectEmpty())
		{
			CPoint pt = m_scrollInfo.btnDown.rc.CenterPoint();
			GetPaintManager()->Triangle(pDC, CPoint(pt.x - 3, pt.y), CPoint(pt.x + 3, pt.y), CPoint(pt.x, pt.y - 3), GetXtremeColor(COLOR_BTNTEXT));
		}
	}

	if (m_bTearOffPopup && GetControlCount() > 0)
	{
		GetPaintManager()->DrawTearOffGripper(pDC, m_rcTearOffGripper, m_bTearOffSelected, TRUE);
	}
}


BOOL CXTPPopupBar::SetSelected(int nSelected, BOOL bKeyboard)
{
	if (!CXTPCommandBar::SetSelected(nSelected, bKeyboard))
		return FALSE;

	if (m_nSelected != -1 && !bKeyboard && (GetControl(m_nSelected)->GetID() == XTP_ID_POPUPBAR_EXPAND))
	{
		CXTPCommandBars* pCommandBars = GetCommandBars();
		ASSERT(pCommandBars);

		if (pCommandBars->GetCommandBarsOptions()->bShowFullAfterDelay)
		{
			SetTimer(TID_EXPANDHOVER, s_nExpandHoverDelay, NULL);
		}
	}
	else
	{
		KillTimer(TID_EXPANDHOVER);

	}

	if (m_scrollInfo.bScroll && m_nSelected != -1)
	{
		if (GetControl(m_nSelected)->GetHideFlags() & xtpHideScroll)
		{
			if (m_nSelected >= m_scrollInfo.nScrollLast)
			{
				m_scrollInfo.nScrollLast = m_nSelected + 1;
				AdjustScrolling(NULL, TRUE, TRUE);
			}
			else
			{
				m_scrollInfo.nScrollFirst = m_nSelected;
				AdjustScrolling(NULL, FALSE, TRUE);
			}
		}
	}

	return TRUE;
}

void CXTPPopupBar::OnTimer(UINT_PTR nIDEvent)
{
	int nCount = GetControlCount();

	switch (nIDEvent)
	{
		case TID_HOVERUP:
			if (m_scrollInfo.nScrollLast == nCount)
			{
				m_scrollInfo.btnUp.KillTimer();
				return;
			}
			m_scrollInfo.nScrollLast++;
			AdjustScrolling(NULL, TRUE, TRUE);

			break;

		case TID_HOVERDOWN:
			if (m_scrollInfo.nScrollFirst == 0)
			{
				m_scrollInfo.btnDown.KillTimer() ;
				return;
			}
			m_scrollInfo.nScrollFirst--;
			AdjustScrolling(NULL, FALSE, TRUE);
			break;

		case TID_TEAROFFSELECTED:
			{
				CPoint pt;
				GetCursorPos(&pt);
				ScreenToClient(&pt);
				if (!m_bTearOffSelected && m_rcTearOffGripper.PtInRect(pt))
				{
					m_bTearOffSelected = TRUE;
					Redraw();
				}
				if (m_bTearOffSelected && !m_rcTearOffGripper.PtInRect(pt) && !m_bTearOffTracking)
				{
					m_bTearOffSelected = FALSE;
					Redraw();
					KillTimer(m_nTearOffTimer);
				}
			}
			break;

		case TID_EXPANDHOVER:
			if (m_nSelected != -1 && GetControl(m_nSelected)->GetID() == XTP_ID_POPUPBAR_EXPAND)
			{
				GetControl(m_nSelected)->OnExecute();
			}

			KillTimer(TID_EXPANDHOVER);
			break;
	}

	CXTPCommandBar::OnTimer(nIDEvent);
}

BOOL CXTPPopupBar::OnHookKeyDown(UINT nChar, LPARAM lParam)
{
	if (m_bCollapsed)
	{
		if (nChar == VK_DOWN)
		{
			if (GetKeyState(VK_CONTROL) < 0 || m_pControls->GetNext(m_nSelected, +1) <= m_nSelected)
			{
				ExpandBar();
			}
		}
	}
	return CXTPCommandBar::OnHookKeyDown(nChar, lParam);
}

void CXTPPopupBar::SetDefaultItem(UINT uItem, BOOL fByPos)
{
	CXTPControl* pControl = fByPos ? m_pControls->GetAt(uItem) : m_pControls->FindControl(xtpControlError, uItem, TRUE, FALSE);
	if (pControl)
	{
		pControl->SetItemDefault(TRUE);
	}
}

UINT CXTPPopupBar::GetDefaultItem(UINT /*gmdiFlags*/, BOOL fByPos)
{
	for (int i = 0; i < GetControlCount(); i++)
	{
		CXTPControl* pControl = GetControl(i);
		if (!pControl || pControl->GetParent() != this || !pControl->IsVisible())
			continue;

		if (pControl->IsItemDefault())
		{
			return fByPos ? pControl->GetIndex() : pControl->GetID();
		}
	}
	return (UINT)-1;
}

void CXTPPopupBar::Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive)
{
	ASSERT_KINDOF(CXTPPopupBar, pCommandBar);

	CXTPCommandBar::Copy(pCommandBar, bRecursive);

	m_bTearOffPopup = ((CXTPPopupBar*)pCommandBar)->m_bTearOffPopup;
	m_strTearOffCaption = ((CXTPPopupBar*)pCommandBar)->m_strTearOffCaption;
	m_nTearOffID = ((CXTPPopupBar*)pCommandBar)->m_nTearOffID;
	m_nTearOffWidth = ((CXTPPopupBar*)pCommandBar)->m_nTearOffWidth;

	m_bShowShadow = ((CXTPPopupBar*)pCommandBar)->m_bShowShadow;
	m_rcBorders = ((CXTPPopupBar*)pCommandBar)->m_rcBorders;
	m_bContextMenu = ((CXTPPopupBar*)pCommandBar)->m_bContextMenu;
}

void CXTPPopupBar::SetPopupToolBar(BOOL bToolBarType)
{
	m_barType = bToolBarType ? xtpBarTypeNormal : xtpBarTypePopup;
}

////////////////////////////////////////////////////////////////////////////
//

IMPLEMENT_XTP_COMMANDBAR(CXTPPopupToolBar, CXTPPopupBar)

CXTPPopupToolBar::CXTPPopupToolBar()
{
	m_barType = xtpBarTypeNormal;
}

CSize CXTPPopupToolBar::CalcDynamicLayout(int, DWORD /*nMode*/)
{
	return CXTPPopupBar::CalcDynamicLayout(0, 0);
}

CXTPPopupToolBar* CXTPPopupToolBar::CreatePopupToolBar(CXTPCommandBars* pCommandBars)
{
	CXTPPopupToolBar* pPopupBar = (CXTPPopupToolBar*)CXTPCommandBars::m_pPopupToolBarClass->CreateObject();
	pPopupBar->SetCommandBars(pCommandBars);
	return pPopupBar;
}


