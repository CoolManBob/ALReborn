#include "AgcmUIParty.h"

AgcmUIParty::AgcmUIParty()
{
	SetModuleName("AgcmUIParty");

	m_pcsUserDataMemberInfo		= NULL;
	m_pcsUserDataSelfInfo		= NULL;
	m_pcsUserDataBuffGrid		= NULL;

	m_pcsParty					= NULL;
	m_bpartyMemberSelect		= FALSE;

	m_lOperatorID				= AP_INVALID_CID;
	m_lTargetID					= AP_INVALID_CID;
	m_lBanishMemberID			= AP_INVALID_CID;

	ZeroMemory(m_stMemberInfo, sizeof(AgcmUIPartyMemberInfo) * (AGPMPARTY_MAX_PARTY_MEMBER - 1));
	ZeroMemory(&m_stSelfInfo, sizeof(AgcmUIPartyMemberInfo));

	ZeroMemory(m_stMemberBuffGrid, sizeof(AgpdGrid) * (AGPMPARTY_MAX_PARTY_MEMBER - 1));

//	m_eExpType					= AGPMPARTY_EXP_TYPE_BY_NONE;

	m_lBonusDamage				= 0;
	m_lBonusDefense				= 0;
	m_lBonusMaxHP				= 0;
	m_lBonusMaxMP				= 0;

	m_bNotifyBonusStats			= FALSE;
	m_pcsAgpmGrid				= NULL;

	m_bIsSelfUserMenuOn			= FALSE;
	m_bIsBuffIconOn				= TRUE;
	m_lMemberUserMenuOnIndex	= (-1);

	m_lEventPartyDelegationLeader			=	0;
	m_lEventPartyDelegationLeaderFailed		=	0;
	m_lEventPartyMemberLevelDiff			=	0;

	m_lEventMemberUserMenuOnBuffOff			=	0;		// 2005.06.16 By SungHoon
	m_lEventMemberUserMenuOffBuffOff		=	0;		// 2005.06.16 By SungHoon

	ZeroMemory(m_szRequestRecallOperatorID, sizeof(m_szRequestRecallOperatorID));
	m_lRequestRecallID	= AP_INVALID_CID;
	ZeroMemory(m_szInviterID, sizeof(m_szInviterID));
	
	m_lInvitedClock		= 0;
	m_ulRequestedClock	= 0;
}

AgcmUIParty::~AgcmUIParty()
{
}

