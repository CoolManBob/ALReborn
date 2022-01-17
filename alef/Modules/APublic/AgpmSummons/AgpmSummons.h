// AgpmSummons.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2005. 07. 26.

#ifndef _AGPMSUMMONS_H_
#define _AGPMSUMMONS_H_

#include "ApModule.h"
#include "AuPacket.h"
#include "AuRandomNumber.h"

#include "AgpmCharacter.h"
#include "AgpmFactors.h"
#include "AgpmSystemMessage.h"
#include "AgpdSummons.h"

typedef enum _eAgpmSummonsPacketType
{
	AGPMSUMMONS_PACKET_NEW_SUMMONS = 0,
	AGPMSUMMONS_PACKET_REMOVE_SUMMONS,
	AGPMSUMMONS_PACKET_SET_PROPENSITY,
	AGPMSUMMONS_PACKET_SET_MAX_SUMMONS,
	AGPMSUMMONS_PACKET_SET_MAX_TAME,
} eAgpmSummonsPacketType;

typedef enum _eAgpmSummonsCallback
{
	AGPMSUMMONS_CB_NEW_SUMMONS = 0,
	AGPMSUMMONS_CB_REMOVE_SUMMONS,
	AGPMSUMMONS_CB_SET_PROPENSITY,
	AGPMSUMMONS_CB_SET_MAX_SUMMONS,
	AGPMSUMMONS_CB_SET_MAX_TAME,
	AGPMSUMMONS_CB_REMOVE_SUMMONS_BY_LEVEL_LIMIT_REGION,
} eAgpmSummonsCallback;

typedef enum _eAgpmSummonsSummonsCheckResult
{
	AGPMSUMMONS_SUMMONS_CHECK_RESULT_SUCCESS = 1,
	AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_ABNORMAL,
	AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_MAX_COUNT,
	AGPMSUMMONS_SUMMONS_CHECK_RESULT_FAILURE_LEVEL_LIMIT_REGION,
} eAgpmSummonsSummonsCheckResult;

typedef enum _eAgpmSummonsTamableCheckResult
{
	AGPMSUMMONS_TAMABLE_CHECK_RESULT_SUCCESS = 1,
	AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MAX_COUNT,
	AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_INVALID_TARGET,
	AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_ALREADY_TAMING,
	AGPMSUMMONS_TAMABLE_CHECK_RESULT_FAILURE_MISS,
} eAgpmSummonsTamableCheckResult;

typedef enum _eAgpmSummonsFixedCheckResult
{
	AGPMSUMMONS_FIXED_CHECK_RESULT_SUCCESS = 1,
	AGPMSUMMONS_FIXED_CHECK_RESULT_FAILURE_ABNORMAL,
	AGPMSUMMONS_FIXED_CHECK_RESULT_FAILURE_MAX_COUNT,
} eAgpmSummonsFixedCheckResult;

class AgpmSummons : public ApModule
{
private:
	AgpmCharacter* m_pagpmCharacter;
	AgpmFactors* m_pagpmFactors;
	AgpmSystemMessage* m_pagpmSystemMessage;

	INT16 m_nIndexADCharacter;

public:
	AuPacket m_csPacket;

private:
	BOOL SetOwnerAndLifeTime(AgpdCharacter* pcsSummons, INT32 lOwnerCID, INT32 lOwnerTID, EnumAgpdSummonsType eSummonsType, 
								EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime);

	// OnReceive
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnReceiveNewSummons(INT32 lOwnerCID, INT32 lSummonsCID, INT32 lSummonsTID, EnumAgpdSummonsType eSummonsType, 
								EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime,
								BOOL bForce);
	BOOL OnReceiveRemoveSummons(INT32 lOwnerCID, INT32 lSummonsCID);
	BOOL OnReceiveSetPropensity(INT32 lOwnerCID, INT32 lSummonsCID, EnumAgpdSummonsPropensity eSummonsPropensity);
	BOOL OnReceiveSetMaxSummons(INT32 lOwnerCID, INT8 cValue);
	BOOL OnReceiveSetMaxTame(INT32 lOwnerCID, INT8 cValue);

public:
	AgpmSummons();
	virtual ~AgpmSummons();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();

	// Attached Data
	AgpdSummonsADChar* GetADCharacter(AgpdCharacter* pData);
	static BOOL ConAgpdSummonsADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgpdSummonsADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBResetMonster(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL NewSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsSummons, INT32 lSummonsTID, 
					EnumAgpdSummonsType eSummonsType, EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce);
	BOOL NewTaming(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTame, INT32 lTameTID, 
					EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce);
	BOOL NewFixed(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFixed, INT32 lFixedTID, 
					EnumAgpdSummonsPropensity eSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce);

