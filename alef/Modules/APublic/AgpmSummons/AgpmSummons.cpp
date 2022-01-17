// AgpmSummons.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2005. 07. 26.

#include "AgpmSummons.h"
#include <algorithm>

AgpmSummons::AgpmSummons()
{
	SetModuleName("AgpmSummons");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle2(FALSE);

	m_pagpmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmSystemMessage = NULL;

	m_nIndexADCharacter = -1;

	SetPacketType(AGPMSUMMONS_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// Owner CID
							AUTYPE_INT32,			1,	// Summons CID
							AUTYPE_INT32,			1,	// Summons TID
							AUTYPE_INT8,			1,	// Summons Type
							AUTYPE_INT8,			1,	// Summons Propensity
							AUTYPE_UINT32,			1,	// Start Time
							AUTYPE_UINT32,			1,	// End Time
							AUTYPE_UINT8,			1,	// Force
							AUTYPE_END,				0
							);
}

AgpmSummons::~AgpmSummons()
{
}

BOOL AgpmSummons::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmSystemMessage = (AgpmSystemMessage*)GetModule("AgpmSystemMessage");

	if(!m_pagpmCharacter || !m_pagpmFactors)
		return FALSE;

	m_nIndexADCharacter = m_pagpmCharacter->AttachCharacterData(this, sizeof(AgpdSummonsADChar), ConAgpdSummonsADChar, DesAgpdSummonsADChar);
	if(m_nIndexADCharacter < 0)
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackResetMonster(CBResetMonster, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmSummons::OnInit()
{
	return TRUE;
}

BOOL AgpmSummons::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmSummons::OnDestroy()
{
	return TRUE;
}

AgpdSummonsADChar* AgpmSummons::GetADCharacter(AgpdCharacter* pData)
{
	AgpdSummonsADChar* pcsSummonsADChar = NULL;
	if(m_pagpmCharacter)
		pcsSummonsADChar = (AgpdSummonsADChar*)m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID)pData);

	if(!pcsSummonsADChar)
		return NULL;

	return pcsSummonsADChar;
}

BOOL AgpmSummons::ConAgpdSummonsADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpmSummons* pThis = static_cast<AgpmSummons*>(pClass);

	AgpdSummonsADChar* pcsSummonsADChar = pThis->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	ZeroMemory(&pcsSummonsADChar->m_SummonsArray, sizeof(pcsSummonsADChar->m_SummonsArray));
	ZeroMemory(&pcsSummonsADChar->m_TameArray, sizeof(pcsSummonsADChar->m_TameArray));
	ZeroMemory(&pcsSummonsADChar->m_FixedArray, sizeof(pcsSummonsADChar->m_FixedArray));
	ZeroMemory(&pcsSummonsADChar->m_stSummonsData, sizeof(pcsSummonsADChar->m_stSummonsData));

	pcsSummonsADChar->m_SummonsArray.m_cMaxNum = 1;
	pcsSummonsADChar->m_SummonsArray.m_pSummons = new AgpdSummonsArray::SummonsVector();
	pcsSummonsADChar->m_SummonsArray.m_pSummons->reserve(AGPMSUMMONS_MAX_SUMMONS);

	pcsSummonsADChar->m_TameArray.m_cMaxNum = AGPMSUMMONS_MAX_TAME;			// 이놈은 스킬로 좌우되기 땀시 처음부터 맥스로 줘 버린다.
	pcsSummonsADChar->m_FixedArray.m_cMaxNum = AGPMSUMMONS_MAX_FIXED;		// 이놈은 스킬로 좌우되기 땀시 처음부터 맥스로 줘 버린다.
	pcsSummonsADChar->m_lOwnerCID = 0;
	pcsSummonsADChar->m_lOwnerTID = 0;
	
	return TRUE;
}

BOOL AgpmSummons::DesAgpdSummonsADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpmSummons* pThis = static_cast<AgpmSummons*>(pClass);

	AgpdSummonsADChar* pcsSummonsADChar = pThis->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	if(pcsSummonsADChar->m_SummonsArray.m_pSummons)
	{
		delete pcsSummonsADChar->m_SummonsArray.m_pSummons;
		pcsSummonsADChar->m_SummonsArray.m_pSummons = NULL;
	}

	return TRUE;
}

BOOL AgpmSummons::CBResetMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpmSummons* pThis = static_cast<AgpmSummons*>(pClass);

	AgpdSummonsADChar* pcsSummonsADChar = pThis->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	if(pcsSummonsADChar->m_SummonsArray.m_pSummons)
	{
		delete pcsSummonsADChar->m_SummonsArray.m_pSummons;
		pcsSummonsADChar->m_SummonsArray.m_pSummons = NULL;
	}

	ZeroMemory(&pcsSummonsADChar->m_SummonsArray, sizeof(pcsSummonsADChar->m_SummonsArray));
	ZeroMemory(&pcsSummonsADChar->m_TameArray, sizeof(pcsSummonsADChar->m_TameArray));
	ZeroMemory(&pcsSummonsADChar->m_FixedArray, sizeof(pcsSummonsADChar->m_FixedArray));
	ZeroMemory(&pcsSummonsADChar->m_stSummonsData, sizeof(pcsSummonsADChar->m_stSummonsData));

	pcsSummonsADChar->m_SummonsArray.m_cMaxNum = 1;
	pcsSummonsADChar->m_SummonsArray.m_pSummons = new AgpdSummonsArray::SummonsVector();
	pcsSummonsADChar->m_SummonsArray.m_pSummons->reserve(AGPMSUMMONS_MAX_SUMMONS);

	pcsSummonsADChar->m_TameArray.m_cMaxNum = AGPMSUMMONS_MAX_TAME;			// 이놈은 스킬로 좌우되기 땀시 처음부터 맥스로 줘 버린다.
	pcsSummonsADChar->m_FixedArray.m_cMaxNum = AGPMSUMMONS_MAX_FIXED;		// 이놈은 스킬로 좌우되기 땀시 처음부터 맥스로 줘 버린다.
	pcsSummonsADChar->m_lOwnerCID = 0;
	pcsSummonsADChar->m_lOwnerTID = 0;
	
	return TRUE;
}

