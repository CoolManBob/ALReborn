#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"

#include "glyph.h"
#include "acurpglyph.h"

RwInt32 GlyphGlobalDataOffset = -1;


/*
 *****************************************************************************
 */
static void *
GlyphOpen(void *instance,
							 RwInt32 offset __RWUNUSED__,
							 RwInt32 size __RWUNUSED__)
{
	return instance;
}


static void *
GlyphClose(void *instance,
							RwInt32 offset __RWUNUSED__,
							RwInt32 size __RWUNUSED__)
{
	return instance;
}


/*
 *****************************************************************************
 */
RwBool
RpGlyphPluginAttach(void)
{
	/*
	 * Register global space...
	 */
	GlyphGlobalDataOffset = RwEngineRegisterPlugin(0,
										  rwID_GLYPH,
										  GlyphOpen,
										  GlyphClose);

	if (GlyphGlobalDataOffset < 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/*
 *****************************************************************************
 */
RpGlyph *
RpGlyphCreate()
{
	RpGlyph *glyph;

	glyph = (RpGlyph *) RwMalloc(sizeof(RpGlyph));
	if (glyph)
	{
		memset(glyph, 0, sizeof(RpGlyph));
	}

	return glyph;
}


/*
 *****************************************************************************
 */
RwBool
RpGlyphDestroy(RpGlyph *glyph)
{
	if (glyph && glyph->texture)
		RwTextureDestroy(glyph->texture);

	if (glyph)
		RwFree(glyph);

	return TRUE;
}


/*
 *****************************************************************************
 */
RwTexture *
rpGlyphGetTexture(RpGlyph *glyph)
{
	return glyph->texture;
}


/*
 *****************************************************************************
 */
RpGlyph *
rpGlyphSetTexture(RpGlyph *glyph, RwTexture *texture)
{
	if (glyph->texture)
		RwTextureDestroy(glyph->texture);

	glyph->texture = texture;
	RwTextureAddRef(texture);

	return glyph;
}


/*
 *****************************************************************************
 */
RwV3d *
rpGlyphGetPosition(RpGlyph *glyph)
{
	return &(glyph->position);
}


/*
 *****************************************************************************
 */
RpGlyph *
rpGlyphSetPosition(RpGlyph *glyph, RwV3d *pos)
{
	glyph->position = *pos;

	return glyph;
}


/*
 *****************************************************************************
 */
RwReal
rpGlyphGetRadius(RpGlyph *glyph)
{
	return glyph->radius;
}


/*
 *****************************************************************************
 */
RpGlyph *
rpGlyphSetRadius(RpGlyph *glyph, RwReal radius)
{
	glyph->radius = radius;

	return glyph;
}


/*
 *****************************************************************************
 */
RwReal
rpGlyphGetOffset(RpGlyph *glyph)
{
	return glyph->offset;
}


/*
 *****************************************************************************
 */
RpGlyph *
rpGlyphSetOffset(RpGlyph *glyph, RwReal offset)
{
	glyph->offset = offset;

	return glyph;
}


/*
 *****************************************************************************
 */
RpGlyph *
rpGlyphSetRotate(RpGlyph *glyph, RwV3d *axis, RwReal angle)
{
	glyph->axis = *axis;
	glyph->angle = angle;

	return glyph;
}


/*
 *****************************************************************************
 */

RpCollisionTriangle *GlyphWorldCollisionCB
	(
	RpIntersection *		intersection	,
	RpWorldSector *			sector			,
	RpCollisionTriangle *	collTriangle	,
	RwReal					distance		,
	void *					data
	)
{
	RwInt32 i;
	RpGlyph *glyph = (RpGlyph *) data;
	RwV3d *v;
	RwV3d vi;

	if (glyph->nov < ACURPGLYPH_MAX_VERTICES - 3)
	{
		for (i = 0; i < 3; i++)
		{
			v = collTriangle->vertices[i];
			RwV3dTransformPoints(&vi, v, 1, &glyph->matrix);

			RwIm3DVertexSetPos(glyph->TriList + glyph->nov, (RwReal) (v->x + glyph->offsets.x), (RwReal) (v->y + glyph->offsets.y), (RwReal) (v->z + glyph->offsets.z));
			RwIm3DVertexSetRGBA(glyph->TriList + glyph->nov, 255, 255, 255, 255);
			RwIm3DVertexSetU(glyph->TriList + glyph->nov, (RwReal) (vi.x / glyph->radius + 0.5));
			RwIm3DVertexSetV(glyph->TriList + glyph->nov, (RwReal) (vi.z / glyph->radius + 0.5));
			
			RwIm3DVertexSetNormal(glyph->TriList + glyph->nov, collTriangle->normal.x, collTriangle->normal.y, collTriangle->normal.z);

			glyph->TriIndex[glyph->noi] = glyph->nov;

			glyph->nov += 1;
			glyph->noi += 1;
		}
	}

	return collTriangle;
}

RpGlyph *
RpGlyphUpdate(RpGlyph *glyph, RpWorld *world)
{
	RpIntersection	intersection;
	RwV3d scale = {glyph->radius, glyph->radius, glyph->radius};
	RwV3d offsets = {0, glyph->offset, 0};
	RwMatrix matrix;

	RwMatrixRotate(&matrix, &glyph->axis, glyph->angle, rwCOMBINEREPLACE);

	RwV3dTransformPoints(&glyph->offsets, &offsets, 1, &matrix);

	RwMatrixTranslate(&matrix, &glyph->position, rwCOMBINEPOSTCONCAT);

	RwMatrixInvert(&glyph->matrix, &matrix);

	glyph->nov = 0;
	glyph->noi = 0;

	intersection.type=rpINTERSECTSPHERE;
	intersection.t.sphere.center = glyph->position;
	intersection.t.sphere.radius = glyph->radius;

	RpCollisionWorldForAllIntersections (world, &intersection, GlyphWorldCollisionCB, (void *) glyph );

	return glyph;
}


/*
 *****************************************************************************
 */
RpGlyph *
RpGlyphRender(RpGlyph *glyph)
{
	/*
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDZERO);

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);
	*/

	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *) rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) RwTextureGetRaster(glyph->texture));

	if (RwIm3DTransform(glyph->TriList, glyph->nov, NULL, rwIM3D_VERTEXUV))
	{                         
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, glyph->TriIndex, glyph->noi);

		RwIm3DEnd();

		return glyph;
	}

	return NULL;
}

