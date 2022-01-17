//@{ Jaewon 20040618
// created
// Quake3 style dynamic light map rendering module for terrain.
//@} Jaewon

//@{ Jaewon 20041118
#include <algorithm>
//@} Jaewon

#include "AgcmDynamicLightmap.h"

#include "ApMemoryTracker.h"

// vertex shader object code
const DWORD g_vs11_lightmap[] =
{
    0xfffe0101, 0x0000001f, 0x80000000, 0x900f0000, 0x0000001f, 0x8000000a, 
    0x900f0001, 0x0000001f, 0x80000005, 0x900f0002, 0x00000014, 0xc00f0000, 
    0x90e40000, 0xa0e40000, 0x00000001, 0xd00f0000, 0x90e40001, 0x00000001, 
    0xe00f0000, 0x90e40002, 0x0000ffff
};

struct LIGHTMAP_VERTEX {
	FLOAT x, y, z;
	DWORD color;
	FLOAT u,v;
};

static const FLOAT cOffsetY = 3.0f;
static CONST INT32 cNonmovableViewSectorRange = 2;

AgcmDynamicLightmap::AgcmDynamicLightmap() 
					: m_pLightMap(NULL), m_pAgcmRender(NULL), 
					m_pAgcmMap(NULL), m_pApmOcTree(NULL),
					m_pAgcmOcTree(NULL), m_bOn(TRUE),
					m_pVShader(NULL), m_bUseVShader(FALSE), m_pVDeclaration(NULL), m_pVBuffer(NULL)
{
	SetModuleName("AgcmDynamicLightmap");

	EnableIdle(TRUE);

	light1 = light2 = NULL;
}

AgcmDynamicLightmap::~AgcmDynamicLightmap()
{
	OnDestroy();
}

BOOL AgcmDynamicLightmap::OnAddModule()
{
	m_pAgcmRender = (AgcmRender*)GetModule("AgcmRender");

	if(!m_pAgcmRender)
	{
		ASSERT(!"Can't get the AgcmRender module in AgcmDynamicLightmap::OnAddModule()!");
		return FALSE;
	}

	if(!m_pAgcmRender->SetCallbackPostRender(postRenderCB, this))
	{
		ASSERT(!"Can't set postRenderCB callback in AgcmDynamicLightmap::OnAddModule()!");
		return FALSE;
	}

	m_pAgcmMap = (AgcmMap*)GetModule("AgcmMap");

	if(!m_pAgcmMap)
	{
		ASSERT(!"Can't get the AgcmMap module in AgcmDynamicLightmap::OnAddModule()!");
		return FALSE;
	}

	m_pAgcmOcTree = (AgcmOcTree*)GetModule("AgcmOcTree");

	if(!m_pAgcmOcTree)
	{
		ASSERT(!"Can't get the AgcmOcTree module in AgcmDynamicLightmap::OnAddModule()!");
		return FALSE;
	}

	m_pApmOcTree = (ApmOcTree*)GetModule("ApmOcTree");

	if(!m_pApmOcTree)
	{
		ASSERT(!"Can't get the ApmOcTree module in AgcmDynamicLightmap::OnAddModule()!");
		return FALSE;
	}
	return TRUE;
}

BOOL AgcmDynamicLightmap::OnInit()
{
	m_pLightMap = CreateLightmap(512);
    if(m_pLightMap == NULL)
	{
		ASSERT(!"Can't create a light map texture in AgcmDynamicLightmap::OnInit()!");
		return FALSE;    
	}

	//. 2006. 3. 3. Nonstopdj
	//. D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED flag로 생성된 정적인 버퍼로 런타임에 lock/unlock하고 있었다.
	//. 이럴 경우 시스템메모리에 있는 버퍼를 이용하여 DrawPrimitiveUP하는 것보다 느릴 수 있다.
	//. 그러므로 AcuFrameMemory를 이용하여 라이트맵을 그려준다. 적잖은 프레임향상을 가져왔음 (^^)V
	m_bUseVShader = FALSE;

	//const D3DCAPS9* pCaps = (const D3DCAPS9*)RwD3D9GetCaps();
	//if((pCaps->VertexShaderVersion & 0xffff) >= 0x0101)
	//{
	//	// enable shader.
	//	m_bUseVShader = TRUE;

	//	// create a shader.
	//	RwBool hr;
	//	hr = RwD3D9CreateVertexShader((RwUInt32 *)g_vs11_lightmap, &m_pVShader);
	//	ASSERT(hr);

	//	// create a vertex declaration.
	//	D3DVERTEXELEMENT9 lightmapVertexFormat[] = {
	//		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, 
	//										D3DDECLUSAGE_POSITION, 0},
	//		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, 
	//										D3DDECLUSAGE_COLOR, 0},
	//		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,
	//										D3DDECLUSAGE_TEXCOORD, 0},
	//		D3DDECL_END()
	//	};
	//	hr = RwD3D9CreateVertexDeclaration(lightmapVertexFormat, &m_pVDeclaration);
	//	ASSERT(hr);

	//	// create a vertex buffer.
	//	UINT32 offset = 0;
	//	hr = RwD3D9CreateVertexBuffer(sizeof(LIGHTMAP_VERTEX), sizeof(LIGHTMAP_VERTEX)*100*100*6, &m_pVBuffer, &offset);
	//	ASSERT(hr);
	//}

	return TRUE;
}

BOOL AgcmDynamicLightmap::OnDestroy()
{	
	// delete the vertex buffer.
	if(m_pVBuffer)
	{
		RwD3D9DestroyVertexBuffer(sizeof(LIGHTMAP_VERTEX), sizeof(LIGHTMAP_VERTEX)*100*100*6, m_pVBuffer, 0);
		m_pVBuffer = NULL;
	}

	// delete the vertex declaration.
	if(m_pVDeclaration)
	{
		RwD3D9DeleteVertexDeclaration(m_pVDeclaration);
		m_pVDeclaration = NULL;
	}

	// delete the shader.
	if(m_pVShader)
	{
		RwD3D9DeleteVertexShader(m_pVShader);
		m_pVShader = NULL;
	}
	
	if(m_pLightMap)
	{
		RwTextureDestroy(m_pLightMap);
		m_pLightMap = NULL;
	}

	clearLights();

	return TRUE;
}

