#pragma once

#include <rwcore.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rttiff.h>
#include <rtcharse.h>
#include <rpworld.h>
#include <rtquat.h>
#include <rphanim.h>
#include <rpskin.h>
#include <rpusrdat.h>
#include <rtintsec.h>
#include <rpcollis.h>
#include <rtpick.h>
#include <rtanim.h>
#include <rphanim.h>
#include <rpspline.h>
#include <rplodatm.h>
#include <rpmatfx.h>
#include <rpmorph.h>
#include <rtdict.h>
#include <rpuvanim.h>

#include "AcuRpUVAnimData.h"
#include "AcuRpMatD3DFx.h"

#include <rtfsyst.h>

//#include "AcuRpDMaterial.h"
#include "AcuRpMTexture.h"

#include "skeleton.h"

/* 
 * Warning: set MAX_NB_FILES_PER_FS to the appropriate value.
 *
 * MAX_NB_FILES_PER_FS limits the number of files that can be concurrently
 * opened by the file system(s) registered. 
 * On Windows a file size is 96 bytes. 
 */
#define MAX_NB_FILES_PER_FS (20)   

#define RWUTIL_PI								3.141592
#define RWUTIL_MAX_SUBSYSTEMS					16
#define RWUTIL_MAX_VIDEOMODE					50
#define RWUTIL_DEFAULT_VIEWWINDOW				(0.5f)
#define RWUTIL_RESOURCEDEFAULTARENASIZE			(4 << 20)
#define RWUTIL_SUBSYSTEM_DEFAULT				0
#define RWUTIL_DISPATCHSIZE						(1000)
#define RWUTIL_RENDER_CIRCLE_NUMPOINTS			15
#define RWUTIL_CLUMP_ARRAY_MAX					20


#ifdef WIDE_SCREEN
#define RWUTIL_DEFAULT_ASPECTRATIO (16.0f/9.0f)
#else
#define RWUTIL_DEFAULT_ASPECTRATIO (4.0f/3.0f)
#endif //  WIDE_SCREEN



/*#if (!defined(RsAssert))
#include <assert.h>
#define RsAssert(_condition) assert(_condition)
#endif

#define RSASSERT(_condition)                      \
do                                                \
{                                                 \
    if (!(_condition))                            \
    {                                             \
        _rwDebugSendMessage(rwDEBUGMESSAGE,       \
                            RWSTRING(__FILE__),   \
                            __LINE__,             \
                            "Assertion failed:",  \
                            #_condition);         \
    }                                             \
    RsAssert((_condition));                       \
}                                                 \
while (0)

#if (!defined(RSASSERT))
#define RSASSERT(_condition)
#endif

enum RsPrintPos
{
    rsPRINTPOSMIDDLE    = 0,

    rsPRINTPOSLEFT   = 1,
    rsPRINTPOSRIGHT  = 2,
    rsPRINTPOSTOP    = 4,
    rsPRINTPOSBOTTOM = 8,

    rsPRINTPOSTOPLEFT     = rsPRINTPOSTOP    | rsPRINTPOSLEFT,
    rsPRINTPOSTOPRIGHT    = rsPRINTPOSTOP    | rsPRINTPOSRIGHT,
    rsPRINTPOSBOTTOMLEFT  = rsPRINTPOSBOTTOM | rsPRINTPOSLEFT,
    rsPRINTPOSBOTTOMRIGHT = rsPRINTPOSBOTTOM | rsPRINTPOSRIGHT,

    rsPRINTPOSFORCENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RsPrintPos RsPrintPos;

enum RsPrintMargin
{
    rsPRINTMARGINMIDDLE = 0,

#if ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)))
    rsPRINTMARGINTOP    = 2,
    rsPRINTMARGINBOTTOM = 4,
    rsPRINTMARGINLEFT   = 5,
    rsPRINTMARGINRIGHT  = 5,
#else
    rsPRINTMARGINTOP    = 1,
    rsPRINTMARGINBOTTOM = 1,
    rsPRINTMARGINLEFT   = 1,
    rsPRINTMARGINRIGHT  = 1,
#endif
    rsPRINTMARGINFORCENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RsPrintMargin RsPrintMargin;*/

