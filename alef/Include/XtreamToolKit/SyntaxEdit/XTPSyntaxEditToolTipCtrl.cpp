// XTPSyntaxEditToolTipCtrl.cpp
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// common includes
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSystemHelpers.h"

// syntax editor includes
#include "XTPSyntaxEditToolTipCtrl.h"
#include "XTPSyntaxEditDefines.h"
#include "XTPSyntaxEditStruct.h"
#include "XTPSyntaxEditPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT XTP_TIP_TIMER_ID = 1001;

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditToolTipCtrl

CXTPSyntaxEditToolTipCtrl::CXTPSyntaxEditToolTipCtrl()
: m_pParentWnd(NULL)
, m_nDelayTime(5000)
{
	RegisterWindowClass();
}

CXTPSyntaxEditToolTipCtrl::~CXTPSyntaxEditToolTipCtrl()
{
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CXTPSyntaxEditToolTipCtrl, CWnd)
	//{{AFX_MSG_MAP(CXTPSyntaxEditToolTipCtrl)
	ON_WM_NCHITTEST_EX()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXTPSyntaxEditToolTipCtrl::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	WNDCLASS wndcls;
	if (hInstance == NULL) hInstance = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInstance, XTP_EDIT_CLASSNAME_TOOLTIP, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_VREDRAW | CS_HREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInstance;
		wndcls.hIcon = NULL;
		wndcls.hCursor = ::LoadCursor(0, IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_INFOBK + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = XTP_EDIT_CLASSNAME_TOOLTIP;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditToolTipCtrl message handlers

LRESULT CXTPSyntaxEditToolTipCtrl::OnNcHitTest(CPoint point)
{
	UNREFERENCED_PARAMETER(point);
	return (LRESULT)HTTRANSPARENT;
}

BOOL CXTPSyntaxEditToolTipCtrl::Create(CWnd * pParentWnd)
{
	ASSERT_VALID(pParentWnd);
	m_pParentWnd = pParentWnd;

	// Already created?
	if (::IsWindow(m_hWnd))
		return TRUE;

	// first, create a tool tip window
	if (!CWnd::CreateEx(WS_EX_TOOLWINDOW, XTP_EDIT_CLASSNAME_TOOLTIP, NULL,
		WS_POPUP | WS_BORDER | WS_CLIPSIBLINGS, CXTPEmptyRect(), NULL, 0))
	{
		TRACE0("Failed to create tooltip window.\n");
		return FALSE;
	}

	return TRUE;
}

void CXTPSyntaxEditToolTipCtrl::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rc(this);

	dc.SetBkColor(::GetSysColor(COLOR_INFOBK));
	dc.SetTextColor(::GetSysColor(COLOR_INFOTEXT));

	CXTPFontDC font(&dc, XTPSyntaxEditPaintManager()->GetFontToolTip());
	CRect rcText(rc);
	rcText.left += 3;
	rcText.top += 3;

	dc.DrawText(m_strToolTipText, rcText,
		DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS);
}

void CXTPSyntaxEditToolTipCtrl::ReCalcToolTipRect()
{
	CWindowDC dc(NULL);

	CXTPFontDC font(&dc, XTPSyntaxEditPaintManager()->GetFontToolTip());

	CRect rcDesktop = XTPMultiMonitor()->GetWorkArea();
	CRect rc(0,0,(rcDesktop.right-m_rcHover.left)-10,20);
	dc.DrawText(m_strToolTipText, rc,
		DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS);

	m_rcHover.right = m_rcHover.left + rc.Width() + 8;
	m_rcHover.bottom = m_rcHover.top + rc.Height() + 8;

	MoveWindow(&m_rcHover);
}

void CXTPSyntaxEditToolTipCtrl::Hide()
{
	// stop existing timer.
	KillTimer(XTP_TIP_TIMER_ID);

	if (::IsWindowVisible(m_hWnd))
	{
		ShowWindow(SW_HIDE);
	}
}

void CXTPSyntaxEditToolTipCtrl::Activate(const CString& strText)
{
	BOOL bVisible = ::IsWindowVisible(m_hWnd);

	// stop existing timer.
	KillTimer(XTP_TIP_TIMER_ID);

	// hide the window if the tip text has changed.
	if (bVisible && (m_strToolTipText != strText))
	{
		ShowWindow(SW_HIDE);
	}

	// set the tip text.
	m_strToolTipText = strText;

	if (!bVisible)
	{
		ReCalcToolTipRect();
		ShowWindow(SW_SHOWNA);
		Invalidate();
	}

	// set timer.
	SetTimer(XTP_TIP_TIMER_ID, m_nDelayTime, NULL);
}

void CXTPSyntaxEditToolTipCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == XTP_TIP_TIMER_ID)
	{
		Hide();
		return;
	}

	CWnd::OnTimer(nIDEvent);
}
