#ifndef	__AGPDITEMCONVERT_H__
#define	__AGPDITEMCONVERT_H__

#include "ApBase.h"
#include "AgpmSkill.h"
#include "AgpdItemTemplate.h"

//#define	AGPDITEMCONVERT_MAX_PHYSICAL_CONVERT		8
#define	AGPDITEMCONVERT_MAX_WEAPON_SOCKET			8
#define	AGPDITEMCONVERT_MAX_ARMOUR_SOCKET			6
#define	AGPDITEMCONVERT_MAX_ETC_SOCKET				5
#define	AGPDITEMCONVERT_MAX_SPIRIT_STONE			5

#define	AGPDITEMCONVERT_MAX_DESCRIPTION				128

#define	AGPDITEMCONVERT_MAX_ITEM_RANK				16
#define	AGPDITEMCONVERT_MAX_SPIRITSTONE_RANK		5


typedef enum	_AgpdItemConvertResult {
	AGPDITEMCONVERT_RESULT_NONE					= 0,
	AGPDITEMCONVERT_RESULT_SUCCESS,
	AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL,
	AGPDITEMCONVERT_RESULT_IS_EGO_ITEM,
	AGPDITEMCONVERT_RESULT_INVALID_CATALYST,
	AGPDITEMCONVERT_RESULT_FAILED,
	AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME,
	AGPDITEMCONVERT_RESULT_FAILED_AND_INIT,
	AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY,
	AGPDITEMCONVERT_RESULT_INVALID_ITEM,
	AGPDITEMCONVERT_RESULT_MAX
} AgpdItemConvertResult;

typedef enum	_AgpdItemConvertSocketResult {
	AGPDITEMCONVERT_SOCKET_RESULT_NONE				= 0,
	AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS,
	AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL,
	AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM,
	AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY,
	AGPDITEMCONVERT_SOCKET_RESULT_FAILED,
	AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME,
	AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT,
	AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY,
	AGPDITEMCONVERT_SOCKET_RESULT_INVALID_ITEM,				// Socket을 뚫을 수 없는 넘이다.
	AGPDITEMCONVERT_SOCKET_RESULT_MAX
} AgpdItemConvertSocketResult;

typedef enum	_AgpdItemConvertSpiritStoneResult {
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS		= 0,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_ALREADY_FULL,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_EGO_ITEM,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_IMPROPER_ITEM,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALID_RANK,
	AGPDITEMCONVERT_SPIRITSTONE_RESULT_MAX
} AgpdItemConvertSpiritStoneResult;

typedef enum	_AgpdItemConvertRuneResult {
	AGPDITEMCONVERT_RUNE_RESULT_NONE				= 0,
	AGPDITEMCONVERT_RUNE_RESULT_SUCCESS,
	AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM,
	AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL,
	AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM,
	AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL,
	AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL,
	AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART,
	AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT,
	AGPDITEMCONVERT_RUNE_RESULT_FAILED,
	AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME,
	AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT,
	AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY,
	AGPDITEMCONVERT_RUNE_RESULT_INVALID_ITEM,
	AGPDITEMCONVERT_RUNE_RESULT_MAX
} AgpdItemConvertRuneResult;


typedef struct	_AgpdItemConvertPhysical {
	INT32		lWeaponAddValue;
	//INT32		lWeaponProbability;
	INT32		lArmourAddValue;
	//INT32		lArmourProbability;
	CHAR		szRank[16];
	BOOL		bIsConvertableSpirit[AGPDITEMCONVERT_MAX_SPIRITSTONE_RANK + 1];

	INT32		lSuccessProb[AGPDITEMCONVERT_MAX_ITEM_RANK + 1];
	INT32		lFailProb[AGPDITEMCONVERT_MAX_ITEM_RANK + 1];
	INT32		lInitProb[AGPDITEMCONVERT_MAX_ITEM_RANK + 1];
	INT32		lDestroyProb[AGPDITEMCONVERT_MAX_ITEM_RANK + 1];
} AgpdItemConvertPhysical;

/*
typedef struct	_AgpdItemConvertPhysicalFail {
	INT32		lKeepCurrent;
	INT32		lInitializeSame;
	INT32		lInitialize;
	INT32		lDestroy;
} AgpdItemConvertPhysicalFail;
*/

