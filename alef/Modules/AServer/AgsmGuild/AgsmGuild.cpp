// AgsmGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.


#include "AgsmGuild.h"
#include "ApAutoLockCharacter.h"

#include "AgsmSiegeWar.h"
#include "AgpmArchlord.h"
#include "AuTimeStamp.h"
#include "AgpmBattleGround.h"
#include "AgppGuild.h"
#include "AgspGuild.h"
#include "AgsmSystemMessage.h"
#include "ApmMap.h"

#define AGSMGUILD_MAX_MEMBER_COUNT							50			// 실제 제한. AgpmGuild 에 있는 놈은 Hash 의 크기세팅이다.
#define AGSMGUILD_IDLE_BATTLE_PROCESS_INTERVAL				1000		// ms. 1초
#define AGSMGUILD_IDLE_MEMBER_STATUS_REFRESH				1000 * 30	// ms. 30초. 2005.06.16.steeple

//////////////////////////////////////////////////////////////////////////

AgsmGuild::AgsmGuild()
{
	m_hEventGuildLoadComplete = NULL;

	SetModuleName("AgsmGuild");
	SetModuleType(APMODULE_TYPE_SERVER);

	SetModuleData(sizeof(AgsdGuildMember));
	SetModuleData(sizeof(AgsdGuildBattleProcess), AGSMGUILD_DATA_TYPE_BATTLE_PROCESS);

	EnableIdle2(TRUE);

	m_pagpmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmGuild = NULL;
	m_pagpmPvP = NULL;
	m_pagpmLog = NULL;
	
	m_pagsmServerManager = NULL;
	m_pagsmInterServerLink = NULL;
	m_pagsmAOIFilter = NULL;
	m_pagsmCharacter = NULL;
	m_pagsmChatting = NULL;
	m_pagsmCharManager = NULL;
	m_pagsmLogin = NULL;
	m_pagsmItem = NULL;
	m_pagpmSiegeWar = NULL;
	m_pagpmArchlord = NULL;
	m_pagpmBattleGround		= NULL;
	m_pagsmSystemMessage	= NULL;
	m_papmMap				= NULL;

	m_nIndexADMember = 0;

	// Packet Define
	SetPacketType(AGSMGUILD_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_MEMORY_BLOCK,	1,	// class AgpdGuild
							AUTYPE_MEMORY_BLOCK,	1,	// class AgpdGuildMember
							AUTYPE_MEMORY_BLOCK,	1,	// Notice
							AUTYPE_END,				0
							);

	m_ulBattleReadyTime = AGPMGUILD_BATTLE_READY_TIME;
	m_ulBattleCancelEnableTime = AGPMGUILD_BATTLE_CANCEL_ENABLE_TIME;
	m_lBattleNeedMemberCount = AGPMGUILD_BATTLE_NEED_MEMBER_COUNT;
	m_lBattleNeedLevelSum = AGPMGUILD_BATTLE_NEED_LEVEL_SUM;

	m_ulLastProcessBattleClock = 0;
	m_ulLastProcessLeaveGuildClock = 0;
}

AgsmGuild::~AgsmGuild()
{
	m_ApGuildLeaveList.DeleteAllMemory();
}

