/******************************************************************************
Module:  AgpcParty.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 13
******************************************************************************/

#include "AgcmParty.h"

AgcmParty::AgcmParty()
{
	SetModuleName("AgcmParty");

	SetPacketType(AGCMPARTY_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,		// operation
							AUTYPE_INT32,		1,		// operator id
							AUTYPE_INT32,		1,		// target id
							AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,
							AUTYPE_END,			0
							);
}

AgcmParty::~AgcmParty()
{
}

BOOL AgcmParty::OnInit()
{
	return TRUE;
}

BOOL AgcmParty::OnAddModule()
{
	m_pagpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pagpmParty			= (AgpmParty *)			GetModule("AgpmParty");
	m_pagcmCharacter		= (AgcmCharacter*)		GetModule("AgcmCharacter");
	m_pAgpmFactors			= (AgpmFactors*)		GetModule("AgpmFactors");

	if (!m_pagpmCharacter || !m_pagpmParty || !m_pagcmCharacter)
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackCheckRemoveChar(CBCheckRemoveChar, this))
		return FALSE;

	if (!m_pagpmParty->SetCBRemoveMember(CBRemoveMember, this))
		return FALSE;
	if (!m_pagpmParty->SetCBCheckDestroyParty(CBCheckDestroyParty, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmParty::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgcmParty::OnDestroy()
{
	return TRUE;
}

BOOL AgcmParty::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !nSize)
		return FALSE;

	INT8		cOperation = (-1);
	INT32		lOperatorID = AP_INVALID_CID;
	INT32		lTargetID = AP_INVALID_CID;
	CHAR		*szTargetID	= NULL;
	PVOID		pvBuffer[2];

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lOperatorID,
						&lTargetID,
						&szTargetID);

	switch (cOperation)	{
	case AGCMPARTY_PACKET_OPERATION_INVITE:
		{
			pvBuffer[0] = (PVOID) lTargetID;
			pvBuffer[1] = (PVOID) szTargetID;
			//SendPacketAccept(lTargetID, lOperatorID);
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE, &lOperatorID, pvBuffer);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_REJECT:
		{
			//MessageBox(NULL, "파티 초대가 거부되었습니다.", "AgcmParty", 1);
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_REJECT, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_INVITE_ACCEPT:
		{
			//MessageBox(NULL, "파티 초대 성공", "AgcmParty", 1);
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_ACCEPT, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_LEAVE:
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_LEAVE, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_BANISH:
		{
			//MessageBox(NULL, "파티에서 쫒겨났습니다.", "AgcmParty", 1);
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_BANISH, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER:
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_PARTY_MEMBER:
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_ALREADY_PARTY_MEMBER, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_PARTY_MEMBER_IS_FULL:
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_PARTY_MEMBER_IS_FULL, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_NOT_LEADER:
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_NOT_LEADER, &lOperatorID, &lTargetID);
		}
		break;

	case AGCMPARTY_PACKET_OPERATION_FAILED:
		{
			//MessageBox(NULL, "파티 오퍼레이션이 실패했습니다.", "AgcmParty", 1);
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_OPERATION_FAILED, &lOperatorID, &lTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_NO_LOGIN_MEMBER:		//	해당 플레이어가 로그인 하지 않았다.
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_NO_LOGIN_MEMBER, &lOperatorID, szTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_DELEGATION_LEADER :					//	파티장 위임에 성공했다.
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_DELEGATION_LEADER, &lOperatorID, &lTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_DELEGATION_LEADER_FAILED :			//	파티장 위임에 실패했다.
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_DELEGATION_LEADER_FAILED, &lOperatorID, &lTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_LEVEL_DIFF :			//	파티 초대할 멤버레벨차이가 너무 난다.
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_LEVEL_DIFF, &lOperatorID, &lTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_REFUSE :
		{
			EnumCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_REFUSE, &lOperatorID, szTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_REQUEST_RECALL:
		{
			OnReceiveRequestRecall(lOperatorID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_ACCEPT_RECALL:
		{
			OnReceiveAcceptRecall(lOperatorID, lTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_REJECT_RECALL:
		{
			OnReceiveRejectRecall(lOperatorID, lTargetID);
		}
		break;
//	2005.10.26. By SungHoon
	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_OPERATOR:
		{
			EnumCallback(AGCMPARTY_CB_ID_INVITE_FAILED_MURDERER_OPERATOR, &lOperatorID, NULL);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_TARGET:
		{
			EnumCallback(AGCMPARTY_CB_ID_INVITE_FAILED_MURDERER_TARGET, &lOperatorID, szTargetID);
		}
		break;
	case AGCMPARTY_PACKET_OPERATION_LEAVE_BY_MURDERER:
		{
			EnumCallback(AGCMPARTY_CB_ID_LEAVE_BY_MURDERER, &lOperatorID, NULL);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmParty::OnReceiveRequestRecall(INT32 lOperator)
{
	AgpdCharacter	*pcsOperator	= m_pagpmCharacter->GetCharacter(lOperator);
	if (!pcsOperator)
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= m_pagcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdPartyADChar	*pcsOperatorAttachData	= m_pagpmParty->GetADCharacter(pcsOperator);
	AgpdPartyADChar	*pcsSelfAttachData		= m_pagpmParty->GetADCharacter(pcsSelfCharacter);

	if (pcsOperatorAttachData->lPID != pcsSelfAttachData->lPID)
		return FALSE;

	return EnumCallback(AGCMPARTY_CB_ID_REQUEST_RECALL, pcsOperator, NULL);
}

BOOL AgcmParty::OnReceiveAcceptRecall(INT32 lOperator, INT32 lTarget)
{
	AgpdCharacter	*pcsOperator	= m_pagpmCharacter->GetCharacter(lOperator);
	if (!pcsOperator)
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= m_pagcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	return EnumCallback(AGCMPARTY_CB_ID_ACCEPT_RECALL, pcsOperator, NULL);
}

BOOL AgcmParty::OnReceiveRejectRecall(INT32 lOperator, INT32 lTarget)
{
	AgpdCharacter	*pcsOperator	= m_pagpmCharacter->GetCharacter(lOperator);
	if (!pcsOperator)
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= m_pagcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	return EnumCallback(AGCMPARTY_CB_ID_REJECT_RECALL, pcsOperator, NULL);
}

BOOL AgcmParty::SendPacketInvite(INT32 lOperator, INT32 lTarget)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketInvite(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketInvite() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmParty::SendPacketReject(INT32 lOperator, INT32 lTarget)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketReject(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketReject() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmParty::SendPacketAccept(INT32 lOperator, INT32 lTarget)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketAccept(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketAccept() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmParty::SendPacketLeave(INT32 lOperator)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketLeave(lOperator, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketLeave() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmParty::SendPacketBanish(INT32 lOperator, INT32 lTarget)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketBanish(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketBanish() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmParty::SendPacketFailed(INT32 lOperator)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketFailed(lOperator, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketFailed() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmParty::SendPacketUpdateExpType(AgpdParty *pcsParty, INT32 lMemberID )
{
	if (!pcsParty)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmParty->MakePacketExpDivision(pcsParty, lMemberID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

/*
	2005.05.17 By SungHoon
	파티 경험치 분배 방식 변경패킷을 서버로 전송한다.
*/
BOOL AgcmParty::SendPacketUpdateExpType(INT32 lPartyID, INT32 lMemberID,  AgpmPartyCalcExpType eCalExpType)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmParty->MakePacketExpDivision(lPartyID, lMemberID, eCalExpType, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

/*
	2005.04.18	By SungHoon
	아이디로 다른 플레이어를 파티에 초대한다.
*/
BOOL AgcmParty::SendPacketInviteByCharID(INT32 lOperator, CHAR *szTarget)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketInviteByCharID(lOperator, szTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		OutputDebugString("AgsmParty::SendPacketInviteByCharID() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

/*
	2005.04.19	By SungHoon
	아이템 분배 방식 변경을 서버로 요청한다.
*/
BOOL AgcmParty::SendPacketUpdateItemDivision(INT32 lPartyID, INT32 lMemberID,  AgpmPartyOptionDivisionItem eDivisionItem)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmParty->MakePacketItemDivision(lPartyID, lMemberID, eDivisionItem, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

/*
	2005.05.02	By SungHoon
	파티장 위임 패킷을 서버로 보낸다.
*/
BOOL AgcmParty::SendPacketDelegationLeader(INT32 lOperator, INT32 lTarget )
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketDelegationLeader(lOperator, lTarget, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmParty::SendPacketAcceptRecall(INT32 lTarget )
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketAcceptRecall(lTarget, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmParty::SendPacketRejectRecall(INT32 lTarget )
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketRejectRecall(lTarget, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

PVOID AgcmParty::MakePacketInvite(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_INVITE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgcmParty::MakePacketReject(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_REJECT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgcmParty::MakePacketAccept(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_INVITE_ACCEPT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgcmParty::MakePacketLeave(INT32 lOperator, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_LEAVE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												NULL);

	return pvPacket;
}

PVOID AgcmParty::MakePacketBanish(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_BANISH;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgcmParty::MakePacketFailed(INT32 lOperator, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_FAILED;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												NULL);

	return pvPacket;
}

/*
	2005.04.15	By SungHoon
	아이디로 다른 플레이어를 파티에 초대하는 패킷을 만든다.
*/
PVOID AgcmParty::MakePacketInviteByCharID(INT32 lOperator, CHAR *szTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_INVITE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												szTarget);
	return pvPacket;
}

PVOID AgcmParty::MakePacketDelegationLeader(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_DELEGATION_LEADER;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);
	return pvPacket;
}

PVOID AgcmParty::MakePacketAcceptRecall(INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_ACCEPT_RECALL;

	INT32	lSelfCID	= m_pagcmCharacter->GetSelfCID();

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lSelfCID,
												&lTarget,
												NULL);
}

PVOID AgcmParty::MakePacketRejectRecall(INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGCMPARTY_PACKET_OPERATION_REJECT_RECALL;

	INT32	lSelfCID	= m_pagcmCharacter->GetSelfCID();

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lSelfCID,
												&lTarget,
												NULL);
}

BOOL AgcmParty::CBCheckRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmParty	*pThis		= (AgcmParty *)	pClass;
	INT32		*lCID		= (INT32 *)		pData;
	BOOL		*pbRemove	= (BOOL *)		pCustData;

	//@{ Jaewon 20050826
	// If it's already FALSE, any further processings are not necessary.
	if(*pbRemove == FALSE)
		return TRUE;
	//@} Jaewon

	if (*lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter	*pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(*lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdPartyADChar	*pcsPartyADChar	= pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	if (pcsPartyADChar && pcsPartyADChar->lPID != AP_INVALID_PARTYID)
		*pbRemove = FALSE;

	return TRUE;
}

BOOL AgcmParty::CBRemoveMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmParty		*pThis				= (AgcmParty *)		pClass;
	AgpdParty		*pcsParty			= (AgpdParty *)		pData;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pCustData;

	if (!pcsCharacter->m_bIsAddMap)
	{
		pThis->m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
	}
	*/

	return TRUE;
}

BOOL AgcmParty::CBCheckDestroyParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmParty		*pThis				= (AgcmParty *)		pClass;
	INT32			lMemberID			= (INT32)			pData;
	BOOL			*pbIsDestroyParty	= (BOOL *)			pCustData;

	if (lMemberID == pThis->m_pagcmCharacter->GetSelfCID())
		*pbIsDestroyParty	= TRUE;

	return TRUE;
}

BOOL AgcmParty::SetCallbackReceiveInvite(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_REJECT, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveInviteAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_ACCEPT, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_LEAVE, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveBanish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_BANISH, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveInviteFailedAlreadyOtherPartyMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveInviteFailedAlreadyPartyMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_ALREADY_PARTY_MEMBER, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveInviteFailedPartyMemberIsFull(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_PARTY_MEMBER_IS_FULL, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveInviteFailedNotLeader(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_NOT_LEADER, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackReceiveOperationFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_OPERATION_FAILED, pfCallback, pClass);
}

/*
	2005.04.18	By SungHoon
	파티 초대시 해당 플레이어가 존재하지 않았을 경우 불릴 CallBack 함수 등록
*/
BOOL AgcmParty::SetCallbackReceiveInviteFailedNoLoginMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_NO_LOGIN_MEMBER, pfCallback, pClass);
}

/*
	2005.05.02	By SungHoon
	파티장 위임에 성공했을 경우 불릴 CallBack 함수 등록
*/
BOOL AgcmParty::SetCallbackReceiveDelegationLeader(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_DELEGATION_LEADER, pfCallback, pClass);
}

/*
	2005.05.02 By SungHoon
	파티장 위임에 실패했을 경우 불릴 CallBack 함수 등록
*/
BOOL AgcmParty::SetCallbackReceiveDelegationLeaderFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_DELEGATION_LEADER_FAILED, pfCallback, pClass);
}

/*
	2005.05.27 By SungHoon
	파티 초대한 멤버의 레벨 차이가 너무 날 경우 불리워질 함수 등록
*/
BOOL AgcmParty::SetCallbackReceiveInviteFailedLevelDiff(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_LEVEL_DIFF, pfCallback, pClass);
}

/*
	2005.05.27 By SungHoon
	파티 초대한 멤버의 레벨 차이가 너무 날 경우 불리워질 함수 등록
*/
BOOL AgcmParty::SetCallbackReceiveInviteFailedRefuse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_REFUSE, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackRequestRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_REQUEST_RECALL, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackAcceptRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_ACCEPT_RECALL, pfCallback, pClass);
}

BOOL AgcmParty::SetCallbackRejectRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_REJECT_RECALL, pfCallback, pClass);
}

/*
	2005.10.26. By SungHoon
*/
BOOL AgcmParty::SetCallbackReceiveInviteFailedMurdererOperator(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_INVITE_FAILED_MURDERER_OPERATOR, pfCallback, pClass);
}

/*
	2005.10.26. By SungHoon
*/
BOOL AgcmParty::SetCallbackReceiveInviteFailedMurdererTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_INVITE_FAILED_MURDERER_TARGET, pfCallback, pClass);
}

/*
	2005.10.26. By SungHoon
*/
BOOL AgcmParty::SetCallbackReceiveLeaveByMurderer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMPARTY_CB_ID_LEAVE_BY_MURDERER, pfCallback, pClass);
}