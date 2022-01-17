#if !defined(AFX_DUNGEONTOOLBAR_H__DD3C1F17_B2AD_4AF2_89F5_B95CE455AB68__INCLUDED_)
#define AFX_DUNGEONTOOLBAR_H__DD3C1F17_B2AD_4AF2_89F5_B95CE455AB68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DungeonToolBar.h : header file
//

#include "DungeonTemplateEditDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDungeonToolBar window

#define DTB_TOOLIMAGE_HEIGHT	64
// 세로 이미지로 업이미지와 눌린 이미지 두개로 들어간다.
// 즉 세로 이미지의 크기는 아이템수*2*DTB_TOOLIMAGE_HEIGHT

#define DTB_BACKGROUND_COLOR	RGB( 0 , 0 , 0 )
#define DTB_HIGHLIGHT_COLOR		RGB( 255 , 255 , 255 )
#define DTB_TEXT_COLOR			RGB( 255 , 255 , 0 )

#define WM_DUNGEONTOOLBAR_ITEM_CHANAGE	( WM_USER + 135 )
// wParam : prev
// lParam : next
#define WM_DUNGEON_UPDATE	( WM_USER + 136 )

#define DUNGEON_LAYER_HEIGHT	25

class CDungeonToolBar : public CWnd
{
// Construction
public:
	enum	IDC_CONTROL
	{
		IDC_TEMPLATE_BUTTON = 0,
		IDC_APPLY_DUNGEON	= 1,
		BUTTON_MAX			= 2
	};

	struct gdiButton
	{
		CRect	m_rect		;
		CString	m_strName	;
	};

	gdiButton	m_aButton[ BUTTON_MAX ];
	INT32		m_nButtonPressed;

	BOOL	AddButton( INT32 nIndex , CRect rect , CString strName )
	{
		m_aButton[ nIndex ].m_rect		= rect		;
		m_aButton[ nIndex ].m_strName	= strName	;

		return TRUE;
	};

	INT32	GetPressedButton( int x , int y )
	{
		for( int i = 0 ; i < BUTTON_MAX ; i ++ )
		{
			if( m_aButton[ i ].m_rect.PtInRect( CPoint( x , y ) ) )
				return i;
		}

		return -1; // ERROR
	}

	void	PressButton( int nButton )
	{
		m_nButtonPressed = nButton;
		if( nButton >= 0 && nButton < BUTTON_MAX )
			InvalidateRect( m_aButton[ nButton ].m_rect , FALSE );
	}

	void	ReleaseButton()
	{
		int nButton = m_nButtonPressed;
		m_nButtonPressed = -1;
		if( nButton >= 0 && nButton < BUTTON_MAX )
			InvalidateRect( m_aButton[ nButton ].m_rect , FALSE );
	}

	void	DrawButtons( CDC * pDC )
	{
		COLORREF	rgbBase		= RGB( 172 , 172 , 172 );
		COLORREF	rgbBright	= RGB( 225 , 225 , 225 );
		COLORREF	rgbDark		= RGB( 56 , 56 , 56 );

		CRect	rect , * pRect;

		pDC->SetBkMode( TRANSPARENT );

		for( int i = 0 ; i < BUTTON_MAX ; i++ )
		{
			rect	= m_aButton[ i ].m_rect;
			pRect	= &m_aButton[ i ].m_rect;
			rect.DeflateRect( 2 , 2 , 2 , 2 );
			pDC->FillSolidRect( rect , rgbBase );

			if( i == m_nButtonPressed )
			{
				pDC->FillSolidRect( pRect->left , pRect->top , pRect->Width() , 2 , rgbDark);
				pDC->FillSolidRect( pRect->left , pRect->top , 2 , pRect->Height() , rgbDark);

				pDC->FillSolidRect( pRect->left , pRect->bottom - 2 , pRect->Width() , 2 , rgbBright);
				pDC->FillSolidRect( pRect->right - 2 , pRect->top , 2 , pRect->Height() , rgbBright);
			}
			else
			{
				pDC->FillSolidRect( pRect->left , pRect->top , pRect->Width() , 2 , rgbBright);
				pDC->FillSolidRect( pRect->left , pRect->top , 2 , pRect->Height() , rgbBright);

				pDC->FillSolidRect( pRect->left , pRect->bottom - 2 , pRect->Width() , 2 , rgbDark);
				pDC->FillSolidRect( pRect->right - 2 , pRect->top , 2 , pRect->Height() , rgbDark);
			}

			pDC->DrawText( m_aButton[ i ].m_strName , pRect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );			
		}
	}

	FLOAT	UpdateDungeonWallHeight( UINT32 uDungeonTemplateID , BOOL bUpdateForce = FALSE );

public:
	CDungeonToolBar();

	struct DungeonToolItem
	{
		UINT32	uItemIndex	;
		CString	strName		;
	};

	DungeonToolItem	m_aToolItem[ 8 ];
	UINT32			m_nCurrentToolItem	;
	UINT32			m_nToolItemCount	;

	CBitmap			m_bmpTool			;
	//CButton			m_ctlTemplateButton	;
	//CButton			m_ctlApply			;

	BOOL	AddDungeonToolItem( UINT32 uItemIndex , CString strName );
	void	UpdateTemplate		();

	BOOL	SelectItem			( UINT32 uItem );
	UINT32	GetItem				();
	UINT32	m_uCurrentTemplateIndex;

	// 레이어 처리..
	INT32	m_nCurrentLayer			;
	INT32	SetLayer( INT32 nLayer );
	INT32	GetLayer() { return m_nCurrentLayer; }
	FLOAT	GetHeight();

	CRect	m_rectLayerSelect;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDungeonToolBar)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDungeonToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDungeonToolBar)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUNGEONTOOLBAR_H__DD3C1F17_B2AD_4AF2_89F5_B95CE455AB68__INCLUDED_)
