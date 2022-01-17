// AgsmSearch.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.27.

#include "AgsmSearch.h"

AgsmSearch::AgsmSearch()
{
	SetModuleName("AgsmSearch");
	SetModuleType(APMODULE_TYPE_SERVER);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	m_pcsAgpmCharacter = NULL;
	m_pcsAgpmSearch = NULL;
}

AgsmSearch::~AgsmSearch()
{
}

BOOL AgsmSearch::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmSearch = (AgpmSearch*)GetModule("AgpmSearch");

	if(!m_pcsAgpmCharacter || !m_pcsAgpmSearch)
		return FALSE;

	if(!m_pcsAgpmSearch->SetCallbackSearchName(CBSearchName, this))
		return FALSE;

	if(!m_pcsAgpmSearch->SetCallbackSearchTID(CBSearchTID, this))
		return FALSE;

	if(!m_pcsAgpmSearch->SetCallbackSearchLevel(CBSearchLevel, this))
		return FALSE;

	if(!m_pcsAgpmSearch->SetCallbackSearchTIDLevel(CBSearchTIDLevel, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmSearch::SetCharacterData(AgpdSearch& stSearch, AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	memset(&stSearch, 0, sizeof(stSearch));

	if(m_pcsAgpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
		_tcsncpy(stSearch.m_szName, pcsCharacter->m_szID, AGPDCHARACTER_NAME_LENGTH);
	else
		_tcsncpy(stSearch.m_szName, pcsCharacter->m_pcsCharacterTemplate->m_szTName, AGPDCHARACTER_NAME_LENGTH);
	stSearch.m_lTID = pcsCharacter->m_pcsCharacterTemplate->m_lID;
	stSearch.m_lMinLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	stSearch.m_lMaxLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	stSearch.m_stPos = pcsCharacter->m_stPos;

	return TRUE;
}

BOOL AgsmSearch::SendResult(AgpdSearch &stSearch, UINT32 ulNID, INT8 cOperation)
{
	if(!ulNID)
		return 0;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmSearch->MakeSearchPacket(
										&nPacketLength,
										cOperation,
										stSearch);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmSearch->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmSearch::CBSearchName(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSearch* pstSearch = static_cast<AgpdSearch*>(pData);
	AgsmSearch* pThis = static_cast<AgsmSearch*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSearchName"));

	// 이름 검색
	AgpdCharacter* pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pstSearch->m_szName);
	if(pcsCharacter)
	{
		AgpdSearch stSearch;
		pThis->SetCharacterData(stSearch, pcsCharacter);
		pThis->SendResult(stSearch, ulNID);
	}
	else
	{
		pThis->SendResult(*pstSearch, ulNID, AGPMSEARCH_PACKET_FAIL);
	}
	
	return TRUE;
}

BOOL AgsmSearch::CBSearchTID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSearch* pstSearch = static_cast<AgpdSearch*>(pData);
	AgsmSearch* pThis = static_cast<AgsmSearch*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	if(pstSearch->m_lTID == 0)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSearchTID"));

	// PC TID 에서 함 찾아본다.
	if(find(g_alPCTID, g_alPCTID + AGPMCHARACTER_PC_TID_COUNT, pstSearch->m_lTID) == g_alPCTID + AGPMCHARACTER_PC_TID_COUNT)
		return FALSE;

	// 결과 보낼놈.
	AgpdSearch stSearch;

	// 걍 캐릭터 전체를 시퀀스 돌면서 검사한다.
	// 성능상 문제가 생기면 바꾸자.
	INT32 lIndex = 0, lCount = 0;
	for(AgpdCharacter* pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		pcsCharacter && lCount < AGPMSEARCH_MAX_COUNT;
		pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		if(pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) == FALSE)
			continue;

		if(_tcslen(pcsCharacter->m_szID) == 0)
			continue;

		if(pcsCharacter->m_pcsCharacterTemplate->m_lID == pstSearch->m_lTID)
		{
			pThis->SetCharacterData(stSearch, pcsCharacter);
			pThis->SendResult(stSearch, ulNID);
			++lCount;
		}
	}

	if(lCount == 0)
		pThis->SendResult(*pstSearch, ulNID, AGPMSEARCH_PACKET_FAIL);

	return TRUE;
}

//#define SEARCH_TEST		1
BOOL AgsmSearch::CBSearchLevel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSearch* pstSearch = static_cast<AgpdSearch*>(pData);
	AgsmSearch* pThis = static_cast<AgsmSearch*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	// 레벨 값이 정상인지 확인하고
	if(pstSearch->m_lMinLevel < 1 || pstSearch->m_lMaxLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;
	if(pstSearch->m_lMaxLevel < 1 || pstSearch->m_lMaxLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSearchLevel"));

	// 결과 보낼놈.
	AgpdSearch stSearch;

	// 걍 캐릭터 전체를 시퀀스 돌면서 검사한다.
	// 성능상 문제가 생기면 바꾸자.
	INT32 lIndex = 0, lCount = 0, lLevel = 0;
	for(AgpdCharacter* pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		pcsCharacter && lCount < AGPMSEARCH_MAX_COUNT;
		pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
#ifndef SEARCH_TEST
		if(pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) == FALSE)
			continue;

		if(_tcslen(pcsCharacter->m_szID) == 0)
			continue;
#endif

		lLevel = pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter);
		if(lLevel >= pstSearch->m_lMinLevel && lLevel <= pstSearch->m_lMaxLevel)
		{
			pThis->SetCharacterData(stSearch, pcsCharacter);
			pThis->SendResult(stSearch, ulNID);
			++lCount;
		}
	}

	if(lCount == 0)
		pThis->SendResult(*pstSearch, ulNID, AGPMSEARCH_PACKET_FAIL);

	return TRUE;
}

