// AgsmAdminCharacter.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 03. 29.
//
// Code 양이 너무 길어져서 관리를 위해서 분리

#include "AgsmAdmin.h"
#include "AgsmTitle.h"
#include <list>

///////////////////////////////////////////////////////////////////////////////////////
// Search

// pstSearch 는 Search 한 데이터, pcsAgpdCharacter 는 Admin 의 AgpdCharacter 이다.
BOOL AgsmAdmin::SearchCharacter(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;

	// 레벨 세팅 2006.01.11. steeple
	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
		return FALSE;

	// 2004.08.16. steeple
	if(pstSearch->m_iField == 1)	// Account
	{
		// 2005.05.02. steeple
		// 드디어 Relay Search 개발한다~~~
		return SearchCharacterByAccount2(pstSearch, pcsAgpdCharacter);
		//return SearchCharacterByAccount(pstSearch, pcsAgpdCharacter);
	}

	AgpdAdmin* pcsAgpdAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdAdmin)
		return FALSE;

	// Admin 의 DPNID 를 받고
	DPNID dpnAdminID = m_pagsmCharacter->GetCharDPNID(pcsAgpdAdmin->m_lAdminCID);
	dpnAdminID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstSearch->m_lObjectCID != 0)
	{
		// Lock 작업 - 2004.01.16.
		if(pcsAgpdCharacter->m_lID == pstSearch->m_lObjectCID)
			pcsSearchCharacter = pcsAgpdCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSearch->m_lObjectCID);
	}
	else
	{
		if(strcmp(pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName) == 0)
			pcsSearchCharacter = pcsAgpdCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSearch->m_szSearchName);
	}

	if(!pcsSearchCharacter)// || m_pagsmCharacter->GetCharDPNID(pcsSearchCharacter) == 0)
	{
		// 접속해 있지 않은 유저일 수 있다.
		// DB 에서 찾아보자.
		//SearchCharacterFromDB(pstSearch->m_szSearchName, pstSearch->m_lObjectCID, pcsAgpdCharacter);
		SearchCharacterDB(pstSearch, pcsAgpdCharacter);
		return TRUE;
	}

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsSearchCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// 얘들 필요없다 -_-;;
	// 2004.03.15. - ServerID 를 체크해서 다른 서버에 있는 캐릭터라면 해당 서버로 검색 패킷을 보낸다.
	//{
	//	if(pcsAgsdCharacter->m_ulServerID != GetThisServerID())
	//	{
	//		SendSearchOtherServer(pstSearch, pcsAgpdCharacter->m_lID, GetGameServerNID(pcsAgsdCharacter->m_ulServerID));

	//		if(pcsSearchCharacter != pcsAgpdCharacter)
	//			pcsSearchCharacter->m_Mutex.Release();

	//		return TRUE;
	//	}
	//}

	//////////////////////////////////////////////////////////////////////////
	// 여기까지 온것이라면, 현재 서버에서 컨트롤 하는 캐릭터이다. 정보를 보낸다.

	// 기본정보를 보낸다.
	m_pagsmCharacter->SendCharacterAllInfo(pcsSearchCharacter, dpnAdminID);

	// Item 정보 따로 또 보낸다.
	SendCharAllItemUseCustom(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// Search Result 를 보낸다.
	SendSearchResult(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// 추가정보를 보낸다.
	SendSearchCharDataSub(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// Party 정보도 보낸다. 2004.03.22.
	//SendSearchCharParty(pcsSearchCharacter, pcsAgpdCharacter);

	// Char Guild ID 도 보낸다. 2005.04.25.
	m_pagsmGuild->SendCharGuildData(m_pagpmGuild->GetJoinedGuildID(pcsSearchCharacter), pcsSearchCharacter->m_szID, NULL,
									pcsSearchCharacter->m_lID, dpnAdminID);

	// Money 는 따로 또 보낸다. 2005.05.18.
	SendSearchCharMoney(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// 2005.09.14.
	// 서버 통합 이후 많아진 Bank 까지 알아야 하니 새로 요청한다.
	SearchAccountBank(pcsAgsdCharacter->m_szAccountID, pcsAgpdCharacter);

	// 현상수배 정보 보내준다. 2006.12.05. steeple
	SendCharWantedCriminal(pcsSearchCharacter->m_szID, pcsAgpdCharacter->m_lID, dpnAdminID);

	//퀘스트 정보를 전부 보내준다.
	SendCharQuestInfo(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	//타이틀 정보를 전부 보내준다.
	SendCharTitleInfo(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	if(pcsAgpdCharacter != pcsSearchCharacter)
		pcsSearchCharacter->m_Mutex.Release();

	return TRUE;
}

// DB 로 검색을 날린다.
// pcsAgpdCharacter 는 Admin 이다. 쿠후훗~
// 변경. 2004.03.15 - Game Server 라면 Relay 서버로 패킷을 보낸다.
BOOL AgsmAdmin::SearchCharacterFromDB(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	//if(!m_pagsmAdminDB)
	//	return FALSE;

	//if(IsGameServer())
	//{
	//	SendSearchOtherServer(pstSearch, pcsAgpdCharacter->m_lID, GetRelayServerNID());
	//	return TRUE;
	//}

	//stAgsdAdminSearchMatch stMatch;
	//ZeroMemory(&stMatch, sizeof(stMatch));

	//stMatch.m_lAdminCID = pcsAgpdCharacter->m_lID;
	//stMatch.m_lObjectCID = pstSearch->m_lObjectCID;
	//if(strlen(pstSearch->m_szSearchName) <= AGPACHARACTER_MAX_ID_STRING)
	//	strncpy(stMatch.m_szName, pstSearch->m_szSearchName, AGPACHARACTER_MAX_ID_STRING);
	
	// 이에 대한 콜백은 이미 등록해 놓는다.
	// AgsmAdminDB 에서 모든 작업이 성공하면 Callback 한번 불러준다.
	//m_pagsmAdminDB->SearchCharacter(&stMatch);

	return TRUE;
}

// AgsmAdminDB 를 통해서 불리게 된다. - EnumCallback 으로..
// DB 검색이 완전히 성공했을 때 불리게 되므로, 걱정 없이 정보를 날리면 된다.
// 뒤의 lCID 는 검색한 Admin 의 CID 가 오게 된다.
BOOL AgsmAdmin::SearchCharacterResultDB(CHAR* szName, INT32 lCID)
{
	if(!m_pagsmCharacter)
		return FALSE;

	AgpdCharacter* pcsAgpdCharacter = NULL;

	// 이름이 있다면 이름으로 검색
	if(szName)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(szName);

	// 못 찾으면 뷁 을 외친다.
	if(!pcsAgpdCharacter)
		return FALSE;

	DPNID dpnAdminID = m_pagsmCharacter->GetCharDPNID(lCID);

	m_pagsmCharacter->SendCharacterAllInfo(pcsAgpdCharacter, dpnAdminID);
	SendSearchResult(pcsAgpdCharacter, lCID, dpnAdminID);
	SendSearchCharDataSub(pcsAgpdCharacter, lCID, dpnAdminID);

	return TRUE;
}

// 2004.08.16.
// SearchCharacter 함수에서 Flag 를 보고 Account 검색이면 이 함수로 오게 된다.
BOOL AgsmAdmin::SearchCharacterByAccount(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;

	if(!pstSearch->m_szSearchName || strlen(pstSearch->m_szSearchName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_SEARCH_ACCOUNT, pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName);

	return TRUE;
}

// 2005.05.02. steeple
// Relay 쪽으로 바로 때려준다.
BOOL AgsmAdmin::SearchCharacterByAccount2(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;

	if(!pstSearch->m_szSearchName || strlen(pstSearch->m_szSearchName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT, pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName);

	return TRUE;
}

// 2005.05.18. steeple
// Relay 쪽으로 바로 때려준다.
BOOL AgsmAdmin::SearchCharacterDB(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;
	if(!pstSearch->m_szSearchName || strlen(pstSearch->m_szSearchName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_RELAY_SEARCH_CHARACTER, pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName);
	return TRUE;
}
// 2005.09.14. steeple
// Relay 족으로 바로 때려준다.
BOOL AgsmAdmin::SearchAccountBank(CHAR* szAccountName, AgpdCharacter* pcsAgpdCharacter)
{
	if(!szAccountName || !pcsAgpdCharacter)
		return FALSE;

	if(strlen(szAccountName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT_BANK, pcsAgpdCharacter->m_szID, szAccountName);

	return TRUE;
}

// 2005.05.02. steeple
// AgsmRelay 에서 불린다.
BOOL AgsmAdmin::ProcessSearchAccountDB(CHAR* szAdminCharName, CHAR* szAccountName, stAgpdAdminSearchResult* pstSearchResult)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szAccountName || !pstSearchResult)
		return FALSE;

	// 어드민을 찾고
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
	if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;

	if(m_pagpmCharacter->GetCharacter(pstSearchResult->m_szCharName))
		pstSearchResult->m_lStatus = 1;

	// 휴~ 커스텀 패킷 다 만들었다~~~
	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT, pstSearchResult, sizeof(stAgpdAdminSearchResult), lCID, ulNID);
	return TRUE;
}

// 2005.05.06. steeple
// AgsmRelay 에서 불린다.
BOOL AgsmAdmin::ProcessSearchCharacterDB(CHAR* szAdminCharName, CHAR* szCharName, stAgpdAdminSearchResult* pstSearchResult,
										stAgpdAdminCharDataBasic* pstBasic, stAgpdAdminCharDataStatus* pstStatus,
										stAgpdAdminCharDataStat* pstStat, stAgpdAdminCharDataMoney* pstMoney,
										CHAR* szTreeNode, CHAR* szProduct)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szCharName)
		return FALSE;

	// 어드민을 찾고
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
    if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;

	// 커스텀 패킷을 순서대로 보낸다. Result 만 나중에 보낸다.
	if(pstBasic)
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_BASIC, pstBasic, sizeof(stAgpdAdminCharDataBasic), lCID, ulNID);
	if(pstStatus)
	{
		pstStatus->m_llMaxExp = m_pagpmCharacter->GetLevelUpExp(pstStatus->m_lLevel);
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STATUS, pstStatus, sizeof(stAgpdAdminCharDataStatus), lCID, ulNID);
	}
	if(pstStat)
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STAT, pstStat, sizeof(stAgpdAdminCharDataStat), lCID, ulNID);
	if(pstMoney)
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_MONEY, pstMoney, sizeof(stAgpdAdminCharDataMoney), lCID, ulNID);

	if((szTreeNode && strlen(szTreeNode) > 0) || (szProduct && strlen(szProduct) > 0))
	{
		stAgpdAdminSkillString stSkill;
		memset(&stSkill, 0, sizeof(stSkill));
		_tcsncpy(stSkill.m_szCharName, szCharName, AGPDCHARACTER_NAME_LENGTH);
		_tcsncpy(stSkill.m_szTreeNode, szTreeNode, AGPMADMIN_MAX_SKILL_STRING_LENGTH);
		_tcsncpy(stSkill.m_szProduct, szProduct, AGPMADMIN_MAX_SKILL_STRING_LENGTH);

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SKILL, &stSkill, sizeof(stSkill), lCID, ulNID);
	}

	if(pstSearchResult)
	{
		stAgpdAdminCharDataSub stSub;
		memset(&stSub, 0, sizeof(stSub));

		strncpy(stSub.m_szAccName, pstSearchResult->m_szAccName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(stSub.m_szCharName, szCharName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(stSub.m_szLastIP, pstSearchResult->m_szIP, AGPMADMIN_MAX_IP_STRING);

		if(pstBasic)
			pstSearchResult->m_lTID = pstBasic->m_lTID;

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SUB, &stSub, sizeof(stSub), lCID, ulNID);
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT, pstSearchResult, sizeof(stAgpdAdminSearchResult), lCID, ulNID);

		// 현상수배 정보 보내준다. 2006.12.05. steeple
		SendCharWantedCriminal(szCharName, lCID, ulNID);
	}

	return TRUE;
}

