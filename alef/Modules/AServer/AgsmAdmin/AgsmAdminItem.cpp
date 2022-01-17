// AgsmAdminItem.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 03. 29.
//
// Code 양이 너무 길어져서 관리를 위해서 분리

#include "AgsmAdmin.h"
#include <list>

//////////////////////////////////////////////////////////////////////////
// Item Operation
BOOL AgsmAdmin::ProcessItemOperation(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
		return FALSE;

	switch(pstItemOperation->m_cOperation)
	{
		case AGPMADMIN_ITEM_CREATE:
			ProcessItemCreate(pstItemOperation, pcsAgpdCharacter);
			break;
		
		case AGPMADMIN_ITEM_CONVERT:
			ProcessItemConvert(pstItemOperation, pcsAgpdCharacter);
			break;

		case AGPMADMIN_ITEM_DELETE:
			ProcessItemDelete(pstItemOperation, pcsAgpdCharacter);
			break;

		case AGPMADMIN_ITEM_OPTION_ADD:
			ProcessItemOptionAdd(pstItemOperation, pcsAgpdCharacter);
			break;

		case AGPMADMIN_ITEM_OPTION_REMOVE:
			ProcessItemOptionRemove(pstItemOperation, pcsAgpdCharacter);
			break;

		case AGPMADMIN_ITEM_UPDATE:
			ProcessItemUpdate(pstItemOperation, pcsAgpdCharacter);
			break;
	}

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	/*char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[ItemOperation] %s(%s) : Operation(%d), Target(%s,%s), %I64d, TID(%d), Count(%d), PhLv(%d), Soc(%d), Opt(%d,%d,%d,%d,%d)\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		pstItemOperation->m_cOperation,
		pstItemOperation->m_szCharName,
		pstItemOperation->m_szAccName,
		pstItemOperation->m_ullDBID,
		pstItemOperation->m_lTID,
		pstItemOperation->m_lCount,
		pstItemOperation->m_cPhysicalConvertLevel,
		pstItemOperation->m_cSocket,
		pstItemOperation->m_alOptionTID[0],
		pstItemOperation->m_alOptionTID[1],
		pstItemOperation->m_alOptionTID[2],
		pstItemOperation->m_alOptionTID[3],
		pstItemOperation->m_alOptionTID[4]);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);*/

	return TRUE;
}

