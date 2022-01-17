// XTPRibbonTheme.cpp: implementation of the CXTPRibbonTheme class.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPOffice2007Image.h"

#include "TabManager/XTPTabManager.h"
#include "TabManager/XTPTabPaintManager.h"

#include "CommandBars/XTPOffice2007Theme.h"
#include "CommandBars/XTPOffice2007FrameHook.h"

#include "CommandBars/XTPControlPopup.h"
#include "CommandBars/XTPControlComboBox.h"
#include "CommandBars/XTPControlEdit.h"
#include "CommandBars/XTPCommandBar.h"
#include "CommandBars/XTPPopupBar.h"
#include "CommandBars/XTPStatusBar.h"
#include "CommandBars/XTPCommandBars.h"

#include "XTPRibbonGroup.h"
#include "XTPRibbonBar.h"
#include "XTPRibbonTheme.h"
#include "XTPRibbonTab.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPRibbonTheme::CXTPRibbonTheme()
{
	m_bHotTrackingGroups = TRUE;
	m_bFrameStatusBar = TRUE;
	m_bClearTypeTextQuality = TRUE;

	m_nSplitButtonDropDownWidth = 12;
	m_nSplitButtonPopupWidth = 26;

	m_pTabPaintManager = CreateTabPaintManager();
	m_pTabPaintManager->RefreshMetrics();

	m_strOfficeFont = _T("Segoe UI");
	m_bUseOfficeFont = TRUE;

	m_bOffsetPopupLabelText = FALSE;

	m_nFontHeight = 0;

}

CXTPRibbonTheme::~CXTPRibbonTheme()
{
	CMDTARGET_RELEASE(m_pTabPaintManager);
}

AFX_INLINE LPCTSTR FormatName(LPCTSTR lpszName)
{
	return lpszName;
}

AFX_INLINE CString FormatName(LPCTSTR lpszFormat, LPCTSTR lpszParameter)
{
	CString strResult;
	strResult.Format(lpszFormat, lpszParameter);
	return strResult;
}

AFX_INLINE BOOL IsCompositeRect(CXTPCommandBar* pRibbonBar, const CRect& rc)
{
	if (!pRibbonBar || !pRibbonBar->IsRibbonBar() || !((CXTPRibbonBar*)pRibbonBar)->IsDwmEnabled())
		return FALSE;

	return CRect().IntersectRect(((CXTPRibbonBar*)pRibbonBar)->GetCaptionRect(), rc);
}

#ifdef _XTP_INCLUDE_RIBBON
BOOL CXTPControl::HasDwmCompositedRect() const
{
	return IsCompositeRect(GetParent(), GetRect());
}
#endif

AFX_INLINE DWORD GetDrawImageFlags(CXTPControl* pControl, const CRect& rc)
{
	return IsCompositeRect(pControl->GetParent(), rc) ? XTP_DI_COMPOSITE : 0;
}

AFX_INLINE DWORD GetDrawImageFlags(CXTPControl* pControl)
{
	return IsCompositeRect(pControl->GetParent(), pControl->GetRect()) ? XTP_DI_COMPOSITE : 0;
}


CString GetColorName(XTPRibbonTabContextColor color)
{
	switch (color)
	{
		case xtpRibbonTabContextColorBlue: return _T("BLUE");
		case xtpRibbonTabContextColorYellow: return _T("YELLOW");
		case xtpRibbonTabContextColorGreen: return _T("GREEN");
		case xtpRibbonTabContextColorRed: return _T("RED");
		case xtpRibbonTabContextColorPurple: return _T("PURPLE");
		case xtpRibbonTabContextColorCyan: return _T("CYAN");
		case xtpRibbonTabContextColorOrange: return _T("ORANGE");
	}

	return _T("");
}



//////////////////////////////////////////////////////////////////////////
// CXTPRibbonTheme

class CXTPRibbonTheme::CRibbonAppearanceSet : public CXTPTabPaintManager::CAppearanceSetPropertyPageFlat
{
public:
	CRibbonAppearanceSet(CXTPRibbonTheme* pRibbonTheme)
		: m_pRibbonTheme(pRibbonTheme)
	{
		m_rcHeaderMargin.SetRect(0, 1, 0, 0);
	}
protected:
	void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);

	int GetButtonHeight(const CXTPTabManager* /*pTabManager*/)
	{
		return m_pRibbonTheme->m_nTabsHeight + 1;
	}

	CXTPRibbonTheme* m_pRibbonTheme;

};

class CXTPRibbonTheme::CRibbonColorSet : public CXTPTabPaintManager::CColorSetOffice2003
{
protected:
	void RefreshMetrics()
	{
		CColorSetOffice2003::RefreshMetrics();
	}
};

void CXTPRibbonTheme::DrawRibbonTab(CDC* pDC, CXTPRibbonTab* pItem)
{
	XTPTabPosition tabPosition = pItem->GetTabManager()->GetPosition();

	CRect rcEntry(pItem->GetRect());
	CXTPTabPaintManager::CAppearanceSet::InflateRectEx(rcEntry, CRect(0, 1, -1,  -1), tabPosition);

	XTPRibbonTabContextColor tabColor = pItem->GetContextColor();

	if (tabColor != xtpRibbonTabContextColorNone)
	{
		if (pItem->IsSelected() || pItem->IsHighlighted())
		{
			CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("CONTEXTTAB%s"), GetColorName(tabColor)));

			int nState = 0;
			if (pItem->IsSelected())
			{
				nState = pItem->IsFocused() || pItem->IsHighlighted() ? 4 : 3;
			}
			else if (pItem->IsHighlighted())
			{
				nState = 1;
			}

			if (pImage) pImage->DrawImage(pDC, rcEntry, pImage->GetSource(nState, 5), CRect(5, 5, 5, 5));
		}
	}
	else
	{
		if (pItem->IsSelected() || pItem->IsHighlighted())
		{
			CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONTAB")));

			int nState = 0;
			if (pItem->IsSelected() && pItem->IsFocused())
			{
				nState = 5;
			}
			else if (pItem->IsSelected() && pItem->IsHighlighted())
			{
				nState = 4;
			}
			else if (pItem->IsSelected())
			{
				nState = 3;
			}
			else if (pItem->IsPressed())
			{
				nState = 2;
			}
			else if (pItem->IsHighlighted())
			{
				nState = 1;
			}

			if (pImage) pImage->DrawImage(pDC, rcEntry, pImage->GetSource(nState, 6), CRect(5, 5, 5, 5), 0xFF00FF);
		}
	}

	if (pItem->GetButtonLength() < pItem->GetContentLength())
	{
		CRect rcSeparator(rcEntry.right, rcEntry.top, rcEntry.right + 1, rcEntry.bottom);
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONTABSEPARATOR")));
		if (pImage) pImage->DrawImage(pDC, rcSeparator, pImage->GetSource(), CRect(0, 0, 0, 0));
	}

	if (tabColor != xtpRibbonTabContextColorNone && pItem->GetContextHeader() &&
		(pItem->GetContextHeader()->m_pFirstTab == pItem))
	{
		CRect rcSeparator(rcEntry.left, rcEntry.top, rcEntry.left + 1, rcEntry.bottom - 1);
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("CONTEXTTABSEPARATOR")));
		if (pImage) pImage->DrawImage(pDC, rcSeparator, pImage->GetSource(), CRect(0, 0, 0, 0));
	}
	if (tabColor != xtpRibbonTabContextColorNone && pItem->GetContextHeader() &&
		(pItem->GetContextHeader()->m_pLastTab == pItem))
	{
		CRect rcSeparator(rcEntry.right, rcEntry.top, rcEntry.right + 1, rcEntry.bottom - 1);
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("CONTEXTTABSEPARATOR")));
		if (pImage) pImage->DrawImage(pDC, rcSeparator, pImage->GetSource(), CRect(0, 0, 0, 0));
	}
}

void CXTPRibbonTheme::CRibbonAppearanceSet::DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem)
{
	m_pRibbonTheme->DrawRibbonTab(pDC, (CXTPRibbonTab*)pItem);

	m_pPaintManager->DrawSingleButtonIconAndText(pDC, pItem, pItem->GetRect() , TRUE);
}


