#include "AgsmBattleGround.h"
#include "AgppBattleGround.h"
#include "AgpmEventSpawn.h"
#include "AgsmEventSpawn.h"
#include "AgsmPvP.h"
#include "StrUtil.h"
#include "AuStrTable.h"
#include "AgsmSystemMessage.h"
#include "AgpmSkill.h"
#include "AgsmSkill.h"
#include "AgsmDeath.h"
#include "AgsmServerStatus.h"
#include "AgpmEventTeleport.h"

AgsmBattleGround* AgsmBattleGround::m_pInstance = NULL;

AgsmBattleGround::AgsmBattleGround()
	: m_ulClockCount(0), pLuaState(0)
{
	m_pInstance = this;
	m_Mutex.Init();
	
	SetModuleName("AgsmBattleGround");
	SetPacketType(AGPM_BATTLEGROUND_PACKET_TYPE);
	
	m_pagpmBattleGround	= NULL;
	m_pagpmCharacter	= NULL;
	m_pagsmCharacter	= NULL;
	m_pagsmPvP			= NULL;
	m_pagpmLog			= NULL;
	m_pagpmEventBinding	= NULL;
	m_pagsmSystemMessage	= NULL;
	m_pagpmSkill		= NULL;
	m_pagsmSkill		= NULL;
	m_pagsmDeath		= NULL;
	m_pagsmServerStatus = NULL;
	
	m_pBattleGroundState	= BATTLEGROUND_STATE_NONE;
	m_bIsEventBattleGround	= FALSE;

	m_pAbilityNode		= NULL;
	m_pSpawnListNode	= NULL;
	m_papmMap			= NULL;
	m_pcsAgpmConfig		= NULL;

	pLuaState = lua_open();
	luaL_openlibs(pLuaState);
}

AgsmBattleGround::~AgsmBattleGround()
{
	if(pLuaState)
		lua_close(pLuaState);
}

BOOL AgsmBattleGround::OnInit()
{
	//EnableIdle(TRUE);

	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagsmCharacter		= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgpmEventSpawn		= (AgpmEventSpawn*)GetModule("AgpmEventSpawn");
	m_pcsAgsmEventSpawn		= (AgsmEventSpawn*)GetModule("AgsmEventSpawn");
	m_pcsApmEventManager	= (ApmEventManager*)GetModule("ApmEventManager");
	m_pagsmPvP				= (AgsmPvP*)GetModule("AgsmPvP");
	m_pagpmLog				= (AgpmLog*)GetModule("AgpmLog");
	m_pagpmEventBinding		= (AgpmEventBinding*)GetModule("AgpmEventBinding");
	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	m_pagpmSkill			= (AgpmSkill*)GetModule("AgpmSkill");
	m_pagsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pagsmDeath			= (AgsmDeath*)GetModule("AgsmDeath");
	m_pagsmServerStatus		= (AgsmServerStatus*)GetModule("AgsmServerStatus");
	m_papmMap				= (ApmMap*)GetModule("ApmMap");
	m_pcsAgpmConfig			= (AgpmConfig*)GetModule("AgpmConfig");
	m_pagpmEventTeleport	= (AgpmEventTeleport*) GetModule("AgpmEventTeleport");
	
	if(!m_pagpmBattleGround || !m_pagpmCharacter || !m_pagsmPvP || !m_pagpmEventBinding
		|| !m_pagsmSystemMessage || !m_pagpmSkill || !m_pagsmSkill || !m_pagsmDeath || !m_pagsmServerStatus 
		|| !m_papmMap || !m_pagpmEventTeleport
		)
		return FALSE;

	if(!m_pagpmEventTeleport->SetCallbackGetBattleGroundState(CBGetBattleGroundState, this))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// xml
	if(!m_XmlData.LoadFile("ini\\SpawnDataBattleGround.xml"))
		return FALSE;

	//m_XmlData.Print();

	AuXmlNode* pSpawnDataNode = m_XmlData.FirstChild("BattleGroundSpawnData");
	if(!pSpawnDataNode)
		return FALSE;

	m_pAbilityNode   = pSpawnDataNode->FirstChild( "AbilityControl" );
	m_pSpawnListNode = pSpawnDataNode->FirstChild( "SpawnList" );

	AuXmlNode* pAddSkillNode = pSpawnDataNode->FirstChild( "AddSkill" );
	if(pAddSkillNode)
	{
		AuXmlElement* pLooserSkill = pAddSkillNode->FirstChildElement("Looser");
		if(pLooserSkill)
		{
			pLooserSkill->Attribute("CastHalt", &m_BattleGroundResult.Looser.SkillTID);
			pLooserSkill->Attribute("RaceHalt", &m_BattleGroundResult.Looser.SkillTID_RaceCast);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// lua
	if(pLuaState)
	{
		lua_register(pLuaState, "GetBattleGroundState",		AgsmBattleGround::GetBattleGroundState);
		lua_register(pLuaState, "SetBattleGroundState",		AgsmBattleGround::SetBattleGroundState);
		lua_register(pLuaState, "BattleGroundNotice",		AgsmBattleGround::BattleGroundNotice);
		lua_register(pLuaState, "RemoveMonsters",			AgsmBattleGround::RemoveMonsters);
		lua_register(pLuaState, "SpawnAbilityMonster",		AgsmBattleGround::SpawnAbilityMonster);
		lua_register(pLuaState, "SpawnCharacters",			AgsmBattleGround::SpawnCharacters);
		lua_register(pLuaState, "BattleGroundStatistic",	AgsmBattleGround::BattleGroundStatistic);

		pluaPack.Open("ini\\Lua\\BattleGround.lua", (Decrypt_CFunction)StrUtil::Decrypt);

 		if(!pluaPack.dobuffer(pLuaState))
 			return FALSE;
	}

	return TRUE;
}

BOOL AgsmBattleGround::OnTimer(UINT32 ulClockCount)
{
	INT_PTR GetOwningThreadID = m_Mutex.GetOwningThreadID();
	if ( GetOwningThreadID != 0 && GetOwningThreadID != ::GetCurrentThreadId())
		return FALSE;

	AuAutoLock pLock(m_Mutex);
	if(!pLock.Result())
		return FALSE;

	//////////////////////////////////////////////////////////////////////////

	if(ulClockCount < m_ulClockCount + 1000*60) return FALSE;
	
	m_ulClockCount = ulClockCount;
	
	lua_getglobal(pLuaState, "OnTimer");
	int status = lua_pcall(pLuaState, 0, -1, 0);

	if (status != 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);

		return FALSE;
	}
	
	return TRUE;
}

BOOL AgsmBattleGround::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize < 1)
		return FALSE;
		
	PACKET_BATTLEGROUND* pPacket = (PACKET_BATTLEGROUND*)pvPacket;
	
	switch(pPacket->pcOperation)
	{
		case AGPM_BATTLEGROUND_PACKET_NOTICE:
			{
			
			} break;
	}
		
	return TRUE;
}

