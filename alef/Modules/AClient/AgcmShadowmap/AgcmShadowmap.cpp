#include "AgcmShadowmap.h"
#include "AgcmRender.h"
#include "AgcmCharacter.h"

static AgcmShadowmap* _pThis = NULL;
static RwUInt32 _rwD3D9VertexShaderMaxConstants;
static CONST UINT32 cShadowMapSize = 512;
static CONST FLOAT cDepthRange = 3.0f;

// shader source codes
const char g_vs11_weight0[] =
{
	"dcl_normal v4\n"
	"mov r3, v0\n"
	"m4x4 r0, v0, c0\n"
	"mov r5, v4\n"
};
const char g_vs11_weight1[] =
{
	"dcl_blendindices0 v3\n"
	"dcl_normal v4\n"
	"mov a0.x, v3.x\n"
	"m4x3 r3.xyz, v0, c[a0.x+15]\n"
	"m3x3 r5.xyz, v4, c[a0.x+15]\n"
	"mov r3.w, v0.w\n"
	"m4x4 r0, r3, c0\n"
};
const char g_vs11_weight2[] =
{
	"dcl_blendweight0 v2\n"
	"dcl_blendindices0 v3\n"
	"dcl_normal v4\n"
	"mov a0.x, v3.x\n"
	"mul r8, v2.x, c[a0.x+15]\n"
	"mul r9, v2.x, c[a0.x+16]\n"
	"mul r10, v2.x, c[a0.x+17]\n"
	"mov a0.x, v3.y\n"
	"mad r8, v2.y, c[a0.x+15], r8\n"
	"mad r9, v2.y, c[a0.x+16], r9\n"
	"mad r10, v2.y, c[a0.x+17], r10\n"
	"m4x3 r3.xyz, v0, r8\n"
	"m3x3 r5.xyz, v4, r8\n"
	"mov r3.w, v0.w\n"
	"m4x4 r0, r3, c0\n"
};
const char g_vs11_weight3[] =
{
	"dcl_blendweight0 v2\n"
	"dcl_blendindices0 v3\n"
	"dcl_normal v4\n"
	"mov a0.x, v3.x\n"
	"mul r8, v2.x, c[a0.x+15]\n"
	"mul r9, v2.x, c[a0.x+16]\n"
	"mul r10, v2.x, c[a0.x+17]\n"
	"mov a0.x, v3.y\n"
	"mad r8, v2.y, c[a0.x+15], r8\n"
	"mad r9, v2.y, c[a0.x+16], r9\n"
	"mad r10, v2.y, c[a0.x+17], r10\n"
	"mov a0.x, v3.z\n"
	"mad r8, v2.z, c[a0.x+15], r8\n"
	"mad r9, v2.z, c[a0.x+16], r9\n"
	"mad r10, v2.z, c[a0.x+17], r10\n"
	"m4x3 r3.xyz, v0, r8\n"
	"m3x3 r5.xyz, v4, r8\n"
	"mov r3.w, v0.w\n"
	"m4x4 r0, r3, c0\n"
};
const char g_vs11_weight4[] =
{
	"dcl_blendweight0 v2\n"
	"dcl_blendindices0 v3\n"
	"dcl_normal v4\n"
	"mov a0.x, v3.x\n"
	"mul r8, v2.x, c[a0.x+15]\n"
	"mul r9, v2.x, c[a0.x+16]\n"
	"mul r10, v2.x, c[a0.x+17]\n"
	"mov a0.x, v3.y\n"
	"mad r8, v2.y, c[a0.x+15], r8\n"
	"mad r9, v2.y, c[a0.x+16], r9\n"
	"mad r10, v2.y, c[a0.x+17], r10\n"
	"mov a0.x, v3.z\n"
	"mad r8, v2.z, c[a0.x+15], r8\n"
	"mad r9, v2.z, c[a0.x+16], r9\n"
	"mad r10, v2.z, c[a0.x+17], r10\n"
	"mov a0.x, v3.w\n"
	"mad r8, v2.w, c[a0.x+15], r8\n"
	"mad r9, v2.w, c[a0.x+16], r9\n"
	"mad r10, v2.w, c[a0.x+17], r10\n"
	"m4x3 r3.xyz, v0, r8\n"
	"m3x3 r5.xyz, v4, r8\n"
	"mov r3.w, v0.w\n"
	"m4x4 r0, r3, c0\n"
};

const char g_vs11_shadowmapheader[] =
{
	"vs.1.1\n"
	"dcl_position v0\n"
	"dcl_texcoord0 v1\n"
};
const char g_vs11_shadowmapbody[] =
{
	"mov oPos, r0\n"
	"mov oT0, v1\n"
	"mov oT1, r0.z\n"
};
const char g_ps11_shadowmap[] =
{
	"ps.1.1\n"
	"tex t0\n"
	"texcoord t1\n"
	"mov r0.rgb, t1\n"
	//@{ Jaewon 20050117
	"+mov r0.a, t0.a\n"
	//@} Jaewon
};
//@{ Jaewon 20050117
const char g_ps14_shadowmap[] = 
{
	"ps.1.4\n"
	"texld r0, t0\n"
	"texld r1, t1\n"
	"mov r0.rgb, r1\n"
};
//@} Jaewon

const char g_vs11_shadowsceneheader[] =
{
	"vs.1.1\n"
	"dcl_position v0\n"
	"dcl_texcoord0 v1\n"
};
const char g_vs11_shadowscenebody[] =
{
	"mov oPos, r0\n"
	"add oPos.z, c14.x, r0.z\n"
	"m4x4 r0, r3, c8\n"
	"mov oT0, v1\n"
	"mov oT1, r0\n"
	//"add oT2, r0, c12.xyyy\n"
	//"add oT3, r0, c12.yxyy\n"
	"m4x4 r1, r3, c4\n"
	"add oT2, r1.z, c14.y\n"
	"dp3 r5, r5, -c13\n"
	"max r5, r5, c12.y\n"
	"mul oD0, c12.z, r5\n"
};
//@{ Jaewon 20050117
const char g_vs14_shadowscenebody[] =
{
	"mov oPos, r0\n"
	"mov oT5, v1\n"
	"add oPos.z, c14.x, r0.z\n"
	"m4x4 r0, r3, c8\n"
	"mov oT0, r0\n"
	"add oT1, r0, c12.yxyy\n"
	"add oT2, r0, c12.xyyy\n"
	"add oT3, r0, c12.xxyy\n"
	"m4x4 r1, r3, c4\n"
	"add r2, r1.z, c14.w\n"
	"min r2, r2, c12.w\n"
	"max oT4, r2, c12.y\n"
	"dp3 r5, r5, -c13\n"
	"max r5, r5, c12.y\n"
	"mul oD0, c12.z, r5\n"
};
//@} Jaewon

