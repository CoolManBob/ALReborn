// AgcmPathFinder.cpp
// -----------------------------------------------------------------------------
//                                _____        _   _     ______ _           _                                
//     /\                        |  __ \      | | | |   |  ____(_)         | |                               
//    /  \    __ _  ___ _ __ ___ | |__) | __ _| |_| |__ | |__   _ _ __   __| | ___ _ __      ___ _ __  _ __  
//   / /\ \  / _` |/ __| '_ ` _ \|  ___/ / _` | __| '_ \|  __| | | '_ \ / _` |/ _ \ '__|    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| | | | | | |    | (_| | |_| | | | |    | | | | | (_| |  __/ |    _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|_| |_| |_|_|     \__,_|\__|_| |_|_|    |_|_| |_|\__,_|\___|_|   (_) \___| .__/| .__/ 
//            __/ |                                                                             | |   | |    
//           |___/                                                                              |_|   |_|    
//
// path finding module
//
// -----------------------------------------------------------------------------
// Originally created on 08/12/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcmPathFinder.h"
#include "ApmObject.h"
#include "AgcmObject.h"

template< typename InItr >
class AcuCatmull_Rom
{
	typedef std::vector<RwV3d>			CONTAINER;
	typedef CONTAINER::const_iterator	C_ITR;

	CONTAINER	m_container;
	RwReal		m_fUstep;
public:
	AcuCatmull_Rom() : m_fUstep(0.f){};

	size_t	bSize()const { return m_container.size(); };
	C_ITR	bBegin()const { return m_container.begin(); };
	C_ITR	bEnd()const { return m_container.end(); };

	RwInt32 bCatmull_Rom(InItr first, InItr last, size_t samplingPerLine)
	{
		typedef typename std::iterator_traits<InItr>::difference_type	DIST;
		DIST dist = std::distance(first, last);
		if( samplingPerLine < 2 
		 || dist < 3 
		  )
			return -1;

		m_container.clear();
		m_container.reserve((dist-1) * samplingPerLine + 1);

		m_fUstep	= 1.f/static_cast<RwReal>(samplingPerLine);

		if( dist == 3 )
		{
			InItr	it_curr = first;
			const D3DXVECTOR3*	gp1	= reinterpret_cast<const D3DXVECTOR3*>(&(*(it_curr)));
			const D3DXVECTOR3*	gp2	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++it_curr)));
			const D3DXVECTOR3*	gp3	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++it_curr)));

			m_container.push_back( *reinterpret_cast<const RwV3d*>(gp1) );
			vCatmul_Rom( gp1, gp1, gp2, gp3, samplingPerLine );
		}
		else
		{
			InItr	it_curr = first;
			InItr	tmp		= it_curr;
			
			const D3DXVECTOR3*	gp1	= reinterpret_cast<const D3DXVECTOR3*>(&(*(tmp)));
			const D3DXVECTOR3*	gp2	= gp1;
			const D3DXVECTOR3*	gp3	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++tmp)));
			const D3DXVECTOR3*	gp4	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++tmp)));
			
			m_container.push_back( *reinterpret_cast<const RwV3d*>(gp1) );
			vCatmul_Rom( gp1, gp2, gp3, gp4, samplingPerLine );

			for( DIST i=1; i<dist-3; ++i )
			{
				tmp = it_curr;
				
				gp1	= reinterpret_cast<const D3DXVECTOR3*>(&(*(tmp)));
				gp2	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++tmp)));
				gp3	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++tmp)));
				gp4	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++tmp)));
				
				vCatmul_Rom( gp1, gp2, gp3, gp4, samplingPerLine );

				++it_curr;
			}

			
			gp1	= reinterpret_cast<const D3DXVECTOR3*>(&(*(it_curr)));
			gp2	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++it_curr)));
			gp3	= reinterpret_cast<const D3DXVECTOR3*>(&(*(++it_curr)));
			gp4	= gp3;

			vCatmul_Rom( gp1, gp2, gp3, gp4, samplingPerLine );
		}


		return 0;
	}

private:
	VOID	vCatmul_Rom(const D3DXVECTOR3* ctrlPoint1
		, const D3DXVECTOR3* ctrlPoint2
		, const D3DXVECTOR3* ctrlPoint3
		, const D3DXVECTOR3* ctrlPoint4
		, const size_t numPerLine)
	{
		RwReal	fu = m_fUstep;
		for( size_t i=1; i<numPerLine; ++i, fu += m_fUstep )
		{
			RwV3d	tmp = {0.f, 0.f, 0.f};
			D3DXVec3CatmullRom( reinterpret_cast<LPD3DXVECTOR3>(&tmp)
			, ctrlPoint1, ctrlPoint2, ctrlPoint3, ctrlPoint4, fu );

			m_container.push_back(tmp);
		}
		m_container.push_back( *reinterpret_cast<const RwV3d*>(ctrlPoint3) );
	}
};

