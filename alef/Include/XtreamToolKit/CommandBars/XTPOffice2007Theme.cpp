// XTPOffice2007Theme.cpp : implementation of the CXTPOffice2007Theme class.
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
#include "Common/XTPOffice2007Image.h"
#include "Common/XTPVc80Helpers.h"

#include "XTPControls.h"
#include "XTPControl.h"
#include "XTPControlPopup.h"
#include "XTPCommandBar.h"
#include "XTPPopupBar.h"
#include "XTPControlGallery.h"
#include "XTPStatusBar.h"

#include "XTPOffice2007Theme.h"

#include "XTPOffice2007FrameHook.h"

#ifndef OIC_WINLOGO
#define OIC_WINLOGO 32517
#endif

#ifndef LAYOUT_BITMAPORIENTATIONPRESERVED
#define LAYOUT_BITMAPORIENTATIONPRESERVED 0x00000008
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace XTPPaintThemes;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class _XTP_EXT_CLASS CXTPControlGalleryOffice2007Theme : public CXTPControlGalleryPaintManager
{
public:
	CXTPControlGalleryOffice2007Theme(CXTPPaintManager* pPaintManager)
		: CXTPControlGalleryPaintManager(pPaintManager)
	{

	}

public:
	virtual void FillControl(CDC* pDC, CXTPControlGallery* pGallery, CRect rc);
	virtual void DrawPopupScrollBar(CDC* pDC, CXTPControlGallery* pGallery);
	virtual void DrawScrollBar(CDC* pDC, CXTPScrollBase* pGallery);
	virtual void DrawLabel(CDC* pDC, CXTPControlGalleryItem* pLabel, CRect rc);
	virtual void RefreshMetrics();

protected:
	COLORREF m_clrControlGallerySelected;
	COLORREF m_clrControlGalleryNormal;
	COLORREF m_clrControlGalleryBorder;

};


CXTPOffice2007Theme::CXTPOffice2007Theme()
{
	m_nShadowOptions = xtpShadowOfficeAlpha;
	m_systemTheme = xtpSystemThemeUnknown;
	m_bThemedStatusBar = TRUE;
	m_bFrameStatusBar = FALSE;
	m_bOffice2007Padding = TRUE;

	delete m_pGalleryPaintManager;
	m_pGalleryPaintManager = new CXTPControlGalleryOffice2007Theme(this);

	m_pImages = XTPOffice2007Images();
}

CXTPOffice2007Theme::~CXTPOffice2007Theme()
{
}

CXTPOffice2007Images* CXTPOffice2007Theme::GetImages() const
{
	return m_pImages;
}

void CXTPOffice2007Theme::SetImages(CXTPOffice2007Images* pImages)
{
	m_pImages = pImages;
}

void CXTPOffice2007Theme::SetImageHandle(HMODULE hResource, LPCTSTR lpszIniFileName)
{
	GetImages()->SetHandle(hResource, lpszIniFileName);
	RefreshMetrics();
}

CXTPOffice2007Image* CXTPOffice2007Theme::LoadImage(LPCTSTR lpszFileName)
{
	return GetImages()->LoadFile(lpszFileName);
}

BOOL CXTPOffice2007Theme::IsImagesAvailable()
{
	return TRUE;
}

