#ifdef USE_API

/****************************************************************************
 *
 * win.c
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
 * Copyright (c) 1999, 2000, 2001 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/*
 * We define the OS version to be MS Windows 98, NT4 or greater so the mouse
 * wheel is supported... 
 */
#define _WIN32_WINDOWS 0x0410
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500

//@{ Jaewon 20051027
//#define VLD_AGGREGATE_DUPLICATES
//#include "vld.h"
//@} Jaewon

#include "ApBase.h"

#include "ApLockManager.h"

#include <mmsystem.h>
#include <shellapi.h>

/*
#ifdef _DEBUG
#include "Stackwalker.h"
#endif // _DEBUG
*/

#include "AuProfileManager.h"

#if (defined(_MSC_VER))
#include <tchar.h>
#endif /* (defined(_MSC_VER)) */

#include <stdio.h>

#include "rwcore.h"
#include "rtbmp.h"

#include "resource.h"
#include "platform.h"
#include "skeleton.h"
#include "mouse.h"
//#include "vecfont.h"
#include "win.h"

#include "AuInputComposer.h"
#include "AgcmUILogin.h"
#include "AgcmFPSManager.h"

#ifdef RWSPLASH
/* Splash screen */
#include "splash.h"
#endif

#include "rtfsyst.h"

#ifdef _USE_DINPUT_
#include <basetsd.h>
#include <dinput.h>
#endif //_USE_DINPUT_

#define MAX_SUBSYSTEMS (16)
#define UNC_PATH_SIZE       (256)   /* should be sufficient */

#if (!defined(_MAX_PATH))
/*
 * See 
 * Program Files/Microsoft Visual Studio/VC98/Include/STDLIB.H
 */
#ifndef _MAC
#define _MAX_PATH   260        /* max. length of full pathname */
#else /* def _MAC */
#define _MAX_PATH   256        /* max. length of full pathname */
#endif /* _MAC */
#endif /* (!defined(_MAX_PATH)) */

typedef struct _win_int_64 WinInt64;
struct _win_int_64
{
    RwInt32             msb;
    RwUInt32            lsb;
};

#define WinInt64Sub(_result, _end, _start)              \
do                                                      \
{                                                       \
    (_result).lsb = (_end).lsb - (_start).lsb;          \
    if ((_end).msb < (_start).msb)                      \
    {                                                   \
        (_result).msb = (_end).msb - (_start).msb - 1;  \
    }                                                   \
    else                                                \
    {                                                   \
        (_result).msb = (_end).msb - (_start).msb;      \
    }                                                   \
}                                                       \
while (0)

#define doubleFromWinInt64(_x)                                     \
   ( ((double)((_x).msb))*((double)(1<<16))*((double)(1<<16))      \
      + ((double)((_x).lsb)) )

static RwBool       ForegroundApp = TRUE;
static RwBool       RwInitialized = FALSE;
static RwSubSystemInfo GsubSysInfo[MAX_SUBSYSTEMS];
static RwInt32      GnumSubSystems = 0;
static RwInt32      GcurSel = 0, GcurSelVM = 0;
static RwInt32      FrameCount = 0;

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
static RwBool       DisplayClocks = FALSE;
static WinInt64     ClocksStart = { 0, 0 };
static WinInt64     ClocksEnd = { 0, 0 };
#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

static AuTickCounter	g_csTickCounter;

/* Class name for the MS Window's window class. */

static const RwChar *AppClassName = RWSTRING("RwAppClass");

/* Scan code translation tables */

static const int KeyTableEnglish[256] = { /* ENGLISH */
    rsNULL, rsESC, '1', '2', '3', '4', '5', '6', /* 0 */
    '7', '8', '9', '0', '-', '=', rsBACKSP, rsTAB, /* 8 */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', /* 16 */
    'o', 'p', '[', ']', rsENTER, rsLCTRL, 'a', 's', /* 24 */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 32 */
    '\'', '`', rsLSHIFT, '#', 'z', 'x', 'c', 'v', /* 40 */
    'b', 'n', 'm', ',', '.', '/', rsRSHIFT, rsTIMES, /* 48 */
    rsLALT, ' ', rsCAPSLK, rsF1, rsF2, rsF3, rsF4, rsF5, /* 56 */
    rsF6, rsF7, rsF8, rsF9, rsF10, rsNUMLOCK, rsNULL, rsHOME, /* 64 */
    rsPADUP, rsPADPGUP, rsMINUS, rsPADLEFT, rsPAD5, rsPADRIGHT, rsPLUS, rsPADEND, /* 72 */
    rsPADDOWN, rsPADPGDN, rsPADINS, rsPADDEL, rsNULL, rsNULL, '\\', rsF11, /* 80 */
    rsF12, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 88 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 96 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 104 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 112 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 120 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 128 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 136 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 144 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsPADENTER, rsRCTRL, rsNULL, rsNULL, /* 152 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 160 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 168 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsDIVIDE, rsNULL, rsNULL, /* 176 */
    rsRALT, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 184 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNUMLOCK, rsNULL, rsHOME, /* 192 */
    rsUP, rsPGUP, rsNULL, rsLEFT, rsNULL, rsRIGHT, rsNULL, rsEND, /* 200 */
    rsDOWN, rsPGDN, rsINS, rsDEL, rsNULL, rsNULL, rsNULL, rsNULL, /* 208 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 216 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 224 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 232 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 240 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL /* 248 */
};


/* platform specfic global data */

//typedef struct
//{
//    HWND window;
//    HINSTANCE instance;
//    RwBool fullScreen;
//    RwV2d lastMousePos;
//}
//psGlobalType;

psGlobalType PsGlobal;

#ifdef _USE_DINPUT_

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAMPLE_BUFFER_SIZE  100      // arbitrary number of buffer elements

POINTS	g_stMousePosPrev;
POINTS	g_stMousePos;

HRESULT InitDirectInputDevice( HWND hWnd );
HRESULT ReadImmediateDIData();
HRESULT ReadBufferedDIData();
VOID FreeDirectInput();

BOOL					g_bImmediate = FALSE;

LPDIRECTINPUT8			g_pDI    = NULL;         
LPDIRECTINPUTDEVICE8	g_pMouse = NULL;

#endif // _USE_DINPUT_

#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

#ifdef UNDER_CE
#ifndef MAKEPOINTS
#define MAKEPOINTS(l) (*((POINTS FAR *) & (l)))
#endif
#endif

#ifdef _USE_DINPUT_
#define MAKEPOINTS(l) g_stMousePos
#endif // _USE_DINPUT_

MSG g_msgLastReceived;

//@{ 2006/08/09 burumal
//#include "AuKbHook.h"

void EnableWindowsShortcutKeys();
void DisableWindowsShortcutKeys();
//@}

/*
 *****************************************************************************
 */
void
psWindowSetText(const RwChar *text)
{
    SetWindowText(PSGLOBAL(window), text);

    return;
}


/*
 *****************************************************************************
 */
void
psErrorMessage(const RwChar *message)
{
    MessageBox(NULL, message, RsGlobal.appName,
#ifndef UNDER_CE
               MB_TASKMODAL |
#endif
               MB_ICONERROR | MB_TOPMOST | MB_OK);

    return;
}


/*
 *****************************************************************************
 */
