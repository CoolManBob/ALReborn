#include "AgpmEventSkillMaster.h"
#include "ApMemoryTracker.h"
#include "AgpmLog.h"

AgpmEventSkillMaster::AgpmEventSkillMaster()
{
	SetModuleName("AgpmEventSkillMaster");

	m_pcsApmEventManager	= NULL;
	m_pcsAgpmFactors		= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmGrid			= NULL;
	m_pcsAgpmItem			= NULL;
	m_pcsAgpmSkill			= NULL;

	SetPacketType(AGPMEVENT_SKILLMASTER_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,			// Operation
							AUTYPE_PACKET,		1,			// event base packet
							AUTYPE_INT32,		1,			// Character ID
							AUTYPE_INT32,		1,			// Skill ID
							AUTYPE_INT32,		1,			// Skill TID
							AUTYPE_INT8,		1,			// Operation Result
							AUTYPE_INT8,		1,			// skill point
							AUTYPE_INT8,		1,			// 초기화 했었는지 여부
							AUTYPE_END,			0
							);

	m_csPacketEventData.SetFlagLength(sizeof(INT8));
	m_csPacketEventData.SetFieldType(
							AUTYPE_INT8,		1,			// race type
							AUTYPE_INT8,		1,			// class type
							AUTYPE_END,			0
							);

	ZeroMemory(m_stMastery, sizeof(INT32) * AURACE_TYPE_MAX * AUCHARCLASS_TYPE_MAX * AGPMEVENT_SKILL_MAX_MASTERY * AGPMEVENT_SKILL_MAX_MASTERY_SKILL);

	if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
		m_pInitString = "initialized";
	else
		m_pInitString = "치약바르자";

}

AgpmEventSkillMaster::~AgpmEventSkillMaster()
{
}

