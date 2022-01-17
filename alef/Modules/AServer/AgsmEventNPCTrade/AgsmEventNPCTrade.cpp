//2003.02.27 -Ashulam-

#include "AgsmEventNPCTrade.h"

#include <stdio.h>
#include "ApMemoryTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmEventNPCTrade::AgsmEventNPCTrade()
{
	SetModuleName("AgsmEventNPCTrade");

	SetPacketType(AGPMEVENT_NPCTRADE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_PACKET,	1,		// event
							AUTYPE_INT8,	1,		// operation
							AUTYPE_INT32,	1,		// CID
							AUTYPE_INT32,	1,		// IID
							AUTYPE_PACKET,	1,		// detail info
							AUTYPE_INT32,	1,		// timestamp		가격변동등 파는 물건에 변화가 있을때 이값이 증가된다.
													//                이 값으로 아이템 가격을 새로 받을지 결정한다.
							AUTYPE_INT32,	1,      // result
							AUTYPE_INT8,	1,		// layer
							AUTYPE_INT8,	1,		// row
							AUTYPE_INT8,	1,		// column
							AUTYPE_END,		0
							);

	m_csDetailInfo.SetFlagLength(sizeof(INT8));
	m_csDetailInfo.SetFieldType(AUTYPE_INT32,	1,		// Item TID		: TemplateID
							AUTYPE_INT32,	1,		// Item Price	: 아이템 가격.
							AUTYPE_INT32,	1,		// Item Count	: 아이템 수량.
							AUTYPE_INT16,	1,		// Stauts		: Status
							AUTYPE_INT16,	1,		// Layer		: Layer
							AUTYPE_INT16,	1,		// Row			: Row
							AUTYPE_INT16,	1,		// Column		: Column
							AUTYPE_END,		0
							);
}

AgsmEventNPCTrade::~AgsmEventNPCTrade()
{
}

