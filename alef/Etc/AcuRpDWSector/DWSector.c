#include <memory.h>

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "rpcriter.h"

#include "dwsector.h"
#include "dwsstream.h"

#include "acurpdwsector.h"

RwInt32 DWSectorGlobalDataOffset		= -1;
RwInt32 DWSectorWorldDataOffset			= -1;
RwInt32 DWSectorWorldSectorDataOffset	= -1;

RwInt32 DWSectorCollisionDataOffset;


/*
 *****************************************************************************
 */
static void *
DWSectorOpen(void *instance,
							 RwInt32 offset __RWUNUSED__,

							 RwInt32 size __RWUNUSED__)
{
	return instance;
}


static void *
DWSectorClose(void *instance,
							RwInt32 offset __RWUNUSED__,
							RwInt32 size __RWUNUSED__)
{
	return instance;
}


/*
 *****************************************************************************
 */
static void *
DWSectorWorldSectorDataConstructor(void *worldSector,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (DWSectorWorldSectorDataOffset > 0)
	{
		DWSectorWorldSectorData *data;

		data = DWSECTORWORLDSECTOR(worldSector);
		memset(data, 0, sizeof(DWSectorWorldSectorData));

		data->curDetail = -1;
	}

	return worldSector;
}


static void *
DWSectorWorldSectorDataDestructor(void *worldSector,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	if (DWSectorWorldSectorDataOffset > 0)
	{
		RpDWSectorDestroyWorldSector(worldSector);
	}

	return worldSector;
}



RpDWSector *DWSectorCopy(RpDWSector *dstDWSector, RpDWSector *srcDWSector)
{
	RwInt32 i;

	RpDWSectorDestroy(dstDWSector);

	dstDWSector->atomic = RpAtomicClone(srcDWSector->atomic);

	return dstDWSector;
}


static void *
DWSectorWorldSectorDataCopier(void *dstworldSector, const void *srcworldSector,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	RwInt32 i;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		DWSectorWorldSectorData *srcdata;
		DWSectorWorldSectorData *dstdata;

		srcdata				= DWSECTORWORLDSECTOR(srcworldSector)	;
		dstdata				= DWSECTORWORLDSECTOR(dstworldSector)	;

		dstdata->numDetail	= dstdata->numDetail					;
		dstdata->curDetail	= srcdata->curDetail					;

		RpDWSectorUpdateCurrent(dstworldSector);

		for (i = 0; i < DWSECTOR_MAX_DETAIL; i++)
		{
			if (!DWSectorCopy(dstdata->details + i, srcdata->details + i))
				return NULL;
		}
	}

	return dstworldSector;
}


/*
 *****************************************************************************
 */
static void *
DWSectorWorldDataConstructor(void *world,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (DWSectorWorldDataOffset > 0)
	{
		DWSectorWorldData *data;

		data = DWSECTORWORLD(world);
		memset(data, 0, sizeof(DWSectorWorldData));
	}

	return world;
}


static void *
DWSectorWorldDataDestructor(void *world,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	if (DWSectorWorldDataOffset > 0)
	{
		DWSectorWorldData *data;

		data = DWSECTORWORLD(world);
	}

	return world;
}


static void *
DWSectorWorldDataCopier(void *dstworld, const void *srcworld,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	if (DWSectorWorldDataOffset > 0)
	{
		DWSECTORWORLDDATA(dstworld, numDetail) = DWSECTORWORLDDATACONST(srcworld, numDetail);
	}

	return dstworld;
}



/*
 *****************************************************************************
 */