BOOL AgsmGuild::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmItem = (AgpmItem *)GetModule("AgpmItem");
	m_pagpmGuild = (AgpmGuild*)GetModule("AgpmGuild");
	m_pagpmPvP = (AgpmPvP*)GetModule("AgpmPvP");
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");

	m_pagsmServerManager = (AgsmServerManager*)GetModule("AgsmServerManager2");
	m_pagsmInterServerLink = (AgsmInterServerLink*)GetModule("AgsmInterServerLink");
	m_pagsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");
	m_pagsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagsmChatting = (AgsmChatting*)GetModule("AgsmChatting");
	m_pagsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pagsmLogin = (AgsmLogin*)GetModule("AgsmLogin");
	m_pagsmItem = (AgsmItem*)GetModule("AgsmItem");
	m_pagpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	//m_pagsmSiegeWar = (AgsmSiegeWar*)GetModule("AgsmSiegeWar");
	m_pagpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");
	m_papmMap		= (ApmMap*)GetModule("ApmMap");

	if(!m_pagpmCharacter || !m_pagpmFactors || !m_pagpmGuild || !m_pagpmPvP ||
		!m_pagsmServerManager || !m_pagsmInterServerLink ||
		!m_pagsmCharacter || !m_pagsmChatting || !m_pagsmCharManager || !m_pagpmSiegeWar || !m_pagpmArchlord || !m_papmMap)
		return FALSE;

	m_nIndexADMember = m_pagpmGuild->AttachMemberData(this, sizeof(AgsdGuildMember), ConAgsdGuildMember, DesAgsdGuildMember);
	if(m_nIndexADMember < 0)
		return FALSE;

	// AgpmGuild
	if(!m_pagpmGuild->SetCallbackGuildCreateCheck(CBCreateGuildCheck, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildCreateEnableCheck(CBCreateGuildCheckEnable, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildCreate(CBCreateGuildSuccess, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildCreateFail(CBCreateGuildFail, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinRequest(CBJoinRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinFail(CBJoinFail, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinEnableCheck(CBJoinCheckEnable, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinReject(CBJoinReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoin(CBJoin, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildLeave(CBLeave, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildForcedLeave(CBForcedLeave, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildDestroy(CBDestroyGuild, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackCheckPassword(CBCheckPassword, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackUpdateNotice(CBUpdateNotice, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackSystemMessage(CBSystemMessage, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallbackBattlePerson(CBBattlePerson, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleRequest(CBBattleRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleAccept(CBBattleAccept, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleReject(CBBattleReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleCancelRequest(CBBattleCancelRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleCancelAccept(CBBattleCancelAccept, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleCancelReject(CBBattleCancelReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleWithdraw(CBBattleWithdraw, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildUpdateMaxMemberCount(CBGuildUpdateMaxMemberCount, this))
		return FALSE;
//	2005.07.28. By SungHoon
	if(!m_pagpmGuild->SetCallbackReceiveLeaveRequestSelf(CBReceiveLeaveRequestSelf, this))
		return FALSE;
	
	// AgpmCharacter
	if(!m_pagpmCharacter->SetCallbackUpdateLevel(CBCharacterLevelUp, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))		// 2005.11.09. steeple 추가~~
		return FALSE;

	if(!m_pagsmInterServerLink->SetCallbackConnect(CBServerConnect, this))
		return FALSE;
	//if(!m_pagsmInterServerLink->SetCallbackSetFlag(CBServerSetFlag, this))
	//	return FALSE;

	if(!m_pagsmCharacter->SetCallbackSendCharacterNewID(CBSendCharacterNewID, this))	// login server to game server
		return FALSE;	
	if(!m_pagsmCharacter->SetCallbackIsGuildMaster(CBIsGuildMaster, this))	// 2005.06.01 By SungHoon
		return FALSE;	

	if(!m_pagsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;
	
	// 2005.11.09. steeple. 접속해제 콜백은 AgpmCharacter::RemoveCharacter 로 바뀌었다. 
	//if(!m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnect, this))
	//	return FALSE;

	if(!m_pagsmCharManager->SetCallbackDeleteComplete(CBDeleteComplete, this))
		return FALSE;
	
	if(!m_pagsmChatting->SetCallbackGuildMemberInvite(CBGuildMemberInviteByCommand, this))
		return FALSE;
	if(!m_pagsmChatting->SetCallbackGuildLeave(CBGuildLeaveByCommand, this))
		return FALSE;
	if(!m_pagsmChatting->SetCallbackGuildJointMessage(CBGuildJointMessage, this))
		return FALSE;

	if(!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveCell, this))
		return FALSE;

	//if(!m_pagsmCharacter->SetCallbackSendCharView(CBSendCharView, this))
	//	return FALSE;
//	2005.07.08. By SungHoon
	if(!m_pagpmGuild->SetCallbackReceiveGuildList(CBReceiveGuildListInfo, this))
		return FALSE;
//	2005.08.02. By SungHoon
	if(!m_pagpmGuild->SetCallbackReceiveJoinRequestSelf(CBReceiveJoinRequestSelf, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackDestroyGuildCheckTime(CBDestroyGuildCheckTime, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackRenameGuildID(CBGuildRenameGuildID,this))
		return FALSE;

//	2005.09.05. By SungHoon
	if (m_pagsmLogin)
	{
		if(!m_pagsmLogin->SetCallbackCharacterRenamed(CBGuildRenameCharacterID,this))
			return FALSE;
	}

	if(!m_pagpmGuild->SetCallbackBuyGuildMark(CBGuildBuyGuildMark,this))
		return FALSE;

	if(!m_pagpmGuild->SetCallbackBuyGuildMarkForce(CBGuildBuyGuildMarkForce,this))
		return FALSE;
	
	if(!m_pagpmGuild->SetCallbackJointRequest(CBJointRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackJointReject(CBJointReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackJoint(CBJoint, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackJointLeave(CBJointLeave, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileRequest(CBHostileRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileReject(CBHostileReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostile(CBHostile, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileLeaveRequest(CBHostileLeaveRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileLeaveReject(CBHostileLeaveReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileLeave(CBHostileLeave, this))
		return FALSE;

	if (!m_pagsmItem->SetCallbackPickupCheckItem(CBItemPickupCheck, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_REQUEST, OnOperationWorldChampionshipRequest, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_ENTER, OnOperationWorldChampionshipEnter, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_REQUEST, OnOperationCSAppointmentRequest, this))
		return FALSE;
	
	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ANSWER, OnOperationCSAppointmentAnswer, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_REQUEST, OnOperationCSSuccessionRequest, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ANSWER, OnOperationCSSuccessionAnswer, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmGuild::OnInit()
{
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	if(!m_pagpmBattleGround)
		return FALSE;

	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	if(!m_pagsmSystemMessage)
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmGuild::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmGuild::OnIdle2(UINT32 ulClockCount)
{
	//STOPWATCH2(GetModuleName(), _T("OnIdle2"));

	ProcessAllBattle(ulClockCount);

	//ProcessRefreshAllGuildMemberStatus(ulClockCount);

	CheckLeaveGuildList(ulClockCount);

	return TRUE;
}

BOOL AgsmGuild::OnDestroy()
{
	// 배틀 리스트를 비운다.
	BattleListLock();

	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); ++Itr )
		RemoveBattleProcess( *Itr );

	m_listBattleProcess.clear();

	BattleListUnlock();

	return TRUE;
}

BOOL AgsmGuild::SetEventGuildLoadComplete(HANDLE hEvent)
{
	m_hEventGuildLoadComplete = hEvent;
	return TRUE;
}

AgsdGuildMember* AgsmGuild::GetADMember(AgpdGuildMember* pData)
{
	if(m_pagpmGuild)
		return (AgsdGuildMember*)m_pagpmGuild->GetAttachedModuleData(m_nIndexADMember, (PVOID)pData);

	return NULL;
}

BOOL AgsmGuild::ConAgsdGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgsdGuildMember* pAttachedData = pThis->GetADMember((AgpdGuildMember*)pData);

	if(!pAttachedData)
		return FALSE;

	pAttachedData->m_lServerID = 0;
	pAttachedData->m_ulNID = 0;

	pAttachedData->m_pcsCharacter = NULL;

	return TRUE;
}

BOOL AgsmGuild::DesAgsdGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
// Operation

// 2005.03.18. steeple
// 서버간 싱크를 맞추기 위해서 AgsmGuild 가 패킷을 주고 받는다.
BOOL AgsmGuild::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgsmGuild::OnReceive");
	
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	PVOID pvGuild = NULL;
	PVOID pvMember = NULL;
	PVOID pvNotice = NULL;

	INT16 nGuildLength = -1;
	INT16 nMemberLength = -1;
	INT16 nNoticeLength = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvGuild, &nGuildLength,
						&pvMember, &nMemberLength,
						&pvNotice, &nNoticeLength
						);

	switch(cOperation)
	{
		case AGSMGUILD_PACKET_SYNC_CREATE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			OnReceiveSyncCreate(pcsGuild);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_JOIN:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			AgpdGuildMember* pcsMember = reinterpret_cast<AgpdGuildMember*>(pvMember);
			OnReceiveSyncJoin(pcsGuild, pcsMember);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_LEAVE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			AgpdGuildMember* pcsMember = reinterpret_cast<AgpdGuildMember*>(pvMember);
			OnReceiveSyncLeave(pcsGuild, pcsMember);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_FORCED_LEAVE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			AgpdGuildMember* pcsMember = reinterpret_cast<AgpdGuildMember*>(pvMember);
			OnReceiveSyncForcedLeave(pcsGuild, pcsMember);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_DESTROY:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			OnReceiveSyncDestroy(pcsGuild);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_UPDATE_MAX_MEMBER_COUNT:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			OnReceiveSyncUpdateMaxMemberCount(pcsGuild);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_UPDATE_NOTICE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			CHAR* szNotice = reinterpret_cast<CHAR*>(pvNotice);
			OnReceiveSyncUpdateNotice(pcsGuild, szNotice, nNoticeLength);
			break;
		}
	}

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 생성된 길드를 받는다.
BOOL AgsmGuild::OnReceiveSyncCreate(AgpdGuild* pcsGuild)
{
	if( !pcsGuild )			return FALSE;

	AgpdGuild* pcsNewGuild = m_pagpmGuild->CreateGuild(pcsGuild->m_szID, pcsGuild->m_lTID, pcsGuild->m_lRank, pcsGuild->m_lMaxMemberCount, pcsGuild->m_lUnionID);
	if( !pcsNewGuild )		return FALSE;
		
	ZeroMemory( pcsNewGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1) );
	strncpy( pcsNewGuild->m_szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING );

	// CreateionDate
	if(pcsGuild->m_lCreationDate != -1)		// 길드 생성 날짜가 넘어왔다면 세팅 (보통 클라이언트)
		pcsNewGuild->m_lCreationDate = pcsGuild->m_lCreationDate;
	else								// 길드 생성 날짜가 넘어오지 않았다면 지금 시간으로 세팅 (보통 서버)
		pcsNewGuild->m_lCreationDate = m_pagpmGuild->GetCurrentTimeStamp();

	// Password
	ZeroMemory( pcsNewGuild->m_szPassword, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH + 1) );
	strncpy( pcsNewGuild->m_szPassword, pcsGuild->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH );

	pcsNewGuild->m_szNotice = NULL;

	// Lock 을 풀고
	pcsNewGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 조인한 사람을 받는다.
BOOL AgsmGuild::OnReceiveSyncJoin( AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember )
{
	if( !pcsGuild || !pcsMember )		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock( pcsGuild->m_szID );
	if( !pcsGuildIn )					return FALSE;
		
	// Join 에 성공하면
	if( m_pagpmGuild->JoinMember( pcsGuildIn, pcsMember->m_szID, pcsMember->m_lRank, pcsMember->m_lJoinDate, pcsMember->m_lLevel,
								pcsMember->m_lTID, pcsMember->m_cStatus))
	{
		AgpdGuildMember* pcsNewMember = m_pagpmGuild->GetMember(pcsGuildIn, pcsMember->m_szID);
		if(pcsNewMember)
		{
			// 다른 멤버에게 조인을 알린다.
			SendNewMemberJoinToOtherMembers(pcsGuildIn, pcsNewMember);
		}
	}

	pcsGuildIn->m_Mutex.Release();

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 길드에서 탈퇴함을 받는다.
BOOL AgsmGuild::OnReceiveSyncLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	if(!m_pagpmGuild->LeaveMember(pcsGuild->m_szID, pcsMember->m_szID, FALSE))
		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if(!pcsGuildIn)
		return FALSE;

	SendLeaveAllowToOtherMembers(pcsGuildIn, pcsMember->m_szID);

	pcsGuildIn->m_Mutex.Release();

	LeaveChar(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsMember->m_szID);

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 길드에서 짤림을 받는다.
BOOL AgsmGuild::OnReceiveSyncForcedLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	if(!m_pagpmGuild->ForcedLeaveMember(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsMember->m_szID))
		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if(!pcsGuildIn)
		return FALSE;

	SendForcedLeaveToOtherMembers(pcsGuildIn, pcsMember->m_szID);

	pcsGuildIn->m_Mutex.Release();

	LeaveChar(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsMember->m_szID);

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 길드 해체를 받는다.
BOOL AgsmGuild::OnReceiveSyncDestroy(AgpdGuild* pcsGuild)
{
	if( !pcsGuild )		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if( !pcsGuildIn )	return FALSE;

	// Member 이름들을 미리 백업해놓는다.
	INT32 i = 0;
	CHAR** pszMemberID = NULL;
	INT32 lMemberCount = pcsGuildIn->m_pMemberList->GetObjectCount();
	if(lMemberCount > 0)
	{
		pszMemberID = new CHAR*[lMemberCount];
		for(i = 0; i < lMemberCount; i++)
		{
			pszMemberID[i] = new CHAR[AGPACHARACTER_MAX_ID_STRING+1];
			memset(pszMemberID[i], 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));
		}
	}

	INT32 lIndex = 0; i = 0;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuildIn->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
				ppcsMember = (AgpdGuildMember**)pcsGuildIn->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(*ppcsMember)
			strncpy(pszMemberID[i], (*ppcsMember)->m_szID, AGPACHARACTER_MAX_ID_STRING);

		i++;
	}

	// Destroy 불러주고~
	if(m_pagpmGuild->DestroyGuild(pcsGuildIn))
	{
		DestroyGuild(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsGuild->m_lMaxMemberCount, pszMemberID, FALSE, FALSE);
	}

	// 백업해 놓은 Member 이름 제거
	if(pszMemberID && lMemberCount)
	{
		for(i = 0; i < lMemberCount; i++)
		{
			delete [] pszMemberID[i];
		}

		delete [] pszMemberID;
	}

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 길드 맥스 멤버 카운트를 받았다.
BOOL AgsmGuild::OnReceiveSyncUpdateMaxMemberCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if(!pcsGuildIn)
		return FALSE;

	UpdateMaxMemberCount(pcsGuildIn, pcsGuild->m_lMaxMemberCount, FALSE);

    pcsGuildIn->m_Mutex.Release();

	return TRUE;
}

// 2005.03.18. steeple
// 다른 게임 서버에서 길드 공지사항을 받는다.
BOOL AgsmGuild::OnReceiveSyncUpdateNotice(AgpdGuild* pcsGuild, CHAR* szNotice, INT16 nNoticeLength)
{
	if( !pcsGuild )		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if( !pcsGuildIn )	return FALSE;

	// 먼저 번에 있다면 Clear
	if( pcsGuildIn->m_szNotice )
	{
		delete [] pcsGuildIn->m_szNotice;
		pcsGuildIn->m_szNotice = NULL;
	}

	// 세팅해준다.
	if( szNotice && nNoticeLength > 0 && nNoticeLength <= AGPMGUILD_MAX_NOTICE_LENGTH )
	{
		pcsGuildIn->m_szNotice = new CHAR[nNoticeLength+1];
		ZeroMemory(pcsGuildIn->m_szNotice, sizeof(CHAR) * (nNoticeLength + 1));
		memcpy( pcsGuildIn->m_szNotice, szNotice, nNoticeLength );
	}

	SendNoticeToAllMember( pcsGuildIn );

	pcsGuildIn->m_Mutex.Release();

	return TRUE;
}

// 2005.01.24. steeple
// Guild 생성 시 이름을 사용 가능한지 DB 에서 체크한다.
BOOL AgsmGuild::CreateGuildCheck(AgpdGuild* pcsGuild, CHAR* szMasterID)
{
	if(!pcsGuild || !szMasterID)
		return FALSE;

	// 걍 Callback 부르면 된다.
	EnumCallback(AGSMGUILD_CB_DB_MASTER_CHECK, pcsGuild, szMasterID);

	return TRUE;
}

// Guild 생성 시 가능한 지 체크한다.
BOOL AgsmGuild::CreateGuildCheckEnable(CHAR* szGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	INT8 cErrorCode = -1;
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT32 lData1 = 0;
	INT32 lData2 = 0;
	
	INT32 lTaxRatio = 0;

	BOOL bAlreadyMember = FALSE;

	// Character 의 정보를 얻는다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
	if(!pcsCharacter)
		return FALSE;

	lCID = pcsCharacter->m_lID;
	ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	INT32 lLevel = 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	INT64 llMoney = 0;
	m_pagpmCharacter->GetMoney(pcsCharacter, &llMoney);

	AgpdGuildADChar* pcsADCharGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsADCharGuild)
	{
		if(strlen(pcsADCharGuild->m_szGuildID) != 0 || strlen(pcsADCharGuild->m_szRequestJoinGuildID) != 0)
			bAlreadyMember = TRUE;
	}

	// 릴리즈하기전에 구하자. -_-;
	lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	// 이미 다른 길드의 멤버라면 Error
	if(bAlreadyMember)
		cErrorCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER;

	// Guild Name 특수문자 체크
	if(cErrorCode == -1)
	{
		if(!CheckGuildID(szGuildID))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_USE_SPECIAL_CHAR;
	}

	// Guild Name 중복 체크
	if(cErrorCode == -1)
	{
		std::string szTempName = szGuildID;
		std::transform(szTempName.begin(),szTempName.end(),szTempName.begin(),toupper); //대소구분없이 비교...//JK_길드명오류
		AgpdGuild* pcsGuild = NULL;
		INT32 Index = 0;

		for(pcsGuild = m_pagpmGuild->GetGuildSequence(&Index); pcsGuild; pcsGuild = m_pagpmGuild->GetGuildSequence(&Index))
		{
			std::string szID = pcsGuild->m_szID;
			std::transform(szID.begin(),szID.end(),szID.begin(),toupper); //대소구분없이 비교...//JK_길드명오류
			if( szTempName == szID )
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID;
				break;
			}
		}
	}

	// 길드 이름이 성의 이름과 같으면 허용 할 수 없다.
	if (cErrorCode == -1)
	{
		for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
		{
			if (COMPARE_EQUAL == m_pagpmSiegeWar->m_csSiegeWarInfo[i].m_strCastleName.CompareNoCase(szGuildID))
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID;
				break;
			}
		}
	}

	// 종족 체크..

	// Level Check
	if(cErrorCode == -1)
	{
		if(lLevel < AGPMGUILD_CREATE_REQUIRE_LEVEL)
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_MORE_LEVEL;
	}

	/* 돈과 아이템은 MaxMember쪽의 데이터로 검사한다.
	// 돈 검사
	if(cErrorCode == -1)
	{
		if(llMoney < AGPMGUILD_CREATE_REQUIRE_MONEY)
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_MORE_MONEY;
	}
	*/

	AgpdRequireItemIncreaseMaxMember *pcsRequirement = m_pagpmGuild->GetRequireIncreaseMaxMember(AGSMGUILD_MAX_MEMBER_COUNT);
	if (cErrorCode == -1 && pcsRequirement)
	{
		// 길드 정책 변경 - arycoat 2008.02
		/*if (pcsRequirement->m_lSkullTID)
		{
			AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter, pcsRequirement->m_lSkullTID);
			if(!pcsItem)
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_ITEM;
				lData1 = pcsRequirement->m_lSkullTID;
				lData2 = pcsRequirement->m_lSkullCount;
			}

			if (cErrorCode == -1 && pcsItem->m_nCount < pcsRequirement->m_lSkullCount)
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_ITEM;
				lData1 = pcsRequirement->m_lSkullTID;
				lData2 = pcsRequirement->m_lSkullCount;
			}
		}*/

		INT32 lGheld = pcsRequirement->m_lGheld;
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lGheld * lTaxRatio) / 100;
		}
		lGheld = lGheld + lTax;	

		if (cErrorCode == -1 && pcsCharacter->m_llMoney < lGheld)
		{
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_MORE_MONEY;
			lData1 = pcsRequirement->m_lGheld;
		}
	}

	// Error 가 발생했으면, 메시지를 보낸다.
	if(cErrorCode != -1)
		SendSystemMessage(cErrorCode, ulNID, NULL, NULL, lData1, lData2);

	return cErrorCode == -1 ? TRUE : FALSE;
}

BOOL AgsmGuild::CheckGuildKorID(CHAR* szGuildID, int idLength)
{
	//공백을 체크한다.
	BOOL bContainBanLetter = FALSE;

	unsigned char	cData;
	unsigned char	cData2;

	for( INT32 lCounter=0; lCounter<idLength; lCounter++ )
	{
		cData = szGuildID[lCounter];

		//영문자 소문자인지 본다.
		if( ('a' <= cData) && (cData <= 'z' ) )
		{
			continue;
		}

		//영문자 대문자인지 본다.
		if( ('A' <= cData) && (cData <= 'Z' ) )
		{
			continue;
		}

		//숫자인지 본다.
		if( ('0' <= cData) && (cData <= '9' ) )
		{
			continue;
		}

		//완성된 한글인지 본다. 한글은 0x80보다 크다.
		if( cData > 0x80 )
		{
			if( lCounter < idLength )
			{
				if( (0xB0 <= cData) && (cData <= 0xC8) )
				{
					cData2 = szGuildID[lCounter+1];

					if( (0xA1 <= cData2) && (cData2 <= 0xFE) )
					{
						//한글이니까 lCounter++ 한다.
						lCounter++;
						continue;
					}
				}
			}
		}

		bContainBanLetter = TRUE;
		break;
	}
	
	return !bContainBanLetter;
}

//특수문자이면 FALSE
BOOL AgsmGuild::CheckGuildCnID(CHAR* szGuildID, int idLength)
{
	unsigned char c1;

	for ( int i = 0; i < idLength; ++i )
	{
		c1 = szGuildID[i];

		if ( c1 < '0'
			|| ( c1 > '9' && c1 < 'A' )//특수문자체크
			|| ( c1 > 'Z' && c1 < 'a' )
			|| ( c1 > 'z' && c1 <= 127 ) )
			return FALSE;
	}

	return TRUE;
}

// 길드 이름을 사용할 수 있는 지 체크한다.
// AgsmLoginClient 에서 Copy & Paste & Modify
BOOL AgsmGuild::CheckGuildID(CHAR* szGuildID)
{
	BOOL bResult = TRUE;
	INT32 lGuildIDLength = (INT32)strlen(szGuildID);

	if(lGuildIDLength > 0 && lGuildIDLength <= AGPMGUILD_MAX_MAKE_GUILD_ID_LENGTH)
	{
		if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
			bResult = CheckGuildCnID(szGuildID, lGuildIDLength);
		else
			bResult = CheckGuildKorID(szGuildID, lGuildIDLength);
	}
	else
	{
		bResult = FALSE;
	}

	// 욕설인지 확인한다. 2005.10.11. By SungHoon
	if (bResult == TRUE)
	{
		ApSafeArray<CHAR, 128> tempName;
		tempName.MemSetAll();

		strncpy(&tempName[0], szGuildID, 128);
		_mbslwr((unsigned char*)&tempName[0]);

		if (FALSE == m_pagpmCharacter->CheckFilterText(&tempName[0]))
			return FALSE;
	}

	return bResult;
}

// AgpmGuild 에서 길드를 다 만든 후 부른다.
BOOL AgsmGuild::CreateGuildSuccess(CHAR* szGuildID, CHAR* szMasterID)
{
	//STOPWATCH2(GetModuleName(), _T("CreateGuildSuccess"));

	if(!szGuildID || !szMasterID)
	{
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !szGuildID || !szMasterID\n");
		return FALSE;
	}

	// Character 정보를 얻어놓는다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
	if(!pcsCharacter)
	{
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !pcsCharacter\n");
		return FALSE;
	}

	INT32 lCID = pcsCharacter->m_lID;

	INT32 lLevel = 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	INT32 lRace = 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);

	INT32 lTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;

	INT32 lServerID = 0;
	UINT32 ulNID = 0;
	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(pcsAgsdCharacter)
	{
		lServerID = pcsAgsdCharacter->m_ulServerID;
		ulNID = pcsAgsdCharacter->m_dpnidCharacter;
	}

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
	{
		pcsCharacter->m_Mutex.Release();
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !pcsGuild\n");
		return FALSE;
	}

	// Max Member Count 를 계산해서 넣는다.
	pcsGuild->m_lMaxMemberCount = CalcMaxMemberCount(pcsCharacter);

	// TID 넣어주고
	pcsGuild->m_lTID = lTID;
	
	// Master Race 를 Guild UnionID 로 넣어준다.
	pcsGuild->m_lUnionID = lRace;

	// Guild 생성 패킷을 보낸다.
//	SendCreateGuild(pcsGuild, lCID, ulNID);		2005.08.16. By SungHoon

	// 기본 멤버에 대한 조건을 가져와서 처리한다.
	AgpdRequireItemIncreaseMaxMember *pcsRequirement = m_pagpmGuild->GetRequireIncreaseMaxMember(AGSMGUILD_MAX_MEMBER_COUNT);
	if (pcsRequirement)
	{
		// 돈 까준다.
		INT32 lGheld = pcsRequirement->m_lGheld;
		INT32 lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lGheld * lTaxRatio) / 100;
		}
		lGheld = lGheld + lTax;	
	
		m_pagpmCharacter->SubMoney(pcsCharacter, lGheld);
		m_pagpmCharacter->PayTax(pcsCharacter, lTax);

		// 길드 정책 변경 - arycoat 2008.02
		// 필요 해골 삭제
		// 아이템도 까준다.
		/*if (pcsRequirement->m_lSkullTID)
		{
			AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter, pcsRequirement->m_lSkullTID);
			if(pcsItem)
			{
				m_pagpmItem->SubItemStackCount(pcsItem, pcsRequirement->m_lSkullCount, TRUE);
			}
		}*/
	}

	// Master 를 Member 로 넣어준다.
	UINT32 lCurrentTimeStamp = m_pagpmGuild->GetCurrentTimeStamp();
	m_pagpmGuild->JoinMember(pcsGuild, szMasterID, AGPMGUILD_MEMBER_RANK_MASTER, lCurrentTimeStamp,
												lLevel, lTID, AGPMGUILD_MEMBER_STATUS_ONLINE);

	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMasterID);
	if(!pcsMember)	// 여기서 못 얻으면 에러
	{
		pcsGuild->m_Mutex.Release();
		pcsCharacter->m_Mutex.Release();
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !pcsMember");
		return FALSE;
	}

	GuildJoinSuccess(pcsGuild, pcsMember, pcsCharacter, TRUE);

	// 길드초대 거부 플래그를 없앤다		2005.06.02 By SungHoon
	pcsCharacter->m_lOptionFlag = m_pagpmCharacter->SetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmCharacter->MakePacketOptionFlag(pcsCharacter, pcsCharacter->m_lOptionFlag, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
/*	2005.08.16. By SungHoon
	// Attached Data Setting
	AgpdGuildADChar* pcsAttachedData = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsAttachedData)
	{
		ZeroMemory(pcsAttachedData->m_szGuildID, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
		strncpy(pcsAttachedData->m_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		pcsAttachedData->m_pMemberData = pcsMember;
	}

	// Master 가 Guild 에 Join 했다고 보내준다.
	SendJoinMember(pcsGuild, pcsMember, lCID, ulNID);

	// AgsdGuildMember Data Setting
	AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
	if(pcsAgsdMember)
		UpdateAgsdGuildMember(pcsAgsdMember, lServerID, ulNID, pcsCharacter);

	// Master 에게 자기 길드 정보를 준다.
	SendCharGuildData(szGuildID, szMasterID, pcsMember, lCID, ulNID);

	// 주변에 길드아이디를 뿌린다.
	SendCharGuildIDToNear(szGuildID, szMasterID, pcsCharacter->m_stPos, ulNID);
*/
	//////////////////////////////////////////////////////////////////////////
	// DB 에 쓴다.
	// Guild Inert
	EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT, pcsGuild, NULL);

	// Master Insert
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, szGuildID, pcsMember);

	// 2005.03.18. steeple
	// 다른 서버에 Sync 한다.
	//SendSyncCreate(pcsGuild);
	//SendSyncJoin(pcsGuild, pcsMember);

	// Guild Unlock
	pcsGuild->m_Mutex.Release();

	// Character Unlock
	pcsCharacter->m_Mutex.Release();

	// 길드 생성이 되었다는 메시지를 보낸다.
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_CREATE_COMPLETE, ulNID);

	// Log
	if (m_pagpmLog)
	{
		INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

		if( pcsAgsdCharacter)
		{
			m_pagpmLog->WriteLog_GuildCreate(0, 
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										lTID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										NULL,
										0									// gheld creation price
										);
		}
	}

	m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | return TRUE 직전");

	return TRUE;
}

// 현재로서는 불리지 않지만 나중에라도 쓸 지 몰라서 추가. 2004.06.23
BOOL AgsmGuild::CreateGuildFail(CHAR* szGuildID, CHAR* szMasterID)
{
	return TRUE;
}

// pcsCharacter 는 Lock 되어 있다.
BOOL AgsmGuild::JoinRequest(CHAR* szGuildID, CHAR* szMasterID, AgpdCharacter* pcsCharacter)
{
	if(!szGuildID || !szMasterID || !pcsCharacter)
		return FALSE;

	if(m_pagpmGuild->IsMaster(szGuildID, szMasterID) == FALSE && m_pagpmGuild->IsSubMaster(szGuildID, szMasterID) == FALSE) 
		return FALSE;

	INT8 cErrorCode = -1;
	UINT32 ulMasterNID = 0;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(pcsGuild)
	{
		if(pcsGuild->m_lMaxMemberCount <= m_pagpmGuild->GetMemberCount(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_MAX_MEMBER;

		// 2005.04.21. steeple
		// 배틀중에는 할 수 없음~
		if(m_pagpmGuild->IsBattleStatus(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE;

		// 2006.11.14. parn
		// 중국의 경우 union이 다르면 (즉, 길마의 종족과 다르면) 가입할 수 없음~
		//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && !m_pagpmCharacter->IsSameRace(pcsCharacter, pcsGuild->m_lUnionID))
		//	cErrorCode = AGPMGUILD_SYSTEM_CODE_JOIN_FAIL;

		AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
		if(pcsMaster)
			ulMasterNID = GetMemberNID(pcsMaster);

		pcsGuild->m_Mutex.Release();
	}

	if(cErrorCode == -1)
	{
		AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
		if(pcsAttachedGuild && (strlen(pcsAttachedGuild->m_szGuildID) != 0 || strlen(pcsAttachedGuild->m_szRequestJoinGuildID) != 0))// 이미 여타 길드에 가입 되어 있는 사람인지 확인
			cErrorCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER2;
		else
		{
			if(m_pagpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN) == TRUE)		//	2005.06.01 By SungHoon
			{
				SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REFUSE, ulMasterNID, pcsCharacter->m_szID );
				return FALSE;
			}
		}
	}

	if(cErrorCode == -1)
	{
		AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsMaster)
		{
			// 상대방이 배틀스퀘어 안에 있다.
			if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
			{
				m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_MEMBER, NULL, NULL, 0, 0, pcsMaster);
				pcsMaster->m_Mutex.Release();
				return TRUE;
			}

			// 마스터가 배틀스퀘어 안에 있다.
			if(m_pagpmPvP->IsCombatPvPMode(pcsMaster))
			{
				m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_GUILD, NULL, NULL, 0, 0, pcsMaster);
				pcsMaster->m_Mutex.Release();
				return TRUE;
			}

			pcsMaster->m_Mutex.Release();
		}
	}
	if(cErrorCode != -1)
		return SendSystemMessage(cErrorCode, ulMasterNID);
	else
		// pcsCharacter 에게 패킷을 보내주면 된다.
		return SendJoinRequest(szGuildID, szMasterID, pcsCharacter->m_szID, pcsCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

// 아무것도 Lock 안되어 있다.
BOOL AgsmGuild::JoinCheckEnable(CHAR* szGuildID, CHAR* szCharID, BOOL bSelfRequest )
{
	if(!szGuildID || !szCharID)
		return FALSE;

	INT8 cErrorCode = -1;

	CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szMasterID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));

	BOOL bJoinRequest = FALSE;
	UINT32 ulMasterNID = 0;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	INT32 lUnionID = -1;
	if(pcsGuild)
	{
		strcpy(szMasterID, pcsGuild->m_szMasterID);

		if(pcsGuild->m_lMaxMemberCount <= m_pagpmGuild->GetMemberCount(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_MAX_MEMBER;

		// 2005.04.21. steeple
		// 배틀중에는 할 수 없음~
		if(m_pagpmGuild->IsBattleStatus(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE;

		AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
		if(pcsMaster)
			ulMasterNID = GetMemberNID(pcsMaster);

		if (pcsGuild->m_pGuildJoinList->GetObject(szCharID)) bJoinRequest = TRUE;

		lUnionID = pcsGuild->m_lUnionID;

		pcsGuild->m_Mutex.Release();
	}

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if (pcsCharacter)
	{
		// 2006.11.14. parn
		// 중국의 경우 union이 다르면 (즉, 길마의 종족과 다르면) 가입할 수 없음~
		//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && !m_pagpmCharacter->IsSameRace(pcsCharacter, lUnionID))
		//	cErrorCode = AGPMGUILD_SYSTEM_CODE_JOIN_FAIL;
	}

	if(cErrorCode == -1)
	{
		if(!bJoinRequest && pcsCharacter)
		{
			AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
			if (!pcsAttachedGuild)
			{
				if (strlen(pcsAttachedGuild->m_szGuildID) != 0 || strlen(pcsAttachedGuild->m_szRequestJoinGuildID) != 0)	// 이미 여타 길드에 가입 되어 있는 사람인지 확인
					cErrorCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER2;
			}
		}
	}

	AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacterLock(szMasterID);
	if(pcsMaster)
	{
		// 상대방이 배틀스퀘어 안에 있다.
		if(pcsCharacter)
		{
			if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
			{
				m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_MEMBER, NULL, NULL, 0, 0, pcsMaster);
				pcsCharacter->m_Mutex.Release();
				pcsMaster->m_Mutex.Release();
				return TRUE;
			}
		}
		// 마스터가 배틀스퀘어 안에 있다.
		if(m_pagpmPvP->IsCombatPvPMode(pcsMaster))
		{
			m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_GUILD, NULL, NULL, 0, 0, pcsMaster);
			if (pcsCharacter) pcsCharacter->m_Mutex.Release();
			pcsMaster->m_Mutex.Release();
			return TRUE;
		}

		pcsMaster->m_Mutex.Release();
	}

	if (cErrorCode != -1)
	{
		// 에러 시스템 메시지는 마스터와, 가입하려했던 사람 두명에게 다 준다.
		if (!bSelfRequest) SendSystemMessage(cErrorCode, ulMasterNID);

		if(pcsCharacter && (bSelfRequest || !bJoinRequest))		//	스스로 가입신청중이거나 가입대기 상태가 아니고 로긴 되었다면
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOIN_FAIL, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
		}
	}
	if(pcsCharacter) pcsCharacter->m_Mutex.Release();


	return cErrorCode == -1 ? TRUE : FALSE;
}

// CharID 가 Guild 가입을 거절했다.
// 아무것도 Lock 안되어 있음.
BOOL AgsmGuild::JoinReject(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	UINT32 ulNID = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(pcsMaster)
	{
		ulNID = GetMemberNID(pcsMaster);
	}

	pcsGuild->m_Mutex.Release();
	
	return SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REJECT, ulNID, szCharID);
}

// 아무것도 Lock 안되어 있고,
// AgpmGuild 에서 이미 가입이 된 상태이고, 그 후의 일을 하면 된다.
BOOL AgsmGuild::JoinMember(CHAR* szGuildID, CHAR* szMemberID)
{
	//STOPWATCH2(GetModuleName(), _T("JoinMember"));

	if(!szGuildID || !szMemberID)
		return FALSE;

	if (AllowJoinMember(szGuildID, szMemberID)) return TRUE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMemberID);
	if(!pcsCharacter)
	{
		// 만약에 여기서 캐릭터를 못 얻더라도 나머지 멤버에게는 보내줘야 함.
		// SendNewMemberJoinToOtherMembers(...);
		return FALSE;
	}

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}
/*
	AgpdGuildRequestMember **pcsRequestMember = (AgpdGuildRequestMember **)pcsGuild->m_pGuildJoinList->GetObject(szMemberID);
	if (*pcsRequestMember)
	{
		if (pcsGuild->m_pGuildJoinList->RemoveObject(szMemberID))
			delete *pcsRequestMember;
	}
*/
	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsMember)
	{
		pcsGuild->m_Mutex.Release();
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	// Member Data 를 제대로 세팅해준다.
	INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);;
	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	INT32 lTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;

	pcsMember->m_lLevel		= lLevel;
	pcsMember->m_lTID		= lTID;
	pcsMember->m_cStatus	= AGPMGUILD_MEMBER_STATUS_ONLINE;
	pcsMember->m_lJoinDate	= m_pagpmGuild->GetCurrentTimeStamp();

	GuildJoinSuccess(pcsGuild, pcsMember, pcsCharacter, TRUE);

	// 모든 멤버에게 Join 했음을 알린다.
	SendNewMemberJoinToOtherMembers(pcsGuild, pcsMember);
	//////////////////////////////////////////////////////////////////////////
	// DB 에 쓴다.
	// Member Insert
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, szGuildID, pcsMember);

	// 2005.03.18. steeple
	// 다른 서버에 Sync 한다.
	//SendSyncJoin(pcsGuild, pcsMember);

	if (m_pagpmLog)
	{
		INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

		CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
		ZeroMemory(szDesc, sizeof(szDesc));
		sprintf(szDesc, "Master=[%s]", pcsGuild->m_szMasterID);

		AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if (pcsAgsdCharacter)
		{
			m_pagpmLog->WriteLog_GuildIn(0, 
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										lTID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										szDesc
										);
		}
	}

	// Unlock
	pcsGuild->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 아무것도 Lock 안되어 있음
// szCharID 가 szGuildID 에서 탈퇴한다.
BOOL AgsmGuild::Leave(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szMasterID, 0, AGPACHARACTER_MAX_ID_STRING+1);

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	if (m_pagpmGuild->IsMaster(pcsGuild, szCharID))		//	길마는 탈퇴할수 없다.	2005.10.18. By SungHoon
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	else if (m_pagpmGuild->IsSubMaster(pcsGuild, szCharID))
	{
		memset(pcsGuild->m_szSubMasterID, 0, sizeof(pcsGuild->m_szSubMasterID));
	}

	strncpy(szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);

	// 2005.04.21. steeple
	// 배틀중에는 할 수 없음~
/*	if(m_pagpmGuild->IsBattleStatus(pcsGuild))
	{
		pcsGuild->m_Mutex.Release();
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
		if(pcsCharacter)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsCharacter->m_Mutex.Release();
		}
		return FALSE;
	}
*/

	SendLeaveAllowToOtherMembers(pcsGuild, szCharID);

	// 2005.03.18. steeple
	// 다른 서버에 Sync 한다.
	AgpdGuildMember csMember;
	memset(&csMember, 0, sizeof(csMember));		// ApBase 를 상속받지 않고 독립적이므로 memset 해도 된다.
	strcpy(csMember.m_szID, szCharID);
	//SendSyncLeave(pcsGuild, &csMember);

	// 길드 락 풀고
	pcsGuild->m_Mutex.Release();

	//	탈퇴 대기 리스트에 있으면 삭제하고
	RemoveLeaveMember(szCharID);

	// szCharID 에게 길드 해체를 알리고, 리셋한다.
	LeaveChar(szGuildID, szMasterID, szCharID);
	//////////////////////////////////////////////////////////////////////////
	// DB
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, szGuildID, szCharID);
	
	if(m_pagpmLog)
	{
		// 캐릭터 락하지 말구 그냥 얻어온다.
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(szCharID);
		if(pcsCharacter)
		{
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if(pcsAgsdCharacter)
			{
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

			CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
			ZeroMemory(szDesc, sizeof(szDesc));
			sprintf(szDesc, "Master=[%s]", szMasterID);

			m_pagpmLog->WriteLog_GuildOut(0,
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										szDesc
										);
			}
		}
	}
    
	return TRUE;
}

// 아무것도 Lock 안되어 있음
BOOL AgsmGuild::ForcedLeave(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szMasterID, 0, AGPACHARACTER_MAX_ID_STRING+1);

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	if (m_pagpmGuild->IsMaster(pcsGuild, szCharID))		//	길마는 탈퇴할수 없다.	2005.10.18. By SungHoon
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	else if (m_pagpmGuild->IsSubMaster(pcsGuild, szCharID))
	{
		memset(pcsGuild->m_szSubMasterID, 0, sizeof(pcsGuild->m_szSubMasterID));
	}

	strncpy(szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);

	// 2005.04.21. steeple
	// 배틀중에는 할 수 없음~
/*	if(m_pagpmGuild->IsBattleStatus(pcsGuild))
	{
		pcsGuild->m_Mutex.Release();
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsCharacter)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsCharacter->m_Mutex.Release();
		}
		return FALSE;
	}
*/
	SendForcedLeaveToOtherMembers(pcsGuild, szCharID);

	// 2005.03.18. steeple
	// 다른 서버에 Sync 한다.
	AgpdGuildMember csMember;
	memset(&csMember, 0, sizeof(csMember));		// ApBase 를 상속받지 않고 독립적이므로 memset 해도 된다.
	strcpy(csMember.m_szID, szCharID);
	//SendSyncForcedLeave(pcsGuild, &csMember);

	pcsGuild->m_Mutex.Release();

	// szCharID 에게 길드 해체를 알리고, 리셋한다.
	LeaveChar(szGuildID, szMasterID, szCharID);

	// 짤린 놈에게 메시지를 보낸준다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if(pcsCharacter)
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE2, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
		pcsCharacter->m_Mutex.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// DB
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, szGuildID, szCharID);

	if(m_pagpmLog)
	{
		// 캐릭터 락하지 말구 그냥 얻어온다.
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(szCharID);
		if(pcsCharacter)
		{
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if(pcsAgsdCharacter)
			{
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

			CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
			ZeroMemory(szDesc, sizeof(szDesc));
			sprintf(szDesc, "Master=[%s] 강제탈퇴", szMasterID);

			m_pagpmLog->WriteLog_GuildOut(0,
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										szDesc
										);
			}
		}
	}

	return TRUE;
}

// 아무것도 Lock 안되어 있음
// szCharID 의 길드를 없애준다.
BOOL AgsmGuild::LeaveChar(CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	// 해당 캐릭터에게 길드 해체를 알리고, 자기 길드 정보를 리셋해준다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if(!pcsCharacter)
		return FALSE;

	INT32 lCID = pcsCharacter->m_lID;
	UINT32 ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsAttachedGuild)
	{
		memset(pcsAttachedGuild->m_szGuildID, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
		memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
		pcsAttachedGuild->m_pMemberData = NULL;
	}

	AuPOS stPos = pcsCharacter->m_stPos;

	//	길드에 탈퇴했을 경우 길드초대 거부 플래그를 비설정한다.	2005.06.07 By SungHoon
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);

	pcsCharacter->m_Mutex.Release();

	SendDestroyGuild(szGuildID, szMasterID, lCID, ulNID);
	SendCharGuildData(NULL, szCharID, NULL, lCID, ulNID);
	
	// 주변에 길드아이디 뿌린다.
	SendCharGuildIDToNear(NULL, szCharID, stPos, NULL, NULL, FALSE, ulNID);

	return TRUE;
}

// pcsGuild 는 AgpmGuild 에서 Lock 되어 있다.
// Member 들에게 DestroyGuild 패킷을 날린다.
BOOL AgsmGuild::DestroyGuild(CHAR* szGuildID, CHAR* szMasterID, INT32 lMemberCount, CHAR** pszMemberID, BOOL bSaveDB, BOOL bWriteLog)
{
	if(!szGuildID || !szMasterID || !lMemberCount || !pszMemberID)
		return FALSE;

	CHAR szAccountID[AGPDLOG_MAX_ACCOUNTID+1];
	ZeroMemory(szAccountID, sizeof(szAccountID));
	CHAR szIP[AGPDLOG_MAX_IPADDR_STRING+1];
	ZeroMemory(szIP, sizeof(szIP));
	CHAR szWorld[AGPDLOG_MAX_WORLD_NAME+1];
	ZeroMemory(szWorld, sizeof(szWorld));

	AgpdCharacter* pcsCharacter = NULL;
	AgsdCharacter* pcsAgsdCharacter = NULL;

	INT32 lMasterLevel = 0;
	INT32 lMasterTID = 0;
	INT64 llMasterExp = 0;
	INT64 llMasterGheldInven = 0;
	INT64 llMasterGheldBank = 0;

	for(INT32 i = 0; i < lMemberCount; i++)
	{
		if(!pszMemberID[i])
			continue;

		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pszMemberID[i]);
		if(!pcsCharacter)
			continue;

		pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pcsAgsdCharacter)
		{
			pcsCharacter->m_Mutex.Release();
			continue;
		}

		if (0 == strcmp(pcsCharacter->m_szID, szMasterID))
		{
			m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMasterLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
			llMasterExp	= m_pagpmFactors->GetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
			lMasterTID = ((AgpdCharacterTemplate *)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
			strcpy(szAccountID, pcsAgsdCharacter->m_szAccountID);
			strcpy(szIP, &pcsAgsdCharacter->m_strIPAddress[0]);
			strcpy(szWorld, pcsAgsdCharacter->m_szServerName);
			llMasterGheldInven = pcsCharacter->m_llMoney;
			llMasterGheldBank = pcsCharacter->m_llBankMoney;
		}
		//	길드가 사라질 경우 길드초대 거부 플래그를 비설정한다.	2005.06.07 By SungHoon
		pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
		pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
		pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);

		// 해당 Character 의 Guild Attached Data 를 초기화 한다.
		m_pagpmGuild->SetCharAD(pcsCharacter, NULL, -1, -1, FALSE, NULL);

		// 주변에 길드아이디 뿌린다.
		SendCharGuildIDToNear(NULL, pcsCharacter->m_szID, pcsCharacter->m_stPos, NULL, NULL, FALSE, pcsAgsdCharacter->m_dpnidCharacter);

		// Guild Destroy 패킷을 보낸다.
		SendDestroyGuild(szGuildID, szMasterID, 0, pcsAgsdCharacter->m_dpnidCharacter);

		SendCharGuildData(NULL, pcsCharacter->m_szID, NULL, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);

		// 결과 메시지를 보낸다.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY, pcsAgsdCharacter->m_dpnidCharacter);

		pcsCharacter->m_Mutex.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// DB
	if(bSaveDB)
	{
		EnumCallback(AGSMGUILD_CB_DB_GUILD_DELETE, szGuildID, NULL);

		// 2005.03.18. steeple
		// 다른 서버에 Sync 한다.
		//AgpdGuild csGuild;		// 이놈은 memset 하지 말자
		//strcpy(csGuild.m_szID, szGuildID);
		//SendSyncDestroy(&csGuild);
	}

	if(bWriteLog && m_pagpmLog)
	{
		INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

		m_pagpmLog->WriteLog_GuildRemove(0,
										szIP,
										szAccountID,
										szWorld,
										szMasterID,
										lMasterTID,
										lMasterLevel,
										llMasterExp,
										llMasterGheldInven,
										llMasterGheldBank,
										szGuildID,
										NULL
										);
	}

	return TRUE;
}

// pcsGuild 는 AgpmGuild 에서 Lock 되어 있다.
BOOL AgsmGuild::DestroyGuildFail(CHAR* szGuildID, CHAR* szMasterID)
{
	return TRUE;
}

// 2006.08.31. steeple
// szGuildID 로 Guild 를 얻을 때 이미 Lock 되어 있으니 그냥 얻기만 하면 된다.
BOOL AgsmGuild::CleanUpRelation(CHAR* szGuildID, BOOL bSaveDB)
{
	if(!szGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// Joint 부터
	JointLeave(0, szGuildID, TRUE);

	// Hostile
	HostileVector vcHostile(pcsGuild->m_csRelation.m_pHostileVector->begin(), 
							pcsGuild->m_csRelation.m_pHostileVector->end());
	HostileIter iter = vcHostile.begin();
	while(iter != vcHostile.end())
	{
		HostileLeave(szGuildID, iter->m_szGuildID, TRUE);
		++iter;
	}

	return TRUE;
}

// pcsCharacter 는 Lock 되어 있다.
// 2004.11.15. steeple - 공식 변경
INT32 AgsmGuild::CalcMaxMemberCount(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lMaxMemberCount = 0;

	//INT32 lCharisma = 0;
	INT32 lLevel = 0;

	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCharisma, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	//lMaxMemberCount = ((INT32)(lCharisma / 2) + (INT32)(lLevel / 10));
	//lMaxMemberCount = 4 + (INT32)(lLevel / 5);

	lMaxMemberCount = AGSMGUILD_MAX_MEMBER_COUNT;	// 50으로 변경되었음. 2005.06.09. steeple
	return lMaxMemberCount;
}

// 캐릭터가 Level Up 되면 다시 계산 되서 변경되었을 때 불린다.
// Client 에 패킷 보내주고, DB 에 Update 를 날려준다.
BOOL AgsmGuild::UpdateMaxMemberCount(AgpdGuild* pcsGuild, INT32 lNewMaxMember, BOOL bSaveDB)
{
	if(!pcsGuild || lNewMaxMember == 0)
		return FALSE;

	pcsGuild->m_lMaxMemberCount = lNewMaxMember;

	// 접속해 있는 모든 멤버에게 뿌리고,
	SendMaxMemberCountToAllMembers(pcsGuild);

	// DB 업데이트를 한다.
	if(bSaveDB)
	{
		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

		// 2005.03.18. steeple
		// 다른 서버에 Sync 한다.
		//SendSyncUpdateMaxMemberCount(pcsGuild);
	}

	return TRUE;
}

// pcsGuild 는 AgpmGuild 에서 Lock 되어 있다.
BOOL AgsmGuild::CheckPassword(AgpdGuild* pcsGuild, CHAR* szPassword)
{
	if(!pcsGuild || !szPassword)
		return FALSE;

	if(strlen(szPassword) == 0 || strlen(szPassword) > AGPMGUILD_MAX_PASSWORD_LENGTH)
		return FALSE;

	BOOL bValidPassword = FALSE;
	if(strcmp(pcsGuild->m_szPassword, szPassword) == 0)
		bValidPassword = TRUE;

	if(!bValidPassword)
	{
		AgpdGuildMember* pcsMember = m_pagpmGuild->GetMaster(pcsGuild);
		if(pcsMember)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_INVALID_PASSWORD, GetMemberNID(pcsMember));
		}
	}

	return bValidPassword;
}

// AgsdGuildMember Data Update
BOOL AgsmGuild::UpdateAgsdGuildMember(CHAR* szGuildID, CHAR* szMemberID, INT32 lServerID, UINT32 ulNID, AgpdCharacter* pcsCharacter)
{
	if(!szGuildID || !szMemberID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(pcsMember)
	{
		AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
		UpdateAgsdGuildMember(pcsAgsdMember, lServerID, ulNID, pcsCharacter);
	}

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

// AgsdGuildMember Data Update
BOOL AgsmGuild::UpdateAgsdGuildMember(AgsdGuildMember* pcsAgsdMember, INT32 lServerID, UINT32 ulNID, AgpdCharacter* pcsCharacter)
{
	if(!pcsAgsdMember)
		return FALSE;

	pcsAgsdMember->m_lServerID = lServerID;
	pcsAgsdMember->m_ulNID = ulNID;

	pcsAgsdMember->m_pcsCharacter = pcsCharacter;

	return TRUE;
}

BOOL AgsmGuild::EnterGameWorld(AgpdCharacter* pcsCharacter)
{
	if( !pcsCharacter )								return FALSE;
	if( !m_pagpmCharacter->IsPC(pcsCharacter) )		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if( !pcsAgsdCharacter )							return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if( !pcsAttachedGuild )							return TRUE;
	if( !strlen(pcsAttachedGuild->m_szGuildID) )	return TRUE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock( pcsAttachedGuild->m_szGuildID );
	if( !pcsGuild )									return TRUE;

	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if( pcsMember )
	{
		// 접속한 사람에게 Guild Create 를 보내준다.
		pcsAttachedGuild->m_lGuildMarkTID	= pcsGuild->m_lGuildMarkTID;
		pcsAttachedGuild->m_lGuildMarkColor	= pcsGuild->m_lGuildMarkColor;
		pcsAttachedGuild->m_lBRRanking		= pcsGuild->m_lBRRanking;

		SendCreateGuild( pcsGuild, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter );
		
		UpdateAgsdGuildMember( GetADMember( pcsMember ), pcsAgsdCharacter->m_ulServerID, pcsAgsdCharacter->m_dpnidCharacter, pcsCharacter );

		pcsMember->m_cStatus	= AGPMGUILD_MEMBER_STATUS_ONLINE;
		pcsMember->m_lLevel		= m_pagpmCharacter->GetLevel(pcsCharacter);

		// 접속한 사람에게 모든 멤버를 Join 시킨다.
		SendAllMemberJoin( pcsGuild, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter) ;

		// 자신을 포함한 모든 멤버에게 보낸다.
		SendMemberUpdateToMembers( pcsGuild, pcsMember );
		
		// 주위 사람들에게 길드 아이디를 뿌린다.
		SendCharGuildIDToNear(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID, pcsCharacter->m_stPos, 
			pcsAttachedGuild->m_lGuildMarkTID, pcsAttachedGuild->m_lGuildMarkColor, pcsAttachedGuild->m_lBRRanking, 
			pcsAgsdCharacter->m_dpnidCharacter);
		
		EnumCallback( AGSMGUILD_CB_SEND_GUILD_INFO, pcsCharacter, NULL );

		// 2005.04.17. steeple
		// 길드가 길드전 중이라면 여러가지를 보내야 한다.
		if( pcsGuild->IsBattle(pcsCharacter->m_szID) )
		{
			// 먼저 적 길드를 PvP 데이터에 세팅
			EnumCallback( AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE, pcsCharacter, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID );	
		}
		
		// 길드 배틀 정보 전송
		SendBattleInfo( pcsGuild, pcsAgsdCharacter->m_dpnidCharacter );

		// 연대, 적대 길드 정보 보내준다.
		SendJointAll(pcsGuild, pcsAgsdCharacter->m_dpnidCharacter);
		SendHostileAll(pcsGuild, pcsAgsdCharacter->m_dpnidCharacter);

	}		//	가입 신청중
	else
	{
		memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
		strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, pcsAttachedGuild->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		memset(pcsAttachedGuild->m_szGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));

		AgpdGuildRequestMember *pcsRequestMember = GetJoinRequestMember(pcsGuild, pcsCharacter->m_szID);
		if (pcsRequestMember)
		{
			pcsRequestMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;
			SendCreateGuild(pcsGuild,  pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
			SendSelfJoinMember(pcsAttachedGuild->m_szRequestJoinGuildID, pcsRequestMember, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
			SendCharGuildData(pcsRequestMember,  pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}

	pcsGuild->m_Mutex.Release();
	
	return TRUE;
}

BOOL AgsmGuild::Disconnect(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// 가입된 길드가 없다면 그냥 나간다.
	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("Disconnect"));

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
	if(pcsGuild)
	{
		AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
		if(pcsMember)
		{
			// AgsdGuildMember Update
			AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
			if(pcsAgsdMember)
				UpdateAgsdGuildMember(pcsAgsdMember, 0, 0);

			// 로그아웃한 Member Status 를 바꾸고
			pcsMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			// 다른 멤버들에게 업데이트를 알린다.
			SendMemberUpdateToMembers(pcsGuild, pcsMember, FALSE);

			// 길드전 점수 계산 - arycoat 2007.10.30
			if( m_pagpmGuild->IsBattleStatus(pcsGuild) )
				CalcBattleScoreByDisconnect(pcsCharacter);				
		}
		AgpdGuildRequestMember* pcsRequestMember = GetJoinRequestMember(pcsGuild, pcsCharacter->m_szID);
		if(pcsRequestMember)
		{
			pcsRequestMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			SendRequestJoinMemberUpdateToMembers(pcsGuild, pcsRequestMember , FALSE);

		}
		pcsGuild->m_Mutex.Release();
	}
	
	return TRUE;
}

// From AgsmCharManager
// Login Server 에서 캐릭터가 지워졌을 때 불린다.
BOOL AgsmGuild::CharacterDeleteComplete(CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->FindGuildLockFromCharID(szCharID);
	if(!pcsGuild)
		return TRUE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, szCharID);
	if(!pcsGuildMember)
		return FALSE;

	CHAR szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1] = {0, };
	CHAR szPassword[AGPMGUILD_MAX_PASSWORD_LENGTH+1] = {0, };

	strncpy(szGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	strncpy(szPassword, pcsGuild->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH);

	BOOL bIsMaster = m_pagpmGuild->IsMaster(pcsGuild, szCharID);
	if (bIsMaster) // 성주라면
	{
		// 배틀중이고 마스터라면 GG 로 처리해준다.
		if (m_pagpmGuild->IsBattleStatus(pcsGuild))
			BattleWithdraw(pcsGuild, szCharID);

		// 성주라면 성주 자격 박탈
		if (AgpdSiegeWar* pcsSiegeWar = m_pagpmSiegeWar->IsOwnerGuild(szGuildID))
		{
			static AgsmSiegeWar *pagsmSiegeWar = (AgsmSiegeWar*)GetModule("AgsmSiegeWar");
			
			m_pagpmSiegeWar->SetNewOwner(pcsSiegeWar, NULL);
			pagsmSiegeWar->SendPacketCastleInfoToAll(pcsSiegeWar);
		}
	}

	// Guild Unlock
	pcsGuild->m_Mutex.Release();

	switch(pcsGuildMember->m_lRank)
	{
		case AGPMGUILD_MEMBER_RANK_MASTER:
		{
			// 마스터라면 길드 해체!!!
			m_pagpmGuild->OnOperationDestroy(1, szGuildID, szCharID, szPassword, TRUE);
		} break;
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			// 서브마스터라면 서브마스터 자리 공석으로!!!
			ZeroMemory(pcsGuild->m_szSubMasterID, AGPACHARACTER_MAX_ID_STRING);
			m_pagpmGuild->OnOperationLeaveAllow(0, szGuildID, szCharID, TRUE, TRUE);
		} break;
		default:
		{
			// 그냥 길드원이면 탈퇴!!!
			m_pagpmGuild->OnOperationLeaveAllow(0, szGuildID, szCharID, TRUE, TRUE);
		}
	}

	return TRUE;
}

// Character 가 Level UP 하면 불린다.
BOOL AgsmGuild::CharacterLevelUp(AgpdCharacter* pcsCharacter, INT32 lLevelUpNum)
{
	//STOPWATCH2(GetModuleName(), _T("ChaacterLevelUp"));

	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
	if(!pcsGuild)
		return TRUE;
/*		마스터가 레벨업해도 길드 최대인원은 변하지 않는다.
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(pcsMaster)
	{
		// 마스터가 레벨업 했다!!!
		if(strcmp(pcsMaster->m_szID, pcsCharacter->m_szID) == 0)
		{
			INT32 lOldMaxMemberCount = pcsGuild->m_lMaxMemberCount;
			INT32 lNewMaxMemberCount = CalcMaxMemberCount(pcsCharacter);

			// Max Member Count 가 변경되었으면 DB 에 써준다.
			if(lOldMaxMemberCount != lNewMaxMemberCount)
				UpdateMaxMemberCount(pcsGuild, lNewMaxMemberCount);
		}
	}
*/
	// 멤버 정보를 바꿔준다.
	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if(pcsMember)
	{
		// Level 변경
		//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &pcsMember->m_lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

		// 2005.06.27. steeple
		// 진짜 거의 일어나진 않지만 레벨이 0 으로 되길래 바꿔줌.
		pcsMember->m_lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

		// 자기를 포함한 모든 멤버에게 알린다.
		SendMemberUpdateToMembers(pcsGuild, pcsMember);
	}

	AgpdGuildRequestMember* pcsRequestMember = GetJoinRequestMember(pcsGuild, pcsCharacter->m_szID);
	if(pcsRequestMember)
	{
		pcsRequestMember->m_lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

		SendRequestJoinMemberUpdateToMembers(pcsGuild,pcsRequestMember , FALSE);

	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::GuildMessage(AgpdCharacter* pcsCharacter, CHAR* szMessage, INT32 lLength)
{
	if(!pcsCharacter || !szMessage || lLength < 1 || lLength > 254)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
	if(!pcsGuild)
		return FALSE;
		
	if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		return TRUE;

	if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_MESSAGE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		return TRUE;

	AgpdGuildMember** ppcsMember;
	AgsdGuildMember* pcsAgsdMember = NULL;

	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;
			
		AgsdGuildMember* pcsAgsdMember = GetADMember(*ppcsMember);
		if(!pcsAgsdMember) continue;
		if(m_pagpmBattleGround->IsInBattleGround(pcsAgsdMember->m_pcsCharacter)) continue;
		if(pcsAgsdMember->m_pcsCharacter)
		{
			if(m_papmMap->CheckRegionPerculiarity(pcsAgsdMember->m_pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_MESSAGE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE) continue;
		}

		m_pagsmChatting->SendMessage(pcsAgsdMember->m_ulNID, AGPDCHATTING_TYPE_GUILD, pcsCharacter->m_lID, pcsCharacter->m_szID,
															NULL, szMessage, lLength);
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::GuildJointMessage(AgpdCharacter* pcsCharacter, CHAR* szMessage, INT32 lLength)
{
	if(!pcsCharacter || !szMessage || lLength < 1 || lLength > 254)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pcsAttachedGuild->m_szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		AgpdGuild* pcsJoint = NULL;
		if(_tcscmp(pcsGuild->m_szID, iter->m_szGuildID) == 0)
			pcsJoint = pcsGuild;
		else
		{
			pcsJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(!pcsJoint)
			{
				++iter;
				continue;
			}

			pcsJoint->m_Mutex.WLock();
		}

		AgpdGuildMember** ppcsMember;
		INT32 lIndex = 0;
		for(ppcsMember = (AgpdGuildMember**)pcsJoint->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
				ppcsMember = (AgpdGuildMember**)pcsJoint->m_pMemberList->GetObjectSequence(&lIndex))
		{
			if(!*ppcsMember)
				continue;

			m_pagsmChatting->SendMessage(GetMemberNID(*ppcsMember),
										AGPDCHATTING_TYPE_GUILD_JOINT,
										pcsCharacter->m_lID,
										pcsCharacter->m_szID,
										NULL,
										szMessage, lLength);
		}

		if(pcsJoint != pcsGuild)
			pcsJoint->m_Mutex.Release();

		++iter;
	}

	return TRUE;
}

BOOL AgsmGuild::IsGuildMaster(AgpdCharacter *pcsCharacter)
{
	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(NULL == pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;

	return (m_pagpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID));
}

BOOL AgsmGuild::IsSameGuild(AgpdCharacter *pcsChar1, AgpdCharacter *pcsChar2)
{
	AgpdGuildADChar *pcsGuild1 = m_pagpmGuild->GetADCharacter(pcsChar1);
	if( !pcsGuild1 )							return FALSE;
	if( strlen( pcsGuild1->m_szGuildID ) <= 0 )	return FALSE;

	AgpdGuildADChar *pcsGuild2 = m_pagpmGuild->GetADCharacter(pcsChar2);
	if( !pcsGuild2 )							return FALSE;
	if( strlen( pcsGuild2->m_szGuildID ) <= 0 )	return FALSE;

	return 0 == strcmp( pcsGuild1->m_szGuildID, pcsGuild2->m_szGuildID ) ? TRUE : FALSE;
}

UINT32 AgsmGuild::GetMemberNID(AgpdGuildMember* pcsMember)
{
	if(!pcsMember)
		return 0;

	AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
	if(!pcsAgsdMember)
		return 0;

	//STOPWATCH2(GetModuleName(), _T("GetMemberNID"));

	// 2007.04.11. steeple
	// 캐릭터 락 안하고 그냥 AgsdMember 에 있는 거 리턴하게 변경.
	return pcsAgsdMember->m_ulNID;

	// 2007.04.11. steeple
	//
	//// 2005.04.29. steeple
	//// 캐릭터 포인터를 들고 있다가 그걸로 하다보니 간간이 죽었음. 하루에 한번 정도.
	//// 나중에 길드전이 커지게 되면 더 문제 생길 지도 몰라서 그냥 그때 그때 락해서 구하는 걸로 바꿈.
	//// 이 작업이 시간이 상당히 오래 걸린다면 그때에는 다른 방법을 모색해보자.
 //   UINT32 ulNID = 0;
	//AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(pcsMember->m_szID);
	//if(pcsCharacter)
	//{
	//	ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgsdMember->m_pcsCharacter);
	//	pcsCharacter->m_Mutex.Release();
	//}

	//return ulNID;

	//// 2005.04.27. steeple
	//// 잦은 NID 버그로 인해서 그냥 캐릭터 포인터를 들고 있고 그때 그때 리턴해주기로 함.
	//if(pcsAgsdMember->m_pcsCharacter)
	//	return m_pagsmCharacter->GetCharDPNID(pcsAgsdMember->m_pcsCharacter);

	//// 그러나 없으면 원래 값으로 리턴
	//return pcsAgsdMember->m_ulNID;
}

// 2005.06.16. steeple
// 가끔 나온다던 접속 상태에 관한 버그.
// 그것을 수정하기 위해서 30초에 한번씩 리프레쉬를 해준다.
BOOL AgsmGuild::ProcessRefreshAllGuildMemberStatus(UINT32 ulClockCount)
{
	//STOPWATCH2(GetModuleName(), _T("ProcessRefreshAllGuildMemberStatus"));

	// 모든 길드를 돈다.
	AgpdGuild* pcsGuild = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	AgpdGuildRequestMember **ppcsRequestJoinMember = NULL;

	INT32 lIndex1 = 0, lIndex2 = 0;
	for(pcsGuild = m_pagpmGuild->GetGuildSequence(&lIndex1); pcsGuild; pcsGuild = m_pagpmGuild->GetGuildSequence(&lIndex1))
	{
		AuAutoLock csLock(pcsGuild->m_Mutex);
		if (!csLock.Result()) continue;

		if(pcsGuild->m_ulLastRefreshClock == 0)	// 처음 도는 것임.
		{
			pcsGuild->m_ulLastRefreshClock = ulClockCount;
			continue;
		}

		if(ulClockCount - pcsGuild->m_ulLastRefreshClock <= AGSMGUILD_IDLE_MEMBER_STATUS_REFRESH)
			continue;

		pcsGuild->m_ulLastRefreshClock = GetClockCount();		// 새로 구한다. 인자로 넘어온 놈을 쓰지 말고.

		lIndex2 = 0;
		for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if(!*ppcsMember)
				break;

			if(m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID))			// 캐릭터가 접속해있다.
				(*ppcsMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;
			else
				(*ppcsMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			SendMemberUpdateToMembers(pcsGuild, *ppcsMember);
		}
		lIndex2 = 0;
		for(ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex2); ppcsRequestJoinMember;
			ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex2))
		{
			if(!*ppcsRequestJoinMember)
				break;

			if(m_pagpmCharacter->GetCharacter((*ppcsRequestJoinMember)->m_szMemberID))			// 캐릭터가 접속해있다.
				(*ppcsRequestJoinMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;
			else
				(*ppcsRequestJoinMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			SendRequestJoinMemberUpdateToMembers(pcsGuild, *ppcsRequestJoinMember);
		}

	}

	return TRUE;
}

// 2005.04.15. steeple
// 서버에서 데이터를 바꿀 수 있도록 만들었음
BOOL AgsmGuild::SetCustomBattleTime(UINT32 ulReadyTime, UINT32 ulDurationMin, UINT32 ulDurationMax, UINT32 ulCancelEnableTime,
									INT32 lBattleNeedMemberCount, INT32 lBattleNeedLevelSum)
{
	m_ulBattleReadyTime			= ulReadyTime != 0 ? ulReadyTime : AGPMGUILD_BATTLE_READY_TIME;
	m_ulBattleCancelEnableTime	= ulCancelEnableTime != 0 ? ulCancelEnableTime : AGPMGUILD_BATTLE_CANCEL_ENABLE_TIME;
	m_lBattleNeedMemberCount	= lBattleNeedMemberCount != 0 ? lBattleNeedMemberCount : AGPMGUILD_BATTLE_NEED_MEMBER_COUNT;
	m_lBattleNeedLevelSum		= lBattleNeedLevelSum != 0 ? lBattleNeedLevelSum : AGPMGUILD_BATTLE_NEED_LEVEL_SUM;

	return TRUE;
}

// 2005.04.15. steeple
// 양쪽 길드를 다 체크한다.
// -1 로 리턴해야 성공한 것임!!!! -_-;;; 헷갈려도 소용없음~
INT32 AgsmGuild::CheckBattleEnable(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild)
{
	if(!pcsGuild || !pcsEnemyGuild)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_EXIST_GUILD;


	// 상대 길드 마스터의 상태 체크
	AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
	if(!pcsEnemyMaster || pcsEnemyMaster->m_cStatus != AGPMGUILD_MEMBER_STATUS_ONLINE)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;

	AgpdCharacter* pcsCharacterEnemy = m_pagpmCharacter->GetCharacter(pcsEnemyMaster->m_szID);
	if (!pcsCharacterEnemy)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		
	if(m_pagpmCharacter->IsOptionFlag(pcsCharacterEnemy, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE) == TRUE)		//	2005.06.01 By SungHoon
		return AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE;
		
		
	// 길드전 가능 지역 체크
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(!pcsMaster || pcsMaster->m_cStatus != AGPMGUILD_MEMBER_STATUS_ONLINE)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(pcsMaster->m_szID);
	if (!pcsCharacter)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		
	/*if (m_pagpmSiegeWar->GetSiegeWarInfo(pcsCharacter))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS;
		
	if (m_pagpmSiegeWar->GetSiegeWarInfo(pcsCharacterEnemy))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2;
		
	if(m_pagpmCharacter->IsInDungeon(pcsCharacter))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS;
		
	if(m_pagpmCharacter->IsInDungeon(pcsCharacterEnemy))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2;*/
		

	/*if(m_pagpmGuild->GetMemberCount(pcsGuild) < m_lBattleNeedMemberCount)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT;

	if(m_pagpmGuild->GetMemberLevelSum(pcsGuild) < m_lBattleNeedLevelSum)
		return AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL;

	if(m_pagpmGuild->GetMemberCount(pcsEnemyGuild) < m_lBattleNeedMemberCount)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT2;

	if(m_pagpmGuild->GetMemberLevelSum(pcsEnemyGuild) < m_lBattleNeedLevelSum)
		return AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL2;*/

	return -1;
}

// 2005.04.15. steeple
// 한 길드에서 다른 길드에게 배틀 신청을 했다.
BOOL AgsmGuild::BattleRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson)
{
	if( !szGuildID || !szMasterID || !szEnemyGuildID )
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock( szGuildID );
	if( !pcsGuild )		return FALSE;

	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if( !pcsMaster || strcmp(pcsMaster->m_szID, szMasterID) != 0 )
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	INT32 lErrorCode		= -1;
	UINT32 ulEnemyMasterNID	= 0;
	UINT32 ulMasterNID		= GetMemberNID(pcsMaster);

	// 상대할 길드를 구한다.
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID);
	if(!pcsEnemyGuild)
	{
		lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_EXIST_GUILD;
	}
	else
	{
		// 길드전 상태 체크
		if(m_pagpmGuild->IsBattleStatus(pcsGuild))
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS;
		
		if(m_pagpmGuild->IsBattleStatus(pcsEnemyGuild))
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2;
		
		if(lErrorCode == -1)
		{
			lErrorCode = CheckBattleEnable(pcsGuild, pcsEnemyGuild);
			ulEnemyMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsEnemyGuild));
		}

		pcsEnemyGuild->m_Mutex.Release();
	}
	pcsGuild->m_Mutex.Release();

	if(lErrorCode != -1)
	{
		// 에러가 발생했다.
		if (lErrorCode == AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE)
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE, ulMasterNID, szEnemyGuildID); //, NULL, 0, 0);
		else
			SendSystemMessage(lErrorCode, ulMasterNID, NULL, NULL, m_lBattleNeedMemberCount, m_lBattleNeedLevelSum);
	}
	else
	{
		pcsGuild->m_cStatus			= AGPMGUILD_STATUS_BATTLE_DECLARE; // 전쟁 신청
		ZeroMemory(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
		strncpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsEnemyGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		
		pcsEnemyGuild->m_cStatus	= AGPMGUILD_STATUS_BATTLE_DECLARE; // 전쟁 신청	
		ZeroMemory(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
		strncpy(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		
		SendBattleRequest(szEnemyGuildID, szGuildID, lType, ulDuration, ulPerson, ulEnemyMasterNID);

		// 길드전 신청한 사람에게 신청했다고 알려준다.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_REQUEST, ulMasterNID, NULL, NULL, 0, 0);
	}

	return TRUE;
}

// 2005.04.16. steeple
// 배틀을 수락했다. 여기서 검사를 통과하면 배틀 list 에서 관리하게 된다.
// pcsGuild 는 Lock 되어서 넘어온다.
BOOL AgsmGuild::BattleAccept(CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson)
{
	if( !szGuildID || !szEnemyGuildID )	return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if(!pcsGuild)		return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID, pLock2);
	if(!pcsEnemyGuild)	return FALSE;
	

	if( !m_pagpmGuild->IsBattleDeclareStatus(pcsGuild) || strcmp(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsEnemyGuild->m_szID) != 0 ) 
		return FALSE;

	if( !m_pagpmGuild->IsBattleDeclareStatus(pcsEnemyGuild) || strcmp(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild->m_szID) != 0)
	{
		m_pagpmGuild->InitCurrentBattleInfo(pcsGuild);
		return FALSE;	
	}

	// 다시한번 체크해준다.
	INT32 lErrorCode = CheckBattleEnable(pcsGuild, pcsEnemyGuild);
	if(lErrorCode == -1)
	{
		if( !m_pagpmGuild->CheckBattleType( pcsGuild, pcsEnemyGuild, (eGuildBattleType)lType ) )
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_BOTH_MEMBER_COUNT;
			
		AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
		if(!pcsEnemyMaster || pcsEnemyMaster->m_cStatus != AGPMGUILD_MEMBER_STATUS_ONLINE)
		{
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		}
	}
	
	if(lErrorCode != -1)	// 2006.04.19. steeple. -1 이 아니면 에러다. ㅜㅜ
	{
		UINT32 ulNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild));

		if(lErrorCode != AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE)
			SendSystemMessage(lErrorCode, ulNID, szEnemyGuildID); //, NULL, 0, 0);

		return FALSE;
	}

	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	if( AddBattleProcess( szGuildID, szEnemyGuildID, AGPMGUILD_STATUS_BATTLE_READY, ulCurrentTime, m_ulBattleReadyTime, 
							ulCurrentTime + m_ulBattleReadyTime, lType, ulDuration, ulPerson ) )
	{
		// 서로 적 길드를 세팅해주고
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID);
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, pcsEnemyGuild->m_szMasterID);
		strcpy(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szGuildID);
		strcpy(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, pcsGuild->m_szMasterID);

		// 배틀 데이터를 세팅한다.
		pcsGuild->m_cStatus = pcsEnemyGuild->m_cStatus	= AGPMGUILD_STATUS_BATTLE_READY;
		pcsGuild->m_csCurrentBattleInfo.m_eType			= pcsEnemyGuild->m_csCurrentBattleInfo.m_eType			= (eGuildBattleType)lType;
		pcsGuild->m_csCurrentBattleInfo.m_ulPerson		= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulPerson		= ulPerson;
		pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulAcceptTime	= ulCurrentTime;
		pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulReadyTime	= m_ulBattleReadyTime;
		pcsGuild->m_csCurrentBattleInfo.m_ulStartTime	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulStartTime	= ulCurrentTime + m_ulBattleReadyTime;
		pcsGuild->m_csCurrentBattleInfo.m_ulDuration	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulDuration		= ulDuration;
		pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime = pcsEnemyGuild->m_csCurrentBattleInfo.m_ulCurrentTime	= ulCurrentTime;

		// 패킷을 보내준다.
		SendBattleAcceptToAllMembers(pcsGuild);
		SendBattleAcceptToAllMembers(pcsEnemyGuild);

		// DB 저장은 하지 않는다.
		//EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);
		//EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsEnemyGuild, NULL);
		
		// Log
		/*
		if (m_pagpmLog)
		{
			AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
			m_pagpmLog->WriteLog_GuildBattleAccept();
		}
		*/
	}
	else
	{
		// 음 이러면 이미 배틀 리스트에 있는 건데 이 함수를 탔다는 것이 문제가 있음 -0-
	}

	return TRUE;
}

// 2005.04.16. steeple
// 배틀을 거절했다. 그냥 거절 메시지만 보내주면 될듯.
BOOL AgsmGuild::BattleReject(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if( !szGuildID || !szMasterID || !szEnemyGuildID )		return FALSE;
	
	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if(!pcsGuild) return FALSE;
	if( !m_pagpmGuild->IsMaster(szGuildID, szMasterID) )	return FALSE;
		
	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID, pLock2);
	if( !pcsEnemyGuild )		return FALSE;

	AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
	if( pcsEnemyMaster )
	{
		SendSystemMessage( AGPMGUILD_SYSTEM_CODE_BATTLE_REJECT_BY_OTHER, GetMemberNID(pcsEnemyMaster), szGuildID );
	}

	if( m_pagpmGuild->IsBattleDeclareStatus(pcsGuild) )
		m_pagpmGuild->InitCurrentBattleInfo(pcsGuild);

	if( m_pagpmGuild->IsBattleDeclareStatus(pcsEnemyGuild) )
		m_pagpmGuild->InitCurrentBattleInfo(pcsEnemyGuild);

	return TRUE;
}

// 2005.04.16. steeple
// 배틀 취소를 신청한다. 다가오는 시작시간이 1분 미만이라면 불가능하다.
BOOL AgsmGuild::BattleCancelRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if( !szGuildID || !szMasterID || !szEnemyGuildID )		return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if( !pcsGuild )		return FALSE;

	// 마스터가 아니면 GG
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if( !pcsMaster || strcmp( pcsMaster->m_szID, szMasterID ) )
		return FALSE;
		
	if(m_pagpmGuild->IsBattleDeclareStatus(pcsGuild))
	{
		AuAutoLock pLock3;
		AgpdGuild* pcsGuild3 = m_pagpmGuild->GetGuildLock(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pLock3);
		if( pcsGuild3 )
		{
			m_pagpmGuild->InitCurrentBattleInfo(pcsGuild3);
			SendBattleInfoToAllMembers( pcsGuild3 );
		}
		
		m_pagpmGuild->InitCurrentBattleInfo(pcsGuild);
		SendBattleInfoToAllMembers( pcsGuild );
		
		return TRUE;
	}

	if( !m_pagpmGuild->IsEnemyGuild( pcsGuild, szEnemyGuildID ) )
		return FALSE;

	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	if(pcsGuild->m_csCurrentBattleInfo.m_ulStartTime - ulCurrentTime < m_ulBattleCancelEnableTime)
	{
		// 취소할 수 있는 시간이 지나버렸음
		SendSystemMessage( AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_PASSED_TIME, GetMemberNID(pcsMaster), NULL, NULL, m_ulBattleCancelEnableTime );

		return FALSE;
	}

	// 상대편 길마에게 취소신청을 한다.
	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID, pLock2);
	if(pcsEnemyGuild)
	{
		AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
		if(pcsEnemyMaster)
		{
			SendBattleCancelRequest(szEnemyGuildID, pcsEnemyMaster->m_szID, szGuildID, GetMemberNID(pcsEnemyMaster));
		}
		else
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE, GetMemberNID(pcsMaster));
		}
	}

	return TRUE;
}

