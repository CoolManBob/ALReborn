//@{ Jaewon 20040730
// created
// Image-space post-processing effects module.
//@} Jaewon

#include "AgcmPostFX.h"
//@{ Jaewon 20050217
#include "AgcmUIConsole.h"
//@} Jaewon
//@{ Jaewon 20050401
#include "AgcmRender.h"
//@} Jaewon
#include <ctime>
#include <stdlib.h>
#include <rtfsyst.h>

//@{ Jaewon 20040922
#include "AuMD5Encrypt.h"

#include "ApMemoryTracker.h"

#define HASH_KEY_STRING "1111"
//@} Jaeown

//@{ Jaewon 20041126
// StretchRect can be used to support AA in post fx.
#define POSTFX_USE_STRETCH_RECT
//@} Jaewon

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

//@{ Jaewon 20040811
// d3d device lost & reset
static AgcmPostFX *_pThis = NULL;
static rwD3D9DeviceRestoreCallBack _oldD3D9RestoreDeviceCB = NULL;
static rwD3D9DeviceReleaseCallBack _oldD3D9ReleaseDeviceCB = NULL;
static void newD3D9ReleaseDeviceCB()
{
	_pThis->onLostDevice();

	if(_oldD3D9ReleaseDeviceCB)
		_oldD3D9ReleaseDeviceCB();
}
static void newD3D9RestoreDeviceCB()
{
	if(_oldD3D9RestoreDeviceCB)
		_oldD3D9RestoreDeviceCB();

	_pThis->onResetDevice();
}
//@} Jaewon

static const float _fMaxExposure = 3.5f;
static const float _fMinExposure = 1.5f;

// renderware hacks
static RwInt32 _RwD3D9RasterExtOffset = 0;

