#include "AgsmSkill.h"

/*
BOOL AgsmSkill::CBMasteryResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkill		*pThis			= (AgsmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase *)		pData;
	INT32			*plBuffer		= (INT32 *)			pCustData;

	switch (plBuffer[1]) {
	case AGPMSKILL_MASTERY_RESULT_NEED_SELECT:
		{
			return pThis->SendMasteryNeedSelect(pcsBase, plBuffer[0]);
		}
		break;

	case AGPMSKILL_MASTERY_RESULT_SP_FULL:
		{
		}
		break;

	case AGPMSKILL_MASTERY_RESULT_NOT_ACTIVE:
		{
		}
		break;

	case AGPMSKILL_MASTERY_RESULT_OK:
		{
		}
		break;

	case AGPMSKILL_MASTERY_RESULT_FAIL:
		{
		}
		break;
	}

	return TRUE;
}
*/

/*
BOOL AgsmSkill::CBMasteryUpdateTotalSP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkill			*pThis			= (AgsmSkill *)		pClass;
	ApBase				*pcsBase		= (ApBase *)		pData;
	INT32				lMasteryIndex	= *(INT32 *)		pCustData;

	UINT32	ulNID			= 0;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			ulNID = pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase);
		}
		break;
	}

	if (ulNID == 0)
		return TRUE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketUpdateMasterySP(pcsBase, lMasteryIndex, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, ulNID);

	pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}
*/

#ifndef	__NEW_MASTERY__

BOOL AgsmSkill::CBMasteryChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkill			*pThis			= (AgsmSkill *)			pClass;
	ApBase				*pcsBase		= (ApBase *)			pData;
	PVOID				pvPacketMastery	= (PVOID)				pCustData;

	if (pThis->ChangeMastery(pcsBase, pvPacketMastery))
	{
		// 성공이다.
		// send result to client
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketOperation(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE_SUCCESS, &nPacketLength);
		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));

		pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);

		for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
		{
			pThis->SendRelayUpdate(pcsBase, i);
		}

		return bSendResult;
	}
	else
	{
		// 실패다.
		// send result to client
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketOperation(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE_FAIL, &nPacketLength);
		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));

		pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);

		return bSendResult;
	}

	return TRUE;
}

BOOL AgsmSkill::CBMasterySpecialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkill		*pThis		= (AgsmSkill *)		pClass;
	ApBase			*pcsBase	= (ApBase *)		pData;
	INT32			*plBuffer	= (INT32 *)			pCustData;

	INT16			nPacketLength	= 0;
	PVOID			pvPacket		= NULL;

	if (plBuffer[3] && plBuffer[2])
	{
		AgpdSkillMastery	*pcsMastery	= pThis->m_pagpmSkill->GetMastery(pcsBase);
		if (pcsMastery)
		{
			for (int i = 0; i < AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY; ++i)
			{
				if (pcsMastery[plBuffer[0]].cInstantSpecializedType[i] == AGPMSKILL_MASTERY_SPECIALIZED_NONE)
				{
					pcsMastery[plBuffer[0]].cInstantSpecializedType[i]			= (INT8) plBuffer[1];
					pcsMastery[plBuffer[0]].ulStartInstantSpecializedTime[i]	= pThis->GetClockCount();

					// 2시간이다.
					UINT64	ullTime	= 0;
					pThis->m_pagpmEventNature->SetTime(AGPMSKILL_MAX_SPECIALIZE_DURATION, 0, 0, &ullTime);
					pcsMastery[plBuffer[0]].ulEndInstantSpecializedTime[i]
						= pThis->GetClockCount() + pThis->m_pagpmEventNature->ConvertGametoRealTime(ullTime);

					break;
				}
			}
		}
	}

	if (plBuffer[3])	// plBuffer[3] is mastery result (TRUE or FALSE)
	{
		pvPacket	= pThis->m_pagpmSkill->MakePacketOperation(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_SUCCESS, &nPacketLength);

		pThis->SendRelayUpdate(pcsBase, plBuffer[0]);
	}
	else
		pvPacket	= pThis->m_pagpmSkill->MakePacketOperation(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_FAIL, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));

	pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);

	if (plBuffer[3])
	{
		pvPacket	= pThis->m_pagpmSkill->MakePacketUpdateMasterySpecialize(pcsBase, plBuffer[0], &nPacketLength);

		if (pvPacket)
		{
			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));

			pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
		}
	}
	
	return bSendResult;
}

BOOL AgsmSkill::CBMasteryUnSpecialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkill		*pThis			= (AgsmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase *)		pData;
	INT32			lMasteryIndex	= *(INT32 *)		pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketUpdateMasterySpecialize(pcsBase, lMasteryIndex, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));

	pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);

	pThis->SendRelayUpdate(pcsBase, lMasteryIndex);

	return bSendResult;
}

