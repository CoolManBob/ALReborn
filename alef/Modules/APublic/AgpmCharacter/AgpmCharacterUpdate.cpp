#include "AgpmCharacter.h"

//		UpdateInit
//	Functions
//		- Character의 각종 Factor들을 초기화 하고 EnumCallback한다.
//	Arguments
//		- pCharacter : character data pointer
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::UpdateInit(AgpdCharacter *pcsCharacter)
{
	PROFILE("AgpmCharacter::UpdateInit");

	//@{ Jaewon 20051020
	//STOPWATCH("[UpdateInit]\n");
	//@} Jaewon

	if (!pcsCharacter)
		return FALSE;

	pcsCharacter->m_bIsReadyRemove	= FALSE;		// 2007.03.05. steeple

	//InitFactor(pcsCharacter);

	//@{ Jaewon 20051020
	//STOPWATCH_START;
	//@} Jaewon

	EnumCallback(AGPMCHAR_CB_ID_INIT, pcsCharacter, pcsCharacter->m_pcsCharacterTemplate);

	//@{ Jaewon 20051020
	//STOPWATCH_STOP("(1)");
	//@} Jaewon
	
	if (m_pcsApmMap && pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
	{
		return AddCharacterToMap(pcsCharacter);
	}

	return TRUE;
}

//		UpdateFactor
//	Functions
//		- Character의 Status를 수정한다.
//			* 열라 주의할 점 : Mutex를 Unlock 하는 시점이 매우 중요하다.
//							   과연, Callback 하기 전에 Unlock 할 것인가, 아님 그 후에 할 것인가?
//	Arguments
//		- lCID			: Character ID
//			or
//		- szGameID		: Game ID
//		- nNewStatus	: 새로운 상태
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::UpdateFactor(INT32 lCID)
{
	AgpdCharacter *pCharacter = GetCharacter(lCID);

	return UpdateFactor(pCharacter);
}

BOOL AgpmCharacter::UpdateFactor(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	// 변경된 사항에 대해 callback function을 호출한다.
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pCharacter, NULL);

	return TRUE;
}

//		UpdatePosition
//	Functions
//		- Character의 Position 수정한다.
//	Arguments
//		- lCID			: Character ID
//			or
//		- szGameID		: Game ID
//		- pstPos		: 새로운 위치
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::UpdatePosition(INT32 lCID, AuPOS *pstPos, BOOL bCheckBlock, BOOL bSync)
{
	AgpdCharacter *pCharacter = GetCharacter(lCID);

	return UpdatePosition(pCharacter, pstPos, bCheckBlock, bSync);
 }

