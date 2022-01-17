/******************************************************************************
Module:  AgsmItemManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 10. 04
******************************************************************************/

#include "AgsmItemManager.h"
#include "AuTimeStamp.h"
#include <AgsmEventSystem.h>

AgsmItemManager::AgsmItemManager()
{
	SetModuleName("AgsmItemManager");

	SetModuleType(APMODULE_TYPE_SERVER);

	SetModuleData(sizeof(AgpdItem), AGSMITEMMANAGER_DATA_TYPE_ITEM);

	m_lTemplateIDSkull	= AP_INVALID_IID;

	m_pagsmInterServerLink = NULL;
	m_pagsmServerManager = NULL;
	m_pagsmEventSystem = NULL;
}

AgsmItemManager::~AgsmItemManager()
{
}

BOOL AgsmItemManager::OnAddModule()
{
	m_pagpmGrid = (AgpmGrid *) GetModule("AgpmGrid");

	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem	= (AgpmItem *) GetModule("AgpmItem");
	m_pagpmItemConvert = (AgpmItemConvert *) GetModule("AgpmItemConvert");
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmDropItem2 = (AgpmDropItem2 *) GetModule("AgpmDropItem2");
	m_pagpmSystemMessage = (AgpmSystemMessage*)GetModule("AgpmSystemMessage");

	m_pagsmFactors = (AgsmFactors *) GetModule("AgsmFactors");
	m_pagsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmCharManager = (AgsmCharManager *) GetModule("AgsmCharManager");
	m_pagsmItem = (AgsmItem *) GetModule("AgsmItem");
	m_pagsmItemConvert = (AgsmItemConvert *) GetModule("AgsmItemConvert");
	

	if (!m_pagpmCharacter || !m_pagpmItem || !m_pagpmFactors || !m_pagsmFactors ||
		!m_pagsmCharacter || !m_pagsmCharManager || !m_pagsmItem || !m_pagpmItemConvert || !m_pagsmItemConvert || !m_pagpmDropItem2)
		return FALSE;

	if (!m_pagsmCharManager->SetCallbackCreateChar(CBCreateCharacter, this))
		return FALSE;
	if (!m_pagsmCharManager->SetCallbackUnLoadChar(CBUnLoadCharacter, this))
		return FALSE;
	if (!m_pagsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;

	if (!m_pagsmItem->SetCallbackGetNewIID(CBGetNewIID, this))
		return FALSE;
	if (!m_pagsmItem->SetCallbackGetNewDBID(CBGetNewDBID, this))
		return FALSE;
	if (!m_pagsmItem->SetCallbackGetNewCashItemBuyID(CBGetNewCashItemBuyID, this))
		return FALSE;

	if (!m_pagsmItem->SetCallbackCreateNewItem(CBCreateNewItem, this))
		return FALSE;

	if (!m_pagpmItem->SetCallbackDelete(CBDeleteItem, this))
		return FALSE;
	if (!m_pagpmItem->SetCallbackRemoveID(CBRemoveItemID, this))
		return FALSE;

	if (!m_pagpmItem->SetCallbackSplitItem(CBSplitItem, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmItemManager::OnInit()
{
	m_pagsmInterServerLink = (AgsmInterServerLink*)GetModule("AgsmInterServerLink");
	m_pagsmServerManager = (AgsmServerManager2*)GetModule("AgsmServerManager2");
	m_pagsmEventSystem = (AgsmEventSystem*)GetModule("AgsmEventSystem");

	if( !m_pagsmInterServerLink ||
		!m_pagsmServerManager
		/*|| !m_pagsmEventSystem*/)
		return FALSE;

	if( !m_pagpmItem->SetCallback(ITEM_CB_ID_GET_EXTRADATA, CBGetItemExtraData, this) ||
		!m_pagsmInterServerLink->SetCallback(AGSMINTERSERVERLINK_CB_CONNECT, CBRelayConnect, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmItemManager::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize)
{
	return m_csGenerateID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag, lRemoveIDQueueSize, TRUE);
}

BOOL AgsmItemManager::InitItemDBIDServer(INT32 ServerIndex)
{
	return m_csGenerateID64.Initialize(ServerIndex);
}

AgpdItem* AgsmItemManager::CreateItem(INT32 lTID, INT32 lCID, INT32 lCount, BOOL bGenerateDBID, INT32 lSkillTID, BOOL bOptionAdd)
{
	AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	return CreateItem(lTID, pcsCharacter, lCount, bGenerateDBID, lSkillTID, bOptionAdd);
}

AgpdItem* AgsmItemManager::CreateItem(INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lCount, BOOL bGenerateDBID, INT32 lSkillTID, BOOL bOptionAdd)
{
	//STOPWATCH2(GetModuleName(), _T("CreateItem"));

	AgpdItem *pcsItem = m_pagpmItem->AddItem(m_csGenerateID.GetID(), lTID, lCount );

	if (!pcsItem)
		return NULL;

	if( pcsCharacter != NULL )
	{
		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pcsAgsdCharacter)
			return NULL;

		AgsdItem	*pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
		if(!pcsAgsdItem)
			return NULL;

		pcsItem->m_ulCID = pcsCharacter->m_lID;
		pcsItem->m_pcsCharacter = pcsCharacter;

		if (bGenerateDBID && pcsAgsdCharacter->m_dpnidCharacter != 0)
		{
			pcsAgsdItem->m_ullDBIID = (UINT64) GetDBID();

			// 2005.05.10. steeple
			if(!m_pagsmItem->AddDBIDAdmin(pcsAgsdItem->m_ullDBIID))
			{
				pcsAgsdItem->m_ullDBIID = 0;
				m_pagpmItem->RemoveItem(pcsItem, FALSE);
				return NULL;
			}
		}
	}
	else
	{
		pcsItem->m_ulCID = 0;
		pcsItem->m_pcsCharacter = NULL;
		pcsItem->m_eStatus = AGPDITEM_STATUS_NONE;
		AgsdItem	*pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
		if (bGenerateDBID)
		{
			pcsAgsdItem->m_ullDBIID = (UINT64) GetDBID();

			// 2005.05.10. steeple
			if(!m_pagsmItem->AddDBIDAdmin(pcsAgsdItem->m_ullDBIID))
			{
				pcsAgsdItem->m_ullDBIID = 0;
				m_pagpmItem->RemoveItem(pcsItem, FALSE);
				return NULL;
			}
		}
	}

	AgsdItem	*pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem)
	{
		// 2007.08.13. steeple
		// 아바타 아이템에서 Parents 에 의해서 생기는 아이템은 DB 저장하지 않는다.
		if(!m_pagpmItem->IsAvatarItem(pcsItem->m_pcsItemTemplate) || bGenerateDBID)
			pcsAgsdItem->m_bIsNeedInsertDB = TRUE;
	}

	pcsItem->m_lSkillTID	= lSkillTID;

	pcsItem->m_nInUseItem	= AGPDITEM_CASH_ITEM_UNUSE;
	pcsItem->m_lRemainTime	= pcsItem->m_pcsItemTemplate->m_lRemainTime;
	pcsItem->m_lExpireTime	= 0;
	pcsItem->m_llStaminaRemainTime = 0;

	m_pagpmItem->InitItem(pcsItem);

	if(bOptionAdd)
	{
		for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
		{
			if(!pcsItem->m_pcsItemTemplate)
				break;

			if(pcsItem->m_pcsItemTemplate->m_alOptionTID[i] == 0)
				break;

			m_pagpmItem->AddItemOption(pcsItem, pcsItem->m_pcsItemTemplate->m_alOptionTID[i], FALSE);
		}
	}

	return pcsItem;
}

AgpdItem* AgsmItemManager::CreateItem(INT32 lTID, INT32 lItemCount, CHAR *pszPosition, INT16 nStatus, CHAR *pszConvert,
									INT32 lDurability, INT32 lMaxDurability, INT32 lStatusFlag, CHAR *pszOption, CHAR *pszSkillPlus,
									INT16 nInUse, INT32 lUseCount, INT64 lRemainTime, UINT32 lExpireTime, INT64 llStaminaRemainTime,
									UINT64 ullDBID, AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("CreateItem2"));

	// verify item template
	AgpdItemTemplate *pcsTemplate = m_pagpmItem->GetItemTemplate(lTID);
	if (NULL == pcsTemplate)
		return NULL;
	
	AgpdItem *pcsItem = CreateItem(pcsTemplate->m_lID, NULL, lItemCount, FALSE);
	if (NULL == pcsItem)
		return NULL;

	// 2005.05.10. steeple
	if(!m_pagsmItem->AddDBIDAdmin(ullDBID))
	{
		m_pagpmItem->RemoveItem(pcsItem, FALSE);
		return NULL;
	}
	
	// owner
	pcsItem->m_pcsCharacter	= pcsCharacter;
	pcsItem->m_ulCID = pcsCharacter ? pcsCharacter->m_lID : AP_INVALID_CID;

	// position
	if (NULL != pszPosition && 0 < _tcslen(pszPosition))
	{
		m_pagsmCharacter->DecodingPosition(pszPosition, &pcsItem->m_posItem);

		pcsItem->m_anGridPos[0]		= (INT32) pcsItem->m_posItem.x;
		pcsItem->m_anGridPos[1]		= (INT32) pcsItem->m_posItem.y;
		pcsItem->m_anGridPos[2]		= (INT32) pcsItem->m_posItem.z;
	}

	// item DBID
	AgsdItem *pcsAgsdItem	= m_pagsmItem->GetADItem(pcsItem);
	
	pcsAgsdItem->m_bIsNeedInsertDB	= FALSE;
	
	pcsAgsdItem->m_ullDBIID			= ullDBID;

	// status
	pcsItem->m_eStatus				= nStatus;

	if (NULL != pszConvert && 0 < _tcslen(pszConvert))
	{
		if (!m_pagsmItemConvert->DecodeConvertHistory(pcsItem, pszConvert, (INT32)_tcslen(pszConvert)))
		{
			//ASSERT(!"AgsmItemManager::GetSelectResult3() failed ParseStringItemConvertHistory()");
		}

		m_pagpmItemConvert->CalcConvertFactor(pcsItem);
	}

	// durability
	INT32	lMaxItemDurability		= 0;
	m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lMaxItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);	
	
	if (lMaxDurability == (-1000) &&
		lMaxItemDurability > 0)
		lMaxDurability	= lMaxItemDurability;
	else if (lMaxDurability > lMaxItemDurability)
		lMaxDurability	= lMaxItemDurability;

	if (lDurability > lMaxDurability ||
		lDurability == (-1000))
		lDurability = lMaxDurability;
	else if (lDurability < 0)
		lDurability = 0;

	m_pagpmFactors->SetValue(&pcsItem->m_csFactor, lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	m_pagpmFactors->SetValue(&pcsItem->m_csFactor, lMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

	// status flag
	pcsItem->m_lStatusFlag	= lStatusFlag;
	
	// option
	m_pagsmItem->DecodingOption(pcsItem, pszOption, (INT32)_tcslen(pszOption));
	
	// skill plus
	m_pagsmItem->DecodingSkillPlus(pcsItem, pszSkillPlus, (INT32)_tcslen(pszSkillPlus));

	// in use?
	pcsItem->m_nInUseItem = (INT8)nInUse;
	
	// use count
	pcsItem->m_lCashItemUseCount = lUseCount;
	
	// remain time
	pcsItem->m_lRemainTime = lRemainTime;
	
	// expire date
	pcsItem->m_lExpireTime = lExpireTime;

	// stamina remain time
	pcsItem->m_llStaminaRemainTime = llStaminaRemainTime;

	return pcsItem;
}

AgpdItem* AgsmItemManager::CreateItemSkull()
{
	if (m_lTemplateIDSkull == AP_INVALID_IID)
	{
		for(AgpaItemTemplate::iterator it = m_pagpmItem->csTemplateAdmin.begin(); it != m_pagpmItem->csTemplateAdmin.end(); ++it)
		{
			AgpdItemTemplate* pcsTemplate = it->second;

			if (pcsTemplate->m_nType == AGPMITEM_TYPE_OTHER)
			{
				AgpdItemTemplateOther	*pcsTemplateOther = (AgpdItemTemplateOther *) pcsTemplate;
				
				if (pcsTemplateOther->m_eOtherItemType == AGPMITEM_OTHER_TYPE_SKULL)
				{
					m_lTemplateIDSkull = pcsTemplate->m_lID;
					break;
				}
			}
		}

		if (m_lTemplateIDSkull == AP_INVALID_IID)
			return NULL;
	}

	AgpdItem *pcsItemSkull = CreateItem(m_lTemplateIDSkull);

	if (pcsItemSkull)
	{
		pcsItemSkull->m_pstSkullInfo = (AgpmItemSkullInfo *) GlobalAlloc(GMEM_FIXED, sizeof(AgpmItemSkullInfo));
		ZeroMemory(pcsItemSkull->m_pstSkullInfo, sizeof(AgpmItemSkullInfo));
	}

	return pcsItemSkull;
}

AgpdItem* AgsmItemManager::CreateItemWithOutOption(INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lCount, BOOL bGenerateDBID, INT32 lSkillTID)
{
	return CreateItem(lTID, pcsCharacter, lCount, bGenerateDBID, lSkillTID, FALSE);
}

BOOL AgsmItemManager::CBCreateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpdCharacter	*pcsCharacter = (AgpdCharacter *) pData;
	AgsmItemManager	*pThis = (AgsmItemManager *) pClass;
	AgpdItemADChar	*pstItemADChar = pThis->m_pagpmItem->GetADCharacter(pcsCharacter);

	AgpdItemADCharTemplate* pcsADCharTemplate 
		= pThis->m_pagpmItem->GetADCharacterTemplate((AgpdCharacterTemplate *) pcsCharacter->m_pcsCharacterTemplate);
	if (!pcsADCharTemplate)
		return FALSE;

	for (int i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		if (!pcsADCharTemplate->m_lDefaultEquipITID[i])
			continue;

		AgpdItem *pcsItem = pThis->CreateItem(pcsADCharTemplate->m_lDefaultEquipITID[i], pcsCharacter);
		if (!pcsItem)
			continue;

		if (!pThis->m_pagpmItem->EquipItem(pcsCharacter, pcsItem, TRUE, TRUE))
		{
			pThis->m_pagpmItem->RemoveItem(pcsItem->m_lID);

			continue;
		}
	}

	for (int i = 0; i < AGPMITEM_DEFAULT_INVEN_NUM; i++)
	{
		if (pcsADCharTemplate->m_aalDefaultInvenInfo[i][AGPDITEM_DEFAULT_INVEN_INDEX_TID] == AP_INVALID_IID)
			break;

		INT32	lCreateItemNum	= 1;
		AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pagpmItem->GetItemTemplate(pcsADCharTemplate->m_aalDefaultInvenInfo[i][AGPDITEM_DEFAULT_INVEN_INDEX_TID]);
		if (pcsItemTemplate && pcsItemTemplate->m_bStackable)
		{
			lCreateItemNum	= pcsADCharTemplate->m_aalDefaultInvenInfo[i][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT] / pcsItemTemplate->m_lMaxStackableCount;
			
			if (pcsADCharTemplate->m_aalDefaultInvenInfo[i][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT] % pcsItemTemplate->m_lMaxStackableCount)
				++lCreateItemNum;
		}

		INT32	lStackCount	= pcsADCharTemplate->m_aalDefaultInvenInfo[i][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT];
		for (int j = 0; j < lCreateItemNum; ++j)
		{
			AgpdItem *pcsItem = pThis->CreateItem(pcsADCharTemplate->m_aalDefaultInvenInfo[i][AGPDITEM_DEFAULT_INVEN_INDEX_TID], NULL, lStackCount);
			if (!pcsItem)
				continue;

			lStackCount	-= pcsItem->m_nCount;

			if (!pThis->m_pagpmItem->AddItemToInventory(pcsCharacter, pcsItem))
			{
				pThis->m_pagpmItem->RemoveItem(pcsItem->m_lID);

				continue;
			}
		}
	}

	return TRUE;
}

BOOL AgsmItemManager::CBUnLoadCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpdCharacter	*pcsCharacter = (AgpdCharacter *) pData;
	AgsmItemManager	*pThis = (AgsmItemManager *) pClass;
	AgpdItemADChar	*pcsADChar = pThis->m_pagpmItem->GetADCharacter( pcsCharacter );
	AgpdGridItem	*pcsGridItem;

	// 이 캐럭터 데이타를 UnLoad 한다. 걍 pcsCharacter가 가지고 있는 아템들을 Remove한다.

	AgpdGridItem	*pcsAgpdGridItem;

	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsAgpdGridItem = pThis->m_pagpmItem->GetEquipItem( pcsCharacter, i );
		if( pcsAgpdGridItem == NULL )
			continue;

		pThis->m_pagpmItem->RemoveItem(pcsAgpdGridItem->m_lItemID);
	}

	i = 0;
	for (pcsGridItem = pThis->m_pagpmGrid->GetItemSequence(&pcsADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = pThis->m_pagpmGrid->GetItemSequence(&pcsADChar->m_csInventoryGrid, &i))
	{
		pThis->m_pagpmItem->RemoveItem( pcsGridItem->m_lItemID );
	}

	return TRUE;
}

BOOL AgsmItemManager::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpdCharacter	*pcsCharacter = (AgpdCharacter *) pData;
	AgsmItemManager	*pThis = (AgsmItemManager *) pClass;

	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
	{
		pThis->m_pagsmItem->SendPacketUpdateReverseOrbReuseTime(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
		pThis->m_pagsmItem->SendPacketUpdateTransformReuseTime(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
		return TRUE;
	}

	return TRUE;
}

// 아이템 생성 kelovon 20051123
//		CBCreateNewItem
//	Functions
//		- Create New Item
//	Arguments
//		- pData : PVOID	pvBuffer[5];
//					pvBuffer[0]: INT32 lTID
//					pvBuffer[1]: AgpdCharacter *pcsCharacter
//					pvBuffer[2]: INT32 lItemCount
//					pvBuffer[3]: BOOL bGenerateDBID
//					pvBuffer[4]: INT32 lSkillTID
//					pvBuffer[5]: AgpdItem** pcsNewItem, return value
//		- pClass : this module pointer
//	Return value
//		- BOOL : 처리결과
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItemManager::CBCreateNewItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemManager		*pThis		= (AgsmItemManager*)pClass;
	PVOID				*pvBuffer	= (PVOID*)pData;

	AgpdItem *pcsNewItem= pThis->CreateItem(PtrToInt(pvBuffer[0]), (AgpdCharacter*)pvBuffer[1],
											PtrToInt(pvBuffer[2]), PtrToInt(pvBuffer[3]), PtrToInt(pvBuffer[4]));

	*(AgpdItem**)pvBuffer[5] = pcsNewItem;

	return TRUE;
}

BOOL AgsmItemManager::CBRemoveItemID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemManager		*pThis				= (AgsmItemManager *)	pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	pThis->m_csGenerateID.AddRemoveID(pcsItem->m_lID);

	return TRUE;
}

