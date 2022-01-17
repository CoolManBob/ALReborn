/*===========================================================================

	AgsmGamble.cpp

===========================================================================*/


#include "AgsmGamble.h"
#include "ApMemoryTracker.h"
#include "AuTimeStamp.h"


/****************************************************/
/*		The Implementation of AgsmGamble class		*/
/****************************************************/
//
AgsmGamble::AgsmGamble()
	{
	SetModuleName(_T("AgsmGamble"));
	SetModuleType(APMODULE_TYPE_SERVER);
	
	m_pAgpmLog = NULL;
	}


AgsmGamble::~AgsmGamble()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgsmGamble::OnAddModule()
	{
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem				= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmDropItem2		= (AgpmDropItem2 *) GetModule("AgpmDropItem2");
	m_pAgpmGamble			= (AgpmGamble *) GetModule("AgpmGamble");
	
	m_pAgsmCharacter		= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pAgsmItemManager		= (AgsmItemManager *) GetModule("AgsmItemManager");

	m_pAgpmLog				= (AgpmLog *) GetModule("AgpmLog");

	if (!m_pAgpmCharacter || !m_pAgpmItem || !m_pAgpmGamble || !m_pAgpmDropItem2
		|| !m_pAgsmCharacter || !m_pAgsmItemManager)
		return FALSE;

	if (!m_pAgpmGamble->SetCallbackEventRequest(CBEventRequest, this))
		return FALSE;
	if (!m_pAgpmGamble->SetCallbackGamble(CBGamble, this))
		return FALSE;
	
	return TRUE;
	}


BOOL AgsmGamble::OnInit()
	{
	return TRUE;
	}




//	Operations
//==========================================
//
BOOL AgsmGamble::OnGamble(AgpdCharacter *pAgpdCharacter, INT32 lGambleTID)
	{
	if (NULL == pAgpdCharacter || 0 == lGambleTID)
		return FALSE;

	AgpdGamble *pAgpdGamble = m_pAgpmGamble->GetTemplate(lGambleTID);
	if (NULL == pAgpdGamble)
		return FALSE;

	INT32 lResult = AGPMGAMBLE_RESULT_NONE;

	// validate
	if (FALSE == m_pAgpmGamble->IsValid(pAgpdCharacter, pAgpdGamble, &lResult))
		{
		return SendResultGamble(pAgpdCharacter, 0, lResult);
		}

	// evaluate item
	AgpdGambleItemList ItemList;
	if (0 >= m_pAgpmGamble->GetGambleItemList(pAgpdGamble, pAgpdCharacter, &ItemList))
		{
		return SendResultGamble(pAgpdCharacter, 0, AGPMGAMBLE_RESULT_NOT_ENOUGH_LEVEL);
		}
	
	// 적어도 하나이상의 아이템을 가진 랭크로 확률계산을 한다.
	INT32 lRandMax = 0;
	for (INT32 lRank = 0; lRank < AGPDGAMBLE_MAX_RANK; lRank++)
		{
		if (ItemList.Count(lRank) > 0)
			lRandMax += pAgpdGamble->m_alRank[lRank];
		}		// GetGambleItemList가 0보다 크므로 lRandMax가 0보다 크다는 것이 보장된다.
	
	INT32 lRankProb = m_Random.randInt(lRandMax - 1);
	INT32 lRankProbSum = 0;
	INT32 lFinalRank = -1;
	for (INT32 lRank = 0; lRank < AGPDGAMBLE_MAX_RANK; lRank++)
		{
		if (ItemList.Count(lRank) > 0)
			{
			lRankProbSum += pAgpdGamble->m_alRank[lRank];
			if (lRankProbSum >= lRankProb)
				{
				lFinalRank = lRank;
				break;
				}
			}
		}
	
	if (-1 == lFinalRank)
		return FALSE;
	
	INT32 lItemProb = m_Random.randInt(ItemList.Count(lFinalRank) - 1);
	AgpdItemTemplate *pAgpdItemTemplate = ItemList.m_apItemTemplateByRank[lFinalRank][lItemProb];
	if (!pAgpdItemTemplate)
		return FALSE;

	// create item	
	INT32 lTID = pAgpdItemTemplate->m_lID;
	INT32 lCount = 1;
	AgpdItem *pAgpdItem = m_pAgsmItemManager->CreateItem(lTID, NULL, lCount);
	if (!pAgpdItem)
		return FALSE;

	// add option, ...
	m_pAgpmDropItem2->ProcessConvertItem(pAgpdItem, pAgpdCharacter);
	
	// add to inven.
	if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE))
		{
		m_pAgpmItem->RemoveItem(pAgpdItem);
		return FALSE;
		}
	
	m_pAgpmCharacter->SubCharismaPoint(pAgpdCharacter, m_pAgpmGamble->Cost(pAgpdCharacter, pAgpdGamble));
	
	SendResultGamble(pAgpdCharacter, lTID, AGPMGAMBLE_RESULT_SUCCESS);

	return TRUE;
	}






//	Callbacks
//======================================
//
BOOL AgsmGamble::CBGamble(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGamble *pThis = (AgsmGamble *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	INT32 lGambleTID = *((INT32 *) pCustData);

	return pThis->OnGamble(pAgpdCharacter, lGambleTID);
	}


BOOL AgsmGamble::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGamble *pThis = (AgsmGamble *)pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	ApdEvent *pApdEvent = (ApdEvent *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmGamble->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->_GetCharacterNID(pAgpdCharacter));
	pThis->m_pAgpmGamble->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}




//	Packet Send
//===================================================
//
BOOL AgsmGamble::SendResultGamble(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8	cOperation = AGPMGAMBLE_OPERATION_GAMBLE;
	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pAgpmGamble->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGAMBLE_PACKET_TYPE,
															&cOperation,				// op
															&pAgpdCharacter->m_lID,		// CID
															&lItemTID,					// Gamble TID(Result Item TID)
															&lResult,					// Result
															NULL						// embedded packet
															);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmGamble->m_csPacket.FreePacket(pvPacket);

	return bResult;	
	}




//	Helper
//===============================================
//
UINT32 AgsmGamble::_GetCharacterNID(INT32 lCID)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(lCID);
	}


UINT32 AgsmGamble::_GetCharacterNID(AgpdCharacter *pAgpdCharacter)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter);
	}




//	Log
//===================================================
//
BOOL AgsmGamble::WriteGambleLog(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lCharge)
	{
	if (NULL == m_pAgpmLog || NULL == pAgpdCharacter || NULL == pAgpdItem)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	return TRUE;

	//return m_pAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ITEM_GAMBLE,
	//								0,
	//								&pAgsdCharacter->m_strIPAddress[0],
	//								pAgsdCharacter->m_szAccountID,
	//								pAgsdCharacter->m_szServerName,
	//								pAgpdCharacter->m_szID,
	//								pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
	//								m_pAgpmCharacter->GetLevel(pAgpdCharacter),
	//								m_pAgpmCharacter->GetExp(pAgpdCharacter),
	//								pAgpdCharacter->m_llMoney,
	//								pAgpdCharacter->m_llBankMoney,
	//								// result item ...
	//								// charge ...
	//								);
	}