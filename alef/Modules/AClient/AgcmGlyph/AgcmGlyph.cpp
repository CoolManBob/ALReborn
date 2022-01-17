#include "AgcmGlyph.h"

#include "ApMemoryTracker.h"
//#include "AcuMath.h"
#include "AcuSinTbl.h"
#include "AgcmUIConsole.h"
#include "AgcmItem.h"

USING_ACUMATH;

#include "AcuParticleDraw.h"

#define GLYPH_MAX_VERTEX_COUNT	9
#define GLYPH_MAX_INDEX_COUNT	24

extern "C"
{
void	_rwD3D9RenderStateFlushCache();
}

struct mat2by2
{
	union{
		struct
		{
			float _m11, _m12;
			float _m21, _m22;
		};
		float _m[2][2];
	};

	mat2by2() : _m11(1.f), _m12(0.f), _m21(0.f), _m22(1.f){};
	mat2by2(RwReal deg)
		: _m11( AcuSinTbl::Cos(deg) ), _m12( -AcuSinTbl::Sin(deg) )
		, _m21( AcuSinTbl::Sin(deg) ), _m22(  AcuSinTbl::Cos(deg) )
	{
	};
};
inline void RwV2dByMat2by2(RwV2d* pout, const RwV2d* pin, const mat2by2* pmat)
{
	RwV2d	in = *pin;
	pout->x = in.x * pmat->_m11 + in.y * pmat->_m21;
	pout->y = in.x * pmat->_m12 + in.y * pmat->_m22;
};
//. 2005. 11. 2. Nonstopdj
//. RwMaterial을 돌때 필요한..
RwInt32		AgcmGlyph::m_MateriralIndex = 0;
RwTexture*	AgcmGlyph::m_pGlyphTexture	= NULL;

AgcmGlyph::AgcmGlyph()
{
	//Set Module Name 
	SetModuleName("AgcmGlyph");

	EnableIdle(TRUE);

	m_uiLastTick	= 0;
	m_listGlyph		= NULL;

	m_pCamera		= NULL;
	m_pcmMap		= NULL;
	m_pcmRender		= NULL;
	m_pcmResourceLoader = NULL;
	
	m_pcsAgcmCharacter = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmItemConvert = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgcmSkill = NULL;
	m_pMouseOnClump	= NULL;

	m_pcsAgcmCamera2 = NULL;

	ZeroMemory(m_fHeightSave, sizeof(float) * 15 * 15);

	ZeroMemory(&m_stDestMove,sizeof(m_stDestMove));
	m_pDestMoveTextureParticle = NULL;

	ZeroMemory(&m_stDestEnemyTarget,sizeof(m_stDestEnemyTarget));
	ZeroMemory(m_pDestEnemyTargetTexture,sizeof(m_pDestEnemyTargetTexture));
	
	m_RBObjClumplist.clear();
//	m_pDestMoveClump = NULL;
//	m_pDestMoveRtAnim = NULL;
//	m_bAdd3DDestMove = FALSE;

	//@{ 2006/04/04 burumal
	m_csDestTarget.Init();
	//@}

	//@{ 2006/09/01 burumal
	m_bShowDestMoveTarget = TRUE;
	//@}	
}

AgcmGlyph::~AgcmGlyph()
{

}

BOOL	AgcmGlyph::OnAddModule()
{
	m_pcmRender = (AgcmRender *)GetModule("AgcmRender");

	if (m_pcmRender && !m_pcmRender->SetCallbackPostRender( CB_POST_RENDER, this ) ) 
		return FALSE;

	m_pcmMap = (AgcmMap *)GetModule("AgcmMap");
	m_pcmResourceLoader = (AgcmResourceLoader*) GetModule("AgcmResourceLoader");

	m_pcsAgcmCharacter = (AgcmCharacter*) GetModule("AgcmCharacter");
	m_pcsAgpmItem = (AgpmItem*) GetModule("AgpmItem");
	m_pcsAgpmItemConvert = (AgpmItemConvert*) GetModule("AgpmItemConvert");
	m_pcsAgpmFactors = (AgpmFactors*) GetModule("AgpmFactors");
	m_pcsAgcmSkill = (AgcmSkill*) GetModule("AgcmSkill");

	//. 2006. 5. 22. nonstopdj
	//. 실루엣렌더 Ink의 두께계산을 위해 필요.
	m_pcsAgcmCamera2 = (AgcmCamera2*) GetModule("AgcmCamera2");

	//. 2005. 11. 1. Nonstopdj
	//. ApmObject와 AgcmObject를 사용.
	m_pApmObject	= (ApmObject*) GetModule("ApmObject");
	m_pcmObject		= (AgcmObject*) GetModule("AgcmObject");

	if(m_pcsAgcmSkill)
	{
		m_pcsAgcmSkill->SetCallbackStartCastSelfCharacter(CB_SELF_CHARACTER_CAST_SKILL, this);
	}

	//. 2005. 11. 3. Nonstopdj
	//. clumplist를 clear할 callback을 등록
	if(m_pcmMap)
	{
		m_pcmMap->SetCallbackLoadMap( CB_CLEAR_CLUMPLIST, this);
	}

	return TRUE;
}

BOOL	AgcmGlyph::OnInit()
{
	m_uiLastTick = 0;
	m_listGlyph = NULL;
	
	//3D Dest-Move Target Cursor Init
	if ( !m_pDestMoveTextureParticle )
	{
		m_pDestMoveTextureParticle = RwTextureRead("Particles.tif",NULL);
		if ( m_pDestMoveTextureParticle )
			RwTextureSetAddressing( m_pDestMoveTextureParticle, rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestMoveTextureParticle && "Particles.tif 텍스쳐가 없습니다!" );
		m_stDestMove.eDrawParticle = DEST_MOVE_PARTICLE_STOP;
	}

	//3D Dest-Target Cursor Init
	if ( !m_pDestEnemyTargetTexture[0] )
	{
		m_pDestEnemyTargetTexture[0] = RwTextureRead("ET_Default.tif",NULL);
		if ( m_pDestEnemyTargetTexture[0] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[0], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[0] && "ET_Default.tif 텍스쳐가 없습니다!" );
	}

	if ( !m_pDestEnemyTargetTexture[1] )
	{
		m_pDestEnemyTargetTexture[1] = RwTextureRead("ET_Magic.tif",NULL);
		if ( m_pDestEnemyTargetTexture[1] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[1], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[1] && "ET_Magic.tif 텍스쳐가 없습니다!" );
	}
	
	if ( !m_pDestEnemyTargetTexture[2] )
	{
		m_pDestEnemyTargetTexture[2] = RwTextureRead("ET_Water.tif",NULL);
		if ( m_pDestEnemyTargetTexture[2] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[2], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[2] && "ET_Water.tif 텍스쳐가 없습니다!" );
	}
	
	if ( !m_pDestEnemyTargetTexture[3] )
	{
		m_pDestEnemyTargetTexture[3] = RwTextureRead("ET_Fire.tif",NULL);
		if ( m_pDestEnemyTargetTexture[3] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[3], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[3] && "ET_Fire.tif 텍스쳐가 없습니다!" );
	}

	if ( !m_pDestEnemyTargetTexture[4] )
	{
		m_pDestEnemyTargetTexture[4] = RwTextureRead("ET_Earth.tif",NULL);
		if ( m_pDestEnemyTargetTexture[4] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[4], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[4] && "ET_Earth.tif 텍스쳐가 없습니다!" );
	}

	if ( !m_pDestEnemyTargetTexture[5] )
	{
		m_pDestEnemyTargetTexture[5] = RwTextureRead("ET_Wind.tif",NULL);
		if ( m_pDestEnemyTargetTexture[5] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[5], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[5] && "ET_Wind.tif 텍스쳐가 없습니다!" );
	}

	if ( !m_pDestEnemyTargetTexture[6] )
	{			
		m_pDestEnemyTargetTexture[6] = RwTextureRead("ET_Default_Deco.tif",NULL);
		if ( m_pDestEnemyTargetTexture[6] )
			RwTextureSetAddressing( m_pDestEnemyTargetTexture[6], rwTEXTUREADDRESSCLAMP);
		ASSERT( NULL != m_pDestEnemyTargetTexture[6] && "ET_Default_Deco.tif 텍스쳐가 없습니다!" );
	}
	
	//@{ 2006/09/01 burumal
	AS_REGISTER_TYPE_BEGIN(AgcmGlyph, AgcmGlyph);
	AS_REGISTER_METHOD0(void, ShowDestMoveTarget);
	AS_REGISTER_METHOD0(void, HideDestMoveTarget);	
	AS_REGISTER_TYPE_END;
	//@}

	return TRUE;
}

BOOL	AgcmGlyph::OnDestroy()
{
	m_RBObjClumplist.clear();

	GlyphData*	cur_node = m_listGlyph;
	GlyphData*	remove_node;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete []remove_node->vertex;
		delete remove_node;
	}

	m_listGlyph = NULL;

	//3D Dest-Move Target Cursor Destroy
	if(m_pDestMoveTextureParticle)
	{
		RwTextureDestroy(m_pDestMoveTextureParticle);
		m_pDestMoveTextureParticle = NULL;
	}

	for(int i=0;i<7;++i)
	{
		if(m_pDestEnemyTargetTexture[i])
		{
			RwTextureDestroy(m_pDestEnemyTargetTexture[i]);
			m_pDestEnemyTargetTexture[i] = NULL;
		}
	}

	return TRUE;
}

BOOL	AgcmGlyph::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmGlyph::OnIdle");

	//. 2005. 11. 04. Nonstopdj
	//. 초기로딩시 m_pCamera가 null일수 있다.
	if(m_pCamera)
	{
		UINT32	tickdiff = ulClockCount - m_uiLastTick;
		m_uiLastTick = ulClockCount;

		RwFrame* CameraFrame = RwCameraGetFrame( m_pCamera );
		LockFrame();
		RwMatrix* CameraMatrix = RwFrameGetLTM( CameraFrame );
		UnlockFrame();

		RwV3d* at = RwMatrixGetAt(CameraMatrix);

		UpdateGlyph(tickdiff,at);

		UpdateDestMoveTarget(tickdiff);
		UpdateDestEnemyTarget(tickdiff);
	}

	return TRUE;
}