void CXTPRibbonTheme::UpdateFonts()
{
	CNonClientMetrics ncm;
	CLogFont lf;

	int nBaseHeight = ncm.lfMenuFont.lfHeight < 0 ? -ncm.lfMenuFont.lfHeight : ncm.lfMenuFont.lfHeight;
	if (m_nFontHeight > 11)
		nBaseHeight = m_nFontHeight;

	if (nBaseHeight <= 12) nBaseHeight = 11;

	int nLogPixel = 96; //CWindowDC(NULL).GetDeviceCaps(LOGPIXELSY);

	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfCharSet = ncm.lfMenuFont.lfCharSet;

	if (m_bClearTypeTextQuality && XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		lf.lfQuality = 5;
		ncm.lfSmCaptionFont.lfQuality = 5;
		ncm.lfStatusFont.lfQuality = 6;
	}
	if (m_bUseOfficeFont && FontExists(m_strOfficeFont))
	{
		STRCPY_S(lf.lfFaceName, LF_FACESIZE, m_strOfficeFont);
		STRCPY_S(ncm.lfStatusFont.lfFaceName, LF_FACESIZE, m_strOfficeFont);
	}
	else
	{
		STRCPY_S(lf.lfFaceName, LF_FACESIZE, ncm.lfMenuFont.lfFaceName);
	}

	lf.lfHeight = nBaseHeight * 120 / nLogPixel;

	if (m_bUseStandardFont)
	{
		LOGFONT lfFont = lf;
		SetCommandBarsFontIndirect(&lfFont, TRUE);
	}

	m_nTextHeight = max(22, nBaseHeight * 144 / nLogPixel + 6);

	m_nEditHeight = max(22, nBaseHeight * 195 / nLogPixel);

	m_nTabsHeight = m_nEditHeight > 22 ? m_nEditHeight + 2 : 23;

	m_nGroupCaptionHeight = max(17, nBaseHeight * 144 / nLogPixel);

	m_nSplitButtonDropDownWidth = max(12, m_nTextHeight / 2);

	m_fontGroupCaption.SetStandardFont(&lf);

	LOGFONT lfIcon = lf;
	STRCPY_S(lfIcon.lfFaceName, LF_FACESIZE, ncm.lfMenuFont.lfFaceName);
	VERIFY(m_fontIcon.SetStandardFont(&lfIcon));

	lfIcon.lfWeight = FW_BOLD;
	VERIFY(m_fontIconBold.SetStandardFont(&lfIcon));

	m_pTabPaintManager->RefreshMetrics();
	m_pTabPaintManager->SetFontIndirect(&lf, TRUE);

	m_fontSmCaption.SetStandardFont(&ncm.lfSmCaptionFont);

	m_fontToolTip.SetStandardFont(&ncm.lfStatusFont);
}

void CXTPRibbonTheme::RefreshMetrics()
{
	CXTPOffice2007Theme::RefreshMetrics();



	//////////////////////////////////////////////////////////////////////////
	// Colors

	m_clrRibbonFace = GetImages()->GetImageColor(_T("Ribbon"), _T("RibbonFace"));

	m_pTabPaintManager->GetColorSet()->m_clrHighlightText.SetStandardValue(GetImages()->GetImageColor(_T("Ribbon"), _T("TabNormalText")));
	m_pTabPaintManager->GetColorSet()->m_clrNormalText.SetStandardValue(GetImages()->GetImageColor(_T("Ribbon"), _T("TabNormalText")));
	m_pTabPaintManager->GetColorSet()->m_clrSelectedText.SetStandardValue(GetImages()->GetImageColor(_T("Ribbon"), _T("TabSelectedText")));

	m_clrRibbonGroupCaptionText = GetImages()->GetImageColor(_T("Ribbon"), _T("GroupCaptionText"));

	m_arrColor[XPCOLOR_TOOLBAR_TEXT].SetStandardValue(GetImages()->GetImageColor(_T("Ribbon"), _T("GroupClientText")));
	m_clrRibbonText = GetImages()->GetImageColor(_T("Ribbon"), _T("RibbonText"));

	m_clrControlEditNormal = GetImages()->GetImageColor(_T("Ribbon"), _T("ControlEditNormal"));

	m_clrMinimizedFrameEdgeHighLight = GetImages()->GetImageColor(_T("Ribbon"), _T("MinimizedFrameEdgeHighLight"));
	m_clrMinimizedFrameEdgeShadow = GetImages()->GetImageColor(_T("Ribbon"), _T("MinimizedFrameEdgeShadow"));

	m_arrColor[XPCOLOR_MENUBAR_TEXT].SetStandardValue(GetImages()->GetImageColor(_T("Ribbon"), _T("MenuPopupText")));


	m_clrShadowFactor = 0;

	m_arrColor[XPCOLOR_MENUBAR_GRAYTEXT].SetStandardValue(RGB(167, 167, 167));
	m_arrColor[XPCOLOR_HIGHLIGHT_TEXT].SetStandardValue(GetImages()->GetImageColor(_T("Ribbon"), _T("ControlHighlightText")));

	m_arrColor[XPCOLOR_MENUBAR_BORDER].SetStandardValue(RGB(134, 134, 134));
	m_arrColor[XPCOLOR_MENUBAR_FACE].SetStandardValue(RGB(250, 250, 250));

	m_clrMenuExpand.SetStandardValue(RGB(233, 238, 238), RGB(233, 238, 238));

	CreateGradientCircle();
}


CXTPTabPaintManager* CXTPRibbonTheme::CreateTabPaintManager()
{
	CXTPTabPaintManager* pTabPaintManager = new CXTPTabPaintManager();
	pTabPaintManager->SetAppearanceSet(new CRibbonAppearanceSet(this));
	pTabPaintManager->SetLayout(xtpTabLayoutSizeToFit);

	pTabPaintManager->SetColorSet(new CRibbonColorSet);
	pTabPaintManager->m_bFillBackground = FALSE;
	pTabPaintManager->m_bHotTracking = TRUE;
	pTabPaintManager->m_clientFrame = xtpTabFrameSingleLine;
	pTabPaintManager->m_bDrawTextNoPrefix = FALSE;
	pTabPaintManager->m_rcButtonMargin.SetRect(0, 2, 0, 2);
	pTabPaintManager->m_rcButtonTextPadding.SetRect(4, 0, 4, 0);
	pTabPaintManager->m_nButtonExtraLength = 21;
	pTabPaintManager->m_bDrawTextEndEllipsis = FALSE;
	pTabPaintManager->m_bDrawFocusRect = FALSE;
	pTabPaintManager->EnableToolTips(xtpTabToolTipShrinkedOnly);
	pTabPaintManager->m_bClearTypeTextQuality = TRUE;
	pTabPaintManager->m_bClipHeader = FALSE;

	return pTabPaintManager;
}

void CXTPRibbonTheme::FillGroupPopupToolBar(CDC* pDC, CXTPRibbonGroup* pGroup, CXTPCommandBar* pCommandBar)
{
	pDC->SetBkMode(TRANSPARENT);

	CXTPClientRect rcRibbon(pCommandBar);

	pDC->FillSolidRect(rcRibbon, RGB(191, 219, 255));

	rcRibbon.InflateRect(3, 3);
	XTPRibbonTabContextColor tabColor = pGroup->GetParentTab()->GetContextColor();

	if (tabColor != xtpRibbonTabContextColorNone)
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("CONTEXTTAB%sCLIENT"), GetColorName(tabColor)));
		if (pImage) pImage->DrawImage(pDC, rcRibbon, pImage->GetSource(0, 1), CRect(8, 8, 8, 8));
	}
	else
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONGROUPS")));
		if (pImage) pImage->DrawImage(pDC, rcRibbon, pImage->GetSource(0, 1), CRect(8, 8, 8, 8));
	}
}

void CXTPRibbonTheme::DrawGroup(CDC* pDC, CXTPRibbonGroup* pGroup)
{
	CRect rcGroup(pGroup->GetRect());

	CRect rcGroupClient(rcGroup.left, rcGroup.top, rcGroup.right, rcGroup.bottom - m_nGroupCaptionHeight);
	CRect rcGroupCaption(rcGroup.left, rcGroupClient.bottom, rcGroup.right, rcGroup.bottom);


	if (!pGroup->IsReduced())
	{
		CXTPOffice2007Image* pImage, *pImageCaption;

		if (pGroup->GetParentTab()->GetContextColor() == xtpRibbonTabContextColorNone)
		{
			pImage = LoadImage(FormatName(_T("RIBBONGROUPCLIENT")));
			pImageCaption = LoadImage(FormatName(_T("RIBBONGROUPCAPTION")));
		}
		else
		{
			pImage = LoadImage(FormatName(_T("CONTEXTTABGROUPCLIENT")));
			pImageCaption = LoadImage(FormatName(_T("CONTEXTTABGROUPCAPTION")));
		}

		int nState = pGroup->IsHighlighted() ? 1 : 0;

		if (pImage) pImage->DrawImage(pDC, rcGroupClient, pImage->GetSource(nState, 2), CRect(5, 5, 5, 5));
		if (pImageCaption) pImageCaption->DrawImage(pDC, rcGroupCaption,  pImageCaption->GetSource(nState, 2), CRect(5, 5, 5, 5));


		CString strCaption = pGroup->GetCaption();
		XTPDrawHelpers()->StripMnemonics(strCaption);

		CXTPFontDC font(pDC, &m_fontGroupCaption);

		CRect rcGroupCaptionText(rcGroupCaption);
		rcGroupCaptionText.OffsetRect(0, -1);
		if (pGroup->IsOptionButtonVisible())
			rcGroupCaptionText.right -= m_nGroupCaptionHeight + 1;

		pDC->SetTextColor(m_clrRibbonGroupCaptionText);
		pDC->DrawText(strCaption, rcGroupCaptionText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS | DT_CENTER);
	}
}

void CXTPRibbonTheme::FillTabPopupToolBar(CDC* pDC, CXTPPopupToolBar* pPopupToolBar)
{
	CXTPClientRect rcRibbonClient(pPopupToolBar);
	pDC->FillSolidRect(rcRibbonClient, m_clrRibbonFace);
}