void
psWarningMessage(const RwChar *message)
{
    MessageBox(NULL, message, RsGlobal.appName,
#ifndef UNDER_CE
               MB_TASKMODAL |
#endif
               MB_ICONWARNING | MB_TOPMOST | MB_OK);

    return;
}


/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera *camera)
{
    //@{ Jaewon 20050706
	// ATI guys said that most applications(excluding benchmark apps) should 
	// use rwRASTERFLIPWAITVSYNC so as to prevent the screen tearing in the
	// full screen mode.
	RwCameraShowRaster(camera, PSGLOBAL(window), PSGLOBAL(fullScreen)?rwRASTERFLIPWAITVSYNC:0);
	//@} Jaewon

    if ((0 < FrameCount) && (0 == --FrameCount))
    {
#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
        if (DisplayClocks)
        {
            const static WinInt64 ClocksBias = { 0, 13 + 1 };
            WinInt64            ClocksElapsed;
            RwChar              title[256];
            RwChar              buffer[256];

            /* *INDENT-OFF* */
            _asm
            {
                ;              /* RDTSC - get ending timestamp to edx:eax */
                ;              /* (13 cycles) */
                _emit 0x0F;
                _emit 0x31;
                ;              /* save ending timestamp */
                mov            ClocksEnd.msb, edx;
                mov            ClocksEnd.lsb, eax;
            }
            /* *INDENT-ON* */      

            WinInt64Sub(ClocksElapsed, ClocksEnd, ClocksStart);
            WinInt64Sub(ClocksElapsed, ClocksElapsed, ClocksBias);

            _sntprintf(title, sizeof(title),
                       _T("%s(%d)"), __FILE__, __LINE__);
            _sntprintf(buffer, sizeof(buffer),
                       _T("%24.0f Elapsed Clocks"),
                       doubleFromWinInt64(ClocksElapsed));

            MessageBox(NULL, buffer, title, MB_OK | MB_ICONINFORMATION);
        }

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

        exit(0); /* AppEventHandler(rsRWTERMINATE, NULL) */ ;
    }

    return;
}


/*
 *****************************************************************************
 */
RwUInt32
psTimer(void)
{
    RwUInt32 time;
//    TIMECAPS timecaps;

#ifdef UNDER_CE
    time = (RwUInt32) GetTickCount();
#else
/*
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));
    timeBeginPeriod(timecaps.wPeriodMin);
    time = (RwUInt32) timeGetTime();
    timeEndPeriod(timecaps.wPeriodMin);\
*/
	time = g_csTickCounter.GetTickCount();
#endif

    return time;
}


/*
 *****************************************************************************
 */
RwImage * 
psGrabScreen(RwCamera *camera)
{
    RwRaster *camRas;
    RwInt32 width, height;
    RwImage *image;

    camRas = RwCameraGetRaster(camera);

    if( camRas )
    {
        width = RwRasterGetWidth(camRas);
        height = RwRasterGetHeight(camRas);
        image = RwImageCreate(width, height, 32);

        if( image )
        {
            RwImageAllocatePixels(image);
            RwImageSetFromRaster(image, camRas);

            return image;
        }
        else
        {
            return NULL;
        }
    }

    return NULL;
}


/*
 *****************************************************************************
 */
void
psMouseSetVisibility(RwBool visible)
{
    ShowCursor(visible);

    return;
}

static RwBool validDelta = FALSE;


/*
 *****************************************************************************
 */
void
psMouseSetPos(RwV2d *pos)
{
    POINT point;

    validDelta = FALSE;

    point.x = (RwInt32) pos->x;
    point.y = (RwInt32) pos->y;

    ClientToScreen(PSGLOBAL(window), &point);

    SetCursorPos(point.x, point.y);

    return;
}


/*
 *****************************************************************************
 */
RwChar *
psPathnameCreate(const RwChar *srcBuffer)
{
    RwChar *dstBuffer;
    RwChar *charToConvert;

    /* 
     * First duplicate the string 
     */	
	//@{ 2006/07/18 burumal
	// RtlValidateHeap 체크에서 오류가 발생 하므로 수정함
	//dstBuffer = (RwChar *) malloc(sizeof(RwChar) * (rwstrlen(srcBuffer) + 1));
    dstBuffer = (RwChar *) RwMalloc(sizeof(RwChar) * (rwstrlen(srcBuffer) + 1), 0);	
	//@}

    if( dstBuffer )
    {
        rwstrcpy(dstBuffer, srcBuffer);

        /* 
         * Convert a path for use on Windows. 
         * Convert all /s and :s into \s 
         */
        while( (charToConvert = rwstrchr(dstBuffer, '/')) )
        {
            *charToConvert = '\\';
        }
#if 0
        while( (charToConvert = rwstrchr(dstBuffer, ':')) )
        {
            *charToConvert = '\\';
        }
#endif
    }

    return dstBuffer;
}


/*
 *****************************************************************************
 */
void
psPathnameDestroy(RwChar *buffer)
{
    if( buffer )
    {
        RwFree(buffer);
    }

    return;
}


/*
 *****************************************************************************
 */
RwChar
psPathGetSeparator(void)
{
    return '\\';
}


/*
 *****************************************************************************
 */
RwMemoryFunctions *
psGetMemoryFunctions(void)
{
    return NULL;
}


/*
 *****************************************************************************
 */
RwBool
psInstallFileSystem(void)
{
	RwChar      curDirBuffer[_MAX_PATH];
    RwUInt32    retValue;
    RtFileSystem *wfs, *unc;

    RwUInt32 drivesMask;
    RwInt32 drive;
    RwChar  fsName[2];
    
    /* get the current directory */
    retValue = GetCurrentDirectory(_MAX_PATH, curDirBuffer);
    if (0 == retValue || retValue > _MAX_PATH)
    {
        return FALSE;
    }

    /* Register a unc file system */
    
    /* tag a \ to the end of the current directory */
    /* only fails if the buffer size is exhausted */
    rwstrcat( curDirBuffer, "\\" );

    /** Setup the file system manager */
    if ((unc = RtWinFSystemInit(MAX_NB_FILES_PER_FS, curDirBuffer, "unc")) != NULL)
    {
        /* Now actually register the file system */
        if (RtFSManagerRegister(unc) == FALSE)
        {
            return (FALSE);
        }
    }
    else
    {
        return (FALSE);
    }
    
    /* Now register local file systems */
    
    CharUpper(curDirBuffer);

    /* 
     * loop through all logical drives and register a file system for each
     * valid one.
     * Start at 2: this removes a:
     */
    drivesMask = GetLogicalDrives();

    for( drive = 2, drivesMask >>= 1; drivesMask != 0; drive++, drivesMask >>= 1)
    {
        if (drivesMask & 0x01)
        {
            RwInt32 ret;
            RwChar  deviceName[4];

            deviceName[0] = drive + 'A' - 1;
            deviceName[1] = ':';
            deviceName[2] = '\\';   /* Needed for Win98 */
            deviceName[3] = '\0';

			if( deviceName[0] != curDirBuffer[0] ) continue;

            ret = GetDriveType(deviceName);

            if ( ret != DRIVE_UNKNOWN && ret != DRIVE_NO_ROOT_DIR && ret != DRIVE_CDROM)
            {
                /* Fix device name */
                deviceName[2] = '\0';

                fsName[0] = deviceName[0];
                fsName[1] = '\0';

                /** Setup the file system manager */
                if ((wfs = RtWinFSystemInit(MAX_NB_FILES_PER_FS, deviceName, fsName)) != NULL)
                {
                    /* Now actually register the file system */
                    if (RtFSManagerRegister(wfs) == FALSE)
                    {
                        return (FALSE);
                    }
                    else
                    {
                        /* Set the unc file system as default if we have a unc path */
                        if (curDirBuffer[1] != ':')
                        {
                            RtFSManagerSetDefaultFileSystem(unc);
                        }
                        else if (deviceName[0] == curDirBuffer[0])
                        {
                            RtFSManagerSetDefaultFileSystem(wfs);
                        }
                    }
                }
                else
                {
                    return (FALSE);
                }
            }
        }
    }
    
    return (TRUE);
}


