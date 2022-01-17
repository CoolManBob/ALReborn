// AgsmSummons.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2005. 08. 01.

#include "AgsmSummons.h"
#include "AgsmEventSpawn.h"

const double AGSMSUMMONS_RANDOM_POS_WIDTH = 400.0;
const UINT32 AGSMSUMMONS_REMOVE_SUMMONS_TERM = 2000;	// 2초

AgsmSummons::AgsmSummons()
{
	SetModuleName("AgsmSummons");
	SetModuleType(APMODULE_TYPE_SERVER);

	EnableIdle(FALSE);

	m_papmEventManager = NULL;
	m_pagpmCharacter = NULL;
	m_pagpmAI2 = NULL;
	m_pagpmEventSpawn = NULL;
	m_pagsmEventSpawn = NULL;
	m_pagpmSummons = NULL;
	m_pagpmArchlord = NULL;

	m_pagsmSystemMessage = NULL;
	m_pagsmCharacter = NULL;
	m_pagsmCharManager = NULL;
}

AgsmSummons::~AgsmSummons()
{
}

BOOL AgsmSummons::OnAddModule()
{
	m_papmEventManager = (ApmEventManager*)GetModule("ApmEventManager");

	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmAI2 = (AgpmAI2*)GetModule("AgpmAI2");
	m_pagpmEventSpawn = (AgpmEventSpawn*)GetModule("AgpmEventSpawn");
	m_pagpmSummons = (AgpmSummons*)GetModule("AgpmSummons");
	m_pagpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");

	m_pagsmSystemMessage = (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	m_pagsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");

	if(!m_papmEventManager || !m_pagpmCharacter || !m_pagpmEventSpawn || !m_pagpmSummons || !m_pagpmArchlord)
		return FALSE;

	if(!m_pagsmCharacter || !m_pagsmCharManager)
		return FALSE;

	// SetCallbackRemoveChar에서 SetCallbackRemoveRide로 수정
	// SetCallbackRemoveChar가 RemoveCharacter보다 일찍 호출된다.
	// 개념상으로는 PreRemoveCharacter와 동일
	if (!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackBindingRegionChange(CBUpdateRegionIndex, this))
		return FALSE;

	if(!m_pagpmSummons->SetCallbackSetPropensity(CBSetPropensity, this))
		return FALSE;
	if(!m_pagpmSummons->SetCallbackRemoveSummonsByLevelLimitRegion(CBRemoveSummonsByLevelLimitRegion, this))
		return FALSE;

	if(!m_pagpmArchlord->SetCallbackCancelArchlord(CBCancelArchlord, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmSummons::OnInit()
{
	m_pagsmEventSpawn = (AgsmEventSpawn*)GetModule("AgsmEventSpawn");

	return TRUE;
}

BOOL AgsmSummons::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmSummons::OnDestroy()
{
	return TRUE;
}

BOOL AgsmSummons::CBRemoveCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if (!pvData || !pvClass)
		return FALSE;

	AgsmSummons		*pThis			= (AgsmSummons *)	pvClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pvData;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (NULL == pcsAgsdCharacter)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	if (false == pcsAgsdCharacter->m_DetailInfo.bRemoveByDead &&
		false == pcsAgsdCharacter->m_DetailInfo.bRemoveByEndPeriod)
		pcsAgsdCharacter->m_DetailInfo.bRemoveByOwnerRemove = true;

	// Fixed 를 가장 먼저 검사한다!!!!!! 켈켈켈 2005.10.05. steeple
	if (pThis->m_pagpmCharacter->IsStatusFixed(pcsCharacter))
	{
		if(pcsAgsdCharacter->m_DetailInfo.bRemoveByOwnerRemove == false)
		{
			INT32 lOwnerCID = pThis->m_pagpmSummons->GetOwnerCID(pcsCharacter);
			if (0 >= lOwnerCID)
				return TRUE;			// 없어도 TRUE 로 리턴 하세. 2005.10.07. steeple

			AgpdCharacter *pcsOwner = pThis->m_pagpmCharacter->GetCharacterLock(lOwnerCID);
			if (NULL == pcsOwner)
				return TRUE;			// 없어도 TRUE 로 리턴 하세. 2005.10.07. steeple

			pThis->m_pagpmSummons->RemoveFixedInOwner(pcsOwner, pcsCharacter->m_lID);
			pThis->SendRemoveSummons(pcsOwner, pcsCharacter->m_lID);

			pThis->EnumCallback(AGSMSUMMONS_CB_END_FIXED, pcsCharacter, pcsOwner);
			pcsOwner->m_Mutex.Release();

			return TRUE;
		}
	}
	else if (pThis->m_pagpmCharacter->IsStatusTame(pcsCharacter))
	{
		if(pcsAgsdCharacter->m_DetailInfo.bRemoveByOwnerRemove == false)
		{
			// 소환수나 테이밍 몬스터가 외부적인 요인(텔레포트를 탄다든지)에 의해서 삭제되는 경우
			INT32 lOwnerCID = pThis->m_pagpmSummons->GetOwnerCID(pcsCharacter);
			if (0 > lOwnerCID)
				return TRUE;			// 없어도 TRUE 로 리턴 하세. 2005.10.07. steeple

			AgpdCharacter *pcsOwner = pThis->m_pagpmCharacter->GetCharacterLock(lOwnerCID);
			if (pcsOwner)
			{
				pThis->m_pagpmSummons->RemoveTameInOwner(pcsOwner, pcsCharacter->m_lID);
				pcsOwner->m_Mutex.Release();
			}

			pThis->EnumCallback(AGSMSUMMONS_CB_END_TAMING, pcsCharacter, pcsOwner);
			return TRUE;
		}
	}
	else if (pThis->m_pagpmCharacter->IsStatusSummoner(pcsCharacter))
	{
		if(pcsAgsdCharacter->m_DetailInfo.bRemoveByOwnerRemove == false)
		{
			INT32 lOwnerCID = pThis->m_pagpmSummons->GetOwnerCID(pcsCharacter);
			if (0 >= lOwnerCID)
				return TRUE;			// 없어도 TRUE 로 리턴 하세. 2005.10.07. steeple

			AgpdCharacter *pcsOwner = pThis->m_pagpmCharacter->GetCharacterLock(lOwnerCID);
			if (NULL == pcsOwner)
				return TRUE;			// 없어도 TRUE 로 리턴 하세. 2005.10.07. steeple

			pThis->m_pagpmSummons->RemoveSummonsInOwner(pcsOwner, pcsCharacter->m_lID);
			pThis->SendRemoveSummons(pcsOwner, pcsCharacter->m_lID);

			pThis->EnumCallback(AGSMSUMMONS_CB_END_SUMMONS, pcsCharacter, pcsOwner);
			pcsOwner->m_Mutex.Release();

			return TRUE;
		}
	}

//	if (FALSE == pThis->m_pagpmCharacter->IsPC(pcsCharacter))
//		return TRUE;

	pThis->RemoveAllSummons(pcsCharacter,
					AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);

	return TRUE;
}

// 2005.10.07. steeple
BOOL AgsmSummons::RemoveAllSummons(AgpdCharacter* pcsCharacter, INT32 lRemoveType)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

//	if (FALSE == m_pagpmCharacter->IsPC(pcsCharacter))
//		return TRUE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	if(lRemoveType & AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS)
	{
		while(pcsSummonsADChar->m_SummonsArray.m_pSummons->empty() == false)
		{
			AgpdSummonsArray::iterator iter = pcsSummonsADChar->m_SummonsArray.m_pSummons->begin();
			if(iter == pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
				break;

			AgpdSummons tempSummons = *iter;
			if (0 == tempSummons.m_lCID)
				break;

			// 2005.10.31. steeple
			// 부활할 때 이 루틴을 탄다. 그러므로 패킷 보내주어야 한다.
			SendRemoveSummons(pcsCharacter, tempSummons.m_lCID);

			//AgpdCharacter	*pcsSummon	= m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_SummonsArray.m_arrSummons[0].m_lCID);
			//if (pcsSummon && (m_pagpmCharacter->IsNPC(pcsSummon) || pcsSummon->m_bIsProtectedNPC))
			//	AuLogFile("RemoveNPC.log", "Removed by AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS\n");

			m_pagpmCharacter->RemoveCharacter(tempSummons.m_lCID);
		
			iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
							pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
							tempSummons.m_lCID);

			if (iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
				pcsSummonsADChar->m_SummonsArray.m_pSummons->erase(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin());
		}
	}

	if(lRemoveType & AGSMSUMMONS_CB_REMOVE_TYPE_TAME)
	{
		while(false == pcsSummonsADChar->m_TameArray.m_arrTame.empty())
		{
			// 2005.10.31. steeple
			// 부활할 때 이 루틴을 탄다. 그러므로 패킷 보내주어야 한다.
			SendRemoveSummons(pcsCharacter, pcsSummonsADChar->m_TameArray.m_arrTame[0].m_lCID);

			AgpdCharacter* pcsTame = m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_TameArray.m_arrTame[0].m_lCID);
			if(pcsTame)
			{
				ResetTame(pcsTame, pcsCharacter->m_lID);
				pcsTame->m_Mutex.Release();
			}

			pcsSummonsADChar->m_TameArray.m_arrTame.erase(pcsSummonsADChar->m_TameArray.m_arrTame.begin());
		}
	}

	if(lRemoveType & AGSMSUMMONS_CB_REMOVE_TYPE_FIXED)
	{
		while(false == pcsSummonsADChar->m_FixedArray.m_arrFixed.empty())
		{
			AgpdSummons tempFixed = pcsSummonsADChar->m_FixedArray.m_arrFixed[0];

			if (0 == tempFixed.m_lCID)
				break;

			// 2005.10.31. steeple
			// 부활할 때 이 루틴을 탄다. 그러므로 패킷 보내주어야 한다.
			SendRemoveSummons(pcsCharacter, tempFixed.m_lCID);

			//AgpdCharacter	*pcsSummon	= m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_FixedArray.m_arrFixed[0].m_lCID);
			//if (pcsSummon && (m_pagpmCharacter->IsNPC(pcsSummon) || pcsSummon->m_bIsProtectedNPC))
			//	AuLogFile("RemoveNPC.log", "Removed by AGSMSUMMONS_CB_REMOVE_TYPE_FIXED\n");

			m_pagpmCharacter->RemoveCharacter(pcsSummonsADChar->m_FixedArray.m_arrFixed[0].m_lCID);
		
			AgpdFixedArray::iterator iter = std::find(pcsSummonsADChar->m_FixedArray.m_arrFixed.begin(),
														pcsSummonsADChar->m_FixedArray.m_arrFixed.end(),
														tempFixed.m_lCID);

			if (iter != pcsSummonsADChar->m_FixedArray.m_arrFixed.end())
				pcsSummonsADChar->m_FixedArray.m_arrFixed.erase(pcsSummonsADChar->m_FixedArray.m_arrFixed.begin());
		}
	}

	return TRUE;
}

// 2007.03.30. steeple
// 해당 TID 의 소환수를 없앤다. 
BOOL AgsmSummons::RemoveSummonsByTID(AgpdCharacter* pcsCharacter, INT32 lTID)
{
	if(!pcsCharacter || !lTID)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	// 2007.04.02. steeple
	// 임시벡터를 만들어서 작업한다.
	std::vector<AgpdSummons> vcTmp;
	vcTmp.assign(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
				pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
	// 소환수
	std::vector<AgpdSummons>::iterator iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		if(iter->m_lTID == lTID)
		{
			// 2007.04.09. steeple
			// 서로 지워지는 콜백때문에 타이머 도입. 2초 후에 지운다.
			AddTimer(AGSMSUMMONS_REMOVE_SUMMONS_TERM, iter->m_lCID, this, RemoveSummonsAfterTerm, NULL);

			//m_pagpmSummons->RemoveSummonsInOwner(pcsCharacter, iter->m_lCID);
			//SendRemoveSummons(pcsCharacter, iter->m_lCID);
			//m_pagpmCharacter->RemoveCharacter(iter->m_lCID);
		}

		++iter;
	}
	vcTmp.clear();

	// Tame 은 하지 말자.
	//vcTmp.assign(pcsSummonsADChar->m_TameArray.m_arrTame.begin(),
	//			pcsSummonsADChar->m_TameArray.m_arrTame.end());
	//// Tame
	//iter = vcTmp.begin();
	//while(iter2 != vcTmp.end())
	//{
	//	if(iter2->m_lTID == lTID)
	//	{
	//		SendRemoveSummons(pcsCharacter, iter->m_lCID);

	//		AgpdCharacter* pcsTame = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
	//		if(pcsTame)
	//		{
	//			ResetTame(pcsTame, pcsCharacter->m_lID);
	//			pcsTame->m_Mutex.Release();
	//		}
	//	}

	//	++iter2;
	//}

	vcTmp.assign(pcsSummonsADChar->m_FixedArray.m_arrFixed.begin(),
				pcsSummonsADChar->m_FixedArray.m_arrFixed.end());
	// Fixed
	iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		if(iter->m_lTID == lTID)
		{
			// 2007.04.09. steeple
			// 서로 지워지는 콜백때문에 타이머 도입. 2초 후에 지운다.
			AddTimer(AGSMSUMMONS_REMOVE_SUMMONS_TERM, iter->m_lCID, this, RemoveSummonsAfterTerm, NULL);

			//m_pagpmSummons->RemoveFixedInOwner(pcsCharacter, iter->m_lCID);
			//SendRemoveSummons(pcsCharacter, iter->m_lCID);
			//m_pagpmCharacter->RemoveCharacter(iter->m_lCID);
		}

		++iter;
	}
	vcTmp.clear();

	return TRUE;
}

// 2005.10.12. steeple
// Tame 상태였던 놈을 리셋한다.
BOOL AgsmSummons::ResetTame(AgpdCharacter* pcsTame, INT32 lOwnerCID)
{
	if(!pcsTame)
		return FALSE;

	m_pagpmSummons->RemoveOwner(pcsTame, 0);
	m_pagpmCharacter->SetStatusTame(pcsTame, FALSE);

	// 테이밍 상태동안 생성된 어그로를 없애주기 위해서 히스토리 초기화를 한다.
	AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pcsTame);
	if (pcsHistory)
	{
		for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
		{
			if (pcsHistory->m_astEntry[i].m_lPartyID != AP_INVALID_PARTYID)
				continue;

			if (pcsHistory->m_astEntry[i].m_csSource.m_eType == APBASE_TYPE_CHARACTER)
			{
				AgpdCharacter	*pcsHistoryTarget	= m_pagpmCharacter->GetCharacterLock(pcsHistory->m_astEntry[i].m_csSource.m_lID);
				if (pcsHistoryTarget)
				{
					if (m_pagsmCharacter->RemoveHistory(pcsHistoryTarget, (ApBase *) pcsTame))
						--i;

					pcsHistoryTarget->m_Mutex.Release();
				}
			}
		}
	}

	m_pagsmCharacter->InitHistory(pcsTame);

	// End Taming Callback 불러준다.
	EnumCallback(AGSMSUMMONS_CB_END_TAMING, pcsTame, NULL);

	return TRUE;
}

