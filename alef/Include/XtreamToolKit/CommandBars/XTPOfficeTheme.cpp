// XTPOfficeTheme.cpp : implementation of the CXTPOfficeTheme class.
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
#include "XTPControlEdit.h"
#include "XTPControls.h"
#include "XTPCommandBars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace XTPPaintThemes;

#ifndef DT_HIDEPREFIX
#define DT_HIDEPREFIX 0x00100000
#endif //!DT_HIDEPREFIX


CXTPOfficeTheme::CXTPOfficeTheme()
{
	m_bFlatMenuBar = TRUE;

	m_iconsInfo.bUseFadedIcons = TRUE;
	m_iconsInfo.bIconsWithShadow = TRUE;
	m_iconsInfo.bUseDisabledIcons = TRUE;
	m_iconsInfo.bOfficeStyleDisabledIcons = TRUE;

	m_bEmbossedDisabledText = FALSE;
	m_nShadowOptions = xtpShadowOfficeAlpha + xtpShadowShowPopupControl;

	m_nPopupBarTextPadding = 8;
}

void CXTPOfficeTheme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType /*barType*/, XTPBarPosition barPosition /*= xtpBarPopup*/)
{
	if (!bEnabled)
	{
		if (IsKeyboardSelected(bSelected))
			Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, barPosition != xtpBarPopup ? (bChecked ? XPCOLOR_HIGHLIGHT_PUSHED : XPCOLOR_HIGHLIGHT) : XPCOLOR_MENUBAR_FACE);
		else if (bChecked)
			Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_DISABLED_BORDER, barPosition != xtpBarPopup ? XPCOLOR_HIGHLIGHT_CHECKED : -1);
		return;
	}
	if (bPopuped) Rectangle(pDC, rc, XPCOLOR_MENUBAR_BORDER, XPCOLOR_TOOLBAR_FACE);
	else if (bChecked == 2 && !bSelected && !bPressed) Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_3DFACE);
	else if (bChecked && !bSelected && !bPressed) Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED);
	else if (bChecked && bSelected && !bPressed) Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED);
	else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED);
	else if (bSelected || bPressed) Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT);
}


void CXTPOfficeTheme::DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, BOOL)
{
	if (!bEnabled)
	{
		if (!m_iconsInfo.bUseDisabledIcons && !pImage->IsAlpha())
			pImage->DrawMono(pDC, pt, pImage->GetIcon(), sz, GetXtremeColor(XPCOLOR_DISABLED));
		else
		{
			if (m_iconsInfo.bOfficeStyleDisabledIcons && !pImage->HasDisabledIcon())
			{
				pImage->CreateDisabledIcon(m_clrDisabledIcon.clrLight, m_clrDisabledIcon.clrDark);
			}
			pImage->Draw(pDC, pt, pImage->GetDisabledIcon(), sz);
		}
	}
	else if (bChecked == 2 && !bPressed)
	{
		if (!pImage->IsAlpha())
			pImage->DrawMono(pDC, pt, pImage->GetIcon(), sz, GetXtremeColor(XPCOLOR_DISABLED));
		else
		{
			if (m_iconsInfo.bOfficeStyleDisabledIcons && !pImage->HasDisabledIcon())
			{
				pImage->CreateDisabledIcon(m_clrDisabledIcon.clrLight, m_clrDisabledIcon.clrDark);
			}
			pImage->Draw(pDC, pt, pImage->GetDisabledIcon(), sz);
		}
	}
	else if (bPopuped || bChecked)
		pImage->Draw(pDC, pt, bChecked ? pImage->GetCheckedIcon() : pImage->GetIcon(), sz);
	else if (!bSelected && !bPressed)
	{
		pImage->Draw(pDC, pt, m_iconsInfo.bUseFadedIcons ? pImage->GetFadedIcon() : pImage->GetIcon(), sz);
	}
	else if ((bSelected && !bPressed) || (!bSelected && bPressed == TRUE))
	{
		if (m_iconsInfo.bIconsWithShadow && !pImage->GetShadowIcon().IsEmpty())
		{
			pImage->Draw(pDC, CPoint(pt.x + 1, pt.y + 1), pImage->GetShadowIcon(), sz);
			pImage->Draw(pDC, CPoint(pt.x - 1, pt.y - 1), pImage->GetHotIcon(), sz);
		}
		else
		{
			pImage->Draw(pDC, pt, pImage->GetHotIcon(), sz);

		}
	}
	else if ((bSelected && bPressed) || (IsKeyboardSelected(bPressed)))
		pImage->Draw(pDC, pt, pImage->GetPressedIcon(), sz);
}




