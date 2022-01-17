#if !defined(AFX_UITILELIST_OTHERS_H__D9285062_66D9_403F_A998_EB99A34595E3__INCLUDED_)
#define AFX_UITILELIST_OTHERS_H__D9285062_66D9_403F_A998_EB99A34595E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_Others.h : header file
//

#include "aulist.h"
#include "UITileList_PluginBase.h"
#include "Plugin_Water.h"

/////////////////////////////////////////////////////////////////////////////
// CUITileList_Others window

#define	UITILELIST_TEXT_HEIGHT	20

class CUITileList_Others : public CWnd
{
// Construction
public:
	CUITileList_Others();

	struct	Plugin
	{
		CString						name	;
		COLORREF					color	;
		CUITileList_PluginBase	*	pWnd	;
	};

	AuList< Plugin >	m_listPlugin		;
	AuNode< Plugin > *	m_pCurrentPlugin	;
// Attributes
public:
	Plugin * GetCurrentPlugin()
	{
		if( m_pCurrentPlugin ) return &m_pCurrentPlugin->GetData();
		else return NULL;
	}

// Operations
public:
	BOOL AddPlugin				( CUITileList_PluginBase * pPlugin , CString name , COLORREF color );

	BOOL LButtonDownGeometry	( RwV3d * pPos );
	BOOL LButtonUpGeometry		( RwV3d * pPos );
	BOOL LButtonDblClkGeometry	( RwV3d * pPos );
	BOOL RButtonDownGeometry	( RwV3d * pPos );
	BOOL RButtonUpGeometry		( RwV3d * pPos );
	BOOL RButtonDblClkGeometry	( RwV3d * pPos );
	BOOL MouseMoveGeometry		( RwV3d * pPos );
	
	BOOL SelectObject			( RpClump * pClump );

	BOOL Window_LButtonDown		( RsMouseStatus *ms );
	BOOL Window_LButtonUp		( RsMouseStatus *ms );
	BOOL Window_LButtonDblClk	( RsMouseStatus *ms );
	BOOL Window_RButtonDown		( RsMouseStatus *ms );
	BOOL Window_RButtonUp		( RsMouseStatus *ms );
	BOOL Window_RButtonDblClk	( RsMouseStatus *ms );
	BOOL Window_MouseMove		( RsMouseStatus *ms );
	BOOL Window_MouseWheel		( BOOL bForward		);
	
	BOOL Window_KeyDown			( RsKeyStatus *ks );
	
	void ChangeSectorGeometry	( ApWorldSector * pSector );

	BOOL OnWindowRender			();
	BOOL OnIdle					( UINT32 ulClockCount );
	
	BOOL CleanUpData			();
	BOOL Export					( BOOL bServer );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_Others)
	//}}AFX_VIRTUAL

// Implementation
public:
	void	OnChangePlugin		( Plugin * pPrev , Plugin * pNext );

	void	DrawMenu( CDC * pDC );
	int		GetMenuIndex( CPoint point );
	int		GetPluginTop( int nIndex );
	virtual ~CUITileList_Others();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileList_Others)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_OTHERS_H__D9285062_66D9_403F_A998_EB99A34595E3__INCLUDED_)
