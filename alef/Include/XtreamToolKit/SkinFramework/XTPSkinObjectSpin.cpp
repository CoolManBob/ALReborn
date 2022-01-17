// XTPSkinObjectSpin.cpp: implementation of the CXTPSkinObjectSpin class.
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

#include "Common/XTPDrawHelpers.h"

#include "XTPSkinObjectSpin.h"
#include "XTPSkinManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define UD_HITNOWHERE 0
#define UD_HITDOWN 1
#define UD_HITUP 2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectSpin, CXTPSkinObjectFrame)

CXTPSkinObjectSpin::CXTPSkinObjectSpin()
{
	m_strClassName = _T("SPIN");

	m_nHotButton = UD_HITNOWHERE;
	m_nPressedButton = UD_HITNOWHERE;
}

CXTPSkinObjectSpin::~CXTPSkinObjectSpin()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectSpin, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectSpin)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectSpin message handlers


BOOL CXTPSkinObjectSpin::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}




UINT CXTPSkinObjectSpin::HitTest(CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);

	if (!rc.PtInRect(pt))
		return UD_HITNOWHERE;

	if (GetStyle() & UDS_HORZ)
	{
		// Horizontal placement
		if (pt.x < (rc.right / 2))
		{
			return UD_HITDOWN;
		}
		else if (pt.x > (rc.right / 2))
		{
			return UD_HITUP;
		}
	}
	else
	{
		if (pt.y > (rc.bottom / 2))
		{
			return UD_HITDOWN;
		}
		else if (pt.y < (rc.bottom / 2))
		{
			return UD_HITUP;
		}
	}

	return UD_HITNOWHERE;
}

