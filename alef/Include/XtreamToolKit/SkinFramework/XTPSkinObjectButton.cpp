// XTPSkinObjectButton.cpp: implementation of the CXTPSkinObjectButton class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
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

#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPSkinObjectButton.h"
#include "XTPSkinDrawTools.h"
#include "XTPSkinManager.h"

#define DT_HIDEPREFIX       0x00100000

#define CBR_CLIENTRECT 0
#define CBR_CHECKBOX   1
#define CBR_RADIOBOX   2
#define CBR_CHECKTEXT  3
#define CBR_RADIOTEXT  4
#define CBR_GROUPTEXT  5
#define CBR_GROUPFRAME 6
#define CBR_PUSHBUTTON 7


#define BS_HORZMASK   (BS_LEFT | BS_RIGHT | BS_CENTER)
#define BS_VERTMASK   (BS_TOP | BS_BOTTOM | BS_VCENTER)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CXTPSkinObjectButton, CXTPSkinObjectFrame)

CXTPSkinObjectButton::CXTPSkinObjectButton()
{
	m_bHot = FALSE;
	m_bPushed = FALSE;

	m_bStyle = (BYTE)0xFF;

	m_strClassName = _T("BUTTON");
	m_nCtlColorMessage = WM_CTLCOLORBTN;
}

CXTPSkinObjectButton::~CXTPSkinObjectButton()
{

}

BEGIN_MESSAGE_MAP(CXTPSkinObjectButton, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE_VOID(BM_SETSTYLE, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(BM_SETCHECK, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(WM_CAPTURECHANGED, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(WM_ENABLE, OnInvalidate)
	ON_MESSAGE(WM_UPDATEUISTATE, OnUpdateUIState)
	ON_MESSAGE(BM_SETSTATE, OnSetState)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectButton message handlers

int CXTPSkinObjectButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	Invalidate(FALSE);

	return 0;
}

void CXTPSkinObjectButton::OnSkinChanged(BOOL bPrevState, BOOL bNewState)
{
	CXTPSkinObjectFrame::OnSkinChanged(bPrevState, bNewState);

	m_bHot = FALSE;
	m_bPushed = FALSE;

	if (!bPrevState)
	{
		m_bPushed = FALSE;
	}
}


BOOL CXTPSkinObjectButton::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPSkinObjectButton::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetButtonStyle() == BS_GROUPBOX)
		return;

	CXTPSkinObjectFrame::OnMouseMove(nFlags, point);

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

BOOL CXTPSkinObjectButton::IsDefWindowProcAvail(int nMessage) const
{
	if (m_bWindowsForms)
		return FALSE;

	if (nMessage == WM_TIMER)
		return FALSE;

	if (m_bActiveX)
		return TRUE;

	return CXTPSkinObjectFrame::IsDefWindowProcAvail(nMessage);
}

LRESULT CXTPSkinObjectButton::OnSetState(WPARAM wParam, LPARAM /*lParam*/)
{
	BOOL bVisible = GetStyle() & WS_VISIBLE;
	if (bVisible) ModifyStyle(WS_VISIBLE, 0);
	Default();
	if (bVisible) ModifyStyle(0, WS_VISIBLE);

	m_bPushed = (wParam != 0);

	Invalidate(FALSE);

	return 0;
}

void CXTPSkinObjectButton::OnSetFocus(CWnd* pOldWnd)
{
	CXTPSkinObjectFrame::OnSetFocus(pOldWnd);
	Invalidate(FALSE);
}

void CXTPSkinObjectButton::OnKillFocus(CWnd* pNewWnd)
{
	CXTPSkinObjectFrame::OnKillFocus(pNewWnd);
	Invalidate(FALSE);
}

void CXTPSkinObjectButton::OnDefaultAndInvalidate()
{
	Default();
	Invalidate(FALSE);
}

void CXTPSkinObjectButton::OnInvalidate()
{
	Invalidate(FALSE);
}


LRESULT CXTPSkinObjectButton::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0)
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectButton::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CXTPSkinObjectButton::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	if ((GetButtonStyle() == BS_GROUPBOX) && !m_bActiveX && !m_bWindowsForms)
	{
		CFont* pOldFont = pDC->SelectObject(GetFont());
		pDC->SetBkColor(GetColor(COLOR_3DFACE));

		OnDraw(pDC, rc);

		pDC->SelectObject(pOldFont);
	}
	else
	{
		CXTPBufferDC memDC(*pDC, rc);

		FillBackground(&memDC, rc, WM_CTLCOLORBTN);

		CFont* pOldFont = memDC.SelectObject(GetFont());
		memDC.SetBkMode(TRANSPARENT);

		OnDraw(&memDC, rc);

		memDC.SelectObject(pOldFont);
	}
}