// 2007.03.30. steeple
// 소환수들에게도 투명 걸거나 빼준다. Tame 이나 Fixed 는 안해도 될 거 같다.
BOOL AgsmSummons::UpdateTransparentToSummons(AgpdCharacter* pcsCharacter, BOOL bSet)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	// 소환수
	AgpdSummonsArray::iterator iter = pcsSummonsADChar->m_SummonsArray.m_pSummons->begin();
	while(iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
	{
		AgpdCharacter* pcsSummons = m_pagpmCharacter->GetCharacter(iter->m_lCID);
		if(!pcsSummons || pcsSummons->m_bIsReadyRemove || !pcsSummons->m_Mutex.WLock())
		{
			++iter;
			continue;
		}

		if(bSet)
		{
			UINT32 ulDuration = 7 * 24 * 60 * 60 * 1000;	// 임시로 일주일 준다.
			m_pagsmCharacter->SetSpecialStatusTime(pcsSummons, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, ulDuration);
		}
		else
			m_pagpmCharacter->UpdateUnsetSpecialStatus(pcsSummons, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);

		pcsSummons->m_Mutex.Release();

		++iter;
	}

	return TRUE;
}

INT32 AgsmSummons::ProcessSummons(AgpdCharacter* pcsOwner, INT32 lSummonsTID, UINT32 ulDuration, INT32 lCount, INT32 lSkillLevel,
								  EnumAgpdSummonsType eType, EnumAgpdSummonsPropensity ePropensity,
								  BOOL bForce, BOOL bSendPacket)
{
	AuPOS pPos;
	pPos.x = 0;
	pPos.y = 0;

	return ProcessSummons(pPos, pcsOwner, lSummonsTID, ulDuration, lCount, lSkillLevel, eType, ePropensity, bForce, bSendPacket);
}

