#include "AgcuUIControlCamera.h"
#include "AgcmLogin.h"
#include "AgcmUILoginSetting.h"
#include "AgcmCamera2.h"



AgcuUIControlCamera::AgcuUIControlCamera( void )
: m_nLoginMode( AGCMLOGIN_MODE_PRE_LOGIN ), m_pCamera( NULL ), m_pNature( NULL ), 
	m_eCurrentRaceType( AURACE_TYPE_NONE ), m_eCurrentClassType( AUCHARCLASS_TYPE_NONE ),
	m_eCurrentZoomStatus( Zoom_Ready ), m_Calculator( 0.0f, 0.0f, 0.0f, 0.0f )
{	
	m_vZoomCurrPos.x = 0.0f;
	m_vZoomCurrPos.y = 0.0f;
	m_vZoomCurrPos.z = 0.0f;

	m_vZoomCurrAt.x = 0.0f;
	m_vZoomCurrAt.y = 0.0f;
	m_vZoomCurrAt.z = 0.0f;

	m_fZoomCurrPerspective = 1.0f;

	m_vZoomNextPos.x = 0.0f;
	m_vZoomNextPos.y = 0.0f;
	m_vZoomNextPos.z = 0.0f;

	m_vZoomNextAt.x = 0.0f;
	m_vZoomNextAt.y = 0.0f;
	m_vZoomNextAt.z = 0.0f;

	m_fZoomNextPerspective = 1.0f;
}

INT32 AgcuUIControlCamera::OnUpdate( float fElapsed )
{
	if( !fElapsed ) return 0;

	switch( m_nLoginMode )
	{
	case AGCMLOGIN_MODE_CHARACTER_CREATE :	return _UpdateCharacterCreate( fElapsed );	break;
	}

	return 0;
}

INT32 AgcuUIControlCamera::OnChangeMode( int nMode )
{
	int nPrevMode = m_nLoginMode;
	m_nLoginMode = nMode;

	RwMatrix* pmatTM = NULL;
	float fPerspective = 0.0f;

	switch( m_nLoginMode )
	{
	case AGCMLOGIN_MODE_SERVER_SELECT :
		{
			pmatTM = GetUILoginSetting()->GetLoginTM();
			fPerspective = GetUILoginSetting()->GetLoginPerspective();

			_UpdateCameraAndNature( pmatTM, fPerspective );
		}
		break;

	case AGCMLOGIN_MODE_CHARACTER_SELECT :
		{
			pmatTM = GetUILoginSetting()->GetSelectTM();
			fPerspective = GetUILoginSetting()->GetSelectPerspective();

			_UpdateCameraAndNature( pmatTM, fPerspective );
		}
		break;

	case AGCMLOGIN_MODE_CHARACTER_CREATE :
		{
			if( nPrevMode != AGCMLOGIN_MODE_WAIT_MY_CHARACTER )
			{
				pmatTM = GetUILoginSetting()->GetCreateTM( AURACE_TYPE_HUMAN );
				fPerspective = GetUILoginSetting()->GetCreatePerspective( AURACE_TYPE_HUMAN );

				_UpdateCameraAndNature( pmatTM, fPerspective );
			}
		}
		break;
	}

	return nPrevMode;
}

INT32 AgcuUIControlCamera::OnChangeRace( AuRaceType eRaceType, AuCharClassType eClassType )
{
	if( !m_pCamera ) return 0;
	if( m_nLoginMode != AGCMLOGIN_MODE_CHARACTER_CREATE ) return 0;
	if( !_IsValidRaceAndClassType( eRaceType, eClassType ) ) return 0;

	m_eCurrentZoomStatus = Zoom_Ready;
	m_eCurrentRaceType = eRaceType;
	m_eCurrentClassType = eClassType;

	RwMatrix* pCameraTM = GetUILoginSetting()->GetCreateTM( m_eCurrentRaceType );
	float fPerspective = GetUILoginSetting()->GetCreatePerspective( m_eCurrentRaceType );

	if( pCameraTM )
	{
		m_vZoomCurrPos = pCameraTM->pos;
		m_vZoomCurrAt = pCameraTM->at;
		m_fZoomCurrPerspective = fPerspective;
	}

	RwMatrix* pZoomTM = GetUILoginSetting()->GetZoomTM( m_eCurrentRaceType, m_eCurrentClassType );
	float fZoomPerspective = GetUILoginSetting()->GetZoomPerspective( m_eCurrentRaceType, m_eCurrentClassType );

	if( pZoomTM )
	{
		m_vZoomNextPos = pZoomTM->pos;
		m_vZoomNextAt = pZoomTM->at;
		m_fZoomNextPerspective = fZoomPerspective;
	}

	_UpdateCameraAndNature( pCameraTM, fPerspective );
	return 0;
}

