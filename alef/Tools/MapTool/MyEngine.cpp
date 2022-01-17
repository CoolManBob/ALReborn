// MyEngine.cpp: implementation of the MyEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "maptool.h"
#include "MyEngine.h"
#include "rpcollis.h"
#include "rpmatfx.h"
#include "acurpmtexture.h"
#include "rpuvanim.h"
#include "AcuRpUVAnimData.h"
#include "AcuParticleDraw.h"

//@{ Jaewon 20041230
#include <rprandom.h>
#include "AcuRtAmbOcclMap.h"
#include <rppvs.h>
//@} Jaewon

//#include "AcuRpDWSector.h"
//#include "AcuRpGlyph.h"
/******* Parn 님 작업 시작 *********/
#include "rpusrdat.h"
/******* Parn 님 작업 끝 *********/

// Image Loader
#include "rtbmp.h"
#include "rtpng.h"
#include "MainWindow.h"
#include "MainFrm.h"
#include "rplodatm.h"

/********* Parn님 작업 시작 ********/
#include "ApModule.h"
#include "ApModuleManager.h"
/********* Parn님 작업 끝 **********/

#include "AgpmGrid.h"

#include "ProgressDlg.h"

#include "AgcmPreLODManager.h"
#include "AgcmResourceLoader.h"
#include "AgcmShadowmap.h"

#include "AgpmEventRefinery.h"
#include "AgcmEventRefinery.h"

#include "AgcmEventProductDlg.h"

#include "AuPackingManager.h"

//@{ Jaewon 20050113
#include "Plugin_AmbOcclMap.h"
//@} Jaewon

#include "AgcdEffGlobal.h"
#include "Plugin_BossSpawn.h"

#include "AgpmBillInfo.h"

#include "AgpmRide.h"
#include "AgpmSummons.h"
#include "AgcmRide.h"

#include "AgpmWantedCriminal.h" 
#include "AgpmSiegeWar.h"
#include "AgpmGuild.h"

#include "AgpmEventGacha.h"
#include "AgpmEventBinding.h"
#include "AgpmBattleGround.h"


// 마고자 (2004-10-15 오전 10:06:45) : 남맨남맨의 새 디버그 코드로 

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventTeleportDlgD" )
#else
#pragma comment ( lib , "AgcmEventTeleportDlg" )
#endif
#endif

extern AuCircularBuffer	g_AuCircularBuffer;

MainWindow				g_MainWindow				;
MyEngine				g_MyEngine					;

ApmMap				*	g_pcsApmMap					= NULL;	// 맵 퍼블릭 모듈..

AgpmCharacter		*	g_pcsAgpmCharacter			= NULL;
AgcmCharacter		*	g_pcsAgcmCharacter			= NULL;

AgpmItem			*	g_pcsAgpmItem				= NULL;
AgpmDropItem		*	g_pcsAgpmDropItem			= NULL;
AgcmItem			*	g_pcsAgcmItem				= NULL;

AgpmUnion			*	g_pcsAgpmUnion				= NULL;

AgpmParty			*	g_pcsAgpmParty				= NULL;

ApmObject			*	g_pcsApmObject				= NULL;
AgcmObject			*	g_pcsAgcmObject				= NULL;
AgpmFactors			*	g_pcsAgpmFactors			= NULL;

AgcmRender			*	g_pcsAgcmRender				= NULL;	// Render Module

ApmEventManager		*	g_pcsApmEventManager		= NULL;
AgpmEventNature		*	g_pcsAgpmEventNature		= NULL;
AgcmEventNature		*	g_pcsAgcmEventNature		= NULL;
AgpmEventSpawn		*	g_pcsAgpmEventSpawn			= NULL;
AgpmEventNPCDialog	*	g_pcsAgpmEventNPCDialog		= NULL;
AgpmEventNPCTrade	*	g_pcsAgpmEventNPCTrade		= NULL;
AgpmAuction			*	g_pcsAgpmEventAuction		= NULL;

//AgpmAI				*	g_pcsAgpmAI					= NULL;
AgpmAI2				*	g_pcsAgpmAI2				= NULL;

//AgcmCamera			*	g_pcsAgcmCamera				= NULL;
AgcmCamera2			*	g_pcsAgcmCamera2			= NULL;

AgcmWater			*	g_pcsAgcmWater				= NULL;
AgcmFont			*	g_pcsAgcmFont				= NULL;

AgpmShrine			*	g_pcsAgpmShrine				= NULL;

ApmOcTree			*	g_pcsApmOcTree				= NULL;
AgcmOcTree			*	g_pcsAgcmOcTree				= NULL;

AgcmTuner			*	g_pcsAgcmTuner				= NULL;

AgcmGrass			*	g_pcsAgcmGrass				= NULL;

AgpmGrid			*	g_pcsAgpmGrid				= NULL;
AgcmLODManager		*	g_pcsAgcmLODManager			= NULL;
AgcmSound			*	g_pcsAgcmSound				= NULL;
//AgcmEffect			*	g_pcsAgcmEffect				= NULL;
AgpmSkill			*	g_pcsAgpmSkill				= NULL;
AgcmSkill			*	g_pcsAgcmSkill				= NULL;
AgpmEventTeleport	*	g_pcsAgpmEventTeleport		= NULL;
//AgcmImportClientData*	g_pcsAgcmImportClientData	= NULL;
AgcmEventNPCTrade	*	g_pcsAgcmEventNPCTrade		= NULL;
AgcmEventEffect		*	g_pcsAgcmEventEffect		= NULL;
AgpmTimer			*	g_pcsAgpmTimer				= NULL;
AgcmUIControl		*	g_pcsAgcmUIControl			= NULL;
AgcmPreLODManager	*	g_pcsAgcmPreLODManager		= NULL;
AgcmResourceLoader	*	g_pcsAgcmResourceLoader		= NULL;

AgpmEventBank		*	g_pcsAgpmEventBank			= NULL;
AgcmEventBank		*	g_pcsAgcmEventBank			= NULL;
AgcmEventManager	*	g_pcsAgcmEventManager		= NULL;

AcuObject				g_csAcuObject				;
AcuIMDraw				g_csAcuIMDraw				;
AcuParticleDraw			g_csAcuParticleDraw			;

AgpmPathFind		*		g_pcsAgpmPathFind				= NULL;
//AgpmEventMasterySpecialize*	g_pcsAgpmEventMasterySpecialize	= NULL;

AgpmItemConvert		*		g_pcsAgpmItemConvert		= NULL;

AgpmEventItemConvert	*	g_pcsAgpmEventItemConvert	= NULL;
AgcmEventItemConvert	*	g_pcsAgcmEventItemConvert	= NULL;

AgpmEventGuild			*	g_pcsAgpmEventGuild			= NULL;
AgcmEventGuild			*	g_pcsAgcmEventGuild			= NULL;

AgpmEventProduct		*	g_pcsAgpmEventProduct		= NULL;
AgcmEventProduct		*	g_pcsAgcmEventProduct		= NULL;

AgcmPostFX				*	g_pcsAgcmPostFX				= NULL;
AgcmUVAnimation			*	g_pcsAgcmUVAnimation		= NULL;

// AgcmMinimap				g_csAgcmMinimap				= NULL;

AgcmShadow				*	g_pcsAgcmShadow				= NULL;
AgcmShadow2				*	g_pcsAgcmShadow2			= NULL;

AgcmShadowmap			*	g_pcsAgcmShadowmap			= NULL;

//@{ Jaewon 20040621
AgcmDynamicLightmap*	g_pcsAgcmDynamicLightmap= NULL;
//@} Jaewon

AgpmEventBinding		*	g_pcsAgpmEventBinding		= NULL;
AgpmBattleGround		*	g_pcsAgpmBattleGround		= NULL;

AgpmQuest				*	g_pcsAgpmQuest				= NULL;
AgpmEventQuest			*	g_pcsAgpmEventQuest			= NULL;

AgpmEventSkillMaster	*	g_pcsAgpmEventSkillMaster		= NULL;
AgcmEventSkillMaster	*	g_pcsAgcmEventSkillMaster		= NULL;
AgcmEff2				*	g_pcsAgcmEff2					= NULL	;

ApmEventManagerDlg	*	g_pcsApmEventManagerDlg		= NULL;

