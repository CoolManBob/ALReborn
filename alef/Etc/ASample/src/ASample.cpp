
/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 2001 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * main.c
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: 
 * Reviewed by: 
 *
 * Purpose: Starting point for any new RW demo using the demo skeleton.
 *
 ****************************************************************************/

#include "headers.h"

#include "alworld.h"
#include "alcamera.h"
#include "alclump.h"
#include "alcharacter.h"
#include "allight.h"

#include "alshadow.h"

#include "rppatch.h"

#ifdef RWMETRICS
//#include "metrics.h"
#endif

#define DEFAULT_SCREEN_WIDTH (640)
#define DEFAULT_SCREEN_HEIGHT (480)

#ifdef WIDE_SCREEN
#define DEFAULT_ASPECTRATIO (16.0f/9.0f)
#else
#define DEFAULT_ASPECTRATIO (4.0f/3.0f)
#endif

#define DEFAULT_VIEWWINDOW (0.5f)

static RwBool FPSOn = TRUE;
static RwBool DPVSOn = TRUE;
static RwBool DetailOn = FALSE;
static RwBool FogOn = FALSE;

static RwInt32 FrameCounter = 0;
static RwInt32 FramesPerSecond = 0;

static RwRGBA ForegroundColor = {200, 200, 200, 255};
static RwRGBA BackgroundColor = { 64,  64,  64,   0};

extern RwBool ShadowEnabled;

RpWorld *World = NULL;
RwCamera *Camera = NULL;
RtCharset *Charset = NULL;

RwBool FirstRender = TRUE;
RwCullMode FaceCullMode = rwCULLMODECULLNONE;

extern RwChar *part_name;
extern RwInt32 NodeID;
extern RwInt32 CurrentPart;
extern RwV3d *PlayerPosition;

AcuCamera	*g_clCamera = NULL;
AcuMenu		*g_clMenu = NULL;

RpClump *DetailMap = NULL;
RpAtomic *DetailMapAtomic = NULL;

RwInt32 WorldIndex = 1;

RwReal FogDensity = 0.00005f;


/*
 *****************************************************************************
 */
static RwReal
aGetTime(void)
{
	return (RwReal)RsTimer()/1000.0f;
}

/*
 *****************************************************************************
 */
RwBool DPVSRunningCallBack(RwBool testEnable)
{
	RwFrame* frame = NULL;

	if( testEnable )
	{
		return TRUE;
	}

#ifdef _USE_DPVS_
	RpDPVSSetCulling(Camera, DPVSOn);
#endif _USE_DPVS_

	frame = RwCameraGetFrame(Camera);

	if( frame )
	{
		RwFrameUpdateObjects(frame);
	}


	return TRUE;
}

/*
 *****************************************************************************
 */
RwBool FogCallBack(RwBool testEnable)
{
	if( testEnable )
	{
		return TRUE;
	}

	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *) FogOn);

	return TRUE;
}

/*
 *****************************************************************************
 */
RwBool DetailCallBack(RwBool testEnable)
{
	RwFrame* frame = NULL;

	if( testEnable )
	{
		return TRUE;
	}

	if (DetailOn)
	{
		DetailMap = AlClumpLoad("models/detail.dff");
		if (!DetailMap)
			return FALSE;

		RpWorldAddClump(World, DetailMap);

		DetailMapAtomic = AlClumpGetFirstAtomic(DetailMap);
//		RpCollisionGeometryBuildData(RpAtomicGetGeometry(DetailMapAtomic), NULL);
	}
	else
	{
		RpWorldRemoveClump(World, DetailMap);

		RpClumpDestroy(DetailMap);

		DetailMap = NULL;
		DetailMapAtomic = NULL;
	}

	return TRUE;
}

/*
 *****************************************************************************
 */