LRESULT CXTPSkinObjectSpin::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0)
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectSpin::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CXTPSkinObjectSpin::OnDraw(CDC* pDC)
{

	CRect rcClient;
	GetClientRect(&rcClient);

	CXTPBufferDC dcMem(*pDC, rcClient);

	dcMem.FillSolidRect(rcClient, GetColor(COLOR_3DFACE));

	DWORD dwStyle = GetStyle();

	int nLower = 0, nUpper = 0;
	SendMessage(UDM_GETRANGE32, (WPARAM) &nLower, (LPARAM) &nUpper);
	BOOL bEnabled = (nUpper != nLower) && IsWindowEnabled();

	CRect rcBtn = rcClient;

	CXTPSkinManagerClass* pClassSpin = GetSkinClass();


	HWND hWndBuddy = (HWND)SendMessage(UDM_GETBUDDY);
	if (hWndBuddy && GetWindowLong(hWndBuddy, GWL_EXSTYLE) & WS_EX_CLIENTEDGE)
	{
		if (dwStyle & UDS_ALIGNRIGHT)
		{
			rcBtn.DeflateRect(0, 1, 1, 1);

			CXTPSkinManagerClass* pClassEdit = GetSkinManager()->GetSkinClass(_T("EDIT"));
			COLORREF clrBorderColor = pClassEdit->GetThemeColor(0, 0, TMT_BORDERCOLOR);
			dcMem.Draw3dRect(rcClient, clrBorderColor, clrBorderColor);
			dcMem.FillSolidRect(rcClient.left, rcClient.top + 1, 1, rcClient.Height() - 2, GetColor(COLOR_WINDOW));
		}
		else if (dwStyle & UDS_ALIGNLEFT)
		{
			rcBtn.DeflateRect(1, 1, 0, 1);

			CXTPSkinManagerClass* pClassEdit = GetSkinManager()->GetSkinClass(_T("EDIT"));
			COLORREF clrBorderColor = pClassEdit->GetThemeColor(0, 0, TMT_BORDERCOLOR);
			dcMem.Draw3dRect(rcClient, clrBorderColor, clrBorderColor);
			dcMem.FillSolidRect(rcClient.right - 1, rcClient.top + 1, 1, rcClient.Height() - 2, GetColor(COLOR_WINDOW));
		}
	}

	RECT rc = rcBtn;

	if (dwStyle & UDS_HORZ)
	{
		int nState = DNHZS_NORMAL;
		if (!bEnabled)
			nState = DNHZS_DISABLED;
		else if (m_nPressedButton == UD_HITDOWN && m_nHotButton == UD_HITDOWN)
			nState = DNHZS_PRESSED;
		else if ((m_nHotButton == UD_HITDOWN || m_nPressedButton == UD_HITDOWN)  && (m_nPressedButton != UD_HITUP))
			nState = DNHZS_HOT;

		rc.right = (rcBtn.right + rcBtn.left) / 2;
		pClassSpin->DrawThemeBackground(&dcMem, SPNP_DOWNHORZ, nState, &rc);


		nState = UPHZS_NORMAL;
		if (!bEnabled)
			nState = UPHZS_DISABLED;
		else if (m_nPressedButton == UD_HITUP && m_nHotButton == UD_HITUP)
			nState = UPHZS_PRESSED;
		else if ((m_nHotButton == UD_HITUP || m_nPressedButton == UD_HITUP) && (m_nPressedButton != UD_HITDOWN))
			nState = UPHZS_HOT;

		rc.left = rcBtn.right - (rc.right - rc.left); // handles odd-x case, too
		rc.right = rcBtn.right;
		pClassSpin->DrawThemeBackground(&dcMem, SPNP_UPHORZ, nState, &rc);

	}
	else
	{
		int nState = UPS_NORMAL;
		if (!bEnabled)
			nState = UPS_DISABLED;
		else if (m_nPressedButton == UD_HITUP && m_nHotButton == UD_HITUP)
			nState = UPS_PRESSED;
		else if ((m_nHotButton == UD_HITUP || m_nPressedButton == UD_HITUP) && (m_nPressedButton != UD_HITDOWN))
			nState = UPS_HOT;

		rc.bottom = (rcBtn.bottom + rcBtn.top) / 2;

		pClassSpin->DrawThemeBackground(&dcMem, SPNP_UP, nState, &rc);

		nState = DNS_NORMAL;
		if (!bEnabled)
			nState = DNS_DISABLED;
		else if (m_nPressedButton == UD_HITDOWN && m_nHotButton == UD_HITDOWN)
			nState = DNS_PRESSED;
		else if ((m_nHotButton == UD_HITDOWN || m_nPressedButton == UD_HITDOWN)  && (m_nPressedButton != UD_HITUP))
			nState = DNS_HOT;

		rc.top = rcBtn.bottom - (rc.bottom - rc.top); // handles odd-y case, too
		rc.bottom = rcBtn.bottom;
		pClassSpin->DrawThemeBackground(&dcMem, SPNP_DOWN, nState, &rc);
	}
}

void CXTPSkinObjectSpin::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	UINT uHot = HitTest(point);

	if (uHot != m_nHotButton)
	{
		m_nHotButton = uHot;
		Invalidate(FALSE);

		if (m_nHotButton != HTNOWHERE)
		{
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT};
			_TrackMouseEvent(&tme);
		}
	}
}

void CXTPSkinObjectSpin::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectSpin::OnLButtonDown(UINT nFlags, CPoint point)
{
	CXTPSkinObjectFrame::OnLButtonDown(nFlags, point);

	UINT uHot = HitTest(point);

	if (uHot != HTNOWHERE)
	{
		m_nPressedButton = uHot;
		Invalidate(FALSE);
	}

}

void CXTPSkinObjectSpin::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nPressedButton != HTNOWHERE)
	{
		m_nPressedButton = HTNOWHERE;
		Invalidate(FALSE);
	}

	CXTPSkinObjectFrame::OnLButtonUp(nFlags, point);
}

void CXTPSkinObjectSpin::OnTimer(UINT_PTR nIDEvent)
{
	CXTPSkinObjectFrame::OnTimer(nIDEvent);
}
