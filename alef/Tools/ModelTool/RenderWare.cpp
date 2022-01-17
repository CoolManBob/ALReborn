#include "StdAfx.h"
#include "RenderWare.h"
#include "RWUtil.h"
#include "AgcEngine.h"
#include "platform.h"

CRenderWare::CRenderWare()
{
	m_bFullScreen         = FALSE;
	m_dwWidth             = 0;
	m_dwHeight            = 0;
	m_dwDepth             = 0;

	m_dwNumSubSystems     = 0;
	m_dwCurSysIndex       = 0;
	m_dwNumVideoModes     = 0;
	m_dwCurVideoModeIndex = 0;

	m_dwFramesPerSecond   = 0;
	m_fDeltaTime          = 0.0f;

	memset(m_sSubSysInfo,	0,	sizeof(RwSubSystemInfo)	* RWUTIL_MAX_SUBSYSTEMS);
	memset(m_sVideoModes,	0,	sizeof(RwVideoMode)		* RWUTIL_MAX_VIDEOMODE);

	m_Charset	= NULL;
	m_World		= NULL;
	m_Camera	= NULL;
	m_hWnd		= NULL;

	m_BackgroundColor.red   = 128;
	m_BackgroundColor.green = 128;
	m_BackgroundColor.blue  = 128;
	m_BackgroundColor.alpha = 255;
}

CRenderWare::~CRenderWare()
{
	Destroy();
}

RwBool CRenderWare::Initialize(HWND hWnd, RwUInt32 dwWidth, RwUInt32 dwHeight, RwUInt32 dwBpp, RwBool bFull)
{
	if( !hWnd )		return FALSE;

	m_hWnd        = hWnd;
	m_dwWidth     = dwWidth;
	m_dwHeight    = dwHeight;
	m_dwDepth     = dwBpp;
	m_bFullScreen = bFull;

	RwEngineOpenParams  openParams;
	openParams.displayID = hWnd;

	if( !RwEngineInit(NULL, NULL, 0, RWUTIL_RESOURCEDEFAULTARENASIZE) )		return FALSE;

	InitializeDebugMessage();

	if( !AttachPlugIn() )								return FALSE;
	if( !RwEngineOpen(&openParams) )					return FALSE;
	if( !SetDevice(RWUTIL_SUBSYSTEM_DEFAULT) )			return FALSE;
	if( !RwEngineStart() )								return FALSE;
    if( !RtFSManagerOpen( RTFSMAN_UNLIMITED_NUM_FS ) )	return FALSE;
	if( !psInstallFileSystem() )						return FALSE;
	if( !CreateObject() )								return FALSE;
	if( !RegisterImageLoader() )						return FALSE;

	return TRUE;
}

VOID CRenderWare::Destroy()
{
	RtFSManagerUnregister( RtFSManagerGetFileSystemFromName("unc") );
	RtFSManagerClose();

	DeleteObject();

	RwEngineStop();
	RwEngineClose();
	RwEngineTerm();
}

RwBool CRenderWare::BegineScene(RwBool bIsClear)
{
	if( bIsClear )
		RwCameraClear(m_Camera, &m_BackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);
		              
	return RwCameraBeginUpdate( m_Camera ) ? TRUE : FALSE;
}

void CRenderWare::EndScene()
{
	if( RwCameraEndUpdate(m_Camera) )
		RwCameraShowRaster( m_Camera, m_hWnd, 0 );
}

VOID CRenderWare::UpdateFPS()
{
    static RwBool firstCall = TRUE;
    static RwUInt32 lastFrameTime, lastAnimTime;
	static RwUInt32 dwFrameCounter = 0;

	RwUInt32 thisTime = (RwInt32)(timeGetTime());

    if(firstCall)
    {
        lastFrameTime = lastAnimTime = thisTime;
        firstCall = FALSE;
    }

    if(thisTime > (lastFrameTime + 1000))
    {
        m_dwFramesPerSecond = dwFrameCounter;
        dwFrameCounter = 0;
        lastFrameTime = thisTime;
    }

	m_fDeltaTime = (thisTime - lastAnimTime) * 0.001f;
	
	lastAnimTime = thisTime;

	++dwFrameCounter;
}	