#define RASTEREXTFROMRASTER(raster) \
    ((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

struct _rwD3D9Palette
{
    PALETTEENTRY    entries[256];
    RwInt32     globalindex;
};

struct _rwD3D9RasterExt
{
    LPDIRECT3DTEXTURE9      texture;
    _rwD3D9Palette          *palette;
    RwUInt8                 alpha;              /* This texture has alpha */
    RwUInt8                 cube : 4;           /* This texture is a cube texture */
    RwUInt8                 face : 4;           /* The active face of a cube texture */
    RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
    RwUInt8                 compressed : 4;     /* This texture is compressed */
    RwUInt8                 lockedMipLevel;
    LPDIRECT3DSURFACE9      lockedSurface;
    D3DLOCKED_RECT          lockedRect;
    D3DFORMAT               d3dFormat;          /* D3D format */
    LPDIRECT3DSWAPCHAIN9    swapChain;
    HWND                    window;
};

extern "C"
{
extern LPDIRECT3DSURFACE9           _RwD3D9RenderSurface;

extern RwBool
_rwD3D9SetRenderTarget(RwUInt32 index,
                       LPDIRECT3DSURFACE9 rendertarget);
}

// vertex format for rendering a quad during post-process
struct PFXVERTEX
{
	float x, y, z;
	float u, v;
	
	const static D3DVERTEXELEMENT9 Decl[3];
};

// vertex declaration for post-processing
const D3DVERTEXELEMENT9 PFXVERTEX::Decl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

AgcmPostFX::Technique::Technique()
				: m_hTechnique(NULL),
				//@{ Jaewon 20041011
				m_bDepedentRead(NULL)
				//@} Jaewon

{
	memset(m_name, 0, sizeof(m_name));
	memset(m_szParamName, 0, sizeof(m_szParamName));
	memset(m_hParam, 0, sizeof(m_hParam));
	memset(m_nParamSize, 0, sizeof(m_nParamSize));
	memset(m_vParamDef, 0, sizeof(m_vParamDef));
}

AgcmPostFX::Technique::~Technique()
{
}

void AgcmPostFX::Technique::init(LPD3DXEFFECT pFX, D3DXHANDLE hTechnique)
{
	// get the post fx technique handle.
	m_hTechnique = hTechnique;

	// get the technique description.
	D3DXTECHNIQUE_DESC pDesc;
	if(FAILED(pFX->GetTechniqueDesc(m_hTechnique, &pDesc)))
	{
		ASSERT(0);
	}
	strcpy(m_name, pDesc.Name);

	char buf[MAX_PATH];

	// obtain the handles to the changeable parameters, if any.
	for(int i=0; i<NUM_PARAMS; ++i)
	{
		sprintf(buf, "Parameter%d", i);
		D3DXHANDLE hAnno = pFX->GetAnnotationByName(m_hTechnique, buf);
		LPCSTR szParamName;
		if(hAnno && SUCCEEDED(pFX->GetString(hAnno, &szParamName)))
		{
			m_hParam[i] = pFX->GetParameterByName(NULL, szParamName);
			strcpy(m_szParamName[i], szParamName);
		}

		// get the parameter size.
		sprintf(buf, "Parameter%dSize", i);
		hAnno = pFX->GetAnnotationByName(m_hTechnique, buf);
		if(hAnno)
			pFX->GetInt(hAnno, &m_nParamSize[i]);

		// get the parameter default.
		sprintf(buf, "Parameter%dDef", i);
		hAnno = pFX->GetAnnotationByName(m_hTechnique, buf);
		if(hAnno)
			pFX->GetVector(hAnno, &m_vParamDef[i]);
	}

	//@{ Jaewon 20041011
	// check the dependent read.
	D3DXHANDLE hAnno = pFX->GetAnnotationByName(m_hTechnique, "bDependentRead");
	BOOL bDependentRead;
	if(hAnno && SUCCEEDED(pFX->GetBool(hAnno, &bDependentRead)))
	{
		if(bDependentRead)
		{
			m_bDepedentRead = pFX->GetTechniqueByName("Copy");
			ASSERT(m_bDepedentRead);
		}
		else
			m_bDepedentRead = NULL;
	}
	//@} Jaewon
}

AgcmPostFX::RenderTargetChain::RenderTargetChain()
				: m_nNext(0)//, m_bFirstRender(true),
				//m_bLastRender(false)
{
	memset(m_pRenderTarget, 0, sizeof(m_pRenderTarget));
}

AgcmPostFX::RenderTargetChain::~RenderTargetChain()
{
	for(int i=0; i<2; ++i)
	{
		if(m_pRenderTarget[i])
		{
			RwRasterDestroy(m_pRenderTarget[i]);
			m_pRenderTarget[i] = NULL;
		}
	}
}

bool AgcmPostFX::RenderTargetChain::init(int w, int h)
{
	ASSERT(w>0 && h>0);
	ASSERT(m_pRenderTarget[0] == NULL);
	ASSERT(m_pRenderTarget[1] == NULL);

	for(int i=0; i<2; ++i)
	{
		m_pRenderTarget[i] = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
		if(m_pRenderTarget[i] == NULL)
			return false;
	}

	return true;
}

void AgcmPostFX::RenderTargetChain::flip()
{
	m_nNext = 1 - m_nNext;
}
		
RwRaster* AgcmPostFX::RenderTargetChain::target() const
{
	return m_pRenderTarget[m_nNext];
}

RwRaster* AgcmPostFX::RenderTargetChain::source() const
{
	return m_pRenderTarget[1-m_nNext];
}

AgcmPostFX::AgcmPostFX()
		: m_pSceneRenderTarget(NULL), m_pOriginalRenderTarget(NULL),
		m_bOn(FALSE), m_pd3dEffect(NULL), m_hTexScene(NULL),
		m_pVertexDecl(NULL), m_pVertexBuffer(NULL), m_bValid(FALSE),
		m_hWindowSize(NULL), m_hElapsedTime(NULL),
		m_pAveragingTechnic(NULL), m_pExposureControlTechnic(NULL),
		//@{ Jaewon 20040921
		m_hTexCurrentExposure(NULL), m_hTexPrev(NULL),
		m_hRadius(NULL), m_hCenter(NULL),
		//@} Jaewon
		//@{ Jaewon 20041011
		m_pSquareRenderTarget(NULL), m_hTexSquare(NULL),
		m_hSquareRatio(NULL),
		//@} Jaewon
		//@{ Jaewon 20041014
		m_pZBuffer(NULL), m_pOriginalZBuffer(NULL),
		//@} Jaewon
		//@{ Jaewon 20041122
		m_bDoNotCreate(true),
		//@} Jaewon
		//@{ Jaewon 20050223
		m_bSaveNextFrame(false), m_hTexTransSource(NULL),
		//@} Jaewon
		//@{ Jaewon 20050304
		m_pTransitionSource(NULL),
		//@} Jaewon
		//@{ Jaewon 20050401
		m_pAgcmRender(NULL)
		//@} Jaewon
{
	memset(m_pAverageRenderTarget, 0, sizeof(m_pAverageRenderTarget));

	SetModuleName("AgcmPostFX");

	//@{ Jaewon 2004081
	// for static callbacks
	ASSERT(_pThis==NULL);
	_pThis = this;
	//@} Jaewon
}

AgcmPostFX::~AgcmPostFX()
{
	OnDestroy();
}


BOOL AgcmPostFX::OnAddModule()
{
	_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);

	//@{ Jaewon 20040811
	// save original device lost & reset callbacks.
    _oldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	_oldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();
	// set new callbacks.
	_rwD3D9DeviceSetReleaseCallback(newD3D9ReleaseDeviceCB);
    _rwD3D9DeviceSetRestoreCallback(newD3D9RestoreDeviceCB);
	//@} Jaewon

	return TRUE;
}

void AgcmPostFX::On() 
{ 
	if(m_bValid && !m_bOn)
	{
		//@{ Jaewon 20050106
		// if not already created, create offscreen buffers.
		if(isReady() == false)
			createRenderTargets();
		//@} Jaewon

		//@{ Jaewon 20050131
		// createRenderTargets() might fail, so...
		if(!m_bValid)
		{
			ASSERT(0);
			return;
		}
		//@} Jaewon

		// set the raster of the main camera to m_pSceneRenderTarget.
		RwCamera* pMainCam = GetCamera();
		if(pMainCam == NULL)
		{
			ASSERT(0);
			return;
		}
		m_pOriginalRenderTarget = RwCameraGetRaster(pMainCam);
		//@{ Jaewon 20041013
		m_pOriginalZBuffer = RwCameraGetZRaster(pMainCam);
		//@} Jaewon
		if(m_pSceneRenderTarget == NULL)
		{
			ASSERT(0);
			return;
		}
#ifndef POSTFX_USE_STRETCH_RECT
		//@{ Jaewon 20041014
		if(m_pZBuffer == NULL)
		{
			ASSERT(0);
			return;
		}
		//@} Jaewon
		RwCameraSetRaster(pMainCam, m_pSceneRenderTarget);
		//@{ Jaewon 20041014
		RwCameraSetZRaster(pMainCam, m_pZBuffer);
		//@} Jaewon
#endif
		
		m_bOn = TRUE; 
	}
}

void AgcmPostFX::Off(bool doNotDestroyTargets) 
{ 
	if(m_bValid && m_bOn)
	{
		// restore the raster of the main camera.
		RwCamera* pMainCam = GetCamera();
		if(pMainCam == NULL)
		{
			ASSERT(0);
			return;
		}
		RwRaster* pRaster = RwCameraGetRaster(pMainCam);
		//ASSERT(pRaster==m_pSceneRenderTarget);
		RwCameraSetRaster(pMainCam, m_pOriginalRenderTarget);
		m_pOriginalRenderTarget = NULL;
		//@{ Jaewon 20041014
		RwCameraSetZRaster(pMainCam, m_pOriginalZBuffer);
		//@} Jaewon
		//@{ Jaewon 20041013
		m_pOriginalZBuffer = NULL;
		//@} Jaewon

		m_bOn = FALSE; 

		//@{ Jaewon 20050106
		// destroy offscreen buffers.
		if(!doNotDestroyTargets && isReady() == true)
			destroyRenderTargets();
		//@} Jaewon
	}
}

BOOL AgcmPostFX::OnInit()
{
	//@{ Jaewon 20050401
	m_pAgcmRender = (AgcmRender*)GetModule("AgcmRender");
	if(!m_pAgcmRender)
	{
		ASSERT(!"Can't get the AgcmRender module in AgcmPostFX::OnInit()!");
		return FALSE;
	}
	//@} Jaewon

	//@{ Jaewon 20050217
	// Register type & members to the script engine.
	/*AgcmUIConsole *pAgcmUIConsole = (AgcmUIConsole*)GetModule("AgcmUIConsole");
	if(pAgcmUIConsole)
	{
		bool result;
		result = pAgcmUIConsole->getScriptEngine().registerObjectType("AgcmPostFX", sizeof(AgcmPostFX));
		ASSERT(result);
		result = pAgcmUIConsole->getScriptEngine().registerObjectMethod("AgcmPostFX", "void On()", asMETHOD(AgcmPostFX,On));
		ASSERT(result);
		result = pAgcmUIConsole->getScriptEngine().registerObjectMethod("AgcmPostFX", "void Off(bool)", asMETHOD(AgcmPostFX,Off));
		ASSERT(result);
		result = pAgcmUIConsole->getScriptEngine().registerObjectVariable("AgcmPostFX", "int m_bOn", offsetof(AgcmPostFX,m_bOn));
		ASSERT(result);

		// Register the instance.
		result = pAgcmUIConsole->getScriptEngine().registerGlobalVariable("AgcmPostFX postFX", this);
		ASSERT(result);
		// Register the keyword to the console.
		pAgcmUIConsole->getConsole().registerKeyword("postFX.");
	}*/
	AS_REGISTER_TYPE_BEGIN(AgcmPostFX, postFX);
		AS_REGISTER_METHOD0(void, On);
		AS_REGISTER_METHOD1(void, Off, bool);
		//@{ Jaewon 20050223
		AS_REGISTER_METHOD0(void, saveNextFrame);
		//@} Jaewon
		AS_REGISTER_VARIABLE(int, m_bOn);
	AS_REGISTER_TYPE_END;
	//@} Jaewon

	RwCamera* pMainCam = GetCamera();
	ASSERT(pMainCam);
	RwRaster* pRaster = RwCameraGetRaster(pMainCam);
	int w = RwRasterGetWidth(pRaster);
	int h = RwRasterGetHeight(pRaster);

	// load the fx.
	HRESULT hr;
	LPD3DXBUFFER pErrorMsgs = NULL;
	//@{ Jaewon 20040921
	// macro definition added.
	D3DXMACRO macro[] = {
		{ "ARCHLORD", "1" },
		{ NULL, NULL }
	};
	//@{ Jaewon 20040922
	// support md5 encryption of the fx file.
	if(RwFexist("./effect/post.txt"))
	// encrypted file
	{
		// get the file to a memory.
		FILE *pFile = fopen("./effect/post.txt", "rb");
		ASSERT(pFile);

		fseek(pFile, 0, SEEK_END);	
	    long fileSize = ftell(pFile);		
		fseek(pFile, 0, SEEK_SET);

		char *buffer = new char[fileSize];
		memset(buffer, 0, fileSize);
		size_t readBytes = fread(buffer, sizeof(char), fileSize, pFile);
		ASSERT(readBytes == fileSize);
		fclose(pFile);

		// decrypt the file.
		AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
		bool bOK = MD5.DecryptString(MD5_HASH_KEY_STRING, buffer, fileSize);
#else
		bool bOK = MD5.DecryptString(HASH_KEY_STRING, buffer, fileSize);
#endif
		if(bOK == false)
		{
			delete [] buffer;
			ASSERT(0);
		}
		else
		{
		// create a effect from the decrypted buffer.
		hr = D3DXCreateEffect(((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice()),
								buffer, fileSize,
								macro, NULL,
								0,//D3DXSHADER_SKIPVALIDATION,
								NULL,
								&m_pd3dEffect,
								&pErrorMsgs);

		delete [] buffer;
		}
	}
	else if(RwFexist("./effect/post.fx"))
	// plain file
	{
		/*hr = D3DXCreateEffectFromFile(((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice()),
								"./effect/post.fx",
								macro, NULL,
								0,//D3DXSHADER_SKIPVALIDATION,
								NULL,
								&m_pd3dEffect,
								&pErrorMsgs);*/
		hr = D3DXCreateEffectFromFile(((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice()),
			"./effect/post.fx",
			macro, NULL,
			D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,//D3DXSHADER_SKIPVALIDATION,
			NULL,
			&m_pd3dEffect,
			&pErrorMsgs);
	}
	else
	{
		//@{ Jaewon 20050318
		// Only in debug mode
#ifdef _DEBUG
		MessageBox(NULL, "post.txt or post.fx not found, so post fx will be disabled.", "AgcmPostFX", MB_OK);
#endif
		//@} Jaewon
		ASSERT(0);
		m_pd3dEffect = NULL;
		return TRUE;
	}
	//@} Jaewon
	//@} Jaewon
	if(FAILED(hr))
	{
		if(pErrorMsgs)
		{
			const char* pLog = (const char*)pErrorMsgs->GetBufferPointer();
			OutputDebugString(pLog);
			char buf[1024];
			sprintf(buf, "FX compile error : \"%s\", so post fx will be disabled.", pLog);
			//@{ Jaewon 20050401
			// Only in debug mode
#ifdef _DEBUG
			MessageBox(NULL, buf, "AgcmPostFX", MB_OK);	
#endif
			//@} Jaewon
			pErrorMsgs->Release();
		}
		
		ASSERT(0);

		m_pd3dEffect = NULL;

		return TRUE;
	}
	else if(pErrorMsgs)
	{
		pErrorMsgs->Release();
	}

	// obtain the handle to the saved scene texture.
	m_hTexScene = m_pd3dEffect->GetParameterByName(NULL, "g_txScene");
	ASSERT(m_hTexScene);

	//@{ Jaewon 20041011
	m_hTexSquare = m_pd3dEffect->GetParameterByName(NULL, "g_txSquare");
	ASSERT(m_hTexSquare);
	m_hSquareRatio = m_pd3dEffect->GetParameterByName(NULL, "g_squareRatio");
	ASSERT(m_hSquareRatio);
	//@} Jaewon

	//@{ Jaewon 20040921
	// obtain the handle to the previous result.
	m_hTexPrev = m_pd3dEffect->GetParameterByName(NULL, "g_txPrev");
	ASSERT(m_hTexPrev);
	// radius & center
	m_hCenter = m_pd3dEffect->GetParameterByName(NULL, "g_center");
	ASSERT(m_hCenter);
	m_hRadius = m_pd3dEffect->GetParameterByName(NULL, "g_radius");
	ASSERT(m_hRadius);
	//@} Jaewon

	//@{ Jaewon 20050223
	m_hTexTransSource = m_pd3dEffect->GetParameterByName(NULL, "g_txTransSource");
	#ifndef USE_MFC
	ASSERT(m_hTexTransSource);
	#endif
	//@} Jaewon

	// obtain handles to the global parameters.
	m_hWindowSize = m_pd3dEffect->GetParameterByName(NULL, "g_windowSize");
	ASSERT(m_hWindowSize);
	m_hElapsedTime = m_pd3dEffect->GetParameterByName(NULL, "g_fElapsedTime");
	ASSERT(m_hElapsedTime);
	m_pd3dEffect->SetFloat(m_hElapsedTime, 0.0f);

	// get all techniques in the fx.
	D3DXHANDLE hTechnic;
	for(int i=0; hTechnic=m_pd3dEffect->GetTechnique(i); ++i)
	{
		Technique *pTechnic = new Technique();
		pTechnic->init(m_pd3dEffect, hTechnic);
		m_technics.push_back(pTechnic);
		if(strcmp(pTechnic->m_name, "Average") == 0)
			m_pAveragingTechnic = pTechnic;
		else if(strcmp(pTechnic->m_name, "ControlExposure") == 0)
			m_pExposureControlTechnic = pTechnic;
	}

	// create a vertex declaration and a vertex buffer.
    if(RwD3D9CreateVertexDeclaration(PFXVERTEX::Decl, (void**)&m_pVertexDecl) == FALSE)
    {
		ASSERT(0);
		
		m_pVertexDecl = NULL;

        return TRUE;
    }
	UINT32 offset = 0;
	if(RwD3D9CreateVertexBuffer(sizeof(PFXVERTEX), sizeof(PFXVERTEX)*4, (void**)&m_pVertexBuffer, &offset) == FALSE)
	{
		ASSERT(0);

		m_pVertexBuffer = NULL;
		
		return TRUE;
	}

	// fill in the vertex buffer.
	PFXVERTEX Quad[4] =
	{
		{ -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
		{  1.0f,  1.0f, 0.5f, 1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
		{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f }
	};
	LPVOID pVBData;
	//@{ Jaewon 20040921
	// remove D3DLOCK_DISCARD
	if(SUCCEEDED(m_pVertexBuffer->Lock(0, 0, &pVBData, 0)))
	//@} Jaewon
	{
		memcpy(pVBData, Quad, sizeof(Quad));
		m_pVertexBuffer->Unlock();
	}
	else
	{
		ASSERT(0);

		return TRUE;
	}

	//@{ Jaewon 20040811
/*	// init the render target chain.
	if(m_RTChain.init(w, h) == false)
	{
		ASSERT(0);

		m_RTChain.m_pRenderTarget[0] = NULL;
		m_RTChain.m_pRenderTarget[1] = NULL;

		return TRUE;
	}

	// create a render target for the main scene.
	m_pSceneRenderTarget = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
	if(m_pSceneRenderTarget == NULL)
	{
		ASSERT(0);
		return TRUE;
	}

	// set some global parameters.
	_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
	m_pd3dEffect->SetTexture(m_hTexScene, rasExt->texture);
	D3DXVECTOR4 windowSize(float(w), float(h), 0, 0);
	m_pd3dEffect->SetVector(m_hWindowSize, &windowSize);

	// create render targets for scene averaging.
	int qdwSize = 64;
	for(i=0; i<4; ++i, qdwSize/=4)
	{
		m_pAverageRenderTarget[i] = RwRasterCreate(qdwSize, qdwSize, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
		ASSERT(m_pAverageRenderTarget[i]);
	}
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[0]));
	hr = m_pd3dEffect->SetTexture("g_tx64x64", rasExt->texture);
	ASSERT(SUCCEEDED(hr));
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[1]));
	hr = m_pd3dEffect->SetTexture("g_tx16x16", rasExt->texture);
	ASSERT(SUCCEEDED(hr));
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[2]));
	hr = m_pd3dEffect->SetTexture("g_tx4x4", rasExt->texture);
	ASSERT(SUCCEEDED(hr));
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[3]));
	hr = m_pd3dEffect->SetTexture("g_txExposure", rasExt->texture);
	ASSERT(SUCCEEDED(hr));

	// create render targets for the luminance feedback control.
	if(m_lumFeedbackRTChain.init(1, 1) == false)
	{
		ASSERT(0);

		m_lumFeedbackRTChain.m_pRenderTarget[0] = NULL;
		m_lumFeedbackRTChain.m_pRenderTarget[1] = NULL;
	}
	// set the default luminance.
	UINT8 *pPixel = RwRasterLock(m_lumFeedbackRTChain.source(), 0, rwRASTERLOCKWRITE|rwRASTERLOCKNOFETCH);
	*(pPixel+0) = UINT8(255.0f*(2.0f-1.318)/2.182);
	*(pPixel+1) = *(pPixel+0);
	*(pPixel+2) = *(pPixel+0);
	*(pPixel+3) = *(pPixel+0);
	RwRasterUnlock(m_lumFeedbackRTChain.source());
*/
	//@} Jaewon

	// obtain the handle to the current exposure texture.
	m_hTexCurrentExposure = m_pd3dEffect->GetParameterByName(NULL, "g_txCurrentExposure");
	ASSERT(m_hTexCurrentExposure);

	//@{ Jaewon 20040921
	// if there are any textures associated with a image file,
	// load the file and bind it.
	D3DXEFFECT_DESC effectDesc;
    m_pd3dEffect->GetDesc(&effectDesc);

    for(int i=0; i!=effectDesc.Parameters; ++i)
    {
        D3DXPARAMETER_DESC desc;
        D3DXHANDLE handle;
        D3DXHANDLE annot;

        handle = m_pd3dEffect->GetParameter(NULL, i);
        m_pd3dEffect->GetParameterDesc(handle, &desc);

		// if it is a scene texture, just skip it.
		if(handle == m_hTexScene)
			continue;

		if(desc.Class == D3DXPC_OBJECT
			&& desc.Type == D3DXPT_TEXTURE)
		// if it is a texture parameter,
		{
			// get the filename for the texture.
			annot = m_pd3dEffect->GetAnnotationByName(handle, "ResourceName");
			if(NULL == annot)
				continue;

			const RwChar *file;
			hr = m_pd3dEffect->GetString(annot, &file);
			ASSERT(SUCCEEDED(hr));

/*			// drop the file extension.
			RwChar texName[rwTEXTUREBASENAMELENGTH];
			strncpy(texName, file, rwTEXTUREBASENAMELENGTH);
			RwInt32 len = (RwInt32)strlen(texName);
			while(len >= 0)
			{
				if(texName[len] == '.')
				{
					texName[len] = '\0';
					break;
				}
				--len;
			}*/
			RwChar texName[MAX_PATH];
			//@{ Jaewon 20050116
			// Etc -> NotPacked
			strcpy(texName, ".\\Texture\\NotPacked\\");
			//@} Jaewon
			strcat(texName, file);

			// get the texture type.
			int ttype = 0; // 0 - 1D,2D / 1 - Cube / 2 - Volume
			annot = m_pd3dEffect->GetAnnotationByName(handle, "TextureType");
			if(annot)
			{
				const RwChar *textureType;
				hr = m_pd3dEffect->GetString(annot, &textureType);
				ASSERT(SUCCEEDED(hr));

				if(0 == stricmp(textureType, "volume"))
					ttype = 2;
				else if(0 == stricmp(textureType, "cube"))
					ttype = 1;
				else
					ttype = 0;
			}

			LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
			LPDIRECT3DBASETEXTURE9 texture;
			HRESULT hr;
			if(ttype == 2)
				hr = D3DXCreateVolumeTextureFromFile(pd3dDevice, texName, (LPDIRECT3DVOLUMETEXTURE9 *)&texture);
			else if(ttype == 1)
				hr = D3DXCreateCubeTextureFromFile(pd3dDevice, texName, (LPDIRECT3DCUBETEXTURE9 *)&texture);
			else
				hr = D3DXCreateTextureFromFile(pd3dDevice, texName, (LPDIRECT3DTEXTURE9 *)&texture);

			if(SUCCEEDED(hr))
			{
				m_pd3dEffect->SetTexture(handle, texture);
				m_textureLoaded.push_back(texture);
			}
			else
			{
				//@{ Jaewon 20041005
				MD_SetErrorMessage("AgcmPostFX : Cannot load a texture \"%s\", related post fxs will not function properly.", texName);
				
// 마고자 (2004-10-07 오전 11:20:36) : 
// 에러가 두번씩 떠서 일단 주석처리 해뒀어용.
//#ifdef _DEBUG
//				char buf[1024];
//				sprintf(buf, "Cannot load a texture : \"%s\", related post fxs will not function properly.", texName);
//				MessageBox(NULL, buf, "AgcmPostFX", MB_OK);	
//#endif
				//@} Jaewon
			}
		}
	}
	//@} Jaweon
		
	return TRUE;
}

BOOL AgcmPostFX::OnDestroy()
{
	//@{ Jaewon 20041026
	// in order to be safe
	Off();
	//@} Jaewon

	m_hTexScene = NULL;
	m_hWindowSize = NULL;
	m_hElapsedTime = NULL;
	m_hTexCurrentExposure = NULL;
	//@{ Jaewon 20040921
	m_hTexPrev = NULL;
	m_hRadius = NULL;
	m_hCenter = NULL;
	//@} Jaewon
	//@{ Jaewon 20041011
	m_hTexSquare = NULL;
	m_hSquareRatio = NULL;
	//@} Jaewon
	//@{ Jaewon 20050223
	m_hTexTransSource = NULL;
	//@} Jaewon

	if(m_pSceneRenderTarget)
		destroyRenderTargets();

	//@{ Jaewon 20040921
	// release loaded textures.
	UINT32 i;
	for(i=0; i<m_textureLoaded.size(); ++i)
	{
		m_textureLoaded[i]->Release();
	}
	m_textureLoaded.clear();
	//@} Jaewon

	// clear the active pipeline.
	for(i=0; i<m_pipeline.size(); ++i)
	{
		delete m_pipeline[i];
	}
	m_pipeline.clear();

	// destroy the vertex declaration and the vertex buffer.
	if(m_pVertexDecl)
	{
		RwD3D9DeleteVertexDeclaration(m_pVertexDecl);
		m_pVertexDecl = NULL;
	}
	if(m_pVertexBuffer)
	{
		RwD3D9DestroyVertexBuffer(sizeof(PFXVERTEX), sizeof(PFXVERTEX)*4, m_pVertexBuffer, 0);
		m_pVertexBuffer = NULL;
	}

	// clear all techniques.
	for(i=0; i<m_technics.size(); ++i)
	{
		delete m_technics[i];
	}
	m_technics.clear();

	m_pAveragingTechnic = NULL;

	// destroy the fx.
	if(m_pd3dEffect)
	{
		m_pd3dEffect->Release();
		m_pd3dEffect = NULL;
	}
	
	return TRUE;
}
	
void AgcmPostFX::onLostDevice()
{	
	//@{ Jaewon 20040811
	if(m_pd3dEffect==NULL) return;

	m_pd3dEffect->OnLostDevice();

	//@{ Jaewon 20041122
	if(m_pSceneRenderTarget)
	{
		destroyRenderTargets();
		m_bDoNotCreate = false;
	}
	else
		m_bDoNotCreate = true;
	//@} Jaewon
}

void AgcmPostFX::onResetDevice()
{
	//@{ Jaewon 20040811
	if(m_pd3dEffect==NULL) return;
		
	m_pd3dEffect->OnResetDevice();

	//@{ Jaewon 20041122
	if(m_bDoNotCreate == false)
		createRenderTargets();
	//@} Jaewon
}

//@{ Jaewon 20041011
// get the current pipeline sequence.
// return FALSE if there is no active pipeline.
BOOL AgcmPostFX::getPipeline(char* pipeline, int nMax) const
{
	ASSERT(nMax > 0);

	if(m_pSceneRenderTarget==NULL || m_pipeline.size()==0)
	{
		return FALSE;
	}

	strncpy(pipeline, m_technics[m_pipeline[0]->m_nFxIndex]->m_name, nMax);
	nMax -= strlen(pipeline);
	if(nMax <= 0)
		return TRUE;
	for(UINT32 i=1; i<m_pipeline.size(); ++i)
	{
		strncat(pipeline, "-", nMax);
		nMax -= 1;
		if(nMax <= 0)
			return TRUE;
		strncat(pipeline, m_technics[m_pipeline[i]->m_nFxIndex]->m_name, nMax);
		nMax -= strlen(m_technics[m_pipeline[i]->m_nFxIndex]->m_name);
		if(nMax <= 0)
			return TRUE;
	}

	return TRUE;
}
//@} Jaewon

BOOL AgcmPostFX::setPipeline(const char* pipeline, BOOL bCheck)
{
	//@{ Jaewon 20050106
	// render targets may be created when the system is turned on.
	//if(m_pSceneRenderTarget==NULL)
	//{
	//	return FALSE;
	//}
	//@} Jaewon

	if(bCheck && !isPipelineValid(pipeline))
	{
		return FALSE;	
	}

	m_bValid = TRUE;

	// construct the pipeline.
	// first, clear the previous pipeline, if any.
	for(UINT32 i=0; i<m_pipeline.size(); ++i)
	{
		delete m_pipeline[i];
	}
	m_pipeline.clear();
	// parse the string and fill the pipeline.
	char seps[] = "-";
	char *token;
	char buf[256];
	ASSERT(strlen(pipeline) < 256);
	strcpy(buf, pipeline);
	token = strtok(buf, seps);
	while(token != NULL)
	{
		// search the technique list for a technique corresponding to a token.
		UINT32 index;
		for(index=0; index<m_technics.size(); ++index)
		{
			if(strcmp(m_technics[index]->m_name, token) == 0)
				break;
		}
		ASSERT(index < m_technics.size());
		// add a new technique instance.
		TechniqueInstance *pNew = new TechniqueInstance();
		pNew->m_nFxIndex = index;
		for(int i=0; i<NUM_PARAMS; ++i)
			pNew->m_vParam[i] = m_technics[pNew->m_nFxIndex]->m_vParamDef[i];
		m_pipeline.push_back(pNew);
		// get next token.
		token = strtok(NULL, seps);
	}
	
	return TRUE;
}

BOOL AgcmPostFX::isPipelineValid(const char* pipeline) const
{
	//@{ Jaewon 20050401
	if(m_pd3dEffect==NULL) 
		return FALSE;
	//@} Jaewon

	if(pipeline==NULL || strlen(pipeline)==0)
		return FALSE;
	
	// parse the string and check the validity.
	char seps[] = "-";
	char *token;
	char buf[256];
	ASSERT(strlen(pipeline) < 256);
	strcpy(buf, pipeline);
	token = strtok(buf, seps);
	while(token != NULL)
	{
		// search the technique list for a technique corresponding to a token.
		UINT32 index;
		for(index=0; index<m_technics.size(); ++index)
		{
			if(strcmp(m_technics[index]->m_name, token) == 0)
				break;
		}
		if(index >= m_technics.size())
			return FALSE;
		if(FAILED(m_pd3dEffect->ValidateTechnique(m_technics[index]->m_hTechnique)))
			return FALSE;
		// get next token.
		token = strtok(NULL, seps);
	}
	
	return TRUE;
}

void AgcmPostFX::render()
{
	static char profileBuf[30][64];
	PROFILE("AgcmPostFX::render");

	//@{ Jaewon 20050401
	if(m_pd3dEffect==NULL) 
		return;
	//@} Jaewon

	//@{ Jaewon 20050223
	// If m_bSaveNextFrame is true, save the back buffer 
	// to the transition source and return.

	//@{ kday 20050305
	// m_pTransitionSource == NULL
	if(!m_bOn && m_bSaveNextFrame && m_pTransitionSource )
	//if(!m_bOn && m_bSaveNextFrame)
	//@} kday
	{
		RwCamera* pMainCam = GetCamera();
		RwCameraEndUpdate(pMainCam);
		LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
		pd3dDevice->EndScene();

		//@{ Jaewon 20050304
		// m_RTChain.m_pRenderTarget[0] -> m_pTransitionSource
		_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
		//@} Jaewon
		LPDIRECT3DSURFACE9 pSourceSurface;
		HRESULT hr = rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);
		ASSERT(hr==D3D_OK);
		hr = pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSourceSurface, NULL, D3DTEXF_NONE);
		ASSERT(hr==D3D_OK);
		pSourceSurface->Release();

		pd3dDevice->BeginScene();
		LockFrame();
		RwCameraBeginUpdate(pMainCam);
		//@{ Jaewon 20050401
		RwD3D9SetRenderState(D3DRS_FOGEND,  *((DWORD*)&(m_pAgcmRender->m_fFogEnd))); 
		//@} Jaewon
		UnlockFrame();

		m_bSaveNextFrame = false;
		return;
	}
	//@} Jaewon

	static clock_t prev = clock();
	clock_t now = clock();
	float dt = float(now - prev)/CLOCKS_PER_SEC;
	prev = now;

	if(!m_bOn)
		return;

	if(m_hElapsedTime)
		m_pd3dEffect->SetFloat(m_hElapsedTime, dt);

	RwCamera* pMainCam = GetCamera();
	RwCameraEndUpdate(pMainCam);

	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());

	pd3dDevice->EndScene();

//@{ Jaewon 20041126
#ifdef POSTFX_USE_STRETCH_RECT
	{
	// copy the back buffer to m_pSceneRenderTarget.
	_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
	LPDIRECT3DSURFACE9 pSceneSurface;
	HRESULT hr = rasExt->texture->GetSurfaceLevel(0, &pSceneSurface);
	ASSERT(hr==D3D_OK);
	pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSceneSurface, NULL, D3DTEXF_NONE);
	pSceneSurface->Release();
	}
#endif
//@} Jaewon

	// automatic exposure control
	if(m_autoExposure.m_bAutoExposureControl)
		controlExposure();

	// if the specified time has passed, update the target exposure level.
	static clock_t prevAve = clock();
	clock_t nowAve = clock();
	float dtAve = float(nowAve - prevAve)/CLOCKS_PER_SEC;
	if(dtAve > m_autoExposure.m_fTargetUpdateInterval)
	{
		if(m_autoExposure.m_bAutoExposureControl)
		{
			averageScene();
			//m_autoExposure.m_fTargetExposureLevel = getSceneBrightness();
			//if(m_autoExposure.m_fTargetExposureLevel<0.25f) m_autoExposure.m_fTargetExposureLevel = 0.25f;
			//else if(m_autoExposure.m_fTargetExposureLevel>0.6f) m_autoExposure.m_fTargetExposureLevel = 0.6f;
			//m_autoExposure.m_fTargetExposureLevel = 1.0f/m_autoExposure.m_fTargetExposureLevel - 0.5f;
		}
		prevAve += ( clock_t ) m_autoExposure.m_fTargetUpdateInterval;

		//char buf[64];
		//sprintf(buf, "Target exposure level : %f\n", m_autoExposure.m_fTargetExposureLevel);
		//OutputDebugString(buf);
	}
	
	// perform post-processing.
	for(UINT32 i=0; i<m_pipeline.size(); ++i)
	// for each technique instance
	{
		TechniqueInstance *pTI = m_pipeline[i];
		Technique *pT = m_technics[m_pipeline[i]->m_nFxIndex];
		
		// if there are any parameters, initialize them here.
		for(int j=0; j<NUM_PARAMS; ++j)
			if(pT->m_hParam[j])
				m_pd3dEffect->SetVector(pT->m_hParam[j], &(pTI->m_vParam[j]));

		// render the quad.
		if(SUCCEEDED(pd3dDevice->BeginScene()))
		{
			sprintf(profileBuf[4*i+0], "AgcmPostFX::render::%s", pT->m_name);
			PROFILE(profileBuf[4*i+0]);

			//@{ Jaewon 20041011
			if(pT->m_bDepedentRead)
			// copy the previous target to a pow2 target.
			{
				if(!m_pSquareRenderTarget)
				{
					pd3dDevice->EndScene();
					continue;
				}

				m_pd3dEffect->SetTechnique(pT->m_bDepedentRead);
				RwD3D9SetVertexDeclaration(m_pVertexDecl);
				UINT32 cPasses;
				m_pd3dEffect->Begin(&cPasses, 0);
				// set up the textures.
				if(i==0)// && p==0)
				// if this is the very first post-process rendering,
				// obtain the source textures from the scene.
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}
				else
				// Otherwise, initialize the post-process source texture to
				// the previous render target.
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(m_RTChain.source());
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}				
				RwD3D9SetRenderTarget(0, m_pSquareRenderTarget);
				D3DVIEWPORT9 vp;
				pd3dDevice->GetViewport(&vp);
				vp.Width = RwRasterGetWidth(m_pSceneRenderTarget); 
				vp.Height = RwRasterGetHeight(m_pSceneRenderTarget);
				pd3dDevice->SetViewport(&vp);
				m_pd3dEffect->BeginPass(0);
				pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
				pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				m_pd3dEffect->EndPass();
				m_pd3dEffect->End();
			}
			//@} Jaewon

			m_pd3dEffect->SetTechnique(pT->m_hTechnique);

			// set the vertex declaration.
			RwD3D9SetVertexDeclaration(m_pVertexDecl);

			// draw the quad.
			UINT32 cPasses;
			m_pd3dEffect->Begin(&cPasses, 0);
			for(UINT32 p=0; p<cPasses; ++p)
			{
				//@{ Jaewon 20040921
				// m_hTexSource removed.
				// use m_hTexPrev instead.
				sprintf(profileBuf[4*i+1], "AgcmPostFX::render::%s::%s", pT->m_name, "SetTexture");
				PROFILE(profileBuf[4*i+1]);
				// set up the textures.
				if(i==0)// && p==0)
				// if this is the very first post-process rendering,
				// obtain the source textures from the scene.
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}
				else
				// Otherwise, initialize the post-process source texture to
				// the previous render target.
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(m_RTChain.source());
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}
				//@} Jaewon

				{
				sprintf(profileBuf[4*i+2], "AgcmPostFX::render::%s::%s", pT->m_name, "SetRenderTarget");
				PROFILE(profileBuf[4*i+2]);
				// set up the new render target.
				if(i==m_pipeline.size()-1)// && p==cPasses-1)
				// if this is the very last post-process rendering,
				// output to the original back buffer.
				{
					_rwD3D9SetRenderTarget(0, _RwD3D9RenderSurface);
					//RwD3D9SetRenderTarget(0, m_pOriginalRenderTarget);
				}
				else
				// Otherwise, output to the next render target.
				{
					RwD3D9SetRenderTarget(0, m_RTChain.target());
				}
				}

				// clear the render target.
				//pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000,
				//	1.0f, 0L);

				// render.
				{
				sprintf(profileBuf[4*i+3], "AgcmPostFX::render::%s::%s", pT->m_name, "Draw");
				PROFILE(profileBuf[4*i+3]);
				m_pd3dEffect->BeginPass(p);
				pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
				pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				m_pd3dEffect->EndPass();
				}

				// update next rendertarget index.
				m_RTChain.flip();
			}
			m_pd3dEffect->End();

			pd3dDevice->EndScene();
		}
	}

	//@{ Jaewon 20050304
	// If m_bSaveNextFrame is true, save the back buffer 
	// to the transition source and return.
	if(m_bSaveNextFrame)
	{
		_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
		LPDIRECT3DSURFACE9 pSourceSurface;
		HRESULT hr = rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);
		ASSERT(hr==D3D_OK);
		hr = pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSourceSurface, NULL, D3DTEXF_NONE);
		ASSERT(hr==D3D_OK);
		pSourceSurface->Release();

		m_bSaveNextFrame = false;
	}
	//@} Jaewon

	{
	PROFILE("AgcmPostFX::render::restore");
	pd3dDevice->BeginScene();

#ifndef POSTFX_USE_STRETCH_RECT
	// now, restore the original render target.
	RwCameraSetRaster(pMainCam, m_pOriginalRenderTarget);

	//@{ Jaewon 20041013
	RwCameraSetZRaster(pMainCam, m_pOriginalZBuffer);
	//@} Jaewon
#endif

	LockFrame();
	RwCameraBeginUpdate(pMainCam);
	//@{ Jaewon 20050401
	// Why was this commented out? --a
	RwD3D9SetRenderState(D3DRS_FOGEND,  *((DWORD*)&(m_pAgcmRender->m_fFogEnd))); 
	//@} Jaewon
	UnlockFrame();
	}
}