GlyphData*		AgcmGlyph::AddStaticGlyph(RwTexture*	tex,RwV3d* pos,float width,float height,DWORD color,INT32 lifecount,BOOL bSizeVariable, unsigned char uFlag)
{
	//. clear Clumplist
	m_RBObjClumplist.clear();

	GlyphData*		new_data = new GlyphData;

	//@{ 2006/10/27 burumal
	new_data->flag = uFlag;
	//@}

	new_data->life_count = lifecount;
	new_data->tex = tex;

	//. 2005. 11. 1. Nonstopdj
	//. Clamp모드변경
	if ( tex )
		RwTextureSetAddressing( tex, rwTEXTUREADDRESSCLAMP);

	new_data->staticpos.x = pos->x;
	new_data->staticpos.y = pos->y;
	new_data->staticpos.z = pos->z;

	new_data->movepos = NULL;
	
	new_data->sphere.center = *pos;
	
	float halfw = width * 0.5f;
	float halfh = height * 0.5f;

	float dist = (float)sqrt((halfw * halfw) + (halfh * halfh));
	
	new_data->sphere.radius = dist;

	new_data->width = width;
	new_data->height = height;

	memcpy(&new_data->color , &color, 4);
	new_data->rot_angle = 0.0f;

	if(!bSizeVariable)
	{
		//INT32	diff = MAP_STEPSIZE;						// tile size
		//int		xdif = (int)width/diff + 6;
		//int		zdif = (int)height/diff + 6;

		//new_data->vertex = new RwIm3DVertex[xdif*zdif*6];
		
		//. 2005. 10. 28. Nonstopdj
		//. 가변크기의 vertex를 가진다.
		//new_data->vertex = new RwIm3DVertex[6];
		new_data->vertex = new D3DVTX_PCT[GLYPH_MAX_VERTEX_COUNT];
		new_data->indies = new WORD[GLYPH_MAX_INDEX_COUNT];


		new_data->type = AGCM_GLYPH_STATIC_POS_DIRTY;
	}
	else
	{
		new_data->vertex = NULL;
		new_data->type = AGCM_GLYPH_STATIC_POS_SIZE_UPDATE;
	}

	new_data->vert_count = 0;
			
	new_data->next = m_listGlyph;
	m_listGlyph = new_data;

	return new_data;
}

GlyphData*	AgcmGlyph::AddMovableGlyph(RwTexture* tex,RwFrame* frame,float width,float height,DWORD color,INT32 lifecount,BOOL bSizeVariable, unsigned char uFlag)
{
	GlyphData*		new_data = new GlyphData;

	//@{ 2006/10/27 burumal
	new_data->flag = uFlag;
	//@}

	new_data->life_count = lifecount;
	new_data->tex = tex;

	new_data->type = AGCM_GLYPH_MOVABLE_POS;
	new_data->movepos = frame;

	float halfw = width * 0.5f;
	float halfh = height * 0.5f;

	float dist = (float)sqrt((halfw * halfw) + (halfh * halfh));
	
	new_data->sphere.radius = dist;

	new_data->width = width;
	new_data->height = height;

	memcpy(&new_data->color , &color, 4);
	new_data->rot_angle = 0.0f;

	if(!bSizeVariable)
	{
		INT32	diff = (int) MAP_STEPSIZE;						// tile size
		int		xdif = (int)width/diff + 6;
		int		zdif = (int)height/diff + 6;

		new_data->vertex = new D3DVTX_PCT[xdif*zdif*6];
		new_data->type = AGCM_GLYPH_MOVABLE_POS;
	}
	else
	{
		new_data->vertex = NULL;
		new_data->type = AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE;
	}

	new_data->vert_count = 0;
			
	new_data->next = m_listGlyph;
	m_listGlyph = new_data;

	return new_data;
}

void AgcmGlyph::UpdateGlyph(INT32 tickdiff, RwV3d* pAt)
{
	GlyphData*	cur_node = m_listGlyph;
	GlyphData*	before_node = cur_node;
	GlyphData*	remove_node;

	bool		bDelete = true;
	while(cur_node)
	{
		bDelete = true;
		if(cur_node->life_count != -99999)
		{
			cur_node->life_count -= tickdiff;
		}
		else bDelete = false;
				
		if(cur_node->life_count < 0 && bDelete)
		{
			remove_node = cur_node;
			if(cur_node == m_listGlyph)
			{
				m_listGlyph = cur_node->next;
			}
			else
			{
				before_node->next = cur_node->next;
			}

			cur_node = cur_node->next;

			delete []remove_node->vertex;
			delete []remove_node->indies;
			delete remove_node;
		}
		else
		{
			RwMatrix* pMatrix;
			RwV3d* pPos;
			if(cur_node->type == AGCM_GLYPH_MOVABLE_POS || cur_node->type == AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE)
			{
				LockFrame();
				pMatrix = RwFrameGetLTM( cur_node->movepos );
				UnlockFrame();

				pPos = RwMatrixGetPos(pMatrix);	
				cur_node->sphere.center = *pPos;
			}
			
			RwFrustumTestResult  res = RwCameraFrustumTestSphere(m_pCamera,&cur_node->sphere);
			
			if(res != rwSPHEREOUTSIDE)
			{
				cur_node->bInFrustum = true;

				// test
				//cur_node->rot_angle += 0.5f;
				//if(cur_node->rot_angle >= 360.0f) cur_node->rot_angle = 0.0f;

				if(cur_node->type != AGCM_GLYPH_STATIC_POS)
				{
					// Im3DVertex Update /////////////////////////////////////////////////////////////
					if(cur_node->type == AGCM_GLYPH_STATIC_POS_SIZE_UPDATE || cur_node->type == AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE)
					{
						int		diff = (int) MAP_STEPSIZE;
						int		xdif = (int)cur_node->width/diff + 6;
						int		zdif = (int)cur_node->height/diff + 6;

						//. 2005. 10. 28. Nonstopdj. 사용하지 않는 듯함.
						//cur_node->vertex = (RwIm3DVertex*)AcuFrameMemory::AllocFrameMemory(sizeof(RwIm3DVertex) * xdif * zdif * 6);
					}

					if(cur_node->type == AGCM_GLYPH_MOVABLE_POS || cur_node->type == AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE)
					{
						//. break포인트를 위한..
						int	nBreakPoint = 0;
						//. 2005. 10. 28. Nonstopdj. 사용하지 않는 듯함.
						//cur_node->vert_count = SetVerts(cur_node->vertex,pPos->x,pPos->z,cur_node->width,cur_node->height,cur_node->rot_angle,cur_node->color);
					}
					else if(cur_node->type == AGCM_GLYPH_STATIC_POS_DIRTY || cur_node->type == AGCM_GLYPH_STATIC_POS_SIZE_UPDATE)
					{	
						SetVertsCompact(&cur_node->vertex, &cur_node->indies,
							&cur_node->vert_count, &cur_node->index_count, &cur_node->primitive_count,
							cur_node->staticpos.x, cur_node->staticpos.z,
							cur_node->width, cur_node->height, 
							cur_node->sphere.radius, cur_node->rot_angle,cur_node->color);
					
						if(cur_node->type == AGCM_GLYPH_STATIC_POS_DIRTY)
							cur_node->type = AGCM_GLYPH_STATIC_POS;
					}
				}
				else			
				{
					if(cur_node->rot_angle != 0.0f)		// Rotation Update
					{
						float pos_x = cur_node->staticpos.x;
						float pos_z = cur_node->staticpos.z;
						float start_x = pos_x - cur_node->width * 0.5f;
						float start_z = pos_z - cur_node->height * 0.5f;
						float uval = 1.0f/cur_node->width;
						float vval = 1.0f/cur_node->height;

						//float sinangle = AcuMath::GetSinValFast((INT32)cur_node->rot_angle);
						//float cosangle = AcuMath::GetCosValFast((INT32)cur_node->rot_angle);
						float sinangle = AcuSinTbl::Sin(cur_node->rot_angle);
						float cosangle = AcuSinTbl::Cos(cur_node->rot_angle);

						//. 2005. 10. 28. Nonstopdj. 사용하지 않는 듯함.
						/*
						RwV2d	vOut,vInUV;
						for(int i=0;i<cur_node->vert_count;++i)
						{
                            vInUV.x = (cur_node->vertex[i].objVertex.x - start_x) * uval - 0.5f;
							vInUV.y = (cur_node->vertex[i].objVertex.z - start_z) * vval - 0.5f;
							vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
							vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
							RwIm3DVertexSetU(&cur_node->vertex[i], vOut.x + 0.5f);
							RwIm3DVertexSetV(&cur_node->vertex[i], vOut.y + 0.5f);
						}
						*/
					}

					UINT8				red = cur_node->color.red;
					UINT8				green = cur_node->color.green;
					UINT8				blue = cur_node->color.blue;
					UINT8				alpha = cur_node->color.alpha;

					if(cur_node->vertex)
					{
						//. 2005. 10. 28. Nonstopdj. 사용하지 않는 듯함.
						for(int i=0;i<cur_node->vert_count;++i)			// Color Update
						{
							cur_node->vertex[i].m_dwColr = DEF_ARGB32(alpha, red, green, blue);
							//RwIm3DVertexSetRGBA(&cur_node->vertex[i],red, green, blue, alpha);
						}
					}
				}
			}
			else 
			{
				cur_node->bInFrustum = false;
			}
			
			before_node = cur_node;
			cur_node = cur_node->next;
		}
	}
}

void AgcmGlyph::DeleteGlyph(GlyphData* del_node)
{
	m_RBObjClumplist.clear();

	if(del_node == m_listGlyph)
	{
		m_listGlyph = m_listGlyph->next;
		if(del_node->type != AGCM_GLYPH_STATIC_POS_SIZE_UPDATE && 
			del_node->type != AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE && 
			del_node->vertex && del_node->indies)
		{
			delete []del_node->vertex; 
			delete []del_node->indies; 
		}
		delete del_node;
	}
	else
	{
		GlyphData*		cur_GD = m_listGlyph;
		GlyphData*		bef_GD = cur_GD;

		while(cur_GD)
		{
			if(cur_GD == del_node)
			{
				bef_GD->next = cur_GD->next;

				if(del_node->type != AGCM_GLYPH_STATIC_POS_SIZE_UPDATE && 
					del_node->type != AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE && 
					del_node->vertex && del_node->indies)
				{
					delete []del_node->vertex; 
					delete []del_node->indies;
				}
				delete del_node;
				return;
			}

			bef_GD = cur_GD;
			cur_GD = cur_GD->next;
		}
	}
}

void	AgcmGlyph::UpdatePosStaticGlyph(GlyphData*	pData,float x,float y,float z)
{
	pData->staticpos.x = x;
	pData->staticpos.y = y;
	pData->staticpos.z = z;
	pData->sphere.center = pData->staticpos;

	if(pData->type == AGCM_GLYPH_STATIC_POS)
		pData->type = AGCM_GLYPH_STATIC_POS_DIRTY;
}

