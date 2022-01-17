#include "AgsmAI2.h"
#include "ApAutoLockCharacter.h"
#include "AgpmBattleGround.h"
#include "AgpmMonsterPath.h"
#include "AgppCharacter.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const float			AGSMAI2_Radian				= 3.141592f/180.0f;
const float			AGSMAI2_Visibility			= 2500.0f;
const float			AGSMAI2_Summons_Visibility	= 1500.0f;		// 소환수 시야는 15미터. 2005.10.28. steeple
const float			AGSMAI2_Summons_Max_Follow	= 2000.0f;		// 소환수가 주인에게 강제로 따라가게끔 하는 거리. 2007.07.25. steeple
const float			AGSMAI2_Summons_Max_Jump	= 2500.0f;		// 소환수가 주인에게 강제로 점프되게끔 하는 거리. 2007.07.25. steeple
const float			AGSMAI2_Pet_Max_Jump		= 1500.0f;		// 펫이 주인에게 강제로 점프되게끔 하는 거리.

AgsmAI2::AgsmAI2()
{
	SetModuleName("AgsmAI2");
	m_lSummonAOIRange = 500;
	m_lPetAOIRange = 200;
	m_lPetAOIRange2 = 300;
	
	m_pagpmBattleGround	= NULL;
}

AgsmAI2::~AgsmAI2()
{

}

BOOL			AgsmAI2::OnAddModule()
{
	// Parent Module들 가져오기
	m_pcsApmMap			= (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmPathFind	= (AgpmPathFind *) GetModule("AgpmPathFind");
	m_pcsAgpmFactors	= (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgsmFactors	= (AgsmFactors *) GetModule("AgsmFactors");
	m_pcsApmObject		= (ApmObject *) GetModule("ApmObject");
	m_pcsAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem		= (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmSummons	= (AgpmSummons *) GetModule("AgpmSummons");
	m_pcsAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmPvP		= (AgpmPvP *) GetModule("AgpmPvP");
	m_pcsAgpmAI2		= (AgpmAI2 *) GetModule("AgpmAI2");
	m_pcsAgpmEventNPCDialog = (AgpmEventNPCDialog *) GetModule( "AgpmEventNPCDialog" );
	m_pcsApmEventManager= (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmEventSpawn	= (AgpmEventSpawn *) GetModule("AgpmEventSpawn");
	m_pcsAgsmEventSpawn	= (AgsmEventSpawn *) GetModule("AgsmEventSpawn");
	m_pcsAgsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pcsAgsmItem		= (AgsmItem *) GetModule("AgsmItem");
	m_pcsAgsmSkill		= (AgsmSkill *) GetModule("AgsmSkill");
	m_pcsAgsmItemManager= (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pcsAgsmChatting	= (AgsmChatting *) GetModule("AgsmChatting");
	m_pcsAgsmCombat		= (AgsmCombat *) GetModule("AgsmCombat" );
	m_pcsAgsmSummons	= (AgsmSummons *) GetModule("AgsmSummons");
	m_pcsAgpmSiegeWar	= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");

	// Character모듈과 Public AI 있으면 일단 OK
	if ( !m_pcsApmMap || !m_pcsAgpmPathFind || !m_pcsAgpmCharacter || !m_pcsAgpmItem ||
		!m_pcsAgpmSummons || !m_pcsAgpmPvP || !m_pcsAgpmSkill ||
		!m_pcsAgpmAI2 || !m_pcsAgpmEventNPCDialog || !m_pcsAgsmCharacter || !m_pcsAgsmItem ||
		!m_pcsAgsmSkill || !m_pcsAgsmItemManager || !m_pcsAgsmChatting || !m_pcsAgsmCombat ||
		!m_pcsAgsmFactors || !m_pcsAgsmSummons ||
		!m_pcsAgpmSiegeWar
		)
		return FALSE;

	// Update Character Callback 등록하고
	if (!m_pcsAgpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;

	// MonsterPathFind관련 Callback 등록
	if (!m_pcsAgpmCharacter->SetCallbackMonsterAIPathFind( CBPathFind , this ))
		return FALSE;

	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackSpawn(CBSpawnCharacter, this))
		return FALSE;
	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackSpawnUsedData(CBSpawnUsedCharacter, this))
		return FALSE;
	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackInitReusedCharacter(CBInitReusedCharacter, this))
		return FALSE;

	if( !m_pcsAgsmCombat->SetCallbackNPCDialog( CBNPCChatting, this ) )
		return FALSE;

	if(!m_pcsAgsmCombat->SetCallbackAttackStart(CBAttackStart, this))
		return FALSE;

/*	if (m_pcsAgsmCharacter && !m_pcsAgsmCharacter->SetCallbackSendCharacterAllServerInfo(CBSendCharacterAllInfo, this))
		return FALSE;*/

	if (!m_pcsAgpmCharacter->SetCallbackSetCombatMode(CBSetCombatMode, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackResetCombatMode(CBResetCombatMode, this))
		return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackCheckActionAttackTarget(CBCheckActionAttackTarget, this))
	//	return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
	//	return FALSE;
	if (!m_pcsAgsmSkill->SetCallbackAffectedSkill(CBAffectedSkill, this))
		return FALSE;

	if(!m_pcsAgpmCharacter->SetCallbackPinchWantedRequest(CBPinchWantedRequest, this))
		return FALSE;

	return TRUE;
}	

BOOL AgsmAI2::OnInit()
{
	m_pagpmBattleGround	= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagpmMonsterPath = (AgpmMonsterPath*)GetModule("AgpmMonsterPath");
	
	if(!m_pagpmBattleGround || !m_pagpmMonsterPath)
		return FALSE;
	
	return TRUE;
}

BOOL AgsmAI2::CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pvData;
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;
	INT32				lAI2ID = *((INT32 *)pvCustData);

	if( !pcsCharacter || !pThis )
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSpawnCharacter"));

	AgpdAI2ADChar			*pcsAgpdAI2ADChar;

	pcsAgpdAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData( pcsCharacter );

	if( pcsAgpdAI2ADChar != NULL )
	{
		AgpdAI2Template			*pcsAI2Template;

		pcsAgpdAI2ADChar->m_csTargetPos = pcsCharacter->m_stPos;
		pcsAgpdAI2ADChar->m_csFirstSpawnPos = pcsCharacter->m_stPos;
		pcsAI2Template = pThis->m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( lAI2ID );
		//pcsCharacter->m_pcsCharacterTemplate->m_eCharType = AGPDCHAR_TYPE_MONSTER;

		if( pcsAI2Template == NULL )
		{
			//표준 1번 AI를 받아내서 디폴트 세팅을한다.
			pcsAI2Template = pThis->m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( 1 );
		}

		if( pcsAI2Template != NULL )
		{
			ApdEvent			*pcsEvent;
			AuMATRIX			*pstDirection;

			pcsEvent = NULL;

			pcsEvent = pThis->m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter)->m_pstEvent;

			if( pcsEvent )
			{
				pcsAgpdAI2ADChar->m_csSpawnBasePos = *pThis->m_pcsApmEventManager->GetBasePos( pcsEvent->m_pcsSource, &pstDirection );
				pcsAgpdAI2ADChar->m_fSpawnRadius = pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

				pcsAgpdAI2ADChar->m_lAttackInterval = (INT32)pThis->m_pcsAgpmCharacter->GetAttackIntervalMSec( pcsCharacter );
				pcsAgpdAI2ADChar->m_bUseAI2 = TRUE;
				pcsAgpdAI2ADChar->m_pcsAgpdAI2Template = pcsAI2Template;

				pcsAgpdAI2ADChar->m_lWrapPosition = pThis->m_csRand.randInt(360);

				//2004.10.25일 몬스터 대사 통합되면서 주석처리됨.
				//등장하며 대사를 읊조린다.
//				pThis->ProcessNPCDialog( (ApBase *)pcsCharacter, AGPD_NPCDIALOG_MOB_SPAWN );

				// 2005.09.06. steeple
				// Spawn 된 후에 바로 Cast 해야 할 스킬이 있으면 Cast 해준다.
				pThis->ProcessSpawnCastSkill(pcsCharacter);
			}
		}
	}

	return TRUE;
}

BOOL AgsmAI2::InitReusedCharacterAI(AgpdCharacter *pcsCharacter, AgpdAI2Template *pcsAgpdAI2Template)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdAI2ADChar			*pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData( pcsCharacter );

	if (pcsAgpdAI2Template)
		pcsAgpdAI2ADChar->m_pcsAgpdAI2Template	= pcsAgpdAI2Template;
	else
	{
		//표준 1번 AI를 받아내서 디폴트 세팅을한다.
		pcsAgpdAI2ADChar->m_pcsAgpdAI2Template	= m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( 1 );
	}

	pcsAgpdAI2ADChar->m_ulLastAttackTime	= 0;

	pcsAgpdAI2ADChar->m_ulPrevProcessTime	= 0;
	pcsAgpdAI2ADChar->m_ulProcessInterval	= 0;
	pcsAgpdAI2ADChar->m_ulLastPathFindTime	= 0;

	pcsAgpdAI2ADChar->m_ulNextWanderingTime	= 0;
	pcsAgpdAI2ADChar->m_ulNextDialogTargetPC	= 0;

	pcsAgpdAI2ADChar->m_csItemResult.MemSetAll();
	pcsAgpdAI2ADChar->m_csSkillResult.MemSetAll();

	pcsAgpdAI2ADChar->m_bScreamUsed	= FALSE;
	pcsAgpdAI2ADChar->m_ulScreamStartTime	= 0;

	pcsAgpdAI2ADChar->m_ulNextDecStaticTime	= 0;
	pcsAgpdAI2ADChar->m_ulNextDecSlowTime	= 0;
	pcsAgpdAI2ADChar->m_ulNextDecFastTime	= 0;

	pcsAgpdAI2ADChar->m_csTargetPos	= pcsCharacter->m_stPos;
	pcsAgpdAI2ADChar->m_csFirstSpawnPos = pcsCharacter->m_stPos;


	ApdEvent			*pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter)->m_pstEvent;

	if( pcsEvent )
	{
		AuMATRIX			*pstDirection;
		pcsAgpdAI2ADChar->m_csSpawnBasePos = *m_pcsApmEventManager->GetBasePos( pcsEvent->m_pcsSource, &pstDirection );
		pcsAgpdAI2ADChar->m_fSpawnRadius = pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

		pcsAgpdAI2ADChar->m_lAttackInterval = (INT32) m_pcsAgpmCharacter->GetAttackIntervalMSec( pcsCharacter );
		pcsAgpdAI2ADChar->m_bUseAI2 = TRUE;

	}

	pcsAgpdAI2ADChar->m_lTargetID	= AP_INVALID_CID;
	pcsAgpdAI2ADChar->m_lReadyTargetID	= AP_INVALID_CID;

	pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

	pcsAgpdAI2ADChar->m_pcsBossCharacter	= NULL;

	pcsAgpdAI2ADChar->m_pcsFollower.MemSetAll();

	pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower	= 0;
	pcsAgpdAI2ADChar->m_lCurrentFollowers	= 0;

	pcsAgpdAI2ADChar->m_lCastSkillLevel = 0;

	// 2005.09.06. steeple
	// Spawn 된 후에 바로 Cast 해야 할 스킬이 있으면 Cast 해준다.
	ProcessSpawnCastSkill(pcsCharacter);

	return TRUE;
}

BOOL AgsmAI2::CBSpawnUsedCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pvCustData;
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;

	if( !pcsCharacter || !pThis )
		return FALSE;

	return pThis->InitReusedCharacterAI(pcsCharacter, NULL);
}

BOOL AgsmAI2::CBInitReusedCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pData;
	AgsmAI2				*pThis = (AgsmAI2 *) pClass;

	if( !pcsCharacter || !pThis )
		return FALSE;

	return pThis->InitReusedCharacterAI(pcsCharacter, (AgpdAI2Template *) pCustData);
}

BOOL AgsmAI2::CBNPCChatting(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;
	ApBase				*pcsApBase = (ApBase *)pvData;
	eAgpdEventNPCDialogMob eDialogType = *((eAgpdEventNPCDialogMob *)pvCustData);

	BOOL				bResult;

	bResult = FALSE;

	if( pcsApBase )
	{
		bResult = pThis->ProcessNPCDialog( pcsApBase, eDialogType );
	}

	return bResult;
}

BOOL AgsmAI2::ProcessNPCDialog( ApBase *pcsApBase, eAgpdEventNPCDialogMob eDialogType )
{
	BOOL			bResult;
	bResult = FALSE;

	if( pcsApBase )
	{
		char			*pstrDialogText = NULL;
		INT32			lDialogLength = 0;

		//캐릭터라면......
		if( pcsApBase->m_eType == APBASE_TYPE_CHARACTER )
		{
			INT32			lNPCTID = ((AgpdCharacter *)pcsApBase)->m_pcsCharacterTemplate->m_lID;

			pstrDialogText = m_pcsAgpmEventNPCDialog->GetMobDialog( lNPCTID, eDialogType );

			if( pstrDialogText )
			{
				lDialogLength = (INT32)_tcslen( pstrDialogText );
				bResult = m_pcsAgsmChatting->SendMessageNormal( pcsApBase, pstrDialogText, lDialogLength );
			}
		}
	}
	
	return bResult;
}

