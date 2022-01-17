/*====================================================================

	AgsmRefinery.cpp

====================================================================*/

#include "AgsmRefinery.h"


/********************************************************/
/*		The Implementation of AgsmRefinery class		*/
/********************************************************/
//
AgsmRefinery::AgsmRefinery()
	{
	SetModuleName("AgsmRefinery");
	SetModuleType(APMODULE_TYPE_SERVER);
	
	EnableIdle(FALSE);

	m_pAgpmRefinery		= NULL;
	m_pAgpmDropItem2	= NULL;
	m_pAgsmItem			= NULL;
	m_pAgsmItemManager	= NULL;
	m_pAgsmCharacter	= NULL;
	m_pAgpmFactors		= NULL;
	m_pAgpmItemConvert	= NULL;
	}


AgsmRefinery::~AgsmRefinery()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgsmRefinery::OnAddModule()
	{
	m_pAgpmRefinery		= (AgpmRefinery *) GetModule("AgpmRefinery");
	m_pAgpmDropItem2	= (AgpmDropItem2 *) GetModule("AgpmDropItem2");
	m_pAgsmItem			= (AgsmItem *) GetModule("AgsmItem");
	m_pAgsmItemManager	= (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmItemConvert	= (AgpmItemConvert *) GetModule("AgpmItemConvert");

	if (!m_pAgpmRefinery || !m_pAgsmItem || !m_pAgsmItemManager || !m_pAgsmCharacter ||
	    !m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmItemConvert	|| !m_pAgpmDropItem2)
		return FALSE;

	//if (!m_pAgpmRefinery->SetCallbackRefine(CBRefine, this))
	//	return FALSE;

	if (!m_pAgpmRefinery->SetCallbackRefineItem(CBRefineItem, this))
		return FALSE;

	if (!m_pAgpmRefinery->SetCallbackBadboy(CBBadboy, this))
		return FALSE;

	return TRUE;
	}


BOOL AgsmRefinery::OnInit()
	{
	return TRUE;
	}


BOOL AgsmRefinery::OnIdle(UINT32 ulClockCount)
	{
	return TRUE;
	}


BOOL AgsmRefinery::OnDestroy()
	{
	return TRUE;
	}




//	Map
//===================================
//
AgsdRefineOptionGroup* AgsmRefinery::GetOptionGroup(INT32 lID)
	{
	OptionGroupIter Iter = m_MapOptionGroup.find(lID);
	if (Iter == m_MapOptionGroup.end())
		return NULL;
	
	return &(Iter->second);
	}


AgsdRefineSocket* AgsmRefinery::GetSocket(INT32 lID)
	{
	SocketIter Iter = m_MapSocket.find(lID);
	if (Iter == m_MapSocket.end())
		return NULL;
	
	return &(Iter->second);
	}




//	Stream
//===================================
//
//BOOL AgsmRefinery::StreamReadOptionGroup(CHAR *pszFile, BOOL bDecryption)
//	{
//	if (!pszFile || !strlen(pszFile))
//		return FALSE;
//
//	AuExcelTxtLib	csExcelTxtLib;
//
//	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
//		return FALSE;
//	
//	CHAR *psz = NULL;
//	INT32 lGroupID = 0;
//	INT32 lOptionTID = 0;
//	INT32 lRate = 0;
//
//	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)	
//		{
//		psz = csExcelTxtLib.GetData(AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_GROUP_ID, lRow);
//		if (NULL == psz)
//			{
//			TRACE("!!! Error : Option Group ID is NULL in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		lGroupID = atoi(psz);
//		if (0 == lGroupID)
//			{
//			TRACE("!!! Error : Option Group ID is 0 in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//
//		psz = csExcelTxtLib.GetData(AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_OPTION_TID, lRow);
//		if (NULL == psz)
//			{
//			TRACE("!!! Error : Option TID is NULL in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		lOptionTID = atoi(psz);
//		if (0 == lOptionTID)
//			{
//			TRACE("!!! Error : Option TID is 0 in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//				
//		psz = csExcelTxtLib.GetData(AGSMREFINERY_REFINE_ITEM_OPTION_GROUP_EXCEL_COLUMN_RATE, lRow);
//		if (NULL == psz)
//			{
//			TRACE("!!! Error : Rate is NULL in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		lRate = atoi(psz);
//		if (0 == lRate)
//			{
//			TRACE("!!! Error : Rate is 0 in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}		
//
//		// find
//		AgsdRefineOptionGroup *pAgsdRefineOptionGroup = GetOptionGroup(lGroupID);
//		if (NULL == pAgsdRefineOptionGroup)			// if not found, insert new
//			{
//			AgsdRefineOptionGroup OptionGroup;
//			OptionGroup.m_lID = lGroupID;
//			OptionGroup.m_lOptions[0] = lOptionTID;
//			OptionGroup.m_lRates[0] = lRate;
//			OptionGroup.m_lCount = 1;
//			
//			m_MapOptionGroup.insert(OptionGroupPair(lGroupID, OptionGroup));
//			}
//		else
//			{
//			pAgsdRefineOptionGroup->m_lOptions[pAgsdRefineOptionGroup->m_lCount] = lOptionTID;
//			// accmulate probability
//			pAgsdRefineOptionGroup->m_lRates[pAgsdRefineOptionGroup->m_lCount] = lRate + pAgsdRefineOptionGroup->m_lRates[pAgsdRefineOptionGroup->m_lCount-1];
//			pAgsdRefineOptionGroup->m_lCount++;
//			}
//		}
//		
//	// validation
//	for (OptionGroupIter Iter = m_MapOptionGroup.begin(); Iter != m_MapOptionGroup.end(); Iter++)
//		{
//		AgsdRefineOptionGroup *pAgsdRefineOptionGroup = &(Iter->second);
//		INT32 lRateSum = pAgsdRefineOptionGroup->m_lRates[pAgsdRefineOptionGroup->m_lCount-1];
//		ASSERT(10000 == lRateSum);
//		}
//	
//	return TRUE;	
//  }


//BOOL AgsmRefinery::StreamReadSocketInchant(CHAR *pszFile, BOOL bDecryption)
//	{
//	if (!pszFile || !strlen(pszFile))
//		return FALSE;
//
//	AuExcelTxtLib	csExcelTxtLib;
//
//	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
//		return FALSE;
//	
//	CHAR *psz = NULL;
//	INT32 lID = 0;
//	INT32 lMax = 0;
//	INT32 lRate = 0;
//
//	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)	
//		{
//		AgsdRefineSocket SocketInchant;
//		// id
//		psz = csExcelTxtLib.GetData(AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN_ID, lRow);
//		if (NULL == psz)
//			{
//			TRACE("!!! Error : Scoket Inchant ID is NULL in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		lID = atoi(psz);
//		if (0 == lID)
//			{
//			TRACE("!!! Error : Socket Inchant ID is 0 in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		
//		// max socket
//		psz = csExcelTxtLib.GetData(AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN_MAX_SOCKET, lRow);
//		if (NULL == psz)
//			{
//			TRACE("!!! Error : Max Scoket is NULL in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		lMax = atoi(psz);
//		if (0 == lMax)
//			{
//			TRACE("!!! Error : Max Socket is 0 in AgsmRefinery::StreamReadOptionGroup()\n");
//			continue;
//			}
//		
//		SocketInchant.m_lID = lID;
//		//SocketInchant.m_lCount = lMax;
//		
//		// rate
//		for (INT32 i = 0; i<lMax; ++i)
//			{
//			psz = csExcelTxtLib.GetData(AGSMREFINERY_REFINE_ITEM_SOCKET_EXCEL_COLUMN_RATE1 + i, lRow);
//			if (NULL == psz)
//				{
//				TRACE("!!! Error : Max Scoket is NULL in AgsmRefinery::StreamReadOptionGroup()\n");
//				continue;
//				}
//			lRate = atoi(psz);
//			if (0 == lMax)
//				{
//				TRACE("!!! Error : Max Socket is 0 in AgsmRefinery::StreamReadOptionGroup()\n");
//				continue;
//				}
//			
//			if (0 == i)
//				{
//				SocketInchant.m_Rates[SocketInchant.m_lCount] = lRate;
//				SocketInchant.m_lCount++;
//				}
//			else
//				{
//				SocketInchant.m_Rates[SocketInchant.m_lCount] = lRate + SocketInchant.m_Rates[SocketInchant.m_lCount-1];
//				SocketInchant.m_lCount++;
//				}
//			}
//		
//		// validation
//		INT32 lRateSum = SocketInchant.m_Rates[SocketInchant.m_lCount-1];
//		ASSERT(10000 == lRateSum);
//		
//		m_MapSocket.insert(SocketPair(lID, SocketInchant));
//		}
//		
//	return TRUE;			
//	}




//	Callbacks
//============================================
//
//BOOL AgsmRefinery::CBRefine(PVOID pData, PVOID pClass, PVOID pCustData)
//	{
//	AgsmRefinery *pThis = (AgsmRefinery *) pClass;
//	PVOID		*ppvBuffer = (PVOID *) pData;
//
//	return pThis->OnRefine((AgpdCharacter *) pCustData, (AgpdRefineTemplate *) ppvBuffer[0], (INT32 *) ppvBuffer[1]);
//	}

BOOL AgsmRefinery::CBRefineItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRefinery *pThis = (AgsmRefinery *) pClass;

	return pThis->OnRefineItem((AgpdCharacter *) pCustData, (INT32 *) pData);
}