VOID CRenderWare::ShowFPS()
{
	if( !m_Charset )	return;

	char szText[512];
	sprintf( szText, RWSTRING("FPS: %03d"), m_dwFramesPerSecond );
	RwUtilPrintText( m_Charset, szText, 0, 0, rsPRINTPOSTOPRIGHT );

	GetCurrentDirectory( 512, szText );
	RwUtilPrintText( m_Charset, szText, 0, 1, rsPRINTPOSTOPRIGHT );
}

VOID CRenderWare::SetCameraSize(RwRect *rect, RwReal viewWindow, RwReal aspectRatio)
{
	if( !m_Camera )		return;

	//Raster Setting
	RwRaster* pRaster = RwCameraGetRaster( m_Camera );
	if( pRaster )
		RwRasterDestroy( pRaster );

	pRaster = RwRasterCreate( rect->w, rect->h, 0, rwRASTERTYPECAMERA );
	_ASSERTE(pRaster);

	RwCameraSetRaster( m_Camera, pRaster );

	//ZRaster Setting
	pRaster = RwCameraGetZRaster( m_Camera );
	if( pRaster )
		RwRasterDestroy( pRaster );

	pRaster = RwRasterCreate( rect->w, rect->h, 0, rwRASTERTYPEZBUFFER );
	_ASSERTE( pRaster );

	RwCameraSetZRaster( m_Camera, pRaster );

	//View Window Setting
	const float viewWindowWidth = 0.7f;
	RwV2d vViewWindow = { viewWindowWidth, viewWindowWidth * ((RwReal)rect->h/(RwReal)rect->w) };
	RwCameraSetViewWindow( m_Camera, &vViewWindow );
}

VOID CRenderWare::SetClearColor(RwUInt8 red, RwUInt8 green, RwUInt8 blue, RwUInt8 alpha)
{
	m_BackgroundColor.red   = red;
	m_BackgroundColor.green = green;
	m_BackgroundColor.blue  = blue;
	m_BackgroundColor.alpha = alpha;
}


VOID CRenderWare::InitializeDebugMessage()
{
	RwDebugSetHandler( RwUtilDebugMessageHandler );
	RwUtilDebugMessage("Debugging Initialized", "Debugging Initialized");
}

VOID CRenderWare::AddVideoModes()
{
	m_dwNumVideoModes = RwEngineGetNumVideoModes();

	for( RwUInt32 i = 0; i < m_dwNumVideoModes; ++i )
		RwEngineGetVideoModeInfo(&m_sVideoModes[i], i);

	m_dwCurVideoModeIndex = (RwUInt32)(RwEngineGetCurrentVideoMode());
}

RwBool CRenderWare::AttachPlugIn()
{
	if( !RpWorldPluginAttach() )			return FALSE;
	if( !RpHAnimPluginAttach() )			return FALSE;
	if( !RpSkinPluginAttach() )				return FALSE;
	if( !RpUserDataPluginAttach() )			return FALSE;
	if( !RpCollisionPluginAttach() )		return FALSE;
	if( !RtAnimInitialize() )				return FALSE;
	if( !RpSplinePluginAttach() )			return FALSE;
	if( !RpMTexturePluginAttach() )			return FALSE;
	if( !RpMorphPluginAttach() )			return FALSE;
	if( !RpLODAtomicPluginAttach() )		return FALSE;
	if( !RpMatFXPluginAttach() )			return FALSE;
	if( !RpUVAnimPluginAttach() )			return FALSE;
	if( !RpUVAnimDataPluginAttach() )		return FALSE;
	if( !RpMaterialD3DFxPluginAttach() )	return FALSE;

	return TRUE;
}

