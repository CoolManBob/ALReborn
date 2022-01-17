// AgcmUIGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 08.

#include "AgcmUIGuild.h"

#include "ApUtil.h"
#include "ApMemoryTracker.h"
#include "ApAutoLockCharacter.h"
#include "AuStrTable.h"
#include "AgpmEventGuild.h"
#include "AgpmSystemMessage.h"

#define AGCMUIGUILD_COMMAND_LEAVE_ENG					"/GOUT"
#define AGCMUIGUILD_COMMAND_LEAVE						"/길드탈퇴"
#define AGCMUIGUILD_COMMAND_BATTLE_REQUEST				"/길드전신청"
#define AGCMUIGUILD_COMMAND_BATTLE_CANCEL				"/길드전취소"
#define AGCMUIGUILD_COMMAND_BATTLE_WITHDRAW_ENG			"/GG"
#define AGCMUIGUILD_COMMAND_BATTLE_WITHDRAW				"/패배"

AgcmUIGuild::AgcmUIGuild()
{
	SetModuleName("AgcmUIGuild");

	EnableIdle(TRUE);

	m_pcsAgpmGuild = NULL;
	m_pcsAgpmPvP = NULL;
	
	m_pcsAgpmCharacter = NULL;
	m_pcsAgcmCharacter = NULL;
	m_pcsAgcmGuild = NULL;
	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgcmEventGuild = NULL;
	m_pcsAgcmChatting = NULL;
	m_pcsAgcmFont = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgcmUIPartyOption = NULL;
	m_pcsAgpmGrid = NULL;
	m_pcsAgcmUIControl = NULL;
	m_pcsAgcmTextBoard = NULL;
	m_pcsAgcmUIChatting2 = NULL;
	m_pcsAgpmEventGuild = NULL;

	memset(m_szSelfGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	memset(m_szInputGuildName, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	memset(m_szInputPassword, 0, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH+1));

	m_lEventOpenGuildMainUI = 0;
	m_lEventOpenGuildCreateUI = 0;
	m_lEventOpenGuildDestroyUI = 0;
	m_lEventOpenGuildNoticeUI = 0;
	m_lEventOpenGuildMemberUI = 0;
	m_lEventOpenGuildGuildListUI = 0;		//	2005.07.06. By SungHoon
	m_lEventOpenGuildCreateConfirmUI = 0;
	m_lEventOpenGuildCreateCompleteUI = 0;

	m_lEventCloseGuildMainUI = 0;
	m_lEventCloseGuildCreateUI = 0;
	m_lEventCloseGuildDestroyUI = 0;
	m_lEventCloseGuildNoticeUI = 0;
	m_lEventCloseGuildMemberUI = 0;
	m_lEventCloseGuildGuildListUI = 0;		//	2005.07.06. By SungHoon
	m_lEventCloseGuildCreateConfirmUI = 0;
	m_lEventCloseGuildCreateCompleteUI = 0;

	m_pstNoticeUD = NULL;
	m_lNoticeUDIndex = 0;

	m_pstRecordUD = NULL;
	m_pstUDGuildDetailInfo = NULL;
	m_pstUDGuildMemberDetailInfo = NULL;
	m_pstUDGuildListDetailInfo = NULL;		//	2005.07.07 By SungHoon

	m_lRecordUDIndex = 0;
	m_lUDGuildDetailInfo = 0;
	m_lUDGuildMemberDetailInfo = 0;
	m_lUDGuildListDetailInfo = 0;			//	2005.07.07 By SungHoon
	
	m_pstMemberIndexUD = NULL;
	for(int i = 0; i < AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST; ++i)
	{
		m_arrMemberIndex[i]			= i;
		m_arrSelectedMemberIndex[i]	= i;
	}
	m_listAgcdGuildMember.clear();

	m_lVisibleGuildCount = 0;		//	2005.07.07 By SungHoon

	for(int i = 0; i < AGPMGUILD_MAX_VISIBLE_GUILD_LIST; ++i)		//	2005.07.07 By SungHoon
	{
		m_arrGuildListIndex[i] = i;

		m_parrAgcdGuildList[i] = new AgcdGuildList;
		memset( m_parrAgcdGuildList[i], 0, sizeof(AgcdGuildList) );
	}

	m_pstSelectedMemberIDUD = NULL;
	memset(m_szSelectedMemberID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));
	m_pstSelectedMemberIndexUD = NULL;
	m_lSelectedMemberIndex = -1;
	m_lSelectedGuildIndex = -1;		//	2005.07.18 By SungHoon

	m_bGuildInfoUIOpened = FALSE;
	m_cGuildInfoUILastTab = 0;
	m_lCurrentPage = 0;

	m_lGuildListCurrentPage = 0;		//	2005.07.07 By SungHoon
	
	memset(&m_stGuildMainOpenPos, 0, sizeof(m_stGuildMainOpenPos));

	m_pstJoinGuildIDUD = NULL;
	memset(m_szJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));

	m_pstJoinGuildMasterIDUD = NULL;
	memset(m_szJoinGuildMasterID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));

	m_pstBattleEnemyGuildIDUD = NULL;
	memset(m_szBattleEnemyGuildID, 0, sizeof(m_szBattleEnemyGuildID));

	m_pstAppointAndCancelBtnID	= NULL;
	ZeroMemory( m_szAppointAndCancelBtnID , sizeof(m_szAppointAndCancelBtnID) );
	strcpy_s( m_szAppointAndCancelBtnID , AGPMGUILD_MAX_GUILD_ID_LENGTH , ClientStr().GetStr( STI_STATICTEXT_SUBMASTER_APPOINT ) );

	m_bIsGuildUIOpen	= FALSE;

//	2005.07.18 By SungHoon
	m_bEnableIncreaseMaxMember = FALSE;
	m_bEnableGuildBattle = FALSE;

//	2005.07.20. By SungHoon
	m_lMaxGuildListPage = 0;
	m_lMaxMemberListPage = 0;

//	2005.07.21. By SungHoon
	m_lGuildMemberCount = 0;
	m_lGuildListMaxCount = 0;

//	2005.07.21. By SungHoon
	m_bEnableLeaveGuildNotice = FALSE;
	m_bEnableLeaveGuildMemberList = FALSE;
//	2005.08.24. By SungHoon
	m_bEnableAllowGuildMemberList = FALSE;

//	2005.07.22. By SungHoon
	m_bEnableInviteParty = FALSE;
	m_bEnableChatWhisper = FALSE;

//	2005.08.31. By SungHoon
	m_bEnableCreateGuild = TRUE;
	m_bEnableGuildMaster = FALSE;
	m_bEnableRenameGuildID = FALSE;

//	2005.10.10. By SungHoon
	m_lGuildMarkLayerIndex = 0;
//	2005.10.12. By SungHoon
	m_lGuildMarkSelectedTID = 0;
//	2005.10.18. By SungHoon
	m_lGuildMarkSelectedColor = 0xFFFFFFFF;

	m_pcsGuildMarkPreviewLargeGridItem = NULL;
	m_pcsGuildMarkPreviewSmallGridItem = NULL;

//	2005.10.14. By SungHoon
	m_pcsGuildMarkPreviewLargePatternGridItem = NULL;
	m_pcsGuildMarkPreviewSmallPatternGridItem = NULL;
	m_pcsGuildMarkPreviewLargeSymbolGridItem = NULL;
	m_pcsGuildMarkPreviewSmallSymbolGridItem = NULL;

//	2005.10.20. By SUngHoon
	m_bEnableBuyGuildMark = FALSE;

	m_lGuildMarkDetailInfo = 0;

	m_pcsGuildMarkPreviewBuyGridItem  = NULL;
	m_pcsGuildMarkPreviewBuyPatternGridItem  = NULL;
	m_pcsGuildMarkPreviewBuySymbolGridItem  = NULL;

	m_pcsGuildMarkMainGridItem  = NULL;
	m_pcsGuildMarkMainPatternGridItem  = NULL;
	m_pcsGuildMarkMainSymbolGridItem  = NULL;

//	2005.10.26. By SungHoon
	m_pcsGuildMarkGuildListGridItem  = NULL;
	m_pcsGuildMarkGuildListPatternGridItem  = NULL;
	m_pcsGuildMarkGuildListSymbolGridItem  = NULL;

//	2005.10.27. By SungHoon
	m_pcsGuildMarkGuildDestroyGridItem  = NULL;
	m_pcsGuildMarkGuildDestroyPatternGridItem  = NULL;
	m_pcsGuildMarkGuildDestroySymbolGridItem  = NULL;

	memset(m_szRelationGuildID, 0, sizeof(m_szRelationGuildID));
	memset(m_szRelationMasterID, 0, sizeof(m_szRelationMasterID));

	m_lEventOpenJointRequestConfirmUI = 0;
	m_lEventOpenJointAcceptConfirmUI = 0;
	m_lEventOpenJointLeaveConfirmUI = 0;
    m_lEventOpenHostileRequestConfirmUI = 0;
	m_lEventOpenHostileAcceptConfirmUI = 0;
	m_lEventOpenHostileLeaveRequestConfirmUI = 0;
	m_lEventOpenHostileLeaveAcceptConfirmUI = 0;

	m_lEventOpenWorldChampionshipUI = 0;

	m_eListMode = AGCMUIGUILD_LIST_VIEW_MODE_NORMAL;
}

AgcmUIGuild::~AgcmUIGuild()
{
	m_listAgcdGuildMember.clear();
}