BOOL AgcmUIParty::OnAddModule()
{
	m_pcsApmMap				= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgpmParty			= (AgpmParty *)			GetModule("AgpmParty");
	m_pcsAgcmParty			= (AgcmParty *)			GetModule("AgcmParty");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmUIMain			= (AgcmUIMain *)		GetModule("AgcmUIMain");
	m_pcsAgpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pcsAgcmSkill			= (AgcmSkill *)			GetModule("AgcmSkill");
	m_pcsAgpmGrid			= (AgpmGrid *)			GetModule("AgpmGrid");
	m_pcsApmMap				= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgcmChatting		= (AgcmChatting2 *)		GetModule("AgcmChatting2");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgcmPrivateTrade	= (AgcmPrivateTrade *)	GetModule("AgcmPrivateTrade");
	
	if (!m_pcsApmMap ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgpmParty ||
		!m_pcsAgcmParty ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmUIMain ||
		!m_pcsAgpmSkill ||
		!m_pcsAgcmSkill ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem ||
		!m_pcsAgcmPrivateTrade)
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorParty(CBUpdateFactorParty, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this))
		return FALSE;

	if (!m_pcsAgpmParty->SetCBAddParty(CBAddParty, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBRemoveParty(CBRemoveParty, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBAddMember(CBAddMember, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBRemoveMember(CBRemoveMember, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBChangeLeader(CBChangeLeader, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBResetEffectArea(CBResetEffectArea, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBUpdateExpType(CBUpdateExpType, this))
		return FALSE;
	if (!m_pcsAgpmParty->SetCBUpdateBonusStats(CBUpdateBonusStats, this))
		return FALSE;

	if (!m_pcsAgcmParty->SetCallbackReceiveInvite(CBReceiveInvite, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveReject(CBReceiveReject, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteAccept(CBReceiveInviteAccept, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveLeave(CBReceiveLeave, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveBanish(CBReceiveBanish, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedAlreadyOtherPartyMember(CBReceiveInviteFailedAlreadyOtherMember, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedAlreadyPartyMember(CBReceiveInviteFailedAlreadyPartyMember, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedPartyMemberIsFull(CBReceiveInviteFailedMemberIsFull, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedNotLeader(CBReceiveInviteFailedNotLeader, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveOperationFailed(CBReceiveOperationFailed, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackRequestRecall(CBRequestRecall, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackAcceptRecall(CBAcceptRecall, this))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackRejectRecall(CBRejectRecall, this))
		return FALSE;

//	2005.05.03	파티장 위임 성공과 실패에 대한 콜백 등록(PartyOptionUI에서 옮겨옴)
	if (!m_pcsAgcmParty->SetCallbackReceiveDelegationLeader(CBReceiveDelegationLeader, this))		// 2005.05.02 By SungHoon
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveDelegationLeaderFailed(CBReceiveDelegationLeaderFailed, this))		// 2005.05.02 By SungHoon
		return FALSE;

	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedLevelDiff(CBReceiveInvietFailedLevelDiff, this))		// 2005.05.02 By SungHoon
		return FALSE;

	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedRefuse(CBReceiveOperationFailedRefuse, this))		// 2005.06.01 By SungHoon
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;

//	2005.07.04 파티원메뉴(좌상단)를 선택할때 메인 유저 메뉴 닫기 By SungHoon
	if (!m_pcsAgcmCharacter->SetCallbackSelectTarget(CBSelectTarget, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackAddBuffedList(CBAddBuffedSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveBuffedList(CBRemoveBuffedSkill, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackUseItemByTID(CBUseItem, this))
		return FALSE;

//	2005.10.26. By SungHoon
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedMurdererOperator(CBReceiveInviteFailedMurdererSelf ,this ))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedMurdererTarget(CBReceiveInviteFailedMurdererTarget ,this ))
		return FALSE;
	if (!m_pcsAgcmParty->SetCallbackReceiveLeaveByMurderer(CBReceiveLeaveByMurderer ,this ))
		return FALSE;

	if (!AddEvent())
		return FALSE;

	if (!AddFunction())
		return FALSE;

	if (!AddDisplay())
		return FALSE;

	if (!AddUserData())
		return FALSE;

	if (!AddBoolean())
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::OnInit()
{
	for (int i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER - 1; ++i)
	{
		m_pcsAgpmGrid->Init(&m_stMemberBuffGrid[i], 1, 1, AGPMSKILL_MAX_SKILL_BUFF);
	}

	return TRUE;
}

BOOL AgcmUIParty::OnDestroy()
{
	for (int i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER - 1; ++i)
	{
		m_pcsAgpmGrid->Remove(&m_stMemberBuffGrid[i]);
	}

	return TRUE;
}

BOOL AgcmUIParty::AddUserData()
{
	m_pcsUserDataSelfInfo		= m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTY_USERDATA_SELF_INFO,
																   m_stMemberInfo,
																   sizeof(AgcmUIPartyMemberInfo),
																   1,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDataSelfInfo)
		return FALSE;

	m_pcsUserDataMemberInfo		= m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTY_USERDATA_MEMBER_INFO,
																   m_stMemberInfo,
																   sizeof(AgcmUIPartyMemberInfo),
																   AGPMPARTY_MAX_PARTY_MEMBER - 1,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDataMemberInfo)
		return FALSE;

	m_pcsUserDataBuffGrid		= m_pcsAgcmUIManager2->AddUserData("PartyMemberBuffGrid",
																   m_stMemberBuffGrid,
																   sizeof(AgpdGrid),
																   AGPMPARTY_MAX_PARTY_MEMBER - 1,
																   AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataBuffGrid)
		return FALSE;

	m_pcsUserDataRecallTargetName	= m_pcsAgcmUIManager2->AddUserData("PartyRecallTargetName",
																   m_szRequestRecallOperatorID,
																   sizeof(m_szRequestRecallOperatorID),
																   1,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDataRecallTargetName)
		return FALSE;

	m_pcsUserDataInviterName	= m_pcsAgcmUIManager2->AddUserData("PartyInviterName",
																   m_szInviterID,
																   sizeof(m_szInviterID),
																   1,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDataInviterName)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_INVITE_MEMBER, CBInviteMember, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_BANISH_MEMBER, CBBanishMember, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_LEAVE_PARTY, CBLeaveParty, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_INVITE_REJECT, CBInviteReject, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_INVITE_ACCEPT, CBInviteAccept, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_USE_SKILL_FOR_MEMBER, CBUseSkillForMember, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PartySelfSelect", CBSelfSelect, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PartyMemberSelect", CBMemberSelect, 0))
		return FALSE;
	
	if (!m_pcsAgcmUIManager2->AddFunction(this, "MoveCharacterMenu", CBMoveCharacterMenu, 1, "CharacterMenu UI"))
		return FALSE;

//	파티장 위임 버튼을 선택할 경우 불리워질 CallBack
	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTY_FUNCTION_DELEGATION_LEADER, CBDelegationLeader, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PartyAcceptRecall", CBAcceptRecall, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "PartyRejectRecall", CBRejectRecall, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PartyGetRecallRequestEdit", CBGetRecallRequestEdit, 1, "Edit Control"))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_MEMBER_NAME, AGCMUI_PARTY_DISPLAY_ID_MEMBER_NAME, CBDisplayMemberName, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_MEMBER_HP, AGCMUI_PARTY_DISPLAY_ID_MEMBER_HP, CBDisplayMemberHP, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_MEMBER_MAXHP, AGCMUI_PARTY_DISPLAY_ID_MEMBER_MAXHP, CBDisplayMemberMaxHP, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_MEMBER_LEVEL, AGCMUI_PARTY_DISPLAY_ID_MEMBER_LEVEL, CBDisplayMemberLevel, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_BONUS_DAMAGE, AGCMUI_PARTY_DISPLAY_ID_BONUS_DAMAGE, CBDisplayBonusDamage, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_BONUS_DEFENSE, AGCMUI_PARTY_DISPLAY_ID_BONUS_DEFENSE, CBDisplayBonusDefense, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_BONUS_MAXHP, AGCMUI_PARTY_DISPLAY_ID_BONUS_MAXHP, CBDisplayBonusMaxHP, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_BONUS_MAXMP, AGCMUI_PARTY_DISPLAY_ID_BONUS_MAXMP, CBDisplayBonusMaxMP, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCMUI_PARTY_DISPLAY_LEADER_NAME, AGCMUI_PARTY_DISPLAY_ID_LEADER_NAME, CBDisplayLeaderName, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyRecallTargetName", AGCMUI_PARTY_DISPLAY_ID_RECALL_TARGET_NAME, CBDisplayRecallTargetName, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyRequestRecall", AGCMUI_PARTY_DISPLAY_ID_REQUEST_RECALL, CBDisplayRequestRecall, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyInviter", 1, CBDisplayInviter, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::AddEvent()
{
	m_lEventOpenPartyUI	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_OPEN_PARTY_UI);
	if (m_lEventOpenPartyUI < 0)
		return FALSE;
	m_lEventClosePartyUI	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_CLOSE_PARTY_UI);
	if (m_lEventClosePartyUI < 0)
		return FALSE;

	m_lEventRecvInvite	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_INVITE, CBEventReturnRecvInvite, this);
	if (m_lEventRecvInvite < 0)
		return FALSE;
	m_lEventRecvReject	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_REJECT);
	if (m_lEventRecvReject < 0)
		return FALSE;
	m_lEventRecvInviteAccept	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_INVITE_ACCEPT);
	if (m_lEventRecvInviteAccept < 0)
		return FALSE;
	m_lEventRecvLeave	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_LEAVE);
	if (m_lEventRecvLeave < 0)
		return FALSE;
	m_lEventRecvBanish	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_BANISH);
	if (m_lEventRecvBanish < 0)
		return FALSE;
	m_lEventRecvAlreadyOtherMember	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_FAILED_OTHER_MEMBER);
	if (m_lEventRecvAlreadyOtherMember < 0)
		return FALSE;
	m_lEventRecvAlreadyPartyMember	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_FAILED_ALREADY_MEMBER);
	if (m_lEventRecvAlreadyPartyMember < 0)
		return FALSE;
	m_lEventRecvMemberIsFull	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_PARTY_MEMBER_FULL);
	if (m_lEventRecvMemberIsFull < 0)
		return FALSE;
	m_lEventRecvNotLeader	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_NOT_LEADER);
	if (m_lEventRecvNotLeader < 0)
		return FALSE;
	m_lEventRecvOperationFailed	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_RECEIVE_OPERATION_FAILED);
	if (m_lEventRecvOperationFailed < 0)
		return FALSE;

	m_lEventConformBanishMember	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_CONFORM_BANISH_MEMBER, CBEventReturnBanishMember, this);
	if (m_lEventConformBanishMember < 0)
		return FALSE;
	m_lEventConformLeaveParty	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_CONFORM_LEAVE_PARTY, CBEventReturnLeaveParty, this);
	if (m_lEventConformLeaveParty < 0)
		return FALSE;

	m_lEventSelfActiveEffectAreaKnight	= m_pcsAgcmUIManager2->AddEvent("PartySelfActiveEffectAreaKnight");
	if (m_lEventSelfActiveEffectAreaKnight < 0)
		return FALSE;
	m_lEventSelfActiveEffectAreaRanger	= m_pcsAgcmUIManager2->AddEvent("PartySelfActiveEffectAreaRanger");
	if (m_lEventSelfActiveEffectAreaRanger < 0)
		return FALSE;
	m_lEventSelfActiveEffectAreaMage	= m_pcsAgcmUIManager2->AddEvent("PartySelfActiveEffectAreaMage");
	if (m_lEventSelfActiveEffectAreaMage < 0)
		return FALSE;
	m_lEventSelfDisableEffectArea		= m_pcsAgcmUIManager2->AddEvent("PartySelfDisableEffectArea");
	if (m_lEventSelfDisableEffectArea < 0)
		return FALSE;
	m_lEventSelfSetDead					= m_pcsAgcmUIManager2->AddEvent("PartySelfSetDead");
	if (m_lEventSelfSetDead < 0)
		return FALSE;

	m_lEventSelfSetLeader	= m_pcsAgcmUIManager2->AddEvent("PartySelfSetLeader");
	if (m_lEventSelfSetLeader < 0)
		return FALSE;
	m_lEventSelfSetMember	= m_pcsAgcmUIManager2->AddEvent("PartySelfSetMember");
	if (m_lEventSelfSetMember < 0)
		return FALSE;

	m_lEventActiveEffectAreaKnight	= m_pcsAgcmUIManager2->AddEvent("PartyActiveEffectAreaKnight");
	if (m_lEventActiveEffectAreaKnight < 0)
		return FALSE;
	m_lEventActiveEffectAreaRanger	= m_pcsAgcmUIManager2->AddEvent("PartyActiveEffectAreaRanger");
	if (m_lEventActiveEffectAreaRanger < 0)
		return FALSE;
	m_lEventActiveEffectAreaMage	= m_pcsAgcmUIManager2->AddEvent("PartyActiveEffectAreaMage");
	if (m_lEventActiveEffectAreaMage < 0)
		return FALSE;
	m_lEventDisableEffectArea		= m_pcsAgcmUIManager2->AddEvent("PartyDisableEffectArea");
	if (m_lEventDisableEffectArea < 0)
		return FALSE;
	m_lEventSetDead					= m_pcsAgcmUIManager2->AddEvent("PartySetDead");
	if (m_lEventSetDead < 0)
		return FALSE;

	m_lEventSetLeader	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_SET_LEADER);
	if (m_lEventSetLeader < 0)
		return FALSE;
	m_lEventSetMember	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_SET_MEMBER);
	if (m_lEventSetMember < 0)
		return FALSE;

	m_lEventBonusDamage		= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_BONUS_DAMAGE);
	if (m_lEventBonusDamage < 0)
		return FALSE;
	m_lEventBonusDefense	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_BONUS_DEFENSE);
	if (m_lEventBonusDefense < 0)
		return FALSE;
	m_lEventBonusMaxPoint	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_BONUS_MAX_POINT);
	if (m_lEventBonusMaxPoint < 0)
		return FALSE;

	m_lEventChangeLeader	= m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_CHANGE_LEADER);
	if (m_lEventChangeLeader < 0)
		return FALSE;

	m_lEventSelfUserMenuOn	= m_pcsAgcmUIManager2->AddEvent("PartySelfUserMenuOn");
	if (m_lEventSelfUserMenuOn < 0)
		return FALSE;
	m_lEventSelfUserMenuOff	= m_pcsAgcmUIManager2->AddEvent("PartySelfUserMenuOff");
	if (m_lEventSelfUserMenuOff < 0)
		return FALSE;

	m_lEventMemberUserMenuOn	= m_pcsAgcmUIManager2->AddEvent("PartyMemberUserMenuOn");
	if (m_lEventMemberUserMenuOn < 0)
		return FALSE;
	m_lEventMemberUserMenuOff	= m_pcsAgcmUIManager2->AddEvent("PartyMemberUserMenuOff");
	if (m_lEventMemberUserMenuOff < 0)
		return FALSE;

	m_lEventOpenRequestRecallUI	= m_pcsAgcmUIManager2->AddEvent("PartyOpenRequestRecallUI");
	if (m_lEventOpenRequestRecallUI < 0)
		return FALSE;

/*
	2005.05.03	By SungHoon
	파티장 위임에 성공 실패시 출력될 메세지에 관한 이벤트 등록
*/
//	파티장 위임한 전 파티장이 볼 이벤트
	m_lEventPartyDelegationLeader			=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_DELEGATION_LEADER);
//	파티장 위임에 실패했을 경우 이벤트
	m_lEventPartyDelegationLeaderFailed		=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_DELEGATION_LEADER_FAILED);
//	파티 멤버 레벨 차이가 클 경우
	m_lEventPartyMemberLevelDiff			=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_INVITE_MEMBER_LEVEL_DIFF);

