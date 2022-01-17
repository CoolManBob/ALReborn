#ifndef __AGCMUIPARTY_H__
#define __AGCMUIPARTY_H__

#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmParty.h"
#include "AgcmParty.h"
#include "AgcmChatting2.h"
#include "AgcmSkill.h"
#include "AgcmUIManager2.h"
#include "AgcmUIMain.h"
#include "AgpmItem.h"
#include "AgcmPrivateTrade.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIPartyD" )
#else
#pragma comment ( lib , "AgcmUIParty" )
#endif
#endif


// define user data names
/////////////////////////////////////////////////////////////
#define	AGCMUI_PARTY_USERDATA_MEMBER_INFO					"PartyMemberInfo"
#define	AGCMUI_PARTY_USERDATA_SELF_INFO						"PartySelfInfo"

// define display names
/////////////////////////////////////////////////////////////
#define	AGCMUI_PARTY_DISPLAY_MEMBER_NAME					"PartyMemberName"
#define	AGCMUI_PARTY_DISPLAY_MEMBER_HP						"PartyMemberHP"
#define	AGCMUI_PARTY_DISPLAY_MEMBER_MAXHP					"PartyMemberMaxHP"
#define	AGCMUI_PARTY_DISPLAY_MEMBER_LEVEL					"PartyMemberLevel"

#define	AGCMUI_PARTY_DISPLAY_BONUS_DAMAGE					"PartyBonusDamage"
#define	AGCMUI_PARTY_DISPLAY_BONUS_DEFENSE					"PartyBonusDefense"
#define	AGCMUI_PARTY_DISPLAY_BONUS_MAXHP					"PartyBonusMaxHP"
#define	AGCMUI_PARTY_DISPLAY_BONUS_MAXMP					"PartyBonusMaxMP"
#define	AGCMUI_PARTY_DISPLAY_LEADER_NAME					"PartyLeaderName"

// define function names
/////////////////////////////////////////////////////////////
#define	AGCMUI_PARTY_FUNCTION_INVITE_MEMBER					"PartyInviteMember"
#define	AGCMUI_PARTY_FUNCTION_BANISH_MEMBER					"PartyBanishMember"
#define	AGCMUI_PARTY_FUNCTION_LEAVE_PARTY					"PartyLeaveParty"
#define	AGCMUI_PARTY_FUNCTION_INVITE_REJECT					"PartyInviteReject"
#define	AGCMUI_PARTY_FUNCTION_INVITE_ACCEPT					"PartyInviteAccept"

#define	AGCMUI_PARTY_FUNCTION_DELEGATION_LEADER				"PartyDelegationLeader"			//	파티장 위임 2005.05.03 By SungHoon 

#define	AGCMUI_PARTY_FUNCTION_USE_SKILL_FOR_MEMBER			"PartyUseSkillForMember"

// define event names
/////////////////////////////////////////////////////////////
#define	AGCMUI_PARTY_EVENT_OPEN_PARTY_UI					"PartyOpenUI"
#define	AGCMUI_PARTY_EVENT_CLOSE_PARTY_UI					"PartyCloseUI"

#define	AGCMUI_PARTY_EVENT_RECEIVE_INVITE					"PartyRecvInvite"
#define	AGCMUI_PARTY_EVENT_RECEIVE_REJECT					"PartyRecvReject"
#define	AGCMUI_PARTY_EVENT_RECEIVE_INVITE_ACCEPT			"PartyRecvInviteAccept"
#define	AGCMUI_PARTY_EVENT_RECEIVE_LEAVE					"PartyRecvLeave"
#define	AGCMUI_PARTY_EVENT_RECEIVE_BANISH					"PartyRecvBanish"
#define	AGCMUI_PARTY_EVENT_RECEIVE_OPERATION_FAILED			"PartyRecvOperationFailed"
#define	AGCMUI_PARTY_EVENT_RECEIVE_FAILED_OTHER_MEMBER		"PartyRecvAlreadyOtherMember"
#define AGCMUI_PARTY_EVENT_RECEIVE_FAILED_ALREADY_MEMBER	"PartyRecvAlreadyPartyMember"
#define	AGCMUI_PARTY_EVENT_RECEIVE_PARTY_MEMBER_FULL		"PartyRecvMemberFull"
#define	AGCMUI_PARTY_EVENT_RECEIVE_NOT_LEADER				"PartyRecvNotLeader"

