// XTTabView.cpp : implementation of the CXTTabView class.
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
#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTVC50Helpers.h"
#include "XTMemDC.h"
#include "XTTabCtrlButtons.h"
#include "XTTabBase.h"
#include "XTTabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTTabView

IMPLEMENT_THEME_REFRESH(CXTTabView, CCtrlView)
IMPLEMENT_DYNCREATE(CXTTabView, CCtrlView)

BEGIN_MESSAGE_MAP(CXTTabView, CCtrlView)
	//{{AFX_MSG_MAP(CXTTabView)
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
	ON_TABCTRLEX_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTTabView construction/destruction

CXTTabView::CXTTabView()
{
	m_bBoldFont = FALSE;
	ImplAttach(&GetTabCtrl());
}

CXTTabView::~CXTTabView()
{
}

bool CXTTabView::Init()
{
	if (!CXTTabViewBase::Init())
		return false;

	// Make sure the common controls library is initialized.
	::InitCommonControls();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTabView tooltip related functions

void CXTTabView::UpdateDocTitle()
{
	TCHAR szText[256];

	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.cchTextMax = _countof(szText);
	tci.pszText = szText;

	GetTabCtrl().GetItem(GetTabCtrl().GetCurSel(), &tci);
	GetDocument()->SetTitle(tci.pszText);
}

int CXTTabView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// If current active view is a child view, then do nothing.
	CFrameWnd* pParentFrame = GetParentFrame();
	CView* pView = pParentFrame ? pParentFrame->GetActiveView() : NULL;

	if (IsChildView(pView))
	{
		return MA_NOACTIVATE;
	}

	return CCtrlView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CXTTabView::OnInitialUpdate()
{
	CCtrlView::OnInitialUpdate();

	// Get a pointer to the active view.
	CView* pActiveView = DYNAMIC_DOWNCAST(CView, GetActiveView());
	if (pActiveView != NULL)
	{
		// Get a pointer to the parent frame.
		CFrameWnd* pParentFrame = GetParentFrame();
		if (pParentFrame != NULL)
		{
			// If the active view for the frame does not match the active
			// view for the tab control, set the frames active view.
			if (pParentFrame->GetActiveView() != pActiveView)
			{
				pParentFrame->SetActiveView(pActiveView);
			}
		}
	}
}
