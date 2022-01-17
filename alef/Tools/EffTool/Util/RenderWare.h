#pragma once

#include <rwcore.h>
#include <skeleton.h>

#include <d3dx9core.h>
#include <d3d9.h>

class CRenderWare : public CSingleton< CRenderWare >
{
public:
	CRenderWare();
	
public:     //Public Methods
	bool	Initialise   (HWND hWnd);
	void	Destroy      (void);

	void	Render();
	BOOL	Idle();
	BOOL	MsgProc( MSG* pMsg );

	void	bRenderTo2ndWnd( HWND hWnd );

	bool	SetCameraViewSize(int dx, int dy);

	float	GetViewpotWidth()		{	return (float)RwRasterGetWidth( RwCameraGetRaster( CCamera::bGetInst().bGetPtrRwCam() ) );	}
	float	GetViewpotHeight()		{	return (float)RwRasterGetHeight( RwCameraGetRaster( CCamera::bGetInst().bGetPtrRwCam() ) );	}

	RsEventStatus	OnEventHandler( RsEvent event, void *param );
	RsEventStatus	OnKeyboardEventHandler( RsEvent event, void *param );
	RsEventStatus	OnMouseEventHandler( RsEvent event, void *param );

	LPD3DXLINE	bGetD3dxLine()		{	return m_pd3dLine;	}

private:
	LPDIRECT3DDEVICE9	m_pd3dDevice;
	LPD3DXLINE			m_pd3dLine;
	bool				m_bInitialised;
};