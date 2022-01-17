#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4819 )

#include "ApBase.h"
#include "BugSlay/BugSlayerUtil.h"

#include "AcuFrameMemory.h"
#include "AcuIMDraw.h"
#include "AcuDeviceCheck.h"
#include "AcuParticleDraw.h"

#include <AgcEngine.h>

#include "AgcModule.h"
#include "AgcmResourceLoader.h"
#include "AgpmFactors.h"
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AgcmAmbOcclMap.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgpmItemConvert.h"
#include "AgcmItemConvert.h"
#include "AgcmPrivateTrade.h"
#include "AgcmConnectManager.h"
#include "AgcmLogin.h"
#include "AgpmAuctionCategory.h"
#include "AgpmAuction.h"
#include "AgcmAuction.h"
#include "AgcmRecruit.h"
#include "AgcmCamera2.h"
#include "AgpmTimer.h"
#include "ApmEventManager.h"
#include "AgcmEventManager.h"
#include "AgpmEventNature.h"
#include "AgcmEventNature.h"
#include "AgpmEventTeleport.h"
#include "AgpmEventNPCDialog.h"
#include "AgpmEventNPCTrade.h"
#include "AgcdEffGlobal.h"
#include "AgcmEventEffect.h"
#include "AgcmEventNPCDialog.h"
#include "AgcmEventNPCTrade.h"
#include "AgpmEventBank.h"
#include "AgcmEventBank.h"
#include "AgpmEventItemConvert.h"
#include "AgcmEventItemConvert.h"
#include "AgpmEventGuild.h"
#include "AgcmEventGuild.h"
#include "AgpmEventProduct.h"
#include "AgcmEventProduct.h"
#include "AgpmEventQuest.h"
#include "AgcmEventQuest.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmEff2.h"
#include "AgcmSound.h"
#include "AgpmParty.h"
#include "AgcmParty.h"
#include "AgcmUIManager2.h"
#include "AgcmWater.h"
#include "AgcmSkill.h"
#include "AgcmLODManager.h"
#include "AgcmPreLODManager.h"
#include "AgcmFont.h"
#include "AgpmChatting.h"
#include "AgcmChatting2.h"
#include "AgcmUIControl.h"
#include "AgcmLensFlare.h"
#include "AgcmTextBoardMng.h"
#include "AgcmTuner.h"
#include "AgpmGrid.h"
#include "AgcmGlyph.h"
#include "AgcmTargeting.h"
#include "AgpmQuest.h"
#include "AgcmQuest.h"
#include "AgpmBuddy.h"
#include "AgpmChannel.h"
#include "AgcmGlare.h"
#include "AgpmCombat.h"
#include "AgcmUVAnimation.h"
#include "AgpmWorld.h"
#include "AgcmWorld.h"
#include "AgpmRide.h"
#include "AgcmRide.h"
#include "AgpmSummons.h"
#include "AgpmSystemMessage.h"

#include "AgpmAdmin.h"
#include "AgcmAdmin.h"

#include "AgcmUIMain.h"
#include "AgpmUIStatus.h"
#include "AgcmUIStatus.h"
#include "AgcmUILogin.h"
#include "AgcmUIChatting2.h"
#include "AgcmUICharacter.h"
#include "AgcmUICooldown.h"
#include "AgcmUIAuction.h"
#include "AgcmUIItem.h"
#include "AgcmUISkill2.h"
#include "AgcmUIEventItemRepair.h"
#include "AgcmUIEventTeleport.h"
#include "AgcmUIParty.h"
#include "AgcmUIPartyOption.h"
#include "AgcmUIEventBank.h"
#include "AgcmUIEventItemConvert.h"
#include "AgcmUIEventNPCDialog.h"
#include "AgcmUIGuild.h"
#include "AgcmUISplitItem.h"
#include "AgcmUIQuest2.h"
#include "AgcmUIBuddy.h"
#include "AgcmUIChannel.h"
#include "AgcmUIPvP.h"
#include "AgcmUICustomizing.h"
#include "AgcmUISystemMessage.h"

#include "AgpmSearch.h"
#include "AgcmUISearch.h"

#include "AgcmMinimap.h"

#include "AgcmShadow.h"
#include "AgcmShadow2.h"
#include "AgcmUIOption.h"

#include "ApmOcTree.h"
#include "AgcmOcTree.h"
#include "AgcmGrass.h"

