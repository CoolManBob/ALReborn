//@{ Jaewon 20040618
// created.
// Quake3 style dynamic light map rendering module for terrain.
//@} Jaewon

#ifndef __AGCMDYNAMICLIGHTMAP_H__
#define __AGCMDYNAMICLIGHTMAP_H__

#include "AgcModule.h"
#include "AgcmMap.h"
#include "AgcmRender.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmDynamicLightmapD" )
#else
#pragma comment ( lib , "AgcmDynamicLightmap" )
#endif
#endif

class AgcmDynamicLightmap : public AgcModule
{
public:
	// lightmap light information structure
	struct LightInfo
	{
		// pointer to the renderware light
		RpLight* pLight_;
		// bounding sphere for custom render culling
		RwSphere sphere_;
		// visibility flag
		BOOL bVisible_;
		// mobility flag
		BOOL bMovable_;
		// on/off
		BOOL bOn_;
		// camera z index for the octree occlusion
		INT32 iCamZ_;

		LightInfo() : pLight_(NULL), bVisible_(FALSE), bMovable_(TRUE), bOn_(TRUE), iCamZ_(0) {}
	};

	RpLight		*light1,*light2;


	AgcmDynamicLightmap();
	virtual ~AgcmDynamicLightmap();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);
	
	// add a light to the lightmap light list.
	BOOL addLight(RpLight* pLight, BOOL bMovable=TRUE);
	// remove a light from the lightmap light list.
	BOOL removeLight(RpLight* pLight);
	// clear the light list.
	void clearLights();
	// turn on/off individual lights.
	BOOL turnOnLight(RpLight* pLight);
	BOOL turnOffLight(RpLight* pLight);

	// render all visible lightmaps.
	void render();

	// preview the light map output for debugging.
	RwBool Im2DRenderQuad(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,
						FLOAT z, FLOAT recipCamZ, FLOAT uvOffset) const;
	void previewLightmap();

	// post & pre render callbacks
	static BOOL	postRenderCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// custom rendering callbacks for occlusion
	static BOOL customUpdateCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL customRenderCB(PVOID pData, PVOID pClass, PVOID pCustData);

	AgcmMap* GetMapModule() const { return m_pAgcmMap; }
	RwTexture* GetLightmap() const { return m_pLightMap; }
	// compute the attenuation factor of the lightmap in given uv distance.
	FLOAT GetAttenuation(FLOAT d) const 
	{
		return max(1.0f-d, 0.004f);//m_fAttenuationCoeff[0] + m_fAttenuationCoeff[2] * d2; 
	}

	// on/off
	void On() { m_bOn = TRUE; }
	void Off() { m_bOn = FALSE; }
	BOOL isOn() { return m_bOn; }

	// vertex shader 
	PVOID m_pVShader;
	PVOID m_pVDeclaration;
	PVOID m_pVBuffer;
	BOOL m_bUseVShader;
protected:
	// default lightmap texture.
	RwTexture* m_pLightMap;

	// modules used.
	AgcmRender* m_pAgcmRender;
	AgcmMap* m_pAgcmMap;
	AgcmOcTree* m_pAgcmOcTree;
	ApmOcTree* m_pApmOcTree;

	// a list of lightmap lights 
	std::vector<LightInfo*> m_lights;

	// lightmap module on/off
	BOOL m_bOn;

	// create a light map texture.
	RwTexture* CreateLightmap(UINT32 size);
};

#endif