// 2005.05.16. steeple
// AgsmRelay 에서 부른다.
// 2005.06.08. steeple
// Bank Item 이 날라올 때는 szTargetName 에 AccountName 이 온다.
BOOL AgsmAdmin::ProcessSearchCharItemDB(CHAR* szAdminCharName, CHAR* szTargetName, stAgpdAdminItem* pstItem)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szTargetName || !pstItem)
		return FALSE;

	// 어드민을 찾고
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT16 nAdminLevel = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
    if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		nAdminLevel = m_pagpmAdmin->GetAdminLevel(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;

	// 캐쉬 아이템일 때 레벨 5 가 되지 않으면 보내주지 않는다.
	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItem->m_lTID);
	if(pcsItemTemplate && IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && nAdminLevel < AGPMADMIN_LEVEL_4)
		return FALSE;

	// Custom Packet 보내준다.
	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, pstItem, sizeof(stAgpdAdminItem), lCID, ulNID);

	return TRUE;
}

BOOL AgsmAdmin::ProcessSearchCharTitleDB(CHAR* szAdminCharName, CHAR* szTargetName, stAgpdAdminCharTitle* pstTitle)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szTargetName || !pstTitle)
		return FALSE;

	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT16 nAdminLevel = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
    if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		nAdminLevel = m_pagpmAdmin->GetAdminLevel(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;


	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE, pstTitle, sizeof(stAgpdAdminCharTitle), lCID, ulNID);

	return TRUE;
}









//////////////////////////////////////////////////////////////////////////
// Character Operation Processing

