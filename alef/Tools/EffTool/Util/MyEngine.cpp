#include "stdafx.h"
#include "resource.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpusrdat.h"
#include "rpspline.h"
#include "rtanim.h"
#include "rplodatm.h"
#include "rpmatfx.h"
#include "rpmorph.h"
#include "rpnormmap.h"
#include "rpuvanim.h"

#include "AcuRpMtexture.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include "MyEngine.h"

#include "time.h"

// Image Loader
#include "rtbmp.h"
#include "rtpng.h"

#include "MainWindow.h"
#include <win.h>

#include "AgcmDynamicLightmap.h"
#include "AgcmRender.h"
#include "AgcmSound.h"
#include "AgcmFont.h"
#include "AgcmUVAnimation.h"
#include "AgcdGeoTriAreaTbl.h"

#include "AgcuEffPath.h"
#include "AgcuEffUtil.h"
#include "AgcuBillBoard.h"

#include "../EffTool.h"
#include "AgcdEffGlobal.h"

#include "Camera.h"
#include "../ToolOption.h"

#include "AcuMathFunc.h"
#include "AcuRpUVAnimData.h"

MainWindow			g_MainWindow;
MyEngine			g_MyEngine;


AcuObject			g_csAcuObject;		//WHAT IS THIS?
AcuFrameMemory		g_csAcuFrameMemory;	//WHAT IS THIS?


ApmMap*					g_pcsApmMap;
AgcmEff2*				g_pAgcmEff2;
AgcmDynamicLightmap*	g_pAgcmDynamicLightmap;
ApmOcTree*				g_pcsApmOcTree;
AgcmOcTree*				g_pcsAgcmOcTree;
AgcmPostFX*				g_pcsAgcmPostFX;
AgcmRender*				g_pAgcmRender;
AgcmSound*				g_pAgcmSound;
AgcmFont*				g_pAgcmFont;
//AgcmTuner*				g_pAgcmTuner;
AgcmResourceLoader*		g_pAgcmResourceLoader;

AgcmUVAnimation*		g_pAgcmUVAnimation;


CEffUt_GeoSphere<VTX_PD>	g_lightSphere
( 25.f
 , 24
 , "point light" );
CEffUt_GeoTrumpet<VTX_PD>	g_lightTrumpet
( 50.f
 , DEF_D2R(30)
 , 24
 , "spot light");

#include "AcuRpMtexture.h"

//----> from modeltool RWUtil.cpp
/*****************************************************************************
* Desc :
*****************************************************************************/
RpLight *Eff2Ut_RwUtilCreateAmbientLight(RwRGBAReal color, RpWorld* world)
{
	RpLight* light;

	light = RpLightCreate(rpLIGHTAMBIENT);

	if(light)
	{
		RpLightSetColor(light, &color);

		if(world)
		{
			RpWorldAddLight(world, light);
		}
	}

	return light;
}
/*****************************************************************************
* Desc :
*****************************************************************************/
RpLight *Eff2Ut_RwUtilCreateMainLight(RwRGBAReal color, RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTDIRECTIONAL);
    if(light)
    {
		RpLightSetColor(light, &color);

        RwFrame *frame = RwFrameCreate();
        if(frame)
        {
            RwV3d yAxis = {0.0f, 1.0f, 0.0f};
			RwV3d xAxis = {1.0f, 0.0f, 0.0f};

            RwFrameRotate(frame, &yAxis, 225.0f, rwCOMBINEREPLACE);
			RwFrameRotate(frame, &xAxis, -45.0f, rwCOMBINEPOSTCONCAT);
            RpLightSetFrame(light, frame);

			if(world)
			{
				RpWorldAddLight(world, light);
			}

            return light;
        }

        RpLightDestroy(light);
    }

    return NULL;
}
//<----



//-----------------------------------------------------------------------------
//
// class MyEngine
//
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////
MyEngine::MyEngine()
: m_pTexHeightMap(NULL)

