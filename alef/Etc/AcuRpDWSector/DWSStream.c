#include "rwcore.h"
#include "rpworld.h"

#include "acurpdwsector.h"
#include "dwsector.h"
#include "dwsstream.h"
#include "dwsnative.h"

#include "aplib.h"
#include <memory.h>

#define	DWSECTOR_EXIST(pointer)							(pointer ? 0:1)

#define	DWSECTOR_STREAM_WRITE_EXIST(stream, source, sizeTotal, size)		\
{																	\
	RwBool exist;													\
																	\
	if (source)														\
		exist = 1;													\
	else															\
		exist = 0;													\
																	\
	RwStreamWrite(stream, (const void *) &exist, sizeof(RwBool));	\
	sizeTotal += sizeof(RwBool);									\
																	\
	if (exist && size)												\
	{																\
		RwStreamWrite(stream, (const void *) source, (size));		\
		sizeTotal += (size);										\
	}																\
}

#define	DWSECTOR_STREAM_WRITE_MEMORY(source, destination, sizeTotal, size)	\
if (source)																	\
{																			\
	memcpy(destination, source, (size));									\
	destination += (size);													\
	sizeTotal += (size);														\
}


#define	DWSECTOR_STREAM_SIZE_EXIST(pointer, sizeTotal, size)		\
{																	\
	RwBool exist;													\
																	\
	if (pointer)													\
		exist = 1;													\
	else															\
		exist = 0;													\
																	\
	sizeTotal += sizeof(RwBool);									\
																	\
	if (exist && size)												\
	{																\
		sizeTotal += (size);										\
	}																\
}

#define	DWSECTOR_STREAM_READ_EXIST(stream, pointer, size)			\
{																	\
	RwBool exist;													\
																	\
	RwStreamRead(stream, &exist, sizeof(RwBool));					\
																	\
	if (exist && size)												\
	{																\
		RwStreamRead(stream, (void *) pointer, (size));				\
	}																\
}

#define DWSECTOR_STREAM_READ_MEMORY(condition, source, destination, size)	\
if (condition)																\
{																			\
	memcpy(destination, source, (size));									\
	pointer += (size);														\
}


typedef struct RpCollisionData RpCollisionData;
struct RpCollisionData 
{
    RwInt32         flags			;
    void		*	tree			;
    RwInt32         numTriangles	;
    RwUInt16	*	triangleMap		;
};

RpCollisionData			*	COL		= NULL;
RpMeshHeader			*	MH		= NULL;
DWSectorWorldSectorData	*	ASDF	= NULL;
RpWorldSector			*	WS1		= NULL;
RpWorldSector			*	WS2		= NULL;

static RwInt8	NumDetail		=	-1;
static RwInt32	IndexSector1	;
static RwInt32	IndexSector2	;

/*
 *****************************************************************************
 */
typedef struct _DWSectorWorldData _DWSectorWorldData;
struct _DWSectorWorldData
{
	RwUInt8				numDetail;				/* Number of detail */
	RwUInt8				modeWrite;				/* Writing Mode */
};

typedef struct _DWSectorWorldSectorData _DWSectorWorldSectorData;
struct _DWSectorWorldSectorData
{
	RwUInt8				numDetail;				/* Number of detail */
	RwInt8				curDetail;				/* Current detail index (-1 when disabled) */
};

/*
 *****************************************************************************
 */