void AgcmPostFX::changeRenderTarget()
{
	PROFILE("AgcmPostFX::changeRenderTarget");

	if(!m_bOn)
		return;

#ifndef POSTFX_USE_STRETCH_RECT
	RwCamera* pMainCam = GetCamera();
	RwCameraSetRaster(pMainCam, m_pSceneRenderTarget);	

	//@{ Jaewon 20041013
	RwCameraSetZRaster(pMainCam, m_pZBuffer);
	//@} Jaewon
#endif
}

void AgcmPostFX::OnCameraStateChange(CAMERASTATECHANGETYPE ctype)
{
	// for the initialization of render targets in window mode
	if(ctype == CSC_RESIZE && m_pSceneRenderTarget == NULL)
	{
		onLostDevice();
		onResetDevice();
	}
}

void AgcmPostFX::averageScene()
{
	PROFILE("AgcmPostFX::render::averageScene");

	if(m_pAverageRenderTarget[3] == NULL || m_pAveragingTechnic == NULL)
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());

	// do scene averaging.
	if(SUCCEEDED(pd3dDevice->BeginScene()))
	{
		m_pd3dEffect->SetTechnique(m_pAveragingTechnic->m_hTechnique);

		// set the vertex declaration.
		RwD3D9SetVertexDeclaration(m_pVertexDecl);

		// draw the quad.
		UINT32 cPasses;
		m_pd3dEffect->Begin(&cPasses, 0);
		ASSERT(cPasses == 4);
		for(UINT32 p=0; p<cPasses; ++p)
		{
			RwD3D9SetRenderTarget(0, m_pAverageRenderTarget[p]);

			// clear the render target.
			//pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000,
			//	1.0f, 0L);

			// render.
			m_pd3dEffect->BeginPass(p);
			pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
			pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			m_pd3dEffect->EndPass();
		}
		m_pd3dEffect->End();

		pd3dDevice->EndScene();
	}
}

