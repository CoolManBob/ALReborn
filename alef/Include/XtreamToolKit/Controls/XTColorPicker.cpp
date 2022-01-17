// XTColorPicker.cpp : implementation of the CXTColorPicker class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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

#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPDrawHelpers.h"

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTButton.h"
#include "XTColorRef.h"
#include "XTColorSelectorCtrl.h"
#include "XTColorPopup.h"
#include "XTColorPicker.h"
#include "XTMemDC.h"
#include "XTThemeManager.h"
#include "XTButtonTheme.h"
#include "XTHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_XT_SHOWPOPUP = (WM_XTP_CONTROLS_BASE + 29);

/////////////////////////////////////////////////////////////////////////////
// CXTColorPickerTheme
/////////////////////////////////////////////////////////////////////////////

class CXTColorPickerTheme : public CXTButtonTheme
{
protected:
	virtual COLORREF GetTextColor (UINT nState, CXTButton* pButton);
	virtual BOOL DrawWinThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);
	virtual BOOL DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);
	virtual void DrawButtonText (CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);
	virtual void DrawFocusRect (CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);

private:
	void DrawArrow(CDC* pDC, CRect& rcArrow, COLORREF crArrow);
	void DrawColorPicker(LPDRAWITEMSTRUCT lpDIS, CXTColorPicker* pColorPicker);
};

COLORREF CXTColorPickerTheme::GetTextColor(UINT nState, CXTButton* pButton)
{
	CXTColorPicker* pColorPicker = DYNAMIC_DOWNCAST(CXTColorPicker, pButton);
	ASSERT_VALID(pColorPicker);

	if (nState & ODS_DISABLED)
		return m_crTextDisabled;

	COLORREF clr;
	if (pColorPicker->IsTextInColor())
		clr = pColorPicker->GetColor();
	else
		clr = pColorPicker->GetContrastColor();

	return clr;
}

void CXTColorPickerTheme::DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton)
{
	CXTColorPicker* pColorPicker = DYNAMIC_DOWNCAST(CXTColorPicker, pButton);
	ASSERT_VALID(pColorPicker);

	// Draw the window text (if any)
	if (pColorPicker->IsTextShown())
		CXTButtonTheme::DrawButtonText(pDC, nState, rcItem, pButton);
}

void CXTColorPickerTheme::DrawFocusRect(CDC* pDC, UINT /*nState*/, CRect& rcItem, CXTButton* pButton)
{
	if (!UseWinXPThemes(pButton))
	{
		CXTContextTextColorHandler foreHandler(pDC, m_crText);
		pButton->GetClientRect(&rcItem);
		rcItem.DeflateRect(3, 3);
		pDC->DrawFocusRect(rcItem);
	}
}

void CXTColorPickerTheme::DrawArrow(CDC* pDC, CRect& rcArrow, COLORREF crArrow)
{
	POINT ptsArrow[3];

	int x = rcArrow.CenterPoint().x;
	int y = rcArrow.CenterPoint().y;

	ptsArrow[0].x = x-4;
	ptsArrow[0].y = y-2;
	ptsArrow[1].x = x + 4;
	ptsArrow[1].y = y-2;
	ptsArrow[2].x = x;
	ptsArrow[2].y = y + 2;

	CXTPBrushDC dcBrush(pDC->GetSafeHdc(), crArrow);
	CXTPPenDC dcPen(pDC->GetSafeHdc(), crArrow);

	pDC->SetPolyFillMode(WINDING);
	pDC->Polygon(ptsArrow, 3);
}