INT32 AgsmSummons::ProcessSummons(AuPOS stSummonPos, AgpdCharacter* pcsOwner, INT32 lSummonsTID, UINT32 ulDuration, INT32 lCount, INT32 lSkillLevel,
								  EnumAgpdSummonsType eType, EnumAgpdSummonsPropensity ePropensity,
								  BOOL bForce, BOOL bSendPacket)
{
	if(!m_pagpmAI2 || !pcsOwner || lSummonsTID < 1 || lCount < 1)
		return 0;

	AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(lSummonsTID);
	if(!pcsTemplate)
		return 0;

	// It is impossible when the character is resurrecting. 2008.06.24. steeple
	// 포탈 등의 로딩창이 뜨는 이동 중에는 소환 못한다. 2008.04.08. steeple
	if(m_pagsmCharacter->IsNotLogoutStatus(pcsOwner) &&
		m_pagsmCharacter->IsResurrectingNow(pcsOwner) == FALSE)
		return 0;

	INT32 lIndex = 0;
	AgpdSpawn* pstSpawn = m_pagpmEventSpawn->GetSpawnByMonsterTID(pcsTemplate->m_lID, &lIndex);
	if(!pstSpawn)
		return 0;

	ApdEvent* pstEvent = NULL;
	if(pstSpawn->m_pstParent)
		pstEvent = pstSpawn->m_pstParent->m_pstChild;

	INT32 lAI2ID = pstSpawn->m_stSpawnConfig->m_lAITID;
	AgpdAI2Template* pcsAI2Template = m_pagpmAI2->m_aAI2Template.GetAITemplate(lAI2ID);
	if(!pcsAI2Template)
		return 0;

	INT32 lCreateCount = 0;
	for(INT32 i = 0; i < lCount; i++)
	{
		// 체크먼저 해보고
		if(m_pagpmSummons->IsSummonsEnable(pcsOwner, bForce) != AGPMSUMMONS_SUMMONS_CHECK_RESULT_SUCCESS)
			break;

		// 캐릭터를 만든다.
		AgpdCharacter* pcsSummons = m_pagsmCharManager->CreateCharacter(NULL, NULL, pcsTemplate->m_lID, 0, FALSE);
		if(!pcsSummons)
			continue;

		INT32 lSummonsCID = pcsSummons->m_lID;

		AgpdAI2ADChar* pcsAI2Summons = m_pagpmAI2->GetCharacterData(pcsSummons);
		if(!pcsAI2Summons)
		{
			m_pagpmCharacter->RemoveCharacter(lSummonsCID);
			continue;
		}

		AgpdAI2ADChar* pcsAI2Owner = m_pagpmAI2->GetCharacterData(pcsOwner);
		if(!pcsAI2Owner)
		{
			m_pagpmCharacter->RemoveCharacter(lSummonsCID);
			continue;
		}

		m_pagpmCharacter->SetStatusSummoner(pcsSummons, TRUE);

		// 2005.09.18. steeple
		// 아무래도 소환수 Lock 해야 겠지????
		if (pcsSummons->m_Mutex.WLock())
		{
			// 2005.09.06. steeple
			// NewSummons 체크는 맨 위에서 해뿐지자.
			// 소환 해줬으므로, Public 모듈에 추가시킨다.
			UINT32 ulStartTime = GetClockCount();
			UINT32 ulEndTime = ulStartTime + ulDuration;
			if(!ulDuration)
				ulEndTime = 0xFFFFFFFF;		// ulDuration 이 0 이면 무제한으로 준다.

			if(!m_pagpmSummons->NewSummons(pcsOwner, pcsSummons, lSummonsTID, eType, ePropensity, ulStartTime, ulEndTime, bForce))
			{
				pcsSummons->m_Mutex.Release();

				// 소환한 놈 없애버린다.
				m_pagpmCharacter->RemoveCharacter(lSummonsCID);
				continue;
			}

			pcsAI2Summons->m_pcsBossCharacter = pcsOwner;
			pcsAI2Owner->m_pcsFollower[0] = pcsSummons;

			// SkillLevel Setting
			pcsAI2Summons->m_lCastSkillLevel = lSkillLevel;

			// 2005.09.08. steeple
			// AI의 Attribute에 고정형 몬스터라고 설정된 내용을 적용
			if(pcsAI2Template && pcsAI2Template->m_bDontMove)
				pcsSummons->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;

			if(stSummonPos.x == 0 && stSummonPos.y == 0)
				pcsSummons->m_stPos = GetRandomPos(pcsOwner->m_stPos);
			else
				pcsSummons->m_stPos = stSummonPos;

			if(m_pagpmEventSpawn->GetCharacterData(pcsSummons))
				m_pagpmEventSpawn->GetCharacterData(pcsSummons)->m_pstEvent = pstEvent;

			m_pagpmCharacter->UpdateInit(pcsSummons);

			pcsSummons->m_ulCheckBlocking	= 0;

			if(!m_pagsmCharManager->EnterGameWorld(pcsSummons, FALSE))
			{
				m_pagpmSummons->RemoveSummonsInOwner(pcsOwner, lSummonsCID);

				pcsSummons->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(lSummonsCID);
				continue;
			}

			// 2008.04.08. steeple
			// UpdateRegion 임의로 불러줌.
			m_pagpmCharacter->UpdateRegion(pcsSummons);

			AgsdCharacter* pcsAgsdSummons = m_pagsmCharacter->GetADCharacter(pcsSummons);
			if(pcsAgsdSummons)
				pcsAgsdSummons->m_bDestroyWhenDie = TRUE;

			m_pagpmEventSpawn->EnumCallback(AGPMSPAWN_CB_SPAWN, pcsSummons, &lAI2ID);

			lCreateCount++;

			// 패킷 보내준다.
			if(bSendPacket)
			{
				SendNewSummons(pcsOwner, lSummonsCID, lSummonsTID, (INT8)eType, (INT8)ePropensity, ulStartTime, ulStartTime + ulDuration, bForce);
			}

			// 주인이 투명이면 투명 걸어준다. 2007.03.30. steeple
			if(m_pagpmCharacter->IsStatusTransparent(pcsOwner))
			{
				UINT32 ulTrasnparentDuration = 7 * 24 * 60 * 60 * 1000;		// 임시로 일주일 준다.
				m_pagsmCharacter->SetSpecialStatusTime(pcsSummons, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, ulTrasnparentDuration);
			}

			// 콜백 부른다. 2005.09.18. steeple
			EnumCallback(AGSMSUMMONS_CB_NEW_SUMMONS, pcsSummons, pcsOwner);

			pcsSummons->m_Mutex.Release();
		}
	}

	return lCreateCount;
}

