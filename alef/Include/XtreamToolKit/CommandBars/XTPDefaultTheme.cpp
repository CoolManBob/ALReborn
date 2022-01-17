// XTPDefaultTheme.cpp : implementation of the CXTPDefaultTheme class.
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
#include "XTPCommandBars.h"
#include "XTPControlComboBox.h"
#include "XTPControlEdit.h"
#include "XTPControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace XTPPaintThemes;

// CXTPDefaultTheme

CXTPDefaultTheme::CXTPDefaultTheme()
{
	m_iconsInfo.bUseFadedIcons = FALSE;
	m_iconsInfo.bIconsWithShadow = FALSE;

	m_nPopupBarText = COLOR_BTNTEXT;
}

void CXTPDefaultTheme::RefreshMetrics()
{
	CXTPPaintManager::RefreshMetrics();

	m_clrFloatingGripper.SetStandardValue(GetXtremeColor(COLOR_ACTIVECAPTION));
	m_clrFloatingGripperText.SetStandardValue(GetXtremeColor(COLOR_CAPTIONTEXT));
}

void CXTPDefaultTheme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition /*barPosition = xtpBarPopup*/)
{
	if (barType == xtpBarTypePopup && !bChecked)
	{
		if (bSelected || bPressed)
		{
			Rectangle(pDC, rc, COLOR_HIGHLIGHT, COLOR_HIGHLIGHT);
		}
		return;
	}

	if (!bEnabled)
	{
		if (bChecked) Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_BTNHILIGHT);
		if (IsKeyboardSelected(bSelected)) Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	}
	else if (bChecked && !bSelected && !bPressed)
	{
		DrawShadedRect(pDC, rc);
		Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	}
	else if (bChecked || bPopuped || IsKeyboardSelected(bPressed) || (bSelected && bPressed)) Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	else if (bSelected || bPressed) Draw3dRect(pDC, rc, COLOR_BTNHILIGHT, COLOR_3DSHADOW);
}

void CXTPDefaultTheme::DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, BOOL)
{
	if (!bEnabled)
	{
		if (!m_iconsInfo.bUseDisabledIcons && !pImage->IsAlpha())
		{
			pImage->DrawMono(pDC, pt + CPoint(1, 1), pImage->GetIcon(), sz, GetXtremeColor(COLOR_3DHILIGHT));
			pImage->DrawMono(pDC, pt, pImage->GetIcon(), sz, GetXtremeColor(COLOR_3DSHADOW));
		}
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
		{
			pImage->DrawMono(pDC, pt, pImage->GetCheckedIcon(), sz, GetXtremeColor(COLOR_3DSHADOW));
		}
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
		pImage->Draw(pDC, CPoint(pt.x + 1, pt.y + 1), pImage->GetPressedIcon(), sz);
}

void CXTPDefaultTheme::DrawControlEditSpin(CDC* pDC, CXTPControlEdit* pControlEdit)
{
	BOOL bPressed = pControlEdit->GetPressed();
	BOOL bEnabled = pControlEdit->GetEnabled();
	BOOL bSelected = pControlEdit->GetSelected();

	CRect rcSpin(pControlEdit->GetSpinButtonsRect());
	CRect rcTop(rcSpin.left + 1, rcSpin.top + 1, rcSpin.right - 1, rcSpin.CenterPoint().y);
	CRect rcBottom(rcSpin.left + 1, rcTop.bottom, rcSpin.right - 1, rcSpin.bottom - 1);

	if (!bEnabled)
	{

	}
	else if (!bSelected)
	{
		rcSpin.DeflateRect(1, 1);
		Rectangle(pDC, rcSpin, COLOR_WINDOW, COLOR_3DFACE);
	}
	else
	{
		pDC->FillSolidRect(rcSpin, GetXtremeColor(COLOR_3DFACE));

		if (bPressed == 3)
			pDC->Draw3dRect(rcTop, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_WINDOW));
		else
			pDC->Draw3dRect(rcTop, GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DSHADOW));

		if (bPressed == 4)
			pDC->Draw3dRect(rcBottom, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_WINDOW));
		else
			pDC->Draw3dRect(rcBottom, GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DSHADOW));
	}

	CPoint pt = rcTop.CenterPoint();
	Triangle(pDC, CPoint(pt.x, pt.y - 1), CPoint(pt.x - 2, pt.y + 1), CPoint(pt.x + 2, pt.y + 1),
		GetXtremeColor(bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT));

	pt = rcBottom.CenterPoint();
	Triangle(pDC, CPoint(pt.x, pt.y + 1), CPoint(pt.x - 2, pt.y - 1), CPoint(pt.x + 2, pt.y - 1),
		GetXtremeColor(bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT));
}

