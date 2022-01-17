// AgcuPathFind.cpp
// -----------------------------------------------------------------------------
//                            _____        _   _     ______ _           _                      
//     /\                    |  __ \      | | | |   |  ____(_)         | |                     
//    /  \    __ _  ___ _   _| |__) | __ _| |_| |__ | |__   _ _ __   __| |     ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | |  ___/ / _` | __| '_ \|  __| | | '_ \ / _` |    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | |    | (_| | |_| | | | |    | | | | | (_| | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|_|     \__,_|\__|_| |_|_|    |_|_| |_|\__,_|(_) \___| .__/| .__/ 
//            __/ |                                                               | |   | |    
//           |___/                                                                |_|   |_|    
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 07/20/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcuPathFind.h"
#include "AcuMathFunc.h"
USING_ACUMATH

namespace NS_AgcuPathFind
{
	 RwInt32	TILELIMIT		= 32;		//16*2
	 RwInt32	TILEOFFSET		= 8;		//16*2
};
using namespace NS_AgcuPathFind;

// -----------------------------------------------------------------------------
AgcuPathFind::AgcuPathFind()
: m_pApmMap(NULL)
, m_fTileSize(400.f)
, m_bTruncatedDest(FALSE)
{
	m_cAstar.bInit(&this->m_cMapGeoInfo);
}

// -----------------------------------------------------------------------------
AgcuPathFind::~AgcuPathFind()
{
}

// -----------------------------------------------------------------------------
VOID AgcuPathFind::bSetModule
(ApmMap* pApmMap, AgcmMap* pAgcmMap, ApmObject* pApmObject, AgcmObject* pAgcmObject)
{
	ASSERT( pApmMap && !m_pApmMap && pAgcmMap && pAgcmObject );
	m_pApmMap = pApmMap;

	m_cMapGeoInfo.bSetModule( pApmMap, pAgcmMap, pApmObject, pAgcmObject );
};
// -----------------------------------------------------------------------------
const RwV3d* AgcuPathFind::bGetTruncatedDest()const
{
	ASSERT( m_bTruncatedDest );
	return ( m_bTruncatedDest ) ? (&m_cMapGeoInfo.bGetDest()) : NULL;
};

// -----------------------------------------------------------------------------
RwInt32	AgcuPathFind::vCalcTile(const RwV3d& start, const RwV3d& dest)
{
	RwInt32	ir = m_cMapGeoInfo.bSetRect( start, dest, TILELIMIT, TILEOFFSET );

	m_bTruncatedDest	= (ir) ? TRUE : FALSE;

#ifdef _DEBUG
	if( m_bTruncatedDest )
		ASSERT( ir == 1 );
	else
		ASSERT( ir == 0 );
#endif //_DEBUG

	return ir;
};

// -----------------------------------------------------------------------------
// -1 : error, 0 : go directly, 1 : with A*, 2 : can't find the path
RwInt32	AgcuPathFind::bFindPath
( const RwV3d& start, const RwV3d& dest, CONTAINER_WAYPOINT& out )
{
	RwInt32 ir = 0;

	vCalcTile(start, dest);

	//@{ kday 20050929
	// ;)
	// range limit
	if( m_bTruncatedDest )
		return 2;
	//@} kday

	m_cAstar.bReady();

	if( m_cMapGeoInfo.bLineBlock_Tile() ||
		m_cMapGeoInfo.bLineBlock_Obj() )
	{
		//lunch a*
		ir = m_cAstar.bGeneratePath( m_cMapGeoInfo.bGetPtStart()
								   , m_cMapGeoInfo.bGetPtDest()
								   , m_cMapGeoInfo.bGetStart().y);
		RwInt32 retChk = m_cAstar.bGetWayPoint( out, m_cMapGeoInfo );
		if( !retChk )
		{
			out.back() = start;
			if( ir == 1 )
				out.front() = dest;
			
			#ifdef _DEBUG
			else
				ASSERT( ir == 2 );
			#endif //_DEBUG
		}
		else
			ir = -1;//error
	}
	//else
	//{
	//	out.push_back( start );
	//	out.push_back( dest );
	//}

	ASSERT( !T_ISMINUS4(ir) );
	return ir;
};



#ifdef _DEBUG
// -----------------------------------------------------------------------------
VOID AgcuPathFind::bDbg_RenderPath(RwUInt32 colr)
{
	m_cAstar.bDbg_RenderPath(m_cMapGeoInfo, colr);
};

// -----------------------------------------------------------------------------
VOID AgcuPathFind::bDbg_RenderRect(RwUInt32 colr)
{
	m_cMapGeoInfo.bDbg_RenderHeightMap(colr);
};

// -----------------------------------------------------------------------------
VOID AgcuPathFind::bDbg_RenderLineTile(RwUInt32 colr)
{
	m_cMapGeoInfo.bDbg_RenderLineTile(colr);
};
// -----------------------------------------------------------------------------
VOID AgcuPathFind::bDbg_RenderColliObj(void)
{
	m_cMapGeoInfo.bDbg_RenderColliObj();
};
// -----------------------------------------------------------------------------
VOID AgcuPathFind::bDbg_RenderLineTileIntsct(void)
{
	m_cMapGeoInfo.bDbg_RenderLineTileIntsct();
};
#endif //_DEBUG
// -----------------------------------------------------------------------------
// AgcuPathFind.cpp - End of file
// -----------------------------------------------------------------------------
