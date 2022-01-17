/******************************************************************************
Module:  AgpmParty.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 07
******************************************************************************/

#include "AgpmParty.h"

AgpmParty::AgpmParty()
{
	SetModuleName("AgpmParty");

	SetModuleType(APMODULE_TYPE_PUBLIC);

	SetPacketType(AGPMPARTY_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1,		// Operation
							AUTYPE_INT32,			1,		// Party ID
							AUTYPE_INT8,			1,		// # of MaxMember
							AUTYPE_INT8,			1,		// # of current member
							AUTYPE_INT32,			1,		// Add Remove member
							AUTYPE_MEMORY_BLOCK,	1,		// party member list
							AUTYPE_PACKET,			1,		// factor
							AUTYPE_PACKET,			1,		// effect area
							AUTYPE_INT8,			1,		// AgpmPartyCalcExpType
							AUTYPE_PACKET,			1,		// Bonus Stats
							AUTYPE_INT8	,			1,		// ItemDivisionType
		                    AUTYPE_END,				0
							);

	m_csPacketEffectArea.SetFlagLength(sizeof(INT8));
	m_csPacketEffectArea.SetFieldType(
							AUTYPE_INT8,			1,		// member 1
							AUTYPE_INT8,			1,		// member 2
							AUTYPE_INT8,			1,		// member 3
							AUTYPE_INT8,			1,		// member 4
							AUTYPE_INT8,			1,		// member 5
							AUTYPE_END,				0
							);

	m_csPacketBonusStats.SetFlagLength(sizeof(INT8));
	m_csPacketBonusStats.SetFieldType(
							AUTYPE_INT8,			1,		// damage
							AUTYPE_INT8,			1,		// defense
							AUTYPE_INT16,			1,		// max hp
							AUTYPE_INT16,			1,		// max mp
							AUTYPE_END,				0
							);

	// setting module data
	SetModuleData(sizeof(AgpdParty), AGPMPARTY_DATA_TYPE_PARTY);
	
	m_nIndexADCharacter	= 0		;
	m_pagpmFactors		= NULL	;
	m_pagpmCharacter	= NULL	;
	m_pagpmUnion		= NULL	;

	EnableIdle2(TRUE);

	m_ulPrevRemoveClockCount	= 0;
}

AgpmParty::~AgpmParty()
{
}

BOOL AgpmParty::OnAddModule()
{
	m_pagpmFactors		= (AgpmFactors *)	GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)	GetModule("AgpmCharacter");
	m_pagpmUnion		= (AgpmUnion *)		GetModule("AgpmUnion");

	if (!m_pagpmCharacter || !m_pagpmUnion)
		return FALSE;

	// setting attached data(m_nKeeping) in character module
	if (m_pagpmCharacter)
	{
		m_nIndexADCharacter = 
			m_pagpmCharacter->AttachCharacterData(this, sizeof(AgpdPartyADChar), ConAgpdPartyADChar, DesAgpdPartyADChar);

		if (m_nIndexADCharacter < 0)
			return FALSE;

		if (!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveChar, this))
			return FALSE;
		if (!m_pagpmCharacter->SetCallbackCheckValidNormalAttack(CBCheckValidNormalAttack, this))
			return FALSE;
		if (!m_pagpmCharacter->SetCallbackUpdateLevelPost(CBCharLevelUp, this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmParty::OnInit()
{
	// initialize data size (item, item template data)
	if (!m_csPartyAdmin.InitializeObject(sizeof(AgpdParty *), m_csPartyAdmin.GetCount()))
		return FALSE;

	if (!m_csAdminPartyRemove.InitializeObject(sizeof(AgpdParty *), m_csAdminPartyRemove.GetCount()))
		return FALSE;

	return TRUE;
}

BOOL AgpmParty::OnDestroy()
{
	INT32	lIndex = 0;
	for (AgpdParty	*pcsParty = (AgpdParty *) m_csPartyAdmin.GetObjectSequence(&lIndex); pcsParty; pcsParty = (AgpdParty *) m_csPartyAdmin.GetObjectSequence(&lIndex))
	{
		RemoveParty(pcsParty->m_lID);
	}

	m_csPartyAdmin.RemoveObjectAll();

	return TRUE;
}

BOOL AgpmParty::OnIdle2(UINT32 ulClockCount)
{
	PROFILE("AgpmParty::OnIdle2");

	ProcessRemove(ulClockCount);

	return TRUE;
}

/*
BOOL AgpmParty::OnValid(CHAR* szData, INT16 nSize)
{
	return TRUE;
}
*/

BOOL AgpmParty::ConAgpdPartyADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmParty *pThis = (AgpmParty *) pClass;

	AgpdPartyADChar *pADChar = pThis->GetADCharacter(pData);

	pADChar->lPID = AP_INVALID_PARTYID;

	pThis->m_pagpmFactors->InitFactor(&pADChar->m_csFactorPoint);

	return TRUE;
}

BOOL AgpmParty::DesAgpdPartyADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmParty *pThis = (AgpmParty *) pClass;

	AgpdPartyADChar *pADChar = pThis->GetADCharacter(pData);

	pThis->m_pagpmFactors->DestroyFactor(&pADChar->m_csFactorPoint);

	return TRUE;
}

AgpdPartyADChar* AgpmParty::GetADCharacter(PVOID pData)
{
	return (AgpdPartyADChar *) m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, pData);
}

AgpdParty* AgpmParty::CreatePartyData()
{
	AgpdParty* pcsParty = (AgpdParty *) CreateModuleData(AGPMPARTY_DATA_TYPE_PARTY);

	if (pcsParty)
	{
		pcsParty->m_Mutex.Init((PVOID) pcsParty);
		pcsParty->m_eType	= APBASE_TYPE_PARTY;

//		ZeroMemory(pcsParty->m_lMemberListID, sizeof(INT32) * AGPMPARTY_MAX_PARTY_MEMBER);
//		ZeroMemory(pcsParty->m_pcsMemberList, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

		pcsParty->m_lMemberListID.MemSetAll();
		//pcsParty->m_pcsMemberList.MemSetAll();

		pcsParty->m_nMaxMember			= 0;
		pcsParty->m_nCurrentMember		= 0;

		pcsParty->m_lHighestMemberLevel	= 0;
		pcsParty->m_lLowerMemberLevel	= 0x00FFFFFF;		//	임의로 제일 큰 레벨
		pcsParty->m_lTotalMemberLevel	= 0;

		pcsParty->m_ulRemoveTimeMSec	= 0;

		pcsParty->m_lCurrentGetItemCID	= AP_INVALID_CID;
		pcsParty->m_lFinishGetItemCID	= AP_INVALID_CID;
	}

	return pcsParty;
}

BOOL AgpmParty::DestroyPartyData(AgpdParty *pcsParty)
{
	pcsParty->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsParty, AGPMPARTY_DATA_TYPE_PARTY);
}

BOOL AgpmParty::SetMaxParty(INT32 nCount)
{
	return m_csPartyAdmin.SetCount(nCount);
}

BOOL AgpmParty::SetMaxPartyRemove(INT32 lCount)
{
	return m_csAdminPartyRemove.SetCount(lCount);
}

//	AgpaParty wrap functions
///////////////////////////////////////////////////////////////////////////////
AgpdParty* AgpmParty::AddParty(INT32 lPID)
{
	if (lPID == AP_INVALID_PARTYID)
		return FALSE;

	AgpdParty* pcsParty = CreatePartyData();
	if (!pcsParty)
		return FALSE;

	pcsParty->m_lID = lPID;

	return m_csPartyAdmin.AddParty(pcsParty);
}

BOOL AgpmParty::RemoveParty(INT32 lPID, BOOL bIsLocked)
{
	if (lPID == AP_INVALID_PARTYID)
		return FALSE;

	AgpdParty* pcsParty = GetParty(lPID);
	if (!pcsParty)
	{
		return FALSE;
	}

	if (!bIsLocked &&
		!pcsParty->m_Mutex.RemoveLock())
		return FALSE;

	m_csPartyAdmin.RemoveParty(lPID);

	EnumCallback(AGPMPARTY_CB_REMOVE_ID, pcsParty, NULL);

	pcsParty->m_Mutex.SafeRelease();

	if (m_csAdminPartyRemove.GetCount() > 0)
	{
		AddRemoveParty(pcsParty);
	}
	else
	{
		DestroyPartyData(pcsParty);
	}

	return TRUE;
}

AgpdParty* AgpmParty::GetParty(INT32 lPID)
{
	return m_csPartyAdmin.GetParty(lPID);
}

AgpdParty* AgpmParty::GetPartyLock(INT32 lPID)
{
	AgpdParty *pcsParty = GetParty(lPID);
	if (!pcsParty)
	{
		return NULL;
	}

	if (!pcsParty->m_Mutex.WLock())
	{
		return NULL;
	}

	return pcsParty;
}

AgpdParty* AgpmParty::GetPartyLock(AgpdCharacter *pcsCharacter)
{
	AgpdParty *pcsParty = GetParty(pcsCharacter);
	if (!pcsParty)
	{
		return NULL;
	}

	if (!pcsParty->m_Mutex.WLock())
	{
		return NULL;
	}

	return pcsParty;
}

INT32 AgpmParty::GetLeaderCID( AgpdCharacter *pcsAgpdCharacter )
{
	AgpdPartyADChar		*pcsAgpdPartyADChar = GetADCharacter( pcsAgpdCharacter );

	if (pcsAgpdPartyADChar)
	{
		AgpdParty			*pcsAgpdParty = pcsAgpdPartyADChar->pcsParty;

		if( pcsAgpdParty != NULL )
		{
			return pcsAgpdParty->m_lMemberListID[0];
		}
	}

	return AP_INVALID_CID;
}

INT32 AgpmParty::GetLeaderCID( AgpdParty *pcsAgpdParty )
{
	if( pcsAgpdParty != NULL )
	{
		return pcsAgpdParty->m_lMemberListID[0];
	}

	return AP_INVALID_CID;
}

BOOL AgpmParty::AddMember(INT32 lPID, INT32 lCID)
{
	AgpdParty		*pcsParty = GetParty(lPID);
	AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	return AddMember(pcsParty, pcsCharacter);
}

BOOL AgpmParty::AddMember(AgpdParty *pcsParty, INT32 lCID)
{
	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	return AddMember(pcsParty, pcsCharacter);
}

