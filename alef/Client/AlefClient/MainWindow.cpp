#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rpcollis.h"
#include "rtpick.h"
#include "rtcharse.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include <win.h>
#include <MATH.h>

#include <MagDebug.h>

#include "ApUtil.h"
#include "ApMemoryTracker.h"
#include "AuProfileManager.h"

#include "MyEngine.h"
#include "MainWindow.h"
#include "AlefAdminAPI.h"

#ifdef _AREA_JAPAN_
	#include "AuJapaneseClientAuth.h"
	#include "AuGameInfoJpn.h"

	#ifndef _DEBUG
		#include "AuNPGameLib.h"
	#endif
#endif

#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
	#include "AuNPGameLib.h"
#endif

#ifdef _AREA_GLOBAL_
	#include "AuNPGameLib.h"	
#endif

//#include "AuNPGameLib.h"

extern MINIDUMP_TYPE	g_eMiniDumpType	;//

#define MAP_WIDTH	(10)
#define MAP_HEIGHT	(10)
#define MAP_SIZE	(MAP_WIDTH * MAP_HEIGHT)
#define NOV			(MAP_SIZE * 4)
#define NOT			((MAP_WIDTH - 1) * (MAP_HEIGHT - 1) * 2)

#define GEOMETRY_BLOCKING_OFFSET	(50.0f)

RwV3d VertexList[MAP_SIZE];
#define MAP(x,y)	(VertexList[(y) * MAP_WIDTH + (x)])
#define RWTEXTURE_REALEASE( x )		{ if( x ) {	RwTextureDestroy( x ); x = NULL; } }

BOOL	g_bRemove			= FALSE;

static char* s_szDShowName = "Movie\\Intro.dat";

// 일본용 Helper
#ifdef _AREA_JAPAN_

	void JapanAutoLogin( AgcmLogin* login, AgcmUILogin* uiLogin )
	{
		char password[64] = {"autologin"};

		login->SetIDPassword( const_cast<char*>(g_jAuth.GetMemberID()), password );
		login->ConnectLoginServer();
		
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )uiLogin->GetModule( "AgcmUIManager2" );
		g_pEngine->WaitingDialog(NULL, pcmUIManager->GetUIMessage("LOGIN_WAITING"));
	}

#endif

#ifdef _AREA_KOREA_
	void KoreaAutoLogin( AgcmLogin* login, AgcmUILogin* uiLogin )
	{
		CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
		if( !pWebzenAuth || !pWebzenAuth->IsAutoLogin() ) return;

		login->SetIDPassword( ( char* )pWebzenAuth->GetGameID(), "autologin" );

		AgcmUILogin* pcmUILogin = ( AgcmUILogin* )uiLogin->GetModule( "AgcmUILogin" );
		if( pcmUILogin )
		{
			pcmUILogin->ConnectLoginServer();
		}
	}

#endif

BOOL	__CBCustomCommandCallBack (PVOID pData, PVOID pClass, PVOID pCustData)
{
	char* pStrCommand = (char*)pData;
	CGetArg2* pArg = (CGetArg2*)pCustData;

	if( !pStrCommand || !pArg || !g_pcsAgcmChatting2 )		return FALSE;

	char	strMessage [ 256 ];
	if( !strcmp( pStrCommand , "/PS" ) )
	{
		if( pArg->GetArgCount() < 2 )
		{
			sprintf( strMessage , "퍼스펙티브 값 변경 인자 부족~" );
			g_pcsAgcmChatting2->AddSystemMessage( strMessage , 0xffff0000 );
		}
		else
		{
			FLOAT fPerspect = (FLOAT)atof( pArg->GetParam(1) );
			g_pEngine->SetProjection( fPerspect );

			sprintf( strMessage , "퍼스펙티브 값 변경 (%f)" , fPerspect );
			g_pcsAgcmChatting2->AddSystemMessage( strMessage , 0xffff0000 );
		}

		return TRUE;
	}
	else if( !strcmp( pStrCommand , "/WIDE" ) )
	{
		g_pEngine->SetWideScreen( !g_pEngine->GetWideScreen() );

		if( g_pEngine->GetWideScreen() )
			sprintf( strMessage , "와이드 스크린 모드로 변경" );
		else
			sprintf( strMessage , "일반 스크린 모드로 변경" );

		g_pcsAgcmChatting2->AddSystemMessage( strMessage , 0xffff0000 );
		return TRUE;
	}
	else if( !strcmp( pStrCommand , "/EC" ) )
	{
		if( pArg->GetArgCount() < 2 )
		{
			sprintf( strMessage , "퍼스펙티브 값 변경 인자 부족~" );
			g_pcsAgcmChatting2->AddSystemMessage( strMessage , 0xffff0000 );
		}
		else
		{
			INT32 nRoomType = atoi( pArg->GetParam( 1 ) );

			ASSERT( g_pEngine );
			ASSERT( g_pcsAgcmSound );

			if( nRoomType < 0 ) nRoomType = 0;
			if( nRoomType >= SoundEffectRoomType_MaxCount ) nRoomType = 0;

			g_pcsAgcmSound->SetRoom( ( eSoundEffectRoomType )nRoomType );
		}

		return TRUE;
	}
	else if( !strcmp( pStrCommand , "/LUA" ) )
	{
		if( pArg->GetArgCount() < 2 )
		{
			sprintf( strMessage , "Lua 커맨드 인자 부족" );
			g_pcsAgcmChatting2->AddSystemMessage( strMessage , 0xffff0000 );
		}
		else
		{
			// 첫번째 인자가 파일네임인지 분석
			AuLua * pLua = AuLua::GetPointer();

			char ext[ 256 ];
			_splitpath( pArg->GetParam(1) , NULL , NULL , NULL , ext );
			if( _stricmp( ext , ".LUA" ) == 0 )
			{
				// 파일이다.
				//pLua->RunScript( pArg->GetParam(1) );
				g_pEngine->Lua_RunScript( pArg->GetParam(1) , FALSE );
			}
			else
			{
				string	str;
				for( int i = 1 ; i < pArg->GetArgCount() ; i ++ )
				{
					str.append( pArg->GetParam(i) );
					str.append( " " );
				}

				pLua->RunString( str.c_str() );
			}
		}

		return TRUE;
	}

	return FALSE;
}

//------------------------ CTestPostEffectFX -------------------------------
void	CTestPostEffectFX::Update()
{
	if(m_bCircleFX)
	{
		float tmElapsed = float(clock()) / CLOCKS_PER_SEC - m_tmStart;
		if( tmElapsed < 5.0f )
			g_pcsAgcmPostFX->setRadius( tmElapsed * 0.2f );
		else
		{
			if( m_bPostFXOn )
			{
				g_pcsAgcmPostFX->setPipeline( m_pipeBU );
				g_pcsAgcmPostFX->On();
			}
			else
				g_pcsAgcmPostFX->Off(true);

			m_bCircleFX = false;
		}
	}
}

void	CTestPostEffectFX::PlayEffectFX( const char* szText )
{
	if( !m_bCircleFX )
	{
		if( m_bPostFXOn = g_pcsAgcmPostFX->isOn() )
			g_pcsAgcmPostFX->getPipeline( m_pipeBU, 1023 );
		else
			m_pipeBU[0] = '\0';
	}

	string strBuf( m_pipeBU );
	if( !strBuf.empty() )
		strBuf += "-";
	strBuf += szText;

	g_pcsAgcmPostFX->setPipeline( strBuf.c_str() );
	g_pcsAgcmPostFX->On();

	m_tmStart	= float( clock() ) / CLOCKS_PER_SEC;
	m_bCircleFX	= true;
}

//------------------------ CScreenModeManager -------------------------------
CScreenModeManager::CScreenModeManager() : m_bFullScreen( FALSE )
{
	VERIFY( EnumDisplaySettings( NULL , ENUM_CURRENT_SETTINGS , & m_PrevDevMode ) );

	HWND	hWnd = g_pEngine->GethWnd();
	GetWindowPlacement( hWnd , &m_WPPrev );
	m_dwPrevWindowStyle = (DWORD)::GetWindowLong(hWnd, GWL_STYLE );
}

CScreenModeManager::~CScreenModeManager()
{
	ReturnScreenMode();
}

void	CScreenModeManager::SetFullWindow()
{
	HWND	hWnd = g_pEngine->GethWnd();

	RECT rcDesktop;
	::GetWindowRect( ::GetDesktopWindow(), &rcDesktop );
	::SetWindowLong( hWnd , GWL_STYLE , WS_VISIBLE | WS_POPUP );

	WINDOWPLACEMENT WPNew;
	WPNew.length			= sizeof(WINDOWPLACEMENT);
	GetWindowPlacement( hWnd , &WPNew );
	WPNew.showCmd			= SW_SHOWNORMAL;
	WPNew.rcNormalPosition	= rcDesktop;
	SetWindowPlacement( hWnd , &WPNew );

	m_bFullScreen = TRUE;

	SetFocus( hWnd );
}

BOOL	CScreenModeManager::SetFullScreen( int nWidth , int nHeight , int nDepth )
{
	if( m_bFullScreen ) return TRUE;

	DEVMODE devMode = m_PrevDevMode;
	devMode.dmPelsWidth		= nWidth;
	devMode.dmPelsHeight	= nHeight;
	devMode.dmBitsPerPel	= nDepth;

	switch( ChangeDisplaySettings( &devMode, 0 ) )
	{
	case DISP_CHANGE_SUCCESSFUL:
		break;
	default:
		ASSERT( !"전체화면 변경실패" );
		return FALSE;
	}

	return TRUE;
}

void CScreenModeManager::ReturnScreenMode()
{
	if( m_bFullScreen )
	{
		HWND	hWnd = g_pEngine->GethWnd();
		ChangeDisplaySettingsEx( NULL, &m_PrevDevMode, NULL, 0, NULL );
		::SetWindowLong( hWnd , GWL_STYLE , m_dwPrevWindowStyle );
		::SetWindowPlacement( hWnd , &m_WPPrev);
		m_bFullScreen = FALSE;
	}
}

//------------------------ MainWindow -------------------------------
MainWindow::MainWindow( MyEngine& cMyEngine ) : m_cMyEngine( cMyEngine )
{
	m_eTemplatesLoaded	= AP_SERVICE_AREA_CHINA == g_eServiceArea ? 0 : 1;

	m_pLineList			= NULL;
	m_pIndex			= NULL;

	m_lShowHeightGrid				= 0;
	m_lShowGeometryBlocking			= 0;
	m_lShowSkyBlocking				= 0;
	m_lShowObjectBlocking			= 0;
	m_lShowRidableObjectAvailable	= 0;
	m_lShowCharacterBlocking		= 0;
	m_lShowLineBlocking				= 0;

	m_bUseModalMessage	= TRUE;

	m_bShowRidableInfo		= FALSE;
	m_bEnableTheaterMode	= FALSE;

#ifdef _BIN_EXEC_
	m_bDecryption = FALSE;
#else
	m_bDecryption = TRUE;
#endif // _BIN_EXEC_

}

MainWindow::~MainWindow()
{
	if( m_cMyEngine.m_bEmulationFullscreen )
		ReturnScreenMode();
}

// 디버그 트레이스 체크용..
extern BOOL g_bUseTrace;
static BOOL bLoadTemplates = FALSE;