/// 2005.04.16. steeple
BOOL AgsmGuild::BattleCancelAccept(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// 마스터가 아니면 GG
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(!pcsMaster || strcmp(pcsMaster->m_szID, szMasterID) != 0)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	if(!m_pagpmGuild->IsEnemyGuild(pcsGuild, szEnemyGuildID))
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID);
	if(!pcsEnemyGuild)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// Battle List Lock
	BattleListLock();
	if( RemoveBattleProcessList( pcsGuild->m_szID, pcsEnemyGuild->m_szID ) )
	{
		// 얘를 가장 먼저 보내야 한다.
		SendBattleCancelAcceptToAllMembers( pcsGuild );
		SendBattleCancelAcceptToAllMembers( pcsEnemyGuild );

		// Guild Battle 정보를 보낸다.
		SendBattleInfoToAllMembers( pcsGuild );
		SendBattleInfoToAllMembers( pcsEnemyGuild );
	}
	BattleListUnlock();

	// 초기화 해버리고
	m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
	m_pagpmGuild->InitCurrentBattleInfo( pcsEnemyGuild );

	pcsEnemyGuild->m_Mutex.Release();
	pcsGuild->m_Mutex.Release();

	return TRUE;
}

/// 2005.04.16. steeple
BOOL AgsmGuild::BattleCancelReject(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	// 까짓것 한번 확인해준다.
	if(!m_pagpmGuild->IsMaster(szGuildID, szMasterID))
		return FALSE;

	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID);
	if(!pcsEnemyGuild)
		return FALSE;

	AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
	if(pcsEnemyMaster)
	{
		// 달랑 시스템 메시지 보내주고 아무일 없었다는 듯 끝낸다.
		// 고로 계속 배틀 레뒤 상태가 지속되는 것임.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_REJECT_BY_OTHER, GetMemberNID(pcsEnemyMaster), szGuildID);
	}

	pcsEnemyGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::AddBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2, INT8 cStatus, UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime, INT32 lType, UINT32 ulDuration, UINT32 lPerson)
{
	if( !szGuildID1 || !szGuildID2 )		return FALSE;

	BattleListLock();

	// 이미 안에 있다. 
	if( GetBattleProcess( szGuildID1, szGuildID2 ) )
	{
		BattleListUnlock();
		return FALSE;
	}

	AgsdGuildBattleProcess* pcsBattleProcess = (AgsdGuildBattleProcess*)CreateModuleData( AGSMGUILD_DATA_TYPE_BATTLE_PROCESS );
	if(!pcsBattleProcess)
	{
		BattleListUnlock();
		return FALSE;
	}

	pcsBattleProcess->m_eType			= (eGuildBattleType)lType;
	pcsBattleProcess->m_ulPerson		= lPerson;
	pcsBattleProcess->m_cStatus			= cStatus;
	pcsBattleProcess->m_ulLastCheckTime	= 0;
	pcsBattleProcess->m_ulAcceptTime	= ulAcceptTime;
	pcsBattleProcess->m_ulReadyTime		= ulReadyTime;
	pcsBattleProcess->m_ulStartTime		= ulStartTime;
	pcsBattleProcess->m_ulDuration		= ulDuration;
	
	ZeroMemory(pcsBattleProcess->m_szGuildID1, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
	ZeroMemory(pcsBattleProcess->m_szGuildID2, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
	strncpy( pcsBattleProcess->m_szGuildID1, szGuildID1, AGPMGUILD_MAX_GUILD_ID_LENGTH );
	strncpy( pcsBattleProcess->m_szGuildID2, szGuildID2, AGPMGUILD_MAX_GUILD_ID_LENGTH );

	m_listBattleProcess.push_back(pcsBattleProcess);

	BattleListUnlock();

	return TRUE;
}

// 2005.04.17. steeple
// Battle List Lock 되어 있는 상태에서 불린다.
BOOL AgsmGuild::RemoveBattleProcess(AgsdGuildBattleProcess* pcsGuildBattleProcess)
{
	if( !pcsGuildBattleProcess )		return FALSE;

	return DestroyModuleData(pcsGuildBattleProcess, AGSMGUILD_DATA_TYPE_BATTLE_PROCESS);
}

// 2005.04.17. steeple
// Battle List Lock 되어 있는 상태에서 불린다.
BOOL AgsmGuild::RemoveBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2)
{
	if( !szGuildID1 || !szGuildID2 )	return FALSE;

	AgsdGuildBattleProcess* pcsBattleProcess = GetBattleProcess(szGuildID1, szGuildID2);
	if( !pcsBattleProcess )		return FALSE;

	return RemoveBattleProcess(pcsBattleProcess);
}

// 2005.04.17. steeple
// Battle List Lock 되어 있는 상태에서 불린다.
BOOL AgsmGuild::RemoveBattleProcessList(CHAR* szGuildID1, CHAR* szGuildID2)
{
	if( !szGuildID1 || !szGuildID2 )	return FALSE;

	BOOL bRemove = FALSE;

	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); ++Itr )
	{
		if( !strcmp( (*Itr)->m_szGuildID1, szGuildID1) && !strcmp( (*Itr)->m_szGuildID2, szGuildID2) )
		{
			RemoveBattleProcess( *Itr );
			m_listBattleProcess.erase( Itr );
			bRemove = TRUE;
			break;
		}

		if( !strcmp( (*Itr)->m_szGuildID2, szGuildID1 ) && !strcmp( (*Itr)->m_szGuildID1, szGuildID2 ) )
		{
			RemoveBattleProcess( *Itr );
			m_listBattleProcess.erase( Itr );
			bRemove = TRUE;
			break;
		}
	}

	return bRemove;
}

// 2005.04.17. steeple
// Battle List Lock 되어 있는 상태에서 불린다.
AgsdGuildBattleProcess* AgsmGuild::GetBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2)
{
	if( !szGuildID1 || !szGuildID2 )	return FALSE;

	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); ++Itr )
	{
		if( !strcmp( (*Itr)->m_szGuildID1, szGuildID1 ) && !strcmp( (*Itr)->m_szGuildID2, szGuildID2 ) )
			return (*Itr);

		if( !strcmp( (*Itr)->m_szGuildID2, szGuildID1 ) && !strcmp( (*Itr)->m_szGuildID1, szGuildID2 ) )
			return (*Itr);
	}

	return NULL;

}

// 2005.04.17. steeple
// Idle 에서 불러준다.
BOOL AgsmGuild::ProcessAllBattle(UINT32 ulClockCount)
{
	if(m_ulLastProcessBattleClock != 0)
	{
		// 1초에 한번씩 돈다.
		if(ulClockCount - m_ulLastProcessBattleClock < AGSMGUILD_IDLE_BATTLE_PROCESS_INTERVAL)
			return TRUE;
	}

	//STOPWATCH2(GetModuleName(), _T("ProcessAllBattle"));

	m_ulLastProcessBattleClock = ulClockCount;

	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();

	BattleListLock();
	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); )
	{
		if( !ProcessBattle( *Itr, ulCurrentTime ) )	
		{
			ClearBattleProcessEachGuild( *Itr );
			RemoveBattleProcess( *Itr );
			Itr = m_listBattleProcess.erase(Itr);
		}
		else
			++Itr;
	}
	BattleListUnlock();

	return TRUE;
}

// 2005.04.17. steeple
// 시간 대별 처리를 해주고, 시간이 다 끝난 경우에는 FALSE 리턴해준다. 그러면 list 에서 빠지게 된다.
// BattleProcess 의 List 는 Lock 되어 있다.
BOOL AgsmGuild::ProcessBattle(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if( !pcsBattleProcess )		return FALSE;

	pcsBattleProcess->m_ulLastCheckTime = ulCurrentTime;

	if( IsBattleStart( pcsBattleProcess, ulCurrentTime ) )
		return BattleStart( pcsBattleProcess, ulCurrentTime );
	else if( IsBattleEnd( pcsBattleProcess, ulCurrentTime ) )
		return BattleEnd( pcsBattleProcess, ulCurrentTime );

	return BattleUpdateTime( pcsBattleProcess, ulCurrentTime );
}