BOOL AgsmRefinery::CBBadboy(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pClass || NULL == pData)
		return FALSE;
		
	AgsmRefinery *pThis = (AgsmRefinery *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdItem *pAgpdItem = (AgpdItem *) pCustData;
	
	AgsdCharacter *pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;
	
	AgsdItem *pAgsdItem = NULL;
	if (pAgpdItem)
		pAgsdItem = pThis->m_pAgsmItem->GetADItem(pAgpdItem);
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%04d/%02d/%02d %02d:%02d:%02d] Account[%s] Char[%s] IP[%s] ITEMSEQ[%I64d]\n", 
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
				pAgsdCharacter->m_szAccountID,
				pAgpdCharacter->m_szID,
				&pAgsdCharacter->m_strIPAddress[0],
				pAgsdItem ? pAgsdItem->m_ullDBIID : 0);
	AuLogFile_s("LOG\\AgsmRefinery_BadBoy.txt", strCharBuff);
	
	return TRUE;
	}




//	Operation (after callback)
//====================================================
//
//BOOL AgsmRefinery::OnRefine(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, INT32 lSourceItems[])
//	{
//	if (!pAgpdCharacter || !pAgpdRefineTemplate)
//		return FALSE;
//
//	// validate
//	INT32 eResult = AGPMREFINERY_RESULT_SUCCESS;
//	if (!m_pAgpmRefinery->IsValidStatus(pAgpdCharacter, pAgpdRefineTemplate, lSourceItems, &eResult))
//		{
//		return SendPakcetRefineResult(pAgpdCharacter->m_lID, pAgpdRefineTemplate->m_lItemTID, eResult, 0);
//		}
//
//	// delete source items
//	if (!DeleteSourceItem(pAgpdCharacter, pAgpdRefineTemplate, lSourceItems))
//		{
//		eResult = AGPMREFINERY_RESULT_FAIL;
//		return SendPakcetRefineResult(pAgpdCharacter->m_lID, pAgpdRefineTemplate->m_lItemTID, eResult, 0);		
//		}
//
//	// success probability
//	INT32 lSuccessProb = pAgpdRefineTemplate->m_lSuccessProb;
//
//	// evaluate
//	if (lSuccessProb < m_csRandom.randInt(100))
//		{
//		eResult = AGPMREFINERY_RESULT_FAIL;
//		return SendPakcetRefineResult(pAgpdCharacter->m_lID, pAgpdRefineTemplate->m_lItemTID, eResult, 0);				
//		}
//
//	// add item to inventory
//	AgpdItemADChar* pAgpdItemADChar = m_pAgpmRefinery->m_pAgpmItem->GetADCharacter(pAgpdCharacter);
//	if (!pAgpdItemADChar)
//		{
//		eResult = AGPMREFINERY_RESULT_FAIL;
//		return SendPakcetRefineResult(pAgpdCharacter->m_lID, pAgpdRefineTemplate->m_lItemTID, eResult, 0);				
//		}
//
//	if (m_pAgpmRefinery->m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
//		{
//		eResult = AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY;
//		return SendPakcetRefineResult(pAgpdCharacter->m_lID, pAgpdRefineTemplate->m_lItemTID, eResult, 0);				
//		}
//
//	AgpdItem* pAgpdItem = m_pAgsmItemManager->CreateItem(pAgpdRefineTemplate->m_lResultItemTID,
//														 pAgpdCharacter,
//														 1
//														 );
//
//	// 귀속 아이템 설정.	
//	if (E_AGPMITEM_BIND_ON_ACQUIRE == m_pAgpmRefinery->m_pAgpmItem->GetBoundType(pAgpdItem))
//		m_pAgpmRefinery->m_pAgpmItem->SetBoundOnOwner(pAgpdItem, pAgpdCharacter);
//	
//	m_pAgpmRefinery->m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem);	
//
//	// log.
//	m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_REFINE_REWARD,
//							pAgpdCharacter->m_lID,
//							pAgpdItem,
//							1
//							);
//
//	// send result.
//	BOOL bResult = SendPakcetRefineResult(pAgpdCharacter->m_lID,
//										  pAgpdRefineTemplate->m_lItemTID,
//										  eResult,
//										  pAgpdRefineTemplate->m_lResultItemTID
//										  );
//	
//	return bResult;
//	}

