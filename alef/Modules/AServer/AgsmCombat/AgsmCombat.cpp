/******************************************************************************
Module:  AgsmCombat.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 14
******************************************************************************/

#include <time.h>

#include "AgpmCharacter.h"
#include "AgsmCombat.h"
#include "AgsdCharacter.h"
#include "ApMemoryTracker.h"

AgsmCombat::AgsmCombat()
{
	SetModuleName("AgsmCombat");

	SetModuleType(APMODULE_TYPE_SERVER);
}

AgsmCombat::~AgsmCombat()
{
}

BOOL AgsmCombat::OnAddModule()
{
	m_pagpmGrid		 = (AgpmGrid *)		 GetModule("AgpmGrid");
	m_pagpmFactors	 = (AgpmFactors *)   GetModule("AgpmFactors");
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem		 = (AgpmItem *)		 GetModule("AgpmItem");
	m_pagpmCombat	 = (AgpmCombat *)	 GetModule("AgpmCombat");
	m_pagpmOptimizedPacket2	= (AgpmOptimizedPacket2 *)	GetModule("AgpmOptimizedPacket2");
	m_pagpmAI2		 = (AgpmAI2 *)		 GetModule("AgpmAI2" );
	m_pagpmSummons	 = (AgpmSummons*)	 GetModule("AgpmSummons");
	m_pagpmSiegeWar  = (AgpmSiegeWar*)	 GetModule("AgpmSiegeWar");

	m_pagsmAOIFilter = (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pagsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmItem		 = (AgsmItem *)		 GetModule("AgsmItem");
	m_pagsmFactors	 = (AgsmFactors *)   GetModule("AgsmFactors");
	m_pagsmParty	 = (AgsmParty *)	 GetModule("AgsmParty");

	if (!m_pagpmGrid || !m_pagpmFactors || !m_pagpmCharacter || !m_pagsmAOIFilter || !m_pagsmCharacter || !m_pagsmFactors || 
		!m_pagsmParty || !m_pagpmSummons || !m_pagpmSiegeWar)
		return FALSE;

	if (!m_pagpmItem ||	!m_pagsmItem ||	!m_pagpmCombat)
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackActionAttack(CBActionAttack, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmCombat::OnInit()
{
	return TRUE;
}

BOOL AgsmCombat::OnDestroy()
{
	return TRUE;
}

BOOL AgsmCombat::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmCombat::OnValid(CHAR* pszData, INT16 nSize)
{
	return TRUE;
}

PVOID AgsmCombat::MakePacketAttackResult(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, PVOID pvPacketFactor, INT8 cAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT16 *pnPacketLength)
{
	if (!pAttackChar || !pTargetChar || !pnPacketLength)
		return NULL;

	if (m_pagpmOptimizedPacket2)
		return m_pagpmOptimizedPacket2->MakePacketCharAction(pAttackChar, pTargetChar, cAttackResult, pvPacketFactor, 0, cComboInfo, bForceAttack, ulAdditionalEffect, cHitIndex, pnPacketLength);
	else
	{
		INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

		PVOID	pvPacketAction	= NULL;

		INT8	cAction = AGPDCHAR_ACTION_TYPE_ATTACK;

		pvPacketAction = m_pagpmCharacter->m_csPacketAction.MakePacket(FALSE, pnPacketLength, 0,
																	 &cAction,
																	 &pTargetChar->m_lID,
																	 (lSkillTID != AP_INVALID_SKILLID) ? &lSkillTID : NULL,
																	 &cAttackResult,
																	 pvPacketFactor,
																	 &pAttackChar->m_stPos,
																	 (cComboInfo > 0) ? &cComboInfo : NULL,
																	 (bForceAttack) ? &bForceAttack : NULL,
																	 (ulAdditionalEffect) ? &ulAdditionalEffect : NULL,
																	 (cHitIndex) ? & cHitIndex : NULL);

		if (!pvPacketAction)
			return FALSE;

		PVOID pvPacket = m_pagpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																 &cOperation,					// Operation
																 &pAttackChar->m_lID,			// Character ID
																 NULL,							// Character Template ID
																 NULL,							// Game ID
																 NULL,							// Character Status
																 NULL,							// Move Packet
																 pvPacketAction,				// Action Packet
																 NULL,							// Factor Packet
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

		m_pagpmCharacter->m_csPacketAction.FreePacket(pvPacketAction);

		return pvPacket;
	}

	return NULL;
}

BOOL AgsmCombat::SendAttackResult(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, PVOID pvPacketFactor, INT8 cAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex)
{
	// 공격 했다는 정보를 주변에 있는 캐릭터들에게 보낸다.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, pvPacketFactor, cAttackResult, cComboInfo, bForceAttack, lSkillTID, ulAdditionalEffect, cHitIndex, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	/*
	m_pagsmParty->SendPacketToNearPartyMember(pAttackChar, pvPacket, nPacketLength);
	m_pagsmParty->SendPacketToNearPartyMember(pTargetChar, pvPacket, nPacketLength);
	*/

	/*
	if (!m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pAttackChar->m_stPos))
	{
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}
	*/

	// 파티에 들어 있다면 파티원들간의 싱크를 맞추어준다.
	//m_pagsmParty->SendPacketToParty(pAttackChar, pvPacket, nPacketLength);
	//m_pagsmParty->SendPacketToParty(pTargetChar, pvPacket, nPacketLength);

	/*
	m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pTargetChar->m_stPos);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
	*/

	// 공격자와 방어자와 관련된 넘들한테 패킷을 보낸다.
	UINT32	ulAttackDPNID	= m_pagsmCharacter->GetCharDPNID(pAttackChar);
	UINT32	ulTargetDPNID	= m_pagsmCharacter->GetCharDPNID(pTargetChar);

	if (ulAttackDPNID > 0)
	{
		m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pAttackChar->m_lID);

		SendPacket(pvPacket, nPacketLength, ulAttackDPNID);
	}
	if (ulTargetDPNID > 0)
	{
		m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pTargetChar->m_lID);

		SendPacket(pvPacket, nPacketLength, ulTargetDPNID);
	}

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	// factor 정보를 뺀 패킷을 만들어 주변넘들한테 뿌려준다.
	nPacketLength	= 0;

	PVOID	pvPacketHP	= m_pagpmFactors->MakePacketFactorsCharHP(&pTargetChar->m_csFactor);

	pvPacket = MakePacketAttackResult(pAttackChar, pTargetChar, pvPacketHP, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS, cComboInfo, bForceAttack, lSkillTID, ulAdditionalEffect, cHitIndex, &nPacketLength);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketHP);

	if (!pvPacket)
		return FALSE;

	UINT32	ulExceptNIDs[2];
	INT32	lNumNID		= 0;

	if (ulAttackDPNID != 0)
	{
		ulExceptNIDs[lNumNID]	= ulAttackDPNID;
		++lNumNID;
	}
	if (ulTargetDPNID != 0)
	{
		ulExceptNIDs[lNumNID]	= ulTargetDPNID;
		++lNumNID;
	}

	m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pAttackChar->m_lID);
	
	m_pagsmAOIFilter->SendPacketNearExceptNIDs(pvPacket, nPacketLength, pTargetChar->m_stPos, m_pagpmCharacter->GetRealRegionIndex(pTargetChar), ulExceptNIDs, lNumNID, PACKET_PRIORITY_5);

	// 공격자와, 타겟이 둘다 공성 오브젝트라면,
	// 공격하는 쪽에도 위 패킷을 보내준다.
	// 그리고 맞는 쪽에다가 HP 싱크패킷을 추가로 보내준다.
	if(m_pagpmSiegeWar->IsSiegeWarMonster(pAttackChar) && m_pagpmSiegeWar->IsSiegeWarMonster(pTargetChar))
	{
		m_pagsmAOIFilter->SendPacketNearExceptNIDs(pvPacket, nPacketLength, pAttackChar->m_stPos, m_pagpmCharacter->GetRealRegionIndex(pAttackChar), ulExceptNIDs, lNumNID, PACKET_PRIORITY_5);
		m_pagsmCharacter->SyncHP(pTargetChar);
	}

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmCombat::SendAttackResultNotEnoughArrow(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	// 공격 했다는 정보를 주변에 있는 캐릭터들에게 보낸다.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW, 0, FALSE, AP_INVALID_SKILLID, 0, 0, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	// 공격자와 방어자와 관련된 넘들한테 패킷을 보낸다.
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pAttackChar));

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCombat::SendAttackResultNotEnoughBolt(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	// 공격 했다는 정보를 주변에 있는 캐릭터들에게 보낸다.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT, 0, FALSE, AP_INVALID_SKILLID, 0, 0, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	// 공격자와 방어자와 관련된 넘들한테 패킷을 보낸다.
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pAttackChar));

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCombat::SendAttackResultNotEnoughMP(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	// 공격 했다는 정보를 주변에 있는 캐릭터들에게 보낸다.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP, 0, FALSE, AP_INVALID_SKILLID, 0, 0, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	// 공격자와 방어자와 관련된 넘들한테 패킷을 보낸다.
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pAttackChar));

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCombat::SetCallbackAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_ATTACK, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackCheckDefense(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_CHECK_DEFENSE, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackDamageAdjust(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackPreCheckCombat(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_PRE_CHECK_COMBAT, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackPostCheckCombat(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_POST_CHECK_COMBAT, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackNPCDialog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_NPC_DIALOG, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackConvertDamage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_CONVERT_DAMAGE, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackAttackStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_ATTACK_START, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackDamageAdjustSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST_SIEGEWAR, pfCallback, pClass);
}

BOOL AgsmCombat::CBActionAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmCombat::CBActionAttack");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCombat	*pThis = (AgsmCombat *) pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBActionAttack"));

	PVOID	*pvBuffer	= (PVOID *) pCustData;

	AgpdCharacter	*pAttackChar	= (AgpdCharacter *) pData;
	AgpdCharacter	*pTargetChar	= (AgpdCharacter *) pvBuffer[0];

	UINT8			cComboInfo		= (UINT8)PtrToUint(pvBuffer[3]);
	BOOL			bForceAttack	= PtrToInt(pvBuffer[4]);
	UINT32			ulAdditionalEffect = PtrToUint(pvBuffer[5]);
	UINT8			cHitIndex		= (UINT8) pvBuffer[6];

	if (!pAttackChar || !pTargetChar)
		return FALSE;

	if (pAttackChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
		pTargetChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	// 2005.10.17. steeple
	if (pAttackChar->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALT)
		return FALSE;

	// 2005.11.02. steeple
	// 원정대 깃발 들고 있으면 공격불가. (탈 것 타고 있을 때만)
	if (pThis->m_pagpmItem->IsEquipStandard(pAttackChar) && pAttackChar->m_bRidable)
		return FALSE;

	if (!pThis->m_pagpmCharacter->CheckActionStatus(pAttackChar, AGPDCHAR_STATUS_ATTACK))
		return FALSE;

	if (pThis->m_pagpmCharacter->IsActionBlockCondition(pAttackChar))
		return FALSE;

	// 2007.10.09. steeple
	// Sleep 상태이면 공격못한다.
	if (pThis->m_pagpmCharacter->IsStatusSleep(pAttackChar))
		return FALSE;

	// 상대방이 투명이면 공격하지 못한다. 2005.10.07. steeple
	if (pThis->m_pagpmCharacter->IsStatusFullTransparent(pTargetChar))
		return FALSE;

	// 상대방이 일시 무적이면 공격하지 못한다. 2006.01.11. steeple
	if (pThis->m_pagpmCharacter->IsStatusInvincible(pTargetChar))
		return FALSE;

	if (!pThis->m_pagpmCharacter->CheckVaildNormalAttackTarget(pAttackChar, pTargetChar, bForceAttack))
		return FALSE;



	// 공격에 필요한 뭔가가 있어야 하는지 검사한다.
	/////////////////////////////////////////////////////////////////////////
	AgpdCharacterActionResultType	eActionResult	= AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	pThis->m_pagpmCharacter->CheckActionRequirement(pAttackChar, &eActionResult, AGPDCHAR_ACTION_TYPE_ATTACK);

	switch (eActionResult) {
	case AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW:
		{
			pThis->SendAttackResultNotEnoughArrow(pAttackChar, pTargetChar);
			return FALSE;
		}
		break;

	case AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT:
		{
			pThis->SendAttackResultNotEnoughBolt(pAttackChar, pTargetChar);
			return FALSE;
		}
		break;

	case AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP:
		{
			pThis->SendAttackResultNotEnoughMP(pAttackChar, pTargetChar);
			return FALSE;
		}
		break;
	}

	AgsdCharacter	*pAgsdAttackChar = pThis->m_pagsmCharacter->GetADCharacter(pAttackChar);

	if (pThis->GetClockCount() < pAgsdAttackChar->m_ulNextAttackTime)
	{
		OutputDebugString("Attack Skipped\n");
		//pAgsdAttackChar->m_ulNextAttackTime = pThis->GetClockCount(); // 이상한 로직, 공격속도 스핵에 뚫려서 제거 [ 2011-06-13 silvermoo ]
		return FALSE;
	}

	// 다음 공격시간을 세팅한다.
	UINT32		ulAttackIntervalMSec = pThis->m_pagpmCharacter->GetAttackIntervalMSec(pAttackChar);
	if (ulAttackIntervalMSec <= 0)
		return FALSE;
	pAgsdAttackChar->m_ulNextAttackTime = (UINT32)((FLOAT)pThis->GetClockCount() + (FLOAT)ulAttackIntervalMSec * 0.9f);

	//여기까지 왔다면 우선 공격은 시도한거다. PC가 공격 시도을 시도한 경우 그에 따른 어그로를 넣어주자.
	pThis->m_pagsmCharacter->AddAgroPoint( pTargetChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_SLOW, 1 );

	// 공격하는데 있어서 지불해야할 비용이 있다면 여기서 지불한다.
	//////////////////////////////////////////////////////////////////////////
	pThis->m_pagpmCharacter->PayActionCost(pAttackChar, AGPDCHAR_ACTION_TYPE_ATTACK);

	stAgsmCombatAttackResult	stAttackResult;
	memset(&stAttackResult, 0, sizeof(stAttackResult));
	stAttackResult.pAttackChar	= pAttackChar;
	stAttackResult.pTargetChar	= pTargetChar;
	stAttackResult.nDamage		= 0;
	stAttackResult.ulCheckResult	= 0;

	// 공격자가 일시 무적 상태라면 풀어준다. 2006.01.09. steeple
	if(pThis->m_pagpmCharacter->IsStatusInvincible(pAttackChar))
		pThis->m_pagpmCharacter->UpdateUnsetSpecialStatus(pAttackChar, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE);

	// 공격자가 소환수나 테이밍인데, 주인이 무적이라면 풀어준다. 2006.01.11. steeple
	if(pThis->m_pagpmCharacter->IsStatusSummoner(pAttackChar) || pThis->m_pagpmCharacter->IsStatusTame(pAttackChar))
	{
		AgpdCharacter* pcsOwner = pThis->m_pagpmCharacter->GetCharacterLock(pThis->m_pagpmSummons->GetOwnerCID(pAttackChar));
		if(pcsOwner)
		{
			if(pThis->m_pagpmCharacter->IsStatusInvincible(pcsOwner))
				pThis->m_pagpmCharacter->UpdateUnsetSpecialStatus(pcsOwner, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE);

			pcsOwner->m_Mutex.Release();
		}
	}

	// 공격을 했다는 것을 불러준다. For PvP
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_ATTACK_START, &stAttackResult, NULL);

	// 공격 검사 이전에 처리해야할 부분이 있다면 여기서 처리한다.
	//
	// 아래 콜백 안(AgsmSkill) 에서 공격자가 투명이면 풀리게 처리했다. 2005.10.07. steeple
	//
	////////////////////////////////////////////////////////////////////////
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_PRE_CHECK_COMBAT, &stAttackResult, NULL);

	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_FAILED_ATTACK)
	{
		if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_MELEE_COUNTERATTACK)
		{
			// 공격자의 공격을 막고 방어자가 공격자에게 일반 공격데미지를 입힌다.

			// 일단 공격자한테 어떻게 표시를 어떻게 해줘야 할지 세팅해야 한다.
			// 공격자의 공격 정보를 패킷으로 보내고 반격정보를 따로 보내야 할지 등을 정해야 한다.
			//
			//
			//

			// 암튼 데미지는 구해놓는다.

			BOOL		bIsEquipWeapon			= pThis->m_pagpmItem->IsEquipWeapon(pTargetChar);

			INT32		lCounterAttackDamage	= pThis->m_pagpmCombat->CalcPhysicalAttack(pTargetChar, pAttackChar, bIsEquipWeapon);

			return pThis->ApplyAttackDamage(pTargetChar, pAttackChar, lCounterAttackDamage, lCounterAttackDamage, NULL, AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK, 0, 0, 0, 0, cHitIndex);
		}

		//if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_EVADE_ATTACK)
		//{
		//	// 공격자의 공격을 회피했다.
		//	return pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE, cComboInfo, bForceAttack, stAttackResult.lSkillTID, 0);
		//}
	}

	// 2005.12.22. steeple
	AgpdCharacterActionResultType eEvadeType = pThis->m_pagpmCombat->ProcessEvade(pAttackChar, pTargetChar);
	if(eEvadeType != AGPDCHAR_ACTION_RESULT_TYPE_NONE)
	{
		// 공격자의 공격을 회피했다.
		return pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, eEvadeType, cComboInfo, bForceAttack, stAttackResult.lSkillTID, 0, 0);
	}

	PVOID		pvAttackPacketFactor = NULL;
	PVOID		pvTargetPacketFactor = NULL;
	AgpdFactor	csUpdateFactor;

	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

