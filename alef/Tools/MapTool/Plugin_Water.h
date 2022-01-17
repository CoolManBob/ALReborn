#if !defined(AFX_PLUGIN_WATER_H__2B88678D_5D93_4B50_8BBF_F739B9A9B0F6__INCLUDED_)
#define AFX_PLUGIN_WATER_H__2B88678D_5D93_4B50_8BBF_F739B9A9B0F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Plugin_Water.h : header file
//
#include "UITileList_PluginBase.h"
#include "ApmMap.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugin_Water window

class CPlugin_Water : public CUITileList_PluginBase
{
// Construction
public:
	CPlugin_Water();

	CRect	m_rectAddWater		;
	CRect	m_rectDeleteWater	;
	CRect	m_rectHeightWater	;
	CRect	m_rectEditHWaterStatus	;

	CRect	m_rectHQWaterOnOff			;
	CRect	m_rectHQWaterWaveHeightEdit	;

// Attributes
public:
	BOOL	m_bValidPosition	;
	RwV3d	m_pos				;

	FLOAT	m_fWaterHeight		;
	BOOL	m_bShowSampleWater	;	// 기준높이 표시 플래그.

	UINT	m_uLastSetHeightTime	;	// 잠시 높이 잔상이 남아 보이게 한다.
	UINT	m_uWaterZanSangChargeTime	;

	BOOL	m_bHQWaterMode;

public:
	// Selection...
	BOOL				m_bGeometrySelection	;
	int					m_SelectedPosX1			;
	int					m_SelectedPosY1			;
	ApWorldSector *		m_pSelectedGeometry1	;
	
	int					m_SelectedPosX2			;
	int					m_SelectedPosY2			;
	ApWorldSector *		m_pSelectedGeometry2	;

// Operations
public:
	// Operations...
	void RenderRectancle		(	FLOAT fX1 , FLOAT fZ1 , FLOAT fX2 , FLOAT fZ2 , FLOAT fDHeight , 
									INT32 nRed , INT32 nGreen , INT32 nBlue , INT32 nAlpha , BOOL bBox = FALSE);

	// Virtual Functions...
	virtual BOOL OnSelectedPlugin		();
	virtual BOOL OnDeSelectedPlugin		();

	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );
	
	virtual BOOL OnWindowRender			();
	
	virtual BOOL OnRButtonDblClkGeometry( RwV3d * pPos );

	virtual BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL OnLButtonUpGeometry	( RwV3d * pPos );

	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();
	
	// Operations..
	void PositionValidation		( RwV3d	* pPos );	// 로딩 번위 벗어난것을 체크한다.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_Water)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

	CListBox		m_ctlListBox;
	CButton			m_ctlButton;
	
	int				m_iSelID;		

public:
	virtual ~CPlugin_Water();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_Water)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGIN_WATER_H__2B88678D_5D93_4B50_8BBF_F739B9A9B0F6__INCLUDED_)
