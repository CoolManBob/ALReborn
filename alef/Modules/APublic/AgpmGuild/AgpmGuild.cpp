// AgpmGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.


#include "AgpmGuild.h"
#include <time.h>
#include "ApMemoryTracker.h"
#include "ApAutoLockCharacter.h"
#include "AgppGuild.h"

#define AGPMGUILD_IDLE_TIME_INTERVAL	2000		// 2초에 한번

//////////////////////////////////////////////////////////////////////////

//--------------------------- CGuildMark ---------------------------



//--------------------------- AgpmGuild ---------------------------
AgpmGuild::AgpmGuild()
{
	SetModuleName("AgpmGuild");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	SetModuleData(sizeof(AgpdGuild));
	SetModuleData(sizeof(AgpdGuildMember), AGPMGUILD_DATA_TYPE_MEMBER);
	SetModuleData(sizeof(AgpdGuildBattlePoint), AGPMGUILD_DATA_TYPE_BATTLE_POINT);
	SetModuleData(sizeof(AgpdGuildMarkTemplate), AGPMGUILD_DATA_TYPE_GUILD_MARK_TEMPLATE);	//	2005.10.05. By SungHoon

	EnableIdle2(TRUE);

	m_pagpmCharacter = NULL;
	m_pagpmGrid = NULL;
	m_pagpmItem = NULL;

	m_ulLastRemoveClockCount = 0;
	m_nIndexADCharacter = 0;

	m_lBattlePointMinLevelGap = 0;
	m_lBattlePointMaxLevelGap = 0;

	// Packet Define
	SetPacketType(AGPMGUILD_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT32));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// CID
							AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,		// Guild ID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,		// Master ID
							AUTYPE_INT32,			1,	// Guild TID
							AUTYPE_INT32,			1,		// Guild Rank
							AUTYPE_INT32,			1,	// Creation Date
							AUTYPE_INT32,			1,	// Max Member Count
							AUTYPE_INT32,			1,	// Union ID
							AUTYPE_CHAR,			AGPMGUILD_MAX_PASSWORD_LENGTH + 1,		// Password
							AUTYPE_MEMORY_BLOCK,	1,	// Notice
							AUTYPE_INT8,			1,	// Status
							AUTYPE_INT32,			1,	// Win
							AUTYPE_INT32,			1,	// Draw
							AUTYPE_INT32,			1,	// Lose
							AUTYPE_INT32,			1,	// Guild Point
							AUTYPE_PACKET,			1,	// Member Packet
							AUTYPE_PACKET,			1,	// Battle Packet
							AUTYPE_PACKET,			1,	// BattlePerson Packet
							AUTYPE_PACKET,			1,	// BattleMember Packet
							AUTYPE_PACKET,			1,	// BattleMemberList Packet
							AUTYPE_PACKET,			1,	// GuildList Packet
							AUTYPE_INT32,			1,	// GuildMark TID
							AUTYPE_INT32,			1,	// GuildMark COlor
							AUTYPE_INT32,			1,	// Winner Guild (BOOL)
							AUTYPE_END,			0
							);

	m_csMemberPacket.SetFlagLength(sizeof(INT8));
	m_csMemberPacket.SetFieldType(
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,		// Member ID
							AUTYPE_INT32,			1,	// Member Rank
							AUTYPE_INT32,			1,	// Join Date
							AUTYPE_INT32,			1,	// Member Level
							AUTYPE_INT32,			1,	// Member TID
							AUTYPE_INT8,			1,	// Status
							AUTYPE_END,			0
							);

	m_csBattlePacket.SetFlagLength(sizeof(INT16));
	m_csBattlePacket.SetFieldType(
							AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,		// Enemy Guild ID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,		// Enemy Guild Master ID
							AUTYPE_UINT32,			1,	// Battle Accept Time
							AUTYPE_UINT32,			1,	// Battle Ready Time
							AUTYPE_UINT32,			1,	// Battle Start Time
							AUTYPE_UINT32,			1,	// Battle Mode
							AUTYPE_UINT32,			1,	// Battle Duration
							AUTYPE_UINT32,			1,	// Battle Person(참여인원)
							AUTYPE_UINT32,			1,	// Battle Current Time
							AUTYPE_INT32,			1,	// MyScore
							AUTYPE_INT32,			1,	// EnemyScore
							AUTYPE_INT32,			1,	// MyUpScore
							AUTYPE_INT32,			1,	// EnemyUpScore
							AUTYPE_INT8,			1,	// Battle Result
							AUTYPE_UINT32,			1,	// Round
							AUTYPE_END,			0
							);

	m_csBattlePersonPacket.SetFlagLength(sizeof(INT32));
	m_csBattlePersonPacket.SetFieldType(
						AUTYPE_INT16,			1,		//  게임에 참여하는 인원수(최대100명이다) 
						AUTYPE_MEMORY_BLOCK,	1,		//	메모리청크가 오는뎅 AGPACHARACTER_MAX_ID_STRING + 1로 끈어 읽자
						AUTYPE_END,			0
						);

	m_csBattleMemberPacket.SetFlagLength( sizeof(INT16) );
	m_csBattleMemberPacket.SetFieldType( 
							AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,		// Guild ID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,		// Char ID
							AUTYPE_UINT32,			1,	// Score
							AUTYPE_UINT32,			1,	// Kill
							AUTYPE_UINT32,			1,	// Death
							AUTYPE_END,				0 );

	m_csBattleMemberListPacket.SetFlagLength(sizeof(INT32));
	m_csBattleMemberListPacket.SetFieldType(
						AUTYPE_UINT32,			1,		//  My Member Count
						AUTYPE_MEMORY_BLOCK,	1,		//	My Battle맴버를 그냥 리스트로 쭉 보내버려~~~
						AUTYPE_UINT32,			1,		//  Enemy Member Count
						AUTYPE_MEMORY_BLOCK,	1,		//	Enemy Battle맴버를 그냥 리스트로 쭉 보내버려~~~
						AUTYPE_END,			0
						);

	m_csGuildListPacket.SetFlagLength(sizeof(INT32));
	m_csGuildListPacket.SetFieldType(
						AUTYPE_INT32,			1,		//  Total Guild Count
						AUTYPE_INT32,			1,		//  Page Count
						AUTYPE_INT16,			1,		//  Item Count
						AUTYPE_PACKET,			1,		//	1 GuildList Item
						AUTYPE_PACKET,			1,		//	2 GuildList Item
						AUTYPE_PACKET,			1,		//	3 GuildList Item
						AUTYPE_PACKET,			1,		//	4 GuildList Item
						AUTYPE_PACKET,			1,		//	5 GuildList Item
						AUTYPE_PACKET,			1,		//	6 GuildList Item
						AUTYPE_PACKET,			1,		//	7 GuildList Item
						AUTYPE_PACKET,			1,		//	8 GuildList Item
						AUTYPE_PACKET,			1,		//	9 GuildList Item
						AUTYPE_PACKET,			1,		//	10 GuildList Item
						AUTYPE_PACKET,			1,		//	11 GuildList Item
						AUTYPE_PACKET,			1,		//	12 GuildList Item
						AUTYPE_PACKET,			1,		//	13 GuildList Item
						AUTYPE_PACKET,			1,		//	14 GuildList Item
						AUTYPE_PACKET,			1,		//	15 GuildList Item
						AUTYPE_PACKET,			1,		//	16 GuildList Item
						AUTYPE_PACKET,			1,		//	17 GuildList Item
						AUTYPE_PACKET,			1,		//	18 GuildList Item
						AUTYPE_PACKET,			1,		//	19 GuildList Item
						AUTYPE_END,			0
						);

	m_csGuildListItemPacket.SetFlagLength(sizeof(INT16));
	m_csGuildListItemPacket.SetFieldType(
						AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,		// Guild ID
						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,		// Master ID
						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,		// SubMaster ID
						AUTYPE_INT32,			1,		// Level;
						AUTYPE_UINT32,			1,		// WinPoint;
						AUTYPE_INT32,			1,		// MemberCount;
						AUTYPE_INT32,			1,		// MaxMemberCount;
						AUTYPE_INT32,			1,		// lGuildBattle;
						AUTYPE_INT32,			1,		// lGuildMarkTID;
						AUTYPE_INT32,			1,		// lGuildMarkColor;
						AUTYPE_INT32,			1,		// IsWinner
						AUTYPE_END,			0
						);
	m_csGuildMarkTemplateList.SetCount(AGPMGUILD_MAX_GUILDMARK_TEMPLATE_COUNT);

	m_lGuildMarkBottomCount = 0;
	m_lGuildMarkPatternCount = 0;
	m_lGuildMarkSymbolCount = 0;
	m_lGuildMarkColorCount = 0;
}

AgpmGuild::~AgpmGuild()
{
}

BOOL AgpmGuild::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmGrid		 = (AgpmGrid *)			GetModule("AgpmGrid");
	m_pagpmItem		 = (AgpmItem *)			GetModule("AgpmItem");

	if(!m_pagpmCharacter || !m_pagpmGrid || !m_pagpmItem)
		return FALSE;

	m_nIndexADCharacter = m_pagpmCharacter->AttachCharacterData(this, sizeof(AgpdGuildADChar), ConAgpdGuildADChar, DesAgpdGuildADChar);
	if(m_nIndexADCharacter < 0)
		return FALSE;

	if (!m_pagpmItem->SetCallbackChangeItemOwner(CBChangeItemOwner, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmGuild::OnInit()
{
	// Guild Admin Initialize
	if(!m_csGuildAdmin.InitializeObject(sizeof(AgpdGuild*), m_csGuildAdmin.GetCount()))
		return FALSE;

	// Remove Admin Initialize
	if(!m_csRemoveAdmin.InitializeObject(sizeof(AgpdGuild*), m_csRemoveAdmin.GetCount()))
		return FALSE;

	return TRUE;
}

BOOL AgpmGuild::OnIdle2(UINT32 ulClockCount)
{
	PROFILE("AgpmGuild::OnIdle2");

	if( m_ulLastRemoveClockCount == 0 )
	{
		m_ulLastRemoveClockCount = ulClockCount;
		return TRUE;
	}

	// 시간 체크
	if( (ulClockCount - m_ulLastRemoveClockCount) < AGPMGUILD_IDLE_TIME_INTERVAL )
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("OnIdle2"));

	AgpdGuild** ppcsGuild = NULL;
	INT32 lIndex = 0;
	for(ppcsGuild = (AgpdGuild**)m_csRemoveAdmin.GetObjectSequence(&lIndex);
		ppcsGuild;
		ppcsGuild = (AgpdGuild**)m_csRemoveAdmin.GetObjectSequence(&lIndex))
	{
		if( !*ppcsGuild )	continue;

		(*ppcsGuild)->m_Mutex.WLock();
		DestroyGuildFromRemoveAdmin(*ppcsGuild);
	}

	m_ulLastRemoveClockCount = ulClockCount;

	return TRUE;
}

BOOL AgpmGuild::OnDestroy()
{
	AgpdGuild** ppcsGuild = NULL;
	INT32 lIndex = 0;
	for(ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild;
		ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!*ppcsGuild)
			continue;

		(*ppcsGuild)->m_Mutex.WLock();
		if((*ppcsGuild)->m_bRemove == FALSE)
			DestroyGuild(*ppcsGuild);
	}

	m_csGuildAdmin.RemoveObjectAll();

	lIndex	= 0;
	for(ppcsGuild = (AgpdGuild**)m_csRemoveAdmin.GetObjectSequence(&lIndex); ppcsGuild;
		ppcsGuild = (AgpdGuild**)m_csRemoveAdmin.GetObjectSequence(&lIndex))
	{
		if(!*ppcsGuild)
			continue;

		(*ppcsGuild)->m_Mutex.WLock();
		DestroyGuildFromRemoveAdmin(*ppcsGuild);
	}

	m_csRemoveAdmin.RemoveObjectAll();

	lIndex = 0;
	// Battle Point
	for(AgpdGuildBattlePoint** ppcsBattlePoint = (AgpdGuildBattlePoint**)m_csBattlePointAdmin.GetObjectSequence(&lIndex); ppcsBattlePoint;
			ppcsBattlePoint = (AgpdGuildBattlePoint**)m_csBattlePointAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsBattlePoint, AGPMGUILD_DATA_TYPE_BATTLE_POINT);
	}
	m_csBattlePointAdmin.RemoveObjectAll();		

	DeleteAllGuildMark();
	return TRUE;
}

BOOL AgpmGuild::SetMaxGuild(INT lCount)
{
	m_csGuildAdmin.SetCount(lCount);
	m_csRemoveAdmin.SetCount(lCount);
	return TRUE;
}

AgpdGuildADChar* AgpmGuild::GetADCharacter(AgpdCharacter* pData)
{
	if( !m_pagpmCharacter )		return NULL;

	return (AgpdGuildADChar*)m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID)pData);
}

BOOL AgpmGuild::ConAgpdGuildADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass )		return FALSE;

	AgpmGuild* pThis = (AgpmGuild*)pClass;
	AgpdGuildADChar* pAttachedData = pThis->GetADCharacter((AgpdCharacter*)pData);

	if(!pAttachedData)			return FALSE;

	memset(pAttachedData->m_szGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	memset(pAttachedData->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	pAttachedData->m_pMemberData		= NULL;
	pAttachedData->m_lGuildMarkTID		= 0;
	pAttachedData->m_lGuildMarkColor	= 0xFFFFFFFF;
	pAttachedData->m_lBRRanking			= WINNER_GUILD_NOTRANKED;

	return TRUE;
}

BOOL AgpmGuild::DesAgpdGuildADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

// Guild 에 Attached Data 를 세팅해준다.
INT16 AgpmGuild::AttachGuildData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMGUILD_DATA_TYPE_GUILD, nDataSize, pfConstructor, pfDestructor);
}

// Member 에 Attached Data 를 세팅해준다.
INT16 AgpmGuild::AttachMemberData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMGUILD_DATA_TYPE_MEMBER, nDataSize, pfConstructor, pfDestructor);
}

/*
	2005.10.05. By SungHoon
	GuildMarkTemplate 에 Attached Data 를 세팅해준다.
*/
INT16 AgpmGuild::AttachGuildMarkTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMGUILD_DATA_TYPE_GUILD_MARK_TEMPLATE, nDataSize, pfConstructor, pfDestructor);
}