CSize CXTPOfficeTheme::DrawControlEdit(CDC* pDC, CXTPControlEdit* pControlEdit, BOOL bDraw)
{
	if (!bDraw)
	{
		return CXTPPaintManager::DrawControlEdit(pDC, pControlEdit, bDraw);
	}

	BOOL bPopupBar = pControlEdit->GetParent()->GetType() == xtpBarTypePopup;
	BOOL bSelected = pControlEdit->GetSelected(), bEnabled = pControlEdit->GetEnabled();

	CXTPPaintManager::DrawControlEdit(pDC, pControlEdit, bDraw);

	CRect rc = pControlEdit->GetRect();
	rc.left += pControlEdit->GetLabelWidth();

	if (!bEnabled)
	{
		rc.DeflateRect(1, 0, 0, 0);
		Rectangle (pDC, rc, IsKeyboardSelected(bSelected) ? XPCOLOR_HIGHLIGHT_BORDER : XPCOLOR_DISABLED, COLOR_3DFACE);
	}
	else
	{
		pDC->FillSolidRect(rc.left + 1, rc.top + 1, rc.Width() - (pControlEdit->IsSpinButtonsVisible() ? 18 : 2),
			rc.Height() - 2, GetXtremeColor(COLOR_WINDOW));

		if (bSelected)
		{
			Draw3dRect(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_BORDER);
		}
		else if (bPopupBar)
		{
			Draw3dRect(pDC, rc, XPCOLOR_EDITCTRLBORDER, XPCOLOR_EDITCTRLBORDER);
		}
	}

	if (pControlEdit->IsSpinButtonsVisible())
	{
		BOOL bPressed = pControlEdit->GetPressed();
		CRect rcSpin(pControlEdit->GetSpinButtonsRect());
		CRect rcTop(rcSpin.left, rcSpin.top, rcSpin.right, rcSpin.CenterPoint().y);
		CRect rcBottom(rcSpin.left, rcTop.bottom, rcSpin.right, rcSpin.bottom);

		if (!bEnabled)
		{

		}
		else if (!bSelected)
		{
			pDC->Draw3dRect(rcSpin, GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_WINDOW));
		}
		else
		{
			pDC->FillSolidRect(rcSpin, GetXtremeColor(XPCOLOR_HIGHLIGHT));

			if (bPressed == 3)
				pDC->FillSolidRect(rcTop,  GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED));
			else if (bPressed == 4)
				pDC->FillSolidRect(rcBottom,  GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED));

			pDC->FillSolidRect(rcSpin.left, rcSpin.top, 1, rcSpin.Height(), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
			pDC->FillSolidRect(rcSpin.left, rcBottom.top, rcSpin.Width(), 1, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
		}

		CPoint pt = rcTop.CenterPoint();
		Triangle(pDC, CPoint(pt.x, pt.y), CPoint(pt.x - 2, pt.y + 2), CPoint(pt.x + 2, pt.y + 2),
			GetXtremeColor(bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT));

		pt = rcBottom.CenterPoint();
		Triangle(pDC, CPoint(pt.x, pt.y), CPoint(pt.x - 2, pt.y - 2), CPoint(pt.x + 2, pt.y - 2),
			GetXtremeColor(bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT));

	}

	return 0;
}


CSize CXTPOfficeTheme::DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pControlCombo, BOOL bDraw)
{
	if (!bDraw)
	{
		return CXTPPaintManager::DrawControlComboBox(pDC, pControlCombo, bDraw);
	}

	BOOL bPopupBar = pControlCombo->GetParent()->GetType() == xtpBarTypePopup;

	CXTPFontDC font(pDC, GetIconFont());

	CRect rc = pControlCombo->GetRect();
	BOOL bDropped = pControlCombo->GetDroppedState() != 0;
	BOOL bSelected = pControlCombo->GetSelected();
	BOOL bEnabled = pControlCombo->GetEnabled();

	CXTPPaintManager::DrawControlComboBox(pDC, pControlCombo, bDraw);

	rc.left += pControlCombo->GetLabelWidth();

	int nThumb = pControlCombo->GetThumbWidth();
	CRect rcBtn (rc.right - nThumb, rc.top, rc.right, rc.bottom);
	CRect rcText(rc.left + 3, rc.top + 1, rc.right - rcBtn.Width(), rc.bottom - 1);

	if (!bEnabled)
	{
		rc.DeflateRect(1, 0, 0, 0);
		Rectangle (pDC, rc, IsKeyboardSelected(bSelected) ? XPCOLOR_HIGHLIGHT_BORDER : XPCOLOR_DISABLED, COLOR_3DFACE);
		rcBtn.DeflateRect(1, 1);
	}
	else
	{
		CRect rcEntry(rc.left + 1, rc.top + 1, rc.right - nThumb + 1, rc.bottom -1);
		Rectangle (pDC, rcEntry, COLOR_WINDOW, COLOR_WINDOW);

		if (bSelected)
		{
			Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, COLOR_WINDOW);
			DrawRectangle(pDC, rcBtn, TRUE, bDropped, TRUE, FALSE, FALSE,
				pControlCombo->GetParent()->GetType(), pControlCombo->GetParent()->GetPosition());

			rcBtn.DeflateRect(1, 1);
		}
		else
		{
			rcBtn.DeflateRect(1, 1);

			if (bPopupBar)
			{
				Draw3dRect(pDC, rc, XPCOLOR_EDITCTRLBORDER, XPCOLOR_EDITCTRLBORDER);
				Rectangle(pDC, rcBtn, COLOR_WINDOW, XPCOLOR_EDITCTRLBORDER);
			}
			else
			{
				Draw3dRect(pDC, rcBtn, COLOR_WINDOW, COLOR_WINDOW);
			}
		}
	}

	pControlCombo->DrawEditText(pDC, rcText);

	DrawComboExpandMark(pDC, rcBtn, GetXtremeColor(!pControlCombo->GetEnabled() ? XPCOLOR_DISABLED: bDropped ? XPCOLOR_PUSHED_TEXT: bSelected ? XPCOLOR_HIGHLIGHT_TEXT : XPCOLOR_TOOLBAR_TEXT));

	return 0;
}


