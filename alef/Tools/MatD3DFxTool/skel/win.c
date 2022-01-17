
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
#define WINVER 0x0400

#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>

#if (defined(_MSC_VER))
#include <tchar.h>
#endif /* (defined(_MSC_VER)) */

#include <stdio.h>

#include "rwcore.h"
#include "rtbmp.h"
#include "rtfsyst.h"

#include "resource.h"
#include "skeleton.h"
#include "platform.h"
#include "mouse.h"
//#include "vecfont.h"
#include "win.h"

#ifdef RWSPLASH
/* Splash screen */
#include "splash.h"
#endif

#define MAX_SUBSYSTEMS      (16)

/* 
 * Warning: set MAX_NB_FILES_PER_FS to the appropriate value.
 *
 * MAX_NB_FILES_PER_FS limits the number of files that can be concurrently
 * opened by the file system(s) registered. 
 * On Windows a file size is 96 bytes. 
 */
#define MAX_NB_FILES_PER_FS (20)   
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

#ifndef UNDER_CE
static TIMECAPS     TimeCaps;
#endif
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


/* Class name for the MS Window's window class. */

static const TCHAR *AppClassName = L"RwAppClass";

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

typedef struct
{
    HWND        window;
    HINSTANCE   instance;
    RwBool      fullScreen;
    RwV2d       lastMousePos;
}
psGlobalType;

//@{ Jaewon 20041104
// no static
psGlobalType PsGlobal;
//@} Jaewon


#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

#ifdef UNDER_CE
#ifndef MAKEPOINTS
#define MAKEPOINTS(l) (*((POINTS FAR *) & (l)))
#endif
#endif

extern MsgProcCallBack				MsgProcCB;

/*
 *****************************************************************************
 */
HWND
psWindowGetHandle(void)
{

    return(PSGLOBAL(window));
}

/*
 *****************************************************************************
 */
void
psWindowSetText(const RwChar *text)
{
#ifdef UNICODE
	wchar_t wbuf[256];
	mbstowcs(wbuf, text, strlen(text)+1);
#else
	const RwChar *wbuf = text;
#endif
    SetWindowText(PSGLOBAL(window), wbuf);

    return;
}


/*
 *****************************************************************************
 */
void
psErrorMessage(const RwChar *message)
{
#ifdef UNICODE
	wchar_t wbuf[256];
	mbstowcs(wbuf, message, strlen(message)+1);
#else
	const RwChar *wbuf = message;
#endif
    OutputDebugString(L"Error: ");
    OutputDebugString(wbuf);
    OutputDebugString(L"\n");

    return;
}


/*
 *****************************************************************************
 */
void
psWarningMessage(const RwChar *message)
{
#ifdef UNICODE
	wchar_t wbuf[256];
	mbstowcs(wbuf, message, strlen(message)+1);
#else
	const RwChar *wbuf = message;
#endif
    OutputDebugString(L"Warning: ");
    OutputDebugString(wbuf);
    OutputDebugString(L"\n");

    return;
}


