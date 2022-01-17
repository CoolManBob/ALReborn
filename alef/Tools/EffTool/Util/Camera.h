#pragma once

#include <rwcore.h>
#include "Axis.h"

class CDlgGridToolOption;
class CCamera  
{
	friend class CDlgGridToolOption;

	RwCamera*		m_pRwCam;
												
	RwV3d			m_v3X,			//x axis	
					m_v3Y,			//y axis	
					m_v3Z;			//z axis	
	RwV3d			m_v3EyePos,		//eye pos	
					m_v3AtPos,		//at pos	
					m_v3EyeSubAt;	//eye-at

	AXIS			m_cAxis;

	RwMatrix		m_matBillboard,
					m_matBillboardY;

	BOOL			m_bActiveWnd;

public:
	 RwCamera*	bGetPtrRwCam(void) { return m_pRwCam; };
	 void		bSetActiveWnd(BOOL bActive) { m_bActiveWnd = bActive; };

protected:
	CCamera();
	void			vUpdateMatirx(RwUInt32 flags);
	void			vSetBMatrix();
	void			vSetBWYMatrix();//BWY : Billboard World Y-axis
public:
	virtual ~CCamera();

	static CCamera& bGetInst();

	void		bSetCam		( RwCamera* pRwCam );//	{ m_pRwCam = pRwCam; };
	RwCamera*	bGetCam		( void )			;//	{ return m_pRwCam; };
	LPAXIS		bGetPtrAxis	( void )				{ return &m_cAxis; };
	RwMatrix*	bGetPtrMatB()				{ return &m_matBillboard; };
	RwMatrix*	bGetPtrMatBY()				{ return &m_matBillboardY; };
	RwMatrix*	bGetPtrMatCamLTM()			{ return &RwCameraGetFrame(m_pRwCam)->ltm; };

	const RwV3d*	bGetPtrV3dAtPos()const	{ return &m_v3AtPos; };
	const RwV3d*	bGetPtrV3dEyePos()const	{ return &m_v3EyePos; };
	const RwV3d*	bGetPtrV3dEyeSubAt()const	{ return &m_v3EyeSubAt; };

	const RwV3d*	bGetPtrX()const { return &m_v3X; };
	const RwV3d*	bGetPtrY()const { return &m_v3Y; };
	const RwV3d*	bGetPtrZ()const { return &m_v3Z; };

	void		bSetLookatPt(const RwV3d& v3dLookat, BOOL bMoveEye=TRUE);

	//Rotate
	//바라보는 점을 중심으로 카메라의 위치를 회전
	void		bRotWY		( RwReal deg	);		//WY : world's y axis
	void		bRotCX		( RwReal deg	);		//CX : camera's x axis
	void		bRotCXLimit	( RwReal deg, RwReal fMax=0.f, RwReal fMin=-90.f );
	void		bRotCXLimitToZoom( RwReal deg, RwReal fMax=0.f, RwReal fMin=-90.f );
	void		bRestitution( void );
	//void		bRotAxis	( RwV3d* lpAxis, RwReal	deg );//임의축 회전.

	//카메라의 위치를 중심으로 카메라가 바라보는 점을 회전.
	void		bYaw		( RwReal deg );			//yaxis
	void		bPitch		( RwReal deg );			//xaxis
	void		bRoll		( RwReal deg );			//zaxis

	//Zoom
	void		bZoom		( RwReal ds );
	void		bZoomLimit	( RwReal ds , RwReal fSMin=4.f, RwReal fSMax=400.f );

	//Move
	void		bForward	( RwReal ds );
	void		bUpward		( RwReal ds );
	void		bSideward	( RwReal ds );

	void		bCameraWorkWithKey();
	void		bCameraWorkWithMouse(BOOL bLBtnRot, float dx, float dy);
};/**/

#define GRWCAM	CCamera::bGetInst()
#define G_RWCAM	CCamera::bGetInst()