#include "AgpmGuild.h"
#include "AgcmGuild.h"

#include "AgpmProduct.h"
#include "AgcmProduct.h"
#include "AgcmUIProductSkill.h"
#include "AgcmUIProduct.h"
#include "AgcmProductMed.h"

#include "AgpmRefinery.h"
#include "AgcmRefinery.h"
#include "AgpmEventRefinery.h"
#include "AgcmEventRefinery.h"
#include "AgcmUIRefinery.h"
#include "AgcmUITips.h"
#include "AgcmUIHelp.h"

#include "AgpmRemission.h"
#include "AgcmUIRemission.h"
#include "AgpmMailBox.h"
#include "AgcmUIMailBox.h"
#include "AgcmUISiegeWar.h"

#include "AgpmOptimizedPacket2.h"

#include "AgcmDynamicLightmap.h"
#include "AgcmShadowmap.h"
#include "AgcmPostFX.h"

#include "AgcmUIDebugInfo.h"
//#include "AuPackingManager.h"

#include "AgcmNatureEffect.h"

#include "AgpmAreaChatting.h"
#include "AgpmEventPointLight.h"
#include "AgcmEventPointLight.h"

#include "AgpmScript.h"

#include "AgcmCashMall.h"
#include "AgcmUICashMall.h"

#include "AgcmUICashInven.h"

#include "AgpmMailBox.h"
#include "AgcmUIMailBox.h"

#include "AgpmWantedCriminal.h"
#include "AgcmUIWantedCriminal.h"

#include "AgpmStartupEncryption.h"
#include "AgcmStartupEncryption.h"

#include "AgcmUIConsole.h"

#include "AgpmConfig.h"

#include "AgcmUIHotkey.h"
#include "AgcmUISocialAction.h"

#include "AgpmTax.h"
#include "AgcmUITax.h"

#include "AgpmGuildWarehouse.h"
#include "AgcmUIGuildWarehouse.h"

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//#include "AgpmGamble.h"
//#include "AgcmUIGamble.h"

#include "AgpmCasper.h"

#include "AgpmArchlord.h"
#include "AgcmArchlord.h"
#include "AgcmUIArchlord.h"

#include "AgpmEventGacha.h"
#include "AgcmEventGacha.h"
#include "AgcmUIEventGacha.h"

#include "AgpmBattleGround.h"
#include "AgcmBattleGround.h"

#include "AgcmLODManager.h"
#include "AgcdGeoTriAreaTbl.h"
#include "AgcmBillInfo.h"
#include "AgcmCustomizeRender.h"

#include "MainWindow.h"
#include "MyException.h"

#pragma warning ( pop )

//#define _BIN_EXEC_

class AgcmCustomizeRender;

extern AgpmConfig			*g_pcsAgpmConfig;
extern AgcmResourceLoader	*g_pcsAgcmResourceLoader;
extern AgpmFactors			*g_pcsAgpmFactors;
extern AgpmCharacter		*g_pcsAgpmCharacter;
extern AgcmCharacter		*g_pcsAgcmCharacter;
extern AgpmUnion			*g_pcsAgpmUnion;
extern AgpmItem				*g_pcsAgpmItem;
extern AgcmItem				*g_pcsAgcmItem;
extern AgpmItemConvert		*g_pcsAgpmItemConvert;
extern AgcmItemConvert		*g_pcsAgcmItemConvert;
extern AgcmPrivateTrade		*g_pcsAgcmPrivateTrade;
extern AgpmAuctionCategory	*g_pcsAgpmAuctionCategory;
extern AgpmAuction			*g_pcsAgpmAuction;
extern AgcmAuction			*g_pcsAgcmAuction;
extern AgcmRecruit			*g_pcsAgcmRecruit;

extern AgpmRide*			g_pcsAgpmRide;
extern AgcmRide*			g_pcsAgcmRide;