//		AddMember
//	Functions
//		- 파티에 멤버를 추가한다.
//	Arguments
//		- pcsParty : 파티
//		- pcsCharacter : 추가할 캐릭터
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::AddMember(AgpdParty* pcsParty, AgpdCharacter* pcsCharacter, BOOL bIsCalcEffectArea)
{
	if (!pcsParty || !pcsCharacter)
		return FALSE;

	// 꽉찼는지 검사한다.
	if (IsFull(pcsParty))
	{
		return FALSE;
	}

	// 이미 멤버로 등록되어 있는지 검사한다.
	if (IsMember(pcsParty, pcsCharacter->m_lID))
	{
		return FALSE;
	}

	pcsParty->m_lMemberListID[pcsParty->m_nCurrentMember] = pcsCharacter->m_lID;
	//pcsParty->m_pcsMemberList[pcsParty->m_nCurrentMember] = pcsCharacter;
	pcsParty->m_nCurrentMember++;
	
	INT32	lMemberLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
	if (lMemberLevel > pcsParty->m_lHighestMemberLevel)
		pcsParty->m_lHighestMemberLevel = lMemberLevel;
	if (lMemberLevel < pcsParty->m_lLowerMemberLevel)
		pcsParty->m_lLowerMemberLevel = lMemberLevel;

	INT32	lMemberUnionRank = m_pagpmUnion->GetUnionRank(pcsCharacter);
	if (lMemberUnionRank > pcsParty->m_lHighestMemberUnionRank)
		pcsParty->m_lHighestMemberUnionRank = lMemberUnionRank;

	pcsParty->m_lTotalMemberLevel += lMemberLevel;

	AgpdPartyADChar *pADChar = GetADCharacter(pcsCharacter);
	if (pADChar)
	{
		pADChar->lPID = pcsParty->m_lID;
		pADChar->pcsParty = pcsParty;
	}

	if (bIsCalcEffectArea)
		SetEffectArea(pcsParty);

	// update factors..
	//if (m_pagpmCharacter)
	//	m_pagpmCharacter->ReCalcCharacterFactors(pcsCharacter);

	EnumCallback(AGPMPARTY_CB_ADD_MEMBER, pcsParty, pcsCharacter);

	return TRUE;
}

BOOL AgpmParty::RemoveMember(INT32 lPID, INT32 lCID, BOOL *pbDestroyParty)
{
	AgpdParty		*pcsParty = GetParty(lPID);

	return RemoveMember(pcsParty, lCID, pbDestroyParty);
}

//		RemoveMember
//	Functions
//		- 파티에서 멤버를 삭제한다.
//			(이 함수를 호출하는 부분에서 이 함수 실행후 IsDestroy()를 통해
//			 파티 인원에 대한 검사를 한다. 그 후에 필요하면 파티를 삭제한다.)
//	Arguments
//		- pcsParty : 파티
//		- pcsCharacter : 삭제할 캐릭터
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::RemoveMember(AgpdParty* pcsParty, INT32 lCID, BOOL *pbIsDestroyParty, BOOL bIsCalcEffectArea)
{
	if (!pcsParty || lCID == AP_INVALID_CID)
		return FALSE;

	// 멤버로 들어가 있는지 아닌지 검사한다.
	if (!IsMember(pcsParty, lCID))
	{
		return FALSE;
	}

	if (pcsParty->m_eDivisionItem == AGPMPARTY_DIVISION_ITEM_SEQUENCE)
	{
		ReSetCurrentGetItemMember(pcsParty,lCID);
		ReSetFinishGetItemMember(pcsParty,lCID);
	}

	AgpdCharacter	*pcsRemoveMember = NULL;
	int i = 0;

	// 멤버로 들어와 있으니 찾아서 삭제한다.
	for (i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (pcsParty->m_lMemberListID[i] == lCID)
		{
			//pcsRemoveMember = pcsParty->m_pcsMemberList[i];
			pcsRemoveMember	= m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);

//			CopyMemory(pcsParty->m_lMemberListID + i, pcsParty->m_lMemberListID + i + 1, 
//					   sizeof(INT32) * (pcsParty->m_nCurrentMember - i - 1));

			if (0 < pcsParty->m_nCurrentMember - i - 1)
				pcsParty->m_lMemberListID.MemCopy(i, &pcsParty->m_lMemberListID[i + 1], pcsParty->m_nCurrentMember - i - 1);

//			CopyMemory(pcsParty->m_pcsMemberList + i, pcsParty->m_pcsMemberList + i + 1, 
//					   sizeof(AgpdCharacter *) * (pcsParty->m_nCurrentMember - i - 1));

//			if (0 < pcsParty->m_nCurrentMember - i - 1)
//				pcsParty->m_pcsMemberList.MemCopy(i, &pcsParty->m_pcsMemberList[i + 1], pcsParty->m_nCurrentMember - i - 1);

			--pcsParty->m_nCurrentMember;

			pcsParty->m_lMemberListID[pcsParty->m_nCurrentMember] = AP_INVALID_CID;
//			pcsParty->m_pcsMemberList[pcsParty->m_nCurrentMember] = NULL;

			break;
		}
	}

	if (!pcsRemoveMember)	// 파티에서 lCID를 못찾았다.
		return FALSE;

	INT32	lMemberLevel = m_pagpmCharacter->GetLevel(pcsRemoveMember);

	pcsParty->m_lTotalMemberLevel -= lMemberLevel;
	if (pcsParty->m_lTotalMemberLevel < 0)
		pcsParty->m_lTotalMemberLevel = 0;

	if (lMemberLevel >= pcsParty->m_lHighestMemberLevel)
		ArrangeHighestLevel(pcsParty);
	if (lMemberLevel <= pcsParty->m_lLowerMemberLevel)
		ArrangeLowerLevel(pcsParty);
	if (m_pagpmUnion->GetUnionRank(pcsRemoveMember) >= pcsParty->m_lHighestMemberUnionRank)
		ArrangeHighestUnionRank(pcsParty);

	// 캐릭터 모듈에 붙여 놓은 데이타를 초기화한다.
	AgpdPartyADChar *pADChar = GetADCharacter(pcsRemoveMember);
	if (pADChar)
	{
		pADChar->lPID = AP_INVALID_PARTYID;
		pADChar->pcsParty = NULL;
	}

	//m_pagpmCharacter->ReCalcCharacterFactors(pcsRemoveMember);

	EnumCallback(AGPMPARTY_CB_REMOVE_MEMBER, pcsParty, pcsRemoveMember);

	if (IsNeedDestroy(pcsParty))
	{
		// 멤버가 넘 적다. 파티를 없앤다.
		DestroyParty(pcsParty);
		*pbIsDestroyParty = TRUE;
	}
	else if (i == 0)
	{
		return ChangeLeader(pcsParty, bIsCalcEffectArea);
	}

	return TRUE;
}

/*
	2005.05.02	By SungHoon
	파티의 리더를 바꾼다.
*/
BOOL AgpmParty::DelegationLeader(AgpdParty *pcsParty, INT32 lOperatorID, INT32 lTargetID)
{
	if (!pcsParty || lOperatorID == AP_INVALID_CID || lTargetID == AP_INVALID_CID)
		return FALSE;

	INT16 lOperatosPos	=	-1;
	INT16 lTargetPos	=	-1;
	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (pcsParty->m_lMemberListID[i] == lOperatorID) lOperatosPos = i;
		if (pcsParty->m_lMemberListID[i] == lTargetID) lTargetPos = i;
	}
	if (lOperatosPos == -1 || lTargetPos == -1 || lOperatosPos == lTargetPos) return FALSE;

//	AgpdCharacter *tempCharacter = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[lTargetPos]);
//	pcsParty->m_pcsMemberList[lTargetPos] = pcsParty->m_pcsMemberList[lOperatosPos];
//	pcsParty->m_pcsMemberList[lOperatosPos] = tempCharacter;

	pcsParty->m_lMemberListID[lOperatosPos] ^= ( pcsParty->m_lMemberListID[lTargetPos] 
				^= ( pcsParty->m_lMemberListID[lOperatosPos] ^= pcsParty->m_lMemberListID[lTargetPos] ) );

	if (pcsParty->m_lCurrentGetItemCID == lOperatorID || pcsParty->m_lCurrentGetItemCID == lTargetID) 
	{
		if (pcsParty->m_lCurrentGetItemCID != pcsParty->m_lFinishGetItemCID)
			GetNextGetItemMember(pcsParty);
		else pcsParty->m_lFinishGetItemCID = pcsParty->m_lMemberListID[pcsParty->m_nCurrentMember-1];
	}

	return ChangeLeader(pcsParty);

}

//		ChangeLeader
//	Functions
//		- 파티에서 리더를 바꾼다. 리더가 바뀌어서 일어나는 모든 일을 여기서 처리한다.
//			(파티원 구성, 펙터 계산 등등....)
//	Arguments
//		- pcsParty : 파티
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::ChangeLeader(AgpdParty *pcsParty, BOOL bIsCalcEffectArea)
{
	if (!pcsParty)
		return FALSE;

	if (bIsCalcEffectArea)
		SetEffectArea(pcsParty);

//	if (!CalcPartyFactor(pcsParty->m_pcsMemberList[0], &pcsParty->m_nMaxMember, &pcsParty->m_csFactorPoint, &pcsParty->m_csFactorPercent))
//	{
//		RemoveParty(pcsParty->m_lID);
//		return FALSE;
//	}

	/*
	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (pcsParty->m_pcsMemberList[i])
		{
			m_pagpmCharacter->ReCalcCharacterFactors(pcsParty->m_pcsMemberList[i]);
		}
	}
	*/

	EnumCallback(AGPMPARTY_CB_CHANGE_LEADER, pcsParty, NULL);

	return TRUE;
}

//		IsMember
//	Functions
//		- pcsParty에 파티원이 들어있는지 검사한다.
//	Arguments
//		- pcsParty : 파티
//		- lCID : 찾을 파티원 아뒤
//	Return value
//		- BOOL : 있는지 없는지 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::IsMember(AgpdParty* pcsParty, INT32 lCID)
{
	if (!pcsParty || lCID == AP_INVALID_CID)
		return FALSE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (pcsParty->m_lMemberListID[i] == lCID)
			return TRUE;
	}

	return FALSE;
}

