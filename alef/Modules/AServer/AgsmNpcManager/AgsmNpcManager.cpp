#include "AgsmNpcManager.h"
#include "StrUtil.h"
#include <AgppEventNPCDialog.h>
#include <AgpmEventNPCDialog.h>
#include <AgpmCharacter.h>
#include <AgsmCharacter.h>
#include <AgsmItem.h>
#include <AgsmItemManager.h>
#include <AgsmSkill.h>
#include <AgsmSystemMessage.h>
#include <AgpmSummons.h>
#include <AgpmEventBinding.h>
#include <AgsmEventSystem.h>
#include <AgsmBattleGround.h>
#include <AgpmGuild.h>
#include <AgpmAuction.h>
#include <AgpmReturnToLogin.h>
#include <AgpmWantedCriminal.h>
#include <AgpmBuddy.h>
#include <AgsmBuddy.h>
#include <AgpdItemTemplate.h>
#include <AgsmEpicZone.h>
#include <AgpmParty.h>
#include <AgpmEpicZone.h>
#include <ApmMap.h>
#include <AgpmArchlord.h>
#include <AgpmSiegeWar.h>
#include <AgppGuild.h>
#include <AgpmTitle.h>
#include <AgpmLog.h>
#include <AgsmDeath.h>

AgsmNpcManager* AgsmNpcManager::m_pInstance = NULL;

#define NPCMANAGER_HASHKEY "npcmanager_key"

AgsmNpcManager::AgsmNpcManager()
{
	m_pInstance = this;

	SetModuleName("AgsmNpcManager");
	//SetPacketType(AGPM_BATTLEGROUND_PACKET_TYPE);

	m_pagpmCharacter = NULL;
	m_pagsmCharacter = NULL;
	m_pagpmItem = NULL;
	m_pagsmItem = NULL;
	m_pagpmGrid = NULL;
	m_pagsmItemManager = NULL;
	m_pagpmSkill = NULL;
	m_pagsmSkill = NULL;
	m_pagsmSystemMessage = NULL;
	m_pagpmSummons = NULL;
	m_pagpmEventBinding = NULL;
	m_pagsmEventSystem = NULL;
	m_pagpmGuild = NULL;
	m_pagpmAuction = NULL;
	m_pagpmReturnToLogin = NULL;
	m_pagpmWantedCriminal = NULL;
	m_pagsmEpicZone = NULL;
	m_pagpmParty = NULL;
	m_pagpmEpicZone = NULL;
	m_papmMap = NULL;
	m_pagpmArchlord = NULL;
	m_pagpmSiegeWar = NULL;
	m_pagpmTitle = NULL;
	m_pagpmLog = NULL;
	m_pagsmDeath = NULL;
	m_pagpmBuddy = NULL;
	m_pagsmBuddy = NULL;
}

AgsmNpcManager::~AgsmNpcManager()
{

}


BOOL AgsmNpcManager::OnInit()
{
	m_LuaList.Initialize(200, (OnRegister_CFunction)AgsmNpcManager::LuaRegister);

	if(!LoadXmlFile())
		return FALSE;

	m_pagpmEventNPCDialog	= (AgpmEventNPCDialog*)GetModule("AgpmEventNPCDialog");
	m_pagsmCharacter		= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmItem				= (AgpmItem*)GetModule("AgpmItem");
	m_pagsmItem				= (AgsmItem*)GetModule("AgsmItem");
	m_pagpmGrid				= (AgpmGrid*)GetModule("AgpmGrid");
	m_pagsmItemManager		= (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pagpmSkill			= (AgpmSkill*)GetModule("AgpmSkill");
	m_pagsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	m_pagpmSummons			= (AgpmSummons*)GetModule("AgpmSummons");
	m_pagpmEventBinding		= (AgpmEventBinding*)GetModule("AgpmEventBinding");
	m_pagsmEventSystem		= (AgsmEventSystem*)GetModule("AgsmEventSystem");
	m_pagsmBattleGround		= (AgsmBattleGround*)GetModule("AgsmBattleGround");
	m_pagpmGuild			= (AgpmGuild*)GetModule("AgpmGuild");
	m_pagpmAuction			= (AgpmAuction*)GetModule("AgpmAuction");
	m_pagpmReturnToLogin	= (AgpmReturnToLogin*)GetModule("AgpmReturnToLogin");
	m_pagpmWantedCriminal	= (AgpmWantedCriminal*)GetModule("AgpmWantedCriminal");
	m_pagpmBuddy			= (AgpmBuddy*)GetModule("AgpmBuddy");
	m_pagsmBuddy			= (AgsmBuddy*)GetModule("AgsmBuddy");
	m_pagsmEpicZone			= (AgsmEpicZone*)GetModule("AgsmEpicZone");
	m_pagpmParty			= (AgpmParty*)GetModule("AgpmParty");
	m_pagpmEpicZone			= (AgpmEpicZone*)GetModule("AgpmEpicZone");
	m_papmMap				= (ApmMap*)GetModule("ApmMap");
	m_pagpmArchlord			= (AgpmArchlord*)GetModule("AgpmArchlord");
	m_pagpmSiegeWar			= (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	m_pagpmTitle			= (AgpmTitle*)GetModule("AgpmTitle");
	m_pagpmLog				= (AgpmLog*)GetModule("AgpmLog");
	m_pagsmDeath			= (AgsmDeath*)GetModule("AgsmDeath");

	if( !m_pagpmEventNPCDialog ||
		!m_pagsmCharacter || !m_pagsmCharacter ||
		!m_pagpmItem || !m_pagsmItem || !m_pagsmItemManager ||
		!m_pagpmGrid ||
		!m_pagpmSkill || !m_pagsmSkill ||
		!m_pagsmSystemMessage ||
		!m_pagpmSummons ||
		!m_pagpmEventBinding ||
		!m_pagsmEventSystem || !m_pagsmBattleGround ||
		!m_pagpmGuild ||
		!m_pagpmAuction ||
		!m_pagpmReturnToLogin ||
		!m_pagpmWantedCriminal || !m_papmMap ||
		!m_pagsmEpicZone || !m_pagpmParty || !m_pagpmEpicZone ||
		!m_pagpmArchlord || !m_pagpmSiegeWar ||
		!m_pagpmTitle || !m_pagpmLog || !m_pagsmDeath ||
		!m_pagpmBuddy ||
		!m_pagsmBuddy)
		return FALSE;

	if( !m_pagpmEventNPCDialog->SetCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST_MESSAGEBOX, OnGrantMessageBox, this) ||
		!m_pagpmEventNPCDialog->SetCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST_MENU, OnGrantMenu, this) )
		return FALSE;

	return TRUE;
}

BOOL AgsmNpcManager::OnDestroy()
{
	m_LuaList.Destroy();

	if(!m_NpcExDataMap.empty())
		m_NpcExDataMap.clear();

	return TRUE;
}

BOOL AgsmNpcManager::LoadNpcManagerFile(char* fileName)
{
	if(!m_XmlData.LoadFile(fileName))
		return FALSE;

	AuXmlNode* pLinkFileNode = m_XmlData.FirstChild("NpcManager");
	if(!pLinkFileNode)
		return FALSE;

	AuXmlNode* pNpcNode   = pLinkFileNode->FirstChild("LinkFileName");
	if(!pNpcNode)
		return FALSE;

	for(AuXmlElement* pElem = pNpcNode->FirstChildElement("NPC"); pElem; pElem = pElem->NextSiblingElement())
	{
		CNpcExData* pNpcExData = new CNpcExData;
		if(!pNpcExData)
			continue;

		AuXmlElement* pElemNID = pElem->FirstChildElement("NPCID");
		if(pElemNID)
			pNpcExData->NPCID = atoi(pElemNID->GetText());

		AuXmlElement* pElemSystem = pElem->FirstChildElement("System");
		if(pElemSystem)
			pNpcExData->IsSystemNPC = atoi(pElemSystem->GetText());

		AuXmlElement* pElemFile = pElem->FirstChildElement("LUAFile");
		if(pElemFile)
			strcpy(pNpcExData->strFileName, pElemFile->GetText());

		pNpcExData->pLuaPack = new CLuaStreamPack;
		if(!pNpcExData->pLuaPack)
		{
			TRACE("error : %s[%d]\n", __FUNCDNAME__, __LINE__);

			delete pNpcExData;
			continue;
		}

		CHAR szFileName[MAX_PATH] = { 0, };
		sprintf_s(szFileName, sizeof(szFileName), "ini\\NPCManager\\%s", pNpcExData->strFileName);
		INT32 nPlainText;
		pElemFile->Attribute("PlainText", &nPlainText);

		//if(!pNpcExData->pLuaPack->Open(szFileName, (nPlainText) ?  NULL : (Decrypt_CFunction)AgsmNpcManager::Decrypt))
		if (!pNpcExData->pLuaPack->Open(szFileName, 0))
		{
			TRACE("error : %s[%d]\n", __FUNCDNAME__, __LINE__);

			delete pNpcExData->pLuaPack;
			delete pNpcExData;
			
			continue;
		}

		m_NpcExDataMap.insert(CNpcExDataMap::value_type(pNpcExData->NPCID, pNpcExData));
	}

	return TRUE;
}

