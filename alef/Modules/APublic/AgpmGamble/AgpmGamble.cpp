/*===========================================================================

	AgpmGamble.cpp

===========================================================================*/


#include "AgpmGamble.h"
#include "ApMemoryTracker.h"


/****************************************************/
/*		The Implementation of AgpmGamble class		*/
/****************************************************/
//
AgpmGamble::AgpmGamble()
	{
	SetModuleName("AgpmGamble");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMGAMBLE_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1,	// operation
							AUTYPE_INT32,			1,	// cid
							AUTYPE_INT32,			1,	// gamble tid
							AUTYPE_INT32,			1,	// result
							AUTYPE_PACKET,			1,	// embedded packet, event
							AUTYPE_END,				0
							);
	}


AgpmGamble::~AgpmGamble()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgpmGamble::OnAddModule()
	{
	m_pApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmDropItem2 = (AgpmDropItem2 *) GetModule("AgpmDropItem2");

	if (!m_pApmEventManager || !m_pAgpmCharacter || !m_pAgpmItem || !m_pAgpmGrid || !m_pAgpmFactors)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackActionEventGamble(CBEventAction, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_GAMBLE, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgpmGamble::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8	cOperation = AGPMGAMBLE_OPERATION_NONE;
	INT32	lCID = AP_INVALID_CID;
	INT32	lGambleTID = 0;
	INT32	lResult = AGPMGAMBLE_RESULT_NONE;
	PVOID	pvPacketEmb = NULL;
	
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lGambleTID,
						&lResult,
						&pvPacketEmb
						);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	switch (cOperation)
		{
		case AGPMGAMBLE_OPERATION_EVENT_REQUEST :
			{
			ApdEvent *pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if (!pApdEvent)
				return FALSE;

			if (APDEVENT_FUNCTION_GAMBLE != pApdEvent->m_eFunction)
				return FALSE;
					
			OnOperationEventRequest(pApdEvent, pAgpdCharacter);
			}
			break;

		case AGPMGAMBLE_OPERATION_EVENT_GRANT :
			{
			ApdEvent *pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if (!pApdEvent)
				return FALSE;

			if (APDEVENT_FUNCTION_GAMBLE != pApdEvent->m_eFunction)
				return FALSE;
					
			OnOperationEventGrant(pApdEvent, pAgpdCharacter);
			}
			break;
		
		case AGPMGAMBLE_OPERATION_GAMBLE :
			{
			AgpdGambleParam Param;
			Param.m_lTID = lGambleTID;
			Param.m_lResult = lResult;
			EnumCallback(AGPMGAMBLE_CB_GAMBLE, pAgpdCharacter, &Param);
			}
			break;
		
		default :
			break;
		}

	return TRUE;
	}


BOOL AgpmGamble::OnDestroy()
	{
	for (GambleItemMapIter ItemIter = m_GambleItemMap.begin(); ItemIter != m_GambleItemMap.end(); ItemIter++)
		{
		if (ItemIter->second)
			delete ItemIter->second;
		}
	
	return TRUE;
	}




//	Operation
//==================================================
//
BOOL AgpmGamble::OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	ZeroMemory(&stTargetPos, sizeof(stTargetPos));
	
	if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMGAMBLE_MAX_USE_RANGE, &stTargetPos))
		{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pAgpdCharacter->m_bMove)
			m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMGAMBLE_CB_EVENT_REQUEST, pAgpdCharacter, pApdEvent);
		}
	else
		{
		// move to target position
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_GAMBLE;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_FORWARD, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}


BOOL AgpmGamble::OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	EnumCallback(AGPMGAMBLE_CB_EVENT_GRANT, pAgpdCharacter, pApdEvent);

	return TRUE;
	}




//	Map
//===================================================
//
AgpdGamble*	AgpmGamble::GetTemplate(INT32 lTID)
	{
	GambleMapIter Iter = m_TemplateMap.find(lTID);
	
	if (Iter == m_TemplateMap.end())
		return NULL;
	else
		return &(Iter->second);
	}