//		IsFull
//	Functions
//		- pcsParty가 꽉찼는지 검사한다.
//			리더의 카리스마에 의해 영향받는다.
//	Arguments
//		- pcsParty : 파티
//	Return value
//		- BOOL : 찼는지 아니지 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::IsFull(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return TRUE;

	if (pcsParty->m_nCurrentMember < pcsParty->m_nMaxMember)
		return FALSE;
	else
		return TRUE;
}

//		IsNeedDestroy
//	Functions
//		- pcsParty 파티원이 1명미만이면 이 파티는 더이상 존재할 필요가 없다.
//	Arguments
//		- pcsParty : 파티
//	Return value
//		- BOOL : 파티를 없애야 하는지 아닌지 여부 (TRUE면 없앤다.)
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::IsNeedDestroy(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	if (pcsParty->m_nCurrentMember < 2)
		return TRUE;
	else
		return FALSE;
}

//		CalcPartyFactor
//	Functions
//		- pcsParty 의 최대 파티원, 공격, 방어력을 pcsLeader의 카리스마 수치를 보고 세팅한다.
//	Arguments
//		- pcsParty : 파티
//		- pcsLeader : 파티장
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmParty::CalcPartyFactor(AgpdCharacter *pcsLeader, AgpdCharacter *pcsMember, AgpdFactor *pcsFactorPoint, AgpdFactor *pcsFactorPercent)
//	작업 할때 By SungHoon
BOOL AgpmParty::CalcPartyFactor(AgpdCharacter *pcsLeader, AgpdCharacter *pcsMember, AgpdFactor *pcsFactorPoint)
{
//	if (!pcsLeader || pcsMember || !pcsFactorPoint || !pcsFactorPercent)
	if (!pcsLeader || !pcsMember || !pcsFactorPoint)
		return FALSE;

	m_pagpmFactors->InitFactor(pcsFactorPoint);
	//m_pagpmFactors->InitFactor(pcsFactorPercent);

	// 파티 인원 한계 = MIN(20, 4 + {(charisma point - 10) / 5}^1.1 )
//	FLOAT	fCharisma = 0.0f;
//	m_pagpmFactors->GetValue(&pcsLeader->m_csFactor, &fCharisma, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

//	*pnMaxMember = (INT16) (4 + pow(((fCharisma - 10) / 5.0), 1.1));
//
//	if (*pnMaxMember > 20)
//		*pnMaxMember	= 20;

//	*pnMaxMember	= AGPMPARTY_MAX_PARTY_MEMBER;

	INT32	lOriginalDamage		= 0;
	INT32	lOriginalDefense	= 0;
	INT32	lOriginalMaxHP		= 0;
	INT32	lOriginalMaxMP		= 0;

	m_pagpmFactors->GetValue(pcsFactorPoint, &lOriginalDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_pagpmFactors->GetValue(pcsFactorPoint, &lOriginalDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_pagpmFactors->GetValue(pcsFactorPoint, &lOriginalMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactorPoint, &lOriginalMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	INT32	lBonusDamage		= 0;
	INT32	lBonusDefense		= 0;
	INT32	lBonusMaxHP			= 0;
	INT32	lBonusMaxMP			= 0;

/*
	Delete By SungHoon 2005.06.09 By SungHoon

	// 2004.07.22. steeple
	// Leader 에 따라서 적용되는 보너스 수치들을 세팅한다.
	INT32 lLeaderClass = (INT32)AUCHARCLASS_TYPE_NONE;
	m_pagpmFactors->GetValue(&pcsLeader->m_csFactor, &lLeaderClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
	
	switch ((AuCharClassType) lLeaderClass)	{
	case AUCHARCLASS_TYPE_KNIGHT:
		{
			// 1+[플레이어캐릭터의 기본최대공격력]*[파티리더 레벨]/(100+6*(파티리더레벨-캐릭터레벨))

			INT32	lLeaderLevel		= m_pagpmCharacter->GetLevel(pcsLeader);
			INT32	lMemberLevel		= m_pagpmCharacter->GetLevel(pcsMember);
			INT32	lMemberMaxDamage	= 0;

			m_pagpmFactors->GetValue(&pcsMember->m_csFactor, &lMemberMaxDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

			lBonusDamage		= 1 + lMemberMaxDamage * lLeaderLevel / (100 + 6 * abs(lLeaderLevel - lMemberLevel));

			m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		}
		break;

	case AUCHARCLASS_TYPE_RANGER:
		{
			// 1+[플레이어캐릭터의 기본 방어력]*[파티리더 레벨]/(100+3*(파티리더레벨-캐릭터레벨))

			INT32	lLeaderLevel		= m_pagpmCharacter->GetLevel(pcsLeader);
			INT32	lMemberLevel		= m_pagpmCharacter->GetLevel(pcsMember);
			INT32	lMemberDefense		= 0;

			m_pagpmFactors->GetValue(&pcsMember->m_csFactor, &lMemberDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

			lBonusDefense		= 1 + lMemberDefense * lLeaderLevel / (100 + 3 * abs(lLeaderLevel - lMemberLevel));

			m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		}
		break;

	case AUCHARCLASS_TYPE_MAGE:
		{
			// HP 최대 증가 : 5+[플레이어캐릭터의 기본 HP]*[파티리더 레벨]/(500+12*(파티리더레벨-캐릭터레벨))
			// MP 최대 증가 : 6+[플레이어캐릭터의 기본 MP]*[파티리더 레벨]/(500+15*(파티리더레벨-캐릭터레벨))

			INT32	lLeaderLevel		= m_pagpmCharacter->GetLevel(pcsLeader);
			INT32	lMemberLevel		= m_pagpmCharacter->GetLevel(pcsMember);
			INT32	lMemberMaxHP		= 0;
			INT32	lMemberMaxMP		= 0;

			m_pagpmFactors->GetValue(&pcsMember->m_csFactor, &lMemberMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			m_pagpmFactors->GetValue(&pcsMember->m_csFactor, &lMemberMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

			lBonusMaxHP			= 5 + lMemberMaxHP * lLeaderLevel / (500 + 12 * abs(lLeaderLevel - lMemberLevel));
			lBonusMaxMP			= 6 + lMemberMaxMP * lLeaderLevel / (500 + 15 * abs(lLeaderLevel - lMemberLevel));

			m_pagpmFactors->SetValue(pcsFactorPoint, lBonusMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			m_pagpmFactors->SetValue(pcsFactorPoint, lBonusMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		}
		break;
	}
*/
	struct sLevelValue
	{
		INT32 lMin;
		INT32 lMax;
		INT32 lBonusDamage;
		INT32 lBonusDefense;
		INT32 lBonusMaxHP;
		INT32 lBonusMaxMP;
	};
	sLevelValue LevelValue[ 5 ] = 
	{
		{  1,  25,  5,  5,  60,  60 },
		{ 26,  50, 10, 10, 180, 180 },
		{ 51,  75, 15, 15, 240, 240 },
		{ 76, 100, 20, 20, 300, 300 },
		{ 101, 120, 25, 25, 360, 360 },
	};
	INT32 lLeaderClass	=	(INT32)AUCHARCLASS_TYPE_NONE;
	m_pagpmFactors->GetValue(&pcsLeader->m_csFactor, &lLeaderClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
	INT32	lLeaderLevel	=	m_pagpmCharacter->GetLevel(pcsLeader);
	INT32	lMemberLevel	=	m_pagpmCharacter->GetLevel(pcsMember);
	if (lMemberLevel + 20 >= lLeaderLevel)
	{
		for (int i = 0;i < 5;i++)
		{
			if (LevelValue[i].lMin <= lLeaderLevel && lLeaderLevel <= LevelValue[i].lMax)
			{
				switch ((AuCharClassType) lLeaderClass)		
				{
				case AUCHARCLASS_TYPE_KNIGHT:
					{
						lBonusDamage = LevelValue[i].lBonusDamage;
						m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
						m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
					}
					break;
				case AUCHARCLASS_TYPE_RANGER:
					{
						lBonusDefense = LevelValue[i].lBonusDefense;
						m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
					}
					break;
				case AUCHARCLASS_TYPE_SCION:
					{
						lBonusDefense = LevelValue[i].lBonusDefense;
						m_pagpmFactors->SetValue(pcsFactorPoint, lBonusDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
					}
					break;
				case AUCHARCLASS_TYPE_MAGE:
					{
						lBonusMaxHP = LevelValue[i].lBonusMaxHP;
						lBonusMaxMP = LevelValue[i].lBonusMaxMP;
						m_pagpmFactors->SetValue(pcsFactorPoint, lBonusMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
						m_pagpmFactors->SetValue(pcsFactorPoint, lBonusMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
					}
					break;
				}
				break;
			}
		}
	}
	PVOID	pvBuffer[8];
	pvBuffer[0]	= IntToPtr(lOriginalDamage);
	pvBuffer[1]	= IntToPtr(lOriginalDefense);
	pvBuffer[2]	= IntToPtr(lOriginalMaxHP);
	pvBuffer[3]	= IntToPtr(lOriginalMaxMP);
	pvBuffer[4]	= IntToPtr(lBonusDamage);
	pvBuffer[5]	= IntToPtr(lBonusDefense);
	pvBuffer[6]	= IntToPtr(lBonusMaxHP);
	pvBuffer[7]	= IntToPtr(lBonusMaxMP);

	EnumCallback(AGPMPARTY_CB_UPDATE_PARTY_FACTOR, pcsMember, pvBuffer);

	/*
	switch((AuCharClassType)lLeaderClass)
	{
		case AUCHARCLASS_TYPE_KNIGHT:
		{	// 공격력을 높여준다.
			FLOAT fAddValue = (FLOAT)(fCharisma / 5.0f);
			INT32 lOldDmgMin, lOldDmgMax, lNewDmgMin, lNewDmgMax;
			lOldDmgMin = lOldDmgMax = lNewDmgMin = lNewDmgMax = 0;

			// 위에서 InitFactor 를 하기 때문에 굳이 할 필요는 없지만 그냥 한다.
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldDmgMin, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldDmgMax, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

			lNewDmgMin = lOldDmgMin	+ (INT32)fAddValue;
			lNewDmgMax = lOldDmgMax	 + (INT32)fAddValue;

			// 다시 세팅
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewDmgMin, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewDmgMax, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

			break;
		}
		
		case AUCHARCLASS_TYPE_RANGER:
		{	// 방어력을 높여준다.
			FLOAT fAddValue = (FLOAT)(fCharisma / 8.0f);
			INT32 lOldDefense, lNewDefense;
			lOldDefense = lNewDefense = 0;

			// 위에서 InitFactor 를 하기 때문에 굳이 할 필요는 없지만 그냥 한다.
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

			lNewDefense = lOldDefense + (INT32)fAddValue;

			// 다시 세팅
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

			break;
		}

		case AUCHARCLASS_TYPE_MAGE:
		{	// 카리스마를 제외한 캐릭스탯 수치를 높여준다.
			FLOAT fAddValue = (FLOAT)(fCharisma / 6.0f);
			INT32 lOldValue, lNewValue = 0;

			// Con
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
			lNewValue = lOldValue + (INT32)fAddValue;
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);

			// Str
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
			lNewValue = lOldValue + (INT32)fAddValue;
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);

			// Int
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
			lNewValue = lOldValue + (INT32)fAddValue;
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

			// Dex
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
			lNewValue = lOldValue + (INT32)fAddValue;
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

			// Luk
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LUK);
			lNewValue = lOldValue + (INT32)fAddValue;
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LUK);

			// Wis
//			m_pagpmFactors->GetValue(pcsFactorPoint, &lOldValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
			lNewValue = lOldValue + (INT32)fAddValue;
			m_pagpmFactors->SetValue(pcsFactorPoint, lNewValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);

			break;
		}
	}
	*/

	/*
	*pnMaxMember = 3 + 10 * (2 * 2) * (lCharisma - 20) / (lCharisma + 0.0);
	if (*pnMaxMember > 32)
		*pnMaxMember = 32;
	*/

	//	파티 공격력 & 방어력 세팅
	///////////////////////////////////////////////////////////////////////////

	/*
	INT32	lLeaderClass = 0;
	if (!m_pagpmFactors->GetValue(&pcsLeader->m_csFactor, &lLeaderClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS))
		return FALSE;

	switch (lLeaderClass) {
	case AUCHARCLASS_TYPE_KNIGHT:
		{
			INT32	lDamage = (INT32) (lCharisma / 5);
			if (lDamage > 0)
			{
				AgpdFactorDamage *pcsFactorDamage
					= (AgpdFactorDamage *) m_pagpmFactors->SetFactor(pcsFactorPercent, NULL, AGPD_FACTORS_TYPE_DAMAGE);
				if (!pcsFactorDamage)
					return FALSE;

				pcsFactorDamage->csValue[AGPD_FACTORS_DAMAGE_TYPE_MIN].lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
				pcsFactorDamage->csValue[AGPD_FACTORS_DAMAGE_TYPE_MAX].lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
			}
		}
		break;

	case AUCHARCLASS_TYPE_RANGER:
		{
		}
		break;

	case AUCHARCLASS_TYPE_SCION:
		{
		}
		break;

	case AUCHARCLASS_TYPE_MAGE:
		{
		}
		break;

	default:
		return FALSE;
		break;
	}
	*/

	/*
	AgpdFactorDamage *pcsFactorDamage = (AgpdFactorDamage *) m_pagpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DAMAGE);
	if (!pcsFactorDamage)
		return FALSE;

	pcsFactorDamage->csValue[AGPD_FACTORS_DAMAGE_TYPE_MAX].lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = 10;

	AgpdFactorDefense *pcsFactorDefense = (AgpdFactorDefense *) m_pagpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
	if (!pcsFactorDefense)
		return FALSE;

	pcsFactorDefense->csValue[AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT].lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = 10;
	*/

	return TRUE;
}

AgpdParty *AgpmParty::CreateParty(INT32 lPID, INT32 lPartyLeaderID, INT32 lMemberID)
{
	AgpdCharacter *pcsPartyLeader = m_pagpmCharacter->GetCharacter(lPartyLeaderID);
	AgpdCharacter *pcsMember	  = m_pagpmCharacter->GetCharacter(lMemberID);

	return CreateParty(lPID, pcsPartyLeader, pcsMember);
}

//		CreateParty
//	Functions
//		- 파티를 생성한다. pcsPartyLeader가 pcsMember에게 파티를 요청한경우 이 함수에서 처리한다.
//			1. 파티 데이타를 생성한다.
//			2. pcsPartyLeader의 카리스마를 보고 최대 파티원, 파티 방어, 공격력을 세팅한다.
//			3. pcsMember를 멤버로 추가한다.
//	Arguments
//		- pcsParty : 파티
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
AgpdParty *AgpmParty::CreateParty(INT32 lPID, AgpdCharacter *pcsPartyLeader, AgpdCharacter *pcsMember)
{
	if (lPID == AP_INVALID_PARTYID || !pcsPartyLeader || !pcsMember)
		return NULL;

	AgpdParty *pcsParty = CreateParty(lPID, pcsPartyLeader, pcsMember, AGPMPARTY_MAX_PARTY_MEMBER);

	return pcsParty;
}

AgpdParty *AgpmParty::CreateParty(INT32 lPID, INT32 lLeaderID, INT32 lMemberID, INT16 nMaxMember)
{
	AgpdCharacter *pcsPartyLeader = m_pagpmCharacter->GetCharacter(lLeaderID);
	AgpdCharacter *pcsMember = m_pagpmCharacter->GetCharacter(lMemberID);

	return CreateParty(lPID, pcsPartyLeader, pcsMember, nMaxMember);
}

AgpdParty *AgpmParty::CreateParty(INT32 lPID, AgpdCharacter *pcsPartyLeader, AgpdCharacter *pcsMember, INT16 nMaxMember)
{
	if (lPID == AP_INVALID_PARTYID || !pcsPartyLeader || !pcsMember || nMaxMember < 1)
		return NULL;

	AgpdParty *pcsParty = AddParty(lPID);
	if (!pcsParty)
		return NULL;

	pcsParty->m_nMaxMember		= nMaxMember;

	pcsParty->m_eCalcExpType	= AGPMPARTY_EXP_TYPE_BY_DAMAGE;		//	2005.05.17. By SungHoon	
//	pcsParty->m_eCalcExpType	= AGPMPARTY_EXP_TYPE_BY_COMPLEX;

	pcsParty->m_eDivisionItem	= AGPMPARTY_DIVISION_ITEM_DAMAGE;

	// 이제 리더부터 멤버를 추가한다.
	if (!AddMember(pcsParty, pcsPartyLeader, FALSE))
	{
		RemoveParty(pcsParty->m_lID);
		return NULL;
	}
	if (!AddMember(pcsParty, pcsMember))
	{
		RemoveParty(pcsParty->m_lID);
		return NULL;
	}

	EnumCallback(AGPMPARTY_CB_CREATE, pcsParty, NULL);

	return pcsParty;
}

BOOL AgpmParty::DestroyParty(INT32 lPID)
{
	return DestroyParty(GetParty(lPID));
}

//		DestroyParty
//	Functions
//		- pcsParty를 없앤다.
//			현재 멤버인 모든 캐릭터의 파티 데이타를 초기화하고 펙터를 업데이트한다.
//	Arguments
//		- pcsParty : 파티
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmParty::DestroyParty(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	EnumCallback(AGPMPARTY_CB_REMOVE, pcsParty, NULL);

	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (pcsMember)
		{
			AgpdPartyADChar *pcsADChar = GetADCharacter(pcsMember);
			if (pcsADChar)
			{
				pcsADChar->lPID = AP_INVALID_PARTYID;
				pcsADChar->pcsParty = NULL;
			}

			/*
			if (m_pagpmCharacter)
				m_pagpmCharacter->ReCalcCharacterFactors(pcsParty->m_pcsMemberList[i]);
			*/

			pcsMember->m_Mutex.Release();

			pcsParty->m_lMemberListID[i]	= AP_INVALID_CID;
			//pcsParty->m_pcsMemberList[i]	= NULL;
		}
	}

	return RemoveParty(pcsParty->m_lID, TRUE);
}

/*
	2005.04.15	By SungHoon
	이번에 아이템을 획들할 Player의 
*/
INT32 AgpmParty::GetNextGetItemMember(INT32 lPID, BOOL bTemp)
{
	AgpdParty		*pcsParty = GetParty(lPID);

	return GetNextGetItemMember(pcsParty, bTemp);

}

INT32 AgpmParty::GetNextGetItemMember(AgpdParty *pcsParty, BOOL bTemp)
{
	int i = 0;
	for (i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (pcsParty->m_lMemberListID[i] == pcsParty->m_lCurrentGetItemCID) break;
		if (pcsParty->m_lMemberListID[i] == pcsParty->m_lFinishGetItemCID) break;
	}
	if (i == pcsParty->m_nCurrentMember)		// 예외상황 : 현재 아이템 받을 파티 멤버가 존재하지 않는다.
	{
		if(bTemp == FALSE)
		{
			pcsParty->m_lCurrentGetItemCID = pcsParty->m_lMemberListID[1];
			pcsParty->m_lFinishGetItemCID = pcsParty->m_lMemberListID[pcsParty->m_nCurrentMember-1];
		}

		return (pcsParty->m_lMemberListID[0]);
	}
	if ((pcsParty->m_lMemberListID[i] == pcsParty->m_lFinishGetItemCID) || ((i+1) == pcsParty->m_nCurrentMember))// 마지막 멤버가 받으면 다음은 처음 멤버다.
	{		// 새로 순서가 시작된다
		if(bTemp == FALSE)
		{
			pcsParty->m_lCurrentGetItemCID = pcsParty->m_lMemberListID[0];
			pcsParty->m_lFinishGetItemCID = pcsParty->m_lMemberListID[pcsParty->m_nCurrentMember-1];
		}

		return (pcsParty->m_lMemberListID[i]);
	}

	if(bTemp == FALSE)
		pcsParty->m_lCurrentGetItemCID = pcsParty->m_lMemberListID[i+1];

	return (pcsParty->m_lMemberListID[i]);
}

/*
	2005.05.18 By SungHoon
	현재 아이템 받을 사용자가 나갈 경우 다음 사용자를 아이템 받을 사용자로 설정한다.
*/
VOID AgpmParty::ReSetCurrentGetItemMember( AgpdParty *pcsParty, INT32 lCID)
{
	if (pcsParty->m_lCurrentGetItemCID != lCID) return;
	GetNextGetItemMember(pcsParty);
}

/*
	2005.05.18 By SungHoon
	마지막 아이템 받을 사용자가 나갈 경우 이전 사용자를 마지막 아이템 받을 사용자로 설정한다.
*/
VOID AgpmParty::ReSetFinishGetItemMember(AgpdParty *pcsParty, INT32 lCID)
{
	if (pcsParty->m_lFinishGetItemCID != lCID) return;

	for (int i = pcsParty->m_nCurrentMember -1 ;i >= 0; i--)
	{
		if (pcsParty->m_lMemberListID[i] == pcsParty->m_lFinishGetItemCID)
		{
			if (i > 0)
			{
				pcsParty->m_lFinishGetItemCID = pcsParty->m_lMemberListID[i-1];
				return;
			}
		}
	}
}

BOOL AgpmParty::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lPartyID		= AP_INVALID_PARTYID;
	INT8		cMaxMember		= 0;
	INT8		cCurrentMember	= 0;
	INT32		lMemberID		= AP_INVALID_CID;
	INT32		*plMemberList	= NULL;
	INT16		nLenMemberList	= 0;
	PVOID		pvPacketFactor	= NULL;
	//PVOID		*pvPacketFactorPoint	= NULL;
	//PVOID		*pvPacketFactorPercent	= NULL;

	PVOID		pvPacketEffectArea	= NULL;
	INT8		cCalcExpType	= AGPMPARTY_EXP_TYPE_BY_DAMAGE;
	INT8		cDivisionItem	= AGPMPARTY_DIVISION_ITEM_DAMAGE;

	PVOID		pvPacketBonusStats	= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lPartyID,
						&cMaxMember,
						&cCurrentMember,
						&lMemberID,
						&plMemberList, &nLenMemberList,
						&pvPacketFactor,
						&pvPacketEffectArea,
						&cCalcExpType,
						&pvPacketBonusStats,
						&cDivisionItem
						);

	switch (cOperation){
	case AGPMPARTY_PACKET_OPERATION_ADD:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			// 파티 생성은 여기서 관여하지 않는다. 여기서는 이미 만들어진 파티의 정보를 몽땅 받아서 새로 만드는 일만 한다.
			// 파티 생성은 서버쪽 고유 기능이다.
			if (lPartyID == AP_INVALID_PARTYID ||
				cMaxMember == 0 ||
				cCurrentMember < 2 ||
				plMemberList == NULL/* ||
				pvPacketFactor == NULL*/)
				return FALSE;

//			AgpdFactor	csFactorPoint;
//			AgpdFactor	csFactorPercent;
//
//			m_pagpmFactors->InitFactor(&csFactorPoint);
//			m_pagpmFactors->InitFactor(&csFactorPercent);
//
//			if (!m_pagpmFactors->ReflectPacket(&csFactorPoint, pvPacketFactor, *(INT16 *) pvPacketFactor))
//				return FALSE;

//			AgpdCharacter	*pcsMember[AGPMPARTY_MAX_PARTY_MEMBER];
//			ZeroMemory(pcsMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);
//
//			INT32	lTempMemberList[AGPMPARTY_MAX_PARTY_MEMBER];
//			ZeroMemory(lTempMemberList, sizeof(INT32) * AGPMPARTY_MAX_PARTY_MEMBER);

			ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER>	pcsMember;
			ApSafeArray<INT32, AGPMPARTY_MAX_PARTY_MEMBER>				lTempMemberList;

			pcsMember.MemSetAll();
			lTempMemberList.MemSetAll();
			
			if (cCurrentMember > 0 && cCurrentMember <= AGPMPARTY_MAX_PARTY_MEMBER)
				//CopyMemory(lTempMemberList, plMemberList, sizeof(INT32) * cCurrentMember);
				lTempMemberList.MemCopy(0, plMemberList, cCurrentMember);

			if (!m_pagpmCharacter->GetCharacterLock(&lTempMemberList[0], 2, &pcsMember[0]))
				return FALSE;

			if (!pstCheckArg->bReceivedFromServer && pcsMember[0] && m_pagpmCharacter->IsAllBlockStatus(pcsMember[0]) ||
				!pstCheckArg->bReceivedFromServer && pcsMember[1] && m_pagpmCharacter->IsAllBlockStatus(pcsMember[1]))
			{
				if (pcsMember[0])
					pcsMember[0]->m_Mutex.Release();
				if (pcsMember[1])
					pcsMember[1]->m_Mutex.Release();

				return FALSE;
			}

			AgpdParty	*pcsParty	= NULL;

//			if (plMemberList[0] == pcsMember[0]->m_lID)
//				pcsParty = CreateParty(lPartyID, pcsMember[0], pcsMember[1], cMaxMember, &csFactorPoint, &csFactorPercent);
//			else
//				pcsParty = CreateParty(lPartyID, pcsMember[1], pcsMember[0], cMaxMember, &csFactorPoint, &csFactorPercent);

			if (plMemberList[0] == pcsMember[0]->m_lID)
				pcsParty = CreateParty(lPartyID, pcsMember[0], pcsMember[1], cMaxMember);
			else
				pcsParty = CreateParty(lPartyID, pcsMember[1], pcsMember[0], cMaxMember);

			if (!pcsParty || !pcsParty->m_Mutex.WLock())
			{
				pcsMember[0]->m_Mutex.Release();
				pcsMember[1]->m_Mutex.Release();
				return FALSE;
			}

			pcsParty->m_eCalcExpType	= (AgpmPartyCalcExpType) cCalcExpType; 
			pcsParty->m_eDivisionItem = (AgpmPartyOptionDivisionItem) cDivisionItem; 

//			m_pagpmFactors->DestroyFactor(&csFactorPoint);
//			m_pagpmFactors->DestroyFactor(&csFactorPercent);
			
			pcsMember[0]->m_Mutex.Release();
			pcsMember[1]->m_Mutex.Release();

			int j = 2;
			for (int i = 2; i < cCurrentMember; ++i)
			{
				pcsMember[j] = m_pagpmCharacter->GetCharacterLock(plMemberList[i]);
				if (pcsMember[j])
				{
					BOOL	bAddMember	= AddMember(pcsParty, pcsMember[j], FALSE);

					pcsMember[j]->m_Mutex.Release();

					if (!bAddMember)
						++j;
				}
			}

			OnOperationUpdateEffectArea(pcsParty, pvPacketEffectArea);

			EnumCallback(AGPMPARTY_CB_ADD_PARTY, pcsParty, NULL);

			pcsParty->m_Mutex.Release();
		}
		break;

	case AGPMPARTY_PACKET_OPERATION_REMOVE:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			if (lPartyID == AP_INVALID_PARTYID)
				return FALSE;

			AgpdParty	*pcsParty = GetPartyLock(lPartyID);
			if (!pcsParty)
				return FALSE;

			BOOL	bResult = DestroyParty(pcsParty);

			//pcsParty->m_Mutex.Release();

			//RemoveParty(lPartyID);

			if (!bResult)
				return FALSE;
		}
		break;

	case AGPMPARTY_PACKET_OPERATION_UPDATE:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			if (lPartyID == AP_INVALID_PARTYID)
				return FALSE;

			AgpdParty *pcsParty = GetPartyLock(lPartyID);
			if (!pcsParty)
				return FALSE;

			if (cMaxMember != 0)
			{
				pcsParty->m_nMaxMember = (INT16) cMaxMember;
			}

//			if (pvPacketFactor != NULL)
//			{
//				if (!m_pagpmFactors->ReflectPacket(&pcsParty->m_csFactorPoint, pvPacketFactor, 0))
//				{
//					pcsParty->m_Mutex.Release();
//					return FALSE;
//				}
//
//				EnumCallback(AGPMPARTY_CB_UPDATE_FACTOR, pcsParty, NULL);
//
//				/*
//				// 파티 펙터가 바뀌었으니 파티원들 펙터를 업데이트시켜준ㄴ다.
//				for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
//				{
//					if (pcsParty->m_pcsMemberList[i])
//					{
//						m_pagpmCharacter->ReCalcCharacterFactors(pcsParty->m_pcsMemberList[i]);
//					}
//				}
//				*/
//			}

			OnOperationUpdateEffectArea(pcsParty, pvPacketEffectArea);
			OnOperationUpdateBonusStats(pcsParty, pvPacketBonusStats);

			pcsParty->m_Mutex.Release();
		}
		break;

	case AGPMPARTY_PACKET_OPERATION_ADD_MEMBER:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			if (lPartyID == AP_INVALID_PARTYID || lMemberID == AP_INVALID_CID)
				return FALSE;

			AgpdParty *pcsParty = GetPartyLock(lPartyID);
			if (!pcsParty)
				return FALSE;

			AgpdCharacter	*pcsMember = m_pagpmCharacter->GetCharacterLock(lMemberID);
			if (!pcsMember || !pstCheckArg->bReceivedFromServer && m_pagpmCharacter->IsAllBlockStatus(pcsMember))
			{
				if (pcsMember)
					pcsMember->m_Mutex.Release();

				pcsParty->m_Mutex.Release();
				return FALSE;
			}

			BOOL bResult = AddMember(pcsParty, pcsMember, FALSE);

			OnOperationUpdateEffectArea(pcsParty, pvPacketEffectArea);

			pcsMember->m_Mutex.Release();
			pcsParty->m_Mutex.Release();

			return bResult;
		}
		break;

	case AGPMPARTY_PACKET_OPERATOIN_REMOVE_MEMBER:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			if (lPartyID == AP_INVALID_PARTYID || lMemberID == AP_INVALID_CID)
				return FALSE;

			BOOL	bIsDestroyParty	= FALSE;

			EnumCallback(AGPMPARTY_CB_CHECK_DESTROY_PARTY, IntToPtr(lMemberID), &bIsDestroyParty);

			if (bIsDestroyParty)
			{
				DestroyParty(lPartyID);
			}
			else
			{
				AgpdParty *pcsParty = GetPartyLock(lPartyID);
				if (!pcsParty)
					return FALSE;

				AgpdCharacter	*pcsMember = m_pagpmCharacter->GetCharacterLock(lMemberID);
				if (!pcsMember || !pstCheckArg->bReceivedFromServer && m_pagpmCharacter->IsAllBlockStatus(pcsMember))
				{
					if (pcsMember)
						pcsMember->m_Mutex.Release();

					pcsParty->m_Mutex.Release();
					return FALSE;
				}

				BOOL bDestroyParty = FALSE;

				RemoveMember(pcsParty, pcsMember->m_lID, &bDestroyParty, FALSE);

				pcsMember->m_Mutex.Release();

				if (!bDestroyParty)
					pcsParty->m_Mutex.Release();
			}

			return TRUE;
		}
		break;

	case AGPMPARTY_PACKET_OPERATION_UPDATE_EXP_TYPE:
		{
//			if ((AgpmPartyCalcExpType) cCalcExpType == AGPMPARTY_EXP_TYPE_BY_NONE)
//				return TRUE;

			AgpdParty *pcsParty = GetPartyLock(lPartyID);
			if (!pcsParty)
				return FALSE;
			if (GetLeaderCID(pcsParty) != lMemberID) return FALSE;

			if (pcsParty->m_eCalcExpType != (AgpmPartyCalcExpType) cCalcExpType)
			{
				pcsParty->m_eCalcExpType	= (AgpmPartyCalcExpType) cCalcExpType;

				EnumCallback(AGPMPARTY_CB_UPDATE_EXP_TYPE, pcsParty, &lMemberID);
			}

			pcsParty->m_Mutex.Release();
		}
		break;
//	2005.04.19	By SungHoon
//	아이템 획득 방식 변경에 따른 작업
	case AGPMPARTY_PACKET_OPERATION_UPDATE_ITEM_DIVISION :
		{
			AgpdParty *pcsParty = GetPartyLock(lPartyID);
			if (!pcsParty)
				return FALSE;
			if (GetLeaderCID(pcsParty) != lMemberID) return FALSE;

			if (pcsParty->m_eDivisionItem != (AgpmPartyOptionDivisionItem) cDivisionItem)
			{
				pcsParty->m_eDivisionItem = (AgpmPartyOptionDivisionItem) cDivisionItem;
				EnumCallback(AGPMPARTY_CB_UPDATE_ITEM_DIVISION, pcsParty, &lMemberID);
			}

			pcsParty->m_Mutex.Release();
		}
		break;

	}

	return TRUE;
}