INT32 AgsmSummons::ProcessTaming(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, UINT32 ulDuration, INT32 lSkillLevel, BOOL bForce)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsTarget);
	ASSERT(0 < ulDuration);

	if(!m_pagpmAI2 || !pcsCharacter || !pcsTarget || ulDuration == 0)
		return 0;

	// 이런 저런 검사를 한다.
	// 시스템 메시지 작업. 2005.10.04. steeple
	eAgpmSummonsTamableCheckResult eCheckResult = m_pagpmSummons->IsTameEnable(pcsCharacter, pcsTarget, TRUE, bForce);
	if(eCheckResult != AGPMSUMMONS_TAMABLE_CHECK_RESULT_SUCCESS)
	{
		SendTameCheckResult(m_pagsmCharacter->GetCharDPNID(pcsCharacter), eCheckResult);
		return 0;
	}

	// 자신이 소환한 모든 몬스터들과의 관계를 끊는다.
	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter(pcsTarget);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return 0;

	while(false == pcsSummonsADChar->m_SummonsArray.m_pSummons->empty())
	{
		AgpdSummonsArray::iterator iter = pcsSummonsADChar->m_SummonsArray.m_pSummons->begin();
		if(iter == pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
			break;

		AgpdSummons tempSummons = *iter;
		if (0 == tempSummons.m_lCID)
			break;

		AgpdCharacter	*pcsSummon	= m_pagpmCharacter->GetCharacterLock(tempSummons.m_lCID);
		if (pcsSummon)
		{
			m_pagpmCharacter->SetStatusSummoner(pcsSummon, FALSE);

			AgpdSummonsADChar* pcsSummonAttachData = m_pagpmSummons->GetADCharacter(pcsSummon);
			if(!pcsSummonAttachData)
				break;

			pcsSummonAttachData->m_lOwnerCID = AP_INVALID_CID;
			pcsSummonAttachData->m_lOwnerTID = AP_INVALID_CID;

			pcsSummon->m_Mutex.Release();
		}
	
		iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
						pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
						tempSummons.m_lCID);

		if (iter != pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
			pcsSummonsADChar->m_SummonsArray.m_pSummons->erase(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin());
	}

	m_pagpmCharacter->SetStatusTame(pcsTarget, TRUE);

	UINT32 ulStartTime = GetClockCount();

	if(!m_pagpmSummons->NewTaming(pcsCharacter, pcsTarget, pcsTarget->m_pcsCharacterTemplate->m_lID, AGPMSUMMONS_PROPENSITY_ATTACK, 
									ulStartTime, ulStartTime + ulDuration, bForce))
		return 0;

	// 2005.10.05. steeple 주석처리.
	//m_pagpmSummons->SetMaxTameCount(pcsCharacter, lSkillLevel);
	pcsTarget->m_ulCheckBlocking	= 0;

	//m_pagpmEventSpawn->RemoveMonsterFromSpawnList(pcsTarget);
	m_pagpmEventSpawn->AddRespawnTime(pcsTarget);

	// reset fisrt spawn position
	AgpdAI2ADChar	*pcsAttachData	= m_pagpmAI2->GetCharacterData(pcsTarget);
	if (pcsAttachData)
	{
		pcsAttachData->m_csFirstSpawnPos.x	= 0.0f;
		pcsAttachData->m_csFirstSpawnPos.y	= 0.0f;
		pcsAttachData->m_csFirstSpawnPos.z	= 0.0f;
	}

	// Target 을 일단 정지 시킨다. 2005.10.03. steeple
	m_pagpmCharacter->StopCharacter(pcsTarget, NULL);

	SendNewSummons(pcsCharacter, pcsTarget->m_lID, pcsTarget->m_pcsCharacterTemplate->m_lID, (INT8)AGPMSUMMONS_TYPE_TAME,
					(INT8)AGPMSUMMONS_PROPENSITY_ATTACK, ulStartTime, ulStartTime + ulDuration, bForce);

	// 콜백 부른다. 2005.09.18. steeple
	EnumCallback(AGSMSUMMONS_CB_NEW_TAMING, pcsTarget, pcsCharacter);

	return 1;
}