/*
 *****************************************************************************
 */
RwBool
psNativeTextureSupport(void)
{
    return TRUE;
}


/**********************************************************************/

void
psDebugMessageHandler(RwDebugType type __RWUNUSED__, const RwChar *str)
{
    OutputDebugString(str);
    OutputDebugString(RWSTRING("\n"));

    return;
}


/*
 *****************************************************************************
 */
static RwInt32
winTranslateKey(WPARAM wParam __RWUNUSED__, LPARAM lParam)
{
    RwInt32 nOutKey;

    nOutKey = (lParam & 0x00ff0000) >> 16;
    if (lParam & 0x01000000)
    {
        nOutKey |= 128;
    }

    return nOutKey;
}


/*
 *****************************************************************************
 */
#ifdef RWMOUSE

static void
ClipMouseToWindow(HWND window)
{
    RECT wRect;

    GetWindowRect(window, &wRect);

    if( !PSGLOBAL(fullScreen) )
    {
        wRect.left += GetSystemMetrics(SM_CXFRAME);
        wRect.top  += GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYFRAME);

        wRect.right  -= GetSystemMetrics(SM_CXFRAME);
        wRect.bottom -= GetSystemMetrics(SM_CYFRAME);
    }

    ClipCursor(&wRect);

    return;
}

#endif


/*
 *****************************************************************************
 */
static BOOL
InitApplication(HANDLE instance)
{
    /*
     * Perform any necessary MS Windows application initialization. Basically,
     * this means registering the window class for this application.
     */

	g_csTickCounter.Initialize();

    WNDCLASS windowClass;

#ifdef UNDER_CE
    windowClass.style			= 0;
#else
    windowClass.style			= CS_BYTEALIGNWINDOW | CS_DBLCLKS;
#endif
    windowClass.lpfnWndProc		= (WNDPROC) MainWndProc;
    windowClass.cbClsExtra		= 0;
    windowClass.cbWndExtra		= 0;
    windowClass.hInstance		= (HINSTANCE)instance;
	windowClass.hIcon			= LoadIcon( (HINSTANCE)instance , (LPCTSTR)135 );
    windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground	= NULL;
    windowClass.lpszMenuName	= NULL;
    windowClass.lpszClassName	= AppClassName;
	
    return RegisterClass(&windowClass);
}


/*
 *****************************************************************************
 */
static HWND
InitInstance(HANDLE instance)
{
    /*
     * Perform any necessary initialization for this instance of the 
     * application.
     *
     * Create the MS Window's window instance for this application. The
     * initial window size is given by the defined camera size. The window 
     * is not given a title as we set it during Init3D() with information 
     * about the version of RenderWare being used.
     */

    RECT rect;

    rect.left = rect.top = 0;
    rect.right = RsGlobal.maximumWidth;
    rect.bottom = RsGlobal.maximumHeight;

#ifdef UNDER_CE
    {
        DWORD               style, exStyle;

        style = WS_BORDER | WS_POPUP | WS_CAPTION | WS_SYSMENU ;
        exStyle = WS_EX_NODRAG | WS_EX_CAPTIONOKBTN | WS_EX_WINDOWEDGE;

        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        return CreateWindow(AppClassName, RsGlobal.appName,
                            style,
                            rect.left, rect.top,
                            rect.right - rect.left,
                            rect.bottom - rect.top, NULL, NULL,
                            instance, NULL);
    }
#else
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    return CreateWindow(AppClassName, RsGlobal.appName,
						WS_OVERLAPPED     |
						WS_CAPTION        |
                        WS_SYSMENU        |
                        //WS_THICKFRAME     |
                        WS_MINIMIZEBOX     
                        //WS_MAXIMIZEBOX
						,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        rect.right - rect.left, rect.bottom - rect.top,
                        (HWND)NULL, (HMENU)NULL, (HINSTANCE)instance, NULL);
#endif
}


/*
 *****************************************************************************
 */
#ifdef UNDER_CE
#define CMDSTR  LPWSTR
#else
#define CMDSTR  LPSTR
#endif


/*
 *****************************************************************************
 */
RwBool
psInitialize(void)
{
    PsGlobal.lastMousePos.x = PsGlobal.lastMousePos.y = 0.0;

    PsGlobal.fullScreen = FALSE;

    RsGlobal.ps = &PsGlobal;

    return TRUE;
}


/*
 *****************************************************************************
 */
void
psTerminate(void)
{
	//@{ 2006/08/09 burumal
	EnableWindowsShortcutKeys();
	//@}

	RtFileSystem*	fs;
	fs = RtFSManagerGetFileSystemFromName("unc");

	// 마고자 (2004-04-26 오후 4:21:51) : fs가 널일경우 곤란해요~
	// 비디오 선택을 하지 않았을때 발생함.

	if( fs ) RtFSManagerUnregister (fs);
    return;
}


/*
 *****************************************************************************
 */
RwBool
psAlwaysOnTop(RwBool alwaysOnTop)
{
    RECT winRect;
    HWND hwnd;

    hwnd = PSGLOBAL(window);

    GetWindowRect(hwnd, &winRect);

    if( alwaysOnTop )
    {
        return (RwBool)SetWindowPos(hwnd, HWND_TOPMOST,
                                     winRect.left, winRect.top,
                                     winRect.right - winRect.left,
                                     winRect.bottom - winRect.top, 0);
    }
    else
    {
        return (RwBool)SetWindowPos(hwnd, HWND_NOTOPMOST,
                                     winRect.left, winRect.top,
                                     winRect.right - winRect.left,
                                     winRect.bottom - winRect.top, 0);
    }
}


/*
 *****************************************************************************
 */