static RwBool 
Initialize(void)
{
	g_clCamera = new AcuCamera();
	g_clMenu = new AcuMenu();

	memset(Characters, 0, sizeof(stCharacter) * MAX_CHARACTER_NUMBER);
	memset(&Armor, 0, sizeof(stArmor));
	
	if( RsInitialize() )
	{
		if( !RsGlobal.maximumWidth )
		{
			RsGlobal.maximumWidth = DEFAULT_SCREEN_WIDTH;
		}

		if( !RsGlobal.maximumHeight )
		{
			RsGlobal.maximumHeight = DEFAULT_SCREEN_HEIGHT;
		}

		RsGlobal.appName = RWSTRING("ASample");

		RsGlobal.maxFPS = 120;

		return TRUE;
	}

	return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool 
InitializeMenu(void)
{	
	static RwChar fpsLabel[] = RWSTRING("FPS_F");
	static RwChar dpvsLabel[] = RWSTRING("DPVS_D");
	static RwChar detailLabel[] = RWSTRING("Detail_M");
	static RwChar fogLabel[] = RWSTRING("Fog_O");

	if( g_clMenu->MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
	{
		g_clMenu->MenuAddEntryBool(fpsLabel, &FPSOn, NULL);
		g_clMenu->MenuAddEntryBool(dpvsLabel, &DPVSOn, DPVSRunningCallBack);
		g_clMenu->MenuAddEntryBool(detailLabel, &DetailOn, DetailCallBack);
		g_clMenu->MenuAddEntryBool(fogLabel, &FogOn, FogCallBack);

		return TRUE;
	}

	return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool
InitializeWorld(RwChar *path)
{
	/*
	 * Create an empty world if not loading a BSP...
	 */
	World = AlWorldCreate(path);
	if( World == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create world."));

		return FALSE;
	}

	/*
	 * Create Lights...
	 */
	AmbientLight = AlLightCreateAmbient(World);
	if( AmbientLight == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create ambient light."));

		return FALSE;
	}

	MainLight = AlLightCreateMain(World);
	if( MainLight == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create main light."));

		return FALSE;
	}

	/*
	 * Create a camera using the democom way...
	 */
	Camera = AlCameraCreate(World);
	if( Camera == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create camera."));

		return FALSE;
	}

	if( !AlCharacterTemplateCreate() )
	{
		RsErrorMessage(RWSTRING("Error create character template."));

		return FALSE;
	}

	if( !AlCharacterCreate(World) )
	{
		RsErrorMessage(RWSTRING("Error create self character."));

		return FALSE;
	}

	if( !alCameraSetParentFrame(Camera, PlayerFrame) )
	{
		RsErrorMessage(RWSTRING("Error set camera parent frame."));

		return FALSE;
	}

	if( !SetupShadow() )
	{
		RsErrorMessage(RWSTRING("Error setup shadow."));

		return FALSE;
	}

#ifdef RWMETRICS
	RsMetricsOpen(Camera);
#endif

	FirstRender = TRUE;

	return TRUE;
}

/*
 *****************************************************************************
 */
static RwBool 
Initialize3D(void *param)
{
	if( !RsRwInitialize(param) )
	{
		RsErrorMessage(RWSTRING("Error initializing RenderWare."));

		return FALSE;
	}

#ifdef _USE_DPVS_
	RpDPVSOpen();

	RpDPVSSetGetTimeCallBack(aGetTime);
#endif _USE_DPVS_

	Charset = RtCharsetCreate(&ForegroundColor, &BackgroundColor);
	if( Charset == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create raster charset."));
	
		return FALSE;
	}

	if( !InitializeMenu() )
	{
		RsErrorMessage(RWSTRING("Error initializing menu."));

		return FALSE;
	}

	InitializeWorld("models/ground1.bsp");

	return TRUE;
}


/*
 *****************************************************************************
 */
static void 
TerminateWorld(void)
{
#ifdef RWMETRICS
	RsMetricsClose();
#endif

	if (DetailMap)
	{
		RpWorldRemoveClump(World, DetailMap);
		RpClumpDestroy(DetailMap);

		DetailMap = NULL;
	}

	DestroyShadow();

	AlCharacterDestroy(World);
	AlCharacterTemplateDestroy();

	if( Camera )
	{
#ifdef _USE_DPVS_
		RpDPVSWorldRemoveCamera(World, Camera);
#else
		RpWorldRemoveCamera(World, Camera);
#endif _USE_DPVS_

		/*
		 * This assumes the camera was created the democom way...
		 */
		g_clCamera->CameraDestroy(Camera);
	}

	if( MainLight )
	{
		RwFrame *frame = (RwFrame *)NULL;

		RpWorldRemoveLight(World, MainLight);

		frame = RpLightGetFrame(MainLight);
		if( frame )
		{
			RpLightSetFrame(MainLight, (RwFrame *)NULL);

			RwFrameDestroy(frame);
		}

		RpLightDestroy(MainLight);
	}

	if( AmbientLight )
	{
		RpWorldRemoveLight(World, AmbientLight);

		RpLightDestroy(AmbientLight);
	}

	if( World )
	{
		RpWorldDestroy(World);
	}

	return;
}

/*
 *****************************************************************************
 */
static void 
Terminate3D(void)
{
#ifdef RWMETRICS
	RsMetricsClose();
#endif

	g_clMenu->MenuClose();

	TerminateWorld();

	if( Charset )
	{
		RwRasterDestroy(Charset);
	}

#ifdef _USE_DPVS_
	RpDPVSClose();
#endif _USE_DPVS_

	RsRwTerminate();

	if ( g_clCamera )
	{
			delete g_clCamera;
			g_clCamera = NULL;
	}

	if ( g_clMenu )
	{
			delete g_clMenu;
			g_clMenu = NULL;
	}

	return;
}


/*
 *****************************************************************************
 */
static RwBool 
AttachPlugins(void)
{
	/* 
	 * Attach world plug-in...
	 */
	if( !RpWorldPluginAttach() )
	{
		return FALSE;
	}

	/* 
	 * Attach collision plug-in...
	 */
	if( !RpCollisionPluginAttach() )
	{
		return FALSE;
	}

	/* 
	 * Attach hierarchical animation plug-in...
	 */
	if( !RpHAnimPluginAttach() )
	{
		return FALSE;
	}

	/*
	 * Attach skin plug-in... 
	 */
	if( !RpSkinPluginAttach() )
	{
		return FALSE;
	}

#ifdef _USE_DPVS_
	/*
	 * Attach dPVS plug-in...
	 */
	if ( !RpDPVSPluginAttach() )
	{
		return FALSE;
	}
#endif _USE_DPVS_

	/*
	 * Attach User Data plug-in...
	 */
	if ( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}

	RtAnimInitialize();

#ifdef RWLOGO
	/* 
	 * Attach logo plug-in...
	 */
	if( !RpLogoPluginAttach() )
	{
		return FALSE;
	}
#endif

	return TRUE;
}


/*
 *****************************************************************************
 */
static void 
DisplayOnScreenInfo(RwCamera *camera)
{
	RwChar caption[2560];

	if( FPSOn )
	{
		rwsprintf(caption, RWSTRING("FPS: %03d"), FramesPerSecond);
		RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSTOPRIGHT);
	}

	if (part_name)
	{
		rwsprintf(caption, RWSTRING("Part: %s=%d"), part_name, NodeID);
		RsCharsetPrint(Charset, caption, 0, 20, rsPRINTPOSTOPRIGHT);
	}

	if (PlayerPosition)
	{
		rwsprintf(caption, RWSTRING("POS: %f,%f,%f"), PlayerPosition->x, PlayerPosition->y, PlayerPosition->z);
		RsCharsetPrint(Charset, caption, 0, 10, rsPRINTPOSTOPRIGHT);
	}

	rwsprintf(caption, RWSTRING("Anim: %d"), Characters[CurrentPlayer].nCurrentAnim);
	RsCharsetPrint(Charset, caption, 0, 1, rsPRINTPOSTOPRIGHT);

	return;
}


/*
 *****************************************************************************
 */
static void 
Render(void)
{
	RwCameraClear(Camera, &BackgroundColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

	if( RwCameraBeginUpdate(Camera) )
	{
		if( FirstRender )
		{

			RwRenderStateSet(rwRENDERSTATECULLMODE, (void *) FaceCullMode);
			RwRenderStateSet(rwRENDERSTATEFOGCOLOR, (void *) RWRGBALONG(255, 255, 255, 255));
			RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void *) rwFOGTYPEEXPONENTIAL2);
			RwRenderStateSet(rwRENDERSTATEFOGDENSITY, (void *) &FogDensity);
			FirstRender = FALSE;
		}

		RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *) FogOn);

		if( g_clMenu->MenuGetStatus() != HELPMODE )
		{
#ifdef _USE_DPVS_
			RpDPVSWorldRender(World);
			AlCharacterRender(World);
#else
			RpWorldRender(World);
#endif _USE_DPVS_

			if( ShadowEnabled )
				RenderShadow();

			DisplayOnScreenInfo(Camera);
		}

		g_clMenu->MenuRender(Camera, NULL);

#ifdef RWMETRICS
		RsMetricsRender();
#endif

		RwCameraEndUpdate(Camera);
	}

	/* 
	 * Display camera's raster...
	 */
	RsCameraShowRaster(Camera);

	FrameCounter++;

	return;
}


