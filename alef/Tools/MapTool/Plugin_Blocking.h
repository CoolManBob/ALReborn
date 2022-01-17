// Plugin_Blocking.h: interface for the Plugin_Blocking class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGIN_BLOCKING_H__887AAD35_F302_4013_87FC_5954DEF86DA7__INCLUDED_)
#define AFX_PLUGIN_BLOCKING_H__887AAD35_F302_4013_87FC_5954DEF86DA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#define	DEFAULT_VERTEX_BUFFER_SIZE	( 20000 * 3 )

class CPlugin_Blocking : public CUITileList_PluginBase  
{
protected:
	RwIm3DVertex	*	m_pVertexBuffer	;
	INT32				m_nVertexCount	;
	INT32				m_nTriangleCount;

	CBrush				m_Brush			;
	
	enum
	{
		GEOMETRY_BLOCKING	,	// 지형에 저장된 블러킹정보
		OBJECT_BLOCKING		,	// 오브젝트의 충돌을 이용한 테이블..
		OBJECT_RIDABLE		,
		SKY_BLOCKING		,
		MAX_BLOCKING_COUNT
	};

	INT32				m_nType			;

	//void	AddObjectBlockingPolygon		( ApWorldSector	* pSector , int x , int z , FLOAT fXOrigin , FLOAT fZOrigin , ApDetailSegment *	pSegment );
	void	AddGeometryBlockingPolygon		( ApWorldSector	* pSector , int x , int z , FLOAT fXOrigin , FLOAT fZOrigin , ApDetailSegment *	pSegment );
	void	AddGeometryBlockingTablePolygon	( ApWorldSector	* pSector , int x , int z , FLOAT fXOrigin , FLOAT fZOrigin , ApDetailSegment *	pSegment );

	BOOL				m_bLButtonDown	;
public:
	// Operations..
	BOOL	AllocVertexBuffer		( INT32 nSize );
	void	FreeVertexBuffer		();
	void	UpdateBlockingPolygon	( BOOL bGeometry = TRUE , BOOL bObject = TRUE);
	
	// Virtual Functions...
	virtual BOOL OnSelectedPlugin	();
	virtual BOOL OnDeSelectedPlugin	();

	virtual BOOL OnWindowRender			();
	
	virtual BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL OnLButtonUpGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );

	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();

	CPlugin_Blocking();
	virtual ~CPlugin_Blocking();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_Blocking)
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_Blocking)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_PLUGIN_BLOCKING_H__887AAD35_F302_4013_87FC_5954DEF86DA7__INCLUDED_)