extern ApmEventManager		*g_pcsApmEventManager	;
extern AgpmEventNature		*g_pcsAgpmEventNature	;
extern AgpmTimer			*g_pcsAgpmTimer;
extern AgcmEventNature		*g_pcsAgcmEventNature	;
extern AgpmEventTeleport	*g_pcsAgpmEventTeleport;
extern AgcmEventTeleport	*g_pcsAgcmEventTeleport;
extern AgpmEventNPCDialog	*g_pcsAgpmEventNPCDialog;
extern AgpmEventNPCTrade	*g_pcsAgpmEventNPCTrade;
extern ApmObject			*g_pcsApmObject		;
extern AgcmObject			*g_pcsAgcmObject		;
extern AgcmEventEffect		*g_pcsAgcmEventEffect	;
extern AgcmEventNPCTrade	*g_pcsAgcmEventNPCTrade;
extern AgpmEventItemRepair	*g_pcsAgpmEventItemRepair;
extern AgcmEventItemRepair	*g_pcsAgcmEventItemRepair;
extern AgpmEventBank		*g_pcsAgpmEventBank	;
extern AgcmEventBank		*g_pcsAgcmEventBank	;
extern AgpmEventItemConvert	*g_pcsAgpmEventItemConvert	;
extern AgcmEventItemConvert	*g_pcsAgcmEventItemConvert	;
extern AgcmSound			*g_pcsAgcmSound		;
extern AgpmParty			*g_pcsAgpmParty		;
extern AgcmParty			*g_pcsAgcmParty		;
extern AgcmShadow			*g_pcsAgcmShadow	;
extern AgcmShadow2*			g_pcsAgcmShadow2	;
extern AgcmUIManager2		*g_pcsAgcmUIManager2;
extern AgcmWater			*g_pcsAgcmWater		;
extern AgpmSkill			*g_pcsAgpmSkill		;
extern AgcmSkill			*g_pcsAgcmSkill		;
extern AgpmEventSkillMaster	*g_pcsAgpmEventSkillMaster	;
extern AgcmEventSkillMaster	*g_pcsAgcmEventSkillMaster	;
extern AgcmLODManager		*g_pcsAgcmLODManager;
extern AgcmFont				*g_pcsAgcmFont		;
extern AgcmChatting2		*g_pcsAgcmChatting2	;
extern AgpmChatting			*g_pcsAgpmChatting	;
extern AgcmUIControl		*g_pcsAgcmUIControl	;
extern AgcmLensFlare		*g_pcsAgcmLensFlare	;
extern AgcmTextBoardMng		*g_pcsAgcmTextBoardMng	;
extern AgcmRender			*g_pcsAgcmRender	;
extern AgcmTuner			*g_pcsAgcmTuner		;
extern AgpmGrid				*g_pcsAgpmGrid		;
extern AgcmGlyph			*g_pcsAgcmGlyph		;
extern AgcmTargeting		*g_pcsAgcmTargeting	;
extern AgpmQuest			*g_pcsAgpmQuest		;
extern AgcmQuest			*g_pcsAgcmQuest		;
extern AgcmUVAnimation		*g_pcsAgcmUVAnimation	;

extern AgcmCustomizeRender	*g_pcsAgcmCustomizeRender;

extern AgpmAdmin			*g_pcsAgpmAdmin		;
extern AgcmAdmin			*g_pcsAgcmAdmin		;

extern AgcmUIMain			*g_pcsAgcmUIMain		;
extern AgcmUILogin			*g_pcsAgcmUILogin		;
extern AgcmLogin			*g_pcsAgcmLogin			;
extern AgcmUIChatting2		*g_pcsAgcmUIChatting2	;

extern AgcmUICharacter		*g_pcsAgcmUICharacter	;
extern AgcmUICooldown		*g_pcsAgcmUICooldown	;
extern AgcmUIAuction		*g_pcsAgcmUIAuction		;
extern AgcmUIItem			*g_pcsAgcmUIItem		;
extern AgcmUISkill2			*g_pcsAgcmUISkill2		;
extern AgcmUIEventItemRepair	*g_pcsAgcmUIEventItemRepair	;
extern AgcmUIEventTeleport	*g_pcsAgcmUIEventTeleport	;
extern AgcmUIParty			*g_pcsAgcmUIParty;
extern AgcmUIPartyOption	*g_pcsAgcmUIPartyOption;
extern AgcmUIEventBank		*g_pcsAgcmUIEventBank	;
extern AgcmUIEventNPCDialog *g_pcsAgcmUIEventNPCDialog;
extern AgcmUIEventItemConvert	*g_pcsAgcmUIEventItemConvert;
extern AgcmUISplitItem		*g_pcsAgcmUISplitItem;

extern AgcmMinimap			*g_pcsAgcmMinimap	;
extern ApmMap				*g_pcsApmMap		;

