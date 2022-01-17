/******************************************************************************
Module:  AgsmCombat.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 14
******************************************************************************/

#if !defined(__AGSMCOMBAT_H__)
#define __AGSMCOMBAT_H__

#include "ApBase.h"
#include "AsDefine.h"
#include "ApAdmin.h"

#include "AgsEngine.h"

#include "AgpmGrid.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmCombat.h"
#include "AgpmEventNPCDialog.h"
#include "AgpmOptimizedPacket2.h"
#include "AgpmAI2.h"
#include "AgpmSummons.h"
#include "AgpmSiegeWar.h"

#include "AgsmAOIFilter.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmFactors.h"
#include "AgsmParty.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmCombatD" )
#else
#pragma comment ( lib , "AgsmCombat" )
#endif
#endif

typedef enum _eAgsmCombatCB {
	AGSMCOMBAT_CB_ID_ATTACK				= 0,
	AGSMCOMBAT_CB_ID_CHECK_DEFENSE,
	AGSMCOMBAT_CB_ID_DAMAGE_ADJUST,
	AGSMCOMBAT_CB_ID_PRE_CHECK_COMBAT,
	AGSMCOMBAT_CB_ID_POST_CHECK_COMBAT,
	AGSMCOMBAT_CB_ID_NPC_DIALOG,			//NPC독백용. 콜백
	AGSMCOMBAT_CB_ID_CONVERT_DAMAGE,
	AGSMCOMBAT_CB_ID_ATTACK_START,			// PvP용
	AGSMCOMBAT_CB_ID_DAMAGE_ADJUST_SIEGEWAR,
} eAgsmCombatCB;

typedef enum _AgsmCombatMode {
	AGSMCOMBAT_PvP				= 1,
	AGSMCOMBAT_PvM,
	AGSMCOMBAT_UvU
} AgsmCombatMode;



const int	AGSMCOMBAT_CHECKRESULT_FAILED_ATTACK				= 0x00000001;
const int	AGSMCOMBAT_CHECKRESULT_MELEE_COUNTERATTACK			= 0x00000002;
const int	AGSMCOMBAT_CHECKRESULT_MELEE_CRITICAL_STRIKE		= 0x00000004;
const int	AGSMCOMBAT_CHECKRESULT_MELEE_DEATH_STRIKE			= 0x00000008;
const int	AGSMCOMBAT_CHECKRESULT_EVADE_ATTACK					= 0x00000010;
const int	AGSMCOMBAT_CHECKRESULT_DODGE_ATTACK					= 0x00000020;
const int	AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_HP			= 0x00000040;
const int	AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_MP			= 0x00000080;
const int	AGSMCOMBAT_CHECKRESULT_REFLECT_DAMAGE				= 0x00000100;
const int	AGSMCOMBAT_CHECKRESULT_REDUCE_DAMAGE				= 0x00000200;
const int	AGSMCOMBAT_CHECKRESULT_CHARGE						= 0x00000400;
const int	AGSMCOMBAT_CHECKRESULT_REFLECT_SPECIAL_STATUS		= 0x00001000;
const int	AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_HP		= 0x00002000;
const int	AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_MP		= 0x00004000;
const int	AGSMCOMBAT_CHECKRESULT_LENS_STONE					= 0x00008000;
const int	AGSMCOMBAT_CHECKRESULT_DIVIDE						= 0x00010000;
const int   AGSMCOMBAT_CHECKRESULT_IGNORE_PHYSICAL_DEFENCE		= 0x00020000;
const int	AGSMCOMBAT_CHECKRESULT_IGNORE_ATTRIBUTE_DEFENCE		= 0x00040000;


typedef struct _stAgsmCombatAttackResult {
	AgpdCharacter	*pAttackChar;
	AgpdCharacter	*pTargetChar;
	INT32			nDamage;
	INT32			nHeroicDamage;
	UINT32			ulCheckResult;
	INT32			lSkillTID;

	INT32			lDivideAttrRate;
	INT32			lDivideAttrCaster;
	INT32			lDivideNormalRate;
	INT32			lDivideNormalCaster;
} stAgsmCombatAttackResult, *pstAgsmCombatAttackResult;


class AgsmCombat : public AgsModule {
private:
	AgpmGrid*		m_pagpmGrid;
	AgpmFactors*	m_pagpmFactors;
	AgpmCharacter*	m_pagpmCharacter;
	AgpmItem*		m_pagpmItem;
	AgpmCombat*		m_pagpmCombat;
	AgpmOptimizedPacket2*	m_pagpmOptimizedPacket2;
	AgpmAI2			*m_pagpmAI2;
	AgpmSummons*	m_pagpmSummons;
	AgpmSiegeWar*	m_pagpmSiegeWar;

	AgsmAOIFilter*	m_pagsmAOIFilter;
	AgsmCharacter*	m_pagsmCharacter;
	AgsmItem*		m_pagsmItem;
	AgsmFactors*	m_pagsmFactors;
	AgsmParty*		m_pagsmParty;

	PVOID			MakePacketAttackResult(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, PVOID pvPacketFactor, INT8 cAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT16 *pnPacketLength);

public:
	AgsmCombat();
	~AgsmCombat();

	BOOL OnValid(CHAR* pszData, INT16 nSize);

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL ApplyAttackDamage(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, INT32 lTotalDamage, INT32 lDamageNormal, AgpdFactor *pcsUpdateFactor, AgpdCharacterActionResultType eAttackResult, UINT8 cComboInfo = 0, BOOL bForceAttack = FALSE, INT32 lSkillTID = AP_INVALID_SKILLID, UINT32 ulAdditionalEffect = 0, UINT8 cHitIndex = 0);
	
	INT32 AdjustHeightDamage(AgpdCharacter* pAttackChar, AgpdCharacter* pTargetChar, INT32 lDamage);
	INT32 ProcessDivide(stAgsmCombatAttackResult& stAttackResult, AgpdFactor* pcsUpdateFactor, INT32* plTotalDamage);
	BOOL IsAttrInvincible(AgpdCharacter* pcsCharacter);

	//BOOL SendAttack(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar);
	BOOL SendAttackResult(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, PVOID pvPacketFactor, INT8 cAttackResult, UINT8 cComboInfo = 0, BOOL bForceAttack = FALSE, INT32 lSkillTID = AP_INVALID_SKILLID, UINT32 ulAdditionalEffect = 0, UINT8 cHitIndex = 0);
	BOOL SendAttackResultNotEnoughArrow(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar);
	BOOL SendAttackResultNotEnoughBolt(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar);
	BOOL SendAttackResultNotEnoughMP(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar);

	BOOL SetCallbackAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCheckDefense(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDamageAdjust(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPreCheckCombat(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPostCheckCombat(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNPCDialog(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackConvertDamage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAttackStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDamageAdjustSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CBActionAttack(PVOID pData, PVOID pClass, PVOID pCustData);

	// Script function
	void KillMonster(int attackCID, int monsterCID);
};

#endif //__AGSMCOMBAT_H__