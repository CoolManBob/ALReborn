// AgcmUIGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 08.

#ifndef _AGCMUIGUILD_H_
#define _AGCMUIGUILD_H_

#include "AgpmGuild.h"
#include "AgpmPvP.h"
#include "AgpmSiegeWar.h"

#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmGuild.h"
#include "AgcmUIManager2.h"
#include "AgcmEventGuild.h"
#include "AgcmChatting2.h"
#include "AgpmFactors.h"
#include "AgpmItem.h"
#include "AgpmParty.h"
#include "AgcmUIPartyOption.h"
#include "AgpmGrid.h"
#include "AgcmFont.h"
#include "AgcmUIControl.h"
#include "AgcmUIChatting2.h"

#include "AgcmUIGuildBattleOffer.h"
#include "AgcmUIGuildBattleMember.h"
#include "AgcmUIGuildBattleResult.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmUIGuildD.lib")
#else
	#pragma comment(lib, "AgcmUIGuild.lib")
#endif
#endif

typedef enum _eAgcmUIGuild
{
	AGCMUI_GUILD_DISPLAY_ID_TEXT = 0,
} eAgcmUIGuild;

#define UI_MESSAGE_ID_GUILD_MAIN_TEXT						"Guild_Main_Text"
#define UI_MESSAGE_ID_GUILD_CREATE_TEXT						"Guild_Create_Text"
#define UI_MESSAGE_ID_GUILD_DESTROY_TEXT					"Guild_Destroy_Text"
#define UI_MESSAGE_ID_GUILD_NOTICE_TEXT						"Guild_Notice_Text"
#define UI_MESSAGE_ID_GUILD_CREATE_CONFIRM_TEXT				"Guild_Create_Confirm_Text"
#define UI_MESSAGE_ID_GUILD_CREATE_COMPLETE_TEXT			"Guild_Create_Complete_Text"
#define UI_MESSAGE_ID_GUILD_DESTROY_CONFIRM_TEXT			"Guild_Destroy_Confirm_Text"
#define UI_MESSAGE_ID_GUILD_DESTROY_COMPLETE_TEXT			"Guild_Destroy_Complete_Text"

#define UI_MESSAGE_ID_GUILD_NAME							"Guild_Name"
#define UI_MESSAGE_ID_GUILD_PASSWORD						"Guild_Password"
#define UI_MESSAGE_ID_GUILD_PASSWORD_CHECK					"Guild_Password_Check"
#define UI_MESSAGE_ID_GUILD_DESTROY_NAME					"Guild_Destroy_Name"
#define UI_MESSAGE_ID_GUILD_INFO							"Guild_Guild_Info"
#define UI_MESSAGE_ID_GUILD_MEMBER_INFO						"Guild_Member_Info"
#define UI_MESSAGE_ID_GUILD_MASTER							"Guild_Master"
#define UI_MESSAGE_ID_GUILD_SUBMASTER						"Guild_SubMaster"
#define UI_MESSAGE_ID_GUILD_MEMBER							"Guild_Member"

#define UI_MESSAGE_ID_GUILD_NEED_MORE_LEVEL					"Guild_Msg_NeedMoreLevel"
#define UI_MESSAGE_ID_GUILD_NEED_MORE_MONEY					"Guild_Msg_NeedMoreMoney"
#define UI_MESSAGE_ID_GUILD_NEED_ITEM						"Guild_Msg_NeedItem"
#define UI_MESSAGE_ID_ALREADY_MEMBER						"Guild_Msg_AlreadyMember"
#define UI_MESSAGE_ID_ALREADY_MEMBER2						"Guild_Msg_AlreadyMember2"
#define UI_MESSAGE_ID_MAX_MEMBER							"Guild_Msg_MaxMember"
#define UI_MESSAGE_EXIST_GUILD_ID							"Guild_Msg_ExistGuildID"
#define UI_MESSAGE_USE_SPECIAL_CHAR							"Guild_Msg_UseSpecialChar"
#define UI_MESSAGE_JOIN_FAIL								"Guild_Msg_JoinFail"
#define UI_MESSAGE_JOIN_REJECT								"Guild_Msg_JoinReject"
#define UI_MESSAGE_JOIN										"Guild_Msg_Join"
#define UI_MESSAGE_INVALID_PASSWORD							"Guild_Msg_InvalidPassword"
#define UI_MESSAGE_CREATE_COMPLETE							"Guild_Msg_CreateComplete"
#define UI_MESSAGE_DESTROY									"Guild_Msg_Destroy"
#define UI_MESSAGE_LEVEL_UP									"Guild_Msg_LevelUp"
#define UI_MESSAGE_LEAVE									"Guild_Msg_Leave"
#define UI_MESSAGE_FORCED_LEAVE								"Guild_Msg_ForcedLeave"
#define UI_MESSAGE_FORCED_LEAVE2							"Guild_Msg_ForcedLeave2"
#define UI_MESSAGE_CHECK_PASSWORD							"Guild_Msg_CheckPassword"
#define UI_MESSAGE_MEMBER_ONLINE							"Guild_Msg_Member_Online"
#define UI_MESSAGE_MEMBER_OFFLINE							"Guild_Msg_Member_Offline"
#define UI_MESSAGE_GUILD_APPOINT_CONFIRM					"GuildAppointConfirm_Message"
#define UI_MESSAGE_GUILD_SUCCESSION_CONFIRM					"GuildSuccessionConfirm_Message"
#define UI_MESSAGE_GUILD_MEMBERRANK_UPDATE					"GuildMemberRankUpdate_Message"
#define UI_MESSAGE_GUILD_APPOINTCANCEL_CONFIRM				"GuildMemberAppointCancelConfirm_Message"

