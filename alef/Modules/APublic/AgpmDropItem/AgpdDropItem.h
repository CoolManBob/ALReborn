#if !defined(__AGPDDROPITEM_H__)
#define __AGPDDROPITEM_H__

#include <stdlib.h>
#include <memory.h>
#include "ApModule.h"

const INT32		AGPADROPITEMTEMPLATE = 999;
const INT32		AGPDDROPITEM_ITEM_COUNT = 50;
const INT32		AGPDDROPITEM_COUNT = 50;
const INT32		AGPDDROP_EQUIPITEM_COUNT = 99;
const INT32		AGPDDROPITEM_RUNECOUNT = 10;
const INT32		AGPDDROPITEM_PROBABLE = 100000;

//------------------------------------------------------------------------------------------------------------------
//여기부터 새로운 드랍
enum eAgpmDropEquipCategory
{
	AGPMDROPITEM_EQUIP_HUMAN_FIGHTER=0,
	AGPMDROPITEM_EQUIP_HUMAN_ARCHER,
	AGPMDROPITEM_EQUIP_HUMAN_MAGE,
	AGPMDROPITEM_EQUIP_HUMAN_MONK,
	AGPMDROPITEM_EQUIP_ORC_FIGHTER,
	AGPMDROPITEM_EQUIP_ORC_ARCHER,
	AGPMDROPITEM_EQUIP_ORC_MAGE,
	AGPMDROPITEM_EQUIP_ORC_MONK,
	AGPMDROPITEM_EQUIP_DRG_FIGHTER,
	AGPMDROPITEM_EQUIP_DRG_ARCHER,
	AGPMDROPITEM_EQUIP_DRG_MAGE,
	AGPMDROPITEM_EQUIP_DRG_MONK,
	AGPMDROPITEM_EQUIP_ELF_FIGHTER,
	AGPMDROPITEM_EQUIP_ELF_ARCHER,
	AGPMDROPITEM_EQUIP_ELF_MAGE,
	AGPMDROPITEM_EQUIP_ELF_MONK,
	AGPMDROPITEM_EQUIP_MAX_RACE,
};

enum eAgpmDropItemCategory
{
	AGPMDROPITEM_EQUIP = 0,		//장비
	AGPMDROPITEM_CONSUME,		//소비형
	AGPMDROPITEM_VALUABLE,		//귀중품
	AGPMDROPITEM_OPTIONSTONE,	//기원석. 뭔가 더 글러싸한 이름없나? 쩝.
	AGPMDROPITEM_GHELLD,		//겔드
	AGPMDROPITEM_PRODUCT,		//생산아이템
	AGPMDROPITEM_RARE,			//레어.
	AGPMDROPITEM_EVENT,			//이벤트 아이템.
	AGPMDROPITEM_LOTTERY,		//복권아이템
	AGPMDROPITEM_MAX_CATEGORY
};

class AgpdDropItemData
{
public:
	float			m_fDropProbable;
	INT32			m_lItemCount;	//실제로 몇개를 가지고 있는가?
	ApSafeArray<INT32, AGPDDROPITEM_COUNT>		m_alTID;
	ApSafeArray<INT32, AGPDDROPITEM_COUNT>		m_alScalar;
	char			m_strItemName[AGPDDROPITEM_COUNT][80];

	AgpdDropItemData()
	{
		m_fDropProbable = 0.0f;
		m_lItemCount = 0;

		m_alTID.MemSetAll();
		m_alScalar.MemSetAll();

		memset( m_strItemName, 0, sizeof( m_strItemName ) );
	}
};

class AgpdDropEquipData
{
public:
	INT32			m_lItemCount;	//실제로 몇개를 가지고 있는가?
	INT32			m_lRace;
	INT32			m_lClass;
	ApSafeArray<INT32, AGPDDROP_EQUIPITEM_COUNT>	m_alTID;
	ApSafeArray<INT32, AGPDDROP_EQUIPITEM_COUNT>	m_alScalar;
	char			m_strItemName[AGPDDROP_EQUIPITEM_COUNT][80];