#ifdef RWMETRICS
void
psMetricsRender(RsVectorFont *vecFont,
                RwV2d *pos, RwMetrics *metrics)
{
#if (defined (D3D8_DRVMODEL_H))
    const RwD3D8Metrics       *d3d8Metrics =
        (const RwD3D8Metrics *) metrics->devSpecificMetrics;

    if (d3d8Metrics)
    {
        RwChar              message[200];

        sprintf(message, "RenderStateChanges = %d",
                                    d3d8Metrics->numRenderStateChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;

        sprintf(message, "TextureStateChanges = %d",
                                    d3d8Metrics->numTextureStageStateChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;   

        sprintf(message, "MaterialChanges = %d",
                                    d3d8Metrics->numMaterialChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;

        sprintf(message, "LightsChanged = %d",
                                    d3d8Metrics->numLightsChanged);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;   
    }
#endif

    return;
}
#endif /* RWMETRICS */


/*
 *****************************************************************************
 */
static void
dialogAddModes(HWND wndListVideMode)
{
    RwInt32             vidMode, numVidModes;
    RwVideoMode         vidModemInfo;
    RwChar              modeStr[100];

    numVidModes = RwEngineGetNumVideoModes();

    /* Add the available video modes to the dialog */
    for (vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        int                 index;

        RwEngineGetVideoModeInfo(&vidModemInfo, vidMode);

        rwsprintf(modeStr, RWSTRING("%lu x %lu x %lu %s"),
                  vidModemInfo.width, vidModemInfo.height,
                  vidModemInfo.depth,
                  vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE ?
                  RWSTRING("(Fullscreen)") : RWSTRING(""));

        /* Add name and an index so we can ID it later */
        index =
            SendMessage(wndListVideMode, CB_ADDSTRING, 0, (LPARAM) modeStr);
        SendMessage(wndListVideMode, CB_SETITEMDATA, index, (LPARAM) vidMode);
    }

    return;
}


static void
dialogInit(HWND hDlg,
           UINT message __RWUNUSED__,
           WPARAM wParam __RWUNUSED__, LPARAM lParam __RWUNUSED__)
{
    HWND                wndList, wndListVideMode;
    RwInt32             subSysNum;
    RwInt32             width, height;

    /* Handle the list box */
    wndList = GetDlgItem(hDlg, IDC_DEVICESEL);
    wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);

    width = RsGlobal.maximumWidth;
    height = RsGlobal.maximumHeight;

    /* Add the names of the sub systems to the dialog */
    for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
    {
        /* Add name and an index so we can ID it later */
        SendMessage(wndList, CB_ADDSTRING, 0,
                    (LPARAM) GsubSysInfo[subSysNum].name);
        SendMessage(wndList, CB_SETITEMDATA, subSysNum, (LPARAM) subSysNum);
    }
    SendMessage(wndList, CB_SETCURSEL, GcurSel, 0);

    /* display avalible modes */
    dialogAddModes(wndListVideMode);

    GcurSelVM = RwEngineGetCurrentVideoMode();
    SendMessage(wndListVideMode, CB_SETCURSEL, GcurSelVM, 0);

    SetFocus(wndList);

    return;
}


static void
dialogDevSelect(HWND hDlg,
                UINT message __RWUNUSED__,
                WPARAM wParam __RWUNUSED__, LPARAM lParam __RWUNUSED__)
{
    HWND                wndList, wndListVideMode;
    RwInt32             selection;
    RwInt32             width, height;

    /* Handle the list box */
    wndList = GetDlgItem(hDlg, IDC_DEVICESEL);
    wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);

    width = RsGlobal.maximumWidth;
    height = RsGlobal.maximumHeight;

    /* Update the selected entry */
    selection = SendMessage(wndList, CB_GETCURSEL, 0, 0);
    if (selection != GcurSel)
    {
        GcurSel = SendMessage(wndList, CB_GETITEMDATA, selection, 0);

        RwEngineSetSubSystem(GcurSel);

        wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);
        /* changed device so update video modes listbox */
        SendMessage(wndListVideMode, CB_RESETCONTENT, 0, 0);

        /* display avalible modes */
        dialogAddModes(wndListVideMode);

        GcurSelVM = RwEngineGetCurrentVideoMode();
        SendMessage(wndListVideMode, CB_SETCURSEL, GcurSelVM, 0);
    }
}

/*
 *****************************************************************************
 */
