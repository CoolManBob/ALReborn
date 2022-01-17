#if !defined(AFX_ALEFTOOLBAR_H__584AB6E8_8170_40D3_8F2A_E3F8589069DC__INCLUDED_)
#define AFX_ALEFTOOLBAR_H__584AB6E8_8170_40D3_8F2A_E3F8589069DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlefToolBar.h : header file
//

#define	TOOLBAR_ICON_WIDTH		20

enum TOOLBAR_INDEX
{
	TOOLBAR_GOTO_SELECT_GEOMETRY,
	TOOLBAR_LIGHTSETTING		,
	TOOLBAR_MOVE_LIGHT			,

	TOOLBAR_TOGGLE_SHOW_GRID	,
	TOOLBAR_TOGGLE_RENDER_STATE	,

	TOOLBAR_INDEX_MAX			,

	TOOLBAR_TOGGLE_SHOW_OBJECT	,
	TOOLBAR_TOGGLE_LIGHT		,
	TOOLBAR_TEST				
};

#define	WM_TOOLBAR_SELECT		( WM_USER + 1004 )
// lParam : Selected Icon Index
// wParam : reserved
#define	WM_TOOLBAR_DETAILCHANGE	( WM_USER + 1005 )
// lParam : Selected Detail..
// wParam : TYPE

#define	WM_TOOLBAR_TIME_UPDATE	( WM_USER + 1006 )

enum DETAILCHANGETYPE
{
	DETAIL_NOTIFY	,	// 알림..
	DETAIL_SET			// 타 윈도우로 넘김.
};

#define	DETAIL_COMBOBOX_WIDTH	200
#define	CLOCK_WIDTH				100

#define	FPS_EDITMODE_WIDTH		140

/////////////////////////////////////////////////////////////////////////////
// CAlefToolBar window
#include "ChildView.h"

class CAlefToolBar : public CWnd
{
// Construction
public:
	CAlefToolBar();

// Parameter
	CImageList	m_imageList_Toolbar_up	;
	CImageList	m_imageList_Toolbar_down;

	int			m_nSelectedIndex		;
	BOOL		m_bLButtonDown			;
	
	int			m_nCurrentDetail		;
	
	int			m_nHour					;
	int			m_nMinute				;

	bool		m_bStopTimer			;
	bool		m_bSpeedTimer			;
	int			m_nHourCount			;
	bool		m_bPrevTimerSetting		;

// Attributes
public:


// Operations
public:
	void OnChangeDetailCombo();

	void ToggleTimer		();
	void AllRound			();

	void ToggleFPSEditMode	();
	void SetFPSEditMode		( BOOL bEdit );
	BOOL GetFPSEditMode		();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlefToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDetail( int detail );
	virtual ~CAlefToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAlefToolBar)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALEFTOOLBAR_H__584AB6E8_8170_40D3_8F2A_E3F8589069DC__INCLUDED_)
