#if !defined(AFX_UITILELIST_MAPTABWND_H__BB06DB2A_5304_4BD4_B011_8A795E3B09E6__INCLUDED_)
#define AFX_UITILELIST_MAPTABWND_H__BB06DB2A_5304_4BD4_B011_8A795E3B09E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_MapTabWnd.h : header file
//

#include "HSVControlStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CUITileList_MapTabWnd window

#define	WM_MAPBRUSHCHANGE	( WM_USER + 1008 )
// lparam : do not use
// wparam : do not use

class CUITileList_MapTabWnd : public CWnd
{
// Construction
public:
	CUITileList_MapTabWnd();

// Attributes
public:
	stMenuItem	m_pMenu[ BRUSHTYPE_COUNT ];
	
	BOOL	m_bLButtonDown;

	CHSVControlStatic	m_ctlRGBSelectStatic;

// Operations
public:

	RwRGBA	GetVertexColor();
	void	SetVertexColor( int r , int g , int b );

	void	SetBrushType( int type );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_MapTabWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUITileList_MapTabWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileList_MapTabWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline RwRGBA RGBtoRwRGBA( COLORREF ref )
{
	RwRGBA	rgb;
	rgb.alpha	= 255;
	rgb.red		= GetRValue( ref );
	rgb.green	= GetGValue( ref );
	rgb.blue	= GetBValue( ref );
	return rgb;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_MAPTABWND_H__BB06DB2A_5304_4BD4_B011_8A795E3B09E6__INCLUDED_)