BOOL CXTPSkinObjectButton::IsPushButton()
{
	DWORD dwStyle = GetStyle();

	switch (GetButtonStyle())
	{
		case LOBYTE(BS_PUSHBUTTON):
		case LOBYTE(BS_DEFPUSHBUTTON):
		case LOBYTE(BS_OWNERDRAW):
			return TRUE;

		default:
			if ((dwStyle & BS_PUSHLIKE) == BS_PUSHLIKE)
				return TRUE;
			break;
	}

	return FALSE;
}


DWORD CXTPSkinObjectButton::GetAlignment()
{
	DWORD bHorz;
	DWORD bVert;

	DWORD dwStyle = GetStyle();

	bHorz = dwStyle & (BS_LEFT | BS_RIGHT | BS_CENTER);
	bVert = dwStyle & (BS_TOP | BS_BOTTOM | BS_VCENTER);

	if (!bHorz || !bVert)
	{
		if (IsPushButton())
		{
			if (!bHorz)
				bHorz = BS_CENTER;
		}
		else
		{
			if (!bHorz)
				bHorz = BS_LEFT;
		}

		if (!bVert)
			bVert = BS_VCENTER;
	}

	if (m_bActiveX)
		bVert = BS_VCENTER;

	return bHorz | bVert;
}


CSize CXTPSkinObjectButton::GetGlyphSize(BOOL bCheckBox)
{
	CXTPSkinManagerClass* pClassButton = GetSkinClass();
	CRect rcButton(0, 0, 13, 13);
	CSize sz(13, 13);

	pClassButton->GetThemePartSize(bCheckBox ? BP_CHECKBOX : BP_RADIOBUTTON,
		1, rcButton, TS_DRAW, &sz);

	return sz;
}

void CXTPSkinObjectButton::CalcRect(CDC* pDC, LPRECT lprc, int code)
{
	CString strText;
	GetWindowText(strText);
	int dy;
	UINT align;
	CSize extent;

	GetClientRect(lprc);

	align = GetAlignment();

	switch (code)
	{
	case CBR_PUSHBUTTON:
		// Subtract out raised edge all around
		InflateRect(lprc, -GetMetrics()->m_cxEdge, -GetMetrics()->m_cyEdge);
		break;

	case CBR_CHECKBOX:
	case CBR_RADIOBOX:
		{
			CSize szGlyph = GetGlyphSize(code == CBR_CHECKBOX);

			switch (align & BS_VERTMASK)
			{
			case BS_VCENTER:
				lprc->top = (lprc->top + lprc->bottom - szGlyph.cy) / 2;
				break;

			case BS_TOP:
			case BS_BOTTOM:
				{
					extent = pDC->GetTextExtent(_T(" "), 1);
					dy = extent.cy + extent.cy/4;

					// Save vertical extent
					extent.cx = dy;

					// Get centered amount

					dy = (dy - szGlyph.cy) / 2;
					if ((align & BS_VERTMASK) == BS_TOP)
						lprc->top += dy;
					else
						lprc->top = lprc->bottom - extent.cx + dy;
					break;
				}
			}
			lprc->bottom = lprc->top + szGlyph.cy;

			if (GetStyle() & BS_RIGHTBUTTON)
				lprc->left = lprc->right - szGlyph.cx;
			else
				lprc->right = lprc->left + szGlyph.cx;
		}
		break;

	case CBR_CHECKTEXT:
	case CBR_RADIOTEXT:
		{
			int nOffset = GetGlyphSize(code == CBR_CHECKBOX).cx + 3;
			if (GetStyle() & BS_RIGHTBUTTON)
			{
				lprc->right -= nOffset;
			}
			else
			{
				lprc->left += nOffset;
			}
		}
		break;

	case CBR_GROUPTEXT:
		if (strText.IsEmpty())
		{
			SetRectEmpty(lprc);
			break;
		}

		extent = pDC->GetTextExtent(strText);
		extent.cx += GetMetrics()->m_cxEdge * 2;

		switch (align & BS_HORZMASK)
		{
		case BS_RIGHT:
			lprc->right -= 6 + GetMetrics()->m_cxBorder;
			lprc->left += lprc->right - (int)(extent.cx);
			break;
		default:
			lprc->left += 8 + GetMetrics()->m_cxBorder;
			lprc->right = lprc->left + (int)(extent.cx);
		}

		lprc->bottom = lprc->top + extent.cy;
		break;

	case CBR_GROUPFRAME:
		extent = pDC->GetTextExtent(_T(" "), 1);
		lprc->top += extent.cy / 2;
		break;
	}
}

