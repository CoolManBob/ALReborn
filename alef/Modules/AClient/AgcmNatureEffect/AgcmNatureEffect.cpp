#include "AgcmNatureEffect.h"
#include "AcuFrameMemory.h"
#include "AcuParticleDraw.h"
#include "ApModuleStream.h"

#include "AgcmUIConsole.h"

static FLOAT NE_EFFECT_FALLEN_START_Y	= 1500.0f	;	// 머리 위 15 미터 부터
static FLOAT NE_EFFECT_FALLEN_END_Y		= 800.0f	;	// 다리 밑 8 미터 까지..
static float BS_OFFSET					= 600.0f	;

#define NE_CHANGE_GAP	5000

static FLOAT				g_fDotValue = 0.0f;
#define PI 3.1415927f

FLOAT	__GetWorldAngle( RwV3d * pAt )
{
	// Y 좌표 무시.

	FLOAT	fArc;
	fArc = (FLOAT) atan( pAt->x / pAt->z ) ;

	while( fArc < 0 )  fArc += PI * 2;

	if( pAt->x < 0 && pAt->z < 0 )
	{
		fArc += PI;
	}
	if( pAt->x > 0 && pAt->z < 0 )
	{
		fArc -= PI;
	}

//	return 360.0f / PI / 2.0f * fArc;
	return fArc;
}

AgcmNatureEffect	* AgcmNatureEffect::m_pThis = NULL;

AgcmNatureEffect::AgcmNatureEffect() :
	m_pcsAgcmRender			( NULL ) ,
	m_pcsAgcmMap			( NULL ) ,
	m_pcsAgpmNatureEffect	( NULL ) ,
	m_bStopping				( FALSE )
{
	SetModuleName("AgcmNatureEffect");
	EnableIdle(TRUE);
	
	m_vecWind = m_stWind.vecWind;
	SetWindEnable( TRUE );
	
	m_listRoot = NULL;
	m_iBeforeRootIndexX = 0;
	m_iBeforeRootIndexZ = 0;

	m_ulLastTick  = 0;

	AgcmNatureEffect::m_pThis = this;
}

AgcmNatureEffect::~AgcmNatureEffect()
{

}

BOOL	AgcmNatureEffect::OnAddModule()
{
	m_pcsAgcmRender			= (AgcmRender		*) GetModule("AgcmRender"		);
	m_pcsAgcmMap			= (AgcmMap			*) GetModule("AgcmMap"			);
	m_pcsAgpmNatureEffect	= (AgpmNatureEffect	*) GetModule("AgpmNatureEffect"	);

	if(m_pcsAgcmRender)
	{
		m_pcsAgcmRender->SetCallbackPostRender2(CB_NATUREEFFECT_POSTRENDER,this);
	}

	if( m_pcsAgpmNatureEffect )
	{
		m_pcsAgpmNatureEffect->SetCallbackChangedNatureEffect( CBNatureEffectChanged , this );
	}

	return TRUE;
}

BOOL	AgcmNatureEffect::OnInit()
{
	AS_REGISTER_TYPE_BEGIN(AgcmNatureEffect, AgcmNatureEffect);
		AS_REGISTER_METHOD1(void, SetNatureEffect , int );
	AS_REGISTER_TYPE_END;

	return TRUE;
}

LuaGlue	LG_SetNatureEffect( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	INT32	nEff	= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );

	if( nEff == -1 )
		AgcmNatureEffect::m_pThis->StopNatureEffect(  );
	else
		AgcmNatureEffect::m_pThis->StopNatureEffect( TRUE );

	AgcmNatureEffect::m_pThis->StartNatureEffect( nEff );

	LUA_RETURN( TRUE );
}

void	AgcmNatureEffect::OnLuaInitialize( AuLua * pLua )
{
	luaDef MVLuaGlue[] = 
	{
		{"SetNatureEffect"		,	LG_SetNatureEffect		},
		{NULL					,	NULL					},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}

void	AgcmNatureEffect::SetNatureEffect( int nEff )
{
	SetWindEnable( TRUE );

	if( nEff == -1 )
		StopNatureEffect(  );
	else
		StopNatureEffect( TRUE );

	if( StartNatureEffect( nEff ) )
	{
		print_ui_console( _T("네이쳐 이펙트를 실행합니다.") );
		print_ui_console( _T("끄고싶을땐 -1을 인자로 넣으세요.") );
	}
	else
	{
		print_ui_console( _T("네이쳐 이펙트를 끕니다.") );
	}
}

void	AgcmNatureEffect::SetWind( FLOAT fXValue )
{
	if( 0.0f == fXValue )
	{
		print_ui_console( _T("바람 끕니다") );
		SetWindEnable( FALSE );
	}
	else
	{
		char	str[ 256 ];
		sprintf( str , "%.2f 값으로 바람을 설정합니다 ( 동쪽으로 불어요 )" , fXValue );
		print_ui_console( str );

		RwV3d	vWind;
		vWind.x = fXValue;
		vWind.y = 0;
		vWind.z = 0;
		SetWindVector( vWind );
		SetWindEnable( TRUE );
	}
}

BOOL	AgcmNatureEffect::OnDestroy()
{
	for(int i=0;i< ( int ) m_vectorTexture.size() ;++i)
	{
		if(m_vectorTexture[i])
		{
			RwTextureDestroy(m_vectorTexture[i]);
			m_vectorTexture[i] = NULL;
		}
	}
	m_vectorTexture.empty();

	ReleaseNatureEffectAll();

	return TRUE;
}

BOOL	AgcmNatureEffect::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmNatureEffect::OnIdle");

	//static int test = 0;
	//if(test++ == 500)
	//{
	//	StartNatureEffect(1);
	//}

	UINT32	tickdiff = ulClockCount - m_ulLastTick;
	m_ulLastTick = ulClockCount;

	UpdateNatureEffects(tickdiff);
	WindIdleProcess( ulClockCount );

	return TRUE;
}

