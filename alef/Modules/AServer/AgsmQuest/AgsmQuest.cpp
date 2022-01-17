// AgsmQuest.cpp: implementation of the AgsmQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmQuest.h"
#include "AgpmEventQuest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmQuest::AgsmQuest()
{
	SetModuleName("AgsmQuest");
}

AgsmQuest::~AgsmQuest()
{

}

BOOL AgsmQuest::OnAddModule()
{
	m_pcsAgpmQuest		= (AgpmQuest*)GetModule("AgpmQuest");
	m_pcsAgpmLog		= (AgpmLog*)GetModule("AgpmLog");
	m_pcsAgsmCharacter	= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmDeath		= (AgsmDeath*)GetModule("AgsmDeath");
	m_pcsAgpmItem		= (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgsmItem		= (AgsmItem*)GetModule("AgsmItem");
	m_pcsAgsmItemManager = (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pcsAgpmGrid		= (AgpmGrid*)GetModule("AgpmGrid");
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmSkill		= (AgpmSkill*)GetModule("AgpmSkill");
	m_pcsAgsmChatting	= (AgsmChatting*)GetModule("AgsmChatting");
	m_pcsAgpmItemConvert = (AgpmItemConvert*)GetModule("AgpmItemConvert");
	m_pcsAgsmItemConvert = (AgsmItemConvert*)GetModule("AgsmItemConvert");
	m_pcsAgsmParty		= (AgsmParty*)GetModule("AgsmParty");

	if (!m_pcsAgpmQuest || !m_pcsAgsmCharacter || !m_pcsAgsmDeath || !m_pcsAgsmItem ||
		!m_pcsAgpmGrid || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmSkill || !m_pcsAgsmChatting ||
		!m_pcsAgpmItemConvert || !m_pcsAgsmItemConvert || !m_pcsAgsmParty)
		return FALSE;

	if (!m_pcsAgpmQuest->SetCallbackRequireQuest(CBRequireQuest, this))
		return FALSE;

	if (!m_pcsAgpmQuest->SetCallbackCompleteQuest(CBRequireComplete, this))
		return FALSE;

	if (!m_pcsAgpmQuest->SetCallbackCancelQuest(CBRequireCancel, this))
		return FALSE;

	if (!m_pcsAgpmQuest->SetCallbackCheckPoint(CBRequireCheckPoint, this))
		return FALSE;

	// 게임서버에서 클라이언트로 데이터 보낼때
	if (!m_pcsAgsmCharacter->SetCallbackSendCharacterAllInfo(SendQuestDataAllInfo, this))
		return FALSE;

	// 로그인에서 게임서버로 데이터 보낼때
	if (!m_pcsAgsmCharacter->SetCallbackSendCharacterNewID(SendQuestDataAtLogin, this))
		return FALSE;

	if (!m_pcsAgsmDeath->SetCallbackCharacterDead(CBMonsterDeath, this))
		return FALSE;

	if (!m_pcsAgsmChatting->SetCallbackQuestComplete(CBChatQuestComplete, this))
		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackPickupQuestValidCheck(CBQuestItemPickupValidCheck, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmQuest::OnInit()
{
	m_pcsAgpmTitle		= (AgpmTitle*)GetModule("AgpmTitle");
	m_pcsAgsmTitle		= (AgsmTitle*)GetModule("AgsmTitle");

	if(!m_pcsAgpmTitle || !m_pcsAgsmTitle)
		return FALSE;

	return TRUE;
}

BOOL AgsmQuest::OnDestroy()
{
	return TRUE;
}

BOOL AgsmQuest::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmQuest::GetSelectQueryFlag(CHAR* szCharName, char *pstrWorldDBName, CHAR* pstrQuery, INT32 lQueryLength)
{
	if (!szCharName ||
		!szCharName[0] ||
		!pstrQuery)
		return FALSE;

	sprintf(pstrQuery, "select flag from %s.chardetail where charid='%s'", pstrWorldDBName, szCharName);

	ASSERT((INT32)strlen(pstrQuery) < lQueryLength);

	return TRUE;
}

BOOL AgsmQuest::GetSelectQueryFlag4(CHAR* pstrQuery, INT32 lQueryLength)
{
	if (!pstrQuery)
		return FALSE;

	strcpy(pstrQuery, "SELECT flag FROM chardetail WHERE charid=:1");

	ASSERT((INT32)strlen(pstrQuery) < lQueryLength);

	return TRUE;
}

//BOOL AgsmQuest::GetSelectResultFlag(COLEDB *pcOLEDB, AgpdCharacter *pcsCharacter)
//{
//	if (!pcOLEDB ||
//		!pcsCharacter)
//		return FALSE;
//
//	AgpdQuest* pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);
//	if (!pcsAgpdQuest)
//		return FALSE;
//
//	CHAR	*szBuffer = NULL;
//	CHAR	*szBufferSE = NULL;
//
//
//	if ((szBuffer = pcOLEDB->GetQueryResult(0)) != NULL)
//	{
//// AGSDQUEST_EXPAND_BLOCK
//		if (szBuffer[0] & DEFAULT_FLAG)
//		{
//			pcsAgpdQuest->m_csFlag.MemoryCopy((BYTE*)szBuffer, 1);
//			if ((szBufferSE = pcOLEDB->GetQueryResult(1)) != NULL)
//				pcsAgpdQuest->m_csFlag.MemoryCopy((BYTE*)szBufferSE, 2);
//		}
//		else
//			pcsAgpdQuest->m_csFlag.InitFlags();
//	}
//
//	return TRUE;
//}

BOOL AgsmQuest::GetSelectQueryCurrentQuest(CHAR* szCharName, char *pstrWorldDBName, CHAR* pstrQuery, INT32 lQueryLength)
{
	if (!szCharName ||
		!szCharName[0] ||
		!pstrQuery)
		return FALSE;

	sprintf(pstrQuery, "select questtid, arg1, arg2, npcid from %s.charquest where charid='%s'", pstrWorldDBName, szCharName);

	ASSERT((INT32)strlen(pstrQuery) < lQueryLength);

	return TRUE;
}

BOOL AgsmQuest::GetSelectQueryCurrentQuest4(CHAR* pstrQuery, INT32 lQueryLength)
{
	if (!pstrQuery)
		return FALSE;

	strcpy(pstrQuery, "SELECT questtid, arg1, arg2, npcid FROM charquest WHERE charid=:1");

	ASSERT((INT32)strlen(pstrQuery) < lQueryLength);

	return TRUE;
}

BOOL AgsmQuest::SetQuestBlock(BYTE *szDestBlock, CHAR *szSrcBlock)
{
	ASSERT(szDestBlock);
	ASSERT(szSrcBlock);

	if (NULL == szDestBlock)
		return FALSE;

	if (NULL == szSrcBlock)
		return FALSE;

	if (FALSE == (szSrcBlock[0] & DEFAULT_FLAG))
		return FALSE;

	memcpy(szDestBlock, szSrcBlock, AGSDQUEST_MAX_FLAG);

	return TRUE;
}

BOOL AgsmQuest::GetSelectResultFlag5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter)
{
	if (!pDatabase ||
		!pcsCharacter)
		return FALSE;

	AgpdQuest* pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	pcsAgpdQuest->m_csFlag.InitFlags();

	SetQuestBlock(&pcsAgpdQuest->m_csFlag.m_btQuestBlock1[0], pDatabase->GetQueryResult(0));
	SetQuestBlock(&pcsAgpdQuest->m_csFlag.m_btQuestBlock2[0], pDatabase->GetQueryResult(1));
	SetQuestBlock(&pcsAgpdQuest->m_csFlag.m_btQuestBlock3[0], pDatabase->GetQueryResult(2));

	return TRUE;
}

BOOL AgsmQuest::GetSelectResultCurrentQuest5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter)
{
	if (!pDatabase ||
		!pcsCharacter)
		return FALSE;

	AgpdQuest* pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	INT16 nIndex = 0;
	CHAR *szBuffer = NULL;

	AgpdCurrentQuest csCurrentQuest;

	// quest id;
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		csCurrentQuest.lQuestID = atoi(szBuffer);

	// param1
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		csCurrentQuest.lParam1 = atoi(szBuffer);

	// param2
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		csCurrentQuest.lParam2 = atoi(szBuffer);

	// npcid
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(csCurrentQuest.szNPCName, szBuffer, AGPACHARACTER_MAX_ID_STRING);

	pcsAgpdQuest->m_vtQuest.push_back(csCurrentQuest);

	return TRUE;
}

BOOL AgsmQuest::SetCallbackDBCurrentQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMQUEST_CB_CURRENT_QUEST, pfCallback, pClass);
}