void	AgcmGlyph::ResizeGlyph(GlyphData*	pData,float	width,float	height)
{
	ASSERT(!(pData->type == AGCM_GLYPH_STATIC_POS_SIZE_UPDATE || pData->type == AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE) );

	if(pData->vertex) 
		delete []pData->vertex;
	if(pData->indies) 
		delete []pData->indies;

	pData->width = width;
	pData->height = height;

	INT32	diff = (int) MAP_STEPSIZE;						// tile size
	int		xdif = (int)width/diff + 6;
	int		zdif = (int)height/diff + 6;
	pData->vertex = new D3DVTX_PCT[xdif*zdif*6];

	float halfw = width * 0.5f;
	float halfh = height * 0.5f;
	float dist = (float)sqrt((halfw * halfw) + (halfh * halfh));
	pData->sphere.radius = dist;
}
//. 2005. 11. 3. Nonstopdj
//. RpClumpForAllAtomics()에 등록. RpAtomic List을 돌아서 처리하는 callback
RpAtomic* AgcmGlyph::AtomicsRenderProcess(RpAtomic* pAtomic, void* pData)
{
	LPDIRECT3DDEVICE9	pd3dDevice	= ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
	AgcmGlyph*			pThis		= (AgcmGlyph*)pData;

	RpGeometry* pGeometry = RpAtomicGetGeometry(pAtomic);

	ASSERT( NULL != pGeometry && "유효하지 않은 RpGeometry* 에 접근하려 합니다." );

	if( pGeometry )
	{
		RwTexture**	TempTextures = (RwTexture**)AcuFrameMemory::AllocFrameMemory(RpGeometryGetNumMaterials(pGeometry) * sizeof(RwTexture*));

		// 프레임 메모리 에러 처리 추가.
		if( NULL == TempTextures ) return pAtomic;

		m_MateriralIndex = 0;	//. TempTexture Array의 인덱스를 초기화한다.
		RpGeometry* pResult = RpGeometryForAllMaterials(pGeometry, AllMaterialRwTexturePointerSwap, (PVOID)TempTextures);
		ASSERT( NULL != pResult && "RpGeometry* 에 RwTexture* Swap에 실패하였습니다.");

		pd3dDevice->SetTransform( D3DTS_TEXTURE0, &pThis->m_TextureMatrix );
		pThis->m_pcmRender->OriginalAtomicRender(pAtomic);

		m_MateriralIndex = 0;	//. TempTexture Array의 인덱스를 초기화한다.
		pResult = RpGeometryForAllMaterials(pGeometry, AllMaterialRwTexturePointerRestore, (PVOID)TempTextures);
		ASSERT( NULL != pResult && "RpGeometry* 에 RwTexture* Restore에 실패하였습니다.");
	}

	return pAtomic;
}

//. 2005. 11. 3. Nonstopdj
//. RpGeometryForAllMaterials()에 등록. RpGeometry의 RpMaterial List을 돌아 RwTexture*를 Swap.
RpMaterial*	AgcmGlyph::AllMaterialRwTexturePointerSwap(RpMaterial *material, void *data)
{
	((RwTexture**)data)[m_MateriralIndex++] = material->texture;
	material->texture = m_pGlyphTexture;	//. Global Glyph texture로 잠시 설정

	return material;
}

//. 2005. 11. 3. Nonstopdj
//. RpGeometryForAllMaterials()에 등록. RpGeometry의 RpMaterial List을 돌아 RwTexture*를 Restore.
RpMaterial*	AgcmGlyph::AllMaterialRwTexturePointerRestore(RpMaterial *material, void *data)
{
	material->texture = ((RwTexture**)data)[m_MateriralIndex++];
	return material;
}

//. 2006. 5. 22. Nonstopdj
//. ref. AgcmRender에 등록된 Custom Render callback.
//.		 설정된 target clump을 이용해 실제 Silhouette을 Render한다.
BOOL	AgcmGlyph::CB_CUSTOM_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData )
{
#ifndef USE_MFC

	AgcmGlyph* pThis = (AgcmGlyph*)pClass;

	DestEnemyTargetGlyph*	pDestEnemyTarget = &pThis->m_stDestEnemyTarget;
	static BOOL		bRenderEdge		= FALSE;
	static float	fMaxDistance	= 1300.0f;

	//@{ 2006/09/07 burumal
	if ( pDestEnemyTarget->pTargetClump )
		if ( pDestEnemyTarget->pTargetClump->stType.eType & ACUOBJECT_TYPE_INVISIBLE )
			return TRUE;
	if ( pThis->m_pMouseOnClump )
		if ( pThis->m_pMouseOnClump->stType.eType & ACUOBJECT_TYPE_INVISIBLE )
			return TRUE;
	//@}

	(pDestEnemyTarget->bAdded ||  pDestEnemyTarget->bSelected) && pDestEnemyTarget->bInFrustum ? 
		bRenderEdge = TRUE : bRenderEdge = FALSE;

	if(pDestEnemyTarget->pTargetClump && bRenderEdge )
	{
		//. vs를 이용한 Silhouette render일 경우. thickness는 무관함.
		if(pThis->m_pcsAgcmCamera2)
		{
			//. 2006. 5. 22. nonstopdj
			//. 거리당 비례하는 Edge thickness.
			RwV3d	vLength;
			RwReal	fCurDist			= 0.0f;
			const RwV3d* pCurCameraPos	= pThis->m_pcsAgcmCamera2->bGetPtrEye();
			const RwV3d* pCurTargetPos	= RwMatrixGetPos( RwFrameGetLTM(RpClumpGetFrame(pDestEnemyTarget->pTargetClump)) );
			
			RwV3dSub(&vLength, pCurTargetPos, pCurCameraPos);
			fCurDist = RwV3dLength(&vLength);

			pThis->m_pcmRender->SetSilThickness(  pThis->m_pcsAgcmCamera2->bGetMaxDistance() / fCurDist );
		}


		RpClumpForAllAtomics( pDestEnemyTarget->pTargetClump, pThis->m_pcmRender->SilhouetteRender, 0);
	}

	//. mouse roll over.
	if(pThis->m_pMouseOnClump && 
		!IsBadReadPtr(pThis->m_pMouseOnClump->atomicList, sizeof(RpAtomic)) &&
	   pDestEnemyTarget->pTargetClump != pThis->m_pMouseOnClump)
	{
		if(pThis->m_pcsAgcmCamera2)
		{
			//. 2006. 5. 22. nonstopdj
			//. 거리당 비례하는 Edge thickness.
			RwV3d	vLength;
			RwReal	fCurDist			= 0.0f;
			const RwV3d* pCurCameraPos	= pThis->m_pcsAgcmCamera2->bGetPtrEye();
			const RwV3d* pCurTargetPos	= RwMatrixGetPos( RwFrameGetLTM(RpClumpGetFrame(pThis->m_pMouseOnClump)) );
			
			RwV3dSub(&vLength, pCurTargetPos, pCurCameraPos);
			fCurDist = RwV3dLength(&vLength);

			pThis->m_pcmRender->SetSilThickness( pThis->m_pcsAgcmCamera2->bGetMaxDistance() / fCurDist );
		}

		RpClumpForAllAtomics( pThis->m_pMouseOnClump, pThis->m_pcmRender->SilhouetteRender, 0);
	}

#endif

	return TRUE;
}

