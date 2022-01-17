#pragma once

#include <list>
#include <string>
#include <hash_map>

using namespace std;
using namespace stdext;

const INT32 CHECKPOINT_COUNT = 5;

enum EnumQuestGroupType
{
	QUESTGROUP_TYPE_NONE =	0,
	QUESTGROUP_TYPE_NPC,
	QUESTGROUP_TYPE_OBJECT
};

struct CheckPointElement
{
	INT32 lIndex;
	INT32 lQuestTID;

	CheckPointElement()
	{
		lIndex = 0;
		lQuestTID = 0;
	}
};

struct QuestGroupData
{
private:
	typedef list<INT32> ListINT;
	typedef ListINT::iterator iterINT;
	typedef list<CheckPointElement> ListCheckPoint;
	typedef ListCheckPoint::iterator iterCheckPiont;

public:
	EnumQuestGroupType	eType;				// Type
	INT32				lID;				// Unique ID (QuestGroup ID로 사용된다. NPC일 경우엔 NPC ID가 해당되며
											// object일 경우엔 OID가 해당된다.)
	ListINT				listGive;			// 부여 퀘스트 TID
	ListINT				listComfirm;		// 완료 퀘스트 TID
	ListCheckPoint		listCheckPoint;		// 체크포인트 Object ID

	QuestGroupData()
	{
		eType = QUESTGROUP_TYPE_NONE;
		lID = 0;
		listGive.clear();
		listComfirm.clear();
		listCheckPoint.clear();
	}
};

class CHashMapQuestGroup
{
public:
	typedef hash_map<string, QuestGroupData*> HashMapQuestGroup;
	typedef HashMapQuestGroup::iterator IterQuestGroup;

	HashMapQuestGroup m_HashMap;

public:
	CHashMapQuestGroup(void);
	virtual ~CHashMapQuestGroup(void);

	void Destory();
	
	QuestGroupData* AddQuestGroup(CHAR* szName, INT32 lID, EnumQuestGroupType eType);
	QuestGroupData* Find(const CHAR* szName);

	BOOL AppendQuestID(CHAR* szName, INT32 lQuestID, BOOL bGive);
};
