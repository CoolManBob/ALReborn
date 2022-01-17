//@{ Jaewon 20041203
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   vis.c                                                      -*
 *-                                                                         -*
 *-  Purpose :   AcuRtAmbOcclMap toolkit                                    -*
 *-                                                                         -*
 *===========================================================================*/

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/

/* Needed for memset */
#include "string.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rprandom.h"

#include "rpcollis.h"
#include "rtintsec.h"
#include "rtbary.h"

#include "rpdbgerr.h"

#include "rppvs.h"
#include "AcuRpMatD3DFx.h"
#include "AcuRtAmbOcclMap.h"

#include "vis.h"
#include "ltmapvar.h"


/*===========================================================================*
 *--- Local Global Variables ------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Local defines ---------------------------------------------------------*
 *===========================================================================*/

/* These are used during lighting inconjunction with
 * the rtLTMAPPMATERIALSKY material collision flag */
#define rtLTMAPDIRECTIONALVIS 1
#define rtLTMAPSKYCOLLISION   2


/*===========================================================================*
 *--- Local types -----------------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local variables -------------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local functions -------------------------------------------------------*
 *===========================================================================*/

void
LtMapCollisionInitCache(void)
{
    rtLtMapGlobals.cachedTriangle.sky = FALSE;
    rtLtMapGlobals.cachedTriangle.hit = 0;
}    
/****************************************************************************
 LtMapAtomicTriangleCollisCallBack
 */
static RpCollisionTriangle *
LtMapAtomicTriangleCollisCallBack(RpIntersection *intersection,
                                  RpCollisionTriangle *collTriangle,
                                  RwReal distance,
                                  void *data)
{
    LtMapBlib *params = (LtMapBlib *) data;
    RpMaterial *material;

    RWFUNCTION(RWSTRING("LtMapAtomicTriangleCollisCallBack"));

    /* Check for non-shadowing materials */
    material = params->geom->matList.materials[params->geom->triangles[collTriangle->index].matIndex];
    if (RtAmbOcclMapMaterialGetFlags(material) & rtAMBOCCLMAPMATERIALNOSHADOW)
    {
        RWRETURN(collTriangle);
    }

/* TODO[3][ACQ]: WE USE cumulativeDist TO DETECT IF A COLLISION OCCURRED
 *               ONCE THE CALLBACK'S EXITED, BUT DO WE NEED THIS TEST?
 *                HRMM, rtLTMAPDIRECTIONALVIS TESTING DOES... */
    if (distance < params->dist)
    {
        /* For directional lights, we need to find the closest collision to the
         * sample point. NOTE: we are not even guaranteed to get the first
         * collision WITHIN an object, let alone between them (and sectors
         * overlap, not to mention that atomics overlap sectors) so we need
         * to keep on colliding */
        params->dist = distance;

        rtLtMapGlobals.cachedTriangle.triangle = *collTriangle;
        rtLtMapGlobals.cachedTriangle.sky = (RtAmbOcclMapMaterialGetFlags(material) & rtAMBOCCLMAPMATERIALSKY);
        rtLtMapGlobals.cachedTriangle.hit = 1;

		//@{ Jaewon 20050408
		// rpAMBOCCLMAPSTYPEINDOOR condition added.
        if (!(rtLtMapGlobals.skyFlags & rtLTMAPDIRECTIONALVIS)
			&& !(_rpAmbOcclMapGlobals.renderStyle & rpAMBOCCLMAPSTYLEINDOOR))
		//@} Jaewon
        {
            /* If we hit anything, stop the search */
            RWRETURN(NULL);
        }
        else
        {
            /* For directional lights, we need to track whether the
             * current closest collision is with a sky polygon or not */
            if (RtAmbOcclMapMaterialGetFlags(material) & rtAMBOCCLMAPMATERIALSKY)
            {
                rtLtMapGlobals.skyFlags |= rtLTMAPSKYCOLLISION;
            }
            else
            {
                rtLtMapGlobals.skyFlags &= ~rtLTMAPSKYCOLLISION;
            }
        }
    }

    RWRETURN(collTriangle);
}


/****************************************************************************
 LtMapAtomicCollisCB
 */
