#pragma once

#include "UITileList_PluginBase.h"
#include "myengine.h"

class CPlugin_LoadBlock : public CUITileList_PluginBase
{
public:
	CPlugin_LoadBlock(void);
	~CPlugin_LoadBlock(void);

	void	MakeGridInfo			( ApWorldSector * pSector );
	void	RenderSectorGrid		( ApWorldSector * pSector );
	void	RenderSectorSelection	( ApWorldSector * pSector );

	static BOOL	CBRenderSector ( DivisionInfo * pDivisionInfo , PVOID pData );

public:
	virtual BOOL OnWindowRender			();
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_LoadBlock)
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_TextureInfo)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