CSize CXTPDefaultTheme::DrawControlEdit(CDC* pDC, CXTPControlEdit* pControlEdit, BOOL bDraw)
{
	if (!bDraw)
	{
		return CXTPPaintManager::DrawControlEdit(pDC, pControlEdit, bDraw);
	}

	BOOL bEnabled = pControlEdit->GetEnabled();
	BOOL bSelected = pControlEdit->GetSelected();

	CXTPPaintManager::DrawControlEdit(pDC, pControlEdit, bDraw);

	CRect rc = pControlEdit->GetRect();
	rc.left += pControlEdit->GetLabelWidth();

	if (bEnabled)
	{
		CRect rcEntry(rc);
		rcEntry.DeflateRect(1, 1);
		Rectangle (pDC, rcEntry, COLOR_WINDOW, COLOR_WINDOW);
	}

	DrawControlEditFrame(pDC, rc, bEnabled, bSelected);

	if (pControlEdit->IsSpinButtonsVisible())
	{
		DrawControlEditSpin(pDC, pControlEdit);

	}
	return 0;
}

void CXTPDefaultTheme::DrawControlEditFrame(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected)
{
	if (!bEnabled)
	{
		rc.DeflateRect(1, 1);
		Draw3dRect(pDC, rc, IsKeyboardSelected(bSelected) ? COLOR_3DSHADOW: COLOR_3DHIGHLIGHT, IsKeyboardSelected(bSelected) ? COLOR_3DSHADOW : COLOR_3DHIGHLIGHT);
	}
	else
	{
		if (bSelected)
		{
			Draw3dRect(pDC, rc, COLOR_3DSHADOW, COLOR_3DHIGHLIGHT);
		}

		rc.DeflateRect(1, 1);
		Draw3dRect(pDC, rc, COLOR_3DFACE, COLOR_3DFACE);
	}
}
void CXTPDefaultTheme::DrawControlComboBoxButton(CDC* pDC, CRect rcBtn, BOOL bEnabled, BOOL bSelected, BOOL bDropped)
{
	rcBtn.DeflateRect(1, 2, 2, 2);

	if (bEnabled)
	{
		Rectangle (pDC, rcBtn, COLOR_3DFACE, COLOR_3DFACE);

		if (bDropped)
		{
			pDC->FillSolidRect(rcBtn.left - 1, rcBtn.top, 1, rcBtn.Height(), GetXtremeColor(COLOR_3DFACE));
			Draw3dRect(pDC, rcBtn, COLOR_3DSHADOW, COLOR_BTNHILIGHT);
			rcBtn.OffsetRect(1, 1);

		}
		else if (bSelected)
		{
			pDC->FillSolidRect(rcBtn.left - 1, rcBtn.top, 1, rcBtn.Height(), GetXtremeColor(COLOR_3DFACE));
			Draw3dRect(pDC, rcBtn, COLOR_BTNHILIGHT, COLOR_3DSHADOW);
		}
		else
		{
			Draw3dRect(pDC, rcBtn, COLOR_BTNHILIGHT, COLOR_BTNHILIGHT);
		}
	}

	DrawComboExpandMark(pDC, rcBtn, GetXtremeColor(COLOR_BTNTEXT));
}