BOOL AgsmBattleGround::OnDead( AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget )
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;
	
	ProcessCharismaPoint(pcsCharacter, pcsTarget);
		
	if( !m_pagpmBattleGround->IsInBattleGround(pcsCharacter) || !m_pagpmBattleGround->IsInBattleGround(pcsTarget)
		|| m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget))
		return FALSE;

	if(m_pagpmCharacter->IsMonster(pcsTarget))
	{
		AbilityMonsterRemove(pcsCharacter, pcsTarget);
	}

	return TRUE;
}

eBattleGroundState AgsmBattleGround::GetBattleGroundState()
{
	return m_pBattleGroundState;
}

int AgsmBattleGround::GetBattleGroundState(lua_State* L)
{
	AgsmBattleGround* pThis = AgsmBattleGround::GetInstance();
	
	lua_pushinteger(L, pThis->m_pBattleGroundState);

	return 1;
}

int AgsmBattleGround::SetBattleGroundState(lua_State* L)
{
	AgsmBattleGround* pThis = AgsmBattleGround::GetInstance();
	
	pThis->m_pBattleGroundState = (eBattleGroundState)lua_tointeger(L, -1);

	switch(pThis->m_pBattleGroundState)
	{
		case 3: // Start...
		{
			pThis->m_pSpawnVector.clear();
			pThis->m_pBossVector.clear();
			
			AuXmlNode* pNode = pThis->m_pSpawnListNode;
			for(AuXmlElement* pElem = pNode->FirstChildElement("Group"); pElem; pElem = pElem->NextSiblingElement())
			{
				_CURRENT_SPAWN pSpawn;
				pSpawn.strSpawnGroupName = std::string("temp");
				pSpawn.pElem = pElem->FirstChildElement("SpawnGroup");

				CHAR* pEvent = (CHAR*)pSpawn.pElem->Attribute("Event");
				if(pThis->m_bIsEventBattleGround && !pEvent)
					continue;

				if(!pThis->m_bIsEventBattleGround && pEvent)
					continue;

				pThis->m_pSpawnVector.push_back(pSpawn);

				AuXmlElement* pBossElem = (pSpawn.pElem) ? pSpawn.pElem->FirstChildElement("Boss") : NULL;
				if(pBossElem)
				{
					_CURRENT_SPAWN pBossSpawn;
					pBossSpawn.strSpawnGroupName = pSpawn.pElem->Attribute("Name"); // <SpawnGroup Name="">
					pBossSpawn.pElem = pSpawn.pElem;
					pThis->m_pBossVector.push_back(pBossSpawn);
				}
			}
		} break;
		case 4: // ending...
			{
				pThis->m_bIsEventBattleGround = FALSE;
			} break;
	}

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "SetBattleGroundState : %02d", pThis->m_pBattleGroundState);
	AuLogFile_s("LOG\\BattleGround.txt", strCharBuff);
	
	return 0;
}