// 2005.10.05. steeple
BOOL AgsmSummons::ProcessFixed(AgpdCharacter* pcsOwner, INT32 lFixedTID, UINT32 ulDuration, INT32 lCount, INT32 lSkillLevel, BOOL bForce)
{
	if(!m_pagpmAI2 || !pcsOwner || lFixedTID < 1 || lCount < 1)
		return 0;

	AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(lFixedTID);
	if(!pcsTemplate)
		return 0;

	INT32 lIndex = 0;
	AgpdSpawn* pstSpawn = m_pagpmEventSpawn->GetSpawnByMonsterTID(pcsTemplate->m_lID, &lIndex);
	if(!pstSpawn)
		return 0;

	INT32 lAI2ID = pstSpawn->m_stSpawnConfig->m_lAITID;
	AgpdAI2Template* pcsAI2Template = m_pagpmAI2->m_aAI2Template.GetAITemplate(lAI2ID);
	if(!pcsAI2Template)
		return 0;

	INT32 lCreateCount = 0;
	for(INT32 i = 0; i < lCount; i++)
	{
		// 체크먼저 해보고
		if(m_pagpmSummons->IsFixedEnable(pcsOwner, bForce) != AGPMSUMMONS_FIXED_CHECK_RESULT_SUCCESS)
			break;

		// 캐릭터를 만든다.
		AgpdCharacter* pcsFixed = m_pagsmCharManager->CreateCharacter(NULL, NULL, pcsTemplate->m_lID, 0, FALSE);
		if(!pcsFixed)
			continue;

		INT32 lFixedCID = pcsFixed->m_lID;

		AgpdAI2ADChar* pcsAI2Fixed = m_pagpmAI2->GetCharacterData(pcsFixed);
		if(!pcsAI2Fixed)
		{
			m_pagpmCharacter->RemoveCharacter(lFixedCID);
			continue;
		}

		AgpdAI2ADChar* pcsAI2Owner = m_pagpmAI2->GetCharacterData(pcsOwner);
		if(!pcsAI2Owner)
		{
			m_pagpmCharacter->RemoveCharacter(lFixedCID);
			continue;
		}

		// 여기서 이미 고정형이 세팅된다.
		m_pagpmCharacter->SetStatusFixed(pcsFixed, TRUE);

		if (pcsFixed->m_Mutex.WLock())
		{
			// 2005.09.06. steeple
			// NewSummons 체크는 맨 위에서 해뿐지자.
			// 소환 해줬으므로, Public 모듈에 추가시킨다.
			UINT32 ulStartTime = GetClockCount();

			if(!m_pagpmSummons->NewFixed(pcsOwner, pcsFixed, lFixedTID, AGPMSUMMONS_PROPENSITY_ATTACK, ulStartTime, ulStartTime + ulDuration, bForce))
			{
				//if (m_pagpmCharacter->IsNPC(pcsFixed) || pcsFixed->m_bIsProtectedNPC)
				//	AuLogFile("RemoveNPC.log", "Removed by ProcessFixed(3)\n");

				pcsFixed->m_Mutex.Release();

				// 소환한 놈 없애버린다.
				m_pagpmCharacter->RemoveCharacter(lFixedCID);
				continue;
			}

			pcsAI2Fixed->m_pcsBossCharacter = pcsOwner;
			pcsAI2Owner->m_pcsFollower[0] = pcsFixed;

			// SkillLevel Setting
			pcsAI2Fixed->m_lCastSkillLevel = lSkillLevel;

			// 2005.09.08. steeple
			// AI의 Attribute에 고정형 몬스터라고 설정된 내용을 적용
			//if(pcsAI2Template && pcsAI2Template->m_bDontMove)
			//	pcsFixed->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;

			pcsFixed->m_stPos = pcsOwner->m_stPos;
			
			m_pagpmCharacter->UpdateInit(pcsFixed);

			pcsFixed->m_ulCheckBlocking	= 0;

			if(!m_pagsmCharManager->EnterGameWorld(pcsFixed, FALSE))
			{
				m_pagpmSummons->RemoveSummonsInOwner(pcsOwner, lFixedCID);

				pcsFixed->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(lFixedCID);
				continue;
			}

			AgsdCharacter* pcsAgsdFixed = m_pagsmCharacter->GetADCharacter(pcsFixed);
			if(pcsAgsdFixed)
				pcsAgsdFixed->m_bDestroyWhenDie = TRUE;

			m_pagpmEventSpawn->EnumCallback(AGPMSPAWN_CB_SPAWN, pcsFixed, &lAI2ID);

			lCreateCount++;

			// 패킷 보내준다.
			SendNewSummons(pcsOwner, lFixedCID, lFixedTID, (INT8)AGPMSUMMONS_TYPE_FIXED,
						(INT8)AGPMSUMMONS_PROPENSITY_ATTACK, ulStartTime, ulStartTime + ulDuration, bForce);

			EnumCallback(AGSMSUMMONS_CB_NEW_FIXED, pcsFixed, pcsOwner);

			pcsFixed->m_Mutex.Release();
		}
	}

	return lCreateCount;
}

