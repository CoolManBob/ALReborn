/******************************************************************************
Module:  AgpmEventFactors.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 12
******************************************************************************/

#if !defined(__AGPMEVENTFACTORS_H__)
#define __AGPMEVENTFACTORS_H__

//#include "AgpdEventFactors.h"

#include "ApmEventManager.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventFactorsD" )
#else
#pragma comment ( lib , "AgpmEventFactors" )
#endif
#endif
//@} Jaewon

const int	AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS			= 300;

typedef INT16	(*AgpmEventFactorsCallBack) (PVOID pData, PVOID pClass, PVOID pCustData);

typedef enum _eAgpmEventFactors {

	AGPM_EVENT_FACTORS_TYPE_AREA			= 0,
	AGPM_EVENT_FACTORS_TYPE_FACTOR,
	AGPM_EVENT_FACTORS_TYPE_CHARACTER,
	AGPM_EVENT_FACTORS_TYPE_ITEM

} eAgpmEventFactors;

typedef enum _eAgpmEventFactorsUpdateFactorType {

	AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT	= 0,
	AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT,
	AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT,
	AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT

} AgpmEventFactorsUpdateFactorType;

typedef struct _stAgpmEventFactorsArg {

	AgpmEventFactorsCallBack	pfnCheckCondition;
	ApModuleDefaultCallBack		pfnCalcFactor;
	ApModuleDefaultCallBack		pfnProcessor;
	PVOID						pvClass;
	ApdEvent					*pcsEvent;
	PVOID						pvUpdateFactor;
	PVOID						pvUpdateFactorPercent;
	PVOID						pvCustData;
	BOOL						bMakePacket;

} stAgpmEventFactorsArg, *pstAgpmEventFactorsArg;

typedef struct _stAgpmEventFactorsCBArg {

	ApdEvent					*pcsEvent;
	ApBase						*pcsTargetBase;
	AgpdFactor					*pcsUpdateFactor;
	AgpdFactor					*pcsUpdateFactorPercent;
	PVOID						pvPacketFactor;
	PVOID						pvCustData;

} stAgpmEventFactorsCBArg, *pstAgpmEventFactorsCBArg;

class AgpmEventFactors : public ApModule {
private:
	ApmEventManager		*m_papmEventManager;
	AgpmFactors			*m_pagpmFactors;
	AgpmCharacter		*m_pagpmCharacter;

private:
	BOOL				ProcessUpdateFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateFactor, BOOL bIsUpdateFactor);

	BOOL				ProcessUpdateEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bIsUpdateFactor,
										 BOOL bMakePacket);

public:
	AgpmEventFactors();
	~AgpmEventFactors();

	// Virtual Function ต้
	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				CalcEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bMakePacket = FALSE);

	BOOL				UpdateEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bMakePacket = FALSE);

	static BOOL			CalcFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateFactor);
	static BOOL			UpdateFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateFactor);
};

#endif //__AGPMEVENTFACTORS_H__