AuXmlNode* AgsmBattleGround::GetNoticeNode()
{
	static AuXmlNode* m_pNoticeNode = NULL;
	
	if(IsEventBattleGroud())
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("Event");
	else
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("BattleGround");

	return m_pNoticeNode;
}

AuXmlNode* AgsmBattleGround::GetEpicNoticeNode()
{
	static AuXmlNode* m_pEpicNoticeNode = NULL;

	m_pEpicNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("Epic");

	return m_pEpicNoticeNode;
}

int AgsmBattleGround::BattleGroundNotice(lua_State* L)
{
	AgsmBattleGround* pThis = AgsmBattleGround::GetInstance();

	int type	= (int)lua_tointeger(L, -2);
	int option	= (int)lua_tointeger(L, -1);

	BOOL bEvent = pThis->IsEventBattleGroud() ? TRUE : FALSE;

	AuXmlNode* pNotice = pThis->GetNoticeNode();
	if(!pNotice) return 0;

	CHAR strNotice[MAX_NOTICE_LEN] = { 0, };
	DWORD dwColor = 0xFFFFFFFF;
	AuXmlElement* pElem = NULL;

	switch(type)
	{
		case 1:
		{
			pElem = pNotice->FirstChildElement("Notice_01");
			if(pElem)
			{
				CHAR* strFormat = (CHAR*)pElem->GetText();
				sprintf( strNotice, strFormat, option );
			}
		} break;
		case 2:
		{
			pElem = pNotice->FirstChildElement("Notice_02");
			if(pElem)
			{
				CHAR* strFormat = (CHAR*)pElem->GetText();
				sprintf( strNotice, strFormat);
			}
		} break;
		case 3:
		{
			pElem = pNotice->FirstChildElement("Notice_03");
			if(pElem)
			{
				CHAR* strFormat = (CHAR*)pElem->GetText();
				sprintf( strNotice, strFormat, option );
			}
		} break;
		case 4:
		{
			pElem = pNotice->FirstChildElement("Notice_04");
			if(pElem)
			{
				CHAR* strFormat = (CHAR*)pElem->GetText();
				sprintf( strNotice, strFormat);
			}
		} break;
		case 5:
			{
				pElem = pNotice->FirstChildElement("Notice_05");
				if(pElem)
				{
					CHAR* strFormat = (CHAR*)pElem->GetText();
					sprintf( strNotice, strFormat);
				}
			} break;
		default:
			return 0;
	}

	if(!pElem)
		return 0;
		
	CHAR* color = (CHAR*)pElem->Attribute("Color");
	if(color)
	{
		CHAR* ptr;
		dwColor = strtoul(color, &ptr, 16);
	}
	
	PACKET_BATTLEGROUND_NOTICE pPacketBattleGroundNotice(strNotice, dwColor, type, bEvent);
	pThis->SendPacketAllUser(pPacketBattleGroundNotice);
	
	return 0;
}

int AgsmBattleGround::SpawnCharacters(lua_State* L)
{
	AgsmBattleGround* pThis = AgsmBattleGround::GetInstance();
	
	INT32 lIndex = 0;
	AgpdCharacter* pcsCharacter = NULL;
	while (pcsCharacter = pThis->m_pagpmCharacter->GetCharSequence(&lIndex))
	{
		if (!pThis->m_pagpmCharacter->IsPC(pcsCharacter) || pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter) == 0) continue;
		if (!pThis->m_pagpmBattleGround->IsInBattleGround(pcsCharacter)) continue;

		AuPOS stNewPosition;
		if (!pThis->m_pagpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stNewPosition)) continue;
		
		AuAutoLock pLock(pcsCharacter->m_Mutex);
		if(!pLock.Result())
			continue;

		pThis->m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);
		pThis->m_pagpmCharacter->UpdatePosition(pcsCharacter, &stNewPosition, 0, TRUE);
	}
	
	return 0;
}

