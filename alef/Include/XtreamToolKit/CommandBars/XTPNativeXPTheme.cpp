// XTPNativeXPTheme.cpp : implementation of the CXTPNativeXPTheme class.
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
#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPPaintManager.h"
#include "XTPControlButton.h"
#include "XTPControlPopup.h"
#include "Common/XTPColorManager.h"
#include "XTPToolBar.h"
#include "XTPPopupBar.h"
#include "XTPControlComboBox.h"
#include "XTPDockBar.h"
#include "XTPControlEdit.h"

#define COLOR_MENUHILIGHT       29

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SPI_GETFLATMENU
#define SPI_GETFLATMENU    0x1022
#endif


using namespace XTPPaintThemes;


CXTPNativeXPTheme::CXTPNativeXPTheme()
{
	m_iconsInfo.bUseFadedIcons = TRUE;
	m_iconsInfo.bIconsWithShadow = FALSE;

	m_nPopupBarText = COLOR_MENUTEXT;
	m_bFlatMenus = TRUE;

	m_bThemedStatusBar = TRUE;
	m_bThemedCheckBox = TRUE;
}

CXTPNativeXPTheme::~CXTPNativeXPTheme()
{
}

void CXTPNativeXPTheme::RefreshMetrics()
{
	CXTPDefaultTheme::RefreshMetrics();

	HWND hWnd = AfxGetMainWnd() ? AfxGetMainWnd()->GetSafeHwnd() : 0;
	m_themeToolbar.OpenThemeData(hWnd, L"TOOLBAR");
	m_themeRebar.OpenThemeData(hWnd, L"REBAR");
	m_themeCombo.OpenThemeData(hWnd, L"COMBOBOX");
	m_themeWindow.OpenThemeData(hWnd, L"WINDOW");
	m_themeSpin.OpenThemeData(hWnd, L"SPIN");

	m_bFlatMenus = TRUE;
	if (!SystemParametersInfo(SPI_GETFLATMENU, 0, &m_bFlatMenus, 0))
	{
		m_bFlatMenus = FALSE;
	}

	if (!m_themeRebar.IsAppThemed() || FAILED(m_themeRebar.GetThemeColor(RP_BAND, 0, TMT_EDGESHADOWCOLOR, &m_clrEdgeShadowColor)))
		m_clrEdgeShadowColor = GetXtremeColor(COLOR_3DSHADOW);

	if (!m_themeRebar.IsAppThemed() || FAILED(m_themeRebar.GetThemeColor(RP_BAND, 0, TMT_EDGEHIGHLIGHTCOLOR, &m_clrEdgeHighLightColor)))
		m_clrEdgeHighLightColor  = GetXtremeColor(COLOR_BTNHILIGHT);

	COLORREF clr;
	if (m_themeToolbar.IsAppThemed() && SUCCEEDED(m_themeToolbar.GetThemeColor(0, 0, TMT_TEXTCOLOR, &clr)))
		m_arrColor[COLOR_BTNTEXT].SetStandardValue(clr);

	if (!m_bFlatMenus && m_themeRebar.IsAppThemed())
	{
		m_bFlatMenus = m_themeRebar.GetThemeSysBool(TMT_FLATMENUS);
	}

	if (!m_bFlatMenus)
	{
		m_arrColor[COLOR_MENUHILIGHT].SetStandardValue(m_arrColor[COLOR_HIGHLIGHT]);
	}
}

void CXTPNativeXPTheme::DrawControlEntry(CDC* pDC, CXTPControl* pButton)
{
	CXTPCommandBar* pParent = pButton->GetParent();

	if (!IsThemeEnabled() || !pParent)
	{
		CXTPDefaultTheme::DrawControlEntry(pDC, pButton);
		return;
	}

	if (pButton->GetType() == xtpControlPopup && pParent->GetPosition() != xtpBarPopup && IsFlatToolBar(pParent) && pParent->GetType() == xtpBarTypeMenuBar)
	{
		if (pButton->GetSelected() || pButton->GetPopuped() || pButton->GetPressed())
		{
			pDC->FillSolidRect(pButton->GetRect(), GetXtremeColor(COLOR_HIGHLIGHT));
		}
	}
	else
	{
		CXTPDefaultTheme::DrawControlEntry(pDC, pButton);
	}
}

COLORREF CXTPNativeXPTheme::GetControlTextColor(CXTPControl* pButton)
{
	CXTPCommandBar* pParent = pButton->GetParent();

	if (!IsThemeEnabled() || !pParent)
		return CXTPDefaultTheme::GetControlTextColor(pButton);

	if (pButton->GetType() == xtpControlPopup && pParent->GetPosition() != xtpBarPopup && IsFlatToolBar(pParent) && pParent->GetType() == xtpBarTypeMenuBar)
	{
		if (pButton->GetSelected() || pButton->GetPopuped() || pButton->GetPressed())
		{
			return GetXtremeColor(COLOR_HIGHLIGHTTEXT);
		}
	}
	return CXTPDefaultTheme::GetControlTextColor(pButton);
}

