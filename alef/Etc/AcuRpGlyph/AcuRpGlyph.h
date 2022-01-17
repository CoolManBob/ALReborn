#ifndef ACURPGLYPH_H
#define ACURPGLYPH_H

#include "rwcore.h"
#include "rpworld.h"


#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpGlyphD" )
#else
#pragma comment ( lib , "AcuRpGlyph" )
#endif


#define rwVENDORID_NHN	(0xfffff0L)
#define rwID_GLYPH_NHN	(0x05)

#define rwID_GLYPH		MAKECHUNKID(rwVENDORID_NHN, rwID_GLYPH_NHN)

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

#define ACURPGLYPH_MAX_VERTICES	1024
#define ACURPGLYPH_MAX_INDICES	1024

/* Glyph data */
typedef struct RpGlyph RpGlyph;
struct RpGlyph
{
	RwTexture		*texture;
	RwReal			offset;
	RwReal			radius;
	RwV3d			position;
	RwV3d			axis;
	RwReal			angle;
	RwMatrix		matrix;
	RwV3d			offsets;

	RwInt32			nov;
	RwInt32			noi;
	RwIm3DVertex	TriList[ACURPGLYPH_MAX_VERTICES];
	RwImVertexIndex	TriIndex[ACURPGLYPH_MAX_INDICES];
};

#ifdef RWDEBUG

#define RpGlyphGetTexture(glyph)			rpGlyphGetTexture(glyph)
#define RpGlyphSetTexture(glyph,tex)		rpGlyphSetTexture(glyph,tex)
#define RpGlyphGetPosition(glyph)			rpGlyphGetPosition(glyph)
#define RpGlyphSetPosition(glyph,pos)		rpGlyphSetPosition(glyph,pos)
#define RpGlyphGetRadius(glyph)				rpGlyphGetRadius(glyph)
#define RpGlyphSetRadius(glyph,rad)			rpGlyphSetRadius(glyph,rad)
#define RpGlyphGetOffset(glyph)				rpGlyphGetOffset(glyph)
#define RpGlyphSetOffset(glyph,os)			rpGlyphSetOffset(glyph,os)
#define RpGlyphSetRotate(glyph,axs,ang)		rpGlyphSetRotate(glyph,axs, ang)

#else // RWDEBUG

#define RpGlyphGetTexture(glyph)			((glyph)->texture)
#define RpGlyphSetTexture(glyph,tex)		rpGlyphSetTexture(glyph,tex)
#define RpGlyphGetPosition(glyph)			(&(glyph)->pos)
#define RpGlyphSetPosition(glyph,pos)		((glyph)->position = *(pos))
#define RpGlyphGetRadius(glyph)				(&(glyph)->radius)
#define RpGlyphSetRadius(glyph,rad)			((glyph)->radius = (rad))
#define RpGlyphGetOffset(glyph)				(&(glyph)->offset)
#define RpGlyphSetOffset(glyph,os)			((glyph)->offset = os)
#define RpGlyphSetRotate(glyph,axs,ang)		((glyph)->axis = *(axs), (glyph)->angle = (ang))

#endif

/* Attach Plugin */
extern RwBool
RpGlyphPluginAttach(void);

/* Create Glyph Object */
extern RpGlyph *
RpGlyphCreate();

/* Destroy Glyph Object */
extern RwBool
RpGlyphDestroy(RpGlyph *glyph);

/* Get/Set Glyph Texture */
extern RwTexture *
rpGlyphGetTexture(RpGlyph *glyph);

extern RpGlyph *
rpGlyphSetTexture(RpGlyph *glyph, RwTexture *texture);

/* Get/Set Glyph Position */
extern RwV3d *
rpGlyphGetPosition(RpGlyph *glyph);

extern RpGlyph *
rpGlyphSetPosition(RpGlyph *glyph, RwV3d *pos);

/* Get/Set Glyph Radius */
extern RwReal
rpGlyphGetRadius(RpGlyph *glyph);

extern RpGlyph *
rpGlyphSetRadius(RpGlyph *glyph, RwReal radius);

/* Get/Set Glyph Offset */
extern RwReal
rpGlyphGetOffset(RpGlyph *glyph);

extern RpGlyph *
rpGlyphSetOffset(RpGlyph *glyph, RwReal offset);

/* Set Glyph axis */
extern RpGlyph *
rpGlyphSetRotate(RpGlyph *glyph, RwV3d *axis, RwReal angle);

/* Update Glyph Object */
extern RpGlyph *
RpGlyphUpdate(RpGlyph *glyph, RpWorld *world);

/* Render Glyph */
extern RpGlyph *
RpGlyphRender(RpGlyph *glyph);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPGLYPH_H */