int AgsmBattleGround::RemoveMonsters(lua_State* L)
{
	AgsmBattleGround* pThis = AgsmBattleGround::GetInstance();
	
	AgpdSpawnGroup *	pstGroup = NULL;
	INT32				lIndex = 0;

	while(pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for(ApdEvent *pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn*)pstEvent->m_pvData)->m_pstNext)
		{
			AgpdSpawn* pSpawnData = (AgpdSpawn*)pstEvent->m_pvData;
			if (!pSpawnData)
				break;

			if (pSpawnData->m_lSiegeWarType == AGPDSPAWN_BATTLEGROUND_NORMAL || pSpawnData->m_lSiegeWarType == AGPDSPAWN_BATTLEGROUND_ABILITY)
			{
				AgpdSpawnInfo stSpawnCharacters[AGPDSPAWN_MAX_SPAWNED_CHAR];
				memcpy(stSpawnCharacters, pSpawnData->m_stSpawnCharacters, sizeof(pSpawnData->m_stSpawnCharacters));

				for (int i = 0; i < pSpawnData->m_lTotalCharacter; ++i)
				{
					if (stSpawnCharacters[i].m_lID == AP_INVALID_CID)
						continue;

					AgpdCharacter	*pcsCharacter	= pThis->m_pagpmCharacter->GetCharacter(stSpawnCharacters[i].m_lID);
					if(!pcsCharacter)
						continue;

					if(pThis->IsEventBattleGroud())
					{
						if(pThis->m_pagpmBattleGround->IsInBattleGround(pcsCharacter)) continue;
					}

					pThis->AbilityMonsterRemove(NULL, pcsCharacter);
					pThis->m_pagpmCharacter->RemoveCharacter(stSpawnCharacters[i].m_lID, FALSE, FALSE);
				}
			}
		}
	}
	
	return 0;
}

BOOL AgsmBattleGround::ProcessSpawn(char *pstrGroupName)
{
	AgpdSpawnGroup	*pcsSpawnGroup	= m_pcsAgpmEventSpawn->GetGroup(pstrGroupName);
	if(!pcsSpawnGroup)	return FALSE;

	for (ApdEvent* pstEvent = pcsSpawnGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
	{
		AgpdSpawn *pstSpawn = (AgpdSpawn*)pstEvent->m_pvData;

		for(INT32 nSpawnNum = 0; nSpawnNum < pstSpawn->m_lTotalCharacter; ++nSpawnNum)
		{
			INT32 lSiegeWarType = ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType;
			if(lSiegeWarType != AGPDSPAWN_BATTLEGROUND_ABILITY) continue;

			AuMATRIX	*pstDirection	= NULL;;
			AuPOS		stSpawnPos	= *m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, &pstDirection);

			m_pcsAgsmEventSpawn->ProcessSpawn(pstEvent, 0);
		}
	}

	return TRUE;
}

int AgsmBattleGround::SpawnAbilityMonster(lua_State* L)
{
	AgsmBattleGround*	pThis = AgsmBattleGround::GetInstance();

	for(CSpawnVector::iterator it = pThis->m_pSpawnVector.begin(); it != pThis->m_pSpawnVector.end(); ++it)
	{
		if(it->pElem == NULL) continue;

		if(pThis->GetSpawnGroupCharCount((CHAR*)it->strSpawnGroupName.c_str()) == 0 )
		{
			std::string strNextGroup = it->pElem->Attribute("Name");

			pThis->ProcessSpawn((CHAR*)strNextGroup.c_str());

			CHAR* szNotice = (CHAR*)it->pElem->Attribute("Notice");
			if(szNotice)
			{
				CHAR strNotice[MAX_NOTICE_LEN] = { 0, };
				DWORD dwColor = 0xFFFFFFFF;
				AuXmlElement* pElem2 = NULL;

				pElem2 = pThis->GetNoticeNode()->FirstChildElement(szNotice);
				if(pElem2)
				{
					CHAR* strFormat = (CHAR*)pElem2->GetText();
					sprintf( strNotice, strFormat);

					CHAR* color = (CHAR*)pElem2->Attribute("Color");
					if(color)
					{
						CHAR* ptr;
						dwColor = strtoul(color, &ptr, 16);
					}

					PACKET_BATTLEGROUND_NOTICE pPacketBattleGroundNotice(strNotice, dwColor, 0, FALSE);
					pThis->SendPacketAllUser(pPacketBattleGroundNotice);
				}
			}
			
			it->pElem = it->pElem->NextSiblingElement();
			it->strSpawnGroupName = strNextGroup;
		}
	}

	lua_pushinteger(L, TRUE);
	
	return 1;
}

