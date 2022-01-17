// XTPDockingPanePaintManager.cpp : implementation of the CXTPDockingPanePaintManager class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPOffice2007Image.h"
#include "TabManager/XTPTabManager.h"
#include "TabManager/XTPTabPaintManager.h"

#include "XTPDockingPanePaintManager.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneTabbedContainer.h"
#include "XTPDockingPaneAutoHidePanel.h"
#include "XTPDockingPane.h"
#include "XTPDockingPaneMiniWnd.h"
#include "XTPDockingPaneSplitterContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CXTPDockingPaneCaptionButton::IsVisible()
{
	return m_pPane->IsCaptionButtonVisible(this);
}

AFX_INLINE void XTPDeflateRectDirection(CRect& rc, int left, int top, int right, int bottom, BOOL bVertical)
{
	if (bVertical)
	{
		rc.DeflateRect(top, left, bottom, right);
	}
	else
	{
		rc.DeflateRect(left, top, right, bottom);
	}
}

AFX_INLINE int XTPGetRectDirectionHeight(const CRect& rc, BOOL bVertical)
{
	return bVertical ? rc.Width() : rc.Height();
}

void XTPFillFramePartRect(CDC* pDC, CRect rc, CWnd* pWnd, CWnd* pParentFrame, const CXTPPaintManagerColorGradient& clrHeaderFace, BOOL bHoriz)
{
	COLORREF clrLight = clrHeaderFace.clrDark, clrDark = clrHeaderFace.clrLight;

	CWnd* pFrame = pParentFrame;

	if (XTPColorManager()->IsLowResolution())
	{
		pDC->FillSolidRect(rc, clrDark);
		return;
	}

	if (pFrame)
	{
		CRect rcFrame;
		pFrame->GetWindowRect(&rcFrame);
		pWnd->ScreenToClient(&rcFrame);

		if (bHoriz)
		{
			float fWidth = (float)max(rcFrame.Width(), GetSystemMetrics(SM_CXFULLSCREEN)/ 2);

			COLORREF clr1 = XTPDrawHelpers()->BlendColors(clrLight, clrDark, float(rc.left - rcFrame.left) / fWidth);
			COLORREF clr2 = XTPDrawHelpers()->BlendColors(clrLight, clrDark, float(rc.right - rcFrame.left) / fWidth);

			XTPDrawHelpers()->GradientFill(pDC, rc, clr1, clr2, TRUE);
		}
		else
		{
			float fHeight = (float)rcFrame.Height();

			COLORREF clr1 = XTPDrawHelpers()->BlendColors(clrLight, clrDark, float(rc.top - rcFrame.top) / fHeight);
			COLORREF clr2 = XTPDrawHelpers()->BlendColors(clrLight, clrDark, float(rc.bottom - rcFrame.top) / fHeight);

			XTPDrawHelpers()->GradientFill(pDC, rc, clr1, clr2, FALSE);
		}
	}
}

void AFX_CDECL CXTPDockingPaneCaptionButton::DrawPinnButton(CDC* pDC, CPoint pt, BOOL bPinned)
{
	pt.y--;

	if (bPinned)
	{
		pDC->MoveTo(pt.x - 4, pt.y); pDC->LineTo(pt.x - 1, pt.y);
		pDC->MoveTo(pt.x - 1, pt.y - 3); pDC->LineTo(pt.x - 1, pt.y + 4);
		pDC->MoveTo(pt.x - 1, pt.y + 2); pDC->LineTo(pt.x + 3, pt.y + 2);
		pDC->LineTo(pt.x + 3, pt.y - 2); pDC->LineTo(pt.x - 1, pt.y - 2);
		pDC->MoveTo(pt.x - 1, pt.y + 1); pDC->LineTo(pt.x + 3, pt.y + 1);
	}
	else
	{
		pt.y += 2;
		pDC->MoveTo(pt.x - 3, pt.y - 1); pDC->LineTo(pt.x + 4, pt.y - 1);
		pDC->MoveTo(pt.x, pt.y - 1); pDC->LineTo(pt.x, pt.y + 3);
		pDC->MoveTo(pt.x - 2, pt.y - 1); pDC->LineTo(pt.x - 2, pt.y - 6);
		pDC->LineTo(pt.x + 2, pt.y - 6); pDC->LineTo(pt.x + 2, pt.y - 1);
		pDC->MoveTo(pt.x + 1, pt.y - 1); pDC->LineTo(pt.x + 1, pt.y - 6);
	}
}

void AFX_CDECL CXTPDockingPaneCaptionButton::DrawMaximizeRestoreButton(CDC* pDC, CPoint pt, BOOL bMaximize, COLORREF clr)
{
	if (bMaximize)
	{
		pDC->Draw3dRect(pt.x - 5, pt.y - 5, 9, 9, clr, clr);
		pDC->FillSolidRect(pt.x - 5, pt.y - 5, 9, 2, clr);
	}
	else
	{
		pDC->Draw3dRect(pt.x - 4, pt.y - 2, 6, 6, clr, clr);
		pDC->FillSolidRect(pt.x - 4, pt.y - 2, 6, 2, clr);

		pDC->FillSolidRect(pt.x - 2, pt.y - 5, 6, 2, clr);
		pDC->SetPixel(pt.x - 2, pt.y - 3, clr);
		pDC->FillSolidRect(pt.x + 3, pt.y - 3, 1, 4, clr);
		pDC->SetPixel(pt.x + 2, pt.y, clr);
	}
}

void CXTPDockingPaneCaptionButton::InvalidateRect()
{
	m_pPane->RedrawPane();
}

BOOL CXTPDockingPaneCaptionButton::IsEnabled() const
{
	CXTPDockingPane* pPane = CXTPDockingPanePaintManager::GetSelectedPane(GetPane());
	if (pPane && ((pPane->GetEnabled() & xtpPaneEnableActions) == 0))
		return FALSE;

	return TRUE;
}

DWORD CXTPDockingPaneCaptionButton::Click (CWnd* pWnd, CPoint pt, BOOL bClient)
{
	if ((::GetCapture() != NULL))
		return FALSE;

	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());

	BOOL  bClick = FALSE;
	CRect rcBtn(m_rcButton);

	while (CWnd::GetCapture() == pWnd)
	{

		BOOL bNewDown = ::PtInRect (rcBtn, pt) != 0;
		if (bNewDown != m_bPressed)
		{
			m_bPressed = bNewDown;

			InvalidateRect();
		}

		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		switch (msg.message)
		{
		case WM_NCMOUSEMOVE:
			break;
		case WM_MOUSEMOVE:
			pt = CPoint((short signed)LOWORD(msg.lParam), (short signed)HIWORD(msg.lParam));
			if (!bClient)
			{
				pWnd->ClientToScreen (&pt);
				CXTPDrawHelpers::ScreenToWindow(pWnd, &pt);
			}
			break;

		case WM_LBUTTONUP:
			bClick = m_bPressed;
			goto ExitLoop;

		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_CANCELMODE:
		case WM_RBUTTONDOWN:
			goto ExitLoop;

		default:
			DispatchMessage (&msg);
			break;
		}
	}
ExitLoop:
	ReleaseCapture();

	m_bPressed = FALSE;
	CheckForMouseOver (pt);

	InvalidateRect();

	return bClick;
}

void CXTPDockingPaneCaptionButton::SetRect (CRect rc)
{
	ASSERT(m_pPane);
	if (!m_pPane)
		return;

	if (rc != m_rcButton)
	{
		m_rcButton = rc;
		m_pPane->OnCaptionButtonRectChanged(this);
	}
}