void CXTColorPickerTheme::DrawColorPicker(LPDRAWITEMSTRUCT lpDIS, CXTColorPicker* pColorPicker)
{
	// draw the button arrow.
	CRect rcArrow(lpDIS->rcItem);
	rcArrow.left = rcArrow.right-XTAuxData().cxHThumb - 2;

	if (lpDIS->itemState & ODS_SELECTED)
	{
		rcArrow.OffsetRect(1, 1);
	}

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	DrawArrow(pDC, rcArrow, CXTButtonTheme::GetTextColor(lpDIS->itemState, pColorPicker));

	// draw the color box.
	CRect rcItem;
	rcItem.left = lpDIS->rcItem.left + 5;
	rcItem.right = lpDIS->rcItem.right - XTAuxData().cxHThumb;
	rcItem.top = lpDIS->rcItem.top + 5;
	rcItem.bottom = lpDIS->rcItem.bottom - 5;

	if ((lpDIS->itemState & ODS_SELECTED) || (pColorPicker->GetCheck() == 1))
		rcItem.OffsetRect(1, 1);

	// create and select the brush for the rectangle background.
	COLORREF clrRect = (lpDIS->itemState & ODS_DISABLED) ? GetXtremeColor(COLOR_3DFACE) : pColorPicker->GetColor();
	if (pColorPicker->IsTextShown() && pColorPicker->IsTextInColor())
		clrRect = pColorPicker->GetContrastColor();
	CXTPBrushDC dcBrush(pDC->m_hDC, clrRect);

	// create and select the pen for the rectangle border.
	COLORREF crPen = (lpDIS->itemState & ODS_DISABLED) ? GetXtremeColor(COLOR_3DSHADOW) : RGB(0, 0, 0);
	CXTPPenDC dcPen(pDC->m_hDC, crPen);

	// draw the color rectangle.
	pDC->Rectangle(rcItem);
	lpDIS->rcItem = rcItem;
}

BOOL CXTColorPickerTheme::DrawWinThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton)
{
	CXTColorPicker* pColorPicker = DYNAMIC_DOWNCAST(CXTColorPicker, pButton);
	ASSERT_VALID(pColorPicker);

	// set display flags based on state.
	if (pColorPicker->m_bPopupActive)
		lpDIS->itemState |= ODS_SELECTED;

	if (!CXTButtonTheme::DrawWinThemeBackground(lpDIS, pButton))
		return FALSE;

	DrawColorPicker(lpDIS, pColorPicker);

	return TRUE;
}

BOOL CXTColorPickerTheme::DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton)
{
	CXTColorPicker* pColorPicker = DYNAMIC_DOWNCAST(CXTColorPicker, pButton);
	ASSERT_VALID(pColorPicker);

	// set display flags based on state.
	if (pColorPicker->m_bPopupActive)
		lpDIS->itemState |= ODS_SELECTED;

	if (!CXTButtonTheme::DrawButtonThemeBackground(lpDIS, pButton))
		return FALSE;

	DrawColorPicker(lpDIS, pColorPicker);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DDX Routines

_XTP_EXT_CLASS void AFXAPI DDX_XTColorPicker(CDataExchange *pDX, int nIDC, COLORREF& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	ASSERT (hWndCtrl != NULL);

	CXTColorPicker* pColorPicker = (CXTColorPicker*) CWnd::FromHandle(hWndCtrl);
	if (pDX->m_bSaveAndValidate)
	{
		value = pColorPicker->GetColor();
	}
	else
	{
		pColorPicker->SetColor(value);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTColorPicker

CXTColorPicker::CXTColorPicker()
: m_dwPopup(CPS_XT_EXTENDED | CPS_XT_MORECOLORS)
, m_clrSelected(CLR_DEFAULT)
, m_clrDefault(CLR_DEFAULT)
, m_bShowText(FALSE)
, m_bColorText(FALSE)
, m_bPopupActive(FALSE)
{
	m_dwxStyle = BS_XT_SHOWFOCUS | BS_XT_WINXP_COMPAT;
	SetTheme(new CXTColorPickerTheme);
}

CXTColorPicker::~CXTColorPicker()
{

}

IMPLEMENT_DYNAMIC(CXTColorPicker, CXTButton)

BEGIN_MESSAGE_MAP(CXTColorPicker, CXTButton)
	//{{AFX_MSG_MAP(CXTColorPicker)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_XT_SELCHANGE, OnSelChange)
	ON_MESSAGE(CPN_XT_DROPDOWN, OnDropDown)
	ON_MESSAGE(CPN_XT_CLOSEUP, OnCloseUp)
	ON_MESSAGE(CPN_XT_SELENDOK, OnSelEndOK)
	ON_MESSAGE(CPN_XT_SELENDCANCEL, OnSelEndCancel)
	ON_MESSAGE(CPN_XT_SELNOFILL, OnSelNoFill)
	ON_MESSAGE_VOID(WM_XT_SHOWPOPUP, OnShowPopup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTColorPicker message handlers

LRESULT CXTColorPicker::OnSelChange(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	if (pOwner != NULL)
	{
		// Notify of selection change.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
			CPN_XT_SELCHANGE), (LPARAM)m_hWnd);
	}

	return 0;
}

LRESULT CXTColorPicker::OnDropDown(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	if (pOwner != NULL)
	{
		// Notify of popup activation.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
			CPN_XT_DROPDOWN), (LPARAM)m_hWnd);
	}

	return 0;
}

