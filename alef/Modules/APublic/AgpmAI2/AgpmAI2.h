#ifndef __AGPMAI2_H__
#define __AGPMAI2_H__

#include "AuExcelTxtLib.h"
#include "ApModule.h"
#include "ApmMap.h"
#include "AgpmPathFind.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmEventSpawn.h"
#include "AgpmMonsterPath.h"

#include "AgpaAI2.h"
#include "AgpdAI2.h"

#include "AuPacket.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmAI2D" )
#else
#pragma comment ( lib , "AgpmAI2" )
#endif
#endif
//@} Jaewon

#define AGPMAI2_TEMPLATENAME				"TemplateName"
#define AGPMAI2_NPC_AI_TYPE					"NPCAIType"
#define AGPMAI2_ADJUST_WARRIOR				"AdjustWarrior"
#define AGPMAI2_ADJUST_ARCHER				"AdjustArcher"
#define AGPMAI2_ADJUST_MAGE					"AdjustMage"
#define AGPMAI2_ADJUST_MONK					"AdjustMonk"
#define AGPMAI2_AGGRESSIVE_POINT			"AggressivePoint"
#define AGPMAI2_ACTION_ADJUST_DAMAGE		"ActionAdjustDamage"
#define AGPMAI2_ACTION_ADJUST_TRY_ATTACK	"ActionAdjustTryAttack"
#define AGPMAI2_ACTION_DEBUFF				"ActionDebuff"
#define AGPMAI2_ACTION_TRYDEBUFF			"ActionTryDebuff"
#define AGPMAI2_ACTION_P2P_HEAL				"ActionP2PHeal"
#define AGPMAI2_ACTION_P2P_BUFF				"ActionP2PBuff"
#define AGPMAI2_USE_ITEM_COUNT				"UseItemCount"
#define AGPMAI2_USE_SKILL_COUNT				"UseSkillCount"
#define AGPMAI2_ITEM_TID					"ItemTID"
#define AGPMAI2_SKILL_TID					"SkillTID"
#define	AGPMAI2_TARGET_PARAMETER			"TargetParameter"
#define AGPMAI2_PARAMATER					"Parameter"
#define AGPMAI2_PERCENT						"Percent"
#define AGPMAI2_OPERATOR					"Operator"
#define AGPMAI2_CONDITION_CHECK				"ConditionCheck"
#define AGPMAI2_TIMERCOUNT					"TimerCount"
#define AGPMAI2_PROBABLE					"Probable"

//AI Tool 전용 데이터
#define AGPMAI2_TID							"TID"
#define AGPMAI2_Name						"NAME"
#define AGPMAI2_UseNormalAttack				"UseNormalAttack"

#define AGPMAI2_HateRace					"HateRace"
#define AGPMAI2_HateRacePercent				"HateRacePercent"
#define AGPMAI2_HateClass					"HateClass"
#define AGPMAI2_HateClassPercent			"HateClassPercent"
#define AGPMAI2_HateAction					"HateAction"
#define AGPMAI2_HateActionPercent			"HateActionPercent"

#define AGPMAI2_FellowWorkerTID				"FellowWorkerTID"
#define AGPMAI2_FellowWorkerName			"FellowWorkerName"
#define AGPMAI2_FellowWorkerRange			"FellowWorkerRange"
#define AGPMAI2_FellowWorkerAcceptCount		"FellowWorkerAcceptCount"
#define AGPMAI2_FellowWorkerRefreshTime		"FellowWorkerRefreshTime"

#define AGPMAI2_FollowerTID					"FollowerTID"
#define AGPMAI2_FollowerName				"FollowerName"
#define AGPMAI2_FollowerNum					"FollowerNUM"
#define AGPMAI2_Max_CumulativeFollower		"Max_CumulativeFollower"
#define AGPMAI2_Min_UpkeepFollower			"Min_UpkeepFollower"
#define AGPMAI2_FollowerRoyalty				"FollowerRoyalty"

#define AGPMAI2_SearchEnemy					"SearchEnemy"
#define AGPMAI2_StopFight					"StopFight"
#define AGPMAI2_Escape						"Escape"

#define AGPMAI2_SKill1						"Skill1"
#define AGPMAI2_SKill2						"Skill2"
#define AGPMAI2_SKill3						"Skill3"
#define AGPMAI2_SKill4						"Skill4"
#define AGPMAI2_SKill5						"Skill5"
#define AGPMAI2_Item1						"Item1"
#define AGPMAI2_Item2						"Item2"
#define AGPMAI2_Item3						"Item3"
#define AGPMAI2_Item4						"Item4"
#define AGPMAI2_Item5						"Item5"