void CXTPOfficeTheme::DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded)
{
	pDC->FillSolidRect(x, y, cx, cy, GetXtremeColor(bExpanded ? XPCOLOR_MENUBAR_EXPANDED : XPCOLOR_TOOLBAR_FACE));
}

void CXTPOfficeTheme::DrawSplitButtonPopup(CDC* pDC, CXTPControl* pButton)
{
	CRect rcButton = pButton->GetRect();
	BOOL bSelected = pButton->GetSelected(), bPopuped = pButton->GetPopuped(), bEnabled = pButton->GetEnabled();

	if (!bPopuped || !bSelected || !bEnabled)
		VerticalLine(pDC, rcButton.right - m_nSplitButtonPopupWidth, rcButton.top + 1, rcButton.bottom - 1,
		!bEnabled ? GetControlTextColor(pButton) : bSelected ? GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER) : GetXtremeColor(COLOR_3DSHADOW));
	else
		Rectangle(pDC, CRect(rcButton.right - m_nSplitButtonPopupWidth, rcButton.top, rcButton.right, rcButton.bottom), XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_PUSHED);

}

void CXTPOfficeTheme::DrawControlPopupGlyph(CDC* pDC, CXTPControl* pButton)
{
	CRect rcButton = pButton->GetRect();
	BOOL bSelected = pButton->GetSelected(), bPopuped = pButton->GetPopuped(), bEnabled = pButton->GetEnabled();
	CPoint pt = CPoint(rcButton.right - 11, rcButton.CenterPoint().y);

	Triangle(pDC, CPoint(pt.x, pt.y - 3), CPoint(pt.x + 3, pt.y), CPoint(pt.x, pt.y + 3),
		pButton->GetType() == xtpControlSplitButtonPopup && bEnabled && bPopuped && bSelected ?
		GetXtremeColor(XPCOLOR_PUSHED_TEXT) : GetControlTextColor(pButton));
}

void CXTPOfficeTheme::FillPopupLabelEntry(CDC* pDC, CRect rc)
{
	pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_LABEL));
}