BOOL AgsmNpcManager::LoadXmlFile()
{
	ApMutualEx m_Lock;
	AuAutoLock pLock(m_Lock);
	if(!pLock.Result())
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//
	if(!m_NpcExDataMap.empty())
		m_NpcExDataMap.clear();

	LoadNpcManagerFile("ini\\NPCManager\\NpcManager.xml");
	LoadNpcManagerFile("ini\\NPCManager\\SystemNpcManager.xml");

	//////////////////////////////////////////////////////////////////////////
	//
	if(!m_MessageXml.LoadFile("ini\\NPCManager\\NpcDialog.xml"))
		return FALSE;

	AuXmlNode* pNpcDialogNode = m_MessageXml.FirstChild("NpcDialog");
	if(!pNpcDialogNode)
		return FALSE;

	m_pMessageBoxNode   = pNpcDialogNode->FirstChild("MessageBox");
	if(!m_pMessageBoxNode)
		return FALSE;

	m_pMenuNode			= pNpcDialogNode->FirstChild("Menu");
	if(!m_pMenuNode)
		return FALSE;

	return TRUE;
}

BOOL AgsmNpcManager::Decrypt( char* buffer, unsigned bufferSize )
{
	AuMD5Encrypt md5;
	return md5.DecryptString( NPCMANAGER_HASHKEY, buffer, bufferSize ) ? true : false;
}

int AgsmNpcManager::LuaRegister(lua_State* pLuaState)
{
	if(pLuaState)
	{
		lua_register(pLuaState, "ShowMessageBox",					AgsmNpcManager::OnShowMessageBox);
		lua_register(pLuaState, "ShowMenu",							AgsmNpcManager::OnShowMenu);
		
		lua_register(pLuaState, "GetTID",							AgsmNpcManager::GetCharacterTID);
		lua_register(pLuaState, "GetRace",							AgsmNpcManager::GetCharacterRace);
		lua_register(pLuaState, "GetLevel",							AgsmNpcManager::GetCharacterLevel);
		lua_register(pLuaState, "GetExp",							AgsmNpcManager::GetCharacterExp);

		lua_register(pLuaState, "GetInventoryEmptyCount",			AgsmNpcManager::GetInventoryEmptySlotCount);
		lua_register(pLuaState, "GetCashInventoryEmptyCount",		AgsmNpcManager::GetCashInventoryEmptySlotCount);
		lua_register(pLuaState, "CheckItemCount",					AgsmNpcManager::CheckItemCount);
		lua_register(pLuaState, "RobItem",							AgsmNpcManager::RobItem);
		lua_register(pLuaState, "GiveItem",							AgsmNpcManager::GiveItem);
		lua_register(pLuaState, "GiveTimeLimitItem",				AgsmNpcManager::GiveTimeLimitItem);

		lua_register(pLuaState, "CastSkill",						AgsmNpcManager::CastSkill);
		lua_register(pLuaState, "Move",								AgsmNpcManager::Move);
		lua_register(pLuaState, "ReturnTown",						AgsmNpcManager::ReturnTown);
		lua_register(pLuaState, "SetDefaultInvincible",				AgsmNpcManager::SetDefaultInvincible);

		lua_register(pLuaState, "IsArchlord",						AgsmNpcManager::IsArchlord);
		lua_register(pLuaState, "IsTransform",						AgsmNpcManager::IsTransform);
		lua_register(pLuaState, "IsRide",							AgsmNpcManager::IsRide);
		lua_register(pLuaState, "GetGuildMemberRank",				AgsmNpcManager::GetGuildMemberRank);
		lua_register(pLuaState, "GetAuctionCount",					AgsmNpcManager::GetAuctionCount);
		lua_register(pLuaState, "IsCriminal",						AgsmNpcManager::IsCriminal);

		lua_register(pLuaState, "CheckAttendance",					AgsmNpcManager::CheckAttendance);
		lua_register(pLuaState, "CheckEventUser",					AgsmNpcManager::CheckEventUser);
		lua_register(pLuaState, "EventUserFlagUpdate",				AgsmNpcManager::EventUserFlagUpdate);

		lua_register(pLuaState, "CheckEventBattleGround",			AgsmNpcManager::CheckEventBattleGround);
		
		lua_register(pLuaState, "CallSystemNPC",					AgsmNpcManager::CallSystemNPC);
		lua_register(pLuaState, "SetChangeName",					AgsmNpcManager::SetChangeName);  // 캐릭명 변경 시스템 - arycoat 2009.03.
		lua_register(pLuaState, "ReturnToLogin",					AgsmNpcManager::ReturnToLogin);
		lua_register(pLuaState, "CerariumOrb",						AgsmNpcManager::CerariumOrb);
		lua_register(pLuaState, "ServerMove",						AgsmNpcManager::ServerMove);	//JK_특성화서버

		lua_register(pLuaState, "IsEquipUnableItemInEpicZone",		AgsmNpcManager::IsEquipUnableItemInEpicZone);
		lua_register(pLuaState, "IsEnableEntrance",					AgsmNpcManager::IsEnableEntrance);
		lua_register(pLuaState, "SetEpicBossZoneStartTime",			AgsmNpcManager::SetEpicBossZoneStartTime);
		lua_register(pLuaState, "MoveIncludeParty",					AgsmNpcManager::MoveIncludeParty);
		lua_register(pLuaState, "IsPartyLeader",					AgsmNpcManager::IsPartyLeader);
		lua_register(pLuaState, "GetBossZoneRemainTime",			AgsmNpcManager::GetBossZoneRemainTime);
		lua_register(pLuaState, "ShowOptionMessageBox",				AgsmNpcManager::ShowOptionMessageBox);
		lua_register(pLuaState, "ShowTitleMain",					AgsmNpcManager::ShowTitleMain);
		lua_register(pLuaState, "ProcessArchlordCastleAttackGuild",	AgsmNpcManager::ProcessArchlordCastleAttackGuild);
		lua_register(pLuaState, "HasTitle",							AgsmNpcManager::HasTitle);
		lua_register(pLuaState, "UserCreationDateCheck",			AgsmNpcManager::UserCreationDateCheck);
		lua_register(pLuaState, "CharismaPointAdd",					AgsmNpcManager::CharismaPointAdd);
		lua_register(pLuaState, "CharismaPointSub",					AgsmNpcManager::CharismaPointSub);
		lua_register(pLuaState, "GetHaveTitleNumber",				AgsmNpcManager::GetHaveTitleNumber);
		lua_register(pLuaState, "AddMoney",							AgsmNpcManager::AddMoney);
		lua_register(pLuaState, "AddExpByPercent",					AgsmNpcManager::AddExpByPercent);
		lua_register(pLuaState, "GetMurderPoint",					AgsmNpcManager::GetMurderPoint);
		lua_register(pLuaState, "IsInGuild",						AgsmNpcManager::IsInGuild);
		lua_register(pLuaState, "ShowBuddyMain",					AgsmNpcManager::ShowBuddyMain);
		lua_register(pLuaState, "GetMentorCID",						AgsmNpcManager::GetMentorCID);
		lua_register(pLuaState, "GetSkillLevel",					AgsmNpcManager::GetSkillLevel);
		lua_register(pLuaState, "GetHaveCharismaPoint",				AgsmNpcManager::GetHaveCharismaPoint);
	}

	return 0;
}

BOOL AgsmNpcManager::ExecScriptMain(INT32 CID, INT32 NID, INT32 EID, INT32 STEP)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	CNpcExDataMap::iterator it = m_NpcExDataMap.find(NID);
	if(it == m_NpcExDataMap.end())
		return FALSE;

	ExecScriptMain(CID, it->second->NPCID, EID, STEP, it->second->pLuaPack);

	return TRUE;
}

