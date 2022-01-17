#include "AgsmEventSkillMaster.h"

AgsmEventSkillMaster::AgsmEventSkillMaster()
{
	SetModuleName("AgsmEventSkillMaster");

	m_pcsAgpmFactors			= NULL;
	m_pcsAgpmCharacter			= NULL;
	m_pcsAgpmItem				= NULL;
	m_pcsAgpmSkill				= NULL;
	m_pcsAgpmEventSkillMaster	= NULL;
	m_pcsAgsmCharacter			= NULL;
	m_pcsAgsmItem				= NULL;
	m_pcsAgsmItemManager		= NULL;
	m_pcsAgsmSkill				= NULL;
	m_pcsAgpmLog				= NULL;
}

AgsmEventSkillMaster::~AgsmEventSkillMaster()
{
}

BOOL AgsmEventSkillMaster::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmSkill				= (AgpmSkill *)				GetModule("AgpmSkill");
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)	GetModule("AgpmEventSkillMaster");
	m_pcsAgsmCharacter			= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmItem				= (AgsmItem *)				GetModule("AgsmItem");
	m_pcsAgsmItemManager		= (AgsmItemManager *)		GetModule("AgsmItemManager");
	m_pcsAgsmSkill				= (AgsmSkill *)				GetModule("AgsmSkill");
	m_pcsAgpmLog				= (AgpmLog *)				GetModule("AgpmLog");

	if (!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill ||
		!m_pcsAgpmEventSkillMaster ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmItem ||
		!m_pcsAgsmItemManager ||
		!m_pcsAgsmSkill)
		return FALSE;

	if (!m_pcsAgpmEventSkillMaster->SetCallbackBuySkillBook(CBBuySkillBook, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackLearnSkill(CBLearnSkill, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackRequestUpgrade(CBRequestUpgrade, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackRequestEvent(CBRequestEvent, this))
		return FALSE;

	if (!m_pcsAgpmEventSkillMaster->SetCallbackSaveAllData(CBSaveAllSkillData, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackRemoveSkill(CBRemoveSkill, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackEndAllSkill(CBEndAllSkill, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackEndSkill(CBEndSkill, this))
		return FALSE;

	if (!m_pcsAgpmEventSkillMaster->SetCallbackSkillInitialize(CBSkillInitialize, this))
		return FALSE;

	if (!m_pcsAgpmEventSkillMaster->SetCallbackInitSkillLog(CBSkillInitLog, this))
		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackUseItemSkillBook(CBUseItemSkillBook, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackUseItemSkillScroll(CBUseItemSkillScroll, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackUseItemSkillRollbackScroll(CBUseItemSkillRollbackScroll, this))
		return FALSE;

	if (!m_pcsAgsmSkill->SetCallbackParseSkillTreeString(CBParseSkillTreeString, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventSkillMaster::CBBuySkillBook(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	PVOID					*ppvBuffer			= (PVOID *)					pCustData;

	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			ppvBuffer[0];
	INT32					lSkillTID			= PtrToInt(ppvBuffer[1]);

	if (!pcsCharacter)
		return FALSE;

	if (!pcsEvent->m_pvData)
		return FALSE;

	if (!pThis->m_pcsAgpmEventSkillMaster->CheckBuySkillBook(pcsCharacter, lSkillTID))
		return FALSE;

	AgpdSkillEventAttachData	*pcsAttachData	= (AgpdSkillEventAttachData *)	pcsEvent->m_pvData;

	// 스킬북 TID를 얻어낸다.
	INT32	lSkillBookTID	= pThis->m_pcsAgpmItem->GetSkillBookTID(pcsAttachData->eRaceType, pcsAttachData->eClassType);			// Item Template ID

//	// 테스트를 위해 임시로 넣는다.
//	//  돼지 변신 물약이다.
//	///////////////////////////////////
//	lSkillBookTID	= 415;

	AgpdItem	*pcsSkillBook	= pThis->m_pcsAgsmItemManager->CreateItem(lSkillBookTID, pcsCharacter, 1, TRUE, lSkillTID);
	if (!pcsSkillBook)
		return FALSE;

//	// 테스트를 위해 임시로 넣는다.
//	//  돼지 변신 물약이다.
//	///////////////////////////////////
//	((AgpdItemTemplateUsable *) pcsSkillBook->m_pcsItemTemplate)->m_nUsableItemType	= AGPMITEM_USABLE_TYPE_SKILL_BOOK;

//	// 스킬 TID를 세팅한다.
//	pcsSkillBook->m_lSkillTID	= lSkillTID;

	if (!pThis->m_pcsAgpmItem->AddItemToInventory(pcsCharacter, pcsSkillBook))
	{
		pThis->m_pcsAgpmItem->RemoveItem(pcsSkillBook, TRUE);

		return FALSE;
	}
	INT32	lTax = 0;
	pThis->m_pcsAgpmCharacter->SubMoney(pcsCharacter, pThis->m_pcsAgpmEventSkillMaster->GetBuyCost(lSkillTID, pcsCharacter, &lTax));
	pThis->m_pcsAgpmCharacter->PayTax(pcsCharacter, lTax);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventSkillMaster->MakePacketBuySkillResult(pcsCharacter->m_lID, AGPMEVENT_SKILL_BUY_RESULT_SUCCESS, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

		pThis->m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);
	}
	
	return TRUE;
}

BOOL AgsmEventSkillMaster::CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	PVOID					*ppvBuffer			= (PVOID *)					pCustData;

	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			ppvBuffer[0];
	INT32					lSkillTID			= PtrToInt(ppvBuffer[1]);

	if (!pcsCharacter)
		return FALSE;

	if (!pcsEvent->m_pvData)
		return FALSE;

	AgpmEventSkillLearnResult eCheckResult	= pThis->m_pcsAgpmEventSkillMaster->CheckLearnSkill(pcsCharacter, pThis->m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));

	if (eCheckResult == AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
	{
		INT32	lTax = 0;
		INT32	lCostMoney	= pThis->m_pcsAgpmEventSkillMaster->GetBuyCost(lSkillTID, pcsCharacter, &lTax);
		INT32   lCostSP		= pThis->m_pcsAgpmEventSkillMaster->GetLearnCostSP(pThis->m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));
		if (lCostMoney < 0)
			return FALSE;

		pThis->m_pcsAgpmCharacter->SubMoney(pcsCharacter, lCostMoney);
		pThis->m_pcsAgpmCharacter->PayTax(pcsCharacter, lTax);

		// 스킬을 배운다.
		AgpdSkill				*pcsLearnSkill		= pThis->m_pcsAgpmSkill->LearnSkill((ApBase *) pcsCharacter, lSkillTID, 1);
		if (!pcsLearnSkill)
		{
			pThis->SendPacketLearnResult(pcsCharacter, AGPMEVENT_SKILL_LEARN_RESULT_FAIL);
			return FALSE;
		}

		pThis->m_pcsAgpmCharacter->UpdateSkillPoint(pcsCharacter, -lCostSP);

		// If Skill is HeroicSkill, Process Pay for HeroicSKill (HeroicPoint, CharismaPoint)
		if(pThis->m_pcsAgpmEventSkillMaster->IsHeroicSkill(lSkillTID))
		{
			AgpdSkillTemplate *pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(lSkillTID);
			if(pcsSkillTemplate)
			{
				INT32 lCostHeroicPoint = pThis->m_pcsAgpmEventSkillMaster->GetLearnCostHeroicPoint(pcsSkillTemplate);
				INT32 lCostCharismaPoint = pThis->m_pcsAgpmEventSkillMaster->GetLearnCostCharismaPoint(pcsSkillTemplate);

				pThis->m_pcsAgpmCharacter->UpdateHeroicPoint(pcsCharacter, -lCostHeroicPoint);
				pThis->m_pcsAgpmCharacter->SubCharismaPoint(pcsCharacter, lCostCharismaPoint);

				AgsdCharacter *pcsAgsdCharacter = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
				if(pcsAgsdCharacter)
				{
					pThis->m_pcsAgpmLog->WriteLog_CharismaUp(0, &pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szAccountID,
						pcsAgsdCharacter->m_szServerName, pcsCharacter->m_szID,
						pcsCharacter->m_pcsCharacterTemplate ? ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID : 0,
						pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter), 
						pThis->m_pcsAgpmCharacter->GetExp(pcsCharacter), 
						pcsCharacter->m_llMoney, pcsCharacter->m_llBankMoney,
						-lCostCharismaPoint, pThis->m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter), pcsCharacter->m_szID
						);
				}
			}
		}

		pThis->EnumCallback(AGSMEVENTSKILL_CB_UPDATE_DB, pcsCharacter, NULL);

		// 2005.03.16. steeple
		pThis->WriteLearnLog(pcsLearnSkill, lCostMoney);

		// Passive Skill 이라면 배운 즉시 캐스트 시킨다.
		if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsLearnSkill))
		{
			pThis->m_pcsAgsmSkill->CastPassiveSkill(pcsLearnSkill);

			//JK_패시브스킬 표시 오류
			///////////////////////////
//			UINT32	dpnid = pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter);
//			if(dpnid != 0)
//			{
//				INT16	nPacketLength = 0;
//				PVOID	pvPacket = pThis->m_pcsAgpmSkill->MakePacketSkill(pcsLearnSkill, &nPacketLength);

//				BOOL	bRetval = pThis->SendPacket(pvPacket, nPacketLength, dpnid);

//				pThis->m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
//			}
			//////////////////////////////
		}
	}
	
	pThis->SendPacketLearnResult(pcsCharacter, eCheckResult);

	if(eCheckResult == AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL AgsmEventSkillMaster::CBRequestUpgrade(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	PVOID					*ppvBuffer			= (PVOID *)					pCustData;

	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			ppvBuffer[0];
	INT32					lSkillID			= PtrToInt(ppvBuffer[1]);

	if (!pcsCharacter)
		return FALSE;

	AgpdSkill				*pcsSkill			= pThis->m_pcsAgpmSkill->GetSkill(lSkillID);
	if (!pcsSkill || !pcsSkill->m_pcsBase || pcsSkill->m_pcsBase->m_lID != pcsCharacter->m_lID)
		return FALSE;

	AgpmEventSkillUpgradeResult	eResult	= pThis->m_pcsAgpmEventSkillMaster->CheckUpgradeSkill(pcsCharacter, pcsSkill);

	if (eResult == AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS)
	{
		INT32	lTax = 0;
		INT32	lCostMoney	= pThis->m_pcsAgpmEventSkillMaster->GetUpgradeCost(pcsSkill, pcsCharacter, &lTax);

#ifdef _HIGHLEVEL_SKILL_TEST_
		lCostMoney = 0;
#endif

		if (lCostMoney < 0)
			return FALSE;

		INT32	lCostSP		= pThis->m_pcsAgpmEventSkillMaster->GetUpgradeCostSP(pcsSkill);

#ifdef _HIGHLEVEL_SKILL_TEST_
		lCostSP = 1;
#endif

		if (lCostSP < 0)
			return FALSE;

		INT32 lSkillTID = pcsSkill->m_pcsTemplate->m_lID;

		pThis->m_pcsAgpmCharacter->UpdateSkillPoint(pcsCharacter, (-lCostSP));
		pThis->m_pcsAgpmCharacter->SubMoney(pcsCharacter, lCostMoney);
		pThis->m_pcsAgpmCharacter->PayTax(pcsCharacter, lTax);

		// If Skill is HeroicSkill, Process Pay for HeroicSKill (HeroicPoint, CharismaPoint)
		if(pThis->m_pcsAgpmEventSkillMaster->IsHeroicSkill(lSkillTID))
		{
			INT32 lCostHeroicPoint = pThis->m_pcsAgpmEventSkillMaster->GetUpgradeCostHeroicPoint(pcsSkill);
			INT32 lCostCharismaPoint = pThis->m_pcsAgpmEventSkillMaster->GetUpgradeCostCharismaPoint(pcsSkill);

			pThis->m_pcsAgpmCharacter->UpdateHeroicPoint(pcsCharacter, -lCostHeroicPoint);
			pThis->m_pcsAgpmCharacter->SubCharismaPoint(pcsCharacter, lCostCharismaPoint);	

			AgsdCharacter *pcsAgsdCharacter = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			if(pcsAgsdCharacter)
			{
				pThis->m_pcsAgpmLog->WriteLog_CharismaUp(0, &pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szAccountID,
					pcsAgsdCharacter->m_szServerName, pcsCharacter->m_szID,
					pcsCharacter->m_pcsCharacterTemplate ? ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID : 0,
					pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter), 
					pThis->m_pcsAgpmCharacter->GetExp(pcsCharacter), 
					pcsCharacter->m_llMoney, pcsCharacter->m_llBankMoney,
					-lCostCharismaPoint, pThis->m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter), pcsCharacter->m_szID
					);
			}
		}

		pThis->m_pcsAgpmSkill->UpdateSkillPoint(pcsSkill, 1);

		// 2005.03.16. steeple
		pThis->WriteUpgradeLog(pcsSkill, lCostMoney);

		pThis->EnumCallback(AGSMEVENTSKILL_CB_UPDATE_DB, pcsCharacter, NULL);
	}

	return pThis->SendPacketUpgradeResult(pcsCharacter, pcsEvent, lSkillID, eResult);
}

BOOL AgsmEventSkillMaster::CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	PVOID					*ppvBuffer			= (PVOID *)					pCustData;

	ApdEvent				*pcsEvent			= (ApdEvent *)				ppvBuffer[0];
	AgpmEventRequestResult	eResult				= (AgpmEventRequestResult)	PtrToInt(ppvBuffer[1]);

	if (!pcsEvent)
		return FALSE;

	INT8	cIsInitialize	= 0;

	if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
	{
		if (strcmp(pcsCharacter->m_szSkillInit, "initialized") == 0)
			cIsInitialize	= 1;
	}
	else
	{
		if (strcmp(pcsCharacter->m_szSkillInit, "치약바르자") == 0)
			cIsInitialize	= 1;
	}

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventSkillMaster->MakePacketResponseEvent(pcsEvent, pcsCharacter->m_lID, eResult, &nPacketLength, cIsInitialize);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmEventSkillMaster::CBSaveAllSkillData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	pThis->m_pcsAgsmCharacter->BackupCharacterData(pcsCharacter);
	pThis->m_pcsAgsmCharacter->EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgsmEventSkillMaster::CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdSkill				*pcsRemoveSkill		= (AgpdSkill *)				pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSkill->MakePacketSkillRemove(pcsRemoveSkill, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);

	pThis->m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmEventSkillMaster::CBEndAllSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	pThis->m_pcsAgsmSkill->EndAllBuffedSkillExceptCashSkill((ApBase *) pcsCharacter, TRUE);
	pThis->m_pcsAgsmSkill->EndAllPassiveSkill((ApBase *) pcsCharacter);

	return TRUE;
}

BOOL AgsmEventSkillMaster::CBEndSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdSkill				*pcsSkill			= (AgpdSkill *)				pCustData;

	if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsSkill))
		return pThis->m_pcsAgsmSkill->EndPassiveSkill(pcsSkill);
	else
		return pThis->m_pcsAgsmSkill->EndBuffedSkill((ApBase *) pcsCharacter, pcsSkill);

	return FALSE;
}

