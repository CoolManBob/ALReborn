/*===========================================================================

	AgsmWantedCriminal.cpp

===========================================================================*/


#include "AgsmWantedCriminal.h"
#include "ApMemoryTracker.h"
#include "ApmMap.h"


/************************************************************/
/*		The Implementation of AgsmWantedCriminal class		*/
/************************************************************/
//
AgsmWantedCriminal::AgsmWantedCriminal()
	{
	SetModuleName(_T("AgsmWantedCriminal"));
	SetModuleType(APMODULE_TYPE_SERVER);

	m_RankingLock.Init();
	m_bReady = FALSE;
	}


AgsmWantedCriminal::~AgsmWantedCriminal()
	{
	m_RankingLock.Destroy();
	}




//	ApModule inherited
//======================================
//
BOOL AgsmWantedCriminal::OnAddModule()
	{
	m_pAgsmServerManager	= (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmInterServerLink	= (AgsmInterServerLink *) GetModule(_T("AgsmInterServerLink"));
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgsmCharacter		= (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgpmPvP				= (AgpmPvP *) GetModule(_T("AgpmPvP"));
	m_pAgsmPvP				= (AgsmPvP *) GetModule(_T("AgsmPvP"));
	m_pAgsmDeath			= (AgsmDeath *) GetModule(_T("AgsmDeath"));
	m_pAgsmItem				= (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgpmWantedCriminal	= (AgpmWantedCriminal *) GetModule(_T("AgpmWantedCriminal"));
	
	m_pAgpmLog				= (AgpmLog *) GetModule(_T("AgpmLog"));
	m_pApmMap				= (ApmMap *) GetModule(_T("ApmMap"));

	if (!m_pAgsmServerManager || !m_pAgsmInterServerLink
		|| !m_pAgpmCharacter || !m_pAgsmCharacter || !m_pAgpmPvP || !m_pAgsmPvP || !m_pAgsmItem || !m_pAgpmWantedCriminal || !m_pApmMap)
		return FALSE;

	if(!m_pAgsmInterServerLink->SetCallbackConnect(CBServerConnect, this))
		return FALSE;

	if (!m_pAgsmCharacter->SetCallbackEnterGameworld(CBEnterGame, this))
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackRemoveChar(CBLeaveGame, this))
		return FALSE;
	
	if (!m_pAgpmCharacter->SetCallbackUpdateLevel(CBUpdateLevel, this))
		return FALSE;

	if (!m_pAgpmWantedCriminal->SetCallbackEventRequest(CBEventRequest, this))
		return FALSE;

	if (!m_pAgpmWantedCriminal->SetCallbackArrange(CBArrange, this))
		return FALSE;
	
	if (!m_pAgpmWantedCriminal->SetCallbackRequestList(CBRequestList, this))
		return FALSE;
	
	if (!m_pAgpmWantedCriminal->SetCallbackMyRank(CBRequestMyRank, this))
		return FALSE;
	
	if (!m_pAgsmPvP->SetCallbackPvPDead(CBPvPDead, this))
		return FALSE;
		
	if (!m_pAgsmPvP->SetCallbackCheckCriminal(CBCheckCriminal, this))
		return FALSE;
	
	// 부활(리버스 오브나 리저렉션 사용 포함)시 final killer reset.
	if (!m_pAgsmDeath->SetCallbackResurrection(CBResurrection, this))
		return FALSE;
	if (!m_pAgsmItem->SetCallbackUseItemReverseOrb(CBResurrection, this))
		return FALSE;
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::OnInit()
	{
	return TRUE;
	}




//	Operations
//==========================================
//
BOOL AgsmWantedCriminal::OnConnectRelayServer()
	{
	m_bReady = FALSE;
	
	m_RankingLock.Lock();
	
	m_pAgpmWantedCriminal->OnDestroy();
	m_pAgpmWantedCriminal->OnInit();
	
	m_Ranking.clear();
	
	EnumCallback(AGSMWANTEDCRIMINAL_CB_SELECT, NULL, NULL);
	
	m_RankingLock.Unlock();
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::OnSelectResultWantedCriminalList(stBuddyRowset *pRowset, BOOL bEnd)
	{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;
		
	if (bEnd)
		{
		m_bReady = TRUE;
		return TRUE;
		}
		
	m_RankingLock.Lock();

	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
		{
		// create mail
		AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->CreateWantedCriminal();
		if (NULL == pAgpdWantedCriminal)
			return FALSE;
		
		pAgpdWantedCriminal->m_bOnline = FALSE;
		
		UINT32 ulCol = 0;
		TCHAR *psz = NULL;

		psz = pRowset->Get(ul, ulCol++);		// char id
		if (NULL == psz)
			{
			m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
			continue;
			}
		_tcsncpy(pAgpdWantedCriminal->m_szCharID, psz, AGPDCHARACTER_NAME_LENGTH);

		psz = pRowset->Get(ul, ulCol++);		// money
		if (NULL == psz)
			{
			m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
			continue;
			}
		pAgpdWantedCriminal->m_llMoney = _ttoi64(psz);

		psz = pRowset->Get(ul, ulCol++);		// class
		if (NULL == psz)
			{
			m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
			continue;
			}
		pAgpdWantedCriminal->m_lClass = _ttoi(psz);

		psz = pRowset->Get(ul, ulCol++);		// level
		if (NULL == psz)
			{
			m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
			continue;
			}
		pAgpdWantedCriminal->m_lLevel = _ttoi(psz);
		
		if (!m_pAgpmWantedCriminal->AddWantedCriminal(pAgpdWantedCriminal))
			{
			m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
			return FALSE;
			}
		
		_InsertRank(pAgpdWantedCriminal);
		}
	
	m_RankingLock.Unlock();
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::OnArrange(AgpdCharacter *pAgpdCharacter, AgpdWantedCriminalParam *pParam)
	{
	if (FALSE == m_bReady)
		return FALSE;
	
	if (NULL == pAgpdCharacter || NULL == pParam || NULL == pParam->m_pAgpdWantedCriminal)
		return FALSE;

	// Check Region Ignore Criminal Rule
	if(m_pApmMap->CheckRegionPerculiarity(pAgpdCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_CRIMINAL_RULE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		return FALSE;
	
	// check killer
	TCHAR *pszKiller = pParam->m_pAgpdWantedCriminal->m_szCharID;
	if (!m_pAgpmWantedCriminal->IsFinalAttacker(pAgpdCharacter, pszKiller))
		{
		//SendArrangeResult(pAgpdCharacter, pParam->m_pAgpdWantedCriminal, AGPMWANTEDCRIMINAL_RESULT_SUCCESS); 
		return FALSE;
		}
	
	// check base bounty
	if (!m_pAgpmWantedCriminal->IsEnoughBaseBounty(pAgpdCharacter))
		{
		SendArrangeResult(pAgpdCharacter, pParam->m_pAgpdWantedCriminal, AGPMWANTEDCRIMINAL_RESULT_FAIL_NOT_ENOUGH_BASE_MONEY);
		return FALSE;
		}
	
	// check money
	INT64 llMoney = pParam->m_pAgpdWantedCriminal->m_llMoney;
	
	if (m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacter) > llMoney)
		{
		return FALSE;
		}
	
	if (!m_pAgpmWantedCriminal->IsEnoughBounty(pAgpdCharacter, llMoney))
		{
		SendArrangeResult(pAgpdCharacter, pParam->m_pAgpdWantedCriminal, AGPMWANTEDCRIMINAL_RESULT_FAIL_NOT_ENOUGH_MONEY);
		return FALSE;
		}

	// 
	BOOL bNew = FALSE;
	AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->GetWantedCriminal(pszKiller);
	if (NULL == pAgpdWantedCriminal)
		{
		pAgpdWantedCriminal = m_pAgpmWantedCriminal->CreateWantedCriminal();
		bNew = TRUE;
		if (NULL == pAgpdWantedCriminal)
			{
			return FALSE;
			}
		}
	
	pAgpdWantedCriminal->m_Mutex.Lock();
	m_RankingLock.Lock();

	if (bNew)
		{
		*pAgpdWantedCriminal = *(pParam->m_pAgpdWantedCriminal);
		if (!m_pAgpmWantedCriminal->AddWantedCriminal(pAgpdWantedCriminal))
			{
			m_RankingLock.Unlock();
			pAgpdWantedCriminal->m_Mutex.Unlock();
			m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
			return FALSE;
			}
		
		_InsertRank(pAgpdWantedCriminal);
		
		EnumCallback(AGSMWANTEDCRIMINAL_CB_INSERT, pAgpdWantedCriminal, NULL);
		}
	else
		{
		_RemoveRank(pAgpdWantedCriminal);
		pAgpdWantedCriminal->m_llMoney += llMoney;
		_InsertRank(pAgpdWantedCriminal);
		
		EnumCallback(AGSMWANTEDCRIMINAL_CB_UPDATE, pAgpdWantedCriminal, NULL);
		}

	AgpdCharacter *pAgpdCharacterKiller = m_pAgpmCharacter->GetCharacterLock(pszKiller);
	if (pAgpdCharacterKiller)
		{
		pAgpdCharacterKiller->m_bIsWantedCriminal = TRUE;
		
		pAgpdWantedCriminal->m_bOnline = TRUE;
		pAgpdWantedCriminal->m_lClass = pAgpdCharacterKiller->m_pcsCharacterTemplate->m_lID;
		pAgpdWantedCriminal->m_lLevel = m_pAgpmCharacter->GetLevel(pAgpdCharacterKiller);
		pAgpdCharacterKiller->m_Mutex.Release();
		
		SendUpdateFlag(pAgpdCharacterKiller);
		}
	else
		{
		if (bNew)
			EnumCallback(AGSMWANTEDCRIMINAL_CB_OFFLINE_CHAR_INFO, pAgpdWantedCriminal, NULL);
		}
	
	m_RankingLock.Unlock();
	pAgpdWantedCriminal->m_Mutex.Unlock();
	
	// sub money
	INT64 llInvenMoney = 0;
	m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llInvenMoney);
	INT64 llBankMoney = m_pAgpmCharacter->GetBankMoney(pAgpdCharacter);
	
	if (llInvenMoney >= llMoney)
		{
		m_pAgpmCharacter->SubMoney(pAgpdCharacter, llMoney);
		}
	else
		{
		INT64 llSub = llMoney - llInvenMoney;
		m_pAgpmCharacter->SubMoney(pAgpdCharacter, llInvenMoney);
		m_pAgpmCharacter->SubBankMoney(pAgpdCharacter, llSub);
		m_pAgsmCharacter->UpdateBankToDB(pAgpdCharacter);
		}
	
	m_pAgpmWantedCriminal->SetFinalAttacker(pAgpdCharacter, NULL);
	SendArrangeResult(pAgpdCharacter, pParam->m_pAgpdWantedCriminal, AGPMWANTEDCRIMINAL_RESULT_SUCCESS);
	WriteLog(AGPDLOGTYPE_ETC_WC_ARRANGE, pAgpdCharacter, pszKiller, llMoney);
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::OnPvPDead(AgpdCharacter *pAgpdCharacterKill, AgpdCharacter *pAgpdCharacterDead)
	{
	if (FALSE == m_bReady)
		return FALSE;
	
	if (NULL == pAgpdCharacterKill || NULL == pAgpdCharacterDead)
		return FALSE;

	if(m_pApmMap->CheckRegionPerculiarity(pAgpdCharacterDead->m_nBindingRegionIndex, APMMAP_PECULIARITY_CRIMINAL_RULE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		return FALSE;

	AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacterDead->m_szID);
	if (pAgpdWantedCriminal)	// dead character is wanted criminal
		{
		pAgpdWantedCriminal->m_Mutex.Lock();
		m_pAgpmWantedCriminal->RemoveWantedCriminal(pAgpdWantedCriminal);
		
		// apply benefit
		m_pAgpmCharacter->AddMoney(pAgpdCharacterKill, pAgpdWantedCriminal->m_llMoney);
		WriteLog(AGPDLOGTYPE_ETC_WC_KILL, pAgpdCharacterKill, pAgpdCharacterDead->m_szID, pAgpdWantedCriminal->m_llMoney);
		SendNotify(pAgpdCharacterKill, pAgpdCharacterDead->m_szID, pAgpdWantedCriminal->m_llMoney, AGPMWANTEDCRIMINAL_RESULT_KILL_WANTED_CRIMINAL);
		
		// apply penalty
		m_RankingLock.Lock();
		
		INT32 lRank = _GetRank(pAgpdWantedCriminal) - 1;
		if (lRank >= 0)
			{
			INT32 lGrade = (INT32) (lRank / 10);
			INT32 lRate = (10 - lGrade);

			if (0 > lRate)
				lRate = 0;

			INT64 llPenaltyExp = (INT64) ( (double)lRate * (double)m_pAgpmCharacter->GetLevelUpExp(pAgpdCharacterDead) / 100.0 );
			if (llPenaltyExp < 3)
				llPenaltyExp	= 3;
			m_pAgsmCharacter->SubExp(pAgpdCharacterDead, llPenaltyExp);
			pAgpdCharacterDead->m_bIsWantedCriminal = FALSE;

			WriteLog(AGPDLOGTYPE_ETC_WC_DEAD, pAgpdCharacterDead, pAgpdCharacterKill->m_szID, llPenaltyExp);
			SendNotify(pAgpdCharacterDead, pAgpdCharacterKill->m_szID, llPenaltyExp, AGPMWANTEDCRIMINAL_RESULT_DEAD_WANTED_CRIMINAL);

			SendUpdateFlag(pAgpdCharacterDead);

			_RemoveRank(pAgpdWantedCriminal);
			
			EnumCallback(AGSMWANTEDCRIMINAL_CB_DELETE, pAgpdWantedCriminal, NULL);
			}
		
		m_RankingLock.Unlock();
		pAgpdWantedCriminal->m_Mutex.Unlock();
		m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);
		}
	else
		{
		// dead character is normal(not assailant, not villain)
		if (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED != pAgpdCharacterDead->m_unCriminalStatus
			&& !m_pAgpmCharacter->IsMurderer(pAgpdCharacterDead)
			&& !m_pAgpmPvP->IsCombatPvPMode(pAgpdCharacterDead)
			&& !m_pAgpmPvP->IsCombatPvPMode(pAgpdCharacterKill)
			&& !m_pAgpmPvP->EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pAgpdCharacterKill, pAgpdCharacterDead)
			)
			{
			//if (AP_SERVICE_AREA_CHINA != g_eServiceArea)
				{
				m_pAgpmWantedCriminal->SetFinalAttacker(pAgpdCharacterDead, pAgpdCharacterKill->m_szID);
				SendUpdateKiller(pAgpdCharacterDead, pAgpdCharacterKill);
				}
			}
		}
		
	return TRUE;
	}


BOOL AgsmWantedCriminal::OnRequestList(AgpdCharacter *pAgpdCharacter, INT32 lPage)
	{
	if (FALSE == m_bReady)
		return FALSE;
	
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	if (0 > lPage || lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;
	
	AgpdWantedCriminal *pAgpdWantedCriminal = NULL;
	
	INT32 lIndex = 0;
	INT32 lDistance = lPage * 10;
	
	CriminalRankIter Iter = m_Ranking.begin();

	if (m_Ranking.size() > lDistance)
	{
		advance(Iter, lDistance);

		while ( Iter != m_Ranking.end() )
		{
			INT32 lRank = (lPage * 10) + lIndex + 1;
			pAgpdWantedCriminal = Iter->second;
			SendWantedCriminal(pAgpdCharacter, pAgpdWantedCriminal, lRank, lPage);
			lIndex++;
			
			if (lIndex > 10)
				break;

			++Iter;
		}
	}

	SendWantedCriminal(pAgpdCharacter, NULL, 0, AGPMWANTEDCRIMINAL_END_OF_LIST);
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::OnRequestMyRank(AgpdCharacter *pAgpdCharacter)
	{
	if (FALSE == m_bReady)
		return FALSE;
	
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacter);
	if (NULL == pAgpdWantedCriminal)
		return FALSE;
	
	INT32 lRank = _GetRank(pAgpdWantedCriminal);
	if (0 == lRank)
		return FALSE;
	
	return SendMyRank(pAgpdCharacter, lRank, pAgpdWantedCriminal->m_llMoney);
	}


BOOL AgsmWantedCriminal::OnSelectOfflineCharacter(TCHAR *pszChar, INT32 lTID, INT32 lLevel)
	{
	AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->GetWantedCriminal(pszChar);
	if (NULL == pAgpdWantedCriminal)
		return FALSE;
	
	pAgpdWantedCriminal->m_Mutex.Lock();
	pAgpdWantedCriminal->m_lClass = lTID;
	pAgpdWantedCriminal->m_lLevel = lLevel;
	pAgpdWantedCriminal->m_Mutex.Unlock();
	
	return TRUE;
	}

// 2006.12.05. steeple
// 이런 저런 효과 없이 그냥 지워준다. 주로 어드민에서 사용된다.
BOOL AgsmWantedCriminal::DeleteWCWithNoEffect(const CHAR* szName)
{
	if(!szName)
		return FALSE;

	AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->GetWantedCriminal((TCHAR*)szName);
	if(pAgpdWantedCriminal)	// dead character is wanted criminal
	{
		pAgpdWantedCriminal->m_Mutex.Lock();
		m_pAgpmWantedCriminal->RemoveWantedCriminal(pAgpdWantedCriminal);
		
		// apply penalty
		m_RankingLock.Lock();
		_RemoveRank(pAgpdWantedCriminal);
		EnumCallback(AGSMWANTEDCRIMINAL_CB_DELETE, pAgpdWantedCriminal, NULL);

		m_RankingLock.Unlock();
		pAgpdWantedCriminal->m_Mutex.Unlock();
		m_pAgpmWantedCriminal->DestroyWantedCriminal(pAgpdWantedCriminal);

		AgpdCharacter* pcsCharacter = m_pAgpmCharacter->GetCharacterLock(const_cast<CHAR*>(szName));
		if(pcsCharacter)
		{
			pcsCharacter->m_bIsWantedCriminal = FALSE;
			SendUpdateFlag(pcsCharacter);
			
			pcsCharacter->m_Mutex.Release();
		}

		return TRUE;
	}
	
	return FALSE;
}

// 2006.12.05. steeple
// _GetRank() 가 protected 여서 하나 만든다.
INT32 AgsmWantedCriminal::GetRank(const CHAR* szName)
{
	if(!szName)
		return FALSE;

	INT32 lRank = 0;
	AgpdWantedCriminal *pAgpdWantedCriminal = m_pAgpmWantedCriminal->GetWantedCriminal((TCHAR*)szName);
	if(pAgpdWantedCriminal)
	{
		pAgpdWantedCriminal->m_Mutex.Lock();

		lRank = _GetRank(pAgpdWantedCriminal);

		pAgpdWantedCriminal->m_Mutex.Unlock();
	}

	return lRank;
}





//	Callbacks
//======================================
//
BOOL AgsmWantedCriminal::CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return TRUE;

	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgsdServer *pAgsdServer = (AgsdServer *) pData;

	AgsdServer *pAgsdServerRelay = pThis->m_pAgsmServerManager->GetRelayServer();
	if (!pAgsdServerRelay)
		return FALSE;

	if (pAgsdServer == pAgsdServerRelay)
		pThis->OnConnectRelayServer();

	return TRUE;
	}


BOOL AgsmWantedCriminal::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *)pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	ApdEvent *pApdEvent = (ApdEvent *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmWantedCriminal->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->_GetCharacterNID(pAgpdCharacter));
	pThis->m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWantedCriminal::CBArrange(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdWantedCriminalParam *pParam = (AgpdWantedCriminalParam *) pCustData;
		
	return pThis->OnArrange(pAgpdCharacter, pParam);
	}


BOOL AgsmWantedCriminal::CBPvPDead(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacterKill = (AgpdCharacter *) pData;
	AgpdCharacter *pAgpdCharacterDead = (AgpdCharacter *) pCustData;

	return pThis->OnPvPDead(pAgpdCharacterKill, pAgpdCharacterDead);
	}


BOOL AgsmWantedCriminal::CBCheckCriminal(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	BOOL *pbCriminal = (BOOL *) pCustData;
	
	// if wanted criminal
	if (NULL != pThis->m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacter->m_szID))
		*pbCriminal = TRUE;

	return TRUE;
	}


BOOL AgsmWantedCriminal::CBRequestList(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	INT32 lPage = *((INT32 *) pCustData);
	
	return pThis->OnRequestList(pAgpdCharacter, lPage);
	}


BOOL AgsmWantedCriminal::CBRequestMyRank(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	return pThis->OnRequestMyRank(pAgpdCharacter);
	}


BOOL AgsmWantedCriminal::CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;
	
	AgpdWantedCriminal *pAgpdWantedCriminal = pThis->m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacter);
	if (NULL != pAgpdWantedCriminal)
		{
		pAgpdWantedCriminal->m_Mutex.Lock();
		pAgpdWantedCriminal->m_bOnline = TRUE;
		pAgpdWantedCriminal->m_lClass = pAgpdCharacter->m_pcsCharacterTemplate->m_lID;
		pAgpdWantedCriminal->m_lLevel = pThis->m_pAgpmCharacter->GetLevel(pAgpdCharacter);
		pAgpdWantedCriminal->m_Mutex.Unlock();
		
		pAgpdCharacter->m_bIsWantedCriminal = TRUE;
		
		pThis->SendUpdateFlag(pAgpdCharacter);
		}
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::CBLeaveGame(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	//STOPWATCH2(pThis->GetModuleName(), _T("CBLeaveGame"));

	AgpdWantedCriminal *pAgpdWantedCriminal = pThis->m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacter);
	if (NULL != pAgpdWantedCriminal)
		{
		pAgpdWantedCriminal->m_Mutex.Lock();
		pAgpdWantedCriminal->m_bOnline = FALSE;
		pAgpdWantedCriminal->m_Mutex.Unlock();
		}
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	INT32 lLevel = *(INT32 *) pCustData;
	
	AgpdWantedCriminal *pAgpdWantedCriminal = pThis->m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacter);
	if (NULL != pAgpdWantedCriminal)
		{
		pAgpdWantedCriminal->m_Mutex.Lock();
		pAgpdWantedCriminal->m_lLevel = lLevel;
		pAgpdWantedCriminal->m_Mutex.Unlock();
		}
	
	return TRUE;
	}