void CXTPRibbonTheme::FillRibbonBar(CDC* pDC, CXTPRibbonBar* pRibbonBar)
{

	CXTPClientRect rcRibbonClient(pRibbonBar);
	if (pRibbonBar->IsFrameThemeEnabled())
		rcRibbonClient.top += m_nRibbonCaptionHeight;

	CRect rcRibbonTabs(rcRibbonClient.left, rcRibbonClient.top, rcRibbonClient.right, rcRibbonClient.top + m_nTabsHeight);
	CRect rcRibbonGroups(rcRibbonClient.left, rcRibbonTabs.bottom, rcRibbonClient.right, rcRibbonClient.bottom);

	pDC->FillSolidRect(rcRibbonTabs, m_clrRibbonFace);
	pDC->FillSolidRect(rcRibbonGroups, m_clrRibbonFace);

	if (pRibbonBar->IsQuickAccessBelowRibbon())
	{
		CRect rcQuickAccess = pRibbonBar->GetQuickAccessRect();
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONQUICKACCESSFRAME")));
		if (pImage) pImage->DrawImage(pDC, rcQuickAccess, pImage->GetSource(0, 1), CRect(3, 3, 3, 3));
	}

	if (!pRibbonBar->IsGroupsVisible() && !pRibbonBar->IsQuickAccessBelowRibbon())
	{
		HorizontalLine(pDC, rcRibbonClient.left, rcRibbonClient.bottom - 2, rcRibbonClient.right, m_clrMinimizedFrameEdgeShadow);
		HorizontalLine(pDC, rcRibbonClient.left, rcRibbonClient.bottom - 1, rcRibbonClient.right, m_clrMinimizedFrameEdgeHighLight);
	}
}

void CXTPRibbonTheme::FillGroupRect(CDC* pDC, CXTPRibbonTab* pActiveTab, CRect rcGroups)
{
	XTPRibbonTabContextColor tabColor = pActiveTab ? pActiveTab->GetContextColor() : xtpRibbonTabContextColorNone;

	if (tabColor != xtpRibbonTabContextColorNone)
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("CONTEXTTAB%sCLIENT"), GetColorName(tabColor)));
		if (pImage) pImage->DrawImage(pDC, rcGroups, pImage->GetSource(0, 1), CRect(8, 8, 8, 8));
	}
	else
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONGROUPS")));
		if (pImage) pImage->DrawImage(pDC, rcGroups, pImage->GetSource(0, 1), CRect(8, 8, 8, 8));
	}
}

CSize CXTPRibbonTheme::DrawControlGroupOption(CDC* pDC, CXTPControl* pControlGroupOption, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc = pControlGroupOption->GetRect();

		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONOPTIONBUTTON")));

		int nState = 0;
		if (pControlGroupOption->GetSelected()) nState = 1;
		if (pControlGroupOption->GetPressed()) nState = 2;

		if (pImage) pImage->DrawImage(pDC, rc, pImage->GetSource(nState, 4), CRect(3, 2, 2, 2), 0xFF00FF);
	}

	return CSize(15, 14);
}

CSize CXTPRibbonTheme::DrawControlGroupPopup(CDC* pDC, CXTPControlPopup* pControlGroupPopup, BOOL bDraw)
{
	CXTPRibbonGroup* pRibbonGroup = pControlGroupPopup->GetRibbonGroup();
	CXTPOffice2007Image* pImage = NULL;

	int nOffset = 0;
	if (pControlGroupPopup->GetSelected()) nOffset = 1;
	if (pControlGroupPopup->GetPopuped()) nOffset = 2;

	if (!pRibbonGroup)
	{
		if (bDraw)
		{
			pImage = LoadImage(FormatName(_T("RIBBONGROUPQUICKACCESS")));
			ASSERT(pImage);

			if (pImage) pImage->DrawImage(pDC, pControlGroupPopup->GetRect(),
				pImage->GetSource(nOffset, 3), CRect(2, 2, 2, 2), 0xFF00FF, GetDrawImageFlags(pControlGroupPopup));
		}
		return DrawControlToolBarParent(pDC, pControlGroupPopup, bDraw);
	}

	if (bDraw)
	{
		CRect rc = pControlGroupPopup->GetRect();

		XTPRibbonTabContextColor tabColor = pRibbonGroup ? pRibbonGroup->GetParentTab()->GetContextColor() : xtpRibbonTabContextColorNone;

		if (tabColor != xtpRibbonTabContextColorNone)
		{
			pImage = LoadImage(FormatName(_T("CONTEXTTAB%sGROUPBUTTON"), GetColorName(tabColor)));
		}
		else
		{
			pImage = LoadImage(FormatName(_T("RIBBONGROUPBUTTON")));
		}

		if (pImage) pImage->DrawImage(pDC, rc, pImage->GetSource(nOffset, 3), CRect(5, 5, 5, 5));
	}

	CSize szButton(max(42, m_nTextHeight * 2 - 3), m_nEditHeight * 3);

	CXTPFontDC font(pDC, GetCommandBarFont(pControlGroupPopup->GetParent(), FALSE));

	int nSplitDropDownHeight = GetSplitDropDownHeight();

	if (bDraw)
	{
		CXTPImageManagerIcon* pImageIcon = pControlGroupPopup->GetImage(0);
		BOOL bDrawImage = (pImageIcon != NULL);

		CRect rcButton = pControlGroupPopup->GetRect();
		if (bDrawImage)
		{
			CSize sz(pImageIcon->GetWidth(), pImageIcon->GetHeight());

			CPoint pt = CPoint(rcButton.CenterPoint().x - sz.cx / 2, rcButton.top + 4);

			if (sz.cx < 20)
			{
				CXTPOffice2007Image* pImageButton = LoadImage(FormatName(_T("RIBBONGROUPBUTTONICON")));
				CRect rc(CPoint(rcButton.CenterPoint().x - pImageButton->GetWidth() / 2, rcButton.top + 2),
					CSize(pImageButton->GetWidth(), pImageButton->GetHeight()));

				if (pImageButton) pImageButton->DrawImage(pDC, rc, pImageButton->GetSource(0, 1), CRect(0, 0, 0, 0));

				pt.y = rcButton.top + 9;
			}

			DrawImage(pDC, pt, sz, pImageIcon,
				pControlGroupPopup->GetSelected(), pControlGroupPopup->GetPressed(),
				pControlGroupPopup->GetEnabled(), FALSE, pControlGroupPopup->GetPopuped());
		}

		CRect rcText = rcButton;
		rcText.bottom -= m_nGroupCaptionHeight + 2;
		rcText.top = rcText.bottom - nSplitDropDownHeight;

		pDC->SetTextColor(GetControlTextColor(pControlGroupPopup));
		DrawControlText2(pDC, pControlGroupPopup, rcText, TRUE, FALSE, TRUE);
	}
	else
	{
		CSize szText = DrawControlText2(pDC, pControlGroupPopup, CXTPEmptyRect(), FALSE, FALSE, TRUE);

		szButton.cx = max(szButton.cx, szText.cx + 6);
	}
	return szButton;
}

//////////////////////////////////////////////////////////////////////////
// CommandBars Theme



BOOL CXTPRibbonTheme::IsImagesAvailable()
{
	return TRUE;
}

void CXTPRibbonTheme::DrawControlEntry(CDC* pDC, CXTPControl* pButton)
{
	if (pButton->GetRibbonGroup() && pButton->GetRibbonGroup()->IsControlsGrouping())
	{
		BOOL bLeft = pButton->GetBeginGroup() || pButton->GetWrap();

		int nIndexNext = pButton->GetControls()->GetNext(pButton->GetIndex());

		BOOL bRight = nIndexNext <= pButton->GetIndex() ||
			pButton->GetControls()->GetAt(nIndexNext)->GetBeginGroup() ||
			pButton->GetControls()->GetAt(nIndexNext)->GetWrap();

		CString strImage = FormatName(bLeft && bRight ? _T("TOOLBARBUTTONSSPECIALSINGLE") :
			bLeft ? _T("TOOLBARBUTTONSSPECIALLEFT") :
			bRight ? _T("TOOLBARBUTTONSSPECIALRIGHT") :
			_T("TOOLBARBUTTONSSPECIALCENTER"));

		CXTPOffice2007Image* pImage = LoadImage(strImage);
		ASSERT(pImage);
		if (!pImage)
			return;

		BOOL bPopuped = pButton->GetPopuped();
		BOOL bEnabled = pButton->GetEnabled();
		BOOL bChecked = pButton->GetChecked();
		BOOL bSelected = pButton->GetSelected();
		BOOL bPressed = pButton->GetPressed();
		CRect rc = pButton->GetRect();

		CRect rcSrc = CRect(0, 0, 22, 22);

		int nState = 0;

		if (!bEnabled)
		{
			if (IsKeyboardSelected(bSelected)) nState = 0 + 1;
		}
		else if (bPopuped) nState = 2 + 1;
		else if (bChecked && !bSelected && !bPressed) nState = 2 + 1;
		else if (bChecked && bSelected && !bPressed) nState = 3 + 1;
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) nState = 1 + 1;
		else if (bSelected || bPressed) nState = 0 + 1;

		rcSrc.OffsetRect(0, rcSrc.Height() * nState);
		pImage->DrawImage(pDC, rc, rcSrc, CRect(2, 2, 2, 2), 0xFF00FF);
		return;
	}

	CXTPOffice2007Theme::DrawControlEntry(pDC, pButton);
}