BOOL AgcmUIGuild::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)GetModule("AgpmItem");
	m_pcsAgpmGuild			= (AgpmGuild*)GetModule("AgpmGuild");
	m_pcsAgpmPvP			= (AgpmPvP*)GetModule("AgpmPvP");
	m_pcsAgpmSiegeWar		= (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	m_pcsAgpmFactors		= (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgpmParty			= (AgpmParty *)GetModule("AgpmParty");

	m_pcsAgcmCharacter		= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgpmGrid			= (AgpmGrid *)GetModule("AgpmGrid");
	m_pcsAgcmGuild			= (AgcmGuild*)GetModule("AgcmGuild");
	m_pcsAgcmUIManager2		= (AgcmUIManager2*)GetModule("AgcmUIManager2");
	m_pcsAgcmEventGuild		= (AgcmEventGuild*)GetModule("AgcmEventGuild");
	m_pcsAgcmChatting		= (AgcmChatting2*)GetModule("AgcmChatting2");
	m_pcsAgcmFont			= (AgcmFont*)GetModule("AgcmFont");
	
	m_pcsAgcmUIControl		= (AgcmUIControl *)GetModule("AgcmUIControl");
	m_pcsAgcmTextBoard		= (AgcmTextBoardMng *)GetModule("AgcmTextBoardMng");
	m_pcsAgcmUIChatting2	= (AgcmUIChatting2 *)GetModule("AgcmUIChatting2");

	m_pcsAgcmUIPartyOption = (AgcmUIPartyOption *)GetModule("AgcmUIPartyOption");

	if(!m_pcsAgpmGuild || !m_pcsAgpmPvP ||
		!m_pcsAgcmCharacter || !m_pcsAgcmGuild || !m_pcsAgcmUIManager2 || !m_pcsAgcmEventGuild || !m_pcsAgcmChatting || 
		!m_pcsAgcmFont
		)
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackGuildCreate(CBCreateComplete, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildJoinRequest(CBJoinRequest, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildJoinReject(CBJoinReject, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildJoin(CBJoin, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildLeave(CBLeave, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildDestroy(CBDestroy, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackCharData(CBCharData, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackUpdateMemberStatus(CBUpdateMemberStatus, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackUpdateNotice(CBUpdateNotice, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackSystemMessage(CBSystemMessage, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildUpdateMaxMemberCount(CBGuildUpdateMaxMemberCount, this))		//	2005.07.18. By SungHoon
		return FALSE;


	if(!m_pcsAgpmGuild->SetCallbackBattleInfo(CBReceiveBattleInfo, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleRequest(CBReceiveBattleRequest, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleAccept(CBReceiveBattleAccept, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleReject(CBReceiveBattleReject, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleCancelRequest(CBReceiveBattleCancelRequest, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleCancelAccept(CBReceiveBattleCancelAccept, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleCancelReject(CBReceiveBattleCancelReject, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleStart(CBReceiveBattleStart, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleUpdateTime(CBReceiveBattleUpdateTime, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleUpdateScore(CBReceiveBattleUpdateScore, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleResult(CBReceiveBattleResult, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleMember(CBReceiveBattleMember, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleMemberList(CBReceiveBattleMemberList, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackBattleRound(CBReceiveBattleRound, this))
		return FALSE;

	if(!m_pcsAgcmEventGuild->SetCallbackGrant(CBEventGrant, this))
		return FALSE;
	
	if(!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	//if(!m_pcsAgcmChatting->SetCallbackCommandGuildLeave(CBGuildLeaveByCommand, this))
	//	return FALSE;
	if(!m_pcsAgcmChatting->SetCallbackCommandProcess(CBProcessChatCommand, this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackReceiveGuildList(CBReceiveGuildListInfo, this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackReceiveLeaveRequestSelf(CBReceiveLeaveRequestSelf, this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackRenameGuildID(CBGuildRenameGuildID,this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackRenameCharID(CBGuildRenameCharID,this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackBuyGuildMark(CBGuildBuyGuildMark,this))
		return FALSE;

	if(!m_pcsAgpmGuild->SetCallbackJointRequest(CBJointRequest, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackJoint(CBJoint, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackJointLeave(CBJointLeave, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackHostileRequest(CBHostileRequest, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackHostile(CBHostile, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackHostileLeaveRequest(CBHostileLeaveRequest, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackHostileLeave(CBHostileLeave, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackJointDetail(CBJointDetail, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackHostileDetail(CBHostileDetail, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGetJointMark(CBGetJointMark, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGetJointColor(CBGetJointColor, this))
		return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildAppointmentAsk( CBGuildAppointAsk , this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildSuccessionAsk	( CBGuildSuccessionAsk , this))
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildAppointmentSysMsg( CBGuildAppointSysMsg , this) )
		return FALSE;
	if(!m_pcsAgpmGuild->SetCallbackGuildMemberRankUpdate( CBGuildMemberRankChange, this) )
		return FALSE;

	if(!AddEvent())
		return FALSE;
	if(!AddFunction())
		return FALSE;
	if(!AddDisplay())
		return FALSE;
	if(!AddUserData())
		return FALSE;
	if(!AddBoolean())
		return FALSE;

	m_cUIGuildBattleOffer.AddModule( m_pcsAgcmUIManager2, this );
	m_cUIGuildBattleMember.AddModule( m_pcsAgcmUIManager2, this );
	m_cUIGuildBattleResult.AddModule( m_pcsAgcmUIManager2, this );

	return TRUE;
}

BOOL AgcmUIGuild::OnInit()
{
	m_pcsAgpmEventGuild = (AgpmEventGuild*)GetModule("AgpmEventGuild");

	if(!m_pcsAgpmEventGuild)
		return FALSE;

	if(!m_pcsAgpmEventGuild->SetCallbackGrantWorldChampionship(CBEventGrantWorldChampionship, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIGuild::OnIdle(UINT32 ulClockCount)
{
	ProcessDrawBattleInfo(ulClockCount);

	//길드전을 신청하고 일정시간이 지났는데도..
	//상대방에서 OK가 날라오지 않는다면.. 자동적으로 
	//Close를 시켜주자 ^^

	GetUIGuildBattleOffer().Update();

	return TRUE;
}

BOOL AgcmUIGuild::OnDestroy()
{
	DestroyPreviewGuildMark();

	return TRUE;
}

BOOL AgcmUIGuild::AddEvent()
{
	m_lEventOpenGuildMainUI = m_pcsAgcmUIManager2->AddEvent("Guild_MainUI_Open");
	if(m_lEventOpenGuildMainUI < 0)
		return FALSE;
	m_lEventOpenGuildCreateUI = m_pcsAgcmUIManager2->AddEvent("Guild_CreateUI_Open");
	if(m_lEventOpenGuildCreateUI < 0)
		return FALSE;
	m_lEventOpenGuildDestroyUI = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyUI_Open");
	if(m_lEventOpenGuildDestroyUI < 0)
		return FALSE;
	m_lEventOpenGuildNoticeUI = m_pcsAgcmUIManager2->AddEvent("Guild_NoticeUI_Open");
	if(m_lEventOpenGuildNoticeUI < 0)
		return FALSE;
	m_lEventOpenGuildMemberUI = m_pcsAgcmUIManager2->AddEvent("Guild_MemberUI_Open");
	if(m_lEventOpenGuildMemberUI < 0)
		return FALSE;
//	2005.07.06. By SungHoon
	m_lEventOpenGuildGuildListUI = m_pcsAgcmUIManager2->AddEvent("Guild_GuildListUI_Open");
	if(m_lEventOpenGuildGuildListUI < 0)
		return FALSE;

	m_lEventOpenGuildCreateConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_CreateConfirmUI_Open");
	if(m_lEventOpenGuildCreateConfirmUI < 0)
		return FALSE;
	m_lEventOpenGuildCreateCompleteUI = m_pcsAgcmUIManager2->AddEvent("Guild_CreateComplete_Open");
	if(m_lEventOpenGuildCreateCompleteUI < 0)
		return FALSE;
	m_lEventOpenGuildDestroyConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyConfirmUI_Open");
	if(m_lEventOpenGuildDestroyConfirmUI < 0)
		return FALSE;
	m_lEventOpenGuildDestroyCompleteUI = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyCompleteUI_Open");
	if(m_lEventOpenGuildDestroyCompleteUI < 0)
		return FALSE;
	
	m_lEventCloseGuildMainUI = m_pcsAgcmUIManager2->AddEvent("Guild_MainUI_Close");
	if(m_lEventCloseGuildMainUI < 0)
		return FALSE;
	m_lEventCloseGuildCreateUI = m_pcsAgcmUIManager2->AddEvent("Guild_CreateUI_Close");
	if(m_lEventCloseGuildCreateUI < 0)
		return FALSE;
	m_lEventCloseGuildDestroyUI = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyUI_Close");
	if(m_lEventCloseGuildDestroyUI < 0)
		return FALSE;
	m_lEventCloseGuildNoticeUI = m_pcsAgcmUIManager2->AddEvent("Guild_NoticeUI_Close");
	if(m_lEventCloseGuildNoticeUI < 0)
		return FALSE;
	m_lEventCloseGuildMemberUI = m_pcsAgcmUIManager2->AddEvent("Guild_MemberUI_Close");
	if(m_lEventCloseGuildMemberUI < 0)
		return FALSE;
//	2005.07.06. By SungHoon
	m_lEventCloseGuildGuildListUI = m_pcsAgcmUIManager2->AddEvent("Guild_GuildListUI_Close");
	if(m_lEventCloseGuildGuildListUI < 0)
		return FALSE;

	m_lEventCloseGuildCreateConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_CreateConfirmUI_Close");
	if(m_lEventCloseGuildCreateConfirmUI < 0)
		return FALSE;
	m_lEventCloseGuildCreateCompleteUI = m_pcsAgcmUIManager2->AddEvent("Guild_CreateCompleteUI_Close");
	if(m_lEventCloseGuildCreateCompleteUI < 0)
		return FALSE;
	m_lEventCloseGuildDestroyConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyConfirmUI_Close");
	if(m_lEventOpenGuildDestroyConfirmUI < 0)
		return FALSE;
	m_lEventCloseGuildDestroyCompleteUI = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyCompleteUI_Close");
	if(m_lEventOpenGuildDestroyCompleteUI < 0)
		return FALSE;

	m_lEventOpenInviteConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Invite_ConfirmUI_Open", CBEventReturnInviteConfirm, this);
	if(m_lEventOpenInviteConfirmUI < 0)
		return FALSE;
	m_lEventOpenLeaveConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Leave_ConfirmUI_Open", CBEventReturnLeaveConfirm, this);
	if(m_lEventOpenLeaveConfirmUI < 0)
		return FALSE;
	m_lEventOpenForcedLeaveConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_ForcedLeave_ConfirmUI_Open", CBEventReturnForcedLeaveConfirm, this);
	if(m_lEventOpenForcedLeaveConfirmUI < 0)
		return FALSE;

	m_lEventOpenBattleRequestConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Battle_Request_ConfirmUI_Open", CBEventReturnBattleRequest, this);
	if(m_lEventOpenBattleRequestConfirmUI < 0)
		return FALSE;
	m_lEventOpenBattleAcceptConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Battle_Accept_ConfirmUI_Open", CBEventReturnBattleAccept, this);
	if(m_lEventOpenBattleAcceptConfirmUI < 0)
		return FALSE;
	m_lEventOpenBattleCancelRequestConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Battle_Cancel_Request_ConfirmUI_Open", CBEventReturnBattleCancelRequest, this);
	if(m_lEventOpenBattleCancelRequestConfirmUI < 0)
		return FALSE;
	m_lEventOpenBattleCancelAcceptConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Battle_Cancel_Accept_ConfirmUI_Open", CBEventReturnBattleCancelAccept, this);
	if(m_lEventOpenBattleCancelAcceptConfirmUI < 0)
		return FALSE;
	m_lEventOpenBattleWithdrawConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_Battle_Withdraw_ConfirmUI_Open", CBEventReturnBattleWithdraw, this);
	if(m_lEventOpenBattleWithdrawConfirmUI < 0)
		return FALSE;

//	2005.07.15. By SungHoon
	m_lEventGuildBattleTimeUIOpen = m_pcsAgcmUIManager2->AddEvent("Guild_GuildBattleTimeUI_Open");
	if(m_lEventGuildBattleTimeUIOpen < 0)
		return FALSE;

//	2005.07.18. By SungHoon
	m_lEventGuildMaxMemberIncreaseUIOpen = m_pcsAgcmUIManager2->AddEvent("Guild_MaxMemberIncreaseUI_Open");
	if(m_lEventGuildMaxMemberIncreaseUIOpen < 0)
		return FALSE;

	m_lEventGuildBattleTimeUIClose = m_pcsAgcmUIManager2->AddEvent("Guild_GuildBattleTimeUI_Close");
	if(m_lEventGuildBattleTimeUIClose < 0)
		return FALSE;

	m_lEventGuildMaxMemberIncreaseUIClose = m_pcsAgcmUIManager2->AddEvent("Guild_MaxMemberIncreaseUI_Close");
	if(m_lEventGuildMaxMemberIncreaseUIClose < 0)
		return FALSE;

	m_lEventGuildMaxMemberIncreaseOK = m_pcsAgcmUIManager2->AddEvent("Guild_MaxMemberIncreaseOK", CBEventReturnGuildMaxMemberIncreaseOK, this);
	if (m_lEventGuildMaxMemberIncreaseOK < 0)
		return FALSE;

//	2005.07.19. By SungHoon
	m_lEventGuildDestroyFailTooEarly = m_pcsAgcmUIManager2->AddEvent("Guild_DestroyFailTooEarly");
	if (m_lEventGuildDestroyFailTooEarly < 0)
		return FALSE;

//	2005.07.22. By SungHoon
	m_lEventWhisperChat = m_pcsAgcmUIManager2->AddEvent("Guild_Whisper_Chat");;
	if (m_lEventWhisperChat < 0)
		return FALSE;

	m_lEventOpenChatUI = m_pcsAgcmUIManager2->AddEvent("Guild_Open_Chat_UI");;
	if (m_lEventOpenChatUI < 0)
		return FALSE;

//	2005.08.02. By SungHoon
	m_lEventConfirmAllowJoin = m_pcsAgcmUIManager2->AddEvent("Guild_Confirm_Allow_Join", CBGuildJoinAllowMember, this );;
	if (m_lEventConfirmAllowJoin < 0)
		return FALSE;

	m_lEventConfirmAllowLeave = m_pcsAgcmUIManager2->AddEvent("Guild_Confirm_Allow_Leave", CBGuildLeaveAllowMember, this );;
	if (m_lEventConfirmAllowLeave < 0)
		return FALSE;

	m_lEventConfirmJoingRequest = m_pcsAgcmUIManager2->AddEvent("Guild_Confirm_Join_Request", CBEventReturnConfirmJoinRequest, this );;
	if (m_lEventConfirmJoingRequest < 0)
		return FALSE;

//	2005.08.22. By SungHoon
	m_lEventInitGuildNoticeUI = m_pcsAgcmUIManager2->AddEvent("Guild_NoticeUI_Init");
	if (m_lEventInitGuildNoticeUI < 0)
		return FALSE;

//	2005.08.26. By SungHoon
	m_lEventOpenGuildRenameUI = m_pcsAgcmUIManager2->AddEvent("Guild_RenameUI_Open");
	if (m_lEventOpenGuildRenameUI < 0)
		return FALSE;

	m_lEventCloseGuildRenameUI = m_pcsAgcmUIManager2->AddEvent("Guild_RenameUI_Close");
	if (m_lEventCloseGuildRenameUI  < 0)
		return FALSE;

//	2005.10.10. By SungHoon
	m_lEventGuildMarkUIOpen = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMarkUI_Open");
	if (m_lEventGuildMarkUIOpen < 0)
		return FALSE;

	m_lEventGuildMarkUIClose = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMarkUI_Close");
	if (m_lEventGuildMarkUIClose < 0)
		return FALSE;

//	2005.10.20. By SungHoon
	m_lEventGuildMarkBuyComplete = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_BuyComplete");
	if (m_lEventGuildMarkBuyComplete < 0)
		return FALSE;
	m_lEventGuildMarkDuplicate = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_Duplicate", CBGuildGuildMarkDuplicate, this );
	if (m_lEventGuildMarkDuplicate < 0)
		return FALSE;
	m_lEventGuildMarkInitialize = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_Initialize", CBGuildGuildMarkInitialize, this);
	if (m_lEventGuildMarkInitialize < 0)
		return FALSE;
	m_lEventGuildMarkNoExistBottom = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_No_Exist_Bottom");
	if (m_lEventGuildMarkNoExistBottom < 0)
		return FALSE;
	m_lEventGuildMarkBuy = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_Buy");			//	길드 마크 구입요청 UI를 띄운다.
	if (m_lEventGuildMarkBuy < 0)
		return FALSE;
	m_lEventGuildMarkInitializeComplete = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_Initialize_Complete");
	if (m_lEventGuildMarkInitializeComplete < 0)
		return FALSE;

//	2005.11.03. By SungHoon
	m_lEventGuildMarkNotEnoughtGuildMemberCount = m_pcsAgcmUIManager2->AddEvent("Guild_GuildMark_Not_Enought_Member");
	if (m_lEventGuildMarkNotEnoughtGuildMemberCount < 0)
		return FALSE;
	m_lEventGuildMemberUpNotEnoughtGuildMemberCount = m_pcsAgcmUIManager2->AddEvent("Guild_MemberUp_Not_Enought_Member");
	if (m_lEventGuildMemberUpNotEnoughtGuildMemberCount < 0)
		return FALSE;

//	2005.11.03. By SungHoon
	m_lEventGuildBattleFailRequestSelfGuild = m_pcsAgcmUIManager2->AddEvent("Guild_GuildBattle_Fail_Request_Self_Guild");
	if (m_lEventGuildBattleFailRequestSelfGuild < 0)
		return FALSE;
	
	// 2006.07.20. steeple
	m_lEventOpenJointRequestConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenJointRequestConfirmUI", CBEventReturnJointRequest, this);
	if(m_lEventOpenJointRequestConfirmUI < 0)
		return FALSE;
    m_lEventOpenJointAcceptConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenJointAcceptConfirmUI", CBEventReturnJointAccept, this);
	if(m_lEventOpenJointAcceptConfirmUI < 0)
		return FALSE;
	m_lEventOpenJointLeaveConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenJointLeaveConfirmUI", CBEventReturnJointLeave, this);
	if(m_lEventOpenJointLeaveConfirmUI < 0)
		return FALSE;
	m_lEventOpenHostileRequestConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenHostileRequestConfirmUI", CBEventReturnHostileRequest, this);
	if(m_lEventOpenHostileRequestConfirmUI < 0)
		return FALSE;
	m_lEventOpenHostileAcceptConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenHostileAcceptConfirmUI", CBEventReturnHostileAccept, this);
	if(m_lEventOpenHostileAcceptConfirmUI < 0)
		return FALSE;
	m_lEventOpenHostileLeaveRequestConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenHostileLeaveRequestConfirmUI", CBEventReturnHostileLeaveRequest, this);
	if(m_lEventOpenHostileLeaveRequestConfirmUI < 0)
		return FALSE;
	m_lEventOpenHostileLeaveAcceptConfirmUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenHostileLeaveAcceptConfirmUI", CBEventReturnHostileLeaveAccept, this);
	if(m_lEventOpenHostileLeaveAcceptConfirmUI < 0)
		return FALSE;
	m_lEventOpenWorldChampionshipUI = m_pcsAgcmUIManager2->AddEvent("Guild_OpenWorldChampionshipUI" );
	if(m_lEventOpenWorldChampionshipUI < 0)
		return FALSE;


	return TRUE;
}

BOOL AgcmUIGuild::AddFunction()
{
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Main_Open_Request", CBMainOpenRequest, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Main_Toggle", CBMainToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Main_Create_Request", CBMainCreateRequest, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Main_Destroy_Request", CBMainDestroyRequest, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Create_Toggle", CBCreateToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Create_Ok_Click", CBCreateOkClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Destroy_Toggle", CBDestroyToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Destroy_Ok_Click", CBDestroyOkClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Create_Confirm_Toggle", CBCreateConfirmToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Create_Confirm_Ok_Click", CBCreateConfirmOkClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Create_Complete_Toggle", CBCreateCompleteToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Destroy_Confirm_Toggle", CBDestroyConfirmToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Destroy_Confirm_Ok_Click", CBDestroyConfirmOkClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Destroy_Complete_Toggle", CBDestroyCompleteToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Notice_Open_Request", CBNoticeOpenRequest, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Notice_Toggle", CBNoticeToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Notice_Tab_Click", CBNoticeTabClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Notice_Close", CBNoticeClose, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Notice_Edit_Click", CBNoticeEditClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Notice_Send_Click", CBNoticeSendClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Toggle", CBMemberToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Tab_Click", CBMemberTabClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Select", CBMemberSelect, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Leave_Click", CBMemberLeaveClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Left_Click", CBMemberLeftClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Right_Click", CBMemberRightClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Close", CBMemberClose, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Select_DClick", CBMemberSelectDClick, 0))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Send_Invite", CBSendJoinRequest, 0))
		return FALSE;

//	2005.07.06. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Toggle", CBGuildListToggle, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Tab_Click", CBGuildListTabClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Close", CBGuildListClose, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Select", CBGuildListClick, 0))
		return FALSE;

//	2005.07.07. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Left_Click", CBGuildListLeftClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Right_Click", CBGuildListRightClick, 0))
		return FALSE;
//	2005.07.15. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildList_Search", CBGuildListSearch, 0))
		return FALSE;
	
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Member_Search", CBGuildMemberSearch, 0))
		return FALSE;	
//	2005.07.15. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildBattleTime_Request", CBGuildBattleTimeOpenUIClick, 0))
		return FALSE;
	
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_MaxMemberIncreaseOK", CBGuildMaxMemberIncreaseOK, 0))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Main_Increase_Request", CBGuildMaxMemberIncreaseOpenUI, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_IncreaseMember_Toggle", CBMaxMemberIncreaseUIToggle, 0))
		return FALSE;

	//if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Battle_Request", CBGuildBattleTimeRequest, 0))
	//	return FALSE;
	
//	2005.07.21. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Leave_Notice_Click", CBGuildClickLeaveGuildNotice, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_WhisperChat_Click", CBGuildClickWhisperChat, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_InviteParty_Click", CBGuildClickInviteParty, 0))
		return FALSE;
//	2005.07.22. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_WhisperChat", CBGuildWhisperChat, 0))
		return FALSE;

//	2005.08.22. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_NoticeUI_Init", CBNoticeUIInit, 0))
		return FALSE;

//	2005.08.22. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Allow_Click", CBGuildAllowClick, 0))
		return FALSE;

//	2005.08.26. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_RenameUI_Open", CBGuildRenameUIOpen, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_RenameUI_Init", CBGuildRenameUIInit, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Rename_OK_Click", CBGuildRenameOKClick, 0))
		return FALSE;

//	2005.10.10. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_Create_GuildMark", CBGuildCreateGuildMark, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_UpdateGuildMarkLayer", CBGuildupdateGuildMarkLayer, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkUI_Init", CBGuildGuildMarkUIInit, 0))
		return FALSE;

//	2005.10.11. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkLClickDown", CBGuildMarkGridLClickDown, 0))
		return FALSE;

//	2005.10.12. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkColorLClickDown", CBGuildMarkColorGridLClickDown, 0))
		return FALSE;

//	2005.10.14. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkClickInitialize", CBGuildGuildMarkClickInitialize, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkClickBuy", CBGuildGuildMarkClickBuy, 0))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkCloseUI", CBGuildGuildMarkUIClose, 0))
		return FALSE;

//	2005.10.20. By SungHoon
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkClickBuyOK", CBGuildGuildMarkClickBuyOK, 0))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_GuildMarkBuyConfirmUIOpen", CBGuildGuildMarkBuyConfirmUIOpen, 0))
		return FALSE;
	
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_OpenJointList", CBOpenJointList, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_OpenHostileList", CBOpenHostileList, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_OnJointRequestBtnClk", CBOnJointRequestBtnClick, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_OnHostileRequestBtnClk", CBOnHostileRequestBtnClick, 0))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_WorldChampionshipRequest", CBOnWorldChampionshipRequestBtnClick, 0))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddFunction(this, "Guild_WorldChampionshipEnter", CBOnWorldChampionshipEnterBtnClick, 0))
		return FALSE;

	
	// 부길마 임명
	if( !m_pcsAgcmUIManager2->AddFunction( this , "Guild_Appoint_Click" , CBGuildAppointClick , 0 ))
		return FALSE;

	// 길드 승계
	if( !m_pcsAgcmUIManager2->AddFunction( this , "Guild_Succession_Click" , CBGuildSuccessionClick , 0 ))
		return FALSE;
	
	return TRUE;
}

BOOL AgcmUIGuild::AddDisplay()
{
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Notice", 0, CBDisplayNotice, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
//	2005.07.04. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_DetailInfo", 0, CBDisplayDetailInfo, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_Status", 0, CBDisplayMemberStatus, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_Name", 0, CBDisplayMemberName, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_Level", 0, CBDisplayMemberLevel, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
//	2005.07.04. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_Class", 0, CBDisplayMemberClass, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
//	2005.07.04. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_Race", 0, CBDisplayMemberRace, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_Select", 0, CBDisplayMemberSelect, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Selected_MemberID", 0, CBDisplaySelectedMember, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Join_GuildID", 0, CBDisplayInviteGuild, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Battle_Record", 0, CBDisplayRecord, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Battle_Enemy_GuildID", 0, CBDisplayDialogEnemyGuildID, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Battle_Type", 0, CBDisplayDialogBattleType, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Battle_Duration", 0, CBDisplayDialogDuration, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Battle_Person", 0, CBDisplayDialogBattlePerson, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

//	2005.07.06. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Member_DetailInfo", 0, CBDisplayMemberDetailInfo, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

//	2005.07.07. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_List_DetailInfo", 0, CBDisplayGuildListDetailInfo, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

//	2005.07.08. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_List_GuildName", 0, CBDisplayGuildName, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_List_GuildMemberCount", 0, CBDisplayGuildMemberCount, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_List_GuildWinPoint", 0, CBDisplayGuildWinPoint, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_GuildList_Selected", 0, CBDisplayGuildSelected, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

//	2005.07.20. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_GuildList_Page", 0, CBDisplayGuildListPage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_GuildList_MaxPage", 0, CBDisplayGuildListMaxPage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_MemberList_Page", 0, CBDisplayGuildMemberListPage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_MemberList_MaxPage", 0, CBDisplayGuildMemberListMaxPage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
//	2005.07.21. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_List_BattleJoin", 0, CBDisplayGuildGuildBattleJoinRequest, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

//	2005.07.21. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_MemberList_AllowButton", 0, CBDisplayGuildAllowButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
//	2005.10.13. By SungHoon
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Mark_DetailInfo", 0, CBDisplayGuildMarkInfo, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_RelationGuildID", 0, CBDisplayRelationGuildID, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_RelationMasterID", 0, CBDisplayRelationMasterID, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Joint_Request_Button", 0, CBDisplayJointRequestBtn, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_Hostile_Request_Button", 0, CBDisplayHostileRequestBtn, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "Guild_WorldChampionShip_Notice", 0, CBDisplayDialogWorldChampionship, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if( !m_pcsAgcmUIManager2->AddDisplay( this , "Guild_AppointAndCancel_BtnText" , 0 , CBDisplayAppointAndCancelBtnText , AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIGuild::AddUserData()
{
	m_pstNoticeUD = m_pcsAgcmUIManager2->AddUserData("Guild_Notice_UD", &m_lNoticeUDIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstNoticeUD)
		return FALSE;

	m_pstRecordUD = m_pcsAgcmUIManager2->AddUserData("Guild_Record_UD", &m_lRecordUDIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstRecordUD)
		return FALSE;

//	2005.07.05. By SungHoon
	m_pstUDGuildDetailInfo = m_pcsAgcmUIManager2->AddUserData("Guild_DetailInfo_UD", &m_lUDGuildDetailInfo, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstUDGuildDetailInfo)
		return FALSE;
//	2005.07.06. By SungHoon
	m_pstUDGuildMemberDetailInfo = m_pcsAgcmUIManager2->AddUserData("Guild_Member_DetailInfo_UD", &m_lUDGuildMemberDetailInfo, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstUDGuildMemberDetailInfo)
		return FALSE;
//	2005.07.07. By SungHoon
	m_pstUDGuildListDetailInfo = m_pcsAgcmUIManager2->AddUserData("Guild_List_DetailInfo_UD", &m_lUDGuildListDetailInfo, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstUDGuildListDetailInfo)
		return FALSE;
//	2005.07.08. By SungHoon
	m_pstGuildListIndexUD = m_pcsAgcmUIManager2->AddUserData("Guild_List_Index", m_arrGuildListIndex, sizeof(INT32), AGPMGUILD_MAX_VISIBLE_GUILD_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstGuildListIndexUD)
		return FALSE;

	m_pstMemberIndexUD = m_pcsAgcmUIManager2->AddUserData("Guild_Member_Index", m_arrMemberIndex, sizeof(INT32), AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstMemberIndexUD)
		return FALSE;

	m_pstSelectedMemberIDUD = m_pcsAgcmUIManager2->AddUserData("Guild_Selected_MemberID_UD", m_szSelectedMemberID, AGPACHARACTER_MAX_ID_STRING, 1, AGCDUI_USERDATA_TYPE_STRING);
	if(!m_pstSelectedMemberIDUD)
		return FALSE;

	m_pstSelectedMemberIndexUD = m_pcsAgcmUIManager2->AddUserData("Guild_Selected_MemberIndex_UD", m_arrSelectedMemberIndex, sizeof(INT32), AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstSelectedMemberIndexUD)
		return FALSE;

	m_pstJoinGuildIDUD = m_pcsAgcmUIManager2->AddUserData("Guild_Join_GuildID", m_szJoinGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);
	if(!m_pstJoinGuildIDUD)
		return FALSE;

	m_pstJoinGuildMasterIDUD = m_pcsAgcmUIManager2->AddUserData("Guild_Join_GuildMasterID", m_szJoinGuildMasterID, AGPACHARACTER_MAX_ID_STRING, 1, AGCDUI_USERDATA_TYPE_STRING);
	if(!m_pstJoinGuildMasterIDUD)
		return FALSE;

	m_pstBattleEnemyGuildIDUD = m_pcsAgcmUIManager2->AddUserData("Guild_Battle_Enemy_GuildID", m_szBattleEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);
	if(!m_pstBattleEnemyGuildIDUD)
		return FALSE;

	m_pcsAgcmUIManager2->AddUserData("Guild_Battle_Type", GetUIGuildBattleOffer().GetBattleTypeString(), AGPMGUILD_MAX_GUILD_ID_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);
	m_pcsAgcmUIManager2->AddUserData("Guild_Battle_Duration", GetUIGuildBattleOffer().GetBattleTimeString(), AGPMGUILD_MAX_GUILD_ID_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);
	m_pcsAgcmUIManager2->AddUserData("Guild_Battle_Person", GetUIGuildBattleOffer().GetBattlePersonString(), AGPMGUILD_MAX_GUILD_ID_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);

//	2005.07.18. By SungHoon
	m_pstEnableIncreaseMaxMember = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Inc_Max_Member", &m_bEnableIncreaseMaxMember, sizeof(m_bEnableIncreaseMaxMember), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstEnableIncreaseMaxMember)
		return FALSE;
	m_pstEnableGuildBattle = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_GuildBattle", &m_bEnableGuildBattle, sizeof(m_bEnableIncreaseMaxMember), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstEnableGuildBattle)
		return FALSE;

//	2005.07.20. By SungHoon
	m_pstGuildListPage = m_pcsAgcmUIManager2->AddUserData("Guild_Guild_List_Page", &m_lUDGuildListPage, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstGuildListPage)
		return FALSE;

	m_pstGuildListMaxPage = m_pcsAgcmUIManager2->AddUserData("Guild_Guild_List_MaxPage", &m_lUDGuildListMaxPage, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstGuildListMaxPage)
		return FALSE;

	m_pstMemberListPage = m_pcsAgcmUIManager2->AddUserData("Guild_Member_List_Page", &m_lUDMemberListPage, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstMemberListPage)
		return FALSE;

	m_pstMemberListMaxPage = m_pcsAgcmUIManager2->AddUserData("Guild_Member_List_MaxPage", &m_lUDMemberListMaxPage, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstGuildListMaxPage)
		return FALSE;

//	2005.07.21. By SungHoon
	m_pstEnableLeaveGuildNotice = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Leave_Notice", &m_bEnableLeaveGuildNotice, sizeof(m_bEnableLeaveGuildNotice), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableLeaveGuildNotice)
		return FALSE;

	m_pstEnableLeaveGuildMemberList = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Leave_Member", &m_bEnableLeaveGuildMemberList, sizeof(m_bEnableLeaveGuildMemberList), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableLeaveGuildMemberList)
		return FALSE;

	m_pstGuildNoticeDetailInfo = m_pcsAgcmUIManager2->AddUserData("Guild_Notice_DetailInfo", &m_lGuildNoticeDetailInfo, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstGuildNoticeDetailInfo)
		return FALSE;

//	2005.07.22. By SungHoon
	m_pstEnableInviateParty = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Invite_Party", &m_bEnableInviteParty, sizeof(m_bEnableInviteParty), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableInviateParty)
		return FALSE;

	m_pstEnableChatWhisper = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Chat_Whisper", &m_bEnableChatWhisper, sizeof(m_bEnableChatWhisper), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableChatWhisper)
		return FALSE;

//	2005.08.24. By SungHoon
	m_pstEnableAllowGuildMemberList = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Allow_Button", &m_bEnableAllowGuildMemberList, sizeof(m_bEnableAllowGuildMemberList), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableAllowGuildMemberList)
		return FALSE;

	m_pstEnableAppoint = m_pcsAgcmUIManager2->AddUserData( "Guild_Enable_Appoint_Button" , &m_bEnableAppointButton , sizeof(m_bEnableAppointButton) , 1 , AGCDUI_USERDATA_TYPE_BOOL );
	if( !m_pstEnableAppoint )
		return FALSE;
	m_pstEnableSuccession = m_pcsAgcmUIManager2->AddUserData( "Guild_Enable_Succession_Button" , &m_bEnableSuccessionButton , sizeof(m_bEnableSuccessionButton) , 1 , AGCDUI_USERDATA_TYPE_BOOL );
	if( !m_pstEnableSuccession )
		return FALSE;

//	2005.08.31. By SungHoon
	m_pstEnableCreateGuild = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Create_Guild", &m_bEnableCreateGuild, sizeof(m_bEnableCreateGuild), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableCreateGuild )
		return FALSE;

	m_pstEnableGuildMaster = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Guild_Master", &m_bEnableGuildMaster, sizeof(m_bEnableGuildMaster), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableGuildMaster )
		return FALSE;

	m_pstEnableGuildCancle = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Guild_Cancel", &m_bEnableGuildCancle, sizeof(m_bEnableGuildCancle), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableGuildCancle )
		return FALSE;

	m_pstEnableRenameGuildID = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_RenameGuildID", &m_bEnableRenameGuildID, sizeof(m_bEnableRenameGuildID), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if(!m_pstEnableRenameGuildID )
		return FALSE;

//	2005.10.10. By SungHoon
	if (!(m_pcsUDGuildMarkGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGrid", &m_stGuildMarkGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkLayerIndex = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkLayerIndex", &m_lGuildMarkLayerIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;

//	2005.10.12. By SungHoon
	if (!(m_pcsUDGuildMarkColorGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkColorGrid", &m_stGuildMarkColorGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkPreviewLargeGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewLargeGrid", &m_stGuildMarkPreviewLargeGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkPreviewSmallGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewSmallGrid", &m_stGuildMarkPreviewSmallGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	if (!(m_pcsUDGuildMarkDetailInfo = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkDetailInfo", &m_lGuildMarkDetailInfo, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;

//	2005.10.14. By SungHoon
	if (!(m_pcsUDGuildMarkPreviewLargePatternGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewLargePatternGrid", &m_stGuildMarkPreviewLargePatternGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkPreviewSmallPatternGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewSmallPatternGrid", &m_stGuildMarkPreviewSmallPatternGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	if (!(m_pcsUDGuildMarkPreviewLargeSymbolGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewLargeSymbolGrid", &m_stGuildMarkPreviewLargeSymbolGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkPreviewSmallSymbolGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewSmallSymbolGrid", &m_stGuildMarkPreviewSmallSymbolGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

//	2005.10.20. By SungHoon
	m_pstEnableBuyGuildMark = m_pcsAgcmUIManager2->AddUserData("Guild_Enable_Buy_GuildMark", &m_bEnableBuyGuildMark, sizeof(m_bEnableBuyGuildMark), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstEnableBuyGuildMark)
		return FALSE;

	if (!(m_pcsUDGuildMarkPreviewBuyGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewBuyGrid", &m_stGuildMarkPreviewBuyGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkPreviewBuyPatternGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewBuyPatternGrid", &m_stGuildMarkPreviewBuyPatternGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkPreviewBuySymbolGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkPreviewBuySymbolGrid", &m_stGuildMarkPreviewBuySymbolGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

//	2005.10.24. By SungHoon
	if (!(m_pcsUDGuildMarkMainGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkMainGrid", &m_stGuildMarkMainGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkMainPatternGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkMainPatternGrid", &m_stGuildMarkMainPatternGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkMainSymbolGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkMainSymbolGrid", &m_stGuildMarkMainSymbolGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

//	2005.10.26. By SungHoon
	if (!(m_pcsUDGuildMarkGuildListGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGuildListGrid", &m_stGuildMarkGuildListGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkGuildListPatternGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGuildListPatternGrid", &m_stGuildMarkGuildListPatternGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkGuildListSymbolGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGuildListSymbolGrid", &m_stGuildMarkGuildListSymbolGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	if (!(m_pcsUDGuildMarkGuildDestroyGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGuildDestroyGrid", &m_stGuildMarkGuildDestroyGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkGuildDestroyPatternGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGuildDestroyPatternGrid", &m_stGuildMarkGuildDestroyPatternGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsUDGuildMarkGuildDestroySymbolGrid = m_pcsAgcmUIManager2->AddUserData("Guild_GuildMarkGuildDestroySymbolGrid", &m_stGuildMarkGuildDestroySymbolGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	// Relation 2006.07.24. steeple
	m_pstRelationGuildID = m_pcsAgcmUIManager2->AddUserData("Guild_RelationGuildID", m_szRelationGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);
	if(!m_pstRelationGuildID)
		return FALSE;
	m_pstRelationMasterID = m_pcsAgcmUIManager2->AddUserData("Guild_RelationMasterID", m_szRelationGuildID, AGPDCHARACTER_NAME_LENGTH, 1, AGCDUI_USERDATA_TYPE_STRING);
	if(!m_pstRelationMasterID)
		return FALSE;
	m_pstJointRequestBtnUD = m_pcsAgcmUIManager2->AddUserData("Guild_JointRequestBtnUD", &m_lJointReqestBtnUDIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstJointRequestBtnUD)
		return FALSE;
	m_pstHostileRequestBtnUD = m_pcsAgcmUIManager2->AddUserData("Guild_HostileRequestBtnUD", &m_lHostileRequestBtnUDIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstHostileRequestBtnUD)
		return FALSE;

	// 부마스터 임명 및 해제 2009. 09. 09	MK
	m_pstAppointAndCancelBtnID = m_pcsAgcmUIManager2->AddUserData( "Guild_Appoint_And_Cancel_Btn" , m_szAppointAndCancelBtnID , sizeof( CHAR ), AGPMGUILD_MAX_GUILD_ID_LENGTH + 1 , AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pstAppointAndCancelBtnID )
		return FALSE;

	// 연대관계 , 적대관계 신청 버튼
	m_pstGuildListRelationBtn = m_pcsAgcmUIManager2->AddUserData( "Guild_List_Relation_Btn" , &m_bGuildListRelationBtn , sizeof(BOOL) , 1 , AGCDUI_USERDATA_TYPE_BOOL );
	if( !m_pstGuildListRelationBtn )
		return FALSE;

	return TRUE;
}

BOOL AgcmUIGuild::AddBoolean()
{
	if(!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveGuildInviteMenu", CBIsActiveGuildInviteMemu, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

//	2005.10.10. By SungHoon
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsGuildMarkSelected", CBIsGuildMarkSelected, AGCDUI_USERDATA_TYPE_NONE))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsDummyDummy", CBIsGuildMarkSelected, AGCDUI_USERDATA_TYPE_NONE))
		return FALSE;

	return TRUE;
}


BOOL AgcmUIGuild::CBDisplayGuildText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayInviteGuild(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szJoinGuildID = (CHAR*)pData;

	sprintf(szDisplay, "%s", szJoinGuildID);
	
	return TRUE;
}


BOOL AgcmUIGuild::CBIsActiveGuildInviteMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgpdCharacter* pcsTargetCharacter = (AgpdCharacter*)pData;

	AgpdCharacter* pcsSelfCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;
	
	if(pcsSelfCharacter == pcsTargetCharacter)
		return FALSE;

	CHAR* szGuildID = pThis->m_pcsAgcmGuild->GetSelfGuildID();
	if(!szGuildID || strlen(szGuildID) == 0)
		return FALSE;

	// Master 가 아니면 나간다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(szGuildID, pcsSelfCharacter->m_szID) &&
		!pThis->m_pcsAgpmGuild->IsSubMaster( szGuildID, pcsSelfCharacter->m_szID) )
		return FALSE;

	return TRUE;
}


// From AgpmGuild
BOOL AgcmUIGuild::CBCreateComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szMasterID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szMasterID)
		return FALSE;

	if(strcmp(pThis->m_szInputGuildName, szGuildID) != 0)
		return TRUE;
	
	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	if(strcmp(pcsCharacter->m_szID, szMasterID) != 0)
		return FALSE;

	// 여기까지 오면 자기가 만들어서 뜬 것임.
	// UI (Create Complete) 를 띄어준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildCreateCompleteUI);

	return TRUE;
}

// From AgpmGuild
BOOL AgcmUIGuild::CBJoinRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = (PVOID*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];

	// Ok Cancel MessageBox 를 띄어준다. - Modal Dialog
	// 결과에 따라서 Join 또는 Join Reject 를 보낸다.
	strncpy(pThis->m_szJoinGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	strncpy(pThis->m_szJoinGuildMasterID, szMasterID, AGPACHARACTER_MAX_ID_STRING);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstJoinGuildIDUD);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstJoinGuildMasterIDUD);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenInviteConfirmUI);

	return TRUE;
}

// From AgpmGuild
BOOL AgcmUIGuild::CBJoinReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	// Message 를 띄어준다.
	// 별달리 처리할 건 없다.

	return TRUE;
}

// From AgpmGuild
// 데이터를 비교해서 자기 길드 작업일때만 처리한다.
BOOL AgcmUIGuild::CBJoin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szMemberID = (CHAR*)pCustData;

	AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	AgpdGuildMember* pcsMember = pThis->m_pcsAgpmGuild->GetMember(pcsGuild, szMemberID);
	if (pcsMember)
		pcsMember->m_lJoinDate = pThis->m_pcsAgpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;

	pcsGuild->m_Mutex.Release();

	// 자기길드가 아니면 나간다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
		return FALSE;

	// Member List UI 를 초기화 한다.
	pThis->SetCurrentPage(0);

	return TRUE;
}

// From AgpmGuild
// 자기 길드 비교는 UI 에 있는 데이터와 한다.
BOOL AgcmUIGuild::CBLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID		= (CHAR*)pData;
	AgcmUIGuild* pThis	= (AgcmUIGuild*)pClass;
	CHAR* szMemberID	= (CHAR*)pCustData;

	// 자기길드가 아니면 나간다.
	if(strcmp(pThis->m_szSelfGuildID, szGuildID) != 0)
		return FALSE;

	// Member List UI 를 초기화 한다.
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return TRUE;
	if (strcmp(pcsCharacter->m_szID, szMemberID)) pThis->RefreshMemberPageUI();		//	다른 길원이 탈퇴하면 리프레쉬 본인이 탈퇴하면
																					// CBDestroty 에서 리프레쉬 됨

	if( pThis->m_pcsAgpmGuild->IsSubMaster( szGuildID , szMemberID ) )
	{
		AgpdGuild*	pdGuild		=	pThis->m_pcsAgpmGuild->GetGuild( szGuildID );
		if( pdGuild )	pThis->m_pcsAgpmGuild->GetGuild( szGuildID )->SetSubMasterID( "" );	
	}

	return TRUE;
}

// From AgpmGuild
// 자기 길드 비교는 UI 에 있는 데이터와 한다.
BOOL AgcmUIGuild::CBForcedLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = (PVOID*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szMemberID = (CHAR*)pCustData;

	if(!ppvBuffer || !pThis || !szMemberID)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];

	// 자기 길드가 아니면 나간다.
	if(strcmp(pThis->m_szSelfGuildID, szGuildID) != 0)
		return FALSE;

	// Member List UI 를 초기화 한다.
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return TRUE;
	if (strcmp(pcsCharacter->m_szID, szMemberID)) pThis->RefreshMemberPageUI();		//	다른 길원이 탈퇴하면 리프레쉬 본인이 탈퇴하면
																					// CBDestroty 에서 리프레쉬 됨

	return TRUE;
}

// From AgpmGuild
// 자기 길드 비교는 UI 에 있는 데이터와 한다.
BOOL AgcmUIGuild::CBDestroy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	PVOID* ppvBuffer = (PVOID*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];

	// 자기 길드가 아니면 나간다.
	if(strcmp(pThis->m_szSelfGuildID, szGuildID) != 0)
		return FALSE;
	// Member UI Data 전체를 초기화한다.
//	pThis->InitMemberUIData();

	// GuildList UI Data 전체를 초기화한다.
//	pThis->InitGuildListUIData();
	
	// 모든 Guild Window 를 닫는다.		2005.08.25. By SungHoon 닫지 말자
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildNoticeUI);
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildGuildListUI);
	
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return TRUE;

	// Master 라면
	if(strcmp(szMasterID, pcsCharacter->m_szID) == 0)
	{
		strncpy(pThis->m_szInputGuildName, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		
		// 길드 해체 완료 UI 를 띄어준다.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildDestroyCompleteUI);
	}
	memset(pThis->m_szSelfGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	pThis->InitGuildUI();
	return TRUE;
}

// From AgpmGuild
// 캐릭터의 Attached Data 가 업뎃 됬을 때 불린다.
// 밖에서 Guild Lock 은 되어 있지 않고, Character Lock 만 되어 있다.
BOOL AgcmUIGuild::CBCharData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)// || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szGuildID = (CHAR*)pCustData;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 2005.10.11. steeple
	// 길드 정보 받을 때마다 투명체크를 해준다.
	if(pcsCharacter != pcsSelfCharacter && pThis->m_pcsAgpmCharacter->IsStatusTransparent(pcsCharacter))
	{
		//@{ Jaewon 20051101
		// Make it invisible immediately, i.e. no fade.
		pThis->m_pcsAgcmCharacter->UpdateTransparent(pcsCharacter, TRUE, 2000);
		//@} Jaewon
	}

	if (szGuildID == NULL)		//	길드 아이디 NULL일 경우
	{
		AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(pThis->m_szSelfGuildID);		//	자기 길드를 뒤져서
		if (pcsGuild)
		{
			BOOL bExistJoinMember = FALSE;
			if (pThis->m_pcsAgpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID)) bExistJoinMember = TRUE;	//	자기 길드라면
			if (bExistJoinMember)
			{
				BOOL bIsWinnerGuild = pcsGuild->m_lBRRanking == WINNER_GUILD_1STPLACE ? TRUE : FALSE;
				pThis->m_pcsAgpmGuild->SetCharAD(pcsCharacter, pThis->m_szSelfGuildID, pcsGuild->m_lGuildMarkTID, pcsGuild->m_lGuildMarkColor, bIsWinnerGuild, NULL);		//	원래 길드아이디로 다시 설정
				pcsGuild->m_Mutex.Release();
				return TRUE;
			}
			pcsGuild->m_Mutex.Release();
		}
		return TRUE;
	}
	// 자기 정보 없뎃이라면
	if(pcsCharacter == pcsSelfCharacter)
	{
		if(strlen(pThis->m_szSelfGuildID) == 0)	// 비어 있을 때만 세팅한다.
		{
			strncpy(pThis->m_szSelfGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		}
	}
	
	pThis->InitGuildUI();
	return TRUE;
}

// From AgpmGuild
// 데이터를 비교해서 자기 길드 작업일때만 처리한다.
// 밖에서 Guild Lock 되어서 넘어온다.
BOOL AgcmUIGuild::CBUpdateMemberStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	AgpdGuildMember* pcsMember = (AgpdGuildMember*)ppvBuffer[0];
	INT8 cOldStatus = ppvBuffer[1] ? *(INT8*)ppvBuffer[1] : (INT8)AGPMGUILD_MEMBER_STATUS_OFFLINE;

	// 자기 길드가 아니면 나간다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(pcsGuild->m_szID))
		return FALSE;

	// 시스템 메시지를 뿌린다. 흐음... 처음 접속할 때는 안뿌려야하는디...
	// 2005.04.04. steeple
	// Status 가 바뀌었을 때만 해주면 된다.
	if(pcsMember->m_cStatus != cOldStatus)
	{
		CHAR* szUIMessage = NULL;
		if(pcsMember->m_cStatus == AGPMGUILD_MEMBER_STATUS_ONLINE)
		{
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_MEMBER_ONLINE);
			if( pThis->GetUIGuildBattleMember().IsOpen() )
			{
				pThis->GetUIGuildBattleMember().Insert( pcsMember->m_szID, pcsMember->m_lLevel );
				pThis->GetUIGuildBattleMember().RefreshList();
			}
		}
		else
		{
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_MEMBER_OFFLINE);
			if( pThis->GetUIGuildBattleMember().IsOpen() )
			{
				pThis->GetUIGuildBattleMember().Delete( pcsMember->m_szID );
				pThis->GetUIGuildBattleMember().RefreshList();
			}
		}

		if(szUIMessage)
		{
			CHAR szMsg[255] = {0};
			sprintf(szMsg, szUIMessage, pcsMember->m_szID);
			SystemMessage.ProcessSystemMessage(szMsg);
		}
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstRecordUD);
	}


	AgcdGuildMemberListItr Itr = find( pThis->m_listAgcdGuildMember.begin(), pThis->m_listAgcdGuildMember.end(), string( pcsMember->m_szID ) );
	if( Itr != pThis->m_listAgcdGuildMember.end() )
	{
		(*Itr).m_cStatus	= pcsMember->m_cStatus;
		(*Itr).m_lLevel		= pcsMember->m_lLevel;
		(*Itr).m_lRank		= pcsMember->m_lRank;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstMemberIndexUD );

		pThis->m_listAgcdGuildMember.sort();
	}

	//sort를 해줘야 하지 않나? ㅎㅎ
	if (strcmp(pcsMember->m_szID, pThis->m_szSelectedMemberID) == 0)		//	선택된 사용자의 상태가 바뀌면 버튼 활성화 리프레쉬한다.
		pThis->RefreshGuildMemberButton();

	return TRUE;
}

// From AgpmGuild
// 밖에서 Guild Lock 되어서 넘어온다.
BOOL AgcmUIGuild::CBUpdateNotice(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 자기 길드가 아니면 나간다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(pcsGuild->m_szID))
		return FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstNoticeUD);

	// 채팅창에도 뿌려준다.
	// 채팅창에 뿌릴 때는 '___' 를 ' ' 로 바꿔준다.
	CHAR szMsg[AGPMGUILD_MAX_NOTICE_LENGTH+16+1];
	if(pcsGuild->m_szNotice && strlen(pcsGuild->m_szNotice) > 0)
	{
		CHAR szTmp[AGPMGUILD_MAX_NOTICE_LENGTH+1];
		CHAR szTmp2[AGPMGUILD_MAX_NOTICE_LENGTH+1];
		memset(szTmp, 0, sizeof(CHAR) * (AGPMGUILD_MAX_NOTICE_LENGTH+1));
		memset(szTmp2, 0, sizeof(CHAR) * (AGPMGUILD_MAX_NOTICE_LENGTH+1));

		// 당황스럽게도 strtok 를 하면 원본도 변경된다. -_-;; 뭐여 이게
		// 멀티쓰레드에서 문제 생길지도 모르니깐, 임시 본을 만들어서 그 놈으로 작업한다.
		strcpy(szTmp2, pcsGuild->m_szNotice);

		CHAR* szToken = NULL;
		szToken = strtok(szTmp2, pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
		while(szToken != NULL)
		{
			strcat(szTmp, szToken);
			strcat(szTmp, " ");
			
			szToken = strtok(NULL, pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
		}

		sprintf(szMsg, "%s %s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_NOTICE_HEADER), szTmp);
	}
	else
		sprintf(szMsg, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_NOTICE_HEADER));

	AgpdChatData	stChatData;
	memset(&stChatData, 0, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_GUILD_NOTICE;
	stChatData.szSenderName = pcsGuild->m_szMasterID;	// 일단 마스터로 한다.
	stChatData.szMessage = szMsg;

	stChatData.lMessageLength = strlen(stChatData.szMessage);
		
	pThis->m_pcsAgcmChatting->AddChatMessage(AGCMCHATTING_TYPE_GUILD, &stChatData);
	pThis->m_pcsAgcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, &stChatData, NULL);

	return TRUE;
}

// From AgpmGuild
// 메시지 코드에 맞게 시스템 메시지를 뿌려준다.
BOOL AgcmUIGuild::CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuildSystemMessage* pstSystemMessage = (AgpdGuildSystemMessage*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	BOOL bIsSystemMessage = TRUE;
	CHAR* szUIMessage = NULL;
	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(CHAR) * 255);
	UINT32 uColor = 0xFFFFFFFF;		//	2005.06.02. By SungHoon
	UINT32 uOffColor = 0xFFFFFFFF;
	CHAR *szOffColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_OFF_COLOR);
	if (szOffColor) uOffColor = atol(szOffColor);	//	거부 색
	UINT32 ulSystemColor = 0xFFFFFF33;
	CHAR *szSystemColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_SYSTEM_MESSAGE_COLOR);
	if(szSystemColor) ulSystemColor = atol(szSystemColor);	//	시스템 색

	switch(pstSystemMessage->m_lCode)
	{
		case AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_EXIST_GUILD_ID);
			break;

		case AGPMGUILD_SYSTEM_CODE_NEED_MORE_LEVEL:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_NEED_MORE_LEVEL);
			break;

		case AGPMGUILD_SYSTEM_CODE_NEED_MORE_MONEY:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_NEED_MORE_MONEY);
			if (szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_alData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_NEED_ITEM:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_NEED_ITEM);
			if (szUIMessage)
			{
				AgpdItemTemplate *	pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pstSystemMessage->m_alData[0]);

				if (pcsItemTemplate)
				{
					if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
						sprintf(szMessage, szUIMessage, pstSystemMessage->m_alData[1], pcsItemTemplate->m_szName);
					else
						sprintf(szMessage, szUIMessage, pcsItemTemplate->m_szName, pstSystemMessage->m_alData[1]);
				}

				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_USE_SPECIAL_CHAR:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_USE_SPECIAL_CHAR);
			break;

		case AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ALREADY_MEMBER);
			break;
			
		case AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER2:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ALREADY_MEMBER2);
			break;

		case AGPMGUILD_SYSTEM_CODE_MAX_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_MAX_MEMBER);
			break;

		case AGPMGUILD_SYSTEM_CODE_JOIN_FAIL:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_JOIN_FAIL);
			break;

		//case AGPMGUILD_SYSTEM_CODE_LEAVE_FAIL:
		//	szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_EXIST_GUILD_ID);
		//	break;

		//case AGPMGUILD_SYSTEM_CODE_NOT_MASTER:
		//	szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_EXIST_GUILD_ID);
		//	break;

		case AGPMGUILD_SYSTEM_CODE_INVALID_PASSWORD:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_INVALID_PASSWORD);
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_CREATE_COMPLETE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CREATE_COMPLETE);
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_DESTROY);
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_JOIN:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_JOIN);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REJECT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_JOIN_REJECT);
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_LEAVE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LEAVE);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_FORCED_LEAVE);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE2:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_FORCED_LEAVE2);
			break;


		///////////////////////////////////////////////////////////////////////////////////////
		// 여기서부터 배틀 관련
		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_EXIST_GUILD:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_EXIST_GUILD);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_MASTER_OFFLINE);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS:
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->GetUIGuildBattleOffer().m_lEventBattleRequestEnd );
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_REQUEST_STATUS);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_BOTH_MEMBER_COUNT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_BOTH_MEMBER_COUNT);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_alData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_ENOUGH_MEMBER_COUNT);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_alData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_alData[1]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2:
		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT2:
		case AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL2:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_REQUEST_STATUS2);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_REQUEST:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_REQUEST);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_REJECT_BY_OTHER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_REJECT_BY_OTHER);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);

				// 멤버 변수 비워준다.
				memset(pThis->m_szBattleEnemyGuildID, 0, sizeof(pThis->m_szBattleEnemyGuildID));
				pThis->GetUIGuildBattleOffer().BattleStringClear();

				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_PASSED_TIME:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_CANCEL_PASSED_TIME);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, (INT32)(pstSystemMessage->m_alData[0] / 60));
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_ACCEPT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_ACCEPT);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_WITHDRAW_UNTIL_START:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_CANNOT_REQUEST_GG);
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_REJECT_BY_OTHER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_CANCEL_REJECT_BY_OTHER);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_USABLE);
			break;

		//case AGPMGUILD_SYSTEM_CODE_BATTLE_DECLARE:
		//	szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_NOT_DECLARE);
		//	break;

		case AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE:			//	2005.06.02. By SungHoon
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GBATTLE_REFUSE);
			uColor = uOffColor;
			if(szUIMessage && pstSystemMessage->m_aszData[0])
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REFUSE:		//	2005.06.02. By SungHoon
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_REFUSE_USER);
			uColor = uOffColor;
			if(szUIMessage && pstSystemMessage->m_aszData[0])
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;
		case AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_GHELD_FOR_INCREASE_MAX :	//	2005.07.18. By SungHoon
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_NOT_ENOUGHT_GHELD_FOR_INCREASE_MAX);
			break;
		case AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX :	//	2005.07.18. By SungHoon
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX);
			break;
		case AGPMGUILD_SYSTEM_CODE_DESTROY_FAIL_TOO_EARLY :
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildDestroyFailTooEarly);
			break;
		case AGPMGUILD_SYSTEM_CODE_NO_EXIST_SEARCH_GUILD :
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_NO_EXIST_SEARCH_GUILD);
			uColor = ulSystemColor;
			break;
		case AGPMGUILD_SYSTEM_CODE_JOIN_REQUEST_SELF :				//	2005.08.03. By SungHoon
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOIN_REQUEST_SELF);
			uColor = ulSystemColor;
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;
		case AGPMGUILD_SYSTEM_CODE_LEAVE_REQUEST :
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_LEAVE_REQUEST_SELF);
			uColor = ulSystemColor;
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;
		case AGPMGUILD_SYSTEM_CODE_RENAME_USE_SPECIAL_CHAR :
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_RENAME_USE_SPECIAL_CHAR);
			bIsSystemMessage = FALSE;
			break;
		case AGPMGUILD_SYSTEM_CODE_RENAME_EXIST_GUILD_ID : 
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_RENAME_EXIST_GUILD_ID);
			bIsSystemMessage = FALSE;
			break;
		case AGPMGUILD_SYSTEM_CODE_RENAME_NOT_MASTER :
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_RENAME_NOT_MASTER);
			bIsSystemMessage = FALSE;
			break;
		case AGPMGUILD_SYSTEM_CODE_RENAME_IMPOSIBLE_GUILD_ID :
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_RENAME_IMPOSIBLE_GUILD_ID);
			bIsSystemMessage = FALSE;
			break;