BOOL AgsmEventNPCTrade::OnAddModule()
{
	// 상위 Module들 가져온다.
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmItemConvert = (AgpmItemConvert *) GetModule( "AgpmItemConvert" );
	m_pcsAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");
	m_pcsAgpmGuild = (AgpmGuild *) GetModule("AgpmGuild");
	m_pcsAgpmEventNPCTrade = (AgpmEventNPCTrade *) GetModule("AgpmEventNPCTrade");
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");

	m_pcsAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pcsAgsmItem = (AgsmItem *) GetModule("AgsmItem");
	m_pcsAgsmItemManager = (AgsmItemManager *) GetModule( "AgsmItemManager" );

	m_pcsAgpmSystemMessage = (AgpmSystemMessage *) GetModule("AgpmSystemMessage");
	m_pcsAgpmLog = (AgpmLog *) GetModule("AgpmLog");

	if (!m_pcsAgpmFactors || !m_pcsApmEventManager || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgsmItem ||
		!m_pcsAgpmGuild || !m_pcsAgpmEventNPCTrade ||
		!m_pcsAgpmItemConvert || !m_pcsAgpmLog)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventNPCTrade(CBActionNPCTrade, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventNPCTrade::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	// m_csPacket
	PVOID				pvEventBase = NULL;
	INT8				cOperation;
	INT32				lCID = AP_INVALID_CID;
	INT32				lIID = AP_INVALID_IID;
	PVOID				pvDetailInfo = NULL;
	INT32				lTimeStamp;
	INT32				lResult;

	ApdEvent			*pstEvent = NULL;

	INT8				cLayer		= (-1);
	INT8				cRow		= (-1);
	INT8				cColumn		= (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize,  
						&pvEventBase, 
						&cOperation,
						&lCID,
						&lIID,
						&pvDetailInfo, 
						&lTimeStamp,
						&lResult,
						&cLayer,
						&cRow,
						&cColumn
						);

	if (pvEventBase)
		pstEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvEventBase);

	if (lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter *pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		pcsAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation)
	{
		case AGPDEVENTNPCTRADE_REQUESTITEMLIST :
			ProcessTradeRequest(pstEvent, pcsAgpdCharacter);						//아이템리스트를 얻어낸다.
			break;
			
		case AGPDEVENTNPCTRADE_SELLITEM :
			ProcessSellItem(pstEvent, pcsAgpdCharacter, lIID, pvDetailInfo);		//아이템을 NPC에게 판다..
			break;
		
		case AGPDEVENTNPCTRADE_BUYITEM :
			ProcessBuyItem(pstEvent, pcsAgpdCharacter, lIID, pvDetailInfo, cLayer, cRow, cColumn);	//아이템을 NPC로부터 산다.
			break;
		
		default :
			break;
	}

	pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmEventNPCTrade::ProcessTradeRequest(ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter)
{
	if (!pstEvent || !pcsAgpdCharacter)
		return FALSE;

	// NPC 와의 거리를 측정하여 거래 가능 거리 안에 있으면 아이템 리스트를 보내주고
	// 거리 밖에 있다면 해당거리까지 이동한 후 아이템 리스트를 보내준다.

	AuPOS	stTargetPos;

	if (m_pcsApmEventManager->CheckValidRange(pstEvent, &pcsAgpdCharacter->m_stPos, AGPMEVENT_NPCTRADE_MAX_USE_RANGE, &stTargetPos))
	{
		// penalty check
		if (m_pcsAgpmCharacter->HasPenalty(pcsAgpdCharacter, AGPMCHAR_PENALTY_NPCTRADE))
			return FALSE;

		// stop character
		pcsAgpdCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
		if (pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

		ProcessItemList(pstEvent, pcsAgpdCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_NPC_TRADE;
		pcsAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pstEvent->m_pcsSource->m_eType;
		pcsAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pstEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

//아이템 리스트를 보내준다.
BOOL AgsmEventNPCTrade::ProcessItemList(ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter)
{
	if (NULL == pstEvent || NULL == pcsAgpdCharacter || NULL == pstEvent->m_pvData)
		return FALSE;

	AgpdEventNPCTradeData *pTradeData = (AgpdEventNPCTradeData *) pstEvent->m_pvData;
	AgpdGrid *pcsAgpdGrid = pTradeData->m_pcsGrid;
	if (!pcsAgpdGrid)
		return FALSE;
	
	for (INT16 nLayer = 0; nLayer < AGPMITEM_NPCTRADEBOX_LAYER; ++nLayer)
	{
		for (INT16 nRow = 0; nRow < AGPMITEM_NPCTRADEBOX_ROW; ++nRow)
		{
			for (INT16 nColumn = 0; nColumn < AGPMITEM_NPCTRADEBOX_COLUMN; ++nColumn)
			{
				AgpdGridItem *pcsGridItem = m_pcsAgpmGrid->GetItem(pcsAgpdGrid, nLayer, nRow, nColumn);
				if (!pcsGridItem)
					continue;
				
				INT32 lIndex = nLayer * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN;
				lIndex += (nRow * AGPMITEM_NPCTRADEBOX_COLUMN);
				lIndex += nColumn;

				if (pcsGridItem != pTradeData->m_ppcInitGridData[lIndex])
				{
					char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "ProcessItemList::GridItem was changed()\n    Character : (%s), original item(%s), changed item(%d)\n",
						pcsAgpdCharacter->m_szID,
						/*((AgpdEventNPCTradeData *) pstEvent->m_pvData)->m_ppcInitGridData[lIndex]->m_lItemTID*/ 0 ,
						pcsGridItem->m_lItemTID);
					AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff);
				}

				SendItemInfo(pcsAgpdCharacter, pstEvent, pcsGridItem->m_lItemID, nLayer, nRow, nColumn, pcsGridItem);
			}
		}
	}

	// send list ended
	SendItemInfoEnded(pcsAgpdCharacter);

	return TRUE;
}

//아이템을 NPC에게 판다.
BOOL AgsmEventNPCTrade::ProcessSellItem( ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter, INT32 lIID, PVOID pvDetailInfo )
{
	if (!pcsAgpdCharacter || !pstEvent || !pvDetailInfo)
		return FALSE;

	// npc와의 거리를 체크한다.
	AuPOS *pstBasePos = m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, NULL);
	if (pstBasePos)
	{
		FLOAT fx = pstBasePos->x - pcsAgpdCharacter->m_stPos.x;
		FLOAT fz = pstBasePos->z - pcsAgpdCharacter->m_stPos.z;

		FLOAT fDistance = (FLOAT) sqrt(fx * fx + fz * fz);

		if (!CheckUsingCashItem(pcsAgpdCharacter) &&
			fDistance > AGPMEVENT_NPCTRADE_MAX_USE_RANGE)
			return FALSE;
	}

	AgpdItemADChar* pcsAgpdItemADChar = m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
		return FALSE;

	//지정한 위치에 지정한 아이템이 있는지본다.
	INT32	lItemCount = 0;
	INT16 nStatus = -1, nLayer = -1, nRow = -1, nColumn = -1;
	m_csDetailInfo.GetField(FALSE, pvDetailInfo, 0, NULL, NULL, &lItemCount, &nStatus, &nLayer, &nRow, &nColumn);

	AgpdGrid* pcsGrid = NULL;
	AgpdItem* pcsItem = m_pcsAgpmItem->GetItem(lIID);
	if(nStatus == AGPDITEM_STATUS_SUB_INVENTORY)
		pcsGrid = &pcsAgpdItemADChar->m_csSubInventoryGrid;
	else
		pcsGrid = &pcsAgpdItemADChar->m_csInventoryGrid;

	BOOL bResult = TRUE;

	//CRequesterID의 인벤에 아이템이 있는지 확인한다.
	AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItem(pcsGrid, nLayer, nRow, nColumn );

	if( pcsGridItem && pcsGridItem->m_lItemID == lIID)
	{
		//템플릿과 돈칸에 넣을수 있는지 확인부터하자.
		if( AgpdItem* pcsAgpdItem = m_pcsAgpmItem->GetItem(lIID) )
		{
			if( AgpdItemTemplate *pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(pcsAgpdItem->m_lTID) )
			{
				AgpdEventNPCTradeData *pcsAgpdEventNPCTradeData = (AgpdEventNPCTradeData *)(pstEvent->m_pvData);
				if( pcsAgpdEventNPCTradeData )
				{
					float fBuyFunc = 0;
					/*BOOL bNPCHandleItem =*/ IsNpcHandleItem(pcsAgpdEventNPCTradeData, pcsAgpdItem->m_lTID, &fBuyFunc);

					// 아이템의 가격을 구하자
					INT32 lItemPrice = (INT32)(m_pcsAgpmItem->GetPCPrice(pcsAgpdItem) * fBuyFunc);

					if ( pcsItemTemplate->m_bStackable )
					{
						if( (lItemCount != 0) && (lItemCount <= pcsAgpdItem->m_nCount) )
						{
							lItemPrice = (INT32)(lItemPrice * lItemCount);
						}
						else
						{
							SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEM_COUNT );
							bResult = FALSE;
						}
					}

					if( lItemPrice > 0 && bResult )
					{
						//돈을 넣을 공간이 충분하다.
						if( m_pcsAgpmCharacter->CheckMoneySpace(pcsAgpdCharacter, lItemPrice) )
						{
							m_pcsAgpmCharacter->AddMoney( pcsAgpdCharacter, lItemPrice );

							//돈을 올려줬으니 아이템도 지워줘야겠지?
							if( pcsItemTemplate->m_bStackable && (lItemCount < pcsAgpdItem->m_nCount) )
							{
								WriteNPCTradeLog(pcsAgpdCharacter, pcsAgpdItem, lItemCount, lItemPrice, 0, TRUE);
								
								m_pcsAgpmItem->SubItemStackCount(pcsAgpdItem, lItemCount, TRUE);

								//다 잘 되었으니 잘 팔았다고 패킷을 쏜다~
								bResult = SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_SELL_SUCCEEDED );

								m_pcsAgpmCharacter->UpdateMoney( pcsAgpdCharacter );
							}
							else
							{
								BOOL bRemoveItem = FALSE;
								if(nStatus == AGPDITEM_STATUS_SUB_INVENTORY)
									bRemoveItem = m_pcsAgpmItem->RemoveItemFromSubInventory( pcsAgpdCharacter, pcsAgpdItem );
								else
									bRemoveItem = m_pcsAgpmItem->RemoveItemFromInventory( pcsAgpdCharacter, pcsAgpdItem );
								
								if(bRemoveItem)
								{
									WriteNPCTradeLog(pcsAgpdCharacter, pcsAgpdItem, lItemCount, lItemPrice, 0, TRUE);
								
									ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
									strncpy(pcsAgpdItem->m_szDeleteReason, "sell to merchant", AGPMITEM_MAX_DELETE_REASON);
									m_pcsAgpmItem->RemoveItem( pcsAgpdItem->m_lID, TRUE );

									//다 잘 되었으니 잘 팔았다고 패킷을 쏜다~
									bResult = SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_SELL_SUCCEEDED );

									m_pcsAgpmCharacter->UpdateMoney( pcsAgpdCharacter );
								}
								//아이템이 안지워졌으니 돈을 원상 복구
								else
								{
									m_pcsAgpmCharacter->SubMoney( pcsAgpdCharacter, lItemPrice );
								}
							}
						}
						else
						{
							SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_SELL_EXCEED_MONEY );
						}
					}
					else
					{
						SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID );
					}
				}
			}
			else
			{
				SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID );
			}
		}
		else
		{
			SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID );
		}
	}
	//팔려는 아이템이 서버상에 존재하지 않는다. 해킹?
	else
	{
		SendSellResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID );
	}
	
	return bResult;
}