BOOL AgsmItemManager::CBDeleteItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemManager	*pThis			= (AgsmItemManager *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)			pData;
	AgsdItem		*pcsAgsdItem	= pThis->m_pagsmItem->GetADItem(pcsItem);

	if (pcsAgsdItem->m_bIsNeedInsertDB)
		return TRUE;

	if (0 == _tcsnicmp(pcsItem->m_szDeleteReason, _T("Throw wastebasket"), _tcslen(_T("Throw wastebasket"))))
	{
		INT32 lCID = pcsItem->m_pcsCharacter ? pcsItem->m_pcsCharacter->m_lID : 0;
		pThis->m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_DEST, lCID, pcsItem, pcsItem->m_nCount ? pcsItem->m_nCount : 1);
	}

	return pThis->m_pagsmItem->SendRelayDelete(pcsItem);
}

BOOL AgsmItemManager::CBGetNewIID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemManager		*pThis			= (AgsmItemManager *)	pClass;
	INT32				*plIID			= (INT32 *)				pData;

	*plIID = pThis->m_csGenerateID.GetID();

	return TRUE;
}

BOOL AgsmItemManager::CBGetNewDBID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemManager		*pThis			= (AgsmItemManager *)	pClass;
	UINT64				*pullDBID		= (UINT64 *)			pData;

	*pullDBID = pThis->GetDBID();

	return TRUE;
}

