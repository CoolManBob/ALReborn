//@{ Jaewon 20041209
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   AcuRtAmbOcclMap.h                                          -*
 *-                                                                         -*
 *-  Purpose :   RtAmbOcclMap toolkit public API                            -*
 *-                                                                         -*
 *===========================================================================*/

/**
 * \defgroup rtambocclmap RtAmbOcclMap
 * \ingroup lighting
 *
 * AmbOcclMap Generation Toolkit for RenderWare.
 */

#ifndef __RTAMBOCCLMAP_H__
#define __RTAMBOCCLMAP_H__

/*===========================================================================*
 *--- Includes --------------------------------------------------------------*
 *===========================================================================*/

#include "rwcore.h"
#include "rpworld.h"

#include "AcuRpMatD3DFx.h"

/*===========================================================================*
 *--- Global Defines --------------------------------------------------------*
 *===========================================================================*/

#define rwID_AMBOCCLMAPPLUGIN (MAKECHUNKID(0xfffff0L,0x09))

#define RPAMBOCCLMAPMATERIALGETDATA(_mtrl)                             \
     ((AmbOcclMapMaterialData *)(((RwUInt8 *)(_mtrl)) +                \
                  _rpAmbOcclMapGlobals.materialOffset))

#define RPAMBOCCLMAPMATERIALGETCONSTDATA(_mtrl)                        \
     ((const AmbOcclMapMaterialData *)(((const RwUInt8 *)(_mtrl)) +    \
                  _rpAmbOcclMapGlobals.materialOffset))

#define RPAMBOCCLMAPATOMICGETDATA(_atmc)                               \
     ((AmbOcclMapObjectData *)(((RwUInt8 *)(_atmc)) +                  \
                  _rpAmbOcclMapGlobals.atomicOffset))

#define RPAMBOCCLMAPATOMICGETCONSTDATA(_atmc)                          \
     ((const AmbOcclMapObjectData *)(((const RwUInt8 *)(_atmc)) +      \
                  _rpAmbOcclMapGlobals.atomicOffset))

#define RPAMBOCCLMAPGEOMETRYGETDATA(_gmtr)                             \
     ((AmbOcclMapGeometryData *)(((RwUInt8 *)(_gmtr)) +                \
                  _rpAmbOcclMapGlobals.geometryOffset))

#define RPAMBOCCLMAPGEOMETRYGETCONSTDATA(_gmtr)                        \
     ((const AmbOcclMapGeometryData *)(((const RwUInt8 *)(_gmtr)) +    \
                  _rpAmbOcclMapGlobals.geometryOffset))

#define RPAMBOCCLMAPWORLDGETDATA(_wrld)							       \
     ((AmbOcclMapWorldData *)(((RwUInt8 *)(_wrld)) +			       \
                  _rpAmbOcclMapGlobals.worldOffset))

#define RPAMBOCCLMAPWORLDGETCONSTDATA(_wrld)                           \
     ((const AmbOcclMapWorldData *)(((const RwUInt8 *)(_wrld)) +       \
                  _rpAmbOcclMapGlobals.worldOffset))

#define rpAMBOCCLMAPUNUSEDALPHA       ((RwUInt8)0)
#define rpAMBOCCLMAPGREEDILYUSEDALPHA ((RwUInt8)1)
#define rpAMBOCCLMAPUSEDALPHA         ((RwUInt8)2)

/* Used during lightmap illumination (sliver triangles are skipped
 * (their texels should be filled by dilate()), because their normals
 * can't be accurately calculated) */
#define rpAMBOCCLMAPDEFAULTSLIVERAREATHRESHOLD (0.001f)

/* Used during lightmap UV calculation (polySets may be
 * joined on the basis of vertices with equal positions) */
#define rpAMBOCCLMAPDEFAULTVERTEXWELDTHRESHOLD (0.1f)

#define rpAMBOCCLMAPDEFAULTAMBOCCLMAPSIZE 128
#define rpAMBOCCLMAPMINAMBOCCLMAPSIZE     16
#define rpAMBOCCLMAPMAXAMBOCCLMAPSIZE     512/*?? any better way of determining this ??*/

#define rpAMBOCCLMAPDEFAULTRAYCOUNT		  128	
#define rpAMBOCCLMAPMAXPREFIXSTRINGLENGTH 4

/* Misc flags for used in the world plugin data. */
#define rpAMBOCCLMAPWORLDFLAGAMBOCCLMAP        0x01

#define RPLTMAPSAMPLEMAPSET(_map, _x, _y, _s)        \
    (_map)[((_y) * (_s) + (_x)) >> 0x03] |= (1 << (((_y) * (_s) + (_x)) & 0x07))

