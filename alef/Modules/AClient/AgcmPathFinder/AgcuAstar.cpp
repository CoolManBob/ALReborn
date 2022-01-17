// AgcuAstar.cpp
// -----------------------------------------------------------------------------
//                                         _                                   
//     /\                        /\       | |                                  
//    /  \    __ _  ___ _   _   /  \   ___| |_  __ _ _ __      ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | | / /\ \ / __| __|/ _` | '__|    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| |/ ____ \\__ \ |_| (_| | |    _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_/_/    \_\___/\__|\__,_|_|   (_) \___| .__/| .__/ 
//            __/ |                                               | |   | |    
//           |___/                                                |_|   |_|    
//
// a-star
//
// -----------------------------------------------------------------------------
// Originally created on 07/29/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcuAstar.h"
#include "ApDefine.h"
#include <algorithm>

using namespace NS_ASTAR;
using namespace NS_ASTAR::PRIVATE;
// -----------------------------------------------------------------------------
// CAstarNode

// -----------------------------------------------------------------------------
CAstarNode::CAstarNode(RwInt32 x, RwInt32 z) 
: m_x(x), m_z(z), m_cost(0), m_childnum(0), m_parent(NULL), m_height(0.f)
{
	memset(m_child, 0, sizeof(m_child));
}
CAstarNode::CAstarNode(const CAstarNode& cpy)
: m_x(cpy.m_x)
, m_z(cpy.m_z)

, m_cost(0)
, m_childnum(0)
, m_parent(NULL)

, m_height(cpy.m_height)
{
	memset(m_child, 0, sizeof(m_child));
}

// -----------------------------------------------------------------------------
CAstarNode::~CAstarNode()
{
}

// -----------------------------------------------------------------------------
RwInt32	CAstarNode::bAddChild(CAstarNode* child)
{
	ASSERT( m_childnum < NS_ASTAR::PRIVATE::e_childTileNum); 
	if(m_childnum < NS_ASTAR::PRIVATE::e_childTileNum)
		m_child[m_childnum++] = child;
	else
		return -1;
	return 0;
}

// -----------------------------------------------------------------------------
bool CAstarNode::operator == (const CAstarNode& cmp)const
{
	return ((m_x == cmp.m_x) && (m_z == cmp.m_z));
};

// -----------------------------------------------------------------------------
// AgcuAstar

// -----------------------------------------------------------------------------
AgcuAstar::AgcuAstar()
: m_pBest(NULL)
{
	m_ptStart.x = 
	m_ptStart.y = 0;
}

// -----------------------------------------------------------------------------
AgcuAstar::~AgcuAstar()
{
	vClearContainer();
}

// -----------------------------------------------------------------------------
VOID AgcuAstar::vStepInitialize(RwInt32 sx, RwInt32 sz, RwInt32 dx, RwInt32 dz)
{
	vClearContainer();

	m_ptStart.x	= sx;
	m_ptStart.y	= sz;
	m_dest.bSetPos(dx, dz);

	LPNODE	pNode	= new NODE(sx, sz);
	pNode->bSetCost( 0 );

	//캐릭 현재 위치를 시작 높이로 정함.
	pNode->bSetHeight( m_pAgcuMapGeoInfo->bGetStart().y );

	m_openContainer.push_back(pNode);
};

// -----------------------------------------------------------------------------
// return val : 0(continue), 1(find!), 2(can't find)
RwInt32 AgcuAstar::vStep()
{
	RwInt32 ir = 0;
	LPNODE newBest = vNewBest();
	if( !newBest )
		ir = 2;
	else if( *(m_pBest=newBest) == m_dest )
		ir = 1;//find it
	else
		vCreateChild( newBest );

	return ir;
};

// -----------------------------------------------------------------------------
class functorDeleteNode
{
public:
	void operator () (CAstarNode*& pnode)
	{
		DEF_SAFEDELETE( pnode );
	};
};
VOID AgcuAstar::vClearContainer()
{
	std::for_each( m_openContainer.begin()
				 , m_openContainer.end()
				 , functorDeleteNode()
				 );
	
	std::for_each( m_closeContainer.begin()
				 , m_closeContainer.end()
				 , functorDeleteNode()
				 );

	m_openContainer.clear();
	m_closeContainer.clear();

	//the best is NULL
	m_pBest = NULL;
};

