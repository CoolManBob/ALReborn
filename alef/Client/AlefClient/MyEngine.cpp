#include "resource.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpusrdat.h"
#include "rpspline.h"
#include "rtanim.h"
#include "rplodatm.h"
#include "rpmatfx.h"
#include "rpmorph.h"
#include "rpuvanim.h"
#include "rptoon.h"
#include "rprandom.h"
#include "rppvs.h"
#include "rtbmp.h"
#include "rtpng.h"
#include "rpUVAnim.h"
#include "rpLODAtm.h"


#ifdef _DPVS
#include "rpdpvs.h"
#endif //_DPVS

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include "time.h"

#include "AcuRpMtexture.h"
#include "AcuRpMatFxEx.h"
#include "AcuRpUVAnimData.h"
#include "AcuObject.h"
#include "AcuRtAmbOcclMap.h"
#include "AcuMyFirewall.h"
#include "AcuRpMatD3DFx.h"

#include <win.h>
#include <wchar.h>
#include <io.h>

#include "ApMemoryTracker.h"
#include "AgpmPathFind.h"

#include "AuPacket.h"
#include "AuStrTable.h"
#include "AuIniManager.h"
#include "AuFilefind.h"
#include "AuProcessManager.h"

#include "AgpmTitle.h"
#include "AgcmTitle.h"

#include "MyEngine.h"
#include "MainWindow.h"
#include "LuaFunc.h"

#ifdef _AREA_JAPAN_
	#include "AuJapaneseClientAuth.h"
	#include "AuGameInfoJpn.h"
	#include "libHGPSHELP.hpp"	//일본 게임 프로세스 감시 에이젼트

	HMODULE g_hgpshelp = NULL;

	#ifndef _DEBUG
		#include "AuNPGameLib.h"
		#pragma comment(lib, "NPGameLib_MTDLL.lib")

		BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
		char* GetNPMessage(DWORD dwResult);
	#endif
#endif

#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
	#include "AuNPGameLib.h"
#endif

#ifdef _AREA_GLOBAL_
	//#include "AuGlobalAuth.h"

	#include "AuNPGameLib.h"
#endif

//#include "AuGameGuard.h"
#include "AuNPGameLib.h"
//extern AuNPWrapper npgl;

#include "AuSingleProcessChecker.h"

#define PATCH_CLIENT_NAME			"archlord.exe"
#define DOWNLOAD_PATCH_CLIENT_NAME	"archlord.bak"
#define TEMP_PATCH_CLIENT_NAME		"_archlord.exe"

extern MINIDUMP_TYPE	g_eMiniDumpType	;//
extern AuCircularBuffer	g_AuCircularBuffer;
extern psGlobalType		PsGlobal;
extern BOOL				g_bUseTrace;

MyEngine				g_cMyEngine;

AgpmConfig*				g_pcsAgpmConfig = NULL;

AgcmResourceLoader*		g_pcsAgcmResourceLoader = NULL;

AgpmResourceInfo*		g_pcsAgpmResourceInfo	= NULL;

AgpmFactors*			g_pcsAgpmFactors	 = NULL;

ApmMap*					g_pcsApmMap = NULL;

ApmObject*				g_pcsApmObject = NULL;
AgcmObject*				g_pcsAgcmObject = NULL;

AgpmCharacter*			g_pcsAgpmCharacter	 = NULL;
AgcmCharacter*			g_pcsAgcmCharacter	 = NULL;

AgpmUnion*				g_pcsAgpmUnion		 = NULL;

AgpmItem*				g_pcsAgpmItem	 = NULL;
AgcmItem*				g_pcsAgcmItem	 = NULL;

AgpmItemConvert*		g_pcsAgpmItemConvert	= NULL;
AgcmItemConvert*		g_pcsAgcmItemConvert	= NULL;

AgpmCombat*				g_pcsAgpmCombat	 = NULL;

AgcmPrivateTrade*		g_pcsAgcmPrivateTrade	 = NULL;

AgcmConnectManager*		g_pcsAgcmConnectManager	 = NULL;
AgpmLogin*				g_pcsAgpmLogin = NULL;
AgcmLogin*				g_pcsAgcmLogin = NULL;

AgpmAuctionCategory*	g_pcsAgpmAuctionCategory	= NULL;
AgpmAuction*			g_pcsAgpmAuction			= NULL;
AgcmAuction*			g_pcsAgcmAuction			 = NULL;
AgcmRecruit*			g_pcsAgcmRecruit			 = NULL;

AgpmRide*				g_pcsAgpmRide	= NULL;
AgcmRide*				g_pcsAgcmRide	= NULL;

AgcmCamera2*			g_pcsAgcmCamera2			 = NULL;

AgcmRender*				g_pcsAgcmRender			 = NULL;

ApmEventManager*		g_pcsApmEventManager		 = NULL;
AgcmEventManager*		g_pcsAgcmEventManager	 = NULL;

AgpmTimer*				g_pcsAgpmTimer			 = NULL;

AgpmEventNature*		g_pcsAgpmEventNature		 = NULL;
AgcmEventNature*		g_pcsAgcmEventNature		 = NULL;

AgpmEventNPCDialog*		g_pcsAgpmEventNPCDialog	 = NULL;
AgcmEventNPCDialog*		g_pcsAgcmEventNPCDialog	 = NULL;

AgpmEventNPCTrade*		g_pcsAgpmEventNPCTrade	 = NULL;
AgcmEventNPCTrade*		g_pcsAgcmEventNPCTrade	 = NULL;

AgpmEventItemRepair*	g_pcsAgpmEventItemRepair	 = NULL;
AgcmEventItemRepair*	g_pcsAgcmEventItemRepair	 = NULL;

AgpmEventTeleport*		g_pcsAgpmEventTeleport	 = NULL;
AgcmEventTeleport*		g_pcsAgcmEventTeleport	 = NULL;

AgpmEventBank*			g_pcsAgpmEventBank		= NULL;
AgcmEventBank*			g_pcsAgcmEventBank		= NULL;

AgpmEventItemConvert*	g_pcsAgpmEventItemConvert	= NULL;
AgcmEventItemConvert*	g_pcsAgcmEventItemConvert	= NULL;

AgpmEventGuild*			g_pcsAgpmEventGuild = NULL;
AgcmEventGuild*			g_pcsAgcmEventGuild = NULL;

AgpmEventProduct*		g_pcsAgpmEventProduct = NULL;
AgcmEventProduct*		g_pcsAgcmEventProduct = NULL;

AgpmEventQuest*			g_pcsAgpmEventQuest = NULL;
AgcmEventQuest*			g_pcsAgcmEventQuest = NULL;

AgcmEventEffect*		g_pcsAgcmEventEffect	 = NULL;
AgcmEff2*				g_pcsAgcmEff2			 = NULL;
AgcmSound*				g_pcsAgcmSound			 = NULL;

AgpmParty*				g_pcsAgpmParty			 = NULL;
AgcmParty*				g_pcsAgcmParty			 = NULL;

AgcmUIManager2*			g_pcsAgcmUIManager2		 = NULL;


AgcmWater*				g_pcsAgcmWater			 = NULL;
AgcmLODManager*			g_pcsAgcmLODManager		 = NULL;
AgcmFont*				g_pcsAgcmFont			 = NULL;
AgcmLensFlare*			g_pcsAgcmLensFlare		 = NULL;

AgpmSkill*				g_pcsAgpmSkill			 = NULL;
AgcmSkill*				g_pcsAgcmSkill			 = NULL;

AgpmEventSkillMaster*	g_pcsAgpmEventSkillMaster	= NULL;
AgcmEventSkillMaster*	g_pcsAgcmEventSkillMaster	= NULL;

AgcmUIMain*				g_pcsAgcmUIMain			 = NULL;
AgcmUIControl*			g_pcsAgcmUIControl		 = NULL;
AgcmChatting2*			g_pcsAgcmChatting2		 = NULL;
AgpmChatting*			g_pcsAgpmChatting		 = NULL;
AgcmTextBoardMng*		g_pcsAgcmTextBoardMng		 = NULL;
AgcmTuner*				g_pcsAgcmTuner			 = NULL;
AgcmGlyph*				g_pcsAgcmGlyph			 = NULL;
AgcmTargeting*			g_pcsAgcmTargeting		 = NULL;

AgpmGrid*				g_pcsAgpmGrid			 = NULL;

AgcmGlare*				g_pcsAgcmGlare			 = NULL;
AgpmWorld*				g_pcsAgpmWorld			 = NULL;
AgcmWorld*				g_pcsAgcmWorld			 = NULL;

AgpmAdmin*		g_pcsAgpmAdmin = NULL;	// 2003. 10. 03 steeple
AgcmAdmin*		g_pcsAgcmAdmin = NULL;	// 2003. 10. 03 steeple

AgpmQuest*				g_pcsAgpmQuest = NULL;
AgcmQuest*				g_pcsAgcmQuest = NULL;
AgpmBuddy*				g_pcsAgpmBuddy = NULL;
AgpmChannel*			g_pcsAgpmChannel = NULL;

AgcmUILogin*			g_pcsAgcmUILogin = NULL;
AgcmUIChatting2*		g_pcsAgcmUIChatting2 = NULL;

AgcmUICharacter*		g_pcsAgcmUICharacter	 = NULL;
AgcmUICooldown*			g_pcsAgcmUICooldown		= NULL;
AgcmUIAuction*			g_pcsAgcmUIAuction		= NULL;
AgcmUIItem*				g_pcsAgcmUIItem		 = NULL;
AgcmUISkill2*			g_pcsAgcmUISkill2		 = NULL;
AgcmUIEventItemRepair*	g_pcsAgcmUIEventItemRepair		 = NULL;
AgcmUIEventTeleport*	g_pcsAgcmUIEventTeleport	 = NULL;
AgcmUIParty*			g_pcsAgcmUIParty = NULL;
AgcmUIPartyOption*		g_pcsAgcmUIPartyOption = NULL;
AgcmUIEventBank*		g_pcsAgcmUIEventBank	 = NULL;
AgcmUIEventNPCDialog*	g_pcsAgcmUIEventNPCDialog = NULL;
AgcmUIEventItemConvert*	g_pcsAgcmUIEventItemConvert	= NULL;
AgcmUIGuild*			g_pcsAgcmUIGuild = NULL;
AgcmUISplitItem*		g_pcsAgcmUISplitItem = NULL;
AgcmUIQuest2*			g_pcsAgcmUIQuest2 = NULL;
AgcmUIBuddy*			g_pcsAgcmUIBuddy = NULL;

AgcmUIChannel*			g_pcsAgcmUIChannel = NULL;

AgcmMinimap*			g_pcsAgcmMinimap		 = NULL;

AgcmShadow*				g_pcsAgcmShadow	 = NULL;
AgcmShadow2*			g_pcsAgcmShadow2 = NULL;
AgcmUIOption*			g_pcsAgcmUIOption = NULL;

ApmOcTree*				g_pcsApmOcTree = NULL;
AgcmOcTree*				g_pcsAgcmOcTree = NULL;

AgpmUIStatus*			g_pcsAgpmUIStatus	= NULL;
AgcmUIStatus*			g_pcsAgcmUIStatus	= NULL;