#define AGPMAI2_Guard						"Guard"
#define AGPMAI2_PreemptiveType				"PreemptiveType"

#define AGPMAI2_PreemptiveCondition1		"PreemptiveCondition1"
#define AGPMAI2_PreemptiveCondition2		"PreemptiveCondition2"
#define AGPMAI2_PreemptiveCondition3		"PreemptiveCondition3"

#define AGPMAI2_PreemptiveRange				"PreemptiveRange"

#define AGPMAI2_Immunity					"Immunity"
#define AGPMAI2_Attribute					"Attribute"

#define AGPMAI2_RaceHuman					"휴먼"
#define AGPMAI2_RaceOrc						"오크"

#define AGPMAI2_ClassWarrior				"전사"
#define AGPMAI2_ClassArcher					"궁수"
#define AGPMAI2_ClassMage					"법사"

#define AGPMAI2_SummonTID					"SummonTID"
#define AGPMAI2_SummonMaxCount				"SummonMaxCount"

// 새로운 MonsterAI관련 전용 데이터
#define AGPMAI2R_TID							"TID"
#define AGPMAI2R_Name							"NAME"
#define AGPMAI2R_UseNormalAttack				"UseNormalAttack"

#define AGPMAI2R_HateRace						"HateRace"
#define AGPMAI2R_HateRacePercent				"HateRacePercent"
#define AGPMAI2R_HateClass						"HateClass"
#define AGPMAI2R_HateClassPercent				"HateClassPercent"

#define AGPMAI2R_FellowWorkerTID				"FellowWorkerTID"
#define AGPMAI2R_FellowWorkerName				"FellowWorkerName"
#define AGPMAI2R_FellowWorkerRange				"FellowWorkerRange"
#define AGPMAI2R_FellowWorkerAcceptCount		"FellowWorkerAcceptCount"
#define AGPMAI2R_FellowWorkerRefreshTime		"FellowWorkerRefreshTime"

#define AGPMAI2R_FollowerTID					"FollowerTID"
#define AGPMAI2R_FollowerName					"FollowerName"
#define AGPMAI2R_FollowerNum					"FollowerNUM"
#define AGPMAI2R_Max_CumulativeFollower			"Max_CumulativeFollower"
#define AGPMAI2R_Min_UpkeepFollower				"Min_UpkeepFollower"
#define AGPMAI2R_FollowerRoyalty				"FollowerRoyalty"

#define AGPMAI2R_SKill1							"Skill1"
#define AGPMAI2R_SKill2							"Skill2"
#define AGPMAI2R_SKill3							"Skill3"
#define AGPMAI2R_SKill4							"Skill4"
#define AGPMAI2R_SKill5							"Skill5"
#define AGPMAI2R_SKillUseHPLimit				"SkillCondition"
#define AGPMAI2R_SKillUseNumber					"SkillCount"

#define AGPMAI2R_Item1							"Item1"
#define AGPMAI2R_ItemUseHPLimit					"ItemUseHPLimit"
#define AGPMAI2R_ItemUseNumber					"ItemUseNumber"
#define AGPMAI2R_ItemUseInterval				"ItemUseInterval"

#define AGPMAI2R_Guard							"Guard"
#define AGPMAI2R_GuardObject					"GuardObject"
#define AGPMAI2R_PreemptiveType					"PreemptiveType"

#define AGPMAI2R_PreemptiveCondition1			"PreemptiveCondition1"
#define AGPMAI2R_PreemptiveCondition2			"PreemptiveCondition2"
#define AGPMAI2R_PreemptiveCondition3			"PreemptiveCondition3"

#define AGPMAI2R_PreemptiveRange				"PreemptiveRange"

#define AGPMAI2R_Immunity						"Immunity"
#define AGPMAI2R_Attribute						"Attribute"

#define AGPMAI2R_SummonTID						"SummonTID"
#define AGPMAI2R_SummonMaxCount					"SummonMaxCount"

#define AGPMAI2R_SearchEnemy					"SearchEnemy"
#define AGPMAI2R_StopFight						"StopFight"
#define AGPMAI2R_Escape							"Escape"
#define AGPMAI2R_EscapeHP						"EscapeHP"
#define AGPMAI2R_EscapeTime						"EscapeTime"

