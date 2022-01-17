#include <windows.h>
#include "rwcore.h"
#include "skel\skeleton.h"
#include "Scene.h"
#include "FXUI.h"
#include "MemoryManager\mmgr.h"

extern Scene* _pScene;
extern FXUI* _pFXUI;

extern void toggleModelStatsDisplay();

bool _bAltKey = false;

RwBool _bSpinOn = FALSE;

static RwInt32 NumRotationKeys = 0;
static RwInt32 NumTranslationKeys = 0;
static RwBool ClumpPick = FALSE;

#define CLUMPTRANSLATIONSPEEDFACTOR (1.5f)
#define CLUMPINCFACTOR (0.1f)
#define CLUMPDECFACTOR (0.03f)

#define CLUMPROTATIONMAXSPEED (100.0f)
#define CLUMPROTATIONINC (CLUMPROTATIONMAXSPEED * CLUMPINCFACTOR)
#define CLUMPROTATIONDEC (CLUMPROTATIONMAXSPEED * CLUMPDECFACTOR)

#define CLUMPSPINFACTOR (5.0f)

static RwV3d Zero = { 0.0f, 0.0f, 0.0f };

static RwBool TranslationChanged = FALSE;
static RwBool RotationChanged = FALSE;

static RwReal ClumpTranslationZ = 0.0f;
static RwV3d ClumpRotation = { 0.0f, 0.0f, 0.0f };

static RwReal ClumpTranslationMaxSpeed;
static RwReal ClumpTranslationInc;
static RwReal ClumpTranslationDec;

RwBool ClumpRotate = FALSE;
RwBool ClumpTranslate = FALSE;

RwBool ClumpDirectTranslate = FALSE;
RwBool ClumpDirectRotate = FALSE;

RwReal ClumpTranslateDeltaZ = 0.0f;
RwV3d ClumpRotateDelta = { 0.0f, 0.0f, 0.0f };


/*
 *****************************************************************************
 */
static void
GetZTranslation(void)
{
	TranslationChanged = FALSE;

	if( ClumpTranslate )
	{
		ClumpTranslationZ += ClumpTranslateDeltaZ * ClumpTranslationInc;
		TranslationChanged = TRUE;

		/* 
		 * Bound the translation...
		 */
		if( fabs(ClumpTranslationZ) > ClumpTranslationMaxSpeed )
		{
			ClumpTranslationZ = (ClumpTranslationZ > 0.0f)?
							   ClumpTranslationMaxSpeed :
							   -ClumpTranslationMaxSpeed;
		}
	}

	/* 
	 * If input ends, slow the moving clump to a stop...
	 */
	if( !ClumpTranslate && ClumpTranslationZ != 0.0f )
	{
		if( fabs(ClumpTranslationZ) < 1.0f )
		{
			ClumpTranslationZ = 0.0f;
		}
		else
		{
			ClumpTranslationZ += (ClumpTranslationZ < 0.0f)?
								  ClumpTranslationDec :
								  -ClumpTranslationDec ;
		}        

		ClumpTranslateDeltaZ = 0.0f;

		TranslationChanged = TRUE;        
	}

	return;
}


/*
 *****************************************************************************
 */
static void
GetRotation(void)
{
	RotationChanged = FALSE;

	if( ClumpRotate )
	{
		ClumpRotation.x += ClumpRotateDelta.x * CLUMPROTATIONINC;
		ClumpRotation.y += ClumpRotateDelta.y * CLUMPROTATIONINC;
		RotationChanged = TRUE;

		/* 
		 * Bound the rotation...
		 */
		if( fabs(ClumpRotation.x) > CLUMPROTATIONMAXSPEED )
		{
			ClumpRotation.x = (ClumpRotation.x > 0.0f)?
							   CLUMPROTATIONMAXSPEED :
							   -CLUMPROTATIONMAXSPEED;
		}
		if( fabs(ClumpRotation.y) > CLUMPROTATIONMAXSPEED )
		{
			ClumpRotation.y = (ClumpRotation.y > 0.0f)?
							   CLUMPROTATIONMAXSPEED :
							   -CLUMPROTATIONMAXSPEED;
		}
	}

	/* 
	 * If input ends, slow the moving clump to a stop...
	 */
	if( !ClumpRotate && !_bSpinOn && (ClumpRotation.x != 0.0f || ClumpRotation.y != 0.0f) )
	{
		if( fabs(ClumpRotation.x) < 1.0f )
		{
			ClumpRotation.x = 0.0f;
		}
		else
		{
			ClumpRotation.x += (ClumpRotation.x < 0.0f)?
								  CLUMPROTATIONDEC :
								  -CLUMPROTATIONDEC ;
		}

		if( fabs(ClumpRotation.y) < 1.0f )
		{
			ClumpRotation.y = 0.0f;
		}
		else
		{
			ClumpRotation.y += (ClumpRotation.y < 0.0f)?
								  CLUMPROTATIONDEC :
								  -CLUMPROTATIONDEC ;
		}

		ClumpRotateDelta.x = ClumpRotateDelta.y = ClumpRotateDelta.z = 0.0f;
		RotationChanged = TRUE;        
	}

	return;
}