// 2005.06.27. steeple
// 길드전 중간에 어떠한 원인인지는 모르겠찌만... 길드전이 안끝나는 현상이 두달만에 발생하였다.
// 코드를 아무리 살펴봐도 특별한 원인을 못찾겠어서, 그냥 Exception 처리를 하게끔 변경.
//
// 여기서 원인은 배틀 중에 하나의 길드가 사라진 것으로 판단하고 그거에 대한 처리를 해줌.
// 그래서 만약에 그러한 일이 발생하더라도 정상으로 복구 해주는 코드임.
//
// 이 함수를 부르는 곳에서는 이 함수를 부른후, pcsBattleProcess 를 Battle List 에서 빼줘야 한다. (지금은 빼주고 있음)
// 
// BattleProcess 의 List 는 Lock 되어 있다.
BOOL AgsmGuild::ProcessBattleException(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessBattleException"));

	switch(pcsBattleProcess->m_cStatus)
	{
		case AGPMGUILD_STATUS_BATTLE_READY:
		{
			// 이 상태에서는 그냥 끝내준다.
			AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
			if(pcsGuild)
			{
				SendBattleCancelAcceptToAllMembers( pcsGuild );
				m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
				SendBattleInfoToAllMembers( pcsGuild );
				pcsGuild->m_Mutex.Release();
			}

			pcsGuild = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
			if( pcsGuild )
			{
				SendBattleCancelAcceptToAllMembers( pcsGuild );
				m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
				SendBattleInfoToAllMembers( pcsGuild );
				pcsGuild->m_Mutex.Release();
			}

			break;
		}

		case AGPMGUILD_STATUS_BATTLE:
		{
			// 여기서는 승자와 패자를 구별해줘야 함.
			AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
			AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
			
			AgpdGuild* pcsWinner = NULL, *pcsLoser = NULL;
			if(pcsGuild1 && !pcsGuild2)
			{
				pcsWinner = pcsGuild1;
				pcsLoser = pcsGuild2;
			}
			else if(!pcsGuild1 && pcsGuild2)
			{
				pcsWinner = pcsGuild2;
				pcsLoser = pcsGuild1;
			}
			else if(!pcsGuild1 && !pcsGuild2)
				return TRUE;

			if(pcsWinner)
			{
				pcsWinner->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW;
				pcsWinner->m_lWin++;
				SendBattleEndToAllMembers(pcsWinner);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsWinner, NULL);	// 뒤에다가 NULL 줘도 된다. 2005.06.27. 기준
				RemoveEnemyGuildToAllMembers(pcsWinner, pcsWinner->m_csCurrentBattleInfo.m_szEnemyGuildID);
				m_pagpmGuild->InitCurrentBattleInfo(pcsWinner);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsWinner, NULL);
				SendBattleInfoToAllMembers(pcsWinner);
			}

			if(pcsLoser)
			{
				pcsLoser->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW;
				pcsLoser->m_lLose++;
				SendBattleEndToAllMembers(pcsLoser);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsLoser, NULL);	// 뒤에다가 NULL 줘도 된다. 2005.06.27. 기준
				RemoveEnemyGuildToAllMembers(pcsLoser, pcsLoser->m_csCurrentBattleInfo.m_szEnemyGuildID);
				m_pagpmGuild->InitCurrentBattleInfo(pcsLoser);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsLoser, NULL);
				SendBattleInfoToAllMembers(pcsLoser);
			}

			if( pcsGuild1 )
				pcsGuild1->m_Mutex.Release();
			if( pcsGuild2 )
				pcsGuild2->m_Mutex.Release();

			break;
		}
	}

	return TRUE;
}

BOOL AgsmGuild::IsBattleStart(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if( pcsBattleProcess->m_cStatus == AGPMGUILD_STATUS_BATTLE_READY && ulCurrentTime > pcsBattleProcess->m_ulStartTime )
		return TRUE;

	return FALSE;
}

//각타입별로 끝나는 의미가 다르다..
BOOL AgsmGuild::IsBattleEnd(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if( !pcsBattleProcess )		return FALSE;

	switch( pcsBattleProcess->m_eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleTotalSurvive:
	case eGuildBattleDeadMatch:
		if( AGPMGUILD_STATUS_BATTLE == pcsBattleProcess->m_cStatus && ulCurrentTime > pcsBattleProcess->m_ulStartTime + pcsBattleProcess->m_ulDuration )
			return TRUE;
		break;
	case eGuildBattlePrivateSurvive:
		{
			if( AGPMGUILD_STATUS_BATTLE != pcsBattleProcess->m_cStatus )
				return FALSE;
				
			if( ulCurrentTime > pcsBattleProcess->m_ulStartTime + pcsBattleProcess->m_ulDuration )
				return TRUE;

			AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
			if( pcsGuild1 )
			{
				/*UINT32 ulHalfPerson = (UINT32)((float)pcsBattleProcess->m_ulPerson * 0.5f);
				if( pcsGuild1->GetKillAll() > ulHalfPerson || pcsGuild1->GetDeathAll() > ulHalfPerson )
					return TRUE;*/
				
				if( pcsBattleProcess->m_ulPerson <= pcsGuild1->m_csCurrentBattleInfo.m_ulRound )	//라운드의 숫자가 커지면..
					return TRUE;
			}
		} break;
	}
	
	return FALSE;
}

// 2005.04.17. steeple
// Battle 시작 시간이 되었다. 시작해주자!!
BOOL AgsmGuild::BattleStart(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	// 양쪽 길드를 구한다.
	AuAutoLock pLock1;
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1, pLock1);
	if(!pcsGuild1)
	{
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	AuAutoLock pLock2;
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2, pLock2);
	if(!pcsGuild2)
	{
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_BATTLE;
	
	pcsGuild1->BattleStart( ulCurrentTime );
	pcsGuild2->BattleStart( ulCurrentTime );
	
	AddEnemyGuildToAllMembers(pcsGuild1, pcsGuild2->m_szID);
	AddEnemyGuildToAllMembers(pcsGuild2, pcsGuild1->m_szID);

	//기존적인 정보들을 날려주자..
	SendBattleMemberListInfoAllMembers(pcsGuild1, pcsGuild2);
	SendBattleMemberListInfoAllMembers(pcsGuild2, pcsGuild1);

	SendBattleStartToAllMembers(pcsGuild1);
	SendBattleStartToAllMembers(pcsGuild2);
	
	if(pcsGuild1->m_csCurrentBattleInfo.m_eType == eGuildBattlePrivateSurvive)
		BattleUpdatePrivateMode( pcsGuild1, pcsGuild2 );

	// DB 저장
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild1, NULL);
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild2, NULL);

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::BattleEnd(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("BattleEnd"));

	// 양쪽 길드를 구한다.
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
	if(!pcsGuild1)
	{
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
	if(!pcsGuild2)
	{
		pcsGuild1->m_Mutex.Release();
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_NONE;
	pcsGuild1->BattleEnd( ulCurrentTime );
	pcsGuild2->BattleEnd( ulCurrentTime );
	pcsGuild1->BattleResult( pcsGuild2 );

	AgpdGuild* pcsWinGuild = NULL;
	AgpdGuild* pcsLoseGuild = NULL;
	switch( pcsGuild1->m_csCurrentBattleInfo.m_cResult )
	{
	case AGPMGUILD_BATTLE_RESULT_WIN:
		pcsWinGuild		= pcsGuild1;
		pcsLoseGuild	= pcsGuild2;
		break;
	case AGPMGUILD_BATTLE_RESULT_LOSE:
		pcsWinGuild		= pcsGuild2;
		pcsLoseGuild	= pcsGuild1;
		break;
	case AGPMGUILD_BATTLE_RESULT_DRAW:
		pcsWinGuild		= pcsGuild1;
		pcsLoseGuild	= pcsGuild2;
		break;
	}
	
	BattleEndProcess(pcsWinGuild, pcsLoseGuild);
	
	pcsGuild2->m_Mutex.Release();
	pcsGuild1->m_Mutex.Release();

	return FALSE;
}

BOOL AgsmGuild::BattleEndProcess(AgpdGuild* pcsWinGuild, AgpdGuild* pcsLoseGuild)
{
	if(!pcsWinGuild || !pcsLoseGuild)
		return FALSE;
		
	INT32 myUpPoint		= pcsWinGuild->m_csCurrentBattleInfo.GetGuildPoint();
	INT32 enemyUpPoint	= pcsLoseGuild->m_csCurrentBattleInfo.GetGuildPoint();

	pcsWinGuild->m_csCurrentBattleInfo.m_lEnemyUpPoint	= enemyUpPoint;
	pcsLoseGuild->m_csCurrentBattleInfo.m_lEnemyUpPoint	= myUpPoint;

	pcsWinGuild->m_lGuildPoint		+= myUpPoint;
	pcsLoseGuild->m_lGuildPoint		+= enemyUpPoint;
	
	// 처리하는 순서가 중요하다. 
	// DB 저장 - History 를 먼저 저장하고 - 이긴놈을 pData 에 넣고 진 놈을 pCustData 에 넣는다. 비긴 경우는 상관없음.
	// 진놈 기준으로도 한번더 저장한다.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsWinGuild, pcsLoseGuild);
	EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsLoseGuild, pcsWinGuild);

	SendBattleMemberListInfoAllMembers( pcsWinGuild, pcsLoseGuild );
	SendBattleMemberListInfoAllMembers( pcsLoseGuild, pcsWinGuild );

	SendBattleEndToAllMembers( pcsWinGuild );
	SendBattleEndToAllMembers( pcsLoseGuild );
	
	RemoveEnemyGuildToAllMembers( pcsWinGuild, pcsLoseGuild->m_szID );
	RemoveEnemyGuildToAllMembers( pcsLoseGuild, pcsWinGuild->m_szID );

	// 모든 길드원 PvP 정보에 EnemyGuild 를 빼준다.
	/*switch( pcsWinGuild->m_csCurrentBattleInfo.m_eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		RemoveEnemyGuildToAllMembers( pcsWinGuild, pcsLoseGuild->m_szID );
		RemoveEnemyGuildToAllMembers( pcsLoseGuild, pcsWinGuild->m_szID );
		break;
	case eGuildBattleTotalSurvive:
		RemoveEnemyGuildToBattleAllMembers( pcsWinGuild, pcsLoseGuild );
		RemoveEnemyGuildToBattleAllMembers( pcsLoseGuild, pcsWinGuild );
		break;
	case eGuildBattlePrivateSurvive:
		break;
	}*/

	// 데이터를 비우고
	m_pagpmGuild->InitCurrentBattleInfo( pcsWinGuild );
	m_pagpmGuild->InitCurrentBattleInfo( pcsLoseGuild );
	
	// DB 저장
	EnumCallback( AGSMGUILD_CB_DB_GUILD_UPDATE, pcsWinGuild, NULL );
	EnumCallback( AGSMGUILD_CB_DB_GUILD_UPDATE, pcsLoseGuild, NULL );

	// Guild Battle 정보를 한번 더 슝 보낸다.
	SendBattleInfoToAllMembers( pcsWinGuild );
	SendBattleInfoToAllMembers( pcsLoseGuild );
	
	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::BattleUpdateTime(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	// 양쪽 길드를 구한다.
	AuAutoLock pLock1;
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1, pLock1);
	if(!pcsGuild1)
		return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2, pLock2);
	if(!pcsGuild2)
		return FALSE;

	pcsGuild1->BattleUpdate( ulCurrentTime );
	pcsGuild2->BattleUpdate( ulCurrentTime );

	SendBattleUpdateTimeToAllMembers(pcsGuild1);
	SendBattleUpdateTimeToAllMembers(pcsGuild2);

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::ClearBattleProcessEachGuild(AgsdGuildBattleProcess* pcsBattleProcess)
{
	if(!pcsBattleProcess)
		return FALSE;

	// 양쪽 길드를 구한다.
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
	if(!pcsGuild1)
		return FALSE;

	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
	if(!pcsGuild2)
	{
		pcsGuild1->m_Mutex.Release();
		return FALSE;
	}

	BOOL bIsNeedUpdateDBGuild1 = strlen(pcsGuild1->m_csCurrentBattleInfo.m_szEnemyGuildID) > 0 ? TRUE : FALSE;
	BOOL bIsNeedUpdateDBGuild2 = strlen(pcsGuild2->m_csCurrentBattleInfo.m_szEnemyGuildID) > 0 ? TRUE : FALSE;

	m_pagpmGuild->InitCurrentBattleInfo(pcsGuild1);
	m_pagpmGuild->InitCurrentBattleInfo(pcsGuild2);

	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_NONE;

	SendBattleInfoToAllMembers(pcsGuild1);
	SendBattleInfoToAllMembers(pcsGuild2);

	// DB 저장
	if( bIsNeedUpdateDBGuild1 )		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild1, NULL);
	if( bIsNeedUpdateDBGuild2 )		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild2, NULL);
		
	pcsGuild2->m_Mutex.Release();
	pcsGuild1->m_Mutex.Release();

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::AddEnemyGuildToAllMembers(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID)
{
	if(!pcsGuild || !szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		//if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;

		pcsCharacter = m_pagpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
		if(pcsCharacter)
		{
			// AgsmPvP 로 가서 EnemyGuildID 를 적 길드로 추가하게 된다.
			EnumCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE, pcsCharacter, szEnemyGuildID);
			pcsCharacter->m_Mutex.Release();
		}
	}

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::RemoveEnemyGuildToAllMembers(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID)
{
	if(!pcsGuild || !szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		//if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;	

		pcsCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember)->m_szID );
		if( pcsCharacter )
		{
			EnumCallback( AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_BATTLE, pcsCharacter, szEnemyGuildID );
			pcsCharacter->m_Mutex.Release();
		}
	}

	return TRUE;
}

BOOL AgsmGuild::AddEnemyGuildToBattleAllMembers(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild)
{
	if( !pcsGuild || !pcsEnemyGuild )	return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	AgpdGuildMember** ppcsMember2 = NULL;
	INT32 lIndex2 = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;	

		pcsCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember)->m_szID );
		if( !pcsCharacter )	continue;

		//적길드의 인원들을. 쭉 보내주자꾸나 ^^
		lIndex2 = 0;
		for( ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2);
			 ppcsMember2;
			 ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if( !pcsEnemyGuild->IsBattle( *ppcsMember2 ) )	continue;

			AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember2)->m_szID );
			if( !pcsEnemyCharacter )						continue;

			EnumCallback( AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
			pcsEnemyCharacter->m_Mutex.Release();
		}

		pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmGuild::RemoveEnemyGuildToBattleAllMembers(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild)
{
	if( !pcsGuild || !pcsEnemyGuild )	return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	AgpdGuildMember** ppcsMember2 = NULL;
	INT32 lIndex2 = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;	

		pcsCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember)->m_szID );
		if( !pcsCharacter )	continue;

		//적길드의 인원들을. 쭉 보내주자꾸나 ^^
		lIndex2 = 0;
		for( ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2);
			 ppcsMember2;
			 ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if( !pcsEnemyGuild->IsBattle( *ppcsMember2 ) )	continue;

			AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember2)->m_szID );
			if( !pcsEnemyCharacter )						continue;

			EnumCallback( AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
			pcsEnemyCharacter->m_Mutex.Release();
		}

		pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmGuild::AddEnemyGuildToBattleMembers(CHAR* szMyID, CHAR* szEnemyID)
{
	if( !szMyID || !szEnemyID )	return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock( szMyID );
	if( !pcsCharacter )			return FALSE;

	AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( szEnemyID );
	if( pcsEnemyCharacter )
	{
		EnumCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
		pcsEnemyCharacter->m_Mutex.Release();
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::RemoveEnemyGuildToBattleMembers(CHAR* szMyID, CHAR* szEnemyID)
{
	if( !szMyID || !szEnemyID )	return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock( szMyID );
	if( !pcsCharacter )			return FALSE;

	AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( szEnemyID );
	if( pcsEnemyCharacter )
	{
		EnumCallback(AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
		pcsEnemyCharacter->m_Mutex.Release();
	}
	
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::BattleUpdatePrivateMode(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if( !pcsGuild1 || !pcsGuild2 )		return FALSE;
	if( eGuildBattlePrivateSurvive != pcsGuild1->m_csCurrentBattleInfo.m_eType )
		return FALSE;

	while( 1 )
	{
		++pcsGuild1->m_csCurrentBattleInfo.m_ulRound;
		++pcsGuild2->m_csCurrentBattleInfo.m_ulRound;

		//전체 사람숫자와.. 현재 라운스의드 의 숫자비교..

		AgpdGuildMember* pMember1 = pcsGuild1->GetCurRoundMember();
		AgpdGuildMember* pMember2 = pcsGuild2->GetCurRoundMember();
		if( pMember1 && pMember2 )
		{
			AgpdCharacter* pcsCharacter1 = m_pagpmCharacter->GetCharacter(pMember1->m_szID);
			AgpdCharacter* pcsCharacter2 = m_pagpmCharacter->GetCharacter(pMember2->m_szID);
			
			if( pcsCharacter1 && pcsCharacter2 )
			{
				SendBattleRoundToAllMembers(pcsGuild1);
				SendBattleRoundToAllMembers(pcsGuild2);
				return TRUE;
			}
			
			if( pcsCharacter1 && !pcsCharacter2 )
			{
				CalcBattleScoreProcess(pcsGuild1, pcsCharacter1, pcsGuild2, pcsCharacter2);
			}
			else if( !pcsCharacter1 && pcsCharacter2 )
			{
				CalcBattleScoreProcess(pcsGuild2, pcsCharacter2, pcsGuild1, pcsCharacter1);
			}
		}
		else
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmGuild::CalcBattleScoreProcess(AgpdGuild* pcsGuild1, AgpdCharacter* pcsWinner, AgpdGuild* pcsGuild2, AgpdCharacter* pcsLoser)
{
	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;
		
	INT32 lUpScore = 1;
	if( pcsWinner )
	{
		if(pcsGuild1->m_csCurrentBattleInfo.m_eType == eGuildBattlePointMatch)
		{
			INT32 lAttackerLevel	= m_pagpmCharacter->GetLevel(pcsWinner);
			INT32 lDeadLevel		= m_pagpmCharacter->GetLevel(pcsLoser);

			// 레벨 차이는 진놈에서 이긴놈을 빼서 준다.	
			lUpScore = m_pagpmGuild->GetBattlePoint(lDeadLevel - lAttackerLevel);
		}

		AgpdGuildMember* pGuildAttack = pcsGuild1->GetMember( pcsWinner->m_szID );
		if( pGuildAttack )
		{
			pGuildAttack->m_ulKill++;
			pGuildAttack->m_ulScore += lUpScore;

			//우리편과 상대방에게 Update된 정보를 쏴주자 ^^
			SendBattleMemberInfoAllMembers(pcsGuild1, pcsGuild1->m_szID, pGuildAttack );
			SendBattleMemberInfoAllMembers(pcsGuild2, pcsGuild1->m_szID, pGuildAttack );
		}
	}
	
	if( pcsLoser )
	{
		AgpdGuildMember* pGuildDead = pcsGuild2->GetMember( pcsLoser->m_szID );
		if( pGuildDead )
			pGuildDead->m_ulDeath++;	
	}

	pcsGuild1->m_csCurrentBattleInfo.m_lMyScore			+= lUpScore;
	pcsGuild1->m_csCurrentBattleInfo.m_lMyUpScore		= lUpScore;

	pcsGuild2->m_csCurrentBattleInfo.m_lEnemyScore		+= lUpScore;
	pcsGuild2->m_csCurrentBattleInfo.m_lEnemyUpScore	= lUpScore;

	// 보낸다.
	SendBattleUpdateScoreToAllMembers(pcsGuild1);
	SendBattleUpdateScoreToAllMembers(pcsGuild2);

	pcsGuild1->m_csCurrentBattleInfo.m_lMyUpScore		= 0;
	pcsGuild2->m_csCurrentBattleInfo.m_lEnemyUpScore	= 0;

	// 점수 올라갈 때마다 DB 에 써야 한다.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild1, NULL);
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild2, NULL);
	
	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::CalcBattleScore(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsDead)
{
	if(!pcsAttacker || !pcsDead)
		return FALSE;

	CHAR* szGuildID1 = m_pagpmGuild->GetJoinedGuildID(pcsAttacker);
	CHAR* szGuildID2 = m_pagpmGuild->GetJoinedGuildID(pcsDead);
	if(!szGuildID1 || !szGuildID2)
		return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(szGuildID1, pLock1);
	if(!pcsGuild1)
		return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(szGuildID2, pLock2);
	if(!pcsGuild2)
		return FALSE;

	// 서로 적이고, 상태가 BATTLE 중 일때만 하면 된다.
	if(!m_pagpmGuild->IsBattleIngStatus(pcsGuild1) || !m_pagpmGuild->IsBattleIngStatus(pcsGuild2) ||
		strcmp(pcsGuild1->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild2->m_szID) != 0 ||
		strcmp(pcsGuild2->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild1->m_szID) != 0)
		return FALSE;
		
	if( !pcsGuild1->IsBattle( pcsAttacker->m_szID ) || !pcsGuild2->IsBattle( pcsDead->m_szID ) )
		return FALSE;

	if( !pcsGuild1->IsPrivateSurvivePlayer( pcsGuild2, pcsAttacker->m_szID, pcsDead->m_szID ) )
		return FALSE;
		
	// 공성존에서는 길드전 룰 적용 안됨
	/*if( m_pagpmSiegeWar->GetSiegeWarInfo(pcsAttacker) || m_pagpmSiegeWar->GetSiegeWarInfo(pcsDead) )
		return FALSE;*/
	
	// 던전서는 길드전 룰 적용 안됨	
	/*if( m_pagpmCharacter->IsInDungeon(pcsAttacker) || m_pagpmCharacter->IsInDungeon(pcsDead) )
		return FALSE;*/
		
	if( pcsGuild1->m_csCurrentBattleInfo.m_eType == eGuildBattleDeadMatch ) //길마 많이 죽이기
	{
		if (!m_pagpmGuild->IsMaster(pcsGuild2, pcsDead->m_szID))
			return TRUE;	// 길드전 맞습니다. 하지만 점수는 안줍니다.
	}
	
	// 점수 계산
	CalcBattleScoreProcess(pcsGuild1, pcsAttacker, pcsGuild2, pcsDead);
	
	if( pcsGuild1->IsOnBattle(pcsAttacker->m_szID) && pcsGuild2->IsOnBattle(pcsDead->m_szID) )
		BattleUpdatePrivateMode( pcsGuild1, pcsGuild2 );
	
	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::CalcBattleScoreByDisconnect(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMostDamager)
{
	if( !pcsCharacter )		return FALSE;

	CHAR* szGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	if( !szGuildID )		return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if( !pcsGuild )			return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pLock2);
	if(!pcsEnemyGuild)		return FALSE;
	
	if(m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
	{
		if(m_pagpmGuild->IsBattleDeclareStatus(pcsGuild))
		{
			BattleCancelAccept(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsEnemyGuild->m_szID);
			return TRUE;
		}
		
		if(m_pagpmGuild->IsBattleReadyStatus(pcsGuild) || m_pagpmGuild->IsBattleIngStatus(pcsGuild))
		{
			if( pcsGuild->m_csCurrentBattleInfo.m_eType == eGuildBattleDeadMatch ) //길마 많이 죽이기에서 길마가 나가면
			{
				BattleWithdraw(pcsGuild, pcsCharacter);
				return TRUE;
			}
		}
	}
	else
	{
		if(m_pagpmGuild->IsBattleIngStatus(pcsGuild))
		{
			if( pcsGuild->m_csCurrentBattleInfo.m_eType == eGuildBattleDeadMatch )
			{
				return TRUE;
			}
		}
	}

	// 배틀중이 아니면 나간다.
	if(!pcsGuild->IsOnBattle( pcsCharacter->m_szID ))
		return FALSE;
	
	CalcBattleScoreProcess(pcsEnemyGuild, pcsMostDamager, pcsGuild, pcsCharacter);
	
	BattleUpdatePrivateMode( pcsGuild, pcsEnemyGuild );
	
	return TRUE;
}

// 2005.04.17. steeple
// pcsGuild 와 pcsCharacter 는 Lock 되어서 넘어온다.
BOOL AgsmGuild::BattleWithdraw(AgpdGuild* pcsGuild, AgpdCharacter* pcsCharacter)
{
	if(!pcsGuild || !pcsCharacter)
		return FALSE;

	return BattleWithdraw(pcsGuild, pcsCharacter->m_szID);
}

// 2005.04.21. steeple
// pcsGuild 는 Lock 되서 넘어온다.
BOOL AgsmGuild::BattleWithdraw(AgpdGuild* pcsGuild, CHAR* szMasterID)
{
	if(!pcsGuild || !szMasterID)
		return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild, szMasterID))
		return FALSE;

	// 상대방 길드가 무조건 이긴걸루 해서 끝낸다.
	AuAutoLock pLock;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pLock);
	if(!pcsEnemyGuild)
		return FALSE;
		
	AgsdGuildBattleProcess* pcsBattleProcess = GetBattleProcess(pcsGuild->m_szID, pcsEnemyGuild->m_szID);
	if(!pcsBattleProcess) return FALSE;
	
	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	
	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_NONE;
	pcsGuild->BattleEnd( ulCurrentTime );
	pcsEnemyGuild->BattleEnd( ulCurrentTime );

	pcsGuild->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW;
	pcsEnemyGuild->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW;

	pcsEnemyGuild->m_lWin++;
	pcsGuild->m_lLose++;
	
	BattleEndProcess(pcsEnemyGuild, pcsGuild); // Enemy Win...

	// Battle List Lock
	BattleListLock();
	RemoveBattleProcessList(pcsGuild->m_szID, pcsEnemyGuild->m_szID);
	BattleListUnlock();

	return TRUE;
}
















// DB 에서 Load 한다.
// 이 함수가 불리면 시작하게 된다.
BOOL AgsmGuild::LoadAllGuildInfo()
{
	// 등록된 Callback 을 불러주면 된다.
	// Guild 정보만 우선 요청하면 Relay 에서 길드를 다 보내주고, 멤버까지 보내주게 된다.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_SELECT, NULL, NULL);
	//EnumCallback(AGSMGUILD_CB_DB_MEMBER_SELECT, NULL, NULL);

	return TRUE;
}

// 로드되어 있는 모든 Guild Info 를 지운다.
BOOL AgsmGuild::ClearAllGuildInfo()
{
	if(m_pagpmGuild)
		m_pagpmGuild->OnDestroy();

	return TRUE;
}

// DB 에서 Guild Data 를 Load 했다.
// 현재는 AgsmRelay 에서 직접 불러준다.
BOOL AgsmGuild::GuildLoadFromDB(AgsdRelay2GuildMaster* pAgsdRelay)
{
	if(!pAgsdRelay)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->CreateGuild(pAgsdRelay->m_szGuildID, pAgsdRelay->m_lGuildTID, pAgsdRelay->m_lGuildRank, pAgsdRelay->m_lMaxMemberCount, pAgsdRelay->m_lUnionID);
	if(pcsGuild)
	{
#ifdef	_DEBUG
		//printf("\n[AgsmGuild] GuildLoad :: GuildID:%s, MasterID:%s, Password:%s", szGuildID, szMasterID, szPassword);
#endif

		ZeroMemory(pcsGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
		strncpy(pcsGuild->m_szMasterID, pAgsdRelay->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);
		
		pcsGuild->m_lCreationDate	= pAgsdRelay->m_lCreationDate;
		pcsGuild->m_lUnionID		= pAgsdRelay->m_lUnionID;

		ZeroMemory( pcsGuild->m_szPassword, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH + 1) );
		strncpy( pcsGuild->m_szPassword, pAgsdRelay->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH );

		pcsGuild->m_cStatus			= pAgsdRelay->m_cStatus;
		pcsGuild->m_lWin			= pAgsdRelay->m_lWin;
		pcsGuild->m_lDraw			= pAgsdRelay->m_lDraw;
		pcsGuild->m_lLose			= pAgsdRelay->m_lLose;
		pcsGuild->m_lGuildMarkTID	= pAgsdRelay->m_lGuildMarkTID;
		pcsGuild->m_lGuildMarkColor	= pAgsdRelay->m_lGuildMarkColor;
		pcsGuild->m_lGuildPoint		= pAgsdRelay->m_lGuildPoint;		// 2007.10.17 arycoat

		if(pAgsdRelay->m_szEnemyGuildID)
		{
			ZeroMemory(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
			strncpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pAgsdRelay->m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		}
		if(pAgsdRelay->m_szBattleStartTime)
			pcsGuild->m_csCurrentBattleInfo.m_ulStartTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(pAgsdRelay->m_szBattleStartTime);

		pcsGuild->m_csCurrentBattleInfo.m_ulDuration	= pAgsdRelay->m_ulBattleDuration;
		pcsGuild->m_csCurrentBattleInfo.m_lMyScore		= pAgsdRelay->m_lMyScore;
		//pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore = lEnemyScore;

		// Enemy Guild 를 얻어온다.
		AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(pAgsdRelay->m_szEnemyGuildID);
		if(pcsEnemyGuild)
		{
			// 서로 적 점수를 넣어준다. 후후훗~
			pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore = pcsEnemyGuild->m_csCurrentBattleInfo.m_lMyScore;
			pcsEnemyGuild->m_csCurrentBattleInfo.m_lEnemyScore = pcsGuild->m_csCurrentBattleInfo.m_lMyScore;

			pcsEnemyGuild->m_Mutex.Release();
		}

		//[KTH] 굳이 서버가 죽었는데 길드전을 다시 시작해줄 필요는 없을듯..
		// Status 가 Battle 이고, Battle Process 에 없으면 새로 추가해준다.
		if(m_pagpmGuild->IsBattleIngStatus(pcsGuild))
		{
			BattleListLock();
			if(!GetBattleProcess(pcsGuild->m_szID, pAgsdRelay->m_szEnemyGuildID))
			{
				// 이 상태에서는 그냥 끝내준다.
				if(pcsGuild)
				{
					m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
					// DB 저장
					EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);
				}

				if( pcsEnemyGuild )
				{
					m_pagpmGuild->InitCurrentBattleInfo( pcsEnemyGuild );
					// DB 저장
					EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsEnemyGuild, NULL);
				}
			}
			BattleListUnlock();
		}

		// 2005.06.09. steeple
		// 길드 인원을 50명으로 변경해준다.
		// 길드 인원이 50명에서 증가가 가능하므로 처리한다.
/*		INT32 lOldMaxMemberCount = pcsGuild->m_lMaxMemberCount;
		if(lOldMaxMemberCount != AGSMGUILD_MAX_MEMBER_COUNT)
		{
			UpdateMaxMemberCount(pcsGuild, AGSMGUILD_MAX_MEMBER_COUNT);
		}
*/
		pcsGuild->m_Mutex.Release();
		
		PVOID pvBuffer[2];
		pvBuffer[0] = &pAgsdRelay->m_llMoney;
		pvBuffer[1] = &pAgsdRelay->m_lSlot;
		// 나중에 권한 설정부분도 일루 오믄 된다.
		
		EnumCallback(AGSMGUILD_CB_GUILD_LOAD, pcsGuild, pvBuffer);
	}

	return TRUE;
}

// DB 에서 Member Data 를 Load 했다.
// 현재는 AgsmRelay 에서 직접 불러준다.
BOOL AgsmGuild::MemberLoadFromDB(CHAR* szMemberID, CHAR* szGuildID, INT32 lRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID)
{
	if(!szMemberID || !szGuildID)
		return FALSE;

	// 2005.04.27. steeple
	// 멤버를 다 로딩해서 날아온건지 확인해야 한다.
	if(strcmp(szMemberID, AGSMGUILD_LOAD_COMPLETE_STRING) == 0)
	{
		if(m_hEventGuildLoadComplete)
		{
			m_pagpmGuild->m_csGuildAdmin.SortGuildMemberDesc( FALSE );

			SetEvent(m_hEventGuildLoadComplete);

			printf("\n\n[AgsmGuild] Guild Loading Complete !!!!!\n\n");

			// 길드 정보를 모두다 수신한 상태이므로 우승자 길드를 파일로 부터 읽어들여 설정한다.
			LoadWinnerGuild();
		}

		return TRUE;
	}

//	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);		//	2005.08.03. Delete By SungHoon
	if (lRank == AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)
	{
		RequestJoinMemberSelf(szGuildID, szMemberID, lRank, lJoinDate, lLevel, lTID);
		return TRUE;
	}

	if(m_pagpmGuild->JoinMember(szGuildID, szMemberID, lRank, lJoinDate, lLevel, lTID, AGPMGUILD_MEMBER_STATUS_OFFLINE))
	{
		//printf("\n[AgsmGuild] MemberLoad :: GuildID:%s, MemberID:%s, Rank:%d, Level:%d", szGuildID, szMemberID, lRank, lLevel);
	}
	//	가입탈퇴대기 중이라면 탈퇴대기리스트에도 추가한다.	2005.08.22. By SungHoon
	if (lRank == AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST)
		InsertLeaveMemberSelf(szGuildID, szMemberID, lJoinDate);

//	pcsGuild->m_Mutex.Release();	//	2005.08.03. Delete By SungHoon
	return TRUE;
}

// 2005.01.24. steeple
// DB 에서 해당 GuildID 로 길드를 만들수 있는 지 리턴받는다.
BOOL AgsmGuild::GuildIDCheckFromDB(BOOL bResult, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword)
{
	CHAR szMessage[255];
	sprintf(szMessage, "AgsmGuild::GuildIDCheckFromDB | bResult:%d, szGuildID:%s, szMasterID:%s, szPassword:%s\n", bResult, szGuildID, szMasterID, szPassword);
	m_pagpmGuild->WriteGuildLog(szMessage);

	if(bResult)	// 만들 수 있음
	{
		m_pagpmGuild->OnOperationCreate(1, szGuildID, szMasterID, -1, -1, -1, -1, -1, szPassword, NULL, 0, -1, -1, -1, -1, -1, -1, FALSE);
	}
	else	// 만들 수 없음
	{
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsCharacter)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsCharacter->m_Mutex.Release();

		}
	}

	return TRUE;
}

// 2006.08.11. steeple
// Relation Guild 로딩.
BOOL AgsmGuild::RelationLoadFromDB(CHAR* szGuildID, CHAR* szRelationGuildID, INT8 cRelation, UINT32 ulDate)
{
	if(!szGuildID || !szRelationGuildID)
		return FALSE;
	
    AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szRelationGuildID);
	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	// cRelation 값은 pcsGuild2 이 pcsGuild1 에 대한 것이다.
	switch((AgpmGuildRelation)cRelation)
	{
		case AGPMGUILD_RELATION_JOINT:
			m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			//m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			//m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			//m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			break;

		case AGPMGUILD_RELATION_JOINT_LEADER:
			// pcsGuild2 가 LEADER 이다.
			m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
			m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
			break;

		case AGPMGUILD_RELATION_HOSTILE:
			m_pagpmGuild->AddHostileGuild(pcsGuild1, pcsGuild2->m_szID, ulDate);
			m_pagpmGuild->AddHostileGuild(pcsGuild2, pcsGuild1->m_szID, ulDate);
			break;
	}

	return TRUE;
}