int AgsmBattleGround::GetSpawnGroupCharCount(char* strGroupName)
{
	INT32 nCount = 0;
	
	AgpdSpawnGroup	*pcsSpawnGroup	= m_pcsAgpmEventSpawn->GetGroup(strGroupName);
	if(!pcsSpawnGroup)
		return 0;

	for (ApdEvent* pstEvent = pcsSpawnGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
	{
		AgpdSpawn *pstSpawn = (AgpdSpawn*)pstEvent->m_pvData;

		if(!pstSpawn)
			continue;

		for(INT32 i = 0; i < pstSpawn->m_lTotalCharacter; i++)
		{
			if(pstSpawn->m_stSpawnCharacters[i].m_lID != 0)
				nCount++;
		}
	}

	return nCount;
}

BOOL AgsmBattleGround::ProcessCharismaPoint( AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget )
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	INT32 lAttackerLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);
	INT32 lTargetLevel		= m_pagpmCharacter->GetLevel(pcsTarget);
	INT32 lPoint = -1, lExtraType = 0;
	INT32 lLevelDiff = lTargetLevel - lAttackerLevel;
	INT32 lKillableCount = 0;
	
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY__PVP_CHARISMA_DROP) == APMMAP_PECULIARITY_RETURN_ENABLE_USE &&
			m_papmMap->CheckRegionPerculiarity(pcsTarget->m_nBindingRegionIndex, APMMAP_PECULIARITY__PVP_CHARISMA_DROP) == APMMAP_PECULIARITY_RETURN_ENABLE_USE)
		{
			if( lLevelDiff >= 10 )
				lPoint = 20,	lExtraType = 1;
			else if(lLevelDiff >= 1)
				lPoint = 10,	lExtraType = 1;
			else if(lLevelDiff == 0)
				lPoint = 5,		lExtraType = 2;
			else if(lLevelDiff >= -9)
				lPoint = 3,		lExtraType = 3;
			else
				lPoint = 0,		lExtraType = 3;

			// 어뷰징 방지 - arycoat 2008.7
			AgsdDeath* pcsAgsdDeath	= m_pagsmDeath->GetADCharacter(pcsTarget);
			if(pcsAgsdDeath)
			{
				CTime cCurrentTime = CTime::GetCurrentTime();

				if( cCurrentTime - pcsAgsdDeath->m_tCharismaGiveTime < CTimeSpan( 0, 0, 10, 0) )
					lPoint = 0;
				else
					pcsAgsdDeath->m_tCharismaGiveTime = cCurrentTime;
			}
		}
	}
	else if(m_pagpmCharacter->IsMonster(pcsTarget) && m_pagpmCharacter->IsPC(pcsCharacter))
	{
		if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY__MOB_CHARISMA_DROP) == APMMAP_PECULIARITY_RETURN_ENABLE_USE &&
			m_papmMap->CheckRegionPerculiarity(pcsTarget->m_nBindingRegionIndex, APMMAP_PECULIARITY__MOB_CHARISMA_DROP) == APMMAP_PECULIARITY_RETURN_ENABLE_USE)
		{
			if( lLevelDiff >= 10 )
				lPoint = 8,	lExtraType = 1;
			else if(lLevelDiff >= 1)
				lPoint = 6,	lExtraType = 1;
			else if(lLevelDiff == 0)
				lPoint = 4,	lExtraType = 2;
			else if(lLevelDiff >= -9)
				lPoint = 2,	lExtraType = 3;
			else
				lPoint = 0,	lExtraType = 3;

			INT32 lGameBonusCharismaRate = m_pagpmCharacter->GetGameBonusCharismaRate(pcsCharacter);

			BOOL  bTPackUser	= FALSE;		
			INT32 ulPCRoomType	= 0;

			m_pagpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsCharacter, &ulPCRoomType);
			if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
				bTPackUser = TRUE;

			lPoint = (m_pcsAgpmConfig->GetCharismaDropAdjustmentRatio(bTPackUser) + (float)lGameBonusCharismaRate/(float)100) * (float)lPoint;
		}

	}
	else
		return FALSE;
	
	if(lPoint <= 0)
		return TRUE;
	
	m_pagpmCharacter->AddCharismaPoint(pcsCharacter, lPoint);

	m_pagsmPvP->SendSystemMessage(AGPMPVP_SYSTEM_CODE_CHARISMA_UP, m_pagsmCharacter->GetCharDPNID(pcsCharacter), NULL, NULL, lPoint, lLevelDiff);
	
	m_pagsmCharacter->SendPacketCharUseEffect(pcsCharacter, AGPDCHAR_ADDITIONAL_EFFECT_CHARISMA_POINT, lExtraType);

	if (m_pagpmLog)
	{
		AgsdCharacter* pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pAgsdCharacter)
			return FALSE;

		INT32 lLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);
		INT64 llExp		= m_pagpmCharacter->GetExp(pcsCharacter);	

		m_pagpmLog->WriteLog_CharismaUp(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID,
			pAgsdCharacter->m_szServerName, pcsCharacter->m_szID,
			((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID,
			lLevel, llExp, pcsCharacter->m_llMoney, pcsCharacter->m_llBankMoney,
			lPoint, m_pagpmCharacter->GetCharismaPoint(pcsCharacter), pcsTarget->m_szID
			);
	}

	return TRUE;
}