CSize CXTPOfficeTheme::DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw)
{
	CXTPCommandBar* pParent = pButton->GetParent();

	CRect rcButton = pButton->GetRect();
	XTPControlType controlType = pButton->GetType();
	XTPButtonStyle buttonStyle = pButton->GetStyle();

	BOOL bDrawImage = (buttonStyle == xtpButtonAutomatic && controlType != xtpControlPopup) ||
		(buttonStyle != xtpButtonCaption);

	BOOL bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked();

	if (controlType == xtpControlSplitButtonPopup && bEnabled == TRUE_SPLITDROPDOWN)
		bEnabled = FALSE;

	BOOL bLayoutRTL = pParent->IsLayoutRTL();
	BOOL bDoubleGripper = pParent->IsPopupBar() ? ((CXTPPopupBar*)pParent)->IsDoubleGripper() : FALSE;

	CString strCaption = pButton->GetCaption();
	if (bLayoutRTL) UpdateRTLCaption(strCaption);

	CSize szIcon = GetIconSize(pButton);
	CSize szCheck(szIcon.cx + 4, szIcon.cy + 4);
	CSize szImage(max(szIcon.cx + 4, m_nTextHeight + 4), max(szIcon.cy + 6, m_nTextHeight));
	int nGripperWidth = bDoubleGripper ? szImage.cx * 2 : szImage.cx;

	if (pButton->GetType() == xtpControlLabel && !m_bOffsetPopupLabelText)
		nGripperWidth = 0;
	if (pButton->GetType() == xtpControlLabel && !bDrawImage)
		szImage.cy = m_nTextHeight;

	if (bDraw)
	{
		if (pButton->GetExpanded() && rcButton.left < nGripperWidth)
		{
			DrawPopupBarGripper(pDC, rcButton.left - 1, rcButton.top, nGripperWidth + 1, rcButton.Height(), TRUE);
		}

		if (pButton->GetType() == xtpControlLabel)
		{
			if (!pParent->GetShowGripper()) nGripperWidth = 0;
			int nLabelGripper = rcButton.left > nGripperWidth ? 0 : nGripperWidth + 1;

			FillPopupLabelEntry(pDC, CRect(rcButton.left + nLabelGripper, rcButton.top, rcButton.right, rcButton.bottom));
		}
		else
			DrawRectangle(pDC, rcButton, bSelected, FALSE, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

		CRect rcImageRect(rcButton.left, rcButton.top, rcButton.left + szImage.cx, rcButton.bottom);
		if (!pParent->GetShowGripper())
			rcImageRect.OffsetRect(m_nPopupBarTextPadding / 2, 0);

		CRect rcCheck (CPoint(rcImageRect.left + rcImageRect.Width()/2 - szCheck.cx/2, rcImageRect.top + rcImageRect.Height()/2 - szCheck.cy/2), szCheck);

		if (bDoubleGripper)
			rcImageRect.OffsetRect(szImage.cx, 0);


		if (bChecked)
			DrawRectangle(pDC, rcCheck, bSelected, FALSE, bEnabled, bChecked, FALSE, xtpBarTypePopup, xtpBarPopup);

		CXTPImageManagerIcon* pImage = pButton->GetIconId() != 0 && bDrawImage ? pButton->GetImage(szIcon.cx) : NULL;
		if (pImage)
		{
			szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();
			CPoint ptImage = CPoint(rcImageRect.left + rcImageRect.Width()/2 - szIcon.cx/2, rcImageRect.top + rcImageRect.Height()/2 - szIcon.cy/2);
			DrawImage(pDC, ptImage, szIcon, pImage, bSelected, FALSE, bEnabled, bChecked, FALSE, FALSE);
		}
		if (bChecked && (bDoubleGripper || !pImage))
		{
			if (controlType != xtpControlRadioButton)
				DrawCheckMark(pDC, rcCheck, !bEnabled ? GetXtremeColor(XPCOLOR_DISABLED) : 0);
			else
				DrawRadioMark(pDC, rcCheck, !bEnabled ? GetXtremeColor(XPCOLOR_DISABLED) : 0);
		}


		COLORREF clrText = GetControlTextColor(pButton);

		pDC->SetTextColor(clrText);
		pDC->SetBkMode (TRANSPARENT);

		if (IsPopupControlType(controlType))
		{
			if (pButton->GetType() == xtpControlSplitButtonPopup)
			{
				DrawSplitButtonPopup(pDC, pButton);
			}
			DrawControlPopupGlyph(pDC, pButton);
		}
	}

	return CXTPPaintManager::DrawControlPopupParent(pDC, pButton, bDraw);
}


COLORREF CXTPOfficeTheme::GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition)
{
	int nColorFace = barPosition == xtpBarPopup ? XPCOLOR_MENUBAR_TEXT : XPCOLOR_TOOLBAR_TEXT;

	if (barType == xtpBarTypePopup)
	{
		return GetXtremeColor(!bEnabled ? XPCOLOR_MENUBAR_GRAYTEXT :
			bSelected ? XPCOLOR_HIGHLIGHT_TEXT : XPCOLOR_MENUBAR_TEXT);
	}

	COLORREF clrText = GetXtremeColor(!bEnabled ? XPCOLOR_TOOLBAR_GRAYTEXT :
		bPopuped ? XPCOLOR_TOOLBAR_TEXT :
		bChecked && !bSelected && !bPressed ? XPCOLOR_CHECKED_TEXT :
		IsKeyboardSelected(bPressed) || (bSelected && (bPressed || bChecked)) ? XPCOLOR_PUSHED_TEXT :
		bSelected || bPressed ? XPCOLOR_HIGHLIGHT_TEXT : nColorFace);

	return clrText;
}

