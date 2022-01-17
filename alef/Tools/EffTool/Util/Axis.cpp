// Axis.cpp: implementation of the CAxis class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "Axis.h"

const RwUInt32		NUMOFANGLE	= 13;
const RwReal		AXISLEN		= 1.f;
const RwReal		CONEHEIGHT	= 1.f;
const RwReal		CONERADIUS	= .125f;
const RwUInt32		NCOL		= 0xFF808080;//GRAY
const RwUInt32		NCOLX		= 0xFFF00000;//RED
const RwUInt32		NCOLY		= 0xFF00F000;//GREEN
const RwUInt32		NCOLZ		= 0xFF0000F0;//BLUE

const RwUInt32		PCOLX		= 0xFF800000;//RED
const RwUInt32		PCOLY		= 0xFF008000;//GREEN
const RwUInt32		PCOLZ		= 0xFF000080;//BLUE
const RwReal		PHI			= 3.1415926536f;
const RwReal		TWOPHI		= 6.2831853072f;

const CAxis::tagstAxisWorld& CAxis::tagstAxisWorld::bGetInst()
{ 
	static tagstAxisWorld inst; 
	return inst; 
};

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
CAxis::CAxis()
: m_bSemi( FALSE )
, m_dwTransformFlags(rwIM3D_ALLOPAQUE)
, m_bShowLine( TRUE )
, m_bShowCone( TRUE )
{
	m_stAxisHeader.m_dwNumOfAngle	= NUMOFANGLE;
	m_stAxisHeader.m_fAxisLen		= AXISLEN;
	m_stAxisHeader.m_fConeHeight	= CONEHEIGHT;
	m_stAxisHeader.m_fConeRadius	= CONERADIUS;
	m_stAxisHeader.m_dwNColX		= NCOLX;
	m_stAxisHeader.m_dwNColY		= NCOLY;
	m_stAxisHeader.m_dwNColZ		= NCOLZ;
	m_stAxisHeader.m_dwPColX		= PCOLX;
	m_stAxisHeader.m_dwPColY		= PCOLY;
	m_stAxisHeader.m_dwPColZ		= PCOLZ;
	
	memset(&m_mat, 0, sizeof(m_mat));
	m_mat.right.x	= 
	m_mat.up.y		= 
	m_mat.at.z		= 1.f;

	// 2 : 라인에 필요한 점.
	// 3 : 삼각형당 필요한 점.
	// 2 : 콘 + 콘바닥
	// 3 : 축이 3개(x, y z)
	m_stlvecVtx.resize( ( 2 + m_stAxisHeader.m_dwNumOfAngle * 3 * 2 ) * 3 );

	vUpdata();
};

CAxis::CAxis(const CAxis& cpy)
: m_bSemi(cpy.m_bSemi)
, m_dwTransformFlags(cpy.m_dwTransformFlags)
{
	m_stAxisHeader	= cpy.m_stAxisHeader;
	m_mat			= cpy.m_mat;
	m_stlvecVtx		= cpy.m_stlvecVtx;
};
CAxis& CAxis::operator = (const CAxis& cpy)
{
	if( &cpy == this )
		return *this;

	m_bSemi				= cpy.m_bSemi;
	m_dwTransformFlags	= cpy.m_dwTransformFlags;
	m_stAxisHeader		= cpy.m_stAxisHeader;
	m_mat				= cpy.m_mat;
	m_stlvecVtx			= cpy.m_stlvecVtx;

	return *this;
};