INT32 AgcuUIControlCamera::OnClickBtnZoom( BOOL bUseForcedZoomStatus, BOOL bForcedZoomStatus )
{
	if( m_nLoginMode != AGCMLOGIN_MODE_CHARACTER_CREATE ) return 0;

	if( bUseForcedZoomStatus )
	{
		switch( m_eCurrentZoomStatus )
		{
		case Zoom_RunningZoomOut :
		case Zoom_RunningZoomIn :
			{
				if( bForcedZoomStatus )
				{
					m_eCurrentZoomStatus = Zoom_RunningZoomIn;
				}
				else
				{
					m_eCurrentZoomStatus = Zoom_RunningZoomOut;
				}

				m_Calculator.accumtick = 1.0f - m_Calculator.accumtick;
				return 0;
			}
			break; 
		}		
	}

	switch( m_eCurrentZoomStatus )
	{
	case Zoom_Ready :
		{
			m_eCurrentZoomStatus = Zoom_RunningZoomIn;
			new( &m_Calculator ) AgcuAccmOnePlusSin( 180.f, 270.f, 1.0f, 1.0f );
		}
		break;

	case Zoom_End :
		{
			m_eCurrentZoomStatus = Zoom_RunningZoomOut;
			new( &m_Calculator ) AgcuAccmOnePlusSin( 180.f, 270.f, 1.0f, 1.0f );
		}
		break;
	}

	AXISVIEW::ClearAxis();
	return 0;
}

void AgcuUIControlCamera::SetCameraAndNature( void* pCamera, void* pNature )
{
	if( !pCamera ) return;
	if( !pNature ) return;

	m_pCamera = pCamera;
	m_pNature = pNature;
}

INT32 AgcuUIControlCamera::_UpdateCharacterCreate( float fElapsed )
{
	float fLength = 0.0f;
	float fPerspective = 0.0f;

	fElapsed *= 1.25f;

	switch( m_eCurrentZoomStatus )
	{
	case Zoom_RunningZoomOut :
		{
			if( m_Calculator.bOnIdle( &fLength, fElapsed ) )
			{
				m_eCurrentZoomStatus = Zoom_Ready;
			}

			_UpdateCameraZoom( fLength, Zoom_RunningZoomOut );
		}
		break;

	case Zoom_RunningZoomIn :
		{
			if( m_Calculator.bOnIdle( &fLength, fElapsed ) )				
			{
				m_eCurrentZoomStatus = Zoom_End;
			}

			_UpdateCameraZoom( fLength, Zoom_RunningZoomIn );
		}
		break;
	}

	return 0;
}

void AgcuUIControlCamera::_UpdateCameraAndNature( RwMatrix* pmatTM, float fPerspective )
{
	if( !pmatTM ) return;
	if( !m_pCamera ) return;
	AgcmCamera2* pCamera = ( AgcmCamera2* )m_pCamera;

	if( !m_pNature ) return;
	AgcmEventNature* pNature = ( AgcmEventNature* )m_pNature;

	pCamera->bSetCamFrm( *pmatTM );
	if( g_pEngine )
	{
		g_pEngine->SetProjection( fPerspective );
	}

	if( pNature->SetCharacterPosition( *( const AuPOS* )pCamera->bGetPtrEye(), TRUE ) )
	{
		pNature->ApplySkySetting();
	}
}

void AgcuUIControlCamera::_UpdateCameraZoom( float fLength, AuCameraZoomStatus eZoomStatus )
{
	if( !g_pEngine ) return;
	if( !m_pCamera ) return;
	AgcmCamera2* pCamera = ( AgcmCamera2* )m_pCamera;

	RwV3d vCameraPos;
	RwV3d vLookAt;

	T_CLAMP( fLength, 0.0f, 1.0f );

	switch( eZoomStatus )
	{
	case Zoom_RunningZoomOut :	LinearIntp( &vCameraPos, &m_vZoomNextPos, &m_vZoomCurrPos, fLength );	break;
	case Zoom_RunningZoomIn :	LinearIntp( &vCameraPos, &m_vZoomCurrPos, &m_vZoomNextPos, fLength );	break;
	default :					return;																	break;
	}
	

	vLookAt = m_vZoomCurrAt;
	RwV3dAdd( &vLookAt, &vLookAt, &vCameraPos );

	float fPerspective = 0.0f;
	switch( eZoomStatus )
	{
	case Zoom_RunningZoomOut :	fPerspective = m_fZoomNextPerspective + ( ( m_fZoomCurrPerspective - m_fZoomNextPerspective ) * fLength );	break;
	case Zoom_RunningZoomIn :	fPerspective = m_fZoomCurrPerspective + ( ( m_fZoomNextPerspective - m_fZoomCurrPerspective ) * fLength );	break;
	default :					return;																										break;
	}

	pCamera->bSetCamFrm( vLookAt, vCameraPos );
	g_pEngine->SetProjection( fPerspective );
}

BOOL AgcuUIControlCamera::_IsValidRaceAndClassType( AuRaceType eRaceType, AuCharClassType eClassType )
{
	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :					break;
	case AURACE_TYPE_ORC :						break;
	case AURACE_TYPE_MOONELF :					break;
	case AURACE_TYPE_DRAGONSCION :				break;
	default	:									return FALSE;
	}

	switch( eClassType )
	{
	case AUCHARCLASS_TYPE_KNIGHT :				break;
	case AUCHARCLASS_TYPE_RANGER :				break;
	case AUCHARCLASS_TYPE_SCION :				break;
	case AUCHARCLASS_TYPE_MAGE :				break;
	default :									return FALSE;
	}

	return TRUE;
}