BOOL AgpmCharacter::UpdatePosition(AgpdCharacter *pCharacter, AuPOS *pstPos, BOOL bCheckBlock, BOOL bSync)
{
	PROFILE("AgpmCharacter::UpdatePosition()");

	if (!pCharacter)
		return FALSE;

	/*
	fprintf(stdout, "prev position : %f, %f, %f,  current position : %f, %f, %f\n",
		pCharacter->m_stPos.x, pCharacter->m_stPos.y, pCharacter->m_stPos.z,
		pstPos->x, pstPos->y, pstPos->z);
	fflush(stdout);
	*/

	pCharacter->m_bSync = bSync;

	// 맵에 추가가 안되어 있다면 걍 캐릭터 m_stPos 값만 바꾼다.
	if (!pCharacter->m_bIsAddMap)
	{
		if (pstPos)
			pCharacter->m_stPos = *pstPos;

		return TRUE;
	}

	// Update 되기 전에 Check 한다.
	// 마고자 (2003-12-18 오전 11:37:07) : 블러킹 체크 여기서..?..

	if (bCheckBlock)
	{
		PROFILE("AgpmCharacter::UpdateInit CheckBlocking");

		BOOL	bMapBlocking			= FALSE;
		BOOL	bUpdatePositionCallback = EnumCallback(AGPMCHAR_CB_ID_UPDATE_POSITION_CHECK, pCharacter, (PVOID) pstPos);

		ApmMap::BLOCKINGTYPE eType = GetBlockingType( pCharacter );

		// 지형 Blocking 일때만, 못가게 한다. Object Blocking은 길찾기에 쓰려고 만든거다. Parn

		if (pstPos && (m_pcsApmMap->CheckBlockingInfo( *pstPos , eType ) & pCharacter->m_ulCheckBlocking))
			bMapBlocking = TRUE;

		if (bMapBlocking || !bUpdatePositionCallback)
		{
			return FALSE;
		}

		if (pstPos )
		{
			if( IsPC( pCharacter ) )
			{
				m_pcsApmMap->GetValidDestination(&pCharacter->m_stPos, pstPos, pstPos , pCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
				GetValidDestination(&pCharacter->m_stPos, pstPos, pstPos , eType , pCharacter );
			}
		}
		else
		{
			// 블러킹 없음.
		}
	}

	BOOL	bUpdatePosition	= FALSE;

	if (m_pcsApmMap)
	{
		PROFILE("AgpmCharacter::ApmMap->UpdateChar");
		
		if (IsMonster(pCharacter))
		{
			bUpdatePosition	= m_pcsApmMap->UpdateChar( pCharacter->m_nDimension , APMMAP_CHAR_TYPE_MONSTER, pCharacter->m_stPos, pstPos ? *pstPos : pCharacter->m_stPos, (INT_PTR) pCharacter, pCharacter->m_lID);
			//bUpdatePosition	= m_pcsApmMap->UpdateMonster(pCharacter->m_stPos, pstPos ? *pstPos : pCharacter->m_stPos, (INT32) pCharacter);
		}
		else if (IsPC(pCharacter) || IsCreature(pCharacter))
		{
			bUpdatePosition	= m_pcsApmMap->UpdateChar( pCharacter->m_nDimension , APMMAP_CHAR_TYPE_PC, pCharacter->m_stPos, pstPos ? *pstPos : pCharacter->m_stPos, (INT_PTR) pCharacter, pCharacter->m_lID);
			//bUpdatePosition	= m_pcsApmMap->UpdateChar(pCharacter->m_stPos, pstPos ? *pstPos : pCharacter->m_stPos, (INT32) pCharacter);
		}
		else
		{
			bUpdatePosition	= m_pcsApmMap->UpdateChar( pCharacter->m_nDimension , APMMAP_CHAR_TYPE_NPC, pCharacter->m_stPos, pstPos ? *pstPos : pCharacter->m_stPos, (INT_PTR) pCharacter, pCharacter->m_lID);
			//bUpdatePosition	= m_pcsApmMap->UpdateNPC(pCharacter->m_stPos, pstPos ? *pstPos : pCharacter->m_stPos, (INT32) pCharacter);
		}
	}

	if (bUpdatePosition)
	{
		AuPOS	stPrevPos = pCharacter->m_stPos;

		if(pstPos)
			pCharacter->m_stPos = *pstPos;

		// 2008.04.08. steeple
		// Update Position 과 AOI Filter 의 순서 때문에, 이렇게 Region 바뀌기 전에 한번 불러준다.
		EnumCallback(AGPMCHAR_CB_ID_PRE_UPDATE_POSITION, pCharacter, (PVOID)&stPrevPos);

		if (pstPos)
		{
			PROFILE("AgpmCharacter::Process Region");

			if (IsPC(pCharacter))
			{
				// 마고자 (2004-09-17 오후 5:21:09) : 바인딩 리전 조사..
				if (bSync || pCharacter->m_ulNextRegionRefreshTime < GetClockCount())
				{
					INT16	nBindingRegion = m_pcsApmMap->GetRegion( pCharacter->m_stPos.x , pCharacter->m_stPos.z );

					pCharacter->m_ulNextRegionRefreshTime	= GetClockCount() + GetRegionRefreshInterval();

					if( nBindingRegion >= 0 && pCharacter->m_nBindingRegionIndex != nBindingRegion )
					{
						// 리젼이 변한다..
						// 콜백호출..

						INT16	nPrevRegion	= pCharacter->m_nBindingRegionIndex;
						pCharacter->m_nBeforeRegionIndex = nPrevRegion;
						pCharacter->m_nBindingRegionIndex = nBindingRegion;
						EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pCharacter, &nPrevRegion );
					}
				}
			}
		}

		{
			PROFILE("AgpmCharacter::UpdatePositionCallback");

			// 변경된 사항에 대해 callback function을 호출한다.
			EnumCallback(AGPMCHAR_CB_ID_UPDATE_POSITION, pCharacter, (PVOID) &stPrevPos);
		}
	}

	pCharacter->m_bSync = FALSE;

	return TRUE;
}

