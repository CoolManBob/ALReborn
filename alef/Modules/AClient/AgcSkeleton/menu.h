
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

/*****************************************************************
*   Comment  : menu.cpp 클래스화
*                    
*   Date&Time : 2002-04-02, 오전 11:13
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
#define MENUENTRYLENGTHMAX (256)

#define COLUMNSPACING (1)
#define NUMHELPLINESMAX (1500)
#define HELPLINELENGTHMAX (78)
#define AUTOREPEATDELAY (700)
#define AUTOREPEATINTERVAL (50)
#define AUTOREPEATTIMEOUT (1000)
#define DISABLEDBRIGHTNESS (0.2f)
#define SELECTEDBRIGHTNESS (0.5f)
#define SCROLLBRIGHTNESS (0.5f)

#define FIXEDPOINT(a) (RwInt32)((a)*65536.0f)

#ifndef max
#define max(x,y) (((x)>=(y))?(x):(y))
#endif

#ifndef min
#define min(x,y) (((x)<=(y))?(x):(y))
#endif

#ifdef RWLOGO
#define  __RWUNUSEDUNLESSRWLOGO__ /* No op */
#endif /* RWLOGO */

#ifndef __RWUNUSEDUNLESSRWLOGO__
#define  __RWUNUSEDUNLESSRWLOGO__  __RWUNUSED__
#endif /* __RWUNUSEDUNLESSRWLOGO__ */

typedef enum
{
    MENUOFF = 0,
    MENUMODE,
    HELPMODE
}
MenuStateEnum;

typedef RwBool (*MenuTriggerCallBack)(RwBool justCheck);

typedef struct menuEntry MenuEntry;
struct menuEntry
{
        RwInt32     type;
        RwChar     *description;
        void       *target;
        RwInt32     minValue;
        RwInt32     maxValue;
        RwInt32     stepSize;
        const RwChar    **enumStrings; /* for enumerated type integers */
        RwInt32     shortcut;
        MenuTriggerCallBack triggerCallBack;
        MenuEntry  *next;
        MenuEntry  *prev;
};

enum menuEntryType
{
    MENUBOOL,
    MENUBOOLTRANSIENT,
    MENUINT,
    MENUREAL,
    MENUTRIGGER
};
typedef enum menuEntryType MenuEntryType;

typedef struct menuState MenuStateType;
struct menuState
{
        RwBool       isOpen;
        RwRaster    *disabledCharset;
        RwRaster    *inverseCharset;
        RwRaster    *scrollCharset;
        RwRaster    *mainCharset;
        MenuEntry   *activeEntry;
        MenuEntry   *menuList;
        MenuEntry   *startingEntry;
        RwRGBA       backgroundColor;
        RwRGBA       foregroundColor;
        RwUInt32     lastKeyPressed;
        RwInt32      maxDescLength;
        RwInt32      longestEntryLength;
        RwInt32      mode;
        RwInt32      numEntries;
        RwInt32      numEntriesPerColumn;
        RwInt32      helpFileNumEntriesPerColumn;
        RwInt32      numHelpLines;
        RwInt32      topHelpLine;
        RwUInt32     timeOfLastAutoRepeat;
        RwUInt32     timeOfLastKeyPress;
};

typedef struct hsvColor MenuHSVColor;
struct hsvColor
{
        RwReal h;
        RwReal s;
        RwReal v;
};

static MenuStateType MenuState =
{
    FALSE, /* isOpen */
    0,  /* disabledCharset */
    0,  /* inverseCharset */
    0,  /* scrollCharset */
    0,  /* mainCharset */
    0,  /* activeEntry */
    0,  /* menuList */
    0,  /* startingEntry */
    {0,   0,  50,   0}, /* backgroundColor */
    {0, 255, 255, 255}, /* foregroundColor */
    0,  /* lastKeyPressed */
    0,  /* maxDescLength */
    0,  /* longestEntryLength */
    0,  /* mode */
    0,  /* numEntries */
    0,  /* numEntriesPerColumn */
    0,  /* helpFileNumEntriesPerColumn */
    0,  /* numHelpLines */
    0,  /* topHelpLine */
    0,  /* timeOfLastAutoRepeat */
    0   /* timeOfLastKeyPress */
};

#define MENU_H_FILE_NAME_MAX	25

class AcuMenu
{
public: 
	RwChar NoHelpFile[MENU_H_FILE_NAME_MAX];
	RwChar HelpFileName[MENU_H_FILE_NAME_MAX];
	RwChar AltHelpFileName[MENU_H_FILE_NAME_MAX];

