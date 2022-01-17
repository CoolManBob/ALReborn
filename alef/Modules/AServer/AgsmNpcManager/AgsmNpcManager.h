#ifndef _AGSM_NPCMANAGER_H
#define _AGSM_NPCMANAGER_H

#include <AgsEngine.h>
#include <AuLua.h>
#include <AuXmlParser.h>

#include <AgsdNpcManager.h>
#include <AgpdCharacter.h>
#include <AgpdGrid.h>

#include <AuRandomNumber.h>

#include <list>
#include <map>

typedef map<INT32, CNpcExData*> CNpcExDataMap;

class AgpmEventNPCDialog;
class AgpmCharacter;
class AgsmCharacter;
class AgpmItem;
class AgsmItem;
class AgpmGrid;
class AgsmItemManager;
class AgpmSkill;
class AgsmSkill;
class AgsmSystemMessage;
class AgpmSummons;
class AgpmEventBinding;
class AgsmEventSystem;
class AgsmBattleGround;
class AgpmGuild;
class AgpmAuction;
class AgpmReturnToLogin;
class AgpmWantedCriminal;
class AgpmBuddy;
class AgsmBuddy;
class AgsmEpicZone;
class AgpmParty;
class AgpmEpicZone;
class ApmMap;
class AgpmArchlord;
class AgpmSiegeWar;
class AgpmTitle;
class AgpmLog;
class AgsmDeath;

class AgsmNpcManager : public AgsModule
{
private:
	static AgsmNpcManager*	m_pInstance;
	
	AgpmEventNPCDialog* m_pagpmEventNPCDialog;
	AgpmCharacter*		m_pagpmCharacter;
	AgsmCharacter*		m_pagsmCharacter;
	AgpmItem*			m_pagpmItem;
	AgsmItem*			m_pagsmItem;
	AgpmGrid*			m_pagpmGrid;
	AgsmItemManager*	m_pagsmItemManager;
	AgpmSkill*			m_pagpmSkill;
	AgsmSkill*			m_pagsmSkill;
	AgsmSystemMessage*	m_pagsmSystemMessage;
	AgpmSummons*		m_pagpmSummons;
	AgpmEventBinding*	m_pagpmEventBinding;
	AgsmEventSystem*	m_pagsmEventSystem;
	AgsmBattleGround*	m_pagsmBattleGround;
	AgpmGuild*			m_pagpmGuild;
	AgpmAuction*		m_pagpmAuction;
	AgpmReturnToLogin*	m_pagpmReturnToLogin;
	AgpmWantedCriminal*	m_pagpmWantedCriminal;
	AgsmEpicZone*		m_pagsmEpicZone;
	AgpmParty*			m_pagpmParty;
	AgpmEpicZone*		m_pagpmEpicZone;
	ApmMap*				m_papmMap;
	AgpmArchlord*		m_pagpmArchlord;
	AgpmSiegeWar*		m_pagpmSiegeWar;
	AgpmBuddy*			m_pagpmBuddy;
	AgsmBuddy*			m_pagsmBuddy;

	AgpmTitle*			m_pagpmTitle;
	AgpmLog*			m_pagpmLog;
	AgsmDeath*			m_pagsmDeath;

	MTRand				m_csRandom;

	CNpcExDataMap		m_NpcExDataMap;

	AuLuaArray			m_LuaList;
	AuXmlDocument		m_XmlData;
	AuXmlDocument		m_MessageXml;

	AuXmlNode* m_pMessageBoxNode;
	AuXmlNode* m_pMenuNode;

public:
	AgsmNpcManager();
	virtual ~AgsmNpcManager();

	static AgsmNpcManager* GetInstance()
	{
		return AgsmNpcManager::m_pInstance;
	}

	BOOL OnInit();
	BOOL OnDestroy();

	BOOL LoadXmlFile();
	BOOL LoadNpcManagerFile(char* fileName);

	static BOOL Decrypt( char* buffer, unsigned bufferSize );
	static int LuaRegister(lua_State* pLuaState);

