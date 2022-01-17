/*====================================================================

	AgsmRelay2.cpp

====================================================================*/


#include "AgsmRelay2.h"

#ifdef __PROFILE__
#include "API_AuFrameProfile.h"
#endif

#include "ApMemoryTracker.h"


/****************************************************/
/*		The Implementation of AgsmRelay2 class		*/
/****************************************************/
//
AgsmRelay2::AgsmRelay2()
	{
	SetModuleName("AgsmRelay2");
	SetPacketType(AGSMRELAY_PACKET_TYPE);

	m_pAgpmConfig			= NULL;
	m_pAgpmCharacter		= NULL;
	m_pAgsmCharacter		= NULL;
	m_pAgpmItem				= NULL;
	m_pAgsmItem				= NULL;
	m_pAgsmItemConvert		= NULL;
	m_pAgsmServerManager	= NULL;
	m_pAgsmServerManager2	= NULL;
	m_pAgpmFactors			= NULL;
	m_pAgpmSkill			= NULL;
	m_pAgsmSkill			= NULL;
	m_pAgsmProduct			= NULL;
	m_pAgsmEventSkillMaster	= NULL;
	m_pAgsmUIStatus			= NULL;
	m_pAgpmGuild			= NULL;
	m_pAgsmGuild			= NULL;
	m_pAgsmInterServerLink	= NULL;
	m_pAgsmQuest			= NULL;
	m_pAgsmDatabasePool		= NULL;
	m_pAgpmAdmin			= NULL;
	m_pAgsmAdmin			= NULL;
	m_pAgsmBuddy			= NULL;
	m_pAgpmMailBox			= NULL;
	m_pAgsmMailBox			= NULL;
	m_pAgsmCashMall			= NULL;
	m_pAgsmReturnToLogin	= NULL;
	m_pAgsmWantedCriminal	= NULL;
	m_pAgsmSiegeWar			= NULL;
	m_pagsmEventSystem		= NULL;
	m_pAgsmServerStatus		= NULL;

	m_pAgpmTitle			= NULL;
	m_pAgsmTitle			= NULL;
	
	m_pSender				= NULL;
	m_pSenderLog			= NULL;

	InitPacket();
	InitPacketCustom();
	InitPacketCharacter();
	InitPacketItem();
	InitPacketItemConvert();
	InitPacketSkill();
	InitPacketUIStatus();
	InitPacketGuildMaster();
	InitPacketGuildMember();
	InitPacketAccountWorld();
	InitPacketQuest();
	InitPacketConcurrentUser();
	InitPacketAdmin();
	InitPacketBuddy();
	InitPacketMail();
	InitPacketCashItemBuyList();
	InitPacketWantedCriminal();
	InitPacketSiegeWar();
	InitPacketGuildItem();
	InitPacketArchlord();
	InitPacketLordGuard();
	}


AgsmRelay2::~AgsmRelay2()
	{
  #ifdef _RELAYDEBUG	
	m_ItemSeq.Reset();
  #endif
	}




//	Packet Initialization Methods
//=========================================================
//
void AgsmRelay2::InitPacket()
	{
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT16,			1, // Parameter
							AUTYPE_INT8,			1, // Status
							AUTYPE_PACKET,			1, // Embeded Packet
							AUTYPE_END,				0
							);

	m_csPacketRowset.SetFlagLength(sizeof(INT16));
	m_csPacketRowset.SetFieldType(//AUTYPE_INT16,		1,		// eAgsmRelay2Operation
								  AUTYPE_INT32,			1,		// Query Index
								  AUTYPE_MEMORY_BLOCK,	1,		// Query Text
								  AUTYPE_MEMORY_BLOCK,	1,		// Headers
								  AUTYPE_UINT32,		1,		// No. of Rows
								  AUTYPE_UINT32,		1,		// No. of Cols
								  AUTYPE_UINT32,		1,		// Row Buffer Size
								  AUTYPE_MEMORY_BLOCK,	1,		// Buffer
								  AUTYPE_MEMORY_BLOCK,	1,		// Offset
								  AUTYPE_END,			0
								  );
	}




