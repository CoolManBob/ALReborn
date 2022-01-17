/******************************************************************************
Module:  AgpmShrine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 23
******************************************************************************/

#if !defined(__AGPMSHRINE_H__)
#define __AGPMSHRINE_H__

#include "ApBase.h"
#include "ApModule.h"
#include "AgpdShrine.h"
#include "AgpaShrine.h"
#include "AgpaShrineTemplate.h"

#include "ApmObject.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "ApmEventManager.h"
#include "AgpmEventNature.h"
#include "AgpmEventSpawn.h"

#include "AuRandomNumber.h"
#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmShrineD" )
#else
#pragma comment ( lib , "AgpmShrine" )
#endif
#endif

typedef enum _eAGPMSHRINE_DATA_TYPE {
	AGPMSHRINE_DATA_TYPE_SHRINE						= 0,
	AGPMSHRINE_DATA_TYPE_TEMPLATE,
	AGPMSHRINE_DATA_TYPE_NUM
} eAGPMSHRINE_DATA_TYPE;

typedef enum _eAgpmShrineCB {
	AGPMSHRINE_CB_ACTIVE_SHRINE						= 0,
	AGPMSHRINE_CB_INACTIVE_SHRINE,

	AGPMSHRINE_CB_GENERATE_SHRINE_EVENT,
	AGPMSHRINE_CB_GENERATE_TOWNHALL_EVENT,

	AGPMSHRINE_CB_CHANGE_SIGIL_OWNER,

	AGPMSHRINE_CB_ADD_SHRINE_EVENT,
} eAgpmShrineCB;

typedef enum _eAgpmShrinePacketOperation {
	AGPMSHRINE_OPERATION_ADD_SHRINE					= 0,
	AGPMSHRINE_OPERATION_REMOVE_SHRINE,
	AGPMSHRINE_OPERATION_UPDATE_SHRINE,
	AGPMSHRINE_OPERATION_GENERATE_SHRINE_EVENT
} eAgpmShrinePacketOperation;


// shrine template 
#define	AGPMSHRINE_INI_NAME_NAME					"Name"
#define	AGPMSHRINE_INI_NAME_SKILL_NAME				"SkillName"
#define	AGPMSHRINE_INI_NAME_IS_RANDOM_POS			"IsRandomPos"
#define	AGPMSHRINE_INI_NAME_SHRINE_POS				"ShrinePos"
#define	AGPMSHRINE_INI_NAME_USE_CONDITION			"UseCondition"
#define	AGPMSHRINE_INI_NAME_USE_INTERVAL_TIME		"UseIntervalTime"
#define	AGPMSHRINE_INI_NAME_ACTIVE_CONDITION		"ActiveCondition"
#define	AGPMSHRINE_INI_NAME_MIN_ACTIVE_DURATION		"MinActiveDuration"
#define	AGPMSHRINE_INI_NAME_MAX_ACTIVE_DURATION		"MaxActiveDurateion"
#define	AGPMSHRINE_INI_NAME_REACTIVE_CONDITION		"ReActiveCondition"
#define	AGPMSHRINE_INI_NAME_REACTIVE_TIME			"ReActiveTime"
#define	AGPMSHRINE_INI_NAME_IS_SIGIL				"IsSigil"
//	INI의	첫번째 factor 는 m_csSigilAttrFactor
//			두번째 factor 는 m_csMinValueFactor
//			세번째 factor 는 m_csMaxValueFactor 이다

// shrine event
#define	AGPMSHRINE_INI_NAME_EVENT_SHRINE_NAME		"ShrineName"

// object.ini
#define AGPMSHRINE_INI_NAME_SHRINE_NAME				"ShrineName"

// spawn template
#define	AGPMSHRINE_INI_NAME_SPAWN_SHRINE_NAME		"ShrineName"
#define	AGPMSHRINE_INI_NAME_SPAWN_GUARDIAN_LEVEL	"GuardianLevel"
#define	AGPMSHRINE_INI_NAME_END						"ShrineEnd"

class AgpmShrine : public ApModule {
private:
	ApmObject*			m_papmObject;
	AgpmFactors*		m_pagpmFactors;
	AgpmCharacter*		m_pagpmCharacter;
	AgpmItem*			m_pagpmItem;
	AgpmEventNature*	m_pagpmEventNature;

	ApmEventManager*	m_papmEventManager;
	AgpmEventSpawn*		m_pagpmEventSpawn;

