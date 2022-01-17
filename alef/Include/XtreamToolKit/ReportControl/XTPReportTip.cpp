// XTPReportTip.cpp : implementation of the CXTPReportTip class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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

#include "XTPReportControl.h"
#include "XTPReportDefines.h"
#include "XTPReportTip.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPReportTip

CXTPReportTip::CXTPReportTip()
{
	m_pParentWnd = NULL;
	m_pItem = NULL;
	m_nRowIndex = -1;
}

CXTPReportTip::~CXTPReportTip()
{
}


BEGIN_MESSAGE_MAP(CXTPReportTip, CWnd)
	//{{AFX_MSG_MAP(CXTPReportTip)
	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST_EX()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPReportTip message handlers

LRESULT CXTPReportTip::OnNcHitTest(CPoint /*point*/)
{
	return (UINT)HTTRANSPARENT;
}

BOOL CXTPReportTip::Create(CWnd* pParentWnd)
{
	ASSERT_VALID(pParentWnd);

	// Already created ?
	if (m_hWnd)
		return TRUE;

	m_pParentWnd = pParentWnd;
	return CWnd::CreateEx(WS_EX_TOOLWINDOW, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, WS_POPUP,
		CXTPEmptyRect(), 0, NULL);
}


void CXTPReportTip::Activate(BOOL bActive)
{
	if (bActive)
	{
		CString strText = m_strTooltipText;
		CWindowDC dc(this);
		CXTPFontDC font(&dc, &m_fntToolTip);
		CXTPWindowRect rc (this);
		rc.right = rc.left + dc.GetTextExtent(strText).cx + 6;

		if ((m_pParentWnd->GetExStyle() & WS_EX_RTLREADING) || (m_pParentWnd->GetExStyle() & WS_EX_LAYOUTRTL))
		{
			rc.OffsetRect(-rc.right + m_rcHover.right + 1, 0);
		}

		CRect rcWork = XTPMultiMonitor()->GetWorkArea();
		if (rc.right > rcWork.right)
			rc.OffsetRect(rcWork.right - rc.right, 0);
		if (rc.left < rcWork.left)
			rc.OffsetRect(rcWork.left - rc.left, 0);
		MoveWindow(rc);
		Invalidate(FALSE);

	}
	SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, (bActive ? SWP_SHOWWINDOW : SWP_HIDEWINDOW | SWP_NOZORDER) | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE);

}

BOOL CXTPReportTip::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CXTPReportTip::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rc(this);

	COLORREF clrText = GetXtremeColor(COLOR_INFOTEXT);
	dc.FillSolidRect(rc, GetXtremeColor(COLOR_INFOBK));
	dc.Draw3dRect(rc, clrText, clrText);

	dc.SetTextColor(clrText);
	dc.SetBkMode(TRANSPARENT);

	if ((m_pParentWnd->GetExStyle() & WS_EX_RTLREADING) || (m_pParentWnd->GetExStyle() & WS_EX_LAYOUTRTL))
	{
		dc.SetTextAlign(TA_RTLREADING);
	}


	CXTPFontDC font(&dc, &m_fntToolTip);
	CRect rcText(rc);
	rcText.left += 3;
	rcText.top -= 2;

	dc.DrawText(m_strTooltipText, rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

}