//	Module Inherited Methods
//=========================================================
//
BOOL AgsmRelay2::OnAddModule()
	{
	CreateDirectory("LOG", NULL);

  #ifdef _RELAYDEBUG
	m_ItemSeq.SetCount(500000);
	m_ItemSeq.InitializeObject(sizeof(INT32), m_ItemSeq.GetCount(), NULL, NULL, this);
  #endif
	
	// get related modules
	m_pAgpmConfig			= (AgpmConfig *) GetModule(_T("AgpmConfig"));
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgsmCharacter		= (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgpmItem				= (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgsmItem				= (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemConvert		= (AgsmItemConvert *) GetModule(_T("AgsmItemConvert"));
	m_pAgsmServerManager	= (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmServerManager2	= (AgsmServerManager2 *) GetModule("AgsmServerManager2");
	m_pAgpmFactors			= (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmSkill			= (AgpmSkill *) GetModule(_T("AgpmSkill"));
	m_pAgsmSkill			= (AgsmSkill *) GetModule(_T("AgsmSkill"));
	m_pAgsmProduct			= (AgsmProduct *) GetModule(_T("AgsmProduct"));
	m_pAgsmEventSkillMaster	= (AgsmEventSkillMaster *) GetModule(_T("AgsmEventSkillMaster"));
	m_pAgsmUIStatus			= (AgsmUIStatus *) GetModule(_T("AgsmUIStatus"));
	m_pAgpmGuild			= (AgpmGuild *) GetModule(_T("AgpmGuild"));
	m_pAgsmGuild			= (AgsmGuild *) GetModule(_T("AgsmGuild"));
	m_pAgsmInterServerLink	= (AgsmInterServerLink *) GetModule(_T("AgsmInterServerLink"));
	m_pAgsmQuest			= (AgsmQuest *) GetModule(_T("AgsmQuest"));
	m_pAgpmAdmin			= (AgpmAdmin *) GetModule(_T("AgpmAdmin"));
	m_pAgsmAdmin			= (AgsmAdmin *) GetModule(_T("AgsmAdmin"));
	m_pAgsmAuctionRelay		= (AgsmAuctionRelay *) GetModule(_T("AgsmAuctionRelay"));
	m_pAgsmDeath			= (AgsmDeath *) GetModule(_T("AgsmDeath"));
	m_pAgsmBuddy			= (AgsmBuddy *) GetModule(_T("AgsmBuddy"));
	m_pAgpmMailBox			= (AgpmMailBox *) GetModule(_T("AgpmMailBox"));
	m_pAgsmMailBox			= (AgsmMailBox *) GetModule(_T("AgsmMailBox"));
	m_pAgsmCashMall			= (AgsmCashMall *) GetModule(_T("AgsmCashMall"));
	m_pAgsmReturnToLogin	= (AgsmReturnToLogin *) GetModule(_T("AgsmReturnToLogin"));
	m_pAgsmWantedCriminal	= (AgsmWantedCriminal *) GetModule(_T("AgsmWantedCriminal"));
	m_pAgsmSiegeWar			= (AgsmSiegeWar *) GetModule(_T("AgsmSiegeWar"));
	m_pAgsmTax				= (AgsmTax *) GetModule(_T("AgsmTax"));
	m_pAgsmGuildWarehouse	= (AgsmGuildWarehouse *) GetModule(_T("AgsmGuildWarehouse"));
	m_pAgsmArchlord			= (AgsmArchlord *) GetModule(_T("AgsmArchlord"));
	m_pAgsmServerStatus		= (AgsmServerStatus *) GetModule(_T("AgsmServerStatus"));

	// if module exist, set callbacks
	if (!m_pAgsmServerManager || !m_pAgsmServerManager2)
		return FALSE;

	if (m_pAgsmInterServerLink && !m_pAgsmInterServerLink->SetCallbackConnect(CBServerConnect, this))
		return FALSE;

	if (m_pAgpmCharacter)
		{
		if (!m_pAgpmCharacter->SetCallbackMoveBankMoney(CBAccountWorldUpdate, this))
			return FALSE;
		if (!m_pAgpmCharacter->SetCallbackUpdateBankSize(CBAccountWorldUpdate, this))
			return FALSE;

		if (!m_pAgpmCharacter->SetCallbackRemoveChar(CBCharacterGameOut, this))
			return FALSE;
		}

	if (m_pAgsmCharacter)
		{
		if (!m_pAgsmCharacter->SetCallbackCharacterUpdate(CBCharacterUpdate, this))
			return FALSE;
		
		if (!m_pAgsmCharacter->SetCallbackBankUpdate(CBAccountWorldUpdate, this))
			return FALSE;
		
		if (!m_pAgsmCharacter->SetCallbackEnterGameworld(CBCharacterGameIn, this))
			return FALSE;
		}

	if (m_pAgsmReturnToLogin)
		{
		if (!m_pAgsmReturnToLogin->SetCallbackNotifySaveAll(CBNotifySaveAll, this))
			return FALSE;
		}
	
	if (m_pAgsmItem)
		{
		if (!m_pAgsmItem->SetCallbackItemInsert(CBItemInsert, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackItemUpdate(CBItemUpdate, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackItemDelete(CBItemDelete, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackCashItemBuyListInsert(CBCashItemBuyListInsert, this))
			return FALSE;
		
		if (!m_pAgsmItem->SetCallbackCashItemBuyListUpdate(CBCashItemBuyListUpdate, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackCashItemBuyListUpdate2(CBCashItemBuyListUpdate2, this))
			return FALSE;

		if (!m_pAgsmItem->SetCallbackUseItemReverseOrb(CBUseReverserOrb, this))
			return FALSE;			

		if (!m_pAgsmItem->SetCallbackUseItemTransform(CBTransform, this))
			return FALSE;			
		}

	if (m_pAgsmItemConvert)
		{
		if (!m_pAgsmItemConvert->SetCallbackUpdateConvertHistoryToDB(CBItemConvertUpdate, this))
			return FALSE;
		}

	if (m_pAgsmSkill)
		{
		if (!m_pAgsmSkill->SetCallbackUpdateSkillMasteryToDB(CBSkillUpdate, this))
			return FALSE;
		}

	if (m_pAgsmEventSkillMaster)
	{
		if (!m_pAgsmEventSkillMaster->SetCallbackUpdateDB(CBSkillUpdate, this))
			return FALSE;
	}

	if (m_pAgsmProduct)
		{
		if (!m_pAgsmProduct->SetCallbackUpdateDB(CBSkillUpdate, this))
			return FALSE;
		}

	if (m_pAgsmUIStatus)
		{
		if (!m_pAgsmUIStatus->SetCallbackUpdateDataToDB(CBUIStatusUpdate, this))
			return FALSE;
		}

	if (m_pAgsmGuild)
		{
		if(!m_pAgsmGuild->SetCallbackDBGuildInsert(CBGuildMasterInsert, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildUpdate(CBGuildMasterUpdate, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildDelete(CBGuildMasterDelete, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildSelect(CBGuildMasterSelect, this))
			return FALSE;
			
		if(!m_pAgsmGuild->SetCallbackDBGuildIDCheck(CBGuildMasterCheck, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberInsert(CBGuildMemberInsert, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberUpdate(CBGuildMemberUpdate, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberDelete(CBGuildMemberDelete, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBMemberSelect(CBGuildMemberSelect, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBGuildInsertBattleHistory(CBGuildBattleHistroyInsert, this))
			return FALSE;

		if(!m_pAgsmGuild->SetCallbackDBRename(CBGuildRename, this))
			return FALSE;

		if (!SetCallbackGuildMaster(CBOperationResultGuildMaster, this))
			return FALSE;

		if (!SetCallbackGuildMasterCheck(CBOperationResultGuildMasterCheck, this))
			return FALSE;

		if (!SetCallbackGuildMember(CBOperationResultGuildMember, this))
			return FALSE;

		if (!m_pAgsmGuild->SetCallbackDBJointInsert(CBGuildJointInsert, this))
			return FALSE;
		if (!m_pAgsmGuild->SetCallbackDBJointUpdate(CBGuildJointUpdate, this))
			return FALSE;
		if (!m_pAgsmGuild->SetCallbackDBJointDelete(CBGuildJointDelete, this))
			return FALSE;
		if (!m_pAgsmGuild->SetCallbackDBHostileInsert(CBGuildHostileInsert, this))
			return FALSE;
		if (!m_pAgsmGuild->SetCallbackDBHostileDelete(CBGuildHostileDelete, this))
			return FALSE;
		
		if (!SetCallbackGuildRelation(CBOperationResultGuildRelation, this))
			return FALSE;
		}

	if (m_pAgsmQuest)
		{
		if (!m_pAgsmQuest->SetCallbackDBCurrentQuest(CBCurrentQuest, this))
			return FALSE;

		if (!m_pAgsmQuest->SetCallbackDBFlag(CBQuestFlag, this))
			return FALSE;
		}

	if (m_pAgsmAdmin && m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
		{
		if (!m_pAgsmAdmin->SetCallbackConcurrentUser(CBConcurrentUser, this))
			return FALSE;

		if (!m_pAgsmAdmin->SetCallbackPingSend(CBPingSend, this))
			return FALSE;
		if (!SetCallbackPing(CBOperationResultPing, this))
			return FALSE;

		// 2005.05.02. steeple
		// Search~~~~
		if (!m_pAgsmAdmin->SetCallbackRelaySearchAccount(CBSearchAccount, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelaySearchCharacter(CBSearchCharacter, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelaySearchAccountBank(CBSearchAccountBank, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayUpdateCharacter(CBAdminUpdateCharacter, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayUpdateCharacter2(CBAdminUpdateCharacter2, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayUpdateBankMoney(CBAdminUpdateBankMoney, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayUpdateCharismaPoint(CBAdminUpdateCharismaPoint, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayItemInsert(CBAdminItemInsert, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayItemDelete(CBAdminItemDelete, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayItemConvertUpdate(CBAdminItemConvertUpdate, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayItemConvertUpdate2(CBAdminItemConvertUpdate2, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelayItemUpdate(CBAdminItemUpdate, this))
			return FALSE;
		if (!m_pAgsmAdmin->SetCallbackRelaySkillUpdate(CBAdminSkillUpdate, this))
			return FALSE;

		if (!SetCallbackAdmin(CBOperationResultAdmin, this))
			return FALSE;
		}

	if (m_pAgsmBuddy)
		{
		if (!m_pAgsmBuddy->SetCallbackDBInsert(CBBuddyInsert, this))
			return FALSE;
		if (!m_pAgsmBuddy->SetCallbackDBRemove(CBBuddyRemove, this))
			return FALSE;
		if (!m_pAgsmBuddy->SetCallbackDBRemove2(CBBuddyRemove2, this))
			return FALSE;
		if (!m_pAgsmBuddy->SetCallbackDBOptions(CBBuddyOptions, this))
			return FALSE;
		if (!m_pAgsmBuddy->SetCallbackDBOptions2(CBBuddyOptions2, this))
			return FALSE;
		if (!m_pAgsmBuddy->SetCallbackDBSelect(CBBuddySelect, this))
			return FALSE;

		if (!SetCallbackBuddy(CBOperationResultBuddy, this))
			return FALSE;
		}

	if (m_pAgsmMailBox)
		{
		if (!m_pAgsmMailBox->SetCallbackWriteMail(CBMailInsert, this))
			return FALSE;
		if (!m_pAgsmMailBox->SetCallbackUpdateMail(CBMailUpdate, this))
			return FALSE;
		if (!m_pAgsmMailBox->SetCallbackDeleteMail(CBMailDelete, this))
			return FALSE;
		if (!m_pAgsmMailBox->SetCallbackSelectMail(CBMailSelect, this))
			return FALSE;
		if (!m_pAgsmMailBox->SetCallbackItemSave(CBMailItemSelect, this))
			return FALSE;

		if (!SetCallbackMail(CBOperationResultMail, this))
			return FALSE;
		if (!SetCallbackMailItem(CBOperationResultMailItem, this))
			return FALSE;
		}
	
	if (m_pAgsmAuctionRelay)
		{
		if (FALSE == m_pAgsmAuctionRelay->SetCallbackBufferingSend(CBBufferingSend, this))
			return FALSE;
		}

	if (m_pAgsmCashMall)
		{
		
		}

	if (m_pAgsmWantedCriminal)
		{
		if (!m_pAgsmWantedCriminal->SetCallbackSelect(CBWantedCriminalSelect, this))
			return FALSE;
		if (!m_pAgsmWantedCriminal->SetCallbackInsert(CBWantedCriminalInsert, this))
			return FALSE;
		if (!m_pAgsmWantedCriminal->SetCallbackUpdate(CBWantedCriminalUpdate, this))
			return FALSE;
		if (!m_pAgsmWantedCriminal->SetCallbackDelete(CBWantedCriminalDelete, this))
			return FALSE;
		if (!m_pAgsmWantedCriminal->SetCallbackOffCharInfo(CBWantedCriminalOffCharInfo, this))
			return FALSE;
			
		if (!SetCallbackWantedCriminal(CBOperationResultWantedCriminal, this))
			return FALSE;
		}

	if (m_pAgsmSiegeWar)
		{
		if (!m_pAgsmSiegeWar->SetCallbackDBSelectCastle(CBCastleSelect, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBUpdateCastle(CBCastleUpdate, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBSelectSiege(CBSiegeSelect, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBUpdateSiege(CBSiegeUpdate, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBInsertSiege(CBSiegeInsert, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBSelectSiegeApplication(CBSiegeApplicationSelect, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBUpdateSiegeApplication(CBSiegeApplicationUpdate, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBInsertSiegeApplication(CBSiegeApplicationInsert, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBSelectSiegeObject(CBSiegeObjectSelect, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBUpdateSiegeObject(CBSiegeObjectUpdate, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBInsertSiegeObject(CBSiegeObjectInsert, this)
			|| !m_pAgsmSiegeWar->SetCallbackDBDeleteSiegeObject(CBSiegeObjectDelete, this)
			)
			return FALSE;
		
		if (!SetCallbackCastle(CBOperationResultCastle, this)
			|| !SetCallbackSiege(CBOperationResultSiege, this)
			|| !SetCallbackSiegeApplication(CBOperationResultSiegeApplication, this)
			|| !SetCallbackSiegeObject(CBOperationResultSiegeObject, this)
			)
			return FALSE;
		}
	
	if (m_pAgsmTax)
		{
		if (!m_pAgsmTax->SetCallbackDBUpdate(CBTaxUpdate, this))
			return FALSE;
		}
	
	if (m_pAgsmGuildWarehouse)
		{
		if (!m_pAgsmGuildWarehouse->SetCallbackDBMoneyUpdate(CBGuildWarehouseMoneyUpdate, this))
			return FALSE;

		if (!m_pAgsmGuildWarehouse->SetCallbackDBItemLoad(CBGuildItemSelect, this))
			return FALSE;
				
		if (!m_pAgsmGuildWarehouse->SetCallbackDBItemIn(CBGuildItemInsert, this))
			return FALSE;
		
		if (!m_pAgsmGuildWarehouse->SetCallbackDBItemOut(CBGuildItemDelete, this))
			return FALSE;
		
		if (!SetCallbackGuildItem(CBOperationResultGuildItem, this))
			return FALSE;
		}
	
	if (m_pAgsmArchlord)
		{
		if (!m_pAgsmArchlord->SetCallbackDBSelectArclord(CBArchlordSelect, this))
			return FALSE;
		if (!m_pAgsmArchlord->SetCallbackDBUpdateArclord(CBArchlordUpdate, this))
			return FALSE;
		if (!m_pAgsmArchlord->SetCallbackDBInsertArchlord(CBArchlordInsert, this))
			return FALSE;
		if (!m_pAgsmArchlord->SetCallbackDBSelectLordGuard(CBLordGuardSelect, this))
			return FALSE;
		if (!m_pAgsmArchlord->SetCallbackDBInsertLordGuard(CBLordGuardInsert, this))
			return FALSE;
		if (!m_pAgsmArchlord->SetCallbackDBDeleteLordGuard(CBLordGuardDelete, this))
			return FALSE;
		}

	return TRUE;
	}

BOOL AgsmRelay2::OnInit()
{
	m_pagsmEventSystem		= (AgsmEventSystem*)GetModule(_T("AgsmEventSystem"));
	m_pAgpmTitle			= (AgpmTitle *) GetModule(_T("AgpmTitle"));
	m_pAgsmTitle			= (AgsmTitle *) GetModule(_T("AgsmTitle"));

	return TRUE;
}

BOOL AgsmRelay2::OnDestroy()
	{
	if (m_pSender)
		{
		m_pSender->Stop();
		m_pSender->Wait(5000);
		delete m_pSender;
		m_pSender	= NULL;
		}
	
	if (m_pSenderLog)
		{
		m_pSenderLog->Stop();
		m_pSenderLog->Wait(5000);
		delete m_pSenderLog;
		m_pSenderLog	= NULL;
		}
	
	return TRUE;
	}


BOOL AgsmRelay2::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	INT16	nParam						= -1;
	INT8	cStatus						= -1;
	PVOID	pvPacketEmb					= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&nParam,
						&cStatus,
						&pvPacketEmb
						);
	switch (nParam)
		{
		case AGSMDATABASE_PARAM_CUSTOM:
			OnParamCustom(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_CHARACTER:
		case AGSMRELAY_PARAM_REVERSEORB:
		case AGSMRELAY_PARAM_TRANSFORM:
		case AGSMRELAY_PARAM_CHARGAMEIN:
		case AGSMRELAY_PARAM_CHARGAMEOUT:
			OnParamCharacter(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_ITEM:
		case AGSMRELAY_PARAM_CASHITEM:
			OnParamItem(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY:
			OnParamItemConvert(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_SKILL:
			OnParamSkill(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_UI_STATUS:
			OnParamUIStatus(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_GUILD_MASTER:
		case AGSMRELAY_PARAM_GUILD_MASTER_CHECK:
		case AGSMRELAY_PARAM_GUILD_BATTLE_HISTORY:
		case AGSMRELAY_PARAM_GUILD_RENAME:
		case AGSMRELAY_PARAM_GUILD_WAREHOUSE_MONEY:
		case AGSMRELAY_PARAM_GUILD_RELATION:
			OnParamGuildMaster(nParam, pvPacketEmb, ulNID); 
			break;

		case AGSMRELAY_PARAM_GUILD_MEMBER:
		case AGSMRELAY_PARAM_GUILD_MEMBER_NAVER:
			OnParamGuildMember(nParam, pvPacketEmb, ulNID); 
			break;

		case AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM :
			OnParamGuildItem(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_ACCOUNT_WORLD:
			OnParamAccountWorld(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_QUEST:
			OnParamQuest(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_QUEST_FLAG:
			OnParamQuestFlag(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_BUDDY:
			OnParamBuddy(nParam, pvPacketEmb, ulNID);
			break;
		
		case AGSMRELAY_PARAM_MAIL:
			OnParamMail(nParam, pvPacketEmb, ulNID);
			break;			

		case AGSMRELAY_PARAM_MAIL_ITEM:
			OnParamMailItem(nParam, pvPacketEmb, ulNID);
			break;				
		
		case AGSMRELAY_PARAM_CONCURRENT_USER:
			OnParamConcurrentUser(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_PING:
			OnParamPing(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_ADMIN:
			OnParamAdmin(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_CASHITEMBUYLIST:
		case AGSMRELAY_PARAM_CASHITEMBUYLIST2:
			OnParamCashItemBuyList(nParam, pvPacketEmb, ulNID);
			break;
			
		case AGSMRELAY_PARAM_WANTEDCRIMINAL:
			OnParamWantedCriminal(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_CASTLE :
			OnParamCastle(nParam, pvPacketEmb, ulNID);
			break;
			
		case AGSMRELAY_PARAM_SIEGE :
			OnParamSiege(nParam, pvPacketEmb, ulNID);
			break;
			
		case AGSMRELAY_PARAM_SIEGE_APPLICATION :
			OnParamSiegeApplication(nParam, pvPacketEmb, ulNID);
			break;
			
		case AGSMRELAY_PARAM_SIEGE_OBJECT :
			OnParamSiegeObject(nParam, pvPacketEmb, ulNID);
			break;
	
		case AGSMRELAY_PARAM_TAX :
			OnParamTax(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_NOTIFY_SAVE_ALL:
			OnParamNotifySaveAll(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMRELAY_PARAM_SKILL_SAVE:
			OnParamSkillSave((PACKET_HEADER*)pvPacket, ulNID);
			break;

		case AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP:
			OnParamWorldChampionship((PACKET_HEADER*)pvPacket, ulNID);
			break;

		case AGSMRELAY_PARAM_ATTENDANCE:
			OnParamAttendance((PACKET_HEADER*)pvPacket, ulNID);
			break;

		case AGSMRELAY_PARAM_EVENT_ITEM:
			OnParamItemGiveEvent((PACKET_HEADER*)pvPacket, ulNID);
			break;

		case AGSMRELAY_PARAM_CHANGENAME:
			OnParamChangeName((PACKET_HEADER*)pvPacket, ulNID);
			break;

		case AGSMRELAY_PARAM_CERARIUMORB:
			OnParamCerariumOrb((PACKET_HEADER*)pvPacket, ulNID);
			break;

		case AGSMRELAY_PARAM_CASHITEMTIMEEXTEND:
			OnParamCashItemTimeExtend((PACKET_HEADER*)pvPacket, ulNID);
			break;
		case AGSMRELAY_PARAM_ADMIN_EDIT_BANK_MONEY:
			OnParamBankMoneyEdit((PACKET_HEADER*)pvPacket, ulNID);
			break;
		case AGSMRELAY_QUERY_CHARISMAPOINTEDIT:
			OnParamCharismaPointEdit((PACKET_HEADER*)pvPacket, ulNID);
			break;
		case AGSMRELAY_PARAM_TITLE:
			OnParamTitle((PACKET_HEADER*)pvPacket, ulNID);
			break;
		case AGSMRELAY_PARAM_CHARACTER_CREATION_DATE:
			OnParamCharacterCreationDate((PACKET_HEADER*)pvPacket, ulNID);
			break;
		//JK_특성화서버
		case AGSMRELAY_PARAM_SERVERMOVE:
			OnParamServerMove((PACKET_HEADER*)pvPacket, ulNID);
			break;

		default:
			OutputDebugString("!!! Error : Unknown Packet Received in AgsmRelay2::OnReceive()\n");
			break;
		}

	return TRUE;
	}




//	Callback Setting Methods
//=================================================
//
BOOL AgsmRelay2::SetCallbackCustom(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMDATABASE_PARAM_CUSTOM, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CHARACTER, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackReverseOrb(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_REVERSEORB, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_TRANSFORM, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCharacterGameIn(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CHARGAMEIN, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCharacterGameOut(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CHARGAMEOUT, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_ITEM, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
		return SetCallback(AGSMRELAY_PARAM_CASHITEM, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackItemConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_SKILL, pfCallback, pClass);
	}

BOOL AgsmRelay2::SetCallbackUIStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_UI_STATUS, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackGuildMaster(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_MASTER, pfCallback, pClass);
	}

BOOL AgsmRelay2::SetCallbackGuildMasterCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_MASTER_CHECK, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackGuildMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_MEMBER, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackGuildBattleHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_BATTLE_HISTORY, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackGuildRename(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_RENAME, pfCallback, pClass);
	}

BOOL AgsmRelay2::SetCallbackGuildRelation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_RELATION, pfCallback, pClass);
	}

BOOL AgsmRelay2::SetCallbackGuildWarehouseMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_WAREHOUSE_MONEY, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackGuildItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackAccountWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_ACCOUNT_WORLD, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_QUEST, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackQuestFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_QUEST_FLAG, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackBuddy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_BUDDY, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_MAIL, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackMailItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_MAIL_ITEM, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackConcurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CONCURRENT_USER, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackPing(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_PING, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCashItemBuyList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CASHITEMBUYLIST, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCashItemBuyList2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CASHITEMBUYLIST2, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackWantedCriminal(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_WANTEDCRIMINAL, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackCastle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_CASTLE, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_SIEGE, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_SIEGE_APPLICATION, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_SIEGE_OBJECT, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackTax(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_TAX, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_ARCHLORD, pfCallback, pClass);
	}


BOOL AgsmRelay2::SetCallbackLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMRELAY_PARAM_LORDGUARD, pfCallback, pClass);
	}




//	MakeRelayPacket
//========================================================
//
//	make final packet for send
//
PVOID AgsmRelay2::MakeRelayPacket(BOOL bPacket, INT16* pnPacketLength, INT16 nParam, INT8 cStatus, PVOID pvPacketEmb)
	{
	return m_csPacket.MakePacket(bPacket, pnPacketLength, AGSMRELAY_PACKET_TYPE, 
								&nParam, 
								&cStatus,
								pvPacketEmb
								);
	}




//	MakeRowsetPacket
//========================================================
//
PVOID AgsmRelay2::MakeRowsetPacket(AuRowset *pRowset)
	{
	ASSERT(NULL != pRowset);

	stBuddyRowset csCustom;

	csCustom.m_lQueryIndex = 0;
	// query, header 나중에
	ZeroMemory(csCustom.m_szQuery, sizeof(csCustom.m_szQuery));
	ZeroMemory(csCustom.m_szHeaders, sizeof(csCustom.m_szHeaders));

	csCustom.m_ulRows = pRowset->GetRowCount();
	csCustom.m_ulCols = pRowset->GetColCount();
	
	csCustom.m_pBuffer = pRowset->GetHeadPtr();
	csCustom.m_ulRowBufferSize = pRowset->GetRowBufferSize();
	
	for (UINT32 i=0; i<pRowset->GetColCount(); i++)
		csCustom.m_lOffsets[i] = pRowset->GetOffset(i);

	INT16 nBufferSize = csCustom.m_ulRowBufferSize * csCustom.m_ulRows;
	INT16 nOffsetsSize = (INT16)csCustom.m_ulCols * sizeof(INT32);

	INT16	nPacketLength = 0;
	
	return m_csPacketRowset.MakePacket(FALSE, &nPacketLength, 0,
											//&csCustom.m_eOperation,
											&csCustom.m_lQueryIndex,
											NULL,
											NULL,
											&csCustom.m_ulRows,
											&csCustom.m_ulCols,
											&csCustom.m_ulRowBufferSize,
											csCustom.m_pBuffer,
											&nBufferSize,
											&csCustom.m_lOffsets[0],
											&nOffsetsSize
											);
	}


PVOID AgsmRelay2::MakeRowsetPacket2(AuRowset *pRowset, INT32 lStep, INT32 lRowsPerStep)
	{
	ASSERT(NULL != pRowset);

	stRowset csCustom;

	csCustom.m_lQueryIndex = 0;
	// query, header 나중에
	ZeroMemory(csCustom.m_szQuery, sizeof(csCustom.m_szQuery));
	ZeroMemory(csCustom.m_szHeaders, sizeof(csCustom.m_szHeaders));

	csCustom.m_ulRows = min((INT32) (pRowset->GetRowCount() - (lStep * lRowsPerStep)), lRowsPerStep);
	csCustom.m_ulCols = pRowset->GetColCount();
	
	csCustom.m_pBuffer = pRowset->Get(lStep * lRowsPerStep, 0);
	csCustom.m_ulRowBufferSize = pRowset->GetRowBufferSize();
	
	for (UINT32 i=0; i<pRowset->GetColCount(); i++)
		csCustom.m_lOffsets[i] = pRowset->GetOffset(i);

	INT16 nBufferSize = csCustom.m_ulRowBufferSize * csCustom.m_ulRows;
	INT16 nOffsetsSize = (INT16)csCustom.m_ulCols * sizeof(INT32);

	INT16	nPacketLength = 0;
	
	PVOID pvPacket = m_csPacketRowset.MakePacket(FALSE, &nPacketLength, 0,
											//&csCustom.m_eOperation,
											&csCustom.m_lQueryIndex,
											NULL,
											NULL,
											&csCustom.m_ulRows,
											&csCustom.m_ulCols,
											&csCustom.m_ulRowBufferSize,
											csCustom.m_pBuffer,
											&nBufferSize,
											&csCustom.m_lOffsets[0],
											&nOffsetsSize
											);

	return pvPacket;
	}




//	MakeAndSendRelayPacket
//========================================================
//
BOOL AgsmRelay2::MakeAndSendRelayPacket(PVOID pvPacketEmb, INT16 nParam, UINT32 ulNID)
	{
	if (!pvPacketEmb)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, nParam, 0, pvPacketEmb);
	if (!pvPacket || 1 > nPacketLength)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);
	return bResult;
	}


BOOL AgsmRelay2::MakeAndSendRelayPacket(PVOID pvPacketEmb, INT16 nParam)
	{
	if (!pvPacketEmb)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, nParam, 0, pvPacketEmb);
	if (!pvPacket || 1 > nPacketLength)
		return FALSE;

	// if log sender exist, and param is log
	if (m_pSenderLog &&
		(nParam >= AGSMRELAY_PARAM_LOG_PLAY) &&
		(nParam <= AGSMRELAY_PARAM_LOG_GHELD))
		{
		if  (!m_pSenderLog->Push(pvPacket, nPacketLength))
			{
			TRACE("!!! Error : Packet push failed in AgsmRelay2::MakeAndSendRelayPacket()\n");
			}		
		}
	else if (m_pSender)
		{
		if  (!m_pSender->Push(pvPacket, nPacketLength))
			{
			TRACE("!!! Error : Packet push failed in AgsmRelay2::MakeAndSendRelayPacket()\n");
			}
		}
	else
		{
		AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
		if (NULL == pRelayServer || FALSE == pRelayServer->m_bIsConnected)
			{
			m_csPacket.FreePacket(pvPacket);
			return FALSE;
			}

		if (!SendPacket(pvPacket, nPacketLength, pRelayServer->m_dpnidServer))
			{
			m_csPacket.FreePacket(pvPacket);
			return FALSE;
			}
		}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;	
	}


BOOL AgsmRelay2::PushToSendQueue(PVOID pvPacket, INT16 nPacketLength)
	{
	if (m_pSender && m_pSender->Push(pvPacket, nPacketLength))
		return TRUE;
		
	TRACE("!!! Error : Packet push failed in AgsmRelay2::PushToSendQueue()\n");
	return FALSE;
	}


BOOL AgsmRelay2::SendAll()
	{
	if (m_pSender)
		m_pSender->SendAll();

	if (m_pSenderLog)
		m_pSenderLog->SendAll();

	return TRUE;
	}


BOOL AgsmRelay2::EnableBufferingSend()
	{
	if (AGSMSERVER_TYPE_GAME_SERVER == m_pAgsmServerManager->GetThisServerType())
		{
		m_pSender = new Relay2Sender;
		if (!m_pSender ||
			!m_pSender->Set((20 * 1024 * 1024), this, m_pAgsmServerManager->GetRelayServer(), _T("GAME")))
		//if (!m_pSender || !m_pSender->Set((20 * 1024 * 1024), this, m_pAgsmServerManager->GetRelayServer()))
			return FALSE;

		m_pSender->Start();
		}

	if (AGSMSERVER_TYPE_GAME_SERVER == m_pAgsmServerManager->GetThisServerType())
		{
		m_pSenderLog = new Relay2Sender;
		if (!m_pSenderLog ||
			!m_pSenderLog->Set((10 * 1024 * 1024), this, m_pAgsmServerManager->GetRelayServer(), _T("LOG")))
		//if (!m_pSenderLog || !m_pSenderLog->Set((20 * 1024 * 1024), this, m_pAgsmServerManager->GetRelayServer()))
			return FALSE;

		m_pSenderLog->Start();
		}

	return TRUE;
	}




//
//===================================
//
BOOL AgsmRelay2::CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return TRUE;

	AgsdServer *pAgsdServer = (AgsdServer *) pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;

	return TRUE;

	if (AGSMSERVER_TYPE_GAME_SERVER != pThis->m_pAgsmServerManager->GetThisServerType())
		return FALSE;

	AgsdServer* pAgsdServerRelay = pThis->m_pAgsmServerManager->GetRelayServer();
	if (!pAgsdServerRelay)
		return FALSE;

	AgsdServer *pAgsdServerThis = NULL;
	CHAR szType[21];
	INT32 lCurrent = 0;
	INT32 lMax = 0;

	pAgsdServerThis = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pAgsdServerThis)
		return FALSE;

	strcpy(szType, "전체");
	pThis->SendConcurrentUser(pAgsdServerThis->m_szWorld, szType, lCurrent, lMax);

	strcpy(szType, "오크");
	pThis->SendConcurrentUser(pAgsdServerThis->m_szWorld, szType, lCurrent, lMax);

	strcpy(szType, "휴먼");
	pThis->SendConcurrentUser(pAgsdServerThis->m_szWorld, szType, lCurrent, lMax);

	return TRUE;
	}




//	SetCallbackOperation
//========================================================
//
//	When module reside on relay server side
//	manually call this method
//	for setting database operation callback
//
BOOL AgsmRelay2::SetCallbackOperation()
	{
	m_pAgsmDatabasePool = (AgsmDatabasePool *) GetModule("AgsmDatabasePool");
	m_pAgsmLog			= (AgsmLog *) GetModule("AgsmLog");

	if (!m_pAgsmDatabasePool || !m_pAgsmLog)
		return FALSE;

	if (!SetCallbackCustom(CBOperationCustom, this)
		|| !SetCallbackCharacter(CBOperation, this)
		|| !SetCallbackReverseOrb(CBOperation, this)
		|| !SetCallbackTransform(CBOperation, this)
		|| !SetCallbackCharacterGameIn(CBOperation, this)
		|| !SetCallbackCharacterGameOut(CBOperation, this)
		|| !SetCallbackItem(CBOperation, this)
		|| !SetCallbackCashItem(CBOperation, this)
		|| !SetCallbackItemConvert(CBOperation, this)
		|| !SetCallbackSkill(CBOperation, this)
		|| !SetCallbackUIStatus(CBOperation, this)
		|| !SetCallbackGuildMaster(CBOperationGuildMaster, this)
		|| !SetCallbackGuildMasterCheck(CBOperationGuildMasterCheck, this)
		|| !SetCallbackGuildMember(CBOperationGuildMember, this)
		|| !SetCallbackGuildBattleHistory(CBOperationGuildMaster, this)
		|| !SetCallbackGuildRename(CBOperationGuildMaster, this)
		|| !SetCallbackGuildWarehouseMoney(CBOperationGuildMaster, this)
		|| !SetCallbackGuildRelation(CBOperationGuildMaster, this)
		|| !SetCallbackGuildItem(CBOperationGuildItem, this)
		|| !SetCallbackAccountWorld(CBOperation, this)
		|| !SetCallbackQuest(CBOperation, this)
		|| !SetCallbackQuestFlag(CBOperation, this)
		|| !SetCallbackBuddy(CBOperationBuddy, this)
		|| !SetCallbackMail(CBOperationMail, this)
		|| !SetCallbackMailItem(CBOperationMailItem, this)
		|| !SetCallbackConcurrentUser(CBOperation, this)
		|| !m_pAgsmLog->SetCallbackWriteLog(CBOperationLog, this)
		|| !SetCallbackPing(CBOperationPing, this)
		|| !SetCallbackAdmin(CBOperationAdmin, this)
		|| !SetCallbackCashItemBuyList(CBOperation, this)
		|| !SetCallbackCashItemBuyList2(CBOperation, this)
		|| !SetCallbackWantedCriminal(CBOperationWantedCriminal, this)
		|| !SetCallbackCastle(CBOperationCastle, this)
		|| !SetCallbackSiege(CBOperationSiege, this)
		|| !SetCallbackSiegeApplication(CBOperationSiegeApplication, this)
		|| !SetCallbackSiegeObject(CBOperationSiegeObject, this)
		|| !SetCallbackTax(CBOperation, this)
		//#############################
		//|| !SetCallbackArchlord(CBOperationArchlord, this)
		//|| !SetCallbackLordGuard(CBOperationLordGuard, this)
		)
		return FALSE;

	return TRUE;
	}




//	CBOperation/CBFinishOperation
//==============================================================
//
//	Common relay-side DB operation callback
//
BOOL AgsmRelay2::CBOperation(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;
	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperation,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery, pAgsdRelay2->m_nForcedPoolIndex);
	}


BOOL AgsmRelay2::CBFinishOperation(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	if (NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		pThis->OnSelectResult(pRowset, pAgsdRelay2);

	if (NULL != pAgsdRelay2)
		pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBFailOperation(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;

	if (AGSMDATABASE_OPERATION_UPDATE == pAgsdRelay2->m_eOperation)
		{
		if (AGSMRELAY_PARAM_ITEM == pAgsdRelay2->m_nParam)
			{
			AgsdRelay2Item *pAgsdRelay2Item = (AgsdRelay2Item *) pAgsdRelay2;

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%s]Item Update fail\n",
							pAgsdRelay2Item->m_szCharName,
							pAgsdRelay2Item->m_szDBID);
			AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);

			pAgsdRelay2->m_eOperation = AGSMDATABASE_OPERATION_INSERT;
			
			// 다시 갈거라 릴리즈하면 안된다.
			return CBOperation((PVOID) pAgsdRelay2, (PVOID) pThis, (PVOID) AGSMRELAY_PARAM_ITEM);
			}
		else if (AGSMRELAY_PARAM_ACCOUNT_WORLD == pAgsdRelay2->m_nParam)
			{
			AgsdRelay2AccountWorld *pAgsdRelay2AccountWorld = (AgsdRelay2AccountWorld *) pAgsdRelay2;

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%s]Bank Update fail\n",
							pAgsdRelay2AccountWorld->m_szAccountID,
							pAgsdRelay2AccountWorld->m_szBankMoney);
			AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);

			pAgsdRelay2->m_eOperation = AGSMDATABASE_OPERATION_INSERT;
			
			// 다시 갈거라 릴리즈하면 안된다.
			return CBOperation((PVOID) pAgsdRelay2, (PVOID) pThis, (PVOID) AGSMRELAY_PARAM_ACCOUNT_WORLD);
			}
		}

	if (NULL != pAgsdRelay2)
		pAgsdRelay2->Release();

	return TRUE;
	}




//	OnSelect result
//==============================================================
//
//	handles common(custom) select result operation
//
BOOL AgsmRelay2::OnSelectResult(AuRowset *pRowset, AgsdDBParam *pAgsdRelay)
	{
	ASSERT(NULL != pRowset);

	AgsdDBParamCustom csRelayCustom;

	//csRelayCustom.m_eOperation = AGSMRELAY_;
	csRelayCustom.m_lQueryIndex = 0;
	csRelayCustom.m_szQuery[0] = _ctextend;

	// headers 나중에...
	csRelayCustom.m_szHeaders[0] = _ctextend;

	csRelayCustom.m_ulRows = pRowset->GetRowCount();
	csRelayCustom.m_ulCols = pRowset->GetColCount();
	
	csRelayCustom.m_pBuffer = pRowset->GetHeadPtr();
	csRelayCustom.m_ulRowBufferSize = pRowset->GetRowBufferSize();
	
	for (UINT32 i=0; i<pRowset->GetColCount(); i++)
		csRelayCustom.m_lOffsets[i] = pRowset->GetOffset(i);

	return SendCustom(&csRelayCustom, AGSMDATABASE_PARAM_CUSTOM, pAgsdRelay->m_ulNID);
	}




/****************************************************/
/*		The Implementation of Relay Sender Thread	*/
/****************************************************/
//
Relay2Sender::Relay2Sender()
	{
	m_pModule	= NULL;
	m_pServer	= NULL;
	m_bStop		= TRUE;
	m_bInit		= FALSE;
	m_csLock.Init();
	}


Relay2Sender::~Relay2Sender()
	{
		m_bInit	= FALSE;
		m_csLock.Destroy();
	}




//	zzThread inherited
//====================================================
//
BOOL Relay2Sender::OnCreate()
	{
	m_bStop = FALSE;
	return TRUE;
	}


void Relay2Sender::OnTerminate()
	{
	// 남은 놈들 다 보낸다.
	if (!Send(TRUE))
		{
		// 에러에 의해 전송이 불가능한 경우다.
		AuAutoLock Lock(m_csLock);
		if (!Lock.Result()) return;

		INT32 lRemain = (INT32)m_QueueItem.size();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Name[%s]. Remained Queue Size=[%d] in Relay2Sender::OnTerminate()\n",
				  m_szName, lRemain);
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME_THREAD, strCharBuff);
		}
	}


INT32 g_lSleep = 10;
DWORD Relay2Sender::Do()
	{
#ifdef __PROFILE__
	FrameProfile_SetEnable( TRUE );
	FrameProfile_SetThreadName( m_szName );
#endif
	while (!m_bStop)
		{
#ifdef __PROFILE__
		FrameProfile_Init();
#endif
		if (!m_bInit || !m_pModule || !m_pServer)
			return 12;

		Send();

		Sleep(g_lSleep);
		}

	return 0;
	}




//	Internal
//=====================================================
//
void Relay2Sender::Remove(PVOID pvPacket)
	{
	m_QueueItem.pop_front();
	m_QueueLength.pop_front();
	}


PVOID Relay2Sender::Get(INT16 &nLength)
	{
	if (!m_bInit || m_QueueItem.size() <= 0)
		{
		nLength = 0;
		return NULL;
		}
	
	ASSERT(m_QueueItem.size() == m_QueueLength.size());

	nLength = m_QueueLength.front();
	return 	m_QueueItem.front();
	}


const INT32 _SEND_PER_LOOP	=	40;
INT32	g_lLoopCount = 0;
BOOL Relay2Sender::Send(BOOL bAll)
	{
	// Check Relay Server
	if (0 == m_pServer->m_dpnidServer)
		return FALSE;;
	
	AuAutoLock Lock(m_csLock);
	if (!Lock.Result()) return FALSE;
	
	INT32 lRemain = (INT32)m_QueueItem.size();
	if (0 == (g_lLoopCount % 20) && lRemain > 100)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Name[%s]. Remained Queue Size=[%d] in Relay2Sender::Send()\n",
				  m_szName, lRemain);
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME_THREAD, strCharBuff);
		}
	g_lLoopCount++;

	INT32 lToSend = 0;
	INT16 nPacketLength = 0;
	PVOID pvPacket = NULL;

	if (bAll)
		lToSend = lRemain;
	else
		lToSend = min(lRemain, _SEND_PER_LOOP);

	for (INT32 lSended = 0; lSended < lToSend; ++lSended)
		{
		pvPacket = Get(nPacketLength);
		if (!pvPacket || 1 > nPacketLength)
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "Name[%s]. Invalid Pakcet[%d][%d] in Relay2Sender::Send()\n",
					  m_szName, pvPacket, nPacketLength);
			AuLogFile_s(AGMSRELAY2_LOG_FILENAME_THREAD, strCharBuff);

			Remove(pvPacket);
			continue;
			}

		if (!m_pModule->SendPacket(pvPacket, nPacketLength, m_pServer->m_dpnidServer))
			return FALSE;

		Remove(pvPacket);
		}

	return TRUE;
	}

#undef _SEND_PER_LOOP


void Relay2Sender::Stop()
	{
	m_bStop = TRUE;
	}




//	Set(Init)
//=====================================================
//
BOOL Relay2Sender::Set(INT32 lBufferSize, AgsModule* pModule, AgsdServer* pServer, CHAR *pszName)
	{
	if (!pModule || !pServer || !m_csBuffer.Init(lBufferSize))
		return FALSE;

	m_pModule = pModule;
	m_pServer = pServer;
	m_bInit = TRUE;
	
	strcpy(m_szName, pszName ? pszName : _T(""));

	return TRUE;
	}




//	Push(to send)
//=====================================================
//
BOOL Relay2Sender::Push(PVOID pvPacket, INT16 nPacketLength)
	{
	if (!m_bInit)
		return FALSE;

	AuAutoLock Lock(m_csLock);
	if (!Lock.Result()) return FALSE;

	PVOID pvBuffer = m_csBuffer.Alloc(nPacketLength);
	::CopyMemory(pvBuffer, pvPacket, nPacketLength);
	m_QueueItem.push_back(pvBuffer);
	m_QueueLength.push_back(nPacketLength);

	return TRUE;
	}


BOOL Relay2Sender::SendAll()
	{
	return Send(TRUE);
	}




//	Ping - 2005.04.07. steeple
//=====================================================
//
BOOL AgsmRelay2::CBPingSend(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	DWORD* pdwSendTickCount = (DWORD*)pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;

	if(!pdwSendTickCount || !pThis)
		return FALSE;
	
	return pThis->SendPing(*pdwSendTickCount);
	}


BOOL AgsmRelay2::SendPing(DWORD dwSendTickCount)
	{
	// 패킷 새로 만들기 귀차나서 ConCurrentUser Packet 을 활용!!!
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	CHAR szTickCount[21];
	sprintf(szTickCount, "%u", dwSendTickCount);

	INT16 nPacketLength	= 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketPingSend = m_csPacketConcurrentUser.MakePacket(FALSE, &nPacketLength, 0,
																   &nOperation,
																   szTickCount,
																   NULL,
																   NULL,
																   NULL
																   );
	if (!pvPacketPingSend)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketPingSend, AGSMRELAY_PARAM_PING);
	m_csPacketConcurrentUser.FreePacket(pvPacketPingSend);
	
	return bResult;
	}


BOOL AgsmRelay2::CBOperationResultPing(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2Ping *pAgsdRelay2 = (AgsdRelay2Ping *) pData;
	
	// Auto release AgsdRelay2
	AgsdRelay2AutoPtr Auto(pAgsdRelay2);	

	if (AGSMDATABASE_OPERATION_SELECT != pAgsdRelay2->m_eOperation)
		return FALSE;

	pThis->m_pAgsmAdmin->ProcessPingRelayReceive(pAgsdRelay2->m_dwPingSendTickCount);

	return TRUE;
	}


BOOL AgsmRelay2::SendSelectResultPing(UINT32 ulNID, DWORD dwSendTickCount)
	{
	if (!ulNID)
		return FALSE;

	AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(ulNID);
	if(!pcsGameServer)
		return FALSE;

	INT32 ulNewNID = pcsGameServer->m_dpnidServer;

	INT16	nPacketLength	= 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	CHAR szTickCount[21];
	sprintf(szTickCount, "%u", dwSendTickCount);

	PVOID pvPacketPingSend = m_csPacketConcurrentUser.MakePacket(FALSE, &nPacketLength, 0,
																   &nOperation,
																   szTickCount,
																   NULL,
																   NULL,
																   NULL
																   );
	if (!pvPacketPingSend)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketPingSend, AGSMRELAY_PARAM_PING, ulNewNID);
	m_csPacketConcurrentUser.FreePacket(pvPacketPingSend);

	return bResult;
	}


BOOL AgsmRelay2::CBOperationPing(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationPing,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationPing(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	if (NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		pThis->OnSelectResultPing(pRowset, pAgsdRelay2);

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultPing(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	if(!pAgsdRelay2)
		return FALSE;

	SendSelectResultPing(pAgsdRelay2->m_ulNID, ((AgsdRelay2Ping*)pAgsdRelay2)->m_dwPingSendTickCount);
	return TRUE;
	}


BOOL AgsmRelay2::OnParamPing(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Ping *pcsRelayPing = new AgsdRelay2Ping;

	CHAR* pszTickCount = NULL;

	m_csPacketConcurrentUser.GetField(FALSE, pvPacket, 0,
								   &pcsRelayPing->m_eOperation,	
								   &pszTickCount,
								   NULL,
								   NULL
								   );

	if (AGSMDATABASE_OPERATION_SELECT != pcsRelayPing->m_eOperation)
		{
		pcsRelayPing->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : Ping Packet is Invalid()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);
		return FALSE;
		}
	
	CHAR szTickCount[21];
	memset(szTickCount, 0, sizeof(szTickCount));
	if(pszTickCount)
		memcpy(szTickCount, pszTickCount, sizeof(CHAR) * 20);

	pcsRelayPing->m_ulNID = ulNID;
	pcsRelayPing->m_dwPingSendTickCount = (DWORD)_atoi64(szTickCount);	// atol >> _atoi64. long range 를 넘어가서 이렇게 바꿈.

	return EnumCallback(AGSMRELAY_PARAM_PING, (PVOID)pcsRelayPing, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBBufferingSend(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	PVOID pvPacket = pData;
	INT16 nPacketLength = (INT16) pCustData;
	
	if (NULL == pThis->m_pSender  ||
		FALSE == pThis->m_pSender->Push(pvPacket, nPacketLength))
		return FALSE;
		
	return TRUE;
	}