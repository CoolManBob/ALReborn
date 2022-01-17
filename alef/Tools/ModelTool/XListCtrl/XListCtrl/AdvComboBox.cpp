/////////////////////////////////////////////////////////////////////////////
// AdvComboBox.cpp : implementation file
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
#include "AdvComboBox.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_COMBOEDIT 108


/////////////////////////////////////////////////////////////////////////////
// CAdvComboBox
IMPLEMENT_DYNAMIC(CAdvComboBox, CWnd) 

CAdvComboBox::CAdvComboBox( BOOL bInst, CWnd * pParent ) :
	m_pDropWnd(0),
	m_bDropListVisible(0),
	m_bInst( bInst ),
	m_pParent(pParent)
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::CAdvComboBox\n"));
	RegisterWindowClass();
	m_pEdit = NULL;
	m_zDelta = 0;
	m_nCurSel = -1;
	m_bDropRectStored= false;
	m_bHasFocus = false;
	m_bHasSentFocus = false;
	m_bSelItem = false;
	m_bFirstPaint = true;
	m_nMinVisItems = 5;
	m_bCodeCreate = false;
	m_bAutoAppend = TRUE;
	m_bDropButtonHot = false;
	m_bTrackMouseLeave = false;
	m_nDefaultDropItems = -1;

	m_dwACBStyle = 0;
	m_dwACBStyle |= ACBS_STANDARD;

	m_pFont = NULL;
}

CAdvComboBox::~CAdvComboBox()
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::~CAdvComboBox\n"));

	delete m_pFont;
	if( m_pDropWnd )
	{
		m_pDropWnd->ShowWindow( SW_HIDE );
		m_bDropListVisible = FALSE;
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;
	}
	if( m_pEdit )
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
	}
}

BEGIN_MESSAGE_MAP(CAdvComboBox, CWnd)
	//{{AFX_MSG_MAP(CAdvComboBox)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	ON_WM_ENABLE()
	ON_WM_CHILDACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SELECTED_ITEM, OnSelectedItem )
	ON_MESSAGE( WM_ON_DROPDOWN_BUTTON, OnDropdownButton )
	ON_MESSAGE( WM_DESTROY_DROPLIST, OnDestroyDropdownList )
	ON_EN_KILLFOCUS(IDC_COMBOEDIT, OnKillfocusEdit)
	ON_EN_SETFOCUS(IDC_COMBOEDIT, OnSetfocusEdit)
	ON_EN_CHANGE(IDC_COMBOEDIT, OnChangeEdit)
	ON_EN_UPDATE(IDC_COMBOEDIT, OnUpdateEdit)

	ON_MESSAGE( CB_ADDSTRING, OnAddString )
	ON_MESSAGE( CB_SETCURSEL, OnSetCurSel )
	ON_MESSAGE( CB_GETCURSEL, OnGetCurSel )
	ON_MESSAGE( CB_SELECTSTRING, OnSelectString )
	ON_MESSAGE( CB_GETCOUNT, OnGetCount )
	ON_MESSAGE( CB_RESETCONTENT, OnResetContent )
	ON_MESSAGE( CB_GETLBTEXT, OnGetLBText )
	ON_MESSAGE( CB_GETLBTEXTLEN, OnGetLBTextLen )
	ON_MESSAGE( CB_GETTOPINDEX, OnGetTopIndex )
	ON_MESSAGE( CB_SETTOPINDEX, OnSetTopIndex )

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAdvComboBox message handlers

LONG CAdvComboBox::OnAddString( WPARAM /*wParam*/, LPARAM lString )
{
	TCHAR* pStr = (TCHAR*)lString;
	return AddString( pStr );
}

LONG CAdvComboBox::OnSetCurSel( WPARAM wIndex, LPARAM /*lParam*/ )
{
	int nIndex = (int)wIndex;
	return SetCurSel( nIndex );
}

LONG CAdvComboBox::OnGetCurSel( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	return GetCurSel();
}

LONG CAdvComboBox::OnSelectString( WPARAM wItemStart, LPARAM lString )
{
	int nItem = (int)wItemStart;
	TCHAR* pStr = (TCHAR*)lString;
	int nIndex = FindStringExact( nItem, pStr );
	return SetCurSel( nIndex );
}

LONG CAdvComboBox::OnGetCount( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	return GetCount();
}

LONG CAdvComboBox::OnResetContent( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	m_list.clear();
	m_strEdit = _T("");
	m_nCurSel = -1;
	SetWindowText( _T("") );
	if( m_pEdit )
	{
		m_pEdit->SetWindowText( _T("") );
	}
	Invalidate();
	return CB_OKAY;
}

LONG CAdvComboBox::OnGetLBText( WPARAM wIndex, LPARAM lString )
{
	int nIndex = (int)wIndex;
	TCHAR* pOutStr = (TCHAR*)lString;
	return GetLBText( nIndex, pOutStr );
}


LONG CAdvComboBox::OnGetLBTextLen( WPARAM wIndex, LPARAM /*lParam*/ )
{
	int nIndex = (int)wIndex;
	return GetLBTextLen( nIndex );
}

LONG CAdvComboBox::OnGetTopIndex( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	return GetTopIndex();
}

LONG CAdvComboBox::OnSetTopIndex( WPARAM wIndex, LPARAM /*lParam*/ )
{
	return SetTopIndex(wIndex);
}

BOOL CAdvComboBox::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, ADVCOMBOBOXCTRL_CLASSNAME, &wndcls)) )
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_WINDOW);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = ADVCOMBOBOXCTRL_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CAdvComboBox::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::Create\n"));
	m_rcCombo = rect;
	m_bCodeCreate = true;

	m_dwACBStyle |= ACBS_STANDARD;
	m_dwStyle = dwStyle;

	LoadString( nID );
	
	BOOL bRet = CWnd::Create(NULL, _T(""), dwStyle, m_rcCombo, pParentWnd, nID );

	return bRet;
}

void CAdvComboBox::LoadString( UINT nStringID )
{
	UINT nIDS;
	if( nStringID )
		nIDS = nStringID;
	else
	{
		nIDS = GetWindowLong( GetSafeHwnd(), GWL_ID );
	}

	//
	// Load string from resources
	CString strItems;
	if( !strItems.LoadString( nIDS ) )
	{
		return;
	}

	//
	// Clear the list
	m_list.clear();

	//
	// Go through the string and look after '\n'.
	TCHAR seps[] = _T("\n");
	TCHAR *token;
	int nLen = strItems.GetLength();
	TCHAR* szTok = new TCHAR[nLen+5];
	memset( szTok, 0, (nLen+5)*sizeof(TCHAR) );
	_tcscpy( szTok, (LPCTSTR)strItems );

	token = _tcstok( szTok, seps );
	while( token != NULL )
	{
		AddString( token );
		token = _tcstok( NULL, seps );
	}

	//
	// Add item to list

	//+++
	delete [] szTok;
}

int CAdvComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::OnCreate\n"));

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if( !m_pFont )
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT) );
		CFont* pF = m_pParent->GetFont();
		ASSERT(pF);
		pF->GetLogFont( &logFont );
		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&logFont);
		SetFont( m_pFont );
	}

	return 0;
}

void CAdvComboBox::PreSubclassWindow() 
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::PreSubclassWindow\n"));

	if (m_pParent == NULL)
		m_pParent = GetParent();
	ASSERT(m_pParent);

	// TODO: Add your specialized code here and/or call the base class
	LoadString();

	if( !m_pFont )
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT) );
		CFont* pF = m_pParent->GetFont();
		ASSERT(pF);
		pF->GetLogFont( &logFont );
		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&logFont);
		SetFont( m_pFont );
	}

	if( !m_bCodeCreate )
	{
		//
		// Do we need to show an edit control. (CBS_DROPDOWN)
		if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
		{
			if( !m_pEdit )
			{
				CRect rect;
				GetClientRect(rect);
				rect.right = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
				if( m_dwACBStyle & ACBS_STANDARD )
				{
					rect.DeflateRect( 2, 2 );
					rect.left += 1;
					rect.top += 2;
					//rect.bottom += 2;
				}

				XLISTCTRL_TRACE(_T("creating edit\n"));
				m_pEdit = new CAdvComboEdit;	//+++
				ASSERT(m_pEdit);
				DWORD dwStyle = 0;
				dwStyle = WS_VISIBLE | WS_CHILD;
				if( GetStyle() & CBS_AUTOHSCROLL )
				{
					dwStyle |= ES_AUTOHSCROLL;
				}
				VERIFY(m_pEdit->Create( dwStyle, rect, this, IDC_COMBOEDIT ));
				m_pEdit->SetFont( m_pFont );
				m_pEdit->SetWindowText( m_strEdit.c_str() );
			}
		}
	}
	CWnd::PreSubclassWindow();
}

void CAdvComboBox::OnChildActivate() 
{
	CWnd::OnChildActivate();
	
	// TODO: Add your message handler code here
	if( !m_pFont )
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT) );
		CFont* pF = m_pParent->GetFont();
		pF->GetLogFont( &logFont );
		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&logFont);
		SetFont( m_pFont );
	}
	if( m_bCodeCreate )
	{
		//
		// Do we need to show an edit control. (CBS_DROPDOWN)
		if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
		{
			if( !m_pEdit )
			{
				CRect rect;
				GetClientRect(rect);
				rect.right = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
				if( m_dwACBStyle & ACBS_STANDARD )
				{
					rect.DeflateRect( 2, 2 );
					rect.left += 1;
					rect.top += 2;
					//rect.bottom += 2;
				}

				m_pEdit = new CAdvComboEdit;	//+++
				DWORD dwStyle = 0;
				dwStyle = WS_VISIBLE | WS_CHILD;
				if( GetStyle() & CBS_AUTOHSCROLL )
				{
					dwStyle |= ES_AUTOHSCROLL;
				}
				m_pEdit->Create( dwStyle, rect, this, IDC_COMBOEDIT );
				m_pEdit->SetFont( m_pFont );
				m_pEdit->SetWindowText( m_strEdit.c_str() );
			}
		}
	}
}

void CAdvComboBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if( m_nCurSel != -1 )
	{
		m_iter = m_list.begin();
		advance( m_iter, m_nCurSel );
		if( m_iter != m_list.end() )
		{
			m_strEdit = m_iter->strText;
		}
		if( m_bFirstPaint )
		{
			if( m_pEdit )
			{
				m_pEdit->SetWindowText( m_strEdit.c_str() );
				m_bFirstPaint = false;
				m_pEdit->EnableWindow( IsWindowEnabled() );
			}
		}
	}
	
	CRect rect;
	CRect rcText;

	GetClientRect(rect);
	rcText = rect;
	rect.left = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
	rcText.right = rect.left-1;

	m_rcDropButton = rect;
	GetClientRect(rect);

	BOOL bWndEnabled = IsWindowEnabled();

	COLORREF clrDisabledBkg = ::GetSysColor(COLOR_BTNFACE);
	COLORREF clrDisabledBorder = ::GetSysColor(COLOR_3DDKSHADOW);
	COLORREF clrDisabledText = ::GetSysColor(COLOR_GRAYTEXT);

	if( !bWndEnabled )
	{
		if( 1 ) // Draw disabled flat control with border? Change to '0'.
		{
			dc.FillSolidRect( rect, clrDisabledBkg );
		}
		else
		{
			CBrush brDisabled(clrDisabledBkg);
			CBrush* pOldBrush = dc.SelectObject(&brDisabled);
			CPen penDisabled( PS_SOLID, 0, clrDisabledBorder);
			CPen* pOldPen = dc.SelectObject(&penDisabled);
			dc.Rectangle(rect);
			dc.SelectObject(pOldBrush);
			dc.SelectObject(pOldPen);
		}
	}
	else
	{
		COLORREF clrEnabledBkg = ::GetSysColor(COLOR_WINDOW);
		dc.FillSolidRect( rect, clrEnabledBkg );
	}


	DWORD dwBtnStyle = 0;
	if( !bWndEnabled )
	{
		dwBtnStyle |= DFCS_INACTIVE;
	}
	dwBtnStyle |= m_bDropListVisible ? (DFCS_SCROLLDOWN|DFCS_PUSHED|DFCS_FLAT) : DFCS_SCROLLDOWN;


	BOOL bThemeActive = FALSE;
	HRESULT hr;

	bThemeActive = g_xpStyle.UseVisualStyles();

	HTHEME hTheme = NULL;
	if( bThemeActive )
		hTheme = g_xpStyle.OpenThemeData( m_hWnd, L"COMBOBOX" );

	// Theme drop btn style
	int nDropBtnThemeStyle = 0;
	if( m_bDropListVisible )
	{
		nDropBtnThemeStyle = CBXS_PRESSED;
	}
	else
	{
		nDropBtnThemeStyle = CBXS_NORMAL;
		if( m_bDropButtonHot )
			nDropBtnThemeStyle = CBXS_HOT;
		if( !bWndEnabled )
			nDropBtnThemeStyle = CBXS_DISABLED;
	}

#if 0  // -----------------------------------------------------------
	if( m_dwACBStyle & ACBS_FLAT )
	{
		if( bThemeActive )
		{
			hr = g_xpStyle.DrawThemeBackground( hTheme, dc.m_hDC, CP_DROPDOWNBUTTON, nDropBtnThemeStyle, &m_rcDropButton, NULL);
		}
		else
		{
			dc.DrawFrameControl(m_rcDropButton, DFC_SCROLL, dwBtnStyle );
		}
	}
	else
	if( m_dwACBStyle & ACBS_STANDARD )