COLORREF CXTPOffice2007Theme::GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition)
{
	if (barType == xtpBarTypeMenuBar && !bSelected && bEnabled && !bPressed && !bChecked && !bPopuped)
	{
		return m_clrMenuBarText;
	}
	return CXTPOffice2003Theme::GetRectangleTextColor(bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
}

void CXTPOffice2007Theme::RefreshMetrics()
{
	CXTPOffice2003Theme::RefreshMetrics();

//////////////////////////////////////////////////////////////////////////

	CXTPPaintManager::CNonClientMetrics ncm;
	ncm.lfSmCaptionFont.lfWeight = FW_NORMAL;

	if (m_bClearTypeTextQuality &&
		XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		ncm.lfCaptionFont.lfQuality = 6;
	}

	ncm.lfCaptionFont.lfWeight = FW_NORMAL;
	if (m_bUseOfficeFont && FontExists(m_strOfficeFont))
		STRCPY_S(ncm.lfCaptionFont.lfFaceName, LF_FACESIZE, m_strOfficeFont);

	if (ncm.lfCaptionFont.lfHeight < 0)
		ncm.lfCaptionFont.lfHeight = min(-11, ncm.lfCaptionFont.lfHeight);

	m_fontFrameCaption.SetStandardFont(&ncm.lfCaptionFont);

	int nBorderSize = 1;
	SystemParametersInfo(SPI_GETBORDER, 0, &nBorderSize, FALSE);

	m_nFrameCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
	m_nFrameCaptionHeight = max(m_nFrameCaptionHeight, 17);

	m_nRibbonCaptionHeight = GetSystemMetrics(SM_CYCAPTION) + 1;
	m_nRibbonCaptionHeight = max(m_nRibbonCaptionHeight, 18);

//////////////////////////////////////////////////////////////////////////

	m_clrStatusTextColor = GetImages()->GetImageColor(_T("StatusBar"), _T("StatusBarText"));
	m_clrStatusBarShadow = GetImages()->GetImageColor(_T("StatusBar"), _T("StatusBarShadow"));

	m_clrStatusBarTop.SetStandardValue(GetImages()->GetImageColor(_T("StatusBar"), _T("StatusBarFaceTopLight")),
		GetImages()->GetImageColor(_T("StatusBar"), _T("StatusBarFaceTopDark")));
	m_clrStatusBarBottom.SetStandardValue(GetImages()->GetImageColor(_T("StatusBar"), _T("StatusBarFaceBottomLight")),
		GetImages()->GetImageColor(_T("StatusBar"), _T("StatusBarFaceBottomDark")));



	m_clrDockBar.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("DockBarFace")));
	m_clrCommandBar.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ToolbarFaceLight")),
		GetImages()->GetImageColor(_T("Toolbar"), _T("ToolbarFaceDark")), 0.75f);

	m_clrToolbarShadow.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ToolbarFaceShadow")));

	m_clrToolbarExpand.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ControlToolbarExpandLight")),
		GetImages()->GetImageColor(_T("Toolbar"), _T("ControlToolbarExpandDark")), 0.75f);


	m_clrPopupControl.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ControlHighlightPopupedLight")),
		GetImages()->GetImageColor(_T("Toolbar"), _T("ControlHighlightPopupedDark")));
	m_clrMenuExpandedGripper.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("MenuPopupExpandedGripperLight")),
		GetImages()->GetImageColor(_T("Toolbar"), _T("MenuPopupExpandedGripperDark")));
	m_clrMenuExpand = m_clrPopupControl;

	m_clrShadowFactor = 0;
	m_clrTearOffGripper.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("MenuPopupTearOffGripper")));

	m_clrMenuBarText = GetImages()->GetImageColor(_T("Toolbar"), _T("MenuBarText"));

	m_arrColor[XPCOLOR_MENUBAR_FACE].SetStandardValue(RGB(246, 246, 246));
	m_arrColor[XPCOLOR_MENUBAR_BORDER].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("MenuPopupBorder")));
	m_arrColor[XPCOLOR_TOOLBAR_GRIPPER].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ToolbarGripper")));
	m_arrColor[XPCOLOR_MENUBAR_TEXT].SetStandardValue(0);
	m_arrColor[XPCOLOR_HIGHLIGHT_TEXT].SetStandardValue(0);
	m_arrColor[XPCOLOR_TOOLBAR_TEXT].SetStandardValue(0);

	m_arrColor[XPCOLOR_TOOLBAR_GRAYTEXT].SetStandardValue(RGB(141, 141, 141));
	m_arrColor[XPCOLOR_HIGHLIGHT_DISABLED_BORDER].SetStandardValue(RGB(141, 141, 141));
	m_arrColor[XPCOLOR_MENUBAR_GRAYTEXT].SetStandardValue(RGB(141, 141, 141));

	m_arrColor[XPCOLOR_FRAME].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("WindowFrame")));

	m_clrFloatingGripper.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("FloatingBarGripper")));
	m_clrFloatingGripperText.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("FloatingBarGripperText")));
	m_arrColor[XPCOLOR_FLOATBAR_BORDER].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("FloatingBarBorder")));

	m_arrColor[COLOR_APPWORKSPACE].SetStandardValue(GetImages()->GetImageColor(_T("Workspace"), _T("AppWorkspace")));
	m_arrColor[XPCOLOR_3DFACE].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("ButtonFace")));
	m_arrColor[XPCOLOR_3DSHADOW].SetStandardValue(m_clrToolbarShadow);

	m_arrColor[XPCOLOR_TOOLBAR_FACE].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ToolbarFace")));
	m_arrColor[XPCOLOR_SEPARATOR].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ToolbarSeparator")));
	m_arrColor[XPCOLOR_DISABLED].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ControlDisabledIcon")));

	m_clrWorkspaceClientTop = GetImages()->GetImageColor(_T("Workspace"), _T("WorkspaceClientTop"));
	m_clrWorkspaceClientMiddle = GetImages()->GetImageColor(_T("Workspace"), _T("WorkspaceClientMiddle"));
	m_clrWorkspaceClientBottom =  GetImages()->GetImageColor(_T("Workspace"), _T("WorkspaceClientBottom"));


	m_arrColor[XPCOLOR_HIGHLIGHT].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightSelected")));
	m_arrColor[XPCOLOR_HIGHLIGHT_BORDER].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightSelectedBorder")));

	m_arrColor[XPCOLOR_HIGHLIGHT_PUSHED].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightPressed")));
	m_arrColor[XPCOLOR_HIGHLIGHT_PUSHED_BORDER].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightPressedBorder")));

	m_arrColor[XPCOLOR_HIGHLIGHT_CHECKED].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightChecked")));
	m_arrColor[XPCOLOR_HIGHLIGHT_CHECKED_BORDER].SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightCheckedBorder")));

	m_arrColor[XPCOLOR_PUSHED_TEXT].SetStandardValue(0);

	m_bLunaTheme = TRUE;

	m_grcLunaSelected.SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightSelectedLight")),
		GetImages()->GetImageColor(_T("Window"), _T("HighlightSelectedDark")));
	m_grcLunaChecked.SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightCheckedLight")),
		GetImages()->GetImageColor(_T("Window"), _T("HighlightCheckedDark")));
	m_grcLunaPushed.SetStandardValue(GetImages()->GetImageColor(_T("Window"), _T("HighlightPressedLight")),
		GetImages()->GetImageColor(_T("Window"), _T("HighlightPressedDark")));

	m_clrFrameBorderActive0 = GetImages()->GetImageColor(_T("Window"), _T("BorderActive0"));
	m_clrFrameBorderActive1 = GetImages()->GetImageColor(_T("Window"), _T("BorderActive1"));
	m_clrFrameBorderActive2 = GetImages()->GetImageColor(_T("Window"), _T("BorderActive2"));
	m_clrFrameBorderActive3 = GetImages()->GetImageColor(_T("Window"), _T("BorderActive3"));
	m_clrFrameBorderInactive0 = GetImages()->GetImageColor(_T("Window"), _T("BorderInactive0"));
	m_clrFrameBorderInactive1 = GetImages()->GetImageColor(_T("Window"), _T("BorderInactive1"));
	m_clrFrameBorderInactive2 = GetImages()->GetImageColor(_T("Window"), _T("BorderInactive2"));
	m_clrFrameBorderInactive3 = GetImages()->GetImageColor(_T("Window"), _T("BorderInactive3"));

	m_clrFrameCaptionTextActiveTitle = GetImages()->GetImageColor(_T("Window"), _T("CaptionTextActiveTitle"));
	m_clrFrameCaptionTextInActive = GetImages()->GetImageColor(_T("Window"), _T("CaptionTextInActive"));
	m_clrFrameCaptionTextActive = GetImages()->GetImageColor(_T("Window"), _T("CaptionTextActive"));

	m_arrColor[XPCOLOR_LABEL].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ControlLabel")));
	m_arrColor[XPCOLOR_EDITCTRLBORDER].SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ControlEditBorder")));

	m_clrDisabledIcon.SetStandardValue(GetImages()->GetImageColor(_T("Toolbar"), _T("ControlDisabledIconLight")),
		GetImages()->GetImageColor(_T("Toolbar"), _T("ControlDisabledIconDark")));

	CreateGradientCircle();
}