	BOOL RemoveCharacter(AgpdCharacter* pcsCharacter);
	BOOL RemoveSummons(AgpdCharacter* pcsCharacter, INT32 lSummonCID);
	BOOL RemoveSummonsInOwner(AgpdCharacter* pcsOwner, INT32 lSummonsCID);
	BOOL RemoveTameInOwner(AgpdCharacter* pcsOwner, INT32 lTameCID);
	BOOL RemoveFixedInOwner(AgpdCharacter* pcsOwner, INT32 lFixedCID);
	BOOL RemoveOwner(AgpdCharacter* pcsSummons, INT32 lOwnerCID);

	// Summons 와 Taming 은 확실하게 분리하자. 
	eAgpmSummonsSummonsCheckResult IsSummonsEnable(AgpdCharacter* pcsCharacter, BOOL bForce = FALSE);
	eAgpmSummonsSummonsCheckResult IsSummonsEnableRegion(AgpdCharacter* pcsCharacter, INT32 lSummonTID, AgpdCharacter* pcsSummon = NULL);
	INT32 GetSummonsCount(AgpdCharacter* pcsCharacter);
	INT32 GetMaxSummonsCount(AgpdCharacter* pcsCharacter);
	BOOL AddMaxSummonsCount(AgpdCharacter* pcsCharacter, INT8 cValue);
	BOOL SetMaxSummonsCount(AgpdCharacter* pcsCharacter, INT8 cValue);
	BOOL IsMySummons(AgpdCharacter* pcsCharacter, INT32 lSummonsCID);
	INT32 GetSummonsTIDCount(AgpdCharacter* pcsCharacter, INT32 lSummonsTID);
	BOOL UpdateAllSummonsPosToOwner(AgpdCharacter* pcsCharacter);

	eAgpmSummonsTamableCheckResult IsTameEnable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bCheckFormula = FALSE, BOOL bForce = FALSE);
	INT32 GetTameCount(AgpdCharacter* pcsCharacter);
	INT32 GetMaxTameCount(AgpdCharacter* pcsCharacter);
	BOOL SetMaxTameCount(AgpdCharacter* pcsCharacter, INT8 cValue);
	INT32 GetTameTIDCount(AgpdCharacter* pcsCharacter, INT32 lTameTID);

	// 2005.10.04. steeple. Fixed 추가
	eAgpmSummonsFixedCheckResult IsFixedEnable(AgpdCharacter* pcsCharacter, BOOL bForce = FALSE);
	INT32 GetAllFixedCount(AgpdCharacter* pcsCharacter);
	INT32 GetEachFixedCount(AgpdCharacter* pcsCharacter, INT32 lFixedTID);
	INT32 GetMaxFixedCount(AgpdCharacter* pcsCharacter);
	INT32 GetFixedTIDCount(AgpdCharacter* pcsCharacter, INT32 lFixedTID);

	INT32 GetOwnerCID(AgpdCharacter* pcsCharacter);
	BOOL IsPet(AgpdCharacter* pcsCharacter);

	EnumAgpdSummonsPropensity GetPropensity(AgpdCharacter* pcsCharacter);
	BOOL SetPropensity(AgpdCharacter* pcsCharacter, EnumAgpdSummonsPropensity eSummonsPropensity);

	INT32 GetSummonsCIDByTID(AgpdCharacter* pcsCharacter, INT32 lTID);

	PVOID MakePacket(INT16* pnPacketLength, INT8 cOperation, INT32* plOwnerCID, INT32* plSummonsCID, INT32* plSummonsTID,
						INT8* pcSummonsType, INT8* pcSummonsPropensity, UINT32* pulStartTime, UINT32* pulEndTime, UINT8 ucForce);
	PVOID MakeNewSummonsPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT32 lSummonsCID, INT32 lSummonsTID, INT8 cSummonsType,
						INT8 cSummonsPropensity, UINT32 ulStartTime, UINT32 ulEndTime, BOOL bForce);
	PVOID MakeRemoveSummonsPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT32 lSummonsCID);
	PVOID MakeSetPropensityPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT32 lSummonsCID, INT8 cSummonsPropensity);
	PVOID MakeSetMaxSummonsPacket(INT16* pnPacketLength, INT32 lOwnerCID, INT8 cValue);
	PVOID MakeSetMaxTamePacket(INT16* pnPacketLength, INT32 lOwnerCID, INT8 cValue);

	BOOL SetCallbackNewSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSetPropensity(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSetMaxSummons(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSetMaxTame(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveSummonsByLevelLimitRegion(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif//_AGPMSUMMONS_H_
