/*===========================================================================

	AgsmGuildWarehouse.cpp

===========================================================================*/


#include "AgsmGuildWarehouse.h"
#include "ApMemoryTracker.h"
#include "AuTimeStamp.h"


/************************************************************/
/*		The Implementation of AgsmGuildWarehouse class		*/
/************************************************************/
//
AgsmGuildWarehouse::AgsmGuildWarehouse()
	{
	SetModuleName(_T("AgsmGuildWarehouse"));
	SetModuleType(APMODULE_TYPE_SERVER);
	
	m_pAgpmLog = NULL;
	}


AgsmGuildWarehouse::~AgsmGuildWarehouse()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgsmGuildWarehouse::OnAddModule()
	{
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmItem				= (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmGuild			= (AgpmGuild *) GetModule(_T("AgpmGuild"));
	m_pAgpmGrid				= (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pAgpmGuildWarehouse	= (AgpmGuildWarehouse *) GetModule(_T("AgpmGuildWarehouse"));
	
	m_pAgsmCharacter		= (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmItem				= (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemManager		= (AgsmItemManager *) GetModule(_T("AgsmItemManager"));
	m_pAgsmGuild			= (AgsmGuild *) GetModule(_T("AgsmGuild"));

	m_pAgpmLog				= (AgpmLog *) GetModule(_T("AgpmLog"));

	if (!m_pAgpmCharacter || !m_pAgpmItem || !m_pAgpmGuild || !m_pAgpmGrid
		|| !m_pAgpmGuildWarehouse || !m_pAgsmCharacter || !m_pAgsmItem
		|| !m_pAgsmItemManager || !m_pAgsmGuild)
		return FALSE;

	if (!m_pAgsmGuild->SetCallbackGuildLoad(CBGuildLoaded, this))
		return FALSE;
	
	if (!m_pAgsmGuild->SetCallbackSendGuildInfo(CBAfterSendGuildInfo, this))
		return FALSE;
		
	// 길드 정책 변경 - arycoat 2008.02.
	if (!m_pAgpmGuild->SetCallbackGuildCheckDestroy(CBCheckGuildDestroy, this))
		return FALSE;

	if (!m_pAgpmGuildWarehouse->SetCallbackOpen(CBOpen, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackClose(CBClose, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackMoneyIn(CBMoneyIn, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackMoneyOut(CBMoneyOut, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackItemIn(CBItemIn, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackItemOut(CBItemOut, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackUpdateMoney(CBUpdateMoney, this))
		return FALSE;
	if (!m_pAgpmGuildWarehouse->SetCallbackExpand(CBExpand, this))
		return FALSE;

	if (!m_pAgpmItem->SetCallbackRemove(CBRemoveItem, this))
		return FALSE;
	if (!m_pAgpmItem->SetCallbackRemoveItemFromGuildWarehouse(CBRemoveItem, this))
		return FALSE;
	
	return TRUE;
	}




//	Operations
//==========================================
//
BOOL AgsmGuildWarehouse::OnLoadResult(AgpdCharacter *pAgpdCharacter, stBuddyRowset *pRowset, BOOL bEnd)
	{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
			return FALSE;	
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	if (bEnd)
		{
		pAgpdGuildWarehouse->m_eStatus = AGPDGUILDWAREHOUSE_STATUS_LOAD_COMPLETED;
		SendAllItems(pAgpdCharacter, pAgpdGuildWarehouse);
		if (pAgpdGuildWarehouse->m_pOpenedMember)
			pAgpdGuildWarehouse->m_pOpenedMember->insert(pAgpdCharacter->m_lID);
		SendOpenResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_LOAD_ENDED);
		return TRUE;
		}

	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
		{
		UINT32 ulCol = 0;
		CHAR *psz = NULL;

		// item
		UINT64	ullItemSeq = 0;
		INT32	lItemTID = 0;
		INT16	nStackCount = 0;
		CHAR	*pszPosition = NULL;
		CHAR	*pszConvert = NULL;
		INT32	lDurability = 0;
		INT32	lMaxDurability = 0;
		INT32	lFlag = 0;
		CHAR	*pszOption = NULL;
		CHAR	*pszSkillPlus = NULL;
		INT32	lInUse = 0;
		INT32	lUseCount = 0;
		INT32	lRemainTime = 0;
		INT32	lExpireDate = 0;
		INT64	llStaminaRemainTime = 0;

		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// item seq
			ullItemSeq = _ttoi64(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// item tid
			lItemTID = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// stack count
			nStackCount = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// position
			pszPosition = psz;
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// conv hist
			pszConvert = psz;
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// durability
			lDurability = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// max durability
			lMaxDurability = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// flag
			lFlag = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// option
			pszOption = psz;
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// option
			pszSkillPlus = psz;
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// in use
			lInUse = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// use count
			lUseCount = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// remain time
			lRemainTime = _ttoi(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// expire date
			lExpireDate = AuTimeStamp::ConvertOracleTimeToTimeStamp(psz);
		if (NULL != (psz = pRowset->Get(ul, ulCol++)))	// remain stamina time
			llStaminaRemainTime = _ttoi64(psz);

		AgpdItem *pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, nStackCount, pszPosition, AGPDITEM_STATUS_NONE,
															 pszConvert, lDurability, lMaxDurability, lFlag, pszOption, pszSkillPlus,
															 lInUse, lUseCount, lRemainTime, lExpireDate, llStaminaRemainTime,
															 ullItemSeq, pAgpdCharacter
															 );
		if (NULL == pAgpdItem)
			continue;
				
		// add to warehouse
		if (!m_pAgpmGuildWarehouse->AddItem(pAgpdGuildWarehouse, pAgpdItem, FALSE))
			{
			m_pAgpmItem->RemoveItem(pAgpdItem);
			continue;
			}
		}
	
	return TRUE;
	}


BOOL AgsmGuildWarehouse::OnOpen(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	// get warehouse
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	BOOL bResult = FALSE;
	
	switch (pAgpdGuildWarehouse->m_eStatus)
		{
		case AGPDGUILDWAREHOUSE_STATUS_LOAD_COMPLETED : // already loaded. send all items
			{
			SendAllItems(pAgpdCharacter, pAgpdGuildWarehouse);
			if (pAgpdGuildWarehouse->m_pOpenedMember)
				pAgpdGuildWarehouse->m_pOpenedMember->insert(pAgpdCharacter->m_lID);
			SendOpenResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_LOAD_ENDED);
			}
			break;
			
		case AGPDGUILDWAREHOUSE_STATUS_WAITING :	// waiting to load. send retry
			{
			SendOpenResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_RETRY);
			}
			break;
			
		case AGPDGUILDWAREHOUSE_STATUS_NOT_LOADED :	// not loaded
			{
			pAgpdGuildWarehouse->m_eStatus = AGPDGUILDWAREHOUSE_STATUS_WAITING;
			EnumCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_LOAD, pAgpdGuildWarehouse, pAgpdCharacter);
			}
			break;
			
		default :
			break;
		}

	return bResult;	
	}


BOOL AgsmGuildWarehouse::OnMoneyIn(AgpdCharacter *pAgpdCharacter, INT64 llMoney)
	{
	if (!pAgpdCharacter || 0 >= llMoney)
		return FALSE;

	// get warehouse
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	// previledge check	
	BOOL bPreviledge = FALSE;
	if (m_pAgpmGuild->IsMaster(pAgpdGuildWarehouse->m_pAgpdGuild, pAgpdCharacter->m_szID))
		bPreviledge = pAgpdGuildWarehouse->GetMasterPreviledge(AGPDGUILDWAREHOUSE_PREV_MONEY_IN);
	else
		bPreviledge = pAgpdGuildWarehouse->GetMemberPreviledge(AGPDGUILDWAREHOUSE_PREV_MONEY_IN);

	if (FALSE == bPreviledge)
		return SendMoneyResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_NOT_ENOUGH_PREV);
		
	// 길드 정책 변경 - arycoat 2008.02
	AgpdGuild* pagdGuild = m_pAgpmGuild->GetGuild(pAgpdCharacter);
	if(!pagdGuild) 
		return FALSE;

	if(m_pAgpmGuild->GetMemberCount(pagdGuild) < 10)
	{
		m_pAgsmGuild->SendSystemMessage( AGPMGUILD_SYSTEM_CODE_WAREHOUSE_NOT_ENOUGH_MEMBER, _GetCharacterNID(pAgpdCharacter));
		return FALSE;
	}
	
	// money check
	INT64 llInvenMoney = 0;
	if (!m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llInvenMoney))
		return FALSE;
	
	if (llMoney > llInvenMoney)
		return FALSE;
	
	// transfer
	if (!m_pAgpmCharacter->SubMoney(pAgpdCharacter, llMoney))
		return FALSE;
	
	if (!m_pAgpmGuildWarehouse->AddMoney(pAgpdGuildWarehouse, llMoney))
		{
		m_pAgpmCharacter->AddMoney(pAgpdCharacter, llMoney);	// restore
		return FALSE;
		}

	// log
	WriteMoneyLog(pAgpdCharacter, llMoney, TRUE);
	
	return TRUE;
	}


BOOL AgsmGuildWarehouse::OnMoneyOut(AgpdCharacter *pAgpdCharacter, INT64 llMoney)
{
	if (!pAgpdCharacter || 0 >= llMoney)
		return FALSE;

	// get warehouse
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	// previledge check	
	BOOL bPreviledge = FALSE;
	if (m_pAgpmGuild->IsMaster(pAgpdGuildWarehouse->m_pAgpdGuild, pAgpdCharacter->m_szID))
		bPreviledge = pAgpdGuildWarehouse->GetMasterPreviledge(AGPDGUILDWAREHOUSE_PREV_MONEY_OUT);
	else
		bPreviledge = pAgpdGuildWarehouse->GetMemberPreviledge(AGPDGUILDWAREHOUSE_PREV_MONEY_OUT);

	if (FALSE == bPreviledge)
		return SendMoneyResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_NOT_ENOUGH_PREV);

	if (m_pAgpmCharacter->CheckMoneySpace(pAgpdCharacter, llMoney))
	{
		if (!m_pAgpmGuildWarehouse->SubMoney(pAgpdGuildWarehouse, llMoney))
			return FALSE;

		m_pAgpmCharacter->AddMoney(pAgpdCharacter, llMoney);

		WriteMoneyLog(pAgpdCharacter, llMoney, FALSE);
	}
	else
	{
		if (AgsmSystemMessage* pcsAgsmSystemMessage = (AgsmSystemMessage*)GetModule("AgsmSystemMessage"))
			pcsAgsmSystemMessage->SendSystemMessage(pAgpdCharacter, AGPMSYSTEMMESSAGE_CODE_INVEN_MONEY_FULL);
	}

	return TRUE;
}


BOOL AgsmGuildWarehouse::OnItemIn(AgpdCharacter *pAgpdCharacter, INT32 lItemID, INT32 lLayer, INT32 lRow, INT32 lColumn)
	{
	if (!pAgpdCharacter || 0 == lItemID)
		return FALSE;

	// get warehouse
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	// previledge check	
	BOOL bPreviledge = FALSE;
	if (m_pAgpmGuild->IsMaster(pAgpdGuildWarehouse->m_pAgpdGuild, pAgpdCharacter->m_szID))
		bPreviledge = pAgpdGuildWarehouse->GetMasterPreviledge(AGPDGUILDWAREHOUSE_PREV_ITEM_IN);
	else
		bPreviledge = pAgpdGuildWarehouse->GetMemberPreviledge(AGPDGUILDWAREHOUSE_PREV_ITEM_IN);

	if (FALSE == bPreviledge)
		return FALSE;
		
	// 길드 정책 변경 - arycoat 2008.02
	AgpdGuild* pagdGuild = m_pAgpmGuild->GetGuild(pAgpdCharacter);
	if(!pagdGuild) 
		return FALSE;

	if(m_pAgpmGuild->GetMemberCount(pagdGuild) < 10)
	{
		m_pAgsmGuild->SendSystemMessage( AGPMGUILD_SYSTEM_CODE_WAREHOUSE_NOT_ENOUGH_MEMBER, _GetCharacterNID(pAgpdCharacter));
		return FALSE;
	}

	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(lItemID);
	if (!pAgpdItem)
		return FALSE;

	if(pAgpdItem->m_lExpireTime > 0)
	{
		return FALSE;
	}

	if (lLayer > pAgpdGuildWarehouse->m_lSlot)
		return FALSE;
	
	// owner & status check
	if (((AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus) && (AGPDITEM_STATUS_SUB_INVENTORY != pAgpdItem->m_eStatus)) ||
		pAgpdItem->m_pcsCharacter != pAgpdCharacter)
		return FALSE;
	
	// bound type check
	if (m_pAgpmItem->GetBoundType(pAgpdItem) != E_AGPMITEM_NOT_BOUND)
		return FALSE;

	INT16 nPrevStatus = pAgpdItem->m_eStatus;
	if(nPrevStatus == AGPDITEM_STATUS_INVENTORY)
	{
		// move
		if (!m_pAgpmItem->RemoveItemFromInventory(pAgpdCharacter, pAgpdItem))
			return FALSE;
	}
	else if(nPrevStatus == AGPDITEM_STATUS_SUB_INVENTORY)
	{
		// move
		if (!m_pAgpmItem->RemoveItemFromSubInventory(pAgpdCharacter, pAgpdItem))
			return FALSE;
	}
	
	if (!m_pAgpmGuildWarehouse->AddItem(pAgpdGuildWarehouse, pAgpdItem, lLayer, lRow, lColumn))
	{
		if(nPrevStatus == AGPDITEM_STATUS_INVENTORY)
			m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem,
											pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
											pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
											pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
											FALSE
											);
		else if(nPrevStatus == AGPDITEM_STATUS_SUB_INVENTORY)
			m_pAgpmItem->AddItemToSubInventory(pAgpdCharacter, pAgpdItem,
											pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
											pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
											pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
											FALSE
											);

		return FALSE;								
	}

	// DB
	EnumCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_IN, pAgpdGuildWarehouse, pAgpdItem);

	// log
	m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_GWARE_IN, pAgpdCharacter->m_lID, pAgpdItem, pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1);

	// notify
	NotifyItemAdd(pAgpdGuildWarehouse, pAgpdItem);

	return TRUE;
	}


BOOL AgsmGuildWarehouse::OnItemOut(AgpdCharacter *pAgpdCharacter, INT32 lItemID, INT32 lStatus, INT32 lLayer, INT32 lRow, INT32 lColumn)
	{
	if (!pAgpdCharacter || 0 == lItemID)
		return FALSE;

	// get warehouse
	AgpdGuild *pAgpdGuild = m_pAgpmGuild->GetGuild(pAgpdCharacter);
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdGuild);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
		return FALSE;

	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	// previledge check	
	BOOL bPreviledge = FALSE;
	if (m_pAgpmGuild->IsMaster(pAgpdGuildWarehouse->m_pAgpdGuild, pAgpdCharacter->m_szID))
		bPreviledge = pAgpdGuildWarehouse->GetMasterPreviledge(AGPDGUILDWAREHOUSE_PREV_ITEM_OUT);
	else
		bPreviledge = pAgpdGuildWarehouse->GetMemberPreviledge(AGPDGUILDWAREHOUSE_PREV_ITEM_OUT);

	if (FALSE == bPreviledge)
		return FALSE;

	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(lItemID);
	if (!pAgpdItem)
		return FALSE;

	if(lStatus == AGPDITEM_STATUS_INVENTORY)
	{
		// inventory full check
		if(m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
			return FALSE;
	}
	else if(lStatus == AGPDITEM_STATUS_SUB_INVENTORY)
	{
		// sub inventory full check
		if(m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csSubInventoryGrid))
			return FALSE;
	}

	if (!m_pAgpmGuildWarehouse->RemoveItem(pAgpdGuildWarehouse, pAgpdItem))
		return FALSE;

	if(lStatus == AGPDITEM_STATUS_INVENTORY)
	{
		if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, lLayer, lRow, lColumn, FALSE))
			m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE);
	}
	else if(lStatus == AGPDITEM_STATUS_SUB_INVENTORY)
	{
		if (!m_pAgpmItem->AddItemToSubInventory(pAgpdCharacter, pAgpdItem, lLayer, lRow, lColumn, FALSE))
			m_pAgpmItem->AddItemToSubInventory(pAgpdCharacter, pAgpdItem, FALSE);
	}

	// DB
	EnumCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_OUT, pAgpdGuildWarehouse, pAgpdItem);

	// log
	m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_GWARE_OUT, pAgpdCharacter->m_lID, pAgpdItem, pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1);

	// notify
	NotifyItemRemove(pAgpdGuildWarehouse, pAgpdItem, pAgpdCharacter);

	return TRUE;
	}