BOOL AgpmCharacter::UpdateRegion(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return TRUE;

	BOOL	bIsStaticCharacter	= FALSE;
	EnumCallback(AGPMCHAR_CB_ID_IS_STATIC_CHARACTER, pCharacter, &bIsStaticCharacter);

	if (IsPC(pCharacter) || bIsStaticCharacter || IsStatusSummoner(pCharacter))
	{
		// 마고자 (2004-09-17 오후 5:21:09) : 바인딩 리전 조사..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pCharacter->m_stPos.x , pCharacter->m_stPos.z );

		if (IsPC(pCharacter))
			pCharacter->m_ulNextRegionRefreshTime	= GetClockCount() + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// 리젼이 변한다..
			// 콜백호출..
			INT16	nPrevRegion	= pCharacter->m_nBindingRegionIndex;

			pCharacter->m_nBeforeRegionIndex = nPrevRegion;

			pCharacter->m_nBindingRegionIndex = nBindingRegion;

			if (IsPC(pCharacter) || IsStatusSummoner(pCharacter))
				EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pCharacter, &nPrevRegion );
		}
	}

	return TRUE;
}

//		UpdateStatus
//	Functions
//		- Character의 Status를 수정한다.
//			* 열라 주의할 점 : Mutex를 Unlock 하는 시점이 매우 중요하다.
//							   과연, Callback 하기 전에 Unlock 할 것인가, 아님 그 후에 할 것인가?
//	Arguments
//		- lCID			: Character ID
//			or
//		- szGameID		: Game ID
//		- nNewStatus	: 새로운 상태
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::UpdateStatus(INT32 lCID, UINT16 nNewStatus)
{
	AgpdCharacter *pCharacter = GetCharacter(lCID);

	return UpdateStatus(pCharacter, nNewStatus);
}

BOOL AgpmCharacter::UpdateStatus(AgpdCharacter *pCharacter, UINT16 nNewStatus)
{
	PROFILE("AgpmCharacter::UpdateStatus");

	if (!pCharacter)
		return FALSE;

	INT16	nOldStatus = pCharacter->m_unCurrentStatus;
	pCharacter->m_unCurrentStatus = nNewStatus;

	// 변경된 사항에 대해 callback function을 호출한다.
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_STATUS, pCharacter, &nOldStatus);

	// 캐릭터의 현재 상태가 select character 에서 다른것으로 변경된 경우 (즉, 게임안으로 실제 들어온 경우이다) 맵에 추가해준다.
	if (m_pcsApmMap && nOldStatus != AGPDCHAR_STATUS_IN_GAME_WORLD && nNewStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
	{
		AddCharacterToMap(pCharacter);
		UpdatePosition(pCharacter, NULL, FALSE, TRUE);
	}

	return TRUE;
}

BOOL AgpmCharacter::UpdateActionStatus(INT32 lCID, UINT16 nNewStatus, BOOL bCheck)
{
	AgpdCharacter *pCharacter = GetCharacter(lCID);

	return UpdateActionStatus(pCharacter, nNewStatus, bCheck);
}

BOOL AgpmCharacter::UpdateActionStatus(AgpdCharacter *pCharacter, UINT16 nNewStatus, BOOL bCheck)
{
	PROFILE("AgpmCharacter::UpdateActionStatus");

	if (!pCharacter)
		return FALSE;

	BOOL	bIsUpdateActionStatus	= TRUE;

	if (bCheck)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) nNewStatus;
		pvBuffer[1] = (PVOID) &bIsUpdateActionStatus;

		EnumCallback(AGPMCHAR_CB_ID_UPDATE_ACTION_STATUS_CHECK, pCharacter, pvBuffer);
	}

	if (!bIsUpdateActionStatus)
		return TRUE;

	INT16	nOldStatus = pCharacter->m_unActionStatus;
	pCharacter->m_unActionStatus = (INT8) nNewStatus;

	// 변경된 사항에 대해 callback function을 호출한다.
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_ACTION_STATUS, pCharacter, &nOldStatus);

	// 죽었다면 attack list를 초기화한다.
	if (pCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD && nOldStatus != AGPDCHAR_STATUS_DEAD)
		ClearAttackerToList(pCharacter);

	return TRUE;
}

