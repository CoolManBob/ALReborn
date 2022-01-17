#include "AgpmEventCharCustomize.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"

AgpmEventCharCustomize::AgpmEventCharCustomize()
{
	SetModuleName("AgpmEventCharCustomize");

	SetPacketType(AGPMEVENT_CHARCUSTOMIZE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_PACKET,	1,		// event base packet
							AUTYPE_INT32,	1,		// character id
							AUTYPE_INT8,	1,		// new face index
							AUTYPE_INT8,	1,		// new hair index
							AUTYPE_END,		0
							);

	m_astCustomizeList.MemSetAll();
}

AgpmEventCharCustomize::~AgpmEventCharCustomize()
{
}

BOOL AgpmEventCharCustomize::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmGrid			= (AgpmGrid *)			GetModule("AgpmGrid");

	if (!m_pcsApmEventManager ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmGrid)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventCharCustomize(CBActionEventCharCustomize, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventCharCustomize::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < sizeof(PACKET_HEADER))
		return FALSE;

	INT8	cOperation		= (-1);
	PVOID	pvPacketBase	= NULL;
	INT32	lCID			= AP_INVALID_CID;

	INT8	cNewFaceIndex	= (-1);
	INT8	cNewHairIndex	= (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketBase,
						&lCID,
						&cNewFaceIndex,
						&cNewHairIndex);

	ApdEvent		*pcsEvent		= m_pcsApmEventManager->GetEventFromBasePacket(pvPacketBase);
	if (!pcsEvent)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation) {
		case AGPMEVENT_CHARCUSTOMIZE_REQUEST:
			{
				OnOperationRequest(pcsCharacter, pcsEvent);
			}
			break;

		case AGPMEVENT_CHARCUSTOMIZE_RESPONSE:
			{
				OnOperationResponse(pcsCharacter, pcsEvent);
			}
			break;

		case AGPMEVENT_CHARCUSTOMIZE_BUY:
			{
				OnOperationBuy(pcsCharacter, pcsEvent, cNewFaceIndex, cNewHairIndex);
			}
			break;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

PVOID AgpmEventCharCustomize::MakePacketRequestEvent(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pcsEvent || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_CHARCUSTOMIZE_REQUEST;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_CHARCUSTOMIZE_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													NULL,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventCharCustomize::MakePacketResponseEvent(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pcsEvent || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_CHARCUSTOMIZE_RESPONSE;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_CHARCUSTOMIZE_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													NULL,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventCharCustomize::MakePacketRequestCustomize(INT32 lCID, ApdEvent *pcsEvent, INT32 lFaceIndex, INT32 lHairIndex, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pcsEvent || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_CHARCUSTOMIZE_BUY;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cFaceIndex		= (INT8)	lFaceIndex;
	INT8	cHairIndex		= (INT8)	lHairIndex;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_CHARCUSTOMIZE_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													&cFaceIndex,
													&cHairIndex);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

BOOL AgpmEventCharCustomize::OnOperationRequest(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent)
{
	if (!pcsCharacter || !pcsEvent)
		return FALSE;

	if (!CheckCharacterStatus(pcsEvent, pcsCharacter))
		return FALSE;

	AuPOS	stTargetPos	= {0,0,0};

	if (CheckValidRange(pcsEvent, pcsCharacter, &stTargetPos))
	{
		if (m_pcsAgpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_CHARCUST))
			return FALSE;
	
		return EnumCallback(AGPMEVENT_CHARCUSTOMIZE_GRANT, pcsCharacter, pcsEvent);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_CHARCUSTOMIZE;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		return m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventCharCustomize::OnOperationResponse(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent)
{
	if (!pcsCharacter || !pcsEvent)
		return FALSE;

	return EnumCallback(AGPMEVENT_CHARCUSTOMIZE_GRANT, pcsCharacter, pcsEvent);
}

BOOL AgpmEventCharCustomize::OnOperationBuy(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent, INT8 cNewFaceIndex, INT8 cNewHairIndex)
{
	if (!pcsCharacter || !pcsEvent)
		return FALSE;
	// pay cost
	const INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);

	INT32 totalCost = 0;
	INT32 totalTax  = 0;
	INT32 minLevel  = 0;
	INT32 itemTid   = 0;

	for (int i = 0; i < CHARCUSTOMIZE_MAX_LIST; ++i)
	{
		if (m_astCustomizeList[i].m_eType == 0)
			break;

		if (m_astCustomizeList[i].m_eType == CHARCUSTOMIZE_TYPE_FACE &&
			m_astCustomizeList[i].m_pcsCharacterTemplate == pcsCharacter->m_pcsCharacterTemplate &&
			m_astCustomizeList[i].m_lNumber == cNewFaceIndex)
		{
			// 겔드로 계산
			if ( m_astCustomizeList[i].m_lPriceSkull == 0 )
			{
				INT32 price = m_astCustomizeList[i].m_lPriceMoney;
				INT32 priceWithTax = GetPriceWithTax(lTaxRatio, price );

				totalCost += priceWithTax;
				totalTax += priceWithTax - price;
			}
			// 아이템으로 계산
			else
			{
				totalCost += m_astCustomizeList[i].m_lPriceMoney;
				itemTid = m_astCustomizeList[i].m_lPriceSkull;
			}

			minLevel = m_astCustomizeList[i].m_lUseLevel;
		}
		else if (m_astCustomizeList[i].m_eType == CHARCUSTOMIZE_TYPE_HAIR &&
			m_astCustomizeList[i].m_pcsCharacterTemplate == pcsCharacter->m_pcsCharacterTemplate &&
			m_astCustomizeList[i].m_lNumber == cNewHairIndex)
		{
			if ( m_astCustomizeList[i].m_lPriceSkull == 0 )
			{
				INT32 price = m_astCustomizeList[i].m_lPriceMoney;
				INT32 priceWithTax = GetPriceWithTax(lTaxRatio, price);
				
				totalCost += priceWithTax;
				totalTax += (priceWithTax - price);
			}
			else
			{
				totalCost += m_astCustomizeList[i].m_lPriceMoney;
				itemTid = m_astCustomizeList[i].m_lPriceSkull;
			}

			minLevel = MIN(m_astCustomizeList[i].m_lUseLevel, minLevel);
		}
	}

	// Level is too low.
	if (minLevel > m_pcsAgpmCharacter->GetLevel(pcsCharacter))
		return FALSE;

	INT64 countOnInven = 0;
	if ( 0 == itemTid )
	{
		m_pcsAgpmCharacter->GetMoney(pcsCharacter, &countOnInven);
	}
	else
	{
		countOnInven = GetItemCount(pcsCharacter, itemTid);
	}

	if ( countOnInven < totalCost )
		return FALSE;

	if (m_pcsAgpmCharacter->UpdateCustomizeIndex(pcsCharacter, cNewFaceIndex, cNewHairIndex))
	{
		if ( 0 == itemTid )
		{
			m_pcsAgpmCharacter->SubMoney(pcsCharacter, totalCost);
			m_pcsAgpmCharacter->PayTax(pcsCharacter, totalTax);
		}
		else
		{
			UsingCustomizingItem(pcsCharacter, itemTid, totalCost);
		}
	}

	PVOID pvBuffer[4] = {IntToPtr(cNewFaceIndex), IntToPtr(cNewHairIndex), IntToPtr(itemTid), IntToPtr(totalCost) };
	EnumCallback(AGPMEVENT_CHARCUSTOMIZE_AFTER_UPDATE, pcsCharacter, pvBuffer);

	return TRUE;
}

CharCustomizeCase AgpmEventCharCustomize::GetCustomizeCase(INT32 lRace, INT32 lClass, CharCustomizeType eType, INT32 lNumber)
{
	for (int i = 0; i < CHARCUSTOMIZE_MAX_LIST; ++i)
	{
		if (m_astCustomizeList[i].m_eType == 0)
			break;

		if (!m_astCustomizeList[i].m_pcsCharacterTemplate)
			continue;

		if (m_pcsAgpmFactors->GetRace(&m_astCustomizeList[i].m_pcsCharacterTemplate->m_csFactor) == lRace &&
			m_pcsAgpmFactors->GetClass(&m_astCustomizeList[i].m_pcsCharacterTemplate->m_csFactor) == lClass &&
			m_astCustomizeList[i].m_lNumber == lNumber,
			m_astCustomizeList[i].m_eType == eType)
		{
			return m_astCustomizeList[i].m_eCase;
		}
	}

	return CHARCUSTOMIZE_NONE;
}

BOOL AgpmEventCharCustomize::CheckCharacterStatus(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if (m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventCharCustomize::CheckValidRange(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, AuPOS *pstDestPos)
{
	if (!pcsEvent || !pcsCharacter || !pstDestPos)
		return FALSE;

	return m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, 800, pstDestPos);
}

BOOL AgpmEventCharCustomize::CBActionEventCharCustomize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventCharCustomize	*pThis	= (AgpmEventCharCustomize *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacterAction	*pstActionData	= (AgpdCharacterAction *)	pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	ApdEvent	*pcsEvent	= pThis->m_pcsApmEventManager->GetEvent(pstActionData->m_csTargetBase.m_eType,
																	pstActionData->m_csTargetBase.m_lID,
																	APDEVENT_FUNCTION_CHAR_CUSTOMIZE);

	if (!pcsEvent)
		return FALSE;

	return pThis->EnumCallback(AGPMEVENT_CHARCUSTOMIZE_GRANT, pcsCharacter, pcsEvent);
}

BOOL AgpmEventCharCustomize::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_CHARCUSTOMIZE_GRANT, pfCallback, pClass);
}

BOOL AgpmEventCharCustomize::SetCallbackAfterUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_CHARCUSTOMIZE_AFTER_UPDATE, pfCallback, pClass);
}

