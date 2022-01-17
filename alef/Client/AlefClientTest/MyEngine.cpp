// MyEngine.cpp: implementation of the MyEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpusrdat.h"
#include "rpspline.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include "MyEngine.h"

#include "AcuRpDMaterial.h"
#include "AcuRpMtexture.h"
#include "AcuRpGlyph.h"
#include "AgcmEffect.h"

#ifdef USE_DPVS
#include "rpdpvs.h"
#endif //USE_DPVS

// Image Loader
#include "rtbmp.h"
#include "rtpng.h"

#include "src/MainWindow.h"

MainWindow			g_MainWindow	;
MyEngine			g_MyEngine		;

/*
void MyEngine::OnRender()
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
    RwCameraClear(m_pCamera, &m_rgbBackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);
	RwRaster	*pRaster	= RwCameraGetRaster( m_pCamera ); // 카메라 레스터 얻어냄.
	static BOOL FirstRender = TRUE;
	RwCullMode FaceCullMode = rwCULLMODECULLNONE;
	RwReal FogDensity = 0.00005f;
	static RwBool FogOn = FALSE;

	// 프리렌더링.
	// 먼저 풀 UI 부터 렌더링
	if( m_pCurrentFullUIModule ) m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_PRERENDER , ( PVOID ) pRaster );

	// 윈도우 UI모듈 렌더링
	// 마고자 (2002-04-22 오후 2:45:48) : 
	{
		AgcWindow	*pWindow;
		AuNode<AgcWindow *>	*pNode = m_ListCurrentWindowModule.GetTailNode();
		while( pNode )
		{
			pWindow = ( AgcWindow * ) pNode->GetData();

			// Render...
			pWindow->WindowControlMessageProc( AgcWindow::MESSAGE_PRERENDER , ( PVOID ) pRaster );


			m_ListCurrentWindowModule.GetPrev( pNode );
		}
		
	}
	if( RwCameraBeginUpdate( g_pEngine->m_pCamera ) )
	{
		if( FirstRender )
		{
			RwCameraSetFogDistance(m_pCamera, 30000.0f);
//			RwRenderStateSet(rwRENDERSTATECULLMODE, (void *) FaceCullMode);
			RwRenderStateSet(rwRENDERSTATEFOGCOLOR, (void *) RWRGBALONG(117, 162, 193, 255));
			RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void *) rwFOGTYPEEXPONENTIAL2);
			RwRenderStateSet(rwRENDERSTATEFOGDENSITY, (void *) &FogDensity);
			RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *) &FogOn);
			FirstRender = FALSE;
		}

#ifdef USE_DPVS
		RpDPVSWorldRender( g_pEngine->m_pWorld );
#else
		RpWorldRender( g_pEngine->m_pWorld );
#endif //USE_DPVS
		// 먼저 풀 UI 부터 렌더링
		if( m_pCurrentFullUIModule ) m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_RENDER , ( PVOID ) pRaster );

		// 윈도우 UI모듈 렌더링
		// 마고자 (2002-04-22 오후 2:45:48) : 
		{
			AgcWindow	*pWindow;
			AuNode<AgcWindow *>	*pNode = m_ListCurrentWindowModule.GetTailNode();
			while( pNode )
			{
				pWindow = ( AgcWindow * ) pNode->GetData();

				// Render...
				pWindow->WindowControlMessageProc( AgcWindow::MESSAGE_RENDER , ( PVOID ) pRaster );

				m_ListCurrentWindowModule.GetPrev( pNode );
			}
			
		}
		RwCameraEndUpdate( g_pEngine->m_pCamera );
	}

	// PostRendering
	// 먼저 풀 UI 부터 렌더링
	if( m_pCurrentFullUIModule ) m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_POSTRENDER , ( PVOID ) pRaster );

	// 윈도우 UI모듈 렌더링
	// 마고자 (2002-04-22 오후 2:45:48) : 
	{
		AgcWindow	*pWindow;
		AuNode<AgcWindow *>	*pNode = m_ListCurrentWindowModule.GetTailNode();
		while( pNode )
		{
			pWindow = ( AgcWindow * ) pNode->GetData();

			// Render...
			pWindow->WindowControlMessageProc( AgcWindow::MESSAGE_POSTRENDER , ( PVOID ) pRaster );

			m_ListCurrentWindowModule.GetPrev( pNode );
		}
		
	}

	DisplayOnScreenInfo();

//    if( RwCameraBeginUpdate(m_pCamera) )
//    {
//        if( m_clMenu->MenuGetStatus() != HELPMODE )
//        {
//        }
//
//        m_clMenu->MenuRender( m_pCamera , NULL );
//
//#ifdef RWMETRICS
//        RsMetricsRender();
//#endif
//
//        RwCameraEndUpdate( m_pCamera );
//    }

    RsCameraShowRaster( m_pCamera );	// Flip the Raster! - -+

    m_nFrameCounter++;
}
*/