#define UI_MESSAGE_BATTLE_NOT_EXIST_GUILD					"Guild_Msg_Battle_Not_Exist_Guild"
#define UI_MESSAGE_BATTLE_ADJUST_DURATION					"Guild_Msg_Battle_Adjust_Duration"
#define UI_MESSAGE_BATTLE_MASTER_OFFLINE					"Guild_Msg_Battle_Master_Offline"
#define UI_MESSAGE_BATTLE_NOT_REQUEST_STATUS				"Guild_Msg_Battle_Not_Request_Status"
#define UI_MESSAGE_BATTLE_BOTH_MEMBER_COUNT					"Guild_Msg_Battle_Both_Member_Count"
#define UI_MESSAGE_BATTLE_NOT_ENOUGH_MEMBER_COUNT			"Guild_Msg_Battle_Not_Enough_Member"
#define UI_MESSAGE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL			"Guild_Msg_Battle_Not_Enough_Level"
#define UI_MESSAGE_BATTLE_NOT_REQUEST_STATUS2				"Guild_Msg_Battle_Not_Request_Status2"
#define UI_MESSAGE_BATTLE_REQUEST							"Guild_Msg_Battle_Request"
#define UI_MESSAGE_BATTLE_ACCEPT							"Guild_Msg_Battle_Accept"
#define UI_MESSAGE_BATTLE_REJECT							"Guild_Msg_Battle_Reject"
#define UI_MESSAGE_BATTLE_REJECT_BY_OTHER					"Guild_Msg_Battle_Reject_By_Other"
#define UI_MESSAGE_BATTLE_CANCEL_PASSED_TIME				"Guild_Msg_Battle_Cancel_Passed_Time"
#define UI_MESSAGE_BATTLE_CANCEL_REQUEST					"Guild_Msg_Battle_Cancel_Request"
#define UI_MESSAGE_BATTLE_CANCEL_REJECT						"Guild_Msg_Battle_Cancel_Reject"
#define UI_MESSAGE_BATTLE_CANCEL_REJECT_BY_OTHER			"Guild_Msg_Battle_Cancel_Reject_By_Other"
#define UI_MESSAGE_BATTLE_CANCEL_ACCEPT						"Guild_Msg_Battle_Cancel_Accpet"
#define UI_MESSAGE_BATTLE_CANNOT_REQUEST_GG					"Guild_Msg_Battle_Cannot_Request_GG"
#define UI_MESSAGE_BATTLE_START								"Guild_Msg_Battle_Start"
#define UI_MESSAGE_BATTLE_WIN								"Guild_Msg_Battle_Win"
#define UI_MESSAGE_BATTLE_DRAW								"Guild_Msg_Battle_Draw"
#define UI_MESSAGE_BATTLE_LOSE								"Guild_Msg_Battle_Lose"
#define UI_MESSAGE_BATTLE_WIN_BY_GG							"Guild_Msg_Battle_Win_By_GG"
#define UI_MESSAGE_BATTLE_LOSE_BY_GG						"Guild_Msg_Battle_Lose_By_GG"
#define UI_MESSAGE_BATTLE_WIN2								"Guild_Msg_Battle_Win2"
#define UI_MESSAGE_BATTLE_DRAW2								"Guild_Msg_Battle_Draw2"
#define UI_MESSAGE_BATTLE_LOSE2								"Guild_Msg_Battle_Lose2"
#define UI_MESSAGE_BATTLE_WIN_BY_GG2						"Guild_Msg_Battle_Win_By_GG2"
#define UI_MESSAGE_BATTLE_LOSE_BY_GG2						"Guild_Msg_Battle_Lose_By_GG2"
#define UI_MESSAGE_BATTLE_NOT_USABLE						"Guild_Msg_Battle_Not_Usable"
#define UI_MESSAGE_BATTLE_NOT_DECLARE						"Guild_Msg_Battle_Declare"

#define UI_MESSAGE_BATTLE_READY_INFO						"Guild_Msg_Battle_Ready_Info"
#define UI_MESSAGE_BATTLE_START_INFO						"Guild_Msg_Battle_Start_Info"

#define UI_MESSAGE_GUILD_REFUSE_USER						"Option_Guild_Off_User"					//	2005.06.02. By SungHoon
#define UI_MESSAGE_GBATTLE_REFUSE							"Option_GBattle_Off_User"				//	2005.06.02. By SungHoon

#define	UI_MESSAGE_NOT_ENOUGHT_GHELD_FOR_INCREASE_MAX		"Guild_Not_Enought_Gheld"				//	2005.07.18. By SungHoon
#define	UI_MESSAGE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX		"Guild_Not_Enought_Skull"				//	2005.07.18. By SungHoon

#define UI_MESSAGE_GUILD_CURRENT_MAX_MEMBER					"Guild_CurrentMaxMember"				//	2005.07.18. By SungHoon
#define UI_MESSAGE_GUILD_NEXT_MAX_MEMBER					"Guild_NextMaxMember"					//	2005.07.18. By SungHoon
#define UI_MESSAGE_GUILD_CAN_NOT_INCREASE_MAX_MEMBER		"Guild_CanNot_Increase_MaxMember"			//	2005.09.08. By SungHoon

#define	UI_MESSAGE_NOTICE_INFO								"Guild_Notice_Info"						//	2005.07.18. By SungHoon

#define UI_MESSAGE_GUILD_DETAIL_INFO						"Guild_GuildDetailInfo"					//	2005.07.18. By SungHoon
#define UI_MESSAGE_GUILD_MEMBER_DETAIL_INFO					"Guild_GuildMemberInfo"					//	2005.07.18. By SungHoon
#define UI_MESSAGE_GUILD_LEADMEMBER_DETAIL_INFO				"Guild_GuildLeadMemberInfo"				
												
#define	UI_MESSAGE_GUILD_SELECT_PAGE_COLOR					"Guild_SelectPageColor"					//	2005.07.20. By SungHoon
#define	UI_MESSAGE_GUILD_DESELECT_PAGE_COLOR				"Guild_DeselectPageColor"				//	2005.07.20. By SungHoon

#define	UI_MESSAGE_GUILD_INVALID_SEARCH_ID					"Guild_Invalid_Search_ID"				//	2005.07.20. By SungHoon
#define UI_MESSAGE_GUILD_NO_EXIST_MEMBER					"Guild_No_Exist_Member"					//	2005.07.20. By SungHoon
#define UI_MESSAGE_GUILD_LEAVE_ERROR_MASTER					"Guild_Leave_Error_Master"			//	2005.07.20. By SungHoon

#define	UI_MESSAGE_GUILD_SYSTEM_MESSAGE_COLOR				"Guild_System_Message_Color"			//	2005.07.20. By SungHoon

#define	UI_MESSAGE_GUILD_LIST_BATTLE_BUTTON_TEXT			"Guild_List_Battle_Button_Text"			//	2005.07.20. By SungHoon
#define	UI_MESSAGE_GUILD_LIST_JOIN_REQUEST_BUTTON_TEXT		"Guild_List_Join_Request_Button_Text"	//	2005.07.20. By SungHoon

#define	UI_MESSAGE_GUILD_NOTICE_DETAIL_NO_GUILD_MEMBER		"Guild_Notice_Detail_NoGuildMember"		//	2005.07.20. By SungHoon
#define	UI_MESSAGE_GUILD_NOTICE_REMAIN_LEAVE_TIME			"Guild_Notice_Detail_RemainLeaveTime"	//	2005.07.20. By SungHoon
#define	UI_MESSAGE_GUILD_NO_EXIST_SEARCH_GUILD				"Guild_No_Exist_Search_Guild"	//	2005.07.21. By SungHoon

#define	UI_MESSAGE_GUILD_TIME_UNIT_HOUR						"Time_Unit_Hour"						//	2005.08.03. By SungHoon
#define	UI_MESSAGE_GUILD_TIME_UNIT_MINUTE					"Time_Unit_Minute"						//	2005.08.03. By SungHoon
#define	UI_MESSAGE_GUILD_TIME_UNIT_SECOND					"Time_Unit_Second"						//	2005.08.03. By SungHoon