// 2005.03.25. steeple
BOOL AgsmGuild::GetSelectQueryCharGuildID(CHAR* szQuery, INT32 lQueryLength)
{
	if(!szQuery)
		return FALSE;

	sprintf(szQuery, "SELECT guildid FROM guildmember WHERE charid=:1");

	if((INT32)strlen(szQuery) >= lQueryLength)
	{
		ASSERT(!"AgsmGuild::GetSelectQueryCharGuildID(CHAR* szQuery, INT32 lQueryLength) | szQuery >= lQueryLength");
		return FALSE;
	}

	return TRUE;
}

// 2005.03.25. steeple
BOOL AgsmGuild::GetSelectQueryCharGuildID(CHAR* szCharID, CHAR* szWorldDBName, CHAR* szQuery, INT32 lQueryLength)
{
	if(!szCharID || !szWorldDBName || !szQuery)
		return FALSE;

	sprintf(szQuery, "SELECT guildid FROM %s.guildmember WHERE charid='%s'", szWorldDBName, szCharID);

	if((INT32)strlen(szQuery) >= lQueryLength)
	{
		ASSERT(!"AgsmGuild::GetSelectQueryCharGuildID(CHAR* szCharID, CHAR* szWorldDBName, CHAR* szQuery, INT32 lQueryLength) | szQuery >= lQueryLength");
		return FALSE;
	}

	return TRUE;
}

// 2005.03.25. steeple
BOOL AgsmGuild::GetSelectResultCharGuildID5(AuDatabase2 *pDatabase, AgpdCharacter* pcsCharacter)
{
	if(!pDatabase || !pcsCharacter)
		return FALSE;

	if(!pDatabase->GetQueryResult(0))
		return TRUE;

		return m_pagpmGuild->SetCharAD(pcsCharacter, pDatabase->GetQueryResult(0), -1, -1 , FALSE, 0);
}







//////////////////////////////////////////////////////////////////////////
// Packet Send
BOOL AgsmGuild::SendSystemMessage(INT32 lCode, UINT32 ulNID, CHAR* szData1 /* = NULL */, CHAR* szData2 /* = NULL */, INT32 lData1 /* = 0 */, INT32 lData2 /* = 0 */)
{
	if(lCode < 0 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildSystemMessagePacket(&nPacketLength,
									&lCode,
									szData1,
									szData2,
									&lData1,
									&lData2
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendSystemMessageToAllMembers(INT32 lCode, AgpdGuild* pcsGuild, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2)
{
	if(lCode < 0 || !pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildSystemMessagePacket(&nPacketLength,
									&lCode,
									szData1,
									szData2,
									&lData1,
									&lData2
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendCreateGuild(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !ulNID)
		return FALSE;

	INT16 nNoticeLength = 0;
	if(pcsGuild->m_szNotice)
		nNoticeLength = (INT16)_tcslen(pcsGuild->m_szNotice);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCreatePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_szMasterID,
									&pcsGuild->m_lTID,
									&pcsGuild->m_lRank,
									&pcsGuild->m_lCreationDate,
									&pcsGuild->m_lMaxMemberCount,
									&pcsGuild->m_lUnionID,
									NULL,	// 클라이언트에 Password 는 보낼 필요 없다.
									pcsGuild->m_szNotice, &nNoticeLength,
									&pcsGuild->m_cStatus,
									&pcsGuild->m_lWin,
									&pcsGuild->m_lDraw,
									&pcsGuild->m_lLose,
									&pcsGuild->m_lGuildPoint,
									&pcsGuild->m_lGuildMarkTID,
									&pcsGuild->m_lGuildMarkColor,
									&pcsGuild->m_lBRRanking
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendJoinRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID, INT32 lCID, UINT32 ulNID)
{
	if(!szGuildID || !szMasterID || !szCharID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJoinRequestPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szMasterID,
									szCharID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendJoinMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !pcsMember || !ulNID)
		return FALSE;

	INT32 lJoinDate = m_pagpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJoinPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsMember->m_szID,
									&pcsMember->m_lRank,
									&lJoinDate,
									&pcsMember->m_lLevel,
									&pcsMember->m_lTID,
									&pcsMember->m_cStatus
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.01. By SungHoon
	가입 신청한 유저를 내려준다.
*/
BOOL AgsmGuild::SendSelfJoinMember(CHAR *szGuildID, AgpdGuildRequestMember *pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!szGuildID || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMGUILD_PACKET_JOIN;
	INT32 lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	PVOID pvPacket = m_pagpmGuild->MakeGuildMemberPacket(&nPacketLength, 
																&cOperation,
																&lCID,
																szGuildID,
																pcsMember->m_szMemberID,
																&lRank,
																&pcsMember->m_lLeaveRequestTime,
																&pcsMember->m_lLevel,
																&pcsMember->m_lTID,
																&pcsMember->m_cStatus
																);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendLeaveAllowMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !szCharID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildLeaveAllowPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									szCharID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.19. By SungHoon
	길드 탈퇴 신청 하기
*/
BOOL AgsmGuild::SendLeaveMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !szCharID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildLeavePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									szCharID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}


BOOL AgsmGuild::SendDestroyGuild(CHAR* szGuildID, CHAR* szMasterID, INT32 lCID, UINT32 ulNID)
{
	if(!szGuildID || !szMasterID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildDestroyPacket(&nPacketLength, &lCID, szGuildID, szMasterID, NULL);	// 클라이언트에 패스워드는 보낼 필요 없다.
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendMaxMemberCount(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildUpdateMaxMemberCountPacket(&nPacketLength, &lCID, pcsGuild->m_szID, &pcsGuild->m_lMaxMemberCount);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 클라이언트에서 szCharID 로 Character 를 얻기 때문에 보내야 한다.
// szGuildID, pcsMember 는 NULL 일 수도 있다.
BOOL AgsmGuild::SendCharGuildData(CHAR* szGuildID, CHAR* szCharID, AgpdGuildMember* pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!szCharID || !ulNID)	// szGuildID, pcsMember 는 NULL 일 수도 있다.
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = NULL;
	if(pcsMember)
	{
		INT32 lJoinDate = m_pagpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;
		pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
								&lCID,
								szGuildID,
								pcsMember->m_szID,
								&pcsMember->m_lRank,
								&lJoinDate,
								&pcsMember->m_lLevel,
								&pcsMember->m_lTID,
								&pcsMember->m_cStatus,
								NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/, 
								NULL /*IsWinner*/
								);
	}
	else
	{
		pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
								&lCID,
								szGuildID,
								szCharID, NULL, NULL, NULL, NULL, NULL, NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
								NULL /*IsWinner*/);
	}

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.17. By SungHoon
	길드 가입 대기 사용자의 경우 처음 접속시 이 함수를 사용 자기 길드 정보를 받는다.
*/
BOOL AgsmGuild::SendCharGuildData(AgpdGuildRequestMember* pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!ulNID ||!pcsMember)	// szGuildID, pcsMember 는 NULL 일 수도 있다.
		return FALSE;

	INT32 lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
								&lCID,
								pcsMember->m_szGuildID,
								pcsMember->m_szMemberID,
								&lRank,
								&pcsMember->m_lLeaveRequestTime,
								&pcsMember->m_lLevel,
								&pcsMember->m_lTID,
								&pcsMember->m_cStatus,
								NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
								NULL /*IsWinner*/
								);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Notice 를 보낸다.
BOOL AgsmGuild::SendNotice(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !lCID || !ulNID)
		return FALSE;

	INT16 nNoticeLength = 0;
	if(pcsGuild->m_szNotice)
		nNoticeLength = (INT16)_tcslen(pcsGuild->m_szNotice);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildUpdateNoticePacket(&nPacketLength,
								&lCID,
								pcsGuild->m_szID,
								pcsGuild->m_szMasterID,
								pcsGuild->m_szNotice, &nNoticeLength
								);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// pcsGuild 의 모든 멤버에게 Notice 를 보낸다.
BOOL AgsmGuild::SendNoticeToAllMember(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nNoticeLength = 0;
	if(pcsGuild->m_szNotice)
		nNoticeLength = (INT16)_tcslen(pcsGuild->m_szNotice);

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildUpdateNoticePacket(&nPacketLength,
								&lCID,
								pcsGuild->m_szID,
								pcsGuild->m_szMasterID,
								pcsGuild->m_szNotice, &nNoticeLength
								);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.12. By SungHoon
	가입대기중인 사용자에게 길마가 허가를 했다.
*/
BOOL AgsmGuild::SendNewMemberJoinToOtherMembers(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		// 새로 가입한 멤버는 스킵
		if(strcmp((*ppcsMember)->m_szID, pcsMember->m_szID) == 0)
			continue;

		SendJoinMember(pcsGuild, pcsMember, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_JOIN, GetMemberNID(*ppcsMember), pcsMember->m_szID);
	}

	return TRUE;
}
/*
	2005.08.01. By SungHoon
	가입신청한 유저의 정보를 모든 길드원에게 내려준다.
*/
BOOL AgsmGuild::SendNewMemberSelfJoinToOtherMembers(CHAR *szGuildID, AgpdGuildRequestMember *pcsMember, INT32 lCID)
{
	if(!szGuildID || !pcsMember)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendSelfJoinMember(szGuildID, pcsMember,  lCID,  GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOIN_REQUEST_SELF, GetMemberNID(*ppcsMember), pcsMember->m_szMemberID);
	}
	pcsGuild->m_Mutex.Release();

	return TRUE;
}
// lNID 로 pcsGuild 의 모든 멤버의 Join 패킷을 보낸다.
// 새로 가입한 멤버에게 보낼 때 쓰이고, 처음 접속시에 쓰인다.
BOOL AgsmGuild::SendAllMemberJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendJoinMember(pcsGuild, *ppcsMember, lCID, ulNID);
	}
//	가입신청 중인 List를 내려준다.
	SendAllMemberSelfJoin(pcsGuild, lCID, ulNID);
	return TRUE;
}

// lNID 로 pcsGuild 의 모든 멤버의 Join 패킷을 보낸다.
// 새로 가입한 멤버에게 보낼 때 쓰이고, 처음 접속시에 쓰인다.
BOOL AgsmGuild::SendOtherMemberMemberJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID, CHAR *szMemberID)
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		if (!strcmp((*ppcsMember)->m_szID, szMemberID)) continue;
		SendJoinMember(pcsGuild, *ppcsMember, lCID, ulNID);
	}
//	가입신청 중인 List를 내려준다.
	SendAllMemberSelfJoin(pcsGuild, lCID, ulNID);
	return TRUE;
}

/*
	2005.08.01. By SungHoon
	가입신청중인 사용자를 내려준다.
*/
BOOL AgsmGuild::SendAllMemberSelfJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	AgpdGuildRequestMember **ppcsMember = NULL;
	AgpdGuildRequestMember *pcsMember = NULL;
	INT32 lIndex = 0;

	AuAutoLock Lock(pcsGuild->m_pGuildJoinList->m_Mutex);
	if (!Lock.Result()) return FALSE;

	for(ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if (!*ppcsMember) break;
		pcsMember = *ppcsMember;
		SendSelfJoinMember(pcsMember->m_szGuildID, pcsMember, 1, ulNID);
	}

	return TRUE;
}

/*
	2005.08.19. By SungHoon
	길드 탈퇴 신청 메세지를 보낸다.
*/
BOOL AgsmGuild::SendLeaveToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		if (!strcmp((*ppcsMember)->m_szID, szCharID)) continue;

		SendLeaveMember(pcsGuild, szCharID, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_LEAVE_REQUEST, GetMemberNID(*ppcsMember), szCharID);
	}

	return TRUE;
}

BOOL AgsmGuild::SendLeaveAllowToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendLeaveAllowMember(pcsGuild, szCharID, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_LEAVE, GetMemberNID(*ppcsMember), szCharID);
	}

	return TRUE;
}

BOOL AgsmGuild::SendForcedLeaveToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendLeaveAllowMember(pcsGuild, szCharID, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE, GetMemberNID(*ppcsMember), szCharID);
	}

	return TRUE;
}

BOOL AgsmGuild::SendMaxMemberCountToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendMaxMemberCount(pcsGuild, 1, GetMemberNID(*ppcsMember));
	}

	return TRUE;
}

BOOL AgsmGuild::SendMemberUpdateToMembers(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, BOOL bIncludeMe)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	INT32 lJoinDate = m_pagpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;		//	지나간 시간을 내려준다
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
							&lCID,
							pcsGuild->m_szID,
							pcsMember->m_szID,
							&pcsMember->m_lRank,
							&lJoinDate,
							&pcsMember->m_lLevel,
							&pcsMember->m_lTID,
							&pcsMember->m_cStatus,
							&pcsGuild->m_lGuildMarkTID,
							&pcsGuild->m_lGuildMarkColor,
							&pcsGuild->m_lBRRanking
							);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if (bIncludeMe) SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	else SendPacketToOtherMembers(pcsGuild, pcsMember->m_szID, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendRequestJoinMemberUpdateToMembers(AgpdGuild* pcsGuild, AgpdGuildRequestMember* pcsMember, BOOL bIncludeMe)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	INT32 lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
							&lCID,
							pcsGuild->m_szID,
							pcsMember->m_szMemberID,
							&lRank,
							&pcsMember->m_lLeaveRequestTime,
							&pcsMember->m_lLevel,
							&pcsMember->m_lTID,
							&pcsMember->m_cStatus,
							NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
							NULL /*IsWinner*/
							);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if (bIncludeMe) SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	else SendPacketToOtherMembers(pcsGuild, pcsMember->m_szMemberID, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}
// 자기의 길드 이름을 주위사람들에게 보낸다.
// lNID 는 자기의 NID 이며, 자기한테는 보내지 않는다.
BOOL AgsmGuild::SendCharGuildIDToNear(CHAR* szGuildID, CHAR* szCharID, AuPOS stPos, INT32 lGuildMarkTID, INT32 lGuildMarkColor, INT32 lBRRanking, UINT32 ulNID)
{
	if(!szCharID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szCharID, NULL, NULL, NULL, NULL, NULL,
									&lGuildMarkTID, &lGuildMarkColor, &lBRRanking);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, stPos, -1, ulNID, PACKET_PRIORITY_6);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.16. steeple
// Battle Request 를 보낸다. 마스터에게 간다.
BOOL AgsmGuild::SendBattleRequest(CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson, UINT32 ulNID)
{
	if(!szGuildID || !szEnemyGuildID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_REQUEST pPacketGuildBattleRequest(&nPacketLength, &lCID,
		szGuildID, NULL, szEnemyGuildID, &lType, &ulDuration, &lPerson);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleRequest);
									
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
// Battle Accept 를 모든 멤버에게 보낸다.
BOOL AgsmGuild::SendBattleAcceptToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_ACCEPT pPacketGuildBattleAccept(&nPacketLength,
		&lCID,
		pcsGuild->m_szID,
		pcsGuild->m_szMasterID,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
		&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
		(INT32*)&pcsGuild->m_csCurrentBattleInfo.m_eType,
		&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
		(UINT32*)&pcsGuild->m_csCurrentBattleInfo.m_ulPerson);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleAccept);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleCancelRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32 ulNID)
{
	if(!szGuildID || !szEnemyGuildID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleCancelReqeustPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szMasterID,
									szEnemyGuildID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleCancelAcceptToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleCancelAcceptPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_szMasterID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleStartToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleStartPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID,
									&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
									&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleEndToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_RESULT pPacketGuildBattleResult(&nPacketLength, &lCID, 
		pcsGuild->m_szID, 
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, 
		&pcsGuild->m_csCurrentBattleInfo.m_lMyUpPoint, 
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpPoint, 
		&pcsGuild->m_csCurrentBattleInfo.m_cResult);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleResult);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleUpdateTimeToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleUpdateTimePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
									&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
									&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleUpdateScoreToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleUpdateScorePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
									&pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
									&pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
									&pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore,
									&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendBattleRoundToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;

	PACKET_GUILD_BATTLE_ROUND pPacketGuildBattleRound(&nPacketLength, &lCID, 
		pcsGuild->m_szID, 
		&pcsGuild->m_csCurrentBattleInfo.m_ulRound);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleRound);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

BOOL AgsmGuild::SendBattleMemberInfoAllMembers(AgpdGuild* pcsGuild, CHAR* szGuildID, AgpdGuildMember* pMember)
{
	if( !pcsGuild || !szGuildID || !pMember )		return FALSE;
	
	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleMemberPacket(&nPacketLength, &lCID, szGuildID, 
																pMember->m_szID, pMember->m_ulScore, pMember->m_ulKill, pMember->m_ulDeath);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendBattleMemberListInfoAllMembers(AgpdGuild* pcsMyGuild, AgpdGuild* pcsEnemyGuild)
{
	if( !pcsMyGuild )		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleMemberListPacket(&nPacketLength, &lCID, pcsMyGuild->m_szID, pcsEnemyGuild->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsMyGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
// 길드 배틀에 관련된 모든 정보를 다 보내준다.
// 보통 배틀이 끝나고 난 후 BattleEnd 를 보낸 다음에 호출되게 된다.
BOOL AgsmGuild::SendBattleInfoToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_INFO pPacketGuildBattle(&nPacketLength,
		&lCID,
		pcsGuild->m_szID,
		pcsGuild->m_szMasterID,
		&pcsGuild->m_cStatus,
		&pcsGuild->m_lWin,
		&pcsGuild->m_lDraw,
		&pcsGuild->m_lLose,
		&pcsGuild->m_lGuildPoint,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID,		
		&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
		(INT32*)&pcsGuild->m_csCurrentBattleInfo.m_eType,
		&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
		&pcsGuild->m_csCurrentBattleInfo.m_ulPerson,
		&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_cResult
		);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattle);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
// 특정 ulNID 로, 길드 배틀 정보를 보내준다.
BOOL AgsmGuild::SendBattleInfo(AgpdGuild* pcsGuild, UINT32 ulNID)
{
	if(!pcsGuild || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_INFO pPacketGuildBattle(&nPacketLength,
		&lCID,
		pcsGuild->m_szID,
		pcsGuild->m_szMasterID,
		&pcsGuild->m_cStatus,
		&pcsGuild->m_lWin,
		&pcsGuild->m_lDraw,
		&pcsGuild->m_lLose,
		&pcsGuild->m_lGuildPoint,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID,
		&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
		(INT32*)&pcsGuild->m_csCurrentBattleInfo.m_eType,
		&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
		&pcsGuild->m_csCurrentBattleInfo.m_ulPerson,
		&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_cResult
		);
		
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattle);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}


// 2005.04.17. steeple
// 같은 패킷은 하나로 전체멤버에게 다 보낸다.
BOOL AgsmGuild::SendPacketToAllMembers(AgpdGuild* pcsGuild, PVOID pvPacket, INT16 nPacketLength, BOOL bBattle)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppcsMember )		continue;
		UINT32	ulNID = GetMemberNID(*ppcsMember);
		if( !ulNID )			continue;
		//if( bBattle && !pcsGuild->IsBattle( (*ppcsMember)->m_szID ) )	continue;
			
		SendPacket(pvPacket, nPacketLength, ulNID);
	}

	return TRUE;
}

// 2008. 09. 18. iluvs
// 같은 패킷을 전체 멤버에게 다 보낸다. (새로운 패킷으로)
BOOL AgsmGuild::SendPacketToAllMembers(AgpdGuild* pcsGuild, PACKET_HEADER& pvPacket)
{
	if(!pcsGuild || pvPacket.unPacketLength == 0)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if (!*ppcsMember)	continue;
		UINT32 ulNID = GetMemberNID(*ppcsMember);
		if (!ulNID)			continue;
		
		AgsEngine::GetInstance()->SendPacket(pvPacket, ulNID);
	}

	return TRUE;
}

/*
	2005.08.16. By SungHoon
	같은 패킷은 하나로 szMemberID 를 제외한 전체멤버에게 다 보낸다.
*/
BOOL AgsmGuild::SendPacketToOtherMembers(AgpdGuild* pcsGuild, CHAR *szMemberID, PVOID pvPacket, INT16 nPacketLength)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !*ppcsMember )		continue;
		UINT32	ulNID = GetMemberNID(*ppcsMember);
		if( !ulNID )			continue;
		if( !strcmp( (*ppcsMember)->m_szID, szMemberID ) )	continue;

		SendPacket(pvPacket, nPacketLength, ulNID);
	}

	return TRUE;
}

/*
	2005.09.05. By SungHoon
	가입 대기중인 사용자에게 패킷을 내려준다.
*/
BOOL AgsmGuild::SendPacketToAllJoinRequestMembers(AgpdGuild* pcsGuild, PVOID pvPacket, INT16 nPacketLength)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	UINT32 ulNID = 0;
	AgpdGuildRequestMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock((*ppcsMember)->m_szMemberID);
		if(!pcsCharacter) continue;

		ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);
		pcsCharacter->m_Mutex.Release();

		if(!ulNID) continue;
		SendPacket(pvPacket, nPacketLength, ulNID);
	}

	return TRUE;
}

// 다른 게임 서버로 길드 생성 패킷을 보낸다.
BOOL AgsmGuild::SendSyncCreate(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketGuild(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_CREATE, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임 서버로 멤버 조인을 보낸다.
BOOL AgsmGuild::SendSyncJoin(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketMember(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_JOIN, pcsGuild, pcsMember);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임 서버로 멤버 탈퇴를 보낸다.
BOOL AgsmGuild::SendSyncLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketMember(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_LEAVE, pcsGuild, pcsMember);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임 서버로 멤버 짤림을 보낸다.
BOOL AgsmGuild::SendSyncForcedLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketMember(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_FORCED_LEAVE, pcsGuild, pcsMember);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임 서버로 길드 해체를 보낸다.
BOOL AgsmGuild::SendSyncDestroy(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketGuild(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_DESTROY, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임 서버로 길드 맥스 멤버 업데이트를 보낸다.
BOOL AgsmGuild::SendSyncUpdateMaxMemberCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketGuild(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_UPDATE_MAX_MEMBER_COUNT, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임 서버로 길드 공지사항을 보낸다.
BOOL AgsmGuild::SendSyncUpdateNotice(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketNotice(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_UPDATE_NOTICE, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// 다른 게임서버로 패킷을 보낸다.
BOOL AgsmGuild::SendSyncPacketToOtherGameServers(PVOID pvPacket, INT16 nPacketLength)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	AgsdServer* pcsThisServer = m_pagsmServerManager->GetThisServer();
	if(!pcsThisServer)
		return FALSE;

	AgsdServer* pcsAgsdServer = NULL;
	INT16 lIndex = 0;
	while((pcsAgsdServer = m_pagsmServerManager->GetGameServers(&lIndex)) != NULL)
	{
		SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer);
	}

	return TRUE;
}

PVOID AgsmGuild::MakeSyncPacketGuild(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild)
{
	if(!pnPacketLength || !pcsGuild)
		return NULL;

	INT16 nGuildSize = sizeof(AgpdGuild);
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								NULL,
								NULL);
}

PVOID AgsmGuild::MakeSyncPacketMember(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pnPacketLength || !pcsGuild || !pcsMember)
		return NULL;

	INT16 nGuildSize = sizeof(AgpdGuild);
	INT16 nMemberSize = sizeof(AgpdGuildMember);
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								pcsMember, &nMemberSize,
								NULL);
}

PVOID AgsmGuild::MakeSyncPacketNotice(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild)
{
	if(!pnPacketLength || !pcsGuild)
		return NULL;

	INT16 nGuildSize = sizeof(AgpdGuild);
	INT16 nNoticeSize = pcsGuild->m_szNotice ? (INT16)_tcslen(pcsGuild->m_szNotice) : 0;
	if(nNoticeSize > 0)
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								NULL,
								pcsGuild->m_szNotice, &nNoticeSize);
	else
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								NULL,
								NULL);
}









//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmGuild::CBCreateGuildCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szMasterID = (CHAR*)pCustData;

	return pThis->CreateGuildCheck(pcsGuild, szMasterID);
}

BOOL AgsmGuild::CBCreateGuildCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pResult = (BOOL*)pCustData;

	if(!ppvBuffer || !pThis || !pResult)
		return FALSE;

	*pResult = pThis->CreateGuildCheckEnable((CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);
	return TRUE;
}

BOOL AgsmGuild::CBCreateGuildSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szMasterID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szMasterID)
		return FALSE;

	return pThis->CreateGuildSuccess(szGuildID, szMasterID);
}

BOOL AgsmGuild::CBCreateGuildFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmGuild::CBJoinRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(!ppvBuffer || !pThis || !pcsCharacter)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];

	return pThis->JoinRequest(szGuildID, szMasterID, pcsCharacter);
}

BOOL AgsmGuild::CBJoinCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pbResult = (BOOL*)pCustData;
	if(!ppvBuffer || !pThis || !pbResult)
		return FALSE;

	BOOL bSelfRequest = *((BOOL*)ppvBuffer[2]);
	*pbResult = pThis->JoinCheckEnable((CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1], bSelfRequest);
	return *pbResult;
}

BOOL AgsmGuild::CBJoinReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szCharID)
		return FALSE;

	return pThis->JoinReject(szGuildID, szCharID);
}

BOOL AgsmGuild::CBJoin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szMemberID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szMemberID)
		return FALSE;

	return pThis->JoinMember(szGuildID, szMemberID);
}

BOOL AgsmGuild::CBJoinFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	INT8* pcErrorCode = (INT8*)pCustData;

	if(!pcsCharacter || !pThis || !pcErrorCode)
		return FALSE;

	return pThis->SendSystemMessage(*pcErrorCode, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmGuild::CBLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szCharID)
		return FALSE;

	return pThis->Leave(szGuildID, szCharID);
}

BOOL AgsmGuild::CBForcedLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!ppvBuffer || !pThis || !szCharID)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];

	return pThis->ForcedLeave(szGuildID, szCharID);
}

BOOL AgsmGuild::CBDestroyGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR** pszMemberID = (CHAR**)pCustData;

	if(!ppvBuffer || !pThis || !pszMemberID)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];
	INT32* plMemberCount = (INT32*)ppvBuffer[2];

	return pThis->DestroyGuild(szGuildID, szMasterID, *plMemberCount, pszMemberID);
}

BOOL AgsmGuild::CBDestroyGuildFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmGuild::CBCheckPassword(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pbValidPassword = (BOOL*)pCustData;

	*pbValidPassword = pThis->CheckPassword((AgpdGuild*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// pcsGuild 는 Lock 되어서 넘어온다.
BOOL AgsmGuild::CBUpdateNotice(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	pThis->SendNoticeToAllMember(pcsGuild);

	// 2005.03.18. steeple
	// 다른 서버에 Sync 한다.
	//pThis->SendSyncUpdateNotice(pcsGuild);
	return TRUE;
}

// 2005.04.17. steeple
// 후후훗~ AgpmGuild 에서도 SystemMessage 보낼 수 있게 하기 위해서
BOOL AgsmGuild::CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuildSystemMessage* pstSystemMessage = (AgpdGuildSystemMessage*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	return pThis->SendSystemMessage(pstSystemMessage->m_lCode,
									pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter),
									pstSystemMessage->m_aszData[0],
									pstSystemMessage->m_aszData[1],
									pstSystemMessage->m_alData[0],
									pstSystemMessage->m_alData[1]
									);
}

BOOL AgsmGuild::CBBattlePerson(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass )
		return FALSE;

	AgpdGuild* pcsGuild	= (AgpdGuild*)pData;
	AgsmGuild* pThis	= (AgsmGuild*)pClass;
	PVOID* ppvBuffer	= (PVOID*)pCustData;

	//아직은 특별히 하는일은 없당. ㅎㅎ

	return TRUE;
}

// 2005.04.15. steeple
BOOL AgsmGuild::CBBattleRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleRequest(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1], *(INT32*)ppvBuffer[2], *(UINT32*)ppvBuffer[3], *(UINT32*)ppvBuffer[4] );

	return TRUE;
}