BOOL	AgcmGlyph::CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmGlyph::CB_POST_RENDER");
	AgcmGlyph* pThis = (AgcmGlyph*)pClass;

	// DestMove Target Glyph Disable(animation)
	//if(pThis->m_stDestMove.eDrawParticle != DEST_MOVE_PARTICLE_STOP && pThis->m_stDestMove.AnimCount < 590)
	//{	
	//	pThis->m_stDestMove.pGlyph->bInFrustum = FALSE;
	//}
	
	if( !pThis->m_pcsAgcmCharacter->GetSelfCharacter() )
	{
		// 셀프케릭터가 없는경우 안돌게함.
		return FALSE;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. 기본 RenderState 설정
	//. 통한의 Shader disable ㅜㅜ
	RwD3D9SetVertexShader(NULL);
	RwD3D9SetPixelShader(NULL);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);	
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	
	RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );	
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE , (void*)TRUE  );
	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
	//RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 10  );

	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS ,(void*)rwTEXTUREADDRESSCLAMP );	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. 2005. 11. 2. Nonstopdj
	//. TerrainBoard Glyph Render	
	GlyphData*		cur_GD = pThis->m_listGlyph;
	while(cur_GD)
	{
		if(cur_GD->bInFrustum && cur_GD->vert_count > 0 && cur_GD->tex)
		{			
			bool bIsVisible = pThis->m_bShowDestMoveTarget ? true : false;
			if ( bIsVisible == false )
				bIsVisible = (cur_GD->flag & eGLYPH_FLAG_FORCED_VISIBLE) ? true : false;
			
			if ( bIsVisible )
			{
				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) RwTextureGetRaster(cur_GD->tex));
				//RwD3D9SetTexture(cur_GD->tex, 0);

				RwMatrix	matId;
				RwMatrixSetIdentity(&matId);
				RwD3D9SetFVF(D3DVTX_PCT::FVF);
				RwD3D9SetTransformWorld( &matId );

				RwD3D9DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST
					, 0LU
					, cur_GD->vert_count
					, cur_GD->primitive_count
					, &cur_GD->indies[0LU]
					, &cur_GD->vertex[0LU]
					, D3DVTX_PCT::SIZE
						);		
			}
		}
		cur_GD = cur_GD->next;
	}
	//.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if ( pThis->m_bShowDestMoveTarget )
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//. 2005. 11. 1. Nostopdj
		//. Projectively Texture Matrix을 이용하여 Ridable Object들에게 드리워진 Glyph 렌더
		//. Atomic이 복잡하게 구성된 Tree Clump들은 AddRenderClumplist()에 일단 추가되지 않음
		//.
		//. 주의사항..
		//. 포탈이나 기타 다른 것에 의해 현재로딩된 sector를 벗어나면 sector의 리소스를 해제하므로
		//. m_RBObjClumplist의 포인터들이 유효하지 않다. 그러므로 로딩화면이나 기타 다른 위치에서 반드시
		//. m_RBObjClumplist를 clear()해줘야 한다.
		ClumpListIter listIt = pThis->m_RBObjClumplist.begin();

		LPDIRECT3DDEVICE9	pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );

		while(listIt != pThis->m_RBObjClumplist.end() && 
			pThis->m_RBObjClumplist.size() > 0		  &&
			pThis->m_listGlyph						  && 
			pThis->m_listGlyph->bInFrustum)
		{
			pThis->UpdateProjectivelyTextureMatrix( pThis->m_listGlyph->staticpos.x, pThis->m_listGlyph->staticpos.z,
											100, 100, //pThis->m_listGlyph->width, pThis->m_listGlyph->height, 
											pThis->m_listGlyph->sphere.radius, pThis->m_listGlyph->rot_angle);
			
			
			RpClump* pClump = (*listIt);
			
			if( pClump && !IsBadReadPtr( pClump , sizeof RpClump ) )
			{
				//. AllMaterialRwTexturePointerSwap에서 사용되는 global Glyph texture;
				if(!m_pGlyphTexture)
					m_pGlyphTexture = pThis->m_listGlyph->tex;

				RpClump* pResultClump = RpClumpForAllAtomics(pClump, AtomicsRenderProcess, (PVOID)pThis);
				ASSERT( NULL != pResultClump && "Projective Glyph Texture Render에 실패하였습니다.");
			}

			//. 2005. 10. 31. Nonstopdj
			//. Rp* 함수들로 다시 제작~
			//if(pClump && pClump->atomicList)
			//{
			//	RpAtomic* pAtomic = pClump->atomicList;
			//	bool bLoopAtomic = true;
			//	while(bLoopAtomic && pAtomic)
			//	{
			//		RpGeometry* pGeometry = pAtomic->geometry;
			//		if(pGeometry)
			//		{
			//			//. 임시 저장용 RwTexture* 배열
			//			//RwTexture**	TempTextures = new RwTexture*[pGeometry->matList.numMaterials];
			//			RwTexture**	TempTextures = (RwTexture**)AcuFrameMemory::AllocFrameMemory(pGeometry->matList.numMaterials * sizeof(RwTexture*));
			//			for(int nIndex = 0; nIndex < pGeometry->matList.numMaterials; ++nIndex)
			//			{
			//				TempTextures[nIndex] = pGeometry->matList.materials[nIndex]->texture;
			//				if(pThis->m_listGlyph->tex)
			//					pGeometry->matList.materials[nIndex]->texture = pThis->m_listGlyph->tex;
			//			}
			//			//. Reder
			//			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &pThis->m_TextureMatrix );
			//			pThis->m_pcmRender->OriginalAtomicRender(pAtomic);
			//			//. 다시 포인터 복구
			//			for(nIndex = 0; nIndex < pGeometry->matList.numMaterials; ++nIndex)
			//			{
			//				 pGeometry->matList.materials[nIndex]->texture = TempTextures[nIndex];
			//			}
			//		}
			//		if(pClump->atomicList == pAtomic->next)
			//			bLoopAtomic = false;
			//		pAtomic = pAtomic->next;
			//	}
			//}

			++listIt;
		}

		//.D3DTSS_TEXTURETRANSFORMFLAGS restore
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		//. 
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
		// DestMove	Target Particle Draw
		if(pThis->m_stDestMove.eDrawParticle == DEST_MOVE_PARTICLE_START)
		{
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE   );
			RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			_rwD3D9RenderStateFlushCache();

			DWORD	color;
			DestMoveTarget*		pDestMove = &pThis->m_stDestMove;
			
			AcuParticleDraw::DrawStart(pThis->m_pDestMoveTextureParticle,PARTICLE_BILLBOARD);

			// Particles Draw
			for(int	j=0;j<5;++j)
			{
				for(int i=0;i<pDestMove->DrawParticleTailNum;++i)
				{
					memcpy(&color,&pDestMove->Particle[j][i].color,4);

					AcuParticleDraw::AccumulateBuffer(&pDestMove->Particle[j][i].pos,
					color,pDestMove->Particle[j][i].size,pDestMove->Particle[j][i].size);
				}
			}

			AcuParticleDraw::DrawEnd();

			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
		}
		else if(pThis->m_stDestMove.eDrawParticle == DEST_MOVE_PARTICLE_CIRCLE)
		{
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE   );
			RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			_rwD3D9RenderStateFlushCache();
			
			DWORD	color;
			DestMoveTarget*		pDestMove = &pThis->m_stDestMove;
			
			AcuParticleDraw::DrawStart(pThis->m_pDestMoveTextureParticle,PARTICLE_BILLBOARD);

			// Particles Draw
			for(int	j=0;j<2;++j)
			{
				for(int i=0;i<pDestMove->DrawParticleTailNum;++i)
				{
					memcpy(&color,&pDestMove->Particle[j][i].color,4);

					AcuParticleDraw::AccumulateBuffer(&pDestMove->Particle[j][i].pos,
					color,pDestMove->Particle[j][i].size,pDestMove->Particle[j][i].size);
				}
			}

			AcuParticleDraw::DrawEnd();
			
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
		}
	}	

	//@{ 2006/04/04 burumal
	pThis->LockDestEnemyTarget();
	//@}

	// Dest Enemy Target Draw
	DestEnemyTargetGlyph*	pDestEnemyTarget = &pThis->m_stDestEnemyTarget;

	if(pDestEnemyTarget->bAdded && pDestEnemyTarget->bInFrustum)
	{
		for(int i=0;i < pDestEnemyTarget->soul_stone_type_num; ++i)
		{
			if(pThis->m_pDestEnemyTargetTexture[pDestEnemyTarget->soul_stone_type[i]])
			{
				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
					(void *) RwTextureGetRaster(pThis->m_pDestEnemyTargetTexture[pDestEnemyTarget->soul_stone_type[i]]));
				
				if (RwIm3DTransform(pDestEnemyTarget->vertex[i], pDestEnemyTarget->vert_count, NULL,
									rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV |
									rwIM3D_VERTEXRGBA))
				{
					RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
					RwIm3DEnd();
				}
			}
		}

		// Skill Decoration Draw
		if(pDestEnemyTarget->eSkillType != DEST_TARGET_NONE && pThis->m_pDestEnemyTargetTexture[6])
		{
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE   );

			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
				(void *) RwTextureGetRaster(pThis->m_pDestEnemyTargetTexture[6]));

			if (RwIm3DTransform(pDestEnemyTarget->vertex_skill_deco, pDestEnemyTarget->vert_count, NULL,
								rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV |
								rwIM3D_VERTEXRGBA))
			{
				RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
				RwIm3DEnd();
			}

			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
		}
	}

	//@{ 2006/04/04 burumal
	pThis->UnlockDestEnemyTarget();
	//@}
	
	return TRUE;
}

void	AgcmGlyph::StartDestMoveTarget(RwV3d*	pos,float	radius,GlyphData*	pGlyph)
{
	m_stDestMove.sphere.center = *pos;
	m_stDestMove.sphere.radius = radius;
	
	m_stDestMove.eDrawParticle = DEST_MOVE_PARTICLE_START;
	m_stDestMove.AnimCount = 0;
	m_stDestMove.DrawParticleTailNum = 1;
	//m_stDestMove.pGlyph = pGlyph;
	
	float		deg;
	float		circle_radius = radius * 0.41f;
	
	// Particles 초기화
	for(int j=0;j<5;++j)
	{
		m_stDestMove.Particle[j][0].acceleration = 0.5f + ( rand() % 10 ) * 0.1f;
		m_stDestMove.Particle[j][0].color.red = 255;
		m_stDestMove.Particle[j][0].color.green = 247;
		m_stDestMove.Particle[j][0].color.blue = 153;
		m_stDestMove.Particle[j][0].color.alpha = 64;
		m_stDestMove.Particle[j][0].size = 8.0f;//(rand() % 60) * 0.1f + 10.0f;

		UINT8	red_diff = m_stDestMove.Particle[j][0].color.red / DEST_MOVE_PARTICLE_NUM;
		UINT8	green_diff = m_stDestMove.Particle[j][0].color.green / DEST_MOVE_PARTICLE_NUM;
		UINT8	blue_diff = m_stDestMove.Particle[j][0].color.blue / DEST_MOVE_PARTICLE_NUM;

		deg = 72.0f * j;
		m_stDestMove.Particle[j][0].deg = deg;
		//m_stDestMove.Particle[j][0].pos.x = pos->x + AcuMath::GetSinValFast((INT32)deg) * circle_radius;
		//m_stDestMove.Particle[j][0].pos.y = pos->y + 50.0f;
		//m_stDestMove.Particle[j][0].pos.z = pos->z + AcuMath::GetCosValFast((INT32)deg) * circle_radius;
		m_stDestMove.Particle[j][0].pos.x = pos->x + AcuSinTbl::Sin(deg) * circle_radius;
		m_stDestMove.Particle[j][0].pos.y = pos->y + 50.0f;
		m_stDestMove.Particle[j][0].pos.z = pos->z + AcuSinTbl::Cos(deg) * circle_radius;
		
		for(int i=1;i<DEST_MOVE_PARTICLE_NUM;++i)
		{
			m_stDestMove.Particle[j][i].color.red = m_stDestMove.Particle[j][i-1].color.red - red_diff;
			m_stDestMove.Particle[j][i].color.green = m_stDestMove.Particle[j][i-1].color.green - green_diff;
			m_stDestMove.Particle[j][i].color.blue = m_stDestMove.Particle[j][i-1].color.blue - blue_diff;
			m_stDestMove.Particle[j][i].color.alpha = m_stDestMove.Particle[j][i-1].color.alpha;
			m_stDestMove.Particle[j][i].size = m_stDestMove.Particle[j][0].size;
		
			m_stDestMove.Particle[j][i].pos.x = m_stDestMove.Particle[j][0].pos.x;
			m_stDestMove.Particle[j][i].pos.y = m_stDestMove.Particle[j][0].pos.y;
			m_stDestMove.Particle[j][i].pos.z = m_stDestMove.Particle[j][0].pos.z;
		}
	}

	//@{ 2006/09/01 burumal
	m_bShowDestMoveTarget = TRUE;
	//@}
}

void	AgcmGlyph::StopDestMoveTarget()
{
	m_stDestMove.eDrawParticle = DEST_MOVE_PARTICLE_STOP;
}