BOOL AgpmEventSkillMaster::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmGrid			= (AgpmGrid *)			GetModule("AgpmGrid");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");
	
	if (!m_pcsApmEventManager ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill
		)
		return FALSE;

	m_nIndexADSkillTemplate	= m_pcsAgpmSkill->AttachSkillTemplateData(this, sizeof(AgpdEventSkillAttachTemplateData), NULL, NULL);
	if (m_nIndexADSkillTemplate < 0)
		return FALSE;

	m_nIndexADCharacterTemplate	= m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgpdEventSkillAttachCharTemplateData), CBCharTemplateConstructor, CBCharTemplateDestructor);
	if (m_nIndexADCharacterTemplate < 0)
		return FALSE;

	// Event Manager에 Event를 등록한다.
	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_SKILLMASTER, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterPacketFunction(CBMakePacketEventData, CBParsePacketEventData, this, APDEVENT_FUNCTION_SKILLMASTER))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventSkillMaster(CBActionSkillMaster, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackSetConstFactor(CBSetConstFactor, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackGetInputTotalCostSP(CBGetInputTotalCostSP, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRollbackSkill(CBRollbackSkill, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackCheckJoinItem(CBCheckJoinItem, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackGetInputTotalCostHeroicPoint(CBGetInputTotalCostHeroicPoint, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventSkillMaster::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation		= (-1);
	PVOID	pvPacketBase	= NULL;
	INT32	lCID			= AP_INVALID_CID;
	INT32	lSkillID		= AP_INVALID_SKILLID;
	INT32	lSkillTID		= AP_INVALID_SKILLID;
	INT8	cResult			= (-1);
	INT8	cSkillPoint		= (-1);
	INT8	cIsInitialize	= (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketBase,
						&lCID,
						&lSkillID,
						&lSkillTID,
						&cResult,
						&cSkillPoint,
						&cIsInitialize);

	ApdEvent		*pcsEvent		= NULL;
	
	if (pvPacketBase)
		pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketBase);
//	if (!pcsEvent)
//		return FALSE;

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	BOOL	bRetval		= TRUE;

	switch (cOperation) {
	case AGPMEVENT_SKILL_OPERATION_BUY_SKILL_BOOK:
		{
			bRetval	= OnOperationBuySkillBook(pcsEvent, pcsCharacter, lSkillTID);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_LEARN_SKILL:
		{
			bRetval	= OnOperationLearnSkill(pcsEvent, pcsCharacter, lSkillTID);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_BUY_RESULT:
		{
			bRetval	= OnOperationBuySkillResult(pcsCharacter, cResult);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_REQUEST_UPGRADE:
		{
			bRetval	= OnOperationRequestUpgrade(pcsEvent, pcsCharacter, lSkillID);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_RESPONSE_UPGRADE:
		{
			if (pstCheckArg->bReceivedFromServer)
				bRetval	= OnOperationResponseUpgrade(pcsEvent, pcsCharacter, lSkillID, cResult);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_REQUEST_EVENT:
		{
			bRetval	= OnOperationRequestEvent(pcsEvent, pcsCharacter);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_RESPONSE_EVENT:
		{
			if (pstCheckArg->bReceivedFromServer)
				bRetval	= OnOperationResponseEvent(pcsEvent, pcsCharacter, cResult, cIsInitialize);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_LEARN_RESULT:
		{
			if (pstCheckArg->bReceivedFromServer)
				bRetval	= OnOperationLearnResult(pcsCharacter, cResult);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_SKILL_INITIALIZE:
		{
			bRetval	= OnOperationSkillInitialize(pcsEvent, pcsCharacter);
		}
		break;

	case AGPMEVENT_SKILL_OPERATION_SKILL_INITIALIZE_RESULT:
		{
			bRetval	= OnOperationSkillInitializeResult(pcsCharacter, cResult);
		}
		break;
	}

	pcsCharacter->m_Mutex.Release();

	return	bRetval;
}

BOOL AgpmEventSkillMaster::OnOperationBuySkillBook(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillTID)
{
	if (!pcsEvent || !pcsCharacter || lSkillTID == AP_INVALID_SKILLID)
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]		= (PVOID) pcsCharacter;
	pvBuffer[1]		= IntToPtr(lSkillTID);

	return EnumCallback(AGPMEVENT_SKILL_CB_BUY_SKILL_BOOK, pcsEvent, pvBuffer);
}

BOOL AgpmEventSkillMaster::OnOperationLearnSkill(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillTID)
{
	if (!pcsEvent || !pcsCharacter || lSkillTID == AP_INVALID_SKILLID)
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]		= (PVOID) pcsCharacter;
	pvBuffer[1]		= IntToPtr(lSkillTID);

	return EnumCallback(AGPMEVENT_SKILL_CB_LEARN_SKILL, pcsEvent, pvBuffer);
}

BOOL AgpmEventSkillMaster::OnOperationBuySkillResult(AgpdCharacter *pcsCharacter, INT8 cResult)
{
	if (!pcsCharacter)
		return FALSE;

	return EnumCallback(AGPMEVENT_SKILL_CB_BUY_RESULT, pcsCharacter, (PVOID) cResult);
}

BOOL AgpmEventSkillMaster::OnOperationRequestUpgrade(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillID)
{
	if (!pcsEvent || !pcsCharacter || lSkillID == AP_INVALID_SKILLID)
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]		= (PVOID) pcsCharacter;
	pvBuffer[1]		= IntToPtr(lSkillID);

	return EnumCallback(AGPMEVENT_SKILL_CB_REQUEST_UPGRADE, pcsEvent, pvBuffer);
}

BOOL AgpmEventSkillMaster::OnOperationResponseUpgrade(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillID, INT8 cResult)
{
	if (!pcsEvent || !pcsCharacter || lSkillID == AP_INVALID_SKILLID)
		return FALSE;

	PVOID	pvBuffer[3];
	pvBuffer[0]		= (PVOID) pcsCharacter;
	pvBuffer[1]		= IntToPtr(lSkillID);
	pvBuffer[2]		= IntToPtr(cResult);

	return EnumCallback(AGPMEVENT_SKILL_CB_RESPONSE_UPGRADE, pcsEvent, pvBuffer);
}

BOOL AgpmEventSkillMaster::OnOperationRequestEvent(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	if (!CheckCharacterStatus(pcsEvent, pcsCharacter))
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]		= (PVOID) pcsEvent;
		pvBuffer[1]		= (PVOID) (INT32) AGPMEVENT_SKILL_REQUEST_RESULT_FAIL;

		EnumCallback(AGPMEVENT_SKILL_CB_REQUEST_EVENT, pcsCharacter, pvBuffer);

		return FALSE;
	}

	AuPOS	stTargetPos	= {0,0,0};

	if (CheckValidRange(pcsEvent, pcsCharacter, &stTargetPos))
	{
		// ok
		PVOID	pvBuffer[2];
		pvBuffer[0]		= (PVOID) pcsEvent;
		pvBuffer[1]		= (PVOID) (INT32) AGPMEVENT_SKILL_REQUEST_RESULT_SUCCESS;

		return EnumCallback(AGPMEVENT_SKILL_CB_REQUEST_EVENT, pcsCharacter, pvBuffer);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_SKILL_MASTER;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		return m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::OnOperationResponseEvent(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT8 cResult, INT8 cIsInitialize)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	PVOID	pvBuffer[3];
	pvBuffer[0]		= (PVOID) pcsEvent;
	pvBuffer[1]		= (PVOID) cResult;
	pvBuffer[2]		= (PVOID) cIsInitialize;

	return EnumCallback(AGPMEVENT_SKILL_CB_RESPONSE_EVENT, pcsCharacter, pvBuffer);
}

BOOL AgpmEventSkillMaster::OnOperationLearnResult(AgpdCharacter *pcsCharacter, INT8 cResult)
{
	if (!pcsCharacter)
		return FALSE;

	return EnumCallback(AGPMEVENT_SKILL_CB_LEARN_RESULT, pcsCharacter, (PVOID) &cResult);
}

BOOL AgpmEventSkillMaster::OnOperationSkillInitialize(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (strcmp(pcsCharacter->m_szSkillInit, m_pInitString) == 0)
		return FALSE;

	// 오키 이놈은 초기화 시켜준다.
	return InitializeSkillTree(pcsCharacter);
}

BOOL AgpmEventSkillMaster::OnOperationSkillInitializeResult(AgpdCharacter *pcsCharacter, INT8 cResult)
{
	if (!pcsCharacter)
		return FALSE;

	return EnumCallback(AGPMEVENT_SKILL_CB_SKILL_INITIALIZE_RESULT, pcsCharacter, &cResult);
}

PVOID AgpmEventSkillMaster::MakePacketBuySkill(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillTID, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		lSkillTID == AP_INVALID_SKILLID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_BUY_SKILL_BOOK;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													NULL,
													&lSkillTID,
													NULL,
													NULL,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSkillMaster::MakePacketLearnSkill(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillTID, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		lSkillTID == AP_INVALID_SKILLID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_LEARN_SKILL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													NULL,
													&lSkillTID,
													NULL,
													NULL,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSkillMaster::MakePacketRequestUpgrade(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillID, INT32 lUpgradedSkillPoint, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		lSkillID == AP_INVALID_SKILLID ||
		lUpgradedSkillPoint < 1 ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_REQUEST_UPGRADE;
	INT8	cSkillPoint		= (INT8) lUpgradedSkillPoint;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													&lSkillID,
													NULL,
													NULL,
													&cSkillPoint,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSkillMaster::MakePacketResponseUpgrade(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillID, AgpmEventSkillUpgradeResult eResult, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		lSkillID == AP_INVALID_SKILLID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_RESPONSE_UPGRADE;
	INT8	cUpgradeResult	= (INT8) eResult;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													&lSkillID,
													NULL,
													&cUpgradeResult,
													NULL,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSkillMaster::MakePacketRequestEvent(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_REQUEST_EVENT;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSkillMaster::MakePacketResponseEvent(ApdEvent *pcsEvent, INT32 lCID, AgpmEventRequestResult eResult, INT16 *pnPacketLength, INT8 cIsInitialize)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_RESPONSE_EVENT;
	INT8	cResult			= (INT8) eResult;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID,
													NULL,
													NULL,
													&cResult,
													NULL,
													&cIsInitialize);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSkillMaster::MakePacketBuySkillResult(INT32 lCID, AgpmEventSkillBuyResult eResult, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_BUY_RESULT;
	INT8	cResult			= (INT8) eResult;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													NULL,
													&lCID,
													NULL,
													NULL,
													&cResult,
													NULL,
													NULL);
}

PVOID AgpmEventSkillMaster::MakePacketLearnResult(INT32 lCID, AgpmEventSkillLearnResult eResult, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_LEARN_RESULT;
	INT8	cResult			= (INT8) eResult;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													NULL,
													&lCID,
													NULL,
													NULL,
													&cResult,
													NULL,
													NULL);
}

PVOID AgpmEventSkillMaster::MakePacketRequestSkillInitialize(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_SKILL_INITIALIZE;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													NULL,
													&lCID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);
}

PVOID AgpmEventSkillMaster::MakePacketResponseSkillInitialize(INT32 lCID, INT8 cResult, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMEVENT_SKILL_OPERATION_SKILL_INITIALIZE_RESULT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SKILLMASTER_PACKET_TYPE,
													&cOperation,
													NULL,
													&lCID,
													NULL,
													NULL,
													&cResult,
													NULL,
													NULL);
}

PVOID AgpmEventSkillMaster::MakePacketEventData(ApdEvent *pcsEvent)
{
	if (!pcsEvent || !pcsEvent->m_pvData)
		return NULL;

	AgpdSkillEventAttachData	*pcsAttachData	= (AgpdSkillEventAttachData *)	pcsEvent->m_pvData;

	INT8	cRaceType	= (INT8) pcsAttachData->eRaceType;
	INT8	cClassType	= (INT8) pcsAttachData->eClassType;

	return m_csPacketEventData.MakePacket(FALSE, NULL, 0,
											&cRaceType,
											&cClassType);
}

BOOL AgpmEventSkillMaster::ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData)
{
	if (!pcsEvent || !pcsEvent->m_pvData || !pvPacketCustomData)
		return FALSE;

	INT8	cRaceType	= AURACE_TYPE_NONE;
	INT8	cClassType	= AUCHARCLASS_TYPE_NONE;

	m_csPacketEventData.GetField(FALSE, pvPacketCustomData, 0,
											&cRaceType,
											&cClassType);

	AgpdSkillEventAttachData	*pcsAttachData	= (AgpdSkillEventAttachData *)	pcsEvent->m_pvData;

	pcsAttachData->eRaceType	= (AuRaceType)		cRaceType;
	pcsAttachData->eClassType	= (AuCharClassType)	cClassType;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CheckCharacterStatus(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if (m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CheckValidRange(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, AuPOS *pstDestPos)
{
	if (!pcsEvent || !pcsCharacter || !pstDestPos)
		return FALSE;

	return m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_SKILL_MAX_USE_RANGE, pstDestPos);
}

BOOL AgpmEventSkillMaster::CheckBuySkillBook(AgpdCharacter *pcsCharacter, INT32 lSkillTID)
{
	return CheckBuySkillBook(pcsCharacter, m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));
}

BOOL AgpmEventSkillMaster::CheckBuySkillBook(AgpdCharacter *pcsCharacter, AgpdSkillTemplate *pcsSkillTemplate)
{
	if (!pcsCharacter || !pcsSkillTemplate)
		return FALSE;

	// lSkillTID 를 산단다. 돈이 충분한지 보구.. 있으면 사준다.

	INT32	lCost	= GetBuyCost(pcsSkillTemplate, pcsCharacter);

	INT64	llMoney	= 0;
	m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llMoney);

	if (llMoney < (INT64) lCost)
		return FALSE;

	// 인벤토리에 빈 공간이 있는지 본다.
	AgpdItemADChar	*pcsAttachItemData	= m_pcsAgpmItem->GetADCharacter(pcsCharacter);

	if (m_pcsAgpmGrid->IsFullGrid(&pcsAttachItemData->m_csInventoryGrid))
		return FALSE;

	return TRUE;
}

AgpmEventSkillLearnResult AgpmEventSkillMaster::CheckLearnSkill(AgpdCharacter *pcsCharacter, AgpdSkillTemplate *pcsSkillTemplate , INT32	nCharacterTID )
{
	if (!pcsCharacter || !pcsSkillTemplate)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	// 2005.05.12. steeple
	// 변신중에는 배울 수 없다.
	if (pcsCharacter->m_bIsTrasform)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	// 이미 배운넘인지 본다.
	if (m_pcsAgpmSkill->IsOwnSkill((ApBase *) pcsCharacter, pcsSkillTemplate))
		return AGPMEVENT_SKILL_LEARN_RESULT_ALREADY_LEARN;

	// 임시로 고렙 스킬 배울 수 있게 해놨다.
#ifdef _HIGHLEVEL_SKILL_TEST_
	AgpdEventSkillHighLevel stHighLevel = GetHighLevelSkillInfo(pcsSkillTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID);
	if(stHighLevel.m_lSkillTID != 0)
		return AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS;
#endif

	// 배울 수 있는 클래스인지 본다.
	if (!m_pcsAgpmSkill->CheckLearnableSkill((ApBase *) pcsCharacter, pcsSkillTemplate->m_szName))
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_LEARNABLE_CLASS;

	INT32	lCostMoney	= GetBuyCost(pcsSkillTemplate, pcsCharacter);
	if (lCostMoney < 0)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	INT64	llInvenMoney	= 0;
	m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llInvenMoney);

	if (llInvenMoney < (INT64) lCostMoney)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	// SP가 충분히 있는지 본다.
	INT32	lCostSP	= GetLearnCostSP(pcsSkillTemplate);
	if (lCostSP < 0)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_SKILLPOINT;

	INT32	lLearnableLevel	= GetLearnableLevel(pcsSkillTemplate);
	if (lLearnableLevel < 0)
		return AGPMEVENT_SKILL_LEARN_RESULT_LOW_LEVEL;

	if (m_pcsAgpmCharacter->GetLevel(pcsCharacter) < lLearnableLevel)
		return AGPMEVENT_SKILL_LEARN_RESULT_LOW_LEVEL;

	INT32	lCharSP	= m_pcsAgpmCharacter->GetSkillPoint(pcsCharacter);

	if (lCharSP < lCostSP)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_SKILLPOINT;

	// 2007.05.26. steeple
	AgpmEventSkillLearnResult eResult = CheckLearnHighLevelSkill(pcsCharacter, pcsSkillTemplate, nCharacterTID);
	if(eResult != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
		return eResult;

	if( IsHeroicSkill( pcsSkillTemplate->m_lID , nCharacterTID ) )
	{
		AgpmEventSkillLearnResult eResultHeroic = CheckLearnHeroicSkill(pcsCharacter, pcsSkillTemplate, nCharacterTID);
		if(eResultHeroic != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
			return eResultHeroic;
	}

	return AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS;
}

// 고렙 스킬 배울 수 있는 지 체크한다.
AgpmEventSkillLearnResult AgpmEventSkillMaster::CheckLearnHighLevelSkill(AgpdCharacter* pcsCharacter, AgpdSkillTemplate* pcsSkillTemplate , INT32	nCharacterTID)
{
	if(!pcsCharacter || !pcsCharacter->m_pcsCharacterTemplate || !pcsSkillTemplate)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	INT32		nCharTID;

	// nCharacterTID에 값이 들어있으면 클라에서 쓰는거다 ㅎ
	if( nCharacterTID )
		nCharTID	=	nCharacterTID;
	else
		nCharTID	=	pcsCharacter->m_pcsCharacterTemplate->m_lID;

	AgpdEventSkillHighLevel stHighLevel = GetHighLevelSkillInfo(pcsSkillTemplate->m_lID, nCharTID);

	// 고렙 스킬이 아니다.
	if(stHighLevel.m_lSkillTID == 0)
	{
		INT32 lRace = GetCharacterRaceID(pcsCharacter);
		if(lRace == NULL)
			return AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS;

		stHighLevel = GetHighLevelSkillInfo(pcsSkillTemplate->m_lID, lRace);
		
		if(stHighLevel.m_lSkillTID == 0)
			return AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS;
	}

	// Condition Check
	BOOL bCheck = TRUE;
	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION; ++i)
	{
		if(stHighLevel.m_stCondition[i].m_lSkillTotalPoint != 0)
		{
			if(m_pcsAgpmSkill->GetTotalSkillPoint(pcsCharacter) < stHighLevel.m_stCondition[i].m_lSkillTotalPoint)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(stHighLevel.m_stCondition[i].m_lSkillTab != 0)
		{
			AgpdCharacterTemplate* pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(stHighLevel.m_stCondition[i].m_lSkillTab);
			if(!pcsCharacterTemplate)
				return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

			INT32	lBaseRace  = m_pcsAgpmFactors->GetRace(&pcsCharacterTemplate->m_csFactor);
			INT32  lBaseClass = m_pcsAgpmFactors->GetClass(&pcsCharacterTemplate->m_csFactor);

			if(m_pcsAgpmSkill->GetTotalSkillPoint(pcsCharacter, lBaseRace, lBaseClass) < stHighLevel.m_stCondition[i].m_lSkillPoint)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(stHighLevel.m_stCondition[i].m_lSkillTID != 0)
		{
			AgpdSkillTemplate* pcsConditionTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_stCondition[i].m_lSkillTID);
			if(!pcsConditionTemplate)
				break;

			// 캐릭터가 스킬을 배웠는 지 체크.
			AgpdSkill* pcsConditionSkill = m_pcsAgpmSkill->GetSkill(pcsCharacter, pcsConditionTemplate->m_szName);
			if(!pcsConditionSkill)
			{
				bCheck = FALSE;
				break;
			}

			// 레벨이 도달했는 지 체크.
			INT32 lConditionLevel = m_pcsAgpmSkill->GetSkillLevel(pcsConditionSkill);
			if(lConditionLevel < stHighLevel.m_stCondition[i].m_lSkillLevel)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(!bCheck) break;
	}

	if(!bCheck)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	return AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS;
}

// Check Enable to Learn Heroic Skill
AgpmEventSkillLearnResult AgpmEventSkillMaster::CheckLearnHeroicSkill(AgpdCharacter* pcsCharacter, AgpdSkillTemplate* pcsSkillTemplate, INT32 nCharacterTID)
{
	if(NULL == pcsCharacter || NULL == pcsSkillTemplate || NULL == pcsCharacter->m_pcsCharacterTemplate)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;

	INT32		nCharTID;

	if(nCharacterTID)
		nCharTID = nCharacterTID;
	else
		nCharTID = pcsCharacter->m_pcsCharacterTemplate->m_lID;

	// It is not a Heroic Skill
	if(IsHeroicSkill(pcsSkillTemplate->m_lID, nCharTID) == FALSE)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_LEARNABLE_CLASS;

	// Heroic Point Check
	INT32 lCostHeroicPoint = GetLearnCostHeroicPoint(pcsSkillTemplate);
	if(lCostHeroicPoint < 0)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_HEROICPOINT;

	INT32 lCharHeroicPoint = m_pcsAgpmCharacter->GetHeroicPoint(pcsCharacter);

	if(lCharHeroicPoint < lCostHeroicPoint)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_HEROICPOINT;

	// Charisma Point Check
	INT32 lCostCharismaPoint = GetLearnCostCharismaPoint(pcsSkillTemplate);
	if(lCostCharismaPoint < 0)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_CHARISMAPOINT;

	INT32 lCharCharismaPoint = m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);

	if(lCharCharismaPoint < lCostCharismaPoint)
		return AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_CHARISMAPOINT;

	// Check Heroic Skill Connection
	AgpdEventSkillHeroic stHeroicSkill = GetHeroicSkillInfo(pcsSkillTemplate->m_lID, nCharTID);

	BOOL bCheck = TRUE;
	for(int i =0; i < AGPMEVENT_SKILL_MAX_HEROIC_CONDITION; ++i)
	{
		// Check user's total used heroic point
		if(stHeroicSkill.m_stCondition[i].m_lUsedTotalHeroicPoint != 0)
		{
			if(m_pcsAgpmSkill->GetTotalUsedHeroicPoint(pcsCharacter) < stHeroicSkill.m_stCondition[i].m_lUsedTotalHeroicPoint)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(stHeroicSkill.m_stCondition[i].m_lSkillTID != 0)
		{
			AgpdSkillTemplate *pcsConditionTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHeroicSkill.m_stCondition[i].m_lSkillTID);
			if(NULL == pcsConditionTemplate)
				break;

			// Check This Skill is learned by Character
			AgpdSkill *pcsCondtionSkill = m_pcsAgpmSkill->GetSkill(pcsCharacter, pcsConditionTemplate->m_szName);
			if(NULL == pcsCondtionSkill)
			{
				bCheck = FALSE;
				break;
			}

			// Check This Skill is approached to Level
			INT32 lConditionLevel = m_pcsAgpmSkill->GetSkillLevel(pcsCondtionSkill);
			if(lConditionLevel < stHeroicSkill.m_stCondition[i].m_lSkillLevel)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(!bCheck) break;
	}

	if(!bCheck)
		return AGPMEVENT_SKILL_LEARN_RESULT_FAIL;
	
	return AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS;
}

AgpmEventSkillUpgradeResult AgpmEventSkillMaster::CheckUpgradeSkill(AgpdCharacter *pcsCharacter, AgpdSkill *pcsSkill)
{
	if (!pcsCharacter || !pcsSkill || !pcsSkill->m_pcsBase)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	if (pcsCharacter->m_lID != pcsSkill->m_pcsBase->m_lID)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	// 2005.05.12. steeple
	// 변신중에는 업그레이드 할 수 없다.
	if (pcsCharacter->m_bIsTrasform)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	// 임시로 고렙 스킬 배울 수 있게 해놨다.
#ifdef _HIGHLEVEL_SKILL_TEST_
	AgpdEventSkillHighLevel stHighLevel = GetHighLevelSkillInfo(pcsSkill->m_pcsTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID);
	if(stHighLevel.m_lSkillTID != 0)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS;
#endif

	if (!IsUpgradable(pcsSkill))
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	if (IsFullUpgrade(pcsSkill))
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	INT32	lCostMoney	= GetUpgradeCost(pcsSkill, pcsCharacter);
	if (lCostMoney < 0)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	INT64	llInvenMoney	= 0;
	m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llInvenMoney);

	if (llInvenMoney < (INT64) lCostMoney)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_MONEY;

	INT32	lCostSP		= GetUpgradeCostSP(pcsSkill);
	if (lCostSP < 0)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	INT32	lCharSP		= m_pcsAgpmCharacter->GetSkillPoint(pcsCharacter);

	if (lCharSP < lCostSP)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_SKILLPOINT;
	
	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP-1)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	INT32	lCharLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);

	if (lCharLevel < ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	if(!m_pcsAgpmSkill->CheckRequireClass(pcsCharacter, (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	// 업그레이드 할 때는 고렙스킬에만 따로 적용되는 이렇다할 제약은 없다.
	AgpmEventSkillUpgradeResult eResult = CheckUpgradeHighLevelSkill(pcsCharacter, pcsSkill);
	if(eResult != AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS)
		return eResult;

	if(IsHeroicSkill(pcsSkill->m_pcsTemplate->m_lID))
	{
		AgpmEventSkillUpgradeResult eResultHeroic = CheckUpgradeHeroicSkill(pcsCharacter, pcsSkill);
		if(eResultHeroic != AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS)
			return eResultHeroic;
	}

	return AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS;
}

// 2007.07.08. steeple
// 고레벨 스킬 업그레이드 할 수 잇는 지 체크한다.
AgpmEventSkillUpgradeResult	AgpmEventSkillMaster::CheckUpgradeHighLevelSkill(AgpdCharacter *pcsCharacter, AgpdSkill *pcsSkill)
{
	if(!pcsCharacter || !pcsCharacter->m_pcsCharacterTemplate || !pcsSkill || !pcsSkill->m_pcsTemplate)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	AgpdEventSkillHighLevel stHighLevel = GetHighLevelSkillInfo(pcsSkillTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID);

	// 고렙 스킬이 아니다.
	if(stHighLevel.m_lSkillTID == 0)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS;

	// Condition Check
	BOOL bCheck = TRUE;
	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION; ++i)
	{
		if(stHighLevel.m_stCondition[i].m_lSkillTotalPoint != 0)
		{
			if(m_pcsAgpmSkill->GetTotalSkillPoint(pcsCharacter) < stHighLevel.m_stCondition[i].m_lSkillTotalPoint)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(stHighLevel.m_stCondition[i].m_lSkillTab != 0)
		{
			AgpdCharacterTemplate* pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(stHighLevel.m_stCondition[i].m_lSkillTab);
			if(!pcsCharacterTemplate)
				return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

			 INT32	lBaseRace  = m_pcsAgpmFactors->GetRace(&pcsCharacterTemplate->m_csFactor);
			 INT32  lBaseClass = m_pcsAgpmFactors->GetClass(&pcsCharacterTemplate->m_csFactor);

			 if(m_pcsAgpmSkill->GetTotalSkillPoint(pcsCharacter, lBaseRace, lBaseClass) < stHighLevel.m_stCondition[i].m_lSkillPoint)
			 {
				 bCheck = FALSE;
				 break;
			 }
		}

		if(stHighLevel.m_stCondition[i].m_lSkillTID != 0)
		{
			AgpdSkillTemplate* pcsConditionTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_stCondition[i].m_lSkillTID);
			if(!pcsConditionTemplate)
				break;

			// 캐릭터가 스킬을 배웠는 지 체크.
			AgpdSkill* pcsConditionSkill = m_pcsAgpmSkill->GetSkill(pcsCharacter, pcsConditionTemplate->m_szName);
			if(!pcsConditionSkill)
			{
				bCheck = FALSE;
				break;
			}

			// 레벨이 도달했는 지 체크.
			INT32 lConditionLevel = m_pcsAgpmSkill->GetSkillLevel(pcsConditionSkill);
			if(lConditionLevel < stHighLevel.m_stCondition[i].m_lSkillLevel)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(!bCheck) break;
	}

	if(!bCheck)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	return AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS;
}

AgpmEventSkillUpgradeResult AgpmEventSkillMaster::CheckUpgradeHeroicSkill(AgpdCharacter* pcsCharacter, AgpdSkill *pcsSkill)
{
	if(NULL == pcsCharacter || NULL == pcsCharacter->m_pcsCharacterTemplate || NULL == pcsSkill)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	INT32		nCharTID;

	nCharTID = pcsCharacter->m_pcsCharacterTemplate->m_lID;

	AgpdSkillTemplate *pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsSkill->m_pcsTemplate->m_lID);
	if(NULL == pcsSkillTemplate)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	// It is not a Heroic Skill
	if(IsHeroicSkill(pcsSkillTemplate->m_lID, nCharTID) == FALSE)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS;

	// Heroic Point Check
	INT32 lCostHeroicPoint = GetUpgradeCostHeroicPoint(pcsSkill);
	if(lCostHeroicPoint < 0)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_HEROICPOINT;

	INT32 lCharHeroicPoint = m_pcsAgpmCharacter->GetHeroicPoint(pcsCharacter);

	if(lCharHeroicPoint < lCostHeroicPoint)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_HEROICPOINT;

	// Charisma Point Check
	INT32 lCostCharismaPoint = GetUpgradeCostCharismaPoint(pcsSkill);
	if(lCostCharismaPoint < 0)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_CHARISMAPOINT;

	INT32 lCharCharismaPoint = m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);

	if(lCharCharismaPoint < lCostCharismaPoint)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_CHARISMAPOINT;

	// Check Heroic Skill Connection
	AgpdEventSkillHeroic stHeroicSkill = GetHeroicSkillInfo(pcsSkillTemplate->m_lID, nCharTID);

	BOOL bCheck = TRUE;
	for(int i =0; i < AGPMEVENT_SKILL_MAX_HEROIC_CONDITION; ++i)
	{
		if(stHeroicSkill.m_stCondition[i].m_lSkillTID != 0)
		{
			AgpdSkillTemplate *pcsConditionTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHeroicSkill.m_stCondition[i].m_lSkillTID);
			if(NULL == pcsConditionTemplate)
				break;

			// Check This Skill is learned by Character
			AgpdSkill *pcsCondtionSkill = m_pcsAgpmSkill->GetSkill(pcsCharacter, pcsConditionTemplate->m_szName);
			if(NULL == pcsCondtionSkill)
			{
				bCheck = FALSE;
				break;
			}

			// Check This Skill is approached to Level
			INT32 lConditionLevel = m_pcsAgpmSkill->GetSkillLevel(pcsCondtionSkill);
			if(lConditionLevel < stHeroicSkill.m_stCondition[i].m_lSkillLevel)
			{
				bCheck = FALSE;
				break;
			}
		}

		if(!bCheck) break;
	}

	if(!bCheck)
		return AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL;

	return AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS;
}

BOOL AgpmEventSkillMaster::CBActionSkillMaster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis				= (AgpmEventSkillMaster *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pstNextAction		= (AgpdCharacterAction *)	pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	ApdEvent				*pcsEvent			= pThis->m_pcsApmEventManager->GetEvent(pstNextAction->m_csTargetBase.m_eType, pstNextAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_SKILLMASTER);
	if (!pcsEvent)
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]		= (PVOID) pcsEvent;
	pvBuffer[1]		= (PVOID) (INT32) AGPMEVENT_SKILL_REQUEST_RESULT_SUCCESS;

	return pThis->EnumCallback(AGPMEVENT_SKILL_CB_REQUEST_EVENT, pcsCharacter, pvBuffer);
}

BOOL AgpmEventSkillMaster::SetCallbackBuySkillBook(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_BUY_SKILL_BOOK, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackLearnSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_LEARN_SKILL, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackRequestUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_REQUEST_UPGRADE, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackResponseUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_RESPONSE_UPGRADE, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackRequestEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_REQUEST_EVENT, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackResponseEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_RESPONSE_EVENT, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackBuyResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_BUY_RESULT, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackSaveAllData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_SAVE_ALL_DATA, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackRemoveSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_REMOVE_SKILL, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackEndAllSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_END_ALL_SKILL, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackEndSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_END_SKILL, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackInitSkillLog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_SKILL_INIT_LOG, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackLearnResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_LEARN_RESULT, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackSkillInitialize(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_SKILL_INITIALIZE, pfCallback, pClass);
}

BOOL AgpmEventSkillMaster::SetCallbackSkillInitializeResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKILL_CB_SKILL_INITIALIZE_RESULT, pfCallback, pClass);
}

INT32* AgpmEventSkillMaster::GetMastery(AuRaceType eRaceType, AuCharClassType eClassType, INT32 lMasteryIndex)
{
	if (eRaceType <= AURACE_TYPE_NONE || eRaceType >= AURACE_TYPE_MAX ||
		eClassType <= AUCHARCLASS_TYPE_NONE || eClassType >= AUCHARCLASS_TYPE_MAX ||
		lMasteryIndex < 0 || lMasteryIndex >= AGPMEVENT_SKILL_MAX_MASTERY)
		return NULL;

	return m_stMastery[eRaceType][eClassType][lMasteryIndex];
}

INT32 AgpmEventSkillMaster::GetCharTID(AuRaceType eRaceType, AuCharClassType eClassType)
{
	switch(eRaceType)
	{
		case AURACE_TYPE_HUMAN:
		{
			switch(eClassType)
			{
				case AUCHARCLASS_TYPE_KNIGHT:	return g_alPCTID[0];
				case AUCHARCLASS_TYPE_RANGER:	return g_alPCTID[1];
				case AUCHARCLASS_TYPE_SCION:	return g_alPCTID[9];
				case AUCHARCLASS_TYPE_MAGE:		return g_alPCTID[2];
				default:						break;
			}
			break;
		}

		case AURACE_TYPE_ORC:
		{
			switch(eClassType)
			{
				case AUCHARCLASS_TYPE_KNIGHT:	return g_alPCTID[3];
				case AUCHARCLASS_TYPE_RANGER:	return g_alPCTID[4];
				case AUCHARCLASS_TYPE_SCION:	return g_alPCTID[10];
				case AUCHARCLASS_TYPE_MAGE:		return g_alPCTID[5];
				default:						break;
			}
			break;
		}

		case AURACE_TYPE_MOONELF:
		{
			switch(eClassType)
			{
				case AUCHARCLASS_TYPE_RANGER:	return g_alPCTID[7];
				case AUCHARCLASS_TYPE_MAGE:		return g_alPCTID[6];
				case AUCHARCLASS_TYPE_SCION:	return g_alPCTID[11];
				case AUCHARCLASS_TYPE_KNIGHT:	return g_alPCTID[8];
				default:						break;
			}
			break;
		}

		case AURACE_TYPE_DRAGONSCION:
		{
			switch(eClassType)
			{
				case AUCHARCLASS_TYPE_KNIGHT:	return g_alPCTID[12];
				case AUCHARCLASS_TYPE_RANGER:	return g_alPCTID[13];
				case AUCHARCLASS_TYPE_SCION:	return g_alPCTID[14];
				case AUCHARCLASS_TYPE_MAGE:		return g_alPCTID[15];
				default:						break;
			}
			break;
		}
	}

	return -1;
}

AgpdSkill* AgpmEventSkillMaster::GetSkill(AgpdCharacter *pcsCharacter, INT32 lSkillTID)
{
	return m_pcsAgpmSkill->GetSkillByTID(pcsCharacter, lSkillTID);
}

INT32 AgpmEventSkillMaster::GetOwnSkillCount(AgpdCharacter* pcsCharacter, BOOL bIncludeProduct, BOOL bIncludeDefault, BOOL bIncludeFreeCost)
{
	if (!pcsCharacter)
		return -1;

	BOOL bResult = TRUE;

	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	AgpdEventSkillAttachCharTemplateData	*pcsCharTemplateAttachData	= GetSkillAttachCharTemplateData(pcsCharacter->m_pcsCharacterTemplate);

	INT32 lCount = 0;
	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (!pcsSkill)
			break;

		if (!bIncludeProduct && ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
			continue;

		if (!bIncludeDefault && pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate)
		{
			for (int j = 0; j < pcsCharTemplateAttachData->m_lNumDefaultSkill; ++j)
			{
				if (pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate[j] == (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)
					continue;
			}
		}

		if (!bIncludeFreeCost && GetBuyCost((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, pcsCharacter) == 0)
			continue;

		++lCount;
	}

	return lCount;
}

INT32 AgpmEventSkillMaster::GetBuyCost(INT32 lSkillTID, AgpdCharacter *pcsCharacter, INT32 *plTax)
{
	return GetBuyCost(m_pcsAgpmSkill->GetSkillTemplate(lSkillTID), pcsCharacter, plTax);
}

INT32 AgpmEventSkillMaster::GetBuyCost(AgpdSkillTemplate *pcsSkillTemplate, AgpdCharacter *pcsCharacter, INT32 *plTax)
{
	if (!pcsSkillTemplate)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData(pcsSkillTemplate);

	INT32 lSkillPrice = pcsAttachTemplateData->m_stSkillCost[1].lCostMoney;
	
	// apply tax
	INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = (lSkillPrice * lTaxRatio) / 100;
	}
	lSkillPrice = lSkillPrice + lTax;	
	if (plTax)
		*plTax = lTax;
	
	return lSkillPrice;
}

INT32 AgpmEventSkillMaster::GetLearnCostSP(INT32 lSkillTID)
{
	return GetLearnCostSP(m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));
}

INT32 AgpmEventSkillMaster::GetLearnCostSP(AgpdSkillTemplate *pcsSkillTemplate)
{
	if (!pcsSkillTemplate)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData(pcsSkillTemplate);

	return pcsAttachTemplateData->m_stSkillCost[1].lCostSkillPoint;
}

INT32 AgpmEventSkillMaster::GetLearnCostHeroicPoint(INT32 lSkillTID)
{
	return GetLearnCostHeroicPoint(m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));
}

INT32 AgpmEventSkillMaster::GetLearnCostHeroicPoint(AgpdSkillTemplate *pcsSkillTemplate)
{
	if(!pcsSkillTemplate)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData = GetSkillAttachTemplateData(pcsSkillTemplate);
	if(!pcsAttachTemplateData)
		return (-1);

	return pcsAttachTemplateData->m_stSkillCost[1].lCostHeroicPoint;
}

INT32 AgpmEventSkillMaster::GetLearnCostCharismaPoint(INT32 lSkillTID)
{
	return GetLearnCostCharismaPoint(m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));
}

INT32 AgpmEventSkillMaster::GetLearnCostCharismaPoint(AgpdSkillTemplate *pcsSkillTemplate)
{
	if(NULL == pcsSkillTemplate)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData = GetSkillAttachTemplateData(pcsSkillTemplate);

	return pcsAttachTemplateData->m_stSkillCost[1].lCostCharismaPoint;
}

INT32 AgpmEventSkillMaster::GetLearnableLevel(INT32 lSkillTID)
{
	return GetLearnableLevel(m_pcsAgpmSkill->GetSkillTemplate(lSkillTID));
}

INT32 AgpmEventSkillMaster::GetLearnableLevel(AgpdSkillTemplate *pcsSkillTemplate)
{
	if (!pcsSkillTemplate)
		return (-1);

	return (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][1];
}

INT32 AgpmEventSkillMaster::GetUpgradeCost(AgpdSkill *pcsSkill, AgpdCharacter *pcsCharacter, INT32 *plTax)
{
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return (-1);

	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1)
		return (-1);

	if (lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL-1) // 더이상 업그레이드 할수 없음
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

	if (pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel <= lSkillLevel)
		return (-1);

	INT32 lSkillPrice = pcsAttachTemplateData->m_stSkillCost[lSkillLevel + 1].lCostMoney;

	// apply tax
	INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = (lSkillPrice * lTaxRatio) / 100;
	}
	lSkillPrice = lSkillPrice + lTax;	
	if (plTax)
		*plTax = lTax;
	
	return lSkillPrice;
}

