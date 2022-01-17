/******************************************************************************
Module:  ServerEngine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 10
******************************************************************************/

#if !defined(__SERVERENGINE_H__)
#define __SERVERENGINE_H__

#include "AgsEngine.h"

#include "ApmMap.h"
#include "AgpmPathFind.h"
#include "ApmObject.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmUnion.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
#include "AgpmCombat.h"
#include "AgpmDropItem.h"
#include "AgpmParty.h"
#include "AgpmSkill.h"
#include "AgpmChatting.h"
#include "AgpmUIStatus.h"
#include "AgpmAuction.h"
#include "AgpmResourceInfo.h"
#include "AgpmLogin.h"
#include "AgpmSystemMessage.h"
#include "AgsmSystemMessage.h"
#include "AgpmConfig.h"

//#include "AgpmScript.h"

#include "AsCommonLib.h"

#include "AgpmAdmin.h"
#include "AgsmAdmin.h"

#include "AgpmItemConvert.h"
#include "AgsmItemConvert.h"
#include "AgsmAOIFilter.h"
#include "AgsmFactors.h"
#include "AgsmMap.h"
#include "AgsmAccountManager.h"
#include "AgsmCharacter.h"
#include "AgsmZoning.h"
#include "AgsmItem.h"
#include "AgsmPrivateTrade.h"
#include "AgsmCharManager.h"
#include "AgsmItemManager.h"
#include "AgsmDropItem2.h"
#include "AgsmSkill.h"
#include "AgsmSkillManager.h"
#include "AgsmCombat.h"
#include "AgsmDeath.h"
#include "AgsmServerManager2.h"
#include "AgsmInterServerLink.h"
#include "AgsmAuction.h"
#include "AgsmAuctionRelay.h"
#include "AgsmLogin.h"
#include "AgsmParty.h"
#include "AgsmShrine.h"
#include "AgsmChatting.h"
#include "AgsmSystemInfo.h"
#include "AgsmUIStatus.h"
#include "AgsmTimer.h"

#include "ApmEventManager.h"
#include "AgsmEventManager.h"
#include "AgpmEventTeleport.h"
#include "AgsmEventTeleport.h"
#include "AgpmEventNPCTrade.h"
#include "AgpmEventNPCDialog.h"
#include "AgsmEventNPCDialog.h"
#include "AgsmEventNPCTrade.h"
#include "AgpmEventSpawn.h"
#include "AgsmEventSpawn.h"
#include "AgpmEventNature.h"
#include "AgpmEventItemRepair.h"
#include "AgsmEventItemRepair.h"
#include "AgsmEventBank.h"
#include "AgpmEventGuild.h"
#include "AgsmEventGuild.h"
#include "AgpmEventProduct.h"
#include "AgsmEventProduct.h"
#include "AgpmEventQuest.h"
#include "AgsmEventQuest.h"
#include "AgpmEventItemConvert.h"
#include "AgsmEventItemConvert.h"
#include "AgpmEventProduct.h"
#include "AgsmEventProduct.h"
#include "AgpmEventRefinery.h"
#include "AgsmEventRefinery.h"
#include "AgsmEventSkillMaster.h"
#include "AgsmEventNature.h"

#include "AgpmEventBinding.h"

#include "AgpmQuest.h"
#include "AgsmQuest.h"

#include "AgpmBuddy.h"
#include "AgsmBuddy.h"

#include "AgpmChannel.h"
#include "AgsmChannel.h"

#include "AgpmMailBox.h"
#include "AgsmMailBox.h"

#include "AgpmRide.h"
#include "AgsmRide.h"

#include "AgpmGuild.h"
#include "AgsmGuild.h"

#include "AgpmProduct.h"
#include "AgsmProduct.h"
#include "AgpmRefinery.h"
#include "AgsmRefinery.h"

#include "AgpmPvP.h"
#include "AgsmPvP.h"

#include "AgpmSearch.h"
#include "AgsmSearch.h"

#include "AgpmLog.h"
#include "AgsmLog.h"

#include "AgpmAI2.h"
#include "AgsmAI2.h"

#include "AgpmSummons.h"
#include "AgsmSummons.h"

#include "AgsmRelay2.h"

#include "AgsmCasper.h"

#include "AgsmServerStatus.h"

#include "AgsmEventCharCustomize.h"

#include "AgpmBillInfo.h"
#include "AgsmBillInfo.h"

#include "AgsmGKforPCRoom.h"
#include "AgsmAreaChatting.h"

#include "AgpmRemission.h"
#include "AgsmRemission.h"

#include "AgpmWantedCriminal.h"
#include "AgsmWantedCriminal.h"

#include "AgsmConfig.h"

#include "AgsmBilling.h"
#include "AgsmCashMall.h"

#include "MemoryPoolStream.h"

#include "AgsmUsedCharDataPool.h"

#include "AgsmReturnToLogin.h"
#include "AgsmSiegeWar.h"
#include "AgsmEventSiegeWarNPC.h"

#include "AgpmTax.h"
#include "AgsmTax.h"

#include "AgpmGuildWarehouse.h"
#include "AgsmGuildWarehouse.h"

#include "AgpmGamble.h"
#include "AgsmGamble.h"

#include "AgsmStartupEncryption.h"

#include "AgpmArchlord.h"
#include "AgsmArchlord.h"

#include "AgpmTitle.h"
#include "AgsmTitle.h"

// for western
#include "AgsmCogpas.h"


class ServerEngine : public AgsEngine {
private:
	INT32	m_lRelayCheckCount;

	MemoryPoolStream	m_csPoolCountStream;

public:
	HANDLE	m_hEventStop;
	HANDLE	m_hEventGuildLoadComplete;

public:
	ServerEngine(GUID guidApp);
	~ServerEngine();

	BOOL	Shutdown();

	BOOL	OnRegisterModule();
	BOOL	RegisterScriptCommand();

	BOOL	OnTerminate();

	BOOL	ConnectRelayServer();
	BOOL	ConnectLKServer();
	BOOL	ConnectBillingServer();
	BOOL	ConnectServers();
	BOOL	DisconnectServers();

	BOOL	InitializeDBID();

	BOOL	SaveAllCharacterData();

	BOOL	RemoveUsers();

	BOOL	SetGuildLoadEvent();
	BOOL	WaitForGuildLoadComplete();

	BOOL	LoadGuildInfoFromDB();
	BOOL	LoadSiegeInfoFromDB();
	
	void PrintCurrentUserCount();
	void PrintGuildInfo();

	static BOOL CBDisconnectServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReplyFailFromRelay(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CheckRelayStatus(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
	static BOOL Shutdown(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

	static unsigned int TimerThreadRoutine( void* pArg );
};

#endif //__SERVERENGINE_H__