BOOL AgsmEventSkillMaster::CBSkillInitialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	INT8					*pbResult			= (INT8 *)					pCustData;

	// 2007.08.01. steeple
	pThis->m_pcsAgsmCharacter->SendPacketSkillInitString(pcsCharacter);

	if ((*pbResult) == AGPMEVENT_SKILL_INITIALIZE_RESULT_SUCCESS && pThis->m_pcsAgpmLog)
	{
		AgsdCharacter* pAgsdCharacter = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
		if(!pAgsdCharacter)
			return FALSE;

		INT32 lLevel = pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter);
		INT64 llExp = pThis->m_pcsAgpmCharacter->GetExp(pcsCharacter);
		INT32 lCost = pThis->m_pcsAgpmEventSkillMaster->GetInitializeCost(pcsCharacter);

		pThis->m_pcsAgpmLog->WriteLog_SkillInit(0,
														&pAgsdCharacter->m_strIPAddress[0],
														pAgsdCharacter->m_szAccountID,
														pAgsdCharacter->m_szServerName,
														pcsCharacter->m_szID,
														((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
														lLevel,
														llExp,
														pcsCharacter->m_llMoney,
														pcsCharacter->m_llBankMoney,
														NULL,			// desc
														lCost
														);
	}

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventSkillMaster->MakePacketResponseSkillInitialize(pcsCharacter->m_lID, (INT8) *pbResult, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);

	pThis->m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmEventSkillMaster::CBSkillInitLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pData || NULL == pClass)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *) pClass;
	AgpdSkill				*pcsSkill			= (AgpdSkill *) pData;
	PVOID					*pvBuffer			= (PVOID *) pCustData;

	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)pvBuffer[0];
	AgpdItem				*pcsItem			= (AgpdItem *) pvBuffer[1];

	if(NULL == pcsCharacter)
		return FALSE;

	// Skill Init에 사용한 아이템이 있으면 아이템 사용 로그를 남겨준다.
	if(pcsItem)
	{
		BOOL bISCashItem = IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType);
		AgsdCharacter *pcsAgsdCharacter = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
		if(!pcsAgsdCharacter)
			return FALSE;

		INT32 lCharacterCharismaPoint = pThis->m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);
		INT32 lReturnCharismaPoint = pThis->m_pcsAgpmEventSkillMaster->GetInputTotalCharismaPoint(pcsSkill);

		CHAR strDesc[64] = {0. };
		sprintf_s(strDesc, sizeof(strDesc), "Remain CP : %d / Refund CP : %d", lCharacterCharismaPoint, lReturnCharismaPoint);

		pThis->m_pcsAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ETC_SKILL_INIT, 0, &pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szAccountID,
										  pcsAgsdCharacter->m_szServerName, pcsCharacter->m_szID, 
										  pcsCharacter->m_pcsCharacterTemplate ? pcsCharacter->m_pcsCharacterTemplate->m_lID : 0,
										  pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter),
										  pThis->m_pcsAgpmCharacter->GetExp(pcsCharacter),
										  pcsCharacter->m_llMoney,
										  pcsCharacter->m_llBankMoney,
										  pcsSkill->m_pcsTemplate ? pcsSkill->m_pcsTemplate->m_lID : 0,
										  NULL,
										  strDesc,
										  pcsItem->m_pcsItemTemplate ? pcsItem->m_pcsItemTemplate->m_lID : 0										  	
										  );
	}

	return TRUE;
}