#define	AGCMUI_PARTY_EVENT_CONFORM_BANISH_MEMBER			"PartyConformBanishMember"
#define	AGCMUI_PARTY_EVENT_CONFORM_LEAVE_PARTY				"PartyConformLeaveParty"

#define	AGCMUI_PARTY_EVENT_ACTIVE_EFFECT_AREA				"PartyActiveEffectArea"
#define	AGCMUI_PARTY_EVENT_DISABLE_EFFECT_AREA				"PartyDisableEffectArea"

#define	AGCMUI_PARTY_EVENT_SET_LEADER						"PartySetLeader"
#define	AGCMUI_PARTY_EVENT_SET_MEMBER						"PartySetMember"

#define	AGCMUI_PARTY_EVENT_BONUS_DAMAGE						"PartyBonusDamage"
#define	AGCMUI_PARTY_EVENT_BONUS_DEFENSE					"PartyBonusDefense"
#define	AGCMUI_PARTY_EVENT_BONUS_MAX_POINT					"PartyBonusMaxPoint"

#define	AGCMUI_PARTY_EVENT_CHANGE_LEADER					"PartyChangeLeader"

#define	AGCMUI_PARTY_EVENT_DELEGATION_LEADER				"Party_DelegationLeader"			// 2005.05.02. By SungHoon
#define	AGCMUI_PARTY_EVENT_DELEGATION_LEADER_FAILED			"Party_DelegationLeaderFailed"		// 2005.05.02. By SungHoon

#define AGCMUI_PARTY_EVENT_INVITE_MEMBER_LEVEL_DIFF			"Party_InviteMemeberLevelDiff"		// 2005.05.27 By SungHoon

#define	AGCMUI_PARTY_EVENT_USER_MENU_ON_BUFF_OFF			"PartyMemberUserMenuOnBuffOff"			//	2005.06.16 By SungHoon
#define	AGCMUI_PARTY_EVENT_USER_MENU_OFF_BUFF_OFF			"PartyMemberUserMenuOffBuffOff"			//	2005.06.16 By SungHoon

// UI Message
#define UI_MESSAGE_ID_PARTY_CREATE							"Party_Create_Message"
#define UI_MESSAGE_ID_PARTY_CREATE_2						"Party_Create_Message_2"
#define UI_MESSAGE_ID_PARTY_DESTROY							"Party_Destroy_Message"
#define UI_MESSAGE_ID_PARTY_NOT_IN_RANGE					"Party_Not_In_Range"
#define UI_MESSAGE_ID_PARTY_NEGATIVE_TEXT_COLOR				"Negative_Text_Color"
#define UI_MESSAGE_ID_PARTY_INVATE_FAILED_REFUSE			"Option_Party_Off_User"

#define UI_MESSAGE_ID_PARTY_TEXT_COLOR						"Party_Text_Color"				// 2005.06.13. By SungHoon
#define UI_MESSAGE_ID_PARTY_ADD_DAMAGE						"Party_Add_Damage"				// 2005.06.13. By SungHoon
#define UI_MESSAGE_ID_PARTY_ADD_DEFENSE						"Party_Add_Defense"				// 2005.06.13. By SungHoon
#define UI_MESSAGE_ID_PARTY_ADD_HPMP						"Party_Add_HPMP"				// 2005.06.13. By SungHoon
#define UI_MESSAGE_ID_PARTY_ADD_NONE						"Party_Add_None"				// 2005.06.13. By SungHoon

#define	UI_MESSAGE_ID_PARTY_REQUEST_RECALL					"Party_Request_Recall"

#define UI_MESSAGE_ID_PARTY_MURDERE_ERROR_COLOR				"Party_Murderer_Error_Color"	//	2005.10.26. By SungHoon
#define UI_MESSAGE_ID_PARTY_MURDERE_ERROR_SELF				"Party_Murderer_Error_Self"		//	2005.10.26. By SungHoon
#define UI_MESSAGE_ID_PARTY_MURDERE_ERROR_TARGET			"Party_Murderer_Error_Target"	//	2005.10.26. By SungHoon
#define UI_MESSAGE_ID_PARTY_MURDERE_ERROR_LEAVE				"Party_Murderer_Error_Leave"	//	2005.10.26. By SungHoon