#define AGPMAI2R_LinkMonsterTID					"LinkMonsterTID"
#define AGPMAI2R_LinkMonsterSight				"LinkMonsterSight"
#define AGPMAI2R_HideMonster					"HideMonster"
#define AGPMAI2R_HideMonsterHP					"HideMonsterHP"
#define AGPMAI2R_HideMonsterTime				"HideMonsterTime"

#define AGPMAI2R_MaintenanceRange				"MaintenanceRange"
#define AGPMAI2R_Helper							"Helper"
#define AGPMAI2R_LootItem						"LootItem"
#define AGPMAI2R_LootItemTime					"LootItemTime"
#define AGPMAI2R_RandomTarget					"RandomTarget"

#define AGPMAI2R_RaceHuman						"인간"
#define AGPMAI2R_RaceOrc						"오크"
#define AGPMAI2R_RaceMoonElf					"문엘프"
#define AGPMAI2R_RaceScion						"시온"

#define AGPMAI2R_ClassWarrior					"전사"
#define AGPMAI2R_ClassArcher					"궁수"
#define AGPMAI2R_ClassMage						"법사"
#define AGPMAI2R_ClassScion						"시온"

#define AGPMAI2R_SKILLARG_HP					"HP"
#define AGPMAI2R_SKILLARG_OPERATOR				"이하"
#define AGPMAI2R_SKILLARG_COUNT					"횟수"
#define AGPMAI2R_SKILLARG_DEFAULT_COUNT			"1"

#define AGPMAI2R_PINCH_MONSTER					"PinchMonster"
#define AGPMAI2R_VISIBILITY						"Visibility"




typedef enum
{
	AGPMAI2_DATA_TEMPLATE	= 0,
} AgpmAI2Data;

typedef enum
{
	AGPMAI2_TARGET_AGGRO_NONE = 0,
	AGPMAI2_TARGET_AGGRO_TARGET,		//타켓한 상대의 상태에 따라.
	AGPMAI2_TARGET_AGGRO_MOBSELF,		//몹 자신의 상태에 따라.
	AGPMAI2_TARGET_AGGRO_TIME,			//시간에 따라(밤,낮등...)
	AGPMAI2_TARGET_AGGRO_NATURE			//자연 상태(날씨)
} AgpmAI2DataAggroParameter;

typedef enum
{
	AGPMAI2_TARGET_PARAMETER_NONE = 0,
	AGPMAI2_TARGET_PARAMETER_HP,
	AGPMAI2_TARGET_PARAMETER_MP,
	AGPMAI2_TARGET_PARAMETER_SP,
	AGPMAI2_TARGET_PARAMETER_LV,
	AGPMAI2_TARGET_PARAMETER_RACE,
	AGPMAI2_TARGET_PARAMETER_CLASS,
	AGPMAI2_TARGET_PARAMETER_TIME,
	AGPMAI2_MAX_TARGETPARAMETER
} AgpmAI2DataTargetParameter;

typedef enum
{
	AGPMAI2_BASE_STATIC = 1,
	AGPMAI2_BASE_PERCENT,
} AgpmAI2DataPercent;

typedef enum
{
	AGPMAI2_OPERATOR_NONE = 0,
	AGPMAI2_OPERATOR_EQUAL,
	AGPMAI2_OPERATOR_NOT_EQUAL,
	AGPMAI2_OPERATOR_BIGGER,
	AGPMAI2_OPERATOR_BIGGER_EQUAL,
	AGPMAI2_OPERATOR_LESS,
	AGPMAI2_OPERATOR_LESS_EQUAL,
	AGPMAI2_MAX_OPERATOR
} AgpmAI2DataOperator;

typedef enum
{
	AGPMAI2_CONDITION_NONE = 0,
	AGPMAI2_CONDITION_TIMER,
	AGPMAI2_CONDITION_COUNT,
	AGPMAI2_CONDITION_INFINITY,
	AGPMAI2_MAX_CONDION_CHECK
}AgpmAI2DataConditionCheck;

class AgpmAI2 : public ApModule 
{
	ApmMap				*m_pcsApmMap;
	AgpmPathFind		*m_pcsAgpmPathFind;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmSkill			*m_pcsAgpmSkill;
	AgpmEventSpawn		*m_pcsAgpmEventSpawn;
	AgpmMonsterPath		*m_pcsAgpmMonsterPath;