COLORREF CXTPRibbonTheme::GetControlTextColor(CXTPControl* pButton)
{
	if (pButton->GetParent()->GetType() == xtpBarTypeRibbon && pButton->GetRibbonGroup() == NULL)
	{
		if (!pButton->GetSelected() && pButton->GetEnabled() &&
			!pButton->GetPressed() && !pButton->GetChecked() && !pButton->GetPopuped())
		{
			return m_clrRibbonText;
		}
	}

	return CXTPOffice2007Theme::GetControlTextColor(pButton);
}

COLORREF CXTPRibbonTheme::GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition /*barPosition*/)
{
	if (barType == xtpBarTypeMenuBar && !bSelected && bEnabled && !bPressed && !bChecked && !bPopuped)
	{
		return m_clrMenuBarText;
	}

	if (barType == xtpBarTypePopup)
	{
		return GetXtremeColor(!bEnabled ? XPCOLOR_MENUBAR_GRAYTEXT : XPCOLOR_MENUBAR_TEXT);
	}

	return GetXtremeColor(!bEnabled ? XPCOLOR_TOOLBAR_GRAYTEXT : XPCOLOR_TOOLBAR_TEXT);
}

void CXTPRibbonTheme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled /*= TRUE*/, BOOL bChecked /*= FALSE*/, BOOL bPopuped /*= FALSE*/, XTPBarType barType, XTPBarPosition barPosition)
{
	if (!IsImagesAvailable())
	{
		CXTPOffice2003Theme::DrawRectangle(pDC, rc, bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
		return;
	}

	if (barType != xtpBarTypePopup)
	{
		BOOL bSmallSize = rc.Height() < 33;

		CXTPOffice2007Image* pImage = LoadImage(FormatName(bSmallSize ? _T("TOOLBARBUTTONS22") : _T("TOOLBARBUTTONS50")));
		ASSERT(pImage);
		if (!pImage)
			return;

		int nState = -1;

		if (!bEnabled)
		{
			if (IsKeyboardSelected(bSelected)) nState = 0;
		}
		else if (bPopuped) nState = 2;
		else if (bChecked && !bSelected && !bPressed) nState = 2;
		else if (bChecked && bSelected && !bPressed) nState = 3;
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) nState = 1;
		else if (bSelected || bPressed) nState = 0;

		if (nState != -1)
		{
			pImage->DrawImage(pDC, rc, pImage->GetSource(nState, 4), CRect(8, 8, 8, 8), COLORREF_NULL);
		}
	}
	else if (barPosition == xtpBarPopup && bChecked && barType == xtpBarTypePopup)
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("MENUCHECKEDITEM")));
		ASSERT(pImage);
		if (!pImage)
			return;

		CRect rcSrc(0, 0, pImage->GetWidth(), pImage->GetHeight() / 2);
		if (!bEnabled)
			rcSrc.OffsetRect(0, rcSrc.Height());

		pImage->DrawImage(pDC, rc, rcSrc, CRect(4, 4, 4, 4));
	}
	else if (barPosition == xtpBarPopup && bSelected && barType == xtpBarTypePopup)
	{
		if (!bEnabled && !IsKeyboardSelected(bSelected))
			return;

		BOOL bSmallSize = rc.Height() < 33;
		CXTPOffice2007Image* pImage = LoadImage(FormatName(
			!bEnabled ? bSmallSize ? _T("MENUSELECTEDDISABLEDITEM22") : _T("MENUSELECTEDDISABLEDITEM54") :
				bSmallSize ? _T("MENUSELECTEDITEM22") : _T("MENUSELECTEDITEM54")));
		ASSERT(pImage);
		if (!pImage)
			return;

		CRect rcSrc(0, 0, pImage->GetWidth(), pImage->GetHeight());
		pImage->DrawImage(pDC, rc, rcSrc, CRect(4, 4, 4, 4));

	}
	else
	{
		CXTPOffice2003Theme::DrawRectangle(pDC, rc, bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
	}
}


void CXTPRibbonTheme::DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton)
{
	if (!IsImagesAvailable())
	{
		CXTPOffice2003Theme::DrawSplitButtonFrame(pDC, pButton, rcButton);
		return;
	}

	BOOL bPressed = pButton->GetPressed(), bSelected = pButton->GetSelected(), bEnabled = pButton->GetEnabled(), bChecked = pButton->GetChecked(),
		bPopuped = pButton->GetPopuped();

	BOOL bSmallSize = pButton->GetStyle() != xtpButtonIconAndCaptionBelow;

	CXTPOffice2007Image* pImageSplit, *pImageSplitDropDown;

	int nSpecialOffset = 0;

	if (pButton->GetRibbonGroup() && pButton->GetRibbonGroup()->IsControlsGrouping())
	{
		bSmallSize = TRUE;
		BOOL bLeft = pButton->GetBeginGroup() || pButton->GetWrap();

		int nIndexNext = pButton->GetControls()->GetNext(pButton->GetIndex());

		BOOL bRight = nIndexNext <= pButton->GetIndex() ||
			pButton->GetControls()->GetAt(nIndexNext)->GetBeginGroup() ||
			pButton->GetControls()->GetAt(nIndexNext)->GetWrap();

		CString strImage = bLeft ? FormatName(_T("TOOLBARBUTTONSSPECIALSPLITLEFT")) :
			FormatName(_T("TOOLBARBUTTONSSPECIALSPLITCENTER"));

		pImageSplit = LoadImage(strImage);

		strImage = bRight ? FormatName(_T("TOOLBARBUTTONSSPECIALSPLITDROPDOWNRIGHT")) :
			FormatName(_T("TOOLBARBUTTONSSPECIALSPLITDROPDOWNCENTER"));
		pImageSplitDropDown  = LoadImage(strImage);

		nSpecialOffset = 1;
	}
	else
	{
		pImageSplit = LoadImage(bSmallSize ? FormatName(_T("TOOLBARBUTTONSSPLIT22")) : FormatName(_T("TOOLBARBUTTONSSPLIT50")));

		pImageSplitDropDown = LoadImage(bSmallSize ? FormatName(_T("TOOLBARBUTTONSSPLITDROPDOWN22")) : FormatName(_T("TOOLBARBUTTONSSPLITDROPDOWN50")));
	}

	if (!pImageSplit || !pImageSplitDropDown)
		return;

	int nSplitDropDownHeight = GetSplitDropDownHeight();

	COLORREF clrText = !bEnabled ? RGB(141, 141, 141) : RGB(21, 66, 139);

	pDC->SetTextColor(clrText);
	pDC->SetBkMode (TRANSPARENT);

	CRect rcSplit = bSmallSize ? CRect(rcButton.left, rcButton.top, rcButton.right - m_nSplitButtonDropDownWidth, rcButton.bottom) :
		CRect(rcButton.left, rcButton.top, rcButton.right, rcButton.bottom - nSplitDropDownHeight);

	CRect rcSplitSrc  = pImageSplit->GetSource(0, 6 + nSpecialOffset);

	CRect rcSplitDropDown = bSmallSize ? CRect(rcButton.right - m_nSplitButtonDropDownWidth, rcButton.top, rcButton.right, rcButton.bottom) :
		CRect(rcButton.left, rcButton.bottom - nSplitDropDownHeight, rcButton.right, rcButton.bottom);

	CRect rcSplitDropDownSrc = pImageSplitDropDown->GetSource(0, 5 + nSpecialOffset);

	int nStateSplit = -1;

	BOOL bMouseInSplit = bSelected == TRUE_SPLITCOMMAND;
	BOOL bMouseInSplitDropDown = bSelected == TRUE_SPLITDROPDOWN;

	if (bEnabled == TRUE_SPLITDROPDOWN && (bSelected || bPressed || bPopuped))
	{
		nStateSplit = 4;
	}
	else if (!bEnabled)
	{
		if (IsKeyboardSelected(bSelected)) nStateSplit = 5;
	}
	else if (bChecked)
	{
		if (bPopuped) nStateSplit = 5;
		else if (!bSelected && !bPressed) nStateSplit = 2;
		else if (bSelected && !bPressed) nStateSplit = 3;
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) nStateSplit = 1;
		else if (bPressed) nStateSplit = 2;
		else if (bSelected || bPressed) nStateSplit = !bMouseInSplit ? 5 : 0;
	}
	else
	{
		if (bPopuped) nStateSplit = 5;
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) nStateSplit = 1;
		else if (bSelected || bPressed) nStateSplit = !bMouseInSplit ? 5 : 0;
	}
	nStateSplit += nSpecialOffset;

	if (nStateSplit != -1)
	{
		rcSplitSrc.OffsetRect(0, rcSplitSrc.Height() * nStateSplit);
		pImageSplit->DrawImage(pDC, rcSplit, rcSplitSrc,
			CRect(2, 2, 2, 2), 0xFF00FF, GetDrawImageFlags(pButton, rcSplit));
	}

	int nStateSplitDropDown = -1;

	if (bEnabled == TRUE_SPLITCOMMAND && (bSelected || bPressed || bPopuped))
	{
		nStateSplitDropDown = 3;
	}
	else if (!bEnabled)
	{
		if (IsKeyboardSelected(bSelected)) nStateSplitDropDown = 4;
	}
	else if (bChecked)
	{
		if (bPopuped) nStateSplitDropDown = 2;
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) nStateSplitDropDown = 0;
		else if (bSelected || bPressed) nStateSplitDropDown = !bMouseInSplitDropDown ? 4 : 0;
		else nStateSplitDropDown = 4;
	}
	else
	{
		if (bPopuped) nStateSplitDropDown = 2;
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) nStateSplitDropDown = 0;
		else if (bSelected || bPressed) nStateSplitDropDown = !bMouseInSplitDropDown ? 4 : 0;
	}

	nStateSplitDropDown += nSpecialOffset;

	if (nStateSplitDropDown != -1)
	{
		rcSplitDropDownSrc.OffsetRect(0, rcSplitDropDownSrc.Height() * nStateSplitDropDown);
		pImageSplitDropDown->DrawImage(pDC, rcSplitDropDown, rcSplitDropDownSrc, CRect(2, 2, 2, 2), 0xFF00FF,
			GetDrawImageFlags(pButton, rcSplitDropDown));
	}

	if (bSmallSize)
	{
		if (bEnabled == TRUE_SPLITCOMMAND) bEnabled = FALSE;

		CPoint pt = CPoint(rcButton.right - m_nSplitButtonDropDownWidth / 2 - 1, rcButton.CenterPoint().y);
		DrawDropDownGlyph(pDC, pButton, pt, bSelected, bPopuped, bEnabled, FALSE);
	}
}