MyEngine::MyEngine()
{
	m_nDimensionX = 0;
	m_nDimensionX = 0;
	m_fSectorSize = 0;

	ZeroMemory(m_astSectors, sizeof(AgcMapSector) * AGCMAP_MAX_SECTOR_DIMENSION * AGCMAP_MAX_SECTOR_DIMENSION);
}

MyEngine::~MyEngine()
{

}

static RwReal
aGetTime(void)
{
	return (RwReal)RsTimer()/1000.0f;
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

	if (!RpHAnimPluginAttach())
	{
		return FALSE;
	}

	if (!RpSkinPluginAttach())
	{
		return FALSE;
	}

    if (!RpCollisionPluginAttach() )
    {
        return FALSE;
    }

	if (!RpSplinePluginAttach() )
	{
		return FALSE;
	}

	if( !RpDMaterialPluginAttach() )
	{
		return FALSE;
	}

	if( !RpMTexturePluginAttach() )
	{
		return FALSE;
	}

	if( !RpGlyphPluginAttach() )
	{
		return FALSE;
	}

	if( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}

	if( !RtAnimInitialize() )
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

#ifdef USE_DPVS
    if( !RpDPVSPluginAttach() )
    {
        return FALSE;
    }

	RpDPVSOpen();
	RpDPVSSetGetTimeCallBack(aGetTime);
#endif //USE_DPVS

	return TRUE;
}

/*
static RwRaster* MipmapGenerationCB(RwRaster * raster, RwImage * image)
{
//	RwTextureRasterGenerateMipmaps(raster, image);

	return raster;
}

RwBool MapNameGenerationCB(RwChar *name, RwChar *maskName, RwUInt8 mipLevel, RwInt32 format)
{
	return TRUE;
}


static RpMaterial *
MaterialGetNumMipLevels(RpMaterial *material, void *data)
{
    RwTexture *texture;

    texture = RpMaterialGetTexture(material);

    *(RwInt32 *)data = RwRasterGetNumLevels(RwTextureGetRaster(texture));

    return NULL;
}
*/

RpWorld *	MyEngine::OnCreateWorld		() // 월드를 생성해서 리턴해준다.
{
	RwStream *stream = NULL;
	RpWorld *world = NULL;
	RwChar *path = NULL;
	//RwTexDictionary *texDict;
	RwChar	szOldPath[128];

	RwImageSetPath(".\\Texture\\");

	GetCurrentDirectory(AGCMCHAR_MAX_PATH, szOldPath);
	SetCurrentDirectory("world");

	/*
	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);
	RwTextureSetMipmapGenerationCallBack(MipmapGenerationCB);
	RwTextureSetMipmapNameCallBack(MapNameGenerationCB);
	*/

	/*
	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "world\\alef.txd");
	if (stream)
	{
		if (RwStreamFindChunk(stream, rwID_TEXDICTIONARY, (RwUInt32 *)NULL, (RwUInt32 *)NULL))
		{
			texDict = RwTexDictionaryStreamRead(stream);
		}
	}

	if (texDict)
		RwTexDictionarySetCurrent(texDict);

	path = RsPathnameCreate("alef.bsp");

	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
	if (stream)
	{
		if (RwStreamFindChunk(stream, rwID_WORLD, (RwUInt32 *)NULL, (RwUInt32 *)NULL))
		{
			world = RpWorldStreamRead(stream);

			RwStreamClose (stream, NULL);
		}
	}

	RsPathnameDestroy(path);

	SetCurrentDirectory(szOldPath);

	if (!RpCollisionWorldQueryData(world))
		RpCollisionWorldBuildData(world, NULL);
	*/
	RwBBox bbox;
	bbox.inf.x=bbox.inf.y=bbox.inf.z=-100000;
	bbox.sup.x=bbox.sup.y=bbox.sup.z=-100000;
	world = RpWorldCreate(&bbox);


	return world;
}

