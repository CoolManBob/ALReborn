/*=====================================================================

	AgpdProduct.h

=====================================================================*/

#ifndef _AGPD_PRODUCT_H_
	#define _AGPD_PRODUCT_H_

#include "ApBase.h"
#include "AgpdItemTemplate.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGPMPRODUCT_CATEGORY
	{
	AGPMPRODUCT_CATEGORY_NONE	= -1,
	AGPMPRODUCT_CATEGORY_HUNT	= 0,
	AGPMPRODUCT_CATEGORY_COOK,
	AGPMPRODUCT_CATEGORY_ALCHEMY,
	AGPMPRODUCT_CATEGORY_MAX,
	};

const enum eAGPMPRODUCT_CATEGORY_EXCEL_COLUMN
	{
	AGPMPRODUCT_CATEGORY_EXCEL_COLUMN_CATEGORY = 0,
	AGPMPRODUCT_CATEGORY_EXCEL_COLUMN_SKILLNAME,
	AGPMPRODUCT_CATEGORY_EXCEL_COLUMN_COMPOSES,
	};

#define AGPMPRODUCT_MAX_LEVEL				15

const enum eAGPMPRODUCT_SKILL_FACTOR
	{
	AGPMPRODUCT_SKILL_FACTOR_MIN_EXP = 0,
	AGPMPRODUCT_SKILL_FACTOR_MAX_EXP,
	AGPMPRODUCT_SKILL_FACTOR_LEVELUP_EXP,
	AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP,
	AGPMPRODUCT_SKILL_FACTOR_ADD_PROB,
	AGPMPRODUCT_SKILL_FACTOR_NUM,
	};

const enum eAGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN
	{
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_SKILL_NAME		= 0,
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_LEVEL,
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_MIN_EXP,
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_MAX_EXP,
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_ADD_PROB,
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_LEVELUP_EXP,
	AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_ACC_LEVELUP_EXP,
	};

#define AGPMPRODUCT_MAX_COMPOSE_SOURCE			4

#define AGPMPRODUCT_MAX_COMPOSE_OWN				100

#define AGPMPRODUCT_MAX_COMPOSE_NAME_LENGTH		32

#define	AGPMPRODUCT_MAX_RANGE					250			// 250 cm

const enum eAGPMPRODUCT_COMPOSE_EXCEL_COLUMN
	{
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_MAKEID			= 0,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_CATEGORY,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RESULT_TID,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RESULT_NAME,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_USING_MONSTER,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_LEVEL,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_PRICE,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_SUCCESS_PROB,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RECEIPE_TID,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RECEIPE_NAME,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RECEIPE_ADD_PROB,

	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_A_TID,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_A_NAME,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_A_COUNT,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_B_TID,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_B_NAME,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_B_COUNT,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_C_TID,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_C_NAME,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_C_COUNT,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_D_TID,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_D_NAME,
	AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_D_COUNT,	
	};

#define AGPMPRODUCT_MAX_GATHER_SKILL	3

const enum eAGPMPRODUCT_GATHER_EXCEL_COLUMN
	{
	AGPMPRODUCT_GATHER_EXCEL_COLUMN_TARGET_TYPE	= 0,
	AGPMPRODUCT_GATHER_EXCEL_COLUMN_TARGET_TID	= 1,
	AGPMPRODUCT_GATHER_EXCEL_COLUMN_CLASS		= 2,
	AGPMPRODUCT_GATHER_EXCEL_COLUMN_SKILL_TID_A	= 3,
	AGPMPRODUCT_GATHER_EXCEL_COLUMN_PROB_A		= 4,
	AGPMPRODUCT_GATHER_EXCEL_COLUMN_ITEM_SET_A	= 5,
	};

/****************************************/
/*		The Definition of Product 		*/
/****************************************/
//
//	==========	Product Category==========
//
struct AgpdProductCategory
	{
	INT32	m_eCategory;
	INT32	m_lSkillTID;
	};

//
//	==========	Product Factor	==========
//
//	!!! Warning : m_lFactors uses 1-based index(level). all value of index 0 is 0
//
class AgpdProductSkillFactor
	{
	public:
		CHAR		m_szName[AGPMSKILL_MAX_SKILL_NAME + 1];
		INT32		m_lFactors[AGPMPRODUCT_MAX_LEVEL+1][AGPMPRODUCT_SKILL_FACTOR_NUM];
		INT32		m_lMaxLevel;
	};


/****************************************/
/*		The Definition of Compose		*/
/****************************************/
//
//	==========	Compose Element	==========
//
struct AgpdComposeElement
	{
	AgpdItemTemplate*	m_pItemTemplate;
	INT32				m_lCount;
	};

//
//	==========	Compose Template	==========
//
class AgpdComposeTemplate
	{
	public:
		INT32					m_eCategory;			// eAGPMPRODUCT_CATEGORY
		INT32					m_lComposeID;			// Compose ID. (= Result TID)
		CHAR					m_szCompose[AGPMPRODUCT_MAX_COMPOSE_NAME_LENGTH];		// Compose Name. (= Result Name)
		INT32					m_lPrice;				// Price.
		INT32					m_lSuccessProb;			// Success probability.
		INT32					m_lAdditionalProb;		// Additional success probability.
		INT32					m_lLevel;				// Level needed
		INT32					m_lNeedTitleTid;		// Title needed
		AgpdComposeElement		m_Receipe;				// Receipe item.

		INT32					m_lRequiredItems;		// No. of required items.
		ApSafeArray<AgpdComposeElement, AGPMPRODUCT_MAX_COMPOSE_SOURCE>	m_RequiredItems;	// Required items.
		AgpdComposeElement		m_ResultItem;											// Result item.
	};


//
//	========== Compose CAD	==========
//
class AgpdComposeCharacterAD : public ApBase
	{
	public :
		INT32	m_lCount;
		ApSafeArray<INT32, AGPMPRODUCT_MAX_COMPOSE_OWN>		m_OwnComposes;
	};

/****************************************/
/*		The Definition of Gather		*/
/****************************************/
//
//	==========	Character Template Attached Data	==========
//
//	is attached to character template(AgpdCharacterTemplate)
//	and determine player character can cast product skill to this character(monster) or not.
//
struct AgpdGatherElement
	{
	INT32		m_lSkillTID;			// 생산스킬 TID
	INT32		m_lProbability;			// 성공확률
	INT32		m_lItemSetID;			// 지급 아이템셋 ID
	};


//struct AgpdGatherClassElement
//	{
//	INT32		m_lCount;
//	ApSafeArray<AgpdGatherElement,AGPMPRODUCT_MAX_GATHER_SKILL>	m_Elements;
//	};


class AgpdGatherCharacterTAD
	{
	public:
		ApSafeArray<AgpdGatherElement, AUCHARCLASS_TYPE_MAX>	m_ClassElement;
	};


//
//	==========	Character Attached Data	==========
//
class AgpdGatherCharacterAD : public ApBase
	{
	public:
		BOOL	m_bIsGathering;		// is gathering status?
		INT32	m_lCharacterID;		// reserved character id
		BOOL	m_nRemained;		// remained item count

		INT32	m_lGatherTargetCID;
		UINT32	m_ulStartGatherTimeMSec;
	};


#endif