BOOL AgsmRefinery::OnRefineItem(AgpdCharacter *pAgpdCharacter, INT32* lSourceItems)
{
	if (NULL == pAgpdCharacter)
		return FALSE;

	// validate
	INT32 eResult = AGPMREFINERY_RESULT_FAIL;
	BOOL bSources[AGPMREFINERY_MAX_GRID] = {FALSE, };

	if (!m_pAgpmRefinery->IsValidStatusRefine(pAgpdCharacter, lSourceItems, &eResult, bSources)) //Call AgpmRefinery::IsValidStatus() at Ln450
		return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);

	// check inventory
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmRefinery->m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
	{
		eResult = AGPMREFINERY_RESULT_FAIL;
		return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);				
	}

	if (m_pAgpmRefinery->m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
	{
		eResult = AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY;
		return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);				
	}
	
    AgpdItem *pSourceItems[AGPMREFINERY_MAX_GRID];
	INT32 lSourceItemsCount[AGPMREFINERY_MAX_GRID];
	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
	{
		if (lSourceItems[i])
		{
			pSourceItems[i] = m_pAgpmRefinery->m_pAgpmItem->GetItem(lSourceItems[i]);
			if(pSourceItems[i] && pSourceItems[i]->m_pcsItemTemplate)
			{
				if(pSourceItems[i]->m_pcsItemTemplate->m_bStackable)
					lSourceItemsCount[i] = pSourceItems[i]->m_nCount;
				else
					lSourceItemsCount[i] = 1;
			}
			else
				pSourceItems[i] = NULL;
		}
		else
			pSourceItems[i] = NULL;
	}

    AgpdRefineItem2* pResultItems = m_pAgpmRefinery->GetRefineResultItems(pAgpdCharacter, pSourceItems);
	if(NULL == pResultItems)
	{
		eResult = AGPMREFINERY_RESULT_FAIL;
		return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);				
	}

	int index = rand()% (int)pResultItems->m_RefineMakeItemVec.size();

	if(pResultItems)
	{
		AgpdRefineMakeItem* makeitem = &(pResultItems->m_RefineMakeItemVec.at(index));

		// Sub pay
		if(pResultItems->m_lPrice > 0)
		{
			if( !m_pAgpmCharacter->SubMoney(pAgpdCharacter, pResultItems->m_lPrice) )
			{
				eResult = AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_MONEY;
				return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);
			}
			else
				m_pAgsmCharacter->WriteGheldLog(AGPDLOGTYPE_GHELD_REFINE_PAY, pAgpdCharacter, pResultItems->m_lPrice);
		}

		// create item
		AgpdItem* pAgpdItem = NULL;
		if(makeitem)
			 pAgpdItem = m_pAgsmItemManager->CreateItem(makeitem->m_lMakeItemTID, NULL,makeitem->m_IMakeItemCount);
		
		if(NULL == pAgpdItem) 
			return FALSE;
		// apply option
		ApplyOption(makeitem, pAgpdItem, pAgpdCharacter);
		
		// apply socket
		ApplySocket(makeitem, pAgpdItem);

		// apply stamina
		ApplyStamina(makeitem, pAgpdItem, pSourceItems);
		
		// add to inventory
		m_pAgpmRefinery->m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem);	
		
        // log.
		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_REFINE_REWARD, pAgpdCharacter->m_lID, pAgpdItem, 1);

        // bonus item create
        AgpdItem* pAgpdBonusItem = NULL;
        if(makeitem)
            pAgpdBonusItem = m_pAgsmItemManager->CreateItem(makeitem->m_IBonusItemTID, NULL, 1);

        if(pAgpdBonusItem) 
		{
			// apply option
			ApplyOption(makeitem, pAgpdBonusItem, pAgpdCharacter);

			// apply socket
			ApplySocket(makeitem, pAgpdBonusItem);

			// add to inventory
			m_pAgpmRefinery->m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdBonusItem);	

			// log.
			m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_REFINE_REWARD, pAgpdCharacter->m_lID, pAgpdBonusItem, 1);
		}

		// delete source items
		if (!DeleteSourceItem(pAgpdCharacter, lSourceItems, lSourceItemsCount))
		{
			m_pAgpmRefinery->m_pAgpmItem->RemoveItem(pAgpdItem);
            m_pAgpmRefinery->m_pAgpmItem->RemoveItem(pAgpdBonusItem);
			eResult = AGPMREFINERY_RESULT_FAIL;
			return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);		
		}

		/// send result.
		BOOL bResult = SendPakcetRefineResult(pAgpdCharacter->m_lID,
											  0,
											  eResult,
											  makeitem->m_lMakeItemTID);
		
		return bResult;
	}


	return SendPakcetRefineResult(pAgpdCharacter->m_lID, 0, eResult, 0);				
}

