/*===================================================================

	AgpmTitle.cpp

===================================================================*/

#include "AgpmTitle.h"

using namespace std;

AgpmTitle::AgpmTitle()
{
	SetModuleName("AgpmTitle");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPM_TITLE_PACKET_TYPE);
}

AgpmTitle::~AgpmTitle()
{
}

BOOL AgpmTitle::OnAddModule()
{
	return TRUE;
}

BOOL AgpmTitle::OnInit()
{
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmProduct		= (AgpmProduct *) GetModule("AgpmProduct");
	m_pAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pAgpmGrid			= (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmItemConvert	= (AgpmItemConvert *) GetModule("AgpmItemConvert");
	m_pAgpmGuild		= (AgpmGuild *) GetModule("AgpmGuild");

	if(!m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmItem || !m_pAgpmProduct || 
		!m_pAgpmSkill || !m_pAgpmGrid || !m_pAgpmItemConvert || !m_pAgpmGuild)
		return FALSE;

	return TRUE;
}

BOOL AgpmTitle::OnDestroy()
{
	INT32 nIndex = 0;
	AgpdTitleTemplate **ppcsTitleTemplate = (AgpdTitleTemplate **) m_stAgpaTitleTemplate.GetObjectSequence(&nIndex);
	while(ppcsTitleTemplate && *ppcsTitleTemplate)
	{
		m_stAgpaTitleTemplate.Remove(*ppcsTitleTemplate);
		ppcsTitleTemplate = (AgpdTitleTemplate **) m_stAgpaTitleTemplate.GetObjectSequence(&nIndex);
	}
	return TRUE;
}

BOOL AgpmTitle::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize == 0)
		return FALSE;

	PACKET_AGPPTITLE* pPacket = (PACKET_AGPPTITLE*)pvPacket;

	if(!pPacket->strCharName)
		return FALSE;

	AgpdCharacter* pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(pPacket->strCharName);

	if(!pAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch(pPacket->nParam)
	{
	case AGPMTITLE_PACKET_OPERATION_TITLE_ADD:
		{
			EnumCallback(AGPMTITLE_CB_TITLE_ADD, pAgpdCharacter, (PACKET_AGPPTITLE_ADD*)pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_USE:
		{
			EnumCallback(AGPMTITLE_CB_TITLE_USE, pAgpdCharacter, (PACKET_AGPPTITLE_USE*)pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_LIST:
		{
			EnumCallback(AGPMTITLE_CB_TITLE_LIST, pAgpdCharacter, (PACKET_AGPPTITLE_LIST*)pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT:
		{
			OnOperationTitleAddResult(pAgpdCharacter, pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT:
		{
			OnOperationTitleUseResult(pAgpdCharacter, pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT:
		{
			OnOperationTitleListResult(pAgpdCharacter, pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_QUEST:
		{
			OnOperationTitleQuest(pAgpdCharacter, pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_DELETE:
		{
			EnumCallback(AGPMTITLE_CB_TITLE_DELETE, pAgpdCharacter, (PACKET_AGPPTITLE_DELETE*)pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT:
		{
			OnOperationTitleDeleteResult(pAgpdCharacter, pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_USE_NEAR:
		{
			OnOperationTitleUseNear(pAgpdCharacter, pPacket);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_UI_OPEN:
		{
			EnumCallback(AGPMTITLE_CB_TITLE_UI_OPEN, pAgpdCharacter, (PACKET_AGPPTITLE_UI_OPEN*)pPacket);
		}
		break;
	}

	pAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

AgpdTitleTemplate* AgpmTitle::GetTitleTemplate(INT32 nTitleTid)
{
	return m_stAgpaTitleTemplate.Get(nTitleTid);
}

AgpdTitleStringTemplate* AgpmTitle::GetTitleStringTemplate(INT32 nTitleTid)
{
	return &(*m_stAgpaTitleStringTemplate.FindByTitleTid(nTitleTid));
}

BOOL AgpmTitle::SetMaxTitleTemplate(INT32 lCount)
{
	m_stAgpaTitleTemplate.SetCount(lCount);

	if(!m_stAgpaTitleTemplate.InitializeObject(sizeof(AgpdTitleTemplate *), m_stAgpaTitleTemplate.GetCount()))
		return FALSE;

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleUseNear(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket)
{
	if(!pcsCharacter || !pvPacket)
		return FALSE;

	return EnumCallback(AGPMTITLE_CB_TITLE_USE_NEAR, pcsCharacter, (PACKET_AGPPTITLE_USE_NEAR*)pvPacket);
}

BOOL AgpmTitle::OnOperationTitleQuest(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket)
{
	if(!pcsCharacter || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_QUEST* pPacket = (PACKET_AGPPTITLE_QUEST*)pvPacket;

	switch(pPacket->nOperation)
	{
	case AGPMTITLE_QUEST_REQUEST:
		EnumCallback(AGPMTITLE_CB_TITLE_QUEST_REQUEST, pcsCharacter, pvPacket);
		break;
	case AGPMTITLE_QUEST_REQUEST_RESULT:
		OnOperationTitleQuestRequestResult(pcsCharacter, pPacket);
		break;
	case AGPMTITLE_QUEST_CHECK:
		EnumCallback(AGPMTITLE_CB_TITLE_QUEST_CHECK, pcsCharacter, pvPacket);
		break;
	case AGPMTITLE_QUEST_CHECK_RESULT:
		OnOperationTitleQuestCheckResult(pcsCharacter, pPacket);
		break;
	case AGPMTITLE_QUEST_COMPLETE:
		EnumCallback(AGPMTITLE_CB_TITLE_QUEST_COMPLETE, pcsCharacter, pvPacket);
		break;
	case AGPMTITLE_QUEST_COMPLETE_RESULT:
		OnOperationTitleQuestCompleteResult(pcsCharacter, pPacket);
		break;
	case AGPMTITLE_QUEST_LIST:
		EnumCallback(AGPMTITLE_CB_TITLE_QUEST_LIST, pcsCharacter, pvPacket);
		break;
	case AGPMTITLE_QUEST_LIST_RESULT:
		OnOperationTitleQuestListResult(pcsCharacter, pPacket);
		break;
	}

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleAddResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_ADD_RESULT* pPacket = (PACKET_AGPPTITLE_ADD_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(!(pPacket->bAddResult == TRUE && pcsCharacter->m_csTitle->AddTitle(pPacket->nTitleID)))
		pPacket->bAddResult = FALSE;

	EnumCallback(AGPMTITLE_CB_TITLE_ADD_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleUseResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_USE_RESULT* pPacket = (PACKET_AGPPTITLE_USE_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pPacket->bUseResult == TRUE)
	{
		if(pPacket->bUse)
			pPacket->bUseResult = TitleUse(pcsCharacter, pPacket->nTitleID, pPacket->lTimeStamp);
		else
			pPacket->bUseResult = TitleUnUse(pcsCharacter, pPacket->nTitleID, pPacket->lTimeStamp);
	}

	EnumCallback(AGPMTITLE_CB_TITLE_USE_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleListResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket)
{
	if(!pcsCharacter || !pvPacket)
		return FALSE;

	if(!pcsCharacter->m_csTitle)
		pcsCharacter->m_csTitle = new AgpdTitle();

	PACKET_AGPPTITLE_LIST_RESULT* pPacket = (PACKET_AGPPTITLE_LIST_RESULT*) pvPacket;

	if(pPacket->bListResult && !pPacket->bTitleListEnd)
	{
		AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

		if(!pTitleTemplate)
			return FALSE;

		if(!pcsCharacter->m_csTitle->AddTitle(pPacket->nTitleID))
			return FALSE;
	}

	pcsCharacter->m_csTitle->m_bLoadedTitle = TRUE;

	EnumCallback(AGPMTITLE_CB_TITLE_LIST_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleDeleteResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_DELETE_RESULT* pPacket = (PACKET_AGPPTITLE_DELETE_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pPacket->bTitleDeleteResult)
	{
		pPacket->bTitleDeleteResult = FALSE;

		if(pcsCharacter->m_csTitle->IsHaveTitle(pPacket->nTitleID) 
			&& pcsCharacter->m_csTitle->RemoveTitle(pPacket->nTitleID))
			pPacket->bTitleDeleteResult = TRUE;

		if(pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pPacket->nTitleID) 
			&& pcsCharacter->m_csTitleQuest->RemoveTitleQuest(pPacket->nTitleID))
			pPacket->bTitleDeleteResult = TRUE;

	}

	EnumCallback(AGPMTITLE_CB_TITLE_DELETE_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleQuestRequestResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_REQUEST_RESULT* pPacket = (PACKET_AGPPTITLE_QUEST_REQUEST_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	AgpdCurrentTitleQuest stTitleQuest;

	stTitleQuest.m_nTitleTid = pTitleTemplate->m_nTitleTid;

	for(IterTitleCheck iter = pTitleTemplate->m_vtTitleCheck.begin(); iter != pTitleTemplate->m_vtTitleCheck.end(); ++iter)
	{
		AgpdTitleCurrentCheck stTitleCheck;

		stTitleCheck.m_nTitleCheckType = (*iter).m_nTitleCheckType;
		stTitleCheck.m_nTitleCheckSet = (*iter).m_nTitleCheckSet;
		stTitleCheck.m_nTitleCheckValue = (*iter).m_nTitleCheckValue;
		stTitleCheck.m_nTitleCurrentValue = 0;

		stTitleQuest.m_vtTitleCurrentCheck.push_back(stTitleCheck);
	}

	stTitleQuest.m_nTitleQuestAcceptTime = pPacket->lTimeStamp;

	if(!(pPacket->bQuestRequestResult == TRUE && pcsCharacter->m_csTitleQuest->AddTitleQuest(stTitleQuest)))
		pPacket->bQuestRequestResult = FALSE;

	EnumCallback(AGPMTITLE_CB_TITLE_QUEST_REQUEST_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleQuestCheckResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_CHECK_RESULT* pPacket = (PACKET_AGPPTITLE_QUEST_CHECK_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	INT32 i = 0;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		if((*iter).m_nTitleTid == pPacket->nTitleID)
		{
			for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
			{
				(*iter2).m_nTitleCurrentValue = pPacket->nTitleCurrentValue[i];
				i++;
			}
		}
	}

	EnumCallback(AGPMTITLE_CB_TITLE_QUEST_CHECK_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleQuestCompleteResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !pvPacket)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT* pPacket = (PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(pPacket->nTitleID);

	if(iter == pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end())
		return FALSE;

	if(pPacket->bCompleteResult)
	{
		(*iter).m_nTitleQuestCompleteTime = pPacket->lTimeStamp;

		if(!pcsCharacter->m_csTitleQuest->RemoveTitleQuest(pPacket->nTitleID))
			pPacket->bCompleteResult = FALSE;
	}

	EnumCallback(AGPMTITLE_CB_TITLE_QUEST_COMPLETE_RESULT, pcsCharacter, pPacket);

	return TRUE;
}

BOOL AgpmTitle::OnOperationTitleQuestListResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket)
{
	if(!pcsCharacter || !pvPacket)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest)
		pcsCharacter->m_csTitleQuest = new AgpdTitleQuest();

	PACKET_AGPPTITLE_QUEST_LIST_RESULT* pPacket = (PACKET_AGPPTITLE_QUEST_LIST_RESULT*) pvPacket;

	AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pPacket->bTitleQuestListResult && !pPacket->bListEnd)
	{
		AgpdTitleTemplate *pTitleTemplate = GetTitleTemplate(pPacket->nTitleID);

		if(!pTitleTemplate)
			return FALSE;

		/*if (pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(pPacket->nTitleID))
			return FALSE;*/

		if (pcsCharacter->m_csTitleQuest->CheckForQuestByTitleID(pPacket->nTitleID))
			return FALSE;

		AgpdCurrentTitleQuest stTitleQuest;

		stTitleQuest.m_nTitleTid = pTitleTemplate->m_nTitleTid;

		stTitleQuest.m_nTitleQuestAcceptTime = pPacket->lAcceptTime;
		stTitleQuest.m_nTitleQuestCompleteTime = pPacket->lCompleteTime;

		INT32 i = 0;

		for(IterTitleCheck iter = pTitleTemplate->m_vtTitleCheck.begin(); iter != pTitleTemplate->m_vtTitleCheck.end(); ++iter)
		{
			AgpdTitleCurrentCheck stTitleCheck;

			stTitleCheck.m_nTitleCheckType = (*iter).m_nTitleCheckType;
			stTitleCheck.m_nTitleCheckSet = (*iter).m_nTitleCheckSet;
			stTitleCheck.m_nTitleCheckValue = (*iter).m_nTitleCheckValue;
			stTitleCheck.m_nTitleCurrentValue = pPacket->nTitleCurrentValue[i];
			i++;

			stTitleQuest.m_vtTitleCurrentCheck.push_back(stTitleCheck);
		}

		if(!pPacket->bComplete)
			pPacket->bTitleQuestListResult = pcsCharacter->m_csTitleQuest->AddTitleQuest(stTitleQuest);
	}

	pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest = TRUE;

	EnumCallback(AGPMTITLE_CB_TITLE_QUEST_LIST_RESULT, pcsCharacter, pPacket);

	return TRUE;
}


BOOL AgpmTitle::SatisfyTitleQuestRequireCondition(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireLevel > m_pAgpmCharacter->GetLevel(pcsCharacter))
		return FALSE;

	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireGuildMaster && !IsGuildMaster(pcsCharacter))
		return FALSE;

	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireClassArchlord && !m_pAgpmCharacter->IsArchlord(pcsCharacter))
		return FALSE;

	INT32 nRace = m_pAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32 nClassType = m_pAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	switch(pcsTitleTemplate->m_stRequireCondition.m_nRequireClass)
	{
	case AGPMTITLE_REQUIRE_CLASS_ALL:
		break;
	case AGPMTITLE_REQUIRE_CLASS_KNIGHT:
		if(!(nRace == AURACE_TYPE_HUMAN && nClassType == AUCHARCLASS_TYPE_KNIGHT))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_ARCHER:
		if(!(nRace == AURACE_TYPE_HUMAN && nClassType == AUCHARCLASS_TYPE_RANGER))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_WIZARD:
		if(!(nRace == AURACE_TYPE_HUMAN && nClassType == AUCHARCLASS_TYPE_MAGE))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_BERSERKER:
		if(!(nRace == AURACE_TYPE_ORC && nClassType == AUCHARCLASS_TYPE_KNIGHT))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_HUNTER:
		if(!(nRace == AURACE_TYPE_ORC && nClassType == AUCHARCLASS_TYPE_RANGER))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_SORCERER:
		if(!(nRace == AURACE_TYPE_ORC && nClassType == AUCHARCLASS_TYPE_MAGE))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_SWASHBUCKLER:
		if(!(nRace == AURACE_TYPE_MOONELF && nClassType == AUCHARCLASS_TYPE_KNIGHT))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_RANGER:
		if(!(nRace == AURACE_TYPE_MOONELF && nClassType == AUCHARCLASS_TYPE_RANGER))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_ELEMENTALER:
		if(!(nRace == AURACE_TYPE_MOONELF && nClassType == AUCHARCLASS_TYPE_MAGE))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_SCION:
		if(nRace != AURACE_TYPE_DRAGONSCION)
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_SLAYER:
		if(!(nRace == AURACE_TYPE_DRAGONSCION && nClassType == AUCHARCLASS_TYPE_KNIGHT))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_ORBITER:
		if(!(nRace == AURACE_TYPE_DRAGONSCION && nClassType == AUCHARCLASS_TYPE_RANGER))
			return FALSE;
		break;
	case AGPMTITLE_REQUIRE_CLASS_SUMMONER:
		if(!(nRace == AURACE_TYPE_DRAGONSCION && nClassType == AUCHARCLASS_TYPE_MAGE))
			return FALSE;
		break;
	}

	INT32 lValue	= 0;

	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireCon > lValue)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireStr > lValue)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireInt > lValue)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireDex > lValue)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireCha > lValue)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireWis > lValue)
		return FALSE;

	INT64 llMoney = 0;

	m_pAgpmCharacter->GetMoney(pcsCharacter, &llMoney);
	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireGheld > llMoney)
		return FALSE;

	if(pcsTitleTemplate->m_stRequireCondition.m_nRequireCash > m_pAgpmCharacter->GetCash(pcsCharacter))
		return FALSE;
	
	for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_REQUIRE_ITEM; i++)
	{
		if(pcsTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][0] != 0 
			&& !CheckItemCount(pcsCharacter, pcsTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][0], pcsTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][1]))
			return FALSE;
	}

	for(INT32 i = 0; i <AGPDTITLE_MAX_TITLE_REQUIRE_TITLE; i++)
	{
		if(pcsTitleTemplate->m_stRequireCondition.m_nRequireTitleTid[i] != 0 
			&& !pcsCharacter->m_csTitle->IsHaveTitle(pcsTitleTemplate->m_stRequireCondition.m_nRequireTitleTid[i]))
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmTitle::SatisfyTitleQuestCompleteCondition(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(lTitleID))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->CheckQuestCompletionCondition(lTitleID))
		return FALSE;

	return TRUE;
}

//기원석 종류 구분하기 위한 함수
BOOL AgpmTitle::CheckSameItemGroup(INT32 nItemTid1, INT32 nItemTid2)
{
	if(!m_pAgpmItem)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate1 = m_pAgpmItem->GetItemTemplate(nItemTid1);
	AgpdItemTemplate* pcsItemTemplate2 = m_pAgpmItem->GetItemTemplate(nItemTid2);

	if(!pcsItemTemplate1 || !pcsItemTemplate2)
		return FALSE;

	if(((AgpdItemTemplateUsable *) pcsItemTemplate1)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
	{
		if(((AgpdItemTemplateUsable *) pcsItemTemplate1)->m_nUsableItemType != ((AgpdItemTemplateUsable *) pcsItemTemplate2)->m_nUsableItemType)
			return FALSE;

		if(((AgpdItemTemplateUsableRune *) pcsItemTemplate1)->m_eRuneAttributeType != ((AgpdItemTemplateUsableRune *) pcsItemTemplate2)->m_eRuneAttributeType)
			return FALSE;
	}
	else if(((AgpdItemTemplateUsable *) pcsItemTemplate1)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
	{
		if(((AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate1)->m_eSpiritStoneType != ((AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate2)->m_eSpiritStoneType)
			return FALSE;
	}
	else
	{
		if(nItemTid1 != nItemTid2)
			return FALSE;
	}

	if(pcsItemTemplate1->m_lFirstCategory != pcsItemTemplate2->m_lFirstCategory)
		return FALSE;

	if(pcsItemTemplate1->m_lSecondCategory != pcsItemTemplate2->m_lSecondCategory)
		return FALSE;

	return TRUE;
}

INT32 AgpmTitle::GetGroupItemCount(AgpdGrid *pcsAgpdGrid, INT32 nItemTid)
{
	INT32 nCount = 0;

	if(!pcsAgpdGrid)
		return FALSE;

	AgpdGridItem *pcsAgpdGridItem = NULL;

	for(INT32 i = 0; i < pcsAgpdGrid->m_lGridCount; i++)
	{
		pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[i];

		if(pcsAgpdGridItem && CheckSameItemGroup(nItemTid, pcsAgpdGridItem->m_lItemTID))
		{
			AgpdItem *pcsAgpdItem = (AgpdItem*)pcsAgpdGridItem->GetParentBase();
			AgpdItemTemplate *pcsItemTemplate = (AgpdItemTemplate*)pcsAgpdItem->m_pcsItemTemplate;

			if(pcsItemTemplate->m_bStackable)
				nCount += pcsAgpdItem->m_nCount;
			else
				nCount++;
		}
	}

	return nCount;
}

INT32 AgpmTitle::GetItemCountFromCharacter(AgpdCharacter* pcsCharacter, INT32 nItemTid)
{
	INT32 nCount = 0;

	if(!pcsCharacter)
		return FALSE;

	if(!m_pAgpmItem)
		return FALSE;

	AgpdGrid *pcsInventory = m_pAgpmItem->GetInventory(pcsCharacter);
	AgpdGrid *pcsCashInventory = m_pAgpmItem->GetCashInventoryGrid(pcsCharacter);
	AgpdGrid *pcsPetInventory = m_pAgpmItem->GetSubInventory(pcsCharacter);

	if(!pcsInventory || !pcsCashInventory || !pcsPetInventory)
		return FALSE;

	nCount += GetGroupItemCount(pcsInventory, nItemTid);
	nCount += GetGroupItemCount(pcsCashInventory, nItemTid);
	nCount += GetGroupItemCount(pcsPetInventory, nItemTid);

	return nCount;
}

BOOL AgpmTitle::IsGuildMaster(AgpdCharacter *pcsCharacter)
{
	AgpdGuildADChar* pcsAttachedGuild = m_pAgpmGuild->GetADCharacter(pcsCharacter);
	if(NULL == pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;

	return (m_pAgpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID));
}

BOOL AgpmTitle::GetEffectValue(AgpdCharacter* pcsCharacter, INT32 nEffectType, INT32 *pnEffectSet, INT32 *pnEffectValue1, INT32 *pnEffectValue2)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	vector<AgpdHasTitle>::iterator iter = pcsCharacter->m_csTitle->GetUsingTitle();

	if(iter == pcsCharacter->m_csTitle->m_vtTitle.end())
		return FALSE;

	AgpdTitleTemplate* pcsTitleTemplate = GetTitleTemplate((*iter).lTitleID);

	IterTitleEffect iter2;

	for(iter2 = pcsTitleTemplate->m_vtTitleEffect.begin(); iter2 != pcsTitleTemplate->m_vtTitleEffect.end(); ++iter2)
	{
		if((*iter2).m_nEffectType == nEffectType)
		{
			*pnEffectSet = (*iter2).m_nEffectSet;
			*pnEffectValue1 = (*iter2).m_nEffectValue1;
			*pnEffectValue2 = (*iter2).m_nEffectValue2;
		}
	}

	return TRUE;
}

INT32 AgpmTitle::GetFactorItemOptionValue(AgpdItemOptionTemplate* pcsItemOptionTemplate, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2)
{
	INT32 nValue = 0;

	if(!m_pAgpmFactors)
		return FALSE;

	if(!pcsItemOptionTemplate)
		return FALSE;

	if(pcsItemOptionTemplate->m_lPointType == 0)
		m_pAgpmFactors->GetValue(&pcsItemOptionTemplate->m_csFactor, &nValue, eFactorType, lFactorSubType1, lFactorSubType2);
	else
	{
		FLOAT fValue = 0.0f;
		m_pAgpmFactors->GetValue(&pcsItemOptionTemplate->m_csFactor, &fValue, eFactorType, lFactorSubType1, lFactorSubType2);
		nValue = (INT32)fValue;
	}

	return nValue;
}

INT32 AgpmTitle::GetItemOptionValue(AgpdItem* pcsItem, INT32 lCharacterLevel, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2)
{
	INT32 nItemOptionValue = 0;

	if(!m_pAgpmItem || !m_pAgpmFactors || !m_pAgpmGrid || !m_pAgpmSkill || !m_pAgpmItemConvert)
		return FALSE;

	if(!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsItem->m_csFactor, &nItemOptionValue, eFactorType, lFactorSubType1, lFactorSubType2);

	return nItemOptionValue;
}

INT32 AgpmTitle::GetCharacterStatusByItem(AgpdCharacter* pcsCharacter, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2)
{
	INT32 nValue = 0;

	if(!m_pAgpmItem || !m_pAgpmFactors || !m_pAgpmGrid)
		return FALSE;

	AgpdItemADChar* pcsItemADChar = m_pAgpmItem->GetADCharacter(pcsCharacter);

	if(!pcsItemADChar)
		return FALSE;

	AgpdGrid* pcsEquipGrid = &pcsItemADChar->m_csEquipGrid;

	if(!pcsEquipGrid)
		return FALSE;

	INT32 nCharacterLevel = 0;
	m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &nCharacterLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* pcsGridItem = m_pAgpmGrid->GetItemSequence(pcsEquipGrid, &nSequencialIndex);

	while(pcsGridItem)
	{
		AgpdItem* pcsItem = m_pAgpmItem->GetItem(pcsGridItem->m_lItemID);
		if(!pcsItem)
			continue;

		INT32 nSubValue = GetItemOptionValue(pcsItem, nCharacterLevel, eFactorType, lFactorSubType1, lFactorSubType2);
		if(nSubValue != 0)
			nValue += nSubValue;

		pcsGridItem = m_pAgpmGrid->GetItemSequence(pcsEquipGrid, &nSequencialIndex);
	}

	return nValue;
}

INT32 AgpmTitle::GetInitialCharacterStatus(AgpdCharacter* pcsCharacter, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2)
{
	INT32 lValue = 0;

	if(!pcsCharacter)
		return FALSE;

	if(!m_pAgpmFactors)
		return FALSE;

	AgpdCharacterTemplate *pcsCharacterTemplate = pcsCharacter->m_pcsCharacterTemplate;

	if(!pcsCharacterTemplate)
		return FALSE;

	INT32 lLevel = m_pAgpmCharacter->GetLevel(pcsCharacter);

	if(lLevel < 0)
		return FALSE;

	m_pAgpmFactors->GetValue(&pcsCharacterTemplate->m_csLevelFactor[lLevel], &lValue, eFactorType, lFactorSubType1, lFactorSubType2);

	return lValue;
}

INT32 AgpmTitle::GetProductSkillLevel(AgpdCharacter* pcsCharacter, INT32 m_eCategory)
{
	INT32 lValue = 0;

	INT32 lSkillList[10];
	INT32 lSkill = m_pAgpmProduct->GetSkillList(m_eCategory, lSkillList, 10);

	for(INT32 i = 0; i < lSkill; i++)
	{
		AgpdSkill* pcsSkill = m_pAgpmSkill->GetSkillByTID(pcsCharacter, lSkillList[i]);
		INT32 lSkillLevel = 0;
		if(pcsSkill)
			lSkillLevel = m_pAgpmSkill->GetSkillLevel(pcsSkill);

		if(lSkillLevel > lValue)
			lValue = lSkillLevel;
	}

	return lValue;
}

BOOL AgpmTitle::CheckItemCount(AgpdCharacter *pcsCharacter, INT32 nItemTid, INT32 nItemCount)
{
	if(!pcsCharacter)
		return FALSE;

	INT32 lCount = 0;

	AgpdGrid *pcsGrid1 = m_pAgpmItem->GetInventory(pcsCharacter);

	if(pcsGrid1)
		lCount += m_pAgpmGrid->GetCountByTemplate(pcsGrid1, AGPDGRID_ITEM_TYPE_ITEM, nItemTid);

	AgpdGrid *pcsGrid2 = m_pAgpmItem->GetCashInventoryGrid(pcsCharacter);

	if(pcsGrid2)
		lCount += m_pAgpmGrid->GetCountByTemplate(pcsGrid2, AGPDGRID_ITEM_TYPE_ITEM, nItemTid);

	if(lCount < nItemCount)
		return FALSE;

	return TRUE;
}

INT32 AgpmTitle::GetCharacterStatusItemAndInitialValue(AgpdCharacter* pcsCharacter, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2)
{
	INT32 lValue = 0;

	if(!pcsCharacter)
		return FALSE;

	lValue += GetInitialCharacterStatus(pcsCharacter, eFactorType, lFactorSubType1, lFactorSubType2);
	lValue += GetCharacterStatusByItem(pcsCharacter, eFactorType, lFactorSubType1, lFactorSubType2);

	return lValue;
}

INT32 AgpmTitle::GetTitleQuestCheckValueInStatus(AgpdCharacter* pcsCharacter, INT32 lCheckValueType)
{
	INT32 lValue = 0;

	if(!pcsCharacter)
		return NULL;

	switch(lCheckValueType)
	{
	case AGPMTITLE_TITLE_CHECK_TYPE_MERDER:
		m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_MAGICTYPEATTACK:
		lValue = GetCharacterStatusItemAndInitialValue(pcsCharacter, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_FIRETYPEATTACK:
		lValue = GetCharacterStatusItemAndInitialValue(pcsCharacter, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_WINDTYPEATTACK:
		lValue = GetCharacterStatusItemAndInitialValue(pcsCharacter, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_WATERTYPEATTACK:
		lValue = GetCharacterStatusItemAndInitialValue(pcsCharacter, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_EARTHTYPEATTACK:
		lValue = GetCharacterStatusItemAndInitialValue(pcsCharacter, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_LEVEL_OF_ALCHEMIST:
		lValue = GetProductSkillLevel(pcsCharacter, AGPMPRODUCT_CATEGORY_ALCHEMY);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_LEVEL_OF_COOKING:
		lValue = GetProductSkillLevel(pcsCharacter, AGPMPRODUCT_CATEGORY_COOK);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_LEVEL_OF_HUNT:
		lValue = GetProductSkillLevel(pcsCharacter, AGPMPRODUCT_CATEGORY_HUNT);
		break;
	case AGPMTITLE_TITLE_CHECK_TYPE_ADVANCED:
		m_pAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		break;
	}

	return lValue;
}

INT32 AgpmTitle::GetHaveTitleNumber(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return 0;

	return pcsCharacter->m_csTitle->GetHaveTitleNumber();
}

BOOL AgpmTitle::SetTitleQuestCheckValue(AgpdCharacter* pcsCharacter, INT32 lTitleID, INT32 lTitleCheckType, INT32 lCheckSet, INT32 lTitleNewValue)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !pcsCharacter->m_csTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(lTitleID))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(lTitleID))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->CorrectCheckStyle(lTitleID, lTitleCheckType, lCheckSet))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->UpdateQuestCheckValue(lTitleID, lTitleCheckType, lCheckSet, lTitleNewValue))
		return FALSE;

	return TRUE;
}

BOOL AgpmTitle::TitleUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 lTimeStamp)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsHaveTitle(lTitleID))
		return FALSE;

	if(pcsCharacter->m_csTitle->IsUsingTitle(lTitleID))
		return FALSE;
	
	if(!pcsCharacter->m_csTitle->UpdateUsingTitle(lTitleID, TRUE) || !pcsCharacter->m_csTitle->UpdateSetTime(lTitleID, lTimeStamp))
		return FALSE;	

	return TRUE;
}

BOOL AgpmTitle::TitleUnUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 lTimeStamp)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsHaveTitle(lTitleID))
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsUsingTitle(lTitleID))
		return FALSE;
	
	if(!pcsCharacter->m_csTitle->UpdateUsingTitle(lTitleID, FALSE) || !pcsCharacter->m_csTitle->UpdateSetTime(lTitleID, lTimeStamp))
		return FALSE;

	return TRUE;
}

BOOL AgpmTitle::SetCallBackTitleAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_ADD, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleUse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_USE, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_LIST, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleUseNear(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_USE_NEAR, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_UI_OPEN, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_REQUEST, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_CHECK, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_COMPLETE, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_LIST, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleAddResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_ADD_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleUseResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_USE_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleListResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_LIST_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestRequestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_REQUEST_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_CHECK_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestCompleteResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_COMPLETE_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleQuestListResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_QUEST_LIST_RESULT, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_DELETE, pfCallback, pClass);
}

BOOL AgpmTitle::SetCallBackTitleDeleteResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMTITLE_CB_TITLE_DELETE_RESULT, pfCallback, pClass);
}