static BOOL CALLBACK
DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            dialogInit(hDlg, message, wParam, lParam);

            return FALSE;
        }

        case WM_COMMAND:
            {
                switch (LOWORD(wParam))
                {
                    case IDC_DEVICESEL:
                    {
                        dialogDevSelect(hDlg, message, wParam, lParam);

                        return TRUE;
                    }

                    case IDC_VIDMODE:
                    {
                        if (HIWORD(wParam) == CBN_SELCHANGE)
                        {
                            HWND                wndListVideMode;
                            RwInt32             vmSel;

                            wndListVideMode =
                                GetDlgItem(hDlg, IDC_VIDMODE);

                            /* Update the selected entry */
                            vmSel =
                                SendMessage(wndListVideMode,
                                            CB_GETCURSEL, 0, 0);
                            GcurSelVM =
                                SendMessage(wndListVideMode,
                                            CB_GETITEMDATA, vmSel, 0);
                        }

                        return TRUE;
                    }

                    case IDOK:
                    {
                        if (HIWORD(wParam) == BN_CLICKED)
                        {
                            EndDialog(hDlg, TRUE);
                        }

                        return TRUE;
                    }

                    case IDEXIT:
                    {
                        if (HIWORD(wParam) == BN_CLICKED)
                        {
                            EndDialog(hDlg, FALSE);
                        }

                        return TRUE;
                    }

                    default:
                    {
                        return FALSE;
                    }
                }
            }

        default:
        {
            return FALSE;
        }
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
LRESULT CALLBACK
MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINTS points;
    static BOOL noMemory = FALSE;

	// IME 처리를 위한 변수들..
	static char strIMEComposingString	[ 256	]	= ""	;	// 조합중인 문자 저장..
	static char	strCanText				[ 256	]	= ""	;	// 특수문자를 위한 변수..
	static int	nCanNumber				= 0					;	// 특수문자이 위치.
	static int	nCanMax					= 0					;	// 특수문자의 목록의 최대 갯수.

	int		len		= 0		;
	HIMC	hIMC	= NULL	;	// IME 핸들..

    switch( message )
    {
#if !defined(UNDER_CE)
        case WM_CREATE:
			{
				/*
				 * Clumps are loaded into the scene by drag and drop.
				 * So make this window a drop site.
				 */
				DragAcceptFiles(window, TRUE);
				AuInputComposer::GetInstance().InitInputComposer();

				switch ( g_eServiceArea )
				{
				case AP_SERVICE_AREA_CHINA:
					// 0x0804 is chinese(PRC) language ID
					AuInputComposer::GetInstance().OnInputLanguageChange(window, 0, 0x0804);
					break;

				case AP_SERVICE_AREA_WESTERN:
					// 0x040C는 French lang ID
					AuInputComposer::GetInstance().OnInputLanguageChange(window, 0, 0x040C);
					break;

				case AP_SERVICE_AREA_JAPAN:
					// 0x0411 is Japanese language ID
					AuInputComposer::GetInstance().OnInputLanguageChange(window, 0, 0x0411);
					break;

				default:
					AuInputComposer::GetInstance().OnInputLanguageChange(window, 0, (LPARAM)GetKeyboardLayout(0));
				}

				return 0L;
			}

#ifdef RWMOUSE
        case WM_MOVE:
			{
				ClipMouseToWindow(window);

				return 0L;
			}
#endif  /* RWMOUSE */

#endif  /* !defined(UNDER_CE) */

        case WM_SIZE:
			{
				RwRect r;

				r.x = 0;
				r.y = 0;
				r.w = LOWORD(lParam);
				r.h = HIWORD(lParam);

#ifdef RWMOUSE
	          ClipMouseToWindow(window);
#endif  /* RWMOUSE */

				switch( wParam )
				{
				case SIZE_MINIMIZED	:
					RsEventHandler(rsACTIVATE, (void *)1);
					break;
				case SIZE_RESTORED	:
					RsEventHandler(rsACTIVATE, (void *)0);
					break;
				}

				if (RwInitialized && r.h > 0 && r.w > 0)
				{
					RsEventHandler(rsCAMERASIZE, &r);

					if (r.w != LOWORD(lParam) && r.h != HIWORD(lParam))
					{
						WINDOWPLACEMENT     wp;

						/* failed to create window of required size */
						noMemory = TRUE;

						/* stop re-sizing */
						ReleaseCapture();

						/* handle maximised window */
						GetWindowPlacement(window, &wp);
						if (wp.showCmd == SW_SHOWMAXIMIZED)
						{
							SendMessage(window, WM_WINDOWPOSCHANGED, 0, 0);
						}
					}
					else
					{
						noMemory = FALSE;
					}

				}

				return 0L;
			}

        case WM_SIZING:
			{
				/* 
				 * Handle event to ensure window contents are displayed during re-size
				 * as this can be disabled by the user, then if there is not enough 
				 * memory things don't work.
				 */
				RECT               *newPos = (LPRECT) lParam;
				RECT                rect;

				/* redraw window */
				if (RwInitialized)
				{
					RsEventHandler(rsIDLE, NULL);
				}

				/* Manually resize window */
				rect.left = rect.top = 0;
				rect.bottom = newPos->bottom - newPos->top;
				rect.right = newPos->right - newPos->left;

				SetWindowPos(window, HWND_TOP, rect.left, rect.top,
							 (rect.right - rect.left),
							 (rect.bottom - rect.top), SWP_NOMOVE);

				return 0L;
			}

        case WM_LBUTTONDOWN:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;

				SetCapture(window);

				RsMouseEventHandler(rsLEFTBUTTONDOWN, &ms);

				return 0L;
			}

        case WM_LBUTTONDBLCLK:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;

				SetCapture(window);

				RsMouseEventHandler(rsLEFTBUTTONDBLCLK, &ms);

				return 0L;
			}

        case WM_MBUTTONDOWN:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;

				SetCapture(window);

				RsMouseEventHandler(rsMIDDLEBUTTONDOWN, &ms);

				return 0L;
			}

        case WM_MBUTTONDBLCLK:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;

				SetCapture(window);

				RsMouseEventHandler(rsMIDDLEBUTTONDBLCLK, &ms);

				return 0L;
			}

        case WM_RBUTTONDBLCLK:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;

				SetCapture(window);

				RsMouseEventHandler(rsRIGHTBUTTONDBLCLK, &ms);

				return 0L;
			}

        case WM_RBUTTONDOWN:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;

				SetCapture(window);

				RsMouseEventHandler(rsRIGHTBUTTONDOWN, &ms);

				return 0L;
			}

        case WM_MOUSEWHEEL:
			{
				RsMouseStatus ms;

				points		= MAKEPOINTS( lParam )					;

				POINT		point = {points.x, points.y}			;

				::ScreenToClient(window, &point)					;

				ms.pos.x	= ( RwReal ) point.x					;
				ms.pos.y	= ( RwReal ) point.y					;
//				ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
//				ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;
				ms.wheel	= (short) HIWORD(wParam)				;


				RsMouseEventHandler(rsMOUSEWHEELMOVE, &ms);

				return 0L;
			}

        case WM_MOUSEMOVE:
			{
				PROFILE("MainWndProc - MouseMove");

				if (ForegroundApp)
				{
					points		= MAKEPOINTS( lParam )					;

					if (validDelta)
					{
						RsMouseStatus ms;

						ms.delta.x = points.x - PSGLOBAL(lastMousePos).x;
						ms.delta.y = points.y - PSGLOBAL(lastMousePos).y;
						ms.pos.x = points.x;
						ms.pos.y = points.y;

						RsMouseEventHandler(rsMOUSEMOVE, &ms);
					}
					else
					{
						validDelta = TRUE;
					}

					PSGLOBAL(lastMousePos).x = points.x;
					PSGLOBAL(lastMousePos).y = points.y;
				}

				return 0L;
			}

        case WM_LBUTTONUP:
			{
				ReleaseCapture();

				RsMouseStatus ms;
				
				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= (wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= (wParam & MK_CONTROL	) ? TRUE : FALSE;

				RsMouseEventHandler(rsLEFTBUTTONUP, &ms);

				return 0L;
			}

		case WM_RBUTTONUP:
			{
				ReleaseCapture();

				RsMouseStatus ms;
				
				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= (wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= (wParam & MK_CONTROL	) ? TRUE : FALSE;

				RsMouseEventHandler(rsRIGHTBUTTONUP, &ms);

				return 0L;
			}
        case WM_MBUTTONUP:
			{
				ReleaseCapture();

				RsMouseStatus ms;
				
				points		= MAKEPOINTS( lParam )					;
				ms.pos.x	= points.x								;
				ms.pos.y	= points.y								;
				ms.shift	= (wParam & MK_SHIFT	) ? TRUE : FALSE;
				ms.control	= (wParam & MK_CONTROL	) ? TRUE : FALSE;

				RsMouseEventHandler(rsMIDDLEBUTTONUP, &ms);

				return 0L;
			}

        case WM_KEYDOWN:
			{
				RsKeyStatus ks;

				if(wParam == 229) // 한영키
				{
					ks.keyScanCode = winTranslateKey(wParam, lParam);
					ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];
					RsKeyboardEventHandler(rsKEYDOWN, &ks);
				}
				else //if (!(lParam & 0x40000000)) /* ignore repeat events */
				{
					ks.keyScanCode = winTranslateKey(wParam, lParam);
					ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];
					RsKeyboardEventHandler(rsKEYDOWN, &ks);

					/*
					if (ks.keyCharCode == rsESC)
						RsEventHandler(rsQUITAPP, NULL);
					*/
				}

				return 0L;
			}

        case WM_KEYUP:
			{
				RsKeyStatus ks;

				ks.keyScanCode = winTranslateKey(wParam, lParam);
				ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];

				RsKeyboardEventHandler(rsKEYUP, &ks);

				return 0L;
			}
		
		case WM_CHAR:
			{
				AuInputComposer::GetInstance().OnChar(window, wParam, lParam);
				return 0L;
			}

		case WM_INPUTLANGCHANGE:
			AuInputComposer::GetInstance().OnInputLanguageChange(window, wParam, lParam);
			break;

		case WM_IME_SETCONTEXT:
			lParam = 0;
			break;

		case WM_IME_STARTCOMPOSITION:
			RsEventHandler( rsIMEStartComposition , NULL );
			return 0L;
		
		case WM_IME_COMPOSITION:
			if (AuInputComposer::GetInstance().OnComposition(window, wParam, lParam))
				return 0L;
			break;

		case WM_IME_NOTIFY				:
			if ( AuInputComposer::GetInstance().OnNotify(window, wParam, lParam) )
				return 0L;
			break;

		case WM_IME_ENDCOMPOSITION		:
			RsEventHandler( rsIMEEndComposition , NULL );
			if ( AuInputComposer::GetInstance().OnEndComposition(window, wParam, lParam) )
				return 0L;
			break;

        case WM_SYSKEYDOWN:
			{
				RsKeyStatus ks;

				if (!(lParam & 0x40000000)) /* ignore repeat events */
				{
					ks.keyScanCode = winTranslateKey(wParam, lParam);
					ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];

					RsKeyboardEventHandler(rsKEYDOWN, &ks);
				}
				else
				{
					return -1L;
				}

				return 0L;
			}

        case WM_SYSKEYUP:
			{
				RsKeyStatus ks;

				ks.keyScanCode = winTranslateKey(wParam, lParam);
				ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];

				RsKeyboardEventHandler(rsKEYUP, &ks);

				return 0L;
			}

        case WM_ACTIVATE:
			{
				if (LOWORD(wParam) == WA_INACTIVE)
				{
	#ifdef RWMOUSE
					ClipCursor(NULL);
	#endif
					SetTimer(window, 1, 20, NULL);
					ForegroundApp = FALSE;

					RsEventHandler(rsACTIVATE, (void *)1);
				}
				else
				{
	#ifdef RWMOUSE
					ClipMouseToWindow(window);
	#endif
					KillTimer(window, 1);
					ForegroundApp = TRUE;
					RsEventHandler(rsACTIVATE, (void *)0);
				}

				return 0L;
			}

		case WM_SETFOCUS:
			{
				RsEventHandler(rsACTIVATE, (void *)0);
				return 0L;
			}
		case WM_KILLFOCUS:
			{
				RsEventHandler(rsACTIVATE, (void *)2);
				return 0L;
			}

        case WM_TIMER:
			{
				if (RwInitialized)
				{
					RsEventHandler(rsIDLE, NULL);
				}

				return 0L;
			}

        case WM_CLOSE:
        case WM_DESTROY:
			{
				/*
				 * Quit message handling.
				 */
				ClipCursor(NULL);

				PostQuitMessage(0);

				return 0L;
			}

