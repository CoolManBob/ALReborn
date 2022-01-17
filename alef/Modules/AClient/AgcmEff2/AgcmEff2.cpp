#include "AgcmEff2.h"
#include "AgcdEffGlobal.h"
#include "AgcuBillBoard.h"
#include "AgcdEffPublicStructs.h"

#include "AgcuEffVBMng.h"

#include "AgcuEffPath.h"

#include "ApMemoryTracker.h"

#ifdef _DEBUG
#include "AgcuEff2ApMemoryLog.h"
#endif // _DEBUG

#include "AcuMathFunc.h"
USING_ACUMATH;

#include "rwcore.h"
#include "cslog.h"

const	RwInt32		TAILTARGETNODEID1	= 701;
const	RwInt32		TAILTARGETNODEID2	= 702;

AgcmEff2::AgcmEff2( VOID ) : m_fpEffPack(NULL)
{
	SetModuleName("AgcmEff2");
	EnableIdle( TRUE );
}

AgcmEff2::~AgcmEff2( VOID )
{

}

BOOL AgcmEff2::OnAddModule()
{
	AgcdEffGlobal::bGetInst().bSetPtrAgcmRender( static_cast<AgcmRender*>( GetModule( "AgcmRender" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmSound( static_cast<AgcmSound*>( GetModule( "AgcmSound" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrResoruceLoader( static_cast<AgcmResourceLoader*>( GetModule( "AgcmResourceLoader" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmMap( static_cast<AgcmMap*>( GetModule( "AgcmMap" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmOcTree( static_cast<AgcmOcTree*>( GetModule( "AgcmOcTree" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrApmOcTree(	static_cast<ApmOcTree*>( GetModule( "ApmOcTree" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmDynamicLightmap( static_cast<AgcmDynamicLightmap*>( GetModule( "AgcmDynamicLightmap" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmPostFX( static_cast<AgcmPostFX*>( GetModule( "AgcmPostFX" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmUVAnimation( static_cast<AgcmUVAnimation*>( GetModule( "AgcmUVAnimation" ) ) );
	AgcdEffGlobal::bGetInst().bSetPtrAgcmEff2( static_cast<AgcmEff2*>( GetModule( "AgcmEff2" ) ) );

	//. ResourceLoader에 등록.
	m_lLoaderLoadID = AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->RegisterLoader((PVOID)(this), CBLoadEffBase, NULL);

#ifndef USE_MFC
	AgcmCharacter * pcsAgcmCharacter = ( AgcmCharacter * ) GetModule( "AgcmCharacter" );
	if( pcsAgcmCharacter )
		pcsAgcmCharacter->SetCallbackRemoveCharacter( CBRemovecharacter , this );
#endif

	return TRUE;
}

#include "AgcmUIConsole.h"
namespace NSDBG_EFF2
{
	namespace PRIVATE
	{
		AgcmUIConsole *	pcsAgcmUIConsole = NULL;

		VOID FlagToggle(const E_GLOBAL_FLAG flag)
		{
			if( AgcdEffGlobal::bGetInst().bFlagChk( flag ) )
				AgcdEffGlobal::bGetInst().bFlagOff( flag );
			else
				AgcdEffGlobal::bGetInst().bFlagOn( flag );
		}
	};
};

VOID AgcmEff2::DBG_ShowWire()
{
	NSDBG_EFF2::PRIVATE::FlagToggle(E_GFLAG_SHOWWIRE);
};
VOID AgcmEff2::DBG_ShowSphere()
{
	NSDBG_EFF2::PRIVATE::FlagToggle(E_GFLAG_SHOWSPHERE);
};
VOID AgcmEff2::DBG_ShowEffFrm()
{
	NSDBG_EFF2::PRIVATE::FlagToggle(E_GFLAG_SHOWFRM);
};
VOID AgcmEff2::DBG_ShowEffMemoryLog()
{
	AgcuEff2ApMemoryLog::bGetInst().bToAgcmUIConsole(NSDBG_EFF2::PRIVATE::pcsAgcmUIConsole);
};
VOID AgcmEff2::DBG_ShowEffInfo(RwUInt32 effID)
{
	if( !NSDBG_EFF2::PRIVATE::pcsAgcmUIConsole )
		return;

	LPEFFSET pEffSet = m_AgcdEffSetMng.FindEffSet( effID );
	if( pEffSet )
	{
		TCHAR	buff[256] = {'\0', };
		wsprintf(buff, _T("name : %s, life : %d, refCnt : %d, accumCnt : %d")
			, pEffSet->bGetTitle()
			, pEffSet->bGetLife()
			, pEffSet->bGetRefCnt()
			, pEffSet->bGetAcumCnt()
			);
		NSDBG_EFF2::PRIVATE::pcsAgcmUIConsole->getConsole().print(buff);
	}
	else
	{
		NSDBG_EFF2::PRIVATE::pcsAgcmUIConsole->getConsole().print(_T("NOT FOUND"));
	}
};
#ifdef _DEBUG
VOID AgcmEff2::DBG_ClearDbgAxis()
{
	AXISVIEW::ClearAxis();
}
#endif //_DEBUG
//@} kday

BOOL AgcmEff2::OnInit()
{
	AgcdEffGlobal::bGetInst().bSetPtrAgcmEventEffect( static_cast<AgcmEventEffect*>( GetModule( "AgcmEventEffect" ) ) );

	RwChar	szTexPath[MAX_PATH]		= "";
	strncpy(szTexPath, AgcuEffPath::GetPath_Tex(), MAX_PATH-1);
	AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->SetTexturePath(szTexPath);
	_BuildEffSetContainer();

	g_agcuEffVBMng.InitVB();

	NSDBG_EFF2::PRIVATE::pcsAgcmUIConsole = (AgcmUIConsole *) GetModule("AgcmUIConsole");
	if(NSDBG_EFF2::PRIVATE::pcsAgcmUIConsole)
	{
		AS_REGISTER_TYPE_BEGIN(AgcmEff2, AgcmEff2);
		AS_REGISTER_METHOD0(void, DBG_ShowWire);
		AS_REGISTER_METHOD0(void, DBG_ShowSphere);
		AS_REGISTER_METHOD0(void, DBG_ShowEffFrm);
		AS_REGISTER_METHOD0(void, DBG_ShowEffMemoryLog);
		AS_REGISTER_METHOD1(void, DBG_ShowEffInfo, uint);

#ifdef _DEBUG
		m_chkEffSetID = 0;
		m_skipEffSetID= 0;
		m_axisDbgType = 0;
		m_dbgBaseIndex= 0;
		AS_REGISTER_VARIABLE(uint, m_chkEffSetID);
		AS_REGISTER_VARIABLE(uint, m_skipEffSetID);
		AS_REGISTER_VARIABLE(int, m_axisDbgType);
		AS_REGISTER_VARIABLE(int, m_dbgBaseIndex);
		AS_REGISTER_METHOD0(void, DBG_ClearDbgAxis);
#endif //_DEBUG

		AS_REGISTER_TYPE_END;
	}

	return TRUE;
}

BOOL AgcmEff2::OnDestroy()
{
	DEF_SAFEFCLOSE(m_fpEffPack);

#ifdef _DEBUG
	RwInt32 nTotalCnt = m_AgcdEffSetMng.ShowState(m_AgcdEffCtrlMng.GetCurrCnt());
	AgcuEff2ApMemoryLog::bGetInst().bToFile();
#endif //_DEBUG

	m_AgcdEffCtrlMng.ClearCtrlSet();
	g_agcuEffVBMng.Clear();

	RwFrameSetRemoveCallback(0, 0);

	return TRUE;
}

FLOAT	G_FSPEED	= 1.f;
UINT32	CONTROLSPEED(UINT32 ulDiffTick)
{
	return static_cast<UINT32>( static_cast<FLOAT>(ulDiffTick) * G_FSPEED );
}

BOOL AgcmEff2::OnIdle(UINT32 ulNowTime)
{
	PROFILE("AgcmEff2::OnIdle()");

	BMATRIX::bGetInst().Update( AgcdEffGlobal::bGetInst().bGetPtrRwCamera() );
	m_AgcdEffCtrlMng.TimeUpdate( CONTROLSPEED(AgcdEffGlobal::bGetInst().bGetDiffTimeMS()) );
	m_AgcdEffCtrlMng.UpdateLoadedEffCtrlList();

	return TRUE;
}

BOOL AgcmEff2::RemoveAllLoadingEffsetList()
{
	m_AgcdEffCtrlMng.ClearPreSet();
	return TRUE;
}


BOOL AgcmEff2::RemoveEffSet(LPEFFCTRL_SET pEffCtrlSet, BOOL bCallCB)
{

	m_AgcdEffCtrlMng.RemoveEffSet( pEffCtrlSet , bCallCB ); 
	m_AgcdEffCtrlMng.RemoveCtrl( pEffCtrlSet );

	return TRUE;
}

VOID AgcmEff2::PushToEffSetPool(LPEFFSET pEffSet)
{
	m_AgcdEffSetMng.PushToEffSetPool(pEffSet);
};

VOID AgcmEff2::PopFromEffSetPool(LPEFFSET pEffSet)
{
	m_AgcdEffSetMng.PopFromEffSetPool(pEffSet);
};

BOOL AgcmEff2::_BuildEffSetContainer(void)
{
	//clear 1st EffCtrlContainer
	m_AgcdEffCtrlMng.ClearCtrlSet();

	//load all Eff
	if( AgcdEffGlobal::bGetInst().bFlagChk(E_GFLAG_USEPACK) )
	{

		LPCSTR	EFFFILE_VAR = "efffile_var.eff";
		RwChar	buff[MAX_PATH] = "";
		strcpy( buff, AgcuEffPath::GetPath_Ini() );
		strcat( buff, EFFFILE_VAR );
		m_fpEffPack = fopen( buff, "rb" );
		ASSERT( m_fpEffPack );
		if( !m_fpEffPack )
		{
			EnableIdle( FALSE );
			return FALSE;
		}

		if(T_ISMINUS4(m_AgcdEffSetMng.LoadEffFile()))
		{
			Eff2Ut_ERR( "m_AgcdEffSetMng.bLoadEffFile() failed @ AgcmEff2::OnInit" );
			EnableIdle( FALSE );
			return FALSE;
		}
	}
	else
	{
		if( T_ISMINUS4( m_AgcdEffSetMng.LoadAll() ) )
		{
			Eff2Ut_ERR( "m_AgcdEffSetMng.bLoadAll() failed @ AgcmEff2::OnInit" );
			return FALSE;
		}
	}

	return TRUE;
};

#ifdef _DEBUG
#include "AcuIMDraw.h"
#endif //_DEBUG

LPEFFCTRL_SET AgcmEff2::CreateEffSet(LPEFFSET pEffSet, LPSTEFFUSEINFO Info, AgcdEffCtrl_Set* pNewEffCtrl_Set)
{
	if( !pEffSet || !pNewEffCtrl_Set )								return NULL;
	if( pNewEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )		return NULL;

	RpClump	* pClump = pNewEffCtrl_Set->GetPtrClumpParent();
	RwFrame * pFrame = pNewEffCtrl_Set->GetFrame();
		
	if( Info->m_pBase )
	{
		switch( Info->m_pBase->m_eType )
		{
		case APBASE_TYPE_OBJECT:
			{
				ApdObject* pdObject	 = (ApdObject *)(Info->m_pBase);
				if( pdObject )
				{
					AgcdObject* pcObject = AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->m_pcsAgcmObject->GetObjectData(pdObject);
					if( pcObject && pcObject->m_stGroup.m_pstList && pcObject->m_stGroup.m_pstList->m_csData.m_pstClump != Info->m_pClumpParent)
					{
						DEF_SAFEDELETE(pNewEffCtrl_Set);
						return NULL;
					}
				}
			}
			break;
		case APBASE_TYPE_CHARACTER:
			{
				AgpdCharacter* pdChracter = (AgpdCharacter*)(Info->m_pBase);
				if( pdChracter )
				{
					AgcdCharacter* pcdCharacter	= AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->m_pcsAgcmCharacter->GetCharacterData(pdChracter);
					if( !pcdCharacter || !pcdCharacter->m_pClump || Info->m_pClumpParent != pcdCharacter->m_pClump )	
					{
						DEF_SAFEDELETE(pNewEffCtrl_Set);
						return NULL;
					}
				}
			}
			break;
		}
	}

	Info->m_stTailInfo.m_eType							= stTailInfo::e_heightbase;
	Info->m_stTailInfo.m_stHeightInfo.m_pFrmTailTarget	= Info->m_pFrmParent;
	Info->m_stTailInfo.m_stHeightInfo.m_fHeight1		= 70.f;
	Info->m_stTailInfo.m_stHeightInfo.m_fHeight2		= -20.f;

#ifndef USE_MFC
	if( DEF_FLAG_CHK( pEffSet->bGetFlag(), FLAG_EFFSET_HASTAIL ) )
	{
		if( !Info->m_pHierarchy )
		{
			DEF_SAFEDELETE(pNewEffCtrl_Set);
			pEffSet->bRelease();
			return NULL;
		}
		
		RwInt32 nNodeIndex1	= RpHAnimIDGetIndex( Info->m_pHierarchy, TAILTARGETNODEID1 );
		RwInt32 nNodeIndex2	= RpHAnimIDGetIndex( Info->m_pHierarchy, TAILTARGETNODEID2 );
		
		if( T_ISMINUS4( nNodeIndex1 ) || T_ISMINUS4( nNodeIndex2 ) )
		{
			DEF_SAFEDELETE(pNewEffCtrl_Set);
			pEffSet->bRelease();
			return NULL;
		}

		if( Info->m_pHierarchy->pNodeInfo[ nNodeIndex1 ].pFrame && Info->m_pHierarchy->pNodeInfo[ nNodeIndex2 ].pFrame )
		{
			
			Info->m_stTailInfo.m_eType					= stTailInfo::e_nodebase;
			Info->m_stTailInfo.m_stNodeInfo.m_pFrmNode1	= Info->m_pHierarchy->pNodeInfo[ nNodeIndex1 ].pFrame;
			Info->m_stTailInfo.m_stNodeInfo.m_pFrmNode2	= Info->m_pHierarchy->pNodeInfo[ nNodeIndex2 ].pFrame;
		}
		else
		{
			DEF_SAFEDELETE(pNewEffCtrl_Set);
			pEffSet->bRelease();
			return NULL;
		}
	}
#endif//USE_MFC

	//. 2006. 이미 만들어진 Dummy Ctrl_set에 설정.
	pNewEffCtrl_Set->Init_Set(Info->m_ulFlags, pEffSet, Info->m_stTailInfo, Info->m_pFrmParent, &Info->m_rgbScale, Info->m_fParticleNumScale, Info->m_fTimeScale , Info->m_vBaseDir );
	pNewEffCtrl_Set->SetRotation(&Info->m_quatRotation);

	{
		PROFILE("UseEffSet - insert");
		if( !pNewEffCtrl_Set )
		{
			pEffSet->bRelease();
			return NULL;
		}
		if( pNewEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )
		{
			DEF_SAFEDELETE(pNewEffCtrl_Set);
			return NULL;
		}
		if( DEF_FLAG_CHK( pEffSet->bGetFlag(), FLAG_EFFSET_MISSILE ) )
		{
			Info->m_stMissileTargetInfo.m_pFrmTarget	= Info->m_pFrmTarget;
			pNewEffCtrl_Set->SetMissileInfo( Info->m_stMissileTargetInfo.m_pFrmTarget
				, &Info->m_stMissileTargetInfo.m_v3dCenter );
			
			if( pNewEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )
			{
				DEF_SAFEDELETE(pNewEffCtrl_Set);
				return NULL;
			}
		}
		if( DEF_FLAG_CHK( pEffSet->bGetFlag(), FLAG_EFFSET_MFRM ) )
		{
			if( DEF_FLAG_CHK( pEffSet->bGetFlag(), FLAG_EFFSET_MFRM_SRC ) )
				pNewEffCtrl_Set->SetMFrmTarget( Info->m_pFrmParent );
			else if( DEF_FLAG_CHK( pEffSet->bGetFlag(), FLAG_EFFSET_MFRM_DST ) )
				pNewEffCtrl_Set->SetMFrmTarget( Info->m_pFrmTarget );
			else if( DEF_FLAG_CHK( pEffSet->bGetFlag(), FLAG_EFFSET_MFRM_CAM ) )
				pNewEffCtrl_Set->SetMFrmTarget( AgcdEffGlobal::bGetInst().bGetPtrCamFrm() );
			
			if( pNewEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )
			{
				DEF_SAFEDELETE(pNewEffCtrl_Set);
			}
		}
		if( DEF_FLAG_CHK( Info->m_ulFlags, stEffUseInfo::E_FLAG_MAINCHARAC ) )
		{
			pNewEffCtrl_Set->FlagOn( FLAG_EFFCTRLSET_MAINCHARAC );
			pNewEffCtrl_Set->SetSoundType3DToSample( TRUE );
		}

		// 이펙트에 옥트리 체크를 사용하지 않도록 한다.
		// 가능하면 트리를 사용하지 않게 플레그를 주석처리한다
		if( Info->m_pBase && Info->m_pBase->m_eType == APBASE_TYPE_CHARACTER )
		{
			// do nothing..
		}
		else
		{
			// 케릭터가 아니면 옥트리 사용..
			if( DEF_FLAG_CHK( Info->m_ulFlags, stEffUseInfo::E_FLAG_STATICEFFECT ) )
			{
				pNewEffCtrl_Set->FlagOn( FLAG_EFFCTRLSET_STATIC );
			}
		}

		if( pNewEffCtrl_Set->FlagChk( FLAG_EFFCTRLSET_STATIC ) )
		{
			if( !AgcdEffGlobal::bGetInst().bGetPtrAgcmOcTree() ||
				!AgcdEffGlobal::bGetInst().bGetPtrAgcmOcTree()->m_pApmOcTree->m_bOcTreeEnable )
				pNewEffCtrl_Set->FlagOff( FLAG_EFFCTRLSET_STATIC );
		}
		if( DEF_FLAG_CHK( Info->m_ulFlags, stEffUseInfo::E_FLAG_STATICEFFECT ) ||
			DEF_FLAG_CHK( Info->m_ulFlags, stEffUseInfo::E_FLAG_LINKTOPARENT ) 	)
		{
			if( Info->m_pClumpParent )
			{
				pNewEffCtrl_Set->FlagOn( FLAG_EFFCTRLSET_UPDATEWITHCLUMP );
				pNewEffCtrl_Set->SetPtrClumpParent(Info->m_pClumpParent);
			}
		}

		pNewEffCtrl_Set->SetLife( Info->m_ulLife );

		if( Info->m_ulDelay && !DEF_FLAG_CHK( Info->m_ulFlags, stEffUseInfo::E_FLAG_LINKTOPARENT ) )
		{
			pNewEffCtrl_Set->FlagOn(FLAG_EFFCTRLSET_WILLBEASYNCPOS);
			pNewEffCtrl_Set->SetAsyncData( Info->m_pFrmParent, Info->m_pFrmTarget, Info->m_v3dCenter, Info->m_ulFlags );
		}
		else
			pNewEffCtrl_Set->SetDirAndOffset( Info->m_pFrmParent, Info->m_pFrmTarget, Info->m_v3dCenter, Info->m_ulFlags );
		
		if( pNewEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )
		{
			DEF_SAFEDELETE(pNewEffCtrl_Set);
			return NULL;
		}

		pNewEffCtrl_Set->SetInfo( Info->m_ulLife
			, Info->m_ulDelay
			, Info->m_fScale
			, Info->m_pBase
			, Info->m_lOwnerCID
			, Info->m_lTargetCID
			, Info->m_lCustData
			, Info->m_fptrNoticeCB
			, Info->m_pNoticeCBClass
			, Info->m_lCustID);	

		if( pNewEffCtrl_Set->GetState() == E_EFFCTRL_STATE_END )
		{
			DEF_SAFEDELETE(pNewEffCtrl_Set);
			return NULL;
		}

		if( DEF_FLAG_CHK(Info->m_ulFlags, stEffUseInfo::E_FLAG_EMITER_WITH_CLUMP) )
		{
			pNewEffCtrl_Set->SetPtrClumpEmiter(Info->m_pClumpEmiter);
		}
		else if( DEF_FLAG_CHK(Info->m_ulFlags, stEffUseInfo::E_FLAG_EMITER_WITH_ATOMIC) )
		{
			pNewEffCtrl_Set->SetPtrAtomicEmiter(Info->m_pAtomicEmiter);
		}

//프레임 축을 보며 디버깅 할때
#ifdef _DEBUG
		if( Info->m_ulEffID == m_chkEffSetID )
		{
			switch( m_axisDbgType )
			{
			case 0://clear and all
				{
					AXISVIEW::ClearAxis();
					if( Info->m_pFrmParent )
						AXISVIEW::AddAxis(Info->m_pFrmParent, 0xffffff00);
					if( Info->m_pFrmTarget )
						AXISVIEW::AddAxis(Info->m_pFrmTarget, 0xff00ffff);
					AXISVIEW::AddAxis( *pNewEffCtrl_Set->GetLTM() );
				}break;
			case 1://only target
				{
					if( Info->m_pFrmTarget )
						AXISVIEW::AddAxis(Info->m_pFrmTarget, 0xff00ffff);
				}break;
			case 2://only parent
				{
					if( Info->m_pFrmParent )
						AXISVIEW::AddAxis(Info->m_pFrmParent, 0xffffff00);
				}break;
			case 3://only effset
				{
					AXISVIEW::AddAxis( *pNewEffCtrl_Set->GetLTM() );
				}break;
			case 4://all but not clear
				{
					if( Info->m_pFrmParent )
						AXISVIEW::AddAxis(Info->m_pFrmParent, 0xffffff00);
					if( Info->m_pFrmTarget )
						AXISVIEW::AddAxis(Info->m_pFrmTarget, 0xff00ffff);
					AXISVIEW::AddAxis( *pNewEffCtrl_Set->GetLTM() );
				}break;
			}
		}
#endif //_DEBUG


#ifdef USE_MFC
		//. ref. Efftool
		//. 현재 플레이하고 있는 effset의 flag를 업데이트하기 위해 실행시 포인터를 들고 있는다.
		AgcdEffGlobal::bGetInst().m_pBitFlags = &(pEffSet->m_dwEffSetBitFlags);
#endif
		//. ref. AgcdEffCtrlMng::m_listEffSetCtrl.
		m_AgcdEffCtrlMng.InsertCtrlSet( pNewEffCtrl_Set );
	}

	return pNewEffCtrl_Set;
}

LPEFFCTRL_SET AgcmEff2::UseEffSet(LPSTEFFUSEINFO pstEffUseInfo, BOOL bForceImmediate)
{
	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_EFFOFF ) )
		return NULL;//이펙트 옵션이 꺼져있다.

	if(AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->m_bForceImmediate)
		bForceImmediate = TRUE;

	//일단, 정적인 오브젝트에 붙는 이펙트는 스케일이 먹도록 해두자.	땜빵
	if( pstEffUseInfo->m_ulFlags & stEffUseInfo::E_FLAG_STATICEFFECT )
	{
		DEF_FLAG_OFF(pstEffUseInfo->m_ulFlags, stEffUseInfo::E_FLAG_NOSCALE);
	}

	LPEFFSET pEffSet	= m_AgcdEffSetMng.FindEffSet( pstEffUseInfo->m_ulEffID );
	if( !pEffSet )
		return NULL;
	
	//. 다른 module에서 사용될수 있으므로 AgcdEffset::bAddRef의 인자를 유지시키기 위해 
	//. AgcdEffset::m_bForceImmediate으로 넣음.
	pEffSet->m_bForceImmediate = bForceImmediate;

	if( T_ISMINUS4( pEffSet->bAddRef() ) )
		return NULL;

	AgcdEffCtrl_Set* pNewEffCtrl_Set = new AgcdEffCtrl_Set(0);

#ifndef USE_MFC
	//. Ctrl Set은 Effset의 로딩이 완료된 후에 만들어져서 Update List에 들어간다.
	//. Loading중인 Effset만 AgcmEffCtrlSet::m_listLoadingEffSet에 들어간다.
	if(pEffSet->m_enumLoadStatus == AGCDEFFSETRESOURCELOADSTATUS_LOADING )
	{
		//. 유효한 Frame~
		if(pstEffUseInfo->m_pFrmParent)
			AgcdEffGlobal::bGetInst().bGetPtrAgcmRender()->AddToRwFrameMapTable(pstEffUseInfo->m_pFrmParent);

		//. 로딩 중인 EffSet* 저장.
		m_AgcdEffCtrlMng.InsertPreEff(pEffSet, pstEffUseInfo, pNewEffCtrl_Set);

		return pNewEffCtrl_Set;
	}
#else
	//. eff tool에서 사용시 새로 추가되는 effctrl을 플레이하기 위해.
	pEffSet->m_enumLoadStatus = AGCDEFFSETRESOURCELOADSTATUS_LOADED;
#endif

	//. 직접 Main Thread Process를 타는 경우와 Back Thread Process를 타는 경우에 쓰이므로 중복코드제거.	
	return CreateEffSet(pEffSet, pstEffUseInfo, pNewEffCtrl_Set);
}

RwInt32 AgcmEff2::GetSoundLength(RwUInt32 dwEffSetID)
{
	LPEFFSET pEffSet = m_AgcdEffSetMng.FindEffSet( dwEffSetID );
	if( !pEffSet )
		return 0;

	if( T_ISMINUS4( pEffSet->bAddRef() ) )
		return 0;

	RwInt32 nLength	= 0;
	pEffSet->bGetSoundLendth( &nLength );

	pEffSet->bRelease();

	return nLength;
}

BOOL AgcmEff2::SetGlobalVariable( RpClump *pClump , RwFrame* pFrmMainCharac, RpWorld* pWorld, RwCamera* pCamera )
{
	AgcdEffGlobal::bGetInst().bSetPtrClumpMainCharac( pClump );
	AgcdEffGlobal::bGetInst().bSetPtrFrmMainCharac( pFrmMainCharac );

	ASSERT( pWorld && pCamera );
	AgcdEffGlobal::bGetInst().bSetPtrRpWorld( pWorld );
	AgcdEffGlobal::bGetInst().bSetPtrRwCamera( pCamera );
	
	return TRUE;
}

BOOL AgcmEff2::SetMainCharacterFrame(RpClump * pClump , RwFrame* pFrmMainCharac)
{
	AgcdEffGlobal::bGetInst().bSetPtrFrmMainCharac( pFrmMainCharac );
	AgcdEffGlobal::bGetInst().bSetPtrClumpMainCharac( pClump );
	
	return TRUE;
}

VOID AgcmEff2::ToneDown(RwBool bSwitch)
{
	if( bSwitch )
	{
		AgcdEffGlobal::bGetInst().bFlagOn(E_GFLAG_TONEDOWN);
		m_AgcdEffCtrlMng.ToneDown();
	}
	else
	{
		AgcdEffGlobal::bGetInst().bFlagOff(E_GFLAG_TONEDOWN);		
		m_AgcdEffCtrlMng.ToneRestore();
	}
}
VOID AgcmEff2::EffOn()
{
	AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_EFFOFF );
}

VOID AgcmEff2::EffOff()
{
	AgcdEffGlobal::bGetInst().bFlagOn( E_GFLAG_EFFOFF );
}

BOOL AgcmEff2::SetCallbackRemoveEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMEFF2_CB_ID_REMOVEEFFECT, pfCallback, pClass );
}


BOOL AgcmEff2::CBLoadEffBase(PVOID pData, PVOID pClass, PVOID pCustData)
{	
	AgcmEff2 * pEff2 = ( AgcmEff2 * ) pClass;
	ApAutoWriterLock	csLock( pEff2->m_RWLock );

	static CSLogIndicator	_sIndicator( "AgcmEff2::CBLoadEffBase" );
	CSLogAutoIndicator	stAutoIndicator( &_sIndicator );

	if(!pData)
		return FALSE;

	AGCDEFFSET_CBARG* pArg = (AGCDEFFSET_CBARG*)pData;
	
	//. Clump와 Texture만 지원. Sound파일은 loading과 play방식에 문제가 많음. ㅜㅜ
	//. AgcdEffBase::E_EFFBASE_SOUND
	if(	!pArg->m_pLoadBase->m_bForceImmediate &&
		pArg->m_eBaseType == AgcdEffBase::E_EFFBASE_OBJECT)
	{
		CCSLog	stLog( 10000 );
		//. Back Thread Loading.
		AgcdEffObj* pObjBase = static_cast<AgcdEffObj*>(pArg->m_pLoadBase);

		if(strlen(pObjBase->bGetClumpName()))
			pObjBase->vLoadClump(pObjBase->bGetClumpName());
	}

	if(pArg->m_pLoadBase->bIsRenderBase())
	{
		CCSLog	stLog( 10001 );
		
		AgcdEffRenderBase*	pRenderBase = static_cast<AgcdEffRenderBase*>(pArg->m_pLoadBase);
		if( pRenderBase->bGetTexIndex() != -1 && (pArg->m_pLoadBase->bGetBaseType() != AgcdEffBase::E_EFFBASE_OBJECT) )
		{
			CCSLog	stLog( 10011 );

			INT32 lIndex	 = pRenderBase->bGetTexIndex();
			if( lIndex >= 0 )
			{
				AgcdEffTex* pTex = pArg->m_pOwnerSet->bGetPtrEffTex( lIndex );
				if(pTex)
					pRenderBase->bSetPtrEffTex(pTex);
			}
		}
	}
	
	// Effset의 Loading Status check.
	++pArg->m_pOwnerSet->m_nCurrentNumOfLoadingBase;

	// Effset의 로딩이 완료된 시점이므로 상태를 바꿔주고, OnIdle에서 업데이트 리스트에 들어간다.
	if(pArg->m_pOwnerSet->m_nCurrentNumOfLoadingBase ==
		(pArg->m_pOwnerSet->bGetVarSizeInfo().m_nNumOfBase))
	{
		CCSLog	stLog( 10002 );
		AuAutoLock	lock(pArg->m_pOwnerSet->m_Mutex);
		if (lock.Result())
			pArg->m_pOwnerSet->m_enumLoadStatus = AGCDEFFSETRESOURCELOADSTATUS_LOADED;
	}

	if(pArg)
		delete pArg;

	return TRUE;
}

BOOL	AgcmEff2::CBRemovecharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEff2*			pThis			=	static_cast< AgcmEff2* >(pClass);
	AgpdCharacter*		ppCharacter		=	static_cast< AgpdCharacter* >(pData);
	if( !ppCharacter)
		return TRUE;

	AgcmCharacter*	pcsAgcmCharacter	=	static_cast< AgcmCharacter* >( pThis->GetModule("AgcmCharacter") );
	AgcdCharacter*	pcdCharacter		=	pcsAgcmCharacter->GetCharacterData( ppCharacter );

	pThis->	m_AgcdEffCtrlMng.RemoveAllEffectByClump( pcdCharacter->m_pClump );

	return TRUE;
}

LPEFFSET AgcmEff2::GetEffectSet( int nEffectID )
{
	return m_AgcdEffSetMng.FindEffSet( nEffectID );
}