void AgcmPostFX::controlExposure()
{
	PROFILE("AgcmPostFX::render::controlExposure");

	if(m_lumFeedbackRTChain.m_pRenderTarget[0] == NULL || m_pExposureControlTechnic == NULL)
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());

	if(SUCCEEDED(pd3dDevice->BeginScene()))
	{
		m_pd3dEffect->SetTechnique(m_pExposureControlTechnic->m_hTechnique);

		// set the vertex declaration.
		RwD3D9SetVertexDeclaration(m_pVertexDecl);

		// draw the quad.
		UINT32 cPasses;
		m_pd3dEffect->Begin(&cPasses, 0);
		ASSERT(cPasses==1);
		RwD3D9SetRenderTarget(0, m_lumFeedbackRTChain.target());

		// clear the render target.
		//pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000,
		//	1.0f, 0L);

		// render.
		m_pd3dEffect->BeginPass(0);
		pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		m_pd3dEffect->EndPass();

		m_lumFeedbackRTChain.flip();
		
		m_pd3dEffect->End();

		pd3dDevice->EndScene();
	}

	_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(m_lumFeedbackRTChain.source());
	m_pd3dEffect->SetTexture(m_hTexCurrentExposure, rasExt->texture);

	// feedback loop
//	float diff = m_fTargetExposureLevel-m_fCurrentExposureLevel;
//	m_fCurrentExposureLevel += diff*m_fFeedbackK*min(dt, 0.1f);

	// clamping