//NPC로부터 아이템을 산다.
BOOL AgsmEventNPCTrade::ProcessBuyItem( ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter, INT32 lIID, PVOID pvDetailInfo, INT8 cLayer, INT8 cRow, INT8 cColumn )
{
	AgpdGrid				*pcsItemGrid;
	AgpdItem				*pcsAgpdItem;
	AgpdGridItem			*pcsGridItem;

	BOOL				bResult;

	bResult = FALSE;

	if (!pcsAgpdCharacter || !pstEvent)
		return FALSE;
	
	// npc와의 거리를 체크한다.
	AuPOS	*pstBasePos	= m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, NULL);
	if (pstBasePos)
	{
		FLOAT				fx = pstBasePos->x - pcsAgpdCharacter->m_stPos.x;
		FLOAT				fz = pstBasePos->z - pcsAgpdCharacter->m_stPos.z;

		FLOAT	fDistance = (FLOAT) sqrt(fx * fx +
								 fz * fz);

		if (!CheckUsingCashItem(pcsAgpdCharacter) &&
			fDistance > AGPMEVENT_NPCTRADE_MAX_USE_RANGE)
			return FALSE;
	}

	if( (AgpdEventNPCTradeData *)pstEvent->m_pvData != NULL )
	{
		pcsItemGrid = ((AgpdEventNPCTradeData *)pstEvent->m_pvData)->m_pcsGrid;
	}

	if( (pcsItemGrid != NULL) && (pvDetailInfo != NULL ) )
	{
		INT16					nStatus, nLayer, nRow, nColumn;
		INT32					lItemCount;

		nStatus = -1;
		nLayer = -1;
		nRow = -1;
		nColumn = -1;

		m_csDetailInfo.GetField(FALSE, pvDetailInfo, 0,
							NULL,				//Item TID
							NULL,				//ItemPrice
							&lItemCount,		//ItemCount
							&nStatus,			//Status
							&nLayer,			//Layer
							&nRow,				//Row
							&nColumn);			//Column

		//존재하는 캐릭인가?
		if( pcsAgpdCharacter != NULL )
		{
			pcsAgpdItem = m_pcsAgpmItem->GetItem( lIID );

			pcsGridItem = m_pcsAgpmGrid->GetItem(pcsItemGrid, nLayer, nRow, nColumn);

			if (!CheckVaildTradeItem(pcsAgpdCharacter, pstEvent, lIID, pcsGridItem, pcsAgpdItem, nLayer, nRow, nColumn))
				return FALSE;

			//존재하는 아이템이 맞는가?
			if( pcsAgpdItem && pcsGridItem && pcsGridItem->m_lItemID == lIID )
			{
				// 아템 타입에 따라 아템을 샀을때의 효과가 다르다.
				// 타입에 맞는 처리를 해준다.

				AgpdItemTemplateUsable *pcsTemplateUsable = (AgpdItemTemplateUsable *) pcsAgpdItem->m_pcsItemTemplate;

				// 스펠북 즉, 이 아템을 사면 곧바로 스킬을 습득하게 된다.
				//if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_eType == AGPMITEM_TYPE_USABLE &&
				//	pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPELLBOOK)
				//{
				//	BuySpellBook(pcsAgpdCharacter, pcsAgpdItem);
				//}
				// 보통의 아템이다. 던이 있음 사서 인벤토리에 넣는 넘들이다.
				//else
				//{
					BuyNormalItem(pcsAgpdCharacter, pcsAgpdItem, lItemCount, nStatus, cLayer, cRow, cColumn ); //NPC의 그리드에 가지고 있는 갯수만큼 바로 산다.
				//}
			}
			//사려는 아이템이 서버상에 존재하지 않는다.
			else
			{
				SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID );
			}
		}
	}

	return bResult;
}

BOOL AgsmEventNPCTrade::ProcessOpenAnywhere(AgpdCharacter *pcsAgpdCharacter, CHAR *pszNPCName)
{
	if (!pcsAgpdCharacter || !pszNPCName)
		return FALSE;

	// get NPC character
	AgpdCharacter *pcsAgpdCharacter_NPC = m_pcsAgpmCharacter->GetCharacter(pszNPCName);
	if (!pcsAgpdCharacter_NPC)
		return FALSE;

	// check cash item
	if (!CheckUsingCashItem(pcsAgpdCharacter))
		return FALSE;

	// get event
	ApdEvent *pstEvent = m_pcsApmEventManager->GetEvent(pcsAgpdCharacter_NPC, APDEVENT_FUNCTION_NPCTRADE);

	// penalty check
	if (m_pcsAgpmCharacter->HasPenalty(pcsAgpdCharacter, AGPMCHAR_PENALTY_NPCTRADE))
		return FALSE;

	// stop character
	pcsAgpdCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
	if (pcsAgpdCharacter->m_bMove)
		m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

	return ProcessItemList(pstEvent, pcsAgpdCharacter);
}