void	AgcmNatureEffect::WindIdleProcess( UINT32 ulClockCount )
{
	if( GetWindEnable() )
	{
		float fTime = ( float ) ( ulClockCount ) / 10000.0f;
		float fValue = ( float ) ( sin( fTime ) * sin( fTime ) * cos( fTime / 3.0f ) * sin( fTime / 1.3f ) ) ;

		m_vecWind.x = m_stWind.vecWind.x * fValue;
		m_vecWind.y = m_stWind.vecWind.y * fValue;
		m_vecWind.z = m_stWind.vecWind.z * fValue;
	}
}

void	AgcmNatureEffect::ReleaseNatureEffectAll()
{
	NatureEffectRoot*	cur_root = m_listRoot;
	NatureEffectRoot*	remove_root;
	NatureEffectGroup*	cur_group;
	NatureEffectGroup*	remove_group;
	NatureEffect*		cur_effect;
	NatureEffect*		remove_effect;

	while(cur_root)
	{
		cur_group = cur_root->listgroup;
		while(cur_group)
		{
			cur_effect = cur_group->pList;
			while(cur_effect)
			{
				remove_effect = cur_effect;
				cur_effect = cur_effect->next;
				delete remove_effect;
			}

			remove_group = cur_group;
			cur_group = cur_group->next;
			delete remove_group;
		}

		remove_root = cur_root;
		cur_root = cur_root->next;
		delete remove_root;
	}

	m_listRoot = NULL;
}