void	AgcmGlyph::UpdateDestMoveTarget(INT32	tickdiff)
{
	int	i;

	if(m_stDestMove.eDrawParticle == DEST_MOVE_PARTICLE_START)
	{
		// 등장 animation
		//UINT8	alpha_particle = (UINT8)(64 + ((FLOAT)m_stDestMove.AnimCount / 800.0f) * 192);
		FLOAT	circle_radius = m_stDestMove.sphere.radius * 0.41f;
		RwV3d*	circle_center = &m_stDestMove.sphere.center;
		
		// Particle변경
		for(int j=0;j<5;++j)
		{
			m_stDestMove.Particle[j][0].acceleration += tickdiff * 0.02f;
			m_stDestMove.Particle[j][0].deg += 0.3f * tickdiff + m_stDestMove.Particle[j][0].acceleration * 0.001f;
			
			//m_stDestMove.Particle[j][0].pos.x = circle_center->x + AcuMath::GetSinValClampTest((INT32)m_stDestMove.Particle[j][0].deg) * circle_radius;
			//m_stDestMove.Particle[j][0].pos.y -= tickdiff * 0.015f * m_stDestMove.Particle[j][0].acceleration;		// 90 / 1000
			//m_stDestMove.Particle[j][0].pos.z = circle_center->z + AcuMath::GetCosValClampTest((INT32)m_stDestMove.Particle[j][0].deg) * circle_radius;
			m_stDestMove.Particle[j][0].pos.x = circle_center->x + AcuSinTbl::Sin(m_stDestMove.Particle[j][0].deg) * circle_radius;
			m_stDestMove.Particle[j][0].pos.y -= tickdiff * 0.015f * m_stDestMove.Particle[j][0].acceleration;		// 90 / 1000
			m_stDestMove.Particle[j][0].pos.z = circle_center->z + AcuSinTbl::Cos(m_stDestMove.Particle[j][0].deg) * circle_radius;

			m_stDestMove.Particle[j][0].size = 8.0f + m_stDestMove.Particle[j][0].acceleration;
			if(m_stDestMove.Particle[j][0].size >= 64.0f) m_stDestMove.Particle[j][0].size = 64.0f;

			for(int i=DEST_MOVE_PARTICLE_NUM-1;i>=1;--i)
			{
				m_stDestMove.Particle[j][i].pos.x = m_stDestMove.Particle[j][i-1].pos.x;
				m_stDestMove.Particle[j][i].pos.y = m_stDestMove.Particle[j][i-1].pos.y;
				m_stDestMove.Particle[j][i].pos.z = m_stDestMove.Particle[j][i-1].pos.z;
				m_stDestMove.Particle[j][i].size = m_stDestMove.Particle[j][i-1].size;
			}
		}
		
		if(m_stDestMove.DrawParticleTailNum < DEST_MOVE_PARTICLE_NUM)	++m_stDestMove.DrawParticleTailNum;
		m_stDestMove.AnimCount += tickdiff;

		if(m_stDestMove.AnimCount > 800) 
		{
			INT32	i, j;
			m_stDestMove.eDrawParticle = DEST_MOVE_PARTICLE_CIRCLE;

			// 높이값 저장
			for(i=0;i<72;++i)
			{
				m_stDestMove.fGroundHeight[i] = m_pcmMap->GetHeight(
					//circle_center->x + AcuMath::GetSinValClampTest(5 * i) * circle_radius,
					//circle_center->z + AcuMath::GetCosValClampTest(5 * i) * circle_radius) + 5.0f;
					circle_center->x + AcuSinTbl::Sin(5.f * static_cast<float>(i)) * circle_radius,
					circle_center->z + AcuSinTbl::Cos(5.f * static_cast<float>(i)) * circle_radius,
					SECTOR_MAX_HEIGHT ) + 5.0f;
			}

			m_stDestMove.fCircleRotAngle = m_stDestMove.Particle[0][0].deg;

			for(j=0;j<2;++j)
			{
				for(i=0;i<DEST_MOVE_PARTICLE_NUM;++i)
				{
					m_stDestMove.Particle[j][i].size = 11.0f;	// Size 7으로 설정
				}
			}

			m_stDestMove.DrawParticleTailNum = 1;
		}
	}
	else if(m_stDestMove.eDrawParticle == DEST_MOVE_PARTICLE_CIRCLE)
	{
		FLOAT	circle_radius = m_stDestMove.sphere.radius * 0.41f;
		RwV3d*	circle_center = &m_stDestMove.sphere.center;

		m_stDestMove.fCircleRotAngle += tickdiff * 0.12f;
		while(m_stDestMove.fCircleRotAngle > 360.f)	m_stDestMove.fCircleRotAngle -= 360.0f;

		INT32	iTableIndex = (INT32)m_stDestMove.fCircleRotAngle / 5;
		INT32	iClampRotAngle = iTableIndex * 5;

		//m_stDestMove.Particle[0][0].pos.x = circle_center->x + AcuMath::GetSinValClampTest(iClampRotAngle) * circle_radius;
		//m_stDestMove.Particle[0][0].pos.y = m_stDestMove.fGroundHeight[iTableIndex];
		//m_stDestMove.Particle[0][0].pos.z = circle_center->z + AcuMath::GetCosValClampTest(iClampRotAngle) * circle_radius;
		m_stDestMove.Particle[0][0].pos.x = circle_center->x + AcuSinTbl::Sin(static_cast<float>(iClampRotAngle)) * circle_radius;
		m_stDestMove.Particle[0][0].pos.y = m_stDestMove.fGroundHeight[iTableIndex];
		m_stDestMove.Particle[0][0].pos.z = circle_center->z + AcuSinTbl::Cos(static_cast<float>(iClampRotAngle)) * circle_radius;
		for(i=DEST_MOVE_PARTICLE_NUM-1;i>=1;--i)
		{
			m_stDestMove.Particle[0][i].pos.x = m_stDestMove.Particle[0][i-1].pos.x;
			m_stDestMove.Particle[0][i].pos.y = m_stDestMove.Particle[0][i-1].pos.y;
			m_stDestMove.Particle[0][i].pos.z = m_stDestMove.Particle[0][i-1].pos.z;
		}

		iTableIndex -= 36;
		if(iTableIndex < 0) iTableIndex += 72;
		iClampRotAngle = iTableIndex * 5;

		//m_stDestMove.Particle[1][0].pos.x = circle_center->x + AcuMath::GetSinValClampTest(iClampRotAngle) * circle_radius;
		//m_stDestMove.Particle[1][0].pos.y = m_stDestMove.fGroundHeight[iTableIndex];
		//m_stDestMove.Particle[1][0].pos.z = circle_center->z + AcuMath::GetCosValClampTest(iClampRotAngle) * circle_radius;
		m_stDestMove.Particle[1][0].pos.x = circle_center->x + AcuSinTbl::Sin(static_cast<float>(iClampRotAngle)) * circle_radius;
		m_stDestMove.Particle[1][0].pos.y = m_stDestMove.fGroundHeight[iTableIndex];
		m_stDestMove.Particle[1][0].pos.z = circle_center->z + AcuSinTbl::Cos(static_cast<float>(iClampRotAngle)) * circle_radius;
		for(i=DEST_MOVE_PARTICLE_NUM-1;i>=1;--i)
		{
			m_stDestMove.Particle[1][i].pos.x = m_stDestMove.Particle[1][i-1].pos.x;
			m_stDestMove.Particle[1][i].pos.y = m_stDestMove.Particle[1][i-1].pos.y;
			m_stDestMove.Particle[1][i].pos.z = m_stDestMove.Particle[1][i-1].pos.z;
		}
		
		if(m_stDestMove.DrawParticleTailNum < DEST_MOVE_PARTICLE_NUM)	++m_stDestMove.DrawParticleTailNum;
	}
}

void	AgcmGlyph::SetMouseOnTargetClump(RpClump* pClump)
{
	m_pMouseOnClump = pClump;
}