static RpAtomic *
LtMapAtomicCollisCB(RpIntersection *intersection,
                    RpWorldSector *sector,
                    RpAtomic *atomic,
                    RwReal distance,
                    void *data)
{
    LtMapBlib   params;

    RWFUNCTION(RWSTRING("LtMapAtomicCollisCB"));

    /* Check for non-shadowing objects */
    if (RtAmbOcclMapAtomicGetFlags(atomic) & rtAMBOCCLMAPOBJECTNOSHADOW)
    {
        RWRETURN(atomic);
    }

/* TODO[5]: SHOULD EARLY-OUT HERE IF THE ATOMIC BSPHERE COLLISION IS  FURTHER FROM
 *          THE START THAN AN EXISTING COLLISION (SHOULD HELP THE DIRECTIONAL LIGHT
 *          CASE). THIS IS COMPLICATED BY TWO THINGS:
 *            o THE DISTANCE PASSED TO THIS FUNCTION IS STUPID, IT'S THE
 *              SHORTEST DISTANCE FROM THE LINE TO THE SPHERE CENTRE, NOT
 *              THE DISTANCE ALONG THE LINE TO THE COLLISION W/ THE SPHERE!
 *            o WHEN A BSPHERE OVERLAPS THE START POINT, IT SHOULD BE GIVEN
 *              A DISTANCE OF ZERO BUT IT'S NOT. */

    params.geom           =  RpAtomicGetGeometry(atomic);
    params.dist           = *(RwReal *)data;
    params.cumulativeDist =  (RwReal *)data;

    RpAtomicForAllIntersections(atomic, intersection, LtMapAtomicTriangleCollisCallBack, &params);

/* TODO[3][ACQ]: WE USE cumulativeDist TO DETECT IF A COLLISION OCCURRED
 *               ONCE THE CALLBACK'S EXITED, BUT DO WE NEED THIS TEST?
 *                HRMM, rtLTMAPDIRECTIONALVIS TESTING DOES... */
    /* For directional lights, params.dist should contain the
     * closest collision within the current atomic. For other
     * light types, it will contain the first detected collision. */
    if (params.dist < *(params.cumulativeDist))
    {
        /* For directional lights, we need to find the closest collision to the
         * sample point. NOTE: we are not even guaranteed to get the first
         * collision WITHIN an object, let alone between them (and sectors
         * overlap, not to mention that atomics overlap sectors) so we need
         * to keep on colliding */
        *(params.cumulativeDist) = params.dist;
		//@{ Jaewon 20050408
		// rpAMBOCCLMAPSTYPEINDOOR condition added.
        if (!(rtLtMapGlobals.skyFlags & rtLTMAPDIRECTIONALVIS)
			&& !(_rpAmbOcclMapGlobals.renderStyle & rpAMBOCCLMAPSTYLEINDOOR))
		//@} Jaewon
        {
            /* If we hit anything, stop the search */
            RWRETURN(NULL);
        }
    }

    RWRETURN(atomic);
}


/*===========================================================================*
 *--- Toolkit API Functions -------------------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapDefaultVisCallBack is the default
 * \ref RtAmbOcclMapIlluminateVisCallBack, called from within
 * \ref RtAmbOcclMapIlluminate and \ref RtAmbOcclMapDefaultSampleCallBack.
 *
 * This is the default callback used for determining the visibility
 * between lights and sample points. Currently, it returns either
 * complete visibility or complete occlusion. User callbacks could
 * implement color-filtering (due to glass or fog, say) by modifying
 * the value of the \ref RwRGBAReal pointed to by the result parameter.
 *
 * This function performs line intersections tests using the \ref rpcollis
 * plugin and \ref rtintersection toolkit. PVS information is not currently
 * used (though area lights are culled using PVS information at a higher
 * level in the lighting calculations).
 *
 * See \ref RtAmbOcclMapIlluminate for further details on lighting
 * calculations.
 *
 * The matd3dfx plugin must be attached before using this function.
 *
 * \param  world      A pointer to the current world (used to perform collisions)
 * \param  result     A pointer to an \ref RwRGBAReal light-filter value
 * \param  samplePos  The world-space position of the current sample
 * \param  lightPos   The world-space position of the current light
 * \param  light      A pointer to the current light
 *
 * \return TRUE if the light is at least partially visible from this sample,
 * otherwise FALSE
 *
 * \see RtAmbOcclMapDefaultSampleCallBack
 * \see RtAmbOcclMapIlluminate
 */
