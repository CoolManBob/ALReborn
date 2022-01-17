// AcUtils.cpp
/////////////////////////////////////
//#include "stdafx.h"
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>
#include "AgcEngine.h"
//#include "MagDebug.h"


////////////////////////////////////////////////////////////////////
// Function : RsRasterReadFromBitmap
// Return   : RwRaster *
// Created  : 마고자 (2002-04-22 오전 11:30:09)
// Parameter: 파일이름
// Note     : bitmap 파일을 읽어서 image로 만든후 레스터로 변환해줌.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
RwRaster * RsRasterReadFromBitmap( char * filename )
{
	RwImage		*pBmp		;
	RwRaster	*pRaster	;
	// 룰룰루 ...;;;
	pBmp = RwImageRead( filename );
	if( !pBmp )
	{
//		// Failed to read
//		#ifdef ALEF_UNDER_MFC_FRAMEWORKS
//		::MessageBox( NULL , "Error Reading Bitmap." , "AlefEngine" , MB_OK );
//		#else
//        RsErrorMessage(RWSTRING("Error Reading Bitmap."));
//		#endif

        return NULL;
	}

	pRaster	= RwRasterCreate(
		RwImageGetWidth	( pBmp ),
		RwImageGetHeight( pBmp ),
		0 ,
		rwRASTERTYPETEXTURE  );	

	if( RwRasterSetFromImage( pRaster , pBmp ) == NULL )
	{
		// Failed to read
		#ifdef ALEF_UNDER_MFC_FRAMEWORKS
		::MessageBox( NULL , "Error Converting Bmp." , "AlefEngine" , MB_OK );
		#else
        RsErrorMessage(RWSTRING("Error Converting Bmp."));
		#endif
        return NULL;
	}

	if( pBmp )
	{
		RwImageDestroy( pBmp );
	}

	return pRaster;
}


////////////////////////////////////////////////////////////////////
// Function : DffLoad
// Return   :
// Created  : 마고자 (2002-04-26 오전 11:19:04)
// Parameter: 
// Note     : DFF 클럼프 파일 로드.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
RpClump * DffLoad(RwChar *filename)
{
    RwStream    *stream = NULL;
    RpClump     *clump = NULL;
    RwChar	    *pathName;

    /* Open stream */
    pathName = RsPathnameCreate(filename);
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, pathName);
    if (stream)
    {
        /* Find clump chunk */
        if (RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL))
        {
            /* Load clump */
            clump = RpClumpStreamRead(stream);
        }

        /* close the stream */
        RwStreamClose( stream, NULL );
    }
    RsPathnameDestroy(pathName);

    return (clump);
}

////////////////////////////////////////////////////////////////////
// Function : AtomicGetBBox
// Return   :
// Created  : 마고자 (2002-04-26 오전 11:19:49)
// Parameter: 
// Note     : Atomic 에서 Bounding Box를 얻어냄.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
void AtomicGetBBox(RpAtomic *atom, RwBBox *box)
{
    RpGeometry    *geom   = RpAtomicGetGeometry(atom);
    RwInt32        nVerts = RpGeometryGetNumVertices(geom);
    RpMorphTarget *morph  = RpGeometryGetMorphTarget(geom, 0);
    RwV3d         *verts  = RpMorphTargetGetVertices(morph);
    RwUInt16       i;

    box->inf = *verts;
    box->sup = *verts;

    for (verts++, i = 1; i < nVerts; ++i, ++verts)
    {
        if (verts->x < box->inf.x) box->inf.x = verts->x;
        if (verts->y < box->inf.y) box->inf.y = verts->y;
        if (verts->z < box->inf.z) box->inf.z = verts->z;
        if (verts->x > box->sup.x) box->sup.x = verts->x;
        if (verts->y > box->sup.y) box->sup.y = verts->y;
        if (verts->z > box->sup.z) box->sup.z = verts->z;
    }

    RwV3dScale(&box->sup, &box->sup, 1.09f);
    RwV3dScale(&box->inf, &box->inf, 1.09f);
}