BOOL AgcmDynamicLightmap::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmDynamicLightmap::OnIdle");

	LockFrame();
	for(int i=0; i< ( int ) m_lights.size(); ++i)
	{
		if(m_lights[i]->bMovable_ == FALSE)
			continue;

		// update the bounding sphere.
		RwV3dAssign(&(m_lights[i]->sphere_.center), 
			RwMatrixGetPos(RwFrameGetLTM((RpLightGetFrame(m_lights[i]->pLight_)))));
		m_lights[i]->sphere_.radius = RpLightGetRadius(m_lights[i]->pLight_);
	}
	UnlockFrame();

	
	// Test
/*	LockFrame();
	RwFrame*	f;

	if(light1)
	{
	removeLight(light1);
	m_pAgcmRender->RemoveLightFromRenderWorld( light1,FALSE );
	f	= RpLightGetFrame	( light1 );
	RpLightSetFrame		( light1 , NULL );
	RwFrameDestroy		( f );
	RpLightDestroy		( light1 );
	}

	if(light2)
	{
	removeLight(light2);
	m_pAgcmRender->RemoveLightFromRenderWorld( light2,FALSE );
	f	= RpLightGetFrame	( light2 );
	RpLightSetFrame		( light2 , NULL );
	RwFrameDestroy		( f );
	RpLightDestroy		( light2 );
	}

	light1 = RpLightCreate(rpLIGHTSPOT);
	light2 = RpLightCreate(rpLIGHTPOINT);

	f = RwFrameCreate();
    RpLightSetFrame( light1 , f);

	RwV3d v;
	RwV3d xaxis = { 1.0f , 0.0f , 0.0f };
	RwRGBAReal color = {1.0f, 0.0f, 0.0f, 1.0f};
	RwMatrix*	mat;

	mat = RwFrameGetLTM(m_pAgcmRender->m_pFrame);
	v.x = mat->pos.x - 10.0f;
	v.y = mat->pos.y + 1000.0f;
	v.z = mat->pos.z - 10.0f;

	RwFrameRotate( f , & xaxis , 90.0f , rwCOMBINEREPLACE );
	RwFrameTranslate( f , & v , rwCOMBINEPOSTCONCAT );

	RpLightSetColor(light1, &color);

	RpLightSetRadius(light1,5000.0f);
	RpLightSetConeAngle( light1 , 15.0f * 3.141592f / 180.0f );
	
	f = RwFrameCreate();
    RpLightSetFrame( light2 , f);

	v.x = mat->pos.x + 10.0f;
	v.y = mat->pos.y + 700.0f;
	v.z = mat->pos.z + 10.0f;
	
	RwFrameRotate( f , & xaxis , 90.0f , rwCOMBINEREPLACE );
	RwFrameTranslate( f , & v , rwCOMBINEPOSTCONCAT );

	color.red = 0.0f;
	color.blue = 1.0f;
	RpLightSetColor(light2, &color);

	RpLightSetRadius(light2,1000.0f);
	
	UnlockFrame();

	m_pAgcmRender->AddLightToRenderWorld(light1,FALSE);
	m_pAgcmRender->AddLightToRenderWorld(light2,FALSE);
	addLight(light1);
	addLight(light2);
	for(i=0; i<m_lights.size(); ++i)
	{
		m_lights[i]->bVisible_ = TRUE;
	}*/

	return TRUE;
}

BOOL AgcmDynamicLightmap::addLight(RpLight* pLight, BOOL bMovable) 
{
	// preconditions
	RpLightType type = RpLightGetType(pLight);
	ASSERT(type == rpLIGHTPOINT || type == rpLIGHTSPOT || type == rpLIGHTSPOTSOFT);
	if(type != rpLIGHTPOINT && type != rpLIGHTSPOT && type != rpLIGHTSPOTSOFT)
		return FALSE;

	// check whether there is the light already in the list.
	for(int i=0; i< ( int ) m_lights.size(); ++i)
		if(m_lights[i]->pLight_ == pLight)
		{
			ASSERT(!"It is already in the lightmap light list!");
			return FALSE;
		}

	// add its info.
	LightInfo* pInfo = new LightInfo();
	pInfo->pLight_ = pLight;
	pInfo->bMovable_ = bMovable;
	
	LockFrame();
	RwV3dAssign(&(pInfo->sphere_.center), 
		RwMatrixGetPos(RwFrameGetLTM((RpLightGetFrame(pInfo->pLight_)))));
	UnlockFrame();

	pInfo->sphere_.radius = RpLightGetRadius(pInfo->pLight_);
	m_lights.push_back(pInfo); 

	// add the light to the world.
	RpWorldAddLight(GetWorld(), pLight);

	// register it as a custom render object.
	if(bMovable)
	{
		m_pAgcmRender->AddCustomRenderObject(this, &(pInfo->sphere_), pInfo->sphere_.radius,
													customUpdateCB, customRenderCB,
													(PVOID)pInfo, NULL);	
	}
	else
	{
		if(m_pApmOcTree->m_bOcTreeEnable)
		{
			OcCustomDataList	param;
			param.BS = pInfo->sphere_;
			param.pData1 = (PVOID)pInfo;
			param.pData2 = NULL;
			param.iAppearanceDistance = cNonmovableViewSectorRange;
			param.pClass = this;
			param.pRenderCB = customUpdateCB;
			param.pUpdateCB = customRenderCB;
			//@{ 2006/11/17 burumal
			param.pDistCorrectCB = NULL;
			//@}
			param.piCameraZIndex = &(pInfo->iCamZ_);

			param.TopVerts[0].x = param.BS.center.x - param.BS.radius;
			param.TopVerts[0].y = param.BS.center.y + param.BS.radius;
			param.TopVerts[0].z = param.BS.center.z - param.BS.radius;

			param.TopVerts[1].x = param.BS.center.x + param.BS.radius;
			param.TopVerts[1].y = param.BS.center.y + param.BS.radius;
			param.TopVerts[1].z = param.BS.center.z - param.BS.radius;

			param.TopVerts[2].x = param.BS.center.x + param.BS.radius;
			param.TopVerts[2].y = param.BS.center.y + param.BS.radius;
			param.TopVerts[2].z = param.BS.center.z + param.BS.radius;

			param.TopVerts[3].x = param.BS.center.x - param.BS.radius;
			param.TopVerts[3].y = param.BS.center.y + param.BS.radius;
			param.TopVerts[3].z = param.BS.center.z + param.BS.radius;

			m_pAgcmOcTree->AddCustomRenderDataToOcTree(pInfo->sphere_.center.x,
															pInfo->sphere_.center.y,
															pInfo->sphere_.center.z, &param);
		}
		else
		{
			m_pAgcmRender->AddCustomRenderToSector(this, &(pInfo->sphere_), 
															customUpdateCB, customRenderCB,
															(PVOID)pInfo, NULL);
		}
	}

	return TRUE;
}