BOOL AgsmWantedCriminal::CBResurrection(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmWantedCriminal *pThis = (AgsmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	// if resurrected, remove final attacker
	pThis->m_pAgpmWantedCriminal->SetFinalAttacker(pAgpdCharacter, NULL);
	pThis->SendUpdateKiller(pAgpdCharacter, _T(""));
	
	return TRUE;
	}




//	Packet Send
//===================================================
//
BOOL AgsmWantedCriminal::SendArrangeResult(AgpdCharacter *pAgpdCharacter, AgpdWantedCriminal *pAgpdWantedCriminal, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_ARRANGE;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmWantedCriminal->MakePacketWC(pAgpdWantedCriminal, &nPacketLength);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
														&cOperation,					// op
														&pAgpdCharacter->m_lID,			// CID
														NULL,							// page
														NULL,							// rank
														&lResult,						// result
														pvPacketEmb
														);

	m_pAgpmWantedCriminal->m_csPacketWC.FreePacket(pvPacketEmb);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWantedCriminal::SendWantedCriminal(AgpdCharacter *pAgpdCharacter, AgpdWantedCriminal *pAgpdWantedCriminal, INT32 lRank, INT32 lPage)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_RES_LIST;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = NULL;
	
	if (pAgpdWantedCriminal)
		{
		pvPacketEmb = m_pAgpmWantedCriminal->MakePacketWC(pAgpdWantedCriminal, &nPacketLength);
		if (!pvPacketEmb || nPacketLength < 1)
			return FALSE;
		}
	
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
														&cOperation,					// op
														&pAgpdCharacter->m_lID,			// CID
														&lPage,							// page
														&lRank,							// rank
														NULL,							// result
														pvPacketEmb
														);

	m_pAgpmWantedCriminal->m_csPacketWC.FreePacket(pvPacketEmb);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWantedCriminal::SendMyRank(AgpdCharacter *pAgpdCharacter, INT32 lRank, INT64 llMoney)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_MY_RANK;
	INT16 nPacketLength = 0;
	
	PVOID pvPacketEmb = m_pAgpmWantedCriminal->m_csPacketWC.MakePacket(FALSE, &nPacketLength, 0,
																	pAgpdCharacter->m_szID,
																	&llMoney,
																	NULL,
																	NULL,
																	NULL
																	);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
														&cOperation,					// op
														&pAgpdCharacter->m_lID,			// CID
														NULL,							// page
														&lRank,							// rank
														NULL,							// result
														pvPacketEmb
														);

	m_pAgpmWantedCriminal->m_csPacketWC.FreePacket(pvPacketEmb);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWantedCriminal::SendUpdateKiller(AgpdCharacter *pAgpdCharacter, AgpdCharacter *pAgpdCharacterKiller)
	{
	if (!pAgpdCharacterKiller)
		return FALSE;
	
	return SendUpdateKiller(pAgpdCharacter, pAgpdCharacterKiller->m_szID);
	}