BOOL AgsmAdmin::ProcessItemCreate(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter, BOOL bCheckAdmin, BOOL bIncludeOffLine, AgpdCharacter* pcsTarget)
{
	if(!m_pagpmItem || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation)
		return FALSE;

	// 개수 확인
	if(pstItemOperation->m_lCount <= 0)
		pstItemOperation->m_lCount = 1;

	INT32 lCreatedCount = 0;
	BOOL bSuccess = FALSE;

	AgpdItem* pcsAgpdItem = NULL;
	AgpdItemTemplate* pcsAgpdItemTemplate = NULL;
	AgsdItem* pcsAgsdItem = NULL;
	
	// 먼저 템플릿을 구한다.
	pcsAgpdItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsAgpdItemTemplate)
		return FALSE;

	INT32 lCID = 0;
	INT32 dpnID = 0;
	if(bCheckAdmin)
	{
		if(!pcsAgpdCharacter)
			return FALSE;

		// 캐쉬 아이템은 레벨5 부터 가능이다.
		if(IS_CASH_ITEM(pcsAgpdItemTemplate->m_eCashItemType) && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		lCID = pcsAgpdCharacter->m_lID;
		dpnID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);
	}

	AgpdCharacter* pcsCharacter = NULL;
	if(pcsTarget)	// 타겟이 넘어 온 경우이다.
	{
		pcsCharacter = pcsTarget;
		if(pcsAgpdCharacter && strcmp(pcsCharacter->m_szID, pcsAgpdCharacter->m_szID) == 0)
			pcsCharacter->m_Mutex.WLock();
	}
	else
	{
		if(pcsAgpdCharacter && strcmp(pstItemOperation->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
			pcsCharacter = pcsAgpdCharacter;
		else
			pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstItemOperation->m_szCharName);
	}

	if(!pcsCharacter)
	{
		// 서버에 없는 캐릭에 넣는 거라면 다른 함수 호출해준다.
		if(bIncludeOffLine)
			ProcessItemCreateToDB(pstItemOperation, pcsAgpdCharacter);

		return TRUE;
	}

	// 돈이면 해당 캐릭터의 인벤토리 돈을 직접 올려준다.
	if(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther*)pcsAgpdItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		INT64 llPrevMoney = 0, llNewMoney = 0;

		m_pagpmCharacter->GetMoney(pcsCharacter, &llPrevMoney);

		m_pagpmCharacter->AddMoney(pcsCharacter, pstItemOperation->m_lCount);
		m_pagpmCharacter->UpdateMoney(pcsCharacter);

		m_pagpmCharacter->GetMoney(pcsCharacter, &llNewMoney);

		lCreatedCount = (INT32)(llNewMoney - llPrevMoney);
		bSuccess = TRUE;

		WriteLog_Gheld(AGPDLOGTYPE_GHELD_GMEDIT, pcsAgpdCharacter, pcsCharacter);
	}
	else
	{
		//custom옵션이 넘어왔을 때 구별해서 만든다.
		if(pstItemOperation->m_alOptionTID[0] != 0)
		{
			if(pcsAgpdItemTemplate->m_bStackable)
				pcsAgpdItem = m_pagsmItemManager->CreateItemWithOutOption(pstItemOperation->m_lTID, pcsCharacter, pstItemOperation->m_lCount);
			else
				pcsAgpdItem = m_pagsmItemManager->CreateItemWithOutOption(pstItemOperation->m_lTID, pcsCharacter);

			if(pstItemOperation->m_alOptionTID[0] != -1)
			{
				for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
				{
					if(pstItemOperation->m_alOptionTID[i] == 0)
						break;

					m_pagpmItem->AddItemOption(pcsAgpdItem, pstItemOperation->m_alOptionTID[i], FALSE);
				}
			}
		}
		else
		{
			// Stackable 인지 구별해서 만든다.
			if(pcsAgpdItemTemplate->m_bStackable)
				pcsAgpdItem = m_pagsmItemManager->CreateItem(pstItemOperation->m_lTID, pcsCharacter, pstItemOperation->m_lCount);
			else
				pcsAgpdItem = m_pagsmItemManager->CreateItem(pstItemOperation->m_lTID, pcsCharacter);
		}

		if(!pcsAgpdItem)
		{
			if(pcsCharacter != pcsAgpdCharacter)
				pcsCharacter->m_Mutex.Release();

			return FALSE;
		}

		// 귀속아이템이라면 귀속과 주인을 세팅해준다. 2005.11.02. steeple
		if(m_pagpmItem->GetBoundType(pcsAgpdItem->m_pcsItemTemplate) == E_AGPMITEM_BIND_ON_ACQUIRE)
		{
			m_pagpmItem->SetBoundType(pcsAgpdItem, E_AGPMITEM_BIND_ON_ACQUIRE);
			m_pagpmItem->SetBoundOnOwner(pcsAgpdItem, pcsCharacter);
		}

		// 2006.01.03. steeple
		// 캐쉬 아이템이면 추가 정보 세팅
		if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		{
			pcsAgpdItem->m_nInUseItem = pstItemOperation->m_nInUseItem;
			pcsAgpdItem->m_lRemainTime = pstItemOperation->m_lRemainTime;
			pcsAgpdItem->m_lExpireTime = pstItemOperation->m_lExpireTime;
			pcsAgpdItem->m_llStaminaRemainTime = pstItemOperation->m_llStaminaRemainTime;
		}

		// Skill Plus 해준다. 입력받은 대로만 plus해준다.
		for(int i = 0; i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; ++i)
		{
			if(pstItemOperation->m_alSkillPlusTID[i] > 0)
				m_pagpmItem->AddItemSkillPlus(pcsAgpdItem, pstItemOperation->m_alSkillPlusTID[i]);
		}

		// 일단 만든 거 넣어주고....
		bSuccess = m_pagpmItem->AddItemToInventory(pcsCharacter, pcsAgpdItem);
		if(bSuccess)
		{
			lCreatedCount++;

			if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
			{
				pcsAgsdItem = m_pagsmItem->GetADItem(pcsAgpdItem);
				stCashItemBuyList csList;
				csList.m_lItemQty = pcsAgpdItemTemplate->m_bStackable ? pstItemOperation->m_lCount : 1;
				csList.m_lItemTID = pcsAgpdItem->m_pcsItemTemplate->m_lID;
				csList.m_llMoney = 0;
				csList.m_ullItemSeq = pcsAgsdItem->m_ullDBIID;
				csList.m_ullOrderNo = 0;
				csList.m_cStatus = AGSDITEM_CASHITEMBUYLIST_STATUS_GMGIVE;
				
				m_pagsmItem->WriteCashItemBuyList(pcsCharacter, &csList);
			}

			WriteLog_Item(AGPDLOGTYPE_ITEM_GM_MAKE, pcsAgpdCharacter, pcsCharacter, pcsAgpdItem,
						pcsAgpdItemTemplate->m_bStackable ? pstItemOperation->m_lCount : 1);
		}
		else
		{
			SendItemOperationResult(pstItemOperation->m_lTID, AGPMADMIN_ITEM_RESULT_FAILED_INVEN_FULL, pstItemOperation->m_lCount, lCID, dpnID);

			ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON  + 1));
			strncpy(pcsAgpdItem->m_szDeleteReason, "Admin : AddItemToInventory() was failed", AGPMITEM_MAX_DELETE_REASON);
			m_pagpmItem->RemoveItem(pcsAgpdItem, TRUE);

			if(pcsCharacter != pcsAgpdCharacter)
				pcsCharacter->m_Mutex.Release();

			return TRUE;
		}

		// Stackable 이 아니면 나머지 채워준다.
		if(!pcsAgpdItemTemplate->m_bStackable)
		{
			for(lCreatedCount; lCreatedCount < pstItemOperation->m_lCount; lCreatedCount++)
			{
				pcsAgpdItem = m_pagsmItemManager->CreateItem(pstItemOperation->m_lTID, pcsCharacter);
				if(!pcsAgpdItem)
					break;

				// 2006.01.03. steeple
				// 캐쉬 아이템이면 추가 정보 세팅
				if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
				{
					pcsAgpdItem->m_nInUseItem = pstItemOperation->m_nInUseItem;
					pcsAgpdItem->m_lRemainTime = pstItemOperation->m_lRemainTime;
					pcsAgpdItem->m_lExpireTime = pstItemOperation->m_lExpireTime;
					pcsAgpdItem->m_llStaminaRemainTime = pstItemOperation->m_llStaminaRemainTime;
				}

				bSuccess = m_pagpmItem->AddItemToInventory(pcsCharacter, pcsAgpdItem);
				if(!bSuccess)
				{
					m_pagpmItem->RemoveItem(pcsAgpdItem);
					break;
				}

				// 로그를 남긴다.
				if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
				{
					pcsAgsdItem = m_pagsmItem->GetADItem(pcsAgpdItem);
					stCashItemBuyList csList;
					csList.m_lItemQty = pcsAgpdItemTemplate->m_bStackable ? pstItemOperation->m_lCount : 1;
					csList.m_lItemTID = pcsAgpdItem->m_pcsItemTemplate->m_lID,
					csList.m_llMoney = 0;
					csList.m_ullItemSeq = pcsAgsdItem->m_ullDBIID;
					csList.m_ullOrderNo = 0;
					csList.m_cStatus = AGSDITEM_CASHITEMBUYLIST_STATUS_GMGIVE;
					
					m_pagsmItem->WriteCashItemBuyList(pcsCharacter, &csList);
				}

				WriteLog_Item(AGPDLOGTYPE_ITEM_GM_MAKE, pcsAgpdCharacter, pcsCharacter, pcsAgpdItem, 1);
			}
		}	// !pcsAgpdItemTemplate->m_bStackable
	}

	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	if(pcsAgpdCharacter)
	{
		// 결과를 보낸다.
		eAgpmAdminItemOperationResult eResult = lCreatedCount > 0 ? AGPMADMIN_ITEM_RESULT_SUCCESS_CREATE : AGPMADMIN_ITEM_RESULT_FAILED_UNKNOWN;
		//SendItemOperationResult(pstItemOperation->m_lTID, eResult, lCreatedCount, lCID, dpnID);

		pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
		pstItemOperation->m_cResult = (INT8)eResult;
		pstItemOperation->m_lCount = lCreatedCount;

		SendItemOperationResult(pstItemOperation, lCID, dpnID);
	}

	return TRUE;
}