	BOOL ExecScriptMain(INT32 CID, INT32 NID, INT32 EID, INT32 STEP);
	INT32 ExecScriptMain(INT32 CID, INT32 NID, INT32 lEvnetIndex, INT32 lStep, CLuaStreamPack* pLuaPack);
	INT32 ExecScriptMessageBox(INT32 CID, INT32 NID, INT32 lEvnetIndex, INT32 lStep, CLuaStreamPack* pLuaPack, BOOL bBtnIndex, CHAR* strString, INT32 ItemID);
	INT32 ExecScriptMenu(INT32 CID, INT32 NID, INT32 lEvnetIndex, INT32 lStep, CLuaStreamPack* pLuaPack);
	
	CHAR* GetFileName(INT32 nNPCID);
	BOOL OnEventRequest(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsNpc, INT32 lEvnetIndex = 0, INT32 lStep = 0);

	BOOL WriteMoveLog(AgpdCharacter *pcsCharacter);
	
	static int OnShowMessageBox(lua_State* L);
	static int OnShowMenu(lua_State* L);
	
	static BOOL OnGrantMessageBox(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL OnGrantMenu(PVOID pData, PVOID pClass, PVOID pCustData);

	//////////////////////////////////////////////////////////////////////////
	//
	static int GetCharacterTID(lua_State* L);
	static int GetCharacterRace(lua_State* L);
	static int GetCharacterLevel(lua_State* L);
	static int GetCharacterExp(lua_State* L);

	static int GetInventoryEmptySlotCount(lua_State* L);
	static int GetCashInventoryEmptySlotCount(lua_State* L);
	static int CheckItemCount(lua_State* L);
	static int RobItem(lua_State* L);
	static int GiveItem(lua_State* L);
	INT32 RobItem(AgpdCharacter* pcsCharacter, AgpdGrid* pagpdGrid, INT32 ItemTID, INT32 Count);

	static int CastSkill(lua_State* L);
	static int Move(lua_State* L);
	static int ReturnTown(lua_State* L);
	static int SetDefaultInvincible(lua_State* L);

	static int IsArchlord(lua_State* L);
	static int IsTransform(lua_State* L);
	static int IsRide(lua_State* L);
	static int IsCriminal(lua_State* L);
	
	static int GetGuildMemberRank(lua_State* L);
	static int GetAuctionCount(lua_State* L);

	static int CheckAttendance(lua_State* L);
	BOOL OnCheckAttendance(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult);

	static int CheckEventUser(lua_State* L);
	BOOL OnCheckEventUser(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult);

	static int CheckEventBattleGround(lua_State* L);
	static int EventUserFlagUpdate(lua_State* L);

	static int GiveTimeLimitItem(lua_State* L);

	static int CallSystemNPC(lua_State* L);
	
	static int SetChangeName(lua_State* L);
	BOOL OnChangeNameResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult);
	
	static int ReturnToLogin(lua_State* L);

	static int CerariumOrb(lua_State* L);
	BOOL CerariumOrbResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult);
	BOOL CharacterSealingResultList(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR** strCharName, INT32* lSlot);
	//JK_특성화서버
	static int ServerMove(lua_State* L);
	BOOL ServerMoveResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult);


	static int IsEquipUnableItemInEpicZone(lua_State* L);
	static int IsEnableEntrance(lua_State* L);
	static int SetEpicBossZoneStartTime(lua_State* L);
	static int MoveIncludeParty(lua_State* L);
	static int IsPartyLeader(lua_State* L);
	static int GetBossZoneRemainTime(lua_State* L);
	static int ShowOptionMessageBox(lua_State* L);
	static int ShowTitleMain(lua_State* L);
	static int ProcessArchlordCastleAttackGuild(lua_State* L);
	static int HasTitle(lua_State* L);
	static int GetHaveTitleNumber(lua_State* L);

	static int RequestJoinWorldChampionShip(lua_State* L);
	static int EnterWorldChampionShip(lua_State* L);

	static int UserCreationDateCheck(lua_State* L);
	BOOL OnUserCreationDateCheck(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult);

	static int CharismaPointAdd(lua_State* L);
	static int CharismaPointSub(lua_State* L);

	static int AddMoney(lua_State* L);
	static int AddExpByPercent(lua_State* L);
	

	static int GetMurderPoint(lua_State* L);
	static int IsInGuild(lua_State* L);

	static int ShowBuddyMain(lua_State* L);

	static int GetMentorCID(lua_State* L);

	static int GetSkillLevel(lua_State* L);

	static int GetHaveCharismaPoint(lua_State* L);
};

#endif // _AGSM_NPCMANAGER_H