// Move 처리
BOOL AgsmAdmin::ProcessCharacterMove(stAgpdAdminCharDataPosition* pstPosition, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstPosition || !pcsAgpdCharacter)
		return FALSE;

	if(!pstPosition->m_szName || strlen(pstPosition->m_szName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	if(m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) == FALSE)
		return FALSE;

	// 어드민 레벨 체크. 2006.01.11. steeple
	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
		return FALSE;

	AgpdCharacter* pcsMoveCharacter = NULL;
	if(strcmp(pcsAgpdCharacter->m_szID, pstPosition->m_szName) == 0)
		pcsMoveCharacter = pcsAgpdCharacter;
	else
		pcsMoveCharacter = m_pagpmCharacter->GetCharacterLock(pstPosition->m_szName);

	if(!pcsMoveCharacter)
	{
		// 서버에 없으면
		// 다른 서버에 있는 지 확인하고, 있으면 그쪽으로 이동 패킷을 날린다.
		// 이도저도 없으면, DB 로 바로 쓴다.
		//
		//
		//

		return TRUE;
	}

	AuPOS auPos;
	memset(&auPos, 0, sizeof(auPos));
	BOOL bValid = FALSE;
	
	if(strlen(pstPosition->m_szTargetName) > 0)
	{
		// 2004.01.16. Move 캐릭과, Target 캐릭은 달라야함
		if(strcmp(pcsMoveCharacter->m_szID, pstPosition->m_szTargetName) != 0)
		{
			// 2004.04.05. - Taget 캐릭이 Admin 이면 이미 Lock 이 되어 있는 상태이다. -0-
			// 서버가 락걸려서 알게 되었음.... TT
			AgpdCharacter* pcsTargetCharacter = NULL;
			if(strcmp(pcsAgpdCharacter->m_szID, pstPosition->m_szTargetName) == 0)
				pcsTargetCharacter = pcsAgpdCharacter;
			else
				pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(pstPosition->m_szTargetName);

			if(pcsTargetCharacter)
			{
				bValid = TRUE;

				auPos.x = pcsTargetCharacter->m_stPos.x;
				auPos.y = pcsTargetCharacter->m_stPos.y;
				auPos.z = pcsTargetCharacter->m_stPos.z;

				if(pcsTargetCharacter != pcsAgpdCharacter)
					pcsTargetCharacter->m_Mutex.Release();
			}
		}
	}
	else
	{
		bValid = TRUE;

		auPos.x = (FLOAT)pstPosition->m_fX;
		auPos.y = (FLOAT)pstPosition->m_fY;
		auPos.z = (FLOAT)pstPosition->m_fZ;
	}

	if(bValid && m_pagpmCharacter->UpdatePosition(pcsMoveCharacter, &auPos, TRUE, TRUE))
	{
		OutputDebugString("AgsmAdmin::ProcessCharacterMove(...) Move-Teleport Success!!!\n");
	}

	if(pcsMoveCharacter != pcsAgpdCharacter)
		pcsMoveCharacter->m_Mutex.Release();

	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
// Edit 처리
BOOL AgsmAdmin::ProcessCharacterEdit(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmFactors || !m_pagsmFactors || !m_pagsmDeath)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) == FALSE)
		return FALSE;

	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
		return FALSE;

	AgpdCharacter* pcsEditCharacter = NULL;
	if(strcmp(pcsAgpdCharacter->m_szID, pstCharEdit->m_szCharName) == 0)
		pcsEditCharacter = pcsAgpdCharacter;
	else
		pcsEditCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[CHARACTEREDIT] %s(%s) : Operation(%d), Target(%s), NewValueInt(%d), NewValueFloat(%f), NewValueINT64(%I64d)\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		pstCharEdit->m_lEditField,
		pstCharEdit->m_szCharName,
		pstCharEdit->m_lNewValueINT,
		pstCharEdit->m_lNewValueFloat,
		pstCharEdit->m_llNewValueINT64);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	
	if(!pcsEditCharacter)
	{
		// 서버에 없으면 DB 를 직접 수정한다.
		// 2005.05.31. steeple
		// 드디어 몇개 만들었다. -_-;;
		if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_DB || pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_BANK_MONEY_DB)
			return ProcessCharacterEditToDB(pstCharEdit, pcsAgpdCharacter);
		else if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_DB2)
			return ProcessCharacterEditToDB2(pstCharEdit, pcsAgpdCharacter);
		else if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_CHARISMA_POINT_DB)
			return ProcessCharismaEditToDB(pstCharEdit, pcsAgpdCharacter);
		else
			return FALSE;
	}

	// DB 에 직접 쓸 때는 문제가 좌표는 가능해야 하므로 레벨3 체크는 여기서 한다.
	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
		return FALSE;

	AgpdFactor* pcsResultFactor = (AgpdFactor*)m_pagpmFactors->GetFactor(&pcsEditCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsResultFactor)
	{
		if(pcsEditCharacter != pcsAgpdCharacter)
			pcsEditCharacter->m_Mutex.Release();

		return FALSE;
	}

	// Point
	AgpdFactorCharPoint* pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		if(pcsEditCharacter != pcsAgpdCharacter)
			pcsEditCharacter->m_Mutex.Release();

		return FALSE;
	}

	// Max Point
	AgpdFactorCharPoint* pcsFactorCharPointMax = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if(!pcsFactorCharPointMax)
	{
		if(pcsEditCharacter != pcsAgpdCharacter)
			pcsEditCharacter->m_Mutex.Release();

		return FALSE;
	}

	BOOL bResult = FALSE;
	PVOID pvFactorPacket = NULL;

	// Edit 항목 대로 잘 처리해야 한다. -0-
	switch(pstCharEdit->m_lEditField)
	{
		case AGPMADMIN_CHAREDIT_HP:	// HP 수정
		{
			INT32 lNewHP;
			if(pstCharEdit->m_lNewValueINT > pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP])
				lNewHP = pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP];
			else
				lNewHP = pstCharEdit->m_lNewValueINT;

			m_pagsmFactors->UpdateCharPoint(&pcsEditCharacter->m_csFactor, &pvFactorPacket, lNewHP, 0, 0);
			
			break;
		}

		case AGPMADMIN_CHAREDIT_MP:	// MP 수정
		{
			INT32 lNewMP;
			if(pstCharEdit->m_lNewValueINT > pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP])
				lNewMP = pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP];
			else
				lNewMP = pstCharEdit->m_lNewValueINT;

			m_pagsmFactors->UpdateCharPoint(&pcsEditCharacter->m_csFactor, &pvFactorPacket, 0, lNewMP, 0);

			break;
		}

		case AGPMADMIN_CHAREDIT_SP:	// SP 수정
		{
			INT32 lNewSP;
			if(pstCharEdit->m_lNewValueINT > pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP])
				lNewSP = pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP];
			else
				lNewSP = pstCharEdit->m_lNewValueINT;

			m_pagsmFactors->UpdateCharPoint(&pcsEditCharacter->m_csFactor, &pvFactorPacket, 0, 0, lNewSP);

			break;
		}

		case AGPMADMIN_CHAREDIT_LEVEL:	// Level 수정
		{
			INT32 lCurrentLevel = m_pagpmCharacter->GetLevel(pcsEditCharacter);
			INT32 lNewLevel = pstCharEdit->m_lNewValueINT;

			if(lNewLevel >= AGPMCHAR_MAX_LEVEL)
				lNewLevel = AGPMCHAR_MAX_LEVEL - 1;

			if(lNewLevel == lCurrentLevel)
				break;

			m_pagsmCharacter->SetCharacterLevel(pcsEditCharacter, lNewLevel, FALSE);
			WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITLEVEL, pcsAgpdCharacter, pcsEditCharacter);
			break;
		}

		case AGPMADMIN_CHAREDIT_EXP:	// EXP 수정
			bResult = m_pagsmDeath->AddBonusExpToChar(pcsEditCharacter, NULL, pstCharEdit->m_llNewValueExp);
			WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITEXP, pcsAgpdCharacter, pcsEditCharacter);
			break;

		case AGPMADMIN_CHAREDIT_MONEY_INV:	// Inventory 에 있는 Ghelld 수정
			bResult = m_pagpmCharacter->SetMoney(pcsEditCharacter, pstCharEdit->m_llNewValueINT64);
			WriteLog_Gheld(AGPDLOGTYPE_GHELD_GMEDIT, pcsAgpdCharacter, pcsEditCharacter);
			break;
		case AGPMADMIN_CHAREDIT_MONEY_BANK:
			bResult = m_pagpmCharacter->SetBankMoney(pcsEditCharacter, pstCharEdit->m_llNewValueINT64);
			WriteLog_Gheld(AGPDLOGTYPE_GHELD_GMEDIT, pcsAgpdCharacter, pcsEditCharacter);
			break;
		case AGPMADMIN_CHAREDIT_CRIMINAL_POINT:	// Criminal Point 수정.
			m_pagpmCharacter->UpdateCriminalStatus(pcsEditCharacter, (AgpdCharacterCriminalStatus)pstCharEdit->m_lNewValueINT);
			break;

		case AGPMADMIN_CHAREDIT_MURDERER_POINT:	// Murderer Point 수정.
			bResult = m_pagpmCharacter->UpdateMurdererPoint(pcsEditCharacter, pstCharEdit->m_lNewValueINT);
			if(bResult && m_pagsmRemission)
			{
				// 결과를 보내준다.
				m_pagsmRemission->SendRemitResult(pcsEditCharacter, AGPMREMISSION_TYPE_INDULGENCE, AGPMREMISSION_RESULT_SUCCESS);
			}
			// 로그 남긴다.
			break;

		case AGPMADMIN_CHAREDIT_CRIMINAL_TIME:	// 남은 Criminal Time 수정.
			if(pstCharEdit->m_lNewValueINT > 0 && pstCharEdit->m_lNewValueINT < 7 * 24 * 60)
			{
				pcsEditCharacter->m_unCriminalStatus = AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED;
				m_pagpmCharacter->SetLastCriminalTime(pcsEditCharacter, pstCharEdit->m_lNewValueINT * 60);
				m_pagsmCharacter->SendPacketNearCharCriminalStatus(pcsEditCharacter);
			}
			else
			{
				bResult = m_pagpmCharacter->UpdateCriminalStatus(pcsEditCharacter, AGPDCHAR_CRIMINAL_STATUS_INNOCENT);
				pcsEditCharacter->m_lLastUpdateCriminalFlagTime = 0;
				m_pagsmCharacter->SendPacketNearCharCriminalStatus(pcsEditCharacter);
			}
				
			// 로그 남긴다.
			break;
		
		case AGPMADMIN_CHAREDIT_MURDERER_TIME:	// 남은 Murderer Time 수정.
			if(pstCharEdit->m_lNewValueINT > 0)
			{
				// pstCharEdit->m_lNewValueINT 의 값은 분으로 넘어온다.
				m_pagpmCharacter->SetLastMurdererTime(pcsEditCharacter, pstCharEdit->m_lNewValueINT * 60);
				m_pagsmCharacter->SendPacketMurdererPoint(pcsEditCharacter);
				bResult = TRUE;
			}
			else
			{
				bResult = m_pagpmCharacter->UpdateMurdererPoint(pcsEditCharacter, 0);
			}
			
			break;
		case AGPMADMIN_CHAREDIT_CHARISMA_POINT:
			{
				bResult = m_pagpmCharacter->UpdateCharismaPoint(pcsEditCharacter, pstCharEdit->m_lNewValueINT);

				m_pagpmLog->WriteLog_CharismaUpByGm(0,
					&pcsAgsdCharacter->m_strIPAddress[0],
					pcsAgsdCharacter->m_szAccountID,
					pcsAgsdCharacter->m_szServerName,
					pcsEditCharacter->m_szID,
					((AgpdCharacterTemplate *)(pcsEditCharacter->m_pcsCharacterTemplate))->m_lID,
					m_pagpmCharacter->GetLevel(pcsEditCharacter),
					m_pagpmCharacter->GetExp(pcsEditCharacter),
					pcsEditCharacter->m_llMoney,
					pcsEditCharacter->m_llBankMoney,
					pstCharEdit->m_lNewValueINT,
					NULL);
			}
			break;
	}

	// FactorPacket 을 보낸다.
	if(pvFactorPacket)
	{
		bResult = m_pagsmCharacter->SendPacketFactor(pvFactorPacket, pcsEditCharacter, PACKET_PRIORITY_2);
		m_pagpmCharacter->m_csPacket.FreePacket(pvFactorPacket);
	}

	// Edit 에 성공했다고 보면 된다.
	if(bResult)
	{
		// 결과를 성공으로 세팅한다.
		pstCharEdit->m_lEditResult = 1;

		// 기타 처리를 한다.
		//
		//
		//

	}

	if(pcsEditCharacter != pcsAgpdCharacter)
		pcsEditCharacter->m_Mutex.Release();

	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	
	return TRUE;
}