AgcmGrass*				g_pcsAgcmGrass = NULL;

AgpmGuild*				g_pcsAgpmGuild = NULL;
AgcmGuild*				g_pcsAgcmGuild = NULL;

AgpmProduct*			g_pcsAgpmProduct = NULL;
AgcmProduct*			g_pcsAgcmProduct = NULL;
AgcmUIProductSkill*		g_pcsAgcmUIProductSkill = NULL;
AgcmUIProduct*			g_pcsAgcmUIProduct = NULL;
AgcmProductMed*			g_pcsAgcmProductMed = NULL;

AgpmRefinery*			g_pcsAgpmRefinery = NULL;
AgcmRefinery*			g_pcsAgcmRefinery = NULL;
AgpmEventRefinery*		g_pcsAgpmEventRefinery = NULL;
AgcmEventRefinery*		g_pcsAgcmEventRefinery = NULL;
AgcmUIRefinery*			g_pcsAgcmUIRefinery = NULL;

AgpmSummons*			g_pcsAgpmSummons = NULL;
AgpmPvP*				g_pcsAgpmPvP = NULL;
AgcmUIPvP*				g_pcsAgcmUIPvP = NULL;

AgpmRemission*			g_pcsAgpmRemission = NULL;
AgcmUIRemission*		g_pcsAgcmUIRemission = NULL;

AgcmUVAnimation*		g_pcsAgcmUVAnimation = NULL;

AgcmDynamicLightmap*	g_pcsAgcmDynamicLightmap = NULL;

AgcmShadowmap*			g_pcsAgcmShadowmap = NULL;

AgcmPostFX*				g_pcsAgcmPostFX = NULL;

AgcmCustomizeRender*	g_pcsAgcmCustomizeRender = NULL;

AgpmOptimizedPacket2*	g_pcsAgpmOptimizedPacket2	= NULL;

AgcmUIDebugInfo*		g_pcsAgcmUIDebugInfo	= NULL;
AgpmNatureEffect*		g_pcsAgpmNatureEffect	= NULL;
AgcmNatureEffect*		g_pcsAgcmNatureEffect	= NULL;

AgpmAreaChatting*		g_pcsAgpmAreaChatting	= NULL;

AgcmUIConsole*			g_pcsAgcmUIConsole = NULL;

AgpmEventCharCustomize	*g_pcsAgpmEventCharCustomize	= NULL;
AgcmEventCharCustomize	*g_pcsAgcmEventCharCustomize	= NULL;
AgcmUICustomizing		*g_pcsAgcmUICustomizing	= NULL;

AgcmUISystemMessage		*g_pcsAgcmUISystemMessage = NULL;

AgcmUITips				*g_pcsAgcmUITips		= NULL;
AgcmUIHelp				*g_pcsAgcmUIHelp		= NULL;

AgpmBillInfo			*g_pcsAgpmBillInfo		= NULL;
AgcmBillInfo			*g_pcsAgcmBillInfo		= NULL;

AgpmPathFind			*g_pcsAgpmPathFind = NULL;

AgpmSystemMessage		*g_pcsAgpmSystemMessage = NULL;
AgcmAmbOcclMap			*g_pcsAgcmAmbOcclMap = NULL;

AgpmEventPointLight		*g_pcsAgpmEventPointLight = NULL;
AgcmEventPointLight		*g_pcsAgcmEventPointLight = NULL;

AgpmScript				*g_pcsAgpmScript = NULL;

AgpmCashMall			*g_pcsAgpmCashMall		= NULL;
AgcmCashMall			*g_pcsAgcmCashMall		= NULL;
AgcmUICashMall			*g_pcsAgcmUICashMall	= NULL;
AgcmUICashInven			*g_pcsAgcmUICashInven	= NULL;

AgpmMailBox				*g_pcsAgpmMailBox		= NULL;
AgcmUIMailBox			*g_pcsAgcmUIMailBox		= NULL;

AgpmReturnToLogin		*g_pcsAgpmReturnToLogin	= NULL;
AgcmReturnToLogin		*g_pcsAgcmReturnToLogin	= NULL;

AgpmWantedCriminal		*g_pcsAgpmWantedCriminal = NULL;
AgcmUIWantedCriminal	*g_pcsAgcmUIWantedCriminal = NULL;

AgpmStartupEncryption	*g_pcsAgpmStartupEncryption = NULL;
AgcmStartupEncryption	*g_pcsAgcmStartupEncryption = NULL;

AgpmSiegeWar			*g_pcsAgpmSiegeWar		= NULL;
AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC	= NULL;
AgcmSiegeWar			*g_pcsAgcmSiegeWar		= NULL;
AgcmEventSiegeWarNPC	*g_pcsAgcmEventSiegeWarNPC	= NULL;
AgcmUISiegeWar			*g_pcsAgcmUISiegeWar	= NULL;

AgpmGuildWarehouse		*g_pcsAgpmGuildWarehouse = NULL;
AgcmUIGuildWarehouse	*g_pcsAgcmUIGuildWarehouse = NULL;

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//AgpmGamble				*g_pcsAgpmGamble = NULL;
//AgcmUIGamble			*g_pcsAgcmUIGamble = NULL;

AgcmUISearch			*g_pcsAgcmUISearch		= NULL;
AgpmSearch				*g_pcsAgpmSearch		= NULL;

AgcmUIHotkey			*g_pcsAgcmUIHotkey = NULL;

AgcmUISocialAction		*g_pcsAgcmUISocialAction = NULL;


AgpmTax					*g_pcsAgpmTax = NULL;
AgcmUITax				*g_pcsAgcmUITax = NULL;

AgpmCasper				*g_pcsAgpmCasper	= NULL;

AgpmArchlord			*g_pcsAgpmArchlord = NULL;
AgcmArchlord			*g_pcsAgcmArchlord = NULL;
AgcmUIArchlord			*g_pcsAgcmUIArchlord = NULL;

AgpmEventGacha			*g_pcsAgpmEventGacha		= NULL;
AgcmEventGacha			*g_pcsAgcmEventGacha		= NULL;
AgcmUIEventGacha		*g_pcsAgcmUIEventGacha		= NULL;

AgpmBattleGround		*g_pcsAgpmBattleGround	= NULL;
AgcmBattleGround		*g_pcsAgcmBattleGround	= NULL;

AgpmTitle				*g_pcsAgpmTitle = NULL;

// exe, dat
void DeleteAndRenameFile(char* oldone, char* newone)
{
	if ( 0 == ::DeleteFile( oldone ) )
	{
		DWORD dwAttr = GetFileAttributes(oldone);
		
		if(dwAttr & FILE_ATTRIBUTE_READONLY)
		{
			::SetFileAttributes(oldone, dwAttr & ~FILE_ATTRIBUTE_READONLY);
			::DeleteFile( oldone );
		}
		else
		{
			Sleep(500);

			if ( 0 == ::DeleteFile( oldone ) )
			{
				Sleep(500);
				if ( 0 == ::DeleteFile( oldone ) ) {
					Sleep(500);
					::DeleteFile( oldone );
				}
			}
		}
	}

	if ( 0 != ::rename(newone, oldone) )
	{
		Sleep(0);
		::rename(newone, oldone);
	}
}


BOOL CALLBACK FindAutoProgram( HWND hWnd, LPARAM lParam )
{
	CHAR*	 szName	= (CHAR *) lParam;
	GetWindowText( hWnd, szName, 127 );
	if( strstr(szName, "큐마우스") || strstr(szName, "qmouse") || strstr(szName, "아크텀") || strstr(szName, "ArcTerm") )
		return FALSE;

	return TRUE;
}

BOOL	SearchAutoProgram(CHAR* szWindowName)
{
	return EnumWindows( FindAutoProgram, (LPARAM)szWindowName ) ? FALSE : TRUE;
}

BOOL	KillAutoPrograms()
{
	CHAR	szName[128];
	memset( szName, 0, sizeof(char) * 128 );

	if ( SearchAutoProgram( szName ) )
	{
		char szText[256];
		sprintf( szText, "비정상적인 프로그램 (%s)을 감지하여 게임을 종료합니다.", szName );
		g_pEngine->MessageDialog( NULL, szText );
		RsEventHandler(rsQUITAPP, NULL);
	}

	return TRUE;
}

BOOL	__AgcmRender_DebugInfo_PreRenderCallback( PVOID pData, PVOID pClass, PVOID pCustData )
{
	g_cMyEngine.m_pMainWindow->DrawGeometryGrid();
	return TRUE;
}

MyEngine::MyEngine() : CMyException( this )
{
	m_bEmulationFullscreen =  g_eServiceArea == AP_SERVICE_AREA_CHINA ? TRUE : FALSE;	//중국은 IME때문에 강제 풀스크린

	// 디버그메시지가 너무 많아서 필요할때 켜서 보도록 합니다. by 마고자
	g_bUseTrace = FALSE;

	PatchPatcher();
	SetupLanguage();
	KillAutoPrograms();

	m_pMainWindow	= new MainWindow( *this );
	m_hCursor		= NULL;
	m_szLoginServer = NULL;

	// Lua Glue 펑션 등록..
	AuLua * pLua = AuLua::GetPointer();
	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction( MVLuaGlue[i].name, MVLuaGlue[i].func );
	}

	#ifdef _USE_HACKSHIELD_
	// 마고자 (2005-04-20 오전 11:23:20) : 
	// HackShield 관련 코드 주석 
	int nResult;
	nResult = _AhnHS_Initialize(
		"EHSvc.dll",
		AhnHS_Callback,
		2901,
		"46300B2E4F216FE0",
		AHNHS_CHKOPT_SPEEDHACK
		//| AHNHS_CHKOPT_READWRITEPROCESSMEMORY
		//| AHNHS_CHKOPT_KDTARCER
		//| AHNHS_CHKOPT_OPENPROCESS
		| AHNHS_CHKOPT_AUTOMOUSE
		| AHNHS_CHKOPT_MESSAGEHOOK
		//| AHNHS_CHKOPT_PROCESSSCAN
		//| AHNHS_CHKOPT_HSMODULE_CHANGE
		//| AHNHS_USE_LOG_FILE
		,
		AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL );

	switch( nResult )
	{
	case HS_ERR_OK:
		// no error

		break;
	default:
		// Error!
		::MessageBox( NULL , "HShield 초기화 과정에서 문제가 발생했습니다. 클라이언트를 강제 종료합니다." , "Archlord" ,  MB_ICONWARNING );
		ExitProcess( 1 );
		break;
	}

	_AhnHS_StartService	();
	#endif// _USE_HACKSHIELD_

#ifdef _AREA_JAPAN_
	g_hgpshelp = LoadLibrary(_T("HGPSHELP.DLL"));
#endif
	PostInit();
}

MyEngine::~MyEngine()
{
	if( m_pMainWindow )
		delete m_pMainWindow;
	if( g_pcsAgcmUIOption )
		delete g_pcsAgcmUIOption;
#ifdef _AREA_JAPAN_
	if(g_hgpshelp)
		FreeLibrary(g_hgpshelp);
#endif
}