BOOL AgpmGamble::MakeGambleItemMap()
	{
	INT32 lIndex = 0;
	AgpdItemTemplate *pAgpdItemTemplate = NULL;
	
	 for(AgpaItemTemplate::iterator it = m_pAgpmItem->csTemplateAdmin.begin(); it != m_pAgpmItem->csTemplateAdmin.end(); ++it)
		{
		pAgpdItemTemplate = it->second;

		// check gamblable item
		if (!pAgpdItemTemplate->m_bEnableGamble)
			continue;
		
		for (GambleMapIter Iter = m_TemplateMap.begin(); Iter != m_TemplateMap.end(); Iter++)
			{
			AgpdGamble *pAgpdGamble = &(Iter->second);
			
			if (!_CheckItemAllType(pAgpdItemTemplate, pAgpdGamble->m_pItemTemplate))
				continue;

			// check race
			if (m_pAgpmFactors->GetRace(&pAgpdGamble->m_pItemTemplate->m_csRestrictFactor) != m_pAgpmFactors->GetRace(&pAgpdItemTemplate->m_csRestrictFactor))
				continue;
	
			// check class
			if (m_pAgpmFactors->GetClass(&pAgpdGamble->m_pItemTemplate->m_csRestrictFactor) != m_pAgpmFactors->GetClass(&pAgpdItemTemplate->m_csRestrictFactor))
				continue;
	
			// check rank
			INT32 lRank = 0;
			m_pAgpmFactors->GetValue(&pAgpdItemTemplate->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
			if (0 == lRank)
				lRank = 1;		// if rank not exist, treat as rank1
			
			// find map element
			AgpdGambleItemList *pAgpdGambleItemList = NULL;
			GambleItemMapIter ItemIter = m_GambleItemMap.find(pAgpdGamble->m_lTID);
			if (ItemIter == m_GambleItemMap.end())
				{
				// add new map element
				AgpdGambleItemList *pAgpdGambleItemList_New= new AgpdGambleItemList;
				m_GambleItemMap.insert(GambleItemMapPair(pAgpdGamble->m_lTID, pAgpdGambleItemList_New));
				pAgpdGambleItemList = pAgpdGambleItemList_New;
				}
			else
				pAgpdGambleItemList = ItemIter->second;

			ASSERT(pAgpdGambleItemList);

			// insert new template			
			BOOL bInsert = pAgpdGambleItemList->Add(pAgpdItemTemplate, lRank-1);	// 0-based
			ASSERT(bInsert);
			}
		
		}
	
  #ifdef _DEBUG
	// validate
	for (GambleMapIter Iter = m_TemplateMap.begin(); Iter != m_TemplateMap.end(); Iter++)
		{
		GambleItemMapIter ItemIter = m_GambleItemMap.find(Iter->first);
		if (ItemIter == m_GambleItemMap.end())
			{
			TRACE("!!! Error : No item map in AgpmGamble::MakeGambleItemMap()\n");
			continue;
			}
		
		for (INT32 i = 0; i < AGPDGAMBLE_MAX_RANK; i++)
			{
			if (Iter->second.m_alRank[i] > 0 &&								// probability of given rank exist
				0 >= ItemIter->second->m_apItemTemplateByRank[i].size())	// but no item to gamble
				{
				TRACE("!!! Error : GambleID(%d) index(%d) in AgpmGamble::MakeGambleItemMap()\n", Iter->first, i);
				ASSERT(!"Gamble");
				}
			}
		}
  #endif	
	
	return TRUE;
	}




//	Character level specefic item list
//===================================================
//
INT32 AgpmGamble::GetGambleItemList(AgpdGamble *pAgpdGamble, AgpdCharacter *pAgpdCharacter, AgpdGambleItemList *pList)
	{
	if (!m_pAgpmDropItem2 || !pAgpdGamble || !pAgpdCharacter)
		return 0;
	
	GambleItemMapIter Iter1 = m_GambleItemMap.find(pAgpdGamble->m_lTID);
	if (Iter1 == m_GambleItemMap.end())
		return 0;

	INT32 lTotal = 0;
	AgpdGambleItemList *pAgpdGambleItemList = pAgpdGambleItemList = Iter1->second;
	for (INT32 i = 0; i < AGPDGAMBLE_MAX_RANK; i++)
		{
		for (GambleItemListIter Iter2 = pAgpdGambleItemList->m_apItemTemplateByRank[i].begin(); Iter2 != pAgpdGambleItemList->m_apItemTemplateByRank[i].end(); Iter2++)
			{
			AgpdDropItemADItemTemplate *pAgpdDropItemADItemTemplate = m_pAgpmDropItem2->GetItemTemplateData(*Iter2);
			if (pAgpdDropItemADItemTemplate &&
				m_pAgpmCharacter->GetLevel(pAgpdCharacter) >= pAgpdDropItemADItemTemplate->m_lSuitableLevelMin)
				{
				if (pList)
					pList->Add(*Iter2, i);
				lTotal++;
				}
			}
		}
	
	return lTotal;
	}


INT32 AgpmGamble::GetGambleItemList(INT32 lTID, AgpdCharacter *pAgpdCharacter, AgpdGambleItemList *pList)
	{
	return GetGambleItemList(GetTemplate(lTID), pAgpdCharacter, pList);
	}




//	Validation
//===================================================
//
BOOL AgpmGamble::IsValid(AgpdCharacter *pAgpdCharacter, AgpdGamble *pAgpdGamble, INT32 *plResult)
	{
	INT32 lResult = AGPMGAMBLE_RESULT_NONE;
	
	if (NULL == pAgpdCharacter || NULL == pAgpdGamble)
		goto GambleIsValid;
	
	// check inventory
	if (m_pAgpmGrid->IsFullGrid(m_pAgpmItem->GetInventory(pAgpdCharacter)))
		{
		lResult = AGPMGAMBLE_RESULT_FULL_INVENTORY;
		goto GambleIsValid;
		}
	
	// race
	if (!m_pAgpmFactors->CheckRace((AuRaceType)m_pAgpmFactors->GetRace(&pAgpdCharacter->m_csFactor), &pAgpdGamble->m_pItemTemplate->m_csRestrictFactor))
		{
		lResult = AGPMGAMBLE_RESULT_NA_RACE;
		goto GambleIsValid;
		}
	
	// class
	if (!m_pAgpmFactors->CheckClass((AuCharClassType)m_pAgpmFactors->GetClass(&pAgpdCharacter->m_csFactor), &pAgpdGamble->m_pItemTemplate->m_csRestrictFactor))
		{
		lResult = AGPMGAMBLE_RESULT_NA_CLASS;
		goto GambleIsValid;
		}
	
	// cost
	if (Cost(pAgpdCharacter, pAgpdGamble) > m_pAgpmCharacter->GetCharismaPoint(pAgpdCharacter))
		{
		lResult = AGPMGAMBLE_RESULT_NOT_ENOUGH_POINT;
		goto GambleIsValid;
		}

	lResult = AGPMGAMBLE_RESULT_SUCCESS;

  GambleIsValid:
	if (plResult)
		*plResult = lResult;
	
	return (AGPMGAMBLE_RESULT_SUCCESS == lResult);
	}


BOOL AgpmGamble::IsValid(AgpdCharacter *pAgpdCharacter, INT32 lGambleTID, INT32 *plResult)
	{
	return IsValid(pAgpdCharacter, GetTemplate(lGambleTID));
	}

INT32 AgpmGamble::Cost(AgpdCharacter *pAgpdCharacter, AgpdGamble *pAgpdGamble)
	{
	return m_pAgpmCharacter->GetLevel(pAgpdCharacter) * pAgpdGamble->m_lBaseCost;
	}



//	Event Packet
//===================================
//
BOOL AgpmGamble::StreamReadGamble(CHAR *pszFile, BOOL bDecryption)
	{
	if (NULL == pszFile || '\0' == *pszFile)
		return FALSE;
	
	AuExcelTxtLib Excel;
	if (!Excel.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;
	
	for (INT32 lRow = 1; lRow < Excel.GetRow(); lRow++)
		{
		CHAR *psz = NULL;
		AgpdGamble	Gamble;
		AgpdItemTemplate *pAgpdItemTemplate = NULL;

		psz = Excel.GetData(AGPMGAMBLE_EXCEL_COLUMN_IMAGEID, lRow);
		if (!psz)
			continue;
		Gamble.m_lTID = atoi(psz);
		
		pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(Gamble.m_lTID);
		if (NULL == pAgpdItemTemplate)
			continue;
		
		// never gamble other type
		if (AGPMITEM_TYPE_OTHER == pAgpdItemTemplate->m_nType)
			continue;
		
		Gamble.m_pItemTemplate = pAgpdItemTemplate;
		
		psz = Excel.GetData(AGPMGAMBLE_EXCEL_COLUMN_NAME, lRow);
		if (!psz)
			continue;
		strncpy(Gamble.m_szName, psz, AGPMITEM_MAX_ITEM_NAME);
		
		psz = Excel.GetData(AGPMGAMBLE_EXCEL_COLUMN_COST, lRow);
		if (!psz)
			continue;
		Gamble.m_lBaseCost = atoi(psz);		
		
		INT32 lTotalRankProb = 0;
		for (INT32 lRankIndex = 0; lRankIndex < AGPDGAMBLE_MAX_RANK; lRankIndex++)
			{
			psz = Excel.GetData(AGPMGAMBLE_EXCEL_COLUMN_RANK1 + lRankIndex, lRow);
			if (psz)
				{
				Gamble.m_alRank[lRankIndex] = atoi(psz);
				lTotalRankProb += Gamble.m_alRank[lRankIndex];
				}
			}

		if (100 != lTotalRankProb)
			{
			ASSERT(!"Sum of total prob. isn't 100");
			continue;
			}
			
		// add to map
		m_TemplateMap.insert(GambleMapPair(Gamble.m_lTID, Gamble));
		}

	return MakeGambleItemMap();
	}




//	Event Packet
//===================================
//
PVOID AgpmGamble::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMGAMBLE_OPERATION_EVENT_REQUEST;
	
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMGAMBLE_PACKET_TYPE,
										   &cOperation,			// operation
										   &lCID,				// cid
										   NULL,				// Gamble TID
										   NULL,				// result
										   pvPacketEvent		// embedded packet
										   );

	return pvPacket;
	}


