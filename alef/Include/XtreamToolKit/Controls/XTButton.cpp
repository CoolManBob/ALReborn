// XTButton.cpp : implementation of the CXTButton class.
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
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTFunctions.h"
#include "XTMemDC.h"
#include "XTHelpers.h"
#include "XTThemeManager.h"
#include "XTButtonTheme.h"
#include "XTFunctions.h"
#include "XTButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WM_QUERYUISTATE
#define WM_UPDATEUISTATE    0x0128
#define WM_QUERYUISTATE     0x0129

#define UISF_HIDEACCEL      0x2
#define UISF_HIDEFOCUS      0x1
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTButton
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTButton, CButton)

IMPLEMENT_THEME_HOST(CXTButton)
IMPLEMENT_THEME_REFRESH(CXTButton, CButton)

CXTButton::CXTButton(CRuntimeClass* pThemeFactoryClass/*=NULL*/)
: CXTThemeManagerStyleHost(pThemeFactoryClass ? pThemeFactoryClass : GetThemeFactoryClass())
, m_nBorderGap(4)
, m_nImageGap(8)
, m_bUserPosition(false)
, m_bChecked(false)
, m_dwxStyle(BS_XT_FLAT | BS_XT_SHOWFOCUS)
, m_sizeImage(CSize(0, 0))
, m_ptImage(CPoint(0, 0))
, m_ptText(CPoint(0, 0))
, m_pIcon(NULL)
, m_bPreSubclassInit(true)
{
	m_bOwnerDraw = FALSE;
	m_bPushed = FALSE;
	m_bHot = FALSE;
}


CXTButton::~CXTButton()
{
	CleanUpGDI();

	CMDTARGET_RELEASE(m_pIcon);
}

HICON CXTButton::GetNormalIcon()
{
	return m_pIcon ? (HICON)m_pIcon->GetIcon() : NULL;
}

HICON CXTButton::GetSelectedIcon()
{
	return m_pIcon ? (HICON)m_pIcon->GetHotIcon() : NULL;
}


BEGIN_MESSAGE_MAP(CXTButton, CButton)
	//{{AFX_MSG_MAP(CXTButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(BM_SETSTYLE, OnDefaultAndInvalidate)
	ON_MESSAGE(BM_SETCHECK, OnDefaultAndInvalidate)
	ON_MESSAGE(WM_CAPTURECHANGED, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(WM_ENABLE, OnInvalidate)
	ON_MESSAGE(WM_UPDATEUISTATE, OnUpdateUIState)
	ON_MESSAGE(BM_SETSTATE, OnSetState)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	//}}AFX_MSG_MAP
	ON_WM_GETDLGCODE()
	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
END_MESSAGE_MAP()

void CXTButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	GetTheme()->DrawItem(lpDIS, this);
}

void CXTButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetButtonStyle() == BS_GROUPBOX)
		return;

	CButton::OnMouseMove(nFlags, point);

	CRect rc;
	GetClientRect(&rc);

	BOOL bHot = rc.PtInRect(point);
	if (bHot != m_bHot)
	{
		m_bHot = bHot;
		Invalidate(FALSE);

		if (m_bHot)
		{
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
		}
	}
}

LRESULT CXTButton::OnSetState(WPARAM wParam, LPARAM /*lParam*/)
{
	BOOL bVisible = GetStyle() & WS_VISIBLE;
	if (bVisible) ModifyStyle(WS_VISIBLE, 0);
	Default();
	if (bVisible) ModifyStyle(0, WS_VISIBLE);

	m_bPushed = (wParam != 0);
	Invalidate(FALSE);

	return 0;
}

void CXTButton::OnSetFocus(CWnd* pOldWnd)
{
	CButton::OnSetFocus(pOldWnd);
	Invalidate(FALSE);
}

void CXTButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);
	Invalidate(FALSE);
}

LRESULT CXTButton::OnDefaultAndInvalidate(WPARAM, LPARAM)
{
	LRESULT lResult = Default();
	Invalidate(FALSE);

	return lResult;
}

