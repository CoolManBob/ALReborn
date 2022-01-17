// AcuObjecWire.cpp
// -----------------------------------------------------------------------------
//                       ____  _       _           _   _   _ _                                
//     /\               / __ \| |     (_)         | | | | | (_)                               
//    /  \    ___ _   _| |  | | |__    _  ___  ___| | | | | |_ _ __  ___      ___ _ __  _ __  
//   / /\ \  / __| | | | |  | | '_ \  | |/ _ \/ __| | | | | | | '__|/ _ \    / __| '_ \| '_ \ 
//  / ____ \| (__| |_| | |__| | |_) | | |  __/ (__|  V _ V  | | |  |  __/ _ | (__| |_) | |_) |
// /_/    \_\\___|\__,_|\____/|_.__/  | |\___|\___|\__/ \__/|_|_|   \___|(_) \___| .__/| .__/ 
//                                   _/ |                                        | |   | |    
//                                  |__/                                         |_|   |_|    
//
// post fx effect
//
// -----------------------------------------------------------------------------
// Originally created on 05/13/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AcuObjecWire.h"
#include "ApDefine.h"

#include <vector>
#include <algorithm>

#include "AcuMathFunc.h"
USING_ACUMATH;

#include "AgcuBasisGeo.h"
#include "AgcuVtxType.h"
USING_AGCUVTX;

#include "AgcdEffGlobal.h"

namespace PRIVATE
{
	USING_AGCUGEOUD;

	AgcuGeo_Sphere<VTX_PD>	sphere(0.5f, 24, "sphere");

	std::vector<WORD>		m_ib;
	std::vector<VTX_PD>		m_vb;
	RwUInt32				m_colr;
	
	//Render atomic
	RpAtomic* RenderAtomic( RpAtomic* atom, void* data=NULL )
	{
		//@{ 2006/02/22 burumal

		/*
		if( !atom )
			return atom;
		RpGeometry*	 geo	= RpAtomicGetGeometry( atom );
		if( !geo )
			return atom;
		RpMorphTarget*	morph	= RpGeometryGetMorphTarget(geo, 0);
		if( !morph )
			return atom;
		RwV3d*			verts	= RpMorphTargetGetVertices( morph );
		if( !verts )
			return atom;
		RpTriangle*		tri		= RpGeometryGetTriangles(geo);
		RwInt32			numtri	= RpGeometryGetNumTriangles(geo);
		RwInt32			numvtx	= RpGeometryGetNumVertices(geo);
		
		m_ib.clear();
		m_ib.reserve( numtri * 6 );

		for( RwInt32 i=0; i<numtri; ++i, ++tri )
		{
			m_ib.push_back( tri->vertIndex[0] );
			m_ib.push_back( tri->vertIndex[1] );
			m_ib.push_back( tri->vertIndex[1] );
			m_ib.push_back( tri->vertIndex[2] );
			m_ib.push_back( tri->vertIndex[2] );
			m_ib.push_back( tri->vertIndex[0] );
		}

		m_vb.clear();
		m_vb.reserve(numvtx);
		VTX_PD	vtx( *(D3DXVECTOR3*)verts, m_colr );
		for( RwInt32 i=0; i<numvtx; ++i, ++verts )
		{
			vtx.pos = *(D3DXVECTOR3*)verts;
			m_vb.push_back( vtx );
		}

		RwD3D9SetFVF( VTX_PD::FVF );
		RwD3D9SetTransformWorld( RwFrameGetLTM( RpAtomicGetFrame(atom) ) );
		
		RwD3D9DrawIndexedPrimitiveUP ( D3DPT_LINELIST
			, 0LU
			, numvtx
			, numtri*3
			, &m_ib.front()
			, &m_vb.front()
			, VTX_PD::SIZE
			);

		return atom;
		*/

		RpAtomic* pFirst = atom;

		do {
			if( !atom )
				return atom;
			RpGeometry*	 geo	= RpAtomicGetGeometry( atom );
			if( !geo )
				return atom;
			RpMorphTarget*	morph	= RpGeometryGetMorphTarget(geo, 0);
			if( !morph )
				return atom;
			RwV3d*			verts	= RpMorphTargetGetVertices( morph );
			if( !verts )
				return atom;
			RpTriangle*		tri		= RpGeometryGetTriangles(geo);
			RwInt32			numtri	= RpGeometryGetNumTriangles(geo);
			RwInt32			numvtx	= RpGeometryGetNumVertices(geo);

			m_ib.clear();
			m_ib.reserve( numtri * 6 );

			for( RwInt32 i=0; i<numtri; ++i, ++tri )
			{
				m_ib.push_back( tri->vertIndex[0] );
				m_ib.push_back( tri->vertIndex[1] );
				m_ib.push_back( tri->vertIndex[1] );
				m_ib.push_back( tri->vertIndex[2] );
				m_ib.push_back( tri->vertIndex[2] );
				m_ib.push_back( tri->vertIndex[0] );
			}

			m_vb.clear();
			m_vb.reserve(numvtx);
			VTX_PD	vtx( *(D3DXVECTOR3*)verts, m_colr );
			for( RwInt32 i=0; i<numvtx; ++i, ++verts )
			{
				vtx.pos = *(D3DXVECTOR3*)verts;
				m_vb.push_back( vtx );
			}

			RwD3D9SetFVF( VTX_PD::FVF );
			RwD3D9SetTransformWorld( RwFrameGetLTM( RpAtomicGetFrame(atom) ) );

			RwD3D9DrawIndexedPrimitiveUP ( D3DPT_LINELIST
				, 0LU
				, numvtx
				, numtri*3
				, &m_ib.front()
				, &m_vb.front()
				, VTX_PD::SIZE
				);

			atom = atom->next;

		} while (atom && (atom != pFirst));

		return atom;

		//@}
	}
};

