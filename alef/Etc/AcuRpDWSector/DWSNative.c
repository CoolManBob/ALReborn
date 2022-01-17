
#include "rwcore.h"

#include "rpworld.h"

#include "aplib.h"

//#include "D3D8pipe.h"
//#include "D3D8VertexBufferManager.h"
#include "d3d8.h"

#include "dwsnative.h"

extern _rwD3D8ResourceEntryInstanceDataDestroy();
extern _rxD3D8VertexBufferManagerCreate();

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

								 WORLD SECTOR

   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

RwStream *DWSectorNativeWrite(RwStream *stream, const RpWorldSector *sector, RwResEntry *repEntry)
{
	RpWorld			*world;
	RwPlatformID	id = rwID_PCD3D8;

	world = RpWorldSectorGetWorld(sector);

	/* Write some native data */
//	if (rpWORLDNATIVE & RpWorldGetFlags(world))
	{
		RxD3D8ResEntryHeader	*resEntryHeader;
		RxD3D8InstanceData	  *instancedData;
		RwInt32				 numMeshes;
		RwUInt32				size;

		/* Calculate total size */
		size = sizeof(RwPlatformID) + sizeof(RwInt32) + repEntry->size;

		resEntryHeader = (RxD3D8ResEntryHeader *) (repEntry + 1);

		instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
		numMeshes = resEntryHeader->numMeshes;

		while (numMeshes--)
		{
			size += instancedData->numIndices * sizeof(RxVertexIndex);
			size += instancedData->numVertices * instancedData->stride;

			instancedData++;
		}

		/* Convert the material pointers to index for serialization */
		instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
		numMeshes = resEntryHeader->numMeshes;

		while (numMeshes--)
		{
			instancedData->material = (RpMaterial *)
				_rpMaterialListFindMaterialIndex(&world->matList,
												 instancedData->material);
			instancedData++;
		}

		/* Write a chunk header so we get a VERSION NUMBER
		if (!RwStreamWriteChunkHeader(stream, rwID_STRUCT, size))
		{
			return NULL;
		} */

		/* Write a platform unique identifier */
		if (!RwStreamWrite(stream, (const void *) &id, sizeof(RwPlatformID)))
		{
			return NULL;
		}

		/* Write header size */
		if (!RwStreamWrite(stream, (void *) &(repEntry->size), sizeof(RwInt32)))
		{
			return NULL;
		}

		/* Write header */
		if (!RwStreamWrite(stream, (void *) (repEntry + 1),
						   repEntry->size))
		{
			return NULL;
		}

		/* Write native mesh info */
		instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
		numMeshes = resEntryHeader->numMeshes;

		while (numMeshes--)
		{
			RxVertexIndex	*indexBuffer;
			RwUInt8			*vertexBuffer;

			/* Write index information */
			if (D3D_OK == IDirect3DIndexBuffer8_Lock((LPDIRECT3DINDEXBUFFER8)instancedData->indexBuffer,
												0, 0, (RwUInt8 **)&indexBuffer, 0))
			{
				void *rv;

				rv = RwStreamWrite(stream, (void *)indexBuffer, instancedData->numIndices * sizeof(RxVertexIndex));

				IDirect3DIndexBuffer8_Unlock((LPDIRECT3DINDEXBUFFER8)instancedData->indexBuffer);

				if (!rv)
				{
					return NULL;
				}
			}

			/* Write vertex information */
			if (D3D_OK == IDirect3DVertexBuffer8_Lock((LPDIRECT3DVERTEXBUFFER8)instancedData->vertexBuffer,
												instancedData->baseIndex * instancedData->stride,
												instancedData->numVertices * instancedData->stride,
												&vertexBuffer,
												D3DLOCK_NOSYSLOCK))
			{
				void *rv;

				rv = RwStreamWrite(stream, (void *)vertexBuffer, instancedData->numVertices * instancedData->stride);

				IDirect3DVertexBuffer8_Unlock((LPDIRECT3DVERTEXBUFFER8)instancedData->vertexBuffer);

				if (!rv)
				{
					return NULL;
				}
			}

			instancedData++;
		}

		/* Patch the material pointers back */
		instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
		numMeshes = resEntryHeader->numMeshes;

		while (numMeshes--)
		{
			instancedData->material =
				_rpMaterialListGetMaterial(&world->matList,
										   (RwInt32)
										   (instancedData->material));
			instancedData++;
		}
	}

	return stream;
}