CSize CXTPDefaultTheme::DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pControlCombo, BOOL bDraw)
{
	if (!bDraw)
	{
		return CXTPPaintManager::DrawControlComboBox(pDC, pControlCombo, bDraw);
	}

	CXTPFontDC font(pDC, GetIconFont());

	CRect rc = pControlCombo->GetRect();
	BOOL bEnabled = pControlCombo->GetEnabled();

	CXTPPaintManager::DrawControlComboBox(pDC, pControlCombo, bDraw);

	rc.left += pControlCombo->GetLabelWidth();

	int nThumb = pControlCombo->GetThumbWidth();
	CRect rcBtn (rc.right - nThumb, rc.top, rc.right, rc.bottom);

	BOOL bDropped = pControlCombo->GetDroppedState() != 0;
	BOOL bSelected = pControlCombo->GetSelected();

	if (bEnabled)
	{
		CRect rcEntry(rc);
		rcEntry.DeflateRect(1, 1);
		Rectangle (pDC, rcEntry, COLOR_WINDOW, COLOR_WINDOW);
	}

	CRect rcText(rc.left + 3, rc.top + 1, rc.right - rcBtn.Width() + 1, rc.bottom - 1);
	pControlCombo->DrawEditText(pDC, rcText);

	DrawControlComboBoxButton(pDC, rcBtn, bEnabled, bSelected, bDropped);
	DrawControlEditFrame(pDC, rc, bEnabled, bSelected);


	return 0;
}


CSize CXTPDefaultTheme::DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam)
{
	if (controlType == xtpButtonExpandFloating || controlType == xtpButtonHideFloating)
	{
		if (pButton->GetPressed() || pButton->GetSelected() || (controlType == xtpButtonExpandFloating && pButton->GetPopuped()))
			pDC->FillSolidRect(pButton->GetRect(), GetXtremeColor(COLOR_3DFACE));

	}

	return CXTPPaintManager::DrawSpecialControl(pDC, controlType, pButton, pBar, bDraw, lpParam);
}

CSize CXTPDefaultTheme::GetPopupBarImageSize(CXTPCommandBar* pBar)
{
	CSize szIcon = pBar->GetIconSize();
	CSize szImage(max(szIcon.cx + 4, m_nTextHeight), max(szIcon.cy + 4, m_nTextHeight));
	return szImage;
}

