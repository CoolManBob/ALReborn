////////////////////////////////////////////////////////////////////
// Function : AcUtils
// Return   :
// Created  : 마고자 (2002-04-22 오전 11:27:32)
// Parameter: 
// Note     : 
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////

#if !defined( ACUTILS_H_ )
#define ACUTILS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <rwcore.h>
#include <rpworld.h>

RwRaster *	RsRasterReadFromBitmap		( char * filename );

RpClump *	DffLoad						( RwChar *filename );
void		DffSave						( RpAtomic *atomic, char *filename );

void		AtomicGetBBox				( RpAtomic *atom, RwBBox *box );
void		ClumpGetBBox				( RpClump *pClump , RwBBox *box );
RpWorld *	BspLoad						( RwChar *filename );

#endif // ACUTILS_H_