BOOL AgcmDynamicLightmap::removeLight(RpLight* pLight) 
{
	// preconditions
	RpLightType type = RpLightGetType(pLight);
	ASSERT(type == rpLIGHTPOINT || type == rpLIGHTSPOT || type == rpLIGHTSPOTSOFT);
	if(type != rpLIGHTPOINT && type != rpLIGHTSPOT && type != rpLIGHTSPOTSOFT)
		return FALSE;

	// find & remove.
	std::vector<LightInfo*>::iterator itr;
	for(itr=m_lights.begin(); itr!=m_lights.end(); ++itr)
	{
		if((*itr)->pLight_ == pLight)
		{
			if((*itr)->bMovable_)
			{
				m_pAgcmRender->RemoveCustomRenderObject(this, (PVOID)(*itr), NULL);	
			}
			else
			{
				if(m_pApmOcTree->m_bOcTreeEnable)
				{
					m_pAgcmOcTree->RemoveCustomRenderDataFromOcTree((*itr)->sphere_.center.x,
																	(*itr)->sphere_.center.y,
																	(*itr)->sphere_.center.z, 
																	this, (PVOID)(*itr), NULL);
				}
				else
				{
					m_pAgcmRender->RemoveCustomRenderFromSector(this, &((*itr)->sphere_), (PVOID)(*itr), NULL);
				}

			}
			RpWorldRemoveLight(GetWorld(), pLight);
			delete *itr;
			itr = m_lights.erase(itr);
			return TRUE;
		}
	}

	return FALSE;
}

void AgcmDynamicLightmap::clearLights()
{
	for(int i=0; i< ( int ) m_lights.size(); ++i)
	{
		if(m_lights[i]->bMovable_)
		{
			m_pAgcmRender->RemoveCustomRenderObject(this, (PVOID)m_lights[i], NULL);	
		}
		else
		{
			if(m_pApmOcTree->m_bOcTreeEnable)
			{
				m_pAgcmOcTree->RemoveCustomRenderDataFromOcTree(m_lights[i]->sphere_.center.x,
																m_lights[i]->sphere_.center.y,
																m_lights[i]->sphere_.center.z,
																this, (PVOID)m_lights[i], NULL);
			}
			else
			{
				m_pAgcmRender->RemoveCustomRenderFromSector(this, &(m_lights[i]->sphere_), (PVOID)m_lights[i], NULL);
			}

		}
		RpWorldRemoveLight(GetWorld(), m_lights[i]->pLight_);
		delete m_lights[i];
	}

	m_lights.clear();
}

