// XTPSkinObjectProgress.cpp: implementation of the CXTPSkinObjectProgress class.
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

#include "XTPSkinObjectProgress.h"
#include "XTPSkinManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectProgress
IMPLEMENT_DYNCREATE(CXTPSkinObjectProgress, CXTPSkinObjectFrame)

CXTPSkinObjectProgress::CXTPSkinObjectProgress()
{
	m_strClassName = _T("PROGRESS");
	m_bStaticEdgeRemoved = FALSE;
}

CXTPSkinObjectProgress::~CXTPSkinObjectProgress()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectProgress, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectProgress)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectProgress message handlers


int CXTPSkinObjectProgress::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (GetExStyle() & WS_EX_STATICEDGE)
	{
		ModifyStyleEx(WS_EX_STATICEDGE, 0);
		SetWindowPos(NULL, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

		m_bStaticEdgeRemoved = TRUE;
	}

	return 0;
}

void CXTPSkinObjectProgress::OnSkinChanged(BOOL bPrevState, BOOL bNewState)
{
	CXTPSkinObjectFrame::OnSkinChanged(bPrevState, bNewState);

	BOOL bRemovedStaticEdge = m_pManager->IsEnabled() || CXTPWinThemeWrapper().IsThemeActive();

	if (m_bStaticEdgeRemoved != bRemovedStaticEdge)
	{
		if (bRemovedStaticEdge)
			ModifyStyleEx(WS_EX_STATICEDGE, 0);
		else
			ModifyStyleEx(0, WS_EX_STATICEDGE);

		SetWindowPos(NULL, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		m_bStaticEdgeRemoved = bRemovedStaticEdge;
	}
}

BOOL CXTPSkinObjectProgress::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

LRESULT CXTPSkinObjectProgress::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0)
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC) OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectProgress::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CXTPSkinObjectProgress::OnDraw(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CXTPBufferDC dcMem(*pDC, rcClient);

	dcMem.FillSolidRect(rcClient, GetColor(COLOR_3DFACE));

	DWORD dwStyle = GetStyle();
	BOOL bVert = dwStyle & PBS_VERTICAL;

	CXTPSkinManagerClass* pClassProgress = GetSkinClass();
	pClassProgress->DrawThemeBackground(&dcMem, dwStyle & PBS_VERTICAL ? PP_BARVERT : PP_BAR, 0,  rcClient);

	CRect rc = rcClient;

	if (bVert)
		rc.DeflateRect(3, 4);
	else
		rc.DeflateRect(4, 3);

	int iStart, iEnd, dxBlock;

	PBRANGE range;
	SendMessage(PBM_GETRANGE, (WPARAM) FALSE, (LPARAM) &range);

	int iLow = range.iLow;
	int iHigh = range.iHigh;
	int iPos =  (int)SendMessage(PBM_GETPOS);


	if (bVert)
	{
		iStart = rcClient.top;
		iEnd = rcClient.bottom;
		dxBlock = (rcClient.right - rcClient.left) * 2 / 3;
	}
	else
	{
		iStart = rcClient.left;
		iEnd = rcClient.right;
		dxBlock = (rcClient.bottom - rcClient.top) * 2 / 3;
	}

	int x = MulDiv(iEnd - iStart, iPos - iLow, iHigh - iLow);

	dxBlock = pClassProgress->GetThemeInt(0, 0, TMT_PROGRESSCHUNKSIZE, dxBlock);
	int dxSpace = pClassProgress->GetThemeInt(0, 0, TMT_PROGRESSSPACESIZE, 2);


	if (dxBlock == 0)
		dxBlock = 1;

	int nBlocks = (x + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace);

	for (int i = 0; i < nBlocks; i++)
	{
		if (bVert)
		{
			rc.top = rc.bottom - dxBlock;

			if (rc.bottom <= rcClient.top)
				break;

			if (rc.top <= rcClient.top)
				rc.top = rcClient.top + 1;

		}
		else
		{
			rc.right = rc.left + dxBlock;

			if (rc.left >= rcClient.right)
				break;

			if (rc.right >= rcClient.right)
				rc.right = rcClient.right - 1;
		}

		pClassProgress->DrawThemeBackground(&dcMem, dwStyle & PBS_VERTICAL ? PP_CHUNKVERT : PP_CHUNK, 0,  rc);


		if (bVert)
			rc.bottom = rc.top - dxSpace;
		else
			rc.left = rc.right + dxSpace;
	}
}