void CXTPNativeXPTheme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled , BOOL bChecked , BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition /*= xtpBarPopup*/)
{
	if (!IsThemeEnabled())
	{
		CXTPDefaultTheme::DrawRectangle(pDC, rc, bSelected, bPressed, bEnabled , bChecked , bPopuped, barType, barPosition);
		return;
	}

	if (barType == xtpBarTypePopup && !bChecked)
	{
		if (bSelected || bPressed)
		{
			Rectangle(pDC, rc, COLOR_HIGHLIGHT, COLOR_MENUHILIGHT);
		}
		return;
	}

	if (bChecked == 2 && bEnabled)
		bChecked = FALSE;

	int nState = !bEnabled ? (bChecked ? TS_PRESSED : TS_DISABLED) :
		bPopuped ? TS_PRESSED :
		bChecked && !bPressed ? (bSelected ? TS_HOTCHECKED : TS_CHECKED) :
		bSelected && bPressed || IsKeyboardSelected(bPressed) ? TS_PRESSED :
		bSelected ? TS_HOT : TS_NORMAL;

	m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_BUTTON, nState, &rc, 0);

}
void CXTPNativeXPTheme::FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	if (!IsThemeEnabled())
	{
		CXTPDefaultTheme::FillCommandBarEntry(pDC, pBar);
		return;
	}
	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarPopup)
	{
		if (m_bFlatMenus)
		{
			Rectangle(pDC, rc, COLOR_3DSHADOW, COLOR_MENU);
		}
		else
		{
			pDC->FillSolidRect(rc, GetXtremeColor(COLOR_MENU));
			Draw3dRect(pDC, rc, COLOR_3DFACE, COLOR_3DDKSHADOW);
			rc.DeflateRect(1, 1);
			Draw3dRect(pDC, rc, COLOR_BTNHILIGHT, COLOR_3DSHADOW);
		}
	}
	else if (pBar->GetPosition() == xtpBarFloating)
	{
		if (pBar->IsDialogBar())
		{
			CXTPDefaultTheme::FillCommandBarEntry(pDC, pBar);
			return;
		}

		if (pBar->GetType() == xtpBarTypePopup)
			pDC->FillSolidRect(rc, GetXtremeColor(COLOR_MENU));
		else
		{
			pDC->FillSolidRect(rc, GetXtremeColor(pBar->GetType() == xtpBarTypePopup ? COLOR_MENU : COLOR_3DFACE));
			m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), 0, 0, &rc, NULL);
		}

		m_themeWindow.DrawThemeBackground(pDC->GetSafeHdc(), WP_SMALLFRAMELEFT, CS_ACTIVE, CRect(rc.left, rc.top, rc.left + 3, rc.bottom), 0);
		m_themeWindow.DrawThemeBackground(pDC->GetSafeHdc(), WP_SMALLFRAMERIGHT, CS_ACTIVE, CRect(rc.right - 3, rc.top, rc.right, rc.bottom), 0);
		m_themeWindow.DrawThemeBackground(pDC->GetSafeHdc(), WP_SMALLFRAMEBOTTOM, CS_ACTIVE, CRect(rc.left, rc.bottom - 3, rc.right, rc.bottom), 0);
	}
	else
	{
		if (pBar->GetPosition() == xtpBarTop)
			FillDockBarRect(pDC, pBar, pBar->GetParent());
		else pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));

		m_themeRebar.DrawThemeBackground(pDC->GetSafeHdc(), RP_BAND, 0, &rc, NULL);

		if (!IsFlatToolBar(pBar))
		{
			pDC->Draw3dRect(rc, m_clrEdgeHighLightColor, m_clrEdgeShadowColor);
		}
	}
}

void CXTPNativeXPTheme::FillDockBarRect(CDC* pDC, CWnd* pWnd, CWnd* pParent)
{
	if (IsThemeEnabled())
	{
		CRect rc, rcParent, rcClient, rcClipRect;
		pWnd->GetClientRect(&rcClient);
		rcClipRect = rcClient;

		if (pParent)
		{
			pParent->GetWindowRect(rcClient);
			pWnd->ScreenToClient(rcClient);
		}

		m_themeRebar.DrawThemeBackground(pDC->GetSafeHdc(), 0, 0, &rcClient, &rcClipRect);
	}
	else
		pDC->FillSolidRect(CXTPClientRect(pWnd), GetXtremeColor(COLOR_3DFACE));
}