// 2005.04.15. steeple
BOOL AgsmGuild::CBBattleAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdGuildBattle* pstGuildBattle = (AgpdGuildBattle*)pCustData;

	pThis->BattleAccept(szGuildID, pstGuildBattle->m_szEnemyGuildID, pstGuildBattle->m_eType, pstGuildBattle->m_ulDuration, pstGuildBattle->m_ulPerson);

	return TRUE;
}

// 2005.04.15. steeple
BOOL AgsmGuild::CBBattleReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleReject(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::CBBattleCancelRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleCancelRequest(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::CBBattleCancelAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleCancelAccept(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::CBBattleCancelReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleCancelReject(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.17. steeple
// pcsGuild 와 pcsCharacter 는 Lock 되어서 넘어온다.
BOOL AgsmGuild::CBBattleWithdraw(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	return pThis->BattleWithdraw(pcsGuild, pcsCharacter);
}







// 다른 Server 와 연결이 성공했을 때 불린다.
// Relay 서버인지 확인하고, Relay 서버라면 DB 로딩을 시작한다.
BOOL AgsmGuild::CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return TRUE;

	AgsdServer* pcsServer = (AgsdServer*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	AgsdServer* pcsRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return FALSE;

	if(pcsServer == pcsRelayServer)
		pThis->LoadAllGuildInfo();

	return TRUE;
}

// 다른 Server 와 연결이 성공했을 때 불린다.
// Relay 서버인지 확인하고, Relay 서버라면 DB 로딩을 시작한다.
BOOL AgsmGuild::CBServerSetFlag(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
	/*
	if(!pData || !pClass)
		return TRUE;

	AgsdServer* pcsServer = (AgsdServer*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	AgsdServer* pcsRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return FALSE;

	if(pcsServer == pcsRelayServer)
		pThis->LoadAllGuildInfo();

	return TRUE;
	*/
}

BOOL AgsmGuild::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	if(!pcsCharacter || !pThis)
		return FALSE;

	return pThis->EnterGameWorld(pcsCharacter);
}

BOOL AgsmGuild::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	if(!pcsCharacter || !pThis)
		return FALSE;

	return pThis->Disconnect(pcsCharacter);
}

BOOL AgsmGuild::CBDeleteComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	CHAR* szCharID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	return pThis->CharacterDeleteComplete(szCharID);
}

BOOL AgsmGuild::CBCharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	INT32* plLeveUpNum = (INT32*)pCustData;

	pThis->CharacterLevelUp(pcsCharacter, *plLeveUpNum);
	return TRUE;
}

// 2005.11.09. steeple
BOOL AgsmGuild::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	// PC 일 때만 해주면 된다.
	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		pThis->Disconnect(pcsCharacter);

	return TRUE;
}

// From AgsmAOIFilter
// 캐릭터가 Cell 을 이동할 때, 자기의 정보를 주위 사람들에게 뿌린다.
BOOL AgsmGuild::CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return TRUE;
	
	if(strlen(pcsAttachedGuild->m_szGuildID) > 0)
	{
		AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(pcsAgsdCharacter)
			pThis->SendCharGuildIDToNear(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID, pcsCharacter->m_stPos, 
				pcsAttachedGuild->m_lGuildMarkTID, pcsAttachedGuild->m_lGuildMarkColor, pcsAttachedGuild->m_lBRRanking, 
				pcsAgsdCharacter->m_dpnidCharacter);
	}

	return TRUE;
}

// 2005.03.23. steeple
// AgpmOptimizedPacket2 에서 CharView 할 때 보낸다.
// 즉 이 함수가 호출이 되면 두번 보내게 되는 것이므로 뺀다. (콜백 등록을 주석처리 했음)
//
// From AgsmCharacter
// 한 캐릭터가 이동할 때, 그 주위에 있는 캐릭터의 정보를 이동하는 캐릭터에게 보내준다.
// pcsCharacter 는 주위에 있는 캐릭터이고, 넘어온 NID 가 이동한 캐릭터이다.
// 즉, pcsCharacter 의 길드 정보를 NID 로 보내는 것이다.
BOOL AgsmGuild::CBSendCharView(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	PROFILE("AgsmGuild::CBSendCharView");

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	// PC 가 아니면 나간다.
	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter) == FALSE)
		return TRUE;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// 길드원이 아니면 나간다.
	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	UINT32 ulNID = PtrToUint(ppvBuffer[0]);
	BOOL bGroup = PtrToInt(ppvBuffer[1]);
	BOOL bIsExceptSelf = PtrToInt(ppvBuffer[2]);
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
													&pcsCharacter->m_lID,
													pcsAttachedGuild->m_szGuildID,
													pcsCharacter->m_szID, NULL, NULL, NULL, NULL, NULL,
													&pcsAttachedGuild->m_lGuildMarkTID,
													&pcsAttachedGuild->m_lGuildMarkColor,
													&pcsAttachedGuild->m_lBRRanking
													);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = FALSE;
	if(bGroup)
	{
		if(bIsExceptSelf)
		{
			bSendResult = pThis->m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_4);
		}
		else
		{
			bSendResult = pThis->m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
		}
	}
	else
		bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);

	pThis->m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	
	return bSendResult;
}

// 2005.03.25. steeple
// Login Server 에서 Game Server 로 데이터를 넘겨줄 때 사용한다.
BOOL AgsmGuild::CBSendCharacterNewID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter	= (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	UINT32 ulNID = *(UINT32*)pCustData;

	// 길드아이디가 있을 때만 보내면 된다.
	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsAttachedGuild && strlen(pcsAttachedGuild->m_szGuildID) > 0)
		pThis->SendCharGuildData(pcsAttachedGuild->m_szGuildID, pThis->m_pagsmCharacter->GetRealCharName(pcsCharacter->m_szID), NULL, 1, ulNID);

	return TRUE;
}

/*
	2005.06.07. By SungHoon
	길드 마스터인지 확인한다.
	길드에 가입 되었지 않음 pCustData = 0
	길드마스트				pCustData = 1
	길드에 가입 되어 있음	pCustData = 2
*/

BOOL AgsmGuild::CBIsGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter	= (AgpdCharacter*)pData;
	AgsmGuild* pThis			= (AgsmGuild*)pClass;
	INT32 *plResult				= (BOOL *)pCustData;

	*plResult = 0;

	if (pThis->IsGuildMaster(pcsCharacter))
		*plResult = 1;
	else if (pThis->m_pagpmGuild->IsAnyGuildMember(pcsCharacter))
		*plResult = 2;

	return TRUE;
}

// From AgsmChatting
// pcsCharacter (마스터가 됨) 는 Lock 되어 있음
BOOL AgsmGuild::CBGuildMemberInviteByCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR* szMessage = (CHAR*)ppvBuffer[0];
	INT32* plLength = (INT32*)ppvBuffer[1];

	// 명령어를 날린 사람의 길드 정보를 조사한다.
	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;	

	if(!szMessage || *plLength < 1 || *plLength > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	CHAR szTargetID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szTargetID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));		// 2005.06.07. By SungHoon

	memcpy(szTargetID, szMessage, *plLength);
	//pThis->m_pagpmCharacter->MakeGameID(szTargetID,szTargetID);

	// Target Character Lock
	AgpdCharacter* pcsTarget = pThis->m_pagpmCharacter->GetCharacterLock(szTargetID);
	if(!pcsTarget)
		return FALSE;

	pThis->JoinRequest(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID, pcsTarget);

	pcsTarget->m_Mutex.Release();
	
	return TRUE;
}

// From AgsmChatting
BOOL AgsmGuild::CBGuildLeaveByCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	// 명령어를 날린 사람의 길드 정보를 조사한다.
	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;

	// Master 이면 불가능
	if(pThis->m_pagpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID))
		return FALSE;

	// 2005.02.16. steeple
	// 아래함수를 부르면 해당캐릭터를 락한번 더 하게 된다.
	// 만약 데드락 걸린다면 락을 풀고 함수를 불러줘야 한다.
	pThis->m_pagpmGuild->OnOperationLeave(pcsCharacter->m_lID, pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID);
	return TRUE;
}

BOOL AgsmGuild::CBGuildJointMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR* szMessage = (CHAR*)ppvBuffer[0];
	INT32* plLength = (INT32*)ppvBuffer[1];

	return pThis->GuildJointMessage(pcsCharacter, szMessage, *plLength);
}

BOOL AgsmGuild::CBItemPickupCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild *pThis = (AgsmGuild*)pClass;
	stAgpmItemCheckPickupItem *pPickupItem = (stAgpmItemCheckPickupItem*)pData;
	BOOL *pCanPickup = (BOOL*)pCustData;

	// 이미 획득 불가능한 상태라면 리턴
	if (FALSE == *pCanPickup)
		return TRUE;

	// 겁나 하드 코딩... ㅡㅡ;
//	if (E_AGPMITEM_BIND_ON_GUILDMASTER != pPickupItem->pcsItem->m_pcsItemTemplate->m_lStatusFlag)
//		return TRUE;
	
	if (4608 != pPickupItem->pcsItem->m_pcsItemTemplate->m_lID)
		return TRUE;

	if (FALSE == pThis->IsGuildMaster(pPickupItem->pcsCharacter))
	{
		*pCanPickup = FALSE;

		INT16 nPacketLength = 0;
		PVOID pvPacket = pThis->m_pagpmArchlord->MakepacketMessageID(&nPacketLength, AGPMARCHLORD_MESSAGE_GUILD_MASTER_ITEM);

		if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pPickupItem->pcsCharacter->m_lID));
	}

	return TRUE;
}

BOOL AgsmGuild::SetCallbackDBGuildInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildInsertBattleHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBRename(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_RENAME, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBJointInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBJointUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_JOINT_UPDATE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBJointDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBHostileInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_HOSTILE_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBHostileDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackAddEnemyGuildByBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackRemoveEnemyGuildByBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackAddEnemyGuildBySurviveBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackRemoveEnemyGuildBySurviveBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_SELECT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_SELECT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildIDCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MASTER_CHECK, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackGuildLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_GUILD_LOAD, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackSendGuildInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_SEND_GUILD_INFO, pfCallback, pClass);
}

/*
	2005.07.08. By SungHoon
	길드 리스트를 받았을 경우 Call-Back
*/
BOOL AgsmGuild::CBReceiveGuildListInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID *pvArray = (PVOID *)pData;
	INT32 *pCID	= ( INT32 *)pCustData;

	PVOID pvPacket = pvArray[ 0 ];
	CHAR *szGuildID = ( CHAR *)pvArray[ 1 ];

	if (!pvPacket && !szGuildID) return FALSE;

	INT32 lGuildListCurrentPage = 0;
	INT32 lMaxGuildCount = 0;
	INT16 nVisibleGuildCount = 0;

	INT16 nPacketLength = 0;
	PVOID pvGuildListPacket = NULL;

	UINT32 ulNID = 0;
	{
		ApAutoLockCharacter Lock(pThis->m_pagpmCharacter, *pCID);
		AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
		if (!pcsCharacter) return FALSE;

		AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if (!pcsAgsdCharacter) return FALSE;
		ulNID = pcsAgsdCharacter->m_dpnidCharacter;
	}

	if (szGuildID)
	{
		pvGuildListPacket = pThis->m_pagpmGuild->MakeGuildListPacket(pCID, szGuildID,&nPacketLength);
		if (pvGuildListPacket == NULL)
			pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NO_EXIST_SEARCH_GUILD, ulNID);
	}
	else
	{
		PVOID pvGuildListItemPacket[ AGPMGUILD_MAX_VISIBLE_GUILD_LIST ] = { 0 };
		pThis->m_pagpmGuild->m_csGuildListPacket.GetField(FALSE, pvPacket, 0,
							&lMaxGuildCount,
							&lGuildListCurrentPage,
							&nVisibleGuildCount,
							&pvGuildListItemPacket[0],
							&pvGuildListItemPacket[1],
							&pvGuildListItemPacket[2],
							&pvGuildListItemPacket[3],
							&pvGuildListItemPacket[4],
							&pvGuildListItemPacket[5],
							&pvGuildListItemPacket[6],
							&pvGuildListItemPacket[7],
							&pvGuildListItemPacket[8],
							&pvGuildListItemPacket[9],
							&pvGuildListItemPacket[10],
							&pvGuildListItemPacket[11],
							&pvGuildListItemPacket[12],
							&pvGuildListItemPacket[13],
							&pvGuildListItemPacket[14],
							&pvGuildListItemPacket[15],
							&pvGuildListItemPacket[16],
							&pvGuildListItemPacket[17],
							&pvGuildListItemPacket[18]
							);

		pvGuildListPacket = pThis->m_pagpmGuild->MakeGuildListPacket( pCID, lGuildListCurrentPage, &nPacketLength , NULL );
	}
	if (!pvGuildListPacket) return FALSE;
	pThis->m_pagpmGuild->m_csPacket.FreePacket( pvGuildListPacket );

	pThis->SendPacket( pvGuildListPacket, nPacketLength, ulNID );

	return TRUE;
}

/*
	2005.07.28. By SungHoon
	길드탈퇴 신청을 받았을 경우 불린다.
*/
BOOL AgsmGuild::CBReceiveLeaveRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR *szGuildID = (CHAR *)pData;
	CHAR *szMemberID = ( CHAR*)pCustData;

	pThis->RequestLeaveMemberSelf(szGuildID, szMemberID);
	return TRUE;
}

/*
	2005.08.01. By SungHoon
	길드 가입 신청을 받았을 경우 불린다.
*/
BOOL AgsmGuild::CBReceiveJoinRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID **ppBuffer = (PVOID **)pData;
	BOOL *pResult= ( BOOL *)pCustData;
	CHAR *szGuildID = (CHAR *)ppBuffer[0];
	CHAR *szMemberID = (CHAR *)ppBuffer[1];
	INT32 *pMemberRank = (INT32 *)ppBuffer[2];
	INT32 *pJoinDate = (INT32 *)ppBuffer[3];
	INT32 *pLevel = (INT32 *)ppBuffer[4];
	INT32 *pTID = (INT32 *)ppBuffer[5];

	if (pThis->RequestJoinMemberSelf(szGuildID, szMemberID, *pMemberRank, *pJoinDate, *pLevel, *pTID))
	{
		*pResult = TRUE;

	//	이쯤에서 DB에 저장한다.
		AgpdGuildMember cGuildMember;
		memset(&cGuildMember, 0, sizeof(AgpdGuildMember));
		strncpy( cGuildMember.m_szID, szMemberID, AGPDCHARACTER_MAX_ID_LENGTH);
		cGuildMember.m_lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;

		pThis->EnumCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, szGuildID, &cGuildMember);
	}
	return TRUE;
}
/*
	2005.07.13. By SungHoon
	길드 최대 인원이 변경된다.
*/
BOOL AgsmGuild::CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	INT32 lCID = *( INT32 *)pData;
	INT32 lNewMemberCount = *( INT32 *)pCustData;

	ApAutoLockCharacter Lock(pThis->m_pagpmCharacter, lCID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;
	DPNID ulNID = pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	AgpdGuildADChar* pcsADCharGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsADCharGuild) return FALSE;

	
	AgpdRequireItemIncreaseMaxMember *pcsRequirement = pThis->m_pagpmGuild->GetRequireIncreaseMaxMember(lNewMemberCount);
	if(!pcsRequirement) return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->GetGuildLock(pcsADCharGuild->m_szGuildID);
	if(!pcsGuild) return FALSE;

	if(pThis->m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID) == FALSE)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	INT32 lMemberCount = pThis->m_pagpmGuild->GetMemberCount(pcsGuild);
	if( lMemberCount < AGPMGUILD_GUILDMEMBER_INCREASE_ENABLE_MEMBER_COUNT )
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// 길드 정책 변경 - arycoat 2008.02
	// 필요해골 삭제
	/*AgpdItem *pcsItem = pThis->m_pagpmItem->GetInventoryItemByTID(pcsCharacter,pcsRequirement->m_lSkullTID);
	if(!pcsItem)
	{
		pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX, ulNID);
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	if(pcsItem->m_nCount < pcsRequirement->m_lSkullCount)
	{
		pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX, ulNID);
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}*/
	
	if(pcsCharacter->m_llMoney < pcsRequirement->m_lGheld)
	{
		pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_GHELD_FOR_INCREASE_MAX, ulNID);
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	
	// 길드 정책 변경 - arycoat 2008.02
	// 필요해골 삭제
	/*if (pThis->m_pagsmItem)	// log
		pThis->m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsCharacter->m_lID, pcsItem, pcsRequirement->m_lSkullCount);
	pThis->m_pagpmItem->SubItemStackCount(pcsItem, pcsRequirement->m_lSkullCount, TRUE);*/
	
	pThis->m_pagpmCharacter->SubMoney(pcsCharacter,pcsRequirement->m_lGheld);
	pThis->UpdateMaxMemberCount(pcsGuild, lNewMemberCount, TRUE );

	AgsdCharacter *pAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	pThis->m_pagpmLog->WriteLog_GuildExpansion(0,
											pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : "",
											pAgsdCharacter ? pAgsdCharacter->m_szAccountID : "",
											pAgsdCharacter ? pAgsdCharacter->m_szServerName : "",
											pcsCharacter->m_szID,
											pcsCharacter->m_pcsCharacterTemplate->m_lID,
											pThis->m_pagpmCharacter->GetLevel(pcsCharacter),
											pThis->m_pagpmCharacter->GetExp(pcsCharacter),
											pcsCharacter->m_llMoney,
											pcsCharacter->m_llBankMoney,
											pcsGuild->m_szID,
											pcsRequirement->m_lGheld,
											lNewMemberCount
											);

	pThis->SendMaxMemberCountToAllMembers(pcsGuild);

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

/*
	2005.07.26. By SungHoon
	탈퇴 리스트를 순회하며 24시간이 지난 유저는 탈퇴시킨다.

	문제 소지가 있으면 바로 수정해야함...Critical
*/
struct GuildMemberID
{
	CHAR m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR m_szMemberID[AGPACHARACTER_MAX_ID_STRING+1];
};

VOID AgsmGuild::CheckLeaveGuildList( UINT32 ulClockCount )
{
	if(m_ulLastProcessLeaveGuildClock == 0)
	{
		m_ulLastProcessLeaveGuildClock = ulClockCount;
		return;
	}

	// 시간 체크
	if((ulClockCount - m_ulLastProcessLeaveGuildClock) < AGSMGUILD_IDLE_TIME_INTERVAL)
		return;

	m_ulLastProcessLeaveGuildClock = GetClockCount();

	UINT lCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	AgpdGuildRequestMember *pcsMember = NULL;

	ApSort < GuildMemberID * > ApLeaveMember;

	{
		AuAutoLock Lock(m_ApGuildLeaveList.m_Mutex);
		if (!Lock.Result()) return;

		for(pcsMember = m_ApGuildLeaveList.GetHead();!m_ApGuildLeaveList.IsEnd();pcsMember = m_ApGuildLeaveList.GetNext())
		{
			if (!pcsMember) break;
			if (lCurrentTime - pcsMember->m_lLeaveRequestTime > AGPMGUILD_LEAVE_NEED_TIME)
			{
		//	대기시간 24시간이 지났으므로 길드에서 탈퇴시켜 버린다.
				GuildMemberID *pGuildMemberID = new GuildMemberID;
				memset(pGuildMemberID, 0, sizeof(GuildMemberID));
				strncpy(pGuildMemberID->m_szGuildID, pcsMember->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
				strncpy(pGuildMemberID->m_szMemberID, pcsMember->m_szMemberID, AGPACHARACTER_MAX_ID_STRING);
				ApLeaveMember.Insert(pGuildMemberID);
			}
		}
	}

	GuildMemberID *pGuildMemberID = NULL;
	for(pGuildMemberID = ApLeaveMember.GetHead();!ApLeaveMember.IsEnd();pGuildMemberID = ApLeaveMember.GetNext())
	{
		if(!pGuildMemberID) break;
		m_pagpmGuild->OnOperationLeaveAllow(1, pGuildMemberID->m_szGuildID, pGuildMemberID->m_szMemberID, FALSE );
		delete pGuildMemberID;
	}
}

/*
	2005.07.27. By SungHoon
	탈퇴 리스트에 추가한다.
*/
BOOL AgsmGuild::RequestLeaveMemberSelf(CHAR *szGuildID, CHAR  *szMemberID)
{
	AgpdGuild *pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	AgpdGuildMember *pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsGuildMember)
	{
		pcsGuild->m_Mutex.Release();
		JoinRequestReject(szGuildID, szMemberID);
		return FALSE;
	}
	pcsGuildMember->m_lRank = AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST;
	pcsGuildMember->m_lJoinDate = m_pagpmGuild->GetCurrentTimeStamp();

	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, szGuildID, pcsGuildMember);

	SendLeaveToOtherMembers(pcsGuild, szMemberID);
	SendLeaveMember(pcsGuild, szMemberID, 1, GetMemberNID(pcsGuildMember));

	pcsGuild->m_Mutex.Release();

	return InsertLeaveMemberSelf(szGuildID, szMemberID, m_pagpmGuild->GetCurrentTimeStamp());
}

BOOL AgsmGuild::InsertLeaveMemberSelf(CHAR *szGuildID, CHAR *szMemberID , INT32 lLeaveRequestTime)
{
	AgpdGuildRequestMember *pcsMember = new AgpdGuildRequestMember;
	pcsMember->m_lLeaveRequestTime = lLeaveRequestTime;
	strncpy(pcsMember->m_szMemberID, szMemberID, AGPDCHARACTER_MAX_ID_LENGTH);
	strncpy(pcsMember->m_szGuildID, szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH);

	m_ApGuildLeaveList.Insert(pcsMember);

	return TRUE;
}

/*
	2005.07.27. By SungHoon
	길드 가입 리스트에 추가한다.
*/
BOOL AgsmGuild::RequestJoinMemberSelf(CHAR* szGuildID, CHAR* szCharID, INT32 lMemberRank, INT32 lJoinDate,INT32 lLevel, INT32 lTID)
{
	INT32 lCID = -1;
	UINT32 ulNID = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	{
		ApAutoLockCharacter Lock(m_pagpmCharacter, szCharID);
		AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();

		if (pcsCharacter)
		{
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (pcsAgsdCharacter) ulNID = pcsAgsdCharacter->m_dpnidCharacter;

			AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
			if(pcsAttachedGuild)
			{
				if (strlen(pcsAttachedGuild->m_szGuildID) > 0 || strlen(pcsAttachedGuild->m_szRequestJoinGuildID) > 0)
				{
					SendSystemMessage(AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER, ulNID);
					return TRUE;
				}
			}
			if (lLevel == -1) lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			if (lTID == -1) lTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
			if (lJoinDate == -1) lJoinDate = m_pagpmGuild->GetCurrentTimeStamp();

			memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0 , AGPMGUILD_MAX_GUILD_ID_LENGTH + 1);
			strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH); 
		}
	}
	AgpdGuildRequestMember *pcsRequestMember = new AgpdGuildRequestMember();
	pcsRequestMember->m_lTID = lTID;
	pcsRequestMember->m_lLevel = lLevel;
	pcsRequestMember->m_lLeaveRequestTime = lJoinDate;
	strncpy(pcsRequestMember->m_szMemberID, szCharID, AGPDCHARACTER_MAX_ID_LENGTH);
	strncpy(pcsRequestMember->m_szGuildID, szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH);
	pcsRequestMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;

	pcsGuild->m_pGuildJoinList->AddObject((PVOID)&pcsRequestMember, pcsRequestMember->m_szMemberID);

	SendCreateGuild(pcsGuild, lCID, ulNID);
	SendSelfJoinMember(szGuildID, pcsRequestMember, lCID, ulNID);
	SendCharGuildData(pcsRequestMember, lCID, ulNID);

	SendNewMemberSelfJoinToOtherMembers(szGuildID, pcsRequestMember, lCID);

	SendRequestJoinMemberUpdateToMembers(pcsGuild, pcsRequestMember, FALSE);

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

/*
	2005.08.01. By SungHoon
	길드 가입 신청 중인 유저에게 가입허가를 낼 경우
*/
BOOL AgsmGuild::AllowJoinMember(CHAR *szGuildID, CHAR  *szMemberID)
{
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT32 lServerID = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

//	가입대기 리스트에서 삭제하구
	AgpdGuildRequestMember *pcsRequestMember = GetJoinRequestMember(pcsGuild, szMemberID);
	if (!pcsRequestMember) return FALSE;

	// Member Data 를 제대로 세팅해준다.
	INT32 lLevel = pcsRequestMember->m_lLevel;
	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	INT32 lTID = pcsRequestMember->m_lTID;

	INT8 cStatus = pcsRequestMember->m_cStatus;;

	if (pcsGuild->m_pGuildJoinList->RemoveObject(szMemberID) == TRUE)
		delete pcsRequestMember;

	pcsGuild->m_Mutex.Release();

	//	실제 멤버 리스트에 추가
	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsMember)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	pcsMember->m_lLevel = lLevel;
	pcsMember->m_lTID = lTID;
	pcsMember->m_cStatus = cStatus;
	pcsMember->m_lJoinDate = m_pagpmGuild->GetCurrentTimeStamp();
	pcsMember->m_lRank = AGPMGUILD_MEMBER_RANK_NORMAL;

	{
		ApAutoLockCharacter Lock(m_pagpmCharacter, szMemberID);
		AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
		if(pcsCharacter)
			GuildJoinSuccess(pcsGuild, pcsMember, pcsCharacter, FALSE);
	}

//	가입되었다고 내려준다. 클라이언트에서는 같은 목록을 사용하므로 더이상 Join하지 않는다.
	SendMemberUpdateToMembers(pcsGuild, pcsMember);		

	//////////////////////////////////////////////////////////////////////////
	// DB 에 쓴다.
	// Member Insert
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, szGuildID, pcsMember);

	if (m_pagpmLog)
	{
		CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
		ZeroMemory(szDesc, sizeof(szDesc));
		sprintf(szDesc, "Master=[%s]", pcsGuild->m_szMasterID);

		m_pagpmLog->WriteLog_GuildIn(0, 
										"",
										"",
										"",
										pcsMember->m_szID,
										pcsMember->m_lTID,
										pcsMember->m_lLevel,
										0,
										0,
										0,
										szGuildID,
										szDesc
										);
	}

	// Unlock
	pcsGuild->m_Mutex.Release();

	return TRUE;
}

/*
	2005.08.01. By SungHoon
	szMemberID를 길드 탈퇴 리스트에서 제거한다.
*/
BOOL AgsmGuild::RemoveLeaveMember(CHAR *szMemberID)
{
	AgpdGuildRequestMember *pcsMember = NULL;
	AuAutoLock Lock(m_ApGuildLeaveList.m_Mutex);
	if (!Lock.Result()) return FALSE;

	for(pcsMember = m_ApGuildLeaveList.GetHead();!m_ApGuildLeaveList.IsEnd();)
	{
		if (!pcsMember) break;
		if (!strcmp(pcsMember->m_szMemberID, szMemberID))
		{
			delete pcsMember;
			pcsMember = m_ApGuildLeaveList.Erase();
		}
		else pcsMember = m_ApGuildLeaveList.GetNext();
	}
	return FALSE;
}

/*
	2005.08.02. By SungHoon
	가입 신청을 취소한다.
*/
BOOL AgsmGuild::JoinRequestReject(CHAR *szGuildID, CHAR *szMemberID)
{
	ApAutoLockCharacter Lock(m_pagpmCharacter, szMemberID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;
	AgpdGuildADChar* pcsAgsdCharacter = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter) return FALSE;
	if (strcmp(szGuildID,pcsAgsdCharacter->m_szRequestJoinGuildID)) return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAgsdCharacter->m_szRequestJoinGuildID);
	if(!pcsGuild)
		return FALSE;

//	가입대기 리스트에서 삭제하구
	AgpdGuildRequestMember *pcsRequestMember = GetJoinRequestMember(pcsGuild, szMemberID);
	if (!pcsRequestMember)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	if (pcsGuild->m_pGuildJoinList->RemoveObject(szMemberID) == TRUE)
		delete pcsRequestMember;

	pcsGuild->m_Mutex.Release();

	Leave(szGuildID, szMemberID);
	return TRUE;
}
/*
	2005.08.17. By SungHoon
	길드 해체가 가능한지 체크한다.
*/
BOOL AgsmGuild::CBDestroyGuildCheckTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdGuild * pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pResult = (BOOL*)pCustData;

	*pResult = TRUE;
	// 하루가 경과하지 않았으면		2006.07.19. By SungHoon
	UINT32 lCurrentTime = pThis->m_pagpmGuild->GetCurrentTimeStamp();
	if ( lCurrentTime - pcsGuild->m_lCreationDate < AGPMGUILD_GUILD_DESTROY_TIME )
		*pResult = FALSE;
	return TRUE;
}

/*
	2005.08.16. By SungHoon
	함수 호출 일원화를 위해 길드에 가입을 성공했을 경우 불리워지는 것들을 묶었다.
*/
VOID AgsmGuild::GuildJoinSuccess(AgpdGuild *pcsGuild, AgpdGuildMember *pcsGuildMember, AgpdCharacter *pcsCharacter, BOOL bCreate)
{
	if (!pcsGuild || !pcsGuildMember || !pcsCharacter) return;
	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter) return;

	INT32 lCID = pcsCharacter->m_lID;
	UINT32 ulNID = pcsAgsdCharacter->m_dpnidCharacter;
	INT32 lServerID = pcsAgsdCharacter->m_ulServerID;

	// 새로 가입한 멤버에게 Guild 생성 패킷을 보낸다.
	if (bCreate) SendCreateGuild(pcsGuild, lCID, ulNID);

	// 새로 가입한 놈의 자기 정보 세팅
	m_pagpmGuild->SetCharAD(pcsCharacter, pcsGuild->m_szID, pcsGuild->m_lGuildMarkTID, pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsGuildMember);
	AgsdGuildMember* pcsAgsdMember = GetADMember(pcsGuildMember);
	if(pcsAgsdMember)
		UpdateAgsdGuildMember(pcsAgsdMember, lServerID, ulNID, pcsCharacter);

	// 가입한 멤버에게 모든 멤버의 정보를 보내준다. (Join Packet 으로.. 자기 포함)
	if (bCreate) SendAllMemberJoin(pcsGuild, lCID, ulNID);
	else SendOtherMemberMemberJoin(pcsGuild, lCID, ulNID, pcsCharacter->m_szID);

	// 새로 가입한 멤버에게 자기의 정보를 보내준다.
	SendCharGuildData(pcsGuild->m_szID, pcsCharacter->m_szID, pcsGuildMember, lCID, ulNID);

	// 주변에 길드아이디를 뿌린다.
	SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
				pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, ulNID);

	//	길드에 가입했을 경우 길드초대 거부 플래그를 셋팅한다.	2005.06.07 By SungHoon
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);

	EnumCallback(AGSMGUILD_CB_SEND_GUILD_INFO, pcsCharacter, NULL);
}

