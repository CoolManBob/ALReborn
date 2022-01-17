// Cam.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"

#include "Camera.h"

#include "rwcore.h"
#include "ptrdata.h"

#include <memory.h>
#include <math.h>
#include <rtquat.h>

#include "AgcuEffUtil.h"//for KEYDOWN
#include "AgcdEffGlobal.h"
#include "AgcuBillBoard.h"

BOOL	bForcedZoom		= FALSE;
BOOL	bBackToOrgLen	= FALSE;
RwReal	fAccel			= -0.04f;//-8.f;
RwReal	fOrgLen			= 0.f;

CCamera& CCamera::bGetInst()
{
	static CCamera inst; 
	return inst;
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCamera::CCamera()
: m_pRwCam(NULL)
, m_bActiveWnd(FALSE)
{
	m_v3X.x	= 1.f, m_v3X.y = 0.f, m_v3X.z = 0.f;
	m_v3Y.x	= 0.f, m_v3Y.y = 1.f, m_v3Y.z = 0.f;
	m_v3Z.x	= 0.f, m_v3Z.y = 0.f, m_v3Z.z = 1.f;

	m_v3EyePos.x	=  0.f,
	m_v3EyePos.y	=  0.f,
	m_v3EyePos.z	= 10.f;

	m_v3AtPos.x		= 
	m_v3AtPos.y		= 
	m_v3AtPos.z		=  0.f;

	RwV3dSub( &m_v3Z, &m_v3AtPos, &m_v3EyePos );

	m_cAxis.bSetupSize( 0.25f, 0.25f, 0.0625f );
	m_cAxis.bSetupSemi( TRUE );

	memset( &m_matBillboard, 0, sizeof( m_matBillboard ) );

	m_matBillboardY			= m_matBillboard;
}
CCamera::~CCamera()
{
}
//void /*CCamera::/**/vUpdateMatirx( RwUInt32 flags );
void CCamera::bSetCam		( RwCamera* pRwCam )
{
	static RwBool	b1st	= TRUE;
	m_pRwCam = pRwCam; 
	if( b1st ){
		b1st	= !b1st;

		m_v3X.x	= -1.f, m_v3X.y = 0.f, m_v3X.z =  0.f;
		m_v3Y.x	=  0.f, m_v3Y.y = 1.f, m_v3Y.z =  0.f;
		m_v3Z.x	=  0.f, m_v3Z.y = 0.f, m_v3Z.z = -1.f;

		m_v3EyePos.x	= 0.f,
		m_v3EyePos.y	= 0.f,
		m_v3EyePos.z	= 10.f;

		m_v3AtPos.x		= 
		m_v3AtPos.y		= 
		m_v3AtPos.z		= 0.f;

		RwV3dSub( &m_v3EyeSubAt, &m_v3EyePos, &m_v3AtPos  );

		vUpdateMatirx(3);
	}
};

RwCamera* CCamera::bGetCam		( void )
{
	return m_pRwCam;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::vUpdateMatirx( RwUInt32 flags )
{
	RwFrame*	pFrm	= RwCameraGetFrame(m_pRwCam);
	RwMatrix*	pMat	= &pFrm->modelling;//&pFrm->ltm;//&m_pRwCam->viewMatrix;//

	RwV3dNormalize( &m_v3Y, &m_v3Y );
	RwV3dSub( &m_v3Z, &m_v3AtPos, &m_v3EyePos );//RwV3dSub( &m_v3Z, &m_v3EyePos, &m_v3AtPos );//
	RwV3dNormalize( &m_v3Z, &m_v3Z );
	RwV3dCrossProduct( &m_v3X, &m_v3Y, &m_v3Z );

	pMat->right	= m_v3X;
	pMat->up	= m_v3Y;
	pMat->at	= m_v3Z;	
	pMat->pos	= m_v3EyePos;

	pMat->flags	= flags;
	/*
	//RwCameraGetViewMatrix(m_pRwCam)->flags	= 3;
	pMat->flags	= 3;
	pMat->pad1	= 
	pMat->pad2	= 
	pMat->pad3	= 0;/**/

	RwFrameUpdateObjects(pFrm);

	m_cAxis.bSetPos(m_v3AtPos);

	vSetBMatrix();
	vSetBWYMatrix();
};


//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::vSetBMatrix()
{
	m_matBillboard.right.x	= -m_v3X.x;
	m_matBillboard.right.y	= -m_v3X.y;
	m_matBillboard.right.z	= -m_v3X.z;
	
	m_matBillboard.at.x		= -m_v3Z.x;
	m_matBillboard.at.y		= -m_v3Z.y;
	m_matBillboard.at.z		= -m_v3Z.z;

	m_matBillboard.up		= m_v3Y;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::vSetBWYMatrix()
{	
	m_matBillboardY.right.x	= -m_v3X.x;
	m_matBillboardY.right.y	= -m_v3X.y;
	m_matBillboardY.right.z	= -m_v3X.z;

	m_matBillboardY.up.x	= 0.f;
	m_matBillboardY.up.y	= 1.f;
	m_matBillboardY.up.z	= 0.f;

	RwV3dCrossProduct( &m_matBillboardY.at
					 , &m_matBillboardY.right
					 , &m_matBillboardY.up );
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bSetLookatPt(const RwV3d& v3dLookat, BOOL bMoveEye)
{
	m_v3AtPos	= v3dLookat;

	if( bMoveEye )
	{
		RwV3dAdd( &m_v3EyePos, &m_v3AtPos, &m_v3EyeSubAt ); 
	}
	else
	{
		RwV3dSub(&m_v3EyeSubAt, &m_v3AtPos, &m_v3EyePos );
		RwV3dCrossProduct(&m_v3X, PTRAXISWY, &m_v3EyeSubAt);
		RwV3dCrossProduct(&m_v3Y, &m_v3EyeSubAt, &m_v3X);
		RwV3dNegate(&m_v3EyeSubAt, &m_v3EyeSubAt);
		//RwV3dAdd( &m_v3EyePos, &m_v3AtPos, &m_v3EyeSubAt );
	}

	vUpdateMatirx(3);
};

//바라보는 점을 중심으로 카메라의 위치를 회전
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bRotWY( RwReal deg	)//WY : world's y axis
{
	RwMatrix	mat;
	RwV3d		v3WorldY	= { 0.f, 1.f, 0.f };

	RwMatrixRotate(&mat, &v3WorldY, deg, rwCOMBINEREPLACE);

	RwV3dTransformPoint( &m_v3Y, &m_v3Y, &mat );
	RwV3dTransformPoint( &m_v3EyeSubAt, &m_v3EyeSubAt, &mat );
	RwV3dAdd( &m_v3EyePos, &m_v3AtPos, &m_v3EyeSubAt );

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bRotCX( RwReal deg	)//CX : camera's y axis
{
	RwMatrix	mat;

	RwMatrixRotate(&mat, &m_v3X, deg, rwCOMBINEREPLACE);

	RwV3dTransformPoint( &m_v3Y, &m_v3Y, &mat );
	RwV3dTransformPoint( &m_v3EyeSubAt, &m_v3EyeSubAt, &mat );
	RwV3dAdd( &m_v3EyePos, &m_v3AtPos, &m_v3EyeSubAt ); 

	vUpdateMatirx(3);
};


//////////////////////////////////////////////////////////////////////
// bYaw
// y-axis : up vector
//////////////////////////////////////////////////////////////////////
void CCamera::bYaw( RwReal deg )
{
	RwMatrix	mat;

	RwMatrixRotate(&mat, &m_v3Y, deg, rwCOMBINEREPLACE);

	RwV3dTransformPoint( &m_v3EyeSubAt, &m_v3EyeSubAt, &mat );
	RwV3dSub( &m_v3AtPos, &m_v3EyePos, &m_v3EyeSubAt );

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// bPitch
// x-axis : side vector or cross vector
//////////////////////////////////////////////////////////////////////
void CCamera::bPitch( RwReal deg )
{
	RwMatrix	mat;

	RwMatrixRotate(&mat, &m_v3X, deg, rwCOMBINEREPLACE);

	RwV3dTransformPoint( &m_v3EyeSubAt, &m_v3EyeSubAt, &mat );
	RwV3dSub( &m_v3AtPos, &m_v3EyePos, &m_v3EyeSubAt );

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// bRoll
// z-axis : look vector or view direction
//////////////////////////////////////////////////////////////////////
void CCamera::bRoll( RwReal deg )
{
	return;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bRotCXLimit( RwReal deg, RwReal fMax, RwReal fMin )
{
	//bRotCXLimitToZoom( deg, fMax, fMin );
	//return;

	RwMatrix	mat;
	
	RwReal fDeg		= 0.f;
	RwReal fAfter	= 0.f;
	RwReal fdot		= -m_v3Z.y;

		 if( fdot >  1.f )	fdot	=  1.f;
	else if( fdot < -1.f )	fdot	= -1.f;
	
	fDeg		= ( (RwReal)(acos( fdot )) * 180.f / 3.141592654f );

	if( m_v3Y.y > 0.f )	fDeg	= -fDeg;

	fAfter	= fDeg + deg;

		 if( deg == 0.f )			return;
	else if( deg > 0.f )
	{
			 if( fDeg	>= fMax )	return;
		else if( fAfter >  fMax )	deg	= fMax - fDeg;
	}
	else//deg < 0.f
	{
			 if( fDeg	<= fMin )	return;
		else if( fAfter	<  fMin )	deg = fMin - fDeg;
	}


	RwMatrixRotate(&mat, &m_v3X, deg, rwCOMBINEREPLACE);

	RwV3dTransformPoint( &m_v3Y, &m_v3Y, &mat );
	RwV3dTransformPoint( &m_v3EyeSubAt, &m_v3EyeSubAt, &mat );
	RwV3dAdd( &m_v3EyePos, &m_v3AtPos, &m_v3EyeSubAt ); 

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bRotCXLimitToZoom( RwReal deg, RwReal fMax, RwReal fMin )
{
	RwMatrix	mat;
	
	RwReal fDeg		= 0.f;
	RwReal fAfter	= 0.f;
	RwReal fdot		= -m_v3Z.y;

		 if( fdot >  1.f )	fdot	=  1.f;
	else if( fdot < -1.f )	fdot	= -1.f;
	
	fDeg		= ( (RwReal)(acos( fdot )) * 180.f / 3.141592654f );

	if( m_v3Y.y > 0.f )	fDeg	= -fDeg;

	fAfter	= fDeg + deg;

		 if( deg == 0.f )			return;
	else if( deg > 0.f )
	{
		if( bForcedZoom && !bBackToOrgLen )
		{
			bBackToOrgLen	= TRUE;
		}
			 if( fDeg	>= fMax )	return;
		else if( fAfter >  fMax )	deg	= fMax - fDeg;
	}
	else//deg < 0.f
	{
		if( fDeg <= fMin )
		{
			if( !bForcedZoom )
			{
				bForcedZoom	= TRUE;
				fOrgLen	= RwV3dLength(&m_v3EyeSubAt);
			}

			bZoom(-20.f*deg);

			return;
		}
		else if( fAfter	<  fMin )	deg = fMin - fDeg;
	}
/*
BOOL	bForcedZoom		= FALSE;
BOOL	bBackToOrgLen	= FALSE;
RwReal	fAccel			= 2.f;
RwReal	fOrgLen			= 0.f;*/

	RwMatrixRotate(&mat, &m_v3X, deg, rwCOMBINEREPLACE);

	RwV3dTransformPoint( &m_v3Y, &m_v3Y, &mat );
	RwV3dTransformPoint( &m_v3EyeSubAt, &m_v3EyeSubAt, &mat );
	RwV3dAdd( &m_v3EyePos, &m_v3AtPos, &m_v3EyeSubAt ); 

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bRestitution( void )
{
	if( !bBackToOrgLen )
		return;

	RwReal	fSqLen		= Eff2Ut_RwV3dSqLength( m_v3EyeSubAt );
	RwReal	fSqLenOrg	= fOrgLen*fOrgLen;
	
	if( fSqLen+1.5f >= fSqLenOrg )
	{
		bBackToOrgLen	= FALSE;
		bForcedZoom		= FALSE;
		fOrgLen = 0.f;
		return;
	}

	static int nphi = 1;
	if( KEYDOWN( '1' ) )
		nphi = 1;
	else if( KEYDOWN( '2' ) )
		nphi = 2;
	else if( KEYDOWN( '3' ) )
		nphi = 3;
	else if( KEYDOWN( '4' ) )
		nphi = 4;

	float cof = 0.f;
	switch( nphi )
	{
	case 1: // 2phi -> -90 ~ 270
		cof = ( 1.f + sinf( fSqLen*DEF_2PI/fSqLenOrg - DEF_PI*0.5f) ) * 0.5f;
		//TOWND("1: // 2phi -> -90 ~ 270");
		break;
	case 2: // 1phi -> 0 ~ 180
		cof = sinf( fSqLen*DEF_PI/fSqLenOrg);
		//TOWND("2: // 1phi -> 0 ~ 180");
		break;
	case 3: // 3/2 phi -> 0 ~ 270
		cof = ( 1.f + sinf( fSqLen*DEF_PI*3.f*0.5f/fSqLenOrg) ) * 0.5f;
		//TOWND("3: // 3/2 phi -> 0 ~ 270");
		break;
	case 4: // 1phi -> 90 ~ 270
		cof = ( 1.f + sinf( fSqLen*DEF_PI/fSqLenOrg + DEF_PI*0.5f) ) * 0.5f;
		//TOWND("4: // 1phi -> 90 ~ 270");
		break;
	}

	bZoomLimit( (
		fAccel*
		cof *
		AgcdEffGlobal::bGetInst().bGetDiffTimeMS()-0.15f) * 
		AgcdEffGlobal::bGetInst().bGetDiffTimeMS()
		, 4.f, fOrgLen );
};
//void CCamera::bRotAxis	( RwV3d* lpAxis, RwReal	deg ){};//임의축 회전.

//Zoom
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bZoom( RwReal ds )
{
	RwV3d	v3zoom	= { m_v3Z.x, m_v3Z.y, m_v3Z.z };
	RwV3dScale( &v3zoom, &v3zoom, ds );

	if( ds == 0.f )
		return;
	if( ds > 0.f ){
		if( RwV3dLength( &m_v3EyeSubAt ) < RwV3dLength( &v3zoom ) )	
			return;
	}

	RwV3dAdd( &m_v3EyePos, &m_v3EyePos, &v3zoom );
	RwV3dSub( &m_v3EyeSubAt, &m_v3EyePos, &m_v3AtPos  );

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bZoomLimit( RwReal ds , RwReal fMin, RwReal fMax )
{
	
	RwReal	fLen	= RwV3dLength( &m_v3EyeSubAt );
	RwV3d	v3zoom	= { m_v3Z.x, m_v3Z.y, m_v3Z.z };
	RwV3dScale( &v3zoom, &v3zoom, ds );

	if( ds == 0.f )
		return;
	if( ds > 0.f ){
		if( fLen <= RwV3dLength( &v3zoom ) )	
			return;
		if( fLen <= fMin )
			return;
	}else if( fLen >= fMax )
		return;

	RwV3dAdd( &m_v3EyePos, &m_v3EyePos, &v3zoom );
	RwV3dSub( &m_v3EyeSubAt, &m_v3EyePos, &m_v3AtPos  );

	vUpdateMatirx(3);
};

//Move
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bForward( RwReal ds )
{
	DWORD	dwZ		= Eff2Ut_FTODW( m_v3Z.y );
	RwV3d	v3zoom	=
		( abs(m_v3Z.y) < abs(m_v3Y.y)  ) ? m_v3Z : m_v3Y;

	v3zoom.y	= 0.f;
	RwV3dNormalize( &v3zoom, &v3zoom );
	RwV3dScale( &v3zoom, &v3zoom, ds );

	RwV3dAdd( &m_v3EyePos	, &m_v3EyePos	, &v3zoom );	
	RwV3dAdd( &m_v3AtPos	, &m_v3AtPos	, &v3zoom );

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bUpward( RwReal ds )
{
	RwV3d	v3zoom	= { 0.f, 1.f, 0.f };
	RwV3dScale( &v3zoom, &v3zoom, ds );

	RwV3dAdd( &m_v3EyePos	, &m_v3EyePos	, &v3zoom );	
	RwV3dAdd( &m_v3AtPos	, &m_v3AtPos	, &v3zoom );

	vUpdateMatirx(3);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bSideward( RwReal ds )
{
	RwV3d	v3zoom	= { m_v3X.x, m_v3X.y, m_v3X.z };
	RwV3dScale( &v3zoom, &v3zoom, ds );

	RwV3dAdd( &m_v3EyePos	, &m_v3EyePos	, &v3zoom );	
	RwV3dAdd( &m_v3AtPos	, &m_v3AtPos	, &v3zoom );

	vUpdateMatirx(3);
};

const FLOAT	ZOOMLIMIT	= 40000.f;
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bCameraWorkWithKey()
{
	if( !m_bActiveWnd )
		return;
	//camera work with keyboard
	static RwReal fMS	= 1.f;
	static RwReal fRS	= 1.f;
	static RwReal fZS	= 10.f;

	static BOOL	bR	= FALSE;
	static BOOL	bM	= FALSE;
	static BOOL	bZ	= FALSE;
	static RwReal*	pVar	= &fMS;
	if( KEYDOWN_ONECE( 'R', bR ) ){
		pVar	= &fRS;
	}else if( KEYDOWN_ONECE( 'M', bR ) ){
		pVar	= &fMS;
	}else if( KEYDOWN_ONECE( 'Z', bR ) ){
		pVar	= &fZS;
	}

	RwReal	fDelta	= 0.1f;
	
	RwReal fMoveSpeed	= fMS;
	RwReal fRotSpeed	= fRS;
	RwReal fZoomSpeed	= fZS;

	if( KEYDOWN( VK_MENU ) ){
		RwReal	cof	= 10.f;
		fMoveSpeed	*= cof;
		fRotSpeed	*= cof;
		fZoomSpeed	*= cof;
		fDelta		*= cof;
	}else if( KEYDOWN( VK_SHIFT ) ){
		RwReal	cof	= 0.1f;
		fMoveSpeed	*= cof;
		fRotSpeed	*= cof;
		fZoomSpeed	*= cof;
		fDelta		*= cof;
	}

	if( KEYDOWN( VK_ADD ) )
		*pVar	+= fDelta;
	if( KEYDOWN( VK_SUBTRACT ) )
		*pVar	-= fDelta;

	if( KEYDOWN( 'W' ) )
		bForward( fMoveSpeed );
	if( KEYDOWN( 'S' ) )
		bForward( -fMoveSpeed );
	if( KEYDOWN( 'A' ) )
		bSideward( fMoveSpeed );
	if( KEYDOWN( 'D' ) )
		bSideward( -fMoveSpeed );

	if( KEYDOWN( VK_HOME ) )
		bRotCX( fRotSpeed );
	if( KEYDOWN( VK_END ) )
		bRotCX( -fRotSpeed );
	if( KEYDOWN( VK_DELETE ) )
		bRotWY( fRotSpeed );
	if( KEYDOWN( VK_NEXT ) )
		bRotWY( -fRotSpeed );

	if( KEYDOWN( VK_INSERT ) )
		bZoomLimit( fZoomSpeed, 1.f, ZOOMLIMIT );
	if( KEYDOWN( VK_PRIOR ) )
		bZoomLimit( -fZoomSpeed, 1.f, ZOOMLIMIT );
}


//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void CCamera::bCameraWorkWithMouse(BOOL bLBtnRot, float dx, float dy)
{
	if( !m_bActiveWnd )
		return;

	float	offsetScl	= 1.f;
	if( KEYDOWN(VK_MBUTTON) ){

		if( KEYDOWN(VK_MENU) ){
			
			if( KEYDOWN( VK_CONTROL ) ){//zoom
				if( KEYDOWN( VK_SHIFT ) )	offsetScl	= -1.15f;
				else						offsetScl	= -2.5f;

				bZoomLimit( dy * offsetScl, 1.f, ZOOMLIMIT );
			}else{						//rot
				if( KEYDOWN( VK_SHIFT ) )	offsetScl	= 0.1f;
				else						offsetScl	= 0.5f;

				bRotCX( dy * offsetScl );
				bRotWY( dx * offsetScl );
			}
			
		}else{							//move
			if( KEYDOWN( VK_SHIFT ) )	offsetScl	= 0.615f;
			else						offsetScl	= 2.15f;
			
			bForward( dy * offsetScl );
			bSideward( dx * offsetScl );
		}
	}
	else if( KEYDOWN(VK_RBUTTON) )
	{//zoom
		if( KEYDOWN( VK_SHIFT ) )	offsetScl	= -1.15f;
		else						offsetScl	= -2.5f;

		bZoomLimit( dy * offsetScl, 1.f, ZOOMLIMIT );
	}
	else if( bLBtnRot && KEYDOWN(VK_LBUTTON) )
	{//rot
		if( KEYDOWN( VK_SHIFT ) )	offsetScl	= 0.1f;
		else						offsetScl	= 0.5f;

		bRotCX( dy * offsetScl );
		bRotWY( dx * offsetScl );
	}


};