BOOL AgpmParty::OnOperationUpdateEffectArea(AgpdParty *pcsParty, PVOID pvPacketEffectArea)
{
	if (!pcsParty || !pvPacketEffectArea)
		return FALSE;

	m_csPacketEffectArea.GetField(FALSE, pvPacketEffectArea, 0,
								&pcsParty->m_bIsInEffectArea[0],
								&pcsParty->m_bIsInEffectArea[1],
								&pcsParty->m_bIsInEffectArea[2],
								&pcsParty->m_bIsInEffectArea[3],
								&pcsParty->m_bIsInEffectArea[4]);

	return EnumCallback(AGPMPARTY_CB_RESET_EFFECT_AREA, pcsParty, NULL);
}

BOOL AgpmParty::OnOperationUpdateBonusStats(AgpdParty *pcsParty, PVOID pvPacketBonusStats)
{
	if (!pcsParty || !pvPacketBonusStats)
		return FALSE;

	INT8	cDamage		= 0;
	INT8	cDefense	= 0;
	INT16	nMaxHP		= 0;
	INT16	nMaxMP		= 0;

	m_csPacketBonusStats.GetField(FALSE, pvPacketBonusStats, 0,
									&cDamage,
									&cDefense,
									&nMaxHP,
									&nMaxMP);

	PVOID	pvBuffer[4];
	pvBuffer[0]	= (PVOID) cDamage;
	pvBuffer[1]	= (PVOID) cDefense;
	pvBuffer[2]	= (PVOID) nMaxHP;
	pvBuffer[3]	= (PVOID) nMaxMP;

	return EnumCallback(AGPMPARTY_CB_UPDATE_BONUS_STATS, pcsParty, pvBuffer);
}