// OnReceive
BOOL AgpmGuild::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmGuild::OnReceive");
	
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	INT32 lCID = -1;
	CHAR* szGuildID = NULL;
	CHAR* szMasterID = NULL;
	INT32 lGuildTID = -1;
	INT32 lGuildRank = -1;
	INT32 lCreationDate = -1;
	INT32 lMaxMemberCount = -1;
	INT32 lUnionID = -1;
	CHAR* szPassword = NULL;
	CHAR* szNotice = NULL;
	INT16 nNoticeLength = -1;
	INT8 cStatus = -1;
	INT32 lWin = -1;
	INT32 lDraw = -1;
	INT32 lLose = -1;
	INT32 lGuildPoint = -1;

	PVOID pvMemberPacket = NULL;
	CHAR* szMemberID = NULL;
	INT32 lMemberRank = -1;
	INT32 lJoinDate = -1;
	INT32 lMemberLevel = -1;
	INT32 lMemberTID = -1;
	INT8 cMemberStatus = -1;

	PVOID pvBattlePacket = NULL;
	CHAR* szEnemyGuildID = NULL;
	CHAR* szEnemyGuildMasterID = NULL;
	UINT32 ulAcceptTime = 0;
	UINT32 ulReadyTime = 0;
	UINT32 ulStartTime = 0;
	UINT32 ulBattleType = 0;
	UINT32 ulDuration = 0;
	UINT32 ulBattlePerson = 0;
	UINT32 ulCurrentTime = 0;
	INT32 lMyScore = -1;
	INT32 lEnemyScore = -1;
	INT32 lMyUpScore = -1;
	INT32 lEnemyUpScore = -1;
	INT8 cBattleResult = -1;
	INT32 lGuildMarkTID = -1;
	INT32 lGuildMarkColor = -1;
	BOOL IsWinner = FALSE;
	UINT32 ulRound = -1;

	PVOID pvBattleMemberPacket = NULL;
	PVOID pvBattleMemberListPacket = NULL;
	PVOID pvBattlePersonPacket = NULL;
	PVOID pvGuildListPacket = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&szGuildID,
						&szMasterID,
						&lGuildTID,
						&lGuildRank,
						&lCreationDate,
						&lMaxMemberCount,
						&lUnionID,
						&szPassword,
						&szNotice, &nNoticeLength,
						&cStatus,
						&lWin,
						&lDraw,
						&lLose,
						&lGuildPoint,
						&pvMemberPacket,
						&pvBattlePacket,
						&pvBattlePersonPacket,
						&pvBattleMemberPacket,
						&pvBattleMemberListPacket,
						&pvGuildListPacket,
						&lGuildMarkTID,
						&lGuildMarkColor,
						&IsWinner
						);

	if(pvMemberPacket)
	{
		m_csMemberPacket.GetField(FALSE, pvMemberPacket, 0,
						&szMemberID,
						&lMemberRank,
						&lJoinDate,
						&lMemberLevel,
						&lMemberTID,
						&cMemberStatus
						);
	}

	if(pvBattlePacket)
	{
		m_csBattlePacket.GetField(FALSE, pvBattlePacket, 0,
						&szEnemyGuildID,
						&szEnemyGuildMasterID,
						&ulAcceptTime,
						&ulReadyTime,
						&ulStartTime,
						&ulBattleType,
						&ulDuration,
						&ulBattlePerson,
						&ulCurrentTime,
						&lMyScore,
						&lEnemyScore,
						&lMyUpScore,
						&lEnemyUpScore,
						&cBattleResult,
						&ulRound
						);
	}
	
	INT16 lPerson = 0;
	CHAR* pPersonList = NULL;
	INT16 lPersonSize;
	if( pvBattlePersonPacket )
	{
		m_csBattlePersonPacket.GetField( FALSE, pvBattlePersonPacket, 0, &lPerson, &pPersonList, &lPersonSize );
	}

	CHAR* szMemberGuildID = NULL;
	CHAR* szCharID = NULL;
	UINT32 ulScore = 0;
	UINT32 ulKill = 0;
	UINT32 ulDeath = 0;
	if( pvBattleMemberPacket )
	{
		m_csBattleMemberPacket.GetField( FALSE, pvBattleMemberPacket, 0, &szMemberGuildID, &szCharID, &ulScore, &ulKill, &ulDeath );
	}


	UINT32	ulMyMemberCount;
	AgpdGuildMemberBattleInfo* pMyBattleMember = NULL;
	INT16	lMyInfoSize;
	UINT32	ulEnemyMemberCount;
	AgpdGuildMemberBattleInfo* pEnemyBattleMember = NULL;
	INT16	lEnemyInfoSize;
	if( pvBattleMemberListPacket )
	{
		m_csBattleMemberListPacket.GetField( FALSE, pvBattleMemberListPacket, 0,
								&ulMyMemberCount,
								&pMyBattleMember, &lMyInfoSize,
								&ulEnemyMemberCount,
								&pEnemyBattleMember, &lEnemyInfoSize
								);
	}

	switch(cOperation)
	{
		case AGPMGUILD_PACKET_CREATE:
			OnOperationCreate(lCID, szGuildID, szMasterID, lGuildTID, lGuildRank, lCreationDate, lMaxMemberCount, lUnionID, szPassword, szNotice, nNoticeLength, cStatus, lWin, lDraw, lLose, lGuildMarkTID, lGuildMarkColor, IsWinner);
			break;

		case AGPMGUILD_PACKET_JOIN_REQUEST:
			OnOperationJoinRequest(lCID, szGuildID, szMasterID, szMemberID);
			break;

		case AGPMGUILD_PACKET_JOIN_REJECT:
			OnOperationJoinReject(lCID, szGuildID, szMemberID);
			break;

		case AGPMGUILD_PACKET_JOIN:
			OnOperationJoin(lCID, szGuildID, szMemberID, lMemberRank, lJoinDate, lMemberLevel, lMemberTID, cMemberStatus);
			break;

		case AGPMGUILD_PACKET_LEAVE:
			OnOperationLeave(lCID, szGuildID, szMemberID);
			break;

		case AGPMGUILD_PACKET_FORCED_LEAVE:
			OnOperationForcedLeave(lCID, szGuildID, szMasterID, szMemberID);
			break;

		case AGPMGUILD_PACKET_DESTROY:
			OnOperationDestroy(lCID, szGuildID, szMasterID, szPassword);
			break;

		case AGPMGUILD_PACKET_UPDATE_MAX_MEMBER_COUNT:
			OnOperationUpdateMaxMemberCount(lCID, szGuildID, lMaxMemberCount);
			break;

		case AGPMGUILD_PACKET_CHAR_DATA:
			OnOperationCharacterGuildData(lCID, szGuildID, szMemberID, lMemberRank, lJoinDate, lMemberLevel, lMemberTID, cMemberStatus, lGuildMarkTID, lGuildMarkColor, IsWinner);
			break;

		case AGPMGUILD_PACKET_UPDATE_NOTICE:
			OnOperationUpdateNotice(lCID, szGuildID, szMasterID, szNotice, nNoticeLength);
			break;

		case AGPMGUILD_PACKET_SYSTEM_MESSAGE:
			OnOperationSystemMessage(lCID, szGuildID, szMasterID, lGuildTID, lGuildRank);
			break;

		case AGPMGUILD_PACKET_UPDATE_STATUS:
			OnOperationUpdateStatus(lCID, szGuildID, szMasterID, cStatus);
			break;

		case AGPMGUILD_PACKET_UPDATE_RECORD:
			OnOperationBattleRecord(lCID, szGuildID, lWin, lDraw, lLose, lGuildPoint);
			break;

		case AGPMGUILD_PACKET_BATTLE_INFO:
			OnOperationBattleInfo(lCID, szGuildID, cStatus, szEnemyGuildID, szEnemyGuildMasterID,
									lWin, lDraw, lLose,
									ulAcceptTime, ulReadyTime, ulStartTime, 
									ulBattleType, ulDuration, ulBattlePerson, ulCurrentTime,
									lMyScore, lEnemyScore, lMyUpScore, lEnemyUpScore, cBattleResult);
			break;

		case AGPMGUILD_PACKET_BATTLE_PERSON:
			OnOperationBattlePerson(lCID, szGuildID, lPerson, pPersonList );
			break;

		case AGPMGUILD_PACKET_BATTLE_REQUEST:
			OnOperationBattleRequest(lCID, szGuildID, szMasterID, szEnemyGuildID, ulBattleType, ulDuration, ulBattlePerson );
			break;

		case AGPMGUILD_PACKET_BATTLE_ACCEPT:
			OnOperationBattleAccept(lCID, szGuildID, szMasterID, szEnemyGuildID, ulAcceptTime, ulReadyTime, ulStartTime, ulBattleType, ulDuration, ulBattlePerson);
			break;

		case AGPMGUILD_PACKET_BATTLE_REJECT:
			OnOperationBattleReject(lCID, szGuildID, szMasterID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_BATTLE_CANCEL_REQUEST:
			OnOperationBattleCancelRequest(lCID, szGuildID, szMasterID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_BATTLE_CANCEL_ACCEPT:
			OnOperationBattleCancelAccept(lCID, szGuildID, szMasterID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_BATTLE_CANCEL_REJECT:
			OnOperationBattleCancelReject(lCID, szGuildID, szMasterID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_BATTLE_START:
			OnOperationBattleStart(lCID, szGuildID, szEnemyGuildID, szEnemyGuildMasterID, ulStartTime, ulDuration, ulCurrentTime);
			break;

		case AGPMGUILD_PACKET_BATTLE_UPDATE_TIME:
			OnOperationBattleUpdateTime(lCID, szGuildID, szEnemyGuildID, ulAcceptTime, ulReadyTime, ulStartTime, ulDuration, ulCurrentTime);
			break;
		
		case AGPMGUILD_PACKET_BATTLE_UPDATE_SCORE:
			OnOperationBattleUpdateScore(lCID, szGuildID, szEnemyGuildID, lMyScore, lEnemyScore, lMyUpScore, lEnemyUpScore);
			break;

		case AGPMGUILD_PACKET_BATTLE_WITHDRAW:
			OnOperationBattleWithdraw(lCID, szGuildID, szMasterID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_BATTLE_RESULT:
			OnOperationBattleResult(lCID, szGuildID, szEnemyGuildID, lMyScore, lEnemyScore, cBattleResult);
			break;

		case AGPMGUILD_PACKET_BATTLE_MEMBER:
			OnOperationBattleMember( lCID, szMemberGuildID, szCharID, ulScore, ulKill, ulDeath );
			break;

		case AGPMGUILD_PACKET_BATTLE_MEMBER_LIST:
			OnOperationBattleMemberList( lCID, (INT32)ulMyMemberCount, pMyBattleMember, (INT32)ulEnemyMemberCount, pEnemyBattleMember  );
			break;
			
		case AGPMGUILD_PACKET_BATTLE_ROUND:
			OnOperationBattleRound( lCID, szGuildID, ulRound);
			break;

		case AGPMGUILD_PACKET_GUILDLIST:
			OnOperationGuildList(lCID,pvGuildListPacket,szGuildID);
			break;

//	2005.08.01. By SungHoon
		case AGPMGUILD_PACKET_LEAVE_ALLOW :
			OnOperationLeaveAllow(lCID, szGuildID, szMemberID, TRUE );
			break; 

		case AGPMGUILD_PACKET_RENAME_GUILDID :
			OnOperationRenameGuildID(lCID, szGuildID, szMasterID);
			break;

		case AGPMGUILD_PACKET_RENAME_CHARACTERID:
			OnOperationRenameCharacterID(szGuildID, szMasterID, szMemberID);
			break;
		case AGPMGUILD_PACKET_BUY_GUILDMARK :
			OnOperationBuyGuildMark(lCID, szGuildID, lGuildMarkTID, lGuildMarkColor);
			break;
		case AGPMGUILD_PACKET_BUY_GUILDMARK_FORCE :
			OnOperationBuyGuildMarkForce(lCID, szGuildID, lGuildMarkTID, lGuildMarkColor);
			break;

// 2006.09.12. steeple
		case AGPMGUILD_PACKET_JOINT_REQUEST:
			OnOperationJointRequest(szGuildID, szEnemyGuildID, szMasterID);
			break;

		case AGPMGUILD_PACKET_JOINT_REJECT:
			OnOperationJointReject(szGuildID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_JOINT:
			OnOperationJoint(szGuildID, szEnemyGuildID, lCreationDate, cStatus);
			break;

		case AGPMGUILD_PACKET_JOINT_LEAVE:
			OnOperationJointLeave(lCID, szGuildID, szEnemyGuildID, szMasterID);
			break;

		case AGPMGUILD_PACKET_HOSTILE_REQUEST:
			OnOperationHostileRequest(szGuildID, szEnemyGuildID, szMasterID);
			break;

		case AGPMGUILD_PACKET_HOSTILE_REJECT:
			OnOperationHostileReject(szGuildID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_HOSTILE:
			OnOperationHostile(szGuildID, szEnemyGuildID, lCreationDate);
			break;

		case AGPMGUILD_PACKET_HOSTILE_LEAVE_REQUEST:
			OnOperationHostileLeaveRequest(szGuildID, szEnemyGuildID, szMasterID);
			break;

		case AGPMGUILD_PACKET_HOSTILE_LEAVE_REJECT:
			OnOperationHostileLeaveReject(szGuildID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_HOSTILE_LEAVE:
			OnOperationHostileLeave(lCID, szGuildID, szEnemyGuildID);
			break;

		case AGPMGUILD_PACKET_JOINT_DETAIL:
			OnOperationJointDetail(pvGuildListPacket);
			break;

		case AGPMGUILD_PACKET_HOSTILE_DETAIL:
			OnOperationHostileDetail(pvGuildListPacket);
			break;

		case AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP:
			OnOperationWorldChampionship(pvPacket);
			break;

		case AGPMGUILD_PACKET_CLASS_SOCIETY_SYSTEM:
			OnOperationClassSocietySystem(pvPacket);
			break;
	}
	
	return TRUE;
}

// Character Lock 은 필요없다.
// Server 라면은 lCID 의 Character 와 szMaster 와의 Character 가 같다.
// Client 라면은 다를 수도 있으니 잘 구분해서 해야함.
BOOL AgpmGuild::OnOperationCreate(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, INT32 lGuildTID, INT32 lGuildRank,
								  INT32 lCreationDate, INT32 lMaxMemberCount, INT32 lUnionID, CHAR* szPassword,
								  CHAR* szNotice, INT16 nNoticeLength,
								  INT8 cStatus, INT32 lWin, INT32 lDraw, INT32 lLose,
								  INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL IsWinner)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	// 길드를 만들 수 있는 지 체크한다.
	if(!CheckCreateGuildEnable(szGuildID, szMasterID))
		return FALSE;

	// 길드를 만든다. 만들어진 길드는 Lock 되어서 넘어온다.
	AgpdGuild* pcsGuild = CreateGuild(szGuildID, lGuildTID, lGuildRank, lMaxMemberCount, lUnionID);
	if(!pcsGuild)
	{
		// 알 수 없는 에러
		WriteGuildLog("AgpmGuild::OnOperationCreate | !pcsGuild 알수없는 오류~~ 이거 뜨면 낭패~\n");
		return TRUE;
	}

	// Master 를 세팅해준다.
	ZeroMemory(pcsGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	strncpy(pcsGuild->m_szMasterID, szMasterID, AGPACHARACTER_MAX_ID_STRING);

	if(lCreationDate != -1)		// 길드 생성 날짜가 넘어왔다면 세팅 (보통 클라이언트)
		pcsGuild->m_lCreationDate = lCreationDate;
	else								// 길드 생성 날짜가 넘어오지 않았다면 지금 시간으로 세팅 (보통 서버)
		pcsGuild->m_lCreationDate = GetCurrentTimeStamp();

	// Password 를 세팅한다. (이건 서버일 때만 된다.)
	if(szPassword)
	{
		ZeroMemory(pcsGuild->m_szPassword, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH + 1));
		strncpy(pcsGuild->m_szPassword, szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH);
	}

	// Notice Setting - 2004.09.23. steeple
	if(szNotice && nNoticeLength > 0 && nNoticeLength <= AGPMGUILD_MAX_NOTICE_LENGTH)
	{
		pcsGuild->m_szNotice = new CHAR[nNoticeLength+1];
		ZeroMemory(pcsGuild->m_szNotice, sizeof(CHAR) * (nNoticeLength + 1));
		memcpy(pcsGuild->m_szNotice, szNotice, nNoticeLength);
	}
	else
		pcsGuild->m_szNotice = NULL;

	// Status
	pcsGuild->m_cStatus = cStatus != -1 ? cStatus : (INT8)AGPMGUILD_STATUS_NONE;

	// Record
	pcsGuild->m_lWin = lWin != -1 ? lWin : 0;
	pcsGuild->m_lDraw = lDraw != -1 ? lDraw : 0;
	pcsGuild->m_lLose = lLose != -1 ? lLose : 0;

	// GuildMark 2005.10.19. By SungHoon
	pcsGuild->m_lGuildMarkTID = lGuildMarkTID != -1 ? lGuildMarkTID : 0;
	pcsGuild->m_lGuildMarkColor = lGuildMarkTID != -1 ? lGuildMarkColor : 0;
	pcsGuild->m_lBRRanking = IsWinner;

	// Lock 을 풀고
	pcsGuild->m_Mutex.Release();

	// 콜백 불러준다.
	EnumCallback(AGPMGUILD_CB_CREATE, szGuildID, szMasterID);

	return TRUE;
}

// 2005.01.24. steeple
// DB 에서 길드이름을 검사하려고 만들었다.
// 고로 완전 써버용이 되겠다.
BOOL AgpmGuild::OnOperationCreateCheck(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, INT32 lGuildTID, INT32 lGuildRank,
								INT32 lCreationDate, INT32 lMaxMemberCount, INT32 lUnionID, CHAR* szPassword,
								CHAR* szNotice, INT16 nNoticeLength)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	// 임시 길드 데이터를 만든다.
	AgpdGuild* pcsGuild = (AgpdGuild*)CreateModuleData(AGPMGUILD_DATA_TYPE_GUILD);
	if(!pcsGuild)
	{
		WriteGuildLog("AgpmGuild::OnOperationCreateCheck | 임시 길드 데이터 생성 실패 - 이것도 뜨면 낭패\n");
		return NULL;
	}

	// 뮤텍스 생성할 필요 없다.

	// Data Setting
	memset(pcsGuild->m_szID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
	strncpy(pcsGuild->m_szID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	memset(pcsGuild->m_szPassword, 0, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH+1));
	pcsGuild->m_szNotice = NULL;

	// Master 를 세팅해준다.
	ZeroMemory(pcsGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	strncpy(pcsGuild->m_szMasterID, szMasterID, AGPACHARACTER_MAX_ID_STRING);

	// Password 를 세팅한다. (이건 서버일 때만 된다.)
	if(szPassword)
	{
		ZeroMemory(pcsGuild->m_szPassword, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH + 1));
		strncpy(pcsGuild->m_szPassword, szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH);
	}
	//	2005.10.18. By SungHoon 나중에 길드마크에 대한 설정 해줘야 한다.
	pcsGuild->m_lGuildMarkTID = 0;
	pcsGuild->m_lGuildMarkColor = 0xFFFFFFFF;

	// 길드전에서 우승한 길드인지 설정은 길드 정보를 DB에서 모두다 가져온 이후에 설정한다.
	pcsGuild->m_lBRRanking = WINNER_GUILD_NOTRANKED;

	// 콜백 불러준다.
	EnumCallback(AGPMGUILD_CB_CREATE_CHECK, pcsGuild, szMasterID);

	// 임시로 만든 길드 데이터를 지운다.
	DestroyModuleData(pcsGuild);

	return TRUE;
}

// Master 가 Char 를 초대한다.
// 서버라면 szCharID 에게 초대 패킷을 보내면 되고
// 클라이언트라면 UI 를 띄우면 된다.
BOOL AgpmGuild::OnOperationJoinRequest(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID)
{
	if(!szGuildID || !szMasterID || !szCharID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if(!pcsCharacter)
		return FALSE;

	PVOID pBuffer[2];
	pBuffer[0] = szGuildID;
	pBuffer[1] = szMasterID;
	EnumCallback(AGPMGUILD_CB_JOIN_REQUEST, pBuffer, pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// Char 가 가입을 거절한다.
BOOL AgpmGuild::OnOperationJoinReject(INT32 lCID, CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	// 아무것도 할 것 없고 바로 EnumCallback
	EnumCallback(AGPMGUILD_CB_JOIN_REJECT, szGuildID, szCharID);
	return TRUE;
}

// Char 가 해당 Guild 의 Member 가 된다.
// 현재는 그냥 패킷만 오면 자동으로 멤버가 된다. -0-;;
BOOL AgpmGuild::OnOperationJoin(INT32 lCID, CHAR* szGuildID, CHAR* szMemberID, INT32 lMemberRank, INT32 lJoinDate,
								INT32 lLevel, INT32 lTID, INT8 cStatus)
{
	if(!szGuildID || !szMemberID)
		return FALSE;

	if(!CheckJoinEnable(szGuildID, szMemberID, lMemberRank == AGPMGUILD_MEMBER_RANK_JOIN_REQUEST ? TRUE : FALSE ))
		return FALSE;

	if(!JoinMember(szGuildID, szMemberID, lMemberRank, lJoinDate, lLevel, lTID, cStatus))
	{
		//INT8 cErrorCode = AGPMGUILD_ERROR_CODE_JOIN_FAIL;
		//EnumCallback(AGPMGUILD_CB_JOIN_FAIL, szMemberID, &cErrorCode);
		return FALSE;
	}

	// 여기까지 왔으면 가입이 된 것임.
	EnumCallback(AGPMGUILD_CB_JOIN, szGuildID, szMemberID);
	return TRUE;
}


// Member 가 탈퇴함
BOOL AgpmGuild::OnOperationLeave(INT32 lCID, CHAR* szGuildID, CHAR* szMemberID)
{
	if(!szGuildID || !szMemberID)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_LEAVE_REQUEST_SELF, szGuildID, szMemberID);
	return TRUE;

/*
	if(!szGuildID || !szMemberID)
		return FALSE;

	if(LeaveMember(szGuildID, szMemberID))
	{
		// 성공
		EnumCallback(AGPMGUILD_CB_LEAVE, szGuildID, szMemberID);
	}
	else
	{
		// 실패 콜백을 불러준다.
		EnumCallback(AGPMGUILD_CB_LEAVE_FAIL, szGuildID, &lCID);
	}

	return TRUE;
*/
}

// Master 가 Member 를 짜름
BOOL AgpmGuild::OnOperationForcedLeave(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szMemberID)
{
	if(!szGuildID || !szMasterID || !szMemberID)
		return FALSE;

	if(ForcedLeaveMember(szGuildID, szMasterID, szMemberID))
	{
		// 성공
		PVOID pBuffer[2];
		pBuffer[0] = szGuildID;
		pBuffer[1] = szMasterID;
		EnumCallback(AGPMGUILD_CB_FORCED_LEAVE, pBuffer, szMemberID);
	}
	else
	{
		// 실패
		EnumCallback(AGPMGUILD_CB_FORCED_LEAVE_FAIL, szMasterID, szMemberID);
	}

	return TRUE;
}

// 길드 해체. Master 가 하는 것임.
BOOL AgpmGuild::OnOperationDestroy(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword, BOOL bAdmin)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	INT8 cErrorCode = -1;

	// 2006.01.24. steeple
	if(!bAdmin)
	{
		// Master 인지 확인
		if(!IsMaster(pcsGuild, szMasterID))
		{
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NOT_MASTER;
			EnumCallback(AGPMGUILD_CB_DESTROY_FAIL, &lCID, &cErrorCode);
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}
	}

	BOOL bCheckTimeResult = TRUE;
	if(!bAdmin)
	{
		EnumCallback(AGPMGUILD_CB_DESTROY_GUILD_CHECK_TIME, pcsGuild, &bCheckTimeResult );
	}

	if (!bCheckTimeResult )
	{
		pcsGuild->m_Mutex.Release();

		ApAutoLockCharacter Lock( m_pagpmCharacter, szMasterID);
		AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
		if (pcsCharacter)
			OnOperationSystemMessage(AGPMGUILD_SYSTEM_CODE_DESTROY_FAIL_TOO_EARLY, NULL, NULL, 0, 0, pcsCharacter);
		return FALSE;
	}

	//205.04.21. steeple
	// 배틀중일 때는 해체 불가능~
	if(IsBattleStatus(pcsGuild))
	{
		pcsGuild->m_Mutex.Release();

		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsCharacter)
		{
			OnOperationSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, NULL, NULL, 0, 0, pcsCharacter);
			pcsCharacter->m_Mutex.Release();
		}
		return FALSE;
	}

	BOOL	bCheckResult	= TRUE;
	EnumCallback(AGPMGUILD_CB_CHECK_DESTROY, pcsGuild, &bCheckResult);
	if (!bCheckResult)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	
	PVOID pvBuffer[3];
	pvBuffer[0] = pvBuffer[1] = pvBuffer[2] = NULL;

	// 비밀번호 데이터는 서버에만 있기 때문에 이렇게 해준다.
	BOOL bValidPassword = TRUE;
	pvBuffer[0] = pcsGuild;
	pvBuffer[1] = szPassword;
	if(!bAdmin)
	{
		EnumCallback(AGPMGUILD_CB_CHECK_PASSWORD, pvBuffer, &bValidPassword);
	}

	if(!bValidPassword)	// 패스워드 확인에 실패하면 GG
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// GuildID, MasterID 백업
	CHAR _szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR _szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	ZeroMemory(_szGuildID, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	ZeroMemory(_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));
	strncpy(_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	strncpy(_szMasterID, szMasterID, AGPACHARACTER_MAX_ID_STRING);
	
	// Member 이름들을 미리 백업해놓는다.
	INT32 i = 0;
	CHAR** pszMemberID = NULL;
	INT32 lMemberCount = pcsGuild->m_pMemberList->GetObjectCount();
	lMemberCount += pcsGuild->m_pGuildJoinList->GetObjectCount();
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
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
				ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(*ppcsMember)
			strncpy(pszMemberID[i], (*ppcsMember)->m_szID, AGPACHARACTER_MAX_ID_STRING);

		i++;
	}

	lIndex = 0;
	for(AgpdGuildRequestMember** ppcsRequestMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsRequestMember ;
				ppcsRequestMember  = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(*ppcsRequestMember)
			strncpy(pszMemberID[i], (*ppcsRequestMember )->m_szMemberID, AGPACHARACTER_MAX_ID_STRING);

		i++;
	}

	EnumCallback(AGPMGUILD_CB_DESTROY_PREPROCESS, pcsGuild, NULL);

	// Destroy 불러주고~
	BOOL bResult = DestroyGuild(pcsGuild);

	// EnumCallback
	pvBuffer[0] = _szGuildID;
	pvBuffer[1] = _szMasterID;
	pvBuffer[2] = &lMemberCount;
	EnumCallback(AGPMGUILD_CB_DESTROY, pvBuffer, pszMemberID);

	// 백업해 놓은 Member 이름 제거
	if(pszMemberID && lMemberCount)
	{
		for(i = 0; i < lMemberCount; i++)
		{
			delete [] pszMemberID[i];
		}
		delete [] pszMemberID;
	}

	return bResult;
}

// Max Member Count 가 업데이트 되었다!!!!
// 보통 이 함수는 Client 에서 불리게 된다.
BOOL AgpmGuild::OnOperationUpdateMaxMemberCount(INT32 lCID, CHAR* szGuildID, INT32 lNewMaxMemberCount)
{
	if(!szGuildID || lNewMaxMemberCount < 1)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_UPDATE_MAX_MEMBER_COUNT, &lCID , &lNewMaxMemberCount);
/*	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(pcsGuild)
	{
		pcsGuild->m_lMaxMemberCount = lNewMaxMemberCount;
		EnumCallback(AGPMGUILD_CB_UPDATE_MAX_MEMBER_COUNT, pcsGuild, NULL);
	}

	pcsGuild->m_Mutex.Release();
*/
	return TRUE;
}

// 캐릭터 길드 정보 세팅
// szGuildID 는 NULL 일 수도 있음.
// 보통 이 함수는 클라이언트에서 불리게 된다.
BOOL AgpmGuild::OnOperationCharacterGuildData(INT32 lCID, CHAR* szGuildID, CHAR* szMemberID, INT32 lMemberRank, INT32 lJoinDate,
											  INT32 lLevel, INT32 lTID, INT8 cStatus, INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL IsWinner )
{
	if(!szMemberID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMemberID);

	if(pcsCharacter)
	{
		if (m_pagpmCharacter->IsPC(pcsCharacter))
		{
			// Guild Member Data 가 온 것일 수도 있으니 Up	date Member Status 를 불러준다.
			UpdateMemberStatus(szGuildID, szMemberID,lMemberRank, lJoinDate, lLevel, lTID, cStatus);
		}

		// 현재(2004.06.27) Attached Data 에 있는 Member Data 는 쓰지 않는다. 나중에 쓰이면 그때 넣자.
			SetCharAD(pcsCharacter, szGuildID, lGuildMarkTID, lGuildMarkColor, IsWinner, NULL);

		//ASSERT(m_pagpmCharacter->IsPC(pcsCharacter) && "PC 가 아닌 길드 이름을 받았다. 뭐여 이건");

		// 콜백 불러준다.
		EnumCallback(AGPMGUILD_CB_CHAR_DATA, pcsCharacter, szGuildID);

		pcsCharacter->m_Mutex.Release();

		return TRUE;
	}
	else
	{
		// kermi 2006.8.10
		// 공성 NPC Guild의 길드 이름을 설정하기 위해 추가
		AgpdCharacter* monsterCheck = m_pagpmCharacter->GetCharacter(lCID);
		if (monsterCheck && m_pagpmCharacter->IsMonster(monsterCheck))
		{
			SetCharAD(monsterCheck, szGuildID, lGuildMarkTID, lGuildMarkColor, IsWinner, NULL);

			EnumCallback(AGPMGUILD_CB_CHAR_DATA, monsterCheck, szGuildID);
		}
		else
		{
			// 2006.09.07. steeple
			// 길드가 있고 멤버가 있다면 Update 해준다.
			AgpdGuild* pcsGuild = GetGuild(szGuildID);
			if(pcsGuild)
			{
				AuAutoLock Lock(pcsGuild->m_Mutex);
				if (Lock.Result())
				{
					AgpdGuildMember* pcsMember = GetMember(pcsGuild, szMemberID);
					if(pcsMember)
						UpdateMemberStatus(szGuildID, szMemberID, lMemberRank, lJoinDate, lLevel, lTID, cStatus);
				}
			}
		}
	}

	return FALSE;
}

BOOL AgpmGuild::OnOperationUpdateNotice(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szNotice, INT16 nNoticeLength, BOOL bAdmin)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	if(!bAdmin)
	{
		// Master 가 아니면 return FALSE
		if(!IsMaster(pcsGuild, szMasterID))
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}
	}

	// 먼저 번에 있다면 Clear
	if(pcsGuild->m_szNotice)
	{
		delete [] pcsGuild->m_szNotice;
		pcsGuild->m_szNotice = NULL;
	}

	// 세팅해준다.
	if(szNotice && nNoticeLength > 0 && nNoticeLength <= AGPMGUILD_MAX_NOTICE_LENGTH)
	{
		pcsGuild->m_szNotice = new CHAR[nNoticeLength+1];
		ZeroMemory(pcsGuild->m_szNotice, sizeof(CHAR) * (nNoticeLength + 1));
		memcpy(pcsGuild->m_szNotice, szNotice, nNoticeLength);
	}

	EnumCallback(AGPMGUILD_CB_UPDATE_NOTICE, pcsGuild, NULL);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 시스템 메시지를 받음
BOOL AgpmGuild::OnOperationSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2, AgpdCharacter* pcsCharacter)
{
	AgpdGuildSystemMessage stSystemMessage;
	memset(&stSystemMessage, 0, sizeof(stSystemMessage));

	stSystemMessage.m_lCode = lCode;
	stSystemMessage.m_aszData[0] = szData1;
	stSystemMessage.m_aszData[1] = szData2;
	stSystemMessage.m_alData[0] = lData1;
	stSystemMessage.m_alData[1] = lData2;

	EnumCallback(AGPMGUILD_CB_SYSTEM_MESSAGE, &stSystemMessage, pcsCharacter);
	return TRUE;
}







// 2005.04.14. steeple
// Status 만 Update 되었을 때.
// 보통 이렇게 혼자만 오는 경우는 드물다.
BOOL AgpmGuild::OnOperationUpdateStatus(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, INT8 cStatus)
{
	if(!szGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	INT8 cOldStatus = pcsGuild->m_cStatus;
	
	if(cStatus != -1)
		pcsGuild->m_cStatus = cStatus;

	// Lock 한 채루 넘겨준다.
	EnumCallback(AGPMGUILD_CB_UPDATE_STATUS, pcsGuild, &cOldStatus);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.14. steeple
// Record 정보가 왔다. 얘도 보통 이렇게 혼자 불리는 경우는 드물다.
BOOL AgpmGuild::OnOperationBattleRecord(INT32 lCID, CHAR* szGuildID, INT32 lWin, INT32 lDraw, INT32 lLose, INT32 lGuildPoint)
{
	if( !szGuildID )		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if( !pcsGuild )			return FALSE;

	if(lWin != -1)				pcsGuild->m_lWin = lWin;
	if(lDraw != -1)				pcsGuild->m_lDraw = lDraw;
	if(lLose != -1)				pcsGuild->m_lLose = lLose;
	if(lGuildPoint != -1)		pcsGuild->m_lGuildPoint = lGuildPoint;

	EnumCallback(AGPMGUILD_CB_UPDATE_RECORD, pcsGuild, NULL);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.14. steeple
// Battle Info 를 받는다. 보통 PC가 처음 들어왔을 때 받게 된다.
BOOL AgpmGuild::OnOperationBattleInfo(INT32 lCID, CHAR* szGuildID, INT8 cStatus, CHAR* szEnemyGuildID, CHAR* szEnemyGuildMasterID,
								INT32 lWin, INT32 lDraw, INT32 lLose,
								UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime,
								UINT32 ulType, UINT32 ulDuration, UINT32 ulPerson,
								UINT32 ulCurrentTime,
								INT32 lMyScore, INT32 lEnemyScore, INT32 lMyUpScore, INT32 lEnemyUpScore, INT8 cResult)
{
	if(!szGuildID)		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)		return FALSE;

	if(cStatus != -1)
		pcsGuild->m_cStatus = cStatus;

	if(lWin != -1)
		pcsGuild->m_lWin = lWin;
	if(lDraw != -1)
		pcsGuild->m_lDraw = lDraw;
	if(lLose != -1)
		pcsGuild->m_lLose = lLose;

	if(szEnemyGuildID && strlen(szEnemyGuildID) <= AGPMGUILD_MAX_GUILD_ID_LENGTH)
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID);
		
	if(szEnemyGuildMasterID && strlen(szEnemyGuildMasterID) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, szEnemyGuildMasterID);

	if(ulAcceptTime != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime = ulAcceptTime;
	if(ulReadyTime != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime = ulReadyTime;
	if(ulStartTime != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulStartTime = ulStartTime;
	if(ulDuration != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulDuration = ulDuration;

	pcsGuild->m_csCurrentBattleInfo.m_eType				= (eGuildBattleType)ulType;
	pcsGuild->m_csCurrentBattleInfo.m_ulPerson			= ulPerson;

	if(ulCurrentTime != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime = ulCurrentTime;

	if(lMyScore != -1)
		pcsGuild->m_csCurrentBattleInfo.m_lMyScore = lMyScore;
	if(lEnemyScore != -1)
		pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore = lEnemyScore;
	if(lMyUpScore != -1)
		pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore = lMyUpScore;
	if(lEnemyUpScore != -1)
		pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore = lEnemyUpScore;

	if(cResult != -1)
		pcsGuild->m_csCurrentBattleInfo.m_cResult = cResult;

	EnumCallback(AGPMGUILD_CB_BATTLE_INFO, pcsGuild, NULL);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmGuild::OnOperationBattlePerson(INT32 lCID, CHAR* szGuildID, INT16 lPersonCount, CHAR* pPersonList )
{
	if( !szGuildID || !lPersonCount || !pPersonList )	return FALSE;

	AuAutoLock pLock;
	AgpdGuild* pGuild = GetGuildLock(szGuildID, pLock);
	if( !pGuild )		return FALSE;
	
	if(IsBattleIngStatus(pGuild) == TRUE)
		return FALSE;

	AgpdGuildMember* pMember = NULL;
	CHAR szName[AGPACHARACTER_MAX_ID_STRING + 1];
	INT16 nOffset = 0;
	INT16 lStrLen;
	for( INT16 i=0; i<lPersonCount; ++i )
	{
		memset( szName, 0, AGPACHARACTER_MAX_ID_STRING + 1 );

		lStrLen = *(INT16*)(pPersonList + nOffset);
		nOffset += sizeof(INT16);
		strncpy( szName, pPersonList + nOffset, lStrLen );
		nOffset += lStrLen;

		pMember = pGuild->GetMember( szName );
		if( pMember )
		{
			pMember->m_bBattle		= TRUE;
			pMember->m_ulSequence	= i;
		}
	}

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID)&lPersonCount;
	pvBuffer[1] = (PVOID)pPersonList;
	EnumCallback( AGPMGUILD_CB_BATTLE_PERSON, pGuild, pvBuffer );

	return TRUE;
}

// 2005.04.14. steeple
BOOL AgpmGuild::OnOperationBattleRequest(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32 ulType, UINT32 ulDuration, UINT32 ulPerson)
{
	//return FALSE;

	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	PVOID pvBuffer[5];
	pvBuffer[0] = (PVOID)szMasterID;
	pvBuffer[1] = (PVOID)szEnemyGuildID;
	pvBuffer[2] = (PVOID)&ulType;
	pvBuffer[3] = (PVOID)&ulDuration;
	pvBuffer[4] = (PVOID)&ulPerson;

	EnumCallback(AGPMGUILD_CB_BATTLE_REQUEST, szGuildID, pvBuffer);

	return TRUE;
}

// 2005.04.14. steeple
// 클라이언트는 시간을 다 받게 되고 서버는 ulDuration 만 받고 나머지는 서버에서 처리하게 된다.
BOOL AgpmGuild::OnOperationBattleAccept(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID,
								UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime, UINT32 ulType, UINT32 ulDuration, UINT32 ulPerson)
{
	//return FALSE;

	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	AgpdGuildBattle stGuildBattle;
	stGuildBattle.Init();

	strncpy(stGuildBattle.m_szEnemyGuildID, szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	stGuildBattle.m_ulAcceptTime		= ulAcceptTime;
	stGuildBattle.m_ulReadyTime			= ulReadyTime;
	stGuildBattle.m_ulStartTime			= ulStartTime;
	stGuildBattle.m_ulDuration			= ulDuration;

	stGuildBattle.m_eType				= (eGuildBattleType)ulType;
	stGuildBattle.m_ulPerson			= ulPerson;

	stGuildBattle.m_ulCurrentTime		= ulAcceptTime;	// Accept 시점에 받았기 때문에 ulAcceptTime == ulCurrentTime

	EnumCallback(AGPMGUILD_CB_BATTLE_ACCEPT, szGuildID, &stGuildBattle);

	return TRUE;
}

// 2005.04.14. steeple
BOOL AgpmGuild::OnOperationBattleReject(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID)szMasterID;
	pvBuffer[1] = (PVOID)szEnemyGuildID;

	EnumCallback(AGPMGUILD_CB_BATTLE_REJECT, szGuildID, pvBuffer);

	return TRUE;
}

// 2005.04.14. steeple
BOOL AgpmGuild::OnOperationBattleCancelRequest(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID)szMasterID;
	pvBuffer[1] = (PVOID)szEnemyGuildID;

	EnumCallback(AGPMGUILD_CB_BATTLE_CANCEL_REQUEST, szGuildID, pvBuffer);

	return TRUE;
}

// 2005.04.14. steeple
BOOL AgpmGuild::OnOperationBattleCancelAccept(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID)szMasterID;
	pvBuffer[1] = (PVOID)szEnemyGuildID;

	EnumCallback(AGPMGUILD_CB_BATTLE_CANCEL_ACCEPT, szGuildID, pvBuffer);

	return TRUE;
}

BOOL AgpmGuild::OnOperationBattleCancelReject(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID)szMasterID;
	pvBuffer[1] = (PVOID)szEnemyGuildID;

	EnumCallback(AGPMGUILD_CB_BATTLE_CANCEL_REJECT, szGuildID, pvBuffer);

	return TRUE;
}

// 2005.04.14. steeple
// 이 패킷은 보통 서버에서 클라이언트로 오게 된다.
BOOL AgpmGuild::OnOperationBattleStart(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, CHAR* szEnemyGuildMasterID, UINT32 ulStartTime, UINT32 ulDuration,
								UINT32 ulCurrentTime)
{
	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	if(strlen(szEnemyGuildID) <= AGPMGUILD_MAX_GUILD_ID_LENGTH)
	{
		ZeroMemory(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
		strncpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	}

	if(strlen(szEnemyGuildMasterID) <= AGPACHARACTER_MAX_ID_STRING)
	{
		ZeroMemory(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, AGPACHARACTER_MAX_ID_STRING+1);
		strncpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, szEnemyGuildMasterID, AGPACHARACTER_MAX_ID_STRING);
	}

	if(ulStartTime != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulStartTime = ulStartTime;
	if(ulDuration != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulDuration = ulDuration;
	if(ulCurrentTime != 0)
		pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime = ulCurrentTime;

	EnumCallback(AGPMGUILD_CB_BATTLE_START, pcsGuild, NULL);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.14. steeple
// 이 패킷은 보통 서버에서 클라이언트로 오게 된다.
BOOL AgpmGuild::OnOperationBattleUpdateTime(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, UINT32 ulAcceptTime, UINT32 ulReadyTime,
								UINT32 ulStartTime, UINT32 ulDuration, UINT32 ulCurrentTime)
{
	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// 적길드가 맞을 때만 하면 될듯
	if(strcmp(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID) == 0)
	{
		if(ulAcceptTime != 0)
			pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime = ulAcceptTime;
		if(ulReadyTime != 0)
			pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime = ulReadyTime;
		if(ulStartTime != 0)
			pcsGuild->m_csCurrentBattleInfo.m_ulStartTime = ulStartTime;
		if(ulDuration != 0)
			pcsGuild->m_csCurrentBattleInfo.m_ulDuration = ulDuration;
		if(ulCurrentTime != 0)
			pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime = ulCurrentTime;

		EnumCallback(AGPMGUILD_CB_BATTLE_UPDATE_TIME, pcsGuild, NULL);
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.14. steeple
// 이 패킷은 보통 서버에서 클라이언트로 오게 된다.
BOOL AgpmGuild::OnOperationBattleUpdateScore(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lMyScore, INT32 lEnemyScore, INT32 lMyUpScore, INT32 lEnemyUpScore)
{
	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	// 설마 둘이 같진 않겠지만... 패킷이 잘못왔을 때를 대비한다.
	if(strcmp(szGuildID, szEnemyGuildID) == 0)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// 적길드가 맞을 때만 하면 될듯
	if(strcmp(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID) == 0)
	{
		if(lMyScore != -1)		pcsGuild->m_csCurrentBattleInfo.m_lMyScore		= lMyScore;
		if(lEnemyScore != -1)	pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore	= lEnemyScore;
		if(lMyUpScore != -1)	pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore	= lMyUpScore;
		if(lEnemyUpScore != -1)	pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore	= lEnemyUpScore;

		EnumCallback(AGPMGUILD_CB_BATTLE_UPDATE_SCORE, pcsGuild, NULL);
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.17. steeple
// 이 놈은 보통 클라이언트에서 서버로 보내는데 상당히 크리티컬 하므로 밸리데이션 체크를 여러겹 한다.
BOOL AgpmGuild::OnOperationBattleWithdraw(INT32 lCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(lCID < 1 || !szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	// 길드를 구하고
	AgpdGuild* pcsGuild = GetGuildLock(GetJoinedGuildID(pcsCharacter));
	if(pcsGuild)
	{
		if(strcmp(pcsCharacter->m_szID, pcsGuild->m_szMasterID) == 0)
		{
			if(IsBattleReadyStatus(pcsGuild))
			{
				// 레디 상태에서는 GG 칠 수 없다.
				OnOperationSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_WITHDRAW_UNTIL_START, NULL, NULL, 0, 0, pcsCharacter);
			}
			else if(IsBattleIngStatus(pcsGuild) && IsEnemyGuild(pcsGuild, szEnemyGuildID))
			{
				// 이렇게 되었을 때 불러준다.
				EnumCallback(AGPMGUILD_CB_BATTLE_WITHDRAW, pcsGuild, pcsCharacter);
			}
		}

		pcsGuild->m_Mutex.Release();
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2005.04.14. steeple
// 이 패킷은 보통 서버에서 클라이언트로 오게 된다.
BOOL AgpmGuild::OnOperationBattleResult(INT32 lCID, CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lMyScore, INT32 lEnemyScore, INT8 cBattleResult)
{
	if( !szGuildID || !szEnemyGuildID )			return FALSE;
	if( !strcmp(szGuildID, szEnemyGuildID) )	return FALSE;
		
	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if( !pcsGuild )			return FALSE;

	//agpdguild* pcsenemyguild = getguildlock(szenemyguildid);
	//if( !pcsenemyguild )
	//{
	//	pcsguild->m_mutex.release();
	//	return false;
	//}

	// 적길드가 맞을 때만 하면 될듯
	if( !strcmp( pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID ) )
	{
		if( lMyScore != -1 )		pcsGuild->m_csCurrentBattleInfo.m_lMyScore		= lMyScore;
		if( lEnemyScore != -1 )		pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore	= lEnemyScore;
		if( cBattleResult != -1 )	pcsGuild->m_csCurrentBattleInfo.m_cResult		= cBattleResult;

		EnumCallback( AGPMGUILD_CB_BATTLE_RESULT, pcsGuild, NULL );
	}
	
	//pcsEnemyGuild->m_Mutex.Release();
	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmGuild::OnOperationBattleMember(INT32 lCID, CHAR* szGuildID, CHAR* szCharID, UINT32 ulScore, UINT32 ulKill, UINT32 ulDeath )
{
	PVOID pvArray[ 5 ];
	pvArray[0] = (PVOID)szGuildID;
	pvArray[1] = (PVOID)szCharID;
	pvArray[2] = (PVOID)&ulScore;
	pvArray[3] = (PVOID)&ulKill;
	pvArray[4] = (PVOID)&ulDeath;
	EnumCallback(AGPMGUILD_CB_BATTLE_MEMBER, &pvArray, NULL);
	return TRUE;
}

BOOL AgpmGuild::OnOperationBattleMemberList(INT32 lCID, INT32 lMyCount, AgpdGuildMemberBattleInfo* pMyMemberInfo, INT32 lEnemyCount, AgpdGuildMemberBattleInfo* pEnemyMemberInfo )
{
	PVOID pvArray[ 4 ];
	pvArray[0] = (PVOID)&lMyCount;
	pvArray[1] = (PVOID)pMyMemberInfo;
	pvArray[2] = (PVOID)&lEnemyCount;
	pvArray[3] = (PVOID)pEnemyMemberInfo;
	EnumCallback(AGPMGUILD_CB_BATTLE_MEMBER_LIST, &pvArray, &lCID);
	return TRUE;
}

BOOL AgpmGuild::OnOperationBattleRound(INT32 lCID, CHAR* szGuildID, UINT32 ulRound)
{
	PVOID pvArray[ 2 ];
	pvArray[0] = (PVOID)szGuildID;
	pvArray[1] = (PVOID)&ulRound;
	EnumCallback(AGPMGUILD_CB_BATTLE_ROUND, &pvArray, NULL);
	return TRUE;
}

BOOL AgpmGuild::OnOperationGuildList(INT32 lCID, PVOID pvGuildListPacket, CHAR *szGuildID)
{
	PVOID pvArray[ 2 ];
	pvArray[0] = ( PVOID )pvGuildListPacket;
	pvArray[1] = ( PVOID )szGuildID;
	EnumCallback(AGPMGUILD_CB_GUILDLIST , &pvArray, &lCID);
	return TRUE;
}

// 인자로 넘어온 GuildData 를 Admin 에 넣는다.
BOOL AgpmGuild::AddGuild(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;
	
	return PtrToInt(m_csGuildAdmin.AddGuild(pcsGuild));
}

// 인자로 넘어온 GuildData 를 Admin 에서 뺀다.
BOOL AgpmGuild::RemoveGuild(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	return m_csGuildAdmin.RemoveGuild(pcsGuild->m_szID);
}

// GuildData 를 Admin 에서 받는다.
AgpdGuild* AgpmGuild::GetGuild(CHAR* szGuildID)
{
	if(!szGuildID)
		return NULL;

	AgpdGuild* pcsGuild = m_csGuildAdmin.GetGuild(szGuildID);
	if(!pcsGuild || pcsGuild->m_bRemove)
		return NULL;

	return pcsGuild;
}

// GuildData 를 Admin 에서 받은 후, Lock 을 해서 넘긴다.
inline AgpdGuild* AgpmGuild::GetGuildLock(CHAR* szGuildID)
{
	AgpdGuild* pcsGuild = GetGuild(szGuildID);
	if(!pcsGuild)
		return NULL;

	if (!pcsGuild->m_Mutex.WLock())
		return NULL;

	if(pcsGuild->m_bRemove)	// 지워지는 길드라면 return NULL
	{
		pcsGuild->m_Mutex.Release();
		return NULL;
	}

	return pcsGuild;
}

AgpdGuild* AgpmGuild::GetGuildLock(CHAR* szGuildID, AuAutoLock& pLock)
{
	AgpdGuild* pcsGuild = GetGuild(szGuildID);
	if(!pcsGuild) return NULL;
		
	if(pcsGuild->m_bRemove) return NULL;

	pLock.SetMutualEx(pcsGuild->m_Mutex);
	pLock.Lock();	

	if (!pLock.Result()) return NULL;

	return pcsGuild;
}


INT32 AgpmGuild::GetGuildCount()
{
	return m_csGuildAdmin.GetObjectCount();
}

AgpdGuild* AgpmGuild::GetGuildSequence(INT32* plIndex)
{
	if(!plIndex)
		return NULL;

	AgpdGuild** ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(plIndex);
	if(ppcsGuild)
		return *ppcsGuild;

	return NULL;
}

// 길드를 만들고 만든 길드데이터를 리턴한다.
// GuildAdmin 에 넣어주는 작업까지 해버린다.
AgpdGuild* AgpmGuild::CreateGuild(CHAR* szGuildID, INT32 lGuildTID, INT32 lGuildRank, INT32 lMaxMemberCount, INT32 lUnionID)
{
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return NULL;

	// 이미 같은 이름의 길드가 있으면 나간다.
	if(GetGuild(szGuildID))
		return NULL;

	AgpdGuild* pcsGuild = (AgpdGuild*)CreateModuleData(AGPMGUILD_DATA_TYPE_GUILD);
	if(!pcsGuild)
		return NULL;

	// Mutex Init
	pcsGuild->m_Mutex.Init((PVOID)pcsGuild);

	// Data Setting
	memset(pcsGuild->m_szID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
	strncpy(pcsGuild->m_szID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	if(lGuildTID != -1)
		pcsGuild->m_lTID = lGuildTID;
	if(lGuildRank != -1)
		pcsGuild->m_lRank = lGuildRank;
	if(lMaxMemberCount != -1)
		pcsGuild->m_lMaxMemberCount = lMaxMemberCount;
	if(lUnionID != -1)
		pcsGuild->m_lUnionID = lUnionID;

	memset(pcsGuild->m_szPassword, 0, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH+1));
	pcsGuild->m_szNotice = NULL;

	pcsGuild->m_pMemberList = new ApAdmin();
	pcsGuild->m_pMemberList->SetCount(AGPMGUILD_MAX_MEMBER_COUNT);
	pcsGuild->m_pMemberList->InitializeObject(sizeof(AgpdGuildMember*), pcsGuild->m_pMemberList->GetCount());
	
	pcsGuild->m_pGuildJoinList = new ApAdmin();
	pcsGuild->m_pGuildJoinList->SetCount(AGPMGUILD_MAX_MEMBER_COUNT);
	pcsGuild->m_pGuildJoinList->InitializeObject(sizeof(AgpdGuildRequestMember*), pcsGuild->m_pGuildJoinList->GetCount());

	pcsGuild->m_ulLastRefreshClock = 0;

	pcsGuild->m_bRemove = FALSE;

	// Status, Record, Battle
	pcsGuild->m_cStatus = (INT8)AGPMGUILD_STATUS_NONE;
	pcsGuild->m_lWin = pcsGuild->m_lDraw = pcsGuild->m_lLose = 0;
	pcsGuild->m_csCurrentBattleInfo.Init();

	// GuildMark
	pcsGuild->m_lGuildMarkTID = 0;
	pcsGuild->m_lGuildMarkColor = 0xFFFFFFFF;

	// Related Guild
	memset(&pcsGuild->m_csRelation, 0, sizeof(pcsGuild->m_csRelation));
	pcsGuild->m_csRelation.m_pJointVector = new JointVector;
	pcsGuild->m_csRelation.m_pHostileVector = new HostileVector;

	// Admin 에 추가
	AddGuild(pcsGuild);

	// 리턴하기 전에는 Lock 을 해서 넘긴다.
	if (!pcsGuild->m_Mutex.WLock())
	{
		DestroyGuild(pcsGuild);
		return NULL;
	}

	return pcsGuild;
}

// 길드를 만들 수 있는 지 체크한다.
BOOL AgpmGuild::CheckCreateGuildEnable(CHAR* szGuildID, CHAR* szMasterID)
{
	BOOL bResult = TRUE;

	// 다른 체크할 거 있으면 불러준다.
	PVOID pBuffer[2];
	pBuffer[0] = szGuildID;
	pBuffer[1] = szMasterID;
	EnumCallback(AGPMGUILD_CB_CREATE_ENABLE_CHECK, pBuffer, &bResult);

	return bResult;
}

// 길드를 해체한다.
BOOL AgpmGuild::DestroyGuild(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	return DestroyGuild(pcsGuild);
}

// 길드를 해체한다.
// pcsGuild 는 Lock 되어서 넘어온다.
BOOL AgpmGuild::DestroyGuild(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	// Remove 를 체크해준다.
	pcsGuild->m_bRemove	= TRUE;

	// 해체 작업
	INT32 lIndex = 0;
	AgpdGuildMember** ppcsMember = NULL;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember)
			break;

		DestroyModuleData(*ppcsMember, AGPMGUILD_DATA_TYPE_MEMBER);
	}
	
	pcsGuild->m_pMemberList->RemoveObjectAll();
	delete pcsGuild->m_pMemberList;

	// 가입대기자는 왜 안지우고 있었을까 -_-;
	// 2006.03.08. steeple
	AgpdGuildRequestMember** ppcsRequestMember = NULL;
	for(ppcsRequestMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsRequestMember;
			ppcsRequestMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsRequestMember)
			break;

		// 얘는 서버에서 그냥 new 하고 있다. -0-;
		delete *ppcsRequestMember;
	}
	
	pcsGuild->m_pGuildJoinList->RemoveObjectAll();
	delete pcsGuild->m_pGuildJoinList;

	// Notice - 2004.09.23. steeple
	if(pcsGuild->m_szNotice)
	{
		delete [] pcsGuild->m_szNotice;
		pcsGuild->m_szNotice = NULL;
	}

	// Related Guild
	if(pcsGuild->m_csRelation.m_pJointVector)
	{
		delete pcsGuild->m_csRelation.m_pJointVector;
		pcsGuild->m_csRelation.m_pJointVector = NULL;
	}

	if(pcsGuild->m_csRelation.m_pHostileVector)
	{
		delete pcsGuild->m_csRelation.m_pHostileVector;
		pcsGuild->m_csRelation.m_pHostileVector = NULL;
	}

	// Admin 에서 뺀다.
	RemoveGuild(pcsGuild);

	// 일단 Unlock
	pcsGuild->m_Mutex.Release();

	// Remove Admin 에 넣어준다.
	m_csRemoveAdmin.AddGuild(pcsGuild);

	return TRUE;
}

// Remove Admin 에서 길드 메모리를 해제한다.
// OnIdle 에서 불리게 된다.
// pcsGuild 는 Lock 되어 있다.
BOOL AgpmGuild::DestroyGuildFromRemoveAdmin(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	// Remove Admin 에서 빼준다.
	m_csRemoveAdmin.RemoveGuild(pcsGuild->m_szID);

	// 뮤텍스 삭제
	pcsGuild->m_Mutex.Destroy();

	// Destroy
	DestroyModuleData(pcsGuild);

	return TRUE;
}

// 길드멤버를 얻는다.
// pcsGuild 는 Lock 되어서 넘어온다.
AgpdGuildMember* AgpmGuild::GetMember(AgpdGuild* pcsGuild, CHAR* szMemberID)
{
	if(!pcsGuild || !szMemberID)
		return NULL;

	AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObject(szMemberID);
	if(!ppcsMember)
		return NULL;

	return *ppcsMember;
}

// 마스터를 얻는다.
// pcsGuild 는 Lock 되어서 넘어온다.
AgpdGuildMember* AgpmGuild::GetMaster(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return NULL;

	// Master 가 비어있으면 NULL
	if(strlen(pcsGuild->m_szMasterID) == 0)
		return NULL;

	AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObject(pcsGuild->m_szMasterID);
	if(!ppcsMember)
		return NULL;

	return *ppcsMember;
}

// 길드멤버 수를 얻는다.
// pcsGuild 는 Lock 되어서 넘어온다.
inline INT32 AgpmGuild::GetMemberCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return 0;

	// 전체 멤버 루프를 돌면서 카운트를 센다.
	INT32 lCount = 0;

	AgpdGuildMember* pcsMember = NULL;
	INT32 lIndex = 0;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
													ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember)
			break;
		pcsMember = *ppcsMember;
		if(!pcsMember)
			break;

		if(pcsMember->m_lRank != AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)
			lCount++;
	}

	return lCount;
}

INT32 AgpmGuild::GetAllMemberCount(AgpdGuild* pcsGuild)
{
	return pcsGuild ? pcsGuild->m_pMemberList->GetObjectCount() : 0;
}
// 온라인인 멤버 수를 얻는다.
// pcsGuild 는 Lock 되어서 넘어온다.
INT32 AgpmGuild::GetOnlineMemberCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return 0;

	// 전체 멤버 루프를 돌면서 카운트를 센다.
	INT32 lCount = 0;

	AgpdGuildMember* pcsMember = NULL;
	INT32 lIndex = 0;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
													ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember)
			break;
		pcsMember = *ppcsMember;
		if(!pcsMember)
			break;

		if(pcsMember->m_lRank != AGPMGUILD_MEMBER_RANK_JOIN_REQUEST && pcsMember->m_cStatus == AGPMGUILD_MEMBER_STATUS_ONLINE)
			lCount++;
	}

	return lCount;
}

// 길드의 마스터를 세팅한다.
// pcsMaster 로 넘어온 Member 는 pcsGuild 의 Member 이어야만 한다.
// pcsGuild 는 Lock 되어서 넘어온다.
BOOL AgpmGuild::SetMaster(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMaster)
{
	if(!pcsGuild || !pcsMaster)
		return FALSE;

	BOOL bChange = FALSE;

	// 기존의 마스터를 얻는다.
	AgpdGuildMember* pcsOldMaster = GetMaster(pcsGuild);
	if(!pcsOldMaster)	// 마스터가 세팅 안되어 있다면
	{
		// 인자로 넘어온 놈을 마스터로 해준다.
		bChange = TRUE;

		pcsMaster->m_lRank = AGPMGUILD_MEMBER_RANK_MASTER;
		ZeroMemory(pcsGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
		strncpy(pcsGuild->m_szMasterID, pcsMaster->m_szID, AGPACHARACTER_MAX_ID_STRING);
	}
	else
	{
		// 같은 놈이 아닐 때만 하면 된다.
		if(strcmp(pcsGuild->m_szMasterID, pcsMaster->m_szID) != 0)
		{
			bChange = TRUE;

			pcsMaster->m_lRank = AGPMGUILD_MEMBER_RANK_MASTER;
			ZeroMemory(pcsGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
			strncpy(pcsGuild->m_szMasterID, pcsMaster->m_szID, AGPACHARACTER_MAX_ID_STRING);
		}
	}

	// 마스터가 변경되었음을 알린다.
	if(bChange)
	{
		// EnumCallback(....);
	}

	return TRUE;
}

BOOL AgpmGuild::IsMaster(CHAR* szGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	BOOL bResult = IsMaster(pcsGuild, szMasterID);

	pcsGuild->m_Mutex.Release();

	return bResult;
}

BOOL AgpmGuild::IsMaster(AgpdGuild* pcsGuild, CHAR* szMasterID)
{
	if(!pcsGuild || !szMasterID)
		return FALSE;

	return strcmp(pcsGuild->m_szMasterID, szMasterID) == 0 ? TRUE : FALSE;
}

BOOL AgpmGuild::IsSubMaster( CHAR* szGuildID , CHAR* szSubMasterID )
{
	if( !szGuildID || !szSubMasterID )	return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if( !pcsGuild )		return FALSE;

	BOOL bResult		=	IsSubMaster( pcsGuild , szSubMasterID );

	pcsGuild->m_Mutex.Release();

	return bResult;
}

BOOL AgpmGuild::IsSubMaster( AgpdGuild* pcsGuild , CHAR* szSubMasterID )
{
	if( !pcsGuild || !szSubMasterID  )	return FALSE;

	return (!strcmp(pcsGuild->m_szSubMasterID, szSubMasterID));
}

// 이 캐릭터가 길드랭크 시스템에서 제외되어야 하는 것인가
BOOL AgpmGuild::IsExceptionCharacterOfGuildRankSystem(AgpdGuild* pcsGuild, AgpdCharacter* pcsCharacter)
{
	if(!pcsGuild || !pcsCharacter)
		return FALSE;

	AgpdGuildMember* pcsGuildMember = GetMember(pcsGuild, pcsCharacter->m_szID);
	if(!pcsGuildMember)
		return FALSE;

	BOOL bRet = FALSE;

	switch(pcsGuildMember->m_lRank)
	{
		case AGPMGUILD_MEMBER_RANK_JOIN_REQUEST:
		case AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST:
			{
				bRet = TRUE;
				break;
			}
		default:
				bRet= FALSE;
	}

	return bRet;
}

// 2005.04.16. steeple
INT32 AgpmGuild::GetMemberLevelSum(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return 0;

	// 전체 멤버 루프를 돌면서 레벨을 더한다.
	INT32 lLevelSum = 0;

	AgpdGuildMember* pcsMember = NULL;
	INT32 lIndex = 0;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
													ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember)
			break;
		pcsMember = *ppcsMember;
		if(!pcsMember)
			break;

		lLevelSum += pcsMember->m_lLevel;
	}

	return lLevelSum;
}

AgpdGuild* AgpmGuild::GetGuild(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdGuildADChar* pcsGuildADChar = GetADCharacter(pcsCharacter);
	if(!pcsGuildADChar)
		return NULL;

	return GetGuild(pcsGuildADChar->m_szGuildID);
}

AgpdGuild* AgpmGuild::GetGuildLock(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdGuildADChar* pcsGuildADChar = GetADCharacter(pcsCharacter);
	if(!pcsGuildADChar)
		return NULL;

	return GetGuildLock(pcsGuildADChar->m_szGuildID);
}

// Memeber Data 를 생성한다.
AgpdGuildMember* AgpmGuild::CreateGuildMember(CHAR* szCharID)
{
	if(!szCharID)
		return NULL;

	AgpdGuildMember* pcsMember = (AgpdGuildMember*)CreateModuleData(AGPMGUILD_DATA_TYPE_MEMBER);
	if(!pcsMember)
		return FALSE;

	// 이름을 세팅한다.
	strncpy(pcsMember->m_szID, szCharID, AGPACHARACTER_MAX_ID_STRING);

	// Rank 는 일반회원으로
	pcsMember->m_lRank = AGPMGUILD_MEMBER_RANK_NORMAL;

	return pcsMember;
}

// Guild 에 Member 를 추가한다.
// pcsGuild 는 Lock 되어서 넘어온다.
BOOL AgpmGuild::AddMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	return PtrToInt(pcsGuild->m_pMemberList->AddObject((PVOID)&pcsMember, pcsMember->m_szID));
}

// Guild 에 Member 가 가입한다.
// 밖에서 Character 가 Lock 되어 있을 수도 있고, 아닐 수도 있다.
BOOL AgpmGuild::JoinMember(CHAR* szGuildID, CHAR* szCharID, INT32 lMemberRank, INT32 lJoinDate,
						   INT32 lLevel, INT32 lTID, INT8 cStatus)
{
	if(!szCharID || !szGuildID)
		return FALSE;

//	2005.08.03. By SungHoon
	if (lMemberRank == AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)		//	 서버 사이드라면 가입대기 리스트에 추가. 맘에 안드는 코드
	{
		PVOID ppBuffer[6];
		ppBuffer[0] = (PVOID)szGuildID;
		ppBuffer[1] = (PVOID)szCharID;
		ppBuffer[2] = (PVOID)&lMemberRank;
		ppBuffer[3] = (PVOID)&lJoinDate;
		ppBuffer[4] = (PVOID)&lLevel;
		ppBuffer[5] = (PVOID)&lTID;
		BOOL bResult = FALSE;
		EnumCallback(AGPMGUILD_CB_JOIN_REQUEST_SELF, ppBuffer, &bResult);
		if (bResult) return FALSE;
	}

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	BOOL bResult = JoinMember(pcsGuild, szCharID, lMemberRank, lJoinDate, lLevel, lTID, cStatus);

	if(bResult)
	{
		switch(lMemberRank)
		{
			case AGPMGUILD_MEMBER_RANK_SUBMASTER:
				{
					memcpy(pcsGuild->m_szSubMasterID, szCharID, AGPACHARACTER_MAX_ID_STRING);
				} break;
		}
	}

	pcsGuild->m_Mutex.Release();

	return bResult;
}

// pcsGuild 는 Lock 해서 들어온다.
BOOL AgpmGuild::JoinMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lMemberRank, INT32 lJoinDate,
						   INT32 lLevel, INT32 lTID, INT8 cStatus)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	BOOL bAleadyMember = FALSE;
	AgpdGuildMember* pcsMember = GetMember(pcsGuild, szCharID);
	if( pcsMember )
	{
		bAleadyMember = TRUE;
	}
	else
	{
		// Max 치에 도달했다면 실패
		if( pcsGuild->m_lMaxMemberCount <= GetMemberCount(pcsGuild) )
			return FALSE;

		pcsMember = CreateGuildMember(szCharID);
	}

	// 기타 데이터 들을 세팅하고
	if(lMemberRank != -1)	pcsMember->m_lRank		= lMemberRank;
	if(lJoinDate != -1)		pcsMember->m_lJoinDate	= lJoinDate;
	if(lLevel != -1)		pcsMember->m_lLevel		= lLevel;
	if(lTID != -1)			pcsMember->m_lTID		= lTID;
	if(cStatus != -1)		pcsMember->m_cStatus	= cStatus;

	return bAleadyMember ? TRUE : AddMember( pcsGuild, pcsMember );
}

// 클라이언트는 여기 들어와봤자 콜백이 없기 때문에, 항상 TRUE 이다.
BOOL AgpmGuild::CheckJoinEnable(CHAR* szGuildID, CHAR* szCharID, BOOL bSelfRequest)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	PVOID pvBuffer[3];
	pvBuffer[0] = szGuildID;
	pvBuffer[1] = szCharID;
	pvBuffer[2] = &bSelfRequest;
	BOOL bResult = TRUE;

	EnumCallback(AGPMGUILD_CB_JOIN_ENABLE_CHECK, pvBuffer, &bResult);
	return bResult;
}

// Guild 에서 Member 를 뺀다. (ApAdmin 에서만 빼는 것임)
// pcsGuild 는 Lock 되어서 넘어온다.
BOOL AgpmGuild::RemoveMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	return pcsGuild->m_pMemberList->RemoveObject(pcsMember->m_szID);
}

// Guild 에서 Member 가 탈퇴한다.
// szCharID 가 스스로 탈퇴하는 것임.
BOOL AgpmGuild::LeaveMember(CHAR* szGuildID, CHAR* szCharID, BOOL bNotifyMaster, BOOL bAdmin)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsMember = GetMember(pcsGuild, szCharID);
	if(!pcsMember)
	{		//	가입 대기중일때는 베틀중이라도 상관없다.
		if (pcsGuild->m_pGuildJoinList->RemoveObject(szCharID))
			return TRUE;		//	가입대기 리스트도 체크한다.2005.08.16. By SungHoon

		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// Admin일 경우 이런저런 이유 불문, 무조건 짜른다.
	if (!bAdmin)
	{
//		해당 길드가 전투중이면 안된다.
		if(IsBattleStatus(pcsGuild))
		{
			if (bNotifyMaster)
			{
				AgpdCharacter *pcsMasterCharacter = NULL;
				ApAutoLockCharacter Lock(m_pagpmCharacter, pcsGuild->m_szMasterID);
				pcsMasterCharacter = Lock.GetCharacterLock();
				if (pcsMasterCharacter)
					OnOperationSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, NULL, NULL, 0, 0, pcsMasterCharacter);
			}
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}
	}

	BOOL bResult = RemoveMember(pcsGuild, pcsMember);

	if(bResult)
		DestroyModuleData(pcsMember, AGPMGUILD_DATA_TYPE_MEMBER);
	
	pcsGuild->m_Mutex.Release();

	return bResult;
}

// Guild 에서 Member 가 짤린다.
// szForceID 가 szCharID 를 짜르는데, 권한이 없으면 못 짜름.
BOOL AgpmGuild::ForcedLeaveMember(CHAR* szGuildID, CHAR* szForceID, CHAR* szCharID)
{
	if(!szGuildID || !szForceID || !szCharID)
		return FALSE;

	// 같은 사람이다. 장난하냐??
	if(strcmp(szForceID, szCharID) == 0)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

//	해당 길드가 전투중이면 안된다.
	if(IsBattleStatus(pcsGuild))
	{
		AgpdCharacter *pcsMasterCharacter = NULL;
		ApAutoLockCharacter Lock(m_pagpmCharacter, pcsGuild->m_szMasterID);
		pcsMasterCharacter = Lock.GetCharacterLock();
		if (pcsMasterCharacter)
			OnOperationSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, NULL, NULL, 0, 0, pcsMasterCharacter);

		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	AgpdGuildMember* pcsForce = GetMember(pcsGuild, szForceID);
	AgpdGuildMember* pcsLeave = GetMember(pcsGuild, szCharID);
	if(!pcsForce || !pcsLeave)
	{
		if (pcsGuild->m_pGuildJoinList->RemoveObject(szCharID)) return TRUE;		//	가입대기 리스트도 체크한다.2005.08.16. By SungHoon
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// 강퇴시키는 사람이 권한이 있어야 한다. 현재는 마스터만 가능
	if(pcsForce->m_lRank != AGPMGUILD_MEMBER_RANK_MASTER)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	BOOL bResult = RemoveMember(pcsGuild, pcsLeave);

	if(bResult)
		DestroyModuleData(pcsLeave, AGPMGUILD_DATA_TYPE_MEMBER);
	
	pcsGuild->m_Mutex.Release();
	return TRUE;
}

// Member Status 를 업데이트 한다.
inline BOOL AgpmGuild::UpdateMemberStatus(CHAR* szGuildID, CHAR* szMemberID, INT32 lMemberRank, INT32 lJoinDate,
								   INT32 lLevel, INT32 lTID, INT8 cStatus)
{
	if(!szGuildID || !szMemberID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsMember = GetMember(pcsGuild, szMemberID);
	if(pcsMember)
	{
		UpdateMemberStatus(pcsGuild, pcsMember, lMemberRank, lJoinDate, lLevel, lTID, cStatus);
	}
	else
	{
		// 못 구하면 새로 넣어버린다.
		if (lMemberRank == AGPMGUILD_MEMBER_RANK_JOIN_REQUEST || lMemberRank == -1 )		//	가입 대기중인 유저면 무시한다.
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}
		BOOL bJoin = JoinMember(pcsGuild, szMemberID, lMemberRank, lJoinDate, lLevel, lTID, cStatus);
		if(bJoin)
		{
			pcsMember = GetMember(pcsGuild, szMemberID);
			if(pcsMember)
			{
				PVOID pvBuffer[2];
				pvBuffer[0] = pcsMember;
				pvBuffer[1] = &cStatus;
				EnumCallback(AGPMGUILD_CB_UPDATE_MEMBER_STATUS, pcsGuild, pvBuffer);
			}
		}
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// Member Status 를 업데이트 한다.
// pcsGuild 는 Lock 되어서 넘어온다.
inline BOOL AgpmGuild::UpdateMemberStatus(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, INT32 lMemberRank, INT32 lJoinDate,
								   INT32 lLevel, INT32 lTID, INT8 cStatus)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT8 cOldStatus = pcsMember->m_cStatus;

	PVOID pvBuffer[2];
	pvBuffer[0] = pcsMember;
	pvBuffer[1] = &cOldStatus;	// 이전 상태를 넣어준다.

	if(lMemberRank != -1)
		pcsMember->m_lRank = lMemberRank;
	if(lJoinDate != -1)
		pcsMember->m_lJoinDate = GetCurrentTimeStamp() - lJoinDate;
	if(lLevel != -1)
		pcsMember->m_lLevel = lLevel;
	if(lTID != -1)
		pcsMember->m_lTID = lTID;
	if(cStatus != -1)
		pcsMember->m_cStatus = cStatus;

	// Status 가 바꼈다고 EnumCallback
	EnumCallback(AGPMGUILD_CB_UPDATE_MEMBER_STATUS, pcsGuild, pvBuffer);

	return TRUE;
}

// 모든 Member CID 를 리스트에 넣어준다.
// Character 객체를 가져와야 하기 때문에 온라인인 경우에만 가능하고, 클라이언트에서는 전체를 얻기 힘들다.
INT32 AgpmGuild::GetMemberCIDList(AgpdCharacter* pcsCharacter, list<INT32>* pList)
{
	if(!pcsCharacter || !pList)
		return 0;

	AgpdGuildADChar* pcsGuildADChar = GetADCharacter(pcsCharacter);
	if(!pcsGuildADChar)
		return 0;

	if(strlen(pcsGuildADChar->m_szGuildID) == 0)
		return 0;

	return GetMemberCIDList(pcsGuildADChar->m_szGuildID, pList);
}

INT32 AgpmGuild::GetMemberCIDList(CHAR* szGuildID, list<INT32>* pList)
{
	if(!szGuildID || !pList)
		return 0;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(!pcsGuild)
		return 0;

	INT32 lCount = GetMemberCIDList(pcsGuild, pList);

	pcsGuild->m_Mutex.Release();

	return lCount;
}

// 모든 Member CID 를 리스트에 넣어준다.
// Character 객체를 가져와야 하기 때문에 온라인인 경우에만 가능하고, 클라이언트에서는 전체를 얻기 힘들다.
INT32 AgpmGuild::GetMemberCIDList(AgpdGuild* pcsGuild, list<INT32>* pList)
{
	if(!pcsGuild || !pList)
		return 0;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lCount = 0;
	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
		ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			break;

		pcsCharacter = m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID);
		if(!pcsCharacter)
			continue;

		pList->push_back(pcsCharacter->m_lID);
		lCount++;
	}

	return lCount;
}

// 캐릭터가 길드에 가입되어 있는 지 아닌 지 확인한다.
BOOL AgpmGuild::IsAnyGuildMember(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetJoinedGuildID(pcsCharacter);
	if(!szGuildID)
		return FALSE;

	return strlen(szGuildID) > 0 ? TRUE : FALSE;
}

// 캐릭터가 가입한 길드 아이디를 리턴한다.
CHAR* AgpmGuild::GetJoinedGuildID(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	AgpdGuildADChar* pcsGuildADChar = GetADCharacter(pcsCharacter);
	if(!pcsGuildADChar)
		return NULL;

	return pcsGuildADChar->m_szGuildID;
}

// Character Attached Data
// pcsCharacter 은 Lock 되어 있음.
// GuildLock 은 pcsMember 가 NULL 이 아닐 때만 되어있음
inline BOOL AgpmGuild::SetCharAD(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT32 lGuildMarkTID, INT32 lGuildMarkColor, 
								 INT32 lBRRanking, AgpdGuildMember* pcsMember)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// szGuildID 가 NULL 이거나, pcsMember 가 NULL 일수도 있다.
	// 그것은 해당 캐릭터가 Guild 에서 탈퇴한 것(짤렸다던가.. 등등) 이라고 보면 됨.

	memset(pcsAttachedGuild->m_szGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));

	if(szGuildID && strlen(szGuildID) > 0)
	{
		strncpy(pcsAttachedGuild->m_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		if (lGuildMarkTID != -1) pcsAttachedGuild->m_lGuildMarkTID = lGuildMarkTID;
		if (lGuildMarkColor != -1) pcsAttachedGuild->m_lGuildMarkColor = lGuildMarkColor;
		pcsAttachedGuild->m_lBRRanking = lBRRanking;
	}
	else
	{
		pcsAttachedGuild->m_lGuildMarkTID = 0;
		pcsAttachedGuild->m_lGuildMarkColor = 0xFFFFFFFF;
		pcsAttachedGuild->m_lBRRanking = lBRRanking;
	}
	pcsAttachedGuild->m_pMemberData = pcsMember;

	return TRUE;
}