BOOL		MyEngine::OnAttachPlugins	()	// 플러그인 연결 은 여기서
{
    if( !RpWorldPluginAttach() )		return FALSE;
	if( !RpMatFXPluginAttach() )		return FALSE;
	if( !RpHAnimPluginAttach() )		return FALSE;
	if( !RpSkinPluginAttach() )			return FALSE;
    if( !RpCollisionPluginAttach() )	return FALSE;
	if( !RpSplinePluginAttach() )		return FALSE;
	if( !RpMorphPluginAttach() )		return FALSE;
	if( !RpMTexturePluginAttach() )		return FALSE;
	if( !RpUserDataPluginAttach() )		return FALSE;
	if( !RpLODAtomicPluginAttach() )	return FALSE;
	if( !RpUVAnimPluginAttach() )		return FALSE;
	if( !RpUVAnimDataPluginAttach() )	return FALSE;
	if( !RpMatFxExPluginAttach() )		return FALSE;
	if( !RpMaterialD3DFxPluginAttach() )return FALSE;
	if( !RpRandomPluginAttach() )		return FALSE;
	if( !RpAmbOcclMapPluginAttach() )	return FALSE;
	if( !RpPVSPluginAttach() )			return FALSE;
	if( !RpGeoTriAreaTblPluginAttach() )return FALSE;
	if( !RtAnimInitialize() )			return FALSE;
	if( !RpToonPluginAttach() )			return FALSE;

#ifdef _DPVS
	if( !RpDPVSPluginAttach() )			return FALSE;
#endif //_DPVS

	return TRUE;
}

RpWorld *	MyEngine::OnCreateWorld()
{
	SetupOption();

	AgcdUIOptionTexture	eTQCharacter	= g_pcsAgcmUIOption->GetTextureQualityCharacter();
	AgcdUIOptionTexture	eTQObject		= g_pcsAgcmUIOption->GetTextureQualityObject();
	AgcdUIOptionTexture	eTQEffect		= g_pcsAgcmUIOption->GetTextureQualityEffect();
	AgcdUIOptionTexture	eTQWorld		= g_pcsAgcmUIOption->GetTextureQualityWorld();

	CHAR	szTemp[256];
	sprintf(szTemp, "Texture\\Effect\\%s;Texture\\UI\\;Texture\\Character\\%s;Texture\\Object\\%s;Texture\\World\\%s;Texture\\UI\\Base\\;Texture\\SKILL\\;Texture\\Item\\;Texture\\Etc\\;Texture\\Minimap\\;Texture\\NotPacked\\;Texture\\AmbOccl\\",
			eTQEffect == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQEffect == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"),
			eTQCharacter == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQCharacter == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"),
			eTQObject == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQObject == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"),
			eTQWorld == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQWorld == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"));

	RwChar* path = RsPathnameCreate( RWSTRING( szTemp ) );
    RwImageSetPath( path );
    RsPathnameDestroy( path );

	RpMaterialD3DFxSetSearchPath( "Effect\\" );
	return AGCMMAP_THIS->GetWorld();
}

RwCamera *	MyEngine::OnCreateCamera( RpWorld * pWorld )
{
	m_fFarClipPlane = 50000.0f;
	m_fNearClipPlane = 100.0f;

	return CreateCamera( pWorld );
}

BOOL		MyEngine::OnCreateLight		()	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
{
	ASSERT( m_pWorld );

	m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
	if( m_pLightAmbient )
	{
		float	ambiantvalue		= 0.0f;//0.4f;
		float	directionalvalue	= 0.5f;

		RwRGBAReal color = {ambiantvalue, ambiantvalue, ambiantvalue, 1.0f};

		RpLightSetColor(m_pLightAmbient, &color);
		RpLightSetFlags(m_pLightAmbient, rpLIGHTLIGHTATOMICS | rpLIGHTLIGHTWORLD);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
	}

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);
    if( m_pLightDirect )
	{
		RwFrame *pstFrame = RwFrameCreate();
		if( !pstFrame )
		{
	        RsErrorMessage( RWSTRING( "Cannot create light frame." ) );
			return FALSE;
		}

		RpLightSetFrame( m_pLightDirect , pstFrame );

		RwV3d v = { 0.0f , 0.0f , 0.0f };
		RwFrameTranslate( pstFrame, &v, rwCOMBINEREPLACE );

		RwMatrix* pMatrix = RwFrameGetLTM( pstFrame	);
		RwV3d* pAt = RwMatrixGetAt( pMatrix );
		pAt->x	= 0.556f;
		pAt->y	= -0.658f;
		pAt->z	= 0.056f;

		RwMatrixOrthoNormalize( pMatrix,pMatrix );

		RwRGBAReal color = {(240.0f/255.0f), (202.0f/255.0f), (116.0f/255.0f), 1.0f };
		RpLightSetColor( m_pLightDirect, &color );
		RpLightSetFlags( m_pLightDirect, rpLIGHTLIGHTATOMICS | rpLIGHTLIGHTWORLD );

		RpWorldAddLight( m_pWorld, m_pLightDirect );
	}

	return TRUE;
}