BOOL AgcmDynamicLightmap::turnOnLight(RpLight* pLight)
{
	// preconditions
	RpLightType type = RpLightGetType(pLight);
	ASSERT(type == rpLIGHTPOINT || type == rpLIGHTSPOT || type == rpLIGHTSPOTSOFT);
	if(type != rpLIGHTPOINT && type != rpLIGHTSPOT && type != rpLIGHTSPOTSOFT)
		return FALSE;

	// find & turn on.
	std::vector<LightInfo*>::iterator itr;
	for(itr=m_lights.begin(); itr!=m_lights.end(); ++itr)
	{
		if((*itr)->pLight_ == pLight)
		{
			(*itr)->bOn_ = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmDynamicLightmap::turnOffLight(RpLight* pLight)
{
	// preconditions
	RpLightType type = RpLightGetType(pLight);
	ASSERT(type == rpLIGHTPOINT || type == rpLIGHTSPOT || type == rpLIGHTSPOTSOFT);
	if(type != rpLIGHTPOINT && type != rpLIGHTSPOT && type != rpLIGHTSPOTSOFT)
		return FALSE;

	// find & turn off.
	std::vector<LightInfo*>::iterator itr;
	for(itr=m_lights.begin(); itr!=m_lights.end(); ++itr)
	{
		if((*itr)->pLight_ == pLight)
		{
			(*itr)->bOn_ = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

// render-a-light functor
struct renderImpl : public std::unary_function<const RpLight*, void>
{
	renderImpl(AgcmDynamicLightmap* pThis) : m_pThis(pThis) {}
	void operator() (AgcmDynamicLightmap::LightInfo* pInfo) 
	{
		if(pInfo->bOn_ == FALSE
		|| pInfo->bVisible_ == FALSE
		|| pInfo->iCamZ_ > 3) 
			return;

		if(RpLightGetType(pInfo->pLight_) == rpLIGHTPOINT)
		{
			renderPoint(pInfo->pLight_);
		}
		else
		{
			renderSpot(pInfo->pLight_);
		}

		// turn off the visibility flag.
		pInfo->bVisible_ = FALSE;
	}

	void renderPoint(const RpLight* pLight)
	{
		PROFILE("renderImpl::renderPoint");

		INT32 i, j, k, tri;
		RwV3d tmp;
		const RwRGBAReal* col = RpLightGetColor(pLight);

		// get the illumination aabb from the point light's sphere.
		RwBBox aabb;
		FLOAT radius = RpLightGetRadius(pLight);
		m_pThis->LockFrame();
		RwV3d center = *RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(pLight)));
		m_pThis->UnlockFrame();
		RwV3d diagonal;
		diagonal.x = diagonal.y = diagonal.z = radius*1.732f/* root 3 */;
		RwV3dSub(&aabb.inf, &center, &diagonal);
		RwV3dAdd(&aabb.sup, &center, &diagonal);
		
		// get the region & its heights.
		FLOAT afHeights[100][100];
		RwRect region;
		region.x = INT32(aabb.inf.x/MAP_STEPSIZE)-1;
		region.y = INT32(aabb.inf.z/MAP_STEPSIZE)-1;
		region.w = INT32(aabb.sup.x/MAP_STEPSIZE)+1 - region.x;
		region.h = INT32(aabb.sup.z/MAP_STEPSIZE)+1 - region.y;
		
		ASSERT(region.w>0 && region.w<100
			&& region.h>0 && region.h<100);

		//@{ Jaewon 20050204
		// If it is out of range, just return.
		if(!(region.w>0 && region.w<100
			&& region.h>0 && region.h<100))
			return;
		//@} Jaewon

		for(i=0; i<=region.h; ++i)
		{
			for(j=0; j<=region.w; ++j)
			{
				//@{ kday 20050117
				//for only terrain height
				//afHeights[j][i] = m_pThis->GetMapModule()->GetHeight(FLOAT((j+region.x)*MAP_STEPSIZE),
				//												FLOAT((i+region.y)*MAP_STEPSIZE)); 
				afHeights[j][i] = m_pThis->GetMapModule()->HP_GetHeightGeometryOnly(FLOAT((j+region.x)*MAP_STEPSIZE),
																					FLOAT((i+region.y)*MAP_STEPSIZE)); 
				//@} kday
			}
		}
	
		INT32 allocated;
		RwIm3DVertex* pVertexArray;
		LIGHTMAP_VERTEX* pVertexBuffer;
		if(m_pThis->m_bUseVShader == FALSE)
		{
		// allocate a buffer.
		allocated = sizeof(RwIm3DVertex)*region.w*region.h*6/* 2 tri */;
		pVertexArray 
			= (RwIm3DVertex*)AcuFrameMemory::AllocFrameMemory(allocated);

		//@{ 2006/07/12 nonstopdj
		if ( !pVertexArray )
			return;
		//@}
		}
		else
		{
		UINT32 offset = 0;
		pVertexBuffer;
		HRESULT hr = ((LPDIRECT3DVERTEXBUFFER9)(m_pThis->m_pVBuffer))->Lock(0, 
										sizeof(LIGHTMAP_VERTEX)*region.w*region.h*6, 
										(void**)&pVertexBuffer, 0);
		ASSERT(hr==D3D_OK);
		}
	
		// calculate texture coordinates & fill the vertex buffer
		// according to the aabb test result.
		UINT numVerts=0;

		RwV3d pos[4], fnormal, edge[2], atCam, tangent, binormal, projected, lv;
		RwV2d uv[3];
		RwBBox aabbTile;
		RwRGBA rgba[3];

		RwCamera*	pCam = m_pThis->GetCamera();
		m_pThis->LockFrame();
		RwMatrix*	pCamLTM = RwFrameGetLTM(RwCameraGetFrame(pCam));
		m_pThis->UnlockFrame();
		
		for(i=0; i<region.h; ++i)
		{
			for(j=0; j<region.w; ++j)
			{
				pos[0].x = FLOAT((j+region.x)*MAP_STEPSIZE);
				pos[0].z = FLOAT((i+region.y)*MAP_STEPSIZE);
				pos[0].y = afHeights[j][i] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				pos[1].x = FLOAT((j+1+region.x)*MAP_STEPSIZE);
				pos[1].z = FLOAT((i+region.y)*MAP_STEPSIZE);
				pos[1].y = afHeights[j+1][i] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				pos[2].x = FLOAT((j+1+region.x)*MAP_STEPSIZE);
				pos[2].z = FLOAT((i+1+region.y)*MAP_STEPSIZE);
				pos[2].y = afHeights[j+1][i+1] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				pos[3].x = FLOAT((j+region.x)*MAP_STEPSIZE);
				pos[3].z = FLOAT((i+1+region.y)*MAP_STEPSIZE);
				pos[3].y = afHeights[j][i+1] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				// get the tile aabb.
				RwBBoxInitialize(&aabbTile, &pos[0]);
				RwBBoxAddPoint(&aabbTile, &pos[1]);
				RwBBoxAddPoint(&aabbTile, &pos[2]);
				RwBBoxAddPoint(&aabbTile, &pos[3]);

				// test against the light aabb.
				if((aabb.inf.y>=aabbTile.sup.y) || (aabb.sup.y<=aabbTile.inf.y))
				// skip this tile.
				{
					continue;
				}
				
				UINT32 iTri[] = { 0, 1, 3,
								1, 2, 3 };

				for(tri=0; tri<2; ++tri)
				// for each triangle in a rect,
				{
				// compute the face normal.
				RwV3dSub(&edge[0], &pos[iTri[3*tri+2]], &pos[iTri[3*tri+0]]);
				RwV3dSub(&edge[1], &pos[iTri[3*tri+1]], &pos[iTri[3*tri+0]]);
				RwV3dCrossProduct(&fnormal, &edge[0], &edge[1]);
				RwV3dNormalize(&fnormal, &fnormal);

				// get the camera vector for culling back faces.
				RwV3dAssign(&atCam, RwMatrixGetPos(pCamLTM));
				RwV3dSub(&atCam, &pos[iTri[3*tri+0]], &atCam);

				// get the light vector.
				RwV3dSub(&lv, &pos[iTri[3*tri+0]], &center);

				// cull the back face.
				if(RwV3dDotProduct(&fnormal, &atCam) < 0.0f
					&& RwV3dDotProduct(&fnormal, &lv) < 0.0f)
				{
					// compute the texture coordinate.
					RwV3dAssign(&tangent, &edge[0]);
					RwV3dNormalize(&tangent, &tangent);
					RwV3dCrossProduct(&binormal, &fnormal, &tangent);
					RwV3dAssign(&projected, &center);
					RwV3dSub(&tmp, &pos[iTri[3*tri+0]], &center);
					FLOAT perpDist = RwV3dDotProduct(&fnormal, &tmp);
					ASSERT(perpDist<=0);
					RwV3dIncrementScaled(&projected, &fnormal, perpDist);
					if(radius<=-perpDist) break;
					FLOAT curR = sqrtf(radius*radius - perpDist*perpDist);
					FLOAT rcpOfR = 1.0f/curR;
					RwV3dScale(&tangent, &tangent, rcpOfR);
					RwV3dScale(&binormal, &binormal, rcpOfR);
					for(k=0; k<3; ++k)
					{
						RwV3dSub(&tmp, &pos[iTri[3*tri+k]], &projected);
						uv[k].x = (RwV3dDotProduct(&tmp, &tangent)+1.0f)/2.0f;
						uv[k].y = (RwV3dDotProduct(&tmp, &binormal)+1.0f)/2.0f;
					}

					// compute the color.
					for(k=0; k<3; ++k)
					{
						RwV3dSub(&tmp, &pos[iTri[3*tri+k]], &center);
						FLOAT d = RwV3dDotProduct(&tmp, &tmp)/(radius*radius);
						FLOAT fRatio;
						if(d>=1) 
							fRatio = 1.0f;
						else
							fRatio = m_pThis->GetAttenuation(sqrtf(d))
									/ m_pThis->GetAttenuation(sqrtf((2.0f*uv[k].x-1.0f)*(2.0f*uv[k].x-1.0f) + 
															(2.0f*uv[k].y-1.0f)*(2.0f*uv[k].y-1.0f)));
						//ASSERT(fRatio>=0.0f && fRatio<=1.01f);
						if(fRatio>1.0f)
							fRatio = 1.0f;
						rgba[k].red = UINT8(255.0f*fRatio*col->red);
						rgba[k].green = UINT8(255.0f*fRatio*col->green);
						rgba[k].blue = UINT8(255.0f*fRatio*col->blue);
						rgba[k].alpha = UINT8(255.0f*fRatio);
					}

					if(m_pThis->m_bUseVShader == FALSE)
					{
						// set the vertex array.
						RwIm3DVertex* pVertex = pVertexArray+numVerts;
						for(k=0; k<3; ++k)
						{
						RwIm3DVertexSetPos(pVertex+k, pos[iTri[3*tri+k]].x, pos[iTri[3*tri+k]].y, pos[iTri[3*tri+k]].z);
						RwIm3DVertexSetNormal(pVertex+k, fnormal.x, fnormal.y, fnormal.z);
						RwIm3DVertexSetU(pVertex+k, uv[k].x);
						RwIm3DVertexSetV(pVertex+k, uv[k].y);
						RwIm3DVertexSetRGBA(pVertex+k, rgba[k].red, rgba[k].green, rgba[k].blue, rgba[k].alpha);
						}
					}
					else
					{
						// set the vertex buffer.
						LIGHTMAP_VERTEX* pVertex = pVertexBuffer+numVerts;
						for(k=0; k<3; ++k)
						{
						(pVertex+k)->x = pos[iTri[3*tri+k]].x;
						(pVertex+k)->y = pos[iTri[3*tri+k]].y;
						(pVertex+k)->z = pos[iTri[3*tri+k]].z;
						(pVertex+k)->u = uv[k].x;
						(pVertex+k)->v = uv[k].y;
						(pVertex+k)->color = 0xff000000 | (rgba[k].red << 16) | (rgba[k].green << 8) | rgba[k].blue;
						}
					}
					
					numVerts+=3;
				}
				}
			}
		}

		if(m_pThis->m_bUseVShader)
		{
			((LPDIRECT3DVERTEXBUFFER9)(m_pThis->m_pVBuffer))->Unlock();
		}
		
		if(numVerts > 0)
		// render triangles.
		{
			if(m_pThis->m_bUseVShader == FALSE)
			{
			if(RwIm3DTransform(pVertexArray, numVerts, NULL,
							rwIM3D_VERTEXUV | 
							rwIM3D_VERTEXXYZ | 
							rwIM3D_VERTEXRGBA | 
							rwIM3D_NOCLIP))
			{
				RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
				RwIm3DEnd();
			}
			}
			else
			{
			RwD3D9SetStreamSource(0, m_pThis->m_pVBuffer, 0, sizeof(LIGHTMAP_VERTEX));
			RwD3D9DrawPrimitive(D3DPT_TRIANGLELIST, 0, numVerts/3);	
			}
		}
		
		if(m_pThis->m_bUseVShader == FALSE)

		{
			// release a buffer.
			AcuFrameMemory::DeallocFrameMemory(allocated);
		}
	}

	void renderSpot(const RpLight* pLight)
	{
		PROFILE("renderImpl::renderSpot");

		INT32 i, j, k, tri;
		RwV3d unitX, unitZ;
		const RwRGBAReal* col = RpLightGetColor(pLight);
		
		// get the illumination aabb from the spot light's cone.
		RwBBox aabb;
		FLOAT radius = RpLightGetRadius(pLight);
		m_pThis->LockFrame();
		RwV3d center = *RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(pLight)));
		RwV3d at = *RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(pLight)));
		m_pThis->UnlockFrame();
		RwV3dNormalize(&at, &at);
		FLOAT coneAngle = RpLightGetConeAngle(pLight);
		ASSERT(coneAngle<=3.141592f/2.0f);
		RwBBoxInitialize(&aabb, &center);
		// calculate 4 points of the cone base rect
		// and use them to expand the aabb.
		RwV3d coneBase[4];
		RwV3d centerOfBase, baseX, baseZ, tmp;
		FLOAT radiusOfBase;
		// center of the cone base circle
		RwV3dAssign(&centerOfBase, &center);
		RwV3dIncrementScaled(&centerOfBase, &at, radius);
		// radius of the cone base circle
		radiusOfBase = radius*tanf(coneAngle);
		// x axis of the base rect(arbitrary vector perpendicular to the at vector).
		tmp.x = tmp.y = tmp.z = 0.0f;
		if(fabs(at.x) > fabs(at.y) &&  fabs(at.x) > fabs(at.z)) tmp.z = 1.0f;
		else tmp.x = 1.0f;
		RwV3dCrossProduct(&baseX, &at, &tmp);
		RwV3dNormalize(&unitX, &baseX);
		RwV3dScale(&baseX, &unitX, radiusOfBase);
		// z axis of the base rect.
		RwV3dCrossProduct(&unitZ, &unitX, &at);
		RwV3dScale(&baseZ, &unitZ, radiusOfBase);
		// 4 points
		RwV3dAdd(&coneBase[0], &centerOfBase, &baseX);
		RwV3dAdd(&coneBase[0], &coneBase[0], &baseZ);
		RwV3dAdd(&coneBase[1], &centerOfBase, &baseX);
		RwV3dSub(&coneBase[1], &coneBase[1], &baseZ);
		RwV3dSub(&coneBase[2], &centerOfBase, &baseX);
		RwV3dSub(&coneBase[2], &coneBase[2], &baseZ);
		RwV3dSub(&coneBase[3], &centerOfBase, &baseX);
		RwV3dAdd(&coneBase[3], &coneBase[3], &baseZ);
		// expand the aabb.
		RwBBoxAddPoint(&aabb, &coneBase[0]);
		RwBBoxAddPoint(&aabb, &coneBase[1]);
		RwBBoxAddPoint(&aabb, &coneBase[2]);
		RwBBoxAddPoint(&aabb, &coneBase[3]);

		// get the region & its heights.
		FLOAT afHeights[100][100];
		RwRect region;
		region.x = INT32(aabb.inf.x/MAP_STEPSIZE)-1;
		region.y = INT32(aabb.inf.z/MAP_STEPSIZE)-1;
		region.w = INT32(aabb.sup.x/MAP_STEPSIZE)+1 - region.x;
		region.h = INT32(aabb.sup.z/MAP_STEPSIZE)+1 - region.y;
		
		ASSERT(region.w>0 && region.w<100
			&& region.h>0 && region.h<100);

		//@{ Jaewon 20050204
		// If it is out of range, just return.
		if(!(region.w>0 && region.w<100
			&& region.h>0 && region.h<100))
			return;
		//@} Jaewon

		for(i=0; i<=region.h; ++i)
		{
			for(j=0; j<=region.w; ++j)
			{
				afHeights[j][i] = m_pThis->GetMapModule()->GetHeight(FLOAT((j+region.x)*MAP_STEPSIZE),
																FLOAT((i+region.y)*MAP_STEPSIZE) , SECTOR_MAX_HEIGHT); 
			}
		}
	
		INT32 allocated;
		RwIm3DVertex* pVertexArray;
		LIGHTMAP_VERTEX* pVertexBuffer;
		if(m_pThis->m_bUseVShader == FALSE)
		{
		// allocate a buffer.
		allocated = sizeof(RwIm3DVertex)*region.w*region.h*6/* 2 tri */;
		pVertexArray 
			= (RwIm3DVertex*)AcuFrameMemory::AllocFrameMemory(allocated);

		//@{ 2006/07/12 nonstopdj
		if ( !pVertexArray )
			return;
		//@}
		}
		else
		{
		UINT32 offset = 0;
		pVertexBuffer;
		HRESULT hr = ((LPDIRECT3DVERTEXBUFFER9)(m_pThis->m_pVBuffer))->Lock(0, 
										sizeof(LIGHTMAP_VERTEX)*region.w*region.h*6, 
										(void**)&pVertexBuffer, 0);
		ASSERT(hr==D3D_OK);
		}
	
		// calculate texture coordinates & fill the vertex buffer
		// according to the aabb test result.
		UINT numVerts=0;

		RwV3d pos[4], fnormal, edge[2], atCam, lv;
		RwV2d uv[3];
		RwBBox aabbTile;
		RwRGBA rgba[3];

		RwCamera*	pCam = m_pThis->GetCamera();
		m_pThis->LockFrame();
		RwMatrix*	pCamLTM = RwFrameGetLTM(RwCameraGetFrame(pCam));
		m_pThis->UnlockFrame();
		
		for(i=0; i<region.h; ++i)
		{
			for(j=0; j<region.w; ++j)
			{
				pos[0].x = FLOAT((j+region.x)*MAP_STEPSIZE);
				pos[0].z = FLOAT((i+region.y)*MAP_STEPSIZE);
				pos[0].y = afHeights[j][i] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				pos[1].x = FLOAT((j+1+region.x)*MAP_STEPSIZE);
				pos[1].z = FLOAT((i+region.y)*MAP_STEPSIZE);
				pos[1].y = afHeights[j+1][i] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				pos[2].x = FLOAT((j+1+region.x)*MAP_STEPSIZE);
				pos[2].z = FLOAT((i+1+region.y)*MAP_STEPSIZE);
				pos[2].y = afHeights[j+1][i+1] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				pos[3].x = FLOAT((j+region.x)*MAP_STEPSIZE);
				pos[3].z = FLOAT((i+1+region.y)*MAP_STEPSIZE);
				pos[3].y = afHeights[j][i+1] + cOffsetY;//(m_pThis->m_bUseVShader?0.0f:cOffsetY);

				// get the tile aabb.
				RwBBoxInitialize(&aabbTile, &pos[0]);
				RwBBoxAddPoint(&aabbTile, &pos[1]);
				RwBBoxAddPoint(&aabbTile, &pos[2]);
				RwBBoxAddPoint(&aabbTile, &pos[3]);

				// test against the light aabb.
				if((aabb.inf.y>=aabbTile.sup.y) || (aabb.sup.y<=aabbTile.inf.y))
				// skip this tile.
				{
					continue;
				}
				
				UINT32 iTri[] = { 0, 1, 3,
								1, 2, 3 };

				for(tri=0; tri<2; ++tri)
				// for each triangle in a rect,
				{
				// compute the face normal.
				RwV3dSub(&edge[0], &pos[iTri[3*tri+2]], &pos[iTri[3*tri+0]]);
				RwV3dSub(&edge[1], &pos[iTri[3*tri+1]], &pos[iTri[3*tri+0]]);
				RwV3dCrossProduct(&fnormal, &edge[0], &edge[1]);
				RwV3dNormalize(&fnormal, &fnormal);

				// get the camera vector for culling back faces.
				RwV3dAssign(&atCam, RwMatrixGetPos(pCamLTM));
				RwV3dSub(&atCam, &pos[iTri[3*tri+0]], &atCam);

				// get the light vector.
				RwV3dSub(&lv, &pos[iTri[3*tri+0]], &center);

				// cull the back face.
				if(RwV3dDotProduct(&fnormal, &atCam) < 0.0f
					&& RwV3dDotProduct(&fnormal, &lv) < 0.0f)				
				{
					RwV3dSub(&tmp, &pos[iTri[3*tri+0]], &center);
					FLOAT perpDist = RwV3dDotProduct(&fnormal, &tmp);
					ASSERT(perpDist<=0);
					if(radius<=-perpDist) break;

					// compute the texture coordinate & color.
					for(k=0; k<3; ++k)
					{
						RwV3dSub(&tmp, &pos[iTri[3*tri+k]], &center);
						FLOAT atDist = RwV3dDotProduct(&at, &tmp);
						if(atDist<0.001f) atDist = 0.001f;
						ASSERT(radius>0);
						FLOAT curR = radiusOfBase*atDist/radius;
						uv[k].x = (RwV3dDotProduct(&unitX, &tmp)/curR+1.0f)/2.0f;
						uv[k].y = (RwV3dDotProduct(&unitZ, &tmp)/curR+1.0f)/2.0f;
						FLOAT fRatio;
						if(atDist>=radius) 
							fRatio = 0.0f;
						else
							fRatio = m_pThis->GetAttenuation(atDist/radius);
						rgba[k].red = UINT8(255.0f*fRatio*col->red);
						rgba[k].green = UINT8(255.0f*fRatio*col->green);
						rgba[k].blue = UINT8(255.0f*fRatio*col->blue);
						rgba[k].alpha = UINT8(255.0f*fRatio);
					}

					if(m_pThis->m_bUseVShader == FALSE)
					{
						// set the vertex array.
						RwIm3DVertex* pVertex = pVertexArray+numVerts;
						for(k=0; k<3; ++k)
						{
						RwIm3DVertexSetPos(pVertex+k, pos[iTri[3*tri+k]].x, pos[iTri[3*tri+k]].y, pos[iTri[3*tri+k]].z);
						RwIm3DVertexSetNormal(pVertex+k, fnormal.x, fnormal.y, fnormal.z);
						RwIm3DVertexSetU(pVertex+k, uv[k].x);
						RwIm3DVertexSetV(pVertex+k, uv[k].y);
						RwIm3DVertexSetRGBA(pVertex+k, rgba[k].red, rgba[k].green, rgba[k].blue, rgba[k].alpha);
						}
					}
					else
					{
						// set the vertex buffer.
						LIGHTMAP_VERTEX* pVertex = pVertexBuffer+numVerts;
						for(k=0; k<3; ++k)
						{
						(pVertex+k)->x = pos[iTri[3*tri+k]].x;
						(pVertex+k)->y = pos[iTri[3*tri+k]].y;
						(pVertex+k)->z = pos[iTri[3*tri+k]].z;
						(pVertex+k)->u = uv[k].x;
						(pVertex+k)->v = uv[k].y;
						(pVertex+k)->color = 0xff000000 | (rgba[k].red << 16) | (rgba[k].green << 8) | rgba[k].blue;
						}
					}
					
					numVerts+=3;
				}
				}
			}
		}
		
		if(numVerts > 0)
		// render triangles.
		{
			if(m_pThis->m_bUseVShader == FALSE)
			{
			if(RwIm3DTransform(pVertexArray, numVerts, NULL,
							rwIM3D_VERTEXUV | 
							rwIM3D_VERTEXXYZ | 
							rwIM3D_VERTEXRGBA | 
							rwIM3D_NOCLIP))
			{
				RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
				RwIm3DEnd();
			}
			}
			else
			{
			RwD3D9SetStreamSource(0, m_pThis->m_pVBuffer, 0, sizeof(LIGHTMAP_VERTEX));
			RwD3D9DrawPrimitive(D3DPT_TRIANGLELIST, 0, numVerts/3);	
			}
		}

		if(m_pThis->m_bUseVShader == FALSE)
		{
		// release a buffer.
		AcuFrameMemory::DeallocFrameMemory(allocated);
		}
		else
		{
		((LPDIRECT3DVERTEXBUFFER9)(m_pThis->m_pVBuffer))->Unlock();
		}
	}

	AgcmDynamicLightmap* m_pThis;
};