#endif	//__NEW_MASTERY__

//BOOL AgsmSkill::CBMasteryRollback(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgsmSkill		*pThis			= (AgsmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return FALSE;
//
//	UINT32			ulNID			= pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase);
//
//	if (!pThis->m_pagpmSkill->CheckMasteryRollbackCost(pcsBase))
//	{
//		pThis->SendMasteryRollbackResult(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_FAIL, ulNID);
//		return FALSE;
//	}
//
//	if (!pThis->m_pagpmSkill->PayMasteryRollbackCost(pcsBase))
//	{
//		pThis->SendMasteryRollbackResult(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_FAIL, ulNID);
//		return FALSE;
//	}
//
//	pThis->m_pagpmSkill->ResetAllMastery(pcsBase);
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//		pThis->SendRelayUpdate(pcsBase, i);
//
//	return pThis->SendMasteryRollbackResult(pcsBase, AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_SUCCESS, ulNID);
//}
//
//BOOL AgsmSkill::SendMasteryInfo(ApBase *pcsBase)
//{
//	UINT32	ulNID	= 0;
//
//	switch (pcsBase->m_eType) {
//	case APBASE_TYPE_CHARACTER:
//		{
//			ulNID = m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase);
//		}
//		break;
//	}
//
//	return SendMasteryInfo(pcsBase, ulNID);
//}
//
//BOOL AgsmSkill::SendMasteryInfo(ApBase *pcsBase, UINT32 ulNID)
//{
//	if (!pcsBase || ulNID == 0)
//		return FALSE;
//
//#ifdef	__NEW_MASTERY__
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pagpmSkill->MakePacketMasteryAllUpdate(pcsBase, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	SendPacket(pvPacket, nPacketLength, ulNID);
//
//	m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	nPacketLength	= 0;
//	pvPacket		= m_pagpmSkill->MakePacketMasteryChangedNode(pcsBase, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	SendPacket(pvPacket, nPacketLength, ulNID);
//
//	m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	return TRUE;
//
//#else
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pagpmSkill->MakePacketAddMastery(pcsBase, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);
//
//	m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//
//#endif	//__NEW_MASTERY__
//}

/*
BOOL AgsmSkill::SendMasteryNeedSelect(ApBase *pcsBase, INT32 lMasteryIndex)
{
	UINT32	ulNID	= 0;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			ulNID = m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase);
		}
		break;
	}

	return SendMasteryNeedSelect(pcsBase, lMasteryIndex, ulNID);
}

BOOL AgsmSkill::SendMasteryNeedSelect(ApBase *pcsBase, INT32 lMasteryIndex, UINT32 ulNID)
{
	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmSkill->MakePacketNeedSelectSkill(pcsBase, lMasteryIndex, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);

	m_pagpmSkill->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}
*/

//INT32 AgsmSkill::GetTotalSP(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return (-1);
//
//	AgpdSkillAttachData	*pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	INT32	lTotalSP	= 0;
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//#ifdef	__NEW_MASTERY__
//		lTotalSP += (INT32) pcsAttachData->m_csMastery[i].m_ucTotalInputSP;
//#else
//		if (pcsAttachData->m_csMastery[i].bIsActive)
//		{
//			lTotalSP += pcsAttachData->m_csMastery[i].lTotalSP;
//		}
//#endif	//__NEW_MASTERY__
//	}
//
//	return lTotalSP;
//}

#ifndef	__NEW_MASTERY__

BOOL AgsmSkill::ChangeMastery(ApBase *pcsBase, PVOID pvPacketMastery)
{
	if (!pcsBase || !pvPacketMastery)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	if (!GetFieldChangeMasteryPacket(pcsBase, pvPacketMastery))
		return FALSE;

	INT32	lOriginalTotalSP	= GetTotalSP(pcsBase);

	if (!CheckReceiveMastery(pcsBase))
		return FALSE;

	if (!UpdateReceiveMastery(pcsBase))
		return FALSE;

	INT32	lNewTotalSP			= GetTotalSP(pcsBase);

	// 새로운 sp랑 위 sp랑 비교해서 character의 sp를 조절해준다.
	m_pagpmCharacter->UpdateSkillPoint((AgpdCharacter *) pcsBase, lOriginalTotalSP - lNewTotalSP);

	// Rollback인 경우 여기서 대가를 지불한다.
	if (pcsAttachData->m_bIsMasteryRollback)
		m_pagpmSkill->PayMasteryRollbackCost(pcsBase);

	return TRUE;
}

BOOL AgsmSkill::GetFieldChangeMasteryPacket(ApBase *pcsBase, PVOID pvPacketMastery)
{
	if (!pcsBase || !pvPacketMastery)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= m_pagpmSkill->GetMasteryTemplate(pcsBase);
	if (!pcsMasteryTemplate)
		return FALSE;

	ZeroMemory(pcsAttachData->m_csMasteryBuffer, sizeof(AgpdSkillMastery) * AGPMSKILL_MAX_MASTERY);
	CopyMemory(&pcsAttachData->m_csMasteryTemplateBuffer, pcsMasteryTemplate, sizeof(AgpdSkillMasteryTemplate));

	PVOID	pvPacketMasteryTree[AGPMSKILL_MAX_MASTERY];
	ZeroMemory(pvPacketMasteryTree, sizeof(PVOID) * AGPMSKILL_MAX_MASTERY);

	m_pagpmSkill->m_csPacketMastery.GetField(FALSE, pvPacketMastery, 0,
											&pvPacketMasteryTree);

	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
	{
		if (!pvPacketMasteryTree[i])
			break;

		INT8	cMasteryIndex	= (-1);
		INT32	lTotalSP		= (-1);
		PVOID	pvPacketMasteryActiveNode[AGPMSKILL_MAX_MASTERY_SKILL * AGPMSKILL_MAX_MASTERY_TREE_DEPTH * AGPMSKILL_MAX_NUM_SKILL_NODE];

		ZeroMemory(pvPacketMasteryActiveNode, sizeof(PVOID) * AGPMSKILL_MAX_MASTERY_SKILL * AGPMSKILL_MAX_MASTERY_TREE_DEPTH * AGPMSKILL_MAX_NUM_SKILL_NODE);

		m_pagpmSkill->m_csPacketMasteryTree.GetField(FALSE, pvPacketMasteryTree[i], 0,
													&cMasteryIndex,
													&lTotalSP,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													&pvPacketMasteryActiveNode);

		if (cMasteryIndex == (-1) || lTotalSP == (-1))
			continue;

		pcsAttachData->m_csMasteryBuffer[cMasteryIndex].bIsActive	= TRUE;
		pcsAttachData->m_csMasteryBuffer[cMasteryIndex].lTotalSP	= lTotalSP;

		if (!pvPacketMasteryActiveNode[0])
			continue;

		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL * AGPMSKILL_MAX_MASTERY_TREE_DEPTH * AGPMSKILL_MAX_NUM_SKILL_NODE; ++j)
		{
			if (!pvPacketMasteryActiveNode[j])
				break;

			CHAR	*szSkillName	= NULL;
			INT32	lInputSP		= (-1);
			INT32	lNeedSP			= (-1);

			m_pagpmSkill->m_csPacketMasteryActiveNode.GetField(FALSE, pvPacketMasteryActiveNode[j], 0,
														&szSkillName,
														&lNeedSP,
														&lInputSP);

			if (!szSkillName ||
				!szSkillName[0])
				continue;

			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_SKILL; ++k)
			{
				BOOL	bSetData	= FALSE;

				AgpdMasterySkillTree	*pcsSkillTree	= &pcsAttachData->m_csMasteryTemplateBuffer.m_csMastery[cMasteryIndex].m_stMasterySkillTree[k];

				for (int l = 0; l < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++l)
				{
					for (int m = 0; m < pcsSkillTree->m_csSkillNode[l].m_lNumSkill; ++m)
					{
						if (!strncmp(pcsSkillTree->m_csSkillNode[l].m_szSkillName[m], szSkillName, AGPMSKILL_MAX_SKILL_NAME))
						{
							pcsSkillTree->m_csSkillNode[l].m_bIsActiveSkill[m]	= TRUE;
							pcsSkillTree->m_csSkillNode[l].m_lNeedSP[m]			= lNeedSP;
							pcsSkillTree->m_csSkillNode[l].m_lInputSP[m]		= lInputSP;

							bSetData	= TRUE;
							break;
						}
					}

					if (bSetData)
						break;
				}

				if (bSetData)
					break;
			}
		}
	}

	return TRUE;
}