CSize CXTPDefaultTheme::DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw)
{
	CXTPCommandBar* pParent = pButton->GetParent();

	CRect rcButton = pButton->GetRect();
	XTPControlType controlType = pButton->GetType();
	XTPButtonStyle buttonStyle = pButton->GetStyle();

	BOOL bDrawImage = (buttonStyle == xtpButtonAutomatic && controlType != xtpControlPopup) ||
		(buttonStyle != xtpButtonCaption);

	BOOL bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked(),
		bPopuped = pButton->GetPopuped();
	if (controlType == xtpControlSplitButtonPopup && bEnabled == TRUE_SPLITDROPDOWN)
		bEnabled = FALSE;

	BOOL bLayoutRTL = pParent->IsLayoutRTL();
	BOOL bDoubleGripper = pParent->IsPopupBar() ? ((CXTPPopupBar*)pParent)->IsDoubleGripper() : FALSE;

	CString strCaption = pButton->GetCaption();
	if (bLayoutRTL) UpdateRTLCaption(strCaption);

	CSize szIcon = GetIconSize(pButton);
	CSize szImage(max(szIcon.cx + 4, m_nTextHeight), max(szIcon.cy + 4, m_nTextHeight));

	if (pButton->GetType() == xtpControlLabel && !bDrawImage)
		szImage.cy = m_nTextHeight;

	if (bDraw)
	{
		CRect rcSelection(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom);
		CRect rcImageRect(rcButton.left, rcButton.top, rcButton.left + szImage.cx, rcButton.bottom);
		CRect rcCheck (CPoint(rcImageRect.left + rcImageRect.Width()/2 - 10, rcImageRect.top + rcImageRect.Height()/2 - 10), CSize(20, 20));

		if (pButton->GetExpanded())
		{
			pDC->FillSolidRect(rcButton, GetXtremeColor(COLOR_3DLIGHT));
		}


		COLORREF clrText = GetControlTextColor(pButton);
		pDC->SetTextColor(clrText);

		if (m_bSelectImageInPopupBar && bSelected)
			DrawRectangle(pDC, rcSelection, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

		if (bChecked && !m_bSelectImageInPopupBar)
		{
			DrawRectangle(pDC, rcImageRect, bSelected, FALSE, bEnabled, bChecked, FALSE, xtpBarTypeNormal, xtpBarPopup);
			rcSelection.left = rcImageRect.right + 1;
		}

		if (bDoubleGripper)
			rcImageRect.OffsetRect(szImage.cx + 1, 0);

		CXTPImageManagerIcon* pImage = pButton->GetIconId() != 0 && bDrawImage ? pButton->GetImage(szIcon.cx) : NULL;
		if (pImage)
		{
			szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();
			if (!m_bSelectImageInPopupBar) DrawRectangle(pDC, rcImageRect, bSelected, FALSE, bEnabled, bDoubleGripper ? FALSE : bChecked, FALSE, xtpBarTypeNormal, xtpBarPopup);
			CPoint ptImage = CPoint(rcImageRect.left + rcImageRect.Width()/2 - szIcon.cx/2, rcImageRect.top + rcImageRect.Height()/2 - szIcon.cy/2);
			DrawImage(pDC, ptImage, szIcon, pImage, bSelected, FALSE, bEnabled, bChecked, FALSE, FALSE);
			rcSelection.left = rcImageRect.right + 1;
		}

		if (bChecked && (bDoubleGripper || !pImage))
		{
			if (controlType != xtpControlRadioButton)
				DrawCheckMark(pDC, rcCheck, m_bSelectImageInPopupBar ? clrText: GetXtremeColor(!bEnabled ? COLOR_GRAYTEXT : m_nPopupBarText));
			else
				DrawRadioMark(pDC, rcCheck, m_bSelectImageInPopupBar ? clrText: GetXtremeColor(!bEnabled ? COLOR_GRAYTEXT : m_nPopupBarText));
		}


		pDC->SetBkMode (TRANSPARENT);

		if (IsPopupControlType(controlType))
		{
			CPoint pt = CPoint(rcButton.right - 11, rcButton.CenterPoint().y);
			if (pButton->GetType() == xtpControlSplitButtonPopup)
			{
				if (!bPopuped || !bSelected || !bEnabled)
				{
					if (bSelected && !m_bSelectImageInPopupBar) DrawRectangle(pDC, rcSelection, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);;
					VerticalLine(pDC, rcButton.right - 20, rcButton.top + 1, rcButton.bottom - 1, !bEnabled ? clrText : GetXtremeColor(bSelected ? COLOR_3DSHADOW : m_nPopupBarText));
				}
				else
				{
					rcSelection.right -= 20;
					Draw3dRect(pDC, CRect(rcButton.right - 20, rcButton.top, rcButton.right, rcButton.bottom), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
					if (bSelected && !m_bSelectImageInPopupBar) DrawRectangle(pDC, rcSelection, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);
				}

				Triangle(pDC, CPoint(pt.x, pt.y - 3), CPoint(pt.x + 3, pt.y), CPoint(pt.x, pt.y + 3), !bPopuped || !bSelected ? clrText : GetXtremeColor(m_nPopupBarText));
			}
			else
			{
				if (bSelected && !m_bSelectImageInPopupBar) DrawRectangle(pDC, rcSelection, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);
				Triangle(pDC, CPoint(pt.x, pt.y - 3), CPoint(pt.x + 3, pt.y), CPoint(pt.x, pt.y + 3), clrText);
			}

		}
		else if (bSelected && !m_bSelectImageInPopupBar)
			DrawRectangle(pDC, rcSelection, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);
	}

	return CXTPPaintManager::DrawControlPopupParent(pDC, pButton, bDraw);

}

COLORREF CXTPDefaultTheme::GetRectangleTextColor(BOOL bSelected, BOOL /*bPressed*/, BOOL bEnabled, BOOL /*bChecked*/, BOOL /*bPopuped*/, XTPBarType barType, XTPBarPosition barPosition)
{
	if (barType == xtpBarTypePopup)
	{
		return GetXtremeColor(!bEnabled ? COLOR_GRAYTEXT: bSelected ? COLOR_HIGHLIGHTTEXT : m_nPopupBarText);
	}

	int nColorFace = barPosition == xtpBarPopup ? m_nPopupBarText : COLOR_BTNTEXT;
	return GetXtremeColor(!bEnabled ? COLOR_GRAYTEXT : nColorFace);
}

void CXTPDefaultTheme::DrawShadedRect(CDC* pDC, CRect& rect)
{
	WORD bits[] =
	{
		0x0055, 0x00AA, 0x0055, 0x00AA,
		0x0055, 0x00AA, 0x0055, 0x00AA
	};

	CBitmap bitmap;
	bitmap.CreateBitmap(8, 8, 1, 1, &bits);

	CBrush brush;
	brush.CreatePatternBrush(&bitmap);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	COLORREF crBack = pDC->SetBkColor(GetXtremeColor(COLOR_3DFACE));
	COLORREF crText = pDC->SetTextColor(GetXtremeColor(COLOR_3DHIGHLIGHT));

	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	pDC->SelectObject(pOldBrush);
	pDC->SetBkColor(crBack);
	pDC->SetTextColor(crText);

	brush.DeleteObject();
}


void CXTPDefaultTheme::DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton)
{
	BOOL bPressed = pButton->GetPressed(), bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked(),
		bPopuped = pButton->GetPopuped();

	if (pButton->GetStyle() == xtpButtonIconAndCaptionBelow)
	{
		int nSplitDropDownHeight = GetSplitDropDownHeight();

		BOOL bVert = pButton->GetParent()->GetPosition() == xtpBarLeft ||
			pButton->GetParent()->GetPosition() == xtpBarRight;

		if (bVert)
			DrawRectangle(pDC, CRect(rcButton.left + nSplitDropDownHeight, rcButton.top, rcButton.right, rcButton.bottom),
				bSelected || bPopuped, bPressed, bEnabled, bChecked, FALSE, xtpBarTypeNormal, pButton->GetParent()->GetPosition());
		else
			DrawRectangle(pDC, CRect(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom - nSplitDropDownHeight),
				bSelected || bPopuped, bPressed, bEnabled, bChecked, FALSE, xtpBarTypeNormal, pButton->GetParent()->GetPosition());


		if ((bSelected || bPressed || bPopuped) && bEnabled)
		{
			if (bVert)
				DrawRectangle(pDC, CRect(rcButton.left, rcButton.top, rcButton.left + nSplitDropDownHeight, rcButton.bottom),
					TRUE, FALSE, bEnabled, FALSE, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());
			else
				DrawRectangle(pDC, CRect(rcButton.left, rcButton.bottom - nSplitDropDownHeight, rcButton.right, rcButton.bottom),
					TRUE, FALSE, bEnabled, FALSE, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());
		}

		return;
	}

	DrawRectangle(pDC, CRect(rcButton.left, rcButton.top, rcButton.right - 12, rcButton.bottom),
		bSelected || bPopuped, bPressed, bEnabled, bChecked, FALSE, xtpBarTypeNormal, pButton->GetParent()->GetPosition());

	CPoint pt = CPoint(rcButton.right - 7, rcButton.CenterPoint().y);

	if ((bSelected || bPressed || bPopuped) && bEnabled)
		DrawRectangle(pDC, CRect(rcButton.right - 12, rcButton.top, rcButton.right, rcButton.bottom),
		TRUE, FALSE, bEnabled, FALSE, bPopuped, xtpBarTypeNormal, pButton->GetParent()->GetPosition());

	if (bEnabled == TRUE_SPLITCOMMAND) bEnabled = FALSE;
	DrawDropDownGlyph(pDC, pButton, pt, bSelected, bPopuped, bEnabled, FALSE);
}


void CXTPDefaultTheme::FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	CRect rc;
	pBar->GetClientRect(&rc);
	pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));

	if (pBar->GetPosition() == xtpBarFloating || pBar->GetPosition() == xtpBarPopup)
	{
		Draw3dRect(pDC, rc, COLOR_3DFACE, COLOR_3DDKSHADOW);
		rc.DeflateRect(1, 1);
		Draw3dRect(pDC, rc, COLOR_BTNHILIGHT, COLOR_3DSHADOW);
	}
	else if (!IsFlatToolBar(pBar))
	{
		Draw3dRect(pDC, rc, COLOR_BTNHILIGHT, COLOR_3DSHADOW);
	}
}