//////////////////////////////////////////////////////////////////////////
//

void CXTPOffice2007Theme::DrawCaptionText(CDC* pDC, CRect rcCaptionText, CWnd* pSite, BOOL bActive)
{
	CString strWindowText;
	pSite->GetWindowText(strWindowText);

	pDC->SetBkMode(TRANSPARENT);
	CXTPFontDC font(pDC, &m_fontFrameCaption);

	pDC->SetTextColor(!bActive ? m_clrFrameCaptionTextInActive : m_clrFrameCaptionTextActive); //RGB(62, 106, 170));

	if (pSite->GetStyle() & FWS_PREFIXTITLE)
	{
#if _MSC_VER >= 1200
		CFrameWnd* pFrame = pSite->IsFrameWnd() ? (CFrameWnd*)pSite : NULL;
		CString strTitle = pFrame ? pFrame->GetTitle() : _T("");

		if (!strTitle.IsEmpty() && strWindowText.Right(strTitle.GetLength()) == strTitle & ((pSite->GetExStyle() & WS_EX_LAYOUTRTL) == 0))
		{
			strWindowText.Delete(strWindowText.GetLength() - strTitle.GetLength(), strTitle.GetLength());
			pDC->DrawText(strWindowText, rcCaptionText, DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);

			int nExtent = pDC->GetTextExtent(strWindowText).cx;
			rcCaptionText.left += nExtent + 2;

			if (rcCaptionText.left < rcCaptionText.right - 5)
			{
				if (bActive) pDC->SetTextColor(m_clrFrameCaptionTextActiveTitle);
				pDC->DrawText(strTitle, rcCaptionText, DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);
			}

		}
		else
#endif
		{
			pDC->DrawText(strWindowText, rcCaptionText, DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);
		}
	}
	else
	{
		pDC->DrawText(strWindowText, rcCaptionText, DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);
	}
}



HICON CXTPOffice2007Theme::GetFrameSmallIcon(CWnd* pFrame)
{
	if (!pFrame)
		return NULL;

	DWORD dwStyle = pFrame->GetStyle();
	DWORD dwExStyle = pFrame->GetExStyle();

	if (dwExStyle & WS_EX_TOOLWINDOW)
		return NULL;

	if ((dwStyle & WS_SYSMENU) == 0)
		return NULL;

	HICON hIcon = (HICON)(DWORD_PTR)::SendMessage(pFrame->m_hWnd, WM_GETICON, ICON_SMALL, 0);
	if (hIcon)
		return hIcon;

	hIcon = (HICON)(DWORD_PTR)::GetClassLongPtr(pFrame->m_hWnd, GCLP_HICONSM);
	if (hIcon)
		return hIcon;

	if (((dwStyle & (WS_BORDER | WS_DLGFRAME)) != WS_DLGFRAME) && ((dwExStyle & WS_EX_DLGMODALFRAME) == 0))
	{
		ULONG_PTR dwResult;

		if (SendMessageTimeout(pFrame->GetSafeHwnd(),
			WM_QUERYDRAGICON,
			0,
			0,
			SMTO_NORMAL,
			100,
			&dwResult))
		{
			hIcon = (HICON)dwResult;
		}

		if (hIcon == NULL)
		{
			hIcon = AfxGetApp()->LoadOEMIcon(OIC_WINLOGO);
		}
	}

	return hIcon;
}

HICON CXTPOffice2007Theme::GetFrameLargeIcon(CWnd* pFrame)
{
	if (!pFrame)
		return NULL;

	HICON hIcon = (HICON)(DWORD_PTR)::SendMessage(pFrame->m_hWnd, WM_GETICON, ICON_BIG, 0);
	if (hIcon)
		return hIcon;

	hIcon = (HICON)(DWORD_PTR)::GetClassLongPtr(pFrame->m_hWnd, GCLP_HICON);
	if (hIcon)
		return hIcon;

	return hIcon;
}



