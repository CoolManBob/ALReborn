
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
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "camera.h"
#include "ABillBoard.h"

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

static RwBool FPSOn = FALSE;

static RwInt32 FrameCounter = 0;
static RwInt32 FramesPerSecond = 0;

static RwRGBA ForegroundColor = {200, 200, 200, 255};
static RwRGBA BackgroundColor = { 64,  64,  64,   0};

static RwV3d Yaxis = { 0.0f, 1.0f, 0.0f };
static RwV3d Xaxis = { 1.0f, 0.0f, 0.0f };
static RwV3d Zaxis = { 0.0f, 0.0f, 1.0f };

RpWorld *World = NULL;
RwCamera *Camera = NULL;
RtCharset *Charset = NULL;
RpLight		*AmbientLight = NULL;
RpLight		*MainLight = NULL;
RpAtomic	*Atomic1 = NULL;
RpAtomic	*Atomic2 = NULL;
RpAtomic	*Atomic3 = NULL;

AcuCamera	*g_clCamera = NULL;
AcuMenu		*g_clMenu = NULL;
AcuBillBoard	*g_clBillBoard = NULL;


static RpLight *
CreateAmbientLight(RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTAMBIENT);
    if( light )
    {
        RpWorldAddLight(world, light);

        return light;
    }

    return NULL;
}

static RpLight *
CreateMainLight(RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTDIRECTIONAL);
    if( light )
    {
        RwFrame *frame;

        frame = RwFrameCreate();
        if( frame )
        {
            RwFrameRotate(frame, &Xaxis, 30.0f, rwCOMBINEREPLACE);
            RwFrameRotate(frame, &Yaxis, 30.0f, rwCOMBINEPOSTCONCAT);

            RpLightSetFrame(light, frame);

            RpWorldAddLight(world, light);

            return light;
        }

        RpLightDestroy(light);
    }

    return NULL;
}
/*
 *****************************************************************************
 */
static RpWorld *
CreateWorld(void)
{
    RpWorld *world;
    RwBBox bb;

    bb.inf.x = bb.inf.y = bb.inf.z = -100.0f;
    bb.sup.x = bb.sup.y = bb.sup.z = 100.0f;

    world = RpWorldCreate(&bb);

    return world;
}


/*
 *****************************************************************************
 */
static RwCamera *
CreateCamera(RpWorld *world)
{
    RwCamera *camera;

    camera = g_clCamera->CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);
    if( camera )
    {
        RwCameraSetNearClipPlane(camera, 0.1f);
        RwCameraSetFarClipPlane(camera, 30.0f);

        RpWorldAddCamera(world, camera);

        return camera;
    }

    return NULL;
}


/*
 *****************************************************************************
 */
