#include "rwcore.h"
#include "rpworld.h"

#include "acurpdmaterial.h"
#include "dmaterial.h"
#include "dmstream.h"

#include "acurpmtexture.h"

/*
 *****************************************************************************
 */
RwStream *
DMaterialMaterialDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *material,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	DMaterialMaterialData *data;
	RwUInt32 sizeTotal;

	data = DMATERIALMATERIAL(material);

	/* Reset total size read. */
	sizeTotal = 0;

	size = sizeof(DMaterialMaterialData);
	RwStreamRead(stream, (void *) data, size);
	sizeTotal += size;

	return stream;
}


RwStream *
DMaterialMaterialDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *material,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	DMaterialMaterialData *data;
	RwUInt32 sizeTotal;

	data = DMATERIALMATERIAL(material);

	/* Reset total size read. */
	sizeTotal = 0;

	size = sizeof(DMaterialMaterialData);
	RwStreamWrite(stream, (const void *) data, size);
	sizeTotal += size;

	return stream;
}


RwInt32
DMaterialMaterialDataGetStreamSize(const void *object,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	return sizeof(DMaterialMaterialData);
}


/*
 *****************************************************************************
 */
RwStream *
DMaterialAtomicDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *atomic,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	/* Reset total size read. */
	sizeTotal = 0;

	size = sizeof(RwBool);
	RwStreamRead(stream, (void *)&enabled, size);
	sizeTotal += size;

	if (enabled)
	{
		RpDMaterialAtomicEnableDynamic((RpAtomic *) atomic);
	}

	return stream;
}


RwStream *
DMaterialAtomicDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *atomic,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	enabled = DMATERIALATOMICDATA(atomic, enabled);

	/* Reset total size read. */
	sizeTotal = 0;

	/* Write out the flag. */
	size = sizeof(RwBool);
	RwStreamWrite(stream, (const void *)&enabled, size);
	sizeTotal += size;

	return stream;
}


RwInt32
DMaterialAtomicDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	return sizeof(RwBool);
}


/*
 *****************************************************************************
 */
RwStream *
DMaterialWorldDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *world,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	/* Reset total size read. */
	sizeTotal = 0;

	size = sizeof(RwBool);
	RwStreamRead(stream, (void *)&enabled, size);
	sizeTotal += size;

	if (enabled)
	{
		RpDMaterialWorldEnableDynamic((RpWorld *) world);
	}

	return stream;
}


RwStream *
DMaterialWorldDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *world,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	enabled = DMATERIALWORLDDATA(world, enabled);

	/* Reset total size read. */
	sizeTotal = 0;

	/* Write out the flag. */
	size = sizeof(RwBool);
	RwStreamWrite(stream, (const void *)&enabled, size);
	sizeTotal += size;

	return stream;
}


RwInt32
DMaterialWorldDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	return sizeof(RwBool);
}


/*
 *****************************************************************************
 */
RwStream *
DMaterialWorldSectorDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *worldSector,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return stream;
}


RwStream *
DMaterialWorldSectorDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *worldSector,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	return stream;
}


RwInt32
DMaterialWorldSectorDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	return 0;
}
