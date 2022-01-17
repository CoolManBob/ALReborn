
/****************************************************************************
 *
 * skeleton.h
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
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

#ifndef SKELETON_H
#define SKELETON_H

#include "ApBase.h"

#include "rwcore.h"

#include "rtcharse.h"

#if (defined(__MWERKS__) && (defined (__R5900__) || defined (macintosh)) )
#define RsSprintf sprintf
#endif /* (defined(__MWERKS__) && (defined (__R5900__) || defined (macintosh)) ) */

#if (!defined(RsSprintf))
#define RsSprintf rwsprintf
#endif /* (!defined(RsSprintf)) */

/* Use this define to get
 * MENUING(no path added to the original path file) behavior */
/* #define MENUING */

/* Use this define to get 
 * DEBUGSTATION behavior */
/* #define DEBUGSTATION */

/* Use this define to add
 * CwComUtil (2.7 and after) compatibility for debugstation use*/
/* #define CWDEBUGAT */

/* Use this define to get
 * CDROM behavior */
/* #define CDROM */

#ifdef RWTERMINAL
#include "terminal.h"
#endif

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

#ifdef DEBUGSTATION
#ifdef CWDEBUGAT

extern void
cwDbgPrintf(const char *fmt, ...);

#define printf cwDbgPrintf

#endif
#endif

#if (defined(RWDEBUG))

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)

#if (!defined(_XBOX))
#include <windows.h>
#endif /* (!defined(_XBOX)) */

#if (!defined(UNDER_CE))
#include <crtdbg.h>
#endif /* (!defined(UNDER_CE)) */

#define RsAssert(_condition) _ASSERTE(_condition)

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

#if (!defined(RsAssert))
#include <assert.h>
#define RsAssert(_condition) assert(_condition)
#endif /* (!defined(RsAssert)) */

