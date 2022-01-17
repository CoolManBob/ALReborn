// ApmEventCondition.cpp: implementation of the ApmEventManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ApmEventManager.h"
#include <math.h>
#include "AuMath.h"
#include "AuRandomNumber.h"
#include "ApMemoryTracker.h"

BOOL	ApmEventManager::SetCondition(ApdEvent *pstEvent, ApdEventCondFlag eFlags)
{
	if (!pstEvent)
		return FALSE;

	if (!pstEvent->m_pstCondition)
	{
		pstEvent->m_pstCondition = new ApdEventCondition;
	}

	if (eFlags & APDEVENT_COND_TARGET)
	{
		pstEvent->m_pstCondition->m_pstTarget = new ApdEventConditionTarget;
	}

	if (eFlags & APDEVENT_COND_AREA)
	{
		pstEvent->m_pstCondition->m_pstArea = new ApdEventConditionArea;
	}

	if (eFlags & APDEVENT_COND_ENVIRONMENT)
	{
		pstEvent->m_pstCondition->m_pstEnvironment = new ApdEventConditionEnv;
	}

	if (eFlags & APDEVENT_COND_TIME)
	{
		pstEvent->m_pstCondition->m_pstTime = new ApdEventConditionTime;
	}

	return TRUE;
}

void	ApmEventManager::DestroyCondition(ApdEvent *pstEvent)
{
	if (!pstEvent || !pstEvent->m_pstCondition)
		return;

	if (pstEvent->m_pstCondition->m_pstTarget)
		delete pstEvent->m_pstCondition->m_pstTarget;

	if (pstEvent->m_pstCondition->m_pstArea)
		delete pstEvent->m_pstCondition->m_pstArea;

	if (pstEvent->m_pstCondition->m_pstEnvironment)
		delete pstEvent->m_pstCondition->m_pstEnvironment;

	if (pstEvent->m_pstCondition->m_pstTime)
		delete pstEvent->m_pstCondition->m_pstTime;

	delete pstEvent->m_pstCondition;
}

BOOL	ApmEventManager::CopyCondition(ApdEvent *pstDstEvent, ApdEvent *pstSrcEvent)
{
	if (!pstSrcEvent->m_pstCondition)
		return TRUE;

	INT32	eFlags;

	eFlags = APDEVENT_COND_NONE;

	if (pstSrcEvent->m_pstCondition->m_pstTarget)
		eFlags |= APDEVENT_COND_TARGET;

	if (pstSrcEvent->m_pstCondition->m_pstArea)
		eFlags |= APDEVENT_COND_AREA;

	if (pstSrcEvent->m_pstCondition->m_pstEnvironment)
		eFlags |= APDEVENT_COND_ENVIRONMENT;

	if (pstSrcEvent->m_pstCondition->m_pstTime)
		eFlags |= APDEVENT_COND_TIME;

	DestroyCondition(pstDstEvent);

	SetCondition(pstDstEvent, (ApdEventCondFlag) eFlags);

	if (eFlags & APDEVENT_COND_TARGET)
		memcpy(pstDstEvent->m_pstCondition->m_pstTarget, pstSrcEvent->m_pstCondition->m_pstTarget, sizeof(ApdEventConditionTarget));

	if (eFlags & APDEVENT_COND_AREA)
		memcpy(pstDstEvent->m_pstCondition->m_pstArea, pstSrcEvent->m_pstCondition->m_pstArea, sizeof(ApdEventConditionArea));

	if (eFlags & APDEVENT_COND_ENVIRONMENT)
		memcpy(pstDstEvent->m_pstCondition->m_pstEnvironment, pstSrcEvent->m_pstCondition->m_pstEnvironment, sizeof(ApdEventConditionEnv));

	if (eFlags & APDEVENT_COND_TIME)
		memcpy(pstDstEvent->m_pstCondition->m_pstTime, pstSrcEvent->m_pstCondition->m_pstTime, sizeof(ApdEventConditionTime));

	return TRUE;
}