#define	UI_MESSAGE_GUILD_OFF_COLOR							"Negative_Text_Color"					//	2005.06.02. By SungHoon

#define UI_MESSAGE_GUILD_MEMBERLIST_JOIN_ALLOW_BUTTON_TEXT	"Guild_Join_Allow_Button_Text"			//	2005.08.24. By SungHoon
#define UI_MESSAGE_GUILD_MEMBERLIST_LEAVE_ALLOW_BUTTON_TEXT	"Guild_Leave_Allow_Button_Text"			//	2005.08.24. By SungHoon

#define UI_MESSAGE_BATTLE_ADJUST_MIN_DURATION				"Guild_Msg_Battle_Adjust_Min_Duration"	//	2005.11.24. By SungHoon
#define UI_MESSAGE_BATTLE_ADJUST_MAX_DURATION				"Guild_Msg_Battle_Adjust_Max_Duration"	//	2005.11.24. By SungHoon

#define AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST					19

#define AGCMUIGUILD_CLOSE_UI_DISTANCE						100

#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_1					"GuildBattle_1"
#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_2					"GuildBattle_2"
#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_3					"GuildBattle_3"
#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_4					"GuildBattle_4"

#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_NUMBER_1			"GuildBattle_Time_1"
#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_NUMBER_2			"GuildBattle_Time_2"
#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_NUMBER_3			"GuildBattle_Time_3"
#define UI_MESSAGE_ID_GUILD_BATTLE_TIME_NUMBER_4			"GuildBattle_Time_4"

#define	UI_MESSAGE_GUILD_MEMBER_JOIN_REQUEST				"Guild_Member_Join_Request"
#define	UI_MESSAGE_GUILD_MEMBER_LEAVE_REQUEST				"Guild_Member_Leave_Request"

#define UI_MESSAGE_GUILD_JOIN_REQUEST_SELF					"Guild_Join_Request_Self"
#define UI_MESSAGE_GUILD_LEAVE_REQUEST_SELF					"Guild_Leave_Request_Self"

#define UI_MESSAGE_GUILD_RENAME_USE_SPECIAL_CHAR			"Guild_Rename_Use_Special_Char"
#define UI_MESSAGE_GUILD_RENAME_EXIST_GUILD_ID				"Guild_Rename_Exist_Guild_ID"
#define UI_MESSAGE_GUILD_RENAME_NOT_MASTER					"Guild_Rename_Not_Master"
#define UI_MESSAGE_GUILD_RENAME_IMPOSIBLE_GUILD_ID			"Guild_Rename_Imposible_Guild_ID"
#define UI_MESSAGE_GUILD_RENAME_SUCCESS						"Guild_Rename_Success"

#define UI_MESSAGE_GUILD_JOINT_REQUEST_BUTTON_TEXT			"Guild_Joint_Request_Button_Text"
#define UI_MESSAGE_GUILD_JOINT_LEAVE_BUTTON_TEXT			"Guild_Joint_Leave_Button_Text"
#define UI_MESSAGE_GUILD_HOSTILE_REQUEST_BUTTON_TEXT		"Guild_Hostile_Request_Button_Text"
#define UI_MESSAGE_GUILD_HOSTILE_LEAVE_BUTTON_TEXT			"Guild_Hostile_Leave_Button_Text"

#define UI_MESSAGE_GUILD_JOINT_DISABLE						"Guild_Joint_Disable"
#define UI_MESSAGE_GUILD_JOINT_NOT_LEADER					"Guild_Joint_Not_Leader"
#define UI_MESSAGE_GUILD_JOINT_ALREADY_JOINT				"Guild_Joint_Already_Joint"
#define UI_MESSAGE_GUILD_JOINT_MAX							"Guild_Joint_Max"
#define UI_MESSAGE_GUILD_JOINT_MASTER_OFFLINE				"Guild_Joint_Maste_Offline"
#define UI_MESSAGE_GUILD_JOINT_NOT_ENOUGH_MEMBER			"Guild_Joint_Not_Enough_Member"
#define UI_MESSAGE_GUILD_JOINT_OTHER_NOT_ENOUGH_MEMBER		"Guild_Joint_Other_Not_Enough_Member"
#define UI_MESSAGE_GUILD_JOINT_WAIT							"Guild_Joint_Wait"
#define UI_MESSAGE_GUILD_JOINT_REJECT						"Guild_Joint_Reject"
#define UI_MESSAGE_GUILD_JOINT_FAILURE						"Guild_Joint_Failure"
#define UI_MESSAGE_GUILD_JOINT_SUCCESS						"Guild_Joint_Success"
#define UI_MESSAGE_GUILD_JOINT_LEAVE_SUCCESS				"Guild_Joint_Leave_Success"
#define UI_MESSAGE_GUILD_JOINT_LEAVE_OTHER_GUILD			"Guild_Joint_Leave_Other_Guild"
#define UI_MESSAGE_GUILD_JOINT_DESTROY						"Guild_Joint_Destroy"
#define UI_MESSAGE_GUILD_JOINT_SUCCESS2						"Guild_Joint_Success2"

#define UI_MESSAGE_GUILD_HOSTILE_DISABLE					"Guild_Hostile_Disable"
#define UI_MESSAGE_GUILD_HOSTILE_ALREADY_HOSTILE			"Guild_Hostile_Already_Hostile"
#define UI_MESSAGE_GUILD_HOSTILE_MAX						"Guild_Hostile_Max"
#define UI_MESSAGE_GUILD_HOSTILE_MASTER_OFFLINE				"Guild_Hostile_Master_Offline"
#define UI_MESSAGE_GUILD_HOSTILE_NOT_ENOUGH_MEMBER			"Guild_Hostile_Not_Enough_Member"
#define UI_MESSAGE_GUILD_HOSTILE_OTHER_NOT_ENOUGH_MEMBER	"Guild_Hostile_Other_Not_Enough_Member"
#define UI_MESSAGE_GUILD_HOSTILE_MASTER_OFFLINE				"Guild_Hostile_Master_Offline"
#define UI_MESSAGE_GUILD_HOSTILE_WAIT						"Guild_Hostile_Wait"
#define UI_MESSAGE_GUILD_HOSTILE_REJECT						"Guild_Hostile_Reject"
#define UI_MESSAGE_GUILD_HOSTILE_FAILURE					"Guild_Hostile_Failure"
#define UI_MESSAGE_GUILD_HOSTILE_SUCCESS					"Guild_Hostile_Success"