RwStream *
RpDWSectorStreamWrite(RpDWSector *dwSector, RwStream *stream, RpWorldSector *worldSector)
{
	RwInt32		size;
	RwInt32		sizeTotal = 0;
	RwInt32		i;
	RpWorld *	world = RpWorldSectorGetWorld(worldSector);

	{
		RwInt32	srcLength;
		RwInt32	packLength = 0;
		RwInt32	workLength;

		RwChar	*srcBuffer;
		RwChar	*packBuffer;
		RwChar	*workBuffer;
		RwStream *	memstream;
		RwMemory	memory;

		memstream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &memory);
		if (!memstream)
			return NULL;

		if (dwSector->atomic)
			if (!RpAtomicStreamWrite(dwSector->atomic, memstream))
				return NULL;

		RwStreamClose(memstream, &memory);

		srcLength = memory.length;
		workLength = 10000000;

		if (srcLength)
		{
			srcBuffer = memory.start;
			packBuffer = RwMalloc(((srcLength * 9 ) / 8 ) + 16);
			workBuffer = RwMalloc(workLength);

			packLength = aP_pack(srcBuffer, packBuffer, srcLength, workBuffer, NULL);
		}

		sizeTotal = sizeof(packLength) + sizeof(srcLength) + packLength;

		RwStreamWriteChunkHeader(stream, rwID_DWSECTOR_DATA, sizeTotal);

		size = sizeof(packLength);
		if (!RwStreamWrite(stream, (void *) &packLength, size))
			return NULL;
		sizeTotal -= size;

		size = sizeof(srcLength);
		if (!RwStreamWrite(stream, (void *) &srcLength, size))
			return NULL;
		sizeTotal -= size;

		if (srcLength)
		{
			size = packLength;
			if (!RwStreamWrite(stream, (void *) packBuffer, size))
				return NULL;
			sizeTotal -= size;

			RwFree(packBuffer);
			RwFree(workBuffer);
			RwFree(srcBuffer);
		}

		/*
		RwStreamWriteChunkHeader(stream, rwID_DWSECTOR_DATA, RpDWSectorStreamGetSize(dwSector, worldSector));

		if (dwSector->atomic)
			if (!RpAtomicStreamWrite(dwSector->atomic, stream))
				return NULL;
		*/

		/*
		fields = RpDWSectorGetFields(dwSector);

		srcLength = ((fields & rpDWSECTOR_FIELD_VERTICES) ? (sizeof(RwV3d) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_POLYGONS) ? (sizeof(RpPolygon) * dwSector->numPolygons) : 0) +
						  ((fields & rpDWSECTOR_FIELD_NORMALS) ? (sizeof(RpVertexNormal) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_PRELITLUM) ? (sizeof(RwRGBA) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_FLAG_TEXCOORDS) * sizeof(RwTexCoords) * dwSector->numVertices);

		workLength = 10000000;

		if (srcLength)
		{
			srcBuffer = malloc(srcLength);
			packBuffer= malloc(((srcLength * 9 ) / 8 ) + 16);
			workBuffer = malloc(workLength);

			pointer = srcBuffer;

			size = 0;
			DWSECTOR_STREAM_WRITE_MEMORY(dwSector->vertices, pointer, size, sizeof(RwV3d) * dwSector->numVertices);
			DWSECTOR_STREAM_WRITE_MEMORY(dwSector->polygons, pointer, size, sizeof(RpPolygon) * dwSector->numPolygons);
			DWSECTOR_STREAM_WRITE_MEMORY(dwSector->normals, pointer, size, sizeof(RpVertexNormal) * dwSector->numVertices);
			DWSECTOR_STREAM_WRITE_MEMORY(dwSector->preLitLum, pointer, size, sizeof(RwRGBA) * dwSector->numVertices);
			for (i = 0; i < rwMAXTEXTURECOORDS; i++)
			{
				DWSECTOR_STREAM_WRITE_MEMORY(dwSector->texCoords[i], pointer, size, sizeof(RwTexCoords) * dwSector->numVertices);
			}

			packLength = aP_pack(srcBuffer, packBuffer, size, workBuffer, NULL);
		}

		sizeTotal = sizeof(dwSector->numVertices) + sizeof(dwSector->numPolygons) + sizeof(fields) + sizeof(packLength) + packLength;
		RwStreamWriteChunkHeader(stream, rwID_DWSECTOR_DATA, sizeTotal);

		size = sizeof(dwSector->numVertices);
		RwStreamWrite(stream, (const void *) &dwSector->numVertices, size);
		sizeTotal -= size;

		size = sizeof(dwSector->numPolygons);
		RwStreamWrite(stream, (const void *) &dwSector->numPolygons, size);
		sizeTotal -= size;

		size = sizeof(fields);
		RwStreamWrite(stream, (void *) &fields, size);
		sizeTotal -= size;

		size = sizeof(packLength);
		RwStreamWrite(stream, (void *) &packLength, size);
		sizeTotal -= size;

		if (srcLength)
		{
			size = packLength;
			RwStreamWrite(stream, packBuffer, packLength);
			sizeTotal -= size;

			free(srcBuffer);
			free(packBuffer);
			free(workBuffer);
		}
		*/
	}

	/*
	if (sizeTotal)
		return NULL;
	*/


	/*
	DWSECTOR_STREAM_WRITE_EXIST(stream, dwSector->vertices, sizeTotal, sizeof(RwV3d) * dwSector->numVertices)

	DWSECTOR_STREAM_WRITE_EXIST(stream, dwSector->normals, sizeTotal, sizeof(RpVertexNormal) * dwSector->numVertices)

	DWSECTOR_STREAM_WRITE_EXIST(stream, dwSector->polygons, sizeTotal, sizeof(RpPolygon) * dwSector->numPolygons)

	DWSECTOR_STREAM_WRITE_EXIST(stream, dwSector->preLitLum, sizeTotal, sizeof(RwRGBA) * dwSector->numVertices)

	for (i = 0; i < rwMAXTEXTURECOORDS; i++)
	{
		DWSECTOR_STREAM_WRITE_EXIST(stream, dwSector->texCoords[i], sizeTotal, sizeof(RwTexCoords) * dwSector->numVertices)
	}

	_rpMeshWrite(dwSector->mesh, (const void *) world, stream, &world->matList);

	repExist = dwSector->repEntry != NULL;

	size = sizeof(RwInt32);
	RwStreamWrite(stream, (const void *) &repExist, size);
	sizeTotal += size;

	if (repExist)
	{
		DWSectorNativeWrite(stream, worldSector     , dwSector->repEntry);
	}
	*/

	return stream;
}