void	AgcmGlyph::StartEnemyTarget(RwFrame*	pFrame, RpClump* pClump, float	radius)
{
	//@{ 2006/04/04 burumal
	LockDestEnemyTarget();
	//@}

	m_stDestEnemyTarget.pTargetClump = pClump;

	//. 외부에서 설정.
	m_stDestEnemyTarget.bSelected = TRUE;

	// 주인공 무기의 정보를 얻어온다..
	m_stDestEnemyTarget.soul_stone_type_num = 1;

	m_stDestEnemyTarget.bEnableDefault = TRUE;
	m_stDestEnemyTarget.soul_stone_type[0] = DEST_TARGET_NORMAL;
	m_stDestEnemyTarget.soul_stone_level[0] = 0;

	m_stDestEnemyTarget.bAdded = TRUE;

	m_stDestEnemyTarget.bInFrustum = FALSE;
	m_stDestEnemyTarget.movepos = pFrame;
		
	m_stDestEnemyTarget.sphere.radius = radius;

	m_stDestEnemyTarget.width = radius;
	m_stDestEnemyTarget.height = radius;

	int		diff = (int) MAP_STEPSIZE;
	int		xdif = (int)radius/diff + 6;
	int		zdif = (int)radius/diff + 6;
	m_stDestEnemyTarget.mem_alloc_size = sizeof(RwIm3DVertex) * xdif * zdif * 6;

	m_stDestEnemyTarget.color[0].red = 255;
	m_stDestEnemyTarget.color[0].green = 255;
	m_stDestEnemyTarget.color[0].blue = 255;
	m_stDestEnemyTarget.color[0].alpha = 255;

	m_stDestEnemyTarget.color_anim_type = 0;
	m_stDestEnemyTarget.color_anim_val = 1.0f;
	m_stDestEnemyTarget.eSkillType = DEST_TARGET_NONE;

	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	AgpdItem*	pEquipWeaponItem = NULL;
	if( pcmItem )
	{
		pEquipWeaponItem = pcmItem->GetCurrentEquipWeapon(m_pcsAgcmCharacter->GetSelfCharacter());
		if(!pEquipWeaponItem)
		{
			//@{ 2006/04/04 burumal
			UnlockDestEnemyTarget();
			//@}

			return;
		}
	}

	AgpdItemConvertADItem*	pcsAttachData = m_pcsAgpmItemConvert->GetADItem(pEquipWeaponItem);
	INT32		iSoulRank;
	INT32		iSoulType;

	//m_stDestEnemyTarget.soul_stone_type[1] = DEST_TARGET_AIR;
	//m_stDestEnemyTarget.soul_stone_level[1] = 0;

	//m_stDestEnemyTarget.soul_stone_type[2] = DEST_TARGET_WATER;
	//m_stDestEnemyTarget.soul_stone_level[2] = 0;

	//m_stDestEnemyTarget.soul_stone_type[3] = DEST_TARGET_FIRE;
	//m_stDestEnemyTarget.soul_stone_level[3] = 0;

	//m_stDestEnemyTarget.soul_stone_type[4] = DEST_TARGET_EARTH;
	//m_stDestEnemyTarget.soul_stone_level[4] = 0;

	//m_stDestEnemyTarget.soul_stone_type[5] = DEST_TARGET_MAGIC;
	//m_stDestEnemyTarget.soul_stone_level[5] = 0;

	m_stDestEnemyTarget.soul_stone_type_num = 1;

	if(pcsAttachData->m_lNumConvert < 0)
	{
		m_stDestEnemyTarget.soul_stone_type_num = 1;
	}
	else
	{
		if(pcsAttachData->m_lNumConvert > 0)
		{
			for(int i=0;i<pcsAttachData->m_lNumConvert;++i)
			{
				if(!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
					!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate )
					continue;

				iSoulType = ((AgpdItemTemplateUsableSpiritStone*)pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType;
				iSoulRank = 0;

				// 정령석 레벨 가져오기
				m_pcsAgpmFactors->GetValue(&((AgpdItemTemplateUsableSpiritStone*) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_csFactor,
											&iSoulRank, AGPD_FACTORS_TYPE_ITEM , AGPD_FACTORS_ITEM_TYPE_RANK );

				BOOL	bFind = FALSE;
				for(int j=0;j<m_stDestEnemyTarget.soul_stone_type_num;++j)
				{
					if(m_stDestEnemyTarget.soul_stone_type[j] == iSoulType)
					{
						bFind = TRUE;
						if(iSoulRank > m_stDestEnemyTarget.soul_stone_level[j]) 
							m_stDestEnemyTarget.soul_stone_level[j] = iSoulRank;

						break;
					}
				}

				if(!bFind)
				{
					if(iSoulType == DEST_TARGET_AIR) 
					{
						// 기본 이미지랑 교체
						m_stDestEnemyTarget.soul_stone_type[0] = DEST_TARGET_AIR;
						m_stDestEnemyTarget.soul_stone_level[0] = iSoulRank;
						m_stDestEnemyTarget.bEnableDefault = FALSE;
					}
					else
					{
						m_stDestEnemyTarget.soul_stone_type[m_stDestEnemyTarget.soul_stone_type_num] = iSoulType;
						m_stDestEnemyTarget.soul_stone_level[m_stDestEnemyTarget.soul_stone_type_num] = iSoulRank;
						++m_stDestEnemyTarget.soul_stone_type_num;
					}
				}
			}
		}
	}

	for(int i=1;i<m_stDestEnemyTarget.soul_stone_type_num;++i)
	{
		m_stDestEnemyTarget.color[i].red = 255;
		m_stDestEnemyTarget.color[i].green = 255;
		m_stDestEnemyTarget.color[i].blue = 255;
		m_stDestEnemyTarget.color[i].alpha = 255;

		m_stDestEnemyTarget.rot_angle[i] = 0.0f;
	}

	//@{ 2006/04/04 burumal
	UnlockDestEnemyTarget();
	//@}
}

void	AgcmGlyph::StopEnemyTarget()
{
	m_stDestEnemyTarget.bAdded = FALSE;
}

void	AgcmGlyph::CloseEnemyTargetSelectWindow()
{
	m_stDestEnemyTarget.bSelected = FALSE;
	m_stDestEnemyTarget.pTargetClump = NULL;
	m_pMouseOnClump = NULL;
}

void	AgcmGlyph::UpdateDestEnemyTarget(INT32	tickdiff)
{
	//@{ 2006/04/04 burumal
	LockDestEnemyTarget();
	//@}

	PROFILE("AgcmGlyph:UpdateDestEnemyTarget");

	const	float	ET_RotVal[6] = { 0.0f, -0.1f, 0.05f, 0.0f, 0.0f, 0.1f };

	if( m_stDestEnemyTarget.bAdded &&
		!IsBadReadPtr( m_stDestEnemyTarget.movepos , sizeof RwFrame ) )
	{
		RwMatrix* pMatrix;
		RwV3d* pPos;

		pMatrix = RwFrameGetLTM( m_stDestEnemyTarget.movepos );
		pPos = RwMatrixGetPos(pMatrix);	
		
		m_stDestEnemyTarget.sphere.center = *pPos;
		RwFrustumTestResult  res = RwCameraFrustumTestSphere(m_pCamera,&m_stDestEnemyTarget.sphere);
			
		if(res != rwSPHEREOUTSIDE)
		{
			m_stDestEnemyTarget.bInFrustum = TRUE;

			if(m_stDestEnemyTarget.bEnableDefault)
			{
				// 0번 vertex색상 변경
				if(m_stDestEnemyTarget.color_anim_type == 0)			// 감소
				{
					m_stDestEnemyTarget.color_anim_val -= 0.0008f * tickdiff;
					if(m_stDestEnemyTarget.color_anim_val < 0.6f)
					{
						m_stDestEnemyTarget.color_anim_val = 0.6f;
						m_stDestEnemyTarget.color_anim_type = 1;
					}
				}
				else if(m_stDestEnemyTarget.color_anim_type == 1)		// 증가
				{
					m_stDestEnemyTarget.color_anim_val += 0.0008f * tickdiff;
					if(m_stDestEnemyTarget.color_anim_val > 1.0f)
					{
						m_stDestEnemyTarget.color_anim_val = 1.0f;
						m_stDestEnemyTarget.color_anim_type = 0;
					}
				}

				UINT8	color_val = (UINT8)(m_stDestEnemyTarget.color_anim_val * 255.0f);

				m_stDestEnemyTarget.color[0].red = m_stDestEnemyTarget.color[0].green = m_stDestEnemyTarget.color[0].blue = color_val;
			}

			for(int i=0;i<m_stDestEnemyTarget.soul_stone_type_num;++i)
			{
				m_stDestEnemyTarget.vertex[i] = (RwIm3DVertex*)AcuFrameMemory::AllocFrameMemory(m_stDestEnemyTarget.mem_alloc_size);

				// AllocFrameMemory 에러처리 추가.
				if( m_stDestEnemyTarget.vertex[i] )
				{
					m_stDestEnemyTarget.rot_angle[i] += ET_RotVal[m_stDestEnemyTarget.soul_stone_type[i]] * tickdiff;
					if(m_stDestEnemyTarget.rot_angle[i] >= 360.0f) m_stDestEnemyTarget.rot_angle[i] = 0.0f;
					else if(m_stDestEnemyTarget.rot_angle[i] < 0.0f) m_stDestEnemyTarget.rot_angle[i] = 359.0f;

					m_stDestEnemyTarget.vert_count = SetVerts(m_stDestEnemyTarget.vertex[i],pPos->x,pPos->z,m_stDestEnemyTarget.width,
														m_stDestEnemyTarget.height,m_stDestEnemyTarget.rot_angle[i],
														m_stDestEnemyTarget.color[i]);
				}
			}

			if(m_stDestEnemyTarget.eSkillType != DEST_TARGET_NONE)
			{
				m_stDestEnemyTarget.skill_duration -= tickdiff;
				if(m_stDestEnemyTarget.skill_duration < 0)
				{
					m_stDestEnemyTarget.eSkillType = DEST_TARGET_NONE;
				}
				else
				{
					m_stDestEnemyTarget.vertex_skill_deco = (RwIm3DVertex*)AcuFrameMemory::AllocFrameMemory(m_stDestEnemyTarget.mem_alloc_size);
					// AllocFrameMemory 에러처리 추가.
					if( m_stDestEnemyTarget.vertex_skill_deco )
					{
						m_stDestEnemyTarget.rot_angle_skill_deco += 0.07f * tickdiff;
						if(m_stDestEnemyTarget.rot_angle_skill_deco >= 360.0f) m_stDestEnemyTarget.rot_angle_skill_deco = 0.0f;

						SetVerts(m_stDestEnemyTarget.vertex_skill_deco,pPos->x,pPos->z,m_stDestEnemyTarget.width,
															m_stDestEnemyTarget.height,m_stDestEnemyTarget.rot_angle_skill_deco,
															m_stDestEnemyTarget.color_skill_deco);
					}
				}
			}
		}
		else 
		{
			m_stDestEnemyTarget.bInFrustum = FALSE;
		}
	}

	//@{ 2006/04/04 burumal
	UnlockDestEnemyTarget();
	//@}
}

BOOL	AgcmGlyph::CB_SELF_CHARACTER_CAST_SKILL ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmGlyph*		pThis = (AgcmGlyph*) pClass;
	
	//@{ 2006/04/04 burumal
	if ( !pThis )
		return TRUE;

	pThis->LockDestEnemyTarget();
	//@}

	if(pThis->m_stDestEnemyTarget.bAdded )//&& pThis->m_stDestEnemyTarget.bEnableDefault)
	{
		pThis->m_stDestEnemyTarget.rot_angle_skill_deco = 0.0f;
		pThis->m_stDestEnemyTarget.color_skill_deco.red = 255;
		pThis->m_stDestEnemyTarget.color_skill_deco.green = 255;
		pThis->m_stDestEnemyTarget.color_skill_deco.blue = 255;
		pThis->m_stDestEnemyTarget.color_skill_deco.alpha = 255;

		FLOAT	fSkillAnimationDuration = *((FLOAT*)pData);
		INT8	cSkillType = *((INT8*)pCustData);

		pThis->m_stDestEnemyTarget.skill_duration = (INT32)(fSkillAnimationDuration * 1000.0f);
		
		if(cSkillType == 0)
		{
			pThis->m_stDestEnemyTarget.eSkillType = DEST_TARGET_ATTACK;
		}
		else if(cSkillType == 1)
		{
			pThis->m_stDestEnemyTarget.eSkillType = DEST_TARGET_BUFF;
		}
	}

	//@{ 2006/04/04 burumal
	pThis->UnlockDestEnemyTarget();
	//@}

	return	TRUE;
}

INT32 AgcmGlyph::SetVerts(RwIm3DVertex*	inVertex,float x,float z,float width,float height,float rot_angle,RwRGBA	color)
{
	// Im3DVertex Update /////////////////////////////////////////////////////////////
	int			diff = (int) MAP_STEPSIZE;
	float		halfw = width * 0.5f;
	float		halfh = height * 0.5f;

	float		start_x = x - halfw;
	float		start_z = z - halfh;

	INT32 xst = ((INT32)start_x) / diff - 1;
	INT32 xend = ((INT32)(x + halfw)) / diff +1;

	INT32 zst = ((INT32)start_z) / diff - 1;
	INT32 zend = ((INT32)(z + halfh)) / diff +1;

	INT32 xdist = xend - xst;
	INT32 zdist = zend - zst;

	UINT8	red = color.red;
	UINT8	green = color.green;
	UINT8	blue = color.blue;
	UINT8	alpha = color.alpha;

	RwIm3DVertex*		imv = inVertex;
					
	RwV3d               vIn[3];

	float				uval = 1.0f/width;
	float				vval = 1.0f/height;
	INT32				vert_count = 0;

	INT32 i,j;
	for(i = 0; i<= zdist; ++i)
	{
		for(j = 0; j<= xdist; ++j)
		{
			m_fHeightSave[j][i] = m_pcmMap->GetHeight((FLOAT)((j+xst)*diff),(FLOAT)((i+zst)*diff) , SECTOR_MAX_HEIGHT ) + 5.0f;
		}
	}
	
	int		ti,tj;
//	RwV3d	a, b, normal1,normal2,normal3;

	for(i = zst; i < zend; ++i)
	{
		for(j = xst; j < xend; ++j)
		{
			ti = i-zst;
			tj = j-xst;

			vIn[0].x = (FLOAT)j*diff; 
			vIn[0].z = (FLOAT)i*diff;
			vIn[0].y = m_fHeightSave[tj][ti];

			vIn[1].x = (FLOAT)j*diff;
			vIn[1].z = (FLOAT)(i+1)*diff;
			vIn[1].y = m_fHeightSave[tj][ti+1];
			
			vIn[2].x = (FLOAT)(j+1)*diff;
			vIn[2].z = (FLOAT)i*diff;
			vIn[2].y = m_fHeightSave[tj+1][ti];

			RwIm3DVertexSetPos(imv, vIn[0].x, vIn[0].y, vIn[0].z);
			RwIm3DVertexSetPos(imv + 1, vIn[1].x, vIn[1].y, vIn[1].z);
			RwIm3DVertexSetPos(imv + 2, vIn[2].x, vIn[2].y, vIn[2].z);

			// Normal 계산
			//RwV3dSub(&a, &vIn[2],&vIn[0]);
			//RwV3dSub(&b, &vIn[1],&vIn[0]);

			//RwV3dCrossProduct(&normal1, &a, &b);
			//RwV3dNormalize(&normal1, &normal1);

			//RwV3dSub(&a, &vIn[1],&vIn[3]);
			//RwV3dSub(&b, &vIn[2],&vIn[3]);

			//RwV3dCrossProduct(&normal2, &a, &b);
			//RwV3dNormalize(&normal2, &normal2);

			//RwV3dAdd(&normal3 , &normal1 , &normal2);
			//RwV3dNormalize(&normal3, &normal3);
			//
			//RwIm3DVertexSetNormal(imv, normal3.x , normal3.y , normal3.z );
			//RwIm3DVertexSetNormal(imv+1, normal3.x , normal3.y , normal3.z );
			//RwIm3DVertexSetNormal(imv+2, normal3.x , normal3.y , normal3.z );

			// 계산
			if(rot_angle == 0.0f)
			{
				RwIm3DVertexSetU(imv, (vIn[0].x - start_x) * uval);
				RwIm3DVertexSetU(imv + 1, (vIn[1].x - start_x) * uval);
				RwIm3DVertexSetU(imv + 2, (vIn[2].x - start_x) * uval);
				
				RwIm3DVertexSetV(imv, (vIn[0].z - start_z) * vval);
				RwIm3DVertexSetV(imv + 1, (vIn[1].z - start_z) * vval);
				RwIm3DVertexSetV(imv + 2, (vIn[2].z - start_z) * vval);
			}
			else
			{
				//float	sinangle = AcuMath::GetSinValFast((INT32)rot_angle);
				//float	cosangle = AcuMath::GetCosValFast((INT32)rot_angle);
				float	sinangle = AcuSinTbl::Sin(rot_angle);
				float	cosangle = AcuSinTbl::Cos(rot_angle);

				RwV2d	vOut,vInUV;

                vInUV.x = (vIn[0].x - start_x) * uval - 0.5f;
				vInUV.y = (vIn[0].z - start_z) * vval - 0.5f;
				vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
				vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
				RwIm3DVertexSetU(imv, vOut.x + 0.5f);
				RwIm3DVertexSetV(imv, vOut.y + 0.5f);

				vInUV.x = (vIn[1].x - start_x) * uval - 0.5f;
				vInUV.y = (vIn[1].z - start_z) * vval - 0.5f;
				vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
				vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
				RwIm3DVertexSetU(imv+1, vOut.x + 0.5f);
				RwIm3DVertexSetV(imv+1, vOut.y + 0.5f);

				vInUV.x = (vIn[2].x - start_x) * uval - 0.5f;
				vInUV.y = (vIn[2].z - start_z) * vval - 0.5f;
				vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
				vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
				RwIm3DVertexSetU(imv+2, vOut.x + 0.5f);
				RwIm3DVertexSetV(imv+2, vOut.y + 0.5f);
			}

			RwIm3DVertexSetRGBA(imv, red, green, blue, alpha);
			RwIm3DVertexSetRGBA(imv + 1, red, green, blue, alpha);
			RwIm3DVertexSetRGBA(imv + 2, red, green, blue, alpha);
		
			imv += 3;
			vert_count += 3;
			
			vIn[0].x = (FLOAT)(j+1)*diff; 
			vIn[0].z = (FLOAT)i*diff;
			vIn[0].y = m_fHeightSave[tj+1][ti];
			
			vIn[1].x = (FLOAT)j*diff;
			vIn[1].z = (FLOAT)(i+1)*diff;
			vIn[1].y = m_fHeightSave[tj][ti+1];

			vIn[2].x = (FLOAT)(j+1)*diff;
			vIn[2].z = (FLOAT)(i+1)*diff;
			vIn[2].y = m_fHeightSave[tj+1][ti+1];

			RwIm3DVertexSetPos(imv, vIn[0].x, vIn[0].y, vIn[0].z);
			RwIm3DVertexSetPos(imv + 1, vIn[1].x, vIn[1].y, vIn[1].z);
			RwIm3DVertexSetPos(imv + 2, vIn[2].x, vIn[2].y, vIn[2].z);

			// Normal 계산
			//RwV3dSub(&a, &vIn[2],&vIn[0]);
			//RwV3dSub(&b, &vIn[1],&vIn[0]);

			//RwV3dCrossProduct(&normal1, &a, &b);
			//RwV3dNormalize(&normal1, &normal1);

			//RwV3dSub(&a, &vIn[1],&vIn[3]);
			//RwV3dSub(&b, &vIn[2],&vIn[3]);

			//RwV3dCrossProduct(&normal2, &a, &b);
			//RwV3dNormalize(&normal2, &normal2);

			//RwV3dAdd(&normal3 , &normal1 , &normal2);
			//RwV3dNormalize(&normal3, &normal3);

			//RwIm3DVertexSetNormal(imv, normal3.x , normal3.y , normal3.z );
			//RwIm3DVertexSetNormal(imv+1, normal3.x , normal3.y , normal3.z );
			//RwIm3DVertexSetNormal(imv+2, normal3.x , normal3.y , normal3.z );

			if(rot_angle == 0.0f)
			{
				RwIm3DVertexSetU(imv, (vIn[0].x - start_x) * uval);
				RwIm3DVertexSetU(imv + 1, (vIn[1].x - start_x) * uval);
				RwIm3DVertexSetU(imv + 2, (vIn[2].x - start_x) * uval);
			
				RwIm3DVertexSetV(imv, (vIn[0].z - start_z) * vval);
				RwIm3DVertexSetV(imv + 1, (vIn[1].z - start_z) * vval);
				RwIm3DVertexSetV(imv + 2, (vIn[2].z - start_z) * vval);
			}
			else
			{
				//float	sinangle = AcuMath::GetSinValFast((INT32)rot_angle);
				//float	cosangle = AcuMath::GetCosValFast((INT32)rot_angle);
				float	sinangle = AcuSinTbl::Sin(rot_angle);
				float	cosangle = AcuSinTbl::Cos(rot_angle);
				
				RwV2d	vOut,vInUV;

                vInUV.x = (vIn[0].x - start_x) * uval - 0.5f;
				vInUV.y = (vIn[0].z - start_z) * vval - 0.5f;
				vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
				vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
				RwIm3DVertexSetU(imv, vOut.x + 0.5f);
				RwIm3DVertexSetV(imv, vOut.y + 0.5f);

				vInUV.x = (vIn[1].x - start_x) * uval - 0.5f;
				vInUV.y = (vIn[1].z - start_z) * vval - 0.5f;
				vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
				vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
				RwIm3DVertexSetU(imv+1, vOut.x + 0.5f);
				RwIm3DVertexSetV(imv+1, vOut.y + 0.5f);

				vInUV.x = (vIn[2].x - start_x) * uval - 0.5f;
				vInUV.y = (vIn[2].z - start_z) * vval - 0.5f;
				vOut.x = vInUV.x * cosangle - vInUV.y * sinangle;
				vOut.y = vInUV.x * sinangle + vInUV.y * cosangle;
				RwIm3DVertexSetU(imv+2, vOut.x + 0.5f);
				RwIm3DVertexSetV(imv+2, vOut.y + 0.5f);
			}
			
			RwIm3DVertexSetRGBA(imv, red, green, blue, alpha);
			RwIm3DVertexSetRGBA(imv + 1, red, green, blue, alpha);
			RwIm3DVertexSetRGBA(imv + 2, red, green, blue, alpha);

			vert_count += 3;
			imv += 3;
		}
	}

	return vert_count;
}

//. 2005. 10. 31. Nonstopdj
void AgcmGlyph::SetVertsCompact(D3DVTX_PCT** ppVertex, WORD** ppIndies, int* vertex_count, int* index_count,  int* primitive_count,
								float posx, float posz, float fwidth, float fheight, RwReal radius, float rot_angle, RwRGBA	color)
{
	UINT8	red		= color.red;
	UINT8	green	= color.green;
	UINT8	blue	= color.blue;
	UINT8	alpha	= color.alpha;

	const RwReal	TILESIZE	= 400.f;
	const RwReal	SQRT2		= 1.41421356f;
	const RwReal	INV_2RADIUS = 1.f / (2.f * radius);
	const RwReal	DEFAULT_BOARDSIZE	= 7.064f*2.f;
	const RwUInt32	MAX_TERRAINBOARDSIZE = 10;

	//1. vertex num
	const RwReal	fw = SQRT2 * radius;	//충분히 큰 사정사각형의 폭/2.
	const POINT	ptstart = { static_cast<int>((posx - fw - ( (posx - fw < 0.f) ? TILESIZE : 0.f ) )/TILESIZE)
						  , static_cast<int>((posz - fw - ( (posz - fw < 0.f) ? TILESIZE : 0.f ) )/TILESIZE) };
	const POINT	ptend	= { static_cast<int>((posx + fw + ( (posx + fw < 0.f) ? 0.f : TILESIZE ) )/TILESIZE)
						  , static_cast<int>((posz + fw + ( (posz + fw < 0.f) ? 0.f : TILESIZE ) )/TILESIZE) };
	const RwV2d	lefttop = { static_cast<RwReal>(ptstart.x) * TILESIZE
						  , static_cast<RwReal>(ptstart.y) * TILESIZE };
	const RwInt32 width  = MIN( MAX_TERRAINBOARDSIZE, ptend.x - ptstart.x);
	const RwInt32 height = MIN( MAX_TERRAINBOARDSIZE, ptend.y - ptstart.y);

	*vertex_count		= (width+1)*(height+1);
	*index_count		= (width*height*6);
	*primitive_count	= (width*height*2);

	//. ASSERT( !(*vertex_count > GLYPH_MAX_VERTEX_COUNT) && "Glyph Vertex array size overflow!!");
	//. ASSERT( !(*index_count  > GLYPH_MAX_INDEX_COUNT) && "Glyph Index array size overflow!!");
	//. 2005. 11. 01. Nonstopdj
	//. vertex count, index count가 define된 크기보다 클 일이 없겠지만 alef의 예외를 생각해서.. 
	if(*vertex_count > GLYPH_MAX_VERTEX_COUNT)
	{
		delete[] *ppVertex;
		*ppVertex = new D3DVTX_PCT[*vertex_count];
	}
	if(*index_count > GLYPH_MAX_INDEX_COUNT)
	{
		delete[] *ppIndies;
		*ppIndies = new WORD[*index_count];
	}

	D3DVTX_PCT* pvtx	= *ppVertex;
	WORD* pIndies		= *ppIndies;

	//2. 버텍스 재설정.
	const mat2by2	matuv(rot_angle);
	RwReal	fz = lefttop.y;

	//((alpha) << 24) | ((red) << 16) | ((green) << 8) | ((blue));
	DWORD dwColor =  DEF_ARGB32(alpha, red, green, blue);

	for(int i=0, acpitch=0; i< height; ++i, acpitch+=(width+1), fz+=TILESIZE )
	{
		RwReal fv=(fz - (posz - radius)) * INV_2RADIUS - 0.5f;
		RwReal fx=lefttop.x;

		for(int j=0; j<width; ++j, fx+= TILESIZE, ++pvtx, pIndies += 6)
		{
			//vertex setting
			pvtx->m_v3Pos.x = fx;
			pvtx->m_v3Pos.z = fz;
			pvtx->m_v3Pos.y = m_pcmMap->HP_GetHeightGeometryOnly(fx, fz) + 5.f;
			pvtx->m_dwColr = dwColor;
			pvtx->m_v2Tutv.x = (fx - (posx - radius))*INV_2RADIUS - 0.5f;
			pvtx->m_v2Tutv.y = fv;
			//uv rotation
			RwV2dByMat2by2((RwV2d*)(&pvtx->m_v2Tutv), (RwV2d*)(&pvtx->m_v2Tutv), &matuv);
			pvtx->m_v2Tutv.x += 0.5f;
			pvtx->m_v2Tutv.y += 0.5f;

			BOOL bTurnEdge = TRUE;			

			ApWorldSector		* pSector = m_pcmMap->m_pcsApmMap->GetSector( fx , fz );
			ApCompactSegment	* pSegment;
			if( pSector && ( pSegment = pSector->C_GetSegment( fx , 0.0f , fz ) ) )
			{
				bTurnEdge	= pSegment->stTileInfo.GetEdgeTurn();
			}
			if(bTurnEdge)
			{
				*(pIndies+0) = static_cast<WORD>(acpitch+j+0);
				*(pIndies+1) = static_cast<WORD>(acpitch+j+1);
				*(pIndies+2) = static_cast<WORD>(acpitch+j+(width+1)+1);

				*(pIndies+3) = static_cast<WORD>(acpitch+j+0);
				*(pIndies+4) = static_cast<WORD>(acpitch+j+(width+1));
				*(pIndies+5) = static_cast<WORD>(acpitch+j+(width+1)+1);
			}
			else
			{
				*(pIndies+0) = static_cast<WORD>(acpitch+j+0);
				*(pIndies+1) = static_cast<WORD>(acpitch+j+1);
				*(pIndies+2) = static_cast<WORD>(acpitch+j+(width+1));

				*(pIndies+3) = static_cast<WORD>(acpitch+j+1);
				*(pIndies+4) = static_cast<WORD>(acpitch+j+(width+1)+1);
				*(pIndies+5) = static_cast<WORD>(acpitch+j+(width+1));
			}
		}
		//one more
		{
			//vertex setting
			pvtx->m_v3Pos.x = fx;
			pvtx->m_v3Pos.z = fz;
			pvtx->m_v3Pos.y = m_pcmMap->HP_GetHeightGeometryOnly(fx, fz) + 5.f;
			pvtx->m_dwColr = dwColor;
			pvtx->m_v2Tutv.x = (fx - (posx - radius))*INV_2RADIUS - 0.5f;
			pvtx->m_v2Tutv.y = fv;
			//uv rotation
			RwV2dByMat2by2((RwV2d*)(&pvtx->m_v2Tutv), (RwV2d*)(&pvtx->m_v2Tutv), &matuv);
			pvtx->m_v2Tutv.x += 0.5f;
			pvtx->m_v2Tutv.y += 0.5f;

			++pvtx;
		}
	}

	//one more
	{
		RwReal fv=(fz - (posz - radius))*INV_2RADIUS - 0.5f;
		RwReal fx=lefttop.x;
		for(int j=0; j < width+1; ++j, fx+= TILESIZE, ++pvtx)
		{
			//vertex setting
			pvtx->m_v3Pos.x = fx;
			pvtx->m_v3Pos.z = fz;
			pvtx->m_v3Pos.y =  m_pcmMap->HP_GetHeightGeometryOnly(fx, fz) + 5.f;
			pvtx->m_dwColr = dwColor;
			pvtx->m_v2Tutv.x = (fx - (posx - radius))*INV_2RADIUS - 0.5f;
			pvtx->m_v2Tutv.y = fv;
			//uv rotation
			RwV2dByMat2by2((RwV2d*)(&pvtx->m_v2Tutv), (RwV2d*)(&pvtx->m_v2Tutv), &matuv);
			pvtx->m_v2Tutv.x += 0.5f;
			pvtx->m_v2Tutv.y += 0.5f;
		}
	}


	//. 2005. 11. 1. Nonstopdj
	//. 한번 더 Render해줄 Clump List를 구성한다.
	m_RBObjClumplist.clear();

	POINT ptIndexInSector = {0, 0};
	ApWorldSector* pSector = m_pcmMap->m_pcsApmMap->GetSector( posx, posz );
	if( pSector )
	{
		pSector->D_GetSegment(posx, posz, (RwInt32*)(&(ptIndexInSector.x)), (RwInt32*)(&(ptIndexInSector.y)) );
			
		RwInt32	ridableindexlist[ SECTOR_MAX_COLLISION_OBJECT_COUNT ] =  {0, };
		RwInt32 numOfObj = pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT, ptIndexInSector.x, ptIndexInSector.y, ridableindexlist , SECTOR_MAX_COLLISION_OBJECT_COUNT );
		ASSERT( numOfObj <= SECTOR_MAX_COLLISION_OBJECT_COUNT );

		for( int nindex = 0; nindex < numOfObj; ++nindex )
		{
			//. AWS_RIDABLEOBJECT의 clump*추가
			AddRenderClumplist(ridableindexlist[nindex]);
		}


		//. 2005. 11. 2. Nonstopdj
		//. Block Object의 RpClump*는 일단 list에 추가하지 않는다.
		//RwInt32	collisionindexlist[ SECTOR_MAX_COLLISION_OBJECT_COUNT ] =  {0, };
		//numOfObj = pSector->GetIndex( ApWorldSector::AWS_COLLISIONOBJECT, ptIndexInSector.x, ptIndexInSector.y, collisionindexlist );
		//ASSERT( numOfObj <= SECTOR_MAX_COLLISION_OBJECT_COUNT );

		//
		//for( nindex = 0; nindex < numOfObj; ++nindex )
		//{
		//	//. ridable object이면서 collision object인것은 제외시키고 push_back한다.
		//	if( ridableindexlist[nindex] != collisionindexlist[nindex])
		//	{
		//		//. AWS_COLLISIONOBJECT의 clump*추가
		//		AddRenderClumplist(collisionindexlist[nindex]);
		//	}
		//}
	}

}