AgcmEventSpawnDlg	*	g_pcsAgcmEventSpawnDlg		= NULL;
AgcmEventNatureDlg	*	g_pcsAgcmEventNatureDlg		= NULL;
AgcmEventNPCDialogDlg*	g_pcsAgcmEventNPCDialogDlg	= NULL;
AgcmEventNPCTradeDlg*	g_pcsAgcmEventNPCTradeDlg	= NULL;
AgcmEventAuctionDlg *	g_pcsAgcmEventAuctionDlg	= NULL;
//AgcmAIDlg			*	g_pcsAgcmAIDlg				= NULL;
AgcmShrineDlg		*	g_pcsAgcmShrineDlg			= NULL;
AgcmEventTeleportDlg*	g_pcsAgcmEventTeleportDlg	= NULL;
AgcmBlockingDlg		*	g_pcsAgcmBlockingDlg		= NULL;
AgcmLODDlg			*	g_pcsAgcmLODDlg				= NULL;
AgcmEffectDlg		*	g_pcsAgcmEffectDlg			= NULL;
AgcmFactorsDlg		*	g_pcsAgcmFactorsDlg			= NULL;
//AgcmAnimationDlg	*	g_pcsAgcmAnimationDlg		= NULL;
//AgcmSkillDlg		*	g_pcsAgcmSkillDlg			= NULL;
AgcmWaterDlg		*	g_pcsAgcmWaterDlg			= NULL;
AgcmWaterDlg2		*	g_pcsAgcmWaterDlg2			= NULL;
AgcmObjectTypeDlg	*	g_pcsAgcmObjectTypeDlg		= NULL;
AgcmEventQuestDlg		*	g_pcsAgcmEventQuestDlg		= NULL;
AgcmEventBindingDlg		*	g_pcsAgcmEventBindingDlg	= NULL;
AgcmEventSkillMasterDlg	*	g_pcsAgcmEventSkillMasterDlg	= NULL;

AgpmEventRefinery		*	g_pcsAgpmEventRefinery	= NULL;
AgcmEventRefinery		*	g_pcsAgcmEventRefinery	= NULL;

AgpmProduct				*	g_pcsAgpmProduct			= NULL;
AgcmEventProductDlg		*	g_pcsAgcmEventProductDlg	= NULL;

//@{ Jaewon 20041229
AgcmAmbOcclMap		*	g_pcsAgcmAmbOcclMap			= NULL;
//@} Jaewon
AgcmNatureEffect	*	g_pcsAgcmNatureEffect		= NULL;

AgpmEventPointLight		*	g_pcsAgpmEventPointLight	= NULL;
AgcmEventPointLight		*	g_pcsAgcmEventPointLight	= NULL;

AgpmBillInfo			*g_pcsAgpmBillInfo		= NULL;

AgpmRide				*g_pcsAgpmRide			= NULL;
//AgpmSummons				*g_pcsAgpmSummons		= NULL;
AgcmRide				*g_pcsAgcmRide			= NULL;

AgpmWantedCriminal		*g_pcsAgpmWantedCriminal= NULL;
AgpmSiegeWar			*g_pcsAgpmSiegeWar		= NULL;
AgpmGuild				*g_pcsAgpmGuild			= NULL;

AgpmTax					*g_pcsAgpmTax			= NULL;
AgcmUITax				*g_pcsAgcmUITax			= NULL;

AgpmGuildWarehouse		*g_pcsAgpmGuildWarehouse	= NULL;
AgcmUIGuildWarehouse	*g_pcsAgcmUIGuildWarehouse	= NULL;
AgpmArchlord			*g_pcsAgpmArchlord			= NULL;

AgpmEventGacha			*g_pcsAgpmEventGacha		= NULL;
AgcmEventGachaDlg		*g_pcsAgcmEventGachaDlg		= NULL;

	
// SetCallbackPostRender2
BOOL	__AgcmRender_PostRenderCallback2 (PVOID pData, PVOID pClass, PVOID pCustData)
{
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)false );

	if( g_pEngine && g_pEngine->m_pCurrentFullUIModule )
		g_MainWindow.OnWindowRender();

	//@{ Jaewon 20050104
	if(g_MyEngine.m_computingAmbOcclMaps)
	// render the illumination progress text.
	{
		TCHAR buf[32];
		_stprintf(buf, _T("%03.1f%%(%03d/%03d)"), g_MyEngine.m_progress, g_MyEngine.m_ambOcclMap.getSessionStartObj()+1, g_MyEngine.m_ambOcclMap.getSessionTotalObj());
		g_pcsAgcmFont->FontDrawStart(0);
		g_pcsAgcmFont->DrawTextMapTool(0, 30, buf, 0, (UINT8)255, RGB(255,0,0), false);
		g_pcsAgcmFont->FontDrawEnd();

		//@{ Jaewon 20050113
		CUITileList_Others::Plugin *pPlugin = g_pMainFrame->m_pTileList->m_pOthers->GetCurrentPlugin();
		if(pPlugin && strcmp(pPlugin->name, "Ambient Occlusion Map Plugin")==0
		   && g_MyEngine.m_ambOcclMap.getSessionTotalObj()>0)
		{
			// update progress bars.
			((CPlugin_AmbOcclMap*)(pPlugin->pWnd))->setObjectProgress(100*g_MyEngine.m_ambOcclMap.getSessionStartObj()/g_MyEngine.m_ambOcclMap.getSessionTotalObj());
			((CPlugin_AmbOcclMap*)(pPlugin->pWnd))->setSliceProgress(int(g_MyEngine.m_progress));
		}
		//@} Jaewon
	}
	//@} Jaewon

	return TRUE;
}

// SetCallbackPostRender
BOOL	__AgcmRender_PostRenderCallback (PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( g_pEngine && g_pEngine->m_pCurrentFullUIModule )
		g_MainWindow.RenderStateClear();
	return TRUE;
}
/*
static DestroyEffectAll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//g_pcsAgcmEffect->EffectSetAllDataRemove();
}
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyEngine::MyEngine()
{
	m_pParallelCamera				= NULL;
	m_pSubCameraMainCameraSubRaster	= NULL;
	m_pSubCameraMainCameraSubZRaster= NULL;

	m_bIsRenderSubCamera			= FALSE		;
	m_nSubCameraType				= SC_OFF	;
	m_bRenderObject					= FALSE		;

	//@{ Jaewon 20050103
	m_computingAmbOcclMaps			= false;
	//@} Jaewon

	//@{ Jaewon 20050113
	m_inIlluminationCallBack		= false;
	//@} Jaewon

	RpMTextureEnableLinearFog( TRUE );
}

MyEngine::~MyEngine()
{

}

BOOL		MyEngine::OnAttachPlugins	()	// 플러그인 연결 은 여기서
{
    /* 
     * Attach world plug-in...
     */
    if( !RpWorldPluginAttach() )
    {
        return FALSE;
    }

    if (!RpCollisionPluginAttach() )
    {
        return FALSE;
    }

	if( !RpMatFXPluginAttach() )
	{
		return FALSE;
	}

	if( !RpMTexturePluginAttach() )
	{
		return FALSE;
	}

	if( !RpSplinePluginAttach() )
	{
		return FALSE;
	}

	/******* Parn 님 작업 시작 *********/
	if( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}
	/******* Parn 님 작업 끝 *********/
	
	if (!RpHAnimPluginAttach())
	{
		return FALSE;
	}

	if (!RpSkinPluginAttach())
	{
		return FALSE;
	}
	
	if (!RtAnimInitialize())
	{
		return FALSE;
	}

	if (!RpLODAtomicPluginAttach() )
	{
		return FALSE;
	}

#ifdef RWLOGO
    /* 
     * Attach logo plug-in...
     */
    if( !RpLogoPluginAttach() )
    {
        return FALSE;
    }
#endif

	if( !RpUVAnimPluginAttach() )
	{
		return FALSE;
	}

	if( !RpUVAnimDataPluginAttach() )
	{
		return FALSE;
	}

	if( !RpMaterialD3DFxPluginAttach() )
	{
		return FALSE;
	}

	//@{ Jaewon 20041229
	if(!RpRandomPluginAttach())
	{
		return FALSE;
	}
	if(!RpAmbOcclMapPluginAttach())
	{
		return FALSE;
	}
	if(!RpPVSPluginAttach())
	{
		return FALSE;
	}
	//@} Jaewon

	return TRUE;

}

RpWorld *	MyEngine::OnCreateWorld		() // 월드를 생성해서 리턴해준다.
{
	//@{ Jaewon 20050103
	// .\\Texture\\AmbOccl\\ added.
	//@{ Jaewon 20050115
	// .\\Texture\\NotPacked\\ added.
	RwImageSetPath(".\\Texture\\;.\\Texture\\Etc\\;.\\Texture\\Character\\;.\\Texture\\Object\\;.\\Texture\\Effect\\;.\\Texture\\UI\\;.\\Map\\Temp\\Rough\\;.\\Texture\\AmbOccl\\;.\\Texture\\NotPacked\\;");
	//@} Jaewon
	//@} Jaewon

	//@{ Jaewon 20040907
	RpMaterialD3DFxSetSearchPath(".\\Effect\\");
	//@} Jaewon

	// 맵데이타 로딩.
//	if( g_MainWindow.m_Map.Init( ) )
		return g_MainWindow.LoadWorld();
//	else
//		return NULL; //에러.
	//return CreateWorld();
}

