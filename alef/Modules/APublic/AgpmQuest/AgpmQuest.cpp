// AgpmQuest.cpp: implementation of the AgpmQuest class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "AgpmQuest.h"
#include "ApMemoryTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpmQuest::AgpmQuest()
{
	m_pcsAgpmCharacter	= NULL;
	m_pcsAgpmFactors	= NULL;
	m_pcsAgpmItem		= NULL;
	m_pcsAgpmSkill		= NULL;
	m_pcsAgpmGrid		= NULL;
	m_pcsApmEventManager = NULL;

	SetModuleName("AgpmQuest");
	SetModuleData( sizeof( AgpdQuestTemplate ), AGPDQUEST_DATA_TYPE_TEMPLATE );
	SetModuleData( sizeof( AgpdQuestGroup ), AGPDQUEST_DATA_TYPE_QUEST_GROUP );
	SetPacketType(AGPMQUEST_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
// AGSDQUEST_EXPAND_BLOCK
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation
							AUTYPE_INT32,			1, // Character ID
							AUTYPE_PACKET,			1, // 퀘스트를 발생시킨 ApdEvent 패킷
							AUTYPE_PACKET,			1, // 패킷 상세 데이터
							AUTYPE_INT32,			1, // Quest ID
							AUTYPE_INT32,			1, // Check Point Index
							AUTYPE_INT32,			1, // Result
							AUTYPE_MEMORY_BLOCK,	1, // Flag Memory1
							AUTYPE_MEMORY_BLOCK,	1, // Flag Memory2
							AUTYPE_MEMORY_BLOCK,	1, // Flag Memory3 
		                    AUTYPE_END,				0
							);

	m_csPacketQuest.SetFlagLength(sizeof(INT8));
	m_csPacketQuest.SetFieldType(
								AUTYPE_INT32,		1,	// Quest ID 
								AUTYPE_INT32,		1,	// Param 1
								AUTYPE_INT32,		1,	// Param 2
								AUTYPE_INT32,		1,	// Item TID
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, 
								AUTYPE_END,			0
								);
}

AgpmQuest::~AgpmQuest()
{

}

BOOL AgpmQuest::OnAddModule()
{
	// get character
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmFactors	= (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgpmItem		= (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgpmSkill		= (AgpmSkill*)GetModule("AgpmSkill");
	m_pcsApmEventManager = (ApmEventManager*)GetModule("ApmEventManager");
	m_pcsAgpmGrid		= (AgpmGrid*)GetModule("AgpmGrid");

	if (!m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmSkill || 
		!m_pcsApmEventManager || !m_pcsAgpmGrid) return FALSE;

	m_lIndexAttachData = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdQuest), AgpdQuestConstructor, AgpdQuestDestructor);
	if (m_lIndexAttachData < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmQuest::AgpdQuestConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData))
		return FALSE;

	AgpmQuest *pThis = (AgpmQuest *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgpdQuest *pcsAgpdQuest = (AgpdQuest *)pThis->GetAttachAgpdQuestData(pstAgpdCharacter);

#ifdef new
#undef new
#endif
	new(pcsAgpdQuest) AgpdQuest;
#ifdef new
#undef new	
#define new DEBUG_NEW
#endif	
	return TRUE;
}

BOOL AgpmQuest::AgpdQuestDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData))
		return FALSE;

	AgpmQuest *pThis = (AgpmQuest *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgpdQuest *pcsAgpdQuest = (AgpdQuest *)pThis->GetAttachAgpdQuestData(pstAgpdCharacter);

	pcsAgpdQuest->~AgpdQuest();

	return TRUE;
}

BOOL AgpmQuest::OnInit()
{
	// QuestTemplate Admin 초기화
	if (!m_csQuestTemplate.InitializeObject( sizeof(AgpdQuestTemplate *), m_csQuestTemplate.GetCount()))
		return FALSE;

	if (!m_csQuestGroup.InitializeObject( sizeof(AgpdQuestGroup*), m_csQuestGroup.GetCount()))
		return FALSE;

	return TRUE;
}