AuPOS AgsmSummons::GetRandomPos(const AuPOS& stPosBase, double dWidth)
{
	if(dWidth == 0.0)
		dWidth = AGSMSUMMONS_RANDOM_POS_WIDTH;

	double dRandX = m_csRandom.rand(dWidth * 2);
	dRandX -= AGSMSUMMONS_RANDOM_POS_WIDTH;

	double dRandZ = m_csRandom.rand(dWidth * 2);
	dRandZ -= AGSMSUMMONS_RANDOM_POS_WIDTH;

	AuPOS stPos = stPosBase;
	stPos.x += (FLOAT)dRandX;
	stPos.z += (FLOAT)dRandZ;
	
	return stPos;
}

AuPOS AgsmSummons::GetFrontofCharacterPos(AgpdCharacter* pcsTarget, double dWidth)
{
	AuPOS stSummonPosition = static_cast<AgpdCharacter*>(pcsTarget)->m_stPos;
	AuPOS stDirection	   = static_cast<AgpdCharacter*>(pcsTarget)->m_stDirection;

	stDirection.x = stDirection.x * dWidth;
	stDirection.y = stDirection.y * dWidth;
	stDirection.z = stDirection.z * dWidth;

	stSummonPosition.x	   += stDirection.x;
	stSummonPosition.y	   += stDirection.y;
	stSummonPosition.z     += stDirection.z;

	return stSummonPosition;
}

BOOL AgsmSummons::CheckSummonsPeriod(AgpdCharacter* pcsCharacter, UINT32 ulClockCount)
{
	if(!pcsCharacter)
		return FALSE;

	if(!m_pagpmCharacter->IsStatusSummoner(pcsCharacter) && !m_pagpmCharacter->IsStatusTame(pcsCharacter))
		return TRUE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter(pcsCharacter);
	if(!pcsSummonsADChar)
		return FALSE;

	// 한번 더 유효성 체크를 한다.
	if(pcsSummonsADChar->m_lOwnerCID == 0 ||
		pcsSummonsADChar->m_stSummonsData.m_eType == AGPMSUMMONS_TYPE_NONE ||
		pcsSummonsADChar->m_stSummonsData.m_ulStartTime == 0 ||
		pcsSummonsADChar->m_stSummonsData.m_ulEndTime == 0)
		return TRUE;

	// 시간이 넘어갔3
	if(ulClockCount > pcsSummonsADChar->m_stSummonsData.m_ulEndTime)
	{
		//AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacter(pcsSummonsADChar->m_lOwnerCID);
		EndSummonsPeriod(pcsCharacter);
	}

	return TRUE;
}

