// XTMenuListBox.cpp : implementation of the CXTMenuListBox class.
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
#include "Common/XTPColorManager.h"
#include "Common/XTPImageManager.h"


#include "XTDefines.h"
#include "XTUtil.h"
#include "XTGlobal.h"
#include "XTVC50Helpers.h"
#include "XTMenuListBox.h"
#include "XTMemDC.h"
#include "XTMenuListBoxTheme.h"
#include "XTThemeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTMenuListBox::CONTENT_ITEM

CXTMenuListBox::CONTENT_ITEM::CONTENT_ITEM()
{
	m_pIcon = NULL;
}

CXTMenuListBox::CONTENT_ITEM::~CONTENT_ITEM()
{
	if (m_pIcon)
		delete m_pIcon;
}

/////////////////////////////////////////////////////////////////////////////
// CXTMenuListBox
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_THEME_HOST(CXTMenuListBox)
IMPLEMENT_THEME_REFRESH(CXTMenuListBox, CListBox)

CXTMenuListBox::CXTMenuListBox()
: CXTThemeManagerStyleHost(GetThemeFactoryClass())
, m_bNoBorder(false)
, m_bHilight(true)
, m_bNoUnderline(true)
, m_hCursor(NULL)
, m_bHilightItemID(-1)
, m_bTimerActive(false)
, m_nPrevIndex(-1)
{
}

CXTMenuListBox::~CXTMenuListBox()
{
	while (!m_arContentItems.IsEmpty())
	{
		CONTENT_ITEM* pCI = m_arContentItems.RemoveHead();
		delete pCI;
	}
}


IMPLEMENT_DYNAMIC(CXTMenuListBox, CListBox)

BEGIN_MESSAGE_MAP(CXTMenuListBox, CListBox)
	//{{AFX_MSG_MAP(CXTMenuListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Checks mouse movement, sets point member, sets timer.

BOOL CXTMenuListBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCursor != NULL)
	{
		CONTENT_ITEM* pCI = GetMenuItem(m_nPrevIndex);
		if (pCI && pCI->m_bEnabled)
		{
			SetCursor(m_hCursor);
			return TRUE;
		}
	}

	return CListBox::OnSetCursor(pWnd, nHitTest, message);
}

void CXTMenuListBox::RedrawItem(int iIndex)
{
	CRect rcItem;
	GetItemRect(iIndex, &rcItem);
	InvalidateRect(&rcItem);
}

void CXTMenuListBox::RedrawItem(CPoint point)
{
	BOOL bOutSide;
	int nIndex = ItemFromPoint(point, bOutSide);

	if (!bOutSide)
	{
		RedrawItem(nIndex);
	}
}

void CXTMenuListBox::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	BOOL bOutSide;
	int nIndex = ItemFromPoint (point, bOutSide);

	CRect rcItem;
	GetItemRect(nIndex, &rcItem);

	if ((m_nPrevIndex != -1) && (m_nPrevIndex != nIndex))
	{
		RedrawItem(m_nPrevIndex);
	}

	if (bOutSide || !rcItem.PtInRect(point))
	{
		RedrawItem(nIndex);
		nIndex = -1;
	}

	if (nIndex != GetCurSel())
	{
		SetCurSel(nIndex);

		if (nIndex != -1)
		{
			RedrawItem(nIndex);

			TRACKMOUSEEVENT tme =
			{
				sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0
			};
			_TrackMouseEvent(&tme);
		}
	}

	m_nPrevIndex = nIndex;
}

void CXTMenuListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);
	RedrawItem(point);
}

void CXTMenuListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonUp(nFlags, point);
	RedrawItem(point);

	CONTENT_ITEM* pCI = GetMenuItem(GetCurSel());
	if (pCI && !pCI->m_bEnabled)
		return;

	CWnd* pWndOwner = GetOwner();
	ASSERT(pWndOwner);

	pWndOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
		(LPARAM)GetCurSel(), (WPARAM)GetDlgCtrlID());
}

// Sets the highlight flag based on cursor position.

void CXTMenuListBox::OnMouseLeave()
{
	//reset mouse vars
	OnMouseMove(0, CPoint(-1, -1));
}

// Initial draw routine, draws button on mouse over,
// on mouse press, and on mouse out.

void CXTMenuListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	GetTheme()->DrawItem(this, lpDIS);
}

int CXTMenuListBox::GetTextHeight(LPCTSTR lspzItem) const
{
	CWindowDC dc(NULL);

	CXTStringHelper strItem(lspzItem);

	// get the height for a single line text item.
	CFont* pOldFont = dc.SelectObject(GetFont());
	CSize size = dc.GetTextExtent(strItem);
	dc.SelectObject(pOldFont);

	// set the height of the item with single line text.
	int iTextHeight = size.cy;

	// for each newline character we need to adjust the height.
	for (int i = 0; i < strItem.GetLength(); ++i)
	{
		if (strItem[i] == _T('\n'))
		{
			iTextHeight += size.cy;
		}
	}

	return iTextHeight;
}

// Sets item height

void CXTMenuListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// get the text height.
	int iTextHeight = GetTextHeight((LPCTSTR)lpMIS->itemData);

	// calculate the total height of the item.
	lpMIS->itemHeight = GetTheme()->m_cyEdge + GetTheme()->m_cyIcon + GetTheme()->m_cyEdge +
		iTextHeight + GetTheme()->m_cyEdge;
}

void CXTMenuListBox::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CListBox::OnWindowPosChanged(lpwndpos);
	InvalidateRect(NULL);
}

BOOL CXTMenuListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// Make sure that the control is owner drawn.
	dwStyle |= LBS_OWNERDRAWVARIABLE | LBS_NOINTEGRALHEIGHT;

	if (!CListBox::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	// Set the font used by the Outlook bar.
	SetFont(CFont::FromHandle((HFONT)
		::GetStockObject(DEFAULT_GUI_FONT)));

	return TRUE;
}

BOOL CXTMenuListBox::EnableMenuItem(int iItem, BOOL bEnabled)
{
	CONTENT_ITEM* pCI = GetMenuItem(iItem);
	if (pCI != NULL)
	{
		pCI->m_bEnabled = bEnabled;
		return TRUE;
	}
	return FALSE;
}

int CXTMenuListBox::AddMenuItem(UINT nIconID, LPCTSTR lpszText, BOOL bEnabled/*= TRUE*/)
{
	CONTENT_ITEM* pCI = new CONTENT_ITEM;
	ASSERT(pCI != NULL);

	pCI->m_nIndex = AddString(lpszText);
	pCI->m_strText = lpszText;
	pCI->m_bEnabled = bEnabled;

	/*// Load the icon for the menu item.
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nIconID), RT_GROUP_ICON);
	pCI->m_hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(nIconID),
		IMAGE_ICON, GetTheme()->m_cxIcon, GetTheme()->m_cyIcon, LR_DEFAULTCOLOR);*/

	pCI->m_pIcon = new CXTPImageManagerIcon(nIconID, 0, 0);
	pCI->m_pIcon->SetIcon(nIconID, GetTheme()->m_cxIcon, GetTheme()->m_cyIcon);

	m_arContentItems.AddTail(pCI);

	return pCI->m_nIndex;
}

int CXTMenuListBox::InsertMenuItem(int iIndex, UINT nIconID, LPCTSTR lpszText, BOOL bEnabled/*= TRUE*/)
{
	CONTENT_ITEM* pCI = new CONTENT_ITEM;
	ASSERT(pCI != NULL);

	pCI->m_nIndex = InsertString(iIndex, lpszText);
	pCI->m_strText = lpszText;
	pCI->m_bEnabled = bEnabled;

	// Load the icon for the menu item.
//  HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nIconID), RT_GROUP_ICON);
//  pCI->m_hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(nIconID),
//      IMAGE_ICON, GetTheme()->m_cxIcon, GetTheme()->m_cyIcon, LR_DEFAULTCOLOR);
	pCI->m_pIcon = new CXTPImageManagerIcon(nIconID, 0, 0);
	pCI->m_pIcon->SetIcon(nIconID, GetTheme()->m_cxIcon, GetTheme()->m_cyIcon);

	POSITION pos = m_arContentItems.FindIndex(pCI->m_nIndex);
	if (pos != NULL)
	{
		m_arContentItems.InsertBefore(pos, pCI);
	}
	else
	{
		m_arContentItems.AddTail(pCI);
	}

	return pCI->m_nIndex;
}

BOOL CXTMenuListBox::OnEraseBkgnd(CDC* /*pDC*/)
{
	return FALSE;
}

void CXTMenuListBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get the client rect.
	CRect rcClient;
	GetClientRect(&rcClient);

	// Paint to a memory device context to help
	// eliminate screen flicker.
	CXTMemDC memDC(&dc, rcClient, GetTheme()->m_crgBack);

	// Now let the window do its default painting...
	CWnd::DefWindowProc (WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

CXTMenuListBox::CONTENT_ITEM* CXTMenuListBox::GetMenuItem(int iItem)
{
	CONTENT_ITEM* pCI = NULL;

	// Get the menu item.
	POSITION pos = m_arContentItems.FindIndex(iItem);
	if (pos != NULL)
	{
		pCI = m_arContentItems.GetAt(pos);
	}

	return pCI;
}

void CXTMenuListBox::SetHilightCursor(UINT nIDCursor, bool bNoBorder/*= false*/, bool bNoUnderline/*= false*/)
{
	// load the new cursor.
	m_hCursor = AfxGetApp()->LoadCursor(nIDCursor);

	// display a border or text underline when highlighted ?
	m_bNoBorder = bNoBorder;
	m_bNoUnderline = bNoUnderline;
}

void CXTMenuListBox::SetColors(COLORREF clrText, COLORREF clrBack, COLORREF clrBackDark/*= COLORREF_NULL*/)
{
	GetTheme()->m_crItemText.SetCustomValue(clrText);  // Must be custom value used to prevent reset color in RefreshMetrics.
	GetTheme()->m_crgBack.SetCustomValue(clrBack, clrBackDark);
}