INT32 AgsmNpcManager::ExecScriptMain(INT32 CID, INT32 NID, INT32 lEvnetIndex, INT32 lStep, CLuaStreamPack* pLuaPack)
{
	if( /*CID == 0 || lEvnetIndex == 0 ||*/ !pLuaPack )
		return 0;

	lua_State* pLuaState = m_LuaList.GetHandle();
	if(!pLuaState)
		return 0;

	if(!pLuaPack->dobuffer(pLuaState))
		return 0;

	lua_getglobal(pLuaState, "Main");
	lua_pushnumber(pLuaState, CID);
	lua_pushnumber(pLuaState, NID);
	lua_pushnumber(pLuaState, lEvnetIndex);
	lua_pushnumber(pLuaState, lStep);

	int status = lua_pcall(pLuaState, 4, 1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		
		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return 0;
	}
	
	int ret = 0;
	while (lua_gettop(pLuaState))
	{
		switch (lua_type(pLuaState, lua_gettop(pLuaState)))
		{
		case LUA_TNUMBER: 
			ret = (int)lua_tonumber(pLuaState, lua_gettop(pLuaState));
			break;
		default: 
			ret = -1;
			break;
		}
		lua_pop(pLuaState, 1 );
	}
	
	m_LuaList.RestoreHandle(pLuaState);
	
	return ret;
}

INT32 AgsmNpcManager::ExecScriptMessageBox(INT32 CID, INT32 NID, INT32 lEvnetIndex, INT32 lStep, CLuaStreamPack* pLuaPack, BOOL bBtnIndex, CHAR* strString, INT32 ItemID)
{
	if( /*CID == 0 || lEvnetIndex == 0 ||*/ !pLuaPack )
		return 0;

	lua_State* pLuaState = m_LuaList.GetHandle();
	if(!pLuaState)
		return 0;
	
	if(!pLuaPack->dobuffer(pLuaState))
		return 0;
	
	lua_getglobal(pLuaState, "OnMessageBox");
	lua_pushnumber(pLuaState, CID);
	lua_pushnumber(pLuaState, NID);
	lua_pushnumber(pLuaState, lEvnetIndex);
	lua_pushnumber(pLuaState, lStep);
	
	lua_pushnumber(pLuaState, bBtnIndex);
	lua_pushstring(pLuaState, strString);
	lua_pushnumber(pLuaState, ItemID);

	int status = lua_pcall(pLuaState, 7, 1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		
		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return 0;
	}
	
	int ret = 0;
	while (lua_gettop(pLuaState))
	{
		switch (lua_type(pLuaState, lua_gettop(pLuaState)))
		{
		case LUA_TNUMBER: 
			ret = (int)lua_tonumber(pLuaState, lua_gettop(pLuaState));
			break;
		default: 
			ret = -1;
			break;
		}
		lua_pop(pLuaState, 1 );
	}
	
	m_LuaList.RestoreHandle(pLuaState);
	
	return ret;
}

INT32 AgsmNpcManager::ExecScriptMenu(INT32 CID, INT32 NID, INT32 lEvnetIndex, INT32 lStep, CLuaStreamPack* pLuaPack)
{
	if( /*CID == 0 || lEvnetIndex == 0 ||*/ !pLuaPack )
		return 0;

	lua_State* pLuaState = m_LuaList.GetHandle();
	if(!pLuaState)
		return 0;

	if(!pLuaPack->dobuffer(pLuaState))
		return 0;

	lua_getglobal(pLuaState, "OnMenu");
	lua_pushnumber(pLuaState, CID);
	lua_pushnumber(pLuaState, NID);
	lua_pushnumber(pLuaState, lEvnetIndex);
	lua_pushnumber(pLuaState, lStep);
	
	int status = lua_pcall(pLuaState, 4, 1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		
		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return 0;
	}
	
	int ret = 0;
	while (lua_gettop(pLuaState))
	{
		switch (lua_type(pLuaState, lua_gettop(pLuaState)))
		{
		case LUA_TNUMBER: 
			ret = (int)lua_tonumber(pLuaState, lua_gettop(pLuaState));
			break;
		default: 
			ret = -1;
			break;
		}
		lua_pop(pLuaState, 1 );
	}
	
	m_LuaList.RestoreHandle(pLuaState);
	
	return ret;
}

CHAR* AgsmNpcManager::GetFileName(INT32 nNPCID)
{
	if(!nNPCID)
		return NULL;

	CNpcExDataMap::iterator it = m_NpcExDataMap.find(nNPCID);
	if(it != m_NpcExDataMap.end())
		return it->second->strFileName;

	return NULL;
}

BOOL AgsmNpcManager::OnEventRequest( AgpdCharacter* pcsCharacter, AgpdCharacter* pcsNpc, INT32 lEvnetIndex /* = 0 */, INT32 lStep /* = 0 */ )
{
	if(!pcsCharacter || !pcsNpc)
		return FALSE;

	AuAutoLock pLock(pcsCharacter->m_Mutex);
	if(!pLock.Result())
		return FALSE;

	if(m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	CNpcExDataMap::iterator it = m_NpcExDataMap.find(pcsNpc->m_nNPCID);
	if(it == m_NpcExDataMap.end())
		return FALSE;

	ExecScriptMain(pcsCharacter->m_lID, it->second->NPCID, lEvnetIndex, lStep, it->second->pLuaPack);

	return TRUE;
}

int AgsmNpcManager::OnShowMessageBox(lua_State* L)
{
	AgsmNpcManager*	pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -7);
	INT32 NID = (INT32)lua_tonumber(L, -6);
	INT32 EID = (INT32)lua_tonumber(L, -5);
	INT32 STEP = (INT32)lua_tonumber(L, -4);

	INT32 MessageType = (INT32)lua_tonumber(L, -3);
	INT32 MessageGroup = (INT32)lua_tonumber(L, -2);
	INT32 MessageID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 7);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	CHAR strMessageGroup[64];
	memset(strMessageGroup, 0, sizeof(strMessageGroup));
	sprintf_s(strMessageGroup, sizeof(strMessageGroup), "Group_%d", MessageGroup);
	AuXmlElement* pElemGroup = pThis->m_pMessageBoxNode->FirstChildElement(strMessageGroup);
	if(!pElemGroup)
		return 0;

	CHAR strMessage[64];
	memset(strMessage, 0, sizeof(strMessage));
	sprintf_s(strMessage, sizeof(strMessage), "Message_%d", MessageID);
	AuXmlElement* pElem = pElemGroup->FirstChildElement(strMessage);
	if(!pElem)
		return 0;

	PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX pPacket(CID, NID, EID, STEP);
	pPacket.lMessageBoxType = MessageType;
	strcpy(pPacket.strMessage, pElem->GetText());
	pThis->SendPacketUser(pPacket, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	return 1;
}

int AgsmNpcManager::OnShowMenu(lua_State* L)
{
	AgsmNpcManager*	pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -3);
	INT32 NID = (INT32)lua_tonumber(L, -2);

	INT32 MenuID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 3);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	CHAR strMenuID[64];
	memset(strMenuID, 0, sizeof(strMenuID));
	sprintf_s(strMenuID, sizeof(strMenuID), "Menu_%d", MenuID);
	AuXmlElement* pElem = pThis->m_pMenuNode->FirstChildElement(strMenuID);
	if(!pElem)
		return 0;

	PACKET_EVENTNPCDIALOG_GRANT_MENU pPacket(CID, NID);
	AuXmlElement* pElemType = pElem->FirstChildElement("UIType");
	if(pElemType)
	{
		CHAR* pText = (CHAR*)pElemType->GetText();
		if(pText)
			pPacket.lMenuType = (INT32)atoi(pText);
	}

	AuXmlElement* pElemTitle = pElem->FirstChildElement("TITLE");
	if(pElemTitle)
	{
		CHAR* pTitle = (CHAR*)pElemTitle->GetText();
		if(pTitle)
			strcpy_s(pPacket.strTitle, MAX_BUTTON_TEXT_SIZE, pTitle);
	}

	AuXmlElement* pElemDialog = pElem->FirstChildElement("Dialog");
	if(pElemDialog)
	{
		CHAR* pDialog = (CHAR*)pElemDialog->GetText();
		if(pDialog)
			strcpy_s(pPacket.strDialog, MESSAGE_MAX_SIZE, pDialog);
	}

	AuXmlElement* pElemButton = pElem->FirstChildElement("Button");

	for(INT32 i = 0; i < MAX_BUTTON_COUNT; ++i)
	{
		if(!pElemButton)
			break;

		AuXmlElement* pButtonEID = pElemButton->FirstChildElement("EID");
		if(pButtonEID)
		{
			CHAR* pEID = (CHAR*)pButtonEID->GetText();
			if(pEID)
				pPacket.ButtonArray[i].EID = (INT32)atoi(pEID);
		}

		AuXmlElement* pButtonSTEP = pElemButton->FirstChildElement("STEP");
		if(pButtonSTEP)
		{
			CHAR* pSTEP = (CHAR*)pButtonSTEP->GetText();
			if(pSTEP)
				pPacket.ButtonArray[i].STEP = (INT32)atoi(pSTEP);
		}

		AuXmlElement* pButtonItemTID = pElemButton->FirstChildElement("ItemTID");
		if(pButtonItemTID)
		{
			CHAR* pItemTID = (CHAR*)pButtonItemTID->GetText();
			if(pItemTID)
				pPacket.ButtonArray[i].ItemTID = (INT32)atoi(pItemTID);
		}

		AuXmlElement* pButtonTEXT = pElemButton->FirstChildElement("TEXT");
		if(pButtonTEXT)
		{
			CHAR* pTEXT = (CHAR*)pButtonTEXT->GetText();
			if(pTEXT)
				strcpy_s(pPacket.ButtonArray[i].strText, MAX_BUTTON_TEXT_SIZE, pTEXT);
		}

		pElemButton = pElemButton->NextSiblingElement();
	}

	pThis->SendPacketUser(pPacket, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	return 1;
}