BOOL AgsmGuildWarehouse::OnUpdateMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llDiff)
	{
	// save to db
	EnumCallback(AGSMGUILDWAREHOUSE_CB_DB_MONEY_UPDATE, pAgpdGuildWarehouse, NULL);
	
	// notify to all guild members
	return NotifyMoney(pAgpdGuildWarehouse, pAgpdGuildWarehouse->m_llMoney);
	}


BOOL AgsmGuildWarehouse::OnExpand(AgpdCharacter *pAgpdCharacter)
	{
	//###############
	return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (pAgpdGuildWarehouse->m_lSlot > 4)
		return FALSE;

	// enough money to pay?	
	INT64 llCost = 0;
	if (pAgpdCharacter->m_llMoney < llCost)
		return SendExpandResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_NOT_ENOUGH_MONEY);

	// pay
	m_pAgpmCharacter->SubMoney(pAgpdCharacter, llCost);
	
	// notify to all guild members
	return SendExpandResult(pAgpdCharacter, AGPMGUILDWAREHOUSE_RESULT_SUCCESS);
	}


BOOL AgsmGuildWarehouse::OnItemRemove(AgpdItem *pAgpdItem)
	{
	if (!pAgpdItem || !pAgpdItem->m_pcsGuildWarehouseBase)
		return FALSE;

	AgpdGuildWarehouse *pAgpdGuildWarehouse = (AgpdGuildWarehouse *) pAgpdItem->m_pcsGuildWarehouseBase;

	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (!m_pAgpmGuildWarehouse->RemoveItem(pAgpdGuildWarehouse, pAgpdItem))
		return FALSE;

	// DB
	EnumCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_OUT, pAgpdGuildWarehouse, pAgpdItem);

	return TRUE;
	}