BOOL CXTPDockingPaneCaptionButton::CheckForMouseOver (CPoint pt)
{
	if (!PtInRect (pt) && m_bSelected)
	{
		m_bSelected = FALSE;
		InvalidateRect();
	}

	if (!m_bSelected && PtInRect (pt) && IsEnabled())
	{
		TRACKMOUSEEVENT tme =
		{
			sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_pPane->GetPaneHwnd()
		};
		_TrackMouseEvent(&tme);

		m_bSelected = TRUE;

		InvalidateRect();
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPanePaintManager


CXTPDockingPanePaintManager::CXTPDockingPanePaintManager()
: m_bUseOfficeFont(FALSE)
, m_strOfficeFont(_T("Tahoma"))
{
	m_pTabPaintManager = new CXTPTabPaintManager();
	m_pTabPaintManager->SetLayout(xtpTabLayoutSizeToFit);
	m_pTabPaintManager->SetPosition(xtpTabPositionBottom);
	m_pTabPaintManager->m_clientFrame = xtpTabFrameSingleLine;
	m_pTabPaintManager->m_bBoldSelected = FALSE;
	m_pTabPaintManager->EnableToolTips();

	m_pPanelPaintManager = new CXTPTabPaintManager();
	m_pPanelPaintManager->SetLayout(xtpTabLayoutCompressed);
	m_pPanelPaintManager->m_clientFrame = xtpTabFrameNone;
	m_pPanelPaintManager->m_bBoldSelected = FALSE;
	m_pPanelPaintManager->m_bInvertGradient = TRUE;
	m_pPanelPaintManager->m_bFillBackground = FALSE;

	m_bDrawSingleTab = FALSE;
	m_nSplitterSize = 4;
	m_bDrawCaptionIcon = FALSE;

	m_rcCaptionMargin.SetRect(0, 2, 0, 1);
	m_nCaptionFontGap = 5;

	m_bShowCaption = TRUE;
	m_bUseStandardFont = TRUE;

	m_bHighlightActiveCaption = TRUE;
	m_systemTheme = xtpSystemThemeDefault;
	m_bUseBoldCaptionFont = TRUE;
	m_dwSplitterStyle = xtpPaneSplitterFlat;

	m_nCaptionButtonStyle = xtpPaneCaptionButtonDefault;
	m_nTitleHeight = 0;
	m_nTabsHeight = 0;
	m_bLunaTheme = FALSE;

}

DWORD CXTPDockingPanePaintManager::GetSplitterStyle() const
{
	return m_dwSplitterStyle;
}

void CXTPDockingPanePaintManager::SetSplitterStyle(DWORD dwStyle)
{
	m_dwSplitterStyle = dwStyle;

	m_nSplitterSize = dwStyle == xtpPaneSplitterFlat ? 4 : 7;

}

XTPCurrentSystemTheme CXTPDockingPanePaintManager::GetCurrentSystemTheme() const
{
	if (m_systemTheme == xtpSystemThemeDefault)
		return XTPColorManager()->IsLunaColorsDisabled() ? xtpSystemThemeUnknown :
			XTPColorManager()->GetCurrentSystemTheme();

	if (m_systemTheme == xtpSystemThemeAuto)
		return XTPColorManager()->GetWinThemeWrapperTheme();

	return m_systemTheme;
}

void CXTPDockingPanePaintManager::SetLunaTheme(XTPCurrentSystemTheme systemTheme)
{
	m_systemTheme = systemTheme;
	RefreshMetrics();

}

COLORREF CXTPDockingPanePaintManager::GetXtremeColor(UINT nIndex)
{
	if (nIndex > XPCOLOR_LAST) return nIndex;
	return m_arrColor[nIndex];
}

void CXTPDockingPanePaintManager::SetColors(int cElements, CONST INT* lpaElements, CONST COLORREF* lpaRgbValues)
{
	for (int i = 0; i < cElements; i++)
		m_arrColor[lpaElements[i]] = lpaRgbValues[i];
}



CXTPDockingPanePaintManager::~CXTPDockingPanePaintManager()
{
	m_pTabPaintManager->InternalRelease();
	m_pPanelPaintManager->InternalRelease();
}

CXTPTabPaintManager* CXTPDockingPanePaintManager::SetTabPaintManager(CXTPTabPaintManager* pManager)
{
	m_pTabPaintManager->InternalRelease();
	m_pTabPaintManager = pManager;

	m_pTabPaintManager->SetLayout(xtpTabLayoutSizeToFit);
	m_pTabPaintManager->SetPosition(xtpTabPositionBottom);
	m_pTabPaintManager->m_bBoldSelected = FALSE;
	m_pTabPaintManager->EnableToolTips();

	pManager->RefreshMetrics();
	return pManager;
}

CXTPTabPaintManager* CXTPDockingPanePaintManager::SetPanelPaintManager(CXTPTabPaintManager* pManager)
{
	delete m_pPanelPaintManager;
	m_pPanelPaintManager = pManager;

	m_pPanelPaintManager->SetLayout(xtpTabLayoutCompressed);
	m_pPanelPaintManager->m_clientFrame = xtpTabFrameNone;
	m_pPanelPaintManager->m_bBoldSelected = FALSE;
	m_pPanelPaintManager->m_bInvertGradient = TRUE;
	m_pPanelPaintManager->m_bFillBackground = FALSE;


	pManager->RefreshMetrics();
	return pManager;
}


void CXTPDockingPanePaintManager::UseOfficeFont(BOOL bUseOfficeFont)
{
	m_bUseOfficeFont = bUseOfficeFont;
	RefreshMetrics();
}

void CXTPDockingPanePaintManager::SetCaptionFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardFont /*= FALSE*/)
{
	m_bUseStandardFont = bUseStandardFont;

	if (!pLogFont)
		return;

	BOOL bUseSystemFont = (pLogFont->lfCharSet > SYMBOL_CHARSET);
	if (m_bUseOfficeFont && !bUseSystemFont && bUseStandardFont && XTPDrawHelpers()->FontExists(_T("Tahoma")))
	{
		STRCPY_S(pLogFont->lfFaceName, LF_FACESIZE, _T("Tahoma"));
	}

	m_fntTitle.DeleteObject();
	VERIFY(m_fntTitle.CreateFontIndirect(pLogFont));

	pLogFont->lfOrientation = 900;
	pLogFont->lfEscapement = 2700;
	pLogFont->lfHeight = pLogFont->lfHeight < 0 ? __min(-11, pLogFont->lfHeight) : pLogFont->lfHeight;
	STRCPY_S(pLogFont->lfFaceName, LF_FACESIZE, CXTPDrawHelpers::GetVerticalFontName(m_bUseOfficeFont));

	m_fntTitleVertical.DeleteObject();
	m_fntTitleVertical.CreateFontIndirect(pLogFont);


	CWindowDC dc(CWnd::GetDesktopWindow());
	CXTPFontDC font(&dc, &m_fntTitle);
	m_nTitleHeight = max(13, dc.GetTextExtent(_T(" "), 1).cy) + m_nCaptionFontGap;

}

void CXTPDockingPanePaintManager::RefreshMetrics()
{
	RefreshXtremeColors();

	CXTPColorManager* pColorManager = XTPColorManager();

	for (int i = 0; i <= XPCOLOR_LAST; i++)
		m_arrColor[i] = pColorManager->GetColor(i);

	m_clrSplitterGripper.SetStandardValue(m_arrColor[COLOR_3DSHADOW]);
	m_clrSplitter.SetStandardValue(m_arrColor[COLOR_3DFACE]);
	m_clrSplitterGradient.SetStandardValue(GetSysColor(COLOR_3DFACE), XTPColorManager()->LightColor(GetSysColor(COLOR_3DFACE), GetSysColor(COLOR_WINDOW), 0xcd));

	if (m_bUseStandardFont)
	{
		NONCLIENTMETRICS ncm =
		{
			sizeof(NONCLIENTMETRICS)
		};
		VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
		if (!m_bUseBoldCaptionFont)
			ncm.lfSmCaptionFont.lfWeight = FW_NORMAL;

		SetCaptionFontIndirect(&ncm.lfSmCaptionFont, TRUE);
	}

	m_pTabPaintManager->RefreshMetrics();
	m_pPanelPaintManager->RefreshMetrics();

	CWindowDC dc(CWnd::GetDesktopWindow());
	CXTPFontDC font(&dc, m_pTabPaintManager->GetFont(FALSE));
	m_nTabsHeight = max(24, dc.GetTextExtent(_T(" "), 1).cy + 10);

	m_themeExplorer.OpenTheme(0, L"EXPLORERBAR");
	m_themeButton.OpenTheme(0, L"BUTTON");
	m_themeToolbar.OpenThemeData(0, L"TOOLBAR");
	m_themeWindow.OpenThemeData(0, L"WINDOW");
	m_bLunaTheme = FALSE;
}

void CXTPDockingPanePaintManager::Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen)
{
	CXTPPenDC pen(*pDC, GetXtremeColor(nPen));
	pDC->MoveTo(x0, y0);
	pDC->LineTo(x1, y1);
}

void CXTPDockingPanePaintManager::Line(CDC* pDC, int x0, int y0, int x1, int y1)
{
	pDC->MoveTo(x0, y0);
	pDC->LineTo(x1, y1);
}


void CXTPDockingPanePaintManager::Rectangle(CDC* pDC, CRect rc, int nBrush, int nPen)
{
	pDC->FillSolidRect(rc, GetXtremeColor(nBrush));
	pDC->Draw3dRect(rc, GetXtremeColor(nPen), GetXtremeColor(nPen));
}

void CXTPDockingPanePaintManager::Pixel(CDC* pDC, int x, int y, int nPen)
{
	pDC->SetPixel(x, y, GetXtremeColor(nPen));
}

CRect CXTPDockingPanePaintManager::GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* /*pPane*/)
{
	return CRect(0, 0, 0, 0);
}


void CXTPDockingPanePaintManager::DrawCaptionButtons(CDC* pDC, CXTPDockingPaneCaptionButtons* pButtons, CRect& rcCaption, COLORREF clr, int nButtonSize, int nButtonGap, BOOL bVertical)
{
	CRect rcButton(
		bVertical ? CPoint(rcCaption.CenterPoint().x - nButtonSize / 2, rcCaption.bottom - nButtonSize) :
			CPoint(rcCaption.right - nButtonSize , rcCaption.CenterPoint().y - nButtonSize / 2),
		CSize(nButtonSize, nButtonSize));

	for (int i = 0; i < pButtons->GetSize(); i++)
	{
		CXTPDockingPaneCaptionButton* pButton = pButtons->GetAt(i);

		if (i == 0)
		{
			CXTPDockingPane* pPane = GetSelectedPane(pButton->GetPane());

			if (pPane && ((pPane->GetEnabled() & xtpPaneEnableActions) == 0))
				clr = GetXtremeColor(COLOR_GRAYTEXT);
		}

		if (pButton->IsVisible())
		{


			pButton->SetRect(rcButton);
			DrawCaptionButton(pDC, pButton, clr);

			if (bVertical)
			{
				rcButton.OffsetRect(0, - nButtonSize - nButtonGap);
			}
			else
			{
				rcButton.OffsetRect(- nButtonSize - nButtonGap, 0);
			}
		}
		else
		{
			pButton->SetRect(CXTPEmptyRect());
		}
	}
	if (bVertical)
	{
		rcCaption.bottom = rcButton.bottom;
	}
	else
	{
		rcCaption.right = rcButton.right;
	}
}

void CXTPDockingPanePaintManager::DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF clrButton)
{
	CRect rc = pButton->GetRect();
	CPoint pt = rc.CenterPoint();

	XTPDockingPaneCaptionButtonStyle nCaptionButtonStyle = m_nCaptionButtonStyle;

	if (nCaptionButtonStyle == xtpPaneCaptionButtonThemedToolBar)
	{
		if (!m_themeToolbar.IsAppThemed())
		{
			nCaptionButtonStyle = xtpPaneCaptionButtonOffice;
		}
		else
		{
			if (pButton->IsPressed())
			{
				m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_BUTTON, TS_PRESSED, &rc, 0);
			}
			else if (pButton->IsSelected())
			{
				m_themeToolbar.DrawThemeBackground(pDC->GetSafeHdc(), TP_BUTTON, TS_HOT, &rc, 0);
			}
		}
	}

	if (nCaptionButtonStyle == xtpPaneCaptionButtonThemedToolWindow)
	{
		if (!m_themeWindow.IsAppThemed() || pButton->GetID() != XTP_IDS_DOCKINGPANE_CLOSE )
		{
			nCaptionButtonStyle = xtpPaneCaptionButtonOffice;
		}
		else
		{
			BOOL bActive = TRUE;
			if (pButton->GetPane()->GetType() == xtpPaneTypeTabbedContainer)
				bActive = ((CXTPDockingPaneTabbedContainer*)pButton->GetPane())->IsActive();
			if (pButton->GetPane()->GetType() == xtpPaneTypeMiniWnd)
				bActive = ((CXTPDockingPaneMiniWnd*)pButton->GetPane())->IsActive();

			int nState = pButton->IsPressed() ? CBS_PUSHED: pButton->IsSelected() ? CBS_HOT : !bActive ? 5 : CBS_NORMAL;
			m_themeWindow.DrawThemeBackground(pDC->GetSafeHdc(), WP_SMALLCLOSEBUTTON, nState, &rc, 0);
			return;
		}
	}

	if (nCaptionButtonStyle == xtpPaneCaptionButtonThemedExplorerBar)
	{
		if (!m_themeExplorer.IsAppThemed())
		{
			nCaptionButtonStyle = xtpPaneCaptionButtonOffice;
		}
		else
		{
			if (pButton->GetID() == XTP_IDS_DOCKINGPANE_CLOSE || pButton->GetID() == XTP_IDS_DOCKINGPANE_AUTOHIDE)
			{

				BOOL bActive = TRUE;
				if (pButton->GetPane()->GetType() == xtpPaneTypeTabbedContainer)
					bActive = ((CXTPDockingPaneTabbedContainer*)pButton->GetPane())->IsActive();
				if (pButton->GetPane()->GetType() == xtpPaneTypeMiniWnd)
					bActive = ((CXTPDockingPaneMiniWnd*)pButton->GetPane())->IsActive();

				bActive = m_bHighlightActiveCaption && bActive;

				if (pButton->GetID() == XTP_IDS_DOCKINGPANE_CLOSE)
				{
					int nState = pButton->IsPressed() ? EBHC_PRESSED: bActive || pButton->IsSelected() ? EBHC_HOT : EBHC_NORMAL;
					if (SUCCEEDED(m_themeExplorer.DrawThemeBackground(*pDC, EBP_HEADERCLOSE, nState, pButton->GetRect(), 0)))
						return;
				}
				else
				{
					int nState = pButton->IsPressed() ? EBHP_PRESSED: pButton->IsSelected() ? EBHP_HOT : EBHP_NORMAL;
					if (SUCCEEDED(m_themeExplorer.DrawThemeBackground(*pDC, EBP_HEADERPIN, (pButton->GetState() & xtpPanePinPushed ? 0 : 3) + nState, pButton->GetRect(), 0)))
						return;
				}
			}

			nCaptionButtonStyle = xtpPaneCaptionButtonThemedButton;
		}
	}

	if (nCaptionButtonStyle == xtpPaneCaptionButtonThemedButton)
	{
		if (!m_themeButton.IsAppThemed())
		{
			nCaptionButtonStyle = xtpPaneCaptionButtonOffice;
		}
		else
		{
			int nState = pButton->IsPressed() ? PBS_PRESSED: pButton->IsSelected() ? PBS_HOT: PBS_NORMAL;
			if (nState != PBS_NORMAL)
			{
				m_themeButton.DrawThemeBackground(*pDC, BP_PUSHBUTTON, nState, pButton->GetRect(), 0);
				m_themeButton.GetThemeColor(BP_PUSHBUTTON, nState, TMT_TEXTCOLOR, &clrButton);
			}
		}
	}

	if (nCaptionButtonStyle == xtpPaneCaptionButtonDefault)
	{
		pDC->DrawFrameControl(&rc, DFC_BUTTON, DFCS_ADJUSTRECT | DFCS_BUTTONPUSH | (pButton->IsPressed() ? DFCS_PUSHED : 0));

		if (pButton->IsPressed()) pt.Offset(1, 1);
	}

	if (nCaptionButtonStyle == xtpPaneCaptionButtonOffice)
	{
		if (pButton->IsPressed())
		{
			pDC->Draw3dRect(rc, GetXtremeColor(COLOR_BTNTEXT), GetXtremeColor(COLOR_BTNHILIGHT));
			pt.Offset(1, 1);
		}
		else if (pButton->IsSelected())
			pDC->Draw3dRect(rc, GetXtremeColor(COLOR_BTNHILIGHT), GetXtremeColor(COLOR_BTNTEXT));
	}

	if (nCaptionButtonStyle == xtpPaneCaptionButtonOffice2003)
	{
		if (pButton->IsSelected())
		{
			if (!m_bLunaTheme)
			{
				Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER);
				clrButton = GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT);
			}
			else
			{
				XTPDrawHelpers()->GradientFill(pDC, rc, RGB(255, 242, 200), RGB(255, 212, 151), FALSE);
				pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
				clrButton = 0;
			}
		}
		if (pButton->IsPressed())
		{
			if (!m_bLunaTheme)
			{
				Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_BORDER);
				clrButton = GetXtremeColor(XPCOLOR_PUSHED_TEXT);
			}
			else
			{
				XTPDrawHelpers()->GradientFill(pDC, rc, RGB(254, 142, 75), RGB(255, 207, 139), FALSE);
				pDC->Draw3dRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER), GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER));
				clrButton = 0;
			}
		}
	}

	DrawCaptionButtonEntry(pDC, pButton, pt, clrButton);
}