// CharID 로 Guild 를 찾아서 GuildID 를 Attached Data 에 세팅해준다.
// 제대로 세팅되면 return TRUE. 길드가 없다면 return FALSE
// 나중에 LoginServer 에서 DB 로 넘길시에는 필요없는 함수가 될 듯.
BOOL AgpmGuild::FindCharGuildID(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) != 0)	// 이미 세팅 되어 있으면 나간다.
		return TRUE;

	AgpdGuild** ppcsGuild = NULL;
	AgpdGuild* pcsGuild = NULL;
	AgpdGuildMember* pcsMember = NULL;

	INT32 lIndex = 0;
	for(ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild;
			ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!ppcsGuild)
			continue;
		pcsGuild = *ppcsGuild;
		if(!pcsGuild)
			continue;

		pcsGuild->m_Mutex.WLock();
		if(pcsGuild->m_bRemove)
		{
			pcsGuild->m_Mutex.Release();
			continue;
		}

		pcsMember = GetMember(pcsGuild, pcsCharacter->m_szID);
		if(pcsMember)
		{
			memset(pcsAttachedGuild->m_szGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
			memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
			strncpy(pcsAttachedGuild->m_szGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
			pcsAttachedGuild->m_lGuildMarkTID = pcsGuild->m_lGuildMarkTID;
			pcsAttachedGuild->m_lGuildMarkColor = pcsGuild->m_lGuildMarkColor;
			pcsAttachedGuild->m_lBRRanking = pcsGuild->m_lBRRanking;

			pcsGuild->m_Mutex.Release();
			return TRUE;
		}
//	가입대기 리스트도 검사한다.
		AgpdGuildRequestMember **ppcsRequestMember = (AgpdGuildRequestMember **)pcsGuild->m_pGuildJoinList->GetObject(pcsCharacter->m_szID);
		if (!ppcsRequestMember)
		{
			AgpdGuildRequestMember *pcsRequestMember = *ppcsRequestMember;
			if (!pcsRequestMember)
			{
				memset(pcsAttachedGuild->m_szGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
				memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
				strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
				pcsGuild->m_Mutex.Release();
				return TRUE;
			}
		}

		pcsGuild->m_Mutex.Release();
	}

	// 못 찾았으면 return FALSE
	return FALSE;
}

// CharID 로 Guild 를 찾는다.
// 리턴값은 Guild 포인터 이다. Lock 해서 넘겨준다.
AgpdGuild* AgpmGuild::FindGuildLockFromCharID(CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("FindGuildLockFromCharID"));

	AgpdGuild** ppcsGuild = NULL;
	AgpdGuild* pcsGuild = NULL;
	AgpdGuildMember* pcsMember = NULL;

	INT32 lIndex = 0;
	for(ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild;
			ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!ppcsGuild)
			continue;
		pcsGuild = *ppcsGuild;
		if(!pcsGuild)
			continue;

		pcsGuild->m_Mutex.WLock();
		if(pcsGuild->m_bRemove)
		{
			pcsGuild->m_Mutex.Release();
			continue;
		}

		pcsMember = GetMember(pcsGuild, szCharID);
		if(pcsMember)
		{
			// 멤버를 찾으면 바로 길드 포인터를 리턴
			return pcsGuild;
		}
		//	가입대기 리스트도 검사한다.
		AgpdGuildRequestMember **ppcsRequestMember = (AgpdGuildRequestMember **)pcsGuild->m_pGuildJoinList->GetObject(szCharID);
		if (ppcsRequestMember)
		{
			if (*ppcsRequestMember)
				return pcsGuild;
		}

		pcsGuild->m_Mutex.Release();
	}

	// 못 찾았으면 return FALSE
	return NULL;
}