// 2005.05.18. steeple
BOOL AgsmAdmin::ProcessItemCreateToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	// 개수 확인
	if(pstItemOperation->m_lCount <= 0)
		pstItemOperation->m_lCount = 1;

	AgpdItem* pcsAgpdItem = NULL;
	AgpdItemTemplate* pcsAgpdItemTemplate = NULL;
	AgsdItem* pcsAgsdItem = NULL;
	
	// 먼저 템플릿을 구한다.
	pcsAgpdItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsAgpdItemTemplate)
		return FALSE;

	// 돈인 경우에는 그냥 리턴. 돈을 DB 에 업데이트 할때는 Character 정보 바꾸는 데서 한다.
	if(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther*)pcsAgpdItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		return FALSE;
	}

	//custom옵션이 넘어왔을 때 구별해서 만든다.
	if(pstItemOperation->m_alOptionTID[0] != 0)
	{
		if(pcsAgpdItemTemplate->m_bStackable)
			pcsAgpdItem = m_pagsmItemManager->CreateItemWithOutOption(pstItemOperation->m_lTID, NULL, pstItemOperation->m_lCount);
		else
			pcsAgpdItem = m_pagsmItemManager->CreateItemWithOutOption(pstItemOperation->m_lTID, NULL);

		if(pstItemOperation->m_alOptionTID[0] != -1)
		{
			for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
			{
				if(pstItemOperation->m_alOptionTID[i] == 0)
					break;

				m_pagpmItem->AddItemOption(pcsAgpdItem, pstItemOperation->m_alOptionTID[i], FALSE);
			}
		}
	}
	else
	{
		// Stackable 인지 구별해서 만든다.
		if(pcsAgpdItemTemplate->m_bStackable)
			pcsAgpdItem = m_pagsmItemManager->CreateItem(pstItemOperation->m_lTID, NULL, pstItemOperation->m_lCount);
		else
			pcsAgpdItem = m_pagsmItemManager->CreateItem(pstItemOperation->m_lTID, NULL);
	}

	if(!pcsAgpdItem)
		return FALSE;

	// 귀속아이템이라면 귀속과 주인을 세팅해준다. 2005.11.02. steeple
	// 여기서는 주인 세팅하기가 낭패이다. Bound_On_Owner 세팅해준다.
	if(m_pagpmItem->GetBoundType(pcsAgpdItem->m_pcsItemTemplate) == E_AGPMITEM_BIND_ON_ACQUIRE)
	{
		m_pagpmItem->SetBoundType(pcsAgpdItem, E_AGPMITEM_BIND_ON_ACQUIRE);
		pcsAgpdItem->m_lStatusFlag |= AGPMITEM_BOUND_ON_OWNER;
		//m_pagpmItem->SetBoundOnOwner(pcsAgpdItem, pcsCharacter);
	}

	// 위치 정보를 세팅해주고
	if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType) == FALSE)
		pcsAgpdItem->m_eStatus = AGPDITEM_STATUS_INVENTORY;
	else
		pcsAgpdItem->m_eStatus = AGPDITEM_STATUS_CASH_INVENTORY;

	pcsAgpdItem->m_anGridPos[0] = pstItemOperation->m_lLayer;
	pcsAgpdItem->m_anGridPos[1] = pstItemOperation->m_lRow;
	pcsAgpdItem->m_anGridPos[2] = pstItemOperation->m_lColumn;

	// 2006.01.03. steeple
	// 캐쉬 아이템이면 추가 정보 세팅
	if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		pcsAgpdItem->m_nInUseItem = pstItemOperation->m_nInUseItem;
		pcsAgpdItem->m_lRemainTime = pstItemOperation->m_lRemainTime;
		pcsAgpdItem->m_lExpireTime = pstItemOperation->m_lExpireTime;
		pcsAgpdItem->m_llStaminaRemainTime = pstItemOperation->m_llStaminaRemainTime;
	}

	// Skill Plus 해준다. 입력받은 대로만 plus해준다.
	for(int i = 0; i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; ++i)
	{
		if(pstItemOperation->m_alSkillPlusTID[i] > 0)
			m_pagpmItem->AddItemSkillPlus(pcsAgpdItem, pstItemOperation->m_alSkillPlusTID[i]);
	}

    // 여기서 일단 Insert 를 해준다.
	EnumCallback(AGSMADMIN_CB_RELAY_ITEM_INSERT, pcsAgpdItem, pstItemOperation);

	// 잠깐 쉬어주고 (INSERT 쿼리가 저장 되기 위해서. 넘 짧아서 안될 수도 있지만...)
	Sleep(0);

	// Convert 해야 할 게 있으면 해준다.
	AgpdItemConvertADItem* pcsConvertADItem = m_pagpmItemConvert->GetADItem(pcsAgpdItem);
	if(!pcsConvertADItem)
		return TRUE;

	// 무엇을 개조하는 지 보고 따로 처리해준다. 물리개조와 소켓개수만 가능하다. (2005.05.18. 기준)
	BOOL bConvertUpdate = FALSE;
	if(pstItemOperation->m_cPhysicalConvertLevel > 0 && pstItemOperation->m_cPhysicalConvertLevel != pcsConvertADItem->m_lPhysicalConvertLevel)
		bConvertUpdate |= m_pagpmItemConvert->SetPhysicalConvert(pcsAgpdItem, pstItemOperation->m_cPhysicalConvertLevel, FALSE);
	if(pstItemOperation->m_cSocket > pcsConvertADItem->m_lNumSocket)
		bConvertUpdate |= m_pagpmItemConvert->SetSocketConvert(pcsAgpdItem, pstItemOperation->m_cSocket);

	if(bConvertUpdate)
		EnumCallback(AGSMADMIN_CB_RELAY_ITEM_CONVERT_UPDATE, pcsAgpdItem, NULL);


	if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		pcsAgsdItem = m_pagsmItem->GetADItem(pcsAgpdItem);
		stCashItemBuyList csList;
		csList.m_lItemQty = pcsAgpdItemTemplate->m_bStackable ? pstItemOperation->m_lCount : 1;
		csList.m_lItemTID = pcsAgpdItem->m_pcsItemTemplate->m_lID,
		csList.m_llMoney = 0;
		csList.m_ullItemSeq = pcsAgsdItem->m_ullDBIID;
		csList.m_ullOrderNo = 0;
		csList.m_cStatus = AGSDITEM_CASHITEMBUYLIST_STATUS_GMGIVE;
		
		m_pagsmItem->WriteCashItemBuyList(&csList, pstItemOperation->m_szAccName, pstItemOperation->m_szCharName);
	}

	WriteLog_Item(AGPDLOGTYPE_ITEM_GM_MAKE, pcsAgpdCharacter,
				pstItemOperation->m_szAccName,
				pstItemOperation->m_szCharName,
				pcsAgpdItem,
				pcsAgpdItemTemplate->m_bStackable ? pstItemOperation->m_lCount : 1
				);
	
	// 다 끝나면 만든 아이템 지운다.
	m_pagpmItem->RemoveItem(pcsAgpdItem, FALSE);	// DB 에는 저장하지 않는다.

	return TRUE;
}

//BOOL AgsmAdmin::ProcessItemConvert(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
//{
//	if(!pstItemOperation || !pcsAgpdCharacter)
//		return FALSE;
//
//	INT32 lCID = pcsAgpdCharacter->m_lID;
//	INT32 dpnID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);
//
//	// 먼저 개조에 이용할 아이템을 생성한다.
//	AgpdItem* pcsAgpdConvertItem = m_pagsmItemManager->CreateItem(pstItemOperation->m_lTID, pcsAgpdCharacter);
//	if(!pcsAgpdConvertItem)
//		return FALSE;
//
//	// Inventory 에 있는 Item 을 가져온다.
//	AgpdItemADChar* pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);
//	if(!pcsAgpdItemADChar)
//		return FALSE;
//
//	AgpdGridItem* pcsAgpdGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csInventoryGrid, pstItemOperation->m_lLayer,
//																								pstItemOperation->m_lRow, pstItemOperation->m_lColumn);
//	if(!pcsAgpdGridItem)
//		return FALSE;
//
//	AgpdItem* pcsBaseItem = m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
//	if(!pcsBaseItem)
//		return FALSE;
//
//	// 로그 저장할 정보를 미리 빼놓는다.
//	CHAR szBaseItemName[AGPMITEM_MAX_ITEM_NAME+1], szConvertItemName[AGPMITEM_MAX_ITEM_NAME+1];
//	UINT64 llBaseItemDBID, llConvertItemDBID;
//
//	memset(szBaseItemName, 0, AGPMITEM_MAX_ITEM_NAME+1);
//	memset(szConvertItemName, 0, AGPMITEM_MAX_ITEM_NAME+1);
//	llBaseItemDBID = llConvertItemDBID = 0L;
//
//	AgpdItemTemplate* pcsBaseItemTemplate = (AgpdItemTemplate*)pcsBaseItem->m_pcsItemTemplate;
//	AgpdItemTemplate* pcsConvertItemTemplate = (AgpdItemTemplate*)pcsAgpdConvertItem->m_pcsItemTemplate;
//
//	AgsdItem* pcsAgsdBaseItem = m_pagsmItem->GetADItem(pcsBaseItem);
//	AgsdItem* pcsAgsdConvertItem = m_pagsmItem->GetADItem(pcsAgpdConvertItem);
//
//	if(pcsBaseItemTemplate && pcsConvertItemTemplate && pcsAgsdBaseItem && pcsAgsdConvertItem)
//	{
//		strncpy(szBaseItemName, pcsBaseItemTemplate->m_szName, AGPMITEM_MAX_ITEM_NAME);
//		strncpy(szConvertItemName, pcsConvertItemTemplate->m_szName, AGPMITEM_MAX_ITEM_NAME);
//
//		llBaseItemDBID = pcsAgsdBaseItem->m_ullDBIID;
//		llConvertItemDBID = pcsAgsdConvertItem->m_ullDBIID;
//	}
//	else	// 이거 못 얻으면 어디선가 에러난 것이기 때문에 걍 나간다.
//		return FALSE;
//	
//	// 개조!!!!!
//	BOOL bSuccess = m_pagpmItem->ConvertItem(pcsBaseItem, pcsAgpdConvertItem);
//	
//	INT32 lConvertLevel = pcsBaseItem->m_stConvertHistory.lConvertLevel;
//	
//	// 결과를 보낸다.
//	if(bSuccess)
//	{
//		SendItemOperationResult(pstItemOperation->m_lItemID, AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT, 1, lCID, dpnID);
//
//		// 로그를 남긴다.
//		m_pagpmItemLog->WriteConvertLog(llBaseItemDBID, szBaseItemName, lConvertLevel, llConvertItemDBID, szConvertItemName, pcsAgpdCharacter->m_szID, bSuccess);
//	}
//	else
//	{
//		// 만들었던 아이템을 지운다.
//		m_pagpmItem->RemoveItem(pcsAgpdConvertItem);
//
//		// 결과를 보낸다.
//		SendItemOperationResult(pstItemOperation->m_lItemID, AGPMADMIN_ITEM_RESULT_FAILED_CONVERT, 1, lCID, dpnID);
//
//		// 로그를 남긴다.
//		m_pagpmItemLog->WriteConvertLog(llBaseItemDBID, szBaseItemName, lConvertLevel, llConvertItemDBID, szConvertItemName, pcsAgpdCharacter->m_szID, bSuccess);
//	}
//
//	return TRUE;
//}

