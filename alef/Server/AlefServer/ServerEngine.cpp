/******************************************************************************
Module:  AgsEngine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 10
******************************************************************************/

#include "ServerEngine.h"
#include "magdebug.h"
//#include "AuSharedMemory.h"
//#include "ApmProfiler.h"
//#include "AgsmPacketMonitor.h"
#include "AuPacket.h"
#include "AuGameEnv.h"
#include "ExceptionHandler.h"
#include "ApMemoryChecker.h"
#include "AgsmEventGacha.h"
#include "AgpmBattleGround.h"
#include "AgsmBattleGround.h"
#include "AgsmNpcManager.h"
#include "AgsmEventSystem.h"
#include "AgsmEpicZone.h"
#include "AgpmEpicZone.h"
#include "AgpmMonsterPath.h"

#ifdef _AREA_JAPAN_
#include "AuNProtect.h"
#endif

#ifdef _AREA_CHINA_
	#define ENC_SVR_DATA	1
#else
	#define ENC_SVR_DATA	0
#endif

#ifdef _AREA_CHINA_
#include "AgsmGameholic.h"
#endif

#define	RELAY_CHECK_COUNT			18

// public module
ApmMap*					g_pcsApmMap;
ApmObject*				g_pcsApmObject;
AgpmPathFind*			g_pcsAgpmPathFind;
AgpmFactors*			g_pcsAgpmFactors;
AgpmCharacter*			g_pcsAgpmCharacter;
//AgpmScript*				g_pcsAgpmScript;
AgpmUnion*				g_pcsAgpmUnion;
AgpmGrid*				g_pcsAgpmGrid;
AgpmItem*				g_pcsAgpmItem;
AgpmItemConvert*		g_pcsAgpmItemConvert;
AgpmCombat*				g_pcsAgpmCombat;
AgpmDropItem2*			g_pcsAgpmDropItem2;
AgpmParty*				g_pcsAgpmParty;
AgpmShrine*				g_pcsAgpmShrine;
AgpmSkill*				g_pcsAgpmSkill;
AgpmEventSkillMaster*	g_pcsAgpmEventSkillMaster;
AgpmChatting*			g_pcsAgpmChatting;
AgpmTimer*				g_pcsAgpmTimer;
AgpmUIStatus*			g_pcsAgpmUIStatus;
AgpmAuction*			g_pcsAgpmAuction;
AgpmResourceInfo*		g_pcsAgpmResourceInfo;
AgpmLogin*				g_pcsAgpmLogin;

// server-side module
AgsmAOIFilter*			g_pcsAgsmAOIFilter;
AgsmFactors*			g_pcsAgsmFactors;
AgsmMap*				g_pcsAgsmMap;
AgsmAccountManager*		g_pcsAgsmAccountManager;
AgsmCharacter*			g_pcsAgsmCharacter;
AgsmZoning*				g_pcsAgsmZoning;
AgsmItem*				g_pcsAgsmItem;
AgsmItemConvert*		g_pcsAgsmItemConvert;
AgsmPrivateTrade*		g_pcsAgsmPrivateTrade;
AgsmCharManager*		g_pcsAgsmCharManager;
AgsmItemManager*		g_pcsAgsmItemManager;
AgsmDropItem2*			g_pcsAgsmDropItem2;
AgsmCombat*				g_pcsAgsmCombat;
//AgsmChat				g_csAgsmChat;
//AgsmShrine			g_csAgsmShrine;
AgsmDeath*				g_pcsAgsmDeath;
AgsmShrine*				g_pcsAgsmShrine;
AgsmSkill*				g_pcsAgsmSkill;
AgsmSkillManager*		g_pcsAgsmSkillManager;
AgsmEventSkillMaster*	g_pcsAgsmEventSkillMaster;
AgsmServerManager*		g_pcsAgsmServerManager;
AgsmInterServerLink*	g_pcsAgsmInterServerLink;
AgsmAuctionRelay*		g_pcsAgsmAuctionRelay;
AgsmAuction*			g_pcsAgsmAuction;
AgsmLogin*				g_pcsAgsmLogin;
AgsmSystemInfo*			g_pcsAgsmSystemInfo;
AgsmParty*				g_pcsAgsmParty;
AgsmChatting*			g_pcsAgsmChatting;
AgsmUIStatus*			g_pcsAgsmUIStatus;
AgsmTimer*				g_pcsAgsmTimer;
AgsmConfig				*g_pcsAgsmConfig;	// kelovon, 20051010

// Event Module
ApmEventManager*		g_pcsApmEventManager;
AgsmEventManager*		g_pcsAgsmEventManager;
AgsmEventNature*		g_pcsAgsmEventNature;
AgpmEventTeleport*		g_pcsAgpmEventTeleport;
AgsmEventTeleport*		g_pcsAgsmEventTeleport;
AgpmEventNPCTrade*		g_pcsAgpmEventNPCTrade;
AgpmEventNPCDialog*		g_pcsAgpmEventNPCDialog;
AgsmEventNPCDialog*		g_pcsAgsmEventNPCDialog;
AgsmEventNPCTrade*		g_pcsAgsmEventNPCTrade;
AgpmEventSpawn*			g_pcsAgpmEventSpawn;
AgsmEventSpawn*			g_pcsAgsmEventSpawn;
AgpmEventNature*		g_pcsAgpmEventNature;
AgpmEventItemRepair*	g_pcsAgpmEventItemRepair;
AgsmEventItemRepair*	g_pcsAgsmEventItemRepair;
AgpmEventBank*			g_pcsAgpmEventBank;
AgsmEventBank*			g_pcsAgsmEventBank;
AgpmEventGuild*			g_pcsAgpmEventGuild;
AgsmEventGuild*			g_pcsAgsmEventGuild;
AgpmProduct*			g_pcsAgpmProduct;
AgpmEventProduct*		g_pcsAgpmEventProduct;
AgpmEventRefinery*		g_pcsAgpmEventRefinery;
AgpmRefinery*			g_pcsAgpmRefinery;
AgpmEventQuest*			g_pcsAgpmEventQuest;
AgsmEventQuest*			g_pcsAgsmEventQuest;
AgpmEventItemConvert*	g_pcsAgpmEventItemConvert;
AgsmEventItemConvert*	g_pcsAgsmEventItemConvert;

AgpmEventBinding*		g_pcsAgpmEventBinding;

AgpmAI2*				g_pcsAgpmAI2;
AgsmAI2*				g_pcsAgsmAI2;

AgpmQuest*				g_pcsAgpmQuest;
AgsmQuest*				g_pcsAgsmQuest;

AgpmBuddy*				g_pcsAgpmBuddy;
AgsmBuddy*				g_pcsAgsmBuddy;

AgpmChannel*			g_pcsAgpmChannel;
AgsmChannel*			g_pcsAgsmChannel;

AgpmMailBox*			g_pcsAgpmMailBox;
AgsmMailBox*			g_pcsAgsmMailBox;

// 2004.06.23. steeple
AgpmGuild*				g_pcsAgpmGuild;
AgsmGuild*				g_pcsAgsmGuild;

AgpmRide*				g_pcsAgpmRide;
AgsmRide*				g_pcsAgsmRide;

AgsmProduct*			g_pcsAgsmProduct;
AgsmEventProduct*		g_pcsAgsmEventProduct;
AgsmRefinery*			g_pcsAgsmRefinery;
AgsmEventRefinery*		g_pcsAgsmEventRefinery;

// 2004.12.22. steeple
AgpmPvP*				g_pcsAgpmPvP;
AgsmPvP*				g_pcsAgsmPvP;

// 2005.08.02. steeple
AgpmSummons*			g_pcsAgpmSummons;
AgsmSummons*			g_pcsAgsmSummons;

AgpmSearch*				g_pcsAgpmSearch;
AgsmSearch*				g_pcsAgsmSearch;

// 2004.04.27. steeple
AgpmLog*				g_pcsAgpmLog;
AgsmLog*				g_pcsAgsmLog;

AgpmAdmin*				g_pcsAgpmAdmin;
AgsmAdmin*				g_pcsAgsmAdmin;

//#ifdef _PROFILE_
//ApmProfiler*			g_pcsApmProfiler;
//#endif	//_PROFILE_
//AgsmPacketMonitor*		g_pcsAgsmPacketMonitor;

AgsmRelay2*				g_pcsAgsmRelay2;

AgpmCasper*				g_pcsAgpmCasper;
AgsmCasper*				g_pcsAgsmCasper;

AgpmOptimizedPacket2*	g_pcsAgpmOptimizedPacket2;

AgsmServerStatus*		g_pcsAgsmServerStatus;

AgpmEventCharCustomize*	g_pcsAgpmEventCharCustomize;
AgsmEventCharCustomize*	g_pcsAgsmEventCharCustomize;

AgpmBillInfo*			g_pcsAgpmBillInfo;
AgsmBillInfo*			g_pcsAgsmBillInfo;

AgsmGK*					g_pcsAgsmGK;

AgpmAreaChatting*		g_pcsAgpmAreaChatting;
AgsmAreaChatting*		g_pcsAgsmAreaChatting;

AgsmGlobalChatting*		g_pcsAgsmGlobalChatting;