void CXTPDockingPanePaintManager::DrawPane(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	dc.FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));
	dc.SetBkMode(TRANSPARENT);

	CRect rcCaption(rc);

	if (pPane->IsTabsVisible())
	{
		AdjustCaptionRect(pPane, rc);
		m_pTabPaintManager->DrawTabControl(pPane, &dc, rc);
	}

	if (pPane->IsTitleVisible())
	{
		DrawCaption(dc, pPane, rcCaption);
	}

}


void CXTPDockingPanePaintManager::AdjustCaptionRect(const CXTPDockingPaneTabbedContainer* pPane, CRect& rc)
{
	if (pPane->IsTitleVisible())
	{
		if (pPane->IsCaptionVertical())
		{
			rc.left += m_nTitleHeight + m_rcCaptionMargin.top + m_rcCaptionMargin.bottom;
		}
		else
		{
			rc.top += m_nTitleHeight + m_rcCaptionMargin.top + m_rcCaptionMargin.bottom;
		}
	}
}

void CXTPDockingPanePaintManager::AdjustClientRect(CXTPDockingPaneTabbedContainer* pPane, CRect& rect, BOOL bApply)
{
	CPoint pt = rect.TopLeft();
	AdjustCaptionRect(pPane, rect);


	if (pPane->IsTabsVisible())
	{
		if (bApply)
		{
			CClientDC dc(pPane);

			CRect rcTabs(rect);
			rcTabs.OffsetRect(-pt);

			m_pTabPaintManager->RepositionTabControl(pPane, &dc, rcTabs);
		}
		m_pTabPaintManager->AdjustClientRect(pPane, rect);
	}
}

void CXTPDockingPanePaintManager::DrawSplitter(CDC& dc, CXTPDockingPaneSplitterWnd* pSplitter)
{
	CXTPClientRect rc((CWnd*)pSplitter);

	if (m_dwSplitterStyle & xtpPaneSplitter3D)
	{
		dc.FillSolidRect(rc, m_clrSplitter);

		if (pSplitter->IsHorizontal())
		{
			dc.FillSolidRect(0, 0, rc.Width(), 1, GetXtremeColor(COLOR_3DHIGHLIGHT));
			dc.FillSolidRect(0, rc.bottom - 2, rc.Width(), 1, GetXtremeColor(XPCOLOR_3DSHADOW));
			dc.FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, GetXtremeColor(COLOR_BTNTEXT));
		}
		else
		{
			dc.FillSolidRect(0, 0, 1, rc.Height(), GetXtremeColor(COLOR_3DHIGHLIGHT));
			dc.FillSolidRect(rc.right - 2, 0, 1, rc.Height(), GetXtremeColor(XPCOLOR_3DSHADOW));
			dc.FillSolidRect(rc.right - 1, 0, 1, rc.Height(), GetXtremeColor(COLOR_BTNTEXT));
		}
	}
	else if (m_dwSplitterStyle & xtpPaneSplitterSoft3D)
	{
		int nCenter = m_nSplitterSize / 2;

		if (pSplitter->IsHorizontal())
		{
			dc.FillSolidRect(0, 0, rc.Width(), 1, GetXtremeColor(COLOR_3DDKSHADOW));
			dc.FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, GetXtremeColor(COLOR_3DDKSHADOW));

			dc.FillSolidRect(0, 1, rc.Width(), nCenter - 1, GetXtremeColor(COLOR_WINDOW));
			dc.FillSolidRect(0, nCenter, rc.Width(), rc.Height() - nCenter - 1, m_clrSplitter);
		}
		else
		{
			dc.FillSolidRect(0, 0, 1, rc.Height(), GetXtremeColor(COLOR_3DDKSHADOW));
			dc.FillSolidRect(rc.right - 1, 0, 1, rc.Height(), GetXtremeColor(COLOR_3DDKSHADOW));

			dc.FillSolidRect(1, 0, nCenter - 1, rc.Height(), GetXtremeColor(COLOR_WINDOW));
			dc.FillSolidRect(nCenter, 0, rc.Width() - nCenter - 1, rc.Height(), m_clrSplitter);
		}
	}
	else if (m_dwSplitterStyle & xtpPaneSplitterGradient)
	{
		XTPFillFramePartRect(&dc, rc, (CWnd*)pSplitter, pSplitter->GetParent(), m_clrSplitterGradient, TRUE);
	}
	else if (m_dwSplitterStyle & xtpPaneSplitterGradientVert)
	{
		XTPFillFramePartRect(&dc, rc, (CWnd*)pSplitter, pSplitter->GetParent(), m_clrSplitterGradient, FALSE);
	}
	else
	{
		dc.FillSolidRect(rc, m_clrSplitter);
	}

	if (m_dwSplitterStyle & xtpPaneSplitterGripperOfficeXP)
	{
		if (pSplitter->IsHorizontal())
		{
			dc.FillSolidRect(rc.CenterPoint().x, 0 + 2, 11, 1, m_clrSplitterGripper);
			dc.FillSolidRect(rc.CenterPoint().x, 0 + 4, 11, 1, m_clrSplitterGripper);
		}
		else
		{
			dc.FillSolidRect(2, rc.CenterPoint().y, 1, 11, m_clrSplitterGripper);
			dc.FillSolidRect(4, rc.CenterPoint().y, 1, 11, m_clrSplitterGripper);
		}
	}
	if (m_dwSplitterStyle & xtpPaneSplitterGripperOffice2003)
	{
		if (pSplitter->IsHorizontal())
		{
			int nLeft = rc.CenterPoint().x - min(32, rc.Width() /2 - 10);
			int nRight = rc.CenterPoint().x + min(32, rc.Width() /2 - 10);
			int y = rc.CenterPoint().y - 1;

			for (int x = nLeft; x < nRight; x += 4)
			{
				dc.FillSolidRect(CRect(x + 1, y + 1, x + 3, y + 3), GetXtremeColor(COLOR_BTNHIGHLIGHT));
				dc.FillSolidRect(CRect(x, y, x + 2, y + 2), m_clrSplitterGripper);
			}
		}
		else
		{
			int nTop = rc.CenterPoint().y - min(32, rc.Height() /2 - 10);
			int nBottom = rc.CenterPoint().y + min(32, rc.Height() /2 - 10);
			int x = rc.CenterPoint().x - 1;

			for (int y = nTop; y < nBottom; y += 4)
			{
				dc.FillSolidRect(CRect(x + 1, y + 1, x + 3, y + 3), GetXtremeColor(COLOR_BTNHIGHLIGHT));
				dc.FillSolidRect(CRect(x, y, x + 2, y + 2), m_clrSplitterGripper);
			}
		}

	}
}


///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneOfficeTheme

class CXTPDockingPaneOfficeTheme::COfficePanelColorSet :
	public CXTPTabPaintManager::CColorSetVisualStudio
{
public:
	virtual void RefreshMetrics();
};

void CXTPDockingPaneOfficeTheme::COfficePanelColorSet::RefreshMetrics()
{
	CColorSetVisualStudio::RefreshMetrics();

	if (m_pPaintManager->GetAppearance() == xtpTabAppearanceFlat)
	{
		m_clrSelectedText.SetStandardValue(GetSysColor(COLOR_3DDKSHADOW));
		m_clrNormalText.SetStandardValue(GetSysColor(COLOR_3DDKSHADOW));
	}
}


CXTPDockingPaneOfficeTheme::CXTPDockingPaneOfficeTheme()
{
	m_pPanelPaintManager->SetAppearance(xtpTabAppearanceFlat);
	m_pPanelPaintManager->SetColorSet(new COfficePanelColorSet);
	m_pTabPaintManager->SetAppearance(xtpTabAppearanceVisualStudio);
	m_bGradientCaption = FALSE;
	m_bUseBoldCaptionFont = FALSE;
}

void CXTPDockingPaneOfficeTheme::RefreshMetrics()
{
	CXTPDockingPaneGripperedTheme::RefreshMetrics();

	m_clrActiveCaption.SetStandardValue(GetXtremeColor(COLOR_ACTIVECAPTION), GetXtremeColor(27));
	m_clrNormalCaption.SetStandardValue(GetXtremeColor(COLOR_INACTIVECAPTION), GetSysColor(28));

	m_clrNormalCaptionText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));
	m_clrActiveCaptionText.SetStandardValue(GetXtremeColor(COLOR_CAPTIONTEXT));

	if (XTPSystemVersion()->IsWin95())
	{
		// Windows 95
		m_clrNormalCaption.SetStandardValue(GetXtremeColor(COLOR_INACTIVECAPTION));
		m_clrActiveCaption.SetStandardValue(GetXtremeColor(COLOR_ACTIVECAPTION));
	}
}