BOOL AgpmSummons::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	INT32 lOwnerCID = -1;
	INT32 lSummonsCID = -1;
	INT32 lSummonsTID = -1;
	INT8 cSummonsType = -1;
	INT8 cSummonsPropensity = -1;
	UINT32 ulStartTime = 0;
	UINT32 ulEndTime = 0;
	UINT8 ucForce = 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lOwnerCID,
						&lSummonsCID,
						&lSummonsTID,
						&cSummonsType,
						&cSummonsPropensity,
						&ulStartTime,
						&ulEndTime,
						&ucForce);

	switch(cOperation)
	{
		case AGPMSUMMONS_PACKET_NEW_SUMMONS:
			OnReceiveNewSummons(lOwnerCID, lSummonsCID, lSummonsTID, (EnumAgpdSummonsType)cSummonsType,
								(EnumAgpdSummonsPropensity)cSummonsPropensity, ulStartTime, ulEndTime, ucForce);
			break;

		case AGPMSUMMONS_PACKET_REMOVE_SUMMONS:
			OnReceiveRemoveSummons(lOwnerCID, lSummonsCID);
			break;

		case AGPMSUMMONS_PACKET_SET_PROPENSITY:
			OnReceiveSetPropensity(lOwnerCID, lSummonsCID, (EnumAgpdSummonsPropensity)cSummonsPropensity);
			break;

		case AGPMSUMMONS_PACKET_SET_MAX_SUMMONS:
			OnReceiveSetMaxSummons(lOwnerCID, cSummonsType);
			break;

		case AGPMSUMMONS_PACKET_SET_MAX_TAME:
			OnReceiveSetMaxTame(lOwnerCID, cSummonsType);
			break;
	}

	return TRUE;
}

