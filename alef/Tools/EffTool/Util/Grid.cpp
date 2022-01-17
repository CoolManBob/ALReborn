// Grid.cpp: implementation of the CGrid class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "Grid.h"

//struct RxObjSpace3DVertex
//{
//    RwV3d       objVertex;        /**< position */
//    RwV3d       objNormal;        /**< normal */
//    RwUInt32    color;            /**< emissive color*/
//    RwReal      u;                /**< u */
//    RwReal      v;                /**< v */
//};

CGrid::tagstLine::tagstLine()
{
	memset( this, 0, sizeof( *this ) );
};
CGrid::tagstLine::tagstLine(const RwV3d& v3d1, const RwV3d& v3d2, RwUInt32 dwCol )
{
	memset( this, 0, sizeof( *this ) );

	m_rwIm3dVtx1.objVertex	= v3d1;
	m_rwIm3dVtx2.objVertex	= v3d2;

	m_rwIm3dVtx1.color		= 
	m_rwIm3dVtx2.color		= dwCol;
};
CGrid::tagstLine::tagstLine(const tagstLine& cpy)
{
	m_rwIm3dVtx1	= cpy.m_rwIm3dVtx1;
	m_rwIm3dVtx2	= cpy.m_rwIm3dVtx2;
};
		
CGrid::tagstLine& CGrid::tagstLine::operator= (const tagstLine& cpy)
{
	if( this == &cpy )	return *this;
	
	m_rwIm3dVtx1	= cpy.m_rwIm3dVtx1;
	m_rwIm3dVtx2	= cpy.m_rwIm3dVtx2;

	return *this;
};

void CGrid::tagstLine::Update( RwV3d* pv3d1, RwV3d* pv3d2, RwUInt32* pDwCol )
{
	if( pv3d1 )	m_rwIm3dVtx1.objVertex	= *pv3d1;
	if( pv3d2 )	m_rwIm3dVtx2.objVertex	= *pv3d2;
	if( pDwCol) m_rwIm3dVtx1.color		= 
				m_rwIm3dVtx2.color		= *pDwCol;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGrid::CGrid()
{
	memset(&m_mat, 0, sizeof(m_mat));
	m_mat.right.x	= 
	m_mat.up.y		= 
	m_mat.at.z		= 1.f;
};
CGrid::~CGrid()
{
	vClear();
};

VOID CGrid::vClear()
{
	m_stlvecStLine.clear();
}

RwBool CGrid::bRender(RwMatrix* pMat, RwUInt32* pTransformFlags)
{
	BOOL	br	= FALSE;
    if( RwIm3DTransform(  m_stlvecStLine[0].m_aRwIm3dVtx
						, m_dwLineNum<<1
						, pMat ? pMat : &m_mat
						, pTransformFlags ? *pTransformFlags : m_dwTransformFlags) )
    {                               
        br	= RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
        
        RwIm3DEnd();

		
		/*RwIm3DVertex	test[3];
		memset( test, 0, sizeof( test ) );
		test[1].objVertex.x	= 2.f;
		test[2].objVertex.x	= 1.f;
		test[2].objVertex.x	= 1.f;
		test[2].objVertex.z	= -1.f;

		test[0].color		= 
		test[1].color		= 
		test[2].color		= 0xffff0000;

		RwIm3DTransform( test, 3, &m_mat, *pTransformFlags );
        br	= RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);        
        RwIm3DEnd();/**/

    }else	
		return FALSE;

	return br;
};

RwInt32 CGrid::vUpdate( void )
{
	
	int	i	= 0;

	RwReal	currx	= 0.f,
			currz	= 0.f;

	RwV3d	v3dPos1	= m_v3dCenter,
			v3dPos2	= m_v3dCenter;

	STLINE	stLine(v3dPos1, v3dPos2, m_dwCol);


	v3dPos1.z		= m_v3dCenter.z - m_sizeWorld.cy * m_v2dTileSize.y * 0.5f;
	v3dPos2.z		= m_v3dCenter.z + m_sizeWorld.cy * m_v2dTileSize.y * 0.5f;

	currx	= m_v3dCenter.x - m_sizeWorld.cx * m_v2dTileSize.x * 0.5f;

	for( i=0; i<m_sizeWorld.cx+1; ++i ){
		v3dPos1.x	= 
		v3dPos2.x	= currx;

		stLine.Update( &v3dPos1, &v3dPos2, NULL );

		m_stlvecStLine.push_back(stLine);

		currx		+= m_v2dTileSize.x;
	}

	v3dPos1	= m_v3dCenter;
	v3dPos2	= m_v3dCenter;

	v3dPos1.x		= m_v3dCenter.z - m_sizeWorld.cx * m_v2dTileSize.x * 0.5f;
	v3dPos2.x		= m_v3dCenter.z + m_sizeWorld.cx * m_v2dTileSize.x * 0.5f;

	currz	= m_v3dCenter.z - m_sizeWorld.cy * m_v2dTileSize.y * 0.5f;

	for( i=0; i<m_sizeWorld.cy+1; ++i ){
		v3dPos1.z	= 
		v3dPos2.z	= currz;

		stLine.Update( &v3dPos1, &v3dPos2, NULL );

		m_stlvecStLine.push_back(stLine);

		currz		+= m_v2dTileSize.y;
	}

	return 0;
}

RwInt32 CGrid::bBuildGrid(RwInt32 wwidth, RwInt32 wdepth, 
						RwV2d v2dTileSize, 
						RwUInt32 col, RwV3d center)
{
	m_sizeWorld.cx	= wwidth;
	m_sizeWorld.cy	= wdepth;

	m_v2dTileSize		= v2dTileSize;

	m_dwCol			= col;
	m_v3dCenter		= center;

	vClear();
	m_stlvecStLine.reserve( wwidth+1 + wdepth+1 );

	m_dwLineNum	= wwidth+1 + wdepth+1;

	return vUpdate();
};

RwInt32 CGrid::bSetColor( RwUInt32 dwCol )
{
	m_dwCol			= dwCol;

	return vUpdate();
};

RwInt32 CGrid::bSetCenter( RwV3d v3dCenter )
{
	m_v3dCenter		= v3dCenter;

	return vUpdate();
};
RwInt32 CGrid::bSetSzieTile( RwV2d v2dTileSize )
{
	m_v2dTileSize		= v2dTileSize;

	return vUpdate();
};
RwInt32 CGrid::bSetSzieMap( RwInt32 wwidth, RwInt32 wdepth )
{
	m_sizeWorld.cx	= wwidth;
	m_sizeWorld.cy	= wdepth;

	return vUpdate();
};