LRESULT CXTColorPicker::OnCloseUp(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_bPopupActive = FALSE;

	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	if (pOwner != NULL)
	{
		// Notify of popup close up.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
			CPN_XT_CLOSEUP), (LPARAM)m_hWnd);
	}

	Invalidate();
	return 0;
}

LRESULT CXTColorPicker::OnSelEndOK(WPARAM wParam, LPARAM /*lParam*/)
{
	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	if (pOwner != NULL)
	{
		SetColor((COLORREF)wParam);

		// Notify of popup close up.
		// NB: doing so may destroy the picker so lets copy off
		// the window handle/control ID for safety
		HWND hWndSender = m_hWnd;
		int nControlID = GetDlgCtrlID();
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(nControlID,
			CPN_XT_CLOSEUP), (LPARAM)hWndSender);

		// Notify of selection made.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(nControlID,
			CPN_XT_SELENDOK), (LPARAM)hWndSender);
	}

	return 0;
}

LRESULT CXTColorPicker::OnSelEndCancel(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	if (pOwner != NULL)
	{
		// Notify of popup close up.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
			CPN_XT_CLOSEUP), (LPARAM)m_hWnd);

		// Notify of cancel.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
			CPN_XT_SELENDCANCEL), (LPARAM)m_hWnd);
	}

	return 0;
}

LRESULT CXTColorPicker::OnSelNoFill(WPARAM wParam, LPARAM /*lParam*/)
{
	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	if (pOwner != NULL)
	{
		SetColor((COLORREF)wParam);

		// Notify of no fill press.
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),
			CPN_XT_SELNOFILL), (LPARAM)m_hWnd);
	}

	return 0;
}

bool CXTColorPicker::Init()
{
	if (!CXTButton::Init())
		return false;

	// Set the font.
	SetFont(&XTAuxData().font);

	return true;
}

void CXTColorPicker::SetColor(COLORREF clr)
{
	if (clr != GetColor())
	{
		m_clrSelected = clr;
		RedrawWindow();
	}
}

COLORREF CXTColorPicker::GetContrastColor() const
{
	double Intensity = CXTColorRef(GetColor()).getIntensity();

	return Intensity > 128 ? CXTColorRef::BLACK : CXTColorRef::WHITE;
}

void CXTColorPicker::ShowPopupWindow()
{
	m_bPopupActive = TRUE;

	// Make sure that we have input focus.
	if (GetFocus() != this)
	{
		SetFocus();
	}

	// Post a message instead of displaying right away - this will take care
	// of asynchronous focusing issues
	PostMessage(WM_XT_SHOWPOPUP);
}

void CXTColorPicker::OnShowPopup()
{
	// Get the size of the picker button.
	CRect rcWindow;
	GetWindowRect(&rcWindow);

	// Create the color popup window.
	CXTColorPopup *pColorPopup = new CXTColorPopup(TRUE);
	pColorPopup->Create(rcWindow, this, m_dwPopup, GetColor(), GetDefaultColor());
	pColorPopup->SetFocus();
}

BOOL CXTColorPicker::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (!m_bPopupActive)
	{
		if (message == WM_LBUTTONDOWN)
		{
			ShowPopupWindow();
			// in this case the message is not "swallowed" so the button will
			// get it and display itself in a recessed state
		}
		else if (message == WM_KEYDOWN &&
			(wParam == VK_RETURN ||
			wParam == VK_SPACE ||
			wParam == VK_DOWN))
		{
			ShowPopupWindow();
			return TRUE;  // swallow message
		}
	}

	return CXTButton::OnWndMsg(message, wParam, lParam, pResult);
}
