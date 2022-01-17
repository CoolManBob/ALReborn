#if !defined(__AGPDQUESTTEMPLATE_H__)
#define __AGPDQUESTTEMPLATE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "AgpdItemTemplate.h"
#include "AgpdItem.h"
#include "AgpdSkill.h"
#include "AgpdCharacter.h"
#include <list>

using namespace std;

const int AGPDQUEST_MAX_NAME			= 60;	// 영문위해 확장
const int AGPDQUEST_MAX_SCENARIO		= 60;	// 영문위해 확장
const int AGPDQUEST_MAX_AREA			= 60;	// 영문위해 확장
const int AGPDQUEST_MOTIVE_MAX_COUNT	= 5;
const int AGPDQUEST_MAX_DIALOG_TEXT		= 2000;	//문엘프퀘스트 관련 2000 확장	AGSDQUEST_EXPAND_BLOCK
const int AGPDQUEST_MAX_SUMMARY_TEXT	= 120;
//const int AGPDQUEST_MAX_GROUP_NAME		= 30;
const int AGPDQUEST_MAX_SUMMARY_TEXT_COUNT	= 5;
//const int AGPDQUEST_MAX_QUEST	= 100;
const int AGPDQUEST_MAX_CHECKPOINT		= 5;

enum Enum_AGPMQUEST_DATA_TYPE
{
	AGPDQUEST_DATA_TYPE_TEMPLATE			= 0,
	AGPDQUEST_DATA_TYPE_QUEST_GROUP,	
};

enum Enum_AGPMQUEST_CATEGORY
{
	AGPDQUEST_SYSTEM_TYPE_NONE			= 0,
	AGPDQUEST_SYSTEM_TYPE_SCENARIO,
	AGPDQUEST_SYSTEM_TYPE_NORMAL,
	AGPDQUEST_SYSTEM_TYPE_EPIC
};

enum Enum_AGPDQUEST_SUMMARY_FACTOR
{
	AGPDQUEST_SUMMARY_FACTOR_NONE		= 0,
	AGPDQUEST_SUMMARY_FACTOR_LEVEL,
	AGPDQUEST_SUMMARY_FACTOR_MONEY,
	AGPDQUEST_SUMMARY_FACTOR_ITEM_COUNT,
	AGPDQUEST_SUMMARY_FACTOR_MONSTER1_COUNT,
	AGPDQUEST_SUMMARY_FACTOR_MONSTER2_COUNT,
	AGPDQUEST_SUMMARY_FACTOR_CHECKPOINT_1,
	AGPDQUEST_SUMMARY_FACTOR_CHECKPOINT_2,
	AGPDQUEST_SUMMARY_FACTOR_CHECKPOINT_3,
	AGPDQUEST_SUMMARY_FACTOR_CHECKPOINT_4,
	AGPDQUEST_SUMMARY_FACTOR_CHECKPOINT_5,
	AGPDQUEST_SUMMARY_FACTOR_NORMAL_ITEM,
};

enum Enum_AGPDQUEST_TYPE
{
	AGPDQUEST_TYPE_NORMAL			= 0,
	AGPDQUEST_TYPE_AUTOSTART		= 1,			// 자동 부여 Quest
	AGPDQUEST_TYPE_REPEAT			= 2,			// 반복해서 수령이 가능한 퀘스트의 경우..
};

// 아이템 평가
struct AgpdElementItem
{
	AgpdItemTemplate*	pItemTemplate;
	INT32 lCount;		// 아이템 수량
	INT32 lUpgrade;		// 아이템 업그레이드 1 ~ 8
	INT32 lSocket;		// 아이템 소켓 개조 1 ~ 8
	AgpmItemBoundTypes	m_eBoundType;
};

// 스킬 평가
struct AgpdElementSkill
{
	AgpdSkillTemplate* pSkillTemplate;
	INT32 lExp;
};

// 몬스터
struct AgpdElementMonster
{
	AgpdCharacterTemplate* pMonsterTemplate;	// 몬스터 정보
	INT32 lCount;								// 몬스터 수량

	AgpdElementItem Item;						// Item 정보
	INT32 lDropRate;							// Item Drop 확률
};

struct AgpdElementSummaryText
{
	CHAR szSummaryText[AGPDQUEST_MAX_SUMMARY_TEXT + 1];
	Enum_AGPDQUEST_SUMMARY_FACTOR eFactor;
};