BOOL AgsmQuest::SetCallbackDBFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMQUEST_CB_FLAG, pfCallback, pClass);
}

BOOL AgsmQuest::CBRequireQuest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmQuest *pThis		= (AgsmQuest*)pClass;
	CBQuestParam* pParam	= (CBQuestParam*)pData;

	return pThis->RequireQuest(pParam->pcsAgpdCharacter, pParam->pcsEvent, pParam->lQuestTID);
}

BOOL AgsmQuest::CBRequireCheckPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmQuest *pThis		= (AgsmQuest*)pClass;
	CBQuestParam* pParam	= (CBQuestParam*)pData;

	return pThis->RequireCheckPoint(pParam->pcsAgpdCharacter, pParam->pcsEvent, pParam->lQuestTID, pParam->lCheckPointIndex);
}

BOOL AgsmQuest::CBQuestItemPickupValidCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmQuest *pThis				= (AgsmQuest*)pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgpdItem *pcsAgpdItem			= (AgpdItem*)pCustData;

	return pThis->QuestItemPickupValidCheck(pcsAgpdCharacter, pcsAgpdItem);
}

BOOL AgsmQuest::CBRequireComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmQuest *pThis		= (AgsmQuest*)pClass;
	CBQuestParam* pParam	= (CBQuestParam*)pData;

	return pThis->RequireComplete(pParam->pcsAgpdCharacter, pParam->pcsEvent, pParam->lQuestTID);
}

BOOL AgsmQuest::CBRequireCancel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmQuest *pThis		= (AgsmQuest*)pClass;
	CBQuestParam* pParam	= (CBQuestParam*)pData;

	return pThis->RequireCancel(pParam->pcsAgpdCharacter, pParam->lQuestTID);
}

BOOL AgsmQuest::RequireCancel(AgpdCharacter* pcsAgpdCharacter, INT32 lQuestTID)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	AgpdQuest::IterQuest iter = pcsAgpdQuest->FindByQuestID(lQuestTID);
	if (iter == pcsAgpdQuest->m_vtQuest.end())
		return FALSE;

	AgpdQuestTemplate* pcsQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get(lQuestTID);
	if (!pcsQuestTemplate)
		return FALSE;

	AgpdGridItem* pcsAgpdGridItem;
	AgpdItem* pAgpdItem;
	AgpdItemADChar* pcsAgpdItemADChar = m_pcsAgpmQuest->m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	if (!pcsAgpdItemADChar) return FALSE;

	// 퀘스트 아이템 삭제
	if (0 != pcsQuestTemplate->m_PreStart.Item.pItemTemplate)
	{
		// 시작할때 부여해준 아이템은 삭제한다.
		pcsAgpdGridItem = m_pcsAgpmGrid->GetQuestItemByTemplate(&pcsAgpdItemADChar->m_csInventoryGrid, AGPDGRID_ITEM_TYPE_ITEM, 
															pcsQuestTemplate->m_PreStart.Item.pItemTemplate->m_lID);

		if (pcsAgpdGridItem)
		{
			pAgpdItem = m_pcsAgpmQuest->m_pcsAgpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			if (pAgpdItem)
			{
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
											pAgpdItem,
											pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
											);
											
				m_pcsAgpmQuest->m_pcsAgpmItem->RemoveItem(pAgpdItem, TRUE);
			}
		}
	}

	PVOID pBuffer[2];
	pBuffer[0] = (PVOID)AGSMQUEST_DB_CURRENTQUEST_DELETE;
	pBuffer[1] = (PVOID)pcsAgpdCharacter->m_szID;
	EnumCallback(AGSMQUEST_CB_CURRENT_QUEST, &(*iter), pBuffer);
	pcsAgpdQuest->m_vtQuest.erase(iter);
	SendPacketQuestCancelResult(pcsAgpdCharacter->m_lID, lQuestTID, TRUE);

	if (m_pcsAgpmLog)
	{
		AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if (pcsAgsdCharacter)
		{
			INT32 lTID = ((AgpdCharacterTemplate *)pcsAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
			INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter);

			m_pcsAgpmLog->WriteLog_QuestCancel(0,
												&pcsAgsdCharacter->m_strIPAddress[0],
												pcsAgsdCharacter->m_szAccountID,
												pcsAgsdCharacter->m_szServerName,
												pcsAgpdCharacter->m_szID,
												lTID,
												lLevel,
												m_pcsAgpmCharacter->GetExp(pcsAgpdCharacter),
												pcsAgpdCharacter->m_llMoney,
												pcsAgpdCharacter->m_llBankMoney,
												lQuestTID,
												NULL
												);
		}
	}

	return TRUE;
}

BOOL AgsmQuest::CheckPointWork(AgpdCharacter* pcsAgpdCharacter, AgpdQuestGroup* pQuestGroup, AgpdQuest *pcsAgpdQuest, INT32 lQuestTID, INT32 lCheckPointIndex)
{
	ASSERT(AGPDQUEST_MAX_CHECKPOINT > lCheckPointIndex);
	ASSERT(0 < lQuestTID);

	// 플레이어가 가지고 있는 퀘스트인지 확인
	AgpdQuest::IterQuest iterQ = pcsAgpdQuest->FindByQuestID(lQuestTID);
	if (iterQ == pcsAgpdQuest->m_vtQuest.end())
		return FALSE;

	for ( list< AgpdQuestCheckPoint >::iterator iter = pQuestGroup->m_CheckPoint.begin(); iter != pQuestGroup->m_CheckPoint.end(); ++iter ) 
	{
		if ((iter->lQuestTID == lQuestTID) && (iter->lIndex == lCheckPointIndex))
		{
			AgpdQuestTemplate* pQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get(lQuestTID);
			if (!pQuestTemplate) 
				return FALSE;

			ASSERT(pQuestTemplate->m_CheckPoint.CheckPointItem[lCheckPointIndex].pItemTemplate);

			if (!pQuestTemplate->m_CheckPoint.CheckPointItem[lCheckPointIndex].pItemTemplate) 
				return FALSE;

			AgpdGrid *pInventory = m_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
			if (!pInventory) 
				return FALSE;

			// 인벤토리가 가득차 있는지 확인
			if (1 > (pInventory->m_lGridCount - pInventory->m_lItemCount))
			{
				SendPacketInventoryFull(pcsAgpdCharacter->m_lID, lQuestTID);
				return FALSE;
			}

			// 해당 아이템이 있는지 확인후 생성
			if (!m_pcsAgpmGrid->CheckEnoughItem(pInventory, pQuestTemplate->m_CheckPoint.CheckPointItem[lCheckPointIndex].pItemTemplate->m_lID, 
													pQuestTemplate->m_CheckPoint.CheckPointItem[lCheckPointIndex].lItemCount))
			{
				AgpdItem* pcsAgpdItem = m_pcsAgsmItemManager->CreateItem(pQuestTemplate->m_CheckPoint.CheckPointItem[lCheckPointIndex].pItemTemplate->m_lID,
					pcsAgpdCharacter, pQuestTemplate->m_CheckPoint.CheckPointItem[lCheckPointIndex].lItemCount);

				if (!pcsAgpdItem)
					return FALSE;

				if (E_AGPMITEM_BIND_ON_ACQUIRE == m_pcsAgpmItem->GetBoundType(pcsAgpdItem))
					m_pcsAgpmItem->SetBoundOnOwner(pcsAgpdItem, pcsAgpdCharacter);

				m_pcsAgpmItem->SetQuestType(pcsAgpdItem);

				if ( FALSE == m_pcsAgpmItem->AddItemToInventory( pcsAgpdCharacter, pcsAgpdItem ) )
				{
					m_pcsAgpmItem->RemoveItem(pcsAgpdItem, TRUE);
					return FALSE;
				}

				// log.
				m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_REWARD, pcsAgpdCharacter->m_lID,
											pcsAgpdItem,
											pcsAgpdItem->m_nCount ? pcsAgpdItem->m_nCount : 1
											);

				return TRUE;
			}
			else return FALSE;
		}
	}

	return TRUE;
}