// -----------------------------------------------------------------------------
AcuObjecWire& AcuObjecWire::bGetInst()
{
	static AcuObjecWire inst;
	return inst;
}
// -----------------------------------------------------------------------------
AcuObjecWire::AcuObjecWire()
: m_flag(0LU)
, m_fptrRenderSphere	( &AcuObjecWire::vRenderNothing )
, m_fptrRenderClump		( &AcuObjecWire::vRenderNothing )
, m_fptrRenderCollision	( &AcuObjecWire::vRenderNothing )
, m_fptrRenderPicking	( &AcuObjecWire::vRenderNothing )
{
}

// -----------------------------------------------------------------------------
AcuObjecWire::~AcuObjecWire()
{
}
	
// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bAddClump(RpClump* pClump)
{
	ASSERT( pClump );
	if( pClump )
		m_containerClump.push_back(pClump);

	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bRemClump(RpClump* pClump)
{
	CLUMP_ITR	it_f =
		std::find( m_containerClump.begin(), m_containerClump.end(), pClump );
	if( it_f != m_containerClump.end() )
		m_containerClump.erase( it_f );

	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bClear()
{
	m_containerClump.clear();

	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bRender()
{
	if( m_flag == 0L )
		return 0;

	USING_AGCUGEOUD;
	AgcuRenderStateForWire Setrenderstate;

	RwInt32 ir = 0L;

	CLUMP_ITR	it_curr = m_containerClump.begin();
	CLUMP_ITR	it_last = m_containerClump.end();

	for( ; (it_curr != it_last) && (ir == 0L); ++it_curr )
		ir = vRenderObj( *it_curr );

	return ir;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bRenderAtomic(RpAtomic* atom, RwUInt32 colr)
{
	if( DEF_FLAG_ON(m_flag, EFLAG_CLUMP) )
	{
		USING_AGCUGEOUD;
		AgcuRenderStateForWire Setrenderstate;

		PRIVATE::m_colr	= colr;
		PRIVATE::RenderAtomic( atom );
	}

	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bRenderClump(RpClump* clump)
{
	if( clump )
	{
		CLUMP_ITR	it_f =
			std::find( m_containerClumpImdeate.begin(), m_containerClumpImdeate.end(), clump );
		if( it_f == m_containerClumpImdeate.end() )
		{			
			USING_AGCUGEOUD;
			AgcuRenderStateForWire Setrenderstate;

			m_containerClumpImdeate.push_back(clump);
			vRenderObj(clump);
		}
	}

	return 0LU;
};
// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bRenderClumpOnce(RpClump* clump)
{
	USING_AGCUGEOUD;
	AgcuRenderStateForWire Setrenderstate;
	return vRenderObj(clump);
}
// -----------------------------------------------------------------------------
void AcuObjecWire::bOnIdle(void)
{
	m_containerClumpImdeate.clear();
};
// -----------------------------------------------------------------------------
void AcuObjecWire::bOnOffSphere(bool on)
{
	on ? DEF_FLAG_ON(m_flag, EFLAG_SPHERE) : DEF_FLAG_OFF(m_flag, EFLAG_SPHERE);
	m_fptrRenderSphere = on ? &AcuObjecWire::vRenderSphere : &AcuObjecWire::vRenderNothing;
};

// -----------------------------------------------------------------------------
void AcuObjecWire::bOnOffClump(bool on)
{
	on ? DEF_FLAG_ON(m_flag, EFLAG_CLUMP) : DEF_FLAG_OFF(m_flag, EFLAG_CLUMP);
	m_fptrRenderClump = on ? &AcuObjecWire::vRenderClump : &AcuObjecWire::vRenderNothing;
};

// -----------------------------------------------------------------------------
void AcuObjecWire::bOnOffCollision(bool on)
{
	on ? DEF_FLAG_ON(m_flag, EFLAG_COLLISION) : DEF_FLAG_OFF(m_flag, EFLAG_COLLISION);
	m_fptrRenderCollision = on ? &AcuObjecWire::vRenderCollision : &AcuObjecWire::vRenderNothing;
};

// -----------------------------------------------------------------------------
void AcuObjecWire::bOnOffPicking(bool on)
{
	on ? DEF_FLAG_ON(m_flag, EFLAG_PICKING) : DEF_FLAG_OFF(m_flag, EFLAG_PICKING);
	m_fptrRenderPicking = on ? &AcuObjecWire::vRenderPicking : &AcuObjecWire::vRenderNothing;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::vRenderObj(RpClump* clump)
{
	(this->*m_fptrRenderSphere)(clump);
	(this->*m_fptrRenderClump)(clump);
	(this->*m_fptrRenderCollision)(clump);
	(this->*m_fptrRenderPicking)(clump);
	
	return 0LU;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::vRenderSphere(RpClump* clump)
{
	//D3DXMATRIX	mat(1.f,0.f,0.f,0.f,
	//				0.f,1.f,0.f,0.f,
	//				0.f,0.f,1.f,0.f,
	//				0.f,0.f,0.f,1.f);
	//mat.m[0][0]	= 
	//mat.m[1][1] = 
	//mat.m[2][2]	= 
	//clump->stType.boundingSphere.radius+clump->stType.boundingSphere.radius;

	////AuAutoFile fp("kday radius.txt", "at" );
	////if( fp )
	////	fprintf( fp, "%f\n", clump->stType.boundingSphere.radius );

	//mat.m[3][0] = clump->stType.boundingSphere.center.x;
	//mat.m[3][1] = clump->stType.boundingSphere.center.y;
	//mat.m[3][2] = clump->stType.boundingSphere.center.z;

	//*(D3DXVECTOR3*)&mat.m[3][0] += *(D3DXVECTOR3*)&RwFrameGetLTM(RpClumpGetFrame( clump ))->pos;

	//USING_AGCUGEOUD;
	//AgcuGeoBase<VTX_PD>*	psphere = &PRIVATE::sphere;
	////if( FAILED( PRIVATE::sphere.::bRenderW( AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->m_pCurD3D9Device, &mat ) ) )
	////	return -1;
	////psphere->bRenderW(AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->m_pCurD3D9Device, &mat);

	RwSphere	sphere = clump->stType.boundingSphere;
	//RwV3dAdd( &sphere.center, &sphere.center, RwMatrixGetPos(RwFrameGetLTM(RpClumpGetFrame( clump ))) );
	RwV3dTransformPoint( &sphere.center, &sphere.center, RwFrameGetLTM(RpClumpGetFrame( clump )) );

	USING_AGCUGEOUD;
	AgcuGeoBase<VTX_PD>*	psphere = &PRIVATE::sphere;
	psphere->bRwRenderW( sphere );

	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::vRenderClump(RpClump* clump)
{
	PRIVATE::m_colr	= 0xFF00FFFF;
	RpClumpForAllAtomics ( clump, PRIVATE::RenderAtomic, NULL );
	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::vRenderCollision(RpClump* clump)
{
	PRIVATE::m_colr	= 0xFFFF0000;
	PRIVATE::RenderAtomic( (RpAtomic*)clump->stType.pCollisionAtomic );
	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::vRenderPicking(RpClump* clump)
{
	PRIVATE::m_colr	= 0xFFFFFF00;
	PRIVATE::RenderAtomic( (RpAtomic*)clump->stType.pPickAtomic );
	return 0;
};


// -----------------------------------------------------------------------------
RwInt32 AcuObjecWire::bRenderSphere( const RwSphere& sphere )
{
	USING_AGCUGEOUD;
	AgcuRenderStateForWire Setrenderstate;

	USING_AGCUGEOUD;
	AgcuGeoBase<VTX_PD>*	psphere = &PRIVATE::sphere;
	psphere->bRwRenderW( sphere );

	return 0;
};

// -----------------------------------------------------------------------------
// AcuObjecWire.cpp - End of file
// -----------------------------------------------------------------------------