BOOL	AgcmNatureEffect::StopNatureEffect( BOOL bForce )
{
	if( bForce || ( m_listRoot && GetStopFlag() ) )
	{
		// 강제면 그냥 때려버림 ..
		ReleaseNatureEffectAll();
		EnumCallback( AGCMNATUREEFFECT_END , NULL , ( PVOID ) &bForce  );

		return TRUE;
	}
	else
	{
		if( GetStopFlag() )
			return TRUE;// 진행중이다.

		if( m_listRoot )
		{
			// 콜백 번저 때리고..
			EnumCallback( AGCMNATUREEFFECT_END , NULL , ( PVOID ) &bForce  );

			// 스탑 플래그 설정.
			SetStopFlag();
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL	AgcmNatureEffect::StartNatureEffect(INT32		effID , BOOL bForce )
{
	if(!m_listRoot) InitRoots();
	if( -1 == effID ) return FALSE;

	m_bStopping = FALSE;

	if (effID >= ( int ) m_vectorstNEInfo.size())
		return FALSE;
	
#ifdef	_DEBUG
// 기존에 있는지 체크
	NatureEffectGroup*	pGroup = m_listRoot->listgroup;
	while(pGroup)
	{
		if(effID == pGroup->effID)
		{
			ASSERT(!"같은 Nature Effect를 또 발동 시키려 하고 있음");
		}
		
		pGroup = pGroup->next;
	}
#endif

	NatureEffectInfo * pstNEInfo = &m_vectorstNEInfo[ effID ];

	if( !EnumCallback( AGCMNATUREEFFECT_START , ( PVOID ) pstNEInfo , ( PVOID ) &bForce  ) )
	{
		// 스카이에서 막힌경우..
		// 또는 다른콜백에서 리턴을 False 낸 경우..
		return FALSE;
	}

	// 각 root에 group을 추가하고 group안에 effect객체를 생성한다
	NatureEffectRoot*	cur_root = m_listRoot;
	NatureEffectGroup*	nw_group;
	NatureEffect*		nw_effect;
	enumNatureEffectType	eType = pstNEInfo->eType;

	UINT8		red,green,blue,alpha;
	INT32		minx,minz,xdiff,zdiff;
	INT32		iDensityMin		= (INT32)(2000.0f * pstNEInfo->fDensityMin);
	INT32		iDensityDiff	= (INT32)(2000.0f * pstNEInfo->fDensityMax - iDensityMin);
	float		fMakeDiff		;
	float		fMakeHeight		;

	float		fSizeDiff		= pstNEInfo->fSizeMax	- pstNEInfo->fSizeMin;
	float		fSpeedMin		= pstNEInfo->fSpeedMin;
	float		fSpeedDiff		= pstNEInfo->fSpeedMax	- fSpeedMin;
	float		fSwingMin		= pstNEInfo->fSwingMin;
	float		fSwingDiff		= pstNEInfo->fSwingMax	- fSwingMin;
	
	DWORD		rand_color[32];
	INT32		iRedDiff		= pstNEInfo->cRedMax	- pstNEInfo->cRedMin	;
	INT32		iGreenDiff		= pstNEInfo->cGreenMax	- pstNEInfo->cGreenMin	;
	INT32		iBlueDiff		= pstNEInfo->cBlueMax	- pstNEInfo->cBlueMin	;
	INT32		iAlphaDiff		= pstNEInfo->cAlphaMax	- pstNEInfo->cAlphaMin	;

	// 랜덤 칼라셋 준비함.
	for(int i=0;i<32;++i)
	{
		if(iRedDiff) red = pstNEInfo->cRedMin + rand() % iRedDiff; 
		else red = pstNEInfo->cRedMin;
		if(iGreenDiff)	green = pstNEInfo->cGreenMin + rand() % iGreenDiff;
		else green = pstNEInfo->cGreenMin;
		if(iBlueDiff) blue = pstNEInfo->cBlueMin + rand() % iBlueDiff;
		else blue = pstNEInfo->cBlueMin;
		if(iAlphaDiff)	alpha = pstNEInfo->cAlphaMin + rand() % iAlphaDiff;
		else alpha = alpha = pstNEInfo->cAlphaMin;

		rand_color[i] = ARGB32_TO_DWORD(alpha,blue,green,red);
	}

	while(cur_root)
	{
		minx = (INT32)cur_root->region.inf.x;
		minz = (INT32)cur_root->region.inf.z;

		xdiff = (INT32)(cur_root->region.sup.x - minx);
		zdiff = (INT32)(cur_root->region.sup.z - minz);

		nw_group = new NatureEffectGroup;

		nw_group->effID = effID;
		if(iDensityDiff)	nw_group->effectNum = iDensityMin + rand() % iDensityDiff;
		else nw_group->effectNum = iDensityMin;

		nw_group->pList = NULL;
		nw_group->next = cur_root->listgroup;
		cur_root->listgroup = nw_group;

		switch( eType )
		{
		case NATURE_EFFECT_SNOW:
			{
				fMakeHeight = cur_root->fCenterHeight + NE_EFFECT_FALLEN_START_Y;
				// 1000정도의 여분을 두자.. 센터 지형 높이보다.. 리프레쉬시 1000정도의 여분을 두므로.
				fMakeDiff = (NE_EFFECT_FALLEN_START_Y + 1000.0f) / (float)nw_group->effectNum;
				for(int i=0;i<nw_group->effectNum;++i)
				{
					nw_effect = new NatureEffect;
					
					nw_effect->vPos.x = (float)(minx + rand() % xdiff);
					nw_effect->vPos.y = fMakeHeight + i * fMakeDiff;
					nw_effect->vPos.z = (float)(minz + rand() % zdiff);

					nw_effect->iWaitCount = 0;

					nw_effect->fWidth = nw_effect->fHeight = pstNEInfo->fSizeMin + (float)(rand() % 64) * fSizeDiff;
					nw_effect->dwColor = rand_color[rand() % 32];

					nw_effect->fParticleRotAngle = 0.0f;
					nw_effect->fMoveAngle = 0.0f;

					nw_effect->fDownSpeed = ( fSpeedMin + (float)(rand() % 100) * fSpeedDiff ) / 100.0f;
					nw_effect->fSpeed = fSwingMin * 10.0f + (float)(rand() % 10) * fSwingDiff;
									
					nw_effect->next = nw_group->pList;
					nw_group->pList = nw_effect;
				}
			}
			break;

		case NATURE_EFFECT_RAIN					:
		case NATURE_EFFECT_RAIN_WITH_THUNDER	:
		case NATURE_EFFECT_RAIN_WITH_FOG		:
			{
				fMakeHeight = cur_root->fCenterHeight + NE_EFFECT_FALLEN_START_Y;
				// 1000cm정도의 여분을 두자.. 센터 지형 높이보다.. 리프레쉬시 1000정도의 여분을 두므로.
				fMakeDiff = (NE_EFFECT_FALLEN_START_Y + 1000.0f) / (float)nw_group->effectNum;
				for(int i=0;i<nw_group->effectNum;++i)
				{
					nw_effect = new NatureEffect;
					
					nw_effect->vPos.x = (float)(minx + rand() % xdiff);
					nw_effect->vPos.y = fMakeHeight + i * fMakeDiff;
					nw_effect->vPos.z = (float)(minz + rand() % zdiff);

					nw_effect->iWaitCount = 0;

					nw_effect->fWidth = 10.0f;
					nw_effect->fHeight = 64.0f * pstNEInfo->fSizeMin + (float)(rand() % 64) * fSizeDiff;
					nw_effect->dwColor = rand_color[rand() % 32];

					nw_effect->fParticleRotAngle = 0.0f;
					nw_effect->fMoveAngle = 0.0f;

					nw_effect->fDownSpeed = fSpeedMin + (float)(rand() % 1000) * fSpeedDiff * 0.001f;
					nw_effect->fSpeed = 0.0f;
								
					nw_effect->next = nw_group->pList;
					nw_group->pList = nw_effect;
				}
			}
			break;
		default:
			ASSERT( !"없는타입" );
			return FALSE;
		}

		cur_root = cur_root->next;
	}
	
	return TRUE;
}

BOOL	AgcmNatureEffect::EndNatureEffect(INT32		effID, BOOL bForce )
{
	NatureEffectRoot*	cur_root = m_listRoot;
	
	NatureEffectGroup*	cur_group;
	NatureEffectGroup*	bef_group;
	NatureEffect*		cur_effect;
	NatureEffect*		remove_effect;

	while(cur_root)
	{
		cur_group = cur_root->listgroup;
		while(cur_group)
		{
			if(cur_group->effID == effID)
			{
				cur_effect = cur_group->pList;
				while(cur_effect)
				{
					remove_effect = cur_effect;
					cur_effect = cur_effect->next;

					delete remove_effect;
				}
	
				if(cur_group == cur_root->listgroup)
				{
					cur_root->listgroup = cur_group->next;
				}
				else
				{
					bef_group->next = cur_group->next;
				}
				delete	cur_group;
				break;
			}

			bef_group = cur_group;
			cur_group = cur_group->next;
		}

		cur_root = cur_root->next;
	}

	return TRUE;
}

void	AgcmNatureEffect::InitRoots()
{
	if(!m_pcsAgcmRender || !m_pcsAgcmRender->m_pFrame) return;
	ASSERT(!m_listRoot);

	RwMatrix*	pLTM = RwFrameGetLTM(m_pcsAgcmRender->m_pFrame);
	RwV3d*		pPos = RwMatrixGetPos(pLTM);

	INT32		center_index_x = (INT32)(pPos->x /NE_ROOT_REGION_SIZE);
	INT32		center_index_z = (INT32)(pPos->z /NE_ROOT_REGION_SIZE);
	INT32		end_index_x = center_index_x + 2;
	INT32		end_index_z = center_index_z + 2;

	NatureEffectRoot*	nw_root;
	float		min_x,min_z;
    
	for(int i = center_index_x - 2; i<= end_index_x; ++i)
	{
		for(int j = center_index_z - 2; j<= end_index_z; ++j)
		{
			nw_root = new	NatureEffectRoot;

            min_x = i * NE_ROOT_REGION_SIZE;
			min_z = j * NE_ROOT_REGION_SIZE;

			nw_root->bs.center.x = min_x + NE_ROOT_REGION_SIZE_HALF;
			nw_root->bs.center.z = min_z + NE_ROOT_REGION_SIZE_HALF;
			//nw_root->fCenterHeight = m_pcsAgcmMap->GetHeight(nw_root->bs.center.x,nw_root->bs.center.z , SECTOR_MAX_HEIGHT );
			nw_root->fCenterHeight = pPos->y;
			
			nw_root->bs.center.y = nw_root->fCenterHeight + BS_OFFSET;
			nw_root->bs.radius = NE_ROOT_REGION_SIZE_HALF;

			nw_root->region.inf.x = min_x;
			nw_root->region.inf.y = 0.0f;
			nw_root->region.inf.z = min_z;

			nw_root->region.sup.x = min_x + NE_ROOT_REGION_SIZE;
			nw_root->region.sup.y = 0.0f;
			nw_root->region.sup.z = min_z + NE_ROOT_REGION_SIZE;

			nw_root->xindex = i;
			nw_root->zindex = j;

			nw_root->listgroup = NULL;
			nw_root->bForceUpdate = TRUE;

			nw_root->next = m_listRoot;
			m_listRoot = nw_root;
		}
	}

	m_iBeforeRootIndexX = center_index_x;
	m_iBeforeRootIndexZ = center_index_z;
}

void	AgcmNatureEffect::RefreshRoots()
{
	if(!m_pcsAgcmRender || !m_pcsAgcmRender->m_pFrame) return;
	if(!m_listRoot) return;
	
    RwMatrix*	pLTM = RwFrameGetLTM(m_pcsAgcmRender->m_pFrame);
	RwV3d*		pPos = RwMatrixGetPos(pLTM);

	INT32		center_index_x = (INT32)(pPos->x /NE_ROOT_REGION_SIZE);
	INT32		center_index_z = (INT32)(pPos->z /NE_ROOT_REGION_SIZE);

	// 업데이트 불필요
	if(center_index_x == m_iBeforeRootIndexX && center_index_z == m_iBeforeRootIndexZ)
	{
		// 높이만 조절함.
		NatureEffectRoot*	cur_root = m_listRoot;

		while(cur_root)
		{
			cur_root->fCenterHeight = pPos->y;
			cur_root = cur_root->next;
		}
		return;
	}

	INT32		end_index_x = center_index_x + 2;
	INT32		end_index_z = center_index_z + 2;

	NatureEffectRoot*	cur_root = m_listRoot;
	NatureEffectRoot*	bef_root;
	NatureEffectRoot*	recycle_root;
	float		min_x,min_z;

	INT32		xdiff,zdiff;

	NatureEffectRoot*	listRecycle = NULL;

	while(cur_root)
	{
		if(cur_root->xindex > center_index_x)
			xdiff = cur_root->xindex - center_index_x;
		else
			xdiff = center_index_x - cur_root->xindex;
		
		if(cur_root->zindex > center_index_z)
			zdiff = cur_root->zindex - center_index_z;
		else
			zdiff = center_index_z - cur_root->zindex;

		if(xdiff > 2 || zdiff > 2) // 재활용 list에 넣기
		{
			if(cur_root == m_listRoot)
			{
				m_listRoot = cur_root->next;
			}
			else
			{
				bef_root->next = cur_root->next;
			}
			recycle_root = cur_root;
			cur_root = cur_root->next;
			
			recycle_root->next = listRecycle;
			listRecycle = recycle_root;
		}
		else
		{
			// 높이를 카메라 높이로 맞춤..
			cur_root->fCenterHeight = pPos->y;


			bef_root = cur_root;
			cur_root = cur_root->next;
		}
	}

	if(!listRecycle) return;

	INT32	before_min_x = m_iBeforeRootIndexX - 2;
	INT32	before_max_x = m_iBeforeRootIndexX + 2;
	INT32	before_min_z = m_iBeforeRootIndexZ - 2;
	INT32	before_max_z = m_iBeforeRootIndexZ + 2;

	NatureEffectGroup*	cur_group;
	NatureEffect*		cur_effect;

	for(int i = center_index_x - 2; i<= end_index_x; ++i)
	{
		for(int j = center_index_z - 2; j<= end_index_z; ++j)
		{
			// 이미 있음
			if(i >= before_min_x && i <= before_max_x && j >= before_min_z && j <= before_max_z)
				continue;

			ASSERT(listRecycle);
            cur_root = listRecycle;
			listRecycle = listRecycle->next;

            min_x = i * NE_ROOT_REGION_SIZE;
			min_z = j * NE_ROOT_REGION_SIZE;

			cur_root->bs.center.x = min_x + NE_ROOT_REGION_SIZE_HALF;
			cur_root->bs.center.z = min_z + NE_ROOT_REGION_SIZE_HALF;
			//cur_root->fCenterHeight = m_pcsAgcmMap->GetHeight(cur_root->bs.center.x,cur_root->bs.center.z , SECTOR_MAX_HEIGHT );
			cur_root->fCenterHeight = pPos->y;

			cur_root->bs.center.y = cur_root->fCenterHeight + BS_OFFSET;
			cur_root->bs.radius = NE_ROOT_REGION_SIZE_HALF;

			xdiff = (INT32)(min_x - cur_root->region.inf.x);
			zdiff = (INT32)(min_z - cur_root->region.inf.z);

			cur_root->region.inf.x = min_x;
			cur_root->region.inf.z = min_z;

			cur_root->region.sup.x = min_x + NE_ROOT_REGION_SIZE;
			cur_root->region.sup.z = min_z + NE_ROOT_REGION_SIZE;

			cur_root->xindex = i;
			cur_root->zindex = j;

			// 객체들 상태 업데이트(포지션등..)
			cur_group = cur_root->listgroup;
			while(cur_group)
			{
				cur_effect = cur_group->pList;
				while(cur_effect)
				{
					cur_effect->vPos.x += (float)xdiff;
					cur_effect->vPos.z += (float)zdiff;
	
					cur_effect = cur_effect->next;
				}

				cur_group = cur_group->next;
			}

			cur_root->next = m_listRoot;
			m_listRoot = cur_root;
		}
	}

	m_iBeforeRootIndexX = center_index_x;
	m_iBeforeRootIndexZ = center_index_z;
}

void	AgcmNatureEffect::UpdateNatureEffects(UINT32	tickdiff)
{
	if( !m_pcsAgcmRender->m_pCamera ) return;

	NatureEffectRoot*		cur_root = m_listRoot;
	NatureEffectGroup*		cur_group;
	NatureEffect*			cur_effect;
	enumNatureEffectType	eType;
	FLOAT					fCenterHeight;
	RwFrustumTestResult		res;
	RwCamera*				pCheckCamera = m_pcsAgcmRender->m_pCamera;
	RwV3d	*				pAt;

	{
		// 카메라의 At 벡터 구하기.
		RwMatrix * pMatrix = RwFrameGetLTM( RwCameraGetFrame( m_pcsAgcmRender->m_pCamera ) );
		pAt = RwMatrixGetAt( pMatrix );
	}

	FLOAT					fTickDiff = (float)tickdiff;

	FLOAT					fXWindDiff = 0.0f;
	FLOAT					fZWindDiff = 0.0f;
	RwV3d	* pWind = GetWindVector();
	FLOAT	fWindDir = __GetWorldAngle( pWind );
	FLOAT	fWindAngle = fWindDir - __GetWorldAngle( pAt );

	if( GetWindEnable() )
	{
		// 바람에 의해서 날리는 양 계산..
		fXWindDiff  = pWind->x * fTickDiff;
		fZWindDiff  = pWind->z * fTickDiff;
	}

	BOOL	bDotCalcOnce = TRUE;

	while(cur_root)
	{
		// 바운딩 스피어 체크
		res = RwCameraFrustumTestSphere( pCheckCamera, &cur_root->bs );
		if(res == rwSPHEREOUTSIDE)
		{
			cur_root->bCameraIn = FALSE;
			if(!cur_root->bForceUpdate)
			{
				cur_root = cur_root->next;
				continue;
			}
		}
		else
		{
			cur_root->bCameraIn = TRUE;
		}

		fCenterHeight = cur_root->fCenterHeight;
		cur_group = cur_root->listgroup;

		while(cur_group)
		{
			NatureEffectInfo * pstNEInfo = &m_vectorstNEInfo[ cur_group->effID ];

			cur_effect = cur_group->pList;
			eType = pstNEInfo->eType;
			
			switch( eType )
			{
			case NATURE_EFFECT_SNOW:
				/*{
					while(cur_effect)
					{
						if(cur_effect->iWaitCount < 0)
						{
							cur_effect->iWaitCount += tickdiff;
							cur_effect = cur_effect->next;
							continue;
						}

						// 눈 갱신~


						cur_effect = cur_effect->next;
					}
				}
				break;*/

			case NATURE_EFFECT_RAIN					:
			case NATURE_EFFECT_RAIN_WITH_THUNDER	:
			case NATURE_EFFECT_RAIN_WITH_FOG		:
				{
					if( GetWindEnable() )
					{
						while(cur_effect)
						{
							if(cur_effect->iWaitCount < 0)
							{
								cur_effect->iWaitCount += tickdiff;
								cur_effect = cur_effect->next;
								continue;
							}

							//바람의 영향을 받는 비 갱신~
							FLOAT	fDropDist = cur_effect->fDownSpeed * fTickDiff;
							cur_effect->vPos.y -= fDropDist;

							// Falling
							if( cur_effect->vPos.y < fCenterHeight - NE_EFFECT_FALLEN_END_Y)
							{
								cur_effect->vPos.y += NE_EFFECT_FALLEN_START_Y + NE_EFFECT_FALLEN_END_Y;
								cur_root->bForceUpdate = FALSE;
							}

							// 바람 처리부.
							cur_effect->vPos.x += fXWindDiff;
							cur_effect->vPos.z += fZWindDiff;

							if( bDotCalcOnce )
							{
								g_fDotValue = ( float ) sin( ( float )fWindAngle ) * sqrt( fXWindDiff * fXWindDiff + fZWindDiff * fZWindDiff );
							}

							// 넘어가는것 돌려놓기..
							if( fXWindDiff < 0.0f )	while( cur_effect->vPos.x < cur_root->region.inf.x ) cur_effect->vPos.x += NE_ROOT_REGION_SIZE;
							else					while( cur_effect->vPos.x > cur_root->region.sup.x ) cur_effect->vPos.x -= NE_ROOT_REGION_SIZE;

							if( fZWindDiff < 0.0f )	while( cur_effect->vPos.z < cur_root->region.inf.z ) cur_effect->vPos.z += NE_ROOT_REGION_SIZE;
							else					while( cur_effect->vPos.z > cur_root->region.sup.z ) cur_effect->vPos.z -= NE_ROOT_REGION_SIZE;
							
							cur_effect = cur_effect->next;
						}
					}
					else
					{
						while(cur_effect)
						{
							if(cur_effect->iWaitCount < 0)
							{
								cur_effect->iWaitCount += tickdiff;
								cur_effect = cur_effect->next;
								continue;
							}

							//바람의 영향을 받지 않는 비 갱신~
							cur_effect->vPos.y -= cur_effect->fDownSpeed * fTickDiff;

							// Falling
							if(cur_effect->vPos.y < fCenterHeight - NE_EFFECT_FALLEN_END_Y && !GetStopFlag() )
							{
								cur_effect->vPos.y += NE_EFFECT_FALLEN_START_Y + NE_EFFECT_FALLEN_END_Y;
								cur_root->bForceUpdate = FALSE;
							}
							
							cur_effect = cur_effect->next;
						}
					}
				}
				break;
			case NATURE_EFFECT_SANDSTORM:
				{
					while(cur_effect)
					{
						if(cur_effect->iWaitCount < 0)
						{
							cur_effect->iWaitCount += tickdiff;
							cur_effect = cur_effect->next;
							continue;
						}

						//모래폭풍 갱신~

						cur_effect = cur_effect->next;
					}
				}

			default:
				ASSERT( !"응? -_-" );
				break;
			}
			
			cur_group = cur_group->next;
		}

		cur_root = cur_root->next;
	}

	RefreshRoots();

	if( GetStopFlag() )
	{
		if( GetLooseTime() >= NE_CHANGE_GAP )
		{
			// 다 끝났으니 스톱시킴..
			ReleaseNatureEffectAll();
			m_bStopping = FALSE;
		}
		else
		{
			// 알파값 갱신..

		}
	}
}

BOOL	AgcmNatureEffect::CB_NATUREEFFECT_POSTRENDER ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmNatureEffect::CB_POSTRENDER");

	AgcmNatureEffect*	pThis = (AgcmNatureEffect*) pClass;

	NatureEffectRoot*	cur_root = pThis->m_listRoot;
	NatureEffectGroup*	cur_group;
	NatureEffect*		cur_effect;
	RwTexture*			pTexture;

	BYTE	aAlphaTable[ 0xff ];
	if( pThis->GetStopFlag() )
	{
		// 마고자 (2006-03-20 오전 10:35:05) : 
		// 스카이변경시 사용할 알파 플래그..
		// 한번에 미리 계산해서 테이블 적용.
		FLOAT fRatio = 1.0f - ( FLOAT ) pThis->GetLooseTime() / ( FLOAT ) NE_CHANGE_GAP;
		for( int i = 0 ; i < 0xff ; i ++ )
		{
			aAlphaTable[ i ] = ( BYTE ) ( ( FLOAT ) i * fRatio );
		}
	}

	while(cur_root)
	{
		if(cur_root->bCameraIn)
		{
			cur_group = cur_root->listgroup;
			while(cur_group)
			{
				ASSERT( cur_group->effID < NATURE_EFFECT_NUM );
				if( cur_group->effID < NATURE_EFFECT_NUM )
				{
					NatureEffectInfo * pstNEInfo = &pThis->m_vectorstNEInfo[ cur_group->effID ];

					pTexture = pThis->m_vectorTexture[ pstNEInfo->texID ];

					switch( pstNEInfo->eType )
					{
					case NATURE_EFFECT_SNOW					:
					default:
						{
							AcuParticleDraw::DrawStart(pTexture,PARTICLE_BILLBOARD);
							cur_effect = cur_group->pList;
							while(cur_effect)
							{
								if(cur_effect->iWaitCount >= 0)
								{
									DWORD	dwColor = cur_effect->dwColor;
									if( pThis->GetStopFlag() )
									{
										dwColor =	( cur_effect->dwColor & 0x00ffffff ) |
													( aAlphaTable[ ( cur_effect->dwColor & 0xff000000 ) >> 24 ] << 24 );
									}
									AcuParticleDraw::AccumulateBuffer(&cur_effect->vPos,dwColor,cur_effect->fWidth,cur_effect->fHeight);
								}
								cur_effect = cur_effect->next;
							}
						}
						break;
					case NATURE_EFFECT_RAIN					:
					case NATURE_EFFECT_RAIN_WITH_THUNDER	:
					case NATURE_EFFECT_RAIN_WITH_FOG		:
					case NATURE_EFFECT_SANDSTORM			:
						{
							AcuParticleDraw::DrawStart(pTexture,PARTICLE_YBILLBOARD);
							cur_effect = cur_group->pList;
							while(cur_effect)
							{
								if(cur_effect->iWaitCount >= 0)
								{
									DWORD	dwColor = cur_effect->dwColor;
									if( pThis->GetStopFlag() )
									{
										dwColor =	( cur_effect->dwColor & 0x00ffffff ) |
													( aAlphaTable[ (cur_effect->dwColor & 0xff000000 ) >> 24 ] << 24 );
									}
									AcuParticleDraw::AccumulateBufferRotationSelf(&cur_effect->vPos,dwColor,cur_effect->fWidth,cur_effect->fHeight , g_fDotValue );
								}
								cur_effect = cur_effect->next;
							}
						}
						break;
					}  

					AcuParticleDraw::DrawEnd();
				}

				cur_group = cur_group->next;
			}
		}

		cur_root = cur_root->next;
	}

	return TRUE;
}

//Streaming
BOOL	AgcmNatureEffect::LoadNatureEffectInfoFromINI(char*	szFile, BOOL bDecryption)
{
	if (!szFile) return FALSE;

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	CHAR			szFileName[256];
	INT32			index;
		
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	RwTextureSetAutoMipmapping(TRUE);
	RwTextureSetMipmapping(TRUE);

	// szFile을 읽는다.
	VERIFY(csStream.Open(szFile, 0 , bDecryption) && "NatureEffect INI파일을 읽지 못했습니다");

	{
		csStream.ReadSectionName(0);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, AGCMNATUREEFFECT_INI_NAME_TEXTURE_NAME))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue,"%d:%s",&index,szFileName);

				RwTexture * pTexture = RwTextureRead(szFileName,NULL);
				ASSERT( pTexture );

				if( pTexture )
				{
					RwTextureSetFilterMode( pTexture , rwFILTERLINEAR );
				}

				m_vectorTexture.push_back( pTexture );
			}
		}
	}

	{
		csStream.ReadSectionName(1);
		csStream.SetValueID(-1);
		INT32			lID = 0;
		NatureEffectInfo	* pstNEInfo;

		#define GET_UINT8_VALUE( vcValue ) { INT32 nValue; sscanf(szValue, "%d", &nValue );  vcValue = ( UINT8 ) nValue; }

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();
			while ( (INT32) m_vectorstNEInfo.size() <= lID)
			{
				NatureEffectInfo neInfo;

				m_vectorstNEInfo.push_back(neInfo);
			}

			pstNEInfo = &m_vectorstNEInfo[ lID ];

			if(!strcmp(szValueName, AGCMNATUREEFFECT_INI_NAME_NEFFECT_ID))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &lID);

				/*
				NatureEffectInfo neInfo;
				m_vectorstNEInfo.push_back( neInfo );
				*/

				ASSERT(lID < NATURE_EFFECT_NUM);
			}
			else if(!strcmp(szValueName, AGCMNATUREEFFECT_INI_NAME_TEXTURE_ID))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &pstNEInfo->texID ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_TYPE"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &pstNEInfo->eType ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_SIZE_MIN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fSizeMin ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_SIZE_MAX"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fSizeMax ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_SPEED_MIN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fSpeedMin ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_SPEED_MAX"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fSpeedMax ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_SWING_MIN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fSwingMin ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_SWING_MAX"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fSwingMax ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_DENSITY_MIN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fDensityMin ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_DENSITY_MAX"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &pstNEInfo->fDensityMax ); 
			}
			else if(!strcmp(szValueName, "NEFFECT_RED_MIN"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cRedMin )
			}
			else if(!strcmp(szValueName, "NEFFECT_RED_MAX"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cRedMax )
			}
			else if(!strcmp(szValueName, "NEFFECT_GREEN_MIN"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cGreenMin )
			}
			else if(!strcmp(szValueName, "NEFFECT_GREEN_MAX"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cGreenMax )
			}
			else if(!strcmp(szValueName, "NEFFECT_BLUE_MIN"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cBlueMin )
			}
			else if(!strcmp(szValueName, "NEFFECT_BLUE_MAX"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cBlueMax )
			}
			else if(!strcmp(szValueName, "NEFFECT_ALPHA_MIN"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cAlphaMin )
			}
			else if(!strcmp(szValueName, "NEFFECT_ALPHA_MAX"))
			{
				csStream.GetValue(szValue, 256);
				GET_UINT8_VALUE( pstNEInfo->cAlphaMax )
			}
			else if(!strcmp(szValueName, "NEFFECT_SKYSET"))
			{
				csStream.GetValue(szValue, 256);

				INT32 nValue;
				sscanf(szValue, "%d", &nValue ); 
				pstNEInfo->nSkySet = nValue;
			}
		}
	}

	return TRUE;
}