BOOL AgpmQuest::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmQuest::OnDestroy()
{
	INT32 lIndex = 0;
	AgpdQuestTemplate	*pcsAgpdQuestTemplate;
	
	// 등록된 모든 Quest Template에 대해서...
	for (pcsAgpdQuestTemplate = GetTemplateSequence(&lIndex); pcsAgpdQuestTemplate; pcsAgpdQuestTemplate = GetTemplateSequence(&lIndex))
	{
		DestroyTemplate(pcsAgpdQuestTemplate);
	}

	m_csQuestTemplate.RemoveObjectAll();
	
	lIndex = 0;
	AgpdQuestGroup	*pcsAgpdQuestGroup;
	for (pcsAgpdQuestGroup = GetGroupSequence(&lIndex); pcsAgpdQuestGroup; pcsAgpdQuestGroup = GetGroupSequence(&lIndex))
	{
		DestroyGroup(pcsAgpdQuestGroup);
	}

	return TRUE;
}

BOOL AgpmQuest::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	INT8	cOperation			= -1;
	INT32   lCID				= -1;
	INT32	lQuestTID			= -1;
	INT32	lCheckPointIndex	= -1;
	INT32	lResult				= -1;
	UINT16	nBufferLength1		= 0;
	BYTE*	pBuffer1			= NULL;
	UINT16  nBufferLength2		= 0;
	BYTE*	pBuffer2			= NULL;
	UINT16  nBufferLength3		= 0;
	BYTE*	pBuffer3			= NULL;
	PVOID	pvPacketEventBase	= NULL;
	PVOID	pvPacketQuest		= NULL;
	ApdEvent* pcsEvent			= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID, 
						&pvPacketEventBase,
						&pvPacketQuest,
						&lQuestTID,
						&lCheckPointIndex,
						&lResult,
						&pBuffer1,
						&nBufferLength1,
						&pBuffer2,
						&nBufferLength2,
						&pBuffer3,
						&nBufferLength3);

	if (cOperation >= AGPMQUEST_PACKET_OPERATION_MAX) 
		return FALSE;

	if (pvPacketEventBase)
	{
		pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
		if(!pcsEvent)
			return FALSE;
	}

	AgpdCharacter *pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		pcsAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	CBQuestParam Param;
	Param.pcsAgpdCharacter	= pcsAgpdCharacter;
	Param.pcsEvent			= pcsEvent;
	Param.lQuestTID			= lQuestTID;
	Param.lCheckPointIndex	= lCheckPointIndex;
	Param.bResult			= (BOOL)lResult;
	Param.pvPacket			= pvPacketQuest;
	Param.pBuffer1			= pBuffer1;
	Param.nBufferLength1	= nBufferLength1;
	Param.pBuffer2			= pBuffer2;
	Param.nBufferLength2	= nBufferLength2;
	Param.pBuffer3			= pBuffer3;
	Param.nBufferLength3	= nBufferLength3;

	switch (cOperation)
	{
	case AGPMQUEST_PACKET_OPERATION_INIT_QUEST:
		{
			INT32 lQuestID, lParam1, lParam2, lItemTID;
			CHAR* szNPCName;

			GetFieldEmbeddedPacket(pvPacketQuest, lQuestID, lParam1, lParam2, lItemTID, szNPCName);

			AgpdQuest *pAgpdQuest = GetAttachAgpdQuestData(pcsAgpdCharacter);
			if (!pAgpdQuest) break;

			pAgpdQuest->AddNewQuest(lQuestID, lParam1, lParam2);
		}
		break;

	case AGPMQUEST_PACKET_OPERATION_INIT_FLAGS:
		{
			// Flag 버퍼 설정
			AgpdQuest *pAgpdQuest = GetAttachAgpdQuestData(pcsAgpdCharacter);
			if (!pAgpdQuest) break;

			if (pBuffer1)
				pAgpdQuest->m_csFlag.MemoryCopy(pBuffer1, 0);
			if (pBuffer2)
				pAgpdQuest->m_csFlag.MemoryCopy(pBuffer2, 1);
			if (pBuffer3)
				pAgpdQuest->m_csFlag.MemoryCopy(pBuffer3, 2);
		}
		break;

	default:
		EnumCallback(cOperation, &Param, NULL);
		break;
	};

	pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmQuest::SetCallbackRequireQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_REQUIRE_QUEST, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackRequireQuestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_REQUIRE_QUEST_RESULT, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackCompleteQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_QUEST_COMPLETE, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackCompleteQuestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_QUEST_COMPLETE_RESULT, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackCancelQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_CANCEL, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackCancelQuestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_CANCEL_RESULT, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackInventoryFull(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_INVENTORY_FULL, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackQuestInventoryFull(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_QUEST_INVENTORY_FULL, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackUpdateQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_UPDATE_QUEST, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackCheckPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_CHECKPOINT, pfCallback, pClass);
}