//	Send Packet
//===============================================
//
BOOL AgsmRefinery::SendPakcetRefineResult(INT32 lCID, INT32 lItemTID, INT32 eResult, INT32 lResultItemTID)
	{
	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMREFINERY_PACKET_OPERATION_RESULT;

	PVOID pvPacket = m_pAgpmRefinery->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMREFINERY_PACKET_TYPE,
															&cOperation,
															&lCID,
															&lItemTID,
															NULL,
															&eResult,
															&lResultItemTID
															);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmCharacter->GetCharDPNID(lCID));
	m_pAgpmRefinery->m_csPacket.FreePacket(pvPacket);

	return bResult;	
	}


////	Helper
////==============================================
//// 정령석 정제시에 불림 또는 해골 정제시에
//BOOL AgsmRefinery::DeleteSourceItem(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, INT32 lSourceItems[])
//	{
//	if (!pAgpdCharacter)
//		return FALSE;
//
//	// ########## refine template의 모든 source에 대해 sub stack count
//
//		
//	// check item and quantity
//	INT32 lQuantity = pAgpdRefineTemplate->m_lQuantity;
//	INT32 lSub = 0;
//	AgpdItem *pAgpdItem = NULL;
//	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
//		{
//		pAgpdItem = m_pAgpmRefinery->m_pAgpmItem->GetItem(lSourceItems[i]);
//		if (!pAgpdItem || pAgpdRefineTemplate->m_lItemTID != pAgpdItem->m_lTID)
//			continue;
//		lSub = min(lQuantity, pAgpdItem->m_nCount);
//
//		// log. 아이템이 사라질 수 있어 미리 남긴다.
//		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_REFINE_PAY, pAgpdCharacter->m_lID,
//								pAgpdItem,
//								lSub
//								);	
//
//		m_pAgpmRefinery->m_pAgpmItem->SubItemStackCount(pAgpdItem, lSub);
//		lQuantity -= lSub;
//		}
//	
//	// pay
//	m_pAgpmCharacter->SubMoney(pAgpdCharacter, pAgpdRefineTemplate->m_lPrice);
//	
//	// log
//	m_pAgsmCharacter->WriteGheldLog(AGPDLOGTYPE_GHELD_REFINE_PAY, pAgpdCharacter, pAgpdRefineTemplate->m_lPrice);
//
//	return TRUE;
//	}