void CXTPOffice2007Theme::DrawRibbonFrame(CDC* pDC, CXTPOffice2007FrameHook* pFrameHook)
{
	CWnd* pSite = pFrameHook->GetSite();
	BOOL bActive = pFrameHook->IsFrameActive();

	CXTPClientRect rcBorders(pSite);
	pSite->ClientToScreen(rcBorders);

	CXTPWindowRect rc(pSite);

	int nRightBorder = rcBorders.left - rc.left, nLeftBorder = rcBorders.left - rc.left;
	int nBottomBorder = rc.bottom - rcBorders.bottom;

	rc.OffsetRect(-rc.TopLeft());
	CRect rcFrame(rc);

	int nCaptionHeight = pFrameHook->GetCaptionHeight();
	rcFrame.top += nCaptionHeight;

	int nStatusHeight = 0;
	BOOL bHasStatusBar = pFrameHook->IsFrameHasStatusBar(&nStatusHeight);

	int nBordersHeight = bHasStatusBar ? rcFrame.Height() - nStatusHeight - 1 : rcFrame.Height();


	if (nLeftBorder > 0) pDC->FillSolidRect(rc.left + 0, rcFrame.top, 1, rcFrame.Height(), bActive ? m_clrFrameBorderActive0 : m_clrFrameBorderInactive0);
	if (nLeftBorder > 1) pDC->FillSolidRect(rc.left + 1, rcFrame.top, 1, nBordersHeight, bActive ? m_clrFrameBorderActive1 : m_clrFrameBorderInactive1);
	if (nLeftBorder > 2) pDC->FillSolidRect(rc.left + 2, rcFrame.top, 1, nBordersHeight, bActive ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
	if (nLeftBorder > 3) pDC->FillSolidRect(rc.left + 3, rcFrame.top, nLeftBorder - 3, nBordersHeight, bActive ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);

	if (nRightBorder > 0) pDC->FillSolidRect(rc.right - 1, rcFrame.top, 1, rcFrame.Height(), bActive ? m_clrFrameBorderActive0 : m_clrFrameBorderInactive0);
	if (nRightBorder > 1) pDC->FillSolidRect(rc.right - 2, rcFrame.top, 1, nBordersHeight, bActive ? m_clrFrameBorderActive1 : m_clrFrameBorderInactive1);
	if (nRightBorder > 2) pDC->FillSolidRect(rc.right - 3, rcFrame.top, 1, nBordersHeight, bActive ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
	if (nRightBorder > 3) pDC->FillSolidRect(rc.right - nRightBorder, rcFrame.top, nRightBorder - 3, nBordersHeight, bActive ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);

	pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1,
		bActive ? pSite->GetStyle() & WS_MAXIMIZE ? m_clrFrameBorderActive3 : m_clrFrameBorderActive0 : m_clrFrameBorderInactive0);

	if (nBottomBorder > 1)
	{
		pDC->FillSolidRect(rc.left + 1, rc.bottom - nBottomBorder, rc.Width() - 2, nBottomBorder - 1, bActive ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
	}

	//////////////////////////////////////////////////////////////////////////
	CRect rcCaption(rc.left, rc.top, rc.right, rc.top + nCaptionHeight);

	if (pFrameHook->IsCaptionVisible())
	{
		CXTPBufferDC dc(*pDC, rcCaption);

		CXTPOffice2007Image* pImage = LoadImage(_T("FRAMETOPLEFT"));
		CRect rcSrcTopLeft(pImage->GetSource(bActive ? 0 : 1, 2));

		CRect rcTopLeft(rc.left, rc.top, rc.left + rcSrcTopLeft.Width(), rcCaption.bottom);
		pImage->DrawImage(&dc, rcTopLeft, rcSrcTopLeft, CRect(0, 5, 0, 3));

		//

		pImage = LoadImage(_T("FRAMETOPRIGHT"));
		CRect rcSrcTopRight(pImage->GetSource(bActive ? 0 : 1, 2));

		CRect rcTopRight(rc.right - rcSrcTopRight.Width(), rc.top, rc.right, rcCaption.bottom);
		pImage->DrawImage(&dc, rcTopRight, rcSrcTopRight, CRect(0, 5, 0, 3));

		pImage = LoadImage(_T("FRAMETOPCENTER"));

		CRect rcTopCenter(rc.left + rcTopLeft.Width(), rc.top, rc.right - rcSrcTopRight.Width(), rcCaption.bottom);
		pImage->DrawImage(&dc, rcTopCenter, pImage->GetSource(bActive ? 0 : 1, 2), CRect(0, 5, 0, 3));

		CRect rcCaptionText(rcCaption);
		rcCaptionText.left = 7;
		rcCaptionText.DeflateRect(0, pFrameHook->GetFrameBorder(), 0, 3);

		HICON hIcon = GetFrameSmallIcon(pSite);
		if (hIcon)
		{
			CSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));

			int cBorders = pFrameHook->GetFrameBorder() - 1;
			int cxySlot = rcCaption.Height() - cBorders;
			int nTop = cBorders + (cxySlot - szIcon.cy)/2;

			DWORD dwLayout = XTPDrawHelpers()->IsContextRTL(&dc);
			if (dwLayout & LAYOUT_RTL)
				XTPDrawHelpers()->SetContextRTL(&dc, dwLayout | LAYOUT_BITMAPORIENTATIONPRESERVED);

			DrawIconEx(dc.m_hDC, rcCaptionText.left, nTop, hIcon,
				szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);

			if (dwLayout & LAYOUT_RTL)
				XTPDrawHelpers()->SetContextRTL(&dc, dwLayout);


			rcCaptionText.left = 7 + szIcon.cx + 5;
		}

		CXTPControls* pCaptionButtons = pFrameHook->GetCaptionButtons();
		for (int i = 0; i < pCaptionButtons->GetCount(); i++)
		{
			CXTPControl* pControl = pCaptionButtons->GetAt(i);
			if (!pControl->IsVisible())
				continue;

			DrawRibbonFrameCaptionButton(&dc, pControl->GetRect(), pControl->GetID(), pControl->GetSelected(), pControl->GetPressed(), pControl->GetEnabled() && bActive);

			rcCaptionText.right = min(rcCaptionText.right, pControl->GetRect().left);
		}

		rcCaptionText.right -= nRightBorder;


		DrawCaptionText(&dc, rcCaptionText, pSite, bActive);
	}
	else
	{

		CXTPOffice2007Image* pImage = LoadImage(_T("FRAMETOPLEFT"));
		CRect rcSrc(pImage->GetSource(bActive ? 0 : 1, 2));
		rcSrc.right = nLeftBorder;

		CRect rcTopLeft(rc.left, rcCaption.top, rc.left + nLeftBorder, rcCaption.bottom);
		pImage->DrawImage(pDC, rcTopLeft, rcSrc, CRect(0, 5, 0, 3));

		//////////////////////////////////////////////////////////////////////////

		pImage = LoadImage(_T("FRAMETOPRIGHT"));
		rcSrc = pImage->GetSource(bActive ? 0 : 1, 2);
		rcSrc.left = rcSrc.right - nRightBorder;

		CRect rcTopRight(rc.right - nRightBorder, rcCaption.top, rc.right, rcCaption.bottom);
		pImage->DrawImage(pDC, rcTopRight, rcSrc, CRect(0, 5, 0, 3));
	}

	if (bHasStatusBar)
	{
		CXTPOffice2007Image* pImage = LoadImage(_T("STATUSBARLIGHT"));
		CRect rcSrc(0, 0, nLeftBorder - 1, pImage->GetHeight());
		CRect rcLight(rc.left + 1, rc.bottom - nStatusHeight - nBottomBorder, rc.left + nLeftBorder, rc.bottom - nBottomBorder);
		pImage->DrawImage(pDC, rcLight, rcSrc, CRect(0, 0, 0, 0));

		pImage = LoadImage(_T("STATUSBARDARK"));
		rcSrc.SetRect(0, 0, nRightBorder - 1, pImage->GetHeight());
		CRect rcDark(rc.right - nRightBorder, rc.bottom - nStatusHeight - nBottomBorder, rc.right - 1, rc.bottom - nBottomBorder);
		pImage->DrawImage(pDC, rcDark, rcSrc, CRect(0, 0, 0, 0));

		if (nLeftBorder > 3)
		{
			pImage = LoadImage(_T("FRAMEBOTTOMLEFT"));
			rcSrc = pImage->GetSource(bActive ? 0 : 1, 2);
			CRect rcBottomLeft(rc.left, rc.bottom - rcSrc.Height(), rc.left + rcSrc.Width(), rc.bottom);
			pImage->DrawImage(pDC, rcBottomLeft, rcSrc, CRect(0, 0, 0, 0), 0xFF00FF);
		}

		if (nRightBorder > 3)
		{
			pImage = LoadImage(_T("FRAMEBOTTOMRIGHT"));
			rcSrc = pImage->GetSource(bActive ? 0 : 1, 2);
			CRect rcBottomRight(rc.right - rcSrc.Width(), rc.bottom - rcSrc.Height(), rc.right, rc.bottom);
			pImage->DrawImage(pDC, rcBottomRight, rcSrc, CRect(0, 0, 0, 0), 0xFF00FF);
		}
	}
}