BOOL AgpmSummons::OnReceiveNewSummons(INT32 lOwnerCID, INT32 lSummonsCID, INT32 lSummonsTID, EnumAgpdSummonsType eSummonsType,
									  EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce)
{
	ASSERT(1 < lOwnerCID);
	ASSERT(1 < lSummonsCID);
	ASSERT(1 < lSummonsTID);
	ASSERT(0 < eSummonsType);
	ASSERT(0 <= eSummonsPropensity);
	ASSERT(0 != ulStartTime);
	ASSERT(0 != ulEndTime);

	if(lOwnerCID < 1 || lSummonsCID < 1 || lSummonsTID < 1 || eSummonsType < 0 || eSummonsPropensity < 0 || ulStartTime == 0 || ulEndTime == 0)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
	if(!pcsCharacter)
		return FALSE;

	if (AGPMSUMMONS_TYPE_ELEMENTAL == eSummonsType || AGPMSUMMONS_TYPE_PET == eSummonsType || AGPMSUMMONS_TYPE_MONSTER == eSummonsType)
	{
		if(NewSummons(pcsCharacter, m_pagpmCharacter->GetCharacter(lSummonsCID), lSummonsTID, eSummonsType, eSummonsPropensity, ulStartTime, ulEndTime, bForce))
			EnumCallback(AGPMSUMMONS_CB_NEW_SUMMONS, pcsCharacter, &lSummonsCID);
	}
	else if (AGPMSUMMONS_TYPE_TAME == eSummonsType)
	{
		if(NewTaming(pcsCharacter, m_pagpmCharacter->GetCharacter(lSummonsCID), lSummonsTID, eSummonsPropensity, ulStartTime, ulEndTime, bForce))
			EnumCallback(AGPMSUMMONS_CB_NEW_SUMMONS, pcsCharacter, &lSummonsCID);
	}
	else if (AGPMSUMMONS_TYPE_FIXED == eSummonsType)
	{
		if(NewFixed(pcsCharacter, m_pagpmCharacter->GetCharacter(lSummonsCID), lSummonsTID, eSummonsPropensity, ulStartTime, ulEndTime, bForce))
			EnumCallback(AGPMSUMMONS_CB_NEW_SUMMONS, pcsCharacter, &lSummonsCID);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSummons::OnReceiveRemoveSummons(INT32 lOwnerCID, INT32 lSummonsCID)
{
	if(lOwnerCID < 1 || lSummonsCID < 1)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
	if(!pcsCharacter)
		return FALSE;

	if(RemoveSummons(pcsCharacter, lSummonsCID))
		EnumCallback(AGPMSUMMONS_CB_REMOVE_SUMMONS, pcsCharacter, &lSummonsCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2005.09.26. steeple
BOOL AgpmSummons::OnReceiveSetPropensity(INT32 lOwnerCID, INT32 lSummonsCID, EnumAgpdSummonsPropensity eSummonsPropensity)
{
	if(lOwnerCID < 1 || lSummonsCID < 1)
		return FALSE;

	AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
	if(!pcsOwner)
		return FALSE;

	AgpdCharacter* pcsSummons = m_pagpmCharacter->GetCharacterLock(lSummonsCID);
	if(!pcsSummons)
	{
		pcsOwner->m_Mutex.Release();
		return FALSE;
	}

	// 이 작업은 소환수만 가능하다.
	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsSummons);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar || pcsSummonsADChar->m_stSummonsData.m_eType != AGPMSUMMONS_TYPE_ELEMENTAL)
	{
		pcsSummons->m_Mutex.Release();
		pcsOwner->m_Mutex.Release();
		return FALSE;
	}

	if(IsMySummons(pcsOwner, pcsSummons->m_lID))
	{
		// Owner 것도 바꿔준다.
		AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsOwner);
		if(pcsSummonsADChar && pcsSummonsADChar->m_SummonsArray.m_pSummons)
		{
			AgpdSummonsArray::iterator iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
														pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
														pcsSummons->m_lID);
			if(iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
				iter->m_ePropensity = eSummonsPropensity;
		}

		SetPropensity(pcsSummons, eSummonsPropensity);
		EnumCallback(AGPMSUMMONS_CB_SET_PROPENSITY, pcsOwner, pcsSummons);
	}

    pcsSummons->m_Mutex.Release();
	pcsOwner->m_Mutex.Release();

	return TRUE;
}

// 2005.09.26. steeple
BOOL AgpmSummons::OnReceiveSetMaxSummons(INT32 lOwnerCID, INT8 cValue)
{
	if(lOwnerCID < 1 || cValue < 0)
		return FALSE;

	AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
	if(!pcsOwner)
		return FALSE;

	SetMaxSummonsCount(pcsOwner, cValue);
	pcsOwner->m_Mutex.Release();

	return TRUE;
}

// 2005.09.26. steeple
BOOL AgpmSummons::OnReceiveSetMaxTame(INT32 lOwnerCID, INT8 cValue)
{
	if(lOwnerCID < 1 || cValue < 0)
		return FALSE;

	AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
	if(!pcsOwner)
		return FALSE;

	SetMaxTameCount(pcsOwner, cValue);
	pcsOwner->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSummons::NewSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsSummons, INT32 lSummonsTID, 
							 EnumAgpdSummonsType eSummonsType, EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsSummons);
	ASSERT(1 < lSummonsTID);
	ASSERT(0 <= eSummonsPropensity);
	ASSERT(0 != ulStartTime);
	ASSERT(0 != ulEndTime);

	if(!pcsCharacter || !pcsSummons || lSummonsTID < 1 || eSummonsType < 0 || eSummonsPropensity < 0 || ulStartTime == 0 || ulEndTime == 0)
		return FALSE;

	AuAutoLock lock(pcsSummons->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (eSummonsType != AGPMSUMMONS_TYPE_MONSTER)
	{
		if (FALSE == m_pagpmCharacter->IsStatusSummoner(pcsSummons))
			return FALSE;
	}

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	// Max 치 이상 소환 했다.
	if(!bForce && pcsSummonsADChar->m_SummonsArray.m_cMaxNum <= GetSummonsCount(pcsCharacter))
		return FALSE;

	// 소환수로 등록되어 있는데 또 다시 등록 하려고 하는지 확인
	AgpdSummonsArray::iterator iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
												pcsSummons->m_lID);

	// 찾지 못해야 정상
	//ASSERT(false == pcsSummonsADChar->m_SummonsArray.m_arrSummons.IsValidIterator(iter));

	// 이미 등록되어 있는 소환수 이므로 종료
	if (iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
		return FALSE;

	// 비어있는 곳을 찾는다.
	if(!bForce)
	{
		ASSERT(pcsSummonsADChar->m_SummonsArray.m_pSummons->size() < ( UINT8 ) pcsSummonsADChar->m_SummonsArray.m_cMaxNum);
		if (pcsSummonsADChar->m_SummonsArray.m_pSummons->size() >= ( UINT8 ) pcsSummonsADChar->m_SummonsArray.m_cMaxNum)
			return FALSE;
	}

	AgpdSummons Summon;
	Summon.m_lCID			= pcsSummons->m_lID;
	Summon.m_lTID			= lSummonsTID;
	Summon.m_ePropensity	= eSummonsPropensity;
	Summon.m_eType			= eSummonsType;
	Summon.m_ulStartTime	= ulStartTime;
	Summon.m_ulEndTime		= ulEndTime;

	pcsSummonsADChar->m_SummonsArray.m_pSummons->push_back(Summon);

	// 2007.03.26. steeple
	// Pet 소환했을 때는 Max 를 하나 늘려준다.
	if(eSummonsType == AGPMSUMMONS_TYPE_PET)
		SetMaxSummonsCount(pcsCharacter, pcsSummonsADChar->m_SummonsArray.m_cMaxNum + 1);

	if(!SetOwnerAndLifeTime(pcsSummons, pcsCharacter->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID, eSummonsType, eSummonsPropensity, ulStartTime, ulEndTime))
		return FALSE;

	return TRUE;
}

BOOL AgpmSummons::NewTaming(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTame, INT32 lTameTID, 
							EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce)
{

	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsTame);
	ASSERT(1 < lTameTID);
	ASSERT(0 <= eSummonsPropensity);
	ASSERT(0 != ulStartTime);
	ASSERT(0 != ulEndTime);

	if(!pcsCharacter || !pcsTame || lTameTID < 1 || eSummonsPropensity < 0 || ulStartTime == 0 || ulEndTime == 0)
		return FALSE;

	AuAutoLock lock(pcsTame->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (FALSE == m_pagpmCharacter->IsStatusTame(pcsTame))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	// Max 치 이상 소환 했다.
	if(!bForce && pcsSummonsADChar->m_TameArray.m_cMaxNum <= GetTameCount(pcsCharacter))
		return FALSE;

	// 테이밍에 등록되어 있는데 또 다시 등록 하려고 하는지 확인
	AgpdTameArray::iterator iter = std::find(pcsSummonsADChar->m_TameArray.m_arrTame.begin(),
												pcsSummonsADChar->m_TameArray.m_arrTame.end(),
												pcsTame->m_lID);

	// 찾지 못해야 정상
	ASSERT(false == pcsSummonsADChar->m_TameArray.m_arrTame.IsValidIterator(iter));

	// 이미 등록되어 있으므로 종료
	if (iter != pcsSummonsADChar->m_TameArray.m_arrTame.end())
		return FALSE;

	// 추가 가능한지 확인
	ASSERT(pcsSummonsADChar->m_TameArray.m_arrTame.size() < pcsSummonsADChar->m_TameArray.m_arrTame.GetLength() - 1);
	if (pcsSummonsADChar->m_TameArray.m_arrTame.size() >= pcsSummonsADChar->m_TameArray.m_arrTame.GetLength() - 1)
		return FALSE;

	AgpdSummons Tame;

	Tame.m_lCID			= pcsTame->m_lID;
	Tame.m_lTID			= lTameTID;
	Tame.m_ePropensity	= eSummonsPropensity;
	Tame.m_eType		= AGPMSUMMONS_TYPE_TAME;
	Tame.m_ulStartTime	= ulStartTime;
	Tame.m_ulEndTime	= ulEndTime;

	pcsSummonsADChar->m_TameArray.m_arrTame.push_back(Tame);

	if(!SetOwnerAndLifeTime(pcsTame, pcsCharacter->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID, AGPMSUMMONS_TYPE_TAME, eSummonsPropensity, ulStartTime, ulEndTime))
		return FALSE;

	return TRUE;
}

// 2005.10.04. steeple
BOOL AgpmSummons::NewFixed(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFixed, INT32 lFixedTID, 
					EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce)
{

	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsFixed);
	ASSERT(1 < lFixedTID);
	ASSERT(0 <= eSummonsPropensity);
	ASSERT(0 != ulStartTime);
	ASSERT(0 != ulEndTime);

	if(!pcsCharacter || !pcsFixed || lFixedTID < 1 || eSummonsPropensity < 0 || ulStartTime == 0 || ulEndTime == 0)
		return FALSE;

	AuAutoLock lock(pcsFixed->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (FALSE == m_pagpmCharacter->IsStatusFixed(pcsFixed))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	// Max 치 이상 소환 했다.
	if(!bForce && pcsSummonsADChar->m_FixedArray.m_cMaxNum <= GetAllFixedCount(pcsCharacter))
		return FALSE;

	// Fixed에 등록되어 있는데 또 다시 등록 하려고 하는지 확인
	AgpdFixedArray::iterator iter = std::find(pcsSummonsADChar->m_FixedArray.m_arrFixed.begin(),
												pcsSummonsADChar->m_FixedArray.m_arrFixed.end(),
												pcsFixed->m_lID);

	// 찾지 못해야 정상
	ASSERT(false == pcsSummonsADChar->m_FixedArray.m_arrFixed.IsValidIterator(iter));

	// 이미 등록되어 있으므로 종료
	if (iter != pcsSummonsADChar->m_FixedArray.m_arrFixed.end())
		return FALSE;

	// 비어있는 곳을 찾는다.
	ASSERT(pcsSummonsADChar->m_FixedArray.m_arrFixed.size() < pcsSummonsADChar->m_FixedArray.m_arrFixed.GetLength() - 1);
	if (pcsSummonsADChar->m_FixedArray.m_arrFixed.size() >= pcsSummonsADChar->m_FixedArray.m_arrFixed.GetLength() - 1)
		return FALSE;

	AgpdSummons Fixed;
	Fixed.m_lCID		= pcsFixed->m_lID;
	Fixed.m_lTID		= lFixedTID;
	Fixed.m_ePropensity	= eSummonsPropensity;
	Fixed.m_eType		= AGPMSUMMONS_TYPE_FIXED;
	Fixed.m_ulStartTime	= ulStartTime;
	Fixed.m_ulEndTime	= ulEndTime;

	pcsSummonsADChar->m_FixedArray.m_arrFixed.push_back(Fixed);

	if(!SetOwnerAndLifeTime(pcsFixed, pcsCharacter->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID, AGPMSUMMONS_TYPE_FIXED, eSummonsPropensity, ulStartTime, ulEndTime))
		return FALSE;

	return TRUE;
}

BOOL AgpmSummons::SetOwnerAndLifeTime(AgpdCharacter* pcsSummons, INT32 lOwnerCID, INT32 lOwnerTID, EnumAgpdSummonsType eSummonsType, 
									  EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime)
{
	if(!pcsSummons || lOwnerCID < 1 || lOwnerTID == AP_INVALID_CID || eSummonsType < 0 || eSummonsPropensity < 0 || ulStartTime == 0 || ulEndTime == 0)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsSummons);
	if(!pcsSummonsADChar)
		return FALSE;

	// 이 전 주인은 쌩까고 걍 새로 덮어 쓰자.
	pcsSummonsADChar->m_lOwnerCID = lOwnerCID;
	pcsSummonsADChar->m_lOwnerTID = lOwnerTID;

	// 자기 자신의 정보도 세팅.
	pcsSummonsADChar->m_stSummonsData.m_lCID = pcsSummons->m_lID;
	pcsSummonsADChar->m_stSummonsData.m_lTID = pcsSummons->m_pcsCharacterTemplate->m_lID;
	pcsSummonsADChar->m_stSummonsData.m_eType = eSummonsType;
	pcsSummonsADChar->m_stSummonsData.m_ePropensity = eSummonsPropensity;
	pcsSummonsADChar->m_stSummonsData.m_ulStartTime = ulStartTime;
	pcsSummonsADChar->m_stSummonsData.m_ulEndTime = ulEndTime;

	return TRUE;
}