//	BOOL		abIsSuccessSecondSpirit[AGPD_FACTORS_ATTRIBUTE_MAX_TYPE];
//	ZeroMemory(abIsSuccessSecondSpirit, sizeof(BOOL) * AGPD_FACTORS_ATTRIBUTE_MAX_TYPE);

	ApSafeArray<BOOL, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>		abIsSuccessSecondSpirit;
	abIsSuccessSecondSpirit.MemSetAll();

	AgpdCharacterActionResultType	eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	BOOL		bIsEquipWeapon	= pThis->m_pagpmItem->IsEquipWeapon(pAttackChar);

	int			nCombatPreCheck = 0;

	BOOL		bIsAttrInvincible = pThis->IsAttrInvincible(pTargetChar);	// 타겟이 속성 무적인지

	if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_IGNORE_PHYSICAL_DEFENCE)	// PrecheckCombat 결과가 물리방어 무시인지 체크
		nCombatPreCheck |= AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_PHYSICAL_DEFENCE;

	if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_IGNORE_ATTRIBUTE_DEFENCE)	// precheckCombat 결과가 속성방어 무시인지 체크
		nCombatPreCheck |= AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_ATTRIBUTE_DEFENCE; 

	/*
	INT16 nDamage = pThis->m_pagsmFactors->ProcessCombat(
														&pAttackChar->m_csFactor,
														&pTargetChar->m_csFactor,
														AGSM_FACTORS_MELEE_ATTACK, 
														(100 - stAttackResult.nDamage),
														lReflectDamage,
														&pvAttackPacketFactor,
														&pvTargetPacketFactor,
														&csUpdateFactor
														);
	*/
	INT32 lHeroicDamage = 0;

	//공격을 한다.
	INT32 nDamage = pThis->m_pagpmCombat->ProcessCombat(pAttackChar,
														 pTargetChar,
														 &csUpdateFactor,
														 &abIsSuccessSecondSpirit[0],
														 &eResultType,
														 bIsEquipWeapon,
														 bIsAttrInvincible,
														 &lHeroicDamage,
														 nCombatPreCheck);

	AgsdCharacter	*pcsAgsdTarget	= pThis->m_pagsmCharacter->GetADCharacter(pTargetChar);

	if (pcsAgsdTarget && pcsAgsdTarget->m_bIsSuperMan)
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS;

	if (pcsAgsdTarget && pcsAgsdTarget->m_bIsMaxDefence)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;

		pThis->ApplyAttackDamage(pAttackChar, pTargetChar, 1, 1, NULL, eResultType, 0, 0, 0, 0, 0);

		pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return TRUE;
	}

	// 일반 공격에 대해서 무적 체크를 한다. 2007.07.02. steeple
	if (pThis->m_pagpmCharacter->IsStatusNormalATKInvincible(pTargetChar))
	{
		INT32 lRandom = pThis->m_pagpmCombat->m_csRandom.randInt(100);
		if(pcsAgsdTarget->m_stInvincibleInfo.lNormalATKProbability > lRandom)
			eResultType = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS;
	}

	// 방패에 의해 블럭 되거나 공격이 실패한 경우 걍 패킷만 보내고 끝낸다.
	///////////////////////////////////////////////////////////////////////////
	if (eResultType == AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS ||
		eResultType == AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK)
	{
		//2004.10.25일 몬스터 대사 통합되면서 주석처리됨.
/*		//미스나 블럭, PC가 몹에게 공격시 MISS,BLock이 났으면 몹이 약올리는 말을 읊조린다.
		if( pThis->m_pagpmCharacter->IsPC( pAttackChar ) && pThis->m_pagpmCharacter->IsMonster( pTargetChar ) )
		{
			eAgpdEventNPCDialogMob		eDialogType;

			eDialogType = AGPD_NPCDIALOG_MOB_PC_MISS;

			pThis->EnumCallback( AGSMCOMBAT_CB_ID_NPC_DIALOG, (ApBase *)pAttackChar, (void *)&eDialogType );
		}*/

		pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, eResultType, cComboInfo, bForceAttack, AP_INVALID_SKILLID, 0, 0);

		pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return TRUE;
	}
	else if (nDamage <= 0)
	{
		pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS, cComboInfo, bForceAttack, AP_INVALID_SKILLID, 0, 0);

		pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return TRUE;
	}

	// 일단 성공한 경우이다.
	///////////////////////////////////////////////////////////////////////////

	// 타겟이 sleep 상태이면 풀어준다. 2007.06.27. steeple
	if(pThis->m_pagpmCharacter->IsStatusSleep(pTargetChar))
		pThis->m_pagpmCharacter->UpdateUnsetSpecialStatus(pTargetChar, AGPDCHAR_SPECIAL_STATUS_SLEEP);

	// 파티에 따라 보너스 데미지가 있는지 검사한다.
	//////////////////////////////////////////////////////////////////////////////////
	if (nDamage > 0)
		nDamage += (INT32) (nDamage * (pThis->m_pagsmParty->GetPartyBonusDamage(pAttackChar, NULL) / 100.0));

	INT32	lMeleeDamage	= 0;
	pThis->m_pagpmFactors->GetValue(&csUpdateFactor, &lMeleeDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);
	stAttackResult.nDamage		= (-lMeleeDamage);

	stAttackResult.ulCheckResult	= 0;

	// 공격 검사 후 처리해야할 부분이 있다면 여기서 처리한다.
	/////////////////////////////////////////////////////////////////////////
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_POST_CHECK_COMBAT, &stAttackResult, &csUpdateFactor);

	INT32	lReflectDamage		= 0;
	INT32	lConvertDamage		= 0;

	if (stAttackResult.nDamage > 0 && stAttackResult.ulCheckResult == 0)
	{
		// 방어력과 반격데미지를 체크한다.
		pThis->EnumCallback(AGSMCOMBAT_CB_ID_CHECK_DEFENSE, &stAttackResult, &lReflectDamage);
	}

	eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;

	INT32	lAttackSkillTID		= AP_INVALID_SKILLID;
	INT32	lReflectSkillTId	= AP_INVALID_SKILLID;

	// Divide 처리는 맨 처음에 해준다. 2007.06.27. steeple
	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_DIVIDE)
	{
		 pThis->ProcessDivide(stAttackResult, &csUpdateFactor, &nDamage);
	}

	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_MELEE_DEATH_STRIKE)
	{
		// 걍 한방에 죽여야 한다.  타겟의 현재 HP만큼 데미지를 증가시켜 죽여버린다.
		// 데미진 콜백에서 처리되어 진다.

		eResultType = AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE;

		lAttackSkillTID		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_MELEE_CRITICAL_STRIKE)
	{
		// 크리티컬 스트라이크다. 데미지는 콜백에서 이미 계산을 다 해놓았구.. 여기선 할일 없구..
		// 공격 결과를 보낼때 타입을 CRITICAL로 세팅한다.

		eResultType = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL;

		lAttackSkillTID		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_REFLECT_DAMAGE)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_REFLECT_DAMAGE;

		lReflectSkillTId		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_REDUCE_DAMAGE)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE;

		lReflectSkillTId		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_HP ||
			 stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_MP)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_CONVERT_DAMAGE;

		lReflectSkillTId		= stAttackResult.lSkillTID;
		
		// 임시 저장 후
		lConvertDamage = stAttackResult.nDamage;
		stAttackResult.nDamage = 0;
	}
	else if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_HP ||
			stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_MP)
	{
		// 임시 저장
		lConvertDamage = stAttackResult.nDamage;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CHARGE)
	{
		eResultType = AGPDCHAR_ACTION_RESULT_TYPE_CHARGE;

		lAttackSkillTID			= stAttackResult.lSkillTID;
	}
	
	/*
	// 구한 데미지에 대해 필요하면 수정을 한다.
	//	(damage adjust skill)
	//////////////////////////////////////////////////////////////
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST, pAttackChar, &stAttackResult.nDamage);
	*/

	//nDamage	= stAttackResult.nDamage;

	// 공격 정보 보내는것에 대한 수정이 필요하다.
	//		(SendAttack과 SendAttackResult를 하나로 통합하고 공격과 결과 정보를 한꺼번에 전송한다.
	///////////////////////////////////////////////////////
	//if (!bSendAttackResult)
	//	pThis->SendAttack(pAttackChar, pTargetChar);

	// 공성전 관련 데미지 보정
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST_SIEGEWAR, &stAttackResult, &csUpdateFactor);

	// 높이에 따른 데미지 처리. 2006.06.23. steeple
	stAttackResult.nDamage += pThis->AdjustHeightDamage(pAttackChar, pTargetChar, stAttackResult.nDamage);

	if (stAttackResult.nDamage > 0)
	{
		if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_LENS_STONE)
			ulAdditionalEffect |= AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE;

		pThis->ApplyAttackDamage(pAttackChar, pTargetChar, -(((-lMeleeDamage) - stAttackResult.nDamage) - nDamage), stAttackResult.nDamage, &csUpdateFactor, eResultType, cComboInfo, bForceAttack, lAttackSkillTID, ulAdditionalEffect, cHitIndex);

		// 캐쉬 아이템처리. 2005.12.12. steeple
		pThis->m_pagpmItem->SubCashItemStackCountOnAttack(pAttackChar);
	}
	else
	{
		if(eResultType == AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE)
			pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE, cComboInfo, bForceAttack, lReflectSkillTId, ulAdditionalEffect, cHitIndex);
		else
			pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS, cComboInfo, bForceAttack, lReflectSkillTId, ulAdditionalEffect, cHitIndex);
	}

	if (lReflectDamage > 0)
	{
		// 위에서 Reflect 에 관한 건 보냈기 때문에 결과는 초기화를 해준다.
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;

		pThis->ApplyAttackDamage(pTargetChar, pAttackChar, lReflectDamage, lReflectDamage, NULL, eResultType, 0, bForceAttack, lReflectSkillTId, 0, 0);
	}

	// Convert Damage 는 위의 처리를 다 한 후 해준다. - 2004.09.17. steeple
	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_HP ||
		stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_MP ||
		stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_HP ||
		stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_MP)
	{
		// Convert Damage 를 적용 시켜준다.
		stAttackResult.nDamage = lConvertDamage;
		pThis->EnumCallback(AGSMCOMBAT_CB_ID_CONVERT_DAMAGE, &stAttackResult, NULL);
	}

	pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	return TRUE;
}