//////////////////////////////////////////////////////////////////////////
// Battle
BOOL AgpmGuild::IsBattleStatus(AgpdGuild* pcsGuild)
{
	if( IsBattleReadyStatus(pcsGuild) || IsBattleIngStatus(pcsGuild) || IsBattleDeclareStatus(pcsGuild) )
		return TRUE;

	return  FALSE;
}

BOOL AgpmGuild::IsBattleDeclareStatus(AgpdGuild* pcsGuild)
{
	return pcsGuild && pcsGuild->m_cStatus == AGPMGUILD_STATUS_BATTLE_DECLARE ? TRUE : FALSE;
}

BOOL AgpmGuild::IsBattleReadyStatus(AgpdGuild* pcsGuild)
{
	return pcsGuild && pcsGuild->m_cStatus == AGPMGUILD_STATUS_BATTLE_READY ? TRUE : FALSE;
}

BOOL AgpmGuild::IsBattleIngStatus(AgpdGuild* pcsGuild)
{
	return pcsGuild && pcsGuild->m_cStatus == AGPMGUILD_STATUS_BATTLE ? TRUE : FALSE;
}

BOOL AgpmGuild::CheckBattleType(AgpdGuild* pGuild, AgpdGuild* pEnemyGuild, eGuildBattleType eType )
{
	if( !pGuild || !pEnemyGuild )	return FALSE;

	switch( eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		return TRUE;
	case eGuildBattleTotalSurvive:
	case eGuildBattlePrivateSurvive:
		return pGuild->GetBattleMemberCount() == pEnemyGuild->GetBattleMemberCount() ? TRUE: FALSE;
	}

	return FALSE;
}

