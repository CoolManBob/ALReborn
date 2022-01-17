#include "AgcmSkill.h"

void DummyFunction3()
{
	// 이건 빌드 워닝 방지용
	// 더미 펑션

	DebugBreak();
}


//BOOL AgcmSkill::SetRollbackMastery()
//{
//	AgpdCharacter	*pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	if (!m_pcsAgpmSkill->CheckMasteryRollbackCost((ApBase *) pcsSelfCharacter))
//		return FALSE;
//
//#ifndef	__NEW_MASTERY__
//	AgpdSkillAttachData	*pcsAttachSkillData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsSelfCharacter);
//	if (!pcsAttachSkillData)
//		return FALSE;
//
//	pcsAttachSkillData->m_bIsMasteryRollback = TRUE;
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::ResetRollbackMastery()
//{
//#ifndef	__NEW_MASTERY__
//	AgpdCharacter	*pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachSkillData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsSelfCharacter);
//	if (!pcsAttachSkillData)
//		return FALSE;
//
//	pcsAttachSkillData->m_bIsMasteryRollback = FALSE;
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::IsUpdateMastery()
//{
//	if (m_bIsProcessUpdateMastery &&
//		m_ulStartProcessUpdateMasteryTime > 0 &&
//		m_ulStartProcessUpdateMasteryTime + 4000 < GetPrevClockCount())
//		return FALSE;
//
//	return m_bIsProcessUpdateMastery;
//}
//
//BOOL AgcmSkill::SetUpdateMasteryFlag()
//{
//	m_ulStartProcessUpdateMasteryTime	= GetPrevClockCount();
//
//	m_bIsProcessUpdateMastery	= TRUE;
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::ResetUpdateMasteryFlag()
//{
//	m_bIsProcessUpdateMastery	= FALSE;
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::StartUpdateMastery(BOOL bRollback)
//{
//	//SetUpdateMasteryFlag();
//
//	if (bRollback)
//		if (!SetRollbackMastery())
//		{
//			ResetUpdateMasteryFlag();
//			return FALSE;
//		}
//
//#ifndef	__NEW_MASTERY__
//
//	ZeroMemory(m_lTempCreatedSkillID, sizeof(INT32) * AGCMSKILL_MAX_TEMP_CREATED_SKILL);
//
//	AgpdCharacter	*pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//	{
//		//ResetUpdateMasteryFlag();
//		return FALSE;
//	}
//
//	AgpdSkillAttachData	*pcsAttachSkillData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsSelfCharacter);
//	if (!pcsAttachSkillData)
//	{
//		//ResetUpdateMasteryFlag();
//		return FALSE;
//	}
//
//	CopyMemory(pcsAttachSkillData->m_csMasteryBuffer, pcsAttachSkillData->m_csMastery, sizeof(AgpdSkillMastery) * AGPMSKILL_MAX_MASTERY);
//	CopyMemory(&pcsAttachSkillData->m_csMasteryTemplateBuffer, &pcsAttachSkillData->m_csMasteryTemplate, sizeof(AgpdSkillMasteryTemplate));
//
//	pcsAttachSkillData->m_lCharSkillPointBackup	= m_pcsAgpmFactors->GetSkillPoint(&pcsSelfCharacter->m_csFactor);
//
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::StopUpdateMastery(BOOL bOK)
//{
//	AgpdCharacter	*pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachSkillData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsSelfCharacter);
//	if (!pcsAttachSkillData)
//		return FALSE;
//
//	ResetRollbackMastery();
//
//#ifdef	__NEW_MASTERY__
//
//	RefreshNeedSkillPoint((ApBase *) pcsSelfCharacter);
//
//	if (bOK)
//	{
//		/*
//		if (!SendChangeMastery())
//			return FALSE;
//		*/
//		
//		//RefreshSkillPoint((ApBase *) pcsSelfCharacter);
//
//		// 이부분에서 마스터리 내용이 바뀌었다고 콜백을 불러줘야 한다. 그래야 UI 상에서 Refresh를 한다.
//		EnumCallback(AGCMSKILL_CB_ID_MASTERY_UPDATE, pcsSelfCharacter, NULL);
//	}
//	else
//	{
//		//RefreshSkillPoint((ApBase *) pcsSelfCharacter);
//
//		// 이부분에서 마스터리 내용이 바뀌었다고 콜백을 불러줘야 한다. 그래야 UI 상에서 Refresh를 한다.
//		EnumCallback(AGCMSKILL_CB_ID_MASTERY_UPDATE, pcsSelfCharacter, NULL);
//	}
//
//#else
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//		pcsAttachSkillData->m_csMastery[i].bIsUpdate = FALSE;
//
//	RefreshNeedSkillPoint((ApBase *) pcsSelfCharacter);
//
//	if (bOK)
//	{
//		/*
//		if (!SendChangeMastery())
//			return FALSE;
//		*/
//
//		ZeroMemory(pcsAttachSkillData->m_csMasteryBuffer, sizeof(AgpdSkillMastery) * AGPMSKILL_MAX_MASTERY);
//		ZeroMemory(&pcsAttachSkillData->m_csMasteryTemplateBuffer, sizeof(AgpdSkillMasteryTemplate));
//
//		RefreshSkillPoint((ApBase *) pcsSelfCharacter);
//
//		// 이부분에서 마스터리 내용이 바뀌었다고 콜백을 불러줘야 한다. 그래야 UI 상에서 Refresh를 한다.
//		EnumCallback(AGCMSKILL_CB_ID_MASTERY_UPDATE, pcsSelfCharacter, NULL);
//	}
//	else
//	{
//		CopyMemory(pcsAttachSkillData->m_csMastery, pcsAttachSkillData->m_csMasteryBuffer, sizeof(AgpdSkillMastery) * AGPMSKILL_MAX_MASTERY);
//		CopyMemory(&pcsAttachSkillData->m_csMasteryTemplate, &pcsAttachSkillData->m_csMasteryTemplateBuffer, sizeof(AgpdSkillMasteryTemplate));
//
//		// character skill point 도 원래대로 돌려놔야 한다.
//		m_pcsAgpmFactors->SetValue(&pcsSelfCharacter->m_csFactor, pcsAttachSkillData->m_lCharSkillPointBackup, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);
//
//		RefreshSkillPoint((ApBase *) pcsSelfCharacter);
//
//		// 이부분에서 마스터리 내용이 바뀌었다고 콜백을 불러줘야 한다. 그래야 UI 상에서 Refresh를 한다.
//		EnumCallback(AGCMSKILL_CB_ID_MASTERY_UPDATE, pcsSelfCharacter, NULL);
//	}
//
//	m_lTempSkillID	= 0x7FFFFFFF;
//
//	for (i = 0; i < AGCMSKILL_MAX_TEMP_CREATED_SKILL; ++i)
//	{
//		if (m_lTempCreatedSkillID[i] != 0)
//		{
//			m_pcsAgpmSkill->RemoveSkill(m_lTempCreatedSkillID[i]);
//		}
//	}
//
//	//ResetUpdateMasteryFlag();
//
//#endif	__NEW_MASTERY__
//
//	return TRUE;
//}