RwCamera *	MyEngine::OnCreateCamera	( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	ASSERT( pWorld );
	m_fNearClipPlane	= ALEF_SECTOR_WIDTH / 100.0f;
	m_fFarClipPlane		= ALEF_SECTOR_WIDTH * 10.0f;
	
	RwCamera *pCamera = CreateCamera( pWorld );

	RwCameraSetFogDistance( pCamera , ALEF_SECTOR_WIDTH * 3.0f / 10.0f );

	// Parallel Camera 생성..
	m_pParallelCamera = m_clCamera->CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);
	if( m_pParallelCamera )
	{
		RwCameraSetNearClipPlane	( m_pParallelCamera, RwCameraGetNearClipPlane	( pCamera ) );
		RwCameraSetFarClipPlane		( m_pParallelCamera, RwCameraGetFarClipPlane	( pCamera ) );

        RwCameraSetProjection		( m_pParallelCamera, rwPARALLEL );

		
		RpWorldAddCamera			( m_pWorld, m_pParallelCamera );

		VERIFY( m_pSubCameraMainCameraSubRaster		= RwRasterCreate(0, 0, 0, rwRASTERTYPECAMERA	) );
		VERIFY( m_pSubCameraMainCameraSubZRaster	= RwRasterCreate(0, 0, 0, rwRASTERTYPEZBUFFER	) );

		RwCameraSetRaster(m_pParallelCamera, m_pSubCameraMainCameraSubRaster);
		RwCameraSetZRaster(m_pParallelCamera, m_pSubCameraMainCameraSubZRaster);

	}	

	return pCamera;
}

BOOL		MyEngine::OnCreateLight		()	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
{
	ASSERT( m_pWorld );
	//return AgcEngine::OnCreateLight();
	m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
	
	float	ambiantvalue		= 0.4f;
	float	directionalvalue	= 0.7f;

    if (m_pLightAmbient)
    {
		RwRGBAReal color = {ambiantvalue, ambiantvalue, ambiantvalue, 0.0f};
//		RwRGBAReal color = {0.1f, 0.1f, 0.1f, 0.1f};
		RpLightSetColor(m_pLightAmbient, &color);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
    }

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);

    if (m_pLightDirect)
    {
        RwFrame *f = RwFrameCreate();
        RpLightSetFrame( m_pLightDirect , f);

		RwV3d v = { 0.0f , 0.0f , 0.0f };
		RwFrameTranslate	( f , & v ,	rwCOMBINEREPLACE );

		RwMatrix	* pMatrix	= RwFrameGetLTM	( f			);
		RwV3d		* pAt		= RwMatrixGetAt		( pMatrix	);
		pAt->x	= 0.556f;
		pAt->y	= -0.658f;
		pAt->z	= 0.056f;

//		RwFrameAddChild( RwCameraGetFrame( m_pCamera ) , f );

		RwRGBAReal color = {directionalvalue, directionalvalue, directionalvalue, 0.0f};
		RpLightSetColor(m_pLightDirect, &color);

        RpWorld * pWorld = RpWorldAddLight(m_pWorld, m_pLightDirect);
    }

//	float	directionalvalue2	= 0.5f;
//	m_pLightDirect2 = RpLightCreate(rpLIGHTDIRECTIONAL);
//    if (m_pLightDirect2)
//    {
//        RwFrame *f = RwFrameCreate();
//        RpLightSetFrame( m_pLightDirect2 , f);
//
//		RwV3d v = { 0.0f , 0.0f , 0.0f };
//		RwFrameTranslate	( f , & v ,	rwCOMBINEREPLACE );
//
//		RwMatrix	* pMatrix	= RwFrameGetMatrix	( f			);
//		RwV3d		* pAt		= RwMatrixGetAt		( pMatrix	);
//		
//		pAt->x	= -0.627f;
//		pAt->y	= -0.588f;
//		pAt->z	= -0.511f;
//
////		RwFrameAddChild( RwCameraGetFrame( m_pCamera ) , f );
//
//		RwRGBAReal color = {directionalvalue2, directionalvalue2, directionalvalue2, 0.0f};
//		RpLightSetColor(m_pLightDirect2, &color);
//
//        RpWorld * pWorld = RpWorldAddLight(m_pWorld, m_pLightDirect2);
//    }

	return TRUE;
}

AgcWindow *	MyEngine::OnSetMainWindow	()	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.
{
	//g_MainWindow.AddChild( &g_csAgcmMinimap.m_clMinimapWindow );
	
	return &g_MainWindow;
}

BOOL		MyEngine::OnRegisterModulePreCreateWorld		()
{

	return TRUE;
}