//@{ Jaewon 20050408
// 't'(parametric value for the intersection) parameter added
RwBool
RtAmbOcclMapDefaultVisCallBack(RpWorld    *world,
							   RwRGBAReal __RWUNUSED__ *result,
							   RwV3d      *samplePos,
							   RwV3d      *lightPos,
							   RpLight    *light,
							   RwReal     *t)
//@} Jaewon
{
    RpIntersection is;
    RwReal distance, worldDist;
    RwUInt32 worldFlags;
    //RpWorld *worldBkp;

    RWAPIFUNCTION(RWSTRING("RtAmbOcclMapDefaultVisCallBack"));

/* TODO[2][5]: PUT THE PARAMS IN A STRUCT AND/OR MAYBE ALLOW TO DEAL W/ AN ARRAY OF VIS-TESTS */

    /* Can the sample see the light source?
     * [N.B. We do it this way around because otherwise lights outside the world
     *  could see in to the sample, which is wrong. The collision test looks for
     *  collisions w/ front-facing triangles in the direction of travel along
     *  the line... so to do 'back-facing' collisions reverse the line... or to
     *  do 'no-culling' collisions, do a test in both directions] */
    is.t.line.start = *samplePos;
    is.t.line.end   = *lightPos;
    is.type         =  rpINTERSECTLINE;
/* TODO[3][ABE]: SHOULD WE PUSH SAMPLES AWAY FROM THE LIGHT TO IMPROVE LIGHTING?
 *               I THINK NOT (TILL A BETTER METHOD CAN BE FOUND - NAIVE PUSHING
 *               TOWARDS OR AWAY FROM THE LIGHT WILL ALWAYS BREAK IN HALF OF CASES)
 *                THIS WORKS FOR DM4 WITH CONCAVE GEOMETRY AND LOTS OF ANNOYING
 *               ORTHOGONAL TRIANGLES, BUT FOR A CONVEX *OBJECT* LIKE A TREE WITH
 *               THIN LEAVES IT WILL MESS EVERYTHING UP (THOUGH IN THAT CASE VERTEX
 *               LIGHTING WITH SHADOWS WILL ALIAS sooo BADLY YOU WON'T NOTICE :) */
#if 0
    {
        /* Move the sample away from the light... */
        RwV3d delta;
        RwV3dSub(&delta, lightPos, samplePos);
        RwV3dNormalize(&delta, &delta);
        RwV3dScale(&delta, &delta, -0.05f);/*+0.05f);*/
        RwV3dAdd(&(is.t.line.start), &(is.t.line.start), &delta);
    }
#endif

    if( rtLtMapGlobals.cachedTriangle.hit > 0 )
    {
        RwV3d delta;
        RwReal distance;

        RwV3dSub(&delta, &is.t.line.end, &is.t.line.start);
        if (RtIntersectionLineTriangle(&is.t.line.start, &delta,
                            rtLtMapGlobals.cachedTriangle.triangle.vertices[0],
                            rtLtMapGlobals.cachedTriangle.triangle.vertices[1],
                            rtLtMapGlobals.cachedTriangle.triangle.vertices[2],
                                    &distance))
        {
            if (!rtLtMapGlobals.cachedTriangle.sky)
            {
                rtLtMapGlobals.cachedTriangle.hit++;
				//@{ Jaewon 20050408
				if(t)
					*t = distance;
				//@} Jaewon
                return(FALSE);
            }
        }
    }            
    
    /* We treat collisions with 'sky' materials differently - they should occlude
     * everything but directional lights. Hence we treat them as ordinary occluders
     * for all non-directional light types (including area light samples), but they
     * will transmit directional lights even after the ray has collided with them
     * (so, in true Quake-engine style, sky polygons can be used for VIS-blocking).
     * Note that this means that for directional lights we have to find the CLOSEST
     * collision to the lightmap sample, not just any collision - it matters whether
     * the closest polygon is a sky or non-sky polygon (if it's a sky polygon then
     * directional lights will be visible no matter what other polygons collide with
     * the ray behind it). */
    if ((NULL != light) && (rpLIGHTDIRECTIONAL == RpLightGetType(light)))
    {
        rtLtMapGlobals.skyFlags = rtLTMAPDIRECTIONALVIS;
    }
    else
    {
        rtLtMapGlobals.skyFlags = 0;
    }

    /* Collide with the world */
    distance = rtLtMapGlobals.visCallBackCollisionScalar;
    
    /* For directional lights, distance should contain the
     * closest collision with worldsectors. For other light
     * types, it will contain the first detected collision. */
    worldDist = distance;
    worldFlags = rtLtMapGlobals.skyFlags;

    /* If we haven't collided already (or if we're testing
     * a directional light), test for atomic intersections */
    if ((distance >= rtLtMapGlobals.visCallBackCollisionScalar) ||
        (rtLtMapGlobals.skyFlags & rtLTMAPDIRECTIONALVIS))
    {
        RpWorldForAllAtomicIntersections(world, &is, LtMapAtomicCollisCB, &distance);
    }

    if (rtLtMapGlobals.skyFlags & rtLTMAPDIRECTIONALVIS)
    {
        /* Was the closest collision with the world or atomics? */
        if (worldDist < distance)
            rtLtMapGlobals.skyFlags = worldFlags;
    }

    /* The reason for the ">= 0.95f" test is that, for area light emitters, you will
     * collide with the emitter surface roughly at the point when you reach the light. */
/* TODO[3][ABE]: USE A #DEFINE FOR THIS VALUE (SHOULD PROBABLY CHECK THAT RpLights HAVE A
 *               SAFE RADIUS AROUND THEM OF THIS SIZE - DO A SPHERE INTERSECTION TO CHECK) */
    if ((distance >= rtLtMapGlobals.visCallBackCollisionScalar) ||
        (rtLtMapGlobals.skyFlags & rtLTMAPSKYCOLLISION))
    {
		//@{ Jaewon 20050408
		if(t)
			*t = 1.0f;
		//@} Jaewon

         /* The default 'result' value is ok */
         RWRETURN(TRUE);
    }

	//@{ Jaewon 20050408
	if(t)
		*t = distance;
	//@} Jaewon
    RWRETURN(FALSE);
}