//	파티 버프 아이콘을 안보이게 하는 이벤트		2005.06.16 By SungHoon
	m_lEventMemberUserMenuOnBuffOff			=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_USER_MENU_ON_BUFF_OFF);
	m_lEventMemberUserMenuOffBuffOff		=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTY_EVENT_USER_MENU_OFF_BUFF_OFF);

	m_lEventAcceptRecall	= m_pcsAgcmUIManager2->AddEvent("PartyAcceptRecall");
	if (m_lEventAcceptRecall < 0)
		return FALSE;
	m_lEventRejectRecall	= m_pcsAgcmUIManager2->AddEvent("PartyRejectRecall");
	if (m_lEventRejectRecall < 0)
		return FALSE;
	m_lEventTargetAcceptRecall	= m_pcsAgcmUIManager2->AddEvent("PartyTargetAcceptRecall");
	if (m_lEventTargetAcceptRecall < 0)
		return FALSE;
	m_lEventTargetRejectRecall	= m_pcsAgcmUIManager2->AddEvent("PartyTargetRejectRecall");
	if (m_lEventTargetRejectRecall < 0)
		return FALSE;

	m_lEventUseRecallSuccess	= m_pcsAgcmUIManager2->AddEvent("PartyUseRecallSuccess");
	if (m_lEventUseRecallSuccess < 0)
		return FALSE;

	m_nEventPartySetSelfClassHK = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassHK" );
	if( m_nEventPartySetSelfClassHK < 0 ) return FALSE;

	m_nEventPartySetSelfClassHA = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassHA" );
	if( m_nEventPartySetSelfClassHA < 0 ) return FALSE;

	m_nEventPartySetSelfClassHW = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassHW" );
	if( m_nEventPartySetSelfClassHW < 0 ) return FALSE;

	m_nEventPartySetSelfClassOK = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassOK" );
	if( m_nEventPartySetSelfClassOK < 0 ) return FALSE;

	m_nEventPartySetSelfClassOA = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassOA" );
	if( m_nEventPartySetSelfClassOA < 0 ) return FALSE;

	m_nEventPartySetSelfClassOW = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassOW" );
	if( m_nEventPartySetSelfClassOW < 0 ) return FALSE;

	m_nEventPartySetSelfClassMK = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassMK" );
	if( m_nEventPartySetSelfClassMK < 0 ) return FALSE;

	m_nEventPartySetSelfClassMA = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassMA" );
	if( m_nEventPartySetSelfClassMA < 0 ) return FALSE;

	m_nEventPartySetSelfClassMW = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassMW" );
	if( m_nEventPartySetSelfClassMW < 0 ) return FALSE;

	m_nEventPartySetSelfClassDS = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassDS" );
	if( m_nEventPartySetSelfClassDS < 0 ) return FALSE;

	m_nEventPartySetSelfClassDK = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassDK" );
	if( m_nEventPartySetSelfClassDK < 0 ) return FALSE;

	m_nEventPartySetSelfClassDA = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassDA" );
	if( m_nEventPartySetSelfClassDA < 0 ) return FALSE;

	m_nEventPartySetSelfClassDW = m_pcsAgcmUIManager2->AddEvent( "PartyChageSelfClassDW" );
	if( m_nEventPartySetSelfClassDW < 0 ) return FALSE;


	m_nEventPartySetMemberClassHK = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassHK" );
	if( m_nEventPartySetMemberClassHK < 0 ) return FALSE;

	m_nEventPartySetMemberClassHA = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassHA" );
	if( m_nEventPartySetMemberClassHA < 0 ) return FALSE;

	m_nEventPartySetMemberClassHW = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassHW" );
	if( m_nEventPartySetMemberClassHW < 0 ) return FALSE;

	m_nEventPartySetMemberClassOK = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassOK" );
	if( m_nEventPartySetMemberClassOK < 0 ) return FALSE;

	m_nEventPartySetMemberClassOA = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassOA" );
	if( m_nEventPartySetMemberClassOA < 0 ) return FALSE;

	m_nEventPartySetMemberClassOW = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassOW" );
	if( m_nEventPartySetMemberClassOW < 0 ) return FALSE;

	m_nEventPartySetMemberClassMK = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassMK" );
	if( m_nEventPartySetMemberClassMK < 0 ) return FALSE;

	m_nEventPartySetMemberClassMA = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassMA" );
	if( m_nEventPartySetMemberClassMA < 0 ) return FALSE;

	m_nEventPartySetMemberClassMW = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassMW" );
	if( m_nEventPartySetMemberClassMW < 0 ) return FALSE;

	m_nEventPartySetMemberClassDS = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassDS" );
	if( m_nEventPartySetMemberClassDS < 0 ) return FALSE;

	m_nEventPartySetMemberClassDK = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassDK" );
	if( m_nEventPartySetMemberClassDK < 0 ) return FALSE;

	m_nEventPartySetMemberClassDA = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassDA" );
	if( m_nEventPartySetMemberClassDA < 0 ) return FALSE;

	m_nEventPartySetMemberClassDW = m_pcsAgcmUIManager2->AddEvent( "PartyChageMemberClassDW" );
	if( m_nEventPartySetMemberClassDW < 0 ) return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePartyMemu", CBIsActivePartyMemu, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePartyBanish", CBIsActivePartyBanish, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePartyLeave", CBIsActivePartyLeave, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

//	2005.05.03		By SungHoon
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePartyOptionUI", CBIsActivePartyOptionUI, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePartyDelegationLeader", CBIsActiveDelegationLeader, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::CBReceiveInvite(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	PVOID *			pvBuffer		= (PVOID *)			pCustData;
	INT32			lTargetID		= (INT32)			pvBuffer[0];
	CHAR			*szOperatorID	= (CHAR *)			pvBuffer[1];

	if (!szOperatorID)
		return TRUE;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;
	
	if (pThis->m_lInvitedClock > 0)
	{
		if (pThis->m_lInvitedClock + (1000 * 10) > pThis->GetClockCount())
			return TRUE;
	}

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	strcpy(pThis->m_szInviterID, szOperatorID);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataInviterName);

	if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
		pThis->m_lInvitedClock = pThis->GetClockCount();

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvInvite);
}

BOOL AgcmUIParty::CBReceiveReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_ulRequestedClock = 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvReject);
}

BOOL AgcmUIParty::CBReceiveInviteAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_ulRequestedClock = 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvInviteAccept);
}

BOOL AgcmUIParty::CBReceiveLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvLeave);
}

BOOL AgcmUIParty::CBReceiveBanish(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvBanish);
}

BOOL AgcmUIParty::CBReceiveInviteFailedAlreadyOtherMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_ulRequestedClock = 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvAlreadyOtherMember);
}

BOOL AgcmUIParty::CBReceiveInviteFailedAlreadyPartyMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_ulRequestedClock = 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvAlreadyPartyMember);
}

BOOL AgcmUIParty::CBReceiveInviteFailedMemberIsFull(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_ulRequestedClock = 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvMemberIsFull);
}

BOOL AgcmUIParty::CBReceiveInviteFailedNotLeader(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_ulRequestedClock = 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvNotLeader);
}

BOOL AgcmUIParty::CBReceiveInviteFailedMurdererSelf(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty	*pThis	= (AgcmUIParty *)	pClass;
	INT32	*plCID = (INT32 *)pData;	
	
	pThis->m_ulRequestedClock = 0;

	UINT32 lColor = 0xFFFF9900;
	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_MURDERE_ERROR_COLOR);
	if (szColor) lColor = atol(szColor);
	return (SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_MURDERE_ERROR_SELF), lColor));

}

BOOL AgcmUIParty::CBReceiveInviteFailedMurdererTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIParty	*pThis		= (AgcmUIParty *)pClass;
	INT32	*plCID = (INT32 *)pData;	
	CHAR	*szTargetID = (CHAR *)pCustData;	
	
	pThis->m_ulRequestedClock = 0;

	UINT32 lColor = 0xFFFF9900;
	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_MURDERE_ERROR_COLOR);
	if (szColor) lColor = atol(szColor);

	CHAR *szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_MURDERE_ERROR_TARGET);
	CHAR szBuffer[512] = { 0 };
	if (szMessage)
		wsprintf( szBuffer, szMessage, szTargetID );
	return (SystemMessage.ProcessSystemMessage(szBuffer, lColor ));
}

BOOL AgcmUIParty::CBReceiveLeaveByMurderer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32	*plCID = (INT32 *)pData;	

	UINT32 lColor = 0xFFFF9900;
	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_MURDERE_ERROR_COLOR);
	if (szColor) lColor = atol(szColor);
	return (SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_MURDERE_ERROR_LEAVE), lColor) );
}

BOOL AgcmUIParty::CBReceiveOperationFailed(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return TRUE;

	pThis->m_lOperatorID	= lOperatorID;
	pThis->m_lTargetID		= lTargetID;
	
	pThis->m_lInvitedClock		= 0;
	pThis->m_ulRequestedClock	= 0;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvOperationFailed);
}

BOOL AgcmUIParty::CBRequestRecall(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty	*pThis	= (AgcmUIParty *)	pClass;
	AgpdCharacter	*pcsOperator	= (AgpdCharacter *)	pData;

	CopyMemory(pThis->m_szRequestRecallOperatorID, pcsOperator->m_szID, sizeof(CHAR) * AGPDCHARACTER_MAX_ID_LENGTH);

	pThis->m_lRequestRecallID	= pcsOperator->m_lID;

	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataRecallTargetName, TRUE);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenRequestRecallUI);

	return TRUE;
}

BOOL AgcmUIParty::CBAcceptRecall(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty	*pThis	= (AgcmUIParty *)	pClass;
	AgpdCharacter	*pcsOperator	= (AgpdCharacter *)	pData;

	CopyMemory(pThis->m_szRequestRecallOperatorID, pcsOperator->m_szID, sizeof(CHAR) * AGPDCHARACTER_MAX_ID_LENGTH);

	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataRecallTargetName, TRUE);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTargetAcceptRecall);

	return TRUE;
}

BOOL AgcmUIParty::CBRejectRecall(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty	*pThis	= (AgcmUIParty *)	pClass;
	AgpdCharacter	*pcsOperator	= (AgpdCharacter *)	pData;

	CopyMemory(pThis->m_szRequestRecallOperatorID, pcsOperator->m_szID, sizeof(CHAR) * AGPDCHARACTER_MAX_ID_LENGTH);

	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataRecallTargetName);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTargetRejectRecall);

	return TRUE;
}

BOOL AgcmUIParty::CBAddParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	AgpdParty		*pcsParty		= (AgpdParty *)		pData;

	// pcsParty에 SelfCharacter가 멤버로 들어있는지 본다.
	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (!pThis->m_pcsAgpmParty->IsMember(pcsParty, pcsSelfCharacter->m_lID))
		return FALSE;

	pThis->m_bIsSelfUserMenuOn		= FALSE;
	pThis->m_lMemberUserMenuOnIndex	= (-1);

	// pcsParty에 대한 정보들을 이 모듈에서 사용하는 UserData들에 세팅한후 refresh시킨다.
	if (!pThis->SetMemberInfo(pcsParty))
		return FALSE;

