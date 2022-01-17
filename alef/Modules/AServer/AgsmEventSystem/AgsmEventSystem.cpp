#include "AgsmEventSystem.h"
#include <AgspEventSystem.h>
#include <AgpmCharacter.h>
#include <AgsmCharacter.h>
#include <AgsmServerManager2.h>
#include <AgsmNpcManager.h>
#include <AgpmItem.h>
#include <AgsmItemManager.h>

AgsmEventSystem::AgsmEventSystem()
{
	SetModuleName("AgsmEventSystem");

	m_pagpmCharacter = NULL;
	m_pagsmCharacter = NULL;
	m_pagsmServerManager2 = NULL;
	m_pagsmNpcManager = NULL;
	m_pagsmAccountManager = NULL;
	m_pagpmItem = NULL;
	m_pagsmItem = NULL;
	m_pagsmItemManager = NULL;
	m_pagpmLog = NULL;
}

AgsmEventSystem::~AgsmEventSystem()
{

}

BOOL AgsmEventSystem::OnInit()
{
	m_pagpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagsmCharacter		= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagsmServerManager2	= (AgsmServerManager2 *)GetModule("AgsmServerManager2");
	m_pagsmNpcManager		= (AgsmNpcManager*)GetModule("AgsmNpcManager");
	m_pagsmAccountManager	= (AgsmAccountManager*)GetModule("AgsmAccountManager");
	m_pagpmItem				= (AgpmItem*)GetModule("AgpmItem");
	m_pagsmItem				= (AgsmItem*)GetModule("AgsmItem");
	m_pagsmItemManager		= (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pagpmLog				= (AgpmLog*)GetModule("AgpmLog");

	if( !m_pagpmCharacter || !m_pagsmCharacter ||
		!m_pagsmServerManager2 ||
		!m_pagsmNpcManager ||
		!m_pagsmAccountManager ||
		!m_pagpmItem || !m_pagsmItem || !m_pagsmItemManager ||
		!m_pagpmLog)
		return FALSE;

	return TRUE;
}

BOOL AgsmEventSystem::ProcessAttendance( INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type )
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	int nIndex = 0;
	AgsdServer* pGameServer = m_pagsmServerManager2->GetThisServer();

	if(!pGameServer)
		return FALSE;

	PACKET_AGSMCHARACTER_RELAY_ATTENDANCE_REQUEST pRelayPacket(pagsdCharacter->m_szAccountID, pcsCharacter->m_szID, pGameServer->m_szGroupName);
	pRelayPacket.CID = CID;
	pRelayPacket.NID = NID;
	pRelayPacket.EID = EID;
	pRelayPacket.STEP = STEP;
	pRelayPacket.Type = Type;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::ProcessAttendanceResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult)
{
	m_pagsmNpcManager->OnCheckAttendance(CID, NID, EID, STEP, Type, nResult);

	return TRUE;
}

BOOL AgsmEventSystem::ProcessEventUser(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	int nIndex = 0;
	AgsdServer* pGameServer = m_pagsmServerManager2->GetThisServer();

	if(!pGameServer)
		return FALSE;

	PACKET_AGSMCHARACTER_RELAY_EVENT_USER_REQUEST pRelayPacket(pagsdCharacter->m_szAccountID, pcsCharacter->m_szID, pGameServer->m_szGroupName);
	pRelayPacket.CID = CID;
	pRelayPacket.NID = NID;
	pRelayPacket.EID = EID;
	pRelayPacket.STEP = STEP;
	pRelayPacket.Type = Type;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}
BOOL AgsmEventSystem::ProcessEventUserResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult)
{
	m_pagsmNpcManager->OnCheckEventUser(CID, NID, EID, STEP, Type, nResult);
	return TRUE;
}


BOOL AgsmEventSystem::ProcessEventUserFlagUpdate( INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 Flag )
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	int nIndex = 0;
	AgsdServer* pGameServer = m_pagsmServerManager2->GetThisServer();

	if(!pGameServer)
		return FALSE;

	PACKET_AGSMCHARACTER_RELAY_EVENT_USER_FLAG_UPDATE pRelayPacket(pagsdCharacter->m_szAccountID, pcsCharacter->m_szID, pGameServer->m_szGroupName);
	pRelayPacket.CID = CID;
	pRelayPacket.NID = NID;
	pRelayPacket.EID = EID;
	pRelayPacket.STEP = STEP;
	pRelayPacket.Type = Type;
	pRelayPacket.Flag = Flag;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