////////////////////////////////////////////////////////////////////
// Function : ClumpGetBBox
// Return   :
// Created  : 마고자 (2002-08-26 오전 11:50:51)
// Parameter: 
// Note     : 
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
RpAtomic *	GetBBoxRpAtomicCallBack		( RpAtomic * atomic, void *data	);

void		ClumpGetBBox				( RpClump *pClump , RwBBox *box )
{
	box->inf.x	= 0.0f;
	box->inf.x	= 0.0f;
	box->inf.x	= 0.0f;
	box->sup.x	= 0.0f;
	box->sup.x	= 0.0f;
	box->sup.x	= 0.0f;

	RpClumpForAllAtomics( pClump , GetBBoxRpAtomicCallBack , ( void * ) box );

    RwV3dScale( &box->sup , &box->sup, 1.09f);
    RwV3dScale( &box->inf , &box->inf, 1.09f);
}

RpAtomic   * GetBBoxRpAtomicCallBack(RpAtomic * atomic, void *data)
{
	ASSERT( NULL != atomic );

	RwBBox			*pBBox	= ( RwBBox * ) data							;
    RpGeometry		*geom   = RpAtomicGetGeometry		( atomic	)	;

	// 마고자 (2004-05-31 오후 4:46:11) : 널체크.
	if( NULL == geom ) return atomic;

    RwInt32			nVerts	= RpGeometryGetNumVertices	( geom		)	;
    RpMorphTarget	*morph	= RpGeometryGetMorphTarget	( geom , 0	)	;
    RwV3d			*verts	= RpMorphTargetGetVertices	( morph		)	;

    RwUInt16		i;

	if(	pBBox->inf.x	== 0.0f &&
		pBBox->inf.x	== 0.0f &&
		pBBox->inf.x	== 0.0f &&
		pBBox->sup.x	== 0.0f &&
		pBBox->sup.x	== 0.0f &&
		pBBox->sup.x	== 0.0f )
	{
		pBBox->inf = *verts;
		pBBox->sup = *verts;
	}

    for (verts++, i = 1; i < nVerts; ++i, ++verts)
    {
        if (verts->x < pBBox->inf.x) pBBox->inf.x = verts->x;
        if (verts->y < pBBox->inf.y) pBBox->inf.y = verts->y;
        if (verts->z < pBBox->inf.z) pBBox->inf.z = verts->z;
        if (verts->x > pBBox->sup.x) pBBox->sup.x = verts->x;
        if (verts->y > pBBox->sup.y) pBBox->sup.y = verts->y;
        if (verts->z > pBBox->sup.z) pBBox->sup.z = verts->z;
    }

	return atomic;
}
////////////////////////////////////////////////////////////////////
// Function : DffSave
// Return   :
// Created  : 마고자 (2002-04-26 오전 11:20:27)
// Parameter: 
// Note     : 클럼프를 저장하는 함수.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
void DffSave(RpAtomic *atomic, char *filename)
{
    if (atomic)
    {
        RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, filename);
        RpClump  *clump  = RpAtomicGetClump(atomic);

        if (stream && clump)
        {
            RpClumpStreamWrite(clump, stream);
            RsWarningMessage("Wrote model!");

            RwStreamClose(stream, NULL);
        }
        else
        {
            RsWarningMessage("Couldn't open stream!");
        }
    }
    else
    {
        RsWarningMessage("No atomic to export!");
    }
}

////////////////////////////////////////////////////////////////////
// Function : BspLoad
// Return   :
// Created  : 마고자 (2002-04-26 오전 11:20:38)
// Parameter: 
// Note     : BSP 월드 데이타를 로딩하기위한 유틸리티 함수.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
RpWorld * BspLoad(RwChar *filename)
{
    RpWorld  *world = NULL;
    RwStream *stream = NULL;
    RwChar	 *pathName;

    pathName = RsPathnameCreate(filename);

    /* try to read in the stream and treat as a BSP */
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, pathName);
    if (stream)
    {
        /* Find clump chunk */
        if (RwStreamFindChunk(stream, rwID_WORLD, NULL, NULL))
        {
            /* Load the world */
            world = RpWorldStreamRead(stream);
        }

        /* close the stream */
        RwStreamClose( stream, NULL );
    }

    RsPathnameDestroy(pathName);

    return world;
}