CAxis::~CAxis()
{
	vClear();
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::vClear()
{
	m_stlvecVtx.clear();
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::vUpdata()
{
	vUpdataX();
	vUpdataY();
	vUpdataZ();
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::vUpdataX()
{
	LPRWIM3DVTX	pLineX	= vGetPtrLineX();
	LPRWIM3DVTX	pConeX	= vGetPtrConeX();
	LPRWIM3DVTX	pConeBX	= vGetPtrConeBottomX();

	RwV3d	v1,v2,v3;

	RwReal	fAngleStep	= TWOPHI / (RwReal)m_stAxisHeader.m_dwNumOfAngle;
	RwReal	fAngleCurr	= 0.f;
	RwReal	fSin1		= 0.f;
	RwReal	fSin2		= 0.f;
	RwReal	fCos1		= 0.f;
	RwReal	fCos2		= 0.f;

	//line
	v1	= CAxis::tagstAxisWorld::bGetInst().bGetX();
	v1.x	*= -m_stAxisHeader.m_fAxisLen;
	v2	= CAxis::tagstAxisWorld::bGetInst().bGetX();
	v2.x	*= m_stAxisHeader.m_fAxisLen;
	//line
	//RwV3dScale( &v2, &CAxis::tagstAxisWorld::bGetInst().bGetX(), m_stAxisHeader.m_fAxisLen );
	//RwV3dScale( &v1, &v2, -1.f );

	pLineX->Update( v1, m_stAxisHeader.m_dwNColX );
	(pLineX+1)->Update( v2, m_stAxisHeader.m_dwPColX );

	//cone & cone bottom
	v1.x = v1.y = v1.z = 0.f;
	v2.x = v2.y = v2.z = 0.f;
	v3.x = v3.y = v3.z = 0.f;
	
	v1.x	= 
	v2.x	= 
	v3.x	= m_stAxisHeader.m_fAxisLen;

	for( int i = 0; i<(int)m_stAxisHeader.m_dwNumOfAngle; ++i ){

		fSin1	= (RwReal)sin( fAngleCurr );
		fSin2	= (RwReal)sin( fAngleCurr + fAngleStep );
		fCos1	= (RwReal)cos( fAngleCurr );
		fCos2	= (RwReal)cos( fAngleCurr + fAngleStep );

		//cone
		v1.x	= m_stAxisHeader.m_fAxisLen + m_stAxisHeader.m_fConeHeight;

		v2.y	= m_stAxisHeader.m_fConeRadius * fCos1;
		v2.z	= m_stAxisHeader.m_fConeRadius * fSin1;		
		v3.y	= m_stAxisHeader.m_fConeRadius * fCos2;
		v3.z	= m_stAxisHeader.m_fConeRadius * fSin2;

		(pConeX+0)->Update( v1, m_stAxisHeader.m_dwPColX );
		(pConeX+1)->Update( v2, m_stAxisHeader.m_dwNColX );
		(pConeX+2)->Update( v3, m_stAxisHeader.m_dwNColX );

		//cone bottom
		v1.x	= m_stAxisHeader.m_fAxisLen;

		(pConeBX+0)->Update( v1, m_stAxisHeader.m_dwPColX );
		(pConeBX+1)->Update( v3, m_stAxisHeader.m_dwNColX );
		(pConeBX+2)->Update( v2, m_stAxisHeader.m_dwNColX );

		fAngleCurr	+= fAngleStep;
		pConeX		+= 3;
		pConeBX		+= 3;
	}
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::vUpdataY()
{
	LPRWIM3DVTX	pLineY	= vGetPtrLineY();
	LPRWIM3DVTX	pConeY	= vGetPtrConeY();
	LPRWIM3DVTX	pConeBY	= vGetPtrConeBottomY();

	RwV3d	v1,v2,v3;

	RwReal	fAngleStep	= TWOPHI / (RwReal)m_stAxisHeader.m_dwNumOfAngle;
	RwReal	fAngleCurr	= 0.f;
	RwReal	fSin1		= 0.f;
	RwReal	fSin2		= 0.f;
	RwReal	fCos1		= 0.f;
	RwReal	fCos2		= 0.f;

	//line
	v1	= CAxis::tagstAxisWorld::bGetInst().bGetY();
	v1.y	*= -m_stAxisHeader.m_fAxisLen;
	v2	= CAxis::tagstAxisWorld::bGetInst().bGetY();
	v2.y	*= m_stAxisHeader.m_fAxisLen;
	//line
	//RwV3dScale( &v2, &CAxis::tagstAxisWorld::bGetInst().bGetY(), m_stAxisHeader.m_fAxisLen );
	//RwV3dScale( &v1, &v2, -1.f );

	pLineY->Update( v1, m_stAxisHeader.m_dwNColY );
	(pLineY+1)->Update( v2, m_stAxisHeader.m_dwPColY );

	//cone & cone bottom
	v1.x = v1.y = v1.z = 0.f;
	v2.x = v2.y = v2.z = 0.f;
	v3.x = v3.y = v3.z = 0.f;
	
	v1.y	= 
	v2.y	= 
	v3.y	= m_stAxisHeader.m_fAxisLen;

	for( int i = 0; i<(int)m_stAxisHeader.m_dwNumOfAngle; ++i ){

		fSin1	= (RwReal)sin( fAngleCurr );
		fSin2	= (RwReal)sin( fAngleCurr + fAngleStep );
		fCos1	= (RwReal)cos( fAngleCurr );
		fCos2	= (RwReal)cos( fAngleCurr + fAngleStep );

		//cone
		v1.y	= m_stAxisHeader.m_fAxisLen + m_stAxisHeader.m_fConeHeight;

		v2.z	= m_stAxisHeader.m_fConeRadius * fCos1;
		v2.x	= m_stAxisHeader.m_fConeRadius * fSin1;		
		v3.z	= m_stAxisHeader.m_fConeRadius * fCos2;
		v3.x	= m_stAxisHeader.m_fConeRadius * fSin2;

		(pConeY+0)->Update( v1, m_stAxisHeader.m_dwPColY );
		(pConeY+1)->Update( v2, m_stAxisHeader.m_dwNColY );
		(pConeY+2)->Update( v3, m_stAxisHeader.m_dwNColY );

		//cone bottom
		v1.y	= m_stAxisHeader.m_fAxisLen;

		(pConeBY+0)->Update( v1, m_stAxisHeader.m_dwPColY );
		(pConeBY+1)->Update( v3, m_stAxisHeader.m_dwNColY );
		(pConeBY+2)->Update( v2, m_stAxisHeader.m_dwNColY );

		fAngleCurr	+= fAngleStep;
		pConeY		+= 3;
		pConeBY		+= 3;
	}
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::vUpdataZ()
{
	LPRWIM3DVTX	pLineZ	= vGetPtrLineZ();
	LPRWIM3DVTX	pConeZ	= vGetPtrConeZ();
	LPRWIM3DVTX	pConeBZ	= vGetPtrConeBottomZ();

	RwV3d	v1,v2,v3;

	RwReal	fAngleStep	= TWOPHI / (RwReal)m_stAxisHeader.m_dwNumOfAngle;
	RwReal	fAngleCurr	= 0.f;
	RwReal	fSin1		= 0.f;
	RwReal	fSin2		= 0.f;
	RwReal	fCos1		= 0.f;
	RwReal	fCos2		= 0.f;

	//line
	v1	= CAxis::tagstAxisWorld::bGetInst().bGetZ();
	v1.z	*= -m_stAxisHeader.m_fAxisLen;
	v2	= CAxis::tagstAxisWorld::bGetInst().bGetZ();
	v2.z	*= m_stAxisHeader.m_fAxisLen;
	//line
	//RwV3dScale( &v2, &CAxis::tagstAxisWorld::bGetInst().bGetZ(), m_stAxisHeader.m_fAxisLen );
	//RwV3dScale( &v1, &v2, -1.f );

	pLineZ->Update( v1, m_stAxisHeader.m_dwNColZ );
	(pLineZ+1)->Update( v2, m_stAxisHeader.m_dwPColZ );

	//cone & cone bottom
	v1.x = v1.y = v1.z = 0.f;
	v2.x = v2.y = v2.z = 0.f;
	v3.x = v3.y = v3.z = 0.f;
	
	v1.z	= 
	v2.z	= 
	v3.z	= m_stAxisHeader.m_fAxisLen;

	for( int i = 0; i<(int)m_stAxisHeader.m_dwNumOfAngle; ++i ){

		fSin1	= (RwReal)sin( fAngleCurr );
		fSin2	= (RwReal)sin( fAngleCurr + fAngleStep );
		fCos1	= (RwReal)cos( fAngleCurr );
		fCos2	= (RwReal)cos( fAngleCurr + fAngleStep );

		//cone
		v1.z	= m_stAxisHeader.m_fAxisLen + m_stAxisHeader.m_fConeHeight;

		v2.x	= m_stAxisHeader.m_fConeRadius * fCos1;
		v2.y	= m_stAxisHeader.m_fConeRadius * fSin1;		
		v3.x	= m_stAxisHeader.m_fConeRadius * fCos2;
		v3.y	= m_stAxisHeader.m_fConeRadius * fSin2;

		(pConeZ+0)->Update( v1, m_stAxisHeader.m_dwPColZ );
		(pConeZ+1)->Update( v2, m_stAxisHeader.m_dwNColZ );
		(pConeZ+2)->Update( v3, m_stAxisHeader.m_dwNColZ );

		//cone bottom
		v1.z	= m_stAxisHeader.m_fAxisLen;

		(pConeBZ+0)->Update( v1, m_stAxisHeader.m_dwPColZ );
		(pConeBZ+1)->Update( v3, m_stAxisHeader.m_dwNColZ );
		(pConeBZ+2)->Update( v2, m_stAxisHeader.m_dwNColZ );

		fAngleCurr	+= fAngleStep;
		pConeZ		+= 3;
		pConeBZ		+= 3;
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::bSetupNumOfAngle( RwUInt32 dwNumOfAngle )
{
	m_stAxisHeader.m_dwNumOfAngle	= dwNumOfAngle;

	vClear();

	// 2 : 라인에 필요한 점.
	// 3 : 삼각형당 필요한 점.
	// 2 : 콘 + 콘바닥
	// 3 : 축이 3개(x, y z)
	m_stlvecVtx.resize( ( 2 + m_stAxisHeader.m_dwNumOfAngle * 3 * 2 ) * 3 );

	vUpdata();
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::bSetupSize( RwReal fAxisLen
					  , RwReal fConeHeight
					  , RwReal fConeRadius )
{
	m_stAxisHeader.m_fAxisLen		= fAxisLen;
	m_stAxisHeader.m_fConeHeight	= fConeHeight;
	m_stAxisHeader.m_fConeRadius	= fConeRadius;
	
	vUpdata();
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::bSetupColorPositive( RwUInt32 dwColX, RwUInt32 dwColY, RwUInt32 dwColZ )
{
	LPRWIM3DVTX	pLineX	= vGetPtrLineX() + 1;
	LPRWIM3DVTX	pLineY	= vGetPtrLineY() + 1;
	LPRWIM3DVTX	pLineZ	= vGetPtrLineZ() + 1;

	LPRWIM3DVTX	pConeX	= vGetPtrConeX(); 
	LPRWIM3DVTX	pConeY	= vGetPtrConeY(); 
	LPRWIM3DVTX	pConeZ	= vGetPtrConeZ(); 

	LPRWIM3DVTX	pConeBX	= vGetPtrConeBottomX(); 
	LPRWIM3DVTX	pConeBY	= vGetPtrConeBottomY(); 
	LPRWIM3DVTX	pConeBZ	= vGetPtrConeBottomZ(); 

	pLineX->Update( dwColX );
	pLineY->Update( dwColY );
	pLineZ->Update( dwColZ );
		
	int	i	= 0;
	for( i=0; i<(int)m_stAxisHeader.m_dwNumOfAngle; ++i ){
		(pConeX)->Update( dwColX );
		(pConeY)->Update( dwColY );
		(pConeZ)->Update( dwColZ );
		
		(pConeBX)->Update( dwColX );
		(pConeBY)->Update( dwColY );
		(pConeBZ)->Update( dwColZ );

		pConeX	+=3;
		pConeY	+=3;
		pConeZ	+=3;
		
		pConeBX	+=3;
		pConeBY	+=3;
		pConeBZ	+=3;
	};
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::bSetupColorNegative( RwUInt32 dwColX, RwUInt32 dwColY, RwUInt32 dwColZ )
{
	LPRWIM3DVTX	pLineX	= vGetPtrLineX();
	LPRWIM3DVTX	pLineY	= vGetPtrLineY();
	LPRWIM3DVTX	pLineZ	= vGetPtrLineZ();

	LPRWIM3DVTX	pConeX	= vGetPtrConeX(); 
	LPRWIM3DVTX	pConeY	= vGetPtrConeY(); 
	LPRWIM3DVTX	pConeZ	= vGetPtrConeZ(); 

	LPRWIM3DVTX	pConeBX	= vGetPtrConeBottomX(); 
	LPRWIM3DVTX	pConeBY	= vGetPtrConeBottomY(); 
	LPRWIM3DVTX	pConeBZ	= vGetPtrConeBottomZ(); 

	pLineX->Update( dwColX );
	pLineY->Update( dwColY );
	pLineZ->Update( dwColZ );
		
	int	i	= 0;
	for( i=0; i<(int)m_stAxisHeader.m_dwNumOfAngle; ++i ){
		(pConeX + 1)->Update( dwColX );
		(pConeY + 1)->Update( dwColY );
		(pConeZ + 1)->Update( dwColZ );
		(pConeX + 2)->Update( dwColX );
		(pConeY + 2)->Update( dwColY );
		(pConeZ + 2)->Update( dwColZ );
		
		(pConeBX + 1)->Update( dwColX );
		(pConeBY + 1)->Update( dwColY );
		(pConeBZ + 1)->Update( dwColZ );
		(pConeBX + 2)->Update( dwColX );
		(pConeBY + 2)->Update( dwColY );
		(pConeBZ + 2)->Update( dwColZ );

		pConeX	+=3;
		pConeY	+=3;
		pConeZ	+=3;
		
		pConeBX	+=3;
		pConeBY	+=3;
		pConeBZ	+=3;
	};
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CAxis::bSetupSemi( RwBool bSemi )
{
	m_bSemi	= bSemi;

	LPRWIM3DVTX	pLineX	= vGetPtrLineX();
	LPRWIM3DVTX	pLineY	= vGetPtrLineY();
	LPRWIM3DVTX	pLineZ	= vGetPtrLineZ();

	if( m_bSemi ){//-m_stAxisHeader.m_fAxisLen;
		pLineX->m_vtx.objVertex.x	= 
		pLineY->m_vtx.objVertex.y	= 
		pLineZ->m_vtx.objVertex.z	= 0.f;
	}else{
		pLineX->m_vtx.objVertex.x	= 
		pLineY->m_vtx.objVertex.y	= 
		pLineZ->m_vtx.objVertex.z	= -m_stAxisHeader.m_fAxisLen;
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwBool CAxis::bRender(RwMatrix* pMat, RwUInt32* pTransformFlags)
{	
	BOOL	br	= FALSE;

	if( m_bShowLine ){
		if( RwIm3DTransform(  &m_stlvecVtx[0].m_vtx
							, 6
							, pMat ? pMat : &m_mat
							, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
		{                               
			br	= RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
        
			RwIm3DEnd();

			if( br == FALSE )	return FALSE;

		}else	
			return FALSE;
	}

	if( m_bShowCone ){
		if( RwIm3DTransform(  &m_stlvecVtx[6].m_vtx
							, (RwUInt32)m_stlvecVtx.size()-6
							, pMat ? pMat : &m_mat
							, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
		{                               
			br	= RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        
			RwIm3DEnd();

			if( br == FALSE )	return FALSE;

		}else	
			return FALSE;
	}

	return br;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwBool CAxis::bRenderXZLine(RwMatrix* pMat, RwUInt32* pTransformFlags)
{
	BOOL	br	= FALSE;

	if( m_bShowLine ){
		if( RwIm3DTransform(  &vGetPtrLineX()->m_vtx
							, 2
							, pMat ? pMat : &m_mat
							, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
		{                               
			br	= RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
        
			RwIm3DEnd();

			if( br == FALSE )	
				return FALSE;

		}else	
			return FALSE;
		
		if( RwIm3DTransform(  &vGetPtrLineZ()->m_vtx
							, 2
							, pMat ? pMat : &m_mat
							, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
		{                               
			br	= RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
        
			RwIm3DEnd();

			if( br == FALSE )	
				return FALSE;

		}else	
			return FALSE;

	}

	return br;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwBool CAxis::bPick( RwReal& fDist, EAXISORDER& eAxis, RwMatrix* pMat, CPickUser* pPick )
{
	RwBool	br		= FALSE,
			bTemp	= FALSE;


	RwV3d	uvRay	= {0.f, 0.f, 0.f};
	RwV3d	vCamPos	= {0.f, 0.f, 0.f};
	
	pPick->bGetRayCamPosWithMat( &uvRay, &vCamPos, pMat );

	RwReal	fU	= 0.f,
			fV	= 0.f,
			fT	= 0.f;

	LPRWIM3DVTX	pConeX	= vGetPtrConeX(); 
	LPRWIM3DVTX	pConeY	= vGetPtrConeY(); 
	LPRWIM3DVTX	pConeZ	= vGetPtrConeZ(); 

	LPRWIM3DVTX	pConeBX	= vGetPtrConeBottomX(); 
	LPRWIM3DVTX	pConeBY	= vGetPtrConeBottomY(); 
	LPRWIM3DVTX	pConeBZ	= vGetPtrConeBottomZ(); 

	int	i	= 0;
	for( i=0; i<(int)m_stAxisHeader.m_dwNumOfAngle; ++i ){
		//x axis
		bTemp	= pPick->IntersectTriangle( &uvRay, &vCamPos
									, &(pConeX+0)->m_vtx.objVertex
									, &(pConeX+1)->m_vtx.objVertex
									, &(pConeX+2)->m_vtx.objVertex
									, br ? &fT : &fDist, &fU, &fV );
		if( bTemp ){
			if( br ){
				if( fT < fDist ){
					fDist	= fT;
					eAxis	= AXIS_X;
				}
			}else{
				br		= bTemp;
				eAxis	= AXIS_X;
			}
		}
		bTemp	= pPick->IntersectTriangle( &uvRay, &vCamPos
									, &(pConeBX+0)->m_vtx.objVertex
									, &(pConeBX+1)->m_vtx.objVertex
									, &(pConeBX+2)->m_vtx.objVertex
									, br ? &fT : &fDist, &fU, &fV );
		if( bTemp ){
			if( br ){
				if( fT < fDist ){
					fDist	= fT;
					eAxis	= AXIS_X;
				}
			}else{
				br		= bTemp;
				eAxis	= AXIS_X;
			}
		}
		//y axis
		bTemp	= pPick->IntersectTriangle( &uvRay, &vCamPos
									, &(pConeY+0)->m_vtx.objVertex
									, &(pConeY+1)->m_vtx.objVertex
									, &(pConeY+2)->m_vtx.objVertex
									, br ? &fT : &fDist, &fU, &fV );
		if( bTemp ){
			if( br ){
				if( fT < fDist ){
					fDist	= fT;
					eAxis	= AXIS_Y;
				}
			}else{
				br		= bTemp;
				eAxis	= AXIS_Y;
			}
		}
		bTemp	= pPick->IntersectTriangle( &uvRay, &vCamPos
									, &(pConeBY+0)->m_vtx.objVertex
									, &(pConeBY+1)->m_vtx.objVertex
									, &(pConeBY+2)->m_vtx.objVertex
									, br ? &fT : &fDist, &fU, &fV );
		if( bTemp ){
			if( br ){
				if( fT < fDist ){
					fDist	= fT;
					eAxis	= AXIS_Y;
				}
			}else{
				br		= bTemp;
				eAxis	= AXIS_Y;
			}
		}
		//z axis
		bTemp	= pPick->IntersectTriangle( &uvRay, &vCamPos
									, &(pConeZ+0)->m_vtx.objVertex
									, &(pConeZ+1)->m_vtx.objVertex
									, &(pConeZ+2)->m_vtx.objVertex
									, br ? &fT : &fDist, &fU, &fV );
		if( bTemp ){
			if( br ){
				if( fT < fDist ){
					fDist	= fT;
					eAxis	= AXIS_Z;
				}
			}else{
				br		= bTemp;
				eAxis	= AXIS_Z;
			}
		}
		bTemp	= pPick->IntersectTriangle( &uvRay, &vCamPos
									, &(pConeBZ+0)->m_vtx.objVertex
									, &(pConeBZ+1)->m_vtx.objVertex
									, &(pConeBZ+2)->m_vtx.objVertex
									, br ? &fT : &fDist, &fU, &fV );
		if( bTemp ){
			if( br ){
				if( fT < fDist ){
					fDist	= fT;
					eAxis	= AXIS_Z;
				}
			}else{
				br		= bTemp;
				eAxis	= AXIS_Z;
			}
		}


		pConeX	+=3;
		pConeY	+=3;
		pConeZ	+=3;
		
		pConeBX	+=3;
		pConeBY	+=3;
		pConeBZ	+=3;
	};

	return br;
};