BOOL AgpmGuild::IsEnemyGuild(CHAR* szGuildID, CHAR* szEnemyGuildID, BOOL bBattleIng)
{
	if(!szGuildID || !szEnemyGuildID)
		return FALSE;

	BOOL bResult = FALSE;

	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if(pcsGuild)
	{
		bResult = IsEnemyGuild(pcsGuild, szEnemyGuildID, bBattleIng);

		pcsGuild->m_Mutex.Release();
	}

	return bResult;
}

BOOL AgpmGuild::IsEnemyGuild(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID, BOOL bBattleIng)
{
	if(!pcsGuild || !szEnemyGuildID || strlen(szEnemyGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	// Battle 중일 때, 적 길드인지 체크해야 한다면
	// pcsGuild 가 BattleIngStatus 인지 확인한다.
	if(bBattleIng && IsBattleIngStatus(pcsGuild) == FALSE)
		return FALSE;

	return strcmp(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID) == 0 ? TRUE : FALSE;
}

BOOL AgpmGuild::InitCurrentBattleInfo(AgpdGuild* pcsGuild)
{
	if( !pcsGuild )		return FALSE;

	// 2005.04.20. steeple
	// 아 그냥 Status 도 초기화 같이 해버리기로 함. -0-;
	pcsGuild->m_cStatus = AGPMGUILD_STATUS_NONE;
	pcsGuild->m_csCurrentBattleInfo.Init();
	pcsGuild->BattleMemberInit();
	return TRUE;
}

INT32 AgpmGuild::GetMinimumWinPoint(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild, UINT32 ulDuration)
{
	if(!pcsGuild || !pcsEnemyGuild)
		return 0;

	// ulDuration 은 초단위로 넘어온다.
	if(ulDuration == 0)
		ulDuration = AGPMGUILD_BATTLE_DURATION_MIN / 60;
	else
		ulDuration /= 60;

	INT32 lLevelAvg = (INT32)(GetMemberLevelSum(pcsGuild) / (GetMemberCount(pcsGuild) != 0 ? GetMemberCount(pcsGuild) : 20));
	INT32 lEnemyLevelAvg = (INT32)(GetMemberLevelSum(pcsEnemyGuild) / (GetMemberCount(pcsEnemyGuild) != 0 ? GetMemberCount(pcsEnemyGuild) : 20));

	INT32 lLevelGap = lLevelAvg - lEnemyLevelAvg;
	INT32 lBattlePoint = GetBattlePoint(lLevelGap);

	lLevelGap = abs(lLevelGap);
	if(lLevelGap == 0)
		lLevelGap = 1;	// 아래 공식에서 0 으로 가는 것을 방지
	if(lBattlePoint == 0)
		lBattlePoint = 1;

	INT32 lMinimumWinPoint = (INT32)((FLOAT)(lLevelGap * lBattlePoint * ulDuration) / 2.0f);
	return lMinimumWinPoint;
}

INT32 AgpmGuild::GetBattlePoint(INT32 lLevelGap)
{
	lLevelGap = min( max( m_lBattlePointMinLevelGap, lLevelGap ), m_lBattlePointMaxLevelGap );
	if( lLevelGap == 0 )
		lLevelGap = AGPMGUILD_BATTLE_POINT_LEVEL_GAP_ZERO;

	AgpdGuildBattlePoint** ppcsBattlePoint = (AgpdGuildBattlePoint**)m_csBattlePointAdmin.GetObject( lLevelGap );
	return ppcsBattlePoint && (*ppcsBattlePoint) ? (*ppcsBattlePoint)->m_lPoint : 0;
}

BOOL AgpmGuild::SetMaxBattlePointAdmin(INT32 lCount)
{
	if(lCount <= 0)
		return FALSE;

	m_csBattlePointAdmin.SetCount(lCount);
	return m_csBattlePointAdmin.InitializeObject(sizeof(AgpdGuildBattlePoint*), lCount);
}

BOOL AgpmGuild::ReadBattlePointTxt(CHAR* szFile, BOOL bDecryption)
{
	if(!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16 nRow = csExcelTxtLib.GetRow();
	INT16 nCol = csExcelTxtLib.GetColumn();

	INT16 nCurrentRow = 1;
	INT16 nCurrentCol = 1;
	INT16 nIndex = 0;

	INT32 lLevelGap = 0;
	INT32 lBattlePoint = 0;

	CHAR* szBuffer = NULL;
	while(true)
	{
		// Level Gap
		szBuffer = csExcelTxtLib.GetData(0, nCurrentRow);
		if(!szBuffer)
			break;
		lLevelGap = atoi(szBuffer);

		AgpdGuildBattlePoint* pcsBattlePoint = (AgpdGuildBattlePoint*)CreateModuleData(AGPMGUILD_DATA_TYPE_BATTLE_POINT);
	
		// Level Gap
		pcsBattlePoint->m_lLevelGap = lLevelGap;
		if(m_lBattlePointMinLevelGap > pcsBattlePoint->m_lLevelGap)
			m_lBattlePointMinLevelGap = pcsBattlePoint->m_lLevelGap;
		if(m_lBattlePointMaxLevelGap < pcsBattlePoint->m_lLevelGap)
			m_lBattlePointMaxLevelGap = pcsBattlePoint->m_lLevelGap;

		// Battle Point
        szBuffer = csExcelTxtLib.GetData(1, nCurrentRow);
		lBattlePoint = atoi(szBuffer);
		pcsBattlePoint->m_lPoint = lBattlePoint;

		// Admin 은 Key 를 0 으로 주면 안된다.
		if(pcsBattlePoint->m_lLevelGap == 0)
			lLevelGap = AGPMGUILD_BATTLE_POINT_LEVEL_GAP_ZERO;

		if(!m_csBattlePointAdmin.AddObject((PVOID)&pcsBattlePoint, lLevelGap))
			DestroyModuleData(pcsBattlePoint, AGPMGUILD_DATA_TYPE_BATTLE_POINT);

		nCurrentRow++;
		if(nCurrentRow > nRow)
			break;
	}

	return TRUE;
}






// Relation Guild
BOOL AgpmGuild::AddJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, INT8 cRelation)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pJointVector)
		return FALSE;

	if(pcsGuild->m_csRelation.m_pJointVector->size() >= AGPMGUILD_MAX_JOINT_GUILD)
		return FALSE;

	if(IsJointGuild(pcsGuild, szGuildID))
		return FALSE;

	AgpdGuildRelationUnit stUnit;
	memset(&stUnit, 0, sizeof(stUnit));

	_tcsncpy(stUnit.m_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	stUnit.m_ulDate = ulDate;
	stUnit.m_cRelation = cRelation;

	pcsGuild->m_csRelation.m_pJointVector->push_back(stUnit);

	return TRUE;
}

BOOL AgpmGuild::IsJointGuild(CHAR* szGuildID, CHAR* szJointGuildID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	return IsJointGuild(pcsGuild, szJointGuildID);
}

BOOL AgpmGuild::IsJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pJointVector)
		return FALSE;

	JointIter iter = std::find_if(pcsGuild->m_csRelation.m_pJointVector->begin(),
									pcsGuild->m_csRelation.m_pJointVector->end(),
									FindRelation(szGuildID));
	return iter != pcsGuild->m_csRelation.m_pJointVector->end();
}

BOOL AgpmGuild::IsJointLeader(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !pcsGuild->m_csRelation.m_pJointVector)
		return FALSE;

	if(!szGuildID)
		szGuildID = pcsGuild->m_szID;

	JointIter iter = std::find_if(pcsGuild->m_csRelation.m_pJointVector->begin(),
									pcsGuild->m_csRelation.m_pJointVector->end(),
									FindRelation(szGuildID));
	if(iter == pcsGuild->m_csRelation.m_pJointVector->end())
		return FALSE;

	if(iter->m_cRelation == AGPMGUILD_RELATION_JOINT_LEADER)
		return TRUE;
	else
		return FALSE;
}

BOOL AgpmGuild::RemoveJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pJointVector)
		return FALSE;

	JointIter iter = std::find_if(pcsGuild->m_csRelation.m_pJointVector->begin(),
									pcsGuild->m_csRelation.m_pJointVector->end(),
									FindRelation(szGuildID));
	if(iter == pcsGuild->m_csRelation.m_pJointVector->end())
		return FALSE;

    pcsGuild->m_csRelation.m_pJointVector->erase(iter);
	return TRUE;
}

BOOL AgpmGuild::ClearJointGuild(AgpdGuild* pcsGuild)
{
	if(!pcsGuild || !pcsGuild->m_csRelation.m_pJointVector)
		return FALSE;

	pcsGuild->m_csRelation.m_pJointVector->clear();
	return TRUE;
}

AgpdGuildRelationUnit* AgpmGuild::GetJointGuild(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pJointVector)
		return NULL;

	JointIter iter = std::find_if(pcsGuild->m_csRelation.m_pJointVector->begin(),
									pcsGuild->m_csRelation.m_pJointVector->end(),
									FindRelation(szGuildID));
	if(iter == pcsGuild->m_csRelation.m_pJointVector->end())
		return NULL;

	return &(*iter);
}

INT32 AgpmGuild::GetJointGuildCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild || !pcsGuild->m_csRelation.m_pJointVector)
		return 0;

	return (INT32)pcsGuild->m_csRelation.m_pJointVector->size();
}

BOOL AgpmGuild::AddHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pHostileVector)
		return FALSE;

	if(pcsGuild->m_csRelation.m_pHostileVector->size() >= AGPMGUILD_MAX_HOSTILE_GUILD)
		return FALSE;

	if(IsHostileGuild(pcsGuild, szGuildID))
		return FALSE;

	AgpdGuildRelationUnit stUnit;
	memset(&stUnit, 0, sizeof(stUnit));

	_tcsncpy(stUnit.m_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	stUnit.m_ulDate = ulDate;
	stUnit.m_cRelation = (INT8)AGPMGUILD_RELATION_HOSTILE;

	pcsGuild->m_csRelation.m_pHostileVector->push_back(stUnit);

	return TRUE;
}

BOOL AgpmGuild::IsHostileGuild(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	return IsHostileGuild(pcsGuild, szHostileGuildID);
}

BOOL AgpmGuild::IsHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pHostileVector)
		return FALSE;

	HostileIter iter = std::find_if(pcsGuild->m_csRelation.m_pHostileVector->begin(),
									pcsGuild->m_csRelation.m_pHostileVector->end(),
									FindRelation(szGuildID));
	return iter != pcsGuild->m_csRelation.m_pHostileVector->end();
}

BOOL AgpmGuild::IsHostileGuild(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	CHAR* szGuildID = GetJoinedGuildID(pcsCharacter);
	CHAR* szHostileGuildID = GetJoinedGuildID(pcsTarget);
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	return IsHostileGuild(szGuildID, szHostileGuildID);
}

BOOL AgpmGuild::RemoveHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pHostileVector)
		return FALSE;

	HostileIter iter = std::find_if(pcsGuild->m_csRelation.m_pHostileVector->begin(),
									pcsGuild->m_csRelation.m_pHostileVector->end(),
									FindRelation(szGuildID));
	if(iter == pcsGuild->m_csRelation.m_pHostileVector->end())
		return FALSE;

    pcsGuild->m_csRelation.m_pHostileVector->erase(iter);
	return TRUE;
}

AgpdGuildRelationUnit* AgpmGuild::GetHostileGuild(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID || _tcslen(szGuildID) == 0 || !pcsGuild->m_csRelation.m_pHostileVector)
		return NULL;

	HostileIter iter = std::find_if(pcsGuild->m_csRelation.m_pHostileVector->begin(),
									pcsGuild->m_csRelation.m_pHostileVector->end(),
									FindRelation(szGuildID));
	if(iter == pcsGuild->m_csRelation.m_pHostileVector->end())
		return NULL;

	return &(*iter);
}

INT32 AgpmGuild::GetHostileGuildCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild || !pcsGuild->m_csRelation.m_pHostileVector)
		return 0;

	return (INT32)pcsGuild->m_csRelation.m_pHostileVector->size();
}

void AgpmGuild::SetLastRelationWaitGuildID(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild)
		return;

	_tcsncpy(pcsGuild->m_csRelation.m_szLastWaitGuildID, szGuildID ? szGuildID : _T(""), AGPMGUILD_MAX_GUILD_ID_LENGTH);
}

const CHAR* AgpmGuild::GetLastRelationWaitGuildID(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return NULL;

	return pcsGuild->m_csRelation.m_szLastWaitGuildID;
}

BOOL AgpmGuild::IsLastRelationWaitGuildID(AgpdGuild* pcsGuild, CHAR* szGuildID)
{
	if(!pcsGuild || !szGuildID)
		return 0;

	return !_tcscmp(pcsGuild->m_csRelation.m_szLastWaitGuildID, szGuildID);
}









//////////////////////////////////////////////////////////////////////////
// Etc
UINT32 AgpmGuild::GetCurrentTimeStamp()
{
	time_t timeval;
	(void)time(&timeval);
	
	return (UINT32)timeval;
}

UINT32 AgpmGuild::GetCurrentTimeDate(CHAR* szTimeBuf)
{
	if(!szTimeBuf)
		return 0;

	INT32 lCurrentTimeStamp = GetCurrentTimeStamp();

	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)lCurrentTimeStamp;
	tm_ptr = localtime(&timeval);

	strftime(szTimeBuf, 32, "%Y%m%d%H%M", tm_ptr);
	return (UINT32)strlen(szTimeBuf);
}

BOOL AgpmGuild::WriteGuildLog(CHAR* szMessage)
{
	return TRUE;

	if(!szMessage)
		return FALSE;

	FILE* pfFile = fopen(AGPMGUILD_LOG_FILE_NAME, "a+");
	if(!pfFile)
		return FALSE;

	fprintf(pfFile, "%s", szMessage);

	fclose(pfFile);

	return TRUE;
}

// 2007.07.03. steeple
BOOL AgpmGuild::IsSameGuild(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	CHAR* szGuildID1 = GetJoinedGuildID(pcsCharacter);
	CHAR* szGuildID2 = GetJoinedGuildID(pcsTarget);
	if(szGuildID1 && szGuildID2 && _tcscmp(szGuildID1, szGuildID2) == 0)
		return TRUE;

	return FALSE;
}




//////////////////////////////////////////////////////////////////////////
// Packet
PVOID AgpmGuild::MakeGuildPacket(BOOL bPacket, INT16* pnPacketLength, UINT8 cType, INT8* pcOperation, INT32* plCID,
						CHAR* szGuildID, CHAR* szMasterID, INT32* plGuildTID, INT32* plGuildRank, INT32* plCreationDate, INT32* plMaxMemberCount,
						INT32* plUnionID, CHAR* szPassword, CHAR* szNotice, INT16* pnNoticeLength,
						INT8* pcStatus, INT32* plWin, INT32* plDraw, INT32* plLose, INT32* pGuildPoint,
						PVOID pvMemberPacket, PVOID pvBattlePacket, PVOID pvBattlePersonPacket, PVOID pvBattleMemberPacket, PVOID pvBattleMemberListPacket, PVOID pvGuildListPacket, 
						INT32* plGuildMarkTID, INT32 *plGuildMarkColor, BOOL *pIsWinner)
{
	if(szNotice && pnNoticeLength)
		return m_csPacket.MakePacket(bPacket, pnPacketLength, cType, pcOperation, plCID,
						szGuildID, szMasterID, plGuildTID, plGuildRank, plCreationDate, plMaxMemberCount, plUnionID, szPassword,
						szNotice, pnNoticeLength,
						pcStatus, plWin, plDraw, plLose, pGuildPoint,
						pvMemberPacket,
						pvBattlePacket,
						pvBattlePersonPacket,
						pvBattleMemberPacket,
						pvBattleMemberListPacket,
						pvGuildListPacket,
						plGuildMarkTID,
						plGuildMarkColor,
						pIsWinner
						);
	else
		return m_csPacket.MakePacket(bPacket, pnPacketLength, cType, pcOperation, plCID,
						szGuildID, szMasterID, plGuildTID, plGuildRank, plCreationDate, plMaxMemberCount, plUnionID, szPassword,
						NULL,
						pcStatus, plWin, plDraw, plLose, pGuildPoint,
						pvMemberPacket,
						pvBattlePacket,
						pvBattlePersonPacket,
						pvBattleMemberPacket,
						pvBattleMemberListPacket,
						pvGuildListPacket,
						plGuildMarkTID,
						plGuildMarkColor,
						pIsWinner
						);
}

PVOID AgpmGuild::MakeGuildCreatePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, INT32* plGuildTID,
						INT32* plGuildRank, INT32* plCreationDate, INT32* plMaxMemberCount, INT32* plUnionID, CHAR* szPassword,
						CHAR* szNotice, INT16* pnNoticeLength, 
						INT8* pcStatus, INT32* plWin, INT32* plDraw, INT32* plLose, INT32* pGuildPoint,
						INT32 *plGuildMarkTID, INT32 *plGuildMarkColor, BOOL *pIsWinner)
{
	INT8 cOperation = AGPMGUILD_PACKET_CREATE;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, szMasterID, plGuildTID, plGuildRank, plCreationDate, plMaxMemberCount, plUnionID, szPassword,
						szNotice, pnNoticeLength,
						pcStatus, plWin, plDraw, plLose, pGuildPoint,
						NULL, NULL, NULL, NULL, NULL, NULL,
						plGuildMarkTID, plGuildMarkColor, pIsWinner);
}