#ifdef RWSPLASH
            /* 
             * If we're playing the splash screen the wave driver may
             * need to tell us something, so pass it on.
             */
        case MM_WOM_OPEN:
        case MM_WOM_DONE:
        case MM_WOM_CLOSE:
			{
				aviaudioMessage(window, message, wParam, lParam);

				break;
			}
#endif
        case WM_DROPFILES:
		default:
			AppWindowEventHandler( window, message, wParam, lParam);
			break;
			
		case WM_SETCURSOR:
			return AppWindowEventHandler( window, message, wParam, lParam);
    }

    /*
     * Let Windows handle all other messages.
     */
    LRESULT	hResult	= DefWindowProc(window, message, wParam, lParam);

	return hResult;
}

/*
 *****************************************************************************
 */
RwBool
psSelectDevice(RwBool useDefault __RWUNUSED__)
{
    HWND                hWnd;
    HINSTANCE           hInstance;
    RwVideoMode         vm;
    RwInt32             subSysNum;
    RwInt32             AutoRenderer = 0;

    hWnd = PSGLOBAL(window);
    hInstance = PSGLOBAL(instance);

    GnumSubSystems = RwEngineGetNumSubSystems();
    if (!GnumSubSystems)
    {
        return FALSE;
    }

    /* Just to be sure ... */
    GnumSubSystems =
        (GnumSubSystems > MAX_SUBSYSTEMS) ? MAX_SUBSYSTEMS : GnumSubSystems;

    /* Get the names of all the sub systems */
    for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
    {
        RwEngineGetSubSystemInfo(&GsubSysInfo[subSysNum], subSysNum);
    }

    /* Get the default selection */
    GcurSel = RwEngineGetCurrentSubSystem();

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)

    RWGETWINREGDWORD(AutoRenderer, _T("AutoRenderer"));
    RWGETWINREGDWORD(DisplayClocks, _T("DisplayClocks"));

    /* *INDENT-OFF* */
    _asm
    {
        ;              /* pre-load memory variables into data cache */
        mov            edx, ClocksStart.msb;
        mov            eax, ClocksStart.lsb;

        ;               /*RDTSC - get beginning timestamp to edx:eax */
        _emit 0x0F;
        _emit 0x31;
        ;              /* save beginning timestamp (1 cycle) */
        mov             ClocksStart.msb, edx;
        mov             ClocksStart.lsb, eax;
    }
    /* *INDENT-ON* */

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

    /* Allow the user to choose */
    if (!(AutoRenderer ||
        DialogBox(hInstance,
        MAKEINTRESOURCE(IDD_DEVICESELECT), hWnd, DialogProc)))
    {
        return FALSE;
    }

    /* Set the driver to use the correct sub system */
    if (!RwEngineSetSubSystem(GcurSel))
    {
        return FALSE;
    }

    /* Set up the video mode and set the apps window
    * dimensions to match */
    if (!RwEngineSetVideoMode(GcurSelVM))
    {
        return FALSE;
    }

    RwEngineGetVideoModeInfo(&vm, RwEngineGetCurrentVideoMode());
    if (vm.flags & rwVIDEOMODEEXCLUSIVE)
    {
        RsGlobal.maximumWidth = vm.width;
        RsGlobal.maximumHeight = vm.height;
        PSGLOBAL(fullScreen) = TRUE;
    }

	RwD3D9EngineSetRefreshRate(vm.refRate);

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
    RWGETWINREGDWORD(FrameCount, _T("FrameCount"));
#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

    return TRUE;
}


/*
 *****************************************************************************
 */
#ifdef RWSPLASH
RwBool
psDisplaySplashScreen(RwBool state)
{
    RECT r;
    HDC hdc;

    if (state)
    {
        /* only play if a video mode already exists */
        r.left = 0;
        r.top = 0;
        r.right = RsGlobal.maximumWidth;
        r.bottom = RsGlobal.maximumHeight;

        hdc = GetDC(PSGLOBAL(window));

        PlaySplashScreen(PSGLOBAL(window), hdc, 
            RWSTRING("../../media/win/splash.avi"), &r);

        ReleaseDC(PSGLOBAL(window), hdc);
    }

    return TRUE;
}
#endif


/*
 *****************************************************************************
 */
static RwChar **
CommandLineToArgv(RwChar *cmdLine, RwInt32 *argCount)
{
    RwInt32 numArgs = 0;
    RwBool inArg, inString;
    RwInt32 i, len;
    RwChar *res, *str, **aptr;

    len = strlen(cmdLine);

    /* 
     * Count the number of arguments...
     */
    inString = FALSE;
    inArg = FALSE;

    for(i=0; i<=len; i++)
    {
        if( cmdLine[i] == '"' )
        {
            inString = !inString;
        }

        if( (cmdLine[i] <= ' ' && !inString) || i == len )
        {
            if( inArg ) 
            {
                inArg = FALSE;
                
                numArgs++;
            }
        } 
        else if( !inArg )
        {
            inArg = TRUE;
        }
    }

    /* 
     * Allocate memory for result...
     */
    res = (RwChar *)malloc(sizeof(RwChar *) * numArgs + len + 1);
    str = res + sizeof(RwChar *) * numArgs;
    aptr = (RwChar **)res;

    strcpy(str, cmdLine);

    /*
     * Walk through cmdLine again this time setting pointer to each arg...
     */
    inArg = FALSE;
    inString = FALSE;

    for(i=0; i<=len; i++)
    {
        if( cmdLine[i] == '"' )
        {
            inString = !inString;
        }

        if( (cmdLine[i] <= ' ' && !inString) || i == len )
        {
            if( inArg ) 
            {
                if( str[i-1] == '"' )
                {
                    str[i-1] = '\0';
                }
                else
                {
                    str[i] = '\0';
                }
                
                inArg = FALSE;
            }
        } 
        else if( !inArg && cmdLine[i] != '"' )
        {
            inArg = TRUE; 
            
            *aptr++ = &str[i];
        }
    }

    *argCount = numArgs;

    return (RwChar **)res;
}