	AgpaShrine			m_csShrineAdmin;
	AgpaShrineTemplate	m_csTemplateAdmin;

	INT16				m_nIndexADChar;
	INT16				m_nIndexADObject;
	INT16				m_nIndexADSpawn;

	MTRand				m_csRandom;

public:
	AuPacket			m_csPacket;

public:
	AgpmShrine();
	~AgpmShrine();

	AgpdShrine*			AddShrine(INT32 lShrineID, INT32 lTID);
	AgpdShrine*			AddShrine(AgpdShrine *pcsShrine);
	BOOL				RemoveShrine(INT32 lShrineID);
	AgpdShrine*			GetShrine(INT32 lShrineID);
	AgpdShrine*			GetShrineSequence(INT32 *plIndex);

	AgpdShrineTemplate*	AddShrineTemplate(INT32 lTID);
	AgpdShrineTemplate*	AddShrineTemplate(AgpdShrineTemplate *pcsShrine);
	BOOL				RemoveShrineTemplate(INT32 lTID);
	AgpdShrineTemplate*	GetShrineTemplate(INT32 lTID);
	AgpdShrineTemplate*	GetShrineTemplate(CHAR *szName);
	AgpdShrineTemplate*	GetShrineSequenceTemplate(INT32 *plIndex);

	BOOL				OnAddModule();

	BOOL				OnInit();
	BOOL				OnDestroy();
	BOOL				OnIdle(UINT32 ulClockCount);

	BOOL				SetMaxShrine(INT32 lCount);
	BOOL				SetMaxShrineTemplate(INT32 lCount);

	AgpdShrineADChar*	GetADCharacter(AgpdCharacter *pcsCharacter);
	AgpdShrineADObject*	GetADObject(ApdObject *pcsObject);
	AgpdShrineADSpawn*	GetADSpawn(PVOID pvData);

	static BOOL			ConAgpdShrineADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgpdShrineADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgpdShrineADObject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgpdShrineADObject(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgpdShrineADSpawn(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgpdShrineADSpawn(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdShrine*			CreateShrine();
	BOOL				DestroyShrine(AgpdShrine *pcsShrine);

	AgpdShrineTemplate*	CreateTemplate();
	BOOL				DestroyTemplate(AgpdShrineTemplate *pcsTemplate);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	PVOID				MakePacketAddShrine(AgpdShrine *pcsShrine, INT16 *pnPacketLength);
	PVOID				MakePacketRemoveShrine(AgpdShrine *pcsShrine, INT16 *pnPacketLength);
	PVOID				MakePacketUpdateActive(AgpdShrine *pcsShrine, INT16 *pnPacketLength);

	PVOID				MakePacketGenerateShrineEvent(ApdEvent *pcsEvent, ApBase *pcsGenerator, INT16 *pnPacketLength);

	BOOL				SetCallbackActiveShrine(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackInactiveShrine(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackGenerateShrineEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGenerateTownHallEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackChangeSigilOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackAddShrineEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				CheckDay();

	BOOL				IsGuardian(AgpdCharacter *pcsCharacter);

	BOOL				ActiveShrine(AgpdShrine *pcsShrine, UINT32 ulClockCount);
	BOOL				InactiveShrine(AgpdShrine *pcsShrine, UINT32 ulClockCount);

	// Stream 관련 함수들
	BOOL				StreamWriteTemplate(CHAR *szFile);
	BOOL				StreamReadTemplate(CHAR *szFile);

	static BOOL			TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL			CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ObjectWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			ObjectReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	BOOL				GetSigilFactor(AgpdShrine *pcsShrine, AgpdFactor *pcsFactor);

	BOOL				ChangeSigilOwner(AgpdShrine *pcsShrine, ApBase *pcsBase);

	static BOOL			CBChangeItemOwner(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				AddKillGuardian(AgpdCharacter *pcsKiller, AgpdCharacter *pcsGuardian, UINT32 ulKillTime);
	BOOL				ResetKillGuardian(AgpdCharacter *pcsCharacter, AgpdShrine *pcsShrine, INT32 lGuardianLevel);
	BOOL				IsKillGuardianConditionTrue(AgpdCharacter *pcsKiller, AgpdShrine *pcsShrine, INT32 lGuardianLevel, INT32 lKillCount, UINT32 ulKillTimeLimit);

	static BOOL			CBSpawnRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
};

#endif //__AGPMSHRINE_H__
