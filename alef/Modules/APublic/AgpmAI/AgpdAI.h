#ifndef	_AGPDAI_H_
#define _AGPDAI_H_

#include "ApBase.h"
#include "AgpmFactors.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpdEventSpawn.h"
#include "AuList.h"

#define AGPDAI_TEMPLATE_NAME_LENGTH		20
#define AGPDAI_MAX_USABLE_ITEM_COUNT	10
#define AGPDAI_MAX_USABLE_SKILL_COUNT	10
#define AGPDAI_MAX_HELP_CHARTYPE_COUNT  10

// Action Type
typedef enum
{
	AGPDAI_ACTION_TYPE_HOLD = 0,		// 제자리에서 대기
	AGPDAI_ACTION_TYPE_MOVE,			// 특정 위치로 이동
	AGPDAI_ACTION_TYPE_FOLLOW,			// Target을 따라감
	AGPDAI_ACTION_TYPE_MOVE_AWAY,		// Target에서 도망감
	AGPDAI_ACTION_TYPE_WANDER,			// 배회
	AGPDAI_ACTION_TYPE_GUARD,			// Target을 지킴
	AGPDAI_ACTION_TYPE_ATTACK,			// Target을 공격
	AGPDAI_ACTION_TYPE_ASSIST,			// Target을 도와줌
	AGPDAI_ACTION_TYPE_YELL,			// Target과 정보교환
	AGPDAI_ACTION_TYPE_PICKUP,			// Target을 줍는다.
	AGPDAI_ACTION_TYPE_USE_ITEM,		// Item을 사용
	AGPDAI_ACTION_TYPE_USE_SKILL,		// Skill을 사용
	AGPDAI_ACTION_TYPE_SCREAM,			// Scream Fear사용
	AGPDAI_ACTION_MAX_TYPE
} AgpdAIActionType;

typedef enum
{
	AGPDAI_TARGET_FLAG_RELATION_AGGRESSIVE	= 0x0001,	// Relation이 -로 안좋은 관계이다.
	AGPDAI_TARGET_FLAG_RELATION_FRIENDLY	= 0x0002,	// Relation이 +로 좋은 관계이다.
	AGPDAI_TARGET_FLAG_AGRO					= 0x0004,	// Agro Point가 있다.
	AGPDAI_TARGET_FLAG_HISTORY_DAMAGE		= 0x0010,	// History에 해를 입힌 것으로 되어있다.
	AGPDAI_TARGET_FLAG_HISTORY_BUFFER		= 0x0020,	// History에 도움을 준 것으로 되어있다.
	AGPDAI_TARGET_FLAG_STRONG				= 0x0040,	// 강한 Target
	AGPDAI_TARGET_FLAG_WEAK					= 0x0080,	// 약한 Target
	AGPDAI_TARGET_FLAG_BOSS					= 0x0100,	// Boss
	AGPDAI_TARGET_FLAG_FOLLOWER				= 0x0200,	// Follower
	AGPDAI_TARGET_FLAG_SELF					= 0x0400,	// 자기 자신
	AGPDAI_TARGET_FLAG_OTHER				= 0x0800,	// 다른 놈
	AGPDAI_TARGET_MAX_FLAG
} AgpdAITargetFlag;

typedef enum
{
	AGPDAI_FACTOR_TYPE_DEFAULT = 0,				// 기본 Factor
	AGPDAI_FACTOR_TYPE_CHAR_FACTOR,				// Character의 Factor
	AGPDAI_FACTOR_TYPE_CHAR_STATUS,				// Character의 Status
	AGPDAI_FACTOR_TYPE_CHAR_ITEM,				// Character가 가진 Item
	AGPDAI_FACTOR_TYPE_ACTION_HISTORY,			// Action Histroy
	AGPDAI_FACTOR_TYPE_ENV_POSITION,			// Target으로부터의 위치
	AGPDAI_FACTOR_TYPE_EMOTION,					// 현재 기분
	AGPDAI_FACTOR_MAX_TYPE
} AgpdAIFactorType;

typedef enum
{
	AGPDAI_EMOTION_TYPE_NORMAL = 0,				// 보통
	AGPDAI_EMOTION_TYPE_PEACE,					// 평화로움
	AGPDAI_EMOTION_TYPE_NERVOUS,				// 흥분
	AGPDAI_EMOTION_TYPE_ANGER,					// 광분
	AGPDAI_EMOTION_MAX_TYPE
} AgpdAIEmotionType;

