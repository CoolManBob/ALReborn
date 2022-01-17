/////////////////////////////////////////////////////////////////////////////
// DropListBox.cpp : implementation file
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
#include "DropListBox.h"
#include "AdvComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDropListBox

CDropListBox::CDropListBox( CWnd* pComboParent, CDropScrollBar* pScroll )
:
	m_pComboParent( pComboParent ),
	m_pScroll( pScroll )
{
	m_pListFont = new CFont;
	LOGFONT logFont;
	memset( &logFont, 0, sizeof(LOGFONT) );
	_tcscpy( logFont.lfFaceName, _T("MS Sans Serif") );
	logFont.lfHeight = 20;
	m_pListFont->CreateFontIndirect(&logFont);

	m_nLastTopIdx = 0;

	m_dwACBStyle = 0;

	m_bSelectDisabled = FALSE;
}

CDropListBox::~CDropListBox()
{
	delete m_pListFont;
}

BEGIN_MESSAGE_MAP(CDropListBox, CListBox)
	//{{AFX_MSG_MAP(CDropListBox)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_VRC_SETCAPTURE, OnSetCapture )
	ON_MESSAGE( WM_VRC_RELEASECAPTURE, OnReleaseCapture )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDropListBox message handlers

int CDropListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Because this window doesn't have an owner, there will appear 
	// a 'blank' button on the taskbar. The following are to hide 
	// that 'blank' button on the taskbar
	ShowWindow( SW_HIDE );
	ModifyStyleEx( 0, WS_EX_TOOLWINDOW );// |WS_VSCROLL );//| WS_EX_NOACTIVATE ); // WS_EX_CONTROLPARENT
	ShowWindow( SW_SHOW );
	SetWindowPos( &wndTopMost, lpCreateStruct->x, lpCreateStruct->y, 
		lpCreateStruct->cx, lpCreateStruct->cy, SWP_SHOWWINDOW );

	SetFont( static_cast<CAdvComboBox*>(m_pComboParent)->GetFont() ); 
	return 0;
}

LONG CDropListBox::OnSetCapture( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	SetCapture();
	return FALSE;
}

LONG CDropListBox::OnReleaseCapture( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	ReleaseCapture();
	return FALSE;
}

void CDropListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	//
	// Is mouse within listbox
	CRect rcClient;
	GetClientRect( rcClient );
	if( !rcClient.PtInRect( point ) )
	{
		ReleaseCapture();
		GetParent()->SendMessage( WM_VRC_SETCAPTURE );
	}

	//
	// Set selection item under mouse
	int nPos = point.y / GetItemHeight(0) + GetTopIndex();
	PLIST_ITEM pItem = (PLIST_ITEM)GetItemDataPtr(nPos);
	if( (DWORD)pItem != -1 )
	{
		if( GetCurSel() != nPos && !pItem->bDisabled )
		{
			SetCurSel( nPos );
		}
	}

	//
	// Check if we have autoscrolled
	if( m_nLastTopIdx != GetTopIndex() )
	{
		//int nDiff = m_nLastTopIdx - GetTopIndex();
		m_nLastTopIdx = GetTopIndex();

		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		if( m_pScroll->GetScrollInfo( &info, SIF_ALL|SIF_DISABLENOSCROLL ) )
		{
			info.nPos = m_nLastTopIdx;
			m_pScroll->SetScrollInfo( &info );
		}
	}

	CListBox::OnMouseMove(nFlags, point);
}

void CDropListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
//	CListBox::OnLButtonUp(nFlags, point);
}

void CDropListBox::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	//
	// Is mouse within listbox
	CRect rcClient;
	GetClientRect( rcClient );
	if( !rcClient.PtInRect( point ) )
	{
		ReleaseCapture();
		GetParent()->SendMessage( WM_VRC_SETCAPTURE );
	}

	//
	// Set selection item under mouse
	int nPos = point.y / GetItemHeight(0) + GetTopIndex();
	PLIST_ITEM pItem = (PLIST_ITEM)GetItemDataPtr(nPos);
	if( (DWORD)pItem != -1 )
	{
		if( pItem->bDisabled )
		{
			return;
		}
	}

	nPos = GetCurSel();

	//
	// Is selected item disabled
	if( nPos != LB_ERR )
	{
		PLIST_ITEM pItem;
		pItem = (PLIST_ITEM)GetItemDataPtr( nPos );
		if( pItem->bDisabled )
		{
			return;
		}
	}

	//
	// Send current selection to comboedit
	if( nPos != -1 )
		m_pComboParent->PostMessage( WM_SELECTED_ITEM, (WPARAM)nPos, 0 );

	//
	// Destroy dropdown
	ReleaseCapture();
	m_pComboParent->PostMessage( WM_DESTROY_DROPLIST );

