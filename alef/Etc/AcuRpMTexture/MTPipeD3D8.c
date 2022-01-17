#include "rwcore.h"
#include "rpworld.h"

#include "mtexture.h"
#include "mtpipe.h"

#ifdef D3D8

#include <D3D8.h>
#include <d3dx8.h>

/* Multi-Texture Pipeline */
static RxPipeline	*MTextureAtomicPipe;
static RxPipeline	*MTextureWorldSectorPipe;

/* Old Pipeline Render Callback */
static RxD3D8AllInOneRenderCallBack DefaultAtomicRenderCallback = NULL;
static RxD3D8AllInOneRenderCallBack DefaultWorldSectorRenderCallback = NULL;

static RwInt32	MaxTexturePerPass = 0;
static RwInt32	MaxTextureStage = 0;

/*
 *****************************************************************************
 */
void
MTextureRenderCallBack(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	RxD3D8ResEntryHeader	*resEntryHeader;
	RxD3D8InstanceData		*instancedData;
	RwInt32					numMeshes;
	RwBool					lighting;
	RwBool					vertexAlphaBlend;
	RwBool					forceBlack;
	RwUInt32				ditherEnable;
	RwUInt32				shadeMode;
	void					*lastVertexBuffer;
	RwInt16					currentTexture;
	MTextureMaterialData	*pData;
	RwInt16					index;

	if ( type == rpATOMIC && !MTEXTUREATOMICDATA(object, enabled) )
	{
		DefaultAtomicRenderCallback(repEntry, object, type, flags);
		return;
	}

	if ( type != rpATOMIC && !MTEXTUREWORLDSECTORDATA(object, enabled) )
	{
		DefaultWorldSectorRenderCallback(repEntry, object, type, flags);
		return;
	}

	/* Get lighting state */
	RwD3D8GetRenderState(D3DRS_LIGHTING, &lighting);

	forceBlack = FALSE;

	if (lighting)
	{
		if (flags & rxGEOMETRY_PRELIT)
		{
			/* Emmisive color from the vertex colors */
			RwD3D8SetRenderState(D3DRS_COLORVERTEX, TRUE);
			RwD3D8SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
		}
		else
		{
			/* Emmisive color from material, set to black in the submit node */
			RwD3D8SetRenderState(D3DRS_COLORVERTEX, FALSE);
			RwD3D8SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
		}
	}
	else
	{
		if ((flags & rxGEOMETRY_PRELIT) == 0)
		{
			forceBlack = TRUE;

			RwD3D8GetRenderState(D3DRS_DITHERENABLE, &ditherEnable);
			RwD3D8GetRenderState(D3DRS_SHADEMODE, &shadeMode);

			RwD3D8SetRenderState(D3DRS_TEXTUREFACTOR, 0xff000000);
			RwD3D8SetRenderState(D3DRS_DITHERENABLE, FALSE);
			RwD3D8SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		}
	}

	/* Enable clipping */
	if (type == rpATOMIC)
	{
		RpAtomic				*atomic;
		RwCamera				*cam;
		RpGeometry				*geometry;

		atomic = (RpAtomic *)object;
		geometry = RpAtomicGetGeometry(atomic);

		cam = RwCameraGetCurrentCamera();

		if (RwD3D8CameraIsSphereFullyInsideFrustum(cam, RpAtomicGetWorldBoundingSphere(atomic)))
		{
			RwD3D8SetRenderState(D3DRS_CLIPPING, FALSE);
		}
		else
		{
			RwD3D8SetRenderState(D3DRS_CLIPPING, TRUE);
		}
	}
	else
	{
		RpWorldSector   *worldSector;
		RwCamera		*cam;

		worldSector = (RpWorldSector *)object;

		cam = RwCameraGetCurrentCamera();

		if (RwD3D8CameraIsBBoxFullyInsideFrustum(cam, RpWorldSectorGetTightBBox(worldSector)))
		{
			RwD3D8SetRenderState(D3DRS_CLIPPING, FALSE);
		}
		else
		{
			RwD3D8SetRenderState(D3DRS_CLIPPING, TRUE);
		}
	}

	/* Set texture to NULL if hasn't any texture flags */
	if ( (flags & (rxGEOMETRY_TEXTURED | rpGEOMETRYTEXTURED2)) == 0)
	{
		RwD3D8SetTexture(NULL, 0);

		if (forceBlack)
		{
			RwD3D8SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

			RwD3D8SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		}
	}

	/* Get vertex alpha Blend state */
	vertexAlphaBlend = _rwD3D8RenderStateIsVertexAlphaEnable();

	/* Set Last vertex buffer to force the call */
	lastVertexBuffer = (void *)0xffffffff;

	/* Get the instanced data */
	resEntryHeader = (RxD3D8ResEntryHeader *)(repEntry + 1);
	instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);

	/*
	 * Data shared between meshes
	 */

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D8SetPixelShader(0);

	/*
	 * Vertex shader
	 */
	RwD3D8SetVertexShader(instancedData->vertexShader);

	/* Get the number of meshes */
	numMeshes = resEntryHeader->numMeshes;
	while (numMeshes--)
	{
		if (instancedData->vertexAlpha ||
			(0xFF != instancedData->material->color.alpha))
		{
			if (!vertexAlphaBlend)
			{
				vertexAlphaBlend = TRUE;

				_rwD3D8RenderStateVertexAlphaEnable(TRUE);
			}
		}
		else
		{
			if (vertexAlphaBlend)
			{
				vertexAlphaBlend = FALSE;

				_rwD3D8RenderStateVertexAlphaEnable(FALSE);
			}
		}

		if (lighting)
		{
			if (instancedData->vertexAlpha)
			{
				RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			}
			else
			{
				RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
			}

			RwD3D8SetSurfaceProperties(&instancedData->material->color,
										&instancedData->material->surfaceProps,
										(flags & rxGEOMETRY_MODULATE));
		}

		/*
		 * Render
		 */

		/* Set the stream source */
		if (lastVertexBuffer != instancedData->vertexBuffer)
		{
			RwD3D8SetStreamSource(0, instancedData->vertexBuffer, instancedData->stride);

			lastVertexBuffer = instancedData->vertexBuffer;
		}

		pData = MTEXTUREMATERIAL(instancedData->material);
		currentTexture = 0;

		while (currentTexture <= pData->textureNumber)
		{
			if ( (flags & (rxGEOMETRY_TEXTURED | rpGEOMETRYTEXTURED2)) )
			{
				// Single-pass texturing 을 먼저 한다. Lighting의 영향을 고려해서 한 Pass에서 할 수 있는 최대한의 Multi-Texturing을 한다.
				for (index = 0;
					index < MaxTexturePerPass && index < MaxTextureStage - 1 && currentTexture <= pData->textureNumber;
					index++, currentTexture++)
				{
					if (currentTexture == 0)
					{
						RwD3D8SetTexture(instancedData->material->texture,		index);
						RwD3D8SetTextureStageState(index, D3DTSS_TEXCOORDINDEX,	index);

						RwD3D8SetTextureStageState(index, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
						RwD3D8SetTextureStageState(index, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
						RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
					}
					else
					{
						while (!pData->textureType[currentTexture - 1] && currentTexture <= pData->textureNumber)
							currentTexture++;

						RwD3D8SetTexture(pData->texture[currentTexture - 1],	index);
						RwD3D8SetTextureStageState(index, D3DTSS_TEXCOORDINDEX,	currentTexture);

						if (index)
						{
							if (pData->textureType[currentTexture - 1] == rpMTEXTURE_TYPE_ALPHA)
							{
								// Alpha를 적용하려면, Single Pass에서 한 장을 더 처리할 수 있어야 되고, 
								// Lighting을 고려하여 1 pass를 더 남겨놔야 한다. 만약 이 조건이 허락되지 않으면, 다음 pass로 넘긴다.
								if (index >= MaxTexturePerPass - 1 || index >= MaxTextureStage - 1)
									break;

								RwD3D8SetTextureStageState(index, D3DTSS_COLOROP,	D3DTOP_SELECTARG2);
								RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);
							}
							else if (pData->textureType[currentTexture - 1] == rpMTEXTURE_TYPE_NORMALALPHA)
							{
								RwD3D8SetTextureStageState(index, D3DTSS_COLOROP,	D3DTOP_BLENDTEXTUREALPHA);
								RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP,	D3DTOP_BLENDTEXTUREALPHA);
							}
							else if (pData->textureType[currentTexture - 1] == rpMTEXTURE_TYPE_NORMAL)
							{
								RwD3D8SetTextureStageState(index, D3DTSS_COLOROP,	D3DTOP_BLENDCURRENTALPHA);
								RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP,	D3DTOP_BLENDCURRENTALPHA);
							}

							RwD3D8SetTextureStageState(index, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
							RwD3D8SetTextureStageState(index, D3DTSS_COLORARG2,	D3DTA_CURRENT);
							RwD3D8SetTextureStageState(index, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
							RwD3D8SetTextureStageState(index, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);
						}
						else
						{
							RwD3D8SetTextureStageState(index, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
							RwD3D8SetTextureStageState(index, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
							RwD3D8SetTextureStageState(index, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
							RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
						}
					}
				}

				RwD3D8SetTextureStageState(index, D3DTSS_COLORARG1, D3DTA_CURRENT);
				RwD3D8SetTextureStageState(index, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				RwD3D8SetTextureStageState(index, D3DTSS_COLOROP, D3DTOP_MODULATE);
				RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		
				if (forceBlack)
				{
					/* Only change the colorop, we need to use the texture alpha channel */
					RwD3D8SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
					RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
				}
			}

			/* Set the Index buffer */
			if (instancedData->indexBuffer != NULL)
			{
				RwD3D8SetIndices(instancedData->indexBuffer, instancedData->baseIndex);

				/* Draw the indexed primitive */
				RwD3D8DrawIndexedPrimitive((D3DPRIMITIVETYPE)instancedData->primType,
										   0, instancedData->numVertices,
										   0, instancedData->numIndices);
			}
			else
			{
				RwD3D8DrawPrimitive((D3DPRIMITIVETYPE)instancedData->primType,
									instancedData->baseIndex,
									instancedData->numVertices);
			}

			if ( !(flags & (rxGEOMETRY_TEXTURED | rpGEOMETRYTEXTURED2)) )
				break;
		}

		for (index = 0; index < 8; index++)
		{
			RwD3D8SetTexture(NULL, index);
			RwD3D8SetTextureStageState(index, D3DTSS_TEXCOORDINDEX,	0);
			RwD3D8SetTextureStageState(index, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D8SetTextureStageState(index, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}

		/* Move onto the next instancedData */
		instancedData++;
	}

	if (forceBlack)
	{
		RwD3D8SetRenderState(D3DRS_DITHERENABLE, ditherEnable);
		RwD3D8SetRenderState(D3DRS_SHADEMODE, shadeMode);

		if (_rwD3D8RWGetRasterStage(0))
		{
			RwD3D8SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		}
		else
		{
			RwD3D8SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
			RwD3D8SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		}
	}

	return;
}


RwBool MTextureWorldSectorInstanceCallback(void *object, RxD3D8InstanceData *instancedData, RwBool reinstance)
{
	const RpWorldSector *sector;
	RpGeometryFlag flags;
	RwUInt32 numVertices;
	RwUInt32 vbSize;
	RwInt32 offset;
	RwUInt32 fvfFlags;
	RwUInt8 *lockedVertexBuffer;
	RwUInt8 *vertexBuffer;
	const RwV3d *pos;

	sector = (const RpWorldSector *)object;
	flags = (RpGeometryFlag)
	RpWorldGetFlags((RpWorld *)RWSRCGLOBAL(curWorld));

	/* Positions */
	instancedData->stride = sizeof(RwV3d);
	fvfFlags = D3DFVF_XYZ;

	/* Normals */
	if (flags & rxGEOMETRY_NORMALS)
	{
		instancedData->stride += sizeof(RwV3d);
		fvfFlags |= D3DFVF_NORMAL;
	}

	/* Pre-lighting */
	if (flags & rxGEOMETRY_PRELIT)
	{
		instancedData->stride += sizeof(RwRGBA);
		fvfFlags |= D3DFVF_DIFFUSE;
	}

	/* mdr: the default stuff */

	/* Texture coordinates */
	/*if (flags & rxGEOMETRY_TEXTURED)
	{
	instancedData->stride += sizeof(RwTexCoords);
	fvfFlags |= (D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
	}
	else if (flags & rpWORLDTEXTURED2)
	{
	instancedData->stride += (sizeof(RwTexCoords) << 1);
	fvfFlags |= (D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1));
	}*/

	/* mdr: i am only going to instance 3 texcoords set for this example */
	instancedData->stride += sizeof(RwTexCoords) * 6;
	fvfFlags |= (D3DFVF_TEX6 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) | D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE2(5));

	/*
	* Create the vertex buffer
	*/
	vbSize = instancedData->stride * instancedData->numVertices;

	if (FALSE == RwD3D8DynamicVertexBufferCreate(fvfFlags, vbSize, &instancedData->vertexBuffer))
	{
		return(FALSE);
	}

	/*
	* Lock the vertex buffer
	*/
	IDirect3DVertexBuffer8_Lock((LPDIRECT3DVERTEXBUFFER8)instancedData->vertexBuffer,
	instancedData->baseIndex * instancedData->stride, vbSize, &lockedVertexBuffer, 0);

	/* Positions */
	pos = (const RwV3d *)&sector->vertices[instancedData->minVert];

	vertexBuffer = lockedVertexBuffer;
	numVertices = instancedData->numVertices;
	while (numVertices--)
	{
		*((RwV3d *)vertexBuffer) = *pos;
		vertexBuffer += instancedData->stride;
		pos++;
	}

	offset = sizeof(RwV3d);

	/* Normals */
	if (flags & rxGEOMETRY_NORMALS)
	{
		const RpVertexNormal *normal;

		normal = (const RpVertexNormal *)&(sector->normals[instancedData->minVert]);

		vertexBuffer = lockedVertexBuffer + offset;
		numVertices = instancedData->numVertices;
		while (numVertices--)
		{
			RPV3DFROMVERTEXNORMAL(*((RwV3d *)vertexBuffer), *normal);
			vertexBuffer += instancedData->stride;
			normal++;
		}

		offset += sizeof(RwV3d);
	}

	/* Pre-lighting */
	if (flags & rxGEOMETRY_PRELIT)
	{
		const RwRGBA *color;
		const RwRGBA *matColor;
		RwUInt32 alpha;

		color = (const RwRGBA *)&(sector->preLitLum[instancedData->minVert]);

		matColor = (const RwRGBA *)RpMaterialGetColor(instancedData->material);

		vertexBuffer = lockedVertexBuffer + offset;
		numVertices = instancedData->numVertices;

		alpha = 0xff;

		if ( (flags & rxGEOMETRY_MODULATE) != 0 && (*((const RwUInt32 *)matColor) != 0xffffffff) )
		{
			while (numVertices--)
			{
				*((RwUInt32 *)vertexBuffer) =
				(((color->alpha * (matColor->alpha + 1)) & 0xff00) << (24-8)) |
				(((color->red * (matColor->red + 1)) & 0xff00) << (16-8)) |
				(((color->green * (matColor->green + 1)) & 0xff00) << (8-8)) |
				((color->blue * (matColor->blue + 1)) >> 8);

				/* Does the pre-light contain alpha */
				alpha &= color->alpha;

				vertexBuffer += instancedData->stride;
				color++;
			}
		}
		else
		{
			while (numVertices--)
			{
				*((RwUInt32 *)vertexBuffer) = ((color->alpha << 24) |
				(color->red << 16) |
				(color->green << 8) |
				(color->blue));

				/* Does the pre-light contain alpha */
				alpha &= color->alpha;

				vertexBuffer += instancedData->stride;
				color++;
			}
		}

		instancedData->vertexAlpha = (alpha != 0xff);

		offset += sizeof(RwUInt32);
	}
	else
	{
		instancedData->vertexAlpha = FALSE;
	}

	/* mdr: the default stuff */

	/* Texture coordinates */
	/*if (flags & rxGEOMETRY_TEXTURED)
	{
	const RwTexCoords *texCoord;

	texCoord = (const RwTexCoords *)(((const RwUInt8 *)(sector->texCoords[0][instancedData->minVert])));

	vertexBuffer = lockedVertexBuffer + offset;
	numVertices = instancedData->numVertices;
	while (numVertices--)
	{
	*((RwTexCoords *)vertexBuffer) = *texCoord;
	vertexBuffer += instancedData->stride;
	texCoord++;
	} 
	}
	else if (flags & rpWORLDTEXTURED2)
	{
	const RwTexCoords *texCoord1;
	const RwTexCoords *texCoord2;

	texCoord1 = (const RwTexCoords *)(((const RwUInt8 *)(sector->texCoords[0][instancedData->minVert])));
	texCoord2 = (const RwTexCoords *)(((const RwUInt8 *)(sector->texCoords[1][instancedData->minVert])));

	vertexBuffer = lockedVertexBuffer + offset;
	numVertices = instancedData->numVertices;
	while (numVertices--)
	{
	*((RwTexCoords *)vertexBuffer) = *texCoord1;
	texCoord1++;

	*(((RwTexCoords *)vertexBuffer) + 1) = *texCoord2;
	texCoord2++;

	vertexBuffer += instancedData->stride;
	} 
	}*/

	/* mdr: my stuff */
	{
		const RwTexCoords *texCoord1;
		const RwTexCoords *texCoord2;
		const RwTexCoords *texCoord3;
		const RwTexCoords *texCoord4;
		const RwTexCoords *texCoord5;
		const RwTexCoords *texCoord6;

		texCoord1 = (const RwTexCoords *)&(sector->texCoords[0][instancedData->minVert]);
		texCoord2 = (const RwTexCoords *)&(sector->texCoords[1][instancedData->minVert]);
		texCoord3 = (const RwTexCoords *)&(sector->texCoords[2][instancedData->minVert]);
		texCoord4 = (const RwTexCoords *)&(sector->texCoords[3][instancedData->minVert]);
		if (!sector->texCoords[4])
			texCoord5 = NULL;
		else
			texCoord5 = (const RwTexCoords *)&(sector->texCoords[4][instancedData->minVert]);
		if (!sector->texCoords[5])
			texCoord6 = NULL;
		else
			texCoord6 = (const RwTexCoords *)&(sector->texCoords[5][instancedData->minVert]);

		vertexBuffer = lockedVertexBuffer + offset;
		numVertices = instancedData->numVertices;
		while (numVertices--)
		{
			*((RwTexCoords *)vertexBuffer) = *texCoord1;
			texCoord1++;

			*(((RwTexCoords *)vertexBuffer) + 1) = *texCoord2;
			texCoord2++;

			*(((RwTexCoords *)vertexBuffer) + 2) = *texCoord3;
			texCoord3++;

			*(((RwTexCoords *)vertexBuffer) + 3) = *texCoord4;
			texCoord4++;

			if( texCoord5 )
			{
				*(((RwTexCoords *)vertexBuffer) + 4) = *texCoord5;
				texCoord5++;
			}

			if( texCoord6 )
			{
				*(((RwTexCoords *)vertexBuffer) + 5) = *texCoord6;
				texCoord6++;
			}

			vertexBuffer += instancedData->stride;
		}
	}

	/*
	* Unlock the vertex buffer
	*/
	IDirect3DVertexBuffer8_Unlock((LPDIRECT3DVERTEXBUFFER8)instancedData->vertexBuffer);

	/*
	* Set the vertex shader flags
	*/
	instancedData->vertexShader = fvfFlags;

	return(TRUE);
}


/*
 ***************************************************************************
 */
RxPipeline *
MTextureCreateAtomicPipeline(void)
{
	RxPipeline  *pipe;

	pipe = RxPipelineCreate();
	if (pipe)
	{
		RxLockedPipe	*lpipe;

		lpipe = RxPipelineLock(pipe);
		if (NULL != lpipe)
		{
			RxNodeDefinition	*instanceNode;

			/*
			 * Get the instance node definition
			 */
			instanceNode = RxNodeDefinitionGetD3D8AtomicAllInOne();

			/*
			 * Add the node to the pipeline
			 */
			lpipe = RxLockedPipeAddFragment(lpipe, NULL, instanceNode, NULL);

			/*
			 * Unlock the pipeline
			 */
			lpipe = RxLockedPipeUnlock(lpipe);

			return pipe;
		}

		RxPipelineDestroy(pipe);
	}

	return NULL;
}

RxPipeline *
MTextureCreateWorldSectorPipeline(void)
{
	RxPipeline  *pipe;

	pipe = RxPipelineCreate();
	if (pipe)
	{
		RxLockedPipe	*lpipe;

		lpipe = RxPipelineLock(pipe);
		if (NULL != lpipe)
		{
			RxNodeDefinition	*instanceNode;

			/*
			 * Get the instance node definition
			 */
			instanceNode = RxNodeDefinitionGetD3D8WorldSectorAllInOne();

			/*
			 * Add the node to the pipeline
			 */
			lpipe = RxLockedPipeAddFragment(lpipe, NULL, instanceNode, NULL);

			/*
			 * Unlock the pipeline
			 */
			lpipe = RxLockedPipeUnlock(lpipe);

			return pipe;
		}

		RxPipelineDestroy(pipe);
	}

	return NULL;
}

/*
 ***************************************************************************
 */
RwBool
MTexturePipeOpen()
{
	RxNodeDefinition	*instanceAtomicNode;
	RxNodeDefinition	*instanceWorldSectorNode;
	RxPipelineNode		*nodeAtomic;
	RxPipelineNode		*nodeWorldSector;
	const D3DCAPS8 *caps;

	caps = RwD3D8GetCaps();

	MaxTexturePerPass = caps->MaxSimultaneousTextures;
	MaxTextureStage = caps->MaxTextureBlendStages;

	/*
	 * Create a new atomic pipeline
	 */
	MTextureAtomicPipe = MTextureCreateAtomicPipeline();
	MTextureWorldSectorPipe = MTextureCreateWorldSectorPipeline();

	/*
	 * Get the instance node definition
	 */
	instanceAtomicNode = RxNodeDefinitionGetD3D8AtomicAllInOne();
	instanceWorldSectorNode = RxNodeDefinitionGetD3D8WorldSectorAllInOne();

	/*
	 * Set the pipeline specific data
	 */
	nodeAtomic = RxPipelineFindNodeByName(MTextureAtomicPipe, instanceAtomicNode->name, NULL, NULL);
	nodeWorldSector = RxPipelineFindNodeByName(MTextureWorldSectorPipe, instanceWorldSectorNode->name, NULL, NULL);

	/*
	 * Cache the default render callback
	 */
	DefaultAtomicRenderCallback = RxD3D8AllInOneGetRenderCallBack(nodeAtomic);
	DefaultWorldSectorRenderCallback = RxD3D8AllInOneGetRenderCallBack(nodeWorldSector);

	/*
	 * Set the VShader render callback
	 */
	RxD3D8AllInOneSetRenderCallBack(nodeAtomic, MTextureRenderCallBack);
	RxD3D8AllInOneSetRenderCallBack(nodeWorldSector, MTextureRenderCallBack);

	RxD3D8AllInOneSetInstanceCallBack(nodeWorldSector, MTextureWorldSectorInstanceCallback);

	return TRUE;
}


void MTexturePipeClose()
{
	/*
	 * Destroy All Pipelines
	 */
	RxPipelineDestroy(MTextureAtomicPipe);
	RxPipelineDestroy(MTextureWorldSectorPipe);
}


RpAtomic *
MTextureAtomicSetPipeline(RpAtomic *atomic)
{
    RpAtomicSetPipeline(atomic, MTextureAtomicPipe);

	return atomic;
}


RpWorldSector *
MTextureWorldSectorSetPipeline(RpWorldSector *worldSector)
{
    RpWorldSectorSetPipeline(worldSector, MTextureWorldSectorPipe);

	return worldSector;
}

void 
RpMTextureSetBlendMode(RwUInt32  nBlendMode)
{
	// do nothing
}


#endif // D3D8