BOOL	AgcmNatureEffect::SaveNatureEffectInfoToINI(char*		szFileName, BOOL bEncryption )
{
	CHAR szValue[255];

	ApModuleStream csStream;
	if(!csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE))	return FALSE;
	
	if(!csStream.SetSection("Textures")) return FALSE;

	// 텍스쳐 라이팅..
	{
		int i = 0;
		vector<RwTexture*>::iterator Iter;
		// Ridable과 겹치는 경우.
		for ( Iter = m_vectorTexture.begin( ) ; Iter != m_vectorTexture.end( ) ; Iter++ , i++)
		{
			RwTexture * pTexture = (*Iter);
			sprintf(szValue, "%d:%s", i, RwTextureGetName(pTexture));
			if(!csStream.WriteValue(AGCMNATUREEFFECT_INI_NAME_TEXTURE_NAME, szValue))
				return FALSE;
		}
	}

	if(!csStream.SetSection("NatureEffectInfo"))	return FALSE;

	// 이펙트 설정 라이팅.
	{
		int i = 0;
		vector<NatureEffectInfo>::iterator Iter;
		for ( Iter = m_vectorstNEInfo.begin( ) ; Iter != m_vectorstNEInfo.end( ) ; Iter++ , i++)
		{
			NatureEffectInfo * pstNEInfo = &(*Iter);

			sprintf(szValue, "%d", i);
			if(!csStream.WriteValue(AGCMNATUREEFFECT_INI_NAME_NEFFECT_ID, szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->texID);
			if(!csStream.WriteValue(AGCMNATUREEFFECT_INI_NAME_TEXTURE_ID, szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->eType);
			if(!csStream.WriteValue("NEFFECT_TYPE", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fSizeMin);
			if(!csStream.WriteValue("NEFFECT_SIZE_MIN", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fSizeMax);
			if(!csStream.WriteValue("NEFFECT_SIZE_MAX", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fSpeedMin);
			if(!csStream.WriteValue("NEFFECT_SPEED_MIN", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fSpeedMax);
			if(!csStream.WriteValue("NEFFECT_SPEED_MAX", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fSwingMin);
			if(!csStream.WriteValue("NEFFECT_SWING_MIN", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fSwingMax);
			if(!csStream.WriteValue("NEFFECT_SWING_MAX", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fDensityMin);
			if(!csStream.WriteValue("NEFFECT_DENSITY_MIN", szValue))
				return FALSE;

			print_compact_format(szValue, "%f", pstNEInfo->fDensityMax);
			if(!csStream.WriteValue("NEFFECT_DENSITY_MAX", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cRedMin);
			if(!csStream.WriteValue("NEFFECT_RED_MIN", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cRedMax);
			if(!csStream.WriteValue("NEFFECT_RED_MAX", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cGreenMin);
			if(!csStream.WriteValue("NEFFECT_GREEN_MIN", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cGreenMax);
			if(!csStream.WriteValue("NEFFECT_GREEN_MAX", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cBlueMin);
			if(!csStream.WriteValue("NEFFECT_BLUE_MIN", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cBlueMax);
			if(!csStream.WriteValue("NEFFECT_BLUE_MAX", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cAlphaMin);
			if(!csStream.WriteValue("NEFFECT_ALPHA_MIN", szValue))
				return FALSE;

			sprintf(szValue, "%d", pstNEInfo->cAlphaMax);
			if(!csStream.WriteValue("NEFFECT_ALPHA_MAX", szValue))
				return FALSE;

			sprintf(szValue, "%d\n========================================;", pstNEInfo->nSkySet);
			if(!csStream.WriteValue("NEFFECT_SKYSET", szValue))
				return FALSE;

		}
	}

	return csStream.Write(szFileName, 0 , bEncryption);
}

BOOL	AgcmNatureEffect::CBNatureEffectChanged ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmNatureEffect	* pThis = ( AgcmNatureEffect * ) pClass;
	INT32				* pNatureEffect = ( INT32 * ) pData;

	ASSERT( pThis );
	ASSERT( pNatureEffect );

	if( pThis )
	{
		if( *pNatureEffect == -1 )
			pThis->StopNatureEffect(  );
		else
			pThis->StopNatureEffect( TRUE );

		if( pThis->StartNatureEffect( *pNatureEffect ) )
		{
			TRACE( _T("네이쳐 이펙트를 실행합니다.") );
		}
		else
		{
			TRACE( _T("네이쳐 이펙트를 끕니다.") );
		}
	}

	return TRUE;
}