PVOID AgpmGuild::MakeGuildJoinRequestPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szCharID, NULL, NULL, NULL, NULL, NULL);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOIN_REQUEST;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, szMasterID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

PVOID AgpmGuild::MakeGuildJoinRejectPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szCharID, NULL, NULL, NULL, NULL, NULL);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOIN_REJECT;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

PVOID AgpmGuild::MakeGuildJoinPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMemberID, INT32* plMemberRank,
									 INT32* plJoinDate, INT32* plLevel, INT32* plTID, INT8* pcStatus)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szMemberID, plMemberRank, plJoinDate, plLevel, plTID, pcStatus);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOIN;
	PVOID pvPacket =  MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

PVOID AgpmGuild::MakeGuildMemberPacket(INT16* pnPacketLength, INT8 *pOperation, INT32* plCID, CHAR* szGuildID, CHAR* szMemberID, INT32 *plMemberRank, 
									   INT32* plJoinDate, INT32* plLevel, INT32* plTID, INT8* pcStatus)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szMemberID, plMemberRank, plJoinDate, plLevel, plTID, pcStatus);

	if(!pvMemberPacket)
		return NULL;

	PVOID pvPacket =  MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, pOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

/*
	2005.08.12. By SungHoon
	실제 길드 탈퇴 패킷을 생성한다.
*/
PVOID AgpmGuild::MakeGuildLeaveAllowPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szCharID, NULL, NULL, NULL, NULL, NULL);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_LEAVE_ALLOW;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}


PVOID AgpmGuild::MakeGuildLeavePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szCharID, NULL, NULL, NULL, NULL, NULL);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_LEAVE;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL,  NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

PVOID AgpmGuild::MakeGuildForcedLeavePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szCharID, NULL, NULL, NULL, NULL, NULL);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_FORCED_LEAVE;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, szMasterID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}


PVOID AgpmGuild::MakeGuildDestroyPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword)
{
	INT8 cOperation = AGPMGUILD_PACKET_DESTROY;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, szMasterID, NULL, NULL, NULL, NULL, NULL, szPassword, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
}

PVOID AgpmGuild::MakeGuildUpdateMaxMemberCountPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT32* plNewMaxMemberCount)
{
	INT8 cOperation = AGPMGUILD_PACKET_UPDATE_MAX_MEMBER_COUNT;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, plNewMaxMemberCount, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
}

PVOID AgpmGuild::MakeGuildCharDataPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID, INT32* plRank,
										 INT32* plJoinDate, INT32* plLevel, INT32* plTID, INT8* pcStatus,
										 INT32* plGuildMarkTID, INT32* plGuildMarkColor, INT32* pIsWinner)
{
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szCharID, plRank, plJoinDate, plLevel, plTID, pcStatus);

	if(!pvMemberPacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_CHAR_DATA;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						plGuildMarkTID, plGuildMarkColor, pIsWinner );
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

PVOID AgpmGuild::MakeGuildUpdateNoticePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID,
						CHAR* szNotice, INT16* pnNoticeLength)
{
	INT8 cOperation = AGPMGUILD_PACKET_UPDATE_NOTICE;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
						szGuildID, szMasterID, NULL, NULL, NULL, NULL, NULL, NULL,
						szNotice, pnNoticeLength,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
}

// System Message 를 Code 로 보낸다.
// 각 대응 변수들
//			CID <= plCode
//			GuildID <= szData1
//			MasterID <= szData2
//			GuildTID <= lData1
//			GuildRank <= lData2
PVOID AgpmGuild::MakeGuildSystemMessagePacket(INT16* pnPacketLength, INT32* plCode, CHAR* szData1, CHAR* szData2, INT32* plData1, INT32* plData2)
{
	INT8 cOperation = AGPMGUILD_PACKET_SYSTEM_MESSAGE;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCode,
						szData1, szData2, plData1, plData2, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
}

// Battle Packet
PVOID AgpmGuild::MakeGuildBattlePacket(PACKET_GUILD_BATTLE& pPacket)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pPacket.pnPacketLength, 0, 
		pPacket.szEnemyGuildID, 
		pPacket.szEnemyGuildMasterID,
		pPacket.pulAcceptTime,
		pPacket.pulReadyTime,
		pPacket.pulStartTime, 
		pPacket.pType,
		pPacket.pulDuration,
		pPacket.pPerson,
		pPacket.pulCurrentTime,
		pPacket.plMyScore,
		pPacket.plEnemyScore,
		pPacket.plMyUpScore,
		pPacket.plEnemyUpScore,
		pPacket.pcBattleResult,
		pPacket.ulRound
		);

	if(!pvBattlePacket)
		return NULL;
		
	PVOID pvPacket = MakeGuildPacket(TRUE, pPacket.pnPacketLength, AGPMGUILD_PACKET_TYPE, 
		pPacket.pcOperation, 
		pPacket.plCID, 
		pPacket.szGuildID, 
		pPacket.szMasterID,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		pPacket.pcStatus, 
		pPacket.plWin, 
		pPacket.plDraw, 
		pPacket.plLose, 
		pPacket.pGuildPoint,
		NULL, 
		pvBattlePacket, 
		NULL, NULL, NULL, NULL,	NULL, NULL, NULL);

	m_csBattlePacket.FreePacket(pvBattlePacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

PVOID AgpmGuild::MakeGuildBattlePersonPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT16* pPersonCount, CHAR* pPersonList, INT16* pSize )
{
	PVOID pvPersonPacket = m_csBattlePersonPacket.MakePacket( FALSE, pnPacketLength, 0, pPersonCount, pPersonList, pSize );
	if( !pvPersonPacket )	return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_BATTLE_PERSON;
	PVOID pGuildPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
										szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
										NULL, NULL, NULL, NULL, NULL,
										NULL, NULL, pvPersonPacket, NULL, NULL, NULL,
										NULL, NULL, NULL);

	m_csBattlePersonPacket.FreePacket( pvPersonPacket );
	return pGuildPacket;
}

PVOID AgpmGuild::MakeGuildBattleRequestPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, INT32* pType, UINT32* pulDuration, UINT32* pPerson)
{
	PACKET_GUILD_BATTLE_REQUEST pPacketGuildBattleRequest(pnPacketLength, plCID, szGuildID, szMasterID, szEnemyGuildID, pType, pulDuration, pPerson);

	return MakeGuildBattlePacket(pPacketGuildBattleRequest);
}

PVOID AgpmGuild::MakeGuildBattleAcceptPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32* pulAcceptTime, UINT32* pulReadyTime, UINT32* pulStartTime, 
											INT32* pType, UINT32* pulDuration, UINT32* pPerson)
{
	PACKET_GUILD_BATTLE_ACCEPT pPacketGuildBattleAccept(pnPacketLength, plCID, szGuildID, szMasterID, szEnemyGuildID, pulAcceptTime, pulReadyTime, pulStartTime, pType, pulDuration, pPerson);

	return MakeGuildBattlePacket(pPacketGuildBattleAccept);
}

PVOID AgpmGuild::MakeGuildBattleRejectPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	PACKET_GUILD_BATTLE_REJECT pPacketGuildBattleReject(pnPacketLength, plCID, szGuildID, szMasterID, szEnemyGuildID);
	
	return MakeGuildBattlePacket(pPacketGuildBattleReject);
}

PVOID AgpmGuild::MakeGuildBattleCancelPacket(INT16* pnPacketLength, INT32* plCID, INT8* pcOperation, CHAR* szGuildID, CHAR* szMasterID,
						CHAR* szEnemyGuildID)
{
	PACKET_GUILD_BATTLE_CANCEL pPacketGuildBattleCancel(pnPacketLength, plCID, pcOperation, szGuildID, szMasterID, szEnemyGuildID);
	
	return MakeGuildBattlePacket(pPacketGuildBattleCancel);
}

PVOID AgpmGuild::MakeGuildBattleCancelReqeustPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	INT8 cOperation = AGPMGUILD_PACKET_BATTLE_CANCEL_REQUEST;
	return MakeGuildBattleCancelPacket(pnPacketLength, plCID, &cOperation, szGuildID, szMasterID, szEnemyGuildID);
}

PVOID AgpmGuild::MakeGuildBattleCancelAcceptPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	INT8 cOperation = AGPMGUILD_PACKET_BATTLE_CANCEL_ACCEPT;
	return MakeGuildBattleCancelPacket(pnPacketLength, plCID, &cOperation, szGuildID, szMasterID, szEnemyGuildID);
}

PVOID AgpmGuild::MakeGuildBattleCancelRejectPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	INT8 cOperation = AGPMGUILD_PACKET_BATTLE_CANCEL_REJECT;
	return MakeGuildBattleCancelPacket(pnPacketLength, plCID, &cOperation, szGuildID, szMasterID, szEnemyGuildID);
}

PVOID AgpmGuild::MakeGuildBattleStartPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID, CHAR* szEnemyGuildMasterID,
						UINT32* pulStartTime, UINT32* pulDuration, UINT32* pulCurrentTime)
{
	PACKET_GUILD_BATTLE_START pPacketGuildBattleStart(pnPacketLength, plCID, szGuildID, szEnemyGuildID, szEnemyGuildMasterID, pulStartTime, pulDuration, pulCurrentTime);
	
	return MakeGuildBattlePacket(pPacketGuildBattleStart);
}

PVOID AgpmGuild::MakeGuildBattleUpdateTimePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID,
						UINT32* pulAcceptTime, UINT32* pulReadyTime, UINT32* pulStartTime, UINT32* pulDuration, UINT32* pulCurrentTime)
{
	PACKET_GUILD_BATTLE_UPDATE_TIME pPacketGuildBattleUpdateTime(pnPacketLength, plCID, szGuildID, szEnemyGuildID, pulCurrentTime);
	
	return MakeGuildBattlePacket(pPacketGuildBattleUpdateTime);
}

PVOID AgpmGuild::MakeGuildBattleUpdateScorePacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID, 
						INT32* plMyScore, INT32* plEnemyScore, INT32* plMyUpScore, INT32* plEnemyUpScore)
{
	PACKET_GUILD_BATTLE_UPDATE_SCORE pPacketGuildBattleUpdateScore(pnPacketLength, plCID, szGuildID, szEnemyGuildID, plMyScore, plEnemyScore, plMyUpScore, plEnemyUpScore);
	
	return MakeGuildBattlePacket(pPacketGuildBattleUpdateScore);
						
}

PVOID AgpmGuild::MakeGuildBattleWithdrawPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	PACKET_GUILD_BATTLE_WITHDRAW pPacketGuildBattleWithdraw(pnPacketLength, plCID, szGuildID, szMasterID, szEnemyGuildID);

	return MakeGuildBattlePacket(pPacketGuildBattleWithdraw);
}

PVOID AgpmGuild::MakeGuildBattleMemberPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szCharID, UINT32 ulScore, UINT32 ulKill, UINT32 ulDeath)
{
	if( !szGuildID || !szCharID )	return NULL;

	PVOID pvMemberPacket = m_csBattleMemberPacket.MakePacket( FALSE, pnPacketLength, 0, szGuildID, szCharID, &ulScore, &ulKill, &ulDeath );
	if( !pvMemberPacket )			return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_BATTLE_MEMBER;
	PVOID pGuildPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
										szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
										NULL, NULL, NULL, NULL, NULL,
										NULL, NULL, NULL, pvMemberPacket, NULL, NULL,
										NULL, NULL, NULL);

	m_csBattleMemberPacket.FreePacket( pvMemberPacket );
	return pGuildPacket;
}

PVOID AgpmGuild::MakeGuildBattleMemberListPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, CHAR* szEnemyGuildID)
{
	//적이랑 내꺼랑 두번 보내주자 ^^
	//Packet준비..
	if( !szGuildID )		return NULL;
	AgpdGuild* pMyGuild = GetGuildLock(szGuildID);
	if( !pMyGuild )			return NULL;

	AgpdGuild* pEnemyGuild = GetGuildLock(szEnemyGuildID);
	if( !pEnemyGuild )
	{
		pMyGuild->m_Mutex.Release();
		return NULL;
	}

	UINT32 ulMyCount = pMyGuild->GetBattleMemberCount();
	AgpdGuildMemberBattleInfo* pMyInfo = pMyGuild->GetBattleMemberPack();
	if(!pMyInfo) return NULL;

	UINT32 ulEnemyCount = pEnemyGuild->GetBattleMemberCount();
	AgpdGuildMemberBattleInfo* pEnemyInfo = pEnemyGuild->GetBattleMemberPack();
	if(!pEnemyInfo) return NULL;
	
	pEnemyGuild->m_Mutex.Release();
	pMyGuild->m_Mutex.Release();

	INT16 lMyInfoSize		= (INT16) ( (UINT32) sizeof(AgpdGuildMemberBattleInfo) * ulMyCount		);
	INT16 lEnemyInfoSize	= (INT16) ( (UINT32) sizeof(AgpdGuildMemberBattleInfo) * ulEnemyCount	);
	PVOID pvMemberListPacket = m_csBattleMemberListPacket.MakePacket( FALSE, pnPacketLength, 0, &ulMyCount, pMyInfo, &lMyInfoSize,
																							&ulEnemyCount, pEnemyInfo, &lEnemyInfoSize );
	delete [] pMyInfo;
	delete [] pEnemyInfo;

	if( !pvMemberListPacket )	return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_BATTLE_MEMBER_LIST;
	PVOID pGuildPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, plCID,
										szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
										NULL, NULL, NULL, NULL, NULL,
										NULL, NULL, NULL, NULL, pvMemberListPacket, NULL,
										NULL, NULL, NULL);

	m_csBattleMemberListPacket.FreePacket( pvMemberListPacket );
	return pGuildPacket;
}


/*
	2005.10.19. By SungHoon
	길드 마크 생성 패킷을 생성한다.
*/
PVOID AgpmGuild::MakeGuildBuyGuildMarkPacket(INT16* pnPacketLength, INT32 lCID, CHAR *szGuildID, INT32 *plGuildMarkTID, INT32 *plGuildMarkColor)
{
	INT8 cOperation = AGPMGUILD_PACKET_BUY_GUILDMARK;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						plGuildMarkTID, plGuildMarkColor, NULL/*IsWinner*/);
}

/*
	2005.10.19. By SungHoon
	길드 마크 생성 패킷을 생성한다.
*/
PVOID AgpmGuild::MakeGuildBuyGuildMarkForcePacket(INT16* pnPacketLength, INT32 lCID, CHAR *szGuildID, INT32 *plGuildMarkTID, INT32 *plGuildMarkColor)
{
	INT8 cOperation = AGPMGUILD_PACKET_BUY_GUILDMARK_FORCE ;
	return MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						plGuildMarkTID, plGuildMarkColor,  NULL/*IsWinner*/);
}


/////////////////////////////////////////////////////////////////////
// Relation (Joint/Hotile) Packet
PVOID AgpmGuild::MakeGuildJointRequestPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szJointGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOINT_REQUEST;
	INT32 lCID = 1;

	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, szMasterID,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL/*IsWinner*/);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildJointRejectPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szJointGuildID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szJointGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOINT_REJECT;
	INT32 lCID = 1;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, NULL,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL/*IsWinner*/);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildJointPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szJointGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOINT;
	INT32 lCID = 1;
	INT32 lDate = (INT32)ulDate;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, NULL,
							NULL, NULL, &lDate, NULL, NULL, NULL, NULL, NULL,
							&cRelation, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildJointLeavePacket(INT16* pnPacketLength, INT32 lCID, CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szNewLeader)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szJointGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_JOINT_LEAVE;
	// Master ID 자리에 NewLeader 가 온다. NULL 일 수도 있다.
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, szNewLeader,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL,  NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildHostileRequestPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szHostileGuildID, NULL, NULL, NULL, NULL, NULL, NULL, 
								NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_REQUEST;
	INT32 lCID = 1;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, szMasterID,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildHostileRejectPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szHostileGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_REJECT;
	INT32 lCID = 1;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, NULL,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL,  NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildHostilePacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID, UINT32 ulDate)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szHostileGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE;
	INT32 lCID = 1;
	INT32 lDate = (INT32)ulDate;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, NULL,
							NULL, NULL, &lDate, NULL, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL,  NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildHostileLeaveRequestPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szHostileGuildID, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_LEAVE_REQUEST;
	INT32 lCID = 1;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, szMasterID,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL,  NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildHostileLeaveRejectPacket(INT16* pnPacketLength, CHAR* szGuildID, CHAR* szHostileGuildID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szHostileGuildID, NULL, NULL, NULL, NULL, NULL, NULL, 
								NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_LEAVE_REJECT;
	INT32 lCID = 1;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, NULL,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

PVOID AgpmGuild::MakeGuildHostileLeavePacket(INT16* pnPacketLength, INT32 lCID, CHAR* szGuildID, CHAR* szHostileGuildID)
{
	PVOID pvBattlePacket = m_csBattlePacket.MakePacket(FALSE, pnPacketLength, 0,
								szHostileGuildID, NULL, NULL, NULL, NULL, NULL, NULL, 
								NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL);
	if(!pvBattlePacket)
		return NULL;

	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_LEAVE;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID, szGuildID, szHostileGuildID,
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL, NULL,
							NULL, pvBattlePacket, NULL, NULL, NULL, NULL,
							NULL, NULL, NULL);
	m_csBattlePacket.FreePacket(pvBattlePacket);
	return pvPacket;
}

















//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgpmGuild::SetCallbackGuildCreate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CREATE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildCreateCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CREATE_CHECK, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildCreateEnableCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CREATE_ENABLE_CHECK, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildCreateFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CREATE_FAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildJoinRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOIN_REQUEST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildJoinEnableCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOIN_ENABLE_CHECK, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildJoinFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOIN_FAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildJoinReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOIN_REJECT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildJoin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOIN, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_LEAVE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildLeaveFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_LEAVE_FAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildForcedLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_FORCED_LEAVE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildForcedLeaveFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_FORCED_LEAVE_FAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildDestroyPreProcess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_DESTROY_PREPROCESS, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildDestroy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_DESTROY, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildDestroyFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_DESTROY_FAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildCheckDestroy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CHECK_DESTROY, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildUpdateMaxMemberCount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_UPDATE_MAX_MEMBER_COUNT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackCheckPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CHECK_PASSWORD, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackCharData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_CHAR_DATA, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackUpdateMemberStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_UPDATE_MEMBER_STATUS, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackUpdateNotice(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_UPDATE_NOTICE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_SYSTEM_MESSAGE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackUpdateStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_UPDATE_STATUS, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackUpdateRecord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_UPDATE_RECORD, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_INFO, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattlePerson(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_PERSON, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_REQUEST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_ACCEPT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_REJECT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleCancelRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_CANCEL_REQUEST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleCancelAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_CANCEL_ACCEPT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleCancelReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_CANCEL_REJECT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_START, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleUpdateTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_UPDATE_TIME, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleUpdateScore(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_UPDATE_SCORE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleWithdraw(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_WITHDRAW, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_RESULT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_MEMBER, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleMemberList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_MEMBER_LIST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBattleRound(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BATTLE_ROUND, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackRenameGuildID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_RENAME_GUILDID, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackRenameCharID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_RENAME_CHARID, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildMarkTemplateLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_GUILDMARKTEMPLATE_LOAD, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBuyGuildMark(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BUY_GUILDMARK, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackBuyGuildMarkForce(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_BUY_GUILDMARK_FORCE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGetGuildMarkTexture(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_GET_GUILDMARK_TEXTURE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGetGuildMarkSmallTexture(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_GET_GUILDMARK_SMALL_TEXTURE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackJointRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOINT_REQUEST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackJointReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOINT_REJECT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackJoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOINT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackJointLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOINT_LEAVE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostileRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE_REQUEST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostileReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE_REJECT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostile(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostileLeaveRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE_LEAVE_REQUEST, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostileLeaveReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE_LEAVE_REJECT, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostileLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE_LEAVE, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackJointDetail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOINT_DETAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackHostileDetail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_HOSTILE_DETAIL, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGetJointMark(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_GET_JOINT_MARK, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGetJointColor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_GET_JOINT_COLOR, pfCallback, pClass);
}

BOOL AgpmGuild::SetCallbackGuildAppointmentAsk( ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback( AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ASK , pfCallback , pClass );
}

BOOL AgpmGuild::SetCallbackGuildSuccessionAsk( ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback( AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ASK , pfCallback , pClass );
}

BOOL AgpmGuild::SetCallbackGuildAppointmentSysMsg( ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback( AGPMGUILD_CB_CLASSSOCIETY_SYSTEM_MESSAGE , pfCallback , pClass );
}

BOOL AgpmGuild::SetCallbackGuildMemberRankUpdate( ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback( AGPMGUILD_CB_CLASSSOCIETY_MEMBERRANK_UPDATE , pfCallback , pClass );
}


//////////////////////////////////////////////////////////////////////////
// Debug
BOOL AgpmGuild::PrintGuildInfo()
{
	INT32 lIndex = 0, lIndex2 = 0;
	AgpdGuild** ppcsGuild = NULL;
	AgpdGuild* pcsGuild = NULL;
	AgpdGuildMember* pcsMember = NULL;

	for(ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild; ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!ppcsGuild)
			break;
		pcsGuild = *ppcsGuild;
		if(!pcsGuild)
			break;

		pcsGuild->m_Mutex.WLock();
		if(pcsGuild->m_bRemove)
		{
			pcsGuild->m_Mutex.Release();
			continue;
		}

		fprintf(stdout, "Count : %d, GuildID : %s, MasterID : %s, MaxMemberCount : %d\n", lIndex, pcsGuild->m_szID, pcsGuild->m_szMasterID,
								pcsGuild->m_lMaxMemberCount);

		lIndex2 = 0;
		for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2); ppcsMember;
														ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if(!ppcsMember)
				break;
			pcsMember = *ppcsMember;
			if(!pcsMember)
				break;

			fprintf(stdout, "\t MemberID : %s, Rank : %d, Level : %d\n", pcsMember->m_szID, pcsMember->m_lRank, pcsMember->m_lLevel);
		}

		pcsGuild->m_Mutex.Release();
	}

	return TRUE;
}