#define UI_MESSAGE_GUILD_HOSTILE_LEAVE_DISABLE				"Guild_Hostile_Leave_Disable"
#define UI_MESSAGE_GUILD_HOSTILE_LEAVE_WAIT					"Guild_Hostile_Leave_Wait"
#define UI_MESSAGE_GUILD_HOSTILE_LEAVE_REJECT				"Guild_Hostile_Leave_Reject"
#define UI_MESSAGE_GUILD_HOSTILE_LEAVE_FAILURE				"Guild_Hostile_Leave_Failure"
#define UI_MESSAGE_GUILD_HOSTILE_LEAVE_SUCCESS				"Guild_Hostile_Leave_Success"

#define UI_MESSAGE_GUILD_ARCHLORD_DEFENSE_SUCCESS			"Guild_Archlord_Defense_Success"
#define UI_MESSAGE_GUILD_ARCHLORD_DEFENSE_FAILURE			"Guild_Archlord_Defense_Failure"
#define UI_MESSAGE_GUILD_ARCHLORD_ATTACK_SUCCESS			"Guild_Archlord_Attack_Success"
#define UI_MESSAGE_GUILD_ARCHLORD_ATTACK_FAILURE			"Guild_Archlord_Attack_Failure"

#define UI_MESSAGE_GUILD_STOREHOUCE_MEMBER_FAIL				"Guild_Store_Member_Fail"
#define UI_MESSAGE_GUILD_DESTORY_KEEP_FAIL					"Guild_Destory_Keep_Fail"

#define	AGCMUIGUILD_MAX_GUILD_BATTLE_TIME					4

#define	AGCMUIGUILD_MAX_VIEW_PAGE							10
#define	AGCMUIGUILD_HALF_VIEW_PAGE							5

#define AGCMUIGUILD_SECOND_EQUAL_HOUR						( 60 * 60 )
#define AGCMUIGUILD_SECOND_EQUAL_MIN						( 60 )

#define AGCMUIGUILD_GUILDMARK_GRID_LAYER					3
#define AGCMUIGUILD_GUILDMARK_GRID_ROW						7
#define AGCMUIGUILD_GUILDMARK_GRID_COLUMN					5

#define	AGCMUIGUILD_GUILDMARK_GRID_COLOR_ROW				3
#define AGCMUIGUILD_GUILDMARK_GRID_COLOR_COLUMN				8

#define	AGCMUIGUILD_GUILDMARK_MAX_COLOR						AGPMGUILD_GUILDMARK_MAX_COLOR

typedef enum _eAgcmUIGuildDrawBattleInfoType
{
	AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_INIT = 0,
	AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_ACCEPT,
	AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_START,
	AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_TIME,
	AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_SCORE,
	AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_RESULT,
} AgcmUIGuildDrawBattleInfoType;

typedef enum _eAgcmUIGuildListViewMode
{
	AGCMUIGUILD_LIST_VIEW_MODE_NORMAL = 0,
	AGCMUIGUILD_LIST_VIEW_MODE_JOINT,
	AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE,
} eAgcmUIGuildListViewMode;

class AgpmEventGuild;

class AgcmUIGuild : public AgcModule
{
friend AgcmUIGuildBattleOffer;
friend AgcmUIGuildBattleMember;
friend AgcmUIGuildBattleResult;

private:
	AgpmGuild* m_pcsAgpmGuild;
	AgpmPvP* m_pcsAgpmPvP;
	AgpmSiegeWar* m_pcsAgpmSiegeWar;

	AgcmCharacter* m_pcsAgcmCharacter;
	AgpmCharacter* m_pcsAgpmCharacter;
	AgpmItem* m_pcsAgpmItem;
	AgcmGuild* m_pcsAgcmGuild;
	AgcmUIManager2* m_pcsAgcmUIManager2;
	AgcmEventGuild* m_pcsAgcmEventGuild;
	AgcmChatting2* m_pcsAgcmChatting;
	AgcmFont* m_pcsAgcmFont;
	AgpmFactors* m_pcsAgpmFactors;
	AgcmUIPartyOption *m_pcsAgcmUIPartyOption;
	AgpmParty *m_pcsAgpmParty;
	AgpmGrid *m_pcsAgpmGrid;
	AgcmUIControl* m_pcsAgcmUIControl;
	AgcmTextBoardMng* m_pcsAgcmTextBoard;
	AgcmUIChatting2* m_pcsAgcmUIChatting2;
	AgpmEventGuild* m_pcsAgpmEventGuild;

	AgcmUIGuildBattleOffer	m_cUIGuildBattleOffer;
	AgcmUIGuildBattleMember	m_cUIGuildBattleMember;
	AgcmUIGuildBattleResult	m_cUIGuildBattleResult;