#define RPLTMAPSAMPLEMAPUNSET(_map, _x, _y, _s)        \
    (_map)[((_y) * (_s) + (_x)) >> 0x03] &= ~(1 << (((_y) * (_s) + (_x)) & 0x07))

#define RPLTMAPSAMPLEMAPGET(_map, _x, _y, _s)        \
    ((_map)[((_y) * (_s) + (_x)) >> 0x03] & (1 << (((_y) * (_s) + (_x)) & 0x07)))

#define RPLTMAPSAMPLEMAPSIZE(_x, _y) \
    ((((_x) * (_y)) + 0x07) >> 0x03)

/*===========================================================================*
 *--- Plugin Types ----------------------------------------------------------*
 *===========================================================================*/

enum RpAmbOcclMapStyle
{
    rpAMBOCCLMAPSTYLENASTYLE          = 0x0,

    rpAMBOCCLMAPSTYLERENDERBASE       = 0x1, /**< The base texture should be rendered */
    rpAMBOCCLMAPSTYLERENDERAMBOCCLMAP = 0x2, /**< The lightmap should be rendered */
    rpAMBOCCLMAPSTYLEPOINTSAMPLE      = 0x4, /**< The lightmap should be point-sampled */

	//@{ Jaewon 20050408
	rpAMBOCCLMAPSTYLEINDOOR           = 0x8,
	//@} Jaewon

    rpAMBOCCLMAPSTYLEFORCEENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RpAmbOcclMapStyle RpAmbOcclMapStyle;

typedef struct AmbOcclMapGlobals AmbOcclMapGlobals;
struct AmbOcclMapGlobals
{
    /* Offsets */
    RwInt32      atomicOffset;
    RwInt32      geometryOffset;
    RwInt32      worldOffset;
    RwInt32      materialOffset;

	RwModuleInfo module;
    RwUInt32     lightMapSize;            /* The current default on-creation lightmap resolution.
                                           * Overridden by per-object values */
    RwUInt32     renderStyle;             /* The current RpLtMapStyle */
    RwChar       prefix[rpAMBOCCLMAPMAXPREFIXSTRINGLENGTH + 1]; /* Default prefix string for lightmaps */
    RwUInt32     lightMapCount;
    RwReal       sliverAreaThreshold;     /* Used during lightmap illumination (sliver triangles are
                                           * skipped (their texels should be filled by dilate()),
                                           * because their normals can't be accurately calculated) */
    RwReal       vertexWeldThreshold;     /* Used during lightmap UV calculation (polySets may be
                                           * joined on the basis of vertices with equal positions) */
    RwUInt32     rasFormat;               /* Format used for creating lightmap rasters. */
	
	RwUInt32	 numRays;				  /* number of hemisphere rays used to compute accessibility & average unoccluded direction */
};

typedef struct rpLtMapSampleMap rpLtMapSampleMap;
struct rpLtMapSampleMap
{
    RwUInt32    refCount;
    RwUInt8     *samplePixels;
};
typedef struct AmbOcclMapObjectData AmbOcclMapObjectData;
struct AmbOcclMapObjectData
{
    RwUInt32   flags;      /* See \ref RtLtMapObjectFlags */
    RwUInt32   numSamples; /* Number of lightmap texels used to light this object (does
                            * NOT include vertex prelights) - used in progressCBs  */
    RwUInt32   lightMapSize; /* Width of lightmap texture to create for this object.
                              * object - used in progressCBs. Zero means use
                              * the global default value on creation. */
    RwUInt8    clearCol;
    RwRGBA     color;
    RwTexture *lightMap;