BOOL AgsmSkill::CheckReceiveMastery(ApBase *pcsBase)
{
	if (!pcsBase)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	INT32	lChangedSP	= 0;
	INT32	lOriginalSP	= 0;

	BOOL	bRollback	= FALSE;

	INT32	lLevel		= 0;
	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
	{
		m_pagpmFactors->GetValue(&((AgpdCharacter *) pcsBase)->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	}

	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
	{
		if (pcsAttachData->m_csMasteryBuffer[i].bIsActive)
		{
			lChangedSP += pcsAttachData->m_csMasteryBuffer[i].lTotalSP;
			lOriginalSP	+= pcsAttachData->m_csMastery[i].lTotalSP;

			if (pcsAttachData->m_csMasteryBuffer[i].lTotalSP < pcsAttachData->m_csMastery[i].lTotalSP)
				bRollback = TRUE;

			// 자기 레벨보다 더 많은 포인트를 마스터리에 넣을 순 없다.
			if (lLevel < pcsAttachData->m_csMasteryBuffer[i].lTotalSP)
				return FALSE;
		}
		/*
		else
		{
			lChangedSP += pcsAttachData->m_csMastery[i].lTotalSP;
			lOriginalSP += pcsAttachData->m_csMastery[i].lTotalSP;
		}
		*/
	}

	lOriginalSP += m_pagpmCharacter->GetSkillPoint((AgpdCharacter *) pcsBase);

	// 절대 클수 없다.
	if (lOriginalSP < lChangedSP)
		return FALSE;

	for (i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
	{
		if (!pcsAttachData->m_csMasteryBuffer[i].bIsActive)
			continue;

		INT32	lMaxInputSP	= 0;

		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
		{
			AgpdMasterySkillTree	*pcsSkillTree	= &pcsAttachData->m_csMasteryTemplateBuffer.m_csMastery[i].m_stMasterySkillTree[j];

			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
			{
				for (int l = 0; l < pcsSkillTree->m_csSkillNode[k].m_lNumSkill; ++l)
				{
					//if (pcsSkillTree->m_csSkillNode[k].m_lInputSP[l] + pcsSkillTree->m_csSkillNode[k].m_lNeedSP[l] > pcsAttachData->m_csMasteryBuffer[i].lTotalSP)
					//	return FALSE;

					if (pcsSkillTree->m_csSkillNode[k].m_lInputSP[l] < 0)
						return FALSE;

					if (lMaxInputSP < pcsSkillTree->m_csSkillNode[k].m_lInputSP[l])
						lMaxInputSP = pcsSkillTree->m_csSkillNode[k].m_lInputSP[l];
				}
			}
		}

		if (lMaxInputSP > pcsAttachData->m_csMasteryBuffer[i].lTotalSP)
			return FALSE;
	}

	if (bRollback)
	{
		if (!m_pagpmSkill->CheckMasteryRollbackCost(pcsBase))
			return FALSE;

		pcsAttachData->m_bIsMasteryRollback = TRUE;
	}
	else
		pcsAttachData->m_bIsMasteryRollback = FALSE;

	return TRUE;
}

BOOL AgsmSkill::UpdateReceiveMastery(ApBase *pcsBase)
{
	if (!pcsBase)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
	{
		if (!pcsAttachData->m_csMasteryBuffer[i].bIsActive)
			continue;

		pcsAttachData->m_csMastery[i].bIsActive	= pcsAttachData->m_csMasteryBuffer[i].bIsActive;
		pcsAttachData->m_csMastery[i].lTotalSP	= pcsAttachData->m_csMasteryBuffer[i].lTotalSP;

		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
		{
			AgpdMasterySkillTree	*pcsSkillTree		= &pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_stMasterySkillTree[j];
			AgpdMasterySkillTree	*pcsBufferSkillTree	= &pcsAttachData->m_csMasteryTemplateBuffer.m_csMastery[i].m_stMasterySkillTree[j];

			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
			{
				for (int l = 0; l < pcsSkillTree->m_csSkillNode[k].m_lNumSkill; ++l)
				{
					if (pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l] &&
						!pcsBufferSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l])
					{
						// 스킬 삭제
						AgpdSkill *pcsSkill = m_pagpmSkill->GetSkill(pcsSkillTree->m_csSkillNode[k].m_lSkillID[l]);

						if (pcsSkill)
						{
							if (m_pagpmSkill->IsPassiveSkill(pcsSkill))
								EndPassiveSkill(pcsSkill, TRUE);

							m_pagpmSkill->ForgetSkill(pcsBase, pcsSkill);
						}

						pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l]	= FALSE;
						pcsSkillTree->m_csSkillNode[k].m_lSkillID[l]		= AP_INVALID_SKILLID;
						pcsSkillTree->m_csSkillNode[k].m_lInputSP[l]		= 0;
					}
					else if (!pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l] &&
						pcsBufferSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l])
					{
						// 스킬 추가
						AgpdSkill	*pcsSkill = m_pagpmSkill->LearnSkill(pcsBase, pcsBufferSkillTree->m_csSkillNode[k].m_szSkillName[l], i, pcsBufferSkillTree->m_csSkillNode[k].m_lNeedSP[l]);
						if (!pcsSkill)
							return FALSE;

						if (m_pagpmSkill->IsPassiveSkill(pcsSkill))
							CastPassiveSkill(pcsSkill);

						pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l]	= TRUE;
						pcsSkillTree->m_csSkillNode[k].m_lSkillID[l]		= pcsSkill->m_lID;
						pcsSkillTree->m_csSkillNode[k].m_lInputSP[l]		= pcsBufferSkillTree->m_csSkillNode[k].m_lInputSP[l];
					}

					if (pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l] &&
						pcsBufferSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l])
					{
						// lNeedSP & lInputSP 동기화
						pcsSkillTree->m_csSkillNode[k].m_lNeedSP[l] = pcsBufferSkillTree->m_csSkillNode[k].m_lNeedSP[l];

						INT32	lDifferInputSP	= pcsBufferSkillTree->m_csSkillNode[k].m_lInputSP[l] - pcsSkillTree->m_csSkillNode[k].m_lInputSP[l];

						if (lDifferInputSP != 0)
						{
							AgpdSkill	*pcsSkill	= m_pagpmSkill->GetSkill(pcsSkillTree->m_csSkillNode[k].m_lSkillID[l]);
							m_pagpmSkill->UpdateSkillPoint(pcsSkill, lDifferInputSP);

							pcsSkillTree->m_csSkillNode[k].m_lInputSP[l]	= pcsBufferSkillTree->m_csSkillNode[k].m_lInputSP[l];
						}
					}
				}
			}
		}
	}

	// lNeedSP를 세팅한다.
	for (i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
	{
		if (!pcsAttachData->m_csMasteryBuffer[i].bIsActive)
			continue;

		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
		{
			AgpdMasterySkillTree	*pcsSkillTree		= &pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_stMasterySkillTree[j];

			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
			{
				INT32	lActiveMaxNeedSP	= 0;
				INT32	lActiveSkill		= 0;
				INT32	lNotActiveSkill		= 0;

				for (int l = 0; l < pcsSkillTree->m_csSkillNode[k].m_lNumSkill; ++l)
				{
					if (pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l])
					{
						++lActiveSkill;

						if (pcsSkillTree->m_csSkillNode[k].m_lNeedSP[l] > lActiveMaxNeedSP)
							lActiveMaxNeedSP = pcsSkillTree->m_csSkillNode[k].m_lNeedSP[l];
					}
					else
						++lNotActiveSkill;
				}

				if (lNotActiveSkill == 0)
					continue;
				else if (lActiveSkill == 0)
				{
					AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= m_pagpmSkill->GetMasteryTemplate(pcsBase);
					if (!pcsMasteryTemplate)
						return FALSE;

					for (l = 0; l < pcsSkillTree->m_csSkillNode[k].m_lNumSkill; ++l)
					{
						pcsSkillTree->m_csSkillNode[k].m_lNeedSP[l] = pcsMasteryTemplate->m_csMastery[k].m_stMasterySkillTree[j].m_csSkillNode[k].m_lNeedSP[0];
					}
				}
				else if (lActiveMaxNeedSP > 0)
				{
					for (l = 0; l < pcsSkillTree->m_csSkillNode[k].m_lNumSkill; ++l)
					{
						if (!pcsSkillTree->m_csSkillNode[k].m_bIsActiveSkill[l])
						{
							pcsSkillTree->m_csSkillNode[k].m_lNeedSP[l] = lActiveMaxNeedSP + AGPMSKILL_MAX_UPGRADE_SP;
						}
					}
				}
			}
		}
	}

	return TRUE;
}

