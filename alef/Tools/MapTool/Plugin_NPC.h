#if !defined(AFX_PLUGIN_NPC_H__FB1071C9_5BBB_4326_981D_3298C6D4CF48__INCLUDED_)
#define AFX_PLUGIN_NPC_H__FB1071C9_5BBB_4326_981D_3298C6D4CF48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Plugin_NPC.h : header file
//
#include "MyEngine.h"
#include "UITileList_PluginBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugin_NPC window

class CPlugin_NPC : public CUITileList_PluginBase
{
// Construction
public:
	CPlugin_NPC();

// Attributes
public:

protected:
	int	m_nCharacterIDCounter;
	// 케릭터 아이디용.. 하나씩 증가시키면서 사용함.
	AgpdCharacter	*	m_pCurrentCharacter	;
	RpClump			*	m_pSelectedClump	;

	CComboBox		m_wndCombo			;

	RsMouseStatus	m_PrevMouseStatus	;

	// 팝업 메뉴 관리용.
	AgpdCharacter	*	m_pPopupCurrentCharacter	;
	RpClump			*	m_pPopupSelectedClump		;


// Operations
public:

	virtual	BOOL OnSelectedPlugin		();
	virtual	BOOL OnDeSelectedPlugin		();
	
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual	BOOL OnLButtonUpGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );
	virtual	BOOL OnSelectObject			( RpClump * pClump );
	
	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();
	virtual	BOOL OnCleanUpData			();

	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks	);
	virtual	BOOL Window_OnLButtonDblClk	( RsMouseStatus *ms );

	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual	BOOL Window_OnRButtonUp		( RsMouseStatus *ms );
	virtual	BOOL Window_OnRButtonDown	( RsMouseStatus *ms );

	virtual BOOL OnWindowRender			();


	virtual void OnChangeSectorGeometry	( ApWorldSector * pSector );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_NPC)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlugin_NPC();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_NPC)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGIN_NPC_H__FB1071C9_5BBB_4326_981D_3298C6D4CF48__INCLUDED_)