//const char g_ps11_shadowscene[] =
//{
//	"ps.1.2\n"
//	"def c7, 1.0, 0.0, 0.0, -0.005\n"
//	"def c6, 0.7, 0.7, 0.7, 0.7\n"
//	"def c5, 0.334, 0.334, 0.334, 0.0\n"
//	"def c4, 0.0, 0.0, 0.0, 1.0\n"
//	"def c3, 1.0, 1.0, 0.0, 0.0\n"
//	"texcoord t0\n"
//	"tex t1\n"
//	"tex t2\n"
//	"tex t3\n"
//	"sub r0.rgb, t0, t1\n"
//	"sub r1.rgb, t0, t2\n"
//	"lrp t1.rgb, c7, r0, r1\n"
//	"sub r1.rgb, t0, t3\n"
//	"lrp t2.rgb, c3, t1, r1\n"
//	"cmp r1, -t2, c4, v0\n"
//	"dp3_sat r0.rgb, r1, c5\n"
//	"+mov r0.a, c4.a\n"
//};
const char g_ps11_shadowscene[] =
{
	"ps.1.2\n"
	"def c0, 0.0, 0.0, 0.0, 0.0\n"
	"tex t0\n"
	"tex t1\n"
	"texcoord t2\n"
	"sub r1.rgb, t2, t1\n"
	"cmp_sat r0.rgb, -r1, c0, v0\n"
	"+mov r0.a, t0.a\n"
};
//@{ Jaewon 20050117
const char g_ps14_shadowscene[] = 
{
	"ps.1.4\n"
	"def c0, 0, 0, 0, 0\n"
	"def c1, 1.0, 1.0, 1.0, 1.0\n"
	"def c2, 0.25, 0.25, 0.25, 0.25\n"
	// Load a texel at (u, v).
	"texld r0, t0\n"
	// Get a base texture coordinate.
	"texcrd r4.xyz, t0\n"
	// Load a texel at (u, v+1).
	"texld r1, t1\n"
	// Load a texel at (u+1, v).
	"texld r2, t2\n"
	// Unpack sample 0.
	"mov_d8 r0.r, r0.r\n"
	"add r0.r, r0.r, r0.g\n"
	// Unpack sample 1.
	"mov_d8 r1.r, r1.r\n"
	"add r0.g, r1.r, r1.g\n"
	// Unpack sample 2.
	"mov_d8 r2.r, r2.r\n"
	"add r0.b, r2.r, r2.g\n"
	// Compute 8*u, 8*v for subtexel precision dependent reads in phase 2.
	//"mov_x8 r1, r4.x\n"
	//"mov_x8 r2, r4.y\n"
	
	"phase\n"

	// Get a base alpha.
	"texld r3, t5\n"
	// Dependent-read a subtexel precision value for the u coordinate.
	//"texld r4, r1\n"
	// Dependent-read a subtexel precision value for the v coordinate.
	//"texld r5, r2\n"
	// Get a light-space depth from vertex shader.
	"texcrd r1.xyz, t4\n"
	// Load a texel at (u+1, v+1).
	"texld r2, t3\n"
	// Unpack sample 3.
	"mov_d8 r2.r, r2.r\n"
	"add r0.a, r2.r, r2.g\n"
	// Compare shadow map samples to the surface depth.
	"sub r0, r1.z, r0\n"
	"cmp r0, -r0, c0, v0\n"
	// Do percentage closer filtering.
	//"lrp r1, r5, r0.g, r0.r\n"
	//"lrp r2, r5, r0.a, r0.b\n"
	//"lrp r0.rgb, r4, r2, r1\n"
	"dp4 r0, c2, r0\n"
	// Set default alpha.
	"mov r0.a, r3.a\n"
};
//@} Jaewon
/*
const char g_vs11_shadowmapbody[] =
{
	"mov oPos, r0\n"
	"mov oT0, v1\n"
	"mul r1.z, r0.z, c4.x\n"
	"mul r1.z, r1.z, c4.z\n"
	"frc r2.y, r1.z\n"
	"add r1.z, r1.z, -r2.y\n"
	"mul oT1.xyz, r1.z, c4.w\n"
	"mov oT1.y, r2.y\n"
};
const char g_ps11_shadowmap[] =
{
	"ps.1.1\n"
	"tex t0\n"
	"texcoord t1\n"
	"mov r0.rgb, t1\n"
	"mov r0.a, t0.a\n"
};
const char g_vs11_shadowscenebody[] =
{
	"mov oPos, r0\n"
	"add oPos.z, c14.x, r0.z\n"
	//"mov oT0, v1\n"
	"m4x4 r0, r3, c8\n"
	"mov oT1, r0\n"
	"add oT2, r0, c12.xyyy\n"
	"add oT3, r0, c12.yxyy\n"
	"m4x4 r1, r3, c4\n"
	"add r1.z, r1.z, c14.y\n"
	"mul r1.z, r1.z, c14.z\n"
	"frc r2.y, r1.z\n"
	"add r1.z, r1.z, -r2.y\n"
	"mul oD0.xyz, r1.z, c14.w\n"
	"mov oD0.y, r2.y\n"
};
const char g_ps11_shadowscene[] =
{
	"ps.1.2\n"
	"def c7, 1.0, 1.0, -1.0, 1.0\n"
	"def c6, 0.0, 0.0, 0.0, 0.0\n"
	"def c5, 0.0, -1.0, 1.0, 0.0\n"
	"def c4, 1.0, 1.0, 1.0, 1.0\n"
	"def c3, 0.3, 0.0, 0.0, 0.0\n"
	"def c2, 0.7, 0.7, 0.7, 1.0\n"
	//"texcoord t0\n"
	"tex t1\n"
	"sub r0, v0, t1\n"
	"mul r0, r0, c7\n"
	"cmp r1, -r0, c4, c6\n"
	"dp3 r0, r1, c5\n"
	"cnd t0, r0.a, c6, c3\n"
	"dp3 r0.rgb, r1, t0\n"
	"add r0.rgb, r0, c2\n"
	"+mov r0.a, c4.a\n"
};*/

AgcmShadowmap::AgcmShadowmap() :
 m_pAgcmRender(NULL),
 m_pAgcmCharacter(NULL),
 m_pShadowMap(NULL),
 m_bOn(FALSE), 
 m_pShadowMapPShader(NULL),
 m_pShadowScenePShader(NULL),
 m_bShaderSupported(FALSE),
 m_pShadowCam(NULL),
 m_pShadowMapZ(NULL),
 m_fPrevRadius(0.0f),
 m_atomicPipeline(NULL),
 m_atomicSkinPipeline(NULL),
 m_atomicPipelineForShadowmap(NULL),
 m_atomicSkinPipelineForShadowmap(NULL),
 m_pShadowMapTexture(NULL),
 m_pPlayerClump(NULL),
 m_bPs14Supported(false),
 m_p11bitEncodingTexture(NULL),
 m_pSubTexelPrecisionTexture(NULL),
 m_fDepthBias(0.01f)
{
	SetModuleName("AgcmShadowmap");

	for(INT32 i=0; i<5; ++i)
	{
		m_pShadowMapVShader[i] = NULL;
		m_pShadowSceneVShader[i] = NULL;
	}

	ASSERT(_pThis==NULL);
	_pThis = this;
}

AgcmShadowmap::~AgcmShadowmap()
{
	OnDestroy();
}

BOOL AgcmShadowmap::OnAddModule()
{
	m_pAgcmRender = (AgcmRender*)GetModule("AgcmRender");

	if(!m_pAgcmRender)
	{
		ASSERT(!"Can't get the AgcmRender module in AgcmShadowmap::OnAddModule()!");
		return FALSE;
	}

	if(!m_pAgcmRender->SetCallbackPostRender(postRenderCB, this))
	{
		ASSERT(!"Can't set postRenderCB callback in AgcmShadowmap::OnAddModule()!");
		return FALSE;
	}

	if(!m_pAgcmRender->SetCallbackPreRender(preRenderCB, this))
	{
		ASSERT(!"Can't set preRenderCB callback in AgcmShadowmap::OnAddModule()!");
		return FALSE;
	}

	return TRUE;
}
PVOID AgcmShadowmap::getShader(const char* pSrc, BOOL bPixel) const
{
	PVOID ret = NULL;
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsgs = NULL;

	HRESULT hresult = D3DXAssembleShader( pSrc, strlen(pSrc), NULL, NULL, 0,/*D3DXSHADER_SKIPVALIDATION,*/ &pShader, &pErrorMsgs);
	if( FAILED( hresult ) )
	{
		const char* pLog = (const char*)pErrorMsgs->GetBufferPointer();
		OutputDebugString(pLog);
		goto FAIL;
	}

	RwBool hr = bPixel ? RwD3D9CreatePixelShader((const RwUInt32 *)(pShader->GetBufferPointer()), &ret) : 
						 RwD3D9CreateVertexShader((const RwUInt32 *)(pShader->GetBufferPointer()), &ret);

	DEF_SAFERELEASE( pErrorMsgs );
	DEF_SAFERELEASE( pShader );

	return hr ? ret : NULL;

FAIL:
	DEF_SAFERELEASE( pErrorMsgs );
	DEF_SAFERELEASE( pShader );
	return NULL;
}

BOOL AgcmShadowmap::OnInit()
{
	return TRUE;
}

BOOL AgcmShadowmap::OnDestroy()
{
	Off();
	
	m_pPlayerClump = NULL;
	
	return TRUE;	
}