BOOL AgsmEventNPCTrade::ProcessOpenAnywhere_Sundry(AgpdCharacter *pcsAgpdCharacter)
{
	return FALSE;//return ProcessOpenAnywhere(pcsAgpdCharacter, "<만물상인> 대릴");
}

BOOL AgsmEventNPCTrade::CBActionNPCTrade(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventNPCTrade		*pThis				= (AgsmEventNPCTrade *)		pClass;
	AgpdCharacter			*pcsAgpdCharacter	= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pcsActionData		= (AgpdCharacterAction *)	pCustData;

	AuPOS	stTargetPos;

	ApdEvent				*pstEvent			= pThis->m_pcsApmEventManager->GetEvent(pcsActionData->m_csTargetBase.m_eType, pcsActionData->m_csTargetBase.m_lID, APDEVENT_FUNCTION_NPCTRADE);
	if (!pstEvent)
		return FALSE;

	if (pThis->m_pcsApmEventManager->CheckValidRange(pstEvent, &pcsAgpdCharacter->m_stPos, AGPMEVENT_NPCTRADE_MAX_USE_RANGE, &stTargetPos))
	{
		pThis->ProcessItemList(pstEvent, pcsAgpdCharacter);
	}
	else
	{
		return FALSE;

		// 아래코드.. 블럭킹에 걸려 멈춘경우 무한루프에 빠지게 된다.

		/*
		// stTargetPos 까지 이동
		pcsAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_NPC_TRADE;
		pcsAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pstEvent->m_pcsSource->m_eType;
		pcsAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pstEvent->m_pcsSource->m_lID;

		pThis->m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stTargetPos, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE);
		*/
	}

	return TRUE;
}

BOOL AgsmEventNPCTrade::BuyNormalItem(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lItemCount, INT16 nStatus, INT8 cLayer, INT8 cRow, INT8 cColumn )
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	//아이템 템플릿을 얻어낸다. 스택여부와 아이템 가격 계산을 위해.
	AgpdItemTemplate			*pcsItemTemplate;

	pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsAgpdItem->m_lTID );

	if( !pcsItemTemplate )
	{
		return FALSE;
	}

	// 2006.06.22. steeple
	eCantBuy cb = CheckBuyable(pcsAgpdCharacter, pcsItemTemplate);
	if( YOU_CAN_BUY != cb )
	{
		if(m_pcsAgpmSystemMessage)
		{
			if ( CANT_BUY_NOT_GUILDMASTER )
				m_pcsAgpmSystemMessage->ProcessSystemMessage(AGPMSYSTEMMESSAGE_PACKET_SYSTEM_MESSAGE, AGPMSYSTEMMESSAGE_CODE_ITEM_FOR_GUILDMASTER, -1, -1, NULL, NULL, pcsAgpdCharacter);
			else
				m_pcsAgpmSystemMessage->ProcessSystemMessage(AGPMSYSTEMMESSAGE_PACKET_SYSTEM_MESSAGE, AGPMSYSTEMMESSAGE_CODE_ITEM_CANNOT_BUY, -1, -1, NULL, NULL, pcsAgpdCharacter);
		}
		//SendBuy( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_CANNOT_BUY );
		
		return FALSE;
	}

	if(pcsItemTemplate->m_lID == 486)
	{
		ASSERT(!"어우샹 강축이 사지내 ㅜㅜ");
	}

	//아이템 가격을 얻어낸다.
	INT32				lItemPrice;
	
	if( pcsItemTemplate->m_bStackable )
	{
		if( (0 < lItemCount) && ( lItemCount <= pcsItemTemplate->m_lMaxStackableCount ) )
		{
			lItemPrice = (INT32)((double)m_pcsAgpmItem->GetNPCPrice(pcsAgpdItem) * (double)lItemCount);
		}
		else
		{
			SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEM_COUNT );

			return FALSE;
		}
	}
	else
	{
		lItemPrice = (INT32)m_pcsAgpmItem->GetNPCPrice(pcsAgpdItem);
		lItemCount = 0; //넌스택커블 아이템이므로 갯수는 1로 고정!!
	}

	if (lItemPrice <= 0)
	{
		SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID );
		return FALSE;
	}

	// apply tax
	INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsAgpdCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = (lItemPrice * lTaxRatio) / 100;
	}
	lItemPrice = lItemPrice + lTax;


	if (m_pcsAgpmCharacter->IsMurderer(pcsAgpdCharacter))
	{
		// murderer level 이 1인경우 아템 가격이 50% 상승하고 2이상인 경우는 아템 거래가 중지된다.
		INT32	lMurdererLevel = m_pcsAgpmCharacter->GetMurdererLevel(pcsAgpdCharacter);
		switch (lMurdererLevel) {
		case 1:
			lItemPrice = (INT32) (lItemPrice * 1.5f);
			break;
		case 2:
		case 3:
			SendBuyResult(pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_NO_SELL_TO_MURDERER);
			return FALSE;
			break;
		}
	}

	INT32 lTID = pcsAgpdItem->m_lTID;
	BOOL	bResult = TRUE;

	AgpdItemADChar *pcsAgpdItemADChar = m_pcsAgpmItem->GetADCharacter( pcsAgpdCharacter );

	//인벤도 있고~ 
	if( pcsAgpdItemADChar != NULL )
	{
		INT64	llInvenMoney	= 0;
		if (!m_pcsAgpmCharacter->GetMoney(pcsAgpdCharacter, &llInvenMoney) ||
			llInvenMoney < (INT64) lItemPrice)
		{
			SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_BUY_NOT_ENOUGH_MONEY );
			return FALSE;
		}

		//돈이 있는지부터 확인해본다.
		if( m_pcsAgpmCharacter->SubMoney( pcsAgpdCharacter, lItemPrice ) )
		{
			AgpdItem			*pcsNewItem;

			//아이템을 만든다. 그리고 Client로 아이템이 만들어졌다고 알려준다.
			pcsNewItem = m_pcsAgsmItemManager->CreateItem( lTID, NULL, lItemCount );

			if( pcsNewItem != NULL )
			{
				//NPC로부터 구입한 아이템이라고 세팅한다.
				//pcsNewItem->m_eStatus = AGPDITEM_STATUS_NPC_TRADE;
				//m_pcsAgpmItem->ChangeItemOwner(pcsNewItem, pcsAgpdCharacter);

				INT16	nLayer	= (INT16) cLayer;
				INT16	nRow	= (INT16) cRow;
				INT16	nColumn	= (INT16) cColumn;

				BOOL	bAddItemToInventory	= TRUE;

				// 귀속아이템이면 소유자 설정
				if (E_AGPMITEM_BIND_ON_ACQUIRE == m_pcsAgpmItem->GetBoundType(pcsNewItem->m_pcsItemTemplate))
				{
					m_pcsAgpmItem->SetBoundOnOwner(pcsNewItem, pcsAgpdCharacter);
				}

				if (cLayer > (-1) && cRow > (-1) && cColumn > (-1))
				{
					if(nStatus == AGPDITEM_STATUS_SUB_INVENTORY)
						bAddItemToInventory = m_pcsAgpmItem->AddItemToSubInventory(pcsAgpdCharacter, pcsNewItem, nLayer, nRow, nColumn);
					else
						bAddItemToInventory	= m_pcsAgpmItem->AddItemToInventory( pcsAgpdCharacter, pcsNewItem, nLayer, nRow, nColumn );
				}
				else
				{
					if(nStatus == AGPDITEM_STATUS_SUB_INVENTORY)
						bAddItemToInventory	= m_pcsAgpmItem->AddItemToSubInventory( pcsAgpdCharacter, pcsNewItem );
					else
						bAddItemToInventory	= m_pcsAgpmItem->AddItemToInventory( pcsAgpdCharacter, pcsNewItem );
				}

				//아이템 구입성공!!
				if( bAddItemToInventory )
				{
					bResult = SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_BUY_SUCCEEDED );

					//돈을 갱신한다.
					m_pcsAgpmCharacter->UpdateMoney( pcsAgpdCharacter );

					// 세금 처리는 구입이 성공했을 때만 한다. 2007.07.27. steeple
					m_pcsAgpmCharacter->PayTax(pcsAgpdCharacter, lTax);

					// 2005.03.15. steeple. Log 를 남긴다.
					WriteNPCTradeLog(pcsAgpdCharacter, pcsNewItem, lItemCount, lItemPrice, 0, FALSE);
				}
				//아이템 구입실패~
				else
				{
					//원래대로 돈을 돌려준다.
					m_pcsAgpmCharacter->AddMoney( pcsAgpdCharacter, lItemPrice );
					
					//생성된 아이템을 지운다.
					m_pcsAgsmItem->SendPacketItemRemove( pcsNewItem->m_lID, m_pcsAgsmCharacter->GetCharDPNID( pcsAgpdCharacter ) );

					//인벤이 꽉차서 못산다고 알려준다.
					SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_BUY_FULL_INVENTORY );

					m_pcsAgpmItem->RemoveItem(pcsNewItem);
				}
			}
			//아이템 생성 실패? 이런경우도 있나?
			else
			{
				//원래대로 돈을 돌려준다.
				m_pcsAgpmCharacter->AddMoney( pcsAgpdCharacter, lItemPrice );

				//서버의 문제로 아이템을 살 수 없다고 클라이언트에 알려야하나?? 쩝.... 운영정책에따라
				//나중에 추가하자. 지금은 그냥 넘어간다.
			}
		}
		//돈이 없군. 
		else
		{
			//돈없어서 못 산다고 클라이언트에 알린다.
			SendBuyResult( pcsAgpdCharacter, AGPDEVENTNPCTRADE_RESULT_BUY_NOT_ENOUGH_MONEY );
		}
	}

	return bResult;
}

