#include "AgsmTitle.h"
#include "AgsmItem.h"
#include "AgpmGrid.h"
#include "AgsmParty.h"
#include "AgpmLog.h"
#include "AuGameEnv.h"

AgsmTitle::AgsmTitle()
{
	SetModuleName("AgsmTitle");
	SetModuleType(APMODULE_TYPE_SERVER);

	EnableIdle(FALSE);

	m_pAgpmTitle = NULL;
	m_pAgsmCharacter = NULL;
	m_pAgpmCharacter = NULL;
	m_pAgsmServerManager = NULL;
	m_pAgpmItem = NULL;
	m_pAgsmItem = NULL;
	m_pAgpmGrid = NULL;
	m_pAgsmItemManager = NULL;
	m_pAgpmSkill = NULL;
	m_pAgsmSkill = NULL;
	m_pAgpmQuest = NULL;
	m_pAgsmDeath = NULL;
	m_pAgpmProduct = NULL;
	m_pAgsmCharManager = NULL;
	m_pAgsmAOIFilter = NULL;
	m_pAgsmParty = NULL;
	m_pAgpmLog = NULL;
}

AgsmTitle::~AgsmTitle()
{
}

BOOL AgsmTitle::OnAddModule()
{
	return TRUE;
}

BOOL AgsmTitle::OnInit()
{
	m_pAgpmTitle = (AgpmTitle*) GetModule("AgpmTitle");
	m_pAgsmCharacter = (AgsmCharacter*) GetModule("AgsmCharacter");
	m_pAgpmCharacter = (AgpmCharacter*) GetModule("AgpmCharacter");
	m_pAgsmServerManager = (AgsmServerManager2*) GetModule("AgsmServerManager2");
	m_pAgpmItem = (AgpmItem*) GetModule("AgpmItem");
	m_pAgsmItemManager = (AgsmItemManager*) GetModule("AgsmItemManager");
	m_pAgpmSkill = (AgpmSkill*) GetModule("AgpmSkill");
	m_pAgsmSkill = (AgsmSkill*) GetModule("AgsmSkill");
	m_pAgpmQuest = (AgpmQuest*) GetModule("AgpmQuest");
	m_pAgsmDeath = (AgsmDeath*) GetModule("AgsmDeath");
	m_pAgpmProduct = (AgpmProduct*) GetModule("AgpmProduct");
	m_pAgsmCharManager = (AgsmCharManager*) GetModule("AgsmCharManager");
	m_pAgsmAOIFilter = (AgsmAOIFilter*) GetModule("AgsmAOIFilter");
	m_pAgsmItem = (AgsmItem*) GetModule("AgsmItem");
	m_pAgpmGrid = (AgpmGrid*) GetModule("AgpmGrid");
	m_pAgsmParty = (AgsmParty*) GetModule("AgsmParty");
	m_pAgpmLog = (AgpmLog*) GetModule("AgpmLog");

	if(!m_pAgpmTitle || !m_pAgsmCharacter || !m_pAgpmCharacter || !m_pAgsmServerManager
		|| !m_pAgpmItem || !m_pAgsmSkill || !m_pAgpmQuest || !m_pAgsmDeath || !m_pAgpmProduct
		|| !m_pAgsmCharManager || !m_pAgsmAOIFilter || !m_pAgsmItem || !m_pAgpmGrid || !m_pAgsmParty
		|| !m_pAgpmLog)
		return FALSE;

	m_pAgpmTitle->SetCallBackTitleAdd(CBTitleAdd, this);
	m_pAgpmTitle->SetCallBackTitleUse(CBTitleUse, this);
	m_pAgpmTitle->SetCallBackTitleList(CBTitleList, this);
	m_pAgpmTitle->SetCallBackTitleDelete(CBTitleDelete, this);

	m_pAgpmTitle->SetCallBackTitleQuestRequest(CBTitleQuestRequest, this);
	m_pAgpmTitle->SetCallBackTitleQuestCheck(CBTitleQuestCheck, this);
	m_pAgpmTitle->SetCallBackTitleQuestComplete(CBTitleQuestComplete, this);
	m_pAgpmTitle->SetCallBackTitleQuestList(CBTitleQuestList, this);

	m_pAgsmDeath->SetCallbackCharacterDead(CBMonsterDeath, this);
	//m_pAgpmQuest->SetCallbackCompleteQuest(CBQuestComplete, this);
	m_pAgpmCharacter->SetCallbackUpdateLevelPost(CBLevelUp, this);
	m_pAgsmCharacter->SetCallbackReCalcResultFactor(CBRecalcFactor, this);
	m_pAgpmProduct->SetCallbackGather(CBGather, this);
	m_pAgpmSkill->SetCallbackUpdateSkillPoint(CBUpdateSkillPoint, this);
	m_pAgpmItem->SetCallbackPickupItemResult(CBPickUpItemResult, this);
	m_pAgsmCharacter->SetCallbackSendCharacterNewID(CBSendTitleDataAllInfo, this);
	//m_pAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this);

	return TRUE;
}

BOOL AgsmTitle::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmTitle::OnDestroy()
{
	return TRUE;
}

BOOL AgsmTitle::CBTitleAdd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_ADD *pPacket = (PACKET_AGPPTITLE_ADD *)pCustData;

	if(!pcsCharacter || !pPacket)
		return FALSE;

	return pThis->TitleAddRequest(pcsCharacter, pPacket->nTitleID);
}

BOOL AgsmTitle::CBTitleUse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_USE *pPacket = (PACKET_AGPPTITLE_USE *)pCustData;

	if(!pcsCharacter || !pPacket)
		return FALSE;

	return pThis->TitleUseRequest(pcsCharacter, pPacket->nTitleID, pPacket->bUse);
}

BOOL AgsmTitle::CBTitleList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_LIST *pPacket = (PACKET_AGPPTITLE_LIST *)pCustData;

	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pPacket)
		return FALSE;

	if(!pcsCharacter->m_csTitle->m_bLoadedTitle)
		return pThis->MakeAndSendTitleListRelayPacket(pcsCharacter);
	else
		return pThis->TitleListSend(pcsCharacter);
}

BOOL AgsmTitle::CBTitleDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_DELETE *pPacket = (PACKET_AGPPTITLE_DELETE *)pCustData;
	
	if(!pcsCharacter || !pPacket)
		return FALSE;

	return pThis->TitleDeleteRequest(pcsCharacter, pPacket->nTitleID);
}

BOOL AgsmTitle::CBTitleQuestRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_QUEST_REQUEST *pPacket = (PACKET_AGPPTITLE_QUEST_REQUEST *)pCustData;

	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest || !pPacket)
		return FALSE;

	if(pPacket->nOperation != AGPMTITLE_QUEST_REQUEST)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = pThis->m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(pPacket->nTitleID))
		return FALSE;

	if(pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pPacket->nTitleID))
		return FALSE;

	if(pcsCharacter->m_csTitleQuest->IsFullTitleQuest())
		return FALSE;

	if(!pThis->m_pAgpmTitle->SatisfyTitleQuestRequireCondition(pcsCharacter, pPacket->nTitleID))
		return FALSE;

	if(!pThis->RobCostItem(pcsCharacter, pPacket->nTitleID))
		return FALSE;

	pThis->WriteLogTitle(AGSMTITLE_LOG_TITLEQUEST_ADD, pcsCharacter, pPacket->nTitleID);

	return pThis->MakeAndSendTitleQuestRequestRelayPacket(pcsCharacter, pPacket->nTitleID);
}