/*
 *****************************************************************************
 */
static void
ClumpControlTranslateZ(RwReal deltaZ)
{
	RwFrame *clumpFrame = (RwFrame *)NULL;

	clumpFrame = RpClumpGetFrame(_pScene->getClump());

	if( clumpFrame )
	{
		RwV3d translation = {0.0f, 0.0f, 0.0f};
		translation.z = -deltaZ;
		RwFrameTranslate(clumpFrame, &translation, rwCOMBINEPOSTCONCAT);
	}
	return;
}

//@{ Jaewon 20040915
/*
 *****************************************************************************
 */
static void
ClumpControlTranslateXY(RwReal deltaX, RwReal deltaY)
{
	RwFrame *clumpFrame = (RwFrame *)NULL;

	clumpFrame = RpClumpGetFrame(_pScene->getClump());

	if( clumpFrame )
	{
		RwV3d translation = {0.0f, 0.0f, 0.0f};
		translation.x = deltaX;
		translation.y = -deltaY;
		RwFrameTranslate(clumpFrame, &translation, rwCOMBINEPOSTCONCAT);
	}
	return;
}
//@} Jaewon

/*
 *****************************************************************************
 */
static void
ClumpControlRotate(RwReal deltaX, RwReal deltaY)
{
	RwFrame *clumpFrame = (RwFrame *)NULL;

	clumpFrame = RpClumpGetFrame(_pScene->getClump());

	if( clumpFrame )
	{
		RwMatrix *cameraMatrix;
		RwV3d right, up, clumpPos;

		cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(_pScene->getCamera()));
		right = *RwMatrixGetRight(cameraMatrix);
		up = *RwMatrixGetUp(cameraMatrix);

		/*
		 * Rotate about the clump's bounding sphere center...
		 */
		RwV3dTransformPoints(&clumpPos, &(_pScene->m_clumpBoundingSphere.center), 1, RwFrameGetLTM(clumpFrame));

		/*
		 * First translate back to the origin...
		 */
		RwV3dScale(&clumpPos, &clumpPos, -1.0f);
		RwFrameTranslate(clumpFrame, &clumpPos, rwCOMBINEPOSTCONCAT);

		/*
		 * ...do the rotations...
		 */
		RwFrameRotate(clumpFrame, &up, deltaY, rwCOMBINEPOSTCONCAT);
		RwFrameRotate(clumpFrame, &right, -deltaX, rwCOMBINEPOSTCONCAT);

		/*
		 * ...and translate back...
		 */
		RwV3dScale(&clumpPos, &clumpPos, -1.0f);
		RwFrameTranslate(clumpFrame, &clumpPos, rwCOMBINEPOSTCONCAT);
    
	}    
	return;
}


/*
 *****************************************************************************
 */
void
updateClumpControl(RwReal delta)
{
	if (_pScene->getClump())
	{
		GetZTranslation();

		if( TranslationChanged )
		{
			ClumpControlTranslateZ(ClumpTranslationZ * delta);
		}
    
		GetRotation();

		if( RotationChanged || _bSpinOn)
		{
			ClumpControlRotate(ClumpRotation.x * delta, ClumpRotation.y * delta);
		}
	}
	return;
}

/*
 *****************************************************************************
 */
void
ClumpControlDirectTranslateZ(RwReal deltaZ)
{    
	ClumpControlTranslateZ(deltaZ);

	return;
}


/*
 *****************************************************************************
 */