void CXTPRibbonTheme::DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL /*bExpanded*/)
{
	pDC->FillSolidRect(x, y, cx, cy, RGB(233, 238, 238));

	pDC->FillSolidRect(x + cx - 1, y, 1, cy, RGB(197, 197, 197));
	pDC->FillSolidRect(x + cx, y, 1, cy, RGB(245, 245, 245));
}

void CXTPRibbonTheme::FillPopupLabelEntry(CDC* pDC, CRect rc)
{
	pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_LABEL));
	pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, RGB(197, 197, 197));
}

void CXTPRibbonTheme::FillMorePopupToolBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	CRect rc;
	pBar->GetClientRect(&rc);

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("MOREPOPUPBARFRAME")));
	if (!pImage)
		pImage = LoadImage(FormatName(_T("POPUPBARFRAME")));

	ASSERT(pImage);
	if (!pImage)
		return;

	pImage->DrawImage(pDC, rc, pImage->GetSource(), CRect(4, 4, 4, 4));
}

CSize CXTPRibbonTheme::DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam)
{
	if (controlType == xtpButtonExpandToolbar)
	{
		BOOL bVert = IsVerticalPosition(pBar->GetPosition());

		CXTPOffice2007Image* pImage = LoadImage(FormatName(
			bVert ? _T("TOOLBAREXPANDBUTTONVERTICAL") : _T("TOOLBAREXPANDBUTTONHORIZONTAL")));

		if (!pImage)
			return CXTPOffice2007Theme::DrawSpecialControl(pDC, controlType, pButton, pBar, bDraw, lpParam);

		if (!bDraw)
		{
			CSize szBar = *(CSize*)lpParam;
			if (!bVert)
				pButton->SetRect(CRect(szBar.cx - 12, 0, szBar.cx, szBar.cy));
			else
				pButton->SetRect(CRect(0, szBar.cy - 12, szBar.cx, szBar.cy));
		}
		else
		{
			CRect rc = pButton->GetRect();

			pImage->DrawImage(pDC, rc, pImage->GetSource(
				pButton->GetPopuped() ? 2 : pButton->GetSelected() ? 1 : 0 , 3), CRect(3, 3, 3, 3));

			COLORREF clrText = GetXtremeColor(pButton->GetPopuped() ? XPCOLOR_TOOLBAR_TEXT: pButton->GetSelected() ? XPCOLOR_HIGHLIGHT_TEXT : XPCOLOR_TOOLBAR_TEXT);
			BOOL bHiddenExists = *(BOOL*)lpParam;

			if (!bVert) rc.OffsetRect(-1, 0); else rc.OffsetRect(0, -1);
			DrawExpandSymbols(pDC, IsVerticalPosition(pBar->GetPosition()), CRect(CPoint(rc.left + 1, rc.top + 1), rc.Size()) , bHiddenExists, GetXtremeColor(COLOR_BTNHIGHLIGHT));
			DrawExpandSymbols(pDC, IsVerticalPosition(pBar->GetPosition()), rc, bHiddenExists, clrText);
		}
		return 0;
	}

	return CXTPOffice2007Theme::DrawSpecialControl(pDC, controlType, pButton, pBar, bDraw, lpParam);
}

void CXTPRibbonTheme::FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarPopup)
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("POPUPBARFRAME")));
		ASSERT(pImage);
		if (!pImage)
			return;

		pImage->DrawImage(pDC, rc, pImage->GetSource(), CRect(4, 4, 4, 4));
	}
	else if (IsFlatToolBar(pBar))
	{
		CXTPOffice2003Theme::FillCommandBarEntry(pDC, pBar);
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("TOOLBARFACEHORIZONTAL")));

		if (!pImage)
		{
			CXTPOffice2003Theme::FillCommandBarEntry(pDC, pBar);
			return;
		}

		pImage->DrawImage(pDC, rc, pImage->GetSource(), CRect(5, 5, 5, 5));
	}
	else if (pBar->GetPosition() == xtpBarLeft || pBar->GetPosition() == xtpBarRight)
	{
		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("TOOLBARFACEVERTICAL")));

		if (!pImage)
		{
			CXTPOffice2003Theme::FillCommandBarEntry(pDC, pBar);
			return;
		}

		pImage->DrawImage(pDC, rc, pImage->GetSource(), CRect(5, 5, 5, 5));
	}
	else
	{
		CXTPOffice2003Theme::FillCommandBarEntry(pDC, pBar);
	}
}

CSize CXTPRibbonTheme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	if (pBar->GetType() == xtpBarTypePopup)
	{
		if (!bDraw)
			return CSize(2, 2);

		CRect rc;
		pBar->GetClientRect(&rc);
		CRect rcControl = pControl->GetRect();

		if (!pControl->GetWrap())
		{
			int nLeft = rcControl.left - 1 + GetPopupBarGripperWidth(pBar) + m_nPopupBarTextPadding;
			HorizontalLine(pDC, nLeft, rcControl.top - 2, rcControl.right, RGB(197, 197, 197));
			HorizontalLine(pDC, nLeft, rcControl.top - 1, rcControl.right, RGB(245, 245, 245));
		}
		else
		{
			VerticalLine(pDC, rcControl.left - 2, pControl->GetRowRect().top - 1, pControl->GetRowRect().bottom + 1, RGB(197, 197, 197));
		}

		return CSize(2, 2);
	}
	else
	{
		return CXTPOffice2003Theme::DrawCommandBarSeparator(pDC, pBar, pControl, bDraw);
	}
}

void CXTPRibbonTheme::DrawRibbonFrameContextHeaders(CDC* pDC, CXTPRibbonBar* pRibbonBar, CXTPRibbonTabContextHeaders* pContextHeaders)
{
	int nCount = pContextHeaders->GetCount();
	if (nCount == 0)
		return;

	CXTPFontDC font(pDC, m_pTabPaintManager->GetFont());
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_pTabPaintManager->GetColorSet()->m_clrNormalText);

	for (int i = 0; i < nCount; i++)
	{
		CXTPRibbonTabContextHeader* pHeader = pContextHeaders->GetHeader(i);

		CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("ContextTab%sHeader"), GetColorName(pHeader->m_color)));
		if (!pImage)
			return;

		CRect rc(pHeader->m_rcRect);
		pImage->DrawImage(pDC, rc, pImage->GetSource(), CRect(1, 1, 1, 1));

		rc.left += 6;
		int nFrameBorder = pRibbonBar->IsFrameThemeEnabled() ?
			pRibbonBar->GetFrameHook()->GetFrameBorder() : 0;

		rc.top += nFrameBorder - 2;

		if (pRibbonBar->IsDwmEnabled())
		{
			DrawDwmCaptionText(pDC, rc, pHeader->m_strCpation, pRibbonBar->GetSite(), pRibbonBar->GetFrameHook()->IsFrameActive());
		}
		else
		{
			pDC->DrawText(pHeader->m_strCpation, rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS | DT_LEFT);
		}
	}

}

#define XTP_DTT_GLOWSIZE        (1UL << 11)     // iGlowSize has been specified
#define XTP_DTT_COMPOSITED      (1UL << 13)     // Draws text with antialiased alpha (needs a DIB section)