//@{ 2006/08/09 burumal
void EnableWindowsShortcutKeys()
{	
	//DisableWindowsWinKey(false, NULL);
	//DisableWindowsStickKeys(false);
	//DisableWindowsFilterKeys(false);	
}

void DisableWindowsShortcutKeys()
{
	/*
#ifdef _DEBUG
	HMODULE hKbHookModule = LoadLibrary("AuKbHookD.DLL");
#else
	HMODULE hKbHookModule = LoadLibrary("AuKbHook.DLL");
#endif

	if ( hKbHookModule )
	{
		DisableWindowsWinKey(true, PSGLOBAL(instance));
		DisableWindowsStickKeys(true);
		DisableWindowsFilterKeys(true);	
	}
	else
		MessageBox(NULL,"AuKbHook dll file not found!","Archlord Error",MB_OK);
		*/
}
//@}

struct FPSManager
{

};

/*
 *****************************************************************************
 */
int PASCAL
WinMain(HINSTANCE instance, 
        HINSTANCE prevInstance  __RWUNUSED__, 
        CMDSTR cmdLine, 
        int cmdShow)
{
    RwV2d pos;
    RwInt32 argc, i;
    RwChar **argv;

	/*
#ifdef _DEBUG
	InitAllocCheck();
#endif // _DEBUG
	*/
	
    RSREGSETBREAKALLOC(_T("RWBREAKALLOC"));

    /* 
     * Initialize the platform independent data.
     * This will in turn initialize the platform specific data...
     */
    if( RsEventHandler(rsINITIALIZE, NULL) == rsEVENTERROR )
    {
        return FALSE;
    }

    /*
     * Register the window class...
     */
    if( !InitApplication(instance) )
    {
        return FALSE;
    }

    /*
     * Get proper command line params, cmdLine passed to us does not
     * work properly under all circumstances...
     */
    cmdLine = GetCommandLine();

    /*
     * Parse command line into standard (argv, argc) parameters...
     */
    argv = CommandLineToArgv(cmdLine, &argc);

	// 패치코드 검사기능을 2009년 07월 23일부로 국내버전에만 적용하도록 수정합니다.
	// 모든 언어버전에 적용하기 위해서는 #ifdef 구문을 제거하면 됩니다.
#ifdef _AREA_KOREA_
	if( !AgcmUILogin::ParsePatchCodeString( argc >= 2 ? argv[ 1 ] : NULL ) )
	{
		::MessageBox( NULL, "Cannot run AlefClient.exe alone.", "Invalid Client running", MB_OK );
		return FALSE;
	}
#endif

    /* 
     * Parse command line parameters (except program name) one at 
     * a time BEFORE RenderWare initialization...
     */
    for(i=1; i<argc; i++)
    {		
        RsEventHandler(rsPREINITCOMMANDLINE, argv[i]);
    }

    /*
     * Create the window...
     */
    PSGLOBAL(window) = InitInstance(instance);
    if( PSGLOBAL(window) == NULL )
    {
        return FALSE;
    }

    PSGLOBAL(instance) = instance;

	//@{ 2006/08/09 burumal	
	DisableWindowsShortcutKeys();
	//@}

#ifdef _USE_DINPUT_
	// DirectInput Setting
	InitDirectInputDevice(PSGLOBAL(window));
#endif // _USE_DINPUT_

    /* 
     * Initialize the 3D (RenderWare) components of the app...
     */
    if( rsEVENTERROR == RsEventHandler(rsRWINITIALIZE, PSGLOBAL(window)) )
    {
        DestroyWindow(PSGLOBAL(window));

        RsEventHandler(rsTERMINATE, NULL);

        return FALSE;

    }

    RwInitialized = TRUE;

    /* 
     * Full screen or not?...
     */
    if( PSGLOBAL(fullScreen) )
    {
        SetWindowLong(PSGLOBAL(window), GWL_STYLE, WS_POPUP);
        SetWindowPos(PSGLOBAL(window), 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_FRAMECHANGED);
    }

    /* 
     * Parse command line parameters (except program name) one at 
     * a time AFTER RenderWare initialization...
     */
    for(i=1; i<argc; i++)
    {
        RsEventHandler(rsCOMMANDLINE, argv[i]);
    }

    /* 
     * Force a camera resize event...
     */
    {
        RwRect r;

        r.x = 0;
        r.y = 0;
        r.w = RsGlobal.maximumWidth;
        r.h = RsGlobal.maximumHeight;

        RsEventHandler(rsCAMERASIZE, &r);
    }

	//@{ 2006/03/02 burumal	
	if ( !PSGLOBAL(fullScreen) )
	{
		RECT rcTemp;
		::GetWindowRect(PsGlobal.window, &rcTemp);

	#ifdef _DEBUG
		::MoveWindow(PsGlobal.window, 0, 0, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, TRUE);
	#else
		::MoveWindow(PsGlobal.window, 
			(GetSystemMetrics(SM_CXSCREEN)-(rcTemp.right - rcTemp.left))/2, (GetSystemMetrics(SM_CYSCREEN)-(rcTemp.bottom - rcTemp.top))/2,
				rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, TRUE);
	#endif
	}
	//@}

    /*
     * Show the window, and refresh it...
     */
    ShowWindow(PSGLOBAL(window), cmdShow);
    UpdateWindow(PSGLOBAL(window));

    /* 
     * Set the initial mouse position...
     */
    pos.x = RsGlobal.maximumWidth * 0.5f;
    pos.y = RsGlobal.maximumHeight * 0.5f;

#ifdef _USE_DINPUT_
	g_stMousePos.x = (UINT32) pos.x;
	g_stMousePos.y = (UINT32) pos.y;
#endif // _USE_DINPUT_

    RsMouseSetPos(&pos);

    /*
     * Enter the message processing loop...
     */
    while( !RsGlobal.quit )
    {
		PROFILE("WinMain - Loop");

        if( PeekMessage(&g_msgLastReceived, NULL, 0U, 0U, PM_REMOVE/*|PM_NOYIELD*/) )
        {
			PROFILE("WinMain - DispatchMessage");

			//GetQueueStatus( QS_KEY | QS_MOUSE | QS_MOUSEBUTTON | QS_MOUSEMOVE )

            if( g_msgLastReceived.message == WM_QUIT )
            {
                break;
            }
            else
            {
#ifdef _USE_DINPUT_
				if (g_msgLastReceived.message == WM_MOUSEMOVE)
					continue;
#endif // _USE_DINPUT_

				// 2004.01.12. steeple
				if(RsIsDialogMessage(&g_msgLastReceived) == FALSE)
				{
					// TranslateMessage(&message);
					DispatchMessage(&g_msgLastReceived);

					//if( GetQueueStatus( QS_KEY | QS_MOUSE | QS_MOUSEBUTTON | QS_MOUSEMOVE ) )
					//{
					//	continue;
					//}
					//else
					//{
					//	break;
					//}
				}
            }
        }
		else if ( TRUE == RwInitialized )
		{
			RsEventHandler( rsIDLE, NULL );
			AgcmFPSManager::AdjustFPS( ForegroundApp );
		}
		/*
        else if( !ForegroundApp )
        {
			PROFILE("WinMain - WaitMessage");

            WaitMessage();
        }
		*/

		/*
//		if( ForegroundApp )
        {
            if ( (int)TRUE == RwInitialized )
            {
#ifdef _USE_DINPUT_
				g_stMousePosPrev = g_stMousePos;

				if (g_bImmediate)
					ReadImmediateDIData();
				else
					ReadBufferedDIData();

				if (g_stMousePosPrev.x != g_stMousePos.x || g_stMousePosPrev.y != g_stMousePos.y)
				{
					POINTS points;

					points.x = g_stMousePos.x;
					points.y = g_stMousePos.y;

					MainWndProc(PSGLOBAL(window), WM_MOUSEMOVE, NULL, (LPARAM) MAKELPARAM(points.x, points.y));
				}
#endif // _USE_DINPUT_

				RsEventHandler( rsIDLE, NULL );

				AgcmFPSManager::AdjustFPS( ForegroundApp );
            }
        }
		*/
    }

    /* 
     * About to shut down - block resize events again...
     */
    RwInitialized = FALSE;

    /*
     * Tidy up the 3D (RenderWare) components of the application...
     */


    RsEventHandler(rsRWTERMINATE, NULL);

#ifdef _USE_DINPUT_
	// Free DirectInputDevice
	FreeDirectInput();
#endif // _USE_DINPUT_

    /*
     * Kill the window...
     */
    DestroyWindow(PSGLOBAL(window));

    /*
     * Free the platform dependent data...
     */
    RsEventHandler(rsTERMINATE, NULL);

    /* 
     * Free the argv strings...
     */
    free(argv);

    return g_msgLastReceived.wParam;
}