BOOL AgsmTitle::CBTitleQuestCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_QUEST_CHECK *pPacket = (PACKET_AGPPTITLE_QUEST_CHECK *)pCustData;

	if(!pcsCharacter || !pPacket)
		return FALSE;

	if(pPacket->nOperation != AGPMTITLE_QUEST_CHECK)
		return FALSE;

	INT32 nCurrentValue = pPacket->nTitleCurrentValue;

	return pThis->TitleQuestCheck(pcsCharacter, pPacket->nTitleID, pPacket->nTitleCheckType, pPacket->nTitleCheckSet, nCurrentValue);
}

BOOL AgsmTitle::CBTitleQuestComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_QUEST_COMPLETE *pPacket = (PACKET_AGPPTITLE_QUEST_COMPLETE *)pCustData;

	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest || !pPacket)
		return FALSE;

	if(pPacket->nOperation != AGPMTITLE_QUEST_COMPLETE)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = pThis->m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(pPacket->nTitleID))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pPacket->nTitleID))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->CheckQuestCompletionCondition(pPacket->nTitleID))
		return FALSE;

	if(pcsCharacter->m_csTitle->IsFullTitle())
		return FALSE;

	pThis->WriteLogTitle(AGSMTITLE_LOG_TITLEQUEST_COMPLETE, pcsCharacter, pPacket->nTitleID);

	return pThis->MakeAndSendTitleQuestCompleteRelayPacket(pcsCharacter, pPacket->nTitleID);
}

BOOL AgsmTitle::CBTitleQuestList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmTitle *pThis = (AgsmTitle *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	PACKET_AGPPTITLE_QUEST_LIST *pPacket = (PACKET_AGPPTITLE_QUEST_LIST *)pCustData;

	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !pPacket)
		return FALSE;

	if(pPacket->nOperation != AGPMTITLE_QUEST_LIST)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest)
		return pThis->MakeAndSendTitleQuestListRelayPacket(pcsCharacter);
	else
		return pThis->TitleQuestListSend(pcsCharacter);
}

BOOL AgsmTitle::CBMonsterDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle*)pClass;
	ApBase *pcsBaseMonster = (ApBase*)pData;
	ApBase *pcsBase = (ApBase*)pCustData;

	if(APBASE_TYPE_CHARACTER != pcsBaseMonster->m_eType)
		return FALSE;

	BOOL bResult = FALSE;
	if(APBASE_TYPE_CHARACTER == pcsBase->m_eType)
	{
		if(pThis->m_pAgpmCharacter->IsPC((AgpdCharacter*)pcsBase) && 
			pThis->m_pAgpmCharacter->IsMonster((AgpdCharacter*)pcsBaseMonster))
			bResult = pThis->MonsterDeathByCharacter((AgpdCharacter*)pcsBase, (AgpdCharacter*)pcsBaseMonster);
	}
	else if(APBASE_TYPE_PARTY == pcsBase->m_eType)
	{
		if(pThis->m_pAgpmCharacter->IsMonster((AgpdCharacter*)pcsBaseMonster))
			bResult = pThis->MonsterDeathByParty((AgpdParty*)pcsBase, (AgpdCharacter*)pcsBaseMonster);
	}

	return bResult;
}

BOOL AgsmTitle::CBQuestComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmTitle *pThis		= (AgsmTitle*)pClass;
	CBQuestParam* pParam	= (CBQuestParam*)pData;

	return pThis->QuestComplete(pParam->pcsAgpdCharacter);
}

BOOL AgsmTitle::CBLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;
	INT32 lChangeLevel = *(INT32 *)pCustData;

	return pThis->CharacterLevelUp(pcsCharacter, lChangeLevel);
}

BOOL AgsmTitle::CBRecalcFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;

	return pThis->RecalcFactor(pcsCharacter);
}

BOOL AgsmTitle::CBGather(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData || !pCustData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle *) pClass;
	AgpdSkill *pAgpdSkill = (AgpdSkill *) ((PVOID *) pData)[0];
	ApBase *pTarget = (ApBase *) ((PVOID *) pData)[1];
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pCustData;

	return pThis->OnGather(pcsCharacter, pAgpdSkill, pTarget);
}

BOOL AgsmTitle::CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData || !pCustData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle *) pClass;
	AgpdSkill *pcsSkill = (AgpdSkill *) pData;

	return pThis->OnUpdateSkillPoint((AgpdCharacter *)pcsSkill->m_pcsBase);
}

BOOL AgsmTitle::CBPickUpItemResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData || !pCustData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle *) pClass;
	INT8 cResult = *(INT8 *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;

	AgpdItemTemplate *pcsItemTemplate = (AgpdItemTemplate *) ppvBuffer[0];
	INT32 lItemCount = PtrToInt(ppvBuffer[1]);
	INT32 lCid = PtrToInt(ppvBuffer[2]);
	AgpdItem *pcsItem = (AgpdItem *) ppvBuffer[4];

	if(!pcsItemTemplate)
		return FALSE;

	AgpdCharacter *pcsCharacter = pThis->m_pAgpmCharacter->GetCharacter(lCid);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsItem)
		return FALSE;

	if(cResult != AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS)
		return FALSE;

	if(pcsItem->m_lPrevOwner != 0)
		return FALSE;

	return pThis->OnPickUpItemResult(pcsCharacter, pcsItemTemplate, lItemCount);
}

BOOL AgsmTitle::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;

	return pThis->OnEnterGameWorld(pcsCharacter);
}

BOOL AgsmTitle::CBSendTitleDataAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData || !pCustData)
		return FALSE;

	AgsmTitle *pThis = (AgsmTitle *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	UINT32 ulNID = *(UINT32 *) pCustData;

	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	if(!pcsCharacter->m_csTitle->m_bLoadedTitle || !pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest)
		return FALSE;

	for(IterTitle iter = pcsCharacter->m_csTitle->m_vtTitle.begin(); iter != pcsCharacter->m_csTitle->m_vtTitle.end(); ++iter)
	{
		PACKET_AGSP_TITLE_LIST_RESULT_RELAY pPacket(pThis->m_pAgsmCharacter->GetRealCharName(pcsCharacter->m_szID), TRUE, FALSE);

		pPacket.nTitleID = (*iter).lTitleID;
		pPacket.bUseTitle = (*iter).bUsingTitle;

		AuTimeStamp::ConvertTimeStampToOracleTime((*iter).lSetTime, pPacket.m_szDate, _MAX_DATETIME_LENGTH);

		AgsEngine::GetInstance()->SendPacket(pPacket, ulNID);
	}

	PACKET_AGSP_TITLE_LIST_RESULT_RELAY pPacketListEnd(pThis->m_pAgsmCharacter->GetRealCharName(pcsCharacter->m_szID), TRUE, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacketListEnd, ulNID);

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		PACKET_AGSP_TITLE_QUEST_LIST_RESULT pPacket(pThis->m_pAgsmCharacter->GetRealCharName(pcsCharacter->m_szID), TRUE, FALSE);

		pPacket.nTitleID = (*iter).m_nTitleTid;
		pPacket.bTitleQuestComplete = FALSE;

		int i = 0;
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			pPacket.nTitleQuestCheckValue[i] = (*iter2).m_nTitleCurrentValue;
			i++;
		}

		AuTimeStamp::ConvertTimeStampToOracleTime((*iter).m_nTitleQuestAcceptTime, pPacket.m_szDateAccept, _MAX_DATETIME_LENGTH);
		AuTimeStamp::ConvertTimeStampToOracleTime((*iter).m_nTitleQuestCompleteTime, pPacket.m_szDateComplete, _MAX_DATETIME_LENGTH);

		AgsEngine::GetInstance()->SendPacket(pPacket, ulNID);
	}

	PACKET_AGSP_TITLE_QUEST_LIST_RESULT pPacketQuestListEnd(pThis->m_pAgsmCharacter->GetRealCharName(pcsCharacter->m_szID), TRUE, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacketQuestListEnd, ulNID);

	return TRUE;
}