//. 2005. 11. 3. Nonstopdj
//. RpClump*를 sector로딩후에 유효한지 보장하기 위해 sector로딩시점에 m_RBObjClumplist를 clear한다.
BOOL	AgcmGlyph::CB_CLEAR_CLUMPLIST( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmGlyph::CB_CLEAR_CLUMPLIST");
	AgcmGlyph* pThis = (AgcmGlyph*)pClass;
	
	if(pThis->m_RBObjClumplist.size() > 0)
		pThis->m_RBObjClumplist.clear();

	return TRUE;
}

//. 2005. 11. 1. Nonstopdj
//. nIndex로 부터 Clump*를 찾고 list에 push_back한다.
void	AgcmGlyph::AddRenderClumplist(const INT32 nIndex)
{
	ApdObject* pApdObject = m_pApmObject->GetObject(nIndex);
	AgcdObject*	pAgcdObject = m_pcmObject->GetObjectData( pApdObject );

	if( pApdObject && pAgcdObject )
	{
		AgcdObjectGroupList* pGrouplist = pAgcdObject->m_stGroup.m_pstList;
		while(pGrouplist)
		{
			//. object group list에 있는 모든 clump*를 push_back
			m_RBObjClumplist.push_back(pGrouplist->m_csData.m_pstClump);

			pGrouplist = pGrouplist->m_pstNext;
		}
	}
}

