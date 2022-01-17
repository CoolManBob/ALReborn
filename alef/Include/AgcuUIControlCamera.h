#ifndef __AGCU_UI_CONTROL_CAMERA_H__
#define __AGCU_UI_CONTROL_CAMERA_H__



#include "ContainerUtil.h"
#include "rwcore.h"
#include "ApDefine.h"
#include "AcuMath.h"
USING_ACUMATH;


class AgcuUIControlCamera
{
	enum AuCameraZoomStatus
	{
		Zoom_Ready								= 0,
		Zoom_End,
		Zoom_RunningZoomOut,
		Zoom_RunningZoomIn,
	};

private :
	int											m_nLoginMode;

	void*										m_pCamera;
	void*										m_pNature;

	RwV3d										m_vZoomCurrPos;
	RwV3d										m_vZoomCurrAt;
	float										m_fZoomCurrPerspective;

	RwV3d										m_vZoomNextPos;
	RwV3d										m_vZoomNextAt;
	float										m_fZoomNextPerspective;

	AuRaceType									m_eCurrentRaceType;
	AuCharClassType								m_eCurrentClassType;
	AuCameraZoomStatus							m_eCurrentZoomStatus;

	AgcuAccmOnePlusSin							m_Calculator;

public :
	AgcuUIControlCamera( void );
	virtual ~AgcuUIControlCamera( void )		{	}

public :
	INT32			OnUpdate					( float fElapsed );
	INT32			OnChangeMode				( int nMode );
	INT32			OnChangeRace				( AuRaceType eRaceType, AuCharClassType eClassType = AUCHARCLASS_TYPE_KNIGHT );
	INT32			OnClickBtnZoom				( BOOL bUseForcedZoomStatus = FALSE, BOOL bForcedZoomStatus = FALSE );

public :
	INT32			GetLoginMode				( void ) { return m_nLoginMode; }
	void			SetCameraAndNature			( void* pCamera, void* pNature );

private :
	INT32			_UpdateCharacterCreate		( float fElapsed );
	void			_UpdateCameraAndNature		( RwMatrix* pmatTM, float fPerspective );
	void			_UpdateCameraZoom			( float fLength, AuCameraZoomStatus eZoomStatus );
	BOOL			_IsValidRaceAndClassType	( AuRaceType eRaceType, AuCharClassType eClassType );
};



#endif