BOOL AgsmTitle::TitleListSend(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	if(!pcsCharacter->m_csTitle->m_bLoadedTitle)
		return FALSE;

	for(IterTitle iter = pcsCharacter->m_csTitle->m_vtTitle.begin(); iter != pcsCharacter->m_csTitle->m_vtTitle.end(); ++iter)
		MakeAndSendTitleListResultPacket(pcsCharacter, (*iter).lTitleID, (*iter).lSetTime, (*iter).bUsingTitle, FALSE, TRUE);

	MakeAndSendTitleListResultPacket(pcsCharacter, 0, 0, FALSE, TRUE, TRUE);

	return TRUE;
}

BOOL AgsmTitle::TitleQuestListSend(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest)
		return FALSE;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		INT32 nCheckValue[AGPDTITLE_MAX_TITLE_CHECK];
		
		INT32 i = 0;

		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			nCheckValue[i] = (*iter2).m_nTitleCurrentValue;
			i++;
		}

		MakeAndSendTitleQuestListResultPacket(pcsCharacter, (*iter).m_nTitleTid, nCheckValue, FALSE, 
			(*iter).m_nTitleQuestAcceptTime, (*iter).m_nTitleQuestCompleteTime, FALSE, TRUE);
	}

	MakeAndSendTitleQuestListResultPacket(pcsCharacter, 0, NULL, 0, 0, 0, TRUE, TRUE);

	return TRUE;
}

BOOL AgsmTitle::UsingTitleSendToClient(AgpdCharacter* pcsCharacter)
{
	if(!m_pAgpmTitle)
		return FALSE;

	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	if(!pcsCharacter->m_csTitle->m_bLoadedTitle)
		return FALSE;

	INT32 nTitleTid = pcsCharacter->m_csTitle->GetUseTitle();
	IterTitle iterTitle = pcsCharacter->m_csTitle->FindByTitleID(nTitleTid);

	if(nTitleTid == 0) //Not using a Title
		return FALSE;

	TitleUnUse(pcsCharacter, (*iterTitle).lTitleID, (*iterTitle).lSetTime);

	TitleUse(pcsCharacter, (*iterTitle).lTitleID, (*iterTitle).lSetTime);
	MakeAndSendTitleUseResultPacket(pcsCharacter, (*iterTitle).lTitleID, TRUE, (*iterTitle).bUsingTitle, (*iterTitle).lSetTime);
	//MakeAndSendTitleUseNearPacket(pcsCharacter, (*iterTitle).lTitleID, (*iterTitle).bUsingTitle);

	return TRUE;
}

BOOL AgsmTitle::TitleAddRequest(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(lTitleID))
		return FALSE;

	if(pcsCharacter->m_csTitle->IsFullTitle())
		return FALSE;

	if(!TitleRewardProcess(pcsCharacter, lTitleID))
		return FALSE;

	WriteLogTitle(AGSMTITLE_LOG_TITLE_ADD, pcsCharacter, lTitleID);

	return MakeAndSendTitleAddRelayPacket(pcsCharacter, lTitleID);
}

BOOL AgsmTitle::TitleUseRequest(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bUse)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);
	UINT32 lTimeStamp = AuTimeStamp::GetCurrentTimeStamp();

	if(!pTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsHaveTitle(lTitleID))
		return MakeAndSendTitleUseResultPacket(pcsCharacter, lTitleID, FALSE, bUse, lTimeStamp, AGPMTITLE_USE_FAIL_DONT_HAVE_TITLE);

	if(pcsCharacter->m_csTitle->IsUsingTitle(lTitleID) == bUse)
		return MakeAndSendTitleUseResultPacket(pcsCharacter, lTitleID, FALSE, bUse, lTimeStamp, AGPMTITLE_USE_FAIL_ALREADY_USE);
	
	if(bUse)
	{
		// 2011. 12. 21. silvermoo
		// 타이틀 재사용시 쿨타임 제거
// 		if(AuTimeStamp::AddTime(pcsCharacter->m_csTitle->GetLastSetTime(), 0, 1, 0, 0) > lTimeStamp)
// 			return MakeAndSendTitleUseResultPacket(pcsCharacter, lTitleID, FALSE, bUse, lTimeStamp, AGPMTITLE_USE_FAIL_NOT_ENOUGH_TIME);

		INT32 lRecentTitleTid = pcsCharacter->m_csTitle->GetUseTitle();

		WriteLogTitle(AGSMTITLE_LOG_TITLE_USE, pcsCharacter, lTitleID);

		return MakeAndSendTitleUseRelayPacket(pcsCharacter, lTitleID, lRecentTitleTid);
	}
	else
	{
		WriteLogTitle(AGSMTITLE_LOG_TITLE_USE, pcsCharacter, 0);

		return MakeAndSendTitleUseRelayPacket(pcsCharacter, 0, lTitleID);
	}
}

BOOL AgsmTitle::TitleDeleteRequest(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsHaveTitle(lTitleID) && !pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(lTitleID))
		return FALSE;

	WriteLogTitle(AGSMTITLE_LOG_TITLE_DELETE, pcsCharacter, 0);

	return MakeAndSendTitleDeleteRelayPacket(pcsCharacter, lTitleID);
}

BOOL AgsmTitle::TitleQuestCheck(AgpdCharacter* pcsCharacter, INT32 lTitleID, INT32 lCheckType, INT32 lCheckSet, INT32 lNewValue)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest || !m_pAgpmTitle)
		return FALSE;

	if(!m_pAgpmTitle->SetTitleQuestCheckValue(pcsCharacter, lTitleID, lCheckType, lCheckSet, lNewValue))
		return FALSE;

	return MakeAndSendTitleQuestCheckRelayPacket(pcsCharacter, lTitleID);
}


BOOL AgsmTitle::SetTitleQuestStatusCheckValue(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pTitleTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	IterCurrentTitleQuest currentTitleQuest = pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(lTitleID);

	if(!&currentTitleQuest)
		return FALSE;

	INT32	lValue	= 0;

	for(IterTitleCurrentCheck iter = (*currentTitleQuest).m_vtTitleCurrentCheck.begin(); iter != (*currentTitleQuest).m_vtTitleCurrentCheck.end(); ++iter)
	{
		(*iter).m_nTitleCurrentValue = m_pAgpmTitle->GetTitleQuestCheckValueInStatus(pcsCharacter, (*iter).m_nTitleCheckType);
		TitleQuestCheck(pcsCharacter, lTitleID, (*iter).m_nTitleCheckType, (*iter).m_nTitleCheckSet, (*iter).m_nTitleCurrentValue);
	}

	return TRUE;
}