//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck3);
	SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_CREATE));
	SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_CREATE_2));

	pThis->CheckMemberTransparent(pcsParty);

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenPartyUI);
}

BOOL AgcmUIParty::CBRemoveParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
	AgpdParty		*pcsParty			= (AgpdParty *)		pData;

	// 2005.02.11. steeple
	// RemoveParty 가 날라올 때 자기캐릭터가 해당 파티의 멤버라면 m_pcsParty 를 NULL 로 해준다.
	if (pThis->m_pcsAgpmParty->IsMember(pcsParty, pThis->m_pcsAgcmCharacter->GetSelfCID()))
		pThis->m_pcsParty = NULL;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventClosePartyUI);
}

BOOL AgcmUIParty::CBAddMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
	AgpdParty		*pcsParty			= (AgpdParty *)		pData;
	AgpdCharacter	*pcsAddMember		= (AgpdCharacter *)	pCustData;

	pThis->SetMemberInfo(pcsParty);

	// 2005.10.11. steeple
	// 투명 때문에 약간 수정.
	if(pThis->m_pcsAgpmCharacter->IsStatusTransparent(pcsAddMember))
	{
		//@{ Jaewon 20051101
		// Make it invisible immediately, i.e. no fade.
		pThis->m_pcsAgcmCharacter->UpdateTransparent(pcsAddMember, TRUE, 2000);
		//@} Jaewon
	}
	
	return TRUE;
}

BOOL AgcmUIParty::CBRemoveMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
	AgpdParty		*pcsParty			= (AgpdParty *)		pData;

	AgpdCharacter	*pcsRemoveMember	= (AgpdCharacter *)	pCustData;
	if (pcsRemoveMember->m_lID == pThis->m_pcsAgcmCharacter->GetSelfCID())
	{
		// 2005.10.11. steeple
		pThis->CheckMemberTransparent(pcsParty);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventClosePartyUI);
		pThis->m_pcsParty = NULL;
	}
	else
	{
		pThis->SetMemberInfo(pcsParty);

		// 2005.10.11. steeple
		if(pThis->m_pcsAgpmCharacter->IsStatusTransparent(pcsRemoveMember))
			//@{ Jaewon 20051101
			// Make it invisible immediately, i.e. no fade.
			pThis->m_pcsAgcmCharacter->UpdateTransparent(pcsRemoveMember, TRUE, 2000);
			//@} Jaewon
	}
	
	pThis->CloseUserMenu();

	return TRUE;
}

BOOL AgcmUIParty::CBChangeLeader(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmUIParty::CBResetEffectArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;

	if(!pThis->m_pcsParty)
		return TRUE;

	INT32	lProcessMember	= 0;

	AgpdCharacter	*pcsLeader		= NULL;

	for (int i = 0; i < pThis->m_pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		if (i == 0)
			pcsLeader	= pcsMember;

		if (pcsMember == pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		{
			if (pcsMember->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfSetDead, 0, TRUE);
			}
			else if (pThis->m_pcsParty->m_bIsInEffectArea[i] && pcsLeader)
			{
				INT32	lLeaderClass	= pThis->m_pcsAgpmFactors->GetClass(&pcsLeader->m_pcsCharacterTemplate->m_csFactor);

				switch (lLeaderClass) {
				case AUCHARCLASS_TYPE_KNIGHT:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfActiveEffectAreaKnight, 0, TRUE);
					break;

				case AUCHARCLASS_TYPE_RANGER:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfActiveEffectAreaRanger, 0, TRUE);
					break;

				case AUCHARCLASS_TYPE_MAGE:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfActiveEffectAreaMage, 0, TRUE);
					break;
				}
			}
			else
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfDisableEffectArea, 0, TRUE);
		}
		else
		{
			if (pcsMember->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetDead, lProcessMember, TRUE);
			}
			else if (pThis->m_pcsParty->m_bIsInEffectArea[i] && pcsLeader)
			{
				INT32	lLeaderClass	= pThis->m_pcsAgpmFactors->GetClass(&pcsLeader->m_pcsCharacterTemplate->m_csFactor);

				switch (lLeaderClass) {
				case AUCHARCLASS_TYPE_KNIGHT:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveEffectAreaKnight, lProcessMember, TRUE);
					break;

				case AUCHARCLASS_TYPE_RANGER:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveEffectAreaRanger, lProcessMember, TRUE);
					break;

				case AUCHARCLASS_TYPE_MAGE:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveEffectAreaMage, lProcessMember, TRUE);
					break;
				}
			}
			else
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventDisableEffectArea, lProcessMember, TRUE);

			++lProcessMember;
		}
	}

	if (!pThis->m_bNotifyBonusStats)
		pThis->ThrowBonusEvent();

	return TRUE;

	//return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetListControl);
}

BOOL AgcmUIParty::CBUpdateExpType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
	AgpdParty		*pcsParty			= (AgpdParty *)		pData;

//	switch (pcsParty->m_eCalcExpType) {
//	case AGPMPARTY_EXP_TYPE_BY_DAMAGE:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck1);
//		break;
//
//	case AGPMPARTY_EXP_TYPE_BY_LEVEL:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck2);
//		break;
//
//	case AGPMPARTY_EXP_TYPE_BY_COMPLEX:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck3);
//		break;
//	}

	return TRUE;
}

BOOL AgcmUIParty::CBUpdateBonusStats(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
	AgpdParty		*pcsParty			= (AgpdParty *)		pData;
	PVOID			*ppvBuffer			= (PVOID *)			pCustData;

	pThis->m_lBonusDamage		= (INT32)			ppvBuffer[0];
	pThis->m_lBonusDefense		= (INT32)			ppvBuffer[1];
	pThis->m_lBonusMaxHP		= (INT32)			ppvBuffer[2];
	pThis->m_lBonusMaxMP		= (INT32)			ppvBuffer[3];

	if (pThis->m_lBonusDamage == 0 && pThis->m_lBonusDefense == 0 && pThis->m_lBonusMaxHP == 0 && pThis->m_lBonusMaxMP == 0)
	{
		DWORD dwColor = 0xFF7BFFCD;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_TEXT_COLOR);
		if (szColor) dwColor = atol(szColor);

		CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_ADD_NONE);
		if (szUIMessage) SystemMessage.ProcessSystemMessage(szUIMessage, dwColor);
		return TRUE;
	}

	pThis->m_bNotifyBonusStats	= FALSE;

	return pThis->ThrowBonusEvent();
}

BOOL AgcmUIParty::CBUpdateFactorParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
	AgpdFactor		*pcsFactor			= (AgpdFactor *)	pData;

	AgpdFactorOwner	*pcsFactorOwner		= (AgpdFactorOwner *) pThis->m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER);
	if (!pcsFactorOwner || pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_ID] == AP_INVALID_CID || !pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER])
		return TRUE;

	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER];

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdParty		*pcsSelfParty		= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (!pcsSelfParty)
		return TRUE;

	AgpdParty		*pcsParty			= pThis->m_pcsAgpmParty->GetParty(pcsCharacter);
	if (!pcsParty)
		return TRUE;

	if (pcsSelfParty == pcsParty)
		pThis->SetMemberInfo(pcsParty);

	return TRUE;
}

BOOL AgcmUIParty::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty			*pThis				= (AgcmUIParty *)			pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;

	if (pThis->m_pcsAgcmCharacter->IsCharacterTransforming(pcsCharacter))
		return TRUE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventClosePartyUI);
}

BOOL AgcmUIParty::CBDisplayMemberName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		lID != AGCMUI_PARTY_DISPLAY_ID_MEMBER_NAME ||
		!szDisplay)
		return FALSE;

	AgcmUIPartyMemberInfo	*pstMemberInfo	= (AgcmUIPartyMemberInfo *) pData;

	if (pstMemberInfo->szMemberName && pstMemberInfo->szMemberName[0])
		sprintf(szDisplay, "%s", pstMemberInfo->szMemberName);
	else
		sprintf(szDisplay, " ");

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayMemberHP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		lID != AGCMUI_PARTY_DISPLAY_ID_MEMBER_HP ||
		!plValue)
		return FALSE;

	AgcmUIPartyMemberInfo	*pstMemberInfo	= (AgcmUIPartyMemberInfo *) pData;

	*plValue	= pstMemberInfo->lMemberHP;

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayMemberMaxHP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		lID != AGCMUI_PARTY_DISPLAY_ID_MEMBER_MAXHP ||
		!plValue)
		return FALSE;

	AgcmUIPartyMemberInfo	*pstMemberInfo	= (AgcmUIPartyMemberInfo *) pData;

	*plValue	= pstMemberInfo->lMemberMaxHP;

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayMemberLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		lID != AGCMUI_PARTY_DISPLAY_ID_MEMBER_LEVEL ||
		!szDisplay)
		return FALSE;

	AgcmUIPartyMemberInfo	*pstMemberInfo	= (AgcmUIPartyMemberInfo *) pData;

	sprintf(szDisplay, "Lv%d", pstMemberInfo->lMemberLevel);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayBonusDamage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lBonusDamage);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayBonusDefense(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lBonusDefense);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayBonusMaxHP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lBonusMaxHP);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayBonusMaxMP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lBonusMaxMP);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayLeaderName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	if(!pThis->m_pcsParty)
		return TRUE;

	AgpdCharacter	*pcsLeader	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[0]);

	if (pcsLeader)
		sprintf(szDisplay, "%s", pcsLeader->m_szID);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayRecallTargetName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, "%s", pThis->m_szRequestRecallOperatorID);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayRequestRecall(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_REQUEST_RECALL), pThis->m_szRequestRecallOperatorID);

	return TRUE;
}

BOOL AgcmUIParty::CBDisplayInviter(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;

	sprintf(szDisplay, "%s", pThis->m_szInviterID);

	return TRUE;
}

