#include "StdAfx.h"
#include ".\hashmapnpc.h"

CHashMapQuestGroup::CHashMapQuestGroup(void)
{
}

CHashMapQuestGroup::~CHashMapQuestGroup(void)
{
	Destory();
}

void CHashMapQuestGroup::Destory()
{
	IterQuestGroup iter = m_HashMap.begin();

	while (iter != m_HashMap.end())
	{
		delete iter->second;
		++iter;
	}

	m_HashMap.clear();
}

QuestGroupData* CHashMapQuestGroup::AddQuestGroup(CHAR* szName, INT32 lID, EnumQuestGroupType eType)
{
	QuestGroupData* pData = new QuestGroupData;
	pData->lID = lID;
	pData->eType = eType;

	m_HashMap.insert(HashMapQuestGroup::value_type(szName, pData));
	return pData;
}

QuestGroupData* CHashMapQuestGroup::Find(const CHAR* szName)
{
	IterQuestGroup iter = m_HashMap.find(szName);

	if (iter != m_HashMap.end()) 
		return iter->second;

	return NULL;
}

BOOL CHashMapQuestGroup::AppendQuestID(CHAR* szName, INT32 lQuestID, BOOL bGive)
{
	QuestGroupData* pData = Find(szName);
	if (!pData) return FALSE;

	if(bGive)
		pData->listGive.push_back(lQuestID);
	else
		pData->listComfirm.push_back(lQuestID);

	return TRUE;
}