// TID 와 Level 검사를 한방에
BOOL AgsmSearch::CBSearchTIDLevel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSearch* pstSearch = static_cast<AgpdSearch*>(pData);
	AgsmSearch* pThis = static_cast<AgsmSearch*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	if(pstSearch->m_lTID == 0)
		return FALSE;

	// PC TID 에서 함 찾아본다.
	if(find(g_alPCTID, g_alPCTID + AGPMCHARACTER_PC_TID_COUNT, pstSearch->m_lTID) == g_alPCTID + AGPMCHARACTER_PC_TID_COUNT)
		return FALSE;

	// 레벨 값이 정상인지 확인하고
	if(pstSearch->m_lMinLevel < 1 || pstSearch->m_lMaxLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;
	if(pstSearch->m_lMaxLevel < 1 || pstSearch->m_lMaxLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSearchTIDLevel"));

	// 결과 보낼놈.
	AgpdSearch stSearch;

	// 걍 캐릭터 전체를 시퀀스 돌면서 검사한다.
	// 성능상 문제가 생기면 바꾸자.
	INT32 lIndex = 0, lCount = 0, lLevel = 0;
	for(AgpdCharacter* pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		pcsCharacter && lCount < AGPMSEARCH_MAX_COUNT;
		pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		if(pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) == FALSE)
			continue;

		if(pcsCharacter->m_pcsCharacterTemplate->m_lID != pstSearch->m_lTID)
			continue;

		if(_tcslen(pcsCharacter->m_szID) == 0)
			continue;

		lLevel = pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter);
		if(lLevel >= pstSearch->m_lMinLevel && lLevel <= pstSearch->m_lMaxLevel)
		{
			pThis->SetCharacterData(stSearch, pcsCharacter);
			pThis->SendResult(stSearch, ulNID);
			++lCount;
		}
	}

	if(lCount == 0)
		pThis->SendResult(*pstSearch, ulNID, AGPMSEARCH_PACKET_FAIL);

	return TRUE;
}