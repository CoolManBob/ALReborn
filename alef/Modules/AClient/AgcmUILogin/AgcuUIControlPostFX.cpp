#include "AgcuUIControlPostFX.h"
#include "AgcmUILogin.h"


AgcuUIControlPostFX::AgcuUIControlPostFX( void )
: m_pPostFX( NULL ), m_bIsTranslate( FALSE ), m_bIsPostFXOn( FALSE ),
	m_fTranslateElapsedTime( 0.0f ), m_fTranslateTimeScale( 1.0f )
{
	memset( m_strPostFXPipeLine, 0, sizeof( CHAR ) * 256 );
}

BOOL AgcuUIControlPostFX::OnInitialzie( void* pUILogin )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pUILogin;
	if( !pcmUILogin ) return FALSE;

	m_pPostFX = ( AgcmPostFX* )pcmUILogin->GetModule( "AgcmPostFX" );
	if( !m_pPostFX ) return FALSE;

	m_bIsTranslate = FALSE;
	m_bIsPostFXOn = FALSE;

	m_fTranslateElapsedTime = 0.0f;
	m_fTranslateTimeScale = 1.0f;

	memset( m_strPostFXPipeLine, 0, sizeof( CHAR ) * 256 );
	return TRUE;
}

BOOL AgcuUIControlPostFX::OnUpdate( float fElapsedTime )
{
	if( !m_pPostFX ) return FALSE;
	if( !m_bIsTranslate ) return FALSE;

	// calculate passed time
	float fScaleTime = m_fTranslateTimeScale * fElapsedTime;
	m_fTranslateElapsedTime += fElapsedTime;

	if( m_fTranslateElapsedTime < 1.0f )
	{
		// passed time under 1 second, save time radius
		m_pPostFX->setRadius( m_fTranslateElapsedTime );
	}
	else
	{
		// if PostFX is enable..
		if( m_bIsPostFXOn )
		{
			// set rendering pipeline to PostFX, and On it
			m_pPostFX->setPipeline( m_strPostFXPipeLine );
			m_pPostFX->On();
		}
		else
		{
			// off PostFX
			m_pPostFX->Off( true );
		}

		// terminate FX
		m_bIsTranslate = FALSE;
	}

	return TRUE;
}

BOOL AgcuUIControlPostFX::OnStartPostFX( void )
{
	// it must enable PostFX, and not running current
	if( !m_pPostFX ) return FALSE;
	if( m_bIsTranslate ) return FALSE;

	if( !m_pPostFX->isReady() )
	{
		m_pPostFX->createRenderTargets();
	}

	m_fTranslateTimeScale = 2.0f;
	if( g_pEngine )
	{
		g_pEngine->OnRender();
	}

	m_pPostFX->saveImmediateFrame();

	// save PostFX enable or not
	m_bIsPostFXOn = m_pPostFX->isOn();
	if( m_bIsPostFXOn )
	{
		// save FX pipeline
		m_pPostFX->getPipeline( m_strPostFXPipeLine, 255 );
	}
	else
	{
		memset( m_strPostFXPipeLine, 0, sizeof( CHAR ) * 256 );
	}

	char strBuffer[ 512 ] = { 0, };
	strcpy( strBuffer, m_strPostFXPipeLine );

	if( strlen( m_strPostFXPipeLine ) > 0 )
	{
		strcat( strBuffer, "-" );
	}

	strcat( strBuffer, "TransCrossFade" );

	m_pPostFX->setPipeline( strBuffer );
	m_pPostFX->On();
	m_pPostFX->setRadius( 0.0f );

	m_fTranslateElapsedTime = 0.0f;
	m_bIsTranslate = TRUE;

	return TRUE;
}

BOOL AgcuUIControlPostFX::OnStopPostFX( void )
{
	// it must enable PostFX, and not running current
	if( !m_pPostFX ) return FALSE;

	m_pPostFX->Off();
	return TRUE;
}
