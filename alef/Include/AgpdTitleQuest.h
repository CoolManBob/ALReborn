#ifndef _AGPD_CURRENT_TITLEQUEST
#define _AGPD_CURRENT_TITLEQUEST

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

#include "AgpdTitleTemplate.h"
#include "AgpdTitle.h"

#include <vector>

const INT32 AGPDTITLE_MAX_TITLE_QUEST_NUMBER	= 10;

class AgpdTitleCurrentCheck
{
public:
	INT32 m_nTitleCheckType;
	INT32 m_nTitleCheckSet;
	INT32 m_nTitleCheckValue;
	INT32 m_nTitleCurrentValue;

	AgpdTitleCurrentCheck() : m_nTitleCheckType(0), m_nTitleCheckSet(0), m_nTitleCheckValue(0), m_nTitleCurrentValue(0)
	{
	}
};

//typedef ApVector<AgpdTitleCurrentCheck, AGPDTITLE_MAX_TITLE_CHECK> VectorTitleCurrentCheck;
typedef vector< AgpdTitleCurrentCheck> VectorTitleCurrentCheck;
typedef VectorTitleCurrentCheck::iterator IterTitleCurrentCheck;

class AgpdCurrentTitleQuest
{
public:
	INT32 m_nTitleTid;
	UINT32 m_nTitleQuestAcceptTime;
	UINT32 m_nTitleQuestCompleteTime;
	VectorTitleCurrentCheck m_vtTitleCurrentCheck;

	AgpdCurrentTitleQuest() : m_nTitleTid(0), m_nTitleQuestAcceptTime(0), m_nTitleQuestCompleteTime(0)
	{
	}

};

//typedef ApVector<AgpdCurrentTitleQuest, AGPDTITLE_MAX_TITLE_QUEST_NUMBER> VectorCurrentTitleQuest;
typedef vector< AgpdCurrentTitleQuest> VectorCurrentTitleQuest;
typedef VectorCurrentTitleQuest::iterator IterCurrentTitleQuest;


class AgpdTitleQuest
{
public:
	BOOL m_bLoadedTitleQuest;
	VectorCurrentTitleQuest m_vtCurrentTitleQuest;

public:
	AgpdTitleQuest();

	BOOL IsFullTitleQuest();

	BOOL AddTitleQuest(AgpdCurrentTitleQuest CurrentTitleQuest);

	BOOL RemoveTitleQuest(INT32 lTitleID);

	BOOL IsHaveTitleQuest(INT32 lTitleID);

	BOOL CorrectCheckStyle(INT32 lTitleID, INT32 TitleCheckType, INT32 TitleCheckSet);

	BOOL UpdateQuestCheckValue(INT32 TitleID, INT32 TitleCheckType, INT32 TitleCheckSet, INT32 TitleCheckNewValue);

	INT32 GetQuestCheckValue(INT32 TitleID, INT32 TitleCheckType);

	BOOL CheckQuestCompletionCondition(INT32 lTitleID);

	vector<AgpdCurrentTitleQuest>::iterator FindTitleQuestByTitleID(INT32 lTitleID);

	bool CheckForQuestByTitleID(INT32 lTitleID);
};

#endif //_AGPD_CURRENT_TITLEQUEST