RwBool
RpDWSectorPluginAttach(void)
{
	RwInt32 offset;

	DWSectorCollisionDataOffset = RpWorldSectorGetPluginOffset(rwID_COLLISPLUGIN);
	if (DWSectorCollisionDataOffset < 0)
	{
		return FALSE;
	}

	/*
	 * Register global space...
	 */
	DWSectorGlobalDataOffset = RwEngineRegisterPlugin(0,
										  rwID_DWSECTOR	,
										  DWSectorOpen	,
										  DWSectorClose	);

	if (DWSectorGlobalDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register worldsector extension space...
	 */
	DWSectorWorldSectorDataOffset = RpWorldSectorRegisterPlugin(sizeof(DWSectorWorldSectorData),
										rwID_DWSECTOR						,
										DWSectorWorldSectorDataConstructor	,
										DWSectorWorldSectorDataDestructor	,
										DWSectorWorldSectorDataCopier		);

	if (DWSectorWorldSectorDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpWorldSectorRegisterPluginStream(
										rwID_DWSECTOR						,
										DWSectorWorldSectorDataReadStream	,
										DWSectorWorldSectorDataWriteStream	,
										DWSectorWorldSectorDataGetStreamSize);

	if (offset != DWSectorWorldSectorDataOffset )
	{
		return FALSE;
	}

	/*
	 * Register world extension space...
	 */
	DWSectorWorldDataOffset = RpWorldRegisterPlugin(sizeof(DWSectorWorldData),
									rwID_DWSECTOR					,
									DWSectorWorldDataConstructor	,
									DWSectorWorldDataDestructor		,
									DWSectorWorldDataCopier			);

	if (DWSectorWorldDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpWorldRegisterPluginStream(
									rwID_DWSECTOR					,
									DWSectorWorldDataReadStream		,
									DWSectorWorldDataWriteStream	,
									DWSectorWorldDataGetStreamSize	);

	if (offset != DWSectorWorldDataOffset )
	{
		return FALSE;
	}

	return TRUE;
}


/*
 *****************************************************************************
 */
RpWorldSector *DWSectorInitWorldSector(RpWorldSector *worldSector, RwUInt8 numDetail, RwInt8 curDetail)
{
	if (DWSectorWorldSectorDataOffset > 0 && numDetail <= DWSECTOR_MAX_DETAIL && curDetail >= 0)
	{
		DWSECTORWORLDSECTORDATA(worldSector, numDetail) = numDetail;
		DWSECTORWORLDSECTORDATA(worldSector, curDetail) = curDetail;

		RpCollisionWorldSectorDestroyData(worldSector);

		RpDWSectorUpdateCurrent(worldSector);

		return worldSector;
	}

	return NULL;
}

typedef struct _DWSectorInit _DWSectorInit;
struct _DWSectorInit
{
	RwUInt8	numDetail;
	RwInt8	curDetail;
};

RpWorldSector *s_DWSectorInit(RpWorldSector *worldSector, void *data)
{
	_DWSectorInit *arg = (_DWSectorInit *) data;

	return DWSectorInitWorldSector(worldSector, arg->numDetail, arg->curDetail);
}

RpWorld *
RpDWSectorInitWorld(RpWorld *world, RwUInt8 numDetail, RwInt8 curDetail)
{
	_DWSectorInit data;

	DWSECTORWORLDDATA(world, numDetail) = numDetail;

	data.curDetail = curDetail;
	data.numDetail = numDetail;

	return RpWorldForAllWorldSectors(world, s_DWSectorInit, (void *) &data);
}

/*
 *****************************************************************************
 */
RpDWSector *
RpDWSectorInit(RpDWSector *dwSector, RwUInt32 numVertices, RwUInt32 numPolygons, RwUInt8 numTexCoords)
{
	RwInt32 i;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		dwSector->geometry	= RpGeometryCreate	( numVertices , numPolygons ,
			rpGEOMETRYTRISTRIP	| rpGEOMETRYTEXCOORDSETS(numTexCoords) | rpGEOMETRYNORMALS	|
			rpGEOMETRYLIGHT		| rpGEOMETRYPRELIT											);

		dwSector->atomic	= RpAtomicCreate	();

		if (!RpAtomicSetGeometry	( dwSector->atomic , dwSector->geometry , rpATOMICSAMEBOUNDINGSPHERE )	)
			return NULL;

		AgcmMap::m_pThis->LockFrame();

		if (!RpAtomicSetFrame		( dwSector->atomic , RwFrameCreate()	)								)
			return NULL;

		AgcmMap::m_pThis->UnlockFrame();

		RpGeometryDestroy(dwSector->geometry);

		return dwSector;
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RwBool 
RpDWSectorDestroy(RpDWSector *dwSector)
{
	RwInt32 i;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		if( dwSector->atomic )
		{
			RpWorld * pWorld;

			pWorld = RpAtomicGetWorld( dwSector->atomic );

			if( pWorld )
			{
				RpWorldRemoveAtomic( pWorld , dwSector->atomic );
			}
			else
			{
				// 널일수도 있음..;
			}

			AcuObject::DestroyAtomicData(dwSector->atomic);

			RpAtomicDestroy(dwSector->atomic);
			
		}

		dwSector->atomic	= NULL;
		dwSector->geometry	= NULL;

		return TRUE;
	}

	return FALSE;
}


/*
 *****************************************************************************
 */
RpWorldSector *
RpDWSectorDestroyDetail(RpWorldSector *worldSector, RwUInt8 numDetail)
{
	DWSectorWorldSectorData *data;
	RpDWSector *detail;
	RwInt8 curDetail;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);
		curDetail = data->curDetail;

		if (numDetail < data->numDetail)
		{
			RpDWSectorUpdateCurrent(worldSector);
			RpDWSectorSetCurrentDetail(worldSector, numDetail);

			RpCollisionWorldSectorDestroyData(worldSector);

			RpDWSectorUpdateCurrent(worldSector);
			RpDWSectorSetCurrentDetail(worldSector, curDetail);

			detail = data->details + numDetail;

			if (!RpDWSectorDestroy(detail))
				return NULL;

			return worldSector;
		}
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpWorldSector *
RpDWSectorDestroyWorldSector(RpWorldSector *worldSector)
{
	DWSectorWorldSectorData	*	data	;
	RwUInt8						i		;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);

		RpDWSectorUpdateCurrent		( worldSector					);
		RpDWSectorSetCurrentDetail	( worldSector, data->orgDetail	);

		for (i = 0; i < data->numDetail; i++)
		{
			RpDWSectorDestroyDetail(worldSector, i);
		}

		return worldSector;
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpWorldSector *
RpDWSectorUpdateCurrent(RpWorldSector *worldSector)
{
	RwInt32						i		;
	DWSectorWorldSectorData	*	data	;
	RpDWSector				*	detail	;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);

		if (	data->curDetail >=	0					&&
				data->curDetail <	data->numDetail		)
		{
			detail = data->details + data->curDetail;
			return worldSector;
		}
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpDWSector *
RpDWSectorGetDetail(RpWorldSector *worldSector, RwUInt8 numDetail)
{
	DWSectorWorldSectorData *data;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);
		if (numDetail < data->numDetail)
			return (data->details + numDetail);
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RwInt8 
RpDWSectorGetCurrentDetail(RpWorldSector *worldSector)
{
	if (DWSectorWorldSectorDataOffset > 0)
	{
		return DWSECTORWORLDSECTORDATA(worldSector, curDetail);
	}

	return -1;
}

/*
 *****************************************************************************
 */
RpWorldSector *
RpDWSectorSetCurrentDetail(RpWorldSector *worldSector, RwUInt8 numDetail)
{
	DWSectorWorldSectorData	*	data	;
	RpDWSector				*	detail	;
	RpWorld					*	world	= RpWorldSectorGetWorld(worldSector);
	RwInt32						i		;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);
		if (numDetail < data->numDetail)
		{
			detail = data->details + numDetail;

			if ( data->curDetail != numDetail	)
				RpDWSectorUpdateCurrent	( worldSector									);

			if ( world && data->details[data->curDetail].atomic)
				RpWorldRemoveAtomic		( world, data->details[data->curDetail].atomic	);

			if ( world && detail->atomic		)
				RpWorldAddAtomic		( world, detail->atomic							);

			data->curDetail = numDetail;

			if ( world )
			{
				worldSector->numVertices = 0;
				worldSector->numPolygons = 0;

				// 매시 정보 다시 생성..
				RpWorldLock		(world);
				RpWorldUnlock	(world);
			}
		}

		return worldSector;
	}

	return NULL;
}

RpWorldSector *DWSectorSetCurrentDetail(RpWorldSector *worldSector, void *data)
{
	return RpDWSectorSetCurrentDetail(worldSector, (RwUInt8) data);
}

RpWorld *
RpDWSectorSetCurrentWorldDetail(RpWorld *world, RwUInt8 numDetail)
{
	return RpWorldForAllWorldSectors(world, DWSectorSetCurrentDetail, (void *)numDetail);
}


/*
 *****************************************************************************
 */
RpWorldSector *
RpDWSectorBuildWorldSectorCollision(RpWorldSector *worldSector)
{
	DWSectorWorldSectorData	*	data		;
	RpCollisionBuildParam		param		;
	RwUInt8						curDetail	;
	RwInt8						i			;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data		= DWSECTORWORLDSECTOR(worldSector)	;
		curDetail	= data->curDetail					;

		param.dummy = 0									;

		for (i = 0; i < data->numDetail; i++)
		{
			if( data->details[i].geometry )
				RpCollisionGeometryBuildData(data->details[i].geometry, &param);
		}

		return worldSector;
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpWorldSector *DWSectorBuildWorldSectorCollision(RpWorldSector *worldSector, void *data)
{
	return RpDWSectorBuildWorldSectorCollision(worldSector);
}


RpWorld *
RpDWSectorBuildWorldCollision(RpWorld *world)
{
	return RpWorldForAllWorldSectors(world, DWSectorBuildWorldSectorCollision, NULL);
}


/*
 *****************************************************************************
 */
RpWorldSector *DWSectorDestroyWorldSector(RpWorldSector *worldSector, void *_data)
{
	DWSectorWorldSectorData *data;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);

		RpDWSectorUpdateCurrent		( worldSector					);
		RpDWSectorSetCurrentDetail	( worldSector, data->orgDetail	);
	}

	return worldSector;
}

RpWorld *
RpDWSectorDestroyWorld(RpWorld *world)
{
	return RpWorldForAllWorldSectors(world, DWSectorDestroyWorldSector, NULL);
}


/*
 *****************************************************************************
 */
RwBool
RpDWSectorSetWriteMode(RpWorld *world, RwUInt8 mode)
{

	if (DWSectorWorldDataOffset > 0)
	{
		DWSECTORWORLDDATA(world, modeWrite) = mode;
	}

	return TRUE;
}


/*
 *****************************************************************************
 */
RpDWSectorFields
RpDWSectorGetFields(RpDWSector *dwSector)
{
	RpDWSectorFields	fields = rpDWSECTOR_FIELD_NONE;
	//RwInt32				i;

	return fields;
}