typedef enum	_AgcmUIParty_Display_ID {
	AGCMUI_PARTY_DISPLAY_ID_MEMBER_NAME					= 0,
	AGCMUI_PARTY_DISPLAY_ID_MEMBER_HP,
	AGCMUI_PARTY_DISPLAY_ID_MEMBER_MAXHP,
	AGCMUI_PARTY_DISPLAY_ID_MEMBER_LEVEL,
	AGCMUI_PARTY_DISPLAY_ID_BONUS_DAMAGE,
	AGCMUI_PARTY_DISPLAY_ID_BONUS_DEFENSE,
	AGCMUI_PARTY_DISPLAY_ID_BONUS_MAXHP,
	AGCMUI_PARTY_DISPLAY_ID_BONUS_MAXMP,
	AGCMUI_PARTY_DISPLAY_ID_LEADER_NAME,
	AGCMUI_PARTY_DISPLAY_ID_RECALL_TARGET_NAME,
	AGCMUI_PARTY_DISPLAY_ID_REQUEST_RECALL,
} AgcmUIParty_Display_ID;


typedef struct _AgcmUIPartyMemberInfo {
	CHAR		szMemberName[AGPACHARACTER_MAX_ID_STRING + 1];
	INT32		lMemberHP;
	INT32		lMemberMaxHP;
	INT32		lMemberClass;
	INT32		lMemberLevel;
	BOOL		bIsInEffectArea;

	INT32		lMemberID;
} AgcmUIPartyMemberInfo;


class AgcmUIParty : public AgcModule
{
private:
	ApmMap					*m_pcsApmMap;
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgpmParty				*m_pcsAgpmParty;
	AgcmParty				*m_pcsAgcmParty;
	AgcmUIManager2			*m_pcsAgcmUIManager2;
	AgcmUIMain				*m_pcsAgcmUIMain;
	AgpmSkill				*m_pcsAgpmSkill;
	AgcmSkill				*m_pcsAgcmSkill;
	AgpmGrid				*m_pcsAgpmGrid;
	AgcmChatting2			*m_pcsAgcmChatting;
	AgpmItem				*m_pcsAgpmItem;
	AgcmPrivateTrade		*m_pcsAgcmPrivateTrade;

	// UI에서 표시해야할 데이타들
	///////////////////////////////////////////////////////////////
	AgcmUIPartyMemberInfo	m_stMemberInfo[AGPMPARTY_MAX_PARTY_MEMBER - 1];
	AgcmUIPartyMemberInfo	m_stSelfInfo;

	AgpdGrid				m_stMemberBuffGrid[AGPMPARTY_MAX_PARTY_MEMBER - 1];

	AgpdParty				*m_pcsParty;

	INT32					m_lOperatorID;
	INT32					m_lTargetID;
	INT32					m_lBanishMemberID;

//	AgpmPartyCalcExpType	m_eExpType;

	BOOL					m_bNotifyBonusStats;

	INT32					m_lBonusDamage;
	INT32					m_lBonusDefense;
	INT32					m_lBonusMaxHP;
	INT32					m_lBonusMaxMP;

	// user data
	///////////////////////////////////////////////////////////////
	AgcdUIUserData			*m_pcsUserDataSelfInfo;
	AgcdUIUserData			*m_pcsUserDataMemberInfo;
	AgcdUIUserData			*m_pcsUserDataBuffGrid;

	AgcdUIUserData			*m_pcsUserDataRecallTargetName;
	AgcdUIUserData			*m_pcsUserDataInviterName;

	// event id
	///////////////////////////////////////////////////////////////
	INT32					m_lEventOpenPartyUI;
	INT32					m_lEventClosePartyUI;

	INT32					m_lEventRecvInvite;
	INT32					m_lEventRecvReject;
	INT32					m_lEventRecvInviteAccept;
	INT32					m_lEventRecvLeave;
	INT32					m_lEventRecvBanish;
	INT32					m_lEventRecvAlreadyOtherMember;
	INT32					m_lEventRecvAlreadyPartyMember;
	INT32					m_lEventRecvMemberIsFull;
	INT32					m_lEventRecvNotLeader;
	INT32					m_lEventRecvOperationFailed;

	INT32					m_lEventConformBanishMember;
	INT32					m_lEventConformLeaveParty;

	INT32					m_lEventSelfActiveEffectAreaKnight;
	INT32					m_lEventSelfActiveEffectAreaRanger;
	INT32					m_lEventSelfActiveEffectAreaMage;
	INT32					m_lEventSelfDisableEffectArea;
	INT32					m_lEventSelfSetDead;

	INT32					m_lEventSelfSetLeader;
	INT32					m_lEventSelfSetMember;

	INT32					m_lEventActiveEffectAreaKnight;
	INT32					m_lEventActiveEffectAreaRanger;
	INT32					m_lEventActiveEffectAreaMage;
	INT32					m_lEventDisableEffectArea;
	INT32					m_lEventSetDead;

	INT32					m_lEventSetLeader;
	INT32					m_lEventSetMember;

	INT32					m_lEventBonusDamage;
	INT32					m_lEventBonusDefense;
	INT32					m_lEventBonusMaxPoint;

	INT32					m_lEventChangeLeader;

	INT32					m_lEventSelfUserMenuOn;
	INT32					m_lEventSelfUserMenuOff;

	INT32					m_lEventMemberUserMenuOn;
	INT32					m_lEventMemberUserMenuOff;

//	파티장 위임결과 출력시 사용되는 이벤트 
	INT32					m_lEventPartyDelegationLeader;
	INT32					m_lEventPartyDelegationLeaderFailed;

//	파티 버프 스킬 표시 유무에 사용되는 이벤트
	INT32					m_lEventMemberUserMenuOnBuffOff;
	INT32					m_lEventMemberUserMenuOffBuffOff;

	INT32					m_lEventPartyMemberLevelDiff;

	INT32					m_lEventOpenRequestRecallUI;

	INT32					m_lEventAcceptRecall;
	INT32					m_lEventRejectRecall;

	INT32					m_lEventTargetAcceptRecall;
	INT32					m_lEventTargetRejectRecall;

	INT32					m_lEventUseRecallSuccess;

	BOOL					SetMemberInfo(AgpdParty *pcsParty);
	BOOL					SetBuffedGrid(AgpdParty *pcsParty);

	BOOL					ThrowInitEvent();

	BOOL					RefreshUserDataMemberInfo();
	BOOL					RefreshUserDataSelfInfo();
	BOOL					RefreshUserDataBuffGrid();

	BOOL					ThrowBonusEvent();

	BOOL					m_bIsSelfUserMenuOn;
	INT32					m_lMemberUserMenuOnIndex;

	BOOL					m_bIsBuffIconOn;