/*
BOOL AgcmSkill::CBMasteryResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase *)		pData;
	INT32			*plBuffer		= (INT32 *)			pCustData;

	switch (plBuffer[1]) {
	case AGPMSKILL_MASTERY_RESULT_NEED_SELECT:
		{
			AgpdMasterySkillNode	*pcsMasterySkillNode = pThis->m_pcsAgpmSkill->GetNeedSelectSkillNode(pcsBase, plBuffer[0]);
			if (!pcsMasterySkillNode)
				// 선택할 필요가 없네요.. 뭔가 이상하네요.. 암튼 리턴~
				return FALSE;

			// 여기서 마스터리에 있는 스킬 트리중 스킬 하나를 선택하도록 한다.
			//
			//	pcsMasterySkillNode->m_szSkillName[AGPMSKILL_MAX_NUM_SKILL_NODE][AGPMSKILL_MAX_SKILL_NAME + 1]
			//
			//
		}
		break;

	case AGPMSKILL_MASTERY_SP_CHANGE_OK:
		{
			pThis->StopUpdateMastery(TRUE);
		}
		break;

	case AGPMSKILL_MASTERY_SP_CHANGE_FAIL:
		{
			pThis->StopUpdateMastery(FALSE);
		}
		break;
	}

	return TRUE;
}
*/