// 2005.05.31. steeple
// 여기는 Level, Exp, InvenMoney, Position 관련 업데이트다.
// 뒤에 오는 놈은 어드민이다.
BOOL AgsmAdmin::ProcessCharacterEditToDB(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	// DB 에 바로 쓰는 것이다.
	// 각 변수에 특정 값들이 날라온다.
	//
	// lCID 에는 TID
	// NewValueINT 에는 Level
	// NewValueExp 에는 Exp
	// NewValueINT64 에는 InvenMoney
	// NewValueChar 에는 Position 이 온다.

	if(pstCharEdit->m_lNewValueINT < 1  || pstCharEdit->m_lNewValueINT >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(pstCharEdit->m_lCID);
	if(!pcsTemplate)
		return FALSE;

	// Exp 검사는 하지 않는다.
	//INT32 lMaxExp = 0;
	//m_pagpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[pstCharEdit->m_lNewValueINT], &lMaxExp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP);
	//if((INT32)pstCharEdit->m_lNewValueFloat >= lMaxExp)
	//	return FALSE;

	if(pstCharEdit->m_llNewValueINT64 > (LONG)AGPDCHARACTER_MAX_INVEN_MONEY)
		return FALSE;

	if(strlen(pstCharEdit->m_szNewValueChar) >= 64)
		return FALSE;

	WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITDB, pcsAgpdCharacter, pstCharEdit->m_szCharName, pstCharEdit->m_lCID,
					pstCharEdit->m_lNewValueINT, pstCharEdit->m_llNewValueExp, pstCharEdit->m_llNewValueINT64);
	if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_DB)
		EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARACTER, pstCharEdit, pcsAgpdCharacter);
	else if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_BANK_MONEY_DB)
		EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_BANK_MONEY, pstCharEdit, pcsAgpdCharacter);
		

	return TRUE;
}

// 2006.02.06. steeple
// 여기는 Criminal, Murderer 관련 Update 다.
// 뒤에 오는 놈은 어드민이다.
BOOL AgsmAdmin::ProcessCharacterEditToDB2(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	// DB 에 바로 쓰는 것이다.
	// 각 변수에 특정 값들이 날라온다.
	//
	// lCID 에는 TID
	// NewValueINT 에는 CriminalPoint
	// NewValueFloat 에는 MurdererPoint
	// NewValueINT64 에는 RemainedCriminalTime
	// NewValueCharLength 에는 RemainedMurdererTime

	// 다들 0 보다 작으면 이상한 것이3.
	if(pstCharEdit->m_lNewValueINT < 0 || pstCharEdit->m_lNewValueFloat < 0.0f ||
		pstCharEdit->m_llNewValueINT64 < 0L || pstCharEdit->m_lNewValueCharLength < 0)
		return FALSE;

	// 시간은 엄하게 일주일 이상 세팅하는 일이 없도록 하자.
	// 시간은 분단위로 날아온다.
	INT32 lOneWeekPerMin = 7 * 24 * 60;
	if((INT32)pstCharEdit->m_llNewValueINT64 > lOneWeekPerMin ||
		pstCharEdit->m_lNewValueCharLength > lOneWeekPerMin)
		return FALSE;

	//WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITDB, pcsAgpdCharacter, pstCharEdit->m_szCharName, pstCharEdit->m_lCID,
	//				pstCharEdit->m_lNewValueINT, pstCharEdit->m_lNewValueFloat, pstCharEdit->m_llNewValueINT64);

	EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARACTER2, pstCharEdit, pcsAgpdCharacter);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCharismaEditToDB(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(pstCharEdit->m_lNewValueINT < 1)
		pstCharEdit->m_lNewValueINT = 0;

	EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARISMA_POINT, pstCharEdit, pcsAgpdCharacter);

	return TRUE;
}

// 2006.12.01. steeple
// 현상수배 지워주기
BOOL AgsmAdmin::ProcessWCEdit(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmWantedCriminal || !m_pagsmWantedCriminal)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(_tcslen(pstCharEdit->m_szCharName) == 0)
		return FALSE;

	pstCharEdit->m_lEditResult = (INT8)m_pagsmWantedCriminal->DeleteWCWithNoEffect(pstCharEdit->m_szCharName);

	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
