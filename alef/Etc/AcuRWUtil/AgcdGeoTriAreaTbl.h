// AgcdGeoTriAreaTbl.h
// -----------------------------------------------------------------------------
//                          _  _____            _______      _                          _______ _     _     _     
//     /\                  | |/ ____|          |__   __|    (_)   /\                   |__   __| |   | |   | |    
//    /  \    __ _  ___  __| | |  __  ___  ___    | |   _ __ _   /  \   _ __  ___  __ _   | |  | |__ | |   | |__  
//   / /\ \  / _` |/ __|/ _` | | |_ |/ _ \/ _ \   | |  | '__| | / /\ \ | '__|/ _ \/ _` |  | |  | '_ \| |   | '_ \ 
//  / ____ \| (_| | (__| (_| | |__| |  __/ (_) |  | |  | |  | |/ ____ \| |  |  __/ (_| |  | |  | |_) | | _ | | | |
// /_/    \_\\__, |\___|\__,_|\_____|\___|\___/   |_|  |_|  |_/_/    \_\_|   \___|\__,_|  |_|  |_.__/|_|(_)|_| |_|
//            __/ |                                                                                               
//           |___/                                                                                                
//
// geometry triangle surface area
//
// -----------------------------------------------------------------------------
// Originally created on 03/18/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCDGEOTRIAREATBL_20050318
#define _H_AGCDGEOTRIAREATBL_20050318

/*===========================================================================*
 *--- Includes --------------------------------------------------------------*
 *===========================================================================*/

#include "rwcore.h"
#include "rpworld.h"

/*===========================================================================*
 *--- Plugin Types ----------------------------------------------------------*
 *===========================================================================*/

typedef struct	stGeoTriTbl
{
	RwReal*		ptbl;
}GEOTRITBL, *LPGEOTRITBL;
typedef const struct stGeoTriTbl* LPCGEOTRITBL;

/*===========================================================================*
 *--- Global Defines --------------------------------------------------------*
 *===========================================================================*/

#define rwVENDERID_GEOTRIAREATBL	0xfffff0L
#define	rwID_GEOTRIAREATBL			0x10
#define	rwID_GEOTRIAREATBLPLUGIN	(MAKECHUNKID(rwVENDERID_GEOTRIAREATBL,rwID_GEOTRIAREATBL))

#define	rwGEOTRIAREATBLSIZE			(sizeof(GEOTRITBL))

#define RPGEOTRIAREATBLGETDATA(_gmtr)		\
	( (LPGEOTRITBL)( ((RwUInt8*)(_gmtr)) + _rpGeometryTriAreaTblOffset ) )


/*===========================================================================*
 *--- Toolkit API Functions -------------------------------------------------*
 *===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern	RwInt32	_rpGeometryTriAreaTblOffset;

extern	RwInt32	
	RpClumpGet3DPos(RwV3d* out, RpClump* clump, RwReal posInArea);
extern	RwInt32	
	RpAtomicGet3DPos(RwV3d* out, RpAtomic* atom, RwReal posInArea);
extern RwBool
	RpGeoTriAreaTblPluginAttach(void);

extern	RwReal	
	RpClumpGetSurfaceArea(RpClump* clump);
extern	RwReal	
	RpAtomicGetSurfaceArea(RpAtomic* atom);

#ifdef __cplusplus
}
#endif // __cplusplus



#endif // _H_AGCDGEOTRIAREATBL_20050318
// -----------------------------------------------------------------------------
// AgcdGeoTriAreaTbl.h - End of file
// -----------------------------------------------------------------------------

