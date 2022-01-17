// AgcuMapGeoInfo.cpp
// -----------------------------------------------------------------------------
//                            __  __              _____            _____        __                            
//     /\                    |  \/  |            / ____|          |_   _|      / _|                           
//    /  \    __ _  ___ _   _| \  / | __ _ _ __ | |  __  ___  ___   | |  _ __ | |_  ___       ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | | |\/| |/ _` | '_ \| | |_ |/ _ \/ _ \  | | | '_ \|  _|/ _ \     / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | |  | | (_| | |_) | |__| |  __/ (_) |_| |_| | | | | | (_) | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|_|  |_|\__,_| .__/ \_____|\___|\___/|_____|_| |_|_|  \___/ (_) \___| .__/| .__/ 
//            __/ |                       | |                                                    | |   | |    
//           |___/                        |_|                                                    |_|   |_|    
//
// terrain's geometry
//
// -----------------------------------------------------------------------------
// Originally created on 07/20/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcuMapGeoInfo.h"
#include "AcuMathFunc.h"
USING_ACUMATH



#ifdef _DEBUG
#include "AgcuEffUtil.h"
#include "AcuObjecWire.h"
struct stObjColl
{
	RpAtomic*	m_pAtomic;
	RwLine		m_line;
	RwUInt32	m_colr;

public:
	stObjColl(RpAtomic* pAtomic, const RwLine& line, RwUInt32 colr)
		: m_pAtomic(pAtomic), m_colr(colr)
	{
		m_line = line;
	};

	void Render()
	{
		AcuObjecWire::bGetInst().bRenderAtomic(m_pAtomic, m_colr);
		
		AcuIMDraw::RenderLine( m_line.start, m_line.end, 0xffffff00 );
	}
};
struct functRenderObjColli
{
	void operator () (stObjColl& obj)
	{
		obj.Render();
	}
};
std::list<stObjColl>	g_containerDbgObjColli;

struct stVtxLine
{
	RwV3d	m_linestart, m_lineend;

	typedef std::vector<RwV3d>	CONTAINER;
	CONTAINER	m_container;

	void clear()
	{
		m_container.clear();
	}

	void add(const RwV3d& linestart, const RwV3d& lineend, const RwV3d* arr, RwInt32 num)
	{
		m_linestart = linestart;
		m_lineend = lineend;

		std::copy( arr, arr+num, std::back_insert_iterator<CONTAINER>(m_container) );
	}

	void render()
	{
		//. 2006. 3. 2. Nonstopdj
		if(m_container.size())
		{
			AcuIMDraw::RenderLine( m_linestart, m_lineend, 0xffff0000 );
			AcuIMDraw::RenderLineStrip( &m_container.front(), m_container.size(), 0xffffff00 );
		}
	}
}g_DbgLineTileIntsct;

#endif //_DEBUG

// -----------------------------------------------------------------------------
// AgcuMapGeoInfo
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
AgcuMapGeoInfo::AgcuMapGeoInfo
(ApmMap* pApmMap, AgcmMap* pAgcmMap, ApmObject* pApmObject, const RwReal tilesize)
: m_pApmMap(pApmMap)
, m_pAgcmMap(pAgcmMap)
, m_pApmObject(pApmObject)
, m_tilesize(tilesize)
, m_ridableheight(100.f)
, m_width(0)
, m_height(0)
{
	m_ptStart.x	= 
	m_ptStart.y	= 0;
	m_ptDest = m_ptStart;
	m_ptLeftBottom = m_ptStart;

	m_vLeftBottom.x = 
	m_vLeftBottom.y = 
	m_vLeftBottom.z = 0.f;

	m_fptrGetLinesForChk[e_l2r	]	= &AgcuMapGeoInfo::vGetLinesForChk_l2r;
	m_fptrGetLinesForChk[e_r2l	]	= &AgcuMapGeoInfo::vGetLinesForChk_r2l;
	m_fptrGetLinesForChk[e_t2b	]	= &AgcuMapGeoInfo::vGetLinesForChk_t2b;
	m_fptrGetLinesForChk[e_b2t	]	= &AgcuMapGeoInfo::vGetLinesForChk_b2t;
	m_fptrGetLinesForChk[e_lt2rb]	= &AgcuMapGeoInfo::vGetLinesForChk_lt2rb;
	m_fptrGetLinesForChk[e_rb2lt]	= &AgcuMapGeoInfo::vGetLinesForChk_rb2lt;
	m_fptrGetLinesForChk[e_rt2lb]	= &AgcuMapGeoInfo::vGetLinesForChk_rt2lb;
	m_fptrGetLinesForChk[e_lb2rt]	= &AgcuMapGeoInfo::vGetLinesForChk_lb2rt;
	
	m_fptrGetLinesForChk[e_justpoint]	= NULL; //이넘을 타는건 없어야 할것!
}

// -----------------------------------------------------------------------------
AgcuMapGeoInfo::~AgcuMapGeoInfo()
{
}

// -----------------------------------------------------------------------------
// 필요한 모듈을 얻음
VOID AgcuMapGeoInfo::bSetModule
(ApmMap* pApmMap, AgcmMap* pAgcmMap, ApmObject* pApmObject, AgcmObject* pAgcmObject)
{
	ASSERT( pApmMap && pAgcmMap && pApmObject && pAgcmObject );
	m_pApmMap = pApmMap;
	m_pAgcmMap = pAgcmMap;
	m_pApmObject = pApmObject;
	m_pAgcmObject = pAgcmObject;
}