void CXTPRibbonTheme::DrawDwmCaptionText(CDC* pDC, CRect rcCaptionText, CString strWindowText, CWnd* pSite, BOOL bActive)
{
	USES_CONVERSION;

	if (pSite->GetStyle() & WS_MAXIMIZE)
	{
		pDC->SetTextColor(0xFFFFFF);
		pDC->DrawText(strWindowText, rcCaptionText,
			DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);
		return;
	}

	CXTPWinThemeWrapper wrapper;
	wrapper.OpenTheme(0, L"WINDOW");

	if (!wrapper.IsAppThemed())
	{
		pDC->SetTextColor(0);
		pDC->DrawText(strWindowText, rcCaptionText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS | DT_LEFT);
		return;
	}

	CRect rcBuffer(rcCaptionText);
	rcBuffer.InflateRect(5, 0);

	HDC hDC = 0;
	HPAINTBUFFER pb = wrapper.BeginBufferedPaint(pDC->GetSafeHdc(), rcBuffer, XTP_BPBF_TOPDOWNDIB, 0, &hDC);

	if (hDC != NULL)
	{
		::BitBlt(hDC, rcBuffer.left, rcBuffer.top, rcBuffer.Width(), rcBuffer.Height(), pDC->GetSafeHdc(), rcBuffer.left, rcBuffer.top, SRCCOPY);
		::SetTextColor(hDC, 0);
		XTP_UX_DTTOPTS op;
		op.dwSize = sizeof(op);
		op.dwFlags = XTP_DTT_COMPOSITED | XTP_DTT_GLOWSIZE;
		op. iGlowSize = 8;
		CXTPFontDC font(CDC::FromHandle(hDC), &m_fontFrameCaption);

		wrapper.DrawThemeTextEx(hDC, pSite->GetStyle() & WS_MAXIMIZE ? WP_MAXCAPTION : WP_CAPTION, bActive ? FS_ACTIVE : FS_INACTIVE, T2CW((LPTSTR)(LPCTSTR)strWindowText), -1,
			DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX,  rcCaptionText, &op);
	}

	wrapper.EndBufferedPaint(pb, TRUE);
}

void CXTPRibbonTheme::DrawRibbonFrameCaptionBar(CDC* pDC, CXTPRibbonBar* pRibbonBar)
{
	CRect rcCaption = pRibbonBar->GetCaptionRect();
	rcCaption.top--;

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("FRAMETOPCENTER")));
	pImage->DrawImage(pDC, rcCaption, pImage->GetSource(0, 2), CRect(0, 5, 0, 3));

	if (!pRibbonBar->IsQuickAccessBelowRibbon() && pRibbonBar->IsQuickAccessVisible() &&
		((CXTPControls*)pRibbonBar->GetQuickAccessControls())->GetVisibleCount() > 0)
	{
		CRect rcQuickAccess = pRibbonBar->GetQuickAccessRect();
		int nFrameBorder = 0;
		CRect rcQuickAccessArea(rcQuickAccess.left - 15, nFrameBorder, rcQuickAccess.right, pRibbonBar->GetQuickAccessHeight() + nFrameBorder);

		BOOL bDwm = FALSE;
		pImage = LoadImage(FormatName(bDwm ? _T("RIBBONQUICKACCESSAREADWM") : _T("RIBBONQUICKACCESSAREA")));
		if (pImage)
		{
			pImage->PreserveOrientation();
			pImage->DrawImage(pDC, rcQuickAccessArea, pImage->GetSource(), CRect(16, 3, 16, 3));
		}
	}

	CXTPRibbonTabContextHeaders* pContextHeaders = pRibbonBar->GetContextHeaders();
	if (pContextHeaders->GetCount() != 0)
	{
		DrawRibbonFrameContextHeaders(pDC, pRibbonBar, pContextHeaders);
	}
}

void CXTPRibbonTheme::DrawRibbonFrameCaption(CDC* pDC, CXTPRibbonBar* pRibbonBar, BOOL bActive)
{
	CFrameWnd* pSite = (CFrameWnd*)pRibbonBar->GetSite();
	CRect rcCaption = pRibbonBar->GetCaptionRect();
	CXTPOffice2007Image* pImage;

	if (!pRibbonBar->IsDwmEnabled())
	{

		pImage = LoadImage(FormatName(_T("FRAMETOPLEFT")));

		ASSERT(pImage);
		if (!pImage)
			return;

		CRect rcSrc(pImage->GetSource(bActive ? 0 : 1, 2));

		CRect rcTopLeft(rcCaption.left, rcCaption.top, rcCaption.left + rcSrc.Width(), rcCaption.bottom);
		pImage->DrawImage(pDC, rcTopLeft, rcSrc, CRect(0, 5, 0, 3), 0xFF00FF);

		//////////////////////////////////////////////////////////////////////////

		pImage = LoadImage(FormatName(_T("FRAMETOPRIGHT")));

		rcSrc = pImage->GetSource(bActive ? 0 : 1, 2);
		CRect rcTopRight(rcCaption.right - rcSrc.Width(), rcCaption.top, rcCaption.right, rcCaption.bottom);
		pImage->DrawImage(pDC, rcTopRight, rcSrc, CRect(0, 5, 0, 3));


		//////////////////////////////////////////////////////////////////////////

		pImage = LoadImage(FormatName(_T("FRAMETOPCENTER")));

		CRect rcTopCenter(rcTopLeft.right, rcCaption.top, rcTopRight.left, rcCaption.bottom);
		pImage->DrawImage(pDC, rcTopCenter, pImage->GetSource(bActive ? 0 : 1, 2), CRect(0, 5, 0, 3));
	}
	else
	{
		pDC->FillSolidRect(rcCaption, 0);
	}

	//////////////////////////////////////////////////////////////////////////

	if (!pRibbonBar->IsQuickAccessBelowRibbon() && pRibbonBar->IsQuickAccessVisible() &&
		((CXTPControls*)pRibbonBar->GetQuickAccessControls())->GetVisibleCount() > 0)
	{
		CRect rcQuickAccess = pRibbonBar->GetQuickAccessRect();
		int nFrameBorder = pRibbonBar->GetFrameHook()->GetFrameBorder();
		CRect rcQuickAccessArea(rcQuickAccess.left - 15, nFrameBorder, rcQuickAccess.right, pRibbonBar->GetQuickAccessHeight() + nFrameBorder);

		BOOL bDwm = IsCompositeRect(pRibbonBar, rcQuickAccess);
		pImage = LoadImage(FormatName(bDwm ? _T("RIBBONQUICKACCESSAREADWM") : _T("RIBBONQUICKACCESSAREA")));
		if (pImage)
		{
			pImage->PreserveOrientation();
			pImage->DrawImage(pDC, rcQuickAccessArea, pImage->GetSource(), CRect(16, 3, 16, 3));
		}
	}


	//////////////////////////////////////////////////////////////////////////


	if (pRibbonBar->GetSystemButton() == NULL)
	{
		HICON hIcon = GetFrameSmallIcon(pSite);
		if (hIcon)
		{
			CSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));

			int nTop = (rcCaption.Height()  - szIcon.cy)/2;
			int nLeft = rcCaption.left + 10;

			DrawIconEx(pDC->GetSafeHdc(), nLeft, nTop, hIcon,
				szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);
		}
	}


	CRect rcCaptionText = pRibbonBar->GetCaptionTextRect();

	CXTPRibbonTabContextHeaders* pContextHeaders = pRibbonBar->GetContextHeaders();
	if (pContextHeaders->GetCount() != 0)
	{
		DrawRibbonFrameContextHeaders(pDC, pRibbonBar, pContextHeaders);
	}

	CXTPFontDC font(pDC, &m_fontFrameCaption);

	CString strWindowText;
	pSite->GetWindowText(strWindowText);
	rcCaptionText.left += max(0, (rcCaptionText.Width() - pDC->GetTextExtent(strWindowText).cx) / 2);

	if (pSite->GetStyle() & WS_MAXIMIZE)
	{
		rcCaptionText.top += pRibbonBar->GetFrameHook()->GetFrameBorder() / 2;
	}

	if (!pRibbonBar->IsDwmEnabled())
	{
		DrawCaptionText(pDC, rcCaptionText, pSite, bActive);
	}
	else
	{
		DrawDwmCaptionText(pDC, rcCaptionText, strWindowText, pSite, bActive);
	}
}

void CXTPRibbonTheme::DrawRibbonQuickAccessButton(CDC* pDC, CXTPControlPopup* pControl)
{
	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONQUICKACCESSBUTTON")));

	ASSERT(pImage);
	if (!pImage)
		return;

	pImage->DrawImage(pDC, pControl->GetRect(), pImage->GetSource(pControl->GetPopuped() ? 2 : pControl->GetSelected() ? 1 : 0, 4),
		CRect(2, 2, 2, 2), 0xFF00FF, GetDrawImageFlags(pControl));
}

void CXTPRibbonTheme::DrawRibbonQuickAccessMoreButton(CDC* pDC, CXTPControlPopup* pControl)
{
	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("RIBBONQUICKACCESSMORE")));

	ASSERT(pImage);
	if (!pImage)
		return;

	pImage->DrawImage(pDC, pControl->GetRect(), pImage->GetSource(pControl->GetPopuped() ? 2 : pControl->GetSelected() ? 1 : 0, 4),
		CRect(2, 2, 2, 2), 0xFF00FF, GetDrawImageFlags(pControl));

}