BOOL		MyEngine::OnRegisterModule		()
{
	g_AuCircularBuffer.Init(2048 * 1024);

	REGISTER_MODULE( g_pcsApmMap			, ApmMap		)
	REGISTER_MODULE( g_pcsApmOcTree			, ApmOcTree		)
	REGISTER_MODULE( g_pcsAgcmOcTree		, AgcmOcTree	)
	REGISTER_MODULE( g_pcsAgcmPostFX					, AgcmPostFX					)
	REGISTER_MODULE( g_pcsAgcmRender		, AgcmRender	)
	REGISTER_MODULE( g_pcsAgcmLODManager	, AgcmLODManager)
	REGISTER_MODULE( g_pcsApmObject			, ApmObject		)

	// 마고자 (2005-06-20 오후 5:48:59) : 
	// Spawn 이벤트를 스트리밍 하는 과정에서
	// Sub로 콜백이 들어가는게 있는데 거기서 EnumEnd 가 박혀 버려서
	// 그 이후에 스트리밍 돼는건 하나도 읽어지지가 않는다..
	// 그래서 그것보다 먼저 처리하기 위해 여기에 콜백 등록 위치시킴.
	if ( g_pcsApmObject &&
		!g_pcsApmObject->AddStreamCallback(
		APMOBJECT_DATA_OBJECT						,
		CPlugin_BossSpawn::BossInfoStreamReadCB		,
		CPlugin_BossSpawn::BossInfoStreamWriteCB	,
		( CPlugin_BossSpawn::GetClass() )	)
	)
		return FALSE;

	REGISTER_MODULE( g_pcsAgcmResourceLoader, AgcmResourceLoader )
	REGISTER_MODULE_SINGLETON( AgcmMap )

	if( g_pcsAgcmResourceLoader )
	{
		// 마고자 (2004-01-29 오후 2:58:33) : 스레드 로딩 사용하지 않음.
		g_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;
		g_pcsAgcmResourceLoader->EnableEncryption( FALSE );
		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\Etc\\"))
			return FALSE;
	}

	REGISTER_MODULE( g_pcsAgcmObject		, AgcmObject	)
	REGISTER_MODULE( g_pcsAgpmTimer			, AgpmTimer		)

	//@{ Jaewon 20041229
	REGISTER_MODULE( g_pcsAgcmAmbOcclMap				, AgcmAmbOcclMap				)
	//@} Jaewon

	//@{ Jaewon 20040621
	REGISTER_MODULE( g_pcsAgcmDynamicLightmap			, AgcmDynamicLightmap			)
	//@} Jaewon

	AGCMMAP_THIS->SetCollisionAtomicCallback			( AgcmObject::CBGetCollisionAtomic				);
	AGCMMAP_THIS->SetCollisionAtomicCallbackFromAtomic	( AgcmObject::CBGetCollisionAtomicFromAtomic	);
	AGCMMAP_THIS->SetVertexColorLockMode				( rpGEOMETRYLOCKPRELIGHT | rpGEOMETRYLOCKVERTICES );

	// gemani 2005.2.22	
	REGISTER_MODULE( g_pcsAgcmNatureEffect				, AgcmNatureEffect				)

	REGISTER_MODULE( g_pcsAgpmFactors					, AgpmFactors					)
	REGISTER_MODULE( g_pcsAgpmCharacter					, AgpmCharacter					)
	REGISTER_MODULE( g_pcsAgpmBillInfo					, AgpmBillInfo					)
	REGISTER_MODULE( g_pcsAgcmFont						, AgcmFont						)
	REGISTER_MODULE( g_pcsAgcmSound						, AgcmSound						)
	REGISTER_MODULE( g_pcsAgcmShadow					, AgcmShadow					)
	REGISTER_MODULE( g_pcsAgcmShadowmap					, AgcmShadowmap					)
	REGISTER_MODULE( g_pcsApmEventManager				, ApmEventManager				)
	REGISTER_MODULE( g_pcsAgpmArchlord					, AgpmArchlord					)
	REGISTER_MODULE( g_pcsAgcmCharacter					, AgcmCharacter					)
	REGISTER_MODULE( g_pcsAgpmGrid						, AgpmGrid						)
	REGISTER_MODULE( g_pcsAgcmUIControl					, AgcmUIControl					)
	REGISTER_MODULE( g_pcsAgpmItem						, AgpmItem						)
	REGISTER_MODULE( g_pcsAgpmDropItem					, AgpmDropItem					)
	REGISTER_MODULE( g_pcsAgcmItem						, AgcmItem						)
	REGISTER_MODULE( g_pcsAgpmUnion						, AgpmUnion						)
	REGISTER_MODULE( g_pcsAgpmParty						, AgpmParty						)
	REGISTER_MODULE( g_pcsAgpmEventNature				, AgpmEventNature				)

	//REGISTER_MODULE( g_pcsAgcmEffect					, AgcmEffect					)
	//if( g_pcsAgcmEffect )	g_pcsAgcmEffect->SetNecessary		( m_pWorld, m_pCamera		);

	REGISTER_MODULE( g_pcsAgcmUVAnimation				, AgcmUVAnimation				)

	if( ALEF_LOADING_MODE == LOAD_NORMAL && ALEF_USE_EFFECT )
	{
		REGISTER_MODULE( g_pcsAgcmEff2						, AgcmEff2						)
		if( g_pcsAgcmEff2 )
		{
			g_pcsAgcmEff2->bSetGlobalVariable( NULL ,  NULL/*pFrmMainCharac*/, m_pWorld, m_pCamera );
			AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_USEPACK );
		}
	}

	REGISTER_MODULE( g_pcsAgcmEventNature				, AgcmEventNature				)
	REGISTER_MODULE( g_pcsAgpmEventSpawn				, AgpmEventSpawn				)
	REGISTER_MODULE( g_pcsAgpmEventNPCDialog			, AgpmEventNPCDialog				)
	REGISTER_MODULE( g_pcsAgpmEventNPCTrade				, AgpmEventNPCTrade				)
	REGISTER_MODULE( g_pcsAgpmEventAuction				, AgpmAuction					)
	REGISTER_MODULE( g_pcsApmEventManagerDlg			, ApmEventManagerDlg			)
	//REGISTER_MODULE( g_pcsAgcmCamera					, AgcmCamera					)
	REGISTER_MODULE( g_pcsAgcmCamera2					, AgcmCamera2					)

	REGISTER_MODULE( g_pcsAgcmWater						, AgcmWater						)
	REGISTER_MODULE( g_pcsAgpmShrine					, AgpmShrine					)
	REGISTER_MODULE( g_pcsAgcmShrineDlg					, AgcmShrineDlg					)
	REGISTER_MODULE( g_pcsAgcmEventNatureDlg			, AgcmEventNatureDlg			)
	REGISTER_MODULE( g_pcsAgcmGrass						, AgcmGrass						)
	
	REGISTER_MODULE( g_pcsAgpmSkill						, AgpmSkill						)
	REGISTER_MODULE( g_pcsAgcmSkill						, AgcmSkill						)
	REGISTER_MODULE( g_pcsAgpmItemConvert				, AgpmItemConvert				)

	REGISTER_MODULE( g_pcsAgpmPathFind					, AgpmPathFind					)
	REGISTER_MODULE( g_pcsAgpmAI2						, AgpmAI2						)

	REGISTER_MODULE( g_pcsAgpmGuild						, AgpmGuild						)
	REGISTER_MODULE( g_pcsAgpmSiegeWar					, AgpmSiegeWar					)

	REGISTER_MODULE( g_pcsAgpmTax						, AgpmTax						)
	REGISTER_MODULE( g_pcsAgpmGuildWarehouse			, AgpmGuildWarehouse			)

	REGISTER_MODULE( g_pcsAgcmEventSpawnDlg				, AgcmEventSpawnDlg				)
	REGISTER_MODULE( g_pcsAgpmEventTeleport				, AgpmEventTeleport				)
	REGISTER_MODULE( g_pcsAgcmEventTeleportDlg			, AgcmEventTeleportDlg			)
	REGISTER_MODULE( g_pcsAgcmEventNPCDialogDlg			, AgcmEventNPCDialogDlg			)
	REGISTER_MODULE( g_pcsAgcmEventNPCTradeDlg			, AgcmEventNPCTradeDlg			)
	REGISTER_MODULE( g_pcsAgcmEventAuctionDlg			, AgcmEventAuctionDlg			)
	REGISTER_MODULE( g_pcsAgcmEventEffect				, AgcmEventEffect				)
	REGISTER_MODULE( g_pcsAgcmBlockingDlg				, AgcmBlockingDlg				)
	REGISTER_MODULE( g_pcsAgcmPreLODManager				, AgcmPreLODManager				)
	REGISTER_MODULE( g_pcsAgcmLODDlg					, AgcmLODDlg					)
	REGISTER_MODULE( g_pcsAgcmEffectDlg					, AgcmEffectDlg					)
	REGISTER_MODULE( g_pcsAgcmFactorsDlg				, AgcmFactorsDlg				)
	//REGISTER_MODULE( g_pcsAgcmAnimationDlg				, AgcmAnimationDlg				)
	//REGISTER_MODULE( g_pcsAgcmSkillDlg					, AgcmSkillDlg					)

	REGISTER_MODULE( g_pcsAgcmWaterDlg					, AgcmWaterDlg					)
	REGISTER_MODULE( g_pcsAgcmWaterDlg2					, AgcmWaterDlg2					)
	REGISTER_MODULE( g_pcsAgcmTuner						, AgcmTuner						)
	//REGISTER_MODULE( g_pcsAgpmEventMasterySpecialize	, AgpmEventMasterySpecialize	)

	// 마고자 (2004-06-15 오후 5:57:51) : 네똥앚씨 추가~.
	REGISTER_MODULE( g_pcsAgcmEventManager				, AgcmEventManager				)
	REGISTER_MODULE( g_pcsAgpmEventBank					, AgpmEventBank					)
	REGISTER_MODULE( g_pcsAgcmEventBank					, AgcmEventBank					)

	REGISTER_MODULE( g_pcsAgpmEventItemConvert			, AgpmEventItemConvert			)
	REGISTER_MODULE( g_pcsAgcmEventItemConvert			, AgcmEventItemConvert			)

	REGISTER_MODULE( g_pcsAgpmEventGuild				, AgpmEventGuild				)
	REGISTER_MODULE( g_pcsAgcmEventGuild				, AgcmEventGuild				)

	REGISTER_MODULE( g_pcsAgpmProduct					, AgpmProduct					)
	REGISTER_MODULE( g_pcsAgpmEventProduct				, AgpmEventProduct				)
	REGISTER_MODULE( g_pcsAgcmEventProduct				, AgcmEventProduct				)
		
	REGISTER_MODULE( g_pcsAgcmShadow2					, AgcmShadow2					)

	REGISTER_MODULE( g_pcsAgpmEventBinding				, AgpmEventBinding				)
	//REGISTER_MODULE( g_pcsAgpmBattleGround				, AgpmBattleGround				)
	REGISTER_MODULE( g_pcsAgcmEventBindingDlg			, AgcmEventBindingDlg			)
	
	REGISTER_MODULE( g_pcsAgpmQuest						, AgpmQuest						)
	REGISTER_MODULE( g_pcsAgpmEventQuest				, AgpmEventQuest				)
	REGISTER_MODULE( g_pcsAgcmEventQuestDlg				, AgcmEventQuestDlg				)

	REGISTER_MODULE( g_pcsAgpmEventSkillMaster			, AgpmEventSkillMaster			)
	REGISTER_MODULE( g_pcsAgcmEventSkillMaster			, AgcmEventSkillMaster			)
	REGISTER_MODULE( g_pcsAgcmEventSkillMasterDlg		, AgcmEventSkillMasterDlg		)

	REGISTER_MODULE( g_pcsAgpmEventRefinery				, AgpmEventRefinery				)
	REGISTER_MODULE( g_pcsAgcmEventRefinery				, AgcmEventRefinery				)

	REGISTER_MODULE( g_pcsAgcmEventProductDlg			, AgcmEventProductDlg			)

	REGISTER_MODULE( g_pcsAgpmEventPointLight			, AgpmEventPointLight			)
	REGISTER_MODULE( g_pcsAgcmEventPointLight			, AgcmEventPointLight			)

	REGISTER_MODULE( g_pcsAgpmRide						, AgpmRide						)
	REGISTER_MODULE( g_pcsAgcmRide						, AgcmRide						)

	REGISTER_MODULE( g_pcsAgpmWantedCriminal			, AgpmWantedCriminal			)
	REGISTER_MODULE( g_pcsAgpmEventGacha				, AgpmEventGacha				)
	REGISTER_MODULE( g_pcsAgcmEventGachaDlg				, AgcmEventGachaDlg				)

	/*
	REGISTER_MODULE( g_pcsAgcmUITax						, AgcmUITax						)
	REGISTER_MODULE( g_pcsAgcmUIGuildWarehouse			, AgcmUIGuildWarehouse			)
	*/

	g_Const.Update();

	theApp.SetStartUpProcess( 10 );

	//REGISTER_MODULE( g_csAgcmObjectTypeDlg		)
	RwTextureSetMipmapping		( TRUE );
	RwTextureSetAutoMipmapping	( TRUE );
	
	if( g_pcsAgcmRender )
	{
		g_pcsAgcmRender->SetUseLODBoundary( false );
		g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame(m_pCamera));		// modify by gemani
		g_pcsAgcmRender->SetLight(m_pLightAmbient, m_pLightDirect);
		g_pcsAgcmRender->SetWorldCamera( m_pWorld, m_pCamera);
	}

	if( g_pcsApmMap )
	{
		g_pcsApmMap->SetLoadingMode	( FALSE , TRUE	);
		g_pcsApmMap->SetAutoLoadData( FALSE			);
	}

	if( g_pcsApmObject )
	{
		g_pcsApmObject->SetMaxObject			(100000);
		g_pcsApmObject->SetMaxObjectTemplate	(5000);
	}

	if( g_pcsAgpmEventTeleport )
	{
		g_pcsAgpmEventTeleport->SetMaxTeleportPoint(300);
		g_pcsAgpmEventTeleport->SetMaxTeleportGroup(300);
	}

	if( g_pcsAgcmObject )
	{
		g_pcsAgcmObject->SetClumpPath(".\\Object\\");
		g_pcsAgcmObject->SetMaxAnimation(1000);
		g_pcsAgcmObject->SetAnimationPath(".\\Object\\Animation\\");
		g_pcsAgcmObject->SetSetupObject(TRUE, TRUE);
	}

	theApp.SetStartUpProcess( 20 );