BOOL AgsmAI2::CheckCondition( AgpdAI2ConditionTable	*pcsConditionTbl, AgpdAI2UseResult *pcsUseResult, UINT32 lClockCount )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsConditionTbl->m_lConditionCheck == AGPMAI2_CONDITION_TIMER )
	{
		if( (UINT32)(pcsUseResult->m_lClock + pcsConditionTbl->m_lTimerCount) < lClockCount )
		{
			pcsUseResult->m_lUsedCount++;
			pcsUseResult->m_lClock = lClockCount;
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lConditionCheck == AGPMAI2_CONDITION_COUNT )
	{
		if( pcsUseResult->m_lUsedCount < pcsConditionTbl->m_lMaxUsableCount )
		{
			pcsUseResult->m_lUsedCount++;
			pcsUseResult->m_lClock = lClockCount;
			bResult = TRUE;
		}
	}

	else if( pcsConditionTbl->m_lConditionCheck == AGPMAI2_CONDITION_INFINITY )
	{
		if( (UINT32)(pcsUseResult->m_lClock + pcsConditionTbl->m_lTimerCount) < lClockCount )
		{
			pcsUseResult->m_lUsedCount++;
			pcsUseResult->m_lClock = lClockCount;
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgsmAI2::CheckParameter( INT32 lParameter, INT32 lMaxParameter, AgpdAI2ConditionTable *pcsConditionTbl )
{
	INT32			lScalar;
	BOOL			bResult;

	bResult = FALSE;

	if( pcsConditionTbl->m_bPercent )
	{
		if( lMaxParameter > 0 )
		{
			lScalar =  (lParameter*100)/lMaxParameter;
		}
		else
		{
			return bResult;
		}
	}
	else
	{
		lScalar = lParameter;
	}

	if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_EQUAL )
	{
		if( lScalar == pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_NOT_EQUAL )
	{
		if( lScalar != pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_BIGGER )
	{
		if( lScalar > pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_BIGGER_EQUAL )
	{
		if( lScalar >= pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_LESS )
	{
		if( lScalar < pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_LESS_EQUAL )
	{
		if( lScalar <= pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgsmAI2::SetTargetPos( AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS *pcPos )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdAI2ADChar != NULL )
	{
		pcsAgpdAI2ADChar->m_csTargetPos = *pcPos;
		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmAI2::GetTargetPosFromSpawn(AgpdCharacter* pcsCharacter, AuPOS *pcsCurrentPos, AuPOS *pcsTargetPos, AuPOS *pcsSpawnBasePos, float fRadius )
{
	FLOAT	fX, fZ;
	FLOAT	fTempX, fTempZ;

	FLOAT	fTempRadius;
	AuPOS	stTempAuPos;
	BOOL	bResult;

	//10미터를 이동하는 거리로 잡는다.
	bResult = FALSE;
	fTempRadius = 1000.0f; 

	for (INT32 nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
	{
		fTempX = (FLOAT)m_csRand.rand((double)(fTempRadius * 2.0f)) - fTempRadius;
		fTempZ = (FLOAT)m_csRand.rand((double)(fTempRadius * 2.0f)) - fTempRadius;

		fX = (pcsCurrentPos->x + fTempX) - pcsSpawnBasePos->x;
		fZ = (pcsCurrentPos->z + fTempZ) - pcsSpawnBasePos->z;

		if(!IsRunawayStateCharacter(pcsCharacter))
		{
			//스폰지역 밖이면 그냥 지나간다.
			if (fX*fX + fZ*fZ > fRadius * fRadius )
			{
				continue;
			}
		}

		stTempAuPos.x = pcsCurrentPos->x + fTempX;
		stTempAuPos.z = pcsCurrentPos->z + fTempZ;

		//블럭이 없다면 굿! 앗싸아~
		if (m_pcsApmMap->CheckBlockingInfo(stTempAuPos , ApmMap::GROUND ) != ApTileInfo::BLOCKGEOMETRY)
		{
			pcsTargetPos->x = stTempAuPos.x;
			pcsTargetPos->z = stTempAuPos.z;

			bResult = TRUE;
			break;
		}
	}

	return bResult;
}

BOOL AgsmAI2::CBUpdateCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	PROFILE("AgsmAI2::CBUpdateCharacter");

	AgsmAI2 *			pThis = (AgsmAI2 *) pvClass;
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pvData;
	AgpdAI2ADChar *		pstAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);
	UINT32				ulClockCount = PtrToUint(pvCustData);

	if( AGPDCHAR_STATUS_DEAD == pcsCharacter->m_unActionStatus )
		return TRUE;

	if (!pcsCharacter->m_bIsAddMap)
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBUpdateCharacter"));

	// 2005.10.05. steeple. Fixed 추가
	// 2005.08.03. steeple. 수정
	if(pThis->m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter) || pThis->m_pcsAgpmCharacter->IsStatusTame(pcsCharacter) ||
		pThis->m_pcsAgpmCharacter->IsStatusFixed(pcsCharacter))
	{
		if (!pThis->ProcessSummonAI(pcsCharacter, ulClockCount))
		{
			//if (pThis->m_pcsAgpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by AI2 (ProcessSummonAI() is failed)\n");

			pThis->m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			return FALSE;
		}

		pThis->m_pcsAgsmSummons->CheckSummonsPeriod(pcsCharacter, ulClockCount);
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);

		return TRUE;
	}	

	if( NULL == pstAI2ADChar )
		return FALSE;

	// Interval만큼 안지났으면, return
	if (pstAI2ADChar->m_ulPrevProcessTime + pstAI2ADChar->m_ulProcessInterval > ulClockCount)
		return TRUE;

	eAgpmAI2StatePCAI eResult = AGPDAI2_STATE_PC_AI_NONE;

	// PC SKILL AI가 있는 놈들은 PC AI를 진행해준다.
	if(TRUE == pThis->ProcessPCAI(pcsCharacter, pstAI2ADChar, ulClockCount, &eResult))
		return TRUE;

	if( FALSE == pstAI2ADChar->m_bUseAI2 )
		return TRUE;

	if(NULL == pstAI2ADChar->m_pcsAgpdAI2Template)
		return TRUE;

	//뭔가 타켓이 있거나 NPC Guard의 경우는 빠릿빠릿~하게 움직인다.
	if( pstAI2ADChar->m_lTargetID != 0 || pstAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
	{

	}
	//타켓이 없으면 논다~
	else
	{
		if (!pThis->m_pcsAgsmCharacter->IsIdleProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, ulClockCount))
			return TRUE;
	}

	pThis->m_pcsAgsmCharacter->ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI);

	if (pThis->m_pcsAgpmCharacter->IsCombatMode(pcsCharacter) || pstAI2ADChar->m_lTargetID != 0)
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	else if (pstAI2ADChar->m_pcsAgpdAI2Template->m_lAgressivePoint != 0)
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_ONE_HALF_SECOND);
	else
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS);

	pThis->m_pcsAgsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, ulClockCount);

	pstAI2ADChar->m_ulPrevProcessTime = ulClockCount;

	if( (AGPMAI2_TYPE_PATROL_NPC == pstAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type) || 
		(AGPMAI2_TYPE_FIXED_NPC == pstAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type) )
	{
		//NPC에 관련된 AI를 수행한다.
		pThis->ProcessGuardNPCAI(pcsCharacter, pstAI2ADChar, ulClockCount);
	}
	else if( pThis->m_pcsAgpmCharacter->IsMonster(pcsCharacter) || pThis->m_pcsAgpmCharacter->IsCreature(pcsCharacter) ||
		pThis->m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsCharacter) )
	{
		//Mob에 관련된 AI를 수행한다.
		//pThis->ProcessAI(pcsCharacter, pstAI2ADChar, ulClockCount);
		pThis->ProcessMonsterAI(pcsCharacter, pstAI2ADChar, ulClockCount);
	}

	return TRUE;
}

BOOL AgsmAI2::CBPathFind(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pvData;
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;
	AgpdAI2ADChar		*pstAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);

	BOOL				*pbIgnoreBlocking	= (BOOL *)	pvCustData;

	if (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsCharacter->m_ulSpecialStatus)
		return TRUE;

	if (!*pbIgnoreBlocking && (
		pstAI2ADChar->m_lTargetID == AP_INVALID_CID ||
		pstAI2ADChar->m_bIsNeedPathFind))
		return TRUE;

	AgpdCharacter		*pcsTargetChar	= pThis->m_pcsAgpmCharacter->GetCharacter(pstAI2ADChar->m_lTargetID);
	if (!pcsTargetChar)
		return TRUE;

	if (pThis->ProcessCheckTargetArea(pcsCharacter, pcsTargetChar))
		return TRUE;

	pstAI2ADChar->m_bIsNeedPathFind	= TRUE;

	//공격전에 길을 찾아둔다.
	BOOL bResult = pThis->m_pcsAgpmPathFind->pathFind(  pcsCharacter->m_stPos.x,
											pcsCharacter->m_stPos.z,
											pcsTargetChar->m_stPos.x,
											pcsTargetChar->m_stPos.z,
											pstAI2ADChar->m_pclPath );

	if (!bResult || *pbIgnoreBlocking)
	{
		pcsCharacter->m_ulCheckBlocking		= 0;
		pstAI2ADChar->m_bIsNeedPathFind	= FALSE;
	}

	//
	CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

	pcsNode = pstAI2ADChar->m_pclPath->GetNextPath();

	if( pcsNode )
	{
		AuPOS			csPos;

		csPos.x = pcsNode->data->m_fX;
		csPos.y = 0.0f;
		csPos.z = pcsNode->data->m_fY;

		pThis->m_pcsAgpmCharacter->MoveCharacter( pcsCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
	}

	return TRUE;
}

void AgsmAI2::ProcessCommonAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	if( NULL == pcsAgpdCharacter )
		return;

	//STOPWATCH2(GetModuleName(), _T("ProcesssCommonAI"));

	AgpdCharacterTemplate	*pcsTemplate;
	INT32					lMaxHP;
	INT32					lDecreaseScalar;

	pcsTemplate = pcsAgpdCharacter->m_pcsCharacterTemplate;

	if( pcsAgpdAI2ADChar->m_ulNextDecSlowTime < lClockCount )
	{
		//1/1000초 단위이므로 1000을 곱한다. 그냥 1/1000단위로 입력해 달라고 하는게 낫겠군.
		pcsAgpdAI2ADChar->m_ulNextDecSlowTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lSlowTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lSlowPercent)/100;

		if( lDecreaseScalar == 0 )
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory( pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseScalar  );
	}

	if( pcsAgpdAI2ADChar->m_ulNextDecFastTime < lClockCount )
	{
		//1/1000초 단위이므로 1000을 곱한다. 그냥 1/1000단위로 입력해 달라고 하는게 낫겠군.
		pcsAgpdAI2ADChar->m_ulNextDecFastTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lFastTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lFastPercent)/100;

		if( lDecreaseScalar == 0 )
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory( pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_FAST, lDecreaseScalar  );
	}

	//보스몹의 경우 주변에 몹을 자신의 부하로 만들수 있다.
	if( m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) <= 0 )
		return;

	if( m_pcsAgpmAI2->GetCumulativeFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) <= pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower )
		return;

	AgpdAI2ADChar		*pcsAgpdAI2TempADChar;
	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
	alCID.MemSetAll();
	alCID2.MemSetAll();

	INT32				lNumCID;
	INT32				lCounter;
	float				fVisibility = 2000.0f;
	float				fTempX, fTempZ;

	lNumCID = m_pcsApmMap->GetCharList( pcsAgpdCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, fVisibility,
										&alCID[0], AGSMAI2_MAX_PROCESS_TARGET,
										&alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

	AgpdCharacter	*pcsFollowerCharacter	= NULL;

	//부하들중에 어느정도 이상 멀리 떨어지면 리스트에서 삭제한다.
	for( lCounter=0; lCounter<AGPDAI2_MAX_FOLLOWER_COUNT; ++lCounter )
	{
		if( NULL == pcsAgpdAI2ADChar->m_pcsFollower[lCounter] )
			continue;

		// 2007.03.05. steeple
		if(pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_bIsReadyRemove)
			continue;

		fTempX = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
		fTempZ = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

		//시야 밖으로 자신의 부하가 사라졌다면 뺀다~
		if( fTempX*fTempX + fTempZ*fTempZ > fVisibility*fVisibility )
		{
			pcsFollowerCharacter	= pcsAgpdAI2ADChar->m_pcsFollower[lCounter];

			pcsAgpdAI2TempADChar = m_pcsAgpmAI2->GetCharacterData( pcsFollowerCharacter );

			if( pcsAgpdAI2TempADChar )
			{
				if (pcsFollowerCharacter->m_Mutex.WLock())
				{
					//부하의 보스를 없애주고...
					pcsAgpdAI2TempADChar->m_pcsBossCharacter = NULL;

					//보스의 부하리스트에서 삭제한다.
					pcsAgpdAI2ADChar->m_pcsFollower[lCounter] = NULL;
					pcsAgpdAI2ADChar->m_lCurrentFollowers--;

					pcsFollowerCharacter->m_Mutex.Release();
				}
			}
		}	
	}

	for (int i = 0; i < lNumCID; ++i)
	{
		AgpdCharacter	*pcsListCharacter	= m_pcsAgpmCharacter->GetCharacter(alCID2[i]);
		AgpdAI2ADChar	*pcsAI2ADFollowerChar; 
		INT32			lFollowerTID;

		if (!pcsListCharacter)
			continue;

		if (pcsListCharacter->m_bIsReadyRemove)
			continue;

		//자기 자신은 뺀다.
		if (pcsListCharacter->m_lID == pcsAgpdCharacter->m_lID )
			continue;

		for( INT32 lFollowerCount=0; lFollowerCount<AGPDAI2_MAX_FOLLOWER_COUNT; ++lFollowerCount )
		{
			lFollowerTID = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFollower.m_lFollowerInfo[lFollowerCount].m_lFollowerTID;

			if( lFollowerTID )
			{
				if( pcsListCharacter->m_pcsCharacterTemplate->m_lID == lFollowerTID )
				{
					if( pcsAgpdAI2ADChar->m_lCurrentFollowers >= m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) )
					{
						break;
					}

					pcsAI2ADFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);

					//보스가 없는 몹인 경우만 자신의 부하로 만들수 있다.
					if( pcsAI2ADFollowerChar && pcsAI2ADFollowerChar->m_pcsBossCharacter == NULL )
					{
						BOOL				bExist;

						bExist = FALSE;

						for( int j=0; j<AGPDAI2_MAX_FOLLOWER_COUNT; ++j )
						{
							if( pcsAgpdAI2ADChar->m_pcsFollower[j] == pcsListCharacter )
							{
								bExist = TRUE;
								break;
							}
						}

						if( bExist == FALSE )
						{
							if (!pcsListCharacter->m_Mutex.WLock())
								break;

							pcsAI2ADFollowerChar->m_pcsBossCharacter = pcsAgpdCharacter;

							pcsListCharacter->m_Mutex.Release();

							for(int k=0; k<AGPDAI2_MAX_FOLLOWER_COUNT; ++k )
							{
								if( pcsAgpdAI2ADChar->m_pcsFollower[k] == NULL )
								{
									pcsAgpdAI2ADChar->m_pcsFollower[k] = pcsListCharacter;
									pcsAgpdAI2ADChar->m_lCurrentFollowers++;
									pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower++;
									break;
								}
							}
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}

AgpdCharacter* AgsmAI2::GetTargetPC( AgpdCharacter *pcsAgpdMobCharacter, AgpdAI2ADChar *pcsAI2ADChar, AuPOS csMobPos, float fPreemptiveRange )
{
	//만약 크리쳐면 바로 리턴한다.
	if( m_pcsAgpmCharacter->IsCreature(pcsAgpdMobCharacter) )
	{
		return NULL;
	}

	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	NearCharList;
	NearCharList.MemSetAll();
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		NearCharIDList;
	NearCharIDList.MemSetAll();

	INT32	lNumCID	= 0;

	// 타겟이 따로 정해진 룰에 의해 구해지는 놈들은 따로 구해본다. 여기서 구해지면 넘어온 놈들을 타겟으로 처리하자.
	PVOID	pvBuffer[3];
	pvBuffer[0]	= (PVOID) pcsAgpdMobCharacter;
	pvBuffer[1]	= (PVOID) &NearCharList[0];
	pvBuffer[2]	= (PVOID) &lNumCID;

	float	fVisibility = AGSMAI2_Visibility;

	if(pcsAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility > 0.0f)
	{
		fVisibility = pcsAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility;
	}

	EnumCallback(AGSMAI2_CB_GET_TARGET, pvBuffer, NULL);

	if (lNumCID <= 0)
	{
		//주변에 PC가 있나본다.
		lNumCID = m_pcsApmMap->GetCharList(pcsAgpdMobCharacter->m_nDimension, APMMAP_CHAR_TYPE_PC | APMMAP_CHAR_TYPE_MONSTER, csMobPos,
												fVisibility, &NearCharList[0], AGSMAI2_MAX_PROCESS_TARGET,
												&NearCharIDList[0], AGSMAI2_MAX_PROCESS_TARGET);
	}
	else
		return (AgpdCharacter *) NearCharList[0];

	//나(몬스터) 말고 누군가 있는경우.
	if( lNumCID <= 0 )
		return NULL;

	INT32 lMaxDamage = 0;
	AgpdAI2ADChar	*pcsTempAI2ADChar = NULL;
	AgpdCharacter	*pcsTargetCharacter	= NULL;
	AgpdCharacter	*pcsAgroTargetCharacter = NULL;

	float			fAgroDistance = 0.0f;
	INT32			lMostAgro = 0;

	for (int i = 0; i < lNumCID; ++i)
	{
		// 2007.02.27. steeple
		// CID 배열로 부터 다시 Character 를 얻어오게끔 수정.
		AgpdCharacter* pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(NearCharIDList[i]);
		if (!pcsListCharacter)
			continue;

		// 2007.03.05. steeple
		if (pcsListCharacter->m_bIsReadyRemove ||
			m_pcsAgpmCharacter->IsCreature(pcsListCharacter) ||
			pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
			pcsListCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO ||
			m_pcsAgpmCharacter->IsStatusFullTransparent(pcsListCharacter) ||
			m_pcsAgpmCharacter->IsStatusInvincible(pcsListCharacter))
			continue;

		if (TRUE == m_pcsAgpmCharacter->IsMonster(pcsListCharacter))
		{
			if (FALSE == m_pcsAgpmCharacter->IsStatusTame(pcsListCharacter)
				&& FALSE == m_pcsAgpmCharacter->IsStatusSummoner(pcsListCharacter)) // 소환수랑 몹끼리만 전투를 하는 바람에 뺐었는데, 기획팀에서 다시 넣어 달라고 해서 넣음. -arycoat 2010.10.13
				continue;
		}

		pcsTempAI2ADChar = m_pcsAgpmAI2->GetCharacterData( pcsListCharacter );

		if (!pcsListCharacter->m_Mutex.WLock())
			continue;
			
		if( m_pagpmBattleGround->IsInBattleGround(pcsAgpdMobCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsListCharacter) )
		{
			if(m_pcsAgpmCharacter->IsSameRace(pcsAgpdMobCharacter, pcsListCharacter))
				continue;
		}

		// 어그로가 가장 높은 유저(어그로 총 합이 제일 작은) 찾기
		INT32 lAgro = m_pcsAgsmCharacter->GetAgroFromEntry( pcsAgpdMobCharacter, pcsListCharacter->m_lID );
		if( lAgro < lMostAgro )
		{
			pcsTargetCharacter = pcsListCharacter;
			lMostAgro = lAgro;
		}

		//나를 때린적이 없는 PC인경우.
		if( lAgro == 0 )
		{
			//선공 몹이면 나를 때린적이 없어도 타켓으로한다.
			if( fPreemptiveRange != 0.0f && pcsAI2ADChar->m_pcsAgpdAI2Template->m_lPreemptiveType > 0)
			{
				//fPreemptiveRange안에 존재하는가?
				float			fTempX, fTempZ;

				fTempX = pcsListCharacter->m_stPos.x - pcsAgpdMobCharacter->m_stPos.x;
				fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdMobCharacter->m_stPos.z;

				if( fTempX*fTempX + fTempZ*fTempZ < fPreemptiveRange*fPreemptiveRange )
				{
					//PC에 선공 방지 플래그가 있으면 공격하지 않는다.
					if( pcsListCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO )
					{
						//타켓으로 정하지 않는다.
					}
					else
					{
						//어그로 조건(LV, HP, MP, SP상태등...)에 따라 어그로를 발동시킨다.
						if( TRUE == CheckPreemptive( pcsAgpdMobCharacter, pcsListCharacter, pcsAI2ADChar ) )
						{
							float			fTempX, fTempZ;
							INT32			lMaxHP;
							INT32			lDecreaseAggroPointPerCycle;

							fTempX = pcsListCharacter->m_stPos.x - pcsAgpdMobCharacter->m_stPos.x;
							fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdMobCharacter->m_stPos.z;

							//아직 아무도 타켓으로 정해지지 않은 경우.
							if( pcsAgroTargetCharacter == NULL )
							{
								pcsAgroTargetCharacter = pcsListCharacter;
								fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
							}
							//누군가 이미 타켓으로 정해진 경우.
							else
							{
								if( fAgroDistance > fTempX*fTempX + fTempZ*fTempZ )
								{
									pcsAgroTargetCharacter = pcsListCharacter;
									fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
								}
							}

							//여기까지왔으면 우선 어그로를 주고 보자~
							lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdMobCharacter->m_csFactor);

							lDecreaseAggroPointPerCycle = (lMaxHP*pcsAgpdMobCharacter->m_pcsCharacterTemplate->m_csDecreaseData.m_lSlowPercent)/100;

							//5회 정도 Slow 감소 어그로 없어질만큼의 Slow 어그로를 부여한다.
							if( lDecreaseAggroPointPerCycle == 0 )
							{
								lDecreaseAggroPointPerCycle = 1;
							}

							lDecreaseAggroPointPerCycle *= 5;

							m_pcsAgsmCharacter->AddAgroPoint( pcsAgpdMobCharacter, pcsListCharacter, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseAggroPointPerCycle );
						}
						// 스크림 발동 조건을 확인한다.
						else if ( TRUE == CheckPreemptive( pcsAgpdMobCharacter, pcsListCharacter, pcsAI2ADChar ) )
						{
							//스크림사용
							if ( pcsListCharacter && CheckUseScream( pcsAgpdMobCharacter, pcsAI2ADChar, GetClockCount() ) )
							{
								pcsAI2ADChar->m_bScreamUsed = ProcessUseScream( pcsAgpdMobCharacter, pcsAI2ADChar, pcsListCharacter );
								if (TRUE == pcsAI2ADChar->m_bScreamUsed)
								{
									pcsAI2ADChar->m_ulScreamStartTime = GetClockCount();
								}
							}
						}
					}
				}
			}
		}
		
		if(pcsListCharacter)
			pcsListCharacter->m_Mutex.Release();
	}

	if( (pcsTargetCharacter == NULL) && (pcsAgroTargetCharacter != NULL) )
	{
		pcsTargetCharacter = pcsAgroTargetCharacter;
	}

	if (!pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_vtTID.empty())
	{
		//STOPWATCH2(GetModuleName(), _T("GetTargetPC;; m_csSummon.m_vtTID"));

		if (pcsAI2ADChar->m_lTargetID == AP_INVALID_CID && pcsTargetCharacter)
		{
			// 만약 Summon하는 몬스터라면 Summon하자. 일단 Duration을 9999999 로 하자 나중에 로직 바꿔서 무한대 세팅 해야지
			m_pcsAgpmSummons->SetMaxSummonsCount(pcsAgpdMobCharacter, pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_lMaxCount);

			for (ApVector<INT32, 5>::iterator iter = pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_vtTID.begin();
				pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_vtTID.end() != iter;
				++iter)
			{
				m_pcsAgsmSummons->ProcessSummons(pcsAgpdMobCharacter, *iter, 99999999, pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_lMaxCount, 1);
			}
		}

		if (pcsTargetCharacter)
		{
			ProcessSetTargetToSummons(pcsAgpdMobCharacter, pcsTargetCharacter->m_lID);
		}

		// 만약 Summon하는 몬스터인데 Target이 없으면, Summon한 몬스터 다 없애자.
		if (pcsTargetCharacter == NULL)
		{
			m_pcsAgsmSummons->RemoveAllSummons(pcsAgpdMobCharacter,
						AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);

		}
	}

	//가장 적대치가 높은PC의 ID를 리턴한다!
	return pcsTargetCharacter;
}

void AgsmAI2::SetFollowersTarget( AgpdCharacter *pcsAgpdCharacter, AuPOS csMobPos, AgpdAI2ADChar *pcsAgpdAI2ADChar )
{
	//STOPWATCH2(GetModuleName(), _T("SetFollowersTarget"));

	AgpdCharacter			*pcsFollower;
	AgpdAI2ADChar			*pcsFollowerAI2ADChar;

	ApSafeArray<INT32, AGPDAI2_MAX_FOLLOWER_COUNT>		alBossTargetList;
	ApSafeArray<INT32, AGPDAI2_MAX_FOLLOWER_COUNT>		alTempBossTargetList;

	if(m_pcsAgpmAI2->CheckIsBossMob(pcsAgpdAI2ADChar) == FALSE)
		return;

	//타켓을 정해준다.
	INT32				lTargets;
	lTargets = m_pcsAgsmCharacter->GetMobListFromHistoryEntry( pcsAgpdCharacter, &alBossTargetList[0], AGPDAI2_MAX_FOLLOWER_COUNT );

	//보스몹인 내가 누군가와 싸우고 있을때만, 부하들에게 타켓을 정해주는게 가능하다.
	if( lTargets > 0 )
	{
		for( int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i )
		{
			pcsFollower = pcsAgpdAI2ADChar->m_pcsFollower[i];

			if( pcsFollower != NULL && pcsFollower->m_Mutex.WLock())
			{
				pcsFollowerAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsFollower);

//-----------------------------------------------------------------------------------
				INT32				lTargetShareFollowers;

				lTargetShareFollowers = m_pcsAgpmAI2->GetUpkeepFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template);

				//앞 n개의 슬롯은 보스와 같은 타켓을 유지한다. 난이도 조절용으로 사용가능함.
				if( i < lTargetShareFollowers )
				{
					pcsFollowerAI2ADChar->m_lTargetID = pcsAgpdAI2ADChar->m_lTargetID;
				}
				else
				{
					//Slow, Fast적대치에 대해 적당한 값을 입력해주고 어느정도 타켓고정 효과를 준다.

					//지금 누군가와 싸우고 있는경우~ 그냥 타켓 유지~ 특별한 간섭은 하지 않는다.
					if( pcsFollowerAI2ADChar->m_lTargetID )
					{
					}
					//타켓이 없으니 보스의 지시를 받아 타켓을 정하자~
					else
					{
						BOOL			bExist;

						bExist = FALSE;

						//어그로가 없는경우.
						if( m_pcsAgsmCharacter->IsNoAgro( pcsFollower ) )
						{
							AgpdCharacter			*pcsAgpdCharacter;
							AgpdAI2ADChar			*pcsTempAI2ADChar;
							INT32					lTargetPC;

							lTargetPC = 0;

							//미리 복사해둔다.
							alTempBossTargetList.MemCopy(0, &alBossTargetList[0], AGPDAI2_MAX_FOLLOWER_COUNT);

							for( int lFollowersCount=0; lFollowersCount<pcsAgpdAI2ADChar->m_lCurrentFollowers; lFollowersCount++ )
							{
								pcsAgpdCharacter = pcsAgpdAI2ADChar->m_pcsFollower[lFollowersCount];

								if( pcsAgpdCharacter )
								{
									pcsTempAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsFollower);

									if( pcsTempAI2ADChar )
									{
										if( pcsTempAI2ADChar->m_lTargetID )
										{
											for( int j=0; j<AGPDAI2_MAX_FOLLOWER_COUNT; ++j )
											{
												if( alTempBossTargetList[j] == pcsTempAI2ADChar->m_lTargetID )
												{
													alTempBossTargetList[j] = 0;
												}
											}
										}
									}										
								}
							}

							for( int k=0; k<AGPDAI2_MAX_FOLLOWER_COUNT; ++k )
							{
								if( alTempBossTargetList[k] )
								{
									lTargetPC = alTempBossTargetList[k];
									break;
								}
							}

							if( lTargetPC )
							{
								pcsFollowerAI2ADChar->m_lTargetID = lTargetPC;
							}
							//타켓이 없는 경우는 보스와 같은 타켓을 공유한다.
							else
							{
								pcsFollowerAI2ADChar->m_lTargetID = pcsAgpdAI2ADChar->m_lTargetID;
							}
						}
						//어그로가 있는경우.
						else
						{
							//어그로가 가장 높은놈을 타켓으로 정한다.
						}
					}
				}
				
				if(pcsFollower)
					pcsFollower->m_Mutex.Release();
			}
		}
	}
}