BOOL AgsmEventSystem::ProcessChangeName(INT32 CID, INT32 NID, INT32 EID, INT32 STEP)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdServer* pGameServer = m_pagsmServerManager2->GetThisServer();
	if(!pGameServer)
		return FALSE;

	m_pagpmCharacter->SetActionBlockTime(pcsCharacter, 10000);

	CHAR strNewName[AGPDCHARACTER_MAX_ID_LENGTH+1] = { 0, };
	sprintf_s(strNewName, sizeof(strNewName), "%s@%s", pcsCharacter->m_szID, pGameServer->m_szGroupName);

	PACKET_AGSMCHARACTER_RELAY_CHANGENAME_REQUEST pPacket(pcsCharacter->m_szID, strNewName);
	pPacket.CID = CID;
	pPacket.NID = NID;
	pPacket.EID = EID;
	pPacket.STEP = STEP;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::ProcessChangeNameResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;
	
	m_pagpmCharacter->ClearActionBlockTime(pcsCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "ChangeName : CharacterID = [%s] Result = [%d]", pcsCharacter->m_szID, nResult);
	AuLogFile_s("Log\\EventSystem.log", strCharBuff);

	if(nResult == 0) // success
	{
		AgsdServer* pGameServer = m_pagsmServerManager2->GetThisServer();
		if(!pGameServer)
			return FALSE;

		AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pagsdCharacter)
			return FALSE;

		AgsdAccount	*pcsAgsdAccount	= m_pagsmAccountManager->GetAccount(pagsdCharacter->m_szAccountID);
		if (!pcsAgsdAccount || pcsAgsdAccount->m_nNumChar < 1)
			return FALSE;

		m_pagpmCharacter->m_csACharacter.RemoveCharacter(pcsCharacter->m_lID, pcsCharacter->m_szID);

		CHAR strNewName[AGPDCHARACTER_MAX_ID_LENGTH+1] = { 0, };
		CHAR strOldName[AGPDCHARACTER_MAX_ID_LENGTH+1] = { 0, };
		sprintf_s(strNewName, sizeof(strNewName), "%s@%s", pcsCharacter->m_szID, pGameServer->m_szGroupName);
		sprintf_s(strOldName, sizeof(strOldName), "%s", pcsCharacter->m_szID);

		memset(pcsCharacter->m_szID, NULL, AGPDCHARACTER_MAX_ID_LENGTH+1);
		strcpy_s(pcsCharacter->m_szID, AGPDCHARACTER_MAX_ID_LENGTH, strNewName);

		memset(pcsAgsdAccount->m_szCharName[0], NULL, AGPACHARACTER_MAX_ID_STRING+1);
		strcpy_s(pcsAgsdAccount->m_szCharName[0], AGPACHARACTER_MAX_ID_STRING, strNewName);

		m_pagpmCharacter->m_csACharacter.AddCharacter(pcsCharacter, pcsCharacter->m_lID, pcsCharacter->m_szID);

		m_pagpmLog->WriteLog_ETC(	AGPDLOGTYPE_ETC_RENAME,
									0,
									&pagsdCharacter->m_strIPAddress[0],
									pagsdCharacter->m_szAccountID,
									pagsdCharacter->m_szServerName,
									strOldName,
									pcsCharacter->m_pcsCharacterTemplate->m_lID,
									m_pagpmCharacter->GetLevel(pcsCharacter),
									m_pagpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									0,
									NULL,
									NULL,
									0,
									strNewName
									);
	}

	m_pagsmNpcManager->OnChangeNameResult(CID, NID, EID, STEP, nResult);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 캐릭터 봉인 시스템 - 2009.04
BOOL AgsmEventSystem::ProcessCerariumOrb(INT32 CID, INT32 NID, INT32 EID, INT32 STEP)
{
	switch(EID)
	{
		case 100: // 캐릭터 리스트 요청
			{
				CharacterSealingRequestList(CID, NID, EID, STEP);
			} break;
		case 101:
			{
				CharacterSealingRequestCharacterName(CID, NID, EID, STEP);
			} break;
		case 200: // 해제
			{
				ProcessCharacterReleaseSealing(CID, NID, EID, STEP);
			} break;
	}

	return TRUE;
}

BOOL AgsmEventSystem::CharacterSealingRequestCharacterName(INT32 CID, INT32 NID, INT32 EID, INT32 STEP)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	PACKET_RELAY_CERARIUMORB_CHARACTERNAME_REQUEST pPacket;
	pPacket.CID = CID;
	pPacket.NID = NID;
	pPacket.EID = EID;
	pPacket.STEP = -1;
	strcpy(pPacket.strAccountName, pagsdCharacter->m_szAccountID);
	strcpy(pPacket.strWorldName, pagsdCharacter->m_szServerName);
	pPacket.Slot = STEP;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::CharacterSealingResultCharacterName(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR* strCharName)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	if(!strcmp(pcsCharacter->m_szID, strCharName))
	{
		INT32 nResult = 2;
		m_pagsmNpcManager->CerariumOrbResult(CID, NID, EID, STEP, nResult);
	}
	else
	{
		CharacterSealing(CID, NID, EID, STEP, strCharName);
	}

	return TRUE;
}