#endif	//__NEW_MASTERY__

//BOOL AgsmSkill::MakeStringMasteryActiveNodeSequence(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer, INT32 lStringBufferSize)
//{
//	if (!pcsBase ||
//		!szStringBuffer ||
//		lStringBufferSize < 1 ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//#ifdef	__NEW_MASTERY__
//
//	AgpdSkillAttachData			*pcsAttachData		= m_pagpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	ZeroMemory(szStringBuffer, sizeof(CHAR) * lStringBufferSize);
//
//	if (pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP <= 0)
//		return TRUE;
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//	{
//		if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][0].m_lSkillTID == AP_INVALID_SKILLID)
//			continue;
//
//		sprintf(szStringBuffer + strlen(szStringBuffer), "%d:", i);
//
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID == 0)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID != AP_INVALID_SKILLID)
//			{
//				sprintf(szStringBuffer + strlen(szStringBuffer), "%d,", pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID);
//			}
//		}
//
//		sprintf(szStringBuffer + strlen(szStringBuffer), ";");
//	}
//
//#else
//
//	AgpdSkillMastery			*pcsMastery			= m_pagpmSkill->GetMastery(pcsBase);
//	if (!pcsMastery)
//		return FALSE;
//
//	AgpdSkillAttachData			*pcsAttachData		= m_pagpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= &pcsAttachData->m_csMasteryTemplate;
//	if (!pcsMasteryTemplate)
//		return FALSE;
//
//	ZeroMemory(szStringBuffer, sizeof(CHAR) * lStringBufferSize);
//
//	if (pcsMastery[lMasteryIndex].lTotalSP <= 0)
//		return TRUE;
//
//	INT32	lPrintedSize	= 0;
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//	{
//		AgpdMasterySkillTree	*pcsSkillTree	= &pcsMasteryTemplate->m_csMastery[lMasteryIndex].m_stMasterySkillTree[i];
//
//		if (pcsSkillTree->m_csSkillNode[0].m_lNumSkill == 0)
//			break;
//
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			// 노드들중 Active된 노드들을 m_lNeedSP를 기준으로 오름차순 정렬한다.
//
//			// szStringBuffer에 순서대로 SkillTID를 출력한다.
//			// SkillTID 간의 구분은 ','로 한다.
//			INT32	lSize	= SortPrintActiveNode(&pcsSkillTree->m_csSkillNode[j], szStringBuffer + lPrintedSize, lStringBufferSize - lPrintedSize);
//			if (lSize > 0)
//			{
//				lPrintedSize += (lSize);
//			}
//			else
//				break;
//		}
//
//		// Skill Tree간 구분을 위하여 다음 Skill Tree가 있다면 ';'로 구분해준다.
//		if (lPrintedSize >= lStringBufferSize)
//			return FALSE;
//
//		sprintf(szStringBuffer + lPrintedSize - 1, ";");
//	}
//
//	if (lPrintedSize > 0)
//		szStringBuffer[--lPrintedSize]	= '\0';
//
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}
//
//#ifndef	__NEW_MASTERY__
//
//INT32 AgsmSkill::SortPrintActiveNode(AgpdMasterySkillNode *pcsSkillNode, CHAR *szStringBuffer, INT32 lStringBufferSize)
//{
//	if (!pcsSkillNode || !szStringBuffer || lStringBufferSize < 1)
//		return 0;
//
//	// 노드들중 Active된 노드들을 m_lNeedSP를 기준으로 오름차순 정렬한다.
//	
//	// szStringBuffer에 순서대로 SkillTID를 출력한다.
//	// SkillTID 간의 구분은 ','로 한다.
//
//	INT32	lBaseNeedSP		= pcsSkillNode->m_lBaseNeedSP;
//
//	for (int i = 0; i < AGPMSKILL_MAX_NUM_SKILL_NODE; ++i)
//	{
//		for (int j = 0; j < AGPMSKILL_MAX_NUM_SKILL_NODE; ++j)
//		{
//			if (pcsSkillNode->m_lNeedSP[j] == lBaseNeedSP)
//			{
//				if (pcsSkillNode->m_bIsActiveSkill[j])
//				{
//					lBaseNeedSP += AGPMSKILL_MAX_UPGRADE_SP;
//					if (lBaseNeedSP == 6)
//						lBaseNeedSP = 5;
//
//					AgpdSkillTemplate	*pcsSkillTemplate	= m_pagpmSkill->GetSkillTemplate(pcsSkillNode->m_szSkillName[j]);
//					if (!pcsSkillTemplate)
//						return 0;
//
//					sprintf(szStringBuffer + strlen(szStringBuffer), "%d,", pcsSkillNode->m_lNeedSP[j] + pcsSkillTemplate->m_lID * 100);
//
//					break;
//				}
//				else
//					return strlen(szStringBuffer);
//			}
//
//			// szStringBuffer의 길이가 충분한가 본다.
//			if (strlen(szStringBuffer) + 4 >= lStringBufferSize)
//				return 0;
//		}
//	}
//
//	return strlen(szStringBuffer);
//}
//
//#endif	//__NEW_MASTERY__
//
//BOOL AgsmSkill::ParseStringMasteryActiveNodeSequence(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer, INT32 lTotalMasteryPoint)
//{
//	if (!pcsBase ||
//		!szStringBuffer ||
//		lTotalMasteryPoint < 0 ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//#ifdef	__NEW_MASTERY__
//
//	if (lTotalMasteryPoint == 0)
//		return TRUE;
//
//	INT32	lStringBufferLength	= strlen(szStringBuffer);
//	if (lStringBufferLength < 1)
//		return TRUE;
//
//	AgpdSkillAttachData		*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP	= (UINT8) lTotalMasteryPoint;
//	pcsAttachData->m_csMastery[lMasteryIndex].m_ucMasteryIndex	= (UINT8) lMasteryIndex;
//
//	INT32	lPrevStringPoint	= 0;
//
//	INT32	lSkillTreeIndex		= 0;
//	INT32	lSkillTID			= 0;
//
//	for (int i = 0; i < lStringBufferLength; ++i)
//	{
//		if (szStringBuffer[i] == ':')
//		{
//			lSkillTreeIndex	= 0;
//
//			sscanf(szStringBuffer + lPrevStringPoint, "%d", &lSkillTreeIndex);
//
//			lPrevStringPoint	= i + 1;
//		}
//		else if (szStringBuffer[i] == ';')
//		{
//			lPrevStringPoint	= i + 1;
//		}
//		else if (szStringBuffer[i] == ',')
//		{
//			lSkillTID	= 0;
//
//			sscanf(szStringBuffer + lPrevStringPoint, "%d", &lSkillTID);
//
//			if (lSkillTID != 0)
//			{
//				for (int j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//				{
//					if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_lSkillTID == 0)
//						break;
//
//					if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_lSkillTID == lSkillTID &&
//						pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_ucSPForActive <= (UINT8) lTotalMasteryPoint)
//					{
//						AgpdSkill	*pcsSkill	= m_pagpmSkill->LearnSkill(pcsBase, lSkillTID, lMasteryIndex);
//						if (pcsSkill)
//						{
//							pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_bIsChanged	= TRUE;
//							pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_lSkillID		= pcsSkill->m_lID;
//
//							if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_ucMaxInputSP > (UINT8) lTotalMasteryPoint)
//								m_pagpmSkill->SetSkillPoint(pcsSkill, lTotalMasteryPoint);
//							else
//								m_pagpmSkill->SetSkillPoint(pcsSkill, pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[lSkillTreeIndex][j].m_ucMaxInputSP);
//						}
//
//						break;
//					}
//				}
//			}
//
//			lPrevStringPoint	= i + 1;
//		}
//		else
//			continue;
//	}
//
//	// active 되지 않은 스킬들에 대해 SPForActive 수치들을 조정해준다.
//	for (i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//	{
//		if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][0].m_lSkillTID == 0)
//			continue;
//
//		INT8	cActiveSkill[AGPMSKILL_MAX_MASTERY_TREE_DEPTH];
//		ZeroMemory(cActiveSkill, sizeof(INT8) * AGPMSKILL_MAX_MASTERY_TREE_DEPTH);
//
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID == 0)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID != 0)
//			{
//				++cActiveSkill[pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth];
//			}
//		}
//
//		for (j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID == 0)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID == 0)
//			{
//				if (cActiveSkill[pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth] > 0)
//				{
//					pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucSPForActive
//						+= AGPMSKILL_MAX_UPGRADE_SP * cActiveSkill[pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth];
//
//					pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_bIsChanged	= TRUE;
//				}
//			}
//		}
//	}
//
//#else
//
//	if (lTotalMasteryPoint == 0)
//		return TRUE;
//
//	INT32	lStringBufferLength	= strlen(szStringBuffer);
//	INT32	lPrevStringPoint	= 0;
//
//	if (lStringBufferLength == 0)
//		return TRUE;
//
//	AgpdSkillAttachData		*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	pcsAttachData->m_csMastery[lMasteryIndex].bIsActive		= TRUE;
//	pcsAttachData->m_csMastery[lMasteryIndex].lTotalSP		= lTotalMasteryPoint;
//
//	pcsAttachData->m_csMastery[lMasteryIndex].lMasteryIndex	= lMasteryIndex;
//
//	for (int j = 0; j < lStringBufferLength; ++j)
//	{
//		if (szStringBuffer[j] == ',' ||
//			szStringBuffer[j] == ';' ||
//			j == lStringBufferLength - 1)
//		{
//			CHAR	szSkillTID[5];
//			ZeroMemory(szSkillTID, sizeof(CHAR) * 5);
//
//			if (j == lStringBufferLength - 1)
//				++j;
//
//			CopyMemory(szSkillTID, szStringBuffer + lPrevStringPoint, j - lPrevStringPoint);
//
//			INT32	lSkillTID	= atoi(szSkillTID);
//
//			INT32	lActiveSP	= lSkillTID % 100;
//			lSkillTID			= (INT32) (lSkillTID / 100);
//
//			AgpdSkillTemplate	*pcsSkillTemplate	= m_pagpmSkill->GetSkillTemplate(lSkillTID);
//			if (!pcsSkillTemplate)
//				return FALSE;
//
//			if (!m_pagpmSkill->LearnSkill(pcsBase, pcsSkillTemplate->m_szName, lMasteryIndex, lActiveSP))
//				return FALSE;
//
//			lPrevStringPoint	= j + 1;
//		}
//	}
//
//	// 마스터리 전체를 돌면서 세팅이 덜된 값들에 대한 세팅을 해준다.
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//	{
//		AgpdMasterySkillTree	*pcsSkillTree	= &pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_stMasterySkillTree[i];
//
//		for (j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsSkillTree->m_csSkillNode[j].m_lNumSkill == 0)
//				continue;
//
//			for (int k = 0; k < AGPMSKILL_MAX_NUM_SKILL_NODE; ++k)
//			{
//				if (pcsSkillTree->m_csSkillNode[j].m_lSkillID[k] == AP_INVALID_SKILLID)
//					continue;
//
//				pcsSkillTree->m_csSkillNode[j].m_bIsActiveSkill[k]	= TRUE;
//
//				if (pcsSkillTree->m_csSkillNode[j].m_lMaxInputSP < lTotalMasteryPoint)
//					pcsSkillTree->m_csSkillNode[j].m_lInputSP[k]	= pcsSkillTree->m_csSkillNode[j].m_lMaxInputSP;
//				else
//					pcsSkillTree->m_csSkillNode[j].m_lInputSP[k]	= lTotalMasteryPoint;
//
//				m_pagpmSkill->SetSkillPoint(m_pagpmSkill->GetSkill(pcsSkillTree->m_csSkillNode[j].m_lSkillID[k]), pcsSkillTree->m_csSkillNode[j].m_lInputSP[k]);
//			}
//		}
//	}
//
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}

