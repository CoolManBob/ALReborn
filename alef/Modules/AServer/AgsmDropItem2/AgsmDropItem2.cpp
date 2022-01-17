#include "AgsmDropItem2.h"
#include "AgpdDropItem2.h"
#include "AgpdAI2.h"

AgsmDropItem2::AgsmDropItem2()
{
	SetModuleName("AgsmDropItem2");
	
	m_pcsAgpmConfig = NULL;
}

AgsmDropItem2::~AgsmDropItem2()
{
}

BOOL AgsmDropItem2::OnAddModule()
{
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmParty = (AgpmParty *) GetModule( "AgpmParty" );
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmItemConvert = (AgpmItemConvert *) GetModule( "AgpmItemConvert" );
	m_pcsAgpmDropItem2 = (AgpmDropItem2 *) GetModule("AgpmDropItem2");

	m_pcsAgpmConfig = (AgpmConfig *) GetModule("AgpmConfig");

	m_pcsAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pcsAgsmItemManager = (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pcsAgsmItem = (AgsmItem *) GetModule("AgsmItem");
	m_pcsAgsmParty = (AgsmParty *) GetModule("AgsmParty");
	m_pcsAgpmAI2 = (AgpmAI2 *) GetModule("AgpmAI2");
	
	if( !m_pcsAgpmFactors || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmItemConvert || !m_pcsAgpmDropItem2 ||
		!m_pcsAgsmCharacter || !m_pcsAgsmItemManager || !m_pcsAgsmItem || !m_pcsAgsmParty /*|| !m_pcsAgpmAI2*/)
	{
		return FALSE;
	}

	if (!m_pcsAgpmItem->SetCallbackDropMoneyToField(CBDropMoneyToField, this))
		return FALSE;

	if (!m_pcsAgpmDropItem2->SetCallbackDropItem(CBDropItem, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmDropItem2::OnInit()
{
	m_pcsAgsmTitle = (AgsmTitle *) GetModule("AgsmTitle");

	return TRUE;
}

BOOL AgsmDropItem2::DropItem( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter )
{
	//STOPWATCH2(GetModuleName(), _T("DropItem"));
	AgpdCharacter* pcsLooter;

	if ( pcsFirstLooter )
		pcsLooter = m_pcsAgpmCharacter->GetCharacter(pcsFirstLooter->m_lID);
	else
		pcsLooter = NULL;

	// Check pcsFirstLooter is TPackUser
	BOOL	bTPackUser		= FALSE;
	INT32	ulPCRoomType	= 0;

	m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsLooter, &ulPCRoomType);

	if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
		bTPackUser = TRUE;

	// 중국 중독 방지. 중독 상태일때는 아이템 드랍이 없다.
	if ( NULL != pcsLooter && AS_RED == pcsLooter->m_lAddictStatus ) return TRUE;

	AgpdParty	*pcsAgpdParty	= NULL;
	INT32		lPCLevel = 0;

	// PC 혹은 소환수, 테이밍된 몬스터는 아이템을 드랍하면 안된다.
	///////////////////////////////////////////////////////////////////

	if (m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter) ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SUMMONER ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_TAME)
		return TRUE;

	if (pcsFirstLooter)
	{
		if (pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsAgpdParty = m_pcsAgpmParty->GetParty( ((AgpdCharacter *)pcsFirstLooter) );
		}
		else if (pcsFirstLooter->m_eType == APBASE_TYPE_PARTY)
		{
			pcsAgpdParty = (AgpdParty *) pcsFirstLooter;
		}

		//파티에 속해있다면?
		if( pcsAgpdParty )
		{
			lPCLevel = m_pcsAgpmParty->GetPartyHighestLevel( pcsAgpdParty );
		}
		else
		{
			lPCLevel = m_pcsAgpmCharacter->GetLevel((AgpdCharacter *)pcsFirstLooter);
		}
	}

	if (!m_pcsAgpmDropItem2->ProcessDropItem(pcsAgpdCharacter, lPCLevel, pcsFirstLooter))
		return FALSE;
	
	m_pcsAgpmDropItem2->ProcessDropItem2(pcsAgpdCharacter, lPCLevel, pcsFirstLooter);

	// LootItem이 있는 녀석들은 LootItem을 Drop한다.
	DropLootItem(pcsAgpdCharacter, pcsFirstLooter);

	//마지막으로 돈을 드랍한다.
	INT32 lMinGhelld = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lGhelldMin;
	INT32 lMaxGhelld = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lGhelldMax;

	// 중독 상태로 진입하는 유저에게는 돈을 절반만 떨군다.
	if ( NULL != pcsLooter && AS_YELLOW == pcsLooter->m_lAddictStatus )
	{
		lMinGhelld /= 2;
		lMaxGhelld /= 2;
	}

	if( lMinGhelld == lMaxGhelld )
	{
	}
	else
	{
		if( lMinGhelld > lMaxGhelld )
		{
			INT32			lTemp;
			lTemp = lMinGhelld;
			lMinGhelld = lMaxGhelld;
			lMaxGhelld = lTemp;
		}

		lMinGhelld += m_csRand.randInt( lMaxGhelld - lMinGhelld );

		INT32	lLevel	= m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter);

		if (lLevel >= lPCLevel)
			lMinGhelld = (INT32)((FLOAT)(lMinGhelld) * (1.0f + pow((FLOAT)(lLevel - lPCLevel) * 2.0f, 2.0f) / 3000.0f));
		else
			lMinGhelld = (INT32)((FLOAT)(lMinGhelld) * (1.0f - pow((FLOAT)(lPCLevel - lLevel) * 2.0f, 2.0f) / 300.0f));

		if (lMinGhelld <= 0)
			lMinGhelld	= 1;
		else if (lMinGhelld > lMaxGhelld)
			lMinGhelld	= lMaxGhelld;

		// 2007.03.14. gloomy white day. 
		if (m_pcsAgpmConfig)
			lMinGhelld = (INT32)(m_pcsAgpmConfig->GetGheldDropAdjustmentRatio(bTPackUser) * (float)lMinGhelld);

		// 2005.03.31. steeple
		// Party 가 아니라면 걍 떨궈주고, 파티라면 근처에 있는 파티원들에게 직접 넣어준다.
		if(!pcsAgpdParty)
		{
			if (pcsFirstLooter &&
				pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER &&
				m_pcsAgsmCharacter->IsAutoPickup((AgpdCharacter *) pcsFirstLooter))
			{
				INT32 lBonusMoneyByCash = m_pcsAgpmCharacter->GetGameBonusMoney((AgpdCharacter *) pcsFirstLooter);
				lMinGhelld += (INT32)((double)lMinGhelld * (double)(lBonusMoneyByCash) / (double)100.0);

				m_pcsAgpmCharacter->AddMoney((AgpdCharacter *) pcsFirstLooter, lMinGhelld);
				m_pcsAgsmItem->SendPacketPickupItemResult((INT8)AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS,
											1,			// llID 돈이면 없어도 된다.
											m_pcsAgpmItem->GetMoneyTID(),
											lMinGhelld,
											m_pcsAgsmCharacter->GetCharDPNID((AgpdCharacter *) pcsFirstLooter));
			}
			else
			{
				//돈의 TID를 얻어 lMinGhelld만큼 드랍하면된다.
				AgpdItemTemplate *	pcsAgpdItemTemplate	= m_pcsAgpmItem->GetItemTemplate( m_pcsAgpmItem->GetMoneyTID() );
				if (!pcsAgpdItemTemplate)
					return FALSE;

				AgpdItem *			pcsMoney			= m_pcsAgsmItemManager->CreateItem( pcsAgpdItemTemplate->m_lID, NULL, lMinGhelld, FALSE );
				if (!pcsMoney)
					return FALSE;

				if (DropItemToField(pcsAgpdCharacter, pcsAgpdParty ? pcsAgpdParty : pcsFirstLooter, pcsMoney))
					return FALSE;
			}
		}
		else
		{
			// 파티다.
			ApSafeArray<AgpdCharacter*, AGPMPARTY_MAX_PARTY_MEMBER>	pcsNearMember;
			pcsNearMember.MemSetAll();

			INT32 lMemberTotalLevel = 0;
			INT32 lNumNearMember = m_pcsAgsmParty->GetNearMember(pcsAgpdParty, pcsAgpdCharacter, &pcsNearMember[0], &lMemberTotalLevel);

			// 2005.03.31. steeple
			// 근처에 있는 파티원들의 수로 나눈후 몫을 올려서 균등분배해준다.
			if(lMinGhelld > 0 && lNumNearMember > 0)
			{
				INT32 lGhelldForOneMember = 0;
				INT32 lDivision = (INT32)(lMinGhelld / lNumNearMember);
				INT32 lRemainder = lMinGhelld % lNumNearMember;

				INT32 lMostDamagerCID = 0;
				AgsdCharacterHistoryEntry* pcsEntry = m_pcsAgsmCharacter->GetMostDamagerInParty(pcsAgpdCharacter, pcsAgpdParty->m_lID);
				if(pcsEntry)
					lMostDamagerCID = pcsEntry->m_csSource.m_lID;

				for(INT32 i = 0; i < lNumNearMember; i++)
				{
					// 멤버에게 돈을 올려준다.
					if(pcsNearMember[i])
					{
						if(pcsNearMember[i]->m_lID == lMostDamagerCID)
							lGhelldForOneMember = lDivision + lRemainder;
						else
							lGhelldForOneMember = lDivision;

						if(lGhelldForOneMember == 0)
							continue;

						// 2005.12.21. steeple
						// 게임보너스를 적용시킨다.
						INT32 lBonusMoneyByCash = m_pcsAgpmCharacter->GetGameBonusMoney(pcsNearMember[i]);
						lGhelldForOneMember += (INT32)((double)lGhelldForOneMember * (double)(lBonusMoneyByCash) / (double)100.0);

						m_pcsAgpmCharacter->AddMoney(pcsNearMember[i], lGhelldForOneMember);
						m_pcsAgsmItem->SendPacketPickupItemResult((INT8)AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS,
													1,			// llID 돈이면 없어도 된다.
													m_pcsAgpmItem->GetMoneyTID(),
													lGhelldForOneMember,
													m_pcsAgsmCharacter->GetCharDPNID(pcsNearMember[i]));
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmDropItem2::DropItemToField(AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter, AgpdItem *pcsItem)
{
	AgpdItemTemplate *			pcsAgpdItemTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;;
	AgsdItem *					pcsAgsdItem			= m_pcsAgsmItem->GetADItem(pcsItem);

	//죽은 몬스터의 위치에 떨군다.
	pcsItem->m_posItem = pcsAgpdCharacter->m_stPos;

	INT32	lRandomX	= m_csRand.randInt(100);
	INT32	lRandomZ	= m_csRand.randInt(100);

	if (lRandomX > 50)
		pcsItem->m_posItem.x	+= lRandomX;
	else
		pcsItem->m_posItem.x	-= (lRandomX + 50);

	if (lRandomZ > 50)
		pcsItem->m_posItem.z	+= lRandomZ;
	else
		pcsItem->m_posItem.z	-= (lRandomZ + 50);

	if (pcsFirstLooter)
	{
		pcsAgsdItem->m_csFirstLooterBase.m_eType	= pcsFirstLooter->m_eType;
		pcsAgsdItem->m_csFirstLooterBase.m_lID		= pcsFirstLooter->m_lID;

		pcsAgsdItem->m_ulDropTime					= GetClockCount();
	}

	m_pcsAgpmItem->AddItemToField( pcsItem );

	return TRUE;
}

BOOL AgsmDropItem2::CBDropMoneyToField( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pClass || !pData)
		return FALSE;

	AgsmDropItem2		*pThis;
	AgpdCharacter		*pcsAgpdCharacter;
	INT32				lDropMoneyCount;
	BOOL				bResult;

	pThis	= (AgsmDropItem2 *) pClass;
	pcsAgpdCharacter = (AgpdCharacter *)pData;
	lDropMoneyCount = *((INT32 *)pCustData);
	bResult = FALSE;

	if( pcsAgpdCharacter && (lDropMoneyCount>0) )
	{
		//돈을 빼보자~
		if( pThis->m_pcsAgpmCharacter->SubMoney( pcsAgpdCharacter, lDropMoneyCount ) )
		{
			AgpdItem			*pcsAgpdItem;
			INT32				lMoneyTID;

			lMoneyTID = pThis->m_pcsAgpmItem->GetMoneyTID();

			//돈을 뺐으니~ 땅에 돈을 떨군다.
			pcsAgpdItem = pThis->m_pcsAgsmItemManager->CreateItem( lMoneyTID, NULL, lDropMoneyCount, FALSE );

			if( pcsAgpdItem )
			{
				pcsAgpdItem->m_posItem = pcsAgpdCharacter->m_stPos;

				bResult = pThis->m_pcsAgpmItem->AddItemToField( pcsAgpdItem );
			}
		}
	}

	return bResult;
}

BOOL AgsmDropItem2::CBDropItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmDropItem2 *		pThis				= (AgsmDropItem2 *) pClass;
	AgpdItemTemplate *	pcsItemTemplate		= (AgpdItemTemplate *) pData;
	AgpdDropItemInfo2 *	pcsDropInfo			= (AgpdDropItemInfo2 *) pCustData;
	AgpdItem *			pcsItem;
	AgpdDropItemADItemTemplate *	pcsADItemTemplate	= pThis->m_pcsAgpmDropItem2->GetItemTemplateData(pcsItemTemplate);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBDropItem"));

	pcsItem		= pThis->m_pcsAgsmItemManager->CreateItem(pcsItemTemplate->m_lID,
														  NULL,
														  (pcsDropInfo->m_lItemStackCount > 0) ? pcsDropInfo->m_lItemStackCount : 1,
														  FALSE
														  );
	ASSERT(pcsItem);
	if (!pcsItem)
		return FALSE;

	//ASSERT(pcsADItemTemplate->m_lGroupID);
	//if (!pcsADItemTemplate->m_lGroupID)
	//	return FALSE;

	pThis->m_pcsAgpmDropItem2->ProcessConvertItem(pcsItem, pcsDropInfo->m_pcsDropCharacter);

	// 2007.02.05. steeple
	pThis->m_pcsAgsmItem->ProcessItemSkillPlus(pcsItem, pcsDropInfo->m_pcsDropCharacter);

	if (pcsItem->m_pcsItemTemplate->m_lID != 4608)
	{
		if (pThis->m_pcsAgsmItem->AddItemToPartyMember(pcsDropInfo->m_pcsFirstLooter, pcsDropInfo->m_pcsDropCharacter, pcsItem) == TRUE)		//	2005.04.22. By SungHoon
			return TRUE;
	}

	if (pcsDropInfo->m_pcsFirstLooter &&
		pcsDropInfo->m_pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER &&
		pThis->m_pcsAgsmCharacter->IsAutoPickup((AgpdCharacter *) pcsDropInfo->m_pcsFirstLooter) &&
		pcsItem->m_pcsItemTemplate->m_lID != 4608)
	{
		if (pThis->m_pcsAgpmItem->AddItemToInventory((AgpdCharacter *) pcsDropInfo->m_pcsFirstLooter, pcsItem) != TRUE)
		{
			if (pThis->m_pcsAgpmItem->IsEnableSubInventory((AgpdCharacter*)pcsDropInfo->m_pcsFirstLooter))
			{
				if(pThis->m_pcsAgpmItem->AddItemToSubInventory((AgpdCharacter*)pcsDropInfo->m_pcsFirstLooter, pcsItem) != TRUE)
				{
					return pThis->DropItemToField(pcsDropInfo->m_pcsDropCharacter, pcsDropInfo->m_pcsFirstLooter, pcsItem);
				}
			}
			else
			{
				return pThis->DropItemToField(pcsDropInfo->m_pcsDropCharacter, pcsDropInfo->m_pcsFirstLooter, pcsItem);
			}
		}

		// 돈이 아닐 때 인벤으로 바로 아이템 들어갈 때 로그를 남긴다.
		if(!(((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
			((AgpdItemTemplateOther *) pcsItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY))
			pThis->m_pcsAgsmItem->WritePickupLog(pcsDropInfo->m_pcsFirstLooter->m_lID, pcsItem, 
												(pcsDropInfo->m_lItemStackCount > 0) ? pcsDropInfo->m_lItemStackCount : 1);

		if (pThis->m_pcsAgpmItem->IsQuestItem(pcsItem) &&
			E_AGPMITEM_BIND_ON_ACQUIRE == pThis->m_pcsAgpmItem->GetBoundType(pcsItem))
				pThis->m_pcsAgpmItem->SetBoundOnOwner(pcsItem, (AgpdCharacter *) pcsDropInfo->m_pcsFirstLooter);

		INT32	lItemCount	= pcsItem->m_nCount;

		pThis->m_pcsAgsmItem->SendPacketItem(pcsItem, pThis->m_pcsAgsmCharacter->GetCharDPNID((AgpdCharacter *) pcsDropInfo->m_pcsFirstLooter));
		pThis->m_pcsAgsmItem->SendPacketPickupItemResult(AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS, (pcsItem) ? pcsItem->m_lID : AP_INVALID_IID, pcsItemTemplate->m_lID, lItemCount, pThis->m_pcsAgsmCharacter->GetCharDPNID((AgpdCharacter *) pcsDropInfo->m_pcsFirstLooter));
		pThis->m_pcsAgsmTitle->OnPickUpItemResult((AgpdCharacter *) pcsDropInfo->m_pcsFirstLooter, pcsItemTemplate, lItemCount);

		return TRUE;
	}

	return pThis->DropItemToField(pcsDropInfo->m_pcsDropCharacter, pcsDropInfo->m_pcsFirstLooter, pcsItem);
}

BOOL AgsmDropItem2::DropLootItem(AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsFirstLooter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_fLootItemRange > 0)
	{
		CLootItemVector::iterator iter = pcsAgpdAI2ADChar->m_vLootItem.begin();
		while(iter != pcsAgpdAI2ADChar->m_vLootItem.end())
		{
			if(iter->m_ulItemID == 0) 
			{
				iter++;
				continue;
			}

			AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(iter->m_ulItemID);
			if(NULL == pcsItem)
			{
				iter++;
				continue;
			}

			if(DropItemToField(pcsAgpdCharacter, pcsFirstLooter, pcsItem) == TRUE)
			{
				iter = pcsAgpdAI2ADChar->m_vLootItem.erase(iter);
				continue;
			}

			iter++;
		}
	}

	return TRUE;
}