BOOL MainWindow::LoadTemplates()
{
	ASSERT( m_eTemplatesLoaded == 2 );

	bLoadTemplates = TRUE;

	// 마고자 (2005-05-31 오후 3:18:31) : 
	// 프로파일링.
	UINT32	uStartTime		= GetTickCount();
	UINT32	uLastCheckTime	= uStartTime	;
	UINT32	uCurrentCheckTime = uStartTime	;
	char	strFile[ 1024 ];

	// 이걸 TRUE로 놓으면 starting.log 에 로그가 기록이 됨.
	BOOL	bLoggingFile = FALSE;

	if(g_pcsAgcmGrass)
		VERIFY(g_pcsAgcmGrass->LoadGrassInfoFromINI("Ini/GrassTemplate.ini",m_bDecryption));			// 임시

	g_pEngine->OnRender();

	if (g_pcsAgcmWater)
	{
		VERIFY(g_pcsAgcmWater->LoadHWaterStatusFromINI("Ini/HWaterStatus.ini", m_bDecryption));
		VERIFY(g_pcsAgcmWater->LoadStatusInfoT1FromINI("Ini/WaterStatusT1.ini", m_bDecryption));
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmLensFlare)
		VERIFY(g_pcsAgcmLensFlare	->StreamReadTemplate		("Ini/LensFlare.ini", m_bDecryption));
#ifdef _DEBUG
	MEMORYSTATUS	stMemory;
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... LensFlare.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmEventNPCDialog)
	{
		sprintf(strFile, "Ini\\%sNPCDialog.txt", m_cMyEngine.m_szLanguage);
		g_pcsAgpmEventNPCDialog->LoadNPCDialogRes(strFile, m_bDecryption);
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... TextBoard.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmTextBoardMng)
		VERIFY(g_pcsAgcmTextBoardMng	->StreamReadTemplate		("Ini/TextBoard.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... TextBoard.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmNatureEffect)
		VERIFY(g_pcsAgcmNatureEffect	->LoadNatureEffectInfoFromINI		("Ini/NatureEffect.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... NatureEffect.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmEventNature)
		VERIFY(g_pcsAgcmEventNature	->LoadTextureScriptFile	("Ini/alefskysetting.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... alefskysetting.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmEventNature)
		VERIFY(g_pcsAgpmEventNature	->SkySet_StreamRead		("Ini/SkySet.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... SkySet.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmTargeting)
		VERIFY(g_pcsAgcmTargeting	->StreamReadTemplate		("Ini/Targeting.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... Targeting.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmUIManager2)
	{
		sprintf(strFile, "Ini\\%sUIMessage.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgcmUIManager2   ->StreamReadUIMessage		(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... UIMessage.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmGuild)
	{
		VERIFY(g_pcsAgpmGuild->ReadRequireItemIncreaseMaxMember("Ini\\GuildMaxMember.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... GuildMaxMember.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		VERIFY( g_pcsAgpmGuild->ReadGuildMarkTemplate("Ini\\guildmark.txt", m_bDecryption) );

		if(g_pcsAgcmUIGuild)
		{
			//g_pcsAgcmUIGuild->InitGuildBattleUIMessage();
			g_pcsAgcmUIGuild->InitGuildMarkGridItem();
		}
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... guildmark.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmUIManager2)
	{
		sprintf(strFile, "Ini\\%sUI_1024x768.ini", m_cMyEngine.m_szLanguage);
		//VERIFY(g_pcsAgcmUIManager2->StreamRead(strFile, FALSE));
		VERIFY(g_pcsAgcmUIManager2->StreamRead(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... UI.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmUIControl)
		VERIFY(g_pcsAgcmUIControl	->UIIniRead				("INI\\UIDataList.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... UIDataList.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmEventTeleport)
		VERIFY(g_pcsAgpmEventTeleport	->StreamReadGroup	("Ini\\TeleportGroup.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... TeleportGroup.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmEventTeleport)
		VERIFY(g_pcsAgpmEventTeleport	->StreamReadPoint	("Ini\\TeleportPoint.ini", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... TeleportPoint.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	if (g_pcsAgpmEventTeleport)
		VERIFY(g_pcsAgpmEventTeleport	->StreamReadFee		("Ini\\TeleportFee.txt", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... TeleportFee.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsApmObject)
		VERIFY(g_pcsApmObject		->StreamReadTemplate("Ini\\ObjectTemplate.ini", NULL, NULL, NULL, m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ObjectTemplate.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmFactors)
		VERIFY(g_pcsAgpmFactors		->CharacterTypeStreamRead("Ini\\CharType.ini", m_bDecryption));	
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... CharType.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	g_pEngine->OnRender();

	if (g_pcsAgpmItem)
	{
		// 통파일 함 읽어보고..
		BOOL bResult = g_pcsAgpmItem->StreamReadTemplate( "Ini\\ItemTemplateAll.ini", NULL, m_bDecryption );
		if( !bResult )
		{
			// 통파일 읽기에 실패하면 쪼갠 파일 읽기를 시도한다.
			bResult = g_pcsAgpmItem->StreamReadTemplates( "Ini\\ItemTemplate", "INI\\ItemTemplateEntry.ini", NULL, m_bDecryption );
		}

		VERIFY( bResult );
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemTemplate.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmItem)
		VERIFY(g_pcsAgpmItem			->StreamReadBankSlotPrice	("Ini\\BankSlotPrice.txt", m_bDecryption));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... BankSlotPrice.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmSkill)
	{
		VERIFY( g_pcsAgpmSkill->StreamReadTemplate("Ini\\SkillTemplate.ini", NULL, m_bDecryption) );
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... SkillTemplate.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	//charactertemplate.ini 분리..
	if (g_pcsAgpmCharacter)
	{
		sprintf(strFile, "Ini\\%sCharacterTemplatePublic.ini", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmCharacter	->StreamReadTemplate		(strFile, NULL, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... CharacterTemplatePublic.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmCharacter) {
		VERIFY(g_pcsAgcmCharacter	->StreamReadTemplate			("Ini\\CharacterTemplateClient.ini", NULL, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterTemplateClient.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		VERIFY(g_pcsAgcmCharacter	->StreamReadTemplateAnimation	("Ini\\CharacterTemplateAnimation.ini", NULL, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterTemplateAnimation.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
		VERIFY(g_pcsAgcmCharacter	->StreamReadTemplateCustomize	("Ini\\CharacterTemplateCustomize.ini", NULL, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterTemplateCustomize.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmSkill) {
		VERIFY(g_pcsAgcmSkill	->StreamReadTemplateSkill	("Ini\\CharacterTemplateSkill.ini", NULL, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterTemplateSkill.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
		VERIFY(g_pcsAgcmSkill	->StreamReadTemplateSkillSound	("Ini\\CharacterTemplateSkillSound.ini", NULL, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterTemplateSkillSound.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmEventEffect) {
		VERIFY(g_pcsAgcmEventEffect	->StreamReadTemplate	("Ini\\CharacterTemplateEventEffect.ini", NULL, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterTemplateEventEffect.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... CharacterTemplateClient.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG


	CHAR	szBuffer[512];
	if (g_pcsAgpmCharacter)
	{
		VERIFY( g_pcsAgpmCharacter->StreamReadImportData("Ini\\CharacterDataTable.txt", szBuffer, m_bDecryption) );
		
		if (g_eServiceArea == AP_SERVICE_AREA_CHINA || g_eServiceArea == AP_SERVICE_AREA_KOREA || g_eServiceArea == AP_SERVICE_AREA_JAPAN)
			VERIFY( g_pcsAgpmCharacter->StreamReadCharismaTitle("Ini\\CharismaTitleTable.txt", m_bDecryption) );	// 2007.02.20 laki. charisma title
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... CharacterDataTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmItem)
	{
		sprintf(strFile, "Ini\\%sItemDataTable.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmItem		->StreamReadImportData		(strFile, szBuffer, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemDataTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmAuctionCategory )
		VERIFY(g_pcsAgpmAuctionCategory->LoadCategoryInfo		("Ini\\ItemDataTable.txt", m_bDecryption ? true : false ));
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemDataTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmItem)
	{
		VERIFY( g_pcsAgpmItem->StreamReadTransformData("Ini\\ItemTransformTable.txt", m_bDecryption) );
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemTransformTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmItem)
	{
		sprintf(strFile, "Ini\\%sItemOptionTable.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmItem		->StreamReadOptionData		(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemTransformTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmItem)
	{
		sprintf(strFile, "Ini\\avatarset.ini", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmItem		->StreamReadAvatarSet		(strFile, m_bDecryption));
	}

#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemOptionTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmQuest)
	{
		sprintf(strFile, "Ini\\%sQuestTemplate.ini", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmQuest		->StreamReadTemplate		(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... QuestTemplate.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmQuest)
	{
		sprintf(strFile, "Ini\\%sQuestGroup.ini", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmQuest		->StreamReadGroup		(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... QuestGroup.ini - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmSkill)
	{
		VERIFY( g_pcsAgpmSkill->ReadSkillSpecTxt("Ini\\Skill_Spec.txt", m_bDecryption) );
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... Skill_Spec.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmSkill)
	{
		VERIFY( g_pcsAgpmSkill->ReadSkillConstTxt("Ini\\Skill_Const.txt", m_bDecryption) );
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... Skill_Const.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmSkill)
	{
		VERIFY( g_pcsAgpmSkill->ReadSkillConst2Txt("Ini\\Skill_Const2.txt", m_bDecryption) );
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... Skill_Const2.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmSkill)
	{
		sprintf(strFile, "Ini\\%sSkillTooltip.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmSkill		->ReadSkillTooltipTxt	(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... SkillTooltip.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmEventSkillMaster)
	{
		VERIFY(g_pcsAgpmEventSkillMaster	->StreamReadMasteryTxt		("Ini\\SkillMastery.txt", m_bDecryption));
		VERIFY(g_pcsAgpmEventSkillMaster	->StreamReadHighLevelSkill	("Ini\\SkillMastery_High.txt", m_bDecryption));
		VERIFY(g_pcsAgpmEventSkillMaster	->StreamReadMasteryEvolution("Ini\\SkillMasteryEvolution.txt", m_bDecryption));
		VERIFY(g_pcsAgpmEventSkillMaster	->StreamReadHeroicSkill		("ini\\SkillMastery_Heroic.txt" , m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... SkillMastery.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmItemConvert)
	{
		VERIFY(g_pcsAgpmItemConvert->StreamReadConvertTable("Ini\\ItemConvertTable.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ItemConvertTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		sprintf(strFile, "Ini\\%sItemRuneAttributeTable.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgpmItemConvert->StreamReadRuneAttribute(strFile, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ItemRuneAttributeTable.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		VERIFY(g_pcsAgpmItemConvert->StreamReadConvertPointTable("Ini\\ItemConvertPoint.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ItemConvertPoint.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgpmProduct)
	{
		VERIFY(g_pcsAgpmProduct->StreamReadCategory("Ini\\ProductCategory.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ProductCategory.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		VERIFY(g_pcsAgpmProduct->StreamReadCompose("Ini\\ProductCompose.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ProductCompose.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		VERIFY(g_pcsAgpmProduct->StreamReadFactor("Ini\\ProductExp.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ProductExp.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		VERIFY(g_pcsAgpmProduct->StreamReadGatherCharacterTAD("Ini\\ProductGather.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... ProductGather.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	}

	g_pEngine->OnRender();

	if (g_pcsAgpmRefinery)
	{
//		VERIFY(g_pcsAgpmRefinery->StreamReadRefineTemplate("Ini\\Refinery.txt", m_bDecryption));
//#ifdef _DEBUG
//		GlobalMemoryStatus(&stMemory);
//		TRACE("Loaded ... Refinery.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
//#endif // _DEBUG

		VERIFY(g_pcsAgpmRefinery->StreamReadRefineItem2("Ini\\RefineryItem2.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... RefineryItem.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

//		sprintf(strFile, "Ini\\%sRefineryOptionStone.txt", m_cMyEngine.m_szLanguage);
//		VERIFY(g_pcsAgpmRefinery->StreamReadOptionStone(strFile, m_bDecryption));
//#ifdef _DEBUG
//		GlobalMemoryStatus(&stMemory);
//		TRACE("Loaded ... RefineryOptionStone.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
//#endif // _DEBUG

	}

	//중국 아크로드2.0에서 겜블삭제 supertj@081013
	//if (g_pcsAgpmGamble)
	//	VERIFY(g_pcsAgpmGamble->StreamReadGamble("Ini\\GambleTable.txt", m_bDecryption));

	g_pEngine->OnRender();

	if (g_pcsAgcmUIItem)
	{
		sprintf(strFile, "Ini\\%sItemTooltip.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgcmUIItem->StreamReadTooltipData(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemTooltip.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgcmUIItem)
	{
		sprintf(strFile, "Ini\\%sItemTooltipDesc.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgcmUIItem->StreamReadTooltipDescData(strFile, m_bDecryption));
	}
#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... ItemTooltipDesc.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if (g_pcsAgpmCharacter)
	{
		VERIFY(g_pcsAgpmCharacter->StreamReadCharGrowUpTxt("Ini\\GrowUpFactor.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... GrowUpFactor.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmUICharacter)
	{
		sprintf(strFile, "Ini\\%sLevelUpMessage.txt", m_cMyEngine.m_szLanguage);
		VERIFY(g_pcsAgcmUICharacter->StreamReadLevelupMessages(strFile, m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... LevelUpMessage.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgpmEventCharCustomize)
	{
		VERIFY(g_pcsAgpmEventCharCustomize->StreamReadCustomizeList("Ini\\CharacterCustomizeList.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... CharacterCustomizeList.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmUITips)
	{
		VERIFY(g_pcsAgcmUITips->ReadTipImages("Ini\\Tips.txt", m_bDecryption));
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... Tips.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

		sprintf(strFile, "Ini\\%sTipText.txt", m_cMyEngine.m_szLanguage);
		g_pcsAgcmUITips->ReadTipTexts(strFile, m_bDecryption);
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... TipText.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	if (g_pcsAgcmUIHelp)
	{
		sprintf(strFile, "Ini\\%sHelp.txt", m_cMyEngine.m_szLanguage);
		g_pcsAgcmUIHelp->ReadHelp(strFile, m_bDecryption);
#ifdef _DEBUG
		GlobalMemoryStatus(&stMemory);
		TRACE("Loaded ... TipText.txt - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG
	}

	g_pEngine->OnRender();

	// 중국 로긴창 2D 모드
	//if (g_pcsAgcmUILogin && g_eServiceArea == AP_SERVICE_AREA_CHINA)
	//	g_pcsAgcmUILogin->SetSimpleUI();

	if (g_pcsAgcmCharacter)
		VERIFY(g_pcsAgcmCharacter->StreamReadPolyMorphTable		("ini\\PolyMorphTable.txt", NULL, TRUE));

	g_pEngine->OnRender();

	if( g_pcsApmMap )
	{
		if( g_pcsApmMap->LoadTemplate( "Ini/" REGIONTEMPLATEFILE , TRUE ) )
		{
			// 성공
		}
		else
		{
			// 실패..
			MD_SetErrorMessage( "리젼 템플릿 파일 읽기 실패~" );
		}		
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmLODManager)
		g_pcsAgcmLODManager			->ReadLODDistanceData("INI\\LODDISTANCETYPE.TXT", m_bDecryption);

	g_pEngine->OnRender();

	if (g_pcsAgcmEventEffect)
	{
		g_pcsAgcmEventEffect->ReadMovingSoundData("INI\\EE_MOVE_SOUND.TXT", m_bDecryption);
		//g_pcsAgcmEventEffect->ReadConvertedItemGradeEffectData("INI\\EE_SS_GRADE.TXT", m_bDecryption);
		g_pcsAgcmEventEffect->ReadSpiritStoneHitEffectData("INI\\EE_SS_TARGET.TXT", m_bDecryption);
		g_pcsAgcmEventEffect->ReadSpiritStoneAttrEffectData("INI\\EE_SS_CHANGE.TXT", m_bDecryption);
		g_pcsAgcmEventEffect->ReadCommonCharEffectData("INI\\EE_COMMON_CHAR.TXT", m_bDecryption);
		g_pcsAgcmEventEffect->ReadSocialAnimationData("INI\\SOCIAL_PC.txt", "INI\\SOCIAL_GM.txt", "CHARACTER\\ANIMATION\\", m_bDecryption);
		g_pcsAgcmEventEffect->ReadConvertedArmourAttrEffectData("INI\\EE_SS_FX.TXT", m_bDecryption);
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmUIAuction)
	{
		g_pcsAgcmUIAuction->SetCategoryText();
	}

	g_pEngine->OnRender();

	if (g_pcsAgpmSiegeWar)
	{
		g_pcsAgpmSiegeWar->ReadSiegeInfo("ini\\SiegeWar.txt", TRUE);
		g_pcsAgpmSiegeWar->ReadSiegeWarObjectInfo("ini\\SiegeWarObject.txt", TRUE);
	}

#ifdef _DEBUG
	GlobalMemoryStatus(&stMemory);
	TRACE("Loaded ... DEFAULTSOUND.TXT - Allocation : %d\n", stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif // _DEBUG

	g_pEngine->OnRender();

	if ( g_pcsAgcmUIHotkey )
		g_pcsAgcmUIHotkey->SetIniInfo(UI_HOTKEY_INI_FILE, m_bDecryption);

	g_pEngine->OnRender();

#ifdef _BIN_EXEC_
	//	VERIFY(g_csAgcmPreLODManager.ObjectPreLODStreamRead	("Ini\\ObjectPreLOD.ini"));
	//	m_pstDummyClump = g_csAgcmObject.LoadClump("Dummy.dff");
	//	VERIFY(m_pstDummyClump);
	//	g_csAgcmPreLODManager.SetDummyClump(m_pstDummyClump);
	if (g_pcsAgcmLODManager)
		g_pcsAgcmLODManager->SetForceZeroLevel(TRUE);

	g_pEngine->OnRender();

	if (g_pcsAgcmRender)
		g_pcsAgcmRender->SetUseLODBoundary(FALSE);

	g_pEngine->OnRender();

#endif // _BIN_EXEC_

	if (g_pcsAgpmTimer)
	{
		g_pcsAgpmTimer->SetTime(12, 0);
		g_pcsAgpmEventNature->SetSpeedRate( 6 );
		g_pcsAgpmEventNature->StartTimer();
	}

	g_pEngine->OnRender();

	if (g_pcsAgcmEventNature)
		g_pcsAgcmEventNature->ApplySkySetting();

	g_pEngine->OnRender();

	// 마고자 (2005-05-31 오후 3:17:09) : 
	// 여기서 로딩화면으로 바뀜.
	if (g_pcsAgcmUILogin)
		g_pcsAgcmUILogin->StartLoginProcess();

	g_pEngine->OnRender();

	if (g_pcsAgcmChatting2)
	{
		g_pcsAgcmChatting2->OnLoadFilterFile( "INI\\Base.dat", m_bDecryption );
		g_pcsAgcmChatting2->OnLoadFilterFile( "INI\\CustomTextFilter.txt", FALSE );
	}

	if (g_pcsAgpmCharacter)
		g_pcsAgpmCharacter->StreamReadChatFilterText("INI\\Base.dat", m_bDecryption);

	g_pEngine->OnRender();	
	
	if ( g_pcsAgcmEventEffect )
		g_pcsAgcmEventEffect->StreamReadExclusiveEffectMappingTable("ini\\ExcEffTable.txt", TRUE);

	g_pEngine->OnRender();

	::ShowCursor( TRUE );
	::SetCursor( m_cMyEngine.m_hCursor );

#ifdef _AREA_JAPAN_
	if ( g_jAuth.GetAutoLogin() )
	{
		JapanAutoLogin( g_pcsAgcmLogin, g_pcsAgcmUILogin );
		g_jGameInfo.StartGame( g_jAuth.GetMemberID() );
	}
#endif

#ifdef _AREA_KOREA_
	KoreaAutoLogin( g_pcsAgcmLogin, g_pcsAgcmUILogin );
#endif

	g_cMyEngine.LoadTelportPoint();

	if( !g_pcsAgcmUICharacter->OnInitializeProperty() )
	{
#ifdef _DEBUG
		OutputDebugString( "[ AgcmUICharacter ] Failed to load Character Information Dialog Settings.\n" );
		return FALSE;
#endif
	}

	return TRUE;
}

void MainWindow::UpdateLoadState()
{
	switch( m_eTemplatesLoaded )
	{
	case 0:
		{
			static INT32 nStapOneTick = GetTickCount();
			if( GetTickCount() - nStapOneTick < 3000 )
				return;
			else
			{
				m_eTemplatesLoaded = 2;
				return;
				//PlayDShow( s_szDShowName );
				//break;
				return;
			}
		}

	case 1:
		if( IsCompleteDShow( s_szDShowName ) )
		{
			StopDShow( s_szDShowName );
			break;
		}
		else
			return;

	case 2:
		if( !bLoadTemplates )
		{
			LoadTemplates();
			break;
		}
		else
			return;

	default:
		return;
	}

	++m_eTemplatesLoaded;
}

int		MainWindow::GetWindowSizeIndex()
{
	if (g_pcsAgcmUIOption->m_iScreenHeight >= 1200 && g_pcsAgcmUIOption->m_iScreenWidth >= 1600 )
		return 2;
	else if (g_pcsAgcmUIOption->m_iScreenHeight >= 1024 && g_pcsAgcmUIOption->m_iScreenHeight < 1200)
		return 1;

	return 0;
}

void MainWindow::PlayDShow( char* szName )
{
	if( !szName || !szName[0] )		return;

	//LPDShowNode	pNode = m_cDShowMng.GetNode( szName );
	//if( !pNode )
	//	m_cDShowMng.Insert( szName, g_pcsAgcmUIOption->m_iScreenWidth, g_pcsAgcmUIOption->m_iScreenHeight );
}

void MainWindow::StopDShow( char* szName )
{
	if( !szName || !szName[0] )		return;

	//m_cDShowMng.Delete( szName );
}

BOOL MainWindow::IsCompleteDShow( char* szName )
{
	//LPDShowNode	pNode = m_cDShowMng.GetNode( szName );
	//return pNode && EC_COMPLETE == pNode->GetEvent() ? TRUE : FALSE;

	return TRUE;
}

// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
BOOL MainWindow::OnInit()
{
	RwFrame		*pFrame = RwCameraGetFrame	( g_pEngine->m_pCamera	);
	RwV3d		pos = { 0.0f , 1.0f , 0.0f };
	RwFrameTranslate( pFrame , &pos , rwCOMBINEPOSTCONCAT );

	if( g_pcsAgcmChatting2 )
	{
		// 마고자 (2004-11-03 오후 6:20:34) : 커맨드 프로시져 등록..
		VERIFY( g_pcsAgcmChatting2->SetCallbackCommandProcess( __CBCustomCommandCallBack , this ) );
	}


	AS_REGISTER_TYPE_BEGIN(MainWindow, Debug);
		AS_REGISTER_VARIABLE(int, m_lShowHeightGrid				);
		AS_REGISTER_VARIABLE(int, m_lShowGeometryBlocking		);
		AS_REGISTER_VARIABLE(int, m_lShowSkyBlocking			);
		AS_REGISTER_VARIABLE(int, m_lShowRidableObjectAvailable	);
		AS_REGISTER_VARIABLE(int, m_lShowObjectBlocking			);
		AS_REGISTER_VARIABLE(int, m_lShowCharacterBlocking		);
		AS_REGISTER_VARIABLE(int, m_lShowLineBlocking			);
		AS_REGISTER_METHOD0(void, ToggleRidableInfo				);
		AS_REGISTER_METHOD0(void, EnableTheaterMode				);
		AS_REGISTER_METHOD0(void, ReportApMemory				);
		AS_REGISTER_METHOD0(void, IntensiveCrash				);
		AS_REGISTER_METHOD0(void, ToggleMainThreadSleep			);
		AS_REGISTER_METHOD0(void, ToggleBackThreadSleep			);
		AS_REGISTER_METHOD0(void, ToggleCameraAngle				);
	AS_REGISTER_TYPE_END;

	if( m_cMyEngine.m_bEmulationFullscreen )
		SetFullWindow();

	g_pcsApmObject->SetCallbackLoadError( CBObjectLoadError , this );

	if(g_pcsAgpmAdmin)
		g_pcsAgpmAdmin->SetCallbackAdminClientLogin(CBAdminClientLogin, this);

	//Loading Txture Loading
	m_texLoadingSnda[0]	= RwTextureRead( "snda_logo_1024", NULL );
	m_texLoadingSnda[1]	= RwTextureRead( "snda_logo_1280", NULL );
	m_texLoadingSnda[2]	= RwTextureRead( "snda_logo_1600", NULL );
	m_texLoadingNHN[0] = RwTextureRead("LG_Title_1024", NULL);
	m_texLoadingNHN[1] = RwTextureRead("LG_Title_1280", NULL);
	m_texLoadingNHN[2] = RwTextureRead("LG_Title_1600", NULL);

	m_LogoGameGrade.OnCreate();

	for( int i=0; i<3; ++i )
	{
		RwTextureSetAddressing( m_texLoadingSnda[i], rwTEXTUREADDRESSCLAMP );
		RwTextureSetAddressing( m_texLoadingNHN[i], rwTEXTUREADDRESSCLAMP );
	}

	m_pLineList	= new RwIm3DVertex[ 65535 ];
	m_pIndex = new RwImVertexIndex[ 65535 ];

	//m_cDShowMng.CreateDevice( (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice() );
	//PlayDShow( s_szDShowName );		//이미봤다면 그냥 Play 하지 말고 넘어가자 ^^

	return TRUE;
}

struct stDisplayMode
{
	stDisplayMode( int w , int h , int d ) : nWidth(w),nHeight(h),nDepth(d)	{		}

	bool	operator == ( const stDisplayMode& rhs )	{
		return nWidth == rhs.nWidth && nHeight == rhs.nHeight && nDepth == rhs.nDepth ? TRUE : FALSE;
	}

	int	nWidth;
	int nHeight;
	int nDepth;
};
typedef vector< stDisplayMode >::iterator	DisplayModeItr;

void MainWindow::ToggleFullScreenTest()
{
	if( IsFullScreen() )
	{
		// 돌려놓기
		ReturnScreenMode();
	}
	else
	{
		stDisplayMode	cCurDisplay( g_pcsAgcmUIOption->m_iScreenWidth, g_pcsAgcmUIOption->m_iScreenHeight, g_pcsAgcmUIOption->m_iScreenDepth );

		vector< stDisplayMode >	vecDisplay;
		vecDisplay.push_back( stDisplayMode( 1600, 1200, 32 ) );
		vecDisplay.push_back( stDisplayMode( 1600, 1200, 16 ) );
		vecDisplay.push_back( stDisplayMode( 1280, 1024, 32 ) );
		vecDisplay.push_back( stDisplayMode( 1280, 1024, 16 ) );
		vecDisplay.push_back( stDisplayMode( 1024, 768, 32 ) );
		vecDisplay.push_back( stDisplayMode( 1024, 768, 16 ) );
		
		DisplayModeItr Itr = find( vecDisplay.begin(), vecDisplay.end(), cCurDisplay );
		if( Itr == vecDisplay.end() )
		{
			::MessageBox( NULL , "Cannot determine valid screen mode." , "Archlord" , MB_ICONERROR | MB_OK );
			::PostQuitMessage( 1 );
		}

		if( !SetFullScreen( cCurDisplay.nWidth, cCurDisplay.nHeight, cCurDisplay.nDepth ) )
		{
			char strMessageDebug[ 1024 ] = "";
			char strTemp[ 256 ];
			sprintf( strTemp , "Change Display Mode Fail : (%d*%d*%d)\r\n" , (*Itr).nWidth , (*Itr).nHeight , (*Itr).nDepth );
			MD_SetErrorMessage( strTemp );
			strcat( strMessageDebug , strTemp );
			strcat( strMessageDebug, 	"\r\n\r\nCannot change screen mode. This client cannot change proper resolution for playing Archlord.\r\n"\
										"Crash this client and report this information to Archlord?" );
			
			if( IDYES == ::MessageBox( NULL , strMessageDebug , "Archlord" , MB_ICONERROR | MB_YESNOCANCEL ) )
				IntensiveCrash();

			::PostQuitMessage( 1 );
		}
	}

	//	char strMessageDebug[ 1024 ] = "";
	//	
	//	for( ; iTer != vecDisplay.end() ; iTer ++ )
	//	{
	//		if(	SetFullScreen( (*iTer).nWidth, (*iTer).nHeight, (*iTer).nDepth ) )
	//			break;

	//		char strTemp[ 256 ];
	//		sprintf( strTemp , "Change Display Mode Fail : (%d*%d*%d)\r\n" , (*iTer).nWidth , (*iTer).nHeight , (*iTer).nDepth );
	//		MD_SetErrorMessage( strTemp );
	//		strcat( strMessageDebug , strTemp );
	//	}

	//	if( iTer == vecDisplay.end() )
	//	{
	//		strcat( strMessageDebug 	"\r\n\r\nCannot change screen mode. This client cannot change proper resolution for playing Archlord.\r\n"
	//									"Crash this client and report this information to Archlord?" );
	//		
	//		if( IDYES == ::MessageBox( NULL , strMessageDebug , "Archlord" , MB_ICONERROR | MB_YESNOCANCEL ) )
	//			IntensiveCrash();

	//		::PostQuitMessage( 1 );
	//	}
	//}
}

void MainWindow::EnableTheaterMode()
{
	m_bEnableTheaterMode = !m_bEnableTheaterMode;

	if( m_bEnableTheaterMode )
	{
		print_ui_console( "촬영 모드를 가동합니다 ']'버튼으로 온오프하세요" );
		print_ui_console( "Shift + ']' 할경우 케릭터 표시함!" );
	}
	else
	{
		print_ui_console( "촬영 모드 사용하지 않습니다" );
	}
}

void	MainWindow::ToggleMainThreadSleep()	
{
	if( g_pEngine->GetDebugFlag() & AgcEngine::SLEEP_MAIN_THREAD )
	{
		// 플래그 끄기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~( AgcEngine::SLEEP_MAIN_THREAD | AgcEngine::SLEEP_BACK_THREAD ) );
		print_ui_console( "슬립 모드를 사용하지 않습니다." );
	}
	else
	{
		// 플래그 켜기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~( AgcEngine::SLEEP_MAIN_THREAD | AgcEngine::SLEEP_BACK_THREAD ) );
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() | AgcEngine::SLEEP_MAIN_THREAD );
		print_ui_console( "F5 버튼으로 메인스레드가 동작하지 않게됩니다." );
	}
}
void	MainWindow::ToggleBackThreadSleep()
{
	if( g_pEngine->GetDebugFlag() & AgcEngine::SLEEP_BACK_THREAD )
	{
		// 플래그 끄기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~( AgcEngine::SLEEP_MAIN_THREAD | AgcEngine::SLEEP_BACK_THREAD ) );
		print_ui_console( "슬립 모드를 사용하지 않습니다." );
	}
	else
	{
		// 플래그 켜기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~( AgcEngine::SLEEP_MAIN_THREAD | AgcEngine::SLEEP_BACK_THREAD ) );
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() | AgcEngine::SLEEP_BACK_THREAD );
		print_ui_console( "F5 버튼으로 백스레드가 동작하지 않게됩니다." );
	}
}

void		MainWindow::AutoHunt()
{
	if( g_pEngine->GetDebugFlag() & AgcEngine::AUTO_HUNTING )
	{
		// 플래그 끄기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~AgcEngine::AUTO_HUNTING );
		print_ui_console( "자동 사냥을 종료합니다." );
	}
	else
	{
		// 플래그 켜기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() | AgcEngine::AUTO_HUNTING );
		print_ui_console( "자동사냥을 시작합니다." );
	}
}

void	MainWindow::ToggleCameraAngle()
{
	if( g_pEngine->GetDebugFlag() & AgcEngine::CAMERA_ANGLE_CHANGE )
	{
		// 플래그 끄기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~AgcEngine::CAMERA_ANGLE_CHANGE );
		print_ui_console( "앵글 변화기능을 종료합니다." );
	}
	else
	{
		// 플래그 켜기..
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() | AgcEngine::CAMERA_ANGLE_CHANGE );
		print_ui_console( "Control + Wheel로 카메라 앵글을 변화합니다." );
	}
}

//extern AuNPWrapper npgl;
BOOL MainWindow::OnIdle			( UINT32 ulClockCount )
{
	PROFILE("MainWindow::OnIdle");

	UpdateLoadState();
	AutoHuntIdle();

	// nprotect 체크..
#ifdef _AREA_KOREA_
	{
		static	UINT32	_suLastCheckTime = ulClockCount;
		if( ulClockCount - _suLastCheckTime > 10000)
		{
			_suLastCheckTime = ulClockCount;

			// 10분에 한번씩 GameMon 실행여부 확인..
			if (!npgl.Check())
			{
				// 게임 강제 종료.
				::ExitProcess( 1 );
			}
		}
	}
#endif

#ifdef _AREA_GLOBAL_
	//{
	//	static	UINT32	_suLastCheckTime = ulClockCount;
	//	if( ulClockCount - _suLastCheckTime > 10000)
	//	{
	//		_suLastCheckTime = ulClockCount;

	//		// 10분에 한번씩 GameMon 실행여부 확인..
	//		//if (!npgl.Check())
	//		{
	//			// 게임 강제 종료.
	//			::ExitProcess( 1 );
	//		}
	//	}
	//}
#endif

	if( g_eMiniDumpType == MiniDumpWithFullMemory && g_pEngine->GetDebugFlag() & AgcEngine::OBJECT_LOADING_ERROR )
	{
		g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() & ~AgcEngine::OBJECT_LOADING_ERROR );		// 메시지 박스를 띄운다..
		g_pcsAgcmUISystemMessage->AddSystemMessage( "Object Loading Failed!" );
		g_pEngine->Lua_Flush_Queue();
	}


	// 풀 메모리 덤프에서만 메모리체크함.
	if( g_eMiniDumpType == MiniDumpWithFullMemory )
		VideoMemoryCheck();

	return TRUE;	
}

void MainWindow::OnClose()	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
{
	ShowCursor(TRUE);

	//m_cDShowMng.DestoryDevice();

	for( int i = 0; i < 3; ++i )
	{
		RWTEXTURE_REALEASE( m_texLoadingSnda[i] );
		RWTEXTURE_REALEASE( m_texLoadingNHN[i] );
	}

	DEF_SAFEDELETEARRAY( m_pLineList );
	DEF_SAFEDELETEARRAY( m_pIndex );
}

RpGeometry *	CBReinstanceGeometry(RpGeometry *pstGeometry, PVOID pvData)
{
	if ( !pstGeometry || !RpSkinGeometryGetSkin(pstGeometry) )
		return pstGeometry;

	RpGeometryLock(pstGeometry, rpGEOMETRYLOCKALL);
	RpGeometryUnlock(pstGeometry);

	return pstGeometry;
}

RpAtomic *		CBReinstanceAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	if( !pstAtomic )		return pstAtomic;

	CBReinstanceGeometry(RpAtomicGetGeometry(pstAtomic), pvData);
	RpLODAtomicForAllLODGeometries(pstAtomic, CBReinstanceGeometry, pvData);

	return pstAtomic;
}

BOOL MainWindow::OnKeyDown( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
	case rsESC:
		if( m_eTemplatesLoaded == 1 )
		{
			StopDShow( s_szDShowName );
			++m_eTemplatesLoaded;
		}
		break;
	case rsLALT:
	case rsRALT:
		{
			if( g_pcsAgcmUIOption )
			{
				g_pcsAgcmUIOption->m_bVisibleViewItem ^= TRUE;
				g_pcsAgcmUIOption->UpdateViewItem( TRUE );
			}
		}
		break;

	case rsF4:
		if (g_pEngine->IsLAltDown())
			RsEventHandler(rsQUITAPP, NULL);
		break;
	case rsF6:
		g_pcsAgcmUIGuild->GetUIGuildBattleResult().OpenResult();
		break;


	case '-':
#ifdef _DEBUG
		if (g_pEngine->IsLAltDown() && g_pEngine->IsLShiftDown() && g_pEngine->IsLCtrlDown() && g_pcsAgcmUIDebugInfo)
			g_pcsAgcmUIDebugInfo->EnableDebugInfo(TRUE);
#endif
		break;

	case '/':
		if( g_pcsAgcmUIConsole && g_pEngine->IsLAltDown() && g_pEngine->IsLShiftDown() && g_pEngine->IsLCtrlDown() )
			g_pcsAgcmUIConsole->On();
		break;

	case rsF7:
		{
			// 서버로 어드민client 띄워도 될지 패킷 보낸다.
			g_pcsAgcmAdmin->SendAdminClientRequest();
			
		}
		break;

	case rsINS:
		{
			g_pcsAgcmTextBoardMng->SetDrawHPMP( !g_pcsAgcmTextBoardMng->GetDrawHPMP() );

		}
		
		break;
	case '[':
		if( m_bEnableTheaterMode )
		{
			AgcmEventNature::__SetTime(g_pcsAgcmEventNature, g_pcsAgpmTimer->GetCurHour() + 1, g_pcsAgpmTimer->GetCurMinute());
		}
		break;

	case ']':
		if( m_bEnableTheaterMode )
		{
			AgpdCharacter* pcsCharacter = g_pcsAgpmCharacter->GetCharacter(g_pcsAgcmCharacter->m_lSelfCID);
			if( !pcsCharacter )		break;

			AgcdCharacter* pstCharacter = g_pcsAgcmCharacter->GetCharacterData(pcsCharacter);
			if( !pstCharacter )		break;

			g_bRemove = !g_bRemove;

			if( g_bRemove )
			{
				if( !g_pEngine->IsLShiftDown() )
				{
					g_pcsAgcmRender->RemoveClumpFromWorld(pstCharacter->m_pClump);
				}

				g_pEngine->SetRenderMode( TRUE, FALSE );
				if( g_pcsAgcmTargeting )
					g_pcsAgcmTargeting->m_bShowGlyph = FALSE;
			}
			else
			{
				if (!g_pEngine->IsLShiftDown())
				{
					g_pcsAgcmEventEffect->CharaterEquipEffectRemove( pcsCharacter );
					g_pcsAgcmRender->RemoveClumpFromWorld(pstCharacter->m_pClump);
					g_pcsAgcmRender->AddClumpToWorld(pstCharacter->m_pClump);
					g_pcsAgcmRender->AddUpdateInfotoClump(pstCharacter->m_pClump, g_pcsAgcmCharacter, AgcmCharacter::CBUpdateCharacter,NULL,pcsCharacter,NULL);
					if (g_pcsAgcmShadow)
						g_pcsAgcmShadow->AddShadow(pstCharacter->m_pClump);

					g_pcsAgcmEventEffect->CharaterEquipEffectUpdate( pcsCharacter );
				}

				g_pEngine->SetRenderMode(TRUE, TRUE);
				if (g_pcsAgcmTargeting)
					g_pcsAgcmTargeting->m_bShowGlyph = TRUE;
			}
		}
		break;

#ifdef _DEBUG
	case rsF3:
		g_pcsAgcmSound->StopBGM( 0 );
		break;

	case rsF8:
		break;

	case ',':	AGCMMAP_THIS->__SetAutoLoadRange(
					AGCMMAP_THIS->GetMapDataLoadRadius	() - 1,
					AGCMMAP_THIS->GetRoughLoadRadius		() - 1,
					AGCMMAP_THIS->GetDetailLoadRadius		() - 1);
				break;
	case '.':	AGCMMAP_THIS->__SetAutoLoadRange(
					AGCMMAP_THIS->GetMapDataLoadRadius	() + 1,
					AGCMMAP_THIS->GetRoughLoadRadius		() + 1,
					AGCMMAP_THIS->GetDetailLoadRadius		() + 1);
				break;

	case '\'':
		{
			g_pcsAgcmPostFX->setPipeline("DownSample-BlurH-BlurV-BloomComp");
			g_pcsAgcmPostFX->On();
		}
		break;
	case 'p':
		{
			g_pcsAgcmPostFX->setPipeline("DownSample-BoxBlur1-BoxBloomComp");
			g_pcsAgcmPostFX->On();
		}
		break;
	case ';':
		{
			g_pcsAgcmPostFX->Off();
		}
		break;
	case 'l':
		{
			if(g_pcsAgcmPostFX->isAutoExposureControlOn())
				g_pcsAgcmPostFX->AutoExposureControlOff();
			else
				g_pcsAgcmPostFX->AutoExposureControlOn();
		}
		break;
	case 'z':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "ClampingCircle" );
		break;
	case 'x':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "Wave" );
		break;
	case 'c':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "Shockwave" );
		break;
	case 'v':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "Ripple" );
	case 'b':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "Darken" );
		break;
	case 'n':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "Brighten" );
		break;
	case 'a':
		if(g_pEngine->IsLCtrlDown())
			PlayEffectFX( "Twist" );
		break;
	case 's':
		if( g_pEngine->IsLCtrlDown() )
			PlayEffectFX( "TransSaturate" );
		break;
	case 'd':
		if(g_pEngine->IsLCtrlDown())
		{	
			PlayEffectFX( "TransMeshuggah" );

			// Randomize the meshuggah texture.
			const char* meshuggahTextures[] = { "meshuggah0Tex", "meshuggah1Tex", "meshuggah2Tex", "meshuggah3Tex" };
			D3DXHANDLE hDst = g_pcsAgcmPostFX->m_pd3dEffect->GetParameterByName(NULL, "meshuggahTex");
			D3DXHANDLE hSrc = g_pcsAgcmPostFX->m_pd3dEffect->GetParameterByName(NULL, meshuggahTextures[ rand() & 3 ]);
			LPDIRECT3DBASETEXTURE9 srcTex;
			g_pcsAgcmPostFX->m_pd3dEffect->GetTexture(hSrc, &srcTex);
			g_pcsAgcmPostFX->m_pd3dEffect->SetTexture(hDst, srcTex);
		}
		break;

	case 35:
		if( g_pEngine->IsLCtrlDown() )
		{
			INT32 lIndex = 0;
			for( AgpdCharacterTemplate* pcsTemplate = g_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = g_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
			{
				AgcdCharacterTemplate* pstTemplate = g_pcsAgcmCharacter->GetTemplateData( pcsTemplate );
				if( pstTemplate->m_pClump )
					RpClumpForAllAtomics( pstTemplate->m_pClump, CBReinstanceAtomic, NULL );
			}
		}
		break;
#endif // _DEBUG

	default:
		break;
	}

	return TRUE;
}

BOOL MainWindow::OnKeyUp( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
	case 255:
		{
			if (ks->keyScanCode == 183)
				g_pEngine->m_bSaveScreen = TRUE;
		}
		break;
#ifdef _DEBUG
	case rsPADENTER:
		{
			ApMemoryManager::GetInstance().ReportMemoryInfo();
		}
		break;
#endif // _DEBUG

	default:
		break;
	}

	return TRUE;
}

void MainWindow::OnPreRender( RwRaster* raster )	// 월드 렌더하기전.
{
	Update();
}

void MainWindow::OnRender( RwRaster *raster	)		// 렌더는 여기서;
{
	PROFILE("MainWindow::OnRender");

	//DShow를 그려준다 ^^

	if (!g_bRemove)
		AgcmUIManager2::CB_POST_RENDER(NULL, g_pcsAgcmUIManager2, NULL);

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER , (void*)rwFILTERNEAREST );

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

#ifdef _DEBUG 
	g_pcsAgcmFont->FontDrawStart(0);

	char	str[ 256 ];
		
	wsprintf( str , "FPS: %03d", m_cMyEngine.GetFps() );
	g_pcsAgcmFont->DrawTextIM2D(950, 10, str, 0 ,230);

	/*
	if (g_pcsAgpmTimer)
	{
		wsprintf( str , "Time: %04d/%02d/%02d %02d:%02d:%02d",
						g_pcsAgpmTimer->GetCurYear(),
						g_pcsAgpmTimer->GetCurMonth(),
						g_pcsAgpmTimer->GetCurDay(),
						g_pcsAgpmTimer->GetCurHour(),
						g_pcsAgpmTimer->GetCurMinute(),
						g_pcsAgpmTimer->GetCurSecond());

		g_pcsAgcmFont->DrawTextIM2D(850, 30, str, 0 ,230);
	}
	*/
	wsprintf( str , "Current Effects: %d" , g_pcsAgcmEff2->GetCtrlMng()->GetCurrCnt() );
	g_pcsAgcmFont->DrawTextIM2D(850, 30, str, 0 ,230);

	if(g_pcsAgcmCharacter->GetSelfCharacter())
	{
		sprintf(str,"POS: %.0f , %.0f , %.0f", g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.x,
		g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.y,
		g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.z);
		g_pcsAgcmFont->DrawTextIM2D(720, 50, str, 0 );
	}

	AgpdCharacter* ppdSelf = g_pcsAgcmCharacter->GetSelfCharacter();
	AgpdCharacter* ppdTarget = g_pcsAgcmUICharacter->GetTargetCharacter();
	if( ppdSelf && ppdTarget )
	{
		AgcdCharacter* pcdSelf = g_pcsAgcmCharacter->GetCharacterData( ppdSelf );
		AgcdCharacter* pcdTarget = g_pcsAgcmCharacter->GetCharacterData( ppdTarget );
		if( pcdSelf && pcdSelf->m_pClump && pcdTarget && pcdTarget->m_pClump )
		{
			RwFrame* pFrameSelf = RpClumpGetFrame( pcdSelf->m_pClump );
			RwFrame* pFrameTarget = RpClumpGetFrame( pcdTarget->m_pClump );
			if( pFrameSelf && pFrameTarget )
			{
				RwMatrix* pTmSelf = RwFrameGetMatrix( pFrameSelf );
				RwMatrix* pTmTarget = RwFrameGetMatrix( pFrameTarget );
				if( pTmSelf && pTmTarget )
				{
					RwV3d* pPosSelf = RwMatrixGetPos( pTmSelf );
					RwV3d* pPosTarget = RwMatrixGetPos( pTmTarget );
					if( pPosSelf && pPosTarget )
					{
						RwV3d vDistance = { 0.0f, 0.0f, 0.0f };
						RwV3dSub( &vDistance, pPosTarget, pPosSelf );

						float fDistance = RwV3dLength( &vDistance ) / 100.0f;

						char strDebug[ 64 ] = { 0, };
						sprintf_s( strDebug, sizeof( char ) * 64, "Distance to Target = %3.3fm", fDistance );
						g_pcsAgcmFont->DrawTextIM2D(720, 70, strDebug, 0 );
					}
				}
			}
		}
	}

	g_pcsAgcmFont->FontDrawEnd();
	
	
	//@{ kday 20050306
	AXISVIEW::RenderAxisContainer();
	//@} kday

	//@{ 2006/03/07 burumal
	// for Multi-Picking Atomic Debugging
	/*
	AgpdCharacter* pPlayer = g_pcsAgcmCharacter->GetSelfCharacter();	
	if ( pPlayer != NULL )
	{
		//INT32 lTargetID = g_pcsAgcmCharacter->GetCharacterData(pPlayer)->m_lSelectTargetID;
		INT32 lTargetID = g_pcsAgcmCharacter->GetCharacterData(pPlayer)->m_lLockTargetID;
		if ( lTargetID != AP_INVALID_CID )
		{
			AgpdCharacter* pTarget = g_pcsAgpmCharacter->GetCharacter(lTargetID);
			if ( pTarget )
			{
				AgcdCharacter* pTargetData = g_pcsAgcmCharacter->GetCharacterData(pTarget);

				RpClump* pPlayerClump;
				RwV3d vPlayerCenter, vTargetCenter;

				if ( pTargetData )
				{
					pPlayerClump = g_pcsAgcmCharacter->GetCharacterData(pPlayer)->m_pClump;
					
					if ( pPlayerClump )
					{
						vPlayerCenter = pPlayerClump->stType.boundingSphere.center;
						RwV3dTransformPoint(&vPlayerCenter, &vPlayerCenter, RwFrameGetLTM(RpClumpGetFrame(pPlayerClump)));
						
						ASSERT(pTargetData->m_pPickAtomic);
						const RwSphere* pBSphere = RpAtomicGetWorldBoundingSphere(pTargetData->m_pPickAtomic);

						if ( pBSphere )
						{
							vTargetCenter = pBSphere->center;
							//RwV3dTransformPoint(&vTargetCenter, &vTargetCenter, RwFrameGetLTM(RpAtoGetFrame(pTargetData->m_pPickAtomic)));
							
							AcuIMDraw::RenderLine(vPlayerCenter, vTargetCenter, 0xffff0000);

							g_pcsAgcmFont->FontDrawStart(0);

							const PickedObject* pCur = g_pcsAgcmTargeting->GetPickedObject();
							for ( int nIdx = 0; nIdx < g_pcsAgcmTargeting->GetPickedNum(); nIdx++ )
							{
								if ( !pCur )
									break;

								wsprintf(str, "PickAtomic[%d]: %x [%x]", nIdx, pCur->pNewFirstPickAtomic, pTargetData->m_pOrgPickAtomic);
								g_pcsAgcmFont->DrawTextIM2D(50, 60+13*nIdx, str, 0 ,230);
								
								pCur = pCur->next;
							}
							
							g_pcsAgcmFont->FontDrawEnd();
						}
					}
				}
			}
		}
	}
	*/
	//@}

#else	// _DEBUG
	//AXISVIEW::RenderAxisContainer();

	if(g_pEngine->m_bDrawFPS)
	{
		static		int			Total_Grass_Count = 0;
		static		int			Grass_Update_Count = 0;

		g_pcsAgcmFont->FontDrawStart(0);

		char	str[ 256 ];
		
		wsprintf( str , "FPS: %03d", m_cMyEngine.GetFps() );
		g_pcsAgcmFont->DrawTextIM2D(950, 10, str, 0 ,230);

		if (g_pcsAgpmTimer)
		{
			wsprintf( str , "Time: %04d/%02d/%02d %02d:%02d:%02d",
							g_pcsAgpmTimer->GetCurYear(),
							g_pcsAgpmTimer->GetCurMonth(),
							g_pcsAgpmTimer->GetCurDay(),
							g_pcsAgpmTimer->GetCurHour(),
							g_pcsAgpmTimer->GetCurMinute(),
							g_pcsAgpmTimer->GetCurSecond());

			g_pcsAgcmFont->DrawTextIM2D(850, 30, str, 0 ,230);
		}

		if(g_pcsAgcmCharacter->GetSelfCharacter())
		{
			sprintf(str,"POS: %.3f , %.3f , %.3f", g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.x,
			g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.y,
			g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.z);
			g_pcsAgcmFont->DrawTextIM2D(720, 50, str, 0 );
		}

		if(g_pcsAgcmGrass)
		{
			wsprintf( str , "Grass Draw: %d", g_pcsAgcmGrass->m_iDrawGrassCount + g_pcsAgcmGrass->m_iDrawGrassCountNear);
			g_pcsAgcmFont->DrawTextIM2D(900, 70, str, 0 ,230);
		}

		if(g_pcsAgcmEff2)
		{
			wsprintf( str , "Effect Count: %d", g_pcsAgcmEff2->GetCtrlMng()->GetCurrCnt());
			g_pcsAgcmFont->DrawTextIM2D(870, 90, str, 0 ,230);
		}

		g_pcsAgcmFont->FontDrawEnd();
	}

#endif //_DEBUG

	// 라이더블 정보..
	if( m_bShowRidableInfo )
	{
		if( g_pcsAgcmCharacter->GetSelfCharacter() )
		{
			AuPOS pos = g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos;

			ApWorldSector* pWorldSector = g_pcsApmMap->GetSector( pos.x , pos.z );

			INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
			INT32	nSegmentX , nSegmentZ;
			pWorldSector->D_GetSegment( SECTOR_HIGHDETAIL ,pos.x , pos.z , & nSegmentX , & nSegmentZ );

			INT32	nObjectCount = pWorldSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , nSegmentX , nSegmentZ , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );

			g_pcsAgcmFont->FontDrawStart(0);

			char	str[ 256 ];
			sprintf( str, "(%d,%d) 섹터 (%d,%d) 내부좌표 : Ridable %d", pWorldSector->GetArrayIndexX() , pWorldSector->GetArrayIndexZ(), nSegmentX , nSegmentZ , nObjectCount );

			int nLine = 0;

			g_pcsAgcmFont->DrawTextIM2D( 0.0f , ( float ) ( 300 + 20 * nLine++ ), str, 0 ,230 );

			FLOAT fHeight = AGCMMAP_THIS->GetHeight( pos.x , pos.z , SECTOR_MAX_HEIGHT );
			sprintf( str, "GetHeight(MAX_HEIGHT) = %.0f (%d , %s)",  fHeight, g_nRidableCount, ( g_bRidableChecked ? "TRUE" : "FALSE" ) );
			g_pcsAgcmFont->DrawTextIM2D( 0.0f , ( float ) ( 300 + 20 * nLine++ ) , str, 0 ,230 );

			FLOAT fHeightFromY = AGCMMAP_THIS->GetHeight( pos.x , pos.z , pos.y );
			sprintf( str , "GetHeight(%.0f) = %.0f (%d , %s)", pos.y, fHeightFromY, g_nRidableCount, ( g_bRidableChecked ? "TRUE" : "FALSE" ) );
			g_pcsAgcmFont->DrawTextIM2D( 0.0f , ( float ) ( 300 + 20 * nLine++ ) , str, 0 ,230 );

			for( int i = 0 ; i < nObjectCount ; ++ i )
			{
				ApdObject* pstApdObject = g_pcsApmObject->GetObject( aObjectList[ i ]	);
				if( !pstApdObject )		continue;

				wsprintf( str , "\"%s\'" , pstApdObject->m_pcsTemplate->m_szName );
				g_pcsAgcmFont->DrawTextIM2D( 0.0f , ( float ) ( 300 + 20 * nLine++ ) , str, 0 ,230);
			}

			g_pcsAgcmFont->FontDrawEnd();
		}
	}

	//g_pcsAgcmFont->FontDrawStart(0);
	//g_pcsAgcmFont->DrawTextIM2D( 0, 0, " ", 0 );
	//g_pcsAgcmFont->FontDrawEnd();
	//m_cDShowMng.FrameRender( 0.f );
}

void	Render( RwTexture* pTexture )
{
// 	if( pTexture )
// 	{
// 		float fWidth = (float)RwRasterGetWidth(RwTextureGetRaster(pTexture));
// 		float fHeight = (float)RwRasterGetHeight(RwTextureGetRaster(pTexture));
// 		g_pEngine->DrawIm2D( pTexture, 0.0f, 0.0f, fWidth, fHeight );
// 	}
	RwRaster * camRaster = RwCameraGetRaster( g_pEngine->m_pCamera );

	if( pTexture && camRaster )
	{
		float scale = camRaster->height / AgcmUIManager2::GetModeHeight();
		float x = (camRaster->width * 0.5f) - (AgcmUIManager2::GetModeWidth()*0.5f*scale);

		g_pEngine->DrawIm2D( pTexture, 0, 0, (float)camRaster->width, (float)camRaster->height, 0, 0, 0, 0, 0xffffffff, 255 );

		FLOAT fWidth = ( FLOAT )( RwRasterGetWidth( pTexture->raster ) ) * scale;
		FLOAT fHeight = ( FLOAT )( RwRasterGetHeight( pTexture->raster ) ) * scale;

		g_pEngine->DrawIm2D( pTexture, x, 0, fWidth, fHeight );
	}
}

void MainWindow::OnWindowRender()
{
	if (m_texLoadingNHN)
	{
#ifdef _AREA_KOREA_
		if( !m_LogoGameGrade.OnRender() )
#endif
		{
			switch( m_eTemplatesLoaded )
			{
			case 0:
				Render( m_texLoadingSnda[GetWindowSizeIndex()] );
				break;
			case 1:
				break;
			case 2:
				Render( m_texLoadingNHN[GetWindowSizeIndex()] );
				break;
			}
		}
		
		if( 0 == m_eTemplatesLoaded || 2 == m_eTemplatesLoaded )
		{
			switch( g_eMiniDumpType )
			{
			case MiniDumpWithFullMemory:
				{
					char str[ 256 ];
					sprintf( str, "%s'", "Full Memory Dump for Internal Test" );
					g_pcsAgcmFont->FontDrawStart(0);
					g_pcsAgcmFont->DrawTextIM2D( 20.0f , ( float ) 730 , str, 0 , 255 );
					g_pcsAgcmFont->FontDrawEnd();
				}
				break;
			case MiniDumpNormal:
			default:
				// do nothing
				break;
			}
		}
	}
}

void MainWindow::OnPostRender	( RwRaster *raster	)	// 렌더는 여기서;
{
	PROFILE("MainWindow::OnPostRender()");
}

void MainWindow::DrawGeometryGrid()
{
	if( !m_lShowHeightGrid				&&
		!m_lShowGeometryBlocking		&&
		!m_lShowSkyBlocking				&&
		!m_lShowRidableObjectAvailable	&& 
		!m_lShowObjectBlocking			&&
		!m_lShowCharacterBlocking		&&
		!m_lShowLineBlocking )
		return;

	if( !g_pcsAgcmCharacter->GetSelfCharacter() )	return;

	AuPOS pos = g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos;
	ApWorldSector* pSectorCurrent = g_pcsApmMap->GetSector( pos );
	if( !pSectorCurrent )							return;

	INT32			nVertexCount		= 0;
	INT32			nTriangeCount		= 0;
	INT32			nIndexCount			= 0;

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL  );

	// 좌표 얻어냄..
	FLOAT	fStartX , fStartZ;
	INT32	nIndexX , nIndexZ;
	pSectorCurrent->D_GetSegment( pos.x , pos.z , &nIndexX , &nIndexZ );
	pSectorCurrent->D_GetSegment( nIndexX , nIndexZ , &fStartX , &fStartZ );

	#define	GRID_DEPTH	16
	int		nDepth	= GRID_DEPTH;
	fStartX	-= MAP_STEPSIZE * ( FLOAT ) nDepth / 2.0f;
	fStartZ	-= MAP_STEPSIZE * ( FLOAT ) nDepth / 2.0f;

	if( m_lShowHeightGrid )
	{
		// 그리드 출력
		//static	RwIm3DVertex	s_pVertexBuffer[ ( GRID_DEPTH	+ 1 ) * ( GRID_DEPTH	+ 1 ) ]; 
		//static	RwImVertexIndex	s_pVertexIndex[ ( GRID_DEPTH	) * ( GRID_DEPTH	) * 4 * 2 ];
		static	INT32		s_nType = 1;

		// 현재 지형의 Wire Frame 을 그려줌.
		INT32	r,g,b,a;

		switch( s_nType )
		{
		case 0:
			return;
		default:
			r = 255 , g = 255 , b = 255 , a = 128; break;
		}
		
		RwIm3DVertex * pLineList = m_pLineList;

		float	fX	, fZ;
		int		i	, j ;
		float	height	;

		for( j = 0 ; j < nDepth + 1 ; j ++ )
		{
			for( i = 0 ; i < nDepth + 1 ; i ++ )
			{
				fX = fStartX + ( FLOAT ) i * MAP_STEPSIZE ;
				fZ = fStartZ + ( FLOAT ) j * MAP_STEPSIZE ;
				
//				switch( s_nType )
//				{
//				default:
					height = AGCMMAP_THIS->GetHeight( fX , fZ , g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.y);
//					break;
//				}

				RwIm3DVertexSetPos	( &pLineList[ i + j * ( nDepth + 1 ) ] , 
										fX,
										height + 10.0f,
										fZ);

				RwIm3DVertexSetU	( &pLineList[ i + j * ( nDepth + 1 ) ] , 1.0f );    
				RwIm3DVertexSetV	( &pLineList[ i + j * ( nDepth + 1 ) ] , 1.0f );
				RwIm3DVertexSetRGBA	( &pLineList[ i + j * ( nDepth + 1 ) ] , r , g , b , a );
			}
		}

		RwImVertexIndex	* pIndex = m_pIndex;
		for( j = 0 ; j < nDepth ; j ++ )
		{
			for( i = 0 ; i < nDepth  ; i ++ )
			{
				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 0	] = ( i		) +	( j		) * ( nDepth + 1 );
				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 1	] = ( i + 1	) +	( j		) * ( nDepth + 1 );

				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 2	] = ( i	+ 1	) +	( j		) * ( nDepth + 1 );
				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 3	] = ( i	+ 1	) +	( j + 1	) * ( nDepth + 1 );

				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 4	] = ( i	+ 1	) +	( j + 1	) * ( nDepth + 1 );
				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 5	] = ( i		) +	( j	+ 1	) * ( nDepth + 1 );

				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 6	] = ( i		) +	( j	+ 1	) * ( nDepth + 1 );
				pIndex[ ( i + j * ( nDepth ) ) * 4 * 2 + 7	] = ( i		) +	( j		) * ( nDepth + 1 );
			}
		}

		if( RwIm3DTransform( pLineList , ( nDepth + 1 ) * ( nDepth + 1 ) , NULL, rwIM3D_ALLOPAQUE ) )
		{                         
			RwIm3DRenderIndexedPrimitive( rwPRIMTYPELINELIST,
				pIndex , ( nDepth ) * ( nDepth ) * 4 * 2 );

			RwIm3DEnd();
		}	
	}

	//if( m_lShowRidableObjectAvailable )
	//{
		// 그리드 출력

		/*
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL  );

		AuPOS				pos;
		ApWorldSector		* pSector = NULL;

		if(g_pcsAgcmCharacter->GetSelfCharacter())
		{
			pos		= g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos;
			pSector	= g_pcsApmMap->GetSector( pos );
		}

		if( pSector )
		{
			RpAtomic		* pCollision	;
			rsDWSectorInfo	stDetailInfo	;
			int				oid				;

			RwV3d			vOffset			;
			vOffset.x = 0.0f	;
			vOffset.y = 1000.0f	;
			vOffset.z = 0.0f	;

			AGCMMAP_THIS->GetDWSector( pSector , pSector->GetCurrentDetail() , &stDetailInfo );
			INT32	nType = AcuObject::GetAtomicType( stDetailInfo.pDWSector->atomic , &oid , NULL , NULL , NULL, ( void ** ) &pCollision );

			__RenderAtomicOutLine( pCollision );
		}
		*/
	//}

	if( m_lShowGeometryBlocking || m_lShowRidableObjectAvailable || m_lShowObjectBlocking || m_lShowSkyBlocking )
	{
		// __RenderGeometryBlocking( );
		float	fX	, fZ;
		int		i	, j ;
		//float	height	;

		ApWorldSector * pSector;
		ApCompactSegment	* pSegment;

		FLOAT	fXOrigin	;
		FLOAT	fZOrigin	;
		FLOAT	pHeight[ 9 ];
		FLOAT	fHeight		;

		RwIm3DVertex * pLineList = m_pLineList;
		RwImVertexIndex	* pIndex = m_pIndex;

		nVertexCount		= 0;
		nTriangeCount		= 0;
		nIndexCount			= 0;

		INT32 nCountRidable;
		INT32 nCountBlocking;
		INT32 pArrayID[ 10 ];

		for( j = 0 ; j < nDepth + 1 ; j ++ )
		{
			for( i = 0 ; i < nDepth + 1 ; i ++ )
			{
				fX = fStartX + ( FLOAT ) i * MAP_STEPSIZE ;
				fZ = fStartZ + ( FLOAT ) j * MAP_STEPSIZE ;

				pSector = g_pcsApmMap->GetSector( fX , fZ );
				if( NULL == pSector ) continue;

				pSector->D_GetSegment( fX , fZ , &nIndexX , &nIndexZ );
				pSegment = pSector->C_GetSegment( nIndexX , nIndexZ );

				if( NULL == pSegment ) continue;

				nCountRidable = pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , nIndexX , nIndexZ , pArrayID , SECTOR_MAX_COLLISION_OBJECT_COUNT );
				nCountBlocking = pSector->GetIndex( ApWorldSector::AWS_COLLISIONOBJECT , nIndexX , nIndexZ , pArrayID , SECTOR_MAX_COLLISION_OBJECT_COUNT );

				// 마고자 (2005-03-31 오전 11:35:00) : 그무엇도 만족하지 않으면..
				if(	( m_lShowGeometryBlocking && !pSegment->stTileInfo.GetGeometryBlocking()  ) 	&&
					( m_lShowRidableObjectAvailable && nCountRidable == 0		) &&
					( m_lShowObjectBlocking && nCountBlocking == 0 )
				) continue;

				fXOrigin	= fX;
				fZOrigin	= fZ;

				// 높이 정리해둠..
				pHeight[ 0 ]	=	AGCMMAP_THIS->HP_GetHeight( fX					, fZ				);
				pHeight[ 2 ]	=	AGCMMAP_THIS->HP_GetHeight( fX + MAP_STEPSIZE	, fZ				);
				pHeight[ 6 ]	=	AGCMMAP_THIS->HP_GetHeight( fX					, fZ + MAP_STEPSIZE	);
				pHeight[ 8 ]	=	AGCMMAP_THIS->HP_GetHeight( fX + MAP_STEPSIZE	, fZ + MAP_STEPSIZE	);

				// 오브젝트 블러킹 표시..
				if( m_lShowGeometryBlocking )
				{
					if( pSegment->stTileInfo.GetGeometryBlocking() )
					{
						fHeight = pHeight[ 0 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , fHeight , fZ	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 255 , 0 , 0 , 128	);

						fHeight = pHeight[ 2 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 255 , 0 , 0 , 128	);
						
						fHeight = pHeight[ 6 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 255 , 0 , 0 , 128	);
						
						fHeight = pHeight[ 8 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE , fHeight , fZ + MAP_STEPSIZE	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 255 , 0 , 0 , 128	);

						pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
						pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
						pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
						pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
						pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
						pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

						nIndexCount		+= 6;
						nVertexCount	+= 4;
						nTriangeCount	+= 2;
					}


					/*
					 // 오브젝트 블러킹 별로 쓸일 없으니까 표시 생략시킴.
					if( pSegment->stTileInfo.objectblock )
					{
						// 여분의 그리드 정보 획득.
						pHeight[ 1 ]	=	( pHeight[ 0 ] + pHeight[ 2 ] ) * 0.5f;
						pHeight[ 3 ]	=	( pHeight[ 0 ] + pHeight[ 6 ] ) * 0.5f;
						pHeight[ 4 ]	=	( pHeight[ 2 ] + pHeight[ 6 ] ) * 0.5f;
						pHeight[ 5 ]	=	( pHeight[ 2 ] + pHeight[ 8 ] ) * 0.5f;
						pHeight[ 7 ]	=	( pHeight[ 6 ] + pHeight[ 8 ] ) * 0.5f;

						if( pSegment->stTileInfo.GetObjectBlocking( TP_LEFTTOP ) )
						{
							fX	= fXOrigin + 0;
							fZ	= fZOrigin + 0;

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , pHeight[ 0 ] , fZ	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , 0 , 255 , 128	);

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 1 ] , fZ + 0	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , pHeight[ 3 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 4 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , 0 , 255 , 128	);

							pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
							pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

							nIndexCount		+= 6;
							nVertexCount	+= 4;
							nTriangeCount	+= 2;
						}
						
						if( pSegment->stTileInfo.GetObjectBlocking( TP_RIGHTTOP ) )
						{
							fX	= fXOrigin + MAP_STEPSIZE / 2.0f;
							fZ	= fZOrigin + 0;

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , pHeight[ 0 ] , fZ	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , 0 , 255 , 128	);

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 1 ] , fZ + 0	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , pHeight[ 3 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 4 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , 0 , 255 , 128	);

							pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
							pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

							nIndexCount		+= 6;
							nVertexCount	+= 4;
							nTriangeCount	+= 2;
						}	
						
						if( pSegment->stTileInfo.GetObjectBlocking( TP_LEFTBOTTOM ) )
						{
							fX	= fXOrigin + 0;
							fZ	= fZOrigin + MAP_STEPSIZE / 2.0f;

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , pHeight[ 0 ] , fZ	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , 0 , 255 , 128	);

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 1 ] , fZ + 0	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , pHeight[ 3 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 4 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , 0 , 255 , 128	);

							pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
							pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

							nIndexCount		+= 6;
							nVertexCount	+= 4;
							nTriangeCount	+= 2;
						}	
						
						if( pSegment->stTileInfo.GetObjectBlocking( TP_RIGHTBOTTOM ) )
						{
							fX	= fXOrigin + MAP_STEPSIZE / 2.0f;
							fZ	= fZOrigin + MAP_STEPSIZE / 2.0f;
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , pHeight[ 0 ] , fZ	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , 0 , 255 , 128	);

							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 1 ] , fZ + 0	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , pHeight[ 3 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , 0 , 255 , 128	);
							
							RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE / 2.0f , pHeight[ 4 ] , fZ + MAP_STEPSIZE / 2.0f	);
							RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f				);    
							RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f				);
							RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , 0 , 255 , 128	);

							pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
							pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
							pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
							pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

							nIndexCount		+= 6;
							nVertexCount	+= 4;
							nTriangeCount	+= 2;
						}
					}
					*/

				}
				// 오브젝트 블러킹 표시..
				if( m_lShowGeometryBlocking )
				{
					if( pSegment->stTileInfo.GetSkyBlocking() )
					{
						fHeight = pHeight[ 0 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , fHeight , fZ	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , 0 , 255 , 128	);

						fHeight = pHeight[ 2 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , 0 , 255 , 128	);
						
						fHeight = pHeight[ 6 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , 0 , 255 , 128	);
						
						fHeight = pHeight[ 8 ] + GEOMETRY_BLOCKING_OFFSET;
						RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE , fHeight , fZ + MAP_STEPSIZE	);
						RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f					);    
						RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f					);
						RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , 0 , 255 , 128	);

						pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
						pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
						pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
						pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
						pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
						pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

						nIndexCount		+= 6;
						nVertexCount	+= 4;
						nTriangeCount	+= 2;
					}
				}

				// 라이더블 갯수표시..
				if( m_lShowRidableObjectAvailable && nCountRidable )
				{
					fHeight = pHeight[ 0 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , fHeight , fZ	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , nCountRidable * 50 , 0 , 128	);

					fHeight = pHeight[ 2 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , nCountRidable * 50 , 0 , 128	);
					
					fHeight = pHeight[ 6 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , nCountRidable * 50 , 0 , 128	);
					
					fHeight = pHeight[ 8 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE , fHeight , fZ + MAP_STEPSIZE	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , nCountRidable * 50 , 0 , 128	);

					pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
					pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
					pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
					pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
					pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
					pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

					nIndexCount		+= 6;
					nVertexCount	+= 4;
					nTriangeCount	+= 2;
				}

				// 라이더블 갯수표시..
				if( m_lShowObjectBlocking && nCountBlocking )
				{
					fHeight = pHeight[ 0 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 0 ] , fX , fHeight , fZ	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 0 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 0 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 0 ] , 0 , 0 , nCountBlocking * 50 , 128	);

					fHeight = pHeight[ 2 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 1 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 1 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 1 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 1 ] , 0 , 0 , nCountBlocking * 50 , 128	);
					
					fHeight = pHeight[ 6 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 2 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 2 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 2 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 2 ] , 0 , 0 , nCountBlocking * 50 , 128	);
					
					fHeight = pHeight[ 8 ] + GEOMETRY_BLOCKING_OFFSET;
					RwIm3DVertexSetPos	( &pLineList[ nVertexCount + 3 ] , fX + MAP_STEPSIZE , fHeight , fZ + MAP_STEPSIZE	);
					RwIm3DVertexSetU	( &pLineList[ nVertexCount + 3 ] , 1.0f					);    
					RwIm3DVertexSetV	( &pLineList[ nVertexCount + 3 ] , 1.0f					);
					RwIm3DVertexSetRGBA	( &pLineList[ nVertexCount + 3 ] , 0 , 0 , nCountBlocking * 50 , 128	);

					pIndex[ nTriangeCount * 3 + 0 ] = nVertexCount + 0 ;
					pIndex[ nTriangeCount * 3 + 1 ] = nVertexCount + 1 ;
					pIndex[ nTriangeCount * 3 + 2 ] = nVertexCount + 2 ;
					pIndex[ nTriangeCount * 3 + 3 ] = nVertexCount + 2 ;
					pIndex[ nTriangeCount * 3 + 4 ] = nVertexCount + 1 ;
					pIndex[ nTriangeCount * 3 + 5 ] = nVertexCount + 3 ;

					nIndexCount		+= 6;
					nVertexCount	+= 4;
					nTriangeCount	+= 2;
				}
			}
		}

		if( nVertexCount )
		{
			if( RwIm3DTransform( pLineList , nVertexCount , NULL, rwIM3D_VERTEXRGBA ) )
			{                         
				RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST, pIndex, nIndexCount );
				RwIm3DEnd();
			}
		}
	}

	if( m_lShowCharacterBlocking )
	{
		INT32	lIndex = 0;

		AgpdCharacter			*pcsCharacter;
		AgpdCharacterTemplate	*pcsTemplate;

		RwIm3DVertex * pLineList = m_pLineList	;
		RwImVertexIndex	* pIndex = m_pIndex		;

		int nCount = 0;

		for (pcsCharacter = g_pcsAgpmCharacter->GetCharSequence(&lIndex);
			pcsCharacter;
			pcsCharacter = g_pcsAgpmCharacter->GetCharSequence(&lIndex))
		{
			// 케릭터들 돌면서 블러킹 정보 출력.
			pcsTemplate = pcsCharacter->m_pcsCharacterTemplate;

			if( pcsTemplate->IsBlocking() )
			{
				if( pcsTemplate->IsBoxCollision() )
				{
					AuBoxCollision bcol;
					bcol.SetSize( pcsTemplate->m_fSiegeWarCollBoxWidth , pcsTemplate->m_fSiegeWarCollBoxHeight );
					bcol.SetOffset( pcsTemplate->m_fSiegeWarCollObjOffsetX , pcsTemplate->m_fSiegeWarCollObjOffsetZ );
					bcol.Transform( pcsCharacter->m_stPos , pcsCharacter->m_fTurnY );

					static FLOAT	fBlockingVisibleHeight = 3000.0f;

					for( int i = 0 ; i < 4 ; i ++ )
					{
						bcol.p[ i ].y = AGCMMAP_THIS->GetHeight( bcol.p[ i ].x , bcol.p[ i ].z , SECTOR_MAX_HEIGHT );
					}

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 0 ] , bcol.p[ 0 ].x , bcol.p[ 0 ].y , bcol.p[ 0 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 0 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 0 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 0 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 1 ] , bcol.p[ 1 ].x , bcol.p[ 1 ].y , bcol.p[ 1 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 1 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 1 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 1 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 2 ] , bcol.p[ 2 ].x , bcol.p[ 2 ].y , bcol.p[ 2 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 2 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 2 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 2 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 3 ] , bcol.p[ 3 ].x , bcol.p[ 3 ].y , bcol.p[ 3 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 3 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 3 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 3 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 4 ] , bcol.p[ 0 ].x , bcol.p[ 0 ].y + fBlockingVisibleHeight , bcol.p[ 0 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 4 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 4 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 4 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 5 ] , bcol.p[ 1 ].x , bcol.p[ 1 ].y + fBlockingVisibleHeight , bcol.p[ 1 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 5 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 5 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 5 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 6 ] , bcol.p[ 2 ].x , bcol.p[ 2 ].y + fBlockingVisibleHeight , bcol.p[ 2 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 6 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 6 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 6 ] , 255 , 0 , 0 , 128 );

					RwIm3DVertexSetPos	( &pLineList[ nCount * 8 + 7 ] , bcol.p[ 3 ].x , bcol.p[ 3 ].y + fBlockingVisibleHeight , bcol.p[ 3 ].z	);
					RwIm3DVertexSetU	( &pLineList[ nCount * 8 + 7 ] , 1.0f );    
					RwIm3DVertexSetV	( &pLineList[ nCount * 8 + 7 ] , 1.0f );
					RwIm3DVertexSetRGBA	( &pLineList[ nCount * 8 + 7 ] , 255 , 0 , 0 , 128 );

					pIndex[ nCount * 24 + 0  ] = nCount * 8 + 4 ;
					pIndex[ nCount * 24 + 1  ] = nCount * 8 + 1 ;
					pIndex[ nCount * 24 + 2  ] = nCount * 8 + 0 ;

					pIndex[ nCount * 24 + 3  ] = nCount * 8 + 4 ;
					pIndex[ nCount * 24 + 4  ] = nCount * 8 + 5 ;
					pIndex[ nCount * 24 + 5  ] = nCount * 8 + 1 ;

					pIndex[ nCount * 24 + 6  ] = nCount * 8 + 5 ;
					pIndex[ nCount * 24 + 7  ] = nCount * 8 + 2 ;
					pIndex[ nCount * 24 + 8  ] = nCount * 8 + 1 ;

					pIndex[ nCount * 24 + 9  ] = nCount * 8 + 5 ;
					pIndex[ nCount * 24 + 10 ] = nCount * 8 + 6 ;
					pIndex[ nCount * 24 + 11 ] = nCount * 8 + 2 ;

					pIndex[ nCount * 24 + 12 ] = nCount * 8 + 6 ;
					pIndex[ nCount * 24 + 13 ] = nCount * 8 + 3 ;
					pIndex[ nCount * 24 + 14 ] = nCount * 8 + 2 ;

					pIndex[ nCount * 24 + 15 ] = nCount * 8 + 6 ;
					pIndex[ nCount * 24 + 16 ] = nCount * 8 + 7 ;
					pIndex[ nCount * 24 + 17 ] = nCount * 8 + 3 ;

					pIndex[ nCount * 24 + 18 ] = nCount * 8 + 7 ;
					pIndex[ nCount * 24 + 19 ] = nCount * 8 + 0 ;
					pIndex[ nCount * 24 + 20 ] = nCount * 8 + 3 ;

					pIndex[ nCount * 24 + 21 ] = nCount * 8 + 7 ;
					pIndex[ nCount * 24 + 22 ] = nCount * 8 + 4 ;
					pIndex[ nCount * 24 + 23 ] = nCount * 8 + 0 ;

					nCount++;
				}
				else
				{
					// 실린더 콜리젼..

					RwSphere stSphere;
					stSphere.center.x = pcsCharacter->m_stPos.x;
					stSphere.center.y = pcsCharacter->m_stPos.y;
					stSphere.center.z = pcsCharacter->m_stPos.z;
					stSphere.center.y += 100.0f;
					stSphere.radius = pcsTemplate->m_fSiegeWarCollSphereRadius;

					Eff2Ut_RenderSphereXZ( stSphere );
				}
			}
		}

		if( nCount )
		{
			if( RwIm3DTransform( pLineList , nCount * 8 , NULL, rwIM3D_VERTEXRGBA ) )
			{                         
				RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST, pIndex, nCount * 24 );
				RwIm3DEnd();
			}
		}
	}

	if( m_lShowLineBlocking )
	{
		UINT32			uSectorCount = g_pcsApmMap->GetCurrentLoadedSectorCount	();

		RwIm3DVertex * pLineList = m_pLineList	;
		RwImVertexIndex	* pIndex = m_pIndex		;

		INT32			nCount = 0;

		RwUInt32    colorFront , colorBack;

		RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 255 , 0 , 0 , 128  );
		colorFront = pLineList[ 0 ].color;
		RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , 128  );
		colorBack = pLineList[ 0 ].color;

		FLOAT fBlockingVisibleHeight = 2000.0f;

		vector< UINT32 > vecIndex;

		for( UINT32 i = 0 ; i < uSectorCount ; i ++ )
		{
			AuPOS pos = g_pcsAgcmCharacter->GetSelfCharacter()->m_stPos;
			ApWorldSector * pSector = g_pcsApmMap->GetCurrentLoadedSectors()[ i ];

			#define ABS( value ) ( ( value ) > 0 ? ( value ) : - ( value ) )

			if( ABS( pSector->GetArrayIndexX() - pSectorCurrent->GetArrayIndexX() ) > 1 ||
				ABS( pSector->GetArrayIndexZ() - pSectorCurrent->GetArrayIndexZ() ) > 1 )
				continue;

			for( vector< ApWorldSector::AuLineBlock >::iterator iTer = pSector->m_vecBlockLine.begin();
					iTer != pSector->m_vecBlockLine.end() ;
					iTer ++ )
			{
				ApWorldSector::AuLineBlock * pLine = &( *iTer );

				// 겹치나.
				{
					BOOL	bFound = FALSE;

					for( vector< UINT32 >::iterator	iter = vecIndex.begin(); iter != vecIndex.end(); ++iter )
					{
						if( pLine->uSerial == *iter )
						{
							bFound = TRUE;
							break;
						}
					}

					if( bFound ) continue;
				}

				vecIndex.push_back( pLine->uSerial );

				RwIm3DVertexSetPos	( &pLineList[ 0 + nCount * 8] , pLine->start.x , pLine->start.y + fBlockingVisibleHeight , pLine->start.z	);
				RwIm3DVertexSetU	( &pLineList[ 0 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 0 + nCount * 8 ] , 1.0f				);
				pLineList[ 0 + nCount * 8 ].color = colorFront;

				RwIm3DVertexSetPos	( &pLineList[ 1 + nCount * 8 ] , pLine->end.x , pLine->end.y + fBlockingVisibleHeight , pLine->end.z	);
				RwIm3DVertexSetU	( &pLineList[ 1 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 1 + nCount * 8 ] , 1.0f				);
				pLineList[ 1 + nCount * 8 ].color = colorFront;
				
				RwIm3DVertexSetPos	( &pLineList[ 2 + nCount * 8 ] , pLine->start.x , pLine->start.y , pLine->start.z	);
				RwIm3DVertexSetU	( &pLineList[ 2 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 2 + nCount * 8 ] , 1.0f				);
				pLineList[ 2 + nCount * 8 ].color = colorFront;
				
				RwIm3DVertexSetPos	( &pLineList[ 3 + nCount * 8 ] , pLine->end.x , pLine->end.y , pLine->end.z	);
				RwIm3DVertexSetU	( &pLineList[ 3 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 3 + nCount * 8 ] , 1.0f				);
				pLineList[ 3 + nCount * 8 ].color = colorFront;

				RwIm3DVertexSetPos	( &pLineList[ 4 + nCount * 8 ] , pLine->end.x , pLine->end.y + fBlockingVisibleHeight , pLine->end.z	);
				RwIm3DVertexSetU	( &pLineList[ 4 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 4 + nCount * 8 ] , 1.0f				);
				pLineList[ 4 + nCount * 8 ].color = colorBack;

				RwIm3DVertexSetPos	( &pLineList[ 5 + nCount * 8 ] , pLine->start.x , pLine->start.y + fBlockingVisibleHeight , pLine->start.z	);
				RwIm3DVertexSetU	( &pLineList[ 5 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 5 + nCount * 8 ] , 1.0f				);
				pLineList[ 5 + nCount * 8 ].color = colorBack;

				RwIm3DVertexSetPos	( &pLineList[ 6 + nCount * 8 ] , pLine->end.x , pLine->end.y , pLine->end.z	);
				RwIm3DVertexSetU	( &pLineList[ 6 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 6 + nCount * 8 ] , 1.0f				);
				pLineList[ 6 + nCount * 8 ].color = colorBack;
				
				RwIm3DVertexSetPos	( &pLineList[ 7 + nCount * 8 ] , pLine->start.x , pLine->start.y , pLine->start.z	);
				RwIm3DVertexSetU	( &pLineList[ 7 + nCount * 8 ] , 1.0f				);    
				RwIm3DVertexSetV	( &pLineList[ 7 + nCount * 8 ] , 1.0f				);
				pLineList[ 7 + nCount * 8 ].color = colorBack;

				pIndex[ 0  + nCount * 12]	= 1 + nCount * 8;
				pIndex[ 1  + nCount * 12]	= 2 + nCount * 8;
				pIndex[ 2  + nCount * 12]	= 0 + nCount * 8;
				pIndex[ 3  + nCount * 12]	= 1 + nCount * 8;
				pIndex[ 4  + nCount * 12]	= 3 + nCount * 8;
				pIndex[ 5  + nCount * 12]	= 2 + nCount * 8;

				pIndex[ 6  + nCount * 12]	= 5 + nCount * 8;
				pIndex[ 7  + nCount * 12]	= 6 + nCount * 8;
				pIndex[ 8  + nCount * 12]	= 4 + nCount * 8;
				pIndex[ 9  + nCount * 12]	= 5 + nCount * 8;
				pIndex[ 10 + nCount * 12]	= 7 + nCount * 8;
				pIndex[ 11 + nCount * 12]	= 6 + nCount * 8;

				nCount ++;
			}
		}

		if( nCount )
		{
			RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK );

			if( RwIm3DTransform( pLineList , nCount * 8 , NULL, rwIM3D_VERTEXRGBA ) )
			{                         
				RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST, pIndex , nCount * 12 );
				RwIm3DEnd();
			}

			RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE );
		}
	}
}

void	__RenderAtomicOutLine( RpAtomic * pAtomic )
{
#ifdef _DEBUG
	// 마고자 (2004-02-04 오후 12:15:13) : 아토믹 폴리건 출력.
	if( !pAtomic )			return;

	RpGeometry * pGeometry = RpAtomicGetGeometry( pAtomic );
	if( !pGeometry )		return;

	RpMorphTarget* pMorphTarget = RpGeometryGetMorphTarget( pGeometry , 0 );
	if( !pMorphTarget )		return;

	static RwIm3DVertex pLineList[ 65536 ];
	static RwImVertexIndex pIndex[ 65536 ];

	// 버텍스 카피.
	for( int i = 0 ; i < pGeometry->numVertices ; ++i )
	{
        RwIm3DVertexSetPos( pLineList + i, pMorphTarget->verts[i].x, pMorphTarget->verts[i].y + 50.0f, pMorphTarget->verts[i].z );
        RwIm3DVertexSetRGBA( pLineList + i , 255 , 124 , 180 , 255 ); 
	}

	for( int i = 0 ; i < pGeometry->numTriangles ; ++i )
	{
		pIndex[ 6 * i + 0 ] = pGeometry->triangles[ i ].vertIndex[ 0 ];
		pIndex[ 6 * i + 1 ] = pGeometry->triangles[ i ].vertIndex[ 1 ];
		pIndex[ 6 * i + 2 ] = pGeometry->triangles[ i ].vertIndex[ 1 ];
		pIndex[ 6 * i + 3 ] = pGeometry->triangles[ i ].vertIndex[ 2 ];
		pIndex[ 6 * i + 4 ] = pGeometry->triangles[ i ].vertIndex[ 2 ];
		pIndex[ 6 * i + 5 ] = pGeometry->triangles[ i ].vertIndex[ 0 ];
	}

	if( RwIm3DTransform( pLineList , pGeometry->numVertices , NULL, rwIM3D_ALLOPAQUE ) )
	{                         
		RwIm3DRenderIndexedPrimitive( rwPRIMTYPELINELIST, pIndex , pGeometry->numTriangles * 6 );
		RwIm3DEnd();
	}

#endif // _DEBUG
}

void MainWindow::ReportApMemory()
{
	ApMemoryManager::GetInstance().ReportMemoryInfo();
}

void MainWindow::IntensiveCrash()
{
	MD_SetErrorMessage( "IntensiveCrash()" );

	// 의도적인 충돌..
	int *pNull = NULL;
	*pNull = 0; // Crash
}

void	MainWindow::AutoHuntIdle()
{
	/*
	if( g_pEngine->GetDebugFlag() & AgcEngine::AUTO_HUNTING )
	{
		AgpdCharacter * pSelfCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
		if( !pSelfCharacter ) return;

		INT32 nTargetCID = pSelfCharacter->m_alTargetCID[ 0 ];
		AgpdCharacter * pTarget = g_pcsAgpmCharacter->GetCharacter( nTargetCID );

		// 자동 사냥 Process 

		// 현재 내가 타겟을 잡고 있는것이 있으면 Random Skill 실행.
		// 타겟을 잡고 있는 것이 없으면 근처에 몬스터 검색해서 돌진.
		// 근처에 타겟이 없는 경우 매크로 스타트 지점으로 이동.

		if( nTargetCID && pTarget && !pTarget->IsDead() )
		{
			// 타겟을 잡고 있는 경우..
			// 직접 클릭하고 랜덤 스킬을 실행한다.
			g_pcsAgcmTargeting->TargetCID( nTargetCID , TRUE );

			// 스킬을 사용한다.

			for( int i = 0 ; i < 6 ; i ++ )
			{
				if( g_pcsAgcmUIMain->UseBelt( 0 , i ) ) break;
			}
		}
		else
		{
			// 타겟이 없느경우 
			// 케릭터 리스트르 돌면서 몬스터를 골라서
			// 타겟으로 잡는다.
			AgpdCharacter			*pcsCharacterNearest = NULL ;
			FLOAT					fNearestDistance = 999999999999999.0f;

			INT32	lIndex = 0;
			bool	bFirst	= true;

			for (AgpdCharacter * pcsCharacter = g_pcsAgpmCharacter->GetCharSequence(&lIndex);
				pcsCharacter;
				pcsCharacter = g_pcsAgpmCharacter->GetCharSequence(&lIndex))
			{
				// 나인지
				if( pcsCharacter == pSelfCharacter ) continue;

				// 몬스터인지 확인
				if( !g_pcsAgpmCharacter->IsMonster( pcsCharacter ) ) continue;

				// 살아있는지

				if( pcsCharacter->IsDead() ) continue;

				// 거리 확인
				FLOAT	fDistance;
				fDistance = AUPOS_DISTANCE_XZ( pSelfCharacter->m_stPos , pcsCharacter->m_stPos );

				// 15미터 안쪽에 것만 검사..
				if( bFirst || ( fDistance < 1500.0f && fDistance < fNearestDistance )  )
				{
					bFirst = false;
					fNearestDistance = fDistance;
					pcsCharacterNearest = pcsCharacter;
				}
			}

			if( pcsCharacterNearest )
			{
				// 이놈을 패라..
				g_pcsAgcmTargeting->TargetCID( pcsCharacterNearest->m_lID , TRUE );
			}
		}
		
	}
	*/
}

BOOL	MainWindow::CBObjectLoadError(PVOID pData, PVOID pClass, PVOID pCustData)
{
	g_pEngine->SetDebugFlag( g_pEngine->GetDebugFlag() | AgcEngine::OBJECT_LOADING_ERROR );
	return TRUE;
}

BOOL	MainWindow::CBAdminClientLogin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminServerInfo* ppdServerInfo = ( stAgpdAdminServerInfo* )pData;
	MainWindow* pThis = static_cast<MainWindow*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pCustData);

	// 어드민모듈이 로드되어 있지 않으면
	if( !g_pcsAgcmAdmin->GetDLLModuleHandle() )
	{
		// 어드민DLL 을 로딩하고 초기화
		if( g_pcsAgcmAdmin->StartAdminClient() )
		{
			// 이벤트를 어드민DLL 창으로도 보내줄수 있도록 연결하고..
			pThis->SetDialogMSGFuncClass( g_pcsAgcmAdmin->CBIsDialogMessage, g_pcsAgcmAdmin );

			ADMIN_CB fnOpenDlg = g_pcsAgcmAdmin->GetDLLFunction( "ADMIN_Main_OnOpenDlg" );
			if( fnOpenDlg )
			{
				// 어드민 DLL 창을 연다.
				fnOpenDlg( NULL, NULL, NULL );
			}
		}
	}
	else
	{
		// 이벤트를 어드민DLL 창으로도 보내줄수 있도록 연결하고..
		pThis->SetDialogMSGFuncClass( g_pcsAgcmAdmin->CBIsDialogMessage, g_pcsAgcmAdmin );

		ADMIN_CB fnOpenDlg = g_pcsAgcmAdmin->GetDLLFunction( "ADMIN_Main_OnOpenDlg" );
		if( fnOpenDlg )
		{
			// 어드민 DLL 창을 연다.
			fnOpenDlg( NULL, NULL, NULL );
		}
	}

	return TRUE;
}

void	MainWindow::VideoMemoryCheck()
{
	static	UINT32	s_uLastCheck = GetTickCount();

	UINT32	 uCurrent = GetTickCount();

	if( uCurrent - s_uLastCheck > 1000 )
	{
        const RwUInt32 memAvailable = IDirect3DDevice9_GetAvailableTextureMem((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
		s_uLastCheck = uCurrent;

		char	szText[ 256 ];
		sprintf( szText, "Video Memory Available State : %.0fMB\n", ( FLOAT ) memAvailable / ( 1024.0f * 1024.0f ) );

		// //ddraw.lib dxguid.lib
		//DWORD	dwTotal , dwFree;
		//if( g_pEngine->GetVideoMemoryState( &dwTotal , &dwFree ) )
		//{
		//	sprintf( str , "Video Memory Available State : %.0f%% ( %d / %d ) .... %d\n" ,
		//		( FLOAT ) dwFree / ( FLOAT ) dwTotal * 100.0f , dwFree , dwTotal , memAvailable );
		//}
		//else
		//{
		//	sprintf( str , "Video Memory Check Error!\n" );
		//}

		OutputDebugString( szText );
	}
}



CLogoViewer::CLogoViewer( void )
{
	m_nStartTime = 0;
	m_nDuration = 0;

	memset( m_strImageFileName, 0, sizeof( char ) * 256 );
	m_pTexture = NULL;
	m_fTextureWidth = 0.0f;
	m_fTextureHeight = 0.0f;
}

CLogoViewer::~CLogoViewer( void )
{
}

BOOL CLogoViewer::OnCreate( void )
{
	if( m_pTexture )
	{
		OnDestroy();
	}

	char* pFileName = NULL;
// 	switch( g_pcsAgcmUIOption->m_iScreenWidth )
// 	{
// 	case 1280 :		pFileName = "LG_Grade_1280";		break;
// 	case 1600 :		pFileName = "LG_Grade_1600";		break;
// 	default :		pFileName = "LG_Grade_1024";		break;
// 	}

	pFileName = "LG_Grade_1024";

	if (g_pcsAgcmUIOption->m_iScreenHeight >= 1200 && g_pcsAgcmUIOption->m_iScreenWidth >= 1600 )
		pFileName = "LG_Grade_1600";
	else if (g_pcsAgcmUIOption->m_iScreenHeight >= 1024 && g_pcsAgcmUIOption->m_iScreenHeight < 1200)
		pFileName = "LG_Grade_1280";

	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	m_pTexture = RwTextureRead( pFileName, NULL );
	if( !m_pTexture ) return FALSE;

	RwRaster* pRaster = RwTextureGetRaster( m_pTexture );
	if( !pRaster ) return FALSE;

	m_fTextureWidth = ( float )RwRasterGetWidth( pRaster );
	m_fTextureHeight = ( float )RwRasterGetHeight( pRaster );

	RwTextureSetAddressing( m_pTexture, rwTEXTUREADDRESSCLAMP );
	RwTextureSetFilterMode( m_pTexture, rwFILTERLINEARMIPLINEAR );

	m_nStartTime = 0;
	m_nDuration = 3000;

	return TRUE;
}

BOOL CLogoViewer::OnUpdate( __int64 nCurrTime )
{
	if( !g_pEngine || !m_pTexture ) return FALSE;

	if( m_nStartTime == 0 )
	{
		m_nStartTime = nCurrTime;
	}

	if( nCurrTime > m_nStartTime + m_nDuration )
	{
		OnDestroy();
		return FALSE;
	}

	return TRUE;
}

BOOL CLogoViewer::OnRender( void )
{
	if( !g_pEngine || !m_pTexture ) return FALSE;

	__int64 nTime = ::timeGetTime();
	if( !OnUpdate( nTime ) ) return FALSE;

	//g_pEngine->DrawIm2D( m_pTexture, 0.0f, 0.0f, m_fTextureWidth, m_fTextureHeight );

	RwRaster * camRaster = RwCameraGetRaster( g_pEngine->m_pCamera );

	if( m_pTexture && camRaster )
	{
		float scale = camRaster->height / AgcmUIManager2::GetModeHeight();
		float x = (camRaster->width * 0.5f) - (AgcmUIManager2::GetModeWidth()*0.5f*scale);

		g_pEngine->DrawIm2D( m_pTexture, 0, 0, (float)camRaster->width, (float)camRaster->height, 0, 0, 0, 0, 0xffffffff, 255 );

		FLOAT fWidth = ( FLOAT )( RwRasterGetWidth( m_pTexture->raster ) ) * scale;
		FLOAT fHeight = ( FLOAT )( RwRasterGetHeight( m_pTexture->raster ) ) * scale;

		g_pEngine->DrawIm2D( m_pTexture, x, 0, fWidth, fHeight );
	}
	return TRUE;
}

BOOL CLogoViewer::OnDestroy( void )
{
	if( m_pTexture )
	{
		RwTextureDestroy( m_pTexture );
		m_pTexture = NULL;
	}

	m_nStartTime = 0;
	return TRUE;
}


