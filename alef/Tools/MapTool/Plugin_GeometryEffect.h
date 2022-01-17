#pragma once


// CPlugin_GeometryEffect

class CPlugin_GeometryEffect : public CUITileList_PluginBase
{
	DECLARE_DYNAMIC(CPlugin_GeometryEffect)

public:
	CPlugin_GeometryEffect();
	virtual ~CPlugin_GeometryEffect();

	virtual BOOL OnWindowRender			();
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );

	void	RenderSectorGrid		( ApWorldSector * pSector );
	void	RenderSectorSelection	( ApWorldSector * pSector );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