//아이템 정제시에 불림
BOOL AgsmRefinery::DeleteSourceItem(AgpdCharacter *pAgpdCharacter, INT32 lSourceItems[], INT32 lSourceItemsCount[])
{
	if (!pAgpdCharacter)
		return FALSE;

	// check item and quantity
	AgpdItem *pAgpdItem = NULL;
	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
	{
		pAgpdItem = m_pAgpmRefinery->m_pAgpmItem->GetItem(lSourceItems[i]);
		if (!pAgpdItem)
			continue;

		// log. 아이템이 사라질 수 있어 미리 남긴다.
		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_REFINE_PAY, 
			pAgpdCharacter->m_lID,
			pAgpdItem,
			pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1);

		ZeroMemory(pAgpdItem->m_szDeleteReason, sizeof(pAgpdItem->m_szDeleteReason));
		strncpy(pAgpdItem->m_szDeleteReason, "Refine material item", AGPMITEM_MAX_DELETE_REASON);

		m_pAgpmRefinery->m_pAgpmItem->SubItemStackCount(pAgpdItem, lSourceItemsCount[i]);
	}

	return TRUE;
}

BOOL AgsmRefinery::ApplyOption(AgpdRefineMakeItem* item, AgpdItem *pAgpdItem, AgpdCharacter *pcsCharacter)
{
    if(pAgpdItem)
        return m_pAgpmDropItem2->ProcessConvertItemOptionRefinery(pAgpdItem, pcsCharacter, item->m_lOptionMIN, item->m_lOptionMAX);
	return TRUE;
}