// -----------------------------------------------------------------------------
// return value : 0( in rect ), 1( out of rect so truncat dest )
RwInt32 AgcuMapGeoInfo::bSetRect( const RwV3d& pt1, const RwV3d& pt2
								, const RwInt32 TILELIMIT, const RwInt32 TILEOFFSET )
{
	RwInt32 ir	= 0;

	RwInt32	x1	= (RwInt32)( (pt1.x - ((pt1.x<0.f) ? m_tilesize : 0.f) ) / m_tilesize );
	RwInt32 z1	= (RwInt32)( (pt1.z - ((pt1.z<0.f) ? m_tilesize : 0.f) ) / m_tilesize );
	RwInt32	x2	= (RwInt32)( (pt2.x - ((pt2.x<0.f) ? m_tilesize : 0.f) ) / m_tilesize );
	RwInt32 z2	= (RwInt32)( (pt2.z - ((pt2.z<0.f) ? m_tilesize : 0.f) ) / m_tilesize );

	//@{ kday 20050929
	// ;)
	// 허용 범위가 넘으면 검사안함..
	if( abs(x1-x2) > TILELIMIT + TILEOFFSET * 2 
		|| abs(z1-z2) > TILELIMIT + TILEOFFSET * 2 
		)
		return 1; //out of limit
	//@} kday
	
	m_vStart	= pt1;
	m_vDest		= pt2;

	m_vDir_S2D_ProjToXZ.x =  pt2.x - pt1.x;
	m_vDir_S2D_ProjToXZ.y =  pt2.z - pt1.z;
	RwV2dNormalize (&m_vDir_S2D_ProjToXZ, &m_vDir_S2D_ProjToXZ);

	m_ptStart.x	= x1;
	m_ptStart.y	= z1;
	m_ptDest.x	= x2;
	m_ptDest.y	= z2;

	if( x2 < x1 )
		SWAP(x1, x2);
	if( z2 < z1 )
		SWAP(z1, z2);

	//max chk
	RwInt32 checkx	= 0;
	RwInt32 checkz	= 0;
	RwInt32 dx	= x2 - x1;
	RwInt32 dz	= z2 - z1;
	POINT	ptLeftBottom	= {0, 0};
	{
		if( dx < TILELIMIT )
		{
			checkx = 0;	//statx-tileoffset, destx+tileoffset
			m_width = dx + 2*TILEOFFSET;
			ptLeftBottom.x = x1 - TILEOFFSET;
		}
		else if( dx <= TILELIMIT + TILEOFFSET * 2 )
		{
			checkx = 1;	//order of x is (TILELIMIT + TILEOFFSET * 2)
			m_width = TILELIMIT + TILEOFFSET * 2;
			RwInt32 tmpOffset = m_width - dx;
			if( T_ISODD(tmpOffset) )
				ptLeftBottom.x = x1 - tmpOffset/2 -1;
			else
				ptLeftBottom.x = x1 - tmpOffset/2;
		}
		else
		{
			ASSERT( !"허용범위가 넘는경우는 위에서 리턴했으므로 이리 안탄다" );
			checkx = 2;	//order of x is (TILELIMIT + TILEOFFSET * 2) and destx is truncated
			m_width = TILELIMIT + TILEOFFSET * 2;
			ptLeftBottom.x = x1;
		}

		if( dz < TILELIMIT )
		{
			checkz = 0;	//statz-tileoffset, destz+tileoffset
			m_height = dz + 2*TILEOFFSET;
			ptLeftBottom.y = z1 - TILEOFFSET;
		}
		else if( dz <= TILELIMIT + TILEOFFSET * 2 )
		{
			checkz = 1;	//order of z is (TILELIMIT + TILEOFFSET * 2)
			m_height = TILELIMIT + TILEOFFSET * 2;
			RwInt32 tmpOffset = m_height - dz;
			if( T_ISODD(tmpOffset) )
				ptLeftBottom.y = z1 - tmpOffset/2 -1;
			else
				ptLeftBottom.y = z1 - tmpOffset/2;
		}
		else
		{
			ASSERT( !"허용범위가 넘는경우는 위에서 리턴했으므로 이리 안탄다" );
			checkz = 2;	//order of z is (TILELIMIT + TILEOFFSET * 2) and destx is truncated
			m_height = TILELIMIT + TILEOFFSET * 2;
			ptLeftBottom.y = z1;
		}


		if( checkx == 2 || checkz == 2 )
		{
			ASSERT( !"허용범위가 넘는경우는 위에서 리턴했으므로 이리 안탄다" );
			vClampDest( dx, dz, TILELIMIT, TILEOFFSET );
			// TODO : ptLeftBottom 이 갱신되어야 한다.
			ir = 1;		//return value should depict truncated dest
		}

	}


	//relative to leftbottom
	m_ptStart.x	-= ptLeftBottom.x;
	m_ptStart.y	-= ptLeftBottom.y;
	m_ptDest.x	-= ptLeftBottom.x;
	m_ptDest.y	-= ptLeftBottom.y;
	m_ptLeftBottom = ptLeftBottom;
	
	m_vLeftBottom.x	= ((RwReal)ptLeftBottom.x)*m_tilesize;
	m_vLeftBottom.z	= ((RwReal)ptLeftBottom.y)*m_tilesize;


	ASSERT( (m_ptStart.x>=0 && m_ptStart.x<=m_width) && (m_ptDest.x>=0 && m_ptDest.x<=m_width) );
	ASSERT( (m_ptStart.y>=0 && m_ptStart.y<=m_height) && (m_ptDest.y>=0 && m_ptDest.y<=m_height) );

	m_heightmap.bSetOrder( m_height+1, m_width+1 );

	vBuildHeightMap();

	m_vLeftBottom.y = m_heightmap(0, 0);

	if( T_ISMINUS4(vBuildLineTile()) )
		return 1;//한타일 범위는 검사 안한다 ㅡㅡ


#ifdef _DEBUG
	g_containerDbgObjColli.clear();
	g_DbgLineTileIntsct.clear();
#endif //_DEBUG

	return ir;
};

// -----------------------------------------------------------------------------
// 목적지가 허용 범위를 벗어났을때 호출되며 목적지점을 clamping 한다.
void AgcuMapGeoInfo::vClampDest( RwInt32 dx, RwInt32 dz
							   , const RwInt32 TILELIMIT, const RwInt32 TILEOFFSET)
{
	if( dx > TILELIMIT + TILEOFFSET * 2 )
	{		
		dx -= TILELIMIT + TILEOFFSET * 2;
		if( m_ptStart.x < m_ptDest.x )
		{
			m_ptDest.x -= dx;
			m_vDest.x	= static_cast<RwReal>(m_ptDest.x+1)*m_tilesize;
		}
		else
		{
			ASSERT( m_ptStart.x != m_ptDest.x );
			m_ptDest.x += dx;
			m_vDest.x	= static_cast<RwReal>(m_ptDest.x)*m_tilesize;
		}
	}

	if( dz > TILELIMIT + TILEOFFSET * 2 )
	{
		dz -= TILELIMIT + TILEOFFSET * 2;
		if( m_ptStart.y < m_ptDest.y )
		{
			m_ptDest.y -= dz;
			m_vDest.z	= static_cast<RwReal>(m_ptDest.y+1)*m_tilesize;
		}
		else
		{
			ASSERT( m_ptStart.y != m_ptDest.y );
			m_ptDest.y += dz;
			m_vDest.z	= static_cast<RwReal>(m_ptDest.x)*m_tilesize;
		}
	}
};