BOOL AgpmQuest::SetCallbackCheckPointResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMQUEST_PACKET_OPERATION_CHECKPOINT_RESULT, pfCallback, pClass);
}

AgpdQuest* AgpmQuest::GetAttachAgpdQuestData(AgpdCharacter *pcsCharacter)
{
	if(m_pcsAgpmCharacter)
		return (AgpdQuest*)m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexAttachData, (PVOID)pcsCharacter);

	return NULL;
	
}

BOOL AgpmQuest::SetMaxTemplateCount(INT32 lCount)
{
	return m_csQuestTemplate.SetCount(lCount);
}

BOOL AgpmQuest::SetMaxGroupCount(INT32 lCount)
{
	return m_csQuestGroup.SetCount(lCount);
}

AgpdQuestTemplate* AgpmQuest::GetTemplateSequence(INT32 *plIndex)
{
	AgpdQuestTemplate **pcsTemplate = (AgpdQuestTemplate **) m_csQuestTemplate.GetObjectSequence(plIndex);

	if (!pcsTemplate)
		return NULL;

	return  *pcsTemplate;
}

AgpdQuestTemplate* AgpmQuest::AddTemplate(INT32 lTID)
{
	AgpdQuestTemplate *pcsTemplate;

	pcsTemplate = CreateTemplate();
	if (!pcsTemplate)
		return NULL;

	pcsTemplate->m_lID = lTID;
	if (!m_csQuestTemplate.Add(pcsTemplate))
	{
		DestroyTemplate(pcsTemplate);
		return NULL;
	}

	return pcsTemplate;
}

BOOL AgpmQuest::DeleteTemlate(INT32 lTID)
{
	AgpdQuestTemplate *pcsTemplate = m_csQuestTemplate.Get(lTID); 
	if (pcsTemplate)
	{
		DestroyTemplate(pcsTemplate);
		m_csQuestTemplate.Remove(lTID);
		return TRUE;
	}

	return FALSE;
}

AgpdQuestTemplate* AgpmQuest::CreateTemplate()
{
	AgpdQuestTemplate *pcsTemplate = (AgpdQuestTemplate *) CreateModuleData(AGPDQUEST_DATA_TYPE_TEMPLATE);

	if (pcsTemplate)
	{
		pcsTemplate->m_Mutex.Init((PVOID) pcsTemplate);
		pcsTemplate->m_eType = APBASE_TYPE_QUEST_TEMPLATE;
		
		ZeroMemory(&pcsTemplate->m_QuestInfo.szName , AGPDQUEST_MAX_NAME + 1);
	}

	return pcsTemplate;
}

BOOL AgpmQuest::DestroyTemplate(AgpdQuestTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsTemplate, AGPDQUEST_DATA_TYPE_TEMPLATE);
}

AgpdQuestGroup* AgpmQuest::AddGroup(INT32 lTID)
{
	AgpdQuestGroup *pcsGroup;

	pcsGroup = CreateGroup();
	if (!pcsGroup)
		return NULL;

	pcsGroup->m_lID = lTID;
	if (!m_csQuestGroup.Add(pcsGroup))
	{
		DestroyGroup(pcsGroup);
		return NULL;
	}

	return pcsGroup;	
}

