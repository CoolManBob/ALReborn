// AgcmGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.

#include "AgcmGuild.h"
#include "ApMemoryTracker.h"
#include "AgcmUIGuild.h"
#include "AgcmTextBoardMng.h"
#include "AgppGuild.h"
#include "AuStrTable.h"

//----------------------- AgcdGuildMember -------------------------
bool AgcdGuildMember::LevelLess( const AgcdGuildMember& lhs, const AgcdGuildMember& rhs )
{
	return lhs.m_lLevel > rhs.m_lLevel;
}

//----------------------- AgcmGuild -------------------------
AgcmGuild::AgcmGuild()
{
	SetModuleName( "AgcmGuild" );
	SetModuleType( APMODULE_TYPE_CLIENT );

	EnableIdle( FALSE );

	m_pcsAgpmFactors		= NULL;
	m_pcsAgpmGuild			= NULL;

	m_pcsAgcmCharacter		= NULL;
	m_pcsAgcmUIControl		= NULL;
	m_pcsAgcmResourceLoader	= NULL;

	m_szTexturePath[0]		= 0;
}

AgcmGuild::~AgcmGuild()
{
}

BOOL AgcmGuild::OnAddModule()
{
	m_pcsAgpmGuild				= (AgpmGuild*)GetModule("AgpmGuild");
	m_pcsAgpmFactors			= (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgcmCharacter			= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgcmUIControl			= (AgcmUIControl*)GetModule("AgcmUIControl");
	m_pcsAgcmResourceLoader		= (AgcmResourceLoader*)GetModule("AgcmResourceLoader");
	m_pcsAgcmTextBoardMng		= (AgcmTextBoardMng*)GetModule("AgcmTextBoardMng");

	if( !m_pcsAgpmFactors || !m_pcsAgpmGuild || !m_pcsAgpmCharacter || !m_pcsAgcmCharacter || 
		!m_pcsAgcmUIControl || !m_pcsAgcmResourceLoader || !m_pcsAgcmTextBoardMng)
		return FALSE;

	m_nIndexAttachDataGuildMarkTemplate = m_pcsAgpmGuild->AttachGuildMarkTemplateData(this, sizeof(AgcdGuildMarkTemplate), ConAgcdGuildMarkTemplate, DesAgcdGuildMarkTemplate);
	if(m_nIndexAttachDataGuildMarkTemplate < 0)
		return FALSE;

	//if(!m_pcsAgpmCharacter->SetCallbackCheckValidNormalAttack(CBCheckGuildAttackable, this))
	//	return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackInitCharacter(CBInitCharacter, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildUpdateMaxMemberCount(CBGuildUpdateMaxMemberCount, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackRenameGuildID(CBGuildRenameGuildID,this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackRenameCharID(CBGuildRenameCharID,this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildMarkTemplateLoad(CBGuildMarktemplateLoad,this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackGetGuildMarkTexture(CBGetGuildMarkTexture,this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGetGuildMarkSmallTexture(CBGetGuildMarkSmallTexture,this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_RESULT, CB_OnResultWorldChampionShipRequest, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmGuild::OnInit()
{
	return TRUE;
}

BOOL AgcmGuild::OnDestroy()
{
	return TRUE;
}

BOOL AgcmGuild::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

CHAR* AgcmGuild::GetSelfGuildID()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )			return NULL;

	AgpdGuildADChar* pcsAttachedGuild = m_pcsAgpmGuild->GetADCharacter( pcsCharacter );
	return pcsAttachedGuild ? pcsAttachedGuild->m_szGuildID : NULL;
}

AgpdGuild*	AgcmGuild::GetSelfGuildLock()
{
	CHAR* szGuildID = GetSelfGuildID();
	if( !szGuildID )		return NULL;
	if( !m_pcsAgpmGuild )	return NULL;

	return m_pcsAgpmGuild->GetGuildLock( szGuildID );
}

BOOL AgcmGuild::IsSelfGuildOperation(CHAR* szGuildID)
{
	if( !szGuildID )		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
	if( !pcsAttachedGuild )
		return FALSE;

	return strcmp(pcsAttachedGuild->m_szGuildID, szGuildID) == 0 ? TRUE : FALSE;
}

INT32 AgcmGuild::GetSelfCharLevel()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return 0;

	INT32 lLevel = 0;
	m_pcsAgpmFactors->GetValue( &pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

	return lLevel;
}