// -----------------------------------------------------------------------------
// 출발지에서 목적지로 향하는 레이(XZ평면에투영된)와 타일과의 원충돌검사
bool AgcuMapGeoInfo::vSphereCheck(const RwV3d& tileCenter)
{
	const RwV2d	tmp	= { tileCenter.x - m_vLeftBottom.x
					  , tileCenter.z - m_vLeftBottom.z	};
	const RwReal Radius	= sqrtf( (m_tilesize*0.5f)*(m_tilesize*0.5f)
							   + (m_tilesize*0.5f)*(m_tilesize*0.5f) );

	return ( fabsf(V2dCrossProduct( &tmp, &m_vDir_S2D_ProjToXZ )) < Radius );
};

// -----------------------------------------------------------------------------
// 영역안의 모든 하이트 정보를 배열로 저장
void AgcuMapGeoInfo::vBuildHeightMap(void)
{
	RwReal	x	= m_vLeftBottom.x;
	RwReal	z	= m_vLeftBottom.z;
	for( int i=0; i<m_height+1; ++i )
	{
		x = m_vLeftBottom.x;
		for( int j=0; j<m_width+1; ++j )
		{
			m_heightmap(i,j) = 
				m_pAgcmMap->HP_GetHeightGeometryOnly(x, z);

			x += m_tilesize;
		}
		z += m_tilesize;
	}
};

// -----------------------------------------------------------------------------
// 출발타일과 목적지 타일을 잇는 타일단위 라인구성
RwInt32 AgcuMapGeoInfo::vBuildLineTile(void)
{
	m_lineTile.clear();

	eLineSlopeType slope = vSlopeType( m_ptStart, m_ptDest );

	switch( slope )
	{
	case e_l2r://start.x to dest.x
		{
			for( POINT pt = m_ptStart; pt.x <= m_ptDest.x; ++pt.x )
				m_lineTile.push_back( pt );
		}break;
	case e_r2l://start.x to dest.x
		{
			for( POINT pt = m_ptStart; pt.x >= m_ptDest.x; --pt.x )
				m_lineTile.push_back( pt );
		}break;
	case e_t2b://start.y to dest.y
		{
			for( POINT pt = m_ptStart; pt.y >= m_ptDest.y; --pt.y )
				m_lineTile.push_back( pt );
		}break;
	case e_b2t://start.y to dest.y
		{
			for( POINT pt = m_ptStart; pt.y <= m_ptDest.y; ++pt.y )
				m_lineTile.push_back( pt );
		}break;


	case e_lt2rb:
	case e_rb2lt:
	case e_rt2lb:
	case e_lb2rt:
		{
			RwInt32 dx = m_ptDest.x - m_ptStart.x;
			RwInt32 dy = m_ptDest.y - m_ptStart.y;

			ASSERT( dx && dy );


			// TODO : ( abs(dx) < abs(dy) ) 조건검사로 계산량을 줄일것.
			//if( abs(dx) < abs(dy) )
			{
				// y = ax + b : y == z, x == x, a = dy/dx, b = y0-(a*x0)
				RwReal	a = (m_vDest.z - m_vStart.z)/(m_vDest.x - m_vStart.x);
				RwReal	b = m_vStart.z - a*m_vStart.x;

				RwReal	x = static_cast<RwReal>(m_ptStart.x + ((dx > 0) ? 1 : 0)) * m_tilesize + m_vLeftBottom.x;
				RwReal	y = 0.f;

				RwInt32 xStep = (dx > 0) ? 1 : -1;
				POINT ptLast = m_ptStart;
				POINT pt = m_ptStart;
				for( RwInt32 i=0; i<=abs(dx); ++i )
				{
					y = a*x + b;

					if( i == abs(dx) )
						pt = m_ptDest;
					else
					{
						pt.y = static_cast<RwInt32>( (y - ((y<0.f) ? m_tilesize : 0.f )) / m_tilesize );
						pt.y -= m_ptLeftBottom.y;
					}

					RwInt32 num = pt.y - ptLast.y;
					RwInt32 yStep = (num>=0) ? 1 : -1;
					for( RwInt32 k=0; k<=abs(num); ++k )
					{
						ASSERT( ptLast.x >= 0 && ptLast.x < m_width );
						ASSERT( ptLast.y >= 0 && ptLast.y < m_height );
						m_lineTile.push_back(ptLast);
						ptLast.y += yStep;
					}						

					pt.x += xStep;
					ptLast = pt;

					if( T_ISMINUS4(dx) )
						x-=m_tilesize;
					else
						x+=m_tilesize;
				}
			}
			//else
			//{
			//	// y = ax + b : y == x, x == z, a = dy/dx, b = y0-(a*x0)
			//	RwReal	a = (m_vDest.x - m_vStart.x)/(m_vDest.z - m_vStart.z);
			//	RwReal	b = m_vStart.x - a*m_vStart.z;
			//	
			//	RwReal	x = static_cast<RwReal>(m_ptStart.y) * m_tilesize + m_vLeftBottom.y;
			//	RwReal	y = 0.f;

			//	POINT ptLast = m_ptStart;
			//	POINT pt = {0,0};
			//	RwInt32 yStep = (dy > 0) ? 1 : -1;
			//	dy = abs(dy);
			//	for( int i=0; i<dy; ++i )
			//	{
			//		y = a*x + b;
			//		pt.y = static_cast<RwInt32>( (y - ((y<0.f) ? m_tilesize : 0.f )) / m_tilesize );
			//		pt.y -= m_ptLeftBottom.y;

			//		RwInt32 num = pt.x - ptLast.x;
			//		RwInt32 xStep = (num>=0) ? 1 : -1;
			//		for( RwInt32 k=0; k<=abs(num); ++k )
			//		{
			//			m_lineTile.push_back(ptLast);
			//			ptLast.x += xStep;
			//		}						

			//		pt.z += yStep;
			//		ptLast = pt;

			//		x+=m_tilesize;
			//	}
			//}

		}break;

	case e_justpoint:
		m_lineTile.push_back( m_ptStart );
		return -1;
		break;

	default:
		ASSERT( !"INVALID" );
		return -1;
	}

	return 0;
};