BOOL AgpmQuest::DeleteGroup(INT32 lTID)
{
	AgpdQuestGroup *pcsGroup = m_csQuestGroup.Get(lTID); 
	if (!pcsGroup)
		return FALSE;

	DestroyGroup(pcsGroup);
	return TRUE;	
}

AgpdQuestGroup* AgpmQuest::GetGroupSequence(INT32 *plIndex)
{
	AgpdQuestGroup **pcsGroup = (AgpdQuestGroup **) m_csQuestGroup.GetObjectSequence(plIndex);

	if (!pcsGroup)
		return NULL;

	return  *pcsGroup;
}

AgpdQuestGroup* AgpmQuest::CreateGroup()
{
	AgpdQuestGroup *pcsGroup = new AgpdQuestGroup;//(AgpdQuestGroup *) CreateModuleData(AGPDQUEST_DATA_TYPE_QUEST_GROUP);

	if (pcsGroup)
	{
//		pcsGroup->m_Mutex.Init((PVOID) pcsGroup);
		
		ZeroMemory(&pcsGroup->m_szName, AGPDCHARACTER_MAX_ID_LENGTH + 1);
	}

	return pcsGroup;
}

BOOL AgpmQuest::DestroyGroup(AgpdQuestGroup* pcsGroup)
{
	if (!pcsGroup)
		return FALSE;

	// 주의 : new로 생성하기 때문에 delete로 삭제한다. -kermi-
//	pcsGroup->m_Mutex.Destroy();
//	return DestroyModuleData((PVOID) pcsGroup, AGPDQUEST_DATA_TYPE_QUEST_GROUP);
	m_csQuestGroup.Remove(pcsGroup->m_lID);
	delete pcsGroup;

	return TRUE;
}

BOOL AgpmQuest::IsHaveQuest(AgpdCharacter *pcsCharacter, INT32 lQuestTID)
{
	if (!pcsCharacter || !lQuestTID) return FALSE;

	AgpdQuest *pcsAgpdQuest = GetAttachAgpdQuestData(pcsCharacter);
	if (!pcsAgpdQuest) return FALSE;

	return pcsAgpdQuest->IsHaveQuest(lQuestTID);
}