//아이템을 구입한 결과를 보낸다.
BOOL AgsmEventNPCTrade::SendBuyResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult)
	{
	if (NULL == pcsAgpdCharacter)
		return FALSE;
	
	INT8	cOperation = AGPDEVENTNPCTRADE_BUYITEM;
	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
											NULL,								// event
											&cOperation,						// operation
											&pcsAgpdCharacter->m_lID,			// CID
											NULL,								// IID
											NULL,								// detail
											NULL,								// timestamp
											&lResult,							// result
											NULL,								// layer
											NULL,								// row
											NULL								// column
											);
	if (!pvPacket)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


//아이템을 NPC에게 판매한 결과를 클라이언트로 보낸다.
BOOL AgsmEventNPCTrade::SendSellResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult)
	{
	if (NULL == pcsAgpdCharacter)
		return FALSE;
	
	INT8	cOperation = AGPDEVENTNPCTRADE_SELLITEM;
	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
											NULL,								// event
											&cOperation,						// operation
											&pcsAgpdCharacter->m_lID,			// CID
											NULL,								// IID
											NULL,								// detail
											NULL,								// timestamp
											&lResult,							// result
											NULL,								// layer
											NULL,								// row
											NULL								// column
											);
	if (!pvPacket)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


//아이템 정보를 보내준다.
BOOL AgsmEventNPCTrade::SendItemInfo( AgpdCharacter *pcsAgpdCharacter, ApdEvent *pcsEvent, INT32 lIID, INT16 nLayer, INT16 nRow, INT16 nColumn, AgpdGridItem *pcsGridItem )
{
	if (!(AgpdEventNPCTradeData *) pcsEvent->m_pvData || !pcsGridItem)
		return FALSE;

	BOOL			bResult	= FALSE;

	if( (pcsAgpdCharacter!=NULL) && (lIID != 0) )
	{
		AgpdItem			*pcsAgpdItem = m_pcsAgpmItem->GetItem( lIID );

		if (!pcsAgpdItem)
		{
			INT32	lIndex = nLayer * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN;
			lIndex += (nRow * AGPMITEM_NPCTRADEBOX_COLUMN);
			lIndex += nColumn;

			ApBase	*pcsEventBase	= m_pcsApmEventManager->GetBase(pcsEvent->m_pcsSource->m_eType, pcsEvent->m_pcsSource->m_lID);
	
			char strCharBuff[512] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "SendItemInfo::GetItem() return NULL\n    send item information : id(%d), original id (%d), request cid(%s), event base id(%s)\n",
				lIID,
				/*((AgpdEventNPCTradeData *) pcsEvent->m_pvData)->m_alItemID[lIndex]*/ 0 ,
				(pcsAgpdCharacter) ? pcsAgpdCharacter->m_szID : NULL,
				(pcsEventBase && pcsEventBase->m_eType == APBASE_TYPE_CHARACTER) ? ((AgpdCharacter *) pcsEventBase)->m_pcsCharacterTemplate->m_szTName : NULL);
			AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff);
		}

		if( (pcsAgpdCharacter != NULL) )
		{
			if(pcsAgpdItem->m_pcsItemTemplate->m_lID == 486)
			{
				ASSERT(!"어우샹 강축을 왜 팔어~~");
			}

			if (!CheckVaildTradeItem(pcsAgpdCharacter, pcsEvent, lIID, pcsGridItem, pcsAgpdItem, nLayer, nRow, nColumn))
				return FALSE;

			if( m_pcsAgsmItem->SendPacketItem( pcsAgpdItem, m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter)) )
			{
				INT8				cOperation;
				INT16				nPacketLength;
				INT32				lTID;
				INT32				lPrice;
				INT32				lStackCount;
				INT32				lTimeStamp;
				void				*pvEventBase;
				void				*pvPrice;
				void				*pvDetailInfo;
				void				*pvPacket;

				cOperation = AGPDEVENTNPCTRADE_REQUESTITEMLIST;
				lTID = pcsAgpdItem->m_lTID;

				if (pcsAgpdItem->m_pcsItemTemplate && ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_bStackable)
					lStackCount = pcsAgpdItem->m_nCount;
				else
					lStackCount = 1;

				lTimeStamp = 1;
				pvPrice = m_pcsAgpmFactors->GetFactor( &pcsAgpdItem->m_csFactor, AGPD_FACTORS_TYPE_PRICE );
				if (!pvPrice)
					bResult	= FALSE;
				else
				{
					pvEventBase = m_pcsApmEventManager->MakeBasePacket( pcsEvent );

					if( (pvEventBase != NULL) )
					{
						lPrice = *((INT32 *)pvPrice);

						pvDetailInfo  = m_csDetailInfo.MakePacket(FALSE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
															&lTID,				//TID
															&lPrice,			//Price
															&lStackCount,		//StackCount
															NULL,
															&nLayer,			//Layer
															&nRow,				//Row
															&nColumn			//Column
														);

						pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
																	pvEventBase,    //pvEventBase
																	&cOperation,    // operation
																	&pcsAgpdCharacter->m_lID,			//lCID
																	&lIID,          //IID
																	pvDetailInfo,   //pvDetailInfo
																	&lTimeStamp,	//lTimeStamp
																	NULL,			//lResult
																	NULL,
																	NULL,
																	NULL
																	);

						m_csPacket.FreePacket(pvEventBase);

						if (pvDetailInfo)
							m_csDetailInfo.FreePacket(pvDetailInfo);

						if( pvPacket != NULL )
						{
							bResult = SendPacket( pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter) );
							
							m_csPacket.FreePacket(pvPacket);
						}
						else
						{
							bResult = FALSE;
						}
					}
					else
					{
						bResult = FALSE;
					}
				}
			}
			else
			{
				bResult = FALSE;
			}
		}
	}

	return bResult;
}