/*
 *****************************************************************************
 */
RwStream *
RpDWSectorStreamRead(RpDWSector *dwSector, RwStream *stream, RpWorldSector *worldSector)
{
	RwInt32 size;
	RwInt32 i;
	RpWorld *world = RpWorldSectorGetWorld(worldSector);
	RwInt32	repExist;

	{
		RwInt32	srcLength;
		RwInt32	packLength;

		RwChar *	srcBuffer;
		RwChar *	packBuffer;
		RwStream *	memstream;
		RwMemory	memory;

		size = sizeof(packLength);
		RwStreamRead(stream, (void *) &packLength, size);

		size = sizeof(srcLength);
		RwStreamRead(stream, (void *) &srcLength, size);

		if (packLength)
		{
			packBuffer = RwMalloc(packLength);
			srcBuffer = RwMalloc(srcLength);

			if (packLength)
			{
				size = packLength;
				RwStreamRead(stream, (void *) packBuffer, size);

				if (size && srcLength != aP_depack_asm_fast(packBuffer, srcBuffer))
				{
					return NULL;
				}

				memory.start = srcBuffer;
				memory.length = srcLength;

				memstream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &memory);
				if (!memstream)
					return NULL;

				if (RwStreamFindChunk(memstream, rwID_ATOMIC, &size, NULL))
				{
					if (size)
					{
						dwSector->atomic = RpAtomicStreamRead(memstream);
						if (dwSector->atomic)
						{
							AgcmMap::m_pThis->LockFrame();
							RpAtomicSetFrame(dwSector->atomic, RwFrameCreate());
							AgcmMap::m_pThis->UnlockFrame();

							dwSector->geometry = RpAtomicGetGeometry(dwSector->atomic);
						}
					}
				}

				RwStreamClose(memstream, &memory);

				RwFree(srcBuffer);
				RwFree(packBuffer);
			}
		}
	}

	/*
	if( RwStreamFindChunk(stream, rwID_ATOMIC, &size, NULL) )
	{
		if (size)
		{
			dwSector->atomic = RpAtomicStreamRead(stream);
			if (dwSector->atomic)
			{
				RpAtomicSetFrame(dwSector->atomic, RwFrameCreate());
				dwSector->geometry = RpAtomicGetGeometry(dwSector->atomic);
			}
		}
	}
	*/

	/*
	size = sizeof(dwSector->numVertices);
	RwStreamRead(stream, (void *) &dwSector->numVertices, size);

	size = sizeof(dwSector->numPolygons);
	RwStreamRead(stream, (void *) &dwSector->numPolygons, size);

	RpDWSectorInit(dwSector, dwSector->numVertices, dwSector->numPolygons, 8);

	{
		RpDWSectorFields	fields;
		RwInt32	srcLength;
		RwInt32	packLength;
		RwInt32	workLength;

		RwChar	*srcBuffer;
		RwChar	*packBuffer;
		RwChar	*workBuffer;
		RwChar	*pointer;

		size = sizeof(fields);
		RwStreamRead(stream, (void *) &fields, size);

		srcLength = ((fields & rpDWSECTOR_FIELD_VERTICES) ? (sizeof(RwV3d) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_POLYGONS) ? (sizeof(RpPolygon) * dwSector->numPolygons) : 0) +
						  ((fields & rpDWSECTOR_FIELD_NORMALS) ? (sizeof(RpVertexNormal) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_PRELITLUM) ? (sizeof(RwRGBA) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_FLAG_TEXCOORDS) * sizeof(RwTexCoords) * dwSector->numVertices);

		size = sizeof(packLength);
		RwStreamRead(stream, (void *) &packLength, size);

		if (packLength)
		{
			packBuffer = malloc(packLength);
			srcBuffer = malloc(srcLength);

			size = packLength;
			RwStreamRead(stream, (void *) packBuffer, size);

			if (size && srcLength != aP_depack_asm(packBuffer, srcBuffer))
			{
				return NULL;
			}

			pointer = srcBuffer;

			DWSECTOR_STREAM_READ_MEMORY(fields & rpDWSECTOR_FIELD_VERTICES, pointer, dwSector->vertices, sizeof(RwV3d) * dwSector->numVertices);
			DWSECTOR_STREAM_READ_MEMORY(fields & rpDWSECTOR_FIELD_POLYGONS, pointer, dwSector->polygons, sizeof(RpPolygon) * dwSector->numPolygons);
			DWSECTOR_STREAM_READ_MEMORY(fields & rpDWSECTOR_FIELD_NORMALS, pointer, dwSector->normals, sizeof(RpVertexNormal) * dwSector->numVertices);
			DWSECTOR_STREAM_READ_MEMORY(fields & rpDWSECTOR_FIELD_PRELITLUM, pointer, dwSector->vertices, sizeof(RwRGBA) * dwSector->numVertices);
			for (i = 0; i < (fields & rpDWSECTOR_FIELD_FLAG_TEXCOORDS); i++)
			{
				DWSECTOR_STREAM_READ_MEMORY(1, pointer, dwSector->texCoords[i], sizeof(RwTexCoords) * dwSector->numVertices);
			}

			free(packBuffer);
			free(srcBuffer);
		}
	}
	*/

	/*

	DWSECTOR_STREAM_READ_EXIST(stream, dwSector->vertices, sizeof(RwV3d) * dwSector->numVertices);

	DWSECTOR_STREAM_READ_EXIST(stream, dwSector->normals, sizeof(RpVertexNormal) * dwSector->numVertices);

	DWSECTOR_STREAM_READ_EXIST(stream, dwSector->polygons, sizeof(RpPolygon) * dwSector->numPolygons);

	DWSECTOR_STREAM_READ_EXIST(stream, dwSector->preLitLum, sizeof(RwRGBA) * dwSector->numVertices);

	for (i = 0; i < rwMAXTEXTURECOORDS; i++)
	{
		DWSECTOR_STREAM_READ_EXIST(stream, dwSector->texCoords[i], sizeof(RwTexCoords) * dwSector->numVertices);
	}

	dwSector->mesh = _rpMeshRead(stream, (const void *) world, &world->matList);

	size = sizeof(RwInt32);
	RwStreamRead(stream, (const void *) &repExist, size);

	if (repExist)
		dwSector->repEntry = DWSectorNativeRead(stream, worldSector);
	*/

	return stream;
}