BOOL AgpmSummons::RemoveSummons(AgpdCharacter* pcsCharacter, INT32 lSummonCID)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(0 != lSummonCID);

	if(!pcsCharacter || !lSummonCID)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	AgpdSummonsArray::iterator iterSummon = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
														pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
														lSummonCID);

	if (iterSummon != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
	{
		// 소환수의 CID이다.
		AgpdCharacter *pcsSummons = m_pagpmCharacter->GetCharacter((*iterSummon).m_lCID);
		if (pcsSummons)
		{
			m_pagpmCharacter->SetStatusSummoner(pcsSummons, FALSE);
			RemoveOwner(pcsSummons, pcsCharacter->m_lID);
		}

		RemoveSummonsInOwner(pcsCharacter, lSummonCID);
		return TRUE;
	}


	AgpdTameArray::iterator iterTame = std::find(pcsSummonsADChar->m_TameArray.m_arrTame.begin(),
														pcsSummonsADChar->m_TameArray.m_arrTame.end(),
														lSummonCID);

	if (true == pcsSummonsADChar->m_TameArray.m_arrTame.IsValidIterator(iterTame))
	{
		// 소환수의 CID이다.
		AgpdCharacter *pcsTame = m_pagpmCharacter->GetCharacter((*iterTame).m_lCID);
		if (pcsTame)
		{
			m_pagpmCharacter->SetStatusSummoner(pcsTame, FALSE);
			RemoveOwner(pcsTame, pcsCharacter->m_lID);
		}

		RemoveTameInOwner(pcsCharacter, lSummonCID);
		return TRUE;
	}


	AgpdFixedArray::iterator iterFixed = std::find(pcsSummonsADChar->m_FixedArray.m_arrFixed.begin(),
														pcsSummonsADChar->m_FixedArray.m_arrFixed.end(),
														lSummonCID);

	if (true == pcsSummonsADChar->m_FixedArray.m_arrFixed.IsValidIterator(iterFixed))
	{
		// 소환수의 CID이다.
		AgpdCharacter *pcsFixed = m_pagpmCharacter->GetCharacter((*iterFixed).m_lCID);
		if (pcsFixed)
		{
			m_pagpmCharacter->SetStatusFixed(pcsFixed, FALSE);
			RemoveOwner(pcsFixed, pcsCharacter->m_lID);
		}

		RemoveFixedInOwner(pcsCharacter, lSummonCID);
		return TRUE;
	}

	return TRUE;
}

