#ifndef __AGCM_UI_LOGIN_SETTING_CAMERA_H__
#define __AGCM_UI_LOGIN_SETTING_CAMERA_H__



#include "rwcore.h"
#include "ApDefine.h"
#include "ContainerUtil.h"


enum LoginSettingDataType
{
	NoData										= 0,

	LoginDivision,

	CharacterSelect_Position,
	CharacterSelect_MoveOffSet,
	CharacterCreate_Position,

	CameraTM_Login,
	CameraTM_CharacterSelect,
	CameraTM_CharacterCreate,
	CameraTM_CharacterCreateZoom,

	DataTypeCount,
};


struct stViewSet
{
public :
	RwMatrix									m_matTM;
	float										m_fPerspective;

	stViewSet( void ) : m_fPerspective( 1.0f )
	{
	}

	void			MoveTo						( RwV3d vTargetPos );
	void			MoveForward					( float fDistance );
	void			MoveBackward				( float fDistance );
	void			MoveLeft					( float fDistance );
	void			MoveRight					( float fDistance );
	void			MoveUp						( float fDistance );
	void			MoveDown					( float fDistance );

	void			RotateFront					( float fAngle );
	void			RotateBack					( float fAngle );
	void			RotateLeft					( float fAngle );
	void			RotateRight					( float fAngle );

	void			LookAt						( RwV3d vTargetPos );

	void			Initialize					( void ) { RwMatrixSetIdentity( &m_matTM ); m_fPerspective = 1.0f; }
	void			CopyFrom					( RwMatrix matTM, float fPerspective );

private :
	void			_MoveBy						( RwV3d vMoveDistance );
	void			_RotateBy					( RwV3d vAxis, float fAngle );
	RwV3d			_NormalizeVector			( RwV3d vVector );
};



class AgcmUILoginSettingCamera
{
private :
	stViewSet									m_CamSetLogin;				// Login Window View
	stViewSet									m_CamSetCharacterSelect;	// Character Select Window View

	ContainerMap< AuRaceType, stViewSet >		m_CamSetCharacterCreate;	// Character Create Window View, By Race

	ContainerMap< AuCharClassType, stViewSet >	m_CamSetForHuman;			// Zoom setting for Human, By Class
	ContainerMap< AuCharClassType, stViewSet >	m_CamSetForOrc;				// Zoom setting for Orc, By Class
	ContainerMap< AuCharClassType, stViewSet >	m_CamSetForMoonElf;			// Zoom setting for MoonElf, By Class
	ContainerMap< AuCharClassType, stViewSet >	m_CamSetForDragonScion;		// Zoom setting for DragonSion, By Class

public :
	AgcmUILoginSettingCamera( void )			{ 	}
	~AgcmUILoginSettingCamera( void )			{	}

public :
	void			ClearCameraData				( void );

	void			SetLoginTM					( RwMatrix matTM, float fPerspective );
	void			SetSelectTM					( RwMatrix matTM, float fPerspective );
	void			AddCreateTM					( AuRaceType eRace, RwMatrix matTM, float fPerspective );
	void			AddZoomTM					( AuRaceType eRace, AuCharClassType eClass, RwMatrix matTM, float fPerspective );

	RwMatrix*		GetLoginTM					( void );
	RwMatrix*		GetSelectTM					( void );
	RwMatrix*		GetCreateTM					( AuRaceType eRace );
	RwMatrix*		GetZoomTM					( AuRaceType eRace, AuCharClassType eClass );

	float			GetLoginPerspective			( void );
	float			GetSelectPerspective		( void );
	float			GetCreatePerspective		( AuRaceType eRace );
	float			GetZoomPerspective			( AuRaceType eRace, AuCharClassType eClass );
};


#endif