RwCamera *	MyEngine::OnCreateCamera	( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	m_fFarClipPlane = 30000.0f;
	m_fNearClipPlane = 100.0f;

	return CreateCamera( pWorld );
}

/*
RwCamera *	MyEngine::OnCreateCamera	( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	m_fFarClipPlane = 300000.0f;
	m_fNearClipPlane = 100.0f;
    RwCamera *camera;

    camera = m_clCamera->CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);

    if( camera )
    {
        RwCameraSetNearClipPlane	( camera , m_fNearClipPlane	);
        RwCameraSetFarClipPlane		( camera , m_fFarClipPlane	);

#ifdef USE_DPVS
        if (!RpDPVSWorldAddCamera( pWorld , camera ))
			return NULL;
#else
		if (!RpWorldAddCamera( pWorld, camera ))
			return NULL;
#endif //USE_DPVS

        return camera;
    }

    return NULL;

	return CreateCamera( pWorld );
}

BOOL		MyEngine::OnCreateLight		()	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
{
    m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);

    if (m_pLightAmbient)
    {
		RwRGBAReal color = {0.7f, 0.7f, 0.7f, 1.0f};
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
		RwFrameRotate( f , & xaxis , 70.0f , rwCOMBINEREPLACE );

		RwRGBAReal color = {1.0f, 1.0f, 1.0f, 1.0f};
		RpLightSetColor(m_pLightDirect, &color);

        RpWorldAddLight(m_pWorld, m_pLightDirect);
    }

	return TRUE;
}
*/

AgcWindow *	MyEngine::OnSetMainWindow	()	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.
{
	return &g_MainWindow;
}