extern AgcmUIBuddy			*g_pcsAgcmUIBuddy	;
extern AgpmSearch			*g_pcsAgpmSearch	;
extern AgcmUISearch			*g_pcsAgcmUISearch	;

extern AgcmGlare			*g_pcsAgcmGlare		;
extern AgcmUIOption			*g_pcsAgcmUIOption;
extern ApmOcTree			*g_pcsApmOcTree;
extern AgcmOcTree			*g_pcsAgcmOcTree;

extern AgpmUIStatus			*g_pcsAgpmUIStatus;
extern AgcmUIStatus			*g_pcsAgcmUIStatus;

extern AgcmGrass			*g_pcsAgcmGrass;

extern AgpmGuild			*g_pcsAgpmGuild;

extern AgpmProduct			*g_pcsAgpmProduct;
extern AgcmProduct			*g_pcsAgcmProduct;
extern AgcmUIProductSkill	*g_pcsAgcmUIProductSkill;
extern AgcmUIProduct		*g_pcsAgcmUIProduct;
extern AgcmProductMed		*g_pcsAgcmProductMed;

extern AgpmRefinery			*g_pcsAgpmRefinery;
extern AgcmRefinery			*g_pcsAgcmRefinery;
extern AgpmEventRefinery	*g_pcsAgpmEventRefinery;
extern AgcmEventRefinery	*g_pcsAgcmEventRefinery;
extern AgcmUIRefinery		*g_pcsAgcmUIRefinery;

extern AgpmSummons			*g_pcsAgpmSummons;

extern AgpmPvP				*g_pcsAgpmPvP;
extern AgcmUIPvP			*g_pcsAgcmUIPvP;

extern AgpmEventCharCustomize	*g_pcsAgpmEventCharCustomize;
extern AgcmEventCharCustomize	*g_pcsAgcmEventCharCustomize;
extern AgcmUICustomizing		*g_pcsAgcmUICustomizing;

extern AgcmUISystemMessage		*g_pcsAgcmUISystemMessage;

extern AgcmUITips				*g_pcsAgcmUITips;
extern AgcmUIHelp				*g_pcsAgcmUIHelp;

extern AgpmRemission			*g_pcsAgpmRemission;
extern AgcmUIRemission			*g_pcsAgcmUIRemission;

extern AgpmOptimizedPacket2	*g_pcsAgpmOptimizedPacket2;

extern AgcmUIDebugInfo		*g_pcsAgcmUIDebugInfo;

extern AgpmAreaChatting		*g_pcsAgpmAreaChatting;

extern AgcmUIConsole		*g_pcsAgcmUIConsole;
extern AgcmNatureEffect	*g_pcsAgcmNatureEffect;
extern AgpmEventPointLight	*g_pcsAgpmEventPointLight;
extern AgcmEventPointLight	*g_pcsAgcmEventPointLight;

extern AgpmScript			*g_pcsAgpmScript;

extern AgpmCashMall			*g_pcsAgpmCashMall;
extern AgcmCashMall			*g_pcsAgcmCashMall;
extern AgcmUICashMall		*g_pcsAgcmUICashMall;
extern AgcmUICashInven		*g_pcsAgcmUICashInven;

extern AgpmMailBox			*g_pcsAgpmMailBox;
extern AgcmUIMailBox		*g_pcsAgcmUIMailBox;

extern AgpmReturnToLogin	*g_pcsAgpmReturnToLogin;
extern AgcmReturnToLogin	*g_pcsAgcmReturnToLogin;

extern AgpmWantedCriminal	*g_pcsAgpmWantedCriminal;
extern AgcmUIWantedCriminal	*g_pcsAgcmUIWantedCriminal;

extern AgpmStartupEncryption *g_pcsAgpmStartupEncryption;
extern AgcmStartupEncryption *g_pcsAgcmStartupEncryption;

extern AgpmSiegeWar			*g_pcsAgpmSiegeWar;
extern AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC;
extern AgcmSiegeWar			*g_pcsAgcmSiegeWar;
extern AgcmEventSiegeWarNPC	*g_pcsAgcmEventSiegeWarNPC;
extern AgcmUISiegeWar		*g_pcsAgcmUISiegeWar;