//	// 모듈 데이타 초기화..re
//	{
//		// 섹터를 한개만 로딩하게 함..
//		ALEF_LOAD_RANGE_X2 = ALEF_LOAD_RANGE_X1 + 1;
//		ALEF_LOAD_RANGE_Y2 = ALEF_LOAD_RANGE_Y1 + 1;
//	}

	// Texture Dictionary의 이름을 정해주자.
	//AGCMMAP_THIS->SetTexDict				( ".\\Texture\\Tiles.txd" );

	if( g_pcsAgcmWater )
	{
		g_pcsAgcmWater->SetWorldCamera( g_pEngine->m_pCamera );
		g_pcsAgcmWater->m_iReflectionUpdateCount = 0;
	}

	if( AGCMMAP_THIS )
		AGCMMAP_THIS->SetCamera				( g_pEngine->m_pCamera );

//	char	strMapDataDirectory[ 1024 ];
//	wsprintf( strMapDataDirectory , "%s\\MAP\\Data" , ALEF_CURRENT_DIRECTORY );
	if( g_pcsApmMap && !g_pcsApmMap->Init( NULL ) )
		return FALSE;

	char	strImageDataDirectory[ 1024 ];
	wsprintf( strImageDataDirectory , "%s" , ALEF_CURRENT_DIRECTORY );
	if( AGCMMAP_THIS )
		AGCMMAP_THIS->SetDataPath( strImageDataDirectory );

	/******** Parn님이 작업 시작 *********/
	if( g_pcsAgcmObject )
		g_pcsAgcmObject->SetRpWorld( m_pWorld );
	/******** Parn님이 작업 끝 *********/

	char strFileNameTileScript [ 1024 ];
	wsprintf( strFileNameTileScript , "%s\\%s" , ALEF_CURRENT_DIRECTORY , ALEF_TILE_LIST_FILE_NAME );
	char strImagePath [ 1024 ];
	wsprintf( strImagePath , "%s\\Map\\Tile" , ALEF_CURRENT_DIRECTORY );

	if( AGCMMAP_THIS )
	{
		// 데이타 로딩과 폴리건생성..
		AGCMMAP_THIS->InitMapClientModule	( m_pWorld );
		AGCMMAP_THIS->InitMapToolData		( strFileNameTileScript , strImagePath );
	}

	if( g_pcsAgpmEventNature )
	{
		g_pcsAgpmEventNature->AddDefaultSkySet();
	}

	if( g_pcsAgpmCharacter )
	{
		g_pcsAgpmCharacter->SetMaxCharacterTemplate	( 1000							);
		g_pcsAgpmCharacter->SetMaxCharacter			( 1000							);
	}

	if( g_pcsAgcmCharacter )
	{
		g_pcsAgcmCharacter->SetWorld					( m_pWorld, m_pCamera			);
		g_pcsAgcmCharacter->SetMaxAnimations			( 20000							);
		g_pcsAgcmCharacter->SetClumpPath				( ".\\Character\\"				);
		g_pcsAgcmCharacter->SetAnimationPath			( ".\\Character\\Animation\\"	);
		g_pcsAgcmCharacter->SetSendMoveDelay			( 500							);
		g_pcsAgcmCharacter->AddFlags					( E_AGCM_CHAR_FLAGS_CONVERT_ANIMTYPE2	);
	}

	if( g_pcsAgcmObject )
	{
		g_pcsAgcmObject->		SetClumpPath			( ".\\Object\\"					);
		g_pcsAgcmObject->		SetAnimationPath		( ".\\Object\\Animation\\"		);
		g_pcsAgcmObject->		SetRpWorld				( m_pWorld						);
		g_pcsAgcmObject->		SetMaxAnimation			( 1000							);
	}

	if( g_pcsAgpmItem && g_pcsAgcmSkill )
	{
		//g_pcsAgcmSkill->SetMaxEffectNum				( 10000							);

		g_pcsAgpmItem->		SetMaxItem				( 3000							);
		g_pcsAgpmItem->		SetMaxItemTemplate		( 20000							);
		g_pcsAgpmItem->		SetMaxItemRemove		( 3000							);
		g_pcsAgpmItem->		SetMaxItemOptionTemplate( 2000							);

		g_pcsAgcmItem->SetClumpPath					( ".\\Character\\"				);
		g_pcsAgcmItem->SetWorld						( m_pWorld						);
		g_pcsAgcmItem->SetMaxItemClump				( 1000							);
	}

	if( g_pcsAgpmEventNature )
	{
		g_pcsAgpmEventNature->SetTime		( 60 * 1000 * 60 + 1 );
		g_pcsAgpmEventNature->StopTimer	();
	}

	if( g_pcsAgcmRender )
	{
		g_pcsAgcmRender->SetCallbackPostRender	( __AgcmRender_PostRenderCallback , this );
		g_pcsAgcmRender->SetCallbackPostRender2	( __AgcmRender_PostRenderCallback2 , this );
		
		g_pcsAgcmRender->SetCallbackPostRenderOcTreeClear	( __AgcmRender_Maptool_PreRenderCallback , this );
	}
	
	if( g_pcsAgcmFont )
		g_pcsAgcmFont->SetWorldCamera			( m_pCamera			);
	
	if( AGCMMAP_THIS )
		AGCMMAP_THIS->SetAutoLoadRange	( g_Const.m_nLoadRange_Data , g_Const.m_nLoadRange_Rough , g_Const.m_nLoadRange_Detail );

	/*
	//Effect 초기화
	if( g_pcsAgcmEffect )
	{
		g_pcsAgcmEffect->SetIniFilePath		( ".\\Effect\\Ini\\"		);
		g_pcsAgcmEffect->SetAnimationPath	( ".\\Effect\\Animation\\"	);
		g_pcsAgcmEffect->SetTexturePath		( ".\\TEXTURE\\Effect\\"	);
		g_pcsAgcmEffect->SetClumpPath		( ".\\Effect\\Clump\\"		);
	//	g_pcsAgcmEffect->SetSoundPath		( ".\\Sound\\Effect\\"		);
	//	g_pcsAgcmEffect->SetNecessary		( m_pWorld, m_pCamera		);
		g_pcsAgcmEffect->SetTexDictFile		( ".\\Texture\\Effect.txd"	);
	}
	*/
	
	theApp.SetStartUpProcess( 30 );

	if( g_pcsAgcmEventNature )
	{
		RwRGBA	stFogColor = {255, 255, 255, 255};
		// g_pcsAgcmEventNature->SetSkyRadius( AGCMMAP_THIS->GetRoughLoadRadius() * 2500 + 2500 );
		g_pcsAgcmEventNature->SetSkyRadius( (INT32) g_Const.m_nLoadRange_Rough );
		g_pcsAgcmEventNature->Init( m_pWorld , m_pLightAmbient , m_pLightDirect , m_pCamera );
		//g_pcsAgcmEventNature->SetFog( TRUE , AGCMMAP_THIS->GetRoughLoadRadius() * 2500 , 0.00002f , &stFogColor , rwFOGTYPEEXPONENTIAL2 );
		g_pcsAgcmEventNature->SetFog( g_Const.m_bUseFog , g_Const.m_fFogDistance , g_Const.m_fFogFarClip , &stFogColor );

		g_pcsAgcmEventNature->LoadTextureScriptFile();
		g_pcsAgcmEventNature->SetSkyTemplateID( 0 , ASkySetting::DA_ALL );
	}
	

	if( g_pcsAgpmEventNature && g_pcsAgcmEventNature )
	{
		char	strSkySet[ 1024 ];
		wsprintf( strSkySet , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , AGPMNATURE_SKYSET_FILENAME );
		if(  g_pcsAgpmEventNature->SkySet_StreamRead( strSkySet , FALSE ) )
		{
			TRACE( "g_pcsAgpmEventNature->SkySet_StreamRead( strSkySet ) Success\n" );
		}
		else
		{
			TRACE( "스카이 템플릿 로딩실패\n" );
			return FALSE;
		}

		g_pcsAgpmEventNature->SetTime		( 0 );
		g_pcsAgpmEventNature->StopTimer	();
		g_pcsAgcmEventNature->SetTimeForce( 0 );

		g_pcsAgcmEventNature->ApplySkySetting();
		g_pcsAgcmEventNature->ShowSky( g_Const.m_bUseSky );
	}