BOOL AgsmItemManager::CBGetNewCashItemBuyID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemManager		*pThis			= (AgsmItemManager *)	pClass;
	UINT64				*pullDBID		= (UINT64 *)			pData;

	*pullDBID = pThis->GetCashItemBuyID();

	return TRUE;
}

UINT64 AgsmItemManager::GetDBID()
{
	return m_csGenerateID64.GetID();
}

UINT64 AgsmItemManager::GetCashItemBuyID()
{
	return m_csGenerateID64CashItemBuy.GetID();
}

BOOL AgsmItemManager::CBSplitItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemManager		*pThis				= (AgsmItemManager *)	pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	INT32				lSplitStackCount	= PtrToInt(ppvBuffer[0]);
	AgpdItem			**ppcsNewItem		= (AgpdItem **)			ppvBuffer[1];
	AgpdItemStatus		*peItemStatus		= (AgpdItemStatus *)	ppvBuffer[2];

	if (!ppcsNewItem)
		return FALSE;

	if (pcsItem->m_nCount <= lSplitStackCount ||
		lSplitStackCount < 1 ||
		!((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_bStackable)
		return FALSE;

	pThis->m_pagpmItem->SubItemStackCount(pcsItem, lSplitStackCount);

	*ppcsNewItem	= pThis->CreateItem(pcsItem->m_lTID, pcsItem->m_pcsCharacter, lSplitStackCount, TRUE);

	if (*ppcsNewItem)
	{
		(*ppcsNewItem)->m_eStatus		= (INT16) *peItemStatus;
		(*ppcsNewItem)->m_lSkillLevel	= pcsItem->m_lSkillLevel;
		(*ppcsNewItem)->m_lSkillTID		= pcsItem->m_lSkillTID;
	}

	return TRUE;
}

AgpdItem *AgsmItemManager::GetSelectResult5(AgpdCharacter *pAgpdCharacter, AuDatabase2 *pDatabase)
{
	if (!pDatabase || !pAgpdCharacter)
		return NULL;

	AgpdItem	*pAgpdItem = NULL;

	UINT64	ullDBID		= 0;
	CHAR	szOwnerName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	INT32	lTemplateID	= 0;
	INT32	lStackCount	= 1;

	INT16	nStatus		= (-0);

	CHAR	szPosition[65];
	CHAR	szConvertHistory[257];
	CHAR	szOption[33];
	CHAR	szSkillPlus[33];

	INT32	lDurability	= 0;
	INT32	lMaxDurability	= 0;

	INT32	lStatusFlag	= 0;

	INT32	lInUse = 0;
	INT32	lUseCount = 0;
	INT64	llRemainTime = 0;
	CHAR	szExpireDate[33];
	INT64	llStaminaRemainTime = 0;

	ZeroMemory(szOwnerName, sizeof(szOwnerName));
	ZeroMemory(szPosition, sizeof(szPosition));
	ZeroMemory(szConvertHistory, sizeof(szConvertHistory));
	ZeroMemory(szOption, sizeof(szOption));
	ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	ZeroMemory(szExpireDate, sizeof(szExpireDate));

	INT16	nIndex = 0;
	CHAR	*szBuffer = NULL;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		ullDBID					= (INT64)_atoi64(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)		// fetched but we don't use
		strncpy(szOwnerName, szBuffer, AGPDCHARACTER_MAX_ID_LENGTH);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lTemplateID				= (INT32) atoi(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lStackCount				= (INT32) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		nStatus					= (INT16) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
	{
		strncpy(szPosition, szBuffer, 64);
	}

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
	{
		strncpy(szConvertHistory, szBuffer, 256);
	}

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lDurability				= (INT32) atoi(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lMaxDurability			= (INT32) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lStatusFlag				= (INT32) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
	{
		strncpy(szOption, szBuffer, 32);
	}

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
	{
		strncpy(szSkillPlus, szBuffer, 32);
	}

	// in use
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lInUse = (INT32) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lUseCount = (INT32) atoi(szBuffer);

	// remain time
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		llRemainTime = _atoi64(szBuffer);

	// expire date
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
	{
		strncpy(szExpireDate, szBuffer, 32);
	}

	// stamina remain time
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		llStaminaRemainTime = _atoi64(szBuffer);

	if (ullDBID && lTemplateID != AP_INVALID_IID && szOwnerName[0])
	{
		INT32 lCorrectInUse = lInUse & 0x000000FF;
		pAgpdItem = CreateItem(lTemplateID, lStackCount, szPosition, nStatus, szConvertHistory, lDurability, lMaxDurability,
							   lStatusFlag, szOption, szSkillPlus, (INT16) lCorrectInUse, lUseCount,
							   llRemainTime, AuTimeStamp::ConvertOracleTimeToTimeStamp(szExpireDate), llStaminaRemainTime,
							   ullDBID, pAgpdCharacter
							   );
	}


	return pAgpdItem;
}

AgpdItem* AgsmItemManager::GetSelectBankResult5(AgpdCharacter *pAgpdCharacter, AuDatabase2 *pDatabase)
{
	// 2007.02.06 laki
	// difference between two method is second column(AccountID or CharID)
	// that fetched in GetSelectResult5() but not referenced.
	// so delegate. hmm
	return GetSelectResult5(pAgpdCharacter, pDatabase);
}

INT32 AgsmItemManager::Remove1857Rune(CHAR *szBuffer, INT32 lBufferSize)
{
	// netong
	// extra code for event
	
	if (!szBuffer || lBufferSize < 1)
		return 0;

	INT32	lStartIndex	= 0;
	INT32	lOffset		= 0;

	INT32	lTID		= 0;

	INT32	lNum1857Rune	= 0;

	while (lOffset < strlen(szBuffer))
	{
		sscanf(szBuffer + lStartIndex, "%d", &lTID);

		for ( ; lOffset < strlen(szBuffer); ++lOffset)
		{
			if (szBuffer[lOffset] == ',' ||
				szBuffer[lOffset] == ':')
				break;
		}

		if (lOffset == strlen(szBuffer))
		{
			if (lTID == 1857)
			{
				szBuffer[lStartIndex]	= NULL;

				++lNum1857Rune;
			}

			break;
		}
		else
		{
			if (lTID == 1857)
			{
				INT32	lCopyLength	= (INT32)_tcslen(szBuffer) - lOffset - 1;

				CopyMemory(szBuffer + lStartIndex, szBuffer + lOffset + 1, sizeof(CHAR) * lCopyLength);

				szBuffer[lStartIndex + lCopyLength] = NULL;

				lOffset	= lStartIndex;

				++lNum1857Rune;
			}
			else
			{
				++lOffset;
				lStartIndex	= lOffset;
			}
		}
	}

	return lNum1857Rune;
}

BOOL AgsmItemManager::CBGetItemExtraData( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemManager* pThis = (AgsmItemManager*)pClass;
	AgpdItem* pcsItem = (AgpdItem*)pData;
	PACKET_AGPMITEM_EXTRA* pPacketExtra = (PACKET_AGPMITEM_EXTRA*)pCustData;

	if(!pThis || !pcsItem)
		return FALSE;

	AgsdItem* pcsAgsdItem = pThis->m_pagsmItem->GetADItem(pcsItem);
	if(!pcsAgsdItem)
		return FALSE;

	AgpdSealData* pcsSealData = pThis->GetSealingData(pcsAgsdItem->m_ullDBIID);
	if(pcsSealData)
	{
		pPacketExtra->Type = 1;
		memcpy(&pPacketExtra->ExtraData.SealData, pcsSealData, sizeof(AgpdSealData));
	}

	return TRUE;
}

BOOL AgsmItemManager::CBRelayConnect( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(!pData || !pClass)
		return TRUE;

	AgsdServer *pAgsdServer = (AgsdServer *) pData;
	AgsmItemManager* pThis = (AgsmItemManager*)pClass;

	AgsdServer *pRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if (!pRelayServer)
		return FALSE;

	if (pAgsdServer == pRelayServer)
	{
		if( pThis->m_pagsmEventSystem )
			pThis->m_pagsmEventSystem->SendRequestSealingDataAll();
	}

	return TRUE;
}

BOOL AgsmItemManager::AddSealingData(AgsdSealData* pcsSealData)
{
	if(!pcsSealData)
		return FALSE;

	AuAutoLock pLock(m_SealDataMutex);
	if(!pLock.Result())
		return FALSE;

	pair<AgsSealData::iterator, bool> ib = m_SealingData.insert(AgsSealData::value_type(pcsSealData->DBID, pcsSealData->SealData));
	if(ib.second != true)
		return FALSE;

	return TRUE;
}

BOOL AgsmItemManager::RemoveSealingData(UINT64 DBID)
{
	AuAutoLock pLock(m_SealDataMutex);
	if(!pLock.Result())
		return FALSE;

	AgsSealData::size_type st = m_SealingData.erase(DBID);
	if(st == 0)
		return FALSE;

	return TRUE;
}

AgpdSealData* AgsmItemManager::GetSealingData(UINT64 DBID)
{
	AgsSealData::iterator it = m_SealingData.find(DBID);
	
	return (it != m_SealingData.end()) ? &it->second : NULL;
}