// 2005.10.12. steeple
// 여기 앞에 오는 소환수의 포인터는 락이 안되어 있다.
// 그런데, 락하면 안된다.
// 절대 락같은 짓은 하지 말자.
// 이유는 묻지 말 것.
// 
// 지속 시간이 지나간 소환수들을 여기서 없애준다.
BOOL AgsmSummons::EndSummonsPeriod(AgpdCharacter* pcsSummons)
{
	if(!pcsSummons)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter(pcsSummons);
	if(!pcsSummonsADChar)
		return FALSE;

	EnumAgpdSummonsType eSummonsType = pcsSummonsADChar->m_stSummonsData.m_eType;
	if(eSummonsType == AGPMSUMMONS_TYPE_TAME)
	{
		AgpdCharacter *pcsOwner = m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_lOwnerCID);
		if (pcsOwner)
		{
			// Owner, Summons 양 쪽에서 서로를 빼준다.
			// 이거 하면 Summons 쪽 시간 데이터는 초기화 된다.
			m_pagpmSummons->RemoveSummons(pcsOwner, pcsSummons->m_lID);

			// 패킷 보내준다.
			SendRemoveSummons(pcsOwner, pcsSummons->m_lID);
			pcsOwner->m_Mutex.Release();
		}

		ResetTame(pcsSummons, 0);
	}
	else
		EnumCallback(AGSMSUMMONS_CB_END_PERIOD_START, pcsSummons, NULL);

	return TRUE;

	//AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsSummons);
	//if(!pcsAgsdCharacter)
	//	return FALSE;

	//// 이 플래그가 TRUE 이면 이 작업을 한번이상 한 것이다.
	//if(pcsAgsdCharacter->m_bRemoveByPeriod == TRUE)
	//	return FALSE;

	//pcsAgsdCharacter->m_bRemoveByPeriod = TRUE;		// 이 루틴이 여러번 타는 것을 방지.
	//pcsAgsdCharacter->m_DetailInfo.bRemoveByEndPeriod = true;

	//EnumAgpdSummonsType eSummonsType = pcsSummonsADChar->m_stSummonsData.m_eType;

	//if(!pcsOwner && pcsOwner->m_Mutex.WLock())
	//{
	//	// Owner, Summons 양 쪽에서 서로를 빼준다.
	//	// 이거 하면 Summons 쪽 시간 데이터는 초기화 된다.
	//	m_pagpmSummons->RemoveSummons(pcsOwner, pcsSummons->m_lID);

	//	// 패킷 보내준다.
	//	SendRemoveSummons(pcsOwner, pcsSummons->m_lID);

	//	// Summons 랑, Fixed 쪽으로는 콜백을 여기서 불러준다.
	//	if(eSummonsType == AGPMSUMMONS_TYPE_ELEMENTAL)
	//		EnumCallback(AGSMSUMMONS_CB_END_SUMMONS, pcsSummons, pcsOwner);
	//	else if(eSummonsType == AGPMSUMMONS_TYPE_FIXED)
	//		EnumCallback(AGSMSUMMONS_CB_END_FIXED, pcsSummons, pcsOwner);

	//	pcsOwner->m_Mutex.Release();
	//}

	//// 타입에 따라서 다르게 처리한다.
	//switch(eSummonsType)
	//{
	//	case AGPMSUMMONS_TYPE_ELEMENTAL:
	//		// Remove 해버리자. 후후훗.
	//		m_pagpmCharacter->RemoveCharacter(pcsSummons, FALSE, FALSE);
	//		break;

	//	case AGPMSUMMONS_TYPE_TAME:
	//		ResetTame(pcsSummons, 0);
	//		break;

	//	case AGPMSUMMONS_TYPE_FIXED:
	//		// 얘는 걍 Remove
	//		m_pagpmCharacter->RemoveCharacter(pcsSummons, FALSE, FALSE);
	//		break;
	//}

	//return TRUE;
}

BOOL AgsmSummons::SendNewSummons(AgpdCharacter* pcsOwner, INT32 lSummonsCID, INT32 lSummonsTID, INT8 cSummonsType,
								 INT8 cSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce)
{
	if(!pcsOwner || lSummonsCID < 1)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmSummons->MakeNewSummonsPacket(&nPacketLength, pcsOwner->m_lID, lSummonsCID, lSummonsTID, cSummonsType,
										cSummonsPropensity, ulStartTime, ulEndTime, bForce);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsOwner));
	m_pagpmSummons->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmSummons::SendRemoveSummons(AgpdCharacter* pcsOwner, INT32 lSummonsCID)
{
	if(!pcsOwner || lSummonsCID < 1)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmSummons->MakeRemoveSummonsPacket(&nPacketLength, pcsOwner->m_lID, lSummonsCID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsOwner));
	m_pagpmSummons->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.09.26. steeple
// 둘다 Lock 되어 있는 상태임.
BOOL AgsmSummons::SendSetPropensity(AgpdCharacter* pcsOwner, AgpdCharacter* pcsSummons)
{
	if(!pcsOwner || !pcsSummons)
		return FALSE;

	// Summons 의 Propensity 를 Owner 에게 보내면 된다.
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmSummons->MakeSetPropensityPacket(&nPacketLength, pcsOwner->m_lID, pcsSummons->m_lID, (INT8)m_pagpmSummons->GetPropensity(pcsSummons));
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsOwner));
	m_pagpmSummons->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.10.07. steeple