//	2005.10.20. By SungHoon 추가
		case AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NO_EXIST_SKULL : 		//	해골이 없다.
			break;
		case AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NOT_ENOUGHT_SKULL :	//	해골이 부족하다.
			break;
		case AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NOT_ENOUGHT_GHELD :	//	겔드가 부족하다.
			break;
		case AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_NO_EXIST_BOTTOM :		//	바탕을 선택하지 않았다.
			break;
		case AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_DUPLICATE :			//	중복되는 길드마크가 있다.
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkDuplicate);
			break;

		case AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_DISABLE);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_NOT_LEADER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_NOT_LEADER);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_ALREADY_JOINT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_ALREADY_JOINT);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_MAX:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_MAX);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_MASTER_OFFLINE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_MASTER_OFFLINE);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_NOT_ENOUGH_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_NOT_ENOUGH_MEMBER);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, AGPMGUILD_MIN_JOINT_MEMBER_COUNT);
				szUIMessage = NULL;
			}
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_OHTER_NOT_ENOUGH_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_OTHER_NOT_ENOUGH_MEMBER);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_WAIT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_WAIT);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_REJECT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_REJECT);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_FAILURE);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_SUCCESS);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_SUCCESS:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_LEAVE_SUCCESS);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_OTHER_GUILD:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_LEAVE_OTHER_GUILD);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_DESTROY:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_DESTROY);
			break;
		case AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS2:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_SUCCESS2);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, pstSystemMessage->m_aszData[0]);
				szUIMessage = NULL;
			}
			break;

		case AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_DISABLE);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_ALREADY_HOSTILE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_ALREADY_HOSTILE);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_MAX:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_MAX);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_MASTER_OFFLINE);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_NOT_ENOUGH_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_NOT_ENOUGH_MEMBER);
			if(szUIMessage)
			{
				sprintf(szMessage, szUIMessage, AGPMGUILD_MIN_HOSTILE_MEMBER_COUNT);
				szUIMessage = NULL;
			}
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_OTHER_NOT_ENOUGH_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_OTHER_NOT_ENOUGH_MEMBER);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_WAIT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_WAIT);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_REJECT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_REJECT);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_FAILURE);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_SUCCESS:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_SUCCESS);
			break;

		case AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_DISABLE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_LEAVE_DISABLE);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_WAIT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_LEAVE_WAIT);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_REJECT:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_LEAVE_REJECT);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_FAILURE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_LEAVE_FAILURE);
			break;
		case AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_SUCCESS:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_LEAVE_SUCCESS);
			break;

		case AGPMGUILD_SYSTEM_CODE_ARCHLORD_DEFENSE_SUCCESS:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_ARCHLORD_DEFENSE_SUCCESS);
			break;
		case AGPMGUILD_SYSTEM_CODE_ARCHLORD_DEFENSE_FAILURE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_ARCHLORD_DEFENSE_FAILURE);
			break;
		case AGPMGUILD_SYSTEM_CODE_ARCHLORD_ATTACK_SUCCESS:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_ARCHLORD_ATTACK_SUCCESS);
			break;
		case AGPMGUILD_SYSTEM_CODE_ARCHLORD_ATTACK_FAILURE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_ARCHLORD_ATTACK_FAILURE);
			break;
		case AGPMGUILD_SYSTEM_CODE_WAREHOUSE_NOT_ENOUGH_MEMBER:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_STOREHOUCE_MEMBER_FAIL);
			break;
		case AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY_NOT_EMPTY_WAREHOUSE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_DESTORY_KEEP_FAIL);
			break;
		case AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION:
			szUIMessage = ClientStr().GetStr(STI_REFUSE_GUILD_RELATION);
			break;
	}

	if (bIsSystemMessage)
	{
		if(szUIMessage)
			SystemMessage.ProcessSystemMessage(szUIMessage, uColor);
		else if(strlen(szMessage) > 0)
			SystemMessage.ProcessSystemMessage(szMessage, uColor);
	}
	else		//	길드 이름 바꾸기 에러의 경우 메세지 박스 띄운다.	2005.08.31. By SungHoon
	{
		if (szUIMessage) pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(szUIMessage);
	}

	return TRUE;
}