	INT16				m_nCharacterAttachAI2Index;
	INT16				m_nSpawnAttachIndex;
	AuExcelTxtLib		m_csAuExcelTxtLib;

public:
	AgpaAI2Template		m_aAI2Template;

	AgpmAI2();
	virtual ~AgpmAI2();

	BOOL				OnAddModule();
	BOOL				OnInit();

	INT32				GetMaxFellowerWorkers( AgpdAI2Template *pcsTemplate );
	INT32				GetMaxFellowRange(AgpdAI2Template *pcsTemplate );
	INT32				GetMaxFellowAcceptWorkers(AgpdAI2Template *pcsTemplate );
	INT32				GetMaxFollowers( AgpdAI2Template *pcsTemplate );
	INT32				GetUpkeepFollowers( AgpdAI2Template *pcsTemplate );
	INT32				GetCumulativeFollowers( AgpdAI2Template *pcsTemplate );

	INT32				GetEmptyTemplateID();

	INT32				StreamWriteTemplate(CHAR *szFile);
	INT32				StreamReadTemplate(CHAR *szFile);

	static BOOL		CBAI2TemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL		CBAI2TemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL		ConAgpdAI2ADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		DestAgpdAI2ADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdAI2ADChar	*GetCharacterData(AgpdCharacter *pcsCharacter);

	bool			LoadAITemplate( char *pstrFileName );
	BOOL			LoadMonsterAITemplate( char *pstrFileName );

	INT32			GetEmptyRaceClassSlot( AgpdAI2RaceClassAdjust *pcsRaceClassAdjust );
	INT32			GetEmptyActionSlot( AgpdAI2ActionAdjust *pcsActionAdjust );
	INT32			GetEmptyFellowWorkerSlot( AgpdAI2FellowWorker *pcsFellowWorker );
	BOOL			IsFellowWorker( AgpdAI2FellowWorker *pcsFellowWorker, INT32 lTID );
	INT32			GetEmptyFollowerSlot( AgpdAI2Follower *pcsFollower );
	INT32			GetEmptySKillSlot( AgpdAI2Template *pcsAI2Template );
	INT32			GetEmptyItemSlot( AgpdAI2Template *pcsAI2Template );
	INT32			GetEmptyPreemptiveSlot(AgpdAI2Template *pcsAI2Template );

	BOOL			SetEmptyRaceClassSlot( AgpdAI2RaceClassAdjust *pcsRaceClassAdjust, INT32 lRace, INT32 lClass, INT32 lHateAdjust, INT32 lHateClassAdjust );
	BOOL			SetEmptyActionSlot( AgpdAI2ActionAdjust *pcsActionAdjust, eAgpdAI2ActionAdjust eAdjustAction, INT32 lHateAdjust );
	BOOL			SetEmptyFellowWorkerSlot( AgpdAI2FellowWorker *pcsFellowWorker, INT32 lFellowWorkerTID, char *pstrCharName );
	BOOL			SetEmptyFollowerSlot( AgpdAI2Follower *pcsFollower, INT32 lFollowerTID, INT32 lMaxFollowerCount );

	BOOL			SetSkill( AgpdAI2Template *pcsTemplate, char *pstrSkillTID, char *pstrSkillLevel, char *pstrArg, char *pstrScalar, char *pstrOperator, char *pstrCountTime, char *pstrCount );
	BOOL			SetItem( AgpdAI2Template *pcsTemplate, char *pstrItemTID, char *pstrArg, char *pstrScalar, char *pstrOperator, char *pstrCountTime, char *pstrCount );
	BOOL			SetPreemptive( AgpdAI2Template *pcsTemplate, char *pstrAggroTarget, char *pstrArg, char *pstrScalar, char *pstrOperator, char *pstrAccept );

	BOOL			ResetRemoveBossAI(AgpdCharacter *pcsCharacter);

	BOOL			CheckIsBossMob(AgpdAI2ADChar *pcsAgpdAI2ADChar);
	BOOL			CheckHavePinchWantedMonster(AgpdCharacter *pcsCharacter);
	AgpdCharacter*	CheckHavePinchMonster(AgpdCharacter *pcsCharacter);

	eAgpmAI2StatePCAI GetPCAIState(AgpdCharacter *pcsCharacter);
	BOOL			  InitPCAIInfo(AgpdCharacter *pcsCharacter);
	
	
	STMonsterPathInfo* GetMonsterPathInfo(AgpdCharacter *pcsCharacter);

	static BOOL		CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif