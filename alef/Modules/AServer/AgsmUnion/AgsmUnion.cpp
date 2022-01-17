/******************************************************************************
Module:  AgsmUnion.cpp
Notices: Copyright (c) NHN Studio (netong)
Purpose: Union Rank Point와 관련된 모든 계산을 처리한다.
******************************************************************************/

#include "AgsmUnion.h"

extern AgpdUnionRank	g_csUnionRankTable;

AgsmUnion::AgsmUnion()
{
	SetModuleName("AgsmUnion");
}

AgsmUnion::~AgsmUnion()
{
}

BOOL AgsmUnion::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmUnion				= (AgpmUnion *)			GetModule("AgpmUnion");

	m_pcsAgsmFactors			= (AgsmFactors *)		GetModule("AgsmFactors");
	m_pcsAgsmCharacter			= (AgsmCharacter *)		GetModule("AgsmCharacter");

	if (!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmUnion ||

		!m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgpmUnion->SetCallbackUpdateUnionRank(CBUpdateUnionRank, this))
		return FALSE;
	if (!m_pcsAgpmUnion->SetCallbackUpdateUnionRankPoint(CBUpdateUnionRankPoint, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmUnion::OnInit()
{
	return TRUE;
}

BOOL AgsmUnion::OnDestroy()
{
	return TRUE;
}

INT32 AgsmUnion::GetBonusUnionRank(AgpdCharacter *pcsCharacter, INT32 lBonusExp, AgpdItem *pcsSkull)
{
	if (!pcsCharacter)
		return 0;

	INT32	lUnionRank = m_pcsAgpmUnion->GetUnionRank(pcsCharacter);

	// 레벨 4 이하는 획득하는 Exp 만큼, 5이상은 pcsSkull에 저장되어 있는 소유자 UnionRank에 따라...
	if (pcsSkull && pcsSkull->m_pstSkullInfo)
	{
		// pcsSkull이 진짜 유골 아템인지 검사한다.
		if (((AgpdItemTemplateOther *) pcsSkull->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_OTHER ||
			((AgpdItemTemplateOther *) pcsSkull->m_pcsItemTemplate)->m_eOtherItemType != AGPMITEM_OTHER_TYPE_SKULL)
			return 0;

		// pcsSkull의 주인, 즉 죽인넘이 pcsCharacter이지 검사한다.
		if (!CheckUnionKiller(pcsCharacter, pcsSkull))
			return 0;

		// pcsSkull에 의해 저장되어 있는 유니온랭크 차이에 따라 값을 구한다.
		INT32	lUnionDiffer = pcsSkull->m_pstSkullInfo->lSkullUnionRank - pcsSkull->m_pstSkullInfo->lKillerUnionRank;

		if (lUnionDiffer > 2) lUnionDiffer = 2;
		if (lUnionDiffer < (-3)) lUnionDiffer = (-3);

		return g_csUnionRankTable.m_csUnionRankTable[pcsSkull->m_pstSkullInfo->lKillerUnionRank].m_lBonusRankPoint[(2 - lUnionDiffer)];
	}
	else if (lUnionRank <= 4)
		return lBonusExp;
	else
		return 0;

	return 0;
}

INT32 AgsmUnion::GetPenaltyUnionRankByUvU(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT32	lUnionRank = m_pcsAgpmUnion->GetUnionRank(pcsCharacter);

	if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
		return 0;

	return g_csUnionRankTable.m_csUnionRankTable[lUnionRank].m_lLoseRankPointByUvU;
}

INT32 AgsmUnion::GetPenaltyUnionRankByNormal(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT32	lUnionRank = m_pcsAgpmUnion->GetUnionRank(pcsCharacter);

	if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
		return 0;

	return g_csUnionRankTable.m_csUnionRankTable[lUnionRank].m_lLoseRankPointByNormal;
}

INT32 AgsmUnion::GetRestoreExpBySkull(AgpdCharacter *pcsCharacter, AgpdItem *pcsSkull)
{
	if (!pcsCharacter || !pcsSkull || !pcsSkull->m_pstSkullInfo)
		return 0;

	// pcsSkull이 진짜 유골 아템인지 검사한다.
	if (((AgpdItemTemplateOther *) pcsSkull->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_OTHER ||
		((AgpdItemTemplateOther *) pcsSkull->m_pcsItemTemplate)->m_eOtherItemType != AGPMITEM_OTHER_TYPE_SKULL)
		return 0;

	INT32	lUnionRank = m_pcsAgpmUnion->GetUnionRank(pcsCharacter);
	if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
		return 0;

	return g_csUnionRankTable.m_csUnionRankTable[lUnionRank].m_lRestoreExpBySkull;
}

BOOL AgsmUnion::CheckUnionKiller(AgpdCharacter *pcsCharacter, AgpdItem *pcsSkull)
{
	if (!pcsCharacter || !pcsSkull || !pcsSkull->m_pstSkullInfo)
		return FALSE;

	if (pcsSkull->m_pstSkullInfo->szKiller && pcsSkull->m_pstSkullInfo->szKiller[0])
	{
		// pcsSkull의 주인, 즉 죽인넘이 pcsCharacter이지 검사한다.
		if (strncmp(pcsSkull->m_pstSkullInfo->szKiller, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING) != 0)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		for (int i = 0; pcsSkull->m_pstSkullInfo->lPartyMemberID[i] != AP_INVALID_CID; ++i)
		{
			if (pcsSkull->m_pstSkullInfo->lPartyMemberID[i] == pcsCharacter->m_lID)
				return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

BOOL AgsmUnion::CBUpdateUnionRankPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmUnion			*pThis			= (AgsmUnion *)			pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	// 변경된 rank point를 패킷으로 만들어서 pcsCharacter에게 보낸다.

	PVOID	pvPacketFactor = pThis->m_pcsAgsmFactors->MakePacketUpdateUnionRank(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
		return FALSE;

	BOOL	bSendResult = pThis->m_pcsAgsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter);

	pThis->m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	return bSendResult;
}

BOOL AgsmUnion::CBUpdateUnionRank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmUnion			*pThis			= (AgsmUnion *)			pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	// union rank가 변경되었다. 다시 계산되어야 할 값들이 있음 여기서 다시 계산한다.
	//
	//		1. 파티 리더라면 파티의 최대 참여인원, 공격력, 방어력 등등을 다시 계산해 넣는다.
	//
	//
	//
	//
	//
	//

	return TRUE;
}