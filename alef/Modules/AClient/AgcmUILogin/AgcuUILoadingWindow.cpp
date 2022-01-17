#include "AgcuUILoadingWindow.h"
#include "AgcmUIManager2.h"
#include "AgcmSound.h"
#include "AgcmUIOption.h"




AgcuUILoadingWindow::AgcuUILoadingWindow( void )
: m_nMax( 0 ), m_nCurrent( 0 ), m_bIsLoadingWindowOpen( FALSE ), m_bIsPostLoadingWindow( FALSE ),
	m_pUserDataLoadingMax( NULL ), m_pUserDataLoadingCurrent( NULL ), m_fSoundVolume( 0.0f )
{
}

BOOL AgcuUILoadingWindow::OnInitialize( void* pUILogin )
{
	m_nCurrent = 0;
	m_nMax = 0;

	m_bIsLoadingWindowOpen = FALSE;
	m_bIsPostLoadingWindow = FALSE;

	m_fSoundVolume = 0.0f;
	return TRUE;
}

BOOL AgcuUILoadingWindow::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_pUserDataLoadingMax = pcmUIManager->AddUserData( "LG_LoadingMax", &m_nMax, sizeof( INT32 ), 1, AGCDUI_USERDATA_TYPE_INT32 );
	m_pUserDataLoadingCurrent = pcmUIManager->AddUserData( "LG_LoadingCurrent", &m_nCurrent, sizeof( INT32 ), 1, AGCDUI_USERDATA_TYPE_INT32 );
	return TRUE;
}

BOOL AgcuUILoadingWindow::OnRefresh( void* pUIManager, INT32 nCurrent, INT32 nMax )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	if( m_nMax >= 0 )
	{
		m_nMax = nMax;
	}

	m_nCurrent = nCurrent;
	pcmUIManager->SetUserDataRefresh( ( AgcdUIUserData* )m_pUserDataLoadingCurrent );
	return TRUE;
}

static void AdjustUIPos( AgcmUIManager2 * pcmUIManager )
{
	std::string screenName = "UI_LG_Loading";
	std::string slidName = "LG_Slide";

	if( AgcmUIManager2::GetUIMode() != AGCDUI_MODE_1024_768 )
	{
		screenName += "_";
		slidName += "_";

		char tmp[10] = {0,};

		itoa( (int)AgcmUIManager2::GetModeWidth(), tmp, 10 );

		screenName += tmp;
		slidName += tmp;
	}

	AgcdUI* loadingScreen = pcmUIManager->GetUI( (char*)screenName.c_str() );
	AgcdUIControl* progBar = 0;

	if( loadingScreen )
	{
		progBar = pcmUIManager->GetControl( loadingScreen, (char*)slidName.c_str() );
	}

	RwRaster * raster = RwCameraGetRaster( g_pEngine->m_pCamera );

	if( raster && progBar )
	{
		// π”∏  « ≈Õ πŸ≤ﬁ
		for( int i=0; i<loadingScreen->m_pcsUIWindow->m_csTextureList.GetCount(); ++i)
		{
			RwTexture * tex = loadingScreen->m_pcsUIWindow->m_csTextureList.GetImage_Index( i );
			if( tex )
				RwTextureSetFilterMode( tex, rwFILTERLINEARMIPLINEAR );
		}
		for( int i=0; i<progBar->m_pcsBase->m_csTextureList.GetCount(); ++i)
		{
			RwTexture * tex = progBar->m_pcsBase->m_csTextureList.GetImage_Index( i );
			if( tex )
				RwTextureSetFilterMode( tex, rwFILTERLINEARMIPLINEAR );
		}

		static bool first = true;

		if( !first )
			return;

		first = false;

		// ¡¬«• ¡∂¡§
		float scale = raster->height / AgcmUIManager2::GetModeHeight();
		progBar->m_pcsBase->y = (int)(progBar->m_pcsBase->y * scale + 0.5f);
		progBar->m_pcsBase->h = (int)(progBar->m_pcsBase->h * scale + 0.5f);

		progBar->m_pcsBase->w = (int)( ( (AgcmUIManager2::GetModeWidth() * 0.5f) - progBar->m_pcsBase->x ) * 2.0f * scale + 0.5f );
		progBar->m_pcsBase->x = (raster->width * 0.5f) - (progBar->m_pcsBase->w * 0.5f);
	}
}

BOOL AgcuUILoadingWindow::OnOpen( void* pUIManager, INT32 nStart )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	AgcmSound* pcmSound = ( AgcmSound* )pcmUIManager->GetModule( "AgcmSound" );
	if( !pcmSound ) return FALSE;

	m_fSoundVolume = pcmSound->Get3DSampleVolume();
	pcmSound->Set3DSampleVolume( 0.0f );

	m_nCurrent = nStart;
	m_bIsLoadingWindowOpen = TRUE;

	//-----------------------------------------------------------------------
	// «ÿªÛµµø° ∏¬∞‘ ¡∂¿˝
	AdjustUIPos( pcmUIManager );
	//-----------------------------------------------------------------------

	pcmUIManager->SetUserDataRefresh( ( AgcdUIUserData* )m_pUserDataLoadingCurrent );
	return TRUE;
}

BOOL AgcuUILoadingWindow::OnUpdateAdd( void* pUIManager, INT32 nAddCount )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_nCurrent += nAddCount;
	pcmUIManager->SetUserDataRefresh( ( AgcdUIUserData* )m_pUserDataLoadingCurrent );
	return TRUE;
}

BOOL AgcuUILoadingWindow::OnClose( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	AgcmSound* pcmSound = ( AgcmSound* )pcmUIManager->GetModule( "AgcmSound" );
	if( !pcmSound ) return FALSE;

	m_bIsLoadingWindowOpen = FALSE;

	AgcmUIOption* pcmOption = ( AgcmUIOption* )g_pEngine->GetModule( "AgcmUIOption" );
	if( pcmOption && pcmOption->m_bEffectSound )
	{
		pcmSound->Set3DSampleVolume( m_fSoundVolume );
		pcmOption->UpdateEffSoundVal( TRUE );
		pcmOption->UpdateEnvSoundVal( TRUE );
		pcmOption->UpdateBGMSoundVal( TRUE );
	}

	return TRUE;
}