BOOL	AgsmQuest::StartQuest(AgpdCharacter* pcsAgpdCharacter, AgpdQuestTemplate* pQuestTemplate)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	// 퀘스트를 시작할수 있는 조건이 충족되었다면 퀘스트를 시작
	if (!m_pcsAgpmQuest->EvaluationStartCondition(pcsAgpdCharacter, pQuestTemplate->m_lID))
		return FALSE;

	pcsAgpdQuest->AddNewQuest(pQuestTemplate->m_lID);
	SendPacketQuestRequireResult(pcsAgpdCharacter->m_lID, pQuestTemplate->m_lID, TRUE, pcsAgpdQuest);

	// 시작시 부여되는 아이템이 있다면 퀘스트 인벤토리로 넣어준다.
	if (pQuestTemplate->m_PreStart.Item.pItemTemplate)
	{
		AgpdItem* pcsAgpdItem = m_pcsAgsmItemManager->CreateItem(pQuestTemplate->m_PreStart.Item.pItemTemplate->m_lID,
															pcsAgpdCharacter, pQuestTemplate->m_PreStart.Item.lCount);

		if (!pcsAgpdItem)
			return FALSE;

		if (E_AGPMITEM_BIND_ON_ACQUIRE == m_pcsAgpmItem->GetBoundType(pcsAgpdItem))
			m_pcsAgpmItem->SetBoundOnOwner(pcsAgpdItem, pcsAgpdCharacter);

		m_pcsAgpmItem->SetQuestType(pcsAgpdItem);

//					m_pcsAgpmItem->AddItemToQuest(pcsAgpdCharacter, pcsAgpdItem);
		if (FALSE == m_pcsAgpmItem->AddItemToInventory( pcsAgpdCharacter, pcsAgpdItem ,FALSE))
		{
			m_pcsAgpmItem->RemoveItem(pcsAgpdItem, TRUE);
			return FALSE;
		}

		// log.
		m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_REWARD, pcsAgpdCharacter->m_lID,
									pcsAgpdItem,
									pcsAgpdItem->m_nCount ? pcsAgpdItem->m_nCount : 1
									);

	}

	// DB
	AgpdQuest::IterQuest iterQ = pcsAgpdQuest->FindByQuestID(pQuestTemplate->m_lID);
	PVOID pBuffer[2];
	pBuffer[0] = (PVOID)AGSMQUEST_DB_CURRENTQUEST_INSERT;
	pBuffer[1] = (PVOID)pcsAgpdCharacter->m_szID;
	EnumCallback(AGSMQUEST_CB_CURRENT_QUEST, &(*iterQ), pBuffer);
	
	// Log
	if (m_pcsAgpmLog)
	{
		AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if (pcsAgsdCharacter)
		{
			INT32 lTID = ((AgpdCharacterTemplate *)pcsAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
			INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter);

			m_pcsAgpmLog->WriteLog_QuestAccept(0,
												&pcsAgsdCharacter->m_strIPAddress[0],
												pcsAgsdCharacter->m_szAccountID,
												pcsAgsdCharacter->m_szServerName,
												pcsAgpdCharacter->m_szID,
												lTID,
												lLevel,
												m_pcsAgpmCharacter->GetExp(pcsAgpdCharacter),
												pcsAgpdCharacter->m_llMoney,
												pcsAgpdCharacter->m_llBankMoney,
												pQuestTemplate->m_lID,
												NULL
												);
		}
	}

	return TRUE;
}