    rpLtMapSampleMap    *sampleMap;
};

typedef struct AmbOcclMapObjectStreamData AmbOcclMapObjectStreamData;
struct AmbOcclMapObjectStreamData
{
    RwUInt32 flags;        /* See \ref RtLtMapObjectFlags */
    RwUInt32 numSamples;   /* Number of lightmap texels used to light this object (does
                            * NOT include vertex prelights) - used in progressCBs */
    RwUInt32 lightMapSize; /* Saved per-object in case lightmaps are missing on load and
                            * so blank lightmaps of the appropriate size need creating */
    RwChar   lightMapName[rwTEXTUREBASENAMELENGTH]; /* The name of this atomic's lightmap */
};

typedef struct AmbOcclMapGeometryData
{
    struct LtMapPolySet *PolySetArray;     /* Set by LtMapObjectAllocateUV ().  Stops
                                     * shared geometry being re-lightmapped. */
    RwInt32      numSets;           /* Only valid if PolySetArray != NULL */
} AmbOcclMapGeometryData;
typedef AmbOcclMapGeometryData AmbOcclMapGeometryStreamData; /* File/memory are identical */

typedef struct AmbOcclMapWorldData AmbOcclMapWorldData;
struct AmbOcclMapWorldData
{
    RwUInt32    flag;            /* General purpose flag. */
    RwReal      lightMapDensity; /* World-space lightmap sample density */
};

typedef struct AmbOcclMapWorldStreamData AmbOcclMapWorldStreamData;
struct AmbOcclMapWorldStreamData
{
    RwReal   lightMapDensity; /* World-space lightmap sample density, multiplied by a per-material value */
};

typedef struct AmbOcclMapMaterialData AmbOcclMapMaterialData;
struct AmbOcclMapMaterialData
{
    RwUInt32 flags;            /* See RtLtMapMaterialFlags */
    RwReal   lightMapDensity;  /* Multiplies the world's lightmap density, useful
                                * to get large floor areas sampled at a lower rate */
};

/* Memory/file structures are identical as of yet */
typedef struct AmbOcclMapMaterialData AmbOcclMapMaterialStreamData;

/*===========================================================================*
 *--- Global Types ----------------------------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapIlluminateSampleCallBack is the callback to be called, from
 * within \ref RtAmbOcclMapIlluminate, for groups of samples in the objects
 * currently being illuminated.
 *
 * For lightmapped objects, samples are grouped on a per-polygon basis and
 * for vertex-lit objects, samples are grouped on a per-object basis (see
 * \ref RtAmbOcclMapObjectFlags).
 *
 * This callback will receive an array of color values to fill in, each
 * representing one sample in the current object - this may correspond to
 * a texel in the current object's lightmap or the prelight color of a
 * vertex, depending on whether the object is lightmapped and/or vertex-lit.
 * It will receive positions (in world-space) for each sample and the normal
 * vector (again, in world-space) of each sample (normals are interpolated
 * across polygons for non-flat-shaded materials. See \ref RtAmbOcclMapMaterialFlags).
 * For lightmap samples (not vertex-lighting samples), it will receive
 * barycentric coordinates within the current polygon.  It will also receive
 * a list of RpLights affecting the current object.
 *
 * The barycentric coordinates may be used, for example, to allow a callback
 * to easily import existing lighting data (e.g from previously generated
 * lightmaps in a different format, or from an art package with lighting
 * functionality).
 *
 * NOTE: The alpha channel of the RwRGBA results array must NOT be modified.
 * These values are used internally and their modification may result in
 * unwanted visual artifacts in the resulting lighting solution.
 *
 * The default RtAmbOcclMapIlluminateSampleCallBacks supplied with RtAmbOcclMap is
 * \ref RtAmbOcclMapDefaultSampleCallBack. This callback performs point and area
 * lighting (the area lights use are those passed to \ref RtAmbOcclMapIlluminate).
 *
 * \param  results          A pointer to an array of \ref RwRGBA sample color values
 * \param  samplePositions  A pointer to an array of \ref RwV3d values specifying the
 *                          world-space positions of each of the samples in the results array
 * \param  baryCoords       A pointer to an array of \ref RwV3d values specifying the
 *                          barycentric coordinates (within the current polygon) of
 *                          each of the samples in the results array
 * \param  numSamples       The length of the results, samplePositions, baryCoords and normals arrays
 * \param  lights           An array of pointers to \ref RpLight's affecting the current object
 * \param  numLights        The length of the lights array
 * \param  normals          A pointer to an array of \ref RwV3d values specifying the
 *                          world-space, unit normals of each of the samples in the results array
 *
 * \return A pointer to the results array on success, NULL otherwise
 *
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapIlluminateVisCallBack
 * \see RtAmbOcclMapIlluminateProgressCallBack
 */
typedef RwRGBA *(*RtAmbOcclMapIlluminateSampleCallBack)(RwRGBA        *results,
														RwV3d         *samplePositions,
														RwV3d         *baryCoords,
														RwUInt32       numSamples,
														RpLight      **lights,
														RwUInt32       numLights,
														RwV3d         *normals);

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapIlluminateVisCallBack is the callback to be called, from
 * within \ref RtAmbOcclMapIlluminate, to determine the visibility between a
 * sample and a light.
 *
 * This callback is called for all samples in the current
 * \ref RtAmbOcclMapLightingSession and for each light source which may
 * potentially affect each of those samples (this may not be all the
 * lights in the scene, as some hierarchical culling is performed).
 * Each sample may represent a texel in the current object's lightmap
 * or the prelight color of a vertex, depending on whether the object
 * is lightmapped and/or vertex-lit (see \ref RtAmbOcclMapObjectFlags).
 *
 * The callback will receive a pointer to the world of the current
 * \ref RtAmbOcclMapLightingSession (this may be used to perform intersection
 * tests), the world-space position of the sample, the world-space
 * position of the light, a pointer to a light and a pointer to an
 * \ref RwRGBAReal result value.
 *
 * If the light pointer is NULL, this means that the current light
 * is an area light (as opposed to an \ref RpLight), of an internal
 * format. The area lights use are those passed to \ref RtAmbOcclMapIlluminate.
 *
 * The callback should return FALSE to signify that the light is wholly
 * occluded w.r.t the sample position, otherwise it should return TRUE.
 * Partial degrees of (color-frequency-dependent) occlusion may be
 * expressed by modifying the RwRGBAReal value. This defaults to bright
 * white but may be reduced to signify that the light from the light
 * source should be attenuated. This could be used to take into account
 * light-filtering objects in the scene (such as colored glass or fog).
 *
 * The default RtAmbOcclMapIlluminateVisCallBack supplied with RtAmbOcclMap is
 * \ref RtAmbOcclMapDefaultVisCallBack. This callback performs visibility
 * tests using the line-intersection tests from \ref rtintersection. It tests
 * for occlusion by RpWorldSectors and RpAtomics and it respects the
 * relevant \ref RtAmbOcclMapObjectFlags and \ref RtAmbOcclMapMaterialFlags but it
 * does not filter light; visibility is determined to be either one or zero.
 *
 * \param  world      The world of the current RtAmbOcclMapLightingSession
 * \param  result     An RwRGBAReal value to attentuate this light's
 *                    contribution to the current sample
 * \param  samplePos  The world-space positiuon of the sample
 * \param  lightPos   The world-space positiuon of the light
 * \param  light      A pointer to the light (NULL if it is an are light)
 *
 * \return TRUE if the light is visible from the sample, FALSE if it is occluded
 *
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapIlluminateSampleCallBack
 * \see RtAmbOcclMapIlluminateProgressCallBack
 */
//@{ Jaewon 20050408
// 't'(parametric value for the intersection) parameter added
typedef RwBool (*RtAmbOcclMapIlluminateVisCallBack)(RpWorld    *world,
													RwRGBAReal *result,
													RwV3d      *samplePos,
													RwV3d      *lightPos,
													RpLight    *light,
													RwReal     *t);
//@} Jaewon

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapIlluminateProgressCallBack is the callback to be called, from
 * within \ref RtAmbOcclMapIlluminate, to allow a user to track lighting progress.
 *
 * The progress callback will be called at several stages during lighting,
 * with a different 'message' parameter value used at each stage (see
 * \ref RtAmbOcclMapProgressMessage). It will be called at the very start of
 * lighting (for a given \ref RtAmbOcclMapLightingSession), before any samples
 * are lit. It will also be called at the very end of lighting, after all
 * samples have been lit. It will be called before and after each lighting
 * 'slice' (see \ref RtAmbOcclMapIlluminate) and also after each group of
 * samples have been lit.
 *
 * For lightmapped objects, samples are grouped on a per-polygon basis and
 * for vertex-lit objects, samples are grouped on a per-object basis (see
 * \ref RtAmbOcclMapObjectFlags).
 *
 * The progress callback will receive a RwReal value specifying the percentage
 * of samples already lit in the current \ref RtAmbOcclMapLightingSession (see
 * \ref RtAmbOcclMapLightingSessionGetNumSamples).
 *
 * By returning FALSE, the progress callback may cause early termination of
 * the current lighting 'slice' (this may be used, for example, to keep
 * the time spent lighting each slice fairly constant).
 *
 * There is no default progress callback supplied with RtAmbOcclMap.
 *
 * \param  message  A \ref RtAmbOcclMapProgressMessage identifying the stage
 *                  of the current call to the progress callback
 * \param  value    The percentage of samples already lit in the
 *                  current \ref RtAmbOcclMapLightingSession
 *
 * \return FALSE to immediately terminate lighting, otherwise TRUE
 *
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapIlluminateSampleCallBack
 * \see RtAmbOcclMapIlluminateVisCallBack
 */
typedef RwBool (*RtAmbOcclMapIlluminateProgressCallBack)(RwInt32 message,
                                                    RwReal value);


/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapAmbOcclMapProcessCallBack is the callback to be called, from
 * within \ref RtAmbOcclMapLightingSessionAmbOcclMapProcess, to allow a lightmap
 * to be process after a lighting session.
 *
 * For some platform, the lightmap needs to be converted before it can be
 * used during rendering. This function allows a post process operation to
 * be performed on the lightmaps before it can be used.
 *
 * The actual operation is platform specific.
 *
 * The callback is only called for lightmaps that were updated from the
 * last lighting session.
 *
 * \param  lightmap A pointer to the input lightmap data.
 *
 * \return A pointer to the lightmap on success, NULL otherwise.
 *
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapLightingSessionAmbOcclMapProcess
 * \see RtAmbOcclMapGetAmbOcclMapProcessCallBack
 * \see RtAmbOcclMapSetAmbOcclMapProcessCallBack
 */
typedef RwTexture *(*RtAmbOcclMapAmbOcclMapProcessCallBack)(RwTexture *lightmap);

/*===========================================================================*
 *--- Toolkit Types ---------------------------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapProgressMessage is an enumerated type identifying the different
 * stages at which the \ref RtAmbOcclMapIlluminateProgressCallBack may be called
 * from within \ref RtAmbOcclMapIlluminate.
 *
 * \see RtAmbOcclMapIlluminateProgressCallBack
 * \see RtAmbOcclMapIlluminate
 */
enum RtAmbOcclMapProgressMessage
{
    rtAMBOCCLMAPPROGRESSNAMESSAGE = 0,