void CXTPOfficeTheme::FillIntersectRect(CDC* pDC, CXTPPopupBar* pPopupBar, COLORREF clr)
{
	if (pPopupBar && pPopupBar->GetControlPopup())
	{
		CXTPCommandBar* pParentBar = pPopupBar->GetControlPopup()->GetParent();
		if (!pParentBar || pParentBar->GetType() == xtpBarTypePopup)
			return;

		CRect rcParent;
		pParentBar->GetWindowRect(rcParent);
		CRect rcExclude = pPopupBar->GetControlPopup()->GetRect();
		pParentBar->ClientToScreen(&rcExclude);

		CRect rcWindow;
		pPopupBar->GetWindowRect(&rcWindow);

		CRect rcIntersect;

		if (rcIntersect.IntersectRect(rcWindow, rcExclude))
		{
			pPopupBar->ScreenToClient(rcIntersect);

			if (rcIntersect.Width() > 1) rcIntersect.DeflateRect(1, 0);
			if (rcIntersect.Height() > 1) rcIntersect.DeflateRect(0, 1);

			pDC->FillSolidRect(rcIntersect, clr);
		}
	}
}

void CXTPOfficeTheme::FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarFloating)
	{
		COLORREF clrPen = GetXtremeColor(XPCOLOR_FLOATBAR_BORDER);
		COLORREF clrBrush =  GetXtremeColor(pBar->GetType() == xtpBarTypePopup ? XPCOLOR_MENUBAR_FACE :
			IsFlatToolBar(pBar) ? COLOR_3DFACE : XPCOLOR_TOOLBAR_FACE);

		pDC->Draw3dRect(rc, clrPen, clrPen);
		rc.DeflateRect(1, 1);

		pDC->FillSolidRect(rc, clrBrush);
		pDC->Draw3dRect(rc, clrPen, clrPen);

		pDC->SetPixel(2, 2, clrPen);
		pDC->SetPixel(rc.right - 2, 2, clrPen);
		pDC->SetPixel(2, rc.bottom - 2, clrPen);
		pDC->SetPixel(rc.right - 2, rc.bottom - 2, clrPen);
	}
	else if (pBar->GetPosition() == xtpBarPopup)
	{
		Rectangle(pDC, rc, XPCOLOR_MENUBAR_BORDER, XPCOLOR_MENUBAR_FACE);

		FillIntersectRect(pDC, (CXTPPopupBar*)pBar, GetXtremeColor(XPCOLOR_TOOLBAR_FACE));
	}
	else if (IsFlatToolBar(pBar))
	{
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));
	}
	else
	{
		COLORREF clr3DFace = GetXtremeColor(COLOR_3DFACE);

		pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_TOOLBAR_FACE));
		pDC->Draw3dRect(rc, clr3DFace, clr3DFace);

		pDC->SetPixel(1, 1, clr3DFace);
		pDC->SetPixel(rc.right - 2, 1, clr3DFace);
		pDC->SetPixel(1, rc.bottom - 2, clr3DFace);
		pDC->SetPixel(rc.right - 2, rc.bottom - 2, clr3DFace);
	}
}