static RwV3d RWUTIL_XAXIS	= {1.0f, 0.0f, 0.0f};
static RwV3d RWUTIL_YAXIS	= {0.0f, 1.0f, 0.0f};
static RwV3d RWUTIL_ZAXIS	= {0.0f, 0.0f, 1.0f};
static RwV3d RWUTIL_ZEROVEC	= {0.0f, 0.0f, 0.0f};

static RwRGBA RWUTIL_RED		= {255,   0,   0, 255};
static RwRGBA RWUTIL_CYAN		= {0,   255, 255, 255};
static RwRGBA RWUTIL_GREEN		= {0,   255,   0, 255};
static RwRGBA RWUTIL_BLUE		= {0,     0, 255, 255};
static RwRGBA RWUTIL_YELLOEW	= {255, 255,   0, 255};

struct RwUtilAtomicIntersectParams
{
	RpIntersection   intersection;
	RpAtomic*        pickedAtomic;
	RwReal           minDistance;
};

struct RwUtilAtomicCompareVerticesParams
{
	RwReal		min_x;
	RwReal		min_y;
	RwReal		min_z;

	RwReal		max_x;
	RwReal		max_y;
	RwReal		max_z;
};

struct RwUtilClumpArray
{
	INT32		num;
	RpClump*	array[RWUTIL_CLUMP_ARRAY_MAX];
};

class RwUtilClumpFindAtomicIndexParams
{
public:
	RwUtilClumpFindAtomicIndexParams() : m_lAtomicIndex(-1), m_lCBCount(0), m_pstFindAtomic(NULL)
	{
	}

	INT32		m_lAtomicIndex;
	INT32		m_lCBCount;
	RpAtomic*	m_pstFindAtomic;
};

#define RWUTIL_GET_ATOMIC_MAX			10
class RwUtilClumpGetAtomicParams
{
public:
	RwUtilClumpGetAtomicParams() : m_lCount(0)
	{
		memset( m_pastAtomic, 0, sizeof( RpAtomic* ) * RWUTIL_GET_ATOMIC_MAX );
	}

	RpAtomic*	m_pastAtomic[RWUTIL_GET_ATOMIC_MAX];
	INT32		m_lCount;
};

class RwUtilClumpGetAtomicUDAInt32Params
{
public:
	RwUtilClumpGetAtomicUDAInt32Params()
	{
		m_pstFindAtomic	= NULL;
		m_pszUDAName	= NULL;
		m_lUDAInt32		= -1;
	}

	RwChar*		m_pszUDAName;
	RpAtomic*	m_pstFindAtomic;
	RwInt32		m_lUDAInt32;
};

#define RWUTIL_CLUMP_HAS_NUM_ATOMICS	100
class RwUtilClumpGetBBoxParams
{
public:
	RwUtilClumpGetBBoxParams()
	{
		memset( this, 0, sizeof( RwUtilClumpGetBBoxParams ) );
	}

	RwV3d*		m_pastVerticesArray[RWUTIL_CLUMP_HAS_NUM_ATOMICS];
	RwInt32		m_alVertices[RWUTIL_CLUMP_HAS_NUM_ATOMICS];
	INT32		m_lCount;
};