BOOL AgcmUIParty::CBInviteMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;

	if (!pcsSourceControl->m_pstUserData ||
		!pcsSourceControl->m_pstUserData->m_stUserData.m_pvData)
		return FALSE;

	AgpdCharacter			*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdCharacter			*pcsTargetCharacter		= (AgpdCharacter *)			pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;

	if (pThis->m_ulRequestedClock > 0)
	{
		if (pThis->m_ulRequestedClock + (1000 * 10) > pThis->GetClockCount())
			return TRUE;
	}

	if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
		pThis->m_ulRequestedClock = pThis->GetClockCount();

	return pThis->m_pcsAgcmParty->SendPacketInvite(pcsSelfCharacter->m_lID, pcsTargetCharacter->m_lID);

	/*
	if (pThis->m_pcsAgpmFactors->IsInRange(&pcsSelfCharacter->m_stPos, &pcsTargetCharacter->m_stPos, 400, 0, NULL))
	{
		return pThis->m_pcsAgcmParty->SendPacketInvite(pcsSelfCharacter->m_lID, pcsTargetCharacter->m_lID);
	}
	else
	{
		return SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_NOT_IN_RANGE));
	}
	*/

	return TRUE;
}

BOOL AgcmUIParty::CBBanishMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;

	if (!pcsSourceControl->m_pstUserData ||
		!pcsSourceControl->m_pstUserData->m_stUserData.m_pvData)
		return FALSE;

	AgpdCharacter			*pcsTargetCharacter		= (AgpdCharacter *)			pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;

	AgpdCharacter			*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (!pThis->m_pcsParty)
		return FALSE;

	if (pThis->m_pcsParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID ||
		pcsTargetCharacter == pcsSelfCharacter)
		return FALSE;

	pThis->m_lBanishMemberID	= pcsTargetCharacter->m_lID;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConformBanishMember);
}

BOOL AgcmUIParty::CBLeaveParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConformLeaveParty);
}

BOOL AgcmUIParty::CBInviteReject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;
	
	pThis->m_lInvitedClock = 0;

	return pThis->m_pcsAgcmParty->SendPacketReject(pThis->m_lOperatorID, pThis->m_lTargetID);
}

BOOL AgcmUIParty::CBInviteAccept(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;
	
	pThis->m_lInvitedClock = 0;

	return pThis->m_pcsAgcmParty->SendPacketAccept(pThis->m_lOperatorID, pThis->m_lTargetID);
}

BOOL AgcmUIParty::CBUseSkillForMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
//	if (!pClass || !pcsSourceControl)
//		return FALSE;
//
//	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;
//
//	if (!pcsSourceControl->m_pstUserData ||
//		pcsSourceControl->m_pstUserData->m_eType != AGCDUI_USERDATA_TYPE_STRING ||
//		!pcsSourceControl->m_pstUserData->m_stUserData.m_pvData)
//		return FALSE;
//
//	AgcmUIPartyMemberInfo	*pcsMemberInfo			= (AgcmUIPartyMemberInfo *)	pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;
//
//	AgpdGridItem			*pcsSkillGridItem		= pThis->m_pcsAgcmUIMain->GetSkillGridItem();
//
//	if (pcsSkillGridItem && pcsSkillGridItem->m_eType == AGPDGRID_ITEM_TYPE_SKILL)
//	{
//		if (pThis->m_pcsAgcmSkill)
//			pThis->m_pcsAgcmSkill->CastSkill(pcsSkillGridItem->m_lItemID, pcsMemberInfo[pcsSourceControl->m_lUserDataIndex].lMemberID, FALSE);
//	}

	return TRUE;
}

BOOL AgcmUIParty::CBSelfSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)		pClass;

	pThis->CloseUserMenu();

	if (pThis->m_bIsSelfUserMenuOn)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfUserMenuOff, 0, TRUE);
		pThis->m_bIsSelfUserMenuOn		= FALSE;
	}
	else
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfUserMenuOn, 0, TRUE);
		pThis->m_bIsSelfUserMenuOn		= TRUE;
	}

//	pThis->m_pcsAgcmCharacter->SelectTarget(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsAgcmCharacter->GetSelfCID());

	return TRUE;
}

/*
	2005.06.16 Modify SungHoon
	버프 아이콘을 안보이게 설정했을 경우 다른 Event Call 하게 수정
*/
BOOL AgcmUIParty::CBMemberSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)		pClass;
	AgcdUIControl* pcdUserMenuControl = ( AgcdUIControl* )pData1;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfUserMenuOff, 0, TRUE);
	pThis->m_bIsSelfUserMenuOn		= FALSE;

	pThis->CloseUserMenu();

	if (pThis->m_lMemberUserMenuOnIndex != pcsSourceControl->m_lUserDataIndex)
	{
		if (pThis->m_bIsBuffIconOn)
            pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMemberUserMenuOn, pcsSourceControl->m_lUserDataIndex, TRUE);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMemberUserMenuOnBuffOff, pcsSourceControl->m_lUserDataIndex, TRUE);

		pThis->m_lMemberUserMenuOnIndex	= pcsSourceControl->m_lUserDataIndex;
	}

	pThis->m_bpartyMemberSelect = TRUE;
	pThis->m_pcsAgcmCharacter->SelectTarget(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_stMemberInfo[pcsSourceControl->m_lUserDataIndex].lMemberID);
	pThis->m_bpartyMemberSelect = FALSE;

	if( pcdUserMenuControl )
	{
		pcdUserMenuControl->m_pcsBase->y += pcsSourceControl->m_lUserDataIndex * pcsSourceControl->m_pcsBase->h;
	}

	return TRUE;
}

BOOL AgcmUIParty::CBMoveCharacterMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pcsSourceControl)
		return FALSE;

	if (((ApBase *) pData1)->m_eType != APBASE_TYPE_UI)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	AgcdUI			*pcsTargetUI	= (AgcdUI *)		pData1;
	INT32			lX				= -21;
	INT32			lY				= pcsSourceControl->m_pcsBase->h + 12 + ( pcsSourceControl->m_lUserDataIndex * pcsSourceControl->m_pcsBase->h );

	pcsSourceControl->m_pcsBase->ClientToScreen(&lX, &lY);

	pcsTargetUI->m_pcsUIWindow->MoveWindow(lX,
										   lY,
										   pcsTargetUI->m_pcsUIWindow->w,
										   pcsTargetUI->m_pcsUIWindow->h);

	return TRUE;
}

//BOOL AgcmUIParty::CBGetListControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pData1)
//		return FALSE;
//
//	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;
//	AgcdUIControl			*pcsControl				= (AgcdUIControl *)			pData1;
//
//	AcUIList				*pcsList				= (AcUIList *) pcsControl->m_pcsBase;
//
//	for (int i = 0; i < pThis->m_pcsUserDataMemberInfo->m_stUserData.m_lCount; ++i)
//	{
//		AcUIListItem			*pcsListItem			= pcsList->GetListItemPointer_Index(i);
//
//		for (INT32 lStatusIndex = 0; lStatusIndex < 2; ++lStatusIndex)
//		{
//			stStatusInfo	*pstStatusInfo	= pcsListItem->GetStatusInfo_Index(lStatusIndex);
//
//			if (strcmp(pstStatusInfo->m_szStatusName, "On"))
//			{
//				if (pThis->m_stMemberInfo[i].bIsInEffectArea)
//				{
//					pcsListItem->SetStatus(lStatusIndex);
//
//					continue;
//				}
//			}
//			else if (strcmp(pstStatusInfo->m_szStatusName, "Off"))
//			{
//				if (!pThis->m_stMemberInfo[i].bIsInEffectArea)
//				{
//					pcsListItem->SetStatus(lStatusIndex);
//				}
//			}
//		}
//	}
//
//	return TRUE;
//}
//
//BOOL AgcmUIParty::CBSetCheck1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass)
//		return FALSE;
//
//	AgcmUIParty		*pThis				= (AgcmUIParty *)	pClass;
//
//	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	// 파티 리더인지 본다.
//	if (pThis->m_pcsParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID)
//		return TRUE;
//
//	// 서버로 이 정보를 보낸다.
//	if (pThis->m_eExpType != AGPMPARTY_EXP_TYPE_BY_DAMAGE)
//	{
//		pThis->m_eExpType	= AGPMPARTY_EXP_TYPE_BY_DAMAGE;
//
//		pThis->m_pcsAgcmParty->SendPacketUpdateExpType(pThis->m_pcsParty, AGPMPARTY_EXP_TYPE_BY_DAMAGE);
//
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck1);
//	}
//
//	return TRUE;
//}
//
//BOOL AgcmUIParty::CBSetCheck2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass)
//		return FALSE;
//
//	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
//
//	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	// 파티 리더인지 본다.
//	if (pThis->m_pcsParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID)
//		return TRUE;
//
//	// 서버로 이 정보를 보낸다.
//	if (pThis->m_eExpType != AGPMPARTY_EXP_TYPE_BY_LEVEL)
//	{
//		pThis->m_eExpType	= AGPMPARTY_EXP_TYPE_BY_LEVEL;
//
//		pThis->m_pcsAgcmParty->SendPacketUpdateExpType(pThis->m_pcsParty, AGPMPARTY_EXP_TYPE_BY_LEVEL);
//
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck2);
//	}
//
//	return TRUE;
//}
//
//BOOL AgcmUIParty::CBSetCheck3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass)
//		return FALSE;
//
//	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
//
//	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	// 파티 리더인지 본다.
//	if (pThis->m_pcsParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID)
//		return TRUE;
//
//	// 서버로 이 정보를 보낸다.
//	if (pThis->m_eExpType != AGPMPARTY_EXP_TYPE_BY_COMPLEX)
//	{
//		pThis->m_eExpType	= AGPMPARTY_EXP_TYPE_BY_COMPLEX;
//
//		pThis->m_pcsAgcmParty->SendPacketUpdateExpType(pThis->m_pcsParty, AGPMPARTY_EXP_TYPE_BY_COMPLEX);
//
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetCheck3);
//	}
//
//	return TRUE;
//}

