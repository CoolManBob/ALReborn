// AgcdGeoTriAreaTbl.cpp
// -----------------------------------------------------------------------------
//                          _  _____            _______      _                          _______ _     _                      
//     /\                  | |/ ____|          |__   __|    (_)   /\                   |__   __| |   | |                     
//    /  \    __ _  ___  __| | |  __  ___  ___    | |   _ __ _   /  \   _ __  ___  __ _   | |  | |__ | |     ___ _ __  _ __  
//   / /\ \  / _` |/ __|/ _` | | |_ |/ _ \/ _ \   | |  | '__| | / /\ \ | '__|/ _ \/ _` |  | |  | '_ \| |    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| (_| | |__| |  __/ (_) |  | |  | |  | |/ ____ \| |  |  __/ (_| |  | |  | |_) | | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|\_____|\___|\___/   |_|  |_|  |_/_/    \_\_|   \___|\__,_|  |_|  |_.__/|_|(_) \___| .__/| .__/ 
//            __/ |                                                                                             | |   | |    
//           |___/                                                                                              |_|   |_|    
//
// geometry triangle surface area
//
// -----------------------------------------------------------------------------
// Originally created on 03/18/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcdGeoTriAreaTbl.h"

#include <rpdbgerr.h>
#include <rplodatm.h>

// -----------------------------------------------------------------------------
typedef struct stFindAtom
{
	RpAtomic* atom;
	RwReal			posInAtomicArea;
}stFindAtom;

RwInt32	_rpGeometryTriAreaTblOffset	= 0;

// -----------------------------------------------------------------------------
RwReal TriSurfArea(const RwV3d* in0, const RwV3d* in1, const RwV3d* in2)
{	
	RwV3d	v01, v02, vcross;
	RwV3dSub( &v01, in1, in0 );
	RwV3dSub( &v02, in2, in0 );
	RwV3dCrossProduct( &vcross, &v01, &v02 );
	return (RwV3dLength (&vcross) * 0.5f);
}

// -----------------------------------------------------------------------------
#ifdef _DEBUG
LPGEOTRITBL GeoTriTblGetTbl(const RpGeometry* geo)
{
	LPGEOTRITBL	tritbl	= RPGEOTRIAREATBLGETDATA(geo);
	return tritbl;
}
#else 
#define GeoTriTblGetTbl(geo)	RPGEOTRIAREATBLGETDATA(geo)
#endif //__DEBUG

// -----------------------------------------------------------------------------
#ifdef _DEBUG
void GeoTriTblSetTbl(RpGeometry* geo, RwReal* trisufareatbl)
{	
	GeoTriTblGetTbl(geo)->ptbl	= trisufareatbl;
};
#else 
#define GeoTriTblSetTbl(geo, trisufareatbl)	GeoTriTblGetTbl(geo)->ptbl	= (trisufareatbl)
#endif //__DEBUG

// -----------------------------------------------------------------------------
LPGEOTRITBL GeoTriTblAlloc(RpGeometry* geo)
{
	RwInt32	trinum = 0;
	RwReal*	tbl	= NULL;

	RWFUNCTION(RWSTRING("GeoTriTblAlloc"));
	RWASSERT( geo && !GeoTriTblGetTbl(geo) );

	if( !GeoTriTblGetTbl(geo)->ptbl )
	{
		trinum = RpGeometryGetNumTriangles(geo);
		RWASSERT( trinum );
		if( trinum )
		{
			tbl	= (RwReal*)malloc(trinum * sizeof(RwReal));
			RWASSERT( tbl );
			GeoTriTblSetTbl(geo, tbl);
		}
	}

	RWRETURN(GeoTriTblGetTbl(geo));
}

// -----------------------------------------------------------------------------
LPGEOTRITBL GeoTriTblDealloc(RpGeometry* geo)
{
	RWFUNCTION(RWSTRING("GeoTriTblDealloc"));
	RWASSERT( geo && GeoTriTblGetTbl(geo) );

	if( GeoTriTblGetTbl(geo)->ptbl )
	{
		RwFree(GeoTriTblGetTbl(geo)->ptbl);
		GeoTriTblSetTbl(geo, NULL);
	}

	RWRETURN(GeoTriTblGetTbl(geo));
}

