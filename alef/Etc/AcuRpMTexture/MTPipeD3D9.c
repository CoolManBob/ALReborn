#ifndef _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "rwcore.h"
#include "rpworld.h"

#include "mtexture.h"
#include "mtpipe.h"

#include <D3D9.h>
#include <d3dx9.h>
#include <assert.h>

#include "shadow2.h"

//#include "Terrain_linear_Rough.h"
#include "Terrain_linear.h"
#include "Terrain2_linear.h"
#include "Terrain3_linear.h"

#include "Terrain_linear_FadeOut.h"

#define ACURPMTEXTURE	3000000

/* Multi-Texture Pipeline */
static RxPipeline	*MTextureAtomicPipe;
static RxPipeline	*MTextureWorldSectorPipe;

/* Old Pipeline Render Callback */
static RxD3D9AllInOneRenderCallBack DefaultAtomicRenderCallback = NULL;
static RxD3D9AllInOneRenderCallBack DefaultWorldSectorRenderCallback = NULL;

//@{ Jaewon 20050512
// Terrain gloss map support
static RwBool _PixelShaderSupport = FALSE;
static RwBool _GlossMapEnabled = FALSE;
static RwBool _TSSArgTempSupport = FALSE;
//@} Jaewon

static RwInt32	MaxTexturePerPass	= 0;
static RwInt32	MaxTextureStage		= 0;

static void*	ShadowVertexShader	= NULL				;

static void*	TerrainVertexShader	= NULL				;
static void*	TerrainVertexShader2 = NULL				;
static void*	TerrainVertexShader3 = NULL				;
static void*	TerrainVertexShaderFO = NULL			;
static void*	TerrainVertexShader_Rough	= NULL		;

//@{ Jaewon 20050513
// Terrain gloss map support
static void* TerrainGlossMapVertexShader0 = NULL;
static void* TerrainGlossMapVertexShader1 = NULL;
static void* TerrainGlossMapVertexShader2 = NULL;
static void* TerrainGlossMapVertexShader3 = NULL;
//@} Jaewon
static RwBool	VertexShaderEnable	= FALSE				;

// 마고자 (2004-05-17 오전 11:14:47) : 디폴트를 모듈레이트 투로 설정.
static RwUInt32	__sTextureBlendMode	= D3DTOP_MODULATE4X	;
static RwBool	__bUseLinearFog		= FALSE				;

RwCamera*	RPMTEX_WorldCamera		;
RwMatrix*	RPMTEX_invShadowMatrix	;

RwTexture*	RPMTEX_ShadowTexture	;
RwUInt32	RPMTEX_FadeDist			;

typedef struct RenderData
{
	RxD3D9ResEntryHeader	*resEntryHeader;
	RxD3D9InstanceData		*instancedData;
	RwUInt32				flags;
	RwUInt32				sectordist;
	RpAtomic*				atomic;
	struct RenderData*		next;
}RenderData;

static void*	MemoryPool = NULL;
static RwInt32	MemoryOffset = 0;

#define LOCAL_BUFFER_COUNT 7
static RenderData*		lBuffer[LOCAL_BUFFER_COUNT];

//@{ Jaewon 20050513
extern RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable);
extern RwBool _rwD3D9TextureHasAlpha(const RwTexture *texture);
extern RwBool _rwD3D9RenderStateIsVertexAlphaEnable(void);

#ifndef ID3DXBuffer_GetBufferPointer
#define ID3DXBuffer_GetBufferPointer(_buffer) (_buffer)->lpVtbl->GetBufferPointer(_buffer)
#else
#pragma message("ID3DXBuffer_GetBufferPointer is already defined.")
#endif

#ifndef ID3DXBuffer_GetBufferSize
#define ID3DXBuffer_GetBufferSize(_buffer) (_buffer)->lpVtbl->GetBufferSize(_buffer)
#else
#pragma message("ID3DXBuffer_GetBufferSize is already defined.")
#endif

#ifndef ID3DXBuffer_Release
#define ID3DXBuffer_Release(_buffer) (_buffer)->lpVtbl->Release(_buffer)
#else
#pragma message("ID3DXBuffer_Release is already defined.")
#endif
//@} Jaewon

/****************************************************************************
 Global variables
 */
//@{ Jaewon 20050512
// Major refactoring & Terrain gloss map support
// AB_C notation
// A : Number of texture layers
// B : Number of passes
// C : Pass index
//--- Without gloss maps
void textureOpSetup11(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
void * perInstanceTextureSetup11(RxD3D9InstanceData *pInstanceData)
{
	//. 2006. 1. 17. Nonstopdj
	//. assert와 pInstanceData->material->texture->refCount로 일단 crash를 막음.
	//. 근본적인 해결책을 꼭 찾자~ -_-
	assert( pInstanceData->material->texture->refCount > 0 ); 

	if( pInstanceData->material->texture->refCount > 0)
		RwD3D9SetTexture(pInstanceData->material->texture, 0);

	return TerrainVertexShader;
}

void * perInstanceTextureSetup11_Rough(RxD3D9InstanceData *pInstanceData)
{
	//. 2006. 1. 17. Nonstopdj
	//. assert와 pInstanceData->material->texture->refCount로 일단 crash를 막음.
	//. 근본적인 해결책을 꼭 찾자~ -_-
	assert( pInstanceData->material->texture->refCount > 0 ); 

	if( pInstanceData->material->texture->refCount > 0)
		RwD3D9SetTexture(pInstanceData->material->texture, 0);

	return TerrainVertexShader_Rough;
}

void textureOpSetup31(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
	RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE	);

	RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAARG1	,	D3DTA_DIFFUSE		);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