bool AgcmShadowmap::On()
{
	if( !m_pAgcmRender->m_bVertexShaderEnable )		return false;
	if( m_bOn )										return true;
		
	ASSERT(!m_pShadowMap);

	m_bShaderSupported = FALSE;

	// check shaders.
	const D3DCAPS9* pCaps = (const D3DCAPS9*)RwD3D9GetCaps();
	_rwD3D9VertexShaderMaxConstants = pCaps->MaxVertexShaderConst;

	if( (pCaps->PixelShaderVersion & 0xffff) < 0x0102 )		return false;

	m_bPs14Supported	= ((pCaps->PixelShaderVersion & 0xffff)) >= 0x0104 ? true : false;
	m_pShadowMap		= RwRasterCreate( cShadowMapSize, cShadowMapSize, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888 );
	if( !m_pShadowMap )			return false;
		
	m_pShadowMapZ		= RwRasterCreate(cShadowMapSize, cShadowMapSize, 0, rwRASTERTYPEZBUFFER);
	if( !m_pShadowMapZ )		goto FAIL;

	m_pShadowMapTexture	= RwTextureCreate(m_pShadowMap);
	if( !m_pShadowMapTexture )	goto FAIL;

	RwTextureSetAddressing( m_pShadowMapTexture, rwTEXTUREADDRESSCLAMP );//rwTEXTUREADDRESSBORDER);
	RwTextureSetFilterMode( m_pShadowMapTexture, rwFILTERNEAREST );

	// create a 11-bit encoding texture.
	m_p11bitEncodingTexture = create11BitEncodingMap();
	if( !m_p11bitEncodingTexture)
	{
		m_bPs14Supported = false;
	}
	else
	{
		RwTextureSetAddressing( m_p11bitEncodingTexture, rwTEXTUREADDRESSCLAMP );
		RwTextureSetFilterMode( m_p11bitEncodingTexture, rwFILTERNEAREST );
	}

	LockFrame();

	bool bLockStart = true;

	// create a camera for the shadow map.
	m_pShadowCam = RwCameraCreate();
	if( !m_pShadowCam )		goto FAIL;

	RwCameraSetFrame(m_pShadowCam, RwFrameCreate());
	if( !RwCameraGetFrame( m_pShadowCam ) )		goto FAIL;

	RwCameraSetProjection( m_pShadowCam, rwPARALLEL );
	RwCameraSetRaster( m_pShadowCam, m_pShadowMap );
	RwCameraSetZRaster (m_pShadowCam, m_pShadowMapZ );

	bLockStart = false;

	UnlockFrame();

	// assemble & create shaders.
	char buf[4096];
	// shadow map shaders
	strcpy(buf, g_vs11_shadowmapheader);
	strcat(buf, g_vs11_weight0);
	strcat(buf, g_vs11_shadowmapbody);
	m_pShadowMapVShader[0] = getShader(buf);
	ASSERT(m_pShadowMapVShader[0]);

	strcpy(buf, g_vs11_shadowmapheader);
	strcat(buf, g_vs11_weight1);
	strcat(buf, g_vs11_shadowmapbody);
	m_pShadowMapVShader[1] = getShader(buf);
	ASSERT(m_pShadowMapVShader[1]);

	strcpy(buf, g_vs11_shadowmapheader);
	strcat(buf, g_vs11_weight2);
	strcat(buf, g_vs11_shadowmapbody);
	m_pShadowMapVShader[2] = getShader(buf);
	ASSERT(m_pShadowMapVShader[2]);

	strcpy(buf, g_vs11_shadowmapheader);
	strcat(buf, g_vs11_weight3);
	strcat(buf, g_vs11_shadowmapbody);
	m_pShadowMapVShader[3] = getShader(buf);
	ASSERT(m_pShadowMapVShader[3]);

	strcpy(buf, g_vs11_shadowmapheader);
	strcat(buf, g_vs11_weight4);
	strcat(buf, g_vs11_shadowmapbody);
	m_pShadowMapVShader[4] = getShader(buf);
	ASSERT(m_pShadowMapVShader[4]);

	m_pShadowMapPShader = getShader( m_bPs14Supported ? g_ps14_shadowmap : g_ps11_shadowmap, TRUE);
	ASSERT( m_pShadowMapPShader );

	// shadow scene shaders
	strcpy(buf, g_vs11_shadowsceneheader);
	strcat(buf, g_vs11_weight0);
	strcat(buf, m_bPs14Supported ? g_vs14_shadowscenebody : g_vs11_shadowscenebody );
	m_pShadowSceneVShader[0] = getShader(buf);
	ASSERT(m_pShadowSceneVShader[0]);

	strcpy(buf, g_vs11_shadowsceneheader);
	strcat(buf, g_vs11_weight1);
	strcat(buf, m_bPs14Supported ? g_vs14_shadowscenebody : g_vs11_shadowscenebody);
	m_pShadowSceneVShader[1] = getShader(buf);
	ASSERT(m_pShadowSceneVShader[1]);

	strcpy(buf, g_vs11_shadowsceneheader);
	strcat(buf, g_vs11_weight2);
	strcat(buf, m_bPs14Supported ? g_vs14_shadowscenebody : g_vs11_shadowscenebody);
	m_pShadowSceneVShader[2] = getShader(buf);
	ASSERT(m_pShadowSceneVShader[2]);

	strcpy(buf, g_vs11_shadowsceneheader);
	strcat(buf, g_vs11_weight3);
	strcat(buf, m_bPs14Supported ? g_vs14_shadowscenebody : g_vs11_shadowscenebody);
	m_pShadowSceneVShader[3] = getShader(buf);
	ASSERT(m_pShadowSceneVShader[3]);

	strcpy(buf, g_vs11_shadowsceneheader);
	strcat(buf, g_vs11_weight4);
	strcat(buf, m_bPs14Supported ? g_vs14_shadowscenebody : g_vs11_shadowscenebody);
	m_pShadowSceneVShader[4] = getShader(buf);
	ASSERT(m_pShadowSceneVShader[4]);

	m_pShadowScenePShader = getShader(m_bPs14Supported ? g_ps14_shadowscene : g_ps11_shadowscene, TRUE);
	ASSERT(m_pShadowScenePShader);

	// create pipelines.
	m_atomicPipeline = createAtomicPipeline();
	if( !m_atomicPipeline )		goto FAIL;

	m_atomicSkinPipeline = createAtomicSkinPipeline();
	if( !m_atomicSkinPipeline )	goto FAIL;

	m_atomicPipelineForShadowmap = createAtomicPipelineForShadowmap();
	if( !m_atomicPipelineForShadowmap )		goto FAIL;

	m_atomicSkinPipelineForShadowmap = createAtomicSkinPipelineForShadowmap();
	if( !m_atomicSkinPipelineForShadowmap )	goto FAIL;

	m_bShaderSupported	= TRUE;
	m_bOn				= TRUE;

	return true;

FAIL:
	if( bLockStart )
		UnlockFrame();

	Off();		// release
	return false;
}

void AgcmShadowmap::Off()
{
	if( !m_bOn )		return;

	ASSERT(m_pShadowMap);			// why null?
		
	if(m_atomicPipeline)
	{
		RxPipelineDestroy(m_atomicPipeline);
		m_atomicPipeline = NULL;
	}

	if(m_atomicSkinPipeline)
	{
		RxPipelineDestroy(m_atomicSkinPipeline);
		m_atomicSkinPipeline = NULL;
	}

	if(m_atomicPipelineForShadowmap)
	{
		RxPipelineDestroy(m_atomicPipelineForShadowmap);
		m_atomicPipelineForShadowmap = NULL;
	}

	if(m_atomicSkinPipelineForShadowmap)
	{
		RxPipelineDestroy(m_atomicSkinPipelineForShadowmap);
		m_atomicSkinPipelineForShadowmap = NULL;
	}
	
	// destroy the camera.
	if(m_pShadowCam)
    {
		RwCameraSetRaster(m_pShadowCam, NULL);
		RwCameraSetZRaster(m_pShadowCam, NULL);

		LockFrame();
        RwFrame* pFrame = RwCameraGetFrame(m_pShadowCam);
        if(pFrame)
        {
            RwCameraSetFrame(m_pShadowCam, NULL);
            RwFrameDestroy(pFrame);
        }
		UnlockFrame();
        
        RwCameraDestroy(m_pShadowCam);
		m_pShadowCam = NULL;
    }

	// destroy a texture.
	if(m_pShadowMapTexture)
	{
		RwTextureSetRaster(m_pShadowMapTexture, NULL);
		RwTextureDestroy(m_pShadowMapTexture);
		m_pShadowMapTexture = NULL;
	}
	
	// destroy ps14-related textures.
	if(m_p11bitEncodingTexture)
	{
		RwTextureDestroy(m_p11bitEncodingTexture);
		m_p11bitEncodingTexture = NULL;
	}
	if(m_pSubTexelPrecisionTexture)
	{
		RwTextureDestroy(m_pSubTexelPrecisionTexture);
		m_pSubTexelPrecisionTexture = NULL;
	}

	// destroy offscreen buffers.
	if(m_pShadowMap)
	{
		RwRasterDestroy(m_pShadowMap);
		m_pShadowMap = NULL;
	}
	if(m_pShadowMapZ)
	{
		RwRasterDestroy(m_pShadowMapZ);
		m_pShadowMapZ = NULL;
	}

	// delete shaders.
	INT32 i;
	for(i=0; i<5; ++i)
	{
		if(m_pShadowMapVShader[i])
		{
			RwD3D9DeleteVertexShader(m_pShadowMapVShader[i]);
			m_pShadowMapVShader[i] = NULL;
		}
	}
	if(m_pShadowMapPShader)
	{
		RwD3D9DeletePixelShader(m_pShadowMapPShader);
		m_pShadowMapPShader = NULL;
	}
	for(i=0; i<5; ++i)
	{
		if(m_pShadowSceneVShader[i])
		{
			RwD3D9DeleteVertexShader(m_pShadowSceneVShader[i]);
			m_pShadowSceneVShader[i] = NULL;
		}
	}
	if(m_pShadowScenePShader)
	{
		RwD3D9DeletePixelShader(m_pShadowScenePShader);
		m_pShadowScenePShader = NULL;
	}
	
	m_fPrevRadius		= 0.0f;

	m_bShaderSupported	= FALSE;
	m_bPs14Supported	= false;
	
	m_fDepthBias		= 0.01f;
	m_bOn				= FALSE;
}