typedef struct	_AgpdItemConvertAddSocket {
	INT32		lWeaponProb;
	INT32		lWeaponCost;
	INT32		lArmourProb;
	INT32		lArmourCost;
	INT32		lEtcProb;
	INT32		lEtcCost;
} AgpdItemConvertAddSocket;

typedef struct	_AgpdItemConvertAddSocketFail {
	INT32		lWeaponKeepCurrent;
	INT32		lWeaponInitializeSame;
	INT32		lWeaponInitialize;
	INT32		lWeaponDestroy;
	INT32		lArmourKeepCurrent;
	INT32		lArmourInitializeSame;
	INT32		lArmourInitialize;
	INT32		lArmourDestroy;
	INT32		lEtcKeepCurrent;
	INT32		lEtcInitializeSame;
	INT32		lEtcInitialize;
	INT32		lEtcDestroy;
} AgpdItemConvertAddSocketFail;

typedef struct	_AgpdItemConvertSpiritStone {
	INT32		lWeaponAddValue;
	INT32		lWeaponProbability;
	INT32		lArmourAddValue;
	INT32		lArmourProbability;
} AgpdItemConvertSpiritStone;

typedef struct	_AgpdItemConvertSameAttrBonus {
	INT32		lWeaponBonus;
	INT32		lArmourBonus;
} AgpdItemConvertSameAttrBonus;

typedef struct	_AgpdItemConvertRune {
	INT32		lWeaponProbability;
	INT32		lArmourProbability;
	INT32		lEtcProbability;
} AgpdItemConvertRune;

typedef struct	_AgpdItemConvertRuneFail {
	INT32		lWeaponKeepCurrent;
	INT32		lWeaponInitializeSame;
	INT32		lWeaponInitialize;
	INT32		lWeaponDestroy;
	INT32		lArmourKeepCurrent;
	INT32		lArmourInitializeSame;
	INT32		lArmourInitialize;
	INT32		lArmourDestroy;
	INT32		lEtcKeepCurrent;
	INT32		lEtcInitializeSame;
	INT32		lEtcInitialize;
	INT32		lEtcDestroy;
} AgpdItemConvertRuneFail;



typedef struct _AgpdItemConvertPoint {
	INT32		lPhysicalPoint[AGPDITEMCONVERT_MAX_ITEM_RANK + 1];
	INT32		lSpiritStonePoint[AGPDITEMCONVERT_MAX_SPIRIT_STONE + 1];
	INT32		lRunePoint[AGPDITEMCONVERT_MAX_SPIRIT_STONE + 1];
	INT32		lSocketPoint[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
} AgpdItemConvertPoint;


typedef struct	_AgpdItemConvertSocketAttr {
	BOOL		bIsSpiritStone;
	INT32		lTID;
	AgpdItemTemplate	*pcsItemTemplate;
} AgpdItemConvertSocketAttr;

class AgpdItemConvertADChar 
{
public:
	AgpdFactor					m_csRuneAttrFactor;
	/*stOptionSkillData			m_stOptionSkillData;*/
};

class AgpdItemConvertADItem 
{
public:
	INT32						m_lPhysicalConvertLevel;

	INT32						m_lNumSocket;

	INT32						m_lNumConvert;
	ApSafeArray<AgpdItemConvertSocketAttr, AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1>	m_stSocketAttr;

	AgpdFactor					m_csRuneAttrFactor;
	stOptionSkillData			m_stOptionSkillData;
};

class AgpdItemConvertADItemTemplate {
public:
	ApSafeArray<BOOL, AGPMITEM_EQUIP_KIND_NUM>	m_bRuneConvertableEquipKind;
	ApSafeArray<BOOL, AGPMITEM_PART_NUM>		m_bRuneConvertableEquipPart;

	INT32						m_lRuneSuccessProbability;
	INT32						m_lRuneRestrictLevel;

	INT32						m_lAntiTypeNumber;

	AgpdSkillTemplate			*m_pcsSkillTemplate;
	INT32						m_lSkillLevel;
	INT32						m_lSkillProbability;

	CHAR						m_szDescription[AGPDITEMCONVERT_MAX_DESCRIPTION + 1];
};

#endif	//__AGPDITEMCONVERT_H__