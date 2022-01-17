// AcuSinTbl.cpp
// -----------------------------------------------------------------------------
//                       _____ _       _______ _     _                      
//     /\               / ____(_)     |__   __| |   | |                     
//    /  \    ___ _   _| (___  _ _ __    | |  | |__ | |     ___ _ __  _ __  
//   / /\ \  / __| | | |\___ \| | '_ \   | |  | '_ \| |    / __| '_ \| '_ \ 
//  / ____ \| (__| |_| |____) | | | | |  | |  | |_) | | _ | (__| |_) | |_) |
// /_/    \_\\___|\__,_|_____/|_|_| |_|  |_|  |_.__/|_|(_) \___| .__/| .__/ 
//                                                             | |   | |    
//                                                             |_|   |_|    
//
// sin table
//
// -----------------------------------------------------------------------------
// Originally created on 02/19/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AcuSinTbl.h"

NSACUMATH_BEGIN

const AcuSinTbl*	AcuSinTbl::SINGLETON = NULL;
float				AcuSinTbl::sin[AcuSinTbl::E_TABLESIZE];
AcuSinTbl			g_sintable;

// -----------------------------------------------------------------------------
AcuSinTbl::AcuSinTbl()
{
	ASSERT( !SINGLETON );

	SINGLETON = this;
	float *psin = sin;
	for( int i=0; i<E_TABLESIZE; ++i )
		*(psin++) = sinf( static_cast<float>(i)*DEF_2PI/256.f );
}

NSACUMATH_END
// -----------------------------------------------------------------------------
// AcuSinTbl.cpp - End of file
// -----------------------------------------------------------------------------