	CHAR m_szSelfGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];

	CHAR m_szInputGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR m_szInputPassword[AGPMGUILD_MAX_PASSWORD_LENGTH+1];

	// Notice UI
	AgcdUIUserData* m_pstNoticeUD;
	INT32 m_lNoticeUDIndex;	// 그냥 임시 변수

	// Member & Record Status
	AgcdUIUserData* m_pstRecordUD;
	INT32 m_lRecordUDIndex;

	//	2005.07.06. By SungHoon
	AgcdUIUserData* m_pstUDGuildDetailInfo;
	INT32 m_lUDGuildDetailInfo;

	//	2005.07.06. By SungHoon
	AgcdUIUserData* m_pstUDGuildMemberDetailInfo;
	INT32 m_lUDGuildMemberDetailInfo;

	//	2005.07.07. By SungHoon
	AgcdUIUserData* m_pstUDGuildListDetailInfo;
	INT32 m_lUDGuildListDetailInfo;

	//	2005.07.08. By SungHoon
	AgcdUIUserData* m_pstGuildListIndexUD;
	INT32 m_arrGuildListIndex[AGPMGUILD_MAX_VISIBLE_GUILD_LIST];

	// Member UI
	AgcdUIUserData* m_pstMemberIndexUD;
	INT32 m_arrMemberIndex[AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST];
	AgcdGuildMemberList		m_listAgcdGuildMember;
	AgcdGuildList*			m_parrAgcdGuildList[AGPMGUILD_MAX_VISIBLE_GUILD_LIST];		//	2005.07.07. by SungHoon

	INT32 m_lVisibleGuildCount;			//	2005.07.07. by SungHoon

	AgcdUIUserData* m_pstSelectedMemberIDUD;
	CHAR m_szSelectedMemberID[AGPACHARACTER_MAX_ID_STRING+1];
	AgcdUIUserData* m_pstSelectedMemberIndexUD;
	INT32 m_arrSelectedMemberIndex[AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST];
	INT32 m_lSelectedMemberIndex;
	
	INT32 m_lSelectedGuildIndex;			//	2005.07.07. by SungHoon

	BOOL m_bGuildInfoUIOpened;
	INT8 m_cGuildInfoUILastTab;
	INT32 m_lCurrentPage;
	INT32 m_lGuildListCurrentPage;			//	2005.07.07 by SungHoon
	INT32 m_lGuildListMaxCount;				//	2005.07.19. By SungHoon
	INT32 m_lGuildMemberCount;				//	2005.07.21. By SungHoon

	BOOL m_bIsGuildUIOpen;

	AuPOS m_stGuildMainOpenPos;

	AgcdUIUserData* m_pstJoinGuildIDUD;
	CHAR m_szJoinGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];

	AgcdUIUserData* m_pstJoinGuildMasterIDUD;
	CHAR m_szJoinGuildMasterID[AGPACHARACTER_MAX_ID_STRING+1];

	// Battle
	AgcdUIUserData* m_pstBattleEnemyGuildIDUD;
	CHAR m_szBattleEnemyGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];

	INT32 m_lEventOpenGuildMainUI;
	INT32 m_lEventOpenGuildCreateUI;
	INT32 m_lEventOpenGuildDestroyUI;
	INT32 m_lEventOpenGuildNoticeUI;
	INT32 m_lEventOpenGuildMemberUI;
	INT32 m_lEventOpenGuildGuildListUI;				//	2005.07.06. By SungHoon
	INT32 m_lEventOpenGuildCreateConfirmUI;
	INT32 m_lEventOpenGuildCreateCompleteUI;
	INT32 m_lEventOpenGuildDestroyConfirmUI;
	INT32 m_lEventOpenGuildDestroyCompleteUI;

	INT32 m_lEventCloseGuildMainUI;
	INT32 m_lEventCloseGuildCreateUI;
	INT32 m_lEventCloseGuildDestroyUI;
	INT32 m_lEventCloseGuildNoticeUI;
	INT32 m_lEventCloseGuildMemberUI;
	INT32 m_lEventCloseGuildGuildListUI;			
	INT32 m_lEventCloseGuildCreateConfirmUI;
	INT32 m_lEventCloseGuildCreateCompleteUI;
	INT32 m_lEventCloseGuildDestroyConfirmUI;
	INT32 m_lEventCloseGuildDestroyCompleteUI;

	INT32 m_lEventOpenInviteConfirmUI;
	INT32 m_lEventOpenLeaveConfirmUI;
	INT32 m_lEventOpenForcedLeaveConfirmUI;

	INT32 m_lEventOpenBattleRequestConfirmUI;
	INT32 m_lEventOpenBattleAcceptConfirmUI;
	INT32 m_lEventOpenBattleCancelRequestConfirmUI;
	INT32 m_lEventOpenBattleCancelAcceptConfirmUI;
	INT32 m_lEventOpenBattleWithdrawConfirmUI;

	INT32 m_lEventGuildBattleTimeUIOpen;			
	INT32 m_lEventGuildMaxMemberIncreaseUIOpen;		

	INT32 m_lEventGuildBattleTimeUIClose;			
	INT32 m_lEventGuildMaxMemberIncreaseUIClose;	
	INT32 m_lEventConfirmJoingRequest;				

	INT32 m_lEventInitGuildNoticeUI;				//	2005.08.22. By SungHoon

	BOOL m_bEnableIncreaseMaxMember;				//	2005.07.18. By SungHoon
	AgcdUIUserData* m_pstEnableIncreaseMaxMember;	//	2005.07.18. By SungHoon

	BOOL m_bEnableGuildBattle;				//	2005.07.18. By SungHoon
	AgcdUIUserData* m_pstEnableGuildBattle;	//	2005.07.18. By SungHoon

	INT32	m_lEventGuildMaxMemberIncreaseOK;	//	2005.07.18. By SungHoon
	INT32	m_lEventGuildDestroyFailTooEarly;	//	2005.07.18. By SungHoon

	BOOL m_bEnableLeaveGuildNotice;					//	2005.07.21. By SungHoon
	AgcdUIUserData *m_pstEnableLeaveGuildNotice;		//	2005.07.21. By SungHoon

	BOOL m_bEnableLeaveGuildMemberList;						//	2005.07.21. By SungHoon
	AgcdUIUserData *m_pstEnableLeaveGuildMemberList;		//	2005.07.21. By SungHoon

	BOOL m_bEnableAllowGuildMemberList;						//	2005.08.24. By SungHoon
	AgcdUIUserData *m_pstEnableAllowGuildMemberList;		//	2005.08.24. By SungHoon

	BOOL m_bEnableInviteParty;						//	2005.07.22. By SungHoon
	AgcdUIUserData *m_pstEnableInviateParty;		//	2005.07.22. By SungHoon

	BOOL m_bEnableChatWhisper;						//	2005.07.22. By SungHoon
	AgcdUIUserData *m_pstEnableChatWhisper;		//	2005.07.22. By SungHoon

	INT32 m_lGuildNoticeDetailInfo;						//	2005.07.21. By SungHoon
	AgcdUIUserData *m_pstGuildNoticeDetailInfo;			//	2005.07.21. By SungHoon

	INT32 m_lEventWhisperChat;							//	2005.07.22. By SungHoon
	INT32 m_lEventOpenChatUI;							//	2005.07.22. By SungHoon

	INT32 m_lEventConfirmAllowJoin;						//	2005.08.02. By SungHoon
	INT32 m_lEventConfirmAllowLeave;					//	2005.08.02. By SungHoon

	BOOL m_bEnableCreateGuild;						//	2005.08.31. By SungHoon
	AgcdUIUserData *m_pstEnableCreateGuild;			//	2005.08.31. By SungHoon

	BOOL m_bEnableGuildMaster;						//	2005.08.31. By SungHoon
	AgcdUIUserData *m_pstEnableGuildMaster;			//	2005.08.31. By SungHoon

	BOOL m_bEnableGuildCancle;						//  길드 해제 버튼
	AgcdUIUserData *m_pstEnableGuildCancle;			//	길드 해제 버튼


	BOOL			m_bEnableRenameGuildID;						//	2005.08.31. By SungHoon
	AgcdUIUserData *m_pstEnableRenameGuildID;			//	2005.08.31. By SungHoon

	INT32			m_lEventCloseGuildRenameUI ;					//	2005.08.31. By SungHoon

	// 2006.07.20. steeple
	INT32			m_lEventOpenJointRequestConfirmUI;
	INT32			m_lEventOpenJointAcceptConfirmUI;
	INT32			m_lEventOpenJointLeaveConfirmUI;
    INT32			m_lEventOpenHostileRequestConfirmUI;
	INT32			m_lEventOpenHostileAcceptConfirmUI;
	INT32			m_lEventOpenHostileLeaveRequestConfirmUI;
	INT32			m_lEventOpenHostileLeaveAcceptConfirmUI;

	CHAR			m_szRelationGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
	AgcdUIUserData* m_pstRelationGuildID;
	CHAR			m_szRelationMasterID[AGPDCHARACTER_NAME_LENGTH + 1];
	AgcdUIUserData* m_pstRelationMasterID;

	AgcdUIUserData* m_pstJointRequestBtnUD;
	INT32			m_lJointReqestBtnUDIndex;
	AgcdUIUserData* m_pstHostileRequestBtnUD;
	INT32			m_lHostileRequestBtnUDIndex;

	// 부길마 임명 및 해제 버튼
	AgcdUIUserData* m_pstAppointAndCancelBtnID;
	CHAR			m_szAppointAndCancelBtnID[ AGPMGUILD_MAX_GUILD_ID_LENGTH + 1 ];


	BOOL			m_bGuildListRelationBtn;
	AgcdUIUserData* m_pstGuildListRelationBtn;


	eAgcmUIGuildListViewMode m_eListMode;

	GuildDetailVector		m_JointDetailVector;
	GuildDetailVector		m_HostileDetailVector;

	BOOL					m_bBattleAccept;

	INT32 m_lEventOpenWorldChampionshipUI;


	BOOL			m_bEnableAppointButton;
	BOOL			m_bEnableSuccessionButton;
	
	AgcdUIUserData* m_pstEnableAppoint;
	AgcdUIUserData* m_pstEnableSuccession;