BOOL AgsmEventSkillMaster::CBUseItemSkillBook(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdItem				*pcsItem			= (AgpdItem *)				pData;

	INT32	lTemplateRace	= AURACE_TYPE_NONE;
	INT32	lTemplateClass	= AUCHARCLASS_TYPE_NONE;

	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lTemplateRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lTemplateClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	INT32	lRace			= AURACE_TYPE_NONE;
	INT32	lClass			= AUCHARCLASS_TYPE_NONE;

	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsCharacter->m_csFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	// 테스트를 위해 빼놓는다.
	// 추후 꼭 다시 넣어야 한다.
	if ((lTemplateRace != AURACE_TYPE_NONE && lTemplateRace != lRace) ||
		(lTemplateClass != AUCHARCLASS_TYPE_NONE && lTemplateClass != lClass))
	{
		pThis->SendPacketLearnResult(pcsItem->m_pcsCharacter, AGPMEVENT_SKILL_LEARN_RESULT_NOT_LEARNABLE_CLASS);
		return FALSE;
	}

	AgpmEventSkillLearnResult eCheckResult	= pThis->m_pcsAgpmEventSkillMaster->CheckLearnSkill(pcsItem->m_pcsCharacter, pThis->m_pcsAgpmSkill->GetSkillTemplate(pcsItem->m_lSkillTID));

	if (eCheckResult == AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
	{
		// 스킬을 배운다.
		AgpdSkill				*pcsLearnSkill		= pThis->m_pcsAgpmSkill->LearnSkill((ApBase *) pcsItem->m_pcsCharacter, pcsItem->m_lSkillTID, 1);
		if (!pcsLearnSkill)
		{
			pThis->SendPacketLearnResult(pcsItem->m_pcsCharacter, AGPMEVENT_SKILL_LEARN_RESULT_FAIL);
			return FALSE;
		}

		pThis->m_pcsAgpmCharacter->UpdateSkillPoint(pcsItem->m_pcsCharacter, -1);

		pThis->EnumCallback(AGSMEVENTSKILL_CB_UPDATE_DB, pcsItem->m_pcsCharacter, NULL);

		// Passive Skill 이라면 배운 즉시 캐스트 시킨다.
		if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsLearnSkill))
			pThis->m_pcsAgsmSkill->CastPassiveSkill(pcsLearnSkill);
	}
	else
	{
		pThis->SendPacketLearnResult(pcsItem->m_pcsCharacter, eCheckResult);
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmEventSkillMaster::CBUseItemSkillScroll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdItem				*pcsItem			= (AgpdItem *)				pData;
	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)			pCustData;

	return pThis->m_pcsAgsmSkill->CastSkill((ApBase *) pcsItem->m_pcsCharacter, ((AgpdItemTemplateUsableSkillScroll *) pcsItem->m_pcsItemTemplate)->m_lSkillTID, ((AgpdItemTemplateUsableSkillScroll *) pcsItem->m_pcsItemTemplate)->m_lSkillLevel, (ApBase *) pcsTargetCharacter, FALSE, FALSE, pcsItem->m_lID);
}

