/******************************************************************************
Module:  AgsmShrine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 24
******************************************************************************/

#include "AgsmShrine.h"

AgsmShrine::AgsmShrine()
{
	SetModuleName("AgsmShrine");

	EnableIdle2(TRUE);

	m_lShrineGenerateID	= 1;
}

AgsmShrine::~AgsmShrine()
{
}

BOOL AgsmShrine::OnAddModule()
{
	m_papmObject		= (ApmObject *)			GetModule("ApmObject");
	m_papmMap			= (ApmMap *)			GetModule("ApmMap");

	m_pagpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pagpmSkill		= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pagpmShrine		= (AgpmShrine *)		GetModule("AgpmShrine");

	m_pagsmAOIFilter	= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pAgsmServerManager	= (AgsmServerManager *)	GetModule("AgsmServerManager2");
	m_pagsmCharacter	= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pagsmSkill		= (AgsmSkill *)			GetModule("AgsmSkill");
	m_pagsmSkillManager	= (AgsmSkillManager *)	GetModule("AgsmSkillManager");

	if (!m_papmObject || !m_papmMap || !m_pagpmFactors || !m_pagpmCharacter || !m_pagpmSkill || !m_pagpmShrine || !m_pagsmAOIFilter || !m_pagsmCharacter || !m_pagsmSkill || !m_pagsmSkillManager || !m_pAgsmServerManager)
		return FALSE;

	m_nIndexADSector = m_papmMap->AttachSectorData(this, sizeof(AgsdShrineADSector), CBConAgsdADSector, CBDesAgsdADSector);
	if (m_nIndexADSector < 0)
		return FALSE;

	if (!m_pagpmShrine->SetCallbackGenerateShrineEvent(CBGenerateShrineEvent, this))
		return FALSE;

	if (!m_pagpmShrine->SetCallbackActiveShrine(CBActiveShrine, this))
		return FALSE;

	if (!m_pagpmShrine->SetCallbackInactiveShrine(CBInactiveShrine, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackReCalcResultFactor(CBReCalcResultFactor, this))
		return FALSE;

	if (!m_pagpmShrine->SetCallbackAddShrineEvent(CBAddShrineEvent, this))
		return FALSE;

	if (!m_pagsmAOIFilter->SetCallbackAddChar(CBAddChar, this))
		return FALSE;
	if (!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveChar, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmShrine::OnInit()
{
	for (int i = 0; i < AGPMCHAR_MAX_UNION; i++)
	{
		m_pagpmFactors->InitFactor(&m_csUnionFactor[i]);
		m_csUnionFactor[i].m_bPoint = TRUE;
	}

	return TRUE;
}

BOOL AgsmShrine::OnDestroy()
{
	return TRUE;
}

BOOL AgsmShrine::OnIdle2(UINT32 ulClockCount)
{
	// shrine active, inactive 처리를 해준다.

	INT32		lIndex		= 0;
	for (AgpdShrine *pcsShrine = m_pagpmShrine->GetShrineSequence(&lIndex); pcsShrine; pcsShrine = m_pagpmShrine->GetShrineSequence(&lIndex))
	{
		if (!pcsShrine->m_Mutex.WLock())
			continue;

		if (pcsShrine->m_bIsActive)
		{
			// active인 경우 inactive 되는 조건을 검사해서 조건이 만족한다면 inactive 시켜준다.
			switch (pcsShrine->m_pcsTemplate->m_eActiveCondition) {
			case AGPMSHRINE_ACTIVE_ALWAYS:
				break;

			case AGPMSHRINE_ACTIVE_DAY_ONLY:
				{
					// 낮이 아니라면 inactive
					if (!m_pagpmShrine->CheckDay())
					{
						m_pagpmShrine->InactiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;

			case AGPMSHRINE_ACTIVE_SPAWN:
				{
					// 현재 active duration 시간이 지났는지 검사한다.
					if (pcsShrine->m_ulLastActiveDurationMSec < ulClockCount - pcsShrine->m_ulLastActiveTimeMSec)
					{
						// inactive 시킨다.
						m_pagpmShrine->InactiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;
			}
		}
		else
		{
			// inactive은 경우 active될 조건을 검사해서 조건이 만족하면 active 시켜준다.
			switch (pcsShrine->m_pcsTemplate->m_eReActiveCondition) {
			case AGPMSHRINE_REACTIVE_DAY_ONLY:
				{
					// 낮이라면 active
					if (m_pagpmShrine->CheckDay())
					{
						m_pagpmShrine->ActiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;

			case AGPMSHRINE_REACTIVE_TWICE_LAST_ACTIVE_TIME:
				{
					// 현재 last active duration 시간의 2배가 지났는지 검사한다.
					if (pcsShrine->m_ulLastActiveDurationMSec * 2 < ulClockCount - pcsShrine->m_ulLastInactiveTimeMSec)
					{
						// active 시킨다.
						m_pagpmShrine->ActiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;
			}
		}

		pcsShrine->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmShrine::CBGenerateShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmShrine		*pThis			= (AgsmShrine *)	pClass;
	ApdEvent		*pcsEvent		= (ApdEvent *)		pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pCustData;

	AgpdShrine		*pcsShrine		= (AgpdShrine *)	pcsEvent->m_pvData;
	if (pcsShrine)
	{
		// shrine이 active 상태인지 본다.
		if (!pcsShrine->m_bIsActive)
			return FALSE;

		// 스킬의 타겟을 pcsCharacter로 세팅한다.
		for (int i = 0; pcsShrine->m_lSkillID[i] != AP_INVALID_SKILLID; i++)
		{
			AgpdSkill *pcsSkill = pThis->m_pagpmSkill->GetSkill(pcsShrine->m_lSkillID[i]);
			if (pcsSkill)
			{
				pcsSkill->m_csTargetBase.m_eType	= pcsCharacter->m_eType;
				pcsSkill->m_csTargetBase.m_lID		= pcsCharacter->m_lID;
			}
		}
	}

	return TRUE;
}

BOOL AgsmShrine::CBActiveShrine(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmShrine		*pThis		= (AgsmShrine *) pClass;
	AgpdShrine		*pcsShrine	= (AgpdShrine *) pData;

	if (!pcsShrine->m_pcsBase || !pcsShrine->m_pcsTemplate)
		return FALSE;

	// shrine object를 active 상태로 만든다.
	pThis->m_papmObject->UpdateStatus(pcsShrine->m_pcsBase->m_lID, APDOBJ_STATUS_ACTIVE_TYPE1);

	// 이 shrine에 속한 스킬을 가져다가 object에 붙인다.
	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; i++)
	{
		if (!pcsShrine->m_pcsTemplate->m_szSkillName[i] || !strlen(pcsShrine->m_pcsTemplate->m_szSkillName[i]))
			continue;

		AgpdSkillTemplate *pcsSkillTemplate = pThis->m_pagpmSkill->GetSkillTemplate(pcsShrine->m_pcsTemplate->m_szSkillName[i]);
		if (pcsSkillTemplate)
		{
			AgpdSkill *pcsSkill = pThis->m_pagsmSkillManager->CreateSkill(pcsShrine->m_pcsBase, pcsSkillTemplate->m_lID);
			if (!pcsSkill)
				continue;

			pcsSkill->m_csTargetBase.m_eType	= APBASE_TYPE_NONE;
			pcsSkill->m_csTargetBase.m_lID		= 0;

			if (!pThis->m_pagsmSkill->CastSkill(pcsSkill))
			{
				pThis->m_pagpmSkill->RemoveSkill(pcsSkill->m_lID);
				continue;
			}

			pcsShrine->m_lSkillID[i] = pcsSkill->m_lID;
		}
	}

	// 이 shrine이 위치해 있는 sector에 attach 해놓은 데이타를 업데이트 한다.
	//		(해당 sector에 이 shrine 정보를 저장한다.)

	ApWorldSector *pcsSector = pThis->m_papmMap->GetSector(pcsShrine->m_posShrine);
	if (pcsSector)
	{
		AgsdShrineADSector *pcsADSector = pThis->GetADSector(pcsSector);
		if (pcsADSector)
		{
			pcsADSector->pcsShrine = pcsShrine;
		}
	}

	return TRUE;
}

BOOL AgsmShrine::CBInactiveShrine(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
	
	AgsmShrine		*pThis		= (AgsmShrine *) pClass;
	AgpdShrine		*pcsShrine	= (AgpdShrine *) pData;

	// shrine object를 inactive 상태로 만든다.
	pThis->m_papmObject->UpdateStatus(pcsShrine->m_pcsBase->m_lID, APDOBJ_STATUS_NORMAL);

	// 이 shrine에 속한 스킬을 없앤다.
	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; i++)
	{
		if (pcsShrine->m_lSkillID[i] != AP_INVALID_SKILLID)
		{
			pThis->m_pagpmSkill->RemoveSkill(pcsShrine->m_lSkillID[i]);

			pcsShrine->m_lSkillID[i] = AP_INVALID_SKILLID;
		}
	}

	// 이 shrine이 위치해 있는 sector에 attach 해놓은 데이타를 업데이트 한다.
	//		(이 shrine이 위치해 있던 sector의 shrine정보를 초기화한다)

	ApWorldSector *pcsSector = pThis->m_papmMap->GetSector(pcsShrine->m_posShrine);
	if (pcsSector)
	{
		AgsdShrineADSector *pcsADSector = pThis->GetADSector(pcsSector);
		if (pcsADSector)
		{
			pcsADSector->pcsShrine = NULL;
		}
	}

	return TRUE;
}

BOOL AgsmShrine::CBReCalcResultFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmShrine		*pThis			= (AgsmShrine *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	INT32	lUnion = pThis->m_pagpmCharacter->GetUnion(pcsCharacter);
	if (lUnion < 0 || lUnion >= AGPMCHAR_MAX_UNION)
		return FALSE;

	pThis->m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, &pThis->m_csUnionFactor[lUnion], TRUE, FALSE, TRUE, FALSE);

	return TRUE;
}

BOOL AgsmShrine::CBAddShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pstEvent		= (ApdEvent *)				pData;
	AgsmShrine			*pThis			= (AgsmShrine *)			pClass;
	AgpdShrineTemplate	*pcsTemplate	= (AgpdShrineTemplate *)	pCustData;

	// shrine을 추가한다.
	AgpdShrine			*pcsShrine		= pThis->m_pagpmShrine->AddShrine(pcsTemplate->m_lID, pcsTemplate->m_lID);
	if (!pcsShrine)
		return FALSE;

	++pThis->m_lShrineGenerateID;

	//pstEvent->m_pvData = pcsShrine;

	return TRUE;
}

BOOL AgsmShrine::CBConAgsdADSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApWorldSector	*pcsSector	= (ApWorldSector *)	pData;
	AgsmShrine		*pThis		= (AgsmShrine *)	pClass;

	AgsdShrineADSector	*pcsADSector	= pThis->GetADSector(pcsSector);
	if (!pcsADSector)
		return FALSE;

	pcsADSector->pcsShrine = NULL;

	return TRUE;
}

BOOL AgsmShrine::CBDesAgsdADSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgsdShrineADSector*	AgsmShrine::GetADSector(PVOID pvSector)
{
	if (!pvSector)
		return FALSE;

	return (AgsdShrineADSector *) m_papmMap->GetAttachedModuleData(m_nIndexADSector, pvSector);
}

BOOL AgsmShrine::CBAddChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
//2004.7.16 광준옹의 지시로 주석처리했음.
/*	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmShrine		*pThis		= (AgsmShrine *)	pClass;
	ApWorldSector	*pSector	= (ApWorldSector *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter) return FALSE;

	// 이 서버가 관리하는 섹터가 아닌경우 아무짓도 안한다.
	AgsdServer *pcsThisServer			= pThis->m_pAgsmServerManager->GetThisServer();

	if (pcsAgsdCharacter->m_ulServerID != pcsThisServer->m_lServerID)
		return FALSE;

	for (int i = (int) pSector->GetIndexX() - 1; i <= (int) pSector->GetIndexX() + 1; i++)
	{
		for (int j = (int) pSector->GetIndexZ() - 1; j <= (int) pSector->GetIndexZ() + 1; j++)
		{
			ApWorldSector *pAroundSector = pThis->m_pagsmCharacter->GetSectorBySectorIndex(i, 0, j);
			if (!pAroundSector)
				continue;

			AgsdShrineADSector *pcsADSector = pThis->GetADSector(pAroundSector);
			if (!pcsADSector)
				continue;

			if (pcsADSector->pcsShrine)
			{
				INT16	nPacketLength = 0;

				PVOID	pvPacket = pThis->m_pagpmShrine->MakePacketAddShrine(pcsADSector->pcsShrine, &nPacketLength);

				if (!pvPacket || nPacketLength < 1)
					continue;

				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

				pThis->m_pagpmShrine->m_csPacket.FreePacket(pvPacket);
			}
		}
	}*/

	return TRUE;
}

BOOL AgsmShrine::CBMoveChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	/*

	AgsmShrine		*pThis		= (AgsmShrine *)	pClass;
	ApWorldSector	*pSector	= (ApWorldSector *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter) return FALSE;

	// 이 서버가 관리하는 섹터가 아닌경우 아무짓도 안한다.
	AgsdServer *pcsThisServer			= pThis->m_pagsmServerManager->GetThisServer();

	if (pcsAgsdCharacter->m_ulServerID != pcsThisServer->m_lServerID)
		return FALSE;

	ApWorldSector	*paForwardSector[9];
	ApWorldSector	*paBackwardSector[9];
	ZeroMemory(&paForwardSector, sizeof(ApWorldSector *) * 9);
	ZeroMemory(&paBackwardSector, sizeof(ApWorldSector *) * 9);

	// 이동 방향을 보고 새로 데이타 싱크를 맞추어야 하는 섹터를 구한다.
	if (!pThis->m_pagsmCharacter->GetForwardSector(pcsAgsdCharacter->m_pPrevSector, pcsAgsdCharacter->m_pCurrentSector, paForwardSector, paBackwardSector))
	{
		return FALSE;
	}

	for (int i = 0; i < 9; i++)
	{
		if (!paBackwardSector[i])
			continue;

		AgsdShrineADSector *pcsADSector = pThis->GetADSector(paBackwardSector[i]);
		if (!pcsADSector)
			continue;

		if (pcsADSector->pcsShrine)
		{
			INT16	nPacketLength = 0;

			PVOID	pvPacket = pThis->m_pagpmShrine->MakePacketRemoveShrine(pcsADSector->pcsShrine, &nPacketLength);

			if (!pvPacket || nPacketLength < 1)
				continue;

			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

			pThis->m_pagpmShrine->m_csPacket.FreePacket(pvPacket);
		}

	}

	for (i = 0; i < 9; i++)
	{
		if (!paForwardSector[i])
			continue;

		AgsdShrineADSector *pcsADSector = pThis->GetADSector(paForwardSector[i]);
		if (!pcsADSector)
			continue;

		if (pcsADSector->pcsShrine)
		{
			INT16	nPacketLength = 0;

			PVOID	pvPacket = pThis->m_pagpmShrine->MakePacketAddShrine(pcsADSector->pcsShrine, &nPacketLength);

			if (!pvPacket || nPacketLength < 1)
				continue;

			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

			pThis->m_pagpmShrine->m_csPacket.FreePacket(pvPacket);
		}
	}

	*/

	return TRUE;
}

BOOL AgsmShrine::CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	ApdEvent*			pcsEvent = (ApdEvent *) pvData;
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pvCustData;
	AgsmShrine *		pThis = (AgsmShrine *) pvClass;

	ASSERT(pcsEvent && pcsCharacter && pThis);

	if (!pcsEvent->m_pvData)
		return FALSE;

	AgpdSpawn			*pcsSpawn = (AgpdSpawn *) pcsEvent->m_pvData;

	AgpdShrineADSpawn	*pcsAttachSpawnData = pThis->m_pagpmShrine->GetADSpawn(pcsSpawn);

	if (strlen(pcsAttachSpawnData->m_szShrineName))
	{
		// spawn에 shrine 정보가 세팅되어 있는 경우이다.
		// spawn된 character에 shrine spawn 정보를 세팅한다.

		if (pcsAttachSpawnData->m_lShrineID == 0)
		{
			AgpdShrineTemplate	*pcsShrineTemplate = pThis->m_pagpmShrine->GetShrineTemplate(pcsAttachSpawnData->m_szShrineName);
			if (pcsShrineTemplate)
			{
				// shrine data id와 shrine template data id는 동일하다는 전재하에 개발되었다.
				pcsAttachSpawnData->m_lShrineID = pcsShrineTemplate->m_lID;
			}
			else
				return FALSE;
		}

		AgpdShrineADChar	*pcsAttachCharData = pThis->m_pagpmShrine->GetADCharacter(pcsCharacter);

		pcsAttachCharData->m_bIsGuardian	= TRUE;
		pcsAttachCharData->m_lShrineID		= pcsAttachSpawnData->m_lShrineID;
		pcsAttachCharData->m_lGuardianLevel	= pcsAttachSpawnData->m_lShrineLevel;
	}

	return TRUE;
}