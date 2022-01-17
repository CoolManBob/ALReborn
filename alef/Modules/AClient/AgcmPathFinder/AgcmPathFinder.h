// AgcmPathFinder.h
// -----------------------------------------------------------------------------
//                                _____        _   _     ______ _           _               _     
//     /\                        |  __ \      | | | |   |  ____(_)         | |             | |    
//    /  \    __ _  ___ _ __ ___ | |__) | __ _| |_| |__ | |__   _ _ __   __| | ___ _ __    | |__  
//   / /\ \  / _` |/ __| '_ ` _ \|  ___/ / _` | __| '_ \|  __| | | '_ \ / _` |/ _ \ '__|   | '_ \ 
//  / ____ \| (_| | (__| | | | | | |    | (_| | |_| | | | |    | | | | | (_| |  __/ |    _ | | | |
// /_/    \_\\__, |\___|_| |_| |_|_|     \__,_|\__|_| |_|_|    |_|_| |_|\__,_|\___|_|   (_)|_| |_|
//            __/ |                                                                               
//           |___/                                                                                
//
// path finding module
//
// -----------------------------------------------------------------------------
// Originally created on 08/12/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCMPATHFINDER_20050812
#define _H_AGCMPATHFINDER_20050812

#include "AgcModule.h"
#include "AgcuPathFind.h"

// -----------------------------------------------------------------------------

class AgcmPathFinder : public AgcModule
{
	typedef std::vector<RwV3d>		CONTAINER_WAYPOINT;
public:
	// Construction/Destruction
	AgcmPathFinder();
	virtual	~AgcmPathFinder();

	// Operators

	// Accessors

	// Interface methods
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulNowTime);

	RwInt32	bFindPath(const RwV3d&	start, const RwV3d& dest);
	BOOL	bPopWayPoint(RwV3d& thePos);

#ifdef _DEBUG
	VOID	bDbg_Render(RwInt32 colr=0xffffffff);
	VOID	bDbg_RenderWayPoint(RwInt32 colr=0xffffffff);
#endif //_DEBUG

private:
	// Data members
	AgcuPathFind		m_cPathFind;
	CONTAINER_WAYPOINT	m_waypoint;
	RwV3d				m_vDest;

	// Implementation methods
	RwInt32	vIntpWayPoint(void);

};

#endif // _H_AGCMPATHFINDER_20050812
// -----------------------------------------------------------------------------
// AgcmPathFinder.h - End of file
// -----------------------------------------------------------------------------