BOOL AgsmQuest::RequireQuest(AgpdCharacter* pcsAgpdCharacter, ApdEvent* pcsEvent, INT32 lQuestTID)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	// 퀘스트는 최대 10개까지 가능
	if (AGPDQUEST_CURRENT_ID_MAX <= pcsAgpdQuest->m_vtQuest.size())
		return FALSE;

	// 이미 해당 퀘스트를 가지고 있다면 무시
	if (pcsAgpdQuest->IsHaveQuest(lQuestTID)) return FALSE;

	// 이미 완료한 퀘스트 라면 무시
	AgpdQuestTemplate* pQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get(lQuestTID);
	if (!pQuestTemplate) 
		return FALSE;

	// 반복퀘스트의 경우 수령했는지 확인하지 않는다.
	if (pQuestTemplate->m_eQuestType != AGPDQUEST_TYPE_REPEAT )
	{
		if (pcsAgpdQuest->m_csFlag.GetValueByDBID(pQuestTemplate->m_lID))
			return FALSE;
	}

	// 인벤토리 및 퀘스트 인벤토리가 가득차 있는지 확인
	AgpdGrid *pInventory = m_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
	if (!pInventory) return FALSE;

	// 퀘스트를 시작 할 때 아이템을 부여하는게 아니라면 확인할 필요가 없다.
	if (NULL != pQuestTemplate->m_PreStart.Item.pItemTemplate)
	{
		if (m_pcsAgpmGrid->IsFullGrid(pInventory))
		{
			SendPacketInventoryFull(pcsAgpdCharacter->m_lID, lQuestTID);
			return FALSE;
		}
	}

	// 아이템이면 수락하기전에 한번 확인 과정이 필요하다.
	if( pcsEvent && pcsEvent->m_pcsSource&& pcsEvent->m_pcsSource->m_eType == APBASE_TYPE_ITEM )
	{
		AgpdItem * pcsItem = m_pcsAgpmQuest->m_pcsAgpmItem->GetItem( pcsEvent->m_pcsSource->m_lID  );

		// 퀘스트 창을 띄우고 다른사람에게 넘긴 경우가 되것다.
		if( pcsItem && pcsItem->m_pcsCharacter != pcsAgpdCharacter )
			return FALSE;
	}

	// 자동부여 퀘스트라면 퀘스트 시작
	if (pQuestTemplate->m_eQuestType == AGPDQUEST_TYPE_AUTOSTART)
		return StartQuest(pcsAgpdCharacter, pQuestTemplate);

	if (NULL == pcsEvent)
	{
		return FALSE;
	}

	// Object가 부여해준 퀘스트
	AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)pcsEvent->m_pvData;
	if (!pcsAttachData) 
		return FALSE;

	// Object가 가지고 있는 Quest Group을 이용하여 평가
	AgpdQuestGroup* pQuestGroup = m_pcsAgpmQuest->m_csQuestGroup.Get(pcsAttachData->lQuestGroupID);
	if (!pQuestGroup) 
		return FALSE;

	// 퀘스트를 시작할수 있는지 확인
	list<INT32>::iterator iter;
	for (iter = pQuestGroup->m_GrantQuest.begin(); iter != pQuestGroup->m_GrantQuest.end(); ++iter)
	{
		if (*iter == lQuestTID)
		{
			if (!StartQuest(pcsAgpdCharacter, pQuestTemplate))
				return FALSE;
		}
	}

	// 새퀘스트시스템
	// 이부분에서 pcsEvent->pcsSource 의 Type이 Item인 경우..
	// 수락했으므로 해당 아이템을 제거해줘야함.
	if( pcsEvent->m_pcsSource->m_eType == APBASE_TYPE_ITEM )
	{
		AgpdItem * pcsItem = m_pcsAgpmQuest->m_pcsAgpmItem->GetItem( pcsEvent->m_pcsSource->m_lID  );

		if( pcsItem && pcsItem->m_pcsCharacter == pcsAgpdCharacter )
		{
			// 아이템이 존재하고 , 본인 소유일경우.. 아이템 제거
			m_pcsAgpmQuest->m_pcsAgpmItem->RemoveItem( pcsEvent->m_pcsSource->m_lID , TRUE);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL AgsmQuest::CompleteQuest(AgpdCharacter* pcsAgpdCharacter, AgpdQuestTemplate* pQuestTemplate)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	// 퀘스트를 완료할수 있는 조건이 충족되었다면 보상해준다.
	if (!m_pcsAgpmQuest->EvaluationCompleteCondition(pcsAgpdCharacter, pQuestTemplate->m_lID))
		return FALSE;

	AgpdQuest::IterQuest iterQ = pcsAgpdQuest->FindByQuestID(pQuestTemplate->m_lID);
	if (iterQ != pcsAgpdQuest->m_vtQuest.end())
	{
		// 보상으로 아이템을 줄때 인벤토리가 가득차 있다면 메세지를 뿌려주고 종료한다.
		INT32 lItemCount = 0;
		if (pQuestTemplate->m_Result.Item1.pItemTemplate) ++lItemCount;
		if (pQuestTemplate->m_Result.Item2.pItemTemplate) ++lItemCount;
		if (pQuestTemplate->m_Result.Item3.pItemTemplate) ++lItemCount;
		if (pQuestTemplate->m_Result.Item4.pItemTemplate) ++lItemCount;

		// 보상 아이템의 수량만큼의 인벤토리 공간이 있어야 퀘스트를 완료할 수 있다.
		if (0 < lItemCount)
		{
			AgpdGrid *pInventory = m_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
			if (!pInventory) return FALSE;
			
			if (pInventory->m_lGridCount - pInventory->m_lItemCount < lItemCount)
			{
				SendPacketInventoryFull(pcsAgpdCharacter->m_lID, pQuestTemplate->m_lID);
				return FALSE;
			}
		}

		// DB Delete
		PVOID pBuffer[2];
		pBuffer[0] = (PVOID)AGSMQUEST_DB_CURRENTQUEST_DELETE;
		pBuffer[1] = (PVOID)pcsAgpdCharacter->m_szID;
		EnumCallback(AGSMQUEST_CB_CURRENT_QUEST, &(*iterQ), pBuffer);

		if( pQuestTemplate->m_eQuestType == AGPDQUEST_TYPE_REPEAT )
			pcsAgpdQuest->m_csFlag.SetValueByDBID(pQuestTemplate->m_lID, FALSE);
		else
			pcsAgpdQuest->m_csFlag.SetValueByDBID(pQuestTemplate->m_lID, TRUE);

		SendPacketQuestCompleteResult(pcsAgpdCharacter->m_lID, pQuestTemplate->m_lID, TRUE);

		// 아이템 삭제 및 생성 진행
		QuestResult(pcsAgpdCharacter, pQuestTemplate);

		// DB Update
		pBuffer[0] = (PVOID)AGSMQUEST_DB_DATA_FLAG_UPDATE;
		pBuffer[1] = (PVOID)pcsAgpdCharacter->m_szID;
		EnumCallback(AGSMQUEST_CB_FLAG, pcsAgpdQuest, pBuffer);

		pcsAgpdQuest->m_vtQuest.erase(iterQ);

		m_pcsAgsmTitle->QuestComplete(pcsAgpdCharacter);
		
		// Log3
		if (m_pcsAgpmLog)
		{
			AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
			if (pcsAgsdCharacter)
			{
				INT32 lTID = ((AgpdCharacterTemplate *)pcsAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
				INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter);
				
				m_pcsAgpmLog->WriteLog_QuestComplete(0,
													&pcsAgsdCharacter->m_strIPAddress[0],
													pcsAgsdCharacter->m_szAccountID,
													pcsAgsdCharacter->m_szServerName,
													pcsAgpdCharacter->m_szID,
													lTID,
													lLevel,
													m_pcsAgpmCharacter->GetExp(pcsAgpdCharacter),
													pcsAgpdCharacter->m_llMoney,
													pcsAgpdCharacter->m_llBankMoney,
													pQuestTemplate->m_lID,
													NULL
													);
			}
		}
	}

	return TRUE;
}

BOOL AgsmQuest::RequireComplete(AgpdCharacter* pcsAgpdCharacter, ApdEvent* pcsEvent, INT32 lQuestTID)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	if (!pcsEvent)
		return FALSE;

	// 해당 퀘스트를 가지고 있지 않다면 무시
	if (!pcsAgpdQuest->IsHaveQuest(lQuestTID))
		return FALSE;

	// 아이템이면 완료하기전에 한번 확인 과정이 필요하다.
	if( pcsEvent->m_pcsSource->m_eType == APBASE_TYPE_ITEM )
	{
		AgpdItem * pcsItem = m_pcsAgpmQuest->m_pcsAgpmItem->GetItem( pcsEvent->m_pcsSource->m_lID  );

		// 퀘스트 창을 띄우고 다른사람에게 넘긴 경우가 되것다.
		if( pcsItem && pcsItem->m_pcsCharacter != pcsAgpdCharacter )
			// 아이템이 존재하지 않음을 알려줄 필요가 있다.
			return FALSE;
	}

	AgpdQuestTemplate* pQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get(lQuestTID);
	if (!pQuestTemplate)
		return FALSE;

	if (pQuestTemplate->m_eQuestType == AGPDQUEST_TYPE_AUTOSTART)
		return CompleteQuest(pcsAgpdCharacter, pQuestTemplate);

	if (NULL == pcsEvent)
	{
		return FALSE;
	}

	if( (APBASE_TYPE_CHARACTER == pcsEvent->m_pcsSource->m_eType) || (APBASE_TYPE_OBJECT == pcsEvent->m_pcsSource->m_eType) )//문엘프퀘스트	AGSDQUEST_EXPAND_BLOCK
	{
		AgpdCharacter* pNPC = m_pcsAgpmQuest->m_pcsAgpmCharacter->GetCharacter(pcsEvent->m_pcsSource->m_lID);

		// NPC가 부여해준 퀘스트
		AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)pcsEvent->m_pvData;
		if (!pcsAttachData) 
			return FALSE;

		// NPC가 가지고 있는 Quest Group을 이용하여 평가
		AgpdQuestGroup* pQuestGroup = m_pcsAgpmQuest->m_csQuestGroup.Get(pcsAttachData->lQuestGroupID);
		if (!pQuestGroup) 
			return FALSE;

		// 퀘스트를 완료할수 있는지 확인
		list<INT32>::iterator iter;
		for (iter = pQuestGroup->m_EstimateQuest.begin(); iter != pQuestGroup->m_EstimateQuest.end(); ++iter)
		{
			if (*iter == lQuestTID)
			{
				if (!CompleteQuest(pcsAgpdCharacter, pQuestTemplate))
					return FALSE;
			}
		}
	}
	else if( APBASE_TYPE_ITEM == pcsEvent->m_pcsSource->m_eType )
	{
		// Item 이 부여한 퀘스트
		AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)pcsEvent->m_pvData;
		if (!pcsAttachData) 
			return FALSE;

		// Item이 가지고 있는 Quest Group을 이용하여 평가
		AgpdQuestGroup* pQuestGroup = m_pcsAgpmQuest->m_csQuestGroup.Get(pcsAttachData->lQuestGroupID);
		if (!pQuestGroup) 
			return FALSE;

		// 퀘스트를 완료할수 있는지 확인
		list<INT32>::iterator iter;
		for (iter = pQuestGroup->m_EstimateQuest.begin(); iter != pQuestGroup->m_EstimateQuest.end(); ++iter)
		{
			if (*iter == lQuestTID)
			{
				if (!CompleteQuest(pcsAgpdCharacter, pQuestTemplate))
					return FALSE;
			}
		}

		// 여기서 아이템 삭제.
		m_pcsAgpmQuest->m_pcsAgpmItem->RemoveItem( pcsEvent->m_pcsSource->m_lID , TRUE);
	}

	return TRUE;
}

BOOL AgsmQuest::SendPacketQuestRequireResult(INT32 lCID, INT32 lQuestTID, BOOL bResult, AgpdQuest* pcsAgpdQuest)
{
	INT16 nPacketLength = 0;
	AgpdQuest::IterQuest iter = pcsAgpdQuest->FindByQuestID(lQuestTID);

	if (iter == pcsAgpdQuest->m_vtQuest.end()) return FALSE;

	PVOID pvPacketQuest = m_pcsAgpmQuest->MakePacketEmbeddedQuest((*iter).lQuestID, 
													(*iter).lParam1, (*iter).lParam2, 0, (*iter).szNPCName);
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketRequireQuestResult(&nPacketLength, lCID, bResult, lQuestTID, pvPacketQuest);

	if (!pvPacket)
	{
		m_pcsAgpmQuest->m_csPacketQuest.FreePacket(pvPacketQuest);
		return FALSE;
	}

	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bPacketResult;
}