BOOL AgsmRefinery::ApplySocket(AgpdRefineMakeItem* item, AgpdItem *pAgpdItem)
{
	if(pAgpdItem)
		return m_pAgpmDropItem2->ProcessConvertItemSocket(pAgpdItem, item->m_lSocketInchantMIN, item->m_lSocketInchantMAX);
	return FALSE;
}

// 2008.07.22. steeple
// 재료에 Stamina 가 있고, 결과에도 Stamina 가 있는 거라면, 재료의 Stamina 값을 결과에 그대로 세팅해준다.
BOOL AgsmRefinery::ApplyStamina(AgpdRefineMakeItem* item, AgpdItem* pAgpdItem, AgpdItem* pcsSourceItems[])
{
	if(!pAgpdItem || !pcsSourceItems)
		return TRUE;

	if(pAgpdItem->m_pcsItemTemplate->m_llStaminaRemainTime == 0)
		return TRUE;

	AgpdItem* pcsSourceStaminaItem = NULL;
	for(int i = 0; i < AGPMREFINERY_MAX_GRID; ++i)
	{
		if(!pcsSourceItems[i]) continue;

		if( pcsSourceItems[i]->m_pcsItemTemplate->m_llStaminaRemainTime != 0 &&
			pcsSourceItems[i]->m_llStaminaRemainTime != 0)
		{
			pcsSourceStaminaItem = pcsSourceItems[i];
			break;
		}
	}

	// 없으면 나간다.
	if(!pcsSourceStaminaItem)
		return TRUE;

	// Stamina 세팅
	pAgpdItem->m_llStaminaRemainTime = pcsSourceStaminaItem->m_llStaminaRemainTime;

	return TRUE;
}