//	CListBox::OnLButtonDown(nFlags, point);
}

int CDropListBox::GetBottomIndex()
{
	int nTop = GetTopIndex();
	CRect rc;
	GetClientRect( &rc );
	int nVisCount = rc.Height() / GetItemHeight(0);
	return nTop + nVisCount;
}

void CDropListBox::SetTopIdx(int nPos, BOOL bUpdateScrollbar)
{
	m_nLastTopIdx = nPos;
	SetTopIndex( nPos );
	if( bUpdateScrollbar )
	{
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		if( m_pScroll->GetScrollInfo( &info, SIF_ALL|SIF_DISABLENOSCROLL ) )
		{
			info.nPos = m_nLastTopIdx;
			m_pScroll->SetScrollInfo( &info );
		}
	}
}

void CDropListBox::GetTextSize(LPCTSTR lpszText, int nCount, CSize &size)
{
	CClientDC dc(this);
	int nSave = dc.SaveDC();
	dc.SelectObject( static_cast<CAdvComboBox*>(m_pComboParent)->GetFont() );
	size = dc.GetTextExtent( lpszText, nCount );
	if( m_dwACBStyle & ACBS_CHECKED )
	{
		size.cx += 14;
	}
	dc.RestoreDC(nSave);
}

int CDropListBox::AddListItem( LIST_ITEM& item )
{
	PLIST_ITEM pItem = new LIST_ITEM;
	*pItem = item;
	int nPos = AddString( pItem->strText.c_str() );
	SetItemDataPtr( nPos, (void*)pItem );
	return nPos;
}

BOOL CDropListBox::DestroyWindow() 
{
	PLIST_ITEM pItem;
	for( int i = 0; i < GetCount(); i++ )
	{
		pItem = (PLIST_ITEM)GetItemDataPtr( i );
		delete pItem;
	}
	
	return CListBox::DestroyWindow();
}

void CDropListBox::SetDLBStyle(DWORD dwStyle)
{
	m_dwACBStyle = dwStyle;
}

int CDropListBox::SetCurSel(int nSelect)
{
	PLIST_ITEM pItem = NULL;
	int nCur = GetCurSel();
	int nWay = nSelect - nCur;
	int nTmp = nSelect;
	if( !m_bSelectDisabled )
	{
		// Select the next in list the is NOT disabled
		if( nWay < 0 )
		{
			// Select previous in list
			pItem = (PLIST_ITEM)GetItemDataPtr( nTmp );
			while( (DWORD)pItem != -1 )
			{
				if( !pItem->bDisabled )
				{
					nSelect = nTmp;
					break;
				}
				nTmp--;
				pItem = (PLIST_ITEM)GetItemDataPtr( nTmp );
			}
		}
		else
		if( nWay > 0 )
		{
			// Select next in list
			pItem = (PLIST_ITEM)GetItemDataPtr( nTmp );
			while( (DWORD)pItem != -1 )
			{
				if( !pItem->bDisabled )
				{
					nSelect = nTmp;
					break;
				}
				nTmp++;
				pItem = (PLIST_ITEM)GetItemDataPtr( nTmp );
			}
		}
	}

	pItem = (PLIST_ITEM)GetItemDataPtr( nSelect);
	if( (DWORD)pItem != -1 )
	{
		if( pItem->bDisabled )
		{
			return nSelect;
		}
	}
	int nr = CListBox::SetCurSel( nSelect );

	if( nr != -1 )
	{
		//
		// Set scrollbar
		int nTopIdx = GetTopIndex();

		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		if( m_pScroll->GetScrollInfo( &info, SIF_ALL|SIF_DISABLENOSCROLL ) )
		{
			info.nPos = nTopIdx;
			m_pScroll->SetScrollInfo( &info );
		}
	}
	return nr;
}

int CDropListBox::GetMaxVisibleItems()
{
	CRect rectClient;
	GetClientRect(&rectClient);
	int n = rectClient.Height() / GetItemHeight(0);
	return n;
}