//	m_csAgcmShadow.	AddWorldCamera			( m_pCamera		);
//
//	m_csAgcmShadow.	AddWorldDirectLight		( m_pLightDirect	);
//	m_csAgcmShadow.	AddWorldAmbientLight	( m_pLightAmbient	);
//	m_csAgcmShadow.	AddRpWorld				( m_pWorld			);

	if( g_pcsAgpmShrine )
		g_pcsAgpmShrine->SetMaxShrine				( 100				);
	if( g_pcsAgpmShrine )
		g_pcsAgpmShrine->SetMaxShrineTemplate		( 100				);

	//if( g_pcsAgcmEventEffect )
	//	g_pcsAgcmEventEffect->SetEffectDataNum(10000);

	if( g_pcsAgpmSkill )
	{
		g_pcsAgpmSkill->		SetMaxSkill				( 1000							);
		g_pcsAgpmSkill->		SetMaxSkillTemplate		( 3000							);
		g_pcsAgpmSkill->		SetMaxSkillSpecializeTemplate	( 10					);
		g_pcsAgpmSkill->		SetMaxSkillTooltipTemplate	( 200						);
	}
	
	//g_pcsAgcmEventNature->SetSkyTemplateID( 0 );
	if( g_pcsAgpmTimer )
		g_pcsAgpmTimer->SetTime( 12 );
	if( g_pcsAgcmEventNature )
		g_pcsAgcmEventNature->ApplySkySetting();
	
	if( g_pcsAgcmLODManager )
		g_pcsAgcmLODManager->SetForceZeroLevel( TRUE );

	if( g_pcsAgpmEventBinding )
		g_pcsAgpmEventBinding->SetMaxBindingData(100);
	
	// 케릭터모듈 접속하지 않음..
	AgcmCharacter::m_sbStandAloneMode	= TRUE;

	// 페레럴 카메라 설정..
	MoveSubCameraToCenter();

	RpMTextureCreateShaders();

	theApp.SetStartUpProcess( 40 );

	if( g_pcsAgpmQuest )
	{
		g_pcsAgpmQuest->SetMaxTemplateCount	( AGPDQUEST_MAX_QUEST_TEMPLATE	);
		g_pcsAgpmQuest->SetMaxGroupCount	( AGPDQUEST_MAX_QUEST			);
	}

	if( g_pcsAgcmCamera2 )
	{
		// 카메라 기능을 꺼버림..
		g_pcsAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_login	);
	}

	AuPackingManager*	pPackingManager = AuPackingManager::MakeInstance();
	//pPackingManager->setLoadFromPackingData(false);

	g_csAcuParticleDraw.Init();
	g_csAcuParticleDraw.SetWorldCamera(m_pCamera);
	
	return TRUE;
}

void		MyEngine::OnDestroyLight			()
{
	DestroyLights();
}

BOOL		MyEngine::OnInit()
{
//	m_DP8.StartDP( "61.96.38.110:9998");
	
	AcuFrameMemory::Create();
	AcuFrameMemory::InitManager(40000000);				// 40MB 할당

	ShowFps( TRUE );

	return TRUE;
}

VOID MyEngine::OnWorldRender()
{
	g_csAcuParticleDraw.CameraMatrixUpdate();
	g_pcsAgcmRender->Render();
}

void MyEngine::OnRender	()
{
	PROFILE("AgcEngine::OnRender");
    ++m_nFrameCounter;

	static bool bRenderStateSet = false;
	
	switch( m_nActivated )
	{
	case	AGCENGINE_NORMAL	:
	case	AGCENGINE_MINOR		:
		// do no op
		break;
	case	AGCENGINE_SUSPENDED	:
	default:
		return;
	}

	{
	PROFILE("AgcEngine::OnRender-OnRenderPreCameraUpdate()");
	// 카메라 업데이트 돼기 전 , 버쳘 펑션..
	OnRenderPreCameraUpdate	();
	}
	
	if( m_pCamera == NULL )		return;
    RwCameraClear( m_pCamera , &m_rgbBackgroundColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE );

	LockFrame();
	if( RwCameraBeginUpdate( g_pEngine->m_pCamera ) )
	{
		// Fog Set
		RwD3D9SetRenderState( D3DRS_FOGEND		,  *((DWORD*)(&g_pcsAgcmRender->m_fFogEnd)));

		UnlockFrame();
		if(!bRenderStateSet)
		{
			RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLNONE	);
			bRenderStateSet = true;
		}

		OnWorldRender();

		// Add by gemani(2004.03.30): octree관련 정보를 출력
		g_pcsAgcmOcTree->DrawOctreeDebugSectorBox();
		g_pcsAgcmOcTree->DrawDebugBoxes();

		//if( m_pCurrentFullUIModule ) m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_RENDER , NULL);
//		{
//		PROFILE("AgcEngine::OnRender-OnRender()");
//		if( m_pCurrentFullUIModule ) m_pCurrentFullUIModule->OnRender( NULL );
//		}
						
		RwCameraEndUpdate( g_pEngine->m_pCamera );
	}
	else
		UnlockFrame();

	if( IsRenderObject() )	RenderSubCamera				();
	else					RenderSubCameraGeometryOnly	();

//	if( m_pCurrentFullUIModule )
//		m_pCurrentFullUIModule->OnPostRender( RwCameraGetRaster( m_pCamera ) );

	// Display Camera Raster
	RsCameraShowRaster( m_pCamera );	// Flip the Raster! - -+
}


BOOL	MyEngine::OnCameraResize			( RwRect * pRect )
{
    if( m_clCamera->CameraSize( m_pCamera , pRect , 
        DEFAULT_VIEWWINDOW , DEFAULT_ASPECTRATIO_NORMAL ) )
	{
		m_bSuspendedFlag = false;
	}
	else
		m_bSuspendedFlag = true;

	SetProjection();

	if( m_pSubCameraMainCameraSubRaster && m_pSubCameraMainCameraSubZRaster )
	{
		RwRect subRect;
		
		subRect.x = (RwInt32)(pRect->w * 0.75f);
		subRect.y = 0;
		
		subRect.w = (RwInt32)(pRect->w * 0.25f);
		subRect.h = (RwInt32)(pRect->w * 0.25f);
		
		RwRasterSubRaster(m_pSubCameraMainCameraSubRaster, 
			RwCameraGetRaster(m_pCamera), &subRect);
		RwCameraSetRaster(m_pParallelCamera, m_pSubCameraMainCameraSubRaster);

		RwRasterSubRaster(m_pSubCameraMainCameraSubZRaster, 
			RwCameraGetZRaster(m_pCamera), &subRect);
		RwCameraSetZRaster(m_pParallelCamera, m_pSubCameraMainCameraSubZRaster);
	}
	
    return TRUE;	
}

BOOL		MyEngine::OnDestroyCamera		()
{
	if( m_pParallelCamera )
	{
#ifdef _DPVS
		RpDPVSWorldRemoveCamera(m_pWorld, m_pParallelCamera);
#else
        RpWorldRemoveCamera(m_pWorld, m_pParallelCamera);
#endif //_DPVS
        m_clCamera->CameraDestroy(m_pParallelCamera);
		
		m_pParallelCamera = NULL;
	}

    if( m_pCamera )
    {
#ifdef _DPVS
		RpDPVSWorldRemoveCamera(m_pWorld, m_pCamera);
#else
        RpWorldRemoveCamera(m_pWorld, m_pCamera);
#endif //_DPVS

        /*
         * This assumes the camera was created the democom way...
         */
        m_clCamera->CameraDestroy(m_pCamera);

		m_pCamera = NULL;
    }

	return TRUE;
}

