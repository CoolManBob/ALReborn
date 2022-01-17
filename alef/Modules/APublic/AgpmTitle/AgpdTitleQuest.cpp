#include "AgpdTitleQuest.h"

AgpdTitleQuest::AgpdTitleQuest()
{
	m_bLoadedTitleQuest = FALSE;
}

vector<AgpdCurrentTitleQuest>::iterator AgpdTitleQuest::FindTitleQuestByTitleID(INT32 lTitleID)
{
	IterCurrentTitleQuest iter;

	for(iter = m_vtCurrentTitleQuest.begin(); iter != m_vtCurrentTitleQuest.end(); ++iter)
	{
		if ((*iter).m_nTitleTid == lTitleID) return iter;
	}

	return iter;
}

bool AgpdTitleQuest::CheckForQuestByTitleID(INT32 lTitleID)
{
	IterCurrentTitleQuest iter;

	for (iter = m_vtCurrentTitleQuest.begin(); iter != m_vtCurrentTitleQuest.end(); ++iter)
	{
		if ((*iter).m_nTitleTid == lTitleID) return true;
	}

	return false;
}

BOOL AgpdTitleQuest::IsFullTitleQuest()
{
	if(m_vtCurrentTitleQuest.size() >= AGPDTITLE_MAX_TITLE_QUEST_NUMBER)
		return TRUE;

	return FALSE;
}

BOOL AgpdTitleQuest::AddTitleQuest(AgpdCurrentTitleQuest CurrentTitleQuest)
{
	if(IsFullTitleQuest())
		return FALSE;

	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(CurrentTitleQuest.m_nTitleTid);

	if(iter != m_vtCurrentTitleQuest.end())
		return FALSE;

	m_vtCurrentTitleQuest.push_back(CurrentTitleQuest);
	return TRUE;
}

BOOL AgpdTitleQuest::RemoveTitleQuest(INT32 lTitleID)
{
	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(lTitleID);

	if(iter == m_vtCurrentTitleQuest.end())
		return FALSE;

	m_vtCurrentTitleQuest.erase(iter);

	return TRUE;
}

BOOL AgpdTitleQuest::IsHaveTitleQuest(INT32 lTitleID)
{
	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(lTitleID);

	if(iter == m_vtCurrentTitleQuest.end())
		return FALSE;

	return TRUE;
}

BOOL AgpdTitleQuest::CorrectCheckStyle(INT32 lTitleID, INT32 TitleCheckType, INT32 TitleCheckSet)
{
	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(lTitleID);

	if(iter == m_vtCurrentTitleQuest.end())
		return FALSE;

	IterTitleCurrentCheck iter2;

	for(iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		if((*iter2).m_nTitleCheckType == TitleCheckType && (*iter2).m_nTitleCheckSet == TitleCheckSet)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpdTitleQuest::UpdateQuestCheckValue(INT32 TitleID, INT32 TitleCheckType, INT32 TitleCheckSet, INT32 TitleCheckNewValue)
{
	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(TitleID);

	if(iter == m_vtCurrentTitleQuest.end())
		return FALSE;

	IterTitleCurrentCheck iter2;

	for(iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		if((*iter2).m_nTitleCheckType == TitleCheckType && (*iter2).m_nTitleCheckSet == TitleCheckSet)
		{
			(*iter2).m_nTitleCurrentValue = TitleCheckNewValue;
			return TRUE;
		}
	}

	return FALSE;
}

INT32 AgpdTitleQuest::GetQuestCheckValue(INT32 TitleID, INT32 TitleCheckType)
{
	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(TitleID);

	if(iter == m_vtCurrentTitleQuest.end())
		return NULL;

	IterTitleCurrentCheck iter2;

	for(iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		if((*iter2).m_nTitleCheckType == TitleCheckType)
			return (*iter2).m_nTitleCurrentValue;
	}

	return NULL;
}

BOOL AgpdTitleQuest::CheckQuestCompletionCondition(INT32 lTitleID)
{
	IterCurrentTitleQuest iter = FindTitleQuestByTitleID(lTitleID);

	if(iter == m_vtCurrentTitleQuest.end())
		return FALSE;

	IterTitleCurrentCheck iter2;

	for(iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
	{
		if((*iter2).m_nTitleCheckValue > (*iter2).m_nTitleCurrentValue)
			return FALSE;
	}

	return TRUE;
}