COLORREF CXTPDockingPaneOfficeTheme::FillCaptionPart(CDC& dc, CXTPDockingPaneBase* /*pPane*/, CRect rcCaption, BOOL bActive, BOOL /*bVertical*/)
{
	if (m_bGradientCaption)
	{
		XTPDrawHelpers()->GradientFill(&dc, rcCaption, bActive ? m_clrActiveCaption : m_clrNormalCaption, TRUE);
	}
	else
	{
		if (bActive)
		{
			dc.FillSolidRect(rcCaption, m_clrActiveCaption.clrLight);
		}
		else
		{
			CXTPPenDC pen(dc, GetXtremeColor(XPCOLOR_3DSHADOW));
			dc.MoveTo(rcCaption.left + 1, rcCaption.top); dc.LineTo(rcCaption.right - 1 , rcCaption.top);
			dc.MoveTo(rcCaption.left, rcCaption.top + 1); dc.LineTo(rcCaption.left, rcCaption.bottom - 1);
			dc.MoveTo(rcCaption.left + 1, rcCaption.bottom - 1); dc.LineTo(rcCaption.right - 1 , rcCaption.bottom - 1);
			dc.MoveTo(rcCaption.right -1 , rcCaption.top + 1); dc.LineTo(rcCaption.right - 1, rcCaption.bottom - 1);
		}
	}

	return  GetXtremeColor(bActive ? COLOR_CAPTIONTEXT : COLOR_BTNTEXT);
}

void CXTPDockingPaneOfficeTheme::DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical)
{
	COLORREF clrButton = FillCaptionPart(dc, pPane, rcCaption, bActive, bVertical);

	COLORREF clrText = bActive ? m_clrActiveCaptionText : m_clrNormalCaptionText;

	XTPDeflateRectDirection(rcCaption, 0, 0, 2, 0, bVertical);
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, clrButton, 14, 2, bVertical);

	// Title text
	dc.SetTextColor(IsCaptionEnabled(pPane) ? clrText : GetXtremeColor(COLOR_GRAYTEXT));
	XTPDeflateRectDirection(rcCaption, 4, 1, 0, 0, bVertical);

	DrawCaptionIcon(dc, pPane, rcCaption, bVertical);
	DrawCaptionText(dc, strTitle, rcCaption, bVertical);
}


///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneDefaultTheme

CXTPDockingPaneDefaultTheme::CXTPDockingPaneDefaultTheme()
{
	m_pPanelPaintManager->SetAppearance(xtpTabAppearancePropertyPage);
	m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPage);

	m_nCaptionButtonStyle = xtpPaneCaptionButtonDefault;
}

void CXTPDockingPaneDefaultTheme::DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc)
{
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DFACE), 0);
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_3DFACE));

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);

	CXTPBufferDC dcCache(dc, rcCaption);
	dcCache.SetBkMode(TRANSPARENT);

	dcCache.FillSolidRect(rcCaption, GetXtremeColor(COLOR_3DFACE));

	rcCaption.DeflateRect(1, 2, 1, 2);

	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	CString strTitle;
	pPane->GetWindowText(strTitle);

	DrawCaptionPart(dcCache, pPane, rcCaption, strTitle, bActive, FALSE);
}

void CXTPDockingPaneDefaultTheme::DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical)
{
	dc.FillSolidRect(rcCaption, GetXtremeColor(bActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));

	XTPDeflateRectDirection(rcCaption, 0, 0, 2, 0, bVertical);
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, GetXtremeColor(COLOR_BTNTEXT), 14, 0, bVertical);

	dc.SetTextColor(IsCaptionEnabled(pPane) ? (GetXtremeColor(bActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT))
		: GetXtremeColor(COLOR_GRAYTEXT));
	XTPDeflateRectDirection(rcCaption, 4, 1, 0, 0, bVertical);

	DrawCaptionIcon(dc, pPane, rcCaption, bVertical);
	DrawCaptionText(dc, strTitle, rcCaption, bVertical);
}

void CXTPDockingPaneDefaultTheme::DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	BOOL bVertical = pPane->IsCaptionVertical();

	CRect rcCaption(rc);
	XTPDeflateRectDirection(rcCaption, 0, 2, 0, XTPGetRectDirectionHeight(rc, bVertical)  - m_nTitleHeight - 1, bVertical);

	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	if (bVertical)
	{
		dc.FillSolidRect(rc.left, rc.top, rcCaption.right + 2, rcCaption.Height(), GetXtremeColor(COLOR_3DFACE));
	}
	else
	{
		dc.FillSolidRect(rc.left, rc.top, rc.Width(), rcCaption.bottom + 2, GetXtremeColor(COLOR_3DFACE));
	}

	DrawCaptionPart(dc, pPane, rcCaption, pPane->GetSelected() ? pPane->GetSelected()->GetTitle() : _T(""), bActive, bVertical);
}
///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneGripperedTheme

void CXTPDockingPanePaintManager::DrawCaptionButtonEntry(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, CPoint pt, COLORREF clrButton)
{
	CXTPPenDC pen(*pDC, clrButton);

	if (pButton->GetID() == XTP_IDS_DOCKINGPANE_CLOSE)
	{
		pDC->MoveTo(pt.x - 4, pt.y - 3 - 1); pDC->LineTo(pt.x + 3 , pt.y + 4 - 1);
		pDC->MoveTo(pt.x - 4, pt.y + 3 - 1); pDC->LineTo(pt.x + 3 , pt.y - 4 - 1);

		pDC->MoveTo(pt.x - 3, pt.y - 3 - 1); pDC->LineTo(pt.x + 4 , pt.y + 4 - 1);
		pDC->MoveTo(pt.x - 3, pt.y + 3 - 1); pDC->LineTo(pt.x + 4 , pt.y - 4 - 1);

	}
	else if (pButton->GetID() == XTP_IDS_DOCKINGPANE_AUTOHIDE)
	{
		CXTPDockingPaneCaptionButton::DrawPinnButton(pDC, pt, pButton->GetState() & xtpPanePinPushed);
	}
	else if (pButton->GetID() == XTP_IDS_DOCKINGPANE_MAXIMIZE || pButton->GetID() == XTP_IDS_DOCKINGPANE_RESTORE)
	{
		CXTPDockingPaneCaptionButton::DrawMaximizeRestoreButton(pDC, pt, pButton->GetID() == XTP_IDS_DOCKINGPANE_MAXIMIZE, clrButton);
	}
	else if (pButton->GetID() == XTP_IDS_DOCKINGPANE_MENU)
	{
		CXTPBrushDC brush (*pDC, clrButton);
		CPoint pts[] =
		{
			CPoint(pt.x - 4, pt.y - 1), CPoint(pt.x - 1, pt.y + 2) , CPoint(pt.x, pt.y + 2), CPoint(pt.x + 3, pt.y - 1)
		};
		pDC->Polygon(pts, 4);
	}
}

CXTPDockingPaneGripperedTheme::CXTPDockingPaneGripperedTheme()
{
	m_nCaptionButtonStyle = xtpPaneCaptionButtonOffice;
}

void CXTPDockingPaneGripperedTheme::DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL, BOOL bVertical)
{
	COLORREF clrText = GetXtremeColor(COLOR_BTNTEXT);

	XTPDeflateRectDirection(rcCaption, 0, 0, 2, 0, bVertical);
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, clrText, 14, 2, bVertical);

	int nOffset = 4;
	XTPDeflateRectDirection(rcCaption, 4, 1, 2, 0, bVertical);

	if (!strTitle.IsEmpty())
	{
		dc.SetTextColor(IsCaptionEnabled(pPane) ? clrText : GetXtremeColor(COLOR_GRAYTEXT));


		DrawCaptionIcon(dc, pPane, rcCaption, bVertical);
		int nWidth = DrawCaptionText(dc, strTitle, rcCaption, bVertical, TRUE);

		nOffset = nWidth + 4 + (bVertical ? rcCaption.top : rcCaption.left);
	}

	if (bVertical)
	{
		if (rcCaption.bottom > nOffset)
		{
			int nCenter = rcCaption.CenterPoint().x;
			dc.Draw3dRect(nCenter - 3, nOffset, 3, rcCaption.bottom - nOffset, GetXtremeColor(COLOR_BTNHILIGHT), GetXtremeColor(COLOR_3DSHADOW));
			dc.Draw3dRect(nCenter + 1, nOffset, 3, rcCaption.bottom - nOffset, GetXtremeColor(COLOR_BTNHILIGHT), GetXtremeColor(COLOR_3DSHADOW));
		}
	}
	else
	{
		if (rcCaption.right > nOffset)
		{
			int nCenter = rcCaption.CenterPoint().y;
			dc.Draw3dRect(nOffset, nCenter - 3, rcCaption.right - nOffset, 3, GetXtremeColor(COLOR_BTNHILIGHT), GetXtremeColor(COLOR_3DSHADOW));
			dc.Draw3dRect(nOffset, nCenter + 1, rcCaption.right - nOffset, 3, GetXtremeColor(COLOR_BTNHILIGHT), GetXtremeColor(COLOR_3DSHADOW));
		}
	}
}

void CXTPDockingPaneGripperedTheme::DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc)
{
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DFACE), 0);
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_3DFACE));

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);

	CXTPBufferDC dcCache(dc, rcCaption);
	dcCache.SetBkMode(TRANSPARENT);

	dcCache.FillSolidRect(rcCaption, GetXtremeColor(COLOR_3DFACE));

	CString strTitle;
	pPane->GetWindowText(strTitle);

	rcCaption.DeflateRect(1, 2, 1, 2);

	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();
	DrawCaptionPart(dcCache, pPane, rcCaption, strTitle, bActive, FALSE);
}

void CXTPDockingPaneGripperedTheme::DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	BOOL bVertical = pPane->IsCaptionVertical();

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	XTPDeflateRectDirection(rcCaption, 0, m_rcCaptionMargin.top, 0, XTPGetRectDirectionHeight(rc, bVertical) - nTitleHeight - 1, bVertical);

	if (bVertical)
	{
		dc.FillSolidRect(rc.left, rc.top, rcCaption.right + 2, rc.Height(), m_clrSplitter);
	}
	else
	{
		dc.FillSolidRect(rc.left, rc.top, rc.Width(), rcCaption.bottom + 2, m_clrSplitter);
	}

	CString strTitle = pPane->GetSelected() ? pPane->GetSelected()->GetTitle() : _T("");
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();
	DrawCaptionPart(dc, pPane, rcCaption, strTitle, bActive, bVertical);

}

//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneExplorerTheme

CXTPDockingPaneExplorerTheme::CXTPDockingPaneExplorerTheme()
{
	m_bUseBoldCaptionFont = FALSE;
	m_rcCaptionMargin.SetRect(0, 0, 0, 0);
	m_nCaptionFontGap = 9;

	m_nCaptionButtonStyle = xtpPaneCaptionButtonThemedToolBar;
}