//	m_fCurrentExposureLevel = max(_fMinExposure/2.0f, min(m_fCurrentExposureLevel, _fMaxExposure*2.0f));
}

//@{ Jaewon 20040921
void AgcmPostFX::setRadius(float radius)
{
	if(!m_bOn)
		return;
	
	ASSERT(m_hRadius);

	m_pd3dEffect->SetFloat(m_hRadius, radius);
}

void AgcmPostFX::setCenter(float u, float v)
{
	if(!m_bOn)
		return;

	ASSERT(m_hCenter);

	D3DXVECTOR4 center(u, v, 0, 0);
	m_pd3dEffect->SetVector(m_hCenter, &center);
}
//@} Jaewon

//@{ Jaewon 20041011
void AgcmPostFX::AutoExposureControlOff() 
{ 
	if(m_autoExposure.m_bAutoExposureControl)
	{
		if(m_lumFeedbackRTChain.source())
		{
			// set the exposure level to a default.
			UINT8 *pPixel = RwRasterLock(m_lumFeedbackRTChain.source(), 0, rwRASTERLOCKWRITE|rwRASTERLOCKNOFETCH);
			*(pPixel+0) = UINT8(255.0f*0.4f);
			*(pPixel+1) = *(pPixel+0);
			*(pPixel+2) = *(pPixel+0);
			*(pPixel+3) = *(pPixel+0);
			RwRasterUnlock(m_lumFeedbackRTChain.source());		
		}

		m_autoExposure.m_bAutoExposureControl = FALSE; 
	}
}
//@} Jaewon