void CXTButton::OnInvalidate()
{
	Invalidate(FALSE);
}

LRESULT CXTButton::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0)
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTButton::OnPaint()
{
	CPaintDC dcPaint(this);
	CXTPBufferDC dc(dcPaint); // device context for painting
	OnDraw(&dc);
}

void CXTButton::OnDraw(CDC* pDC)
{
	DRAWITEMSTRUCT dis;
	ZeroMemory(&dis, sizeof(DRAWITEMSTRUCT));

	dis.CtlType = ODT_BUTTON;
	dis.CtlID = GetDlgCtrlID();
	dis.itemState = 0;

	if (::GetFocus() == m_hWnd) dis.itemState |= ODS_FOCUS;
	if (m_bPushed) dis.itemState |= ODS_SELECTED;

	LRESULT dwState = SendMessage(WM_QUERYUISTATE);

	if (dwState & UISF_HIDEACCEL) dis.itemState |= ODS_NOACCEL;
	if (dwState & UISF_HIDEFOCUS) dis.itemState |= ODS_NOFOCUSRECT;
	if (dwState & UISF_HIDEFOCUS) dis.itemState |= ODS_NOFOCUSRECT;

	if (!IsWindowEnabled()) dis.itemState |= ODS_DISABLED;

	dis.hwndItem = m_hWnd;
	dis.hDC = pDC->GetSafeHdc();
	dis.rcItem = CXTPClientRect(this);

	DrawItem(&dis);
}

BYTE CXTButton::GetButtonStyle() const
{
	BYTE bStyle = BYTE(GetStyle() & 0xF);
	return bStyle;
}

LRESULT CXTButton::OnUpdateUIState(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = ::DefWindowProc(m_hWnd, WM_UPDATEUISTATE, wParam, lParam);
	Invalidate(FALSE);

	return lResult;
}

LRESULT CXTButton::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = ::DefWindowProc(m_hWnd, WM_SETTEXT, wParam, lParam);
	Invalidate(FALSE);

	return lResult;
}

BOOL CXTButton::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

bool CXTButton::Init()
{
	m_bOwnerDraw = m_hWnd && ((GetStyle() & 0xF) == BS_OWNERDRAW);
	if (m_hWnd) Invalidate(FALSE);

	return true;
}

void CXTButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();

	if (m_bPreSubclassInit)
	{
		// Initialize the control.
		Init();
	}
}

int CXTButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize the control.
	Init();

	return 0;
}

BOOL CXTButton::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CButton::PreCreateWindow(cs))
		return FALSE;

	// When creating controls dynamically Init() must
	// be called from OnCreate() and not from
	// PreSubclassWindow().

	m_bPreSubclassInit = false;

	return TRUE;
}


UINT CXTButton::OnGetDlgCode()
{
	if (m_bOwnerDraw)
	{
		if (GetFocus() == this)
			return DLGC_DEFPUSHBUTTON | DLGC_BUTTON;

		return DLGC_BUTTON ;
	}
	return CButton::OnGetDlgCode();
}

LRESULT CXTButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	if (m_bOwnerDraw)
	{
		DWORD dwOldStyle = GetStyle();

		ModifyStyle(0, (long)wParam | BS_OWNERDRAW,0);

		if ((dwOldStyle != GetStyle()) && LOWORD(lParam))
			Invalidate();

		return 0;
	}

	return OnDefaultAndInvalidate(wParam, lParam);
}

void CXTButton::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

//////////////////////////////////////////////////////////////////////////
//

CSize CXTButton::GetImageSize() const
{
	if (!GetTheme()->IsIconVisible())
		return CSize(0, 0);

	return m_sizeImage;
}

CPoint CXTButton::GetImagePoint() const
{
	if (!GetTheme()->IsIconVisible())
		return CPoint(0, 0);

	return m_ptImage;
}

CPoint CXTButton::GetTextPoint() const
{
	if (!GetTheme()->IsIconVisible())
		return CPoint(0, 0);

	return m_ptText;
}