BOOL		MyEngine::OnRenderPreCameraUpdate	()
{
	if (g_pcsAgcmShadow2)
		g_pcsAgcmShadow2->ShadowTextureRender();

	//@{ Jaewon 20040817
	// if the post fx is on, change the main
	// render target to the offscreen buffer.
	if(g_pcsAgcmPostFX)
		g_pcsAgcmPostFX->changeRenderTarget();
	//@} Jaewon

	return TRUE;
}

BOOL		MyEngine::OnAddModule()
{
	//g_pcsAgcmWater->LoadStatusInfoT1FromINI("./Ini/WaterStatusT1.ini");
	return TRUE;
}

void		MyEngine::OnTerminate()
{
	this->SetFullUIModule( NULL );

	//@{ Jaewon 20050107
	// if created, it should be destroyed here.
	m_ambOcclMap.destroy();
	//@] Jaewon

	// 20070515 강제 종료 코드
	{
		g_pcsAgcmSound->OnDestroy();
		// 강제로 종료시켜버림..
		::ExitProcess( 0 );
	}

	AgcEngine::OnTerminate();
	AcuFrameMemory::Release();

	g_csAcuParticleDraw.Release();

}

//@{ Jaewon 20050103
// progress callback for ambient occlusion maps
RwBool illuminateProgressCB(RwInt32 message, RwReal value)
{
	//@{ Jaewon 20050113
	g_MyEngine.m_inIlluminationCallBack = true;
	//@} Jaewon

	g_MyEngine.m_progress = value;
	//g_pcsAgcmRender->OnIdle(g_MyEngine.GetClockCount());
	//g_MyEngine.OnRender();

	//@{ Jaewon 20050113
    MSG msg;
	// pump messages.
    while(PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if(!GetMessageW(&msg, NULL, 0, 0))
        {
            PostQuitMessage((int)msg.wParam);
            return FALSE;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

	if(false == g_MyEngine.m_computingAmbOcclMaps)
	{
		g_MyEngine.m_inIlluminationCallBack = false;
		return FALSE;
	}
	//@} Jaewon

	//@{ Jaewon 20050110
	//if(GetAsyncKeyState(VK_PAUSE)&0x8000)
	//// pause computing.
	//{
	//	DisplayMessage("Computing paused.");
	//	return FALSE;
	//}
	//@} Jaewon

	//@{ Jaewon 20050113
	g_MyEngine.m_inIlluminationCallBack = false;
	//@} Jaewon

	return TRUE;
}
//@} Jaewon

VOID	MyEngine::OnEndIdle()
{
	//@{ Jaewon 20050103
	// incremental ambient occlusion map illumination
	//@{ Jaewon 20050113
	// m_inIlluminationCallBack check added.
	if(m_computingAmbOcclMaps && !m_inIlluminationCallBack)
	//@} Jaewon
	{
		if(m_ambOcclMap.getSessionTotalObj() > 0
			&& m_ambOcclMap.getSessionStartObj() >= m_ambOcclMap.getSessionTotalObj())
		// illumination completed.
		{
			//@{ Jaewon 20050113
			CUITileList_Others::Plugin *pPlugin = g_pMainFrame->m_pTileList->m_pOthers->GetCurrentPlugin();
			if(pPlugin && strcmp(pPlugin->name, "Ambient Occlusion Map Plugin")==0)
			{
				// update progress bars.
				((CPlugin_AmbOcclMap*)(pPlugin->pWnd))->setObjectProgress(100);
				((CPlugin_AmbOcclMap*)(pPlugin->pWnd))->setSliceProgress(100);

				//@{ Jaewon 20050204
				((CPlugin_AmbOcclMap*)(pPlugin->pWnd))->endComputing();
				//@} Jaewon
			}
			//@} Jaewon

			m_computingAmbOcclMaps = false;
		}
		else
		{
			int numObj = m_ambOcclMap.compute(illuminateProgressCB);
			ASSERT(numObj >= 0);
		}
	}
	//@} Jaewon

	AcuFrameMemory::Clear();

	AgcEngine::OnEndIdle();
}

void	MyEngine::RenderSubCamera()
{
	if( NULL	== GetParallelCamera() ) return;
	if( SC_OFF	== GetSubCameraType	() ) return;

	m_bIsRenderSubCamera = TRUE;

    RwCameraClear( GetParallelCamera(), &m_rgbBackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate( GetParallelCamera() ) )
    {
		// Fog Set
		RwD3D9SetRenderState( D3DRS_FOGEND		,  *((DWORD*)(&g_pcsAgcmRender->m_fFogEnd)));

		switch( GetSubCameraType() )
		{
		case	SC_MINIMAPX4	:
		case	SC_MINIMAP		:
			{
				g_pcsAgcmRender->ResetTickCount();
				g_pcsAgcmRender->SetWorldCamera( m_pWorld , GetParallelCamera() );
				g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame(GetParallelCamera()));
				
				g_pcsAgcmRender->RenderGeometryOnly(1);
				g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame( m_pCamera ) );
				g_pcsAgcmRender->SetWorldCamera( m_pWorld , m_pCamera );
			}
			break;
		case	SC_ROUGH	:
			{
				g_pcsAgcmRender->ResetTickCount();
				g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame(GetParallelCamera()));
				g_pcsAgcmRender->SetWorldCamera( m_pWorld , GetParallelCamera() );
				g_pcsAgcmRender->Render();
				g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame( m_pCamera ) );
				g_pcsAgcmRender->SetWorldCamera( m_pWorld , m_pCamera );
			}
			break; 
		}

        RwCameraEndUpdate( GetParallelCamera() );
    }

	m_bIsRenderSubCamera = FALSE;
}

void	MyEngine::RenderSubCameraGeometryOnly()
{
	if( NULL	== GetParallelCamera() ) return;
	if( SC_OFF	== GetSubCameraType	() ) return;

	m_bIsRenderSubCamera = TRUE;

    RwCameraClear( GetParallelCamera(), &m_rgbBackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate( GetParallelCamera() ) )
    {
		// Fog Set
		RwD3D9SetRenderState( D3DRS_FOGEND		,  *((DWORD*)(&g_pcsAgcmRender->m_fFogEnd)));
		switch( GetSubCameraType() )
		{
		case	SC_MINIMAP	:
			{
				g_pcsAgcmRender->ResetTickCount();
				g_pcsAgcmRender->SetWorldCamera( m_pWorld , GetParallelCamera() );
				g_pcsAgcmRender->RenderGeometryOnly();
				g_pcsAgcmRender->SetWorldCamera( m_pWorld , m_pCamera );
			}
			break;
		case	SC_ROUGH	:
			{
				RwRGBAReal		rgbColorBackup , rgbColorSetup;
				rgbColorBackup = *RpLightGetColor( g_MyEngine.m_pLightAmbient );

				rgbColorSetup.alpha	= 1.0f;
				rgbColorSetup.red	= 1.0f;
				rgbColorSetup.green	= 1.0f;
				rgbColorSetup.blue	= 1.0f;

				RpLightSetColor( g_MyEngine.m_pLightAmbient , &rgbColorSetup );
				RpWorldRemoveLight( g_pEngine->m_pWorld , g_MyEngine.m_pLightDirect );

				if( g_MainWindow.m_pCurrentGeometry )
				{
					rsDWSectorInfo stInfo;
					//RwUInt32		uFlagsBackup	;
					AGCMMAP_THIS->GetDWSector( g_MainWindow.m_pCurrentGeometry , SECTOR_HIGHDETAIL , & stInfo );
					
					// 프리라잇 제거..
//					uFlagsBackup = RpGeometryGetFlags( stInfo.pDWSector->geometry );
//					RpGeometrySetFlags( stInfo.pDWSector->geometry , uFlagsBackup & ~( OR ) );

					VERIFY( RpGeometryLock( stInfo.pDWSector->geometry , rpGEOMETRYLOCKALL ) );
					RwRGBA * pPreLitLum				= RpGeometryGetPreLightColors	( stInfo.pDWSector->geometry		);
					RwRGBA * rgbVertexColorBackup	= new RwRGBA[ stInfo.pDWSector->geometry->numVertices ];
					// 빽업
					memcpy( ( void * ) rgbVertexColorBackup , ( void * ) pPreLitLum , ( sizeof RwRGBA ) * stInfo.pDWSector->geometry->numVertices );
					for( int i = 0 ; i < stInfo.pDWSector->geometry->numVertices ; ++i )
					{
						(*( pPreLitLum + i )).red	= 255;
						(*( pPreLitLum + i )).green	= 255;
						(*( pPreLitLum + i )).blue	= 255;
						(*( pPreLitLum + i )).alpha	= 255;
					}
					RpGeometryUnlock( stInfo.pDWSector->geometry );

					RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void *) FALSE );
					
					g_pcsAgcmRender->ResetTickCount();
					g_pcsAgcmRender->SetWorldCamera( m_pWorld , GetParallelCamera() );
					g_pcsAgcmRender->RenderGeometryOnly();
					g_pcsAgcmRender->SetWorldCamera( m_pWorld , m_pCamera );

					if(g_pEngine->m_bFogEnable)
						RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void *) TRUE );

//					RpGeometrySetFlags( stInfo.pDWSector->geometry , uFlagsBackup );

					VERIFY( RpGeometryLock( stInfo.pDWSector->geometry , rpGEOMETRYLOCKALL ) );
					memcpy( ( void * ) pPreLitLum , ( void * ) rgbVertexColorBackup , ( sizeof RwRGBA ) * stInfo.pDWSector->geometry->numVertices );
					RpGeometryUnlock( stInfo.pDWSector->geometry );

					delete [] rgbVertexColorBackup;
				}

				RpLightSetColor( g_MyEngine.m_pLightAmbient , &rgbColorBackup );
				RpWorldAddLight( g_pEngine->m_pWorld , g_MyEngine.m_pLightDirect );
			}
			break; 
		}

        RwCameraEndUpdate( GetParallelCamera() );
    }

	m_bIsRenderSubCamera = FALSE;
}


