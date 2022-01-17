#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#define	DEFAULT_VERTEX_BUFFER_SIZE	( 20000 * 3 )

class CPlugin_NoLayer : public CUITileList_PluginBase  
{
protected:
	RwIm3DVertex	*	m_pVertexBuffer	;
	INT32				m_nVertexCount	;
	INT32				m_nTriangleCount;

	void	AddGeometryBlockingPolygon		( ApWorldSector	* pSector , int x , int z , FLOAT fXOrigin , FLOAT fZOrigin , ApDetailSegment *	pSegment );

	BOOL				m_bLButtonDown	;
public:
	// Operations..
	BOOL	AllocVertexBuffer		( INT32 nSize );
	void	FreeVertexBuffer		();
	void	UpdateBlockingPolygon	();
	
	// Virtual Functions...
	virtual BOOL OnSelectedPlugin	();
	virtual BOOL OnDeSelectedPlugin	();

	virtual BOOL OnWindowRender			();
	
	virtual BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL OnLButtonUpGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );

	CPlugin_NoLayer();
	virtual ~CPlugin_NoLayer();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_NoLayer)
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_NoLayer)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