void CXTButton::CleanUpGDI()
{
	if (m_pIcon)
	{
		m_pIcon->Refresh();
	}
}

BOOL CXTButton::SetIcon(CSize size, UINT nID, UINT nHotID/*= 0*/, BOOL bRedraw/*= TRUE*/)
{
	return CXTButton::SetIcon(
		size, MAKEINTRESOURCE(nID), MAKEINTRESOURCE(nHotID), bRedraw);
}

BOOL CXTButton::SetIcon(CSize size, LPCTSTR lpszID, LPCTSTR lpszHotID/*= NULL*/, BOOL bRedraw/*= TRUE*/)
{
	CXTPImageManagerIconHandle hIcon;
	CXTPImageManagerIconHandle hIconHot;

	hIcon.CreateIconFromResource(lpszID, size);

	// Return false if the icon handle is NULL.
	if (hIcon.IsEmpty())
	{
		TRACE1("Failed to load Icon resource %s.\n", lpszID);
		return FALSE;
	}

	// If we are using a pushed image as well...
	if (lpszHotID)
	{
		hIconHot.CreateIconFromResource(lpszHotID, size);

		// Return false if the icon handle is NULL.
		if (hIconHot.IsEmpty())
		{
			TRACE0("Failed to load Icon resource.\n");
			return FALSE;
		}
	}

	return CXTButton::SetIcon(size, hIcon, hIconHot, bRedraw);
}

BOOL CXTButton::SetIcon(CSize size, CXTPImageManagerIcon* pIcon, BOOL bRedraw/*= TRUE*/)
{
	m_sizeImage = size;

	// Construct the icon manager.
	CMDTARGET_RELEASE(m_pIcon);

	m_pIcon = pIcon;

	// Redraw the button.
	if (::IsWindow(m_hWnd) && bRedraw)
		Invalidate();

	return TRUE;
}

BOOL CXTButton::SetIcon(CSize size, CXTPImageManagerIconHandle hIcon, CXTPImageManagerIconHandle hIconHot, BOOL bRedraw/*= TRUE*/)
{
	ASSERT(!hIcon.IsEmpty());

	// Save the image size and set the icon
	// handles to NULL.
	size = size != CSize(0) ? size : hIcon.GetExtent();

	CXTPImageManagerIcon* pIcon = new CXTPImageManagerIcon(0, size.cx, size.cy);
	pIcon->SetIcon(hIcon);

	if (!hIconHot.IsEmpty())
		pIcon->SetHotIcon(hIconHot);

	return SetIcon(size, pIcon, bRedraw);
}


BOOL CXTButton::SetIcon(CSize size, HICON hIcon, HICON hIconHot/*= NULL*/, BOOL bRedraw/*= TRUE*/)
{
	return SetIcon(size, CXTPImageManagerIconHandle(hIcon), CXTPImageManagerIconHandle(hIconHot), bRedraw);
}

BOOL CXTButton::SetBitmap(CSize size, UINT nID, BOOL bRedraw/*= TRUE*/)
{
	// Free previous resources (if any).

	CXTPImageManagerIconHandle hIconHandle;

	BOOL bAlphaBitmap = FALSE;
	HBITMAP hBitmap = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(nID), &bAlphaBitmap);

	if (!hBitmap)
		return FALSE;

	if (bAlphaBitmap)
	{
		hIconHandle = hBitmap; // Will call DeleteObject;
	}
	else
	{
		CBitmap bmpIcon;
		bmpIcon.Attach(hBitmap);

		// convert the bitmap to a transparent icon.
		HICON hIcon = CXTPTransparentBitmap(bmpIcon).ConvertToIcon();

		hIconHandle = hIcon; // Will call DestoyIcon;
	}

	if (hIconHandle.IsEmpty())
		return FALSE;

	return CXTButton::SetIcon(size, hIconHandle, CXTPImageManagerIconHandle(), bRedraw);
}

