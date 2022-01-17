#ifndef	_APDEVENT_H_
#define _APDEVENT_H_

#include "ApBase.h"
#include "ApMemory.h"

#include "AgpmFactors.h"

#define APDEVENT_MAX_EVENT					5
#define APDEVENT_FUNCTION_VALID(function)	((function) >= APDEVENT_FUNCTION_NONE && (function) < APDEVENT_MAX_FUNCTION)

#define APDEVENT_EID_FLAG_LOCAL				0x40000000

#define APDEVENT_TARGET_NUMBER				3

typedef enum
{
	APDEVENT_FUNCTION_NONE					= 0,
	APDEVENT_FUNCTION_SPAWN					,
	APDEVENT_FUNCTION_FACTOR				,
	APDEVENT_FUNCTION_VEHICLE				,
	APDEVENT_FUNCTION_SCHEDULE				,
	APDEVENT_FUNCTION_HIDDEN				,
	APDEVENT_FUNCTION_SHOP					,
	APDEVENT_FUNCTION_INFORMATION			,
	APDEVENT_FUNCTION_TELEPORT				,
	APDEVENT_FUNCTION_NPCTRADE				,
	APDEVENT_FUNCTION_CONVERSATION			,
	APDEVENT_FUNCTION_NATURE				,
	APDEVENT_FUNCTION_STATUS				,
	APDEVENT_FUNCTION_ACTION				,
	APDEVENT_FUNCTION_SKILL					,
	APDEVENT_FUNCTION_SHRINE				,
	APDEVENT_FUNCTION_UVU_REWARD			,
	APDEVENT_FUNCTION_ITEM_REPAIR			,
	APDEVENT_FUNCTION_MASTERY_SPECIALIZE	,
	APDEVENT_FUNCTION_BINDING				,
	APDEVENT_FUNCTION_BANK					,
	APDEVENT_FUNCTION_NPCDAILOG				,
	APDEVENT_FUNCTION_ITEMCONVERT			,
	APDEVENT_FUNCTION_GUILD					,
	APDEVENT_FUNCTION_PRODUCT				,
	APDEVENT_FUNCTION_SKILLMASTER			,
	APDEVENT_FUNCTION_REFINERY				,
	APDEVENT_FUNCTION_QUEST					,
	APDEVENT_FUNCTION_AUCTION				,
	APDEVENT_FUNCTION_CHAR_CUSTOMIZE		,
	APDEVENT_FUNCTION_POINTLIGHT			,
	APDEVENT_FUNCTION_REMISSION				,
	APDEVENT_FUNCTION_WANTEDCRIMINAL		,
	APDEVENT_FUNCTION_SIEGEWAR_NPC			,
	APDEVENT_FUNCTION_TAX					,
	APDEVENT_FUNCTION_GUILD_WAREHOUSE		,
	APDEVENT_FUNCTION_ARCHLORD				,
	APDEVENT_FUNCTION_GAMBLE				,
	APDEVENT_FUNCTION_GACHA					,
	APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP	,
	APDEVENT_MAX_FUNCTION
} ApdEventFunction;

typedef enum
{
	APDEVENT_AREA_SPHERE		= 0	,
	APDEVENT_AREA_FAN				,
	APDEVENT_AREA_BOX				,
	APDEVENT_MAX_AREA
} ApdEventArea;

typedef enum
{
	APDEVENT_COND_NONE			= 0x00,
	APDEVENT_COND_TARGET		= 0x01,
	APDEVENT_COND_AREA			= 0x02,
	APDEVENT_COND_ENVIRONMENT	= 0x04,
	APDEVENT_COND_TIME			= 0x08
} ApdEventCondFlag;

struct ApdEventConditionTarget : public ApMemory<ApdEventConditionTarget, 20>
{
	AgpdFactor					m_stFactor;
	//INT32						m_alItemTID[APDEVENT_TARGET_NUMBER];
	ApSafeArray<INT32, APDEVENT_TARGET_NUMBER>	m_alItemTID;
	INT32						m_lCID;

	ApdEventConditionTarget::ApdEventConditionTarget()
	{
		ZeroMemory(&m_stFactor, sizeof(m_stFactor));
		m_alItemTID.MemSetAll();
		m_lCID = 0;
	}
};

struct	ApdEventConditionEnv : public ApMemory<ApdEventConditionEnv, 20>
{
};

struct ApdEventConditionTime : public ApMemory<ApdEventConditionTime, 20>
{
	INT32						m_lActiveTimeOffset;
	INT32						m_lEndTimeOffset;

	ApdEventConditionTime::ApdEventConditionTime()
	{
		m_lActiveTimeOffset	= 0;
		m_lEndTimeOffset	= 0;
	}
};

struct ApdEventConditionArea : public ApMemory<ApdEventConditionArea, 20000>
{
	ApdEventArea				m_eType;
	union
	{
		FLOAT					m_fSphereRadius;
		struct
		{
			FLOAT				m_fRadius;
			FLOAT				m_f2BySinThetaDiv2;
		} m_stPan;
		AuBOX					m_stBox;
	} m_uoData;

	ApdEventConditionArea::ApdEventConditionArea()
	{
		m_eType	= (ApdEventArea) 0;
		ZeroMemory(&m_uoData, sizeof(m_uoData));
	}
};

struct ApdEventCondition : public ApMemory<ApdEventCondition, 20000>
{
	ApdEventConditionTarget *	m_pstTarget;
	ApdEventConditionEnv *		m_pstEnvironment;
	ApdEventConditionTime *		m_pstTime;
	ApdEventConditionArea *		m_pstArea;

	ApdEventCondition::ApdEventCondition()
	{
		m_pstTarget			= NULL;
		m_pstEnvironment	= NULL;
		m_pstTime			= NULL;
		m_pstArea			= NULL;
	}
};

typedef struct
{
	INT32				m_lEID;				// APDEVENT_EID_FLAG_LOCAL 이 Set되어 있으면, Local Generated Number이다.
											// 아니면, Sync를 위한 Unique ID이다.
	ApdEventFunction	m_eFunction;
	PVOID				m_pvData;
	ApdEventCondition *	m_pstCondition;
	ApBase *			m_pcsSource;

	UINT32				m_ulEventStartTime;

	ApMutualEx			m_Mutex;
} ApdEvent;

typedef struct
{
	UINT16				m_unFunction;
	//ApdEvent			m_astEvent[APDEVENT_MAX_EVENT];
	ApSafeArray<ApdEvent, APDEVENT_MAX_EVENT>	m_astEvent;
} ApdEventAttachData;

#endif //_APDEVENT_H_