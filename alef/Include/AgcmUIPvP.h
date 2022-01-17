#ifndef	__AGCMUIPVP_H__
#define	__AGCMUIPVP_H__

#include "ApmMap.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"

#include "AgcmUIManager2.h"
#include "AgcmUIMain.h"
#include "AgcmUICharacter.h"
#include "AgcmChatting2.h"
#include "AgcmWorld.h"

#include "AgpmItem.h"
#include "AgpmPvP.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIPvPD" )
#else
#pragma comment ( lib , "AgcmUIPvP" )
#endif
#endif

#define UI_MESSAGE_ID_ENTER_SAFE_AREA			"PvP_Msg_EnterSafeArea"
#define UI_MESSAGE_ID_ENTER_FREE_AREA			"PvP_Msg_EnterFreeArea"
#define UI_MESSAGE_ID_ENTER_FREE_AREA_2			"PvP_Msg_EnterFreeArea_2"
#define UI_MESSAGE_ID_ENTER_FREE_AREA_3			"PvP_Msg_EnterFreeArea_3"
#define UI_MESSAGE_ID_ENTER_FREE_AREA_4			"PvP_Msg_EnterFreeArea_4"
#define UI_MESSAGE_ID_ENTER_COMBAT_AREA			"PvP_Msg_EnterCombatArea"
#define UI_MESSAGE_ID_ENTER_COMBAT_AREA_2		"PvP_Msg_EnterCombatArea_2"
#define UI_MESSAGE_ID_ENTER_COMBAT_AREA_3		"PvP_Msg_EnterCombatArea_3"
#define UI_MESSAGE_ID_ENTER_COMBAT_AREA_4		"PvP_Msg_EnterCombatArea_4"
#define UI_MESSAGE_ID_ENTER_COMBAT_AREA_5		"PvP_Msg_EnterCombatArea_5"
#define UI_MESSAGE_ID_ENTER_COMBAT_AREA_6		"PvP_Msg_EnterCombatArea_6"
#define UI_MESSAGE_ID_NOW_PVP_STATUS			"PvP_Msg_NowPvPStatus"
#define UI_MESSAGE_ID_NONE_PVP_STATUS			"PvP_Msg_NonePvPStatus"
#define UI_MESSAGE_ID_DEAD						"PvP_Msg_Dead"
#define UI_MESSAGE_ID_DEAD_GUILD_MEMBER			"PvP_Msg_DeadGuildMember"
#define UI_MESSAGE_ID_DEAD_PARTY_MEMBER			"PvP_Msg_DeadPartyMember"
#define UI_MESSAGE_ID_ITEM_DROP					"PvP_Msg_ItemDrop"
#define UI_MESSAGE_ID_ITEM_ANTI_DROP			"PvP_Msg_ItemAntiDrop"
#define UI_MESSAGE_ID_START_INVINCIBLE			"PvP_Msg_StartInvincible"
#define UI_MESSAGE_ID_END_INVINCIBLE			"PvP_Msg_EndInvincible"
#define UI_MESSAGE_ID_CANNOT_USE_TELEPORT		"PvP_Msg_CannotUseTeleport"
#define UI_MESSAGE_ID_KILL_PLAYER				"PvP_Msg_KillPlayer"
#define UI_MESSAGE_ID_MOVE_BATTLE_SQAURE		"PvP_Msg_MoveBattleSqaure"
#define UI_MESSAGE_ID_LEAVE_GUILD_OR_PARTY		"PvP_Msg_LeaveGuildOrParty"
#define UI_MESSAGE_ID_LEAVE_GUILD_OR_PARTY_2	"PvP_Msg_LeaveGuildOrParty_2"
#define UI_MESSAGE_ID_CANNOT_INVITE_GUILD		"PvP_Msg_CannotInviteGuild"
#define UI_MESSAGE_ID_CANNOT_INVITE_PARTY		"PvP_Msg_CannotInviteParty"
#define UI_MESSAGE_ID_TARGET_INVINCIBLE			"PvP_Msg_TargetInvincible"
#define UI_MESSAGE_ID_NOT_ENOUGH_LEVEL			"PvP_Msg_NotEnoughLevel"
#define UI_MESSAGE_ID_CANNOT_INVITE_MEMBER		"PvP_Msg_CannotInviteMember"
#define UI_MESSAGE_ID_CANNOT_ATTACK_FRIEND		"PvP_Msg_CannotAttackFriend"
#define UI_MESSAGE_ID_CANNOT_ATTACK_OTHER		"PvP_Msg_CannotAttackOther"
#define UI_MESSAGE_ID_GET_MURDERERPOINT			"PvP_Msg_GetMurdererPoint"
#define UI_MESSAGE_ID_CHARISMA_GET_POINT		"CharismaGetPoint"
#define UI_MESSAGE_ID_CANNOT_PVP_BY_LEVEL		"CannotPvPByLevel"
#define UI_MESSAGE_ID_DISABLE_NORMAL_ATTACK		"DisableNormalAttack"
#define UI_MESSAGE_ID_DISABLE_SKILL_CAST		"DisableSkillCast"