BOOL AgpmEventCharCustomize::StreamReadCustomizeList(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	if (nRow < 1)
		return FALSE;

	INT16			nCurRow			= 1;

	for (int i = 1; i < nRow; ++i)
	{
		for (int j = 0; j < nColumn; ++j)
		{
			CHAR	*szValue		= csExcelTxtLib.GetData(j, i);
			if (!szValue)
				continue;

			CHAR	*szColumnName	= csExcelTxtLib.GetData(j, 0);
			if (!szColumnName)
				continue;

			if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_TYPE) == 0)
			{
				if (strcmp(szValue, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_HAIR) == 0)
				{
					m_astCustomizeList[i - 1].m_eType	= CHARCUSTOMIZE_TYPE_HAIR;
				}
				else if (strcmp(szValue, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_FACE) == 0)
				{
					m_astCustomizeList[i - 1].m_eType	= CHARCUSTOMIZE_TYPE_FACE;
				}
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_NUMBER) == 0)
			{
				m_astCustomizeList[i - 1].m_lNumber	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_NAME) == 0)
			{
				strncpy(m_astCustomizeList[i - 1].m_szName, szValue, CHARCUSTOMIZE_MAX_NAME);
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_CHRACTERTID) == 0)
			{
				m_astCustomizeList[i - 1].m_pcsCharacterTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(atoi(szValue));
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_USELEVEL) == 0)
			{
				m_astCustomizeList[i - 1].m_lUseLevel	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_PRICE_MONEY) == 0)
			{
				m_astCustomizeList[i - 1].m_lPriceMoney	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_PRICE_SKULL) == 0)
			{
				m_astCustomizeList[i - 1].m_lPriceSkull	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_CASE) == 0)
			{
				m_astCustomizeList[i - 1].m_eCase	= (CharCustomizeCase) atoi(szValue);
			}
		}
	}

	return TRUE;
}