BOOL AgsmNpcManager::OnGrantMessageBox(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmNpcManager* pThis = (AgsmNpcManager*)pClass;
	PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX* pPacket = (PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX*)pData;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pPacket->CID);
	/*AgpdCharacter* pcsNpc		= pThis->m_pagpmCharacter->GetCharacter(pPacket->NID);*/
	if(!pcsCharacter /*|| !pcsNpc*/)
		return FALSE;

	AuAutoLock pLock(pcsCharacter->m_Mutex);
	if(!pLock.Result())
		return FALSE;

	if(pThis->m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	CNpcExDataMap::iterator it = pThis->m_NpcExDataMap.find(pPacket->NID);
	if(it == pThis->m_NpcExDataMap.end())
		return FALSE;

	INT32 EID		= pPacket->lEvnetIndex;
	INT32 STEP		= pPacket->lStep;
	INT32 BTIndex	= pPacket->bBtnIndex;
	CHAR* strString	= ""/*pPacket->strString*/;
	INT32 ItemID	= pPacket->nItemID;

	pThis->ExecScriptMessageBox(pcsCharacter->m_lID, it->second->NPCID, EID, pPacket->lStep, it->second->pLuaPack, BTIndex, strString, ItemID);

	return TRUE;
}

BOOL AgsmNpcManager::OnGrantMenu(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmNpcManager* pThis = (AgsmNpcManager*)pClass;
	PACKET_EVENTNPCDIALOG_REQUEST_MENU* pPacket = (PACKET_EVENTNPCDIALOG_REQUEST_MENU*)pData;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pPacket->CID);
	/*AgpdCharacter* pcsNpc		= pThis->m_pagpmCharacter->GetCharacter(pPacket->NID);*/
	if(!pcsCharacter /*|| !pcsNpc*/)
		return FALSE;

	AuAutoLock pLock(pcsCharacter->m_Mutex);
	if(!pLock.Result())
		return FALSE;

	if(pThis->m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	CNpcExDataMap::iterator it = pThis->m_NpcExDataMap.find(pPacket->NID);
	if(it == pThis->m_NpcExDataMap.end())
		return FALSE;

	pThis->ExecScriptMenu(pcsCharacter->m_lID, it->second->NPCID, pPacket->lEvnetIndex, pPacket->lStep, it->second->pLuaPack);

	return TRUE;
}

int AgsmNpcManager::GetCharacterTID(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	lua_pushinteger(L, pcsCharacter->m_pcsCharacterTemplate->m_lID);

	return 1;
}

int AgsmNpcManager::GetCharacterRace(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	AuRace pRace = pThis->m_pagpmCharacter->GetCharacterRace(pcsCharacter);

	lua_pushinteger(L, pRace.detail.nRace);

	return 1;
}

int AgsmNpcManager::GetCharacterLevel(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 lLevel = pThis->m_pagpmCharacter->GetLevel(pcsCharacter);

	lua_pushinteger(L, lLevel);

	return 1;
}

int AgsmNpcManager::GetCharacterExp(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT64 lExp = pThis->m_pagpmCharacter->GetExp(pcsCharacter);

	lua_pushinteger(L, lExp);

	return 1;
}

int AgsmNpcManager::GetInventoryEmptySlotCount(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 lCount = 0;
	AgpdGrid* pagpdGrid = pThis->m_pagpmItem->GetInventory(pcsCharacter);
	if (pagpdGrid) 
	{
		lCount = (pagpdGrid->m_lGridCount - pagpdGrid->m_lItemCount);
	}
	
	lua_pushinteger(L, lCount);

	return 1;
}

int AgsmNpcManager::GetCashInventoryEmptySlotCount(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 lCount = 0;
	AgpdGrid* pagpdGrid = pThis->m_pagpmItem->GetCashInventoryGrid(pcsCharacter);
	if (pagpdGrid) 
	{
		lCount = (pagpdGrid->m_lGridCount - pagpdGrid->m_lItemCount);
	}

	lua_pushinteger(L, lCount);

	return 1;
}

int AgsmNpcManager::CheckItemCount( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);

	INT32 ItemTID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 lCount = 0;
	AgpdGrid* pagpdGrid1 = pThis->m_pagpmItem->GetInventory(pcsCharacter);
	if (pagpdGrid1) 
	{
		lCount += pThis->m_pagpmGrid->GetCountByTemplate(pagpdGrid1, AGPDGRID_ITEM_TYPE_ITEM, ItemTID);
	}

	AgpdGrid* pagpdGrid2 = pThis->m_pagpmItem->GetCashInventoryGrid(pcsCharacter);
	if (pagpdGrid2)
	{
		lCount += pThis->m_pagpmGrid->GetCountByTemplate(pagpdGrid2, AGPDGRID_ITEM_TYPE_ITEM, ItemTID);	
	}

	lua_pushinteger(L, lCount);

	return 1;
}

int AgsmNpcManager::CheckEventBattleGround(lua_State* L)
{
	AgsmNpcManager *pThis = AgsmNpcManager::GetInstance();

	INT32 lEventBattleGround = pThis->m_pagsmBattleGround->IsEventBattleGroud();

	lua_pushinteger(L, lEventBattleGround);

	return 1;
}

INT32 AgsmNpcManager::RobItem(AgpdCharacter* pcsCharacter, AgpdGrid* pagpdGrid, INT32 ItemTID, INT32 Count)
{
	AgpdGridItem* pagpdGridItem;
	AgpdItem *pAgpdItem;

	INT32 lIndex = 0;
	INT32 lCount = Count;
	while ((0 < lCount) && (pagpdGridItem = m_pagpmGrid->GetItemByTemplate(lIndex, pagpdGrid, AGPDGRID_ITEM_TYPE_ITEM, ItemTID)))
	{
		pAgpdItem = m_pagpmItem->GetItem(pagpdGridItem->m_lItemID);
		if (!pAgpdItem) continue;

		if (((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_bStackable)
		{
			if (pAgpdItem->m_nCount < lCount)
			{
				// pAgpdItem이 목표수량보다 적다면 수량만큼 없앤다.
				lCount -= pAgpdItem->m_nCount;
				
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
											pAgpdItem,
											pAgpdItem->m_nCount
											);					
				
				m_pagpmItem->SubItemStackCount(pAgpdItem, pAgpdItem->m_nCount);
			}
			else
			{
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
											pAgpdItem,
											lCount
											);
			
				m_pagpmItem->SubItemStackCount(pAgpdItem, lCount);
				lCount = 0;
			}
		}
		else
		{
			// log. 아이템이 사라질 수 있어 미리 남긴다.
			m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
										pAgpdItem,
										pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
										);
		
			--lCount;
			m_pagpmItem->RemoveItem(pAgpdItem, TRUE);
		}
		lIndex = 0;//JK_두개삭제시 밀려서 하나만 삭제 되는 현상수정
	}

	return lCount;
}

int AgsmNpcManager::RobItem(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -3);

	INT32 ItemTID = (INT32)lua_tonumber(L, -2);
	INT32 Count = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 3);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 lCount1 = Count;
	if(lCount1 > 0)
	{
		AgpdGrid* pagpdGrid1 = pThis->m_pagpmItem->GetInventory(pcsCharacter);
		if (pagpdGrid1)
		{
			lCount1 = pThis->RobItem(pcsCharacter, pagpdGrid1, ItemTID, Count);
		}
	}

	INT32 lCount2 = lCount1;
	if(lCount2 > 0)
	{
		AgpdGrid* pagpdGrid2 = pThis->m_pagpmItem->GetCashInventoryGrid(pcsCharacter);
		if (pagpdGrid2) 
		{
			lCount2 = pThis->RobItem(pcsCharacter, pagpdGrid2, ItemTID, lCount1);
		}
	}

	lua_pushinteger(L, lCount2);

	return 1;
}

