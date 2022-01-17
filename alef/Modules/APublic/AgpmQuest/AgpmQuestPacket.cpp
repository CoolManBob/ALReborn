#include "AgpmQuest.h"

PVOID AgpmQuest::MakePacketEmbeddedQuest(INT32 lQuestID, INT32 lParam1, INT32 lParam2, INT32 lItemTID, CHAR* szNPCName)
{
	INT16	nPacketLength = 0;
	return m_csPacketQuest.MakePacket(FALSE, &nPacketLength, 0, 
									&lQuestID,
									&lParam1,
									&lParam2,
									&lItemTID,
									szNPCName);
}

BOOL AgpmQuest::GetFieldEmbeddedPacket(PVOID pvPacket, INT32 &lQuestID, INT32 &lParam1, INT32 &lParam2, INT32 &lItemTID, CHAR*& szNPCName)
{
	m_csPacketQuest.GetField(FALSE, pvPacket, NULL,
							&lQuestID,
							&lParam1,
							&lParam2,
							&lItemTID,
							&szNPCName);

	return TRUE;
}

PVOID AgpmQuest::MakePacketInitQuest(INT16 *pnPacketLength, INT32 lCID, AgpdCurrentQuest* pCurrentQuest)
{
	PVOID pvEmbeddedPacket = MakePacketEmbeddedQuest(pCurrentQuest->lQuestID, 
												pCurrentQuest->lParam1, pCurrentQuest->lParam2, 0, pCurrentQuest->szNPCName);

	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_INIT_QUEST;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
								&cOperation,
								&lCID,
								NULL,
								pvEmbeddedPacket,
								0,
								0,
								0,
								NULL,
								NULL,
								NULL);
}

// 새로운 퀘스트를 요청
PVOID AgpmQuest::MakePacketRequireQuest(INT16 *pnPacketLength, ApdEvent *pEvent, INT32 lCID, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	PVOID pvEventPacket = m_pcsApmEventManager->MakeBasePacket(pEvent);

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_REQUIRE_QUEST;
//	AGSDQUEST_EXPAND_BLOCK
	PVOID pvPacket =  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE, 
										&cOperation,
										&lCID,
										pvEventPacket,
										NULL,
										&lQuestTID,
										0,
										0,
										NULL,
										NULL,
										NULL);

	if (pvEventPacket)
		m_csPacket.FreePacket(pvEventPacket);

	return pvPacket;
}

PVOID AgpmQuest::MakePacketRequireQuestResult(INT16 *pnPacketLength, INT32 lCID, BOOL bResult, INT32 lQuestTID, PVOID pvEmbeddedPacket)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_REQUIRE_QUEST_RESULT;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
								&cOperation,
								&lCID,
								NULL,
								pvEmbeddedPacket,
								&lQuestTID,
								0,
								&bResult,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmQuest::MakePacketQuestComplete(INT16 *pnPacketLength, ApdEvent *pEvent, INT32 lCID, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	PVOID pvEventPacket = m_pcsApmEventManager->MakeBasePacket(pEvent);

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_QUEST_COMPLETE;
//	AGSDQUEST_EXPAND_BLOCK
	PVOID pvPacket =  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE, 
										&cOperation,
										&lCID,
										pvEventPacket,
										NULL,
										&lQuestTID,
										0,
										0,
										NULL,
										NULL,
										NULL);

	if (!pvEventPacket)
		m_csPacket.FreePacket(pvEventPacket);

	return pvPacket;	
}

PVOID AgpmQuest::MakePacketQuestCompleteResult(INT16 *pnPacketLength, INT32 lCID, BOOL bResult, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_QUEST_COMPLETE_RESULT;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
								&cOperation,
								&lCID,
								NULL,
								NULL,
								&lQuestTID,
								0,
								&bResult,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmQuest::MakePacketInitFlag(INT16 *pnPacketLength, INT32 lCID, AgpdQuest* pcsAgpdQuest)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_INIT_FLAGS;
	INT16 nSize = AGSDQUEST_MAX_FLAG;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
										&cOperation,
										&lCID,
										NULL,
										NULL,
										0,
										0,
										0,
										&pcsAgpdQuest->m_csFlag.m_btQuestBlock1, &nSize,
										&pcsAgpdQuest->m_csFlag.m_btQuestBlock2, &nSize,
										&pcsAgpdQuest->m_csFlag.m_btQuestBlock3, &nSize);

}

PVOID AgpmQuest::MakePacketQuestCancel(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_CANCEL;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
									&cOperation,
									&lCID,
									NULL,
									NULL,
									&lQuestTID,
									0,
									0,
									NULL,
									NULL,
									NULL);
}

PVOID AgpmQuest::MakePacketQuestCancelResult(INT16 *pnPacketLength, INT32 lCID, BOOL bResult, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_CANCEL_RESULT;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
									&cOperation,
									&lCID,
									NULL,
									NULL,
									&lQuestTID,
									0,
									&bResult,
									NULL,
									NULL,
									NULL);
}

PVOID AgpmQuest::MakePacketInventoryFull(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_INVENTORY_FULL;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
									&cOperation,
									&lCID,
									NULL,
									NULL,
									&lQuestTID,
									0,
									0,
									NULL,
									NULL,
									NULL);
}

PVOID AgpmQuest::MakePacketQuestInventoryFull(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID)
{
	if (!pnPacketLength) return NULL;

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_QUEST_INVENTORY_FULL;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
									&cOperation,
									&lCID,
									NULL,
									NULL,
									&lQuestTID,
									0,
									0,
									NULL,
									NULL,
									NULL);
}

PVOID AgpmQuest::MakePacketUpdateQuest(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID, INT32 lItemTID, AgpdCurrentQuest* pCurrentQuest)
{
	if (!pnPacketLength) return NULL;

	PVOID pvEmbeddedPacket = MakePacketEmbeddedQuest(pCurrentQuest->lQuestID, 
												pCurrentQuest->lParam1, pCurrentQuest->lParam2, lItemTID, pCurrentQuest->szNPCName);

	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_UPDATE_QUEST;
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
								&cOperation,
								&lCID,
								NULL,
								pvEmbeddedPacket,
								0,
								0,
								0,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmQuest::MakePacketRequireCheckPoint(INT16 *pnPacketLength, INT32 lCID, ApdEvent *pEvent, INT32 lQuestTID, INT32 lCheckPointIndex)
{
	if (!pnPacketLength) return NULL;
	INT8 cOperation = AGPMQUEST_PACKET_OPERATION_CHECKPOINT;
	PVOID pvEventPacket = m_pcsApmEventManager->MakeBasePacket(pEvent);
//	AGSDQUEST_EXPAND_BLOCK
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMQUEST_PACKET_TYPE,
								&cOperation,
								&lCID,
								pvEventPacket,
								NULL,
								&lQuestTID,
								&lCheckPointIndex,
								0,
								NULL,
								NULL,
								NULL);
}