BOOL AgsmBattleGround::CheckControlSkill(AgpdCharacter* pcsCharacter, AgpdSkill *pcsSkill)
{
	if(!pcsCharacter) 
		return FALSE;

	if(!m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		return FALSE;
		
	switch(m_pBattleGroundState)
	{
		case BATTLEGROUND_STATE_ING:
		case BATTLEGROUND_STATE_NORMAL:
		{
			return AbilityMonsterPush(pcsCharacter, pcsSkill);
		} break;
	}
	
	return FALSE;
}

AuXmlElement* AgsmBattleGround::FindSkillCondition(AuXmlNode* node, string strGroup, int TID)
{
	for(AuXmlNode* node2 = node->FirstChild("Group"); node2; node2 = node2->NextSibling())
	{
		AuXmlElement* pElemt = node2->ToElement();
		if(pElemt->Attribute("SpawnGroup") != strGroup)
			continue;

		for(AuXmlElement* node4 = node2->FirstChildElement("Skill"); node4; node4 = node4->NextSiblingElement())
		{
			int nTID;

			if( (node4->QueryIntAttribute("TID", &nTID) != TIXML_SUCCESS) || ( nTID != TID)) continue;

			return node4;
		}
	}

	return NULL;
}

/*
<BattleGroundSpawnData>
	<AbilityControl>
		<Group SpawnGroup="fbo001">
			<Skill TID="954" Delay="60" Count="1" LastSpawn=""></Skill>
			<Character AgpdCharacter=""></Character>
		</Group>
		...
	</AbilityControl>
	...
</BattleGroundSpawnData>
*/

BOOL AgsmBattleGround::AbilityMonsterPush(AgpdCharacter* pcsCharacter, AgpdSkill *pcsSkill)
{
	AgpdSpawnADChar* pagpdSpawnADChar = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter);
	if(!pagpdSpawnADChar)
		return FALSE;

	AuXmlNode* node = m_pAbilityNode;
	if(!node)
		return FALSE;
	//JK_서버크래쉬 수정
	if(!pagpdSpawnADChar->m_pstSpawn || !pagpdSpawnADChar->m_pstSpawn->m_pstParent)
		return FALSE;

	string strSpawnGrup(pagpdSpawnADChar->m_pstSpawn->m_pstParent->m_szName);
	AuXmlElement* pElem = FindSkillCondition(node, strSpawnGrup, pcsSkill->m_pcsTemplate->m_lID);
	if(!pElem)
		return FALSE;

	UINT64 ulTime = AuTimeStamp2::GetCurrentTimeStamp();
	DOUBLE ulLastSpawnTime;

	INT32 nDelay, nCount;
	pElem->QueryIntAttribute("Delay", &nDelay);
	pElem->QueryIntAttribute("Count", &nCount);

	if(pElem->QueryDoubleAttribute("LastSpawn", &ulLastSpawnTime) == TIXML_SUCCESS)
	{
		if(ulLastSpawnTime > ulTime - nDelay*1000)
			return FALSE;
	}

	int count = 0;
	for( AuXmlNode* node = pElem->FirstChild(); node; node = node->NextSibling(), ++count )
	{
	}

	if( count >= nCount)
		return FALSE;

	AuAutoLock pLock(m_MutexAbilityXml);
	if(!pLock.Result())
		return FALSE;

	AuXmlElement pCharacter("Character");
	pCharacter.SetAttribute("AgpdCharacter", pcsCharacter->m_lID);
	pElem->InsertEndChild(pCharacter);
	
	m_AMonsterIndex.push_back(pcsCharacter->m_lID);

	return TRUE;
}