#endif // -----------------------------------------------------------
	{
		if( bThemeActive )
		{
			COLORREF clrBorder;
			hr = g_xpStyle.GetThemeColor( hTheme, BP_PUSHBUTTON, bWndEnabled ? PBS_NORMAL : PBS_DISABLED, TMT_BORDERCOLOR, &clrBorder );
			if( FAILED( hr ) )
			{
				clrBorder = RGB(0,0,0);
			}
			CPen penBorder( PS_SOLID, 0, clrBorder );
			CPen* oldBorderPen = dc.SelectObject( &penBorder );
			dc.Rectangle( &rect );
			m_rcDropButton.DeflateRect(0,1,0,1);
			m_rcDropButton.left -= 1;
			m_rcDropButton.right -= 1;

			if( !bWndEnabled )
			{
				COLORREF clrDisabledLightBorder;
				COLORREF clrDisabledFill;
				hr = g_xpStyle.GetThemeColor( hTheme, BP_PUSHBUTTON, bWndEnabled ? PBS_NORMAL : PBS_DISABLED, TMT_FILLCOLOR, &clrDisabledLightBorder );
				if( FAILED( hr ) )
				{
					clrDisabledLightBorder = RGB(255,255,255);
				}
				hr = g_xpStyle.GetThemeColor( hTheme, WP_DIALOG, 0, TMT_FILLCOLOR, &clrDisabledFill );
				if( FAILED( hr ) )
				{
					clrDisabledFill = RGB(255,0,0);
				}
				CPen penDisBorder( PS_SOLID, 0, clrDisabledLightBorder );
				CBrush brFill( clrDisabledBkg );//clrDisabledFill );
				CRect rcl = rect;
				rcl.DeflateRect(1,1);
				rcl.right = m_rcDropButton.left;
				CBrush *oldBr = dc.SelectObject( &brFill );
				dc.SelectObject( &penDisBorder );
				dc.Rectangle( &rcl );
				dc.SelectObject( oldBr );
			}

			dc.SelectObject( &oldBorderPen );
			// Button
			hr = g_xpStyle.DrawThemeBackground( hTheme, dc.m_hDC, CP_DROPDOWNBUTTON, nDropBtnThemeStyle, &m_rcDropButton, NULL);
		}
		else
		{
			COLORREF clrTopLeft = ::GetSysColor(COLOR_3DSHADOW);
			COLORREF clrBottomRight = ::GetSysColor(COLOR_3DHILIGHT);
			dc.Draw3dRect( &rect, clrTopLeft, clrBottomRight );
			clrTopLeft = ::GetSysColor(COLOR_3DDKSHADOW);
			clrBottomRight = ::GetSysColor(COLOR_3DLIGHT);
			rect.DeflateRect(1,1);
			dc.Draw3dRect( &rect, clrTopLeft, clrBottomRight );
			m_rcDropButton.DeflateRect(0,2,0,2);
			m_rcDropButton.left -= 2;
			m_rcDropButton.right -= 2;
			// Button
			dc.DrawFrameControl(m_rcDropButton, DFC_SCROLL, dwBtnStyle );
		}

		//
		// Adjust rects
		rcText.DeflateRect(4,3,2,3);
	}

	if( bThemeActive )
		hr = g_xpStyle.CloseThemeData( hTheme );



	if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWNLIST
	{
		//
		// Draw Text as selected
		COLORREF clrBackground;
		COLORREF clrOldBkColor;
		COLORREF clrOldTextColor;
		clrBackground = ::GetSysColor(COLOR_HIGHLIGHT);
		clrOldBkColor = dc.SetBkColor( clrBackground );
	//	clrOldTextColor = dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CFont* pOldFont = dc.SelectObject( m_pFont );
		rcText.top -= 2;
		rcText.bottom += 2;
		rcText.left -= 2;
		rcText.right += 1;

		if( m_bHasFocus && !m_bDropListVisible )
		{
			dc.FillSolidRect( rcText, bWndEnabled ? clrBackground : clrDisabledBkg );
			clrOldTextColor = dc.SetTextColor( 
				bWndEnabled ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : clrDisabledText );
			dc.DrawText( m_strEdit.c_str(), &rcText, DT_SINGLELINE|DT_VCENTER);
		}
		else
		{
			//+++dc.FillSolidRect( rcText, 
			//+++	bWndEnabled ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : clrDisabledBkg );
			dc.FillSolidRect( rcText, 
				bWndEnabled ? ::GetSysColor(COLOR_WINDOW) : clrDisabledBkg );
			clrOldTextColor = dc.SetTextColor( 
				bWndEnabled ? ::GetSysColor(COLOR_BTNTEXT) : clrDisabledText );
			dc.DrawText( m_strEdit.c_str(), &rcText, DT_SINGLELINE|DT_VCENTER);
		}

		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
	}
	else
	{
		if( m_pEdit )
		{
			m_pEdit->SetFont( m_pFont );
		}
	}
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CAdvComboBox::OnEraseBkgnd(CDC* pDC) 
{
	return CWnd::OnEraseBkgnd(pDC);
//	return TRUE;	
}


int CAdvComboBox::SetItemHeight(int nIndex, int nHeight)
{
	if( nIndex == -1 )
	{
		if( nHeight < 10 || nHeight > 50 )
		{
			return CB_ERR;
		}
		else
		{
			//
			// Button rect
			GetClientRect(m_rcDropButton);
			m_rcDropButton.left = m_rcDropButton.right - ::GetSystemMetrics(SM_CXHSCROLL);

			return 0;
		}
	}
	return CB_ERR;
}

void CAdvComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this )
	{
		SetFocus();
	}

	//
	// Is mouse over drop button?
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_rcDropButton.PtInRect( point ) )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			InvalidateRect( m_rcDropButton );
			Invalidate();
		}
	}
	else
	if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWNLIST
	{
		CRect rc = m_rcCombo;
		GetClientRect( &rc );
		if( rc.PtInRect( point ) )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

LONG CAdvComboBox::OnSelectedItem( WPARAM wParam, LPARAM /*lParam*/ )
{
	list<LIST_ITEM> itemlist;
	list<LIST_ITEM>::iterator itemiter;

	int nPos = (int)wParam;
	itemlist = m_pDropWnd->GetList();
	itemiter = itemlist.begin();
	advance( itemiter, nPos );
	m_strEdit = itemiter->strText;

	m_nCurSel = FindStringExact( 0, m_strEdit.c_str() );

	SetWindowText( m_strEdit.c_str() );
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_pEdit )
		{
			m_pEdit->SetWindowText( m_strEdit.c_str() );
			m_pEdit->SetFocus();
			m_pEdit->SetSel( 0, -1, TRUE );
		}
	}
	// Send message to parent(dialog)
	m_bSelItem = true;
	int nId = GetDlgCtrlID();
	m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );

	Invalidate();
	OnDestroyDropdownList(0,0);

	//
	// See to it that the drop button is redrawn
	InvalidateRect( m_rcDropButton );

	m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );

	return TRUE;
}