// kelovon
AgpmSystemMessage		*g_pcsAgpmSystemMessage;
AgsmSystemMessage		*g_pcsAgsmSystemMessage;
AgpmConfig				*g_pcsAgpmConfig;

AgpmRemission			*g_pcsAgpmRemission;
AgsmRemission			*g_pcsAgsmRemission;

AgpmWantedCriminal		*g_pcsAgpmWantedCriminal;
AgsmWantedCriminal		*g_pcsAgsmWantedCriminal;

AgpmCashMall			*g_pcsAgpmCashMall;

AgsmBilling				*g_pcsAgsmBilling;
AgsmCashMall			*g_pcsAgsmCashMall;

AgsmUsedCharDataPool	*g_pcsAgsmUsedCharDataPool;

AgpmReturnToLogin		*g_pcsAgpmReturnToLogin;
AgsmReturnToLogin		*g_pcsAgsmReturnToLogin;

AgpmSiegeWar			*g_pcsAgpmSiegeWar;
AgsmSiegeWar			*g_pcsAgsmSiegeWar;
AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC;
AgsmEventSiegeWarNPC	*g_pcsAgsmEventSiegeWarNPC;

AgpmStartupEncryption	*g_pcsAgpmStartupEncryption;
AgsmStartupEncryption	*g_pcsAgsmStartupEncryption;

AgpmTax					*g_pcsAgpmTax;
AgsmTax					*g_pcsAgsmTax;

AgpmGuildWarehouse		*g_pcsAgpmGuildWarehouse;
AgsmGuildWarehouse		*g_pcsAgsmGuildWarehouse;

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//AgpmGamble				*g_pcsAgpmGamble;
//AgsmGamble				*g_pcsAgsmGamble;

AgpmArchlord			*g_pcsAgpmArchlord;
AgsmArchlord			*g_pcsAgsmArchlord;

AgpmEventGacha			*g_pcsAgpmEventGacha;
AgsmEventGacha			*g_pcsAgsmEventGacha;

AgpmBattleGround		*g_pcsAgpmBattleGround;
AgsmBattleGround		*g_pcsAgsmBattleGround;

AgsmNpcManager			*g_pcsAgsmNpcManager;
AgsmEventSystem			*g_pcsAgsmEventSystem;

AgsmEpicZone			*g_pcsAgsmEpicZone;
AgpmEpicZone			*g_pcsAgpmEpicZone;

AgsmLoginClient			*g_pcsAgsmLoginClient; //JK_중복로그인

AgpmMonsterPath			*g_pcsAgpmMonsterPath;

AgpmTitle				*g_pcsAgpmTitle;
AgsmTitle				*g_pcsAgsmTitle;

ServerEngine::ServerEngine()
{
//	SetGUID(guidApp);
	m_lRelayCheckCount	= 0;

	m_hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventGuildLoadComplete = CreateEvent(NULL, TRUE, FALSE, NULL);	// 2005.04.27. steeple. 길드 로딩 될때까진 유저 안받는다.

#ifdef _M_X64
	m_csPoolCountStream.ReadPoolCount("Ini\\MemoryPoolCount(64bit).ini");
#else
	m_csPoolCountStream.ReadPoolCount("Ini\\MemoryPoolCount.ini");
#endif
}

ServerEngine::~ServerEngine()
{
	CloseHandle(m_hEventStop);
	CloseHandle(m_hEventGuildLoadComplete);
}