/*
 *****************************************************************************
 */
RwInt32
RpDWSectorStreamGetSize(RpDWSector *dwSector, RpWorldSector *worldSector)
{
	RwInt32 size;
	RwInt32 sizeTotal = 0;
	RwInt32 i;
	RpWorld *world = RpWorldSectorGetWorld(worldSector);

	{
		RwInt32	srcLength;
		RwInt32	packLength = 0;
		RwInt32	workLength;

		RwChar	*srcBuffer;
		RwChar	*packBuffer;
		RwChar	*workBuffer;
		RwStream *	memstream;
		RwMemory	memory;

		memstream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &memory);
		if (!memstream)
			return NULL;

		if (dwSector->atomic)
			if (!RpAtomicStreamWrite(dwSector->atomic, memstream))
				return NULL;

		RwStreamClose(memstream, &memory);

		srcLength = memory.length;
		workLength = 10000000;

		if (srcLength)
		{
			srcBuffer = memory.start;
			packBuffer = RwMalloc(((srcLength * 9 ) / 8 ) + 16);
			workBuffer = RwMalloc(workLength);

			packLength = aP_pack(srcBuffer, packBuffer, srcLength, workBuffer, NULL);
		}

		if (srcLength)
		{
			RwFree(packBuffer);
			RwFree(workBuffer);
			RwFree(srcBuffer);
		}

		return sizeof(packLength) + sizeof(srcLength) + packLength + rwCHUNKHEADERSIZE;
	}

	/*
	return (dwSector->atomic ? (RpAtomicStreamGetSize(dwSector->atomic) + rwCHUNKHEADERSIZE) : 0);
	*/

	/*
	{
		RpDWSectorFields	fields;
		RwInt32	srcLength;
		RwInt32	packLength;
		RwInt32	workLength;

		RwChar	*srcBuffer;
		RwChar	*packBuffer;
		RwChar	*workBuffer;
		RwChar	*pointer;

		fields = RpDWSectorGetFields(dwSector);

		srcLength = ((fields & rpDWSECTOR_FIELD_VERTICES) ? (sizeof(RwV3d) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_POLYGONS) ? (sizeof(RpPolygon) * dwSector->numPolygons) : 0) +
						  ((fields & rpDWSECTOR_FIELD_NORMALS) ? (sizeof(RpVertexNormal) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_PRELITLUM) ? (sizeof(RwRGBA) * dwSector->numVertices) : 0) +
						  ((fields & rpDWSECTOR_FIELD_FLAG_TEXCOORDS) * sizeof(RwTexCoords) * dwSector->numVertices);

		workLength = 10000000;

		srcBuffer = malloc(srcLength);
		packBuffer= malloc(((srcLength * 9 ) / 8 ) + 16);
		workBuffer = malloc(workLength);

		pointer = srcBuffer;

		size = 0;
		DWSECTOR_STREAM_WRITE_MEMORY(dwSector->vertices, pointer, size, sizeof(RwV3d) * dwSector->numVertices);
		DWSECTOR_STREAM_WRITE_MEMORY(dwSector->polygons, pointer, size, sizeof(RpPolygon) * dwSector->numPolygons);
		DWSECTOR_STREAM_WRITE_MEMORY(dwSector->normals, pointer, size, sizeof(RpVertexNormal) * dwSector->numVertices);
		DWSECTOR_STREAM_WRITE_MEMORY(dwSector->preLitLum, pointer, size, sizeof(RwRGBA) * dwSector->numVertices);
		for (i = 0; i < rwMAXTEXTURECOORDS; i++)
		{
			DWSECTOR_STREAM_WRITE_MEMORY(dwSector->texCoords[i], pointer, size, sizeof(RwTexCoords) * dwSector->numVertices);
		}

		packLength = aP_pack(srcBuffer, packBuffer, size, workBuffer, NULL);

		sizeTotal = sizeof(dwSector->numVertices) + sizeof(dwSector->numPolygons) + sizeof(fields) + sizeof(packLength) + packLength;

		free(srcBuffer);
		free(packBuffer);
		free(workBuffer);
	}
	*/

	/*
	size = sizeof(dwSector->numVertices);
	sizeTotal += size;

	size = sizeof(dwSector->numPolygons);
	sizeTotal += size;

	DWSECTOR_STREAM_SIZE_EXIST(dwSector->vertices, sizeTotal, sizeof(RwV3d) * dwSector->numVertices)

	DWSECTOR_STREAM_SIZE_EXIST(dwSector->normals, sizeTotal, sizeof(RpVertexNormal) * dwSector->numVertices)

	DWSECTOR_STREAM_SIZE_EXIST(dwSector->polygons, sizeTotal, sizeof(RpPolygon) * dwSector->numPolygons)

	DWSECTOR_STREAM_SIZE_EXIST(dwSector->preLitLum, sizeTotal, sizeof(RwRGBA) * dwSector->numVertices)

	for (i = 0; i < rwMAXTEXTURECOORDS; i++)
	{
		DWSECTOR_STREAM_SIZE_EXIST(dwSector->texCoords[i], sizeTotal, sizeof(RwTexCoords) * dwSector->numVertices)
	}

	size = _rpMeshSize(dwSector->mesh, (const void *) world);
	sizeTotal += size;

	size = sizeof(RwInt32);
	sizeTotal += size;

	if (dwSector->repEntry)
	{
		size = DWSectorNativeSize(worldSector, dwSector->repEntry);
		sizeTotal += size;
	}
	*/

	return sizeTotal;
}