BOOL AgpmSummons::RemoveSummonsInOwner(AgpdCharacter* pcsOwner, INT32 lSummonsCID)
{
	if(!pcsOwner || lSummonsCID < 1)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsOwner);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	AgpdSummonsArray::iterator iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
												lSummonsCID);

	ASSERT(iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
	if (iter == pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
		return FALSE;

	// 2007.03.26. steeple
	// Pet 이라면 Max 하나 줄여준다.
	if(iter->m_eType == AGPMSUMMONS_TYPE_PET)
		SetMaxSummonsCount(pcsOwner, pcsSummonsADChar->m_SummonsArray.m_cMaxNum - 1);

	pcsSummonsADChar->m_SummonsArray.m_pSummons->erase(iter);

	return TRUE;
}

BOOL AgpmSummons::RemoveTameInOwner(AgpdCharacter* pcsOwner, INT32 lTameCID)
{
	if(!pcsOwner || lTameCID < 1)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsOwner);
	if(!pcsSummonsADChar)
		return FALSE;

	AgpdTameArray::iterator iter = std::find(pcsSummonsADChar->m_TameArray.m_arrTame.begin(),
											pcsSummonsADChar->m_TameArray.m_arrTame.end(),
											lTameCID);

	ASSERT(true == pcsSummonsADChar->m_TameArray.m_arrTame.IsValidIterator(iter));
	if (iter == pcsSummonsADChar->m_TameArray.m_arrTame.end())
		return FALSE;

	pcsSummonsADChar->m_TameArray.m_arrTame.erase(iter);

	return TRUE;
}

BOOL AgpmSummons::RemoveFixedInOwner(AgpdCharacter* pcsOwner, INT32 lFixedCID)
{
	if(!pcsOwner || lFixedCID < 1)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsOwner);
	if(!pcsSummonsADChar)
		return FALSE;

	AgpdFixedArray::iterator iter = std::find(pcsSummonsADChar->m_FixedArray.m_arrFixed.begin(),
											pcsSummonsADChar->m_FixedArray.m_arrFixed.end(),
											lFixedCID);

	ASSERT(true == pcsSummonsADChar->m_FixedArray.m_arrFixed.IsValidIterator(iter));
	if (iter == pcsSummonsADChar->m_FixedArray.m_arrFixed.end())
		return FALSE;

	pcsSummonsADChar->m_FixedArray.m_arrFixed.erase(iter);

	return TRUE;
}

BOOL AgpmSummons::RemoveOwner(AgpdCharacter* pcsSummons, INT32 lOwnerCID)
{
	if(!pcsSummons)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsSummons);
	if(!pcsSummonsADChar)
		return FALSE;

	// 그냥 주인 검사는 하지 않고 빼버린다.
	pcsSummonsADChar->m_lOwnerCID = 0;
	pcsSummonsADChar->m_lOwnerTID = 0;
	memset(&pcsSummonsADChar->m_stSummonsData, 0, sizeof(pcsSummonsADChar->m_stSummonsData));

	return TRUE;
}

// 캐릭터가 Remove 될 때는 소환수를 없애주고,
// 소환수가 Remove 될 때는 Owner 에게서 빼준다.
//BOOL AgpmSummons::RemoveCharacter(AgpdCharacter* pcsCharacter)
//{
//	if(!pcsCharacter)
//		return FALSE;
//
//	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
//	if(!pcsSummonsADChar)
//		return FALSE;
//
//	// 소환수 돌면서 없애준다.
//	for(INT32 i = 0; i < AGPMSUMMONS_MAX_SUMMONS; i++)
//	{
//		if(pcsSummonsADChar->m_SummonsArray.m_arrSummons[i].m_lCID == 0)
//			break;
//
//		// 음... 이거 꼭 해야 하나..????
//		m_pagpmCharacter->RemoveCharacter(pcsSummonsADChar->m_SummonsArray.m_arrSummons[i].m_lCID);
//		pcsSummonsADChar->m_SummonsArray.m_arrSummons.erase(&pcsSummonsADChar->m_SummonsArray.m_arrSummons[i]);
//	}
//
//
//	if(pcsSummonsADChar->m_lOwnerCID != AP_INVALID_CID)		// Summons
//	{
//		// Owner 를 구한다.
//		AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_lOwnerCID);
//		if(pcsOwner)
//		{
//			RemoveSummonsInOwner(pcsOwner, pcsCharacter->m_lID);
//			pcsOwner->m_Mutex.Release();
//		}
//	}
//
//	return TRUE;
//}

// 2006.10.31. steeple
// bIsSiegeWar 를 bForce 로 바꿈. 조금 더 범용적이 됨.
//
// 2005.08.02. steeple
eAgpmSummonsSummonsCheckResult AgpmSummons::IsSummonsEnable(AgpdCharacter* pcsCharacter, BOOL bForce)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_ABNORMAL;

	INT32 lSummonsCount = GetSummonsCount(pcsCharacter);
	INT32 lMaxSummonsCount = GetMaxSummonsCount(pcsCharacter);

	// 2006.10.26. steeple
	// std vector 로 바꿨기 때문에 AGPMSUMMONS_MAX_SUMMONS 로 비교하는 건 빼준다.

	//ASSERT(AGPMSUMMONS_MAX_SUMMONS >= lMaxSummonsCount);
	// 소환수의 숫자를 늘려주는 스킬을 쓰고나면 이런 경우가 나올수 있기 때문에 주석처리