BOOL AgsmTitle::TitleRewardProcess(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pTitleTemplate)
		return FALSE;

	BOOL bResult = TRUE;

	if(pTitleTemplate->m_stReward.m_nRewardMoney != 0 && !m_pAgpmCharacter->AddMoney(pcsCharacter, pTitleTemplate->m_stReward.m_nRewardMoney))
		bResult = FALSE;

	for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_REWARD_ITEM; i++)
	{
		if(pTitleTemplate->m_stReward.m_nRewardItemTid[i] > 0)
		{
			AgpdItem *pcsItem = m_pAgsmItemManager->CreateItem(pTitleTemplate->m_stReward.m_nRewardItemTid[i], pcsCharacter);

			if(!pcsItem)
				bResult = FALSE;

			if(!m_pAgpmItem->AddItemToInventory(pcsCharacter, pcsItem))
				bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AgsmTitle::RobCostItem(AgpdCharacter *pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pTitleTemplate)
		return FALSE;

	if(pTitleTemplate->m_stRequireCondition.m_nRequireGheld != 0 && !m_pAgpmCharacter->SubMoney(pcsCharacter, pTitleTemplate->m_stRequireCondition.m_nRequireGheld))
		return FALSE;

	for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_REQUIRE_ITEM; i++)
	{
		if(pTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][0] != 0 && pTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][2] != 1)
		{
			if(!RobItem(pcsCharacter, pTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][0], pTitleTemplate->m_stRequireCondition.m_nRequireItemTid[i][1]))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmTitle::RobItem(AgpdCharacter *pcsCharacter, INT32 nItemTid, INT32 nItemCount)
{
	if(!pcsCharacter)
		return FALSE;
	
	INT32 lCount1 = nItemCount;
	if(lCount1 > 0)
	{
		AgpdGrid* pagpdGrid1 = m_pAgpmItem->GetInventory(pcsCharacter);
		if (pagpdGrid1)
		{
			lCount1 = RobItem(pcsCharacter, pagpdGrid1, nItemTid, nItemCount);
		}
	}

	INT32 lCount2 = lCount1;
	if(lCount2 > 0)
	{
		AgpdGrid* pagpdGrid2 = m_pAgpmItem->GetCashInventoryGrid(pcsCharacter);
		if (pagpdGrid2) 
		{
			lCount2 = RobItem(pcsCharacter, pagpdGrid2, nItemTid, lCount1);
		}
	}

	if(lCount2 > 0)
		return FALSE;

	return TRUE;
}

INT32 AgsmTitle::RobItem(AgpdCharacter* pcsCharacter, AgpdGrid* pagpdGrid, INT32 ItemTID, INT32 Count)
{
	AgpdGridItem* pagpdGridItem;
	AgpdItem *pAgpdItem;

	INT32 lIndex = 0;
	INT32 lCount = Count;
	while ((0 < lCount) && (pagpdGridItem = m_pAgpmGrid->GetItemByTemplate(lIndex, pagpdGrid, AGPDGRID_ITEM_TYPE_ITEM, ItemTID)))
	{
		pAgpdItem = m_pAgpmItem->GetItem(pagpdGridItem->m_lItemID);
		if (!pAgpdItem) continue;

		if (((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_bStackable)
		{
			if (pAgpdItem->m_nCount < lCount)
			{
				// pAgpdItem이 목표수량보다 적다면 수량만큼 없앤다.
				lCount -= pAgpdItem->m_nCount;
				
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
											pAgpdItem,
											pAgpdItem->m_nCount
											);					
				
				m_pAgpmItem->SubItemStackCount(pAgpdItem, pAgpdItem->m_nCount);
			}
			else
			{
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
											pAgpdItem,
											lCount
											);
			
				m_pAgpmItem->SubItemStackCount(pAgpdItem, lCount);
				
				lCount = 0;
			}
		}
		else
		{
			// log. 아이템이 사라질 수 있어 미리 남긴다.
			m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_ROB, pcsCharacter->m_lID,
										pAgpdItem,
										pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
										);
		
			--lCount;

			m_pAgpmItem->RemoveItem(pAgpdItem, TRUE);
		}
	}

	return lCount;
}

BOOL AgsmTitle::SkillCast(AgpdCharacter* pcsCharacter, INT32 SkillTid)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = m_pAgpmSkill->GetSkillTemplate(SkillTid);
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = 1;

	INT32 lAdditionalSkillTID = (INT32)pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel, m_csRandom.randInt(100));

	if(lAdditionalSkillTID)
	{
		AgpdSkillTemplate* pcsSkillTemplate2 = m_pAgpmSkill->GetSkillTemplate(lAdditionalSkillTID);

		INT32 SkillTID2 = (INT32)pcsSkillTemplate2->GetAdditionalSkillTID(lSkillLevel, m_csRandom.randInt(100));
		if(SkillTID2)
			m_pAgsmSkill->CastSkill(pcsCharacter, SkillTID2, lSkillLevel, NULL, FALSE, FALSE);
		else
			m_pAgsmSkill->CastSkill(pcsCharacter, lAdditionalSkillTID, lSkillLevel, NULL, FALSE, FALSE);
	}
	else
		m_pAgsmSkill->CastSkill(pcsCharacter, SkillTid, lSkillLevel, NULL, FALSE, FALSE);

	return TRUE;
}

BOOL AgsmTitle::TitleEffectCast(AgpdCharacter* pcsCharacter, INT32 TitleTid)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(TitleTid);

	if(!pcsTitleTemplate)
		return FALSE;

	for(IterTitleSkillEffect iter = pcsTitleTemplate->m_vtTitleSkillEffect.begin(); iter != pcsTitleTemplate->m_vtTitleSkillEffect.end(); ++iter)
		SkillCast(pcsCharacter, (*iter).m_nSkillTid);

	return TRUE;
}

BOOL AgsmTitle::TitleEffectEnd(AgpdCharacter* pcsCharacter, INT32 TitleTid)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(TitleTid);

	if(!pcsTitleTemplate)
		return FALSE;

	for(IterTitleSkillEffect iter = pcsTitleTemplate->m_vtTitleSkillEffect.begin(); iter != pcsTitleTemplate->m_vtTitleSkillEffect.end(); ++iter)
	{
		AgpdSkill *pcsSKill = m_pAgpmSkill->GetSkillByTID(pcsCharacter, (*iter).m_nSkillTid);
		m_pAgsmSkill->EndSkill(pcsSKill);
	}

	return TRUE;
}

INT32 AgsmTitle::GetEffectiveSkillTid(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitle || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pcsCharacter->m_csTitle->GetUseTitle());

	if(!pcsTitleTemplate)
		return FALSE;

	for(IterTitleSkillEffect iter = pcsTitleTemplate->m_vtTitleSkillEffect.begin(); iter != pcsTitleTemplate->m_vtTitleSkillEffect.end(); ++iter)
	{
		if((*iter).m_nSkillTid != 0)
			return (*iter).m_nSkillTid;
	}
	
	return FALSE;
}

BOOL AgsmTitle::TitleUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 timeStamp)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(!m_pAgpmTitle->TitleUse(pcsCharacter, lTitleID, timeStamp))
		return FALSE;

	TitleEffectCast(pcsCharacter, lTitleID);

	return TRUE;
}