#define RSASSERT(_condition)                      \
do                                                \
{                                                 \
    if (!(_condition))                            \
    {                                             \
        _rwDebugSendMessage(rwDEBUGMESSAGE,       \
                            RWSTRING(__FILE__),   \
                            __LINE__,             \
                            "Assertion failed:",  \
                            #_condition);         \
    }                                             \
    RsAssert((_condition));                       \
}                                                 \
while (0)

#endif /* (defined(RWDEBUG)) */

#if (!defined(RSASSERT))
#define RSASSERT(_condition) /* No-op */
#endif /* (!defined(RSASSERT)) */

#define RSASSERTISTYPE(_f, _t) \
   RSASSERT( (!(_f)) || ((((const RwObject *)(_f))->type)==(_t)) )

enum RsInputDeviceType
{
    rsKEYBOARD,
    rsMOUSE,
    rsPAD
};
typedef enum RsInputDeviceType RsInputDeviceType;

enum RsEventStatus
{
    rsEVENTERROR,
    rsEVENTPROCESSED,
    rsEVENTNOTPROCESSED
};
typedef enum RsEventStatus RsEventStatus;

enum RsEvent
{
#ifdef RWSPLASH
    rsDISPLAYSPLASH,
#endif
    rsCAMERASIZE,
    rsCOMMANDLINE,
    rsFILELOAD,
    rsINITDEBUG,
    rsINPUTDEVICEATTACH,
    rsLEFTBUTTONDOWN,
    rsLEFTBUTTONUP,
    rsMOUSEMOVE,
    rsMOUSEWHEELMOVE,
    rsPLUGINATTACH,
    rsREGISTERIMAGELOADER,
    rsRIGHTBUTTONDOWN,
    rsRIGHTBUTTONUP,
    rsRWINITIALIZE,
    rsRWTERMINATE,
    rsSELECTDEVICE,
    rsINITIALIZE,
    rsTERMINATE,
    rsIDLE,
    rsKEYDOWN,
    rsKEYUP,
    rsQUITAPP,
    rsPADBUTTONDOWN,
    rsPADBUTTONUP,
    rsPADANALOGUELEFT,
    rsPADANALOGUELEFTRESET,
    rsPADANALOGUERIGHT,
    rsPADANALOGUERIGHTRESET,
    rsPREINITCOMMANDLINE,
    rsACTIVATE
};
typedef enum RsEvent RsEvent;

typedef RsEventStatus (*RsInputEventHandler)(RsEvent event, void *param);

typedef struct RsInputDevice RsInputDevice;
struct RsInputDevice
{
    RsInputDeviceType inputDeviceType;
    RwBool used;
    RsInputEventHandler inputEventHandler;
};

typedef struct RsGlobalType RsGlobalType;
struct RsGlobalType
{
    const RwChar *appName;
    RwInt32 maximumWidth;
    RwInt32 maximumHeight;
    RwInt32 maxFPS;
    RwBool  quit;

    void   *ps; /* platform specific data */

#ifdef RWTERMINAL
    RsTerminal *terminal;
#endif

    RsInputDevice keyboard;
    RsInputDevice mouse;
    RsInputDevice pad;
};

typedef struct RsMouseStatus RsMouseStatus;
struct RsMouseStatus
{
    RwV2d   pos;
    RwV2d   delta;
    RwBool  shift;
    RwBool  control;
};

enum RsKeyCodes
{
    rsESC       = 128,

    rsF1        = 129,
    rsF2        = 130,
    rsF3        = 131,
    rsF4        = 132,
    rsF5        = 133,
    rsF6        = 134,
    rsF7        = 135,
    rsF8        = 136,
    rsF9        = 137,
    rsF10       = 138,
    rsF11       = 139,
    rsF12       = 140,

    rsINS       = 141,
    rsDEL       = 142,
    rsHOME      = 143,
    rsEND       = 144,
    rsPGUP      = 145,
    rsPGDN      = 146,

    rsUP        = 147,
    rsDOWN      = 148,
    rsLEFT      = 149,
    rsRIGHT     = 150,

    rsPADINS    = 151,
    rsPADDEL    = 152,
    rsPADHOME   = 153,
    rsPADEND    = 154,
    rsPADPGUP   = 155,
    rsPADPGDN   = 156,

    rsPADUP     = 157,
    rsPADDOWN   = 158,
    rsPADLEFT   = 159,
    rsPADRIGHT  = 160,

    rsNUMLOCK   = 161,
    rsDIVIDE    = 162,
    rsTIMES     = 163,
    rsMINUS     = 164,
    rsPLUS      = 165,
    rsPADENTER  = 166,
    rsPAD5      = 167,

    rsBACKSP    = 168,
    rsTAB       = 169,
    rsCAPSLK    = 170,
    rsENTER     = 171,
    rsLSHIFT    = 172,
    rsRSHIFT    = 173,
    rsLCTRL     = 174,
    rsRCTRL     = 175,
    rsLALT      = 176,
    rsRALT      = 177,

    rsNULL       = 255
};
typedef enum RsKeyCodes RsKeyCodes;

typedef struct RsKeyStatus RsKeyStatus;
struct RsKeyStatus
{
    RwInt32     keyScanCode;
    RwInt32     keyCharCode;
};

typedef struct RsPadButtonStatus RsPadButtonStatus;
struct RsPadButtonStatus
{
    RwInt32     padID;
    RwUInt32    padButtons;
};

enum RsPadButtons
{
    rsPADDPADLEFT   = 0x00000001,
    rsPADDPADRIGHT  = 0x00000002,
    rsPADDPADUP     = 0x00000004,
    rsPADDPADDOWN   = 0x00000008,
    rsPADSTART      = 0x00000010,
    rsPADSELECT     = 0x00000020,
    rsPADBUTTON1    = 0x00000040,
    rsPADBUTTON2    = 0x00000080,
    rsPADBUTTON3    = 0x00000100,
    rsPADBUTTON4    = 0x00000200,
    rsPADBUTTON5    = 0x00000400,
    rsPADBUTTON6    = 0x00000800,
    rsPADBUTTON7    = 0x00001000,
    rsPADBUTTON8    = 0x00002000,
    rsPADBUTTONA1   = 0x00004000,
    rsPADBUTTONA2   = 0x00008000
};
typedef enum RsPadButtons RsPadButtons;

enum RsPrintPos
{
    rsPRINTPOSMIDDLE    = 0,

    rsPRINTPOSLEFT   = 1,
    rsPRINTPOSRIGHT  = 2,
    rsPRINTPOSTOP    = 4,
    rsPRINTPOSBOTTOM = 8,

    rsPRINTPOSTOPLEFT     = rsPRINTPOSTOP    | rsPRINTPOSLEFT,
    rsPRINTPOSTOPRIGHT    = rsPRINTPOSTOP    | rsPRINTPOSRIGHT,
    rsPRINTPOSBOTTOMLEFT  = rsPRINTPOSBOTTOM | rsPRINTPOSLEFT,
    rsPRINTPOSBOTTOMRIGHT = rsPRINTPOSBOTTOM | rsPRINTPOSRIGHT,

    rsPRINTPOSFORCENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RsPrintPos RsPrintPos;

/* Screen margins in charset width/height units, used w/ RsPrintPos */
enum RsPrintMargin
{
    rsPRINTMARGINMIDDLE = 0,

#if ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)))
    rsPRINTMARGINTOP    = 2,
    rsPRINTMARGINBOTTOM = 4,
    rsPRINTMARGINLEFT   = 5,
    rsPRINTMARGINRIGHT  = 5,
#else /* ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)) */
    rsPRINTMARGINTOP    = 1,
    rsPRINTMARGINBOTTOM = 1,
    rsPRINTMARGINLEFT   = 1,
    rsPRINTMARGINRIGHT  = 1,
#endif /* ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)) */
    rsPRINTMARGINFORCENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RsPrintMargin RsPrintMargin;

extern RsGlobalType RsGlobal;

extern RsEventStatus AppEventHandler(RsEvent event, void *param);
extern RwBool        AttachInputDevices(void);

extern RsEventStatus RsEventHandler(RsEvent event, void *param);
extern RsEventStatus RsKeyboardEventHandler(RsEvent event, void *param);
extern RsEventStatus RsMouseEventHandler(RsEvent event, void *param);
extern RsEventStatus RsPadEventHandler(RsEvent event, void *param);

extern RwBool                   
RsInitialize(void);

extern RwBool                   
RsRegisterImageLoader(void);

extern RwBool                   
RsRwInitialize(void *param);

extern RwBool                   
RsSelectDevice(void);

extern RwBool                   
RsInputDeviceAttach(RsInputDeviceType inputDevice,
                    RsInputEventHandler inputEventHandler);

extern RwBool                   
RsAlwaysOnTop(RwBool alwaysOnTop);

extern RwBool               
RsSetModelTexturePath(const RwChar *pathName);

extern RwChar                   
RsPathGetSeparator(void);

extern RwChar *                 
RsPathnameCreate(const RwChar *srcBuffer);

extern RwUInt32                 
RsTimer(void);

extern RwUInt8                  
RsKeyFromScanCode(RwUInt8 scan, RwBool shiftKeyDown);

extern void                     
RsCameraShowRaster(RwCamera *camera);

extern void                     
RsErrorMessage(const RwChar *text);

extern void                     
RsMouseSetVisibility(RwBool visible);

extern void                     
RsMouseSetPos(RwV2d *pos);

extern void                     
RsPathnameDestroy(RwChar *buffer);

extern void                     
RsRwTerminate(void);

extern void                     
RsTerminate(void);

extern void                     
RsWarningMessage(const RwChar *text);

extern void                     
RsWindowSetText(const RwChar *text);

extern RwBool 
RsLoadPresetViews(void);

extern RwBool 
RsSetPresetView(RwCamera *camera, RwInt32 viewNum);

extern RwBool 
RsSetNextPresetView(RwCamera *camera);

extern RwBool 
RsSetPreviousPresetView(RwCamera *camera);

extern RwBool 
RsSavePresetView(RwCamera *camera);

extern RwChar *
RsGetPresetViewDescription(void);

extern RwBool 
RsDestroyPresetViews(void);

extern RwImage *
RsGrabScreen(RwCamera *camera);

extern RtCharset *
RsCharsetPrint(RtCharset *charSet,
               const RwChar *string,
               RwInt32 x, RwInt32 y,
               RsPrintPos corner);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* SKELETON_H */