void
ClumpControlDirectRotate(RwReal deltaX, RwReal deltaY)
{
	ClumpControlRotate(deltaX, deltaY);

	if( _bSpinOn )
	{
		/*
		 * Make sure spin is large enough to be noticed...
		 */
		if( fabs(deltaX) > 0.25f )
		{
			ClumpRotation.x = deltaX * CLUMPSPINFACTOR;
		}
		if( fabs(deltaY) > 0.25f )
		{
			ClumpRotation.y = deltaY * CLUMPSPINFACTOR;
		}
	}

	return;
}

/*
 *****************************************************************************
 */
void
resetClumpControl(void)
{
	/*
	 * Initialize parameters for manipulation based on clump bounding sphere
	 */
	ClumpTranslationMaxSpeed = _pScene->m_clumpBoundingSphere.radius * CLUMPTRANSLATIONSPEEDFACTOR;
	ClumpTranslationInc = ClumpTranslationMaxSpeed * CLUMPINCFACTOR;
	ClumpTranslationDec = ClumpTranslationMaxSpeed * CLUMPDECFACTOR;

	ClumpTranslationZ = 0.0f;
	ClumpRotation = Zero;

	return;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonDown(RsMouseStatus * mouseStatus __RWUNUSED__)
{
	if(_pScene->getClump())
	{
		ClumpDirectTranslate = TRUE;
		return rsEVENTPROCESSED;
	}

	return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonUp(RsMouseStatus *mouseStatus __RWUNUSED__)
{
	ClumpDirectTranslate = FALSE;

	return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonDown(RsMouseStatus *mouseStatus)
{
	if(_pScene->getClump())
	{
		if(ClumpPick)
		{
			_pFXUI->findClosestMaterialToPosition(_pScene->getClump(), _pScene->getCamera(), &mouseStatus->pos);
		}
    
		ClumpDirectRotate = TRUE;
	}

	return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonUp(RsMouseStatus * mouseStatus __RWUNUSED__)
{
	ClumpDirectRotate = FALSE;

	return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandleMouseMove(RsMouseStatus *mouseStatus)
{
	/*
	 * Mouse move event handling...
	 */
	if(ClumpDirectRotate)
	{
		ClumpControlDirectRotate(mouseStatus->delta.y * 0.5f, 
			mouseStatus->delta.x * 0.5f );
	}

	if(ClumpDirectTranslate)
	{
		//@{ Jaewon 20041103
		// the offset should be adjusted according to the clump's size.
		//@{ Jaewon 20040915
		if(ClumpPick)
		// panning
			ClumpControlTranslateXY(mouseStatus->delta.x * 0.01f * _pScene->m_clumpBoundingSphere.radius, mouseStatus->delta.y * 0.01f * _pScene->m_clumpBoundingSphere.radius);
		else
		// zooming
			ClumpControlDirectTranslateZ(-mouseStatus->delta.y * 0.005f * _pScene->m_clumpBoundingSphere.radius);
		//@} Jaewon
		//@} Jaewon
	}

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
/*	if(MenuMouseHandler(event, param) == rsEVENTPROCESSED)
	{
		return rsEVENTPROCESSED;
	}
*/
	/*
	 * ...then the application events, if necessary...
	 */
	switch(event)
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

		default:
		{
			return rsEVENTNOTPROCESSED;
		}
	}

	return rsEVENTNOTPROCESSED;
}

//@{ Jaewon 20040917
static RpAtomic *atomicDisableEffectCB(RpAtomic *atomic, void *data)
{
    RpAtomicFxDisable(atomic);
    return atomic;
}
static RpAtomic *atomicEnableEffectCB(RpAtomic *atomic, void *data)
{
    RpAtomicFxEnable(atomic);
    return atomic;
}
//@} Jaewon

/*
 *****************************************************************************
 */
static RsEventStatus
HandleKeyDown(RsKeyStatus *keyStatus)
{
	switch(keyStatus->keyCharCode)
	{
		case rsPGUP:
		{
			/*
			 * PAGE-UP...
			 */
			ClumpTranslate = TRUE;
			ClumpTranslateDeltaZ = -1.0f;
			NumTranslationKeys++;
			return rsEVENTPROCESSED;
		}

		case rsPGDN:
		{
			/*
			 * PAGE-DOWN...
			 */
			ClumpTranslate = TRUE;
			ClumpTranslateDeltaZ = 1.0f;
			NumTranslationKeys++;
			return rsEVENTPROCESSED;
		}

		case rsLEFT:
		{
			/*
			 * CURSOR-LEFT...
			 */
			ClumpRotate = TRUE;
			ClumpRotateDelta.y = -1.0f;
			NumRotationKeys++;
			return rsEVENTPROCESSED;
		}

		case rsRIGHT:
		{
			/*
			 * CURSOR-RIGHT...
			 */
			ClumpRotate = TRUE;
			ClumpRotateDelta.y = 1.0f;
			NumRotationKeys++;
			return rsEVENTPROCESSED;
		}

		case rsUP:
		{
			/*
			 * CURSOR-UP...
			 */
			ClumpRotate = TRUE;
			ClumpRotateDelta.x = 1.0f;
			NumRotationKeys++;
			return rsEVENTPROCESSED;
		}

		case rsDOWN:
		{
			/*
			 * CURSOR-DOWN...
			 */
			ClumpRotate = TRUE;
			ClumpRotateDelta.x = -1.0f;
			NumRotationKeys++;
			return rsEVENTPROCESSED;
		}

		case rsLCTRL:
		case rsRCTRL:
		{
			/*
			 * CONTROL KEY
			 */
			 ClumpPick = TRUE;

			 return rsEVENTPROCESSED;
		}

		//@{ Jaewon 20040909
		case rsLALT:
		case rsRALT:
		{
			_bAltKey = true;
			return rsEVENTPROCESSED;
		}
		//@} Jaewon


		//@{ Jaewon 20040917
		// test fx disable/enable with no reinstancing.
		case rsF1:
		{
			RpClumpForAllAtomics(_pScene->getClump(), atomicDisableEffectCB, NULL);
			return rsEVENTPROCESSED;
		}
		//@} Jaewon

		//@{ Jaewon 20041103
		// toggle the model stats display.
		case rsF2:
		{
			toggleModelStatsDisplay();
			return rsEVENTPROCESSED;
		}
		//@} Jaewon

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
HandleKeyUp(RsKeyStatus *keyStatus)
{
	/*
	 * Whatever you want or...
	 */
	switch(keyStatus->keyCharCode)
	{
		case rsLEFT:
		case rsRIGHT:
		case rsUP:
		case rsDOWN:
		{
			/*
			 * CURSOR-RIGHT or CURSOR-LEFT...
			 * CURSOR-DOWN or CURSOR-UP...
			 */
			NumRotationKeys--;
			if ( NumRotationKeys <= 0 )
			{
				ClumpRotate = FALSE;
				NumRotationKeys = 0;
				ClumpRotateDelta.x = ClumpRotateDelta.y = 0.0f;
			}
			return rsEVENTPROCESSED;
		}

		case rsPGUP:
		case rsPGDN:
		{
			/*
			 * PAGE-DOWN or PAGE-UP..
			 */
			NumTranslationKeys--;
			if( NumTranslationKeys == 0 )
			{
				ClumpTranslate = FALSE;
				ClumpTranslateDeltaZ = 0.0f;
			}
			return rsEVENTPROCESSED;
		}

		case rsLCTRL:
		case rsRCTRL:
		{
			/*
			 * CONTROL KEY
			 */
			 ClumpPick = FALSE;

			 return rsEVENTPROCESSED;
		}

		//@{ Jaewon 20040909
		case rsLALT:
		case rsRALT:
		{
			_bAltKey = false;
			return rsEVENTPROCESSED;
		}
		//@} Jaewon

		//@{ Jaewon 20040917
		// test fx disable/enable with no reinstancing.
		case rsF1:
		{
			RpClumpForAllAtomics(_pScene->getClump(), atomicEnableEffectCB, NULL);
			return rsEVENTPROCESSED;
		}
		//@} Jaewon
		
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
/*	if(MenuKeyboardHandler(event, param) == rsEVENTPROCESSED)
	{
		return rsEVENTPROCESSED;
	}
*/
	/*
	 * ...then the application events, if necessary...
	 */
	switch(event)
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

	return rsEVENTNOTPROCESSED;

}


/*
 *****************************************************************************
 */
RwBool
attachInputDevices(void)
{
	RsInputDeviceAttach(rsKEYBOARD, KeyboardHandler);

	RsInputDeviceAttach(rsMOUSE, MouseHandler);

	return TRUE;
}

/*
 *****************************************************************************
 */