BOOL AgsmQuest::SendPacketUpdateQuest(INT32 lCID, INT32 lQuestTID, INT32 lItemTID, AgpdCurrentQuest* pcsAgpdCurrentQuest)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketUpdateQuest(&nPacketLength, lCID, lQuestTID, lItemTID, pcsAgpdCurrentQuest);

	if (!pvPacket)
	{
		m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bPacketResult;
}

BOOL AgsmQuest::SendPacketQuestCompleteResult(INT32 lCID, INT32 lQuestTID, BOOL bResult)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketQuestCompleteResult(&nPacketLength, lCID, bResult, lQuestTID);

	if (!pvPacket)
		return FALSE;

	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bPacketResult;
}

BOOL AgsmQuest::SendPacketQuestCancelResult(INT32 lCID, INT32 lQuestID, BOOL bResult)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketQuestCancelResult(&nPacketLength, lCID, bResult, lQuestID);

	if (!pvPacket)
		return FALSE;
	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bPacketResult;
}

BOOL AgsmQuest::SendPacketInventoryFull(INT32 lCID, INT32 lQuestID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketInventoryFull(&nPacketLength, lCID, lQuestID);

	if (!pvPacket)
		return FALSE;

	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bPacketResult;
}

BOOL AgsmQuest::SendPacketQuestInventoryFull(INT32 lCID, INT32 lQuestID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketQuestInventoryFull(&nPacketLength, lCID, lQuestID);

	if (!pvPacket)
		return FALSE;

	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bPacketResult;
}

BOOL AgsmQuest::SendQuestDataAtLogin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmQuest		*pThis			= (AgsmQuest *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	AgpdQuest* pAgpdQuest = pThis->m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);
	if (!pAgpdQuest)
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmQuest->MakePacketInitFlag(&nPacketLength, pcsCharacter->m_lID, pAgpdQuest);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, ulNID);
	pThis->m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	for( AgpdQuest::IterQuest iter = pAgpdQuest->m_vtQuest.begin(); iter != pAgpdQuest->m_vtQuest.end(); ++iter)
	{
		pvPacket = pThis->m_pcsAgpmQuest->MakePacketInitQuest(&nPacketLength, pcsCharacter->m_lID, &(*iter));

		if (!pvPacket || nPacketLength < 1)
			break;

		pThis->SendPacket(pvPacket, nPacketLength, ulNID);
		pThis->m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}

BOOL AgsmQuest::SendQuestDataAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmQuest		*pThis			= (AgsmQuest *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	AgpdQuest* pAgpdQuest = pThis->m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);
	if (!pAgpdQuest)
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmQuest->MakePacketInitFlag(&nPacketLength, pcsCharacter->m_lID, pAgpdQuest);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	// 완료된 퀘스트 플래그 전송
	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, ulNID);
	pThis->m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	// 진행중인 퀘스트 정보 전송
	for( AgpdQuest::IterQuest iter = pAgpdQuest->m_vtQuest.begin(); iter != pAgpdQuest->m_vtQuest.end(); ++iter)
	{
		pvPacket = pThis->m_pcsAgpmQuest->MakePacketInitQuest(&nPacketLength, pcsCharacter->m_lID, &(*iter));

		if (!pvPacket || nPacketLength < 1)
			break;

		pThis->SendPacket(pvPacket, nPacketLength, ulNID);
		pThis->m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}

BOOL AgsmQuest::QuestResult(AgpdCharacter* pcsAgpdCharacter, AgpdQuestTemplate* pcsQuestTemplate)
{
	AgpdItemADChar* pcsAgpdItemADChar = m_pcsAgpmQuest->m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	if (!pcsAgpdItemADChar) return FALSE;

	// 완료조건에 아이템이 있다면 삭제한다.
	if (0 != pcsQuestTemplate->m_CompleteCondition.Item.pItemTemplate)
	{
		AgpdGridItem* pcsAgpdGridItem;
		AgpdItem *pAgpdItem;

		INT32 lIndex = 0;
		INT32 lCount = pcsQuestTemplate->m_CompleteCondition.Item.lCount;
		while (pcsAgpdGridItem = m_pcsAgpmGrid->GetItemByTemplate(lIndex, &pcsAgpdItemADChar->m_csInventoryGrid, AGPDGRID_ITEM_TYPE_ITEM, 
																	pcsQuestTemplate->m_CompleteCondition.Item.pItemTemplate->m_lID))
		{
			pAgpdItem = m_pcsAgpmQuest->m_pcsAgpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			if (!pAgpdItem) continue;

			if (((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_bStackable)
			{
				if (pAgpdItem->m_nCount < lCount)
				{
					// pAgpdItem이 목표수량보다 적다면 수량만큼 없앤다.
					lCount -= pAgpdItem->m_nCount;
					
					// log. 아이템이 사라질 수 있어 미리 남긴다.
					m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
												pAgpdItem,
												pAgpdItem->m_nCount
												);					
					
					m_pcsAgpmQuest->m_pcsAgpmItem->SubItemStackCount(pAgpdItem, pAgpdItem->m_nCount);
				}
				else
				{
					// log. 아이템이 사라질 수 있어 미리 남긴다.
					m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
												pAgpdItem,
												lCount
												);
				
					m_pcsAgpmQuest->m_pcsAgpmItem->SubItemStackCount(pAgpdItem, lCount);
					lCount = 0;
				}
			}
			else
			{
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
											pAgpdItem,
											pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
											);
			
				--lCount;
				m_pcsAgpmQuest->m_pcsAgpmItem->RemoveItem(pAgpdItem, TRUE);
			}

			if (0 >= lCount) break;
		}
	}

	// Check Point에 있는 Item을 삭제한다.
	for (INT32 i = 0; i < AGPDQUEST_MAX_CHECKPOINT; ++i)
	{
		AgpdGridItem* pcsAgpdGridItem;
		AgpdItem *pAgpdItem;

		if (!pcsQuestTemplate->m_CheckPoint.CheckPointItem[i].pItemTemplate)
			continue;

		INT32 lIndex = 0;
		INT32 lCount = pcsQuestTemplate->m_CheckPoint.CheckPointItem[i].lItemCount;

		while (pcsAgpdGridItem = m_pcsAgpmGrid->GetItemByTemplate(lIndex, &pcsAgpdItemADChar->m_csInventoryGrid, AGPDGRID_ITEM_TYPE_ITEM, 
			pcsQuestTemplate->m_CheckPoint.CheckPointItem[i].pItemTemplate->m_lID))
		{
			pAgpdItem = m_pcsAgpmQuest->m_pcsAgpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
			if (!pAgpdItem) continue;
			if (!m_pcsAgpmItem->IsQuestItem(pAgpdItem)) continue;

			if (((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_bStackable)
			{
				if (pAgpdItem->m_nCount < lCount)
				{		
					// log. 아이템이 사라질 수 있어 미리 남긴다.
					m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
												pAgpdItem,
												pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
												);

					// pAgpdItem이 목표수량보다 적다면 수량만큼 없앤다.
					lCount -= pAgpdItem->m_nCount;
					m_pcsAgpmQuest->m_pcsAgpmItem->SubItemStackCount(pAgpdItem, pAgpdItem->m_nCount);
				}
				else
				{
					// log. 아이템이 사라질 수 있어 미리 남긴다.
					m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
												pAgpdItem,
												lCount
												);
																
					m_pcsAgpmQuest->m_pcsAgpmItem->SubItemStackCount(pAgpdItem, lCount);
					lCount = 0;
				}
			}
			else
			{
				// log. 아이템이 사라질 수 있어 미리 남긴다.
				m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_PAY, pcsAgpdCharacter->m_lID,
											pAgpdItem,
											pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
											);
			
				--lCount;
				m_pcsAgpmQuest->m_pcsAgpmItem->RemoveItem(pAgpdItem, TRUE);
			}

			if (0 >= lCount) break;
		}
	}

	if (0 != pcsQuestTemplate->m_Result.lExp)
	{
		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;

		INT32 lExp = pcsQuestTemplate->m_Result.lExp;

		m_pcsAgpmTitle->GetEffectValue(pcsAgpdCharacter, AGPMTITLE_TITLE_EFFECT_QUEST_BONUS_EXP, &nEffectSet, &nEffectValue1, &nEffectValue2);
		
		if(!nEffectSet)
			lExp = lExp + (INT32)((FLOAT)lExp * (FLOAT)(nEffectValue1) / 100.0f);
		else
			lExp = lExp + nEffectValue1;

		m_pcsAgsmDeath->AddBonusExpToChar(pcsAgpdCharacter, NULL, lExp, TRUE, FALSE, TRUE);
	}

	if (0 != pcsQuestTemplate->m_Result.lMoney)
	{
		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;

		INT32 lMoney = pcsQuestTemplate->m_Result.lMoney;

		m_pcsAgpmTitle->GetEffectValue(pcsAgpdCharacter, AGPMTITLE_TITLE_EFFECT_QUEST_BONUS_GHELD, &nEffectSet, &nEffectValue1, &nEffectValue2);

		if(!nEffectSet)
			lMoney = lMoney + (INT32)((FLOAT)lMoney * (FLOAT)(nEffectValue1) / 100.0f);
		else
			lMoney = lMoney + nEffectValue1;

		m_pcsAgpmQuest->m_pcsAgpmCharacter->AddMoney(pcsAgpdCharacter, lMoney);
		m_pcsAgsmItem->SendPacketPickupItemResult((INT8)AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS,
											1,			// llID 돈이면 없어도 된다.
											m_pcsAgpmItem->GetMoneyTID(),
											lMoney,
											m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter));
		
		// log
		m_pcsAgsmCharacter->WriteGheldLog(AGPDLOGTYPE_GHELD_QUEST_REWARD, pcsAgpdCharacter, lMoney);
	}

	QuestResultMakeItem(&pcsQuestTemplate->m_Result.Item1, pcsAgpdCharacter);
	QuestResultMakeItem(&pcsQuestTemplate->m_Result.Item2, pcsAgpdCharacter);
	QuestResultMakeItem(&pcsQuestTemplate->m_Result.Item3, pcsAgpdCharacter);
	QuestResultMakeItem(&pcsQuestTemplate->m_Result.Item4, pcsAgpdCharacter);

	if (NULL != pcsQuestTemplate->m_Result.ProductSkill.pSkillTemplate)
	{
		m_pcsAgpmSkill->LearnSkill(pcsAgpdCharacter, pcsQuestTemplate->m_Result.ProductSkill.pSkillTemplate->m_szName, 
									pcsQuestTemplate->m_Result.ProductSkill.lExp);
	}

	return TRUE;
}