INT32 AgpmEventSkillMaster::GetUpgradeCostSP(AgpdSkill *pcsSkill)
{
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return (-1);

	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1)
		return (-1);

	if (lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL-1) // 더이상 업그레이드 할수 없음
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

	if (pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel <= lSkillLevel)
		return (-1);

	return pcsAttachTemplateData->m_stSkillCost[lSkillLevel + 1].lCostSkillPoint;
}

INT32 AgpmEventSkillMaster::GetUpgradeCostHeroicPoint(AgpdSkill *pcsSkill)
{
	if(NULL == pcsSkill)
		return (-1);

	INT32	lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if(lSkillLevel < 1)
		return (-1);

	if(lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL-1)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData = GetSkillAttachTemplateData((AgpdSkillTemplate *)pcsSkill->m_pcsTemplate);
	if(NULL == pcsAttachTemplateData)
		return (-1);

	if(pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel <= lSkillLevel)
		return (-1);

	return pcsAttachTemplateData->m_stSkillCost[lSkillLevel + 1].lCostHeroicPoint;
}

INT32 AgpmEventSkillMaster::GetUpgradeCostCharismaPoint(AgpdSkill *pcsSkill)
{
	if(NULL == pcsSkill)
		return (-1);

	INT32	lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if(lSkillLevel < 1)
		return (-1);

	if(lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL-1)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData = GetSkillAttachTemplateData((AgpdSkillTemplate *)pcsSkill->m_pcsTemplate);
	if(NULL == pcsAttachTemplateData)
		return (-1);

	if(pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel <= lSkillLevel)
		return (-1);

	return pcsAttachTemplateData->m_stSkillCost[lSkillLevel + 1].lCostCharismaPoint;
}