BOOL AgsmCombat::ApplyAttackDamage(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, INT32 lTotalDamage, INT32 lDamageNormal, AgpdFactor *pcsUpdateFactor, AgpdCharacterActionResultType eAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	if (lTotalDamage <= 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("ApplyAttackDamage"));

	BOOL	bAllocFactor	= FALSE;
	if (!pcsUpdateFactor)
	{
		bAllocFactor	= TRUE;
		pcsUpdateFactor = new (AgpdFactor);
	}

	// 여기서 위에서 나온 데미지를 실제 캐릭터의 status에 반영한다.
	AgpdFactor			*pcsUpdateFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateFactorResult)
	{
		m_pagpmFactors->DestroyFactor(pcsUpdateFactor);
		if (bAllocFactor)
			delete(pcsUpdateFactor);

		return FALSE;
	}

	m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lDamageNormal), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);
	m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lTotalDamage), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	// 만약 현재 HP가 데미지보다 작다면.. (즉, 죽은 놈이라면)
	// 남은 MP, SP도 모두 0으로 만들어 버린다. (기획에서 해달란다 ㅡ.ㅡ)
	///////////////////////////////////////////////////////////////////////////
	INT32	lCurrentTargetHP	= 0;
	INT32	lCurrentTargetMP	= 0;
	INT32	lCurrentTargetSP	= 0;

	m_pagpmFactors->GetValue(&pTargetChar->m_csFactor, &lCurrentTargetHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if (lCurrentTargetHP <= lTotalDamage)
	{
		// 이넘은 이제 죽은놈이다.
		// 현재 MP, SP를 가져와서 0으로 만들어준다.
		m_pagpmFactors->GetValue(&pTargetChar->m_csFactor, &lCurrentTargetMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(&pTargetChar->m_csFactor, &lCurrentTargetSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

		m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lCurrentTargetMP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lCurrentTargetSP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lCurrentTargetHP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	}

	PVOID pvTargetPacketFactor = m_pagpmFactors->CalcFactor(&pTargetChar->m_csFactor, pcsUpdateFactor, FALSE, TRUE, TRUE, FALSE);
	if (pvTargetPacketFactor)
	{
		SendAttackResult(pAttackChar, pTargetChar, pvTargetPacketFactor, eAttackResult, cComboInfo, bForceAttack, lSkillTID, ulAdditionalEffect, cHitIndex);

		m_pagpmFactors->m_csPacket.FreePacket(pvTargetPacketFactor);
	}
	m_pagsmFactors->ResetResultFactorDamageInfo(&pTargetChar->m_csFactor);
	m_pagsmFactors->ResetResultFactorDamageInfo(pcsUpdateFactor);

	// player간 쌈박질인지 검사한다.
	if (m_pagsmCharacter->GetCharDPNID(pAttackChar) && m_pagsmCharacter->GetCharDPNID(pTargetChar))
	{
		// player끼리 싸우는거라면 정당방위 설정을 위해 공격자 리스트에 추가한다.
		m_pagpmCharacter->AddAttackerToList(pTargetChar, pAttackChar->m_lID);
	}

	// 공격자가 PC, 타겟이 몬스터일 경우의 처리
	if( m_pagpmCharacter->IsPC(pAttackChar) && m_pagpmCharacter->IsMonster(pTargetChar) ||
		// 공격자가 몬스터이고 타겟이 소환수 일때의 처리
		m_pagpmCharacter->IsMonster(pAttackChar) && m_pagpmCharacter->IsStatusSummoner(pTargetChar))
	{
		if (m_pagpmAI2)
		{
			//만약 보스가 있는 부하몬스터라면?
			AgpdAI2ADChar		*pcsAgpdAI2ADChar = NULL;
			pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData( pTargetChar );

			if( pcsAgpdAI2ADChar )
			{
				if( pcsAgpdAI2ADChar->m_pcsBossCharacter )
				{
					//보스의 적대치에 부하몬스터를 공격한 PC를 넣는다.
					m_pagsmCharacter->AddAgroPoint( pcsAgpdAI2ADChar->m_pcsBossCharacter, pAttackChar, AGPD_FACTORS_AGRO_TYPE_SLOW, lTotalDamage );
				}

				m_pagsmCharacter->AddAgroPoint( pTargetChar, pAttackChar, pcsUpdateFactorResult );
			}
		}
	}
	// 공격자가 소환수나 테이밍 몬스터이고 타겟이 몬스터일 경우의 처리
	else if ( m_pagpmCharacter->IsMonster(pTargetChar) && 
			 (m_pagpmCharacter->IsStatusTame( pAttackChar ) || m_pagpmCharacter->IsStatusSummoner( pAttackChar ) ) )
	{
		INT32 lOwnerCID = m_pagpmSummons->GetOwnerCID( pAttackChar );
		if (0 != lOwnerCID)
		{
			AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
			if (NULL != pcsCharacter)
			{
				// 소환수나 테이밍 몬스터가 공격한 경우엔 데미지는 소유자가 준것으로 하고 
				// 어그로는 해당 소환수나 테이밍 몬스터로 부여한다.
				m_pagsmCharacter->AddHistory(pTargetChar, pcsCharacter, pcsUpdateFactorResult, GetClockCount());
				m_pagsmCharacter->AddAgroPoint(pTargetChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_STATIC, lTotalDamage);

				pcsCharacter->m_Mutex.Release();
			}
		}
	}
	// 공격자가 몬스터이고 타겟이 PC일 경우의 처리
	else if ( m_pagpmCharacter->IsMonster( pAttackChar ) && m_pagpmCharacter->IsPC( pTargetChar ) )
	{
		// 만약 타겟 PC가 소환수, 테이밍 몬스터를 가지고 있다면 PC를 때린 몬스터의 어그로를 소환수, 테이밍 몬스터에게 부여해준다.
		AgpdSummonsADChar *pcsSummonsADChar = m_pagpmSummons->GetADCharacter( pTargetChar );
		ASSERT(pcsSummonsADChar);

		if (pcsSummonsADChar)
		{
			AgpdCharacter* pcsTempChar = NULL;

			if(!pcsSummonsADChar->m_SummonsArray.m_pSummons)
				return FALSE;

			AgpdSummonsArray::SummonsVector tmpVector(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
														pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
			AgpdSummonsArray::iterator iter = tmpVector.begin();
			while(iter != tmpVector.end())
			{
				pcsTempChar = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
				ASSERT(pcsTempChar);

				if (pcsTempChar)
				{
					m_pagsmCharacter->AddAgroPoint(pcsTempChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_STATIC, lTotalDamage);

					pcsTempChar->m_Mutex.Release();
				}

				++iter;
			}

			for (int i = 0; i < pcsSummonsADChar->m_TameArray.m_arrTame.size(); ++i)
			{
				pcsTempChar = m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_TameArray.m_arrTame[i].m_lCID);
				ASSERT(pcsTempChar);

				if (pcsTempChar)
				{
					m_pagsmCharacter->AddAgroPoint(pcsTempChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_STATIC, lTotalDamage);

					pcsTempChar->m_Mutex.Release();
				}
			}
		}
	}

	// 2005.11.29. steeple
	// 몹 몰이 패널티를 위한 작업
	if(m_pagpmCharacter->IsPC(pAttackChar) && m_pagpmCharacter->IsMonster(pTargetChar))
	{
		m_pagsmCharacter->AddTargetInfo(pAttackChar, pTargetChar->m_lID, GetClockCount());
	}

	// 아템 내구도를 수정한다.
	//////////////////////////////////////////////////////////////////////////////////
	INT32			lItemID	= 0;

	AgpdItem	*pcsWeapon = m_pagpmItem->GetEquipWeapon( pAttackChar );

	if( pcsWeapon != NULL )
	{
		// 공격자 무기 내구도 수정
		m_pagsmItem->ReduceDurability(pcsWeapon);
	}

	// 피해자 방어구 내구도 수정
	m_pagsmItem->ReduceArmourDurability(pTargetChar);
	// 아템 내구도를 수정한다.
	//////////////////////////////////////////////////////////////////////////////////

	stAgsmCombatAttackResult	stAttackResult;
	memset(&stAttackResult, 0, sizeof(stAttackResult));
	stAttackResult.pAttackChar	= pAttackChar;
	stAttackResult.pTargetChar	= pTargetChar;
	stAttackResult.nDamage		= lTotalDamage;

	EnumCallback(AGSMCOMBAT_CB_ID_ATTACK, &stAttackResult, pcsUpdateFactor);

	m_pagpmFactors->DestroyFactor(pcsUpdateFactor);

	if (bAllocFactor)
		delete pcsUpdateFactor;

	return TRUE;
}

// 2006.06.23. steeple
// 높이에 따른 데미지 변화값을 리턴한다.
INT32 AgsmCombat::AdjustHeightDamage(AgpdCharacter* pAttackChar, AgpdCharacter* pTargetChar, INT32 lDamage)
{
	if(!pAttackChar || !pTargetChar || !lDamage)
		return 0;

	// 일단 높이차이가 나야지 뭘 하던지 말던지 한다.

	// 현재 (2006.06.23) 는 공성일 때만 작업한다.

	return 0;
}

// 2007.06.27. steeple
// Divide Damage 관련 작업.
INT32 AgsmCombat::ProcessDivide(stAgsmCombatAttackResult& stAttackResult, AgpdFactor* pcsUpdateFactor, INT32* plTotalDamage)
{
	if(!pcsUpdateFactor)
		return 0;

	if(!stAttackResult.nDamage && !stAttackResult.lDivideAttrRate && !stAttackResult.lDivideAttrCaster &&
		!stAttackResult.lDivideNormalRate && !stAttackResult.lDivideNormalCaster)
		return 0;

	// 일반데미지
	if(stAttackResult.lDivideNormalRate && stAttackResult.lDivideNormalCaster)
	{
		// Caster 가 있어야만 작업한다. 또한 자기 자신은 아니여야 한다.
		AgpdCharacter* pcsCaster = m_pagpmCharacter->GetCharacter(stAttackResult.lDivideNormalCaster);
		if(pcsCaster && !pcsCaster->m_bIsReadyRemove && pcsCaster->m_lID != stAttackResult.pTargetChar->m_lID)
		{
			AuAutoLock csLock(pcsCaster->m_Mutex);

			INT32 lReduceDamage = (INT32)((double)stAttackResult.nDamage * (double)stAttackResult.lDivideNormalRate / (double)100);

			if(lReduceDamage > 0)
			{
				stAttackResult.nDamage -= lReduceDamage;
				m_pagpmFactors->SetValue(pcsUpdateFactor, -(stAttackResult.nDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);

				// 줄인 만큼의 데미지는 Caster 에게 간다.
				m_pagsmCharacter->DecreaseHP(pcsCaster, lReduceDamage);

				// 추가효과 준다.
				m_pagsmCharacter->SendPacketCharUseEffect(pcsCaster, AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP);			
			}

			// 줄인 만큼의 데미지는 Caster 에게 간다.
			//ApplyAttackDamage(stAttackResult.pAttackChar, pcsCaster, lReduceDamage, lReduceDamage, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS, 0, TRUE/*force*/, 0);
		}
	}

	// 속성데미지
	if(stAttackResult.lDivideAttrRate && stAttackResult.lDivideAttrCaster)
	{
		// Caster 가 있어야만 작업한다. 또한 자기 자신은 아니여야 한다.
		AgpdCharacter* pcsCaster = m_pagpmCharacter->GetCharacter(stAttackResult.lDivideAttrCaster);
		if(pcsCaster && !pcsCaster->m_bIsReadyRemove && pcsCaster->m_lID != stAttackResult.pTargetChar->m_lID)
		{
			AuAutoLock csLock(pcsCaster->m_Mutex);

			INT32 lReduceDamage = 0;
			INT32 lReduceDamageSum = 0;

			// 속성 데미지를 다 구해온다.
			ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lSpiritDamage;
			lSpiritDamage.MemSetAll();

			int j = 0;
			for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				m_pagpmFactors->GetValue(pcsUpdateFactor, &lSpiritDamage[i], AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC + j);
				lSpiritDamage[i] = -(lSpiritDamage[i]);	// 음수로 되어 있는 거 양수로 일단 바꾼다.
				++j;
			}

			j = 0;
			for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				lReduceDamage = 0;
				lReduceDamage = (INT32)((double)lSpiritDamage[i] * (double)stAttackResult.lDivideAttrRate / (double)100);
				lReduceDamageSum += lReduceDamage;

				// 값 다시 세팅해준다.
				if(lReduceDamage > 0)
					m_pagpmFactors->SetValue(pcsUpdateFactor, -(lSpiritDamage[i] - lReduceDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC + j);

				++j;
			}

			if(lReduceDamageSum > 0 && plTotalDamage && *plTotalDamage > lReduceDamageSum)
			{
				*plTotalDamage -= lReduceDamageSum;
				m_pagpmFactors->SetValue(pcsUpdateFactor, -(*plTotalDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

				// 줄인 만큼의 데미지는 Caster 에게 간다.
				m_pagsmCharacter->DecreaseHP(pcsCaster, lReduceDamageSum);

				// 추가효과 준다.
				m_pagsmCharacter->SendPacketCharUseEffect(pcsCaster, AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP);
			}

			// 줄인 만큼의 데미지는 Caster 에게 간다.
			//ApplyAttackDamage(stAttackResult.pAttackChar, pcsCaster, lReduceDamageSum, lReduceDamageSum, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS, 0, TRUE/*force*/, 0);
		}
	}

	return stAttackResult.nDamage;
}

// 2007.07.10. steeple
// 속성무적
BOOL AgsmCombat::IsAttrInvincible(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(m_pagpmCharacter->IsStatusAttrInvincible(pcsCharacter) == FALSE)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT32 lRand = m_pagpmCombat->m_csRandom.randInt(100);
	if(pcsAgsdCharacter->m_stInvincibleInfo.lAttrProbability > lRand)
		return TRUE;

	return FALSE;
}

void AgsmCombat::KillMonster(int attackerCID, int monsterCID)
{
	AgpdCharacter *pAttacker = m_pagpmCharacter->GetCharacterLock(attackerCID);
	if (NULL == pAttacker)
		return;

	AgpdCharacter *pMonster = m_pagpmCharacter->GetCharacterLock(monsterCID);
	if (NULL == pMonster)
	{
		pAttacker->m_Mutex.Release();
		return;
	}

	INT32	lCurrentTargetHP	= 0;
	m_pagpmFactors->GetValue(&pMonster->m_csFactor, &lCurrentTargetHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	ApplyAttackDamage(pAttacker, pMonster, lCurrentTargetHP, lCurrentTargetHP, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS);

	if (pMonster)
		pMonster->m_Mutex.Release();
	if (pAttacker)
		pAttacker->m_Mutex.Release();
}