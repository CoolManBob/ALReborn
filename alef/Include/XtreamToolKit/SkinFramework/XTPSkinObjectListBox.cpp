// XTPSkinObjectListBox.cpp: implementation of the CXTPSkinObjectListBox class.
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

#include "XTPSkinObjectListBox.h"
#include "XTPSkinManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CXTPSkinObjectUser32Control::CXTPSkinObjectUser32Control()
{
	ZeroMemory(&m_si, sizeof(m_si));
}

void CXTPSkinObjectUser32Control::CheckScrollBarsDraw()
{
	if (m_pManager->IsComCtlV6())
		return;

	if (!IsWindow(m_hWnd))
		return;

	for (int i = 0; i < 2; i++)
	{
		if (m_spi[i].fVisible)
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);

			GetScrollInfo(i, &si, SIF_POS | SIF_RANGE);

			if (si.nPos != m_si[i].nPos || si.nMax != m_si[i].nMax || si.nMin != m_si[i].nMin)
			{
				m_si[i] = si;
				RedrawScrollBar(&m_spi[i]);
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CXTPSkinObjectUser32Control, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectUser32Control)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CXTPSkinObjectUser32Control::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = CXTPSkinObjectFrame::OnEraseBkgnd(pDC);

	CheckScrollBarsDraw();

	return bResult;

}

void CXTPSkinObjectUser32Control::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSkinObjectFrame::OnKeyDown(nChar, nRepCnt, nFlags);
	CheckScrollBarsDraw();
}

void CXTPSkinObjectUser32Control::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSkinObjectFrame::OnChar(nChar, nRepCnt, nFlags);
	CheckScrollBarsDraw();
}

void CXTPSkinObjectUser32Control::OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	Default();
	CheckScrollBarsDraw();
}

void CXTPSkinObjectUser32Control::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	Default();
	CheckScrollBarsDraw();
}

BOOL CXTPSkinObjectUser32Control::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bResult = CXTPSkinObjectFrame::OnWndMsg(message, wParam, lParam, pResult);

	if (message == WM_MOUSEWHEEL)
	{
		CheckScrollBarsDraw();
	}

	return bResult;
}

void CXTPSkinObjectUser32Control::OnWindowPosChanged(WINDOWPOS FAR* /*lpwndpos*/)
{
	Default();
	CheckScrollBarsDraw();
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectComCtl32Control

CXTPSkinObjectComCtl32Control::CXTPSkinObjectComCtl32Control()
{
}
void CXTPSkinObjectComCtl32Control::CheckScrollBarsDraw()
{
	if (!m_pManager->IsWin9x())
		return;

	CXTPSkinObjectUser32Control::CheckScrollBarsDraw();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CXTPSkinObjectListBox, CXTPSkinObjectFrame)

CXTPSkinObjectListBox::CXTPSkinObjectListBox()
{
	m_strClassName = _T("LISTBOX");
	m_nCtlColorMessage = WM_CTLCOLORLISTBOX;
}

CXTPSkinObjectListBox::~CXTPSkinObjectListBox()
{

}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectListBox message handlers