/*
	2005.07.08. By SungHoon
	서버에서 해당 페이지의 길드 리스트 패킷을 만든다.
*/
PVOID AgpmGuild::MakeGuildListPacket(INT32 *pCID, INT32 lPage, INT16 *pnPacketLength , CHAR *szGuildID )
{
	AgpdGuild* pcsGuild = NULL;

	INT32 lPageCount = 0;
	INT32 lGuildCount = m_csGuildAdmin.GetObjectCount();
//	해당 페이지 만큼 데이터 Skip
	if (lPage * AGPMGUILD_MAX_VISIBLE_GUILD_LIST >= lGuildCount) return NULL;

	INT16 nCount = 0;
	PVOID pvGuildListItemPacket[ AGPMGUILD_MAX_VISIBLE_GUILD_LIST ] = { 0 };

	{
		AgpdGuild* pcsGuild = NULL;

		AuAutoLock Lock(m_csGuildAdmin.m_ApGuildSortList.m_Mutex);
		if (Lock.Result())
		{
			pcsGuild = m_csGuildAdmin.m_ApGuildSortList.GetHead();

			if (lPage > 0)
			{
				for(;!m_csGuildAdmin.m_ApGuildSortList.IsEnd();
					pcsGuild = m_csGuildAdmin.m_ApGuildSortList.GetNext())
				{
					if(!pcsGuild) break;
					
					AuAutoLock pLock(pcsGuild->m_Mutex);

					if(pcsGuild->m_bRemove) continue;
					if(GetMemberCount(pcsGuild) < 10) continue;	// 길드 정책 변경 - arycoat 2008.02

					lPageCount++;
					if (lPageCount > (lPage * AGPMGUILD_MAX_VISIBLE_GUILD_LIST)) break;
				}
			}
			for(;!m_csGuildAdmin.m_ApGuildSortList.IsEnd();
				pcsGuild = m_csGuildAdmin.m_ApGuildSortList.GetNext())
			{
				if(!pcsGuild) break;

				AuAutoLock pLock(pcsGuild->m_Mutex);

				if(pcsGuild->m_bRemove) continue;
				if(GetMemberCount(pcsGuild) < 10) continue;	// 길드 정책 변경 - arycoat 2008.02

				INT16 nPacketLength = 0;
				pvGuildListItemPacket[nCount] = MakeGuildListPacket(&nPacketLength, pcsGuild);

				nCount++;
				if (pvGuildListItemPacket[nCount]) m_csGuildListItemPacket.FreePacket(pvGuildListItemPacket[nCount]);

				if (nCount >= AGPMGUILD_MAX_VISIBLE_GUILD_LIST) break;
			}
		}
	}
	if (nCount <= 0) return NULL;
	
	PVOID pvGuildListPacket =  m_csGuildListPacket.MakePacket(FALSE, pnPacketLength, 0, 
						&lGuildCount,
						&lPage,
						&nCount,
						pvGuildListItemPacket[0],
						pvGuildListItemPacket[1],
						pvGuildListItemPacket[2],
						pvGuildListItemPacket[3],
						pvGuildListItemPacket[4],
						pvGuildListItemPacket[5],
						pvGuildListItemPacket[6],
						pvGuildListItemPacket[7],
						pvGuildListItemPacket[8],
						pvGuildListItemPacket[9],
						pvGuildListItemPacket[10],
						pvGuildListItemPacket[11],
						pvGuildListItemPacket[12],
						pvGuildListItemPacket[13],
						pvGuildListItemPacket[14],
						pvGuildListItemPacket[15],
						pvGuildListItemPacket[16],
						pvGuildListItemPacket[17],
						pvGuildListItemPacket[18]);

	INT8 cOperation = AGPMGUILD_PACKET_GUILDLIST;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, pCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, pvGuildListPacket,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);

	m_csGuildListPacket.FreePacket(pvGuildListPacket);

	return pvPacket;

}

/*
	2005.07.15. By SungHoon
	서버에서 해당 길드 아이디를 찾아 해당 페이지를 내려준다
*/
PVOID AgpmGuild::MakeGuildListPacket(INT32 *pCID, CHAR *szGuildID, INT16 *pnPacketLength )
{
	AgpdGuild* pcsGuild = GetGuildLock(szGuildID);
	if (!pcsGuild) return NULL;
	INT32 lPage = pcsGuild->m_lPage;
	pcsGuild->m_Mutex.Release();

	return (MakeGuildListPacket(pCID,lPage,pnPacketLength, szGuildID));
}

/*
	2005.07.08. By SungHoon
	길드리스트에 필요한 패킷을 만든다
*/
PVOID AgpmGuild::MakeGuildListPacket(INT16 *pnPacketLength, AgpdGuild *pcsGuild )
{
	INT32 lLevel			= 0;
	INT32 lWinPoint			= pcsGuild->m_lGuildPoint;
	INT32 lMemberCount		= GetMemberCount(pcsGuild);
	INT32 lMaxMemberCount	= pcsGuild->m_lMaxMemberCount;
	INT32 lGuildBattle		= 0;
	INT32 lGuildMarkTID		= pcsGuild->m_lGuildMarkTID;
	INT32 lGuildMarkColor	= pcsGuild->m_lGuildMarkColor;
	INT32 lBRRanking		= pcsGuild->m_lBRRanking;

	return m_csGuildListItemPacket.MakePacket( FALSE, pnPacketLength, 0, 
												pcsGuild->m_szID,
												pcsGuild->m_szMasterID,
												pcsGuild->m_szSubMasterID,
												&lLevel ,
												&lWinPoint,
												&lMemberCount,
												&lMaxMemberCount,
												&lGuildBattle, 
												&lGuildMarkTID,
												&lGuildMarkColor,
												&lBRRanking );
}

/*
	2005.07.08 By SungHoon
	길드 리스트 패킷이 왔을 경우 AgcmUIGuild 에서 처리하기 위해 패킷을 CallBack으로 내려준다.
*/
BOOL AgpmGuild::SetCallbackReceiveGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_GUILDLIST, pfCallback, pClass);
}

/*
	2005.07.28 By SungHoon
	클라이언트에서 길드탈퇴 요청이 들어왔을 경우 불릴 CallBack 함수 등록
*/
BOOL AgpmGuild::SetCallbackReceiveLeaveRequestSelf(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_LEAVE_REQUEST_SELF, pfCallback, pClass);
}

/*
	2005.08.02. By SungHoon
	가입대기 신청이 들어왔을 경우 불릴 CallBack
*/
BOOL AgpmGuild::SetCallbackReceiveJoinRequestSelf(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_JOIN_REQUEST_SELF, pfCallback, pClass);
}


/*
	2005.07.08. By SungHoon
	클라이언트에서 해당 페이지의 길드리스트를 요청한다.
*/
PVOID AgpmGuild::MakeGuildListRequestPacket(INT32 *pCID, INT32 lPage, INT16 *pnPacketLength )
{
	PVOID pvGuildListPacket =  m_csGuildListPacket.MakePacket(FALSE, pnPacketLength, 0, 
						NULL,
						&lPage,		
						NULL,
						NULL,		//	1
						NULL,		//	2
						NULL,		//	3
						NULL,		//	4
						NULL,		//	5
						NULL,		//	6
						NULL,		//	7
						NULL,		//	8
						NULL,		//	9
						NULL,		//	10
						NULL,		//	11
						NULL,		//	12
						NULL,		//	13
						NULL,		//	14
						NULL,		//	15
						NULL,		//	16
						NULL,		//	17
						NULL,		//	18
						NULL		//	19
						);

	INT8 cOperation = AGPMGUILD_PACKET_GUILDLIST;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, pCID,
						NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, pvGuildListPacket,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);

	m_csGuildListPacket.FreePacket(pvGuildListPacket);

	return pvPacket;
}

/*
	2005.07.15. By SungHoon
	클라이언트에서 특정길드를 포함한 페이지의 길드리스트를 요청한다.
*/
PVOID AgpmGuild::MakeGuildListRequestFindPacket(INT32 *pCID, CHAR *szGuildID, INT16 *pnPacketLength )
{
	INT8 cOperation = AGPMGUILD_PACKET_GUILDLIST;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, pCID,
						szGuildID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	return pvPacket;
}

/*
	2005.08.24. By SungHoon
	길드 이름 변경 패킷을 만든다.
*/
PVOID AgpmGuild::MakeGuildRenameGuildIDPacket(INT32 *pCID, CHAR *szGuildID, CHAR *szMemberID, INT16 *pnPacketLength )
{
	INT8 cOperation = AGPMGUILD_PACKET_RENAME_GUILDID;
	PVOID pvPacket = MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, pCID,
						szGuildID, szMemberID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	return pvPacket;
}

/*
	2005.09.05. By SungHoon
	길드원 이름 변경 패킷을 만든다.
*/
PVOID AgpmGuild::MakeGuildRenameCharacterIDPacket(CHAR *szGuildID, CHAR *szOldMemberID, CHAR *szNewMemberID, INT16 *pnPacketLength )
{
	INT8 cOperation = AGPMGUILD_PACKET_RENAME_CHARACTERID;
	PVOID pvMemberPacket = m_csMemberPacket.MakePacket(FALSE, pnPacketLength, 0,
														szNewMemberID, NULL, NULL, NULL, NULL, NULL);

	if(!pvMemberPacket)
		return NULL;

	PVOID pvPacket =  MakeGuildPacket(TRUE, pnPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, NULL,
						szGuildID, szOldMemberID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL, NULL, NULL, NULL,
						pvMemberPacket, NULL, NULL, NULL, NULL, NULL,
						NULL/*GuildMarkTID*/, NULL/*GuildMarkColor*/, NULL/*IsWinner*/);
	m_csMemberPacket.FreePacket(pvMemberPacket);

	if(!pvPacket)
		return NULL;

	return pvPacket;
}

/*
	2005.07.15. By SungHoon
	길드인원 증가에 필요한 아이템과 겔드를 계산한다.
*/
AgpdRequireItemIncreaseMaxMember *AgpmGuild::GetRequireIncreaseMaxMember(INT32 lMaxMember)
{
	for (INT16 i = 0 ; i < AGPMGUILD_MAX_AGPD_REQUIRE_MAX_MEMBER_INCREASE; ++i)
	{
		if (lMaxMember == m_stRequireIncreaseMaxMember[i].m_lMaxMember)
			return &m_stRequireIncreaseMaxMember[i];
	}
	return NULL;
}

/*
	2005.07.15. By SungHoon
	길드인원 증가에 필요한 아이템과 겔드 데이터들을 로딩한다.
*/
BOOL AgpmGuild::ReadRequireItemIncreaseMaxMember( CHAR *szFileName, BOOL bEncryption)
{
	if (!szFileName || !szFileName[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFileName, TRUE, bEncryption))
	{
		OutputDebugString("AgpmGuild::ReadRequireItemIncreaseMaxMember() Error (1) !!!\n");
		return FALSE;
	}

	INT32	lRow	= csExcelTxtLib.GetRow();
	INT32	lCurRow	= 1;
	INT32	lIndex	= 0;

	// 처음도 포함하기 때문에 +1
	if (lRow > (AGPMGUILD_MAX_AGPD_REQUIRE_MAX_MEMBER_INCREASE+1)) 
	{
		OutputDebugString("AgpmGuild::ReadRequireItemIncreaseMaxMember() Error (1) !!!\n");
		return FALSE;
	}

	memset(m_stRequireIncreaseMaxMember, 0, sizeof(m_stRequireIncreaseMaxMember));

	for ( ; ; )
	{
		if (lCurRow >= lRow)
			break;

		CHAR		*szMaxMember	= csExcelTxtLib.GetData(0, lCurRow);
		CHAR		*szGheld		= csExcelTxtLib.GetData(1, lCurRow);
		CHAR		*szSkullName	= csExcelTxtLib.GetData(2, lCurRow);
		CHAR		*szSkullTID		= csExcelTxtLib.GetData(3, lCurRow);
		CHAR		*szSkullCount	= csExcelTxtLib.GetData(4, lCurRow);

		++lCurRow;

		//if (!szMaxMember || !szGheld || !szSkullName || !szSkullTID || !szSkullCount)
		if (!szMaxMember)
			continue;

		memset(m_stRequireIncreaseMaxMember[lIndex].m_szSkullName,0,AGPMITEM_MAX_ITEM_NAME+1);

		m_stRequireIncreaseMaxMember[lIndex].m_lMaxMember	= atol(szMaxMember);

		if (szSkullName)
			strncpy(m_stRequireIncreaseMaxMember[lIndex].m_szSkullName,szSkullName,AGPMITEM_MAX_ITEM_NAME);
		if (szGheld)
			m_stRequireIncreaseMaxMember[lIndex].m_lGheld		= atol(szGheld);
		if (szSkullTID)
			m_stRequireIncreaseMaxMember[lIndex].m_lSkullTID	= atol(szSkullTID);
		if (szSkullCount)
			m_stRequireIncreaseMaxMember[lIndex].m_lSkullCount	= atol(szSkullCount);

		lIndex++;
	}

	return TRUE;
}

/*
	2005.08.01. By SungHoon
	길마가 탈퇴신청한 유저를 탈퇴허가 한다.
*/
BOOL AgpmGuild::OnOperationLeaveAllow(INT32 lCID, CHAR* szGuildID, CHAR* szCharID, BOOL bNotifyMaster, BOOL bAdmin)
{
	if (!szGuildID || !szCharID) return FALSE;

	if(LeaveMember(szGuildID, szCharID, bNotifyMaster, bAdmin))
	{
		// 성공
		EnumCallback(AGPMGUILD_CB_LEAVE, szGuildID, szCharID);
	}
	else
	{
		// 실패 콜백을 불러준다.
		EnumCallback(AGPMGUILD_CB_LEAVE_FAIL, szGuildID, &lCID);
	}

	return TRUE;
}

BOOL AgpmGuild::SetCallbackDestroyGuildCheckTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_DESTROY_GUILD_CHECK_TIME, pfCallback, pClass);
}

/*
	2005.08.24. By SungHoon
	길드 아이디를 변경한다.
*/
BOOL AgpmGuild::OnOperationRenameGuildID(INT32 lCID, CHAR* szGuildID, CHAR* szCharID)
{
	if (!szGuildID || !szCharID) return FALSE;

	PVOID pvBuffer[ 2 ];
	pvBuffer[ 0 ] = (PVOID)szGuildID;
	pvBuffer[ 1 ] = (PVOID)szCharID;
	EnumCallback(AGPMGUILD_CB_RENAME_GUILDID, &lCID, pvBuffer);
	return TRUE;
}

/*
	2005.08.29. By SungHoon
	길드 이름을 변경한다. 
*/
BOOL AgpmGuild::RenameGuildID(AgpdGuild *pcsGuild, CHAR *szGuildID)
{
//	길드 이름 바꾸고 새로 넣는다.
	RemoveGuild(pcsGuild);

	memset(pcsGuild->m_szID, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
	strncpy(pcsGuild->m_szID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	return (AddGuild(pcsGuild));
}

BOOL AgpmGuild::IsDuplicatedCharacterOfMigration(CHAR *pszChar)
{
	CHAR *psz = pszChar;
	if (NULL == psz)
		return FALSE;

	while (_T('\0') != *psz)
	{
		if (_T('@') == *psz)
			return TRUE;
		psz++;
	}
	
	return FALSE;
}

/*
	2005.09.05. By SungHoon
	길드원 아이디를 변경한다.
*/
BOOL AgpmGuild::OnOperationRenameCharacterID(CHAR* szGuildID, CHAR* szOldID, CHAR *szNewID)
{
	if (!szGuildID || !szOldID || !szNewID) return FALSE;

	PVOID pvBuffer[ 2 ];
	pvBuffer[ 0 ] = (PVOID)szOldID;
	pvBuffer[ 1 ] = (PVOID)szNewID;
	EnumCallback(AGPMGUILD_CB_RENAME_CHARID, szGuildID, pvBuffer);
	return TRUE;
}


/*
	2005.09.06. By SungHoon
	길드원의 아이디를 바꾼다.
*/
BOOL AgpmGuild::RenameCharID(AgpdGuild *pcsGuild, CHAR *szOldID, CHAR *szNewID)
{
	if (!strcmp(pcsGuild->m_szMasterID, szOldID))		//	길마이면 길마이름 바꾸고
	{
		memset(pcsGuild->m_szMasterID, 0, AGPACHARACTER_MAX_ID_STRING+1);
		strncpy(pcsGuild->m_szMasterID, szNewID, AGPACHARACTER_MAX_ID_STRING);
	}
	else if (!strcmp(pcsGuild->m_szSubMasterID, szOldID))		//	부길마이면 부길마이름 바꾸고
	{
		memset(pcsGuild->m_szSubMasterID, 0, AGPACHARACTER_MAX_ID_STRING+1);
		strncpy(pcsGuild->m_szSubMasterID, szNewID, AGPACHARACTER_MAX_ID_STRING);
	}

	AgpdGuildMember *pcsGuildMember = GetMember(pcsGuild, szOldID);
	if (pcsGuildMember)
	{
		RemoveMember(pcsGuild, pcsGuildMember);
		memset(pcsGuildMember->m_szID, 0, AGPACHARACTER_MAX_ID_STRING+1);
		strncpy(pcsGuildMember->m_szID, szNewID, AGPACHARACTER_MAX_ID_STRING);

		AddMember(pcsGuild, pcsGuildMember);

		return TRUE;
	}

	//	길드 가입 리스트 확인한다.
	AgpdGuildRequestMember **ppcsRequestMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObject(szOldID);
	if (!ppcsRequestMember) return FALSE;
	AgpdGuildRequestMember *pcsRequestMember = *ppcsRequestMember;

	if (!pcsRequestMember)	return FALSE;

	pcsGuild->m_pGuildJoinList->RemoveObject(szOldID);

	memset(pcsRequestMember->m_szMemberID, 0, AGPACHARACTER_MAX_ID_STRING+1);
	strncpy(pcsRequestMember->m_szMemberID, szNewID, AGPACHARACTER_MAX_ID_STRING);

	pcsGuild->m_pGuildJoinList->AddObject((PVOID)&pcsRequestMember, pcsRequestMember->m_szMemberID);


	return TRUE;
}


/*
	2005.10.05. By SungHoon
	길드 마크 템플리트를 new 하고 초기화한다.
*/
AgpdGuildMarkTemplate *AgpmGuild::CreateGuildMarkTemplate()
{
	AgpdGuildMarkTemplate* pGuildMarkTemplate = (AgpdGuildMarkTemplate *)CreateModuleData(AGPMGUILD_DATA_TYPE_GUILD_MARK_TEMPLATE);
	if( pGuildMarkTemplate )
	{
		pGuildMarkTemplate->m_pcsGridItem = m_pagpmGrid->CreateGridItem();
		if( pGuildMarkTemplate->m_pcsGridItem )
		{
			pGuildMarkTemplate->m_pcsGridItem->m_eType = AGPDGRID_ITEM_TYPE_GUILDMARK;
			pGuildMarkTemplate->m_pcsGridItem->SetParentBase((ApBase*) pGuildMarkTemplate);
		}
	}

	return pGuildMarkTemplate;
}

/*
	2005.10.05. By SungHoon
	다 사용한 길드 마크 템플리트를 delete 한다.
*/
VOID AgpmGuild::DestroyGuildMarkTemplate(AgpdGuildMarkTemplate *pGuildMarkTemplate)
{
	if( !pGuildMarkTemplate ) return;

	if( pGuildMarkTemplate->m_szName )
	{
		delete []pGuildMarkTemplate->m_szName;
		pGuildMarkTemplate->m_szName = NULL;
	}

	if( pGuildMarkTemplate->m_pcsGridItem )
	{
		m_pagpmGrid->DeleteGridItem(pGuildMarkTemplate->m_pcsGridItem);
		pGuildMarkTemplate->m_pcsGridItem = NULL;
	}

	DestroyModuleData(pGuildMarkTemplate, AGPMGUILD_DATA_TYPE_GUILD_MARK_TEMPLATE);
}


/*
	2005.10.05. By SungHoon
	길드 마크 템플릿을 Add한다.
*/
AgpdGuildMarkTemplate* AgpmGuild::AddGuildMarkTemplate(CHAR *szName, INT32 lTypeCode, INT32 lTID, INT32 lIndex, INT32 lGheld, INT32 lSkullTID, INT32 lSkullCount)
{
	if( !lTID )		return NULL;

	AgpdGuildMarkTemplate* pcsTemplate = CreateGuildMarkTemplate();
	if (!pcsTemplate)
	{
		TRACE( "AgpmSkill::AddSkillTemplate CreateSkillTemplate 실패\n" );
		return NULL;
	}

	pcsTemplate->m_szName = new CHAR[strlen(szName)+1];
	strcpy(pcsTemplate->m_szName, szName);

	pcsTemplate->m_lTypeCode = lTypeCode;
	pcsTemplate->m_lTID = lTID;
	pcsTemplate->m_lIndex = lIndex;
	pcsTemplate->m_lGheld = lGheld;
	pcsTemplate->m_lSkullTID = lSkullTID;
	pcsTemplate->m_lSkullCount = lSkullCount;

	switch(lTypeCode)
	{
	case AGPMGUILD_ID_MARK_BOTTOM : m_lGuildMarkBottomCount++;		break;
	case AGPMGUILD_ID_MARK_PATTERN : m_lGuildMarkPatternCount++;	break;
	case AGPMGUILD_ID_MARK_SYMBOL : m_lGuildMarkSymbolCount++;		break;
	case AGPMGUILD_ID_MARK_COLOR : m_lGuildMarkColorCount++;		break;
	}

	if ( !m_csGuildMarkTemplateList.AddObject( (PVOID)&pcsTemplate, pcsTemplate->m_lTID ) )
	{
		TRACE( "AgpmGuild::AddGuildMarkTemplate 길드 마크 추가 실패\n" );
		DestroyGuildMarkTemplate(pcsTemplate);
		return NULL;
	}

	return pcsTemplate;
}

/*
	2005.10.05. By SungHoon
	m_csGuildMarkTemplateList에서 해당 길드 ID를 가져온다.
*/
AgpdGuildMarkTemplate *AgpmGuild::GetGuildMarkTemplate(INT32 lGuildTemplateID)
{
	AgpdGuildMarkTemplate **ppGuildMarkTemplate = (AgpdGuildMarkTemplate **)m_csGuildMarkTemplateList.GetObject(lGuildTemplateID);
	return ppGuildMarkTemplate ? *ppGuildMarkTemplate :	NULL;
}

/*
	2005.10.06. By SungHoon
	길드 마크 템플릿 파일을 읽어온다.
*/
BOOL AgpmGuild::ReadGuildMarkTemplate( CHAR *szFileName, BOOL bEncryption)
{
	if (!szFileName || !szFileName[0])
		return FALSE;

	//	2005.10.05. By SungHoon
	DeleteAllGuildMark();
	if (!m_csGuildMarkTemplateList.InitializeObject(sizeof(AgpdGuildMarkTemplate*), m_csGuildMarkTemplateList.GetCount()))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFileName, TRUE, bEncryption))
	{
		OutputDebugString("AgpmGuild::ReadGuildMarkTemplate() Error (1) !!!\n");
		return FALSE;
	}

	INT32	lRow	= csExcelTxtLib.GetRow();
	INT32	lCurRow	= 1;

	CHAR szTypeName[4][_MAX_PATH] = { AGPMGUILD_MARK_BOTTOM, AGPMGUILD_MARK_PATTERN, AGPMGUILD_MARK_SYMBOL, AGPMGUILD_MARK_COLOR};

	for ( ; ; )
	{
		if (lCurRow >= lRow)
			break;

		CHAR		*szType			= csExcelTxtLib.GetData(0, lCurRow);
		CHAR		*szTID			= csExcelTxtLib.GetData(1, lCurRow);
		CHAR		*szName			= csExcelTxtLib.GetData(2, lCurRow);
		CHAR		*szPrice		= csExcelTxtLib.GetData(3, lCurRow);
		CHAR		*szSkullTID		= csExcelTxtLib.GetData(4, lCurRow);
		CHAR		*szSkullCount	= csExcelTxtLib.GetData(5, lCurRow);

		++lCurRow;

		if (!szType || !szTID || !szName || !szPrice || !szSkullTID || !szSkullCount)
			continue;

		INT32 lTID = atol(szTID);
		INT32 lSkullTID = atol(szSkullTID);
		if (!strncmp(AGPMGUILD_MARK_COLOR, szType, strlen( AGPMGUILD_MARK_COLOR )))
		{
			CHAR *szEndString;
			lSkullTID = strtoul(szSkullTID, &szEndString, 16);
		}
		for ( int i = 0 ; i < 4; i++)
		{
			if (!strncmp(szTypeName[ i ], szType, strlen( szTypeName[ i ])))
			{
				lTID = lTID << ( i * 8 );
				AgpdGuildMarkTemplate *pTemplate = AddGuildMarkTemplate(szName, i + 1, lTID, atol(szTID), atol(szPrice), lSkullTID, atol(szSkullCount));
				EnumCallback(AGPMGUILD_CB_GUILDMARKTEMPLATE_LOAD, pTemplate, NULL);
				break;
			}
		}
	}

	return TRUE;
}