PVOID AgpmGamble::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMGAMBLE_OPERATION_EVENT_GRANT;
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMGAMBLE_PACKET_TYPE,
										   &cOperation,			// operation
										   &lCID,				// cid
										   NULL,				// Gamble TID
										   NULL,				// result
										   pvPacketEvent		// embedded packet
										   );

	return pvPacket;
	}




//	Event Callbacks
//======================================================
//
BOOL AgpmGamble::CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmGamble *pThis = (AgpmGamble *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pAgpdCharacter->m_bMove)
		pThis->m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_GAMBLE
															  );

	pThis->EnumCallback(AGPMGAMBLE_CB_EVENT_REQUEST, pApdEvent, pAgpdCharacter);
	
	return TRUE;
	}




//	Callback setting
//===================================================
//
BOOL AgpmGamble::SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGAMBLE_CB_EVENT_REQUEST, pfCallback, pClass);
	}


BOOL AgpmGamble::SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGAMBLE_CB_EVENT_GRANT, pfCallback, pClass);
	}


BOOL AgpmGamble::SetCallbackGamble(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGAMBLE_CB_GAMBLE, pfCallback, pClass);
	}




//	Item Type Check
//===================================================
//
// so fucking data structure design, so fuuuuuuuuuuuuking comparison code
//
BOOL AgpmGamble::_CheckItemAllType(AgpdItemTemplate *pAgpdItemTemplate1, AgpdItemTemplate *pAgpdItemTemplate2)
	{
	// check template type(equip, usable). other type already filtered
	if (pAgpdItemTemplate1->m_nType != pAgpdItemTemplate2->m_nType)
		return FALSE;

	if (AGPMITEM_TYPE_EQUIP == pAgpdItemTemplate1->m_nType)
		{
		if (((AgpdItemTemplateEquipWeapon *) pAgpdItemTemplate1)->m_nWeaponType
			 != ((AgpdItemTemplateEquipWeapon *) pAgpdItemTemplate2)->m_nWeaponType)
			return FALSE;
		}
	else if (AGPMITEM_TYPE_USABLE == pAgpdItemTemplate1->m_nType)
		{
		if (((AgpdItemTemplateUsable *) pAgpdItemTemplate1)->m_nUsableItemType
			!= ((AgpdItemTemplateUsable *) pAgpdItemTemplate2)->m_nUsableItemType)
			return FALSE;

		switch (((AgpdItemTemplateUsable *) pAgpdItemTemplate1)->m_nUsableItemType)
			{
			case AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL :
				if (((AgpdItemTemplateUsableTeleportScroll *) pAgpdItemTemplate1)->m_eTeleportScrollType
					!= ((AgpdItemTemplateUsableTeleportScroll *) pAgpdItemTemplate2)->m_eTeleportScrollType)
					return FALSE;
				break;
			case AGPMITEM_USABLE_TYPE_RUNE :
				if (((AgpdItemTemplateUsableRune *) pAgpdItemTemplate1)->m_eRuneAttributeType
					!= ((AgpdItemTemplateUsableRune *) pAgpdItemTemplate2)->m_eRuneAttributeType)
					return FALSE;
				break;
				
			case AGPMITEM_USABLE_TYPE_POTION :
				if (((AgpdItemTemplateUsablePotion *) pAgpdItemTemplate1)->m_bIsPercentPotion
					!= ((AgpdItemTemplateUsablePotion *) pAgpdItemTemplate2)->m_bIsPercentPotion)
					return FALSE;
				break;
				
			case AGPMITEM_USABLE_TYPE_SKILL_SCROLL :
				if (((AgpdItemTemplateUsableSkillScroll *) pAgpdItemTemplate1)->m_eScrollSubType
					!= ((AgpdItemTemplateUsableSkillScroll *) pAgpdItemTemplate2)->m_eScrollSubType)
					return FALSE;
				break;
				
			case AGPMITEM_USABLE_TYPE_AREA_CHATTING :
				if (((AgpdItemTemplateUsableAreaChatting *) pAgpdItemTemplate1)->m_eAreaChattingType
					!= ((AgpdItemTemplateUsableAreaChatting *) pAgpdItemTemplate1)->m_eAreaChattingType)
					return FALSE;
				break;

			default :
				if (pAgpdItemTemplate1->m_nSubType != pAgpdItemTemplate2->m_nSubType)
					return FALSE;
				break;
			}
		}
	else		// other
		{
		return FALSE;
		}

	return TRUE;
	}