LONG CAdvComboBox::OnDropdownButton( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::OnDropdownButton\n"));
	//
	//
	if( !m_bDropListVisible )
	{
		//
		// Button is pressed
		//
		// Create list
		if( !m_pDropWnd )
		{
			CreateDropList( m_list );
		}
		m_pDropWnd->ShowWindow( SW_SHOW );
		m_bDropListVisible = TRUE;
	}
	else
	{
		OnDestroyDropdownList(0,0);
	}

	// Return TRUE if OK to go back, else return FALSE.
	return TRUE;
}

LONG CAdvComboBox::OnDestroyDropdownList( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::OnDestroyDropdownList\n"));
	//
	// 
	if( m_pDropWnd )
	{
		m_pDropWnd->GetWindowRect( &m_rcDropWnd );
		m_bDropRectStored = true;
		m_pDropWnd->ShowWindow( SW_HIDE );
		m_bDropListVisible = FALSE;
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;

		InvalidateRect( &m_rcDropButton );
		int nId = GetDlgCtrlID();
		if( !m_bSelItem )
		{
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDCANCEL), 
				(LPARAM)m_hWnd );
		}
		else
		{
			m_bSelItem = false;
		}
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_CLOSEUP), (LPARAM)m_hWnd );
	}
	else
	{
		OnComboComplete();
	}

	return TRUE;
}

void CAdvComboBox::CreateDropList( list<LIST_ITEM> &droplist)
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::CreateDropList\n"));
	CRect rc;
	if( m_pDropWnd )
		ASSERT(0);
	m_pDropWnd = new CDropWnd( this, droplist, m_dwACBStyle );
	GetWindowRect( &rc );
	rc.top = rc.bottom ;

	//
	// Get screen size
	CRect rcWorkArea;
	SystemParametersInfo( SPI_GETWORKAREA, 0, (LPRECT)rcWorkArea, 0) ;
	if( rc.bottom >= rcWorkArea.bottom )
	{
		rc.bottom = rcWorkArea.bottom;
	}
	else
	{
	}

	int nStyle = WS_CHILD|/*WS_BORDER|*/LBS_DISABLENOSCROLL|LBS_NOTIFY;
	m_pDropWnd->Create( 0, 0, nStyle , rc, 1 ? GetDesktopWindow() : this, 6 );

	//+++
	if (m_nCurSel > ((int)droplist.size() - 1))
		m_nCurSel = 0;

	m_pDropWnd->GetListBoxPtr()->SetCurSel( m_nCurSel );

	m_pDropWnd->SetFont( m_pFont );

	// Send message to parent(dialog)
	int nId = GetDlgCtrlID();
	m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_DROPDOWN), (LPARAM)m_hWnd );

}

int CAdvComboBox::GetLBText(int nIndex, LPTSTR lpszText)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		return CB_ERR;
	}

	_tcscpy( lpszText, m_iter->strText.c_str() );
	return m_iter->strText.length()+1;
}

void CAdvComboBox::GetLBText(int nIndex, CString &rString)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		rString = "";
		return;
	}
	rString = m_iter->strText.c_str();
}

int CAdvComboBox::GetLBTextLen(int nIndex )
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		return CB_ERR;
	}

	return m_iter->strText.length()+1;
}

int CAdvComboBox::AddString(LPCTSTR lpszString)	//+++
{
	LIST_ITEM item;
	item.strText = lpszString;
	item.bChecked = false;
	item.bDisabled = false;
	item.vpItemData = NULL;
	m_list.push_back( item );

	// this takes too long -
	// sorting is handled in CXListCtrl::SetComboBox()

	//if( GetStyle() & CBS_SORT )
	//{
	//	m_list.sort();
	// Find new item
	//	return FindString( -1, item.strText.c_str() );
	//}
	//else
		return m_list.size()-1;
}

int CAdvComboBox::GetText(LPTSTR lpszText)
{
	if( m_pEdit )
	{	
		CString str;
		m_pEdit->GetWindowText( str );
		_tcscpy( lpszText, (LPCTSTR)str );
		return str.GetLength();
	}
	else
	{
		_tcscpy( lpszText, m_strEdit.c_str() );
		return m_strEdit.length();
	}
}

void CAdvComboBox::GetText(CString &rString)
{
	if( m_pEdit )
	{	
		m_pEdit->GetWindowText( rString );
	}
	else
	{
		rString = m_strEdit.c_str();
	}
}

void CAdvComboBox::SetText(LPCTSTR lpszText)
{
	if( m_pEdit )
	{	
		m_pEdit->SetWindowText( lpszText );
	}
	m_strEdit = lpszText;
}

BOOL CAdvComboBox::PointInWindow(CPoint ptScreenPoint)
{
	CRect rc;
	GetWindowRect( &rc );
	return rc.PtInRect( ptScreenPoint );
}

BOOL CAdvComboBox::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
	// TODO: Add your message handler code here and/or call default
	if( !m_bDropListVisible )
	{
		string str;
		//
		// Select another string from the map
		m_zDelta += zDelta;
		if( m_zDelta >= WHEEL_DELTA )
		{
			//
			// Select item upwards
			m_zDelta = 0;
			SelPrevItem();
		}
		else
		if( m_zDelta <= -WHEEL_DELTA )
		{
			//
			// Select item downwards
			m_zDelta = 0;
			SelNextItem();
		}
	}
	else
	{
		//
		// Handle mousewheel for the droplist here
		//
		// Select another string from the map
		m_zDelta += zDelta;
		if( m_zDelta >= WHEEL_DELTA )
		{
			//
			// Scroll list upwards
			m_zDelta = 0;
			int nTop = m_pDropWnd->GetListBoxPtr()->GetTopIndex();
			nTop -= 3;
			nTop = nTop < 0 ? 0 : nTop;
			m_pDropWnd->GetListBoxPtr()->SetTopIdx( nTop, TRUE );
		}
		else
		if( m_zDelta <= -WHEEL_DELTA )
		{
			//
			// Scroll list downwards
			m_zDelta = 0;
			int nTop = m_pDropWnd->GetListBoxPtr()->GetTopIndex();
			nTop += 3;
			nTop = nTop > m_pDropWnd->GetListBoxPtr()->GetCount() ? 
						m_pDropWnd->GetListBoxPtr()->GetCount() : nTop;
			m_pDropWnd->GetListBoxPtr()->SetTopIdx( nTop, TRUE );
		}
	}
	return TRUE;
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CAdvComboBox::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	//
	// Move Dropdown?
}


void CAdvComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	m_bHasFocus = true;
	Invalidate();
	//
	// Set focus to the edit control? (CBS_DROPDOWN)
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_pEdit )
		{
			m_pEdit->SetFocus();
		}
	}
	BOOL bDropdownList = (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE);
	if( bDropdownList )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SETFOCUS), (LPARAM)m_hWnd );
	}
}

void CAdvComboBox::OnSetfocusEdit() 
{
	m_bHasFocus = false;

	//CWnd* pWnd = GetFocus();

	//+++
	if (m_pEdit)
	{
		m_pEdit->PostMessage(EM_SETSEL, 0, -1);
	}

	if( !m_bHasSentFocus )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SETFOCUS), (LPARAM)m_hWnd );
		m_bHasSentFocus = true;
	}
}

void CAdvComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::OnKillFocus\n"));
	CWnd::OnKillFocus(pNewWnd);

	OnEscapeKey();

	//+++
#if 0  // -----------------------------------------------------------
	// Needed for keydown's like 'Alt-C'("&Cancel" button)
	if( m_pDropWnd ) 
	{
		OnDestroyDropdownList(0,0);
	//}
	m_bHasFocus = false;
	Invalidate();

	BOOL bDropdownList = (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE);
	if( bDropdownList && !m_pDropWnd )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDCANCEL), (LPARAM)m_hWnd );
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_KILLFOCUS), (LPARAM)m_hWnd );
	}
	}
	else
	{
		OnEscapeKey();
	}
#endif // -----------------------------------------------------------
}

void CAdvComboBox::OnKillfocusEdit() 
{
	m_bHasFocus = false;
	Invalidate();

	CWnd* pWnd = GetFocus();
	if( !m_pDropWnd && pWnd != this )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDCANCEL), (LPARAM)m_hWnd );
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_KILLFOCUS), (LPARAM)m_hWnd );
		m_bHasSentFocus = false;
		m_pEdit->SetSel(0,0);
	}

	//+++
	if (m_pDropWnd)
	{
		OnDestroyDropdownList(0,0);
	}
}

void CAdvComboBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
}

void CAdvComboBox::ModifyACBStyle(UINT nRemoveStyle, UINT nAddStyle)
{
	if( nAddStyle & ACBS_FLAT )
	{
		nRemoveStyle |= ACBS_STANDARD;
	}
	else
	if( nAddStyle & ACBS_STANDARD )
	{
		nRemoveStyle |= ACBS_FLAT;
	}
	m_dwACBStyle &= ~nRemoveStyle;
	m_dwACBStyle |= nAddStyle;
	Invalidate();
}

int CAdvComboBox::GetCount()
{
	return m_list.size();
}

int CAdvComboBox::GetCurSel()
{
	CString str;
	GetText( str );
	return FindStringExact( -1, str );
}

int CAdvComboBox::SetCurSel(int nSelect)
{
	if( nSelect == -1 )
	{
		m_nCurSel = nSelect;
		m_strEdit = _T("");
		Invalidate();
		return CB_ERR;
	}
	else
	if( m_list.size() == 0 )
	{
		m_nCurSel = nSelect;
		return CB_ERR;
	}
	else
	if( nSelect < -1 || nSelect > (int)m_list.size()-1 )
	{
		return CB_ERR;
	}
	else
	{
		m_nCurSel = nSelect;
		m_iter = m_list.begin();
		advance( m_iter, nSelect );
		m_strEdit = m_iter->strText;
		Invalidate();
		return m_nCurSel;
	}
}

int CAdvComboBox::FindString(int nStartAfter, LPCTSTR lpszString)
{
	int nPos = 0;
	m_iter = m_list.begin();
	if( nStartAfter != -1 )
	{
		advance( m_iter, nStartAfter );
		nPos = nStartAfter;
	}
	for( m_iter; m_iter != m_list.end(); ++m_iter )
	{
		if( _tcsncmp( m_iter->strText.c_str(), lpszString, _tcslen(lpszString) ) == 0 )
		{
			return nPos;
		}
		nPos++;
	}
	return CB_ERR;
}

int CAdvComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind)
{
	TRACE(_T("in CAdvComboBox::FindStringExact:  nIndexStart=%d\n"), nIndexStart);

	if( nIndexStart > (int)m_list.size() && nIndexStart != -1 )
		return CB_ERR;

	int nPos = 0;
	m_iter = m_list.begin();
	if( nIndexStart != -1 )
	{
		advance( m_iter, nIndexStart );
		nPos = nIndexStart;
	}
	for( m_iter; m_iter != m_list.end(); ++m_iter )
	{
		if( _tcscmp( m_iter->strText.c_str(), lpszFind ) == 0 )
		{
			XLISTCTRL_TRACE(_T("CAdvComboBox::FindStringExact returning %d\n"), nPos);
			return nPos;
		}
		nPos++;
	}
	return CB_ERR;
}

int CAdvComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	if( nStartAfter > (int)m_list.size() )
		return CB_ERR;

	int nPos = 0;
	m_iter = m_list.begin();
	if( nStartAfter != -1 )
	{
		advance( m_iter, nStartAfter );
		nPos = nStartAfter;
	}
	for( m_iter; m_iter != m_list.end(); ++m_iter )
	{
		if( _tcscmp( m_iter->strText.c_str(), lpszString ) == 0 )
		{
			m_nCurSel = nPos;
			m_strEdit = m_iter->strText;
			Invalidate();
			return nPos;
		}
		nPos++;
	}
	return CB_ERR;
}

int CAdvComboBox::SetItemData(int nIndex, DWORD dwItemData)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		return CB_ERR;
	}
	m_iter->vpItemData = (void*)dwItemData;
	return CB_OKAY;
}

DWORD CAdvComboBox::GetItemData(int nIndex)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		return (DWORD)CB_ERR;
	}
	return (DWORD)m_iter->vpItemData;
}

int CAdvComboBox::SetItemDataPtr(int nIndex, void *pData)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		return (DWORD)CB_ERR;
	}
	m_iter->vpItemData = pData;
	return CB_OKAY;
}

void* CAdvComboBox::GetItemDataPtr(int nIndex)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter == m_list.end() || nIndex > (int)m_list.size() )
	{
		return (void*)CB_ERR;
	}
	return m_iter->vpItemData;
}

void CAdvComboBox::ResetContent()
{
	m_list.clear();
	m_strEdit = _T("");
	Invalidate();
}

void AFXAPI DDX_ACBIndex( CDataExchange* pDX, int nIDC, int& index )
{
	/*HWND hWnd =*/ pDX->PrepareCtrl( nIDC );
	if( pDX->m_bSaveAndValidate )
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		index = pACB->GetCurSel();
	}
	else
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		pACB->SetCurSel( index );
	}
}