BOOL MyEngine::OnRegisterModule()
{
	if (!m_csApmMap.		AddModule(this))
	{
		ASSERTE( !"m_csApmMap.AddModule(this)" );
		return FALSE;
	}
	
	if (!m_csAgcmEffect.	AddModule(this))
	{
		ASSERTE( !"m_csAgcmEffect.AddModule(this)" );
		return FALSE;
	}

	if (!m_csAgpmFactors.	AddModule(this))
	{
		ASSERTE( !"m_csAgpmFactors.AddModule(this)" );
		return FALSE;
	}

	if (!m_csApmObject.		AddModule(this))
	{
		ASSERTE( !"m_csApmObject.AddModule(this)" );
		return FALSE;
	}

	if (!m_csAgcmObject.	AddModule(this))
	{
		ASSERTE( !"m_csAgcmObject.AddModule(this)" );
		return FALSE;
	}

	if (!m_csAgcmMap.		AddModule(this))
	{
		ASSERTE( !"m_csAgcmMap.AddModule(this)" );
		return FALSE;
	}
	
	if (!m_csAgpmCharacter.	AddModule(this))
	{
		ASSERTE( !"m_csAgpmCharacter.AddModule(this)" );
		return FALSE;
	}

	if (!m_csAgcmCharacter.	AddModule(this))
	{
		ASSERTE( !"m_csAgcmCharacter.AddModule(this)" );
		return FALSE;
	}

	if (!m_csAgpmItem.		AddModule(this))
	{
		ASSERTE( !"m_csAgpmItem.AddModule(this)" );
		return FALSE;
	}

	if (!m_csAgcmItem.		AddModule(this))
	{
		ASSERTE( !"m_csAgcmItem.AddModule(this)" );
		return FALSE;
	}

	// module initialize
	m_csApmMap.		SetLoadingMode		( TRUE , FALSE	);
	m_csAgcmMap.	SetAutoLoadData		( FALSE			);//TRUE			);
	m_csAgcmMap.	InitMapToolLoadRange( 60 , 70 , 80 , 90 );

	char	strMapDataDirectory[ 1024 ];
	wsprintf( strMapDataDirectory , ".\\MAP\\Data"  );
	if( !m_csApmMap.Init( strMapDataDirectory ) )
		return FALSE;

	char	strClientMapDataDirectory[ 1024 ];
	GetCurrentDirectory( 1024 , strClientMapDataDirectory );
	m_csAgcmMap.		SetDataPath				( strClientMapDataDirectory );

	m_csAgcmMap.InitMapClientModule(m_pWorld);
	m_csAgcmMap.LoadAll();

	ApWorldSector * pSector;
	int		x2 = m_csAgcmMap.GetLoadRangeX2();
	int		z2 = m_csAgcmMap.GetLoadRangeZ2();
	for( int x = m_csAgcmMap.GetLoadRangeX1() ; x < x2 ; ++x  )
	{
		for( int z = m_csAgcmMap.GetLoadRangeZ1() ; z < z2 ; ++z )
		{
			pSector = m_csApmMap.GetSectorByArray( x , 0 , z );

			ASSERT( NULL != pSector );

			if( pSector )
			{
				m_csAgcmMap.LoadDWSector( pSector );

				m_csAgcmMap.SetCurrentDetail( pSector , SECTOR_EMPTY );
			}
		}
	}	

	m_csAgcmEffect.		SetClumpPath			( ".\\Effect\\Clump\\"		);
	m_csAgcmEffect.		SetAnimationPath		( ".\\Effect\\Animation\\"	);
	m_csAgcmEffect.		SetIniFilePath			( ".\\Effect\\Ini\\"		);
	m_csAgcmEffect.		SetTexturePath			( ".\\Effect\\Texture\\"	);
	m_csAgcmEffect.		SetNecessary			( m_pWorld, m_pCamera		);

	m_csApmObject.		SetMaxObjectTemplate	( 100			);
	m_csApmObject.		SetMaxObject			( 1000			);

	m_csAgcmObject.		SetClumpPath			( ".\\Object\\"	);
	m_csAgcmObject.		SetRpWorld				( m_pWorld		);

	m_csAgpmCharacter.	SetMaxCharacterTemplate	( 100	);
	m_csAgpmCharacter.	SetMaxCharacter			( 200	);

	m_csAgcmCharacter.	SetWorld				(m_pWorld, m_pCamera);
	m_csAgcmCharacter.	SetMaxAnimations		( 300	);

	m_csAgcmCharacter.	SetClumpPath			( ".\\Character\\"				);
	m_csAgcmCharacter.	SetAnimationPath		( ".\\Character\\Animation\\"	);
	m_csAgcmCharacter.	SetSendMoveDelay		( 500							);

	m_csAgpmItem.		SetMaxItem				( 1000	);
	m_csAgpmItem.		SetMaxItemTemplate		( 200	);

	m_csAgcmItem.		SetClumpPath			( ".\\Character\\"	);
	m_csAgcmItem.		SetWorld				( m_pWorld			);
	m_csAgcmItem.		SetMaxItemClump			( 500				);

	return TRUE;
}

/*
void MyEngine::OnTerminate()
{
	// 각윈도우 종료신호 보냄.
	AgcWindow	*pWindow;
	AuNode<AgcWindow *>	*pNode = m_ListCurrentWindowModule.GetHeadNode();
	while( pNode )
	{
		pWindow = ( AgcWindow * ) pNode->GetData();
		// 윈도우 찾음.
		pWindow->Close();
		m_ListCurrentWindowModule.GetNext( pNode );
	}
	
	if( m_pCurrentFullUIModule )
		m_pCurrentFullUIModule->Close();
	
#ifdef RWMETRICS
    RsMetricsClose();
#endif

	// 한글 제거.
	HanFontClose();

    m_clMenu->MenuClose();

    if( m_pCamera )
    {
#ifdef USE_DPVS
        RpDPVSWorldRemoveCamera(m_pWorld, m_pCamera);
#else
		RpWorldRemoveCamera(m_pWorld, m_pCamera);
#endif //USE_DPVS
        m_clCamera->CameraDestroy(m_pCamera);
    }

	// 모듈 디스트로이.
	ApModuleManager::Destroy();

	DestroyLights();

    if( m_pWorld )
    {
        RpWorldDestroy( m_pWorld );
    }

    if( m_pCharset )
    {
        RwRasterDestroy( m_pCharset );
    }

#ifdef USE_DPVS
	RpDPVSClose();
#endif //USE_DPVS

    RsRwTerminate();

	if ( m_clCamera )
	{
			delete m_clCamera;
			m_clCamera = NULL;
	}

	if ( m_clMenu )
	{
			delete m_clMenu;
			m_clMenu = NULL;
	}
}
*/