, m_lpDev(NULL)
, m_lpTexture(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
MyEngine::~MyEngine()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
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
	
	if( !RpHAnimPluginAttach() )
    {
        return FALSE;
    }

    if( !RpSkinPluginAttach() )
    {
        return FALSE;
    }

	if ( !RpSplinePluginAttach() )
	{
		return FALSE;
	}

//	if( !RpDMaterialPluginAttach() )
//	{
//		return FALSE;
//	}

	if( !RpMTexturePluginAttach() )
	{
		return FALSE;
	}

	if ( !RtAnimInitialize() )
	{
		return FALSE;
	}

	if ( !RpMorphPluginAttach() )
	{
		return FALSE;
	}
	
	if ( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}
	
	if ( !RpLODAtomicPluginAttach() )
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
	
	

	if (!RpUVAnimPluginAttach())
	{
		return FALSE;
	}

	if (!RpUVAnimDataPluginAttach())
	{
		return FALSE;
	}

	if (!RpGeoTriAreaTblPluginAttach() )
	{
		return FALSE;
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RpWorld* MyEngine::OnCreateWorld() // 월드를 생성해서 리턴해준다.
{
	RpWorld *world;
	RwBBox bb;

	bb.inf.x = bb.inf.y = bb.inf.z = -10000.0f;
	bb.sup.x = bb.sup.y = bb.sup.z = 10000.0f;

	world = RpWorldCreate(&bb);
	
	RwChar *	path;

	path = RsPathnameCreate(RWSTRING(".\\TEXTURE\\EFFECT\\"));
    RwImageSetPath(path);
    RsPathnameDestroy(path);


    return world;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
RwCamera* MyEngine::OnCreateCamera( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	return CreateCamera( pWorld );
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL MyEngine::OnCreateLight()	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
{
	ASSERT( m_pWorld );

	RwRGBAReal AmbientColor    = {(80.0f/255.0f), (80.0f/255.0f), (80.0f/255.0f), 1.0f};
	RwRGBAReal DirectionColor  = {(119.0f/255.0f), (112.0f/255.0f), (93.0f/255.0f), 1.0f}; //{0.4f,  0.4f, 0.4f, 1.0f};//{(128.0f/255.0f), (128.0f/255.0f), (128.0f/255.0f), 1.0f};//{(255.0f/255.0f), (239.0f/255.0f), (202.0f/255.0f), 1.0f};

	if(!(m_pLightAmbient = Eff2Ut_RwUtilCreateAmbientLight(AmbientColor, m_pWorld)))
	{
		return FALSE;
	}

	if(!(m_pLightDirect = Eff2Ut_RwUtilCreateMainLight(DirectionColor, m_pWorld)))
	{
		return FALSE;
	}

	/*
	m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
    if (m_pLightAmbient)
    {
		RwRGBAReal color = {1.0f, 1.0f, 1.0f, 1.0f};
		RpLightSetColor(m_pLightAmbient, &color);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
    }

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);
    if (m_pLightDirect)
    {
        RwFrame *f = RwFrameCreate();
        RpLightSetFrame( m_pLightDirect , f);

		RwV3d v = { 0.0f , 10.0f , 0.0f };
		RwV3d xaxis = { 1.0f , 0.0f , 0.0f };

		RwFrameTranslate( f , & v , rwCOMBINEREPLACE );
		RwFrameRotate( f , & xaxis , 90.0f , rwCOMBINEREPLACE );

		RwRGBAReal color = {0.0f, 0.0f, 0.0f, 0.0f};
		RpLightSetColor(m_pLightDirect, &color);

        RpWorldAddLight(m_pWorld, m_pLightDirect);
    }/**/

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
AgcWindow *	MyEngine::OnSetMainWindow	()	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.
{
	return &g_MainWindow;
}

void CBFrmRender( RwFrame* pFrm )
{
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
	RwBool	bZWrite	= FALSE,
			bZTest	= FALSE,
			bAlpha	= FALSE;
	RwInt32	nCullMode	= rwCULLMODECULLBACK;

	RwRenderStateGet( rwRENDERSTATEZWRITEENABLE, (void *)&bZWrite );
	RwRenderStateGet( rwRENDERSTATEZTESTENABLE, (void *)&bZTest );
	RwRenderStateGet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)&bAlpha );
	RwRenderStateGet( rwRENDERSTATECULLMODE, (void *)&nCullMode );

	
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)TRUE );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *)TRUE );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE );
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK );

	RwUInt32	flag	= rwIM3D_ALLOPAQUE;
	RwMatrix	mat		= *RwFrameGetLTM( pFrm );
	RwV3dNormalize( &mat.right, &mat.right );
	RwV3dNormalize( &mat.up, &mat.up );
	RwV3dNormalize( &mat.at, &mat.at );
	g_MyEngine.bGetPtrFrmAxis()->bRender( &mat );

	//default
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);


	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)bZWrite );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *)bZTest );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)bAlpha );
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)nCullMode );
}
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL MyEngine::OnRegisterModule()
{
	REGISTER_MODULE( g_pAgcmResourceLoader	, AgcmResourceLoader	);
	REGISTER_MODULE( g_pcsApmMap			, ApmMap				);
	REGISTER_MODULE( g_pcsApmOcTree			, ApmOcTree				);
	//if (g_pcsApmOcTree)	g_pcsApmOcTree->m_bOcTreeEnable = TRUE;			// 다른 곳의 onAddModule에서 참고할수 있다..
	REGISTER_MODULE( g_pcsAgcmOcTree		, AgcmOcTree			);
	
	REGISTER_MODULE( g_pcsAgcmPostFX		, AgcmPostFX			)
	REGISTER_MODULE( g_pAgcmRender			, AgcmRender			);
	//REGISTER_MODULE( g_pAgcmDynamicLightmap	, AgcmDynamicLightmap	);
	REGISTER_MODULE( g_pAgcmSound			, AgcmSound				);
	REGISTER_MODULE( g_pAgcmUVAnimation		, AgcmUVAnimation		);
	REGISTER_MODULE( g_pAgcmEff2			, AgcmEff2				);
	REGISTER_MODULE( g_pAgcmFont			, AgcmFont				);

	

	g_pAgcmRender->SetUseLODBoundary( false );
	g_pAgcmRender->SetMainFrame( RwCameraGetFrame( m_pCamera ) );
	g_pAgcmRender->SetWorldCamera( m_pWorld, m_pCamera );
	g_pAgcmRender->SetLight( m_pLightAmbient, m_pLightDirect );

	g_pAgcmEff2->bSetGlobalVariable( NULL, NULL/*pFrmMainCharac*/, m_pWorld, m_pCamera );

	//AgcdEffGlobal::bGetInst().bFlagOn( E_GFLAG_USEPACK );
	AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_USEPACK );

	
	BOOL	bUsePathSearch = FALSE;
	AuPackingManager*	pPackingManager = 	AuPackingManager::MakeInstance();
	//pPackingManager->LoadReferenceFile(NULL);			// NULL이면 현재 디렉토리를 사용한다..	
	if( bUsePathSearch )
	{
		// 마고자 (2005-05-02 오후 3:34:36) : 
		// 디렉토리를 리컬시브하게 검사함.
		pPackingManager->LoadReferenceFile(NULL);			// NULL이면 현재 디렉토리를 사용한다..
	}
	else
	{
		// 마고자 (2005-05-02 오후 3:34:47) : 
		// 지정된 디렉토리의 레퍼런스만 읽어들임.

		// 현재 디렉토리 구어냄..
		char	strCurrentFolder[255];
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );

		char	strPath[ 1024 ];

		#define	SETDATADIRECTORY( x )										\
			sprintf( strPath , "%s\\%s" , strCurrentFolder , x );			\
			pPackingManager->LoadReferenceFile( strPath , true , false )

		// pPackingManager->LoadReferenceFile(NULL);

		SETDATADIRECTORY( "character"					);
		SETDATADIRECTORY( "character\\defaulthead\\face");
		SETDATADIRECTORY( "character\\defaulthead\\hair");
		SETDATADIRECTORY( "character\\animation"		);
		SETDATADIRECTORY( "effect\\animation"			);
		SETDATADIRECTORY( "effect\\clump"				);
		SETDATADIRECTORY( "object"						);
		SETDATADIRECTORY( "object\\animation"			);
		SETDATADIRECTORY( "sound\\ui"					);
		SETDATADIRECTORY( "texture\\character"			);
		SETDATADIRECTORY( "texture\\character\\low"		);
		SETDATADIRECTORY( "texture\\character\\medium"	);
		SETDATADIRECTORY( "texture\\effect"				);
		SETDATADIRECTORY( "texture\\effect\\low"		);
		SETDATADIRECTORY( "texture\\effect\\medium"		);
		SETDATADIRECTORY( "texture\\etc"				);
		SETDATADIRECTORY( "texture\\item"				);
		SETDATADIRECTORY( "texture\\minimap"			);
		SETDATADIRECTORY( "texture\\object"				);
		SETDATADIRECTORY( "texture\\object\\low"		);
		SETDATADIRECTORY( "texture\\object\\medium"		);
		SETDATADIRECTORY( "texture\\skill"				);
		SETDATADIRECTORY( "texture\\ui"					);
		SETDATADIRECTORY( "texture\\ui\\base"			);
		SETDATADIRECTORY( "texture\\world"				);
		SETDATADIRECTORY( "texture\\world\\low"			);
		SETDATADIRECTORY( "texture\\world\\medium"		);
		SETDATADIRECTORY( "world\\grass"				);
		SETDATADIRECTORY( "world\\octree"				);
		SETDATADIRECTORY( "world\\water"				);
	}


	pPackingManager->SetFilePointer();