//@{ Jaewon 20041122
// in order to save video memory when post fx is off
void AgcmPostFX::createRenderTargets()
{
	ASSERT(m_pSceneRenderTarget == NULL);

	//@{ Jaewon 20050401
	if(m_pd3dEffect==NULL)
	{
		ASSERT(0);

		Off();
		m_bValid = FALSE;
		return;
	}
	//@} Jaewon

	// all render target should be recreated.
	// if the post fx was enabled,
	// turn it on again with new targets.
	RwCamera* pMainCam = GetCamera();
	ASSERT(pMainCam);
	RwRaster* pRaster = RwCameraGetRaster(pMainCam);

	//. 2006. 3. 14. Nonstopdj
	//. RwRaster* validation check
	if(!pRaster)
		return;

	int w = RwRasterGetWidth(pRaster);
	int h = RwRasterGetHeight(pRaster);
	
	// init the render target chain.
	if(m_RTChain.init(w, h) == false)
	{
		ASSERT(0);

		m_RTChain.m_pRenderTarget[0] = NULL;
		m_RTChain.m_pRenderTarget[1] = NULL;

		Off();
		m_bValid = FALSE;
		return;
	}

	// create a render target for the main scene.
	m_pSceneRenderTarget = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
	if(m_pSceneRenderTarget == NULL)
	{
		ASSERT(0);

		Off();
		m_bValid = FALSE;
		return;
	}

#ifndef POSTFX_USE_STRETCH_RECT
	//@{ Jaewon 20041013
	// create a z buffer for the main scene.
	m_pZBuffer = RwRasterCreate(w, h, 0, rwRASTERTYPEZBUFFER);
	if(m_pZBuffer == NULL)
	{
		ASSERT(0);

		Off();
		m_bValid = FALSE;
		return;
	}
	//@} Jaewon
#endif

	// create render targets for scene averaging.
	int qdwSize = 64;
	for(int i=0; i<4; ++i, qdwSize/=4)
	{
		m_pAverageRenderTarget[i] = RwRasterCreate(qdwSize, qdwSize, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
		ASSERT(m_pAverageRenderTarget[i]);
	}

	// create render targets for the luminance feedback control.
	if(m_lumFeedbackRTChain.init(1, 1) == false)
	{
		ASSERT(0);

		m_lumFeedbackRTChain.m_pRenderTarget[0] = NULL;
		m_lumFeedbackRTChain.m_pRenderTarget[1] = NULL;
	}
	// set the default luminance.
	UINT8 *pPixel = RwRasterLock(m_lumFeedbackRTChain.source(), 0, rwRASTERLOCKWRITE|rwRASTERLOCKNOFETCH);
	//@{ Jaewon 20041011
	// 0.4f
	*(pPixel+0) = UINT8(255.0f*0.4f);
	//@} Jaewon
	*(pPixel+1) = *(pPixel+0);
	*(pPixel+2) = *(pPixel+0);
	*(pPixel+3) = *(pPixel+0);
	RwRasterUnlock(m_lumFeedbackRTChain.source());
	
	//@{ Jaewon 20041011
	// create a square render target.
	int wPOW2=1, hPOW2=1;
	//@{ Jaewon 20041014
	// 2 -> 1
	while(wPOW2 < w) wPOW2 <<= 1;
	while(hPOW2 < h) hPOW2 <<= 1;
	//@} Jaewon
	m_pSquareRenderTarget = RwRasterCreate(wPOW2, hPOW2, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
	//@{ Jaewon 20050131
	// It might fail, so...
	if(m_pSquareRenderTarget == NULL)
	{
		ASSERT(0);

		Off();
		m_bValid = FALSE;
		return;
	}
	//@} Jaewon
	// set the render target.
	_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSquareRenderTarget));
	m_pd3dEffect->SetTexture(m_hTexSquare, rasExt->texture);	
	// set the 'g_squareRatio' parameter.
	D3DXVECTOR4 squareRatio(float(w)/float(wPOW2), float(h)/float(hPOW2), 0, 0);
	HRESULT hr;
	hr = m_pd3dEffect->SetVector(m_hSquareRatio, &squareRatio);

	// 마고자 (2004-10-24 오후 3:30:20) : 느므 귀찮아서 제거 - -;
	//ASSERT(SUCCEEDED(hr));

	//@{ Jaewon 20050304
	m_pTransitionSource = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
	if(m_pTransitionSource == NULL)
	{
		ASSERT(0);

		Off();
		m_bValid = FALSE;
		return;
	}
	//@} Jaewon

	//@} Jaewon

	// update global parameters.
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
	m_pd3dEffect->SetTexture(m_hTexScene, rasExt->texture);
	
	D3DXVECTOR4 windowSize(float(w), float(h), 0, 0);
	m_pd3dEffect->SetVector(m_hWindowSize, &windowSize);
	
	rasExt = RASTEREXTFROMRASTER(m_lumFeedbackRTChain.source());
	m_pd3dEffect->SetTexture(m_hTexCurrentExposure, rasExt->texture);
	
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[0]));
	hr = m_pd3dEffect->SetTexture("g_tx64x64", rasExt->texture);
	ASSERT(SUCCEEDED(hr));

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[1]));	
	hr = m_pd3dEffect->SetTexture("g_tx16x16", rasExt->texture);
	ASSERT(SUCCEEDED(hr));

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[2]));
	hr = m_pd3dEffect->SetTexture("g_tx4x4", rasExt->texture);
	ASSERT(SUCCEEDED(hr));

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[3]));
	hr = m_pd3dEffect->SetTexture("g_txExposure", rasExt->texture);
	ASSERT(SUCCEEDED(hr));

	//@{ Jaewon 20050223
	// We use m_pTransitionSource as a transition source texture.
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
	m_pd3dEffect->SetTexture(m_hTexTransSource, rasExt->texture);
	//@} Jaewon

	// restore 'on' settings.
	if(isOn())
	{
		m_pOriginalRenderTarget = pRaster;
#ifndef POSTFX_USE_STRETCH_RECT
		RwCameraSetRaster(pMainCam, m_pSceneRenderTarget);
#endif
		//@{ Jaewon 20041014
		m_pOriginalZBuffer = RwCameraGetZRaster(pMainCam);
#ifndef POSTFX_USE_STRETCH_RECT
		RwCameraSetZRaster(pMainCam, m_pZBuffer);
#endif
		//@} Jaewon
	}
	//@} Jaewon
}