INT32 AgpmEventSkillMaster::GetInputTotalCostSP(AgpdSkill *pcsSkill)
{
	if (!pcsSkill)
		return 0;

	INT32	lTotalCostSP	= GetLearnCostSP((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL)
		return (-1);

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

	if (pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel < lSkillLevel)
		return (-1);

	for (int i = 1; i < lSkillLevel; ++i)
	{
		lTotalCostSP	+= pcsAttachTemplateData->m_stSkillCost[i].lCostSkillPoint;
	}

	return lTotalCostSP;
}

INT32 AgpmEventSkillMaster::GetInputTotalCostHeroicPoint(AgpdSkill *pcsSkill)
{
	if(NULL == pcsSkill)
		return 0;

	INT32	lTotalHeroicPoint = GetLearnCostHeroicPoint((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);

	INT32	lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if(lSkillLevel < 1 || lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL)
		return 0;

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData = GetSkillAttachTemplateData((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);
	if(NULL == pcsAttachTemplateData)
		return 0;

	if (pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel < lSkillLevel)
		return 0;

	for(int i = 1; i < lSkillLevel; ++i)
	{
		lTotalHeroicPoint += pcsAttachTemplateData->m_stSkillCost[i].lCostSkillPoint;
	}

	return lTotalHeroicPoint;
}

INT32 AgpmEventSkillMaster::GetInputTotalCharismaPoint(AgpdSkill *pcsSkill)
{
	if(NULL == pcsSkill)
		return 0;

	INT32 lTotalCharismaPoint = 0;

	INT32	lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if(lSkillLevel < 1 || lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL)
		return 0;

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData = GetSkillAttachTemplateData((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);
	if(NULL == pcsAttachTemplateData)
		return 0;

	if (pcsAttachTemplateData->lMaxUpgradeLevel < 1 || pcsAttachTemplateData->lMaxUpgradeLevel < lSkillLevel)
		return 0;

	for(int i = lSkillLevel; i > 0; --i)
	{
		lTotalCharismaPoint += pcsAttachTemplateData->m_stSkillCost[i].lCostCharismaPoint;
	}

	return lTotalCharismaPoint;
}

BOOL AgpmEventSkillMaster::IsFullUpgrade(AgpdSkill *pcsSkill)
{
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return TRUE;

	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1)
		return TRUE;

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

	return (pcsAttachTemplateData->lMaxUpgradeLevel <= lSkillLevel);
}

BOOL AgpmEventSkillMaster::IsUpgradable(AgpdSkill *pcsSkill)
{
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return FALSE;

	return IsUpgradable((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);
}

BOOL AgpmEventSkillMaster::IsUpgradable(AgpdSkillTemplate *pcsSkillTemplate)
{
	if (!pcsSkillTemplate)
		return FALSE;

	if (pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_CAN_UPGRADE)
		return TRUE;

	return FALSE;
}

BOOL AgpmEventSkillMaster::IsCurrentClassSkill( AgpdSkill* pcsSkill )
{
	if( !pcsSkill )
		return FALSE;

	AgpdCharacter			*pdCharacter	=	static_cast< AgpdCharacter* >( pcsSkill->m_pcsBase );
	AgpdEventSkillHighLevel	stEventSkill	=	GetHighLevelSkillInfo( pcsSkill->m_pcsTemplate->m_lID );

	if( m_pcsAgpmFactors->GetRace( &pdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		if( stEventSkill.m_lCharTID	!=	pdCharacter->m_lTID1 )
			return FALSE;
	}

	return TRUE;
}

// 2005.10.20. steeple
BOOL AgpmEventSkillMaster::IsLimitedMaxLevel(AgpdSkillTemplate* pcsSkillTemplate)
{
	if(!pcsSkillTemplate)
		return FALSE;

	if(pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_LIMITED_MAX_LEVEL)
		return TRUE;

	return FALSE;
}

INT32 AgpmEventSkillMaster::GetMasteryIndex(AgpdSkillTemplate *pcsSkillTemplate)
{
	if (!pcsSkillTemplate)
		return 0;

	if (m_pcsAgpmSkill->IsArchlordSkill(pcsSkillTemplate))
		return AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX;

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData(pcsSkillTemplate);

	return pcsAttachTemplateData->lMasteryIndex;
}

AgpdEventSkillAttachTemplateData* AgpmEventSkillMaster::GetSkillAttachTemplateData(AgpdSkillTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return NULL;

	return (AgpdEventSkillAttachTemplateData *) m_pcsAgpmSkill->GetAttachedModuleData(m_nIndexADSkillTemplate, (PVOID) pcsTemplate);
}

AgpdEventSkillAttachCharTemplateData* AgpmEventSkillMaster::GetSkillAttachCharTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate)
{
	if (!pcsCharacterTemplate)
		return NULL;

	return (AgpdEventSkillAttachCharTemplateData *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacterTemplate, (PVOID) pcsCharacterTemplate);
}

BOOL AgpmEventSkillMaster::CBCharTemplateConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmEventSkillMaster::CBCharTemplateDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventSkillMaster	*pThis					= (AgpmEventSkillMaster *)	pClass;
	AgpdCharacterTemplate	*pcsCharacterTemplate	= (AgpdCharacterTemplate *)	pData;

	AgpdEventSkillAttachCharTemplateData	*pcsCharTemplateAttachData	= pThis->GetSkillAttachCharTemplateData(pcsCharacterTemplate);
	if (!pcsCharTemplateAttachData)
		return FALSE;

	if (pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate)
	{
		delete [] pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate;

		pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate	= NULL;
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventSkillMaster	*pThis					= (AgpmEventSkillMaster *)	pClass;
	ApdEvent				*pcsDestEvent			= (ApdEvent *)				pData;

//	pcsDestEvent->m_pvData	= malloc(sizeof(AgpdSkillEventAttachData));
	pcsDestEvent->m_pvData	= (PVOID) new BYTE[sizeof(AgpdSkillEventAttachData)];
	if (!pcsDestEvent->m_pvData)
		return FALSE;

	ZeroMemory(pcsDestEvent->m_pvData, sizeof(AgpdSkillEventAttachData));

	((AgpdSkillEventAttachData *) pcsDestEvent->m_pvData)->eRaceType	= AURACE_TYPE_NONE;
	((AgpdSkillEventAttachData *) pcsDestEvent->m_pvData)->eClassType	= AUCHARCLASS_TYPE_NONE;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventSkillMaster	*pThis					= (AgpmEventSkillMaster *)	pClass;
	ApdEvent				*pcsDestEvent			= (ApdEvent *)				pData;

	if (pcsDestEvent->m_pvData)
	{
//		free(pcsDestEvent->m_pvData);
		delete [] (BYTE*)pcsDestEvent->m_pvData;
		pcsDestEvent->m_pvData	= NULL;
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBMakePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis					= (AgpmEventSkillMaster *)	pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)				pData;
	PVOID					*ppvPacketCustomData	= (PVOID *)					pCustData;

	*ppvPacketCustomData	= pThis->MakePacketEventData(pcsEvent);

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBParsePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis					= (AgpmEventSkillMaster *)	pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)				pData;
	PVOID					pvPacketCustomData		= (PVOID)					pCustData;

	return pThis->ParseEventDataPacket(pcsEvent, pvPacketCustomData);
}

BOOL AgpmEventSkillMaster::CBSetConstFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis					= (AgpmEventSkillMaster *)	pClass;
	AgpdSkillTemplate		*pcsSkillTemplate		= (AgpdSkillTemplate *)		pData;
	INT32					lMaxSkillLevel			= PtrToInt(pCustData);

	if (lMaxSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		lMaxSkillLevel	= AGPMSKILL_MAX_SKILL_CAP - 1;

	AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= pThis->GetSkillAttachTemplateData(pcsSkillTemplate);

	if (lMaxSkillLevel > AGPMSKILL_MAX_UPGRADE_SP)
		pcsAttachTemplateData->lMaxUpgradeLevel	= AGPMSKILL_MAX_UPGRADE_SP;
	else
		pcsAttachTemplateData->lMaxUpgradeLevel	= lMaxSkillLevel;

	// 2005.10.20. steeple
	if(pThis->IsLimitedMaxLevel(pcsSkillTemplate))
		pcsAttachTemplateData->lMaxUpgradeLevel = pcsSkillTemplate->m_lLimitedMaxLevel;

	// 여기서 스킬 구입 비용, 업그레이드 스킬 포인트, 업그레이드 비용, 스킬 배울때 스킬 포인트 등을 세팅해야 한다.
	// 현재는 명확한 값이 없다. 정리 필요함.

	pcsAttachTemplateData->m_stSkillCost[1].lCostMoney			= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_COST][1];
	pcsAttachTemplateData->m_stSkillCost[1].lCostSkillPoint		= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][1];
	pcsAttachTemplateData->m_stSkillCost[1].lCostHeroicPoint	= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_HEROIC_POINT][1];
	pcsAttachTemplateData->m_stSkillCost[1].lCostCharismaPoint	= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CHARISMA_POINT][1];

	for (int i = 2; i <= lMaxSkillLevel; ++i)
	{
		pcsAttachTemplateData->m_stSkillCost[i].lCostMoney			= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_UPGRADE_COST][i];
		pcsAttachTemplateData->m_stSkillCost[i].lCostSkillPoint		= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][i];
		pcsAttachTemplateData->m_stSkillCost[i].lCostHeroicPoint	= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_HEROIC_POINT][i];
		pcsAttachTemplateData->m_stSkillCost[i].lCostCharismaPoint	= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CHARISMA_POINT][i];
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBGetInputTotalCostSP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis				= (AgpmEventSkillMaster	*)	pClass;
	AgpdSkill				*pcsSkill			= (AgpdSkill *)				pData;
	INT32					*plInputSP			= (INT32 *)					pCustData;

	INT32					lInputSP			= pThis->GetInputTotalCostSP(pcsSkill);

	if (lInputSP >= 0)
		*plInputSP	= lInputSP;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBGetInputTotalCostHeroicPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis				= (AgpmEventSkillMaster *) pClass;
	AgpdSkill				*pcsSkill			= (AgpdSkill *)			   pData;
	INT32					*plInputHeroicPoint = (INT32 *)				   pCustData;

	INT32					lInputHeroicPoint   = pThis->GetInputTotalCostHeroicPoint(pcsSkill);

	if (lInputHeroicPoint >= 0)
		*plInputHeroicPoint = lInputHeroicPoint;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBRollbackSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSkillMaster	*pThis				= (AgpmEventSkillMaster	*)	pClass;
	AgpdSkill				*pcsSkill			= (AgpdSkill *)				pData;
	AgpdItem				*pcsRollbackScroll	= (AgpdItem *)				pCustData;

	if (!pThis->CheckTargetSkillForRollback(pcsSkill))
		return FALSE;

	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pcsSkill->m_pcsBase;
	
	INT32					lRefundCPoint		= pThis->GetInputTotalCharismaPoint(pcsSkill); 

	pThis->EnumCallback(AGPMEVENT_SKILL_CB_END_SKILL, pcsCharacter, pcsSkill);

	pThis->EnumCallback(AGPMEVENT_SKILL_CB_REMOVE_SKILL, pcsSkill, pcsCharacter);

	pThis->m_pcsAgpmSkill->RemoveOwnSkillList(pcsSkill);

	pThis->m_pcsAgpmSkill->RemoveSkill(pcsSkill->m_lID);

	pThis->m_pcsAgpmSkill->AdjustSkillPoint(pcsCharacter);

	pThis->m_pcsAgpmSkill->AdjustHeroicPoint(pcsCharacter);

	pThis->EnumCallback(AGPMEVENT_SKILL_CB_SAVE_ALL_DATA, pcsCharacter, NULL);

	pThis->m_pcsAgpmItem->SubItemStackCount(pcsRollbackScroll, 1);

	PVOID pvBuffer[2];	
	pvBuffer[0] = (PVOID)pcsCharacter;
	pvBuffer[1] = (PVOID)pcsRollbackScroll;

	if(pThis->m_pcsAgpmSkill->IsHeroicSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
	{
		// Log 남겨준다.
		// Heroic Skill 일때만 카리스마 포인트 때문에 남겨준다.
		pThis->EnumCallback(AGPMEVENT_SKILL_CB_SKILL_INIT_LOG, pcsSkill, pvBuffer);
	}

	///////////////////////////////////////////////////
	pThis->m_pcsAgpmCharacter->AddCharismaPoint(pcsCharacter, lRefundCPoint);

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBCheckJoinItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventSkillMaster	*pThis				= (AgpmEventSkillMaster	*)	pClass;
	PVOID					*ppvBuffer			= (PVOID *)					pData;

	AgpdItem	*pcsItem1	= (AgpdItem *)	ppvBuffer[0];
	AgpdItem	*pcsItem2	= (AgpdItem *)	ppvBuffer[1];
	BOOL		*pbCheckJoinItem	= (BOOL *)	ppvBuffer[2];

	if (!pcsItem1 || !pcsItem2 || !pbCheckJoinItem)
		return FALSE;

	if (((AgpdItemTemplate *) pcsItem1->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
		((AgpdItemTemplateUsable *) pcsItem1->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_BOOK)
	{
		if (pcsItem1->m_lSkillTID != 0 &&
			pcsItem1->m_lSkillTID != pcsItem2->m_lSkillTID)
			*pbCheckJoinItem	= FALSE;
	}

	return TRUE;
}

INT32 AgpmEventSkillMaster::GetTotalLearnCost(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	// 배우는데 들어간 돈을 다 뽑아낸다.
	INT32	lCostMoney	= 0;

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (!pcsSkill)
			break;

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
			continue;

		lCostMoney	+= GetBuyCost((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, pcsCharacter);

		INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
		if(lSkillLevel < 1 || lSkillLevel >= AGPMEVENT_SKILL_MAX_LEVEL) continue;

		for (int j = 1; j < lSkillLevel; ++j)
		{
			AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

			lCostMoney	+= pcsAttachTemplateData->m_stSkillCost[j + 1].lCostMoney;
		}
	}

	return lCostMoney;
}

INT32 AgpmEventSkillMaster::GetInitializeCost(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	// 비용 계산한다.
	INT32	lInputCostSP	= 0;

	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (!pcsSkill)
			continue;

		lInputCostSP	+= GetInputTotalCostSP(pcsSkill);
	}

	return (INT32) (lInputCostSP * 1000 * (m_pcsAgpmCharacter->GetLevel(pcsCharacter) * 0.2));
}

INT32 AgpmEventSkillMaster::GetTotalCharismaPointByLearnSkill(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0;

	// Calc Cost
	INT32 lTotalCharismaPoint = 0;

	AgpdSkillAttachData *pcsAttachData = m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill *pcsSkill = pcsAttachData->m_apcsSkill[i];
		if(NULL == pcsSkill)
			break;

		if(m_pcsAgpmSkill->IsHeroicSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) == FALSE)
			continue;

		lTotalCharismaPoint += GetInputTotalCharismaPoint(pcsSkill);
	}

	return lTotalCharismaPoint;
}

