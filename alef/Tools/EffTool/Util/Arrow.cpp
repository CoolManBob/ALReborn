// Arrow.cpp: implementation of the CArrow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "RwWzrdTest1.h"
#include "Arrow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// const variable
const RwUInt32		COLBEGIN	= 0xFFFFFFFF;//WHITE
const RwUInt32		COLEND		= 0xFF808000;//YELLOW
const RwUInt32		NUMOFANGLE	= 13;
const RwReal		ARROWLEN	= 1.f;
const RwReal		CONEHEIGHT	= 1.f;
const RwReal		CONERADIUS	= .125f;
const RwReal		PHI			= 3.1415926536f;
const RwReal		TWOPHI		= 6.2831853072f;

///////////////////////////////////////////////////////////////////////////////
// struct
CArrow::tagstArrowHeader::tagstArrowHeader()
: m_dwNumOfAngle( NUMOFANGLE )
, m_fLineLen	( ARROWLEN	 )
, m_fConeHeight ( CONEHEIGHT )
, m_fConeRadius ( CONERADIUS )
, m_dwColBegin ( COLBEGIN	 )
, m_dwColEnd	( COLEND	 )
{
	m_v3dStart.x =
	m_v3dStart.y =
	m_v3dStart.z = 0.f; //world's orgin

	m_uv3dDir.x	 = 
	m_uv3dDir.y	 = 0.f;	
	m_uv3dDir.z	 = 1.f;	//world's z axis
};

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////

CArrow::CArrow()
: m_dwTransformFlags(rwIM3D_ALLOPAQUE)
{
	memset(&m_mat, 0, sizeof(m_mat));
	m_mat.right.x	= 
	m_mat.up.y		= 
	m_mat.at.z		= 1.f;

	m_stlvecVtx.resize( (2)/*LINE-LIST*/ + ((NUMOFANGLE * 3) * 2)/*TRI-LIST*/ );

	vUpdate();
}