/*
 *****************************************************************************
 */
RwStream *
DWSectorWorldSectorDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *worldSector,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	RpWorld *world;
	_DWSectorWorldSectorData as;
	DWSectorWorldSectorData *data;
	RwInt32 i;

	world = RpWorldSectorGetWorld(worldSector);
	if (DWSECTORWORLDDATA(world, modeWrite) == rpDWSECTOR_WRITEMODE_CURRENT)
		return stream;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);

		size = sizeof(_DWSectorWorldSectorData);
		RwStreamRead(stream, (void *)&as, size);

		data->curDetail = as.curDetail;
		data->numDetail = as.numDetail;
		data->orgDetail = as.curDetail;

		RpDWSectorUpdateCurrent(worldSector);

		for (i = 0; i < data->numDetail; i++)
		{
			if (i != data->curDetail)
			{
				if (RwStreamFindChunk(stream, rwID_DWSECTOR_DATA, NULL, NULL))
				{
					RpDWSectorStreamRead(data->details + i, stream, (RpWorldSector *) worldSector);
				}
			}
		}

		return stream;
	}

	return NULL;
}


RwStream *
DWSectorWorldSectorDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *worldSector,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	RpWorld *world;
	_DWSectorWorldSectorData as;
	DWSectorWorldSectorData *data;
	RwUInt32 sizeTotal;
	RwInt32 i;

	world = RpWorldSectorGetWorld(worldSector);
	if (DWSECTORWORLDDATA(world, modeWrite) == rpDWSECTOR_WRITEMODE_CURRENT)
		return stream;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(worldSector);

		sizeTotal = 0;

		as.curDetail = data->curDetail;
		as.numDetail = data->numDetail;

		size = sizeof(_DWSectorWorldSectorData);
		RwStreamWrite(stream, (const void *) &as, size);
		sizeTotal += size;

		for (i = 0; i < data->numDetail; i++)
		{
			if (i != data->curDetail)
			{
				RpDWSectorStreamWrite(data->details + i, stream, (RpWorldSector *) worldSector);
				sizeTotal += RpDWSectorStreamGetSize(data->details + i, (RpWorldSector *) worldSector);
			}
		}

		return stream;
	}

	return NULL;
}