BOOL AgpmEventSkillMaster::InitializeSkillTree(AgpdCharacter *pcsCharacter, BOOL bAdmin, BOOL bCash)
{
	if (!pcsCharacter)
		return FALSE;

	// 하나도 배우지 않은 녀석이라면 이니셜을 해주지 않는다.
	if(m_pcsAgpmSkill->CheckHaveAnySkill(pcsCharacter) == FALSE)
		return FALSE;

	INT8 bResult = AGPMEVENT_SKILL_INITIALIZE_RESULT_SUCCESS;

	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);

	EnumCallback(AGPMEVENT_SKILL_CB_END_ALL_SKILL, pcsCharacter, NULL);

	INT64	llTotalLearnCost	= GetTotalLearnCost(pcsCharacter);

	INT32	lTotalCharismaPoint = GetTotalCharismaPointByLearnSkill(pcsCharacter);

	if( !m_pcsAgpmCharacter->CheckMoneySpace(pcsCharacter, llTotalLearnCost) )
	{
		bResult = AGPMEVENT_SKILL_INITIALIZE_RESULT_FAIL_MONEY_OVER;
		EnumCallback(AGPMEVENT_SKILL_CB_SKILL_INITIALIZE, pcsCharacter, &bResult);
		return FALSE;
	}

	AgpdEventSkillAttachCharTemplateData	*pcsCharTemplateAttachData	= GetSkillAttachCharTemplateData(pcsCharacter->m_pcsCharacterTemplate);

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (!pcsSkill)
			break;

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
			continue;

		if (pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate)
		{
			for (int j = 0; j < pcsCharTemplateAttachData->m_lNumDefaultSkill; ++j)
			{
				if (pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate[j] == (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)
					continue;
			}
		}

		if (GetBuyCost((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, pcsCharacter) == 0)
			continue;

		EnumCallback(AGPMEVENT_SKILL_CB_REMOVE_SKILL, pcsSkill, pcsCharacter);

		m_pcsAgpmSkill->RemoveSkill(pcsSkill->m_lID);

		pcsAttachData->m_alSkillID.MemCopy(i, &pcsAttachData->m_alSkillID[i + 1], AGPMSKILL_MAX_SKILL_OWN - i - 1);
		pcsAttachData->m_alSkillID[AGPMSKILL_MAX_SKILL_OWN - 1] = AP_INVALID_SKILLID;

		pcsAttachData->m_apcsSkill.MemCopy(i, &pcsAttachData->m_apcsSkill[i + 1], AGPMSKILL_MAX_SKILL_OWN - i - 1);
		pcsAttachData->m_apcsSkill[AGPMSKILL_MAX_SKILL_OWN - 1] = NULL;

		--i;
	}

	m_pcsAgpmSkill->AdjustSkillPoint(pcsCharacter);

	m_pcsAgpmSkill->AdjustHeroicPoint(pcsCharacter);

	// 어드민이 실행한 게 아닐 때만 아래 처리를 해준다.
	if(!bAdmin)
	{
		m_pcsAgpmCharacter->AddMoney(pcsCharacter, llTotalLearnCost);

		m_pcsAgpmCharacter->AddCharismaPoint(pcsCharacter, lTotalCharismaPoint);

		if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
			strcpy(pcsCharacter->m_szSkillInit, "initialized");
		else
			strcpy(pcsCharacter->m_szSkillInit, "치약바르자");
	}
	else if(bCash)
	{
		// 어드민이더라도 bCash 가 켜져있으면 돈은 돌려준다.
		m_pcsAgpmCharacter->AddMoney(pcsCharacter, llTotalLearnCost);

		m_pcsAgpmCharacter->AddCharismaPoint(pcsCharacter, lTotalCharismaPoint);
	}

	// update DB
	EnumCallback(AGPMEVENT_SKILL_CB_SAVE_ALL_DATA, pcsCharacter, NULL);

	// 
	EnumCallback(AGPMEVENT_SKILL_CB_SKILL_INITIALIZE, pcsCharacter, &bResult);

	return TRUE;
}