// From AgcmEventGuild
// Event 에서 길드 UI 열라고 받는다.
BOOL AgcmUIGuild::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	ApdEvent* pcsEvent = (ApdEvent*)pCustData;

	pThis->m_stGuildMainOpenPos = pcsCharacter->m_stPos;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildMainUI);

	pThis->m_bIsGuildUIOpen	= TRUE;

	return TRUE;
}

BOOL AgcmUIGuild::CBEventGrantWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	ApdEvent* pcsEvent = (ApdEvent*)pCustData;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenWorldChampionshipUI);

	return TRUE;
}

// From AgcmCharacter
BOOL AgcmUIGuild::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if (!pThis->m_bIsGuildUIOpen)
		return TRUE;

	FLOAT fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stGuildMainOpenPos);

	if((INT32)fDistance < AGCMUIGUILD_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateConfirmUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateCompleteUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMainUI);

	pThis->m_bIsGuildUIOpen	= FALSE;

	return TRUE;
}


// From ... ??? 몰라 어딘지 -0-
// Character Menu 에서 버튼 클릭후 불린다.
BOOL AgcmUIGuild::CBSendJoinRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pcsSourceControl)
		return FALSE;

	if(!pcsSourceControl->m_pstShowUD ||
		!pcsSourceControl->m_pstShowUD->m_stUserData.m_pvData)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdCharacter* pcsTargetCharacter = (AgpdCharacter*)pcsSourceControl->m_pstShowUD->m_stUserData.m_pvData;

	return pThis->m_pcsAgcmGuild->SendJoinRequest(pcsTargetCharacter->m_szID);
}