BOOL AgsmWantedCriminal::SendUpdateKiller(AgpdCharacter *pAgpdCharacter, TCHAR *pszKiller)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_UPDATE_KILLER;
	INT16 nPacketLength = 0;
	
	TCHAR	szKiller[AGPDCHARACTER_NAME_LENGTH + 1];
	ZeroMemory(szKiller, sizeof(szKiller));
	if (pszKiller)
		_tcscpy(szKiller, pszKiller);
	
	PVOID pvPacketEmb = m_pAgpmWantedCriminal->m_csPacketWC.MakePacket(FALSE, &nPacketLength, 0,
																	szKiller,
																	NULL,
																	NULL,
																	NULL,
																	NULL
																	);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
														&cOperation,					// op
														&pAgpdCharacter->m_lID,			// CID
														NULL,							// page
														NULL,							// rank
														NULL,							// result
														pvPacketEmb
														);

	m_pAgpmWantedCriminal->m_csPacketWC.FreePacket(pvPacketEmb);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWantedCriminal::SendUpdateFlag(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_UPDATE_FLAG;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
														&cOperation,							// op
														&pAgpdCharacter->m_lID,					// CID
														NULL,									// page
														NULL,									// rank
														&pAgpdCharacter->m_bIsWantedCriminal,	// result
														NULL
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWantedCriminal::SendNotify(AgpdCharacter *pAgpdCharacter, TCHAR *pszCharID, INT64 llValue, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_NOTIFY;
	INT16 nPacketLength = 0;
	
	PVOID pvPacketEmb = m_pAgpmWantedCriminal->m_csPacketWC.MakePacket(FALSE, &nPacketLength, 0,
																	pszCharID,
																	&llValue,
																	NULL,
																	NULL,
																	NULL
																	);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
														&cOperation,					// op
														&pAgpdCharacter->m_lID,			// CID
														NULL,							// page
														NULL,							// rank
														&lResult,						// result
														pvPacketEmb
														);

	m_pAgpmWantedCriminal->m_csPacketWC.FreePacket(pvPacketEmb);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}