    rtAMBOCCLMAPPROGRESSSTART      = 1, /**< This is issued at the beginning of
										 *   an incremental lighting session */
    rtAMBOCCLMAPPROGRESSSTARTSLICE = 2, /**< This is issued at the beginning of every
										 *   slice in an incremental lighting session */
    rtAMBOCCLMAPPROGRESSUPDATE     = 3, /**< This is issued after the lighting of each
										 *   lightmapped triangle or vertex-lit object */
    rtAMBOCCLMAPPROGRESSENDSLICE   = 4, /**< This is issued at the end of every slice
										 *   in an incremental lighting session */
    rtAMBOCCLMAPPROGRESSEND        = 5, /**< This is issued at the end of an
										 *   incremental lighting session */

    rtAMBOCCLMAPPROGRESSFORCEENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RtAmbOcclMapProgressMessage RtAmbOcclMapProgressMessage;

typedef struct RtAmbOcclMapLightingSession RtAmbOcclMapLightingSession;
/**
 * \ingroup rtambcocclmap
 * \struct RtAmbOcclMapLightingSession
 * The RtAmbOcclMapLightingSession structure holds information to be passed to
 * \ref RtAmbOcclMapIlluminate. It is used to parameterize the lighting process.
 *
 * The RtAmbOcclMapLightingSession structure encapsulates a set of objects and
 * keeps track of the proportion of samples, within that set, that have already
 * been lit by calls to \ref RtAmbOcclMapIlluminate. Each call performs lighting for
 * one 'slice' of the whole 'session'. If the camera member is non-NULL, it is
 * important that the camera is not moved between lighting slices.
 *
 * The RtAmbOcclMapLightingSession is also passed to
 * \ref RtAmbOcclMapAmbOcclMapsCreate, \ref RtAmbOcclMapAmbOcclMapsClear,
 * \ref RtAmbOcclMapAmbOcclMapsDestroy and \ref RtAmbOcclMapAreaLightGroupCreate,
 * though not all of the session structure's member will be used in
 * each of these cases.
 *
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapAmbOcclMapsCreate
 * \see RtAmbOcclMapAmbOcclMapsClear
 * \see RtAmbOcclMapAmbOcclMapsDestroy
 * \see RtAmbOcclMapAreaLightGroupCreate
 */
struct RtAmbOcclMapLightingSession
{
    RpWorld        *world;       /**< This world is that in which collisions are performed
                                  *   during illumination (for the purposes of lighting
                                  *   visibility determination) */
    RwCamera       *camera;      /**< An optional pointer to a camera. The camera's frustum
                                  *   may be used to cull objects and/or triangles from the
                                  *   set of those to be processed. */
    RpAtomic      **atomicList;  /**< An optional array of \ref RpAtomic pointers,
                                  *   specifying which atomics to light. If this is NULL
                                  *   then all atomics in the world  (or those inside the
                                  *   optional camera's frustum) will be lit. */
    RwInt32         numAtomics;  /**< The length of the atomicList array. If this is set to
                                  *   '-1' then none of the atomics in the world will be lit. */
    RwUInt32        totalSamples;/**< This specifies how many lightmap samples will be lit in
                                  *   total for the world specified (this is filled in by
                                  *   \ref RtAmbOcclMapIlluminate, not the calling function).
                                  */
    RwUInt32        startObj;    /**< Lighting for the current slice should begin with this
                                  *   object. It is the user's responsibility to increment this
                                  *   value after each slice to process the next slice of objects.
                                  */
    RwUInt32        numObj;      /**< This specifies how many objects should be processed for the
                                  * current slice.
                                  * If it is set to zero, then all objects will be processed.
                                  */
    RwUInt32        totalObj;    /**< This specifies how many objects needs to be processed
                                  * for the current session.
                                  * Internal use only.
                                  */