//	Callbacks
//======================================
//
BOOL AgsmGuildWarehouse::CBGuildLoaded(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdGuild *pAgpdGuild = (AgpdGuild *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;
	if (NULL == ppvBuffer[0] || NULL == ppvBuffer[1])
		return FALSE;
	
	INT64 llMoney = *((INT64 *)ppvBuffer[0]);
	INT32 lSlot = *((INT32 *)ppvBuffer[1]);
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdGuild);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	pAgpdGuildWarehouse->m_llMoney = llMoney;
	pAgpdGuildWarehouse->m_lSlot = lSlot;
	
	return TRUE;
	}


BOOL AgsmGuildWarehouse::CBAfterSendGuildInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return TRUE;
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	return pThis->SendGuildWarehouseBase(pAgpdCharacter, pAgpdGuildWarehouse);
	}


BOOL AgsmGuildWarehouse::CBOpen(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	return pThis->OnOpen(pAgpdCharacter);
	}


BOOL AgsmGuildWarehouse::CBClose(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (pAgpdGuildWarehouse && pAgpdGuildWarehouse->m_pOpenedMember)
		pAgpdGuildWarehouse->m_pOpenedMember->erase(pAgpdCharacter->m_lID);
	
	return TRUE;
	}


BOOL AgsmGuildWarehouse::CBMoneyIn(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	INT64 llMoney = *((INT64 *) pCustData);
	
	return pThis->OnMoneyIn(pAgpdCharacter, llMoney);
	}