	CHAR					m_szRequestRecallOperatorID[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	INT32					m_lRequestRecallID;
	CHAR					m_szInviterID[AGPDCHARACTER_MAX_ID_LENGTH + 1];

//	2005.07.04		By SungHoon
	void					CloseUserMenu();
	BOOL					m_bpartyMemberSelect;
	
	UINT32					m_lInvitedClock;			// 짱깨 여러번 뜨는거 막아달란다.
	UINT32					m_ulRequestedClock;

	INT32					m_nEventPartySetSelfClassHK;
	INT32					m_nEventPartySetSelfClassHA;
	INT32					m_nEventPartySetSelfClassHW;
	INT32					m_nEventPartySetSelfClassOK;
	INT32					m_nEventPartySetSelfClassOA;
	INT32					m_nEventPartySetSelfClassOW;
	INT32					m_nEventPartySetSelfClassMK;
	INT32					m_nEventPartySetSelfClassMA;
	INT32					m_nEventPartySetSelfClassMW;
	INT32					m_nEventPartySetSelfClassDS;
	INT32					m_nEventPartySetSelfClassDK;
	INT32					m_nEventPartySetSelfClassDA;
	INT32					m_nEventPartySetSelfClassDW;
	
	INT32					m_nEventPartySetMemberClassHK;
	INT32					m_nEventPartySetMemberClassHA;
	INT32					m_nEventPartySetMemberClassHW;
	INT32					m_nEventPartySetMemberClassOK;
	INT32					m_nEventPartySetMemberClassOA;
	INT32					m_nEventPartySetMemberClassOW;
	INT32					m_nEventPartySetMemberClassMK;
	INT32					m_nEventPartySetMemberClassMA;
	INT32					m_nEventPartySetMemberClassMW;
	INT32					m_nEventPartySetMemberClassDS;
	INT32					m_nEventPartySetMemberClassDK;
	INT32					m_nEventPartySetMemberClassDA;
	INT32					m_nEventPartySetMemberClassDW;

public:
	AgcmUIParty();
	virtual	~AgcmUIParty();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				AddEvent();
	BOOL				AddFunction();
	BOOL				AddDisplay();
	BOOL				AddUserData();
	BOOL				AddBoolean();

	static BOOL			CBReceiveInvite(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveInviteAccept(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveLeave(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveBanish(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveInviteFailedAlreadyOtherMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveInviteFailedAlreadyPartyMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveInviteFailedMemberIsFull(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveInviteFailedNotLeader(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveOperationFailed(PVOID pData, PVOID pClass, PVOID pCustData);
//	파티장 위임에 성공했을 경우	2005.05.02 by SungHoon
	static BOOL			CBReceiveDelegationLeader(PVOID pData, PVOID pClass, PVOID pCustData);
//	파티장 위임에 실패했을 경우	2005.05.02 by SungHoon
	static BOOL			CBReceiveDelegationLeaderFailed(PVOID pData, PVOID pClass, PVOID pCustData);
//	파티멤버 초대시 레벨 차이가 너무 날경우

	static BOOL			CBReceiveInviteFailedMurdererSelf(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.10.26. By SungHoon
	static BOOL			CBReceiveInviteFailedMurdererTarget(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.10.26. By SungHoon
	static BOOL			CBReceiveLeaveByMurderer(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.10.26. By SungHoon

	static BOOL			CBReceiveInvietFailedLevelDiff(PVOID pData, PVOID pClass, PVOID pCustData);		// 2005.05.27 By SungHoon
	static BOOL			CBReceiveOperationFailedRefuse(PVOID pData, PVOID pClass, PVOID pCustData);		// 2005.06.01 By SungHoon

	static BOOL			CBAddParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBAddMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBChangeLeader(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBResetEffectArea(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateFactorParty(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateExpType(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateBonusStats(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBAddBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBRequestRecall(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBAcceptRecall(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRejectRecall(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUseItem(PVOID pData, PVOID pClass, PVOID pCustData);

	// display callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL			CBDisplayMemberName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayMemberHP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayMemberMaxHP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayMemberLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL			CBDisplayBonusDamage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayBonusDefense(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayBonusMaxHP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayBonusMaxMP(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL			CBDisplayLeaderName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL			CBDisplayRecallTargetName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayRequestRecall(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL			CBDisplayInviter(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	// function callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL			CBInviteMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBBanishMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBLeaveParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBInviteReject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBInviteAccept(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBToolTipOpenPartyMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBToolTipClosePartyMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBUseSkillForMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSelfSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBMemberSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBMoveCharacterMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
//	2005.05.03		By SungHoon	파티장 위임 메뉴를 눌렀을때
	static BOOL			CBDelegationLeader(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBAcceptRecall(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBRejectRecall(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBGetRecallRequestEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// event return callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL			CBEventReturnRecvInvite(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL			CBEventReturnBanishMember(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL			CBEventReturnLeaveParty(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

	// boolean callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL			CBIsActivePartyMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL			CBIsActivePartyBanish(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL			CBIsActivePartyLeave(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

//	2005.05.03		By SungHoon
	static BOOL			CBIsActivePartyOptionUI(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL			CBIsActiveDelegationLeader(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

//	2005.07.04		By SungHoon
	static BOOL			CBSelectTarget(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetVisibleBuffIcon(BOOL bMode = TRUE);		//	2005.06.16 By SungHoon

	BOOL				CheckMemberTransparent(AgpdParty* pcsParty);

private :
	INT32				_GetUpdateClassIconEventSelf( void* pCharacter );
	INT32				_GetUpdateClassIconEventMember( void* pCharacter );
};

#endif //__AGCMUIPARTY_H__