// 퀘스트 -> 보상으로 새로운 퀘스트를 줄수있다.
class AgpdQuestTemplate;
struct AgpdElementQuest
{
	AgpdQuestTemplate* pQuestTemplate;
	INT32 lBackupTID;
};

// 퀘스트 정보
struct AgpdQuestInfo
{
	CHAR szName[AGPDQUEST_MAX_NAME + 1];
	CHAR szScenarioName[AGPDQUEST_MAX_SCENARIO + 1];
	CHAR szAreaInfo[AGPDQUEST_MAX_AREA + 1];
	Enum_AGPMQUEST_CATEGORY eQuestCategory;
	INT32 lLocation;
};

// 퀘스트 시작 조건
struct AgpdStartCondition
{
	INT32	lLevel;
	INT32	lMaxLevel;	//문엘프퀘스트 관련 수정추가 2005.6.2	AGSDQUEST_EXPAND_BLOCK
	INT32	lRace;
	INT32	lClass;
	INT32	lGender;
	AgpdElementQuest	Quest;
};

// 퀘스트 줄때
struct AgpdQuestPreStart
{
	AgpdElementItem		Item;
};

// 퀘스트 대화
struct AgpdQuestDialogText
{
	CHAR szAcceptText[AGPDQUEST_MAX_DIALOG_TEXT + 1];
	CHAR szIncompleteText[AGPDQUEST_MAX_DIALOG_TEXT + 1];
	CHAR szCompleteText[AGPDQUEST_MAX_DIALOG_TEXT + 1];
};

struct AgpdQuestSummaryText
{
	AgpdElementSummaryText SummaryText[AGPDQUEST_MAX_SUMMARY_TEXT_COUNT];
};

struct AgpdCheckPointElement
{
	INT32 lObjectID;
	AgpdItemTemplate* pItemTemplate;
	INT32 lItemCount;
};

struct AgpdCheckPoint
{
	AgpdCheckPointElement CheckPointItem[AGPDQUEST_MAX_CHECKPOINT];
};

// 퀘스트 완료 조건
struct AgpdCompleteCondition
{
	INT32				lLevel;
	INT32				lMoney;
	AgpdElementItem		Item;
	AgpdElementMonster	Monster1;
	AgpdElementMonster	Monster2;
};

// 퀘스트 완료시 보상
struct AgpdResult
{
	INT32				lMoney;
	INT32				lExp;
	AgpdElementQuest	Quest;
	AgpdElementSkill	ProductSkill;	// 생산 스킬만 사용한다.
	AgpdElementItem		Item1;
	AgpdElementItem		Item2;
	AgpdElementItem		Item3;
	AgpdElementItem		Item4;
};

// 하나의 완전한 퀘스트 
class AgpdQuestTemplate : public ApBase
{
public:
	Enum_AGPDQUEST_TYPE		m_eQuestType;
	AgpdQuestInfo			m_QuestInfo;
	AgpdStartCondition		m_StartCondition;
	AgpdQuestPreStart		m_PreStart;
	AgpdQuestDialogText		m_DialogText;
	AgpdCheckPoint			m_CheckPoint;
	AgpdQuestSummaryText	m_SummaryText;
	AgpdCompleteCondition	m_CompleteCondition;
	AgpdResult				m_Result;

	AgpdQuestTemplate()
	{
		m_eQuestType	=	AGPDQUEST_TYPE_NORMAL;
		::ZeroMemory(&m_QuestInfo, sizeof(m_QuestInfo));
		::ZeroMemory(&m_StartCondition, sizeof(m_StartCondition));
		::ZeroMemory(&m_PreStart, sizeof(m_PreStart));
		::ZeroMemory(&m_DialogText, sizeof(m_DialogText));
		::ZeroMemory(&m_CheckPoint, sizeof(m_CheckPoint));
		::ZeroMemory(&m_SummaryText, sizeof(m_SummaryText));
		::ZeroMemory(&m_CompleteCondition, sizeof(m_CompleteCondition));
		::ZeroMemory(&m_Result, sizeof(m_Result));
	}
};

// Quest Group Data
struct AgpdQuestCheckPoint
{
	INT32 lQuestTID;
	INT32 lIndex;
};

class AgpdQuestGroup : public ApBase
{
public:
	CHAR m_szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	list< INT32 > m_GrantQuest;
	list< INT32 > m_EstimateQuest;
	list< AgpdQuestCheckPoint > m_CheckPoint;
};

#endif