BOOL AgsmGuildWarehouse::CBMoneyOut(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	INT64 llMoney = *((INT64 *) pCustData);
	
	return pThis->OnMoneyOut(pAgpdCharacter, llMoney);
	}


BOOL AgsmGuildWarehouse::CBItemIn(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;
	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2] || !ppvBuffer[3])
		return FALSE;
	
	INT32 lItemID = *((INT32 *) ppvBuffer[0]);
	INT32 lLayer = *((INT32 *) ppvBuffer[1]);
	INT32 lRow = *((INT32 *) ppvBuffer[2]);
	INT32 lColumn = *((INT32 *) ppvBuffer[3]);
	
	return pThis->OnItemIn(pAgpdCharacter, lItemID, lLayer, lRow, lColumn);
	}


BOOL AgsmGuildWarehouse::CBItemOut(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;
	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2] || !ppvBuffer[3] || !ppvBuffer[4])
		return FALSE;
	
	INT32 lItemID = *((INT32 *) ppvBuffer[0]);
	INT32 lLayer = *((INT32 *) ppvBuffer[1]);
	INT32 lRow = *((INT32 *) ppvBuffer[2]);
	INT32 lColumn = *((INT32 *) ppvBuffer[3]);
	INT32 lStatus = *((INT32 *) ppvBuffer[4]);
	
	return pThis->OnItemOut(pAgpdCharacter, lItemID, lStatus, lLayer, lRow, lColumn);
	}