	AgpdDropEquipData()
	{
		m_lItemCount = 0;	//실제로 몇개를 가지고 있는가?
		m_lRace = 0;
		m_lClass = 0;
		m_alTID.MemSetAll();
		m_alScalar.MemSetAll();

		memset( m_strItemName, 0, sizeof( m_strItemName ) );
	}
};

class AgpdDropItemEquipTemplate
{
public:
	ApSafeArray<AgpdDropEquipData, AGPMDROPITEM_EQUIP_MAX_RACE>		m_csAgpdDropEquipData;

	INT32						m_lTID;
};

class AgpdDropItemTemplate
{
public:
	char						m_strTemplateName[80];
	INT32						m_lEquip;
	INT32						m_lTID;
	ApSafeArray<AgpdDropItemData, AGPMDROPITEM_MAX_CATEGORY>	m_cDropCategory;
};
//여기까지 새로운 드랍
//=================================================================================================================

class AgpdDropItemInfo
{
public:
	INT32			m_lItemTID;

	//드랍확률과 갯수, 대박 정보.
	INT32			m_lDropRate;
	INT32			m_lERate;
	INT32			m_lMinQuantity;
	INT32			m_lMaxQuantity;
	INT32			m_lJackpotRate;
	INT32			m_lJackpotPiece;
	INT32			m_lRuneOptDamage;
	INT32			m_lRuneOptMinDamage;
	INT32			m_lRuneOptMaxDamage;
	INT32			m_lRuneOptAttackPoint;
	INT32			m_lRuneOptMinAttackPoint;
	INT32			m_lRuneOptMaxAttackPoint;
	INT32			m_lRuneOptDefenseRate;
	INT32			m_lRuneOptMinDefenseRate;
	INT32			m_lRuneOptMaxDefenseRate;
	INT32			m_lRuneOptDefensePoint;
	INT32			m_lRuneOptMinDefensePoint;
	INT32			m_lRuneOptMaxDefensePoint;
	INT32			m_lRuneOptHP;
	INT32			m_lRuneOptMinHP;
	INT32			m_lRuneOptMaxHP;
	INT32			m_lRuneOptMP;
	INT32			m_lRuneOptMinMP;
	INT32			m_lRuneOptMaxMP;
	INT32			m_lRuneOptSP;
	INT32			m_lRuneOptMinSP;
	INT32			m_lRuneOptMaxSP;
	INT32			m_lRuneOptHPRegen;
	INT32			m_lRuneOptMinHPRegen;
	INT32			m_lRuneOptMaxHPRegen;
	INT32			m_lRuneOptMPRegen;
	INT32			m_lRuneOptMinMPRegen;
	INT32			m_lRuneOptMaxMPRegen;
	INT32			m_lRuneOptSPRegen;
	INT32			m_lRuneOptMinSPRegen;
	INT32			m_lRuneOptMaxSPRegen;
	INT32			m_lRuneOptAttackSpeed;
	INT32			m_lRuneOptMinAttackSpeed;
	INT32			m_lRuneOptMaxAttackSpeed;

	ApSafeArray<INT32, AGPDDROPITEM_COUNT+1>	m_alDropRuneCountRate;

	AgpdDropItemInfo();
	~AgpdDropItemInfo();
	void Reset();
};

class AgpdDropItemBag
{
public:
	ApSafeArray<AgpdDropItemInfo, AGPDDROPITEM_ITEM_COUNT>	m_csAgpdDropItemInfo;

	ApSafeArray<INT32, AGPDDROPITEM_COUNT+1>				m_alDropItemCountRate;
	ApSafeArray<INT32, AGPDDROPITEM_COUNT+1>				m_alDropEquipItemCountRate;

	AgpdDropItemBag()
	{
		m_alDropItemCountRate.MemSetAll();
		m_alDropEquipItemCountRate.MemSetAll();
	}
};

#endif