/*
 *****************************************************************************
 */

HWND
RsGetWindowHandle()
{
	return PsGlobal.window;
}

#ifdef _USE_DINPUT_
/////////////////// DirectInput

VOID FreeDirectInput()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( g_pMouse ) 
        g_pMouse->Unacquire();
    
    // Release any DirectInput objects.
    SAFE_RELEASE( g_pMouse );
    SAFE_RELEASE( g_pDI );
}

HRESULT InitDirectInputDevice( HWND hWnd )
{
    HRESULT hr;
    DWORD   dwCoopFlags;

    // Cleanup any previous call first
    FreeDirectInput();

    //dwCoopFlags = DISCL_EXCLUSIVE;
    dwCoopFlags = DISCL_NONEXCLUSIVE;

    dwCoopFlags |= DISCL_FOREGROUND;
    //dwCoopFlags |= DISCL_BACKGROUND;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
        return hr;
    
    // Obtain an interface to the system mouse device.
    if( FAILED( hr = g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL ) ) )
        return hr;
    
    // Set the data format to "mouse format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing a
    // DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = g_pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
        return hr;
    
    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pMouse->SetCooperativeLevel( hWnd, dwCoopFlags );
    if( hr == DIERR_UNSUPPORTED )
    {
        FreeDirectInput();
        MessageBox( PSGLOBAL(window), _T("SetCooperativeLevel() returned DIERR_UNSUPPORTED.\n")
                          _T("For security reasons, background exclusive mouse\n")
                          _T("access is not allowed."), 
                          _T("Mouse"), MB_OK );
        return S_OK;
    }

    if( FAILED(hr) )
        return hr;

    if( !g_bImmediate )
    {
        // IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
        //
        // DirectInput uses unbuffered I/O (buffer size = 0) by default.
        // If you want to read buffered data, you need to set a nonzero
        // buffer size.
        //
        // Set the buffer size to SAMPLE_BUFFER_SIZE (defined above) elements.
        //
        // The buffer size is a DWORD property associated with the device.
        DIPROPDWORD dipdw;

        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = g_pMouse->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
            return hr;
    }

    // Acquire the newly created device
    g_pMouse->Acquire();

    return S_OK;
}

HRESULT ReadImmediateDIData()
{
    HRESULT       hr;
    DIMOUSESTATE2 dims2;      // DirectInput mouse state structure

    if( NULL == g_pMouse ) 
        return S_OK;
    
    // Get the input's device state, and put the state in dims
    ZeroMemory( &dims2, sizeof(dims2) );
    hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
    if( FAILED(hr) ) 
    {
        // DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.
        
        // If input is lost then acquire and keep trying 
        hr = g_pMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pMouse->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

	g_stMousePos.x += dims2.lX;
	g_stMousePos.y += dims2.lY;
    /*
    // The dims structure now has the state of the mouse, so 
    // display mouse coordinates (x, y, z) and buttons.
    _stprintf( strNewText, TEXT("(X=% 3.3d, Y=% 3.3d, Z=% 3.3d) B0=%c B1=%c B2=%c B3=%c B4=%c B5=%c B6=%c B7=%c"),
                         dims2.lX, dims2.lY, dims2.lZ,
                        (dims2.rgbButtons[0] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[1] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[2] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[3] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[4] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[5] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[6] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[7] & 0x80) ? '1' : '0');
	*/
    
    return S_OK;
}

HRESULT ReadBufferedDIData()
{
    DIDEVICEOBJECTDATA didod[ SAMPLE_BUFFER_SIZE ];  // Receives buffered data 
    DWORD              dwElements;
    DWORD              i;
    HRESULT            hr;

    if( NULL == g_pMouse ) 
        return S_OK;
    
    dwElements = SAMPLE_BUFFER_SIZE;
    hr = g_pMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA),
                                     didod, &dwElements, 0 );
    if( hr != DI_OK ) 
    {
        // We got an error or we got DI_BUFFEROVERFLOW.
        //
        // Either way, it means that continuous contact with the
        // device has been lost, either due to an external
        // interruption, or because the buffer overflowed
        // and some events were lost.
        //
        // Consequently, if a button was pressed at the time
        // the buffer overflowed or the connection was broken,
        // the corresponding "up" message might have been lost.
        //
        // But since our simple sample doesn't actually have
        // any state associated with button up or down events,
        // there is no state to reset.  (In a real game, ignoring
        // the buffer overflow would result in the game thinking
        // a key was held down when in fact it isn't; it's just
        // that the "up" event got lost because the buffer
        // overflowed.)
        //
        // If we want to be cleverer, we could do a
        // GetDeviceState() and compare the current state
        // against the state we think the device is in,
        // and process all the states that are currently
        // different from our private state.
        hr = g_pMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pMouse->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

    if( FAILED(hr) )  
        return hr;

    // Study each of the buffer elements and process them.
    //
    // Since we really don't do anything, our "processing"
    // consists merely of squirting the name into our
    // local buffer.
    for( i = 0; i < dwElements; i++ ) 
    {
        switch( didod[ i ].dwOfs )
        {
            case DIMOFS_X:
				g_stMousePos.x += didod[ i ].dwData;
				break;

            case DIMOFS_Y:
				g_stMousePos.y += didod[ i ].dwData;
				break;
        }
    }

    return S_OK;
}

#endif // _USE_DINPUT_


#endif // USE_API