BOOL AgcmUIParty::SetMemberInfo(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	INT32	lHP		= 0;
	INT32	lMaxHP	= 0;

	AgpdCharacter		*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	m_pcsParty		= pcsParty;

	ZeroMemory(m_stMemberInfo, sizeof(AgcmUIPartyMemberInfo) * (AGPMPARTY_MAX_PARTY_MEMBER - 1));

	int		nNumSetMemberInfo = 0;
	for (int i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		AgcmUIPartyMemberInfo	*pstMemberInfo	= NULL;

		if (pcsMember->m_lID == pcsSelfCharacter->m_lID)
			pstMemberInfo	= &m_stSelfInfo;
		else
			pstMemberInfo	= m_stMemberInfo + nNumSetMemberInfo;

		lHP		= 0;
		lMaxHP	= 0;

		m_pcsAgpmFactors->GetValue(&pcsMember->m_csFactor, &lHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pcsAgpmFactors->GetValue(&pcsMember->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

		strncpy(pstMemberInfo->szMemberName, pcsMember->m_szID, AGPACHARACTER_MAX_ID_STRING);
		pstMemberInfo->lMemberHP		= lHP;
		pstMemberInfo->lMemberMaxHP		= lMaxHP;
		pstMemberInfo->lMemberLevel		= m_pcsAgpmCharacter->GetLevel(pcsMember);
		pstMemberInfo->lMemberClass		= m_pcsAgpmFactors->GetClass(&pcsMember->m_csFactor);
		pstMemberInfo->bIsInEffectArea	= pcsParty->m_bIsInEffectArea[i];

		pstMemberInfo->lMemberID		= pcsMember->m_lID;

		if (pcsMember->m_lID != pcsSelfCharacter->m_lID)
			++nNumSetMemberInfo;
	}

	m_pcsUserDataMemberInfo->m_stUserData.m_lCount	= nNumSetMemberInfo;
	m_pcsUserDataMemberInfo->m_stUserData.m_lCount	= nNumSetMemberInfo;

	RefreshUserDataMemberInfo();
	RefreshUserDataSelfInfo();

	SetBuffedGrid(pcsParty);

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataMemberInfo);
	//m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataBuffGrid);

	ThrowInitEvent();

	return TRUE;
}

BOOL AgcmUIParty::ThrowInitEvent()
{
	if (!m_pcsParty)
		return FALSE;

	INT32	lProcessMember	= 0;
	INT32	i				= 0;
	AgpdCharacter *pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;

	if (m_bIsSelfUserMenuOn)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfUserMenuOn, 0, TRUE);

		CloseUserMenu();
	}
	else
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfUserMenuOff, 0, TRUE);

		INT32	lEventOn = m_lEventMemberUserMenuOnBuffOff;
		if (m_bIsBuffIconOn)
			lEventOn = m_lEventMemberUserMenuOn;

		INT32	lEvent = m_lEventMemberUserMenuOffBuffOff;
		if (m_bIsBuffIconOn)
			lEvent = m_lEventMemberUserMenuOff;

		for (i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER - 1; ++i)
		{
			if (m_lMemberUserMenuOnIndex == i)
				m_pcsAgcmUIManager2->ThrowEvent(lEventOn, i, TRUE);
			else
				m_pcsAgcmUIManager2->ThrowEvent(lEvent, i, TRUE);
		}
	}

	AgpdCharacter	*pcsLeader	= NULL;

	for (i = 0; i < m_pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacter(m_pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		if (i == 0)
			pcsLeader	= pcsMember;

		if (pcsMember == m_pcsAgcmCharacter->GetSelfCharacter())
		{
			if (i == 0)
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfSetLeader, 0, TRUE);
			else
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfSetMember, 0, TRUE);

			if (pcsMember->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfSetDead, 0, TRUE);
			}
			else if (m_pcsParty->m_bIsInEffectArea[i] && pcsLeader)
			{
				INT32	lLeaderClass	= m_pcsAgpmFactors->GetClass(&pcsLeader->m_pcsCharacterTemplate->m_csFactor);

				switch (lLeaderClass) {
				case AUCHARCLASS_TYPE_KNIGHT:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfActiveEffectAreaKnight, 0, TRUE);
					break;

				case AUCHARCLASS_TYPE_RANGER:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfActiveEffectAreaRanger, 0, TRUE);
					break;

				case AUCHARCLASS_TYPE_MAGE:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfActiveEffectAreaMage, 0, TRUE);
					break;
				}
			}
			else
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfDisableEffectArea, 0, TRUE);

			INT32 nEventNumber = _GetUpdateClassIconEventSelf( pcsMember );
			m_pcsAgcmUIManager2->ThrowEvent( nEventNumber );
		}
		else
		{
			if (i == 0)
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetLeader, lProcessMember, TRUE);
			else
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMember, lProcessMember, TRUE);

			if (pcsMember->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetDead, lProcessMember, TRUE);
			}
			else if (m_pcsParty->m_bIsInEffectArea[i] && pcsLeader)
			{
				INT32	lLeaderClass	= m_pcsAgpmFactors->GetClass(&pcsLeader->m_pcsCharacterTemplate->m_csFactor);

				switch (lLeaderClass) {
				case AUCHARCLASS_TYPE_KNIGHT:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventActiveEffectAreaKnight, lProcessMember, TRUE);
					break;

				case AUCHARCLASS_TYPE_RANGER:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventActiveEffectAreaRanger, lProcessMember, TRUE);
					break;

				case AUCHARCLASS_TYPE_MAGE:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventActiveEffectAreaMage, lProcessMember, TRUE);
					break;
				}
			}
			else
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventDisableEffectArea, lProcessMember, TRUE);

			INT32 nEventNumber = _GetUpdateClassIconEventMember( pcsMember );
			m_pcsAgcmUIManager2->ThrowEvent( nEventNumber, lProcessMember, TRUE );

			++lProcessMember;
		}
	}

	return TRUE;
}

BOOL AgcmUIParty::SetBuffedGrid(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT32	lProcessMember	= 0;

	for ( int i = 0 ; i < AGPMPARTY_MAX_PARTY_MEMBER - 1 ; i++ )
	{
		m_pcsAgpmGrid->Reset(&m_stMemberBuffGrid[i]);
	}

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsMember ||
			pcsMember == pcsSelfCharacter)
			continue;

//		m_pcsAgpmGrid->Reset(&m_stMemberBuffGrid[lProcessMember]);

		AgpdSkillAttachData	*pcsSkillAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsMember);
		if (pcsSkillAttachData)
		{
			for (int j = 0; j < AGPMSKILL_MAX_SKILL_BUFF; ++j)
			{
				AgpdSkillTemplate		*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsSkillAttachData->m_astBuffSkillList[j].lSkillTID);
				if (!pcsSkillTemplate)
					break;

				// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
				if(m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate))
					continue;

				m_pcsAgcmSkill->SetGridSkillAttachedSmallTexture(pcsSkillTemplate);

				if (!m_pcsAgpmGrid->Add(&m_stMemberBuffGrid[lProcessMember], pcsSkillTemplate->m_pcsGridItemAlarm, 1, 1))
					break;
			}
		}

		++lProcessMember;
	}

	return RefreshUserDataBuffGrid();
}

BOOL AgcmUIParty::RefreshUserDataMemberInfo()
{
	m_pcsUserDataMemberInfo->m_stUserData.m_pvData = m_stMemberInfo;
	if (!m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataMemberInfo))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::RefreshUserDataSelfInfo()
{
	m_pcsUserDataSelfInfo->m_stUserData.m_pvData = &m_stSelfInfo;
	if (!m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSelfInfo))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::RefreshUserDataBuffGrid()
{
	m_pcsUserDataBuffGrid->m_stUserData.m_pvData = m_stMemberBuffGrid;
	if (!m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataBuffGrid))
		return FALSE;

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataBuffGrid, TRUE);

	return TRUE;
}

BOOL AgcmUIParty::ThrowBonusEvent()
{
	int i;
	for (i = 0; i < m_pcsParty->m_nCurrentMember; ++i)
	{
		if (m_pcsParty->m_lMemberListID[i] == m_pcsAgcmCharacter->GetSelfCID())
			break;
	}

	if (i == m_pcsParty->m_nCurrentMember)
		return FALSE;
/*	Delete By SungHoon 2005.06.13 Event->UIMessage
	if (!m_pcsParty->m_bIsInEffectArea[i])
		return TRUE;

	if (m_lBonusDamage > 0)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventBonusDamage);
	if (m_lBonusDefense > 0)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventBonusDefense);
	if (m_lBonusMaxHP > 0 || m_lBonusMaxMP > 0)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventBonusMaxPoint);
*/
	DWORD dwColor = 0xFF7BFFCD;
	CHAR *szColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_TEXT_COLOR);
	if (szColor) dwColor = atol(szColor);

	CHAR szMessage[512];
	if (m_lBonusDamage > 0)
	{
		CHAR *szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_ADD_DAMAGE);
		if(szUIMessage)
		{
			wsprintf(szMessage,szUIMessage,m_lBonusDamage);
			SystemMessage.ProcessSystemMessage( szMessage, dwColor );
		}
	}
	if (m_lBonusDefense > 0)
	{
		CHAR *szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_ADD_DEFENSE);
		if(szUIMessage)
		{
			wsprintf(szMessage,szUIMessage,m_lBonusDefense);
			SystemMessage.ProcessSystemMessage(szMessage, dwColor);
		}
	}
	if (m_lBonusMaxHP > 0 || m_lBonusMaxMP > 0)
	{
		CHAR *szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_ADD_HPMP);
		if(szUIMessage)
		{
			wsprintf(szMessage,szUIMessage,m_lBonusMaxHP);
			SystemMessage.ProcessSystemMessage(szMessage, dwColor);
		}
	}
	m_bNotifyBonusStats	= TRUE;

	return TRUE;
}