/*
 *****************************************************************************
 */
static void 
Idle(void)
{
	RwUInt32 thisTime;
	RwReal deltaTime;

	static RwBool firstCall = TRUE;
	static RwUInt32 lastFrameTime, lastAnimTime;

	if( firstCall )
	{
		lastFrameTime = lastAnimTime = RsTimer();

		firstCall = FALSE;
	}

	thisTime = RsTimer();

	/* 
	 * Has a second elapsed since we last updated the FPS...
	 */
	if( thisTime > (lastFrameTime + 1000) )
	{
		/* 
		 * Capture the frame counter...
		 */
		FramesPerSecond = FrameCounter;
		
		/*
		 * ...and reset...
		 */
		FrameCounter = 0;
		
		lastFrameTime = thisTime;
	}

	/*
	 * Animation update time in seconds...
	 */
	deltaTime = (thisTime - lastAnimTime) * 0.001f;

	AlCharacterUpdate(World, deltaTime);

	lastAnimTime = thisTime;

	Render();

	return;
}


/*
 *****************************************************************************
 */
RsEventStatus
AppEventHandler(RsEvent event, void *param)
{
	switch( event )
	{
		case rsINITIALIZE:
		{
			return Initialize() ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsCAMERASIZE:
		{
			g_clCamera->CameraSize(Camera, (RwRect *)param, 
				DEFAULT_VIEWWINDOW, DEFAULT_ASPECTRATIO);

			return rsEVENTPROCESSED;
		}

		case rsRWINITIALIZE:
		{
			return Initialize3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsRWTERMINATE:
		{
			Terminate3D();

			return rsEVENTPROCESSED;
		}

		case rsPLUGINATTACH:
		{
			return AttachPlugins() ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsINPUTDEVICEATTACH:
		{
			AttachInputDevices();

			return rsEVENTPROCESSED;
		}

		case rsIDLE:
		{
			Idle();

			return rsEVENTPROCESSED;
		}

		default:
		{
			return rsEVENTNOTPROCESSED;
		}
	}
}

/*
 *****************************************************************************
 */
void
SetWorld(RwInt32 worldIndex)
{
	RwChar worldPath[256];
	RwRect r;

	if (World)
	{
		TerminateWorld();
	}

	WorldIndex = worldIndex;
	sprintf(worldPath, "models/ground%d.bsp", worldIndex);
	InitializeWorld(worldPath);

	r.x = 0;
	r.y = 0;
	r.w = RsGlobal.maximumWidth;
	r.h = RsGlobal.maximumHeight;

	AppEventHandler(rsCAMERASIZE, &r);
}