CSize CXTPDefaultTheme::DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* /*pBar*/, BOOL bDraw)
{
	CSize sz(6, max(22, m_nTextHeight + 4));

	if (pDC && bDraw)
	{
		Draw3dRect(pDC, CRect(3, 3, 6, sz.cy), COLOR_BTNHILIGHT, COLOR_3DSHADOW);
	}

	return sz;
}


CSize CXTPDefaultTheme::DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw)
{
	if (pBar->IsDialogBar())
		return DrawDialogBarGripper(pDC, (CXTPDialogBar*)pBar, bDraw);

	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarFloating)
	{
		return CXTPPaintManager::DrawCommandBarGripper(pDC, pBar, bDraw);

	}
	else if (pBar->GetPosition() == xtpBarRight || pBar->GetPosition() == xtpBarLeft)
	{
		if (pDC && bDraw)
			Draw3dRect(pDC, CRect(3, 3, rc.right - 3, 6), COLOR_BTNHILIGHT, COLOR_3DSHADOW);
		return CSize(0, 8);
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		if (pDC && bDraw)
			Draw3dRect(pDC, CRect(3, 3, 6, rc.bottom - 3), COLOR_BTNHILIGHT, COLOR_3DSHADOW);
		return CSize(6, 0);
	}
	return 0;
}