void AgcmGlyph::UpdateProjectivelyTextureMatrix(const float fposx, const float fposz, const float fwidth, const float fheight, const float fRadius, const float fRotationAng)
{
	LPDIRECT3DDEVICE9	pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());

	D3DXMatrixIdentity(&m_TextureMatrix);

	D3DXMATRIX matView;
	//HRESULT hRet = pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	RwD3D9GetTransform( D3DTS_VIEW, &matView);

	FLOAT fDeterminantD3DMatView = D3DXMatrixDeterminant(&matView);

	D3DXMATRIX matViewInv;
	D3DXMatrixInverse(&matViewInv, &fDeterminantD3DMatView, &matView);

	D3DXMATRIX matScale;
	memset(matScale, 0, sizeof(D3DXMATRIX));
	matScale._11 = 1.0f/fwidth;
	matScale._32 = 1.0f/fheight;
	matScale._41 = -fposx * matScale._11 + fwidth * 0.5f * matScale._11;
	matScale._42 = -fposz * matScale._32 + fheight * 0.5f * matScale._32;
	matScale._44 = 1.0f;

	D3DXMATRIXA16 matRotate;
	D3DXMatrixIdentity( &matRotate );
	D3DXMatrixRotationY( &matRotate, fRotationAng );

	m_TextureMatrix = matViewInv * matRotate * matScale;
}

//@{ 2006/04/04 burumal
BOOL AgcmGlyph::IsGlyphedFrame(const RwFrame* pSrcFrame)
{
	BOOL bReturn = FALSE;

	if ( !pSrcFrame )
		return bReturn;	
	
	if ( m_stDestEnemyTarget.bAdded ) 
	{
		if ( m_stDestEnemyTarget.movepos == pSrcFrame )
			bReturn = TRUE;
	}

	return bReturn;
}
//@}

//@{ 2006/03/08 burumal
VOID AgcmGlyph::ShowDestMoveTarget()
{
	m_bShowDestMoveTarget = TRUE;
}

VOID AgcmGlyph::HideDestMoveTarget()
{
	m_bShowDestMoveTarget = FALSE;
}
//@}