void CXTPRibbonTheme::DrawRibbonFrameSystemButton(CDC* pDC, CXTPControlPopup* pControl, CRect rc)
{
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pControl->GetParent();

	if (!pRibbonBar->IsCaptionVisible() || !pRibbonBar->IsTabsVisible())
	{
		CXTPRibbonTheme::DrawControlToolBarParent(pDC, pControl, TRUE);
		return;
	}

	XTPImageState nState = xtpImageNormal;
	if (pControl->GetPopuped())
		nState = xtpImagePressed;
	else if (pControl->GetSelected())
		nState = xtpImageHot;

	CXTPImageManagerIcon* pIcon = pControl->GetImage(ICON_BIG);

	if (pIcon && pIcon->GetWidth() > 32)
	{
		CPoint ptIcon((rc.left + rc.right - pIcon->GetWidth()) / 2, (rc.top + rc.bottom - pIcon->GetHeight()) / 2);

		pIcon->m_bDrawComposited = pControl->HasDwmCompositedRect();
		pIcon->Draw(pDC, ptIcon, pIcon->GetIcon(nState));
		pIcon->m_bDrawComposited = FALSE;
		return;
	}

	CXTPOffice2007Image* pImage = NULL;

	if (rc.Width() > 54 && rc.Height() > 54)
		pImage = LoadImage(FormatName(_T("FRAMESYSTEMBUTTON52")));

	if (pImage == NULL)
		pImage = LoadImage(FormatName(_T("FRAMESYSTEMBUTTON")));

	ASSERT(pImage);
	if (!pImage)
		return;

	int nOffset = 0;
	if (rc.top < 0)
	{
		nOffset = rc.top;
		rc.OffsetRect(0, -nOffset);
	}

	CRect rcSrc(pImage->GetSource(nState == xtpImageNormal ? 0 : nState == xtpImageHot ? 1 : 2, 3));
	CSize szDest(rcSrc.Size());
	CPoint ptDest((rc.left + rc.right - szDest.cx) / 2, nOffset + (rc.top + rc.bottom - szDest.cy) / 2);

	pImage->DrawImage(pDC, CRect(ptDest, szDest), rcSrc, CRect(0, 0, 0, 0));

	if (pIcon)
	{
		CPoint ptIcon((rc.left + rc.right - pIcon->GetWidth()) / 2, nOffset + (rc.top + rc.bottom + 1 - pIcon->GetHeight()) / 2);
		pIcon->m_bDrawComposited = pControl->HasDwmCompositedRect();
		pIcon->Draw(pDC, ptIcon, pIcon->GetIcon(nState));
		pIcon->m_bDrawComposited = FALSE;
	}
	else
	{
		CWnd* pSite = pControl->GetParent()->GetSite();
		HICON hIcon = GetFrameLargeIcon(pSite);
		if (hIcon)
		{
			CPoint ptIcon((rc.left + rc.right - 33) / 2, nOffset + (rc.top + rc.bottom - 31) / 2);

				DrawIconEx(pDC->GetSafeHdc(), ptIcon.x, ptIcon.y, hIcon, 32, 32, NULL, NULL, DI_NORMAL);
		}
	}
}

void CXTPRibbonTheme::DrawRibbonScrollButton(CDC* pDC, CXTPControl* pControl, BOOL bScrollLeft)
{
	CXTPOffice2007Image* pImage = LoadImage(FormatName(bScrollLeft ? _T("RIBBONGROUPSSCROLLLEFT") : _T("RIBBONGROUPSSCROLLRIGHT")));

	ASSERT(pImage);
	if (!pImage)
		return;

	CRect rc(pControl->GetRect());

	int nState = pControl->GetPressed() ? 2 : pControl->GetSelected() ? 1 : 0;

	CXTPOffice2007Image* pImageGlyph = LoadImage(FormatName(bScrollLeft ? _T("RIBBONGROUPSSCROLLLEFTGLYPH") : _T("RIBBONGROUPSSCROLLRIGHTGLYPH")));

	pImage->PreserveOrientation();
	pImage->DrawImage(pDC, rc, pImage->GetSource(nState, 3), CRect(4, 6, 3, 6), 0xFF00FF);

	CSize szGlyph = pImageGlyph->GetSource(0, 3).Size();
	CRect rcGlyph(CPoint((rc.right + rc.left - szGlyph.cx) / 2, (rc.top + rc.bottom - szGlyph.cy) / 2), szGlyph);

	pImageGlyph->DrawImage(pDC, rcGlyph, pImageGlyph->GetSource(nState, 3), CRect(0, 0, 0, 0), 0xFF00FF);

}


void CXTPRibbonTheme::FillStatusBar(CDC* pDC, CXTPStatusBar* pBar)
{
	CXTPClientRect rc(pBar);

	int nWidth = rc.Width();
	if (pBar->GetPaneCount() > 0)
	{
		CRect rcItem(rc);
		pBar->GetItemRect (0, rcItem);
		nWidth = rcItem.right;
	}

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("STATUSBARLIGHT")));

	ASSERT(pImage);
	if (!pImage)
		return;

	CRect rcSrc(0, 0, pImage->GetWidth(), pImage->GetHeight());
	CRect rcLight(rc.left, rc.top, rc.left + nWidth - 3, rc.bottom);
	pImage->DrawImage(pDC, rcLight, rcSrc, CRect(0, 0, 0, 0));


	pImage = LoadImage(FormatName(_T("STATUSBARDEVIDER")));
	rcSrc.SetRect(0, 0, pImage->GetWidth(), pImage->GetHeight());
	CRect rcDevider(rcLight.right, rc.top, rcLight.right + rcSrc.Width(), rc.bottom);
	pImage->DrawImage(pDC, rcDevider, rcSrc, CRect(0, 0, 0, 0));


	pImage = LoadImage(FormatName(_T("STATUSBARDARK")));
	CRect rcDark(rcDevider.right, rc.top, rc.right, rc.bottom);
	rcSrc.SetRect(0, 0, pImage->GetWidth(), pImage->GetHeight());
	pImage->DrawImage(pDC, rcDark, rcSrc, CRect(0, 0, 0, 0));
}

void CXTPRibbonTheme::DrawStatusBarPaneBorder(CDC* /*pDC*/, CRect /*rc*/, BOOL /*bGripperPane*/)
{

}

void CXTPRibbonTheme::DrawStatusBarGripper(CDC* pDC, CRect rcClient)
{
	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("STATUSBARGRIPPER")));

	ASSERT(pImage);
	if (!pImage)
		return;

	CRect rcSrc(0, 0, pImage->GetWidth(), pImage->GetHeight());
	CRect rcGripper(rcClient.right - rcSrc.Width() , rcClient.bottom - rcSrc.Height() - 4, rcClient.right, rcClient.bottom - 4);
	pImage->DrawImage(pDC, rcGripper, rcSrc, CRect(0, 0, 0, 0), RGB(0xFF, 0, 0xFF));
}

CSize CXTPRibbonTheme::DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled)
{
	if (!bDraw)
		return CSize(13, 13);

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("TOOLBARBUTTONCHECKBOX")));

	ASSERT(pImage);
	if (!pImage)
		return CSize(13, 13);

	int nState = 0;
	if (!bEnabled)
		nState = 3;
	else if (bSelected && bPressed)
		nState = 2;
	else if (bSelected)
		nState = 1;

	if (bChecked == 1)
		nState += 4;

	if (bChecked == 2)
		nState += 8;

	pImage->DrawImage(pDC, rc, pImage->GetSource(nState, 12), CRect(0, 0, 0, 0));


	return CSize(13, 13);
}

CSize CXTPRibbonTheme::DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled)
{
	if (!bDraw)
		return CSize(13, 13);

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("TOOLBARBUTTONRADIOBUTTON")));

	ASSERT(pImage);
	if (!pImage)
		return CSize(13, 13);

	int nState = 0;
	if (!bEnabled)
		nState = 3;
	else if (bSelected && bPressed)
		nState = 2;
	else if (bSelected)
		nState = 1;

	if (bChecked)
		nState += 4;

	pImage->DrawImage(pDC, rc, pImage->GetSource(nState, 8), CRect(0, 0, 0, 0));


	return CSize(13, 13);
}

void CXTPRibbonTheme::DrawDropDownGlyph(CDC* pDC, CXTPControl* pControl, CPoint pt, BOOL bSelected, BOOL bPopuped, BOOL bEnabled, BOOL bVert)
{
	if (bVert)
	{
		CXTPOffice2003Theme::DrawDropDownGlyph(pDC, pControl, pt, bSelected, bPopuped, bEnabled, bVert);
		return;
	}

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("TOOLBARBUTTONDROPDOWNGLYPH")));

	ASSERT(pImage);
	if (!pImage)
		return;

	CRect rc(pt.x - 2, pt.y - 2, pt.x + 3, pt.y + 2);
	pImage->DrawImage(pDC, rc, pImage->GetSource(!bEnabled ? 3 : bSelected ? 1 : 0, 4),
		CRect(0, 0, 0, 0), 0xFF00FF, GetDrawImageFlags(pControl, rc)) ;
}