void CXTPNativeXPTheme::FillDockBar(CDC* pDC, CXTPDockBar* pBar)
{
	if (pBar->GetPosition() == xtpBarTop)
		FillDockBarRect(pDC, pBar, pBar);
	else pDC->FillSolidRect(CXTPClientRect(pBar), GetXtremeColor(COLOR_3DFACE));

}

CSize CXTPNativeXPTheme::DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw)
{
	if (!IsThemeEnabled())
	{
		return CXTPDefaultTheme::DrawDialogBarGripper(pDC, pBar, bDraw);
	}

	CSize sz(8, max(22, m_nTextHeight + 4));

	if (pDC && bDraw)
	{
		CRect rc(2, 3, 8, sz.cy);
		m_themeRebar.DrawThemeBackground(pDC->GetSafeHdc(), RP_GRIPPER , 1, &rc, 0);
	}

	return sz;
}

BOOL CXTPNativeXPTheme::IsThemeEnabled() const
{
	return m_themeRebar.IsAppThemed() && m_themeToolbar.IsAppThemed();
}


CSize CXTPNativeXPTheme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	if (!IsThemeEnabled() || !bDraw || pBar->GetType() == xtpBarTypePopup || pBar->GetPosition() == xtpBarPopup)
	{
		return CXTPDefaultTheme::DrawCommandBarSeparator(pDC, pBar, pControl, bDraw);
	}

	CRect rc;
	pBar->GetClientRect(&rc);
	CRect rcControl = pControl->GetRect();
	CRect rcRow = pControl->GetRowRect();

	if (IsVerticalPosition(pBar->GetPosition()))
	{
		if (!pControl->GetWrap())
			m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_SEPARATORVERT, 0, CRect(rcRow.left, rcControl.top - 6, rcRow.right, rcControl.top), NULL);
		else
			m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_SEPARATOR, 0, CRect(rcRow.right, rcRow.top + 4, rcRow.right + 5, rcRow.bottom), NULL);
	}
	else
	{
		if (!pControl->GetWrap())
			m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_SEPARATOR, 0, CRect(rcControl.left - 6, rcRow.top, rcControl.left, rcRow.bottom), NULL);
		else
			m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_SEPARATORVERT, 0, CRect(rcRow.left + 4, rcRow.top - 5, rcRow.right, rcRow.top), NULL);
	}
	return 0;
}

CSize CXTPNativeXPTheme::DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw)
{
	if (!IsThemeEnabled())
	{
		return CXTPDefaultTheme::DrawCommandBarGripper(pDC, pBar, bDraw);
	}

	if (pBar->IsDialogBar())
		return DrawDialogBarGripper(pDC, (CXTPDialogBar*)pBar, bDraw);

	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarFloating)
	{
		ASSERT(pDC);
		CXTPFontDC font(pDC, GetSmCaptionFont());
		CSize sz = pDC->GetTextExtent(_T(" "), 1);

		rc.SetRect(0, 0, rc.right, 3 + max(15, sz.cy));

		if (pDC && bDraw)
		{
			m_themeWindow.DrawThemeBackground(pDC->GetSafeHdc(), WP_SMALLCAPTION, CS_ACTIVE, rc, 0);

			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_clrFloatingGripperText);
			pDC->DrawText(pBar->GetTitle(), CRect(5, 3, rc.right + 3 - 2 * rc.Size().cy, rc.bottom), DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
		}
		return CSize(rc.Width(), max(15, sz.cy));
	}
	else if (pBar->GetPosition() == xtpBarRight || pBar->GetPosition() == xtpBarLeft)
	{
		if (pDC && bDraw)
		{
			rc.top += 2;
			rc.bottom = rc.top + 6;
			m_themeRebar.DrawThemeBackground(pDC->GetSafeHdc(), RP_GRIPPERVERT , 1, &rc, 0);
		}
		return CSize(0, 8);
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		if (pDC && bDraw)
		{
			rc.left += 2;
			rc.right = rc.left + 6;
			m_themeRebar.DrawThemeBackground(pDC->GetSafeHdc(), RP_GRIPPER , 1, &rc, 0);
		}
		return CSize(8, 0);
	}
	return 0;

}

void CXTPNativeXPTheme::AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical)
{
	if (!IsThemeEnabled())
	{
		CXTPDefaultTheme::AdjustExcludeRect(rc, pControl, bVertical);
		return;
	}
	CXTPCommandBar* pParent = pControl->GetParent();
	ASSERT(pParent);

	if (pControl->GetType() == xtpControlComboBox) return;

	if (pParent->GetType() != xtpBarTypePopup)
	{
		if (bVertical)
			rc.DeflateRect(0, 0);
		else
			rc.DeflateRect(0, 0);
	}
	else
		rc.InflateRect(-3, 3);

}