CSize CXTPOfficeTheme::DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* /*pBar*/, BOOL bDraw)
{
	CSize sz(6, max(22, m_nTextHeight + 4));

	if (pDC && bDraw)
	{
		CXTPPenDC pen (*pDC, GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		for (int y = 6; y < sz.cy - 2; y += 2)
			Line(pDC, CPoint(3, y), CPoint(6, y));
	}

	return sz;
}

CSize CXTPOfficeTheme::DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw)
{
	if (pBar->IsDialogBar())
		return DrawDialogBarGripper(pDC, (CXTPDialogBar*)pBar, bDraw);

	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarPopup && pBar->GetType() == xtpBarTypePopup)
	{
		CRect rcBorders = pBar->GetBorders();
		DrawPopupBarGripper(pDC, rcBorders.left - 1, rcBorders.top, GetPopupBarGripperWidth(pBar), rc.bottom - rcBorders.top - rcBorders.bottom);
	}
	if (pBar->GetPosition() == xtpBarFloating)
	{
		CSize sz = CXTPPaintManager::DrawCommandBarGripper(pDC, pBar, bDraw);

		if (pBar->GetType() == xtpBarTypePopup && pBar->GetShowGripper())
		{
			CRect rcBorders = pBar->GetBorders();
			int nTop = rcBorders.top + sz.cy + 1;
			DrawPopupBarGripper(pDC, rcBorders.left - 1, nTop,
				GetPopupBarGripperWidth(pBar), rc.bottom - nTop - rcBorders.bottom);
		}

		return sz;
	}
	else if (pBar->GetPosition() == xtpBarRight || pBar->GetPosition() == xtpBarLeft)
	{
		if (pDC && bDraw)
		{
			CXTPPenDC pen (*pDC, GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
			for (int x = 5; x < rc.Width() - 5; x += 2)
				Line(pDC, CPoint(x, 3), CPoint(x, 6));
		}
		return CSize(0, 8);
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		if (pDC && bDraw)
		{
			CXTPPenDC pen (*pDC, GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
			for (int y = 5; y < rc.Height() - 5; y += 2)
				Line(pDC, CPoint(3, y), CPoint(6, y));
		}
		return CSize(6, 0);
	}
	return 0;
}

CSize CXTPOfficeTheme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	if (!bDraw)
	{
		if (pBar->GetType() == xtpBarTypePopup)
			return CSize(3, 3);

		return CSize(6, 6);
	}

	CRect rc;
	pBar->GetClientRect(&rc);
	CRect rcControl = pControl->GetRect();
	CRect rcRow = pControl->GetRowRect();

	if (pBar->GetType() == xtpBarTypePopup)
	{
		if (!pControl->GetWrap())
			HorizontalLine(pDC, rcControl.left - 1 + GetPopupBarGripperWidth(pBar) + m_nPopupBarTextPadding,
				rcControl.top - 2, rcControl.right + 1, GetXtremeColor(XPCOLOR_SEPARATOR));
		else
			VerticalLine(pDC, rcControl.left - 2, rcRow.top - 1, rcRow.bottom + 1, GetXtremeColor(XPCOLOR_SEPARATOR));
	}
	else if (pBar->GetPosition() != xtpBarPopup && IsVerticalPosition(pBar->GetPosition()))
	{
		if (!pControl->GetWrap())
			HorizontalLine(pDC, rcRow.left, rcControl.top - 4, rcRow.right, GetXtremeColor(XPCOLOR_SEPARATOR));
		else
			VerticalLine(pDC, rcRow.right + 2, rcRow.top + 4, rcRow.bottom, GetXtremeColor(XPCOLOR_SEPARATOR));

	}
	else
	{
		if (!pControl->GetWrap())
			VerticalLine(pDC, rcControl.left - 4, rcRow.top, rcRow.bottom, GetXtremeColor(XPCOLOR_SEPARATOR));
		else
			HorizontalLine(pDC, rcRow.left, rcRow.top - 4, rcRow.right, GetXtremeColor(XPCOLOR_SEPARATOR));
	}
	return 0;
}
void CXTPOfficeTheme::AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical)
{
	CXTPCommandBar* pParent = pControl->GetParent();
	ASSERT(pParent);

	if (pParent->GetType() != xtpBarTypePopup)
	{
		if (pControl->GetType() == xtpControlComboBox) return;

		if (pParent->GetFlags() & xtpFlagSmartLayout && pParent->GetPosition() == xtpBarPopup)
		{
			rc.DeflateRect(0, 1);
		}
		else if (bVertical)
			rc.DeflateRect(1, 0);
		else
			rc.DeflateRect(0, 1);
	}
	else
		rc.InflateRect(1, 0);
}

void CXTPOfficeTheme::DrawStatusBarPaneBorder(CDC* pDC, CRect rc, BOOL /*bGripperPane*/)
{
	Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_3DSHADOW);
}

