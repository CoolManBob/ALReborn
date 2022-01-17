#ifndef __ACU_CREATE_CLUMP__
#define __ACU_CREATE_CLUMP__

#include "apdefine.h"
#include <windows.h>

#include <rwcore.h>
#include <rpworld.h>

#if _MSC_VER < 1300
#ifdef _DEBUG
#pragma comment (lib , "AcuCreateClumpD")
#else
#pragma comment (lib , "AcuCreateClump")
#endif
#endif

#define D_ACC_360_TO_RADIAN								2 * 3.14
#define D_ACC_CREATE_CLUMP_MULTIPLY_CYLINDER_PRECISION	3
#define D_ACC_CREATE_CLUMP_DEFAULT_PRECISION			3

//@{ 2006/02/14 burumal
#define D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP				10
//@}

BOOL ACC_CREATE_CLUMP(CHAR *szDestFileName, AuBLOCKING *pastBlocking, INT32 lBlockingNum, INT32 lPrecision = D_ACC_CREATE_CLUMP_DEFAULT_PRECISION);

BOOL ACC_CREATE_BOX_CLUMP(CHAR *szFilePath, RwV3d *pstInf, RwV3d *pstSup);
BOOL ACC_CREATE_SPHERE_CLUMP(CHAR *szFilePath, RwV3d *pstPos, FLOAT fRadius, INT32 lIterations);
BOOL ACC_CREATE_CYLINER_CLUMP(CHAR *szFilePath, RwV3d *pstPos, FLOAT fHeight, FLOAT fRadius, INT32 lPrecision, FLOAT fOffset = D_ACC_360_TO_RADIAN);

BOOL ACC_CALC_VERTEX_NORMAL(RpClump *pstClump, CHAR *szFilePath, BOOL bReplaceNormals = TRUE);

#endif // __ACU_CREATE_CLUMP__