RwBool CRenderWare::SetDevice(RwUInt32 dwDeviceIndex)
{
    
    RwInt32             AutoRenderer = 0;
    
#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
    RwInt32             ClocksStart_msb;
    RwUInt32            ClocksStart_lsb;
#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */


    RwInt32 numSubSystems = RwEngineGetNumSubSystems();
    if ( !numSubSystems )	return FALSE;

    /* Just to be sure ... */
    numSubSystems = numSubSystems > RWUTIL_MAX_SUBSYSTEMS ? RWUTIL_MAX_SUBSYSTEMS : numSubSystems;

    /* Get the names of all the sub systems */
	RwSubSystemInfo subSysInfo[RWUTIL_MAX_SUBSYSTEMS];
    for ( RwInt32 subSysNum = 0; subSysNum < numSubSystems; subSysNum++ )
        RwEngineGetSubSystemInfo( &subSysInfo[subSysNum], subSysNum );
    
#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)

    RWGETWINREGDWORD(AutoRenderer, _T("AutoRenderer"));
    RWGETWINREGDWORD(DisplayClocks, _T("DisplayClocks"));

    /* *INDENT-OFF* */
    _asm
    {
        ;              /* pre-load memory variables into data cache */
        mov            edx, ClocksStart_msb;
        mov            eax, ClocksStart_lsb;

        ;               /*RDTSC - get beginning timestamp to edx:eax */
        _emit 0x0F;
        _emit 0x31;
        ;              /* save beginning timestamp (1 cycle) */
        mov             ClocksStart_msb, edx;
        mov             ClocksStart_lsb, eax;
    }
    /* *INDENT-ON* */

	struct WinInt64
	{
		RwInt32             msb;
		RwUInt32            lsb;
	};
	WinInt64     ClocksStart = { 0, 0 };
    ClocksStart.msb = ClocksStart_msb;
    ClocksStart.lsb = ClocksStart_lsb;

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

	/* Get the default selection */
    RwInt32 curSel = RwEngineGetCurrentSubSystem();

    /* Set the driver to use the correct sub system */
    if( !RwEngineSetSubSystem( curSel ) )		return FALSE;

    /* Set up the video mode and set the apps window dimensions to match */
	RwInt32 curSelVM = 0;
    if( !RwEngineSetVideoMode(curSelVM) )		return FALSE;
        
	RwVideoMode		cVideoMode;
    RwEngineGetVideoModeInfo( &cVideoMode, RwEngineGetCurrentVideoMode() );

    RECT rect;
    GetClientRect(m_hWnd, &rect);

    RsGlobal.maximumWidth = rect.right;
    RsGlobal.maximumHeight = rect.bottom;

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
    RWGETWINREGDWORD(FrameCount, _T("FrameCount"));
#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

	return TRUE;
}

RwBool CRenderWare::SetSubSystem(RwUInt32 index)
{
	return RwEngineSetSubSystem((RwInt32)( m_dwCurSysIndex = index ));
}

RwBool CRenderWare::SetVideoMode(RwUInt32 index)
{
	return RwEngineSetVideoMode((RwInt32)(m_dwCurVideoModeIndex = index));
}

RwUInt32 CRenderWare::FindVideoModeIndex(RwUInt32 dwWidth, RwUInt32 dwHeight, RwUInt32 dwDepth, RwBool bFullScreen)
{
	if( !bFullScreen )		return 0;

	for( RwUInt32 dw = 0; dw < m_dwNumVideoModes; ++dw )
	{
		if( m_sVideoModes[dw].flags & rwVIDEOMODEEXCLUSIVE &&
			m_sVideoModes[dw].width == (RwInt32)(dwWidth) &&
			m_sVideoModes[dw].height == (RwInt32)(dwHeight) &&
			m_sVideoModes[dw].depth == (RwInt32)(dwDepth) )
			return dw;
	}

	return -1;
}

RwBool CRenderWare::RegisterImageLoader()
{
	if( !RwImageRegisterImageFormat(RWSTRING("bmp"), RtBMPImageRead, 0) )	return FALSE;
    if( !RwImageRegisterImageFormat(RWSTRING("png"), RtPNGImageRead, 0) )	return FALSE;
	if( !RwImageRegisterImageFormat(RWSTRING("tif"), RtTIFFImageRead, 0) )	return FALSE;

	return TRUE;
}

