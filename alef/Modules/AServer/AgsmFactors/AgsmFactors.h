/******************************************************************************
Module:  AgsmFactors.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 10. 16
******************************************************************************/

#if !defined(__AGSMFACTORS_H__)
#define __AGSMFACTORS_H__

#include "ApBase.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmTitle.h"
#include "AgsEngine.h"
#include "AuRandomNumber.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmFactorsD" )
#else
#pragma comment ( lib , "AgsmFactors" )
#endif
#endif


typedef enum _eAgsmFactorsCB {
	AGSM_FACTORS_CB_FACTOR_DB			= 0,

	/*
	AGSM_FACTORS_CB_DB_STREAM_INSERT,
	AGSM_FACTORS_CB_DB_STREAM_DELETE,
	AGSM_FACTORS_CB_DB_STREAM_SELECT,
	AGSM_FACTORS_CB_DB_STREAM_UPDATE
	*/
} eAgsmFactorsCB;

typedef enum _eAgsmFactorsAttackType {
	AGSM_FACTORS_MELEE_ATTACK			= 0,
	AGSM_FACTORS_SKILL_ATTACK
} eAgsmFactorsAttackType;


// 주기적으로 Max보다 적은 HP, MP, SP를 가지고 있는 캐릭터들의 값을 올려준다. 그때 사용되는 값들이다.
// 보통일때 (즉 움직이고 있을때)
const int	AGSMFACTORS_RECOVERY_POINT_HP						= 7;
const int	AGSMFACTORS_RECOVERY_POINT_MP						= 4;
const int	AGSMFACTORS_RECOVERY_POINT_SP						= 4;

// 아무짓도 안하고 있을때...
const int	AGSMFACTORS_RECOVERY_POINT_NOT_ACTION_HP			= 11;
const int	AGSMFACTORS_RECOVERY_POINT_NOT_ACTION_MP			= 6;
const int	AGSMFACTORS_RECOVERY_POINT_NOT_ACTION_SP			= 6;

class AgsmFactors : public AgsModule {
private:
	AgpmFactors		*m_pagpmFactors;
	AgpmCharacter	*m_pagpmCharacter;
	AgpmItem		*m_pagpmItem;
	AgpmTitle		*m_pagpmTitle;

	MTRand			m_csRandom;

public:
	AgsmFactors();
	~AgsmFactors();

	// Virtual Function 들
	BOOL	OnInit();
	BOOL	OnDestroy();

	// 죽었음 TRUE, 살아있음 FALSE 리턴
	BOOL IsDead(AgpdFactor *pcsFactor);
	BOOL Resurrection(AgpdFactor *pcsFactor, PVOID *pvPacket);

	PVOID SetCharPoint(AgpdFactor *pcsFactor, BOOL bResultFactor = TRUE, BOOL bMakePacket = TRUE);
	PVOID SetCharPointFull(AgpdFactor *pcsFactor);

	BOOL ResetCharMaxPoint(AgpdFactor *pcsFactor);
	BOOL AdjustCharMaxPoint(AgpdFactor *pcsFactor);
	BOOL AdjustCharMaxPoint(AgpdFactor *pcsBaseFactor, AgpdFactor *pcsTargetFactor);
	BOOL AdjustCharMaxPointFromBaseFactor(AgpdFactor* pcsUpdateFactor, AgpdFactor* pcsBaseFactor);	// 2005.06.29. steeple
	BOOL AdjustWeaponFactor(AgpdCharacter* pcsCharacter, AgpdFactor* pcsUpdateResultFactor);

	PVOID AddCharPoint(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsFromResultFactor = FALSE, FLOAT fApplyPercent = 1.0f);
	PVOID AddCharPointByPercent(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsFromResultFactor = FALSE, FLOAT fApplyPercent = 1.0f);
	BOOL UpdateCharPoint(AgpdFactor *pcsFactor, PVOID *pvPacket, INT32 lHP, INT32 lMP, INT32 lSP);

	BOOL RecoveryPointInterval(AgpdFactor *pcsFactor, PVOID *pvPacket, INT32 lModifiedPointHP, INT32 lModifiedPointMP, BOOL bIsRecoveryHP = TRUE, BOOL bIsRecoveryMP = TRUE);
	BOOL SaveResultPoint(AgpdFactor *pcsFactor);
	// 각종 필요 펙터들을 계산해서 리턴해준다.
	//	    다른 모듈들에서 펙터에 있는 값을 필요로 할 경우 여기에 인터페이스를 만들어 (필요하다면 계산까지 다 해서) 넘겨준다.
	INT16	GetAttackSpeed(AgpdFactor *pcsFactor);

	PVOID	MakePacketSyncParty(AgpdFactor *pcsFactor);
	PVOID	MakePacketUpdateLevel(AgpdFactor *pcsFactor);
	PVOID	MakePacketUpdateUnionRank(AgpdFactor *pcsFactor);
	PVOID	MakePacketDBData(AgpdFactor *pcsFactor);

	FLOAT	Max(FLOAT lValue1, FLOAT lValue2);
	FLOAT	Min(FLOAT lValue1, FLOAT lValue2);

	BOOL	ResetResultFactorDamageInfo(AgpdFactor *pcsFactor);
};

#endif //__AGSMFACTORS_H__