// 2004.08.25. steeple
// Special Status 를 대입한다.
BOOL AgpmCharacter::UpdateSpecialStatus(INT32 lCID, UINT64 ulStatus, INT32 lReserved)
{
	return UpdateSpecialStatus(GetCharacter(lCID), ulStatus, lReserved);
}

// 2004.08.25. steeple
// Special Status 를 대입한다.
BOOL AgpmCharacter::UpdateSpecialStatus(AgpdCharacter* pCharacter, UINT64 ulStatus, INT32 lReserved)
{
	if(!pCharacter)
		return FALSE;

	UINT64 ulOldStatus = pCharacter->m_ulSpecialStatus;
	INT32  nReserved   = lReserved;

	PVOID pvStatusParam[2];

	pCharacter->m_ulSpecialStatus = ulStatus;

	pvStatusParam[0] = &ulOldStatus;
	pvStatusParam[1] = &nReserved;

	// Callback 호출
	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_SPECIAL_STATUS, pCharacter, &pvStatusParam);
}

// 2004.08.25. steeple
// Special Status 를 세팅한다.
BOOL AgpmCharacter::UpdateSetSpecialStatus(INT32 lCID, UINT64 ulSetStatus, INT32 lReserved)
{
	return UpdateSetSpecialStatus(GetCharacter(lCID), ulSetStatus, lReserved);
}

// 2004.08.25. steeple
// Special Status 를 세팅한다.
BOOL AgpmCharacter::UpdateSetSpecialStatus(AgpdCharacter* pCharacter, UINT64 ulSetStatus, INT32 lReserved)
{
	if(!pCharacter)
		return FALSE;

	UINT64 ulOldStatus = pCharacter->m_ulSpecialStatus;
	INT32  nReserved   = lReserved;

	PVOID pvStatusParam[2];

	pCharacter->m_ulSpecialStatus |= ulSetStatus;

	pvStatusParam[0] = &ulOldStatus;
	pvStatusParam[1] = &nReserved;

	// Callback 호출
	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_SPECIAL_STATUS, pCharacter, pvStatusParam);
}

// 2004.08.25. steeple
// 걸려있는 Special Status 를 해제한다.
BOOL AgpmCharacter::UpdateUnsetSpecialStatus(INT32 lCID, UINT64 ulUnsetStatus, INT32 lReserved)
{
	return UpdateUnsetSpecialStatus(GetCharacter(lCID), ulUnsetStatus);
}

// 2004.08.25. steeple
// 걸려있는 Special Status 를 해제한다.
BOOL AgpmCharacter::UpdateUnsetSpecialStatus(AgpdCharacter* pCharacter, UINT64 ulUnsetStatus, INT32 lReserved)
{
	if(!pCharacter)
		return FALSE;

	UINT64 ulOldStatus = pCharacter->m_ulSpecialStatus;
	INT32  nReserved   = lReserved;

	PVOID pvStatusParam[2];

	pCharacter->m_ulSpecialStatus &= ~ulUnsetStatus;

	pvStatusParam[0] = (PVOID)&ulOldStatus;
	pvStatusParam[1] = (PVOID)&nReserved;

	// Callback 호출
	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_SPECIAL_STATUS, pCharacter, &pvStatusParam);
}

BOOL AgpmCharacter::UpdateCriminalStatus(INT32 lCID, AgpdCharacterCriminalStatus eNewStatus)
{
	return UpdateCriminalStatus(GetCharacter(lCID), eNewStatus);
}

