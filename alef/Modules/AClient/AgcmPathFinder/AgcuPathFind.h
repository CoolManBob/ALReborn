// AgcuPathFind.h
// -----------------------------------------------------------------------------
//                            _____        _   _     ______ _           _     _     
//     /\                    |  __ \      | | | |   |  ____(_)         | |   | |    
//    /  \    __ _  ___ _   _| |__) | __ _| |_| |__ | |__   _ _ __   __| |   | |__  
//   / /\ \  / _` |/ __| | | |  ___/ / _` | __| '_ \|  __| | | '_ \ / _` |   | '_ \ 
//  / ____ \| (_| | (__| |_| | |    | (_| | |_| | | | |    | | | | | (_| | _ | | | |
// /_/    \_\\__, |\___|\__,_|_|     \__,_|\__|_| |_|_|    |_|_| |_|\__,_|(_)|_| |_|
//            __/ |                                                                 
//           |___/                                                                  
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 07/20/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUPATHFIND_20050720
#define _H_AGCUPATHFIND_20050720

#include "ApmMap.h"
#include <rwcore.h>
#include <vector>

#include "AgcuMapGeoInfo.h"
#include "AgcuAstar.h"

// -----------------------------------------------------------------------------
class AgcuPathFind
{
	typedef std::vector<RwV3d>		CONTAINER_WAYPOINT;
	typedef std::vector<RwLine>		CONTAINER_LINE;
	typedef std::vector<RwInt32>	CONTAINER_TILE;
public:
	// Construction/Destruction
	AgcuPathFind();
	virtual	~AgcuPathFind();

	// Operators

	// Accessors
	VOID	bSetModule( ApmMap* pApmMap, AgcmMap* pAgcmMap, ApmObject* pApmObject, AgcmObject* pAgcmObject );

	BOOL			bIsDestTruncated(void)const { return m_bTruncatedDest; };
	const RwV3d*	bGetTruncatedDest()const;

	// Interface methods
	RwInt32	bFindPath( const RwV3d&	start, const RwV3d& dest, CONTAINER_WAYPOINT& out );

#ifdef _DEBUG
	VOID	bDbg_RenderPath(RwUInt32 colr=0xffffffff);
	VOID	bDbg_RenderRect(RwUInt32 colr=0xffffffff);
	VOID	bDbg_RenderLineTile(RwUInt32 colr=0xffffffff);
	VOID	bDbg_RenderColliObj(void);
	VOID	bDbg_RenderLineTileIntsct(void);
#endif //_DEBUG

private:
	// Data members
	ApmMap*			m_pApmMap;
	RwReal			m_fTileSize;

	BOOL			m_bTruncatedDest;

	AgcuMapGeoInfo	m_cMapGeoInfo;
	AgcuAstar		m_cAstar;

	// Implementation methods
	RwInt32 vCalcTile(const RwV3d&	start, const RwV3d& dest);

};

#endif // _H_AGCUPATHFIND_20050720
// -----------------------------------------------------------------------------
// AgcuPathFind.h - End of file
// -----------------------------------------------------------------------------