    RwUInt32        numTriNVert;      /**< Internal use only. */
    RwUInt32        totalTriNVert;    /**< Internal use only. */
    RwReal          invTotalTriNVert; /**< Internal use only. */

    RwImage        *lightMapImg; /**< Internal use only. */
    RwTexture      *lightMap;    /**< Internal use only. */
    rpLtMapSampleMap *sampleMap;   /**< Internal use only. */
    RwUInt32        superSample; /**< This specifies the supersampling value for this current
                                  *   lighting session. */
    RtAmbOcclMapIlluminateSampleCallBack   sampleCallBack;   /**< A \ref RtAmbOcclMapIlluminateSampleCallBack
															  *     to use during lighting. If this is left
															  *     NULL, the default callback will be used. */
    RtAmbOcclMapIlluminateVisCallBack      visCallBack;      /**< A \ref RtAmbOcclMapIlluminateVisCallBack
															  *     to use during lighting. If this is left
															  *     NULL, the default callback will be used. */
    RtAmbOcclMapIlluminateProgressCallBack progressCallBack; /**< A \ref RtAmbOcclMapIlluminateProgressCallBack
															  *     to use during lighting. If this is left
															  *     NULL, no progress callback will be used. */
};

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapMaterialFlags is an enumerated type specifying the different
 * lightmap-related flags which may be applied to materials. These values
 * will be taken into consideration within \ref RtAmbOcclMapIlluminate.
 *
 * \see RtAmbOcclMapMaterialGetFlags
 * \see RtAmbOcclMapMaterialSetFlags
 * \see RtAmbOcclMapMaterialGetAmbOcclMapDensityModifier
 * \see RtAmbOcclMapMaterialSetAmbOcclMapDensityModifier
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapIlluminateVisCallBack
 */
enum RtAmbOcclMapMaterialFlags
{
    rtAMBOCCLMAPMATERIALNAFLAG      = 0,