// 2005.08.30. steeple
// Summons 이 공격해야할 Target 을 얻는다.
// return 되는 AgpdCharacter 는 Lock 하지 않은 상태이다.
AgpdCharacter* AgsmAI2::GetAttackTargetPC(AgpdCharacter* pcsOwner, AgpdCharacter* pcsSummons, EnumAgpdSummonsPropensity ePropensity)
{
	if(!pcsOwner || !pcsSummons)
		return NULL;

	if (AGPMSUMMONS_PROPENSITY_SHADOW == ePropensity)
		return NULL;

	AgpdAI2ADChar* pcsAI2ADSummons = m_pcsAgpmAI2->GetCharacterData(pcsSummons);
	if(!pcsAI2ADSummons)
		return NULL;

	//STOPWATCH2(GetModuleName(), _T("GetAttackTargetPC"));

	// 2005.09.08. steeple
	// TargetID 가 Owner 이면 0 으로 바꾸자.
	AgpdSummonsADChar* pcsSummonsADChar = m_pcsAgpmSummons->GetADCharacter(pcsSummons);
	if(pcsSummonsADChar && pcsSummonsADChar->m_lOwnerCID == pcsAI2ADSummons->m_lTargetID)
	{
		pcsAI2ADSummons->m_lTargetID = 0;
	}

	if(pcsAI2ADSummons->m_lTargetID == pcsSummons->m_lID)
		pcsAI2ADSummons->m_lTargetID = 0;

	if(pcsAI2ADSummons->m_lTargetID == 0 && pcsAI2ADSummons->m_lReadyTargetID != 0)
	{
		pcsAI2ADSummons->m_lTargetID = pcsAI2ADSummons->m_lReadyTargetID;
		pcsAI2ADSummons->m_lReadyTargetID = 0;
	}

	if(pcsAI2ADSummons->m_lTargetID != 0 && (pcsAI2ADSummons->m_lTargetID == pcsAI2ADSummons->m_lReadyTargetID))
		pcsAI2ADSummons->m_lReadyTargetID = 0;

	// 타겟이 있던 없던 간에 주인이랑 너무 멀어지면 주인을 따라가게 한다. NULL 리턴해주면 된다.
	// 자폭 캐릭 제외.
	AuPOS stDest;
	INT32 lTargetDistance = 0;	// IsInRange가 FALSE일 때만 값이 설정된다.
	INT32 lAOIRange = m_lSummonAOIRange;
	ZeroMemory(&stDest, sizeof(stDest));
	if(FALSE == pcsSummons->m_pcsCharacterTemplate->m_bSelfDestructionAttackType &&
		FALSE == m_pcsAgpmFactors->IsInRange(&pcsOwner->m_stPos, &pcsSummons->m_stPos, lAOIRange, 0, &stDest, &lTargetDistance))
	{
		if(lTargetDistance > AGSMAI2_Summons_Max_Follow)
			return NULL;
	}

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacterLock(pcsAI2ADSummons->m_lTargetID);
	if(pcsTarget)
	{
		// 제대로 된 상태라면 바로 리턴
		if(pcsTarget->m_unActionStatus != AGPDCHAR_STATUS_DEAD &&
			m_pcsAgpmPvP->IsAttackable(pcsSummons, pcsTarget, FALSE))
		{
			pcsTarget->m_Mutex.Release();
			return pcsTarget;
		}

		pcsTarget->m_Mutex.Release();
	}

	// 여기까지 왔다면 제대로된 타겟이 아니란 소리다. 그렇다면 다음 타겟을 타겟 아이디로 세팅하고 얻어본다.
	pcsAI2ADSummons->m_lTargetID = pcsAI2ADSummons->m_lReadyTargetID;
	pcsAI2ADSummons->m_lReadyTargetID = 0;

	pcsTarget = m_pcsAgpmCharacter->GetCharacterLock(pcsAI2ADSummons->m_lTargetID);
	if(pcsTarget)
	{
		// 제대로 된 상태라면 바로 리턴
		if(pcsTarget->m_unActionStatus != AGPDCHAR_STATUS_DEAD &&
			m_pcsAgpmPvP->IsAttackable(pcsSummons, pcsTarget, FALSE))
		{
			pcsTarget->m_Mutex.Release();
			return pcsTarget;
		}

		pcsTarget->m_Mutex.Release();
	}

	// 여기까지도 리턴이 안됬다면 공격할 타겟이 없는 것임.
	AgsdCharacterHistoryEntry *pcsCharHistory = m_pcsAgsmCharacter->GetMostAgroChar(pcsSummons);
	if (pcsCharHistory)
	{
		if (APBASE_TYPE_CHARACTER == pcsCharHistory->m_csSource.m_eType)
		{
			AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacter(pcsCharHistory->m_csSource.m_lID);
			if (pcsCharacter)
			{
				if (pcsCharacter->m_lID != m_pcsAgpmSummons->GetOwnerCID(pcsSummons) &&
					pcsCharacter->m_lID != pcsSummons->m_lID &&
					m_pcsAgpmCharacter->IsStatusFixed(pcsCharacter) == FALSE)
				{
					pcsAI2ADSummons->m_lTargetID = pcsCharacter->m_lID;
					return pcsCharacter;
				}
			}
		}
	}

	// 소환수가 공격형인데 타겟이 없다면 주위의 몬스터중 아무놈이나 타겟으로 정해버린다.
	if (AGPMSUMMONS_PROPENSITY_ATTACK == ePropensity)
	{
		// 2005.10.13. steeple
		// 기존의 2마리 가져오는 것에서 33마리로 변경.
		ApSafeArray<AgpdCharacter*, AGSMAI2_MAX_PROCESS_TARGET> TargetCharacter;
		TargetCharacter.MemSetAll();
		ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET> TargetCID;
		TargetCID.MemSetAll();

		// 수성/공성 소환수일 경우 상대방 캐릭터를 공격 할 수 있어야 하기 때문에 경우에 따라서 
		// 검색조건에 APMMAP_CHAR_TYPE_PC 타입을 추가한다.
		INT32 CharType = APMMAP_CHAR_TYPE_MONSTER;

		if (AGPD_SIEGE_MONSTER_SUMMON == m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsSummons))
		{
			CharType |= APMMAP_CHAR_TYPE_PC;
		}

		// 2005.10.28. steeple
		// 소환수 시야는 15미터로 변경. (기존에는 25 미터)
		// 주변에 MOB가 있나본다.
		INT32 lNumCID = m_pcsApmMap->GetCharList(pcsSummons->m_nDimension, CharType, pcsSummons->m_stPos,
												AGSMAI2_Summons_Visibility, (INT_PTR*)&TargetCharacter[0], AGSMAI2_MAX_PROCESS_TARGET,
												&TargetCID[0], AGSMAI2_MAX_PROCESS_TARGET);
		ASSERT(lNumCID <= AGSMAI2_MAX_PROCESS_TARGET);

		// 공격할 수 있는 상대인지 체크한다.
		ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>::iterator iter = TargetCID.begin();
		if(TargetCID.IsValidIterator(iter) == TRUE)
		{
			while(iter != TargetCID.end())
			{
				if(!*iter)
				{
					++iter;
					continue;
				}

				AgpdCharacter* pcsTargetCharacter = m_pcsAgpmCharacter->GetCharacter(*iter);
				if(!pcsTargetCharacter)
				{
					++iter;
					continue;
				}

				// 2006.11.07. steeple
				// 락 하기 전에 유효한 건지 체크한다.
				if(pcsTargetCharacter->m_bIsReadyRemove ||
					!pcsTargetCharacter->m_bIsActiveData ||
					!pcsTargetCharacter->m_bIsAddMap ||
					pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
					pcsTargetCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO ||
					m_pcsAgpmCharacter->IsStatusFullTransparent(pcsTargetCharacter) ||
					m_pcsAgpmCharacter->IsStatusInvincible(pcsTargetCharacter))
				{
					++iter;
					continue;
				}

				AuAutoLock csLock(pcsTargetCharacter->m_Mutex);
				if(!csLock.Result())
				{
					++iter;
					continue;
				}

				if(pcsSummons->m_lID == pcsTargetCharacter->m_lID)
				{
					++iter;
					continue;
				}

				if(m_pcsAgpmPvP->IsAttackable(pcsSummons, pcsTargetCharacter, FALSE))
					return pcsTargetCharacter;

				++iter;
			}
		}
	}

	pcsAI2ADSummons->m_lTargetID = 0;

	return NULL;
}

// 2005.09.06. steeple
// 소환수에게 타겟 주는 함수는 분리했다.
BOOL AgsmAI2::ProcessSetTargetToSummons(AgpdCharacter* pcsOwner, INT32 lTargetCID)
{
	if(!pcsOwner || lTargetCID == 0)
		return FALSE;

	//if(!m_pcsAgpmCharacter->IsPC(pcsOwner))
	//	return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pcsAgpmSummons->GetADCharacter(pcsOwner);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessSetTargetToSummons"));

	// 자기 Summons 들을 돌면서 타겟을 지정해준다.

	AgpdSummonsArray::SummonsVector tmpVector(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
	AgpdSummonsArray::iterator iter = tmpVector.begin();
	while(iter != tmpVector.end())
	{
		AgpdCharacter* pcsSummons = m_pcsAgpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsSummons)
		{
			++iter;
			continue;
		}

		// Summons 에게 공격해야할 대상을 세팅해준다.
		AgpdAI2ADChar* pcsAI2ADSummons = m_pcsAgpmAI2->GetCharacterData(pcsSummons);
		if(pcsAI2ADSummons)
		{
			if(pcsAI2ADSummons->m_lTargetID == 0)
				pcsAI2ADSummons->m_lTargetID = lTargetCID;
			else if(pcsAI2ADSummons->m_lTargetID == lTargetCID)
			{}	// 아무것도 하지 않는다.
			else
				pcsAI2ADSummons->m_lReadyTargetID = lTargetCID;
		}

		pcsSummons->m_Mutex.Release();
		++iter;
	}

	// 자기 Summons 들을 돌면서 타겟을 지정해준다.
	for(INT32 i = 0; i < AGPMSUMMONS_MAX_TAME; i++)
	{
		if(pcsSummonsADChar->m_TameArray.m_arrTame[i].m_lCID == 0)
			break;

		AgpdCharacter* pcsSummons = m_pcsAgpmCharacter->GetCharacterLock(pcsSummonsADChar->m_TameArray.m_arrTame[i].m_lCID);
		if(!pcsSummons)
			continue;

		// Summons 에게 공격해야할 대상을 세팅해준다.
		AgpdAI2ADChar* pcsAI2ADSummons = m_pcsAgpmAI2->GetCharacterData(pcsSummons);
		if(pcsAI2ADSummons)
		{
			if(pcsAI2ADSummons->m_lTargetID == 0)
				pcsAI2ADSummons->m_lTargetID = lTargetCID;
			else if(pcsAI2ADSummons->m_lTargetID == lTargetCID)
			{}	// 아무것도 하지 않는다.
			else
				pcsAI2ADSummons->m_lReadyTargetID = lTargetCID;
		}

		pcsSummons->m_Mutex.Release();
	}
	
	return TRUE;
}