BOOL AgpmCharacter::UpdateCriminalStatus(AgpdCharacter *pcsCharacter, AgpdCharacterCriminalStatus eNewStatus)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacterCriminalStatus	eOldStatus = (AgpdCharacterCriminalStatus) pcsCharacter->m_unCriminalStatus;

	pcsCharacter->m_unCriminalStatus = (INT8) eNewStatus;

	// 변경된 사항에 대해 callback function을 호출한다.
	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_CRIMINAL_STATUS, pcsCharacter, &eOldStatus);
}

BOOL AgpmCharacter::UpdateMurdererPoint(AgpdCharacter *pcsCharacter, INT32 lMurdererPoint)
{
	if (!pcsCharacter || lMurdererPoint < 0 || !m_pcsAgpmFactors)
		return FALSE;

	INT32 lDiff = lMurdererPoint - GetMurdererPoint(pcsCharacter);

	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_MURDERER_POINT, pcsCharacter, &lDiff);

	if (!IsMurderer(pcsCharacter))
		ClearAttackerToList(pcsCharacter);

	return TRUE;
}

BOOL AgpmCharacter::AddMurdererPoint(AgpdCharacter *pcsCharacter, INT32 lAdd)
{
	if (!pcsCharacter || 0 >= lAdd || !m_pcsAgpmFactors)
		return FALSE;

	if(GetMurdererPoint(pcsCharacter) + lAdd >= AGPMCHARACTER_MAX_MURDERER_POINT)
		return TRUE;

	INT32 lMurderPoint = GetMurdererPoint(pcsCharacter) + lAdd;

	return UpdateMurdererPoint(pcsCharacter, lMurderPoint);
}

BOOL AgpmCharacter::SubMurdererPoint(AgpdCharacter *pcsCharacter, INT32 lSub)
{
	if (!pcsCharacter || 0 >= lSub || !m_pcsAgpmFactors)
		return FALSE;

	INT32 lMurderPoint = GetMurdererPoint(pcsCharacter) - lSub;
	
	if (0 > lMurderPoint)
		lMurderPoint = 0;

	return UpdateMurdererPoint(pcsCharacter, lMurderPoint);
}

BOOL AgpmCharacter::UpdateMukzaPoint(AgpdCharacter *pcsCharacter, INT32 lMukzaPoint)
{
	if (!pcsCharacter || lMukzaPoint < 0 || !m_pcsAgpmFactors)
		return FALSE;

	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lMukzaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lMukzaPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_MUKZA_POINT, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::UpdateCharismaPoint(AgpdCharacter *pcsCharacter, INT32 lCharismaPoint)
{
	if (!pcsCharacter)
		return FALSE;

	if (0 > lCharismaPoint)
		lCharismaPoint = 0;

	if (lCharismaPoint > AGPMCHARACTER_MAX_CHARISMA_POINT)
		lCharismaPoint = AGPMCHARACTER_MAX_CHARISMA_POINT;

	INT32 lDiff = lCharismaPoint - GetCharismaPoint(pcsCharacter);

	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_CHARISMA_POINT, pcsCharacter, &lDiff);
	
	return TRUE;
}

BOOL AgpmCharacter::AddCharismaPoint(AgpdCharacter *pcsCharacter, INT32 lAdd)
{
	if (!pcsCharacter || 0 >= lAdd || !m_pcsAgpmFactors)
		return FALSE;

	INT32 lPoint = GetCharismaPoint(pcsCharacter) + lAdd;

	return UpdateCharismaPoint(pcsCharacter, lPoint);
}

BOOL AgpmCharacter::SubCharismaPoint(AgpdCharacter *pcsCharacter, INT32 lSub)
{
	if (!pcsCharacter || 0 >= lSub || !m_pcsAgpmFactors)
		return FALSE;

	INT32 lPoint = GetCharismaPoint(pcsCharacter) - lSub;
	if (0 > lPoint)
		lPoint = 0;

	return UpdateCharismaPoint(pcsCharacter, lPoint);
}

BOOL AgpmCharacter::UpdateLevel(INT32 lCID, INT32 lLevelChange)
{
	return UpdateLevel(GetCharacter(lCID), lLevelChange);
}

BOOL AgpmCharacter::UpdateLevel(AgpdCharacter *pcsCharacter, INT32 lLevelChange)
{
	if (!pcsCharacter)
		return FALSE;

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_LEVEL, pcsCharacter, &lLevelChange);
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_LEVEL_POST, pcsCharacter, &lLevelChange);
	return  TRUE;
}