    rtAMBOCCLMAPMATERIALAMBOCCLMAP    = 1,  /**< This material should be lightmapped
											 *   [for non-lightmapped materials within lightmapped objects,
											 *    texel values will be set to (0, 0, 0) (or (255, 255, 255) if
											 *    the rtLTMAPMATERIALAREALIGHT flag is present, so that light-
											 *    emitting textures appear as bright as the light which they are
											 *    emittering) and the mesh may be 'shrunk' in UV-space so as not
											 *    to waste lightmap texels] */
//    rtLTMAPMATERIALAREALIGHT   = 2,  /**< This material is an area light emitter
//                                      *   (see \ref RtLtMapAreaLightGroupCreate) */
    rtAMBOCCLMAPMATERIALNOSHADOW    = 4,  /**< This material does not block light */
    rtAMBOCCLMAPMATERIALSKY         = 8,  /**< This material blocks everything but directional
										   *   lights, to allow sky polygons to occlude geometry
										   *   and yet emit directional light (sky or sun light,
										   *   being as if cast from an infinite distance) */
    rtAMBOCCLMAPMATERIALFLATSHADE   = 16, /**< This material will be lit as if flat-shaded
										   *   (polygon normals will be used during illumination) */

    rtAMBOCCLMAPMATERIALVERTEXLIGHT = 32, /**< This material will be lit at vertices */
    rtAMBOCCLMAPMATERIALFLAGFORCEENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RtAmbOcclMapMaterialFlags RtAmbOcclMapMaterialFlags;

/**
 * \ingroup rtambocclmap
 * RtAmbOcclMapObjectFlags is an enumerated type specifying the different
 * lightmap-related flags which may be applied to world sectors and
 * atomics. These values will be taken into consideration within
 * \ref RtAmbOcclMapAmbOcclMapsCreate and \ref RtAmbOcclMapIlluminate.
 *
 * \see RtAmbOcclMapAtomicGetFlags
 * \see RtAmbOcclMapAtomicSetFlags
 * \see RtAmbOcclMapAmbOcclMapsCreate
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapIlluminateVisCallBack
 */
enum RtAmbOcclMapObjectFlags
{
    rtAMBOCCLMAPOBJECTNAFLAG = 0,

    rtAMBOCCLMAPOBJECTAMBOCCLMAP  = 1, /**< This object is to be lightmapped */
    rtAMBOCCLMAPOBJECTVERTEXLIGHT = 2, /**< This object's vertex prelight colors should
                                       be lit within \ref RtAmbOcclMapIlluminate. */
    rtAMBOCCLMAPOBJECTNOSHADOW    = 4, /**< This object does not cast shadows (useful, for
                                       example, for moving objects for which dynamic
                                       shadows are to be rendered - such as doors) */
	//@{ Jaewon 20050110
	rtAMBOCCLMAPOBJECTNEEDILLUMINATION = 8, /**< This object need to be illuminated */
	//@} Jaewon