BOOL AgsmTitle::TitleUnUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 timeStamp)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(!m_pAgpmTitle->TitleUnUse(pcsCharacter, lTitleID, timeStamp))
		return FALSE;

	return TitleEffectEnd(pcsCharacter, lTitleID);
}

BOOL AgsmTitle::MonsterDeathByCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMonster)
{
	if(!pcsCharacter || !pcsMonster)
		return FALSE;

	return UpdateTitleQuestCheckValue(pcsCharacter, AGPMTITLE_TITLE_CHECK_TYPE_NATURAL_ENEMY, pcsMonster->m_pcsCharacterTemplate->m_lID, 1);
}

BOOL AgsmTitle::MonsterDeathByParty(AgpdParty *pcsParty, AgpdCharacter* pcsMonster)
{
	if(!pcsParty || !pcsMonster)
		return FALSE;

	ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER> pcsCombatMember;
	pcsCombatMember.MemSetAll();

	INT32	lMemberTotalLevel	= 0;

	INT32 lNumCombatMember = m_pAgsmParty->GetNearMember(pcsParty, pcsMonster, &pcsCombatMember[0], &lMemberTotalLevel);
	if (lNumCombatMember < 1)
		return FALSE;

	for (int i = 0; i < lNumCombatMember; ++i)
		MonsterDeathByCharacter(pcsCombatMember[i], pcsMonster);

	return TRUE;
}

BOOL AgsmTitle::QuestComplete(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	return UpdateTitleQuestCheckValue(pcsCharacter, AGPMTITLE_TITLE_CHECK_TYPE_QUEST_COMPLETE_NUMBER, 0, 1);
}

BOOL AgsmTitle::CharacterLevelUp(AgpdCharacter* pcsCharacter, INT32 lChangeLevel)
{
	if(!pcsCharacter)
		return FALSE;

	return UpdateTitleQuestCheckValue(pcsCharacter, AGPMTITLE_TITLE_CHECK_TYPE_ADVANCED);
}

BOOL AgsmTitle::RecalcFactor(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !m_pAgpmCharacter)
		return FALSE;

	if(!m_pAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	if(m_pAgpmCharacter->IsNPC(pcsCharacter))
		return FALSE;

	BOOL bResult = FALSE;

	if(!pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest)
		return FALSE;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			if((*iter2).m_nTitleCheckType >= AGPMTITLE_TITLE_CHECK_TYPE_MAGICTYPEATTACK && (*iter2).m_nTitleCheckType <= AGPMTITLE_TITLE_CHECK_TYPE_EARTHTYPEATTACK)
			{
				(*iter2).m_nTitleCurrentValue = m_pAgpmTitle->GetTitleQuestCheckValueInStatus(pcsCharacter, (*iter2).m_nTitleCheckType);
				bResult = TitleQuestCheck(pcsCharacter, (*iter).m_nTitleTid, (*iter2).m_nTitleCheckType, (*iter2).m_nTitleCheckSet, (*iter2).m_nTitleCurrentValue);
			}
		}
	}

	return bResult;
}

BOOL AgsmTitle::OnGather(AgpdCharacter* pcsCharacter, AgpdSkill* pcsSkill, ApBase* pTarget)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !pcsSkill || !pTarget)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest)
		return FALSE;

	return UpdateTitleQuestCheckValue(pcsCharacter, AGPMTITLE_TITLE_CHECK_TYPE_NUMBER_OF_GATHER, ((AgpdCharacter *)pTarget)->m_pcsCharacterTemplate->m_lID, 1);
}

BOOL AgsmTitle::OnUpdateSkillPoint(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	BOOL bResult = FALSE;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			if((*iter2).m_nTitleCheckType >= AGPMTITLE_TITLE_CHECK_TYPE_LEVEL_OF_ALCHEMIST && (*iter2).m_nTitleCheckType <= AGPMTITLE_TITLE_CHECK_TYPE_LEVEL_OF_HUNT)
			{
				(*iter2).m_nTitleCurrentValue = m_pAgpmTitle->GetTitleQuestCheckValueInStatus(pcsCharacter, (*iter2).m_nTitleCheckType);
				bResult = TitleQuestCheck(pcsCharacter, (*iter).m_nTitleTid, (*iter2).m_nTitleCheckType, (*iter2).m_nTitleCheckSet, (*iter2).m_nTitleCurrentValue);
			}
		}
	}

	return bResult;
}

BOOL AgsmTitle::OnPickUpItemResult(AgpdCharacter* pcsCharacter, AgpdItemTemplate *pcsItemTemplate, INT32 lItemCount)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	if(!pcsItemTemplate)
		return FALSE;

	if(lItemCount < 1)
		return FALSE;

	BOOL bResult = FALSE;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			if((*iter2).m_nTitleCheckType == AGPMTITLE_TITLE_CHECK_TYPE_COLLECTOR && m_pAgpmTitle->CheckSameItemGroup((*iter2).m_nTitleCheckSet, pcsItemTemplate->m_lID))
			{
				(*iter2).m_nTitleCurrentValue += lItemCount;
				bResult = TitleQuestCheck(pcsCharacter, (*iter).m_nTitleTid, (*iter2).m_nTitleCheckType, (*iter2).m_nTitleCheckSet, (*iter2).m_nTitleCurrentValue);
			}
		}
	}

	return bResult;
}

BOOL AgsmTitle::OnPK(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;
	
	return UpdateTitleQuestCheckValue(pcsCharacter, AGPMTITLE_TITLE_CHECK_TYPE_MERDER, 0, 1);
}

BOOL AgsmTitle::OnItemUse(AgpdCharacter* pcsCharacter,	AgpdItem* pcsItem)
{
	if(!pcsCharacter || !pcsItem)
		return FALSE;

	return UpdateTitleQuestCheckValue(pcsCharacter, AGPMTITLE_TITLE_CHECK_TYPE_ADDICT, pcsItem->m_lTID, 1);
}

BOOL AgsmTitle::OnEnterGameWorld(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(m_pAgpmCharacter->IsPC(pcsCharacter) == FALSE)
		return TRUE;

	if(m_pAgpmCharacter->IsNPC(pcsCharacter))
		return TRUE;

	MakeAndSendTitleListRelayPacket(pcsCharacter);
	MakeAndSendTitleQuestListRelayPacket(pcsCharacter);

	return TRUE;
}

BOOL AgsmTitle::UpdateTitleQuestCheckValue(AgpdCharacter* pcsCharacter, INT32 lCheckType)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	BOOL bResult = FALSE;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			if((*iter2).m_nTitleCheckType == lCheckType)
			{
				(*iter2).m_nTitleCurrentValue = m_pAgpmTitle->GetTitleQuestCheckValueInStatus(pcsCharacter, (*iter2).m_nTitleCheckType);
				bResult = TitleQuestCheck(pcsCharacter, (*iter).m_nTitleTid, (*iter2).m_nTitleCheckType, (*iter2).m_nTitleCheckSet, (*iter2).m_nTitleCurrentValue);
			}
		}
	}

	return bResult;
}

BOOL AgsmTitle::UpdateTitleQuestCheckValue(AgpdCharacter* pcsCharacter, INT32 lCheckType, INT32 lCheckSet, INT32 lAddValue)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	BOOL bResult = FALSE;

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			if((*iter2).m_nTitleCheckType == lCheckType && (*iter2).m_nTitleCheckSet == lCheckSet)
			{
				(*iter2).m_nTitleCurrentValue += lAddValue;
				bResult = TitleQuestCheck(pcsCharacter, (*iter).m_nTitleTid, (*iter2).m_nTitleCheckType, (*iter2).m_nTitleCheckSet, (*iter2).m_nTitleCurrentValue);
			}
		}
	}

	return bResult;
}