RwResEntry *DWSectorNativeRead(RwStream *stream, RpWorldSector *sector)
{
	RwResEntry				*repEntry;
	RpWorld				 *world;
	RwUInt32				version;
	RwUInt32				size;
	RwUInt32				headerSize;
	RwPlatformID			id;
	RxD3D8ResEntryHeader	*resEntryHeader;
	RxD3D8InstanceData	  *instancedData;
	RwInt32				 numMeshes;
	RwBool				  error;

	world = RpWorldSectorGetWorld(sector);

	/*
	if (!RwStreamFindChunk(stream, rwID_STRUCT, &size, &version))
	{
		return NULL;
	}

	if ((version < rwLIBRARYBASEVERSION) ||
		(version > rwLIBRARYCURRENTVERSION))
	{
		return NULL;
	}
	*/

	/* Read the platform unique identifier */
	if (RwStreamRead(stream, (void *) &id, sizeof(RwPlatformID)) !=
		sizeof(RwPlatformID))
	{
		return NULL;
	}

	/* Remove ID from size */
	size -= sizeof(RwPlatformID);

	/* Check this data is funky for this platfrom */
	if (rwID_PCD3D8 != id)
	{
		return NULL;
	}

	/* Read the header size */
	if (RwStreamRead(stream, (void *) &headerSize, sizeof(RwInt32)) !=
		sizeof(RwInt32))
	{
		return NULL;
	}

	/* Remove header size from size */
	size -= sizeof(RwInt32);

	repEntry = (RwResEntry *)RwMalloc(sizeof(RwResEntry) + headerSize);

	/* We have an entry */
	repEntry->link.next = (RwLLLink *)NULL;
	repEntry->link.prev = (RwLLLink *)NULL;
	repEntry->owner = (void *)sector;
	repEntry->size = headerSize;
	repEntry->ownerRef = &repEntry;
	repEntry->destroyNotify = _rwD3D8ResourceEntryInstanceDataDestroy;

	/* Read some native data */
	if (RwStreamRead(stream, (void *) (repEntry + 1), headerSize) != headerSize)
	{
		return NULL;
	}

	/* Remove header from size */
	size -= headerSize;

	/* Fix wrong pointers */
	resEntryHeader = (RxD3D8ResEntryHeader *) (repEntry + 1);

	instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
	numMeshes = resEntryHeader->numMeshes;

	while (numMeshes--)
	{
		instancedData->indexBuffer = NULL;
		instancedData->vertexBuffer = NULL;

		instancedData++;
	}

	/* Load index and vertex data */
	error = FALSE;

	instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
	numMeshes = resEntryHeader->numMeshes;

	while (numMeshes-- && !error)
	{
		RwUInt32 bytesRead;

		error = TRUE;

		/* read index data */
		if (RwD3D8IndexBufferCreate(instancedData->numIndices, &(instancedData->indexBuffer)))
		{
			RwUInt32		ibSize;
			RxVertexIndex   *indexBuffer;

			ibSize = instancedData->numIndices * sizeof(RxVertexIndex);

			if (D3D_OK == IDirect3DIndexBuffer8_Lock((LPDIRECT3DINDEXBUFFER8)instancedData->indexBuffer,
													0, 0, (RwUInt8 **)&indexBuffer, 0))
			{
				bytesRead = RwStreamRead(stream, (void *)indexBuffer, ibSize);

				IDirect3DIndexBuffer8_Unlock((LPDIRECT3DINDEXBUFFER8)instancedData->indexBuffer);

				if (bytesRead == ibSize)
				{
					size -= bytesRead;

					error = FALSE;
				}
			}
		}

		/* read vertex data */
		if (!error)
		{
			RwUInt32		vbSize;
			RwUInt8		 *vertexBuffer;

			error = TRUE;

			vbSize = instancedData->numVertices * instancedData->stride;

			instancedData->managed = TRUE;

			if (_rxD3D8VertexBufferManagerCreate(instancedData->vertexShader, vbSize,
														&instancedData->vertexBuffer,
														&instancedData->baseIndex))
			{
				error = FALSE;
			}

			if (!error)
			{
				error = TRUE;

				IDirect3DVertexBuffer8_Lock((LPDIRECT3DVERTEXBUFFER8)instancedData->vertexBuffer,
									 instancedData->baseIndex * instancedData->stride,
									 vbSize,
									 &vertexBuffer,
									 D3DLOCK_NOSYSLOCK);

				bytesRead = RwStreamRead(stream, (void *)vertexBuffer, vbSize);

				IDirect3DVertexBuffer8_Unlock((LPDIRECT3DVERTEXBUFFER8)instancedData->vertexBuffer);

				if (bytesRead == vbSize)
				{
					size -= bytesRead;

					error = FALSE;
				}
			}
		}

		instancedData++;
	}

	if (error)
	{
		_rwD3D8ResourceEntryInstanceDataDestroy(repEntry);

		return NULL;
	}

	/* Patch the material pointers back */
	instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
	numMeshes = resEntryHeader->numMeshes;

	while (numMeshes--)
	{
		instancedData->material =
			_rpMaterialListGetMaterial(&world->matList,
									   (RwInt32)
									   (instancedData->material));
		instancedData++;
	}

	return repEntry;
}

RwInt32 DWSectorNativeSize(const RpWorldSector *sector, RwResEntry *repEntry)
{
	RwInt32 size = 0;
	RpWorld *world;

	world = RpWorldSectorGetWorld(sector);

	/*
	 * Calculate the native data size
	 */
//	if (rpWORLDNATIVE & RpWorldGetFlags(world) &&
//		(NULL != repEntry))
	{
		RxD3D8ResEntryHeader	*resEntryHeader;
		RxD3D8InstanceData	  *instancedData;
		RwInt32				 numMeshes;

		/* Calculate total size */
		size = sizeof(RwPlatformID) + sizeof(RwInt32) + repEntry->size;

		resEntryHeader = (RxD3D8ResEntryHeader *) (repEntry + 1);

		instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);
		numMeshes = resEntryHeader->numMeshes;

		while (numMeshes--)
		{
			size += instancedData->numIndices * sizeof(RxVertexIndex);
			size += instancedData->numVertices * instancedData->stride;

			instancedData++;
		}
	}

	return size;
}