BOOL MyEngine::OnRegisterModule()
{
	_set_SSE2_enable(1);

#ifdef _AREA_KOREA_
	if ( !npgl.Init() )
	{
		MessageBox( NULL, "GameGuard Initialize Failed.", "GameGuard Error", MB_OK );

		// 강제 종료..
		::ExitProcess( 1 );
	}

	npgl.SetWndHandle( g_pEngine->GethWnd() );
#endif

#ifdef _AREA_JAPAN_
	DWORD dwResult = npgl.Init( g_eServiceArea );
	if ( NPGAMEMON_SUCCESS != dwResult )
	{
		char msg[256];
		_snprintf_s(msg, 256, _TRUNCATE, "GameGuard Error: %lu", dwResult);
		MessageBox(NULL, npgl.GetNPMessage(dwResult), msg, MB_OK);
		
		// 강제 종료..
		::ExitProcess( 1 );
	}

	npgl.SetWndHandle( g_pEngine->GethWnd() );
#endif

#ifdef _AREA_GLOBAL_
	//DWORD dwResult = npgl.Init( g_eServiceArea );
	//if ( NPGAMEMON_SUCCESS != dwResult )
	//{
	//	char msg[256];
	//	_snprintf_s(msg, 256, _TRUNCATE, "GameGuard Error: %lu", dwResult);
	//	MessageBox(NULL, npgl.GetNPMessage(dwResult), msg, MB_OK);
	//	
	//	// 강제 종료..
	//	::ExitProcess( 1 );
	//}

	//npgl.SetWndHandle( g_pEngine->GethWnd() );
#endif

	g_AuCircularBuffer.Init(2048 * 1024);
	g_AuCircularOutBuffer.Init(100 * 1024);
	
	RemoveDebugLogFile();	// 이전 실행한 후 남은 디버그 파일이 있으면 지운다.
	SetMaxConnection(10, 2048000);	// Client Socket의 연결 가능한 최대 갯수를 설정한다.

	REGISTER_MODULE( g_pcsAgpmConfig		, AgpmConfig			)
	REGISTER_MODULE( g_pcsAgcmResourceLoader, AgcmResourceLoader	)
	REGISTER_MODULE( g_pcsAgpmResourceInfo	, AgpmResourceInfo		)
	REGISTER_MODULE( g_pcsApmMap			, ApmMap				)
	REGISTER_MODULE( g_pcsApmOcTree			, ApmOcTree				)
	
	if (g_pcsApmOcTree)	g_pcsApmOcTree->m_bOcTreeEnable = TRUE;			// 다른 곳의 onAddModule에서 참고할수 있다..
	
	REGISTER_MODULE( g_pcsAgcmOcTree		, AgcmOcTree			)
	REGISTER_MODULE( g_pcsAgcmPostFX		, AgcmPostFX			)
	REGISTER_MODULE( g_pcsAgcmCustomizeRender, AgcmCustomizeRender	)
	REGISTER_MODULE( g_pcsAgcmRender		, AgcmRender			)
	REGISTER_MODULE( g_pcsAgcmLODManager	, AgcmLODManager		)
	REGISTER_MODULE( g_pcsAgcmFont			, AgcmFont				)
	REGISTER_MODULE( g_pcsAgcmSound			, AgcmSound				)
	REGISTER_MODULE( g_pcsAgpmFactors		, AgpmFactors			)
	REGISTER_MODULE_SINGLETON( AgcmMap )
	REGISTER_MODULE( g_pcsAgcmShadow		, AgcmShadow			)
	REGISTER_MODULE( g_pcsAgcmShadow2		, AgcmShadow2			)
	REGISTER_MODULE( g_pcsAgcmDynamicLightmap, AgcmDynamicLightmap	)
	REGISTER_MODULE( g_pcsAgcmShadowmap		, AgcmShadowmap	)
	REGISTER_MODULE( g_pcsApmObject			, ApmObject				)
	REGISTER_MODULE( g_pcsAgcmObject		, AgcmObject			)
	REGISTER_MODULE( g_pcsAgcmAmbOcclMap	, AgcmAmbOcclMap		)
	REGISTER_MODULE( g_pcsAgpmCharacter		, AgpmCharacter			)
	REGISTER_MODULE( g_pcsAgcmConnectManager, AgcmConnectManager	)
	REGISTER_MODULE( g_pcsAgpmScript		, AgpmScript			)
	REGISTER_MODULE( g_pcsAgpmBillInfo		, AgpmBillInfo			)
	REGISTER_MODULE( g_pcsAgpmGrid			, AgpmGrid				)
	REGISTER_MODULE( g_pcsAgpmItem			, AgpmItem				)
	REGISTER_MODULE( g_pcsAgpmUnion			, AgpmUnion				)
	REGISTER_MODULE( g_pcsAgpmParty			, AgpmParty				)
	REGISTER_MODULE( g_pcsApmEventManager	, ApmEventManager		)
	REGISTER_MODULE( g_pcsAgpmSummons		, AgpmSummons			)
	REGISTER_MODULE( g_pcsAgpmSkill			, AgpmSkill				)
	REGISTER_MODULE( g_pcsAgpmItemConvert	, AgpmItemConvert		)
	REGISTER_MODULE( g_pcsAgpmGuild, AgpmGuild		)
	REGISTER_MODULE( g_pcsAgpmPvP			, AgpmPvP				)
	REGISTER_MODULE( g_pcsAgpmUIStatus		, AgpmUIStatus			)
	REGISTER_MODULE( g_pcsAgpmOptimizedPacket2	, AgpmOptimizedPacket2	)
	REGISTER_MODULE( g_pcsAgpmSystemMessage, AgpmSystemMessage )
	REGISTER_MODULE( g_pcsAgpmReturnToLogin	, AgpmReturnToLogin		)
	REGISTER_MODULE( g_pcsAgpmArchlord,	AgpmArchlord )
	REGISTER_MODULE( g_pcsAgpmPathFind		, AgpmPathFind			)
	REGISTER_MODULE( g_pcsAgcmCharacter		, AgcmCharacter			)
	REGISTER_MODULE( g_pcsAgcmCamera2		, AgcmCamera2			)
	REGISTER_MODULE( g_pcsAgpmLogin			, AgpmLogin				)
	REGISTER_MODULE( g_pcsAgcmLogin			, AgcmLogin				)
	REGISTER_MODULE( g_pcsAgcmUIControl		, AgcmUIControl			)
	REGISTER_MODULE( g_pcsAgcmUIManager2	, AgcmUIManager2		)
	REGISTER_MODULE( g_pcsAgcmUICooldown	, AgcmUICooldown		)
	REGISTER_MODULE( g_pcsAgpmAuctionCategory, AgpmAuctionCategory	)
	REGISTER_MODULE( g_pcsAgpmAuction		, AgpmAuction			)
	REGISTER_MODULE( g_pcsAgpmRide			, AgpmRide				)
	REGISTER_MODULE( g_pcsAgpmProduct, AgpmProduct	)
	REGISTER_MODULE( g_pcsAgpmRefinery		, AgpmRefinery			)
	REGISTER_MODULE( g_pcsAgpmCombat		, AgpmCombat			)
	REGISTER_MODULE( g_pcsAgpmTimer			, AgpmTimer				)
	REGISTER_MODULE( g_pcsAgpmCashMall		, AgpmCashMall			)
	REGISTER_MODULE( g_pcsAgpmAdmin			, AgpmAdmin				)
	REGISTER_MODULE( g_pcsAgpmSiegeWar, AgpmSiegeWar )
	REGISTER_MODULE( g_pcsAgpmEventSiegeWarNPC, AgpmEventSiegeWarNPC )
	REGISTER_MODULE( g_pcsAgpmTax, AgpmTax)
	REGISTER_MODULE( g_pcsAgpmGuildWarehouse, AgpmGuildWarehouse)

	//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
	//if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	//	REGISTER_MODULE( g_pcsAgpmGamble, AgpmGamble)
	
	REGISTER_MODULE( g_pcsAgpmNatureEffect	, AgpmNatureEffect		)
	REGISTER_MODULE( g_pcsAgpmEventNature	, AgpmEventNature		)
	REGISTER_MODULE( g_pcsAgpmEventNPCDialog, AgpmEventNPCDialog	)
	REGISTER_MODULE( g_pcsAgpmEventNPCTrade	, AgpmEventNPCTrade		)
	REGISTER_MODULE( g_pcsAgpmEventItemRepair, AgpmEventItemRepair	)
	REGISTER_MODULE( g_pcsAgpmEventBank		, AgpmEventBank			)
	REGISTER_MODULE( g_pcsAgpmEventItemConvert,	AgpmEventItemConvert	)
	REGISTER_MODULE( g_pcsAgpmEventGuild, AgpmEventGuild		)
	REGISTER_MODULE( g_pcsAgpmEventSkillMaster	,	AgpmEventSkillMaster	)
	REGISTER_MODULE( g_pcsAgpmEventProduct, AgpmEventProduct	)
	REGISTER_MODULE( g_pcsAgpmEventRefinery	, AgpmEventRefinery		)
	REGISTER_MODULE( g_pcsAgpmEventQuest, AgpmEventQuest )
	REGISTER_MODULE( g_pcsAgpmEventTeleport	, AgpmEventTeleport		)
	REGISTER_MODULE( g_pcsAgpmChatting		, AgpmChatting			)
	REGISTER_MODULE( g_pcsAgpmAreaChatting	, AgpmAreaChatting		)
	REGISTER_MODULE( g_pcsAgcmItem			, AgcmItem				)
	REGISTER_MODULE( g_pcsAgcmItemConvert	, AgcmItemConvert		)
	REGISTER_MODULE( g_pcsAgcmRecruit		, AgcmRecruit			)
	REGISTER_MODULE( g_pcsAgcmPrivateTrade	, AgcmPrivateTrade		)
	REGISTER_MODULE( g_pcsAgcmParty			, AgcmParty				)
	REGISTER_MODULE( g_pcsAgcmRide			, AgcmRide				)
	REGISTER_MODULE( g_pcsAgcmProduct		, AgcmProduct			)
	REGISTER_MODULE( g_pcsAgcmRefinery		, AgcmRefinery			)
	REGISTER_MODULE( g_pcsAgcmSkill			, AgcmSkill				)
	REGISTER_MODULE( g_pcsAgcmGlyph			, AgcmGlyph				)
	REGISTER_MODULE( g_pcsAgcmCashMall		, AgcmCashMall			)
	REGISTER_MODULE( g_pcsAgcmMinimap		, AgcmMinimap			)
	REGISTER_MODULE( g_pcsAgcmSiegeWar, AgcmSiegeWar )
	REGISTER_MODULE( g_pcsAgcmNatureEffect	, AgcmNatureEffect		)
	REGISTER_MODULE( g_pcsAgcmEventManager	, AgcmEventManager		)
	REGISTER_MODULE( g_pcsAgcmArchlord		, AgcmArchlord			)
	REGISTER_MODULE( g_pcsAgcmAuction		, AgcmAuction			)
	REGISTER_MODULE( g_pcsAgcmEventItemRepair, AgcmEventItemRepair	)
	REGISTER_MODULE( g_pcsAgcmEventBank		, AgcmEventBank			)
	REGISTER_MODULE( g_pcsAgcmEventItemConvert,	AgcmEventItemConvert	)
	REGISTER_MODULE( g_pcsAgcmEventGuild, AgcmEventGuild		)
	REGISTER_MODULE( g_pcsAgcmEventSiegeWarNPC, AgcmEventSiegeWarNPC )
	REGISTER_MODULE( g_pcsAgcmEventSkillMaster	,	AgcmEventSkillMaster	)
	REGISTER_MODULE( g_pcsAgcmEventProduct, AgcmEventProduct	)
	REGISTER_MODULE( g_pcsAgcmEventRefinery	, AgcmEventRefinery		)
	REGISTER_MODULE( g_pcsAgcmEventQuest, AgcmEventQuest )
	REGISTER_MODULE( g_pcsAgcmBillInfo	, AgcmBillInfo	)
	REGISTER_MODULE( g_pcsAgcmEventTeleport	, AgcmEventTeleport		)
	REGISTER_MODULE( g_pcsAgcmEventNPCDialog, AgcmEventNPCDialog	)
	REGISTER_MODULE( g_pcsAgcmEventNPCTrade	, AgcmEventNPCTrade		)
	REGISTER_MODULE( g_pcsAgcmUVAnimation	, AgcmUVAnimation		)
	REGISTER_MODULE( g_pcsAgcmEff2			, AgcmEff2				)
	REGISTER_MODULE( g_pcsAgcmEventEffect	, AgcmEventEffect		)
	REGISTER_MODULE( g_pcsAgcmEventNature	, AgcmEventNature		)
	REGISTER_MODULE( g_pcsAgcmTextBoardMng	, AgcmTextBoardMng		)
	REGISTER_MODULE( g_pcsAgcmGuild			, AgcmGuild				)
	REGISTER_MODULE( g_pcsAgcmChatting2		, AgcmChatting2			)
	REGISTER_MODULE( g_pcsAgcmLensFlare		, AgcmLensFlare			)
	REGISTER_MODULE( g_pcsAgcmGrass			, AgcmGrass				)
	REGISTER_MODULE( g_pcsAgpmWorld, AgpmWorld		)
	REGISTER_MODULE( g_pcsAgcmWorld, AgcmWorld		)
	REGISTER_MODULE( g_pcsAgpmQuest, AgpmQuest		)
	REGISTER_MODULE( g_pcsAgcmQuest, AgcmQuest		)
	REGISTER_MODULE( g_pcsAgpmBuddy, AgpmBuddy		)
	REGISTER_MODULE( g_pcsAgpmChannel, AgpmChannel		)
	REGISTER_MODULE( g_pcsAgpmStartupEncryption, AgpmStartupEncryption	)
	REGISTER_MODULE( g_pcsAgcmReturnToLogin	, AgcmReturnToLogin		)
	REGISTER_MODULE( g_pcsAgcmUIMain		, AgcmUIMain			)
	REGISTER_MODULE( g_pcsAgcmUICharacter	, AgcmUICharacter		)
	REGISTER_MODULE( g_pcsAgcmUISplitItem	, AgcmUISplitItem		)
	REGISTER_MODULE( g_pcsAgcmUIAuction		, AgcmUIAuction			)
	REGISTER_MODULE( g_pcsAgcmUIItem		, AgcmUIItem			)
	REGISTER_MODULE( g_pcsAgcmUICashInven	, AgcmUICashInven		)
	REGISTER_MODULE( g_pcsAgcmUISkill2		, AgcmUISkill2			)
	REGISTER_MODULE( g_pcsAgcmUIChatting2	, AgcmUIChatting2		)
	REGISTER_MODULE( g_pcsAgcmUIEventItemRepair, AgcmUIEventItemRepair)
	REGISTER_MODULE( g_pcsAgcmUIEventTeleport, AgcmUIEventTeleport	)
	REGISTER_MODULE( g_pcsAgcmUIParty		, AgcmUIParty			)
	REGISTER_MODULE( g_pcsAgcmUIPartyOption	, AgcmUIPartyOption		)
	REGISTER_MODULE( g_pcsAgcmUIEventBank	, AgcmUIEventBank		)
	REGISTER_MODULE( g_pcsAgcmUIEventNPCDialog, AgcmUIEventNPCDialog)
	REGISTER_MODULE( g_pcsAgcmUIEventItemConvert,	AgcmUIEventItemConvert	)
	REGISTER_MODULE( g_pcsAgcmUIGuild,	AgcmUIGuild	)
	REGISTER_MODULE( g_pcsAgcmUIQuest2, AgcmUIQuest2 )
	REGISTER_MODULE( g_pcsAgcmUIBuddy, AgcmUIBuddy )
	REGISTER_MODULE( g_pcsAgcmUIChannel, AgcmUIChannel )
	REGISTER_MODULE( g_pcsAgcmUIPvP			, AgcmUIPvP				)
	REGISTER_MODULE( g_pcsAgcmUIProductSkill, AgcmUIProductSkill)
	REGISTER_MODULE( g_pcsAgcmUIProduct, AgcmUIProduct)
	REGISTER_MODULE( g_pcsAgcmProductMed, AgcmProductMed)
	REGISTER_MODULE( g_pcsAgcmUIRefinery	, AgcmUIRefinery		)
	REGISTER_MODULE( g_pcsAgpmRemission		, AgpmRemission			)
	REGISTER_MODULE( g_pcsAgcmUIRemission	, AgcmUIRemission		)
	REGISTER_MODULE( g_pcsAgcmUICashMall	, AgcmUICashMall		)
	REGISTER_MODULE( g_pcsAgpmMailBox		, AgpmMailBox			)
	REGISTER_MODULE( g_pcsAgcmUIMailBox		, AgcmUIMailBox			)
	REGISTER_MODULE( g_pcsAgpmWantedCriminal, AgpmWantedCriminal	)
	REGISTER_MODULE( g_pcsAgcmUIWantedCriminal, AgcmUIWantedCriminal)
	REGISTER_MODULE( g_pcsAgcmUIArchlord, AgcmUIArchlord)
	REGISTER_MODULE( g_pcsAgcmUITax			, AgcmUITax				)
	REGISTER_MODULE( g_pcsAgcmUIGuildWarehouse, AgcmUIGuildWarehouse)

	REGISTER_MODULE( g_pcsAgpmSearch, AgpmSearch					)
	REGISTER_MODULE( g_pcsAgcmUISearch, AgcmUISearch				)
	REGISTER_MODULE( g_pcsAgcmWater			, AgcmWater				)
	REGISTER_MODULE( g_pcsAgcmGlare			, AgcmGlare				)
	REGISTER_MODULE( g_pcsAgcmTuner			, AgcmTuner				)
	REGISTER_MODULE( g_pcsAgcmUIStatus		, AgcmUIStatus			)
	REGISTER_MODULE( g_pcsAgcmUIHelp				, AgcmUIHelp					)
	REGISTER_MODULE( g_pcsAgcmUITips				, AgcmUITips					)
	REGISTER_MODULE( g_pcsAgcmUISiegeWar, AgcmUISiegeWar )

	VERIFY( g_pcsAgcmUIOption->AddModule( this ) );

	REGISTER_MODULE( g_pcsAgpmEventCharCustomize	, AgpmEventCharCustomize		)
	REGISTER_MODULE( g_pcsAgpmEventPointLight		, AgpmEventPointLight			)
	REGISTER_MODULE( g_pcsAgcmEventPointLight		, AgcmEventPointLight			)
	REGISTER_MODULE( g_pcsAgcmUILogin		, AgcmUILogin			)
	REGISTER_MODULE( g_pcsAgcmTargeting		, AgcmTargeting			)
	REGISTER_MODULE( g_pcsAgcmUIDebugInfo	, AgcmUIDebugInfo		)

	REGISTER_MODULE( g_pcsAgcmAdmin			, AgcmAdmin				)

	REGISTER_MODULE( g_pcsAgcmUIConsole		, AgcmUIConsole			)
	REGISTER_MODULE( g_pcsAgcmEventCharCustomize	, AgcmEventCharCustomize		)
	REGISTER_MODULE( g_pcsAgcmUICustomizing			, AgcmUICustomizing				)
	REGISTER_MODULE( g_pcsAgcmUISystemMessage		, AgcmUISystemMessage			)
	REGISTER_MODULE( g_pcsAgcmStartupEncryption		, AgcmStartupEncryption			)
	REGISTER_MODULE( g_pcsAgcmUIHotkey, AgcmUIHotkey );
	REGISTER_MODULE( g_pcsAgcmUISocialAction, AgcmUISocialAction );
	REGISTER_MODULE( g_pcsAgpmCasper, AgpmCasper	);
	REGISTER_MODULE( g_pcsAgpmEventGacha, AgpmEventGacha	);
	REGISTER_MODULE( g_pcsAgcmEventGacha, AgcmEventGacha	);
	REGISTER_MODULE( g_pcsAgcmUIEventGacha, AgcmUIEventGacha	);

	REGISTER_MODULE( g_pcsAgpmBattleGround, AgpmBattleGround	);
	REGISTER_MODULE( g_pcsAgcmBattleGround, AgcmBattleGround	);

	REGISTER_MODULE( g_pcsAgpmTitle, AgpmTitle );

	AuPackingManager*	pPackingManager = 	AuPackingManager::MakeInstance();
	
	BOOL	bUsePathSearch = FALSE;
#ifdef _DEBUG
	UINT32	uStart = ::GetTickCount();
#endif

	if( bUsePathSearch )
	{
		// 마고자 (2005-05-02 오후 3:34:36) : 
		// 디렉토리를 리컬시브하게 검사함.
		pPackingManager->LoadReferenceFile(NULL);			// NULL이면 현재 디렉토리를 사용한다..
	}
	else
	{
		// 마고자 (2005-05-02 오후 3:34:47) : 
		// 지정된 디렉토리의 레퍼런스만 읽어들임.

		// 현재 디렉토리 구어냄..
		char	strCurrentFolder[255];
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );

		char	strPath[ 1024 ];

		#define	SETDATADIRECTORY( x )										\
			sprintf( strPath , "%s\\%s" , strCurrentFolder , x );			\
			pPackingManager->LoadReferenceFile( strPath , true , false )

		SETDATADIRECTORY( "character"					);
		SETDATADIRECTORY( "character\\defaulthead\\face");
		SETDATADIRECTORY( "character\\defaulthead\\hair");
		SETDATADIRECTORY( "character\\animation"		);
		SETDATADIRECTORY( "effect\\animation"			);
		SETDATADIRECTORY( "effect\\clump"				);
		SETDATADIRECTORY( "ini\\itemtemplate"			);
		SETDATADIRECTORY( "object"						);
		SETDATADIRECTORY( "object\\animation"			);
		SETDATADIRECTORY( "sound\\ui"					);
		SETDATADIRECTORY( "texture\\character"			);
		SETDATADIRECTORY( "texture\\character\\low"		);
		SETDATADIRECTORY( "texture\\character\\medium"	);
		SETDATADIRECTORY( "texture\\effect"				);
		SETDATADIRECTORY( "texture\\effect\\low"		);
		SETDATADIRECTORY( "texture\\effect\\medium"		);
		SETDATADIRECTORY( "texture\\etc"				);
		SETDATADIRECTORY( "texture\\item"				);
		SETDATADIRECTORY( "texture\\minimap"			);
		SETDATADIRECTORY( "texture\\object"				);
		SETDATADIRECTORY( "texture\\object\\low"		);
		SETDATADIRECTORY( "texture\\object\\medium"		);
		SETDATADIRECTORY( "texture\\skill"				);
		SETDATADIRECTORY( "texture\\ui"					);
		SETDATADIRECTORY( "texture\\ui\\base"			);
		SETDATADIRECTORY( "texture\\ui_type1"			);
		SETDATADIRECTORY( "texture\\ui_type1\\base"		);
		SETDATADIRECTORY( "texture\\world"				);
		SETDATADIRECTORY( "texture\\world\\low"			);
		SETDATADIRECTORY( "texture\\world\\medium"		);
		SETDATADIRECTORY( "texture\\worldmap"			);
		SETDATADIRECTORY( "texture\\worldmap\\type01"	);
		SETDATADIRECTORY( "world\\grass"				);
		SETDATADIRECTORY( "world\\octree"				);
		SETDATADIRECTORY( "world\\water"				);
	}

	#ifdef _DEBUG
	{
		char str[ 256 ];
		sprintf( str , "LoadReferenceFile 작업 : %.1f 초 걸림\n" , ( FLOAT )( ::GetTickCount() - uStart ) / 1000.0f );
		OutputDebugString( str );
	}
	#endif

	pPackingManager->SetFilePointer();

	g_pcsAgcmSound->SetReadFromPack( TRUE );

	if (g_pcsAgcmUIManager2)
	{
		g_pcsAgcmUIManager2->SetMainWindow( m_pMainWindow );
		g_pcsAgcmUIManager2->SetSoundPath( "Sound\\UI\\" );
		g_pcsAgcmUIManager2->SetTexturePath( "Texture\\UI\\" );

		if (g_pcsAgcmUIOption->m_iScreenHeight >= 1200 && g_pcsAgcmUIOption->m_iScreenWidth >= 1600 )
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1600_1200 );
		else if (g_pcsAgcmUIOption->m_iScreenHeight >= 1024 && g_pcsAgcmUIOption->m_iScreenHeight < 1200)
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1280_1024 );
		else
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1024_768 );

		g_pcsAgcmUIManager2->SetMaxUIMessage(1000);
	}

	if (g_pcsAgpmResourceInfo)
	{
		switch( g_eServiceArea )
		{
		case AP_SERVICE_AREA_KOREA :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 12 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 04 );
			}
			break;

		case AP_SERVICE_AREA_WESTERN :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 12 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 02 );
			}
			break;

		case AP_SERVICE_AREA_JAPAN :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 19 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 0 );
			}
			break;

		case AP_SERVICE_AREA_CHINA :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 11 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 02 );
			}
			break;
		}
	}

	if (g_pcsAgcmRender)
	{
		g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame(m_pCamera));		// modify by gemani
		g_pcsAgcmRender->SetWorldCamera( m_pWorld, m_pCamera);
		g_pcsAgcmRender->SetLight( m_pLightAmbient, m_pLightDirect );

		g_pcsAgcmRender->SetCallbackPostRenderOcTreeClear	( __AgcmRender_DebugInfo_PreRenderCallback , this );
	}

	if (g_pcsAgcmWater)
	{
		g_pcsAgcmWater->SetWorldCamera( m_pCamera );
	}

	if (g_pcsApmMap)
	{
		g_pcsApmMap->SetLoadingMode( TRUE, FALSE );
		g_pcsApmMap->SetAutoLoadData( TRUE );

		if( !g_pcsApmMap->Init( NULL ) )
			return FALSE;
	}

	if (AGCMMAP_THIS)
	{
		// 마고자 (2003-12-09 오후 2:51:36) : 콜리전 콜벡추가!
		AGCMMAP_THIS->SetCollisionAtomicCallback( AgcmObject::CBGetCollisionAtomic );
		AGCMMAP_THIS->SetCollisionAtomicCallbackFromAtomic( AgcmObject::CBGetCollisionAtomicFromAtomic );

		AGCMMAP_THIS->SetAutoLoadRange( 8, 6, 2 );

		char strClientMapDataDirectory[ 1024 ];
		GetCurrentDirectory( 1024, strClientMapDataDirectory );
		AGCMMAP_THIS->SetDataPath				( strClientMapDataDirectory );

		AGCMMAP_THIS->InitMapClientModule(m_pWorld);

		AGCMMAP_THIS->SetCamera( m_pCamera );

		AGCMMAP_THIS->SetTexDict( "Texture\\Tiles.txd" );

		if (g_pcsAgcmUIOption->GetTextureQualityWorld() == AGCD_OPTION_TEXTURE_MEDIUM)
			AGCMMAP_THIS->	SetTexturePath			( "Texture\\World\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityWorld() == AGCD_OPTION_TEXTURE_LOW)
			AGCMMAP_THIS->	SetTexturePath			( "Texture\\World\\Low\\" );
		else
			AGCMMAP_THIS->	SetTexturePath			( "Texture\\World\\" );
	}

	if (g_pcsApmObject)
	{
		g_pcsApmObject->		SetMaxObjectTemplate	( 5000							);
		g_pcsApmObject->		SetMaxObject			( 10000							);
		g_pcsApmObject->		SetMaxObjectRemove		( 7000							);
	}

	if (g_pcsAgcmObject)
	{
		g_pcsAgcmObject->		SetClumpPath			( "Object\\"					);
		g_pcsAgcmObject->		SetAnimationPath		( "Object\\Animation\\"		);
		g_pcsAgcmObject->		SetMaxAnimation			( 500							);
		g_pcsAgcmObject->		SetSetupObject			( TRUE			, FALSE			);

		g_pcsAgcmObject->		SetTexDictFile			( "Texture\\Object.txd"		);

		if (g_pcsAgcmUIOption->GetTextureQualityObject() == AGCD_OPTION_TEXTURE_MEDIUM)
			g_pcsAgcmObject->	SetTexturePath			( "Texture\\Object\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityObject() == AGCD_OPTION_TEXTURE_LOW)
			g_pcsAgcmObject->	SetTexturePath			( "Texture\\Object\\Low\\" );
		else
			g_pcsAgcmObject->	SetTexturePath			( "Texture\\Object\\" );

		g_pcsAgcmObject->AddFlags(E_AGCM_OBJECT_FLAGS_EXPORT);
	}

	if (g_pcsAgpmCharacter)
	{
		g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 3000							);
		g_pcsAgpmCharacter->	SetMaxCharacter			( 1500							);
		g_pcsAgpmCharacter->	SetMaxCharacterRemove	( 1000							);
		g_pcsAgpmCharacter->	SetProcessMoveFollowInterval	( 200					);

		g_pcsAgpmCharacter->SetCallbackGameguardAuth( CBGameguardAuth, NULL );
	}

	if (g_pcsAgcmCharacter)
	{
		g_pcsAgcmCharacter->	SetMaxAnimations		( 20000							);

		g_pcsAgcmCharacter->	SetClumpPath			( "Character\\"				);
		g_pcsAgcmCharacter->	SetAnimationPath		( "Character\\Animation\\"	);
		g_pcsAgcmCharacter->	SetSendMoveDelay		( 500							);

		g_pcsAgcmCharacter->	SetMaxIdleEvent			( 500							);

		g_pcsAgcmCharacter->	SetTexDictFile			( "Texture\\Character.txd"	);

		if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_MEDIUM)
			g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_LOW)
			g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\Low\\" );
		else
			g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\" );

		g_pcsAgcmCharacter->AddFlags(E_AGCM_CHAR_FLAGS_EXPORT);
		
		g_pcsAgcmCharacter->SetCallbackSettingCharacterOK(CBGetMyCharacterName, NULL);
	}

	if (g_pcsAgpmItem)
	{
		g_pcsAgpmItem->		SetMaxItem				( 3000							);
		g_pcsAgpmItem->		SetMaxItemRemove		( 3000							);
		g_pcsAgpmItem->		SetCompareTemplate();
	}

	if (g_pcsAgcmItem)
	{
		g_pcsAgcmItem->		SetClumpPath			( "Character\\"				);
		g_pcsAgcmItem->		SetMaxItemClump			( 2000							);

		g_pcsAgcmItem->		SetTexDictFile			( "Texture\\Item.txd"		);

		g_pcsAgcmItem->		SetIconTexturePath		( "Texture\\Item\\"			);

		if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_MEDIUM)
			g_pcsAgcmItem->	SetTexturePath			( "Texture\\Character\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_LOW)
			g_pcsAgcmItem->	SetTexturePath			( "Texture\\Character\\Low\\" );
		else
			g_pcsAgcmItem->	SetTexturePath			( "Texture\\Character\\" );

		g_pcsAgcmItem->AddFlags(E_AGCM_ITEM_FLAGS_EXPORT);
	}

	if (g_pcsAgpmParty)
	{
		g_pcsAgpmParty->		SetMaxParty(1);
	}

	if (g_pcsAgpmSkill)
	{
		g_pcsAgpmSkill->		SetMaxSkill				( 1000							);
		g_pcsAgpmSkill->		SetMaxSkillTemplate		( 3000							);
		g_pcsAgpmSkill->		SetMaxSkillSpecializeTemplate	( 10					);
		g_pcsAgpmSkill->		SetMaxSkillTooltipTemplate	( 2000						);
	}

	if (g_pcsAgcmSkill)
	{
		g_pcsAgcmSkill->		SetMaxIdleEvent			( 500							);
		g_pcsAgcmSkill->		SetIconTexturePath		( "Texture\\Skill\\"			);
	}

	if (g_pcsAgpmChannel)
	{
		g_pcsAgpmChannel->SetMaxChannel(10);
	}

	if (g_pcsAgcmGuild)
	{
		g_pcsAgcmGuild->		SetTexturePath			( "Texture\\UI\\Base\\"		);
	}

	if (g_pcsAgpmProduct)
	{
		g_pcsAgpmProduct->SetMaxSkillFactor(10);
		g_pcsAgpmProduct->SetMaxComposeTemplate(300);
	}
		
	if (g_pcsAgpmRefinery)
	{
		g_pcsAgpmRefinery->SetMaxRefineItem(10000);
	}

	if (g_pcsAgpmAuction)
	{
		g_pcsAgpmAuction->SetMaxSales(10);
	}

	if(g_pcsAgpmGuild)
	{
		g_pcsAgpmGuild->SetMaxGuild(5);		// 1개로는 부족할 지도 몰라서 최대 5개까지
	}

	if (g_pcsAgpmWorld)
	{
		g_pcsAgpmWorld->SetMaxWorld(200);
	}

	if (g_pcsAgcmResourceLoader)
	{
		g_pcsAgcmResourceLoader->SetTexDictPath("Texture\\Object\\");

		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\Etc\\"))
			return FALSE;

		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\UI\\"))
			return FALSE;

		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\UI\\Base\\"))
			return FALSE;
	}

	if( g_pcsAgcmCustomizeRender ) {
		g_pcsAgcmCustomizeRender->SetWorld( m_pWorld );
	}

	if (g_pcsAgcmEff2)
	{
		g_pcsAgcmEff2->SetGlobalVariable( NULL , NULL, m_pWorld, m_pCamera);

#ifdef _DEBUG
		AuAutoFile	fp( "eorb.txt", "rt" );
		if( fp )
		{
			AgcdEffGlobal::bGetInst().bFlagOff(E_GFLAG_USEPACK);
		}
#endif
	}

	if (g_pcsAgpmEventNature)
	{
		g_pcsAgpmEventNature->AddDefaultSkySet();
		g_pcsAgpmEventNature->SetSpeedRate( 5 );
		g_pcsAgpmEventNature->StartTimer();
	}

	if (g_pcsApmEventManager)
	{
		g_pcsApmEventManager->	SetMaxEvent		( 1000		);
	}

	if (g_pcsAgpmEventTeleport)
	{
		g_pcsAgpmEventTeleport->SetMaxTeleportPoint(300);
		g_pcsAgpmEventTeleport->SetMaxTeleportGroup(300);
	}

	if (g_pcsAgcmEventEffect)
	{
		g_pcsAgcmEventEffect->SetSoundDataNum(1000);
	}

	if (g_pcsAgcmGlyph)
	{
		g_pcsAgcmGlyph->SetWorldCamera(m_pCamera);
	}

	if (g_pcsAgcmTargeting)
	{
		g_pcsAgcmTargeting->SetRpWorld(m_pWorld);
		g_pcsAgcmTargeting->SetRwCamera(m_pCamera);
	}

	if (g_pcsAgpmQuest)
	{
		g_pcsAgpmQuest->SetMaxTemplateCount(AGPDQUEST_MAX_QUEST_TEMPLATE * 2);
		g_pcsAgpmQuest->SetMaxGroupCount(AGPDQUEST_MAX_QUEST * 2);
	}
	
	if (g_pcsAgpmMailBox)
	{
		g_pcsAgpmMailBox->SetMaxMail(50);
	}

	if (g_pcsAgcmLogin)
	{
		g_pcsAgcmLogin->SetMainWindow( m_pMainWindow );
		if (m_szLoginServer)
			g_pcsAgcmLogin->SetLoginServer(m_szLoginServer);
	}

	if (g_pcsAgcmUITips)
	{
		g_pcsAgcmUITips->SetTexturePath("Texture\\UI\\Base\\");
	}

	if (g_pcsAgpmCashMall)
		g_pcsAgpmCashMall->SetMaxProduct(200);

	m_cAcuIMDraw.SetWorldCamera(m_pCamera);
	g_pcsAgcmOcTree->SetRpWorld(m_pWorld);

	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( pcmTitle )
	{
		pcmTitle->OnInitialize();
	}

	// Device check후 shader생성
	m_cAcuDeviceCheck.InitMyDevice();

	m_cAcuParticleDraw.Init();
	m_cAcuParticleDraw.SetWorldCamera( m_pCamera );

	RpMTextureEnableLinearFog(TRUE);
	RpMTextureCreateShaders();

	// 모든 모듈이 준비된 시점에서 다시 한번 옵션을 로딩해주자.. 이전 시점은 너무 빨라 모듈이 없다..
	g_pcsAgcmUIOption->InitGameOption();

	return TRUE;
}