BOOL AgcmShadowmap::SetEnable(BOOL bVal)
{
	if( !bVal )
	{
		Off();
		return TRUE;
	}

	return On() ? TRUE : FALSE;
}

RpAtomic* AgcmShadowmap::setShadowMapPipeline(RpAtomic* pAtomic, void* pData)
{
	AgcmShadowmap* pThis = (AgcmShadowmap*)pData;

	// back up the original.
	RxPipeline* pPipeline;
	RpAtomicGetPipeline(pAtomic, &pPipeline);
	pThis->m_pipeBU.push_back(pPipeline);

	// change the pipe.
	RpAtomicSetPipeline(pAtomic, pThis->m_pAgcmRender->IsSkinedPipe(pAtomic) ? pThis->m_atomicSkinPipelineForShadowmap : pThis->m_atomicPipelineForShadowmap);

	return pAtomic;
}

RpAtomic* AgcmShadowmap::setShadowScenePipeline(RpAtomic* pAtomic, void* pData)
{
	AgcmShadowmap* pThis = (AgcmShadowmap*)pData;

	// back up the original.
	RxPipeline* pPipeline;
	RpAtomicGetPipeline(pAtomic, &pPipeline);
	pThis->m_pipeBU.push_back(pPipeline);

	// change the pipe.
	RpAtomicSetPipeline(pAtomic, pThis->m_pAgcmRender->IsSkinedPipe(pAtomic) ? pThis->m_atomicSkinPipeline : pThis->m_atomicPipeline);

	return pAtomic;
}

RpAtomic* AgcmShadowmap::returnToOriginalPipeline(RpAtomic* pAtomic, void* pData)
{
	AgcmShadowmap* pThis = (AgcmShadowmap*)pData;
	
	// get the original.
	RxPipeline* pPipeline;
	pPipeline = pThis->m_pipeBU.front();
	pThis->m_pipeBU.pop_front();

	// set the pipe.
	RpAtomicSetPipeline(pAtomic, pPipeline);

	return pAtomic;
}

void AgcmShadowmap::renderShadowmap()
{
	PROFILE("AgcmShadowmap::renderShadowmap");

	RpClump* pRideClump = m_pAgcmCharacter->GetSelfRideClump();
	
	if( pRideClump && pRideClump->stType.boundingSphere.radius > 2000.0f )		return;
		
	RwRGBA bgColor = { 255, 255, 255, 255 };
	RwCameraClear( m_pShadowCam, &bgColor, rwCAMERACLEARIMAGE|rwCAMERACLEARZ );

	RpClumpForAllAtomics( m_pPlayerClump, setShadowMapPipeline, (PVOID)this );

	LockFrame();
	RwCamera* pCam = RwCameraBeginUpdate(m_pShadowCam);
	if(pCam)
	{
		D3DVIEWPORT9 vp;
		((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->GetViewport(&vp);
		vp.X += 1; vp.Y += 1;
		vp.Width -= 2; vp.Height -= 2;
		((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetViewport(&vp);
		UnlockFrame();

		
		RwD3D9SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (PVOID)TRUE );
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (PVOID)TRUE );
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (PVOID)rwBLENDONE );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDZERO );
		
		RwBool bFogEnable;
		RwRenderStateGet( rwRENDERSTATEFOGENABLE, &bFogEnable );
		RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void*)FALSE );

		m_pAgcmRender->OriginalClumpRender(m_pPlayerClump);

		RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void*)bFogEnable );
		RwRenderStateSet( rwRENDERSTATECULLMODE, (PVOID)rwCULLMODECULLNONE );
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (PVOID)rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDINVSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (PVOID)FALSE );

		RwCameraEndUpdate( m_pShadowCam );
	}
	else
		UnlockFrame();

	RpClumpForAllAtomics( m_pPlayerClump, returnToOriginalPipeline, (PVOID)this );

	if( pRideClump )
	{
		RpClumpForAllAtomics(pRideClump, setShadowMapPipeline, (PVOID)this);

		// render the parent clump to the shadow map.
		LockFrame();
		RwCamera* pCam = RwCameraBeginUpdate(m_pShadowCam);
		if(pCam)
		{
			// keep borders white.
			D3DVIEWPORT9 vp;
			((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->GetViewport(&vp);
			vp.X += 1; vp.Y += 1;
			vp.Width -= 2; vp.Height -= 2;
			((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetViewport(&vp);
			UnlockFrame();

			RwD3D9SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (PVOID)TRUE);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (PVOID)TRUE);
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDONE);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDZERO);

			// disable fog.
			RwBool bFogEnable;
			RwRenderStateGet(rwRENDERSTATEFOGENABLE, &bFogEnable);
			RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);

			m_pAgcmRender->OriginalClumpRender(pRideClump);

			// restore fog state.
			RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)bFogEnable);

			RwRenderStateSet(rwRENDERSTATECULLMODE, (PVOID)rwCULLMODECULLNONE);
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDINVSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (PVOID)FALSE);

			RwCameraEndUpdate(m_pShadowCam);
		}
		else
			UnlockFrame();

		RpClumpForAllAtomics( pRideClump, returnToOriginalPipeline, (PVOID)this );
	}

	ASSERT(m_pipeBU.size() == 0);
}

void AgcmShadowmap::render()
{
	PROFILE("AgcmShadowmap::render");

	RwCamera* pMainCam = GetCamera();

	// blend shadows to the scene.
	LockFrame();
	if(RwCameraBeginUpdate(pMainCam))
	{
		UnlockFrame();
		
		RwD3D9SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (PVOID)TRUE);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDZERO);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDINVSRCCOLOR);

		RwBool bFogEnable;
		RwRenderStateGet(rwRENDERSTATEFOGENABLE, &bFogEnable);
		RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);

		for(UINT32 i=0; i<m_clumps.size(); ++i)
		{
			// set the shadow scene rendering pipeline.
			RpClumpForAllAtomics(m_clumps[i], setShadowScenePipeline, (PVOID)this);

			m_pAgcmRender->OriginalClumpRender(m_clumps[i]);

			// return to the original pipeline.
			RpClumpForAllAtomics(m_clumps[i], returnToOriginalPipeline, (PVOID)this);

			ASSERT(m_pipeBU.size() == 0);
		}

		RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)bFogEnable);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDINVSRCALPHA);

		RwD3D9SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

		RwCameraEndUpdate(pMainCam);
	}
	else 
		UnlockFrame();
}

BOOL AgcmShadowmap::registerClump(RpClump* pClump)
{
	if( !pClump )		return FALSE;

	return m_pAgcmRender->AddUpdateInfotoClump( pClump, this, updateClumpCB, releaseClumpCB, (PVOID)pClump, (PVOID)0 ) ? TRUE : FALSE;
}

BOOL AgcmShadowmap::unregisterClump(RpClump* pClump)
{
	if( !pClump )		return FALSE;
	
	return m_pAgcmRender->RemoveUpdateInfoFromClump2( pClump, this, updateClumpCB, releaseClumpCB ) ? TRUE : FALSE;
}

