// AgcuAstar.h
// -----------------------------------------------------------------------------
//                                         _                  _     
//     /\                        /\       | |                | |    
//    /  \    __ _  ___ _   _   /  \   ___| |_  __ _ _ __    | |__  
//   / /\ \  / _` |/ __| | | | / /\ \ / __| __|/ _` | '__|   | '_ \ 
//  / ____ \| (_| | (__| |_| |/ ____ \\__ \ |_| (_| | |    _ | | | |
// /_/    \_\\__, |\___|\__,_/_/    \_\___/\__|\__,_|_|   (_)|_| |_|
//            __/ |                                                 
//           |___/                                                  
//
// a-star
// http://www.generation5.org/
// -----------------------------------------------------------------------------
// Originally created on 07/29/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUASTAR_20050729
#define _H_AGCUASTAR_20050729

#include <rwcore.h>
#include <list>
#include "magdebug.h"
#include "AgcuMapGeoInfo.h"

namespace NS_ASTAR
{
	namespace PRIVATE
	{
		enum eChildTile
		{
			e_leftTop		= 0,
			e_top			,
			e_rightTop		,

			e_right			,
			
			e_rightBottom	,
			e_bottom		,
			e_leftBottom	,

			e_left			,

			e_childTileNum	,
		};

	};
};
// -----------------------------------------------------------------------------
class AgcuAstar;
class CAstarNode
{
public:
	explicit CAstarNode(RwInt32 x=0, RwInt32 z=0);
	explicit CAstarNode(const CAstarNode& cpy);
	~CAstarNode();

	//set
	void	bSetPos(RwInt32 x, RwInt32 z)	{ m_x = x, m_z = z; };
	void	bSetCost(RwInt32 cost)			{ m_cost = cost;	};
	void	bSetParent(CAstarNode* parent)	{ m_parent = parent;};
	void	bSetHeight(RwReal height)		{ m_height = height;};

	//get
	RwInt32	bGetX()const	{ return m_x; };
	RwInt32	bGetZ()const	{ return m_z; };
	RwInt32 bGetCost()const { return m_cost; };
	RwReal	bGetHeight()const{ return m_height; };

	RwInt32	bAddChild(CAstarNode* child);


	//operator
	bool operator == (const CAstarNode& cmp)const;
	//bool operator < (const CAstarNode& cmp)	{return (m_cost < cmp.m_cost);};

private:
	RwInt32		m_x;
	RwInt32		m_z;
	RwInt32		m_cost;
	RwInt32		m_childnum;
	RwReal		m_height;

	CAstarNode* m_parent;
	CAstarNode*	m_child[NS_ASTAR::PRIVATE::e_childTileNum];

	friend class AgcuAstar;
}; 

// -----------------------------------------------------------------------------
class AgcuAstar
{
	typedef class CAstarNode			NODE, *PNODE, *LPNODE;
	typedef const NODE*					LPCNODE;

	typedef std::list<LPNODE>			NODE_CONTAINER;
	typedef NODE_CONTAINER::iterator	ITRATOR;

	typedef std::vector<RwV3d>			CONTAINER_WAYPOINT;

public:
	// Construction/Destruction
	AgcuAstar();
	virtual	~AgcuAstar();

	// Operators

	// Accessors
	VOID	bInit(const AgcuMapGeoInfo* pAgcuMapGeoInfo);

	// Interface methods
	RwInt32 bGeneratePath(const POINT& ptStart, const POINT& ptDest, RwReal startHeight);
	RwInt32	bGetWayPoint(CONTAINER_WAYPOINT& out, AgcuMapGeoInfo& MapGeoInfo);
	RwInt32 bReady(void);

#ifdef _DEBUG
	VOID	bDbg_RenderPath(const AgcuMapGeoInfo& MapGeoInfo, RwUInt32 colr=0xffffffff);
#endif //_DEBUG

private:
	// Data members
	//AgcuMapGeoInfo*	
	POINT					m_ptStart;

	NODE					m_dest;
	LPNODE					m_pBest;
	NODE_CONTAINER			m_openContainer;
	NODE_CONTAINER			m_closeContainer;
	const AgcuMapGeoInfo*	m_pAgcuMapGeoInfo;

	// Implementation methods
	VOID	vStepInitialize(RwInt32 sx, RwInt32 sz, RwInt32 dx, RwInt32 dz);
	RwInt32	vStep();

	VOID	vClearContainer();
	LPNODE	vNewBest();
	VOID	vCreateChild(LPNODE theBest);
	RwInt32	vLinkChild(LPNODE pParent, const NODE& newnode);
	RwInt32 vAddToOpen(LPNODE pnode);

	RwBool	vValid(LPCNODE pParent, LPNODE pNode)const;		// Called to check validity of a coordinate
	RwInt32 vCost (LPCNODE pParent, LPCNODE pNode)const;		// Called when cost value is need

};

#endif // _H_AGCUASTAR_20050729
// -----------------------------------------------------------------------------
// AgcuAstar.h - End of file
// -----------------------------------------------------------------------------