void CXTPDockingPaneExplorerTheme::DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL, BOOL bVertical)
{
	if (m_themeRebar.IsAppThemed())
	{
		CWnd* pWnd = pPane->GetType() == xtpPaneTypeTabbedContainer ? (CWnd*)(CXTPDockingPaneTabbedContainer*)pPane :
			pPane->GetType() == xtpPaneTypeMiniWnd ? (CWnd*)(CXTPDockingPaneMiniWnd*)pPane : 0;

		ASSERT(pWnd);
		if (!pWnd)
			return;

		if (pPane->GetType() == xtpPaneTypeMiniWnd)
		{
			CRect rcClient;
			pWnd->GetWindowRect(rcClient);
			rcClient.OffsetRect(-rcClient.TopLeft());

			m_themeRebar.DrawThemeBackground(dc, 0, 0, &rcClient, &rcCaption);
		}
		else
		{

			CWnd* pParent = pPane->GetDockingSite();

			CRect rc, rcParent, rcClient, rcClipRect;
			pWnd->GetClientRect(&rcClient);
			rcClipRect = rcClient;

			if (pParent)
			{
				pParent->GetWindowRect(rcParent);
				pWnd->GetWindowRect(rc);

				rcClient.TopLeft() += rcParent.TopLeft() - rc.TopLeft();
				rcClient.BottomRight() += rcParent.BottomRight() - rc.BottomRight();
			}

			m_themeRebar.DrawThemeBackground(dc, 0, 0, &rcClient, &rcCaption);
		}
	}

	COLORREF clrText = GetXtremeColor(COLOR_BTNTEXT);

	XTPDeflateRectDirection(rcCaption, 0, 0, 2, 0, bVertical);
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, clrText, 16, 2, bVertical);

	dc.SetTextColor(IsCaptionEnabled(pPane) ? clrText : GetXtremeColor(COLOR_GRAYTEXT));
	XTPDeflateRectDirection(rcCaption, 4, 1, 0, 0, bVertical);

	DrawCaptionIcon(dc, pPane, rcCaption, bVertical);
	DrawCaptionText(dc, strTitle, rcCaption, bVertical);
}

void CXTPDockingPaneExplorerTheme::RefreshMetrics()
{
	CXTPDockingPaneGripperedTheme::RefreshMetrics();

	m_themeRebar.OpenThemeData(0, L"REBAR");

	if (m_themeRebar.IsAppThemed())
	{
		m_pPanelPaintManager->SetAppearance(xtpTabAppearancePropertyPage);
		m_pPanelPaintManager->SetColor(xtpTabColorWinXP);
		m_pPanelPaintManager->m_bHotTracking = TRUE;

		m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPage);
		m_pTabPaintManager->SetColor(xtpTabColorWinXP);
		m_pTabPaintManager->m_bHotTracking = TRUE;
	}
	else
	{
		m_pPanelPaintManager->SetAppearance(xtpTabAppearanceFlat);
		m_pPanelPaintManager->m_bHotTracking = FALSE;

		m_pTabPaintManager->SetAppearance(xtpTabAppearanceVisualStudio);
		m_pTabPaintManager->m_bHotTracking = FALSE;

	}
}

///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneVisioTheme

CXTPDockingPaneVisioTheme::CXTPDockingPaneVisioTheme()
{
	m_pPanelPaintManager->SetAppearance(xtpTabAppearanceFlat);

	m_pTabPaintManager->SetAppearance(xtpTabAppearanceVisio);
	m_pTabPaintManager->SetPosition(xtpTabPositionTop);

	m_nCaptionFontGap = 2;
	m_nCaptionButtonStyle = xtpPaneCaptionButtonOffice2003;
}



void CXTPDockingPaneVisioTheme::RefreshMetrics()
{
	CXTPDockingPanePaintManager::RefreshMetrics();
}


void CXTPDockingPaneVisioTheme::AdjustCaptionRect(const CXTPDockingPaneTabbedContainer* pPane, CRect& rc)
{
	CXTPDockingPaneOfficeTheme::AdjustCaptionRect(pPane, rc);

	BOOL bVertical = pPane->IsCaptionVertical();

	XTPDeflateRectDirection(rc, 3, 0, 3, 3, bVertical);

	if (!pPane->IsTitleVisible())
	{
		rc.DeflateRect(0, 3, 0, 0);
	}
	if (!pPane->IsTabsVisible() && m_pTabPaintManager->m_clientFrame != xtpTabFrameNone)
	{
		rc.DeflateRect(1, 1);
	}
}

void CXTPDockingPaneVisioTheme::DrawPane(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();
	dc.FillSolidRect(rc, GetXtremeColor(bActive ? XPCOLOR_HIGHLIGHT : XPCOLOR_TOOLBAR_FACE));
	dc.SetBkMode(TRANSPARENT);

	CRect rcClient(rc);

	if (pPane->IsTitleVisible())
	{
		dc.FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_clrSplitter);
		DrawCaption(dc, pPane, rc);
		rcClient.top++;
	}

	AdjustCaptionRect(pPane, rc);

	if (pPane->IsTabsVisible())
	{
		m_pTabPaintManager->DrawTabControl(pPane, &dc, rc);
	}
	else if (m_pTabPaintManager->m_clientFrame != xtpTabFrameNone)
	{
		dc.Draw3dRect(rc.left - 1, rc.top - 1, rc.Width() + 2, rc.Height() + 2, m_pTabPaintManager->GetColorSet()->m_clrFrameBorder, m_pTabPaintManager->GetColorSet()->m_clrFrameBorder);
	}


	Pixel(&dc, rcClient.left, rcClient.top, XPCOLOR_3DFACE);
	Pixel(&dc, rcClient.right - 1, rcClient.top, XPCOLOR_3DFACE);
	Pixel(&dc, rcClient.left, rcClient.bottom -1, XPCOLOR_3DFACE);
	Pixel(&dc, rcClient.right - 1, rcClient.bottom -1, XPCOLOR_3DFACE);
}

void CXTPDockingPaneVisioTheme::DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc)
{
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();
	COLORREF clrFrame = GetXtremeColor(bActive ? XPCOLOR_HIGHLIGHT : XPCOLOR_TOOLBAR_FACE);

	dc.Draw3dRect(rc, clrFrame, 0);
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_3DSHADOW));
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, clrFrame, clrFrame);

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);

	dc.FillSolidRect(rcCaption, clrFrame);

	rcCaption.right -= 2;
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, GetXtremeColor(COLOR_BTNTEXT), 16, 0, FALSE);

	CString strTitle;
	pPane->GetWindowText(strTitle);


	int nRight = rcCaption.right;


	COLORREF clrText = GetXtremeColor(COLOR_BTNTEXT);
	int nLeft = 4;
	if (!strTitle.IsEmpty())
	{
		dc.SetBkMode(TRANSPARENT);
		CRect rcText(rcCaption.left + 4, rcCaption.top + 1, nRight - 2, rcCaption.bottom);
		dc.SetTextColor(clrText);
		CXTPFontDC font(&dc, &m_fntTitle);
		dc.DrawText(strTitle, rcText, DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX);

		CRect rcCalc(0, 0, 0, 0);
		dc.DrawText(strTitle, rcCalc, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);

		nLeft = rcCalc.Width() + rcCaption.left + 14;
	}
	nRight -= 4;

	if (nRight - nLeft > 0)
	{
		int nCenter = rcCaption.CenterPoint().y;
		Line(&dc, nLeft, nCenter - 3, nRight, nCenter - 3, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
		Line(&dc, nLeft, nCenter - 1, nRight, nCenter - 1, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
		Line(&dc, nLeft, nCenter + 1, nRight, nCenter + 1, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
		Line(&dc, nLeft, nCenter + 3, nRight, nCenter + 3, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
	}
}

void CXTPDockingPaneVisioTheme::DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	BOOL bVertical = pPane->IsCaptionVertical();
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();
	int nTitleHeight = m_nTitleHeight;
	CString strTitle = pPane->GetSelected()->GetTitle();

	CRect rcCaption(rc);
	XTPDeflateRectDirection(rcCaption, 0, 2, 1, XTPGetRectDirectionHeight(rc, bVertical) - nTitleHeight - 3, bVertical);


	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, 0, 16, 0, bVertical);

	COLORREF clrText = GetXtremeColor(COLOR_BTNTEXT);

	int nOffset = 4;
	XTPDeflateRectDirection(rcCaption, 4, 1, 2, 0, bVertical);

	if (!strTitle.IsEmpty())
	{
		dc.SetTextColor(IsCaptionEnabled(pPane) ? clrText : GetXtremeColor(COLOR_GRAYTEXT));
		int nWidth = DrawCaptionText(dc, strTitle, rcCaption, bVertical, TRUE);

		nOffset = nWidth + 10 + (bVertical ? rcCaption.top : rcCaption.left);
	}

	if (bVertical)
	{
		if (rcCaption.bottom - nOffset > 0)
		{
			int nCenter = rcCaption.CenterPoint().x;
			Line(&dc, nCenter - 3, nOffset, nCenter - 3, rcCaption.bottom, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
			Line(&dc, nCenter - 1, nOffset, nCenter - 1, rcCaption.bottom, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
			Line(&dc, nCenter + 1, nOffset, nCenter + 1, rcCaption.bottom, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
			Line(&dc, nCenter + 3, nOffset, nCenter + 3, rcCaption.bottom, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
		}
	}
	else
	{
		if (rcCaption.right - nOffset > 0)
		{
			int nCenter = rcCaption.CenterPoint().y;
			Line(&dc, nOffset, nCenter - 3, rcCaption.right, nCenter - 3, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
			Line(&dc, nOffset, nCenter - 1, rcCaption.right, nCenter - 1, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
			Line(&dc, nOffset, nCenter + 1, rcCaption.right, nCenter + 1, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
			Line(&dc, nOffset, nCenter + 3, rcCaption.right, nCenter + 3, !bActive ? XPCOLOR_TOOLBAR_GRIPPER : COLOR_BTNTEXT);
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneOffice2003Theme


CXTPDockingPaneOffice2003Theme::CXTPDockingPaneOffice2003Theme()
{
	m_bDrawGripper = TRUE;
	m_bRoundedCaption = TRUE;
	m_nCaptionFontGap = 7;
	m_bUseBoldCaptionFont = TRUE;

	m_pPanelPaintManager->SetAppearance(xtpTabAppearancePropertyPageFlat);
	m_pPanelPaintManager->SetColor(xtpTabColorOffice2003);
	m_pPanelPaintManager->m_bHotTracking = TRUE;

	m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPageFlat);
	m_pTabPaintManager->SetColor(xtpTabColorOffice2003);
	m_pTabPaintManager->m_bHotTracking = TRUE;

	m_nCaptionButtonStyle = xtpPaneCaptionButtonOffice2003;
}

CXTPDockingPaneOffice2003Theme::~CXTPDockingPaneOffice2003Theme()
{

}


void CXTPDockingPaneOffice2003Theme::RefreshMetrics()
{
	CXTPDockingPaneOfficeTheme::RefreshMetrics();

	m_clrNormalCaption.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 0xcd), GetXtremeColor(COLOR_3DFACE));
	m_clrNormalCaptionText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));

	m_clrActiveCaption.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED));
	m_clrActiveCaptionText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));

	m_bLunaTheme = FALSE;

	if (XTPColorManager()->IsLowResolution())
	{
		m_clrNormalCaption.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
		m_clrActiveCaption.SetStandardValue(GetXtremeColor(COLOR_HIGHLIGHT));
		m_clrActiveCaptionText.SetStandardValue(GetXtremeColor(COLOR_HIGHLIGHTTEXT));
	}

	XTPCurrentSystemTheme systemTheme = GetCurrentSystemTheme();

	switch (systemTheme)
	{
	case xtpSystemThemeBlue:
		{
			m_clrNormalCaption.SetStandardValue(RGB(221, 236, 254), RGB(129, 169, 226), 0.75f);
			m_clrSplitter.SetStandardValue(RGB(216, 231, 252));
			m_clrSplitterGripper.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));

			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(39, 65, 118), RGB(106, 140, 203), RGB(109, 150, 208), RGB(246, 246, 246), RGB(0, 45, 150), RGB(255, 238, 194), RGB(0, 0, 128), RGB(0, 0, 128), RGB(0, 0, 128), RGB(254, 128, 62), RGB(255, 192, 111), RGB(158, 190, 245), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_bLunaTheme = TRUE;

		}
		break;

	case xtpSystemThemeOlive:
		{
			m_clrNormalCaption.SetStandardValue(RGB(244, 247, 222), RGB(183, 198, 145), 0.3f);
			m_clrSplitter.SetStandardValue(RGB(226, 231, 191));
			m_clrSplitterGripper.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));

			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(81, 94, 51), RGB(96, 128, 88), RGB(159, 174, 122), RGB(244, 244, 238), RGB(117, 141, 94), RGB(255, 238, 194), RGB(63, 93, 56), RGB(63, 93, 56), RGB(63, 93, 56), RGB(254, 128, 62), RGB(255, 192, 111), RGB(217, 217, 167), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_bLunaTheme = TRUE;
		}
		break;

	case xtpSystemThemeSilver:
		{
			m_clrNormalCaption.SetStandardValue(RGB(249, 249, 255), RGB(156, 155, 183), 0.75f);
			m_clrSplitter.SetStandardValue(RGB(223, 223, 234));
			m_clrSplitterGripper.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));

			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(84, 84, 117), RGB(110, 109, 143), RGB(168, 167, 190), RGB(253, 250, 255), RGB(124, 124, 148), RGB(255, 238, 194), RGB(75, 75, 111), RGB(75, 75, 111), RGB(75, 75, 111), RGB(254, 128, 62), RGB(255, 192, 111), RGB(215, 215, 229), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_bLunaTheme = TRUE;
		}
		break;
	}

	if (m_bLunaTheme)
	{
		m_clrActiveCaption.SetStandardValue(RGB(255, 213, 140), RGB(255, 166, 76));
		m_clrSplitterGradient.SetStandardValue(XTPColorManager()->grcDockBar);
	}


}