// Ban 처리
BOOL AgsmAdmin::ProcessBan(stAgpdAdminBan* pstBan, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmAdmin || !m_pagpmCharacter)
		return FALSE;

	if(!pstBan || !pcsAgpdCharacter)
		return FALSE;

	// Relay Server 라면 DB 저장만 한다. - 2004.03.30.
	if(IsRelayServer())
	{
		ProcessBanUpdateDB(pstBan);
		return TRUE;
	}

	AgpdCharacter* pcsBanCharacter = NULL;
	if(strcmp(pstBan->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsBanCharacter = pcsAgpdCharacter;
	else
		pcsBanCharacter = m_pagpmCharacter->GetCharacterLock(pstBan->m_szCharName);

	if(!pcsBanCharacter)
	{
		OutputDebugString("AgsmAdmin::ProcessBan(...) pcsBanCharacter is NULL\n");

		// DB 에서 읽어온다.
		//
		//
		
		return FALSE;
	}

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(pcsAgsdCharacter)
	{
		if(pcsAgsdCharacter->m_ulServerID != GetThisServerID())
		{
			SendBanData(pstBan, pcsAgpdCharacter->m_lID, GetGameServerNID(pcsAgsdCharacter->m_ulServerID));
			
			if(pcsBanCharacter != pcsAgpdCharacter)
				pcsBanCharacter->m_Mutex.Release();

			return TRUE;
		}
	}
	

	// 인자로 넘어온 pstBan 을 분석해야 된다.
	
	// 1. Logout 시키기.
	if(pstBan->m_lLogout == 1)
	{
		INT32 lCID = pcsBanCharacter->m_lID;
		DPNID dpnID = m_pagsmCharacter->GetCharDPNID(pcsBanCharacter);

		if(pcsBanCharacter != pcsAgpdCharacter)
			pcsBanCharacter->m_Mutex.Release();

		DestroyClient(dpnID);
		//m_pagsmCharManager->OnDisconnect(lCID, dpnID);
		return TRUE;
	}

	// 기존의 Ban 상태를 점검한다. - 2004.04.01.
	ProcessBanReCalcTime(pcsBanCharacter);
	
	// 2. Ban 적용
	if(pstBan->m_lBanFlag)	// 변한게 있다면~
	{
		ProcessBanApplyCharacter(pstBan, pcsBanCharacter);
	}

	// 기본 정보 요청 또는 변화 적용을 한 후 이리로 온다.
	stAgpdAdminBan stNewBan;
	memset(&stNewBan, 0, sizeof(stNewBan));

	// Ban 데이터를 얻는다.
	ProcessBanGetData(&stNewBan, pcsBanCharacter);

	// 변한게 있다면 새로 보내주고 DB 저장을 한다.
	if(pstBan->m_lBanFlag)
	{
		// 해당 캐릭터에게 변경 사항을 보낸다.
		//m_pagpmCharacter->SendBanData(pcsBanCharacter);

		// DB 저장을 한다. - Relay Server 로 보낸다.
		SendBanData(&stNewBan, pcsAgpdCharacter->m_lID, GetRelayServerNID());
	}

	if(pcsBanCharacter != pcsAgpdCharacter)
		pcsBanCharacter->m_Mutex.Release();

	// 운영자에게도 보낸다.
	SendBanData(&stNewBan, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

BOOL AgsmAdmin::ProcessBanApplyCharacter(stAgpdAdminBan* pstBan, AgpdCharacter* pcsBanCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstBan || !pcsBanCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;
	
	// 현재 시간을 얻는다.
	INT32 lCurrentTime = m_pagpmAdmin->GetCurrentTimeStamp();

	if(pstBan->m_lBanFlag & AGPMADMIN_BAN_FLAG_CHAT)	// Chat Ban
	{
		pcsAgsdCharacter->m_stBan.m_lChatBanStartTime = pstBan->m_lChatBanKeepTime != 0 ? lCurrentTime : 0;
		pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime = pstBan->m_lChatBanKeepTime;
	}

	if(pstBan->m_lBanFlag & AGPMADMIN_BAN_FLAG_CHAR)	// Char Ban
	{
		pcsAgsdCharacter->m_stBan.m_lCharBanStartTime = pstBan->m_lCharBanKeepTime != 0 ? lCurrentTime : 0;
		pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime = pstBan->m_lCharBanKeepTime;
	}

	if(pstBan->m_lBanFlag & AGPMADMIN_BAN_FLAG_ACC)		// Acc Ban
	{
		pcsAgsdCharacter->m_stBan.m_lAccBanStartTime = pstBan->m_lAccBanKeepTime != 0 ? lCurrentTime : 0;
		pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime = pstBan->m_lAccBanKeepTime;
	}

	return TRUE;
}

// pcsAgpdCharacter 의 Ban 내용을 pstBan 에 넣는다.
BOOL AgsmAdmin::ProcessBanGetData(stAgpdAdminBan* pstBan, AgpdCharacter* pcsBanCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstBan || !pcsBanCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;
	
	ZeroMemory(pstBan->m_szCharName, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	ZeroMemory(pstBan->m_szAccName, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));

	strncpy(pstBan->m_szCharName, pcsBanCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(pstBan->m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);

	// Chat Ban
	pstBan->m_lBanFlag |= pcsAgsdCharacter->m_stBan.m_lChatBanStartTime ? AGPMADMIN_BAN_FLAG_CHAT : 0;
	pstBan->m_lChatBanStartTime = pcsAgsdCharacter->m_stBan.m_lChatBanStartTime;
	pstBan->m_lChatBanKeepTime = pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime;

	// Char Ban
	pstBan->m_lBanFlag |= pcsAgsdCharacter->m_stBan.m_lCharBanStartTime ? AGPMADMIN_BAN_FLAG_CHAR : 0;
	pstBan->m_lCharBanStartTime = pcsAgsdCharacter->m_stBan.m_lCharBanStartTime;
	pstBan->m_lCharBanKeepTime = pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime;

	// Acc 도 얻어야 하는데..... 2004.05.18. 작업
	pstBan->m_lBanFlag |= pcsAgsdCharacter->m_stBan.m_lAccBanStartTime ? AGPMADMIN_BAN_FLAG_ACC : 0;
	pstBan->m_lAccBanStartTime = pcsAgsdCharacter->m_stBan.m_lAccBanStartTime;
	pstBan->m_lAccBanKeepTime = pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime;

	return TRUE;
}

// pcsAgpdCharacter 의 Ban 내용을 검색할 때, 시간이 완료된 것은 초기화 한다.
BOOL AgsmAdmin::ProcessBanReCalcTime(AgpdCharacter* pcsBanCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pcsBanCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT32 lCurrentTimeStamp = m_pagpmAdmin->GetCurrentTimeStamp();
	INT32 lKeepTime = 0;

	if(pcsAgsdCharacter->m_stBan.m_lChatBanStartTime != 0)
	{
		lKeepTime = ProcessBanGetKeepTime(pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime);
		if(lKeepTime != -1 && (lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lChatBanStartTime) > lKeepTime)
		{
			pcsAgsdCharacter->m_stBan.m_lChatBanStartTime = 0;
			pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime = 0;
		}
	}

	if(pcsAgsdCharacter->m_stBan.m_lCharBanStartTime != 0)
	{
		lKeepTime = ProcessBanGetKeepTime(pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime);
		if(lKeepTime != -1 && (lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lCharBanStartTime) > lKeepTime)
		{
			pcsAgsdCharacter->m_stBan.m_lCharBanStartTime = 0;
			pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime = 0;
		}
	}

	if(pcsAgsdCharacter->m_stBan.m_lAccBanStartTime != 0)
	{
		lKeepTime = ProcessBanGetKeepTime(pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime);
		if(lKeepTime != -1 && (lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lAccBanStartTime) > lKeepTime)
		{
			pcsAgsdCharacter->m_stBan.m_lAccBanStartTime = 0;
			pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime = 0;
		}
	}

	return TRUE;
}

// Define 되어 있는 KeepTime 을 초단위로 바꿔준다.
INT32 AgsmAdmin::ProcessBanGetKeepTime(INT8 cKeepTime)
{
	INT32 lReturn = 0;
	switch(cKeepTime)
	{
		case AGSDCHAR_BAN_KEEPTIME_5H:
			lReturn = (60 * 60) * 5;
			break;

		case AGSDCHAR_BAN_KEEPTIME_1D:
			lReturn = 60 * 60 * 24;
			break;

		case AGSDCHAR_BAN_KEEPTIME_5D:
			lReturn = (60 * 60 * 24) * 5;
			break;

		case AGSDCHAR_BAN_KEEPTIME_10D:
			lReturn = (60 * 60 * 24) * 10;
			break;

		case AGSDCHAR_BAN_KEEPTIME_30D:
			lReturn = (60 * 60 * 24) * 30;
			break;

		case AGSDCHAR_BAN_KEEPTIME_90D:
			lReturn = (60 * 60 * 24) * 90;
			break;

		case AGSDCHAR_BAN_KEEPTIME_UNLIMIT:
			lReturn = -1;
			break;
	}

	return lReturn;
}

BOOL AgsmAdmin::ProcessBanUpdateDB(stAgpdAdminBan* pstBan)
{
	if(!pstBan)
		return FALSE;

	// Relay Server 일 때만 한다.
	if(!IsRelayServer())
		return FALSE;

	EnumCallback(AGSMADMIN_CB_BAN_UPDATE_DB, pstBan, NULL);

	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
// Send Packet - Public
// 2004.03.1
BOOL AgsmAdmin::SendSearchOtherServer(stAgpdAdminSearch* pstSearch, INT32 lCID, UINT32 ulNID)
{
	if(!pstSearch || !lCID || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvSearchPacket = NULL;
	INT16 nPacketLength = 0;

	pvSearchPacket = m_pagpmAdmin->MakeSearchPacket(&nPacketLength, 
															&pstSearch->m_iType,
															&pstSearch->m_iField,
															&pstSearch->m_lObjectCID,
															pstSearch->m_szSearchName,
															NULL);

	if(!pvSearchPacket)
	{
		OutputDebugString("AgsmAdmin::SendSearchOtherServer(...) pvSearchPakcet is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeSearchPacket(&nPacketLength, &lCID, pvSearchPacket);

	m_pagpmAdmin->m_csSearchPacket.FreePacket(pvSearchPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendSearchOtherServer(...) pvPakcet is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmAdmin::SendSearchResult(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmFactors || !m_pagpmGuild || !m_pagsmCharacter)
		return FALSE;

	if(!pcsAgpdCharacter || !ulNID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// 2005.09.14. steeple
	// Server Name 추가 하면서 걍 편하게 Custom Packet 으로 변경.
	stAgpdAdminSearchResult stSearchResult;
	memset(&stSearchResult, 0, sizeof(stSearchResult));

	strncpy(stSearchResult.m_szCharName, pcsAgpdCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(stSearchResult.m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(stSearchResult.m_szGuildID,
				m_pagpmGuild->GetJoinedGuildID(pcsAgpdCharacter) ? m_pagpmGuild->GetJoinedGuildID(pcsAgpdCharacter) : _T(""),
				AGPACHARACTER_MAX_ID_STRING);

	stSearchResult.m_lTID = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID;
	m_pagpmFactors->GetValue(&pcsAgpdCharacter->m_csFactor, &stSearchResult.m_lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	stSearchResult.m_lStatus = 1;	// 캐릭터가 있다는 것은 온라인이란 소리.

	strcpy(stSearchResult.m_szIP, &pcsAgsdCharacter->m_strIPAddress[0]);
	
	strncpy(stSearchResult.m_szServerName, pcsAgsdCharacter->m_szServerName, AGPMADMIN_MAX_SERVERNAME_LENGTH);

	return SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT, &stSearchResult, sizeof(stSearchResult), lCID, ulNID);
}

// AgsmRelay 에서 부른다.
// 보통 Account 검색을 했을 때 이렇게 불리게 된다.
// 접속해 있지 않은 캐릭터 검색일때도 불릴 수 있다.
//
// DB 검색이다 보니, lRace 에 CharTID 가 온다.
BOOL AgsmAdmin::SendSearchResult(CHAR* szAdminName, CHAR* szCharName, CHAR* szAccName, INT32 lLevel,
								 INT32 lTID, INT32 lStatus, CHAR* szCreationDate)
{
	// 2005.09.14.
	// 살펴보니 이 함수 이제 안쓴다. 퀠퀠퀠.
	return TRUE;
}


// dpnAdminID 에게 pcsAgpdCharacter 의 정보를 보낸다.
BOOL AgsmAdmin::SendSearchCharDataSub(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pcsAgpdCharacter || !ulNID)
		return FALSE;

	// 추가정보를 보낸다.
	PVOID pvPacket = NULL;
	PVOID pvSearchCharDataPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARDATA;

	stAgpdAdminCharDataSub stCharDataSub;
	memset(&stCharDataSub, 0, sizeof(stCharDataSub));

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(pcsAgsdCharacter)
	{
		strncpy(stCharDataSub.m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
		stCharDataSub.m_ulConnectedTimeStamp = pcsAgsdCharacter->m_ulConnectedTimeStamp;
	}

	// IP
	CHAR* szIP = GetPlayerIPAddress(m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	if(szIP && strlen(szIP) <= AGPMADMIN_MAX_IP_STRING)
		strncpy(stCharDataSub.m_szIP, szIP, AGPMADMIN_MAX_IP_STRING);

	pvSearchCharDataPacket = m_pagpmAdmin->MakeCharDataSubPacket(&nPacketLength,
															&pcsAgpdCharacter->m_lID,
															pcsAgpdCharacter->m_szID,
															stCharDataSub.m_szAccName,
															stCharDataSub.m_ulConnectedTimeStamp,
															stCharDataSub.m_szIP,
															stCharDataSub.m_szLastIP);

	if(!pvSearchCharDataPacket)
	{
		OutputDebugString("AgsmAdmin::SearchCharacter(...) pvSearchDataPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeCharDataSubPacket(&nPacketLength, &lCID, pvSearchCharDataPacket);

	m_pagpmAdmin->m_csSearchCharDataPacket.FreePacket(pvSearchCharDataPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SearchCharacter(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

// 2005.05.18. steeple
BOOL AgsmAdmin::SendSearchCharMoney(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsAgpdCharacter || !lCID || !ulNID)
		return FALSE;

	stAgpdAdminCharDataMoney stMoney;
	memset(&stMoney, 0, sizeof(stMoney));

	strcpy(stMoney.m_szCharName, pcsAgpdCharacter->m_szID);
	m_pagpmCharacter->GetMoney(pcsAgpdCharacter, &stMoney.m_llInvenMoney);
	stMoney.m_llBankMoney = m_pagpmCharacter->GetBankMoney(pcsAgpdCharacter);

	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_MONEY, &stMoney, sizeof(stMoney), lCID, ulNID);
	return TRUE;
}

// 2006.12.05. steeple
BOOL AgsmAdmin::SendCharWantedCriminal(CHAR* szCharName, INT32 lCID, UINT32 ulNID)
{
	if(!szCharName || !ulNID)
		return FALSE;

	if(!m_pagpmWantedCriminal || !m_pagsmWantedCriminal)
		return FALSE;

	AgpdWantedCriminal *pAgpdWantedCriminal = m_pagpmWantedCriminal->GetWantedCriminal((TCHAR*)szCharName);
	if(pAgpdWantedCriminal)
	{
		// Money 에다가 보낸다. -_-
		stAgpdAdminCharDataMoney stWantedCriminal;
		_tcscpy(stWantedCriminal.m_szCharName, pAgpdWantedCriminal->m_szCharID);
		stWantedCriminal.m_llInvenMoney = pAgpdWantedCriminal->m_llMoney;
		stWantedCriminal.m_llBankMoney = m_pagsmWantedCriminal->GetRank(szCharName);

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_WC, &stWantedCriminal, sizeof(stWantedCriminal), lCID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::SendCharQuestInfo(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsCharacter || !ulNID)
		return FALSE;

	if(!m_pagsmQuest)
		return FALSE;

	AgpdQuest* pAgpdQuest = m_pagsmQuest->m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);

	if (!pAgpdQuest)
		return FALSE;

	for( AgpdQuest::IterQuest iter = pAgpdQuest->m_vtQuest.begin(); iter != pAgpdQuest->m_vtQuest.end(); ++iter)
	{
		stAgpdAdminCharQuest stQuest;
		stQuest.lQuestID = (*iter).lQuestID;
		stQuest.lParam1 = (*iter).lParam1;
		stQuest.lParam2 = (*iter).lParam2;
		_tcscpy(stQuest.szNPCName, (*iter).szNPCName);

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_QUEST, &stQuest, sizeof(stQuest), lCID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::SendCharTitleInfo(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsCharacter || !ulNID)
		return FALSE;

	if(!m_pagsmTitle)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	for(IterTitle iter = pcsCharacter->m_csTitle->m_vtTitle.begin(); iter != pcsCharacter->m_csTitle->m_vtTitle.end(); ++iter)
	{
		stAgpdAdminCharTitle stTitle;
		strncpy(stTitle.m_szCharName,pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
		stTitle.lTitleTid = (*iter).lTitleID;
		stTitle.bIsTitle = TRUE;
		stTitle.bUseTitle = (*iter).bUsingTitle;

		for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
			stTitle.CheckValue[i] = 0;

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE, &stTitle, sizeof(stTitle), lCID, ulNID);
	}

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		stAgpdAdminCharTitle stTitle;
		strncpy(stTitle.m_szCharName,pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
		stTitle.lTitleTid = (*iter).m_nTitleTid;
		stTitle.bIsTitle = FALSE;
		stTitle.bUseTitle = FALSE;

		INT32 i = 0;
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			stTitle.CheckValue[i] = (*iter2).m_nTitleCurrentValue;
			i++;
		}

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE, &stTitle, sizeof(stTitle), lCID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::SendSearchCharItem(stAgpdAdminCharItem* pstItem, AgpdCharacter* pcsAdminCharacter)
{
	if(!m_pagpmItem || !m_pagsmItem || !m_pagsmCharacter)
		return FALSE;

	// 검색할 캐릭터를 찾는다.
	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstItem->m_lCID != 0)
	{
		if(pstItem->m_lCID == pcsAdminCharacter->m_lID)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstItem->m_lCID);
	}
	else
	{
		if(strcmp(pstItem->m_szCharName, pcsAdminCharacter->m_szID) == 0)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstItem->m_szCharName);
	}

	if(!pcsSearchCharacter)
	{
		// DB 검색을 해본다.
		//SearchCharacterFromDB(pstItem->m_szCharName, pstItem->m_lCID, pcsAdminCharacter);
		return TRUE;
	}

	BOOL bResult = m_pagsmItem->SendPacketItemAll(pcsSearchCharacter, m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter));

	if(pcsSearchCharacter != pcsAdminCharacter)
		pcsSearchCharacter->m_Mutex.Release();
	
	return bResult;
}

// 보통, 이렇게 불리게 되는 건 AgsmAdminDB 를 통해서이다.
BOOL AgsmAdmin::SendSearchCharItem(AgpdItem* pcsAgpdItem, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagsmItem)
		return FALSE;

	if(lCID == 0 || ulNID == 0 || !pcsAgpdItem)
		return FALSE;

	BOOL bResult = m_pagsmItem->SendPacketItem(pcsAgpdItem, ulNID);
	return bResult;
}

// 2005.05.18. steeple
// 이전까지만 해도 Item 정보를 보낼 때 그냥 Add 로 해서 처리하였으나
// 그냥 Add 할 경우 복사, 중복 등 여러가지 문제로 인해서 Add 쪽 루틴이 타이트해졌다.
// 그로 인해서 그냥 Add 할 경우 어드민 클라이언트에서 콜백을 못 받는 경우가 생김.
// 차라리 Custom Packet 으로 보내서 처리하는 게 나을 거라고 판단해서 이렇게 바꿈.
BOOL AgsmAdmin::SendCharAllItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	SendCharEquipItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharInventoryItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharBankItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharSalesBoxItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharMailBoxItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharPetInventoryItemUseCustom(pcsCharacter, lCID, ulNID);

	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacter(lCID);
	if(pcsAdminCharacter && m_pagpmAdmin->GetAdminLevel(pcsAdminCharacter) >= AGPMADMIN_LEVEL_4)
		SendCharCashItemUseCustom(pcsCharacter, lCID, ulNID);

	return TRUE;
}

// 2005.05.18. steeple
// Equip Item 을 보내준다.
BOOL AgsmAdmin::SendCharEquipItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	AgpdItem* pcsItem = NULL;

	stAgpdAdminItem stItem;
	memset(&stItem, 0, sizeof(stItem));

	for(INT32 lPart = AGPMITEM_PART_NONE + 1; lPart < AGPMITEM_PART_NUM; lPart++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem(pcsCharacter, lPart);
		if(pcsGridItem)
		{
			pcsItem = m_pagpmItem->GetItem(pcsGridItem);
			if(pcsItem && pcsItem->m_pcsItemTemplate)
			{
				memset(&stItem, 0, sizeof(stItem));
				ConvertItemData(pcsItem, &stItem);
				SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, (PVOID)&stItem, sizeof(stItem), lCID, ulNID);
			}
		}
	}

	return TRUE;
}

// 2005.05.18. steeple
// Inventory Item 을 보내준다.
BOOL AgsmAdmin::SendCharInventoryItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetInventory(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2005.05.18. steeple
// Bank Item 을 보내준다.
BOOL AgsmAdmin::SendCharBankItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetBank(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2005.05.18. steeple
// Sales Box Item 을 보내준다.
BOOL AgsmAdmin::SendCharSalesBoxItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetSalesBox(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2009.06.17. shootingstart
// Pet Inventory Item 을 보낸다.
BOOL AgsmAdmin::SendCharPetInventoryItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetSubInventory(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2007.03.19. steeple
// Mail Box Item 을 보낸다.
BOOL AgsmAdmin::SendCharMailBoxItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem || !m_pagpmMailBox || !m_pagsmCharacter)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgpdMailCAD* pcsMailCAD = m_pagpmMailBox->GetCAD((ApBase*)pcsCharacter);
	if(!pcsMailCAD)
		return FALSE;

	for(int i = 0; i < pcsMailCAD->GetCount(); ++i)
	{
		INT32 lMailID = pcsMailCAD->Get(i);
		AgpdMail* pcsMail = m_pagpmMailBox->GetMail(lMailID);
		if(pcsMail)
		{
			// 얘는 AgpdItem 객체가 없어서 아래와 같이 몇가지 정보만 보내줘야 한다.
			stAgpdAdminItem stItem;
			memset(&stItem, 0, sizeof(stItem));

			_tcsncpy(stItem.m_szCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
			_tcsncpy(stItem.m_szAccountName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
			_tcsncpy(stItem.m_szServerName, pcsAgsdCharacter->m_szServerName, AGPMADMIN_MAX_SERVERNAME_LENGTH);

			stItem.m_lTID = pcsMail->m_lItemTID;
			stItem.m_ullDBID = pcsMail->m_ullItemSeq;

			stItem.m_lPos = (INT32)AGPDITEM_STATUS_MAILBOX;
			stItem.m_lLayer = 0;
			stItem.m_lRow = i;		// Row 를 숫자로 준다.
			stItem.m_lCol = 0;
			stItem.m_lCount = pcsMail->m_nItemQty;

			// 보낸다.
			SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, (PVOID)&stItem, sizeof(stItem), lCID, ulNID);
		}
	}

	return TRUE;
}

// 2005.12.29. steeple
// Cash Inventory 를 보낸다.
BOOL AgsmAdmin::SendCharCashItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetCashInventoryGrid(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2005.05.18. steeple
BOOL AgsmAdmin::SendGridItemUseCustom(AgpdGrid* pcsGrid, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsGrid)
		return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	AgpdItem* pcsItem = NULL;

	stAgpdAdminItem stItem;
	memset(&stItem, 0, sizeof(stItem));

    for(INT32 lLayer = 0; lLayer < pcsGrid->m_nLayer; lLayer++)
	{
		for(INT32 lRow = 0; lRow < pcsGrid->m_nRow; lRow++)
		{
			for(INT32 lColumn = 0; lColumn < pcsGrid->m_nColumn; lColumn++)
			{
				pcsGridItem = m_pagpmGrid->GetItem(pcsGrid, lLayer, lRow, lColumn);
				if(pcsGridItem)
				{
					pcsItem	= m_pagpmItem->GetItem(pcsGridItem);
					if(pcsItem && pcsItem->m_pcsItemTemplate)
					{
						memset(&stItem, 0, sizeof(stItem));
						ConvertItemData(pcsItem, &stItem);
						SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, (PVOID)&stItem, sizeof(stItem), lCID, ulNID);
					}	// pcsItem
				}	// pcsGridItem
			}
		}
	}

	return TRUE;
}

BOOL AgsmAdmin::SendSearchCharSkill(stAgpdAdminCharSkill* pstSkill, AgpdCharacter* pcsAdminCharacter)
{
	if(!m_pagsmCharacter || !m_pagsmSkill)
		return FALSE;
	
	// 검색할 캐릭터를 찾는다.
	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstSkill->m_lCID!= 0)
	{
		if(pstSkill->m_lCID == pcsAdminCharacter->m_lID)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSkill->m_lCID);
	}
	else
	{
		if(strcmp(pstSkill->m_szCharName, pcsAdminCharacter->m_szID) == 0)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSkill->m_szCharName);
	}

	if(!pcsSearchCharacter)
	{
		// DB 검색을 해본다.
		//SearchCharacterFromDB(pstSkill->m_szCharName, pstSkill->m_lCID, pcsAdminCharacter);
		return TRUE;
	}

	// 몽땅 다 보내버린다. 푸하하하하!!!
	BOOL bResult = m_pagsmSkill->SendSkillAll(pcsSearchCharacter, m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter));

	if(pcsSearchCharacter != pcsAdminCharacter)
		pcsSearchCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgsmAdmin::SendSearchCharParty(stAgpdAdminCharParty* pstParty, AgpdCharacter* pcsAdminCharacter)
{
	if(!m_pagpmCharacter || !m_pagsmCharacter || !m_pagpmParty || !m_pagsmParty)
		return FALSE;

	// 검색할 캐릭터를 찾는다.
	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstParty->m_lCID != 0)
	{
		if(pstParty->m_lCID == pcsAdminCharacter->m_lID)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstParty->m_lCID);
	}
	else
	{
		if(strcmp(pstParty->m_szCharName, pcsAdminCharacter->m_szID) == 0)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstParty->m_szCharName);
	}

	// Party 는 못 구하면 바로 끝~ DB 검색은 안한다.
	if(!pcsSearchCharacter)
		return FALSE;

	// Party 정보 Attach
	AgpdPartyADChar* pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsSearchCharacter);
	if(!pcsPartyADChar || pcsPartyADChar->lPID == AP_INVALID_PARTYID)
	{
		if(pcsSearchCharacter != pcsAdminCharacter)
			pcsSearchCharacter->m_Mutex.Release();
		
		return FALSE;
	}

	// Party 가 아니다.
	if(pcsPartyADChar->pcsParty == NULL)
	{
		if(pcsSearchCharacter != pcsAdminCharacter)
			pcsSearchCharacter->m_Mutex.Release();
		
		return FALSE;
	}

	BOOL bResult = FALSE;
	// Type 검사
	switch(pstParty->m_lType)
	{
		case 0:	// Party 정보 Send
			bResult = m_pagsmParty->SendPartyInfo(pcsPartyADChar->pcsParty, m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter));
			break;

		case 10:	// 검색할 캐릭터의 탈퇴!!!
			// 아직 처리 안함. 2003.10.24.
			break;
	}

	if(pcsSearchCharacter != pcsAdminCharacter)
		pcsSearchCharacter->m_Mutex.Release();

	return bResult;
}

// 2004.03.22.
// pcsSearchCharacter 와 pcsAgpdCharacter 는 이미 Lock 되어 있다.
BOOL AgsmAdmin::SendSearchCharParty(AgpdCharacter* pcsSearchCharacter, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmParty || !m_pagpmFactors)
		return FALSE;

	if(!pcsSearchCharacter || !pcsAgpdCharacter)
		return FALSE;

	// Party 정보 Attach
	AgpdPartyADChar* pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsSearchCharacter);
	if(!pcsPartyADChar || pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return FALSE;

	// Party 가 아니다.
	if(pcsPartyADChar->pcsParty == NULL || pcsPartyADChar->pcsParty->m_nCurrentMember == 0)
		return FALSE;

	AgpdParty	*pcsParty	= m_pagpmParty->GetPartyLock(pcsPartyADChar->lPID);
	if (!pcsParty)
		return FALSE;

	stAgpdAdminCharParty stParty;
	memset(&stParty, 0, sizeof(stParty));

	// 기본 정보 세팅
	stParty.m_lCID = pcsSearchCharacter->m_lID;
	strncpy(stParty.m_szCharName, pcsSearchCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

	INT32 lValue = 0;

	for(int i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER; i++)
	{
		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pcsPartyADChar->pcsParty->m_lMemberListID[i]);
		if (!pcsCharacter)
			continue;

		memset(&stParty.m_stMemberInfo, 0, sizeof(stAgpdAdminCharPartyMember));

		// 맨 처음 놈이 리더라고 함. -0-
		if(i == 0)
			stParty.m_stMemberInfo.m_lLeader = 1;

		// 캐릭터 이름
		strncpy(stParty.m_stMemberInfo.m_szCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

		// Level
		m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		stParty.m_stMemberInfo.m_lLevel = (INT16)lValue;

		// Admin 에게 보낸다~
		SendSearchCharParty(&stParty, pcsAgpdCharacter->m_lID, GetCharacterNID(pcsAgpdCharacter));

		pcsCharacter->m_Mutex.Release();
	}

	pcsParty->m_Mutex.Release();

	return TRUE;
}

// 2004.03.22.
BOOL AgsmAdmin::SendSearchCharParty(stAgpdAdminCharParty* pstParty, INT32 lCID, UINT32 ulNID)
{
	return TRUE;
}

// 2005.05.06. steeple
BOOL AgsmAdmin::SendCustomPacket(INT32 lCustomType, PVOID pvCustomData, INT16 nDataLength, INT32 lCID, UINT32 ulNID)
{
	if(!pvCustomData || nDataLength < 1 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvCustomPacket = m_pagpmAdmin->MakeCustomPacket(&nPacketLength, &lCustomType, pvCustomData, nDataLength);
	if(!pvCustomPacket)
		return FALSE;

	PVOID pvPacket = m_pagpmAdmin->MakeCustomPacket(&nPacketLength, &lCID, pvCustomPacket);
	m_pagpmAdmin->m_csSearchCustomPacket.FreePacket(pvCustomPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Edit Result 패킷을 보낸다.
BOOL AgsmAdmin::SendCharEditResult(stAgpdAdminCharEdit* pstCharEdit, INT32 lCID, UINT32 ulNID)
{
	if(!pstCharEdit || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvCharEditPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_CHAREDIT;

	pvCharEditPacket = m_pagpmAdmin->MakeCharEditPacket(&nPacketLength, pstCharEdit, sizeof(stAgpdAdminCharEdit));
	if(!pvCharEditPacket)
	{
		OutputDebugString("AgsmAdmin::SendCharEditResult(...) pvCharEditPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeCharEditPacket(&nPacketLength, &lCID, pvCharEditPacket);
	
	m_pagpmAdmin->m_csCharEditPacket.FreePacket(pvCharEditPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendCharEditResult(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Send Ban Packet
BOOL AgsmAdmin::SendBanData(stAgpdAdminBan* pstBan, INT32 lCID, UINT32 ulNID)
{
	if(!pstBan || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvBanPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_BAN;

	pvBanPacket = m_pagpmAdmin->MakeBanPacket(&nPacketLength,
															&lCID,
															pstBan->m_szCharName,
															pstBan->m_szAccName,
															&pstBan->m_lLogout,
															&pstBan->m_lBanFlag,
															&pstBan->m_lChatBanStartTime,
															&pstBan->m_lChatBanKeepTime,
															&pstBan->m_lCharBanStartTime,
															&pstBan->m_lCharBanKeepTime,
															&pstBan->m_lAccBanStartTime,
															&pstBan->m_lAccBanKeepTime);
	if(!pvBanPacket)
	{
		OutputDebugString("AgsmAdmin::SendBanData(...) pvBanPacket is NULL \n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeBanPacket(&nPacketLength, &lCID, pvBanPacket);

	m_pagpmAdmin->m_csBanPacket.FreePacket(pvBanPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendBanData(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}








//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmAdmin::CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminSearch* pstSearch = (stAgpdAdminSearch*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(!pstSearch || !pThis || !pcsCharacter)
		return FALSE;

	return pThis->SearchCharacter(pstSearch, pcsCharacter);
}

BOOL AgsmAdmin::CBSearchCharItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminCharItem* pstItem = (stAgpdAdminCharItem*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstItem || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->SendSearchCharItem(pstItem, pcsAdminCharacter);
}

BOOL AgsmAdmin::CBSearchCharSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminCharSkill* pstSkill = (stAgpdAdminCharSkill*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstSkill || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->SendSearchCharSkill(pstSkill, pcsAdminCharacter);
}

BOOL AgsmAdmin::CBSearchCharParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmAdmin::CBCharMove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminCharDataPosition* pstPosition = (stAgpdAdminCharDataPosition*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstPosition || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->ProcessCharacterMove(pstPosition, pcsAdminCharacter);
}

BOOL AgsmAdmin::CBCharEdit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	stAgpdAdminCharEdit* pstCharEdit = (stAgpdAdminCharEdit*)ppvBuffer[0];
	if(!pstCharEdit)
		return FALSE;
	
	// 여기서 갈라준다.
	switch(pstCharEdit->m_lEditField)
	{
		case AGPMADMIN_CHAREDIT_SKILL_LEARN:
		case AGPMADMIN_CHAREDIT_SKILL_LEVEL:
		case AGPMADMIN_CHAREDIT_SKILL_INIT_ALL:
		case AGPMADMIN_CHAREDIT_SKILL_INIT_SPECIFIC:
		case AGPMADMIN_CHAREDIT_PRODUCT_EXP_CHANGE:
		case AGPMADMIN_CHAREDIT_COMPOSE_LEARN:
		case AGPMADMIN_CHAREDIT_COMPOSE_INIT_ALL:
		case AGPMADMIN_CHAREDIT_COMPOSE_INIT_SPECIFIC:
			return pThis->ProcessCharSkillEdit(pstCharEdit, static_cast<stAgpdAdminSkillString*>(ppvBuffer[1]), pcsAdminCharacter);
			break;

		case AGPMADMIN_CHAREDIT_WANTED_CRIMINAL:
			return pThis->ProcessWCEdit(pstCharEdit, pcsAdminCharacter);
			break;

		default:
			return pThis->ProcessCharacterEdit(pstCharEdit, pcsAdminCharacter);
			break;
	}

	return TRUE;
}

BOOL AgsmAdmin::CBBan(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminBan* pstBan = (stAgpdAdminBan*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstBan || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->ProcessBan(pstBan, pcsAdminCharacter);
}









//////////////////////////////////////////////////////////////////////////
// From AgsmAdminDB
BOOL AgsmAdmin::CBSearchCharacterResultDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szName = (CHAR*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	INT32* plCID = (INT32*)pCustData;

	if(!szName || !pThis || !plCID)
		return FALSE;

	pThis->SearchCharacterResultDB(szName, *plCID);
	return TRUE;

	//AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	//AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	//if(!pcsAgpdCharacter || !pThis)
	//	return FALSE;

	//pThis->SendSearchCharacterToSearchingAdmin(pcsAgpdCharacter);

	return TRUE;
}
