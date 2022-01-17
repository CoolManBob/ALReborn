#ifndef __AGSMAI2_H__
#define __AGSMAI2_H__

#include "AgsEngine.h"
#include "AgpmPathFind.h"
#include "AgpmEventSpawn.h"
#include "AgpmPvP.h"
#include "AgpmSkill.h"
#include "AgsmSkill.h"
#include "AgsmEventSpawn.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmChatting.h"
#include "AgsmCombat.h"
#include "AgsmSummons.h"
#include "AgpmEventNPCDialog.h"
#include "AgpmAI2.h"
#include "AuRandomNumber.h"
#include "AuMath.h"
#include "SpecializeListPathFind.h"
#include "AgpmSiegeWar.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAI2D" )
#else
#pragma comment ( lib , "AgsmAI2" )
#endif
#endif

enum AgsmAI2ResultMove
{
	AGSMAI2_RESULT_MOVE_FAIL = 0,
	AGSMAI2_RESULT_MOVE_WANDERING,
	AGSMAI2_RESULT_MOVE_TARGET,
	AGSMAI2_RESULT_MOVE_TARGET_IN_RANGE,
	AGSMAI2_RESULT_MOVE_TO_BOSS,
	AGSMAI2_RESULT_MOVE_RUNAWAY,
	AGSMAI2_RESULT_MOVE_LOOT,
	AGSMAI2_RESULT_MOVE_MAX
};

typedef enum AgsmAI2CB {
	AGSMAI2_CB_GET_TARGET		= 0,
};

enum AgsmAI2SpecificState {
	AGSMAI2_STATE_NONE			= 0,
	AGSMAI2_STATE_USE_SKILL,
	AGSMAI2_STATE_USE_ITEM,
	AGSMAI2_STATE_USE_LOOT_ITEM,
	AGSMAI2_STATE_USE_SCREAM,
	AGSMAI2_STATE_USE_TRANSPARENT,
	AGSMAI2_STATE_MAX
};

typedef std::vector<AgpdAI2LootItem> CLootItemVector;

#define		AGSMAI2_MAX_PROCESS_TARGET		10	// 2007.03.05. steeple 100 에서 10으로 줄였다.
#define		AGSMAI2_MAX_PROCESS_ITEM		10

class AgpmBattleGround;
class AgpmMonsterPath;

class AgsmAI2 : public AgsModule  
{
private:
	ApmMap *				m_pcsApmMap;
	AgpmPathFind *			m_pcsAgpmPathFind;
	AgpmFactors *			m_pcsAgpmFactors;
	AgsmFactors *			m_pcsAgsmFactors;
	ApmObject *				m_pcsApmObject;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmGrid *				m_pcsAgpmGrid;
	AgpmItem *				m_pcsAgpmItem;
	AgpmSummons *			m_pcsAgpmSummons;
	AgpmSkill *				m_pcsAgpmSkill;
	AgpmPvP *				m_pcsAgpmPvP;
	AgpmAI2 *				m_pcsAgpmAI2;
	AgsmCharacter *			m_pcsAgsmCharacter;
	ApmEventManager *		m_pcsApmEventManager;
	AgpmEventSpawn *		m_pcsAgpmEventSpawn;
	AgsmEventSpawn *		m_pcsAgsmEventSpawn;
	AgpmEventNPCDialog *	m_pcsAgpmEventNPCDialog;
	AgsmItem *				m_pcsAgsmItem;
	AgsmItemManager *		m_pcsAgsmItemManager;
	AgsmSkill *				m_pcsAgsmSkill;
	AgsmChatting *			m_pcsAgsmChatting;
	AgsmCombat *			m_pcsAgsmCombat;
	AgsmSummons *			m_pcsAgsmSummons;
	AgpmSiegeWar *			m_pcsAgpmSiegeWar;
	AgpmBattleGround*		m_pagpmBattleGround;
	AgpmMonsterPath*		m_pagpmMonsterPath;
	
	MTRand					m_csRand;
	int						m_lSummonAOIRange;
	int						m_lPetAOIRange;
	int						m_lPetAOIRange2;
	int						m_lScreamRefreshTime;

private:
	BOOL			CheckCondition( AgpdAI2ConditionTable *pcsConditionTbl, AgpdAI2UseResult *pcsUseResult, UINT32 lClockCount );
	BOOL			CheckParameter( INT32 lParameter, INT32 lMaxParameter, AgpdAI2ConditionTable *pcsConditionTbl );

	BOOL			InitReusedCharacterAI(AgpdCharacter *pcsCharacter, AgpdAI2Template *pcsAgpdAI2Template);

public:
	AgsmAI2();
	virtual ~AgsmAI2();

	BOOL			OnAddModule();
	BOOL			OnInit();

	BOOL			SetTargetPos( AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS *pcPos );
	BOOL			GetTargetPosFromSpawn(AgpdCharacter *pcsCharacter, AuPOS *pcsCurrentPos, AuPOS *pcsTargetPos, AuPOS *pcsSpawnBasePos, float fRadius );
		
