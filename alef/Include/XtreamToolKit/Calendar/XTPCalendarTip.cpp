// XTPCalendarTip.cpp
//
// This file is a part of the XTREME CALENDAR MFC class library.
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
#include "Common/XTPSystemHelpers.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarTip

CXTPCalendarTip::CXTPCalendarTip()
{
	m_pParentWnd = NULL;
	m_pFont = NULL;
	m_rcHover.SetRect(0, 0, 0, 0);
}

CXTPCalendarTip::~CXTPCalendarTip()
{}


BEGIN_MESSAGE_MAP(CXTPCalendarTip, CWnd)
//{{AFX_MSG_MAP(CXTPCalendarTip)
ON_WM_MOUSEMOVE()
ON_WM_NCHITTEST_EX()
ON_WM_ERASEBKGND()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarTip message handlers

LRESULT CXTPCalendarTip::OnNcHitTest(CPoint /*point*/)
{
	return (LRESULT)HTTRANSPARENT;
}

BOOL CXTPCalendarTip::Create(CWnd* pParentWnd)
{
	ASSERT_VALID(pParentWnd);

	// Already created ?
	if (m_hWnd)
		return TRUE;

	m_pParentWnd = pParentWnd;
	return CWnd::CreateEx(0, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, WS_POPUP,
						  CXTPEmptyRect(), pParentWnd, NULL);
}


void CXTPCalendarTip::Activate(BOOL bActive)
{
	if (bActive)
	{
		CString strText = m_strTooltipText;
		CWindowDC dc(this);
		CXTPFontDC font(&dc, m_pFont);
		CXTPWindowRect rc (this);
		rc.right = rc.left + dc.GetTextExtent(strText).cx + 6;

		CRect rcWork = XTPMultiMonitor()->GetWorkArea(m_pParentWnd);
		if (rc.right > rcWork.right)
			rc.OffsetRect(rcWork.right - rc.right, 0);
		//      MoveWindow(rc);
		Invalidate(FALSE);
	}

	ShowWindow(bActive ? SW_SHOWNOACTIVATE : SW_HIDE);

}

BOOL CXTPCalendarTip::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CXTPCalendarTip::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rc(this);

	COLORREF clrText = GetSysColor(COLOR_INFOTEXT);
	dc.FillSolidRect(rc, GetSysColor(COLOR_INFOBK));
	dc.Draw3dRect(rc, RGB(241, 239, 226), RGB(113, 111, 100));

	dc.SetTextColor(clrText);
	dc.SetBkMode(TRANSPARENT);

	CXTPFontDC font(&dc, m_pFont);
	CRect rcText(rc);
	rcText.left += 3;
	rcText.top += 3;

	dc.DrawText(m_strTooltipText, rcText, DT_WORDBREAK | DT_NOPREFIX);
}

CSize CXTPCalendarTip::CalcToolTipRect()
{
	CWindowDC dc(NULL);

	CRect rc(0, 0, 152-6, 20);

	CXTPFontDC font(&dc, m_pFont);

	dc.DrawText(m_strTooltipText, rc, DT_CALCRECT | DT_WORDBREAK);

	return CSize(152, rc.Height()+6);
}