void AFXAPI DDX_ACBString( CDataExchange* pDX, int nIDC, CString& value )
{
	/*HWND hWnd =*/ pDX->PrepareCtrl( nIDC );
	if( pDX->m_bSaveAndValidate )
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		pACB->GetText( value );
	}
	else
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		pACB->SetText( value );
	}
}

BOOL CAdvComboBox::GetItemDisabled( int nIndex )
{
	if( nIndex > (int)m_list.size() )
		return CB_ERR;

	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	return m_iter->bDisabled;
}

void CAdvComboBox::SetItemDisabled(int nIndex, BOOL bDisabled)
{
	if( nIndex > (int)m_list.size() )
		return;

	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	m_iter->bDisabled = bDisabled;
}

BOOL CAdvComboBox::GetItemChecked( int nIndex )
{
	if( nIndex > (int)m_list.size() )
		return CB_ERR;

	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	return m_iter->bChecked;
}

void CAdvComboBox::SetItemChecked(int nIndex, BOOL bChecked)
{
	if( nIndex > (int)m_list.size() )
		return;

	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	m_iter->bChecked = bChecked;
}

BOOL CAdvComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		m_bAutoAppend = TRUE;

		XLISTCTRL_TRACE(_T("CAdvComboBox: Key was pressed(AdvComboBox)\n"));

		if( pMsg->wParam == VK_RETURN )
		{
			XLISTCTRL_TRACE(_T("CAdvComboBox: VK_RETURN\n"));
			if( m_pDropWnd )
			{
				int nPos = m_pDropWnd->GetListBoxPtr()->GetCurSel();
				if (nPos != LB_ERR)	//+++
					SendMessage( WM_SELECTED_ITEM, (WPARAM)nPos );
				//else				//+++
					OnDestroyDropdownList(0,0);	//+++
			}
			else
			{
				return CWnd::PreTranslateMessage(pMsg);
			}
		}
		else
		if( pMsg->wParam == VK_ESCAPE )
		{
			XLISTCTRL_TRACE(_T("CAdvComboBox: VK_ESCAPE\n"));
			if( m_pDropWnd )
			{
				OnDestroyDropdownList(0,0);
				Invalidate();
			}
			else	//+++
			{
				OnEscapeKey();
				return CWnd::PreTranslateMessage(pMsg);
			}
		}
		else
		if( pMsg->wParam == VK_F4 )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
		else
		if( pMsg->wParam == VK_UP )
		{
			SelPrevItem();
		}
		else
		if( pMsg->wParam == VK_DOWN )
		{
			SelNextItem();
		}
		else
		if( pMsg->wParam == VK_PRIOR )	//+++
		{
			SelPrevPage();
		}
		else
		if( pMsg->wParam == VK_NEXT )	//+++
		{
			SelNextPage();
		}
		else
		if( pMsg->wParam == VK_DELETE || pMsg->wParam == VK_BACK )
		{
			m_bAutoAppend = FALSE;
			return CWnd::PreTranslateMessage(pMsg);
		}
		else
		if( pMsg->wParam == VK_RIGHT )
		{
			if( m_dwACBStyle & ACBS_AUTOAPPEND )
			{
				// If the cursor is at the end of the text, show autosuggest text
				if( m_pEdit )
				{
					int nS, nE;
					m_pEdit->GetSel( nS, nE );
					if( nS == nE && nS == m_pEdit->LineLength() )
					{
						OnUpdateEdit();
					}
					else
					{
						return CWnd::PreTranslateMessage(pMsg);
					}
				}
			}
			else
			{
				return CWnd::PreTranslateMessage(pMsg);
			}
		}
		else
		{
			return CWnd::PreTranslateMessage(pMsg);
		}
		return TRUE;
	}
	else
	if( pMsg->message == WM_SYSKEYDOWN )
	{
		if( pMsg->wParam == VK_DOWN ||
			pMsg->wParam == VK_UP )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CAdvComboBox::SelPrevItem()
{
	if( m_pDropWnd )
	{
		int nPos = m_pDropWnd->GetListBoxPtr()->GetCurSel();
		if( nPos > 0 )
		{
			m_pDropWnd->GetListBoxPtr()->SetCurSel( --nPos );
		}
	}
	else
	{
		m_iter = m_list.begin();
		advance( m_iter, m_nCurSel );
		--m_iter;
		int nOldSel = m_nCurSel;
		int nPos = m_nCurSel;
		while( m_iter != m_list.end() )
		{
			nPos--;
			if( !m_iter->bDisabled )
			{
				m_strEdit = m_iter->strText;
				if( m_pEdit )
					m_pEdit->SetWindowText( m_strEdit.c_str() );
				m_nCurSel = nPos;
				Invalidate();
				break;
			}
			--m_iter;
		}
		if( nOldSel != m_nCurSel )
		{
			// Send message to parent(dialog)
			int nId = GetDlgCtrlID();
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );
		}
	}
}

void CAdvComboBox::SelNextItem()
{
	if( m_pDropWnd )
	{
		int nPos = m_pDropWnd->GetListBoxPtr()->GetCurSel();
		if( nPos < m_pDropWnd->GetListBoxPtr()->GetCount() )
		{
			m_pDropWnd->GetListBoxPtr()->SetCurSel( ++nPos );
		}
	}
	else
	{
		m_iter = m_list.begin();
		advance( m_iter, m_nCurSel );
		++m_iter;
		int nOldSel = m_nCurSel;
		int nPos = m_nCurSel;
		while( m_iter != m_list.end() )
		{
			nPos++;
			if( !m_iter->bDisabled )
			{
				m_strEdit = m_iter->strText;
				if( m_pEdit )
					m_pEdit->SetWindowText( m_strEdit.c_str() );
				Invalidate();
				m_nCurSel = nPos;
				break;
			}
			++m_iter;
		}
		if( nOldSel != m_nCurSel )
		{
			// Send message to parent(dialog)
			int nId = GetDlgCtrlID();
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );
		}
	}
}