BOOL AgpmParty::SetCBRemoveParty(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_REMOVE, pfCallback, pClass);
}

BOOL AgpmParty::SetCBRemoveID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_REMOVE_ID, pfCallback, pClass);
}

BOOL AgpmParty::SetCBChangeLeader(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_CHANGE_LEADER, pfCallback, pClass);
}

BOOL AgpmParty::SetCBAddMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_ADD_MEMBER, pfCallback, pClass);
}

BOOL AgpmParty::SetCBRemoveMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_REMOVE_MEMBER, pfCallback, pClass);
}

BOOL AgpmParty::SetCBSyncRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_SYNC_REMOVE_CHARACTER, pfCallback, pClass);
}

BOOL AgpmParty::SetCBCreateParty(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_CREATE, pfCallback, pClass);
}

BOOL AgpmParty::SetCBResetEffectArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_RESET_EFFECT_AREA, pfCallback, pClass);
}

BOOL AgpmParty::SetCBAddParty(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_ADD_PARTY, pfCallback, pClass);
}

BOOL AgpmParty::SetCBUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_UPDATE_FACTOR, pfCallback, pClass);
}

BOOL AgpmParty::SetCBCheckDestroyParty(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_CHECK_DESTROY_PARTY, pfCallback, pClass);
}