	//@{ Jaewon 20050421
	rtAMBOCCLMAPOBJECTINDOOR = 16,
	//@} Jaewon

    rtAMBOCCLMAPOBJECTFLAGFORCEENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RtAmbOcclMapObjectFlags RtAmbOcclMapObjectFlags;

/* This is used to generate lists of atomics/sectors for RtAmbOcclMapAmbOcclMapsCreate,
 * RtAmbOcclMapIlluminate and RtAmbOcclMapAreaLightGroupCreate */
typedef struct LtMapSessionInfo LtMapSessionInfo;
struct LtMapSessionInfo
{
    RwSList *localAtomics;
    RwUInt32 numAtomics;
    RwBool   useCamera;
    RtAmbOcclMapLightingSession *session;
};

/*===========================================================================*
 *--- Toolkit API Functions -------------------------------------------------*
 *===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern AmbOcclMapGlobals _rpAmbOcclMapGlobals;

extern RwV3d *
_rtLtMapLMWCRenderTriangle(RwV3d    *wcpos,   RwV2d *tri, RwV3d  *vWC);
extern void
_rtLtMapLMRGBRenderTriangle(RwImage *ipImage, RwV2d *tri, RwRGBA *colors);
extern RwBool
_rtLtMapALWCRenderTriangle(RwSList  *list,    RwV2d *tri, RwV3d  *vWC);

extern LtMapSessionInfo *
_rtLtMapLightingSessionInfoCreate(LtMapSessionInfo       *sessionInfo,
                                  RtAmbOcclMapLightingSession *session,
                                  RwBool                  useCamera);
extern RwBool
_rtLtMapLightingSessionInfoDestroy(LtMapSessionInfo *sessionInfo);

#if 0
extern void _rtLtMapSetupWorldUVs(RpWorld *world);
#endif

extern RwRGBA *
_rtLtMapIlluminateSamples(RwV3d   *samplePositions,
                          RwV3d   *sampleNormals,
                          RwRGBA  *results,
                          RwUInt32 numSamples,
                          RtAmbOcclMapLightingSession *session);


extern RwChar *
_rtLtMapNameGen(RwChar *result, RwBool addExt);

/* AmbOcclMap creation functionality: */
//@{ Jaewon 20050105
// allocate & sign uvs only(do not create maps).
extern RtAmbOcclMapLightingSession *
RtAmbOcclMapAmbOcclMapsCreateUVsOnly(RtAmbOcclMapLightingSession *session,
									 RwReal density);
//@} Jaewon
extern RtAmbOcclMapLightingSession *
RtAmbOcclMapAmbOcclMapsCreate(RtAmbOcclMapLightingSession *session,
							  RwReal density,
							  RwRGBA *color);

extern void
RtAmbOcclMapAmbOcclMapsDestroy(RtAmbOcclMapLightingSession *session);
extern RpAtomic *
RtAmbOcclMapAtomicAmbOcclMapDestroy(RpAtomic *atomic);

extern RwReal
RtAmbOcclMapGetVertexWeldThreshold(void);
extern RwBool
RtAmbOcclMapSetVertexWeldThreshold(RwReal threshold);

extern RwUInt32
RtAmbOcclMapAmbOcclMapGetDefaultSize(void);
extern RwBool
RtAmbOcclMapAmbOcclMapSetDefaultSize(RwUInt32 size);

extern RwUInt32
RtAmbOcclMapAtomicGetAmbOcclMapSize(RpAtomic *atomic);
extern RpAtomic *
RtAmbOcclMapAtomicSetAmbOcclMapSize(RpAtomic *atomic, RwUInt32 size);

extern RwUInt32
RtAmbOcclMapAtomicGetFlags(RpAtomic *atomic);
extern RpAtomic *
RtAmbOcclMapAtomicSetFlags(RpAtomic *atomic, RwUInt32 flags);


/* Lightmap illumination functionality: */
extern RwInt32
RtAmbOcclMapIlluminate(RtAmbOcclMapLightingSession *session,
					   RwUInt32 superSample);

extern RwReal
RtAmbOcclMapGetSliverAreaThreshold(void);
extern RwBool
RtAmbOcclMapSetSliverAreaThreshold(RwReal threshold);

extern RwUInt32
RtAmbOcclMapGetRayCount(void);
extern RwBool
RtAmbOcclMapSetRayCount(RwUInt32 count);

extern RwRGBA *
RtAmbOcclMapDefaultSampleCallBack(RwRGBA   *results,
								  RwV3d    *samplePositions,
								  RwV3d    * baryCoords,
		                          RwUInt32 numSamples,
								  RpLight  **lights,
								  RwUInt32 numLights,
								  RwV3d    *normals);

//@{ Jaewon 20050408
// 't'(parametric value for the intersection) parameter added
extern RwBool
RtAmbOcclMapDefaultVisCallBack(RpWorld    *world,
							   RwRGBAReal *result,
							   RwV3d      *samplePos,
							   RwV3d      *lightPos,
							   RpLight    *light,
							   RwReal     *t);
//@} Jaewon

extern void
RtAmbOcclMapSetVisCallBackCollisionScalar(RwReal scalar);

extern RwReal
RtAmbOcclMapGetVisCallBackCollisionScalar(void);

extern RtAmbOcclMapLightingSession *
RtAmbOcclMapLightingSessionInitialize(RtAmbOcclMapLightingSession *session,
									  RpWorld *world);

extern RtAmbOcclMapLightingSession *
RtAmbOcclMapLightingSessionDeInitialize(RtAmbOcclMapLightingSession *session);

extern RwInt32
RtAmbOcclMapLightingSessionGetNumSamples(RtAmbOcclMapLightingSession *session);
extern RwInt32
RtAmbOcclMapAtomicGetNumSamples(RpAtomic *atomic);

extern RtAmbOcclMapLightingSession *
RtAmbOcclMapImagesPurge(RtAmbOcclMapLightingSession *session);
extern RpAtomic *
RtAmbOcclMapAtomicImagePurge(RpAtomic *atomic);

extern RtAmbOcclMapLightingSession *
RtAmbOcclMapAmbOcclMapsClear(RtAmbOcclMapLightingSession *session, RwRGBA *color);
extern RpAtomic *
RtAmbOcclMapAtomicAmbOcclMapClear(RpAtomic *atomic, RwRGBA *color);


/* Material functionality: */
extern RwUInt32
RtAmbOcclMapMaterialGetFlags(RpMaterial *material);
extern RpMaterial *
RtAmbOcclMapMaterialSetFlags(RpMaterial *material, RwUInt32 flags);

extern RwReal
RtAmbOcclMapMaterialGetAmbOcclMapDensityModifier(RpMaterial *material);
extern RpMaterial *
RtAmbOcclMapMaterialSetAmbOcclMapDensityModifier(RpMaterial *material, RwReal modifier);


/* Texture-saving functionality: */
extern RwTexDictionary *
RtAmbOcclMapTexDictionaryCreate(RtAmbOcclMapLightingSession *session);

extern const RwChar *
RtAmbOcclMapGetDefaultPrefixString(void);
extern RwBool
RtAmbOcclMapSetDefaultPrefixString(RwChar *string);

extern RwUInt32
RtAmbOcclMapGetAmbOcclMapCounter(void);
extern RwBool
RtAmbOcclMapSetAmbOcclMapCounter(RwUInt32 value);

/* Light map process functionality: */
extern RwBool RtAmbOcclMapSetAmbOcclMapProcessCallBack(RtAmbOcclMapAmbOcclMapProcessCallBack cback);
extern RtAmbOcclMapAmbOcclMapProcessCallBack RtAmbOcclMapGetAmbOcclMapProcessCallBack( void );

extern RtAmbOcclMapLightingSession *
RtAmbOcclMapLightingSessionAmbOcclMapProcess(RtAmbOcclMapLightingSession *session);

extern RwReal RtAmbOcclMapWorldCalculateDensity(RpWorld *world);
extern RwReal RtAmbOcclMapAtomicCalculateDensity(RpAtomic *atomic);

extern RwTexture *_rpLtMapClearLightMap(RwTexture *lightMap, RwRGBA *color);

extern rpLtMapSampleMap *
_rpLtMapSampleMapCreate( RwUInt32 w, RwUInt32 h );

extern RwBool
_rpLtMapSampleMapDestroy( rpLtMapSampleMap *sMap );
// Public plugin functions
extern RwBool
RpAmbOcclMapPluginAttach(void);

extern RwBool
RpAmbOcclMapSetRenderStyle(RpAmbOcclMapStyle style, RpWorld *world);
extern RpAmbOcclMapStyle
RpAmbOcclMapGetRenderStyle(void);

extern RwUInt32
RpAmbOcclMapWorldAmbOcclMapsQuery(RpWorld *world);

extern RwTexture *
RpAmbOcclMapAtomicGetAmbOcclMap(RpAtomic *atomic);
extern RpAtomic *
RpAmbOcclMapAtomicSetAmbOcclMap(RpAtomic *atomic, RwTexture *ambOcclMap);

extern RwUInt32 RpAmbOcclMapGetRasterFormat(void);
extern RwBool RpAmbOcclMapSetRasterFormat(RwUInt32 format);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RTAMBOCCLMAP_H__ */