RwBool CRenderWare::CreateObject()
{
	if( !RtCharsetOpen() )		return FALSE;

	RwRGBA     ForegroundColor	= {255,   255,  255,  255};
	RwRGBA     BackgroundColor	= {128,   128,  128,  255};
	RwRGBAReal colorAmbient		= {(80.0f/255.0f), (80.0f/255.0f), (80.0f/255.0f), 1.0f};
	RwRGBAReal colorDirect		= {(119.0f/255.0f), (112.0f/255.0f), (93.0f/255.0f), 1.0f}; 
								//{0.4f,  0.4f, 0.4f, 1.0f};
								//{(128.0f/255.0f), (128.0f/255.0f), (128.0f/255.0f), 1.0f};
								//{(255.0f/255.0f), (239.0f/255.0f), (202.0f/255.0f), 1.0f};

	m_Charset	= RtCharsetCreate( &ForegroundColor, &BackgroundColor );
	m_World		= RwUtilCreateEmptyWorld();

	m_Ambient	= RwUtilCreateAmbientLight( colorAmbient, m_World );
	m_Direction	= RwUtilCreateMainLight( colorDirect, m_World );

	m_mapLight.insert( make_pair( "Ambient", new CRpLight( m_Ambient ) ) );
	m_mapLight.insert( make_pair( "Direction", new CRpLight( m_Direction ) ) );

	m_Camera	= CreateCamera();

	if( !m_Charset || !m_World || !m_Ambient || !m_Direction || !m_Camera )
		return FALSE;

	return TRUE;
}

VOID CRenderWare::DeleteObject()
{
	if( m_Charset )
    {
        RwRasterDestroy( m_Charset );
		RtCharsetClose();
    }

	if( m_Camera )
	{
		RpWorldRemoveCamera( m_World, m_Camera );
        CameraDestroy( m_Camera );
	}

	//RpWorldForAllLights(m_World, RwUtilDestroyLightCB, (void *)(m_World));

	for( RpLightMapItr Itr = m_mapLight.begin(); Itr != m_mapLight.end(); ++Itr )
		if( (*Itr).second )
			delete (*Itr).second;
	m_mapLight.clear();

	if( m_World )
        RpWorldDestroy( m_World );
}


RwCamera* CRenderWare::CreateCamera()
{
	g_pEngine->m_pCamera = g_pEngine->OnCreateCamera(m_World);
	RwCameraSetFarClipPlane( g_pEngine->m_pCamera, 100000.0f );

	return g_pEngine->m_pCamera;

	//g_pEngine->m_pCamera = RwCameraCreate();
	//if( !g_pEngine->m_pCamera )	return NULL;

	//RwCameraSetFrame( g_pEngine->m_pCamera, RwFrameCreate() );

 //   RwFrame *cameraFrame;
 //   RwV3d pos = {0.0f, 0.0f, 200.0f};

 //   cameraFrame = RwCameraGetFrame( g_pEngine->m_pCamera );
	//if( cameraFrame )
	//{
	//	RwFrameTranslate(cameraFrame, &pos, rwCOMBINEREPLACE);
	//	RwFrameRotate(cameraFrame, &RWUTIL_YAXIS, 180.0f, rwCOMBINEPRECONCAT);
	//}

 //   RwCameraSetNearClipPlane( g_pEngine->m_pCamera, 10.0f );
 //   RwCameraSetFarClipPlane( g_pEngine->m_pCamera, 80000.0f );

	//RwV2d	stViewWindow = { 0.5f, 0.5f };
	//RwCameraSetViewWindow( g_pEngine->m_pCamera, &stViewWindow );

 //   RpWorldAddCamera( World, g_pEngine->m_pCamera );

 //   return g_pEngine->m_pCamera;
}


VOID CRenderWare::CameraDestroy(RwCamera* pCamera)
{
	if( !pCamera )		return;

    RwFrame* frame = RwCameraGetFrame(pCamera);
    if( frame )
    {
        RwCameraSetFrame( pCamera, NULL );
        RwFrameDestroy( frame );
    }

    RwRaster* raster = RwCameraGetRaster(pCamera);
    if(raster)
    {
        RwRasterDestroy(raster);
        RwCameraSetRaster(pCamera, NULL);
    }

    raster = RwCameraGetZRaster(pCamera);
    if(raster)
    {
        RwRasterDestroy(raster);
        RwCameraSetZRaster(pCamera, NULL);
    }

    RwCameraDestroy(pCamera);
}