// -----------------------------------------------------------------------------
// block ? TRUE : FALSE
class functrGeoBlckTst
{
	const AgcuMapGeoInfo&	_rfAgcuMapGeoInfo;
	ApmMap*					_pApmMap;
public:
	functrGeoBlckTst(const AgcuMapGeoInfo& rfAgcuMapGeoInfo
					, ApmMap* pApmMap )
					: _rfAgcuMapGeoInfo(rfAgcuMapGeoInfo)
					, _pApmMap(pApmMap)
	{};

	bool operator() (const POINT& pt)const
	{
		RwV3d tileCenter = {0.f,0.f,0.f};
		_rfAgcuMapGeoInfo.bGetTileCenter( tileCenter, pt.x, pt.y );
		if(_pApmMap->CheckBlockingInfo(*(AuPOS*)(&tileCenter) , ApmMap::GROUND ) & ApmMap::GEOMETRY_BLOCKING )
			return true;
		return false;
	}
};
BOOL AgcuMapGeoInfo::bLineBlock_Tile(void)
{
	LINETILE_CITR	it_f = 
	std::find_if( m_lineTile.begin()
				, m_lineTile.end()
				, functrGeoBlckTst(*this, m_pApmMap)
				);
	if( it_f != m_lineTile.end() )
		return TRUE;

	return FALSE;
};

// -----------------------------------------------------------------------------
BOOL AgcuMapGeoInfo::bLineBlock_Obj(void)
{	
	RpIntersection	intsct;
	memset( &intsct, 0, sizeof(intsct) );
	intsct.type	= rpINTERSECTLINE;
	intsct.t.line.start	= m_vStart;
	intsct.t.line.start.y += m_ridableheight;

	RwInt32	retchk		= 0;
	std::list<RwInt32>	colliObjIDContainer;

	
	RwReal	lastHeight	= intsct.t.line.start.y;

	LINETILE_CITR	it_curr = m_lineTile.begin();
	LINETILE_CITR	it_last = m_lineTile.end();

	//@{ kday 20051003
	// ;)
	// added case : there is only one tile.
	if( std::distance( it_curr, it_last ) == 1 )
	{
		intsct.t.line.end	= m_vDest;
		intsct.t.line.end.y += m_ridableheight;
		vGetColliObjIDListInTile( (*it_curr), colliObjIDContainer );
		if( vChckColliObj( intsct, colliObjIDContainer, 6 ) )
			return TRUE;	
	}
	else
	//@} kday
	{
		LINETILE_CITR it_tmp = it_last;
		for( --it_tmp; it_curr != it_last; ++it_curr )
		{
			//1. 해당타일에 포함된 오브젝트 리스트를 얻어온다
			//2. 해당 타일에 해당되는 라인을 구성한다
				//2-1. 가기 기울기에 따른 교차판정할 라인을 얻어온다
				//2-2. 교차판정에서 나온점과 이전 에 기억한 점을 참조한(높이값만조정) 라인과 오브젝트와 교차판정
			colliObjIDContainer.clear();

			//1.
			vGetColliObjIDListInTile( (*it_curr), colliObjIDContainer );		
			if ( colliObjIDContainer.empty() )
				continue;
			//2-1.
			if( it_tmp == it_curr )
			{
				intsct.t.line.end = m_vDest;
			}
			else
			{
				// TODO : consider slant - vGetIntsctBetweenProjRayAndTile's output may be array of intsct
				retchk = vGetIntsctBetweenProjRayAndTile( intsct.t.line.end, (*it_curr), lastHeight );
				if( T_ISMINUS4(retchk) )
				{
					ASSERT( !"vGetIntsctBetweenProjRayAndTile() returns minus!" );
					continue;
				}
			}

			//2-2.
			//@{ kday 20051003
			// ;)
			//intsct.t.line.end.y += m_ridableheight;
			//lastHeight	= intsct.t.line.end.y;
			//if( vChckColliObj( intsct, colliObjIDContainer ) )
			//	return TRUE;

			//@{ kday 20051009
			// ;)
			if( vChckColliObj( intsct, colliObjIDContainer, 6 ) )
				return TRUE;
			//if( vChckColliObj_Modified( intsct, colliObjIDContainer ) )
			//	return TRUE;
			//@} kday


			lastHeight	= intsct.t.line.end.y;
			//@} kday


			intsct.t.line.start = intsct.t.line.end;
		}
	}

	return FALSE;
};