//아이템 정보를 보내준다.
BOOL AgsmEventNPCTrade::SendItemInfoEnded(AgpdCharacter *pcsAgpdCharacter)
	{
	if (NULL == pcsAgpdCharacter)
		return FALSE;
	
	INT8	cOperation = AGPDEVENTNPCTRADE_SEND_ALL_ITEM_INFO;
	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
											NULL,								// event
											&cOperation,						// operation
											&pcsAgpdCharacter->m_lID,			// CID
											NULL,								// IID
											NULL,								// detail
											NULL,								// timestamp
											NULL,								// result
											NULL,								// layer
											NULL,								// row
											NULL								// column
											);
	if (!pvPacket)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	m_csPacket.FreePacket(pvPacket);

	return bResult;
	}





BOOL AgsmEventNPCTrade::SetEventNPCTradeItem()
{
	INT32				lIndex = 0;

	ApdEvent			*pcsApdEvent;

	pcsApdEvent = m_pcsApmEventManager->GetEventSquence(&lIndex);

	for( ; pcsApdEvent; pcsApdEvent = m_pcsApmEventManager->GetEventSquence(&lIndex) )
	{
		if( pcsApdEvent->m_eFunction == APDEVENT_FUNCTION_NPCTRADE )
		{
			AgpdEventNPCTradeData		*pcsNPCTradeData;
			AgpdEventNPCTradeTemplate	*pcsNPCTradeTemplate;

			pcsNPCTradeData = (AgpdEventNPCTradeData *)pcsApdEvent->m_pvData;

			if( pcsNPCTradeData != NULL )
			{
				pcsNPCTradeTemplate = m_pcsAgpmEventNPCTrade->GetNPCTradeTemplate( pcsNPCTradeData->m_lNPCTradeTemplateID );

				if( pcsNPCTradeTemplate != NULL )
				{
					CListNode<AgpdEventNPCTradeItemListData *>		*pcsNPCTradeItemNode;

					pcsNPCTradeItemNode = pcsNPCTradeTemplate->m_csItemList.GetStartNode();

					for( ; pcsNPCTradeItemNode; pcsNPCTradeItemNode = pcsNPCTradeItemNode->m_pcNextNode )
					{
						AgpdItemTemplate		*pcsItemTemplate;

						pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( ((AgpdEventNPCTradeItemListData *)pcsNPCTradeItemNode->m_tData)->m_lItemTID );

						if( pcsItemTemplate != NULL )
						{
							AgpdItem			*pcsAgpdItem;

							//아이템을 만들고~
							pcsAgpdItem = m_pcsAgsmItemManager->CreateItem( pcsItemTemplate->m_lID, NULL, ((AgpdEventNPCTradeItemListData *)pcsNPCTradeItemNode->m_tData)->m_lItemCount );

							if( pcsAgpdItem != NULL )
							{
								//이 아이템은 NPC거래전용이라고 세팅한다.
								pcsAgpdItem->m_eStatus = AGPDITEM_STATUS_NPC_TRADE;
								//Grid에 넣는다.
								m_pcsAgpmGrid->Add( pcsNPCTradeData->m_pcsGrid, pcsAgpdItem->m_pcsGridItem, 1, 1 );
							}
						}
					}

					CopyMemory(pcsNPCTradeData->m_ppcInitGridData, pcsNPCTradeData->m_pcsGrid->m_ppcGridData, sizeof(pcsNPCTradeData->m_ppcInitGridData));

					for (int i = 0; i < AGPMITEM_NPCTRADEBOX_LAYER * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN; ++i)
					{
						if (pcsNPCTradeData->m_pcsGrid->m_ppcGridData &&
							pcsNPCTradeData->m_pcsGrid->m_ppcGridData[i])
						{
							pcsNPCTradeData->m_alItemID[i]		= pcsNPCTradeData->m_pcsGrid->m_ppcGridData[i]->m_lItemID;
							pcsNPCTradeData->m_ppcItemData[i]	= m_pcsAgpmItem->GetItem(pcsNPCTradeData->m_pcsGrid->m_ppcGridData[i]->m_lItemID);
						}
					}
				}
			}
		}
	}

	return TRUE;
}