#define UI_MESSAGE_ID_SKILL_CANNOT_APPLY_EFFECT	"Skill_Cannot_Apply_Effect"	// ÀÓ½Ã ¶«»§ -_-;;  2005.01.31.

const UINT32 AGCMUIPVP_IDLE_CLOCK_INTERVAL		= 500;

class AgcmUIPvP : public AgcModule {
private:
	ApmMap					*m_pcsApmMap;
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmGrid				*m_pcsAgpmGrid;
	AgpmItem				*m_pcsAgpmItem;
	AgpmPvP					*m_pcsAgpmPvP;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmWorld				*m_pcsAgcmWorld;

	AgcmUIControl			*m_pcsAgcmUIControl;
	AgcmUIManager2			*m_pcsAgcmUIManager2;
	AgcmUIMain				*m_pcsAgcmUIMain;
	AgcmUICharacter			*m_pcsAgcmUICharacter;
	AgcmChatting2			*m_pcsAgcmChatting;



	AgpdGridItem			*m_pcsPvPFreeGridItem;
	AgpdGridItem			*m_pcsPvPDangerGridItem;

	BOOL					m_bIsAddPvPFreeItem;
	BOOL					m_bIsAddPvPDangerItem;

	INT32					m_lEventRecvCannotUseTeleport;

	AgcdUIUserData			*m_pcsIsDisableInPvPDanger;
	BOOL					m_bIsDisableInPvPDanger;

	INT32					m_lEventRegionSafe;
	INT32					m_lEventRegionPvPFree;
	INT32					m_lEventRegionPvPDanger;
	
	INT32					m_lEventStartPvP;
	INT32					m_lEventEndPvP;

	BOOL					m_bIsInPvPDanger;
	INT64					m_llCurrentEXP;
	
	UINT32					m_ulRaceBattleRemainedTime;
	UINT32					m_ulPreviousClock;
	

public:
	AgcmUIPvP();
	virtual ~AgcmUIPvP();
	
	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();
	BOOL					OnIdle(UINT32 ulClockCount);

	static BOOL				CBChangeRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateExp(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBRecvCannotUseTeleport(PVOID pData, PVOID pClass, PVOID pCustData);

	// 2005.01.18. steeple
	static BOOL				CBEnterSafeArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBEnterFreeArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBEnterCombatArea(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBUpdateResurrectStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	
	static BOOL				CBRaceBattleStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	// 2005.01.18. steeple
	static BOOL				CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL					AddSystemMessage(CHAR* szMsg, DWORD dwColor = 0XFFFFFF00);

	// race battle
	BOOL	OnRaceBattleStatus(INT32 lRemained);
	void	InitBattleInfo();
	BOOL	DrawBattleInfoResult();	
	BOOL	DrawBattleInfoTime();
	BOOL	DrawBattleInfoScore();
};

#endif	//__AGCMUIPVP_H__