int AgsmNpcManager::GiveItem(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -3);

	INT32 ItemTID = (INT32)lua_tonumber(L, -2);
	INT32 Count = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 3);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	AgpdItemTemplate* pItemTemplate = pThis->m_pagpmItem->GetItemTemplate(ItemTID);
	if (!pItemTemplate)
		return 1;

	AgpdItem* pcsItem = NULL;
	if (pItemTemplate->m_bStackable)
		pcsItem = pThis->m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsCharacter, Count);
	else
		pcsItem = pThis->m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsCharacter);

	if(!pcsItem)
		return 1;

	// 귀속아이템이라면 귀속과 주인을 세팅해준다. 2005.11.02. steeple
	if(pThis->m_pagpmItem->GetBoundType(pcsItem->m_pcsItemTemplate) == E_AGPMITEM_BIND_ON_ACQUIRE)
	{
		pThis->m_pagpmItem->SetBoundType(pcsItem, E_AGPMITEM_BIND_ON_ACQUIRE);
		pThis->m_pagpmItem->SetBoundOnOwner(pcsItem, pcsCharacter);
	}

	// Skill Plus 해준다. 2007.02.05. steeple
	pThis->m_pagsmItem->ProcessItemSkillPlus(pcsItem, pcsCharacter);

	pThis->m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_GIVE, pcsCharacter->m_lID,
										pcsItem,
										pcsItem->m_nCount ? pcsItem->m_nCount : 1
										);

	BOOL bSuccess = pThis->m_pagpmItem->AddItemToInventory(pcsCharacter, pcsItem);

	pThis->m_pagsmItem->SendPacketPickupItemResult(
		AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS, 
		(pcsItem) ? pcsItem->m_lID : AP_INVALID_IID, 
		pItemTemplate->m_lID, 
		(pcsItem->m_nCount) ? pcsItem->m_nCount : 1, 
		pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	lua_pushinteger(L, bSuccess);

	return 1;
}

int AgsmNpcManager::CastSkill( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);

	INT32 SkillTID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pagpmSkill->GetSkillTemplate(SkillTID);
	if(!pcsSkillTemplate)
		return 1;

	INT32 lSkillLevel = 1;

	INT32 lAdditionalSkillTID = (INT32)pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel, pThis->m_csRandom.randInt(100));
	if(lAdditionalSkillTID)
	{
		AgpdSkillTemplate* pcsSkillTemplate2 = pThis->m_pagpmSkill->GetSkillTemplate(lAdditionalSkillTID);

		INT32 SkillTID2 = (INT32)pcsSkillTemplate2->GetAdditionalSkillTID(lSkillLevel, pThis->m_csRandom.randInt(100));
		if(SkillTID2)
		{
			pThis->m_pagsmSkill->CastSkill(pcsCharacter, SkillTID2, lSkillLevel, NULL, FALSE, FALSE);
		}
		else
		{
			pThis->m_pagsmSkill->CastSkill(pcsCharacter, lAdditionalSkillTID, lSkillLevel, NULL, FALSE, FALSE);
		}
	}
	else
	{
		pThis->m_pagsmSkill->CastSkill(pcsCharacter, SkillTID, lSkillLevel, NULL, FALSE, FALSE);
	}

	return 1;
}

int AgsmNpcManager::Move( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -3);

	FLOAT PosX = (FLOAT)lua_tonumber(L, -2);
	FLOAT PosZ = (FLOAT)lua_tonumber(L, -1);

	lua_pop(L, 3);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmCharacter->MoveCharacterPosition(pcsCharacter, PosX, PosZ);

	pThis->WriteMoveLog(pcsCharacter);

	return 1;
}

int AgsmNpcManager::ReturnTown( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *pagsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pagsdCharacter)
		return FALSE;

	// 정상이 아닐 땐 사용불가.
	if(!pcsCharacter->m_bIsAddMap)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if (pThis->m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	if( pThis->m_pagpmCharacter->IsCombatMode(pcsCharacter) 
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		|| (pThis->m_pagpmCharacter->IsActionBlockCondition(pcsCharacter)))
	{
		pThis->m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
		return FALSE;
	}

	AuPOS stPos;
	if (!pThis->m_pagpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stPos))
		return FALSE;

	////////////////////////////////////////////////////////
	/// 여기서부터 케릭터 이동 처리.
	////////////////////////////////////////////////////////
	AuAutoLock pLock(pcsCharacter->m_Mutex);
	if(!pLock.Result())
		return FALSE;

	pThis->m_pagpmCharacter->SetActionBlockTime(pcsCharacter, 3000);
	pagsdCharacter->m_bIsTeleportBlock = TRUE;

	pThis->m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);
	pThis->m_pagpmCharacter->UpdatePosition(pcsCharacter, &stPos, FALSE, TRUE);

	pagsdCharacter->m_bIsTeleportBlock = FALSE;

	pThis->m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsCharacter);

	return 1;
}

int AgsmNpcManager::SetDefaultInvincible( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmCharacter->SetDefaultInvincible(pcsCharacter);

	return 1;
}


int AgsmNpcManager::IsArchlord( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 bIsArchlord = (INT32)pThis->m_pagpmCharacter->IsArchlord(pcsCharacter);

	lua_pushinteger(L, bIsArchlord);

	return 1;
}

int AgsmNpcManager::IsTransform( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 bIsTransform = (INT32)pcsCharacter->m_bIsTrasform;

	lua_pushinteger(L, bIsTransform);	

	return 1;
}

int AgsmNpcManager::IsRide( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 bIsRide = (INT32)pcsCharacter->m_bRidable;

	lua_pushinteger(L, bIsRide);

	return 1;
}

int AgsmNpcManager::CheckAttendance( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -5);
	INT32 NID = (INT32)lua_tonumber(L, -4);
	INT32 EID = (INT32)lua_tonumber(L, -3);
	INT32 STEP = (INT32)lua_tonumber(L, -2);

	INT32 Type = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 5);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmEventSystem->ProcessAttendance(CID, NID, EID, STEP, Type);

	return 1;
}

BOOL AgsmNpcManager::OnCheckAttendance(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	CNpcExDataMap::iterator it = m_NpcExDataMap.find(NID);
	if(it == m_NpcExDataMap.end())
		return FALSE;

	CLuaStreamPack* pLuaPack = it->second->pLuaPack;

	//////////////////////////////////////////////////////////////////////////
	//
	lua_State* pLuaState = m_LuaList.GetHandle();
	if(!pLuaState)
		return 0;

	if(!pLuaPack->dobuffer(pLuaState))
		return 0;

	lua_getglobal(pLuaState, "OnCheckAttendance");
	lua_pushnumber(pLuaState, CID);
	lua_pushnumber(pLuaState, NID);
	lua_pushnumber(pLuaState, EID);
	lua_pushnumber(pLuaState, STEP);
	lua_pushnumber(pLuaState, Type);
	lua_pushnumber(pLuaState, nResult);
	
	int status = lua_pcall(pLuaState, 6, 1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		
		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return 0;
	}
	
	int ret = 0;
	while (lua_gettop(pLuaState))
	{
		switch (lua_type(pLuaState, lua_gettop(pLuaState)))
		{
		case LUA_TNUMBER: 
			ret = (int)lua_tonumber(pLuaState, lua_gettop(pLuaState));
			break;
		default: 
			ret = -1;
			break;
		}
		lua_pop(pLuaState, 1 );
	}
	
	m_LuaList.RestoreHandle(pLuaState);
	
	return TRUE;
}

int AgsmNpcManager::CheckEventUser(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -5);
	INT32 NID = (INT32)lua_tonumber(L, -4);
	INT32 EID = (INT32)lua_tonumber(L, -3);
	INT32 STEP = (INT32)lua_tonumber(L, -2);

	INT32 Type = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 5);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmEventSystem->ProcessEventUser(CID, NID, EID, STEP, Type);

	return 1;
}

BOOL AgsmNpcManager::OnCheckEventUser(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	CNpcExDataMap::iterator it = m_NpcExDataMap.find(NID);
	if(it == m_NpcExDataMap.end())
		return FALSE;

	CLuaStreamPack* pLuaPack = it->second->pLuaPack;

	//////////////////////////////////////////////////////////////////////////
	//
	lua_State* pLuaState = m_LuaList.GetHandle();
	if(!pLuaState)
		return 0;

	if(!pLuaPack->dobuffer(pLuaState))
		return 0;

	lua_getglobal(pLuaState, "OnCheckEventUser");
	lua_pushnumber(pLuaState, CID);
	lua_pushnumber(pLuaState, NID);
	lua_pushnumber(pLuaState, EID);
	lua_pushnumber(pLuaState, STEP);
	lua_pushnumber(pLuaState, Type);
	lua_pushnumber(pLuaState, nResult);

	int status = lua_pcall(pLuaState, 6, 1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);

		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return 0;
	}

	int ret = 0;
	while (lua_gettop(pLuaState))
	{
		switch (lua_type(pLuaState, lua_gettop(pLuaState)))
		{
		case LUA_TNUMBER: 
			ret = (int)lua_tonumber(pLuaState, lua_gettop(pLuaState));
			break;
		default: 
			ret = -1;
			break;
		}
		lua_pop(pLuaState, 1 );
	}

	m_LuaList.RestoreHandle(pLuaState);

	return TRUE;
}


