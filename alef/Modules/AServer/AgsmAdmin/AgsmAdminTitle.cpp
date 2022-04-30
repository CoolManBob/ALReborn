#include "AgsmAdmin.h"
#include "AgsmTitle.h"

BOOL AgsmAdmin::ProcessTitleOperation(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
		return FALSE;

	BOOL bResult = TRUE;

	switch(pstTitle->m_cOperation)
	{
	case AGPMADMIN_TITLE_ADD:
		bResult = ProcessTitleOperationAdd(pstTitle, pcsAgpdCharacter);
		break;
	case AGPMADMIN_TITLE_USE:
		bResult = ProcessTitleOperationUse(pstTitle, pcsAgpdCharacter);
		break;
	case AGPMADMIN_TITLE_DELETE:
		bResult = ProcessTitleOperationDelete(pstTitle, pcsAgpdCharacter);
		break;
	case AGPMADMIN_TITLE_QUEST_ADD:
		bResult = ProcessTitleOperationQuestAdd(pstTitle, pcsAgpdCharacter);
		break;
	case AGPMADMIN_TITLE_QUEST_CHECK:
		bResult = ProcessTitleOperationQuestEdit(pstTitle, pcsAgpdCharacter);
		break;
	case AGPMADMIN_TITLE_QUEST_DELETE:
		bResult = ProcessTitleOperationQuestDelete(pstTitle, pcsAgpdCharacter);
		break;
	case AGPMADMIN_TITLE_QUEST_COMPLETE:
		bResult = ProcessTitleOperationQuestComplete(pstTitle, pcsAgpdCharacter);
		break;
	default:
		break;
	}

	return bResult;
}


BOOL AgsmAdmin::ProcessTitleOperationAdd(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;
	
	if(pcsCharacter->m_csTitle->IsHaveTitle(pstTitle->lTitleTid))
		return FALSE;

	if(pcsCharacter->m_csTitle->IsFullTitle())
		return FALSE;

	if(!m_pagsmTitle->TitleRewardProcess(pcsCharacter, pstTitle->lTitleTid))
		return FALSE;

	m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLE_ADD_BY_GM, pcsCharacter, pstTitle->lTitleTid);

	return m_pagsmTitle->MakeAndSendTitleAddRelayPacket(pcsCharacter, pstTitle->lTitleTid);
}

BOOL AgsmAdmin::ProcessTitleOperationUse(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsHaveTitle(pstTitle->lTitleTid))
		return FALSE;

	if(pcsCharacter->m_csTitle->IsUsingTitle(pstTitle->lTitleTid) == pstTitle->bUseTitle)
		return FALSE;

	if(pstTitle->bUseTitle)
	{
		UINT32 lTimeStamp = AuTimeStamp::GetCurrentTimeStamp();

		INT32 lRecentTitleTid = pcsCharacter->m_csTitle->GetUseTitle();

		m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLE_EDIT_BY_GM, pcsCharacter, pstTitle->lTitleTid);

		return m_pagsmTitle->MakeAndSendTitleUseRelayPacket(pcsCharacter, pstTitle->lTitleTid, lRecentTitleTid);
	}
	else
	{
		m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLE_EDIT_BY_GM, pcsCharacter, 0);

		return m_pagsmTitle->MakeAndSendTitleUseRelayPacket(pcsCharacter, 0, pstTitle->lTitleTid);
	}
}

BOOL AgsmAdmin::ProcessTitleOperationDelete(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitle->IsHaveTitle(pstTitle->lTitleTid))
		return FALSE;

	m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLE_DELETE_BY_GM, pcsCharacter, pstTitle->lTitleTid);

	return m_pagsmTitle->MakeAndSendTitleDeleteRelayPacket(pcsCharacter, pstTitle->lTitleTid);
}

BOOL AgsmAdmin::ProcessTitleOperationQuestAdd(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(pstTitle->lTitleTid))
		return FALSE;

	if(pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pstTitle->lTitleTid))
		return FALSE;

	if(pcsCharacter->m_csTitleQuest->IsFullTitleQuest())
		return FALSE;

	m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLEQUEST_ADD_BY_GM, pcsCharacter, pstTitle->lTitleTid);

	return m_pagsmTitle->MakeAndSendTitleQuestRequestRelayPacket(pcsCharacter, pstTitle->lTitleTid);
}

BOOL AgsmAdmin::ProcessTitleOperationQuestEdit(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(pstTitle->lTitleTid))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pstTitle->lTitleTid))
		return FALSE;

	IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->FindTitleQuestByTitleID(pstTitle->lTitleTid);

	if(!&iter) //Does this work? lol
		return FALSE;

	INT32 i = 0;

	IterTitleCurrentCheck iter2;

	for(iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		(*iter2).m_nTitleCurrentValue = pstTitle->CheckValue[i];
		i++;
	}

	m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLEQUEST_EDIT_BY_GM, pcsCharacter, pstTitle->lTitleTid);
	
	return m_pagsmTitle->MakeAndSendTitleQuestCheckRelayPacket(pcsCharacter, pstTitle->lTitleTid);
}

BOOL AgsmAdmin::ProcessTitleOperationQuestDelete(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pstTitle->lTitleTid))
		return FALSE;

	m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLEQUEST_DELETE_BY_GM, pcsCharacter, pstTitle->lTitleTid);

	return m_pagsmTitle->MakeAndSendTitleDeleteRelayPacket(pcsCharacter, pstTitle->lTitleTid);
}

BOOL AgsmAdmin::ProcessTitleOperationQuestComplete(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstTitle || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmTitle || !m_pagsmTitle)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstTitle->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	AgpdTitleTemplate *pTitleTemplate = m_pagpmTitle->GetTitleTemplate(pstTitle->lTitleTid);

	if(!pTitleTemplate)
		return FALSE;

	if(pcsCharacter->m_csTitle->IsHaveTitle(pstTitle->lTitleTid))
		return FALSE;

	if(!pcsCharacter->m_csTitleQuest->IsHaveTitleQuest(pstTitle->lTitleTid))
		return FALSE;

	if(pcsCharacter->m_csTitle->IsFullTitle())
		return FALSE;

	m_pagsmTitle->WriteLogTitle(AGSMTITLE_LOG_TITLEQUEST_COMPLETE_BY_GM, pcsCharacter, pstTitle->lTitleTid);

	return m_pagsmTitle->MakeAndSendTitleQuestCompleteRelayPacket(pcsCharacter, pstTitle->lTitleTid);
}