// -----------------------------------------------------------------------------
static void*
CB_GeoTriAreaTblConstructor(void* geo, RwInt32 offset, RwInt32 size)
{
	offset;
	size;
	
	if( ((_rpGeometryTriAreaTblOffset) & 0x80000000) ) //_rpGeometryTriAreaTblOffset < 0
		return NULL;

	GeoTriTblGetTbl(geo)->ptbl	= NULL;

	RWRETURN (geo);
}

// -----------------------------------------------------------------------------
static void*
CB_GeoTriAreaTblDestructor(void* geo, RwInt32 offset, RwInt32 size)
{	
	offset;
	size;
	
	if( ((_rpGeometryTriAreaTblOffset) & 0x80000000) ) //_rpGeometryTriAreaTblOffset < 0
		return NULL;

	GeoTriTblDealloc(geo);

	RWRETURN (geo);
}

// -----------------------------------------------------------------------------
static void*
CB_GeoTriAreaTblCopy( void *dstGeo
					, const void *srcGeo
					, RwInt32 offset
					, RwInt32 size
					)
{	
	dstGeo;	srcGeo;	offset;	size;
	RWRETURN (NULL);
}

// -----------------------------------------------------------------------------
RwBool
RpGeoTriAreaTblPluginAttach(void)
{
	RWFUNCTION(RWSTRING("RpGeoTriAreaTblPluginAttach"));
	_rpGeometryTriAreaTblOffset =
		RpGeometryRegisterPlugin( sizeof(GEOTRITBL)
								, rwID_GEOTRIAREATBLPLUGIN
								, CB_GeoTriAreaTblConstructor
								, CB_GeoTriAreaTblDestructor
								, CB_GeoTriAreaTblCopy
								);
	RWASSERT(0 < _rpGeometryTriAreaTblOffset);

	RWRETURN(TRUE);
}


// -----------------------------------------------------------------------------
void GeoTriAreaTblBuildTbl(RpGeometry* geo)
{
	RwInt32			i			= 0;
	RwInt32			trinum		= 0;
	RpMorphTarget*	morph		= NULL;
	RpTriangle*		triangle	= NULL;
	RwV3d*			beginvtx	= NULL;
	RwReal*			ptbl		= GeoTriTblGetTbl(geo)->ptbl;

	RWFUNCTION(RWSTRING("GeoTriAreaTblBuildTbl"));
	RWASSERT( geo && ptbl );
	if( geo && GeoTriTblGetTbl(geo)->ptbl )
	{
		trinum	= RpGeometryGetNumTriangles(geo);

		RWASSERT( RpGeometryGetMorphTarget ( geo, 0 )
			 && RpGeometryGetTriangles( geo )
			 && RpMorphTargetGetVertices( RpGeometryGetMorphTarget ( geo, 0 ) )
			 );

		morph		= RpGeometryGetMorphTarget ( geo, 0 );
		triangle	= RpGeometryGetTriangles( geo );
		beginvtx	= RpMorphTargetGetVertices( morph );

		if(trinum)
		*ptbl = TriSurfArea ( &beginvtx[ triangle->vertIndex[0] ]
							, &beginvtx[ triangle->vertIndex[1] ]
							, &beginvtx[ triangle->vertIndex[2] ]
							);
		++ptbl;
		++triangle;
		for( i=1; i<trinum; ++i, ++triangle, ++ptbl )
		{
			*ptbl = TriSurfArea ( &beginvtx[ triangle->vertIndex[0] ]
								, &beginvtx[ triangle->vertIndex[1] ]
								, &beginvtx[ triangle->vertIndex[2] ]
								) + (*(ptbl-1));
		}
	}
}





// -----------------------------------------------------------------------------
const RwReal* GeoTriAreaTblBSearch(const RwReal pos, const RwReal tbl[], const RwInt32 num)
{
	RwInt32	low		= 0;
	RwInt32 high	= num;
	RwInt32	mid		= 0;

	RWFUNCTION(RWSTRING("GeoTriAreaTblBSearch"));
	RWASSERT( pos >= 0.f && pos <= tbl[num-1] );

	if( pos < 0.f && pos > tbl[num-1] )
		return NULL;

	while( low < high )
	{
		mid	= low + ( (high-low) / 2 );

		if( pos < tbl[mid] )
		{
			high = mid;
		}
		else if( pos > tbl[mid] )
		{
			low = mid+1;
			if( pos < tbl[low] )
				break;
		}
		else
			break;
	}

	return &tbl[mid];
}