//+++
void CAdvComboBox::SelNextPage()
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::SelNextPage\n"));
	if (m_pDropWnd)
	{
		CDropListBox * pListBox = m_pDropWnd->GetListBoxPtr();
		ASSERT(pListBox);

		int nPos        = pListBox->GetCurSel();
		int nCount      = pListBox->GetCount();
		int nMaxVisible = pListBox->GetMaxVisibleItems();
		int nTop        = pListBox->GetTopIndex();

		if (nPos == LB_ERR)							// no selection
			nPos = 0;
		else if (nPos == (nCount - 1))				// end of list
			/*nPos = -1*/;
		else if (nPos < (nTop + nMaxVisible - 1))	// middle of listbox window
			nPos = nTop + nMaxVisible - 1;
		else										// at end of listbox window
			nPos = nTop + nMaxVisible + nMaxVisible - 2;

		if (nPos >= nCount)							// moved past end, reset
			nPos = nCount - 1;

		pListBox->SetCurSel(nPos);
		
		int nSel = pListBox->GetCurSel();
		if (nSel == LB_ERR)
		{
		}
		else
		{
			CString str = _T("");
			pListBox->GetText(nSel, str);
			SetWindowText(str);
		}
	}
	else
	{
		// combo list not dropped

		int nSize = m_list.size();
		m_iter = m_list.begin();
		if (m_nCurSel < 0 || m_nCurSel >= nSize)
			m_nCurSel = 0;
		advance(m_iter, m_nCurSel);
		int nOldSel = m_nCurSel;
		int nPos = m_nCurSel;

		// Page Down goes forward by 10
		for (int i = 0; i < 10; i++)  
		{
			if (nPos >= (nSize-1))
			{
				nPos = nSize - 1;
				break;
			}

			nPos++;
			m_iter++;
		}

		if (nOldSel != nPos)
		{
			m_strEdit = m_iter->strText;
			if (m_pEdit)
				m_pEdit->SetWindowText(m_strEdit.c_str());
			Invalidate();
			m_nCurSel = nPos;

			// Send message to parent(dialog)
			int nId = GetDlgCtrlID();
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );
		}
	}
}

//+++
void CAdvComboBox::SelPrevPage()
{
	if (m_pDropWnd)
	{
		CDropListBox * pListBox = m_pDropWnd->GetListBoxPtr();
		ASSERT(pListBox);

		int nPos        = pListBox->GetCurSel();
		int nCount      = pListBox->GetCount();
		int nMaxVisible = pListBox->GetMaxVisibleItems();
		int nTop        = pListBox->GetTopIndex();

		if (nPos <= 0)
			return;

		if (nPos == LB_ERR)					// no selection
			nPos = nCount - 1;
		else if (nPos == 0)					// top of list
			/*nPos = -1*/;
		else if (nPos == nTop)				// at top of listbox window
		{
			nPos = nTop - nMaxVisible + 1;	// this might be = -1
			if (nPos < 0)
				nPos = 0;
		}
		else								// middle of listbox window
			nPos = nTop;

		pListBox->SetCurSel(nPos);
		
		int nSel = pListBox->GetCurSel();
		if (nSel == LB_ERR)
		{
		}
		else
		{
			CString str = _T("");
			pListBox->GetText(nSel, str);
			SetWindowText(str);
		}
	}
	else
	{
		// combo list not dropped

		int nSize = m_list.size();
		m_iter = m_list.begin();
		if (m_nCurSel < 0 || m_nCurSel >= nSize)
			m_nCurSel = 0;
		advance(m_iter, m_nCurSel);
		int nOldSel = m_nCurSel;
		int nPos = m_nCurSel;

		// Page Up goes back by 10
		for (int i = 0; i < 10; i++)  
		{
			if (nPos <= 0)
			{
				nPos = 0;
				break;
			}

			nPos--;
			m_iter--;
		}

		if (nOldSel != nPos)
		{
			m_strEdit = m_iter->strText;
			if (m_pEdit)
				m_pEdit->SetWindowText(m_strEdit.c_str());
			Invalidate();
			m_nCurSel = nPos;

			// Send message to parent(dialog)
			int nId = GetDlgCtrlID();
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );
			m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );
		}
	}
}

int CAdvComboBox::GetTopIndex()
{
	if( m_bDropListVisible )
	{
		if( m_pDropWnd )
		{
			return m_pDropWnd->GetListBoxPtr()->GetTopIndex();
		}
	}
	return CB_ERR;
}

int CAdvComboBox::SetTopIndex(int nIndex)
{
	if( m_bDropListVisible )
	{
		if( m_pDropWnd )
		{
			return m_pDropWnd->GetListBoxPtr()->SetTopIndex(nIndex);
		}
	}
	return CB_ERR;
}

//
// Will not allocate anything. I can't see the need in doing that.
// Everything is stored in a STL list.
int CAdvComboBox::InitStorage(int nItems, UINT /*nBytes*/)
{
	return nItems;
}

void CAdvComboBox::ShowDropDown(BOOL bShowIt)
{
	if( bShowIt )
	{
		if( !m_bDropListVisible )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
	}
	else
	{
		if( m_bDropListVisible )
		{
			SendMessage( WM_DESTROY_DROPLIST );
			Invalidate();
		}
	}
}

void CAdvComboBox::GetDroppedControlRect(LPRECT lprect)
{
	if( m_bDropListVisible )
	{
		m_pDropWnd->GetWindowRect( lprect );
	}
}

BOOL CAdvComboBox::GetDroppedState()
{
	return m_bDropListVisible;
}

int CAdvComboBox::SetExtendedUI(BOOL /*bExtended*/)
{
	return CB_OKAY;
}

BOOL CAdvComboBox::GetExtendedUI()
{
	return FALSE;
}

int CAdvComboBox::DeleteString(UINT nIndex)
{
	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter != m_list.end() || nIndex > m_list.size() )
	{
		m_list.erase( m_iter );
		return m_list.size();
	}
	else
	{
		return CB_ERR;
	}
}

int CAdvComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	LIST_ITEM item;
	item.strText = lpszString;
	if( nIndex == -1 || (nIndex > (int)m_list.size()) )
	{
		m_list.push_back( item );
		return m_list.size()-1;
	}

	if( nIndex == 0 && (m_list.size()==0) )
	{
		m_list.push_back( item );
		return 0;
	}

	m_iter = m_list.begin();
	advance( m_iter, nIndex );
	if( m_iter != m_list.end() )
	{
		m_iter = m_list.insert( m_iter, item );
		int nPos = 0;
		while( m_iter != m_list.begin() )
		{
			nPos++;
			--m_iter;
		}
		if( nIndex <= m_nCurSel )
		{
			m_nCurSel++;
		}
		return nPos;
	}
	return CB_ERR;
}

DWORD CAdvComboBox::GetEditSel()
{
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_pEdit )
		{
			return m_pEdit->GetSel();
		}
	}
	return (DWORD)CB_ERR;
}

BOOL CAdvComboBox::SetEditSel(int nStartChar, int nEndChar)
{
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_pEdit )
		{
			m_pEdit->SetSel( nStartChar, nEndChar, TRUE );
			return TRUE;
		}
	}
	return CB_ERR;
}


void CAdvComboBox::OnChangeEdit()
{
	if( !m_pDropWnd )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_EDITCHANGE), (LPARAM)m_hWnd );
	}
}