BOOL MyEngine::OnPostInitialize()
{
	// 2004.12.7 기준 size(약 12.0mb)
	VERIFY(g_pcsAgpmSkill->InitMemoryPool(0,100,"AgpmSkill(1)"));		// 216 byte * 100 =			  21600
	VERIFY(g_pcsAgpmItem->InitMemoryPool(0,800,"AgpmItem(1)"));			// 1008 byte * 800 =		 806400
	VERIFY(g_pcsApmObject->InitMemoryPool(0,3500,"ApmObject(1)"));		// 1340 byte * 3500 =		4690000
	VERIFY(g_pcsApmMap->InitMemoryPool(0,600,"ApmMap(1)"));				// 836 byte * 2500 =		2090000
	VERIFY(g_pcsAgpmCharacter->InitMemoryPool(0, 1000,"AgpmCharacter(1)"));	// 8296	byte * 300 =	2488800
	
	//. 2006. 2. 14. Nonstopdj
	//. ApmOctree 기준size를 늘림. 32000 -> 44000
	VERIFY(g_pcsApmOcTree->InitMemoryPool(0,44000,"ApmOcTree(1)"));			// 80 byte * 44000(rootlist 여분)
						//			(한개의 rootlist당 가질수 있는 최대 node 는 64..)) =			2560000
	return	TRUE;
}