//#ifdef _DEBUG
//	AgcdEffGlobal::bGetInst().bSetCBFrmRender( CBFrmRender );
//#endif


	
    RwImageSetPath(".\\Texture\\Effect\\;.\\Texture\\UI\\;.\\Texture\\Character\\;.\\Texture\\Object\\;.\\Texture\\World\\;.\\Texture\\UI\\Base\\;.\\Texture\\SKILL\\;.\\Texture\\Item\\;.\\Texture\\Etc\\;.\\Texture\\Minimap\\;.\\Texture\\AmbOccl\\");
	//RwImageSetPath( AgcuEffPath::bGetPath_Tex() );

	g_pAgcmFont->SetWorldCamera( m_pCamera );

	g_pcsApmMap->SetLoadingMode( FALSE, TRUE );
	g_pcsApmMap->SetAutoLoadData( FALSE );

	
	bInitAccessory();

	g_pAgcmEff2->bSetMainCharacterFrame( NULL , CGlobalVar::bGetInst().bGetPtrFrame(CGlobalVar::e_frm_blue) );

	g_pAgcmSound->m_bSampleEnable = TRUE;
	g_pAgcmSound->SetSampleVolume(1.f);
    
	g_pAgcmSound->m_bSilence3DSound	= FALSE;
	g_pAgcmSound->m_b3DSampleEnable = TRUE;
	g_pAgcmSound->m_bReadFromNotPacked = TRUE;
	g_pAgcmSound->Set3DSampleVolume(1.f);

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
void MyEngine::bOnOffEffFrame(BOOL OnOff)
{
#ifdef _DEBUG
	if( OnOff )
		AgcdEffGlobal::bGetInst().bSetCBFrmRender( CBFrmRender );
	else
		AgcdEffGlobal::bGetInst().bSetCBFrmRender( NULL );
#else
	OnOff;
#endif
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL MyEngine::OnInit()
{
	g_csAcuFrameMemory.Create();
	g_csAcuFrameMemory.InitManager(1048576);				// 10MB 할당

	RwReal	fDencity	= 0.f;
	RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void *)(rwFOGTYPEEXPONENTIAL));
	RwRenderStateSet(rwRENDERSTATEFOGCOLOR, (void *)(RWRGBALONG(255, 255, 255, 255)));
	RwRenderStateSet(rwRENDERSTATEFOGDENSITY, (void *)(&fDencity));

	
	return AgcEngine::OnInit();
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
extern "C"
{
extern void
_rwD3D9RenderStateFlushCache();
}