INT32 AgpmEventCharCustomize::GetPriceWithTax(INT32 taxRatio, INT32 price)
{
	return price + price * taxRatio / 100;
}

int AgpmEventCharCustomize::GetItemCount(AgpdCharacter* pcsCharacter, int tid)
{
	int count = 0;
	AgpdGrid* pcsWhere = 0;

	if ( AgpdItemTemplate* pItemTemplate = m_pcsAgpmItem->GetItemTemplate(tid) )
	{
		// cash item인 경우
		if ( IS_CASH_ITEM(pItemTemplate->m_eCashItemType) )
			pcsWhere = m_pcsAgpmItem->GetCashInventoryGrid( pcsCharacter );
		// 일반 아이템인 경우
		else
			pcsWhere = m_pcsAgpmItem->GetInventory( pcsCharacter );

		if ( pcsWhere )
		{
			int i = 0;

			AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsWhere, &i );
			while ( pcsGridItem )
			{
				if ( pcsGridItem->m_lItemTID == tid )
				{
					count += MAX(((AgpdItem*)pcsGridItem->GetParentBase())->m_nCount, 1);
				}

				pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsWhere, &i );
			}
		}
	}

	return count;
}

bool AgpmEventCharCustomize::UsingCustomizingItem(AgpdCharacter* pcsCharacter, const int tid, const int count)
{
	int use = 0;
	AgpdGrid* pcsWhere = 0;

	if ( AgpdItemTemplate* pItemTemplate = m_pcsAgpmItem->GetItemTemplate(tid) )
	{
		// cash item인 경우
		if ( IS_CASH_ITEM(pItemTemplate->m_eCashItemType) )
			pcsWhere = m_pcsAgpmItem->GetCashInventoryGrid( pcsCharacter );
		// 일반 아이템인 경우
		else
			pcsWhere = m_pcsAgpmItem->GetInventory( pcsCharacter );

		if ( pcsWhere )
		{
			int i = 0;

			AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsWhere, &i );
			while ( pcsGridItem && use <= count )
			{
				if ( pcsGridItem->m_lItemTID == tid )
				{
					AgpdItem* pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);

					if ( pcsItem->m_nCount >= count - use )
					{
						m_pcsAgpmItem->SubItemStackCount(pcsItem, count - use);
						use += (count - use);
					}
					else
					{
						use += pcsItem->m_nCount;
						m_pcsAgpmItem->SubItemStackCount(pcsItem, pcsItem->m_nCount);
						--i;	// stepback index
					}
				}

				pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsWhere, &i );
			}
		}
	}

	return count <= use;
}