BOOL AgsmEventSkillMaster::CBUseItemSkillRollbackScroll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdItem				*pcsItem			= (AgpdItem *)				pData;

	return TRUE;
}

BOOL AgsmEventSkillMaster::CBParseSkillTreeString(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSkillMaster	*pThis				= (AgsmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	CHAR					*szSkillTree		= (CHAR *)					pCustData;

	return pThis->DecodingSkillList(pcsCharacter, szSkillTree, (INT32)_tcslen(szSkillTree));
}

BOOL AgsmEventSkillMaster::SendPacketUpgradeResult(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent, INT32 lSkillID, AgpmEventSkillUpgradeResult eResult)
{
	if (!pcsCharacter || !pcsEvent || lSkillID == AP_INVALID_SKILLID)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketResponseUpgrade(pcsEvent, pcsCharacter->m_lID, lSkillID, eResult, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmEventSkillMaster::SendPacketLearnResult(AgpdCharacter *pcsCharacter, AgpmEventSkillLearnResult eResult)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketLearnResult(pcsCharacter->m_lID, eResult, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

// (skill template id, skill level)
///////////////////////////////////////////
const INT32	g_alOldProductSkillTID[3] = {201, 202, 284};
const INT32	g_lNewProductSkillTID = 663;

BOOL AgsmEventSkillMaster::EncodingSkillList(AgpdCharacter *pcsCharacter, CHAR *szBuffer, INT32 lBufferLength)
{
	if (!pcsCharacter || !szBuffer || lBufferLength < 1)
		return FALSE;

	ZeroMemory(szBuffer, sizeof(CHAR) * lBufferLength);

	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	INT32	lEncodedLength	= 0;

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (!pcsSkill)
			break;

		if (!pcsSkill->m_pcsTemplate)
			continue;

		if(m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate *)pcsSkill->m_pcsTemplate))
			continue;

	
		if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		{
			// 2007.05.02. laki. don't save old production skill. hard-coded skill tid.
			INT32 lSkillTID = pcsSkill->m_pcsTemplate->m_lID;
			if (g_alOldProductSkillTID[0] == lSkillTID || g_alOldProductSkillTID[1] == lSkillTID || g_alOldProductSkillTID[2] == lSkillTID)
				continue;
			
			sprintf(szBuffer + strlen(szBuffer), "%d,%d:", pcsSkill->m_pcsTemplate->m_lID, m_pcsAgpmSkill->GetSkillExp(pcsSkill));
		}
		else
		{
			INT32 lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
			if(lSkillLevel >= 1)
				sprintf(szBuffer + strlen(szBuffer), "%d,%d:", pcsSkill->m_pcsTemplate->m_lID, lSkillLevel);
		}

		if (strlen(szBuffer) + 8 > lBufferLength)
		{
			ASSERT(strlen(szBuffer) < lBufferLength);
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AgsmEventSkillMaster::EncodingDefaultSkillList(AgpdCharacterTemplate *pcsCharacterTemplate, CHAR *szBuffer, INT32 lBufferLength)
{
	if (!pcsCharacterTemplate || !szBuffer || lBufferLength < 1)
		return FALSE;

	ZeroMemory(szBuffer, sizeof(CHAR) * lBufferLength);

	AgpdEventSkillAttachCharTemplateData	*pcsAttachCharTemplateData	= m_pcsAgpmEventSkillMaster->GetSkillAttachCharTemplateData(pcsCharacterTemplate);

	for (int i = 0; i < pcsAttachCharTemplateData->m_lNumDefaultSkill; ++i)
	{
		if (pcsAttachCharTemplateData->m_apcsDefaultSkillTemplate[i]->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		{
			// 2007.05.02. laki. don't save old production skill. hard-coded skill tid.
			INT32 lSkillTID = pcsAttachCharTemplateData->m_apcsDefaultSkillTemplate[i]->m_lID;
			if (g_alOldProductSkillTID[0] == lSkillTID || g_alOldProductSkillTID[1] == lSkillTID || g_alOldProductSkillTID[2] == lSkillTID)
				continue;
			
			sprintf(szBuffer + strlen(szBuffer), "%d,0:", pcsAttachCharTemplateData->m_apcsDefaultSkillTemplate[i]->m_lID);
		}
		else
			sprintf(szBuffer + strlen(szBuffer), "%d,1:", pcsAttachCharTemplateData->m_apcsDefaultSkillTemplate[i]->m_lID);

		if (strlen(szBuffer) + 8 > lBufferLength)
		{
			ASSERT(strlen(szBuffer) < lBufferLength);
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AgsmEventSkillMaster::DecodingSkillList(AgpdCharacter *pcsCharacter, CHAR *szBuffer, INT32 lBufferLength)
{
	if (!pcsCharacter || !szBuffer || lBufferLength < 1)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	INT32	lDecodedLength	= 0;

	while (lDecodedLength < lBufferLength)
	{
		INT32	lSkillTID	= AP_INVALID_SKILLID;
		INT32	lSkillLevel	= 1;

		int j = 0;
		for (j = lDecodedLength; j < lBufferLength; ++j)
		{
			if (szBuffer[j] == ':')
				break;
		}

		if (j == lBufferLength)
			return FALSE;

		sscanf(szBuffer + lDecodedLength, "%d,%d:", &lSkillTID, &lSkillLevel);

		lDecodedLength	= j + 1;

		// 2007.05.02. laki. migration of old production skill. hard-coded skill tid.
		if (g_alOldProductSkillTID[0] == lSkillTID || g_alOldProductSkillTID[1] == lSkillTID || g_alOldProductSkillTID[2] == lSkillTID)
		{
			if (m_pcsAgpmSkill->IsOwnSkill((ApBase *) pcsCharacter, m_pcsAgpmSkill->GetSkillTemplate(g_lNewProductSkillTID)))
				continue;			// continue while loop
			
			lSkillTID = g_lNewProductSkillTID;
			lSkillLevel = 0;					// 생산에선 스킬레벨이 아닌 경험치이다.
		}


		AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->LearnSkill((ApBase *) pcsCharacter, lSkillTID, lSkillLevel);
		if (pcsSkill)
		{
			m_pcsAgpmCharacter->UpdateSkillPoint(pcsCharacter, -(m_pcsAgpmEventSkillMaster->GetInputTotalCostSP(pcsSkill)));

			// Passive Skill 이라면 배운 즉시 캐스트 시킨다.
	//		if (m_pcsAgpmSkill->IsPassiveSkill(pcsSkill))
	//			m_pcsAgsmSkill->CastPassiveSkill(pcsSkill);
		}
	}
	
	return TRUE;
}

BOOL AgsmEventSkillMaster::SetCallbackUpdateDB(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMEVENTSKILL_CB_UPDATE_DB, pfCallback, pClass);
}


// 2005.03.15. steeple
BOOL AgsmEventSkillMaster::WriteLearnLog(AgpdSkill* pcsLearnSkill, INT32 lCostMoney)
{
	if(!pcsLearnSkill)
		return FALSE;

	AgpdCharacter* pAgpdCharacter = (AgpdCharacter*)pcsLearnSkill->m_pcsBase;
	if(!pAgpdCharacter)
		return FALSE;

	AgsdCharacter* pAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	AgpdSkillTemplate* pSkillTemplate = (AgpdSkillTemplate*)pcsLearnSkill->m_pcsTemplate;
	if(!pSkillTemplate)
		return FALSE;

	INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pAgpdCharacter);

	return m_pcsAgpmLog->WriteLog_SkillBuy(0, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									((AgpdCharacterTemplate *)(pAgpdCharacter->m_pcsCharacterTemplate))->m_lID,
									lLevel,
									llExp,
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									pSkillTemplate->m_lID,
									NULL,
									lCostMoney
									);
}

BOOL AgsmEventSkillMaster::WriteUpgradeLog(AgpdSkill* pcsSkill, INT32 lCostMoney)
{
	if(!pcsSkill)
		return FALSE;

	AgpdCharacter* pAgpdCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	if(!pAgpdCharacter)
		return FALSE;

	AgsdCharacter* pAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	AgpdSkillTemplate* pSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pSkillTemplate)
		return FALSE;

	INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pAgpdCharacter);

	CHAR szDesc[AGPDLOG_MAX_DESCRIPTION];
	sprintf(szDesc, "LV=%d, SP=%d", m_pcsAgpmSkill->GetSkillLevel(pcsSkill),
									m_pcsAgpmCharacter->GetSkillPoint(pAgpdCharacter));

	return m_pcsAgpmLog->WriteLog_SkillUp(0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									((AgpdCharacterTemplate *)(pAgpdCharacter->m_pcsCharacterTemplate))->m_lID,
									lLevel,
									llExp,
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									pSkillTemplate->m_lID,
									szDesc,
									lCostMoney
									);
}