CSize CXTPRibbonTheme::DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pControlCombo, BOOL bDraw)
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

	CXTPOffice2007Image* pImage = LoadImage(FormatName(_T("TOOLBARBUTTONSCOMBODROPDOWN")));

	ASSERT(pImage);
	if (!pImage)
		return 0;

	if (!bEnabled)
	{
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));
		pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_EDITCTRLBORDER), GetXtremeColor(XPCOLOR_EDITCTRLBORDER));

		pImage->DrawImage(pDC, rcBtn, pImage->GetSource(4, 5), CRect(2, 2, 2, 2));
	}
	else if (bDropped)
	{
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));
		pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_EDITCTRLBORDER), GetXtremeColor(XPCOLOR_EDITCTRLBORDER));

		pImage->DrawImage(pDC, rcBtn, pImage->GetSource(3, 5), CRect(2, 2, 2, 2));
	}
	else if (bSelected)
	{
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));
		pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_EDITCTRLBORDER), GetXtremeColor(XPCOLOR_EDITCTRLBORDER));

		pImage->DrawImage(pDC, rcBtn, pImage->GetSource(2, 5), CRect(2, 2, 2, 2));

	}
	else
	{
		if (!bPopupBar) pDC->FillSolidRect(rc, m_clrControlEditNormal);
		pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_EDITCTRLBORDER), GetXtremeColor(XPCOLOR_EDITCTRLBORDER));

		pImage->DrawImage(pDC, rcBtn, pImage->GetSource(0, 5), CRect(2, 2, 2, 2));
	}

	pControlCombo->DrawEditText(pDC, rcText);

	CPoint pt = rcBtn.CenterPoint();
	DrawDropDownGlyph(pDC, pControlCombo, pt, bSelected, bDropped, bEnabled, FALSE);

	if (pControlCombo->HasDwmCompositedRect())
	{
		FillCompositeAlpha(pDC, pControlCombo->GetRect());
	}

	return 0;
}

COLORREF CXTPRibbonTheme::GetControlEditBackColor(CXTPControl* pControl)
{
	return !pControl->GetEnabled() ? GetXtremeColor(COLOR_3DFACE) :
		pControl->GetSelected() ? GetXtremeColor(COLOR_WINDOW) :
		pControl->GetParent()->GetType() == xtpBarTypePopup ?
			GetXtremeColor(XPCOLOR_MENUBAR_FACE) : m_clrControlEditNormal;
}

CSize CXTPRibbonTheme::DrawControlEdit(CDC* pDC, CXTPControlEdit* pControlEdit, BOOL bDraw)
{
	if (!bDraw)
	{
		return CXTPPaintManager::DrawControlEdit(pDC, pControlEdit, bDraw);
	}

	BOOL bPopupBar = pControlEdit->GetParent()->GetType() == xtpBarTypePopup;
	BOOL bSelected = pControlEdit->GetSelected(), bEnabled = pControlEdit->GetEnabled();

	if (bPopupBar) pDC->SetTextColor(GetXtremeColor(!pControlEdit->GetEnabled() ? XPCOLOR_MENUBAR_GRAYTEXT: bSelected ? XPCOLOR_HIGHLIGHT_TEXT : XPCOLOR_MENUBAR_TEXT));
	else pDC->SetTextColor(GetXtremeColor(!pControlEdit->GetEnabled() ? XPCOLOR_TOOLBAR_GRAYTEXT : XPCOLOR_TOOLBAR_TEXT));

	CXTPPaintManager::DrawControlEdit(pDC, pControlEdit, bDraw);

	CRect rc = pControlEdit->GetRect();
	rc.left += pControlEdit->GetLabelWidth();

	if (!bEnabled)
	{
		rc.DeflateRect(1, 0, 0, 0);
		Rectangle (pDC, rc, IsKeyboardSelected(bSelected) ? XPCOLOR_HIGHLIGHT_BORDER : XPCOLOR_EDITCTRLBORDER, COLOR_3DFACE);
	}
	else
	{
		if (bSelected)
		{
			Rectangle(pDC, rc, XPCOLOR_EDITCTRLBORDER, COLOR_WINDOW);
		}
		else
		{
			if (!bPopupBar) pDC->FillSolidRect(rc, m_clrControlEditNormal);
			pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_EDITCTRLBORDER), GetXtremeColor(XPCOLOR_EDITCTRLBORDER));
		}
	}

	if (pControlEdit->IsSpinButtonsVisible())
	{
		CXTPOffice2007Image* pImage = LoadImage(_T("TOOLBARBUTTONSPIN"));
		if (!pImage)
			return 0;

		BOOL bPressed = pControlEdit->GetPressed();

		int nState = !bEnabled ? 4 : bSelected && bPressed == 3 ? 3 : bSelected == 3 ? 2 : bSelected ? 1 : 0;
		CRect rcTop(rc.right - 18, rc.top + 1, rc.right - 1, rc.CenterPoint().y);
		pImage->DrawImage(pDC, rcTop, pImage->GetSource(nState, 10), CRect(0, 0, 0, 0), 0xFF00FF);

		nState = !bEnabled ? 9 : bSelected && bPressed == 4 ? 8 : bSelected == 4 ? 7 : bSelected ? 6 : 5;
		CRect rcBottom(rcTop.left, rcTop.bottom, rcTop.right, rc.bottom - 1);
		pImage->DrawImage(pDC, rcBottom, pImage->GetSource(nState, 10), CRect(0, 0, 0, 0), 0xFF00FF);
	}

	if (pControlEdit->HasDwmCompositedRect())
	{
		FillCompositeAlpha(pDC, pControlEdit->GetRect());
	}

	return 0;
}

void CXTPRibbonTheme::DrawKeyboardTip(CDC* pDC, CXTPCommandBarKeyboardTip* pWnd, BOOL bSetRegion)
{
	if (bSetRegion)
	{
		SetRoundRectRegion(pWnd);
	}
	else
	{
		CXTPClientRect rc(pWnd);
		CXTPFontDC font(pDC, &m_fontToolTip);

		COLORREF clrLight = GetImages()->GetImageColor(_T("Window"), _T("TooltipLight"));
		COLORREF clrDark = GetImages()->GetImageColor(_T("Window"), _T("TooltipDark"));
		COLORREF clrBorder = GetImages()->GetImageColor(_T("Window"), _T("TooltipBorder"));
		XTPDrawHelpers()->GradientFill(pDC, rc, clrLight, clrDark, FALSE);
		pDC->Draw3dRect(rc, clrBorder, clrBorder);

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetXtremeColor(pWnd->m_bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT));
		pDC->DrawText(pWnd->m_strTip, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}

void CXTPRibbonTheme::SetRoundRectRegion(CWnd* pWnd)
{
	CXTPWindowRect rc(pWnd);
	rc.OffsetRect(-rc.TopLeft());

	int cx = rc.Width(), cy = rc.Height();

	RECT rgn[] =
	{
		{1, 0, cx - 1, 1}, {0, 1, cx, cy - 1}, {1, cy - 1, cx - 1, cy}
	};

	int nSizeData = sizeof(RGNDATAHEADER) + sizeof(rgn);

	RGNDATA* pRgnData = (RGNDATA*)malloc(nSizeData);
	MEMCPY_S(&pRgnData->Buffer, (void*)&rgn, sizeof(rgn));

	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	pRgnData->rdh.nCount = sizeof(rgn) / sizeof(RECT);
	pRgnData->rdh.nRgnSize = 0;
	pRgnData->rdh.rcBound = CRect(0, 0, cx, cy);

	CRgn rgnResult;
	VERIFY(rgnResult.CreateFromData(NULL, nSizeData, pRgnData));

	free(pRgnData);

	pWnd->SetWindowRgn((HRGN)rgnResult.Detach(), FALSE);
}

void CXTPRibbonTheme::SetCommandBarRegion(CXTPCommandBar* pCommandBar)
{
	if (pCommandBar->GetPosition() != xtpBarPopup)
		return;

	SetRoundRectRegion(pCommandBar);
}

void CXTPRibbonTheme::AdjustExcludeRect(CRect& /*rc*/, CXTPControl* /*pControl*/, BOOL /*bVertical*/)
{
}

void CXTPRibbonTheme::DrawSplitButtonPopup(CDC* pDC, CXTPControl* pButton)
{
	CRect rcButton = pButton->GetRect();
	BOOL bSelected = pButton->GetSelected(), bPopuped = pButton->GetPopuped(), bEnabled = pButton->GetEnabled();

	CXTPOffice2007Image* pImage = LoadImage(_T("MENUSPLITDROPDOWN"));

	int nState = !bSelected || !bEnabled ? 0 : bPopuped ? 2 : 1;

	pImage->DrawImage(pDC, CRect(rcButton.right - m_nSplitButtonPopupWidth, rcButton.top, rcButton.right, rcButton.bottom),
		pImage->GetSource(nState, 3), CRect(2, 2, 2, 2), 0xFF00FF);
}

void CXTPRibbonTheme::DrawControlPopupGlyph(CDC* pDC, CXTPControl* pButton)
{
	CRect rcButton = pButton->GetRect();

	CXTPOffice2007Image* pImage = LoadImage(_T("MENUPOPUPGLYPH"));
	CRect rcSrc(pImage->GetSource(pButton->GetEnabled() ? 0 : 1, 2));
	CPoint pt = CPoint(rcButton.right - 14, (rcButton.top + rcButton.bottom - rcSrc.Height()) / 2);

	pImage->DrawImage(pDC, CRect(pt, rcSrc.Size()), rcSrc, CRect(0, 0, 0, 0), 0xFF00FF);
}

//////////////////////////////////////////////////////////////////////////
//