void AgsmBattleGround::AbilityMonsterRemove(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	ACharacterVector::iterator it = find( m_AMonsterIndex.begin(), m_AMonsterIndex.end(), pcsTarget->m_lID);
	if(it == m_AMonsterIndex.end())
		return;

	m_AMonsterIndex.erase(it);

	AgpdSpawnADChar* pagpdSpawnADChar = m_pcsAgpmEventSpawn->GetCharacterData(pcsTarget);
	if(!pagpdSpawnADChar)
		return;

	AuXmlNode* node = m_pAbilityNode;
	if(!node)
		return;

	//JK_서버 크래쉬 수정
	if(!pagpdSpawnADChar->m_pstSpawn || !pagpdSpawnADChar->m_pstSpawn->m_pstParent)
		return;


	string strSpawnGrup(pagpdSpawnADChar->m_pstSpawn->m_pstParent->m_szName);
	for(AuXmlNode* node2 = node->FirstChild("Group"); node2; node2 = node2->NextSibling())
	{
		AuXmlElement* pElemt = node2->ToElement();
		if(pElemt->Attribute("SpawnGroup") != strSpawnGrup)
			continue;

		for(AuXmlElement* node3 = node2->FirstChildElement("Skill"); node3; node3 = node3->NextSiblingElement())
		{
			AuXmlElement* node5 = NULL;
			for(AuXmlElement* node4 = node3->FirstChildElement("Character"); node4; node4 = node4->NextSiblingElement())
			{
				int nTID;

				if( (node4->QueryIntAttribute("AgpdCharacter", &nTID) != TIXML_SUCCESS) || ( nTID != pcsTarget->m_lID)) continue;

				node5 = node4;
			}

			if(node5)
			{
				AuAutoLock pLock(m_MutexAbilityXml);
				if(!pLock.Result())
					return;

				node3->RemoveChild(node5);

				UINT64 ulTime = AuTimeStamp2::GetCurrentTimeStamp();
				node3->SetDoubleAttribute("LastSpawn", (DOUBLE)ulTime);
				break;
			}
		}
	}

	if(pcsCharacter)
	{
		//////////////////////////////////////////////////////////////////////////
		// Notice
		INT32 lSiegeWarType = pagpdSpawnADChar->m_pstSpawn->m_lSiegeWarType;
		if(lSiegeWarType == AGPDSPAWN_BATTLEGROUND_ABILITY)
		{
			//////////////////////////////////////////////////////////////////////////
			// notice
			AuXmlElement* pElem2 = GetNoticeNode()->FirstChildElement("Notice_13");
			if(pElem2)
			{
				CHAR strNotice[MAX_NOTICE_LEN] = { 0, };
				DWORD dwColor = 0xFFFFFFFF;
				CHAR* strFormat = (CHAR*)pElem2->GetText();
				CHAR* pszRace = m_pagpmCharacter->GetCharacterRaceString(pcsCharacter);
				CHAR* pszName = pcsTarget->m_pcsCharacterTemplate->GetName();
				sprintf( strNotice, strFormat, pszRace, pszName);

				CHAR* color = (CHAR*)pElem2->Attribute("Color");
				if(color)
				{
					CHAR* ptr;
					dwColor = strtoul(color, &ptr, 16);
				}

				PACKET_BATTLEGROUND_NOTICE pPacketBattleGroundNotice(strNotice, dwColor, 0, FALSE);
				SendPacketAllUser(pPacketBattleGroundNotice);
			}

			//////////////////////////////////////////////////////////////////////////
			// Hero Monster Process
			for(CSpawnVector::iterator it = m_pBossVector.begin(); it != m_pBossVector.end(); ++it)
			{
				if(it->pElem == NULL) continue;
				if(it->strSpawnGroupName != strSpawnGrup) continue;

				AuXmlElement* pBossElem = it->pElem->FirstChildElement("Boss");
				if(!pBossElem) continue;

				INT32 BossTID;
				pBossElem->Attribute("TID", &BossTID);				// <Boss TID="">

				if(BossTID != pcsTarget->m_pcsCharacterTemplate->m_lID) continue;

				//////////////////////////////////////////////////////////////////////////
				// notice
				AuXmlElement* pNoticeElem = GetNoticeNode()->FirstChildElement(pBossElem->Attribute("KillNotice"));
				if(pNoticeElem)
				{
					CHAR strNotice2[MAX_NOTICE_LEN] = { 0, };
					DWORD dwColor = 0xFFFFFFFF;
					CHAR* strFormat = (CHAR*)pNoticeElem->GetText();
					CHAR* pszRace1 = m_pagpmCharacter->GetCharacterRaceString(pcsCharacter);
					CHAR* pszRace2 = m_pagpmCharacter->GetCharacterRaceString(pcsTarget);
					sprintf( strNotice2, strFormat, pszRace1, pszRace2);

					CHAR* color = (CHAR*)pNoticeElem->Attribute("Color");
					if(color)
					{
						CHAR* ptr;
						dwColor = strtoul(color, &ptr, 16);
					}

					PACKET_BATTLEGROUND_NOTICE pPacketBattleGroundNotice(strNotice2, dwColor, 0, FALSE);
					SendPacketAllUser(pPacketBattleGroundNotice);
				}
				
				WriteBattleGroundLog(AGPDLOGTYPE_ETC_WC_KILL, pcsCharacter, pcsTarget);
			}
		}
	}
}

void AgsmBattleGround::SetBattleGroundTime(BOOL bUse, INT32 nDay, INT32 nHour, INT32 nMin, INT32 nDuring, BOOL bEvent)
{
	lua_getfield(pLuaState, LUA_GLOBALSINDEX, "Manual_time_set");
	assert(lua_istable(pLuaState, -1));

	lua_pushnumber(pLuaState, bUse);
	lua_setfield(pLuaState, -2, "use");
	
	lua_pushnumber(pLuaState, nDay);
	lua_setfield(pLuaState, -2, "wday");

	lua_pushnumber(pLuaState, nHour);
	lua_setfield(pLuaState, -2, "hour");

	lua_pushnumber(pLuaState, nMin);
	lua_setfield(pLuaState, -2, "min");

	lua_pushnumber(pLuaState, nDuring);
	lua_setfield(pLuaState, -2, "during");

	m_bIsEventBattleGround = bEvent;
}

