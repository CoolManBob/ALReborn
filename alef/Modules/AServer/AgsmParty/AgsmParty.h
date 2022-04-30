/******************************************************************************
Module:  AgsmParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 07
******************************************************************************/

#if !defined(__AGSMPARTY_H__)
#define __AGSMPARTY_H__

#include "ApBase.h"
#include "AsDefine.h"
#include "AgsEngine.h"

#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmParty.h"
#include "AgpmSiegeWar.h"
#include "AgpmDropItem2.h"
#include "AgpmItemConvert.h"

#include "AgsmAOIFilter.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmFactors.h"
#include "AgsmCharacter.h"
#include "AgsmZoning.h"

#include "AgsdParty.h"

#include "AuPacket.h"
#include "AuGenerateID.h"


#define	AGSMPARTY_UPDATE_PARTY_FACTOR	1


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmPartyD" )
#else
#pragma comment ( lib , "AgsmParty" )
#endif
#endif

typedef enum _eAgsmPartyPacketOperation {
	AGSMPARTY_PACKET_OPERATION_INVITE			= 0,
	AGSMPARTY_PACKET_OPERATION_REJECT,
	AGSMPARTY_PACKET_OPERATION_INVITE_ACCEPT,
	AGSMPARTY_PACKET_OPERATION_LEAVE,
	AGSMPARTY_PACKET_OPERATION_BANISH,
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_PARTY_MEMBER,
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_PARTY_MEMBER_IS_FULL,
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_NOT_LEADER,
	AGSMPARTY_PACKET_OPERATION_FAILED,
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_NO_LOGIN_MEMBER,		// 2005.04.21. By SungHoon
	AGSMPARTY_PACKET_OPERATION_DELEGATION_LEADER,					// 2005.05.02. By SungHoon
	AGSMPARTY_PACKET_OPERATION_DELEGATION_LEADER_FAILED,			// 2005.05.02. By SungHoon
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_LEVEL_DIFF,			// 2005.05.27. By SungHoon
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_REFUSE,			// 2005.05.31. By SungHoon

	AGSMPARTY_PACKET_OPERATION_REQUEST_RECALL,
	AGSMPARTY_PACKET_OPERATION_ACCEPT_RECALL,
	AGSMPARTY_PACKET_OPERATION_REJECT_RECALL,
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_OPERATOR,			// 2005.10.24. By SungHoon
	AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_TARGET,			// 2005.10.24. By SungHoon
	AGSMPARTY_PACKET_OPERATION_LEAVE_BY_MURDERER,			// 2005.10.24. By SungHoon
} eAgsmPartyPacketOperation;

const int	AGSMPARTY_PROCESS_IDLE_INTERVAL				= 6000;

typedef enum _AgsmPartyCBID {
	AGSMPARTY_CB_USE_RECALL						= 0,
	AGSMPARTY_CB_BUDDY_CHECK,
} AgsmPartyCBID;

class AgpmBattleGround;
class AgsmSkill;
class AgpmSummons;
class AgsmSystemMessage;
class AgpmEpicZone;

class AgsmParty : public AgsModule {
private:
	ApmMap*				m_papmMap;
	AgpmFactors*		m_pagpmFactors;
	AgpmCharacter*		m_pagpmCharacter;
	AgpmParty*			m_pagpmParty;
	AgpmSiegeWar*		m_pagpmSiegeWar;
	AgpmDropItem2*		m_pagpmDropItem2;
	AgpmItemConvert*	m_pagpmItemConvert;
	ApmEventManager*	m_papmEventManager;

	AgsmAOIFilter*		m_pagsmAOIFilter;
	AgsmServerManager*	m_pAgsmServerManager;
	AgsmFactors*		m_pagsmFactors;
	AgsmCharacter*		m_pagsmCharacter;
	AgsmZoning*			m_pagsmZoning;
	AgpmBattleGround*	m_pagpmBattleGround;
	AgsmSkill*			m_pagsmSkill;
	AgpmSummons*		m_pagpmSummons;
	AgsmSystemMessage*	m_pagsmSystemMessage;

	AgpmEpicZone*		m_pagpmEpicZone;