RwInt32
DWSectorWorldSectorDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	RpWorld *world;
	DWSectorWorldSectorData *data;
	RwUInt32 sizeTotal;
	RwInt32 i;

	world = RpWorldSectorGetWorld(object);
	if (DWSECTORWORLDDATA(world, modeWrite) == rpDWSECTOR_WRITEMODE_CURRENT)
		return 0;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLDSECTOR(object);

		sizeTotal = 0;

		size = sizeof(_DWSectorWorldSectorData);
		sizeTotal += size;

		for (i = 0; i < data->numDetail; i++)
		{
			if (i != data->curDetail)
			{
				sizeTotal += rwCHUNKHEADERSIZE;
				sizeTotal += RpDWSectorStreamGetSize(data->details + i, (RpWorldSector *) object);
			}
		}
	}

	return sizeTotal;
}


/*
 *****************************************************************************
 */
RpWorldSector *DWSectorNextDetail(RpWorldSector *worldSector, void *_data)
{
	DWSectorWorldSectorData *data;

	data = DWSECTORWORLDSECTOR(worldSector);

	if (data->curDetail == data->orgDetail)
	{
		if (data->curDetail != 0)
			RpDWSectorSetCurrentDetail(worldSector, 0);
		else
			RpDWSectorSetCurrentDetail(worldSector, 1);
	}
	else
	{
		RpDWSectorSetCurrentDetail(worldSector, (RwUInt8) (data->curDetail + 1));

		if (data->curDetail == data->orgDetail)
			RpDWSectorSetCurrentDetail(worldSector, (RwUInt8) (data->curDetail + 1));
	}

	return worldSector;
}