CRect CXTPDockingPaneOffice2003Theme::GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* /*pPane*/)
{
	return CRect(0, 0, 10, m_nTitleHeight);
}



int CXTPDockingPanePaintManager::DrawCaptionText(CDC& dc, const CString& strTitle, CRect rcText, BOOL bVertical, BOOL bCalcWidth)
{
	int nWidth = 0;
	if (strTitle.IsEmpty())
		return 0;

	if (bVertical)
	{
		CXTPFontDC font(&dc, &m_fntTitleVertical);

		CSize sz = dc.GetTextExtent(strTitle);
		if (rcText.bottom > rcText.top)
		{
			rcText.left = (rcText.right - (rcText.Width() - sz.cy) / 2);
			rcText.right = rcText.left + rcText.Height();

			dc.DrawText(strTitle, rcText, DT_NOPREFIX | DT_SINGLELINE | DT_NOCLIP | DT_END_ELLIPSIS);
		}
		nWidth = sz.cx;
	}
	else
	{
		CXTPFontDC font(&dc, &m_fntTitle);
		dc.DrawText(strTitle, rcText, DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX);

		if (bCalcWidth)
		{
			CXTPEmptyRect rcCalc;
			dc.DrawText(strTitle, rcCalc, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
			nWidth = rcCalc.Width();
		}
	}
	return nWidth;
}

CXTPDockingPane* CXTPDockingPanePaintManager::GetSelectedPane(CXTPDockingPaneBase* pPane)
{
	if (pPane->GetType() == xtpPaneTypeTabbedContainer)
	{
		return ((CXTPDockingPaneTabbedContainer*)pPane)->GetSelected();
	}
	else if (pPane->GetType() == xtpPaneTypeMiniWnd)
	{
		CXTPDockingPaneBaseList lst;
		pPane->FindPane(xtpPaneTypeTabbedContainer, &lst);

		if (lst.GetCount() == 1)
		{
			return ((CXTPDockingPaneTabbedContainer*)lst.GetHead())->GetSelected();
		}
	}
	return NULL;
}

BOOL CXTPDockingPanePaintManager::IsCaptionEnabled(CXTPDockingPaneBase* pPane)
{
	CXTPDockingPane* pSelectedPane = GetSelectedPane(pPane);
	if (!pSelectedPane)
		return TRUE;
	return pSelectedPane->GetEnabled() & xtpPaneEnableClient;
}

void CXTPDockingPanePaintManager::DrawCaptionIcon(CDC& dc, CXTPDockingPaneBase* pPane, CRect& rcCaption, BOOL bVertical)
{
	if (!m_bDrawCaptionIcon)
		return;

	CSize szIcon = GetTabPaintManager()->m_szIcon;

	CXTPDockingPane* pSelectedPane = GetSelectedPane(pPane);
	CXTPImageManagerIcon* pImage = pSelectedPane ? pSelectedPane->GetIcon(szIcon.cx) : NULL;

	if (!pImage)
		return;

	BOOL bEnabled = IsCaptionEnabled(pPane);
	CXTPImageManagerIconHandle& imageHandle = !bEnabled ? pImage->GetDisabledIcon(): pImage->GetIcon();

	if (!bVertical && rcCaption.Width() >= szIcon.cx)
	{
		pImage->Draw(&dc, CPoint(rcCaption.left, rcCaption.CenterPoint().y - szIcon.cy / 2),
			imageHandle, szIcon);

		rcCaption.left += szIcon.cx + 3;
	}
	else if (bVertical && rcCaption.Height() >= szIcon.cy)
	{
		pImage->Draw(&dc, CPoint(rcCaption.CenterPoint().x - szIcon.cy / 2, rcCaption.top),
			imageHandle, szIcon);

		rcCaption.top += szIcon.cy + 3;
	}
}

void CXTPDockingPaneOffice2003Theme::DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical)
{
	COLORREF clrText = bActive ? m_clrActiveCaptionText : m_clrNormalCaptionText ;

	XTPDeflateRectDirection(rcCaption, 0, 0, 4, 0, bVertical);
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, clrText, 16, 0, bVertical);

	if (!bVertical && m_bDrawGripper && (rcCaption.right > rcCaption.left + 7))
	{
		for (int y = 5; y < rcCaption.Height() - 3; y += 4)
		{
			dc.FillSolidRect(rcCaption.left + 4 + 2, y + 1, 2, 2, GetXtremeColor(COLOR_WINDOW));
			dc.FillSolidRect(rcCaption.left + 3 + 2, y, 2, 2, GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		}
		rcCaption.left += 8;
	}

	if (bVertical && m_bDrawGripper && (rcCaption.bottom > rcCaption.top + 7))
	{
		for (int x = 3; x < rcCaption.Width() - 5; x += 4)
		{
			dc.FillSolidRect(x + 2, rcCaption.top + 4 + 2, 2, 2, GetXtremeColor(COLOR_WINDOW));
			dc.FillSolidRect(x + 1, rcCaption.top + 3 + 2, 2, 2, GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		}
		rcCaption.top += 8;
	}

	dc.SetTextColor(IsCaptionEnabled(pPane) ? clrText : GetXtremeColor(COLOR_GRAYTEXT));

	XTPDeflateRectDirection(rcCaption, 6, 1, 2, 0, bVertical);

	DrawCaptionIcon(dc, pPane, rcCaption, bVertical);
	DrawCaptionText(dc, strTitle, rcCaption, bVertical);
}

void CXTPDockingPaneOffice2003Theme::DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc)
{
	COLORREF clrFrame = GetXtremeColor(XPCOLOR_MENUBAR_BORDER);

	dc.Draw3dRect(rc, clrFrame, clrFrame);
	rc.DeflateRect(1, 1);
	dc.Draw3dRect(rc, clrFrame, clrFrame);
	rc.DeflateRect(1, 1);

	dc.Draw3dRect(rc, clrFrame, clrFrame);
	XTPDrawHelpers()->ExcludeCorners(&dc, rc);
	dc.Draw3dRect(rc, m_clrNormalCaption.clrDark, m_clrNormalCaption.clrDark);

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);

	CXTPBufferDC dcCache(dc, rcCaption);
	dcCache.SetBkMode(TRANSPARENT);

	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	XTPDrawHelpers()->GradientFill(&dcCache, rcCaption,
		bActive ? m_clrActiveCaption : m_clrNormalCaption, FALSE);

	dcCache.SetPixel(rcCaption.left, rcCaption.bottom, m_clrNormalCaption.clrDark);
	dcCache.SetPixel(rcCaption.right - 1, rcCaption.bottom, m_clrNormalCaption.clrDark);

	rcCaption.DeflateRect(1, 0);

	CString strTitle;
	pPane->GetWindowText(strTitle);

	DrawCaptionPart(dcCache, pPane, rcCaption, strTitle, bActive, FALSE);
}

void CXTPDockingPaneOffice2003Theme::DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	BOOL bVertical = pPane->IsCaptionVertical();

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);

	if (bVertical)
	{
		rcCaption.DeflateRect(0, 0, rc.Width() - nTitleHeight - 3, 0);
	}
	else
	{
		rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);
	}

	CRect rcFill(rcCaption);

	if (m_bRoundedCaption)
	{
		XTPDeflateRectDirection(rcFill, 1, 0, 0, 0, bVertical);
		dc.FillSolidRect(rcCaption, m_clrSplitter);

		XTPDrawHelpers()->ExcludeCorners(&dc, rcFill);
	}

	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	XTPDrawHelpers()->GradientFill(&dc, rcFill,
		bActive ? m_clrActiveCaption : m_clrNormalCaption, bVertical);

	if (!m_bRoundedCaption)
	{
		dc.FillSolidRect(rcCaption.left, rcCaption.top, 1, rcCaption.Height(), GetXtremeColor(XPCOLOR_FRAME));
		dc.FillSolidRect(rcCaption.left, rcCaption.top, rcCaption.Width(), 1, GetXtremeColor(XPCOLOR_FRAME));
		dc.FillSolidRect(rcCaption.right - 1, rcCaption.top, 1, rcCaption.Height(), GetXtremeColor(XPCOLOR_FRAME));
	}

	CString strTitle = pPane->GetSelected() ? pPane->GetSelected()->GetTitle() : _T("");
	DrawCaptionPart(dc, pPane, rcCaption, strTitle, bActive, bVertical);
}




CXTPDockingPaneOffice2007Theme::CXTPDockingPaneOffice2007Theme()
{
	m_bRoundedCaption = FALSE;
	m_dwSplitterStyle = xtpPaneSplitterGradientVert;

	m_pPanelPaintManager->SetAppearance(xtpTabAppearancePropertyPage2007);
	m_pPanelPaintManager->m_bHotTracking = TRUE;

	m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPage2007);
	m_pTabPaintManager->m_bHotTracking = TRUE;

	m_pPanelPaintManager->m_rcButtonMargin.SetRect(2, 1, 2, 1);
	m_pTabPaintManager->m_rcButtonMargin.SetRect(2, 1, 2, 1);

}