//	Callback Setting
//===============================================
//
BOOL AgsmWantedCriminal::SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMWANTEDCRIMINAL_CB_SELECT, pfCallback, pClass);
	}


BOOL AgsmWantedCriminal::SetCallbackInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMWANTEDCRIMINAL_CB_INSERT, pfCallback, pClass);
	}


BOOL AgsmWantedCriminal::SetCallbackUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMWANTEDCRIMINAL_CB_UPDATE, pfCallback, pClass);
	}


BOOL AgsmWantedCriminal::SetCallbackDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMWANTEDCRIMINAL_CB_DELETE, pfCallback, pClass);
	}


BOOL AgsmWantedCriminal::SetCallbackOffCharInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMWANTEDCRIMINAL_CB_OFFLINE_CHAR_INFO, pfCallback, pClass);
	}




//	Rank
//===============================================
//
BOOL AgsmWantedCriminal::_InsertRank(AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	BOOL bResult = FALSE;
	
	if (pAgpdWantedCriminal)
		{
		CriminalRankIter Iter = m_Ranking.insert(CriminalRankPair(pAgpdWantedCriminal->m_llMoney, pAgpdWantedCriminal));
		bResult = (Iter != m_Ranking.end()) ? TRUE : FALSE;
		}

	return bResult;
	}