	AuPacket			m_csPacket;
	AuGenerateID		m_csGenerateID;

	//INT16				m_nIndexADParty;

	UINT32				m_ulNextProcessTimeMSec;
public:
//	2005.04.19	Start By SungHoon
	BOOL	InvitePartyMember(  UINT32 ulNID, INT32 lOperatorID, INT32 lTargetID, CHAR *szTargetID );
//	2005.04.19	Finish By SungHoon

	AgsmParty();
	~AgsmParty();

	BOOL	OnAddModule();

	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle2(UINT32 ulClockCount);

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL	OnReceiveAcceptRecall(INT32 lOperator, INT32 lTarget);
	BOOL	OnReceiveRejectRecall(INT32 lOperator, INT32 lTarget);

	BOOL	InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize = 0);

	BOOL	IsAddMember(AgpdCharacter *pcsPartyLeader, AgpdCharacter *pcsMember);
	BOOL	IsAddMember(AgpdParty *pcsParty, AgpdCharacter *pcsMember);

	BOOL	SendPacketInvite(INT32 lOperator, INT32 lTarget, CHAR *szTargetID, DPNID dpnid);
	BOOL	SendPacketReject(INT32 lOperator, INT32 lTarget, DPNID dpnid);
	BOOL	SendPacketAccept(INT32 lOperator, INT32 lTarget, DPNID dpnid);
	BOOL	SendPacketLeave(INT32 lOperator, DPNID dpnid);
	BOOL	SendPacketBanish(INT32 lOperator, INT32 lTarget, DPNID dpnid);
	BOOL	SendPacketInviteFailed(INT32 lOperator, INT32 lTarget, eAgsmPartyPacketOperation eFailReason, DPNID dpnid);
	BOOL	SendPacketInviteFailed(INT32 lOperator, CHAR *szTargetID, eAgsmPartyPacketOperation eFailReason, DPNID dpnid);
	BOOL	SendPacketFailed(INT32 lOperator, DPNID dpnid);
	BOOL	SendDelegationLeaderFailed(INT32 lOperator, DPNID dpnid);		//	2005.05.02	By SungHoon

	BOOL	SendPacketRequestRecall(INT32 lOperator, UINT32 ulNID);
	BOOL	SendPacketAcceptRecall(INT32 lOperator, INT32 lTargetID, UINT32 ulNID);
	BOOL	SendPacketRejectRecall(INT32 lOperator, INT32 lTargetID, UINT32 ulNID);

	PVOID	MakePacketInvite(INT32 lOperator, INT32 lTarget, CHAR *szTargetID, INT16 *pnPacketLength);
	PVOID	MakePacketReject(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID	MakePacketAccept(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID	MakePacketLeave(INT32 lOperator, INT16 *pnPacketLength);
	PVOID	MakePacketBanish(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength);
	PVOID	MakePacketInviteFailed(INT32 lOperator, CHAR *szTarget, eAgsmPartyPacketOperation eFailReason, INT16 *pnPacketLength);
	PVOID	MakePacketInviteFailed(INT32 lOperator, INT32 lTarget, eAgsmPartyPacketOperation eFailReason, INT16 *pnPacketLength);
	PVOID	MakePacketFailed(INT32 lOperator, INT16 *pnPacketLength);

	PVOID	MakePacketDelegationLeader(INT32 lOperator, INT32 lTargetID, INT16 *pnPacketLength);	//	2005.05.02	By SungHoon
	PVOID	MakePacketDelegationLeaderFailed(INT32 lOperator, INT16 *pnPacketLength);	//	2005.05.02	By SungHoon

	PVOID	MakePacketRequestRecall(INT32 lOperator, INT16 *pnPacketLength);
	PVOID	MakePacketAcceptRecall(INT32 lOperator, INT32 lTargetID, INT16 *pnPacketLength);
	PVOID	MakePacketRejectRecall(INT32 lOperator, INT32 lTargetID, INT16 *pnPacketLength);


	BOOL	SendPartyInfo(AgpdParty *pcsParty, DPNID dpnid);
	BOOL	SendPartyInfo(AgpdParty *pcsParty);

	BOOL	SendPartyMemberInfo(AgpdParty *pcsParty, DPNID dpnid, INT32 lExceptMemberID);

	BOOL	SendPartyRemove(AgpdParty *pcsParty);
	BOOL	SendMemberAdd(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, DPNID dpnid);
	BOOL	SendMemberRemove(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, DPNID dpnid);

	BOOL	SyncMemberHP(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, BOOL bIsMemberAdd);
	BOOL	SyncMemberHPMax(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, BOOL bIsMemberAdd);

	BOOL	SendPartyBonusStats(AgpdCharacter *pcsCharacter);

	INT32	GetCurrentGetItemPartyMember(ApBase *pcsFirstLooter, ApBase *pcsDropbase, BOOL bTemp = FALSE );		//	2005.04.21.		By SungHoon
	//AgsdParty*	GetADParty(AgpdParty *pcsParty);

	// AgpmFactors
	static BOOL CBUpdateFactorParty(PVOID pData, PVOID pClass, PVOID pCustData);

	// AgpmParty
	static BOOL CBCreateParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemovePartyID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBChangeLeader(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	
	static BOOL CBCharLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);

//	2005.05.02	Start By SungHoon
	BOOL	DelegationLeader(UINT32 ulNID, INT32 lOperator, INT32 lTarget);
//	2005.05.02	Start By SungHoon

//	2005.04.19	Start By SungHoon
	static BOOL	CBUpdateItemDivision(PVOID pData, PVOID pClass, PVOID pCustData);
//	2005.04.19	Finish By SungHoon

	// AgpmCharacter
	static BOOL CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// ApmMap
	/*
	static BOOL CBMoveSector(PVOID pData, PVOID pClass, PVOID pCustData);
	*/
	static BOOL CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSyncCharacterActions(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSyncCharacterRemove(PVOID pData, PVOID pClass, PVOID pCustData);

	// AgsmCharacter
	//static BOOL CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL CBUpdateHP(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL CBUpdateMaxHP(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBZoningPassControl(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateControlServer(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBReCalcFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBResetEffectFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateExpType(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdatePartyFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBTransform(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetBonusOfPartyMember(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SendPacketToParty(INT32 lPartyID, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority);
	BOOL	SendPacketToParty(AgpdParty *pcsParty, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority);
	BOOL	SendPacketToParty(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority);

	BOOL	SendPacketToPartyExceptOne(AgpdParty *pcsParty, PVOID pvPacket, INT16 nPacketLength, INT32 lExceptMemberID, PACKET_PRIORITY ePriority);

	//BOOL	SendPacketToNearPartyMember(INT32 lPartyID, PVOID pvPacket, INT16 nPacketLength);
	//BOOL	SendPacketToNearPartyMember(AgpdParty *pcsParty, PVOID pvPacket, INT16 nPacketLength);
	BOOL	SendPacketToNearPartyMember(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority);

	//BOOL	SendPacketToFarPartyMember(INT32 lPartyID, PVOID pvPacket, INT16 nPacketLength);
	//BOOL	SendPacketToFarPartyMember(AgpdParty *pcsParty, PVOID pvPacket, INT16 nPacketLength);
	BOOL	SendPacketToFarPartyMember(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority);

	/*
	static BOOL	ConAgsdParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	DesAgsdParty(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	INT16	GetPartyMemberInServer(AgpdParty *pcsParty, INT32 lServerID);

	INT32	CheckValidDistance(AgpdCharacter *pcsPartyMember, AgpdCharacter *pcsBaseCharacter);

	INT32	GetNearMember(AgpdParty *pcsParty, AgpdCharacter *pcsBase, AgpdCharacter **ppcsMemberBuffer = NULL, INT32 *plMemberTotalLevel = NULL);
	//INT32	GetNearMember(AgpdParty *pcsParty, AgpdCharacter **ppcsMemberBuffer = NULL, INT32 *plMemberTotalLevel = NULL);

	BOOL	IsNearMember(AgpdCharacter *pcsOperator, AgpdCharacter *pcsTarget, FLOAT lDistance = 8000);			//2005.06.07. By SungHoon

	INT32	GetPartyBonusDamage(ApBase *pcsBase, AgpdSkill *pcsSkill = NULL);

	BOOL	RemoveFarMemberView(AgpdParty *pcsParty);								//	2005.05.25 By SungHoon
	BOOL	RemoveFarMemberView(AgpdParty *pcsParty, INT32 lCID, int nStartPos = 0);	//	2005.05.25 By SungHoon
	BOOL	RecalcPartyMemberHP(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, BOOL bRecalc);		//	2005.06.28. By SungHoon

	enum RecallPartyError
	{
		DEAD			,
		INSECRETDUNGEON	,
		INSIEGE			,
		LACKOFLEVEL
	};
	template< class ErrorCallback >
	BOOL	CheckPartyMemberForRecallParty( AgpdCharacter *pcsCharacter , ErrorCallback callback );
		// Callback ( error , pcsCharacter );

	BOOL	RecallPartyMember(AgpdCharacter *pcsCharacter);

	BOOL	SetCallbackUseRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackBuddyCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//	For AgpmCharacter
	static BOOL CBUpdateMurdererPoint(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.10.24. By SungHoon
	static BOOL CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL ProcessRemoveMember(INT32 lOperatorID);
};

template< class ErrorCallback >
BOOL AgsmParty::CheckPartyMemberForRecallParty( AgpdCharacter *pcsCharacter , ErrorCallback callback )
{
	if (!pcsCharacter)
		return FALSE;

	AgpdPartyADChar	*pcsAttachData	= m_pagpmParty->GetADCharacter(pcsCharacter);

	if (!pcsAttachData->pcsParty)
		return FALSE;

	if(m_pagpmSiegeWar->IsInSecretDungeon(pcsCharacter))
		return FALSE;

	ApmMap::RegionTemplate	*pcsTargetRegion	= m_papmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
	if( !pcsTargetRegion ) return FALSE;

	AgpdCharacter * pcsManWithProblem = NULL;

	BOOL	bFailed = FALSE;

	for (int i = 0; i < pcsAttachData->pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacterLock(pcsAttachData->pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		if (pcsMember == pcsCharacter)
			continue;

		// 2008.01.03. steeple
		// 해당 캐릭터가 비밀의 던젼에 있으면 안된다.
		if (pcsMember->IsDead() )
		{
			callback( DEAD , pcsCharacter , pcsMember );
			pcsMember->m_Mutex.Release();
			bFailed = TRUE;
			continue;
		}

		if(	m_pagpmSiegeWar->IsInSecretDungeon(pcsMember))
		{
			callback( INSECRETDUNGEON , pcsCharacter , pcsMember );
			pcsMember->m_Mutex.Release();
			bFailed = TRUE;
			continue;
		}

		// 2008.01.03. steeple
		// 해당 캐릭터가 공성지역안에 있어도 안된다.
		AgpdSiegeWar* pcsSiegeWar = m_pagpmSiegeWar->GetSiegeWarInfo(pcsMember);
		if(pcsSiegeWar && m_pagpmSiegeWar->IsStarted(pcsSiegeWar))
		{
			callback( INSIEGE , pcsCharacter , pcsMember );
			pcsMember->m_Mutex.Release();
			bFailed = TRUE;
			continue;
		}

		// Level Check

		// 레벨 제한이 있는지 확인한다.
		if( pcsTargetRegion->nLevelMin &&
			pcsMember->m_nBindingRegionIndex != pcsCharacter->m_nBindingRegionIndex )
		{
			INT32	nLevelOriginal = m_pagpmCharacter->GetLevelOriginal( pcsMember );

			//  최저 레벨보다 높아야 한다
			if( nLevelOriginal < pcsTargetRegion->nLevelMin )
			{
				callback( LACKOFLEVEL , pcsCharacter , pcsMember );
				pcsMember->m_Mutex.Release();
				bFailed = TRUE;
				continue;
			}
		}

		pcsMember->m_Mutex.Release();
	}

	return bFailed;
}


#endif //__AGSMPARTY_H__