/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapSetVisCallBackCollisionScalar sets the collision scalar used by
 * \ref RtAmbOcclMapDefaultVisCallBack to avoid self collision with area lights.
 *
 * The threshold is used to ignore polygons near the light position. Collisions
 * that occur withing the threshold are ignored. This is used when performing
 * area lighting to avoid shadows being cast by the area light geometry itself.
 * 0.0f corresponds to all collisions being ignored, and 1.0f corresponds to no
 * collisions being ignored.
 *
 * The default value is 0.95f.
 *
 * The matd3dfx plugin must be attached before using this function.
 *
 * \param  scalar The wanted value for the collision scalar.
 *
 * \see RtAmbOcclMapDefaultVisCallBack
 * \see RtAmbOcclMapGetVisCallBackCollisionScalar
 * \see RtAmbOcclMapIlluminate
 *
 */
void
RtAmbOcclMapSetVisCallBackCollisionScalar(RwReal scalar)
{
    RWAPIFUNCTION(RWSTRING("RtAmbOcclMapSetVisCallBackCollisionScalar"));
    rtLtMapGlobals.visCallBackCollisionScalar = scalar;
    RWRETURNVOID();
}

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapGetVisCallBackCollisionScalar returns the current value of the
 * collision scalar.
 *
 * see \ref RtAmbOcclMapSetVisCallBackCollisionScalar for more information on the use
 * of the collision scalar.
 *
 * The matd3dfx plugin must be attached before using this function.
 *
 * \return  The value of the collision scalar.
 *
 * \see RtAmbOcclMapSetVisCallBackCollisionScalar
 * \see RtAmbOcclMapDefaultVisCallBack
 * \see RtAmbOcclMapIlluminate
 *
 */
RwReal
RtAmbOcclMapGetVisCallBackCollisionScalar(void)
{
    RWAPIFUNCTION(RWSTRING("RtAmbOcclMapGetVisCallBackCollisionScalar"));

    RWRETURN(rtLtMapGlobals.visCallBackCollisionScalar);
}
