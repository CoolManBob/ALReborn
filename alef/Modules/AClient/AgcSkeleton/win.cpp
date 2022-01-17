#ifdef USE_MFC

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

//#include "stdafx.h"

#include "ApBase.h"
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
#include "platform.h"
#include "skeleton.h"
#include "mouse.h"
//#include "vecfont.h"
#include "win.h"

#ifdef RWSPLASH
/* Splash screen */
#include "splash.h"
#endif

#define MAX_SUBSYSTEMS (16)

#if (!defined(_MAX_PATH))

/*
 * See 
 * Program Files/Microsoft Visual Studio/VC98/Include/STDLIB.H
 */
#ifndef _MAC
#define _MAX_PATH   260		/* max. length of full pathname */
#else /* def _MAC */
#define _MAX_PATH   256		/* max. length of full pathname */
#endif /* _MAC */
#endif /* (!defined(_MAX_PATH)) */

typedef struct _win_int_64 WinInt64;
struct _win_int_64
{
	RwInt32			 msb;
	RwUInt32			lsb;
};

#define WinInt64Sub(_result, _end, _start)			  \
do													  \
{													   \
	(_result).lsb = (_end).lsb - (_start).lsb;		  \
	if ((_end).msb < (_start).msb)					  \
	{												   \
		(_result).msb = (_end).msb - (_start).msb - 1;  \
	}												   \
	else												\
	{												   \
		(_result).msb = (_end).msb - (_start).msb;	  \
	}												   \
}													   \
while (0)

#define doubleFromWinInt64(_x)									 \
   ( ((double)((_x).msb))*((double)(1<<16))*((double)(1<<16))	  \
	  + ((double)((_x).lsb)) )

static RwBool	   ForegroundApp = TRUE;
static RwSubSystemInfo GsubSysInfo[MAX_SUBSYSTEMS];
static RwInt32	  GnumSubSystems = 0;
static RwInt32	  GcurSel = 0, GcurSelVM = 0;
static RwInt32	  FrameCount = 0;

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)
static RwBool	   DisplayClocks = FALSE;
static WinInt64	 ClocksStart = { 0, 0 };
static WinInt64	 ClocksEnd = { 0, 0 };
#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

AuTickCounter	g_csTickCounter;

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

psGlobalType PsGlobal;


#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

#ifdef UNDER_CE
#ifndef MAKEPOINTS
#define MAKEPOINTS(l) (*((POINTS FAR *) & (l)))
#endif
#endif

MSG g_msgLastReceived;

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
	//Grab a handle to the passed window's device context:
	//This is included to maintain non-mfc compatibility, ignoring the OnPaint() default DC
	PAINTSTRUCT psPaint;
	HDC hDC = BeginPaint( PsGlobal.window , &psPaint);
	
	try  {
//		 //Set up the colour struct
//		 RwRGBA black = {0, 0, 0, 0};
//   
//		 //Clear the camera and report on fail
//		 if ( !RwCameraClear(camera, &black, rwCAMERACLEARIMAGE | rwCAMERACLEARZ) )
//		 {
//			OutputDebugString("CRenderWare::Render: Failed to clear camera.\n");
//			return;
//		 }


		 //Finish painting and close device context
		 RwCameraShowRaster( camera, PsGlobal.window, 0x0 );   
	}
	
	catch (...)
	{
		OutputDebugString("CRenderWare::Render: Exception occurred.\n");
		OutputDebugString("\tPossibly due to old clump requiring skin and bone animation.\n");
	}
	
	EndPaint( PsGlobal.window , &psPaint );

	return;
}


/*
 *****************************************************************************
 */
RwUInt32
psTimer(void)
{
	RwUInt32 time;
//	TIMECAPS timecaps;

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
	dstBuffer = (RwChar *)RwMalloc(sizeof(RwChar) * (rwstrlen(srcBuffer) + 1), rwMEMHINTDUR_GLOBAL);

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
	RwChar	  curDirBuffer[_MAX_PATH]; 
	RwUInt32	retValue; 
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

			ret = GetDriveType(deviceName); 

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
RwInt32
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
	windowClass.style = 0;
#else
	windowClass.style = CS_BYTEALIGNWINDOW;
#endif

	windowClass.lpfnWndProc		= (WNDPROC) MainWndProc;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= (HINSTANCE)instance;
	windowClass.hIcon			= NULL;
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

	rect.left	= rect.top = 0;
	rect.right	= RsGlobal.maximumWidth;
	rect.bottom	= RsGlobal.maximumHeight;

#ifdef UNDER_CE
	{
		DWORD			   style, exStyle;

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

	return TRUE;
}


/*
 *****************************************************************************
 */
void
psTerminate(void)
{
    RtFileSystem* fs; 
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
	const RwD3D8Metrics	   *d3d8Metrics =
		(const RwD3D8Metrics *) metrics->devSpecificMetrics;

	if (d3d8Metrics)
	{
		RwChar			  message[200];

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

// 테이블때문에 -_-;
int GetKeyTableEnglish( int index )
{
	return KeyTableEnglish[ index ];
}

HWND
RsGetWindowHandle()
{
	return PsGlobal.window;
}

#endif // USE_MFC