BOOL AgsmWantedCriminal::_RemoveRank(AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	if (pAgpdWantedCriminal)
		{
		for (CriminalRankIter Iter = m_Ranking.lower_bound(pAgpdWantedCriminal->m_llMoney);
				Iter != m_Ranking.upper_bound(pAgpdWantedCriminal->m_llMoney); Iter++)
			{
			if (NULL != Iter->second && pAgpdWantedCriminal == Iter->second)
				{
				m_Ranking.erase(Iter);
				return TRUE;
				}
			}
		}

	return FALSE;
	}


INT32 AgsmWantedCriminal::_GetRank(AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	INT32 lRank = 0;
	if (pAgpdWantedCriminal)
		{
		for (CriminalRankIter Iter = m_Ranking.lower_bound(pAgpdWantedCriminal->m_llMoney);
				Iter != m_Ranking.upper_bound(pAgpdWantedCriminal->m_llMoney); Iter++)
			{
			if (NULL != Iter->second && pAgpdWantedCriminal == Iter->second)
				{
				lRank = (INT32)distance(m_Ranking.begin(), Iter);		// 0-based
				lRank++;										// 1-based
				break;
				}
			}
		}
	
	return lRank;
	}


INT32 AgsmWantedCriminal::_GetRank(AgpdCharacter *pAgpdCharacter)
	{
	return _GetRank(m_pAgpmWantedCriminal->GetWantedCriminal(pAgpdCharacter));
	}