// -----------------------------------------------------------------------------
RwInt32 GeoTriAreaTblGetTri(RwReal* outOffset
							, RwV3d outTri[]
							, const RpGeometry* geo
							, const RwReal pos)
{
	const RwReal* find	
		= GeoTriAreaTblBSearch
		(pos
		, GeoTriTblGetTbl(geo)->ptbl
		, RpGeometryGetNumTriangles(geo)
		);

	RwInt32			index		= (RwInt32)( find - GeoTriTblGetTbl(geo)->ptbl );
	RpMorphTarget*	morph		= RpGeometryGetMorphTarget ( geo, 0 );
	RpTriangle*		triangle	= RpGeometryGetTriangles( geo ) + index;
	RwV3d*			beginvtx	= RpMorphTargetGetVertices( morph );

	RWFUNCTION(RWSTRING("GeoTriAreaTblGetTri"));
	RWASSERT( outOffset && outTri && geo );
	RWASSERT( find );
	RWASSERT( morph && triangle && beginvtx );
	RWASSERT( index < RpGeometryGetNumTriangles(geo) );

	if( !find 
	 || !morph 
	 || !triangle 
	 || !beginvtx 
	 || index >= RpGeometryGetNumTriangles(geo) 
	 )
		return -1;//err

	*outOffset	= pos - GeoTriTblGetTbl(geo)->ptbl[index];
	if( index+1 < RpGeometryGetNumTriangles(geo) )
		++triangle;

	outTri[0]	= beginvtx[ triangle->vertIndex[0] ];
	outTri[1]	= beginvtx[ triangle->vertIndex[1] ];
	outTri[2]	= beginvtx[ triangle->vertIndex[2] ];

	RWASSERT( *outOffset <= TriSurfArea( outTri, outTri+1, outTri+2 ) );

	return 0;
}

void GeoTriAreaTblGet3DPos( RwV3d* out, const RwReal offset, const RwV3d tri[] )
{
	RwV3d	v0to1, v0to2, v1to2, vcrss;
	RwReal	pos	= offset * 2.f;

	RwReal	width, height;
	RwReal	wratio, hratio;
	RwReal	len0to1, len0to2, crsslen;

	RwV3dSub( &v0to1, &tri[1], &tri[0] );
	RwV3dSub( &v0to2, &tri[2], &tri[0] );
	RwV3dSub( &v1to2, &tri[2], &tri[1] );

	len0to1	= RwV3dLength( &v0to1 );
	len0to2	= RwV3dLength( &v0to2 );

	if( len0to1 == 0.f || len0to2 == 0.f )
		*out = tri[0];
	else{
		height	= pos / len0to2;
		height	= (float)floor(height);
		width	= pos - height * len0to2;
		
		RwV3dNormalize( &v0to1, &v0to1 );
		RwV3dNormalize( &v0to2, &v0to2 );	
		RwV3dCrossProduct( &vcrss, &v0to2, &v0to1 );
		crsslen = RwV3dLength( &vcrss );

		if( crsslen == 0.f )
			*out = tri[0];
		else
		{
			height	/= crsslen;

			wratio = width/len0to2;
			hratio = height/len0to1;
			if( (wratio + hratio) > 1.f )
			{
				height	= len0to1 * (1.f - hratio);
				width	= len0to2 * (1.f - wratio);
			}

			RwV3dScale( &v0to1, &v0to1, height );
			RwV3dScale( &v0to2, &v0to2, width );

			out->x = tri[0].x + v0to1.x + v0to2.x;
			out->y = tri[0].y + v0to1.y + v0to2.y;
			out->z = tri[0].z + v0to1.z + v0to2.z;
		}
	}
}


// -----------------------------------------------------------------------------
RwReal	RpAtomicGetSurfaceArea( RpAtomic* atom)
{
	RpGeometry*	geo;
	RwInt32 trinum;

	RWFUNCTION(RWSTRING("RpAtomicGetSurfaceArea"));

	if(!atom)
		return -1.f;	
	if(RpLODAtomicGetCurrentLOD ( atom ) > 0) 
		return -1.f;	
	if( (_rpGeometryTriAreaTblOffset) & 0x80000000 ) //_rpGeometryTriAreaTblOffset < 0
		return -1.f;

	geo = RpLODAtomicGetGeometry (atom, 0);		// 0단계 geometry return
	if (!geo)
	{
		geo = RpAtomicGetGeometry (atom);
		if (!geo)
			return -0.1f;
	}
	trinum = RpGeometryGetNumTriangles(geo);
	//if there isn't area table, build it
	if( !GeoTriTblGetTbl(geo)->ptbl )
		if( GeoTriTblAlloc(geo)->ptbl )
			GeoTriAreaTblBuildTbl(geo);
		else
		{
			RWASSERT( GeoTriTblAlloc(geo)->ptbl );
			return -1.f;
		}
	return GeoTriTblGetTbl(geo)->ptbl[trinum-1];
}