RpWorldSector *DWSectorOrgDetail(RpWorldSector *worldSector, void *_data)
{
	DWSectorWorldSectorData *data;

	data = DWSECTORWORLDSECTOR(worldSector);

	RpDWSectorSetCurrentDetail(worldSector, data->orgDetail);

	return worldSector;
}


RpWorldSector *DWSectorGetSector(RpWorldSector *worldSector, void *data)
{
	RpWorldSector **subSector = (RpWorldSector **) data;

	if (IndexSector1 == IndexSector2)
	{
		*subSector = worldSector;
		return NULL;
	}

	IndexSector2++;

	return worldSector;
}

/*
RpWorldSector *DWSectorSetDetail(RpWorldSector *worldSector, void *data)
{
	RpWorld *subWorld = (RpWorld *) data;
	RpWorldSector *subSector;
	DWSectorWorldSectorData *srcWSdata, *dstWSdata;

	RpDWSectorUpdateCurrent(worldSector);

	IndexSector2 = 0;

	RpWorldForAllWorldSectors(subWorld, DWSectorGetSector, (void *) &subSector);
	RpDWSectorUpdateCurrent(subSector);
	if (!ASDF)
	{
		ASDF = DWSECTORWORLDSECTOR(subSector);
		WS1 = worldSector;
		WS2 = subSector;
		MH = subSector->mesh;
		COL = *DWSECTORCOLLISIONDATA(worldSector);
	}

	srcWSdata = DWSECTORWORLDSECTOR(subSector);
	dstWSdata = DWSECTORWORLDSECTOR(worldSector);

	DWSectorCopy(dstWSdata->details + srcWSdata->curDetail, srcWSdata->details + srcWSdata->curDetail);

//	dstWSdata->details[srcWSdata->curDetail].mesh = srcWSdata->details[srcWSdata->curDetail].mesh;
//	dstWSdata->details[srcWSdata->curDetail].colData = *DWSECTORCOLLISIONDATA(subSector);

	subSector->mesh = NULL;
	*DWSECTORCOLLISIONDATA(subSector) = NULL;

	IndexSector1++;

	return worldSector;
}
*/

RwStream *
DWSectorWorldDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *world,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	_DWSectorWorldData as;
	DWSectorWorldData *data;
	FILE *fp = stream->Type.file.fpFile;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLD(world);

		RwStreamRead(stream, (void *) &as, sizeof(_DWSectorWorldData));
		data->numDetail = as.numDetail;
		data->modeWrite = as.modeWrite;

		RpDWSectorInitWorld((RpWorld *) world, data->numDetail, 0);
	}

	return stream;
}


RwStream *
DWSectorWorldDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *world,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	_DWSectorWorldData as;
	DWSectorWorldData *data;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		data = DWSECTORWORLD(world);
		as.numDetail = data->numDetail;
		as.modeWrite = data->modeWrite;

		RwStreamWrite(stream, (void *) &as, sizeof(_DWSectorWorldData));
	}

	return stream;
}


RwInt32
DWSectorWorldDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	RwInt32 sizeTotal = 0;

	if (DWSectorWorldSectorDataOffset > 0)
	{
		sizeTotal += sizeof(DWSectorWorldData);
	}

	return sizeTotal;
}