BOOL AgsmAI2::ProcessExAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AgpdCharacter *pcsTargetPC, UINT32 lClockCount )
{
	PROFILE("AgsmAI2::ProcessExAI");

	//STOPWATCH2(GetModuleName(), _T("ProcessExAI"));

	BOOL			bUseExAI;
	INT32			lIndex;

	lIndex = -1;
	bUseExAI = FALSE;

	if( pcsTargetPC )
	{
		lIndex = CheckUseSkill( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );
	}
	else if(m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter) /*&& m_pcsAgpmSiegeWar->IsDoingSiegeWar(pcsAgpdCharacter)*/)
	{
		// 2006.08.29. steeple
		// 공성 관련 몬스터는 일단 여기서는 타겟이 없다.
		lIndex = CheckUseSkill( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );
	}

	//스킬사용?
	if( lIndex != -1 )
	{
		if( pcsTargetPC != NULL ||
			(m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter) /*&& m_pcsAgpmSiegeWar->IsDoingSiegeWar(pcsAgpdCharacter)*/))
		{
			bUseExAI = ProcessUseSkill( pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsTargetPC );
		}
	}
	else
	{
		lIndex = CheckUseItem( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );
		//아이템사용?
		if( lIndex != -1 )
		{
			bUseExAI = ProcessUseItem( pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsTargetPC );
		}
		else
		{
			//스크림사용?
			if ( pcsTargetPC && CheckUseScream( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount ) )
			{
				pcsAgpdAI2ADChar->m_bScreamUsed = ProcessUseScream( pcsAgpdCharacter, pcsAgpdAI2ADChar, pcsTargetPC );
				if (TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
				{
					pcsAgpdAI2ADChar->m_ulScreamStartTime = GetClockCount();
				}

				bUseExAI = pcsAgpdAI2ADChar->m_bScreamUsed;
			}
		}
	}

	return bUseExAI;
}
/*
BOOL AgsmAI2::ProcessAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	PROFILE("AgsmAI2::ProcessAI");

	//STOPWATCH2(GetModuleName(), _T("ProcessAI"));

	AgpdCharacter	*pcsAgpdMobCharacter;
	AgpdCharacter	*pcsAgpdPCCharacter;

	BOOL			bResult;
	BOOL			bUseExAI;
	BOOL			bUseAreaAI;

	AuPOS			csMobPos;
	INT32			lMobID;
	float			fPreemptiveRange;

	pcsAgpdMobCharacter = NULL;
	pcsAgpdPCCharacter	= NULL;
	bResult				= FALSE;

	if( !pcsAgpdAI2ADChar || !pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
		return FALSE;

	pcsAgpdMobCharacter	= pcsAgpdCharacter;

	// 스턴 상태인경우는 암짓도 하면 안된다. 흐흐...
	// 2004.08.25. steeple
	//if (pcsAgpdMobCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsAgpdMobCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return TRUE;

	csMobPos = pcsAgpdCharacter->m_stPos;
	lMobID = pcsAgpdCharacter->m_lID;
	fPreemptiveRange = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fPreemptiveRange;

	// 스크림을 사용하는 몬스터라면 일정 시간후에 다시 사용할수 있도록 초기화를 해준다.
	if (TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
	{
		if (pcsAgpdAI2ADChar->m_ulScreamStartTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulFellowWorkerRefreshTime < GetClockCount())
			pcsAgpdAI2ADChar->m_bScreamUsed = FALSE;
	}

	ProcessCommonAI(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );

	// 보스가 없다면 독자적으로 타켓을 찾는다. 보스몹자신, 보스몹이 없는 몬스터는 여기로 들어올것이다.
	if( pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL )
	{
		pcsAgpdPCCharacter = GetTargetPC( pcsAgpdCharacter, pcsAgpdAI2ADChar, csMobPos, fPreemptiveRange );

		//보스몹이다!! 부하들에게 적을 지정해주자~
		if( m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) > 0 && pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL)
		{
			SetFollowersTarget( pcsAgpdCharacter, csMobPos, pcsAgpdAI2ADChar );
		}
	}
	//나를 부하로 데리고 있는 보스가 있다면 그분의 명령에 따라 타켓을 정하자~
	else
	{
		//보스가 있다면 보스가 점지해주신 Target을 타켓으로 정하자~
		if( pcsAgpdAI2ADChar->m_lTargetID != 0 )
		{
			pcsAgpdPCCharacter = m_pcsAgpmCharacter->GetCharacter( pcsAgpdAI2ADChar->m_lTargetID );
		}
		else
		{
			pcsAgpdPCCharacter = NULL;
		}
	}

	if( pcsAgpdPCCharacter )
	{
		if (pcsAgpdAI2ADChar->m_lTargetID != pcsAgpdPCCharacter->m_lID)
			pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

		pcsAgpdAI2ADChar->m_lTargetID = pcsAgpdPCCharacter->m_lID;

		//2004.10.25일 몬스터 대사 통합되면서 주석처리됨.
/*		//타켓이 바뀐경우 읊조린다.
		if( pcsAgpdAI2ADChar->m_lTargetID != pcsAgpdPCCharacter->m_lID )
		{
			pcsAgpdAI2ADChar->m_lTargetID = pcsAgpdPCCharacter->m_lID;
			ProcessNPCDialog( (ApBase *)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_TARGET_PC );
		}
	}
	else
	{
		pcsAgpdAI2ADChar->m_lTargetID = 0;
		pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;
	}

	if (pcsAgpdPCCharacter && !pcsAgpdPCCharacter->m_Mutex.WLock())
		return FALSE;

	// 타겟과의 거리를 계산한다. 너무 멀리 있는 놈이면 타겟에서 삭제한다.
	if (ProcessCheckTargetArea(pcsAgpdCharacter, pcsAgpdPCCharacter))
	{
		pcsAgpdPCCharacter->m_Mutex.Release();

		pcsAgpdPCCharacter	= NULL;
	}

	bUseAreaAI = ProcessCheckArea(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );

	if (bUseAreaAI)
	{
		// AreaAI를 사용했으니 당연히~ 한턴 쉰다.
		// 아무일 없음.
	}
	else
	{
		bUseExAI = ProcessExAI( pcsAgpdCharacter, pcsAgpdAI2ADChar, pcsAgpdPCCharacter, lClockCount );

		if( bUseExAI )
		{
			//ExAI를 사용했으니 당연히~ 한턴 쉰다.
			//아무일 없음.
		}
		else
		{
			//타켓이 있을때.
			//
			// 몬스터가 변신한 상태라면 공격못하게 한다. 2005.10.07. steeple
			// 즉 원래 몬스터가 아니라, PC 에 의해서 크리쳐 같은 걸로 변신한 상태임.
			//
			if( pcsAgpdPCCharacter && pcsAgpdMobCharacter->m_bIsTrasform == FALSE )
			{
				PROFILE("AgsmAI2::AttackTarget");

				BOOL				bInRange;
				BOOL				bPathFind;

				bInRange =  FALSE;
				bPathFind = FALSE;

				AuPOS				stDestPos;
				ZeroMemory(&stDestPos, sizeof(stDestPos));

				bInRange = m_pcsAgpmFactors->IsInRange(&pcsAgpdMobCharacter->m_stPos, &pcsAgpdPCCharacter->m_stPos, &pcsAgpdMobCharacter->m_csFactor, &pcsAgpdPCCharacter->m_csFactor, &stDestPos);

				//공격 범위 안인가?
				if( bInRange )
				{
					//Normal Attack이 가능하면 거리를 계산한다. 그외에는 계산할 필요가 없겠지...
					if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bUseNormalAttack )
					{
						//공격해도 되면 공격한다.
						if( lClockCount > pcsAgpdAI2ADChar->m_ulLastAttackTime + pcsAgpdAI2ADChar->m_lAttackInterval )
						{
							//공격할때 읊조릴 말 호출한다.
							ProcessNPCDialog( pcsAgpdMobCharacter, AGPD_NPCDIALOG_MOB_ATTACK );

							m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsAgpdMobCharacter, (ApBase *)pcsAgpdPCCharacter, NULL);

							pcsAgpdAI2ADChar->m_ulLastAttackTime = lClockCount;
						}
						else
						{
							//만약 이동중이면 멈춘다.
							if (pcsAgpdMobCharacter->m_bMove)
								m_pcsAgpmCharacter->StopCharacter(pcsAgpdMobCharacter, NULL);
						}
					}
					else
					{
						//만약 이동중이면 멈춘다.
						if (pcsAgpdMobCharacter->m_bMove)
							m_pcsAgpmCharacter->StopCharacter(pcsAgpdMobCharacter, NULL);
					}
				}
				//공격범위 밖인가?
				else if (!pcsAgpdAI2ADChar->m_bIsNeedPathFind)
				{
					if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
					{
						pcsAgpdAI2ADChar->m_csTargetPos	= stDestPos;
						m_pcsAgpmCharacter->MoveCharacter( pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE );
						bPathFind = TRUE;
					}
				}
				else
				{
					if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
					{

						pcsAgpdAI2ADChar->m_csTargetPos	= pcsAgpdPCCharacter->m_stPos;

						//공격전에 길을 찾아둔다.
						bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdCharacter->m_stPos.x,
																pcsAgpdCharacter->m_stPos.z,
																pcsAgpdAI2ADChar->m_csTargetPos.x,
																pcsAgpdAI2ADChar->m_csTargetPos.z,
																pcsAgpdAI2ADChar->m_pclPath );

						if (!bResult)
						{
							pcsAgpdCharacter->m_ulCheckBlocking	= 0;
							pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;
						}

						//
						CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

						pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

						if( pcsNode )
						{
							AuPOS			csPos;

							csPos.x = pcsNode->data->m_fX;
							csPos.y = 0.0f;
							csPos.z = pcsNode->data->m_fY;

							m_pcsAgpmCharacter->MoveCharacter( pcsAgpdCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
						}
					}
				}
			}
			//타켓이 없을때. AND // 고정형 몬스터이면 움직이지 않는다.
			else if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
			{
				//부하몹이면 보스몹이 정해준 위치로 움직인다.
				if( pcsAgpdAI2ADChar->m_pcsBossCharacter != NULL )
				{
					if( lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime )
					{
						//아무일없음.
					}
					else
					{
						float				fTempX, fTempZ;

						fTempX = pcsAgpdAI2ADChar->m_csTargetPos.x - pcsAgpdCharacter->m_stPos.x;
						fTempZ = pcsAgpdAI2ADChar->m_csTargetPos.z - pcsAgpdCharacter->m_stPos.z;

						//오차범위 1미터라면 상관없음.
						if( fTempX*fTempX + fTempZ*fTempZ <= 100.0f*100.0f )
						{
						}
						//그렇지 않다면? 길을 찾아 이동시키자~
						else
						{
							m_pcsAgpmCharacter->MoveCharacter( pcsAgpdMobCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE );
						}
					}
				}
				else
				{
					PROFILE("AgsmAI2::MoveWander");

					ApdEvent			*pcsEvent;

					float				fTempX, fTempZ, fTempDistance;

					pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsAgpdCharacter)->m_pstEvent;

					if( pcsEvent != NULL )
					{
						if( lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime )
						{
							//아무일없음.
						}
						else
						{
							//이동중이 아닌데 다음 패스 리스트가 없으면 길을 찾고 
							if( !pcsAgpdCharacter->m_bMove )
							{
								{
									PROFILE("AgsmAI2::GetNewTargetPosition");

									fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
									fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
									fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

									//만약 스폰범위안에 존재하지 않는다면 지금 위치를 새로운 스폰포인트로 잡는다.
									if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
									{
										pcsAgpdAI2ADChar->m_csSpawnBasePos = pcsAgpdCharacter->m_stPos;
									}

									if( GetTargetPosFromSpawn(&pcsAgpdCharacter->m_stPos, &pcsAgpdAI2ADChar->m_csTargetPos, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius ) )
									{
										bool			bResult;
										INT32			lRandTime;

										bResult = false;
										lRandTime = m_csRand.randInt()%15000;

										pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount + 15000 + lRandTime;

										//이동할 곳도 정했으니 그냥 읊조려준다.
										ProcessNPCDialog( (ApBase *)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_WANDERING );

										m_pcsAgpmCharacter->MoveCharacter( pcsAgpdMobCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION );

										if( bResult )
										{
											//AgpmCharacter::OnIdle()에서 CallBack호출! GetNextNode를 호출하고 이동한다.
										}
										else
										{
											//이건 길이 없다는 이야기인데.... 그냥 무시해야지 뭐.... 쩝.
										}
									}

									//보스몹이면 자신의 부하들에게도 같은 위치를 정해주고 위치복사및 길찾기까지처리 미리 해준다.
									if( m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 )
									{
										AgpdAI2ADChar		*pcsAI2ADFollowerChar;
										AgpdCharacter		*pcsAgpdFollowerCharacter;

										INT32				lCurrentFollowers;
										INT32				lRealCounter;
										INT32				lDegreePerFollower;
										float				fFollowerMargin;
										float				fFormationTempX, fFormationTempZ;

										lCurrentFollowers = pcsAgpdAI2ADChar->m_lCurrentFollowers;

										if( lCurrentFollowers > 0 )
										{
											lDegreePerFollower = 360/lCurrentFollowers;
											lRealCounter = 0;
											fFollowerMargin = 300.0f + lCurrentFollowers*30.0f;

											for( int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i )
											{
												pcsAgpdFollowerCharacter = pcsAgpdAI2ADChar->m_pcsFollower[i];

												if( pcsAgpdFollowerCharacter != NULL && pcsAgpdFollowerCharacter->m_Mutex.WLock())
												{
													fFormationTempX = sin(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;
													fFormationTempZ = cos(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;

													pcsAI2ADFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdFollowerCharacter);

													//만약 이동중이면 멈춘다.
													if (pcsAgpdFollowerCharacter->m_bMove)
														m_pcsAgpmCharacter->StopCharacter(pcsAgpdFollowerCharacter, NULL);

													//목적지를 정해준다.
													pcsAI2ADFollowerChar->m_csTargetPos.x = pcsAgpdAI2ADChar->m_csTargetPos.x + fFormationTempX;
													pcsAI2ADFollowerChar->m_csTargetPos.z = pcsAgpdAI2ADChar->m_csTargetPos.z + fFormationTempZ;

													pcsAI2ADFollowerChar->m_ulNextWanderingTime = pcsAgpdAI2ADChar->m_ulNextWanderingTime;

													//길도 찾는다.
													bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdFollowerCharacter->m_stPos.x,
																							pcsAgpdFollowerCharacter->m_stPos.z,
																							pcsAI2ADFollowerChar->m_csTargetPos.x,
																							pcsAI2ADFollowerChar->m_csTargetPos.z,
																							pcsAI2ADFollowerChar->m_pclPath );

													m_pcsAgpmCharacter->MoveCharacter( pcsAgpdFollowerCharacter, &pcsAI2ADFollowerChar->m_csTargetPos, MD_NODIRECTION );

													pcsAgpdFollowerCharacter->m_Mutex.Release();

													++lRealCounter;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pcsAgpdPCCharacter)
		pcsAgpdPCCharacter->m_Mutex.Release();

	return bResult;
}
*/

