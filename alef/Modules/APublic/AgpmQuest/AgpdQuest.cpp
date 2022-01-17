#include "AgpdQuest.h"


AgpdQuest::AgpdQuest()
{
	m_vtQuest.reserve(AGPDQUEST_CURRENT_ID_MAX);
	m_vtQuest.clear();
}

vector<AgpdCurrentQuest>::iterator AgpdQuest::FindByQuestID(INT32 lQuestID)
{
	IterQuest iter;
	for (iter = m_vtQuest.begin(); iter != m_vtQuest.end(); ++iter)
	{
		if ((*iter).lQuestID == lQuestID) return iter;
	}

	return m_vtQuest.end();
}

BOOL AgpdQuest::AddNewQuest(INT32 lQuestTID, INT32 lParam1, INT32 lParam2)
{
	// 새로운 퀘스트 과제 추가
	if ( AGPDQUEST_CURRENT_ID_MAX < m_vtQuest.size() ) return FALSE;

	// 같은 퀘스트가 존재하지 않으면 추가
	IterQuest iter = FindByQuestID(lQuestTID);
	if (iter == m_vtQuest.end())
	{
		AgpdCurrentQuest CurrentQuest;
		CurrentQuest.lQuestID	= lQuestTID;
		CurrentQuest.lParam1	= lParam1;
		CurrentQuest.lParam2	= lParam2;
		//strncpy(CurrentQuest.szNPCName, szNPCName, AGPACHARACTER_MAX_ID_STRING);
		m_vtQuest.push_back(CurrentQuest);
		return TRUE;
	}

	return FALSE;
}

BOOL AgpdQuest::UpdateParam(INT32 lQuestTID, INT32 lParam1, INT32 lParam2)
{
	IterQuest iter = FindByQuestID(lQuestTID);

	if (iter != m_vtQuest.end())
	{
		(*iter).lParam1 = lParam1;
		(*iter).lParam2 = lParam2;
		return TRUE;
	}

	return FALSE;
}

BOOL AgpdQuest::CancelQuest(INT32 lQuestTID)
{
	IterQuest iter = FindByQuestID(lQuestTID);

	if (iter != m_vtQuest.end())
	{
		m_vtQuest.erase(iter);
		return TRUE;
	}

	return FALSE;
}

BOOL AgpdQuest::IsHaveQuest(INT32 lQuestTID)
{
	IterQuest iter = FindByQuestID(lQuestTID);

	if (iter != m_vtQuest.end())
		return TRUE;

	return FALSE;
}

INT32 AgpdQuest::GetMonsterCount1(INT32 lQuestTID)
{
	IterQuest iter = FindByQuestID(lQuestTID);
	if (iter != m_vtQuest.end())
	{
		return (*iter).lParam1;
	}

	return -1;
}

INT32 AgpdQuest::GetMonsterCount2(INT32 lQuestTID)
{
	IterQuest iter = FindByQuestID(lQuestTID);
	if (iter != m_vtQuest.end())
	{
		return (*iter).lParam2;
	}

	return -1;
}

INT32 AgpdQuest::IncrementMonsterCount(INT32 lQuestTID)
{
	IterQuest iter = FindByQuestID(lQuestTID);
	if (iter != m_vtQuest.end())
	{
		return ++((*iter).lParam1);
	}

	return -1;
}

INT32 AgpdQuest::SetMonsterCount(INT32 lQuestTID, INT32 lCount)
{
	IterQuest iter = FindByQuestID(lQuestTID);
	if (iter != m_vtQuest.end())
	{
		return (*iter).lParam1 = lCount;
	}

	return -1;
}

INT32 AgpdQuest::GetCount()
{
	return (INT32)m_vtQuest.size();
}