// -----------------------------------------------------------------------------
AgcmPathFinder::AgcmPathFinder()
{
	SetModuleName("AgcmPathFinder");
	EnableIdle( TRUE );
}

// -----------------------------------------------------------------------------
AgcmPathFinder::~AgcmPathFinder()
{
}


// -----------------------------------------------------------------------------
BOOL AgcmPathFinder::OnAddModule()
{
	return TRUE;
};

// -----------------------------------------------------------------------------
BOOL AgcmPathFinder::OnInit()
{
	ApmMap*		pApmMap		= static_cast<ApmMap*>(GetModule("ApmMap"));
	AgcmMap*	pAgcmMap	= static_cast<AgcmMap*>(GetModule("AgcmMap"));
	ApmObject*	pApmObject	= static_cast<ApmObject*>(GetModule("ApmObject"));
	AgcmObject*	pAgcmObject	= static_cast<AgcmObject*>(GetModule("AgcmObject"));

	ASSERT( pApmMap && pAgcmMap && pApmObject && pAgcmObject );

	m_cPathFind.bSetModule( pApmMap, pAgcmMap, pApmObject, pAgcmObject );

	return TRUE;
};

// -----------------------------------------------------------------------------
BOOL AgcmPathFinder::OnDestroy()
{
	return TRUE;
};

// -----------------------------------------------------------------------------
BOOL AgcmPathFinder::OnIdle(UINT32 ulNowTime)
{
	return TRUE;
};

// -----------------------------------------------------------------------------
// 0 : go directly, 1 : with A*, 2 : can't find the path, 3 : keep in standing
RwInt32 AgcmPathFinder::bFindPath(const RwV3d&	start, const RwV3d& dest)
{
	const RwReal	MIN_MOVELENGTH	= 5.f;//5cm
	if( fabsf( start.x-dest.x ) < MIN_MOVELENGTH
	 && fabsf( start.z-dest.z ) < MIN_MOVELENGTH
	 )
	 return 3;

	m_waypoint.clear();

	m_vDest = dest;
	RwInt32 ir = m_cPathFind.bFindPath( start, dest, m_waypoint );

	const BOOL bUseCatmullRom	= FALSE;
	if( bUseCatmullRom )
	if( ir ==1 || ir == 2 )
		vIntpWayPoint();

	if( ir != 1 )
		m_waypoint.clear();

	return ir;
};

// -----------------------------------------------------------------------------
BOOL AgcmPathFinder::bPopWayPoint(RwV3d& thePos)
{
	if( m_waypoint.empty() )
		return FALSE;

	thePos = m_waypoint.back();
	m_waypoint.pop_back();
	return TRUE;
};

// -----------------------------------------------------------------------------
RwInt32 AgcmPathFinder::vIntpWayPoint(void)
{
	const size_t samplingPerLine = 5;

	typedef CONTAINER_WAYPOINT::const_iterator	CITR;
	AcuCatmull_Rom<CITR> tmp;

	RwInt32 ir = 
		tmp.bCatmull_Rom( m_waypoint.begin(), m_waypoint.end(), samplingPerLine );

	if( !ir )
	{
		m_waypoint.clear();
		m_waypoint.reserve( tmp.bSize() );
		std::back_insert_iterator<CONTAINER_WAYPOINT>	insrtItr(m_waypoint);
		std::copy( tmp.bBegin(), tmp.bEnd(), insrtItr );
	}

	return 0;
};

#ifdef _DEBUG
// -----------------------------------------------------------------------------
VOID AgcmPathFinder::bDbg_Render(RwInt32 colr)
{
	if( KEYDOWN( '1' ) )
	{
		m_cPathFind.bDbg_RenderPath(colr);
		bDbg_RenderWayPoint(colr);
	}
	if( KEYDOWN( '2' ) )
		m_cPathFind.bDbg_RenderRect(colr);
	if( KEYDOWN( '3' ) )
		m_cPathFind.bDbg_RenderLineTile(colr);

	if( KEYDOWN( '4' ) )
		m_cPathFind.bDbg_RenderColliObj();

	if( KEYDOWN( '5' ) )
		m_cPathFind.bDbg_RenderLineTileIntsct();
};
// -----------------------------------------------------------------------------
#include "AcuObjecWire.h"
class functrRenderWayPoint
{
public:
	RwInt32 operator() (const RwV3d& pos)
	{
		static RwSphere	sphere = { {0.f,0.f,0.f,}, 25.f };
		sphere.center = pos;

		return AcuObjecWire::bGetInst().bRenderSphere( sphere );
	};
};
VOID AgcmPathFinder::bDbg_RenderWayPoint(RwInt32 colr)
{
	colr;
	std::for_each( m_waypoint.begin(), m_waypoint.end(), functrRenderWayPoint() );
};
#endif //_DEBUG
// -----------------------------------------------------------------------------
// AgcmPathFinder.cpp - End of file
// -----------------------------------------------------------------------------