void AgcmDynamicLightmap::render()
{
	PROFILE("AgcmDynamicLightmap::render");

	if(m_lights.size()==0) return;

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)RwTextureGetRaster(GetLightmap()));
	
	//RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*)RWRGBALONG(0, 0, 0, 0));
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);//(void*)rwTEXTUREADDRESSBORDER);
	//@{ Jaewon 20041001
	// mipmap off
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	//@} Jaewon

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	
	//@{ Jaewon 20040930
	// disable fog.
	RwBool bFogEnable;
	RwRenderStateGet(rwRENDERSTATEFOGENABLE, &bFogEnable);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	//@} Jaewon

	if(m_bUseVShader)
	{
		D3DMATRIX				wvpMatrix;

		LockFrame();
		RwMatrix identity;
		RwMatrixSetIdentity(&identity);		
		_rwD3D9VSSetActiveWorldMatrix(&identity);
		_rwD3D9VSGetComposedTransformMatrix((PVOID)&wvpMatrix);
		UnlockFrame();

		RwD3D9SetVertexDeclaration(m_pVDeclaration);
		RwD3D9SetVertexShaderConstant(0, (PVOID)&wvpMatrix, 4);
		RwD3D9SetVertexShader(m_pVShader);
		RwD3D9SetPixelShader(NULL);

		RwD3D9SetTexture(GetLightmap(), 0);
		RwD3D9SetTexture(NULL, 1);

		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);//D3DTADDRESS_BORDER);
		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);//D3DTADDRESS_BORDER);
		//RwD3D9SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);

		//@{ Jaewon 20041001
		// mipmap off
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
		//@} Jaewon
	}

	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	std::for_each(m_lights.begin(), m_lights.end(), renderImpl(this));

	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	if(m_bUseVShader)
	{
		RwD3D9SetTexture(NULL, 0);

		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	
	//@{ Jaewon 20040930
	// restore fog state.
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)bFogEnable);
	//@} Jaewon

	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
}