void CXTPOffice2007Theme::DrawRibbonFrameCaptionButton(CDC* pDC, CRect rc, int nId, BOOL bSelected, BOOL bPressed, BOOL bActive)
{
	int nState = !bActive ? 3 : bPressed && bSelected ? 2 : bSelected || bPressed ? 1 : 0;

	if (bSelected || bPressed)
	{
		CXTPOffice2007Image* pImage = LoadImage(_T("FRAMECAPTIONBUTTON"));
		pImage->DrawImage(pDC, rc, pImage->GetSource(bPressed && bSelected ? 1 : 0, 2), CRect(3, 3, 3, 3), 0xFF00FF);
	}

	int nGlyphSize = 23;
	if (rc.Width() < 27) nGlyphSize = 17;
	if (rc.Width() < 15) nGlyphSize = 13;

	LPCTSTR lpszButton = nId == SC_CLOSE ? _T("CLOSE") : nId == SC_MINIMIZE ? _T("MINIMIZE") :
		nId == SC_MAXIMIZE ? _T("MAXIMIZE") : _T("RESTORE");

	CString strImage;
	strImage.Format(_T("FRAMECAPTION%s%i"), lpszButton, nGlyphSize);
	CXTPOffice2007Image* pImage = LoadImage(strImage);

	CRect rcSrc(pImage->GetSource(nState, 5));

	CRect rcGlyph(CPoint((rc.right + rc.left - rcSrc.Width()) / 2, (rc.top + rc.bottom - rcSrc.Height()) / 2), rcSrc.Size());

	pImage->DrawImage(pDC, rcGlyph, rcSrc, CRect(0, 0, 0, 0), 0xFF00FF);
}

void CXTPOffice2007Theme::FillWorkspace(CDC* pDC, CRect rc, CRect rcExclude)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rc);
	pDC->SelectClipRgn(&rgn);

	pDC->ExcludeClipRect(rcExclude);

	CXTPOffice2007Image* pImage = LoadImage(_T("WORKSPACETOPLEFT"));
	ASSERT(pImage);

	CRect rcSrc(pImage->GetSource());
	CRect rcTopLeft(rc);
	rcTopLeft.bottom = rcTopLeft.top + rcSrc.Height();
	rcTopLeft.right = rcTopLeft.left + max(rcTopLeft.Width(), rcSrc.Width());

	pImage->DrawImage(pDC, rcTopLeft, rcSrc, CRect(rcSrc.Width() - 1, 0, 0, 0));

	CRect rcFill(rc.left, rc.top + rcSrc.Height(), rc.right, rc.bottom);
	CRect rcFillTop(rcFill.left, rcFill.top, rcFill.right, rcFill.top + rcFill.Height() * 2 / 3);
	CRect rcFillBottom(rcFill.left, rcFillTop.bottom, rcFill.right, rcFill.bottom);

	XTPDrawHelpers()->GradientFill(pDC, rcFillTop, m_clrWorkspaceClientTop, m_clrWorkspaceClientMiddle, FALSE);
	XTPDrawHelpers()->GradientFill(pDC, rcFillBottom, m_clrWorkspaceClientMiddle, m_clrWorkspaceClientBottom, FALSE);

	pDC->SelectClipRgn(NULL);
}

HRGN CXTPOffice2007Theme::CalcRibbonFrameRegion(CXTPOffice2007FrameHook* pFrameHook, CSize sz)
{
	if (pFrameHook->GetSite()->GetStyle() & WS_MAXIMIZE)
	{
		return NULL;
	}

	int cx = sz.cx, cy = sz.cy;

	RECT rgnTopFrame[] =
	{
		{4, 0, cx - 4, 1}, {2, 1, cx - 2, 2}, {1, 2, cx - 1, 3}, {1, 3, cx - 1, 4}, {0, 4, cx, cy - 4}
	};

	RECT rgnRibbonBottomFrame[] =
	{
		{1, cy - 4, cx - 1, cy - 2}, {2, cy - 2, cx - 2, cy - 1}, {4, cy - 1, cx - 4, cy - 0}
	};

	RECT rgnSimpleBottomFrame[] =
	{
		{0, cy - 4, cx, cy}
	};

	BOOL bHasStatusBar = pFrameHook->IsFrameHasStatusBar() && pFrameHook->GetFrameBorder() > 3;

	int nSizeTopRect = sizeof(rgnTopFrame);
	int nSizeBottomRect = bHasStatusBar ? sizeof(rgnRibbonBottomFrame) : sizeof(rgnSimpleBottomFrame);

	int nSizeData = sizeof(RGNDATAHEADER) + nSizeTopRect + nSizeBottomRect;

	RGNDATA* pRgnData = (RGNDATA*)malloc(nSizeData);
	memcpy(&pRgnData->Buffer, (void*)&rgnTopFrame, nSizeTopRect);
	memcpy(&pRgnData->Buffer + nSizeTopRect, bHasStatusBar ? (void*)&rgnRibbonBottomFrame : (void*)&rgnSimpleBottomFrame, nSizeBottomRect);

	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	pRgnData->rdh.nCount = (nSizeTopRect + nSizeBottomRect) / sizeof(RECT);
	pRgnData->rdh.nRgnSize = 0;
	pRgnData->rdh.rcBound = CRect(0, 0, cx, cy);

	CRgn rgnResult;
	VERIFY(rgnResult.CreateFromData(NULL, nSizeData, pRgnData));

	free(pRgnData);

	return (HRGN)rgnResult.Detach();
}