BOOL AgsmTitle::TitleAddResult(PACKET_AGSP_TITLE_ADD_RESULT_RELAY *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(pPacket->nAddResult == TRUE && pcsCharacter->m_csTitle->AddTitle(pPacket->nTitleID))
		bResult = TRUE;

	MakeAndSendTitleAddResultPacket(pcsCharacter, pPacket->nTitleID, bResult);

	return bResult;
}

BOOL AgsmTitle::TitleUseResult(PACKET_AGSP_TITLE_USE_RESULT_RELAY *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	UINT32 timeStamp = CurrentTimeStampStringToTimeStamp(pPacket->m_szDate);

	if(pPacket->nUseResult == TRUE)
	{
		if(pPacket->bUse)
			bResult = TitleUse(pcsCharacter, pPacket->nTitleID, timeStamp);
		else
			bResult = TitleUnUse(pcsCharacter, pPacket->nTitleID, timeStamp);
	}

	UINT32 lFailReason = AGPMTITLE_USE_SUCCESS;

	if(!bResult)
		lFailReason = AGPMTITLE_USE_FAIL_DONT_KNOW_REASON;

	MakeAndSendTitleUseResultPacket(pcsCharacter, pPacket->nTitleID, bResult, pPacket->bUse, timeStamp, lFailReason);
	MakeAndSendTitleUseNearPacket(pcsCharacter, pPacket->nTitleID, pPacket->bUse);

	return bResult;
}