// -----------------------------------------------------------------------------
void AgcuMapGeoInfo::bGetTileCenter(RwV3d& center, RwInt32 x, RwInt32 z, bool bHeight, RwReal beforeHeight)const
{
	ASSERT( x>=0 && x<=m_width && z>=0 && z<=m_height );
	
	center.x = m_vLeftBottom.x + ((RwReal)x)*m_tilesize + m_tilesize*0.5f;
	center.z = m_vLeftBottom.z + ((RwReal)z)*m_tilesize + m_tilesize*0.5f;

	if(bHeight)
	{
		if( vGetRidableObjIDListInTile(center.x, center.z) )
		{
			ASSERT( beforeHeight != 0.f );
			center.y = m_pAgcmMap->GetHeight( center.x, center.z, beforeHeight );
		}
		else
			center.y = m_pAgcmMap->HP_GetHeightGeometryOnly(center.x, center.z);

	}
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::bTileChk
(const POINT& pt, const RwV3d* pV3dFrom, RwReal* tileCenterHeight)const
{
	RwV3d	center = {0.f, 0.f, 0.f};

	if( pt.x < 0 || pt.x >= m_width 
	 || pt.y < 0 || pt.y >= m_height
	 )
		return 1;

	bGetTileCenter( center, pt.x, pt.y );
	if( m_pApmMap->CheckBlockingInfo( *(reinterpret_cast<AuPOS*>(&center)) , ApmMap::GROUND ) 
	  & ApmMap::GEOMETRY_BLOCKING 
	  )
		return 1;

	if( pV3dFrom )
	{
		
		RpIntersection	intsct;
		memset( &intsct, 0, sizeof(intsct) );
		intsct.type	= rpINTERSECTLINE;
		intsct.t.line.start	= *pV3dFrom;
		intsct.t.line.start.y += m_ridableheight;

		if( pt.x == m_ptDest.x && pt.y == m_ptDest.y )
			intsct.t.line.end = m_vDest;
		else
			bGetTileCenter(intsct.t.line.end, pt.x, pt.y, true, pV3dFrom->y );

		if( tileCenterHeight )
			*tileCenterHeight = intsct.t.line.end.y;
		intsct.t.line.end.y += m_ridableheight;

		std::list<RwInt32>	objIDContainer;

		//1. get object list
		vGetColliObjIDListInTile( pV3dFrom->x, pV3dFrom->z, objIDContainer );
		vGetColliObjIDListInTile( pt, objIDContainer );

		//2. collision check
		//@{ kday 20051003
		// ;)
		//if( vChckColliObj( intsct, objIDContainer ) )
		//	return 1;

		//@{ kday 20051009
		// ;)
		if( vChckColliObj( intsct, objIDContainer, 6 ) )
			return 1;
		//if( vChckColliObj_Modified( intsct, objIDContainer ) )
		//	return 1;
		//@} kday
		
		//@} kday
	}

	return 0;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::bTileChk
(RwInt32 x, RwInt32 z, const RwV3d* pV3dFrom, RwReal* tileCenterHeight)const
{
	const POINT pt = {x, z};
	return bTileChk(pt, pV3dFrom, tileCenterHeight);
};

// -----------------------------------------------------------------------------
AgcuMapGeoInfo::eLineSlopeType 
AgcuMapGeoInfo::vSlopeType(const POINT& ptStart, const POINT& ptEnd)const
{
	eLineSlopeType er = e_slopeNum;

	RwInt32 dx	= ptEnd.x - ptStart.x;
	RwInt32 dy	= ptEnd.y - ptStart.y;

	if(dy==0){
		if( dx > 0 )
			er = e_l2r;
		else if( dx < 0 )
			er = e_r2l;
		else
			er = e_justpoint;
	}
	else if(dx==0)
		er = (dy<0) ? e_t2b : e_b2t;
	else if(dy>0 )
		er = (dx>0) ? e_lt2rb : e_rt2lb;
	else 
		er = (dx>0) ? e_lb2rt : e_rb2lt;
	
	return er;
};
// -----------------------------------------------------------------------------
AgcuMapGeoInfo::eLineSlopeType
AgcuMapGeoInfo::vSlopeType(const RwV3d& ptStart, const RwV3d& ptEnd)const
{
	eLineSlopeType er = e_slopeNum;

	RwReal dx	= ptEnd.x - ptStart.x;
	RwReal dy	= ptEnd.z - ptStart.z;
	ASSERT( !( dx == 0.f && dy == 0.f ) );

	if(dy==0.f){
		if( dx > 0.f )
			er = e_l2r;
		else if( dx < 0.f )
			er = e_r2l;
		else
			er = e_justpoint;
	}
	else if(dx==0.f)
		er = (dy<0.f) ? e_t2b : e_b2t;
	else if(dy>0.f )
		er = (dx>0.f) ? e_lt2rb : e_rt2lb;
	else 
		er = (dx>0.f) ? e_lb2rt : e_rb2lt;

	return er;
};

// -----------------------------------------------------------------------------
// 유효한 타일인덱스인가?
BOOL AgcuMapGeoInfo::vIsAvailableTileIndex(const POINT& ptTileIndex)const
{
	const RECT rc = {0,0,m_width,m_height};
	return PtInRect(&rc, ptTileIndex);
};

// -----------------------------------------------------------------------------
// 유효한 타일인덱스인가?
BOOL AgcuMapGeoInfo::vIsAvailableTileIndex(RwInt32 x, RwInt32 z)const
{
	const POINT ptTileIndex = {x,z};
	return vIsAvailableTileIndex(ptTileIndex);
};

// -----------------------------------------------------------------------------
// 해당 타일에 포함된 컬리전오브젝트아이디를 얻어온다
RwInt32 AgcuMapGeoInfo::vGetColliObjIDListInTile
(const POINT& ptTile, std::list<RwInt32>& idList)const
{
	if( !vIsAvailableTileIndex(ptTile) )
		return -1;//overrange

	RwV3d tileCenter = {0.f, 0.f, 0.f};
	bGetTileCenter(tileCenter, ptTile.x, ptTile.y);

	return vGetColliObjIDListInTile(tileCenter.x, tileCenter.z, idList);
};

// -----------------------------------------------------------------------------
// 해당 타일에 포함된 컬리전오브젝트아이디를 얻어온다
RwInt32 AgcuMapGeoInfo::vGetColliObjIDListInTile
(const RwReal xInWorld, const RwReal zInWorld, std::list<RwInt32>& idList)const
{
	POINT ptIndexInSector = {0, 0};
	ApWorldSector * pSector = m_pApmMap->GetSector( xInWorld, zInWorld );
	if( pSector )
	{
		pSector->D_GetSegment(xInWorld, zInWorld, (RwInt32*)(&(ptIndexInSector.x)), (RwInt32*)(&(ptIndexInSector.y)) );
			
		RwInt32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ] =  {0, };
		RwInt32 numOfObj = pSector->GetObjectCollisionID( ptIndexInSector.x, ptIndexInSector.y, aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT);

		for( int i=0; i<numOfObj; ++i )
			idList.push_back( aObjectList[i] );	
	}

	return 0;
};

// -----------------------------------------------------------------------------
// 해당 타일에 포함된 컬리전오브젝트아이디를 얻어온다
RwInt32 AgcuMapGeoInfo::vGetRidableObjIDListInTile
(const POINT& ptTile, std::list<RwInt32>* idList)const
{
	if( !vIsAvailableTileIndex(ptTile) )
		return -1;//overrange

	RwV3d tileCenter = {0.f, 0.f, 0.f};
	bGetTileCenter(tileCenter, ptTile.x, ptTile.y);

	return vGetRidableObjIDListInTile(tileCenter.x, tileCenter.z, idList);
};

// -----------------------------------------------------------------------------
// 해당 타일에 포함된 컬리전오브젝트아이디를 얻어온다
RwInt32 AgcuMapGeoInfo::vGetRidableObjIDListInTile
(RwReal xInWorld, RwReal zInWorld, std::list<RwInt32>* idList)const
{
	POINT ptIndexInSector = {0, 0};
	ApWorldSector * pSector = m_pApmMap->GetSector( xInWorld, zInWorld );
	if( pSector )
	{
		pSector->D_GetSegment(xInWorld, zInWorld, (RwInt32*)(&(ptIndexInSector.x)), (RwInt32*)(&(ptIndexInSector.y)) );
			
		RwInt32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ] =  {0, };
		RwInt32 numOfObj = pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT, ptIndexInSector.x, ptIndexInSector.y, aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );
		ASSERT( numOfObj <= SECTOR_MAX_COLLISION_OBJECT_COUNT );

		if( idList )
		{
			for( int i=0; i<numOfObj; ++i )
				idList->push_back( aObjectList[i] );	
		}

		return numOfObj;
	}

	return 0;
};