// 2005.03.15. steeple
BOOL AgsmEventNPCTrade::WriteNPCTradeLog(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lItemCount, INT32 lItemPrice, INT32 lNPCTID, BOOL bSell)
{
	return m_pcsAgsmItem->WriteItemLog(bSell ? AGPDLOGTYPE_ITEM_NPC_SELL : AGPDLOGTYPE_ITEM_NPC_BUY,
									pcsCharacter->m_lID, pcsItem, lItemCount, lItemPrice, NULL);
}

BOOL AgsmEventNPCTrade::CheckVaildTradeItem(AgpdCharacter *pcsAgpdCharacter, ApdEvent *pcsEvent, INT32 lIID, AgpdGridItem *pcsGridItem, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	if (!pcsAgpdCharacter || !pcsEvent || !pcsEvent->m_pvData || !pcsGridItem)
		return FALSE;

	if (!pcsAgpdItem)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "pcsAgpdItem is NULL\n");
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff);
		return FALSE;
	}

	INT32	lIndex = nLayer * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN;
	lIndex += (nRow * AGPMITEM_NPCTRADEBOX_COLUMN);
	lIndex += nColumn;

	AgpdItem	*pcsAgpdGridBaseItem	= (AgpdItem *) pcsGridItem->GetParentBase();
	if (!pcsAgpdGridBaseItem)
	{
		char strCharBuff2[256] = { 0, };
		sprintf_s(strCharBuff2, sizeof(strCharBuff2), "pcsGridItem->GetParentBase() return NULL\n");
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff2);

		ApBase	*pcsEventBase	= m_pcsApmEventManager->GetBase(pcsEvent->m_pcsSource->m_eType, pcsEvent->m_pcsSource->m_lID);
		if (!pcsEventBase)
		{
			char strCharBuff3[256] = { 0, };
			sprintf_s(strCharBuff3, sizeof(strCharBuff3), "    event base is not exist : type(%d), id(%d)\n", pcsEvent->m_pcsSource->m_eType, pcsEvent->m_pcsSource->m_lID);
			AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff3);
		}
		else if (pcsEventBase->m_eType == APBASE_TYPE_CHARACTER)
		{
			char strCharBuff3[256] = { 0, };
			sprintf_s(strCharBuff3, sizeof(strCharBuff3), "    request info : charid(%s), npcid(%s), itemid(%d)\n",
				pcsAgpdCharacter->m_szID, ((AgpdCharacter *) pcsEventBase)->m_pcsCharacterTemplate->m_szTName, lIID);
			AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff3);
		}
		else
		{
			char strCharBuff3[256] = { 0, };
			sprintf_s(strCharBuff3, sizeof(strCharBuff3), "    invalid event base : type(%d), id(%d)\n", pcsEvent->m_pcsSource->m_eType, pcsEvent->m_pcsSource->m_lID);
			AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff3);
		}

		return FALSE;
	}

	if (pcsAgpdGridBaseItem != pcsAgpdItem)
	{
		char strCharBuff2[256] = { 0, };
		sprintf_s(strCharBuff2, sizeof(strCharBuff2), "pcsAgpdGridBaseItem != pcsAgpdItem\n    item data TID : original(%d), pcsAgpdItem(%d)", 
			/*((AgpdEventNPCTradeData *) pcsEvent->m_pvData)->m_ppcItemData[lIndex]->m_lTID*/ 0 , pcsAgpdItem->m_lTID);
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff2);

		char strCharBuff3[256] = { 0, };
		sprintf_s(strCharBuff3, sizeof(strCharBuff3), "    pcsAgpdGridBaseItem information : id(%d), original id (%d), tid(%d), ownerid(%s), status(%d), count(%d), GridItemID(%d)\n",
			pcsAgpdGridBaseItem->m_lID,
			/*((AgpdEventNPCTradeData *) pcsEvent->m_pvData)->m_alItemID[lIndex]*/ 0 ,
			(pcsAgpdGridBaseItem->m_pcsItemTemplate) ? pcsAgpdGridBaseItem->m_pcsItemTemplate->m_lID : (-1),
			(pcsAgpdGridBaseItem->m_pcsCharacter) ? pcsAgpdGridBaseItem->m_pcsCharacter->m_szID : "NULL",
			pcsAgpdGridBaseItem->m_eStatus,
			pcsAgpdGridBaseItem->m_nCount,
			pcsGridItem->m_lItemID);
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff3);

		char strCharBuff4[256] = { 0, };
		sprintf_s(strCharBuff4, sizeof(strCharBuff4), "    pcsAgpdItem information         : id(%d), tid(%d), ownerid(%s), status(%d), count(%d)\n",
			pcsAgpdItem->m_lID,
			pcsAgpdItem->m_pcsItemTemplate->m_lID,
			(pcsAgpdItem->m_pcsCharacter) ? pcsAgpdItem->m_pcsCharacter->m_szID : "NULL",
			pcsAgpdItem->m_eStatus,
			pcsAgpdItem->m_nCount);
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff4);

		return FALSE;
	}

	if (pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_NPC_TRADE)
	{
		char strCharBuff2[256] = { 0, };
		sprintf_s(strCharBuff2, sizeof(strCharBuff2), "pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_NPC_TRADE\n    pcsAgpdItem information : id(%d), original id (%d), tid(%d), ownerid(%s), status(%d), count(%d), GridItemID(%d)\n",
			pcsAgpdItem->m_lID,
			/*((AgpdEventNPCTradeData *) pcsEvent->m_pvData)->m_alItemID[lIndex]*/ 0 ,
			pcsAgpdItem->m_pcsItemTemplate->m_lID,
			(pcsAgpdItem->m_pcsCharacter) ? pcsAgpdItem->m_pcsCharacter->m_szID : "NULL",
			pcsAgpdItem->m_eStatus,
			pcsAgpdItem->m_nCount,
			pcsGridItem->m_lItemID);
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff2);

		return FALSE;
	}

	AgpdEventNPCTradeTemplate	*pcsNPCTradeTemplate =
		m_pcsAgpmEventNPCTrade->GetNPCTradeTemplate( ((AgpdEventNPCTradeData *) pcsEvent->m_pvData)->m_lNPCTradeTemplateID );

	CListNode<AgpdEventNPCTradeItemListData *>		*pcsNPCTradeItemNode	=
		pcsNPCTradeTemplate->m_csItemList.GetStartNode();

	BOOL	bFind	= FALSE;

	for ( ; pcsNPCTradeItemNode; pcsNPCTradeItemNode = pcsNPCTradeItemNode->m_pcNextNode )
	{
		if (pcsAgpdItem->m_pcsItemTemplate->m_lID == ((AgpdEventNPCTradeItemListData *)pcsNPCTradeItemNode->m_tData)->m_lItemTID)
		{
			if (!pcsAgpdItem->m_pcsItemTemplate->m_bStackable ||
				pcsAgpdItem->m_nCount == ((AgpdEventNPCTradeItemListData *) pcsNPCTradeItemNode->m_tData)->m_lItemCount ||
				(((AgpdEventNPCTradeItemListData *) pcsNPCTradeItemNode->m_tData)->m_lItemCount > pcsAgpdItem->m_pcsItemTemplate->m_lMaxStackableCount &&
				 pcsAgpdItem->m_nCount == pcsAgpdItem->m_pcsItemTemplate->m_lMaxStackableCount))
			{
				bFind	= TRUE;
				break;
			}
		}
	}

	if (!bFind)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Can't find item in NPCTradItemList\n    pcsAgpdItem information : id(%d), original id(%d), tid(%d), ownerid(%s), status(%d), count(%d), GridItemID(%d)\n",
			pcsAgpdItem->m_lID,
			/*((AgpdEventNPCTradeData *) pcsEvent->m_pvData)->m_alItemID[lIndex]*/ 0 ,
			pcsAgpdItem->m_pcsItemTemplate->m_lID,
			(pcsAgpdItem->m_pcsCharacter) ? pcsAgpdItem->m_pcsCharacter->m_szID : "NULL",
			pcsAgpdItem->m_eStatus,
			pcsAgpdItem->m_nCount,
			pcsGridItem->m_lItemID);
		AuLogFile_s("LOG\\NPCTradeErr.log", strCharBuff);

		return FALSE;
	}

	return TRUE;
}