protected:
	BOOL AddEvent();
	BOOL AddFunction();
	BOOL AddDisplay();
	BOOL AddUserData();
	BOOL AddBoolean();

public:
	AgcmUIGuild();
	virtual ~AgcmUIGuild();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	BOOL OpenGuildUI();
	VOID InitGuildUI();

	AgcmUIGuildBattleOffer&		GetUIGuildBattleOffer()		{	return m_cUIGuildBattleOffer;	}
	AgcmUIGuildBattleMember&	GetUIGuildBattleMember()	{	return m_cUIGuildBattleMember;	}
	AgcmUIGuildBattleResult&	GetUIGuildBattleResult()	{	return m_cUIGuildBattleResult;	}

	CHAR*		GetMemberRankText( INT32 nRank );

	static BOOL CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBIsActiveGuildInviteMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL CBMainOpenRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMainToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMainCreateRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMainDestroyRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	static BOOL CBCreateToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCreateOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBDestroyToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBDestroyOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBCreateConfirmToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCreateConfirmOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBCreateCompleteToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBDestroyConfirmToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBDestroyConfirmOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBDestroyCompleteToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBNoticeOpenRequest	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBNoticeToggle		(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBNoticeUIInit		(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	//	2005.08.22. By SungHoon
	static BOOL CBNoticeTabClick	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBNoticeClose		(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBNoticeEditClick	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBNoticeSendClick	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	BOOL InitMemberUIData();
	BOOL SetCurrentPage(INT32 lPage);
	BOOL RefreshMemberPageUI(INT32 lSelectIndex = -1 );
	BOOL GetMemberListPage();
	AgcdGuildMember* GetGuildMember( INT32 lIndex );

	static BOOL CBMemberToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberTabClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberLeaveClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberLeftClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberRightClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMemberSelectDClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBGuildListToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildListTabClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildListClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildListClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBGuildListLeftClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildListRightClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// 2008.09.02 
	// 부길마 임명 , 길드승계  버튼의 콜백
	static BOOL CBGuildAppointClick		( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pData3 , PVOID pData4 , PVOID pData5 , ApBase* pcsTarget , AgcdUIControl* pcsSourceControl );
	static BOOL CBGuildSuccessionClick	( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pData3 , PVOID pData4 , PVOID pData5 , ApBase* pcsTarget , AgcdUIControl* pcsSourceControl );

	// 2005.07.08. By SungHoon
	BOOL RefreshGuildList();
	BOOL InitGuildListUIData();

	// From Character Menu UI
	static BOOL CBSendJoinRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// Ok Cancel Dialog
	static BOOL CBEventReturnInviteConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnLeaveConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnForcedLeaveConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

	// Display
	static BOOL CBDisplayGuildText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayInviteGuild(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayNotice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBDisplayMemberStatus(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBDisplayMemberName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayMemberLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayDetailInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);	//	2005.07.04. By SungHoon
	static BOOL CBDisplayMemberClass(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	
	static BOOL CBDisplayMemberRace(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	
	static BOOL CBDisplayMemberDetailInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);	//	2005.07.06. By SungHoon
	static BOOL CBDisplayGuildListDetailInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);	//	2005.07.08. By SungHoon
	static BOOL CBDisplayGuildSelected(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl); // 2005.07.08. By SungHoon

	static BOOL CBDisplayGuildName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	
	static BOOL CBDisplayGuildMemberCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildWinPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBDisplayMemberSelect(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBDisplaySelectedMember(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayRecord(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);

	// From AgcmEventGuild
	static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBEventGrantWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDisplayDialogWorldChampionship(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBOnWorldChampionshipRequestBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBOnWorldChampionshipEnterBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	// From AgcmCharacter
	static BOOL CBSelfUpdatePosition	( PVOID pData , PVOID pClass , PVOID pCustData );

	// From AgpmGuild
	static BOOL CBCreateComplete		( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBJoinRequest			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBJoinReject			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBJoin					( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBLeave					( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBForcedLeave			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBDestroy				( PVOID pData , PVOID pClass , PVOID pCustData );	// 자기 길드가 해체되면, 멤버 데이터를 초기화 한다.
	static BOOL CBCharData				( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBUpdateMemberStatus	( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBUpdateNotice			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBSystemMessage			( PVOID pData , PVOID pClass , PVOID pCustData );

	static BOOL CBJointRequest			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBJoint					( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBJointLeave			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBHostileRequest		( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBHostile				( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBHostileLeaveRequest	( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBHostileLeave			( PVOID pData , PVOID pClass , PVOID pCustData );

	static BOOL CBGetJointMark			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL CBGetJointColor			( PVOID pData , PVOID pClass , PVOID pCustData );


	// 부길마 임명 및 해제 관련 패킷 처리 콜백		2008. 09. 03	
	static BOOL CBGuildAppointAsk		( PVOID pData , PVOID pClass , PVOID pCustData );	
	static BOOL CBGuildAppointSysMsg	( PVOID pData , PVOID pClass , PVOID pCustData );

	// 부길마 임명 및 해제 버튼 텍스트 변경 콜백	2008. 09. 09
	static BOOL CBDisplayAppointAndCancelBtnText( PVOID pClass , PVOID pData , AgcdUIDataType eType , INT32 lID , CHAR* szDIsplay , INT32* plValue , AgcdUIControl* pcsSourceControl );
	
	// 길드승계 패킷 처리 콜백			2008. 09. 03
	static BOOL CBGuildSuccessionAsk	( PVOID pData , PVOID pClass , PVOID pCustData );

	// 길드승계 완료시 멤버 랭크 변경 콜백 2008. 09. 18 
	static BOOL CBGuildMemberRankChange	( PVOID pData , PVOID pClass , PVOID pCustData );

	static BOOL CBEventReturnJointRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnJointAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnJointLeave(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnHostileRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnHostileAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnHostileLeaveRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnHostileLeaveAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	
	static BOOL CBOpenJointList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBOpenHostileList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBOnJointRequestBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBOnHostileRequestBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBJointDetail(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostileDetail(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL AddJointDetail(AgcdGuildList& stItem);
	BOOL SortJointDetail();
	BOOL AddHostileDetail(AgcdGuildList& stItem);
	BOOL SetGuildListToJointList();
	BOOL SetGuildListToHostileList();

	static BOOL CBDisplayRelationGuildID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBDisplayRelationMasterID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBDisplayJointRequestBtn(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBDisplayHostileRequestBtn(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);

	// From AgcmChatting2
	static BOOL CBGuildLeaveByCommand(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBProcessChatCommand(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL LeaveByCommand();

	///////////////////////////////////////////////////////////////////////////////////////
	// Battle 관련
	BOOL OpenBattleRequestConfirmUI(CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson);
	BOOL OpenBattleCancelRequestConfirmUI();
	BOOL OpenBattleCancelAcceptConfirmUI();
	BOOL OpenBattleWithdrawConfirmUI();

	BOOL UpdateBattleTime(AgpdGuild* pcsGuild);

	BOOL ProcessDrawBattleInfo(UINT32 ulClockCount);
	BOOL DrawBattleInfo(AgpdGuild* pcsGuild, AgcmUIGuildDrawBattleInfoType eType);
	BOOL DrawBattleInfoInit(AgpdGuild* pcsGuild);
	BOOL DrawBattleInfoAccept(AgpdGuild* pcsGuild);
	BOOL DrawBattleInfoStart(AgpdGuild* pcsGuild);
	BOOL DrawBattleInfoTime(AgpdGuild* pcsGuild);
	BOOL DrawBattleInfoScore(AgpdGuild* pcsGuild);
	BOOL DrawBattleInfoResult(AgpdGuild* pcsGuild);
	BOOL DrawBattleRount(UINT32 ulRount);

	// Ok Cancel Dialog For Battle
	static BOOL CBEventReturnBattleRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnBattleAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnBattleCancelRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnBattleCancelAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBEventReturnBattleWithdraw(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

	// Display
	static BOOL CBDisplayDialogEnemyGuildID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayDialogBattleType(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayDialogDuration(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayDialogBattlePerson(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	// From AgpmGuild For Battle
	static BOOL CBReceiveBattleRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleAccept(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleCancelRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleCancelAccept(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleCancelReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleStart(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleUpdateTime(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleUpdateScore(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleMemberList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveBattleRound(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBReceiveBattleInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGuildMemberSearch(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	// For AgcmUIGuildGuildList
	static BOOL CBReceiveGuildListInfo(PVOID pData, PVOID pClass, PVOID pCustData);		
	static BOOL CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildListSearch(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	
	static BOOL CBGuildBattleTimeOpenUIClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// For AgcmUIGuildMaxMember

	static BOOL CBGuildMaxMemberIncreaseOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildMaxMemberIncreaseOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMaxMemberIncreaseUIToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBEventReturnGuildMaxMemberIncreaseOK(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	
	INT32 CalcFirstPage(INT32 lCurrentPage, INT32 lMaxPage);

	
	static BOOL CBDisplayGuildListPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayGuildListMaxPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL CBDisplayGuildMemberListPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayGuildMemberListMaxPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	
	// for AgcmUIGuildNotice.cpp
	static BOOL CBGuildClickLeaveGuildNotice(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);


	// for AgcmUIGuildMember.cpp
	static BOOL CBGuildClickWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildClickInviteParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	
	static BOOL CBGuildJoinAllowMember(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBGuildLeaveAllowMember(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

	static BOOL CBEventReturnConfirmJoinRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

	// for AgcmUIGuildGuildList.cpp
	static BOOL CBDisplayGuildGuildBattleJoinRequest(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	
	static BOOL CBGuildWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBReceiveLeaveRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData);	

	
	static BOOL CBDisplayGuildAllowButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildAllowClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	VOID AllowSelectMember();

	VOID RefreshGuildMemberButton();
	
	INT32			m_lMaxMemberListPage ;
	INT32			m_lMaxGuildListPage ;

	INT32			m_lUDGuildListPage;	
	AgcdUIUserData* m_pstGuildListPage;	
	INT32			m_lUDGuildListMaxPage;				
	AgcdUIUserData* m_pstGuildListMaxPage;

	INT32			m_lUDMemberListPage;
	AgcdUIUserData* m_pstMemberListPage;
	INT32			m_lUDMemberListMaxPage;
	AgcdUIUserData* m_pstMemberListMaxPage;

private:
	VOID SelectMember(INT32 lSelectIndex);

	INT32 m_lEventOpenGuildRenameUI;
	static BOOL CBGuildRenameUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildRenameUIInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBGuildRenameOKClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);


	static BOOL CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData);		
	static BOOL CBGuildRenameCharID(PVOID pData, PVOID pClass, PVOID pCustData);		
	BOOL RenameCharID(CHAR *szOldID, CHAR *szNewID);			

//	For AgcmUIGuildMark.cpp	
public:
	BOOL	OpenGuildMarkUI();
	BOOL	CloseGuildMarkUI();
	VOID	InitGuildMarkGridItem();		
private:
	INT32 m_lEventGuildMarkUIOpen;				
	INT32 m_lEventGuildMarkUIClose;				

	static BOOL CBGuildCreateGuildMark(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildGuildMarkUIClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	AgpdGrid	m_stGuildMarkGrid;	

	VOID SetGridItem();		

	INT32 m_lGuildMarkLayerIndex;					

	AgcdUIUserData* m_pcsUDGuildMarkGrid;		
	AgcdUIUserData* m_pcsUDGuildMarkLayerIndex;		

	static BOOL CBIsGuildMarkSelected(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);	//	2005.10.10. By SungHoon
	static BOOL CBGuildGuildMarkUIInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildupdateGuildMarkLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	//	2005.10.10. By SungHoon

	static BOOL CBGuildMarkGridLClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	//	2005.10.11. By SungHoon

	AgpdGrid	m_stGuildMarkColorGrid;			
	AgpdGrid	m_stGuildMarkPreviewLargeGrid;	
	AgpdGrid	m_stGuildMarkPreviewSmallGrid;	

	AgpdGrid	m_stGuildMarkPreviewLargePatternGrid;	 
	AgpdGrid	m_stGuildMarkPreviewSmallPatternGrid;	 

	AgpdGrid	m_stGuildMarkPreviewLargeSymbolGrid;	 
	AgpdGrid	m_stGuildMarkPreviewSmallSymbolGrid;	 

	AgpdGridItem *m_pcsGuildMarkPreviewLargeGridItem;	 
	AgpdGridItem *m_pcsGuildMarkPreviewSmallGridItem;	 

	AgpdGridItem *m_pcsGuildMarkPreviewLargePatternGridItem;	 
	AgpdGridItem *m_pcsGuildMarkPreviewSmallPatternGridItem;	 

	AgpdGridItem *m_pcsGuildMarkPreviewLargeSymbolGridItem;	 
	AgpdGridItem *m_pcsGuildMarkPreviewSmallSymbolGridItem;	 

	AgcdUIUserData* m_pcsUDGuildMarkColorGrid;				 

	AgcdUIUserData* m_pcsUDGuildMarkPreviewLargeGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkPreviewSmallGrid;		 

	AgcdUIUserData* m_pcsUDGuildMarkPreviewLargePatternGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkPreviewSmallPatternGrid;		 

	AgcdUIUserData* m_pcsUDGuildMarkPreviewLargeSymbolGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkPreviewSmallSymbolGrid;		 

	static BOOL CBGuildMarkColorGridLClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	//	2005.10.11. By SungHoon

	VOID SetPreviewGuildMark();
	VOID DestroyPreviewGuildMark();

	INT32 m_lGuildMarkSelectedTID;
	INT32 m_lGuildMarkSelectedColor;

	INT32 m_lGuildMarkDetailInfo;							
	AgcdUIUserData *m_pcsUDGuildMarkDetailInfo;			
	static BOOL CBDisplayGuildMarkInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);


	static BOOL CBGuildGuildMarkClickInitialize(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildGuildMarkClickBuy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildGuildMarkClickBuyOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildBuyGuildMark(PVOID pData, PVOID pClass, PVOID pCustData);			//	2005.10.19. By SungHoon

	static BOOL CBGuildGuildMarkInitialize(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);		 
	static BOOL CBGuildGuildMarkDuplicate(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);		 
	static BOOL CBGuildGuildMarkBuyConfirmUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	BOOL GuildMarkBuyConfirmUIOpen();

	BOOL GuildMarkInitialize();

	BOOL m_bEnableBuyGuildMark;				 
	AgcdUIUserData* m_pstEnableBuyGuildMark;	 

	INT32		m_lEventGuildMarkBuyComplete;
	INT32		m_lEventGuildMarkDuplicate;
	INT32		m_lEventGuildMarkNoExistBottom;
	INT32		m_lEventGuildMarkInitialize;
	INT32		m_lEventGuildMarkInitializeComplete;
	INT32		m_lEventGuildMarkBuy;

	AgpdGrid	m_stGuildMarkPreviewBuyGrid;	 
	AgpdGrid	m_stGuildMarkPreviewBuyPatternGrid;	 
	AgpdGrid	m_stGuildMarkPreviewBuySymbolGrid;	 

	AgpdGridItem *m_pcsGuildMarkPreviewBuyGridItem;	 
	AgpdGridItem *m_pcsGuildMarkPreviewBuyPatternGridItem;	 
	AgpdGridItem *m_pcsGuildMarkPreviewBuySymbolGridItem;	 

	AgcdUIUserData* m_pcsUDGuildMarkPreviewBuyGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkPreviewBuyPatternGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkPreviewBuySymbolGrid;		 

protected:
	AgpdGrid	m_stGuildMarkMainGrid;	 
	AgpdGrid	m_stGuildMarkMainPatternGrid;	 
	AgpdGrid	m_stGuildMarkMainSymbolGrid;	 

	AgpdGridItem *m_pcsGuildMarkMainGridItem;	 
	AgpdGridItem *m_pcsGuildMarkMainPatternGridItem;	 
	AgpdGridItem *m_pcsGuildMarkMainSymbolGridItem;	 

	AgcdUIUserData* m_pcsUDGuildMarkMainGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkMainPatternGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkMainSymbolGrid;		 

//	For AgcmUIGuildList.cpp
	AgpdGrid	m_stGuildMarkGuildListGrid;	 
	AgpdGrid	m_stGuildMarkGuildListPatternGrid;	 
	AgpdGrid	m_stGuildMarkGuildListSymbolGrid;	 

	AgpdGridItem *m_pcsGuildMarkGuildListGridItem;	 
	AgpdGridItem *m_pcsGuildMarkGuildListPatternGridItem;	 
	AgpdGridItem *m_pcsGuildMarkGuildListSymbolGridItem;	 

	AgcdUIUserData* m_pcsUDGuildMarkGuildListGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkGuildListPatternGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkGuildListSymbolGrid;		 

//	For AgcmUIGuildDestroyConfirm.cpp
	AgpdGrid	m_stGuildMarkGuildDestroyGrid;						 
	AgpdGrid	m_stGuildMarkGuildDestroyPatternGrid;				 
	AgpdGrid	m_stGuildMarkGuildDestroySymbolGrid;				 

	AgpdGridItem *m_pcsGuildMarkGuildDestroyGridItem;				 
	AgpdGridItem *m_pcsGuildMarkGuildDestroyPatternGridItem;		 
	AgpdGridItem *m_pcsGuildMarkGuildDestroySymbolGridItem		;	 

	AgcdUIUserData* m_pcsUDGuildMarkGuildDestroyGrid;				 
	AgcdUIUserData* m_pcsUDGuildMarkGuildDestroyPatternGrid;		 
	AgcdUIUserData* m_pcsUDGuildMarkGuildDestroySymbolGrid;			 
private:
	BOOL GuildListGuildMarkDraw( INT32 lGuildMarkTID, INT32 lGuildMarkColor );
	BOOL GuildDestroyConfirmGuildMarkDraw( INT32 lGuildMarkTID, INT32 lGuildMarkColor );

	BOOL GuildMarkDraw(AgpdGridItem **ppGridItem, AgcdUIUserData **ppGridConrtol, INT32 lGuildMarkTID, INT32 lGuildMarkColor);

	INT32 GetCurrentTabIndex();

	VOID InitSeletedGridItem();

protected:
	BOOL MakeCommaNumber( CONST CHAR *szNumber, CHAR *szTargetNumber, INT16 lTargetLength );
	BOOL MakeCommaNumber( INT32 lNumber, CHAR *szTargetNumber, INT16 lTargetLength );

	INT32 m_lEventGuildMarkNotEnoughtGuildMemberCount;				 
	INT32 m_lEventGuildMemberUpNotEnoughtGuildMemberCount;			 
	INT32 m_lEventGuildBattleFailRequestSelfGuild;					
};

#endif //_AGCMUIGUILD_H_