BOOL AgpmQuest::EvaluationStartCondition(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID)
{
	// 같은 퀘스트를 2개 부여 받을수 없다.
//	if (IsHaveQuest(pcsAgpdCharacter, lQuestTID)) return FALSE;

	AgpdQuestTemplate *pcsAgpdQuestTemplate = m_csQuestTemplate.Get(lQuestTID);
	if (!pcsAgpdQuestTemplate) return FALSE;

	// 종족 검사
	if (0 != pcsAgpdQuestTemplate->m_StartCondition.lRace)
	{
		if (pcsAgpdQuestTemplate->m_StartCondition.lRace !=
			m_pcsAgpmFactors->GetRace(&pcsAgpdCharacter->m_csFactor))
			return FALSE;
	}

	// 성별
	if (0 != pcsAgpdQuestTemplate->m_StartCondition.lGender)
	{
		if (pcsAgpdQuestTemplate->m_StartCondition.lGender !=
			m_pcsAgpmFactors->GetGender(&pcsAgpdCharacter->m_csFactor))
			return FALSE;
	}

	// 클래스 검사
	if (0 != pcsAgpdQuestTemplate->m_StartCondition.lClass)
	{
		if (pcsAgpdQuestTemplate->m_StartCondition.lClass	!= 
			m_pcsAgpmFactors->GetClass(&pcsAgpdCharacter->m_csFactor))
			return FALSE;
	}

	// 레벨
	if (0 != pcsAgpdQuestTemplate->m_StartCondition.lLevel)
	{
		if (pcsAgpdQuestTemplate->m_StartCondition.lLevel >
			m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter))
			return FALSE;
	}

	// 수행할수 있는 상한 레벨 제한	문엘프퀘스트 관련 수정추가 2005.6.2	AGSDQUEST_EXPAND_BLOCK
	if (0 != pcsAgpdQuestTemplate->m_StartCondition.lMaxLevel)
	{
		if (pcsAgpdQuestTemplate->m_StartCondition.lMaxLevel <
			m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter))
			return FALSE;
	}

	// 이전 퀘스트
	if (NULL != pcsAgpdQuestTemplate->m_StartCondition.Quest.pQuestTemplate)
	{
		AgpdQuest* pcsAgpdQuest = GetAttachAgpdQuestData(pcsAgpdCharacter);
		if (!pcsAgpdQuest) 
			return FALSE;

		// 이전 퀘스트를 수행하지 않았으면 return FALSE
		if (FALSE == pcsAgpdQuest->m_csFlag.GetValueByDBID(pcsAgpdQuestTemplate->m_StartCondition.Quest.pQuestTemplate->m_lID))
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmQuest::EvaluationCompleteCondition(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID)
{
	// 부여받은 퀘스트가 아니면 종료
//	if (!IsHaveQuest(pcsAgpdCharacter, lQuestTID)) return FALSE;

	AgpdQuestTemplate *pcsAgpdQuestTemplate = m_csQuestTemplate.Get(lQuestTID);
	if (!pcsAgpdQuestTemplate) return FALSE;

	// 레벨
	if (0 != pcsAgpdQuestTemplate->m_CompleteCondition.lLevel)
	{
		if (pcsAgpdQuestTemplate->m_CompleteCondition.lLevel > m_pcsAgpmCharacter->GetLevel(pcsAgpdCharacter))
			return FALSE;
	}

	// 돈
	if (0 != pcsAgpdQuestTemplate->m_CompleteCondition.lMoney)
	{
		if (pcsAgpdQuestTemplate->m_CompleteCondition.lMoney > pcsAgpdCharacter->m_llMoney)
			return FALSE;
	}

	// Monster
	if (NULL != pcsAgpdQuestTemplate->m_CompleteCondition.Monster1.pMonsterTemplate)
	{
		AgpdQuest *pcsAgpdQuest = GetAttachAgpdQuestData(pcsAgpdCharacter);
		if (pcsAgpdQuestTemplate->m_CompleteCondition.Monster1.lCount > pcsAgpdQuest->GetMonsterCount1(lQuestTID))
			return FALSE;
	}

	if (NULL != pcsAgpdQuestTemplate->m_CompleteCondition.Monster2.pMonsterTemplate)
	{
		AgpdQuest *pcsAgpdQuest = GetAttachAgpdQuestData(pcsAgpdCharacter);
		if (pcsAgpdQuestTemplate->m_CompleteCondition.Monster2.lCount > pcsAgpdQuest->GetMonsterCount2(lQuestTID))
			return FALSE;
	}

	// Item 조건 검사
	if (m_pcsAgpmItem && pcsAgpdQuestTemplate->m_CompleteCondition.Item.pItemTemplate)
	{
		AgpdGrid *pcsAgpdGrid = m_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
		if (!pcsAgpdGrid) return FALSE;

		INT32 lCount = m_pcsAgpmGrid->GetCountByTemplate(pcsAgpdGrid, AGPDGRID_ITEM_TYPE_ITEM,
												pcsAgpdQuestTemplate->m_CompleteCondition.Item.pItemTemplate->m_lID);

		if (pcsAgpdQuestTemplate->m_CompleteCondition.Item.lCount > lCount)
			return FALSE;
	}

	// CheckPoint 검사
	for (INT32 i = 0; i < AGPDQUEST_MAX_CHECKPOINT; ++i)
	{
		if (m_pcsAgpmItem && pcsAgpdQuestTemplate->m_CheckPoint.CheckPointItem[i].pItemTemplate)
		{
			AgpdGrid *pcsAgpdGrid = m_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
			if (!pcsAgpdGrid) return FALSE;

			INT32 lCount = m_pcsAgpmGrid->GetCountByTemplate(pcsAgpdGrid, AGPDGRID_ITEM_TYPE_ITEM,
												pcsAgpdQuestTemplate->m_CheckPoint.CheckPointItem[i].pItemTemplate->m_lID);

			if (pcsAgpdQuestTemplate->m_CheckPoint.CheckPointItem[i].lItemCount > lCount)
				return FALSE;
		}
	}

	return TRUE;
}

