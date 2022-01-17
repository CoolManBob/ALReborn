/******************************************************************************
Module:  AgpcParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 13
******************************************************************************/

#if !defined(__AGCMPARTY_H__)
#define __AGCMPARTY_H__

#include "AgcModule.h"
#include "ApBase.h"

#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmParty.h"
#include "AgpmFactors.h"

#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmPartyD" )
#else
#pragma comment ( lib , "AgcmParty" )
#endif
#endif

#define		AGCMPARTY_PARTY_WINDOW_NUM		8	

typedef enum _eAgcmPartyPacketOperation {
	AGCMPARTY_PACKET_OPERATION_INVITE			= 0,
	AGCMPARTY_PACKET_OPERATION_REJECT,
	AGCMPARTY_PACKET_OPERATION_INVITE_ACCEPT,
	AGCMPARTY_PACKET_OPERATION_LEAVE,
	AGCMPARTY_PACKET_OPERATION_BANISH,
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_PARTY_MEMBER,
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_PARTY_MEMBER_IS_FULL,
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_NOT_LEADER,
	AGCMPARTY_PACKET_OPERATION_FAILED,
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_NO_LOGIN_MEMBER,			// 2005.04.21. By SungHoon
	AGCMPARTY_PACKET_OPERATION_DELEGATION_LEADER,					// 2005.05.02. By SungHoon
	AGCMPARTY_PACKET_OPERATION_DELEGATION_LEADER_FAILED,			// 2005.05.02. By SungHoon
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_LEVEL_DIFF,			// 2005.05.27. By SungHoon
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_REFUSE,			// 2005.05.31. By SungHoon

	AGCMPARTY_PACKET_OPERATION_REQUEST_RECALL,
	AGCMPARTY_PACKET_OPERATION_ACCEPT_RECALL,
	AGCMPARTY_PACKET_OPERATION_REJECT_RECALL,

	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_OPERATOR,			// 2005.10.24. By SungHoon
	AGCMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_TARGET,			// 2005.10.24. By SungHoon
	AGCMPARTY_PACKET_OPERATION_LEAVE_BY_MURDERER,			// 2005.10.24. By SungHoon

} eAgcmPartyPacketOperation;

typedef enum _AgcmPartyCBID {
	AGCMPARTY_CB_ID_RECEIVE_INVITE				= 0,
	AGCMPARTY_CB_ID_RECEIVE_REJECT,
	AGCMPARTY_CB_ID_RECEIVE_INVITE_ACCEPT,
	AGCMPARTY_CB_ID_RECEIVE_LEAVE,
	AGCMPARTY_CB_ID_RECEIVE_BANISH,
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_ALREADY_PARTY_MEMBER,
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_PARTY_MEMBER_IS_FULL,
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_NOT_LEADER,
	AGCMPARTY_CB_ID_RECEIVE_OPERATION_FAILED,
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_NO_LOGIN_MEMBER,				// 2005.04.21. By SungHoon
	AGCMPARTY_CB_ID_RECEIVE_DELEGATION_LEADER,					// 2005.05.02. By SungHoon
	AGCMPARTY_CB_ID_RECEIVE_DELEGATION_LEADER_FAILED,			// 2005.05.02. By SungHoon
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_LEVEL_DIFF,			// 2005.05.27. By SungHoon
	AGCMPARTY_CB_ID_RECEIVE_INVITE_FAILED_REFUSE,				// 2005.06.01. By SungHoon

	AGCMPARTY_CB_ID_REQUEST_RECALL,
	AGCMPARTY_CB_ID_ACCEPT_RECALL,
	AGCMPARTY_CB_ID_REJECT_RECALL,

	AGCMPARTY_CB_ID_INVITE_FAILED_MURDERER_OPERATOR,			// 2005.10.24. By SungHoon
	AGCMPARTY_CB_ID_INVITE_FAILED_MURDERER_TARGET,			// 2005.10.24. By SungHoon
	AGCMPARTY_CB_ID_LEAVE_BY_MURDERER,			// 2005.10.24. By SungHoon

} AgcmPartyCBID;