// 2005.03.04. steeple
// 얼마만이냐~ AgsmAdminItem~~
// 뒤에 넘어온 pcsAgpdCharacter 는 운영자임
BOOL AgsmAdmin::ProcessItemConvert(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	// 캐쉬 아이템은 레벨5 부터 가능이다.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstItemOperation->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstItemOperation->m_szCharName);

	if(!pcsCharacter)
	{
		return ProcessItemConvertToDB(pstItemOperation, pcsAgpdCharacter);
	}

	// 해당아이템을 찾는다.
	AgpdItem* pcsBaseItem = FindItemByPosition(pstItemOperation, pcsCharacter);
	if(!pcsBaseItem)
	{
		// 운영자가 아니면 풀어준다.
		if(strcmp(pcsCharacter->m_szID, pcsAgpdCharacter->m_szID) != 0)
			pcsCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemConvertADItem* pcsConvertADItem = m_pagpmItemConvert->GetADItem(pcsBaseItem);
	if(!pcsConvertADItem)
	{
		// 운영자가 아니면 풀어준다.
		if(strcmp(pcsCharacter->m_szID, pcsAgpdCharacter->m_szID) != 0)
			pcsCharacter->m_Mutex.Release();

		return FALSE;
	}

	// 무엇을 개조하는 지 보고 따로 처리해준다.
	if(pstItemOperation->m_cPhysicalConvertLevel != pcsConvertADItem->m_lPhysicalConvertLevel)
	{
		if(pstItemOperation->m_cPhysicalConvertLevel > 0)
		{
			if(ProcessItemConvertPhysical(pcsCharacter, pcsBaseItem, pstItemOperation->m_cPhysicalConvertLevel))
			{
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_PHY;

				WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM1, pcsAgpdCharacter, pcsCharacter, pcsBaseItem, 1);
			}
			else
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
		}
		else
		{
			// Physical Convert Init. 2005.06.08. steeple
            if(m_pagpmItemConvert->InitializePhysicalConvert(pcsBaseItem))
			{
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_PHY;

				// AgpmItemConvert 쪽의 콜백 불러준다.
				AgpdItemConvertResult eConvertResult = AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME;
				m_pagpmItemConvert->EnumCallback(AGPDITEMCONVERT_CB_PROCESS_PHYSICAL_CONVERT, pcsBaseItem, &eConvertResult);
				
				WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM1, pcsAgpdCharacter, pcsCharacter, pcsBaseItem, 1);
			}
			else
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
		}
	}
	else if(pstItemOperation->m_cSocket != pcsConvertADItem->m_lNumSocket)
	{
		if(pstItemOperation->m_cSocket > pcsConvertADItem->m_lNumSocket)
		{
			if(ProcessItemConvertSocket(pcsCharacter, pcsBaseItem, pstItemOperation->m_cSocket))
			{
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_SOCKET;
				WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM4, pcsAgpdCharacter, pcsCharacter, pcsBaseItem, 1);
			}
			else
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
		}
		else
		{
			// Socket Num Init. 2005.06.08. steeple
			if(m_pagpmItemConvert->InitializeSocket(pcsBaseItem))
			{
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_SOCKET;

				// AgpmItemConvert 쪽의 콜백 불러준다.
				AgpdItemConvertSocketResult	eCheckResult = AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME;
				m_pagpmItemConvert->EnumCallback(AGPDITEMCONVERT_CB_PROCESS_SOCKET_CONVERT, pcsBaseItem, &eCheckResult);
				
				WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM4, pcsAgpdCharacter, pcsCharacter, pcsBaseItem, 1);
			}
			else
				pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
		}
	}
	else if(pstItemOperation->m_lUsableItemTID != 0)
	{
		if(ProcessItemConvertAddItem(pcsCharacter, pcsBaseItem, pstItemOperation->m_lUsableItemTID, pcsAgpdCharacter))
			pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_ADDITEM;
		else
			pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
	}
	else
	{
		pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
	}

	// 2005.05.26. steeple
	// 성공했다면 해당 캐릭터에게 아이템 정보 새로 보내준다. Factor 를 새로 세팅해주기 위해서.
	if(pstItemOperation->m_cResult != (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT)
	{
		m_pagsmItem->SendPacketItem(pcsBaseItem, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	}

	pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
	SendItemOperationResult(pstItemOperation, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
		
	// 운영자가 아니면 풀어준다.
	if(strcmp(pcsCharacter->m_szID, pcsAgpdCharacter->m_szID) != 0)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessItemDelete(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	// 캐쉬 아이템은 레벨5 부터 가능이다.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstItemOperation->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstItemOperation->m_szCharName);

	if(!pcsCharacter)
	{
		// 2005.05.18. steeple
		ProcessItemDeleteToDB(pstItemOperation, pcsAgpdCharacter);
		return TRUE;
	}

	// 해당아이템을 찾는다.
	AgpdItem* pcsItem = FindItemByPosition(pstItemOperation, pcsCharacter);
	if(!pcsItem)
	{
		// 운영자가 아니면 풀어준다.
		if(pcsCharacter != pcsAgpdCharacter)
			pcsCharacter->m_Mutex.Release();

		return FALSE;
	}

	// 릴리즈 한후
	//m_pagpmItem->ReleaseItem(pcsItem);

	// 2006.01.06. steeple
	// 캐쉬아이템이고 사용중이라면 Unuse 먼저 해준다.
	if(IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType) &&
		pcsItem->m_nInUseItem && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
		m_pagsmItem->UnuseItem(pcsItem);

	// 로그
	WriteLog_Item(AGPDLOGTYPE_ITEM_GM_DESTROY, pcsAgpdCharacter, pcsCharacter, pcsItem,
					pcsItem->m_pcsItemTemplate->m_bStackable ? pcsItem->m_nCount : 1);

	// 지운다.
	ZeroMemory(pcsItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON  + 1));
	
	if(IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType) &&
		AGPMADMIN_ITEM_DELETE_REASON_REPAY == pstItemOperation->m_cReason)
		sprintf(pcsItem->m_szDeleteReason, "Drawback by Admin : %s", pcsAgpdCharacter->m_szID);
	else
		sprintf(pcsItem->m_szDeleteReason, "Deleted by Admin : %s", pcsAgpdCharacter->m_szID);
	m_pagpmItem->RemoveItem(pcsItem, TRUE);

	pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
	pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_DELETE;
	SendItemOperationResult(pstItemOperation, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	// 운영자가 아니면 풀어준다.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2005.05.18. steeple
// 접속해 있지 않은 유저의 아이템을 지워버린다.
BOOL AgsmAdmin::ProcessItemDeleteToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	// 걍 콜백 바로 불러준다.
	WriteLog_Item(AGPDLOGTYPE_ITEM_GM_DESTROY, pcsAgpdCharacter,
				pstItemOperation->m_szAccName,
				pstItemOperation->m_szCharName,
				pstItemOperation->m_ullDBID,
				NULL,
				NULL,
				NULL
				);
	
	EnumCallback(AGSMADMIN_CB_RELAY_ITEM_DELETE, pstItemOperation, pcsAgpdCharacter);
	return TRUE;
}

BOOL AgsmAdmin::ProcessItemConvertPhysical(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lPhysicalConvertLevel)
{
	if(!m_pagpmItemConvert)
		return FALSE;

	if(!pcsCharacter || !pcsItem || lPhysicalConvertLevel < 1)
		return FALSE;

    return m_pagpmItemConvert->SetPhysicalConvert(pcsItem, lPhysicalConvertLevel);
}

BOOL AgsmAdmin::ProcessItemConvertSocket(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lSocket)
{
	if(!m_pagpmItemConvert)
		return FALSE;

	if(!pcsCharacter || !pcsItem || lSocket < 1)
		return FALSE;
	
	return m_pagpmItemConvert->SetSocketConvert(pcsItem, lSocket);
}

BOOL AgsmAdmin::ProcessItemConvertAddItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lAddItemTID, AgpdCharacter *pcsAdmin)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pcsCharacter || !pcsItem || !lAddItemTID)
		return FALSE;

	// 만들 아이템이 맞는지 확인
	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(lAddItemTID);
	if(!pcsItemTemplate)
		return FALSE;

	if(pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
		(((AgpdItemTemplateUsable*)pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SPIRIT_STONE &&
		((AgpdItemTemplateUsable*)pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_RUNE))
		return FALSE;

	// 만들자
	AgpdItem* pcsAddItem = m_pagsmItemManager->CreateItem(lAddItemTID, pcsCharacter, 1);
	if(!pcsAddItem)
		return FALSE;

	pcsAddItem->m_eStatus = AGPDITEM_STATUS_INVENTORY;	// 강제 세팅~
	
	BOOL bResult = FALSE;
	// 정령석
	if(((AgpdItemTemplateUsable*)pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
	{
		//if(m_pagpmItemConvert->IsValidSpiritStone(pcsAddItem) == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS &&
		//	m_pagpmItemConvert->IsSpiritStoneConvertable(pcsItem) == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
		if(m_pagpmItemConvert->SpiritStoneConvert(pcsItem, pcsAddItem, TRUE) == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
		{
			//INT32 lItemRank	= 0;
			//m_pagpmFactors->GetValue(&pcsAddItem->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
			//if(lItemRank >= 1 && lItemRank <= 5)
			//{
				bResult = TRUE;
				AgpdItemConvertSpiritStoneResult eCheckResult = AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS;
				m_pagpmItemConvert->AddSpiritStoneConvert(pcsItem, pcsAddItem);

				WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM2, pcsAdmin, pcsCharacter, pcsItem, 1);
				
				m_pagpmItemConvert->EnumCallback(AGPDITEMCONVERT_CB_PROCESS_SPIRITSTONE_CONVERT, pcsItem, &eCheckResult);

				m_pagpmItem->SubItemStackCount(pcsAddItem, 1);
			//}
		}
	}
	// 기원석
	else if(((AgpdItemTemplateUsable*)pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
	{
		// 기원석을 바를 수 있는 지 체크
		//if(m_pagpmItemConvert->IsValidRuneItem(pcsAddItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS &&
		//	m_pagpmItemConvert->IsRuneConvertable(pcsItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS &&
		//	m_pagpmItemConvert->IsProperPart(pcsItem, pcsAddItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS &&
		//	m_pagpmItemConvert->IsProperLevel(pcsItem, pcsAddItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS &&
		//	m_pagpmItemConvert->IsAntiNumber(pcsItem, pcsAddItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		if(m_pagpmItemConvert->RuneConvert(pcsItem, pcsAddItem, TRUE) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		{
			bResult = TRUE;
			AgpdItemConvertRuneResult eResult = AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
			m_pagpmItemConvert->AddRuneConvert(pcsItem, pcsAddItem);

			WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM3, pcsAdmin, pcsCharacter, pcsItem, 1);

			m_pagpmItemConvert->EnumCallback(AGPDITEMCONVERT_CB_PROCESS_RUNE_CONVERT, pcsItem, &eResult);
			
			m_pagpmItem->SubItemStackCount(pcsAddItem, 1);
		}
	}

	// 제대로 하지 못했다면 만든 Usable Item 지워야 한다.
	if(!bResult)
		m_pagpmItem->RemoveItem(pcsAddItem, FALSE);	// DB 에는 저장하지 않는다.

	return bResult;
}

// 2005.05.31. steeple
// 이거이거.... 뒤에 넘어오는 놈은 Admin
// 아 짜다보니깐 코드 열라 지저분하다......................................
// 아웅..... 내 코드는 이런 게 아니야 ㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜ
BOOL AgsmAdmin::ProcessItemConvertToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmCharacter || !m_pagpmItem || !m_pagsmItem)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	// 캐릭터 접속중이면 안된다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(pstItemOperation->m_szCharName);
	if(pcsCharacter)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_stOriginalItem.m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	// 각 항목별 Validation 체크를 해줘야 한다.
	BOOL bValid = TRUE;
	eAGPDLOGTYPE_ITEM eLogType = AGPDLOGTYPE_ITEM_GM_ITSM1;

	if(pstItemOperation->m_cPhysicalConvertLevel >= 0 && pstItemOperation->m_cPhysicalConvertLevel != pstItemOperation->m_stOriginalItem.m_lPhysicalConvertLevel)
	{
		// 물리 강화
		eLogType = AGPDLOGTYPE_ITEM_GM_ITSM1;
		
		if(pcsItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP)
			bValid = FALSE;

		if(bValid && !(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
			(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR &&
			(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nPart == AGPMITEM_PART_BODY ||
			((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nPart == AGPMITEM_PART_LEGS))))
			bValid = FALSE;

		if(bValid && pstItemOperation->m_cPhysicalConvertLevel > m_pagpmItemConvert->GetPhysicalItemRank(pcsItemTemplate))
			bValid = FALSE;

		// 원래 아이템정보에다가 대입해 놓는다.
		if(bValid)
		{
			pstItemOperation->m_stOriginalItem.m_lPhysicalConvertLevel = pstItemOperation->m_cPhysicalConvertLevel;
			pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_PHY;
		}
	}
	else if(pstItemOperation->m_cSocket >= 0 && pstItemOperation->m_cSocket != pstItemOperation->m_stOriginalItem.m_lNumSocket)
	{
		// 소켓 뚫기
		// 0 으로 날라왔을 때는 1로 맞춰준다.
		eLogType = AGPDLOGTYPE_ITEM_GM_ITSM4;
		
		if(pstItemOperation->m_cSocket == 0)
			pstItemOperation->m_cSocket = (pcsItemTemplate->m_lMinSocketNum > 1) ? pcsItemTemplate->m_lMinSocketNum : 1;

		if(pcsItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP)
			bValid = FALSE;

		if(bValid)
		{
			if(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
			{
				if(pstItemOperation->m_cSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
					bValid = FALSE;
			}
			else if(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)
			{
				if(pstItemOperation->m_cSocket > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
					bValid = FALSE;
			}
			else if(((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RING ||
				((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE ||
				((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)
			{
				if(pstItemOperation->m_cSocket > AGPDITEMCONVERT_MAX_ETC_SOCKET)
					bValid = FALSE;
			}
			else
			{
				// 개조할 수 없는 아이템
				bValid = FALSE;
			}
		}

		// 원래 아이템정보에다가 대입해 놓는다.
		if(bValid)
		{
			pstItemOperation->m_stOriginalItem.m_lNumSocket = pstItemOperation->m_cSocket;
			pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_SOCKET;
		}
	}
	else if(pstItemOperation->m_lUsableItemTID != 0)
	{
		// 정령석, 기원석 박기
		// 여기선 간단한 체크만 하자.............
		// 랭크 검사는 무리임.... 조심히 개조한다.

		// 먼저 소켓 개수
		if(pstItemOperation->m_stOriginalItem.m_lNumConvertedSocket >= pstItemOperation->m_stOriginalItem.m_lNumSocket)
			bValid = FALSE;

		AgpdItemTemplate* pcsAddItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lUsableItemTID);
		if(!pcsAddItemTemplate)
			bValid = FALSE;

		if(bValid && pcsAddItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
			bValid = FALSE;

		if(bValid)
		{
			if(((AgpdItemTemplateUsable*)pcsAddItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
			{
				eLogType = AGPDLOGTYPE_ITEM_GM_ITSM2;
				// 정령석일 경우
				if(((AgpdItemTemplateUsableSpiritStone*)pcsAddItemTemplate)->m_eSpiritStoneType <= AGPMITEM_USABLE_SS_TYPE_NONE ||
					((AgpdItemTemplateUsableSpiritStone*)pcsAddItemTemplate)->m_eSpiritStoneType > AGPMITEM_USABLE_SS_TYPE_AIR)
				{
					bValid = FALSE;
				}
			}
			else if(((AgpdItemTemplateUsable*)pcsAddItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
			{
				eLogType = AGPDLOGTYPE_ITEM_GM_ITSM3;
				// 기원석
				// 랭크랑 이런저런 거 체크해야 하는데... 하기가 난감하군.. -0-;;
			}
		}

		if(bValid)
			pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_ADDITEM;
	}

	if(bValid)
	{
		// 대략의 검사를 통과했다면
		CHAR szConvertHistory[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
		memset(szConvertHistory, 0, sizeof(szConvertHistory));

		// 인코딩 내맘대로 한다.
		sprintf(szConvertHistory, "%d:%d:", pstItemOperation->m_stOriginalItem.m_lPhysicalConvertLevel,
											pstItemOperation->m_stOriginalItem.m_lNumSocket);

		for(INT32 i = 0; i < pstItemOperation->m_stOriginalItem.m_lNumConvertedSocket; i++)
			sprintf(szConvertHistory + strlen(szConvertHistory), "%d,", pstItemOperation->m_stOriginalItem.m_stSocketAttr[i].m_lTID);

		if(pstItemOperation->m_lUsableItemTID != 0)
			sprintf(szConvertHistory + strlen(szConvertHistory), "%d,", pstItemOperation->m_lUsableItemTID);

		sprintf(szConvertHistory + strlen(szConvertHistory) - 1, ":");

		WriteLog_Item(eLogType, pcsAgpdCharacter,
					pstItemOperation->m_szAccName,
					pstItemOperation->m_szCharName,
					pstItemOperation->m_ullDBID,
					szConvertHistory,
					NULL,
					NULL
					);

		EnumCallback(AGSMADMIN_CB_RELAY_ITEM_CONVERT_UPDATE2, szConvertHistory, &pstItemOperation->m_stOriginalItem.m_ullDBID);
	}
	else
	{
		pstItemOperation->m_cResult = (INT8)AGPMADMIN_ITEM_RESULT_FAILED_CONVERT;
	}

	pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
	SendItemOperationResult(pstItemOperation, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

// 2005.04.11. steeple
BOOL AgsmAdmin::ProcessItemOptionAdd(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	// 캐쉬 아이템은 레벨5 부터 가능이다.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstItemOperation->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstItemOperation->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	// 해당아이템을 찾는다.
	AgpdItem* pcsItem = FindItemByPosition(pstItemOperation, pcsCharacter);
	if(!pcsItem)
	{
		// 운영자가 아니면 풀어준다.
		if(pcsCharacter != pcsAgpdCharacter)
			pcsCharacter->m_Mutex.Release();

		return FALSE;
	}

	INT32 lAddedCount = 0;
	// 그냥 루프 돌면서 Add 해준다.
	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if(m_pagpmItem->AddItemOption(pcsItem, pstItemOperation->m_alOptionTID[i], FALSE))
			lAddedCount++;
	}

	pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
	pstItemOperation->m_cResult = lAddedCount > 0 ? (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_OPTINO_ADD :
													(INT8)AGPMADMIN_ITEM_RESULT_FAILED_OPTION_ADD;
	SendItemOperationResult(pstItemOperation, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	if(pstItemOperation->m_cResult == (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_OPTINO_ADD)
	{
		WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM5, pcsAgpdCharacter, pcsCharacter, pcsItem, 1);
	
		m_pagsmItem->SendPacketItem(pcsItem, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	}

	// 운영자가 아니면 풀어준다.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2005.04.11. steeple
BOOL AgsmAdmin::ProcessItemOptionRemove(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	// 캐쉬 아이템은 레벨5 부터 가능이다.
	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstItemOperation->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstItemOperation->m_szCharName);

	if(!pcsCharacter)
		return FALSE;

	// 해당아이템을 찾는다.
	AgpdItem* pcsItem = FindItemByPosition(pstItemOperation, pcsCharacter);
	if(!pcsItem)
	{
		// 운영자가 아니면 풀어준다.
		if(pcsCharacter != pcsAgpdCharacter)
			pcsCharacter->m_Mutex.Release();

		return FALSE;
	}

	// 남게 되는 놈
	ApSafeArray<INT32, AGPDITEM_OPTION_MAX_NUM> alNewOptionTID;
	alNewOptionTID.MemSetAll();

	INT32 lAlreadyCount = 0, lNewCount = 0;
	BOOL bRemoveTID = FALSE;

	// 모든 것을 일단 Remove 한다. 그러면서 Factor 값을 다시 계산해주어야 한다.
	// Remove 될 꺼는 빼고 남을 것을 저장해준다.
	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if(pcsItem->m_aunOptionTID[i] == 0)
			break;

		lAlreadyCount++;	// 기존에 몇개 있는 지 저장
		bRemoveTID = FALSE;
		for(INT32 j = 0; j < AGPDITEM_OPTION_MAX_NUM; j++)
		{
			if(pstItemOperation->m_alOptionTID[j] == 0)
				break;
			 
			if(pcsItem->m_aunOptionTID[i] == pstItemOperation->m_alOptionTID[j])
			{
				bRemoveTID = TRUE;
				break;
			}
		}

		if(!bRemoveTID)	// 안지워 지는 놈이면 남게 되는 놈에 넣어준다.
			alNewOptionTID[lNewCount++] = pcsItem->m_aunOptionTID[i];

		// 일단 원래 붙어있던 놈 Remove
		m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItem->m_apcsOptionTemplate[i]->m_csFactor, TRUE, FALSE, FALSE);
		m_pagpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItem->m_apcsOptionTemplate[i]->m_csFactorPercent, TRUE, FALSE, FALSE);

		pcsItem->m_aunOptionTID[i] = 0;
		pcsItem->m_apcsOptionTemplate[i] = NULL;
	}

	for(INT32 i = 0; i < lNewCount; i++)
		m_pagpmItem->AddItemOption(pcsItem, alNewOptionTID[i], FALSE);

	pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
	pstItemOperation->m_cResult = lNewCount < lAlreadyCount ? (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_OPTION_REMOVE :
																(INT8)AGPMADMIN_ITEM_RESULT_FAILED_OPTION_REMOVE;
	SendItemOperationResult(pstItemOperation, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	if(pstItemOperation->m_cResult == (INT8)AGPMADMIN_ITEM_RESULT_SUCCESS_OPTION_REMOVE)
	{
		WriteLog_Item(AGPDLOGTYPE_ITEM_GM_ITSM5, pcsAgpdCharacter, pcsCharacter, pcsItem, 1);
		
		m_pagsmItem->SendPacketItem(pcsItem, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	}

	// 운영자가 아니면 풀어준다.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2006.02.08. steeple
BOOL AgsmAdmin::ProcessItemUpdate(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagsmCharacter || !m_pagsmItem)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	// 먼저 템플릿을 구한다.
	AgpdItemTemplate* pcsAgpdItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsAgpdItemTemplate)
		return FALSE;

	// 캐쉬 아이템은 레벨5 부터 가능이다.
	if(IS_CASH_ITEM(pcsAgpdItemTemplate->m_eCashItemType) && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstItemOperation->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstItemOperation->m_szCharName);

	if(!pcsCharacter)
	{
		// 서버에 없는 캐릭에 넣는 거라면 다른 함수 호출해준다.
		ProcessItemUpdateToDB(pstItemOperation, pcsAgpdCharacter);
		return TRUE;
	}

	// 아이템을 얻는다.
	AgpdItem* pcsItem = m_pagpmItem->GetItem(pstItemOperation->m_lID);
	if(!pcsItem || pcsItem->m_pcsCharacter != pcsCharacter)
	{
		if(pcsCharacter != pcsAgpdCharacter)
			pcsCharacter->m_Mutex.Release();
		
		return FALSE;
	}

	// 현재는 Durability 업데이트 뿐이다. 2006.02.08. steeple
	INT32 lMaxDurability = m_pagpmItem->GetItemDurabilityMax(pcsItem->m_pcsItemTemplate);
	if(lMaxDurability > 0 && pstItemOperation->m_lDurability >= 0 && pstItemOperation->m_lDurability <= lMaxDurability)
	{
		INT32 lDiffDurability = pstItemOperation->m_lDurability - pstItemOperation->m_stOriginalItem.m_lDurability;
		if(m_pagsmItem->UpdateItemDurability(pcsItem, lDiffDurability, pstItemOperation->m_lMaxDurability))
		{
			pstItemOperation->m_cResult = AGPMADMIN_ITEM_RESULT_SUCCESS_UPDATE;
		}
		else
		{
			pstItemOperation->m_cResult = AGPMADMIN_ITEM_RESULT_FAILED_UPDATE;
		}
	}
	else
		pstItemOperation->m_cResult = AGPMADMIN_ITEM_RESULT_FAILED_UPDATE;

	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	pstItemOperation->m_cOperation = AGPMADMIN_ITEM_RESULT;
	SendItemOperationResult(pstItemOperation, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

BOOL AgsmAdmin::ProcessItemUpdateToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmItem || !m_pagsmCharacter || !m_pagsmItem)
		return FALSE;

	if(!pstItemOperation || !pcsAgpdCharacter)
		return FALSE;

	// 캐릭터 접속중이면 안된다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(pstItemOperation->m_szCharName);
	if(pcsCharacter)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItemOperation->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	// Max 치 초과하지 않게 한다.
	if(pstItemOperation->m_lDurability > m_pagpmItem->GetItemDurabilityMax(pcsItemTemplate) ||
		pstItemOperation->m_lMaxDurability > m_pagpmItem->GetItemDurabilityMax(pcsItemTemplate) ||
		m_pagpmItem->GetItemDurabilityMax(pcsItemTemplate) <= 0)
		return FALSE;
	
	// 걍 콜백 바로 불러준다.
	//WriteLog_Item(AGPDLOGTYPE_ITEM_GM_DESTROY, pcsAgpdCharacter,
	//			pstItemOperation->m_szAccName,
	//			pstItemOperation->m_szCharName,
	//			pstItemOperation->m_ullDBID,
	//			NULL,
	//			NULL
	//			);

	EnumCallback(AGSMADMIN_CB_RELAY_ITEM_UPDATE, pstItemOperation, pcsAgpdCharacter);
	return TRUE;
}

// 2005.04.11. steeple
// 유후~
AgpdItem* AgsmAdmin::FindItemByPosition(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsCharacter)
{
	if(!m_pagpmItem || !m_pagpmItemConvert || /*!m_pagpmItemLog || */!m_pagsmCharacter || !m_pagsmItem || !m_pagsmItemManager)
		return FALSE;

	if(!pstItemOperation || !pcsCharacter)
		return NULL;

	AgpdItem* pcsItem = NULL;
	switch(pstItemOperation->m_lPos)
	{
		case AGPDITEM_STATUS_EQUIP:
		{
			pcsItem = m_pagpmItem->GetEquipSlotItem(pcsCharacter, (AgpmItemPart)pstItemOperation->m_nPart);
			break;
		}

		case AGPDITEM_STATUS_INVENTORY:
		{
			AgpdItemADChar* pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			if(pcsAgpdItemADChar)
			{
				AgpdGridItem* pcsAgpdGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csInventoryGrid,
																pstItemOperation->m_lLayer,
																pstItemOperation->m_lRow,
																pstItemOperation->m_lColumn);
				if(pcsAgpdGridItem)
					pcsItem = m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			}
			break;
		}

		case AGPDITEM_STATUS_CASH_INVENTORY:
		{
			AgpdItemADChar* pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			if(pcsAgpdItemADChar)
			{
				AgpdGridItem* pcsAgpdGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csCashInventoryGrid,
																pstItemOperation->m_lLayer,
																pstItemOperation->m_lRow,
																pstItemOperation->m_lColumn);
				if(pcsAgpdGridItem)
					pcsItem = m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			}
			break;
		}

		case AGPDITEM_STATUS_SUB_INVENTORY:
		{
			AgpdItemADChar* pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			if(pcsAgpdItemADChar)
			{
				AgpdGridItem* pcsAgpdGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csSubInventoryGrid,
																pstItemOperation->m_lLayer,
																pstItemOperation->m_lRow,
																pstItemOperation->m_lColumn);
				if(pcsAgpdGridItem)
					pcsItem = m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			}
			break;
		}

		case AGPDITEM_STATUS_BANK:
		{
			AgpdItemADChar* pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			if(pcsAgpdItemADChar)
			{
				AgpdGridItem* pcsAgpdGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csBankGrid,
																pstItemOperation->m_lLayer,
																pstItemOperation->m_lRow,
																pstItemOperation->m_lColumn);
				if(pcsAgpdGridItem)
					pcsItem = m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			}
			break;
		}
	}

	return pcsItem;
}

// 2005.05.18. steeple
// Convert Item Data. AlefAdminDLL 에 있는 것을 가져옴.
BOOL AgsmAdmin::ConvertItemData(AgpdItem* pcsItem, stAgpdAdminItem* pstItem)
{
	if(!pcsItem || !pcsItem->m_pcsCharacter || !pstItem)
		return FALSE;

	if(!m_pagpmItemConvert || !m_pagsmItem)
		return FALSE;

	AgpdItemTemplate* pcsItemTemplate = (AgpdItemTemplate*)pcsItem->m_pcsItemTemplate;
	if(!pcsItemTemplate)
		return FALSE;

	AgsdItem* pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if(!pcsAgsdItem)
		return FALSE;

	// 먼저 초기화
	memset(pstItem, 0, sizeof(stAgpdAdminItem));

	strncpy(pstItem->m_szCharName, pcsItem->m_pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

	// 2005.09.14. steeple
	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsItem->m_pcsCharacter);
	if(pcsAgsdCharacter)
	{
		strncpy(pstItem->m_szAccountName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
		strncpy(pstItem->m_szServerName, pcsAgsdCharacter->m_szServerName, AGPMADMIN_MAX_SERVERNAME_LENGTH);
	}

	pstItem->m_lTID = pcsItemTemplate->m_lID;
	//pstItem->m_pcsTemplate = NULL;			// 서버와 클라이언트 포인터가 다르므로 얘는 NULL 로 준다.

	pstItem->m_ullDBID = pcsAgsdItem->m_ullDBIID;
	pstItem->m_lID = pcsItem->m_lID;

	pstItem->m_lPos = (INT32)pcsItem->m_eStatus;
	pstItem->m_lLayer = (INT32)pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	pstItem->m_lRow = (INT32)pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	pstItem->m_lCol = (INT32)pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];
	pstItem->m_nPart = (INT16)((AgpdItemTemplateEquip*)pcsItemTemplate)->m_nPart;
	pstItem->m_lCount = (INT32)pcsItem->m_nCount;

	pstItem->m_lPhysicalConvertLevel = m_pagpmItemConvert->GetNumPhysicalConvert(pcsItem);
	pstItem->m_lNumConvertedSocket = m_pagpmItemConvert->GetNumConvertedSocket(pcsItem);
	pstItem->m_lNumSocket = m_pagpmItemConvert->GetNumSocket(pcsItem);

	AgpdItemConvertADItem* pcsAttachedConvert = m_pagpmItemConvert->GetADItem(pcsItem);
	if(pcsAttachedConvert)
	{
		for(int i = 0; i < pcsAttachedConvert->m_lNumSocket; i++)
		{
			if(pcsAttachedConvert->m_lNumConvert > i && pcsAttachedConvert->m_stSocketAttr[i].pcsItemTemplate)
			{
				pstItem->m_stSocketAttr[i].m_bIsSpiritStone = pcsAttachedConvert->m_stSocketAttr[i].bIsSpiritStone;
				pstItem->m_stSocketAttr[i].m_lTID = pcsAttachedConvert->m_stSocketAttr[i].lTID;
				//pstItem->m_stSocketAttr[i].m_pcsItemTemplate = NULL;	// 서버와 클라이언트 포인터가 다르므로 얘는 NULL 로 준다.
			}
		}
	}

	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if(pcsItem->m_aunOptionTID[i] == 0)
			break;

		pstItem->m_alOptionTID[i] = pcsItem->m_aunOptionTID[i];
	}

	// 2007.03.19. steeple
	for(INT32 i = 0; i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; ++i)
	{
		if(pcsItem->m_aunSkillPlusTID[i] == 0)
			break;

		pstItem->m_alSkillPlusTID[i] = pcsItem->m_aunSkillPlusTID[i];
	}

	// 2006.02.08. steeple
	pstItem->m_lDurability = m_pagpmItem->GetItemDurabilityCurrent(pcsItem);
	pstItem->m_lMaxDurability = m_pagpmItem->GetItemDurabilityMax(pcsItem);

	// 2005.12.29. steeple
	pstItem->m_nInUseItem = pcsItem->m_nInUseItem;
	pstItem->m_lRemainTime = pcsItem->m_lRemainTime;
	pstItem->m_lExpireTime = pcsItem->m_lExpireTime;
	pstItem->m_lCashItemUseCount = pcsItem->m_lCashItemUseCount;
	pstItem->m_llStaminaRemainTime = pcsItem->m_llStaminaRemainTime;

	return TRUE;
}







//////////////////////////////////////////////////////////////////////////
// Send Item Operation Result
BOOL AgsmAdmin::SendItemOperationResult(INT32 lTID, eAgpmAdminItemOperationResult eResult, INT32 lCount, INT32 lCID, UINT32 ulNID)
{
	stAgpdAdminItemOperation stItemOperationResult;
	memset(&stItemOperationResult, 0, sizeof(stItemOperationResult));

	stItemOperationResult.m_cOperation = AGPMADMIN_ITEM_RESULT;
	stItemOperationResult.m_lTID = lTID;
	stItemOperationResult.m_cResult = (INT8)eResult;
	stItemOperationResult.m_lCount = lCount;

	return SendItemOperationResult(&stItemOperationResult, lCID, ulNID);
}

BOOL AgsmAdmin::SendItemOperationResult(stAgpdAdminItemOperation* pstItemOperation, INT32 lCID, UINT32 ulNID)
{
	if(!pstItemOperation || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvItemPacket = NULL;
	INT16 nPacketLength = 0;
	INT16 nLength = sizeof(stAgpdAdminItemOperation);
	INT8 cOperation = AGPMADMIN_PACKET_ITEM;

	pvItemPacket = m_pagpmAdmin->MakeItemPacket(&nPacketLength, pstItemOperation, nLength);
	if(!pvItemPacket)
	{
		OutputDebugString("AgsmAdmin::SendItemOperationResult(...) pvItemPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeItemPacket(&nPacketLength, &lCID, pvItemPacket);

	m_pagpmAdmin->m_csItemPacket.FreePacket(pvItemPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendItemOperationResult(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}










//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmAdmin::CBItemOperation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminItemOperation* pstItemOperation = (stAgpdAdminItemOperation*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pCustData;

	if(!pstItemOperation || !pClass || !pcsAgpdCharacter)
		return FALSE;

	return pThis->ProcessItemOperation(pstItemOperation, pcsAgpdCharacter);
}
