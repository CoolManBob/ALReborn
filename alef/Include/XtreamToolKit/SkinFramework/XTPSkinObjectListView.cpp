// XTPSkinObjectListView.cpp: implementation of the CXTPSkinObjectListView class.
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

#include "XTPSkinManager.h"
#include "XTPSkinObjectListView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectListView, CXTPSkinObjectFrame)

CXTPSkinObjectListView::CXTPSkinObjectListView()
{
	m_strClassName = _T("LISTVIEW");
	m_nCtlColorMessage = WM_CTLCOLORLISTBOX;
}

CXTPSkinObjectListView::~CXTPSkinObjectListView()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectListView, CXTPSkinObjectComCtl32Control)
	//{{AFX_MSG_MAP(CXTPSkinObjectListView)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectListView message handlers

void CXTPSkinObjectListView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL && m_pManager->IsWin9x() && (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK))
	{
		BOOL bListView = (GetStyle() & LVS_REPORT) == LVS_LIST;
		DWORD dwIconSpacing = (DWORD)SendMessage(LVM_GETITEMSPACING, TRUE);

		int dx = int((int)nPos - GetScrollPos(SB_HORZ)) * (!bListView ? 1 : LOWORD(dwIconSpacing));

		SendMessage(LVM_SCROLL, dx, 0);
		return;
	}

	CXTPSkinObjectComCtl32Control::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CXTPSkinObjectListView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL && m_pManager->IsWin9x() && (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK))
	{
		DWORD dwIconSpacing = (DWORD)SendMessage(LVM_GETITEMSPACING, TRUE);

		int dy = int((int)nPos - GetScrollPos(SB_VERT)) * HIWORD(dwIconSpacing);

		SendMessage(LVM_SCROLL, 0, dy);
		return;
	}

	CXTPSkinObjectComCtl32Control::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CXTPSkinObjectListView::OnSkinChanged(BOOL bPrevState, BOOL bNewState)
{
	SendMessage(LVM_SETBKCOLOR, 0, bNewState ? GetColor(COLOR_WINDOW) : GetSysColor(COLOR_WINDOW));
	SendMessage(LVM_SETTEXTCOLOR, 0, bNewState ? GetColor(COLOR_WINDOWTEXT) : GetSysColor(COLOR_WINDOWTEXT));
	SendMessage(LVM_SETTEXTBKCOLOR, 0, bNewState ? GetColor(COLOR_WINDOW) : GetSysColor(COLOR_WINDOW));

	CXTPSkinObjectFrame::OnSkinChanged(bPrevState, bNewState);
}

int CXTPSkinObjectListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (GetSkinManager()->IsEnabled())
	{
		SendMessage(LVM_SETBKCOLOR, 0, GetColor(COLOR_WINDOW));
		SendMessage(LVM_SETTEXTBKCOLOR, 0, GetColor(COLOR_WINDOW));
		SendMessage(LVM_SETTEXTCOLOR, 0, GetColor(COLOR_WINDOWTEXT));
	}

	return 0;
}