BOOL AgsmGuildWarehouse::CBUpdateMoney(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdGuildWarehouse *pAgpdGuildWarehouse = (AgpdGuildWarehouse *) pData;
	INT64 llDiff = *((INT64 *) pCustData);
	
	return pThis->OnUpdateMoney(pAgpdGuildWarehouse, llDiff);
	}


BOOL AgsmGuildWarehouse::CBExpand(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	return pThis->OnExpand(pAgpdCharacter);
	}


BOOL AgsmGuildWarehouse::CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmGuildWarehouse *pThis = (AgsmGuildWarehouse *) pClass;
	AgpdItem *pAgpdItem = (AgpdItem *) pData;

	return pThis->OnItemRemove(pAgpdItem);
	}

// 길드 정책 변경 - arycoat 2008.02	
BOOL AgsmGuildWarehouse::CBCheckGuildDestroy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuildWarehouse	*pThis		= (AgsmGuildWarehouse*)	pClass;
	AgpdGuild		*pcsGuild		= (AgpdGuild *)			pData;
	BOOL			*pbCheckResult	= (BOOL *)				pCustData;

	if (!*pbCheckResult)
		return TRUE;
		
	AgpdGuildWarehouse* pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pcsGuild);
	if(!pAgpdGuildWarehouse) return TRUE;
	
	// 돈이 있으면 해체 불가
	if(pAgpdGuildWarehouse->m_llMoney > 0)
	{
		*pbCheckResult	= FALSE;
		
		pThis->m_pAgsmGuild->SendSystemMessage( AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY_NOT_EMPTY_WAREHOUSE, pThis->m_pAgsmGuild->GetMemberNID(pcsGuild->GetMaster()));
		
		return TRUE;
	}
		
	for (int i = 0; i < pAgpdGuildWarehouse->m_WarehouseGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pAgpdGuildWarehouse->m_WarehouseGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pAgpdGuildWarehouse->m_WarehouseGrid.m_nRow; ++k)
			{
				AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(&pAgpdGuildWarehouse->m_WarehouseGrid, i, k, j);
				if (pAgpdGridItem)
				{
					// 아이템이 있어도 해체 불가
					*pbCheckResult	= FALSE;
					
					pThis->m_pAgsmGuild->SendSystemMessage( AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY_NOT_EMPTY_WAREHOUSE, pThis->m_pAgsmGuild->GetMemberNID(pcsGuild->GetMaster()));
					
					return TRUE;
				}
			}
		}
	}

	return TRUE;
}