BOOL	ApmEventManager::GetRandomPos(ApdEvent *pstEvent, AuPOS *pstPos)
{
	if (!pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea || !pstPos)
		return FALSE;

	AuMATRIX *	pstDirection;
	INT32		nTry;

	*pstPos = *GetBasePos(pstEvent->m_pcsSource, &pstDirection);
	if (!pstPos)
		return FALSE;

	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			//FLOAT	fAngle;
			//FLOAT	fRadius;
			FLOAT	fX;
			FLOAT	fZ;
			AuPOS	stTempAuPos;

			for (nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
			{
				fX = FLOAT(m_csRandom.rand(pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius * 2) - pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius);
				fZ = FLOAT(m_csRandom.rand(pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius * 2) - pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius);

				if (fX * fX + fZ * fZ > pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius * pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius)
				{
					continue;
				}

				stTempAuPos.x = pstPos->x + fX;
				stTempAuPos.z = pstPos->z + fZ;

				/*
				fAngle = csRand.rand(2 * AUMATH_PI);
				fRadius = csRand.rand(pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius);

				pstPos->x += cos(fAngle) * fRadius;
				pstPos->z += sin(fAngle) * fRadius;
				*/

				//pstPos->y = m_pcsApmMap->GetHeight(pstPos->x, pstPos->z);

				if (!(m_pcsApmMap->CheckBlockingInfo( stTempAuPos , ApmMap::GROUND ) & ApTileInfo::BLOCKGEOMETRY))
				{
					pstPos->x = stTempAuPos.x;
					pstPos->z = stTempAuPos.z;

					break;
				}
			}
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			FLOAT	fAngle;
			FLOAT	fRadius;

			for (nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
			{
				fAngle = cosf(pstDirection->at.x) + asinf((FLOAT)m_csRandom.rand(2 * pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2) / 2.0f) * 2.0f;
				fRadius = (FLOAT)m_csRandom.rand(pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius);

				pstPos->x += cos(fAngle) * fRadius;
				pstPos->z += sin(fAngle) * fRadius;

				if (!m_pcsApmMap)
					break;

				//pstPos->y = m_pcsApmMap->GetHeight(pstPos->x, pstPos->z);

				if (!(m_pcsApmMap->CheckBlockingInfo(*pstPos , ApmMap::GROUND ) & ApTileInfo::BLOCKGEOMETRY))
					break;
			}
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			AuPOS	posTemp1;
			AuPOS	posTemp2;

			for (nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
			{
				posTemp1.x = pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.x + 
					(FLOAT)m_csRandom.rand(pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.x - pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.x);
				posTemp1.y = 0;
				posTemp1.z = pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.z + 
					(FLOAT)m_csRandom.rand(pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.z - pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.z);

				AuMath::V3DTransform(&posTemp2, &posTemp1, pstDirection);
				AuMath::V3DAdd(pstPos, pstPos, &posTemp2);

				if (!m_pcsApmMap)
					break;

				//pstPos->y = m_pcsApmMap->GetHeight(pstPos->x, pstPos->z);

				if (!(m_pcsApmMap->CheckBlockingInfo(*pstPos , ApmMap::GROUND ) & ApTileInfo::BLOCKGEOMETRY))
					break;
			}
		}
		break;

	default:
		return FALSE;
	}

	if (nTry >= APMEVENT_CONDITION_MAX_TRY)
		return FALSE;

	return TRUE;
}

BOOL ApmEventManager::GetRandomPos4MobWander(ApdEvent *pstEvent, AuPOS *pstObjCurrentPos, AuPOS *pstPos)
{
	PROFILE("ApmEventManager::GetRandomPos4MobWander");

	BOOL				bResult;

	bResult = FALSE;

	if (!m_pcsApmMap)
		return bResult;

	if (!pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea || !pstPos || !pstObjCurrentPos )
		return bResult;

	AuMATRIX *	pstDirection;
	INT32		nTry;

	*pstPos = *GetBasePos(pstEvent->m_pcsSource, &pstDirection);

	if (!pstPos)
		return bResult;

	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
		case APDEVENT_AREA_SPHERE:
		{
			//FLOAT	fAngle;
			//FLOAT	fRadius;
			FLOAT	fX, fZ;
			FLOAT	fTempX, fTempZ;

			FLOAT	fTempRadius;
			AuPOS	stTempAuPos;

			//10미터를 이동하는 거리로 잡는다.
			fTempRadius = 1000.0f; 

			for (nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
			{
				PROFILE("ApmEventManager::GetRandomPos4MobWander (for loop)");

				fTempX = (FLOAT)m_csRandom.rand(fTempRadius * 2.f) - fTempRadius;
				fTempZ = (FLOAT)m_csRandom.rand(fTempRadius * 2.f) - fTempRadius;

				fX = (pstObjCurrentPos->x + fTempX) - pstPos->x;
				fZ = (pstObjCurrentPos->z + fTempZ) - pstPos->z;

				if (fX*fX + fZ*fZ > pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius * pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius)
				{
					continue;
				}

				stTempAuPos.x = pstObjCurrentPos->x + fTempX;
				stTempAuPos.z = pstObjCurrentPos->z + fTempZ;

				{
					PROFILE("ApmEventManager::CheckBlockingInfo");
					//블럭이 없다면 굿! 앗싸아~
					if (!(m_pcsApmMap->CheckBlockingInfo(stTempAuPos , ApmMap::GROUND ) & ApTileInfo::BLOCKGEOMETRY))
					{
						pstPos->x = stTempAuPos.x;
						pstPos->z = stTempAuPos.z;

						bResult = TRUE;
						break;
					}
				}
			}

			break;
		}
		default:
			break;
	}

	return bResult;
}

BOOL ApmEventManager::GetRandomPos(AuPOS *pstBasePos, AuPOS *pstRandomPos, FLOAT fRadiusMin, FLOAT fRadiusMax, BOOL bCheckBlocking)
{
	if (!pstBasePos || !pstRandomPos)
		return FALSE;

	if (fRadiusMin == 0 && fRadiusMax == 0)
	{
		CopyMemory(pstRandomPos, pstBasePos, sizeof(AuPOS));
		return TRUE;
	}

	int i = 0;
	for (i = 0; i < APMEVENT_CONDITION_MAX_TRY; ++i)
	{
		FLOAT	fRandomX		= (FLOAT)m_csRandom.randInt(INT32(fRadiusMax * 2)) - fRadiusMax;
		FLOAT	fRandomZ		= 0.0f;

		INT32	lRandomDistance	= (INT32) (m_csRandom.randInt(INT32(fRadiusMax - fRadiusMin)) + fRadiusMin);

		fRandomZ = sqrt(abs(lRandomDistance * lRandomDistance - fRandomX * fRandomX));

		if (fRandomX < 0)
			fRandomZ = -fRandomZ;

		pstRandomPos->x			= pstBasePos->x + fRandomX;
		pstRandomPos->y			= pstBasePos->y;
		pstRandomPos->z			= pstBasePos->z + fRandomZ;

		if (bCheckBlocking)
		{
			if (!(m_pcsApmMap->CheckBlockingInfo(*pstRandomPos , ApmMap::GROUND ) & ApTileInfo::BLOCKGEOMETRY))
				break;
		}
		else
		{
			break;
		}
	}

	if (i == APMEVENT_CONDITION_MAX_TRY)
		return FALSE;

	return TRUE;
}

INT32 ApmEventManager::GetAreaCharacters( INT32 nDimension , INT32	lCharType, ApdEvent *pstEvent, INT_PTR *alCID, INT16 nCount, INT32* alCID2, INT16 nCount2, AuPOS *pposBase)
{
	AuPOS *		pposSource;
	AuMATRIX *	pstMatrix	= NULL;

	// Validation Check
	if (!m_pcsApmMap || !pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea)
		return 0;

	if (nCount < 1)
		return 0;

	if (pposBase)
		pposSource = pposBase;
	else
	{
		// Base Object/Character/Item의 위치와 방향 matrix를 구한다.
		pposSource = GetBasePos(pstEvent->m_pcsSource, &pstMatrix);
	}

	if (!pposSource)
		return 0;

	// 각 Area type에 따라서 알맞은 map module을 함수를 불러준다.
	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			return m_pcsApmMap->GetCharList( nDimension , lCharType, *pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius, alCID, nCount, alCID2, nCount2);
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetCharList( nDimension , lCharType, *pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius, pstMatrix, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2, alCID, nCount, alCID2, nCount2);
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetCharList( nDimension , lCharType, *pposSource, pstMatrix, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox, alCID, nCount, alCID2, nCount2);
		}
		break;

	default:
		OutputDebugString("ApmEventManager::GetAreaCharacters() Error(1) !!!\n");
		break;
	}

	return 0;
}