BOOL AgsmQuest::QuestResultMakeItem(AgpdElementItem *pItem, AgpdCharacter *pcsAgpdCharacter)
{
	if (NULL != pItem->pItemTemplate)
	{
		// 결과 아이템 생성
		AgpdItem* pNewItem = m_pcsAgsmItemManager->CreateItem(pItem->pItemTemplate->m_lID, NULL, pItem->lCount);
		if (!pNewItem) return FALSE;

		m_pcsAgpmQuest->m_pcsAgpmItem->SetBoundType(pNewItem, pItem->m_eBoundType);

		m_pcsAgpmItemConvert->SetPhysicalConvert(pNewItem, pItem->lUpgrade, FALSE);
		m_pcsAgpmItemConvert->SetSocketConvert(pNewItem, pItem->lSocket);
		if (FALSE == m_pcsAgpmQuest->m_pcsAgpmItem->AddItemToInventory(pcsAgpdCharacter, pNewItem))
		{
			m_pcsAgpmItem->RemoveItem(pNewItem, TRUE);
			return FALSE;
		}

		// log.
		m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_QUEST_REWARD, pcsAgpdCharacter->m_lID,
									pNewItem,
									pNewItem->m_nCount
									);

		m_pcsAgsmItemConvert->SendPacketAdd(pNewItem, m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter), FALSE);
	}

	return TRUE;
}

//문엘프퀘스트 몬스터 마리수와 퀘스트아이템 동시 적용 수정	AGSDQUEST_EXPAND_BLOCK
AgsmQuestDeathResult AgsmQuest::MonsterDeathCheckItem(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsMonster, AgpdElementMonster *pcsQuestMonster, 
											 INT32 lCount, INT32 *pItemTID, AgpdParty *pcsParty, BOOL *pCreated)
{
	// 몬스터가 완료조건에 포함되어 있지 않다.
	if (!pcsQuestMonster->pMonsterTemplate) return AGSMQUEST_DEATH_FAIL;

	// 완료조건의 몬스터와 현재 죽은 몬스터가 같은지 확인
	if (pcsQuestMonster->pMonsterTemplate->m_lID != pcsMonster->m_pcsCharacterTemplate->m_lID) return AGSMQUEST_DEATH_FAIL;

	if (pcsQuestMonster->Item.pItemTemplate && (!pCreated || (FALSE == *pCreated)))
	{
		// 퀘스트 인벤토리에 해당 아이템을 목표 수량까지 가지고 있다면 무시한다.
		AgpdItemADChar* pcsAgpdItemADChar = m_pcsAgpmQuest->m_pcsAgpmItem->GetADCharacter(pcsAttacker);
		if (!pcsAgpdItemADChar) return AGSMQUEST_DEATH_FAIL;

		if (m_pcsAgpmGrid->CheckEnoughItem(&pcsAgpdItemADChar->m_csInventoryGrid, pcsQuestMonster->Item.pItemTemplate->m_lID, 
											pcsQuestMonster->Item.lCount))
			return AGSMQUEST_DEATH_ITEM_ENOUGH;

		// 아직 목표 수량을 충족하지 못했다면 주어진 확률로 아이템 드랍을 한다.
		if (m_csRand.randInt(100) < pcsQuestMonster->lDropRate)
		{
			AgpdItem* pcsAgpdItem = m_pcsAgsmItemManager->CreateItem(pcsQuestMonster->Item.pItemTemplate->m_lID, NULL, 1);

			if (!pcsAgpdItem)
				return AGSMQUEST_DEATH_FAIL;

			AgsdItem* pcsAgsdItem = m_pcsAgsmItem->GetADItem(pcsAgpdItem);
			if (!pcsAgsdItem) 
				return AGSMQUEST_DEATH_FAIL;

			pcsAgsdItem->m_ulDropTime = GetClockCount();
			m_pcsAgpmItem->SetQuestType(pcsAgpdItem);
			if (pcsParty)
			{
				pcsAgsdItem->m_csFirstLooterBase.m_eType = pcsParty->m_eType;
				pcsAgsdItem->m_csFirstLooterBase.m_lID = pcsParty->m_lID;
			}
			else
			{
				pcsAgsdItem->m_csFirstLooterBase.m_eType = pcsAttacker->m_eType;
				pcsAgsdItem->m_csFirstLooterBase.m_lID = pcsAttacker->m_lID;
			}
			
			m_pcsAgpmItem->GetNearPosition(&pcsAttacker->m_stPos, &pcsAgpdItem->m_posItem);
			m_pcsAgpmItem->AddItemToField(pcsAgpdItem);

			*pItemTID = pcsQuestMonster->Item.pItemTemplate->m_lID;
			if (pCreated)
				*pCreated = TRUE;
			return AGSMQUEST_DEATH_ITEM_CREATE;
		}
	}

	return AGSMQUEST_DEATH_NONE;
}

//문엘프퀘스트	AGSDQUEST_EXPAND_BLOCK
AgsmQuestDeathResult AgsmQuest::MonsterDeathCheckCount(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsMonster, AgpdElementMonster *pcsQuestMonster, 
											 INT32 lCount, INT32 *pItemTID, AgpdParty *pcsParty, BOOL *pCreated)
{
	// 몬스터가 완료조건에 포함되어 있지 않다.
	if (!pcsQuestMonster->pMonsterTemplate) return AGSMQUEST_DEATH_FAIL;

	// 완료조건의 몬스터와 현재 죽은 몬스터가 같은지 확인
	if (pcsQuestMonster->pMonsterTemplate->m_lID != pcsMonster->m_pcsCharacterTemplate->m_lID) return AGSMQUEST_DEATH_FAIL;

	if ( ( 0 != pcsQuestMonster->lCount ) && (!pCreated || (FALSE == *pCreated)))
	{
		// 몬스터에 연관된 아이템 정보가 없다면 몬스터 사냥 퀘스트
		// 만약 원하는 수량을 채운상태면 무시
		if (lCount >= pcsQuestMonster->lCount)
			return AGSMQUEST_DEATH_MONSTER_ENOUGH;
		else
			return AGSMQUEST_DEATH_MONSTER_INCREMENT;
	}

	return AGSMQUEST_DEATH_NONE;
}