BOOL AgpmCharacter::ChangedLevel(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_LEVEL, pCharacter, NULL);
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_LEVEL_POST, pCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::UpdateSkillPoint(INT32 lCID, INT32 lPoint)
{
	return UpdateSkillPoint(GetCharacter(lCID), lPoint);
}

BOOL AgpmCharacter::UpdateSkillPoint(AgpdCharacter *pCharacter, INT32 lPoint)
{
	if (!pCharacter || lPoint == 0)
		return FALSE;

	INT32	lCurrentSkillPoint	= 0;
	m_pcsAgpmFactors->GetValue(&pCharacter->m_csFactor, &lCurrentSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);

	m_pcsAgpmFactors->SetValue(&pCharacter->m_csFactor, lCurrentSkillPoint + lPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);

	/*
	AgpdFactor	csUpdateFactor;
	m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

	AgpdFactorDIRT	*pcsFactorDIRT = 
		(AgpdFactorDIRT *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DIRT);
	if (!pcsFactorDIRT) return FALSE;

	pcsFactorDIRT->lValue[AGPD_FACTORS_DIRT_TYPE_SKILL_POINT] = lPoint;

	m_pcsAgpmFactors->CalcFactor(&pCharacter->m_csFactor, &csUpdateFactor, TRUE, FALSE);

	m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
	*/

	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_SKILL_POINT, pCharacter, &lPoint);
}

BOOL AgpmCharacter::UpdateHeroicPoint(INT32 lCID, INT32 lPoint)
{
	return UpdateHeroicPoint(GetCharacter(lCID), lPoint);
}

BOOL AgpmCharacter::UpdateHeroicPoint(AgpdCharacter *pcsCharacter, INT32 lPoint)
{
	if(NULL == pcsCharacter || lPoint == 0)
		return FALSE;

	INT32 lCurrentHeroicPoint = 0;

	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHeroicPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);
	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentHeroicPoint + lPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);

	INT32 lSkillPoint = 0;

	// HeroicPoint 변화에 따른 후속 처리를 해줘야함. 
	// RecalcResultFactor, CastHeroicPassiveSkill
	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_SKILL_POINT, pcsCharacter, &lSkillPoint);
}


BOOL AgpmCharacter::UpdateHeroicPoint(INT32 lCID, INT32 lNewPoint, BOOL bFullUpdate)
{
	return UpdateHeroicPoint(GetCharacter(lCID), lNewPoint, bFullUpdate);
}

BOOL AgpmCharacter::UpdateHeroicPoint(AgpdCharacter *pcsCharacter, INT32 lNewPoint, BOOL bFullUpdate)
{
	if(NULL == pcsCharacter || lNewPoint < 0)
		return FALSE;

	if(bFullUpdate)
	{
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lNewPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);
	}
	else
	{
		INT32 lCurrentHeroicPoint = 0;

		m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHeroicPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentHeroicPoint + lNewPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);
	}

	INT32 lSkillPoint = 0;

	// HeroicPoint 변화에 따른 후속 처리를 해줘야함. 
	// RecalcResultFactor, CastHeroicPassiveSkill
	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_SKILL_POINT, pcsCharacter, &lSkillPoint);
}

BOOL AgpmCharacter::UpdateMoney(INT32 lCID)
{
	AgpdCharacter *pCharacter = GetCharacter(lCID);

	return UpdateMoney(pCharacter);
}

BOOL AgpmCharacter::UpdateMoney(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	// 변경된 사항에 대해 callback function을 호출한다.
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_MONEY, pCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::UpdateCustomizeIndex(AgpdCharacter *pcsCharacter, INT32 lNewFaceIndex, INT32 lNewHairIndex)
{
	if (!pcsCharacter || (lNewFaceIndex < 0 && lNewHairIndex < 0))
		return FALSE;

	if (pcsCharacter->m_lFaceIndex != lNewFaceIndex &&
		lNewFaceIndex >= 0)
		pcsCharacter->m_lFaceIndex	= lNewFaceIndex;

	if (pcsCharacter->m_lHairIndex != lNewHairIndex &&
		lNewHairIndex >= 0)
		pcsCharacter->m_lHairIndex	= lNewHairIndex;

	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_CUSTOMIZE, pcsCharacter, NULL);
}