int AgsmNpcManager::EventUserFlagUpdate( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -6);
	INT32 NID = (INT32)lua_tonumber(L, -5);
	INT32 EID = (INT32)lua_tonumber(L, -4);
	INT32 STEP = (INT32)lua_tonumber(L, -3);

	INT32 Type = (INT32)lua_tonumber(L, -2);
	INT32 Flag = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 6);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	pThis->m_pagsmEventSystem->ProcessEventUserFlagUpdate(CID, NID, EID, STEP, Type, Flag);

	return 1;
}

int AgsmNpcManager::GiveTimeLimitItem( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID		= (INT32)lua_tonumber(L, -4);

	INT32 ItemTID	= (INT32)lua_tonumber(L, -3);
	INT32 Count		= (INT32)lua_tonumber(L, -2);

	INT32 Hour		= (INT32)lua_tonumber(L, -1);

	lua_pop(L, 4);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	AgpdItemTemplate* pItemTemplate = pThis->m_pagpmItem->GetItemTemplate(ItemTID);
	if (!pItemTemplate)
		return 1;

	AgpdItem* pcsItem = NULL;
	if (pItemTemplate->m_bStackable)
		pcsItem = pThis->m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsCharacter, Count);
	else
		pcsItem = pThis->m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsCharacter);

	if(!pcsItem)
		return 1;

	// 귀속아이템이라면 귀속과 주인을 세팅해준다. 2005.11.02. steeple
	if(pThis->m_pagpmItem->GetBoundType(pcsItem->m_pcsItemTemplate) == E_AGPMITEM_BIND_ON_ACQUIRE)
	{
		pThis->m_pagpmItem->SetBoundType(pcsItem, E_AGPMITEM_BIND_ON_ACQUIRE);
		pThis->m_pagpmItem->SetBoundOnOwner(pcsItem, pcsCharacter);
	}

	// Skill Plus 해준다. 2007.02.05. steeple
	pThis->m_pagsmItem->ProcessItemSkillPlus(pcsItem, pcsCharacter);

	UINT32 lCurrentTimeStamp	= AuTimeStamp::GetCurrentTimeStamp();
	UINT32 lExpireTimeStamp	= AuTimeStamp::AddTime(lCurrentTimeStamp, 0, Hour, 0, 0);
	pcsItem->m_lExpireTime	= lExpireTimeStamp;

	AgsdItemADChar* pcsItemADChar = pThis->m_pagsmItem->GetADCharacter(pcsCharacter);
	if( pcsItemADChar )
		pcsItemADChar->m_bUseTimeLimitItem = TRUE;

	pThis->m_pagsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, pThis->GetClockCount());
	pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_200_MS);

	pThis->m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_GIVE, pcsCharacter->m_lID,
										pcsItem,
										pcsItem->m_nCount ? pcsItem->m_nCount : 1
										);

	BOOL bSuccess = pThis->m_pagpmItem->AddItemToInventory(pcsCharacter, pcsItem);

	pThis->m_pagsmItem->SendPacketPickupItemResult(
		AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS, 
		(pcsItem) ? pcsItem->m_lID : AP_INVALID_IID, 
		pItemTemplate->m_lID, 
		(pcsItem->m_nCount) ? pcsItem->m_nCount : 1, 
		pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	lua_pushinteger(L, bSuccess);

	return 1;
}

int AgsmNpcManager::CallSystemNPC( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -4);
	INT32 NID = (INT32)lua_tonumber(L, -3);
	INT32 EID = (INT32)lua_tonumber(L, -2);
	INT32 STEP = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 4);

	pThis->ExecScriptMain(CID, NID, EID, STEP);

	return 1;
}

int AgsmNpcManager::GetGuildMemberRank( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 lRank = 0;
	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->GetGuild(pcsCharacter);
	if(pcsGuild)
	{
		AgpdGuildMember* pcsGuildMember = pThis->m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
		if( pcsGuildMember )
			lRank = pcsGuildMember->m_lRank;
	}

	lua_pushinteger(L, lRank);

	return 1;
}

int AgsmNpcManager::GetAuctionCount( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 lAuctionCount = 0;
	AgpdAuctionCAD *pcsAuctionCAD = pThis->m_pagpmAuction->GetCAD(pcsCharacter);
	if (pcsAuctionCAD)
	{
		lAuctionCount = pcsAuctionCAD->GetCount();
	}

	lua_pushinteger(L, lAuctionCount);

	return 1;
}

int AgsmNpcManager::IsCriminal( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 bIsCriminal = 0;
	AgpdWantedCriminal* pWantedCriminal = pThis->m_pagpmWantedCriminal->GetWantedCriminal(pcsCharacter);
	if(pWantedCriminal)
	{
		bIsCriminal = 1;
	}

	lua_pushinteger(L, bIsCriminal);

	return 1;
}

int AgsmNpcManager::SetChangeName( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -4);
	INT32 NID = (INT32)lua_tonumber(L, -3);
	INT32 EID = (INT32)lua_tonumber(L, -2);
	INT32 STEP = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 4);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmEventSystem->ProcessChangeName(CID, NID, EID, STEP);

	return 1;
}

BOOL AgsmNpcManager::OnChangeNameResult( INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult )
{
	ExecScriptMain(CID, NID, EID, nResult);	//STEP 대신 nResult

	return TRUE;
}

int AgsmNpcManager::ReturnToLogin( lua_State* L )
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	//pThis->m_pagpmReturnToLogin->OnOperationRequest(CID);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacterLock(CID);
	if (!pcsCharacter)
		return FALSE;

	BOOL bResult = pThis->m_pagpmReturnToLogin->EnumCallback(AGPMRETURN_CB_REQUEST, pcsCharacter, NULL);

	pcsCharacter->m_Mutex.Release();

	return 1;
}

int AgsmNpcManager::CerariumOrb(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -4);
	INT32 NID = (INT32)lua_tonumber(L, -3);
	INT32 EID = (INT32)lua_tonumber(L, -2);
	INT32 STEP = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 4);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmEventSystem->ProcessCerariumOrb(CID, NID, EID, STEP);

	return 1;
}

BOOL AgsmNpcManager::CerariumOrbResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult)
{
	ExecScriptMain(CID, NID, EID, nResult);	//STEP 대신 nResult

	return TRUE;
}
//JK_특성화서버
int AgsmNpcManager::ServerMove(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -5);
	INT32 NID = (INT32)lua_tonumber(L, -4);
	INT32 EID = (INT32)lua_tonumber(L, -3);	
	INT32 STEP = (INT32)lua_tonumber(L, -2);
	INT32 ServerIndex = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 5);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	pThis->m_pagsmEventSystem->CharacterMoveServer(CID, NID, EID, STEP, ServerIndex);

	return 1;
}
//JK_특성화서버
BOOL AgsmNpcManager::ServerMoveResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult)
{
	ExecScriptMain(CID, NID, EID, nResult);	//STEP 대신 nResult

	return TRUE;
}

BOOL AgsmNpcManager::CharacterSealingResultList(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR** strCharName, INT32* lSlot)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	CHAR szID[AGSMACCOUNT_MAX_ACCOUNT_CHARACTER][AGPDCHARACTER_MAX_ID_LENGTH+1] = { 0, };
	memcpy(szID, strCharName, sizeof(szID));
	INT32 Slot[AGSMACCOUNT_MAX_ACCOUNT_CHARACTER] = { 0, };
	memcpy(Slot, lSlot, sizeof(Slot));

	CHAR strMenuID[64];
	memset(strMenuID, 0, sizeof(strMenuID));
	sprintf_s(strMenuID, sizeof(strMenuID), "Menu_CharacterList");
	AuXmlElement* pElem = m_pMenuNode->FirstChildElement(strMenuID);
	if(!pElem)
		return 0;

	PACKET_EVENTNPCDIALOG_GRANT_MENU pPacket(CID, NID);
	AuXmlElement* pElemType = pElem->FirstChildElement("UIType");
	if(pElemType)
	{
		CHAR* pText = (CHAR*)pElemType->GetText();
		if(pText)
			pPacket.lMenuType = (INT32)atoi(pText);
	}

	AuXmlElement* pElemTitle = pElem->FirstChildElement("TITLE");
	if(pElemTitle)
	{
		CHAR* pTitle = (CHAR*)pElemTitle->GetText();
		if(pTitle)
			strcpy_s(pPacket.strTitle, MAX_BUTTON_TEXT_SIZE, pTitle);
	}

	AuXmlElement* pElemDialog = pElem->FirstChildElement("Dialog");
	if(pElemDialog)
	{
		CHAR* pDialog = (CHAR*)pElemDialog->GetText();
		if(pDialog)
			strcpy_s(pPacket.strDialog, MESSAGE_MAX_SIZE, pDialog);
	}

	for(INT32 i = 0; i < AGSMACCOUNT_MAX_ACCOUNT_CHARACTER; ++i)
	{
		if(*szID[i] == '\0')
			break;

		strcpy_s(pPacket.ButtonArray[i].strText, MAX_BUTTON_TEXT_SIZE, szID[i]);
		pPacket.ButtonArray[i].EID = EID;
		pPacket.ButtonArray[i].STEP = Slot[i];
	}

	SendPacketUser(pPacket, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	return TRUE;
}

