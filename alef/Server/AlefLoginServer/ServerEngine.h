#ifndef	__ALEF_AUCTION_SERVER_H__
#define __ALEF_AUCTION_SERVER_H__

#include "windows.h"
#include "AsCommonLib.h"
#include "AuDatabase2.h"
#include "AgsmInterServerLink.h"
#include "ApmObject.h"
#include "AuExcelTxtLib.h"
#include "AgpmAI2.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
#include "AgpmCombat.h"
#include "AgpmAdmin.h"
//#include "AgpmItemLog.h"
#include "AgpmChatting.h"
#include "AgpmPathFind.h"
#include "AgpmParty.h"
#include "AgpmSummons.h"
#include "AgpmEventBinding.h"
#include "AgpmUIStatus.h"
#include "AgpmGuild.h"
#include "AgpmWorld.h"
//#include "AgpmOptimizedPacket.h"
#include "AgpmQuest.h"
#include "AgpmResourceInfo.h"
#include "AgpmBillInfo.h"
#include "AgpmLogin.h"
#include "AgpmScript.h"
#include "AgpmConfig.h"
#include "AgpmSystemMessage.h"
#include "AgpmSiegeWar.h"

#include "AgpmLog.h"
#include "AgsmLog.h"

#include "AgsmSystemMessage.h"
#include "AgsmMap.h"
#include "AgsmItem.h"
#include "AgsmItemConvert.h"
#include "AgsmItemManager.h"
//#include "AgsmItemLog.h"
#include "AgsmCharManager.h"
#include "AgsmSkill.h"
#include "AgsmSkillManager.h"
#include "AgsmLoginServer.h"
#include "AgsmLoginClient.h"
#include "AgsmLoginDB.h"
#include "AgsmUIStatus.h"
#include "AgsmChatting.h"
#include "AgsmParty.h"
#include "AgsmGuild.h"
//#include "AgsmAdminDB.h"
#include "AgsmAdmin.h"
#include "AgsmWorld.h"
//#include "AsEchoServer.h"
#include "AgsmQuest.h"
#include "AgsmEventSkillMaster.h"
#include "AgsmDropItem.h"
#include "AgsmEventTeleport.h"
#include "AgpmAuction.h"
#include "AgsmAuction.h"
#include "AgsmBillInfo.h"
#include "AgsmGKforPCRoom.h"
#include "AgsmPvP.h"
#include "AgsmReturnToLogin.h"
#include "AgsmStartupEncryption.h"
#include "AgsmConfig.h"
#include "AgsmServerStatus.h"
#include "AgpmArchlord.h"
//#include "AgsmArchlord.h"
#include "AgpmEventQuest.h"
#include "AgsmLogin.h" //JK_중복로그인
#include "AgpmTitle.h"
#include "AgsmTitle.h"
#include "AgsmPrivateTrade.h"//JK_거래중금지

class ServerEngine : public AgsEngine {
public:
	BOOL m_bIPBlock;

	ServerEngine(GUID guidApp);
	~ServerEngine();

	void	OpenIPInfo( bool &bUseIPBlock, bool &bOpenPCRoom );

	BOOL	OnRegisterModule();
	BOOL	OnTerminate();

	void	InitMemoryPoolCount();
	BOOL	InitServerManager();
	BOOL	ConnectServers();
	BOOL	DisconnectServers();
	BOOL	WaitDBThreads();
};

#endif