/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera *camera)
{
    RwCameraShowRaster(camera, PSGLOBAL(window), 0);

    if ((0 < FrameCount) && (0 == --FrameCount))
    {
#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
        if (DisplayClocks)
        {
            const static WinInt64 ClocksBias = { 0, 13 + 1 };
            WinInt64            ClocksElapsed;
            TCHAR               title[256];
            TCHAR               buffer[256];
            RwInt32             ClocksEnd_msb;
            RwUInt32            ClocksEnd_lsb;

            /* *INDENT-OFF* */
            _asm
            {
                ;              /* RDTSC - get ending timestamp to edx:eax */
                ;              /* (13 cycles) */
                _emit 0x0F;
                _emit 0x31;
                ;              /* save ending timestamp */
                mov            ClocksEnd_msb, edx;
                mov            ClocksEnd_lsb, eax;
            }
            /* *INDENT-ON* */      

            ClocksEnd.msb = ClocksEnd_msb;
            ClocksEnd.lsb = ClocksEnd_lsb;
            

            WinInt64Sub(ClocksElapsed, ClocksEnd, ClocksStart);
            WinInt64Sub(ClocksElapsed, ClocksElapsed, ClocksBias);

            _sntprintf(title, sizeof(title),
                       L"%s(%d)", __FILE__, __LINE__);
            _sntprintf(buffer, sizeof(buffer),
                       L"%24.0f Elapsed Clocks",
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

#ifdef UNDER_CE
    time = (RwUInt32) GetTickCount();
#else
    time = (RwUInt32) timeGetTime();
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

    if (ForegroundApp)
    {
        validDelta = FALSE;

        point.x = (RwInt32) pos->x;
        point.y = (RwInt32) pos->y;

        ClientToScreen(PSGLOBAL(window), &point);

        SetCursorPos(point.x, point.y);
    }

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
    dstBuffer = (RwChar *)RwMalloc(sizeof(RwChar) * (rwstrlen(srcBuffer) + 1),
                                   rwID_NAOBJECT);

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
RwBool
psInstallFileSystem(void)
{
    TCHAR       curDirBuffer[_MAX_PATH];
    RwUInt32    retValue;
    RtFileSystem *wfs, *unc;

    RwUInt32 drivesMask;
    RwInt32 drive;
    RwChar  fsName[2];
#ifdef UNICODE
	char buf[256];
#else
	const char *buf;
#endif
    
    /* get the current directory */
    retValue = GetCurrentDirectory(_MAX_PATH, curDirBuffer);
    if (0 == retValue || retValue > _MAX_PATH)
    {
        return FALSE;
    }

    /* Register a unc file system */
    
    /* tag a \ to the end of the current directory */
    /* only fails if the buffer size is exhausted */
    _tcscat( curDirBuffer, L"\\" );

    /** Setup the file system manager */
#ifdef UNICODE
	wcstombs(buf, curDirBuffer, wcslen(curDirBuffer)+1);
#else
	buf = curDirBuffer;
#endif
    if ((unc = RtWinFSystemInit(MAX_NB_FILES_PER_FS, buf, "unc")) != NULL)
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
#ifdef UNICODE
			wchar_t wbuf[256];
#else
			const RwChar *wbuf;
#endif

            deviceName[0] = drive + 'A' - 1;
            deviceName[1] = ':';
            deviceName[2] = '\\';   /* Needed for Win98 */
            deviceName[3] = '\0';

#ifdef UNICODE
			mbstowcs(wbuf, deviceName, strlen(deviceName)+1);
#else
			wbuf = deviceName;
#endif
            ret = GetDriveType(wbuf);

            if ( ret != DRIVE_UNKNOWN && ret != DRIVE_NO_ROOT_DIR )
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
#ifdef UNICODE
						char cc;
						wctomb(&cc, curDirBuffer[0]);
#else
						char cc = curDirBuffer[0];
#endif
                        /* Set the unc file system as default if we have a unc path */
                        if (curDirBuffer[1] != L':')
                        {
                            RtFSManagerSetDefaultFileSystem(unc);
                        }
                        else if (deviceName[0] == cc)
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
#ifdef UNICODE
	wchar_t wbuf[256];
	mbstowcs(wbuf, str, strlen(str)+1);
#else
	const RwChar *wbuf = str;
#endif
    OutputDebugString(wbuf);
    OutputDebugString(L"\n");

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

    WNDCLASS windowClass;

#ifdef UNDER_CE
    windowClass.style = 0;
#else
    windowClass.style = CS_BYTEALIGNWINDOW;
#endif
    windowClass.lpfnWndProc = (WNDPROC) MainWndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = (HINSTANCE)instance;
	//@{ Jaewon 20040908
    windowClass.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_MAIN));
	//@} Jaewon
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = AppClassName;

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

        style = WS_BORDER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
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
                        WS_OVERLAPPEDWINDOW,
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

#ifndef UNDER_CE
    timeGetDevCaps(&TimeCaps, sizeof(TIMECAPS));
    timeBeginPeriod(TimeCaps.wPeriodMin);
#endif

    return TRUE;
}


/*
 *****************************************************************************
 */
void
psTerminate(void)
{

    timeEndPeriod(TimeCaps.wPeriodMin);
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

        sprintf(message, "VertexBufferSwitches = %d",
                                    d3d8Metrics->numVBSwitches);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;   
    }

#elif (defined (D3D9_DRVMODEL_H))
    const RwD3D9Metrics       *d3d9Metrics =
        (const RwD3D9Metrics *) metrics->devSpecificMetrics;

    if (d3d9Metrics)
    {
        RwChar              message[200];

        sprintf(message, "RenderStateChanges = %d",
                                    d3d9Metrics->numRenderStateChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;

        sprintf(message, "TextureStateChanges = %d",
                                    d3d9Metrics->numTextureStageStateChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;   

        sprintf(message, "SamplerStateChanges = %d",
                                    d3d9Metrics->numSamplerStageStateChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;   

        sprintf(message, "MaterialChanges = %d",
                                    d3d9Metrics->numMaterialChanges);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;

        sprintf(message, "LightsChanged = %d",
                                    d3d9Metrics->numLightsChanged);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;   

        sprintf(message, "VertexBufferSwitches = %d",
                                    d3d9Metrics->numVBSwitches);
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
#ifdef UNICODE
		wchar_t wbuf[256];
#else
		char *wbuf;
#endif

        RwEngineGetVideoModeInfo(&vidModemInfo, vidMode);

        rwsprintf(modeStr, RWSTRING("%lu x %lu x %lu %s"),
                  vidModemInfo.width, vidModemInfo.height,
                  vidModemInfo.depth,
                  vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE ?
                  RWSTRING("(Fullscreen)") : RWSTRING(""));

#ifdef UNICODE
		mbstowcs(wbuf, modeStr, strlen(modeStr)+1);
#else
		wbuf = modeStr;
#endif
        /* Add name and an index so we can ID it later */
        index =
            SendMessage(wndListVideMode, CB_ADDSTRING, 0, (LPARAM) wbuf);
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
#ifdef UNICODE
		wchar_t wbuf[256];
		mbstowcs(wbuf, GsubSysInfo[subSysNum].name, strlen(GsubSysInfo[subSysNum].name)+1);
#else
		char *wbuf = GsubSysInfo[subSysNum].name;
#endif
        SendMessage(wndList, CB_ADDSTRING, 0,
                    (LPARAM) wbuf);
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

#if defined(OPENGL_DRVMODEL_H)
    static DEVMODE  fullScreenDevMode;
#endif /* defined(OPENGL_DRVMODEL_H) */
    
	if(MsgProcCB)
	{
		BOOL bNoFurtherProcessing;
		LRESULT lresult = MsgProcCB(window, message, wParam, lParam, &bNoFurtherProcessing);
		if(bNoFurtherProcessing)
			return lresult;
	}

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

            return 0L;
        }

#ifdef RWMOUSE
        case WM_MOVE:
        {
            ClipMouseToWindow(window);
            
            return 0L;
        }
#endif  /* RWMOUSE */

        case WM_DROPFILES:
        {
            TCHAR  path[_MAX_PATH];
            RwInt32 i, numFiles;
            HDROP   drop = (HDROP)wParam;
            RwChar *uncFsName = "uncd";

            RtFileSystem *defaultFS = RtFSManagerGetDefaultFileSystem();

            numFiles = DragQueryFile(drop, (UINT)(-1), path, _MAX_PATH);
            
            for (i = numFiles - 1; i >= 0; --i)
            {
                RwInt32 j = 0;

                /* Support for dropping remote files */
                RtFileSystem *fs;
                RwUInt32 nIndex;
                RwBool found = FALSE;

                DragQueryFile(drop, (UINT)i, path, _MAX_PATH);
                
                /* Check for a colon or the third back slash 
                 * This is to account for both local and UNC 
                 * path. This assumes that all file name dropped
                 * are absolute.
                 */
                for (nIndex = 0; nIndex < _tcslen(path); nIndex++)
                {
                    if (path[nIndex] == L':')
                    {
                        found = TRUE;
                        break;
                    }

                    /* also try to get the 3rd back slash */ 
                    if (path[nIndex] == L'\\')
                    {
                        j++;
                        if (j == 3)
                        {
                            found = TRUE;
                            break;
                        }
                    }
                }
                
                /* if !found the path format is not supported */
                if (found)
                {
                    /* 
                     * Try to register this file system.
                     * This will fail if it has already been 
                     * registered 
                     */
                    fs = RtWinFSystemInit(MAX_NB_FILES_PER_FS, "", uncFsName);
                    if (fs)
                    {
                        RtFSManagerRegister(fs);
                        RtFSManagerSetDefaultFileSystem(fs);
                    }
                    else
                    {
                        RtFileSystem *unc = RtFSManagerGetFileSystemFromName(uncFsName);
                        RtFSManagerSetDefaultFileSystem(unc);
                    }
                
                    /* now load file */
                    RsEventHandler(rsFILELOAD, path);
                }
                
                j = 0;
            }

            /* Set previously defaulted file system */
            RtFSManagerSetDefaultFileSystem(defaultFS);

            DragFinish(drop);

            return 0L;
        }
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

            points = MAKEPOINTS(lParam);
            ms.pos.x = points.x;
            ms.pos.y = points.y;
            ms.shift = (wParam & MK_SHIFT) ? TRUE : FALSE;
            ms.control = (wParam & MK_CONTROL) ? TRUE : FALSE;

            SetCapture(window);

            RsMouseEventHandler(rsLEFTBUTTONDOWN, &ms);

            return 0L;
        }

        case WM_RBUTTONDOWN:
        {
            RsMouseStatus ms;

            points = MAKEPOINTS(lParam);
            ms.pos.x = points.x;
            ms.pos.y = points.y;
            ms.shift = (wParam & MK_SHIFT) ? TRUE : FALSE;
            ms.control = (wParam & MK_CONTROL) ? TRUE : FALSE;

            SetCapture(window);

            RsMouseEventHandler(rsRIGHTBUTTONDOWN, &ms);

            return 0L;
        }

        case WM_MOUSEWHEEL:
        {
            RwBool forward = FALSE;

            forward = ((short)HIWORD(wParam) < 0) ? FALSE : TRUE;

            RsMouseEventHandler(rsMOUSEWHEELMOVE, (void *)&forward);

            return 0L;
        }

        case WM_MOUSEMOVE:
        {
            if (ForegroundApp)
            {
                points = MAKEPOINTS(lParam);

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

            RsMouseEventHandler(rsLEFTBUTTONUP, NULL);

            return 0L;
        }

        case WM_RBUTTONUP:
        {
            ReleaseCapture();

            RsMouseEventHandler(rsRIGHTBUTTONUP, NULL);

            return 0L;
        }

        case WM_KEYDOWN:
        {
            RsKeyStatus ks;

            if (!(lParam & 0x40000000)) /* ignore repeat events */
            {
                ks.keyScanCode = winTranslateKey(wParam, lParam);
                ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];
                RsKeyboardEventHandler(rsKEYDOWN, &ks);

                if (ks.keyCharCode == rsESC)
                {
                    /* Send a quit message - this allows app to do stuff */
                    RsEventHandler(rsQUITAPP, NULL);
                }
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

        case WM_SYSKEYDOWN:
        {
            RsKeyStatus ks;

            if (!(lParam & 0x40000000)) /* ignore repeat events */
            {
                ks.keyScanCode = winTranslateKey(wParam, lParam);
                ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];

                RsKeyboardEventHandler(rsKEYDOWN, &ks);
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

#if defined(OPENGL_DRVMODEL_H)
        case WM_DISPLAYCHANGE:
            {
                /* take a note of the fullscreen display mode that is
                 * made in the driver */
                if ( (FALSE != PSGLOBAL(fullScreen)) &&
                     (FALSE != ForegroundApp) &&
                     (FALSE == RwInitialized) )
                {
                    EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS,
                                         &fullScreenDevMode );
                }

                return 0L;
            }
#endif /* defined(OPENGL_DRVMODEL_H) */            

        case WM_ACTIVATE:
        {
#if defined(OPENGL_DRVMODEL_H)
            RwBool          Iconified;


            Iconified = HIWORD(wParam) ? TRUE : FALSE;
#endif /* defined(OPENGL_DRVMODEL_H) */

            if (LOWORD(wParam) == WA_INACTIVE)
            {
#ifdef RWMOUSE
                ClipCursor(NULL);
#endif

#if defined(OPENGL_DRVMODEL_H)
                /* if we are in fullscreen mode we need to iconify */
                if( FALSE != PSGLOBAL(fullScreen) )
                {
                    /* do we need to manually iconify? */
                    if( FALSE == Iconified )
                    {
                        /* minimize window */
                        CloseWindow( window );
                    }

                    /* change display settings to the desktop resolution */
                    ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
                }
#endif /* defined(OPENGL_DRVMODEL_H) */

                SetTimer(window, 1, 20, NULL);
                ForegroundApp = FALSE;

                RsEventHandler(rsACTIVATE, (void *)FALSE);
            }
            else
            {
#ifdef RWMOUSE
                ClipMouseToWindow(window);
#endif

#if defined(OPENGL_DRVMODEL_H)
                /* must be performed _before_ ForegroundApp is reset to TRUE */

                /* if we are in fullscreen mode we need to maximize */
                if( FALSE != PSGLOBAL(fullScreen) )
                {
                    if ( FALSE == ForegroundApp )
                    {
                        /* change display settings to the desktop resolution */
                        ChangeDisplaySettings( &fullScreenDevMode,
                                               CDS_FULLSCREEN );
                    }

                    /* do we need to manually restore window? */
                    if( FALSE != Iconified )
                    {
                        /* activate window */
                        ShowWindow( window, SW_SHOW );
                        SetForegroundWindow( window );
                        SetFocus( window );
                    }
                }
#endif /* defined(OPENGL_DRVMODEL_H) */

                KillTimer(window, 1);
                ForegroundApp = TRUE;
                RsEventHandler(rsACTIVATE, (void *)TRUE);
            }

            return 0L;
        }

        case WM_TIMER:
        {
            if (RwInitialized)
            {
				//@{ Jaewon 20041103
                RsEventHandler(rsIDLE, NULL);
				//@} Jaewon
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
    }

    /*
     * Let Windows handle all other messages.
     */
    return DefWindowProc(window, message, wParam, lParam);
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
    
#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
    RwInt32             ClocksStart_msb;
    RwUInt32            ClocksStart_lsb;
#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */


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

    ClocksStart.msb = ClocksStart_msb;
    ClocksStart.lsb = ClocksStart_lsb;

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

    /* Allow the user to choose */
    if (!(AutoRenderer ||
        DialogBox(hInstance,
        MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc)))
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

        SetWindowLong(PSGLOBAL(window), GWL_STYLE, WS_POPUP);
        SetWindowPos(PSGLOBAL(window), 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_FRAMECHANGED);
    }
    else
    {
        RECT rect;

        GetClientRect(hWnd, &rect);

        RsGlobal.maximumWidth = rect.right;
        RsGlobal.maximumHeight = rect.bottom;
    }

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

#error Insert name of the splash avi file here
        PlaySplashScreen(PSGLOBAL(window), hdc, 
            RWSTRING("splash.avi"), &r);

        ReleaseDC(PSGLOBAL(window), hdc);
    }

    return TRUE;
}
#endif

/*
 *****************************************************************************
 */
static void *
HMalloc(size_t size, RwUInt32 hint __RWUNUSED__)
{
    return (malloc(size));
}

/*
 *****************************************************************************
 */
static void *
HRealloc(void *orgMem, size_t newSize, RwUInt32 hint __RWUNUSED__)
{
    return (realloc(orgMem, newSize));
}

/*
 *****************************************************************************
 */
static void *
HCalloc(size_t numObj, size_t sizeObj, RwUInt32 hint __RWUNUSED__)
{
    return (calloc(numObj, sizeObj));
}

/*
 *****************************************************************************
 */
RsEventStatus
PsEventHandler(RsEvent event, void *param)
{
    RsEventStatus       result = rsEVENTNOTPROCESSED;

    switch (event)
    {
        case rsSETMEMORYFUNCS:
            {
                /* We shouldn't mark this event as rsEVENTPROCESSED
                 * since we want the app to be able to override/chain
                 * the memory functions.
                 */
                RwMemoryFunctions *memFuncs;
                
                memFuncs = (RwMemoryFunctions* )param;
                memFuncs->rwmalloc  = HMalloc;
                memFuncs->rwfree    = free;
                memFuncs->rwrealloc = HRealloc;
                memFuncs->rwcalloc  = HCalloc;

                break;
            }
        default:
            break;
    }
    
    return result;
}

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

/*
 *****************************************************************************
 */
int PASCAL
WinMain(HINSTANCE instance, 
        HINSTANCE prevInstance  __RWUNUSED__, 
        CMDSTR cmdLine0, 
        int cmdShow)
{
    MSG message;
    RwV2d pos;
    RwInt32 argc, i;
    TCHAR **argv;
	TCHAR *cmdLine;

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
    argv = CommandLineToArgvW(cmdLine, &argc);


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

    RsMouseSetPos(&pos);

    /*
     * Enter the message processing loop...
     */
    while( !RsGlobal.quit )
    {
        if( PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE|PM_NOYIELD) )
        {
            if( message.message == WM_QUIT )
            {
                break;
            }
            else
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
		//@{ Jaewon 20040908
		//@{ Jaewon 20041103
        else if( ForegroundApp )
		//@} Jaewon
		//@} Jaewon
        {
            if ( FALSE != RwInitialized )
            {
                RsEventHandler( rsIDLE, NULL );
            }
        }
        else
        {
            WaitMessage();
        }
    }

    /* 
     * About to shut down - block resize events again...
     */
    RwInitialized = FALSE;

    /*
     * Tidy up the 3D (RenderWare) components of the application...
     */
    RsEventHandler(rsRWTERMINATE, NULL);

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
    GlobalFree(argv);

    RWCRTDUMPMEMORYLEAKS();

    return message.wParam;
}

/*
 *****************************************************************************
 */
