/****************************************************************************
 *
 * menu.c
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

#include <string.h>

#include "rwcore.h"
#include "rtcharse.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "skeleton.h"
#include "menu.h"

#ifdef RWMOUSE
#include "mouse.h"
#endif

#if defined(WIN32) && !defined(_XBOX)
#define WITH_SHORTCUT
#define WITH_KEYBOARD
#endif





/*
 *****************************************************************************
 */
void AcuMenu::FreeHelpLines(void)
{
    while( MenuState.numHelpLines > 0 )
    {
        RwFree(helpLines[--MenuState.numHelpLines]);

        helpLines[MenuState.numHelpLines] = 0;
    }
}

/*
 *****************************************************************************
 */
RwBool AcuMenu::LoadHelpFile(void)
{
    RwChar *path;
    RwFileFunctions *fileFuncs;
    RwChar line[HELPLINELENGTHMAX+1];
    void *fp;
    RwInt32 length, i;

    length = rwstrlen(NoHelpFile);

    helpLines[MenuState.numHelpLines] = (RwChar *)RwMalloc(length + 1);

    if( helpLines[MenuState.numHelpLines] )
    {
        rwstrcpy(helpLines[MenuState.numHelpLines], NoHelpFile);

        MenuState.topHelpLine = MenuState.numHelpLines++;
    }

    fileFuncs = RwOsGetFileInterface();
    if( !fileFuncs )
    {
        return FALSE;
    }

    path = RsPathnameCreate(RWSTRING(HelpFileName));
    fp = fileFuncs->rwfopen(path, RWSTRING("r"));
    if( !fp )
    {
        RsPathnameDestroy(path);
        path = RsPathnameCreate(RWSTRING(AltHelpFileName));
        fp = fileFuncs->rwfopen(path, RWSTRING("r"));
        if( !fp )
        {
            RsPathnameDestroy(path);

            return FALSE;
        }
    }

    /*
     * Load help lines from the file...
     */
    FreeHelpLines();

    while( fileFuncs->rwfgets(line, HELPLINELENGTHMAX, fp) )
    {
        length = rwstrlen(line);

        for(i = 0; i < length; i++)
        {
            if( (line[i] & 128) || line[i] < 32 )
            {
                line[i] = ' ';
            }
        }

        helpLines[MenuState.numHelpLines] = (RwChar *)RwMalloc(length + 1);

        if( !helpLines[MenuState.numHelpLines] )
        {
            RsErrorMessage(RWSTRING("Out of memory - not all of help loaded"));
            RsPathnameDestroy(path);
            return FALSE;
        }

        rwstrcpy(helpLines[MenuState.numHelpLines], line);

        MenuState.numHelpLines++;
    }

    fileFuncs->rwfclose(fp);

    RsPathnameDestroy(path);

    if( MenuState.numHelpLines == 0 )
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool AcuMenu::HelpModeTrigger(void)
{
    static RwInt32 exitMode;

    if( MenuState.mode != HELPMODE )
    {
        /*if( 0 == MenuState.numHelpLines )
          {
          LoadHelpFile();
          }*/

        exitMode = MenuState.mode;

        MenuState.mode = HELPMODE;
    }
    else
    {
        MenuState.mode = exitMode;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSetStatus(RwInt32 newMode)
{
    /*
     * Returns FALSE if the new MenuState.mode is invalid...
     */

    if( newMode == MenuState.mode )
    {
        /*
         * No need to change anything...
         */
        return TRUE;
    }

    if( newMode == HELPMODE )
    {
        /*
         * MenuState.mode to return to on exit from help...
         */
        MenuState.mode = MENUOFF;

        return HelpModeTrigger();
    }
    else if( newMode == MENUOFF )
    {
        MenuState.mode = newMode;

        return TRUE;
    }
    else if( newMode == MENUMODE )
    {
        /*
         * Menu MenuState.mode only allowed if entries have been added...
         */
        if( MenuState.menuList)
        {
            MenuState.mode = newMode;
        }
        else
        {
            MenuSetStatus(HELPMODE);
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*
 *****************************************************************************
 */
RwInt32
AcuMenu::MenuGetStatus(void)
{
    return MenuState.mode;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuToggle(void)
{
    /*
     * Returns FALSE on failure...
     */
    return MenuSetStatus((MenuState.mode + 1) % 3);
}

/*
 *****************************************************************************
 */
void AcuMenu::ResetActiveEntry(void)
{
    /*
     * Reset active entry to be first menu option...
     */
    for(MenuState.activeEntry = MenuState.menuList;
        MenuState.activeEntry && MenuState.activeEntry->next;
        MenuState.activeEntry = MenuState.activeEntry->next)
    {
        ;
    }

    SelectedMenuEntry = 0;

}

/*
 *****************************************************************************
 */
void AcuMenu::RemoveEntry(MenuEntry *entry)
{
    if( entry->prev )
    {
        entry->prev->next = entry->next;
    }

    if( entry->next )
    {
        entry->next->prev = entry->prev;
    }

    if( entry == MenuState.menuList )
    {
        MenuState.menuList = entry->next;
    }

    if( entry == MenuState.activeEntry )
    {
        ResetActiveEntry();
    }
}

/*
 *****************************************************************************
 */
MenuEntry *AcuMenu::AddNewEntry(void)
{
    MenuEntry *newEntry = (MenuEntry *)RwMalloc(sizeof(MenuEntry));

    if( newEntry )
    {
        newEntry->next = MenuState.menuList;
        newEntry->prev = 0;

        if( MenuState.menuList )
        {
            MenuState.menuList->prev = newEntry;
        }
        else
        {
            /*
             * First entry - make it the active one by default...
             */
            MenuState.activeEntry = newEntry;
        }

        MenuState.menuList = newEntry;

        MenuState.numEntries++;

    }

    return newEntry;
}

/*
 *****************************************************************************
 */
void AcuMenu::RgbToHsv(RwRGBAReal *rgb, MenuHSVColor *hsv)
{
    RwInt32 h = FIXEDPOINT(-1.0f);
    RwInt32 s, vee;
    RwInt32 low, red, green, blue;

	/*
         P4 integer divide by zero fix variables
    */
    RwReal  delta;                          /* small change to compare colours with */

    RwInt32 delta_fp;                       /* fixed point representation of delta */


    red = FIXEDPOINT(rgb->red);
    green = FIXEDPOINT(rgb->green);
    blue = FIXEDPOINT(rgb->blue);

    vee = max(red, max(green, blue));
    low = min(red, min(green, blue));

	/*
         P4 integer divide by zero fix code
    */
    delta = (RwReal)(0.05f * 65536.0f);

    delta_fp = (RwInt32)(delta);

    /*
         P4 integer divide by zero problem

         FIXEDPOINT(0.05f) produces 0 (incorrect value)
         in debug build on a P4

         the expanded version produces 3276 (correct value)
    */
    /*
    if( (vee - low) >= FIXEDPOINT(0.05f) )
	*/
	if ( (vee - low) >= delta_fp )
    {
        s = ((vee - low) << 8) / ((vee + 128) >> 8);

        if( vee == red )
        {
            h = ((green - blue) << 8) / ((vee - low + 128) >> 8);
        }
        else if( vee == green )
        {
            h = FIXEDPOINT(2.0f) + ((blue - red) << 8) /
                ((vee - low + 128) >> 8);
        }
        else if( vee == blue )
        {
            h = FIXEDPOINT(4.0f) + ((red - green) << 8) /
                ((vee - low + 128) >> 8);
        }

        /*
         * Handle periodicity (faster than modulus)...
         */
        if( h < 0 )
        {
            h += FIXEDPOINT(6.0f);
        }
        else if( h >= FIXEDPOINT(6.0f) )
        {
            h -= FIXEDPOINT(6.0f);
        }

        hsv->h = h * (1.0f / 6.0f / 65535.0f);
        hsv->s = s * (1.0f / 65535.0f);
        hsv->v = vee * (1.0f / 65535.0f);
    }
    else
    {
        hsv->h = -1.0f;
        hsv->s = 0.0f;
        hsv->v = vee * (1.0f / 65535.0f);
    }

    return;
}

/*
 *****************************************************************************
 */
void AcuMenu::HsvToRgb(RwRGBAReal *rgb, MenuHSVColor *hsv)
{
    RwReal f, p1, p2, p3;
    int i;
    RwReal h, s, v;

    h = hsv->h;
    s = hsv->s;
    v = hsv->v;

    if( h < 0.0f )
    {
        h = 0.0f;
    }
    else if( h > 1.0f )
    {
        h = 1.0f;
    }

    h *= 6.0f;
    i = (RwInt32)h;
    f = h - i;
    p1 = v * (1.0f - s);
    p2 = v * (1.0f - s * f);
    p3 = v * (1.0f - s * (1.0f - f));

    switch( i )
    {
        case 0:
            rgb->red = v;
            rgb->green = p3;
            rgb->blue = p1;
            break;

        case 1:
            rgb->red = p2;
            rgb->green = v;
            rgb->blue = p1;
            break;

        case 2:
            rgb->red = p1;
            rgb->green = v;
            rgb->blue = p3;
            break;

        case 3:
            rgb->red = p1;
            rgb->green = p2;
            rgb->blue = v;
            break;

        case 4:
            rgb->red = p3;
            rgb->green = p1;
            rgb->blue = v;
            break;

        case 5:
            rgb->red = v;
            rgb->green = p1;
            rgb->blue = p2;
            break;
    }

    rgb->alpha = 1.0f;

    return;
}

/*
 *****************************************************************************
 */
void AcuMenu::InterpolateColor(RwRGBA *color1, RwRGBA *color2, RwReal value, RwRGBA *color)
{
    RwRGBAReal rgb;
    MenuHSVColor hsv, hsv1, hsv2;

    RwRGBARealFromRwRGBA(&rgb, color1);
    RgbToHsv(&rgb, &hsv1);

    RwRGBARealFromRwRGBA(&rgb, color2);
    RgbToHsv(&rgb, &hsv2);

    hsv.h = hsv1.h;
    hsv.s = hsv1.s * 0.6f;
    hsv.v = value * hsv1.v + (1.0f - value) * hsv2.v;

    HsvToRgb(&rgb, &hsv);
    RwRGBAFromRwRGBAReal(color, &rgb);

    return;
}

/*
 *****************************************************************************
 */
void AcuMenu::UpdateLongestEntryLength(MenuEntry *entry)
{
    RwChar temp[MENUENTRYLENGTHMAX];
    RwInt32 i;
    RwInt32 descLength;
    RwInt32 valueLength;
    RwInt32 totalLength;

    switch( entry->type )
    {
        case MENUBOOL:
        case MENUBOOLTRANSIENT:
            {
                valueLength = 3;
                break;
            }
        case MENUINT:
            {
                if( entry->enumStrings )
                {
                    RwInt32 length = 0;
                    valueLength = 0;
                    for(i = entry->minValue; i <= entry->maxValue; i++)
                    {
                        if( entry->enumStrings[i - entry->minValue] &&
                            entry->enumStrings[i - entry->minValue][0] )
                        {
                            rwsprintf(temp, RWSTRING("%s"),
                                      entry->enumStrings[i - entry->minValue]);
                        }
                        else
                        {
                            temp[0] = '\0';
                        }

                        length = rwstrlen(temp);

                        if( length > valueLength )
                        {
                            valueLength = length;
                        }
                    }
                }
                else
                {
                    RwInt32 minlength = 0;
                    valueLength = 0;

                    /*
                     * Usually max length will be longer...
                     */
                    rwsprintf(temp, RWSTRING("%d"), entry->maxValue);
                    valueLength = rwstrlen(temp);

                    rwsprintf(temp, RWSTRING("%d"), entry->minValue);
                    minlength = rwstrlen(temp);

                    if( minlength > valueLength )
                    {
                        valueLength = minlength;
                    }
                }
                break;
            }
        case MENUREAL:
            {
                RwInt32 minLength;
                rwsprintf(temp, RWSTRING("%0.2f"), *(RwReal *)&(entry->maxValue));
                valueLength = rwstrlen(temp);
                rwsprintf(temp, RWSTRING("%0.2f"), *(RwReal *)&(entry->minValue));
                minLength = rwstrlen(temp);
                if( minLength > valueLength )
                {
                    valueLength = minLength;
                }
                break;
            }
        case MENUTRIGGER:
            {
                rwsprintf(temp, RWSTRING("  <"));
                valueLength = rwstrlen(temp);
                break;
            }
        default:
            {
                valueLength = 0;
                break;
            }
    }

    /*
     * Description and keyboard shortcut...
     */
    descLength = rwstrlen(entry->description);

    if( descLength >= 2 &&
        entry->description[descLength-2] == '_' &&
        entry->description[descLength-1] > 31 )
    {
        entry->shortcut = entry->description[descLength - 1];
        descLength -= 2;
        entry->description[descLength] = '\0';

#if (0 && defined(WITH_SHORTCUT))
        /*
         * Allow for the shortcut letter surrounded by '[' and ']',
         * and two spaces...
         */
        descLength = maxDescLength - descLength + 5;
        length += 5;
#endif

    }
    else if( descLength >= 2 &&
             entry->description[descLength - 2] == 'F' &&
             entry->description[descLength - 1] >= '1' &&
             entry->description[descLength - 1] <= '9' )
    {
        entry->shortcut = rsF1 + (entry->description[descLength - 1] - '1');
        descLength -= 2;
        entry->description[descLength] = '\0';

#if(0 && defined(WITH_SHORTCUT))
        /*
         * Allow for the two shortcut letters (eg F1) surrounded
         * by '[' and ']', and a space...
         */
        descLength = maxDescLength - descLength + 5;
        length += 5;
#endif

    }
    else
    {
        /* entry->shortcut = '\0'; */
    }

#ifdef WITH_SHORTCUT
    /*
     * Find the longest description length...
     */
    {
        MenuEntry *ptr;
        RwBool change = FALSE;

        for(ptr = MenuState.menuList; ptr; ptr = ptr->next)
        {
            RwInt32 length;
            length = rwstrlen(ptr->description);
            if( length > MenuState.maxDescLength)
            {
                MenuState.maxDescLength = length;
                change = TRUE;
            }
        }
        /*
         * If maxDescLength changes then have to update all other menu entry lengths...
         */
        if( change )
        {
            for(ptr = MenuState.menuList; ptr; ptr = ptr->next)
            {
                UpdateLongestEntryLength(ptr);
            }
        }
    }

    /*
     * Allow for a space after the description, plus a leading and trailing
     * space in the menu item display string...
     */
    totalLength = MenuState.maxDescLength + valueLength + 3;

#else

    /*
     * Allow for a space after the description, plus a leading and trailing
     * space in the menu item display string...
     */
    totalLength = descLength + valueLength + 3;

#endif /* WITH_SHORTCUT */

#if(defined(WITH_SHORTCUT))
    /*
     * Allow for the two shortcut letters (eg F1) surrounded
     * by '[' and ']', and a space...
     */
    totalLength += 5;
#endif /* WITH_SHORTCUT */

    if( totalLength > MenuState.longestEntryLength )
    {
        MenuState.longestEntryLength = totalLength;
    }

    return;
}

/*
 *****************************************************************************
 */
MenuEntry *AcuMenu::FindByTarget(void *target)
{
    MenuEntry *ptr = 0;

    if( target)
    {
        for(ptr = MenuState.menuList; ptr; ptr = ptr->next)
        {
            if( ptr->target == target || (void *)ptr->triggerCallBack == target )
            {
                break;
            }
        }
    }

    return ptr;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuRemoveEntry(void *target)
{
    MenuEntry *ptr = FindByTarget(target);

    if( ptr )
    {
        RemoveEntry(ptr);

        RwFree(ptr);

        MenuState.numEntries--;

        return TRUE;
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuAddEntryBool(RwChar *description, RwBool *target,
                 MenuTriggerCallBack triggerCallBack)
{
    MenuEntry *newEntry;

    if( target && description )
    {
        newEntry = AddNewEntry();

        if( newEntry )
        {
            newEntry->type = MENUBOOL;
            newEntry->description = description;
            newEntry->target = target;
            newEntry->maxValue = 1;
            newEntry->minValue = 0;
            newEntry->stepSize = 1;
            newEntry->triggerCallBack = triggerCallBack;
            newEntry->shortcut = '\0';

            UpdateLongestEntryLength(newEntry);

            return TRUE;
        }
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuAddEntryBoolTransient(RwChar *description, RwBool *target,
                          MenuTriggerCallBack triggerCallBack)
{
    MenuEntry *newEntry;

    if( target && description )
    {
        newEntry = AddNewEntry();

        if( newEntry )
        {
            newEntry->type = MENUBOOLTRANSIENT;
            newEntry->description = description;
            newEntry->target = target;
            newEntry->maxValue = 1;
            newEntry->minValue = 0;
            newEntry->stepSize = 1;
            newEntry->triggerCallBack = triggerCallBack;
            newEntry->shortcut = '\0';

            UpdateLongestEntryLength(newEntry);

            return TRUE;
        }
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool AcuMenu::ValidateEnumStrings(const RwChar **enumStrings, RwInt32 minValue, RwInt32 maxValue)
{
    RwInt32 i;

    if( enumStrings )
    {
        for(i = minValue; i <= maxValue; i++)
        {
            if( enumStrings[i - minValue] == NULL )
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuAddEntryInt(RwChar *description, RwInt32 *target,
                MenuTriggerCallBack triggerCallBack,
                RwInt32 minValue, RwInt32 maxValue,
                RwInt32 stepSize, const RwChar **enumStrings)
{
    MenuEntry *newEntry;

    if( target && description && (minValue <= maxValue) &&
        ValidateEnumStrings(enumStrings, minValue, maxValue) )
    {
        newEntry = AddNewEntry();

        if( newEntry )
        {
            newEntry->type = MENUINT;
            newEntry->description = description;
            newEntry->target = target;
            newEntry->maxValue = maxValue;
            newEntry->minValue = minValue;
            newEntry->stepSize = stepSize;
            newEntry->triggerCallBack = triggerCallBack;
            newEntry->enumStrings = enumStrings;
            newEntry->shortcut = '\0';

            UpdateLongestEntryLength(newEntry);

            return TRUE;
        }
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuAddSeparator(void)
{
    MenuEntry *newEntry;

    newEntry = AddNewEntry();

    if( newEntry )
    {
        newEntry->type = MENUINT;
        newEntry->description = EmptyString;
        newEntry->target = NULL;
        newEntry->maxValue = 0;
        newEntry->minValue = 0;
        newEntry->triggerCallBack = 0;
        newEntry->enumStrings = (const RwChar **)&EmptyString;
        newEntry->shortcut = '\0';

        UpdateLongestEntryLength(newEntry);
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuAddEntryReal(RwChar *description, RwReal *target,
                 MenuTriggerCallBack triggerCallBack,
                 RwReal minValue, RwReal maxValue, RwReal stepSize)
{
    MenuEntry *newEntry;

    if( target && description && (minValue <= maxValue) )
    {
        newEntry = AddNewEntry();

        if(newEntry )
        {
            newEntry->type = MENUREAL;
            newEntry->description = description;
            newEntry->target = target;
            newEntry->maxValue = *(RwInt32 *)&maxValue;
            newEntry->minValue = *(RwInt32 *)&minValue;
            newEntry->stepSize = *(RwInt32 *)&stepSize;
            newEntry->triggerCallBack = triggerCallBack;
            newEntry->shortcut = '\0';

            UpdateLongestEntryLength(newEntry);

            return TRUE;
        }
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuAddEntryTrigger(RwChar *description, MenuTriggerCallBack triggerCallBack)
{
    MenuEntry *newEntry;

    if( description )
    {
        newEntry = AddNewEntry();

        if( newEntry )
        {
            newEntry->type = MENUTRIGGER;
            newEntry->description = description;
            newEntry->triggerCallBack = triggerCallBack;
            newEntry->shortcut = '\0';

            UpdateLongestEntryLength(newEntry);

            return TRUE;
        }
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSelectEntry(void *target)
{
    MenuState.activeEntry = FindByTarget(target);

    return (MenuState.activeEntry == NULL) ? FALSE : TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSetRangeReal(RwReal *target, RwReal minValue, RwReal maxValue,
                 RwReal stepSize)
{
    MenuEntry *entry = FindByTarget(target);

    if( entry && target && (minValue <= maxValue) )
    {
        entry->maxValue = *(RwInt32 *)&maxValue;
        entry->minValue = *(RwInt32 *)&minValue;
        entry->stepSize = *(RwInt32 *)&stepSize;

        if( *target > maxValue )
        {
            *target = maxValue;
        }
        else if( *target < minValue )
        {
            *target = minValue;
        }

        UpdateLongestEntryLength(entry);

        return TRUE;
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSetRangeInt(RwInt32 *target, RwInt32 minValue, RwInt32 maxValue,
                RwInt32 stepSize, const RwChar **enumStrings)
{
    MenuEntry *entry = FindByTarget(target);

    if( entry && target && (minValue <= maxValue) )
    {
        if( ValidateEnumStrings(enumStrings, minValue, maxValue) )
        {
            entry->maxValue = maxValue;
            entry->minValue = minValue;
            entry->stepSize = stepSize;
            entry->enumStrings = enumStrings;

            if( *target > maxValue )
            {
                *target = maxValue;
            }
            else if( *target < minValue )
            {
                *target = minValue;
            }

            UpdateLongestEntryLength(entry);

            return TRUE;
        }
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
#ifndef OLDMENU

RwBool
AcuMenu::MenuSelectNext(void)
{
    if( !MenuState.activeEntry || !MenuState.menuList )
    {
        return FALSE;
    }

    if( MenuState.activeEntry == MenuState.menuList )
    {
        return FALSE;

#if(0)
        /*
         * To wrap from the bottom menu entry to the top entry...
         */
        ResetActiveEntry();

        SelectedMenuEntry = 0;
#endif /* 0 */
    }
    else
    {
        MenuState.activeEntry = MenuState.activeEntry->prev;

        SelectedMenuEntry++;
    }

    if( MenuState.activeEntry == MenuState.startingEntry )
    {
        return FALSE;
    }

    /*
     * Separator (an enumerated type where the first value string is blank)
     * or disabled menu entry: Skip it...
     */
    if( ( MenuState.activeEntry->type == MENUINT &&
          MenuState.activeEntry->enumStrings &&
          *MenuState.activeEntry->enumStrings == '\0'  )
        ||
        ( MenuState.activeEntry->triggerCallBack &&
          !MenuState.activeEntry->triggerCallBack( TRUE ) ) )
    {
        return MenuSelectNext();
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSelectPrevious(void)
{
    if( !MenuState.activeEntry || !MenuState.menuList )
    {
        return FALSE;
    }

    if( MenuState.activeEntry->next == NULL )
    {
        return FALSE;
#if(0)
        /*
         * To wrap from the top menu entry to the bottom entry...
         */
        MenuState.activeEntry = MenuState.menuList;

        SelectedMenuEntry = MenuState.numEntries - 1;
#endif /* 0 */
    }
    else
    {
        MenuState.activeEntry = MenuState.activeEntry->next;

        SelectedMenuEntry--;
    }

    if( MenuState.activeEntry == MenuState.startingEntry )
    {
        return FALSE;
    }

    /*
     * Separator (an enumerated type where the first value string is blank)
     * or disabled menu entry: Skip it...
     */
    if( ( MenuState.activeEntry->type == MENUINT &&
          MenuState.activeEntry->enumStrings &&
          *MenuState.activeEntry->enumStrings == '\0'  )
        ||
        ( MenuState.activeEntry->triggerCallBack &&
          !MenuState.activeEntry->triggerCallBack( TRUE ) ) )
    {
        return MenuSelectPrevious();
    }

    return TRUE;
}

#else

RwBool
MenuSelectNext(void)
{
    if( !MenuState.activeEntry || !MenuState.menuList )
    {
        return FALSE;
    }

    if( MenuState.activeEntry == MenuState.menuList )
    {
        ResetActiveEntry();
    }
    else
    {
        MenuState.activeEntry = MenuState.activeEntry->prev;
    }

    if( MenuState.activeEntry == MenuState.startingEntry )
    {
        return FALSE;
    }

    /*
     * Separator (an enumerated type where the first value string is blank)
     * or disabled menu entry: Skip it...
     */
    if( ( MenuState.activeEntry->type == MENUINT &&
          MenuState.activeEntry->enumStrings &&
          *MenuState.activeEntry->enumStrings == '\0'  )
        ||
        ( MenuState.activeEntry->triggerCallBack &&
          !MenuState.activeEntry->triggerCallBack( TRUE ) ) )
    {
        return MenuSelectNext();
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSelectPrevious(void)
{
    if( !MenuState.activeEntry || !MenuState.menuList )
    {
        return FALSE;
    }

    if( MenuState.activeEntry->next == NULL )
    {
        MenuState.activeEntry = MenuState.menuList;
    }
    else
    {
        MenuState.activeEntry = MenuState.activeEntry->next;
    }

    if( MenuState.activeEntry == MenuState.startingEntry )
    {
        return FALSE;
    }

    /*
     * Separator (an enumerated type where the first value string is blank)
     * or disabled menu entry: Skip it...
     */
    if( ( MenuState.activeEntry->type == MENUINT &&
          MenuState.activeEntry->enumStrings &&
          *MenuState.activeEntry->enumStrings == '\0'  )
        ||
        ( MenuState.activeEntry->triggerCallBack &&
          !MenuState.activeEntry->triggerCallBack( TRUE ) ) )
    {
        return MenuSelectPrevious();
    }

    return TRUE;
}
#endif /* OLDMENU */

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSelectionAddPercentage(RwReal percentage)
{
    RwReal value;
    RwReal minValue, maxValue;

    if( !MenuState.activeEntry )
    {
        return FALSE;
    }

    switch( MenuState.activeEntry->type )
    {
        case MENUREAL:
            minValue = *(RwReal *)&MenuState.activeEntry->minValue;
            maxValue = *(RwReal *)&MenuState.activeEntry->maxValue;

            value =
                *(RwReal *)(MenuState.activeEntry->target) +
                percentage * ( maxValue - minValue );

            if( value > maxValue )
            {
                value = maxValue;
            }
            else if( value < minValue )
            {
                value = minValue;
            }

            *(RwReal *)(MenuState.activeEntry->target) = value;
            break;

        default:
            return FALSE;
    }

    if( MenuState.activeEntry->triggerCallBack &&
        !MenuState.activeEntry->triggerCallBack(FALSE) )
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuSelectionAddValue(RwInt32 increment)
{
    RwReal value;
    RwReal minValue, maxValue;

    if( !MenuState.activeEntry )
    {
        return FALSE;
    }

    switch( MenuState.activeEntry->type )
    {
        case MENUBOOL:
        case MENUBOOLTRANSIENT:
        case MENUINT:
            (*(RwInt32 *)(MenuState.activeEntry->target)) +=
                increment * MenuState.activeEntry->stepSize;

            if( *(RwInt32 *)(MenuState.activeEntry->target) >
                MenuState.activeEntry->maxValue )
            {
                *(RwInt32 *)(MenuState.activeEntry->target) =
                    MenuState.activeEntry->minValue;
            }
            else if( *(RwInt32 *)(MenuState.activeEntry->target) <
                     MenuState.activeEntry->minValue )
            {
                *(RwInt32 *)(MenuState.activeEntry->target) =
                    MenuState.activeEntry->maxValue;
            }
            break;

        case MENUREAL:
            minValue = *(RwReal *)&MenuState.activeEntry->minValue;
            maxValue = *(RwReal *)&MenuState.activeEntry->maxValue;

            value =
                *(RwReal *)(MenuState.activeEntry->target) +
                (RwReal)increment * *(RwReal *)&MenuState.activeEntry->stepSize;

            if( value > maxValue )
            {
                value = maxValue;
            }
            else if( value < minValue )
            {
                value = minValue;
            }

            *(RwReal *)(MenuState.activeEntry->target) = value;
            break;
        case MENUTRIGGER:
            break;
        default:
            return FALSE;
    }

    if( MenuState.activeEntry->triggerCallBack &&
        !MenuState.activeEntry->triggerCallBack(FALSE) )
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
void
AcuMenu::CheckHelpFilesBottomLine(void)
{
    /*
     * When the bottom of the help screen is reached don't allow the user to
     * scroll down any more...
     */
    if( MenuState.helpFileNumEntriesPerColumn > (MenuState.numHelpLines -
                                                 MenuState.topHelpLine) )
    {
        MenuState.topHelpLine =
            MenuState.numHelpLines - MenuState.helpFileNumEntriesPerColumn;

        if( MenuState.topHelpLine < 0 )
        {
            MenuState.topHelpLine = 0;
        }
    }

    return;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::MenuMouseHandler(RsEvent event, void *param)
{
    static RwBool enabled = FALSE;

    if( MenuState.mode == MENUOFF )
    {
        return rsEVENTNOTPROCESSED;
    }
    else if( MenuState.mode == HELPMODE )
    {
        if( event == rsMOUSEWHEELMOVE )
        {
            RwBool wheelForward = *(RwBool *)param;

            if( wheelForward )
            {
                MenuState.topHelpLine -= 2;

                if( MenuState.topHelpLine < 0 )
                {
                    MenuState.topHelpLine = 0;
                }
            }
            else
            {
                MenuState.topHelpLine += 2;

                /*CheckHelpFilesBottomLine();*/
            }
        }

        return rsEVENTPROCESSED;
    }
    else if( MenuState.mode == MENUMODE )
    {
        /*
         * Allows the mouse wheel to be used to scroll down the menu
         * entries...
         */
        if( event == rsMOUSEWHEELMOVE )
        {
            RwBool wheelForward = *(RwBool *)param;

            if( wheelForward )
            {
                MenuState.startingEntry = MenuState.activeEntry;
                MenuSelectPrevious();
            }
            else
            {
                MenuState.startingEntry = MenuState.activeEntry;
                MenuSelectNext();
            }

            return rsEVENTPROCESSED;
        }

        if( !MenuState.activeEntry ||
            MenuState.activeEntry->type == MENUBOOL ||
            MenuState.activeEntry->type == MENUBOOLTRANSIENT ||
            MenuState.activeEntry->type == MENUTRIGGER ||
            MenuState.activeEntry->type == MENUINT )
        {
            enabled = FALSE;

            return rsEVENTNOTPROCESSED;
        }

        if( event == rsLEFTBUTTONDOWN )
        {
            enabled = TRUE;

            return rsEVENTPROCESSED;
        }

        if( event == rsLEFTBUTTONUP )
        {
            enabled = FALSE;

            return rsEVENTPROCESSED;
        }

        if( enabled && event == rsMOUSEMOVE )
        {
            RsMouseStatus *mouseStatus;
            RwReal biggestDelta;

            mouseStatus = (RsMouseStatus *)param;

            /*
             * Change the entry by the given delta value...
             */
            biggestDelta = mouseStatus->delta.x;

            if( RwRealAbs(mouseStatus->delta.y) > RwRealAbs(biggestDelta) )
            {
                biggestDelta = -mouseStatus->delta.y;
            }

            if( RwRealAbs(biggestDelta) != 0.0f )
            {
                MenuSelectionAddPercentage(biggestDelta * 0.002f);
            }

            return rsEVENTPROCESSED;
        }
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::AutoRepeatPrepare(RsEvent event, void *param)
{
    if( event == rsKEYDOWN )
    {
        /*
         * Shift and control have no autorepeat...
         */
        if( ((RsKeyStatus *)param)->keyCharCode == rsLSHIFT ||
            ((RsKeyStatus *)param)->keyCharCode == rsRSHIFT ||
            ((RsKeyStatus *)param)->keyCharCode == rsLCTRL ||
            ((RsKeyStatus *)param)->keyCharCode == rsRCTRL )
        {
            return FALSE;
        }

        if( !MenuState.lastKeyPressed )
        {
            MenuState.timeOfLastKeyPress = RsTimer();
            MenuState.lastKeyPressed = ((RsKeyStatus *)param)->keyCharCode;
        }

        MenuState.timeOfLastAutoRepeat = RsTimer();
    }
    else if( event == rsPADBUTTONDOWN )
    {
        /*
         * Shift and menu toggle button have no autorepeat
         * (the menu toggle button has no autorepeat because between
         * the button going down and going up, the help file may
         * be loaded, which may take longer than the autorepeat delay...
         */
        if( (((RsPadButtonStatus *)param)->padButtons & rsPADBUTTON3) ||
            (((RsPadButtonStatus *)param)->padButtons & rsPADBUTTON4) )
        {
            return FALSE;
        }

        if( !MenuState.lastKeyPressed )
        {
            MenuState.timeOfLastKeyPress = RsTimer();
            MenuState.lastKeyPressed = ((RsPadButtonStatus *)param)->padButtons;
        }

        MenuState.timeOfLastAutoRepeat = RsTimer();
    }
    else
    {
        /*
         * Stop autorepeat...
         */
        MenuState.lastKeyPressed = 0;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::AutoRepeat(void)
{
    if( !MenuState.lastKeyPressed )
    {
        return FALSE;
    }

    if( RsTimer() - MenuState.timeOfLastKeyPress < AUTOREPEATDELAY )
    {
        return FALSE;
    }

    if( RsTimer() - MenuState.timeOfLastAutoRepeat < AUTOREPEATINTERVAL )
    {
        return FALSE;
    }

    /*
     * Lengthy operations or operations that show an error popup
     * do not have an autorepeat...
     */
    if( RsTimer() - MenuState.timeOfLastAutoRepeat > AUTOREPEATTIMEOUT )
    {
        return FALSE;
    }

#ifdef WITH_KEYBOARD
    {
        RsKeyStatus keyStatus;

        memset(&keyStatus, 0, sizeof(keyStatus));
        keyStatus.keyCharCode = MenuState.lastKeyPressed;
        MenuKeyboardHandler(rsKEYDOWN, &keyStatus);
    }
#else
    {
        RsPadButtonStatus pb;

        memset(&pb, 0, sizeof(pb));
        pb.padButtons = MenuState.lastKeyPressed;
        MenuPadHandler(rsPADBUTTONDOWN, &pb);
    }
#endif

    return TRUE;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::HandlePadButtonDown(RsEvent __RWUNUSED__ event, void *param)
{
    RsPadButtonStatus *padButtonStatus;

    padButtonStatus = (RsPadButtonStatus *)param;

    if( padButtonStatus->padButtons & rsPADDPADUP )
    {
        MenuState.startingEntry = MenuState.activeEntry;
        MenuSelectPrevious();

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        MenuState.startingEntry = MenuState.activeEntry;
        MenuSelectNext();

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        MenuSelectionAddValue(-1);

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        MenuSelectionAddValue(1);

        return rsEVENTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::HandlePadButtonUp(RsEvent __RWUNUSED__  event, void *param)
{
    RsPadButtonStatus *padButtonStatus;

    padButtonStatus = (RsPadButtonStatus *)param;

    if( padButtonStatus->padButtons & rsPADSELECT )
    {
        return rsEVENTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::HandlePadAnalogueLeft(void *param)
{
    if( !MenuState.activeEntry ||
        MenuState.activeEntry->type == MENUTRIGGER ||
        MenuState.activeEntry->type == MENUBOOL ||
        MenuState.activeEntry->type == MENUBOOLTRANSIENT ||
        MenuState.activeEntry->type == MENUINT )
    {
        return rsEVENTNOTPROCESSED;
    }
    else
    {
        RwV2d *delta = (RwV2d *)param;
        RwReal biggestDelta;
        RwReal percentage;

        biggestDelta = -delta->x;

        if( RwRealAbs(delta->y) > RwRealAbs(biggestDelta) )
        {
            biggestDelta = delta->y;
        }

        percentage = biggestDelta * RwRealAbs(biggestDelta) * 0.01f;

        if( RwRealAbs(percentage) > 0.0f )
        {
            MenuSelectionAddPercentage(percentage);
        }

        return rsEVENTPROCESSED;
    }
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::MenuPadHandler(RsEvent event, void *param)
{
    RsPadButtonStatus *padButtonStatus;

    padButtonStatus = (RsPadButtonStatus *)param;

    if (event == rsPADBUTTONUP && padButtonStatus->padButtons & rsPADSELECT )
    {
        MenuToggle();
        return rsEVENTPROCESSED;
    }

    if( MenuState.mode == MENUOFF )
    {
        return rsEVENTNOTPROCESSED;
    }
    else if( MenuState.mode == HELPMODE )
    {
        if( event == rsPADBUTTONDOWN )
        {
            if( padButtonStatus->padButtons & rsPADDPADDOWN )
            {
                MenuState.topHelpLine++;

                /*CheckHelpFilesBottomLine();*/
            }
            else if( padButtonStatus->padButtons & rsPADDPADUP )
            {
                MenuState.topHelpLine--;

                if( MenuState.topHelpLine < 0 )
                {
                    MenuState.topHelpLine = 0;
                }
            }
#if (defined(_XBOX))
            else if( padButtonStatus->padButtons & rsPADBUTTON8 )
            {
                MenuState.topHelpLine += MenuState.helpFileNumEntriesPerColumn;

                /*CheckHelpFilesBottomLine;*/
            }
            else if( padButtonStatus->padButtons & rsPADBUTTON7 )
            {
                MenuState.topHelpLine -= MenuState.helpFileNumEntriesPerColumn;

                if( MenuState.topHelpLine < 0 )
                {
                    MenuState.topHelpLine = 0;
                }
            }
#else
            else if( padButtonStatus->padButtons & rsPADBUTTON6 )
            {
                MenuState.topHelpLine += MenuState.helpFileNumEntriesPerColumn;

                /*CheckHelpFilesBottomLine();*/
            }
            else if( padButtonStatus->padButtons & rsPADBUTTON5 )
            {
                MenuState.topHelpLine -= MenuState.helpFileNumEntriesPerColumn;

                if( MenuState.topHelpLine < 0 )
                {
                    MenuState.topHelpLine = 0;
                }
            }
#endif /* _XBOX */
        }

        AutoRepeatPrepare(event, param);

        return rsEVENTPROCESSED;
    }
    else if( MenuState.mode == MENUMODE )
    {
        RsEventStatus result;

        switch( event )
        {
            case rsPADBUTTONDOWN:
                {
                    result = HandlePadButtonDown(event, param);
                    break;
                }

            case rsPADBUTTONUP:
                {
                    result = HandlePadButtonUp(event, param);
                    break;
                }

            case rsPADANALOGUELEFT:
                {
                    result = HandlePadAnalogueLeft(param);
                    break;
                }

            default:
                {
                    result = rsEVENTNOTPROCESSED;
                    break;
                }
        }

        AutoRepeatPrepare(event, param);

        return result;
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::MenuKeyboardShortcutHandler(RsEvent event, void *param)
{
    RsKeyStatus *keyStatus;
    RwInt32 key;
    MenuEntry *ptr;

    keyStatus = (RsKeyStatus *)param;
    key = keyStatus->keyCharCode;

    if( key >= 'a' && key <= 'z' )
    {
        key += 'A' - 'a';
    }

    for(ptr = MenuState.menuList; ptr; ptr = ptr->next)
    {
        if( (ptr->shortcut == key ) &&
            (!ptr->triggerCallBack || ptr->triggerCallBack(TRUE)) )
        {
            MenuEntry *oldActiveEntry = MenuState.activeEntry;

            MenuState.activeEntry = ptr;

            if( MenuState.activeEntry->type != MENUBOOLTRANSIENT )
            {
                if( event == rsKEYDOWN )
                {
                    MenuSelectionAddValue(1);
                }
            }
            else
            {
                if( event == rsKEYDOWN )
                {
                    if( MenuState.activeEntry->triggerCallBack == NULL ||
                        MenuState.activeEntry->triggerCallBack(FALSE) )
                    {
                        *(RwBool *)MenuState.activeEntry->target = TRUE;
                    }
                }
                else if( event == rsKEYUP )
                {
                    if( MenuState.activeEntry->triggerCallBack )
                    {
                        MenuState.activeEntry->triggerCallBack(FALSE);
                    }

                    *(RwBool *)MenuState.activeEntry->target = FALSE;
                }
            }

            MenuState.activeEntry = oldActiveEntry;

            return rsEVENTPROCESSED;
        }
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::MenuHandleKeyDown(void *param)
{
    RsKeyStatus *keyStatus;

    keyStatus = (RsKeyStatus *)param;

    switch( keyStatus->keyCharCode )
    {
        case rsUP:
            {
                /*
                 * Select previous menu entry...
                 */
                MenuState.startingEntry = MenuState.activeEntry;
                MenuSelectPrevious();

                return rsEVENTPROCESSED;
            }

        case rsDOWN:
            {
                /*
                 * Select next menu entry...
                 */
                MenuState.startingEntry = MenuState.activeEntry;
                MenuSelectNext();

                return rsEVENTPROCESSED;
            }

        case rsLEFT:
            {
                /*
                 * Decrement selected menu entry...
                 */
                MenuSelectionAddValue(-1);

                return rsEVENTPROCESSED;
            }

        case rsRIGHT:
            {
                /*
                 * Increment selected menu entry...
                 */
                MenuSelectionAddValue(1);

                return rsEVENTPROCESSED;
            }
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::MenuHandleKeyUp(void *param)
{
    RsKeyStatus *keyStatus;

    keyStatus = (RsKeyStatus *)param;

    if( (MenuState.activeEntry) && (MenuState.activeEntry->type == MENUBOOLTRANSIENT) )
    {
        if( keyStatus->keyCharCode == rsLEFT ||
            keyStatus->keyCharCode == rsRIGHT ||
            keyStatus->keyCharCode == rsENTER )
        {
            if( MenuState.activeEntry->triggerCallBack )
            {
                MenuState.activeEntry->triggerCallBack(FALSE);
            }

            *(RwBool *)MenuState.activeEntry->target = FALSE;

            return rsEVENTPROCESSED;
        }
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
AcuMenu::MenuKeyboardHandler(RsEvent event, void *param)
{
    RsKeyStatus *keyStatus;
    RwInt32 key;

    if( MenuKeyboardShortcutHandler(event, param) == rsEVENTPROCESSED )
    {
        return rsEVENTPROCESSED;
    }

    keyStatus = (RsKeyStatus *)param;
    key = keyStatus->keyCharCode;

    if (event == rsKEYDOWN && key == ' ')
    {
        MenuToggle();
        return rsEVENTPROCESSED;
    }

    if( MenuState.mode == MENUOFF )
    {
        return rsEVENTNOTPROCESSED;
    }
    else if( MenuState.mode == HELPMODE )
    {
        if( event == rsKEYDOWN )
        {
            if( key == rsDOWN )
            {
                MenuState.topHelpLine++;

                /*CheckHelpFilesBottomLine();*/
            }
            else if( key == rsUP )
            {
                MenuState.topHelpLine--;

                if( MenuState.topHelpLine < 0 )
                {
                    MenuState.topHelpLine = 0;
                }
            }
            else if( key == rsPGDN )
            {
                MenuState.topHelpLine +=
                    (MenuState.helpFileNumEntriesPerColumn - 2);

                /*CheckHelpFilesBottomLine();*/
            }
            else if( key == rsPGUP )
            {
                MenuState.topHelpLine -=
                    (MenuState.helpFileNumEntriesPerColumn - 2);

                if( MenuState.topHelpLine < 0 )
                {
                    MenuState.topHelpLine = 0;
                }
            }
            else if( key == rsESC )
            {
                RsEventHandler(rsQUITAPP, NULL);
            }
        }

        AutoRepeatPrepare(event, param);

        return rsEVENTPROCESSED;
    }
    else if( MenuState.mode == MENUMODE )
    {
        RsEventStatus result;

        switch( event )
        {
            case rsKEYDOWN:
                {
                    result = MenuHandleKeyDown(param);
                    break;
                }

            case rsKEYUP:
                {
                    result = MenuHandleKeyUp(param);
                    break;
                }

            default:
                {
                    result = rsEVENTNOTPROCESSED;
                    break;
                }
        }

        AutoRepeatPrepare(event, param);

        return result;
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
#ifndef OLDMENU

/*
 * New menu renderer where the entries remain in a single column where a
 * only the entries diaplayable in that single column are rendered and to
 * access the the rest the user scrolls up or down, the user is told whether
 * there are more entries above or below the current diaplayed entries...
 */
void
AcuMenu::RenderMenuEntries(RwCamera *camera  __RWUNUSED__,
                  RtCharset *userCharset,
                  RtCharsetDesc *charsetDesc)
{
    const RwChar *moreLabel = RWSTRING(" --- (MORE) --- ");
    RwBool anyDrawn = FALSE;
    RwUInt32 maxLength = 0;
    RwInt32 leftMargin = rsPRINTMARGINLEFT * charsetDesc->width;
    RwInt32 i;

    for(i=0; i<3; i++)
    {
        static RwChar complete[MENUENTRYLENGTHMAX], value[MENUENTRYLENGTHMAX];
        static RwChar desc[MENUENTRYLENGTHMAX], separatorString[MENUENTRYLENGTHMAX];
        static RwInt32 separatorLength = 0;
        MenuEntry *ptr = MenuState.menuList;
        RtCharset *current = userCharset;
        RwInt32 line;
        RwInt32 j;

        /* NOTE: we set line to 1 so the "more" string when rendered at
         * the bottom/top of the menu is not on the same line with the
         * bottom/top displayable menu entry... */
        line = 1;

        /*
         * Sort entries by charset to reduce the number of flushes...
         */
        if (i == 1)
        {
            current = MenuState.inverseCharset;
        }
        else if (i == 2)
        {
            current = MenuState.disabledCharset;
        }

        /*
         * Goto the first displayable menu entry...
         */
        for(j=MenuState.numEntries-1; j>CurrentTopIndex && ptr; j--)
        {
            ptr = ptr->next;
        }

        /*
         * For each of the displayable menu entries...
         */
        for(j=CurrentTopIndex; j<=CurrentBottomIndex && ptr; j++)
        {
            RtCharset *charset = (RtCharset *)NULL;
            RwBool hideSpaces = TRUE;
            RwInt32 length;

            /*
             * Select the charset for each line...
             */
            if( !ptr->triggerCallBack || ptr->triggerCallBack(TRUE) )
            {
                if( ptr == MenuState.activeEntry )
                {
                    charset = MenuState.inverseCharset;
                    hideSpaces = FALSE;
                }
                else
                {
                    charset = userCharset;
                }
            }
            else
            {
                if( ptr == MenuState.activeEntry )
                {
                    MenuSelectNext();
                }

                charset = MenuState.disabledCharset;
            }

            if (charset == current)
            {
                rwsprintf(desc, " %s", ptr->description);

                switch (ptr->type)
                {
                    case MENUBOOL:
                    case MENUBOOLTRANSIENT:
                        {
                            rwsprintf(value, (*(RwBool *)(ptr->target)) ?
                                      RWSTRING(" On ") : RWSTRING("Off "));
                        }
                        break;

                    case MENUINT:
                        {
                            if( ptr->enumStrings )
                            {
                                RwInt32 index = 0;

                                value[0] = '\0';

                                if( ptr->target )
                                {
                                    index = *(RwInt32*)(ptr->target) - ptr->minValue;

                                    if( ptr->enumStrings[index] &&
                                        ptr->enumStrings[index][0] )
                                    {
                                        rwsprintf(value, RWSTRING("%s "),
                                                  ptr->enumStrings[index]);
                                    }
                                }
                            }
                            else
                            {
                                rwsprintf(value, RWSTRING("%d "),
                                          *(RwInt32 *)(ptr->target));
                            }
                        }
                        break;

                    case MENUREAL:
                        {
                            rwsprintf(value, RWSTRING("%0.2f "),
                                      *(RwReal *)(ptr->target));
                        }
                        break;

                    default:
                        {
                            rwsprintf(value, RWSTRING("  < "));
                        }
                        break;
                }

#ifdef WITH_SHORTCUT
                if( ptr->shortcut )
                {
                    if( ptr->shortcut < rsF1 || ptr->shortcut > rsF9 )
                    {
                        RwInt32 spaces =
                            MenuState.maxDescLength - rwstrlen(desc) + 2;

                        rwsprintf(desc, RWSTRING("%s%-*s[%c]"), desc, spaces,
                                  "", ptr->shortcut);
                    }
                    else
                    {
                        RwInt32 spaces =
                            MenuState.maxDescLength - rwstrlen(desc) + 2;

                        rwsprintf(desc, RWSTRING("%s%-*s[F%c]"), desc, spaces,
                                  "", '1' + ptr->shortcut - rsF1);

                    }
                }
#endif /* WITH_SHORTCUT */

                if( rwstrcmp(desc, RWSTRING(" ")) != 0 )
                {
                    /*
                     * Render any menu entries except for separators...
                     */

                    /*
                     * Concatentate the first and second strings,
                     * padding the first string on its left side...
                     */
                    length = MenuState.longestEntryLength - rwstrlen(value);

                    rwsprintf(complete, "%-*s", length, desc);
                    rwstrcat(complete, value);

                    RtCharsetPrintBuffered(
                        charset, complete, leftMargin,
                        (line + rsPRINTMARGINTOP) * charsetDesc->height,
                        hideSpaces);

                    if( rwstrlen(complete) > maxLength )
                    {
                        /*
                         * Adjust the number of "-"s that are used for the
                         * separator between menu entries...
                         */
                        maxLength = rwstrlen(complete);

                        if( ((RwInt32)maxLength - 2) > separatorLength )
                        {
                            separatorLength = (RwInt32)maxLength - 2;

                            if( separatorLength < 0 )
                            {
                                separatorLength = 0;
                            }
                            else if( separatorLength > MENUENTRYLENGTHMAX)
                            {
                                separatorLength = MENUENTRYLENGTHMAX;
                            }

                            memset(separatorString, '-', separatorLength);
                        }
                    }
                }
                else
                {
                    /*
                     * Rendered the separator...
                     */
                    RtCharsetPrintBuffered(
                        MenuState.scrollCharset, separatorString,
                        leftMargin + charsetDesc->width,
                        (line + rsPRINTMARGINTOP) * charsetDesc->height,
                        hideSpaces);
                }

                anyDrawn = TRUE;
            }

            /*
             * Jump to the next displayable menu entry...
             */
            ptr = ptr->prev;
            line++;
        }
    }

    /*
     * If the displayed top menu entry is not the top entry in the menu, then
     * render the "(more)" text (to tell the user there are more entries
     * above)...
     */
    if( CurrentTopIndex > 0 )
    {
        RwInt32 center;

        center = (maxLength * charsetDesc->width - charsetDesc->width *
                  rwstrlen(moreLabel)) / 2;

        center += leftMargin;

        RtCharsetPrintBuffered(MenuState.scrollCharset, moreLabel, center,
                               rsPRINTMARGINTOP*charsetDesc->height, TRUE);

        anyDrawn = TRUE;
    }

    /*
     * If the displayed bottom menu entry is not the bottom entry in the menu,
     * then render the "(more)" text (to tell the user there are more entries
     * below)...
     */
    if( CurrentBottomIndex < MenuState.numEntries - 1 )
    {
        RwInt32 center;

        center = (maxLength * charsetDesc->width - charsetDesc->width *
                  rwstrlen(moreLabel)) / 2;

        center += leftMargin;

        RtCharsetPrintBuffered(MenuState.scrollCharset, moreLabel, center,
                               (rsPRINTMARGINTOP + 1 + MenuState.numEntriesPerColumn) * charsetDesc->height, TRUE);

        anyDrawn = TRUE;
    }

    /*
     * Make sure it's drawn...
     */
    if (anyDrawn != FALSE)
    {
        RtCharsetBufferFlush();
    }

    return;
}

#else

/*
 * Old menu where if the menu entries don't fit in the current column they are
 * rendered in the next...
 */
static void
AcuMenu::RenderMenuEntries(RwCamera *camera  __RWUNUSEDUNLESSRWLOGO__,
                  RtCharset *userCharset,
                  RtCharsetDesc *charsetDesc)
{
    MenuEntry *ptr;
    RwInt32 count, column, line, length, i;
    RtCharset *charset;
    static RwChar complete[MENUENTRYLENGTHMAX], value[MENUENTRYLENGTHMAX];
    static RwChar desc[MENUENTRYLENGTHMAX];

    for (i = 0;i < 3;i++)
    {
        RwBool anyDrawn = FALSE;
        RtCharset *current = userCharset;

        /* Sort entries by charset to reduce the number of flushes */
        if (i == 1)
        {
            current = MenuState.inverseCharset;
        }
        else if (i == 2)
        {
            current = MenuState.disabledCharset;
        }

        count = MenuState.numEntries;

        for(ptr = MenuState.menuList; ptr && count--; ptr = ptr->next)
        {
            RwBool hideSpaces = TRUE;
            /*
             * For each menu entry...
             */
            column = count / MenuState.numEntriesPerColumn;
            line = count - (column * MenuState.numEntriesPerColumn);

            /*
             * Select the charset for each line...
             */
            if( !ptr->triggerCallBack || ptr->triggerCallBack(TRUE) )
            {
                if( ptr == MenuState.activeEntry )
                {
                    charset = MenuState.inverseCharset;
                    hideSpaces = FALSE;
                }
                else
                {
                    charset = userCharset;
                }
            }
            else
            {
                if( ptr == MenuState.activeEntry )
                {
                    MenuSelectNext();
                }
                charset = MenuState.disabledCharset;
            }

            if (charset == current)
            {
                rwsprintf(desc, " %s", ptr->description);

                switch (ptr->type)
                {
                    case MENUBOOL:
                    case MENUBOOLTRANSIENT:
                        {
                            rwsprintf(value, (*(RwBool *)(ptr->target)) ?
                                      RWSTRING(" On ") : RWSTRING("Off "));
                        }
                        break;

                    case MENUINT:
                        {
                            if( ptr->enumStrings )
                            {
                                RwInt32 index = 0;

                                value[0] = '\0';

                                if( ptr->target )
                                {
                                    index = *(RwInt32*)(ptr->target) - ptr->minValue;

                                    if( ptr->enumStrings[index] &&
                                        ptr->enumStrings[index][0] )
                                    {
                                        rwsprintf(value, RWSTRING("%s "),
                                                  ptr->enumStrings[index]);
                                    }
                                }
                            }
                            else
                            {
                                rwsprintf(value, RWSTRING("%d "), *(RwInt32 *)(ptr->target));
                            }
                        }
                        break;

                    case MENUREAL:
                        {
                            rwsprintf(value, RWSTRING("%0.2f "), *(RwReal *)(ptr->target));
                        }
                        break;

                    default:
                        {
                            rwsprintf(value, RWSTRING("  < "));
                        }
                        break;
                }

#ifdef WITH_SHORTCUT
                if( ptr->shortcut )
                {
                    if( ptr->shortcut < rsF1 || ptr->shortcut > rsF9 )
                    {
                        RwInt32 spaces = MenuState.maxDescLength - rwstrlen(desc) + 2;
                        rwsprintf(desc, RWSTRING("%s%-*s[%c]"),
                                  desc, spaces, "", ptr->shortcut);
                    }
                    else
                    {
                        RwInt32 spaces = MenuState.maxDescLength - rwstrlen(desc) + 2;
                        rwsprintf(desc, RWSTRING("%s%-*s[F%c]"),
                                  desc, spaces, "", '1' + ptr->shortcut - rsF1);

                    }
                }
#endif /* WITH_SHORTCUT */

                /*
                 * Concatentate the first and second strings,
                 * padding the first string on its left side...
                 */
                length = MenuState.longestEntryLength - rwstrlen(value);
                rwsprintf(complete, "%-*s", length, desc);
                rwstrcat(complete, value);
                RtCharsetPrintBuffered(charset, complete,
                                       (rsPRINTMARGINLEFT + column * (MenuState.longestEntryLength + COLUMNSPACING)) * charsetDesc->width,
                                       (line + rsPRINTMARGINTOP) * charsetDesc->height, hideSpaces);

                anyDrawn = TRUE;
            }
        }

        /* Make sure it's drawn */
        if (anyDrawn != FALSE)
        {
            RtCharsetBufferFlush();
        }
    }

#ifdef RWLOGO
    {
        RwRect * const rect = RpLogoGetRenderingRect();
        if( rect && (MenuState.numEntries > 
                     (rect->y / charsetDesc->height)-1 ) )
        {
            RpLogoSetState(camera, FALSE);
        }
    }
#endif /* RWLOGO */

    return;
}
#endif /* OLDMENU */

/*
 *****************************************************************************
 */
void
AcuMenu::RenderHelpScreen(RwCamera *camera, RtCharset *charset,
                 RtCharsetDesc *charsetDesc)
{
    RwInt32 helpLine, line;

    RwCameraClear(camera, &MenuState.backgroundColor,
                  rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

    CheckHelpFilesBottomLine();

    for(line = 0; line < MenuState.helpFileNumEntriesPerColumn; line++)
    {
        helpLine = MenuState.topHelpLine + line;

        if( helpLine >= MenuState.numHelpLines )
        {
            break;
        }

        RtCharsetPrintBuffered(charset, helpLines[helpLine],
                               rsPRINTMARGINLEFT*charsetDesc->width,
                               (line + rsPRINTMARGINTOP)*charsetDesc->height,
                               TRUE);
    }

    /* Make sure it's drawn */
    RtCharsetBufferFlush();

#ifdef RWLOGO
    RpLogoSetState(camera, FALSE);
#endif /* RWLOGO */

#ifdef RWMOUSE
    RsMouseSetVisibility(FALSE);
#endif /* RWMOUSE */

    return;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuRender(RwCamera *camera, RwRaster *userCharset)
{
    RtCharsetDesc charsetDesc;
    RwUInt32 rasHeight;

#ifdef RWLOGO
    /*
     * The previous menu render may have turned-off the logo...
     */
    RpLogoSetState(camera, TRUE);
#endif

#ifdef RWMOUSE
    /*
     * ...and may have turned-off the mouse pointer, too...
     */
    RsMouseSetVisibility(TRUE);
#endif

    if( MenuState.mode == MENUOFF )
    {
        return FALSE;
    }

    if( !userCharset )
    {
        userCharset = MenuState.mainCharset;
        if( !userCharset )
        {
            return FALSE;
        }
    }

    RtCharsetGetDesc(userCharset, &charsetDesc);

    rasHeight = RwRasterGetHeight(RwCameraGetRaster(camera));

    MenuState.numEntriesPerColumn = rasHeight / charsetDesc.height;
    /* Take into account margins */
    MenuState.numEntriesPerColumn -= rsPRINTMARGINTOP + rsPRINTMARGINBOTTOM;

    /* Logo and "more" labels not displayed in help mode */
    MenuState.helpFileNumEntriesPerColumn = MenuState.numEntriesPerColumn;

#if (!defined(OLDMENU))
#if (defined(RWLOGO))
    {
        RwRect * const rect = RpLogoGetRenderingRect();
        /* Always want the RW logo visible, so adjust the number of
         * displayable entries... */
        if (NULL != rect)
        {
            MenuState.numEntriesPerColumn -=
                (rasHeight - rect->y) / charsetDesc.height;
        }
    }
#endif /* (defined(RWLOGO)) */

    /* Leave space for the up/down "more" label if we scroll */
    MenuState.numEntriesPerColumn -= 2;
#endif /* (!defined(OLDMENU)) */

    if (MenuState.numEntriesPerColumn >= 1)
    {

#if (!defined(OLDMENU))
        /* Find the last displayable menu entry. */
        CurrentBottomIndex =
            CurrentTopIndex + (MenuState.numEntriesPerColumn - 1);

        if( CurrentBottomIndex > MenuState.numEntries )
        {
            CurrentBottomIndex = MenuState.numEntries;
        }

        /*
         * The window could have been resized smaller or the user has gone
         * down the menu so check that the selected entry is still visible...
         */
        if( SelectedMenuEntry > CurrentBottomIndex )
        {
            CurrentBottomIndex = SelectedMenuEntry;

            CurrentTopIndex =
                CurrentBottomIndex - (MenuState.numEntriesPerColumn - 1);
        }
        else if( SelectedMenuEntry < CurrentTopIndex )
        {
            CurrentTopIndex = SelectedMenuEntry;

            CurrentBottomIndex =
                CurrentTopIndex + (MenuState.numEntriesPerColumn - 1);
        }

        /*
         * The window could have been resized larger or the user has gone up
         * the menu so make sure the maximum number of displayable entries are
         * shown...
         */
        if( CurrentTopIndex > 0 && (CurrentBottomIndex - CurrentTopIndex + 1) <
            (MenuState.numEntriesPerColumn - 1) )
        {
            CurrentTopIndex--;
            if( CurrentTopIndex < 0 )
            {
                CurrentTopIndex = 0;
            }
        }

#endif /* (!defined(OLDMENU)) */
    }
    else
    {
        return FALSE;
    }

    if( MenuState.mode == MENUMODE )
    {
        RenderMenuEntries(camera, userCharset, &charsetDesc);
    }
    else if( MenuState.mode == HELPMODE )
    {
        RenderHelpScreen(camera, userCharset, &charsetDesc);
    }
    else
    {
        return FALSE;
    }

    AutoRepeat();

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuClose(void)
{
    RwBool result = TRUE;
    MenuEntry *ptr;

    FreeHelpLines();

    /*
     * Free all the memory used for menu entries...
     */
    ptr = MenuState.menuList;

    while( ptr )
    {
        MenuEntry *next = ptr->next;

        RemoveEntry(ptr);

        RwFree(ptr);

        MenuState.numEntries--;

        ptr = next;
    }

    if( MenuState.scrollCharset )
    {
        RtCharsetDestroy(MenuState.scrollCharset);
        MenuState.scrollCharset = 0;
    }

    if( MenuState.inverseCharset )
    {
        RtCharsetDestroy(MenuState.inverseCharset);
        MenuState.inverseCharset = 0;
    }

    if( MenuState.disabledCharset )
    {
        RtCharsetDestroy(MenuState.disabledCharset);
        MenuState.disabledCharset = 0;
    }

    if( MenuState.mainCharset )
    {
        RtCharsetDestroy(MenuState.mainCharset);
        MenuState.mainCharset = 0;
    }

    MenuState.isOpen = FALSE;

    return result;
}

/*
 *****************************************************************************
 */
RwBool
AcuMenu::MenuOpen(RwBool createCharset, RwRGBA *foreground, RwRGBA *background)
{
    RwRGBA color;

    if( foreground )
    {
        MenuState.foregroundColor = *foreground;
    }

    if( background )
    {
        MenuState.backgroundColor = *background;
    }

    if( createCharset )
    {
        MenuState.mainCharset =
            RtCharsetCreate(&MenuState.foregroundColor, &MenuState.backgroundColor);

        if( !MenuState.mainCharset )
        {
            return FALSE;
        }
    }
    else
    {
        MenuState.mainCharset = 0;
    }

    /*
     * Calculate the color of the disabled menu items and create
     * a corresponding charset...
     */
    InterpolateColor(&MenuState.foregroundColor, &MenuState.backgroundColor,
                     DISABLEDBRIGHTNESS, &color);

    MenuState.disabledCharset =
        RtCharsetCreate(&color, &MenuState.backgroundColor);

    if( !MenuState.disabledCharset )
    {
        RwFree(MenuState.mainCharset);

        return FALSE;
    }

    /*
     * Calculate the color of the selected menu items and create
     * a corresponding charset...
     */
    InterpolateColor(&MenuState.foregroundColor, &MenuState.backgroundColor,
                     SELECTEDBRIGHTNESS, &color);

    MenuState.inverseCharset =
        RtCharsetCreate(&MenuState.foregroundColor, &color);

    if( !MenuState.inverseCharset )
    {
        RwFree(MenuState.disabledCharset);
        RwFree(MenuState.mainCharset);

        return FALSE;
    }

    /*
     * Calculate the color for the "more" label and separator, and create
     * a corresponding charset...
     */
    InterpolateColor(&MenuState.foregroundColor, &MenuState.backgroundColor,
                     SCROLLBRIGHTNESS, &color);

    MenuState.scrollCharset =
        RtCharsetCreate(&color, &MenuState.backgroundColor);

    if( !MenuState.scrollCharset )
    {
        RwFree(MenuState.scrollCharset);
        RwFree(MenuState.disabledCharset);
        RwFree(MenuState.mainCharset);

        return FALSE;
    }

    MenuState.mode = MENUMODE;

    FreeHelpLines();
    MenuState.isOpen = TRUE;

    LoadHelpFile();

    EmptyString[0] = '\0';

    return TRUE;
}

/*
*****************************************************************************
*/
AcuMenu::AcuMenu()
{
	SelectedMenuEntry = 0;//The current selected entry in the dispalyed menu...
	CurrentTopIndex = 0; // The index to the top entry in the dispalyed menu...
	CurrentBottomIndex = 1; // The index to the bottom entry in the dispalyed menu...


	/*
		NoHelpFile[25]
		HelpFileName[25]
		AltHelpFileName[25]
		선언되어 있음 문자열 길이에 주의 ~!	*/
	sprintf( NoHelpFile, "Cannot open help file");	
	sprintf( HelpFileName, "./readme.txt");
	
	#if (defined(SKY))
	sprintf( AltHelpFileName, "sky.txt"  );
	#elif (defined(DOLPHIN) || defined(EMU))
	sprintf( AltHelpFileName, "./gcn.txt"  );
	#elif (defined(_XBOX))
	sprintf( AltHelpFileName, "xbox.txt" );
	#elif (defined(WIN32))
	sprintf( AltHelpFileName, "./win.txt" );
	#elif (defined(__linux__))
	sprintf( AltHelpFileName, "./linux.txt" );
	#elif (defined(__QNX__))
	sprintf( AltHelpFileName, "./qnx.txt" );
	#else
	sprintf( AltHelpFileName, "./mac.txt" );
	#endif	

}