class AgpdAIScream
{
public:
	bool				m_bUseScream;
	INT32				m_lFearLV1Agro;
	INT32				m_lFearLV2HP;
	bool				m_bFearLV3;

	INT32				m_lHelpLV1CopyAgro;
	INT32				m_lHelpLV2Heal;
	INT32				m_lHelpLV3;
	//INT32				m_alHelpCharTID[AGPDAI_MAX_HELP_CHARTYPE_COUNT];
	ApSafeArray<INT32, AGPDAI_MAX_HELP_CHARTYPE_COUNT>	m_alHelpCharTID;
	bool				m_bHelpAll;
};

class AgpdAIUseItem
{
public:
	INT32				m_lLastTime;
	INT32				m_lHP;
//	INT32				m_alTID[AGPDAI_MAX_USABLE_ITEM_COUNT];
//	INT32				m_alRate[AGPDAI_MAX_USABLE_ITEM_COUNT];

	ApSafeArray<INT32, AGPDAI_MAX_USABLE_ITEM_COUNT>	m_alTID;
	ApSafeArray<INT32, AGPDAI_MAX_USABLE_ITEM_COUNT>	m_alRate;

	AgpdAIUseItem()
	{
		m_lLastTime = 0;
		m_lHP = 0;
//		memset( m_alTID, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_ITEM_COUNT );
//		memset( m_alRate, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_ITEM_COUNT );

		m_alTID.MemSetAll();
		m_alRate.MemSetAll();
	}
};

class AgpdAIUseSkill
{
public:
	INT32				m_lLastTime;
	INT32				m_lHP;
//	INT32				m_alTID[AGPDAI_MAX_USABLE_SKILL_COUNT];
//	INT32				m_alRate[AGPDAI_MAX_USABLE_SKILL_COUNT];

	ApSafeArray<INT32, AGPDAI_MAX_USABLE_SKILL_COUNT>	m_alTID;
	ApSafeArray<INT32, AGPDAI_MAX_USABLE_SKILL_COUNT>	m_alRate;

	AgpdAIUseSkill()
	{
		m_lLastTime = 0;
		m_lHP = 0;
//		memset( m_alTID, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_SKILL_COUNT );
//		memset( m_alRate, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_SKILL_COUNT );

		m_alTID.MemSetAll();
		m_alRate.MemSetAll();
	}
};

struct AgpdAITargetWeight
{
	INT8				m_lRelationAggressive;	// Relation이 -일수록 높은 가중치							( * -Relation)
	INT8				m_lRelationFriendly;	// Relation이 +일수록 높은 가중치							( * Relation)
	INT8				m_lAgro;				// Agro Point가 높을수록 높은 가중치						( * Agro / 1000)
	INT8				m_lHistoryDamage;		// History가 Damage일수록 높은 가중치						( * Damage * 10 / MaxHP)
	INT8				m_lHistoryBuffer;		// History가 Buff일수록 높은 가중치							( * Buff * 10 / MaxHP)
	INT8				m_lNear;				// 거리가 가까울수록 높은 가중치							( * 10 * (1 - Distance / Visibility))
	INT8				m_lFar;					// 거리가 멀수록 높은 가중치								( * 10 * (Distance / Visibility))
	INT8				m_lStrong;				// Target이 자신보다 셀수록 높은 가중치						( * -LevelDiff)
	INT8				m_lWeak;				// Target이 자신보다 약할수록 높은 가중치					( * Leveldiff)
	INT8				m_lBoss;				// Target이 Boss일때 가지는 가중치
	INT8				m_lFollower;			// Target이 Follower일때 가지는 가중치
	INT8				m_lSelf;				// Target이 Self일때 가지는 가중치
	INT8				m_lOther;				// Target이 Self가 아닐때 가지는 가중치
};

struct AgpdAITarget
{
	ApBase				m_csTargetBase;			// Target의 Base Type과 ID(필요하다면)
	INT32				m_lTargetFlags;			// 반드시 필요한 수치 Flag (이 Flag의 계산값이 0이면, Target이 될 수 없다.)
	AgpdAITargetWeight	m_stTargetWeight;		// Target을 선정하는 데 필요한 수치들
	AuPOS				m_stTargetPos;			// Target Position
	FLOAT				m_fRadius;				// Target Radius
};