//	Notify
//===================================================
//
BOOL AgsmGuildWarehouse::NotifyItemAdd(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem)
	{
	if (!pAgpdGuildWarehouse || !pAgpdGuildWarehouse->m_pOpenedMember || !pAgpdItem)
		return FALSE;
	
	for (OpenedMemberIter Iter = pAgpdGuildWarehouse->m_pOpenedMember->begin();
		 Iter != pAgpdGuildWarehouse->m_pOpenedMember->end();
		 Iter++)
		 {
		 SendItemAdd(m_pAgpmCharacter->GetCharacter(*Iter), pAgpdItem);
		 }
		
	return TRUE;
	}


BOOL AgsmGuildWarehouse::NotifyItemRemove(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem, AgpdCharacter *pAgpdCharacterMe)
	{
	if (!pAgpdGuildWarehouse || !pAgpdGuildWarehouse->m_pOpenedMember || !pAgpdItem)
		return FALSE;
	
	for (OpenedMemberIter Iter = pAgpdGuildWarehouse->m_pOpenedMember->begin();
		Iter != pAgpdGuildWarehouse->m_pOpenedMember->end();
		Iter++)
		{
		AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(*Iter);
		if (pAgpdCharacter != pAgpdCharacterMe)		// 아이템을 뺀놈은 지워져서는 안된다.
			SendItemRemove(pAgpdCharacter, pAgpdItem);
		else
			m_pAgsmItem->SendPacketItem(pAgpdItem, _GetCharacterNID(pAgpdCharacter));
		}
		
	return TRUE;
	}