// -----------------------------------------------------------------------------
AgcuAstar::LPNODE AgcuAstar::vNewBest()
{
	LPNODE	ret = NULL;
	if( !m_openContainer.empty() )
	{
		ret = m_openContainer.front();
		ASSERT( ret );
		m_closeContainer.push_front(ret);
		m_openContainer.pop_front();
	}
	return ret;
};

// -----------------------------------------------------------------------------
VOID AgcuAstar::vCreateChild(LPNODE theBest)
{
	const POINT	offset[e_childTileNum]
	= {
		{-1,-1},	//e_leftTop		
		{ 0,-1},	//e_top			
		{ 1,-1},	//e_rightTop		

		{ 1, 0},	//e_Right			

		{ 1, 1},	//e_rightBottom	
		{ 0, 1},	//e_bottom		
		{-1,-1},	//e_leftBottom	

		{-1, 0},	//e_left
	};

	NODE	node(0,0);
	for( int i=0; i<e_childTileNum; ++i )
	{
		node.bSetPos( theBest->bGetX()+offset[i].x
					, theBest->bGetZ()+offset[i].y
					);
		if( vValid(theBest, &node) )
			vLinkChild(theBest, node);
	}
};

// -----------------------------------------------------------------------------
class functorFindInContainer
{
	const CAstarNode* pTheNode;
public:
	functorFindInContainer( const CAstarNode& pTheNode ) : pTheNode(&pTheNode){};
	bool operator() (const CAstarNode* pnode)
	{
		return ( (pTheNode->bGetX() == pnode->bGetX() ) &&
				 (pTheNode->bGetZ() == pnode->bGetZ() ) );
	};
};
RwInt32 AgcuAstar::vLinkChild(LPNODE pParent, const NODE& newnode)
{
	RwInt32	cost = pParent->bGetCost() + vCost(pParent, &newnode);

	ITRATOR	it_f = std::find_if( m_openContainer.begin()
							   , m_openContainer.end()
							   , functorFindInContainer(newnode)
							   );
	if( it_f != m_openContainer.end() )	{
		pParent->bAddChild( *it_f );

		if( cost < (*it_f)->bGetCost() ) {
			(*it_f)->bSetCost(cost);
			(*it_f)->bSetParent(pParent);
		}
	}
	else
	{
		it_f = std::find_if( m_closeContainer.begin()
						   , m_closeContainer.end()
						   , functorFindInContainer(newnode)
						   );
		if( it_f != m_closeContainer.end() ) {
			pParent->bAddChild( *it_f );

			if( cost < (*it_f)->bGetCost() ) {
				(*it_f)->bSetCost(cost);
				(*it_f)->bSetParent(pParent);
			}
		}
		else
		{
			LPNODE pnewnode = new NODE(newnode);
			pnewnode->bSetCost(cost);
			pnewnode->bSetParent(pParent);
			
			vAddToOpen(pnewnode);
		}
	}

	return 0;
};

// -----------------------------------------------------------------------------
class functorLess
{
public:
	bool operator () (CAstarNode* itr, CAstarNode* pVal)
	{
		return ( itr->bGetCost() < pVal->bGetCost() );
	}
};

RwInt32 AgcuAstar::vAddToOpen(LPNODE pnode)
{
	ITRATOR	it_pos = std::lower_bound( m_openContainer.begin()
									 , m_openContainer.end()
									 , pnode
									 , functorLess()
									 );

	m_openContainer.insert( it_pos, pnode );

	return 0;
};
	
// -----------------------------------------------------------------------------
// Called to check validity of a coordinate
RwBool AgcuAstar::vValid(LPCNODE pParent, LPNODE pNode)const
{
	if( !m_pAgcuMapGeoInfo )
		return FALSE;

	RwV3d tileCenter = {0.f, 0.f, 0.f};
	m_pAgcuMapGeoInfo->bGetTileCenter( 
		  tileCenter
		, pParent->bGetX()
		, pParent->bGetZ()
		);
	tileCenter.y = pParent->bGetHeight();

	RwReal	nodeHeight	= 0.f;
	RwInt32 chk = m_pAgcuMapGeoInfo->bTileChk(
		pNode->bGetX()
		, pNode->bGetZ()
		, &tileCenter
		, &nodeHeight
		);
	pNode->bSetHeight( nodeHeight );

	return ((chk == 0) ? TRUE : FALSE);
};
	
// -----------------------------------------------------------------------------
// Called when cost value is need
RwInt32 AgcuAstar::vCost(LPCNODE pDest, LPCNODE pNode)const
{
	RwInt32 dx = pDest->bGetX()-pNode->bGetX();
	RwInt32 dz = pDest->bGetZ()-pNode->bGetZ();

	return (dx*dx + dz*dz);
};