CSize CXTPOfficeTheme::DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rcButton, BOOL bSelected, BOOL bDraw, CXTPCommandBars* pCommandBars)
{
	CSize szIcon = pCommandBars && pCommandBars->GetCommandBarsOptions()->szPopupIcons != CSize(0) ?
		pCommandBars->GetCommandBarsOptions()->szPopupIcons : GetAutoIconSize(FALSE);
	CSize szImage(max(szIcon.cx + 4, m_nTextHeight + 4), max(szIcon.cy + 6, m_nTextHeight));

	if (bDraw)
	{
		pDC->FillSolidRect(rcButton, GetXtremeColor(XPCOLOR_MENUBAR_FACE));
		DrawPopupBarGripper(pDC, 0, rcButton.top, szImage.cx + 1, rcButton.Height());

		if (pButton == NULL)
		{
			return 0;
		}


		XTPControlType controlType = pButton->GetType();
		XTPButtonStyle buttonStyle = pButton->GetStyle();

		if (bSelected)
			DrawRectangle(pDC, rcButton, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

		BOOL bDrawImage = (buttonStyle == xtpButtonAutomatic && controlType != xtpControlPopup) ||
			(buttonStyle == xtpButtonIcon) || (buttonStyle == xtpButtonIconAndCaption);

		CXTPImageManagerIcon* pImage = pButton->GetIconId() != 0 && bDrawImage ? pButton->GetImage(szIcon.cx) : NULL;
		if (pImage)
		{
			szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();
			CPoint ptImage = CPoint(rcButton.left + szImage.cx/2 - szIcon.cx/2, rcButton.top + rcButton.Height()/2 - szIcon.cy/2);
			DrawImage(pDC, ptImage, szIcon, pImage, bSelected, FALSE, TRUE, FALSE, FALSE, FALSE);
		}


		COLORREF clrText = bSelected ? GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT) : GetXtremeColor(XPCOLOR_MENUBAR_TEXT);

		pDC->SetTextColor(clrText);
		pDC->SetBkMode (TRANSPARENT);

		CString strText = pButton->GetCaption();
		StripMnemonics(strText);

		if (pCommandBars && pCommandBars->IsLayoutRTL())
			UpdateRTLCaption(strText);

		CRect rcLabelText(rcButton.left + szImage.cx + 8, rcButton.top, rcButton.right, rcButton.bottom);

		if (controlType == xtpControlPopup || controlType == xtpControlButtonPopup || controlType == xtpControlSplitButtonPopup)
		{
			CPoint pt(rcButton.right - 7, rcButton.CenterPoint().y);
			Triangle(pDC, pt, CPoint(pt.x - 5, pt.y - 5), CPoint(pt.x - 5, pt.y + 5), clrText);
			rcLabelText.right -= 7;
		}
		if (controlType == xtpControlSplitButtonPopup)
		{
			Line(pDC, rcButton.right - 20, rcButton.top + 1, rcButton.right - 20, rcButton.bottom - 1, bSelected ? XPCOLOR_HIGHLIGHT_BORDER : COLOR_3DSHADOW);
			rcLabelText.right = rcButton.right - 20;
		}
		if (controlType == xtpControlComboBox || controlType == xtpControlEdit)
		{
			CRect rcText(0, 0, 0, 0);
			pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_CALCRECT);
			int nThumb = pDC->GetTextExtent(_T(" "), 1).cy + 2;

			CRect rcCombo(rcButton.left + szImage.cx + 8 + rcText.Width() + 5, rcButton.top, rcButton.right, rcButton.bottom);
			CRect rcThumb(rcButton.right - nThumb, rcButton.top, rcButton.right, rcButton.bottom);

			Rectangle(pDC, rcCombo, bSelected ? XPCOLOR_HIGHLIGHT_BORDER : COLOR_3DFACE, COLOR_WINDOW);
			if (controlType == xtpControlComboBox)
			{
				if (!bSelected) pDC->FillSolidRect(rcThumb.left + 2, rcThumb.top + 2, rcThumb.Width() - 4, rcThumb.Height() - 4, GetXtremeColor(COLOR_3DFACE));
				else DrawRectangle(pDC, rcThumb, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

				rcThumb.DeflateRect(1, 1, 1, 1);
				DrawComboExpandMark(pDC, rcThumb, GetXtremeColor(bSelected ? XPCOLOR_HIGHLIGHT_TEXT : XPCOLOR_TOOLBAR_TEXT));
			}
		}

		CXTPFontDC fnt(pDC, &m_fontRegular);
		pDC->DrawText(strText, &rcLabelText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

	}
	return szImage;
}


CSize CXTPOfficeTheme::DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc(1, 1, rcGripper.right + 1, rcGripper.Height() + 1);
		pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_MENUBAR_FACE));
		rc.DeflateRect(1, 1);

		int nLeft = rc.CenterPoint().x - min(16, rcGripper.Width() /2 - 10);
		int nRight = rc.CenterPoint().x + min(16, rcGripper.Width() /2 - 10);

		if (bSelected)
		{
			Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT);
			if (nLeft < nRight)
			{
				Line(pDC, nLeft, 4, nRight, 4, XPCOLOR_HIGHLIGHT_TEXT);
				Line(pDC, nLeft, 6, nRight, 6, XPCOLOR_HIGHLIGHT_TEXT);
			}

		}
		else
		{
			pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_TOOLBAR_FACE));

			if (nLeft < nRight)
			{
				Line(pDC, nLeft, 3, nRight, 3, XPCOLOR_TOOLBAR_GRIPPER);
				Line(pDC, nLeft, 5, nRight, 5, XPCOLOR_TOOLBAR_GRIPPER);
				Line(pDC, nLeft, 7, nRight, 7, XPCOLOR_TOOLBAR_GRIPPER);
			}
		}
	}

	return CSize(0, 9);
}

CSize CXTPOfficeTheme::DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled)
{
	if (m_bThemedCheckBox && m_themeButton.IsAppThemed())
		return CXTPPaintManager::DrawControlCheckBoxMark(pDC, rc, bDraw, bSelected, bPressed, bChecked, bEnabled);

	if (!bDraw)
		return CSize(13, 13);

	if (!bEnabled)
	{
		Draw3dRect(pDC, rc, XPCOLOR_GRAYTEXT, XPCOLOR_GRAYTEXT);
	}
	else
	{
		Rectangle(pDC, rc, bSelected || bPressed ? XPCOLOR_HIGHLIGHT_BORDER : XPCOLOR_3DSHADOW,  bSelected && bPressed ?
			XPCOLOR_HIGHLIGHT_PUSHED : bPressed || bSelected ? XPCOLOR_HIGHLIGHT : COLOR_WINDOW);
	}

	if (bChecked)
	{
		CPoint pt = rc.CenterPoint();
		CXTPPenDC pen(pDC->GetSafeHdc(), GetXtremeColor(!bEnabled || bChecked == 2 ? XPCOLOR_GRAYTEXT : COLOR_BTNTEXT));

		if (CXTPDrawHelpers::IsContextRTL(pDC))  // Revert Check Box
		{
			for (int i = 0; i < 3; i++)
			{
				pDC->MoveTo(pt.x + 3, pt.y - 1 + i);
				pDC->LineTo(pt.x + 1, pt.y + 1 + i);
				pDC->LineTo(pt.x - 4, pt.y - 4 + i);
			}
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				pDC->MoveTo(pt.x - 3, pt.y - 1 + i);
				pDC->LineTo(pt.x - 1, pt.y + 1 + i);
				pDC->LineTo(pt.x + 4, pt.y - 4 + i);
			}
		}
	}

	return CSize(13, 13);
}