void AgcmShadowmap::updateShadowCam()
{
	PROFILE("AgcmShadowmap::updateShadowCam");

	LockFrame();

	// update the shadow map camera.
	// get the main directional light in the world.
	RpLight* pLight = GetDirectionalLight();	
	// get the info about a shadow caster(player character).
	RwSphere sphere = m_pPlayerClump->stType.boundingSphere;
	// transform the sphere center to the world coordinates.
	RwV3d tmp = sphere.center;
	RwV3dTransformPoints(&sphere.center, &tmp, 1, RwFrameGetLTM(RpClumpGetFrame(m_pPlayerClump)));
	
	//@{ Jaewon 20050823
	// If the player ride a horse, use its sphere.
	if(!m_pAgcmCharacter)
		m_pAgcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");

	RpClump *pRideClump = m_pAgcmCharacter->GetSelfRideClump();
	if(pRideClump)
	{
		RwSphere sphere1 = pRideClump->stType.boundingSphere;
		// Transform the sphere center to the world coordinates.
		tmp = sphere1.center;
		RwV3dTransformPoints(&sphere1.center, &tmp, 1, RwFrameGetLTM(RpClumpGetFrame(pRideClump)));

		sphere = sphere1;
	}

	sphere.radius *= 1.5f;

	// set the viewing cube.
	if(sphere.radius != m_fPrevRadius)
	{
		m_fPrevRadius = sphere.radius;
		RwCameraSetNearClipPlane(m_pShadowCam, 0.1f);
		RwCameraSetFarClipPlane(m_pShadowCam, 2.0f*sphere.radius*cDepthRange);
		RwV2d viewRect;
		viewRect.x = viewRect.y = sphere.radius;
		RwCameraSetViewWindow(m_pShadowCam, &viewRect);
	}

	// set the viewing matrix.
	RwFrame* pCamFrame;
	RwMatrix camMatrix;
	RwV3d center, translation;
	RwMatrixCopy(&camMatrix, RwFrameGetMatrix(RpLightGetFrame(pLight)));
	center = sphere.center;
	RwV3dIncrementScaled(&center, RwMatrixGetAt(&camMatrix), -sphere.radius);
	RwV3dSub(&translation, &center, RwMatrixGetPos(&camMatrix));
	RwMatrixTranslate(&camMatrix, &translation, rwCOMBINEPOSTCONCAT);
	pCamFrame = RwCameraGetFrame(m_pShadowCam);
	RwFrameTransform(pCamFrame, &camMatrix, rwCOMBINEREPLACE);

	//@{ Jaewon 20050405
	// Update m_fDepthBias based on the distance between the shadow caster & the main camera.
	RwCamera *pMainCam = GetCamera();
	RwV3d distanceVector;
	RwV3dSub(&distanceVector, RwMatrixGetPos(RwFrameGetLTM(RwCameraGetFrame(pMainCam))), &(sphere.center));
	RwReal distance = RwV3dLength(&distanceVector);
	m_fDepthBias = min(max(distance*0.00001f, 0.003f), 0.01f);
	//@} Jaewon

	UnlockFrame();
}

BOOL AgcmShadowmap::preRenderCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmShadowmap* pThis = (AgcmShadowmap*)pClass;

	if( !pThis->m_bOn || !pThis->m_bShaderSupported	)	return TRUE;
	if( !pThis->m_pPlayerClump )						return TRUE;
	
	pThis->updateShadowCam();

	return TRUE;
}

BOOL AgcmShadowmap::postRenderCB(PVOID pData, PVOID pClass, PVOID pCustData)
{	
	PROFILE("AgcmShadowmap::postRenderCB");

	AgcmShadowmap* pThis = (AgcmShadowmap*)pClass;

	if( !pThis->m_bOn || !pThis->m_bShaderSupported )	return TRUE;
	if( !pThis->m_pPlayerClump )						return TRUE;

	RwCamera* pMainCam = pThis->GetCamera();
	RwCameraEndUpdate(pMainCam);
	
	pThis->renderShadowmap();	// first, render occluders to the shadow map.
	pThis->render();			// render shadows.

	pThis->m_clumps.clear();

	pThis->LockFrame();
	RwCameraBeginUpdate( pMainCam );
	RwD3D9SetRenderState( D3DRS_FOGEND,  *((DWORD*)&(pThis->m_pAgcmRender->m_fFogEnd)) );
	pThis->UnlockFrame();

//	pThis->previewShadowmap();

	return TRUE;
}

BOOL AgcmShadowmap::updateClumpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmShadowmap::updateClumpCB");

	AgcmShadowmap* pThis = (AgcmShadowmap*)pClass;
	RpClump* pClump = (RpClump*)pData;

	if( !pThis->m_bOn || !pThis->m_bShaderSupported )	return TRUE;
	if( !pThis->m_pPlayerClump )						return TRUE;
	
	if( (pClump->stType.eType & ACUOBJECT_TYPE_INVISIBLE) || 
		(pClump->atomicList && ((RwUInt32)pClump->atomicList->stRenderInfo.backupCB3 & eSpecialRenderInvisible)) )
		return TRUE;

	RwSphere sphere = pClump->stType.boundingSphere;
	// transform the sphere center to the world coordinates.
	RwV3d tmp = sphere.center;		
	pThis->LockFrame();
	RwV3dTransformPoints( &sphere.center, &tmp, 1, RwFrameGetLTM(RpClumpGetFrame(pClump)) );
	pThis->UnlockFrame();

	if( RwCameraFrustumTestSphere(pThis->m_pShadowCam, &sphere) != rwSPHEREOUTSIDE )
		pThis->m_clumps.push_back(pClump);

	return TRUE;
}

BOOL AgcmShadowmap::releaseClumpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmShadowmap* pThis = (AgcmShadowmap*)pClass;
	RpClump* pClump = (RpClump*)pData;

	// do nothing...
	
	return TRUE;
}

RxPipeline* AgcmShadowmap::createAtomicPipeline() const
{
	RxPipeline* pPipe = RxPipelineCreate();
	if( !pPipe )	return NULL;

	RxLockedPipe* pLPipe = RxPipelineLock(pPipe);
	if( !pLPipe )
	{
		RxPipelineDestroy(pPipe);
		return NULL;
	}

	RxNodeDefinition* pInstanceNode = RxNodeDefinitionGetD3D9AtomicAllInOne();

	pLPipe = RxLockedPipeAddFragment(pLPipe, NULL, pInstanceNode, NULL);
	pLPipe = RxLockedPipeUnlock(pLPipe);

	RxPipelineNode* pNode = RxPipelineFindNodeByName(pPipe, pInstanceNode->name, NULL, NULL);

	RxD3D9AllInOneSetRenderCallBack( pNode, powerpipeRenderCB );
    RxD3D9AllInOneSetLightingCallBack( pNode, NULL );

	return pPipe;
}

void AgcmShadowmap::powerpipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags)
{
	ASSERT(_pThis->m_bShaderSupported);

	const RxD3D9ResEntryHeader* resEntryHeader = (const RxD3D9ResEntryHeader*)(repEntry + 1);
	if( resEntryHeader->indexBuffer )
		RwD3D9SetIndices( resEntryHeader->indexBuffer );

	_rwD3D9SetStreams( resEntryHeader->vertexStream, resEntryHeader->useOffsets );
	RwD3D9SetVertexDeclaration(resEntryHeader->vertexDeclaration);
    
	_rpD3D9VertexShaderDescriptor desc;
	RwInt32 numMeshes;

	_rxD3D9VertexShaderDefaultBeginCallBack(object, type, &desc);

	if( desc.morphing )			return;
	ASSERT(type == rpATOMIC);

	RwFrame* frame = RpAtomicGetFrame((RpAtomic*)object);
	_pThis->LockFrame();

	const RwMatrix* ltm = RwFrameGetLTM(frame);

	_rwD3D9VSSetActiveWorldMatrix(ltm);

	D3DXMATRIX wvpMatrix;
	_rwD3D9VSGetComposedTransformMatrix((FLOAT*)wvpMatrix);
	RwD3D9SetVertexShaderConstant(0, (FLOAT*)wvpMatrix, 4);

	// concatenate shadow mvp & tex matrices.
	_pThis->catMatrices(ltm);

	// set vertex shader constants.
	RwD3D9SetVertexShaderConstant(8, (FLOAT*)(_pThis->m_matShadowTEX), 4);
	D3DXVECTOR4 vOffset(1.0f/cShadowMapSize, 0.0f, 0.7f, 1.0f);
	RwD3D9SetVertexShaderConstant(12, (FLOAT*)vOffset, 1);
	RwD3D9SetVertexShaderConstant(4, (FLOAT*)(_pThis->m_matShadowMVP), 4);

	D3DXVECTOR4 vRange(-0.1f, -0.01f, 255.0f, -_pThis->m_fDepthBias);

	RwD3D9SetVertexShaderConstant(14, (FLOAT*)vRange, 1);
	RwV4d ldir;
	RwV3d* at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(_pThis->GetDirectionalLight())));
    _rwD3D9VSGetNormalInLocalSpace(at, (RwV3d*)&ldir);
	ldir.w = 0.0f;
	RwD3D9SetVertexShaderConstant(13, (FLOAT*)&ldir, 1);

	_pThis->UnlockFrame();

	// set pixel shader constants.

	// set the vertex shader.
	ASSERT(desc.numWeights==0);
	RwD3D9SetVertexShader(_pThis->m_pShadowSceneVShader[0]);
	
	// set the pixel shader.
	RwD3D9SetPixelShader(_pThis->m_pShadowScenePShader);

	// set a shadow map texture.
	RwD3D9SetTexture(_pThis->m_pShadowMapTexture, 1);
	RwD3D9SetTexture(_pThis->m_pShadowMapTexture, 2);
	RwD3D9SetTexture(_pThis->m_pShadowMapTexture, 3);

	if(_pThis->m_bPs14Supported)
		RwD3D9SetTexture(_pThis->m_pShadowMapTexture, 0);

	// So here is a stopgap measure. --;
	((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	const RxD3D9InstanceData* instancedData = (RxD3D9InstanceData*)(resEntryHeader + 1);
	numMeshes = resEntryHeader->numMeshes;
	while( numMeshes-- )
	{
		RwD3D9SetTexture( instancedData->material->texture, _pThis->m_bPs14Supported ? 3 : 0 );

		if( resEntryHeader->indexBuffer )
			RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType, instancedData->baseIndex, 0, instancedData->numVertices, instancedData->startIndex, instancedData->numPrimitives);
		else
			RwD3D9DrawPrimitive(( D3DPRIMITIVETYPE)resEntryHeader->primType, instancedData->baseIndex, instancedData->numPrimitives );

		instancedData++;		/* Move onto the next instancedData */
	}

	RwD3D9SetPixelShader( 0 );

	RwD3D9SetTexture( NULL, 1 );
	RwD3D9SetTexture( NULL, 2 );
	RwD3D9SetTexture( NULL, 3 );

	if( _pThis->m_bPs14Supported )
	{
		RwD3D9SetTexture(NULL, 4);
		RwD3D9SetTexture(NULL, 5);
	}
}