BOOL AgpmGuild::ReReadGuildMarkTemplate( CHAR *szFileName, BOOL bEncryption)
{
	if (!szFileName || !szFileName[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFileName, TRUE, bEncryption))
	{
		OutputDebugString("AgpmGuild::ReadGuildMarkTemplate() Error (1) !!!\n");
		return FALSE;
	}

	INT32	lRow	= csExcelTxtLib.GetRow();
	INT32	lCurRow	= 1;

	CHAR szTypeName[4][_MAX_PATH] = { AGPMGUILD_MARK_BOTTOM, AGPMGUILD_MARK_PATTERN, AGPMGUILD_MARK_SYMBOL, AGPMGUILD_MARK_COLOR};

	for ( ; ; )
	{
		if (lCurRow >= lRow)
			break;

		CHAR		*szType			= csExcelTxtLib.GetData(0, lCurRow);
		CHAR		*szTID			= csExcelTxtLib.GetData(1, lCurRow);
		CHAR		*szName			= csExcelTxtLib.GetData(2, lCurRow);
		CHAR		*szPrice		= csExcelTxtLib.GetData(3, lCurRow);
		CHAR		*szSkullTID		= csExcelTxtLib.GetData(4, lCurRow);
		CHAR		*szSkullCount	= csExcelTxtLib.GetData(5, lCurRow);

		++lCurRow;

		if (!szType || !szTID || !szName || !szPrice || !szSkullTID || !szSkullCount)
			continue;

		INT32	lTID	= atol(szTID);
		INT32 lSkullTID = atol(szSkullTID);

		for ( int i = 0 ; i < 4; i++)
		{
			if (!strncmp(szTypeName[ i ], szType, strlen( szTypeName[ i ])))
			{
				lTID = lTID << ( i * 8 );
				break;
			}
		}

		if (!strncmp(AGPMGUILD_MARK_COLOR, szType, strlen( AGPMGUILD_MARK_COLOR )))
		{
			CHAR *szEndString;
			lSkullTID = strtoul(szSkullTID, &szEndString, 16);
		}

		AgpdGuildMarkTemplate	*pTemplate	= GetGuildMarkTemplate(lTID);

		if (pTemplate)
		{
			pTemplate->m_lSkullCount	= atol(szSkullCount);
			pTemplate->m_lSkullTID		= lSkullTID;
			pTemplate->m_lGheld			= atol(szPrice);
		}
	}

	return TRUE;
}

/*
	2005.10.07. By SungHoon
	길드 마크 템플릿을 메모리에서 제거한다.
*/
VOID AgpmGuild::DeleteAllGuildMark()
{
	AgpdGuildMarkTemplate **ppTemplate = NULL;
	INT32 lIndex = 0;
	for(ppTemplate = (AgpdGuildMarkTemplate**)m_csGuildMarkTemplateList.GetObjectSequence(&lIndex); ppTemplate;
		ppTemplate = (AgpdGuildMarkTemplate**)m_csGuildMarkTemplateList.GetObjectSequence(&lIndex))
	{
		if(!*ppTemplate)
			continue;

		AgpdGuildMarkTemplate *pTemplate = *(ppTemplate);
		DestroyGuildMarkTemplate(pTemplate);
	}
	m_csGuildMarkTemplateList.RemoveObjectAll();

	m_lGuildMarkBottomCount		= 0;
	m_lGuildMarkPatternCount	= 0;
	m_lGuildMarkSymbolCount		= 0;
	m_lGuildMarkColorCount		= 0;
}

/*
	2005.10.12. By SungHoon
	길드 마크 그리드  최대 아이템갯수를 가져온다.
*/
INT32 AgpmGuild::GetMaxGuildMarkGridItem()
{
	return max( m_lGuildMarkSymbolCount, max( m_lGuildMarkBottomCount, m_lGuildMarkPatternCount ) );
}

/*
	2005.10.12. By SungHoon
	길드 마크 색그리드 아이템의 갯수를 가져온다.
*/
INT32 AgpmGuild::GetMaxGuildMarkColorGridItem()
{
	return m_lGuildMarkColorCount;
}

/*
	2005.10.13. By SungHoon
	특정 길드마크 ID를 비트연산으로 셋팅한다.
*/
INT32 AgpmGuild::SetGuildMarkID(INT32 lTypeCode, INT32 lID, INT32 lSettingID)
{
	INT32 lFlag = 0xFFFFFFFF;
	switch( lTypeCode )
	{
	case AGPMGUILD_ID_MARK_BOTTOM : lFlag = 0xFFFFFF00;		break;
	case AGPMGUILD_ID_MARK_PATTERN : lFlag = 0xFFFF00FF;	break;
	case AGPMGUILD_ID_MARK_SYMBOL : lFlag = 0xFF00FFFF;		break;
	}

	return ( lFlag & lID ) | lSettingID;
}

/*
	2005.10.13. By SungHoon
	길드마크중 Type에 해당하는 ID를 비트연산하여 반환한다.
*/
INT32 AgpmGuild::GetGuildMarkID(INT32 lTypeCode, INT32 lID)
{
	INT32 lFlag = 0xFFFFFFFF;
	switch( lTypeCode )
	{
	case AGPMGUILD_ID_MARK_BOTTOM : return (lID & 0x000000FF);
	case AGPMGUILD_ID_MARK_PATTERN : return (lID & 0x0000FF00);
	case AGPMGUILD_ID_MARK_SYMBOL : return (lID & 0x00FF0000);
	}
	return 0;
}

/*
	2005.10.13. By SungHoon
	특정 길드마크 ID를 비트연산으로 셋팅한다.
*/
INT32 AgpmGuild::SetGuildMarkColor(INT32 lTypeCode, INT32 lID, INT32 lSettingID)
{
	INT32 lResultID = lID;
	INT32 lFlag = 0xFFFFFFFF;
	switch( lTypeCode )
	{
	case AGPMGUILD_ID_MARK_BOTTOM :
		lResultID = ( 0xFFFFFF00 & lID ) | lSettingID;
		break;
	case AGPMGUILD_ID_MARK_PATTERN :
		lResultID = ( 0xFFFF00FF & lID ) | (lSettingID << 8 );
		break;
	case AGPMGUILD_ID_MARK_SYMBOL :
		lResultID = ( 0xFF00FFFF & lID ) | (lSettingID << 16 );
		break;
	}

	return lResultID;
}

/*
	2005.10.13. By SungHoon
	길드마크중 Type에 해당하는 TID를 비트연산하여 반환한다.
*/
INT32 AgpmGuild::GetGuildMarkColorID(INT32 lTypeCode, INT32 lID)
{
	switch( lTypeCode )
	{
	case AGPMGUILD_ID_MARK_BOTTOM :
		return ( (lID & 0x000000FF) << 24 ) & 0xFF000000;
	case AGPMGUILD_ID_MARK_PATTERN :
		return ( (lID & 0x0000FF00) << 16 ) & 0xFF000000;
	case AGPMGUILD_ID_MARK_SYMBOL :
		return ( (lID & 0x00FF0000) << 8 ) & 0xFF000000;
	}
	return 0;
}

/*
	2005.10.13. By SungHoon
	길드마크중 Type에 해당하는 Color값을 반환한다.
*/
INT32 AgpmGuild::GetGuildMarkColor(INT32 lTypeCode, INT32 lID)
{
	AgpdGuildMarkTemplate *pColorTemplate = GetGuildMarkTemplate(GetGuildMarkColorID(lTypeCode, lID));
	return pColorTemplate ? pColorTemplate->m_lSkullTID : 0xFFFFFFFF;
}

/*
	2005.10.18. By SungHoon
	길드 마크 템플릿을 얻어온다.
 */
AgpdGuildMarkTemplate* AgpmGuild::GetGuildMarkTemplate(INT32 lTypeCode, INT32 lGuildMarkTemplateID)
{
	return (GetGuildMarkTemplate(GetGuildMarkID(lTypeCode, lGuildMarkTemplateID)));
}

/*
	2005.10.18. By SungHoon
	AgpdCharacter 가 소속된 길드 마크 Template를 가져온다.
	To 대영
*/
AgpdGuildMarkTemplate* AgpmGuild::GetGuildMarkTemplate(INT32 lTypeCode, AgpdCharacter *pcsCharacter)
{
	return (GetGuildMarkTemplate(lTypeCode, GetGuildMarkID(pcsCharacter)));
}

/*
	2005.10.18. By SungHoon
	AgpdCharacter 가 소속된 길드 마크 Color를 가져온다.
	To 대영
*/
INT32 AgpmGuild::GetGuildMarkColor(INT32 lTypeCode, AgpdCharacter *pcsCharacter)
{
	return (GetGuildMarkColor(lTypeCode, GetGuildMarkColor(pcsCharacter)));
}

INT32 AgpmGuild::GetGuildMarkID(AgpdCharacter *pcsCharacter)
{
	if( !pcsCharacter )		return 0;
	
	AgpdGuildADChar* pAttachedData = GetADCharacter(pcsCharacter);
	return pAttachedData ? pAttachedData->m_lGuildMarkTID : 0;
}

INT32 AgpmGuild::GetGuildMarkColor(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter) return 0xFFFFFFFF;

	AgpdGuildADChar* pAttachedData = GetADCharacter(pcsCharacter);
	return pAttachedData ? pAttachedData->m_lGuildMarkColor : 0xFFFFFFFF;
}

INT32 AgpmGuild::GetJointGuildMarkID(CHAR* szGuildID)
{
	// 콜백을 타고 값을 얻어온다.
	INT32 lTID = 0;
	EnumCallback(AGPMGUILD_CB_GET_JOINT_MARK, szGuildID, &lTID);
	return lTID;
}

INT32 AgpmGuild::GetJointGuildMarkColor(CHAR* szGuildID)
{
	// 콜백을 타고 값을 얻어온다.
	INT32 lColor = 0xFFFFFFFF;
	EnumCallback(AGPMGUILD_CB_GET_JOINT_COLOR, szGuildID, &lColor);
	return lColor;
}


BOOL AgpmGuild::OnOperationBuyGuildMark(INT lCID, CHAR* szGuildID, INT32 lGuildMark, INT32 lGuildColor)
{
	PVOID ppBuffer[3];
	ppBuffer[0] = &lCID;
	ppBuffer[1] = &lGuildMark;
	ppBuffer[2] = &lGuildColor;

	EnumCallback(AGPMGUILD_CB_BUY_GUILDMARK, ppBuffer, szGuildID);

	return TRUE;
}

BOOL AgpmGuild::OnOperationBuyGuildMarkForce(INT lCID, CHAR* szGuildID, INT32 lGuildMark, INT32 lGuildColor)
{
	PVOID ppBuffer[3];
	ppBuffer[0] = &lCID;
	ppBuffer[1] = &lGuildMark;
	ppBuffer[2] = &lGuildColor;

	EnumCallback(AGPMGUILD_CB_BUY_GUILDMARK_FORCE, ppBuffer, szGuildID);

	return TRUE;
}

VOID *AgpmGuild::GetGuildMarkTexture(INT32 lTypeCode, INT32 lGuildMarkTemplateID, BOOL bSmallTexture )
{
	AgpdGuildMarkTemplate* pGuildMarkTemplate = GetGuildMarkTemplate(lTypeCode, lGuildMarkTemplateID);
	if( !pGuildMarkTemplate )		return NULL;

	VOID *pTexture = NULL;
	UINT32 ulCallbackID = bSmallTexture ? AGPMGUILD_CB_GET_GUILDMARK_SMALL_TEXTURE : AGPMGUILD_CB_GET_GUILDMARK_TEXTURE;
	EnumCallback( ulCallbackID, pGuildMarkTemplate, &pTexture );

	return pTexture;
}

/*
	2005.10.19. By SungHoon
	길드 마크의 텍스쳐를 가져온다.
	To 대영
*/
VOID *AgpmGuild::GetGuildMarkTexture(INT32 lTypeCode, AgpdCharacter *pcsCharacter, BOOL bSmallTexture )
{
	AgpdGuildMarkTemplate* pGuildMarkTemplate = GetGuildMarkTemplate(lTypeCode, pcsCharacter);
	if( !pGuildMarkTemplate )		return NULL;

	VOID *pTexture = NULL;
	UINT32 ulCallbackID = bSmallTexture ? AGPMGUILD_CB_GET_GUILDMARK_SMALL_TEXTURE : AGPMGUILD_CB_GET_GUILDMARK_TEXTURE;
	EnumCallback( ulCallbackID, pGuildMarkTemplate, &pTexture );

	return pTexture;
}

/*
	2005.10.21. By SungHoon
	길드 마크가 구입가능한지 알아보고 제거해야할 경우라면 제거한다.
*/
INT32 AgpmGuild::IsEnableBuyGuildMark(INT32 lGuildMarkTID, AgpdCharacter *pcsCharacter)
{
	AgpdGuildMarkTemplate *pGuildMarkTemplate[3];
	pGuildMarkTemplate[0] = GetGuildMarkTemplate(AGPMGUILD_ID_MARK_BOTTOM, lGuildMarkTID);
	pGuildMarkTemplate[1] = GetGuildMarkTemplate(AGPMGUILD_ID_MARK_PATTERN, lGuildMarkTID);
	pGuildMarkTemplate[2] = GetGuildMarkTemplate(AGPMGUILD_ID_MARK_SYMBOL, lGuildMarkTID);

	INT32 lGheld = 0;

	for ( int i = 0 ; i < 3; i++)
	{
		if( !pGuildMarkTemplate[i] )	continue;

		lGheld += pGuildMarkTemplate[i]->m_lGheld;
		
		//길드 정책 변경 - arycoat 2008.02
		/*if( pGuildMarkTemplate[i]->m_lSkullCount > 0 )
		{
			AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter,pGuildMarkTemplate[i]->m_lSkullTID);
			if( !pcsItem )	
				return AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NO_EXIST_SKULL;		// 해골이 없다
			if( pcsItem->m_nCount < pGuildMarkTemplate[i]->m_lSkullCount )
				return AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NOT_ENOUGHT_SKULL;	// 해골이 부족하다.
		}*/
	}

	return pcsCharacter->m_llMoney < lGheld ? AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NOT_ENOUGHT_GHELD : -1;
}

/*
	2005.10.20. By SungHoon
	중복되는 길드 마크가 있는지 검색한다.	안했으면 좋겠다.
*/
BOOL AgpmGuild::IsDuplicateGuildMark(INT32 lGuildMarkTID)
{
	AgpdGuild **ppcsGuild = NULL;
	INT32 lIndex = 0;

	for(ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild;
			ppcsGuild = (AgpdGuild**)m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!ppcsGuild)		continue;
		AgpdGuild *pcsGuild = *ppcsGuild;
		if(!pcsGuild)		continue;

		pcsGuild->m_Mutex.WLock();
		if(pcsGuild->m_bRemove)
		{
			pcsGuild->m_Mutex.Release();
			continue;
		}
		INT32 lCheckMarkTID = pcsGuild->m_lGuildMarkTID;
		pcsGuild->m_Mutex.Release();

		if (lCheckMarkTID == lGuildMarkTID)
			return TRUE;
	}
	return FALSE;
}

BOOL AgpmGuild::CBChangeItemOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmGuild	*pThis			= (AgpmGuild *)	pClass;
	AgpdItem	*pcsItem		= (AgpdItem *)	pData;
	INT32		*plPrevOwner	= (INT32 *)		pCustData;

	if (pcsItem->m_pcsItemTemplate->m_nType != AGPMITEM_TYPE_OTHER ||
		((AgpdItemTemplateOther *) pcsItem->m_pcsItemTemplate)->m_eOtherItemType != AGPMITEM_OTHER_TYPE_ARCHON_SCROLL)
		return TRUE;

	AgpdGuild	*pcsGuild	= pThis->GetGuildLock(pcsItem->m_pcsCharacter);
	if (pcsGuild && pThis->IsMaster(pcsGuild, pcsItem->m_pcsCharacter->m_szID))
	{
		++pcsGuild->m_ulArchonScrollCount;
	}

	if (pcsGuild)
		pcsGuild->m_Mutex.Release();

	AgpdCharacter	*pcsPrevOwner	= pThis->m_pagpmCharacter->GetCharacterLock(*plPrevOwner);
	if (pcsPrevOwner)
	{
		pcsGuild	= pThis->GetGuildLock(pThis->m_pagpmCharacter->GetCharacter(*plPrevOwner));
		if (pcsGuild && pThis->IsMaster(pcsGuild, pcsPrevOwner->m_szID))
		{
			--pcsGuild->m_ulArchonScrollCount;
		}

		if (pcsGuild)
			pcsGuild->m_Mutex.Release();

		pcsPrevOwner->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgpmGuild::OnOperationJointRequest(CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szJointGuildID || !szMasterID)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = szJointGuildID;
	pvBuffer[1] = szMasterID;

	EnumCallback(AGPMGUILD_CB_JOINT_REQUEST, szGuildID, pvBuffer);
	return TRUE;
}

BOOL AgpmGuild::OnOperationJointReject(CHAR* szGuildID, CHAR* szJointGuildID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_JOINT_REJECT, szGuildID, szJointGuildID);
	return TRUE;
}

BOOL AgpmGuild::OnOperationJoint(CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	AgpdGuildRelationUnit stUnit;
	memset(&stUnit, 0, sizeof(stUnit));

	_tcsncpy(stUnit.m_szGuildID, szJointGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	stUnit.m_ulDate = ulDate;
	stUnit.m_cRelation = cRelation;

	EnumCallback(AGPMGUILD_CB_JOINT, szGuildID, &stUnit);
	return TRUE;
}

BOOL AgpmGuild::OnOperationJointLeave(INT32 lCID, CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szNewLeader)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	PVOID pvBuffer[3];
	pvBuffer[0] = szGuildID;
	pvBuffer[1] = szJointGuildID;
	pvBuffer[2] = szNewLeader;

	EnumCallback(AGPMGUILD_CB_JOINT_LEAVE, &lCID, pvBuffer);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostileRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szHostileGuildID || !szMasterID)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = szHostileGuildID;
	pvBuffer[1] = szMasterID;

	EnumCallback(AGPMGUILD_CB_HOSTILE_REQUEST, szGuildID, pvBuffer);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostileReject(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_HOSTILE_REJECT, szGuildID, szHostileGuildID);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostile(CHAR* szGuildID, CHAR* szHostileGuildID, UINT32 ulDate)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	AgpdGuildRelationUnit stUnit;
	memset(&stUnit, 0, sizeof(stUnit));

	_tcsncpy(stUnit.m_szGuildID, szHostileGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	stUnit.m_ulDate = ulDate;
	stUnit.m_cRelation = AGPMGUILD_RELATION_HOSTILE;

	EnumCallback(AGPMGUILD_CB_HOSTILE, szGuildID, &stUnit);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostileLeaveRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szHostileGuildID || !szMasterID)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = szHostileGuildID;
	pvBuffer[1] = szMasterID;

	EnumCallback(AGPMGUILD_CB_HOSTILE_LEAVE_REQUEST, szGuildID, pvBuffer);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostileLeaveReject(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_HOSTILE_LEAVE_REJECT, szGuildID, szHostileGuildID);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostileLeave(INT32 lCID, CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = szGuildID;
	pvBuffer[1] = szHostileGuildID;

	EnumCallback(AGPMGUILD_CB_HOSTILE_LEAVE, &lCID, pvBuffer);
	return TRUE;
}

BOOL AgpmGuild::OnOperationJointDetail(PVOID pvGuildListPacket)
{
	if(!pvGuildListPacket)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_JOINT_DETAIL, pvGuildListPacket, NULL);
	return TRUE;
}

BOOL AgpmGuild::OnOperationHostileDetail(PVOID pvGuildListPacket)
{
	if(!pvGuildListPacket)
		return FALSE;

	EnumCallback(AGPMGUILD_CB_HOSTILE_DETAIL, pvGuildListPacket, NULL);
	return TRUE;
}

void AgpmGuild::OnOperationWorldChampionship(PVOID pvPacket)
{
	PACKET_GUILD_WORLDCHAMPIONSHIP* pPacket = (PACKET_GUILD_WORLDCHAMPIONSHIP*)pvPacket;

	switch(pPacket->pcOperation2)
	{
		case AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_REQUEST:
			{
				EnumCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_REQUEST, pvPacket, NULL);
			} break;
		case AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_ENTER:
			{
				EnumCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_ENTER, pvPacket, NULL);
			} break;
		case AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_RESULT:
			{
				EnumCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_RESULT, pvPacket, NULL);				
			} break;
	}
}

void AgpmGuild::OnOperationClassSocietySystem(PVOID pvPacket)
{
	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST* pPacket = (PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST*)pvPacket;

	switch(pPacket->pcOperation2)
	{
		case AGPMGUILD_CLASS_SOCIETY_SYSTEM_MESSAGE:
			{
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_SYSTEM_MESSAGE, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST:
			{
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_REQUEST, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_ASK:
			{
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ASK, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER:
			{
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ANSWER, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_SUCCESSION_REQUEST:
			{
				// To Do : CallBack Request Succession Guild
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_REQUEST, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_SUCCESSION_ASK:
			{
				// To Do : CallBack Ask Succession Guild
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ASK, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_SUCCESSION_ANSWER:
			{
				// To Do : CallBack Answer Succession Guild
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ANSWER, pvPacket, NULL);
			} break;
		case AGPMGUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK:
			{
				// To Do : CallBack Update Guild Info
				EnumCallback(AGPMGUILD_CB_CLASSSOCIETY_MEMBERRANK_UPDATE , pvPacket , NULL );
			}
	}
}