/*
	2005.08.18. By SungHoon
	가입 대기중인 멤버를 찾아낸다.
*/
AgpdGuildRequestMember *AgsmGuild::GetJoinRequestMember(AgpdGuild *pcsGuild, CHAR *szID)
{
	if(!pcsGuild || !szID)
		return NULL;

	AgpdGuildRequestMember **ppcsRequestMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObject(szID);
	if (!ppcsRequestMember) return NULL;
	return ( *ppcsRequestMember );
}

/*
	2005.08.24. By SungHoon
	길드 이름 변경을 클라이언트로 내려준다.
*/
BOOL AgsmGuild::ProcessAllMemberRenameGuildID(INT32 lCID, CHAR *szOldGuildID, CHAR *szMemberID, AgpdGuild *pcsGuild, PVOID pvRenamePacket , INT16 nPacketLength )
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsMember)->m_szID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			// 새로 가입한 놈의 자기 정보 세팅
			m_pagpmGuild->SetCharAD(pcsCharacter, pcsGuild->m_szID, pcsGuild->m_lGuildMarkTID, pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, *ppcsMember);
			AgsdGuildMember* pcsAgsdMember = GetADMember(*ppcsMember);
			if(pcsAgsdMember)
				UpdateAgsdGuildMember(pcsAgsdMember, pcsAgsdCharacter->m_ulServerID, pcsAgsdCharacter->m_dpnidCharacter, pcsCharacter);

			SendPacket(pvRenamePacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);		//	길드 이름 바꼈다는 패킷 전송
			SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, 
				NULL, NULL, FALSE, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;
}

/*
	2005.08.24. By SungHoon
	길드 이름 변경을 가입대기 상태인 클라이언트로 내려준다.
*/
BOOL AgsmGuild::ProcessAllRequestJoinMemberRenameGuildID(INT32 lCID, CHAR *szOldGuildID, CHAR *szMemberID, AgpdGuild *pcsGuild, PVOID pvRenamePacket, INT16 nPacketLength )
{
	//STOPWATCH2(GetModuleName(), _T("ProcessAllRequestJoinMemberRenameGuildID"));

	INT32 lIndex = 0;
	AgpdGuildRequestMember **ppcsRequestJoinMember = NULL;

//	가입대기중인 사용자에게도 길드이름이 변경되었음으로 내려준다.
	for(ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsRequestJoinMember;
		ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsRequestJoinMember || !*ppcsRequestJoinMember)
			break;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsRequestJoinMember)->m_szMemberID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;

			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
			if(pcsAttachedGuild)
			{
				memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0 , AGPMGUILD_MAX_GUILD_ID_LENGTH + 1);
				strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, pcsGuild->m_szID,AGPMGUILD_MAX_GUILD_ID_LENGTH); 

				memset((*ppcsRequestJoinMember)->m_szGuildID, 0 , AGPMGUILD_MAX_GUILD_ID_LENGTH + 1);
				strncpy((*ppcsRequestJoinMember)->m_szGuildID, pcsGuild->m_szID,AGPMGUILD_MAX_GUILD_ID_LENGTH);

				SendPacket(pvRenamePacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);		//	길드 이름 바꼈다는 패킷 전송
			}
		}
	}
	return TRUE;
}

/*
	2005.08.24. By SungHoon
	길드 아이디 변경을 요청한다.
*/
BOOL AgsmGuild::CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild *pThis = (AgsmGuild *)pClass;
	INT32 *pCID = ( INT32 *)pData;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR *szGuildID = ( CHAR *)ppvBuffer[0];
	CHAR *szMemberID = ( CHAR *)ppvBuffer[1];

	return (pThis->RenameGuild(*pCID, szGuildID, szMemberID));
}

/*
	2005.08.25. By SungHoon
	실제로 길드이름을 바꾸는 로직을 진행한다.
*/
BOOL AgsmGuild::RenameGuild(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID)
{
	// Guild Name 특수문자 체크
	INT8 cErrorCode = -1;
	CHAR szOldGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	memset(szOldGuildID, 0 ,AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
	UINT32 ulNID = 0;

	ApAutoLockCharacter Lock(m_pagpmCharacter, szMemberID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;
	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if (!pcsAttachedGuild) return FALSE;
	strncpy(szOldGuildID, pcsAttachedGuild->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	if(cErrorCode == -1)
	{
		if(!CheckGuildID(szGuildID))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_USE_SPECIAL_CHAR;
	}

	// Guild Name 중복 체크
	if(cErrorCode == -1)
	{
		if(m_pagpmGuild->GetGuild(szGuildID))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_EXIST_GUILD_ID;
	}
	//	길마인지 확인 하구
	if(cErrorCode == -1)
	{
		if (m_pagpmGuild->IsMaster(szOldGuildID, szMemberID) == FALSE) 
			cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_NOT_MASTER;
	}
	//	이전 길드이름에 @가 들어있는지 확인해야 한다.
	if(cErrorCode == -1)
	{
		if (m_pagpmGuild->IsDuplicatedCharacterOfMigration(szOldGuildID)==FALSE)
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_IMPOSIBLE_GUILD_ID;
			}
	}

	// Error 가 발생했으면, 메시지를 보낸다.
	if(cErrorCode != -1)
	{
		SendSystemMessage(cErrorCode, ulNID);
		return FALSE;
	}

	AgpdGuild *pcsGuild = m_pagpmGuild->GetGuildLock(szOldGuildID);
	if (!pcsGuild) return FALSE;

	if (m_pagpmGuild->RenameGuildID(pcsGuild, szGuildID) == FALSE)		//	이름바꾸기에 실패하면 이미 있다고 본다.
	{
		m_pagpmGuild->RenameGuildID(pcsGuild, szOldGuildID);
		pcsGuild->m_Mutex.Release();
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_RENAME_IMPOSIBLE_GUILD_ID, ulNID);
		return FALSE;
	}

//	이 쯤에서 DB에 Update한다.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_RENAME, szOldGuildID, szGuildID);

	// Log
	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	m_pagpmLog->WriteLog_GuildRename(0,
									 pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : "",
									 pAgsdCharacter ? pAgsdCharacter->m_szAccountID : "",
									 pAgsdCharacter ? pAgsdCharacter->m_szServerName : "",
									 pcsCharacter->m_szID,
									 pcsCharacter->m_pcsCharacterTemplate->m_lID,
									 m_pagpmCharacter->GetLevel(pcsCharacter),
									 m_pagpmCharacter->GetExp(pcsCharacter),
									 pcsCharacter->m_llMoney,
									 pcsCharacter->m_llBankMoney,
									 szGuildID,
									 szOldGuildID
									 );

//	전멤버와 근처의 유저한테 통보하구
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildRenameGuildIDPacket(&lCID, szGuildID, szMemberID, &nPacketLength );

	if(!pvPacket || nPacketLength < 1)
		return FALSE;
	ProcessAllMemberRenameGuildID( lCID, szOldGuildID, szMemberID, pcsGuild, pvPacket , nPacketLength);
	ProcessAllRequestJoinMemberRenameGuildID( lCID, szOldGuildID, szMemberID, pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
//	락풀고
	pcsGuild->m_Mutex.Release();
	return TRUE;
}


/*
	2005.09.05. By SungHoon
	캐릭터 이름이 바뀔경우 AgsmLogin으로 부터 콜백 된다.
*/
BOOL AgsmGuild::CBGuildRenameCharacterID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData) return FALSE;
	AgsmGuild *pThis = (AgsmGuild *)pClass;
	CHAR *szOldID = (CHAR *)pData;
	CHAR *szNewID = (CHAR *)pCustData;

	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->FindGuildLockFromCharID(szOldID);
	if (!pcsGuild) return FALSE;

	BOOL bResult = pThis->GuildRenameCharacterID(pcsGuild, szOldID, szNewID);

	pcsGuild->m_Mutex.Release();

	return bResult;
}

/*
	2005.09.05. By SungHoon
	캐릭터 이름이 바뀔경우 길드원 모두에게 길드원의 이름이 변경되었음을 알려준다.
*/
BOOL AgsmGuild::GuildRenameCharacterID(AgpdGuild *pcsGuild, CHAR *szOldID, CHAR *szNewID)
{
	if (!pcsGuild || !szOldID || !szNewID) return FALSE;

	if (!m_pagpmGuild->RenameCharID(pcsGuild, szOldID, szNewID)) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildRenameCharacterIDPacket(pcsGuild->m_szID, szOldID, szNewID, &nPacketLength );
	if (!pvPacket) return FALSE;
	
	SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	SendPacketToAllJoinRequestMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

/*
	2005.10.19. By SungHoon
	길드 마크구입 요청을 한다. 
*/
BOOL AgsmGuild::CBGuildBuyGuildMark(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild *pThis = (AgsmGuild *)pClass;
	PVOID *ppBuffer = (PVOID *)pData;
	CHAR *szGuild = (CHAR *)pCustData;

	INT32 *plCID = ( INT32 *)ppBuffer[0];
	INT32 *pGuildMarkTID = ( INT32 *)ppBuffer[1];
	INT32 *pGuildMarkColor = ( INT32 *)ppBuffer[2];

	return ( pThis->GuildBuyGuildMark(*plCID, szGuild, *pGuildMarkTID, *pGuildMarkColor , FALSE) );
}

/*
	2005.10.19. By SungHoon
	길드 마크구입 요청을 한다. 중복을 무시한다.
*/
BOOL AgsmGuild::CBGuildBuyGuildMarkForce(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild *pThis = (AgsmGuild *)pClass;
	PVOID *ppBuffer = (PVOID *)pData;
	CHAR *szGuild = (CHAR *)pCustData;

	INT32 *plCID = ( INT32 *)ppBuffer[0];
	INT32 *pGuildMarkTID = ( INT32 *)ppBuffer[1];
	INT32 *pGuildMarkColor = ( INT32 *)ppBuffer[2];

	return ( pThis->GuildBuyGuildMark(*plCID, szGuild, *pGuildMarkTID, *pGuildMarkColor, TRUE ) );
}

/*
	2005.10.19. By SungHoon
	길드 마크구입 요청을 한다. pcsGuild는 Lock되어 들어온다.
*/
BOOL AgsmGuild::GuildBuyGuildMark(INT32 lCID, CHAR *szGuildID, INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL bForce)
{
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	{
		ApAutoLockCharacter Lock(m_pagpmCharacter, lCID);
		AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
		if (!pcsCharacter)
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}

		if (!m_pagpmGuild->IsMaster(pcsGuild,pcsCharacter->m_szID))
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}

//	2005.10.25. By SungHoon 임시로 테섭에만 주석처리
		INT32 lMemberCount = m_pagpmGuild->GetMemberCount(pcsGuild);
		if ( lMemberCount < AGPMGUILD_GUILDMARK_ENABLE_MEMBER_COUNT)
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}

		INT32 lResultCode = -1;
		if (bForce == FALSE && m_pagpmGuild->IsDuplicateGuildMark(lGuildMarkTID) == TRUE)
			lResultCode = AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_DUPLICATE;
		if (lResultCode == -1)
			lResultCode = m_pagpmGuild->IsEnableBuyGuildMark(lGuildMarkTID, pcsCharacter);
		if (lResultCode != -1) // 오류
		{
			SendSystemMessage(lResultCode, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}
		
		INT32 lGheld = SubItemBuyGuildMark(lGuildMarkTID, pcsCharacter);			//	아이템을 깍고

		pcsGuild->m_lGuildMarkTID = lGuildMarkTID;					//	길드에 길드 마크 셋팅하고
		pcsGuild->m_lGuildMarkColor = lGuildMarkColor;

		m_pagpmGuild->SetCharAD(pcsCharacter, szGuildID, lGuildMarkTID, lGuildMarkColor, pcsGuild->m_lBRRanking, NULL);

		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);		// DB에 저장하고

		//	로그 남기자.
		AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);

		CHAR szLogBuffer[65];
		wsprintf( szLogBuffer, "%d", lGuildMarkTID);
		m_pagpmLog->WriteLog_GuildBuyMark(0,
												pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : "",
												pAgsdCharacter ? pAgsdCharacter->m_szAccountID : "",
												pAgsdCharacter ? pAgsdCharacter->m_szServerName : "",
												pcsCharacter->m_szID,
												pcsCharacter->m_pcsCharacterTemplate->m_lID,
												m_pagpmCharacter->GetLevel(pcsCharacter),
												m_pagpmCharacter->GetExp(pcsCharacter),
												pcsCharacter->m_llMoney,
												pcsCharacter->m_llBankMoney,
												pcsGuild->m_szID,
												szLogBuffer,
												lGheld
												);

	}

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBuyGuildMarkPacket(&nPacketLength ,lCID, szGuildID, &lGuildMarkTID, 
		&lGuildMarkColor);

	if(!pvPacket || nPacketLength < 1)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	ProcessAllMemberBuyGuildMark(pcsGuild, pvPacket, nPacketLength);
//	ProcessAllRequestJoinMemberBuyGuildMark(pcsGuild, pvPacket, nPacketLength);		//	가입대기중인 유저는 길드마크 보여주지 않는다.

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	pcsGuild->m_Mutex.Release();

	return  TRUE;
}

/*
	2005.10.19. By SungHoon
	길드 마크변경을 클라이언트로 내려준다.
*/
BOOL AgsmGuild::ProcessAllMemberBuyGuildMark(AgpdGuild *pcsGuild, PVOID pvBuyPacket, INT16 nPacketLength )
{
	//STOPWATCH2(GetModuleName(), _T("ProcessAllMemberBuyGuildMark"));

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsMember)->m_szID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			SendPacket(pvBuyPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);		

			SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
								pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;
}

/*
	2005.10.19. By SungHoon
	길드 마크변경을 클라이언트로 내려준다.
*/
BOOL AgsmGuild::ProcessAllRequestJoinMemberBuyGuildMark(AgpdGuild *pcsGuild, PVOID pvBuyPacket, INT16 nPacketLength )
{
	INT32 lIndex = 0;
	AgpdGuildRequestMember **ppcsRequestJoinMember = NULL;

//	가입대기중인 사용자에게도 길드이름이 변경되었음으로 내려준다.
	for(ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsRequestJoinMember;
		ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsRequestJoinMember || !*ppcsRequestJoinMember)
			break;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsRequestJoinMember)->m_szMemberID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;

			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			SendPacket(pvBuyPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;
}

/*
	2005.10.21. By SungHoon
	길드 마크 구입에 필요한 아이템과 겔드를 소모시킨다.
	***주의 분명히 pcsCharacter가 Lock 이 걸린 상황에서 구입가능한 아이템과 겔드가 있는지 미리 확인해두어야 한다.
	AgpmGuild::IsEnableBuyGuildMark 로 확인가능
*/
INT32 AgsmGuild::SubItemBuyGuildMark(INT32 lGuildMarkTID, AgpdCharacter *pcsCharacter)
{
	AgpdGuildMarkTemplate *pGuildMarkTemplate[3];
	pGuildMarkTemplate[0] = m_pagpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_BOTTOM, lGuildMarkTID);
	pGuildMarkTemplate[1] = m_pagpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_PATTERN, lGuildMarkTID);
	pGuildMarkTemplate[2] = m_pagpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_SYMBOL, lGuildMarkTID);

	INT32 lGheld = 0;
	for ( int i = 0 ; i < 3; i++)
	{
		if (pGuildMarkTemplate[i])
		{
			lGheld += pGuildMarkTemplate[i]->m_lGheld;

			// 길드 정책 변경 - arycoat 2008.02
			// 필요해골 삭제
			/*if (pGuildMarkTemplate[i]->m_lSkullCount > 0)
			{
				AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter,pGuildMarkTemplate[i]->m_lSkullTID);
				if (pcsItem)
				{
					if (m_pagsmItem)	// log
						m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsCharacter->m_lID, pcsItem, pGuildMarkTemplate[i]->m_lSkullCount);
					m_pagpmItem->SubItemStackCount(pcsItem, pGuildMarkTemplate[i]->m_lSkullCount, TRUE);
				}
			}*/
		}
	}

	// apply tax	
	INT32 lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = (lGheld * lTaxRatio) / 100;
	}
	lGheld = lGheld + lTax;	
	
	m_pagpmCharacter->SubMoney(pcsCharacter,lGheld);
	m_pagpmCharacter->PayTax(pcsCharacter, lTax);

	return lGheld;
}

BOOL AgsmGuild::SendCharGuildID(CHAR* szGuildID, CHAR* szCharID, AuPOS stPos, INT32 lGuildMarkTID, INT32 lGuildMarkColor, INT32 lBRRanking, UINT32 ulNID)
{
	if(!szCharID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szCharID, NULL, NULL, NULL, NULL, NULL,
									&lGuildMarkTID, &lGuildMarkColor, &lBRRanking);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SetWinnerGuild(CHAR* szGuildName, INT32 lBRRank)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	// 변경하고자 하는 길드가 존재하는지 확인
	AgpdGuild *pcsGuild = m_pagpmGuild->GetGuild(szGuildName);

	if (NULL == pcsGuild)
	{
		ASSERT(pcsGuild);
		return FALSE;
	}

	if(lBRRank < WINNER_GUILD_NOTRANKED || lBRRank >= WINNER_GUILD_MAX)
		return FALSE;

	INT32 index = 0;
	AgpdCharacter *pcsCharacter = NULL;
	AgpdGuildADChar* pcsAttachedGuild = NULL;
	AgpdGuildMember** ppcsMember = NULL;

	// 이전의 승리자 길드가 있는지 확인
	if (false == szWinnerGuildName[lBRRank].IsEmpty())
	{
		AgpdGuild *pcsWinnerGuild = m_pagpmGuild->GetGuild(szWinnerGuildName[lBRRank].GetBuffer());
		if (NULL != pcsWinnerGuild)
		{
			// 이전 길드는 승리길드 표시를 삭제한다.
			pcsWinnerGuild->m_lBRRanking = WINNER_GUILD_NOTRANKED;

			for(ppcsMember = (AgpdGuildMember**)pcsWinnerGuild->m_pMemberList->GetObjectSequence(&index); ppcsMember;
					ppcsMember = (AgpdGuildMember**)pcsWinnerGuild->m_pMemberList->GetObjectSequence(&index))
			{
				if(!ppcsMember || !*ppcsMember)
					continue;

				pcsCharacter = m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID);
				if (NULL == pcsCharacter)
					continue;

				pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
				if(!pcsAttachedGuild)
					continue;

				AuAutoLock lock(pcsCharacter->m_Mutex);
				if (lock.Result())
					pcsAttachedGuild->m_lBRRanking = WINNER_GUILD_NOTRANKED;
			}

			SendPacketWinnerInfo(pcsWinnerGuild);
		}
	}

	index = 0;
	pcsCharacter = NULL;
	pcsAttachedGuild = NULL;
	ppcsMember = NULL;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&index); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&index))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		pcsCharacter = m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID);
		if (NULL == pcsCharacter)
			continue;

		pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
		if(!pcsAttachedGuild)
			continue;

		AuAutoLock lock(pcsCharacter->m_Mutex);
		if (lock.Result())
			pcsAttachedGuild->m_lBRRanking = lBRRank; // BattleRoyal Ranking
	}

	szWinnerGuildName[lBRRank].SetText(szGuildName);
	pcsGuild->m_lBRRanking = lBRRank; // BattleRoyal Ranking

	SendPacketWinnerInfo(pcsGuild);

	return TRUE;	
}

BOOL AgsmGuild::SendPacketWinnerInfo(AgpdGuild* pcsGuild)
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsMember)->m_szID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			SendCharGuildID(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
				pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsAgsdCharacter->m_dpnidCharacter);

			SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
				pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;	
}

// Joint, Hostile 관련.
// 2006.07.13. steeple
BOOL AgsmGuild::CBJointRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szJointGuildID	= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID		= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->JointRequest(szGuildID, szJointGuildID, szMasterID);
}

BOOL AgsmGuild::CBJointReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	CHAR* szJointGuildID	= static_cast<CHAR*>(pCustData);

	return pThis->JointReject(szGuildID, szJointGuildID);
}

BOOL AgsmGuild::CBJoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis				= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID					= static_cast<CHAR*>(pData);
	AgpdGuildRelationUnit* pUnit	= static_cast<AgpdGuildRelationUnit*>(pCustData);

	return pThis->Joint(szGuildID, pUnit);
}

BOOL AgsmGuild::CBJointLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	INT32 lCID				= *static_cast<INT32*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szGuildID			= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szJointGuildID	= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->JointLeave(lCID, szGuildID);
}

BOOL AgsmGuild::CBHostileRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID		= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->HostileRequest(szGuildID, szHostileGuildID, szMasterID);
}

BOOL AgsmGuild::CBHostileReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(pCustData);

	return pThis->HostileReject(szGuildID, szHostileGuildID);
}

BOOL AgsmGuild::CBHostile(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis				= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID					= static_cast<CHAR*>(pData);
	AgpdGuildRelationUnit* pUnit	= static_cast<AgpdGuildRelationUnit*>(pCustData);

	return pThis->Hostile(szGuildID, pUnit);
}

BOOL AgsmGuild::CBHostileLeaveRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID		= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->HostileLeaveRequest(szGuildID, szHostileGuildID, szMasterID);
}

BOOL AgsmGuild::CBHostileLeaveReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(pCustData);

	return pThis->HostileLeaveReject(szGuildID, szHostileGuildID);
}

BOOL AgsmGuild::CBHostileLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	INT32 lCID				= *static_cast<INT32*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szGuildID			= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->HostileLeave(szGuildID, szHostileGuildID);
}

BOOL AgsmGuild::JointRequest(CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	// szGuildID 가 szJointGuildID 한테 신청한 것임.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szJointGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild1, szMasterID))
		return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));

	INT32 lCheck = CheckJointEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		SendSystemMessage(lCheck, ulMasterNID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID);

	// 패킷 전송.
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendJointRequest(pcsGuild2, pcsGuild1->m_szID, pcsGuild1->m_szMasterID, ulMaster2NID);

	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_WAIT, ulMasterNID);

	return TRUE;
}

BOOL AgsmGuild::JointReject(CHAR* szGuildID, CHAR* szJointGuildID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	// 신청은 szJointGuildID 가 한 것임.
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szJointGuildID);
	if(!pcsGuild2)
		return FALSE;

	AuAutoLock csLock2(pcsGuild2->m_Mutex);
	if (!csLock2.Result()) return FALSE;

	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_REJECT, ulMaster2NID);

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild2);	// 마지막 처리 길드 초기화
	return TRUE;
}

BOOL AgsmGuild::Joint(CHAR* szGuildID, AgpdGuildRelationUnit* pUnit)
{
	if(!szGuildID || !pUnit)
		return FALSE;

	// szGuildID 가 신청받은 놈이다. 고로 바꾼다.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(pUnit->m_szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));

	// 다시한번 되는 지 안되는 지 체크
	INT32 lCheck = CheckJointEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		// 여기서는 굳이 이유를 보내지 않고, 걍 실패로 보낸다.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);
		return FALSE;
	}

	// 제대로 온건지 확인해주고
	if(!m_pagpmGuild->IsLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID))
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1);	// 마지막 처리 길드 초기화

	// 자 이제 진짜 넣어준다.
	BOOL bResult = TRUE;
	UINT32 ulDate = m_pagpmGuild->GetCurrentTimeStamp();

	// 둘다 가입한 연대 없이 처음 시작하는 것이다.
	INT32 lGuild1JointCount = m_pagpmGuild->GetJointGuildCount(pcsGuild1);
	INT32 lGuild2JointCount = m_pagpmGuild->GetJointGuildCount(pcsGuild2);
	if(lGuild1JointCount == 0 && lGuild2JointCount == 0)
	{
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);	// 자기 자신도 넣는다.

		if(!bResult)
		{
			// 롤백
			m_pagpmGuild->RemoveJointGuild(pcsGuild1, pcsGuild1->m_szID);
			m_pagpmGuild->RemoveJointGuild(pcsGuild1, pcsGuild2->m_szID);
			m_pagpmGuild->RemoveJointGuild(pcsGuild2, pcsGuild1->m_szID);
			m_pagpmGuild->RemoveJointGuild(pcsGuild2, pcsGuild2->m_szID);

			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);
			return FALSE;
		}

		SendJoint(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		SendJoint(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJoint(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		SendJoint(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);

		SendJointDetail(pcsGuild1, pcsGuild1);
		SendJointDetail(pcsGuild1, pcsGuild2);
		SendJointDetail(pcsGuild2, pcsGuild1);
		SendJointDetail(pcsGuild2, pcsGuild2);

		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild1);
		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild2);

		// DB 에 써준다.
		EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild1, pcsGuild2->m_szID);
		EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild2, pcsGuild1->m_szID);
	}

	// 신청한 길드가 이미 연대에 가입중이다. 고로 추가 작업이 있다.
	else if(lGuild1JointCount > 0 && lGuild2JointCount == 0)
	{
		// 중간에 벡터가 변경되기 때문에, 이렇게 임시로 만들어서 임시를 돈다.
		JointVector vcTmp(pcsGuild1->m_csRelation.m_pJointVector->begin(), pcsGuild1->m_csRelation.m_pJointVector->end());
		JointIter iter = vcTmp.begin();
		while(iter != vcTmp.end())
		{
			AgpdGuild* pcsAlreadyJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(pcsAlreadyJoint)
			{
				AuAutoLock csLock(pcsAlreadyJoint->m_Mutex);
				if (csLock.Result())
				{
					bResult &= m_pagpmGuild->AddJointGuild(pcsAlreadyJoint, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					bResult &= m_pagpmGuild->AddJointGuild(pcsGuild2, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					if(!bResult)
					{
						// 롤백
						m_pagpmGuild->RemoveJointGuild(pcsAlreadyJoint, pcsGuild2->m_szID);
						m_pagpmGuild->RemoveJointGuild(pcsGuild2, iter->m_szGuildID);

						break;
					}

					SendJoint(pcsAlreadyJoint, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					SendJoint(pcsGuild2, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					SendJointDetail(pcsAlreadyJoint, pcsGuild2);
					SendJointDetail(pcsGuild2, pcsAlreadyJoint);

					SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS2, pcsAlreadyJoint, pcsGuild2->m_szID);

					// DB 에 써준다.
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsAlreadyJoint, pcsGuild2->m_szID);
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild2, pcsAlreadyJoint->m_szID);
				}
			}
			++iter;
		}

		if(!bResult)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);

			return FALSE;
		}

		// 자신도 추가해준다.
		m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJoint(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJointDetail(pcsGuild2, pcsGuild2);
		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild2);
	}

	// Guild2가 이미 연대에 가입중이다. 고로 추가 작업이 있다.
	else if(lGuild1JointCount == 0 && lGuild2JointCount > 0)
	{
		// 중간에 벡터가 변경되기 때문에, 이렇게 임시로 만들어서 임시를 돈다.
		JointVector vcTmp(pcsGuild2->m_csRelation.m_pJointVector->begin(), pcsGuild2->m_csRelation.m_pJointVector->end());
		JointIter iter = vcTmp.begin();
		while(iter != vcTmp.end())
		{
			AgpdGuild* pcsAlreadyJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(pcsAlreadyJoint)
			{
				AuAutoLock csLock(pcsAlreadyJoint->m_Mutex);
				if (csLock.Result())
				{
					bResult &= m_pagpmGuild->AddJointGuild(pcsAlreadyJoint, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					bResult &= m_pagpmGuild->AddJointGuild(pcsGuild1, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					if(!bResult)
					{
						// 롤백
						m_pagpmGuild->RemoveJointGuild(pcsAlreadyJoint, pcsGuild1->m_szID);
						m_pagpmGuild->RemoveJointGuild(pcsGuild1, iter->m_szGuildID);

						break;
					}

					SendJoint(pcsAlreadyJoint, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					SendJoint(pcsGuild1, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					SendJointDetail(pcsAlreadyJoint, pcsGuild1);
					SendJointDetail(pcsGuild1, pcsAlreadyJoint);

					SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS2, pcsAlreadyJoint, pcsGuild1->m_szID);

					// DB 에 써준다.
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsAlreadyJoint, pcsGuild1->m_szID);
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild1, pcsAlreadyJoint->m_szID);
				}
			}
			++iter;
		}

		if(!bResult)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);

			return FALSE;
		}

		// 자신도 추가해준다.
		m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJoint(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJointDetail(pcsGuild1, pcsGuild1);
		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild1);
	}

	return TRUE;
}

BOOL AgsmGuild::JointLeave(INT32 lCID, CHAR* szGuildID, BOOL bForce)
{
	if(!szGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = NULL;
	if(!bForce)
	{
		// szGuildID 가 연대 탈퇴를 보냈3.
		// 마스터인지 부터 확인.
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
		if(!pcsCharacter)
			return FALSE;

		pcsGuild = m_pagpmGuild->GetGuild(szGuildID);
		if(!pcsGuild)
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}

		BOOL bMaster = m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID);
		pcsCharacter->m_Mutex.Release();

		if(!bMaster)
			return FALSE;
	}
	else
	{
		pcsGuild = m_pagpmGuild->GetGuild(szGuildID);
		if(!pcsGuild)
			return FALSE;
	}

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	BOOL bIsLeaderLeave = FALSE;

	// 만약에 탈퇴하는 길드가 연대장이라면, 먼저가입한 순으로 정렬한다.
	if(m_pagpmGuild->IsJointLeader(pcsGuild))
	{
		bIsLeaderLeave = TRUE;
		std::sort(pcsGuild->m_csRelation.m_pJointVector->begin(), pcsGuild->m_csRelation.m_pJointVector->end(), SortRelation());
	}

	AgpdGuildRelationUnit stNewLeaderUnit;
	memset(&stNewLeaderUnit, 0, sizeof(stNewLeaderUnit));

	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		if(_tcscmp(pcsGuild->m_szID, iter->m_szGuildID) != 0)
		{
			if(bIsLeaderLeave && _tcslen(stNewLeaderUnit.m_szGuildID) == 0)
				stNewLeaderUnit = *iter;

			// 각 길드를 돌면서 szGuildID 를 빼줘야 한다.
			AgpdGuild* pcsOtherGuild = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(pcsOtherGuild)
			{
				AuAutoLock csLock2(pcsOtherGuild->m_Mutex);
				if (csLock2.Result())
				{
					JointLeave(pcsOtherGuild, szGuildID, stNewLeaderUnit);

					//m_pagpmGuild->RemoveJointGuild(pcsOtherGuild, pcsGuild->m_szID);
					SendJointLeave(pcsOtherGuild, pcsGuild->m_szID, stNewLeaderUnit.m_szGuildID);

					SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_OTHER_GUILD, pcsOtherGuild, pcsGuild->m_szID);

					// 만약에 size 가 1이 된다면 Clear 해준다. 연대해체인거다.
					if(pcsOtherGuild->m_csRelation.m_pJointVector->size() == 1)
					{
						m_pagpmGuild->ClearJointGuild(pcsOtherGuild);
						SendJointLeave(pcsOtherGuild, pcsOtherGuild->m_szID, stNewLeaderUnit.m_szGuildID);
						EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pcsOtherGuild, pcsGuild->m_szID);
						SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_DESTROY, pcsOtherGuild);
					}
				}
			}

			// 이건 pcsGuild 쪽에서 빼는거.
			EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pcsGuild, iter->m_szGuildID);
		}

		++iter;
	}

	// 클리어
	m_pagpmGuild->ClearJointGuild(pcsGuild);
	
	// Joint Leave 를 모든멤버에게 보낸다.
	SendJointLeave(pcsGuild, pcsGuild->m_szID, stNewLeaderUnit.m_szGuildID);

	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_SUCCESS, pcsGuild);

	return TRUE;
}

