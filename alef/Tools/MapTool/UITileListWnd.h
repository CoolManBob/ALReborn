#if !defined(AFX_UITILELISTWND_H__0D375370_63F0_4FC9_9404_67BFBA50C611__INCLUDED_)
#define AFX_UITILELISTWND_H__0D375370_63F0_4FC9_9404_67BFBA50C611__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileListWnd.h : header file
//

#include "UITileList_MapTabWnd.h"
#include "UITileList_ObjectTabWnd.h"
#include "UITileList_TileTabWnd.h"
#include "UITileList_Others.h"

/////////////////////////////////////////////////////////////////////////////
// CUITileListWnd window

// 타일윈도우에 출력돼는 모든내용을 총괄한다.
// 기본적인 기능은 탭을 구현하게 돼며 ,,..
// 세부적인 기능은 해당윈도우를 따로 만들어 코딩하는 방식을 사용한다.
// 메뉴는 일단 텍스트로 구현..

#define	TAB_DEFUALT_COLOR		RGB( 128 , 128 , 128 )
#define	TAB_DEFUALT_BACKCOLOR	RGB( 0 , 0 , 0 )
#define	TAB_SELECTED_COLOR		RGB( 255 , 0 , 0 )
#define	TAB_SELECTED_BACKCOLOR	RGB( 200 , 200 , 200 )

#define	TAB_HEIGHT			20

class CUITileListWnd : public CWnd
{
// Construction
public:
	stMenuItem	m_pMenu[ EDITMODE_TYPECOUNT ];
	int			m_nSelectedTab		;

	CUITileListWnd();

// Attributes
public:
	// 각각 윈도우 핸들들.
	CUITileList_MapTabWnd		* m_pMapWnd			;
	CUITileList_ObjectTabWnd	* m_pObjectWnd		;
	CUITileList_TileTabWnd		* m_pTabWnd			;
	CUITileList_Others			* m_pOthers			;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileListWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void	ChangeTab		( int tab , BOOL bReserve = FALSE );
	INT32	GetCurrentMode	();
	
	void NotifyParent();
	virtual ~CUITileListWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileListWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELISTWND_H__0D375370_63F0_4FC9_9404_67BFBA50C611__INCLUDED_)
