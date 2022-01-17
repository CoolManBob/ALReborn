//	AgsmEventUvUReward module
//		- Union vs Union battle 과 관련된 보상처리
/////////////////////////////////////////////////////////////////////
/*
#include "AgsmEventUvUReward.h"

AgsmEventUvUReward::AgsmEventUvUReward()
{
	SetModuleName("AgsmEventUvUReward");
}

AgsmEventUvUReward::~AgsmEventUvUReward()
{
}

BOOL AgsmEventUvUReward::OnAddModule()
{
	m_pcsAgpmEventUvUReward	= (AgpmEventUvUReward *)	GetModule("AgpmEventUvUReward");
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmParty			= (AgpmParty *)				GetModule("AgpmParty");

	m_pcsAgsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmItem			= (AgsmItem *)				GetModule("AgsmItem");

	m_pcsAgsmDeath			= (AgsmDeath *)				GetModule("AgsmDeath");

	if (!m_pcsAgpmEventUvUReward ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmParty ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmItem ||
		!m_pcsAgsmDeath)
		return FALSE;

	if (!m_pcsAgpmEventUvUReward->SetCallbackRewardExp(CBRewardExp, this))
		return FALSE;
	if (!m_pcsAgpmEventUvUReward->SetCallbackRewardUnionRank(CBRewardUnionRank, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventUvUReward::OnInit()
{
	return TRUE;
}

BOOL AgsmEventUvUReward::OnDestroy()
{
	return TRUE;
}

BOOL AgsmEventUvUReward::CBRewardExp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventUvUReward	*pThis			= (AgsmEventUvUReward *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)			pData;
	AgpdItem			*pcsItemSkull	= (AgpdItem *)				pCustData;

	// pcsCharacter가 pcsItemSkull을 가지고 왔다.
	// 이 Event는 Exp를 보상해주는거다.

	INT32	lRewardExp = pThis->m_pcsAgsmCharacter->GetRestoreExpBySkull(pcsCharacter, pcsItemSkull);

	if (lRewardExp <= 0)
		return TRUE;

	AgsdDeath			*pcsDeath		= pThis->m_pcsAgsmDeath->GetADCharacter(pcsCharacter);
	if (!pcsDeath)
		return FALSE;

	if (lRewardExp > pcsDeath->m_lLastLoseExp) lRewardExp = pcsDeath->m_lLastLoseExp;

	// lRewardExp 만큼이 회복되는 양이다.
	// lRewardExp를 회복해준다.
	// last lose exp를 초기화한다.

	if (lRewardExp <= 0)		// 복구해줄 양이 없다.
		return TRUE;

	if (!pThis->m_pcsAgsmDeath->AddBonusExpToChar(pcsCharacter, lRewardExp))
		return FALSE;

	pcsDeath->m_lLastLoseExp = 0;

	// 유골을 삭제한다.
	return pThis->m_pcsAgpmItem->DeleteItem(pcsItemSkull);
}

BOOL AgsmEventUvUReward::CBRewardUnionRank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventUvUReward	*pThis			= (AgsmEventUvUReward *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)			pData;
	AgpdItem			*pcsItemSkull	= (AgpdItem *)				pCustData;

	// pcsCharacter가 pcsItemSkull을 가지고 왔다.
	// 이 Event는 UnionRank를 보상해주는거다.

	INT32	lBonusUnionRank = pThis->m_pcsAgsmCharacter->GetBonusUnionRank(pcsCharacter, 0, pcsItemSkull);
	if (lBonusUnionRank > 0)
	{
		if (pcsItemSkull->m_pstSkullInfo->szKiller && pcsItemSkull->m_pstSkullInfo->szKiller[0])
		{
			return pThis->m_pcsAgpmCharacter->UpdateUnionRankPoint(pcsCharacter, lBonusUnionRank);
		}
		else
		{
			AgpdParty	*pcsParty = pThis->m_pcsAgpmParty->GetPartyLock(pcsCharacter);
			if (!pcsParty)
			{
				if (pThis->CheckPartyMemberList(pcsCharacter->m_lID, pcsItemSkull))
					return pThis->m_pcsAgpmCharacter->UpdateUnionRankPoint(pcsCharacter, lBonusUnionRank);
				else
					return FALSE;
			}
			else
			{
				INT32	lTotalMemberUnionRank = 0;
				for (int i = 0; pcsParty->m_pcsMemberList[i]; ++i)
				{
					if (pThis->CheckPartyMemberList(pcsParty->m_pcsMemberList[i]->m_lID, pcsItemSkull))
					{
						lTotalMemberUnionRank += pThis->m_pcsAgpmCharacter->GetUnionRank(pcsParty->m_pcsMemberList[i]);
					}
				}

				if (lTotalMemberUnionRank < 1)
				{
					pcsParty->m_Mutex.Release();
					return FALSE;
				}

				for (i = 0; pcsParty->m_pcsMemberList[i]; ++i)
				{
					if (pThis->CheckPartyMemberList(pcsParty->m_pcsMemberList[i]->m_lID, pcsItemSkull))
					{
						INT32	lBonusPoint = (INT32) lBonusUnionRank * (pThis->m_pcsAgpmCharacter->GetUnionRank(pcsParty->m_pcsMemberList[i]) / (lTotalMemberUnionRank + 0.0));

						pThis->m_pcsAgpmCharacter->UpdateUnionRankPoint(pcsParty->m_pcsMemberList[i], lBonusPoint);
					}
				}

				pcsParty->m_Mutex.Release();
			}
		}
	}

	return FALSE;
}

BOOL AgsmEventUvUReward::CheckPartyMemberList(INT32 lMemberID, AgpdItem *pcsItemSkull)
{
	if (lMemberID == AP_INVALID_CID ||
		!pcsItemSkull ||
		!pcsItemSkull->m_pstSkullInfo)
		return FALSE;

	for (int i = 0; pcsItemSkull->m_pstSkullInfo->lPartyMemberID[i] != AP_INVALID_CID; ++i)
	{
		if (pcsItemSkull->m_pstSkullInfo->lPartyMemberID[i] == lMemberID)
			return TRUE;
	}

	return FALSE;
}
*/