BOOL AgsmGuildWarehouse::NotifyMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llMoney)
	{
	if (!pAgpdGuildWarehouse || !pAgpdGuildWarehouse->m_pOpenedMember)
		return FALSE;
	
	for (OpenedMemberIter Iter = pAgpdGuildWarehouse->m_pOpenedMember->begin();
		 Iter != pAgpdGuildWarehouse->m_pOpenedMember->end();
		 Iter++)
		 {
		 SendUpdateMoney(m_pAgpmCharacter->GetCharacter(*Iter), llMoney);
		 }
		
	return TRUE;
	}




//	Packet Send
//===================================================
//
BOOL AgsmGuildWarehouse::SendGuildWarehouseBase(AgpdCharacter *pAgpdCharacter, AgpdGuildWarehouse *pAgpdGuildWarehouse)
	{
	if (!pAgpdCharacter || !pAgpdGuildWarehouse)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_BASE_INFO;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,						// operation
																  &pAgpdCharacter->m_lID,			// cid
																  &pAgpdGuildWarehouse->m_llMoney,	// money
																  &pAgpdGuildWarehouse->m_lSlot,	// slot
																  NULL,								// item id
																  NULL,								// layer
																  NULL,								// row
																  NULL,								// column
																  NULL								// result
																  );

	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgsmGuildWarehouse::SendAllItems(AgpdCharacter *pAgpdCharacter, AgpdGuildWarehouse *pAgpdGuildWarehouse)
	{
	if (!pAgpdCharacter || !pAgpdGuildWarehouse)
		return FALSE;

	for (int i = 0; i < pAgpdGuildWarehouse->m_WarehouseGrid.m_nLayer; ++i)
		{
		for (int j = 0; j < pAgpdGuildWarehouse->m_WarehouseGrid.m_nColumn; ++j)
			{
			for (int k = 0; k < pAgpdGuildWarehouse->m_WarehouseGrid.m_nRow; ++k)
				{
				AgpdGridItem *pAgpdGridItem = m_pAgpmGrid->GetItem(&pAgpdGuildWarehouse->m_WarehouseGrid, i, k, j);
				if (pAgpdGridItem)
					{
					AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem);
					SendItemAdd(pAgpdCharacter, pAgpdItem);
					}
				}
			}
		}

	return TRUE;
	}