RxPipeline* AgcmShadowmap::createAtomicSkinPipeline() const
{
	RxPipeline* pPipe = RxPipelineCreate();
	if( !pPipe )	return NULL;

	RxLockedPipe* pLPipe = RxPipelineLock(pPipe);
	if( !pLPipe )
	{
		RxPipelineDestroy(pPipe);
		return NULL;
	}

	RxNodeDefinition* pInstanceNode = RxNodeDefinitionGetD3D9SkinAtomicAllInOne();

	pLPipe = RxLockedPipeAddFragment(pLPipe, NULL, pInstanceNode, NULL);
	pLPipe = RxLockedPipeUnlock(pLPipe);

	RxPipelineNode* pNode = RxPipelineFindNodeByName(pPipe, pInstanceNode->name, NULL, NULL);

    _rxD3D9SkinVertexShaderSetLightingCallBack( pNode, powerpipeSkinLightingCB );
	_rxD3D9SkinVertexShaderSetGetMaterialShaderCallBack( pNode, powerpipeSkinGetMaterialShaderCB );
	_rxD3D9SkinVertexShaderSetMeshRenderCallBack( pNode, powerpipeSkinMeshRenderCB );

	((_rxD3D9SkinInstanceNodeData *)pNode->privateData)->lightingCallback	= NULL;
	((_rxD3D9SkinInstanceNodeData *)pNode->privateData)->renderCallback		= powerpipeRenderCB;

	return pPipe;
}

RwV4d* AgcmShadowmap::powerpipeSkinLightingCB( void* object, RwUInt32 type, RwV4d* shaderConstantPtr, _rpD3D9VertexShaderDescriptor* desc)
{
	ASSERT(type == rpATOMIC);

	// concatenate shadow mvp & tex matrices.
	RwMatrix identity, ltm;
	RwMatrixSetIdentity( &identity );
	_rwD3D9VSGetWorldMultiplyMatrix( &ltm, &identity );
	_pThis->catMatrices( &ltm );

	// need no light.
	desc->numDirectionalLights	= 0;
	desc->numPointLights		= 0;
	desc->numSpotLights			= 0;

	return shaderConstantPtr;
}

void* AgcmShadowmap::powerpipeSkinGetMaterialShaderCB(const RpMaterial* material, _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch)
{
	ASSERT( material );
	ASSERT( desc );
	ASSERT( dispatch );
	ASSERT(desc->numWeights>0);

	dispatch->offsetBoneMatrices	= 15;
	dispatch->offsetEffect			= _rwD3D9VertexShaderMaxConstants-1;
	dispatch->offsetFogRange		= _rwD3D9VertexShaderMaxConstants-1;
	dispatch->offsetMaterialColor	= _rwD3D9VertexShaderMaxConstants-1;
	dispatch->offsetMorphingCoef	= _rwD3D9VertexShaderMaxConstants-1;

	return _pThis->m_pShadowSceneVShader[desc->numWeights];
}

void AgcmShadowmap::powerpipeSkinMeshRenderCB(RxD3D9ResEntryHeader* resEntryHeader, RxD3D9InstanceData* instancedMesh, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch)
{
	ASSERT(_pThis->m_bShaderSupported);
	if( desc->morphing )			return;

	// renderware bug...--;
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// set vertex shader constants.
	RwD3D9SetVertexShaderConstant(8, (FLOAT*)(_pThis->m_matShadowTEX), 4);
	D3DXVECTOR4 vOffset(1.0f/cShadowMapSize, 0.0f, 0.7f, 1.0f);
	RwD3D9SetVertexShaderConstant(12, (FLOAT*)vOffset, 1);
	RwD3D9SetVertexShaderConstant(4, (FLOAT*)(_pThis->m_matShadowMVP), 4);

	D3DXVECTOR4 vRange(-0.1f, -0.01f, 255.0f, -_pThis->m_fDepthBias);
	RwD3D9SetVertexShaderConstant(14, (FLOAT*)vRange, 1);
	RwV4d ldir;
	RwV3d* at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(_pThis->GetDirectionalLight())));
    _rwD3D9VSGetNormalInLocalSpace(at, (RwV3d*)&ldir);
	ldir.w = 0.0f;
	RwD3D9SetVertexShaderConstant(13, (FLOAT*)&ldir, 1);

	RwD3D9SetVertexShader( instancedMesh->vertexShader );
	RwD3D9SetPixelShader( _pThis->m_pShadowScenePShader );

	RwD3D9SetTexture( instancedMesh->material->texture, 0 );

	// set a shadow map texture.
	RwD3D9SetTexture( _pThis->m_pShadowMapTexture, 1 );
	RwD3D9SetTexture( _pThis->m_pShadowMapTexture, 2 );
	RwD3D9SetTexture( _pThis->m_pShadowMapTexture, 3 );

	if(_pThis->m_bPs14Supported)
	{
		RwD3D9SetTexture( _pThis->m_pShadowMapTexture, 0 );
		RwD3D9SetTexture( instancedMesh->material->texture, 3 );
	}

	//@{ Jaewon 20051011
	// There is a render state problem in shadow map + high quality water situation...
	// So here is a stopgap measure. --;
	((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	//@} Jaewon

	if( resEntryHeader->indexBuffer )
	{
		RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
									instancedMesh->baseIndex,
									0, instancedMesh->numVertices,
									instancedMesh->startIndex, instancedMesh->numPrimitives);
	}
	else
	{
		RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
							instancedMesh->baseIndex,
							instancedMesh->numPrimitives);
	}

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D9SetPixelShader(0);

	RwD3D9SetTexture( NULL, 1 );
	RwD3D9SetTexture( NULL, 2 );
	RwD3D9SetTexture( NULL, 3 );

	if( _pThis->m_bPs14Supported )
	{
		RwD3D9SetTexture( NULL, 4 );
		RwD3D9SetTexture( NULL, 5 );
	}
}

RxPipeline* AgcmShadowmap::createAtomicPipelineForShadowmap() const
{
	RxPipeline* pPipe = RxPipelineCreate();
	if( !pPipe )	return NULL;

	RxLockedPipe* pLPipe = RxPipelineLock(pPipe);
	if( !pLPipe )
	{
		RxPipelineDestroy(pPipe);
		return NULL;
	}

	RxNodeDefinition* pInstanceNode = RxNodeDefinitionGetD3D9AtomicAllInOne();

	pLPipe = RxLockedPipeAddFragment(pLPipe, NULL, pInstanceNode, NULL);
	pLPipe = RxLockedPipeUnlock(pLPipe);

	RxPipelineNode* pNode = RxPipelineFindNodeByName(pPipe, pInstanceNode->name, NULL, NULL);

	RxD3D9AllInOneSetRenderCallBack(pNode, powerpipeRenderForShadowmapCB);
    RxD3D9AllInOneSetLightingCallBack(pNode, NULL);

	return pPipe;
}