BOOL AgsmSummons::SendSetMaxSummons(AgpdCharacter* pcsCharacter, INT32 lMaxCount)
{
	if(!pcsCharacter)
		return FALSE;

	// Max Summons Count 가 변할 때 보내지게 됨.
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmSummons->MakeSetMaxSummonsPacket(&nPacketLength, pcsCharacter->m_lID, (INT8)lMaxCount);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmSummons->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.10.07. steeple
BOOL AgsmSummons::SendSetMaxTame(AgpdCharacter* pcsCharacter, INT32 lMaxCount)
{
	if(!pcsCharacter)
		return FALSE;

	// Max Summons Count 가 변할 때 보내지게 됨.
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmSummons->MakeSetMaxTamePacket(&nPacketLength, pcsCharacter->m_lID, (INT8)lMaxCount);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmSummons->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.10.07. steeple
BOOL AgsmSummons::SendSummonsCheckResult(UINT32 ulNID, eAgpmSummonsSummonsCheckResult eCheckResult)
{
	if(!m_pagsmSystemMessage)
		return FALSE;

	INT32 lCode = -1;
	switch(eCheckResult)
	{
		case AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_MAX_COUNT:
			lCode = AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_MAX_COUNT;
			break;

		case AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_LEVEL_LIMIT_REGION:
			lCode = AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_LEVEL_LIMIT_REGION;
			break;
	}

	if(lCode != -1)
	{
		m_pagsmSystemMessage->SendSystemMessage(ulNID, lCode);
	}

	return TRUE;
}

// 2005.10.04. steeple
BOOL AgsmSummons::SendTameCheckResult(UINT32 ulNID, eAgpmSummonsTamableCheckResult eCheckResult)
{
	if(!m_pagsmSystemMessage)
		return FALSE;

	INT32 lCode = -1;
	switch(eCheckResult)
	{
		case AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MAX_COUNT:
			lCode = AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_MAX_COUNT;
			break;
		case AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_INVALID_TARGET:
			lCode = AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_INVALID_TARGET;
			break;
		case AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_ALREADY_TAMING:
			lCode = AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_ALREADY_TAMING;
			break;
		case AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MISS:
			lCode = AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_MISS;
			break;
	}

	if(lCode != -1)
	{
		m_pagsmSystemMessage->SendSystemMessage(ulNID, lCode);
	}

	return TRUE;
}

// 2005.10.05. steeple
BOOL AgsmSummons::SendFixedCheckResult(UINT32 ulNID, eAgpmSummonsFixedCheckResult eCheckResult)
{
	if(!m_pagsmSystemMessage)
		return FALSE;

	INT32 lCode = -1;
	switch(eCheckResult)
	{
		case AGPMSUMMONS_FIXED_CHECK_RESULT_FAILURE_MAX_COUNT:
			lCode = AGPMSYSTEMMESSAGE_CODE_FIXED_FAILURE_MAX_COUNT;
			break;
	}

	if(lCode != -1)
	{
		m_pagsmSystemMessage->SendSystemMessage(ulNID, lCode);
	}

	return TRUE;
}

BOOL AgsmSummons::SetCallbackNewSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_NEW_SUMMONS, pfCallback, pClass);
}

BOOL AgsmSummons::SetCallbackNewTaming(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_NEW_TAMING, pfCallback, pClass);
}

BOOL AgsmSummons::SetCallbackNewFixed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_NEW_FIXED, pfCallback, pClass);
}

BOOL AgsmSummons::SetCallbackEndSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_END_SUMMONS, pfCallback, pClass);
}

BOOL AgsmSummons::SetCallbackEndTaming(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_END_TAMING, pfCallback, pClass);
}

BOOL AgsmSummons::SetCallbackEndFixed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_END_FIXED, pfCallback, pClass);
}

BOOL AgsmSummons::SetCallbackEndPeriodStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSUMMONS_CB_END_PERIOD_START, pfCallback, pClass);
}

// 2005.09.26. steeple
BOOL AgsmSummons::CBSetPropensity(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsOwner = static_cast<AgpdCharacter*>(pData);
	AgsmSummons* pThis = static_cast<AgsmSummons*>(pClass);
	AgpdCharacter* pcsSummons = static_cast<AgpdCharacter*>(pCustData);

	return pThis->SendSetPropensity(pcsOwner, pcsSummons);
}

// 2006.11.23. steeple
// 아크로드가 끝나면 아크로드가 소환했던 소환수들 다 없애준다.
BOOL AgsmSummons::CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmSummons* pThis = static_cast<AgsmSummons*>(pClass);
	CHAR* szID = static_cast<CHAR*>(pData);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacterLock(szID);
	if(!pcsCharacter)
		return FALSE;

	pThis->RemoveAllSummons(pcsCharacter,
					AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2008.04.08. steeple
// 주인이 레벨 제한 지역에 들어갔을 때, 소환수의 레벨이 레벨 제한을 초과하면 지워야 한다.
BOOL AgsmSummons::CBRemoveSummonsByLevelLimitRegion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmSummons* pThis = static_cast<AgsmSummons*>(pClass);
	UINT32 ulCID = *static_cast<UINT32*>(pData);

	pThis->AddTimer(AGSMSUMMONS_REMOVE_SUMMONS_TERM, ulCID, pThis, RemoveSummonsAfterTerm, NULL);
	return TRUE;
}

// 2008.04.08. steeple
BOOL AgsmSummons::CBUpdateRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSummons* pThis = static_cast<AgsmSummons*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	INT16 nPrevRegionIndex = *static_cast<INT16*>(pCustData);

	if(pThis->m_pagpmCharacter->IsStatusSummoner(pcsCharacter) == FALSE &&
		pThis->m_pagpmCharacter->IsStatusTame(pcsCharacter) == FALSE &&
		pThis->m_pagpmCharacter->IsStatusFixed(pcsCharacter) == FALSE)
		return TRUE;

	// 소환수인데, 이놈이 들어가지 말아야 할 곳에 들어갔다면 죽여버린다. -ㅂ-;
	INT32 lLimitedLevel = pThis->m_pagpmCharacter->GetCurrentRegionLevelLimit(pcsCharacter);
	INT32 lSummonLevel = pThis->m_pagpmCharacter->GetLevel(pcsCharacter);

	if(lLimitedLevel != 0 && lLimitedLevel < lSummonLevel)
		pThis->AddTimer(AGSMSUMMONS_REMOVE_SUMMONS_TERM, pcsCharacter->m_lID, pThis, RemoveSummonsAfterTerm, NULL);

	return TRUE;
}

// 2007.04.09. steeple
// 스킬이 끝난 후에 불러준다.
// 서로 지워지는 콜백 때문에 타이머로 분리했다. (일명 무적버그 -_-)
BOOL AgsmSummons::RemoveSummonsAfterTerm(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if(!lCID || !pClass)
		return FALSE;

	AgsmSummons* pThis = static_cast<AgsmSummons*>(pClass);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(lCID);
	if(pcsCharacter)
	{
		AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(pcsAgsdCharacter)
			pcsAgsdCharacter->m_DetailInfo.bRemoveByEndPeriod = true;
	}

	// 그냥 지우면 된다
	pThis->m_pagpmCharacter->RemoveCharacter(lCID);

	return TRUE;
}