void	MyEngine::MoveSubCameraToCenter( ApWorldSector * pSectorInput , BOOL bUseHeight , FLOAT fGetHeight , INT32 nOffset )
{
	// 카메라 위치 랑 view 크기 조절..
	if( NULL == pSectorInput ) pSectorInput = g_MainWindow.m_pCurrentGeometry;
	FLOAT	fX , fZ;
	RwV2d	vWindow;

	switch( GetSubCameraType() )
	{
	case SC_MINIMAP:
		{
			// 미니맵 촬영 용으로 한 디비젼 크기로 설정함.
			vWindow.y	= MAP_SECTOR_WIDTH * 8.0f	;
			vWindow.x	= vWindow.y					;
			RwCameraSetViewWindow( m_pParallelCamera , & vWindow );

			if( NULL == pSectorInput ) return;

			INT x , z ;
			x = pSectorInput->GetArrayIndexX();
			z = pSectorInput->GetArrayIndexZ();

			ApWorldSector * pSector =
					g_pcsApmMap->GetSectorByArray( 
								x + ( MAP_DEFAULT_DEPTH - x ) % MAP_DEFAULT_DEPTH	,
								0 ,
								z + ( MAP_DEFAULT_DEPTH - z ) % MAP_DEFAULT_DEPTH	);
			
			if( pSector )
			{
				fX = pSector->GetXStart() + MAP_SECTOR_WIDTH * 8;
				fZ = pSector->GetZStart() + MAP_SECTOR_WIDTH * 8;
			}
			else
			{
				return;
			}
		}
		break;
	case SC_MINIMAPX4:
		{
			// 미니맵 촬영 용으로 한 디비젼 크기로 설정함.
			vWindow.y	= MAP_SECTOR_WIDTH * 2.0f	;
			vWindow.x	= vWindow.y					;
			RwCameraSetViewWindow( m_pParallelCamera , & vWindow );

			if( NULL == pSectorInput ) return;

			INT x , z ;
			x = pSectorInput->GetArrayIndexX();
			z = pSectorInput->GetArrayIndexZ();

			ApWorldSector * pSector =
					g_pcsApmMap->GetSectorByArray( 
								x + ( MAP_DEFAULT_DEPTH - x ) % MAP_DEFAULT_DEPTH	,
								0 ,
								z + ( MAP_DEFAULT_DEPTH - z ) % MAP_DEFAULT_DEPTH	);
			
			if( pSector )
			{
				fX = pSector->GetXStart() + MAP_SECTOR_WIDTH * 2 + ( nOffset % 4 ) * MAP_SECTOR_WIDTH * 4;
				fZ = pSector->GetZStart() + MAP_SECTOR_WIDTH * 2 + ( nOffset / 4 ) * MAP_SECTOR_WIDTH * 4;
			}
			else
			{
				return;
			}
		}
		break;
	case SC_ROUGH:
		{
			// 러프맵 촬영을 위해서 한 섹터의 크기로 설정.

			vWindow.y	= MAP_SECTOR_WIDTH / 2.0f	;
			vWindow.x	= vWindow.y					;
			RwCameraSetViewWindow( m_pParallelCamera , & vWindow );

			ApWorldSector * pSector = pSectorInput;
			if( pSectorInput )
			{
				fX = pSector->GetXStart() + ( pSector->GetXEnd() - pSector->GetXStart() ) / 2 ;
				fZ = pSector->GetZStart() + ( pSector->GetZEnd() - pSector->GetZStart() ) / 2 ;
			}
			else
			{
				return;
			}

			fGetHeight	= 0.0f;
			bUseHeight	= TRUE;	
			{
				int nSegmentX , nSegmentZ;
				FLOAT	fHeight;

				for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) + 1 ; ++ nSegmentZ )
				{
					for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) + 1 ; ++ nSegmentX )
					{
						fHeight = AGCMMAP_THIS->GetHeight( 
							pSector->GetXStart()	+ ( FLOAT ) nSegmentX * MAP_STEPSIZE ,
							pSector->GetZStart()	+ ( FLOAT ) nSegmentZ * MAP_STEPSIZE ,
							SECTOR_MAX_HEIGHT );
						
						if( fGetHeight < fHeight )
						{
							fGetHeight = fHeight;
						}
					}
				}
			}

		}
		break;
	case SC_OBJECTCAPTURE:
		{
			// 오브젝트 촬영을 위해서 미니맵 출력의 2배의 크기로 촬영함.

			vWindow.y	= MAP_SECTOR_WIDTH * 4.0f	;
			vWindow.x	= vWindow.y					;
			RwCameraSetViewWindow( m_pParallelCamera , & vWindow );

			if( NULL == pSectorInput ) return;

			INT x , z ;
			x = pSectorInput->GetArrayIndexX();
			z = pSectorInput->GetArrayIndexZ();

			ApWorldSector * pSector =
					g_pcsApmMap->GetSectorByArray( 
								ALEF_LOAD_RANGE_X1	,
								0 ,
								ALEF_LOAD_RANGE_Y1	);
			
			if( pSector )
			{
				fX = pSector->GetXStart() + MAP_SECTOR_WIDTH * 4;
				fZ = pSector->GetZStart() + MAP_SECTOR_WIDTH * 4;
			}
			else
			{
				return;
			}
		}
		break;
	default:
		// TRACE( "뭐시라!\n" );
		return;
	}

	RwFrame		*pFrame = RwCameraGetFrame	( m_pParallelCamera	);

 	ASSERT( NULL != pFrame					);

	FLOAT	fHeight;
	if( bUseHeight )
		fHeight	= fGetHeight + 5.0f * RwCameraGetNearClipPlane( m_pParallelCamera );
	else
		fHeight	= RwCameraGetFarClipPlane( m_pParallelCamera ) * 0.5f;

	if( fHeight < RwCameraGetFarClipPlane( m_pParallelCamera ) * 0.5f )
		fHeight = RwCameraGetFarClipPlane( m_pParallelCamera ) * 0.5f;
	
	RwV3d		pos	= { fX , fHeight , fZ };

	RwV3d		at	= { 0.0f, -1.0f , 0.0f };
	RwV3d		up	= {	0.0f , 0.0f , -1.0f };
	RwV3d		right = { -1.0f , 0.0f , 0.0f };

	RwMatrix	*	pMatrix = RwFrameGetMatrix( pFrame );

	pMatrix->pos	= pos;
	pMatrix->at		= at;
	pMatrix->up		= up;
	pMatrix->right	= right;

	RwMatrixUpdate			( pMatrix						);
	RwFrameUpdateObjects	( pFrame						);
}

BOOL	MyEngine::RenderAtomicOnly	( RpAtomic * pAtomic )
{
	if( NULL	== GetParallelCamera() ) return FALSE;

	m_bIsRenderSubCamera = TRUE;

    RwCameraClear( GetParallelCamera(), &m_rgbBackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate( GetParallelCamera() ) )
    {
		g_pcsAgcmRender->ResetTickCount();

		RpMTextureDrawStart();
		VERIFY( g_pcsAgcmRender->OriginalDefaultAtomicRenderCallback( pAtomic ) );
		RpMTextureDrawEnd(TRUE);
        RwCameraEndUpdate( GetParallelCamera() );
    }

	m_bIsRenderSubCamera = FALSE;

	RsCameraShowRaster( GetParallelCamera() );	// Flip the Raster! - -+

	return TRUE;
}