//	Helper
//===============================================
//
UINT32 AgsmWantedCriminal::_GetCharacterNID(INT32 lCID)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(lCID);
	}


UINT32 AgsmWantedCriminal::_GetCharacterNID(AgpdCharacter *pAgpdCharacter)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter);
	}




//	Log
//===================================================
//
BOOL AgsmWantedCriminal::WriteLog(eAGPDLOGTYPE_ETC eType, AgpdCharacter *pAgpdCharacter, TCHAR *pszCharID, INT64 llValue)
	{
	if (NULL == m_pAgpmLog || NULL == pAgpdCharacter)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	TCHAR szValue[20];
	ZeroMemory(szValue, sizeof(szValue));
	
	if (AGPDLOGTYPE_ETC_WC_KILL == eType ||
		AGPDLOGTYPE_ETC_WC_ARRANGE == eType)
		_stprintf(szValue, _T("Bounty = %I64d"), llValue);
	else if (AGPDLOGTYPE_ETC_WC_DEAD == eType)
		_stprintf(szValue, _T("Exp = %I64d"), llValue);

	return m_pAgpmLog->WriteLog_ETC(eType,
									0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
									m_pAgpmCharacter->GetLevel(pAgpdCharacter),
									m_pAgpmCharacter->GetExp(pAgpdCharacter),
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									0,
									pszCharID,
									szValue,
									0
									);
	}

