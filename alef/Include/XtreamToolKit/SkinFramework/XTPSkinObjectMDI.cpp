// XTPSkinObjectMDI.cpp: implementation of the CXTPSkinObjectMDI class.
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

#include "XTPSkinObjectMDI.h"
#include "XTPSkinManager.h"


#define MM_CALCSCROLL            0x003F

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectMDIClient, CXTPSkinObjectFrame)

CXTPSkinObjectMDIClient::CXTPSkinObjectMDIClient()
{
	m_bMDIClient = TRUE;
	m_bArrange = FALSE;

	m_pMDIParent = NULL;
}

CXTPSkinObjectMDIClient::~CXTPSkinObjectMDIClient()
{

}

BEGIN_MESSAGE_MAP(CXTPSkinObjectMDIClient, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectButton)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_MESSAGE(MM_CALCSCROLL, OnCalcScroll)
	ON_WM_NCACTIVATE()
	ON_WM_SIZE()
	ON_MESSAGE_VOID(WM_MDIICONARRANGE, OnMDIArrange)
	ON_MESSAGE_VOID(WM_MDICASCADE, OnMDIArrange)
	ON_MESSAGE_VOID(WM_MDITILE, OnMDIArrange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CXTPSkinObjectMDIClient::OnHookAttached(LPCREATESTRUCT lpcs, BOOL bAuto)
{
	CXTPSkinObjectFrame* pParent = (CXTPSkinObjectFrame*)GetSkinManager()->Lookup(lpcs->hwndParent);
	if (pParent)
	{
		m_pMDIParent = pParent;
		m_pMDIParent->m_pMDIClient = this;
		m_pMDIParent->InternalAddRef();
	}

	CXTPSkinObjectFrame::OnHookAttached(lpcs, bAuto);
}

void CXTPSkinObjectMDIClient::OnHookDetached(BOOL bAuto)
{
	if (m_pMDIParent)
	{
		m_pMDIParent->m_pMDIClient = NULL;

		m_pMDIParent->InternalRelease();
		m_pMDIParent = NULL;
	}

	CXTPSkinObjectFrame::OnHookDetached(bAuto);
}

BOOL CXTPSkinObjectMDIClient::IsDefWindowProcAvail(int nMessage) const
{
	return (UINT)nMessage == m_nMsgQuerySkinState;
}

void CXTPSkinObjectMDIClient::OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	Default();
	RedrawScrollBar(SB_HORZ);
}

void CXTPSkinObjectMDIClient::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	Default();
	RedrawScrollBar(SB_VERT);
}

LRESULT CXTPSkinObjectMDIClient::OnCalcScroll(WPARAM, LPARAM)
{

	LRESULT lResult = Default();
	RedrawScrollBar(SB_VERT);
	RedrawScrollBar(SB_HORZ);

	return lResult;
}

BOOL CXTPSkinObjectMDIClient::OnNcActivate(BOOL bActive)
{
	BOOL bResult = CXTPSkinObjectFrame::OnNcActivate(bActive);
	RedrawScrollBar(SB_VERT);
	RedrawScrollBar(SB_HORZ);
	return bResult;
}

void CXTPSkinObjectMDIClient::OnSize(UINT nType, int cx, int cy)
{
	if ((m_pManager->GetApplyOptions() & xtpSkinApplyMetrics))
	{
		HWND hwndActive = (HWND)SendMessage(WM_MDIGETACTIVE);
		if (hwndActive && ::GetWindowLongPtr(hwndActive, GWL_STYLE) & WS_MAXIMIZE)
		{
			CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)m_pManager->Lookup(hwndActive);

			if (pFrame && pFrame->IsSkinEnabled())
			{
				CRect rcBorders = pFrame->GetBorders();
				RECT rc;
				rc.left = -rcBorders.left;
				rc.top = -rcBorders.top;
				rc.right = cx + rcBorders.right;
				rc.bottom = cy + rcBorders.bottom;

				::MoveWindow(hwndActive, rc.left, rc.top,
					rc.right - rc.left, rc.bottom - rc.top, TRUE);
				return;
			}
		}
	}

	CXTPSkinObjectFrame::OnSize(nType, cx, cy);
}

void CXTPSkinObjectMDIClient::OnMDIArrange()
{
	if (m_bArrange)
	{
		Default();
		return;
	}
	else
	{
		m_bArrange = TRUE;

		HWND hWnd = ::GetWindow(m_hWnd, GW_CHILD);
		while (hWnd)
		{
			CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)m_pManager->Lookup(hWnd);
			if (pFrame)
			{
				RECT rc = CXTPWindowRect(hWnd);
				pFrame->m_bLockFrameDraw += 1;
				pFrame->SendMessage(WM_NCCALCSIZE, FALSE, (LPARAM)&rc);
				pFrame->m_bLockFrameDraw -= 1;

				pFrame->SetTimer(XTP_TID_REFRESHFRAME, 1000, NULL);
			}

			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}

		Default();

		m_bArrange = FALSE;

		hWnd = ::GetWindow(m_hWnd, GW_CHILD);
		while (hWnd)
		{
			CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)m_pManager->Lookup(hWnd);
			if (pFrame)
			{
				pFrame->RedrawFrame();
			}

			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}
	}
}