BOOL AgcmUIParty::CBEventReturnRecvInvite(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;

	if (lTrueCancel == (INT32) TRUE)
	{
		pThis->m_pcsAgcmParty->SendPacketAccept(pThis->m_lTargetID, pThis->m_lOperatorID);
	}
	else
	{
		pThis->m_pcsAgcmParty->SendPacketReject(pThis->m_lTargetID, pThis->m_lOperatorID);
	}

	return TRUE;
}

BOOL AgcmUIParty::CBEventReturnBanishMember(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgpdCharacter			*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if (!pcsSelfCharacter)
			return FALSE;

		pThis->m_pcsAgcmParty->SendPacketBanish(pcsSelfCharacter->m_lID, pThis->m_lBanishMemberID);
	}

	return TRUE;
}

BOOL AgcmUIParty::CBEventReturnLeaveParty(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgpdCharacter			*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if (!pcsSelfCharacter)
			return FALSE;

		return pThis->m_pcsAgcmParty->SendPacketLeave(pcsSelfCharacter->m_lID);
	}

	return TRUE;
}

BOOL AgcmUIParty::CBIsActivePartyMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;
	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)		pData;

	// pcsTargetCharacter가 이미 파티 멤버인지 검사한다.
	// 파티 멤버라면 FALSE를 리턴한다.

	// pcsTargetCharacter가 PC인지 살펴본다.
	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsTargetCharacter))
		return FALSE;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
		return FALSE;

	// pcsSelfCharacter가 파티 멤버인지 봐서 파티 멤버라면 리더인지 본다. (리더가 아니면 초대할 수 없다.)
	AgpdParty	*pcsSelfParty	= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (pcsSelfParty)
	{
		if (pcsSelfParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID)
			return FALSE;
	}

	// pcsTargetCharacter가 이미 다른 파티에 가입되어 있는지 살펴본다.
	AgpdParty	*pcsTargetParty	= pThis->m_pcsAgpmParty->GetParty(pcsTargetCharacter);
	if (pcsTargetParty)
		return FALSE;

	// 배틀스퀘어 안에 있는지 확인한다.
	if (pThis->m_pcsApmMap)
	{
		ApmMap::RegionTemplate	*pcsRegionTemplate	= pThis->m_pcsApmMap->GetTemplate(pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_nBindingRegionIndex);
		if (pcsRegionTemplate)
		{
			if (pcsRegionTemplate->ti.stType.uFieldType		== ApmMap::FT_PVP &&
				pcsRegionTemplate->ti.stType.uSafetyType	== ApmMap::ST_DANGER )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIParty::CBIsActivePartyBanish(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;
	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)		pData;

	// pcsTargetCharacter가 이미 파티 멤버인지 검사한다.
	// 파티 멤버라면 FALSE를 리턴한다.

	// pcsTargetCharacter가 PC인지 살펴본다.
	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsTargetCharacter))
		return FALSE;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
		return FALSE;

	// pcsSelfCharacter가 파티 멤버인지 봐서 파티 멤버라면 리더인지 본다. (리더가 아니면 초대할 수 없다.)
	AgpdParty	*pcsSelfParty	= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (pcsSelfParty)
	{
		if (pcsSelfParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID)
			return FALSE;
	}
	else
	{
		return FALSE;
	}

	// pcsTargetCharacter와 pcsSelfCharacter의 파티가 같은지 비교한다.
	AgpdParty	*pcsTargetParty	= pThis->m_pcsAgpmParty->GetParty(pcsTargetCharacter);
	if (pcsTargetParty != pcsSelfParty)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::CBIsActivePartyLeave(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;
	AgpdCharacter			*pcsSelfCharacter	= (AgpdCharacter *)		pData;

	// pcsSelfCharacter가 파티 멤버인지 봐서 파티 멤버라면 리더인지 본다. (리더가 아니면 초대할 수 없다.)
	AgpdParty	*pcsSelfParty	= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (!pcsSelfParty)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIParty::CBAddBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty			*pThis				= (AgcmUIParty *)		pClass;
	ApBase				*pcsBase			= (ApBase *)			pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	INT32				lBuffedTID			= (INT32)				ppvBuffer[0];
	INT32				lCasterTID			= (INT32)				ppvBuffer[1];
	BOOL				bIsOnlyUpdate		= (BOOL)				ppvBuffer[2];

	if (!pThis->m_pcsParty ||
		pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsBase)
		return TRUE;

	if (bIsOnlyUpdate)
		return TRUE;
	
	// check party member

	INT32	i;
	INT32	lProcessMember	= 0;
	for (i = 0; i < pThis->m_pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[i]);
		if (pcsMember == pThis->m_pcsAgcmCharacter->GetSelfCharacter())
			continue;

		if (pcsMember == pcsBase)
			break;

		++lProcessMember;
	}

	if (i >= pThis->m_pcsParty->m_nCurrentMember)
		return TRUE;

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(lBuffedTID);
	if (pcsSkillTemplate)
	{
		// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
		if(pThis->m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate))
			return TRUE;

		pThis->m_pcsAgcmSkill->SetGridSkillAttachedSmallTexture(pcsSkillTemplate);
		pThis->m_pcsAgpmGrid->Add(&pThis->m_stMemberBuffGrid[lProcessMember], pcsSkillTemplate->m_pcsGridItemAlarm, 1, 1);
	}

	pThis->RefreshUserDataBuffGrid();

	pThis->ThrowInitEvent();

	return TRUE;
}

BOOL AgcmUIParty::CBRemoveBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty			*pThis				= (AgcmUIParty *)		pClass;
	ApBase				*pcsBase			= (ApBase *)			pData;
	INT32				lBuffedTID			= (INT32)				pCustData;

	if (!pThis->m_pcsParty ||
		pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsBase)
		return TRUE;
	
	// check party member

	INT32	i;
	INT32	lProcessMember	= 0;
	for (i = 0; i < pThis->m_pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[i]);

		if (pcsMember == pThis->m_pcsAgcmCharacter->GetSelfCharacter())
			continue;

		if (pcsMember == pcsBase)
			break;

		++lProcessMember;
	}

	if (i >= pThis->m_pcsParty->m_nCurrentMember)
		return TRUE;

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(lBuffedTID);
	if (pcsSkillTemplate)
	{
		pThis->m_pcsAgpmGrid->RemoveItemAndFillFirst(&pThis->m_stMemberBuffGrid[lProcessMember], pcsSkillTemplate->m_pcsGridItemAlarm);
	}

	pThis->RefreshUserDataBuffGrid();

	pThis->ThrowInitEvent();

	return TRUE;
}

BOOL AgcmUIParty::CBUseItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	INT32			lItemTID		= (INT32)			ppvBuffer[0];

	if (pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return TRUE;

	AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pcsAgpmItem->GetItemTemplate(lItemTID);
	if (!pcsItemTemplate ||
		pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE || 
		((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_RECALL_PARTY)
		return FALSE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUseRecallSuccess);
}

BOOL AgcmUIParty::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT16			*pnOldStatus	= (INT16 *)			pCustData;
	INT32			i;

	if (!pThis->m_pcsParty)
		return TRUE;

	if (pThis->m_pcsAgpmParty->GetParty(pcsCharacter) != pThis->m_pcsParty)
		return TRUE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD &&
		*pnOldStatus != AGPDCHAR_STATUS_DEAD)
	{
		INT32	lProcessMember	= 0;

		for (i = 0; i < pThis->m_pcsParty->m_nCurrentMember; ++i)
		{
			AgpdCharacter	*pcsMember	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[i]);

			if (pcsMember == pcsCharacter)
				break;

			if (pcsMember != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
				++lProcessMember;
		}

		if (i >= pThis->m_pcsParty->m_nCurrentMember)
			return TRUE;

		if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsCharacter)
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfSetDead, 0, TRUE);
		}
		else
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetDead, lProcessMember, TRUE);
		}
	}
	else if (pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD &&
		*pnOldStatus == AGPDCHAR_STATUS_DEAD)
	{
		INT32	lProcessMember	= 0;

		for (i = 0; i < pThis->m_pcsParty->m_nCurrentMember; ++i)
		{
			AgpdCharacter	*pcsMember	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[i]);

			if (pcsMember == pcsCharacter)
				break;

			if (pcsMember != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
				++lProcessMember;
		}

		if (i >= pThis->m_pcsParty->m_nCurrentMember)
			return TRUE;

		AgpdCharacter	*pcsLeader	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_pcsParty->m_lMemberListID[0]);

		if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsCharacter)
		{
			if (pThis->m_pcsParty->m_bIsInEffectArea[i] && pcsLeader)
			{
				INT32	lLeaderClass	= pThis->m_pcsAgpmFactors->GetClass(&pcsLeader->m_pcsCharacterTemplate->m_csFactor);

				switch (lLeaderClass) {
				case AUCHARCLASS_TYPE_KNIGHT:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfActiveEffectAreaKnight, 0, TRUE);
					break;

				case AUCHARCLASS_TYPE_RANGER:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfActiveEffectAreaRanger, 0, TRUE);
					break;

				case AUCHARCLASS_TYPE_MAGE:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfActiveEffectAreaMage, 0, TRUE);
					break;
				}
			}
			else
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelfDisableEffectArea, 0, TRUE);
		}
		else
		{
			if (pThis->m_pcsParty->m_bIsInEffectArea[i] && pcsLeader)
			{
				INT32	lLeaderClass	= pThis->m_pcsAgpmFactors->GetClass(&pcsLeader->m_pcsCharacterTemplate->m_csFactor);

				switch (lLeaderClass) {
				case AUCHARCLASS_TYPE_KNIGHT:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveEffectAreaKnight, lProcessMember, TRUE);
					break;

				case AUCHARCLASS_TYPE_RANGER:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveEffectAreaRanger, lProcessMember, TRUE);
					break;

				case AUCHARCLASS_TYPE_MAGE:
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveEffectAreaMage, lProcessMember, TRUE);
					break;
				}
			}
			else
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventDisableEffectArea, lProcessMember, TRUE);
		}
	}

	return TRUE;
}