int AgsmNpcManager::IsEquipUnableItemInEpicZone(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	INT32 bEquipUnableItem = 0;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	bEquipUnableItem = (INT32)pThis->m_pagpmItem->IsEquipUnableItemInEpicZone(pcsCharacter);
	
	lua_pushinteger(L, bEquipUnableItem);

	return 1;
}

int AgsmNpcManager::IsEnableEntrance(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 lBossZoneID = (INT32)lua_tonumber(L, -1);
	
	INT32  bEntrance = 0;

	lua_pop(L, 1);

	if(pThis->m_pagsmEpicZone)
	{
		bEntrance = (INT32)pThis->m_pagsmEpicZone->IsEnableEntrance(lBossZoneID);
	}

	lua_pushinteger(L, bEntrance);

	return 1;
}

int AgsmNpcManager::SetEpicBossZoneStartTime(lua_State *L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 lBossZoneID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	if(pThis->m_pagsmEpicZone)
	{
		pThis->m_pagsmEpicZone->SetEpicBossZoneStartTime(lBossZoneID, pThis->GetClockCount());
	}

	return 1;
}

int AgsmNpcManager::MoveIncludeParty(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -3);

	FLOAT PosX = (FLOAT)lua_tonumber(L, -2);
	FLOAT PosZ = (FLOAT)lua_tonumber(L, -1);

	lua_pop(L, 3);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(NULL == pcsCharacter)
		return FALSE;

	pThis->m_pagsmCharacter->MoveCharacterIncludeParty(pcsCharacter, PosX, PosZ);

	return 1;
}

int AgsmNpcManager::IsPartyLeader(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	INT32 bLeader = 0;
	INT32 nLeaderCID = 0;

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(NULL == pcsCharacter)
		return FALSE;

	AgpdParty *pcsParty = pThis->m_pagpmParty->GetParty(pcsCharacter);
	if(pcsParty)
	{
		nLeaderCID = pThis->m_pagpmParty->GetLeaderCID(pcsCharacter);
		if(nLeaderCID == CID)
		{
			bLeader = 1;
		}
	}
	else
	{
		bLeader = 1;
	}

	lua_pushinteger(L, bLeader);

	return 1;
}

/*
int AgsmNpcManager::RegisterEntry(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 lBossZoneID = (INT32)lua_tonumber(L, -2);
	INT32 CID		  = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter *pcsPartyLeader = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(NULL == pcsPartyLeader)
		return FALSE;

	if(pThis->m_pagsmEpicZone)
	{
		pThis->m_pagsmEpicZone->RegisterEntry(lBossZoneID, pcsPartyLeader);
	}

	return 1;
}
*/

int AgsmNpcManager::GetBossZoneRemainTime(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 lBossZoneID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	INT32 lRemainTime = pThis->m_pagsmEpicZone->GetRemainTime(lBossZoneID);

	lua_pushinteger(L, lRemainTime);

	return 1;
}

int AgsmNpcManager::ShowOptionMessageBox(lua_State* L)
{
	AgsmNpcManager*	pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -8);
	INT32 NID = (INT32)lua_tonumber(L, -7);
	INT32 EID = (INT32)lua_tonumber(L, -6);
	INT32 STEP = (INT32)lua_tonumber(L, -5);

	INT32 MessageType = (INT32)lua_tonumber(L, -4);
	INT32 MessageGroup = (INT32)lua_tonumber(L, -3);
	INT32 MessageID = (INT32)lua_tonumber(L, -2);
	INT32 Option = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 8);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	CHAR strMessageGroup[64];
	memset(strMessageGroup, 0, sizeof(strMessageGroup));
	sprintf_s(strMessageGroup, sizeof(strMessageGroup), "Group_%d", MessageGroup);
	AuXmlElement* pElemGroup = pThis->m_pMessageBoxNode->FirstChildElement(strMessageGroup);
	if(!pElemGroup)
		return 0;

	CHAR strMessage[64];
	memset(strMessage, 0, sizeof(strMessage));
	sprintf_s(strMessage, sizeof(strMessage), "Message_%d", MessageID);
	AuXmlElement* pElem = pElemGroup->FirstChildElement(strMessage);
	if(!pElem)
		return 0;

	if(Option)
	{
		CHAR strMessage[64];
		memset(strMessage, 0, sizeof(strMessage));
		sprintf_s(strMessage, sizeof(strMessage), pElem->GetText(), Option);
	}

	PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX pPacket(CID, NID, EID, STEP);
	pPacket.lMessageBoxType = MessageType;
	if(Option)
	{
		CHAR strMessage[64];
		memset(strMessage, 0, sizeof(strMessage));
		sprintf_s(strMessage, sizeof(strMessage), pElem->GetText(), Option);
		strcpy(pPacket.strMessage, strMessage);
	}
	else
	{
		strcpy(pPacket.strMessage, pElem->GetText());
	}

	pThis->SendPacketUser(pPacket, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	return 1;
}

int AgsmNpcManager::ShowTitleMain(lua_State* L)
{
	AgsmNpcManager*	pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 NID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	PACKET_AGPPTITLE_UI_OPEN pPacket(pcsCharacter->m_szID);

	pThis->SendPacketUser(pPacket, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	return 1;
}

int AgsmNpcManager::ProcessArchlordCastleAttackGuild(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 NID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	if(pThis->m_pagpmArchlord->GetCurrentStep() != AGPMARCHLORD_STEP_DUNGEON)
		return 0;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	AgpdGuild *pcsGuild = pThis->m_pagpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return 0;

	if(!pThis->m_pagpmSiegeWar->IsThisRegionCastleOwnerGuildMember(pcsCharacter))
		return 0;

	BOOL bIsAlreadyApply = pThis->m_pagpmSiegeWar->IsAlreadyApplGuild(pcsGuild, TRUE);

	if(pThis->m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID) && !bIsAlreadyApply)
		bIsAlreadyApply =  pThis->m_pagpmSiegeWar->AddArchlordCastleAttackGuild(pcsGuild);

	lua_pushinteger(L, bIsAlreadyApply);

	return bIsAlreadyApply;
}

int AgsmNpcManager::GetHaveTitleNumber(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	INT32 nHaveTitleNumber = pThis->m_pagpmTitle->GetHaveTitleNumber(pcsCharacter);

	lua_pushinteger(L, nHaveTitleNumber);

	return 1;
}

int AgsmNpcManager::RequestJoinWorldChampionShip(lua_State* L)
{
	AgsmNpcManager *pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	//////////////////////////////////////////////////////////////////////

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST stRequest(CID);

	pThis->m_pagpmGuild->EnumCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_REQUEST, &stRequest, NULL);

	return 1;
}

int AgsmNpcManager::EnterWorldChampionShip(lua_State* L)
{
	AgsmNpcManager *pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	///////////////////////////////////////////////////////////////////////

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER stEnter(CID);

	pThis->m_pagpmGuild->EnumCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_ENTER, &stEnter, NULL);

	return 1;
}


int AgsmNpcManager::HasTitle(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 TITLETID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	AgpdTitleTemplate* pcsTitle = pThis->m_pagpmTitle->GetTitleTemplate(TITLETID);
	if(!pcsTitle)
		return 0;

	if(!pcsCharacter->m_csTitle)
		return 0;

	BOOL bHasTitle = pcsCharacter->m_csTitle->IsHaveTitle(TITLETID);

	lua_pushinteger(L, bHasTitle);

	return bHasTitle;
}