//	ASSERT(lSummonsCount <= lMaxSummonsCount);

	// 2006.08.22. steeple
	// SiegeWar 용이라면 임시로 Max 를 하나 늘려주므로 하나 더한 값을 체크한다.
	//if(bIsSiegeWar)
	//{
	//	if(lSummonsCount >= lMaxSummonsCount + 1)// ||
	//		//lMaxSummonsCount + 1 > AGPMSUMMONS_MAX_SUMMONS)
	//		return AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_MAX_COUNT;
	//}
	//else
	if(!bForce)
	{
		if(lSummonsCount >= lMaxSummonsCount)
			return AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_MAX_COUNT;
	}
	
	return AGPMSUMMONS_SUMMONS_CHECK_RESULT_SUCCESS;
}

// 2008.04.09. steeple
// 소환수를 소환할 수 있는 지 지역 체크
eAgpmSummonsSummonsCheckResult AgpmSummons::IsSummonsEnableRegion(AgpdCharacter* pcsCharacter, INT32 lSummonTID, AgpdCharacter* pcsSummon)
{
	if(!pcsCharacter || lSummonTID < 1)
	{
		AgpdCharacterTemplate* pcsCharacterTemplate = m_pagpmCharacter->GetCharacterTemplate(pcsCharacter->m_lTID1);
		INT32 lSummonLevel  = m_pagpmFactors->GetLevel(&pcsCharacterTemplate->m_csFactor);
		INT32 lLimitedLevel = m_pagpmCharacter->GetCurrentRegionLevelLimit(pcsCharacter);
		if(lLimitedLevel != 0 && lLimitedLevel < lSummonLevel)
			return AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_LEVEL_LIMIT_REGION;
	}
	else
	{
		// pcsSummon 이 있을 때는 pcsSummon 에서 얻고, 없을 때는 Template 에서 얻는다.
		INT32 lSummonLevel = 0;
		if(pcsSummon)
			lSummonLevel = m_pagpmCharacter->GetLevel(pcsSummon);
		else
		{
			AgpdCharacterTemplate* pcsCharacterTemplate = m_pagpmCharacter->GetCharacterTemplate(lSummonTID);
			if(!pcsCharacterTemplate)
				return AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_ABNORMAL;

			lSummonLevel = m_pagpmFactors->GetLevel(&pcsCharacterTemplate->m_csFactor);
		}

		INT32 lLimitedLevel = m_pagpmCharacter->GetCurrentRegionLevelLimit(pcsCharacter);
		if(lLimitedLevel != 0 && lLimitedLevel < lSummonLevel)
			return AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_LEVEL_LIMIT_REGION;
	}

	return AGPMSUMMONS_SUMMONS_CHECK_RESULT_SUCCESS;
}

// 소환수 개수 리턴.
// 소환한 소환수만 리턴하게 변경. 2005.08.31. steeple
INT32 AgpmSummons::GetSummonsCount(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return 0;

	return (INT32 ) pcsSummonsADChar->m_SummonsArray.m_pSummons->size();
}

// Max 소환수 개수 리턴.
INT32 AgpmSummons::GetMaxSummonsCount(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	return pcsSummonsADChar->m_SummonsArray.m_cMaxNum;
}

// 2005.08.31. steeple
BOOL AgpmSummons::AddMaxSummonsCount(AgpdCharacter* pcsCharacter, INT8 cValue)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(0 != cValue);

	if(!pcsCharacter || cValue == 0)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	pcsSummonsADChar->m_SummonsArray.m_cMaxNum += cValue;
	return TRUE;
}

// 2005.09.26. steeple
// 얘는 걍 세팅하는 함수
BOOL AgpmSummons::SetMaxSummonsCount(AgpdCharacter* pcsCharacter, INT8 cValue)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(0 <= cValue);

	if(!pcsCharacter || cValue < 0)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	pcsSummonsADChar->m_SummonsArray.m_cMaxNum = cValue;
	EnumCallback(AGPMSUMMONS_CB_SET_MAX_SUMMONS, pcsCharacter, &cValue);

	return TRUE;
}

// 2005.09.26. steeple
BOOL AgpmSummons::IsMySummons(AgpdCharacter* pcsCharacter, INT32 lSummonsCID)
{
	if(!pcsCharacter || lSummonsCID < 1)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	AgpdSummonsArray::iterator iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
												lSummonsCID);

	if(iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
		return TRUE;
	else
		return FALSE;
}