/*
	2005.05.03.	By SungHoon
	파티장 위임을 선택할 경우 불리는 CallBack 함수
*/
BOOL AgcmUIParty::CBDelegationLeader(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;

	if (!pcsSourceControl->m_pstUserData ||
		!pcsSourceControl->m_pstUserData->m_stUserData.m_pvData)
		return FALSE;

	AgpdCharacter			*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdCharacter			*pcsTargetCharacter		= (AgpdCharacter *)			pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;

	return pThis->m_pcsAgcmParty->SendPacketDelegationLeader(pcsSelfCharacter->m_lID, pcsTargetCharacter->m_lID);
}

BOOL AgcmUIParty::CBAcceptRecall(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventAcceptRecall);

	pThis->m_pcsAgcmPrivateTrade->SendTradeCancel(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_lID);

	return pThis->m_pcsAgcmParty->SendPacketAcceptRecall(pThis->m_lRequestRecallID);
}

BOOL AgcmUIParty::CBRejectRecall(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIParty				*pThis					= (AgcmUIParty *)			pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRejectRecall);

	return pThis->m_pcsAgcmParty->SendPacketRejectRecall(pThis->m_lRequestRecallID);
}

BOOL AgcmUIParty::CBGetRecallRequestEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	AgcdUIControl	*pcsEditControl	= (AgcdUIControl *)	pData1;

	((AcUIEdit *) pcsEditControl->m_pcsBase)->ClearText();

	CHAR	szBuffer[512];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_REQUEST_RECALL), pThis->m_szRequestRecallOperatorID);

	return ((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
}


/*
	2005.05.03 By SungHoon
	파티장 위임에 성공했다.
*/
BOOL AgcmUIParty::CBReceiveDelegationLeader(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	AgpdCharacter *pcsOperator = pThis->m_pcsAgpmCharacter->GetCharacterLock(lOperatorID);
	AgpdParty *pcsParty = pThis->m_pcsAgpmParty->GetPartyLock(pcsOperator);
	if (!pcsParty)
	{
		pcsOperator->m_Mutex.Release();
		return FALSE;
	}

	BOOL bResult = pThis->m_pcsAgpmParty->DelegationLeader(pcsParty, lOperatorID, lTargetID);
	if (bResult != FALSE)
	{
		INT32 lSelfCID = pThis->m_pcsAgcmCharacter->GetSelfCID();

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyDelegationLeader);
	}

	pcsOperator->m_Mutex.Release();
	pcsParty->m_Mutex.Release();

	return bResult;
}

/*
	2005.05.03 By SungHoon
	파티장 위임에 실패할 경우 서버로 통보 받았다.
*/
BOOL AgcmUIParty::CBReceiveDelegationLeaderFailed(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return FALSE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyDelegationLeaderFailed);
}
/*
	2005.05.03 By SungHoon
	메뉴중 파티옵션 항목이 활성화 될것인지 결정한다.
*/
BOOL AgcmUIParty::CBIsActivePartyOptionUI(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdParty	*pcsSelfParty	= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (pcsSelfParty) return TRUE;
	return FALSE;
}

/*
	2005.05.03 By SungHoon
	메뉴중 파티장 위임 항목이 활성화 될것인지 결정한다.
*/
BOOL AgcmUIParty::CBIsActiveDelegationLeader(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIParty				*pThis				= (AgcmUIParty *)		pClass;
	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)		pData;

	// pcsTargetCharacter가 PC인지 살펴본다.
	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsTargetCharacter))
		return FALSE;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
		return FALSE;

	// pcsSelfCharacter가 파티 멤버인지 봐서 파티 멤버라면 리더인지 본다. (리더가 아니면 초대할 수 없다.)
	AgpdParty	*pcsSelfParty	= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (!pcsSelfParty) return FALSE;

	if (pcsSelfParty->m_lMemberListID[0] != pcsSelfCharacter->m_lID)
		return FALSE;

	// pcsTargetCharacter와 pcsSelfCharacter의 파티가 같은지 비교한다.
	AgpdParty	*pcsTargetParty	= pThis->m_pcsAgpmParty->GetParty(pcsTargetCharacter);
	if (pcsTargetParty != pcsSelfParty)
		return FALSE;

	return TRUE;

}

/*
	2005.05.27 By SungHoon
	파티에 초대한 멤버의 레벨차이가 클 경우
*/
BOOL AgcmUIParty::CBReceiveInvietFailedLevelDiff(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	INT32			lTargetID		= *(INT32 *)		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		lTargetID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return FALSE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyMemberLevelDiff);
}

/*
	2005.05.27 By SungHoon
	상대 멤버가 초대 거부일 경우
*/
BOOL AgcmUIParty::CBReceiveOperationFailedRefuse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	INT32			lOperatorID		= *(INT32 *)		pData;
	CHAR			*szTargetID		= ( CHAR * )		pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID())
		return FALSE;

	pThis->m_ulRequestedClock = 0;

	DWORD dwColor = 0xFF0000;
	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_NEGATIVE_TEXT_COLOR);
	if (szColor) dwColor = atol(szColor);

	CHAR szMessage[ 512 ] = { 0 };
	wsprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTY_INVATE_FAILED_REFUSE), szTargetID);

	SystemMessage.ProcessSystemMessage(szMessage, dwColor);

	return TRUE;
}

/*
	2005.06.16 By SungHoon
	파티 버프 아이콘을 활성/비활성화 시킨다.
*/
BOOL AgcmUIParty::SetVisibleBuffIcon(BOOL bMode /*= TRUE */ )
{
	m_bIsBuffIconOn = bMode;
	CloseUserMenu();

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelfUserMenuOff, 0, TRUE);
	m_bIsSelfUserMenuOn	=	FALSE;
	return TRUE;
}

/*
	2005.07.04 By SungHoon
	메인 유저 메뉴 선택시 파티의 유저메뉴 비활성화 시켜야 됨.
*/
BOOL AgcmUIParty::CBSelectTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIParty		*pThis			= (AgcmUIParty *)	pClass;
	if (pThis->m_bpartyMemberSelect == FALSE)
		pThis->CloseUserMenu();

	return TRUE;
}

void AgcmUIParty::CloseUserMenu()
{
	INT32	lEvent = m_lEventMemberUserMenuOffBuffOff;

	if (m_bIsBuffIconOn)
		lEvent = m_lEventMemberUserMenuOff;
	for (int i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER - 1; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent(lEvent, i, TRUE);
	}
	m_lMemberUserMenuOnIndex	= (-1);
}

// 2005.10.11. steeple
BOOL AgcmUIParty::CheckMemberTransparent(AgpdParty* pcsParty)
{
	if(!pcsParty)
		return FALSE;

	for(INT32 i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);

		if(pcsMember == NULL)
			break;

		if(pcsMember->m_lID == m_pcsAgcmCharacter->GetSelfCID())
			continue;

		if(m_pcsAgpmCharacter->IsStatusTransparent(pcsMember))
			//@{ Jaewon 20051101
			// Make it invisible immediately, i.e. no fade.
			m_pcsAgcmCharacter->UpdateTransparent(pcsMember, TRUE, 2000);
			//@} Jaewon
	}

	return TRUE;
}

INT32 AgcmUIParty::_GetUpdateClassIconEventSelf( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return -1;

	AgpdFactor* ppdFactor = &ppdCharacter->m_pcsCharacterTemplate->m_csFactor;
	if( !ppdFactor ) return AURACE_TYPE_NONE;

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( ppdFactor );
	AuCharClassType eClass = ( AuCharClassType )m_pcsAgpmFactors->GetClass( ppdFactor );

	switch( eRace )
	{
	case AURACE_TYPE_HUMAN :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetSelfClassHK;		break;
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetSelfClassHA;		break;
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetSelfClassHW;		break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetSelfClassOK;		break;
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetSelfClassOA;		break;
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetSelfClassOW;		break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetSelfClassMK;		break;
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetSelfClassMA;		break;
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetSelfClassMW;		break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetSelfClassDK;		break;	// 슬레이어
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetSelfClassDA;		break;	// 오비터
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetSelfClassDW;		break;	// 서머너
			case AUCHARCLASS_TYPE_SCION :		return m_nEventPartySetSelfClassDS;		break;	// 시온
			}
		}
		break;
	}

	return -1;
}

INT32 AgcmUIParty::_GetUpdateClassIconEventMember( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return -1;

	AgpdFactor* ppdFactor = &ppdCharacter->m_pcsCharacterTemplate->m_csFactor;
	if( !ppdFactor ) return AURACE_TYPE_NONE;

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( ppdFactor );
	AuCharClassType eClass = ( AuCharClassType )m_pcsAgpmFactors->GetClass( ppdFactor );

	switch( eRace )
	{
	case AURACE_TYPE_HUMAN :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetMemberClassHK;		break;
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetMemberClassHA;		break;
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetMemberClassHW;		break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetMemberClassOK;		break;
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetMemberClassOA;		break;
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetMemberClassOW;		break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetMemberClassMK;		break;
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetMemberClassMA;		break;
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetMemberClassMW;		break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :		return m_nEventPartySetMemberClassDK;		break;	// 슬레이어
			case AUCHARCLASS_TYPE_RANGER :		return m_nEventPartySetMemberClassDA;		break;	// 오비터
			case AUCHARCLASS_TYPE_MAGE :		return m_nEventPartySetMemberClassDW;		break;	// 서머너
			case AUCHARCLASS_TYPE_SCION :		return m_nEventPartySetMemberClassDS;		break;	// 시온
			}
		}
		break;
	}

	return -1;
}
