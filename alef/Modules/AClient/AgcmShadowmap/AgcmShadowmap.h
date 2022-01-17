#ifndef __AGCMSHADOWMAP_H__
#define __AGCMSHADOWMAP_H__

#include "AgcModule.h"
#include "AgcmMap.h"

#include <deque>

// Shadow mapping shadow module especially for a main playable character.
class AgcmRender;
class AgcmCharacter;
class AgcmShadowmap : public AgcModule
{
public:
	AgcmShadowmap();
	virtual ~AgcmShadowmap();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();

	BOOL registerClump(RpClump* pClump);		// resister a clump as a shadow map receiver.
	BOOL unregisterClump(RpClump* pClump);		// unregister a clump.

	bool On();
	void Off();

	BOOL SetEnable(BOOL	bVal);

	void setPlayerClump(RpClump* pClump) { m_pPlayerClump = pClump; }

	// post & pre render callbacks
	static BOOL	preRenderCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	postRenderCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// clump update & release callbacks
	static BOOL updateClumpCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL releaseClumpCB(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	AgcmRender*		m_pAgcmRender;
	AgcmCharacter*	m_pAgcmCharacter;

	// shadow map renderable texture.
	RwRaster* m_pShadowMap;
	RwRaster* m_pShadowMapZ;
	RwTexture* m_pShadowMapTexture;

	// camera for rendering the shadow map
	RwCamera* m_pShadowCam;

	// matrices for shadow mapping
	D3DXMATRIX m_matShadowVP;
	D3DXMATRIX m_matShadowMVP;
	D3DXMATRIX m_matShadowTEX;
	
	BOOL m_bOn;

	// a shadow caster
	RpClump* m_pPlayerClump;

	// a list of shadow map receivers 
	std::vector<RpClump*> m_clumps;

	// shaders 
	BOOL m_bShaderSupported;
	// 0,1,..,4 => no skin,weight 1,..,weight 4
	PVOID m_pShadowMapVShader[5];
	PVOID m_pShadowMapPShader;
	// 0,1,..,4 => no skin,weight 1,..,weight 4
	PVOID m_pShadowSceneVShader[5];
	PVOID m_pShadowScenePShader;

	// powerpipe pipelines
	RxPipeline* m_atomicPipeline;
	RxPipeline* m_atomicSkinPipeline;
	RxPipeline* m_atomicPipelineForShadowmap;
	RxPipeline* m_atomicSkinPipelineForShadowmap;

	// original pipelines backup
	std::deque<RxPipeline*> m_pipeBU;

	// radius caching for minimal shadow cam updates
	FLOAT m_fPrevRadius;

	//@{ Jaewon 20050118
	// ps.1.4 support 
	bool m_bPs14Supported;
	RwTexture* m_p11bitEncodingTexture;
	RwTexture* m_pSubTexelPrecisionTexture;
	//@} Jaewon

	//@{ Jaewon 20050405
	// A variable depth bias for proper self-shadowing
	float m_fDepthBias;
	//@} Jaewon

	PVOID getShader(const char* pSrc, BOOL bPixel=FALSE) const;

	// update the shadow camera.
	void updateShadowCam();

	// render the shadow map.
	void renderShadowmap();
	// render all clumps which can get shadow from a main PC.
	void render();

	// create pipelines for shadow mapping.
	RxPipeline* createAtomicPipeline() const;
	RxPipeline* createAtomicSkinPipeline() const;
	RxPipeline* createAtomicPipelineForShadowmap() const;
	RxPipeline* createAtomicSkinPipelineForShadowmap() const;

	// concatenate shadow mvp & tex matrices in the shadow scene pipeline.
	void catMatrices(const RwMatrix* ltm);

	// preview the shadow map output for debugging.
	RwBool Im2DRenderQuad(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT z, FLOAT recipCamZ, FLOAT uvOffset) const;
	void previewShadowmap();

	// Create a 11-bit encoding texture for the ps.1.4 version.
	RwTexture* create11BitEncodingMap();
	// Create a subtexel precision texture for the ps.1.4 version.
	RwTexture* createSubTexelPrecisionMap();

	// powerpipe rendering callback for the plain geometry
	static void powerpipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);

	// powerpipe callbacks for the skin geometry
	static RwV4d* powerpipeSkinLightingCB(void* object, RwUInt32 type, RwV4d* shaderConstantPtr, _rpD3D9VertexShaderDescriptor* desc);
	static void* powerpipeSkinGetMaterialShaderCB(const RpMaterial* material, _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch);
	static void powerpipeSkinMeshRenderCB(RxD3D9ResEntryHeader* resEntryHeader, RxD3D9InstanceData* instancedMesh, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch);

	// shadow map rendering powerpipe callbacks for the plain geometry
	static void powerpipeRenderForShadowmapCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);

	// shadow map rendering powerpipe callbacks for the skin geometry
	static RwV4d* powerpipeSkinLightingForShadowmapCB(void* object, RwUInt32 type, RwV4d* shaderConstantPtr, _rpD3D9VertexShaderDescriptor* desc);
	static void* powerpipeSkinGetMaterialShaderForShadowmapCB(const RpMaterial* material, _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch);
	static void powerpipeSkinMeshRenderForShadowmapCB(RxD3D9ResEntryHeader* resEntryHeader, RxD3D9InstanceData* instancedMesh, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch);

	static RpAtomic* setShadowMapPipeline(RpAtomic* pAtomic, void* pData);
	static RpAtomic* setShadowScenePipeline(RpAtomic* pAtomic, void* pData);
	static RpAtomic* returnToOriginalPipeline(RpAtomic* pAtomic, void* pData);
};

#endif