BOOL AgsmAI2::ProcessMonsterAI(AgpdCharacter *pcsCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	AgpdCharacter			*pcsAgpdModCharacter = NULL;
	AgpdCharacter			*pcsAgpdPCCharacter = NULL;
	AgsmAI2ResultMove		eResultMove = AGSMAI2_RESULT_MOVE_FAIL;
	AgsmAI2SpecificState	eSpecificState = AGSMAI2_STATE_NONE;

	BOOL			bResult;
	BOOL			bUseAreaAI;

	AuPOS			csMobPos;
	INT32			lMobID;
	float			fPreemptiveRange;

	pcsAgpdModCharacter = NULL;
	pcsAgpdPCCharacter  = NULL;
	bResult				= FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return FALSE;

	pcsAgpdModCharacter = pcsCharacter;

	// PinchMonster인경우 세팅부터 해준다.
	SetPinchMonster(pcsAgpdModCharacter);

	// 스턴상태인 경우는 한턴을 그냥 넘겨버린다.
	if(pcsAgpdModCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return TRUE;

	csMobPos = pcsCharacter->m_stPos;
	lMobID	 = pcsCharacter->m_lID;
	fPreemptiveRange = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fPreemptiveRange;
	
	// 스크림을 사용하는 몬스터라면 일정 시간후에 다시 사용할수 있도록 초기화를 해준다.
	if(TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
	{
		if(pcsAgpdAI2ADChar->m_ulScreamStartTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulFellowWorkerRefreshTime < GetClockCount())
			pcsAgpdAI2ADChar->m_bScreamUsed = FALSE;
	}

	// 몬스터 공통으로 사용하는 함수를 호출
	MonsterCommonAI(pcsAgpdModCharacter, pcsAgpdAI2ADChar, lClockCount);

	// 보스몹인 경우 Follower들을 관리한다.
	if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 && pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL)
	{
		ManagementFollower(pcsAgpdModCharacter, pcsAgpdAI2ADChar, lClockCount);
	}

	// Enemy를 찾아준다.
	pcsAgpdPCCharacter = FindEnemyTarget(pcsAgpdModCharacter, pcsAgpdAI2ADChar, csMobPos, fPreemptiveRange);

	// AuAutoLock으로 변경 
	AuAutoLock Lock;

	if(pcsAgpdPCCharacter)
	{
		Lock.SetMutualEx(pcsAgpdPCCharacter->m_Mutex);
		Lock.Lock();
		if(!Lock.Result())
			return FALSE;
	}
	
	// AreaCheck를 해준다.
	bUseAreaAI = CheckArea(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	if(bUseAreaAI == TRUE)
		return TRUE;

	eSpecificState = SpecificStateAI(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	if(eSpecificState != AGSMAI2_STATE_NONE)
		return TRUE;

	// PC AI를 Monster에도 적용시켜준다.
	// disease는 AI를 쓰는게 아니고 상태만 변경하는 것이기때문에 나머지 AI를 실행해야한다.
	eAgpmAI2StatePCAI eResult = AGPDAI2_STATE_PC_AI_NONE;

	if(TRUE == ProcessPCAI(pcsAgpdModCharacter, pcsAgpdAI2ADChar, lClockCount, &eResult, TRUE) && eResult != AGPDAI2_STATE_PC_AI_DISEASE)
		return TRUE;

	// Enemy한테 가던지 아니면 자기 목적을 향해서 이동한다.
	eResultMove = MoveAI(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	
	if(eResultMove == AGSMAI2_RESULT_MOVE_TARGET_IN_RANGE)
	{
		AttackEnemy(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	}

	return TRUE;
}

// 2005.08.03. steeple 수정
BOOL AgsmAI2::ProcessSummonAI(AgpdCharacter* pcsSummons, UINT32 lClockCount)
{
	if(!pcsSummons)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pcsAgpmSummons->GetADCharacter(pcsSummons);
	if(!pcsSummonsADChar)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessSummonsAI"));

	if(pcsSummonsADChar->m_lOwnerCID == AP_INVALID_CID)
	{
		ASSERT(0);
		return FALSE;
	}

	// 고정형 소환수일 때는 그냥 나가자. 2005.10.05. steeple
	if(m_pcsAgpmCharacter->IsStatusFixed(pcsSummons))
		return TRUE;

	// Owner 를 구한다.
	ApAutoLockCharacter csAutoLockCharacter(m_pcsAgpmCharacter, pcsSummonsADChar->m_lOwnerCID);
	AgpdCharacter* pcsOwner = csAutoLockCharacter.GetCharacterLock();
	if(!pcsOwner)
	{
		//ASSERT(0);
		return FALSE;
	}

	// 2008.03.17. steeple
	// 아래 TID 검사는 뺐다.
	// 변신풀릴 때 애로사항이 꽃피게 된다.
	//if (pcsSummonsADChar->m_lOwnerTID != AP_INVALID_CID &&
	//	pcsSummonsADChar->m_lOwnerTID != pcsOwner->m_pcsCharacterTemplate->m_lID)
	//{
	//	//ASSERT(0);
	//	return FALSE;
	//}

	// 2005.08.03. steeple
	// 이 곳 AI 엄청 보강 해야 함. -_-;;;
	//
	//
	//
	//

	// 2005.09.06. steeple
	// 먼저 공격을 하고 공격할 대상이 없으면 주인에게 가는 것으로 한다.
	//
	// 캐릭터가 타겟으로 삼은 몬스터가 있다면 공격한다.
	AgpdCharacter* pcsTarget = GetAttackTargetPC(pcsOwner, pcsSummons, pcsSummonsADChar->m_stSummonsData.m_ePropensity);

	if(pcsTarget && pcsTarget->m_lID != pcsOwner->m_lID && pcsTarget->m_lID != pcsSummons->m_lID)
	{
		AuAutoLock csLock(pcsTarget->m_Mutex);
		if (!csLock.Result())
		{
			// 2007.01.24. steeple
			// 얘가 한번 떴는데... 디버깅 하기 힘들다.
			// 타겟 락 못했다고 return FALSE 해버리면 소환수가 사라지게 된다.
			return TRUE;
		}

		INT32 lRange = 0;
		INT32 lTargetRange = 0;
		AuPOS stPos = pcsTarget->m_stPos;

		// 어차피 ActionCharacter 안에서 거리 체크하고 공격하라고 한다.
		if(m_pcsAgpmFactors->IsInRange(&pcsSummons->m_stPos, &pcsTarget->m_stPos, &pcsSummons->m_csFactor, &pcsTarget->m_csFactor, &stPos, &lRange, &lTargetRange))
		{
			m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsSummons, (ApBase *)pcsTarget, NULL);
			return TRUE;
		}
		else if(lTargetRange < AGSMAI2_Summons_Max_Follow)
		{
			m_pcsAgpmCharacter->MoveCharacterFollow(pcsSummons, pcsTarget, lRange, TRUE);
			m_pcsAgsmCharacter->SetIdleInterval(pcsSummons, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_ONE_SECOND);
			return TRUE;
		}
	}

	// 2007.07.25. steeple
	// 전투중임에도 불고하고, 주인이 멀어졌을 때는 주인을 따라가야 하므로 아래 코드 주석처리 하였다.
	//
	//
	// 소환수가 전투중일때는 캐릭터를 따라가거나 하지 않는다.
	//if (TRUE == m_pcsAgpmCharacter->IsCombatMode(pcsSummons))
	//	return TRUE;

	// 거리가 5m 이상 떨어지면 무조건 근처로 따라간다.
	AuPOS stDest;
	INT32 lTargetDistance = 0;	// IsInRange가 FALSE일 때만 값이 설정된다.

	INT32 lAOIRange = m_lSummonAOIRange;
	INT32 lJumpDistance = ( INT32 ) AGSMAI2_Summons_Max_Jump;
	if(m_pcsAgpmSummons->IsPet(pcsSummons))
	{
		if(pcsSummons->m_pcsCharacterTemplate->m_lPetType == 1)
			lAOIRange = m_lPetAOIRange2;
		else
			lAOIRange = m_lPetAOIRange;

		lJumpDistance = ( INT32 ) AGSMAI2_Pet_Max_Jump;
	}

	ZeroMemory(&stDest, sizeof(stDest));
	if(FALSE == m_pcsAgpmFactors->IsInRange(&pcsOwner->m_stPos, &pcsSummons->m_stPos, lAOIRange, 0, &stDest, &lTargetDistance))
	{
		if (lTargetDistance > lJumpDistance)
		{
			if (TRUE == m_pcsAgpmCharacter->IsStatusSummoner(pcsSummons))
			{
				// 소환수가 거리가 너무 멀어지면 강제 소환
				m_pcsAgpmCharacter->StopCharacter(pcsSummons, NULL);
				m_pcsAgpmCharacter->UpdatePosition(pcsSummons, &pcsOwner->m_stPos, FALSE, TRUE);
			}
			else
			{
				// 테이밍 몬스터가 너무 멀어지면 테이밍 종료
				m_pcsAgsmSummons->EndSummonsPeriod(pcsSummons);
			}
		}
		else
		{
			// 일정거리 안에 있으면 따라가기
			m_pcsAgpmCharacter->MoveCharacterFollow(pcsSummons, pcsOwner, lAOIRange, TRUE);
			m_pcsAgsmCharacter->SetIdleInterval(pcsSummons, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS);
		}
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAI(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount, eAgpmAI2StatePCAI *eResult, BOOL bMonsterUse)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar || NULL == eResult)
		return FALSE;

	if(m_pcsAgpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) == TRUE)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	if(!bMonsterUse)
	{
		// Monster들은 ProcessMonsterAI에서 처리하도록 한다.
		if( m_pcsAgpmCharacter->IsMonster(pcsAgpdCharacter) || m_pcsAgpmCharacter->IsCreature(pcsAgpdCharacter) ||
			m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter) )
			return FALSE;
	}

	eAgpmAI2StatePCAI eStatePCAI = AGPDAI2_STATE_PC_AI_NONE;

	eStatePCAI = m_pcsAgpmAI2->GetPCAIState(pcsAgpdCharacter);

	// If Character has no AI_state or has hold state skip PC AI
	if( eStatePCAI == AGPDAI2_STATE_PC_AI_NONE )
		return FALSE;

	BOOL bRet = FALSE;

	switch(eStatePCAI)
	{
	case AGPDAI2_STATE_PC_AI_CONFUSION:
		{
			bRet = ProcessPCAIConfusion(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

			*eResult = AGPDAI2_STATE_PC_AI_CONFUSION;
		}break;

	case AGPDAI2_STATE_PC_AI_BERSERK:
		{
			if(bMonsterUse)
			{
				
			}
			else
			{
				bRet = ProcessPCAIBerserk(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

				*eResult = AGPDAI2_STATE_PC_AI_BERSERK;
			}
		}break;

	case AGPDAI2_STATE_PC_AI_FEAR:
		{
			bRet = ProcessPCAIFear(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

			*eResult = AGPDAI2_STATE_PC_AI_FEAR;
		}break;

	case AGPDAI2_STATE_PC_AI_SHRINK:
		{
			bRet = TRUE;

			*eResult = AGPDAI2_STATE_PC_AI_SHRINK;
		}break;

	case AGPDAI2_STATE_PC_AI_DISEASE:
		{
			bRet = ProcessPCAIDisease(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

			*eResult = AGPDAI2_STATE_PC_AI_DISEASE;
		}break;

	case AGPDAI2_STATE_PC_AI_HOLD:
		{
			bRet = TRUE;

			*eResult = AGPDAI2_STATE_PC_AI_HOLD;
		}break;
	}

	return bRet;
}

BOOL AgsmAI2::ProcessPCAIConfusion(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_CONFUSION)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
	   pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
	   return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	// Calc Next Target Position
	//////////////////////////////////////////////////////////////////
	
	AuPOS stCurrentPOS = pcsAgpdCharacter->m_stPos;
	AuPOS stFirstPOS = pcsAgpdAI2ADChar->m_stPCAIInfo.stFirstPOS;

	if(stFirstPOS.x == 0.0f && stFirstPOS.z == 0.0f)
	{
		// first
		stFirstPOS = stCurrentPOS;
		pcsAgpdAI2ADChar->m_stPCAIInfo.stFirstPOS = pcsAgpdCharacter->m_stPos;

		// 최초 위치보다 2m앞으로 움직인다.
		AuPOS stDestPos = stFirstPOS;
		stDestPos.z = stDestPos.z + 300.0f;
		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDestPos, MD_NODIRECTION, FALSE, TRUE);
	}
	else
	{
		// Not First
		pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount++;
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount >= 6)
			pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount = 0;

		AuPOS stDestPos = stFirstPOS;

		INT32 lDegree = (360 / (6 - pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount));

		FLOAT fTempPOSX = sin(lDegree*AGSMAI2_Radian) * 400.0f;
		FLOAT fTempPOSZ = cos(lDegree*AGSMAI2_Radian) * 400.0f;

		stDestPos.x = stDestPos.x + fTempPOSX;
		stDestPos.z = stDestPos.z + fTempPOSZ;

		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDestPos, MD_NODIRECTION, FALSE, TRUE);
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAIFear(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_FEAR)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////
	
	AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID);
	if(NULL == pcsTarget)
		return FALSE;

	AuPOS stDest;
	AuPOS stFrom		= pcsAgpdCharacter->m_stPos;
	AuPOS stTargetPos	= pcsTarget->m_stPos;
	FLOAT fAttackRange	= 0.0f;

	ZeroMemory(&stDest, sizeof(stDest));

	m_pcsAgpmFactors->GetValue(&pcsTarget->m_pcsCharacterTemplate->m_csFactor, &fAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);

	FLOAT fTempX, fTempZ;

	fTempX = pcsTarget->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
	fTempZ = pcsTarget->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

	if(fTempX*fTempX + fTempZ*fTempZ < (fAttackRange + 5000.0f) * (fAttackRange + 5000.0f))
	{
		if(GetRunawayPosFromPC(pcsAgpdCharacter, &stFrom, &stTargetPos, &stDest) == TRUE)
		{
			m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDest, MD_NODIRECTION, FALSE, TRUE);
		}
		else
		{
			if(pcsAgpdCharacter->m_bMove)
				m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}
	}
	else
	{
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAIBerserk(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_BERSERK)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////

	AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID);
	if(NULL == pcsTarget)
		return FALSE;

	AuPOS stFrom		= pcsAgpdCharacter->m_stPos;
	AuPOS stTargetPos	= pcsTarget->m_stPos;
	FLOAT fSearchRange	= 200.0f;

	FLOAT fTempX, fTempZ;

	fTempX = pcsTarget->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
	fTempZ = pcsTarget->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

	if(fTempX*fTempX + fTempZ*fTempZ > (fSearchRange + 200.0f) * (fSearchRange + 200.0f))
	{
		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE);
	}
	else
	{
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAIDisease(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_DISEASE)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	INT32 lStunDurationTime = pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam1;
	INT32 lStunGenerateTime = pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam2;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam3 + lStunGenerateTime > GetClockCount())
		return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////
	INT32 lAdjustProbability = 0;
	
	// if pcsCharacter has Stun Protect stun_probability has to decrease
	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT)
	{
		AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if(pcsAgsdCharacter)
			lAdjustProbability += pcsAgsdCharacter->m_stInvincibleInfo.lStunProbability;
	}

	if(lAdjustProbability > 0)
	{
		// Stun Fail
		if(m_csRand.randInt(100) < lAdjustProbability)
			return FALSE;
	}

	if(lStunDurationTime == 0)
		lStunDurationTime = 3000;

	// 캐릭터의 Special Status에 Set 해준다.
	m_pcsAgsmCharacter->SetSpecialStatusTime(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_STUN, lStunDurationTime);

	// 질병으로 인한 Stun 걸린 시간을 세팅해준다.
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam3 = GetClockCount();

	return TRUE;
}

INT32 AgsmAI2::CheckUseSkill( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	//STOPWATCH2(GetModuleName(), _T("CheckUseSkill"));
	if(NULL == pcsAgpdCharacter)
		return -1;

	AgpdAI2ConditionTable	*pcsConditionTbl;
	AgpdAI2UseResult		*pcsUseResult;

	INT32 lUseSkillIndex = -1;

	if( pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
	{
		// 2007.01.25. steeple
		// 순서를 앞에서 부터 돌면 뒷번호는 Starvation 걸리는 버그가 있다.
		// 그래서 순서를 랜덤하게 지정해주는 루틴을 작성했다.
		INT32 lTotal = 0;
		for(INT32 i = 0; i < AGPDAI2_MAX_USABLE_SKILL_COUNT; ++i)
		{
			if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[i].m_lSkillID == 0)
				break;

			++lTotal;
		}

		if(lTotal == 0)
			return -1;

		std::vector<INT32> vcTmp;
		for(INT32 i = 0; i < lTotal; ++i)
			vcTmp.push_back(i);

		std::random_shuffle(vcTmp.begin(), vcTmp.end());
		INT32 lCounter = 0;

		std::vector<INT32>::iterator iter = vcTmp.begin();
		while(iter != vcTmp.end())
		{
			lCounter = *iter;

			pcsConditionTbl = &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lCounter].m_csTable;
			pcsUseResult = &pcsAgpdAI2ADChar->m_csSkillResult[lCounter];

			if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_HP )
			{
				INT32			lHPNow, lHPMax;

				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPNow, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

				if( CheckParameter( lHPNow, lHPMax, pcsConditionTbl ) )
				{
					if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
					{
						lUseSkillIndex = lCounter;
						break;
					}
				}
			}
			else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_MP )
			{
				INT32			lMPNow, lMPMax;

				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPNow, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

				if( CheckParameter( lMPNow, lMPMax, pcsConditionTbl ) )
				{
					if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
					{
						lUseSkillIndex = lCounter;
						break;
					}
				}
			}
			else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_SP )
			{
				INT32			lSPNow, lSPMax;

				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPNow, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

				if( CheckParameter( lSPNow, lSPMax, pcsConditionTbl ) )
				{
					if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
					{
						lUseSkillIndex = lCounter;
						break;
					}
				}
			}

			++iter;
		}
	}

	return lUseSkillIndex;
}