// 초대 승낙 거절의 결과를 받는다.
// 승낙했으면 Join 패킷을 보내고, 거절했으면 거절을 알려줄까 말까...
BOOL AgcmUIGuild::CBEventReturnInviteConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(lTrueCancel == (INT32)TRUE)
	{
		pThis->m_pcsAgcmGuild->SendJoin(pThis->m_szJoinGuildID);
	}
	else
	{
		pThis->m_pcsAgcmGuild->SendJoinReject(pThis->m_szJoinGuildID, pThis->m_szJoinGuildMasterID);
	}

	return TRUE;
}

// 탈퇴 할거냐 안할거냐 결과를 받는다.
// 승낙했으면 탈퇴 패킷을 보내준다.
BOOL AgcmUIGuild::CBEventReturnLeaveConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(lTrueCancel == (INT32)TRUE)
	{
		pThis->m_pcsAgcmGuild->SendLeave();
	}

	return TRUE;
}

// 강퇴 시킬거냐 안시킬거냐 결과를 받는다.
// 승낙했으면 강퇴 패킷을 보내준다.
BOOL AgcmUIGuild::CBEventReturnForcedLeaveConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(lTrueCancel == (INT32)TRUE)
	{
		AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if(!pcsCharacter)
			return FALSE;

		if(!pThis->m_pcsAgpmGuild->IsMaster(pThis->m_pcsAgcmGuild->GetSelfGuildID(), pcsCharacter->m_szID))
			return FALSE;

		pThis->m_pcsAgcmGuild->SendForcedLeave(pThis->m_szSelectedMemberID);
	}

	return TRUE;
}