void * perInstanceTextureSetup31(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pInstanceData->material->texture	,		0);
	RwD3D9SetTexture( pData->texture[0]					,		1);
	RwD3D9SetTexture( pData->texture[1]					,		2);

	return TerrainVertexShader;
}
void textureOpSetup32_0(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
void * perInstanceTextureSetup32_0(RxD3D9InstanceData *pInstanceData)
{
	RwD3D9SetTexture(pInstanceData->material->texture,		0);

	return TerrainVertexShader;
}
void textureOpSetup32_1(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_CURRENT		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	1);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	2);
	}

	RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
	RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
}
void * perInstanceTextureSetup32_1(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pData->texture[0]					,		0);
	RwD3D9SetTexture( pData->texture[1]					,		1);

	return TerrainVertexShader;
}
void textureOpSetup51(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
	RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

	RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 4, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
	RwD3D9SetTextureStageState( 4, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 4, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 4, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

	RwD3D9SetTextureStageState( 5, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 5, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 5, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 5, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 5, D3DTSS_ALPHAARG1	,	D3DTA_DIFFUSE		);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
void * perInstanceTextureSetup51(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pInstanceData->material->texture	,		0);
	RwD3D9SetTexture( pData->texture[0]					,		1);
	RwD3D9SetTexture( pData->texture[1]					,		2);
	RwD3D9SetTexture( pData->texture[2]					,		3);
	RwD3D9SetTexture( pData->texture[3]					,		4);
	RwD3D9SetTexture( NULL								,		5);

	return TerrainVertexShader;
}
void textureOpSetup52_0(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
	RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

	RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAARG1	,	D3DTA_DIFFUSE		);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
void * perInstanceTextureSetup52_0(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pInstanceData->material->texture	,		0);
	RwD3D9SetTexture( pData->texture[0]					,		1);
	RwD3D9SetTexture( pData->texture[1]					,		2);
	RwD3D9SetTexture( NULL								,		3);

	return TerrainVertexShader;

}
void textureOpSetup52_1(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_CURRENT		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	3);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	4);
	}

	RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
	RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
}
void * perInstanceTextureSetup52_1(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pData->texture[2]					,		0);
	RwD3D9SetTexture( pData->texture[3]					,		1);
	RwD3D9SetTexture( NULL								,		2);

	return TerrainVertexShader3;
}
void textureOpSetup53_0(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
void * perInstanceTextureSetup53_0(RxD3D9InstanceData *pInstanceData)
{
	RwD3D9SetTexture(pInstanceData->material->texture,		0);

	return TerrainVertexShader;
}
void textureOpSetup53_1(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_CURRENT		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	1);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	2);
	}

	RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
	RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
}
void * perInstanceTextureSetup53_1(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pData->texture[0]					,		0);
	RwD3D9SetTexture( pData->texture[1]					,		1);

	return TerrainVertexShader2;
}
void textureOpSetup53_2(RwBool useShader)
{
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_CURRENT		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	3);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	4);
	}

	RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
	RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
}
void * perInstanceTextureSetup53_2(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pData->texture[2]					,		0);
	RwD3D9SetTexture( pData->texture[3]					,		1);

	return TerrainVertexShader3;
}
//--- With gloss maps
void glossMapTextureOpSetup11(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);
}
void * glossMapPerInstanceTextureSetup11(RxD3D9InstanceData *pInstanceData)
{
	RwD3D9SetTexture(pInstanceData->material->texture, 0);

	return TerrainVertexShader;
}
void glossMapTextureOpSetup32_0(RwBool useShader)
{
	glossMapTextureOpSetup11(useShader);
}
void * glossMapPerInstanceTextureSetup32_0(RxD3D9InstanceData *pInstanceData)
{
	return glossMapPerInstanceTextureSetup11(pInstanceData);
}
void glossMapTextureOpSetup32_1(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	2);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	1);
	}

	RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
	RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
}
void * glossMapPerInstanceTextureSetup32_1(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pData->texture[1]					,		0);
	RwD3D9SetTexture( pData->texture[0]					,		1);

	return TerrainGlossMapVertexShader0;
}
void glossMapTextureOpSetup31(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(1, D3DTSS_RESULTARG,	D3DTA_TEMP			);

	RwD3D9SetTextureStageState(2, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(2, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(2, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(3, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(3, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(3, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(3, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(3, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(4, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA	);
	RwD3D9SetTextureStageState(4, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(4, D3DTSS_COLORARG2,	D3DTA_TEMP		);
	RwD3D9SetTextureStageState(4, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(4, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
		RwD3D9SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,	2);
		RwD3D9SetTextureStageState(3, D3DTSS_TEXCOORDINDEX,	1);
	}
}
void * glossMapPerInstanceTextureSetup31(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture(pInstanceData->material->texture,		0);
	RwD3D9SetTexture( pData->texture[1]				,		2);
	RwD3D9SetTexture( pData->texture[0]				,		3);

	return TerrainGlossMapVertexShader1;
}
void glossMapTextureOpSetup52_0(RwBool useShader)
{
	glossMapTextureOpSetup31(useShader);
}
void * glossMapPerInstanceTextureSetup52_0(RxD3D9InstanceData *pInstanceData)
{
	return glossMapPerInstanceTextureSetup31(pInstanceData);
}
void glossMapTextureOpSetup52_1(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	4);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	3);
	}

	RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
	RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
}
void * glossMapPerInstanceTextureSetup52_1(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture( pData->texture[3]					,		0);
	RwD3D9SetTexture( pData->texture[2]					,		1);

	return TerrainGlossMapVertexShader2;
}
void glossMapTextureOpSetup53_0(RwBool useShader)
{
	glossMapTextureOpSetup11(useShader);
}
void * glossMapPerInstanceTextureSetup53_0(RxD3D9InstanceData *pInstanceData)
{
	return glossMapPerInstanceTextureSetup11(pInstanceData);
}
void glossMapTextureOpSetup53_1(RwBool useShader)
{
	glossMapTextureOpSetup32_1(useShader);
}
void * glossMapPerInstanceTextureSetup53_1(RxD3D9InstanceData *pInstanceData)
{
	return glossMapPerInstanceTextureSetup32_1(pInstanceData);
}
void glossMapTextureOpSetup53_2(RwBool useShader)
{
	glossMapTextureOpSetup52_1(useShader);
}
void * glossMapPerInstanceTextureSetup53_2(RxD3D9InstanceData *pInstanceData)
{
	return glossMapPerInstanceTextureSetup52_1(pInstanceData);
}
void glossMapTextureOpSetup51(RwBool useShader)
{
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(1, D3DTSS_RESULTARG,	D3DTA_TEMP			);

	RwD3D9SetTextureStageState(2, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(2, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(2, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(3, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(3, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(3, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(3, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(3, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(4, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA	);
	RwD3D9SetTextureStageState(4, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(4, D3DTSS_COLORARG2,	D3DTA_TEMP		);
	RwD3D9SetTextureStageState(4, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(4, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(4, D3DTSS_RESULTARG,	D3DTA_TEMP			);

	RwD3D9SetTextureStageState(5, D3DTSS_COLOROP,   __sTextureBlendMode	);
	RwD3D9SetTextureStageState(5, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState(5, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
	RwD3D9SetTextureStageState(5, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(5, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(6, D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR	);
	RwD3D9SetTextureStageState(6, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(6, D3DTSS_COLORARG2,	D3DTA_SPECULAR		);
	RwD3D9SetTextureStageState(6, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(6, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE		);

	RwD3D9SetTextureStageState(7, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA	);
	RwD3D9SetTextureStageState(7, D3DTSS_COLORARG1,	D3DTA_CURRENT		);
	RwD3D9SetTextureStageState(7, D3DTSS_COLORARG2,	D3DTA_TEMP		);
	RwD3D9SetTextureStageState(7, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState(7, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE		);

	if(!useShader)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
		RwD3D9SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,	2);
		RwD3D9SetTextureStageState(3, D3DTSS_TEXCOORDINDEX,	1);
		RwD3D9SetTextureStageState(5, D3DTSS_TEXCOORDINDEX,	4);
		RwD3D9SetTextureStageState(6, D3DTSS_TEXCOORDINDEX,	3);
	}
}
void * glossMapPerInstanceTextureSetup51(RxD3D9InstanceData *pInstanceData)
{
	MTextureMaterialData *pData = MTEXTUREMATERIAL(pInstanceData->material);
	RwD3D9SetTexture(pInstanceData->material->texture,		0);
	RwD3D9SetTexture( pData->texture[1]				,		2);
	RwD3D9SetTexture( pData->texture[0]				,		3);
	RwD3D9SetTexture( pData->texture[3]				,		5);
	RwD3D9SetTexture( pData->texture[2]				,		6);

	return TerrainGlossMapVertexShader3;
}

typedef void (*TextureOpSetupCallBack)(RwBool);
typedef void * (*PerInstanceTextureSetupCallBack)(RxD3D9InstanceData*);
struct RpMTextureRenderCallBacks
{
	TextureOpSetupCallBack textureOpSetupCallBack_[3];
	PerInstanceTextureSetupCallBack perInstanceTextureSetupCallBack_[3];
};
static struct RpMTextureRenderCallBacks _RpMTextureRenderCallBacks[6];
static struct RpMTextureRenderCallBacks _RpMTextureGlossMapRenderCallBacks[6];
//@} Jaewon

/****************************************************************************
 local defines
 */

#define RENDERFLAGS_HAS_TEXTURE  1
#define RENDERFLAGS_HAS_TFACTOR  2

/****************************************************************************
 local types
 */

/****************************************************************************
 local variables
 */
#define NUMPRIMTYPES    7
static const D3DPRIMITIVETYPE _RwD3D9PrimConvTable[NUMPRIMTYPES] =
{
    (D3DPRIMITIVETYPE)0,                      /* rwPRIMTYPENAPRIMTYPE */
    D3DPT_LINELIST,         /* rwPRIMTYPELINELIST */
    D3DPT_LINESTRIP,        /* rwPRIMTYPEPOLYLINE */
    D3DPT_TRIANGLELIST,     /* rwPRIMTYPETRILIST */
    D3DPT_TRIANGLESTRIP,    /* rwPRIMTYPETRISTRIP */
    D3DPT_TRIANGLEFAN,      /* rwPRIMTYPETRIFAN */
    D3DPT_POINTLIST
};

void	SetRenderState(RxD3D9InstanceData*	instancedData,RwBool lighting,RwBool* vertexAlphaBlend,RwUInt32 flags)
{
	const RpMaterial    *material;
	const RwRGBA        *matcolor;
	
	material = instancedData->material;
	matcolor = &(material->color);

	if ((0xFF != matcolor->alpha) ||
		instancedData->vertexAlpha)
	{
		if ((*vertexAlphaBlend) == FALSE)
		{
			(*vertexAlphaBlend) = TRUE;

			_rwD3D9RenderStateVertexAlphaEnable(TRUE);
		}
	}
	else
	{
		if ((*vertexAlphaBlend) != FALSE)
		{
			(*vertexAlphaBlend) = FALSE;

			_rwD3D9RenderStateVertexAlphaEnable(FALSE);
		}
	}

	if (lighting)
	{
		RwD3D9SetSurfaceProperties(&(material->surfaceProps),
								   matcolor,
								   flags);
	}
	else
	{
		if (flags & rxGEOMETRY_MODULATE)
		{
			RwUInt32			tFactor =
			((((RwUInt32)matcolor->alpha)<<24)|(((RwUInt32)matcolor->red)<<16)|(((RwUInt32)matcolor->green)<<8)|((RwUInt32)matcolor->blue));

			RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, tFactor);
		}
	}
}

static PVOID getShader(const char *pSrc)
{
	PVOID ret = NULL;
	HRESULT hresult;
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsgs = NULL;

	// assemble the source.
	hresult = D3DXAssembleShader(pSrc, strlen(pSrc), 
								NULL, NULL, 0, 
								&pShader, &pErrorMsgs);

	// create the shader.
	if(SUCCEEDED(hresult))
	{
		RwBool hr;
		const RwUInt32 *function = ID3DXBuffer_GetBufferPointer(pShader);

		hr = RwD3D9CreateVertexShader(function, &ret);
		
		if(!hr)
			ret = NULL;
	}
	else
	{
		const char* pLog = (const char*)ID3DXBuffer_GetBufferPointer(pErrorMsgs);
		OutputDebugString(pLog);
	}
	
	if(pErrorMsgs)
	{
        ID3DXBuffer_Release(pErrorMsgs);
	}
	if(pShader)
	{
        ID3DXBuffer_Release(pShader);
	}

	return ret;
}

static void __destroyShader(  PVOID * pShader )
{
	RwD3D9DeleteVertexShader( pShader );
}

/*
 *****************************************************************************
 */

void
MTextureRenderCallBack(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	RxD3D9ResEntryHeader	*resEntryHeader;
	RxD3D9InstanceData		*instancedData;
	RwInt32					numMeshes;
	RwInt16					index;
	MTextureMaterialData	*pData;
		
	// shadow2 data 준비/////////////////////////////////////////////////
	RwMatrix				texMat;
	D3DMATRIX				TextureTransformMatrix;
	
	RpAtomic*		atomic = (RpAtomic*)object;
	RwFrame *		pstFrame = RpAtomicGetFrame(atomic);
	
	RwInt32			mode = atomic->stRenderInfo.shaderUseType;
	RwInt32			sectordist = atomic->stRenderInfo.beforeLODLevel;

	////////////////////////////////////////////////////////////////////
	resEntryHeader = (RxD3D9ResEntryHeader *)(repEntry + 1);			// Get header

	instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);			//Get the instanced data
	numMeshes = resEntryHeader->numMeshes;								//Get the number of meshes

	if(mode == 0)
	{
		// 일단 같은 렌더 스테이트를 가지는 것끼리 모으자^^
		RenderData*		nw_data;
	
		RwBool bRoughMap = FALSE;

		// 마고자 (2005-10-04 오후 5:31:26) : 
		// 좀 안좋지만 일단 하드코딩.
		//ACUOBJECT_TYPE_SECTOR_ROUGHMAP		=	0x0020	,	// 러프맵의 경우..
		if( atomic->stType && atomic->stType->eType & 0x0020 )
		{
			bRoughMap = TRUE;
		}
					
		while (numMeshes--)
		{
			pData = MTEXTUREMATERIAL(instancedData->material);

			nw_data = ( RenderData * ) ( ( char * ) MemoryPool + MemoryOffset );

			#ifdef _DEBUG			
			assert( MemoryOffset + 24 < ACURPMTEXTURE && "메모리 풀 초과" ); 
			#endif

			if( MemoryOffset + 24 >= ACURPMTEXTURE )
				break;
				// 메모리 풀 초과 상태라서 더이상 그리지 않음

			MemoryOffset += 24;

			nw_data->instancedData	= instancedData	;
			nw_data->resEntryHeader	= resEntryHeader;
			nw_data->flags			= flags			;
			nw_data->atomic			= atomic		;
			nw_data->sectordist		= sectordist	;
	 
			if( bRoughMap ) 
			{
				nw_data->next = lBuffer[6];
				lBuffer[6] = nw_data;
			}
			else
			switch( pData->textureNumber )
			{
			default:
			case 0:
				{
					nw_data->next = lBuffer[0];
					lBuffer[0] = nw_data;
				}
				break;
			case 2:
				{
					//@{ Jaewon 20050513
					if(_GlossMapEnabled && _TSSArgTempSupport && 6 <= MaxTexturePerPass)
					{
						nw_data->next = lBuffer[2];
						lBuffer[2] = nw_data;
					}
					else
					//@} Jaewon
					if( 4 <= MaxTexturePerPass )
					{
						nw_data->next = lBuffer[1];
						lBuffer[1] = nw_data;
					}
					else if( 2 <= MaxTexturePerPass )
					{
						nw_data->next = lBuffer[2];
						lBuffer[2] = nw_data;
					}
					else
					{
						nw_data->next = lBuffer[0];
						lBuffer[0] = nw_data;
					}
				}
				break;
			case 4:
				{
					//@{ Jaewon 20050513
					if(_GlossMapEnabled && _TSSArgTempSupport && 8 <= MaxTexturePerPass)
					{
						nw_data->next = lBuffer[5];
						lBuffer[5] = nw_data;
					}
					else if(_GlossMapEnabled && !_TSSArgTempSupport && 6 <= MaxTexturePerPass)
					{
						nw_data->next = lBuffer[4];
						lBuffer[4] = nw_data;
					}
					else
					//@} Jaewon
					if( 6 <= MaxTexturePerPass )
					{
						nw_data->next = lBuffer[3];
						lBuffer[3] = nw_data;
					}
					else if ( 4 <= MaxTexturePerPass )
					{
						nw_data->next = lBuffer[4];
						lBuffer[4] = nw_data;
					}
					else if ( 2 <= MaxTexturePerPass )
					{
						nw_data->next = lBuffer[5];
						lBuffer[5] = nw_data;
					}
					else
					{
						nw_data->next = lBuffer[0];
						lBuffer[0] = nw_data;
					}
				}
				break;
			}
			++instancedData;
		}

	}
	else if(mode == 1)
	{
		RwD3D9SetPixelShader(NULL);											// Set the Default Pixel shader
		_rwD3D9EnableClippingIfNeeded(object, type);						// Set clipping
		
		if (resEntryHeader->indexBuffer != NULL)							// Data shared between meshes
		{
			RwD3D9SetIndices(resEntryHeader->indexBuffer);
		}

		_rwD3D9SetStreams(resEntryHeader->vertexStream,resEntryHeader->useOffsets);		//Set the stream sources
		RwD3D9SetVertexDeclaration(resEntryHeader->vertexDeclaration);		// Vertex Declaration
	
		while (numMeshes--)
		{
			if(VertexShaderEnable && RPMTEX_WorldCamera && RPMTEX_ShadowTexture)
			{
				RwMatrixCopy(&texMat, RPMTEX_invShadowMatrix);

				TextureTransformMatrix.m[0][0] = texMat.right.x;
				TextureTransformMatrix.m[0][1] = texMat.up.x;
				TextureTransformMatrix.m[0][2] = texMat.at.x;
				TextureTransformMatrix.m[0][3] = texMat.pos.x;

				TextureTransformMatrix.m[1][0] = texMat.right.y;
				TextureTransformMatrix.m[1][1] = texMat.up.y;
				TextureTransformMatrix.m[1][2] = texMat.at.y;
				TextureTransformMatrix.m[1][3] = texMat.pos.y;

				TextureTransformMatrix.m[2][0] = texMat.right.z;
				TextureTransformMatrix.m[2][1] = texMat.up.z;
				TextureTransformMatrix.m[2][2] = texMat.at.z;
				TextureTransformMatrix.m[2][3] = texMat.pos.z;

				TextureTransformMatrix.m[3][0] = 0.0f;
				TextureTransformMatrix.m[3][1] = 0.0f;
				TextureTransformMatrix.m[3][2] = 0.0f;
				TextureTransformMatrix.m[3][3] = 1.0f;

				RwD3D9SetVertexShaderConstant(12,(void *)&TextureTransformMatrix,2);

				// shadow2 render
				RwD3D9SetVertexShader(ShadowVertexShader);

				index = 0;
				RwD3D9SetTexture(RPMTEX_ShadowTexture, index);
			
				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDZERO);
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDINVSRCCOLOR);

				//@{ Jaewon 20040720
				RwD3D9SetTextureStageState(index, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
				RwD3D9SetTextureStageState(index, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
				RwD3D9SetTextureStageState(index, D3DTSS_COLOROP,   D3DTOP_MODULATE);//D3DTOP_SELECTARG1);
				//@} Jaewon

				RwD3D9SetTextureStageState(index, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
				
				RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *) TRUE);

				RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)FALSE );
  				
				if (resEntryHeader->indexBuffer != NULL)
				{
					RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
											   instancedData->baseIndex,
											   0, instancedData->numVertices,
											   instancedData->startIndex, instancedData->numPrimitives);
				}
				else
				{
					RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
										instancedData->baseIndex,
										instancedData->numPrimitives);
				}
				RwD3D9SetTexture(NULL, index);
				RwD3D9SetTextureStageState(index, D3DTSS_COLOROP, D3DTOP_DISABLE);
				RwD3D9SetTextureStageState(index, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDSRCALPHA);
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDINVSRCALPHA);
				RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)TRUE );
			}
			/* Move onto the next instancedData */
			++instancedData;
		}
	}

	instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);
	RwD3D9SetVertexShader(instancedData->vertexShader);
	
	return;
	//RWRETURNVOID();
}

static void
D3D9DefaultRenderBlack(RxD3D9ResEntryHeader *resEntryHeader,
                       RwBool hasTextureCoordinates)
{
    RxD3D9InstanceData *instancedData;
    RwUInt32    ditherEnable;
    RwUInt32    shadeMode;
    RwBool      useAlphaTexture;
    RwInt32     numMeshes;

    /* Save some renderstates */
    RwD3D9GetRenderState(D3DRS_DITHERENABLE, &ditherEnable);
    RwD3D9GetRenderState(D3DRS_SHADEMODE, &shadeMode);

    /* No dither, No Gouraud, No vertex alpha */
    _rwD3D9RenderStateVertexAlphaEnable(FALSE);
    RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xff000000);
    RwD3D9SetRenderState(D3DRS_DITHERENABLE, FALSE);
    RwD3D9SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

    /* Always black color (only alpha channel may change) */
    RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
    RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
    RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    RwD3D9SetTexture(NULL, 0);

    /* force refresh */
    useAlphaTexture = FALSE;

    /* Get the instanced data */
    instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);

    /* Get the number of meshes */
    numMeshes = resEntryHeader->numMeshes;
    while (numMeshes--)
    {
        /* this function check internally for alpha channel */
        if (hasTextureCoordinates &&
            instancedData->material->texture &&
            _rwD3D9TextureHasAlpha(instancedData->material->texture))
        {
            RwD3D9SetTexture(instancedData->material->texture, 0);

            if (useAlphaTexture == FALSE)
            {
                useAlphaTexture = TRUE;

                /* We only use the alpha channel from the texture */
                RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
            }
        }
        else
        {
            if (useAlphaTexture)
            {
                useAlphaTexture = FALSE;

                RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);

                RwD3D9SetTexture(NULL, 0);
            }
        }

        /*
         * Vertex shader
         */
        RwD3D9SetVertexShader(instancedData->vertexShader);

        /*
         * Render
         */
        if (resEntryHeader->indexBuffer != NULL)
        {
            RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
                                       instancedData->baseIndex,
                                       0, instancedData->numVertices,
                                       instancedData->startIndex, instancedData->numPrimitives);
        }
        else
        {
            RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
                                instancedData->baseIndex,
                                instancedData->numPrimitives);
        }

        /* Move onto the next instancedData */
        instancedData++;
    }

    /* Restore some renderstates */
    RwD3D9SetRenderState(D3DRS_DITHERENABLE, ditherEnable);
    RwD3D9SetRenderState(D3DRS_SHADEMODE, shadeMode);
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
			instanceNode = RxNodeDefinitionGetD3D9AtomicAllInOne();

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
			instanceNode = RxNodeDefinitionGetD3D9WorldSectorAllInOne();

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

//@{ Jaewon 20050512
// Use the 'getShader' function & vertex shader fragments.
#include "VertexShaderFragments.h"
void	
RpMTextureCreateShaders()
{
	char buf[4096];
	unsigned int length;

	if(!VertexShaderEnable ) return;

	strcpy(buf, _VSHeaderFrag);
	strcat(buf, _VSPosTransformFrag);
	strcat(buf, _VSFogFrag);
	strcat(buf, _VSDiffuseFrag);
	strcat(buf, _VSSpecularFrag);
	length = strlen(buf);

	// TerrainVertexShader
	strcat(buf, _VSTexCoordFrag0);
	TerrainVertexShader = getShader(buf);
	assert(TerrainVertexShader);

	// TerrainVertexShader2
	buf[length] = '\0';
	strcat(buf, _VSTexCoordFrag1);
	TerrainVertexShader2 = getShader(buf);
	assert(TerrainVertexShader2);

	// TerrainVertexShader3
	buf[length] = '\0';
	strcat(buf, _VSTexCoordFrag2);
	TerrainVertexShader3 = getShader(buf);
	assert(TerrainVertexShader3);

	// TerrainVertexShaderFO
	// Not used currently
	//TerrainVertexShaderFO = TerrainVertexShader;

	// TerrainGlossMapVertexShader0
	buf[length] = '\0';
	strcat(buf, _VSGlossMapTexCoordFrag0);
	TerrainGlossMapVertexShader0 = getShader(buf);
	assert(TerrainGlossMapVertexShader0);

	// TerrainGlossMapVertexShader1
	buf[length] = '\0';
	strcat(buf, _VSGlossMapTexCoordFrag1);
	TerrainGlossMapVertexShader1 = getShader(buf);
	assert(TerrainGlossMapVertexShader1);

	// TerrainGlossMapVertexShader2
	buf[length] = '\0';
	strcat(buf, _VSGlossMapTexCoordFrag2);
	TerrainGlossMapVertexShader2 = getShader(buf);
	assert(TerrainGlossMapVertexShader2);

	// TerrainGlossMapVertexShader3
	buf[length] = '\0';
	strcat(buf, _VSGlossMapTexCoordFrag3);
	TerrainGlossMapVertexShader3 = getShader(buf);
	assert(TerrainGlossMapVertexShader3);

	// ShadowVertexShader
	strcpy(buf, _VSHeaderFrag);
	strcat(buf, _VSShadow2Frag);
	ShadowVertexShader = getShader(buf);
	assert(ShadowVertexShader);

	// Rough Shader
	strcpy(buf, _VSHeaderFrag_For_Rough);
	strcat(buf, _VSPosTransformFrag);
	strcat(buf, _VSFogFrag);
	strcat(buf, _VSDiffuseFrag);
	strcat(buf, _VSSpecularFrag);
	strcat(buf, _VSTexCoordFrag_For_Rough);
	TerrainVertexShader_Rough = getShader(buf);
	assert(TerrainVertexShader_Rough);

//	HRESULT		hr;
//
//	hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_shadow,
//                                      &ShadowVertexShader);
//
//	if( __bUseLinearFog )
//	{
//		// 마고자 (2004-05-19 오전 5:11:53) : 리니어 포그 사용~.
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain_Linear,
//								  &TerrainVertexShader);
//
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain2_Linear,
//								  &TerrainVertexShader2);
//
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain3_Linear,
//								  &TerrainVertexShader3);
//
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain_LinearFO,
//								  &TerrainVertexShaderFO);
//	}
///*	else
//	{
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain,
//								  &TerrainVertexShader);
//
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain2,
//								  &TerrainVertexShader2);
//
//		hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_terrain3,
//								  &TerrainVertexShader3);
//	}*/
}
//@} Jaewon

RwBool
MTexturePipeOpen()
{
	RxNodeDefinition	*instanceAtomicNode;
	RxNodeDefinition	*instanceWorldSectorNode;
	RxPipelineNode		*nodeAtomic;
	RxPipelineNode		*nodeWorldSector;
	const D3DCAPS9 *caps;

	//@{ Jaewon 20050512
	// Major refactoring & Terrain gloss map support
	// Register rendering callbacks without gloss maps.
	_RpMTextureRenderCallBacks[0].textureOpSetupCallBack_[0] = textureOpSetup11;
	_RpMTextureRenderCallBacks[0].perInstanceTextureSetupCallBack_[0] = perInstanceTextureSetup11;
	_RpMTextureRenderCallBacks[0].textureOpSetupCallBack_[1] = NULL;
	_RpMTextureRenderCallBacks[0].perInstanceTextureSetupCallBack_[1] = NULL;

	_RpMTextureRenderCallBacks[1].textureOpSetupCallBack_[0] = textureOpSetup31;
	_RpMTextureRenderCallBacks[1].perInstanceTextureSetupCallBack_[0] = perInstanceTextureSetup31;
	_RpMTextureRenderCallBacks[1].textureOpSetupCallBack_[1] = NULL;
	_RpMTextureRenderCallBacks[1].perInstanceTextureSetupCallBack_[1] = NULL;

	_RpMTextureRenderCallBacks[2].textureOpSetupCallBack_[0] = textureOpSetup32_0;
	_RpMTextureRenderCallBacks[2].perInstanceTextureSetupCallBack_[0] = perInstanceTextureSetup32_0;
	_RpMTextureRenderCallBacks[2].textureOpSetupCallBack_[1] = textureOpSetup32_1;
	_RpMTextureRenderCallBacks[2].perInstanceTextureSetupCallBack_[1] = perInstanceTextureSetup32_1;
	_RpMTextureRenderCallBacks[2].textureOpSetupCallBack_[2] = NULL;
	_RpMTextureRenderCallBacks[2].perInstanceTextureSetupCallBack_[2] = NULL;

	_RpMTextureRenderCallBacks[3].textureOpSetupCallBack_[0] = textureOpSetup51;
	_RpMTextureRenderCallBacks[3].perInstanceTextureSetupCallBack_[0] = perInstanceTextureSetup51;
	_RpMTextureRenderCallBacks[3].textureOpSetupCallBack_[1] = NULL;
	_RpMTextureRenderCallBacks[3].perInstanceTextureSetupCallBack_[1] = NULL;

	_RpMTextureRenderCallBacks[4].textureOpSetupCallBack_[0] = textureOpSetup52_0;
	_RpMTextureRenderCallBacks[4].perInstanceTextureSetupCallBack_[0] = perInstanceTextureSetup52_0;
	_RpMTextureRenderCallBacks[4].textureOpSetupCallBack_[1] = textureOpSetup52_1;
	_RpMTextureRenderCallBacks[4].perInstanceTextureSetupCallBack_[1] = perInstanceTextureSetup52_1;
	_RpMTextureRenderCallBacks[4].textureOpSetupCallBack_[2] = NULL;
	_RpMTextureRenderCallBacks[4].perInstanceTextureSetupCallBack_[2] = NULL;

	_RpMTextureRenderCallBacks[5].textureOpSetupCallBack_[0] = textureOpSetup53_0;
	_RpMTextureRenderCallBacks[5].perInstanceTextureSetupCallBack_[0] = perInstanceTextureSetup53_0;
	_RpMTextureRenderCallBacks[5].textureOpSetupCallBack_[1] = textureOpSetup53_1;
	_RpMTextureRenderCallBacks[5].perInstanceTextureSetupCallBack_[1] = perInstanceTextureSetup53_1;
	_RpMTextureRenderCallBacks[5].textureOpSetupCallBack_[2] = textureOpSetup53_2;
	_RpMTextureRenderCallBacks[5].perInstanceTextureSetupCallBack_[2] = perInstanceTextureSetup53_2;
	
	// Register rendering callbacks with gloss maps.
	_RpMTextureGlossMapRenderCallBacks[0].textureOpSetupCallBack_[0] = glossMapTextureOpSetup11;
	_RpMTextureGlossMapRenderCallBacks[0].perInstanceTextureSetupCallBack_[0] = glossMapPerInstanceTextureSetup11;
	_RpMTextureGlossMapRenderCallBacks[0].textureOpSetupCallBack_[1] = NULL;
	_RpMTextureGlossMapRenderCallBacks[0].perInstanceTextureSetupCallBack_[1] = NULL;

	_RpMTextureGlossMapRenderCallBacks[1].textureOpSetupCallBack_[0] = glossMapTextureOpSetup32_0;
	_RpMTextureGlossMapRenderCallBacks[1].perInstanceTextureSetupCallBack_[0] = glossMapPerInstanceTextureSetup32_0;
	_RpMTextureGlossMapRenderCallBacks[1].textureOpSetupCallBack_[1] = glossMapTextureOpSetup32_1;
	_RpMTextureGlossMapRenderCallBacks[1].perInstanceTextureSetupCallBack_[1] = glossMapPerInstanceTextureSetup32_1;
	_RpMTextureGlossMapRenderCallBacks[1].textureOpSetupCallBack_[2] = NULL;
	_RpMTextureGlossMapRenderCallBacks[1].perInstanceTextureSetupCallBack_[2] = NULL;

	_RpMTextureGlossMapRenderCallBacks[2].textureOpSetupCallBack_[0] = glossMapTextureOpSetup31;
	_RpMTextureGlossMapRenderCallBacks[2].perInstanceTextureSetupCallBack_[0] = glossMapPerInstanceTextureSetup31;
	_RpMTextureGlossMapRenderCallBacks[2].textureOpSetupCallBack_[1] = NULL;
	_RpMTextureGlossMapRenderCallBacks[2].perInstanceTextureSetupCallBack_[1] = NULL;

	_RpMTextureGlossMapRenderCallBacks[3].textureOpSetupCallBack_[0] = glossMapTextureOpSetup52_0;
	_RpMTextureGlossMapRenderCallBacks[3].perInstanceTextureSetupCallBack_[0] = glossMapPerInstanceTextureSetup52_0;
	_RpMTextureGlossMapRenderCallBacks[3].textureOpSetupCallBack_[1] = glossMapTextureOpSetup52_1;
	_RpMTextureGlossMapRenderCallBacks[3].perInstanceTextureSetupCallBack_[1] = glossMapPerInstanceTextureSetup52_1;
	_RpMTextureGlossMapRenderCallBacks[3].textureOpSetupCallBack_[2] = NULL;
	_RpMTextureGlossMapRenderCallBacks[3].perInstanceTextureSetupCallBack_[2] = NULL;

	_RpMTextureGlossMapRenderCallBacks[4].textureOpSetupCallBack_[0] = glossMapTextureOpSetup53_0;
	_RpMTextureGlossMapRenderCallBacks[4].perInstanceTextureSetupCallBack_[0] = glossMapPerInstanceTextureSetup53_0;
	_RpMTextureGlossMapRenderCallBacks[4].textureOpSetupCallBack_[1] = glossMapTextureOpSetup53_1;
	_RpMTextureGlossMapRenderCallBacks[4].perInstanceTextureSetupCallBack_[1] = glossMapPerInstanceTextureSetup53_1;
	_RpMTextureGlossMapRenderCallBacks[4].textureOpSetupCallBack_[2] = glossMapTextureOpSetup53_2;
	_RpMTextureGlossMapRenderCallBacks[4].perInstanceTextureSetupCallBack_[2] = glossMapPerInstanceTextureSetup53_2;

	_RpMTextureGlossMapRenderCallBacks[5].textureOpSetupCallBack_[0] = glossMapTextureOpSetup51;
	_RpMTextureGlossMapRenderCallBacks[5].perInstanceTextureSetupCallBack_[0] = glossMapPerInstanceTextureSetup51;
	_RpMTextureGlossMapRenderCallBacks[5].textureOpSetupCallBack_[1] = NULL;
	_RpMTextureGlossMapRenderCallBacks[5].perInstanceTextureSetupCallBack_[1] = NULL;
	//@} Jaewon

	caps = RwD3D9GetCaps();
	if ( (caps->VertexShaderVersion & 0xffff) >= 0x0101)
    {
        VertexShaderEnable = TRUE;
    }
	else 
	{
		VertexShaderEnable = FALSE;
	}

	MaxTexturePerPass = caps->MaxSimultaneousTextures;
	MaxTextureStage = caps->MaxTextureBlendStages;

	//@{ Jaewon 20050512
	// Terrain gloss map support
	if((caps->PixelShaderVersion & 0xffff) >= 0x0101)
		_PixelShaderSupport = TRUE;
	else
		_PixelShaderSupport = FALSE;
	if(caps->PrimitiveMiscCaps & D3DPMISCCAPS_TSSARGTEMP)
		_TSSArgTempSupport = TRUE;
	else
		_TSSArgTempSupport = FALSE;
	//@} Jaweon

	/*
	 * Create a new atomic pipeline
	 */
	MTextureAtomicPipe		= MTextureCreateAtomicPipeline		();
	MTextureWorldSectorPipe = MTextureCreateWorldSectorPipeline	();

	/*
	 * Get the instance node definition
	 */
	instanceAtomicNode		= RxNodeDefinitionGetD3D9AtomicAllInOne		();
	instanceWorldSectorNode = RxNodeDefinitionGetD3D9WorldSectorAllInOne();

	/*
	 * Set the pipeline specific data
	 */
	nodeAtomic		= RxPipelineFindNodeByName(MTextureAtomicPipe, instanceAtomicNode->name, NULL, NULL);
	nodeWorldSector	= RxPipelineFindNodeByName(MTextureWorldSectorPipe, instanceWorldSectorNode->name, NULL, NULL);

	/*
	 * Cache the default render callback
	 */
	DefaultAtomicRenderCallback			= RxD3D9AllInOneGetRenderCallBack( nodeAtomic		);
	DefaultWorldSectorRenderCallback	= RxD3D9AllInOneGetRenderCallBack( nodeWorldSector	);

	/*
	 * Set the VShader render callback
	 */
	RxD3D9AllInOneSetRenderCallBack( nodeAtomic			, MTextureRenderCallBack		);
	RxD3D9AllInOneSetRenderCallBack( nodeWorldSector	, MTextureRenderCallBack		);
	
	MemoryPool = malloc( ACURPMTEXTURE );

	return TRUE;
}


void MTexturePipeClose()
{
	/*
	 * Destroy All Pipelines
	 */
	RxPipelineDestroy(MTextureAtomicPipe);
	RxPipelineDestroy(MTextureWorldSectorPipe);

	if (VertexShaderEnable)
	{
		if (ShadowVertexShader)
			RwD3D9DeleteVertexShader (ShadowVertexShader);

		if (TerrainVertexShader)
			RwD3D9DeleteVertexShader (TerrainVertexShader);

		if (TerrainVertexShader2)
			RwD3D9DeleteVertexShader (TerrainVertexShader2);

		if (TerrainVertexShader3)
			RwD3D9DeleteVertexShader (TerrainVertexShader3);

		if (TerrainVertexShaderFO)
			RwD3D9DeleteVertexShader (TerrainVertexShaderFO);
	}

	free(MemoryPool);
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
	__sTextureBlendMode = nBlendMode;
}

RwInt32
RpMTextureSetMaxTexturePerPass( RwInt32 nPass )
{
	RwInt32	backup = MaxTexturePerPass;

	MaxTexturePerPass = nPass;

	if( 8 < nPass )
	{
		MaxTexturePerPass = 8;
	}

	if( nPass <= 0 )
	{
		MaxTexturePerPass = 1;
	}

	return backup;
}

RwBool
RpMTextureEnableVertexShader( RwBool bEnable )
{
	RwBool	bPrev = VertexShaderEnable;
	VertexShaderEnable	= bEnable;
	return bPrev;
}

extern	RwBool
RpMTextureIsEnableVertexShader()
{
	// 마고자 (2004-07-19 오후 5:31:53) : 없는거 같아서
	return VertexShaderEnable;
}


RwBool
RpMTextureEnableLinearFog( RwBool bEnable )
{
	// 마고자 (2004-05-19 오전 5:09:52) : 
	// 툴에서만이라도 리니어 포그 가능하게.
	RwBool backup = __bUseLinearFog;
	__bUseLinearFog = bEnable;
	return backup;
}

extern	RwBool
RpMTextureIsEnableLinearFog()
{
	return __bUseLinearFog;
}

void	SetResEntryHeaderInfo(RxD3D9ResEntryHeader*		resEntryHeader,RpAtomic*	atomic)
{
	if (resEntryHeader->indexBuffer != NULL)							// Data shared between meshes
	{
		RwD3D9SetIndices(resEntryHeader->indexBuffer);
	}

	_rwD3D9SetStreams(resEntryHeader->vertexStream,resEntryHeader->useOffsets);		//Set the stream sources
	RwD3D9SetVertexDeclaration(resEntryHeader->vertexDeclaration);		// Vertex Declaration
	
	if(atomic->stType->frustumTestResult == rwSPHEREINSIDE)
	{
		RwD3D9SetRenderState(D3DRS_CLIPPING, FALSE);
	}
	else
	{
		RwD3D9SetRenderState(D3DRS_CLIPPING, TRUE);
	}
}

extern void 
RpMTextureDrawStart()
{
	int i;
	MemoryOffset = 0;
		
	for( i = 0 ; i < LOCAL_BUFFER_COUNT ; i++ )
	{
		lBuffer[i] = NULL;
	}
}

//@{ Jaewon 20050512
// Major refactoring & Terrain gloss map support
void RpMTextureDrawEnd(RwBool useShader)
{
	RxD3D9ResEntryHeader	*resEntryHeader;
	RxD3D9ResEntryHeader	*before_resEntryHeader = NULL;
	RxD3D9InstanceData		*instancedData;
	RenderData				*cur_data;
	RwUInt32				i, j, k;
	TextureOpSetupCallBack textureOpSetupCallBack;
	void					*vshader;

	RwBool					MyVertexShaderEnable;
	RwBool					lighting;
	RwBool					vertexAlphaBlend;
	RwBool					bUseAlpha = FALSE;
	struct RpMTextureRenderCallBacks (*renderCBs)[6];

	if(_GlossMapEnabled)
		renderCBs = &_RpMTextureGlossMapRenderCallBacks;
	else
		renderCBs = &_RpMTextureRenderCallBacks;

	if(useShader)
		MyVertexShaderEnable = VertexShaderEnable;
	else
		MyVertexShaderEnable = FALSE;

	RwD3D9SetPixelShader(NULL);											// Set the Default Pixel shader

	RwD3D9GetRenderState(D3DRS_LIGHTING, &lighting);					// check lighting

	vertexAlphaBlend = _rwD3D9RenderStateIsVertexAlphaEnable();			//Get vertex alpha Blend state

#define	RENDER_PRIMITIVE()																		\
	if (resEntryHeader->indexBuffer != NULL)														\
	{																								\
	RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType				,		\
	instancedData->baseIndex									,		\
	0, instancedData->numVertices							,		\
	instancedData->startIndex, instancedData->numPrimitives	);		\
	}																								\
	else																							\
	{																								\
	RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType	,							\
	instancedData->baseIndex					,							\
	instancedData->numPrimitives				);							\
	}

	for(i=0; i<6; ++i)
	// For each 'lBuffer'
	{
		// i
		// 0 : 한장 - 1페스 - 텍스쳐 1장
		// 1 : 두장 - 1페스 - 텍스쳐 3장
		// 2 : 두장 - 2패스
		// 3 : 세장 - 1페스 - 텍스쳐 5장
		// 4 : 세장 - 2페스
		// 5 : 세장 - 3페스

		for(j=0; j<3; ++j)
		// For each pass
		{
			cur_data = lBuffer[i];
			if(!cur_data)
				continue;

			textureOpSetupCallBack = (*renderCBs)[i].textureOpSetupCallBack_[j];
			if(textureOpSetupCallBack == NULL)
				break;

			// 텍스쳐 스테이지 설정.
			textureOpSetupCallBack(MyVertexShaderEnable);

			while(cur_data)
			{
				resEntryHeader = cur_data->resEntryHeader;

				//>@ 2005.3.31 gemani
				CS_RESENTRYHEADER_LOCK(resEntryHeader);

				if(!resEntryHeader->isLive)
				{
					CS_RESENTRYHEADER_UNLOCK(resEntryHeader);

					cur_data = cur_data->next;
					continue;
				}
				//<@

				if(resEntryHeader != before_resEntryHeader)
				{
					before_resEntryHeader = resEntryHeader;
					SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
				}

				instancedData = cur_data->instancedData;

				if(instancedData->material && instancedData->material->texture)
				{
					//if(i==0)
					//	instancedData->vertexAlpha = TRUE;

					SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);

					// 텍스쳐 설정.
					vshader = (*renderCBs)[i].perInstanceTextureSetupCallBack_[j](instancedData);

					if(MyVertexShaderEnable)
					{
						//if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
						//{
						//	RwD3D9SetVertexShader(TerrainVertexShaderFO);
						//}
						//else
							RwD3D9SetVertexShader(vshader);
					}
					else	RwD3D9SetVertexShader(instancedData->vertexShader);

					// 2004/07/24 마고자
					// 러프맵 찍을때 텍스쳐가 날아가는 경우가 있는 것으로 확인된다.
					// 일단 죽지 않게 if 걸어서 어설트로 처리한다.
					assert( instancedData->material->texture->refCount > 0 ); 

					if(instancedData->material->texture->refCount > 0)
					{
						RENDER_PRIMITIVE();
					}

				}

				// 2005.3.31 gemani
				CS_RESENTRYHEADER_UNLOCK(resEntryHeader);

				cur_data = cur_data->next;
			}

			// 초기화.
			for(k=0; k<8; ++k)
			{
				RwD3D9SetTextureStageState(k, D3DTSS_TEXCOORDINDEX,	k);
				RwD3D9SetTextureStageState(k, D3DTSS_COLOROP, D3DTOP_DISABLE);
				RwD3D9SetTextureStageState(k, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
			for(k=0; _TSSArgTempSupport && k<8; ++k)
				RwD3D9SetTextureStageState(k, D3DTSS_RESULTARG,	D3DTA_CURRENT);
			RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}
	}


	// 6 : 한장 - 러프맵 그로스 사용 안함.
	{
		// i == 6 인경우.

		i = 6;

		cur_data = lBuffer[i];
		textureOpSetupCallBack = textureOpSetup11; // (*renderCBs)[i].textureOpSetupCallBack_[j];
		if( cur_data && textureOpSetupCallBack )
		{
			// 텍스쳐 스테이지 설정.
			textureOpSetupCallBack(MyVertexShaderEnable);

			while(cur_data)
			{
				resEntryHeader = cur_data->resEntryHeader;

				//>@ 2005.3.31 gemani
				CS_RESENTRYHEADER_LOCK(resEntryHeader);

				if(!resEntryHeader->isLive)
				{
					CS_RESENTRYHEADER_UNLOCK(resEntryHeader);

					cur_data = cur_data->next;
					continue;
				}
				//<@

				if(resEntryHeader != before_resEntryHeader)
				{
					before_resEntryHeader = resEntryHeader;
					SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
				}

				instancedData = cur_data->instancedData;

				if(instancedData->material && instancedData->material->texture)
				{
					//if(i==0)
					//	instancedData->vertexAlpha = TRUE;

					SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);

					// 텍스쳐 설정.
					vshader = perInstanceTextureSetup11_Rough( instancedData ) ; //(*renderCBs)[i].perInstanceTextureSetupCallBack_[j](instancedData);

					if(MyVertexShaderEnable)
					{
						//if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
						//{
						//	RwD3D9SetVertexShader(TerrainVertexShaderFO);
						//}
						//else
							RwD3D9SetVertexShader(vshader);
					}
					else	RwD3D9SetVertexShader(instancedData->vertexShader);

					// 2004/07/24 마고자
					// 러프맵 찍을때 텍스쳐가 날아가는 경우가 있는 것으로 확인된다.
					// 일단 죽지 않게 if 걸어서 어설트로 처리한다.
					assert( instancedData->material->texture->refCount > 0 ); 

					if(instancedData->material->texture->refCount > 0)
					{
						RENDER_PRIMITIVE();
					}

				}

				// 2005.3.31 gemani
				CS_RESENTRYHEADER_UNLOCK(resEntryHeader);

				cur_data = cur_data->next;
			}

			// 초기화.
			for(k=0; k<8; ++k)
			{
				RwD3D9SetTextureStageState(k, D3DTSS_TEXCOORDINDEX,	k);
				RwD3D9SetTextureStageState(k, D3DTSS_COLOROP, D3DTOP_DISABLE);
				RwD3D9SetTextureStageState(k, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
			for(k=0; _TSSArgTempSupport && k<8; ++k)
				RwD3D9SetTextureStageState(k, D3DTSS_RESULTARG,	D3DTA_CURRENT);
			RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}
	}
}
/*extern void 
RpMTextureDrawEnd(RwBool	bUseShader)
{
	RxD3D9ResEntryHeader	*resEntryHeader;
	RxD3D9ResEntryHeader	*before_resEntryHeader = NULL;
	RxD3D9InstanceData		*instancedData;
	MTextureMaterialData	*pData;
	RenderData				*cur_data;
	RwInt32					j;

	RwBool					MyVertexShaderEnable;
	RwBool					lighting;
	RwBool					vertexAlphaBlend;
	RwBool					bUseAlpha = FALSE;

	if(bUseShader)
	{
		MyVertexShaderEnable = VertexShaderEnable;
	}
	else
	{
		MyVertexShaderEnable = FALSE;
	}

	// 마고자 (2004-08-13 오후 3:59:48) : 지형만 컬모드 변경 해봄..
	//RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLBACK	);

	RwD3D9SetPixelShader(NULL);											// Set the Default Pixel shader
	
	RwD3D9GetRenderState(D3DRS_LIGHTING, &lighting);					// check lighting
	
	vertexAlphaBlend = _rwD3D9RenderStateIsVertexAlphaEnable();			//Get vertex alpha Blend state
	
	#define	RENDER_PRIMITIVE()																		\
	if (resEntryHeader->indexBuffer != NULL)														\
	{																								\
		RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType				,		\
								   instancedData->baseIndex									,		\
								   0, instancedData->numVertices							,		\
								   instancedData->startIndex, instancedData->numPrimitives	);		\
	}																								\
	else																							\
	{																								\
		RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType	,							\
							instancedData->baseIndex					,							\
							instancedData->numPrimitives				);							\
	}

	// 모인 걸루 그리자 ^^
	cur_data = lBuffer[0];
	if(cur_data)
	{
		RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
		RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE		);
		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
		
		while(cur_data)
		{
			resEntryHeader = cur_data->resEntryHeader;

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Lock
			CS_RESENTRYHEADER_LOCK( resEntryHeader );
			//@} DDonSS

			//>@ 2005.3.31 gemani
			if(!resEntryHeader->isLive)
			{
				//@{ 20050513 DDonSS : Threadsafe
				// ResEntry Unlock
				CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
				//@} DDonSS

				cur_data = cur_data->next;
				continue;
			}
			//<@

			if(resEntryHeader != before_resEntryHeader)
			{
				before_resEntryHeader = resEntryHeader;
				SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
			}
			
			instancedData = cur_data->instancedData;

			if( instancedData->material && instancedData->material->texture )
			{
				instancedData->vertexAlpha = TRUE;

				SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);

				if(MyVertexShaderEnable)
				{
					if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
					{
						RwD3D9SetVertexShader(TerrainVertexShaderFO);
					}
					else
						RwD3D9SetVertexShader(TerrainVertexShader);
				}
				else	RwD3D9SetVertexShader(instancedData->vertexShader);

				// 2004/07/24 마고자
				// 러프맵 찍을때 텍스쳐가 날아가는 경우가 있는 것으로 확인된다.
				// 일단 죽지 않게 if 걸어서 어설트로 처리한다.
				#ifdef _DEBUG			
				assert( instancedData->material->texture->refCount > 0 ); 
				#endif

				if( instancedData->material->texture->refCount > 0 )
				{

					RwD3D9SetTexture(instancedData->material->texture,		0);
					RENDER_PRIMITIVE();
				}

			}

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Unlock
			CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
			//@} DDonSS

			cur_data = cur_data->next;
		}

		RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}

	cur_data = lBuffer[1];
	if(cur_data)
	{
		RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
		RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

		RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

		RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
		RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
		RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE	);

		RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   __sTextureBlendMode	);
		RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
		RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	1);
		RwD3D9SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,	2);

		while(cur_data)
		{
			resEntryHeader = cur_data->resEntryHeader;

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Lock
			CS_RESENTRYHEADER_LOCK( resEntryHeader );
			//@} DDonSS

			//>@ 2005.3.31 gemani
			if(!resEntryHeader->isLive)
			{
				//@{ 20050513 DDonSS : Threadsafe
				// ResEntry Unlock
				CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
				//@} DDonSS

				cur_data = cur_data->next;
				continue;
			}
			//<@

			if(resEntryHeader != before_resEntryHeader)
			{
				before_resEntryHeader = resEntryHeader;
				SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
			}

			instancedData = cur_data->instancedData;
			pData = MTEXTUREMATERIAL(instancedData->material);

			SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);
			
			if(MyVertexShaderEnable)
			{
				if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
				{
					RwD3D9SetVertexShader(TerrainVertexShaderFO);
				}
				else
					RwD3D9SetVertexShader(TerrainVertexShader);
			}
			else	RwD3D9SetVertexShader(instancedData->vertexShader);
			
			RwD3D9SetTexture( instancedData->material->texture	,		0);
			RwD3D9SetTexture( pData->texture[0]					,		1);
			RwD3D9SetTexture( pData->texture[1]					,		2);

			RENDER_PRIMITIVE();

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Unlock
			CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
			//@} DDonSS

			cur_data = cur_data->next;
		}

		for(j=0;j<4;++j)
		{
			RwD3D9SetTextureStageState(j, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(j, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	cur_data = lBuffer[2];
	if(cur_data)
	{
		while(cur_data)
		{
			resEntryHeader = cur_data->resEntryHeader;

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Lock
			CS_RESENTRYHEADER_LOCK( resEntryHeader );
			//@} DDonSS

			//>@ 2005.3.31 gemani
			if(!resEntryHeader->isLive)
			{
				//@{ 20050513 DDonSS : Threadsafe
				// ResEntry Unlock
				CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
				//@} DDonSS

				cur_data = cur_data->next;
				continue;
			}
			//<@

			if(resEntryHeader != before_resEntryHeader)
			{
				before_resEntryHeader = resEntryHeader;
				SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
			}

			instancedData = cur_data->instancedData;
			pData = MTEXTUREMATERIAL(instancedData->material);

			SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);

			RwD3D9SetTexture(instancedData->material->texture,		0);
					
			RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE		);

			if(MyVertexShaderEnable)
			{
				if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
				{
					RwD3D9SetVertexShader(TerrainVertexShaderFO);
				}
				else
					RwD3D9SetVertexShader(TerrainVertexShader);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			// 두번째 패스에서..
			RwD3D9SetTexture( pData->texture[0]					,		0);
			RwD3D9SetTexture( pData->texture[1]					,		1);

			RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

			RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
			RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	

			if(MyVertexShaderEnable)
			{
				RwD3D9SetVertexShader(TerrainVertexShader2);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	1);
				RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	2);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	1);
			RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Unlock
			CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
			//@} DDonSS

			cur_data = cur_data->next;
		}

		for(j=0;j<2;++j)
		{
			RwD3D9SetTextureStageState(j, D3DTSS_TEXCOORDINDEX,	j);
			RwD3D9SetTextureStageState(j, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(j, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	cur_data = lBuffer[3];
	if(cur_data)
	{
		RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
		RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

		RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

		RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
		RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
		RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

		RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
		RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

		RwD3D9SetTextureStageState( 4, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
		RwD3D9SetTextureStageState( 4, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
		RwD3D9SetTextureStageState( 4, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 4, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);
		
		RwD3D9SetTextureStageState( 5, D3DTSS_COLOROP	,   __sTextureBlendMode	);
		RwD3D9SetTextureStageState( 5, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
		RwD3D9SetTextureStageState( 5, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
		RwD3D9SetTextureStageState( 5, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

		RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
		RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	1);
		RwD3D9SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,	2);
		RwD3D9SetTextureStageState(3, D3DTSS_TEXCOORDINDEX,	3);
		RwD3D9SetTextureStageState(4, D3DTSS_TEXCOORDINDEX,	4);

		while(cur_data)
		{
			resEntryHeader = cur_data->resEntryHeader;

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Lock
			CS_RESENTRYHEADER_LOCK( resEntryHeader );
			//@} DDonSS

			//>@ 2005.3.31 gemani
			if(!resEntryHeader->isLive)
			{
				//@{ 20050513 DDonSS : Threadsafe
				// ResEntry Unlock
				CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
				//@} DDonSS

				cur_data = cur_data->next;
				continue;
			}
			//<@

			if(resEntryHeader != before_resEntryHeader)
			{
				before_resEntryHeader = resEntryHeader;
				SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
			}

			instancedData = cur_data->instancedData;
			pData = MTEXTUREMATERIAL(instancedData->material);

			SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);
						
			RwD3D9SetTexture( instancedData->material->texture	,		0);
			RwD3D9SetTexture( pData->texture[0]					,		1);
			RwD3D9SetTexture( pData->texture[1]					,		2);
			RwD3D9SetTexture( pData->texture[2]					,		3);
			RwD3D9SetTexture( pData->texture[3]					,		4);
			RwD3D9SetTexture( NULL								,		5);

			if(MyVertexShaderEnable)
			{
				if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
				{
					RwD3D9SetVertexShader(TerrainVertexShaderFO);
				}
				else
					RwD3D9SetVertexShader(TerrainVertexShader);
			}
			else
			{
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Unlock
			CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
			//@} DDonSS

			cur_data = cur_data->next;
		}

		for(j=0;j<6;++j)
		{
			RwD3D9SetTextureStageState(j, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(j, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	cur_data = lBuffer[4];
	if(cur_data)
	{
		while(cur_data)
		{
			resEntryHeader = cur_data->resEntryHeader;

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Lock
			CS_RESENTRYHEADER_LOCK( resEntryHeader );
			//@} DDonSS

			//>@ 2005.3.31 gemani
			if(!resEntryHeader->isLive)
			{
				//@{ 20050513 DDonSS : Threadsafe
				// ResEntry Unlock
				CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
				//@} DDonSS

				cur_data = cur_data->next;
				continue;
			}
			//<@

			if(resEntryHeader != before_resEntryHeader)
			{
				before_resEntryHeader = resEntryHeader;
				SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
			}

			instancedData = cur_data->instancedData;
			pData = MTEXTUREMATERIAL(instancedData->material);

			SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);

			RwD3D9SetTexture( instancedData->material->texture	,		0);
			RwD3D9SetTexture( pData->texture[0]					,		1);
			RwD3D9SetTexture( pData->texture[1]					,		2);
			RwD3D9SetTexture( NULL								,		3);

			RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
			RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

			RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_BLENDCURRENTALPHA);
			RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2	,	D3DTA_CURRENT		);
			RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG1	,	D3DTA_CURRENT		);
			RwD3D9SetTextureStageState( 3, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			if(MyVertexShaderEnable)
			{
				if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
				{
					RwD3D9SetVertexShader(TerrainVertexShaderFO);
				}
				else
					RwD3D9SetVertexShader(TerrainVertexShader);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
				RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	1);
				RwD3D9SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,	2);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			// Second Pass

			RwD3D9SetTexture( pData->texture[2]					,		0);
			RwD3D9SetTexture( pData->texture[3]					,		1);
			RwD3D9SetTexture( NULL								,		2);

			RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

			RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP	,   D3DTOP_DISABLE		);
			RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetTextureStageState( 3, D3DTSS_COLOROP	,   D3DTOP_DISABLE		);
			RwD3D9SetTextureStageState( 3, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
			RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	

			if(MyVertexShaderEnable)
			{
				RwD3D9SetVertexShader(TerrainVertexShader3);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	3);
				RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	4);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Unlock
			CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
			//@} DDonSS

			cur_data = cur_data->next;
		}

		for(j=0;j<4;++j)
		{
			RwD3D9SetTextureStageState(j, D3DTSS_TEXCOORDINDEX,	j);
			RwD3D9SetTextureStageState(j, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(j, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	cur_data = lBuffer[5];
	if(cur_data)
	{
		while(cur_data)
		{
			resEntryHeader = cur_data->resEntryHeader;

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Lock
			CS_RESENTRYHEADER_LOCK( resEntryHeader );
			//@} DDonSS

			//>@ 2005.3.31 gemani
			if(!resEntryHeader->isLive)
			{
				//@{ 20050513 DDonSS : Threadsafe
				// ResEntry Unlock
				CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
				//@} DDonSS

				cur_data = cur_data->next;
				continue;
			}
			//<@

			if(resEntryHeader != before_resEntryHeader)
			{
				before_resEntryHeader = resEntryHeader;
				SetResEntryHeaderInfo(resEntryHeader,cur_data->atomic);
			}

			instancedData = cur_data->instancedData;
			pData = MTEXTUREMATERIAL(instancedData->material);

			SetRenderState(instancedData,lighting,&vertexAlphaBlend,cur_data->flags);

			// 두장..
			// 세번에 나뉘어 찍는다..
			// 한번에 못찍으면 두번에 나뉘어 찍는다..

			// 첫장 찍고..
			RwD3D9SetTexture(instancedData->material->texture,		0);
			
			RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE		);

			if(MyVertexShaderEnable)
			{
				if(cur_data->sectordist >= RPMTEX_FadeDist)		// fadeout
				{
					RwD3D9SetVertexShader(TerrainVertexShaderFO);
				}
				else
					RwD3D9SetVertexShader(TerrainVertexShader);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	0);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			// 두번째 패스에서..

			RwD3D9SetTexture( pData->texture[0]					,		0);
			RwD3D9SetTexture( pData->texture[1]					,		1);

			RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

			RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
			RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	

			if(MyVertexShaderEnable)
			{
				RwD3D9SetVertexShader(TerrainVertexShader2);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	1);
				RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	2);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			// 세번째 패스에서..

			RwD3D9SetTexture( pData->texture[2]					,		0);
			RwD3D9SetTexture( pData->texture[3]					,		1);

			RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
			RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);

			RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP	,   __sTextureBlendMode	);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2	,	D3DTA_DIFFUSE		);
			RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP	,   D3DTOP_DISABLE		);

			if(MyVertexShaderEnable)
			{
				RwD3D9SetVertexShader(TerrainVertexShader3);
			}
			else
			{
				RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	3);
				RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	4);
				RwD3D9SetVertexShader(instancedData->vertexShader);
			}

			RENDER_PRIMITIVE();

			RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	1);
			RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			//@{ 20050513 DDonSS : Threadsafe
			// ResEntry Unlock
			CS_RESENTRYHEADER_UNLOCK( resEntryHeader );
			//@} DDonSS

			cur_data = cur_data->next;
		}

		for(j=0;j<2;++j)
		{
			RwD3D9SetTextureStageState(j, D3DTSS_TEXCOORDINDEX,	j);
			RwD3D9SetTextureStageState(j, D3DTSS_COLOROP, D3DTOP_DISABLE);
			RwD3D9SetTextureStageState(j, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	// 마고자 (2004-08-13 오후 3:59:48) : 지형만 컬모드 변경 해봄..
	//RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODENACULLMODE	);
}*/
//@} Jaewon

//@{ Jaewon 20050512
// Terrain gloss map support
RwBool RpMTextureEnableGlossMap(RwBool enable)
{
	_GlossMapEnabled = MaxTexturePerPass>=4?enable:FALSE;
	
	return _GlossMapEnabled;
}
RwBool RpMTextureIsGlossMapEnabled()
{
	return _GlossMapEnabled;
}
//@} Jaewon