	//공통 AI
	void			ProcessCommonAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount );

	//PC AI (PC AI이지만 Monster들도 사용함에 따라 bMonsterUse = TRUE를 넣어주면 Monster가 사용하는 AI
	BOOL			 ProcessPCAI(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount, eAgpmAI2StatePCAI *eResult = NULL, BOOL bMonsterUse = FALSE);
	
	//Detail PC AI
	BOOL			ProcessPCAIConfusion(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL			ProcessPCAIFear(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL			ProcessPCAIDisease(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL			ProcessPCAIBerserk(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL			ProcessPCAIShrink(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
//	BOOL			ProcessPCAIBerserkByMonster(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);

	//몬스터 관련 AI
	BOOL			ProcessAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount );
	//몬스터 AI 리뉴얼 
	BOOL			ProcessMonsterAI(AgpdCharacter *pcsCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL			ProcessExAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AgpdCharacter *pcsTargetPC, UINT32 lClockCount );

	// 소환수 관련 AI
	// 2005.08.03. steeple 수정
	BOOL			ProcessSummonAI( AgpdCharacter *pcsSummons, UINT32 lClockCount );

	//NPC관련 AI
	BOOL			ProcessGuardNPCAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount );

	BOOL			ProcessNPCDialog( ApBase *pcsApBase, eAgpdEventNPCDialogMob eDialogType );

	// 위치에 관련된 처리
	BOOL			ProcessCheckArea( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT lClockCount );
	BOOL			ProcessCheckTargetArea( AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter );


	AgpdCharacter	*GetTargetPC( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS csMobPos, float fPreemptiveRange );
	AgpdCharacter	*GetTargetForGuardNPC(AgpdCharacter *pcsAgpdNPCCharacter, AuPOS csNPCPos, AgpdAI2ADChar *pcsAgpdAI2ADChar );
	void			SetFollowersTarget( AgpdCharacter *pcsAgpdCharacter, AuPOS csMobPos, AgpdAI2ADChar *pcsAgpdAI2ADChar );

	// 2005.08.30. steeple
	AgpdCharacter*	GetAttackTargetPC(AgpdCharacter* pcsOwner, AgpdCharacter* pcsSummons, EnumAgpdSummonsPropensity ePropensity);
	BOOL			ProcessSetTargetToSummons(AgpdCharacter* pcsOwner, INT32 lTargetCID);

	BOOL			ProcessSpawnCastSkill(AgpdCharacter* pcsCharacter);

	INT32			CheckUseItem( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar,  UINT32 lClockCount );
	INT32			CheckUseSkill( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount );
	BOOL			CheckPreemptive( AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter, 
									AgpdAI2ADChar *pcsAgpdAI2ADChar, BOOL bUseScream = FALSE );
	BOOL			CheckUseScream( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount );
	BOOL			ProcessUseItem( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, INT32 lIndex, AgpdCharacter *pcsTargetPC );
	BOOL			ProcessUseSkill( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, INT32 lIndex, AgpdCharacter *pcsTargetPC );
	BOOL			ProcessUseScream( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AgpdCharacter *pcsTargetPC );	


	static BOOL		CBUpdateCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBPathFind(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBSpawnUsedCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBInitReusedCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBNPCChatting(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	//static BOOL		CBRemoveCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL		CBSetCombatMode(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBResetCombatMode(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	//static BOOL		CBCheckActionAttackTarget(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	// From AgsmCombat
	static BOOL		CBAttackStart(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmSkill
	static BOOL		CBAffectedSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmCharacter
	static BOOL		CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgpmCHaracter
	static BOOL		CBPinchWantedRequest(PVOID pData, PVOID pClass, PVOID pCusData);


	// Script function
	void SetSummonAOIRange(int lRange);
	void SetSummonPropensity(int lCID, int lPropensity);

	BOOL	SetCallbackGetTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL CheckEscape(AgpdCharacter* pcsTarget);

	BOOL ManagementFollower(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL CheckArea(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL AttackEnemy(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL MoveWandering(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL MonsterCommonAI(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);

	BOOL RunawayFromPC(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter* pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL LootItem(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	BOOL GetRunawayPosFromPC(AgpdCharacter *pcsAgpdCharacter, AuPOS *pcsMobPos, AuPOS *pcsTargetPCPos, AuPOS *pcsMobTargetPos);
	BOOL CheckRunaway(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount);
	BOOL IsRunawayStateCharacter(AgpdCharacter *pcsAgpdAI2AdChar);

	BOOL CheckHideMonster(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount);
	BOOL ProcessTransparentMonster(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount);

	BOOL SetPinchMonster(AgpdCharacter *pcsCharacter);
	
	AgsmAI2ResultMove MoveAI(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	AgsmAI2SpecificState SpecificStateAI(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount);
	AgpdCharacter* FindEnemyTarget(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS csMobPos, float fPreemptiveRange);
	// profile
#ifdef __PROFILE__
	void SetProfileDelay(int delay);	// ms
	void Sleep1sec();
#endif
};

#endif