BOOL AgcmDynamicLightmap::postRenderCB(PVOID pData, PVOID pClass, PVOID pCustData)
{	
	AgcmDynamicLightmap* pThis = (AgcmDynamicLightmap*)pClass;

	if(pThis->m_bOn == FALSE) return TRUE;

	// render all lightmaps in the list.
	pThis->render();

	//pThis->previewLightmap();

	return TRUE;
}

BOOL AgcmDynamicLightmap::customUpdateCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmDynamicLightmap* pThis = (AgcmDynamicLightmap*)pClass;
	LightInfo* pInfo = (LightInfo*)pData;
	
	// do nothing...

	return TRUE;
}

BOOL AgcmDynamicLightmap::customRenderCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmDynamicLightmap* pThis = (AgcmDynamicLightmap*)pClass;
	LightInfo* pInfo = (LightInfo*)pData;
	
	// turn on the visibility flag.
	pInfo->bVisible_ = true;

	return TRUE;
}

RwTexture* AgcmDynamicLightmap::CreateLightmap(UINT32 size)
{
	// preconditions
	ASSERT(size!=0);

	// create a raster.
	RwRaster* pLightmapRaster;
	//@{ Jaewon 20041001
	// mipmap off
	pLightmapRaster = RwRasterCreate(size, size, 32, 
									rwRASTERTYPETEXTURE|rwRASTERFORMAT8888);
	//@}Jaewon
	if(!pLightmapRaster)
	{
		return NULL;
	}

	// fill the raster.
	UINT8* pPixels = (UINT8*)RwRasterLock(pLightmapRaster, 0, rwRASTERLOCKWRITE);
	UINT32 stride = RwRasterGetStride(pLightmapRaster);

	for (int i=0; i< ( int ) size; ++i)
	{
		for(int j=0; j< ( int ) size; ++j)
		{
			// calculate  squared distance in texture coordinate space.
			FLOAT u, v;
			//@{ Jaewon 20040714
			// align with a texel center.
			u = (FLOAT(j)+0.5f)/FLOAT(size);
			v = (FLOAT(i)+0.5f)/FLOAT(size);
			//@} Jaewon
			FLOAT r2 = (2.0f*u-1.0f)*(2.0f*u-1.0f)+(2.0f*v-1.0f)*(2.0f*v-1.0f);
			FLOAT r = sqrtf(r2);
			//@{ Jaewon 20040712
			// border pixels should be zero so that 
			// clamp texture addressing can be used.
			if(r2<=1.0f 
				&& i!=0 && i!=size-1
				&& j!=0 && j!=size-1) 
			//@} Jaewon
			{
				//@{ Jaewon 20050118
				// 255.0f -> 128.0f
				*(pPixels+4*j+0) = UINT8(128.0f*max(1.0f-r, 0.004f));///(quadratic*r2 +  constant));
				//@} Jaewon
			}
			// the color of a out-of-range pixel should be zero.
			else 
			{
				*(pPixels+4*j+0) = 0;
			}
			*(pPixels+4*j+1) = *(pPixels+4*j+0);
			*(pPixels+4*j+2) = *(pPixels+4*j+0);
			*(pPixels+4*j+3) = 255;
		}

		pPixels += stride;
	}

	RwRasterUnlock(pLightmapRaster);

	// create a texture using the above raster.
	return RwTextureCreate(pLightmapRaster);
}

RwBool AgcmDynamicLightmap::Im2DRenderQuad(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,
									FLOAT z, FLOAT recipCamZ, FLOAT uvOffset) const
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

void AgcmDynamicLightmap::previewLightmap()
{
	RwV2d vx[2] = { {0.53f, 0.02f}, {0.98f, 0.5f} };
	RwRaster* camRas;
	FLOAT crw, crh;
	FLOAT recipCamZ;

	RwCamera* pMainCam = GetCamera();

	camRas = RwCameraGetRaster(pMainCam);
	crw = (FLOAT)RwRasterGetWidth(camRas);
	crh = (FLOAT)RwRasterGetHeight(camRas);

	recipCamZ = 1.0f / RwCameraGetNearClipPlane(pMainCam);

	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDZERO);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (PVOID)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)RwTextureGetRaster(GetLightmap()));

	Im2DRenderQuad(vx[0].x * crw, vx[0].y * crh, vx[0].x * crw + 256.0f,
				vx[0].y * crh + 256.0f, RwIm2DGetNearScreenZ(), recipCamZ,
				0.0f);
	
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (PVOID)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (PVOID)rwBLENDSRCALPHA);
}