// -----------------------------------------------------------------------------
RwInt32	RpAtomicGet3DPos(RwV3d* out, RpAtomic* atom, RwReal posInArea)
{
	//RpGeometry*	geo	= RpAtomicGetGeometry (atom);

	RpGeometry*	geo;
	RwV3d	tri[3];	//the triangle
	RwReal	offset;	//position in the triangle's area
	RWFUNCTION(RWSTRING("RpAtomicGet3DPos"));
	
	if(!atom)
		return -1;	
	if(RpLODAtomicGetCurrentLOD ( atom ) > 0) 
		return -1;	
	if( (_rpGeometryTriAreaTblOffset) & 0x80000000 ) //_rpGeometryTriAreaTblOffset < 0
		return -1;

	geo = RpLODAtomicGetGeometry (atom, 0);		// 0단계 geometry return
	if (!geo)
	{
		geo = RpAtomicGetGeometry (atom);

		if (!geo)
			return 0;
	}
	
	RWASSERT( posInArea >= 0.f && posInArea <= RpAtomicGetSurfaceArea(atom) );

	//if there isn't area table, build it
	if( !GeoTriTblGetTbl(geo)->ptbl )
		if( GeoTriTblAlloc(geo)->ptbl )
			GeoTriAreaTblBuildTbl(geo);
		else
			return -1;//err
	
	if( GeoTriAreaTblGetTri(&offset, tri, geo, posInArea) < 0 )
		return -1;//err

	GeoTriAreaTblGet3DPos( out, offset, tri );

	return 0;
}

// -----------------------------------------------------------------------------
RpAtomic* CB_FindAtomic(RpAtomic* atom, void* pFindAtom)
{
	RwReal atomsurfacearea	= 0.f;
	stFindAtom*	pfind	= (stFindAtom*)pFindAtom;
	if( pfind->atom )
		return atom;//already found

	atomsurfacearea	= RpAtomicGetSurfaceArea(atom);
	if(pfind->posInAtomicArea <= atomsurfacearea)
		pfind->atom = atom;
	else
		pfind->posInAtomicArea -= atomsurfacearea;
	return atom;
}
// -----------------------------------------------------------------------------
RpAtomic* CB_GetAtomicSurface(RpAtomic* atom, void* pClumpSurfArea)
{
	RwReal	atomicarea	= RpAtomicGetSurfaceArea(atom);
	if( atomicarea < 0.f )
		return NULL;//err

	*(RwReal*)pClumpSurfArea	+= atomicarea;
	return atom;
}
// -----------------------------------------------------------------------------
RwReal	RpClumpGetSurfaceArea(RpClump* clump)
{
	RwReal	clumpSurfArea	= 0.f;
	
	if( (_rpGeometryTriAreaTblOffset) & 0x80000000 ) //_rpGeometryTriAreaTblOffset < 0
		return -1.f;

	RpClumpForAllAtomics( clump, CB_GetAtomicSurface, (void*)(&clumpSurfArea) );
	return clumpSurfArea;
}
// -----------------------------------------------------------------------------
RwInt32	RpClumpGet3DPos(RwV3d* out, RpClump* clump, RwReal posInArea)
{
	RwInt32		ir		= 0;
	stFindAtom	stfind	= { NULL, posInArea };

	RWFUNCTION(RWSTRING("RpClumpGet3DPos"));
	RWASSERT( posInArea >= 0.f && posInArea <= RpClumpGetSurfaceArea(clump) );

	if( (_rpGeometryTriAreaTblOffset) & 0x80000000 ) //_rpGeometryTriAreaTblOffset < 0
		return -1;

	RpClumpForAllAtomics( clump, CB_FindAtomic, (void*)(&stfind) );
	if( stfind.atom )
		ir = RpAtomicGet3DPos( out, stfind.atom, stfind.posInAtomicArea );
	return ir;
}

// -----------------------------------------------------------------------------
// AgcdGeoTriAreaTbl.cpp - End of file
// -----------------------------------------------------------------------------