INT32 ApmEventManager::GetAreaCharacters( INT32 nDimension , INT32	lCharType, ApdEvent *pstEvent, INT_PTR *alCID, INT16 nCount, INT32* alCID2, INT16 nCount2, ApBase *pcsBase)
{
	AuPOS *		pposSource;
	AuMATRIX *	pstMatrix;

	// Validation Check
	if (!m_pcsApmMap || !pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea)
		return 0;

	if (nCount < 1)
		return 0;

	if (pcsBase)
		pposSource = GetBasePos(pcsBase, &pstMatrix);
	else
	{
		// Base Object/Character/Item의 위치와 방향 matrix를 구한다.
		pposSource = GetBasePos(pstEvent->m_pcsSource, &pstMatrix);
	}

	if (!pposSource)
		return 0;

	// 각 Area type에 따라서 알맞은 map module을 함수를 불러준다.
	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			return m_pcsApmMap->GetCharList( nDimension , lCharType, *pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius, alCID, nCount, alCID2, nCount2);
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetCharList( nDimension , lCharType, *pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius, pstMatrix, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2, alCID, nCount, alCID2, nCount2);
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetCharList( nDimension , lCharType, *pposSource, pstMatrix, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox, alCID, nCount, alCID2, nCount2);
		}
		break;

	default:
		OutputDebugString("ApmEventManager::GetAreaCharacters() Error(1) !!!\n");
		break;
	}

	return 0;
}