// -----------------------------------------------------------------------------
// 교차점을 구한다.
RwInt32 AgcuMapGeoInfo::vGetIntsctBetweenProjRayAndTile
(RwV3d& intsct, const POINT& ptTile, const RwReal lastHeight, BOOL bOnlyXZAvaiable)
{
	if( !vIsAvailableTileIndex(ptTile) )
		return -1;//overrange

	
	const RwV2d	thelineStart= {m_vStart.x,m_vStart.z};
	const RwV2d	thelineEnd	= {m_vDest.x,m_vDest.z};

	RwV3d			arrPoint[3] = { {0.f,0.f,0.f,}, };
	const eLineSlopeType	Slope = vSlopeType(m_vStart, m_vDest);
	const RwInt32			numpoint = (this->*m_fptrGetLinesForChk[Slope])(arrPoint, ptTile);
	

	ASSERT( numpoint <= 3 );

	RwV2d	v2Point1 = {0.f,0.f};
	RwV2d	v2Point2 = {0.f,0.f};
	RwV2d	v2Intsct = {0.f,0.f};
	
#ifdef _DEBUG
	g_DbgLineTileIntsct.add(m_vStart, m_vDest, arrPoint, numpoint);
#endif //_DEBUG

	for( int i=0; i<numpoint; ++i )
	{
		v2Point1.x = arrPoint[i].x;
		v2Point1.y = arrPoint[i].z;
		v2Point2.x = arrPoint[i+1].x;
		v2Point2.y = arrPoint[i+1].z;

		if( !Intersect2DLine( v2Intsct, thelineStart, thelineEnd, v2Point1, v2Point2, true ) )
		{
			intsct.x = v2Intsct.x;
			intsct.z = v2Intsct.y;

			if( !bOnlyXZAvaiable )
			{
				D3DXVECTOR2 p1ToP2(v2Point2.x-v2Point1.x, v2Point2.y-v2Point1.y);
				D3DXVECTOR2 p1ToIntsct(v2Intsct.x-v2Point1.x, v2Intsct.y-v2Point1.y);
				
				if( vGetRidableObjIDListInTile( ptTile ) )
				{
					intsct.y = m_pAgcmMap->GetHeight(intsct.x, intsct.z, lastHeight );
				}
				else
				{
					LinearIntp( &intsct.y
						, &arrPoint[i].y
						, &arrPoint[i+1].y
						, D3DXVec2LengthSq(&p1ToIntsct)/D3DXVec2LengthSq( &p1ToP2 ) );
				}
			}
			return 0;
		}
	}


	return -1;//failed
};

// -----------------------------------------------------------------------------
// 오브젝트와 충돌 체크
class functrObjCollisionChk
{
public:
	struct stClollisionRet
	{
		BOOL	m_bCollision;
		RwReal	m_fDistance;
		RwV3d	m_arrVertex[3];

		stClollisionRet() 
			: m_bCollision(FALSE)
			, m_fDistance(0.f)
		{
			const RwV3d vzero = {0.f,0.f,0.f};
			RwV3dAssign( m_arrVertex+0, &vzero );
			RwV3dAssign( m_arrVertex+1, &vzero );
			RwV3dAssign( m_arrVertex+2, &vzero );
		}
	};

private:
	ApmObject*		pApmObject_;
	AgcmObject*		pAgcmObject_;
	RpIntersection*	pRpIntsct_;
	stClollisionRet	ret;

public:
	functrObjCollisionChk
		( ApmObject* pApmObject, AgcmObject* pAgcmObject, RpIntersection* pRpIntsct )
		: pApmObject_(pApmObject)
		, pAgcmObject_(pAgcmObject)
		, pRpIntsct_(pRpIntsct){};

	static RpCollisionTriangle* 
	CBIntsct( RpIntersection *intersection
			, RpCollisionTriangle *collTriangle
			, RwReal distance
			, void *pRet)
	{
		collTriangle;

		stClollisionRet* theRet = static_cast<stClollisionRet*>(pRet);


		if( theRet->m_bCollision )
		{
			if( theRet->m_fDistance > distance )
			{
				theRet->m_fDistance  = distance;			
				RwV3dAssign( theRet->m_arrVertex+0, collTriangle->vertices[0] );
				RwV3dAssign( theRet->m_arrVertex+1, collTriangle->vertices[1] );
				RwV3dAssign( theRet->m_arrVertex+2, collTriangle->vertices[2] );
			}
		}
		else
		{
			theRet->m_bCollision = TRUE;
			theRet->m_fDistance  = distance;			
			RwV3dAssign( theRet->m_arrVertex+0, collTriangle->vertices[0] );
			RwV3dAssign( theRet->m_arrVertex+1, collTriangle->vertices[1] );
			RwV3dAssign( theRet->m_arrVertex+2, collTriangle->vertices[2] );
		}

		return NULL;
	}
	bool operator() (RwInt32 objID)
	{
		if( !pApmObject_ || !pAgcmObject_ || !pRpIntsct_ )
			return false;

		ApdObject* pApdObject = pApmObject_->GetObject(objID);
		AgcdObject*	pAgcdObject = pAgcmObject_->GetObjectData( pApdObject );
		if( pApdObject 
		 && pAgcdObject 
		 && pAgcdObject->m_pstCollisionAtomic
		 && pAgcdObject->m_pstCollisionAtomic->geometry
		 && pAgcdObject->m_pstCollisionAtomic->geometry->triangles )
		{
			RpAtomicForAllIntersections( 
				  pAgcdObject->m_pstCollisionAtomic
				, pRpIntsct_
				, CBIntsct
				, static_cast<PVOID>(&ret)
				);

#ifdef _DEBUG
			stObjColl	tmp( pAgcdObject->m_pstCollisionAtomic
				, pRpIntsct_->t.line
				, ret.m_bCollision ? 0xFFFF0000 : 0xFFFFFFFF );
			g_containerDbgObjColli.push_back(tmp);
#endif //_DEBUG

			if( ret.m_bCollision )
				return true;
		}
		return false;
	}