CSize CXTPOfficeTheme::DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled)
{
	if (m_bThemedCheckBox && m_themeButton.IsAppThemed())
		return CXTPPaintManager::DrawControlRadioButtonMark(pDC, rc, bDraw, bSelected, bPressed, bChecked, bEnabled);

	if (!bDraw)
		return CSize(13, 13);

	if (!bEnabled)
	{
		AlphaEllipse(pDC, rc, GetXtremeColor(XPCOLOR_GRAYTEXT), GetXtremeColor(XPCOLOR_3DFACE));
	}
	else
	{
		AlphaEllipse(pDC, rc, GetXtremeColor(bSelected || bPressed ? XPCOLOR_HIGHLIGHT_BORDER : XPCOLOR_3DSHADOW),
			GetXtremeColor(bSelected && bPressed ? XPCOLOR_HIGHLIGHT_PUSHED :
				bPressed || bSelected ? XPCOLOR_HIGHLIGHT : COLOR_WINDOW));
	}

	if (bChecked)
	{
		COLORREF clrFace = GetXtremeColor(!bEnabled || bChecked == 2 ? XPCOLOR_GRAYTEXT : COLOR_BTNTEXT);
		AlphaEllipse(pDC, CRect(rc.left + 4, rc.top + 4, rc.left + 9, rc.top + 9), clrFace, clrFace);
	}

	return CSize(13, 13);
}


void CXTPOfficeTheme::DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton)
{
	BOOL bPressed = pButton->GetPressed(), bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked(),
		bPopuped = pButton->GetPopuped();

	if (pButton->GetStyle() == xtpButtonIconAndCaptionBelow)
	{
		int nSplitDropDownHeight = GetSplitDropDownHeight();

		BOOL bCommandPressed = ((bSelected && bPressed) && !bPopuped && bEnabled);
		BOOL bVert = pButton->GetParent()->GetPosition() == xtpBarLeft ||
			pButton->GetParent()->GetPosition() == xtpBarRight;

		DrawRectangle(pDC, CRect(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom),
			bSelected, bPressed && !bCommandPressed, bEnabled, bChecked, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());

		if ((bSelected || bPressed) && !bPopuped && bEnabled)
		{
			if (bVert)
				pDC->FillSolidRect(rcButton.left + nSplitDropDownHeight, rcButton.top, 1, rcButton.Height(), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
			else
				pDC->FillSolidRect(rcButton.left, rcButton.bottom - nSplitDropDownHeight - 1, rcButton.Width(), 1, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
		}

		if (bCommandPressed)
		{
			if (bVert)
				DrawRectangle(pDC, CRect(rcButton.left + nSplitDropDownHeight, rcButton.top, rcButton.right, rcButton.bottom),
					bSelected, bPressed, bEnabled, bChecked, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());
			else
				DrawRectangle(pDC, CRect(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom - nSplitDropDownHeight),
					bSelected, bPressed, bEnabled, bChecked, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());
		}
		return;
	}

	DrawRectangle(pDC, CRect(rcButton.left, rcButton.top, rcButton.right - (bPopuped ? 0 : m_nSplitButtonDropDownWidth), rcButton.bottom),
		bSelected, bPressed, bEnabled, bChecked, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());

	if ((bSelected || bPressed) && !bPopuped && bEnabled)
		DrawRectangle(pDC, CRect(rcButton.right - m_nSplitButtonDropDownWidth - 1, rcButton.top, rcButton.right, rcButton.bottom),
		TRUE, FALSE, TRUE, FALSE, FALSE, xtpBarTypeNormal, pButton->GetParent()->GetPosition());

	CPoint pt = CPoint(rcButton.right - 7, rcButton.CenterPoint().y);
	if (bEnabled == TRUE_SPLITCOMMAND) bEnabled = FALSE;

	DrawDropDownGlyph(pDC, pButton, pt, bSelected, bPopuped, bEnabled, FALSE);
}