extern AgpmGuildWarehouse	*g_pcsAgpmGuildWarehouse;
extern AgcmUIGuildWarehouse	*g_pcsAgcmUIGuildWarehouse;

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//extern AgpmGamble			*g_pcsAgpmGamble;
//extern AgcmUIGamble			*g_pcsAgcmUIGamble;

extern AgcmUIGuild			*g_pcsAgcmUIGuild;

extern AgcmUIHotkey			*g_pcsAgcmUIHotkey;

extern AgcmUISocialAction	*g_pcsAgcmUISocialAction;

extern AgpmCasper			*g_pcsAgpmCasper;

extern AgpmArchlord			*g_pcsAgpmArchlord;
extern AgcmArchlord			*g_pcsAgcmArchlord;
extern AgcmUIArchlord		*g_pcsAgcmUIArchlord;

extern AgcmConnectManager*	g_pcsAgcmConnectManager	;

extern AgcmPostFX*			g_pcsAgcmPostFX;
extern AgcmEff2*			g_pcsAgcmEff2;
extern AgpmEventGacha*		g_pcsAgpmEventGacha;

extern AgpmBattleGround*	g_pcsAgpmBattleGround;
extern AgcmBattleGround*	g_pcsAgcmBattleGround;

// Point Info for debug teleport
class	TeleportInfo
{
public:
	string	strName;
	AuPOS	pos;

	TeleportInfo( const char * pStr , FLOAT x , FLOAT z ): strName( pStr )
	{	pos.x = x;		pos.y = 0.0f;		pos.z = z;	}
	TeleportInfo(): strName( "noname" ) 
	{	pos.x = 0.0f;	pos.y = 0.0f;		pos.z = 0.0f;	}
};

class MyEngine : public AgcEngine, public CMyException
{
public:
	MyEngine();
	virtual ~MyEngine();

	// Message
	BOOL				OnInit();
	BOOL				OnInitialize();
	void				OnTerminate();
	BOOL				OnAddModule();
	BOOL				OnAttachPlugins();	// 플러그인 연결 은 여기서
	RpWorld *			OnCreateWorld(); // 월드를 생성해서 리턴해준다.
	RwCamera *			OnCreateCamera( RpWorld * pWorld );	// 메인 카메라를 Create해준다.
	BOOL				OnCreateLight();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	AgcWindow *			OnSetMainWindow()	{	return m_pMainWindow;	}

	BOOL				OnRegisterModulePreCreateWorld()	{	return TRUE;	}
	BOOL				OnRegisterModule();

	VOID				OnEndIdle();
	VOID				OnWorldRender();
	BOOL				OnRenderPreCameraUpdate();

	BOOL				OnPostRegisterModule()				{	return TRUE;	}
	BOOL				OnPostInitialize();
	BOOL				OnCameraResize( RwRect * pRect );	

	UINT32				OnWindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam );

	BOOL				OnSelectDevice();
	void				LuaErrorMessage( const char * pStr );
	BOOL				SetupOption();

protected:
	BOOL				RemoveDebugLogFile();	// 2004.01.15. steeple
	BOOL				PatchPatcher();
	VOID				SetupLanguage();
	VOID				PostInit(void);

public:
	static BOOL		CBGetMyCharacterName(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGameguardAuth( PVOID pData, PVOID pClass, PVOID pCustData	);

	INT32			GetTeleportEmptyIndex();
	TeleportInfo *	GetTeleportPoint( INT32 nIndex );
	TeleportInfo *	GetTeleportPoint( string strName );
	BOOL			AddTeleportPoint( INT32 nIndex , TeleportInfo &stInfo );
	BOOL			DeleteTeleportPoint( INT32 nIndex );
	BOOL			DeleteTeleportPoint( string strName );
	BOOL			LoadTelportPoint( string strName = "ggpoint.ini" );
	BOOL			SaveTelportPoint( string strName = "ggpoint.ini" );

public:
	HCURSOR				m_hCursor;
	BOOL				m_bEmulationFullscreen;
	CHAR*				m_szLoginServer;
	MainWindow*			m_pMainWindow;

	AcuFrameMemory		m_cAcuFrameMemory;
	AcuIMDraw			m_cAcuIMDraw;
	AcuDeviceCheck		m_cAcuDeviceCheck;
	AcuParticleDraw		m_cAcuParticleDraw;

	map< INT32 , TeleportInfo >	m_mapTeleportPoint;
};

extern MyEngine g_cMyEngine;