void CAdvComboBox::OnUpdateEdit()
{
	static bool bAutoAppendInProgress;
	CString strEdit;
	m_pEdit->GetWindowText( strEdit );
	if( GetFocus() == m_pEdit )
	{
		if( !bAutoAppendInProgress )
		{
//			m_nCurSel = -1;
			if( m_dwACBStyle & ACBS_AUTOAPPEND && m_bAutoAppend )
			{
				tstring str = (LPCTSTR)strEdit;
				int nEditLen = str.length();
				if( !nEditLen )
					return;
				int nStartSel;
				int nEndSel;

				m_pEdit->GetSel( nStartSel, nEndSel );

				LIST_ITEM item;
				m_iter = m_list.begin();
				while( m_iter != m_list.end() )
				{
					item = *m_iter;
					int nPos = m_iter->strText.find( str, 0 );
					if( nPos == 0 )
					{
						bAutoAppendInProgress = true;
						m_pEdit->SetWindowText( m_iter->strText.c_str() );
						m_pEdit->SetSel( nEditLen, m_iter->strText.length(), TRUE );
						bAutoAppendInProgress = false;
						break;
					}
					++m_iter;
				}
			}
		}

		if( !bAutoAppendInProgress )
		{
			if( m_dwACBStyle & ACBS_AUTOSUGGEST )
			{
				list<LIST_ITEM> suggestlist;
				list<LIST_ITEM>::iterator suggestiter;
				tstring str = (LPCTSTR)strEdit;
				int nEditLen = str.length();
				if( !nEditLen )
				{
					if( m_pDropWnd )
						SendMessage( WM_DESTROY_DROPLIST );
					return;
				}
				int nStartSel;
				int nEndSel;

				m_pEdit->GetSel( nStartSel, nEndSel );

				LIST_ITEM item;
				m_iter = m_list.begin();
				while( m_iter != m_list.end() )
				{
					item = *m_iter;
					int nPos = m_iter->strText.find( str, 0 );
					if( nPos == 0 )
					{
						if (m_iter->strText != str)			//+++
							suggestlist.push_back( item );
					}
					++m_iter;
				}
				if( m_pDropWnd )
					SendMessage( WM_DESTROY_DROPLIST );
				//+++if( suggestlist.size() != 0 )
				if( suggestlist.size() > 1 )	//+++
				{
					m_nCurSel = 0;		//+++
					CreateDropList( suggestlist );
				}
			}
		}
	}

	if( !m_pDropWnd )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		m_pParent->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_EDITUPDATE), (LPARAM)m_hWnd );
	}
}

void CAdvComboBox::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	Invalidate();
	if( m_pEdit )
	{
		m_pEdit->EnableWindow( bEnable );
	}
	
}

BOOL CAdvComboBox::LimitText(int nMaxChars)
{
	if( m_pEdit )
	{
		int nCh = !nMaxChars ? 65535 : nMaxChars;
		m_pEdit->SetLimitText( nCh );
		return TRUE;
	}
	else
	{
		return CB_ERR;
	}
}

void CAdvComboBox::SetMinVisibleItems(int nMinItems)
{
	m_nMinVisItems = nMinItems;
}

int CAdvComboBox::GetMinVisibleItems()
{
	return m_nMinVisItems;
}

void CAdvComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	bool bChange = m_bDropButtonHot;

	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWN
	{
		if( m_rcDropButton.PtInRect( point ) )
		{
			m_bDropButtonHot = true;
		}
		else
		{
			m_bDropButtonHot = false;
		}
	}
	else
	if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWNLIST
	{
		GetClientRect( &m_rcCombo );
		if( m_rcCombo.PtInRect( point ) )
		{
			m_bDropButtonHot = true;
		}
		else
		{
			m_bDropButtonHot = false;
		}
	}


	if( bChange != m_bDropButtonHot )
	{
		if( !m_bTrackMouseLeave )
		{
			//
			// Could not use TrackMouseEvent() under Release. A bug appeared 
			// the second time the dropwindow was shown, and I did not 
			// have to strenght to go look for it. :)
/*			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			TrackMouseEvent(&tme);*/

			m_bTrackMouseLeave = true;
			SetTimer(1, 50, NULL);
		}

		InvalidateRect( &m_rcDropButton );
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CAdvComboBox::OnMouseLeave() 
{
	// TODO: Add your message handler code here and/or call default
	bool bChange = m_bDropButtonHot;
	m_bDropButtonHot = false;
	m_bTrackMouseLeave = false;

	if( bChange != m_bDropButtonHot )
		InvalidateRect( &m_rcDropButton );
}

void CAdvComboBox::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == 1 )
	{
		CPoint point;
		GetCursorPos( &point );
		ScreenToClient( &point );
		if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWN
		{
			if( !m_rcDropButton.PtInRect( point ) )
			{
				KillTimer( 1 );
				OnMouseLeave();
			}
		}
		else
		if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWNLIST
		{
			GetClientRect( &m_rcCombo );
			if( !m_rcCombo.PtInRect( point ) )
			{
				KillTimer( 1 );
				OnMouseLeave();
			}
		}
	}
	CWnd::OnTimer(nIDEvent);
}


int CAdvComboBox::GetDefaultVisibleItems()
{
	return m_nDefaultDropItems;
}

void CAdvComboBox::SetDefaultVisibleItems(int nItems)
{
	m_nDefaultDropItems = nItems;
}

//+++
// function to get combo width based on strings
UINT CAdvComboBox::GetComboWidth()
{
	CDC * pDC = GetDC();
	CFont * pFont = GetFont();

	CFont* pOldFont = pDC->SelectObject(pFont);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	CString str = _T("");
	CSize sz;
	UINT dx = 0;

	for (int i = 0; i < GetCount(); i++)
	{
		GetLBText(i, str);
		sz = pDC->GetTextExtent(str);

		sz.cx += tm.tmAveCharWidth;
		if (sz.cx > (int)dx)
			dx = sz.cx;
	}

	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE) + 5;

	return dx;
}

//+++
void CAdvComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// handle escape and return, in case combo's edit does NOT have focus

	if (m_pParent)
	{
		if (nChar == VK_ESCAPE)
		{
			OnEscapeKey();
			return;
		}
		else if (nChar == VK_RETURN)
		{
			OnComboComplete();
			return;
		}
		else if (nChar == VK_DOWN)
		{
			SelNextItem();
		}
	}
	
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CAdvComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::OnKeyDown\n"));

	if (nChar == VK_DOWN)
		SelNextItem();
	else if (nChar == VK_UP)
		SelPrevItem();
	else if (nChar == VK_NEXT)
		SelNextPage();
	else if (nChar == VK_PRIOR)
		SelPrevPage();

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAdvComboBox::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	XLISTCTRL_TRACE(_T("in CAdvComboBox::OnActivateApp\n"));
	CWnd::OnActivateApp(bActive, (DWORD) hTask);
}