void AgcmShadowmap::powerpipeRenderForShadowmapCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags)
{
	const RxD3D9ResEntryHeader* resEntryHeader;
	const RxD3D9InstanceData* instancedData;

	ASSERT(_pThis->m_bShaderSupported);

	resEntryHeader = (const RxD3D9ResEntryHeader*)(repEntry + 1);

	/*
	 * Set Indices
	 */
	if(resEntryHeader->indexBuffer != NULL)
	{
		RwD3D9SetIndices(resEntryHeader->indexBuffer);
	}

	/* Set the stream sources */
	_rwD3D9SetStreams(resEntryHeader->vertexStream,
						resEntryHeader->useOffsets);

	/*
	 * Vertex Declaration
	 */
	RwD3D9SetVertexDeclaration(resEntryHeader->vertexDeclaration);
    
	_rpD3D9VertexShaderDescriptor desc;

	RwInt32 numMeshes;

	/* Initialize descriptor */
	_rxD3D9VertexShaderDefaultBeginCallBack(object, type, &desc);

	// a morphing geometry is not supported currently.
	if(desc.morphing) 
		return;

	ASSERT(type == rpATOMIC);

	// set matrices for later scene rendering.
	_rwD3D9VSSetActiveWorldMatrix(NULL);
	_rwD3D9VSGetComposedTransformMatrix((FLOAT*)(_pThis->m_matShadowVP));

	RwFrame* frame;
	const RwMatrix* ltm;
	frame = RpAtomicGetFrame((RpAtomic*)object);

	_pThis->LockFrame();
	ltm = RwFrameGetLTM(frame);
	_pThis->UnlockFrame();

	_rwD3D9VSSetActiveWorldMatrix(ltm);

	D3DXMATRIX wvpMatrix;
	_rwD3D9VSGetComposedTransformMatrix((FLOAT*)wvpMatrix);
	
	// set vertex shader constants.
	RwD3D9SetVertexShaderConstant(0, (FLOAT*)wvpMatrix, 4);
	D3DXVECTOR4 vRange(1.0f/*/cDepthRange*/, 0.0f, 255.0f, 0.00392f);
	RwD3D9SetVertexShaderConstant(4, (FLOAT*)vRange, 1);

	// set pixel shader constants.

	// set the vertex shader.
	ASSERT(desc.numWeights==0);
	RwD3D9SetVertexShader(_pThis->m_pShadowMapVShader[0]);
	
	// set the pixel shader.
	RwD3D9SetPixelShader(_pThis->m_pShadowMapPShader);

	//@{ Jaewon 20050118
	if(_pThis->m_bPs14Supported)
	// set texture unit 1 to the 11-bit encoding map.
	{
		RwD3D9SetTexture(_pThis->m_p11bitEncodingTexture, 1);
	}
	//@} Jaewon

	/* Get the instanced data */
	instancedData = (RxD3D9InstanceData*)(resEntryHeader + 1);

	numMeshes = resEntryHeader->numMeshes;
	while(numMeshes--)
	{
		//const RwRGBA *materialcolor;

		//materialcolor = &instancedData->material->color;
		
		// a translucent geometry is not supported currently.
		//if(instancedData->vertexAlpha || (0xFF != materialcolor->alpha))
		//	continue;
 
		// set a base texture.
		RwD3D9SetTexture(instancedData->material->texture, 0);

		//@{ Jaewon 20050823
		// ;)
		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		//@} Jaewon

		/*
		 * Render
		 */
		if(resEntryHeader->indexBuffer != NULL)
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
		++instancedData;
	}

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D9SetPixelShader(0);

	//@{ Jaewon 20050118
	if(_pThis->m_bPs14Supported)
	{
		RwD3D9SetTexture(NULL, 1);
	}
	//@} Jaewon

    return;
}

RxPipeline* AgcmShadowmap::createAtomicSkinPipelineForShadowmap() const
{
	RxPipeline* pPipe;
	
	pPipe = RxPipelineCreate();
	if(pPipe)
	{
		RxLockedPipe* pLPipe;

		pLPipe = RxPipelineLock(pPipe);
		if(NULL != pLPipe)
		{
			RxNodeDefinition* pInstanceNode;
			RxPipelineNode* pNode;

			/*
			 * Get the instance node definition
			 */
			pInstanceNode = RxNodeDefinitionGetD3D9SkinAtomicAllInOne();

			/*
			 * Add the node to the pipeline
			 */
			pLPipe = RxLockedPipeAddFragment(pLPipe, NULL, pInstanceNode, NULL);

			/*
			 * Unlock the pipeline
			 */
			pLPipe = RxLockedPipeUnlock(pLPipe);

			/*
			 * Set the pipeline specific data
			 */
			pNode = RxPipelineFindNodeByName(pPipe, pInstanceNode->name, NULL, NULL);

			/*
			 * Set the lighting callback
			 */
            _rxD3D9SkinVertexShaderSetLightingCallBack(pNode, powerpipeSkinLightingForShadowmapCB);

			/*
			 * Set the GetMaterialShader callback
			 */
			_rxD3D9SkinVertexShaderSetGetMaterialShaderCallBack(pNode, powerpipeSkinGetMaterialShaderForShadowmapCB);


			/*
			 * Set the render callback
			 */
			_rxD3D9SkinVertexShaderSetMeshRenderCallBack(pNode, powerpipeSkinMeshRenderForShadowmapCB);

			//@{ Jaewon 20040706
			// callbacks for the skin FFP pipeline 
			((_rxD3D9SkinInstanceNodeData *)pNode->privateData)->lightingCallback = NULL;
			((_rxD3D9SkinInstanceNodeData *)pNode->privateData)->renderCallback = powerpipeRenderForShadowmapCB;
			//@} Jaewo

			return pPipe;
		}

		RxPipelineDestroy(pPipe);
	}

	return NULL;
}

RwV4d* AgcmShadowmap::powerpipeSkinLightingForShadowmapCB(void* object, RwUInt32 type,
														RwV4d* shaderConstantPtr,
														_rpD3D9VertexShaderDescriptor* desc)
{
	ASSERT(type == rpATOMIC);
	
	// set matrices for later scene rendering.
	_rwD3D9VSSetActiveWorldMatrix(NULL);
	_rwD3D9VSGetComposedTransformMatrix((FLOAT*)(_pThis->m_matShadowVP));

	// need no light.
	desc->numDirectionalLights = 0;
	desc->numPointLights = 0;
	desc->numSpotLights = 0;

	return shaderConstantPtr;
}

void* AgcmShadowmap::powerpipeSkinGetMaterialShaderForShadowmapCB(const RpMaterial* material,
																_rpD3D9VertexShaderDescriptor* desc,
																_rpD3D9VertexShaderDispatchDescriptor* dispatch)
{
	ASSERT(material != NULL);
	ASSERT(desc != NULL);
	ASSERT(dispatch != NULL);

	// adjust constant offsets.
	dispatch->offsetBoneMatrices = 15;
	// following constants are unnecessary.
	dispatch->offsetEffect = _rwD3D9VertexShaderMaxConstants-1;
	dispatch->offsetFogRange = _rwD3D9VertexShaderMaxConstants-1;
	dispatch->offsetMaterialColor = _rwD3D9VertexShaderMaxConstants-1;
	dispatch->offsetMorphingCoef = _rwD3D9VertexShaderMaxConstants-1;

	// return the pre-created shader.
	ASSERT(desc->numWeights>0);
	return _pThis->m_pShadowMapVShader[desc->numWeights];
}