BOOL AgsmGuildWarehouse::SendItemAdd(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem)
	{
	if (!pAgpdCharacter || !pAgpdItem)
		return FALSE;
	
	return m_pAgsmItem->SendPacketItem(pAgpdItem, _GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmGuildWarehouse::SendItemRemove(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem)
	{
	if (!pAgpdCharacter || !pAgpdItem)
		return FALSE;
	
	return m_pAgsmItem->SendPacketItemRemove(pAgpdItem->m_lID, _GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmGuildWarehouse::SendOpenResult(AgpdCharacter *pAgpdCharacter, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_OPEN;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,					// operation
																  &pAgpdCharacter->m_lID,		// cid
																  NULL,							// money
																  NULL,							// slot
																  NULL,							// item id
																  NULL,							// layer
																  NULL,							// row
																  NULL,							// column
																  &lResult						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgsmGuildWarehouse::SendMoneyResult(AgpdCharacter *pAgpdCharacter, INT32 lResult)
{
	return FALSE;
}


BOOL AgsmGuildWarehouse::SendUpdateMoney(AgpdCharacter *pAgpdCharacter, INT64 llMoney)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_BASE_INFO;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,				// operation
																  &pAgpdCharacter->m_lID,	// cid
																  &llMoney,					// money
																  NULL,						// slot
																  NULL,						// item id
																  NULL,						// layer
																  NULL,						// row
																  NULL,						// column
																  NULL						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgsmGuildWarehouse::SendExpandResult(AgpdCharacter *pAgpdCharacter, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_EXPAND;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,					// operation
																  &pAgpdCharacter->m_lID,		// cid
																  NULL,							// money
																  NULL,							// slot
																  NULL,							// item id
																  NULL,							// layer
																  NULL,							// row
																  NULL,							// column
																  &lResult						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}




//	Callback Setting
//===============================================
//
BOOL AgsmGuildWarehouse::SetCallbackDBMoneyUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMGUILDWAREHOUSE_CB_DB_MONEY_UPDATE, pfCallback, pClass);
	}


BOOL AgsmGuildWarehouse::SetCallbackDBItemLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_LOAD, pfCallback, pClass);
	}


BOOL AgsmGuildWarehouse::SetCallbackDBItemIn(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_IN, pfCallback, pClass);
	}


BOOL AgsmGuildWarehouse::SetCallbackDBItemOut(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMGUILDWAREHOUSE_CB_DB_ITEM_OUT, pfCallback, pClass);
	}




//	Helper
//===============================================
//
UINT32 AgsmGuildWarehouse::_GetCharacterNID(INT32 lCID)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(lCID);
	}


UINT32 AgsmGuildWarehouse::_GetCharacterNID(AgpdCharacter *pAgpdCharacter)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter);
	}




//	Log
//===================================================
//
BOOL AgsmGuildWarehouse::WriteMoneyLog(AgpdCharacter *pAgpdCharacter, INT64 llMoney, BOOL bIn)
	{
	if (NULL == m_pAgpmLog || NULL == pAgpdCharacter)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	AgpdGuild *pAgpdGuild = m_pAgpmGuild->GetGuild(pAgpdCharacter);

	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdGuild);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	INT32 lMoney = (INT32) llMoney;
	
	return m_pAgpmLog->WriteLog_Gheld(bIn ? AGPDLOGTYPE_GHELD_GWARE_IN : AGPDLOGTYPE_GHELD_GWARE_OUT,
									0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
									m_pAgpmCharacter->GetLevel(pAgpdCharacter),
									m_pAgpmCharacter->GetExp(pAgpdCharacter),
									pAgpdCharacter->m_llMoney,
									pAgpdGuildWarehouse->m_llMoney,
									lMoney,
									pAgpdGuild ? pAgpdGuild->m_szID : _T("")
									);
	}