// -----------------------------------------------------------------------------
// 
VOID AgcuAstar::bInit(const AgcuMapGeoInfo* pAgcuMapGeoInfo)
{
	m_pAgcuMapGeoInfo = pAgcuMapGeoInfo;
	ASSERT( m_pAgcuMapGeoInfo );
};

// -----------------------------------------------------------------------------
// 
// return
//		1 : find it
//		2 : not found
RwInt32 AgcuAstar::bGeneratePath
(const POINT& ptStart, const POINT& ptDest, const RwReal startHeight)
{
	vStepInitialize(ptStart.x, ptStart.y, ptDest.x, ptDest.y);

	RwInt32 ir = 0;

	do
	{
		ir = vStep();
	}while( ir == 0 );

	ASSERT( ir == 1 || ir == 2 );
	return ir;
};

// -----------------------------------------------------------------------------
// 
RwInt32 AgcuAstar::bGetWayPoint(CONTAINER_WAYPOINT& out, AgcuMapGeoInfo& MapGeoInfo)
{
	if(!m_pBest)
	{
		ASSERT( !"m_pBest == NULL" );
		return -1;
	}

	RwInt32 reserveSize = 0;
	LPNODE	pTmp = m_pBest;
	while( pTmp )
	{
		++reserveSize;
		pTmp = pTmp->m_parent;
	}
	out.reserve( reserveSize );

	RwV3d	tileCenter	= {0.f, 0.f, 0.f};
	LPNODE	pNode		= m_pBest,
			pPastNode	= m_pBest;

	RwInt32	dx = 0, 
			dz = 0;
	RwInt32	pastdx = -1, 
			pastdz = -1;

	while( pNode )
	{
		MapGeoInfo.bGetTileCenter( tileCenter, pNode->m_x, pNode->m_z );
		tileCenter.y = pNode->bGetHeight();

		dx = pNode->m_x - pPastNode->m_x;
		dz = pNode->m_z - pPastNode->m_z;

		if( dx == pastdx && dz == pastdz && (!out.empty()) )
			out.back() = tileCenter;
		else
			out.push_back( tileCenter );

		pastdx	= dx;
		pastdz	= dz;
		
		pPastNode	= pNode;

		pNode	= pNode->m_parent;
	}

	ASSERT( !out.empty() );
	if( out.empty() )
		return -1;

	//std::reverse( out.begin(), out.end() );

	return 0;
};

// -----------------------------------------------------------------------------
// 
RwInt32 AgcuAstar::bReady(void)
{
	m_pBest	= NULL;
	vClearContainer();

	return 0;
};

#ifdef _DEBUG
// -----------------------------------------------------------------------------
// 
VOID AgcuAstar::bDbg_RenderPath(const AgcuMapGeoInfo& MapGeoInfo, RwUInt32 colr)
{
	extern VOID renderTileWithLine(RwReal left, RwReal top, RwReal tilesize
							, const RwReal height[], RwUInt32 colr);

	const RwReal	tileSize = MapGeoInfo.bGetTileSize();
	const RwV3d		leftBtm	 = MapGeoInfo.bGetLeftBottom();//leftBottom

	RwReal			height[4] = {0.f,0.f,0.f,0.f,};

	LPNODE	pNode = m_pBest;
	RwUInt32	ulColr = 0xffffff00;
	while( pNode )
	{
		RwReal	left = leftBtm.x + static_cast<RwReal>(pNode->m_x)*tileSize;
		RwReal	top  = leftBtm.z + static_cast<RwReal>(pNode->m_z)*tileSize;

		height[0]	= MapGeoInfo.bGetHeight(pNode->m_x, pNode->m_z);
		height[1]	= MapGeoInfo.bGetHeight(pNode->m_x+1, pNode->m_z);
		height[2]	= MapGeoInfo.bGetHeight(pNode->m_x+1, pNode->m_z+1);
		height[3]	= MapGeoInfo.bGetHeight(pNode->m_x, pNode->m_z+1);

		renderTileWithLine(left, top, tileSize, height, ulColr);
		ulColr = colr;

		pNode = pNode->m_parent;
	}
};
#endif //_DEBUG
// -----------------------------------------------------------------------------
// AgcuAstar.cpp - End of file
// -----------------------------------------------------------------------------