VOID MyEngine::OnWorldRender()
{

	vRenderAcessory();
	if (g_pAgcmRender)
	{

		if( CGlobalVar::bGetInst().bGetPtrEmiterClump() )
			g_pAgcmRender->OriginalClumpRender( CGlobalVar::bGetInst().bGetPtrEmiterClump() );
		g_pAgcmRender->Render();
	}
}


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::OnTerminate()
{
	bDestroyAcessory();

	g_csAcuFrameMemory.Release();

	// 20070515 강제 종료 코드
	//{
		if( g_pAgcmSound )
			g_pAgcmSound->OnDestroy();
		// 강제로 종료시켜버림..
		//::ExitProcess( 0 );
	//}

	//AgcEngine::OnTerminate();
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::OnEndIdle()
{
	g_csAcuFrameMemory.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
LPCSTR	SZHEIGHTMAP	= "HMap.BMP";
LPCSTR	SZHEIGHTMAPTEX	= "HMapTex.BMP";
VOID MyEngine::bInitAccessory()
{
	RwV2d	v2dTileSize	= { 100.f, 100.f };
	RwV3d	v3dCenter	= { 0.f, 0.f, 0.f };

	m_cGrid1.bBuildGrid( 100, 100, v2dTileSize, 0xff606060, v3dCenter );
	v2dTileSize.x	= 
	v2dTileSize.y	= 1000.f;
	m_cGrid10.bBuildGrid( 10, 10, v2dTileSize, 0xffa0a0a0, v3dCenter );

	m_cAxisWorld.bSetupSize( 5000.f+400.f, 4000.f, 400.f );

///	m_cAxisTest1.bSetupSize( 500.f+40.f, 40.f, 40.f );
//	m_cAxisTest1.bSetupColorPositive(0xffff0000,0xffff0000,0xffff0000);
//	m_cAxisTest1.bSetupColorNegative(0xffff0000,0xffff0000,0xffff0000);
//	m_cAxisTest1.bSetupSemi(TRUE);
//	
//	m_cAxisTest2.bSetupSize( 500.f+40.f, 40.f, 40.f );
//	m_cAxisTest2.bSetupColorPositive(0xffffff00,0xffffff00,0xffffff00);
//	m_cAxisTest2.bSetupColorNegative(0xffffff00,0xffffff00,0xffffff00);
//	m_cAxisTest2.bSetupSemi(TRUE);

	//m_cAxisWorld.bSetupShowCone( bAxisConeSwitch );
	m_cAxisWorld.bSetupColorNegative( 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF );

	m_cHeightMap.bInit( SZHEIGHTMAP );

	Eff2Ut_SAFE_DESTROY_TEX( m_pTexHeightMap );

	char	szTex[MAX_PATH]	= "";
	::GetCurrentDirectory( MAX_PATH, szTex );
	::strcat( szTex, "\\" );
	::strcat( szTex, SZHEIGHTMAPTEX );
	m_pTexHeightMap = RwTextureRead( szTex, NULL );

	{
		CGlobalVar::bGetInst().bOnEngineInitAccessory();

		m_cAxisFrm.bSetupSize( 100.f, 100.f, 10.f );
		m_cAxisFrm.bSetupSemi( TRUE );
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::bDestroyAcessory()
{
	CGlobalVar::bGetInst().bOnEngineDestroyAccessory();

	Eff2Ut_SAFE_DESTROY_TEX( m_pTexHeightMap );
	m_cHeightMap.bClear();

	Eff2Ut_SAFE_DESTROY_TEX( m_lpTexture );
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 MyEngine::vRenderAcessory()
{
	vRenderHMap();

	vRenderAxisGrid();

	AXISVIEW::RenderAxisContainer();

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 MyEngine::vRenderAxisGrid()
{
	
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
	RwBool	bZWrite	= FALSE,
			bZTest	= FALSE,
			bAlpha	= FALSE;
	RwInt32	nCullMode	= rwCULLMODECULLBACK;

	RwRenderStateGet( rwRENDERSTATEZWRITEENABLE, (void *)&bZWrite );
	RwRenderStateGet( rwRENDERSTATEZTESTENABLE, (void *)&bZTest );
	RwRenderStateGet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)&bAlpha );
	RwRenderStateGet( rwRENDERSTATECULLMODE, (void *)&nCullMode );

	
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)TRUE );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *)TRUE );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE );
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK );


	//default
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	{
		RwUInt32	flag	= rwIM3D_ALLOPAQUE;	
		if( CToolOption::bGetInst().bShowGrid() )
		{
			m_cGrid1.bRender(NULL, &flag);
			m_cGrid10.bRender(NULL, &flag);
		}

		if( CToolOption::bGetInst().bShowWorldAxis() )
		{
			m_cAxisWorld.bRender(NULL, &flag);
//			m_cAxisTest1.bRender(NULL, &flag);
//			m_cAxisTest2.bRender(NULL, &flag);
		}

		if( CToolOption::bGetInst().bShowCameraAxis() )
		{
			CCamera::bGetInst().bGetPtrAxis()->bRender(NULL, &flag);
		}
	}

	if( CToolOption::bGetInst().bShowFrame() )
	{
		CGlobalVar::bGetInst().bRenderShowFrm();
	}



	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)bZWrite );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *)bZTest );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)bAlpha );
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)nCullMode );


	return 0;
};


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
INT32 MyEngine::vRenderHMap()
{
	RwBool	bZWrite	= FALSE,
			bZTest	= FALSE,
			bAlpha	= FALSE,
			bLight	= FALSE;
	RwInt32	nCullMode	= rwCULLMODECULLBACK;

	RwRenderStateGet( rwRENDERSTATEZWRITEENABLE, (void *)&bZWrite );
	RwRenderStateGet( rwRENDERSTATEZTESTENABLE, (void *)&bZTest );
	RwRenderStateGet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)&bAlpha );
	RwRenderStateGet( rwRENDERSTATECULLMODE, (void *)&nCullMode );
	RwD3D9GetRenderState( D3DRS_LIGHTING, &bLight );

	
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)TRUE );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *)TRUE );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE );
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK );

	//default
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	RwMatrix	mat;
	RwMatrixSetIdentity( &mat );
	RwD3D9SetTransformWorld ( &mat );

	
	D3DMATERIAL9 material =
	{
		{0.5f, 0.5f, 0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f, 0.5f},
		0.5f,
	};

	RwD3D9SetMaterial( (LPVOID)(&material) );

	//tri_list
	if(CToolOption::bGetInst().bShowField())
	{
		RwD3D9SetRenderState( D3DRS_LIGHTING , TRUE );
		RwD3D9SetTexture ( m_pTexHeightMap, 0LU );
		m_cHeightMap.bRenderD3dS();
	}

	//line_list
	if( CToolOption::bGetInst().bShowFieldWire() )
	{
		RwD3D9SetRenderState( D3DRS_LIGHTING , FALSE );
		RwD3D9SetTexture ( NULL, 0LU );
		m_cHeightMap.bRenderD3dW();
	}

	RwD3D9SetRenderState( D3DRS_LIGHTING, bLight );
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)bZWrite );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *)bZTest );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *)bAlpha );
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)nCullMode );

	return 0;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL MyEngine::vPickPlane(RwV3d* pV3dPicked, const RwV3d& center, BOOL bXZ)
{
	const RwV2d	RECTSIZE = { 10000.f, 10000.f };


	RwV3d*	pV3dRect	= NULL;
	RwV3d	av3drect[4] = {
		{0.f,0.f,0.f,},
		{0.f,0.f,0.f,},
		{0.f,0.f,0.f,},
	};

	if( bXZ )//xz_plane
	{
		RwReal	frad = 
			fabs( acosf( RwV3dDotProduct( CCamera::bGetInst().bGetPtrY(), AXISWORLD::bGetInst().bGetPtrWorldY() ) ) );
		if( DEF_R2D(frad) < 5.f || DEF_R2D(frad) > 175.f )
			return FALSE;
		
		av3drect[0].x = center.x-RECTSIZE.x, av3drect[0].y = center.y, av3drect[0].z = center.z - RECTSIZE.y;
		av3drect[1].x = center.x+RECTSIZE.x, av3drect[1].y = center.y, av3drect[1].z = center.z - RECTSIZE.y;
		av3drect[2].x = center.x-RECTSIZE.x, av3drect[2].y = center.y, av3drect[2].z = center.z + RECTSIZE.y;
		av3drect[3].x = center.x+RECTSIZE.x, av3drect[3].y = center.y, av3drect[3].z = center.z + RECTSIZE.y;
	}
	else//xy_plane
	{
		RwReal	frad = 
			fabs( acosf( RwV3dDotProduct( CCamera::bGetInst().bGetPtrZ(), AXISWORLD::bGetInst().bGetPtrWorldY() ) ) );
		if( DEF_R2D(frad) < 5.f || DEF_R2D(frad) > 175.f )
			return FALSE;

		av3drect[0].x = -RECTSIZE.x, av3drect[0].y =  RECTSIZE.y, av3drect[0].z = 0.f;
		av3drect[1].x =  RECTSIZE.x, av3drect[1].y =  RECTSIZE.y, av3drect[1].z = 0.f;
		av3drect[2].x = -RECTSIZE.x, av3drect[2].y = -RECTSIZE.y, av3drect[2].z = 0.f;
		av3drect[3].x =  RECTSIZE.x, av3drect[3].y = -RECTSIZE.y, av3drect[3].z = 0.f;

		RwV3dTransformVectors ( av3drect, av3drect, 4LU, BMATRIX::bGetInst().bGetPtrMatBY() );
		RwV3dAdd( &av3drect[0], &av3drect[0], &center );
		RwV3dAdd( &av3drect[1], &av3drect[1], &center );
		RwV3dAdd( &av3drect[2], &av3drect[2], &center );
		RwV3dAdd( &av3drect[3], &av3drect[3], &center );
	}
	RwReal	ft	= 0.f,
			fu	= 0.f,
			fv	= 0.f;

	if(	CPickUser::GetInst().IntersectTriangle( CPickUser::GetInst().bGetPtrRay()
			, CCamera::bGetInst().bGetPtrV3dEyePos()
			, &av3drect[0]
			, &av3drect[1]
			, &av3drect[2]
			, &ft, &fu, &fv ) )
	{
		CPickUser::GetInst().bCalcPickedPoint( *pV3dPicked
			, av3drect[0]
			, av3drect[1]
			, av3drect[2]
			, fu, fv );

		return TRUE;
	}
	else if(	CPickUser::GetInst().IntersectTriangle( CPickUser::GetInst().bGetPtrRay()
			, CCamera::bGetInst().bGetPtrV3dEyePos()
			, &av3drect[1]
			, &av3drect[3]
			, &av3drect[2]
			, &ft, &fu, &fv ) )
	{
		CPickUser::GetInst().bCalcPickedPoint( *pV3dPicked
			, av3drect[1]
			, av3drect[3]
			, av3drect[2]
			, fu, fv );
		
		return TRUE;
	}
	
	return FALSE;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
LPSHOWFRM MyEngine::bPickShowFrm(POINT ptMouse)
{
	m_lpPickedShowFrm = CGlobalVar::bGetInst().bPickShowFrm(ptMouse);
	return m_lpPickedShowFrm;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::bMoveShowFrm(POINT ptMouse)
{
	if( !m_lpPickedShowFrm )
		return;
	
	CPickUser::GetInst().Make_Ray( NULL, NULL, CCamera::bGetInst().bGetCam(), ptMouse );

	RwV3d	v3dPicked	= {0.f,0.f,0.f};

	BOOL	bPick	= FALSE;
	switch(CToolOption::bGetInst().bGetLBtnAct())
	{
	case CToolOption::e_lbtn_movefrmOnVirtualPlane:
		{
			bPick = vPickPlane( &v3dPicked, *m_lpPickedShowFrm->bGetPtrPos(), !KEYDOWN(VK_SHIFT) );
		}
		break;
	case CToolOption::e_lbtn_movefrmOnField:
		{
			if( CToolOption::bGetInst().bShowFieldWire() || CToolOption::bGetInst().bShowField() )
			bPick = m_cHeightMap.bPick( &v3dPicked );
		}
		break;
	}
	if( bPick )//vPickPlane( &v3dPicked, *m_lpPickedShowFrm->bGetPtrPos(), !KEYDOWN(VK_SHIFT) ) )
	{
		m_lpPickedShowFrm->bMoveTo( v3dPicked );
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::bRotShowFrm(int dx, int dy)
{
	if( m_lpPickedShowFrm )
	{
		m_lpPickedShowFrm->bRot( dx*0.5f, -dy*0.5f );
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::bBackToOrgShowFrm(void)
{
	if( m_lpPickedShowFrm )
	{
		m_lpPickedShowFrm->bSetIdentity();
	}
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
VOID MyEngine::bSetFrmHeight(void)
{
	CGlobalVar::bGetInst().bSetShowFrmHeight( m_cHeightMap );
};


///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
#include "d3dx9tex.h"
#include "dxerr9.h"
VOID MyEngine::bLoadTex(LPCSTR fName, INT* pnWidth, INT* pnHeight, INT* pnDepth, LPCTSTR* szFmt, LPCTSTR szDir)
{
	if( !m_lpDev )
		return;
	

	Eff2Ut_SAFE_DESTROY_TEX( m_lpTexture );

	char	szTex[MAX_PATH] = "";
	char	szPath[MAX_PATH] = "";
	strcpy( szPath, szDir ? szDir : AgcuEffPath::bGetPath_Tex() );
	strcat( szTex, fName );

	m_lpTexture = Eff2Ut_FindEffTex( szTex, NULL, szPath, AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader() );


	if( m_lpTexture )
	{
		*pnWidth = RwRasterGetWidth( RwTextureGetRaster ( m_lpTexture ) );
		*pnHeight = RwRasterGetHeight( RwTextureGetRaster ( m_lpTexture ) );
		*pnDepth = RwRasterGetDepth( RwTextureGetRaster ( m_lpTexture ) );

		switch(RwRasterGetFormat( RwTextureGetRaster ( m_lpTexture ) ))
		{
		case rwRASTERFORMATDEFAULT			: *szFmt = "DEFAULT";			break;
		case rwRASTERFORMAT1555				: *szFmt = "1555";				break;
		case rwRASTERFORMAT565				: *szFmt = "565";				break;
		case rwRASTERFORMAT4444				: *szFmt = "4444";				break;
		case rwRASTERFORMATLUM8				: *szFmt = "LUM8";				break;
		case rwRASTERFORMAT8888				: *szFmt = "8888";				break;
		case rwRASTERFORMAT888				: *szFmt = "888";				break;
		case rwRASTERFORMAT16				: *szFmt = "16";				break;
		case rwRASTERFORMAT24				: *szFmt = "24";				break;
		case rwRASTERFORMAT32				: *szFmt = "32";				break;
		case rwRASTERFORMAT555				: *szFmt = "555";				break;
		case rwRASTERFORMATAUTOMIPMAP		: *szFmt = "AUTOMIPMAP";		break;
		case rwRASTERFORMATPAL8				: *szFmt = "PAL8";				break;
		case rwRASTERFORMATPAL4				: *szFmt = "PAL4";				break;
		case rwRASTERFORMATMIPMAP			: *szFmt = "MIPMAP";			break;
		case rwRASTERFORMATPIXELFORMATMASK	: *szFmt = "PIXELFORMATMASK";	break;
		case rwRASTERFORMATMASK				: *szFmt = "MASK";				break;
		case rwRASTERFORMATFORCEENUMSIZEINT	: *szFmt = "FORCEENUMSIZEINT";	break;
		default								: *szFmt = "unknown";			break;
		}
	}
};

HRESULT	MyEngine::bRenderTexPreview(HWND hWnd)
{
	if( !this->m_lpDev )
		return D3D_OK;

	
	if( SUCCEEDED( m_lpDev->BeginScene() ) )
	{
		HRESULT	hr	= D3D_OK;

		//DRAW RECT
		float fWidth = static_cast<float>(RwRasterGetWidth( RwCameraGetRaster( CCamera::bGetInst().bGetPtrRwCam() ) ));
		float fHeight = static_cast<float>(RwRasterGetHeight( RwCameraGetRaster( CCamera::bGetInst().bGetPtrRwCam() ) ));

		VTX_RHWT1	avtxRect[4];

		avtxRect[0].rhw = D3DXVECTOR4( 0.f,0.f,0.f,1.f			), avtxRect[0].uv = D3DXVECTOR2(0.f, 0.f);
		avtxRect[1].rhw = D3DXVECTOR4( 0.f,fHeight,0.f,1.f		), avtxRect[1].uv = D3DXVECTOR2(0.f, 1.f);
		avtxRect[2].rhw = D3DXVECTOR4( fWidth,0.f,0.f,1.f		), avtxRect[2].uv = D3DXVECTOR2(1.f, 0.f);
		avtxRect[3].rhw = D3DXVECTOR4( fWidth,fHeight,0.f,1.f	), avtxRect[3].uv = D3DXVECTOR2(1.f, 1.f);

		RwD3D9SetTexture ( m_lpTexture, 0LU );
		RwD3D9SetFVF (VTX_RHWT1::FVF);

		RwD3D9DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP
			, 2
			, avtxRect
			, VTX_RHWT1::SIZE
			);

		hr	= m_lpDev->EndScene();
		if( FAILED( hr ) )
		{
			//err
			ASSERT( !"EndScene failed" );
			return hr;
		}

		hr	= m_lpDev->Present( 0,0,hWnd,0 );
		if( FAILED( hr ) )
		{
			//err
			ASSERT( !"Present failed" );
			return hr;
		}
		RwD3D9SetTexture ( NULL, 0LU );
	}
	return E_FAIL;
};


HRESULT MyEngine::bRenderTexForUVSelect(HWND hWnd
							  , RwTexture* pTex
							  , float fWidth
							  , float fHeight
							  , INT nRow
							  , INT nCol
							  , INT nSelRow
							  , INT nSelCol)
{
	if( !m_lpDev	|| 
		!pTex		|| 
		!hWnd		||
		!nRow		||
		!nCol		||
		nRow<nSelRow ||
		nCol<nSelCol )
		return D3D_OK;
	
	RwRGBA BackgroundColor	= { 0, 0, 0, 255};
	RwCameraClear( m_pCamera, &BackgroundColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);
	if( SUCCEEDED( m_lpDev->BeginScene() ) )
	{
		HRESULT	hr	= D3D_OK;

		VTX_RHWT1	avtxRect[4];

		avtxRect[0].rhw = D3DXVECTOR4( 0.f,0.f,0.f,1.f			), avtxRect[0].uv = D3DXVECTOR2(0.f, 0.f);
		avtxRect[1].rhw = D3DXVECTOR4( 0.f,fHeight,0.f,1.f		), avtxRect[1].uv = D3DXVECTOR2(0.f, 1.f);
		avtxRect[2].rhw = D3DXVECTOR4( fWidth,0.f,0.f,1.f		), avtxRect[2].uv = D3DXVECTOR2(1.f, 0.f);
		avtxRect[3].rhw = D3DXVECTOR4( fWidth,fHeight,0.f,1.f	), avtxRect[3].uv = D3DXVECTOR2(1.f, 1.f);

		RwD3D9SetTexture ( pTex, 0LU );
		RwD3D9SetFVF (VTX_RHWT1::FVF);

		RwD3D9DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP
			, 2
			, avtxRect
			, VTX_RHWT1::SIZE
			);

		//line
		if( theApp.bGetRW().bGetD3dxLine() )
		{

			RwD3D9SetTexture ( NULL, 0LU );

			float	tw		= (float)RwRasterGetWidth( RwTextureGetRaster(pTex) );
			float	th		= (float)RwRasterGetHeight( RwTextureGetRaster(pTex) );

			float	xstep	= fWidth/((float)nCol);
			float	ystep	= fHeight/((float)nRow);

			theApp.bGetRW().bGetD3dxLine()->SetWidth( fWidth*2.f / tw );

			std::vector<D3DXVECTOR2>	line((nCol+1 + nRow+1)*2);
			//vertical line
			int i=0;
			float	x=0.f;
			for(i=0; i<nCol+1; ++i, x+=xstep){
				line[i*2+0]	= D3DXVECTOR2( x,0.f);
				line[i*2+1]	= D3DXVECTOR2( x,fHeight);
			}
			//horizontal line
			float	y=0.f;
			for(i=0; i<nRow+1; ++i, y+=ystep){
				line[(nCol+1+i)*2+0] = D3DXVECTOR2( 0.f,y);
				line[(nCol+1+i)*2+1] = D3DXVECTOR2( fWidth,y);
			}


			if( SUCCEEDED(theApp.bGetRW().bGetD3dxLine()->Begin()) )
			{
				for( i=0; i<(nCol+1 + nRow+1); ++i )
				{
					theApp.bGetRW().bGetD3dxLine()->Draw(&line[i*2], 2, 0xffffffff);					
				}
				float l = xstep*(nSelCol-1);
				float t = ystep*(nSelRow-1);
				float r = l+xstep;
				float b = t+ystep;
				line[0] = D3DXVECTOR2(l,t);
				line[1] = D3DXVECTOR2(r,t);
				line[2] = D3DXVECTOR2(r,b);
				line[3] = D3DXVECTOR2(l,b);
				line[4] = D3DXVECTOR2(l,t);

				theApp.bGetRW().bGetD3dxLine()->Draw(&line[0], 5, 0x80ff0000);
				theApp.bGetRW().bGetD3dxLine()->End();
			}
/*

			std::vector<VTX_RHWD>	line((nCol+1 + nRow+1)*2);

			//vertical line
			int i=0;
			float	x=0.f;
			for(i=0; i<nCol+1; ++i, x+=xstep)
			{
				line[i*2+0].diff = line[i*2+1].diff = 0xffffffff;
				line[i*2+0].rhw	= D3DXVECTOR4( x,0.f,0.f,1.f);
				line[i*2+1].rhw	= D3DXVECTOR4( x,fHeight,0.f,1.f);
			}
			//horizontal line
			float	y=0.f;
			for(i=0; i<nRow+1; ++i, y+=ystep)
			{
				line[(nCol+1+i)*2+0].diff = line[(nCol+1+i)*2+1].diff = 0xffffffff;
				line[(nCol+1+i)*2+0].rhw	= D3DXVECTOR4( 0.f,y,0.f,1.f);
				line[(nCol+1+i)*2+1].rhw	= D3DXVECTOR4( fWidth,y,0.f,1.f);
			}

			RwD3D9SetTexture ( NULL, 0LU );
			RwD3D9SetFVF (VTX_RHWD::FVF);

			RwD3D9DrawPrimitiveUP ( D3DPT_LINELIST
				, (nCol+1 + nRow+1)
				, &line[0]
				, VTX_RHWD::SIZE
				);

			float l = xstep*(nSelCol-1);
			float t = ystep*(nSelRow-1);
			float r = l+xstep;
			float b = t+ystep;

			//selected rect
			line[0].diff = line[1].diff = line[2].diff = line[3].diff = line[4].diff = 0xff000000;
			line[0].rhw = D3DXVECTOR4(l,t,0.f,1.f);
			line[1].rhw = D3DXVECTOR4(r,t,0.f,1.f);
			line[2].rhw = D3DXVECTOR4(r,b,0.f,1.f);
			line[3].rhw = D3DXVECTOR4(l,b,0.f,1.f);
			line[4].rhw = D3DXVECTOR4(l,t,0.f,1.f);
			
			RwD3D9DrawPrimitiveUP ( D3DPT_LINESTRIP
				, 4LU
				, &line[0]
				, VTX_RHWD::SIZE
				);

  */
		}
		

		hr	= m_lpDev->EndScene();
		if( FAILED( hr ) )
		{
			//err
			ASSERT( !"EndScene failed" );
			return hr;
		}

		hr	= m_lpDev->Present( 0,0,hWnd,0 );
		if( FAILED( hr ) )
		{
			//err
			ASSERT( !"Present failed" );
			return hr;
		}
		RwD3D9SetTexture ( NULL, 0LU );
	}
	return E_FAIL;
};

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
BOOL MyEngine::OnRenderPreCameraUpdate()
{
	return TRUE;
};

RwReal GetMapHeight(RwReal x, RwReal y, RwReal z)
{
	if( CToolOption::bGetInst().bShowField() || CToolOption::bGetInst().bShowFieldWire() )
		return g_MyEngine.m_cHeightMap.bGetHeight(x, z);
	else
		return y;

};