BOOL AgpmEventSkillMaster::CheckTargetSkillForRollback(AgpdSkill *pcsSkill)
{
	if (!pcsSkill)
		return FALSE;

	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		return FALSE;

	if (GetBuyCost((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, NULL) == 0 ||
		GetLearnCostSP((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate) == 0)
		return FALSE;

	// 고렙 스킬인 경우 2007.07.08. steeple
	if(m_pcsAgpmSkill->IsHighLevelSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
	{
		// 컨디션 비교를 해야 한다.
		if(!CheckHighLevelSkillForRollback(pcsSkill))
			return FALSE;
	}

	// In Case Heroic Skill
	if(m_pcsAgpmSkill->IsHeroicSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
	{
		if(GetLearnCostCharismaPoint((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) == 0 ||
		   GetLearnCostHeroicPoint((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) == 0)
		   return FALSE;

		// Compare Heroic Skill Condition
		if(!CheckHeroicSkillCondition(pcsSkill))
			return FALSE;
	}

	return TRUE;
}

// 2007.07.08. steeple
// 고레벨 스킬 롤백할 수 있는 지 체크한다.
BOOL AgpmEventSkillMaster::CheckHighLevelSkillForRollback(AgpdSkill* pcsSkill)
{
	if(!pcsSkill || !pcsSkill->m_pcsBase || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	AgpdEventSkillHighLevel stHighLevel = GetHighLevelSkillInfo(pcsSkillTemplate->m_lID);

	// 고렙 스킬이 아니다.
	if(stHighLevel.m_lSkillTID == 0)
		return FALSE;

	// 해당 Skill의 Condition Check
	if( !CheckSkillCondition( pcsCharacter , pcsSkillTemplate ) )
		return FALSE;
	
	return TRUE;
}

//BOOL AgpmEventSkillMaster::CheckHighLevelSkillForRollback(AgpdSkill* pcsSkill)
//{
//	if(!pcsSkill || !pcsSkill->m_pcsBase || !pcsSkill->m_pcsTemplate)
//		return FALSE;
//
//	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
//	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
//	AgpdEventSkillHighLevel stHighLevel = GetHighLevelSkillInfo(pcsSkillTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID);
//
//	// 고렙 스킬이 아니다.
//	if(stHighLevel.m_lSkillTID == 0)
//		return FALSE;
//
//	//// 연관 스킬이 없다면 return TRUE
//	//if(stHighLevel.m_stCondition[0].m_lSkillTID == 0)
//	//	return TRUE;
//
//	HighLevelSkillVector* pVector = GetHighLevelSkillVector(pcsCharacter->m_pcsCharacterTemplate->m_lID);
//	if(!pVector)
//		return FALSE;
//
//	// 벡터 전체를 다 돌면서 배운 스킬을 찾은 후에 롤백한 스킬과 연관이 있는 스킬이 있다면 return FALSE 한다.
//	HighLevelSkillIter iter = pVector->begin();
//	while(iter != pVector->end())
//	{
//		// 같은 건 넘어간다.
//		if(iter->m_lSkillTID == pcsSkillTemplate->m_lID)
//		{
//			++iter;
//			continue;
//		}
//
//		AgpdSkill* pcsOtherSkill = m_pcsAgpmSkill->GetSkillByTID(pcsCharacter, iter->m_lSkillTID);
//		if(pcsOtherSkill && pcsOtherSkill->m_pcsTemplate)
//		{
//			AgpdEventSkillHighLevel stOtherHighLevel = GetHighLevelSkillInfo(pcsOtherSkill->m_pcsTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID);
//			for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION; ++i)
//			{
//				if(stOtherHighLevel.m_stCondition[i].m_lSkillTID == 0)
//					break;
//
//				// 이미 배운 스킬의 Condition 중에서 롤백할 스킬과 같은 스킬이 있다면 롤백할 수 없다.
//				if(stOtherHighLevel.m_stCondition[i].m_lSkillTID == pcsSkillTemplate->m_lID)
//					return FALSE;
//			}
//		}
//
//		++iter;
//	}
//
//	return TRUE;
//}

HighLevelSkillVector* AgpmEventSkillMaster::GetHighLevelSkillVector(INT32 lCharTID)
{
	if(!lCharTID)
		return NULL;

	HighLevelSkillMapIter iter = m_mapHighLevelSkill.find(lCharTID);
	if(iter != m_mapHighLevelSkill.end())
		return &iter->second;
	else
		return NULL;
}

const AgpdEventSkillHighLevel AgpmEventSkillMaster::GetHighLevelSkillInfo(INT32 lSkillTID, INT32 lCharTID)
{
	AgpdEventSkillHighLevel stSkillHighLevel;
	memset(&stSkillHighLevel, 0, sizeof(stSkillHighLevel));

	if(!lSkillTID)
		return stSkillHighLevel;

	if(lCharTID)
	{
		HighLevelSkillVector* pVector = GetHighLevelSkillVector(lCharTID);
		if(pVector)
		{
			HighLevelSkillIter iter = pVector->begin();
			for( iter ; iter != pVector->end() ; ++iter )
			{
				if( (*iter).m_lSkillTID	==	lSkillTID )
					return *iter;
			}

			if(iter != pVector->end())
				return *iter;
		}
	}
	else
	{
		// 전체를 다 돈다.
		HighLevelSkillMapIter iterMap = m_mapHighLevelSkill.begin();
		while(iterMap != m_mapHighLevelSkill.end())
		{
			HighLevelSkillVector& csVector = iterMap->second;
			HighLevelSkillIter iter = std::find(csVector.begin(), csVector.end(), lSkillTID);
			if(iter != csVector.end())
				return *iter;

			++iterMap;
		}
	}

	// 얘는 데이터가 세팅 안된 더미 임. 여기서 리턴되면 못 찾은 것이다.
	return stSkillHighLevel;
}

const AgpdEventSkillHighLevel AgpmEventSkillMaster::GetHighLevelSkillInfoByIndex(INT32 lIndex, INT32 lCharTID)
{
	AgpdEventSkillHighLevel stSkillHighLevel;
	memset(&stSkillHighLevel, 0, sizeof(stSkillHighLevel));

	if(lIndex < 0 || !lCharTID)
		return stSkillHighLevel;

	HighLevelSkillVector* pVector = GetHighLevelSkillVector(lCharTID);
	if(pVector && lIndex < (INT32)pVector->size())
	{
		return (*pVector)[lIndex];
	}

	// 얘는 데이터가 세팅 안된 더미 임. 여기서 리턴되면 못 찾은 것이다.
	return stSkillHighLevel;
}

INT32 AgpmEventSkillMaster::GetHighLevelSkillIndex(INT32 lSkillTID, INT32 lCharTID)
{
	if(!lSkillTID)
		return -1;

	if(lCharTID)
	{
		HighLevelSkillVector* pVector = GetHighLevelSkillVector(lCharTID);
		if(pVector)
		{
			HighLevelSkillIter iter = std::find(pVector->begin(), pVector->end(), lSkillTID);
			if(iter != pVector->end())
				return (INT32)std::distance(pVector->begin(), iter);
		}
	}
	else
	{
		// 전체를 다 돈다.
		HighLevelSkillMapIter iterMap = m_mapHighLevelSkill.begin();
		while(iterMap != m_mapHighLevelSkill.end())
		{
			HighLevelSkillVector& csVector = iterMap->second;
			HighLevelSkillIter iter = std::find(csVector.begin(), csVector.end(), lSkillTID);
			if(iter != csVector.end())
				return (INT32)std::distance(csVector.begin(), iter);

			++iterMap;
		}
	}

	return -1;
}

BOOL AgpmEventSkillMaster::IsHighLevelSkill(INT32 lSkillTID, INT32 lCharTID)
{
	AgpdEventSkillHighLevel stSkillHighLevel = GetHighLevelSkillInfo(lSkillTID, lCharTID);
	return (BOOL)stSkillHighLevel.m_lSkillTID;
}

INT32 AgpmEventSkillMaster::GetCharacterRaceID(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	INT32 lRace = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor);

	switch(lRace)
	{
	case AURACE_TYPE_HUMAN:
		lRace = AGPM_EVENT_SKILL_MASTER_RACE_HUMAN;
		break;
	case AURACE_TYPE_ORC:
		lRace = AGPM_EVENT_SKILL_MASTER_RACE_ORC;
		break;
	case AURACE_TYPE_MOONELF:
		lRace = AGPM_EVENT_SKILL_MASTER_RACE_MOONELF;
		break;
	case AURACE_TYPE_DRAGONSCION:
		lRace = AGPM_EVENT_SKILL_MASTER_RACE_DRAGON_SCION;
		break;
	}

	return lRace;
}

BOOL AgpmEventSkillMaster::CheckSkillCondition( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{

	if( !_CheckConditionAB( pcsCharacter , pcsSkillTemplate  ) )
		return FALSE;

	if( !_CheckConditionC( pcsCharacter , pcsSkillTemplate ) )
		return FALSE;

	if( !_CheckConditionD( pcsCharacter , pcsSkillTemplate ) )
		return FALSE;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CheckSkillCondition( AgpdSkill* pcsSkill )
{
	if( !pcsSkill || !pcsSkill->m_pcsBase || !pcsSkill->m_pcsTemplate )
		return FALSE;

	return CheckSkillCondition( (AgpdCharacter*)pcsSkill->m_pcsBase , (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate );
}

BOOL AgpmEventSkillMaster::CheckHeroicSkillCondition( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{
	if( !_CheckHeroicConditionA( pcsCharacter , pcsSkillTemplate  ) )
		return FALSE;

	if( !_CheckHeroicConditionD( pcsCharacter , pcsSkillTemplate ) )
		return FALSE;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CheckHeroicSkillCondition( AgpdSkill* pcsSkill )
{
	if( !pcsSkill || !pcsSkill->m_pcsBase || !pcsSkill->m_pcsTemplate )
		return FALSE;

	return CheckHeroicSkillCondition( (AgpdCharacter*)pcsSkill->m_pcsBase , (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate );
}


BOOL AgpmEventSkillMaster::_CheckConditionAB( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{
	AgpdEventSkillHighLevel		stHighLevel		=	GetHighLevelSkillInfo( pcsSkillTemplate->m_lID );

	HighLevelSkillVector* pVector = GetHighLevelSkillVector( stHighLevel.m_lCharTID );
	// 현재 Skill을 지울수 있는지 확인한다
	HighLevelSkillIter iter = pVector->begin();
	while(iter != pVector->end())
	{
		// 같은 건 넘어간다.
		if(iter->m_lSkillTID == pcsSkillTemplate->m_lID)
		{
			++iter;
			continue;
		}

		AgpdSkill*	pdSkill	=	GetSkill( pcsCharacter , iter->m_lSkillTID );
		if( !pdSkill )
		{	++iter;
			continue;
		}

		for( INT k = 0 ; k < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION ; ++k )
		{
			if( iter->m_stCondition[ k ].m_lSkillLevel )
			{
				if( iter->m_stCondition[ k ].m_lSkillTID == pcsSkillTemplate->m_lID )
					return FALSE;
			}
		}
				
		++iter;
	}


	return TRUE;
}

BOOL AgpmEventSkillMaster::_CheckConditionC( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{

	INT32	arrSkillTab[]	=	{	1724		,
									1723		,
									1722		,
									1732		,
									10004			};

	AgpdEventSkillHighLevel stHighLevel			=	GetHighLevelSkillInfo( pcsSkillTemplate->m_lID );
	AgpdCharacterTemplate*	pCharacterTemplate	=	m_pcsAgpmCharacter->GetCharacterTemplate( stHighLevel.m_lCharTID );
	if( !pCharacterTemplate )
		pCharacterTemplate	=	m_pcsAgpmCharacter->GetCharacterTemplate( arrSkillTab[0] );

	AuCharClassType			eType				=	( AuCharClassType )m_pcsAgpmFactors->GetClass( &pCharacterTemplate->m_csFactor );
	AgpdSkill*				pMinusSkill			=	m_pcsAgpmSkill->GetSkill( pcsCharacter , pcsSkillTemplate->m_szName );
	INT32					nTotalSkillPoint	=	m_pcsAgpmSkill->GetTotalSkillPoint( pcsCharacter , AURACE_TYPE_DRAGONSCION , eType , FALSE );
	INT32					nMinusPoint			=	m_pcsAgpmSkill->GetSkillLevel( pMinusSkill );
	INT32					nResultPoint		=	nTotalSkillPoint;

	

	// 시온 스킬창에도 다른종족 스킬이 있다
	// 다른종족 스킬을 시온창에서 지울때 Minus가 되지 않는다
	if( m_pcsAgpmSkill->IsWantedRaceClassSkill( pMinusSkill , AURACE_TYPE_DRAGONSCION , eType ) &&
		!m_pcsAgpmSkill->IsEvolutionSkill( (AgpdSkillTemplate*)pMinusSkill->m_pcsTemplate) )
		nResultPoint	-=	nMinusPoint;

	for( INT i = 0 ; i < sizeof(arrSkillTab)/sizeof(INT32) ; ++i )
	{
		HighLevelSkillVector*		pVector				=	GetHighLevelSkillVector( arrSkillTab[i] );
		if( !pVector )
			return TRUE;

		HighLevelSkillIter			Iter		=	pVector->begin();
		for( ; Iter != pVector->end() ; ++Iter )
		{			
			AgpdSkill*			pdSkill			=	GetSkill( pcsCharacter , Iter->m_lSkillTID );
			INT32				nCurrentPoint	=	0;

			if( !pdSkill )
				continue;

			if( (pdSkill != pMinusSkill) && m_pcsAgpmSkill->IsWantedRaceClassSkill( pdSkill , AURACE_TYPE_DRAGONSCION , eType ) )
			{
				if( !m_pcsAgpmSkill->IsEvolutionSkill( (AgpdSkillTemplate*)pdSkill->m_pcsTemplate ) )
					nCurrentPoint	=	m_pcsAgpmSkill->GetSkillLevel( pdSkill );
			}


			for( INT k = 0 ; k < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION ; ++k )
			{
				if( Iter->m_stCondition[ k ].m_lSkillTab	==	stHighLevel.m_lCharTID )
				{
					if( Iter->m_stCondition[ k ].m_lSkillPoint	> (nResultPoint-nCurrentPoint) )
					{						
						return FALSE;
					}
				}
			}

		}
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::_CheckConditionD( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{

	INT32	arrSkillTab[]	=	{	1724		,
									1723		,
									1722		,
									1732		,
									10004			};

	AgpdEventSkillHighLevel stHighLevel			=	GetHighLevelSkillInfo( pcsSkillTemplate->m_lID );
	INT32					nTotalSkillPoint	=	m_pcsAgpmSkill->GetTotalSkillPoint( pcsCharacter  );
	AgpdSkill*				pMinusSkill			=	m_pcsAgpmSkill->GetSkill( pcsCharacter , pcsSkillTemplate->m_szName );
	INT32					nMinusPoint			=	m_pcsAgpmSkill->GetSkillLevel( pMinusSkill );
	INT32					nResultPoint		=	nTotalSkillPoint	-	nMinusPoint;
	

	for( INT i = 0 ; i < sizeof(arrSkillTab)/sizeof(INT32) ; ++i )
	{
		HighLevelSkillVector*		pVector				=	GetHighLevelSkillVector( arrSkillTab[i] );
		if( !pVector )
			return TRUE;

		HighLevelSkillIter			Iter		=	pVector->begin();
		for( ; Iter != pVector->end() ; ++Iter )
		{

			AgpdSkill*			pdSkill			=	GetSkill( pcsCharacter , Iter->m_lSkillTID );
			INT32				nCurrentPoint		=	0;

			if( !pdSkill )
				continue;

			if( pdSkill != pMinusSkill )
				nCurrentPoint	=	m_pcsAgpmSkill->GetSkillLevel( pdSkill );

			for( INT k = 0 ; k < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION ; ++k )
			{
				if( Iter->m_stCondition[ k ].m_lSkillTotalPoint > (nResultPoint-nCurrentPoint) )
				{
					return FALSE;				
				}
			}
		}
	}

	return TRUE;

}

HeroicSkillVector* AgpmEventSkillMaster::GetHeroicSkillVector(INT32 lCharacterTID)
{
	if(!lCharacterTID)
		return NULL;

	HeroicSkillMapIter iter = m_mapHeroicSkill.find(lCharacterTID);
	if(iter != m_mapHeroicSkill.end())
		return &iter->second;
	else
		return NULL;
}

const AgpdEventSkillHeroic AgpmEventSkillMaster::GetHeroicSkillInfo(INT32 lSkillTID, INT32 lCharTID /* = 0 */)
{
	AgpdEventSkillHeroic stSkillHeroic;
	ZeroMemory(&stSkillHeroic, sizeof(stSkillHeroic));

	if(!lSkillTID)
		return stSkillHeroic;

	if(lCharTID)
	{
		HeroicSkillVector *pVector = GetHeroicSkillVector(lCharTID);
		if(pVector)
		{
			HeroicSkillIter iter = pVector->begin();
			for(iter ; iter != pVector->end(); ++iter)
			{
				if((*iter).m_lSkillTID == lSkillTID)
					return *iter;
			}

			if(iter != pVector->end())
				return *iter;
		}
	}
	else
	{
		HeroicSkillMapIter iterMap = m_mapHeroicSkill.begin();
		while(iterMap != m_mapHeroicSkill.end())
		{
			HeroicSkillVector& csVector = iterMap->second;
			HeroicSkillIter iter = std::find(csVector.begin(), csVector.end(), lSkillTID);
			if(iter != csVector.end())
				return *iter;

			++iterMap;
		}
	}

	return stSkillHeroic;
}

const AgpdEventSkillHeroic AgpmEventSkillMaster::GetHeroicSkillInfoByIndex(INT32 lIndex, INT32 lCharTID)
{
	AgpdEventSkillHeroic stSkillHeroic;
	ZeroMemory(&stSkillHeroic, sizeof(stSkillHeroic));

	if(lIndex < 0 || !lCharTID)
		return stSkillHeroic;

	HeroicSkillVector* pVector = GetHeroicSkillVector(lCharTID);
	if(pVector && lIndex < (INT32)pVector->size())
	{
		return (*pVector)[lIndex];
	}

	return stSkillHeroic;
}

INT32 AgpmEventSkillMaster::GetHeroicSkillIndex(INT32 lSkillTID, INT32 lCharTID /* = 0 */)
{
	if(!lSkillTID)
		return -1;

	if(lCharTID)
	{
		HeroicSkillVector* pVector = GetHeroicSkillVector(lCharTID);
		if(pVector)
		{
			HeroicSkillIter iter = std::find(pVector->begin(), pVector->end(), lSkillTID);
			if(iter != pVector->end())
				return (INT32)std::distance(pVector->begin(), iter);
		}
	}
	else
	{
		HeroicSkillMapIter iterMap = m_mapHeroicSkill.begin();
		while(iterMap != m_mapHeroicSkill.end())
		{
			HeroicSkillVector& csVector = iterMap->second;
			HeroicSkillIter iter = std::find(csVector.begin(), csVector.end(), lSkillTID);
			if(iter != csVector.end())
				return (INT32)std::distance(csVector.begin(), iter);

			++iterMap;
		}
	}

	return -1;
}

BOOL AgpmEventSkillMaster::IsHeroicSkill(INT32 lSkillTID, INT32 lCharTID /* = 0 */)
{
	AgpdEventSkillHeroic stSkillHeroic = GetHeroicSkillInfo(lSkillTID, lCharTID);
	return (BOOL)stSkillHeroic.m_lSkillTID;
}

BOOL AgpmEventSkillMaster::_CheckHeroicConditionA( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{

	AgpdEventSkillHeroic		stHeroic		=	GetHeroicSkillInfo( pcsSkillTemplate->m_lID );

	HeroicSkillVector* pVector = GetHeroicSkillVector( stHeroic.m_lCharTID );
	// 현재 Skill을 지울수 있는지 확인한다
	HeroicSkillIter iter = pVector->begin();
	while(iter != pVector->end())
	{
		// 같은 건 넘어간다.
		if(iter->m_lSkillTID == pcsSkillTemplate->m_lID)
		{
			++iter;
			continue;
		}

		AgpdSkill*	pdSkill	=	GetSkill( pcsCharacter , iter->m_lSkillTID );
		if( !pdSkill )
		{	
			++iter;
			continue;
		}

		for( INT k = 0 ; k < AGPMEVENT_SKILL_MAX_HEROIC_CONDITION ; ++k )
		{
			if( iter->m_stCondition[ k ].m_lSkillLevel )
			{
				if( iter->m_stCondition[ k ].m_lSkillTID == pcsSkillTemplate->m_lID )
					return FALSE;
			}
		}

		++iter;
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::_CheckHeroicConditionD( AgpdCharacter* pcsCharacter , AgpdSkillTemplate* pcsSkillTemplate )
{

	AgpdEventSkillHeroic	stHeroic			=	GetHeroicSkillInfo( pcsSkillTemplate->m_lID );
	INT32					nTotalSkillPoint	=	m_pcsAgpmSkill->GetTotalUsedHeroicPoint( (AgpdCharacter*)pcsCharacter );
	AgpdSkill*				pMinusSkill			=	m_pcsAgpmSkill->GetSkill( pcsCharacter , pcsSkillTemplate->m_szName );
	INT32					nMinusPoint			=	GetInputTotalCostHeroicPoint( pMinusSkill );
	INT32					nResultPoint		=	nTotalSkillPoint	-	nMinusPoint;
	HeroicSkillVector*		pVector				=	GetHeroicSkillVector( pcsCharacter->m_pcsCharacterTemplate->m_lID );

	HeroicSkillIter			Iter				=	pVector->begin();
	for( ; Iter != pVector->end() ; ++Iter )
	{
		INT32				nCurrentHeroicPoint	=	0;
		AgpdSkill*			pdSkill				=	GetSkill( pcsCharacter , Iter->m_lSkillTID );
		if( !pdSkill )
			continue;

		if( pdSkill != pMinusSkill )
			nCurrentHeroicPoint						=	GetInputTotalCostHeroicPoint( pdSkill );

		for( INT k = 0 ; k < AGPMEVENT_SKILL_MAX_HEROIC_CONDITION ; ++k )
		{
			if( Iter->m_stCondition[ k ].m_lUsedTotalHeroicPoint > (nResultPoint-nCurrentHeroicPoint) )
				return FALSE;
		}
	}
	
	return TRUE;

}

BOOL AgpmEventSkillMaster::CheckHeroicSkillForClass(AgpdCharacter *pcsCharacter, AgpdSkillTemplate* pcsSkillTemplate)
{
	if(NULL == pcsCharacter || NULL == pcsSkillTemplate)
		return FALSE;

	// Compare Current Character Template with skill Character TID
	// For Scion
	AgpdEventSkillHeroic stHeroicSkill = GetHeroicSkillInfo(pcsSkillTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID);

	if(stHeroicSkill.m_lCharTID == pcsCharacter->m_pcsCharacterTemplate->m_lID)
		return TRUE;

	return FALSE;
}