	BOOL	IsCollision(void)const { return ret.m_bCollision; };
	RwReal	GetDistance(void)const { return ret.m_fDistance; };
};
BOOL AgcuMapGeoInfo::vChckColliObj
(RpIntersection& rpIntsct, const std::list<RwInt32>& idList, RwReal* dist)const
{
	functrObjCollisionChk ret(m_pApmObject,m_pAgcmObject,&rpIntsct);
	std::list<RwInt32>::const_iterator	it_f = 
	std::find_if(
	//std::for_each(
		  idList.begin()
		, idList.end()
		, ret
		);

	if(dist)
		*dist = ret.GetDistance();
	return ((it_f == idList.end()) ? FALSE : TRUE);//ret.IsCollision();
};
// -----------------------------------------------------------------------------
// 오브젝트와 충돌 체크
BOOL AgcuMapGeoInfo::vChckColliObj
(RpIntersection& rpIntsctProjected, const std::list<RwInt32>& idList, RwInt32 loop)const
{
	ASSERT( loop > 1 );

	RpIntersection&	intsct = rpIntsctProjected;
	RwReal	fStep = 1.f / static_cast<RwReal>(loop);

	D3DXVECTOR2	v2Step( (intsct.t.line.end.x - intsct.t.line.start.x)*fStep
					  , (intsct.t.line.end.z - intsct.t.line.start.z)*fStep
					  );
	intsct.t.line.end	= intsct.t.line.start;

	for( int i=0; i<(loop-1); ++i )
	{
		intsct.t.line.end.x += v2Step.x;
		intsct.t.line.end.z += v2Step.y;				
		intsct.t.line.end.y  = intsct.t.line.start.y;

		if( vChckColliObj( intsct, idList ) )
			return TRUE;

		intsct.t.line.start = intsct.t.line.end;
		intsct.t.line.start.y 
			= this->m_pAgcmMap->GetHeight( intsct.t.line.start.x
			, intsct.t.line.start.z
			, intsct.t.line.start.y );
		intsct.t.line.start.y += m_ridableheight;
	}
	intsct.t.line.end.x += v2Step.x;
	intsct.t.line.end.z += v2Step.y;				
	intsct.t.line.end.y  = intsct.t.line.start.y;

	if( vChckColliObj( intsct, idList ) )
		return TRUE;

	return FALSE;
};
// -----------------------------------------------------------------------------
// 오브젝트와 충돌 체크
// 이 함수에서 intsct.t.line.end 에는 높이 정보가 없다.
BOOL AgcuMapGeoInfo::vChckColliObj_Modified
(RpIntersection& intsct, const std::list<RwInt32>& idList)const
{
	// 1. 일단 지형만의 높이 만을 고려
	intsct.t.line.end.y = 
	m_pAgcmMap->HP_GetHeightGeometryOnly(intsct.t.line.end.x, intsct.t.line.end.z);
	intsct.t.line.end.y += m_ridableheight;

	if( vChckColliObj( intsct, idList ) )
	{
		// 2. 충돌 했다면 현재 위치에서의 높이값으로 다시..
		intsct.t.line.end.y = intsct.t.line.start.y;

		RwReal	fDistance = 0.f;
		if( vChckColliObj( intsct, idList, &fDistance ) )
		{
			// 3. 역시 충돌 했다면 충돌된 위치를 시작점으로 하여 다시 평형 체크
			RwV3d	dir = {0.f,0.f,0.f,};
			RwV3dSub( &dir, &intsct.t.line.end, &intsct.t.line.start );
			RwV3dNormalize ( &dir, &dir );
			RpIntersection tmpintsct = intsct;
			RwV3dScale( &dir, &dir, fDistance );


			RwV3dAdd( &tmpintsct.t.line.start
				, &intsct.t.line.start
				, &dir );

			intsct.t.line.start.y = m_pAgcmMap->GetHeight(
											tmpintsct.t.line.start.x
											, tmpintsct.t.line.start.z 
											, tmpintsct.t.line.start.y );
			intsct.t.line.start.y += m_ridableheight;

			tmpintsct.t.line.end.y = intsct.t.line.start.y;

			if( vChckColliObj( tmpintsct, idList, &fDistance ) )
			{
				return TRUE;
			}

		}
		else 
			return FALSE;
	}

	return FALSE;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_l2r(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+0, ptTile.x+1);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+1, ptTile.x+1);

	return 2;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_r2l(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+0, ptTile.x+0);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+1, ptTile.x+0);

	return 2;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_t2b(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+1, ptTile.x+1);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+1, ptTile.x+0);

	return 2;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_b2t(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+0, ptTile.x+0);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+0, ptTile.x+1);

	return 2;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_lt2rb(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+0, ptTile.x+1);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+1, ptTile.x+1);

	arrPos[2].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[2].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[2].y = m_heightmap(ptTile.y+1, ptTile.x+0);

	return 3;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_rb2lt(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+1, ptTile.x+0);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+0, ptTile.x+0);

	arrPos[2].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[2].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[2].y = m_heightmap(ptTile.y+0, ptTile.x+1);

	return 3;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_rt2lb(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+0, ptTile.x+0);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+1, ptTile.x+0);

	arrPos[2].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[2].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[2].y = m_heightmap(ptTile.y+1, ptTile.x+1);

	return 3;
};