void CXTPDockingPaneOffice2007Theme::DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	BOOL bVertical = pPane->IsCaptionVertical();

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);

	if (bVertical)
	{
		rcCaption.DeflateRect(0, 0, rc.Width() - nTitleHeight - 3, 0);
	}
	else
	{
		rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);
	}

	CRect rcFill(rcCaption);

	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	XTPDrawHelpers()->GradientFill(&dc, rcFill,
		bActive ? m_clrActiveCaption : m_clrNormalCaption, bVertical);

	dc.FillSolidRect(rcFill.left, rcFill.top, rcFill.Width(), 1, m_arrColor[XPCOLOR_FRAME]);
	dc.FillSolidRect(rcFill.left, rcFill.top + 1, rcFill.Width(), 1, m_arrColor[COLOR_3DHIGHLIGHT]);

	dc.FillSolidRect(rcFill.left, rcFill.top, 1, rcFill.Height(), m_arrColor[XPCOLOR_FRAME]);
	dc.FillSolidRect(rcFill.left + 1, rcFill.top + 1, 1, rcFill.Height() - 1, m_arrColor[COLOR_3DHIGHLIGHT]);
	dc.FillSolidRect(rcFill.right - 1, rcFill.top, 1, rcFill.Height(), m_arrColor[XPCOLOR_FRAME]);

	CString strTitle = pPane->GetSelected() ? pPane->GetSelected()->GetTitle() : _T("");
	DrawCaptionPart(dc, pPane, rcCaption, strTitle, bActive, bVertical);
}

void CXTPDockingPaneOffice2007Theme::RefreshMetrics()
{
	CXTPDockingPaneOffice2003Theme::RefreshMetrics();

	if (XTPOffice2007Images()->IsValid())
	{
		m_arrColor[XPCOLOR_FRAME] = XTPOffice2007Images()->GetImageColor(_T("Window"), _T("WindowFrame"));
		m_arrColor[XPCOLOR_3DFACE] = XTPOffice2007Images()->GetImageColor(_T("Window"), _T("ButtonFace"));
		m_arrColor[COLOR_3DHIGHLIGHT] = XTPOffice2007Images()->GetImageColor(_T("Window"), _T("EdgeHighLightColor"));

		m_clrSplitter.SetStandardValue(XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("Splitter")));
		m_clrSplitterGripper.SetStandardValue(m_arrColor[XPCOLOR_FRAME]);

		m_arrColor[XPCOLOR_TOOLBAR_GRIPPER] = XTPOffice2007Images()->GetImageColor(_T("Toolbar"), _T("ToolbarGripper"));
		m_arrColor[XPCOLOR_MENUBAR_BORDER] = XTPOffice2007Images()->GetImageColor(_T("Toolbar"), _T("MenuPopupBorder"));

		m_bLunaTheme = TRUE;

		m_clrSplitterGradient.SetStandardValue(XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("SplitterLight")),
			XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("SplitterDark")));

		m_clrNormalCaption.SetStandardValue(XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("NormalCaptionLight")),
			XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("NormalCaptionDark")), 0.25f);
		m_clrNormalCaptionText.SetStandardValue(XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("NormalCaptionText")));

		m_clrActiveCaption.SetStandardValue(XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("ActiveCaptionLight")),
			XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("ActiveCaptionDark")), 0.25f);
		m_clrActiveCaptionText.SetStandardValue(XTPOffice2007Images()->GetImageColor(_T("DockingPane"), _T("ActiveCaptionText")));

	}
	else
	{
		m_arrColor[XPCOLOR_FRAME] = RGB(101, 147, 207);
		m_arrColor[XPCOLOR_3DFACE] = RGB(191, 219, 255);
		m_arrColor[COLOR_3DHIGHLIGHT] = RGB(255, 255, 255);

		m_clrNormalCaption.SetStandardValue(RGB(227, 239, 255), RGB(175, 210, 255), 0.25f);
		m_clrSplitter.SetStandardValue(RGB(216, 231, 252));
		m_clrSplitterGripper.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		m_clrSplitterGradient.SetStandardValue(RGB(190, 218, 254), RGB(100, 143, 203));

		INT nElements[] =
		{
			XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
		};
		COLORREF clrValues[] =
		{
			RGB(39, 65, 118), RGB(106, 140, 203), RGB(109, 150, 208), RGB(246, 246, 246), RGB(0, 45, 150), RGB(255, 238, 194), RGB(0, 0, 128), RGB(0, 0, 128), RGB(0, 0, 128), RGB(254, 128, 62), RGB(255, 192, 111), RGB(158, 190, 245), 0
		};
		SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

		m_bLunaTheme = TRUE;

		m_clrActiveCaption.SetStandardValue(RGB(255, 231, 162), RGB(255, 166, 76));
		m_clrNormalCaptionText.SetStandardValue(RGB(8, 55, 114));
		m_clrActiveCaptionText.SetStandardValue(RGB(8, 55, 114));
	}

}

///////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneShortcutBar2003Theme


CXTPDockingPaneShortcutBar2003Theme::CXTPDockingPaneShortcutBar2003Theme()
{
	m_bDrawGripper = FALSE;
	m_bRoundedCaption = FALSE;
	m_nCaptionFontGap = 3;
}

CXTPDockingPaneShortcutBar2003Theme::~CXTPDockingPaneShortcutBar2003Theme()
{
}


void CXTPDockingPaneShortcutBar2003Theme::RefreshMetrics()
{
	CXTPDockingPaneOffice2003Theme::RefreshMetrics();

	CWindowDC dc(CWnd::GetDesktopWindow());

	if (m_bUseStandardFont)
	{
		LOGFONT lf = { sizeof(LOGFONT)};

		VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT,
			sizeof(LOGFONT), &lf, 0));

		STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Arial"));
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = 20;

		SetCaptionFontIndirect(&lf);
	}

	if (m_bLunaTheme)
	{
		m_clrNormalCaption.SetStandardValue(XTPColorManager()->grcCaption);
		m_clrNormalCaptionText.SetStandardValue(RGB(255, 255, 255));
	}
	else
	{
		m_clrNormalCaption.SetStandardValue(XTPColorManager()->LightColor(
			GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_WINDOW), 0xcd),
			GetXtremeColor(COLOR_3DSHADOW));
	}
}

CRect CXTPDockingPaneShortcutBar2003Theme::GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* /*pPane*/)
{
	return CRect(0, 0, 0, 0);
}


//////////////////////////////////////////////////////////////////////////
///

CXTPDockingPaneNativeXPTheme::CXTPDockingPaneNativeXPTheme()
{
	m_pPanelPaintManager->SetAppearance(xtpTabAppearancePropertyPage);
	m_pPanelPaintManager->SetColor(xtpTabColorWinXP);
	m_pPanelPaintManager->m_bHotTracking = TRUE;

	m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPage);
	m_pTabPaintManager->SetColor(xtpTabColorWinXP);
	m_pTabPaintManager->m_bHotTracking = TRUE;

	m_bGradientCaption = TRUE;
	m_nCaptionFontGap = 7;

	m_nCaptionButtonStyle = xtpPaneCaptionButtonThemedExplorerBar;
}

void CXTPDockingPaneNativeXPTheme::RefreshMetrics()
{
	CXTPDockingPaneOfficeTheme::RefreshMetrics();
}

void CXTPDockingPaneNativeXPTheme::DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc)
{
	if (!m_themeWindow.IsAppThemed())
	{
		CXTPDockingPaneOfficeTheme::DrawFloatingFrame(dc, pPane, rc);
		return;
	}
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	int nState = bActive ? CS_ACTIVE : CS_INACTIVE;

	m_themeWindow.DrawThemeBackground(dc, WP_SMALLFRAMELEFT, nState, CRect(rc.left, rc.top, rc.left + 3, rc.bottom), 0);
	m_themeWindow.DrawThemeBackground(dc, WP_SMALLFRAMERIGHT, nState, CRect(rc.right - 3, rc.top, rc.right, rc.bottom), 0);
	m_themeWindow.DrawThemeBackground(dc, WP_SMALLFRAMEBOTTOM, nState, CRect(rc.left, rc.bottom - 3, rc.right, rc.bottom), 0);

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3 - 2);

	CXTPBufferDC dcCache(dc, rcCaption);
	dcCache.SetBkMode(TRANSPARENT);

	m_themeWindow.DrawThemeBackground(dcCache, WP_SMALLCAPTION, nState, rcCaption, 0);

	rcCaption.DeflateRect(3, 0);

	CString strTitle;
	pPane->GetWindowText(strTitle);

	DrawCaptionPart(dcCache, pPane, rcCaption, strTitle, bActive, FALSE);
}

void CXTPDockingPaneNativeXPTheme::DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical)
{
	if (!m_themeWindow.IsAppThemed())
	{
		CXTPDockingPaneOfficeTheme::DrawCaptionPart(dc, pPane, rcCaption, strTitle, bActive, bVertical);
		return;
	}

	COLORREF clrText = GetXtremeColor(bActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT);

	XTPDeflateRectDirection(rcCaption, 0, 0, 2, 0, bVertical);
	DrawCaptionButtons(&dc, pPane->GetCaptionButtons(), rcCaption, clrText, 16, 3, bVertical);

	dc.SetTextColor(IsCaptionEnabled(pPane) ? clrText : GetXtremeColor(COLOR_GRAYTEXT));
	XTPDeflateRectDirection(rcCaption, 4, 1, 0, 0, bVertical);

	DrawCaptionIcon(dc, pPane, rcCaption, bVertical);
	DrawCaptionText(dc, strTitle, rcCaption, bVertical);
}

void CXTPDockingPaneNativeXPTheme::DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc)
{
	if (!m_themeWindow.IsAppThemed())
	{
		CXTPDockingPaneOfficeTheme::DrawCaption(dc, pPane, rc);
		return;
	}

	BOOL bVertical = pPane->IsCaptionVertical();
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	CRect rcCaption(0, 0, rc.right, m_nTitleHeight + 1);
	if (bVertical)
	{
		rcCaption = CRect(0, 0, m_nTitleHeight + 1, rc.bottom);
	}

	m_themeWindow.DrawThemeBackground(dc, WP_SMALLCAPTION, bActive ? CS_ACTIVE : CS_INACTIVE, rcCaption, 0);

	CString strTitle = pPane->GetSelected() ? pPane->GetSelected()->GetTitle() : _T("");
	DrawCaptionPart(dc, pPane, rcCaption, strTitle, bActive, bVertical);
}


//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneWhidbeyTheme

class CXTPDockingPaneWhidbeyTheme::CColorSetWhidbey : public CXTPTabPaintManager::CColorSetOffice2003
{
public:
	CColorSetWhidbey(BOOL bHighlightSelected)
	{
		m_bHighlightSelected = bHighlightSelected;
		m_bTransparentButton = FALSE;
		m_bGradientButton = FALSE;

	}
	void RefreshMetrics();

	void XTPFillFramePartRect(CDC* pDC, CRect rc, CWnd* pWnd, CWnd* pSite)
	{
		::XTPFillFramePartRect(pDC, rc, pWnd, pSite, m_clrHeaderFace, TRUE);
	}

