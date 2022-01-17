#ifndef __AGCU_UI_CONTROL_POST_FX_H__
#define __AGCU_UI_CONTROL_POST_FX_H__



#include "AgcmPostFX.h"



class AgcuUIControlPostFX
{
private :
	AgcmPostFX*								m_pPostFX;					// PostFX

	BOOL									m_bIsTranslate;				// is FX running
	BOOL									m_bIsPostFXOn;				// is usable PostFX

	float									m_fTranslateElapsedTime;	// time from start running
	float									m_fTranslateTimeScale;		// time scale factor

	CHAR									m_strPostFXPipeLine[ 256 ];	// FX rendering pipeline

public :
	AgcuUIControlPostFX( void );
	virtual ~AgcuUIControlPostFX( void )	{	}

public :
	BOOL			OnInitialzie			( void* pUILogin );
	BOOL			OnUpdate				( float fElapsedTime );
	BOOL			OnStartPostFX			( void );
	BOOL			OnStopPostFX			( void );
};



#endif