static RwBool 
Initialize(void)
{
    g_clCamera = new AcuCamera();
	g_clMenu = new AcuMenu();
	g_clBillBoard = new AcuBillBoard();
	
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

        //RsGlobal.appName = RWSTRING("RW3 Template Example");
		RsGlobal.appName = RWSTRING("AcuBillBoard");

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

    if( g_clMenu->MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {
        g_clMenu->MenuAddEntryBool(fpsLabel, &FPSOn, NULL);

        return TRUE;
    }

    return FALSE;
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

    Charset = RtCharsetCreate(&ForegroundColor, &BackgroundColor);
    if( Charset == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create raster charset."));
    
        return FALSE;
    }

    /*
     * Create an empty world if not loading a BSP...
     */
    World = CreateWorld();
    if( World == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create world."));

        return FALSE;
    }

	AmbientLight = CreateAmbientLight( World );
	if ( AmbientLight == NULL )
	{
		RsErrorMessage(RWSTRING("Give me Light T.T"));
		return FALSE;
	}

	MainLight = CreateMainLight( World );
	if ( MainLight == NULL )
	{
		RsErrorMessage(RWSTRING("Give me Light T.T"));
		return FALSE;
	}

    /*
     * Create a camera using the democom way...
     */
    Camera = CreateCamera(World);
    if( Camera == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }

	/*		Atomic »ý¼º		*/
	{
		g_clBillBoard->SetTextureName("tree01S");
	}
	Atomic1 = g_clBillBoard->CreateBBAtomic();
	if ( Atomic1 == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create Atomic"));
		return FALSE;
	}
	else
	{
		RpWorldAddAtomic( World, Atomic1 );
		RwFrame* Camera_Frame = RwCameraGetFrame( Camera );
		RwV3d	Camera_Trans = { 0.0f, 0.0f, -10.0f };
		RwFrameTranslate( Camera_Frame, &Camera_Trans, rwCOMBINEPOSTCONCAT );
	}

	{
		g_clBillBoard->SetTextureName("tree02S");
		g_clBillBoard->m_v3dOriginalPos.x = 3.0f;
	}
	Atomic2 = g_clBillBoard->CreateBBAtomic();
	if ( Atomic2 == NULL )
	{
		RsErrorMessage(RWSTRING("Cannot create Atomic"));
		return FALSE;
	}
	else RpWorldAddAtomic( World, Atomic2 );

	{
		g_clBillBoard->SetTextureName("tree35S.tif");
		g_clBillBoard->m_v3dOriginalPos.x = 1.5f;
		g_clBillBoard->m_v3dOriginalPos.z = 1.5f;
	}
	Atomic3 = g_clBillBoard->CreateBBAtomic();
	if ( Atomic3 == NULL )
	{
		RsErrorMessage(RWSTRING("CannotcreateAtomic"));
		return FALSE;
	}
	else RpWorldAddAtomic( World, Atomic3 );	
	
	
	
    if( !InitializeMenu() )
    {
        RsErrorMessage(RWSTRING("Error initializing menu."));

        return FALSE;
    }

#ifdef RWMETRICS
    RsMetricsOpen(Camera);
#endif

    return TRUE;
}


/*
 *****************************************************************************
 */
static void 
Terminate3D(void)
{
    
	/*
     * Close or destroy RenderWare components in the reverse order they
     * were opened or created...
     */
	
#ifdef RWMETRICS
    RsMetricsClose();
#endif

    g_clMenu->MenuClose();

    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);
        /*
         * This assumes the camera was created the democom way...
         */
        g_clCamera->CameraDestroy(Camera);
    }

	if ( AmbientLight )
	{
		RpWorldRemoveLight( World, AmbientLight );
		RpLightDestroy( AmbientLight );
	}

	if ( MainLight )
	{
		RpWorldRemoveLight( World, MainLight );
		RpLightDestroy( MainLight );
	}

	if ( Atomic1 )
	{
		RpWorldRemoveAtomic( World, Atomic1 );
		RpAtomicDestroy( Atomic1 );
	}

	if ( Atomic2 )
	{
		RpWorldRemoveAtomic( World, Atomic2 );
		RpAtomicDestroy( Atomic2 );
	}

	if ( Atomic3 )
	{
		RpWorldRemoveAtomic( World, Atomic3 );
		RpAtomicDestroy( Atomic3 );
	}

    if( World )
    {
        RpWorldDestroy(World);
    }

    if( Charset )
    {
        RwRasterDestroy(Charset);
    }

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

	if ( g_clBillBoard )
	{
			delete g_clBillBoard;
			g_clBillBoard = NULL;
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
    RwChar caption[256];

    if( FPSOn )
    {
        rwsprintf(caption, RWSTRING("FPS: %03d"), FramesPerSecond);

        RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSTOPRIGHT);
    }
	
	RwV3d*		Camera_Lookat;
	RwMatrix*	Camera_Matrix;
	
	Camera_Matrix = RwFrameGetMatrix( RwCameraGetFrame( Camera ) );
	Camera_Lookat = RwMatrixGetAt( Camera_Matrix );
	
	rwsprintf(caption, RWSTRING("Camera x = %f, Camera z = %f"), Camera_Lookat->x, Camera_Lookat->z);
	RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSTOPLEFT);	
	
    return;
}


/*
 *****************************************************************************
 */
static void 
Render(void)
{
    RwCameraClear(Camera, &BackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate(Camera) )
    {
        if( g_clMenu->MenuGetStatus() != HELPMODE )
        {
            /*
             * Scene rendering here...
             */
			RpWorldRender( World );

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

    /*
     * Update any animations here...
     */

    lastAnimTime = thisTime;
	g_clBillBoard->RenderIdle( Atomic1, Camera );
	g_clBillBoard->RenderIdle( Atomic2, Camera , 2.0f, 2.0f);
	g_clBillBoard->RenderIdle( Atomic3, Camera , 0.5f, 0.5f);

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