// 2005.04.18. steeple
// Chat Message 안보내려면 FALSE 로 리턴해주면 된다.
BOOL AgcmUIGuild::CBProcessChatCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szCommand = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CGetArg2* pcGetArg2 = (CGetArg2*)pCustData;

	// 커맨드 분석해서 적절히 처리해준다.
	if(strcmp(szCommand, AGCMUIGUILD_COMMAND_BATTLE_REQUEST) == 0)
	{
		//SystemMessage.ProcessSystemMessage("길드전 업데이트가 연기되었습니다.");
		//return FALSE;

		CHAR* szEnemyGuildID = NULL;
		CHAR* szType = NULL;
		INT32 lType;
		CHAR* szDuration = NULL;
		CHAR* szPerson = NULL;

		if( pcGetArg2->GetArgCount() == 4 )
		{
			szEnemyGuildID	= pcGetArg2->GetParam(1);
			szType			= pcGetArg2->GetParam(2);
			lType = atoi( szType );
			if( 3 == lType || 4 == lType )	return FALSE;
			szDuration		= pcGetArg2->GetParam(3);
			pThis->OpenBattleRequestConfirmUI(szEnemyGuildID, lType, atoi(szDuration), 0);
		}

		if( pcGetArg2->GetArgCount() == 5 )
		{
			szEnemyGuildID	= pcGetArg2->GetParam(1);
			szType			= pcGetArg2->GetParam(2);
			if( 0 == lType || 1 == lType || 2 == lType )	return FALSE;
			szDuration		= pcGetArg2->GetParam(3);
			szPerson		= pcGetArg2->GetParam(4);
			pThis->OpenBattleRequestConfirmUI(szEnemyGuildID, lType, atoi(szDuration), atoi(szPerson) );
		}
		
		return FALSE;
	}
	else if(strcmp(szCommand, AGCMUIGUILD_COMMAND_BATTLE_CANCEL) == 0)
	{
		pThis->OpenBattleCancelRequestConfirmUI();
		return FALSE;
	}
	else if(strcmp(szCommand, AGCMUIGUILD_COMMAND_BATTLE_WITHDRAW_ENG) == 0 ||
			strcmp(szCommand, AGCMUIGUILD_COMMAND_BATTLE_WITHDRAW) == 0)
	{
		pThis->OpenBattleWithdrawConfirmUI();
		return FALSE;
	}
	else if(strcmp(szCommand, AGCMUIGUILD_COMMAND_LEAVE_ENG) == 0 ||
			strcmp(szCommand, AGCMUIGUILD_COMMAND_LEAVE) == 0)
	{
		pThis->LeaveByCommand();
		return FALSE;
	}

	return TRUE;
}