BOOL AgsmQuest::MonsterDeathByCharacter(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsMonster, AgpdParty *pcsParty, BOOL *pCreated)
{
	AgpdQuest* pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAttacker);
	if (!pcsAgpdQuest) return FALSE;
	if (pcsAgpdQuest->m_vtQuest.empty()) return FALSE;

	AgpdQuest::IterQuest iter = pcsAgpdQuest->m_vtQuest.begin();

	while (iter != pcsAgpdQuest->m_vtQuest.end())
	{
		AgpdQuestTemplate *pcsAgpdQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get((*iter).lQuestID);
		if (!pcsAgpdQuestTemplate) 
		{
			++iter;
			continue;
		}

		AgsmQuestDeathResult eResult1, eResult2;

		INT32 lItemTID = 0;
		/*
		eResult1 = MonsterDeath(pcsAttacker, pcsMonster, &pcsAgpdQuestTemplate->m_CompleteCondition.Monster1, 
								(*iter).lParam1, &lItemTID, pcsParty, pCreated);
		eResult2 = MonsterDeath(pcsAttacker, pcsMonster, &pcsAgpdQuestTemplate->m_CompleteCondition.Monster2, 
								(*iter).lParam2, &lItemTID, pcsParty, pCreated);
			
		if (AGSMQUEST_DEATH_MONSTER_INCREMENT == eResult1)
			(*iter).lParam1++;

		if (AGSMQUEST_DEATH_MONSTER_INCREMENT == eResult2)
			(*iter).lParam2++;
		*/
		//문엘프퀘스트 추가작업	AGSDQUEST_EXPAND_BLOCK
		
		eResult1 = MonsterDeathCheckItem(pcsAttacker, pcsMonster, &pcsAgpdQuestTemplate->m_CompleteCondition.Monster1, 
								(*iter).lParam1, &lItemTID, pcsParty, pCreated);
		eResult2 = MonsterDeathCheckItem(pcsAttacker, pcsMonster, &pcsAgpdQuestTemplate->m_CompleteCondition.Monster2, 
								(*iter).lParam2, &lItemTID, pcsParty, pCreated);

		eResult1 = AGSMQUEST_DEATH_NONE;	eResult2 = AGSMQUEST_DEATH_NONE;

		eResult1 = MonsterDeathCheckCount(pcsAttacker, pcsMonster, &pcsAgpdQuestTemplate->m_CompleteCondition.Monster1, 
								(*iter).lParam1, &lItemTID, pcsParty, pCreated);
		eResult2 = MonsterDeathCheckCount(pcsAttacker, pcsMonster, &pcsAgpdQuestTemplate->m_CompleteCondition.Monster2, 
								(*iter).lParam2, &lItemTID, pcsParty, pCreated);
			
		if (AGSMQUEST_DEATH_MONSTER_INCREMENT == eResult1)
			(*iter).lParam1++;

		if (AGSMQUEST_DEATH_MONSTER_INCREMENT == eResult2)
			(*iter).lParam2++;
		//문엘프퀘스트 추가작업	AGSDQUEST_EXPAND_BLOCK

		if ((AGSMQUEST_DEATH_MONSTER_INCREMENT == eResult1) || (AGSMQUEST_DEATH_MONSTER_INCREMENT == eResult2))
		{
			// DB Update
			PVOID pBuffer[2];
			pBuffer[0] = (PVOID)AGSMQUEST_DB_CURRENTQUEST_UPDATE;
			pBuffer[1] = (PVOID)pcsAttacker->m_szID;
			EnumCallback(AGSMQUEST_CB_CURRENT_QUEST, &(*iter), pBuffer);

			// Client에도 알려준다.
			SendPacketUpdateQuest(pcsAttacker->m_lID, pcsAgpdQuestTemplate->m_lID, lItemTID, &(*iter));

			// 여러개의 퀘스트가 같은 몬스터를 지정하고 있을경우 한번의 사냥으로 여러개의 퀘스트에 영향을 줄수 있기 때문에
			// 여기서 실행을 종료시킨다.
			return TRUE;	
		}

		++iter;
	}

	return TRUE;
}

BOOL AgsmQuest::MonsterDeathByParty(AgpdParty *pcsParty, AgpdCharacter *pcsMonster)
{
	ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER> pcsCombatMember;
	pcsCombatMember.MemSetAll();

	INT32	lMemberTotalLevel	= 0;

	INT32	lNumCombatMember = m_pcsAgsmParty->GetNearMember(pcsParty, pcsMonster, &pcsCombatMember[0], &lMemberTotalLevel);
	if (lNumCombatMember < 1)
	{
		return FALSE;
	}

	// 아이템을 한번만 생성하기 위해서 사용
	BOOL bCreatedItem = FALSE;

	for (int i = 0; i < lNumCombatMember; ++i)
	{
		MonsterDeathByCharacter(pcsCombatMember[i], pcsMonster, pcsParty, &bCreatedItem);
	}

	return TRUE;
}

BOOL AgsmQuest::CBMonsterDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmQuest *pThis		= (AgsmQuest*)pClass;
	ApBase *pcsBaseMonster	= (ApBase*)pData;
	ApBase *pcsBase			= (ApBase*)pCustData;

	if (APBASE_TYPE_CHARACTER != pcsBaseMonster->m_eType) return FALSE;

	if (APBASE_TYPE_CHARACTER == pcsBase->m_eType)
	{
		// 캐릭터가 몬스터를 죽였을때의 퀘스트 처리
		if (pThis->m_pcsAgpmCharacter->IsPC((AgpdCharacter*)pcsBase) && 
			pThis->m_pcsAgpmCharacter->IsMonster((AgpdCharacter*)pcsBaseMonster))
			pThis->MonsterDeathByCharacter((AgpdCharacter*)pcsBase, (AgpdCharacter*)pcsBaseMonster);
	}
	else if (APBASE_TYPE_PARTY == pcsBase->m_eType)
	{
		// 파티가 몬스터를 죽였을때의 퀘스트 처리
		if (pThis->m_pcsAgpmCharacter->IsMonster((AgpdCharacter*)pcsBaseMonster))
			pThis->MonsterDeathByParty((AgpdParty*)pcsBase, (AgpdCharacter*)pcsBaseMonster);
	}

	return FALSE;	
}