void AgcmShadowmap::powerpipeSkinMeshRenderForShadowmapCB(RxD3D9ResEntryHeader* resEntryHeader,
														RxD3D9InstanceData* instancedMesh,
														const _rpD3D9VertexShaderDescriptor* desc,
														const _rpD3D9VertexShaderDispatchDescriptor* dispatch)
{
	ASSERT(_pThis->m_bShaderSupported);

	// a morphing geometry is not supported currently.
	if(desc->morphing) 
		return;

	//const RwRGBA* materialcolor = &instancedMesh->material->color;

	// a translucent geometry is not supported currently.
	//if(instancedMesh->vertexAlpha || (0xFF != materialcolor->alpha))
	//	return;

	// set vertex shader constants.
	D3DXVECTOR4 vRange(1.0f/*/cDepthRange*/, 0.0f, 255.0f, 0.00392f);
	RwD3D9SetVertexShaderConstant(4, (FLOAT*)vRange, 1);

	// set pixel shader constants.

	// set the vertex shader.
	RwD3D9SetVertexShader(instancedMesh->vertexShader);
	
	// set the pixel shader.
	RwD3D9SetPixelShader(_pThis->m_pShadowMapPShader);

	// set a base texture.
	RwD3D9SetTexture(instancedMesh->material->texture, 0);

	//@{ Jaewon 20050118
	if(_pThis->m_bPs14Supported)
	// set texture unit 1 to the 11-bit encoding map.
	{
		RwD3D9SetTexture(_pThis->m_p11bitEncodingTexture, 1);
	}
	//@} Jaewon

	//@{ Jaewon 20050823
	//TODO: Why should I reset the texture addressing?
	RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	//@} Jaewon

	if(resEntryHeader->indexBuffer != NULL)
	{
		RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
									instancedMesh->baseIndex,
									0, instancedMesh->numVertices,
									instancedMesh->startIndex, instancedMesh->numPrimitives);
	}
	else
	{
		RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
							instancedMesh->baseIndex,
							instancedMesh->numPrimitives);
	}

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D9SetPixelShader(0);

	//@{ Jaewon 20050118
	if(_pThis->m_bPs14Supported)
	{
		RwD3D9SetTexture(NULL, 1);
	}
	//@} Jaewon

	return;
}

void AgcmShadowmap::catMatrices(const RwMatrix* ltm)
{
	D3DMATRIX mTranspose;
    mTranspose.m[0][0] = ltm->right.x;
    mTranspose.m[1][0] = ltm->right.y;
    mTranspose.m[2][0] = ltm->right.z;
	mTranspose.m[3][0] = 0.0f;

    mTranspose.m[0][1] = ltm->up.x;
    mTranspose.m[1][1] = ltm->up.y;
    mTranspose.m[2][1] = ltm->up.z;
	mTranspose.m[3][1] = 0.0f;

    mTranspose.m[0][2] = ltm->at.x;
    mTranspose.m[1][2] = ltm->at.y;
    mTranspose.m[2][2] = ltm->at.z;
	mTranspose.m[3][2] = 0.0f;

    mTranspose.m[0][3] = ltm->pos.x;
    mTranspose.m[1][3] = ltm->pos.y;
    mTranspose.m[2][3] = ltm->pos.z;
	mTranspose.m[3][3] = 1.0f;
	FLOAT fTexelOffs = (0.5f / cShadowMapSize);
	D3DXMATRIX matTexAdjTranspose(0.5f,      0.0f,        0.0f,        0.5f + fTexelOffs,
								  0.0f,     -0.5f,        0.0f,        0.5f + fTexelOffs,
								  0.0f,      0.0f,        0.0f,        1.0f,
								  0.0f,		 0.0f,		  0.0f,        1.0f);
	D3DXMATRIX mTX(mTranspose);
	D3DXMatrixMultiply(&m_matShadowMVP, &m_matShadowVP, &mTX);
	D3DXMatrixMultiply(&m_matShadowTEX, &matTexAdjTranspose, &m_matShadowMVP);
}

RwBool AgcmShadowmap::Im2DRenderQuad(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT z, FLOAT recipCamZ, FLOAT uvOffset) const
{
	RwIm2DVertex vTriFan[4];

	ZeroMemory(vTriFan, sizeof(RwIm2DVertex) * 4);

	RwIm2DVertexSetIntRGBA(&vTriFan[0], 255, 255, 255, 255);
	RwIm2DVertexSetIntRGBA(&vTriFan[1], 255, 255, 255, 255);
	RwIm2DVertexSetIntRGBA(&vTriFan[2], 255, 255, 255, 255);
	RwIm2DVertexSetIntRGBA(&vTriFan[3], 255, 255, 255, 255);

	/*
	 *  Render an opaque white 2D quad at the given coordinates and
	 *  spanning a whole texture.
	 */

	RwIm2DVertexSetScreenX(&vTriFan[0], x2);
	RwIm2DVertexSetScreenY(&vTriFan[0], y2);
	RwIm2DVertexSetScreenZ(&vTriFan[0], z);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[0], recipCamZ);
	RwIm2DVertexSetU(&vTriFan[0], 1.0f + uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vTriFan[0], 1.0f + uvOffset, recipCamZ);

	RwIm2DVertexSetScreenX(&vTriFan[1], x2);
	RwIm2DVertexSetScreenY(&vTriFan[1], y1);
	RwIm2DVertexSetScreenZ(&vTriFan[1], z);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[1], recipCamZ);
	RwIm2DVertexSetU(&vTriFan[1], 1.0f + uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vTriFan[1], uvOffset, recipCamZ);

	RwIm2DVertexSetScreenX(&vTriFan[2], x1);
	RwIm2DVertexSetScreenY(&vTriFan[2], y2);
	RwIm2DVertexSetScreenZ(&vTriFan[2], z);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[2], recipCamZ);
	RwIm2DVertexSetU(&vTriFan[2], uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vTriFan[2], 1.0f + uvOffset, recipCamZ);

	RwIm2DVertexSetScreenX(&vTriFan[3], x1);
	RwIm2DVertexSetScreenY(&vTriFan[3], y1);
	RwIm2DVertexSetScreenZ(&vTriFan[3], z);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[3], recipCamZ);
	RwIm2DVertexSetU(&vTriFan[3], uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vTriFan[3], uvOffset, recipCamZ);

	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vTriFan, 4);

	return TRUE;
}

void AgcmShadowmap::previewShadowmap()
{
	RwV2d vx[2] = { {0.53f, 0.02f}, {0.98f, 0.5f} };

	RwCamera* pMainCam = GetCamera();
	RwRaster* camRas = RwCameraGetRaster(pMainCam);

	FLOAT crw = (FLOAT)RwRasterGetWidth(camRas);
	FLOAT crh = (FLOAT)RwRasterGetHeight(camRas);
	FLOAT recipCamZ = 1.0f / RwCameraGetNearClipPlane(pMainCam);

	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDZERO);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (PVOID)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (PVOID)m_pShadowMap);

	Im2DRenderQuad( vx[0].x * crw, vx[0].y * crh, vx[0].x * crw + 256.0f, vx[0].y * crh + 256.0f, RwIm2DGetNearScreenZ(), recipCamZ, 0.0f );
	
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDSRCALPHA);
}

// Create a 11-bit encoding texture for the ps.1.4 version.
RwTexture *AgcmShadowmap::create11BitEncodingMap()
{
	RwRaster* pRaster = RwRasterCreate( 2048, 1, 32, rwRASTERTYPETEXTURE|rwRASTERFORMAT8888 );
	if( !pRaster )		return NULL;

	// Fill the raster.
	UINT8* pPixels = (UINT8*)RwRasterLock(pRaster, 0, rwRASTERLOCKWRITE);
	UINT32 stride = RwRasterGetStride(pRaster);

	// Map 11 bits into 2 channels.
	for(INT32 i=0; i<2048; ++i)
	{
		*(pPixels+4*i+2) = i & 0xFF;
		*(pPixels+4*i+1) = (i & 0xFF00) >> 3;
		*(pPixels+4*i+0) = 0;
		*(pPixels+4*i+3) = 0;
	}

	RwRasterUnlock(pRaster);

	// Create a texture using the above raster.
	return RwTextureCreate(pRaster);
}

// Create a subtexel precision texture for the ps.1.4 version.
RwTexture *AgcmShadowmap::createSubTexelPrecisionMap()
{
	RwRaster* pRaster = RwRasterCreate( cShadowMapSize*4, 1, 8, rwRASTERTYPETEXTURE|rwRASTERFORMATLUM8 );
	if( !pRaster )		return NULL;

	// Fill the raster.
	UINT8* pPixels = (UINT8*)RwRasterLock(pRaster, 0, rwRASTERLOCKWRITE);
	UINT32 stride = RwRasterGetStride(pRaster);

	// Map 11 bits into 2 channels.
	for(INT32 i=0; i<cShadowMapSize/8; ++i)
	{
		for(INT32 j=0; j<4*8; ++j)
		{
			*(pPixels+(4*8*i+j)) = 8*j+4;
		}
	}

	RwRasterUnlock(pRaster);

	// Create a texture using the above raster.
	return RwTextureCreate(pRaster);
}
