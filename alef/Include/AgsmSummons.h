// AgsmSummons.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2005. 08. 01.

#ifndef _AGSMSUMMONS_H_
#define _AGSMSUMMONS_H_

#include "AuPacket.h"

#include "ApmEventManager.h"

#include "AgpmAI2.h"
#include "AgpmEventSpawn.h"
#include "AgpmSummons.h"
#include "AgpmArchlord.h"

#include "AgsmSystemMessage.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"

#include "AuRandomNumber.h"

const double AGSMSUMMON_SUMMON_POS_WIDTH					= 400.0;

typedef enum _eAgsmSummonsCB
{
	AGSMSUMMONS_CB_NEW_SUMMONS = 1,
	AGSMSUMMONS_CB_NEW_TAMING,
	AGSMSUMMONS_CB_NEW_FIXED,
	AGSMSUMMONS_CB_END_SUMMONS,
	AGSMSUMMONS_CB_END_TAMING,
	AGSMSUMMONS_CB_END_FIXED,
	AGSMSUMMONS_CB_END,
} eAgsmSummonsCB;

typedef enum _eAgsmSummonsRemoveType
{
	AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS		= (1 << 0),
	AGSMSUMMONS_CB_REMOVE_TYPE_TAME			= (1 << 1),
	AGSMSUMMONS_CB_REMOVE_TYPE_FIXED		= (1 << 2),
	AGSMSUMMONS_CB_END_PERIOD_START			= (1 << 3),
} eAgsmSummonsRemoveType;

class AgsmEventSpawn;

class AgsmSummons : public AgsModule
{
private:
	ApmEventManager* m_papmEventManager;

	AgpmCharacter* m_pagpmCharacter;
	AgpmAI2* m_pagpmAI2;
	AgpmEventSpawn* m_pagpmEventSpawn;
	AgsmEventSpawn* m_pagsmEventSpawn;
	AgpmSummons* m_pagpmSummons;
	AgpmArchlord* m_pagpmArchlord;

	AgsmSystemMessage* m_pagsmSystemMessage;
	AgsmCharacter* m_pagsmCharacter;
	AgsmCharManager* m_pagsmCharManager;

	MTRand m_csRandom;

public:
	AgsmSummons();
	virtual ~AgsmSummons();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle2(UINT32 ulClockCount);
	BOOL OnDestroy();

	INT32 ProcessSummons(AgpdCharacter* pcsOwner, INT32 lSummonsTID, UINT32 ulDuration, INT32 lCount = 1, INT32 lSkillLevel = 1,
						EnumAgpdSummonsType eType = AGPMSUMMONS_TYPE_ELEMENTAL, EnumAgpdSummonsPropensity ePropensity = AGPMSUMMONS_PROPENSITY_DEFENSE,
						BOOL bForce = FALSE, BOOL bSendPacket = TRUE);
	INT32 ProcessSummons(AuPOS stSummonPos, AgpdCharacter* pcsOwner, INT32 lSummonsTID, UINT32 ulDuration, INT32 lCount = 1, INT32 lSkillLevel = 1,
						EnumAgpdSummonsType eType = AGPMSUMMONS_TYPE_ELEMENTAL, EnumAgpdSummonsPropensity ePropensity = AGPMSUMMONS_PROPENSITY_DEFENSE,
						BOOL bForce = FALSE, BOOL bSendPacket = TRUE);
	INT32 ProcessTaming(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, UINT32 ulDuration, INT32 lSkillLevel = 1, BOOL bForce = FALSE);
	INT32 ProcessFixed(AgpdCharacter* pcsOwner, INT32 lFixedTID, UINT32 ulDuration, INT32 lCount = 1, INT32 lSkillLevel = 1, BOOL bForce = FALSE);

	AuPOS GetRandomPos(const AuPOS& stPosBase, double dWidth = 0.0f);
	AuPOS GetFrontofCharacterPos(AgpdCharacter* pcsTarget, double dWidth = AGSMSUMMON_SUMMON_POS_WIDTH);

	BOOL CheckSummonsPeriod(AgpdCharacter* pcsCharacter, UINT32 ulClockCount);
	BOOL EndSummonsPeriod(AgpdCharacter* pcsSummons);
	static BOOL CBRemoveCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	BOOL RemoveAllSummons(AgpdCharacter* pcsCharacter, INT32 lRemoveType);
	BOOL RemoveSummonsByTID(AgpdCharacter* pcsCharacter, INT32 lTID);
	BOOL ResetTame(AgpdCharacter* pcsTame, INT32 lOwnerCID);

	BOOL UpdateTransparentToSummons(AgpdCharacter* pcsCharacter, BOOL bSet);

	BOOL SendNewSummons(AgpdCharacter* pcsOwner, INT32 lSummonsCID, INT32 lSummonsTID, INT8 cSummonsType,
						INT8 cSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce);
	BOOL SendRemoveSummons(AgpdCharacter* pcsOwner, INT32 lSummonsCID);
	BOOL SendSetPropensity(AgpdCharacter* pcsOwner, AgpdCharacter* pcsSummons);
	BOOL SendSetMaxSummons(AgpdCharacter* pcsCharacter, INT32 lMaxCount);
	BOOL SendSetMaxTame(AgpdCharacter* pcsCharacter, INT32 lMaxCount);

	BOOL SendSummonsCheckResult(UINT32 ulNID, eAgpmSummonsSummonsCheckResult eCheckResult);
	BOOL SendTameCheckResult(UINT32 ulNID, eAgpmSummonsTamableCheckResult eCheckResult);
	BOOL SendFixedCheckResult(UINT32 ulNID, eAgpmSummonsFixedCheckResult eCheckResult);

	BOOL SetCallbackNewSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNewTaming(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNewFixed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndTaming(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndFixed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndPeriodStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CBSetPropensity(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveSummonsByLevelLimitRegion(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	RemoveSummonsAfterTerm(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);		// 2007.04.09. steeple

};

#endif//_AGSMSUMMONS_H_