void CXTPNativeXPTheme::DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton)
{
	if (!IsThemeEnabled())
	{
		CXTPDefaultTheme::DrawSplitButtonFrame(pDC, pButton, rcButton);
		return;
	}

	BOOL bPressed = pButton->GetPressed(), bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked(),
		bPopuped = pButton->GetPopuped();

	UINT nState = !bEnabled ? TS_DISABLED :
		bPressed && bSelected ? TS_PRESSED :
		bChecked ? TS_CHECKED :
		bPopuped || bSelected ? TS_HOT : TS_NORMAL;

	CRect rc(rcButton.left, rcButton.top, rcButton.right - 12, rcButton.bottom);
	m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_SPLITBUTTON, nState, &rc, 0);


	nState = bPopuped ? TS_PRESSED : nState;
	if (bEnabled == TRUE_SPLITCOMMAND) nState = TS_DISABLED;

	CRect rcArrow(rcButton.right - 12, rcButton.top, rcButton.right, rcButton.bottom);
	m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_SPLITBUTTONDROPDOWN, nState, &rcArrow, 0);
}

void CXTPNativeXPTheme::DrawControlEditSpin(CDC* pDC, CXTPControlEdit* pControlEdit)
{
	if (!IsThemeEnabled())
	{
		CXTPDefaultTheme::DrawControlEditSpin(pDC, pControlEdit);
		return;
	}

	BOOL bPressed = pControlEdit->GetPressed();
	BOOL bEnabled = pControlEdit->GetEnabled();
	BOOL bSelected = pControlEdit->GetSelected();

	CRect rcSpin(pControlEdit->GetSpinButtonsRect());
	CRect rcTop(rcSpin.left, rcSpin.top, rcSpin.right, rcSpin.CenterPoint().y);
	CRect rcBottom(rcSpin.left, rcTop.bottom, rcSpin.right, rcSpin.bottom);

	m_themeSpin.DrawBackground(pDC->GetSafeHdc(), SPNP_UP,
		!bEnabled ? UPS_DISABLED : bSelected && bPressed == 3? UPS_PRESSED :
		bSelected == 3 ? UPS_HOT : UPS_NORMAL, rcTop, 0);

	m_themeSpin.DrawBackground(pDC->GetSafeHdc(), SPNP_DOWN,
		!bEnabled ? DNS_DISABLED : bSelected && bPressed == 4? DNS_PRESSED :
		bSelected == 4 ? DNS_HOT : DNS_NORMAL, rcBottom, 0);
}

void CXTPNativeXPTheme::DrawControlEditFrame(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected)
{
	if (!m_themeCombo.IsAppThemed())
	{
		CXTPDefaultTheme::DrawControlEditFrame(pDC, rc, bEnabled, bSelected);
		return;
	}

	int nStateID = !bEnabled ? CBXS_DISABLED : CBXS_NORMAL;

	COLORREF clrFrame = GetXtremeColor(COLOR_3DSHADOW);
	m_themeCombo.GetThemeColor(CP_DROPDOWNBUTTON, nStateID, TMT_BORDERCOLOR, &clrFrame);

	Draw3dRect(pDC, rc, clrFrame, clrFrame);
}

void CXTPNativeXPTheme::DrawControlComboBoxButton(CDC* pDC, CRect rcBtn, BOOL bEnabled, BOOL bSelected, BOOL bDropped)
{
	if (!m_themeCombo.IsAppThemed())
	{
		CXTPDefaultTheme::DrawControlComboBoxButton(pDC, rcBtn, bEnabled, bSelected, bDropped);
		return;
	}

	int nStateID = !bEnabled ? CBXS_DISABLED :
		bDropped ? CBXS_PRESSED: bSelected ? CBXS_HOT : CBXS_NORMAL;

	rcBtn.DeflateRect(-2, 1, 1, 1);
	m_themeCombo.DrawThemeBackground(pDC->GetSafeHdc(), CP_DROPDOWNBUTTON, nStateID, &rcBtn, 0);

}

CSize CXTPNativeXPTheme::DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam)
{
	if ((controlType == xtpButtonExpandFloating || controlType == xtpButtonHideFloating)
		&& m_themeToolbar.IsAppThemed())
	{
		return CXTPPaintManager::DrawSpecialControl(pDC, controlType, pButton, pBar, bDraw, lpParam);
	}

	return CXTPDefaultTheme::DrawSpecialControl(pDC, controlType, pButton, pBar, bDraw, lpParam);
}
