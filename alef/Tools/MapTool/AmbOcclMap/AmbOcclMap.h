// -----------------------------------------------------------------------------
//                     _      ____            _ __  __                 _     
//     /\             | |    / __ \          | |  \/  |               | |    
//    /  \   _ __ ___ | |__ | |  | | ___  ___| | \  / | __ _ _ __     | |__  
//   / /\ \ | '_ ` _ \| '_ \| |  | |/ __|/ __| | |\/| |/ _` | '_ \    | '_ \ 
//  / ____ \| | | | | | |_) | |__| | (__| (__| | |  | | (_| | |_) | _ | | | |
// /_/    \_\_| |_| |_|_.__/ \____/ \___|\___|_|_|  |_|\__,_| .__/ (_)|_| |_|
//                                                          | |              
//                                                          |_|              
//
// Ambient occlusion map toolkit for MapTool
//
// -----------------------------------------------------------------------------
// Originally created on 12/29/2004 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2004, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AMBOCCLMAP
#define _H_AMBOCCLMAP

#include "AcuRtAmbOcclMap.h"
#include <vector>

// -----------------------------------------------------------------------------

class AmbOcclMap
{
public:
	// Construction/Destruction
	AmbOcclMap();
	virtual	~AmbOcclMap();

	// Operators

	// Accessors
	unsigned int getSessionStartObj() const 
	{ return lightingSession_.startObj; }
	unsigned int getSessionTotalObj() const 
	{ return lightingSession_.totalObj; }
	//@{ Jaewon 20050112
	void setSuperSample(unsigned int superSample)
	{ 
		if(superSample < 1)
			superSample_ = 1;
		else if(superSample > 16)
			superSample_ = 16;
		else
			superSample_ = superSample;
	}
	unsigned int getSuperSample() const
	{ return superSample_; }
	//@} Jaewon

	// Interface methods
	//@{ Jaewon 20050203
	// Dungeons should have a different fx applied 
	// and be computed by using a ray of finite length.
	// rayLength > 0 <-> Dungeon
	//@{ Jaewon 2005020
	// rayCount parameter added.
	//@{ Jaewon 20050712
	// recreateAll parameter added.
	bool create(RwUInt32 rayCount, float rayLength = 0, bool recreateAll=false);
	//@} Jaewon
	//@] Jaewon
	//@} Jaewon
	bool clear();
	//@{ Jaewon 20050712
	// destroyAll parameter added.
	bool destroy(bool destroyAll=false);
	//@} Jaewon
	int compute(RtAmbOcclMapIlluminateProgressCallBack progressCB);
	bool save(const char *directory);

private:
	// Data members
	RtAmbOcclMapLightingSession lightingSession_;
	RpWorld *pWorld_;
	//@{ Jaewon 20050112
	unsigned int superSample_;
	//@} Jaewon

	// Implementation methods
	// create a world, gather & setup atomics from the map and add them to the world. 
	//@{ Jaewon 20050203
	// rayLength parameter added for dungeons.
	RpWorld * gatherAndSetupAtomics(float rayLength = 0);
	//@} Jaewon
	
	//@{ Jaewon 20050414
	// Process its uv sets properly for the ambient occlusion fx and turn its flag on.
	enum AmbOcclMapSizeScale { AOMSS_QUATER=0, AOMSS_HALF, AOMSS_ONE, AOMSS_DOUBLE, AOMSS_QUADRUPLE };
	void prepareForAmbOcclMap(RpAtomic *pAtomic, AmbOcclMapSizeScale scale = AOMSS_ONE) const;
	//@} Jaewon

	// fill AgcmAmbOcclMap data before saving the object.
	static void fillObjectData(ApdObject *pApdObject);
};

#endif // _H_AMBOCCLMAP
// -----------------------------------------------------------------------------
// AmbOcclMap.h - End of file
// -----------------------------------------------------------------------------