// 채팅 메시지로 길드탈퇴를 날렸다. 2005.02.18. steeple
BOOL AgcmUIGuild::LeaveByCommand()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;
	
	if(strlen(m_pcsAgcmGuild->GetSelfGuildID()) == 0)
		return FALSE;

	// 마스터라면 무시
	if(m_pcsAgpmGuild->IsMaster(m_pcsAgcmGuild->GetSelfGuildID(), pcsCharacter->m_szID))
		return TRUE;

	// 탈퇴 확인 / 취소 UI 를 띄어준다.
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenLeaveConfirmUI);

	return TRUE;
}

/*
	2005.07.19. By SungHoon
	페이지 버튼의 제일 처음 버튼의 페이지를 계산한다.
*/
INT32 AgcmUIGuild::CalcFirstPage(INT32 lCurrentPage, INT32 lMaxPage)
{
	if (lMaxPage < AGCMUIGUILD_MAX_VIEW_PAGE) return 0;
	if (lCurrentPage <= AGCMUIGUILD_HALF_VIEW_PAGE) return ( 0 );

	if (lMaxPage - lCurrentPage < AGCMUIGUILD_HALF_VIEW_PAGE) return lMaxPage - AGCMUIGUILD_MAX_VIEW_PAGE;
	return ( lCurrentPage - AGCMUIGUILD_HALF_VIEW_PAGE );

}