INT32 AgcmGuild::GetMemberList(INT32 lIndex, AgcdGuildMemberList& listAgcdGuildMember, INT32 lMaxCount)
{
	if(lIndex < 0 || lMaxCount < 1)
		return 0;

	AgpdGuild* pcsGuild = GetSelfGuildLock();
	if( !pcsGuild )		return 0;
	
	//임의변수를 만들어. list를 때려넣고. @@ 소팅후. 다시뭘 해야겠는뎅.. 아....
	listAgcdGuildMember.clear();

	
	AgpdGuildMember* pcsMember = NULL;
	INT32 lCount = 0;
	INT32 lIndex2 = 0, lFindIndex = 0;

	if(lIndex == 0)	// 처음 부터 달라고 하면 마스터를 넣어준다.
	{
		pcsMember = m_pcsAgpmGuild->GetMaster(pcsGuild);
		if(pcsMember)
		{
			listAgcdGuildMember.push_back( AgcdGuildMember( *pcsMember ) );
			lCount++;
		}
	}
	else
	{
		pcsMember = m_pcsAgpmGuild->GetMaster(pcsGuild);		//	첫페이지가 아니고 길마가 있다면 제일처음 길마가 들어왔으므로 1부터 찾는다.
		if(pcsMember)
		{
			lFindIndex = 1;
		}
	}

	if(lCount >= lMaxCount)
		return lCount;  

	AgcdGuildMemberList	listGuildMember;
	listGuildMember.clear();

	AgpdGuildMember** ppcsMember = NULL;
	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2); ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2) )
	{
		if( !*ppcsMember )		break;
		listGuildMember.push_back( AgcdGuildMember( **ppcsMember ) );
	}
	pcsGuild->m_Mutex.Release();

	//sort( listGuildMember.begin(), listGuildMember.end(), AgcdGuildMember::LevelLess );
	listGuildMember.sort();
	for( AgcdGuildMemberListItr Itr = listGuildMember.begin(); Itr != listGuildMember.end(); ++Itr )
	{
		if( !strcmp( pcsGuild->m_szMasterID, (*Itr).m_szMemberID ) )	continue;
		if( lIndex + 1 > ++lFindIndex )									continue;		// 해당 Index 까지 계속 continue

		listAgcdGuildMember.push_back( AgcdGuildMember( (*Itr) ) );

		lCount++;

		if( lCount >= lMaxCount )
			break;
	}

	return lCount;
}

/*
2005.07.18. By SungHoon
해당 길드 아이디의 사용자를 찾는다.
*/
INT32 AgcmGuild::SearchGuildMember( AgpdGuild *pcsGuild,  CHAR *szMemberID)
{
	if (!strcmp(szMemberID, pcsGuild->m_szMasterID)) return 0;

	INT32 lIndex = 0;
	INT32 lCount = 1;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
		ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !*ppcsMember )		break;
		if( !strcmp(pcsGuild->m_szMasterID, (*ppcsMember)->m_szID) )	continue;

		if ( !strcmp(szMemberID, (*ppcsMember)->m_szID) )
			return lCount;
		else
			++lCount;
	}

	return -1;
}