//#ifndef	__NEW_MASTERY__
//
//BOOL AgcmSkill::CBMasteryUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//	//INT32			lMasteryIndex	= *(INT32 *)		pCustData;
//
//	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	// 자신의 마스터리 업데이트인지 본다.
//	if (pcsBase->m_eType	== APBASE_TYPE_CHARACTER &&
//		pcsBase->m_lID		== pcsSelfCharacter->m_lID)
//	{
//		// 마스터리의 값들이 바뀌었다. 처리해준다. 그림을 다시 그려주던.. 뭘하던..
//		//
//		//
//		//
//	}
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::CBMasteryChangeResult(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgcmSkill			*pThis				= (AgcmSkill *)			pClass;
//	ApBase				*pcsBase			= (ApBase *)			pData;
//	INT8				cResult				= *(INT8 *)				pCustData;
//
//	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	// pcsBase 가 자신의 캐릭터랑 같은넘인지 확인한다.
//	if (pcsBase->m_eType	!= pcsSelfCharacter->m_eType ||
//		pcsBase->m_lID		!= pcsSelfCharacter->m_lID)
//		return FALSE;
//
//	switch (cResult) {
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE_SUCCESS:
//		{
//			// 오키.. 마스터리에 SP를 넣고 빼고 하는짓거리가 성공했다.
//			// 클라이언트에 알려주던.. 뭘하던.. 한다.
//			//
//			//
//			//
//
//			pThis->StopUpdateMastery(TRUE);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE_FAIL:
//		{
//			// 이런 이런 실패다. 음냐... 뭐냐... 음냐...
//			//
//			//
//
//			pThis->StopUpdateMastery(FALSE);
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	return TRUE;
//}
//
//#endif	//__NEW_MASTERY__
//
//BOOL AgcmSkill::CBMasterySpecializeResult(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgcmSkill		*pThis		= (AgcmSkill *)		pClass;
//	ApBase			*pcsBase	= (ApBase *)		pData;
//	INT8			cResult		= *(INT8 *)			pCustData;
//
//	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	// pcsBase 가 자신의 캐릭터랑 같은넘인지 확인한다.
//	if (pcsBase->m_eType	!= pcsSelfCharacter->m_eType ||
//		pcsBase->m_lID		!= pcsSelfCharacter->m_lID)
//		return FALSE;
//
//	switch (cResult) {
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_SUCCESS:
//		{
//			// 성공
//			//
//			//
//			//
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_FAIL:
//		{
//			// 실패
//			//
//			//
//			//
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	return TRUE;
//}
//
//#ifndef	__NEW_MASTERY__
//
//BOOL AgcmSkill::CBMasterySpecialize(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgcmSkill		*pThis		= (AgcmSkill *)		pClass;
//	ApBase			*pcsBase	= (ApBase *)		pData;
//	INT32			*plBuffer	= (INT32 *)			pCustData;
//
//	if (plBuffer[3])
//	{
//		INT16	nPacketLength	= 0;
//		PVOID	pvPacket		= pThis->m_pcsAgpmSkill->MakePacketMasterySpecialize(pcsBase, plBuffer[0], (eAgpmSkillMasterySpecializedType) plBuffer[1], plBuffer[2], &nPacketLength);
//
//		if (!pvPacket || nPacketLength < 1)
//			return FALSE;
//
//		BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength);
//
//		pThis->m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
//
//		return bSendResult;
//	}
//
//	return TRUE;
//}
//
//#endif	//__NEW_MASTERY__

/*
BOOL AgcmSkill::SendMasterySkillSelect(INT32 lMasteryIndex, CHAR *szSkillName)
{
	if (lMasteryIndex < 0 ||
		lMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
		!szSkillName ||
		!szSkillName[0])
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketSelectSkillMastery((ApBase *) m_pcsAgcmCharacter->GetSelfCharacter(), lMasteryIndex, szSkillName, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmSkill::SendAddSPToMastery(INT32 lMasteryIndex)
{
	if (lMasteryIndex < 0 ||
		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketAddSPToMastery((ApBase *) m_pcsAgcmCharacter->GetSelfCharacter(), lMasteryIndex, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmSkill::SendSubSPFromMastery(INT32 lMasteryIndex)
{
	if (lMasteryIndex < 0 ||
		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketSubSPFromMastery((ApBase *) m_pcsAgcmCharacter->GetSelfCharacter(), lMasteryIndex, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}
*/

//#ifndef	__NEW_MASTERY__
//BOOL AgcmSkill::SendChangeMastery()
//{
//	AgpdCharacter	*pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketMasteryActiveNode((ApBase *) pcsSelfCharacter, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);
//
//	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//#endif	//__NEW_MASTERY__
//
//BOOL AgcmSkill::SendMasteryRollback()
//{
//	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketMasteryRollback((ApBase *) pcsSelfCharacter, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);
//
//	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//#ifdef	__NEW_MASTERY__
//
//BOOL AgcmSkill::SendAddSPToMastery(INT32 lMastery, INT32 *plSelectSkillTID, INT32 lArraySize)
//{
//	if (lMastery < 0 ||
//		lMastery >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	UINT32	ulCurrentClock	= GetClockCount();
//
//	if (m_ulNextSendAddSPTime > ulCurrentClock)
//		return TRUE;
//
//	m_ulNextSendAddSPTime	= ulCurrentClock + AGCMSKILL_INTERVAL_TIME_FOR_ADD_SP;
//
//	INT32	lSkillPoint	= m_pcsAgpmCharacter->GetSkillPoint(pcsSelfCharacter);
//	if (lSkillPoint < 1)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData(pcsSelfCharacter);
//	if (!pcsAttachData)
//		return FALSE;
//
//	INT32	lLevel		= m_pcsAgpmCharacter->GetLevel(pcsSelfCharacter);
//	if (lLevel <= (INT32) pcsAttachData->m_csMastery[lMastery].m_ucTotalInputSP)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketAddSPToMastery((ApBase *) pcsSelfCharacter, lMastery, plSelectSkillTID, lArraySize, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);
//
//	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgcmSkill::CBAddSPToMasteryResult(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//	PVOID			*ppvBuffer		= (PVOID *)			pCustData;
//
//	INT32			lResult			= (INT32)			ppvBuffer[0];
//	INT32			*plSelectTID	= (INT32 *)			ppvBuffer[1];
//	INT32			lArraySize		= (INT32)			ppvBuffer[2];
//	INT32			lMasteryIndex	= (INT32)			ppvBuffer[3];
//
//	switch ((AgpmSkillAddSPToMasteryResult) lResult) {
//	case AGPMSKILL_MASTERY_RESULT_OK:
//		{
//			pThis->StopUpdateMastery(TRUE);
//		}
//		break;
//
//	case AGPMSKILL_MASTERY_RESULT_SP_FULL:
//		{
//		}
//		break;
//
//	case AGPMSKILL_MASTERY_RESULT_NEED_SELECT:
//		{
//		}
//		break;
//
//	case AGPMSKILL_MASTERY_RESULT_FAIL:
//	default:
//		{
//			pThis->StopUpdateMastery(FALSE);
//		}
//		break;
//	};
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::CBMasteryNodeUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//
//	pThis->RefreshNeedSkillPoint(pcsBase);
//
//	return TRUE;
//}
//
//#endif	//__NEW_MASTERY__
//
//BOOL AgcmSkill::RefreshSkillPoint(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//#ifdef	__NEW_MASTERY__
//
//#else
//
//	for (int i = 0; i < pcsAttachData->m_csMasteryTemplate.m_lNumMastery; ++i)
//	{
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				for (int l = 0; l < AGPMSKILL_MAX_NUM_SKILL_NODE; ++l)
//				{
//					AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkill(pcsBase, pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_szSkillName[l]);
//					if (pcsSkill)
//					{
//						m_pcsAgpmSkill->UpdateSkillPoint(pcsSkill, pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_lInputSP[l] - m_pcsAgpmSkill->GetSkillPoint(pcsSkill));
//					}
//					else
//					{
//						AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_szSkillName[l]);
//						if (pcsSkillTemplate)
//						{
//							SetBaseSPForActive(pcsSkillTemplate, pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_lNeedSP[l]);
//						}
//					}
//				}
//			}
//		}
//	}
//
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::SetBaseSPForActive(AgpdSkillTemplate *pcsSkillTemplate, INT32 lSP)
//{
//	if (!pcsSkillTemplate)
//		return FALSE;
//
//	CHAR	szLeftString[8];
//	ZeroMemory(szLeftString, sizeof(CHAR) * 8);
//	sprintf(szLeftString, "%d", lSP);
//
//	pcsSkillTemplate->m_pcsGridItem->SetRightBottomString(szLeftString);
//
//	return TRUE;
//}