int AgsmBattleGround::BattleGroundStatistic(lua_State* L)
{
	AgsmBattleGround* pThis = AgsmBattleGround::GetInstance();

	CRaceMap vecRace;

	INT32 lIndex = 0;
	AgpdCharacter* pcsCharacter = NULL;
	while(pcsCharacter = pThis->m_pagpmCharacter->GetCharSequence(&lIndex))
	{
		if(!pThis->m_pagpmCharacter->IsPC(pcsCharacter) || pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter) == 0)
			continue;

		if(!pThis->m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
			continue;

		AuRace pRace = pThis->m_pagpmCharacter->GetCharacterRace(pcsCharacter);

		CRaceMap::iterator it = vecRace.find(pRace);
		if( it == vecRace.end())
			it = (vecRace.insert(CRaceMap::value_type(pRace, 0))).first;

		++(it->second);
	}

	for(CRaceMap::iterator it2 = vecRace.begin(); it2 != vecRace.end(); ++it2)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d]:[%02d] %04d", (it2->first).detail.nRace, (it2->first).detail.nClass, (it2->second));
		AuLogFile_s("LOG\\BattleGround.txt", strCharBuff);
	}

	return 0;
}

BOOL AgsmBattleGround::IsEventBattleGroud()
{
	return m_bIsEventBattleGround;
}
	

BOOL AgsmBattleGround::WriteBattleGroundLog(eAGPDLOGTYPE_ETC eType, AgpdCharacter *pcsKillCharacter, AgpdCharacter *pcsTarget)
{
	if(!pcsKillCharacter || !pcsTarget)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsKillCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	TCHAR szValue[56];
	ZeroMemory(szValue, sizeof(szValue));

	_stprintf(szValue, _T("(%s) 승=%s, 패=%s"), pAgsdCharacter->m_szServerName,
												m_pagpmCharacter->GetCharacterRaceString(pcsKillCharacter),
												m_pagpmCharacter->GetCharacterRaceString(pcsTarget));
	
	return m_pagpmLog->WriteLog_ETC(eType,
									0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pcsTarget->m_pcsCharacterTemplate->m_szTName,
									pcsKillCharacter->m_pcsCharacterTemplate->m_lID,
									m_pagpmCharacter->GetLevel(pcsKillCharacter),
									m_pagpmCharacter->GetExp(pcsKillCharacter),
									pcsKillCharacter->m_llMoney,
									pcsKillCharacter->m_llBankMoney,
									0,
									NULL,
									szValue,
									0,
									pcsKillCharacter->m_szID
									);
	
}

BOOL AgsmBattleGround::SendBattleGroundNotice(CHAR* szNotice, INT32 lNoticeType)
{
	if(NULL == szNotice)
		return FALSE;

	CHAR strNotice[MAX_NOTICE_LEN] = { 0, };
	DWORD dwColor = 0xFFFFFFFF;
	AuXmlElement* pElem2 = NULL;

	pElem2 = GetNoticeNode()->FirstChildElement(szNotice);
	if(pElem2)
	{
		CHAR* strFormat = (CHAR*)pElem2->GetText();
		sprintf(strNotice, strFormat);

		CHAR* color = (CHAR*)pElem2->Attribute("Color");
		if(color)
		{
			CHAR* ptr;
			dwColor = strtoul(color, &ptr, 16);
		}

		PACKET_BATTLEGROUND_NOTICE pPacketBattleGroundNotice(strNotice, dwColor, lNoticeType, FALSE);
		SendPacketAllUser(pPacketBattleGroundNotice);
	}

	return TRUE;
	
}

BOOL AgsmBattleGround::SendBattleGroundNotice(AgpdCharacter *pcsCharacter, CHAR* szNotice, INT32 lNoticeType, CHAR* szDirectNotice)
{
	if(NULL == szNotice || NULL == pcsCharacter)
		return FALSE;

	CHAR strNotice[MAX_NOTICE_LEN] = { 0, };
	DWORD dwColor = 0xFFFFFFFF;
	AuXmlElement* pElem2 = NULL;

	if(lNoticeType == AGPM_BATTLEGROUND_NOTICE_EPICZONE)
	{
		pElem2 = GetEpicNoticeNode()->FirstChildElement(szNotice);
	}
	else
	{
		pElem2 = GetNoticeNode()->FirstChildElement(szNotice);
	}

	if(pElem2)
	{
		if(szDirectNotice)
		{
			sprintf(strNotice, szDirectNotice);
		}
		else
		{
			CHAR* strFormat = (CHAR*)pElem2->GetText();
			sprintf(strNotice, strFormat);
		}

		CHAR* color = (CHAR*)pElem2->Attribute("Color");
		if(color)
		{
			CHAR* ptr;
			dwColor = strtoul(color, &ptr, 16);
		}

		PACKET_BATTLEGROUND_NOTICE pPacketBattleGroundNotice(strNotice, dwColor, lNoticeType, FALSE);
		AgsEngine::GetInstance()->SendPacket(pPacketBattleGroundNotice, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	}

	return TRUE;

}

BOOL AgsmBattleGround::CBGetBattleGroundState(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmBattleGround *pThis		=	(AgsmBattleGround *)pClass;
	INT32 *pState				=	(INT32 *)pData;

	*pState = (INT32)pThis->GetBattleGroundState();

	return TRUE;
}
