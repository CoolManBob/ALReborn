
/****************************************************************************
 *
 * menu.h
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

#ifndef MENU_H
#define MENU_H

#include "rwcore.h"

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

typedef enum
{
    MENUOFF = 0,
    MENUMODE,
    HELPMODE
}
MenuStateEnum;

typedef RwBool (*MenuTriggerCallBack)(RwBool justCheck);

extern RwBool
MenuAddEntryBool(RwChar *description,
                 RwBool *target,
                 MenuTriggerCallBack triggerCallBack);

extern RwBool
MenuAddEntryBoolTransient(RwChar *description,
                          RwBool *target,
                          MenuTriggerCallBack triggerCallBack);

extern RwBool
MenuAddEntryInt(RwChar *description,
                RwInt32 *target,
                MenuTriggerCallBack triggerCallBack,
                RwInt32 minValue,
                RwInt32 maxValue,
                RwInt32 stepSize,
                const RwChar **enumStrings);

extern RwBool
MenuAddEntryReal(RwChar *description,
                 RwReal *target,
                 MenuTriggerCallBack triggerCallBack,
                 RwReal minValue,
                 RwReal maxValue,
                 RwReal stepSize);

extern RwBool
MenuAddEntryTrigger(RwChar *description,
                    MenuTriggerCallBack triggerCallBack);

extern RwBool
MenuAddSeparator(void);

extern RwBool
MenuClose(void);

extern RwInt32
MenuGetStatus(void);

extern RsEventStatus
MenuKeyboardHandler(RsEvent event, void *param);

extern RsEventStatus
MenuKeyboardShortcutHandler(RsEvent event, void *param);

extern RsEventStatus
MenuMouseHandler(RsEvent event, void *param);

extern RwBool
MenuOpen(RwBool createCharset, RwRGBA *foreground, RwRGBA *background);

extern RsEventStatus
MenuPadHandler(RsEvent event, void *param);

extern RwBool
MenuRemoveEntry(void *target);

extern RwBool
MenuRender(RwCamera *camera, RwRaster *Charset);

extern RwBool
MenuSelectEntry(void *target);

extern RwBool
MenuSetRangeInt(RwInt32 *target,
                RwInt32 minValue,
                RwInt32 maxValue,
                RwInt32 stepSize,
                const RwChar **enumStrings);

extern RwBool
MenuSetRangeReal(RwReal *target,
                 RwReal minValue,
                 RwReal maxValue,
                 RwReal stepSize);

extern RwBool
MenuSetStatus(RwInt32 newMode);

extern RwBool
MenuToggle(void);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* MENU_H */