BOOL		RwUtilAtomicHasSkin(RpAtomic *pstAtomic);
VOID		RwUtilAtomicRenderAxis(RpAtomic *pstAtomic);
VOID		RwUtilAtomicRenderBoundingBox(RpAtomic *atomic);
VOID		RwUtilAtomicRenderWireMesh(RpAtomic *atomic, RwRGBA *Color);
RwV3d		RwUtilAverageV3d(RwV3d *pstV1, RwV3d *pstV2);
VOID		RwUtilClumpGetAtomics(RpClump *pstClump, RwUtilClumpGetAtomicParams *pstParams);
INT32		RwUtilClumpFindAtomicIndex(RpClump *clump, RpAtomic *find_atomic);
RwInt32		RwUtilClumpGetUDAInt(RpClump *clump, RpAtomic *find_atomic, CHAR *uda_name);
VOID		RwUtilClumpGetBoundingBox(RpClump **clump, RwInt32 count, RwBBox *box);
VOID		RwUtilClumpGetBoundingSphere(RpClump *pstClump, RwSphere *pstSphere);
RwInt32		RwUtilClumpGetWidth(RpClump *pstClump);
RwInt32		RwUtilClumpGetHeight(RpClump *pstClump);
RwInt32		RwUtilClumpGetRideHeight( RpClump *pstClump, RpHAnimHierarchy* pInHierarchy );
RwInt32		RwUtilClumpGetVertices(RpClump *pstClump);
RwInt32		RwUtilClumpGetTriangles(RpClump *pstClump);
BOOL		RwUtilClumpHasSkin(RpClump *pstClump);
BOOL		RwUtilCopyClumpFile(RpClump *pstClump, CHAR *szPath);
RpLight*	RwUtilCreateAmbientLight(RwRGBAReal color, RpWorld* world = NULL);
RpClump*	RwUtilCreateClump(CHAR *szDffPath, RpWorld *world = NULL);
RpLight*	RwUtilCreateDirectionalLight(RwReal x_rad, RwReal y_rad, RwRGBAReal color, RpWorld* world = NULL);
RpLight*	RwUtilCreateMainLight(RwRGBAReal color, RpWorld *world = NULL);
RpWorld*	RwUtilCreateEmptyWorld(RwBBox* box = NULL);
VOID		RwUtilDebugMessage(RwChar* strFunc, RwChar* strMsg);
VOID		RwUtilDebugMessageHandler(RwDebugType type __RWUNUSED__, const RwChar *str);
BOOL		RwUtilEqualV3d(RwV3d *pV3d, FLOAT x, FLOAT y, FLOAT z);
RwInt32		RwUtilGetAtomicUsrDataArrayInt32(RpAtomic *pstAtomic, CHAR *pszCmpUsrDatAryName);
RwV3d		RwUtilMakeV3d(FLOAT x, FLOAT y, FLOAT z);
RpAtomic*	RwUtilPickWorldNearestAtomic(RwCamera *camera, RpWorld * world, RwV2d *pixel);
RtCharset*	RwUtilPrintText(RtCharset* charset, RwChar* string, RwInt32 x, RwInt32 y, RsPrintPos pos);
VOID		RwUtilRenderPrimTypeLineList(RwIm3DVertex *ver, RwUInt32 count, RwMatrix *pstLTM = NULL);
VOID		RwUtilRenderSphere(RwSphere *sphere, RwMatrix *ltm = NULL, RwRGBA *rgba = NULL);
RwInt32		RwUtilCalcSphere(RpClump* clump, RwSphere* sphere, RwReal fscale = 1.f);
RwInt32		RwUtilCalcSphere(RpAtomic* atom, RwSphere* sphere, RwReal fscale = 1.f);
RpAtomic*	CBCalcSphere(RpAtomic* atom, void* ptInfo);


// Callback
RpAtomic*	RwUtilAtomicGetHeightCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilAtomicGetWidthCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilAtomicAddBSphereCentreCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilAtomicCompareBSphereCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilAtomicGetTrianglesCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilAtomicGetVerticesCB(RpAtomic *atomic, void *data);
RpCollisionTriangle	*RwUtilAtomicForAllTriangleIntersectionCB(RpIntersection *intersection __RWUNUSED__, RpCollisionTriangle *triangle, RwReal distance,  void *data);
RpAtomic*	RwUtilClumpFindAtomicIndexCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilClumpGetUDAIntCB(RpAtomic *atomic, void *data);
RpAtomic*	RwUtilClumpHasSkinCB(RpAtomic *atomic, void *data);
RpGeometry*	RwUtilClumpHasSkinLODAtomicCB(RpGeometry *geom, void *data);
RpLight*	RwUtilDestroyLightCB(RpLight* light, void* data);
RpAtomic*	RwUtilWorldForAllAtomicIntersectLineCB(RpIntersection *intersection __RWUNUSED__, RpWorldSector *sector __RWUNUSED__, RpAtomic *atomic,  RwReal distance __RWUNUSED__, void *data);

VOID		RwUtilRenderUnitSphere(int iterations, FLOAT fRadius);