BOOL MyEngine::OnInit()
{
	m_cAcuFrameMemory.Create();
	m_cAcuFrameMemory.InitManager(1024000);				// 1024 kb 할당

	myfirewall::On();

	m_hCursor = ::LoadCursor( PsGlobal.instance , "CURSOR_M1.CUR" );

#ifdef _AREA_JAPAN_
	char* cmdLine = GetCommandLine();
	if ( false == g_jAuth.Init( cmdLine ) ) return FALSE;

	if ( false == g_jGameInfo.Init( g_jAuth.GetGameString(), g_jAuth.IsRealService() ) )
	{
		if ( false == g_jAuth.IsRealService() )
			MessageBox( NULL, "HanGameInfoInit Fail", "Warning", MB_OK );
	}
#endif

#ifdef _AREA_KOREA_
	char* pCommandLine = ::GetCommandLine();
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( !pWebzenAuth )
	{
#ifdef _DEBUG
		::OutputDebugString( "-- WenzenAuth -- : WebzenAuth instance missing.\n" );
#endif
		return FALSE;
	}

	if( !pWebzenAuth->OnInitialize( pCommandLine ) )
	{
#ifdef _DEBUG
		::OutputDebugString( "-- WenzenAuth -- : WebzenAuth failed to CWebzenAuth::OnInitialize().\n" );
#endif
		return FALSE;
	}
#endif

	return AgcEngine::OnInit();
}