/*
INT32 ApmEventManager::GetAreaNPCs(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, AuPOS *pposBase)
{
	AuPOS *		pposSource;
	AuMATRIX *	pstMatrix	= NULL;

	// Validation Check
	if (!m_pcsApmMap || !pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea)
		return 0;

	if (nCount < 1)
		return 0;

	if (pposBase)
		pposSource = pposBase;
	else
	{
		// Base Object/Character/Item의 위치와 방향 matrix를 구한다.
		pposSource = GetBasePos(pstEvent->m_pcsSource, &pstMatrix);
	}

	if (!pposSource)
		return 0;

	// 각 Area type에 따라서 알맞은 map module을 함수를 불러준다.
	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			return m_pcsApmMap->GetNPCList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetNPCList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius, pstMatrix, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetNPCList(*pposSource, pstMatrix, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox, alCID, nCount);
		}
		break;

	default:
		OutputDebugString("ApmEventManager::GetAreaNPCs() Error(1) !!!\n");
		break;
	}

	return 0;
}

INT32 ApmEventManager::GetAreaNPCs(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, ApBase *pcsBase)
{
	AuPOS *		pposSource;
	AuMATRIX *	pstMatrix;

	// Validation Check
	if (!m_pcsApmMap || !pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea)
		return 0;

	if (nCount < 1)
		return 0;

	if (pcsBase)
		pposSource = GetBasePos(pcsBase, &pstMatrix);
	else
	{
		// Base Object/Character/Item의 위치와 방향 matrix를 구한다.
		pposSource = GetBasePos(pstEvent->m_pcsSource, &pstMatrix);
	}

	if (!pposSource)
		return 0;

	// 각 Area type에 따라서 알맞은 map module을 함수를 불러준다.
	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			return m_pcsApmMap->GetNPCList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetNPCList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius, pstMatrix, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetNPCList(*pposSource, pstMatrix, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox, alCID, nCount);
		}
		break;

	default:
		OutputDebugString("ApmEventManager::GetAreaNPCs() Error(2) !!!\n");
		break;
	}

	return 0;
}

INT32 ApmEventManager::GetAreaMonsters(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, AuPOS *pposBase)
{
	AuPOS *		pposSource;
	AuMATRIX *	pstMatrix	= NULL;

	// Validation Check
	if (!m_pcsApmMap || !pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea)
		return 0;

	if (nCount < 1)
		return 0;

	if (pposBase)
		pposSource = pposBase;
	else
	{
		// Base Object/Character/Item의 위치와 방향 matrix를 구한다.
		pposSource = GetBasePos(pstEvent->m_pcsSource, &pstMatrix);
	}

	if (!pposSource)
		return 0;

	// 각 Area type에 따라서 알맞은 map module을 함수를 불러준다.
	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			return m_pcsApmMap->GetMonsterList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetMonsterList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius, pstMatrix, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetMonsterList(*pposSource, pstMatrix, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox, alCID, nCount);
		}
		break;

	default:
		OutputDebugString("ApmEventManager::GetAreaMonsters() Error(1) !!!\n");
		break;
	}

	return 0;
}

INT32 ApmEventManager::GetAreaMonsters(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, ApBase *pcsBase)
{
	AuPOS *		pposSource;
	AuMATRIX *	pstMatrix;

	// Validation Check
	if (!m_pcsApmMap || !pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstArea)
		return 0;

	if (nCount < 1)
		return 0;

	if (pcsBase)
		pposSource = GetBasePos(pcsBase, &pstMatrix);
	else
	{
		// Base Object/Character/Item의 위치와 방향 matrix를 구한다.
		pposSource = GetBasePos(pstEvent->m_pcsSource, &pstMatrix);
	}

	if (!pposSource)
		return 0;

	// 각 Area type에 따라서 알맞은 map module을 함수를 불러준다.
	switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
	{
	case APDEVENT_AREA_SPHERE:
		{
			return m_pcsApmMap->GetMonsterList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_FAN:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetMonsterList(*pposSource, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius, pstMatrix, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2, alCID, nCount);
		}
		break;

	case APDEVENT_AREA_BOX:
		{
			if (!pstMatrix)
				return 0;

			return m_pcsApmMap->GetMonsterList(*pposSource, pstMatrix, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox, alCID, nCount);
		}
		break;

	default:
		OutputDebugString("ApmEventManager::GetAreaMonsters() Error(1) !!!\n");
		break;
	}

	return 0;
}
*/