void CXTPSkinObjectButton::DrawButtonText(CDC* pDC)
{
	static const BYTE buttonsStyles[] =
	{
		CBR_PUSHBUTTON,
		CBR_PUSHBUTTON,
		CBR_CHECKTEXT,
		CBR_CHECKTEXT,
		CBR_RADIOTEXT,
		CBR_CHECKTEXT,
		CBR_CHECKTEXT,
		CBR_GROUPTEXT,
		CBR_CLIENTRECT,
		CBR_RADIOTEXT,
		CBR_CLIENTRECT,
		CBR_PUSHBUTTON,
	};

	RECT    rc;
	int     x, y, cx = 0, cy = 0;

	CString strText;
	UINT dsFlags = DT_SINGLELINE | DT_LEFT | DT_TOP | DT_NOCLIP;
	BYTE bStyle = GetButtonStyle();

	BOOL pbfPush = IsPushButton();
	if (pbfPush)
	{
		CalcRect(pDC, &rc, CBR_PUSHBUTTON);
	}
	else
	{
		CalcRect(pDC, &rc, buttonsStyles[bStyle]);
	}

	// Alignment
	GetWindowText(strText);
	UINT wAlignment = GetAlignment();

	HGDIOBJ hImage = 0;

	if (GetStyle() & BS_BITMAP)
	{
		hImage = (HGDIOBJ)SendMessage(BM_GETIMAGE, IMAGE_BITMAP);
		if (!hImage)
			return;

		BITMAP bmp;
		GetObject(hImage, sizeof(BITMAP), &bmp);
		cx = bmp.bmWidth;
		cy = bmp.bmHeight;
	}
	else if (GetStyle() & BS_ICON)
	{
		hImage = (HGDIOBJ)SendMessage(BM_GETIMAGE, IMAGE_ICON);
		if (!hImage)
			return;

		XTPSkinFrameworkGetIconSize((HICON)hImage, &cx, &cy);
	}
	else
	{
		// Text button
		if (strText.IsEmpty())
			return;

		if (GetStyle() & BS_MULTILINE)
		{
			dsFlags |= DT_WORDBREAK | DT_EDITCONTROL;
			dsFlags &= ~DT_SINGLELINE;
		}


		CRect rcText(0, 0, rc.right - rc.left, 0);
		pDC->DrawText(strText, rcText, dsFlags | DT_CALCRECT);

		cx = rcText.Width();
		cy = rcText.Height();
	}

	CRect rcText(rc);
	if (pbfPush && ((GetStyle() & BS_MULTILINE) == 0)) rcText.DeflateRect(GetMetrics()->m_cxBorder, GetMetrics()->m_cyBorder);

	// Horizontal
	switch (wAlignment & BS_HORZMASK)
	{
		case BS_LEFT:
			x = rc.left + (pbfPush ? GetMetrics()->m_cxBorder : 0);
			break;

		case BS_RIGHT:
			x = rc.right - cx -  (pbfPush ? GetMetrics()->m_cxBorder : 0);
			dsFlags |= DT_RIGHT;
			break;

		default:
			x = (rc.left + rc.right - cx) / 2;
			dsFlags |= DT_CENTER;
			break;
	}

	// Vertical
	switch (wAlignment & BS_VERTMASK)
	{
		case BS_TOP:
			y = rc.top + (pbfPush ? GetMetrics()->m_cyBorder : 0);
			break;

		case BS_BOTTOM:
			y = rc.bottom - cy - (pbfPush ? GetMetrics()->m_cyBorder : 0);
			dsFlags |= DT_BOTTOM;
			break;

		default:
			y = (rc.top + rc.bottom - cy) / 2;
			dsFlags |= DT_VCENTER;
			break;
	}

	if (GetStyle() & BS_BITMAP)
	{
		pDC->DrawState(CPoint(x, y), CSize(cx, cy), (HBITMAP)hImage, DSS_NORMAL, 0);
	}
	else if (GetStyle() & BS_ICON)
	{
		pDC->DrawState(CPoint(x, y), CSize(cx, cy), (HICON)hImage, DSS_NORMAL, (HBRUSH)0);
	}
	else if (bStyle !=  LOBYTE(BS_USERBUTTON))
	{
		if (SendMessage(WM_QUERYUISTATE) & UISF_HIDEACCEL)
		{
			dsFlags |= DT_HIDEPREFIX;
		}

		if (GetExStyle() & WS_EX_RTLREADING)
			dsFlags |= DT_RTLREADING;

		if (GetStyle() & BS_MULTILINE)
		{
			pDC->DrawText(strText, CRect(x, y, x + cx, y + cy), dsFlags | DT_NOCLIP);
		}
		else
		{
			pDC->DrawText(strText, rcText, dsFlags);
		}
	}

	if (::GetFocus() == m_hWnd)
	{
		if ((SendMessage(WM_QUERYUISTATE) & UISF_HIDEFOCUS) == 0)
		{
			if (!pbfPush)
			{

				RECT rcClient;
				GetClientRect(&rcClient);

				if (bStyle == LOBYTE(BS_USERBUTTON))
					CopyRect(&rc, &rcClient);
				else
				{
					// Try to leave a border all around text.  That causes
					// focus to hug text.
					rc.top = max(rcClient.top, y - GetMetrics()->m_cyBorder);
					rc.bottom = min(rcClient.bottom, rc.top + GetMetrics()->m_cyEdge + cy);

					rc.left = max(rcClient.left, x-GetMetrics()->m_cxBorder);
					rc.right = min(rcClient.right, rc.left + GetMetrics()->m_cxEdge + cx);
				}
			}
			else
				InflateRect(&rc, -GetMetrics()->m_cxBorder, -GetMetrics()->m_cyBorder);

			pDC->SetTextColor(0);
			pDC->SetBkColor(0xFFFFFF);
			// Are back & fore colors set properly?
			::DrawFocusRect(pDC->GetSafeHdc(), &rc);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectButton message handlers

BYTE CXTPSkinObjectButton::GetButtonStyle()
{
	if (m_bStyle != 0xFF)
		return m_bStyle;

	BYTE bStyle = BYTE(GetStyle() & 0xF);
	return bStyle;
}

void CXTPSkinObjectButton::OnDraw(CDC* pDC, CRect rc)
{
	BOOL bPressed = m_bPushed;
	BOOL bHot = m_bHot || (::GetCapture() == m_hWnd);
	BYTE bStyle = GetButtonStyle();
	BOOL bDefault = bStyle == BS_DEFPUSHBUTTON;
	BOOL bEnabled = IsWindowEnabled();
	BOOL bChecked = (BOOL)SendMessage(BM_GETCHECK);

	CXTPSkinManagerClass* pClassButton = GetSkinClass();

	if (IsPushButton())
	{
		int nState = !bEnabled? PBS_DISABLED: bPressed? PBS_PRESSED: bHot? PBS_HOT: bChecked ? PBS_PRESSED : bDefault? PBS_DEFAULTED: PBS_NORMAL;
		pClassButton->DrawThemeBackground(pDC, BP_PUSHBUTTON, nState, rc);
		pDC->SetTextColor(pClassButton->GetThemeColor(BP_PUSHBUTTON, nState, TMT_TEXTCOLOR));
	}
	else if (bStyle == BS_RADIOBUTTON || bStyle == BS_AUTORADIOBUTTON)
	{
		int nState = !bEnabled? RBS_UNCHECKEDDISABLED: bPressed? RBS_UNCHECKEDPRESSED: bHot? RBS_UNCHECKEDHOT: RBS_UNCHECKEDNORMAL;
		if (bChecked) nState += 4;


		pDC->SetTextColor(pClassButton->GetThemeColor(BP_RADIOBUTTON, nState, TMT_TEXTCOLOR, GetColor(COLOR_BTNTEXT)));

		CRect rcRadio;
		CalcRect(pDC, &rcRadio, CBR_RADIOBOX);
		pClassButton->DrawThemeBackground(pDC, BP_RADIOBUTTON, nState, rcRadio);
	}
	else if (bStyle == BS_CHECKBOX || bStyle == BS_AUTOCHECKBOX || bStyle == BS_AUTO3STATE || bStyle == BS_3STATE)
	{
		int nState = !bEnabled? RBS_UNCHECKEDDISABLED: bPressed? RBS_UNCHECKEDPRESSED: bHot? RBS_UNCHECKEDHOT: RBS_UNCHECKEDNORMAL;
		if (bChecked)
			nState += 4;
		if ((bChecked == BST_INDETERMINATE) && (bStyle == BS_AUTO3STATE || bStyle == BS_3STATE))
			nState += 4;

		CRect rcRadio;
		CalcRect(pDC, &rcRadio, CBR_CHECKBOX);

		pClassButton->DrawThemeBackground(pDC, BP_CHECKBOX, nState, rcRadio);

		pDC->SetTextColor(pClassButton->GetThemeColor(BP_CHECKBOX, nState, TMT_TEXTCOLOR, GetColor(COLOR_BTNTEXT)));
	}
	else if (bStyle == BS_GROUPBOX)
	{
		int nState = !bEnabled? GBS_DISABLED: GBS_NORMAL;
		CRect rcGroupBox;
		CalcRect(pDC, &rcGroupBox, CBR_GROUPFRAME);

		pClassButton->DrawThemeBackground(pDC, BP_GROUPBOX, nState, rcGroupBox);

		CRect rcGrouText;
		CalcRect(pDC, &rcGrouText, CBR_GROUPTEXT);

		FillBackground(pDC, rcGrouText, WM_CTLCOLORBTN);

		pDC->SetTextColor(pClassButton->GetThemeColor(BP_GROUPBOX, nState, TMT_TEXTCOLOR, GetColor(COLOR_BTNTEXT)));

	}
	else
	{
	}

	DrawButtonText(pDC);
}

LRESULT CXTPSkinObjectButton::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CXTPSkinObjectFrame::OnSetText(wParam, lParam);

	Invalidate(FALSE);

	return lResult;
}

BOOL CXTPSkinObjectButton::OnHookMessage(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	if (GetButtonStyle() >= BS_OWNERDRAW)
	{
		return FALSE;
	}
	return CXTPSkinObjectFrame::OnHookMessage(nMessage, wParam, lParam, lResult);
}

BOOL CXTPSkinObjectButton::OnHookDefWindowProc(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	if (GetButtonStyle() >= BS_OWNERDRAW)
	{
		return FALSE;
	}

	if (!m_bActiveX && wParam && (nMessage == WM_PAINT || nMessage == WM_PRINTCLIENT))
	{
		OnDraw(CDC::FromHandle((HDC)wParam));
		return TRUE;
	}

	return CXTPSkinObjectFrame::OnHookDefWindowProc(nMessage, wParam, lParam, lResult);
}

LRESULT CXTPSkinObjectButton::OnUpdateUIState(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = ::DefWindowProc(m_hWnd, WM_UPDATEUISTATE, wParam, lParam);
	Invalidate(FALSE);

	return lResult;
}