/*
	2005.08.12. By SungHoon
	길드탈퇴 신청을 받았을 경우 불린다.
*/
BOOL AgcmUIGuild::CBReceiveLeaveRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR *szGuildID = (CHAR *)pData;
	CHAR *szMemberID = ( CHAR*)pCustData;


	AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	AgpdGuildMember *pcsGuildMember = pThis->m_pcsAgpmGuild->GetMember(pcsGuild, szMemberID);
	if(pcsGuildMember)
	{
		pcsGuildMember->m_lRank = AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST;
		pcsGuildMember->m_lJoinDate = pThis->m_pcsAgpmGuild->GetCurrentTimeStamp();
	}

	ApAutoLockCharacter Lock(pThis->m_pcsAgpmCharacter, szMemberID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;

	pThis->m_pcsAgpmGuild->EnumCallback(AGPMGUILD_CB_CHAR_DATA, pcsCharacter, szGuildID);
	return TRUE;
}

BOOL AgcmUIGuild::OpenGuildUI()
{
	// 열린 것으로 변경
	m_bGuildInfoUIOpened = TRUE;

	switch(m_cGuildInfoUILastTab)
	{
		case 0:	// Notice Tab
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenGuildNoticeUI);
			break;

		case 1:	// Member Tab
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenGuildMemberUI);
			break;

		case 2: // Guild List Tab	2005.07.06. By SungHoon
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenGuildGuildListUI);
			break;
	}

	return TRUE;
}

VOID AgcmUIGuild::InitGuildUI()
{
	switch(m_cGuildInfoUILastTab)
	{
		case 0:	// Notice Tab
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventInitGuildNoticeUI);
			break;

		case 1:	// Member Tab
			RefreshMemberPageUI();
			break;

	}
}

/*
	2005.09.05. By SungHoon
	길드원의 이름을 변경한다.
*/
BOOL AgcmUIGuild::CBGuildRenameCharID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;
	CHAR *szGuildID = (CHAR *)pData;
	PVOID *ppBuffer = (PVOID *)pCustData;

	if (!pThis->m_pcsAgcmGuild->GetSelfGuildID()) return FALSE;
	if (strcmp(szGuildID, pThis->m_pcsAgcmGuild->GetSelfGuildID())) return FALSE;

	pThis->RenameCharID((CHAR *)ppBuffer[0], (CHAR *)ppBuffer[1]);

	return TRUE;
}

/*
	자기 자신의 길드에서 szOldID 길원을 szNewID로 아이디를 바꾼다.
*/
BOOL AgcmUIGuild::RenameCharID(CHAR *szOldID, CHAR *szNewID)
{
	if (m_cGuildInfoUILastTab == 0 )
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventInitGuildNoticeUI);

	// 멤버가 현재 화면에 있을 때만 해주면 된다.
	AgcdGuildMemberListItr Itr = find( m_listAgcdGuildMember.begin(), m_listAgcdGuildMember.end(), szOldID );
	if( Itr != m_listAgcdGuildMember.end() )
	{
		sprintf( (*Itr).m_szMemberID, "%s", szNewID );
		if( m_cGuildInfoUILastTab == 1 )
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMemberIndexUD);
	}

	for ( INT16 i = 0 ; i < m_lVisibleGuildCount; i++ )
	{
		if (!strcmp(m_parrAgcdGuildList[i]->m_szMasterID, szOldID))
		{
			sprintf( m_parrAgcdGuildList[i]->m_szMasterID, "%s", szNewID );
			if ( m_cGuildInfoUILastTab == 2 )
				RefreshGuildList();

			break;
		}
	}
	return TRUE;
}

BOOL AgcmUIGuild::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgcdCharacter* pcsAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	if(pcsAgcdCharacter && pcsAgcdCharacter->m_bTransforming)
		return TRUE;

	ZeroMemory( pThis->m_szSelfGuildID		, sizeof( pThis->m_szSelfGuildID )		);
	ZeroMemory( pThis->m_szInputGuildName	, sizeof( pThis->m_szInputGuildName )	);
	ZeroMemory( pThis->m_szInputPassword	, sizeof( pThis->m_szInputPassword )	);

	ZeroMemory( pThis->m_szJoinGuildID		, sizeof( pThis->m_szJoinGuildID )		);
	ZeroMemory( pThis->m_szJoinGuildMasterID, sizeof( pThis->m_szJoinGuildMasterID ));

	ZeroMemory( pThis->m_szRelationGuildID	, sizeof( pThis->m_szRelationGuildID )	);
	ZeroMemory( pThis->m_szRelationMasterID	, sizeof( pThis->m_szRelationMasterID )	);

	pThis->m_JointDetailVector.clear();
	pThis->m_HostileDetailVector.clear();

	return TRUE;
}

CHAR*	AgcmUIGuild::GetMemberRankText( INT32 nRank )
{
	switch( nRank )
	{
	case AGPMGUILD_MEMBER_RANK_MASTER:		return m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_MASTER );
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:	return m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_SUBMASTER );
	case AGPMGUILD_MEMBER_RANK_NORMAL:		return m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_MEMBER );
	}

	return NULL;
}