BOOL ApmEventManager::GetTargetFactorCharacters(ApdEvent *pstEvent, ApmEventConditionProcessor fnProcessor, PVOID pvClass, PVOID pvUpdateData)
{
	if (!m_pcsAgpmCharacter || 
		!pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstTarget || !fnProcessor || !pvClass || !pvUpdateData)
		return FALSE;

	// character를 하나씩 가져와서 pcsTargetFactor와 비교한다.
	// 비교해서 같은넘이면 fnProcessor()에 character pointer를 넘겨준다. (실제 처리는 fnProcessor()에서 한다. 여기선 찾아서 넘기기만 하면된다)

	INT32	lIndex = 0;
	AgpdCharacter *pcsCharacter;

	for (pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		// Deadlock 주의... 여기 조심해야 한다. ㅡ.ㅡ
		//if (!pcsCharacter->m_Mutex.RLock())
		//	continue;

		if (IsTargetFactorCharacter(pstEvent, pcsCharacter))
			fnProcessor(pcsCharacter, pvClass, pvUpdateData);

		//pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL ApmEventManager::GetTargetItemCharacters(ApdEvent *pstEvent, ApmEventConditionProcessor fnProcessor, PVOID pvClass, PVOID pvUpdateData)
{
	if (!m_pcsAgpmCharacter || !m_pcsAgpmItem ||
		!pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstTarget || !fnProcessor || !pvClass || !pvUpdateData)
		return FALSE;

	// character를 하나씩 가져와서 item을 착용하고 있는지 검사한다.
	// 검사해서 착용하고 있다면 fnProcessor()에 character pointer를 넘겨준다. (실제 처리는 fnProcessor()에서 한다. 여기선 찾아서 넘기기만 하면된다)

	INT32	lIndex = 0;
	AgpdCharacter *pcsCharacter;

	for (pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		// Deadlock 주의... 여기 조심해야 한다. ㅡ.ㅡ
		//if (!pcsCharacter->m_Mutex.RLock())
		//	continue;

		if (IsTargetItemCharacter(pstEvent, pcsCharacter))
			fnProcessor(pcsCharacter, pvClass, pvUpdateData);

		//pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL ApmEventManager::IsTargetFactorCharacter(ApdEvent *pstEvent, AgpdCharacter *pcsCharacter)
{
	if (!m_pcsAgpmCharacter || !m_pcsAgpmFactors ||
		!pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstTarget || !pcsCharacter)
		return FALSE;

	return m_pcsAgpmFactors->CompareFactor(&pcsCharacter->m_csFactor, &pstEvent->m_pstCondition->m_pstTarget->m_stFactor);
}

BOOL ApmEventManager::IsTargetItemCharacter(ApdEvent *pstEvent, AgpdCharacter *pcsCharacter)
{
	if (!m_pcsAgpmCharacter || !m_pcsAgpmItem ||
		!pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstTarget || !pcsCharacter)
		return FALSE;

	AgpdGridItem	*pcsAgpdGridItem;
	INT32			lItemID;

	for (int i = 0; i < AGPMITEM_PART_NUM; ++i)
	{
		lItemID	= AP_INVALID_IID;
		
		pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem( pcsCharacter, i );

		if( pcsAgpdGridItem != NULL )
		{
			lItemID = pcsAgpdGridItem->m_lItemID;
		}
		else
		{
			continue;
		}

		AgpdItem	*pcsItem = m_pcsAgpmItem->GetItem(lItemID);
		if (!pcsItem)
			continue;

		for (int j = 0; j < APDEVENT_TARGET_NUMBER; ++j)
		{
			if (pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[j] == AP_INVALID_IID)
				continue;

			if (pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[j] == ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL ApmEventManager::IsActiveSkillTIme(ApdEvent *pstEvent, UINT32 ulClockCount)
{
	if (!pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstTime)
		return FALSE;

	if (pstEvent->m_ulEventStartTime + pstEvent->m_pstCondition->m_pstTime->m_lActiveTimeOffset <= ulClockCount)
		return TRUE;

	return FALSE;
}

BOOL ApmEventManager::IsEndSkillTIme(ApdEvent *pstEvent, UINT32 ulClockCount)
{
	if (!pstEvent || !pstEvent->m_pstCondition || !pstEvent->m_pstCondition->m_pstTime)
		return FALSE;

	if (pstEvent->m_ulEventStartTime + pstEvent->m_pstCondition->m_pstTime->m_lEndTimeOffset <= ulClockCount)
		return TRUE;

	return FALSE;
}