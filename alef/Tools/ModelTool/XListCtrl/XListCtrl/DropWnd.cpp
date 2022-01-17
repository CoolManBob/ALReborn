/////////////////////////////////////////////////////////////////////////////
// DropWnd.cpp : implementation file
// 
// CAdvComboBox Control
// Version: 2.1
// Date: September 2002
// Author: Mathias Tunared
// Email: Mathias@inorbit.com
// Copyright (c) 2002. All Rights Reserved.
//
// This code, in compiled form or as source code, may be redistributed 
// unmodified PROVIDING it is not sold for profit without the authors 
// written consent, and providing that this notice and the authors name 
// and all copyright notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DropWnd.h"
#include "AdvComboBox.h"
#include "DropListBox.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDropWnd

CDropWnd::CDropWnd( CWnd* pComboParent, list<LIST_ITEM> &itemlist, DWORD dwACBStyle )
:
	m_pComboParent( pComboParent ),
	m_list( itemlist ),
	m_dwACBStyle( dwACBStyle ),
	m_nMaxX(1000),	//+++
	m_nMaxY(1000)	//+++
{
	m_bResizing = false;

	m_pListFont = new CFont;
	LOGFONT logFont;
	memset( &logFont, 0, sizeof(LOGFONT) );
	_tcscpy( logFont.lfFaceName, _T("MS Sans Serif") );
	logFont.lfHeight = 12;
	m_pListFont->CreateFontIndirect(&logFont);
}

CDropWnd::~CDropWnd()
{
	delete m_pListFont;
	delete m_scrollbar;
	delete m_listbox;
}

BEGIN_MESSAGE_MAP(CDropWnd, CWnd)
	//{{AFX_MSG_MAP(CDropWnd)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATEAPP()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_VRC_SETCAPTURE, OnSetCapture )
	ON_MESSAGE( WM_VRC_RELEASECAPTURE, OnReleaseCapture )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDropWnd message handlers

BOOL CDropWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	//
	// Calc the size of the wnd
	
	return CWnd::PreCreateWindow(cs);
}

#if 0  // -----------------------------------------------------------
BOOL CDropWnd::Create(DWORD dwStyle, 
					  const RECT& rect, 
					  CWnd* pParentWnd, 
					  UINT nID, 
					  CCreateContext* pContext) 
{
	return CWnd::Create(0, 0, dwStyle, rect, pParentWnd, nID, pContext);
}
#endif // -----------------------------------------------------------

int CDropWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rcWnd;
	GetClientRect( &rcWnd );

	//
	// Because this window doesn't have an owner, there will appear 
	// a 'blank' button on the taskbar. The following are to hide 
	// that 'blank' button on the taskbar
	ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
	SetWindowPos( &wndTopMost, lpCreateStruct->x, lpCreateStruct->y, 
		lpCreateStruct->cx, lpCreateStruct->cy, SWP_SHOWWINDOW );

	//
	// Create scrollbar
	m_rcScroll = rcWnd;
	m_rcScroll.left = m_rcScroll.right - ::GetSystemMetrics(SM_CXVSCROLL);
	m_rcScroll.bottom -= ::GetSystemMetrics(SM_CYVSCROLL);
	m_scrollbar = new CDropScrollBar;
	VERIFY(m_scrollbar->Create( SBS_VERT | SBS_RIGHTALIGN | WS_CHILD, m_rcScroll, this, 100));

	//
	// Create listbox
	m_rcList.SetRect( lpCreateStruct->x, lpCreateStruct->y, 
		lpCreateStruct->x+lpCreateStruct->cx-::GetSystemMetrics(SM_CXVSCROLL), 
		lpCreateStruct->y+lpCreateStruct->cy );
	ScreenToClient( m_rcList );
	m_listbox = new CDropListBox(m_pComboParent, m_scrollbar);
	VERIFY(m_listbox->Create( WS_CHILD | LBS_NOINTEGRALHEIGHT 
		/*| WS_VISIBLE | */ /*| LBS_OWNERDRAWFIXED*/, 
		m_rcList, this, 101 ) );
	m_listbox->SetDLBStyle( m_dwACBStyle );
	m_listbox->ShowWindow( SW_SHOW );

	//
	// Resize this wnd so INTEGRAL_HEIGHT applies!?
	CRect rc;
	int nH = m_listbox->GetItemHeight(0);
	nH = nH*m_list.size()+2;
	// Get screen size
	int nScrX = GetSystemMetrics( SM_CXSCREEN );
	int nScrY = GetSystemMetrics( SM_CYSCREEN );

	int nDefaultItems = static_cast<CAdvComboBox*>(m_pComboParent)->GetDefaultVisibleItems();

	//
	// Check to see if the window should be placed over the combobox
	int nY = lpCreateStruct->y;
	int nItems = m_list.size();
	int nItemHeight = m_listbox->GetItemHeight(0);
	int nVisHeight = nScrY - lpCreateStruct->y;
	CRect rcCombo;
	m_pComboParent->GetWindowRect( &rcCombo );
	if( (nVisHeight / nItemHeight) < static_cast<CAdvComboBox*>(m_pComboParent)->GetMinVisibleItems() )
	{
		int nComboTopY = lpCreateStruct->y - rcCombo.Height();
		if( nDefaultItems == -1 || nDefaultItems > nItems )
		{
			nY = (nComboTopY - nH) < 0 ? 0 : (nComboTopY - nH);
			nH = (nY + nH) > nComboTopY ? nComboTopY - nY : nH;
		}
		else
		{
			nY = nComboTopY - nItemHeight*nDefaultItems;
			nY -= 2;
			nY = nY < 0 ? 0 : nY;
			nH = nComboTopY - nY;
		}
	}
	else
	{
		//
		// Place the window below the combo
		// Check height
		if( nDefaultItems == -1 || nDefaultItems > nItems )
		{
			if( lpCreateStruct->y + nH > nScrY )
			{
				nH = nScrY - lpCreateStruct->y;
			}
			else
			if( nH < ::GetSystemMetrics(SM_CYVSCROLL) )
			{
				nH = ::GetSystemMetrics(SM_CYVSCROLL);
			}
		}
		else
		{
			nH = nDefaultItems * nItemHeight;
			nH = (nY+nH) > nScrY ? nScrY-nY : nH;
			nH += 2;
		}
	}

	// Calc width
	int nW = 0;
	CSize size(0,0);
	for( m_iter = m_list.begin(); m_iter != m_list.end(); ++m_iter )
	{
		m_listbox->GetTextSize( m_iter->strText.c_str(), m_iter->strText.length(), size );
		nW = (size.cx > nW) ? size.cx : nW;
	}

	nW += m_rcScroll.Width() + 8;

	// Check min width
	if( nW < m_rcList.Width() )
	{
		nW = lpCreateStruct->cx;
	}

	//+++
	if (nW < rcCombo.Width())
		nW = rcCombo.Width();

	// Check max width
	int nX = lpCreateStruct->x;
	if( nW > nScrX - lpCreateStruct->x )
	{
		nX = nScrX - nW;
		if( nX < 0 )
			nX = 0;
	}
	if( nX == 0 && nW > nScrX )
		nW = nScrX;

	//nW += 4;			//+++
	m_nMaxX = nW + 100;	//+++
	m_nMaxY = nH + 100;	//+++

	SetWindowPos( &wndTopMost, nX, nY, nW, nH, SWP_SHOWWINDOW|SWP_NOZORDER );

	GetClientRect( &rcWnd );

	//
	// Create sizehandle
	m_rcSizeHandle = rcWnd;
	GetClientRect( &m_rcSizeHandle );
	m_rcSizeHandle.left = m_rcSizeHandle.right - ::GetSystemMetrics(SM_CXVSCROLL);
	m_rcSizeHandle.top = m_rcSizeHandle.bottom - ::GetSystemMetrics(SM_CYVSCROLL);

	//
	// Add items to listbox
	LIST_ITEM item;
	for( m_iter = m_list.begin(); m_iter != m_list.end(); ++m_iter )
	{
		item = *m_iter;
		m_listbox->AddListItem( item );
	}

	//
	// Set values in scrollbar
	m_scrollbar->SetListBox( m_listbox );
	m_scrollbar->ShowScrollBar();

	//
	//
	SetCapture();
	return 0;
}

LONG CDropWnd::OnSetCapture( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	SetCapture();
	return FALSE;
}

LONG CDropWnd::OnReleaseCapture( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	ReleaseCapture();
	return FALSE;
}

void CDropWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	//
	//
	if( m_bResizing )
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
#if 0  // -----------------------------------------------------------
		if( point.x + m_nMouseDiffX >= ::GetSystemMetrics(SM_CXVSCROLL) )
		{
			rcWnd.right = rcWnd.left + point.x + m_nMouseDiffX +2;
		}
		else
		{
			rcWnd.right = rcWnd.left + ::GetSystemMetrics(SM_CXVSCROLL) +1;
		}

		if( point.y + m_nMouseDiffY >= ::GetSystemMetrics(SM_CYVSCROLL) )
		{
			rcWnd.bottom = rcWnd.top + point.y + m_nMouseDiffY +2;
		}
		else
		{
			rcWnd.bottom = rcWnd.top + ::GetSystemMetrics(SM_CXVSCROLL) +1;
		}
#endif // -----------------------------------------------------------

		//+++
		CRect rcCombo;
		m_pComboParent->GetWindowRect( &rcCombo );

		if( point.x + m_nMouseDiffX < rcCombo.Width())
		{
			rcWnd.right = rcWnd.left + rcCombo.Width();
		}
		else if (point.x + m_nMouseDiffX > m_nMaxX)
		{
			rcWnd.right = rcWnd.left + m_nMaxX;
		}
		else
		{
			rcWnd.right = rcWnd.left + point.x + m_nMouseDiffX;
		}

		if( point.y + m_nMouseDiffY < :: GetSystemMetrics(SM_CYVSCROLL) )
		{
			rcWnd.bottom = rcWnd.top + ::GetSystemMetrics(SM_CXVSCROLL) +1;
		}
		else if ( point.y + m_nMouseDiffY > m_nMaxY )
		{
			rcWnd.bottom = rcWnd.top + m_nMaxY;
		}
		else
		{
			rcWnd.bottom = rcWnd.top + point.y + m_nMouseDiffY +2;
		}


		MoveWindow( &rcWnd );
		return;
	}

	//
	// Check point
	if( m_rcList.PtInRect( point ) )
	{
		HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW );
		SetCursor( hCursor );
		ReleaseCapture();
		m_scrollbar->SendMessage( WM_VRC_RELEASECAPTURE );
		m_listbox->SetFocus();
		m_listbox->SendMessage( WM_VRC_SETCAPTURE );
	}
	else
	if( m_rcScroll.PtInRect( point ) )
	{
		HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW );
		SetCursor( hCursor );
		m_scrollbar->SetFocus();
		ReleaseCapture();
		m_listbox->SendMessage( WM_VRC_RELEASECAPTURE );
		m_scrollbar->SendMessage( WM_VRC_SETCAPTURE );
	}
	else
	{
		if( m_rcSizeHandle.PtInRect( point ) )
		{
			HCURSOR hCursor = LoadCursor( NULL, IDC_SIZENWSE );
			SetCursor( hCursor );
		}
		else
		{
			HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW );
			SetCursor( hCursor );
		}
		SetCapture();
		CWnd::OnMouseMove(nFlags, point);
	}
}

void CDropWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	XLISTCTRL_TRACE(_T("in CDropWnd::OnLButtonDown\n"));
	if( m_rcSizeHandle.PtInRect( point ) )
	{
		m_bResizing = true;

		m_ptLastResize = point;

		CRect rcClient;
		GetClientRect( &rcClient );
		m_nMouseDiffX = rcClient.Width() - point.x;
		m_nMouseDiffY = rcClient.Height() - point.y;
		return;
	}

//
// Clean up the code below...
//

	CRect rc;
	CRect rcVScroll(0,0,0,0);
	GetClientRect( &rc );
	
	// Take away vertical scroll

	if( rc.PtInRect( point ) )
	{
	}
	else
	{
		//
		// Calc the point in the parent(PropertyListBox)
		CWnd* pParent = m_pComboParent->GetParent();
		CRect rcParentClient;
		CRect rcParentWnd;
		pParent->GetClientRect( &rcParentClient );
		pParent->GetWindowRect( &rcParentWnd );

		CPoint pt = point;
		ClientToScreen( &pt );
			pt.x -= rcParentWnd.left;
			pt.y -= rcParentWnd.top;

		CAdvComboBox* pList = static_cast<CAdvComboBox*>(m_pComboParent);
		if( !pList->PointInWindow( pt ) )
		{
			
			ReleaseCapture();

			m_pComboParent->PostMessage( WM_DESTROY_DROPLIST );
		}
		else
		{
			ReleaseCapture();
			m_pComboParent->PostMessage( WM_DESTROY_DROPLIST );
		}
	}

	LPARAM l = MAKELPARAM(point.x, point.y);
	m_pComboParent->SendMessage(WM_LBUTTONDOWN, nFlags, l);

	CWnd::OnLButtonDown(nFlags, point);
}

void CDropWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bResizing )
	{
		m_bResizing = false;
		m_listbox->UnlockWindowUpdate();

		m_listbox->RedrawWindow();
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CDropWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	m_rcList.SetRect( 0, 0, cx-::GetSystemMetrics(SM_CXVSCROLL), cy );
	CRect rectBorder = m_rcList;
	m_rcList.DeflateRect(1, 1);
	m_listbox->MoveWindow( &m_rcList );

	m_rcScroll.SetRect( cx-::GetSystemMetrics(SM_CXVSCROLL), 0, cx, cy-::GetSystemMetrics(SM_CYVSCROLL) );
	m_rcScroll.DeflateRect(1, 1);
	m_scrollbar->MoveWindow( &m_rcScroll );

	//?????  +++
	rectBorder.right = m_rcScroll.right + 1;
	//CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));//COLOR_INACTIVECAPTION));	// same as combobox
	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));//COLOR_INACTIVECAPTION));	// same as combobox
	CDC * pDC = GetDC();
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->Rectangle(&rectBorder);
	pDC->SelectObject(pOldPen);
	ReleaseDC(pDC);

	m_rcSizeHandle.SetRect( cx-::GetSystemMetrics(SM_CXVSCROLL), cy-::GetSystemMetrics(SM_CYVSCROLL), cx, cy );
	m_rcSizeHandle.DeflateRect(1, 1);
	InvalidateRect( &m_rcSizeHandle );

	//
	// Fix the scrollbar
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	if( m_scrollbar->GetScrollInfo( &info, SIF_ALL|SIF_DISABLENOSCROLL ) )
	{
		info.nPage = m_listbox->GetBottomIndex() - m_listbox->GetTopIndex();
		m_scrollbar->SetScrollInfo( &info );
	}
}

void CDropWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if( g_xpStyle.UseVisualStyles() )
	{
		HTHEME hTheme = NULL;
		hTheme = g_xpStyle.OpenThemeData( m_hWnd, L"SCROLLBAR" );
		HRESULT hr = g_xpStyle.DrawThemeBackground( hTheme, dc.m_hDC, SBP_LOWERTRACKVERT, SCRBS_NORMAL, &m_rcSizeHandle, NULL);
		hr = g_xpStyle.DrawThemeBackground( hTheme, dc.m_hDC, SBP_SIZEBOX, SZB_RIGHTALIGN, &m_rcSizeHandle, NULL);
		hr = g_xpStyle.CloseThemeData( hTheme );

	}
	else
	{
		dc.DrawFrameControl(m_rcSizeHandle, DFC_SCROLL, DFCS_SCROLLSIZEGRIP );
	}

	// Do not call CWnd::OnPaint() for painting messages
}


BOOL CDropWnd::DestroyWindow() 
{
	ReleaseCapture();
	m_listbox->DestroyWindow();
	
	return CWnd::DestroyWindow();
}

void CDropWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	if( bShow )
	{
//		AnimateWindow( GetSafeHwnd(), 80, AW_VER_POSITIVE | AW_ACTIVATE | AW_SLIDE);
	}
	else
	{
//		AnimateWindow( GetSafeHwnd(), 80, AW_VER_NEGATIVE | AW_HIDE | AW_SLIDE);
	}
	
}

//+++
#if (_MSC_VER > 1200)
void CDropWnd::OnActivateApp(BOOL bActive, DWORD dwThreadID) 
{
	CWnd::OnActivateApp(bActive, dwThreadID);
#else
 void CDropWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CWnd::OnActivateApp(bActive, hTask);
#endif

	if(!bActive) 
	{
		m_pComboParent->PostMessage(WM_DESTROY_DROPLIST);
	}
}