CArrow::~CArrow()
{
	vClear();
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::vClear()
{
	m_stlvecVtx.clear();
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::vUpdate()
{
	LPRWIM3DVTX	pLine	= vGetPtrLine();
	LPRWIM3DVTX	pCone	= vGetPtrCone();
	LPRWIM3DVTX	pConeB	= vGetPtrConeBottom();

	RwV3d	v1,v2;

	RwReal	fAngleStep	= 360.f / (RwReal)m_stArrowHeader.m_dwNumOfAngle;
	RwReal	fAngleCurr	= fAngleStep;
	RwReal	fSin1		= 0.f;
	RwReal	fSin2		= 0.f;
	RwReal	fCos1		= 0.f;
	RwReal	fCos2		= 0.f;

	//line
	v1	= m_stArrowHeader.m_v3dStart;
	RwV3dScale	( &v2, &m_stArrowHeader.m_uv3dDir, m_stArrowHeader.m_fLineLen );
	RwV3dAdd	( &v2, &v1, &v2 );
	(pLine+0)->Update( v1, m_stArrowHeader.m_dwColBegin );
	(pLine+1)->Update( v2, m_stArrowHeader.m_dwColEnd );

	//cone & cone bottom
	RwV3d	v3WY			= {0.f, 1.f, 0.f},
			v3BC			= v2,	//BC : Bottom center
			v3CP				,	//CP : Cone Peak
			v3CrossDirWithWY	;	// v3CrossDirWithWY	= World'Y cross m_stArrowHeader.m_uv3dDir

	RwMatrix	matRot;
	
	RwV3dScale	( &v3CP, &m_stArrowHeader.m_uv3dDir, m_stArrowHeader.m_fConeHeight );
	RwV3dAdd	( &v3CP, &v3BC, &v3CP );

	if( m_stArrowHeader.m_uv3dDir.y > -0.9999f &&
		m_stArrowHeader.m_uv3dDir.y <  0.9999f ){//to avoid zimblock
		RwV3dCrossProduct	( &v3CrossDirWithWY, &v3WY, &m_stArrowHeader.m_uv3dDir );
		RwV3dNormalize		( &v3CrossDirWithWY, &v3CrossDirWithWY );
		RwV3dScale			( &v3CrossDirWithWY, &v3CrossDirWithWY, m_stArrowHeader.m_fConeRadius );
	}else{
		v3CrossDirWithWY.x	= 1.f;
		v3CrossDirWithWY.y	=
		v3CrossDirWithWY.z	= 0.f;
	}


	v1.x = v1.y = v1.z = 0.f;
	v2.x = v2.y = v2.z = 0.f;

	//v1	= v3CrossDirWithWY;
	RwV3dAdd(&v1, &v3BC, &v3CrossDirWithWY);

	for( int i = 0; i<(int)m_stArrowHeader.m_dwNumOfAngle; ++i ){

		RwMatrixRotate( &matRot, &m_stArrowHeader.m_uv3dDir, fAngleCurr, rwCOMBINEREPLACE );
		RwV3dTransformPoint( &v2, &v3CrossDirWithWY, &matRot );
		RwV3dAdd(&v2, &v3BC, &v2);

		//cone
		(pCone+0)->Update( v3CP	, m_stArrowHeader.m_dwColEnd );
		(pCone+1)->Update( v1	, m_stArrowHeader.m_dwColBegin );
		(pCone+2)->Update( v2	, m_stArrowHeader.m_dwColBegin );

		//cone bottom
		(pConeB+0)->Update( v3BC, m_stArrowHeader.m_dwColEnd );
		(pConeB+1)->Update( v2	, m_stArrowHeader.m_dwColBegin );
		(pConeB+2)->Update( v1	, m_stArrowHeader.m_dwColBegin );

		v1			 = v2;
		fAngleCurr	+= fAngleStep;
		pCone		+= 3;
		pConeB		+= 3;
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetupNumOfAngle( RwUInt32 dwNumOfAngle )
{
	m_stArrowHeader.m_dwNumOfAngle	= dwNumOfAngle;

	vClear();

	// 2 : 라인에 필요한 점.
	// 3 : 삼각형당 필요한 점.
	// 2 : 콘 + 콘바닥
	// 3 : 축이 3개(x, y z)
	m_stlvecVtx.resize( 2 + m_stArrowHeader.m_dwNumOfAngle * 3 * 2 );

	vUpdate();
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetupSize		( RwReal fLineLen
							, RwReal fConeHeight
							, RwReal fConeRadius )
{
	m_stArrowHeader.m_fLineLen		= fLineLen;
	m_stArrowHeader.m_fConeHeight	= fConeHeight;
	m_stArrowHeader.m_fConeRadius	= fConeRadius;
	
	vUpdate();
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetPosDirLen(const RwV3d& v3dStartPos
						 , const RwV3d& uv3dDir
						 , RwReal fLineLen )
{
	m_stArrowHeader.m_v3dStart	= v3dStartPos;
	RwV3dNormalize( &m_stArrowHeader.m_uv3dDir, &uv3dDir );
	m_stArrowHeader.m_fLineLen		= fLineLen;

	vUpdate();
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetStartNTarget( const RwV3d& v3dStartPos
							, const RwV3d& v3dTarget )
{
	RwV3d	v3dTSubS	= {0.f, 0.f, 0.f};// target - startpos
	RwV3dSub( &v3dTSubS, &v3dTarget, &v3dStartPos );

	RwReal	lenTSubS	= RwV3dLength(&v3dTSubS);	
	
	RwV3dNormalize( &m_stArrowHeader.m_uv3dDir, &v3dTSubS );
	m_stArrowHeader.m_v3dStart	= v3dStartPos;
	m_stArrowHeader.m_fLineLen	= lenTSubS - m_stArrowHeader.m_fConeHeight;

	vUpdate();
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetupColor( RwUInt32 dwColBegin, RwUInt32 dwColEnd)
{
	m_stArrowHeader.m_dwColBegin	= dwColBegin;
	m_stArrowHeader.m_dwColEnd		= dwColEnd;

	
	LPRWIM3DVTX	pLine	= vGetPtrLine();
	LPRWIM3DVTX	pCone	= vGetPtrCone();
	LPRWIM3DVTX	pConeB	= vGetPtrConeBottom();

	//line
	(pLine+0)->Update(dwColBegin);
	(pLine+1)->Update(dwColEnd);

	int	i	= 0;
	for( i=0; i<(int)m_stArrowHeader.m_dwNumOfAngle; ++i )
	{
		(pCone + 0)->Update( dwColBegin );
		(pCone + 1)->Update( dwColEnd );
		(pCone + 2)->Update( dwColEnd );
		
		(pConeB + 0)->Update( dwColBegin );
		(pConeB + 1)->Update( dwColEnd );
		(pConeB + 2)->Update( dwColEnd );

		pCone	+=3;
		pConeB	+=3;
	};

};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetPos( const RwV3d& v3dStartPos )
{
	m_stArrowHeader.m_v3dStart	= v3dStartPos;
	vUpdate();
};
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID CArrow::bSetDir( const RwV3d& uv3dDir )
{
	RwV3dNormalize( &m_stArrowHeader.m_uv3dDir, &uv3dDir );
	vUpdate();
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwBool CArrow::bRender(RwMatrix* pMat, RwUInt32* pTransformFlags)
{
	if( pMat == NULL )
		pMat	= &m_mat;

	
	BOOL	br	= FALSE;
    if( RwIm3DTransform(  &m_stlvecVtx[0]
						, 2
						, pMat
						, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
    {                               
        br	= RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
        
        RwIm3DEnd();

		if( br == FALSE )	return FALSE;

    }else	
		return FALSE;

    if( RwIm3DTransform(  &m_stlvecVtx[2]
						, (RwUInt32)m_stlvecVtx.size()-2
						, pMat
						, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
    {                               
        br	= RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        
        RwIm3DEnd();

		if( br == FALSE )	return FALSE;

    }else	
		return FALSE;

	return TRUE;
};