// PickUser.cpp: implementation of the CPickUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "RwWzrdTest1.h"
#include "PickUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#pragma warning(disable: 4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPickUser::CPickUser()
: m_fDistance(0.f)
{
	m_uv3dRay.x		= 
	m_uv3dRay.y		= 
	m_uv3dRay.z		= 0.f;
	
	m_v3dCamPos.x	= 
	m_v3dCamPos.y	= 
	m_v3dCamPos.z	= 0.f;
	
	m_v3dPicked.x	= 
	m_v3dPicked.y	= 
	m_v3dPicked.z	= 0.f;

	m_ptMouse.x		= 
	m_ptMouse.y		= 0;
}

CPickUser::~CPickUser()
{
}

CPickUser& CPickUser::GetInst(void)
{
	static CPickUser inst;
	return inst;
};

VOID CPickUser::Make_Ray( RwV3d* lpuv3dRay
						 , RwV3d* lpv3dCamPos
						 , RwCamera* lpRwCam
						 , const POINT& ptMouse)
{
	if( NULL == lpuv3dRay )
		lpuv3dRay	= &m_uv3dRay;
	if( NULL == lpv3dCamPos )
		lpv3dCamPos	= &m_v3dCamPos;

	if( !lpRwCam )	return;//err
	RwFrame*	pFrm	= RwCameraGetFrame( lpRwCam );
	if( !pFrm )		return;//err
	*lpv3dCamPos	= pFrm->modelling.pos;

	//const RwV2d*	pV3dViewWindow	= RwCameraGetViewWindow( lpRwCam );
	RwRaster*		pCamRast		= RwCameraGetRaster( lpRwCam );	
	if( !pCamRast )	return;//err

	RwInt16			xOffset			= 0, 
					yOffset			= 0;
	RwRasterGetOffset(pCamRast, &xOffset, &yOffset);

	RwInt32			width			= RwRasterGetWidth(pCamRast), 
					height			= RwRasterGetHeight(pCamRast);

	//const RwMatrix*	pMatView		= RwCameraGetViewMatrix(lpRwCam);

	// Get the pick ray from the mouse position
    RwV3d vTemp	= {	( (RwReal)(ptMouse.x+xOffset) / (width)  ),// / pV3dViewWindow->y ),
					( (RwReal)(ptMouse.y+yOffset) / (height)  ),// / pV3dViewWindow->x ),
					(1.f)											};

    // Transform the screen space pick ray into 3D space
	RwLine	 tempLine;
	RwLine * line	= &tempLine;
	
    RwV3d               topVert, botVert;
    const RwV3d        *frustVerts	= lpRwCam->frustumCorners;

    /* Figure out top and bottom on the front plane */
    RwV3dSub(&topVert, &frustVerts[2], &frustVerts[3]);
    RwV3dScale(&topVert, &topVert, vTemp.x);
    RwV3dAdd(&topVert, &topVert, &frustVerts[3]);

    RwV3dSub(&botVert, &frustVerts[1], &frustVerts[0]);
    RwV3dScale(&botVert, &botVert, vTemp.x);
    RwV3dAdd(&botVert, &botVert, &frustVerts[0]);

    /* Now interpolate between top and bottom */
    RwV3dSub(&line->start, &topVert, &botVert);
    RwV3dScale(&line->start, &line->start, vTemp.y);
    RwV3dAdd(&line->start, &line->start, &botVert);

    /* Figure out top and bottom on the back plane */
    RwV3dSub(&topVert, &frustVerts[6], &frustVerts[7]);
    RwV3dScale(&topVert, &topVert, vTemp.x);
    RwV3dAdd(&topVert, &topVert, &frustVerts[7]);

    RwV3dSub(&botVert, &frustVerts[5], &frustVerts[4]);
    RwV3dScale(&botVert, &botVert, vTemp.x);
    RwV3dAdd(&botVert, &botVert, &frustVerts[4]);

    /* Now interpolate between top and bottom */
    RwV3dSub(&line->end, &topVert, &botVert);
    RwV3dScale(&line->end, &line->end, vTemp.y);
    RwV3dAdd(&line->end, &line->end, &botVert);

	RwV3dSub( lpuv3dRay, &line->end, &line->start );
	RwV3dNormalize( lpuv3dRay, lpuv3dRay );

	/*
	lpuv3dRay->x  = vTemp.x*pMatView->right.x + vTemp.y*pMatView->up.x + vTemp.z*pMatView->at.x;
	lpuv3dRay->y  = vTemp.x*pMatView->right.y + vTemp.y*pMatView->up.y + vTemp.z*pMatView->at.y;
	lpuv3dRay->z  = vTemp.x*pMatView->right.z + vTemp.y*pMatView->up.z + vTemp.z*pMatView->at.z;
	
	RwV3dNormalize( lpuv3dRay, lpuv3dRay );/**/
}