void	MyEngine::OnTerminate()
{
	// 우선 로딩 스레드를 멈추게 한다.
	g_pcsAgcmResourceLoader->OnPreDestroy();

	INT16	nSize;
	PVOID	pvPacket = MakeSPClientExit(&nSize);

	//@{ 2006/07/31 burumal
	AuExcelTxtLibHelper::GetSingletonPtr()->SaveIniFile();
	//@}

#ifdef _AREA_JAPAN_
	// 한게임 지표 관련 기능. 수집한 정보를 서버로 전송한다.
	g_jGameInfo.ExitGame( g_jAuth.GetMemberID() );
#endif // _AREA_JAPAN_

	g_pcsAgcmSound->OnDestroy();

#ifdef _DEBUG
	ApMemoryManager::GetInstance().ReportMemoryInfo();
#endif

	if (pvPacket)
	{
		SendPacket(pvPacket, nSize);

		FreeSystemPacket(pvPacket);
	}

	if (g_pcsAgcmResourceLoader)
		g_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;

	myfirewall::Off();

	m_cAcuFrameMemory.Release();

	m_cAcuParticleDraw.Release();

#ifdef _AP_MEMORY_TRACKER_
	ApMemoryTracker::GetInstance().ReportLeaks(REPORT_MODE_FILE);
	ApMemoryTracker::GetInstance().DestroyInstance();
#endif

	ApMemoryManager::GetInstance().DestroyInstance();

	RsRwTerminate();
	//RwEngineClose();
	//강제 종료 왜 하는거임??
	//이거때문에 모듈에서 잡은 메모리 전부 해제 못하고 종료 되다 뻑남;;
//	::ExitProcess( 0 );
}

VOID	MyEngine::OnEndIdle()
{
	static UINT32	s_ulTick	= GetClockCount();
	UINT32			ulCurTick	= GetClockCount();

	if (ulCurTick - s_ulTick >= 5000)
	{
		KillAutoPrograms();
		s_ulTick = ulCurTick;
	}

	m_cAcuFrameMemory.Clear();
}

VOID MyEngine::OnWorldRender()
{
	m_cAcuParticleDraw.CameraMatrixUpdate();
	g_pcsAgcmRender->Render();
}

BOOL MyEngine::OnRenderPreCameraUpdate()
{
	if (g_pcsAgcmShadow2)
		g_pcsAgcmShadow2->ShadowTextureRender();

	if(g_pcsAgcmPostFX)
		g_pcsAgcmPostFX->changeRenderTarget();

	return TRUE;
}

BOOL MyEngine::OnAddModule()
{
	return TRUE;
}

#define WM_ALEFDETAILCHANGE			(WM_USER + 13 )		// wParam : Rough,Load , lParam : Detail
#define WM_ALEFTIMEFLOWCHANGE		(WM_USER + 14 )		// wParam : value
#define WM_ALEFSKYTEMPLATECHANGE	(WM_USER + 15 )		// wParam : value
#define WM_ALEFPOSITIONWARP			(WM_USER + 17 )

UINT32	MyEngine::OnWindowEventHandler	(HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_ALEFPOSITIONWARP:
		{
			// 해당 좌표로 워프~

			FLOAT	x , z;
			x = * ( ( FLOAT * ) & wParam );
			z = * ( ( FLOAT * ) & lParam );

			char strMessage[ 256 ];
			sprintf( strMessage , "/move %f,%f" , x , z );

			g_pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , strMessage , strlen( strMessage ) , NULL );
		}
		break;
	case WM_DROPFILES:
		{
			HDROP	hDropInfo = ( HDROP ) wParam;
			
			// TODO: Add your message handler code here and/or call default
			char	lpstr[ 1024 ];
			// 우선 몇개의 파일이 드래그 되는 지 확인한다.
			UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpstr , 1024 );

			if( dragcount > 0 )
			{
				// 모두다 검사 하려면
				for ( int i = 0 ; i < ( int )  dragcount ; i ++ )
				{
					::DragQueryFile( hDropInfo , i , lpstr , 1024 );
					TRACE( "드롭된 %d 번째 파일 -'%s'\n" , i + 1 , lpstr );
				}

				// 이 프로그램에서는 제일 처음것만 참고한다.
				::DragQueryFile( hDropInfo , 0 , lpstr , 1024 );
				
				char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
				_splitpath( lpstr , drive, dir, fname, ext );

				// 파일이름 체크..
				int nLen = 0;
				while( nLen < (int)strlen( ext ) )
				{
					ext[ nLen ] = toupper( ext[ nLen ] );
					nLen ++;
				}
			}
		}
		break;

	case WM_SETCURSOR:
		{
			if( g_pcsAgcmUIManager2 && g_pcsAgcmUIManager2->IsShowCursor() )
			{
				HCURSOR hCursor = g_pcsAgcmUIManager2->GetCurrentCursorHandle();
				::SetCursor( hCursor ? hCursor : m_hCursor );
			}
			else
				::SetCursor( NULL );
		}
		return 1;

	default:
		break; 
		
	}

	return 0;
}

BOOL MyEngine::OnCameraResize( RwRect * pRect )
{
	BOOL bOn = FALSE;
	if(g_pcsAgcmPostFX && g_pcsAgcmPostFX->isOn())
	{
		g_pcsAgcmPostFX->Off();
		bOn = TRUE;
	}
		
	BOOL bResult = AgcEngine::OnCameraResize( pRect );

	if(g_pcsAgcmPostFX && bOn)
		g_pcsAgcmPostFX->On();

	return bResult;
}

// 2004.01.15. 김태희
BOOL MyEngine::RemoveDebugLogFile()
{
	// 이전 Debug File Log 가 있으면 지운다.
	char szPath[512];
	if(GetCurrentDirectory(512, szPath) == 0)
		return FALSE;

	strcat(szPath, "\\");
	strcat(szPath, ALEF_ERROR_FILENAME);
	
	if(_access(szPath, 0) != -1)
	{
		if(!DeleteFile(ALEF_ERROR_FILENAME))
		{
			// 읽기 전용이면 풀고 지운다.
			DWORD dwAttr = GetFileAttributes(ALEF_ERROR_FILENAME);
			if(dwAttr & FILE_ATTRIBUTE_READONLY)
			{
				SetFileAttributes(szPath, dwAttr & ~FILE_ATTRIBUTE_READONLY);
				DeleteFile(ALEF_ERROR_FILENAME);
			}
		}
	}

	// Stack Walker Debug 파일도 지운다.
	memset(szPath, 0, 512);
	if(GetCurrentDirectory(512, szPath) == 0)
		return FALSE;

	strcat(szPath, "\\");
	strcat(szPath, ALEF_STACKWALK_FILENAME);
	
	if(_access(szPath, 0) != -1)
	{
		if(!DeleteFile(ALEF_STACKWALK_FILENAME))
		{
			// 읽기 전용이면 풀고 지운다.
			DWORD dwAttr = GetFileAttributes(ALEF_STACKWALK_FILENAME);
			if(dwAttr & FILE_ATTRIBUTE_READONLY)
			{
				SetFileAttributes(szPath, dwAttr & ~FILE_ATTRIBUTE_READONLY);
				DeleteFile(ALEF_STACKWALK_FILENAME);
			}
		}
	}

	return TRUE;
}

BOOL MyEngine::OnSelectDevice()
{
	BOOL	bSelectWindowMode = FALSE;

	FILE*	fp = fopen("iwantwindow.txt","r");
	if(fp)
	{
		bSelectWindowMode = TRUE;
		fclose(fp);
	}

	fp = fopen("fulldump.txt","r");
	if(fp)
	{
		g_eMiniDumpType	= MiniDumpWithFullMemory;
		fclose(fp);
	}
	else
	{
		g_eMiniDumpType	= MiniDumpNormal;
	}

	if( m_bEmulationFullscreen && !bSelectWindowMode )
	{
		// 그냥 창모드로 띄움.
		if( !RwEngineSetSubSystem(0) )		return FALSE;
		if( !RwEngineSetVideoMode(0) )		return FALSE;

		return TRUE;
	}

#ifdef _DEBUG
	bSelectWindowMode = TRUE;
#endif	// _DEBUG

	if( m_bEmulationFullscreen )
		bSelectWindowMode = TRUE;

	if( bSelectWindowMode )		return FALSE;
	if( !RwEngineSetSubSystem( RwEngineGetCurrentSubSystem() ) )
		return FALSE;

	SetupOption();

	INT32 lScreenWidth	= g_pcsAgcmUIOption->m_iScreenWidth;
	INT32 lScreenHeight	= g_pcsAgcmUIOption->m_iScreenHeight;
	INT32 lColorDepth	= g_pcsAgcmUIOption->m_iScreenDepth;
	INT32 lVideoMode	= RwEngineGetNumVideoModes();

	RwVideoMode			stVideoModeIt;
	RwVideoMode			stVideoMode;
	INT32				lProperVideoMode = -1;

	if ( g_pcsAgcmUIOption->m_bWindowed )
	{
		for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
		{
			if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
				continue;

			if ( ((stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) == 0) && 
					stVideoModeIt.width >= lScreenWidth && 
					stVideoModeIt.height >= lScreenHeight &&
					stVideoModeIt.depth >= lColorDepth )
			{
				stVideoMode = stVideoModeIt;
				lProperVideoMode = lIndex;
				break;
			}
		}
		
		if ( lProperVideoMode == -1 )
		{
			lScreenWidth = 1024;
			lScreenHeight = 768;

			for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
			{
				if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
					continue;

				if ( ((stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) == 0) && 
					stVideoModeIt.width == lScreenWidth && 
					stVideoModeIt.height == lScreenHeight &&
					stVideoModeIt.depth >= lColorDepth )
				{
					stVideoMode = stVideoModeIt;
					lProperVideoMode = lIndex;
					break;
				}
			}
		}
	}
	else
	{
		for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
		{
			if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
				continue;
			
			if ( (stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) && 
					stVideoModeIt.width == lScreenWidth && 
					stVideoModeIt.height == lScreenHeight )
			{
				stVideoMode = stVideoModeIt;
				lProperVideoMode = lIndex;

				if (stVideoModeIt.depth == lColorDepth)
				{
					break;
				}
			}
		}

		if ( lProperVideoMode == -1 )
		{
			lScreenWidth = 1024;
			lScreenHeight = 768;

			for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
			{
				if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
					continue;

				if ( (stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) && 
					stVideoModeIt.width == lScreenWidth && 
					stVideoModeIt.height == lScreenHeight )
				{
					stVideoMode = stVideoModeIt;
					lProperVideoMode = lIndex;

					if (stVideoModeIt.depth == lColorDepth)
					{
						break;
					}
				}
			}
		}
	}

	if( lProperVideoMode == -1 )					return FALSE;
    if( !RwEngineSetVideoMode( lProperVideoMode ) )	return FALSE;
        

	RwD3D9EngineSetRefreshRate(60);

    if (stVideoMode.flags & rwVIDEOMODEEXCLUSIVE)
    {
        RsGlobal.maximumWidth	= stVideoMode.width;
        RsGlobal.maximumHeight	= stVideoMode.height;
		PsGlobal.fullScreen		= TRUE;
    }

	return TRUE;
}

BOOL	MyEngine::OnInitialize			()
{	
	SetupOption();

	RsGlobal.maximumWidth = g_pcsAgcmUIOption->m_iScreenWidth;
	RsGlobal.maximumHeight = g_pcsAgcmUIOption->m_iScreenHeight;

	if( g_eServiceArea == AP_SERVICE_AREA_CHINA )
	{
		if( m_bEmulationFullscreen )
			m_pMainWindow->ToggleFullScreenTest();
	}
	return AgcEngine::OnInitialize();
}