CSize CXTPDefaultTheme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	if (!bDraw)
	{
		if (pBar->GetType() == xtpBarTypePopup)
			return CSize(8, 8);

		return CSize(6, 6);
	}


	CRect rc;
	pBar->GetClientRect(&rc);
	CRect rcControl = pControl->GetRect();
	CRect rcRow = pControl->GetRowRect();

	if (pBar->GetType() == xtpBarTypePopup)
	{
		if (!pControl->GetWrap())
			Draw3dRect(pDC, CRect(rcControl.left + 11, rcControl.top - 2 - 2 -1, rcControl.right - 11, rcControl.top - 0 - 2 -  1), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
		else
			Draw3dRect(pDC, CRect(rcControl.left - 5 , rcRow.top + 3, rcControl.left - 3, rcRow.bottom - 3), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	}
	else if (pBar->GetPosition() != xtpBarPopup && IsVerticalPosition(pBar->GetPosition()))
	{
		if (!pControl->GetWrap())
			Draw3dRect(pDC, CRect(rcRow.left, rcControl.top - 4, rcRow.right, rcControl.top - 2), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
		else
			Draw3dRect(pDC, CRect(rcRow.right + 2, rcRow.top + 4, rcRow.right + 4, rcRow.bottom), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	}
	else
	{
		if (!pControl->GetWrap())
			Draw3dRect(pDC, CRect(rcControl.left - 4, rcRow.top, rcControl.left - 2, rcRow.bottom), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
		else
			Draw3dRect(pDC, CRect(rcRow.left + 4, rcRow.top - 4, rcRow.right, rcRow.top - 2), COLOR_3DSHADOW, COLOR_BTNHILIGHT);
	}
	return 0;
}

void CXTPDefaultTheme::AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical)
{
	CXTPCommandBar* pParent = pControl->GetParent();
	ASSERT(pParent);

	if (pControl->GetType() == xtpControlComboBox) return;

	if (pParent->GetType() != xtpBarTypePopup)
	{
		if (bVertical)
			rc.InflateRect(0, 1);
		else
			rc.InflateRect(1, 0);
	}
	else
	{
		rc.DeflateRect(1, 0);
	}
}

CRect CXTPDefaultTheme::GetCommandBarBorders(CXTPCommandBar* pBar)
{
	if (pBar->GetType() == xtpBarTypePopup)
		return CRect(3, 3, 3, 3);
	else return CXTPPaintManager::GetCommandBarBorders(pBar);
}

CSize CXTPDefaultTheme::DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rcButton, BOOL bSelected, BOOL bDraw, CXTPCommandBars* pCommandBars)
{
	CSize szIcon = pCommandBars && pCommandBars->GetCommandBarsOptions()->szPopupIcons != CSize(0) ?
		pCommandBars->GetCommandBarsOptions()->szPopupIcons : GetAutoIconSize(FALSE);
	CSize szImage(max(szIcon.cx + 4, m_nTextHeight), max(szIcon.cy + 4, m_nTextHeight));

	if (bDraw)
	{
		if (pButton == NULL)
		{
			pDC->FillSolidRect(rcButton, GetXtremeColor(COLOR_3DFACE));
			return 0;
		}
		pDC->FillSolidRect(rcButton, GetXtremeColor(COLOR_3DFACE));

		CRect rcSelection(rcButton.left + 1, rcButton.top, rcButton.right - 1, rcButton.bottom);
		XTPControlType controlType = pButton->GetType();
		XTPButtonStyle buttonStyle = pButton->GetStyle();

		BOOL bDrawImage = (buttonStyle == xtpButtonAutomatic && controlType != xtpControlPopup) ||
			(buttonStyle == xtpButtonIcon) || (buttonStyle == xtpButtonIconAndCaption);

		CXTPImageManagerIcon* pImage = pButton->GetIconId() != 0 && bDrawImage ? pButton->GetImage(szIcon.cx) : NULL;
		if (pImage)
		{
			szIcon.cy = pImage->GetHeight() * szIcon.cx / pImage->GetWidth();
			CRect rcImageRect(rcButton.left, rcButton.top, rcButton.left + szImage.cx, rcButton.bottom);
			DrawRectangle(pDC, rcImageRect, bSelected, FALSE, TRUE, FALSE, FALSE, xtpBarTypeNormal, xtpBarPopup);
			CPoint ptImage = CPoint(rcImageRect.left + rcImageRect.Width()/2 - szIcon.cx/2, rcImageRect.top +  rcImageRect.Height()/2 - szIcon.cy/2);
			DrawImage(pDC, ptImage, szIcon, pImage, bSelected, FALSE, TRUE, FALSE, FALSE, FALSE);
			rcSelection.left = rcImageRect.right + 1;
		}
		if (bSelected)
			Rectangle(pDC, rcSelection, COLOR_HIGHLIGHT, COLOR_HIGHLIGHT);


		COLORREF clrText = GetXtremeColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT);

		pDC->SetTextColor(clrText);
		pDC->SetBkMode (TRANSPARENT);

		CString strText = pButton->GetCaption();
		StripMnemonics(strText);

		if (pCommandBars && pCommandBars->IsLayoutRTL())
			UpdateRTLCaption(strText);

		CRect rcText(rcButton.left + szImage.cx + 4, rcButton.top, rcButton.right, rcButton.bottom);
		CXTPFontDC fnt(pDC, &m_fontRegular);
		pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

		if (controlType == xtpControlPopup || controlType == xtpControlButtonPopup || controlType == xtpControlSplitButtonPopup)
		{
			CPoint pt(rcButton.right - 7, rcButton.CenterPoint().y);
			Triangle(pDC, pt, CPoint(pt.x - 5, pt.y - 5), CPoint(pt.x - 5, pt.y + 5), clrText);
		}
		if (controlType == xtpControlSplitButtonPopup)
		{
			Line(pDC, rcButton.right - 20, rcButton.top + 1, rcButton.right - 20, rcButton.bottom - 1, bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT);
		}

		if (controlType == xtpControlComboBox || controlType == xtpControlEdit)
		{
			rcText.SetRectEmpty();
			pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_CALCRECT);
			int nThumb = pDC->GetTextExtent(_T(" "), 1).cy + 2;

			CRect rcCombo(rcButton.left + szImage.cx + 4 + rcText.Width() + 5, rcButton.top, rcButton.right, rcButton.bottom);
			CRect rcThumb(rcButton.right - nThumb, rcButton.top, rcButton.right, rcButton.bottom);

			Rectangle(pDC, rcCombo, bSelected ? COLOR_HIGHLIGHT : COLOR_3DFACE, COLOR_WINDOW);

			if (controlType == xtpControlComboBox)
			{
				if (!bSelected) pDC->FillSolidRect(rcThumb.left + 2, rcThumb.top + 2, rcThumb.Width() - 4, rcThumb.Height() - 4, GetXtremeColor(COLOR_3DFACE));
				else Rectangle(pDC, rcThumb, COLOR_HIGHLIGHT, COLOR_3DFACE);

				rcThumb.DeflateRect(1, 1, 1, 1);
				DrawComboExpandMark(pDC, rcThumb, GetXtremeColor(COLOR_BTNTEXT));
			}
		}
	}
	return szImage;
}

CSize CXTPDefaultTheme::DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc(rcGripper.left, rcGripper.top, rcGripper.right, rcGripper.bottom - 1);
		pDC->FillSolidRect(rc, GetXtremeColor(bSelected ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
	}

	return CSize(0, 8);
}