///////////////////////////////////////////////////////////////////////////////
// IntersectTriangle
// sdk8 d3d_pick 예제 함수.
///////////////////////////////////////////////////////////////////////////////
RwBool CPickUser::IntersectTriangle(const RwV3d *pvRay,
									const RwV3d *pvCamPos,
									const RwV3d *pv0, 
									const RwV3d *pv1, 
									const RwV3d *pv2, 
									RwReal* t, RwReal* u, RwReal* v,
									RwBool bCalcPickedPt)
{
	if( NULL == pvRay ) 
		pvRay	= &m_uv3dRay;

	// Find vectors for two edges sharing vert0
	RwV3d	edge1,
			edge2;
	RwV3dSub(&edge1, pv1, pv0);
	RwV3dSub(&edge2, pv2, pv0);

    // Begin calculating determinant - also used to calculate U parameter
    RwV3d pvec;
    RwV3dCrossProduct( &pvec, pvRay, &edge2 );//RwV3dCrossProduct( &pvec, &edge2, pvRay );//

    // If determinant is near zero, ray lies in plane of triangle
    RwReal det = RwV3dDotProduct( &edge1, &pvec );

    RwV3d tvec;
    if( det > 0 )  
		RwV3dSub(&tvec, pvCamPos, pv0);
    else{
		RwV3dSub(&tvec, pv0, pvCamPos);
        det = -det;
    }
    if( det < 0.0001f )		
		return FALSE;

    // Calculate U parameter and test bounds
    *u = RwV3dDotProduct ( &tvec, &pvec );
    if( *u < 0.0f || *u > det )	
		return FALSE;

    // Prepare to test V parameter
    RwV3d qvec;
    RwV3dCrossProduct( &qvec, &tvec, &edge1 );//RwV3dCrossProduct( &qvec, &edge1, &tvec );//

    // Calculate V parameter and test bounds
    *v = RwV3dDotProduct( pvRay, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    RwReal fInvDet = 1.0f / det;
    *u *= fInvDet;
    *v *= fInvDet;

	//if(t){// 떨어진 거리를 알고 싶지 않을때..
		*t = RwV3dDotProduct( &edge2, &qvec );
		*t *= fInvDet;
	//}

	if( bCalcPickedPt ){
		bCalcPickedPoint( this->m_v3dPicked
						, *pv0, *pv1, *pv2
						, *u, *v );

		m_cArrow.bSetStartNTarget( *pvCamPos, m_v3dPicked );
	}

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PickPoint
// 삼각형과 Ray의 교차점.
///////////////////////////////////////////////////////////////////////////////
VOID CPickUser::bCalcPickedPoint(	   RwV3d& v3dOut
								,const RwV3d& v0
								,const RwV3d& v1
								,const RwV3d& v2
								,RwReal u, RwReal v)
{
	RwV3d	edge1,	edge2;

	RwV3dSub	( &edge1, &v1	, &v0	);
	RwV3dScale	( &edge1, &edge1, u		);
	RwV3dSub	( &edge2, &v2	, &v0	);
	RwV3dScale	( &edge2, &edge2, v		);

	RwV3dAdd( &v3dOut, &v0	  , &edge1 );
	RwV3dAdd( &v3dOut, &v3dOut, &edge2 );

	return;
};



///////////////////////////////////////////////////////////////////////////////
// bSphereCheck
///////////////////////////////////////////////////////////////////////////////
RwBool CPickUser::bSphereCheck(const RwV3d *puvRay,		//uv	: unit vector
							  const RwV3d *pvCamPos,
							  const RwV3d *pvTargetPos,
							  const RwReal fSBR )		//SBR	: Squre BoundingSphere Radius
{
	RwV3d		vTemp;
	RwV3d		vCamToTarget;

	RwV3dSub( &vCamToTarget, pvTargetPos, pvCamPos );
	RwV3dCrossProduct( &vTemp, puvRay, &vCamToTarget );

	return ( fSBR > ( vTemp.x*vTemp.x + vTemp.y*vTemp.y + vTemp.z*vTemp.z ) );
}

///////////////////////////////////////////////////////////////////////////////
// bRwSphereCheck
///////////////////////////////////////////////////////////////////////////////
RwBool CPickUser::bRwSphereCheck(const RwV3d* puvRay	
		, const RwV3d*		pvCamPos
		, const RwSphere&	bsphere	
		, const RwMatrix*	pMat	)
{
	RwV3d		vTarget  = bsphere.center;

	if( pMat ){
		RwV3dAdd( &vTarget, &vTarget, &pMat->pos );
	}

	return
	bSphereCheck( puvRay, pvCamPos, &vTarget, bsphere.radius*bsphere.radius );
}

///////////////////////////////////////////////////////////////////////////////
// bRwBBoxCheck
///////////////////////////////////////////////////////////////////////////////
RwBool CPickUser::bRwBBoxCheck(const RwV3d* puvRay	
		, const RwV3d*		pvCamPos
		, const RwBBox&		bbox
		, const RwMatrix*	pMat )
{
	RwV3d	boxpoint[8] ={
		{bbox.inf.x, bbox.sup.y, bbox.inf.z},//0
		{bbox.sup.x, bbox.sup.y, bbox.inf.z},//1
		{bbox.inf.x, bbox.sup.y, bbox.sup.z},//2
		{bbox.sup.x, bbox.sup.y, bbox.sup.z},//3
		{bbox.inf.x, bbox.inf.y, bbox.inf.z},//4
		{bbox.sup.x, bbox.inf.y, bbox.inf.z},//5
		{bbox.inf.x, bbox.inf.y, bbox.sup.z},//6
		{bbox.sup.x, bbox.inf.y, bbox.sup.z},//7
	};

	if( pMat ){
		RwV3dTransformPoints( boxpoint
			, boxpoint
			, 8LU
			, pMat
			);
	}

	RwBool	br	= FALSE;
	RwReal	t	= 0.f,
			u	= 0.f,
			v	= 0.f;
	//left
	//0,2,4
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[0], &boxpoint[2], &boxpoint[4], &t, &u, &v ) )	return TRUE;
	//2,6,4
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[2], &boxpoint[6], &boxpoint[4], &t, &u, &v ) )	return TRUE;
	//right
	//3,1,7
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[3], &boxpoint[1], &boxpoint[7], &t, &u, &v ) )	return TRUE;
	//1,5,7
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[1], &boxpoint[5], &boxpoint[7], &t, &u, &v ) )	return TRUE;
	//top
	//0,1,2
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[0], &boxpoint[1], &boxpoint[2], &t, &u, &v ) )	return TRUE;
	//1,3,2
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[1], &boxpoint[3], &boxpoint[2], &t, &u, &v ) )	return TRUE;
	//bottom
	//6,7,4
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[6], &boxpoint[7], &boxpoint[4], &t, &u, &v ) )	return TRUE;
	//7,5,4
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[7], &boxpoint[5], &boxpoint[4], &t, &u, &v ) )	return TRUE;
	//front
	//2,3,6
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[2], &boxpoint[3], &boxpoint[6], &t, &u, &v ) )	return TRUE;
	//3,7,6
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[3], &boxpoint[7], &boxpoint[6], &t, &u, &v ) )	return TRUE;	
	//back
	//1,0,5
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[1], &boxpoint[0], &boxpoint[5], &t, &u, &v ) )	return TRUE;
	//0,4,5
	if( IntersectTriangle( puvRay, pvCamPos, &boxpoint[0], &boxpoint[4], &boxpoint[5], &t, &u, &v ) )	return TRUE;

	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// bGetRayCamPosWithMat
///////////////////////////////////////////////////////////////////////////////
VOID CPickUser::bGetRayCamPosWithMat(	   RwV3d*		puvRayOut
								   ,	   RwV3d*		pvCamPosOut
								   , const RwMatrix*	pMat 
								   , const RwV3d*		puvRayIn
								   , const RwV3d*		pvCamPosIn)
{
	RwMatrix	invsMat;

	RwMatrixInvert( &invsMat, pMat );
	RwV3dTransformPoint( pvCamPosOut, pvCamPosIn ? pvCamPosIn : &m_v3dCamPos, &invsMat );
	
	invsMat.pos.x	= 
	invsMat.pos.y	= 
	invsMat.pos.z	= 0.f;
	RwV3dTransformPoint( puvRayOut, puvRayIn ? puvRayIn : &m_uv3dRay, &invsMat );
};