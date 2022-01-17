#if !defined(__AGPDDROPITEM2_H__)
#define __AGPDDROPITEM2_H__

#include <stdlib.h>
#include <memory.h>
#include "ApModule.h"
#include "AgpmItem.h"

#define AGPMDROP_MAX_RANK		20
#define AGPMDROP_MAX_ITEMRANK	10

class AgpdDropItemEntry
{
public:
	AgpdItemTemplate *	m_pcsItemTemplate;
	INT32				m_lDropRate;

	AgpdDropItemEntry()
	{
		m_pcsItemTemplate	= NULL;
		m_lDropRate			= 0;
	}
};

typedef vector<AgpdDropItemEntry>	AgpdDropItems;
typedef AgpdDropItems::iterator		AgpdDropItemsIter;

typedef vector<AgpdItemOptionTemplate *>	AgpdDropItemOptions;
typedef AgpdDropItemOptions::iterator		AgpdDropItemOptionsIter;

class AgpdDropItemGroup : public ApBase
{
public:
	ApCriticalSection	m_csCSection;
	INT32			m_lDropRate;	// ±×·ìº° Drop Rate (1/100000)
	INT32			m_alLevelBonus[(AGPMCHAR_MAX_LEVEL - 1) / 10 + 1];		// Level¿¡ µû¸¥ Drop·ü Á¶Á¤
	INT32			m_lSuitableLevel;
	AgpdDropItems	m_vtTemplates;
	INT32			m_alRankItemCount[AGPMDROP_MAX_RANK];

	AgpdDropItemGroup()
	{
		Init();
	}

	VOID Init()
	{
		m_lDropRate	= 0;
		m_lSuitableLevel = 0;
		m_vtTemplates.clear();
		memset(m_alLevelBonus, 0, sizeof(INT32) * ((AGPMCHAR_MAX_LEVEL - 1) / 10 + 1));
		memset(m_alRankItemCount, 0, sizeof(INT32) * AGPMDROP_MAX_RANK);
	}
};

class AgpdDropItemEntry2
{
public :
	INT32			m_lItemTID;			// item template id
	INT32			m_lMinCount;		// minimum count
	INT32			m_lMaxCount;		// maximum count
	INT32			m_lProb;			// probability (1/100000)

public :
	AgpdDropItemEntry2()
	{
		m_lItemTID = 0;
		m_lMinCount = 0;
		m_lMaxCount = 0;
		m_lProb = 0;
	}
};

typedef vector<AgpdDropItemEntry2>	AgpdDropItems2;
typedef AgpdDropItems2::iterator	AgpdDropItemsIter2;

// Depth °¡ ÇÏ³ª ´Ã¾î³µ´Ù. 2008.05.16. steeple
struct AgpdDropItemGroupEntry2
{
	INT32 m_lCharacterTID;
	INT32 m_lDropCount;

	AgpdDropItems2 m_vtDropItems;

	AgpdDropItemGroupEntry2() : m_lCharacterTID(0), m_lDropCount(0) {}
};

typedef vector<AgpdDropItemGroupEntry2>		AgpdDropItemGroupEntries;
typedef AgpdDropItemGroupEntries::iterator	AgpdDropItemGroupEntryIter;

class AgpdDropItemGroup2
{
public :
	INT32						m_lCharacterTID;
	AgpdDropItemGroupEntries	m_vtGroupEntries;
	
public :
	AgpdDropItemGroup2() : m_lCharacterTID(0) {}
};

typedef map<INT32, AgpdDropItemGroup2 *>	AgpdDropItem2Map;
typedef AgpdDropItem2Map::iterator			AgpdDropItem2MapIter;
typedef pair<INT32, AgpdDropItemGroup2 *>	AgpdDropItem2MapPair;

class AgpdDropItemSocket
{
public:
	INT32 *	m_plSocketRate;

	AgpdDropItemSocket()
	{
		m_plSocketRate	= NULL;
	}

	~AgpdDropItemSocket()
	{
		if (m_plSocketRate)
		{
			delete [] m_plSocketRate;
			m_plSocketRate	= NULL;
		}
	}
};

class AgpdDropItemOptionNum
{
public:
	INT32 *	m_plOptionRate;

	AgpdDropItemOptionNum()
	{
		m_plOptionRate	= NULL;
	}

	~AgpdDropItemOptionNum()
	{
		if (m_plOptionRate)
		{
			delete [] m_plOptionRate;
			m_plOptionRate	= NULL;
		}
	}
};

class AgpdDropItemOptionTable
{
public:
	INT32	Rate;
	AgpdItemOptionTemplate* pcsAgpdOptionTemplate;

	AgpdDropItemOptionTable()
	{
		Rate = 0;
		pcsAgpdOptionTemplate = NULL;
	}
};

class AgpdDropItemOption
{
public:
	INT32						m_lTotalRate;
	AgpdDropItemOptions			m_vtOptions;
	INT32						m_lRateTableCount;
	AgpdDropItemOptionTable*	m_ppRateTable;

	AgpdDropItemOption()
	{
		m_lTotalRate	= 0;
		m_ppRateTable	= NULL;
		m_lRateTableCount	= 0;
	}
};

class AgpdDropItemADCharTemplate
{
public:
	INT32	m_lRank;
	FLOAT	m_fRateScale;
	AgpmItemUsableSpiritStoneType	m_eSpiritStoneType;
};

class AgpdDropItemADItemTemplate
{
public:
	INT32	m_lDropRank;
	INT32	m_lDropRate;
	INT32	m_lGroupID;
	INT32	m_lSuitableLevelMin;
	INT32	m_lSuitableLevelMax;

	INT32	m_lGachaLevelMin;
	INT32	m_lGachaLevelMax;
};

class AgpdDropItemInfo2
{
public:
	AgpdCharacter *	m_pcsDropCharacter;
	INT32			m_lAttackerLevel;
	ApBase *		m_pcsFirstLooter;
	INT32			m_lItemStackCount;

	AgpdDropItemInfo2()
	{
		m_pcsDropCharacter	= NULL;
		m_lAttackerLevel	= 0;
		m_pcsFirstLooter	= NULL;
		m_lItemStackCount	= 1;
	}
};

#endif