void AgcmPostFX::destroyRenderTargets()
{
	ASSERT(m_pSceneRenderTarget);

	// all render target should be released.
	RwCamera* pMainCam = GetCamera();
	ASSERT(pMainCam);

	if(isOn() && pMainCam )
	{
		// restore the original render target.
		ASSERT(m_pOriginalRenderTarget);
		if( m_pOriginalRenderTarget )
		{
			RwCameraSetRaster(pMainCam, m_pOriginalRenderTarget);
			m_pOriginalRenderTarget = NULL;
		}

		//@{ Jaewon 20041014
		ASSERT(m_pOriginalZBuffer);
		if( m_pOriginalZBuffer )
		{
			RwCameraSetZRaster(pMainCam, m_pOriginalZBuffer);
			m_pOriginalZBuffer = NULL;
		}
		//@} Jaewon
	}

	// destroy the main scene render target.
	if(m_pSceneRenderTarget)
	{
		RwRasterDestroy(m_pSceneRenderTarget);
		m_pSceneRenderTarget = NULL;
	}

	//@{ Jaewon 20041013
	// destroy the z buffer.
	if(m_pZBuffer)
	{
		RwRasterDestroy(m_pZBuffer);
		m_pZBuffer = NULL;
	}
	//@} Jaewon

	// destroy the render target chain.
	m_RTChain.~RenderTargetChain();

	for(int i=0; i<4; ++i)
	{
		if(m_pAverageRenderTarget[i])
		{
			RwRasterDestroy(m_pAverageRenderTarget[i]);
			m_pAverageRenderTarget[i] = NULL;
		}
	}

	m_lumFeedbackRTChain.~RenderTargetChain();
	//@} Jaewon

	//@{ Jaewon 20041011
	// destroy the square render target.
	if(m_pSquareRenderTarget)
	{
		RwRasterDestroy(m_pSquareRenderTarget);
		m_pSquareRenderTarget = NULL;
	}
	//@} Jaewon

	//@{ Jaewon 20050304
	if(m_pTransitionSource)
	{
		RwRasterDestroy(m_pTransitionSource);
		m_pTransitionSource = NULL;
	}
	//@} Jaewon
}
//@} Jaewon

//@{ Jaewon 20050329
// Save the current content of the back buffer.
void AgcmPostFX::saveImmediateFrame()
{
	if(NULL == m_pTransitionSource)
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
	_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
	LPDIRECT3DSURFACE9 pSourceSurface;
	HRESULT hr = rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);
	ASSERT(hr==D3D_OK);
	hr = pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSourceSurface, NULL, D3DTEXF_NONE);
	ASSERT(hr==D3D_OK);
	pSourceSurface->Release();
}
//@} Jaewon