BOOL ServerEngine::OnRegisterModule()
{
	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
		printf("Service Area : China\n");
	else if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
		printf("Service Area : Western\n");
	else if ( g_eServiceArea == AP_SERVICE_AREA_JAPAN )
		printf("Service Area : Japan\n");
	else
		printf("Service Area : Korea\n");

	printf("Processing - 1\n");

	g_iServerClientType = ASSM_SERVER_TYPE_GAME_SERVER;

	// AuCircularBuffer에서 MakePacket을 할때 메모리를 미리 잡아 놓는다.
	// 400MB의 메모리를 초기화 한다.
	g_AuCircularBuffer.Init(m_csPoolCountStream.m_lCircularBuffer * 1024 * 1024);
//	g_AuCircularBuffer.Init(10 * 1024 * 1024);
	g_AuCircularOutBuffer.Init(m_csPoolCountStream.m_lCircularOutBuffer * 1024 * 1024);

	printf("Processing - 2\n");
	// 서버 설정용, 20051007, kelovon
	REGISTER_MODULE(g_pcsAgpmConfig, AgpmConfig);
	REGISTER_MODULE(g_pcsAgpmLog, AgpmLog);
	REGISTER_MODULE(g_pcsAgpmSystemMessage, AgpmSystemMessage);		// System Message, 20050907, kelovon

	REGISTER_MODULE(g_pcsApmMap, ApmMap);
	REGISTER_MODULE(g_pcsApmObject, ApmObject);
	REGISTER_MODULE(g_pcsAgpmPathFind, AgpmPathFind);
	REGISTER_MODULE(g_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(g_pcsAgpmCharacter, AgpmCharacter);

	//REGISTER_MODULE(g_pcsAgpmScript, AgpmScript);
	REGISTER_MODULE(g_pcsAgpmBillInfo, AgpmBillInfo);
	REGISTER_MODULE(g_pcsAgpmUnion, AgpmUnion);
	REGISTER_MODULE(g_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(g_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(g_pcsAgpmParty, AgpmParty);
	REGISTER_MODULE(g_pcsAgpmSummons, AgpmSummons);
	REGISTER_MODULE(g_pcsApmEventManager, ApmEventManager);
	REGISTER_MODULE(g_pcsAgpmSkill, AgpmSkill);
	REGISTER_MODULE(g_pcsAgpmItemConvert, AgpmItemConvert);
	REGISTER_MODULE(g_pcsAgpmTimer, AgpmTimer);
	REGISTER_MODULE(g_pcsAgpmSearch, AgpmSearch);

	REGISTER_MODULE(g_pcsAgpmResourceInfo, AgpmResourceInfo);

	// Event Module
	REGISTER_MODULE(g_pcsAgpmEventNature, AgpmEventNature);
	REGISTER_MODULE(g_pcsAgpmEventSpawn, AgpmEventSpawn);
	//REGISTER_MODULE(g_pcsAgpmShrine, AgpmShrine);
	REGISTER_MODULE(g_pcsAgpmEventBinding, AgpmEventBinding);
	REGISTER_MODULE(g_pcsAgpmEventSkillMaster, AgpmEventSkillMaster);
	REGISTER_MODULE(g_pcsAgpmCombat, AgpmCombat);
	REGISTER_MODULE(g_pcsAgpmChatting, AgpmChatting);
	REGISTER_MODULE(g_pcsAgpmAI2, AgpmAI2);
	REGISTER_MODULE(g_pcsAgpmEventNPCTrade, AgpmEventNPCTrade);
	REGISTER_MODULE(g_pcsAgpmEventNPCDialog, AgpmEventNPCDialog);
	REGISTER_MODULE(g_pcsAgpmEventBank, AgpmEventBank);
	REGISTER_MODULE(g_pcsAgpmEventItemConvert, AgpmEventItemConvert);
	REGISTER_MODULE(g_pcsAgpmQuest, AgpmQuest);
	REGISTER_MODULE(g_pcsAgpmBuddy, AgpmBuddy);
	REGISTER_MODULE(g_pcsAgpmChannel, AgpmChannel);
	REGISTER_MODULE(g_pcsAgpmMailBox, AgpmMailBox);
	REGISTER_MODULE(g_pcsAgpmUIStatus, AgpmUIStatus);
	REGISTER_MODULE(g_pcsAgpmGuild, AgpmGuild);
	REGISTER_MODULE(g_pcsAgpmPvP, AgpmPvP);
	REGISTER_MODULE(g_pcsAgpmRide, AgpmRide);
	REGISTER_MODULE(g_pcsAgpmAdmin, AgpmAdmin);	// AgpmAdmin 은 Public 모듈 중 제일 마지막에 AddModule 한다.

	REGISTER_MODULE(g_pcsAgpmProduct, AgpmProduct);
	REGISTER_MODULE(g_pcsAgpmEventProduct, AgpmEventProduct);	
	REGISTER_MODULE(g_pcsAgpmRefinery, AgpmRefinery);
	REGISTER_MODULE(g_pcsAgpmEventRefinery, AgpmEventRefinery);	
	REGISTER_MODULE(g_pcsAgpmEventItemRepair, AgpmEventItemRepair);
	REGISTER_MODULE(g_pcsAgpmEventGuild, AgpmEventGuild);
	REGISTER_MODULE(g_pcsAgpmEventQuest, AgpmEventQuest);

	REGISTER_MODULE(g_pcsAgpmAuction, AgpmAuction);
	REGISTER_MODULE(g_pcsAgpmDropItem2, AgpmDropItem2);
	REGISTER_MODULE(g_pcsAgpmLogin, AgpmLogin);

	REGISTER_MODULE(g_pcsAgpmOptimizedPacket2, AgpmOptimizedPacket2);

	REGISTER_MODULE(g_pcsAgpmAreaChatting, AgpmAreaChatting);

	REGISTER_MODULE(g_pcsAgpmCashMall, AgpmCashMall);

	REGISTER_MODULE(g_pcsAgpmReturnToLogin, AgpmReturnToLogin);
	REGISTER_MODULE(g_pcsAgpmWantedCriminal, AgpmWantedCriminal);
	REGISTER_MODULE(g_pcsAgpmStartupEncryption, AgpmStartupEncryption);

	REGISTER_MODULE(g_pcsAgpmSiegeWar, AgpmSiegeWar);
	REGISTER_MODULE(g_pcsAgpmArchlord, AgpmArchlord);
	REGISTER_MODULE(g_pcsAgpmEventTeleport, AgpmEventTeleport);
	REGISTER_MODULE(g_pcsAgpmEventSiegeWarNPC, AgpmEventSiegeWarNPC);
	REGISTER_MODULE(g_pcsAgpmEventCharCustomize, AgpmEventCharCustomize);
	REGISTER_MODULE(g_pcsAgpmEventGacha, AgpmEventGacha);
	
	REGISTER_MODULE(g_pcsAgpmRemission, AgpmRemission);
	REGISTER_MODULE(g_pcsAgpmTax, AgpmTax);
	REGISTER_MODULE(g_pcsAgpmGuildWarehouse, AgpmGuildWarehouse);
	
	REGISTER_MODULE(g_pcsAgpmCasper, AgpmCasper);

	//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
	//if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	//{
	//	REGISTER_MODULE(g_pcsAgpmGamble, AgpmGamble);
	//}

	REGISTER_MODULE(g_pcsAgsmSystemMessage, AgsmSystemMessage);		// System Message, 20051212, kelovon
	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	REGISTER_MODULE(g_pcsAgsmFactors, AgsmFactors);
	REGISTER_MODULE(g_pcsAgsmServerManager, AgsmServerManager);
	REGISTER_MODULE(g_pcsAgsmInterServerLink, AgsmInterServerLink);
	REGISTER_MODULE(g_pcsAgsmMap, AgsmMap);
	REGISTER_MODULE(g_pcsAgsmAccountManager, AgsmAccountManager);
	REGISTER_MODULE(g_pcsAgsmCharacter, AgsmCharacter);
	REGISTER_MODULE(g_pcsAgsmZoning, AgsmZoning);
	REGISTER_MODULE(g_pcsAgsmSystemInfo, AgsmSystemInfo);
	REGISTER_MODULE(g_pcsAgsmParty, AgsmParty);
	REGISTER_MODULE(g_pcsAgsmItem, AgsmItem);
	REGISTER_MODULE(g_pcsAgsmCharManager, AgsmCharManager);
	REGISTER_MODULE(g_pcsAgsmTimer, AgsmTimer);
	REGISTER_MODULE(g_pcsAgsmEventNature, AgsmEventNature);
	REGISTER_MODULE(g_pcsAgsmPrivateTrade, AgsmPrivateTrade);
	REGISTER_MODULE(g_pcsAgsmLogin, AgsmLogin);
	REGISTER_MODULE(g_pcsAgsmCombat, AgsmCombat);
	REGISTER_MODULE(g_pcsAgsmSummons, AgsmSummons);
	REGISTER_MODULE(g_pcsAgsmSkill, AgsmSkill);
	REGISTER_MODULE(g_pcsAgsmSkillManager, AgsmSkillManager);
	REGISTER_MODULE(g_pcsAgsmItemConvert, AgsmItemConvert);
	REGISTER_MODULE(g_pcsAgsmItemManager, AgsmItemManager);
	REGISTER_MODULE(g_pcsAgsmDropItem2, AgsmDropItem2);
	//REGISTER_MODULE(g_pcsAgsmShrine, AgsmShrine);
	REGISTER_MODULE(g_pcsAgsmUsedCharDataPool, AgsmUsedCharDataPool);
	REGISTER_MODULE(g_pcsAgsmDeath, AgsmDeath);
	REGISTER_MODULE(g_pcsAgsmChatting, AgsmChatting);
	REGISTER_MODULE(g_pcsAgsmEventManager, AgsmEventManager);
	REGISTER_MODULE(g_pcsAgsmEventSkillMaster, AgsmEventSkillMaster);
	REGISTER_MODULE(g_pcsAgsmEventTeleport, AgsmEventTeleport);
	REGISTER_MODULE(g_pcsAgsmEventNPCTrade, AgsmEventNPCTrade);
	REGISTER_MODULE(g_pcsAgsmEventSpawn, AgsmEventSpawn);
	REGISTER_MODULE(g_pcsAgsmEventItemRepair, AgsmEventItemRepair);
	REGISTER_MODULE(g_pcsAgsmEventBank, AgsmEventBank);
	REGISTER_MODULE(g_pcsAgsmEventGuild, AgsmEventGuild);

	REGISTER_MODULE(g_pcsAgsmProduct, AgsmProduct);
	REGISTER_MODULE(g_pcsAgsmEventProduct, AgsmEventProduct);
	REGISTER_MODULE(g_pcsAgsmRefinery, AgsmRefinery);
	REGISTER_MODULE(g_pcsAgsmEventRefinery, AgsmEventRefinery);
	REGISTER_MODULE(g_pcsAgsmAuctionRelay, AgsmAuctionRelay);
	REGISTER_MODULE(g_pcsAgsmAuction, AgsmAuction);	
	REGISTER_MODULE(g_pcsAgsmEventQuest, AgsmEventQuest);
	REGISTER_MODULE(g_pcsAgsmEventItemConvert, AgsmEventItemConvert);
	REGISTER_MODULE(g_pcsAgsmEventNPCDialog, AgsmEventNPCDialog);
	REGISTER_MODULE(g_pcsAgsmQuest, AgsmQuest);
	REGISTER_MODULE(g_pcsAgsmGuild, AgsmGuild);
	REGISTER_MODULE(g_pcsAgsmBuddy, AgsmBuddy);
	REGISTER_MODULE(g_pcsAgsmAI2, AgsmAI2);
	REGISTER_MODULE(g_pcsAgsmRide, AgsmRide);
	REGISTER_MODULE(g_pcsAgsmChannel, AgsmChannel);

	REGISTER_MODULE(g_pcsAgsmEventCharCustomize, AgsmEventCharCustomize);
	
	REGISTER_MODULE(g_pcsAgsmRemission, AgsmRemission);

	REGISTER_MODULE(g_pcsAgsmPvP, AgsmPvP);
	REGISTER_MODULE(g_pcsAgsmSearch, AgsmSearch);
	REGISTER_MODULE(g_pcsAgsmLog, AgsmLog);
//#ifdef	_PROFILE_
//	REGISTER_MODULE(g_pcsApmProfiler, ApmProfiler);
//#endif	//_PROFILE_
	REGISTER_MODULE(g_pcsAgsmUIStatus, AgsmUIStatus);
	REGISTER_MODULE(g_pcsAgsmReturnToLogin, AgsmReturnToLogin);
	REGISTER_MODULE(g_pcsAgsmWantedCriminal, AgsmWantedCriminal);
	REGISTER_MODULE(g_pcsAgsmMailBox, AgsmMailBox);

	REGISTER_MODULE(g_pcsAgsmCasper, AgsmCasper);

	REGISTER_MODULE(g_pcsAgsmAdmin, AgsmAdmin);

	REGISTER_MODULE(g_pcsAgsmConfig, AgsmConfig);	// kelovon, 20051010

	REGISTER_MODULE(g_pcsAgsmServerStatus, AgsmServerStatus);

	REGISTER_MODULE(g_pcsAgsmBillInfo, AgsmBillInfo);
	REGISTER_MODULE(g_pcsAgsmGK, AgsmGK);

	REGISTER_MODULE(g_pcsAgsmGlobalChatting, AgsmGlobalChatting);
	REGISTER_MODULE(g_pcsAgsmAreaChatting, AgsmAreaChatting);
	
	//REGISTER_MODULE(g_pcsAgsmBilling, AgsmBilling);
	//REGISTER_MODULE(g_pcsAgsmCashMall, AgsmCashMall); //cmb, disabled billing module for now

	REGISTER_MODULE(g_pcsAgsmArchlord, AgsmArchlord);
	
	REGISTER_MODULE(g_pcsAgsmSiegeWar, AgsmSiegeWar);
	REGISTER_MODULE(g_pcsAgsmEventSiegeWarNPC, AgsmEventSiegeWarNPC);

	REGISTER_MODULE(g_pcsAgsmTax, AgsmTax);
	REGISTER_MODULE(g_pcsAgsmGuildWarehouse, AgsmGuildWarehouse);

	REGISTER_MODULE(g_pcsAgsmEventGacha, AgsmEventGacha);

	REGISTER_MODULE(g_pcsAgpmTitle, AgpmTitle);
	REGISTER_MODULE(g_pcsAgsmTitle, AgsmTitle);

	REGISTER_MODULE(g_pcsAgsmStartupEncryption, AgsmStartupEncryption);
	REGISTER_MODULE(g_pcsAgsmRelay2, AgsmRelay2);
	
	REGISTER_MODULE(g_pcsAgpmBattleGround, AgpmBattleGround);
	REGISTER_MODULE(g_pcsAgsmBattleGround, AgsmBattleGround);

#ifdef _AREA_CHINA_
	g_agsmGameholic.Initialize( g_pcsAgpmCharacter, g_pcsAgsmCharacter );
#endif

	REGISTER_MODULE(g_pcsAgsmNpcManager, AgsmNpcManager);
	REGISTER_MODULE(g_pcsAgsmEventSystem, AgsmEventSystem);

	REGISTER_MODULE(g_pcsAgsmEpicZone, AgsmEpicZone);
	REGISTER_MODULE(g_pcsAgpmEpicZone, AgpmEpicZone);

	REGISTER_MODULE(g_pcsAgsmLoginClient, AgsmLoginClient); //JK_중복로그인


	REGISTER_MODULE(g_pcsAgpmMonsterPath, AgpmMonsterPath);

	printf("Processing - 3\n");
	// 스크립트 명령어를 디버그 모드일 때만 등록한다.
	// 모듈 등록 바로 다음에 스크립트 등록 해야 된다.
#ifdef _DEBUG
	RegisterScriptCommand();
#endif

//#ifdef	_PROFILE_
//	g_pcsApmProfiler->SetProfileManager(AuProfileManager::Roots);
//#endif	//_PROFILE_

	//REGISTER_MODULE(g_pcsAgsmPacketMonitor, AgsmPacketMonitor);

	printf("Processing - 4\n");
	g_pcsApmMap->EnableIdle(FALSE);
	g_pcsApmObject->EnableIdle(FALSE);
	g_pcsAgsmCharacter->EnableIdle2(FALSE);

	//g_pcsAgpmFactors->InitMemoryPool(100, 2300000);

	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_RESULT].Initialize(sizeof(AgpdFactor), m_csPoolCountStream.m_lFactorTypeResult, _T("AgpdFactorResult"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_CHAR_STATUS].Initialize(sizeof(AgpdFactorCharStatus), m_csPoolCountStream.m_lFactorTypeCharStatus, _T("AgpdFactorCharStatus"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_CHAR_TYPE].Initialize(sizeof(AgpdFactorCharType), m_csPoolCountStream.m_lFactorTypeCharType, _T("AgpdFactorCharType"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_CHAR_POINT].Initialize(sizeof(AgpdFactorCharPoint), m_csPoolCountStream.m_lFactorTypeCharPoint, _T("AgpdFactorCharPoint"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].Initialize(sizeof(AgpdFactorCharPointMax), m_csPoolCountStream.m_lFactorTypeCharPointMax, _T("AgpdFactorCharPointMax"));
	//g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_CHAR_POINT_RECOVERY_RATE].Initialize(sizeof(AgpdFactorCharPointRecoveryRate), 50000);
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_DAMAGE].Initialize(sizeof(AgpdFactorDamage), m_csPoolCountStream.m_lFactorTypeDamage, _T("AgpdFactorDamage"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_DEFENSE].Initialize(sizeof(AgpdFactorDefense), m_csPoolCountStream.m_lFactorTypeDefense, _T("AgpdFactorDefense"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_ATTACK].Initialize(sizeof(AgpdFactorAttack), m_csPoolCountStream.m_lFactorTypeAttack, _T("AgpdFactorAttack"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_ITEM].Initialize(sizeof(AgpdFactorItem), m_csPoolCountStream.m_lFactorTypeItem, _T("AgpdFactorItem"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_DIRT].Initialize(sizeof(AgpdFactorDIRT), m_csPoolCountStream.m_lFactorTypeDirt, _T("AgpdFactorDIRT"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_PRICE].Initialize(sizeof(AgpdFactorPrice), m_csPoolCountStream.m_lFactorTypePrice, _T("AgpdFactorPrice"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_OWNER].Initialize(sizeof(AgpdFactorOwner), m_csPoolCountStream.m_lFactorTypeOwner, _T("AgpdFactorOwner"));
	g_pcsAgpmFactors->m_csMemoryPoolFactorType[AGPD_FACTORS_TYPE_AGRO].Initialize(sizeof(AgpdFactorAgro), m_csPoolCountStream.m_lFactorTypeAgro, _T("AgpdFactorAgro"));

	g_pcsAgpmFactors->m_csMemoryPoolAgpdFactor.Initialize(sizeof(AgpdFactor), m_csPoolCountStream.m_lAgpdFactor, _T("AgpdFactor"));

	printf("Processing - 5\n");
	g_pcsAgpmSkill->InitMemoryPool(0, m_csPoolCountStream.m_lSkill, _T("AgpdSkill"));
	g_pcsAgpmItem->InitMemoryPool(0, m_csPoolCountStream.m_lItem, _T("AgpdItem"));
	g_pcsAgpmCharacter->InitMemoryPool(0, m_csPoolCountStream.m_lCharacter, _T("AgpdCharacter"));
	g_pcsAgpmGrid->InitMemoryPool(0, m_csPoolCountStream.m_lGridItem, _T("AgpdGridItem"));
	g_pcsAgpmGrid->InitGridMemoryPool(m_csPoolCountStream.m_lGrid);
	g_pcsAgpmAuction->InitMemoryPool(0, 50000, _T("AgpdAuction"));

	//g_pcsAgpmPvP->m_csMemoryPoolAdmin.Initialize(sizeof(AgpmPvPArray), m_csPoolCountStream.m_lPvPArray);
	//g_pcsAgpmPvP->m_csMemoryPoolCharInfo.Initialize(sizeof(AgpdPvPCharInfo), m_csPoolCountStream.m_lPvPCharInfo);
	//g_pcsAgpmPvP->m_csMemoryPoolGuildInfo.Initialize(sizeof(AgpdPvPGuildInfo), m_csPoolCountStream.m_lPvPGuildInfo);

	printf("Processing - 6\n");
	// 2005.03.31. steeple
	// Server Status IOCP Server Start
	g_pcsAgsmServerStatus->SetServerTypeGameServer();
	g_pcsAgsmServerStatus->m_pServerMainClass = this;
	g_pcsAgsmServerStatus->m_pfServerShutdown = Shutdown;
	g_pcsAgsmServerStatus->Start();

	printf("Processing - 7\n");
	g_pcsApmMap->m_csMemoryPool.Initialize(sizeof(ApWorldSector::IdPos), m_csPoolCountStream.m_lMapIdPos, _T("ApWorldSector::IdPos"));

	// module initialize
	g_pcsApmMap->SetLoadingMode	( TRUE , FALSE	);
	g_pcsApmMap->SetAutoLoadData	( FALSE			);


	printf("Processing - 8\n");
	if( !g_pcsApmMap->Init( NULL, ".\\Map\\Data\\Moonee", ".\\Map\\Data\\Compact", ".\\World" ) )
		return FALSE;

	printf("Processing - 9\n");
	// 마고자 (2006-03-08 오전 11:47:57) : 
	// 두번째 데이타가 서버데이타 읽기..
	// 확인 안해서 제대로 될지 -_-;; 태희야 사랑해
	g_pcsApmMap->LoadAll( TRUE , TRUE );
	g_pcsApmMap->LoadTemplate("Ini\\RegionTemplate.ini", ENC_SVR_DATA);

	g_pcsApmObject->		SetMaxObject			( 500000	);
	g_pcsApmObject->		SetMaxObjectTemplate	( 10000	);

	g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 2000	);
	g_pcsAgpmCharacter->	SetMaxCharacter			( 50000	);
	g_pcsAgpmCharacter->	SetMaxCharacterRemove	( 50000	);
	g_pcsAgpmCharacter->	SetRegionRefreshInterval( 3000	);
	//g_pcsAgpmCharacter->	SetMaxIdleEvent			( 100	);
	//g_csAgpmCharacter.	SetIdleIntervalMSec		( 200	);

	g_pcsAgpmItem->		SetMaxItem				( 3000000	);
	g_pcsAgpmItem->		SetMaxItemRemove		( 3000000	);

	g_pcsAgpmParty->		SetMaxParty				( 2000	);
	g_pcsAgpmParty->		SetMaxPartyRemove		( 2000	);

	g_pcsAgpmSkill->		SetMaxSkill				( 300000	);
	g_pcsAgpmSkill->		SetMaxSkillTemplate		( 3000	);
	g_pcsAgpmSkill->		SetMaxSkillSpecializeTemplate	( 10	);
	g_pcsAgpmSkill->		SetMaxSkillRemove		( 500000	);

	g_pcsAgpmChannel->		SetMaxChannel			(5000);
	
	g_pcsAgpmMailBox->		SetMaxMail				(500000);

	//g_pcsAgsmAOIFilter->	SetMaxGroupCount		( 20000	);

	//g_csAgsmCharManager.SetMaxAccount(10);
	//g_csAgsmCharManager.InitServer(1, 0, 0);
	g_pcsAgsmCharacter->	SetMaxCheckRecvChar		( 3000	);
	g_pcsAgsmCharacter->	SetMaxWaitForRemoveChar	( 3000	);
	g_pcsAgsmCharacter->	SetIOCPDispatcherUse	( TRUE	);

	//g_csAgsmItem.		SetMaxIdleEvent			( 4000	);
	g_pcsAgsmItem->		SetMaxFieldItem			( 50000	);
	g_pcsAgsmItem->		SetMaxDBIDItem			( 3000000);

	g_pcsAgsmAccountManager->	SetMaxAccount	( 20000		);

	g_pcsAgpmEventBinding->	SetMaxBindingData	( 100	);

	//g_csAgsmItemManager.	SetMaxAccount	( 20000		);

	//g_csAgsmDeath.			SetMaxHitHistory( 20		);
	//g_pcsAgsmDeath->			SetMaxIdleEvent2	( 4000		);

	g_pcsAgsmServerManager->	SetMaxServer	( AGSMSERVER_MAX_NUM_SERVER );
  
	//g_pcsAgsmServerManager->	SetMaxServer	( 20		);
	//g_pcsAgsmServerManager->	SetMaxTemplate	( 100		);
  
	g_pcsAgsmServerManager->	SetMaxIdleEvent2	( 100		);

	g_pcsAgpmQuest->SetMaxTemplateCount(AGPDQUEST_MAX_QUEST_TEMPLATE * 2);
	g_pcsAgpmQuest->SetMaxGroupCount(AGPDQUEST_MAX_QUEST * 2);

	//g_pcsAgsmConnectionSetupGame->SetMaxIdleEvent2(15);
	g_pcsAgsmInterServerLink->SetMaxIdleEvent2(15);

	// Event module setting
	g_pcsApmEventManager->	SetMaxEvent		( 30000		);

	g_pcsAgpmEventTeleport->	SetMaxTeleportPoint	( 300	);
	g_pcsAgpmEventTeleport->	SetMaxTeleportGroup	( 300	);

	// 2004.06.23. steeple
	g_pcsAgpmGuild->SetMaxGuild( 50000	);
	g_pcsAgpmGuild->SetMaxBattlePointAdmin( 100 );

	// AgpmProduct admin
	g_pcsAgpmProduct->SetMaxSkillFactor(10);
	g_pcsAgpmProduct->SetMaxComposeTemplate(300);
	
	// AgsmProduct admin
	g_pcsAgsmProduct->SetMaxItemSet(2000);

	//g_pcsAgpmRefinery->SetMaxRefineTemplate(100);
	g_pcsAgpmRefinery->SetMaxRefineItem(10000);

	// 2004.12.22. steeple
	g_pcsAgpmPvP->SetMaxAreaDropAdmin(5);
	g_pcsAgpmPvP->SetMaxItemDropAdmin(30);
	g_pcsAgpmPvP->SetMaxSkullDropAdmin(100);
	
	g_pcsAgpmAuction->SetMaxSales(1000000);

	g_pcsAgpmWantedCriminal->SetMaxWantedCriminal(100000);
	
	g_pcsAgpmOptimizedPacket2->SetMaxMemoryPoolCount(m_csPoolCountStream.m_lOptimizedPacket);

	g_pcsAgsmAreaChatting->SetMaxRegionChar(4000);
	g_pcsAgsmAreaChatting->SetMaxClient(4000);

	g_pcsAgpmTitle->SetMaxTitleTemplate(20000);

	printf("Processing - 10\n");
	// module start
	if (!Initialize())
		return FALSE;

	printf("Processing - 11\n");
	if (!g_pcsAgsmInterServerLink->SetCallbackDisconnect(CBDisconnectServer, this))
		return FALSE;

	printf("Processing - 12\n");
	if (!g_pcsAgsmInterServerLink->SetCallbackReplyFail(CBReplyFailFromRelay, this))
		return FALSE;

	if (!g_pcsAgsmServerManager->ReadServerInfo())
	{
		ASSERT( !"g_csAgsmServerManager StreamReadServerInfo 실패" );
		return FALSE;
	}

	printf("Processing - 16\n");
	if (!g_pcsAgsmServerManager->InitThisServer())
	{
		ASSERT( !"g_csAgsmServerManager InitThisServer 실패" );
		return FALSE;
	}

	printf("Processing - 17\n");
	// 서버 config loading
	if (!g_pcsAgsmConfig->LoadConfig())
	{
		ASSERT( !"g_pcsAgsmConfig LoadConfig 실패" );
		return FALSE;
	}

#ifdef _AREA_KOREA_

	printf("Processing - 17-1\n");
	// 서버 config 뒤에 TPack관련 config loading
	if (!g_pcsAgsmConfig->LoadTPackConfig())
	{
		ASSERT( !"g_pcsAgsmConfig LoadTPackConfig 실패");
		return FALSE;
	}

#endif

	printf("Processing - 18\n");
	// 2006.06.14. steeple
	// 알고리즘 초기화 여기서 한다.
	//AuPacket::GetCryptManager().SetUseCrypt(FALSE);		// 얘의 주석을 풀면 암호화 하지 않는다.
	g_pcsAgsmStartupEncryption->SetAlgorithm();
	g_pcsAgsmStartupEncryption->InitializePublicKey();

	printf("Processing - 18 - 1\n");
	// INI Directory 할당
	ApDirString szIniDir = g_pcsAgpmConfig->GetIniDir();
	//ApDirString szIniDir = "INI";

	// Log
	if (!g_pcsAgpmLog->StreamReadFilter("Ini\\FilteredLog.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmLog StreamReadFilter 실패" );
		return FALSE;
	}	

	printf("Processing - 19\n");
	//Teleport
	if (!g_pcsAgpmEventTeleport-> StreamReadGroup("Ini\\TeleportGroup.ini", ENC_SVR_DATA			)	)
	{
		ASSERT( !"g_csAgpmEventTeleport StreamReadGroup 실패" );
		return FALSE;
	}

	printf("Processing - 20\n");
	if (!g_pcsAgpmEventTeleport-> StreamReadPoint("Ini\\TeleportPoint.ini", ENC_SVR_DATA			)	)
	{
		ASSERT( !"g_csAgpmEventTeleport StreamReadGroup 실패" );
		return FALSE;
	}

	printf("Processing - 21\n");
	if (!g_pcsAgpmEventTeleport-> StreamReadFee(szIniDir + "\\TeleportFee.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmEventTeleport StreamReadFee 실패" );
		return FALSE;
	}

	printf("Processing - 22\n");
	//Spawn
	if (!g_pcsAgpmEventSpawn->	StreamReadGroup("Ini\\SpawnGroup.ini"))
	{
		ASSERT( !"g_csAgpmEventSpawn StreamReadGroup 실패" );
		return FALSE;
	}

	printf("Processing - 23\n");
	if (!g_pcsApmObject->			StreamReadTemplate("Ini\\ObjectTemplate.ini", NULL, NULL, NULL, ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csApmObject StreamReadTemplate 실패" );
		return FALSE;
	}

	printf("Processing - 24\n");
	if (!g_pcsApmObject->			StreamRead("Ini\\ObjectStatic.ini"		)	)
	{
		ASSERT( !"g_csApmObject StreamRead(ObjectStatic) 실패" );
		return FALSE;
	}

	printf("Processing - 25\n");
	if (!g_pcsApmObject->			StreamReadAllDivisionVersion2())
	{
		ASSERT( !"g_csApmObject StreamRead(Object) 실패" );
		return FALSE;
	}

	printf("Processing - 26\n");
	if (!g_pcsAgpmSkill->			StreamReadTemplate("Ini\\SkillTemplate.ini", NULL, ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadTemplate 실패" );
		return FALSE;
	}

	printf("Processing - 27\n");
	if (!g_pcsAgpmSkill->			StreamReadTemplate("Ini\\SkillTemplate_Internal.ini", NULL, ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadTemplate 실패" );
		return FALSE;
	}

	printf("Processing - 28\n");
	if (!g_pcsAgpmFactors->		CharacterTypeStreamRead("Ini\\CharType.ini"	, ENC_SVR_DATA )	)
	{
		ASSERT( !"g_csAgpmFactors CharacterTypeStreamRead 실패" );
		return FALSE;
	}

	printf("Processing - 29\n");
	if (!g_pcsAgpmCharacter->		StreamReadTemplate("Ini\\CharacterTemplatePublic.ini"	, FALSE, ENC_SVR_DATA	)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadTemplate 실패" );
		return FALSE;
	}

	printf("Processing - 30\n");
	if (!g_pcsAgpmCharacter->		StreamReadLevelUpExpTxt("Ini\\LevelUpExp.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadLevelUpExpTxt 실패" );
		return FALSE;
	}

	printf("Processing - 30-1\n");
	if (!g_pcsAgpmCharacter->		StreamReadDownExpPerLevelTxt("Ini\\DeathPenaltyExp.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadDownExpPerLevelTxt 실패" );
		return FALSE;
	}

	printf("Processing - 31\n");
	if (!g_pcsAgpmCharacter->		StreamReadCharGrowUpTxt("Ini\\GrowUpFactor.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharGrowUpTxt 실패" );
		return FALSE;
	}

	printf("Processing - 32\n");
	if (!g_pcsAgpmCharacter->		StreamReadCharKind("Ini\\CharKind.ini"				)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharKind 실패" );
		return FALSE;
	}

	//g_pcsAgpmItem->SetRemovePolearm();

	printf("Processing - 33\n");
	if (!g_pcsAgpmItem->			StreamReadTemplate("ini\\ItemTemplateAll.ini", NULL, ENC_SVR_DATA)	)
	{
		if (!g_pcsAgpmItem->			StreamReadTemplates("Ini\\ItemTemplate", "ini\\ItemTemplateEntry.ini", NULL, ENC_SVR_DATA)	)
		{
			ASSERT( !"g_csAgpmItem StreamReadTemplate 실패" );
			return FALSE;
		}
	}

	printf("Processing - 34\n");
	if (!g_pcsAgpmItem->			StreamReadBankSlotPrice(szIniDir + "\\BankSlotPrice.txt", ENC_SVR_DATA )	)
	{
		ASSERT( !"g_csAgpmItem StreamReadBankSlotPrice 실패" );
		return FALSE;
	}

	printf("Processing - 35\n");
	if (!g_pcsAgpmDropItem2->ReadDropGroup(szIniDir + "\\GroupDropRate.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmDropItem2 ReadDropGroup 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmDropItem2->ReadDropGroup2(szIniDir + "\\ItemDropTable2.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmDropItem2 ReadDropGroup2 실패" );
		return FALSE;
	}
	
	printf("Processing - 36\n");
	if (!g_pcsAgpmDropItem2->ReadDropRankRate("Ini\\DropRankRate.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmDropItem2 ReadDropGroup 실패" );
		return FALSE;
	}

	printf("Processing - 37\n");
	if (!g_pcsAgpmDropItem2->ReadDropOption("Ini\\OptionNumDropRate.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmDropItem2 ReadDropGroup 실패" );
		return FALSE;
	}

	printf("Processing - 38\n");
	if (!g_pcsAgpmDropItem2->ReadDropSocket("Ini\\SocketNumDropRate.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmDropItem2 ReadDropGroup 실패" );
		return FALSE;
	}

	printf("Processing - 39\n");
	if (!g_pcsAgpmItemConvert->		StreamReadConvertTable(szIniDir + "\\ItemConvertTable.txt", ENC_SVR_DATA )	)
	{
		ASSERT( !"g_pcsAgpmItemConvert StreamReadConvertTable() 실패" );
		return FALSE;
	}

	printf("Processing - 40\n");
	if (!g_pcsAgpmItemConvert->		StreamReadRuneAttribute(szIniDir + "\\ItemRuneAttributeTable.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_pcsAgpmItemConvert StreamReadRuneAttribute() 실패" );
		return FALSE;
	}

	printf("Processing - 41\n");

	CHAR	szBuffer[512];
	if (!g_pcsAgpmCharacter->		StreamReadImportData(szIniDir + "\\CharacterDataTable.txt", szBuffer, ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadImportData 실패" );
		return FALSE;
	}

	printf("Processing - 42\n");

	if (!g_pcsAgpmItem->			StreamReadImportData(szIniDir + "\\ItemDataTable.txt", szBuffer, ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadImportData 실패" );
		return FALSE;
	}

	printf("Processing - 42.5\n");

	if( g_pcsAgpmEventGacha )
	{
		g_pcsAgpmEventGacha->StreamReadGachaTypeTable( "./Ini/GachaTypeTable.txt" , ENC_SVR_DATA);
		g_pcsAgpmEventGacha->BuildGachaItemTable();
		g_pcsAgpmEventGacha->ReportGachaTable();
	}

	printf("Processing - 43\n");

	if (!g_pcsAgpmItem->StreamReadTransformData("Ini\\ItemTransformTable.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadTransformData 실패" );
		return FALSE;
	}

	printf("Processing - 44\n");

	if (!g_pcsAgpmItem->StreamReadOptionData(szIniDir + "\\ItemOptionTable.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadOptionData 실패" );
		return FALSE;
	}

	printf("Processing - 44.5\n");
	if (!g_pcsAgpmItem->EnumCallback(ITEM_CB_ID_STREAM_READ_OPTION_TABLE, NULL, NULL)) // 게임서버만 Drop Option Rate를 계산... arycoat 2009.7.2
	{
		ASSERT( !"g_csAgpmCharacter StreamReadOptionData 실패2" );
		return FALSE;
	}

	printf("Processing - 45\n");

	if (!g_pcsAgpmItem->StreamReadLotteryBox(szIniDir + "\\ItemLotteryBox.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmItem StreamReadLotteryBox 실패" );
		return FALSE;
	}

	printf("Processing - 45.1\n");

	if (!g_pcsAgpmItem->StreamReadAvatarSet(szIniDir + "\\avatarset.ini", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmItem StreamReadAvatarSet 실패" );
		return FALSE;
	}

	printf("Processing - 46\n");

	if (!g_pcsAgpmSkill->			ReadSkillSpecTxt("Ini\\Skill_Spec.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillSpecTxt 실패" );
		return FALSE;
	}

	printf("Processing - 47\n");

	if (!g_pcsAgpmSkill->			ReadSkillConstTxt("Ini\\Skill_Const.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillConstTxt 실패" );
		return FALSE;
	}

	printf("Processing - 47.1\n");

	if (!g_pcsAgpmSkill->			ReadSkillConst2Txt("Ini\\Skill_Const2.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillConstTxt 실패" );
		return FALSE;
	}

	printf("Processing - 48\n");

	if (!g_pcsAgpmSkill->			ReadSkillConstTxt("Ini\\Skill_Const_Internal.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillConstTxt 실패" );
		return FALSE;
	}

	printf("Processing - 49\n");

	//	if (!g_pcsAgpmSkill->			ReadSkillMasteryTxt("Ini\\Skill_Mastery.txt", FALSE		)	)
//	{
//		ASSERT( !"g_csAgpmSkill ReadSkillMasteryTxt 실패" );
//		return FALSE;
//	}

	printf("Processing - 50\n");

	if (!g_pcsAgpmSkill->			StreamReadSpecialize("Ini\\SkillSpecialization.ini", FALSE		)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadSpecialize 실패" );
		return FALSE;
	}

	printf("Processing - 51\n");

	if (!g_pcsAgpmEventSkillMaster->StreamReadMasteryTxt("Ini\\SkillMastery.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmSkill StreamReadMasteryTxt 실패" );
		return FALSE;
	}

	printf("Processing - 51-1\n");

	if (!g_pcsAgpmEventSkillMaster->StreamReadHighLevelSkill("Ini\\SkillMastery_High.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmSkill StreamReadMasteryTxt 실패" );
		return FALSE;
	}

	printf("Processing - 51-2\n");

	if (!g_pcsAgpmEventSkillMaster->StreamReadMasteryEvolution("Ini\\SkillMasteryEvolution.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmSkill StreamReadMasteryTxt 실패" );
		return FALSE;
	}

	printf("Processing - 51-3\n");

	if (!g_pcsAgpmEventSkillMaster->StreamReadHeroicSkill("Ini\\SkillMastery_Heroic.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmSkill StreamReadMasteryTxt 실패" );
		return FALSE;
	}

	printf("Processing - 52\n");

	if (!g_pcsAgpmProduct->StreamReadCategory("Ini\\ProductCategory.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmProduct StreamRead(ProductCategory.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 53\n");

	if (!g_pcsAgpmProduct->StreamReadCompose("Ini\\ProductCompose.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmProduct StreamRead(ProductCompose.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 54\n");

	if (!g_pcsAgpmProduct->StreamReadFactor("Ini\\ProductExp.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmProduct StreamRead(ProductExp.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 55\n");

	if (!g_pcsAgpmProduct->StreamReadGatherCharacterTAD("Ini\\ProductGather.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmProduct StreamRead(ProductGather.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 56\n");

	if (!g_pcsAgsmProduct->StreamReadProductItemSet("Ini\\ProductResultItem.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgsmProduct StreamRead(ProductResultItem.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 58\n");
	if (!g_pcsAgpmRefinery->StreamReadRefineItem2(szIniDir + "\\RefineryItem2.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmRefinery StreamRead(RefineryItem2.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 62\n");

	if (!g_pcsAgpmGuild->ReadBattlePointTxt("Ini\\GuildBattlePoint.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmGuild ReadBattlePointTxt(GuildBattlePoint.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 63\n");

	if (!g_pcsAgpmPvP->ReadItemDropTxt("Ini\\PVPItemDropTable.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmPvP ReadItemDropTxt(PVPItemDropTable.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 64\n");

	if (!g_pcsAgpmPvP->ReadSkullDropTxt("Ini\\PvPSkullDropTable.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmPvP ReadSkullDropTxt(PvPSkullDropTable.txt) 실패" );
		return FALSE;
	}


	printf("Processing - 65 - 1\n");

	if (!g_pcsAgsmPvP->StreamReadItemDropRate("Ini\\PVPItemDropRate.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgsmPvP StreamReadItemDropRate(PVPItemDropRate.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 65 - 2\n");

	if (!g_pcsAgsmPvP->StreamReadCharismaPoint("Ini\\CharismaPoint.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgsmPvP StreamReadItemDropRate(PVPItemDropRate.txt) 실패" );
		return FALSE;
	}

	printf("Processing - 65 - 3\n");

	if (!g_pcsAgsmPvP->StreamReadRaceBattleStatus("Ini\\RaceBattle.ini"))
	{
		ASSERT( !"g_pcsAgsmPvP StreamWriteRaceBattleStatus(RaceBattle.ini) 실패" );
		return FALSE;
	}
	printf("Processing - 66\n");

	if( !g_pcsAgpmAI2->LoadMonsterAITemplate( "Ini\\MonsterAI.txt" ) )
	{
		ASSERT( !"g_csAgpmAI2 ReadTemplate 실패" );
		return FALSE;
	}
/*
	printf("Processing - 66-2\n");

	if(!g_pcsAgpmMonsterPath->LoadPath("ini\\MonsterPath"))
	{
		ASSERT( !"g_pcsAgpmMonsterPath ReadTemplate 실패");
		return FALSE;
	}
*/
	printf("Processing - 67\n");

	if (!g_pcsAgpmQuest->StreamReadTemplate(szIniDir + "\\QuestTemplate.ini", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmQuest StreamReadTemplate 실패" );
		return FALSE;
	}

	printf("Processing - 68\n");

	if (!g_pcsAgpmQuest->StreamReadGroup("Ini\\QuestGroup.ini", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmQuest StreamReadGroup 실패" );
		return FALSE;
	}

	printf("Processing - 69\n");

	if (!g_pcsAgpmEventCharCustomize->StreamReadCustomizeList(szIniDir + "\\CharacterCustomizeList.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmEventCharCustomize StreamReadCustomizeList 실패" );
		return FALSE;
	}
	printf("Processing - 70\n");

	if (!g_pcsAgpmGuild->ReadRequireItemIncreaseMaxMember(szIniDir + "\\GuildMaxMember.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmGuild ReadRequireItemIncreaseMaxMember 실패" );
		return FALSE;
	}
	printf("Processing - 71\n");

	if (!g_pcsAgpmCharacter->StreamReadChatFilterText("Ini\\Base.dat", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadChatFilterText 실패" );
		return FALSE;
	}
	printf("Processing - 72\n");

	if (!g_pcsAgpmGuild->ReadGuildMarkTemplate(szIniDir + "\\guildmark.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmGuild ReadGuildMarkTemplate 실패" );
		//return FALSE;		
	}

	printf("Processing - 73\n");

	if (!g_pcsAgsmRelay2->EnableBufferingSend())
	{
		ASSERT( !"g_pcsAgsmRelay2 EnableBufferingSend() 실패" );
		return FALSE;
	}

	printf("Processing - 74\n");

	// 아뒤 앞에 붙일 서버플래그를 가져와서 필요 모듈들에 세팅한다.
	INT16	nServerFlag = 0;
	INT16	nFlagSize	= 0;
	g_pcsAgsmCharManager->	InitServer		( 1, nServerFlag, nFlagSize, 15000	);
	g_pcsAgsmItemManager->	InitServer		( 1, nServerFlag, nFlagSize, 500000	);
	g_pcsAgsmSkillManager->	InitServer		( 1, nServerFlag, nFlagSize, 300000	);
	g_pcsAgsmParty->		InitServer		( 1, nServerFlag, nFlagSize, 2000	);
	g_pcsAgsmAdmin->InitServer(1, nServerFlag, nFlagSize);

	// 게임 시간을 세팅한다.
	// 추후 개념이 발전하여 년도가 들어가게 되면 이 내용도 월드별로 DB에 저장해야 한다.
	g_pcsAgpmEventNature->SetSpeedRate(6);
	g_pcsAgpmEventNature->SetTime(11, 0, 0, NULL);

	if (!InitializeDBID())
		return FALSE;

	if (!g_pcsAgpmCharacter->StreamReadStaticCharacter(".\\Ini\\NPC.ini"))
	{
		ASSERT( !".\\Ini\\NPC.ini" );
	}

	printf("Processing - 75\n");

	if( !g_pcsAgpmEventNPCTrade->LoadNPCTradeRes(szIniDir + "\\NPCTradeItemList.txt", ENC_SVR_DATA) )
	{
		ASSERT( !".\\Ini\\NPCTradeItemList.txt" );
		return FALSE;
	}	

	printf("Processing - 76\n");

	if( !g_pcsAgpmEventNPCDialog->LoadMobDialogRes( ".\\Ini\\MobDialog.txt", ENC_SVR_DATA) )
	{
		ASSERT( !".\\Ini\\MobDialog.txt" );
		return FALSE;
	}

	printf("Processing - 77\n");

	// 리젼정보 읽어들이기
	if( !g_pcsApmMap->LoadTemplate( ".\\ini\\RegionTemplate.ini", ENC_SVR_DATA ) )
	{
		ASSERT( !".\\Ini\\RegionTemplate.ini" );
		return FALSE;
	}

	// 리젼특성정보 읽어들이기
	if(!g_pcsApmMap->LoadRegionPerculiarity( ENC_SVR_DATA ))
	{
		ASSERT( !".\\Ini\\RegionPerculiarity.xml" );
		return FALSE;
	}

	printf("Processing - 78\n");

	//NPCTrade Item초기화.
	g_pcsAgsmEventNPCTrade->SetEventNPCTradeItem();

	g_pcsAgpmEventSpawn->LoadSpawnInfoFromExcel( "ini\\spawnExcelData.txt" );

	g_pcsAgpmCashMall->SetMaxProduct(1000);
	if (!g_pcsAgpmCashMall->StreamReadCashMallTab(szIniDir + "\\CashMallTab.txt", ENC_SVR_DATA))
	{
		ASSERT( !".\\Ini\\CashMallTab.txt" );
		return FALSE;
	}

	printf("Processing - 79\n");

	if (!g_pcsAgpmCashMall->StreamReadCashMallItemList("ini\\CashMallItemList.txt", ENC_SVR_DATA))
	{
		ASSERT( !".\\Ini\\CashMallItemList.txt" );
		return FALSE;
	}

	printf("Processing - 80\n");

	if (!g_pcsAgsmChatting->StreamReadChatEvent("ini\\chatevent.txt"))
	{
		ASSERT( !".\\Ini\\chatevent.txt" );
		return FALSE;
	}

	printf("Processing - 81\n");

	if (!g_pcsAgpmSiegeWar->ReadSiegeInfo("ini\\SiegeWar.txt", ENC_SVR_DATA))
	{
		ASSERT( !".\\Ini\\SiegeWar.txt" );
		return FALSE;
	}

	if (!g_pcsAgpmSiegeWar->ReadSiegeWarObjectInfo("ini\\SiegeWarObject.txt", ENC_SVR_DATA))
	{
		ASSERT( !".\\Ini\\SiegeWarObject.txt" );
		return FALSE;
	}

	printf("Processing - 82\n");

	if (!g_pcsAgsmEventSpawn->ReadEventEffectTxt("ini\\EventItemEffect.txt", ENC_SVR_DATA))
	{
		ASSERT( !".\\ini\\EventItemEffect.txt" );
		return FALSE;
	}

	printf("Processing - 83\n");

	if(!g_pcsAgpmTitle->StreamReadTitleDataTemplate("ini\\titledatatable.txt",FALSE))
	{
		ASSERT(!".\\ini\\titledatatable.txt");
		return FALSE;
	}

	// 로딩 끝나고 하는 작업 처리하자
#ifdef _AREA_JAPAN_
	g_pcsAgsmBilling->SetOrderSeed();
#endif
	g_pcsAgsmGuild->SetEventGuildLoadComplete(m_hEventGuildLoadComplete);

	printf("Processing - 84\n");

	g_pcsAgsmCharManager->InitPCRoomModule();

	printf("Processing - 85\n");

	if(!g_pcsAgpmBuddy->ReadMentorConstrinctFile("Ini\\BuddyConstrict.xml"))
	{
		ASSERT(!".\\Ini\\BuddyConstrict.xml");
		return FALSE;
	}
	

	g_pcsAgsmAdmin->LoadAllowedAdminIPList();
	
	//////////////////////////////////////////////////////////////////////////
	// Set Enable Idles
	_beginthreadex(0, 0, ServerEngine::TimerThreadRoutine, this, 0, 0);
	g_pcsAgsmCharacter->EnableIdle3(TRUE);

#ifdef APMEMORY_CHECKER
	ApMemoryChecker::getInstance().setCountLimit(1024 * 1024 * 4);
#endif
#ifdef _AREA_JAPAN_
	if(FALSE == AuGameEnv().IsAlpha())
	{
		printf("Initialize GameGuard module...\n");
		if ( g_nProtect.Init() )
			printf("Success Initialize GameGuard module\n");
		else
			printf("Fail Initialize GameGuard module\n");
	}
#endif
	return TRUE;
}

unsigned int ServerEngine::TimerThreadRoutine( void* pArg )
{
	ServerEngine* pServer = (ServerEngine*)pArg;

	DWORD	     dwCurTime = GetTickCount();
	static DWORD dwLastTime = GetTickCount();

	while (TRUE) 
	{
		if( DWORD(dwCurTime - dwLastTime) >= 100 )
		{
			g_pcsAgsmConfig->OnTimer(dwCurTime);
			g_pcsAgsmBattleGround->OnTimer(dwCurTime);
			g_pcsAgsmEpicZone->OnTimer(dwCurTime);
			
			dwLastTime = dwCurTime; 
		}

		Sleep(10);
		dwCurTime = GetTickCount();
	}
	return 0;
}

BOOL ServerEngine::RegisterScriptCommand()
{
	//AGPMSCRIPT_REGISTER_TYPE_BEGIN(AgpmScript, AgpmScript, g_pcsAgpmScript);
	//	AGPMSCRIPT_REGISTER_METHOD0(void, TestMessage);
	//AGPMSCRIPT_REGISTER_TYPE_END;

	//AGPMSCRIPT_REGISTER_TYPE_BEGIN(AgsmAI2, AgsmAI2, g_pcsAgsmAI2);
	//	AGPMSCRIPT_REGISTER_METHOD1(void, SetSummonAOIRange, int);
	//	AGPMSCRIPT_REGISTER_METHOD2(void, SetSummonPropensity, int, int);
	//AGPMSCRIPT_REGISTER_TYPE_END;

	//AGPMSCRIPT_REGISTER_TYPE_BEGIN(AgsmCombat, AgsmCombat, g_pcsAgsmCombat);
	//	AGPMSCRIPT_REGISTER_METHOD2(void, KillMonster, int, int);
	//AGPMSCRIPT_REGISTER_TYPE_END;
	//
	//AGPMSCRIPT_REGISTER_TYPE_BEGIN(AgsmBilling, AgsmBilling, g_pcsAgsmBilling);
	//	AGPMSCRIPT_REGISTER_METHOD0(void, BillingSvrDisconnect);
	//AGPMSCRIPT_REGISTER_TYPE_END;

	return TRUE;
}

BOOL ServerEngine::SaveAllCharacterData()
{
	g_pcsAgsmCharacter->SaveAllCharacterData();

	g_pcsAgsmRelay2->SendAll();

	return TRUE;
}

BOOL ServerEngine::RemoveUsers()
{
	return g_pcsAgpmCharacter->RemoveAllCharacters();
}

BOOL ServerEngine::OnTerminate()
{
	g_pcsAgpmCharacter->RemoveAllCharacters();

	return Destroy();
}

BOOL ServerEngine::ConnectRelayServer()
{
	AgsdServer2	*pcsServer	= g_pcsAgsmServerManager->GetRelayServer();
	if (!pcsServer)
		return FALSE;

	g_pcsAgsmInterServerLink->ConnectRelayServer();

	while (!pcsServer->m_bIsConnected || !pcsServer->m_bIsReplyConnect)
	{
		Sleep(1000);
	}

	return TRUE;
}

BOOL ServerEngine::ConnectLKServer()
{
	AgsmGKReturn	eReturn	= g_pcsAgsmGK->ConnectLKServer();
	if (eReturn == AGSMGK_RETURN_NOT_EXIST_LKSERVER)
		return TRUE;
	else if (eReturn == AGSMGK_RETURN_CONNECT_FAIL)
		return FALSE;

	return g_pcsAgsmGK->WaitForConnectServer();
}

BOOL ServerEngine::ConnectBillingServer()
{
	eAGSMBILLING_CONNECT_RESULT eResult = g_pcsAgsmBilling->ConnectBillingServer();
	if (eResult == AGSMBILLING_CONNECT_RESULT_NOT_EXIST_SERVER)
		return TRUE;
	else if (eResult == AGSMBILLING_CONNECT_RESULT_FAIL)
		return FALSE;
	else if(eResult == AGSMBILLING_CONNECT_RESULT_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL ServerEngine::ConnectServers()
{
	return g_pcsAgsmInterServerLink->ConnectAll(TRUE);
}

BOOL ServerEngine::DisconnectServers()
{
	g_pcsAgsmInterServerLink->DisconnectAll();
	return TRUE;
}

BOOL ServerEngine::InitializeDBID()
{
	AgsdServer2 *pcsServer = g_pcsAgsmServerManager->GetThisServer();
	if (!pcsServer)
		return FALSE;

	INT32 ServerIndex = pcsServer->m_lServerID;

	g_pcsAgsmItemManager->InitItemDBIDServer(ServerIndex);

	return TRUE;
}

BOOL ServerEngine::SetGuildLoadEvent()
{
	if(!g_pcsAgsmGuild)
		return FALSE;

	g_pcsAgsmGuild->SetEventGuildLoadComplete(m_hEventGuildLoadComplete);
	return TRUE;
}

BOOL ServerEngine::WaitForGuildLoadComplete()
{
	if(!g_pcsAgsmGuild)
		return FALSE;

	if(WaitForSingleObject(m_hEventGuildLoadComplete, INFINITE) == WAIT_FAILED)
		return FALSE;

	printf("\n[ServerEngine] Wait End~~~~~\n");

	return TRUE;
}

BOOL ServerEngine::LoadGuildInfoFromDB()
{
	if(g_pcsAgsmGuild)
		g_pcsAgsmGuild->LoadAllGuildInfo();

	return TRUE;
}

BOOL ServerEngine::LoadSiegeInfoFromDB()
{
	if(g_pcsAgsmSiegeWar)
		g_pcsAgsmSiegeWar->LoadFromDB();

	return TRUE;
}

void ServerEngine::PrintCurrentUserCount()
{
	if(!g_pcsAgsmCharacter)
		return;

	INT32 lCount = g_pcsAgsmCharacter->GetNumOfPlayers();
	printf("\tNumber of players : %d\n\n", lCount);
}

void ServerEngine::PrintGuildInfo()
{
	if(g_pcsAgpmGuild)
		g_pcsAgpmGuild->PrintGuildInfo();
}

BOOL ServerEngine::CBDisconnectServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ServerEngine	*pThis		= (ServerEngine *)	pClass;
	AgsdServer		*pcsServer	= (AgsdServer *)	pData;

	AgsdServer	*pcsThisServer	= g_pcsAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - %s", __FUNCTION__, __LINE__, pcsServer->m_szIP);
	AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

	//if (strcmp(pcsThisServer->m_szGroup, pcsServer->m_szGroup) != 0)
	//	return FALSE;

	AgsdServer	*pcsRelayServer	= g_pcsAgsmServerManager->GetRelayServer();
	if (!pcsRelayServer)
		return FALSE;

	if (pcsServer != pcsRelayServer)
		return TRUE;
	
	pThis->m_AsIOCPServer.m_csTimer.AddTimer(10000, pcsServer->m_lServerID, (PVOID) pThis, CheckRelayStatus, NULL);

	return TRUE;
}

BOOL ServerEngine::CBReplyFailFromRelay(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ServerEngine	*pThis		= (ServerEngine *)	pClass;
	AgsdServer		*pcsServer	= (AgsdServer *)	pData;

	AgsdServer *pcsRelayServer = g_pcsAgsmServerManager->GetRelayServer();
	if (!pcsRelayServer)
		return FALSE;

	if (pcsServer != pcsRelayServer)
		return TRUE;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d]", __FUNCTION__, __LINE__);
	AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

	pThis->m_AsIOCPServer.m_csTimer.AddTimer(10000, pcsRelayServer->m_lServerID, (PVOID) pThis, Shutdown, NULL);

	return TRUE;
}

BOOL ServerEngine::CheckRelayStatus(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass)
		return FALSE;

	ServerEngine	*pThis		= (ServerEngine *)	pClass;

	AgsdServer *pcsRelayServer = g_pcsAgsmServerManager->GetRelayServer();
	if (!pcsRelayServer)
		return FALSE;

	if (!pcsRelayServer->m_bIsConnected ||
		!pcsRelayServer->m_bIsReplyConnect)
	{
		if (pThis->m_lRelayCheckCount > RELAY_CHECK_COUNT)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "m_lRelayCheckCount > RELAY_CHECK_COUNT");
			AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

			pThis->m_AsIOCPServer.m_csTimer.AddTimer(10000, pcsRelayServer->m_lServerID, (PVOID) pThis, Shutdown, NULL);
		}
		else
		{
			++pThis->m_lRelayCheckCount;
			pThis->m_AsIOCPServer.m_csTimer.AddTimer(10000, pcsRelayServer->m_lServerID, (PVOID) pThis, CheckRelayStatus, NULL);
			
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "m_lRelayCheckCount=%d", pThis->m_lRelayCheckCount);
			AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);
		}
	}
	else
	{
		pThis->m_lRelayCheckCount	= 0;
	}

	return TRUE;
}

BOOL ServerEngine::Shutdown(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass)
		return FALSE;

	ServerEngine	*pThis		= (ServerEngine *)	pClass;

#ifdef	_ALEF_SERVICE_MODE
	::SetEvent(pThis->m_hEventStop);
#else
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d]", __FUNCTION__, __LINE__);
	AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

	pThis->Shutdown();
	::ExitProcess(0);
#endif	//_ALEF_SERVICE_MODE

	return TRUE;
}

BOOL ServerEngine::Shutdown()
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d]", __FUNCTION__, __LINE__);
	AuLogFile_s("LOG\\ServerDownLog.txt", strCharBuff);

	printf("\n Save All Character Data..... \n");

	SaveAllCharacterData();

	printf("\n Success !!!! \n");

	SetServerStatus(GF_SERVER_STOP);

	DisconnectServers();

	StopProcess();

	RemoveUsers();

	OnTerminate();

	Stop();

	return TRUE;
}