/*
	2005.06.15. Rename By SungHoon 
	SetCBLeaderLevelUp->SetCBCharLevelUp
*/
BOOL AgpmParty::SetCBCharLevelUp(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_CHAR_LEVEL_UP, pfCallback, pClass);
}

BOOL AgpmParty::SetCBUpdateExpType(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_UPDATE_EXP_TYPE, pfCallback, pClass);
}

BOOL AgpmParty::SetCBUpdateBonusStats(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_UPDATE_BONUS_STATS, pfCallback, pClass);
}

BOOL AgpmParty::SetCBUpdatePartyFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_UPDATE_PARTY_FACTOR, pfCallback, pClass);
}

/*
	2005.04.19	Start By SungHoon
	아이템 분배 방식이 변경될때 불릴 CallBack을 등록한다.
*/
BOOL AgpmParty::SetCBUpdateItemDivision(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPARTY_CB_UPDATE_ITEM_DIVISION, pfCallback, pClass);
}

INT16 AgpmParty::AttachPartyData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMPARTY_DATA_TYPE_PARTY, nDataSize, pfConstructor, pfDestructor);
}

PVOID AgpmParty::MakePacketParty(AgpdParty *pcsParty, INT16 *pnPacketLength)
{
	if (!pcsParty || !pnPacketLength)
		return NULL;

	INT8	cOperation = AGPMPARTY_PACKET_OPERATION_ADD;

//	PVOID	pvPacketFactor = m_pagpmFactors->MakePacketFactors(&pcsParty->m_csFactorPoint);

	PVOID	pvPacketEffectArea	= m_csPacketEffectArea.MakePacket(FALSE, NULL, 0,
																	&pcsParty->m_bIsInEffectArea[0],
																	&pcsParty->m_bIsInEffectArea[1],
																	&pcsParty->m_bIsInEffectArea[2],
																	&pcsParty->m_bIsInEffectArea[3],
																	&pcsParty->m_bIsInEffectArea[4]);

	INT16	nMemberListSize = sizeof(INT32) * pcsParty->m_nCurrentMember;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
															&cOperation,
															&pcsParty->m_lID,
															&pcsParty->m_nMaxMember,
															&pcsParty->m_nCurrentMember,
															NULL,
															&pcsParty->m_lMemberListID[0], &nMemberListSize,
															NULL, // pvPacketFactor,
															pvPacketEffectArea,
															&pcsParty->m_eCalcExpType,
															NULL,
															&pcsParty->m_eDivisionItem);		//	2005.11.14. By SungHoon

	if (pvPacketEffectArea)
		m_pagpmFactors->m_csPacket.FreePacket(pvPacketEffectArea);

//	if (pvPacketFactor)
//		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	return pvPacket;
}

BOOL AgpmParty::CBRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmParty		*pThis			= (AgpmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdPartyADChar	*pcsPartyADChar	= pThis->GetADCharacter(pcsCharacter);

	if (pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveChar"));

	BOOL	bDestroyParty = FALSE;

	AgpdParty	*pcsParty	= pThis->GetPartyLock(pcsPartyADChar->lPID);
	if (!pcsParty)
		return FALSE;

	pThis->EnumCallback(AGPMPARTY_CB_SYNC_REMOVE_CHARACTER, pcsParty, pcsCharacter);

	pThis->RemoveMember(pcsParty, pcsCharacter->m_lID, &bDestroyParty);

	if (!bDestroyParty)
	{
		pcsParty->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgpmParty::CBCheckValidNormalAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmParty			*pThis				= (AgpmParty *)				pClass;
	AgpdCharacter		*pcsAttackChar		= (AgpdCharacter *)			pData;
	AgpdCharacter		*pcsTargetChar		= (AgpdCharacter *)			(((PVOID *) pCustData)[0]);
	BOOL				bForceAttack		= (BOOL)					PtrToInt((((PVOID *) pCustData)[1]));

	if (!pcsTargetChar)
		return FALSE;

	if (pThis->m_pagpmCharacter->IsAllBlockStatus(pcsAttackChar) ||
		pThis->m_pagpmCharacter->IsAllBlockStatus(pcsTargetChar))
		return FALSE;

	// 2005.01.27. steeple
	// PvP 의 도입으로 인해서 이제는 검사할 필요가 없다.

	//if (bForceAttack)
	//{
	//	// 강제 공격이라 하더라도 파티원끼리는 공격할 수 없다.
	//	AgpdPartyADChar	*pcsAttackCharPartyData	= pThis->GetADCharacter(pcsAttackChar);
	//	AgpdPartyADChar	*pcsTargetCharPartyData	= pThis->GetADCharacter(pcsTargetChar);
	//	if (!pcsAttackCharPartyData || !pcsTargetCharPartyData)
	//		return FALSE;

	//	// PartyID 가 Valid 할 때만 체크 2005.01.27. steeple
	//	if (pcsAttackCharPartyData->lPID != AP_INVALID_PARTYID &&
	//		pcsTargetCharPartyData->lPID != AP_INVALID_PARTYID &&
	//		pcsAttackCharPartyData->lPID == pcsTargetCharPartyData->lPID)
	//		return FALSE;
	//}

	return TRUE;
}

BOOL AgpmParty::CBCharLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmParty* pThis = (AgpmParty*)pClass;
	
	AgpdPartyADChar* pcsAttachedParty = pThis->GetADCharacter(pcsCharacter);
	if(!pcsAttachedParty)
		return TRUE;

	AgpdParty* pcsParty = pThis->GetParty(pcsCharacter);
	if(!pcsParty)
		return FALSE;

	if (pcsParty->m_Mutex.WLock())
	{
		//pThis->CalcPartyFactor(pcsCharacter, &pcsParty->m_nMaxMember, &pcsParty->m_csFactorPoint, &pcsParty->m_csFactorPercent);
		pThis->EnumCallback(AGPMPARTY_CB_CHAR_LEVEL_UP, pcsParty, pcsCharacter);

		// 2007.04.10. steeple
		pThis->ArrangeLevel(pcsParty);

		//pThis->ArrangeHighestLevel(pcsParty);
		//pThis->ArrangeLowerLevel(pcsParty);

		pcsParty->m_Mutex.Release();
	}

	return TRUE;
}

PVOID AgpmParty::MakePacketAddMember(AgpdParty *pcsParty, INT32 lMemberID, INT16 *pnPacketLength)
{
	if (!pcsParty)
		return NULL;

	PVOID	pvPacketEffectArea	= m_csPacketEffectArea.MakePacket(FALSE, pnPacketLength, 0,
									(pcsParty->m_lMemberListID[0] == lMemberID) ? &pcsParty->m_bIsInEffectArea[0] : NULL,
									(pcsParty->m_lMemberListID[1] == lMemberID) ? &pcsParty->m_bIsInEffectArea[1] : NULL,
									(pcsParty->m_lMemberListID[2] == lMemberID) ? &pcsParty->m_bIsInEffectArea[2] : NULL,
									(pcsParty->m_lMemberListID[3] == lMemberID) ? &pcsParty->m_bIsInEffectArea[3] : NULL,
									(pcsParty->m_lMemberListID[4] == lMemberID) ? &pcsParty->m_bIsInEffectArea[4] : NULL);

	if (!pvPacketEffectArea)
		return NULL;

	INT8	cOperation = AGPMPARTY_PACKET_OPERATION_ADD_MEMBER;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
												&cOperation,
												&pcsParty->m_lID,
												NULL,
												NULL,
												&lMemberID,
												NULL,
												NULL,
												pvPacketEffectArea,
												NULL,
												NULL,
												NULL);

	m_csPacketEffectArea.FreePacket(pvPacketEffectArea);

	return pvPacket;
}