	RwChar *helpLines[NUMHELPLINESMAX];
	RwChar EmptyString[1];

 // Indexes used for the scrollable menu...
	RwInt32 SelectedMenuEntry ;//The current selected entry in the dispalyed menu...
	RwInt32 CurrentTopIndex ; // The index to the top entry in the dispalyed menu...
	RwInt32 CurrentBottomIndex ; // The index to the bottom entry in the dispalyed menu...


private:	//static함수였던 녀석들 
	void FreeHelpLines(void);
	//RwBool LoadHelpFile(void);
	RwBool HelpModeTrigger(void);
	void ResetActiveEntry(void);
	void RemoveEntry(MenuEntry *entry);
	MenuEntry *AddNewEntry(void);
	void RgbToHsv(RwRGBAReal *rgb, MenuHSVColor *hsv);
	void HsvToRgb(RwRGBAReal *rgb, MenuHSVColor *hsv);
	void InterpolateColor(RwRGBA *color1, RwRGBA *color2, RwReal value, RwRGBA *color);
	void UpdateLongestEntryLength(MenuEntry *entry);
	MenuEntry *FindByTarget(void *target);
	RwBool ValidateEnumStrings(const RwChar **enumStrings, RwInt32 minValue, RwInt32 maxValue);
	RwBool MenuSelectNext(void);
	RwBool MenuSelectPrevious(void);
	RwBool MenuSelectionAddPercentage(RwReal percentage);
	RwBool MenuSelectionAddValue(RwInt32 increment);
	void CheckHelpFilesBottomLine(void);
	RwBool AutoRepeatPrepare(RsEvent event, void *param);
	RwBool AutoRepeat(void);
	RsEventStatus HandlePadButtonDown(RsEvent __RWUNUSED__ event, void *param);
	RsEventStatus HandlePadButtonUp(RsEvent __RWUNUSED__  event, void *param);
	RsEventStatus HandlePadAnalogueLeft(void *param);
	RsEventStatus MenuHandleKeyDown(void *param);
	RsEventStatus MenuHandleKeyUp(void *param);
	void RenderMenuEntries(RwCamera *camera  __RWUNUSED__,
                  RtCharset *userCharset,
                  RtCharsetDesc *charsetDesc);
	void RenderHelpScreen(RwCamera *camera, RtCharset *charset,
                 RtCharsetDesc *charsetDesc);

public:
	AcuMenu();
	RwBool MenuAddEntryBool(RwChar *description,
                 RwBool *target,
                 MenuTriggerCallBack triggerCallBack);
	RwBool MenuAddEntryBoolTransient(RwChar *description,
                          RwBool *target,
                          MenuTriggerCallBack triggerCallBack);
	RwBool MenuAddEntryInt(RwChar *description,
                RwInt32 *target,
                MenuTriggerCallBack triggerCallBack,
                RwInt32 minValue,
                RwInt32 maxValue,
                RwInt32 stepSize,
                const RwChar **enumStrings);
	RwBool MenuAddEntryReal(RwChar *description,
                 RwReal *target,
                 MenuTriggerCallBack triggerCallBack,
                 RwReal minValue,
                 RwReal maxValue,
                 RwReal stepSize);
	RwBool MenuAddEntryTrigger(RwChar *description,
                    MenuTriggerCallBack triggerCallBack);
	RwBool	MenuAddSeparator(void);
	RwBool	MenuClose(void);
	RwInt32	MenuGetStatus(void);
	RsEventStatus	MenuKeyboardHandler(RsEvent event, void *param);
	RsEventStatus	MenuKeyboardShortcutHandler(RsEvent event, void *param);
	RsEventStatus	MenuMouseHandler(RsEvent event, void *param);
	RwBool	MenuOpen(RwBool createCharset, RwRGBA *foreground, RwRGBA *background);
	RsEventStatus	MenuPadHandler(RsEvent event, void *param);
	RwBool	MenuRemoveEntry(void *target);
	RwBool	MenuRender(RwCamera *camera, RwRaster *Charset);
	RwBool	MenuSelectEntry(void *target);
	RwBool	MenuSetRangeInt(RwInt32 *target,
                RwInt32 minValue,
                RwInt32 maxValue,
                RwInt32 stepSize,
                const RwChar **enumStrings);
	RwBool	MenuSetRangeReal(RwReal *target,
                 RwReal minValue,
                 RwReal maxValue,
                 RwReal stepSize);
	RwBool	MenuSetStatus(RwInt32 newMode);
	RwBool	MenuToggle(void);

};




#endif /* MENU_H */