BOOL MyEngine::SetupOption()
{
	if( g_pcsAgcmUIOption )		return TRUE;

	g_pcsAgcmUIOption = new AgcmUIOption;

	TCHAR*	szCommandLine = GetCommandLine();

	AgcmOptionStartup eStartupOption = AGCM_OPTION_STARTUP_NORMAL;
	if (strstr(szCommandLine, PATCHCLIENT_OPTION_HIGH))
		eStartupOption = AGCM_OPTION_STARTUP_HIGH;
	else if (strstr(szCommandLine, PATCHCLIENT_OPTION_LOW))
		eStartupOption = AGCM_OPTION_STARTUP_LOW;

	BOOL	bOptionSaveLoad = strstr( szCommandLine, PATCHCLIENT_OPTION_NOSAVE ) ? FALSE : TRUE;
	TCHAR* szLoginServer = strstr(szCommandLine, PATCHCLIENT_LOGIN_SERVER);
	if( szLoginServer )
		m_szLoginServer = szLoginServer + strlen(PATCHCLIENT_LOGIN_SERVER);

	g_pcsAgcmUIOption->EnableSaveLoad( bOptionSaveLoad );
	g_pcsAgcmUIOption->SetStartMode( eStartupOption);
	g_pcsAgcmUIOption->InitGameOption();

	// 중국일때 테스트및 디버그 편하도록 /w 인자로 클라 띄우면 창모드로 동작하게 함.
	// 현재는 무조건 에뮬풀스크린으로 뜨게 되어있음. 향후 샨다에서 맘이 변해 창모드 띄우게
	// 요청들어오면 이거로 어찌 하면 될듯...
	if( g_eServiceArea == AP_SERVICE_AREA_CHINA )
		m_bEmulationFullscreen = strstr( szCommandLine, "/w") ? FALSE :TRUE;

	return TRUE;
}

// MyEngine::PatchPatcher
//
// 패치 클라이언트가 스스로를 패치하는데 실패했을경우
// DOWNLOAD_PATCH_CLIENT_NAME(archlord.bak)이 존재한다.
//
// 1. PATCH_CLIENT_NAME를 TEMP_PATCH_CLIENT_NAME로 바꾼다.
// 2. DOWNLOAD_PATCH_CLIENT_NAME을 PATCH_CLIENT_NAME으로 바꾼다.
// 3. TEMP_PATCH_CLIENT_NAME을 삭제한다.
//
BOOL MyEngine::PatchPatcher()
{
	// 다운로드 받은 패치 클라이언트가 없으면 그냥 넘어간다
	if ( !DoesFileExist(DOWNLOAD_PATCH_CLIENT_NAME) )
		return TRUE;

	// 98에서 patch client가 아직 실행중일때 이름이 바뀌는지 정확하지 않다.
	// 2000이상이라면 실행중인 파일의 이름도 수정가능하다.
	MoveFile( PATCH_CLIENT_NAME, TEMP_PATCH_CLIENT_NAME );

	if (0 == MoveFile( DOWNLOAD_PATCH_CLIENT_NAME, PATCH_CLIENT_NAME ))
	{	// DOWNLOAD_PATCH_CLIENT_NAME을 PATCH_CLIENT_NAME으로 바꾸는데 실패하면 원상 복구
		MoveFile( TEMP_PATCH_CLIENT_NAME, PATCH_CLIENT_NAME );
	}

	// 프로그램이 실행중이여서 삭제에 실패한다면 조금 기다린후에 다시 삭제를 시도한다.
	// 2000 이상에서는 실행중인 파일이라도 삭제가 가능하다. 98을 위한 코드
	if (0 == DeleteFile( TEMP_PATCH_CLIENT_NAME ) && GetLastError() == ERROR_ACCESS_DENIED)
	{
		Sleep(200);
		DeleteFile( TEMP_PATCH_CLIENT_NAME );	// 그래도 안지워지면 다음기회에
	}

	return TRUE;
}

VOID MyEngine::SetupLanguage()
{
	memset( m_szLanguage, 0, 128 );

	if ( g_eServiceArea != AP_SERVICE_AREA_WESTERN )
		return;

	g_INIFileName = "ini\\sysstr.txt";

	/*TCHAR			szBuffer[256];
	_stprintf( szBuffer, _T("SOFTWARE\\ArchLord") );

	HKEY	hRegKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, szBuffer, 0, KEY_READ, &hRegKey ) )
	{
		MessageBox(NULL, "Error Open Registry !!!", "Archlord", MB_OK);
		return;
	}
	
	// read IP
	DWORD dwType;
	DWORD dwLength = 256;
	TCHAR szTemp[256];
	memset( szTemp, 0, sizeof(char) * 256 );
	if( ERROR_SUCCESS != RegQueryValueEx(hRegKey, _T("Language"), 0, &dwType, (LPBYTE) szTemp, &dwLength ) )
		return;

	RegCloseKey(hRegKey); 

	if (!stricmp(szTemp, "English"))
	{
		g_INIFileName = "ini\\sysstr.txt";
	}
	else if (!stricmp(szTemp, "French"))
	{
		strcpy(m_szLanguage, "fr\\");
		g_INIFileName = "ini\\fr\\sysstr.txt";
	}
	else if (!stricmp(szTemp, "German"))
	{
		strcpy(m_szLanguage, "de\\");
		g_INIFileName = "ini\\de\\sysstr.txt";
	}*/
}

void	MyEngine::LuaErrorMessage( const char * pStr )
{
	#ifdef _DEBUG
	OutputDebugString( pStr );
	#endif

	if( g_pcsAgcmChatting2 )
	{
		g_pcsAgcmChatting2->AddSystemMessage( ( char * ) pStr );
	}
	else
	{
		// do nothing..
	}
}

// 여기서 몹쓸 파일 삭제등등을 처리하자.
void MyEngine::PostInit(void)
{
	if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)
	{
		if ( DoesFileExist("archlord_de.dat") )
			DeleteAndRenameFile("archlord_de.exe", "archlord_de.dat");

		if ( DoesFileExist("archlord_fr.dat") )
			DeleteAndRenameFile("archlord_fr.exe", "archlord_fr.dat");
	}

	::DeleteFile("ArchlordJPTest.ini");
}

BOOL		MyEngine::CBGetMyCharacterName(PVOID pvData, PVOID pClass, PVOID pCustData)
{
	TCHAR*		pszCharacterName = (TCHAR *) pvData;

	//npgl.Send( pszCharacterName );

	return TRUE;
}

BOOL MyEngine::CBGameguardAuth( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if ( /*!pData || !pClass ||*/ !pCustData ) 
		return FALSE;

	//npgl.Auth( pCustData );

	return TRUE;
}

TeleportInfo *	MyEngine::GetTeleportPoint( INT32 nIndex )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	iter = m_mapTeleportPoint.find( nIndex );
	if( iter == m_mapTeleportPoint.end() )
	{
		return NULL;
	}
	else
	{
		// iter->first 는 INT32
		// iter->second 는 TeleportInfo
		return &iter->second;
	}
}

TeleportInfo *	MyEngine::GetTeleportPoint( string strName )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		if( strName == stInfo.strName )
			return &stInfo;
	}

	// 없을때..
	int nMatch = 0;
	TeleportInfo *	pPoint;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		if( strstr( stInfo.strName.c_str() , strName.c_str() ) )
		{
			nMatch++;
			pPoint = &stInfo;
		}
	}

	if( nMatch == 1 )
	{
		return pPoint;
	}
	else
	{
		nMatch = 0;
		// 글자 하나 검색.. (2바이트)
		for( iter = m_mapTeleportPoint.begin() ;
			iter != m_mapTeleportPoint.end();
			iter++ )
		{
			INT32			nIndex	= iter->first	;
			TeleportInfo	&stInfo	= iter->second	;

			if( !strncmp( stInfo.strName.c_str() , strName.c_str() , 2 ) )
			{
				nMatch++;
				pPoint = &stInfo;
			}
		}

		if( nMatch == 1 ) return pPoint;
	}

	return NULL;
}

BOOL			MyEngine::AddTeleportPoint( INT32 nIndex , TeleportInfo &stInfo )
{
	// 있던것도 덮어쓴다.
	m_mapTeleportPoint[ nIndex ] = stInfo;
	return TRUE;
}

INT32			MyEngine::GetTeleportEmptyIndex()
{
	for( INT32 nIndex = 1 ; nIndex < 65535 ; nIndex++ )
	{
		if( !GetTeleportPoint( nIndex ) )
		{
			return nIndex;
		}
	}

	return -1;
}

const string	_strKeyName	= "Name";
const string	_strKeyPos	= "Pos";

BOOL			MyEngine::LoadTelportPoint( string strName )
{
	AuIniManagerA	iniManager;
	iniManager.SetPath(	strName.c_str()	);

	if( iniManager.ReadFile(0, FALSE) )
	{
		m_mapTeleportPoint.clear();

		int		nSectionCount	;

		int		nKeyName	;
		int		nKeyPos		;

		nSectionCount	= iniManager.GetNumSection();	

		// 테긋쳐 추가함..

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nKeyName	= iniManager.FindKey( i , _strKeyName.c_str()	);
			nKeyPos		= iniManager.FindKey( i , _strKeyPos.c_str()	);

			int nSection = atoi( iniManager.GetSectionName( i ) );

			string	strName , strPos;
			strName	= iniManager.GetValue( i , nKeyName );
			strPos	= iniManager.GetValue( i , nKeyPos );
			
			FLOAT	x = 0.0f ,z = 0.0f;
			sscanf( strPos.c_str() , "%f,%f" , &x , &z );

			AddTeleportPoint( nSection , TeleportInfo( strName.c_str() , x , z ) );
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL			MyEngine::SaveTelportPoint( string strName )
{
	AuIniManagerA	ini;
	ini.SetPath( strName.c_str() );

	map< INT32 , TeleportInfo >::iterator	iter;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		// 세팅 과정.
		char	strSection[ 256 ];
		sprintf( strSection , "%d" , nIndex );
		char	strPos[ 256 ];
		sprintf( strPos , "%.0f,%.0f" , stInfo.pos.x , stInfo.pos.z );

		ini.SetValue( strSection , _strKeyName.c_str() , stInfo.strName.c_str() );
		ini.SetValue( strSection , _strKeyPos.c_str() , strPos );
	}

	ini.WriteFile(0, FALSE);

	return TRUE;
}

BOOL			MyEngine::DeleteTeleportPoint( INT32 nIndex )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	iter = m_mapTeleportPoint.find( nIndex );
	if( iter == m_mapTeleportPoint.end() )
	{
		return FALSE;
	}
	else
	{
		m_mapTeleportPoint.erase( iter );
		return TRUE;
	}
}

BOOL			MyEngine::DeleteTeleportPoint( string strName )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		if( strName == stInfo.strName )
		{
			m_mapTeleportPoint.erase( iter );
			return TRUE;
		}
	}

	return FALSE;
}