struct AgpdAIAction
{
	AgpdAIActionType	m_eActionType;			// Action Type
	INT32				m_lActionRate;			// Action Rate

	AgpdAITarget		m_stTarget;

	AgpdAIAction *		m_pstNext;				// 실제 적용된(계산된) Action 중 다음 순위
};

struct AgpdAIFactorCharFactor
{
	INT32				m_lCompareValue;
	AgpdFactor			m_stFactor;
};

struct AgpdAIFactorCharStatus
{
	AgpdCharStatus		m_eStatus;
};

struct AgpdAIFactorCharItem
{
	INT32				m_lItemTID;
};

struct AgpdAIFactorActionHistory
{
};

struct AgpdAIFactorEnvPosition
{
};

struct AgpdAIFactorEmotion
{
	AgpdAIEmotionType	m_eEmotionType;
};

struct AgpdAIActionResult
{
	AgpdAIActionType	m_eActionType;
	INT32				m_lActionRate;
	AgpdAIAction *		m_pstActions;			// 실제 적용된(계산된) Action들의 Rate순 정렬
};

struct AgpdAIFactor
{
	INT32				m_lFactorID;			// AIFactor ID (내부적으로 사용)
	AgpdAIFactorType	m_eType;				// Factor Type
	union
	{
		PVOID						m_pvData;
		AgpdAIFactorCharFactor *	m_pstCharFactor;
		AgpdAIFactorCharStatus *	m_pstCharStatus;
		AgpdAIFactorCharItem *		m_pstCharItem;
		AgpdAIFactorActionHistory *	m_pstActionHistory;
		AgpdAIFactorEnvPosition *	m_pstEnvPosition;
		AgpdAIFactorEmotion *		m_pstEmotion;
	} m_uoData;									// 각 Factor Type에 대한 Data

	//AgpdAIAction		m_astActions[AGPDAI_ACTION_MAX_TYPE];	// Action 정의
	ApSafeArray<AgpdAIAction, AGPDAI_ACTION_MAX_TYPE>	m_astActions;
};

typedef struct
{
	FLOAT					m_fVisibility;				// 시야거리
	UINT32					m_ulProcessInterval;

	//아이템 사용조건.
	AgpdAIUseItem			m_csAIUseItem;
	//스킬 사용조건
	AgpdAIUseItem			m_csAIUseSkill;
	//Scream 조건
	AgpdAIScream			m_csAIScream;

	AuList< AgpdAIFactor >	m_listAIFactors;
} AgpdAI;

typedef struct
{
	INT32					m_lID;
	CHAR					m_szName[AGPDAI_TEMPLATE_NAME_LENGTH];

	AgpdAI					m_stAI;
} AgpdAITemplate;

typedef struct
{
	BOOL					m_bUseAI;
	INT32					m_lTID;

	AgpdAI					m_stAI;

	UINT32					m_ulPrevProcessTime;
	//AgpdAIActionResult		m_astResult[AGPDAI_ACTION_MAX_TYPE];
	ApSafeArray<AgpdAIActionResult, AGPDAI_ACTION_MAX_TYPE>	m_astResult;
} AgpdAIADChar;

typedef struct
{
	//AgpdAIADChar			m_astAIADChar[AGPDSPAWN_MAX_CHAR_NUM];
	ApSafeArray<AgpdAIADChar, AGPDSPAWN_MAX_CHAR_NUM>		m_astAIADChar;
} AgpdAIADSpawn;

//Agro계산용 구조체
typedef enum
{
	AGPDAI_AGRO_TYPE_TRY_ATTACK = 0,
	AGPDAI_AGRO_TYPE_DEMAGE,
	AGPDAI_AGRO_TYPE_TRY_SKILL,
	AGPDAI_AGRO_TYPE_SKILL,
	AGPDAI_AGRO_TYPE_TRY_HEAL,
	AGPDAI_AGRO_TYPE_HEAL,
	AGPDAI_AGRO_TYPE_TRY_BUFF,
	AGPDAI_AGRO_TYPE_BUFF,
	AGPDAI_AGRO_TYPE_TRY_DEBUFF,
	AGPDAI_AGRO_TYPE_DEBUFF,
	AGPDAI_AGRO_TYPE_TRY_TAUNT,
	AGPDAI_AGRO_TYPE_TAUNT,
	AGPDAI_AGRO_TYPE_MAX,
} AgpdAIAgroType;

#endif //_AGPDAI_H_