//BOOL AgsmSkill::EncodingMasterySpecialize(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer, INT32 lStringSize)
//{
//	if (!pcsBase || 
//		!szStringBuffer ||
//		lStringSize < 1 ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	return TRUE;
//}
//
//BOOL AgsmSkill::DecodingMasterySpecialize(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer)
//{
//	if (!pcsBase || 
//		!szStringBuffer ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	return TRUE;
//}

//#ifdef	__NEW_MASTERY__
//
//BOOL AgsmSkill::CBMasteryUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmSkill		*pThis			= (AgsmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//	INT32			lMasteryIndex	= *(INT32 *)		pCustData;
//
//	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
//	{
//		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
//			return TRUE;
//
//		if (!((AgpdCharacter *) pcsBase)->m_bIsAddMap)
//			return TRUE;
//	}
//
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return TRUE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketMasteryUpdate(pcsBase, (INT8) lMasteryIndex, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));
//
//	pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	pThis->SendRelayUpdate(pcsBase, lMasteryIndex);
//
//	return bSendResult;
//}
//
//BOOL AgsmSkill::CBMasteryNodeUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmSkill		*pThis			= (AgsmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//	PVOID			*ppvBuffer		= (PVOID *)			pCustData;
//
//	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
//	{
//		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
//			return TRUE;
//
//		if (!((AgpdCharacter *) pcsBase)->m_bIsAddMap)
//			return TRUE;
//	}
//
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return TRUE;
//
//	INT32			lMasteryIndex	= (INT32)			ppvBuffer[0];
//	INT32			*plChangedNode	= (INT32 *)			ppvBuffer[1];
//	INT32			lNumChangedNode	= (INT32)			ppvBuffer[2];
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketMasteryNodeUpdate(pcsBase, (INT8) lMasteryIndex, plChangedNode, lNumChangedNode, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));
//
//	pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgsmSkill::CBMasteryAddSPResult(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmSkill		*pThis			= (AgsmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//	PVOID			*ppvBuffer		= (PVOID *)			pCustData;
//
//	INT32			lResult			= (INT32)			ppvBuffer[0];
//	INT32			*lSelectTID		= (INT32 *)			ppvBuffer[1];
//	INT32			lArraySize		= (INT32)			ppvBuffer[2];
//	INT32			lMasteryIndex	= (INT32)			ppvBuffer[3];
//
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return TRUE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= pThis->m_pagpmSkill->MakePacketAddSPToMasteryResult(pcsBase, (INT8) lMasteryIndex, (INT8) lResult, lSelectTID, lArraySize, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsBase));
//
//	pThis->m_pagpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	// Log
//	if(lResult == AGPMSKILL_MASTERY_RESULT_OK)
//		pThis->WriteMasteryLog(pcsBase);
//
//	return bSendResult;
//}
//
//#endif	//__NEW_MASTERY__