BOOL AgpmCharacter::UpdateBankSize(AgpdCharacter *pcsCharacter, INT8 cBankSize)
{
	if (!pcsCharacter || cBankSize < 0)
		return FALSE;

	INT8	cPrevBankSize	= pcsCharacter->m_cBankSize;

	pcsCharacter->m_cBankSize	= cBankSize;

	return EnumCallback(AGPMCHAR_CB_ID_UPDATE_BANK_SIZE, pcsCharacter, &cPrevBankSize);
}

BOOL AgpmCharacter::UpdateEventStatusFlag(AgpdCharacter *pcsCharacter, UINT16 unEventStatusFlag)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unEventStatusFlag != unEventStatusFlag)
	{
		pcsCharacter->m_unEventStatusFlag	= unEventStatusFlag;

		EnumCallback(AGPMCHAR_CB_ID_UPDATE_EVENTSTATUSFLAG, pcsCharacter, NULL);
	}

	return TRUE;
}

INT32	AgpmCharacter::GetLevelOriginal(AgpdCharacter *pcsCharacter)
{
	ASSERT( pcsCharacter );

	if(!pcsCharacter)
		return FALSE;

	if( pcsCharacter->GetSpecialState( AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT ) )
		return GetLevelBefore( pcsCharacter );
	else
		return GetLevel( pcsCharacter );
}

BOOL	AgpmCharacter::SetCharacterLevelLimit		( AgpdCharacter * pcsCharacter , INT32 nLevel )
{
	ASSERT( pcsCharacter );
	if(!pcsCharacter)
		return FALSE;

	if( pcsCharacter->GetSpecialState( AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT ) )
	{
		INT32	nCurrentLevelLimit	= GetLevelBefore( pcsCharacter );
		
		if( nCurrentLevelLimit == nLevel ) return TRUE;

		ReleaseCharacterLevelLimit( pcsCharacter );
	}

	this->UpdateSetSpecialStatus( pcsCharacter , AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT );

	return TRUE;
}

BOOL	AgpmCharacter::ReleaseCharacterLevelLimit	( AgpdCharacter * pcsCharacter )
{
	ASSERT( pcsCharacter );
	if(!pcsCharacter)
		return FALSE;

	if( pcsCharacter->GetSpecialState( AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT ) )
	{
		this->UpdateUnsetSpecialStatus( pcsCharacter , AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT );
		// 초기화시킴..
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, 0, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_BEFORELEVEL);
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, 0, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LIMITEDLEVEL);
		return TRUE;
	}
	else return FALSE;
}

AgpmCharacter::LevelLimitState	AgpmCharacter::GetCharacterLevelLimit	( AgpdCharacter * pcsCharacter )
{
	if( pcsCharacter->GetSpecialState( AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT ) )
	{
		INT32	nLevelOriginal	= m_pcsAgpmFactors->GetLevelBefore	( &pcsCharacter->m_csFactor );
		INT32	nLevel			= m_pcsAgpmFactors->GetLevel		( &pcsCharacter->m_csFactor );
		INT32	nLevelLimit		= m_pcsAgpmFactors->GetLevelLimited	( &pcsCharacter->m_csFactor );

		INT32	nLevelCurrent	= ( nLevel > nLevelOriginal ) ? nLevel : nLevelOriginal;

		if( nLevelCurrent > nLevelLimit )
			return AgpmCharacter::LLS_DOWNED;		// 레벨 제한걸림 .. 레벨이 다운되어 있음.
		else if( nLevelLimit > nLevelCurrent )
			return AgpmCharacter::LLS_LIMITED;	// 레벨 제한걸림.. 걸린 레벨보다 낮음. 경험치&레벨업 정상획득
		else
			return AgpmCharacter::LLS_BLOCKED;	// 레벨 제한 걸림.. 경험치는 얻으나 레벨은 못올라감.
	}
	else	return AgpmCharacter::LLS_NONE;
}