BOOL AgcmGuild::SendCreateGuild(CHAR* szGuildName, CHAR* szPassword)
{
	if(!szGuildName || !szPassword)
		return FALSE;

	if(strlen(szGuildName) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;
	if(strlen(szPassword) > AGPMGUILD_MAX_PASSWORD_LENGTH)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildCreatePacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildName, 
		pcsCharacter->m_szID,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		szPassword,
		NULL, NULL,	// Notice
		NULL,		// Status
		NULL,		// Win
		NULL,		// Draw
		NULL,		// Lose
		NULL,		// Guild Point
		NULL,		// GuildMarkTID
		NULL,		// GuildMarkColor
		NULL
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Master 가 szCharID 를 초대한다.
BOOL AgcmGuild::SendJoinRequest(CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	if(strlen(szCharID) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJoinRequestPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		GetSelfGuildID(),
		pcsCharacter->m_szID,
		szCharID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 초대를 거절했다!!!
BOOL AgcmGuild::SendJoinReject(CHAR* szGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	if(strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;
	if(strlen(szMasterID) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJoinRejectPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 내가 szGuildID 에 가입하는 패킷을 보낸다.
// 서버에서 유효성 검사 다 다시 하므로 클라이언트에서는 걍 보내면 된다.
BOOL AgcmGuild::SendJoin(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	if(strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJoinPacket(&nPacketLength, 
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 탈퇴
BOOL AgcmGuild::SendLeave()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	//	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
	//		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildLeavePacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 강퇴
BOOL AgcmGuild::SendForcedLeave(CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildForcedLeavePacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szCharID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 해체
BOOL AgcmGuild::SendDestroy(CHAR* szPassword)
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildDestroyPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szPassword);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 공지사항 수정
BOOL AgcmGuild::SendNotice(CHAR* szNotice)
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nNotcieLength = 0;
	if(szNotice)
		nNotcieLength = strlen(szNotice);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildUpdateNoticePacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szNotice, &nNotcieLength
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendBattlePerson( INT16 lPerson, CHAR* pPersonList, INT16 lSize )
{
	if( !lPerson || !pPersonList )	return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )				return FALSE;


	CHAR* szGuildID = GetSelfGuildID();
	if( !szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH )
		return FALSE;

	if( !m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID) )
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattlePersonPacket( &nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		&lPerson,
		pPersonList,
		&lSize
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
// 배틀 신청 패킷을 보낸다.
BOOL AgcmGuild::SendBattleRequest(CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson )
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleRequestPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID,
		&lType,
		&ulDuration,
		&lPerson
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
BOOL AgcmGuild::SendBattleAccept(CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson )
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleAcceptPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID,
		NULL,
		NULL,
		NULL,
		&lType,
		&ulDuration,
		&lPerson
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
BOOL AgcmGuild::SendBattleReject(CHAR* szEnemyGuildID)
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleRejectPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
BOOL AgcmGuild::SendBattleCancelRequest(CHAR* szEnemyGuildID)
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleCancelReqeustPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
BOOL AgcmGuild::SendBattleCancelAccept(CHAR* szEnemyGuildID)
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleCancelAcceptPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
BOOL AgcmGuild::SendBattleCancelReject(CHAR* szEnemyGuildID)
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleCancelRejectPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.18. steeple
BOOL AgcmGuild::SendBattleWithdraw(CHAR* szEnemyGuildID)
{
	if(!szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szGuildID = GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!m_pcsAgpmGuild->IsMaster(szGuildID, pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildBattleWithdrawPacket(&nPacketLength,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		szEnemyGuildID
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
// Callback

// Character Loading 이 다 끝났을 때 불린다.
// 길드 아이디가 있는 지 찾아서 세팅한다.
BOOL AgcmGuild::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgcmGuild* pThis = (AgcmGuild*)pClass;

	if (pThis->m_pcsAgcmCharacter->IsCharacterTransforming(pcsCharacter))
		return TRUE;

	AgpdGuildADChar* pcsAttachedGuild = (AgpdGuildADChar*)pThis->m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return FALSE;

	if (pThis->m_pcsAgcmTextBoardMng)
	{
		DWORD dwGuildColor = pThis->m_pcsAgcmTextBoardMng->GetGuildIDColor(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID);
		pThis->m_pcsAgcmTextBoardMng->EnableGuild((ApBase*)pcsCharacter, pcsAttachedGuild->m_szGuildID, dwGuildColor);
	}

	return TRUE;
}

/*
2005.07.11. By SungHoon
길드 List를 요청한다.
*/
BOOL AgcmGuild::SendGuildListRequest(INT16 nPage)
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildListRequestPacket(&pcsCharacter->m_lID, nPage, &nPacketLength );
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;

}

/*
2005.07.15. By SungHoon
특정 길드가 포함된 List 페이지 를 요청한다.
*/
BOOL AgcmGuild::SendGuildListRequestFind(CHAR *szGuildID)
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildListRequestFindPacket(&pcsCharacter->m_lID, szGuildID, &nPacketLength );
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;

}

/*
2005.07.13. By SungHoon
길드 최대 인원이 변경된다.
*/
BOOL AgcmGuild::CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmGuild* pThis = (AgcmGuild*)pClass;
	INT32 lCID = *( INT32 *)pClass;
	INT32 lNewMemberCount = *( INT32 *)pCustData;

	AgpdGuild* pcsGuild = pThis->GetSelfGuildLock();
	if(!pcsGuild)		return FALSE;

	pcsGuild->m_lMaxMemberCount = lNewMemberCount;

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

/*
2005.07.14. By SungHoon
길드 최대 인원 변경을 요청한다.
*/
BOOL AgcmGuild::SendMaxMemberCount(CHAR *szGuildID, INT32 lCID, INT32 lMaxMemberCount )
{
	if(!szGuildID) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildUpdateMaxMemberCountPacket(&nPacketLength, &lCID, szGuildID, &lMaxMemberCount);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}


/*
2005.07.28. By SungHoon
유저가 스스로 길드 가입 신청을 한다.
*/
BOOL AgcmGuild::SendSelfJoin(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	if(strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMGUILD_PACKET_JOIN;
	INT32 lMemberRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildMemberPacket(&nPacketLength, 
		&cOperation,
		&pcsCharacter->m_lID,
		szGuildID,
		pcsCharacter->m_szID,
		&lMemberRank,
		NULL,
		NULL,
		NULL,
		NULL
		);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendJoinAllow(CHAR* szMemberID)
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return FALSE;
		
	AgpdGuild* pcsGuild = GetSelfGuildLock();
	if( !pcsGuild )			return FALSE;

	if( !m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID) &&
		!m_pcsAgpmGuild->IsSubMaster( pcsGuild, pcsCharacter->m_szID) )
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMGUILD_PACKET_JOIN;
	INT32 lMemberRank = AGPMGUILD_MEMBER_RANK_NORMAL;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildMemberPacket(&nPacketLength, 
		&cOperation,
		&pcsCharacter->m_lID,
		pcsGuild->m_szID,
		szMemberID,
		&lMemberRank,
		NULL,
		NULL,
		NULL,
		NULL
		);
	pcsGuild->m_Mutex.Release();
	if( !pvPacket || nPacketLength < 1 )	return FALSE;
			
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendLeaveAllow(CHAR* szMemberID)
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return FALSE;

	AgpdGuild* pcsGuild = GetSelfGuildLock();
	if( !pcsGuild )			return FALSE;

	if (!m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMGUILD_PACKET_LEAVE_ALLOW;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildMemberPacket(&nPacketLength, 
		&cOperation,
		&pcsCharacter->m_lID,
		pcsGuild->m_szID,
		szMemberID,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
		);
	pcsGuild->m_Mutex.Release();

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
2005.08.24. By SungHoon
길드 이름 변경을 요청한다.
*/
BOOL AgcmGuild::SendGuildRenameGuildID(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildRenameGuildIDPacket(&lCID, szGuildID, szMemberID, &nPacketLength );

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
2005.08.25. By SungHoon
길드 아이디 변경을 요청한다.
*/
BOOL AgcmGuild::CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmGuild *pThis = (AgcmGuild *)pClass;
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
BOOL AgcmGuild::RenameGuild(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID)
{
	//	길마인지 확인 하구
	if( !m_pcsAgpmGuild->IsMaster( GetSelfGuildID(), szMemberID ) ) return FALSE;

	AgpdGuild *pcsGuild = GetSelfGuildLock();
	if( !pcsGuild )		return FALSE;

	m_pcsAgpmGuild->RenameGuildID(pcsGuild, szGuildID);

	INT32 lIndex = 0;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
		ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !ppcsMember || !*ppcsMember )
			break;

		AgpdCharacter *pcsGuildCharacter = m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
		if ( !pcsGuildCharacter )	continue;

		// 콜백 불러준다.
		m_pcsAgpmGuild->SetCharAD(pcsGuildCharacter, szGuildID, -1, -1, FALSE, NULL);
		m_pcsAgpmGuild->EnumCallback(AGPMGUILD_CB_CHAR_DATA, pcsGuildCharacter, szGuildID);
		pcsGuildCharacter->m_Mutex.Release();
	}

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

/*
2005.08.26. By SungHoon
본인이 길드 마스터인지 확인한다. 
*/
BOOL AgcmGuild::IsSelfGuildMaster()
{
	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter) return FALSE;

	if(m_pcsAgpmGuild->IsMaster(GetSelfGuildID(), pcsCharacter->m_szID)) return TRUE;

	return FALSE;
}

BOOL AgcmGuild::IsGuildBattle( AgpdCharacter* pMy )
{
	if( !pMy )	return FALSE;

	CHAR* szSelfGuildID = m_pcsAgpmGuild->GetJoinedGuildID( pMy );
	AgpdGuild* pcsGuild = m_pcsAgpmGuild->GetGuild( szSelfGuildID );
	if( !pcsGuild || !m_pcsAgpmGuild->IsBattleIngStatus(pcsGuild) )
		return FALSE;

	AgcmUIGuild* pcsAgpmUIGuild	= (AgcmUIGuild*)GetModule("AgcmUIGuild");
	if( !pcsAgpmUIGuild )	return FALSE;

	AgcmUIGuildBattleResult& cUIGuildBattleResult = pcsAgpmUIGuild->GetUIGuildBattleResult();
	switch( pcsGuild->m_csCurrentBattleInfo.m_eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		return TRUE;
	case eGuildBattleTotalSurvive:
		return cUIGuildBattleResult.IsMember( pMy->m_szID, TRUE );
	case eGuildBattlePrivateSurvive:
		return cUIGuildBattleResult.IsCurRoundMember( pMy->m_szID, pcsGuild->m_csCurrentBattleInfo.m_ulRound, TRUE );
	}

	return FALSE;
}

BOOL AgcmGuild::IsGuildBattleMember( AgpdCharacter* pMy, AgpdCharacter* pTarget )
{
	if( !pMy || !pTarget )	return FALSE;

	CHAR* szGuildID = m_pcsAgpmGuild->GetJoinedGuildID(pMy);
	CHAR* szTargetGuildID = m_pcsAgpmGuild->GetJoinedGuildID(pTarget);

	if( !szTargetGuildID || _tcslen(szTargetGuildID) <= 0 )
		return  FALSE;

	AgpdGuild* pcsGuild = m_pcsAgpmGuild->GetGuild(szGuildID);
	if( !pcsGuild || !m_pcsAgpmGuild->IsBattleIngStatus(pcsGuild) )
		return FALSE;

	if( strcmp( pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szTargetGuildID ) )
		return FALSE;

	AgcmUIGuild* pcsAgpmUIGuild	= (AgcmUIGuild*)GetModule("AgcmUIGuild");
	if( !pcsAgpmUIGuild )	return FALSE;

	AgcmUIGuildBattleResult& cUIGuildBattleResult = pcsAgpmUIGuild->GetUIGuildBattleResult();
	switch( pcsGuild->m_csCurrentBattleInfo.m_eType )
	{
	case eGuildBattleTotalSurvive:
		{
			if( cUIGuildBattleResult.IsMember( pMy->m_szID, TRUE ) )
			{
				if( cUIGuildBattleResult.IsMember( pTarget->m_szID, FALSE ) )
					return TRUE;
				else
					return FALSE;
			}
		}
		break;
	case eGuildBattlePrivateSurvive:
		{
			if( cUIGuildBattleResult.IsCurRoundMember( pMy->m_szID, pcsGuild->m_csCurrentBattleInfo.m_ulRound, TRUE ) )
			{
				if( cUIGuildBattleResult.IsCurRoundMember( pTarget->m_szID, pcsGuild->m_csCurrentBattleInfo.m_ulRound, FALSE ) )
					return TRUE;
				else
					return FALSE;
			}
		}
		break;
	default:
		return TRUE;
	}
	
	return FALSE;
}

/*
2005.09.05. By SungHoon
길드원의 이름을 변경한다.
*/
BOOL AgcmGuild::CBGuildRenameCharID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmGuild *pThis = ( AgcmGuild *)pClass;
	CHAR *szGuildID = (CHAR *)pData;
	PVOID *ppBuffer = (PVOID *)pCustData;

	if (!pThis->GetSelfGuildID()) return FALSE;
	if (strcmp(szGuildID, pThis->GetSelfGuildID())) return FALSE;

	pThis->RenameCharID((CHAR *)ppBuffer[0], (CHAR *)ppBuffer[1]);

	return TRUE;
}

/*
2005.09.05. By SungHoon
자기 자신의 길드에서 szOldID 길원을 szNewID로 아이디를 바꾼다.
*/
BOOL AgcmGuild::RenameCharID(CHAR *szOldID, CHAR *szNewID)
{
	AgpdGuild *pcsGuild = GetSelfGuildLock();
	if( !pcsGuild )		return FALSE;

	BOOL bResult = m_pcsAgpmGuild->RenameCharID(pcsGuild, szOldID, szNewID);

	pcsGuild->m_Mutex.Release();

	return bResult;
}

/*
2005.10.05. By SungHoon
AgpdGuildMarkTemplate 에 텍스쳐를 셋팅한다.
*/
BOOL AgcmGuild::SetGridGuildAttachedTexture( AgpdGuildMarkTemplate * pcsAgpdGuildMarkTemplate )
{
	if ( NULL == m_pcsAgcmUIControl || pcsAgpdGuildMarkTemplate == NULL ) return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdGuildMarkTemplate->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;

	AgcdGuildMarkTemplate	*pcsAgcdGuildMarkTemplate	= GetAttachDataGuildMarkTemplate(pcsAgpdGuildMarkTemplate);

	if (!pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szTexturePath);

		if ((pcsAgcdGuildMarkTemplate->m_szTextureName))
		{
			pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture = RwTextureRead(pcsAgcdGuildMarkTemplate->m_szTextureName, NULL);
			if (pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture)
			{
				RwTextureSetFilterMode( pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture, rwTEXTUREADDRESSCLAMP );
			}
			else
			{
				ASSERT(!"!AgcmGuild::SetGridGuildAttachedTexture pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture == NULL");
			}
		}
	}

	if (!pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture)
		return FALSE;	

	*ppRwTexture			= pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture;

	return TRUE;		
}

/*
2005.10.05. By SungHoon
AgpdGuildMarkTemplate 에 작은 텍스쳐를 셋팅한다.
*/
BOOL AgcmGuild::SetGridGuildAttachedSmallTexture( AgpdGuildMarkTemplate * pcsAgpdGuildMarkTemplate )
{
	if ( NULL == m_pcsAgcmUIControl || pcsAgpdGuildMarkTemplate == NULL ) return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdGuildMarkTemplate->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;

	AgcdGuildMarkTemplate	*pcsAgcdGuildMarkTemplate	= GetAttachDataGuildMarkTemplate(pcsAgpdGuildMarkTemplate);
	if (!pcsAgcdGuildMarkTemplate) return FALSE;

	if (!pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szTexturePath);

		if ((pcsAgcdGuildMarkTemplate->m_szSmallTextureName))
		{
			pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture = RwTextureRead(pcsAgcdGuildMarkTemplate->m_szSmallTextureName, NULL);
			if (pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture)
			{
				RwTextureSetFilterMode( pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture, rwTEXTUREADDRESSCLAMP );
			}
			else
			{
				ASSERT(!"!AgcmGuild::SetGridGuildAttachedSmallTexture pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture == NULL");
			}
		}
	}

	if (!pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture)
		return FALSE;	

	*ppRwTexture			= pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture;

	return TRUE;		
}
/*
2005.10.05. By SungHoon
길드 마크 템플리트에 Attach 된 텍스춰 정보를 초기화 한다.
*/
BOOL AgcmGuild::ConAgcdGuildMarkTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmGuild* pThis = (AgcmGuild*)pClass;
	AgcdGuildMarkTemplate* pAttachedData = pThis->GetAttachDataGuildMarkTemplate((AgpdGuildMarkTemplate*)pData);

	if(!pAttachedData)
		return FALSE;
	pAttachedData->m_szTextureName = NULL;
	pAttachedData->m_pGuildMarkTexture = NULL;
	pAttachedData->m_szSmallTextureName = NULL;
	pAttachedData->m_pSmallGuildMarkTexture = NULL;

	return TRUE;
}

/*
2005.10.05. By SungHoon
길드 마크 템플리트에 Attach 된 텍스춰 정보를 제거 한다.
*/
BOOL AgcmGuild::DesAgcdGuildMarkTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)	return FALSE;

	AgcmGuild		*pThis			= (AgcmGuild*)		pClass;
	ApBase			*pcsBase		= (ApBase	*)		pData;

	AgcdGuildMarkTemplate	*pAttachedData = pThis->GetAttachDataGuildMarkTemplate(pcsBase);
	if (!pAttachedData)	return FALSE;
	pThis->ReleaseAgcdGuildMarkTemplate(pAttachedData);
	return TRUE;
}

BOOL AgcmGuild::CBCheckGuildAttackable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmGuild* pThis = (AgcmGuild*)pClass;
	AgpdCharacter* pcsAttackChar = (AgpdCharacter *)pData;
	AgpdCharacter* pcsTargetChar = (AgpdCharacter *)((PVOID *) pCustData)[0];
	BOOL bForceAttack = (BOOL)PtrToInt(((PVOID *) pCustData)[1]);

	if (!pcsTargetChar)
		return FALSE;

	pThis->IsGuildBattleMember( pcsAttackChar, pcsTargetChar );
	return TRUE;
}

BOOL AgcmGuild::CB_OnResultWorldChampionShipRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass ) return FALSE;

	AgcmGuild* pThis = ( AgcmGuild* )pClass;
	PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT* pResult = ( PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT* )pData;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	pcmUIManager->ActionMessageOKDialog( pResult->strMessage );
	return TRUE;
}

/*
2005.10.05. By SungHoon
길드 마크 템플릿에 추가된 텍스쳐 정보 데이터 주소를 가져온다.
*/
AgcdGuildMarkTemplate* AgcmGuild::GetAttachDataGuildMarkTemplate(PVOID pData)
{
	if(m_pcsAgpmGuild)
		return (AgcdGuildMarkTemplate*)m_pcsAgpmGuild->GetAttachedModuleData(m_nIndexAttachDataGuildMarkTemplate, (PVOID)pData);

	return NULL;
}

/*
2005.10.05. By SungHoon
텍스쳐 기본 폴더를 지정한다.
*/
VOID AgcmGuild::SetTexturePath(CHAR *szPath)
{
	m_szTexturePath[0] = 0;

	if (szPath)
		strncat(m_szTexturePath, szPath, _MAX_PATH);
}


/*
2005.10.07. By SungHoon
길드 마크의 텍스쳐이름을 셋팅한다.
*/
BOOL AgcmGuild::CBGuildMarktemplateLoad(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass) return FALSE;
	AgcmGuild *pThis = ( AgcmGuild *)pClass;
	AgpdGuildMarkTemplate	*pcsAgpdGuildMarkTemplate = (AgpdGuildMarkTemplate *)pData;
	AgcdGuildMarkTemplate	*pcsAgcdGuildMarkTemplate = pThis->GetAttachDataGuildMarkTemplate(pcsAgpdGuildMarkTemplate);
	if (!pcsAgcdGuildMarkTemplate) return FALSE;

	pThis->ReleaseAgcdGuildMarkTemplate(pcsAgcdGuildMarkTemplate);
	if (pcsAgpdGuildMarkTemplate->m_lTypeCode < 1 || pcsAgpdGuildMarkTemplate->m_lTypeCode > 4) return FALSE;		//	타입은 세종류

	if (pcsAgcdGuildMarkTemplate->m_szTextureName)
		delete []pcsAgcdGuildMarkTemplate->m_szTextureName;
	if (pcsAgcdGuildMarkTemplate->m_szSmallTextureName)
		delete []pcsAgcdGuildMarkTemplate->m_szSmallTextureName;
	pcsAgcdGuildMarkTemplate->m_szTextureName = NULL;
	pcsAgcdGuildMarkTemplate->m_szSmallTextureName = NULL;

	if (pcsAgpdGuildMarkTemplate->m_lTypeCode == 1 || pcsAgpdGuildMarkTemplate->m_lTypeCode == 2 
		|| pcsAgpdGuildMarkTemplate->m_lTypeCode == 3)
		pcsAgcdGuildMarkTemplate->m_szTextureName = new CHAR[ _MAX_PATH ];
	pcsAgcdGuildMarkTemplate->m_szSmallTextureName = new CHAR[ _MAX_PATH ];

	CHAR szTypeName[4][_MAX_PATH] = { AGPMGUILD_MARK_BOTTOM, AGPMGUILD_MARK_PATTERN, AGPMGUILD_MARK_SYMBOL, AGPMGUILD_MARK_COLOR};

	if ( pcsAgpdGuildMarkTemplate->m_lTypeCode == 1 || pcsAgpdGuildMarkTemplate->m_lTypeCode == 2 
		|| pcsAgpdGuildMarkTemplate->m_lTypeCode == 3)
		wsprintf(pcsAgcdGuildMarkTemplate->m_szTextureName, "GM_%s%02u.png",szTypeName[pcsAgpdGuildMarkTemplate->m_lTypeCode -1], pcsAgpdGuildMarkTemplate->m_lIndex);
	wsprintf(pcsAgcdGuildMarkTemplate->m_szSmallTextureName, "GM_%sS%02u.png",szTypeName[pcsAgpdGuildMarkTemplate->m_lTypeCode -1], pcsAgpdGuildMarkTemplate->m_lIndex);

	return TRUE;
}

/*
2005.10.07. By SungHoon
AgcdGuildMarkTemplate 에 들어있는 내용을 초기화한다.
*/
VOID AgcmGuild::ReleaseAgcdGuildMarkTemplate( AgcdGuildMarkTemplate *pcsAgcdGuildMarkTemplate)
{
	if (pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture)
	{
		RwTextureDestroy(pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture);
		pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture = NULL;
	}

	if (pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture)
	{
		RwTextureDestroy(pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture);
		pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture = NULL;
	}

	if (pcsAgcdGuildMarkTemplate->m_szTextureName)
	{
		delete [] pcsAgcdGuildMarkTemplate->m_szTextureName;
		pcsAgcdGuildMarkTemplate->m_szTextureName	= NULL;
	}

	if (pcsAgcdGuildMarkTemplate->m_szSmallTextureName)
	{
		delete [] pcsAgcdGuildMarkTemplate->m_szSmallTextureName;
		pcsAgcdGuildMarkTemplate->m_szSmallTextureName	= NULL;
	}

}

/*
2005.10.19. By SungHoon
AgpmGuild 에서 AgcdGuildTemplate의 Texture를 사용하기 위한 방법
*/
BOOL AgcmGuild::CBGetGuildMarkTexture(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmGuild *pThis = (AgcmGuild *)pClass;
	AgpdGuildMarkTemplate *pcsAgpdGuildMarkTemplate = (AgpdGuildMarkTemplate *)pData;
	RwTexture **ppTexture = (RwTexture **)pCustData;
	AgcdGuildMarkTemplate *pcsAgcdGuildMarkTemplate	= pThis->GetAttachDataGuildMarkTemplate(pcsAgpdGuildMarkTemplate);

	*(ppTexture) = pcsAgcdGuildMarkTemplate->m_pGuildMarkTexture;

	return TRUE;
}

/*
2005.10.19. By SungHoon
AgpmGuild 에서 AgcdGuildTemplate의 Texture를 사용하기 위한 방법
*/
BOOL AgcmGuild::CBGetGuildMarkSmallTexture(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmGuild *pThis = (AgcmGuild *)pClass;
	AgpdGuildMarkTemplate *pcsAgpdGuildMarkTemplate = (AgpdGuildMarkTemplate *)pData;
	RwTexture **ppTexture = (RwTexture **)pCustData;
	AgcdGuildMarkTemplate *pcsAgcdGuildMarkTemplate	= pThis->GetAttachDataGuildMarkTemplate(pcsAgpdGuildMarkTemplate);

	*(ppTexture) = pcsAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture;

	return TRUE;
}

/*
2005.10.19. By SungHoon
길드 마크를 구입 요청을 한다.
*/
BOOL AgcmGuild::SendGuildBuyGuildMark(INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL bForce )
{
	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter) return FALSE;

	INT32 lCID = pcsCharacter->m_lID;
	CHAR *szGuildID = GetSelfGuildID();
	if (!szGuildID) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = NULL;
	if (bForce) pvPacket = m_pcsAgpmGuild->MakeGuildBuyGuildMarkForcePacket(&nPacketLength ,lCID, szGuildID, &lGuildMarkTID, &lGuildMarkColor);
	else pvPacket = m_pcsAgpmGuild->MakeGuildBuyGuildMarkPacket(&nPacketLength ,lCID, szGuildID, &lGuildMarkTID, &lGuildMarkColor);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmGuild::SendJointRequest(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJointRequestPacket(&nPacketLength, GetSelfGuildID(), szGuildID, pcsSelfCharacter->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendJointReject(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJointRejectPacket(&nPacketLength, GetSelfGuildID(), szGuildID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendJoint(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJointPacket(&nPacketLength, GetSelfGuildID(), szGuildID, 0, 0);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendJointLeave()
{
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildJointLeavePacket(&nPacketLength,
		pcsSelfCharacter->m_lID,
		GetSelfGuildID(),
		GetSelfGuildID(),
		NULL);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendHostileRequest(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildHostileRequestPacket(&nPacketLength, GetSelfGuildID(), szGuildID, pcsSelfCharacter->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendHostileReject(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildHostileRejectPacket(&nPacketLength, GetSelfGuildID(), szGuildID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendHostile(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildHostilePacket(&nPacketLength, GetSelfGuildID(), szGuildID, 0);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendHostileLeaveRequest(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildHostileLeaveRequestPacket(&nPacketLength, GetSelfGuildID(), szGuildID, pcsSelfCharacter->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendHostileLeaveReject(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildHostileRejectPacket(&nPacketLength, GetSelfGuildID(), szGuildID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendHostileLeave(CHAR* szGuildID)
{
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmGuild->MakeGuildHostileLeavePacket(&nPacketLength, pcsSelfCharacter->m_lID, GetSelfGuildID(), szGuildID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmGuild::SendWorldChampionshipRequest()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST pPacket(pcsCharacter->m_lID);
	g_pEngine->SendPacket(pPacket);

	return TRUE;
}

BOOL AgcmGuild::SendWorldChampionshipEnter()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER pPacket(pcsCharacter->m_lID);
	g_pEngine->SendPacket(pPacket);

	return TRUE;
}

BOOL AgcmGuild::SendAppointRequest( CHAR* szMasterID , CHAR* szTarget , INT32 Rank )
{
	if( !szMasterID || !szTarget )		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST pPacket( szMasterID , szTarget , Rank );
	
	g_pEngine->SendPacket(pPacket);

	return TRUE;
}

BOOL AgcmGuild::SendAppointAnswer( CHAR* szMasterID , CHAR* szTarget , INT32 Rank , INT32 Answer )
{
	if( !szMasterID || !szTarget )		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER pPacket( szMasterID , szTarget , Rank , Answer );

	g_pEngine->SendPacket(pPacket);

	return TRUE;
}

BOOL AgcmGuild::SendSuccessionRequest( CHAR* szMasterID , CHAR* szTarget )
{
	if( !szMasterID || !szTarget )		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST pPacket( szMasterID , szTarget );

	g_pEngine->SendPacket(pPacket);

	return TRUE;
}

BOOL AgcmGuild::SendSuccessionAnswer( CHAR* szMasterID , CHAR* szTarget , INT32 Answer )
{
	if( !szMasterID || !szTarget )		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER pPacket( szMasterID , szTarget , Answer );

	g_pEngine->SendPacket(pPacket);

	return TRUE;
}