// 2006.10.20. steeple
INT32 AgpmSummons::GetSummonsTIDCount(AgpdCharacter* pcsCharacter, INT32 lSummonsTID)
{
	if(!pcsCharacter || !lSummonsTID)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	INT32 lCount = 0;
	AgpdSummonsArray::iterator iter = pcsSummonsADChar->m_SummonsArray.m_pSummons->begin();
	while(iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
	{
		if(lSummonsTID == iter->m_lTID)
			++lCount;

		++iter;
	}

	return lCount;
}

// 2006.11.22. steeple
BOOL AgpmSummons::UpdateAllSummonsPosToOwner(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 2006.11.16. steeple
	// 거느리고 있는 소환수가 있다면, 같이 보내준다.
	AgpdSummonsADChar* pcsADSummons = GetADCharacter(pcsCharacter);
	if(!pcsADSummons)
		return TRUE;

	AgpdSummonsArray::SummonsVector tmpVector(pcsADSummons->m_SummonsArray.m_pSummons->begin(),
												pcsADSummons->m_SummonsArray.m_pSummons->end());
	AgpdSummonsArray::iterator iter = tmpVector.begin();
	while(iter != tmpVector.end())
	{
		AgpdCharacter* pcsSummons = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(pcsSummons)
		{
			// 2008.04.08. steeple
			// 소환수가 갈 수 있는 녀석인지 체크 해야 한다.
			eAgpmSummonsSummonsCheckResult eCheckResult = IsSummonsEnableRegion(pcsCharacter,
																pcsSummons->m_pcsCharacterTemplate->m_lID,
																pcsSummons);
			if(eCheckResult != AGPMSUMMONS_SUMMONS_CHECK_RESULT_SUCCESS)
			{
				pcsSummons->m_Mutex.Release();

				// 소환수 없애준다.
				EnumCallback(AGPMSUMMONS_CB_REMOVE_SUMMONS_BY_LEVEL_LIMIT_REGION, &iter->m_lCID, NULL);

				if(eCheckResult == AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_LEVEL_LIMIT_REGION &&
					m_pagpmSystemMessage)
				{
					m_pagpmSystemMessage->ProcessSystemMessage( 0 , AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_LEVEL_LIMIT_REGION,
															-1, -1, NULL, NULL, pcsCharacter);
				}

				++iter;
				continue;
			}

			m_pagpmCharacter->StopCharacter(pcsSummons, NULL);
			m_pagpmCharacter->UpdatePosition(pcsSummons, &pcsCharacter->m_stPos);

			pcsSummons->m_Mutex.Release();
		}

		++iter;
	}

	return TRUE;
}

eAgpmSummonsTamableCheckResult AgpmSummons::IsTameEnable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bCheckFormula, BOOL bForce)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsTarget);

	if(NULL == pcsCharacter || NULL == pcsTarget || NULL == pcsTarget->m_pcsCharacterTemplate)
		return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_INVALID_TARGET;

	// 이미 MAX 찼으면 못한다.
	if(GetTameCount(pcsCharacter) >= GetMaxTameCount(pcsCharacter))
		return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MAX_COUNT;

	if(!m_pagpmCharacter->IsMonster(pcsTarget))
		return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_INVALID_TARGET;

	// 이미 누가 Tame 한 상태라면 못한다.
	if(GetOwnerCID(pcsTarget) != 0)
		return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_ALREADY_TAMING;

	if(pcsTarget->m_pcsCharacterTemplate->m_eTamableType == AGPDCHAR_TAMABLE_TYPE_NONE)
		return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_INVALID_TARGET;

	if(bCheckFormula && pcsTarget->m_pcsCharacterTemplate->m_eTamableType == AGPDCHAR_TAMABLE_TYPE_BY_FORMULA)
	{
		// 공식 계산 들어간다.
		INT32 lCharacterLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
		INT32 lTargetLevel = m_pagpmCharacter->GetLevel(pcsTarget);

		if(lCharacterLevel + lTargetLevel == 0)
			return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MISS;

		MTRand csRandom;
		INT32 lRandom = csRandom.randInt(100);
		if(lRandom > (INT32)(((FLOAT)lCharacterLevel / (FLOAT)(lCharacterLevel + lTargetLevel)) * 100.0f))
			return AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MISS;
	}

	return AGPMSUMMONS_TAMABLE_CHECK_RESULT_SUCCESS;
}

// 2005.08.16. steeple
// 테이밍한 몬스터 개수만 리턴
INT32 AgpmSummons::GetTameCount(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	return pcsSummonsADChar->m_TameArray.m_arrTame.size();
}

// 2005.08.16. steeple
// Max Taming 개수 리턴. 이놈은 보통 각각의 스킬에 따라서 결정되므로, 전체 배열 개수를 넘는지만 체크한다.
INT32 AgpmSummons::GetMaxTameCount(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	return pcsSummonsADChar->m_TameArray.m_cMaxNum;
}

BOOL AgpmSummons::SetMaxTameCount(AgpdCharacter* pcsCharacter, INT8 cValue)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(0 < cValue);

	if(!pcsCharacter)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return FALSE;

	pcsSummonsADChar->m_TameArray.m_cMaxNum = cValue;
	
	// 2005.09.26. steeple
	EnumCallback(AGPMSUMMONS_CB_SET_MAX_TAME, pcsCharacter, &cValue);
	return TRUE;
}

// 2006.10.20. steeple
INT32 AgpmSummons::GetTameTIDCount(AgpdCharacter* pcsCharacter, INT32 lTameTID)
{
	if(!pcsCharacter || !lTameTID)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	INT32 lCount = 0;
	AgpdTameArray::iterator iter = pcsSummonsADChar->m_TameArray.m_arrTame.begin();
	while(iter != pcsSummonsADChar->m_TameArray.m_arrTame.end())
	{
		if(lTameTID == iter->m_lTID)
			++lCount;

		++iter;
	}

	return lCount;
}

// 2005.10.04. steeple
eAgpmSummonsFixedCheckResult AgpmSummons::IsFixedEnable(AgpdCharacter* pcsCharacter, BOOL bForce)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return AGPMSUMMONS_FIXED_CHECK_RESULT_FAILURE_ABNORMAL;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return AGPMSUMMONS_FIXED_CHECK_RESULT_FAILURE_ABNORMAL;

	// Max 치 이상 소환 했다.
	if(!bForce && pcsSummonsADChar->m_FixedArray.m_cMaxNum <= GetAllFixedCount(pcsCharacter))
		return AGPMSUMMONS_FIXED_CHECK_RESULT_FAILURE_MAX_COUNT;

	return AGPMSUMMONS_FIXED_CHECK_RESULT_SUCCESS;
}

// 2005.10.04. steeple
INT32 AgpmSummons::GetAllFixedCount(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	return pcsSummonsADChar->m_FixedArray.m_arrFixed.size();
}

// 2005.10.04. steeple
INT32 AgpmSummons::GetEachFixedCount(AgpdCharacter* pcsCharacter, INT32 lFixedTID)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(0 != lFixedTID);

	if(!pcsCharacter || lFixedTID == 0)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	// operator== 를 CID 비교로 해놓았기 때문에 std::count 는 사용하지 못한다. ㅠㅠ
	INT32 lCount = 0;
	AgpdFixedArray::iterator iter = pcsSummonsADChar->m_FixedArray.m_arrFixed.begin();
	while(iter != pcsSummonsADChar->m_FixedArray.m_arrFixed.end())
	{
		if((*iter).m_lTID == lFixedTID)
			lCount++;

		iter++;
	}

	return lCount;
}

// 2005.10.04. steeple
INT32 AgpmSummons::GetMaxFixedCount(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	return pcsSummonsADChar->m_FixedArray.m_cMaxNum;
}