AgsmQuestDeathResult AgsmQuest::MonsterDeath(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsMonster, AgpdElementMonster *pcsQuestMonster, 
											 INT32 lCount, INT32 *pItemTID, AgpdParty *pcsParty, BOOL *pCreated)
{
	// 몬스터가 완료조건에 포함되어 있지 않다.
	if (!pcsQuestMonster->pMonsterTemplate) return AGSMQUEST_DEATH_FAIL;

	// 완료조건의 몬스터와 현재 죽은 몬스터가 같은지 확인
	if (pcsQuestMonster->pMonsterTemplate->m_lID != pcsMonster->m_pcsCharacterTemplate->m_lID) return AGSMQUEST_DEATH_FAIL;

	if (pcsQuestMonster->Item.pItemTemplate && (!pCreated || (FALSE == *pCreated)))
	{
		// 퀘스트 인벤토리에 해당 아이템을 목표 수량까지 가지고 있다면 무시한다.
		AgpdItemADChar* pcsAgpdItemADChar = m_pcsAgpmQuest->m_pcsAgpmItem->GetADCharacter(pcsAttacker);
		if (!pcsAgpdItemADChar) return AGSMQUEST_DEATH_FAIL;

		if (m_pcsAgpmGrid->CheckEnoughItem(&pcsAgpdItemADChar->m_csInventoryGrid, pcsQuestMonster->Item.pItemTemplate->m_lID, 
											pcsQuestMonster->Item.lCount))
			return AGSMQUEST_DEATH_ITEM_ENOUGH;

		// 아직 목표 수량을 충족하지 못했다면 주어진 확률로 아이템 드랍을 한다.
		if (m_csRand.randInt(100) < pcsQuestMonster->lDropRate)
		{
			AgpdItem* pcsAgpdItem = m_pcsAgsmItemManager->CreateItem(pcsQuestMonster->Item.pItemTemplate->m_lID, NULL, 1);

			if (!pcsAgpdItem)
				return AGSMQUEST_DEATH_FAIL;

			AgsdItem* pcsAgsdItem = m_pcsAgsmItem->GetADItem(pcsAgpdItem);
			if (!pcsAgsdItem) 
				return AGSMQUEST_DEATH_FAIL;

			pcsAgsdItem->m_ulDropTime = GetClockCount();
			m_pcsAgpmItem->SetQuestType(pcsAgpdItem);
			if (pcsParty)
			{
				pcsAgsdItem->m_csFirstLooterBase.m_eType = pcsParty->m_eType;
				pcsAgsdItem->m_csFirstLooterBase.m_lID = pcsParty->m_lID;
			}
			else
			{
				pcsAgsdItem->m_csFirstLooterBase.m_eType = pcsAttacker->m_eType;
				pcsAgsdItem->m_csFirstLooterBase.m_lID = pcsAttacker->m_lID;
			}
			
			m_pcsAgpmItem->GetNearPosition(&pcsAttacker->m_stPos, &pcsAgpdItem->m_posItem);
			m_pcsAgpmItem->AddItemToField(pcsAgpdItem);

			*pItemTID = pcsQuestMonster->Item.pItemTemplate->m_lID;
			if (pCreated)
				*pCreated = TRUE;
			return AGSMQUEST_DEATH_ITEM_CREATE;
		}
	}
	else
	{
		// 몬스터에 연관된 아이템 정보가 없다면 몬스터 사냥 퀘스트
		// 만약 원하는 수량을 채운상태면 무시
		if (lCount >= pcsQuestMonster->lCount)
			return AGSMQUEST_DEATH_MONSTER_ENOUGH;
		else
			return AGSMQUEST_DEATH_MONSTER_INCREMENT;
	}

	return AGSMQUEST_DEATH_NONE;
}

BOOL AgsmQuest::CBChatQuestComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter*	pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmQuest*		pThis = (AgsmQuest*)pClass;
	INT32			lQuestTID = PtrToInt(pCustData);

	pThis->CompleteQuestByChatting(pcsAgpdCharacter, lQuestTID);
	
	return TRUE;
}

BOOL AgsmQuest::CompleteQuestByChatting(AgpdCharacter* pcsAgpdCharacter, INT32 lQuestTID)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	AgpdQuest::IterQuest iterQ = pcsAgpdQuest->FindByQuestID(lQuestTID);
	if (iterQ == pcsAgpdQuest->m_vtQuest.end())
		return FALSE;

	AgpdQuestTemplate* pQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get(lQuestTID);
	if (!pQuestTemplate)
		return FALSE;

	// 보상으로 아이템을 줄때 인벤토리가 가득차 있다면 메세지를 뿌려주고 종료한다.
	INT32 lItemCount = 0;
	if (pQuestTemplate->m_Result.Item1.pItemTemplate) ++lItemCount;
	if (pQuestTemplate->m_Result.Item2.pItemTemplate) ++lItemCount;
	if (pQuestTemplate->m_Result.Item3.pItemTemplate) ++lItemCount;
	if (pQuestTemplate->m_Result.Item4.pItemTemplate) ++lItemCount;

	// 보상 아이템의 수량만큼의 인벤토리 공간이 있어야 퀘스트를 완료할 수 있다.
	if (0 < lItemCount)
	{
		AgpdGrid *pInventory = m_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
		if (!pInventory) return FALSE;
		
		if (pInventory->m_lGridCount - pInventory->m_lItemCount < lItemCount)
		{
			SendPacketInventoryFull(pcsAgpdCharacter->m_lID, lQuestTID);
			return FALSE;
		}
	}

	// DB Delete
	PVOID pBuffer[2];
	pBuffer[0] = (PVOID)AGSMQUEST_DB_CURRENTQUEST_DELETE;
	pBuffer[1] = (PVOID)pcsAgpdCharacter->m_szID;
	EnumCallback(AGSMQUEST_CB_CURRENT_QUEST, &(*iterQ), pBuffer);

	pcsAgpdQuest->m_csFlag.SetValueByDBID(pQuestTemplate->m_lID, TRUE);
	SendPacketQuestCompleteResult(pcsAgpdCharacter->m_lID, lQuestTID, TRUE);

	// 아이템 삭제 및 생성 진행
	QuestResult(pcsAgpdCharacter, pQuestTemplate);

	// DB Update
	pBuffer[0] = (PVOID)AGSMQUEST_DB_DATA_FLAG_UPDATE;
	pBuffer[1] = (PVOID)pcsAgpdCharacter->m_szID;
	EnumCallback(AGSMQUEST_CB_FLAG, pcsAgpdQuest, pBuffer);

	pcsAgpdQuest->m_vtQuest.erase(iterQ);
	
	// Log
	if (m_pcsAgpmLog)
	{
		AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if (pcsAgsdCharacter)
		{
			INT32 lTID = ((AgpdCharacterTemplate *)pcsAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
			INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter);

			m_pcsAgpmLog->WriteLog_QuestComplete(0,
												&pcsAgsdCharacter->m_strIPAddress[0],
												pcsAgsdCharacter->m_szAccountID,
												pcsAgsdCharacter->m_szServerName,
												pcsAgpdCharacter->m_szID,
												lTID,
												lLevel,
												m_pcsAgpmCharacter->GetExp(pcsAgpdCharacter),
												pcsAgpdCharacter->m_llMoney,
												pcsAgpdCharacter->m_llBankMoney,
												lQuestTID,
												NULL
												);
		}
	}
	
	return TRUE;
}

BOOL AgsmQuest::RequireCheckPoint(AgpdCharacter* pcsAgpdCharacter, ApdEvent* pcsEvent, INT32 lQuestTID, INT32 lCheckPointIndex)
{
	if (NULL == pcsEvent) return FALSE;

	AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)pcsEvent->m_pvData;
	if (!pcsAttachData) 
		return FALSE;

	AgpdQuestGroup* pQuestGroup = m_pcsAgpmQuest->m_csQuestGroup.Get(pcsAttachData->lQuestGroupID);
	if (!pQuestGroup) 
		return FALSE;

	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	// CheckPoint가 설정되어 있으면 CheckPoint 기능을 먼저 실행한다.
	if (!pQuestGroup->m_CheckPoint.empty())
		return CheckPointWork(pcsAgpdCharacter, pQuestGroup, pcsAgpdQuest, lQuestTID, lCheckPointIndex);

	return FALSE;
}

BOOL AgsmQuest::QuestItemPickupValidCheck(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem)
{
	AgpdQuest *pcsAgpdQuest = m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsAgpdCharacter);
	if (!pcsAgpdQuest)
		return FALSE;

	for (AgpdQuest::IterQuest iter = pcsAgpdQuest->m_vtQuest.begin(); iter != pcsAgpdQuest->m_vtQuest.end(); ++iter)
	{
		AgpdQuestTemplate* pcsQuestTemplate = m_pcsAgpmQuest->m_csQuestTemplate.Get(iter->lQuestID);
		if (!pcsQuestTemplate)
			return FALSE;

		if (pcsQuestTemplate->m_CompleteCondition.Monster1.Item.pItemTemplate)
		{
			if (pcsQuestTemplate->m_CompleteCondition.Monster1.Item.pItemTemplate->m_lID == 
				((AgpdItemTemplate*)pcsAgpdItem->m_pcsItemTemplate)->m_lID)
				return TRUE;
		}

		if (pcsQuestTemplate->m_CompleteCondition.Monster2.Item.pItemTemplate)
		{
			if (pcsQuestTemplate->m_CompleteCondition.Monster2.Item.pItemTemplate->m_lID == 
				((AgpdItemTemplate*)pcsAgpdItem->m_pcsItemTemplate)->m_lID)
				return TRUE;
		}
	}

	return FALSE;
}