class AgcmParty : public AgcModule
{
private:
	AgpmCharacter		*m_pagpmCharacter;
	AgpmParty			*m_pagpmParty;
	AgcmCharacter		*m_pagcmCharacter;
	AgpmFactors			*m_pAgpmFactors;

public:
	AuPacket			m_csPacket;

public:
	AgcmParty();
	virtual ~AgcmParty();

	BOOL                OnInit();
	BOOL                OnAddModule();
	BOOL                OnIdle(UINT32 ulClockCount);	
	BOOL                OnDestroy();

	BOOL				SendPacketInvite(INT32 lOperator, INT32 lTarget);
	BOOL				SendPacketReject(INT32 lOperator, INT32 lTarget);
	BOOL				SendPacketAccept(INT32 lOperator, INT32 lTarget);
	BOOL				SendPacketLeave(INT32 lOperator);
	BOOL				SendPacketBanish(INT32 lOperator, INT32 lTarget);
	BOOL				SendPacketFailed(INT32 lOperator);
	BOOL				SendPacketInviteByCharID(INT32 lOperator, CHAR *szTarget);	//	2005.04.18	By SungHoon
	BOOL				SendPacketUpdateExpType(AgpdParty *pcsParty, INT32 lMemberID);		//	005.05.17 By SungHoon
	BOOL				SendPacketUpdateExpType(INT32 lPartyID, INT32 lMemberID,  AgpmPartyCalcExpType eCalcExpType);		//	2005.05.17. By SungHoon
	BOOL				SendPacketUpdateItemDivision(INT32 lPartyID, INT32 lMemberID, AgpmPartyOptionDivisionItem eDivisionItem);	//	2005.04.19	By SungHoon
	BOOL				SendPacketDelegationLeader(INT32 lOperator, INT32 lTarget);	//	2005.05.02 By SungHoon

	BOOL				SendPacketAcceptRecall(INT32 lTarget);
	BOOL				SendPacketRejectRecall(INT32 lTarget);

	PVOID				MakePacketInvite(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID				MakePacketReject(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID				MakePacketAccept(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID				MakePacketLeave(INT32 lOperator, INT16 *pnPacketLength);
	PVOID				MakePacketBanish(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID				MakePacketFailed(INT32 lOperator, INT16 *pnPacketLength);
//	2005.04.15	Start By SungHoon
//	문자열 아이디로 파티 멤버 초대
	PVOID				MakePacketInviteByCharID(INT32 lOperator, CHAR *szTarget, INT16 *pnPacketLength);
//	2005.04.15	Finish By SungHoon

//	문자열 아이디로 파티 멤버 초대
	PVOID				MakePacketDelegationLeader(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);	//	2005.05.02

	PVOID				MakePacketAcceptRecall(INT32 lTarget, INT16 *pnPacketLength);
	PVOID				MakePacketRejectRecall(INT32 lTarget, INT16 *pnPacketLength);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				OnReceiveRequestRecall(INT32 lOperator);
	BOOL				OnReceiveAcceptRecall(INT32 lOperator, INT32 lTarget);
	BOOL				OnReceiveRejectRecall(INT32 lOperator, INT32 lTarget);

	static BOOL			CBCheckRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBRemoveMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCheckDestroyParty(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetCallbackReceiveInvite(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveBanish(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteFailedAlreadyOtherPartyMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteFailedAlreadyPartyMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteFailedPartyMemberIsFull(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteFailedNotLeader(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveOperationFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteFailedNoLoginMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 2005.04.21. By SungHoon

	BOOL				SetCallbackReceiveDelegationLeader(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 2005.05.02. By SungHoon
	BOOL				SetCallbackReceiveDelegationLeaderFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 2005.05.02. By SungHoon
	BOOL				SetCallbackReceiveInviteFailedLevelDiff(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.05.27 By SungHoon
	BOOL				SetCallbackReceiveInviteFailedRefuse(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.06.01 By SungHoon
	
	BOOL				SetCallbackRequestRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackAcceptRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackRejectRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackReceiveInviteFailedMurdererOperator(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveInviteFailedMurdererTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceiveLeaveByMurderer(ApModuleDefaultCallBack pfCallback, PVOID pClass);

};

#endif //__AGCMPARTY_H__