// 2006.06.22. steeple
eCantBuy AgsmEventNPCTrade::CheckBuyable(AgpdCharacter* pcsAgpdCharacter, AgpdItemTemplate* pcsItemTemplate)
{
	if(!pcsAgpdCharacter || !pcsItemTemplate)
		return CANT_BUY_GENERAL;

	if(pcsItemTemplate->m_eBuyerType == AGPMITEM_BUYER_TYPE_ALL)
		return YOU_CAN_BUY;

	eCantBuy bCheck = YOU_CAN_BUY;

	if(pcsItemTemplate->m_eBuyerType & AGPMITEM_BUYER_TYPE_GUILD_MASTER_ONLY)
	{
		CHAR* szGuildID = m_pcsAgpmGuild->GetJoinedGuildID(pcsAgpdCharacter);
		if(szGuildID && strlen(szGuildID) > 0)
		{
			if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsAgpdCharacter->m_szID))
				bCheck = CANT_BUY_NOT_GUILDMASTER;
		}
		else
			bCheck = CANT_BUY_GENERAL;
	}

	// 다른 검사들도 하는 데 현재로서는 없다.
	return bCheck;
}

BOOL AgsmEventNPCTrade::CheckUsingCashItem(AgpdCharacter* pcsAgpdCharacter)
{
	return TRUE;

	if (!pcsAgpdCharacter)
		return FALSE;
	
	AgpdGridItem *pcsAgpdGridItem = m_pcsAgpmItem->GetUsingCashItemUsableByType(pcsAgpdCharacter, AGPMITEM_USABLE_TYPE_ANY_SUNDRY, 0);
	if (NULL == pcsAgpdGridItem)
		return FALSE;
	
	AgpdItem *pcsAgpdItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
	if (NULL == pcsAgpdItem)
		return FALSE;

	return TRUE;
}

BOOL AgsmEventNPCTrade::IsNpcHandleItem(AgpdEventNPCTradeData *pcsAgpdEventNPCTradeData, INT32 lTID, float* fBuyFunc)
{
	BOOL bResult = FALSE;
	AgpdEventNPCTradeTemplate *pcsAgpdEventNPCTradeTeamplate
		= m_pcsAgpmEventNPCTrade->GetNPCTradeTemplate(pcsAgpdEventNPCTradeData->m_lNPCTradeTemplateID);

	if( pcsAgpdEventNPCTradeTeamplate )
	{
		if( m_pcsAgpmEventNPCTrade->IsItemInGrid( pcsAgpdEventNPCTradeTeamplate, lTID ) )
		{
			*fBuyFunc = pcsAgpdEventNPCTradeTeamplate->m_fBuyFunc;
			bResult = TRUE;
		}
		else
		{
			*fBuyFunc = pcsAgpdEventNPCTradeTeamplate->m_fBuyOtherFunc;
		}
	}

	return bResult;
}
