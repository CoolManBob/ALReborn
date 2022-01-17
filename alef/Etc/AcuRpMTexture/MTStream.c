#include "rwcore.h"
#include "rpworld.h"

#include "acurpmtexture.h"
#include "mtexture.h"
#include "mtstream.h"

/*
 *****************************************************************************
 */
RwStream *
MTextureMaterialDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *material,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	RwTexture **texture;
	//RwTexture *baseTexture;
	//RwTexture *regTexture;
	//RwChar name[128];
	RwChar *key = "원본|";
	RwUInt32 sizeTotal;
	MTextureMaterialData *pData = MTEXTUREMATERIAL(material);
	RwInt16 index;
	RwUInt32 lengthOut;

	/* Reset total size read. */
	sizeTotal = 0;

	/*
	baseTexture = RpMaterialGetTexture((RpMaterial *) material);
	if (baseTexture)
	{
		if (RwTextureGetMaskName(baseTexture) && RwTextureGetMaskName(baseTexture)[0])
		{
			sprintf(name, "%s|%s", RwTextureGetName(baseTexture), RwTextureGetMaskName(baseTexture));
		}
		else
		{
			sprintf(name, "원본|%s", RwTextureGetName(baseTexture));
		}

		regTexture = RwTexDictionaryFindNamedTexture(RwTexDictionaryGetCurrent(), (const RwChar *) name);
		if (regTexture)
		{
			RpMaterialSetTexture(material, regTexture);
		}
		else
		{
			RwTextureSetName(baseTexture, name);
		}
	}
	*/

	size = sizeof(RwInt16);
	RwStreamRead(stream, (void *)&pData->textureNumber, size);
	sizeTotal += size;

	for (index = 0; index < pData->textureNumber; index++)
	{
		texture = pData->texture + index;

		size = sizeof(RpMTextureType);
		RwStreamRead(stream, (void *)&pData->textureType[index], size);
		sizeTotal += size;

		if (pData->textureType[index])
		{
			/* Find the chunk. */
			if (RwStreamFindChunk(stream,
								  (RwUInt32)rwID_TEXTURE,
								  &lengthOut,
								  (RwUInt32 *)NULL))
			{
				/* And load. */
				*texture = RwTextureStreamRead(stream);
				/*
				if (*texture)
				{
					if (RwTextureGetMaskName(*texture) && RwTextureGetMaskName(*texture)[0])
					{
						sprintf(name, "%s|%s", RwTextureGetName(*texture), RwTextureGetMaskName(*texture));
					}
					else
					{
						sprintf(name, "원본|%s", RwTextureGetName(*texture));
					}

					regTexture = RwTexDictionaryFindNamedTexture(RwTexDictionaryGetCurrent(), (const RwChar *) name);
					if (regTexture)
					{
						RwTextureDestroy(*texture);
						*texture = regTexture;
						RwTextureAddRef(regTexture);
					}
					else
					{
						RwTextureSetName(*texture, name);
					}
				}
				*/

				/* Keep count. */
				sizeTotal += lengthOut;// + rwCHUNKHEADERSIZE;
			}
		}
		else
			*texture = NULL;
	}

	return stream;
}


RwStream *
MTextureMaterialDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *material,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	RwTexture *texture;
	const RwTexture *ret;
	RwInt16 index;
	MTextureMaterialData *pData = MTEXTUREMATERIAL(material);

	RwUInt32 sizeTotal;

	/* Reset total size read. */
	sizeTotal = 0;

	/* Write out the flag. */
	size = sizeof(RwInt16);
	RwStreamWrite(stream, (const void *)&pData->textureNumber, size);
	sizeTotal += size;

	for (index = 0; index < pData->textureNumber; index++)
	{
		texture = MTEXTUREMATERIALDATA(material, texture[index]);

		size = sizeof(RpMTextureType);
		RwStreamWrite(stream, (void *)&pData->textureType[index], size);
		sizeTotal += size;

		if (pData->textureType[index])
		{
			size = RwTextureStreamGetSize(texture);// + rwCHUNKHEADERSIZE;
			ret = RwTextureStreamWrite(texture, stream);
			sizeTotal += size;
		}
	}

	return stream;
}


RwInt32
MTextureMaterialDataGetStreamSize(const void *object,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	RwTexture *texture;
	RpMaterial *material = (RpMaterial *) object;
	RwUInt32 sizeTotal;
	RwInt16 index;
	MTextureMaterialData *pData = MTEXTUREMATERIAL(material);

	sizeTotal = 0;

	/* We always write a flag for the texture. */
	sizeTotal += sizeof(RwInt16);

	for (index = 0; index < pData->textureNumber; index++)
	{
		texture = MTEXTUREMATERIALDATA(material, texture[index]);

		sizeTotal += sizeof(pData->textureType[index]);

		/* Is there a texture? */
		if (pData->textureType[index])
		{
			/* Yep. Then get it's size. */
			size = RwTextureStreamGetSize(texture);// + rwCHUNKHEADERSIZE;
			sizeTotal += size;
		}
	}

	/* Return the size. */
	return sizeTotal;
}


/*
 *****************************************************************************
 */
RwStream *
MTextureAtomicDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *atomic,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	/* Reset total size read. */
	sizeTotal = 0;

	/* Read the flag */
	size = sizeof(RwBool);
	RwStreamRead(stream, (void *)&enabled, size);
	sizeTotal += size;

	/* If enabled, enable effect */
	if (enabled)
	{
		RpMTextureAtomicEnableEffect((RpAtomic *) atomic);
	}

	return stream;
}


RwStream *
MTextureAtomicDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *atomic,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	enabled = MTEXTUREATOMICDATA(atomic, enabled);

	/* Reset total size read. */
	sizeTotal = 0;

	/* Write out the flag. */
	size = sizeof(RwBool);
	RwStreamWrite(stream, (const void *)&enabled, size);
	sizeTotal += size;

	return stream;
}


RwInt32
MTextureAtomicDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	return sizeof(RwBool);
}


/*
 *****************************************************************************
 */
RwStream *
MTextureWorldSectorDataReadStream(RwStream *stream,
					  RwInt32 length __RWUNUSED__,
					  void *worldSector,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	/* Reset total size read. */
	sizeTotal = 0;

	/* Read the flag */
	size = sizeof(RwBool);
	RwStreamRead(stream, (void *)&enabled, size);
	sizeTotal += size;

	if (enabled)
	{
	/* If enabled, enable effect */
		RpMTextureWorldSectorEnableEffect((RpWorldSector *) worldSector);
	}

	return stream;
}


RwStream *
MTextureWorldSectorDataWriteStream(RwStream *stream,
					   RwInt32 length __RWUNUSED__,
					   const void *worldSector,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	RwBool enabled;

	RwUInt32 sizeTotal;

	enabled = MTEXTUREWORLDSECTORDATA(worldSector, enabled);

	/* Reset total size read. */
	sizeTotal = 0;

	/* Write out the flag. */
	size = sizeof(RwBool);
	RwStreamWrite(stream, (const void *)&enabled, size);
	sizeTotal += size;

	return stream;
}


RwInt32
MTextureWorldSectorDataGetStreamSize(const void *object __RWUNUSED__,
						 RwInt32 offset __RWUNUSED__,
						 RwInt32 size __RWUNUSED__)
{
	return sizeof(RwBool);
}