BOOL AgsmEventSystem::CharacterSealing(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR* strCharName)
{
	INT32 ItemTID = 11406; // 케라리움 오브
	AgpdItemTemplate* pItemTemplate = m_pagpmItem->GetItemTemplate(ItemTID);
	if (!pItemTemplate)
		return FALSE;

	AgpdItem* pcsItem = m_pagsmItemManager->CreateItem(pItemTemplate->m_lID);
	if(!pcsItem)
		return FALSE;

	AgsdItem* pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if(!pcsAgsdItem)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	PACKET_RELAY_CERARIUMORB_SEALING pPacket;
	pPacket.CID = CID;
	pPacket.NID = NID;
	pPacket.EID = EID;
	pPacket.STEP = -1;
	strcpy(pPacket.strCharName, strCharName);
	pPacket.ItemID = pcsItem->m_lID;
	pPacket.ullDBIID = pcsAgsdItem->m_ullDBIID;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::CharacterSealingResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 ItemID, UINT64 DBID, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsItem = m_pagpmItem->GetItem(ItemID);
	if(!pcsItem)
		return FALSE;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Character Sealing : CharacterID = [%s] ItemSeq = [%I64d] Result = [%d]", pcsCharacter->m_szID, DBID, nResult);
	AuLogFile_s("Log\\EventSystem.log", strCharBuff);

	if(nResult == 1) // success
	{
		m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_GIVE, pcsCharacter->m_lID,
										pcsItem,
										pcsItem->m_nCount ? pcsItem->m_nCount : 1
										);

		m_pagpmItem->AddItemToInventory(pcsCharacter, pcsItem);

		PACKET_RELAY_CERARIUMORB_SEALING_DATA_REQUEST pPacket;
		pPacket.ItemID = ItemID;
		pPacket.ullDBIID = DBID;

		AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
		if (!pRelayServer || !pRelayServer->m_bIsConnected)
			return FALSE;

		AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);
	}
	
	m_pagsmNpcManager->CerariumOrbResult(CID, NID, EID, STEP, nResult);

	return TRUE;
}

BOOL AgsmEventSystem::ProcessCharacterReleaseSealing(INT32 CID, INT32 NID, INT32 EID, INT32 STEP)
{
	AgpdItem* pcsItem = m_pagpmItem->GetItem(STEP); // ItemID
	if(!pcsItem)
		return FALSE;

	AgsdItem* pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if(!pcsAgsdItem)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	PACKET_RELAY_CERARIUMORB_RELEASE_SEAL_REQUEST pPacket;
	pPacket.CID  = CID;
	pPacket.NID  = NID;
	pPacket.EID  = EID;
	pPacket.STEP = -1;
	strcpy(pPacket.strCharName, pcsCharacter->m_szID);
	pPacket.ItemID = pcsItem->m_lID;
	pPacket.ullDBIID = pcsAgsdItem->m_ullDBIID;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::CharacterSealingRequestList(INT32 CID, INT32 NID, INT32 EID, INT32 STEP)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	PACKET_RELAY_CERARIUMORB_LIST_REQUEST pPacket;
	pPacket.CID = CID;
	pPacket.NID = NID;
	pPacket.EID = EID;
	pPacket.STEP = STEP;
	strcpy(pPacket.strAccountName, pagsdCharacter->m_szAccountID);
	strcpy(pPacket.strWorldName, pagsdCharacter->m_szServerName);

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::CharacterSealingResultList(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR** strCharName, INT32* lSlot)
{
	m_pagsmNpcManager->CharacterSealingResultList(CID, NID, EID, STEP, strCharName, lSlot);	
	return TRUE;
}

BOOL AgsmEventSystem::SendRequestSealingDataAll()
{
	PACKET_RELAY_CERARIUMORB_SEALING_ALL_DATA_REQUEST pPacket;

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::ReceiveSealingData(INT32 ItemID, PVOID SealData)
{
	AgsdSealData* pcsSealData = (AgsdSealData*)SealData;

	if(/*ItemID == 0 ||*/ pcsSealData == NULL)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//
	m_pagsmItemManager->AddSealingData(pcsSealData);

	//////////////////////////////////////////////////////////////////////////
	//
	AgpdItem* pcsItem = m_pagpmItem->GetItem(ItemID);
	if(!pcsItem)
		return FALSE;

	AgpdCharacter* pcsCharacter = pcsItem->m_pcsCharacter;
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pagsdCharacter)
		return FALSE;

	m_pagsmItem->SendPacketItem(pcsItem, pagsdCharacter->m_dpnidCharacter);

	m_pagpmLog->WriteLog_ETC(	AGPDLOGTYPE_ETC_SEAL,
									0,
									&pagsdCharacter->m_strIPAddress[0],
									pagsdCharacter->m_szAccountID,
									pagsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									pcsCharacter->m_pcsCharacterTemplate->m_lID,
									m_pagpmCharacter->GetLevel(pcsCharacter),
									m_pagpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									0,
									NULL,
									NULL,
									0,
									pcsSealData->SealData.strCharName
									);

	return TRUE;
}

BOOL AgsmEventSystem::CharacterReleaseSealingResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 ItemID, UINT64 DBID, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	AgpdItem* pcsItem = m_pagpmItem->GetItem(ItemID);
	if(!pcsItem)
		return FALSE;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Character Sealing : CharacterID = [%s] ItemSeq = [%I64d] Result = [%d]", pcsCharacter->m_szID, DBID, nResult);
	AuLogFile_s("Log\\EventSystem.log", strCharBuff);

	if(nResult == 1) // success
	{
		m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
									pcsItem,
									pcsItem->m_nCount ? pcsItem->m_nCount : 1
									);
	
		m_pagpmItem->RemoveItem(pcsItem, TRUE);

		//////////////////////////////////////////////////////////////////////////
		//
		AgsdItem* pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
		if(!pcsAgsdItem)
			return FALSE;

		AgpdSealData* pcsSealData = m_pagsmItemManager->GetSealingData(pcsAgsdItem->m_ullDBIID);
		if(pcsSealData)
		{
			m_pagpmLog->WriteLog_ETC(	AGPDLOGTYPE_ETC_RELEASESEAL,
										0,
										&pagsdCharacter->m_strIPAddress[0],
										pagsdCharacter->m_szAccountID,
										pagsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										pcsCharacter->m_pcsCharacterTemplate->m_lID,
										m_pagpmCharacter->GetLevel(pcsCharacter),
										m_pagpmCharacter->GetExp(pcsCharacter),
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										0,
										NULL,
										NULL,
										0,
										pcsSealData->strCharName
										);
		}

		m_pagsmItemManager->RemoveSealingData(pcsAgsdItem->m_ullDBIID);
	}
	
	m_pagsmNpcManager->CerariumOrbResult(CID, NID, EID, STEP, nResult);

	return TRUE;
}

