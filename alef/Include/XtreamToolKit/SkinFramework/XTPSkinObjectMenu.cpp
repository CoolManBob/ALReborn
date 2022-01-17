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

#include "XTPSkinObjectMenu.h"
#include "XTPSkinManager.h"
#include "XTPSkinDrawTools.h"

#define MN_SELECTITEM 0x01E5
#define MN_GETHMENU   0x01E1

POINT CXTPSkinObjectFrame::m_sMenuOffset = {0, 0};


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectMenu, CXTPSkinObjectFrame)

CXTPSkinObjectMenu::CXTPSkinObjectMenu()
{
}

CXTPSkinObjectMenu::~CXTPSkinObjectMenu()
{

}

BEGIN_MESSAGE_MAP(CXTPSkinObjectMenu, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectMenu)
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_PRINT, OnPrint)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_MESSAGE(MN_SELECTITEM, OnSelectItem)

	ON_MESSAGE(WM_KEYDOWN, OnKeyDown)
	ON_MESSAGE(WM_SYSKEYDOWN, OnKeyDown)
	ON_MESSAGE(WM_CHAR, OnChar)
	ON_WM_WINDOWPOSCHANGING()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTPSkinObjectMenu::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
	lpwndpos->y += m_sMenuOffset.y;
	lpwndpos->x += m_sMenuOffset.x;

	CXTPSkinObjectFrame::OnWindowPosChanging(lpwndpos);
}

void CXTPSkinObjectMenu::OnPaint()
{
	CXTPSkinObjectPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

#define MIIM_BITMAP      0x00000080
struct MENUITEMINFO98
{
	UINT    cbSize;
	UINT    fMask;
	UINT    fType;          // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
	UINT    fState;         // used if MIIM_STATE
	UINT    wID;            // used if MIIM_ID
	HMENU   hSubMenu;       // used if MIIM_SUBMENU
	HBITMAP hbmpChecked;    // used if MIIM_CHECKMARKS
	HBITMAP hbmpUnchecked;  // used if MIIM_CHECKMARKS
	DWORD   dwItemData;     // used if MIIM_DATA
	LPWSTR  dwTypeData;     // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
	UINT    cch;            // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
	HBITMAP hbmpItem;       // used if MIIM_BITMAP
};

void CXTPSkinObjectMenu::OnDraw(CDC* pDC)
{

	/*NONCLIENTMETRICS nm;
	::ZeroMemory(&nm, sizeof(NONCLIENTMETRICS));
	nm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &nm, 0));
	CFont fnt;
	fnt.CreateFontIndirect(&nm.lfMenuFont);

	CXTPFontDC fntDc(pDC, &fnt);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	CXTPClientRect rc(this);

	pDC->FillSolidRect(rc, GetColor(COLOR_MENU));

	pDC->SetBkMode(TRANSPARENT);

	HMENU hMenu = (HMENU)SendMessage(MN_GETHMENU);
	if (!hMenu)
		return;



	int nCount = GetMenuItemCount(hMenu);
	int y = 0;
	int x = 0;

	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO98 info = { sizeof(MENUITEMINFO98), MIIM_TYPE | MIIM_STATE | MIIM_BITMAP};
		::GetMenuItemInfo(hMenu, i, TRUE, (MENUITEMINFO*)&info);

		if ((info.fType & MFT_SEPARATOR) == MFT_SEPARATOR)
		{
			int nHeight = GetSystemMetrics(SM_CYMENUSIZE) / 2;

			int yT = y + (nHeight / 2) - 1;
			RECT rcItem;

			rcItem.left     = x + 1;
			rcItem.top      = yT;
			rcItem.right    = rc.right - 1;
			rcItem.bottom   = yT + GetSystemMetrics(SM_CYEDGE);

			DrawEdge(pDC->GetSafeHdc(), &rcItem, BDR_SUNKENOUTER, BF_TOP | BF_BOTTOM);

			y += nHeight;
		}
		else
		{
			int nHeight = 0;
			nHeight = max(nHeight, tm.tmHeight + tm.tmExternalLeading + GetSystemMetrics(SM_CYEDGE));

			nHeight += 2;


			CRect rcItem(0, y, rc.right, y + nHeight);

			if (info.fState & MFS_HILITE)
			{
				pDC->FillSolidRect(rcItem, GetColor(COLOR_HIGHLIGHT));
			}

			y += nHeight;
		}
	}*/


	CXTPClientRect rc(this);
	CXTPBufferDC dc(pDC->GetSafeHdc(), rc);

	dc.FillSolidRect(rc, GetColor(COLOR_MENU));

	dc.SetBkMode(TRANSPARENT);
	DefWindowProc(WM_PRINTCLIENT, (WPARAM)dc.GetSafeHdc(), 0);

	//GetMenuBarInfo();
}

void CXTPSkinObjectMenu::OnDrawNonCLient(CDC* pDC)
{
	CRect rcBorders = m_rcBorders;
	CXTPWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());

	XTPSkinFrameworkDrawFrame(pDC->GetSafeHdc(), rc, 1, GetSysColor(COLOR_3DSHADOW));
	rc.DeflateRect(1, 1);
	XTPSkinFrameworkDrawFrame(pDC->GetSafeHdc(), rc, rcBorders.left - 1, GetSysColor(COLOR_MENU));
}

BOOL CXTPSkinObjectMenu::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CXTPSkinObjectMenu::OnNcPaint()
{
	CWindowDC dc(this);
	OnDrawNonCLient(&dc);
}

LRESULT CXTPSkinObjectMenu::OnPrint(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (lParam & PRF_NONCLIENT)
	{
		OnDrawNonCLient(pDC);
	}
	if (lParam & PRF_CLIENT)
	{
		CPoint pt = pDC->SetWindowOrg(-m_rcBorders.left, -m_rcBorders.top);
		OnDraw(pDC);
		pDC->SetWindowOrg(pt);
	}
	return 1;
}

LRESULT CXTPSkinObjectMenu::OnPrintClient(WPARAM wParam, LPARAM /*lParam*/)
{
	CDC* pDC = CDC::FromHandle((HDC)wParam);
	OnDraw(pDC);
	return 1;
}

LRESULT CXTPSkinObjectMenu::OnSelectItem(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	SetRedraw(FALSE);
	LRESULT lResult = Default();
	SetRedraw(TRUE);

	Invalidate(FALSE);

	return lResult;
}

LRESULT CXTPSkinObjectMenu::OnKeyDown(WPARAM wParam, LPARAM /*lParam*/)
{
	if (wParam == VK_UP || wParam == VK_DOWN)
	{
		SetRedraw(FALSE);
		LRESULT lResult = Default();

		if (IsWindow(m_hWnd))
		{
			SetRedraw(TRUE);
			Invalidate(FALSE);
		}

		return lResult;
	}
	else
	{
		return Default();
	}
}

LRESULT CXTPSkinObjectMenu::OnChar(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	SetRedraw(FALSE);
	LRESULT lResult = Default();

	if (IsWindow(m_hWnd))
	{
		SetRedraw(TRUE);
		Invalidate(FALSE);
	}

	return lResult;
}