// 2006.10.20. steeple
INT32 AgpmSummons::GetFixedTIDCount(AgpdCharacter* pcsCharacter, INT32 lFixedTID)
{
	if(!pcsCharacter || !lFixedTID)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	INT32 lCount = 0;
	AgpdFixedArray::iterator iter = pcsSummonsADChar->m_FixedArray.m_arrFixed.begin();
	while(iter != pcsSummonsADChar->m_FixedArray.m_arrFixed.end())
	{
		if(lFixedTID == iter->m_lTID)
			++lCount;

		++iter;
	}

	return lCount;
}

// 2005.08.16. steeple
INT32 AgpmSummons::GetOwnerCID(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return 0;

	return pcsSummonsADChar->m_lOwnerCID;
}

BOOL AgpmSummons::IsPet(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	return pcsSummonsADChar->m_stSummonsData.m_eType == AGPMSUMMONS_TYPE_PET;
}

// 2005.09.25. steeple
EnumAgpdSummonsPropensity AgpmSummons::GetPropensity(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(!pcsCharacter)
		return AGPMSUMMONS_PROPENSITY_ATTACK;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return AGPMSUMMONS_PROPENSITY_ATTACK;

	return pcsSummonsADChar->m_stSummonsData.m_ePropensity;
}

// 2005.09.26. steeple
BOOL AgpmSummons::SetPropensity(AgpdCharacter* pcsCharacter, EnumAgpdSummonsPropensity eSummonsPropensity)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	ASSERT(NULL != pcsSummonsADChar);

	if(!pcsSummonsADChar)
		return FALSE;

	pcsSummonsADChar->m_stSummonsData.m_ePropensity = eSummonsPropensity;
	return TRUE;
}

// 2008.06.19. steeple.
// Return the first summon's CID in a summons vector.
INT32 AgpmSummons::GetSummonsCIDByTID(AgpdCharacter* pcsCharacter, INT32 lTID)
{
	if(!pcsCharacter || lTID < 1)
		return 0;

	AgpdSummonsADChar* pcsSummonsADChar = GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return 0;

	AgpdSummonsArray::iterator iter = pcsSummonsADChar->m_SummonsArray.m_pSummons->begin();
	while(iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
	{
		if(lTID == iter->m_lTID)
			return iter->m_lCID;

		++iter;
	}

	return 0;
}

PVOID AgpmSummons::MakePacket(INT16* pnPacketLength, INT8 cOperation, INT32* plOwnerCID, INT32* plSummonsCID, INT32* plSummonsTID,
								INT8* pcSummonsType, INT8* pcSummonsPropensity, UINT32* pulStartTime, UINT32* pulEndTime, UINT8 ucForce)
{
	if(!pnPacketLength)
		return NULL;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, (INT8)AGPMSUMMONS_PACKET_TYPE,
					&cOperation,
					plOwnerCID,
					plSummonsCID,
					plSummonsTID,
					pcSummonsType,
					pcSummonsPropensity,
					pulStartTime,
					pulEndTime,
					&ucForce);
}

PVOID AgpmSummons::MakeNewSummonsPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT32 lSummonsCID, INT32 lSummonsTID, 
										INT8 cSummonsType, INT8 cSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce)
{
	if(!pnPacketLength)
		return NULL;

	return MakePacket(pnPacketLength, (INT8)AGPMSUMMONS_PACKET_NEW_SUMMONS, &lOwnerCID, &lSummonsCID, &lSummonsTID,
						&cSummonsType, &cSummonsPropensity, &ulStartTime, &ulEndTime, (UINT8)bForce);
}

PVOID AgpmSummons::MakeRemoveSummonsPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT32 lSummonsCID)
{
	if(!pnPacketLength)
		return NULL;

	return MakePacket(pnPacketLength, (INT8)AGPMSUMMONS_PACKET_REMOVE_SUMMONS, &lOwnerCID, &lSummonsCID,
						NULL, NULL, NULL, NULL, NULL, 0);
}

PVOID AgpmSummons::MakeSetPropensityPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT32 lSummonsCID, INT8 cSummonsPropensity)
{
	if(!pnPacketLength)
		return NULL;

	return MakePacket(pnPacketLength, (INT8)AGPMSUMMONS_PACKET_SET_PROPENSITY, &lOwnerCID, &lSummonsCID,
						NULL, NULL, &cSummonsPropensity, NULL, NULL, 0);
}

PVOID AgpmSummons::MakeSetMaxSummonsPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT8 cValue)
{
	if(!pnPacketLength)
		return NULL;

	return MakePacket(pnPacketLength, (INT8)AGPMSUMMONS_PACKET_SET_MAX_SUMMONS, &lOwnerCID, NULL,
						NULL, &cValue, NULL, NULL, NULL, 0);
}

PVOID AgpmSummons::MakeSetMaxTamePacket(INT16* pnPacketLength, INT32 lOwnerCID, INT8 cValue)
{
	if(!pnPacketLength)
		return NULL;

	return MakePacket(pnPacketLength, (INT8)AGPMSUMMONS_PACKET_SET_MAX_TAME, &lOwnerCID, NULL,
						NULL, &cValue, NULL, NULL, NULL, 0);
}

BOOL AgpmSummons::SetCallbackNewSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSUMMONS_CB_NEW_SUMMONS, pfCallback, pClass);
}

BOOL AgpmSummons::SetCallbackRemoveSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSUMMONS_CB_REMOVE_SUMMONS, pfCallback, pClass);
}

BOOL AgpmSummons::SetCallbackSetPropensity(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSUMMONS_CB_SET_PROPENSITY, pfCallback, pClass);
}

BOOL AgpmSummons::SetCallbackSetMaxSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSUMMONS_PACKET_SET_MAX_SUMMONS, pfCallback, pClass);
}

BOOL AgpmSummons::SetCallbackSetMaxTame(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSUMMONS_PACKET_SET_MAX_TAME, pfCallback, pClass);
}

BOOL AgpmSummons::SetCallbackRemoveSummonsByLevelLimitRegion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSUMMONS_CB_REMOVE_SUMMONS_BY_LEVEL_LIMIT_REGION, pfCallback, pClass);
}