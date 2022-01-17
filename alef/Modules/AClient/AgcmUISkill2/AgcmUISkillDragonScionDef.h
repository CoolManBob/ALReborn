#pragma once

#define			DRAGONSCION_SCION		1724
#define			DRAGONSCION_OBITER		1723
#define			DRAGONSCION_SLAYER		1722
#define			DRAGONSCION_SUMMERNER	1732

#define			UI_NAME_SCION			"UI_SkillDragonScion_Scion"
#define			UI_NAME_SLAYER			"UI_SkillDragonScion_Sla"
#define			UI_NAME_OBITER			"UI_SkillDragonScion_Obiter"
#define			UI_NAME_PASSIVE			"UI_SkillDragonScion_Pas"
#define			UI_NAME_SUMMERNER		"UI_SkillDragonScion_Sum"
#define			UI_NAME_ARCHLORD		"UI_ScionArchlordSkill"

#define			UI_NAME_UP_SCION		"UI_SkillUpDragonScion_Scion"
#define			UI_NAME_UP_SLAYER		"UI_SkillUpDragonScion_Sla"
#define			UI_NAME_UP_OBITER		"UI_SkillUpDragonScion_Obiter"
#define			UI_NAME_UP_PASSIVE		"UI_SkillUpDragonScion_Pas"
#define			UI_NAME_UP_SUMMERNER	"UI_SkillUpDragonScion_Sum"


#define			DRAGONSCION_ARCHLORD_SKILL_COUNT		17

// Skill UI Type
enum eDragonScionSkillUIType
{
	SkillUI_NotOpened = 0,
	SkillUI_Scion		,
	SkillUI_Slayer		,
	SkillUI_Obiter		,
	SkillUI_Summerner	,
	SkillUI_Heroic		,
	SkillUI_Passive		,
	SkillUI_Archlord,

	SkillUI_Count		,
};

enum eScionEvent
{
	EVENT_SCION			,
	EVENT_SLAYER		,
	EVENT_OBITER		,
	EVENT_SUMMERNER		,
	EVENT_HEROIC		,
	EVENT_PASSIVE		,
	EVENT_ARCHLORD		,

	EVENT_COUNT			,
};


// 시온 스킬
enum	eDragonScionSkill_Scion
{
	SKILL_SCION_0				,

	SKILL_SCION_1_1				,
	SKILL_SCION_1_2				,
	SKILL_SCION_1_3				,

	SKILL_SCION_2_1				,
	SKILL_SCION_2_2				,
	SKILL_SCION_2_3				,

	SKILL_SCION_3_1				,
	SKILL_SCION_3_2				,

	SKILL_SCION_4_1				,
	SKILL_SCION_4_2				,
	SKILL_SCION_4_3				,

	SKILL_SCION_5_1				,
	SKILL_SCION_5_2				,
	SKILL_SCION_5_3				,

	SKILL_SCION_6_1				,
	SKILL_SCION_6_2				,
	SKILL_SCION_6_3				,

	SKILL_SCION_COUNT			,
};

enum	eDragonScionSKill_Obiter
{
	SKILL_OBITER_1_1			,
	SKILL_OBITER_1_2			,
	SKILL_OBITER_1_3			,
	SKILL_OBITER_1_4			,
	SKILL_OBITER_1_5			,

	SKILL_OBITER_2_1			,
	SKILL_OBITER_2_2			,
	SKILL_OBITER_2_3			,

	SKILL_OBITER_3_1			,
	SKILL_OBITER_3_2			,
	SKILL_OBITER_3_3			,
	SKILL_OBITER_3_4			,

	SKILL_OBITER_COUNT			,

};

enum	eDragonScionSKill_Slayer
{
	SKILL_SLAYER_1_1			,
	SKILL_SLAYER_1_2			,
	SKILL_SLAYER_1_3			,
	SKILL_SLAYER_1_4			,

	SKILL_SLAYER_2_1			,
	SKILL_SLAYER_2_2			,
	SKILL_SLAYER_2_3			,
	SKILL_SLAYER_2_4			,

	SKILL_SLAYER_3_1			,
	SKILL_SLAYER_3_2			,
	SKILL_SLAYER_3_3			,

	SKILL_SLAYER_COUNT			,

};

enum	eDragonScionSkill_Summener
{
	SKILL_SUMMENER_1_1			,
	SKILL_SUMMENER_1_2			,
	SKILL_SUMMENER_1_3			,
	SKILL_SUMMENER_1_4			,
	SKILL_SUMMENER_1_5			,

	SKILL_SUMMENER_2_1			,
	SKILL_SUMMENER_2_2			,
	SKILL_SUMMENER_2_3			,
	SKILL_SUMMENER_2_4			,

	SKILL_SUMMENER_3_1			,
	SKILL_SUMMENER_3_2			,
	SKILL_SUMMENER_3_3			,

	SKILL_SUMMENER_COUNT		,

};

enum	eDragonScionSkill_Passive
{
	SKILL_PASSIVE_1_1			,
	SKILL_PASSIVE_1_2			,
	SKILL_PASSIVE_1_3			,
	
	SKILL_PASSIVE_2_1			,
	SKILL_PASSIVE_2_2			,
	SKILL_PASSIVE_2_3			,

	SKILL_PASSIVE_3_1			,
	SKILL_PASSIVE_3_2			,
	SKILL_PASSIVE_3_3			,

	SKILL_PASSIVE_4_1			,
	SKILL_PASSIVE_4_2			,
	SKILL_PASSIVE_4_3			,

	SKILL_PASSIVE_5_1			,
	SKILL_PASSIVE_5_2			,
	SKILL_PASSIVE_5_3			,

	SKILL_PASSIVE_COUNT			,
};


enum	eDragonScionSKill_Status
{
	SKILL_STATUS_SUCCESS		,
	SKILL_STATUS_OTHER_CLASS	,
	SKILL_STATUS_FAIL			,
};

#define		TOTAL_SCIONSKILL_COUNT	( SKILL_PASSIVE_COUNT + SKILL_SUMMENER_COUNT + SKILL_OBITER_COUNT + SKILL_SCION_COUNT + SKILL_SLAYER_COUNT )