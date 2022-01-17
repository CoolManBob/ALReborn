// XTResizeFormView.cpp: implementation of the CXTResizeFormView class.
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

#include "XTResizeRect.h"
#include "XTResizePoint.h"
#include "XTResize.h"
#include "XTResizeFormView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable: 4355) // 'this' : used in base member initializer list

/////////////////////////////////////////////////////////////////////////////
// CXTResizeFormView

IMPLEMENT_DYNCREATE(CXTResizeFormView, CFormView)

BEGIN_MESSAGE_MAP(CXTResizeFormView, CFormView)
	//{{AFX_MSG_MAP(CXTResizeFormView)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTResizeFormView construction/destruction

CXTResizeFormView::CXTResizeFormView(const UINT nID, const UINT nFlags) :
	CFormView(nID),
	CXTResize(this, nFlags | xtResizeNoSizeIcon)
{

}

void CXTResizeFormView::OnGetMinMaxInfo(MINMAXINFO* pMMI)
{
	// Notice that WM_GETMINMAXINFO must be handled in the parent frame
	CXTResize::GetMinMaxInfo(pMMI);
}



void CXTResizeFormView::AjustResizeRect(CRect& rcWindow)
{
	CSize szMin = m_totalLog;
	rcWindow.right = rcWindow.left + max(rcWindow.Width(), szMin.cx);
	rcWindow.bottom = rcWindow.top + max(rcWindow.Height(), szMin.cy);


	DWORD dwStyle = GetStyle();
	CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
	BOOL bHasVertBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_VSCROLL);

	pBar = GetScrollBarCtrl(SB_HORZ);
	BOOL bHasHorzBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_HSCROLL);

	if (bHasVertBar)
		rcWindow.right -= GetSystemMetrics(SM_CXVSCROLL);

	if (bHasHorzBar)
		rcWindow.bottom -= GetSystemMetrics(SM_CYHSCROLL);
}


void CXTResizeFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CXTResize::Size();
}

void CXTResizeFormView::OnDestroy()
{
	// Save window placement if AutoLoadPlacement was used.
	if (!m_strSection.IsEmpty())
	{
		SavePlacement(m_strSection);
	}

	CFormView::OnDestroy();
}

LRESULT CXTResizeFormView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = CFormView::WindowProc(message, wParam, lParam);

	if (message == WM_INITDIALOG)
	{
		OnInitDialog();
	}

	return lRet;
}

BOOL CXTResizeFormView::OnInitDialog()
{
	CXTResize::Init();

	if (!UpdateData(FALSE))
		TRACE0("UpdateData failed during CXTResizeFormView::OnInitDialog().\n");

	return TRUE;
}