	void FillHeader(CDC* pDC, CRect rc, CXTPTabManager* pTabManager)
	{
		XTPFillFramePartRect(pDC, rc, (CXTPDockingPaneTabbedContainer*)pTabManager, ((CXTPDockingPaneTabbedContainer*)pTabManager)->GetDockingSite());
	}

	COLORREF FillPropertyButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem)
	{
		if (m_bTransparentButton && (!m_bHighlightSelected || !pItem->IsSelected()))
			return COLORREF_NULL;

		if (m_bGradientButton)
			return CColorSetOffice2003::FillPropertyButton(pDC, rcItem, pItem);

		return CColorSet::FillPropertyButton(pDC, rcItem, pItem);

	}
protected:
	BOOL m_bGradientButton;
	BOOL m_bTransparentButton;
	BOOL m_bHighlightSelected;

};

void CXTPDockingPaneWhidbeyTheme::CColorSetWhidbey::RefreshMetrics()
{
	CColorSet::RefreshMetrics();

	if (m_bHighlightSelected)
	{
		m_clrButtonSelected.SetStandardValue(GetSysColor(COLOR_WINDOW));
		m_clrSelectedText.SetStandardValue(GetSysColor(COLOR_WINDOWTEXT));
	}
	else
	{
		m_clrSelectedText.SetStandardValue(GetSysColor(COLOR_3DDKSHADOW));
	}
	m_clrHeaderFace.SetStandardValue(GetSysColor(COLOR_3DFACE), XTPColorManager()->LightColor(GetSysColor(COLOR_3DFACE), GetSysColor(COLOR_WINDOW), 0xcd));
	m_clrNormalText.SetStandardValue(GetSysColor(COLOR_3DDKSHADOW));

	m_bGradientButton = FALSE;
	m_bLunaTheme = FALSE;

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeSilver:
		m_bGradientButton = TRUE;
		m_clrButtonNormal.SetStandardValue(RGB(242, 242, 247));

		m_csPropertyPage2003.clrBorderTopHighlight.SetStandardValue(RGB(255, 255, 255));
		m_csPropertyPage2003.clrDarkShadow.SetStandardValue(RGB(145, 155, 156));
		m_csPropertyPage2003.clrFrameBorder.SetStandardValue(RGB(145, 155, 156));
		m_csPropertyPage2003.clrBorderLeftHighlight.SetStandardValue(RGB(190, 190, 216));

		m_clrHeaderFace.SetStandardValue(RGB(215, 215, 229), RGB(243, 243, 247));
		break;

	case xtpSystemThemeBlue:
		m_bGradientButton = TRUE;
		m_csPropertyPage2003.clrBorderTopHighlight.SetStandardValue(RGB(255, 255, 255));

		m_clrHeaderFace.SetStandardValue(RGB(229, 229, 215), RGB(244, 241, 231));
		break;

	case xtpSystemThemeOlive:
		m_bGradientButton = TRUE;
		m_csPropertyPage2003.clrBorderTopHighlight.SetStandardValue(RGB(255, 255, 255));

		m_clrHeaderFace.SetStandardValue(RGB(229, 229, 215), RGB(244, 241, 231));
		break;
	}

	m_clrAutoHideFace.SetStandardValue(m_clrHeaderFace);
}

class CXTPDockingPaneWhidbeyTheme::CColorSetVisualStudio2005 : public CColorSetWhidbey
{
public:
	CColorSetVisualStudio2005(BOOL bHighlightSelected)
		: CColorSetWhidbey(bHighlightSelected)
	{
		m_bTransparentButton = TRUE;
	}

	void RefreshMetrics();
};

void CXTPDockingPaneWhidbeyTheme::CColorSetVisualStudio2005::RefreshMetrics()
{
	CColorSetWhidbey::RefreshMetrics();

	m_bGradientButton = FALSE;
	m_bLunaTheme = FALSE;

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeSilver:
	case xtpSystemThemeBlue:
	case xtpSystemThemeOlive:
		m_clrNormalText.SetStandardValue(RGB(113, 111, 100));
		if (!m_bHighlightSelected) m_clrSelectedText.SetStandardValue(RGB(113, 111, 100));
		break;
	}
}


CXTPDockingPaneWhidbeyTheme::CXTPDockingPaneWhidbeyTheme()
{
	CXTPTabPaintManager::CAppearanceSetPropertyPageFlat* pAppearanceFlat = (CXTPTabPaintManager::CAppearanceSetPropertyPageFlat*)
		m_pTabPaintManager->SetAppearance(xtpTabAppearancePropertyPageFlat);

	m_pTabPaintManager->m_bDisableLunaColors = TRUE;
	pAppearanceFlat->m_bBlurPoints = FALSE;
	m_pTabPaintManager->SetColorSet(new CColorSetWhidbey(TRUE));
	m_pTabPaintManager->m_bHotTracking = FALSE;
	m_pTabPaintManager->m_clientFrame = xtpTabFrameNone;

	m_pPanelPaintManager->m_bDisableLunaColors = TRUE;
	pAppearanceFlat = (CXTPTabPaintManager::CAppearanceSetPropertyPageFlat*)m_pPanelPaintManager->SetAppearance(xtpTabAppearancePropertyPageFlat);
	pAppearanceFlat->m_bBlurPoints = FALSE;
	m_pPanelPaintManager->SetColorSet(new CColorSetWhidbey(FALSE));
	m_pPanelPaintManager->m_bHotTracking = FALSE;
	m_pPanelPaintManager->SetLayout(xtpTabLayoutAutoSize);

	m_rcCaptionMargin.SetRect(0, 0, 0, 0);
	m_bGradientCaption = TRUE;
	m_bUseBoldCaptionFont = FALSE;
	m_dwSplitterStyle = xtpPaneSplitterGradient;

	m_systemTheme = xtpSystemThemeUnknown;
}

void CXTPDockingPaneWhidbeyTheme::RefreshMetrics()
{
	CXTPDockingPaneOfficeTheme::RefreshMetrics();

	m_clrNormalCaptionText.SetStandardValue(GetXtremeColor(COLOR_INACTIVECAPTIONTEXT));

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
		m_clrSplitterGradient.SetStandardValue(RGB(229, 229, 215), RGB(244, 241, 231));
		m_clrNormalCaption.SetStandardValue(RGB(216, 215, 198), RGB(238, 239, 229));
		m_clrNormalCaptionText.SetStandardValue(0);
		break;

	case xtpSystemThemeOlive:
		m_clrSplitterGradient.SetStandardValue(RGB(229, 229, 215), RGB(244, 241, 231));
		m_clrNormalCaption.SetStandardValue(RGB(216, 215, 198), RGB(238, 239, 229));
		m_clrNormalCaptionText.SetStandardValue(0);
		break;

	case xtpSystemThemeSilver:
		m_clrSplitterGradient.SetStandardValue(RGB(215, 215, 229), RGB(243, 243, 247));
		m_clrNormalCaption.SetStandardValue(RGB(224, 224, 235), RGB(242, 242, 246));
		m_clrActiveCaption.SetStandardValue(RGB(160, 158, 186), RGB(224, 225, 235));
		m_clrNormalCaptionText.SetStandardValue(0);
		break;
	}

}
CXTPDockingPaneVisualStudio2005SecondTheme::CXTPDockingPaneVisualStudio2005SecondTheme()
{
	m_pTabPaintManager->SetAppearance(xtpTabAppearanceVisualStudio2005);
	m_pTabPaintManager->SetColorSet(new CColorSetWhidbey(TRUE));

	m_pPanelPaintManager->SetAppearance(xtpTabAppearanceVisualStudio2005);
	m_pPanelPaintManager->SetColorSet(new CColorSetWhidbey(TRUE));
}

CXTPDockingPaneVisualStudio2005Theme::CXTPDockingPaneVisualStudio2005Theme()
{
	CXTPTabPaintManager::CAppearanceSetPropertyPageFlat* pAppearance =
		new CXTPTabPaintManager::CAppearanceSetPropertyPageFlat;
	pAppearance->m_bBlurPoints = FALSE;

	m_pPanelPaintManager->SetAppearanceSet(pAppearance);
	m_pPanelPaintManager->SetColorSet(new CColorSetVisualStudio2005(FALSE));
	m_pPanelPaintManager->m_bRotateImageOnVerticalDraw = TRUE;


	pAppearance = new CXTPTabPaintManager::CAppearanceSetPropertyPageFlat;
	pAppearance->m_bBlurPoints = FALSE;
	pAppearance->m_bVisualStudio2005Style = TRUE;

	m_pTabPaintManager->SetAppearanceSet(pAppearance);
	m_pTabPaintManager->SetColorSet(new CColorSetVisualStudio2005(TRUE));

	m_pTabPaintManager->m_clientFrame = xtpTabFrameSingleLine;
	m_pTabPaintManager->m_rcClientMargin.top = 1;

}



void CXTPDockingPaneVisualStudio2005SecondTheme::RefreshMetrics()
{
	CXTPDockingPaneWhidbeyTheme::RefreshMetrics();

	m_clrNormalCaption.SetStandardValue(GetXtremeColor(COLOR_INACTIVECAPTION));
	m_clrActiveCaption.SetStandardValue(GetXtremeColor(COLOR_ACTIVECAPTION));
	m_clrCaptionBorder = GetXtremeColor(COLOR_3DSHADOW);

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
		m_clrNormalCaption.SetStandardValue(RGB(204, 199, 186));
		m_clrActiveCaption.SetStandardValue(RGB(59, 128, 237), RGB(49, 106, 197));
		m_clrNormalCaptionText = 0;
		break;
	case xtpSystemThemeOlive:
		m_clrNormalCaption.SetStandardValue(RGB(204, 199, 186));
		m_clrActiveCaption.SetStandardValue(RGB(182, 195, 146), RGB(145, 160, 117));
		m_clrNormalCaptionText = 0;
		break;
	case xtpSystemThemeSilver:
		m_clrNormalCaption.SetStandardValue(RGB(240, 240, 245));
		m_clrActiveCaption.SetStandardValue(RGB(211, 212, 221), RGB(166, 165, 191));
		m_clrNormalCaptionText = 0;
		m_clrCaptionBorder = RGB(145, 155, 156);
		break;
	}
}

COLORREF CXTPDockingPaneVisualStudio2005SecondTheme::FillCaptionPart(CDC& dc, CXTPDockingPaneBase* /*pPane*/, CRect rcCaption, BOOL bActive, BOOL bVertical)
{
	XTPDrawHelpers()->GradientFill(&dc, rcCaption, bActive ? m_clrActiveCaption : m_clrNormalCaption, bVertical);

	CXTPPenDC pen(dc, m_clrCaptionBorder);
	if (bVertical)
	{
		dc.MoveTo(rcCaption.right, rcCaption.bottom - 1);
		dc.LineTo(rcCaption.left , rcCaption.bottom - 1);
		dc.LineTo(rcCaption.left, rcCaption.top);
		dc.LineTo(rcCaption.right, rcCaption.top);

	}
	else
	{
		dc.MoveTo(rcCaption.left, rcCaption.bottom);
		dc.LineTo(rcCaption.left , rcCaption.top);
		dc.LineTo(rcCaption.right - 1, rcCaption.top);
		dc.LineTo(rcCaption.right - 1, rcCaption.bottom - 1);
	}

	return bActive ? m_clrActiveCaptionText : m_clrNormalCaptionText;
}