BOOL AgsmNpcManager::WriteMoveLog(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	ApmMap::RegionTemplate*	pCurrentTemplate = m_papmMap->GetTemplate( pcsCharacter->m_nBindingRegionIndex );
	ApmMap::RegionTemplate*	pBeforeTemplate = m_papmMap->GetTemplate( pcsCharacter->m_nBeforeRegionIndex );

	CHAR *pszSrc	= NULL;
	CHAR *pszDest	= NULL;

	if(pCurrentTemplate && pBeforeTemplate)
	{
		pszSrc	= pBeforeTemplate->pStrName;
		pszDest = pCurrentTemplate->pStrName;
	}

	INT32 lCharTID	= ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);
	INT64 llExp		= m_pagpmCharacter->GetExp(pcsCharacter);

	return m_pagpmLog->WriteLog_Teleport(0,
		&pAgsdCharacter->m_strIPAddress[0],
		pAgsdCharacter->m_szAccountID,
		pAgsdCharacter->m_szServerName,
		pcsCharacter->m_szID,
		lCharTID,
		lLevel,
		llExp,
		pcsCharacter->m_llMoney,
		pcsCharacter->m_llBankMoney,
		pszSrc ? pszSrc : "",
		pszDest ? pszDest : "",
		0
		);
}

int AgsmNpcManager::UserCreationDateCheck(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -6);
	INT32 NID = (INT32)lua_tonumber(L, -5);
	INT32 EID = (INT32)lua_tonumber(L, -4);
	INT32 STEP = (INT32)lua_tonumber(L, -3);

	char *startDate = NULL;
	startDate = new char[32];
	memset(startDate, 0, sizeof(startDate));

	char *endDate = NULL;
	endDate = new char[32];
	memset(endDate, 0, sizeof(endDate));

	startDate = (char *)lua_tostring(L, -2);
	endDate = (char *)lua_tostring(L, -1);

	lua_pop(L, 6);

	pThis->m_pagsmEventSystem->ProcessCharacterCreationDateCheck(CID, NID, EID, STEP, startDate, endDate);

	delete [] startDate;
	delete [] endDate;

	return 1;
}

BOOL AgsmNpcManager::OnUserCreationDateCheck(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	CNpcExDataMap::iterator it = m_NpcExDataMap.find(NID);
	if(it == m_NpcExDataMap.end())
		return FALSE;

	CLuaStreamPack* pLuaPack = it->second->pLuaPack;

	//////////////////////////////////////////////////////////////////////////
	//
	lua_State* pLuaState = m_LuaList.GetHandle();
	if(!pLuaState)
		return 0;

	if(!pLuaPack->dobuffer(pLuaState))
		return 0;

	lua_getglobal(pLuaState, "OnUserCreationDateCheck");
	lua_pushnumber(pLuaState, CID);
	lua_pushnumber(pLuaState, NID);
	lua_pushnumber(pLuaState, EID);
	lua_pushnumber(pLuaState, STEP);
	lua_pushnumber(pLuaState, nResult);

	int status = lua_pcall(pLuaState, 5, 1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);

		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return 0;
	}

	int ret = 0;
	while (lua_gettop(pLuaState))
	{
		switch (lua_type(pLuaState, lua_gettop(pLuaState)))
		{
		case LUA_TNUMBER: 
			ret = (int)lua_tonumber(pLuaState, lua_gettop(pLuaState));
			break;
		default: 
			ret = -1;
			break;
		}
		lua_pop(pLuaState, 1 );
	}

	m_LuaList.RestoreHandle(pLuaState);

	return TRUE;
}

int AgsmNpcManager::CharismaPointAdd(lua_State* L)
{
	AgsmNpcManager *pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 CharismaPoint = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	///////////////////////////////////////////////////////////////////////

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	pThis->m_pagpmCharacter->AddCharismaPoint(pcsCharacter, CharismaPoint);

	if(pThis->m_pagpmLog)
	{
		AgsdCharacter* pAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pAgsdCharacter)
			return FALSE;

		INT32 lLevel	= pThis->m_pagpmCharacter->GetLevel(pcsCharacter);
		INT64 llExp		= pThis->m_pagpmCharacter->GetExp(pcsCharacter);	

		pThis->m_pagpmLog->WriteLog_CharismaUp(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID,
			pAgsdCharacter->m_szServerName, pcsCharacter->m_szID,
			((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID,
			lLevel, llExp, pcsCharacter->m_llMoney, pcsCharacter->m_llBankMoney,
			CharismaPoint, pThis->m_pagpmCharacter->GetCharismaPoint(pcsCharacter), pcsCharacter->m_szID
			);
	}

	return 1;
}

int AgsmNpcManager::CharismaPointSub(lua_State* L)
{
	AgsmNpcManager *pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 CharismaPoint = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	///////////////////////////////////////////////////////////////////////

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	pThis->m_pagpmCharacter->SubCharismaPoint(pcsCharacter, CharismaPoint);

	if(pThis->m_pagpmLog)
	{
		AgsdCharacter* pAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pAgsdCharacter)
			return FALSE;

		INT32 lLevel	= pThis->m_pagpmCharacter->GetLevel(pcsCharacter);
		INT64 llExp		= pThis->m_pagpmCharacter->GetExp(pcsCharacter);	

		pThis->m_pagpmLog->WriteLog_CharismaUp(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID,
			pAgsdCharacter->m_szServerName, pcsCharacter->m_szID,
			((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID,
			lLevel, llExp, pcsCharacter->m_llMoney, pcsCharacter->m_llBankMoney,
			(-1)*CharismaPoint, pThis->m_pagpmCharacter->GetCharismaPoint(pcsCharacter), pcsCharacter->m_szID
			);
	}
	return 1;
}

int AgsmNpcManager::AddMoney(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 MONEY = (INT64)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return 0;

	BOOL bResult = pThis->m_pagpmCharacter->AddMoney(pcsCharacter, MONEY);

	if(bResult)
	{
		pThis->m_pagpmLog->WriteLog_Gheld(	AGPDLOGTYPE_GHELD_EVENT_GIVE, 0, 
			&pcsAgsdCharacter->m_strIPAddress[0],
			pcsAgsdCharacter->m_szAccountID,
			pcsAgsdCharacter->m_szServerName,
			pcsCharacter->m_szID,
			pcsCharacter->m_pcsCharacterTemplate ? pcsCharacter->m_pcsCharacterTemplate->m_lID : 0,
			pThis->m_pagpmCharacter->GetLevel(pcsCharacter),
			pThis->m_pagpmCharacter->GetExp(pcsCharacter),
			pcsCharacter->m_llMoney,
			pcsCharacter->m_llBankMoney,
			MONEY,
			pcsCharacter->m_szID);
	}

	lua_pushnumber(L, bResult);

	return 1;
}

int AgsmNpcManager::AddExpByPercent(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 PERCENT = (INT64)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 0;

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return 0;

	INT64 lAddExp = (INT64) ( (double)PERCENT * (double)pThis->m_pagpmCharacter->GetLevelUpExp(pcsCharacter) / 100.0 );

	BOOL bResult = pThis->m_pagsmDeath->AddBonusExpToChar(pcsCharacter, NULL, lAddExp, FALSE, FALSE, TRUE);

	lua_pushnumber(L, bResult);

	return 1;
}

int AgsmNpcManager::GetMurderPoint(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	INT32 m_nMurderPoint = pThis->m_pagpmCharacter->GetMurdererPoint(pcsCharacter);

	lua_pushinteger(L, m_nMurderPoint);

	return 1;
}

int AgsmNpcManager::IsInGuild(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	BOOL m_bIsInGuild = FALSE;
	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->GetGuild(pcsCharacter);
	if(pcsGuild)
		m_bIsInGuild = TRUE;

	lua_pushinteger(L, m_bIsInGuild);

	return 1;
}

int AgsmNpcManager::ShowBuddyMain(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	pThis->m_pagsmBuddy->SendMentorUIOpen(pcsCharacter);

	return 1;
}

int AgsmNpcManager::GetMentorCID(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 MentorCID = pThis->m_pagpmBuddy->GetMentorCID(pcsCharacter);

	lua_pushinteger(L, MentorCID);

	return 1;
}

int AgsmNpcManager::GetSkillLevel(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -2);
	INT32 SKILLTID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 2);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	AgpdSkill* pcsSkill = pThis->m_pagpmSkill->GetSkillByTID(pcsCharacter, SKILLTID);
	if(!pcsSkill)
	{
		lua_pushinteger(L, 0);
		return 1;
	}

	INT32 SkillLevel = pThis->m_pagpmSkill->GetSkillLevel(pcsSkill);

	lua_pushinteger(L, SkillLevel);

	return 1;
}

int AgsmNpcManager::GetHaveCharismaPoint(lua_State* L)
{
	AgsmNpcManager* pThis = AgsmNpcManager::GetInstance();

	INT32 CID = (INT32)lua_tonumber(L, -1);

	lua_pop(L, 1);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return 1;

	INT32 nCharismaPoint = pThis->m_pagpmCharacter->GetCharismaPoint(pcsCharacter);

	lua_pushinteger(L, nCharismaPoint);

	return 1;
}