// -----------------------------------------------------------------------------
RwInt32 AgcuMapGeoInfo::vGetLinesForChk_lb2rt(RwV3d arrPos[3]
											, const POINT& ptTile)const
{
	if( !vIsAvailableTileIndex( ptTile ) )
		return -1;	//overrange

	arrPos[0].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+0))*m_tilesize;
	arrPos[0].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[0].y = m_heightmap(ptTile.y+0, ptTile.x+0);

	arrPos[1].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[1].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+0))*m_tilesize;
	arrPos[1].y = m_heightmap(ptTile.y+0, ptTile.x+1);

	arrPos[2].x = m_vLeftBottom.x + (static_cast<RwReal>(ptTile.x+1))*m_tilesize;
	arrPos[2].z = m_vLeftBottom.z + (static_cast<RwReal>(ptTile.y+1))*m_tilesize;
	arrPos[2].y = m_heightmap(ptTile.y+1, ptTile.x+1);

	return 3;
};

#ifdef _DEBUG
#include "AgcuVtxType.h"
// -----------------------------------------------------------------------------
VOID renderTileWithLine(RwReal left, RwReal bottom, RwReal tilesize
						, const RwReal height[]
						, RwUInt32 colr=0xffffffff)
{
	USING_AGCUVTX;

	RwMatrix	mat;
	RwMatrixSetIdentity (&mat);
	static VTX_PD	buff[5];
	buff[0].pos.x = left;
	buff[0].pos.y = height[0];
	buff[0].pos.z = bottom;
	buff[0].diff  = colr;
	
	buff[1].pos.x = left+tilesize;
	buff[1].pos.y = height[1];
	buff[1].pos.z = bottom;
	buff[1].diff  = colr;	
	
	buff[2].pos.x = left+tilesize;
	buff[2].pos.y = height[2];
	buff[2].pos.z = bottom+tilesize;
	buff[2].diff  = colr;
	
	buff[3].pos.x = left;
	buff[3].pos.y = height[3];
	buff[3].pos.z = bottom+tilesize;
	buff[3].diff  = colr;

	buff[4] = buff[0];
	
	RwUInt32 lighting = 0LU;
	RwD3D9GetRenderState(D3DRS_LIGHTING, (void*)&lighting);
	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);

	RwD3D9SetTexture ( NULL, 0LU );
	RwD3D9SetFVF (VTX_PD::FVF );
	RwD3D9SetTransformWorld ( &mat );
	RwD3D9DrawPrimitiveUP (
		D3DPT_LINESTRIP
		, 4
		, buff
		, VTX_PD::SIZE
		);

	RwD3D9SetRenderState(D3DRS_LIGHTING, lighting);

};
VOID AgcuMapGeoInfo::bDbg_RenderHeightMap(RwUInt32 colr)
{
	RwV3d	vPos[5]	= { {0.f, 0.f, 0.f},
						{0.f, 0.f, 0.f},
						{0.f, 0.f, 0.f}, };
	RwReal	x	= m_vLeftBottom.x;
	RwReal	z	= m_vLeftBottom.z;
	RwReal	height[4] = {0.f,0.f,0.f,0.f,};
	for( int i=0; i<m_height; ++i )
	{
		x = m_vLeftBottom.x;
		for( int j=0; j<m_width; ++j )
		{
			height[0]	= m_heightmap(i,j);
			height[1]	= m_heightmap(i,j+1);
			height[2]	= m_heightmap(i+1,j+1);
			height[3]	= m_heightmap(i+1,j);
			
			renderTileWithLine(x, z, m_tilesize, height
				, bTileChk( j, i ) ? 0xffff0000 : colr);

			x += m_tilesize;
		}
		z += m_tilesize;
	}

};

// -----------------------------------------------------------------------------
class functr_drawTile
{
	RwV3d		_leftBottom;
	RwReal		_tileSize;
	RwUInt32	_colr;
	const CSecondArray<RwReal>& _heightMap;
public:
	functr_drawTile(  const RwV3d& leftBottom
					, RwReal tileSize
					, const CSecondArray<RwReal>& heightMap
					, RwUInt32 colr=0xffffffff )
					: _heightMap(heightMap)
					, _colr(colr)
					, _tileSize(tileSize)
	{
		_leftBottom = leftBottom;
	};
	void operator () (const POINT& pt)
	{
		RwReal	x = pt.x*_tileSize + _leftBottom.x;
		RwReal	z = pt.y*_tileSize + _leftBottom.z;

		RwReal	height[4] = { _heightMap(pt.y, pt.x)
							, _heightMap(pt.y, pt.x+1)
							, _heightMap(pt.y+1, pt.x+1)
							, _heightMap(pt.y+1, pt.x)	};
		renderTileWithLine(x, z, _tileSize, height, _colr);
	}
};
VOID AgcuMapGeoInfo::bDbg_RenderLineTile(RwUInt32 colr)
{
	std::for_each( m_lineTile.begin()
				 , m_lineTile.end()
				 , functr_drawTile(m_vLeftBottom, m_tilesize, m_heightmap, colr) );
};

VOID AgcuMapGeoInfo::bDbg_RenderColliObj(void)
{
	if( g_containerDbgObjColli.empty() )
		return;

	static	BOOL showall = FALSE;
	if( showall )
	{
		std::for_each( g_containerDbgObjColli.begin()
			, g_containerDbgObjColli.end()
			, functRenderObjColli() );
	}
	else
	{
		BOOL bchangeindex = FALSE;
		static	RwInt32 showindex	= 0;
		if( KEYDOWN( VK_PRIOR ) )
		{
			bchangeindex = TRUE;
			++showindex;
		}
		else if( KEYDOWN( VK_NEXT ) )
		{
			bchangeindex = TRUE;
			--showindex;
		}
		std::list<stObjColl>::iterator it_curr = g_containerDbgObjColli.begin();
		RwInt32	size	= g_containerDbgObjColli.size();
		::T_CLAMP(showindex, 0, size-1);

		//if( bchangeindex )
		//	Eff2Ut_ToConsol( Eff2Ut_FmtMsg("size : %d, index : %d\n", size, showindex) );


		std::advance(it_curr, showindex);

		(*it_curr).Render();
	}
};

VOID AgcuMapGeoInfo::bDbg_RenderLineTileIntsct(void)
{
	g_DbgLineTileIntsct.render();
}
#endif //_DEBUG
// -----------------------------------------------------------------------------
// AgcuMapGeoInfo.cpp - End of file
// -----------------------------------------------------------------------------