BOOL CXTButton::SetTextAndImagePos(CPoint ptImage, CPoint ptText, BOOL bRedraw/*= TRUE*/)
{
	m_ptImage = ptImage;
	m_ptText = ptText;

	// Redraw the button.
	if (::IsWindow(m_hWnd) && bRedraw)
		Invalidate();

	m_bUserPosition = TRUE;
	return TRUE;
}

DWORD CXTButton::SetXButtonStyle(DWORD dwxStyle, BOOL bRedraw/*= TRUE*/)
{
	DWORD dwOldStyle = m_dwxStyle;
	m_dwxStyle = dwxStyle;

	if ((m_dwxStyle & BS_XT_XPFLAT) == BS_XT_XPFLAT)
		SetTheme(new CXTButtonThemeOfficeXP());
	else if ((m_dwxStyle & BS_XT_FLAT) == BS_XT_FLAT)
		SetTheme(new CXTButtonTheme());

	// Redraw the button.
	if (::IsWindow(m_hWnd) && bRedraw)
		Invalidate();

	return dwOldStyle;
}


BOOL CXTButton::IsThemeValid() const
{
	return (GetTheme() != NULL);
}

CXTPImageManagerIcon* CXTButton::GetIcon() const
{
	return m_pIcon;
}

CString CXTButton::GetButtonText(BOOL bRemoveAmpersand)
{
	CString strText;
	GetWindowText(strText);

	if (bRemoveAmpersand)
		XTPDrawHelpers()->StripMnemonics(strText);

	return strText;
}

void CXTButton::SetAlternateColors(COLORREF clr3DFace, COLORREF clr3DHilight, COLORREF clr3DShadow, COLORREF clrBtnText)
{
	if (IsThemeValid())
	{
		GetTheme()->SetAlternateColors(
			clr3DFace, clr3DHilight, clr3DShadow, clrBtnText);
	}
}

void CXTButton::SetColorFace(COLORREF clrFace)
{
	if (IsThemeValid())
	{
		GetTheme()->SetColorFace(clrFace);
	}
}

COLORREF CXTButton::GetColorFace()
{
	if (IsThemeValid())
	{
		return GetTheme()->GetColorFace();
	}
	return XTAuxData().clr3DFace;
}

void CXTButton::SetColorHilite(COLORREF clrHilite)
{
	if (IsThemeValid())
	{
		GetTheme()->SetColorHilite(clrHilite);
	}
}

void CXTButton::SetColorShadow(COLORREF clrShadow)
{
	if (IsThemeValid())
	{
		GetTheme()->SetColorShadow(clrShadow);
	}
}

void CXTButton::SetColorText(COLORREF clrText)
{
	if (IsThemeValid())
	{
		GetTheme()->SetColorText(clrText);
	}
}

void CXTButton::SetXPFlatColors(COLORREF clrBorder, COLORREF clrHighlight, COLORREF clrPressed)
{
	if (IsThemeValid())
	{
		CXTButtonTheme* pButtonTheme = GetTheme();
		if (pButtonTheme->GetTheme() == xtThemeOfficeXP)
		{
			((CXTButtonThemeOfficeXP*)pButtonTheme)->SetColorHilite(clrBorder);
			((CXTButtonThemeOfficeXP*)pButtonTheme)->SetColorShadow(clrBorder);
			((CXTButtonThemeOfficeXP*)pButtonTheme)->SetBackHiliteColor(clrHighlight);
			((CXTButtonThemeOfficeXP*)pButtonTheme)->SetBackPushedColor(clrPressed);
		}
	}
}

void CXTButton::ResetImage()
{
	if (IsThemeValid())
	{
		if (GetTheme()->IsIconVisible())
		{
			GetTheme()->ShowIcon(FALSE);
		}
		else
		{
			GetTheme()->ShowIcon(TRUE);
		}
		Invalidate();
	}
}

CFont* CXTButton::SetFontEx(CFont* pFont)
{
	CFont* pOldFont = GetFont();
	CButton::SetFont(pFont);
	return pOldFont;
}