INT32 AgsmAI2::CheckUseItem( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	//STOPWATCH2(GetModuleName(), _T("CheckUseItem"));
	if(NULL == pcsAgpdCharacter)
		return -1;

	AgpdAI2ConditionTable	*pcsConditionTbl;
	AgpdAI2UseResult		*pcsUseResult;
	AgpdFactor				*pcsFactorResult;

	INT32				lCounter;
	INT32				lUseItemIndex;

	lUseItemIndex = -1;
	pcsFactorResult = NULL;

	pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);

	if(!pcsFactorResult)
		pcsFactorResult	= &pcsAgpdCharacter->m_csFactor;

	if(!pcsFactorResult)
		return lUseItemIndex;

	if( pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
	{
		for( lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; ++lCounter )
		{
			if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseItem[lCounter].m_lItemID != 0 )
			{
				pcsConditionTbl = &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseItem[lCounter].m_csTable;
				pcsUseResult = &pcsAgpdAI2ADChar->m_csItemResult[lCounter];

				if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_HP )
				{
					INT32			lHPNow, lHPMax;

					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

					if( CheckParameter( lHPNow, lHPMax, pcsConditionTbl ) )
					{
						if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
						{
							lUseItemIndex = lCounter;
							break;
						}
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_MP )
				{
					INT32			lMPNow, lMPMax;

					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

					if( CheckParameter( lMPNow, lMPMax, pcsConditionTbl ) )
					{
						if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
						{
							lUseItemIndex = lCounter;
							break;
						}
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_SP )
				{
					INT32			lSPNow, lSPMax;

					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

					if( CheckParameter( lSPNow, lSPMax, pcsConditionTbl ) )
					{
						if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
						{
							lUseItemIndex = lCounter;
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	return lUseItemIndex;
}

BOOL AgsmAI2::CheckPreemptive( AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter, 
								AgpdAI2ADChar *pcsAgpdAI2ADChar, BOOL bUseScream)
{
	//STOPWATCH2(GetModuleName(), _T("CheckPreempTive"));

	AgpdAI2ConditionTable	*pcsConditionTbl;
	AgpdFactor				*pcsFactorResult;

	BOOL				bResult;
	BOOL				bAggroResult;
	INT32				lCounter = 0;
	INT32				lMaxPreemptiveCondition = 0;

	bResult = FALSE;
	pcsFactorResult = NULL;
	pcsConditionTbl = NULL;

	if (bUseScream)
	{
		// condition table 3개중 마지막 항목에는 스크림 발동 조건을 넣어놓는다.
		lCounter = 2;
		lMaxPreemptiveCondition = AGPDAI2_MAX_PREEMPTIVECONDITON;
	}
	else
	{
		lCounter = 0;
		lMaxPreemptiveCondition = AGPDAI2_MAX_PREEMPTIVECONDITON - 1;
	}

	//대상을 본다.
	if( pcsAgpdAI2ADChar )
	{
		for( ; lCounter < lMaxPreemptiveCondition; ++lCounter )
		{
			if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csPreemptiveCondition[lCounter].m_bSet )
			{
				pcsConditionTbl = &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csPreemptiveCondition[lCounter].m_csTable;

				if( pcsConditionTbl->m_lAggroParameter == AGPMAI2_TARGET_AGGRO_MOBSELF )
				{
					pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
				}
				else if( pcsConditionTbl->m_lAggroParameter == AGPMAI2_TARGET_AGGRO_TARGET )
				{
					pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdTargetCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
				}

				if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_HP )
				{
					INT32			lHPNow, lHPMax;

					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lHPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lHPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

					if( CheckParameter( lHPNow, lHPMax, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_MP )
				{
					INT32			lMPNow, lMPMax;

					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lMPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lMPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

					if( CheckParameter( lMPNow, lMPMax, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_SP )
				{
					INT32			lSPNow, lSPMax;

					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lSPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lSPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

					if( CheckParameter( lSPNow, lSPMax, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_LV )
				{
					INT32				lMobLevel, lTargetLevel;
					INT32				lLevelGap;

					lMobLevel = m_pcsAgpmCharacter->GetLevel( pcsAgpdCharacter );
					lTargetLevel = m_pcsAgpmCharacter->GetLevel( pcsAgpdTargetCharacter );

					lLevelGap = lTargetLevel - lMobLevel;

					if( CheckParameter( lLevelGap, 0, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	//거리만 있고 조건은 없는경우. 이런 경우는 무조건 어그로가 없는 상태 Return FALSE다.
	if( pcsConditionTbl == NULL )
	{
		bAggroResult = FALSE;
	}
	//정상적으로 거리와 조건이 있는 경우임.
	else
	{
		//조건이 맞은 경우.
		if( bResult == TRUE) //bResult = TRUE
		{
			//금지 조건이었다면...
			if( pcsConditionTbl->m_bAggroAccept == FALSE )
			{
				bAggroResult = FALSE;
			}
			//허용 
			else
			{
				bAggroResult = TRUE;
			}		
		}
		//조건이 맞지 않은 경우.
		else
		{
			//금지였다면... 반대로 허용한다.
			if( pcsConditionTbl->m_bAggroAccept == FALSE )
			{
				bAggroResult = TRUE;
			}
			//허용이었다면 반대로 금지한다.
			else
			{
				bAggroResult = FALSE;
			}		
		}
	}

	return bAggroResult;
}

BOOL AgsmAI2::CheckUseScream( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	BOOL				bResult;

	bResult = FALSE;

	//스크림 가능한 몬스터인지 본다.
	if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFellowWorker.m_lFellowWorkerTotalCount )
	{
		if( pcsAgpdAI2ADChar->m_bScreamUsed == FALSE )
		{
			if( m_pcsAgsmCharacter->IsNoAgro( pcsAgpdCharacter ) == FALSE )
			{
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgsmAI2::ProcessUseSkill( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, INT32 lIndex, AgpdCharacter *pcsTargetPC )
{
	//STOPWATCH2(GetModuleName(), _T("ProcessUseSkill"));

	BOOL				bResult;

	bResult = FALSE;

	if( pcsAgpdCharacter && pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
	{
		//스킬을 쌔운다~ 냠냠.
		bResult = m_pcsAgsmSkill->CastSkill( pcsAgpdCharacter, 
											pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID,
											pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillLevel,
											pcsTargetPC,
											FALSE,
											TRUE
											);
	}

	return bResult;
}

BOOL AgsmAI2::ProcessUseItem( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, INT32 lIndex, AgpdCharacter *pcsTargetPC )
{
	BOOL				bResult;

	bResult = FALSE;

	//아이템을 사용하자~ 유저는 약오를거야~~ 므흣~
	if( pcsAgpdCharacter && pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
	{
		AgpdItem			*pcsItem;

		//우리의 몬스터는 TID를 주면 아이템을 만들어서 쓰는 자급자족형 몬스터~
		//이거 TID만 주면 사용하게 수정하자~
		pcsItem = m_pcsAgsmItemManager->CreateItem( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseItem[lIndex].m_lItemID, pcsAgpdCharacter, 1 );

		if( pcsItem )
		{
			m_pcsAgsmItem->UseItem( pcsItem, TRUE );
		}
		else
		{
			//아이템이 안만들어졌군. 
			printf( "AgsmAI2::ProcessUseItem, Can't create item\n" );
		}
	}

	return bResult;
}

BOOL AgsmAI2::ProcessUseScream( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AgpdCharacter *pcsTargetPC )
{
	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
	AgpdCharacter		*pcsListCharacter;
	AgpdAI2ADChar		*pcsAgpdAI2ADMob;

	BOOL				bResult;
	INT32				lMaxFellowerWorkers;
	INT32				lCounter, lNumCID;
	INT32				lRealCounter;
	float				fRange;

	alCID.MemSetAll();
	alCID2.MemSetAll();

	bResult = FALSE;
	fRange = (FLOAT)m_pcsAgpmAI2->GetMaxFellowRange( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template );
	lMaxFellowerWorkers = m_pcsAgpmAI2->GetMaxFellowAcceptWorkers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template );
	lRealCounter = 0;

	//주변의 반경 fRange미터 내의 몬스터들을 얻어낸다.
	lNumCID = m_pcsApmMap->GetCharList( pcsAgpdCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, fRange,
										&alCID[0], AGSMAI2_MAX_PROCESS_TARGET,
										&alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

	//MaxFellowerWorkers만큼의 몬스터들에게 적대치를 준다.
	for( lCounter = 0; lCounter<lNumCID; lCounter++ )
	{
		// 스크림을 가지고 있는 몬스터를 제외하고 lMaxFellowerWorkers를 만들어야 되기 때문에 1을 더해준다.
		if( lRealCounter < (lMaxFellowerWorkers + 1))
		{
			pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(alCID2[lCounter]);

			if( pcsListCharacter )
			{
				// 2007.03.05. steeple
				if( pcsListCharacter->m_bIsReadyRemove )
					continue;

				//동종 업종으로 지정된 몬스터인지 본다.
				if( m_pcsAgpmAI2->IsFellowWorker( &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFellowWorker, ((AgpdCharacterTemplate *)pcsListCharacter->m_pcsCharacterTemplate)->m_lID ) )
				{
					if( pcsListCharacter->m_Mutex.WLock() )
					{
						INT32				lDecreaseAggroPointPerCycle;
						INT32				lMaxHP;

						//TargetPC에 대해서 Slow어그로를 넣어준다.
						lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsListCharacter->m_csFactor);

						lDecreaseAggroPointPerCycle = (lMaxHP*pcsListCharacter->m_pcsCharacterTemplate->m_csDecreaseData.m_lSlowPercent)/100;

						//5회 정도 Slow 감소 어그로 없어질만큼의 Slow 어그로를 부여한다.
						if( lDecreaseAggroPointPerCycle == 0 )
						{
							lDecreaseAggroPointPerCycle = 1;
						}

						lDecreaseAggroPointPerCycle *= 7;

						m_pcsAgsmCharacter->AddAgroPoint( pcsListCharacter, pcsTargetPC, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseAggroPointPerCycle );

						pcsAgpdAI2ADMob = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);

						//만약 특별한 타켓이 없다면 바로 반응을 할 수 있도록 타켓을 세팅해준다.
						if( pcsAgpdAI2ADMob != NULL )
						{
							if( pcsAgpdAI2ADMob->m_lTargetID == 0 )
							{
								pcsAgpdAI2ADMob->m_lTargetID = pcsAgpdAI2ADChar->m_lTargetID;
							}
						}

						pcsListCharacter->m_Mutex.Release();

						lRealCounter++;

						bResult = TRUE;
					}
				}
			}
		}
		else
		{
			break;
		}
	}


	return bResult;
}

BOOL AgsmAI2::ProcessCheckArea( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT lClockCount )
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if (m_pcsAgpmCharacter->IsStatusTame(pcsAgpdCharacter))
		return FALSE;

	// 도망자 놈들은 이 AreaCheck를 무시해준다.
	if(AGPDAI2_ESCAPE_ESCAPE == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eEscape)
		return FALSE;

	FLOAT	fTempDistance;
	FLOAT	fTempX;
	FLOAT	fTempZ;

	if (pcsAgpdAI2ADChar->m_csFirstSpawnPos.x == 0.0f &&
		pcsAgpdAI2ADChar->m_csFirstSpawnPos.y == 0.0f &&
		pcsAgpdAI2ADChar->m_csFirstSpawnPos.z == 0.0f)
		return FALSE;

	fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;

	fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csFirstSpawnPos.x;
	fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csFirstSpawnPos.z;

	//만약 스폰범위의 3배안에 존재하지 않는다면 지금 위치를 새로운 스폰포인트로 잡는다.
	// Spawn Radius가 넘 작은 넘들이 있어서.... 걍 100m 로 고정
	if( fTempX*fTempX + fTempZ*fTempZ > (FLOAT) (10000 * 10000) )
	{
		{
			pcsAgpdAI2ADChar->m_csTargetPos	= pcsAgpdAI2ADChar->m_csFirstSpawnPos;

			//이동할 곳도 정했으니 그냥 읊조려준다.
			//ProcessNPCDialog( (ApBase *)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_WANDERING );

			m_pcsAgsmCharacter->InitHistory( pcsAgpdCharacter );
			m_pcsAgsmFactors->Resurrection( &pcsAgpdCharacter->m_csFactor, NULL );
			m_pcsAgpmCharacter->UpdatePosition( pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csFirstSpawnPos, FALSE, TRUE );

			pcsAgpdAI2ADChar->m_lTargetID	= AP_INVALID_CID;
			pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

			pcsAgpdAI2ADChar->m_pclPath->resetIndex();

			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL AgsmAI2::ProcessCheckTargetArea( AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter )
{
	if (!pcsAgpdCharacter || !pcsAgpdTargetCharacter)
		return FALSE;

	if (m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdTargetCharacter))
		return FALSE;

	AgpdAI2ADChar *		pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(pcsAgpdAI2ADChar == NULL)
		return FALSE;
	
	if(IsRunawayStateCharacter(pcsAgpdCharacter) == TRUE)
		return FALSE;

	FLOAT	fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdTargetCharacter->m_stPos.x;
	FLOAT	fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdTargetCharacter->m_stPos.z;

	//만약 스폰범위의 3배안에 존재하지 않는다면 지금 위치를 새로운 스폰포인트로 잡는다.
	// Spawn Radius가 넘 작은 넘들이 있어서.... 걍 100m 로 고정
	if( fTempX*fTempX + fTempZ*fTempZ > (FLOAT) (10000 * 10000) )
	{
		// 너무 멀리 떨어져있다.
		pcsAgpdAI2ADChar->m_lTargetID	= 0;
		pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

		pcsAgpdAI2ADChar->m_pclPath->resetIndex();

		m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

		return TRUE;
	}

	return FALSE;
}

BOOL AgsmAI2::CBSetCombatMode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAI2			*pThis			= (AgsmAI2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdAI2ADChar	*pcsAI2ADChar	= pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);

	if (pcsAI2ADChar->m_bUseAI2 &&
		pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
	{
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	}

	return TRUE;
}

BOOL AgsmAI2::CBResetCombatMode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAI2			*pThis			= (AgsmAI2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdAI2ADChar	*pcsAI2ADChar	= pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);

	if (pcsAI2ADChar->m_bUseAI2 &&
		pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
	{
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS);
	}

	return TRUE;
}

// 2005.08.30. steeple
// AgsmCombat 에서 Attack 시작 할 때 부른다.
BOOL AgsmAI2::CBAttackStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	pstAgsmCombatAttackResult pstAttackResult = static_cast<pstAgsmCombatAttackResult>(pData);
	AgsmAI2* pThis = static_cast<AgsmAI2*>(pClass);

	if(!pstAttackResult->pAttackChar || !pstAttackResult->pTargetChar)
		return FALSE;

	// Attacker 가 PC 가 아니면 할 필요 없다.
//	if(!pThis->m_pcsAgpmCharacter->IsPC(pstAttackResult->pAttackChar))
//		return TRUE;

	return pThis->ProcessSetTargetToSummons(pstAttackResult->pAttackChar, pstAttackResult->pTargetChar->m_lID);
}

// 2005.09.06. steeple
// AgsmSkill 에서 스킬 적용을 다 시키구 나서 최후에 부른다.
BOOL AgsmAI2::CBAffectedSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSkill* pcsSkill = static_cast<AgpdSkill*>(pData);
	AgsmAI2* pThis = static_cast<AgsmAI2*>(pClass);
	AgpdCharacter* pcsTarget = static_cast<AgpdCharacter*>(pCustData);

	if(!pcsSkill->m_pcsBase)
		return FALSE;

	// Skill 쓰는 놈이 PC 가 아니면 할 필요 없다.
	if(!pThis->m_pcsAgpmCharacter->IsPC((AgpdCharacter*)pcsSkill->m_pcsBase))
		return TRUE;

	// Attack Skill (Debuff 포함) 이 아니면 할 필요 없다.
//	if(!pThis->m_pcsAgpmPvP->IsAttackSkill(pcsSkill))
//		return TRUE;

	return pThis->ProcessSetTargetToSummons((AgpdCharacter*)pcsSkill->m_pcsBase, pcsTarget->m_lID);
}

// PinchWantedCharacter가 있는지 Request가 올때 처리하는 CB
BOOL AgsmAI2::CBPinchWantedRequest(PVOID pData, PVOID pClass, PVOID pCusData)
{
	if(!pData || !pClass)
		return FALSE;

	PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST *pRequestPacket = (PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST *)(pData);
	AgsmAI2 *pThis = static_cast<AgsmAI2*>(pClass);

	AgpdCharacter *pcsRequestUser		= pThis->m_pcsAgpmCharacter->GetCharacter(pRequestPacket->lUserID);
	AgpdCharacter *pcsPinchCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(pRequestPacket->lRequestCharacterID);
	if(!pcsRequestUser || !pcsPinchCharacter)
		return FALSE;

	AgsdCharacter* pagsdRequestUser = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsRequestUser);
	if(!pagsdRequestUser)
		return FALSE;

	// 패킷을 생성 
	PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER pAnswerPacket;
	pAnswerPacket.lPinchCharacterID = pRequestPacket->lRequestCharacterID;

	AgpdAI2ADChar *pcsAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData(pcsPinchCharacter);
	if(pcsAI2ADChar && pcsAI2ADChar->m_pcsAgpdAI2Template)
	{
		if(pcsAI2ADChar->m_csPinchInfo.pRequestMobVector.empty() == FALSE)
		{
			INT32 lIndex = 0;

			vector<AgpdCharacter*>::iterator it = pcsAI2ADChar->m_csPinchInfo.pRequestMobVector.begin();
			for( ; it !=  pcsAI2ADChar->m_csPinchInfo.pRequestMobVector.end(); it++)
			{
				AgpdCharacter *pcsWantedCharacter = *it;
				if(pcsWantedCharacter)
				{
					if(lIndex >= MAX_PINCH_WANTED_CHARACTER) break;

					pAnswerPacket.lPinchWantedCharacter[lIndex++] = pcsWantedCharacter->m_lID;
				}
			}
		}
	}

	AgsEngine::GetInstance()->SendPacket(pAnswerPacket, pagsdRequestUser->m_dpnidCharacter);

	return TRUE;
}

// 2005.09.06. steeple
// 몬스터 Spawn Callback 이 불렸을 때 그 함수 안에서 이놈을 불러주게 된다.
BOOL AgsmAI2::ProcessSpawnCastSkill(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 여기선 몬스터 관련만 해준다.
	if(!m_pcsAgpmCharacter->IsMonster(pcsCharacter))
		return TRUE;

	AgpdAI2ADChar* pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsCharacter);
	if(!pcsAgpdAI2ADChar || !pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return TRUE;

	AgpdSkillTemplate* pcsSkillTemplate = NULL;
	for(INT32 lIndex = 0; lIndex < AGPDAI2_MAX_USABLE_SKILL_COUNT; lIndex++)
	{
		if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID == 0)
			break;

		pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID);
		if(!pcsSkillTemplate)
			continue;

		// Passive 나 Self Buff 면 캐스트 해주자.
		if(m_pcsAgpmSkill->IsPassiveSkill(pcsSkillTemplate) ||
			m_pcsAgpmSkill->IsSelfBuffSkill(pcsSkillTemplate))
		{
			if(m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter) || m_pcsAgpmCharacter->IsStatusFixed(pcsCharacter))
			{
				m_pcsAgsmSkill->CastSkill(pcsCharacter, 
										pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID,
										pcsAgpdAI2ADChar->m_lCastSkillLevel,	// 이놈이 다름.
										NULL,
										FALSE,
										TRUE
										);
			}
			else
			{
				m_pcsAgsmSkill->CastSkill(pcsCharacter, 
										pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID,
										pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillLevel,
										NULL,
										FALSE,
										TRUE
										);
			}
		}
	}

	return TRUE;
}


void AgsmAI2::SetSummonAOIRange(int lRange)
{
	ASSERT(lRange);
	m_lSummonAOIRange = lRange;
}

void AgsmAI2::SetSummonPropensity(int lCID, int lPropensity)
{
	ASSERT(0 != lCID);
	
	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

	ASSERT(pcsCharacter);
	if (NULL == pcsCharacter)
		return ;

	m_pcsAgpmSummons->SetPropensity(pcsCharacter, (EnumAgpdSummonsPropensity)lPropensity);
}

#ifdef __PROFILE__
void AgsmAI2::SetProfileDelay(int delay)
{
	SetProfileDelay((DWORD)delay);
}

void AgsmAI2::Sleep1sec()
{
	Sleep(1000);
}
#endif


BOOL AgsmAI2::SetCallbackGetTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAI2_CB_GET_TARGET, pfCallback, pClass);
}

BOOL AgsmAI2::CheckEscape(AgpdCharacter* pcsTarget)
{
	ASSERT(pcsTarget);

	if (NULL == pcsTarget)
		return FALSE;

	if (FALSE == m_pcsAgpmCharacter->IsMonster(pcsTarget))
		return FALSE;

	AgpdAI2ADChar *pstAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsTarget);
	if (NULL == pstAI2ADChar || 
		NULL == pstAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if (AGPDAI2_ESCAPE_ESCAPE == pstAI2ADChar->m_pcsAgpdAI2Template->m_eEscape)
	{
		// 근처의 랜덤한 곳으로 이동하기 위한 좌표를 찾는다.
		if (FALSE == GetTargetPosFromSpawn(pcsTarget, &pcsTarget->m_stPos, &pstAI2ADChar->m_csTargetPos, 
			&pstAI2ADChar->m_csSpawnBasePos, pstAI2ADChar->m_fSpawnRadius * 2.0f/* spawn 지역이 너무 좁아서 좀더 넓혀줌*/))
			return FALSE;

		INT32 lRandTime = m_csRand.randInt() % 15000;
		pstAI2ADChar->m_ulNextWanderingTime = GetClockCount() + 15000 + lRandTime;
		m_pcsAgpmCharacter->MoveCharacter( pcsTarget, &pstAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE );

		return TRUE;
	}
		
	return FALSE;	
}

BOOL AgsmAI2::ManagementFollower(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL || pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return FALSE;

	if(IsRunawayStateCharacter(pcsAgpdCharacter) == TRUE)
		return FALSE;

	// Follower들이 없는 Mob이나 현재 Follower들이 초과된 놈들은 넘긴다.
	if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) <= 0)
		return FALSE;

	if(m_pcsAgpmAI2->GetCumulativeFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) <= pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower)
		return FALSE;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	AgpdAI2ADChar	*pcsAgpdAI2TempADChar = NULL;
	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
	alCID.MemSetAll();
	alCID2.MemSetAll();

	INT32	lNumCID = 0;
	INT32	lCounter = 0;
	float	fVisibility = 2000.0f;
	float	fTempX, fTempZ = 0.0f;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility > 0.0f)
	{
		fVisibility = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	lNumCID = m_pcsApmMap->GetCharList(pcsAgpdCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, fVisibility,
									   &alCID[0], AGSMAI2_MAX_PROCESS_TARGET, 
									   &alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

	AgpdCharacter *pcsFollowerCharacter = NULL;

	// Follower 캐릭터중에서 시야 밖으로 떨어진 녀석들은 리스트에서 삭제한다.
	for(lCounter=0; lCounter<AGPDAI2_MAX_FOLLOWER_COUNT; ++lCounter)
	{
		if(NULL == pcsAgpdAI2ADChar->m_pcsFollower[lCounter])
			continue;

		if(pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_bIsReadyRemove)
			continue;

		if(IsRunawayStateCharacter(pcsAgpdAI2ADChar->m_pcsFollower[lCounter]) == TRUE)
			continue;

		fTempX = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
		fTempZ = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

		// 시야 밖으로 자신의 부하가 사라졌다면 뺀다.(요부분 공식확인 요망)
		if(fTempX*fTempX + fTempZ*fTempZ > fVisibility*fVisibility)
		{
			pcsFollowerCharacter = pcsAgpdAI2ADChar->m_pcsFollower[lCounter];

			pcsAgpdAI2TempADChar = m_pcsAgpmAI2->GetCharacterData(pcsFollowerCharacter);

			if(pcsAgpdAI2TempADChar)
			{
				if(pcsFollowerCharacter && pcsFollowerCharacter->m_Mutex.WLock())
				{
					// 부하의 보스를 없애주고..
					pcsAgpdAI2TempADChar->m_pcsBossCharacter = NULL;

					// 보스의 부하리스트에서 삭제한다.
					pcsAgpdAI2ADChar->m_pcsFollower[lCounter] = NULL;
					pcsAgpdAI2ADChar->m_lCurrentFollowers--;

					pcsFollowerCharacter->m_Mutex.Release();
				}
			}
		}
	}

	for (int i=0; i<lNumCID; ++i)
	{
		AgpdCharacter *pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(alCID2[i]);
		AgpdAI2ADChar *pcsAI2AdFollowerChar;
		INT32		  lFollowerTID;

		if(pcsListCharacter == NULL)
			continue;

		if(pcsListCharacter->m_bIsReadyRemove)
			continue;

		// 자기 자신은 뺀다
		if(pcsListCharacter->m_lID == pcsAgpdCharacter->m_lID)
			continue;

		for( INT32 lFollowerCount=0; lFollowerCount<AGPDAI2_MAX_FOLLOWER_COUNT; ++lFollowerCount)
		{
			lFollowerTID = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFollower.m_lFollowerInfo[lFollowerCount].m_lFollowerTID;

			if(lFollowerTID)
			{
				if(pcsListCharacter->m_pcsCharacterTemplate->m_lID == lFollowerTID)
				{
					if(pcsAgpdAI2ADChar->m_lCurrentFollowers >= m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template))
					{
						break;
					}

					pcsAI2AdFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);

					if(pcsListCharacter->m_Mutex.WLock())
					{
						if(m_pcsAgpmAI2->CheckIsBossMob(pcsAI2AdFollowerChar) == TRUE)
						{
							pcsListCharacter->m_Mutex.Release();
							continue;
						}

						pcsListCharacter->m_Mutex.Release();
					}

					if(pcsListCharacter->m_Mutex.WLock())
					{
						if(IsRunawayStateCharacter(pcsListCharacter) == TRUE)
						{
							pcsListCharacter->m_Mutex.Release();
							continue;
						}

						pcsListCharacter->m_Mutex.Release();
					}

					// 보스가 없는 몹인 경우만 자신의 부하로 만들수 있다.
					if(pcsAI2AdFollowerChar && pcsAI2AdFollowerChar->m_pcsBossCharacter == NULL)
					{
						BOOL	bExist = FALSE;

						for(int j=0; j<AGPDAI2_MAX_FOLLOWER_COUNT; ++j)
						{
							if(pcsAgpdAI2ADChar->m_pcsFollower[j] == pcsListCharacter)
							{
								bExist = TRUE;
								break;
							}
						}
						
						if(bExist == FALSE)
						{
							if(!pcsListCharacter->m_Mutex.WLock())
								continue;

							pcsAI2AdFollowerChar->m_pcsBossCharacter = pcsAgpdCharacter;

							pcsListCharacter->m_Mutex.Release();
						

							for(int k=0; k<AGPDAI2_MAX_FOLLOWER_COUNT; ++k)
							{
								if(pcsAgpdAI2ADChar->m_pcsFollower[k] == NULL)
								{
									if(pcsListCharacter && pcsListCharacter->m_Mutex.WLock())
									{
										pcsAgpdAI2ADChar->m_pcsFollower[k] = pcsListCharacter;
										pcsAgpdAI2ADChar->m_lCurrentFollowers++;
										pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower++;
										pcsListCharacter->m_Mutex.Release();
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return TRUE;
}

// FindEnemyTarget
AgpdCharacter* AgsmAI2::FindEnemyTarget(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS csMobPos, float fPreemptiveRange)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return NULL;

	AgpdCharacter *pcsAgpdPCCharacter = NULL;

	// 보스가 없는 캐릭터 (보스몹 자신, 보스몹이 지정되지 않은 몬스터들
	if(pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL)
	{
		pcsAgpdPCCharacter = GetTargetPC(pcsAgpdCharacter, pcsAgpdAI2ADChar, csMobPos, fPreemptiveRange);

		// 보스몹인경우 Follower들에게 적을 지정해 줘야한다.
		if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 && m_pcsAgpmAI2->CheckIsBossMob(pcsAgpdAI2ADChar))
		{
			SetFollowersTarget(pcsAgpdCharacter, csMobPos, pcsAgpdAI2ADChar);
		}
	}
	else
	{
		if(pcsAgpdAI2ADChar->m_lTargetID != 0)
		{
			pcsAgpdPCCharacter = m_pcsAgpmCharacter->GetCharacter(pcsAgpdAI2ADChar->m_lTargetID);
		}
		else
		{
			pcsAgpdPCCharacter = NULL;
		}
	}

	if(pcsAgpdPCCharacter)
	{
		if(pcsAgpdAI2ADChar->m_lTargetID != pcsAgpdPCCharacter->m_lID)
			pcsAgpdAI2ADChar->m_bIsNeedPathFind = FALSE;

		pcsAgpdAI2ADChar->m_lTargetID = pcsAgpdPCCharacter->m_lID;
	}
	else
	{
		pcsAgpdAI2ADChar->m_lTargetID = 0;
		pcsAgpdAI2ADChar->m_bIsNeedPathFind = FALSE;
	}

	return pcsAgpdPCCharacter;
}

// CheckArea 
// return 값은 Spawn위치를 옮겨주는 ProcessCheckArea()를 사용했는지 안했는지 여부를 돌려준다. (TRUE이면 한턴 쉬어야하니깐)
BOOL AgsmAI2::CheckArea(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	BOOL bUseAreaAI = FALSE; 

	// 타겟과의 거리를 계산한다. 너무 멀리 있는 놈이면 타겟에서 삭제한다.
	if(ProcessCheckTargetArea(pcsAgpdCharacter, pcsAgpdPCCharacter))
	{
		pcsAgpdPCCharacter = NULL;
	}

	bUseAreaAI = ProcessCheckArea(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

	return bUseAreaAI;
}

// MoveToEnemy (Enemy가 있는 녀석들은 Enemy에게 없는 녀석들은 Boss 유/무에 따라 Boss에게로)
// Return 값은 EnemyTarget이 InRange안에 있다는 소리다 곧 TRUE를 리턴해주면 공격개시
AgsmAI2ResultMove AgsmAI2::MoveAI(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL || pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return AGSMAI2_RESULT_MOVE_FAIL;

	AgsmAI2ResultMove eMoveReturn = AGSMAI2_RESULT_MOVE_FAIL;

	// 아이템 주워버리는게 가장 우선순위이다.
	if(LootItem(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount) == TRUE)
	{
		return AGSMAI2_RESULT_MOVE_LOOT;
	}

	if(pcsAgpdPCCharacter && pcsAgpdCharacter->m_bIsTrasform == FALSE)
	{
		PROFILE("AGSMAI2::AttackTarget");

		BOOL bInRange  = FALSE;
		BOOL bPathFind = FALSE;
		BOOL bResult   = FALSE;

		AuPOS stDestPos;
		ZeroMemory(&stDestPos, sizeof(stDestPos));
		
		if(RunawayFromPC(pcsAgpdCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount) == TRUE)
		{
			eMoveReturn = AGSMAI2_RESULT_MOVE_RUNAWAY;
		}
		else
		{
			bInRange = m_pcsAgpmFactors->IsInRange(&pcsAgpdCharacter->m_stPos, &pcsAgpdPCCharacter->m_stPos, &pcsAgpdCharacter->m_csFactor,
												   &pcsAgpdPCCharacter->m_csFactor, &stDestPos);
		}

		// 공격 범위 안인가?
		if(bInRange)
		{
			// 공격할수 있다는 리턴값을 준다.
			eMoveReturn = AGSMAI2_RESULT_MOVE_TARGET_IN_RANGE;
		}
		// 공격범위 밖이라면
		else if (!pcsAgpdAI2ADChar->m_bIsNeedPathFind && eMoveReturn != AGSMAI2_RESULT_MOVE_RUNAWAY &&
				 pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == FALSE)
		{	
			if(FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
			{
				pcsAgpdAI2ADChar->m_csTargetPos = stDestPos;
				m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE);
			}
		}
		else if(eMoveReturn != AGSMAI2_RESULT_MOVE_RUNAWAY &&
			    pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == FALSE)
		{
			if(FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
			{
				pcsAgpdAI2ADChar->m_csTargetPos = pcsAgpdPCCharacter->m_stPos;

				// 공격전에 길을 찾아둔다
				bResult = m_pcsAgpmPathFind->pathFind(pcsAgpdCharacter->m_stPos.x,
													  pcsAgpdCharacter->m_stPos.z,
													  pcsAgpdAI2ADChar->m_csTargetPos.x,
													  pcsAgpdAI2ADChar->m_csTargetPos.z,
													  pcsAgpdAI2ADChar->m_pclPath);

				if(!bResult)
				{
					pcsAgpdCharacter->m_ulCheckBlocking = 0;
					pcsAgpdAI2ADChar->m_bIsNeedPathFind = FALSE;
				}

				CPriorityListNode<int, AgpdPathFindPoint*> *pcsNode;

				pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

				if(pcsNode)
				{
					AuPOS csPos;

					csPos.x = pcsNode->data->m_fX;
					csPos.y = 0.0f;
					csPos.z = pcsNode->data->m_fY;

					m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE);
				}
			}
		}
	}
	else if((FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus)) &&
		    (pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == FALSE))
	{
		if(MoveWandering(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount))
			eMoveReturn = AGSMAI2_RESULT_MOVE_WANDERING;
	}

	return eMoveReturn;
}

// AttackEnemy (Enemy가 Target범위 안에 있으면 공격을 한다.)
BOOL AgsmAI2::AttackEnemy(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdPCCharacter == NULL || pcsAgpdAI2ADChar == NULL || pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return FALSE;

	// Normal Attack이 가능하면 거리를 계산한다. 
	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bUseNormalAttack)
	{
		// 공격해도 되면 공격한다.
		if(lClockCount > pcsAgpdAI2ADChar->m_ulLastAttackTime + pcsAgpdAI2ADChar->m_lAttackInterval)
		{
			// 공격할때 읊조릴 말 호출한다.
			ProcessNPCDialog(pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_ATTACK);

			m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsAgpdCharacter, (ApBase*)pcsAgpdPCCharacter, NULL);

			pcsAgpdAI2ADChar->m_ulLastAttackTime = lClockCount;
		}
		else
		{
			// 만약 이동중이면 멈춘다
			if(pcsAgpdCharacter->m_bMove)
				m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}
	}
	else
	{
		// 만약 이동중이면 멈춘다
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
	}

	return TRUE;
}

// MoveWandering (방랑하는 유닛들)
BOOL AgsmAI2::MoveWandering(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	STMonsterPathInfo *pstMonsterPathInfo = NULL;

	if (pstMonsterPathInfo = m_pcsAgpmAI2->GetMonsterPathInfo(pcsAgpdCharacter))
	{
		// 여기에 Path가 있는 Monster Move를 작성한다.
	}
	else
	{
		// 부하몹이면 보스몹이 정해준 위치로 움직인다.
		if(pcsAgpdAI2ADChar->m_pcsBossCharacter != NULL)
		{
			if(lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime)
			{
				// 아무일 없음
			}
			else
			{
				float		fTempX, fTempZ;

				fTempX = pcsAgpdAI2ADChar->m_csTargetPos.x - pcsAgpdCharacter->m_stPos.x;
				fTempZ = pcsAgpdAI2ADChar->m_csTargetPos.z - pcsAgpdCharacter->m_stPos.z;

				// 오차범위 1미터라면 상관없음
				if(fTempX*fTempX + fTempZ*fTempZ <= 100.0f * 100.0f)
				{
				}
				else
				{
					m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE);
				}
			}
		}
		else
		{
			// 여기서 부터는 그냥 방랑하면서 다녀야하는 녀석들
			PROFILE("AgsmAI2::MoveWander");

			ApdEvent		*pcsEvent;
			pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsAgpdCharacter)->m_pstEvent;

			if(pcsEvent != NULL)
			{
				if(lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime)
				{
					// 아무일 없음
				}
				else
				{
					// 이동중이 아닌데 다음 패스 리스트가 없으면 길을 찾고
					if(!pcsAgpdCharacter->m_bMove)
					{
						if(!GetTargetPosFromSpawn(pcsAgpdCharacter, &pcsAgpdCharacter->m_stPos, &pcsAgpdAI2ADChar->m_csTargetPos, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius))
						{
							float			fTempX, fTempZ, fTempDistance;
							fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
							fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
							fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

							//만약 스폰범위안에 존재하지 않는다면 처음 스폰된 위치로 돌아간다.
							if(fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance)
							{
								pcsAgpdAI2ADChar->m_csTargetPos = pcsAgpdAI2ADChar->m_csFirstSpawnPos;
							}
							else
							{
								return FALSE;
							}
						}

						INT32 lRandTime = m_csRand.randInt()%15000;
						pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount + 15000 + lRandTime;

						// 이동할 곳도 정했으니 그냥 읊조려준다.
						ProcessNPCDialog((ApBase*)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_WANDERING);
						m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION);

						// 보스몹이면 자신의 부하들에게도 같은 위치를 정해주고 위치복사 및 길찾기까지 처리를 미리 해준다.
						if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 && m_pcsAgpmAI2->CheckIsBossMob(pcsAgpdAI2ADChar))
						{
							AgpdAI2ADChar	*pcsAI2ADFollowerChar;
							AgpdCharacter	*pcsAgpdFollowerCharacter;pcsAgpdFollowerCharacter;

							INT32			lCurrentFollower;
							INT32			lRealCounter;
							INT32			lDegreePerFollower;
							float			fFollowerMargin;
							float			fFormationTempX, fFormationTempZ;

							lCurrentFollower = pcsAgpdAI2ADChar->m_lCurrentFollowers;

							if(lCurrentFollower > 0)
							{
								lDegreePerFollower = 360/lCurrentFollower;
								lRealCounter = 0;
								fFollowerMargin = 300.0f + lCurrentFollower*30.0f;

								for(int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i)
								{
									pcsAgpdFollowerCharacter = pcsAgpdAI2ADChar->m_pcsFollower[i];

									if(pcsAgpdFollowerCharacter != NULL && pcsAgpdFollowerCharacter->m_Mutex.WLock())
									{
										fFormationTempX = sin(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;
										fFormationTempZ = cos(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;

										pcsAI2ADFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdFollowerCharacter);

										// 만약 이동중이면 멈춘다
										if(pcsAgpdFollowerCharacter->m_bMove)
											m_pcsAgpmCharacter->StopCharacter(pcsAgpdFollowerCharacter, NULL);

										// 목적지를 정해준다.
										pcsAI2ADFollowerChar->m_csTargetPos.x = pcsAgpdAI2ADChar->m_csTargetPos.x + fFormationTempX;
										pcsAI2ADFollowerChar->m_csTargetPos.z = pcsAgpdAI2ADChar->m_csTargetPos.z + fFormationTempZ;

										pcsAI2ADFollowerChar->m_ulNextWanderingTime = pcsAgpdAI2ADChar->m_ulNextWanderingTime;

										m_pcsAgpmCharacter->MoveCharacter(pcsAgpdFollowerCharacter, &pcsAI2ADFollowerChar->m_csTargetPos, MD_NODIRECTION);

										pcsAgpdFollowerCharacter->m_Mutex.Release();

										++lRealCounter;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	return TRUE;
}

// MonsterCommonAI (Monster공통으로 해줘야하는 AI는 이쪽에서 처리한다.)
BOOL AgsmAI2::MonsterCommonAI(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	AgpdCharacterTemplate *pcsTemplate = NULL;
	INT32 lMaxHP;
	INT32 lDecreaseScalar;

	pcsTemplate = pcsAgpdCharacter->m_pcsCharacterTemplate;
	if(NULL == pcsTemplate)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_ulNextDecSlowTime < lClockCount)
	{
		// 1/1000초 단위이므로 1000을 곱한다.
		pcsAgpdAI2ADChar->m_ulNextDecSlowTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lSlowTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lSlowPercent)/100;

		if(lDecreaseScalar == 0)
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory(pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseScalar);
	}

	if(pcsAgpdAI2ADChar->m_ulNextDecFastTime < lClockCount)
	{
		pcsAgpdAI2ADChar->m_ulNextDecFastTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lFastTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lFastPercent)/100;

		if(lDecreaseScalar == 0)
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory(pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_FAST, lDecreaseScalar);
	}

	return TRUE;
}

// Runaway중에서 반대방향으로 뛰어가는 것
BOOL AgsmAI2::RunawayFromPC(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter* pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdPCCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	BOOL bResult = FALSE;

	if(pcsAgpdAI2ADChar->m_ulStartRunawayTime == 0)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == TRUE)
	{
		return FALSE;
	}

	if(lClockCount < pcsAgpdAI2ADChar->m_ulStartRunawayTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_uIEscapeTime)
	{
		AuPOS stFrom  = pcsAgpdCharacter->m_stPos;
		AuPOS stPCPos = pcsAgpdPCCharacter->m_stPos;
		AuPOS stDest;
		FLOAT fAttackRange = 0.0f;

		m_pcsAgpmFactors->GetValue(&pcsAgpdPCCharacter->m_pcsCharacterTemplate->m_csFactor, &fAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);

		//fPreemptiveRange안에 존재하는가?
		FLOAT			fTempX, fTempZ;

		fTempX = pcsAgpdPCCharacter->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
		fTempZ = pcsAgpdPCCharacter->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

		if( fTempX*fTempX + fTempZ*fTempZ < (fAttackRange+5000.0f) * (fAttackRange+5000.0f) )
		{
			if(GetRunawayPosFromPC(pcsAgpdCharacter, &stFrom, &stPCPos, &stDest) == TRUE)
			{
				m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDest, MD_NODIRECTION, FALSE, TRUE);
			}
			else
			{
				if(GetTargetPosFromSpawn(pcsAgpdCharacter, &stFrom, &stDest, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius))
				{
					m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDest, MD_NODIRECTION, FALSE, TRUE);
				}
			}	
		}
		else
		{
			if(pcsAgpdCharacter->m_bMove)
				m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}

		bResult = TRUE;
	}
	else
	{
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

		if(pcsAgpdAI2ADChar->m_ulStartRunawayTime != 0)
			pcsAgpdAI2ADChar->m_ulStartRunawayTime = 0;

		bResult = TRUE;
	}

	return bResult;
}

// LOOTITEM
BOOL AgsmAI2::LootItem(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	INT32 nItemCount = 0;

	BOOL bRet = FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == TRUE)
	{
		return FALSE;
	}

	// LootRange가 있다는거는 요놈들이 강탈자라는 말
	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_fLootItemRange > 0)
	{
		ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_ITEM> arNearItemList;
		arNearItemList.MemSetAll();

		nItemCount = m_pcsApmMap->GetItemList(pcsAgpdCharacter->m_nDimension, pcsAgpdCharacter->m_stPos, pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_fLootItemRange,
										 &arNearItemList[0], AGSMAI2_MAX_PROCESS_ITEM);

		if(nItemCount > 0)
		{
			for(int i=0; i<nItemCount; ++i)
			{
				AgpdItem *pcsLootItem = m_pcsAgpmItem->GetItem(arNearItemList[i]);
				if(pcsLootItem)
				{
					// 그 아이템의 위치가 1M이내인지를 확인한다.
					FLOAT fTempX = pcsAgpdCharacter->m_stPos.x - pcsLootItem->m_posItem.x;
					FLOAT fTempZ = pcsAgpdCharacter->m_stPos.z - pcsLootItem->m_posItem.z;

					if(fTempX*fTempX + fTempZ*fTempZ > (FLOAT) (100 * 100))
					{
						m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsLootItem->m_posItem, MD_NODIRECTION, FALSE, TRUE);
						break;
					}

					// 이동했다면 이제 아이템을 주워준다.
					if (pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE ||
						pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK ||
						pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD)
						break;

					AgpdAI2LootItem stLootItem;

					if(pcsLootItem->m_Mutex.WLock())
					{
						stLootItem.m_ulItemID = pcsLootItem->m_lID;
						stLootItem.m_ulGetItemTime = lClockCount;
						
						pcsAgpdAI2ADChar->m_vLootItem.push_back(stLootItem);
						m_pcsAgpmItem->RemoveItemFromField(pcsLootItem, TRUE, FALSE);

						pcsLootItem->m_Mutex.Release();
					}
				}
			}

			bRet = TRUE;
		}

		CLootItemVector::iterator iter = pcsAgpdAI2ADChar->m_vLootItem.begin();

		while(iter != pcsAgpdAI2ADChar->m_vLootItem.end())
		{
			if(iter->m_ulItemID == 0) 
			{
				iter++;
				continue;
			}

			if(iter->m_ulGetItemTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_uILootItemTime < lClockCount)
			{
				AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(iter->m_ulItemID);
				if(pcsItem)
				{
					if(m_pcsAgpmItem->DestroyItem(pcsItem))
					{
						iter = pcsAgpdAI2ADChar->m_vLootItem.erase(iter);
						continue;
					}
				}
			}

			iter++;
		}
	}
	
	return bRet;
}

AgsmAI2SpecificState AgsmAI2::SpecificStateAI(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return AGSMAI2_STATE_NONE;

	INT32	lIndex;
	AgsmAI2SpecificState eResult = AGSMAI2_STATE_NONE;

	lIndex = -1;

	if(pcsAgpdPCCharacter)
	{
		lIndex = CheckUseSkill(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);
	}
	else if(m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter))
	{
		// 공성 관련 몬스터는 일단 여기서는 타겟이 없다.
		lIndex = CheckUseSkill(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);
	}

	// 스킬사용
	if(lIndex != -1)
	{
		if(pcsAgpdPCCharacter != NULL ||
		   m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter))
		{
			BOOL bUseSkill = ProcessUseSkill(pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsAgpdPCCharacter);
			if(bUseSkill)
			{
				eResult = AGSMAI2_STATE_USE_SKILL;
			}
		}
	}
	else
	{
		lIndex = CheckUseItem(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);
		// 아이템 사용
		if(lIndex != -1)
		{
			BOOL bUseItem = ProcessUseItem(pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsAgpdPCCharacter);
			if(bUseItem)
			{
				eResult = AGSMAI2_STATE_USE_ITEM;
			}
		}
		else
		{/*
			// 스크림사용
			if(pcsAgpdPCCharacter && CheckUseScream(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount))
			{
				pcsAgpdAI2ADChar->m_bScreamUsed = ProcessUseScream(pcsAgpdCharacter, pcsAgpdAI2ADChar, pcsAgpdPCCharacter);
				if(TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
				{
					pcsAgpdAI2ADChar->m_ulScreamStartTime = GetClockCount();
					eResult = AGSMAI2_STATE_USE_SCREAM;
				}
			}
			*/
		}
	}

	if(eResult == AGSMAI2_STATE_NONE)
	{
		if(ProcessTransparentMonster(pcsAgpdCharacter, lClockCount) == TRUE)
		{
			eResult = AGSMAI2_STATE_USE_TRANSPARENT;
		}
	}

	return eResult;
}

BOOL AgsmAI2::GetRunawayPosFromPC(AgpdCharacter *pcsCharacter, AuPOS *pcsMobPos, AuPOS *pcsTargetPCPos, AuPOS *pcsMobTargetPos)
{
	if(pcsCharacter == NULL || pcsMobPos == NULL || pcsTargetPCPos == NULL || pcsMobTargetPos == NULL)
		return FALSE;

	BOOL	bResult = FALSE;

	AuPOS	stDirection;
	AuPOS	stTempAuPos;
	AuPOS	stFrom   = *pcsMobPos;
	AuPOS	stTarget = *pcsTargetPCPos;
	AuPOS	stDelta = stFrom - stTarget;
		
	FLOAT	fDistance = 500.0f;

	AuMath::V3DNormalize(&stDirection, &stDelta);


	// 지형문제 때문에 Direction이 0.2미만으로 가면 부풀려준다.
	if(stDirection.x < 0.2)
	{
		if(stDirection.x > 0)
		{
			stDirection.x += 0.5;
		}
		else
		{
			stDirection.x -= 0.5;
		}
	}

	if(stDirection.z < 0.2)
	{
		if(stDirection.z > 0)
		{
			stDirection.z += 0.5;
		}
		else
		{
			stDirection.z -= 0.5;
		}
	}

	stTempAuPos.x = (stFrom.x + (stDirection.x * fDistance));
	stTempAuPos.y = (stFrom.y + (stDirection.y * fDistance));
	stTempAuPos.z = (stFrom.z + (stDirection.z * fDistance));

	*pcsMobTargetPos = stTempAuPos;
	ApmMap::BLOCKINGTYPE eType = m_pcsAgpmCharacter->GetBlockingType(pcsCharacter);

	m_pcsAgpmCharacter->GetValidDestination(&stFrom, &stTempAuPos, pcsMobTargetPos, eType);

	if (m_pcsApmMap->CheckBlockingInfo(*pcsMobTargetPos , ApmMap::GROUND ) == ApTileInfo::BLOCKGEOMETRY)
		return FALSE;

	return TRUE;
	

/*
	AuPOS	stTempAuPos;
	AuPOS	stDirection = pcsCharacter->m_stDirection;

	for (INT32 nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
	{
		// 기본 5M는 도망친다.
		FLOAT fDistance = 500.0f;

		fDistance = fDistance;

		stTempAuPos.x = (pcsMobPos->x + (stDirection.x * fDistance));
		stTempAuPos.z = (pcsMobPos->z + (stDirection.z * fDistance));

		//블럭이 없다면 굿! 앗싸아~
		if (m_pcsApmMap->CheckBlockingInfo(stTempAuPos , ApmMap::GROUND ) != ApTileInfo::BLOCKGEOMETRY)
		{
			pcsMobTargetPos->x = stTempAuPos.x;
			pcsMobTargetPos->z = stTempAuPos.z;

			bResult = TRUE;
			break;
		}
		else
		{
			AuPOS stDelta = *pcsTargetPCPos - *pcsMobPos;
			AuMath::V3DNormalize(&stDirection, &stDelta);
		}
	}
*/
	//return bResult;
}

BOOL AgsmAI2::CheckRunaway(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL) 
		return FALSE;

	if (m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter) && pcsAgpdCharacter->m_szID[0] != '\0')
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if (NULL == pcsAgpdAI2ADChar || 
		NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	// BOSS님이 계신다면 도망치지않고 끝까지 불사른다.
	if(pcsAgpdAI2ADChar->m_pcsBossCharacter)
		return FALSE;

	BOOL bResult = FALSE;

	// 도망치는 상태라면 여기서는 그냥 넘어가준다.
	if(pcsAgpdAI2ADChar->m_ulStartRunawayTime != 0)
		return FALSE;

	// 우선 RunawayAI를 사용하는 Monster인지부터 확인한다.
	if (AGPDAI2_ESCAPE_ESCAPE == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eEscape)
	{
		if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_ulEscapeCount >= pcsAgpdAI2ADChar->m_ulEscapeCount)
		{
			INT32 lMaxHP = 0;
			INT32 lCurrentHP = 0;
			INT32 lRunawayHP = 0;

			lMaxHP = m_pcsAgpmCharacter->GetMaxHP(pcsAgpdCharacter);
			lCurrentHP = m_pcsAgpmCharacter->GetHP(pcsAgpdCharacter);

			lRunawayHP = (lMaxHP*pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_lEscapeHP) / 100;

			if(lRunawayHP > lCurrentHP)
			{
				pcsAgpdAI2ADChar->m_ulStartRunawayTime = lClockCount;
				pcsAgpdAI2ADChar->m_ulEscapeCount++;
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgsmAI2::IsRunawayStateCharacter(AgpdCharacter *pcsAgpdCharacter)
{
	if(pcsAgpdCharacter == NULL)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(pcsAgpdAI2ADChar == NULL)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_ulStartRunawayTime != 0)
	{
		// 도망치기 시작한 시간이 있는 놈은 지금 도주중이다
		return TRUE;
	}

	return FALSE;
}

BOOL AgsmAI2::CheckHideMonster(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter)
		return FALSE;

	if (m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter) && pcsAgpdCharacter->m_szID[0] != '\0')
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(!pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	BOOL bResult = FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_lHideCount > pcsAgpdAI2ADChar->m_ulHideCount)
	{
		if(pcsAgpdAI2ADChar->m_ulStartHideTime == 0)
		{
			INT32 lMaxHP = 0;
			INT32 lCurrentHP = 0;
			INT32 lHideHP = 0;
			INT32 lDuration = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_uIHideMonsterTime;

			lMaxHP = m_pcsAgpmCharacter->GetMaxHP(pcsAgpdCharacter);
			lCurrentHP = m_pcsAgpmCharacter->GetHP(pcsAgpdCharacter);

			lHideHP = (lMaxHP*pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_lEscapeHP) / 100;

			if(lHideHP > lCurrentHP)
			{
				pcsAgpdAI2ADChar->m_ulStartHideTime = lClockCount;
				pcsAgpdAI2ADChar->m_ulHideCount++;

				m_pcsAgsmCharacter->SetSpecialStatusTime(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, (UINT32)lDuration);
				bResult = TRUE;
			}
		}
	}
	
	return bResult;
}

BOOL AgsmAI2::ProcessTransparentMonster(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(!pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_ulStartHideTime == 0)
		return FALSE;

	BOOL bRet = FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_lHideCount > 0)
	{
		if(pcsAgpdAI2ADChar->m_ulStartHideTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_uIHideMonsterTime < lClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);

			pcsAgpdAI2ADChar->m_ulStartHideTime = 0;
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL AgsmAI2::SetPinchMonster(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsCharacter);
	if(NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulPinchMonsterTID > 0)
	{
		if(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter != NULL)
			return TRUE;

		AuAutoLock Lock(pcsCharacter->m_Mutex);
		if(!Lock.Result())
			return FALSE;

		UINT32 lPinchMonsterTID = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulPinchMonsterTID;

		ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
		ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
		alCID.MemSetAll();
		alCID2.MemSetAll();

		INT32	lNumCID = 0;
		float	fVisibility = 10000.0f;
		
		lNumCID = m_pcsApmMap->GetCharList(pcsCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsCharacter->m_stPos, fVisibility,
										   &alCID[0], AGSMAI2_MAX_PROCESS_TARGET,
										   &alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

		for(int i=0; i<lNumCID; ++i)
		{
			AgpdCharacter *pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(alCID2[i]);

			if(NULL == pcsListCharacter || NULL == pcsListCharacter->m_pcsCharacterTemplate)
				continue;

			if(pcsListCharacter->m_bIsReadyRemove)
				continue;

			if (pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD ||
				pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				continue;

			AgpdAI2ADChar *pcsAI2ADCharacter = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);
			if(NULL == pcsAI2ADCharacter)
				continue;

			// 자기자신은 뺀다.
			if(pcsListCharacter->m_lID == pcsCharacter->m_lID)
				continue;

			if(pcsListCharacter->m_pcsCharacterTemplate->m_lID == lPinchMonsterTID)
			{
				AuAutoLock Lock(pcsListCharacter->m_Mutex);
				if(!Lock.Result())
					continue;

				pcsAgpdAI2ADChar->m_csPinchInfo.ePinchType	= AGPMAI2_TYPE_PINCH_WANTED;
				pcsAI2ADCharacter->m_csPinchInfo.ePinchType = AGPMAI2_TYPE_PINCH_MONSTER;
				pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter = pcsListCharacter;
				pcsAI2ADCharacter->m_csPinchInfo.pRequestMobVector.push_back(pcsCharacter);
			}
		}
	}

	return TRUE;
}