PVOID AgpmParty::MakePacketRemoveMember(INT32 lPartyID, INT32 lMemberID, INT16 *pnPacketLength)
{
	INT8	cOperation = AGPMPARTY_PACKET_OPERATOIN_REMOVE_MEMBER;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
												&cOperation,
												&lPartyID,
												NULL,
												NULL,
												&lMemberID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	return pvPacket;
}

PVOID AgpmParty::MakePacketRemoveParty(INT32 lPartyID, INT16 *pnPacketLength)
{
	INT8	cOperation = AGPMPARTY_PACKET_OPERATION_REMOVE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
												&cOperation,
												&lPartyID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	return pvPacket;
}

PVOID AgpmParty::MakePacketSyncMemberHP(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if (!pvPacketFactor)
		return NULL;

	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket		= m_pagpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																		&cOperation,					// Operation
																		&pcsCharacter->m_lID,			// Character ID
																		NULL,							// Character Template ID
																		NULL,							// Game ID
																		NULL,							// Character Status
																		NULL,							// Move Packet
																		NULL,							// Action Packet
																		pvPacketFactor,					// Factor Packet
																		NULL,							// llMoney
																		NULL,							// bank money
																		NULL,							// cash
																		NULL,							// character action status
																		NULL,							// character criminal status
																		NULL,							// attacker id (정당방위 설정에 필요)
																		NULL,							// 새로 생성되서 맵에 들어간넘인지 여부
																		NULL,							// region index
																		NULL,							// social action index
																		NULL,							// special status
																		NULL,							// is transform status
																		NULL,							// skill initialization text
																		NULL,							// face index
																		NULL,							// hair index
																		NULL,							// Option Flag
																		NULL,							// bank size
																		NULL,							// event status flag
																		NULL,							// remained criminal status time
																		NULL,							// remained murderer point time
																		NULL,							// nick name
																		NULL,							// gameguard
																		NULL							// last killed time in battlesquare
																		);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	return pvPacket;
}

PVOID AgpmParty::MakePacketSyncMemberHPMax(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactorsCharHPMax(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
		return NULL;

	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket		= m_pagpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																		&cOperation,							// Operation
																		&pcsCharacter->m_lID,					// Character ID
																		NULL,									// Character Template ID
																		NULL,									// Game ID
																		NULL,									// Character Status
																		NULL,									// Move Packet
																		NULL,									// Action Packet
																		pvPacketFactor,							// Factor Packet
																		NULL,									// llMoney
																		NULL,									// bank money
																		NULL,									// cash
																		NULL,									// character action status
																		NULL,									// character criminal status
																		NULL,									// attacker id (정당방위 설정에 필요)
																		NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																		NULL,									// region index
																		NULL,									// social action index
																		NULL,									// special status
																		NULL,									// is transform status
																		NULL,									// skill initialization text
																		NULL,									// face index
																		NULL,									// hair index
																		NULL,									// Option Flag
																		NULL,									// bank size
																		NULL,									// event status flag
																		NULL,									// remained criminal status time
																		NULL,									// remained murderer point time
																		NULL,									// nick name
																		NULL,									// gameguard
																		NULL									// last killed time in battlesquare
																		);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	return pvPacket;
}

PVOID AgpmParty::MakePacketEffectArea(AgpdParty *pcsParty, BOOL *pbOldList, INT16 *pnPacketLength)
{
	if (!pcsParty || !pnPacketLength)
		return NULL;

	PVOID	pvPacketEffectArea	= NULL;

	if (pbOldList)
	{
		if (pbOldList[0] != pcsParty->m_bIsInEffectArea[0] ||
			pbOldList[1] != pcsParty->m_bIsInEffectArea[1] ||
			pbOldList[2] != pcsParty->m_bIsInEffectArea[2] ||
			pbOldList[3] != pcsParty->m_bIsInEffectArea[3] ||
			pbOldList[4] != pcsParty->m_bIsInEffectArea[4])
		{
			pvPacketEffectArea	= m_csPacketEffectArea.MakePacket(FALSE, pnPacketLength, 0,
							(pbOldList[0] == pcsParty->m_bIsInEffectArea[0]) ? NULL : &pcsParty->m_bIsInEffectArea[0],
							(pbOldList[1] == pcsParty->m_bIsInEffectArea[1]) ? NULL : &pcsParty->m_bIsInEffectArea[1],
							(pbOldList[2] == pcsParty->m_bIsInEffectArea[2]) ? NULL : &pcsParty->m_bIsInEffectArea[2],
							(pbOldList[3] == pcsParty->m_bIsInEffectArea[3]) ? NULL : &pcsParty->m_bIsInEffectArea[3],
							(pbOldList[4] == pcsParty->m_bIsInEffectArea[4]) ? NULL : &pcsParty->m_bIsInEffectArea[4]);
		}
	}
	else
	{
		pvPacketEffectArea	= m_csPacketEffectArea.MakePacket(FALSE, pnPacketLength, 0,
						&pcsParty->m_bIsInEffectArea[0],
						&pcsParty->m_bIsInEffectArea[1],
						&pcsParty->m_bIsInEffectArea[2],
						&pcsParty->m_bIsInEffectArea[3],
						&pcsParty->m_bIsInEffectArea[4]);
	}

	if (pvPacketEffectArea)
	{
		INT8	cOperation	= AGPMPARTY_PACKET_OPERATION_UPDATE;

		PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
											&cOperation,
											&pcsParty->m_lID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											pvPacketEffectArea,
											NULL,
											NULL,
											NULL);

		m_csPacketEffectArea.FreePacket(pvPacketEffectArea);

		return	pvPacket;
	}

	return	NULL;
}
/*	2005.05.17 By SingHoon
PVOID AgpmParty::MakePacketCalcExpType(AgpdParty *pcsParty, INT16 *pnPacketLength)
{
	if (!pcsParty || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMPARTY_PACKET_OPERATION_UPDATE_EXP_TYPE;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
									&cOperation,
									&pcsParty->m_lID,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL,
									&pcsParty->m_eCalcExpType,
									NULL,
									NULL);

	return NULL;
}
*/
/*
	2005.04.19	By SungHoon
	아이템 분배방식 변경을 위한 패킷을 만든다.
*/
PVOID AgpmParty::MakePacketItemDivision(AgpdParty *pcsParty, INT32 lMemberCID, INT16 *pnPacketLength)
{
	if (!pcsParty )
		return NULL;

	return MakePacketItemDivision(pcsParty->m_lID, lMemberCID, pcsParty->m_eDivisionItem, pnPacketLength );

}

/*
	2005.04.19	By SungHoon
	아이템 분배방식 변경을 위한 패킷을 만든다.
*/
PVOID AgpmParty::MakePacketItemDivision(INT32 lPartyID, INT32 lMemberCID, AgpmPartyOptionDivisionItem eDivisionItem, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMPARTY_PACKET_OPERATION_UPDATE_ITEM_DIVISION;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
									&cOperation,
									&lPartyID,
									NULL,
									NULL,
									&lMemberCID,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL,
									&eDivisionItem
									);

}

/*
	2005.04.19	By SungHoon
	아이템 분배방식 변경을 위한 패킷을 만든다.
*/
PVOID AgpmParty::MakePacketExpDivision(AgpdParty *pcsParty, INT32 lMemberCID, INT16 *pnPacketLength)
{
	if (!pcsParty )
		return NULL;

	return MakePacketExpDivision(pcsParty->m_lID, lMemberCID, pcsParty->m_eCalcExpType, pnPacketLength );

}

/*
	2005.04.19	By SungHoon
	아이템 분배방식 변경을 위한 패킷을 만든다.
*/
PVOID AgpmParty::MakePacketExpDivision(INT32 lPartyID, INT32 lMemberCID, AgpmPartyCalcExpType eCalExpType, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMPARTY_PACKET_OPERATION_UPDATE_EXP_TYPE;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
									&cOperation,
									&lPartyID,
									NULL,
									NULL,
									&lMemberCID,
									NULL,
									NULL,
									NULL,
									&eCalExpType,
									NULL,
									NULL);

	return NULL;
}