BOOL AgsmTitle::TitleListResult(PACKET_AGSP_TITLE_LIST_RESULT_RELAY *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle)
		pcsCharacter->m_csTitle = new AgpdTitle();

	UINT32 timeStamp = AuTimeStamp::ConvertOracleTimeToTimeStamp(pPacket->m_szDate);

	pcsCharacter->m_csTitle->m_bLoadedTitle = TRUE;

	if(pPacket->bListResult && !pPacket->bTitleListEnd)
	{
		AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

		if(!pTitleTemplate)
			return FALSE;

		if(!pcsCharacter->m_csTitle->AddTitle(pPacket->nTitleID))
			return FALSE;

		if(pPacket->bUseTitle)
			m_pAgpmTitle->TitleUse(pcsCharacter, pPacket->nTitleID, timeStamp);
		else
		{
			if(!pcsCharacter->m_csTitle->UpdateUsingTitle(pPacket->nTitleID, FALSE) || !pcsCharacter->m_csTitle->UpdateSetTime(pPacket->nTitleID, timeStamp))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmTitle::TitleDeleteResult(PACKET_AGSP_TITLE_DELETE_RESULT *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter || !pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(pPacket->bTitleDeleteResult)
	{
		if(pcsCharacter->m_csTitle->IsHaveTitle(pPacket->nTitleID) 
			&& pcsCharacter->m_csTitle->RemoveTitle(pPacket->nTitleID))
			bResult = TRUE;

		if(pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pPacket->nTitleID) 
			&& pcsCharacter->m_csTitleQuest->RemoveTitleQuest(pPacket->nTitleID))
			bResult = TRUE;
	}

	MakeAndSendTitleDeleteResultPacket(pcsCharacter, pPacket->nTitleID, pPacket->bTitleDeleteResult);

	return bResult;
}

BOOL AgsmTitle::TitleQuestRequestResult(PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	BOOL bResult = FALSE;

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

	stTitleQuest.m_nTitleQuestAcceptTime = CurrentTimeStampStringToTimeStamp(pPacket->m_szDate);

	if(pPacket->bTitleQuestRequestResult == TRUE && pcsCharacter->m_csTitleQuest->AddTitleQuest(stTitleQuest))
		bResult = TRUE;

	MakeAndSendTitleQuestRequestResultPacket(pcsCharacter, pPacket->nTitleID, stTitleQuest.m_nTitleQuestAcceptTime, bResult);

	SetTitleQuestStatusCheckValue(pcsCharacter, pPacket->nTitleID);

	return bResult;
}

BOOL AgsmTitle::TitleQuestCheckResult(PACKET_AGSP_TITLE_QUEST_CHECK_RESULT *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	return MakeAndSendTitleQuestCheckResultPacket(pcsCharacter, pPacket->nTitleID, pPacket->bTitleQuestCheckResult);
}

BOOL AgsmTitle::TitleQuestCompleteResult(PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

	if(!pTitleTemplate)
		return FALSE;

	UINT32 m_nTitleQuestCompleteTime = CurrentTimeStampStringToTimeStamp(pPacket->m_szDate);

	IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(pPacket->nTitleID);

	if(!&iter)
		return FALSE;

	if(pPacket->bTitleQuestCompleteResult)
	{
		(*iter).m_nTitleQuestCompleteTime = m_nTitleQuestCompleteTime;

		if(pcsCharacter->m_csTitleQuest->RemoveTitleQuest(pPacket->nTitleID))
			TitleAddRequest(pcsCharacter, pPacket->nTitleID);
	}

	MakeAndSendTitleQuestCompleteResultPacket(pcsCharacter, pPacket->nTitleID, m_nTitleQuestCompleteTime, pPacket->bTitleQuestCompleteResult);

	return TRUE;
}

BOOL AgsmTitle::TitleQuestListResult(PACKET_AGSP_TITLE_QUEST_LIST_RESULT *pPacket)
{
	if(!pPacket)
		return FALSE;

	if(!m_pAgpmCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket->strCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest)
		pcsCharacter->m_csTitleQuest = new AgpdTitleQuest();

	BOOL bResult = FALSE;

	if(pPacket->bTitleQuestListResult && !pPacket->bListEnd)
	{
		AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(pPacket->nTitleID);

		if(!pTitleTemplate)
			return FALSE;

		if(&pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(pPacket->nTitleID))
			return FALSE;

		AgpdCurrentTitleQuest stTitleQuest;

		stTitleQuest.m_nTitleTid = pTitleTemplate->m_nTitleTid;

		stTitleQuest.m_nTitleQuestAcceptTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(pPacket->m_szDateAccept);
		stTitleQuest.m_nTitleQuestCompleteTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(pPacket->m_szDateComplete);

		INT32 i = 0;

		for(IterTitleCheck iter = pTitleTemplate->m_vtTitleCheck.begin(); iter != pTitleTemplate->m_vtTitleCheck.end(); ++iter)
		{
			AgpdTitleCurrentCheck stTitleCheck;

			stTitleCheck.m_nTitleCheckType = (*iter).m_nTitleCheckType;
			stTitleCheck.m_nTitleCheckSet = (*iter).m_nTitleCheckSet;
			stTitleCheck.m_nTitleCheckValue = (*iter).m_nTitleCheckValue;
			stTitleCheck.m_nTitleCurrentValue = pPacket->nTitleQuestCheckValue[i];
			i++;

			stTitleQuest.m_vtTitleCurrentCheck.push_back(stTitleCheck);
		}

		if(!pPacket->bTitleQuestComplete)
			bResult = pcsCharacter->m_csTitleQuest->AddTitleQuest(stTitleQuest);
	}

	pcsCharacter->m_csTitleQuest->m_bLoadedTitleQuest = TRUE;

	return bResult;
}

BOOL AgsmTitle::GetSelectTitleResult(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter)
{
	if(!pDatabase || !pcsCharacter || !pcsCharacter->m_csTitle)
		return FALSE;

	CHAR *szBuffer = NULL;

	INT32 nTitleTid = 0;
	CHAR tempCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR tempDate[_MAX_DATETIME_LENGTH+1];
	BOOL bUseTitle = FALSE;

	if((szBuffer = pDatabase->GetQueryResult(0)) != NULL)
		nTitleTid = (INT32) atoi(szBuffer);
	if((szBuffer = pDatabase->GetQueryResult(1)) != NULL)
		strncpy(tempCharName, szBuffer, AGPACHARACTER_MAX_ID_STRING);
	if((szBuffer = pDatabase->GetQueryResult(2)) != NULL)
		strncpy(tempDate, szBuffer, _MAX_DATETIME_LENGTH);
	if((szBuffer = pDatabase->GetQueryResult(3)) != NULL)
		bUseTitle = (INT32) atoi(szBuffer);

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(nTitleTid);

	if(!pTitleTemplate)	
		return FALSE;

	if(!pcsCharacter->m_csTitle->AddTitle(nTitleTid))
		return FALSE;

	UINT32 timeStamp = AuTimeStamp::ConvertOracleTimeToTimeStamp(tempDate);

	if(bUseTitle)
		m_pAgpmTitle->TitleUse(pcsCharacter, nTitleTid, timeStamp);
	else
	{
		if(!pcsCharacter->m_csTitle->UpdateUsingTitle(nTitleTid, FALSE) || !pcsCharacter->m_csTitle->UpdateSetTime(nTitleTid, timeStamp))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmTitle::GetSelectTitleQuestResult(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter)
{
	if(!pDatabase || !pcsCharacter || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	CHAR *szBuffer = NULL;

	INT32 nTitleTid = 0;
	CHAR tempCharName[AGPACHARACTER_MAX_ID_STRING+1];
	BOOL bComplete = FALSE;
	INT32 nTitleCheckValue[AGPDTITLE_MAX_TITLE_CHECK];
	CHAR tempAcceptDate[_MAX_DATETIME_LENGTH+1];
	CHAR tempCompleteDate[_MAX_DATETIME_LENGTH+1];

	if((szBuffer = pDatabase->GetQueryResult(0)) != NULL)
		nTitleTid = (INT32) atoi(szBuffer);
	if((szBuffer = pDatabase->GetQueryResult(1)) != NULL)
		strncpy(tempCharName, szBuffer, AGPACHARACTER_MAX_ID_STRING);
	if((szBuffer = pDatabase->GetQueryResult(2)) != NULL)
		bComplete = (INT32) atoi(szBuffer);

	for(int i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
	{
		if((szBuffer = pDatabase->GetQueryResult(i+3)) != NULL)
		nTitleCheckValue[i] = (INT32) atoi(szBuffer);
	}

	if((szBuffer = pDatabase->GetQueryResult(8)) != NULL)
		strncpy(tempAcceptDate, szBuffer, _MAX_DATETIME_LENGTH);
	if((szBuffer = pDatabase->GetQueryResult(9)) != NULL)
		strncpy(tempCompleteDate, szBuffer, _MAX_DATETIME_LENGTH);

	AgpdTitleTemplate *pTitleTemplate = m_pAgpmTitle->GetTitleTemplate(nTitleTid);

	if(!pTitleTemplate)	
		return FALSE;

	if(&pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(nTitleTid))
		return FALSE;

	AgpdCurrentTitleQuest stTitleQuest;

	stTitleQuest.m_nTitleTid = nTitleTid;

	stTitleQuest.m_nTitleQuestAcceptTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(tempAcceptDate);
	stTitleQuest.m_nTitleQuestCompleteTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(tempCompleteDate);

	INT32 i = 0;

	for(IterTitleCheck iter = pTitleTemplate->m_vtTitleCheck.begin(); iter != pTitleTemplate->m_vtTitleCheck.end(); ++iter)
	{
		AgpdTitleCurrentCheck stTitleCheck;

		stTitleCheck.m_nTitleCheckType = (*iter).m_nTitleCheckType;
		stTitleCheck.m_nTitleCheckSet = (*iter).m_nTitleCheckSet;
		stTitleCheck.m_nTitleCheckValue = (*iter).m_nTitleCheckValue;
		stTitleCheck.m_nTitleCurrentValue = nTitleCheckValue[i];
		i++;

		stTitleQuest.m_vtTitleCurrentCheck.push_back(stTitleCheck);
	}

	if(!bComplete)
		pcsCharacter->m_csTitleQuest->AddTitleQuest(stTitleQuest);

	return TRUE;
}

BOOL AgsmTitle::WriteLogTitle(INT32 nLogType, AgpdCharacter *pcsCharacter, INT32 nTitleTid)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	switch(nLogType)
	{
	case AGSMTITLE_LOG_TITLEQUEST_ADD:
		{
			return m_pAgpmLog->WriteLog_TitleQuestAdd(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLEQUEST_COMPLETE:
		{
			return m_pAgpmLog->WriteLog_TitleQuestComplete(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLE_ADD:
		{
			return m_pAgpmLog->WriteLog_TitleAdd(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLE_USE:
		{
			return m_pAgpmLog->WriteLog_TitleUse(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLE_DELETE:
		{
			return m_pAgpmLog->WriteLog_TitleDelete(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLE_ADD_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleAddByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLE_DELETE_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleDeleteByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLE_EDIT_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleEditByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLEQUEST_ADD_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleQuestAddByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLEQUEST_EDIT_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleQuestEditByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLEQUEST_DELETE_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleQuestDeleteByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	case AGSMTITLE_LOG_TITLEQUEST_COMPLETE_BY_GM:
		{
			return m_pAgpmLog->WriteLog_TitleQuestCompleteByGm(0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
									m_pAgpmCharacter->GetLevel(pcsCharacter),
									m_pAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									nTitleTid,
									NULL);
		}
		break;
	}
	
	return TRUE;
}

UINT32 AgsmTitle::CurrentTimeStampStringToTimeStamp(TCHAR *szTimeBuf)
{
	if(!szTimeBuf)
		return 0;

	if(strlen(szTimeBuf) != AUTIMESTAMP_SIZE_TIME_STRING)
		return 0;

	TCHAR szTmp[32];
	memset(szTmp, 0, sizeof(szTmp));

	struct tm timeptr;
	memset(&timeptr, 0, sizeof(timeptr));

	memcpy(szTmp, szTimeBuf, 4);
	szTmp[4] = '\0';
	timeptr.tm_year = atoi(szTmp) - 1900;

	memcpy(szTmp, &szTimeBuf[4], 2);
	szTmp[2] = '\0';
	timeptr.tm_mon = atoi(szTmp) - 1;			// January == 0 이므로 1 빼줌.

	memcpy(szTmp, &szTimeBuf[6], 2);
	szTmp[2] = '\0';
	timeptr.tm_mday = atoi(szTmp);

	memcpy(szTmp, &szTimeBuf[8], 2);
	szTmp[2] = '\0';
	timeptr.tm_hour = atoi(szTmp);

	memcpy(szTmp, &szTimeBuf[10], 2);
	szTmp[2] = '\0';
	timeptr.tm_min = atoi(szTmp);

	timeptr.tm_isdst = -1;

	UINT32 timeval = (UINT32)mktime(&timeptr);

	return (timeval != -1) ? timeval : 0;
}

BOOL AgsmTitle::MakeAndSendTitleAddRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	PACKET_AGSP_TITLE_ADD_RELAY pPacket(pcsCharacter->m_szID, lTitleID);

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleUseRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, INT32 nPauseTitleTid)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);
	AgpdTitleTemplate *pcsTitleTemplate2 = m_pAgpmTitle->GetTitleTemplate(nPauseTitleTid);

	if(!pcsTitleTemplate && !pcsTitleTemplate2)
		return FALSE;

	CHAR szBuf[_MAX_DATETIME_LENGTH + 1];
	memset(szBuf, 0, sizeof(szBuf));
	AuTimeStamp::GetCurrentTimeStampString(szBuf, _MAX_DATETIME_LENGTH);

	PACKET_AGSP_TITLE_USE_RELAY pPacket(pcsCharacter->m_szID, lTitleID, szBuf);
	pPacket.nPauseTitleTid = nPauseTitleTid;

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleListRelayPacket(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	PACKET_AGSP_TITLE_LIST_RELAY pPacket(pcsCharacter->m_szID);

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleDeleteRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	PACKET_AGSP_TITLE_DELETE pPacket(pcsCharacter->m_szID, lTitleID);

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestRequestRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	CHAR szBuf[_MAX_DATETIME_LENGTH + 1];
	AuTimeStamp::GetCurrentTimeStampString(szBuf, _MAX_DATETIME_LENGTH);

	PACKET_AGSP_TITLE_QUEST_REQUEST pPacket(pcsCharacter->m_szID, lTitleID);

	strncpy(pPacket.m_szDate, szBuf, _MAX_DATETIME_LENGTH);

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestCheckRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	PACKET_AGSP_TITLE_QUEST_CHECK pPacket(pcsCharacter->m_szID, lTitleID);

	IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(lTitleID);

	if(!&iter)
		return FALSE;

	INT32 i = 0;

	for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		pPacket.nTitleCurrentValue[i] = (*iter2).m_nTitleCurrentValue;
		i++;
	}

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestCompleteRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;
	
	CHAR szBuf[_MAX_DATETIME_LENGTH + 1];
	AuTimeStamp::GetCurrentTimeStampString(szBuf, _MAX_DATETIME_LENGTH);

	PACKET_AGSP_TITLE_QUEST_COMPLETE pPacket(pcsCharacter->m_szID, lTitleID);

	strncpy(pPacket.m_szDate, szBuf, _MAX_DATETIME_LENGTH);

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestListRelayPacket(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	PACKET_AGSP_TITLE_QUEST_LIST pPacket(pcsCharacter->m_szID);

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pRelayServer->m_dpnidServer);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleAddResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bAddResult)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_ADD_RESULT pPacket(pcsCharacter->m_szID, lTitleID, bAddResult);

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleUseResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bUseResult, BOOL bUse, UINT32 lTimeStamp, UINT32 lFailReason)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_USE_RESULT pPacket(pcsCharacter->m_szID, lTitleID, bUseResult, bUse, lTimeStamp, lFailReason);

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleUseNearPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bUse)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(lTitleID !=0 && !pcsTitleTemplate)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_USE_NEAR pPacket;

	memcpy(pPacket.strCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
	pPacket.nTitleID = lTitleID;
	pPacket.bUse = bUse;

	UINT16	nLength = pPacket.unPacketLength + 1;

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);

	if (!pvPacketRaw) return FALSE;

	CopyMemory(pvPacketRaw, &pPacket, nLength);

	*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
	*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;
	
	((PACKET_HEADER *) pvPacketRaw)->unPacketLength		= nLength;
	((PACKET_HEADER *) pvPacketRaw)->lOwnerID			= 0;
	((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= 0;
	((PACKET_HEADER *) pvPacketRaw)->cType				= pPacket.cType;

	BOOL bResult = m_pAgsmAOIFilter->SendPacketNearExceptSelf(pvPacketRaw, nLength, pcsCharacter->m_stPos, m_pAgpmCharacter->GetRealRegionIndex(pcsCharacter), pcsAgsdCharacter->m_dpnidCharacter, PACKET_PRIORITY_5);

	return TRUE;
}

BOOL AgsmTitle::MakeAndSendTitleListResultPacket(AgpdCharacter* pcsCharacter, INT32 TitleID, UINT32 TimeStamp, BOOL UseTitle, BOOL TitleListEnd, BOOL ListResult)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_LIST_RESULT pPacket(pcsCharacter->m_szID, TitleID, TimeStamp, UseTitle, TitleListEnd, ListResult);

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleDeleteResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bDeleteResult)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_DELETE_RESULT pPacket(pcsCharacter->m_szID, lTitleID, bDeleteResult);

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestRequestResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 lTimeStamp, BOOL bQuestRequestResult)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_REQUEST_RESULT pPacket(pcsCharacter->m_szID, lTitleID, bQuestRequestResult);

	pPacket.lTimeStamp = lTimeStamp;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestCheckResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bQuestCheckResult)
{
	if(!pcsCharacter || !pcsCharacter->m_csTitleQuest || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_CHECK_RESULT pPacket(pcsCharacter->m_szID, lTitleID, bQuestCheckResult);
	
	IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(lTitleID);

	if(!&iter)
		return FALSE;

	INT32 i = 0;

	for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		pPacket.nTitleCurrentValue[i] = (*iter2).m_nTitleCurrentValue;
		i++;
	}

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestCompleteResultPacket(AgpdCharacter* pcsCharacter, INT32 TitleID, UINT32 lTimeStamp, BOOL bQuestCompleteResult)
{
	if(!pcsCharacter || !m_pAgpmTitle)
		return FALSE;

	AgpdTitleTemplate *pcsTitleTemplate = m_pAgpmTitle->GetTitleTemplate(TitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT pPacket(pcsCharacter->m_szID, TitleID, bQuestCompleteResult);

	pPacket.lTimeStamp = lTimeStamp;

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}

BOOL AgsmTitle::MakeAndSendTitleQuestListResultPacket(AgpdCharacter* pcsCharacter, INT32 TitleID, INT32 *nTitleCurrentValue, BOOL bComplete, UINT32 lAcceptTimeStamp, UINT32 lCompleteTimeStamp, BOOL bListEnd, BOOL bQuestListResult)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter * pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);

	if(!pcsAgsdCharacter)
		return FALSE;

	PACKET_AGPPTITLE_QUEST_LIST_RESULT pPacket(pcsCharacter->m_szID, TitleID, bQuestListResult);

	pPacket.bComplete = bComplete;
	pPacket.lAcceptTime = lAcceptTimeStamp;
	pPacket.lCompleteTime = lCompleteTimeStamp;

	pPacket.bListEnd = bListEnd;

	if(nTitleCurrentValue)
	{
		for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
			pPacket.nTitleCurrentValue[i] = nTitleCurrentValue[i];
	}

	BOOL bResult = AgsEngine::GetInstance()->SendPacket(pPacket, pcsAgsdCharacter->m_dpnidCharacter);

	return bResult;
}