// szGuildID 가 연태를 탈퇴했다.
// stNewLeaderUnit 에는 새롭게 연대장이 될 길드정보가 온다.
BOOL AgsmGuild::JointLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, AgpdGuildRelationUnit& stNewLeaderUnit)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	// 자기가 탈퇴하는 경우는 여기 들어오나 마나다.
	if(_tcscmp(pcsGuild->m_szID, szGuildID) == 0)
		return FALSE;

	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		if(_tcscmp(iter->m_szGuildID, szGuildID) == 0)
		{
			// 나가는 길드는 제거
			EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pcsGuild->m_szID, szGuildID);
			iter = pcsGuild->m_csRelation.m_pJointVector->erase(iter);
			continue;
		}

		if(_tcscmp(iter->m_szGuildID, stNewLeaderUnit.m_szGuildID) == 0)
		{
			// 새로운 연대장 길드 업뎃
			iter->m_cRelation= (INT8)AGPMGUILD_RELATION_JOINT_LEADER;
			EnumCallback(AGSMGUILD_CB_DB_JOINT_UPDATE, pcsGuild, stNewLeaderUnit.m_szGuildID);
		}

		++iter;
	}

	return TRUE;
}

BOOL AgsmGuild::HostileRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// szGuildID 가 szJointGuildID 한테 신청한 것임.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild1, szMasterID))
		return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));

	INT32 lCheck = CheckHostileEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		SendSystemMessage(lCheck, ulMasterNID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID);

	// 패킷 전송.
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendHostileRequest(pcsGuild2, pcsGuild1->m_szID, pcsGuild1->m_szMasterID, ulMaster2NID);

	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_WAIT, ulMasterNID);

	return TRUE;
}

BOOL AgsmGuild::HostileReject(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// 신청은 szHostileGuildID 가 한 것임.
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);
	if(!pcsGuild2)
		return FALSE;

	AuAutoLock csLock2(pcsGuild2->m_Mutex);
	if (!csLock2.Result()) return FALSE;

	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_REJECT, ulMaster2NID);

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild2);	// 마지막 처리 길드 초기화
	return TRUE;
}

BOOL AgsmGuild::Hostile(CHAR* szGuildID, AgpdGuildRelationUnit* pUnit)
{
	if(!szGuildID || !pUnit)
		return FALSE;

	// szGuildID 가 신청받은 놈이다. 고로 바꾼다.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(pUnit->m_szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));

	// 다시한번 되는 지 안되는 지 체크
	INT32 lCheck = CheckHostileEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		// 여기서는 굳이 이유를 보내지 않고, 걍 실패로 보낸다.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	// 제대로 온건지 확인해주고
	if(!m_pagpmGuild->IsLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID))
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1);	// 마지막 처리 길드 초기화

	// 자 이제 진짜 넣어준다.
	BOOL bResult = TRUE;
	UINT32 ulDate = m_pagpmGuild->GetCurrentTimeStamp();
	
	bResult &= m_pagpmGuild->AddHostileGuild(pcsGuild1, pcsGuild2->m_szID, ulDate);
	bResult &= m_pagpmGuild->AddHostileGuild(pcsGuild2, pcsGuild1->m_szID, ulDate);

	if(!bResult)
	{
		m_pagpmGuild->RemoveHostileGuild(pcsGuild1, pcsGuild2->m_szID);
		m_pagpmGuild->RemoveHostileGuild(pcsGuild2, pcsGuild1->m_szID);

		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	// 성공했다.
	SendHostile(pcsGuild1, pcsGuild2->m_szID, ulDate);
	SendHostile(pcsGuild2, pcsGuild1->m_szID, ulDate);

	SendHostileDetail(pcsGuild1, pcsGuild2);
	SendHostileDetail(pcsGuild2, pcsGuild1);

	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_SUCCESS, pcsGuild1);
	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_SUCCESS, pcsGuild2);

	// DB 에 써준다.
	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_INSERT, pcsGuild1, pcsGuild2->m_szID);
	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_INSERT, pcsGuild2, pcsGuild1->m_szID);

	return TRUE;
}

BOOL AgsmGuild::HostileLeaveRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// szGuildID 가 szJointGuildID 한테 신청한 것임.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild1, szMasterID))
		return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));

	INT32 lCheck = CheckHostileLeaveEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		SendSystemMessage(lCheck, ulMasterNID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID);

	// 패킷 전송.
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendHostileLeaveRequest(pcsGuild2, pcsGuild1->m_szID, pcsGuild1->m_szMasterID, ulMaster2NID);

	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_WAIT, ulMasterNID);

	return TRUE;
}

BOOL AgsmGuild::HostileLeaveReject(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// 신청은 szHostileGuildID 가 한 것임.
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);
	if(!pcsGuild2)
		return FALSE;

	AuAutoLock csLock2(pcsGuild2->m_Mutex);
	if (!csLock2.Result()) return FALSE;

	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_REJECT, ulMaster2NID);

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild2);	// 마지막 처리 길드 초기화
	return TRUE;
}

BOOL AgsmGuild::HostileLeave(CHAR* szGuildID, CHAR* szHostileGuildID, BOOL bForce)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// szHostileGuildID 가 신청한 길드임.
	
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szHostileGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));

	// 제대로 온건지 확인해주고
	if(!bForce && !m_pagpmGuild->IsLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID))
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1);	// 마지막 처리 길드 초기화

	// 빼주면 된다.
	m_pagpmGuild->RemoveHostileGuild(pcsGuild1, pcsGuild2->m_szID);
	m_pagpmGuild->RemoveHostileGuild(pcsGuild2, pcsGuild1->m_szID);

	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, pcsGuild1, pcsGuild2->m_szID);
	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, pcsGuild2, pcsGuild1->m_szID);

	SendHostileLeave(pcsGuild1, pcsGuild2->m_szID);
	SendHostileLeave(pcsGuild2, pcsGuild1->m_szID);

	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_SUCCESS, pcsGuild1);
	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_SUCCESS, pcsGuild2);

	return TRUE;
}

// pcsGuild1 이 신청한 길드, pcsGuild2 가 신청 받는 길드이다.
INT32 AgsmGuild::CheckJointEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if(!pcsGuild1 || !pcsGuild2)
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;

	if(m_pagpmGuild->IsJointGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_ALREADY_JOINT;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;
	if(m_pagpmGuild->GetJointGuildCount(pcsGuild1) >= AGPMGUILD_MAX_JOINT_GUILD)
		return AGPMGUILD_SYSTEM_CODE_JOINT_MAX;
	if(m_pagpmGuild->GetJointGuildCount(pcsGuild1) > 0 &&	// 이미 연대에 가입해 있는데, 연대장이 아니다.
		!m_pagpmGuild->IsJointLeader(pcsGuild1))
		return AGPMGUILD_SYSTEM_CODE_JOINT_NOT_LEADER;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild1) < AGPMGUILD_MIN_JOINT_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_JOINT_NOT_ENOUGH_MEMBER;
#endif

	if(m_pagpmGuild->IsJointGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;
	if(m_pagpmGuild->GetJointGuildCount(pcsGuild2) >= AGPMGUILD_MAX_JOINT_GUILD)		// 이미 연대에 가입해 있다면 GG
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild2) < AGPMGUILD_MIN_JOINT_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_JOINT_OHTER_NOT_ENOUGH_MEMBER;
#endif

	//if(GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2)) == 0)	// 상대 길마가 오프라인.
	//	return AGPMGUILD_SYSTEM_CODE_JOINT_MASTER_OFFLINE;

	AgpdCharacter* pcsMaster2 = m_pagpmCharacter->GetCharacter(pcsGuild2->m_szMasterID);
	if(!pcsMaster2 || pcsMaster2->m_bIsReadyRemove)
		return AGPMGUILD_SYSTEM_CODE_JOINT_MASTER_OFFLINE;

	if(pcsMaster2->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION;

	return -1;
}

// pcsGuild1 이 신청한 길드, pcsGuild2 가 신청 받는 길드이다.
INT32 AgsmGuild::CheckHostileEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if(!pcsGuild1 || !pcsGuild2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;

	if(m_pagpmGuild->IsJointGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_ALREADY_HOSTILE;
	if(m_pagpmGuild->GetHostileGuildCount(pcsGuild1) >= AGPMGUILD_MAX_HOSTILE_GUILD)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_MAX;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild1) < AGPMGUILD_MIN_HOSTILE_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_NOT_ENOUGH_MEMBER;
#endif

	if(m_pagpmGuild->IsJointGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;
	if(m_pagpmGuild->GetHostileGuildCount(pcsGuild2) >= AGPMGUILD_MAX_HOSTILE_GUILD)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild2) < AGPMGUILD_MIN_HOSTILE_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_OTHER_NOT_ENOUGH_MEMBER;
#endif

	//if(GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2)) == 0)	// 상대 길마가 오프라인.
	//	return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	AgpdCharacter* pcsMaster2 = m_pagpmCharacter->GetCharacter(pcsGuild2->m_szMasterID);
	if(!pcsMaster2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	if(pcsMaster2->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION;

	return -1;
}

INT32 AgsmGuild::CheckHostileLeaveEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if(!pcsGuild1 || !pcsGuild2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_DISABLE;

	// 둘다 서로 적대관계여야지 뭐 해제 신청을 하던 말던 한다.
	if(!(m_pagpmGuild->IsHostileGuild(pcsGuild1, pcsGuild2->m_szID) && m_pagpmGuild->IsHostileGuild(pcsGuild2, pcsGuild1->m_szID)))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_DISABLE;

	//if(GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2)) == 0)	// 상대 길마가 오프라인.
	//	return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	AgpdCharacter* pcsMaster2 = m_pagpmCharacter->GetCharacter(pcsGuild2->m_szMasterID);
	if(!pcsMaster2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	if(pcsMaster2->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION;

	return -1;
}

// 2006.08.31. steeple
// 연대-적대 관계 유효성 체크. 서버 뜰 때 한번만 해준다.
BOOL AgsmGuild::CheckValidationForRelation()
{
	// 길드 전체를 돌면서 유효성 체크해준다.
	AgpdGuild** ppcsGuild = NULL;
	INT32 lIndex = 0;
	for(ppcsGuild = (AgpdGuild**)m_pagpmGuild->m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild;
		ppcsGuild = (AgpdGuild**)m_pagpmGuild->m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!*ppcsGuild)
			continue;

		AuAutoLock Lock1((*ppcsGuild)->m_Mutex);
		if (!Lock1.Result()) continue;

		JointIter iter = (*ppcsGuild)->m_csRelation.m_pJointVector->begin();
		while(iter != (*ppcsGuild)->m_csRelation.m_pJointVector->end())
		{
			if(_tcscmp((*ppcsGuild)->m_szID, iter->m_szGuildID) != 0 &&
				!m_pagpmGuild->IsJointGuild(iter->m_szGuildID, (*ppcsGuild)->m_szID))
			{
				EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, *ppcsGuild, iter->m_szGuildID);
				iter = (*ppcsGuild)->m_csRelation.m_pJointVector->erase(iter);
				continue;
			}
			
			++iter;
		}

		HostileIter iter2 = (*ppcsGuild)->m_csRelation.m_pHostileVector->begin();
		while(iter2 != (*ppcsGuild)->m_csRelation.m_pHostileVector->end())
		{
			if(_tcscmp((*ppcsGuild)->m_szID, iter2->m_szGuildID) != 0 &&
				!m_pagpmGuild->IsHostileGuild(iter2->m_szGuildID, (*ppcsGuild)->m_szID))
			{
				EnumCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, *ppcsGuild, iter2->m_szGuildID);
				iter2 = (*ppcsGuild)->m_csRelation.m_pHostileVector->erase(iter2);
				continue;
			}

			++iter2;
		}
	}

	return TRUE;
}

BOOL AgsmGuild::SendJointRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJointRequestPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, szMasterID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

BOOL AgsmGuild::SendJoint(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, INT8 cRelation, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJointPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, ulDate, cRelation);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendJointLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szNewLeader, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJointLeavePacket(&nPacketLength, 1, pcsGuild->m_szID, szGuildID, szNewLeader);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostileRequestPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, szMasterID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

BOOL AgsmGuild::SendHostile(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostilePacket(&nPacketLength, pcsGuild->m_szID, szGuildID, ulDate);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileLeaveRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostileLeaveRequestPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, szMasterID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

BOOL AgsmGuild::SendHostileLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostileLeavePacket(&nPacketLength, 1, pcsGuild->m_szID, szGuildID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendJointAll(AgpdGuild* pcsGuild, UINT32 ulNID)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket[AGPMGUILD_MAX_JOINT_GUILD] = {0,};

	INT32 lCount = 0;
	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		// Joint 보내고
		SendJoint(pcsGuild, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation, ulNID);

		AgpdGuild* pcsJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
		if(!pcsJoint)
		{
			++iter;
			continue;
		}

		AuAutoLock csLock(pcsJoint->m_Mutex);

		if (csLock.Result())
		{
			pvGuildListItemPacket[lCount] = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsJoint);
			if(++lCount >= AGPMGUILD_MAX_JOINT_GUILD)
				break;
		}

		++iter;
	}

	if(lCount == 0)
		return TRUE;

	nPacketLength = 0;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket[0],
											pvGuildListItemPacket[1],
											pvGuildListItemPacket[2],
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL
											);

	for(int i = 0; i < AGPMGUILD_MAX_JOINT_GUILD; ++i)
	{
		if( pvGuildListItemPacket[i] )
			m_pagpmGuild->m_csGuildListItemPacket.FreePacket( pvGuildListItemPacket[i] );
	}

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_JOINT_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket, 
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileAll(AgpdGuild* pcsGuild, UINT32 ulNID)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket[AGPMGUILD_MAX_HOSTILE_GUILD] = {0,};

	INT32 lCount = 0;
	HostileIter iter = pcsGuild->m_csRelation.m_pHostileVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pHostileVector->end())
	{
		SendHostile(pcsGuild, iter->m_szGuildID, iter->m_ulDate, ulNID);

		AgpdGuild* pcsHostile = m_pagpmGuild->GetGuild(iter->m_szGuildID);
		if(!pcsHostile)
		{
			++iter;
			continue;
		}

		AuAutoLock csLock(pcsHostile->m_Mutex);
		if (csLock.Result())
		{
			pvGuildListItemPacket[lCount] = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsHostile);
			if(++lCount >= AGPMGUILD_MAX_HOSTILE_GUILD)
				break;
		}

		++iter;
	}

	if(lCount == 0)
		return TRUE;

	nPacketLength = 0;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket[0],
											pvGuildListItemPacket[1],
											pvGuildListItemPacket[2],
											pvGuildListItemPacket[3],
											NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL,
											NULL,
											//pvGuildListItemPacket[4],
											//pvGuildListItemPacket[5],
											//pvGuildListItemPacket[6],
											//pvGuildListItemPacket[7],
											//pvGuildListItemPacket[8],
											//pvGuildListItemPacket[9],
											//pvGuildListItemPacket[10],
											//pvGuildListItemPacket[11],
											//pvGuildListItemPacket[12],
											//pvGuildListItemPacket[13],
											//pvGuildListItemPacket[14],
											NULL,
											NULL,
											NULL,
											NULL
											);

	for(int i = 0; i < AGPMGUILD_MAX_HOSTILE_GUILD; ++i)
	{
		if(pvGuildListItemPacket[i])
			m_pagpmGuild->m_csGuildListItemPacket.FreePacket(pvGuildListItemPacket[i]);
	}

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket, 
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendJointDetail(AgpdGuild* pcsGuild, AgpdGuild* pcsJoint, UINT32 ulNID)
{
	if(!pcsGuild || !pcsJoint)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsJoint);

	if(!pvGuildListItemPacket || nPacketLength < 1)
		return FALSE;

	nPacketLength = 0;
	INT32 lCount = 1;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL
											);
	m_pagpmGuild->m_csGuildListItemPacket.FreePacket( pvGuildListItemPacket );
	if(!pvGuildListPacket || nPacketLength < 1)
		return FALSE;

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_JOINT_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket,
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileDetail(AgpdGuild* pcsGuild, AgpdGuild* pcsHostile, UINT32 ulNID)
{
	if(!pcsGuild || !pcsHostile)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsHostile);
	if(!pvGuildListItemPacket || nPacketLength < 1)
		return FALSE;

	nPacketLength = 0;
	INT32 lCount = 1;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL
											);
	m_pagpmGuild->m_csGuildListItemPacket.FreePacket( pvGuildListItemPacket );
	if(!pvGuildListPacket || nPacketLength < 1)
		return FALSE;

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket, 
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::OnOperationWorldChampionshipRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST* pPacket = (PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST*)pData;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pPacket->lCID);
	if(!pcsCharacter)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->GetGuild(pcsCharacter);
	if(!pcsGuild || !pThis->m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
	{
		// 길드마스터가 아니므로 월드챔피언쉽 참가 신청을 할수 없음
		AgsdCharacter* pagsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pagsdCharacter)
			return FALSE;

		AuXmlElement* pElem = pThis->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_03");

		if(pElem)
		{
			PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT pPacket2;
			strcpy(pPacket2.strMessage, pElem->GetText());
			
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdCharacter->m_dpnidCharacter);
		}

		return FALSE;
	}

	PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_REQUEST pRelayPacket(pcsCharacter->m_szID, pcsGuild->m_szID);

	AgsdServer* pRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmGuild::OnOperationWorldChampionshipEnter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER* pPacket = (PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER*)pData;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pPacket->lCID);
	if(!pcsCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = pThis->m_pagsmServerManager->GetAimEventServer();
	if (NULL == pAgsdServer)
		return FALSE;

	PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_ENTER pRelayPacket(pcsCharacter->m_szID, pAgsdServer->m_lServerID, pAgsdServer->m_szWorld);

	AgsdServer* pRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

AuXmlNode* AgsmGuild::GetNoticeNodeWorldChampionship()
{
	static AuXmlNode* m_pNoticeNode = NULL;

	if(!m_pNoticeNode)
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("WorldChampionship");

	return m_pNoticeNode;
}

BOOL AgsmGuild::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild		*pThis			= (AgsmGuild *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	//return pThis->SendAllMail(pAgpdCharacter, ulNID);

	// PC 가 아니면 나간다.
	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter) == FALSE)
		return TRUE;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// 길드원이 아니면 나간다.
	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	pThis->SendCharGuildData(pcsAttachedGuild->m_szGuildID, pThis->m_pagsmCharacter->GetRealCharName(pcsCharacter->m_szID), NULL, 1, ulNID);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
AuXmlNode* AgsmGuild::GetNoticeNodeCSAppointment()
{
	static AuXmlNode* m_pNoticeNode = NULL;

	if(!m_pNoticeNode)
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("ClassSociety");

	return m_pNoticeNode;
}

BOOL AgsmGuild::OnOperationCSAppointmentRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST* pPacket = (PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST*)pData;

	switch(pPacket->Rank)
	{
		case AGPMGUILD_MEMBER_RANK_NORMAL:
			{
				pThis->GuildMemberDismissal(pPacket->strSChar, pPacket->strTChar, pPacket->Rank);
			} break;
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
			{
				pThis->GuildMemberAppointmentAsk(pPacket->strSChar, pPacket->strTChar, pPacket->Rank);
			} break;
		default:
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmGuild::OnOperationCSAppointmentAnswer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER* pPacket = (PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER*)pData;	
	
	// 마스터 체크
	AgpdCharacter* pcsMaster = pThis->m_pagpmCharacter->GetCharacter(pPacket->strMasterID);
	AgpdCharacter* pcsMember = pThis->m_pagpmCharacter->GetCharacter(pPacket->strChar);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	AgpdGuild* pcsGuild1 = pThis->m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = pThis->m_pagpmGuild->GetGuild(pcsMember);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	if(!pThis->m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	AgsdCharacter* pagsdMaster = pThis->m_pagsmCharacter->GetADCharacter(pcsMaster);
	AgsdCharacter* pagsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
	if(!pagsdMaster || !pagsdMember)
		return FALSE;

	if(pPacket->Answer == FALSE)
	{
		// 임명을 거부했을때
		AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_01");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	INT32 nRank = pPacket->Rank;
	switch(nRank)
	{
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			if(strlen(pcsGuild1->m_szSubMasterID) > 0)
			{
				// 부길마가 이미 있을때
				AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_02");
				if(pElem)
				{
					PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
					AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
				}

				return TRUE;
			}
		} break;
	default:
		{
			return FALSE;
		} break;
	}

	if(! pThis->GuildMemberRankUpdate(pcsMember, nRank))
		return FALSE;

	AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_03");
	if(pElem)
	{
		PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
		AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
	}

	return TRUE;
}

BOOL AgsmGuild::GuildMemberRankUpdate(AgpdCharacter* pcsCharacter, INT32 nRank)
{
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pcsCharacter);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if(!pcsGuildMember)
		return FALSE;

	INT32 nBeforeRank = pcsGuildMember->m_lRank;

	// 랭크설정
	pcsGuildMember->m_lRank = nRank;
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuild->m_szID, pcsGuildMember);

	switch(nRank)
	{
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
			{
				memcpy(pcsGuild->m_szSubMasterID, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
			} break;
		case AGPMGUILD_MEMBER_RANK_MASTER:
			{
				if(nBeforeRank != AGPMGUILD_MEMBER_RANK_SUBMASTER)
				{
					pcsGuildMember->m_lRank = nBeforeRank;
					EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuildMember->m_szID, pcsGuildMember);
					return FALSE;
				}

				memcpy(pcsGuild->m_szMasterID, pcsGuildMember->m_szID, AGPACHARACTER_MAX_ID_STRING);	
				ZeroMemory(pcsGuild->m_szSubMasterID, AGPACHARACTER_MAX_ID_STRING);	
			} break;
	}

	// 길드 데이터 업데이트
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

	PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK pUpdatePacket(pcsGuild->m_szID, pcsCharacter->m_szID, nRank);
	SendPacketToAllMembers(pcsGuild, pUpdatePacket);

	return TRUE;
}

BOOL AgsmGuild::GuildMemberRankUpdate(AgpdGuild* pcsGuild, AgpdGuildMember* pcsGuildMember, INT32 nRank)
{
	if(!pcsGuildMember || !pcsGuild)
		return FALSE;

	INT32 nBeforeRank = pcsGuildMember->m_lRank;

	// 랭크설정
	pcsGuildMember->m_lRank = nRank;
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuildMember->m_szID, pcsGuildMember);

	switch(nRank)
	{
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			memcpy(pcsGuild->m_szSubMasterID, pcsGuildMember->m_szID, AGPACHARACTER_MAX_ID_STRING);
		} break;
	case AGPMGUILD_MEMBER_RANK_MASTER:
		{
			if(nBeforeRank != AGPMGUILD_MEMBER_RANK_SUBMASTER)
			{
				pcsGuildMember->m_lRank = nBeforeRank;
				EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuildMember->m_szID, pcsGuildMember);
				return FALSE;
			}

			memcpy(pcsGuild->m_szMasterID, pcsGuildMember->m_szID, AGPACHARACTER_MAX_ID_STRING);	
			ZeroMemory(pcsGuild->m_szSubMasterID, AGPACHARACTER_MAX_ID_STRING);		
		} break;
	}

	// 길드 데이터 업데이트
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

	PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK pUpdatePacket(pcsGuild->m_szID, pcsGuildMember->m_szID, nRank);
	SendPacketToAllMembers(pcsGuild, pUpdatePacket);

	return TRUE;
}

BOOL AgsmGuild::OnOperationCSSuccessionRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST *pvPacket = (PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST*)(pData);
	
	AgpdCharacter *pcsMaster = pThis->m_pagpmCharacter->GetCharacter(pvPacket->strSChar);
	AgpdCharacter *pcsMember = pThis->m_pagpmCharacter->GetCharacter(pvPacket->strTChar);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	if(pThis->m_pagpmSiegeWar->GetSiegeWarByOwner(pcsMaster->m_szID))
		return FALSE;

	AgpdGuild* pcsGuild1 = pThis->m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = pThis->m_pagpmGuild->GetGuild(pcsMember);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	AgsdCharacter* pagsdMaster  = pThis->m_pagsmCharacter->GetADCharacter(pcsMaster);
	AgsdCharacter* pagsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
	if(!pagsdMember || !pagsdMaster)
		return FALSE;

	if(pThis->m_pagpmGuild->IsExceptionCharacterOfGuildRankSystem(pcsGuild2, pcsMember))
	{
		AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_05");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	// Check Master
	if(!pThis->m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	// Check SubMaster
	if(!pThis->m_pagpmGuild->IsSubMaster(pcsGuild1, pcsMember->m_szID))
		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK pPacket2(pcsMaster->m_szID, pcsGuild1->m_szID, pcsMember->m_szID);

	AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMember->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmGuild::OnOperationCSSuccessionAnswer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmGuild *pThis		    = static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER *pPacket = (PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER*)(pData);

	AgpdCharacter *pcsMaster    = pThis->m_pagpmCharacter->GetCharacter(pPacket->strMasterID);
	AgpdCharacter *pcsSuccessor = pThis->m_pagpmCharacter->GetCharacter(pPacket->strChar); 

	// Check Being Character
	if(!pcsMaster || !pcsSuccessor)
		return FALSE;

	// Check Being Guild and Same Guild
	AgpdGuild* pcsGuild1 = pThis->m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = pThis->m_pagpmGuild->GetGuild(pcsSuccessor);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	// Check Master
	if(!pThis->m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	// Check SubMaster
	if(!pThis->m_pagpmGuild->IsSubMaster(pcsGuild1, pcsSuccessor->m_szID))
		return FALSE;

	AgsdCharacter* pagsdMaster = pThis->m_pagsmCharacter->GetADCharacter(pcsMaster);
	if(!pagsdMaster)

		return FALSE;
	
	// Refuse a succession 
	if(!pPacket->Answer)
	{
		AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_01");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}
	
	if(! pThis->GuildMemberRankUpdate(pcsMaster, AGPMGUILD_MEMBER_RANK_NORMAL))
		return FALSE;
	
	if(! pThis->GuildMemberRankUpdate(pcsSuccessor, AGPMGUILD_MEMBER_RANK_MASTER))
		return FALSE;

	AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_04");
	if(pElem)
	{
		PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
		AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
	}
	
	return TRUE;
}

// 길드 멤버를 pPacket에 있는 Rank로 임명하기.
BOOL AgsmGuild::GuildMemberAppointmentAsk(CHAR *strMaster, CHAR *strMember, INT32 nRank)
{
	// 마스터 체크
	AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacter(strMaster);
	AgpdCharacter* pcsMember = m_pagpmCharacter->GetCharacter(strMember);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(pcsMember);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	AgsdCharacter* pagsdMaster = m_pagsmCharacter->GetADCharacter(pcsMaster);
	AgsdCharacter* pagsdMember = m_pagsmCharacter->GetADCharacter(pcsMember);
	if(!pagsdMaster || !pagsdMember)
		return FALSE;

	if(m_pagpmGuild->IsExceptionCharacterOfGuildRankSystem(pcsGuild2, pcsMember))
	{
		AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_05");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	if(!m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild2, pcsMember->m_szID);
	if(!pcsGuildMember)
		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK pPacket2(pcsMaster->m_szID, pcsGuild1->m_szID, pcsMember->m_szID, nRank);

	AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMember->m_dpnidCharacter);

	return TRUE;	
}

// 길드 멤버를 AGPMGUILD_MEMBER_RANK_NORMAL로 해임시켜라
BOOL AgsmGuild::GuildMemberDismissal(CHAR *strMaster, CHAR *strMember, INT32 nRank)
{
	// 마스터 체크
	AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacter(strMaster);
	AgpdCharacter* pcsMember = m_pagpmCharacter->GetCharacter(strMember);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pcsMaster);
	if(!pcsGuild)
		return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild, pcsMaster->m_szID))
		return FALSE;

	AgsdCharacter* pagsdMaster = m_pagsmCharacter->GetADCharacter(pcsMaster);
	if(!pagsdMaster)
		return FALSE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, strMember);
	if(!pcsGuildMember)
		return FALSE;

	INT32 Rank = nRank;
	if( pcsGuildMember->m_lRank == Rank)
	{
		AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_02");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	if(m_pagpmGuild->IsExceptionCharacterOfGuildRankSystem(pcsGuild, pcsMember))
	{
		AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_05");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	INT32 nBeforRank = pcsGuildMember->m_lRank;

	GuildMemberRankUpdate(pcsGuild, pcsGuildMember, Rank);

	switch(nBeforRank)
	{
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			memset(pcsGuild->m_szSubMasterID, 0, sizeof(pcsGuild->m_szSubMasterID));
		} break;
	}

	AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_03");
	if(pElem)
	{
		PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
		AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
	}

	return TRUE;
}

void AgsmGuild::LoadWinnerGuild()
{
	AuXmlDocument pDoc;

	if(!pDoc.LoadFile("WinnerGuild.xml"))
		return;

	AuXmlNode* pFileNode = pDoc.FirstChild("WinnerGuild");
	if(!pFileNode)
		return;

	for(AuXmlElement* pWinner = pFileNode->FirstChildElement("WINNER"); pWinner; pWinner = pWinner->NextSiblingElement())
	{
		CHAR* pszGuild = (CHAR*)pWinner->GetText();
		INT32 lRanking = 0;
		pWinner->Attribute("RANK", &lRanking);

		if(pszGuild && lRanking != WINNER_GUILD_NOTRANKED)
		{
			szWinnerGuildName[lRanking].SetText(pszGuild);
			SetWinnerGuild(pszGuild, lRanking);
		}
	}
}

void AgsmGuild::SaveWinnerGuild()
{
	AuXmlDocument pDoc;

	string encoding = "euc-kr";

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", encoding.c_str(), "no" ); 
	pDoc.LinkEndChild( decl ); 

	AuXmlElement * root = new AuXmlElement( "WinnerGuild" ); 
	pDoc.LinkEndChild( root );

	for(INT32 it = WINNER_GUILD_NOTRANKED; it < WINNER_GUILD_MAX; ++it)
	{
		if(szWinnerGuildName[it].IsEmpty())
			continue;

		AuXmlElement * pWinnerGuild = new AuXmlElement( "WINNER" ); 
		if( pWinnerGuild )
		{
			pWinnerGuild->LinkEndChild(new TiXmlText(szWinnerGuildName[it].GetBuffer()));
			pWinnerGuild->SetAttribute("RANK", it);

			root->LinkEndChild( pWinnerGuild );
		}
	}

	pDoc.SaveFile("WinnerGuild.xml");
}
