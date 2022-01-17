
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
 * events.c (win)
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

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "alpick.h"
#include "alcollision.h"
#include "alcharacter.h"
#include "alcamera.h"
#include "alworld.h"

RwBool Ctrl = FALSE;

extern RpWorld *World;

extern RwReal totalTilt;
extern RwReal totalTurn;

extern RwReal currentTilt;
extern RwReal currentTurn;

extern AcuMenu		*g_clMenu;

extern void SetWorld(RwInt32 worldIndex);

/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonDown(RsMouseStatus *mouseStatus __RWUNUSED__)
{
    if( Ctrl )
    {
        RwV2d pixel = mouseStatus->pos;

        AlCharacterAdd(AlPickNearestWorld(&pixel));
    }
	else
	{
		bMove = TRUE;
	}

    return rsEVENTPROCESSED;
}

static RsEventStatus
HandleLeftButtonUp(RsMouseStatus *mouseStatus __RWUNUSED__)
{
	totalTurn = 0.0f;
	bMove = FALSE;

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonDown(RsMouseStatus *mouseStatus __RWUNUSED__)
{
	AlCharacterChangePart();

    return rsEVENTPROCESSED;
}

static RsEventStatus
HandleRightButtonUp(RsMouseStatus *mouseStatus __RWUNUSED__)
{
    /*
     * Right mouse button up event handling...
     */
    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus 
HandleMouseMove(RsMouseStatus *mouseStatus __RWUNUSED__)
{
    if( bMove ) 
    {
		currentTurn = -mouseStatus->delta.x * 0.005f;
		currentTilt = mouseStatus->delta.y * 0.1f;
    }

    return rsEVENTPROCESSED;
}

/*
 *****************************************************************************
 */
static RsEventStatus 
HandleMouseWheel(RsMouseStatus *mouseStatus)
{
	RwBool wheelForward = *(RwBool *) mouseStatus;

	AlCameraZoom(wheelForward);

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus 
MouseHandler(RsEvent event, void *param)
{
    /*
     * Let the menu system have a look-in first...
     */
    if( g_clMenu->MenuMouseHandler(event, param) == rsEVENTPROCESSED )
    {
        return rsEVENTPROCESSED;
    }

    /*
     * ...then the application events, if necessary...
     */
    switch( event )
    {
        case rsLEFTBUTTONDOWN:
        {
            return HandleLeftButtonDown((RsMouseStatus *)param);
        }

        case rsLEFTBUTTONUP:
        {
            return HandleLeftButtonUp((RsMouseStatus *)param);
        }

        case rsRIGHTBUTTONDOWN:
        {
            return HandleRightButtonDown((RsMouseStatus *)param);
        }

        case rsRIGHTBUTTONUP:
        {
            return HandleRightButtonUp((RsMouseStatus *)param);
        }

        case rsMOUSEMOVE:
        {
            return HandleMouseMove((RsMouseStatus *)param);
        }

		case rsMOUSEWHEELMOVE:
		{
			return HandleMouseWheel((RsMouseStatus *)param);
		}

        default:
        {
            return rsEVENTNOTPROCESSED;
        }
    }
}

RwBool SaveWorld()
{
	RwChar *path;
	RwStream *stream;
    RwBool success = TRUE;

    path = RsPathnameCreate(RWSTRING("./world.bsp"));
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, path);
    RsPathnameDestroy(path);

    if( stream )
    {
        if( !RpWorldStreamWrite(World, stream) )
        {   
            RsErrorMessage(RWSTRING("Cannot write BSP file."));

            success = FALSE;
        }

        RwStreamClose(stream, NULL);
    }
    else
    {
        RsErrorMessage(RWSTRING("Cannot open stream to write BSP file."));

        success =  FALSE;
    }

	return TRUE;
}

/*
 *****************************************************************************
 */
static RsEventStatus 
HandleKeyDown(RsKeyStatus *keyStatus)
{
    switch( keyStatus->keyCharCode )
    {
        case rsUP:
        {
            /*
             * CURSOR-UP...
             */
            return rsEVENTPROCESSED;
        }

        case rsDOWN:
        {
            /*
             * CURSOR-DOWN...
             */
            return rsEVENTPROCESSED;
        }

        case rsLEFT:
        {
            /*
             * CURSOR-LEFT...
             */
            return rsEVENTPROCESSED;
        }

        case rsRIGHT:
        {
            /*
             * CURSOR-RIGHT...
             */
            return rsEVENTPROCESSED;
        }

        case rsPGUP:
        {
			AlCharacterSetPlayer(0);
            return rsEVENTPROCESSED;
        }

        case rsPGDN:
        {
			AlCharacterSetPlayer(1);
            return rsEVENTPROCESSED;
        }

        case rsLCTRL:
        case rsRCTRL:
        {
            Ctrl = TRUE;

            return rsEVENTPROCESSED;
        }

		case rsF1:
		{
			SetWorld(1);
            return rsEVENTPROCESSED;
		}

		case rsF2:
		{
			SetWorld(2);
            return rsEVENTPROCESSED;
		}

		case rsF3:
		{
			SetWorld(3);
            return rsEVENTPROCESSED;
		}

		case rsF4:
		{
			SetWorld(4);
            return rsEVENTPROCESSED;
		}

		case rsF5:
		{
			SetWorld(5);
            return rsEVENTPROCESSED;
		}

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			AlCharacterSetAnim(keyStatus->keyCharCode - '0');
			return rsEVENTPROCESSED;
		}

		case rsHOME:
		{
			AlCharacterMove(1,1,1);
			return rsEVENTPROCESSED;
		}

		case 'q':
		{
			AlCameraSetPos(-1);
			return rsEVENTPROCESSED;
		}

		case 'w':
		{
			AlCameraSetPos(1);
			return rsEVENTPROCESSED;
		}

		case 'z':
		{
			AlWorldChangeSector();
			return rsEVENTPROCESSED;
		}

		case 's':
		{
			SaveWorld();
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
static RsEventStatus 
HandleKeyUp(RsKeyStatus *keyStatus __RWUNUSED__)
{
    switch( keyStatus->keyCharCode )
    {
        case rsRCTRL:
        case rsLCTRL:
        {
            Ctrl = FALSE;

            return rsEVENTPROCESSED;
        }

        default:
        {
            return rsEVENTNOTPROCESSED;
        }
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus 
KeyboardHandler(RsEvent event, void *param)
{
    /*
     * Let the menu system have a look-in first...
     */
    if( g_clMenu->MenuKeyboardHandler(event, param) == rsEVENTPROCESSED )
    {
        return rsEVENTPROCESSED;
    }

    /*
     * ...then the application events, if necessary...
     */
    switch( event )
    {
        case rsKEYDOWN:
        {
            return HandleKeyDown((RsKeyStatus *)param);
        }

        case rsKEYUP:
        {
            return HandleKeyUp((RsKeyStatus *)param);
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
RwBool
AttachInputDevices(void)
{
    RsInputDeviceAttach(rsKEYBOARD, KeyboardHandler);

    RsInputDeviceAttach(rsMOUSE, MouseHandler);

    return TRUE;
}

/*
 *****************************************************************************
 */