//BOOL AgsmSkill::WriteMasteryLog(ApBase* pcsBase)
//{
//	if(!m_pagpmLog)
//		return TRUE;
//
//	if(!pcsBase)
//		return FALSE;
//
//	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pcsBase;
//	if(!pcsAgpdCharacter)
//		return FALSE;
//
//	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsBase);
//	if(!pcsAttachData)
//		return FALSE;
//
//	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
//	INT32 lLevel = 0;
//	m_pagpmFactors->GetValue(&pcsAgpdCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
//
//	INT32 lRemainPoint = m_pagpmCharacter->GetSkillPoint(pcsAgpdCharacter);
//	INT32 arrMasteryPoint[AGPMSKILL_MAX_MASTERY];
//
//	for(int i = 0; i < AGPMSKILL_MAX_MASTERY; i++)
//	{
//#ifdef	__NEW_MASTERY__
//		arrMasteryPoint[i] = (INT32)pcsAttachData->m_csMastery[i].m_ucTotalInputSP;
//#else
//		if(pcsAttachData->m_csMastery[i].bIsActive)
//		{
//			arrMasteryPoint[i] = pcsAttachData->m_csMastery[i].lTotalSP;
//		}
//#endif	//__NEW_MASTERY__
//	}
//
//	return m_pagpmLog->WriteLog_SkillPoint(pcsAgpdCharacter->m_szID,
//																		lCharTID,
//																		lLevel,
//																		lRemainPoint,
//																		arrMasteryPoint);
//}
