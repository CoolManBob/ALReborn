// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__65AECF3E_4CF1_4773_94ED_A244E824B443__INCLUDED_)
#define AFX_CHILDVIEW_H__65AECF3E_4CF1_4773_94ED_A244E824B443__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 3D 출력을 위한 창임.


/////////////////////////////////////////////////////////////////////////////
// CChildView window

namespace ChildView
{
   //Flight models
   const short FLIGHT_MOUSE = 0;
   const short FLIGHT_1RPG  = 1;
}

enum MAPTOOL_MODE
{
	EDIT_GEOMETRY	,
	EDIT_TILE		,
	EDIT_OBJECT		
};

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:



// Implementation
public:
	void MakeDirectory();
	virtual  ~CChildView();

	// Generated message map functions

protected:

	// 유틸리티 메시지.
	void	OnFirstTime			( void );	// 윈도우가 그려진 직후 발생하는 메시지.
	void	LoadTextureList		( void );	// 디렉토리 구조를 읽어서 파일을찾아냄..

	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
    afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__65AECF3E_4CF1_4773_94ED_A244E824B443__INCLUDED_)