PVOID AgpmParty::MakePacketBonusStatus(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT32	lDamage		= 0;
	INT32	lDefense	= 0;
	INT32	lMaxHP		= 0;
	INT32	lMaxMP		= 0;

	AgpdPartyADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	if (!pcsAttachData->pcsParty)
		return NULL;

	m_pagpmFactors->GetValue(&pcsAttachData->m_csFactorPoint, &lDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_pagpmFactors->GetValue(&pcsAttachData->m_csFactorPoint, &lDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_pagpmFactors->GetValue(&pcsAttachData->m_csFactorPoint, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsAttachData->m_csFactorPoint, &lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	INT8	cDamage		= (INT8) lDamage;
	INT8	cDefense	= (INT8) lDefense;
	INT16	nMaxHP		= (INT16) lMaxHP;
	INT16	nMaxMP		= (INT16) lMaxMP;

	PVOID	pvPacketBonusStat	= m_csPacketBonusStats.MakePacket(FALSE, NULL, 0,
												(cDamage > 0) ? &cDamage : NULL,
												(cDefense > 0) ? &cDefense : NULL,
												(nMaxHP > 0) ? &nMaxHP : NULL,
												(nMaxMP > 0) ? &nMaxMP : NULL);

	if (!pvPacketBonusStat)
		return NULL;

	INT8	cOperation	= AGPMPARTY_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMPARTY_PACKET_TYPE,
												&cOperation,
												&pcsAttachData->pcsParty->m_lID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												pvPacketBonusStat,
												NULL
												);

	m_csPacketBonusStats.FreePacket(pvPacketBonusStat);

	return pvPacket;
}

INT32 AgpmParty::GetNumPartyMember(INT32 lPID)
{
	return GetNumPartyMember(GetParty(lPID));
}

INT32 AgpmParty::GetNumPartyMember(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return 0;

	return pcsParty->m_nCurrentMember;
}

INT32 AgpmParty::GetPartyHighestLevel(INT32 lPID)
{
	return GetPartyHighestLevel(GetParty(lPID));
}

INT32 AgpmParty::GetPartyHighestLevel(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return 0;

	return pcsParty->m_lHighestMemberLevel;
}

INT32 AgpmParty::GetPartyLowerLevel(INT32 lPID)
{
	return GetPartyLowerLevel(GetParty(lPID));
}

INT32 AgpmParty::GetPartyLowerLevel(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return 0;

	return pcsParty->m_lLowerMemberLevel;
}

INT32 AgpmParty::GetPartyHighestUnionRank(INT32 lPID)
{
	return GetPartyHighestUnionRank(GetParty(lPID));
}

INT32 AgpmParty::GetPartyHighestUnionRank(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return 0;

	return pcsParty->m_lHighestMemberUnionRank;
}

BOOL AgpmParty::ArrangeHighestLevel(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	INT32	lHighestLevel = 0;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		//if (!pcsParty->m_pcsMemberList[0])
		//{
		//	pcsParty->m_pcsMemberList[i] = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		//	if (!pcsParty->m_pcsMemberList[i])
		//		continue;
		//}

		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (pcsCharacter)
		{
			INT32	lMemberLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

			if (lHighestLevel < lMemberLevel)
				lHighestLevel = lMemberLevel;

			pcsCharacter->m_Mutex.Release();
		}
	}

	if (lHighestLevel < 1)
		return FALSE;

	pcsParty->m_lHighestMemberLevel = lHighestLevel;

	return TRUE;
}

/*
	2005.05.28 By SungHoon
	가장 레벨낮은 멤버를 구한다.
*/
BOOL AgpmParty::ArrangeLowerLevel(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	INT32	lLowerLevel = 0;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		//if (!pcsParty->m_pcsMemberList[0])
		//{
		//	pcsParty->m_pcsMemberList[i] = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		//	if (!pcsParty->m_pcsMemberList[i])
		//		continue;
		//}

		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (pcsCharacter)
		{
			INT32	lMemberLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

			if (lLowerLevel == 0) lLowerLevel = lMemberLevel;
			else if (lLowerLevel > lMemberLevel)
					lLowerLevel = lMemberLevel;

			pcsCharacter->m_Mutex.Release();
		}
	}

	if (lLowerLevel < 1)
		return FALSE;

	pcsParty->m_lLowerMemberLevel = lLowerLevel;

	return TRUE;
}

// 2007.04.10. steeple
// 최고렙, 최저렙, 파티 레벨합 을 한방에 정리한다.
BOOL AgpmParty::ArrangeLevel(AgpdParty* pcsParty)
{
	if(!pcsParty)
		return FALSE;

	INT32 lHighestLevel, lLowerLevel, lSumLevel;
	lHighestLevel = lLowerLevel = lSumLevel = 0;

	for(INT32 i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if(pcsCharacter)
		{
			INT32 lMemberLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

			lSumLevel += lMemberLevel;

			if(lHighestLevel < lMemberLevel)
				lHighestLevel = lMemberLevel;

			if(lLowerLevel == 0) lLowerLevel = lMemberLevel;
			else if(lLowerLevel > lMemberLevel)
					lLowerLevel = lMemberLevel;

			pcsCharacter->m_Mutex.Release();
		}
	}

	if(lHighestLevel > 0)
		pcsParty->m_lHighestMemberLevel = lHighestLevel;
	if(lLowerLevel > 0)
		pcsParty->m_lLowerMemberLevel = lLowerLevel;
	if(lSumLevel > 0)
		pcsParty->m_lTotalMemberLevel = lSumLevel;

	return TRUE;
}

BOOL AgpmParty::ArrangeHighestUnionRank(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	INT32	lHighestUnionRank = 0;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		//if (!pcsParty->m_pcsMemberList[0])
		//{
		//	pcsParty->m_pcsMemberList[i] = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		//	if (!pcsParty->m_pcsMemberList[i])
		//		continue;
		//}

		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (pcsCharacter)
		{
			INT32	lMemberUnionRank = m_pagpmUnion->GetUnionRank(pcsCharacter);

			if (lHighestUnionRank < lMemberUnionRank)
				lHighestUnionRank = lMemberUnionRank;

			pcsCharacter->m_Mutex.Release();
		}
	}

	if (lHighestUnionRank < 1)
		return FALSE;

	pcsParty->m_lHighestMemberUnionRank = lHighestUnionRank;

	return TRUE;
}

INT32 AgpmParty::GetPartyAvrMemberLevel(AgpdParty *pcsParty)
{
	return pcsParty->m_lTotalMemberLevel/pcsParty->m_nCurrentMember;
}

INT32 AgpmParty::GetPartyTotalMemberLevel(INT32 lPID)
{
	return GetPartyTotalMemberLevel(GetParty(lPID));
}

INT32 AgpmParty::GetPartyTotalMemberLevel(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return 0;

	return pcsParty->m_lTotalMemberLevel;
}

AgpdParty* AgpmParty::GetParty(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdPartyADChar *pcsPartyADChar = GetADCharacter(pcsCharacter);
	if (!pcsPartyADChar)
		return NULL;

	if (!pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = GetParty(pcsPartyADChar->lPID);

	return pcsPartyADChar->pcsParty;
}

BOOL AgpmParty::ProcessRemove(UINT32 ulClockCount)
{
	if (m_ulPrevRemoveClockCount + AGPMPARTY_PROCESS_REMOVE_INTERVAL > ulClockCount)
		return TRUE;

	INT32		lIndex		= 0;
	AgpdParty	*pcsParty	= NULL;

	AgpdParty	**ppcsParty = (AgpdParty **) m_csAdminPartyRemove.GetObjectSequence(&lIndex);

	while (ppcsParty && *ppcsParty)
	{
		pcsParty		= *ppcsParty;

		if (pcsParty->m_ulRemoveTimeMSec + AGPMPARTY_PRESERVE_PARTY_DATA < ulClockCount)
		{
			// 이제 모듈 데이타를 삭제할 때가 되었다.
			m_csAdminPartyRemove.RemoveObject((INT_PTR) pcsParty);
			lIndex = 0;

			DestroyPartyData(pcsParty);
		}

		ppcsParty = (AgpdParty **) m_csAdminPartyRemove.GetObjectSequence(&lIndex);
	}

	m_ulPrevRemoveClockCount = ulClockCount;

	return TRUE;
}

BOOL AgpmParty::AddRemoveParty(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	pcsParty->m_ulRemoveTimeMSec	= GetClockCount();

	if (!m_csAdminPartyRemove.AddObject(&pcsParty, (INT_PTR) pcsParty))
	{
		AgpdParty	**ppcsParty = (AgpdParty **) m_csAdminPartyRemove.GetObject((INT_PTR) pcsParty);
		if (ppcsParty && *ppcsParty)
		{
			DestroyPartyData(*ppcsParty);

			m_csAdminPartyRemove.RemoveObject((INT_PTR) (*ppcsParty));

			if (m_csAdminPartyRemove.AddObject(&pcsParty, (INT_PTR) pcsParty))
				return TRUE;
		}

		return FALSE;
	}

	return TRUE;
}

BOOL AgpmParty::SetEffectArea(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	//BOOL	abOldList[AGPMPARTY_MAX_PARTY_MEMBER];
	ApSafeArray<BOOL, AGPMPARTY_MAX_PARTY_MEMBER>	abOldList;

	//CopyMemory(abOldList, pcsParty->m_bIsInEffectArea, sizeof(BOOL) * AGPMPARTY_MAX_PARTY_MEMBER);
	abOldList.MemCopy(0, &pcsParty->m_bIsInEffectArea[0], AGPMPARTY_MAX_PARTY_MEMBER);

	pcsParty->m_bIsInEffectArea[0]	= TRUE;

	for (int i = 1; i < pcsParty->m_nCurrentMember; ++i)
	{
		pcsParty->m_bIsInEffectArea[i]	= TRUE;		//	2005.06.13. By SungHoon
		// m_pagpmFactors->IsInRange(&pcsParty->m_pcsMemberList[0]->m_stPos, &pcsParty->m_pcsMemberList[i]->m_stPos, AGPMPARTY_MAX_EFFECT_AREA, 0);
	}

	EnumCallback(AGPMPARTY_CB_RESET_EFFECT_AREA, pcsParty, (PVOID) &abOldList[0]);

	return TRUE;
}

AgpdParty* AgpmParty::GetPartySequence(INT32 *pnIndex)
{
	if (!pnIndex)
		return NULL;

	AgpdParty	**ppcsParty	= (AgpdParty **) m_csPartyAdmin.GetObjectSequence(pnIndex);
	if (!ppcsParty || !*ppcsParty)
		return NULL;

	return *ppcsParty;
}