BOOL AgsmEventSystem::ProcessCharacterCreationDateCheck(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, char* strStartDate, char* strEndDate)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	PACKET_RELAY_CHARACTER_CREATION_DATE_CHECK pRelayPacket;

	pRelayPacket.CID = CID;
	pRelayPacket.NID = NID;
	pRelayPacket.EID = EID;
	pRelayPacket.STEP = STEP;
	strcpy_s(pRelayPacket.strCharName, AGPDCHARACTER_MAX_ID_LENGTH, pcsCharacter->m_szID);
	strcpy_s(pRelayPacket.strStartDate, _MAX_DATETIME_LENGTH, strStartDate);
	strcpy_s(pRelayPacket.strEndDate, _MAX_DATETIME_LENGTH, strEndDate);

	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmEventSystem::ProcessCharacterCreationDateCheckResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult)
{
	m_pagsmNpcManager->OnUserCreationDateCheck(CID, NID, EID, STEP, nResult);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 서버이전 시스템 -JK_특성화서버
BOOL AgsmEventSystem::ProcessServerMove(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 ServerIndex)
{
	switch(EID)
	{
	case 101:
		{
			CharacterMoveServer(CID, NID, EID, STEP, ServerIndex);
		} break;
	}

	return TRUE;
}
//JK_특성화서버
BOOL AgsmEventSystem::CharacterMoveServer(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 ServerIndex)
{

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	AgsdServer2	*pcsServer	= m_pagsmServerManager2->GetServer(ServerIndex);
	if(!pcsServer)
		return FALSE;

	PACKET_AGSMCHARACTER_RELAY_SERVERMOVE_REQUEST pPacket;
	pPacket.CID = CID;
	pPacket.NID = NID;
	pPacket.EID = EID;
	pPacket.STEP = STEP;
	strcpy_s(pPacket.strServerGroupName, AGSM_MAX_SERVER_NAME, pcsServer->m_szGroupName);
	strcpy_s(pPacket.strCharName, AGPDCHARACTER_MAX_ID_LENGTH, pcsCharacter->m_szID);


	AgsdServer* pRelayServer = m_pagsmServerManager2->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}
//JK_특성화서버
BOOL AgsmEventSystem::ProcessServerMoveResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(CID);
	if(!pcsCharacter)
		return FALSE;

	//m_pagpmCharacter->ClearActionBlockTime(pcsCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "ServerMove : CharacterID = [%s] Result = [%d]", pcsCharacter->m_szID, nResult);
	AuLogFile_s("Log\\EventSystem.log", strCharBuff);



	m_pagsmNpcManager->ServerMoveResult(CID, NID, EID, STEP, nResult);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////