void CXTPOffice2007Theme::FillStatusBar(CDC* pDC, CXTPStatusBar* pBar)
{
	CXTPClientRect rc(pBar);

	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_clrStatusBarShadow);
	XTPDrawHelpers()->GradientFill(pDC, CRect(rc.left, rc.top + 1, rc.right, rc.top + 9),
		m_clrStatusBarTop, FALSE);
	XTPDrawHelpers()->GradientFill(pDC, CRect(rc.left, rc.top + 9, rc.right, rc.bottom),
		m_clrStatusBarBottom, FALSE);

}

void CXTPOffice2007Theme::DrawStatusBarPaneBorder(CDC* pDC, CRect rc, BOOL bGripperPane)
{
	if (!bGripperPane)
	{
		pDC->FillSolidRect(rc.right - 1, rc.top, 1, rc.Height() - 2, GetXtremeColor(XPCOLOR_SEPARATOR));
		pDC->FillSolidRect(rc.right, rc.top, 1, rc.Height() - 2, RGB(255, 255, 255));
	}
}

void CXTPOffice2007Theme::DrawStatusBarGripper(CDC* pDC, CRect rcClient)
{
	CPoint pt(rcClient.right - 3, rcClient.bottom - 3);

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3 - y; x++)
		{
			pDC->FillSolidRect(pt.x + 1 - x * 4, pt.y + 1 - y * 4, 2, 2, RGB(255, 255, 255));
			pDC->FillSolidRect(pt.x + 0 - x * 4, pt.y + 0 - y * 4, 2, 2, GetXtremeColor(XPCOLOR_SEPARATOR));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//

void CXTPControlGalleryOffice2007Theme::FillControl(CDC* pDC, CXTPControlGallery* pGallery, CRect rcControl)
{
	pDC->FillSolidRect(rcControl,
		pGallery->GetParent()->GetPosition() == xtpBarPopup ?
		m_pPaintManager->GetXtremeColor(XPCOLOR_MENUBAR_FACE) :
		pGallery->GetSelected() && pGallery->GetEnabled() ? m_clrControlGallerySelected : m_clrControlGalleryNormal);

	if (pGallery->IsShowBorders())
	{
		pDC->Draw3dRect(rcControl, m_clrControlGalleryBorder, m_clrControlGalleryBorder);
	}

	if (pGallery->HasBottomSeparator())
	{
		pDC->FillSolidRect(rcControl.left, rcControl.bottom - 2, rcControl.Width(), 1, RGB(197, 197, 197));
	}
}

void CXTPControlGalleryOffice2007Theme::RefreshMetrics()
{
	CXTPControlGalleryPaintManager::RefreshMetrics();

	m_cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
	m_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	m_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	m_cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	m_cyPopupUp = 21;
	m_cyPopupDown = 19;

	CXTPOffice2007Theme* pPaintManager = (CXTPOffice2007Theme*)m_pPaintManager;

	m_clrControlGallerySelected = pPaintManager->GetImages()->GetImageColor(_T("Toolbar"), _T("ControlGallerySelected"));
	m_clrControlGalleryNormal = pPaintManager->GetImages()->GetImageColor(_T("Toolbar"), _T("ControlGalleryNormal"));
	m_clrControlGalleryBorder = pPaintManager->GetImages()->GetImageColor(_T("Toolbar"), _T("ControlGalleryBorder"));
}

void CXTPControlGalleryOffice2007Theme::DrawLabel(CDC* pDC, CXTPControlGalleryItem* pLabel, CRect rcItem)
{
	CXTPPaintManager* pPaintManager = m_pPaintManager;
	pDC->FillSolidRect(rcItem, pPaintManager->GetXtremeColor(XPCOLOR_LABEL));

	pDC->FillSolidRect(rcItem.left, rcItem.bottom - 1, rcItem.Width(), 1, RGB(197, 197, 197));

	CXTPFontDC fnt(pDC, pPaintManager->GetRegularBoldFont());

	CRect rcText(rcItem);
	rcText.DeflateRect(10, 0);
	pDC->SetTextColor(pPaintManager->GetXtremeColor(XPCOLOR_MENUBAR_TEXT));
	pDC->DrawText(pLabel->GetCaption(), rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void CXTPControlGalleryOffice2007Theme::DrawPopupScrollBar(CDC* pDC, CXTPControlGallery* pGallery)
{
	#define GETPARTSTATE3(ht, bEnabled) \
	(!bEnabled ? 4 : nPressetHt == ht ? 3 : nHotHt == ht ? 2 : nHotHt > 0 || nPressetHt > 0 ? 1 : 0)

	CXTPControlGallery::SCROLLBARTRACKINFO* pSBTrack = pGallery->GetScrollBarTrackInfo();
	CXTPControlGallery::SCROLLBARPOSINFO* pSBInfo = pGallery->GetScrollBarPosInfo();

	CRect rcControl = pGallery->GetRect();
	CRect rcScroll(rcControl.right - 15, rcControl.top, rcControl.right, rcControl.bottom);

	CRect rcScrollUp(rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.top + m_cyPopupUp);
	CRect rcScrollDown(rcScroll.left, rcScrollUp.bottom, rcScroll.right, rcScrollUp.bottom + m_cyPopupDown);
	CRect rcScrollPopup(rcScroll.left, rcScrollDown.bottom, rcScroll.right, rcScroll.bottom);

	CXTPOffice2007Theme* pPaintManager = (CXTPOffice2007Theme*)m_pPaintManager;

	CXTPOffice2007Image* pImage = pPaintManager->LoadImage(_T("CONTROLGALLERYUP"));
	ASSERT(pImage);
	if (!pImage)
		return;

	BOOL bControlEnabled = pGallery->GetEnabled();
	BOOL nPressetHt = pSBTrack ? pSBInfo->ht : -1;
	BOOL nHotHt = pSBTrack ? -1 : pSBInfo->ht;

	int nState = GETPARTSTATE3(XTP_HTSCROLLUP, (bControlEnabled && pGallery->IsScrollButtonEnabled(XTP_HTSCROLLUP)));
	pImage->DrawImage(pDC, rcScrollUp, pImage->GetSource(nState, 5), CRect(3, 3, 3, 3), 0xFF00FF);


	nState = GETPARTSTATE3(XTP_HTSCROLLDOWN, (bControlEnabled && pGallery->IsScrollButtonEnabled(XTP_HTSCROLLDOWN)));
	pImage = pPaintManager->LoadImage(_T("CONTROLGALLERYDOWN"));
	pImage->DrawImage(pDC, rcScrollDown, pImage->GetSource(nState, 5), CRect(3, 3, 3, 3), 0xFF00FF);

	nState = GETPARTSTATE3(XTP_HTSCROLLPOPUP, bControlEnabled);
	pImage = pPaintManager->LoadImage(_T("CONTROLGALLERYPOPUP"));
	pImage->DrawImage(pDC, rcScrollPopup, pImage->GetSource(nState, 5), CRect(3, 3, 3, 3), 0xFF00FF);
}

AFX_INLINE CRect OffsetSourceRect(CRect rc, int nState)
{
	rc.OffsetRect(0, (nState - 1) * rc.Height());
	return rc;
}

void CXTPControlGalleryOffice2007Theme::DrawScrollBar(CDC* pDC, CXTPScrollBase* pGallery)
{
	XTPScrollBarStyle barStyle = pGallery->GetScrollBarStyle();

	if (barStyle != xtpScrollStyleOffice2007Light && barStyle != xtpScrollStyleOffice2007Dark)
	{
		CXTPControlGalleryPaintManager::DrawScrollBar(pDC, pGallery);
		return;
	}

	BOOL bLight = (barStyle == xtpScrollStyleOffice2007Light);


	#define GETPARTSTATE2(ht) \
	(!bEnabled ? 0 : nPressetHt == ht ? 3 : nHotHt == ht ? 2 : nHotHt > 0 || nPressetHt > 0 ? 1 : 0)

	CXTPControlGallery::SCROLLBARTRACKINFO* pSBTrack = pGallery->GetScrollBarTrackInfo();
	CXTPControlGallery::SCROLLBARPOSINFO* pSBInfo = pGallery->GetScrollBarPosInfo();

	BOOL nPressetHt = pSBTrack ? pSBInfo->ht : -1;
	BOOL nHotHt = pSBTrack ? -1 : pSBInfo->ht;

	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);

	if (cWidth <= 0)
	{
		return;
	}

	BOOL bEnabled = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0) && pGallery->IsScrollBarEnabled();

	int nBtnTrackSize =   pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	int nBtnTrackPos = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		nBtnTrackPos = nBtnTrackSize = 0;

	CXTPOffice2007Theme* pPaintManager = (CXTPOffice2007Theme*)m_pPaintManager;

	CXTPOffice2007Image* pImageArrowGlyphs = pPaintManager->LoadImage(_T("CONTROLGALLERYSCROLLARROWGLYPHS"));
	ASSERT(pImageArrowGlyphs);
	if (!pImageArrowGlyphs)
		return;
	CRect rcArrowGripperSrc(0, 0, 9, 9);

	if (pSBInfo->fVert)
	{
		CXTPOffice2007Image* pImageBackground = pPaintManager->LoadImage(
			bLight ? _T("CONTROLGALLERYSCROLLVERTICALLIGHT") : _T("CONTROLGALLERYSCROLLVERTICALDARK"));
		ASSERT(pImageBackground);
		if (!pImageBackground)
			return;

		pImageBackground->DrawImage(pDC, pSBInfo->rc, pImageBackground->GetSource(0, 2), CRect(1, 0, 1, 0));


		CRect rcVScroll(pSBInfo->rc);
		rcVScroll.DeflateRect(1, 0);

		CRect rcArrowUp(rcVScroll.left, rcVScroll.top, rcVScroll.right, pSBInfo->pxUpArrow);
		CRect rcArrowDown(rcVScroll.left, pSBInfo->pxDownArrow, rcVScroll.right, rcVScroll.bottom);

		CXTPOffice2007Image* pImage =  pPaintManager->LoadImage(
			bLight ? _T("CONTROLGALLERYSCROLLARROWSVERTICALLIGHT") : _T("CONTROLGALLERYSCROLLARROWSVERTICALDARK"));
		ASSERT(pImage);
		if (!pImage)
			return;

		int nState = GETPARTSTATE2(XTP_HTSCROLLUP);
		if (nState != 0)
		{
			pImage->DrawImage(pDC, rcArrowUp, pImage->GetSource(nState, 4), CRect(3, 3, 3, 3));
		}

		CRect rcArrowUpGripper(CPoint((rcArrowUp.left + rcArrowUp.right - 9) / 2, (rcArrowUp.top + rcArrowUp.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowUpGripper, OffsetSourceRect(rcArrowGripperSrc, !bEnabled ? ABS_UPDISABLED : nState != 0 ? ABS_UPHOT : ABS_UPNORMAL), CRect(0, 0, 0, 0), RGB(255, 0, 255));

		nState = GETPARTSTATE2(XTP_HTSCROLLDOWN);
		if (nState != 0)
		{
			pImage->DrawImage(pDC, rcArrowDown, pImage->GetSource(nState, 4), CRect(3, 3, 3, 3));
		}

		CRect rcArrowDownGripper(CPoint((rcArrowDown.left + rcArrowDown.right - 9) / 2, (rcArrowDown.top + rcArrowDown.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowDownGripper, OffsetSourceRect(rcArrowGripperSrc, !bEnabled ? ABS_DOWNDISABLED : nState != 0 ? ABS_DOWNHOT : ABS_DOWNNORMAL), CRect(0, 0, 0, 0), RGB(255, 0, 255));


		CRect rcTrack(rcVScroll.left, rcArrowUp.bottom, rcVScroll.right, rcArrowDown.top);

		if (!rcTrack.IsRectEmpty())
		{
			CRect rcLowerTrack(rcTrack.left - 1, rcTrack.top, rcTrack.right + 1, rcTrack.top + nBtnTrackPos);
			CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right, rcLowerTrack.bottom + nBtnTrackSize);
			CRect rcUpperTrack(rcTrack.left - 1, rcBtnTrack.bottom, rcTrack.right + 1, rcTrack.bottom);

			if (!rcLowerTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLUPPAGE) == 3))
			{
				pImageBackground->DrawImage(pDC, rcLowerTrack,
					pImageBackground->GetSource(1, 2), CRect(1, 0, 1, 0));
			}

			if (!rcBtnTrack.IsRectEmpty())
			{
				nState = GETPARTSTATE2(XTP_HTSCROLLTHUMB);
				if (nState > 0) nState--;

				pImage =  pPaintManager->LoadImage(_T("CONTROLGALLERYSCROLLTHUMBVERTICAL"));
				ASSERT(pImage);
				if (!pImage)
					return;

				pImage->DrawImage(pDC, rcBtnTrack, pImage->GetSource(nState, 3), CRect(5, 5, 5, 5));


				if (rcBtnTrack.Height() > 10)
				{
					pImage =  pPaintManager->LoadImage(_T("CONTROLGALLERYSCROLLTHUMBGRIPPERVERTICAL"));
					CRect rcGripper(CPoint(rcBtnTrack.CenterPoint().x - 3, rcBtnTrack.CenterPoint().y - 4), CSize(8, 8));

					pImage->DrawImage(pDC, rcGripper, pImage->GetSource(nState, 3), CRect(0, 0, 0, 0));
				}
			}

			if (!rcUpperTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLDOWNPAGE) == 3))
			{
				pImageBackground->DrawImage(pDC, rcUpperTrack,
					pImageBackground->GetSource(1, 2), CRect(1, 0, 1, 0));
			}
		}
	}
	else
	{
		CXTPOffice2007Image* pImageBackground = pPaintManager->LoadImage(
			bLight ? _T("CONTROLGALLERYSCROLLHORIZONTALLIGHT") : _T("CONTROLGALLERYSCROLLHORIZONTALDARK"));
		if (!pImageBackground)
		{
			CXTPControlGalleryPaintManager::DrawScrollBar(pDC, pGallery);
			return;
		}

		pImageBackground->DrawImage(pDC, pSBInfo->rc, pImageBackground->GetSource(0, 2), CRect(0, 1, 0, 1));

		CRect rcHScroll(pSBInfo->rc);
		rcHScroll.DeflateRect(0, 1);

		CRect rcArrowLeft(rcHScroll.left, rcHScroll.top, pSBInfo->pxUpArrow, rcHScroll.bottom);
		CRect rcArrowRight(pSBInfo->pxDownArrow, rcHScroll.top, rcHScroll.right, rcHScroll.bottom);

		CXTPOffice2007Image* pImage =  pPaintManager->LoadImage(
			bLight ? _T("CONTROLGALLERYSCROLLARROWSHORIZONTALLIGHT") : _T("CONTROLGALLERYSCROLLARROWSHORIZONTALDARK"));
		ASSERT(pImage);
		if (!pImage)
			return;

		int nState = GETPARTSTATE2(XTP_HTSCROLLUP);
		if (nState != 0)
		{
			pImage->DrawImage(pDC, rcArrowLeft, pImage->GetSource(nState, 4), CRect(3, 3, 3, 3));
		}

		CRect rcArrowLeftGripper(CPoint((rcArrowLeft.left + rcArrowLeft.right - 9) / 2, (rcArrowLeft.top + rcArrowLeft.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowLeftGripper, OffsetSourceRect(rcArrowGripperSrc, !bEnabled ? ABS_LEFTDISABLED : nState != 0 ? ABS_LEFTHOT : ABS_LEFTNORMAL), CRect(0, 0, 0, 0), RGB(255, 0, 255));

		nState = GETPARTSTATE2(XTP_HTSCROLLDOWN);
		if (nState != 0)
		{
			pImage->DrawImage(pDC, rcArrowRight, pImage->GetSource(nState, 4), CRect(3, 3, 3, 3));
		}

		CRect rcArrowRightGripper(CPoint((rcArrowRight.left + rcArrowRight.right - 9) / 2, (rcArrowRight.top + rcArrowRight.bottom - 9) / 2), CSize(9, 9));
		pImageArrowGlyphs->DrawImage(pDC, rcArrowRightGripper, OffsetSourceRect(rcArrowGripperSrc, !bEnabled ? ABS_RIGHTDISABLED : nState != 0 ? ABS_RIGHTHOT : ABS_RIGHTNORMAL), CRect(0, 0, 0, 0), RGB(255, 0, 255));



		CRect rcTrack(rcArrowLeft.right, rcHScroll.top, rcArrowRight.left, rcHScroll.bottom);

		if (!rcTrack.IsRectEmpty())
		{
			CRect rcLowerTrack(rcTrack.left, rcTrack.top - 1, rcTrack.left + nBtnTrackPos, rcTrack.bottom + 1);
			CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top, rcLowerTrack.right + nBtnTrackSize, rcTrack.bottom);
			CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top - 1, rcTrack.right, rcTrack.bottom + 1);

			if (!rcLowerTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLUPPAGE) == 3))
			{
				pImageBackground->DrawImage(pDC, rcLowerTrack,
					pImageBackground->GetSource(1, 2), CRect(0, 1, 0, 1));
			}

			if (!rcBtnTrack.IsRectEmpty())
			{
				nState = GETPARTSTATE2(XTP_HTSCROLLTHUMB);
				if (nState > 0) nState--;

				pImage =  pPaintManager->LoadImage(_T("CONTROLGALLERYSCROLLTHUMBHORIZONTAL"));
				ASSERT(pImage);
				if (!pImage)
					return;

				pImage->DrawImage(pDC, rcBtnTrack, pImage->GetSource(nState, 3), CRect(5, 5, 5, 5));


				if (rcBtnTrack.Width() > 10)
				{
					pImage =  pPaintManager->LoadImage(_T("CONTROLGALLERYSCROLLTHUMBGRIPPERHORIZONTAL"));
					CRect rcGripper(CPoint(rcBtnTrack.CenterPoint().x - 3, rcBtnTrack.CenterPoint().y - 4), CSize(8, 8));

					pImage->DrawImage(pDC, rcGripper, pImage->GetSource(nState, 3), CRect(0, 0, 0, 0));
				}
			}

			if (!rcUpperTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLDOWNPAGE) == 3))
			{
				pImageBackground->DrawImage(pDC, rcUpperTrack,
					pImageBackground->GetSource(1, 2), CRect(0, 1, 0, 1));
			}
		}
	}
}
