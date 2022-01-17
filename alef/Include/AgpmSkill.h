#ifndef	__AGPMSKILL_H__
#define	__AGPMSKILL_H__

#include "ApBase.h"

#include "ApModule.h"
#include "AgpdSkill.h"
#include "AgpaSkill.h"
#include "AgpaSkillTemplate.h"
#include "AgpaSkillTooltipTemplate.h"
#include "AgpaSkillSpecializeTemplate.h"
#include "AgpdSkillImportData.h"
#include "AgpmConfig.h"

#include "ApmEventManager.h"
#include "ApmObject.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmParty.h"
#include "AgpmBillInfo.h"

#include "AuPacket.h"
#include "AuExcelTxtLib.h"
#include "AuRandomNumber.h"

#include "AgpdEventSkillMaster.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmSkillD" )
#else
#pragma comment ( lib , "AgpmSkill" )
#endif
#endif

//#define _HIGHLEVEL_SKILL_TEST_	1


const int	AGPMSKILL_PROCESS_REMOVE_INTERVAL	= 5000;
const int	AGPMSKILL_RESERVE_SKILL_DATA		= 8000;

const int   AGPMSKILL_NONRACE_NONCLASS_SKILL	= -1;

typedef enum _eAgpmSkillDataType {
	AGPMSKILL_DATA_TYPE_SKILL			= 0,
	AGPMSKILL_DATA_TYPE_TEMPLATE,
	AGPMSKILL_DATA_TYPE_SPECIALIZE,
	AGPMSKILL_DATA_TYPE_TOOLTIP,
	AGPMSKILL_DATA_TYPE_PRODUCT_TEMPLATE,
} eAgpmSkillDataType;

typedef enum _eAgpmSkillCB {
	AGPMSKILL_CB_INIT_SKILL				= 0,
	AGPMSKILL_CB_UPDATE_SKILL,
	AGPMSKILL_CB_INIT_SKILL_TEMPLATE,
	AGPMSKILL_CB_SKILL_CAST_DELAY0,
	AGPMSKILL_CB_SKILL_ACTIVE,
	AGPMSKILL_CB_SKILL_END,
	AGPMSKILL_CB_RECEIVE_ACTION,
	AGPMSKILL_CB_RECEIVE_ACTION_RESULT,
	AGPMSKILL_CB_RECEIVE_REAL_HIT,
	AGPMSKILL_CB_RECEIVE_ADDITIONAL_EFFECT,
	AGPMSKILL_CB_CHECK_NOW_UPDATE_ACTION_FACTOR,
	AGPMSKILL_CB_CHANGE_STATUS,
	AGPMSKILL_CB_ADD_BUFF,
	AGPMSKILL_CB_REMOVE_BUFF,

	AGPMSKILL_CB_REMOVE_SKILL,
	AGPMSKILL_CB_REMOVE_ID,
	AGPMSKILL_CB_DELETE_SKILL,

	AGPMSKILL_CB_UPDATE_SKILL_POINT,
	AGPMSKILL_CB_UPDATE_DIRT_POINT,

	AGPMSKILL_CB_LEARN_SKILL,
	AGPMSKILL_CB_FORGET_SKILL,

	AGPMSKILL_CB_SET_CONST_FACTOR,

	AGPMSKILL_CB_GET_INPUT_TOTAL_COST_SP,
	AGPMSKILL_CB_GET_INPUT_TOTAL_COST_HEROIC_POINT,

	//AGPMSKILL_CB_MASTERY_RESULT,
//	AGPMSKILL_CB_MASTERY_ADD,
//	AGPMSKILL_CB_MASTERY_CHANGE,
//	AGPMSKILL_CB_MASTERY_CHANGE_RESULT,
//	AGPMSKILL_CB_MASTERY_UPDATE,
//	AGPMSKILL_CB_MASTERY_NODE_UPDATE,
//	AGPMSKILL_CB_MASTERY_SPECIALIZE,
//	AGPMSKILL_CB_MASTERY_UNSPECIALIZE,
//	AGPMSKILL_CB_MASTERY_SPECIALIZE_RESULT,
//	AGPMSKILL_CB_MASTERY_UPDATE_TOTAL_SP,
//
//	AGPMSKILL_CB_MASTERY_ROLLBACK,
//	AGPMSKILL_CB_MASTERY_ROLLBACK_RESULT,
//
//	AGPMSKILL_CB_MASTERY_ADD_SP_RESULT,

	AGPMSKILL_CB_INIT_SPECIALIZE,
	AGPMSKILL_CB_UPDATE_SKILL_EXP,
	AGPMSKILL_CB_CHECK_SKILL_PVP,
	AGPMSKILL_CB_ROLLBACK_SKILL,
	AGPMSKILL_CB_MODIFIED_SKILL_LEVEL,
	AGPMSKILL_CB_INIT_COOLTIME,
	AGPMSKILL_CB_SPECIFIC_SKILL_LEVEL,
	AGPMSKILL_CB_GET_MODIFIED_SKILL_LEVEL,
} eAgpmSkillCB;

typedef enum _eAgpmSkillPacketOperation {
	AGPMSKILL_PACKET_OPERATION_ADD		= 0,
	AGPMSKILL_PACKET_OPERATION_REMOVE,
	AGPMSKILL_PACKET_OPERATION_UPDATE,
	AGPMSKILL_PACKET_OPERATION_UPDATE_ATTACH_DATA,
	AGPMSKILL_PACKET_OPERATION_ADD_BUFFED_LIST,
	AGPMSKILL_PACKET_OPERATION_REMOVE_BUFFED_LIST,
	AGPMSKILL_PACKET_OPERATION_ALL_BUFFED_SKILL,
	AGPMSKILL_PACKET_OPERATION_CAST_SKILL,
	AGPMSKILL_PACKET_OPERATION_CAST_SKILL_RESULT,
	AGPMSKILL_PACKET_OPERATION_REAL_HIT,
	AGPMSKILL_PACKET_OPERATION_ADDITIONAL_EFFECT,
	AGPMSKILL_PACKET_OPERATION_UPDATE_DIRT_POINT,
	AGPMSKILL_PACKET_OPERATION_UPDATE_SKILL_POINT,
	AGPMSKILL_PACKET_OPERATION_REQUEST_SKILL_ROLLBACK,
	AGPMSKILL_PACKET_OPERATION_MODIFIED_SKILL_LEVEL,
	AGPMSKILL_PACKET_OPERATION_INIT_COOLTIME,
	AGPMSKILL_PACKET_OPERATION_SPECIFIC_SKILL_LEVEL,

	AGPMSKILL_PACKET_OPERATION_SEPARATOR,


//	AGPMSKILL_PACKET_OPERATION_MASTERY_ADD,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_ADD_SP,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_ADD_SP_RESULT,
//
//	AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_SUCCESS,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_FAIL,
//
//	AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE_SUCCESS,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_CHANGE_FAIL,
//
//	AGPMSKILL_PACKET_OPERATION_MASTERY_UPDATE,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_NODE_UPDATE,
//
//	AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_SUCCESS,
//	AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_FAIL,
} eAgpmSkillPacketOperation;

//typedef enum _eAgpmSkillAddSPToMasteryResult {
//	AGPMSKILL_MASTERY_RESULT_NEED_SELECT		= 0,
//	AGPMSKILL_MASTERY_RESULT_SP_FULL,
//	AGPMSKILL_MASTERY_RESULT_NOT_ACTIVE,
//	AGPMSKILL_MASTERY_RESULT_OK,
//	AGPMSKILL_MASTERY_RESULT_FAIL,
//	AGPMSKILL_MASTERY_SP_CHANGE_OK,
//	AGPMSKILL_MASTERY_SP_CHANGE_FAIL,
//} AgpmSkillAddSPToMasteryResult;

typedef enum _eAgpmSkillCheckTargetResult {
	AGPMSKILL_CHECK_TARGET_OK					= 0,
	AGPMSKILL_CHECK_TARGET_INVALID,
	AGPMSKILL_CHECK_TARGET_NOT_IN_RANGE,
} AgpmSkillCheckTargetResult;

typedef enum _eAgpmCharacterRoleType
{
	AGPMSKILL_CHARACTER_ROLETYPE_NONE = 0,
	AGPMSKILL_CHARACTER_ROLETYPE_MELEE,
	AGPMSKILL_CHARACTER_ROLETYPE_RANGED,
	AGPMSKILL_CHARACTER_ROLETYPE_MAGIC,
	AGPMSKILL_CHARACTER_ROLETYPE_MAX
}eAgpmCharacterRoleType;


// define ini keyname
////////////////////////////////////////////////////////////////////////////////
#define	AGPMSKILL_INI_NAME_NAME						"Name"
#define	AGPMSKILL_INI_NAME_DESCRIPTION				"Description"

#define	AGPMSKILL_INI_NAME_DEFAULT_SKILL_TNAME		"DefaultSkillTName"

#define AGPMSKILL_INI_NAME_USABLE_SKILL_TNAME		"UsableSkillTName"

#define	AGPMSKILL_INI_NAME_SPECIALIZE_NAME			"Name"
#define	AGPMSKILL_INI_NAME_SPECIALIZE_DESCRIPTION	"Description"
#define	AGPMSKILL_INI_NAME_SPECIALIZE_RESTRICTION	"Restriction"
#define AGPMSKILL_INI_NAME_SPECIALIZE_DURATION		"Duration"

class AgpmEventSkillMaster;

class AgpmSkill : public ApModule
{
private:
	ApmObject*				m_papmObject;
	AgpmCharacter*			m_pagpmCharacter;
	AgpmGrid*				m_pagpmGrid;
	AgpmItem*				m_pagpmItem;
	AgpmFactors*			m_pagpmFactors;
	AgpmParty*				m_pagpmParty;
	AgpmBillInfo*			m_pagpmBillInfo;
	AgpmConfig*				m_pagpmConfig;
	
	ApmEventManager*		m_papmEventManager;
	AgpmEventSkillMaster*	m_pagpmEventSkillMaster;

//	AgsmCharacter*			m_pagsmCharacter;

	MTRand					m_csRandom;

	INT16					m_nIndexADCharacter;
	INT16					m_nIndexADCharacterTemplate;

	INT32					m_lMaxAddedTID;

	std::vector<INT32>		m_vcEventSkillTID;

public:
	AuPacket				m_csPacket;
	AuPacket				m_csPacketBase;
	AuPacket				m_csPacketAction;
	AuPacket				m_csPacketAttachData;
	AuPacket				m_csPacketBuff;

//	AuPacket				m_csPacketMastery;
//	AuPacket				m_csPacketMasteryNode;
//	AuPacket				m_csPacketMasteryTree;
//	AuPacket				m_csPacketMasteryActiveNode;

	AgpaSkill				m_csAdminSkill;
	AgpaSkillTemplate		m_csAdminTemplate;
	AgpaSkillTooltipTemplate	m_csAdminTooltipTemplate;
	AgpaSkillSpecializeTemplate	m_csAdminSpecializeTemplate;

	ApAdmin					m_csAdminSkillRemove;
	UINT32					m_ulPrevRemoveClockCount;

	std::vector<INT32>		m_vcCondition2SkillTID;

//	AgpdSkillMasteryTemplate	m_csSkillMasteryTemplate[AURACE_TYPE_MAX * AUCHARCLASS_TYPE_MAX];

private:
	INT16					GetConstFactorIndex(const CHAR *szName, INT32 lType = 0);
	BOOL					SetSkillType(AgpdSkillTemplate *pcsSkillTemplate, CHAR *szTypeName, CHAR *szValue, INT16 nIndex, CHAR *Arg);
	BOOL					SetTooptipType(AgpdSkillTooltipTemplate *pcsTooltip, CHAR *szTypeName, CHAR *szValue, BOOL bIsFactorPoint);

public:
	AgpmSkill();
	~AgpmSkill();

	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

	BOOL					OnIdle2(UINT32 ulClockCount);

	BOOL					SetMaxSkill(INT32 nCount);
	BOOL					SetMaxSkillTemplate(INT32 nCount);
	BOOL					SetMaxSkillTooltipTemplate(INT32 nCount);
	BOOL					SetMaxSkillSpecializeTemplate(INT32 nCount);
	BOOL					SetMaxSkillRemove(INT32 lCount);

	AgpdSkill*				CreateSkill();
	BOOL					DestroySkill(AgpdSkill *pcsSkill);

	AgpdSkillTemplate*		CreateSkillTemplate();
	BOOL					DestroySkillTemplate(AgpdSkillTemplate *pcsSkillTemplate);

	AgpdSkillSpecializeTemplate*	CreateSpecialize();
	BOOL					DestroySpecialize(AgpdSkillSpecializeTemplate *pcsSpecialize);

	AgpdSkillTooltipTemplate*	CreateTooltipTemplate();
	BOOL					DestroyTooltipTemplate(AgpdSkillTooltipTemplate *pcsTooltip);

	static BOOL				ConAgpdSkillAttachData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdSkillAttachData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ConAgpdSkillTemplateAttachData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdSkillTemplateAttachData(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

//	BOOL					OnReceiveSkill(BOOL bReceivedFromServer, INT8 cOperation, INT32 lSkillID, ApBase *pcsBase, PVOID pvPacketFactor, INT32 lTID, INT8 cStatus, PVOID pvPacketAction,
//										   INT32 lSkillPoint, INT32 lMasteryIndex, UINT32 ulDurationTimeMSec,
//										   INT32 *plAllBuffedSkillTID, INT32 *plAllBuffedSkillOwnerID, INT32 lCasterTID);

	BOOL					OnReceiveSkill(BOOL bReceivedFromServer, INT8 cOperation, INT32 lSkillID, ApBase *pcsBase, PVOID pvPacketFactor, INT32 lTID, INT8 cStatus, PVOID pvPacketAction,
										   INT32 lSkillPoint, PVOID *ppvPacketBuff);

	BOOL					OnReceiveAddBuffedList(ApBase *pcsBase, INT32 lBuffedSkillTID, UINT32 ulDurationMSec, INT32 lCasterID, INT32 ucChargeLevel , UINT32 ulExpireTime );

//	BOOL					OnReceiveSkillMastery(BOOL bReceivedFromServer, INT8 cOperation, ApBase *pcsBase, INT32 lMasteryIndex, PVOID pvPacketMastery, INT8 cAddSPResult, PVOID pvSelectSkillTID, INT16 nSelectSkillTIDSize);
//	BOOL					OnReceiveMasteryAdd(ApBase *pcsBase, PVOID *ppvPacketMasteryTree);
//	BOOL					OnReceiveMasteryAddSP(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectSkillTID, INT16 nSelectSkillTIDSize);
//	BOOL					OnReceiveMasteryAddSPResult(ApBase *pcsBase, INT32 lMasteryIndex, INT8 cAddSPResult, INT32 *plSelectSkillTID, INT16 nSelectSkillTIDSize);
//	BOOL					OnReceiveMasteryUpdate(ApBase *pcsBase, INT32 lMasteryIndex, UINT8 ucTotalInputSP, UINT8 *pucTotalInputSP);
//	BOOL					OnReceiveMasteryNodeUpdate(ApBase *pcsBase, PVOID *ppvPacketMasteryNode);
//	BOOL					OnReceiveMasterySpecialize(ApBase *pcsBase, PVOID *ppvPacketMastery);

	AgpdSkillAttachData*	GetAttachSkillData(ApBase *pcsBase);
	AgpdSkillTemplateAttachData*	GetAttachSkillTemplateData(ApBase *pcsBase);

	AgpdSkill*				AddSkill(INT32 lSkillID, INT32 lSkillTID);
	AgpdSkill*				AddSkill(AgpdSkill *pcsSkill);

	BOOL					RemoveSkill(INT32 lSkillID, BOOL bIsDelete = FALSE);

	//BOOL					DeleteSkill(AgpdSkill *pcsSkill);

	AgpdSkill*				GetSkill(INT32 lSkillID);
	AgpdSkill*				GetSkill(ApBase *pcsBase, CHAR *szSkillName);
	AgpdSkill*				GetSkillLock(INT32 lSkillID);

	AgpdSkill*				GetSkillByTID(AgpdCharacter *pcsCharacter, INT32 lTID);

	BOOL					InitSkill(AgpdSkill *pcsSkill);
	BOOL					InitSkillCoolTime(AgpdSkill* pcsSkill);

	AgpdSkillTemplate*		AddSkillTemplate(INT32 lSkillTID);
	BOOL					RemoveSkillTemplate(INT32 lSkillTID);
	BOOL					RemoveAllSkillTemplate();
	AgpdSkillTemplate*		GetSkillTemplate(INT32 lSkillTID);
	AgpdSkillTemplate*		GetSkillTemplate(const CHAR *szName);
	AgpdSkillTemplate*		GetSkillTemplateSequence(INT32 *pnIndex);

	BOOL					ChangeSkillTemplateName(CHAR *szOriginalName, CHAR * szNewName);

	AgpdSkillSpecializeTemplate*	AddSpecialize(INT32 lSpecializeID);
	AgpdSkillSpecializeTemplate*	GetSpecialize(INT32 lSpecializeID);
	BOOL							RemoveSpecialize(INT32 lSpecializeID);

	AgpdSkillTooltipTemplate*	AddTooltipTemplate(CHAR *szSkillName);
	AgpdSkillTooltipTemplate*	GetTooltipTemplate(CHAR *szSkillName);
	BOOL						RemoveTooltipTemplate(CHAR *szSkillName);

	INT16					AttachSkillData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16					AttachSkillTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16					AttachSpecializeData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	BOOL					SetSkillStatus(AgpdSkill *pcsSkill, eAgpmSkillStatus eStatus);

	static BOOL				CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBGetPetTIDByItemFromSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	PVOID					MakeBasePacket(ApBase *pcsBase);
	PVOID					MakeActionPacket(INT8 cActionType, INT32 lSkillLevel, ApBase *pcsTargetBase, PVOID pvResultFactorPacket, AuPOS *pcsTargetPos, BOOL bForceAttack = FALSE, UINT32 ulCastDelay = 0, UINT32 ulDuration = 0, UINT32 ulRecastDelay = 0, CHAR *szTargetCharID = NULL, INT16 nLenTargetCharID = 0, BOOL bIsFactorNotQueueing = TRUE, UINT32 ulAdditionalEffect = 0);
	PVOID					MakeCastPacket(INT16 nActionType, INT32 *plSkillID, INT32 *plSkillTID, ApBase *pcsOwnerBase, INT32 lSkillLevel, ApBase *pcsTargetBase, AuPOS *pcsTargetPos, BOOL bForceAttack, UINT32 ulCastDelay, UINT32 ulDuration, UINT32 ulRecastDelay, CHAR *szTargetCharID, INT16 nLenTargetCharID, INT16 *pnPacketLength);
	PVOID					MakeCastResultPacket(INT16 nActionType, INT32 *plSkillID, INT32 *plSkillTID, ApBase *pcsOwnerBase, INT32 lSkillLevel, ApBase *pcsTargetBase, PVOID pvPacketResultFactor, INT16 *pnPacketLength, BOOL bIsFactorNotQueueing = FALSE, UINT32 ulAdditionalEffect = 0);
	PVOID					MakePacketRealHit(INT32* plSkillID, ApBase* pcsOwnerBase, INT16* pnPacketLength);
	PVOID					MakePacketAdditionalEffect(INT32* plEffectID, ApBase* pcsBase, INT32* plTargetCID, INT16* pnPacketLength);
	PVOID					MakePacketBuffedList(ApBase *pcsBase, INT32 lBuffedTID, INT32 lCasterTID, BOOL bAdd, UINT32 ulDurationTimeMSec, UINT8 ucChargeLevel, INT16 *pnPacketLength, UINT32 ulExpiredTime = 0);
	PVOID					MakePacketAllBuffedList(ApBase *pcsBase, INT16 *pnPacketLength);
	PVOID					MakePacketSkill(AgpdSkill *pcsSkill, INT16 *pnPacketLength);
	PVOID					MakePacketSkillRemove(AgpdSkill *pcsSkill, INT16 *pnPacketLength);

//	PVOID					MakePacketAddSPToMastery(ApBase *pcsBase, INT8 cMasteryIndex, INT32 *plSelectSkillTID, INT16 nArraySize, INT16 *pnPacketLength);
//	PVOID					MakePacketAddSPToMasteryResult(ApBase *pcsBase, INT8 cMasteryIndex, INT8 cResult, INT32 *plSelectSkillTID, INT16 nArraySize, INT16 *pnPacketLength);
//
//	PVOID					MakePacketMasteryUpdate(ApBase *pcsBase, INT8 cMasteryIndex, INT16 *pnPacketLength);
//	PVOID					MakePacketMasteryAllUpdate(ApBase *pcsBase, INT16 *pnPacketLength);
//	PVOID					MakePacketMasteryNodeUpdate(ApBase *pcsBase, INT8 cMasteryIndex, INT32 *plNodeList, INT32 lNumNode, INT16 *pnPacketLength);
//	PVOID					MakePacketMasteryChangedNode(ApBase *pcsBase, INT16 *pnPacketLength);

	PVOID					MakePacketSkillDIRT(AgpdSkill *pcsSkill, INT16 *pnPacketLength);
	PVOID					MakePacketUpdateSkillPoint(AgpdSkill *pcsSkill, INT16 *pnPacketLength, INT32 lPoint);
	PVOID					MakePacketUpdateDIRTPoint(AgpdSkill *pcsSkill, INT16 *pnPacketLength, INT32 lDuration, INT32 lIntensity, INT32 lRange, INT32 lTarget);

	PVOID					MakePacketOperation(ApBase *pcsBase, INT8 cOperation, INT16 *pnPacketLength);

	PVOID					MakePacketRequestRollback(ApBase *pcsBase, INT32 lSkillID, INT16 *pnPacketLength);

	PVOID					MakePacketModifiedSkillLevel(ApBase* pcsBase, INT16* pnPacketLength);
	PVOID					MakePacketInitCoolTime(ApBase* pcsBase, INT16* pnPacketLength, INT32 lSkillTID);
	PVOID					MakePacketSpecificSkillLevel(AgpdSkill* pcsSkill, INT16* pnPacketLength);

//	PVOID					MakePacketMasteryRollback(ApBase *pcsBase, INT16 *pnPacketLength);
//	PVOID					MakePacketMasteryRollbackResult(ApBase *pcsBase, INT16 *pnPacketLength, eAgpmSkillPacketOperation eResult);

	ApBase*					GetBaseFromPacket(PVOID pvBasePacket);

	ApBase*					GetBase(INT8 cType, INT32 lID);
	ApBase*					GetBaseLock(INT8 cType, INT32 lID);

	INT32					AddBuffedList(pstAgpmSkillBuffList pstBuffList, INT32 lSkillTID, INT32 lSkillID = AP_INVALID_SKILLID,
										  INT16 nOwnerType = APBASE_TYPE_NONE, INT32 lOwnerID = 0, INT32 lCasterTID = 0,
										  INT32 lSpellCount = 0, UINT32 ulDurationTimeMSec = 0, INT32 lChargeLevel = 0 , UINT32 ulExpireTime = 0 );
	INT32					RemoveBuffedList(pstAgpmSkillBuffList pstBuffList, INT32 lTID);
	INT32					RemoveBuffedListByIndex(pstAgpmSkillBuffList pstBuffList, INT32 lIndex);
	pstAgpmSkillBuffList	GetBuffedList(ApBase *pcsBase);
	INT32					GetBuffedSkillCount(ApBase* pcsBase);	// 2005.05.09. steeple
	BOOL					IsBuffedSkillByTID(ApBase* pcsBase, INT32 lSkillTID);

	BOOL					AddUseList(AgpdSkill *pcsSkill, INT32 lSkillID);
	BOOL					RemoveUseList(AgpdSkill *pcsSkill, INT32 lSkillID);

	BOOL					AddUsePassiveList(AgpdSkill *pcsSkill, INT32 lSkillID);
	BOOL					RemoveUsePassiveList(AgpdSkill *pcsSkill, INT32 lSkillID);

	BOOL					AddOwnSkillList(AgpdSkill *pcsSkill);
	BOOL					RemoveOwnSkillList(AgpdSkill *pcsSkill);

	BOOL					CheckSkillInUseList(ApBase *pcsBase, CHAR *szSkillName);

	BOOL					CheckHaveAnySkill(AgpdCharacter *pcsCharacter);

	/*
	BOOL					AddAffectedBase(AgpdSkill *pcsSkill, ApBase *pcsBase);
	BOOL					InitialAffectedBase(AgpdSkill *pcsSkill);
	*/

	BOOL					CheckRequireClass(AgpdCharacter* pcsCharacter, AgpdSkillTemplate *pcsSkillTemplate);
	BOOL					IsOwnSkill(ApBase *pcsBase, AgpdSkillTemplate *pcsSkillTemplate);

	BOOL					CheckLearnableSkill(ApBase *pcsBase, CHAR *szSkillName);
	BOOL					CheckLearnableSkill(ApBase *pcsBase, INT32 lSkillTID);

	AgpdSkill*				LearnSkill(ApBase *pcsBase, CHAR *szSkillName, INT32 lSkillLevel);
	AgpdSkill*				LearnSkill(ApBase *pcsBase, INT32 lSkillTID, INT32 lSkillLevel);

	BOOL					ForgetSkill(ApBase *pcsBase, INT32 lSkillID);
	BOOL					ForgetSkill(ApBase *pcsBase, AgpdSkill *pcsSkill);

	BOOL					IsStatusActionOnActionType4(ApBase* pcsBase);

	INT32					GetTimeBuffType(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					SetCallbackInitSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackInitSkillTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackReceiveAction(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// action packet을 받았다.
	BOOL					SetCallbackReceiveActionResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// action packet을 받았다.
	BOOL					SetCallbackReceiveRealHit(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// Real Hit 를 받았다.
	BOOL					SetCallbackReceiveAdditionalEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Additional Effect 를 받았다.
	BOOL					SetCallbackCheckNowUpdateActionFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// action packet을 받았다.
	BOOL					SetCallbackChangeStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackAddBuffedList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveBuffedList(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackRemoveSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackDeleteSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackUpdateSkillPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateDIRTPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackGetInputTotalCostSP(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackGetInputTotalCostHeroicPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackLearnSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackForgetSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackSetConstFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackUpdateSkillExp(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//BOOL					SetCallbackMasteryResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryInit(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryChange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryChangeResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryNodeUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasterySpecialize(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryUnSpecialize(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasterySpecializeResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryUpdateTotalSP(ApModuleDefaultCallBack pfCallback, PVOID pClass);

//	BOOL					SetCallbackMasteryRollback(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL					SetCallbackMasteryRollbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackInitSpecialize(ApModuleDefaultCallBack pfCallback, PVOID pClass);

//	BOOL					SetCallbackAddSPResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackCheckSkillPvP(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackRollbackSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackModifiedSkillLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackGetModifiedSkillLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackInitCoolTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSpecificSkillLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					UpdateModifiedSkillLevel(ApBase* pcsBase) { return EnumCallback(AGPMSKILL_CB_MODIFIED_SKILL_LEVEL, pcsBase, NULL); }
	INT32					GetModifiedSkillLevel(ApBase* pcsBase);


	// Stream Function 들
	BOOL					StreamWriteTemplate(CHAR *szFile, BOOL bEncryption);
	BOOL					StreamReadTemplate(CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE, BOOL bIsNewTID = FALSE);

	BOOL					StreamWriteSpecialize(CHAR *szFile, BOOL bEncryption);
	BOOL					StreamReadSpecialize(CHAR *szFile, BOOL bDecryption);

	static BOOL				TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL				AttachedTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				AttachedTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL				SpecializeWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				SpecializeReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	// import excel file functions
	////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL					ReadSkillConstTxt(CHAR *szFile, BOOL bDecryption);
	BOOL					ReadSkillConst2Txt(CHAR* szFile, BOOL bDecryption);
	BOOL					ReadSkillSpecTxt(CHAR *szFile, BOOL bDecryption);
//	BOOL					ReadSkillMasteryTxt(CHAR *szFile, BOOL bDecryption);
	BOOL					ReadSkillTooltipTxt(CHAR *szFile, BOOL bDecryption);
	BOOL					ReadSkillConstCreatureTID(AgpdSkillTemplate* pcsSkillTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow, INT32 lStartLevel, INT32 lEndLevel);
	BOOL					ReadSkillConstAdditionalSkillTID(AgpdSkillTemplate* pcsSkillTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow, INT32 lStartLevel, INT32 lEndLevel);
	BOOL					ReadSkillConstLevelUpSkillTID(AgpdSkillTemplate* pcsSkillTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow, INT32 lStartLevel, INT32 lEndLevel);
	
	BOOL					SetSpecialStatus(AgpdSkillTemplate *pcsSkillTemplate, INT32 lValue);

	// AgpdSkillTemplateAttachData 처리 함수들 (기능은 함수 이름 그대로이다.)
	BOOL					AddDefaultSkillTID(ApBase *pcsBaseTemplate, INT32 lSkillTID);
	BOOL					RemoveDefaultSkillTID(ApBase *pcsBaseTemplate, INT32 lSkillTID);
	BOOL					CleanDefaultSkillTID(ApBase *pcsBaseTemplate);

	INT16					ConvertFactorMagnifyIndex(INT16 nIndex);

	UINT64					GetTargetType(AgpdSkill *pcsSkill, ApBase *pcsTargetBase);

	AgpmSkillCheckTargetResult	IsValidTarget(AgpdSkill *pcsSkill, ApBase *pcsTargetBase, BOOL IsForceAttack, AuPOS *pstCalcPos);
	//AgpmSkillCheckTargetResult	IsValidTarget(AgpdSkill *pcsSkill, ApBase *pcsTargetBase, AuPOS *pcsTargetPos, INT32 lTargetUnion, BOOL IsForceAttack = FALSE);
	//AgpmSkillCheckTargetResult	IsValidTarget(AgpdSkill *pcsSkill, AuPOS *pcsTargetPos, BOOL IsForceAttack = FALSE);
	BOOL					CheckSkillPvP(AgpdSkill* pcsSkill, ApBase* pcsTargetBase);
	BOOL					CheckTargetRestriction(AgpdSkill* pcsSkill, ApBase* pcsTargetBase);

	BOOL					IsPassiveSkill(AgpdSkill *pcsSkill);
	BOOL					IsPassiveSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsActionPassiveSkill(AgpdSkill* pcsSkill);
	BOOL					IsActionPassiveSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsCriticalUpPassiveSkill(AgpdSkill* pcsSkill);//JK_패시브스킬 크리티컬 UI 표시 오류 


	BOOL					IsProductSkill(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					IsNormalAttack(AgpdSkill* pcsSKill );

	BOOL					IsSufficientCost(AgpdSkill *pcsSkill, eAgpmSkillActionType *peActionType);
	BOOL					IsSatisfyRequirement(AgpdSkill *pcsSkill);

	BOOL					IsAttackSkill(INT32 lSkillTID);
	BOOL					IsAttackSkill(AgpdSkillTemplate *pcsSkillTemplate);
	BOOL					IsAreaSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsDebuffSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSelfBuffSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsETCSkill( AgpdSkillTemplate* pcsSkillTemplate );

	BOOL					IsDurationByDistanceSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsDurationUnlimited(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSummonsSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsTameSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsFixedSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsTransparentSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsNotAddAgroAtCast(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsATFieldAttackSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsMutationSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsReleaseTargetSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsVisibleEffectTypeSelfOnly(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSubCashItemStackCountOnSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsGameBonusExpSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsGameBonusMoneySkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsCashSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsLevelDiffSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsUnlimitedTargetSiegeWarSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsUnlimitedTargetRaceSkill(AgpdSkillTemplate* pcsSkillTemplate);		// 타켓제한없는 종족스킬 by iluvs
	BOOL					IsPartySkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsInvolveParentRegionSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSphereRangeSiegeWarSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSummonsSkillForSiegeWar(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsDetectSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsRideSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsArchlordSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsGroundTargetSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsDOTSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSleepSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsDispelAllBuffSkill(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					IsForceSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsForceSetSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsForceCastSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsForceTransparentSkill(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					IsDeadTarget(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsResurrectionSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSkillUnionType1(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsSkillUnionType2(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					IsMovePosTarget(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsMovePosSelf(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					IsEvolutionSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL					IsTransformSkill(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL					IsCharacterConfigSkill(AgpdSkillTemplate* pcsSkillTemplate);

	INT32					GetActionOnActionType(AgpdSkillTemplate* pcsSkillTemplate);
	INT32					GetSummonsType(AgpdSkillTemplate* pcsSkillTemplate);
	INT32					GetSummonsTIDByItem(AgpdItemTemplate* pcsItemTemplate);
	INT32					GetDetectType(AgpdSkillTemplate* pcsSkillTemplate);
	INT32					GetSkillLevelUpType(AgpdSkillTemplate* pcsSkillTemplate);

	INT32					GetCostHP(AgpdSkill *pcsSkill);
	INT32					GetCostSP(AgpdSkill *pcsSkill);
	INT32					GetCostMP(AgpdSkill *pcsSkill);
	INT32					GetCostArrow(AgpdSkill *pcsSkill);

	INT32					GetSkillLevel(AgpdSkill *pcsSkill);
	INT32					GetSkillPoint(AgpdSkill *pcsSkill);
	INT32					GetSkillExp(AgpdSkill *pcsSkill);

	BOOL					SetSkillPoint(AgpdSkill *pcsSkill, INT32 lSkillPoint);
	BOOL					SetSkillExp(AgpdSkill *pcsSkill, INT32 lSkillExp);

	UINT32					GetCastDelay(AgpdSkill *pcsSkill);
	UINT32					GetRecastDelay(AgpdSkill *pcsSkill);
	UINT32					GetSemiRecastDelay(AgpdSkill *pcsSkill);
	UINT32					GetSkillDurationTime(AgpdSkill *pcsSkill, INT32 lSkillLevel = 0);
	UINT32					GetSkillInterval(AgpdSkill *pcsSkill, INT32 lSkillLevel = 0);

	BOOL					SetSkillTemplateDurationTime(AgpdSkillTemplate *pcsSkillTemplate, UINT32 lSkillDuration, UINT32 lSkillLevel);

	UINT32					GetCastDelay(AgpdSkillTemplate *pcsSkillTemplate, INT32 lSkillLevel);
	UINT32					GetRecastDelay(AgpdSkillTemplate *pcsSkillTemplate, INT32 lSkillLevel);

	INT32					GetAdjustCastDelay(AgpdCharacter* pcsCharacter);
	INT32					GetAdjustRecastDelay(AgpdCharacter* pcsCharacter);

	INT32					GetRange(AgpdSkill *pcsSkill);
	INT32					GetTargetArea1(AgpdSkill *pcsSkill);
	INT32					GetTargetArea2(AgpdSkill *pcsSkill);

	INT32					GetModifiedHitRate(AgpdCharacter *pcsCharacter);
	INT32					GetModifiedBlockRate(AgpdCharacter *pcsCharacter);
	INT32					GetModifiedHPRegen(AgpdCharacter *pcsCharacter);
	INT32					GetModifiedMPRegen(AgpdCharacter *pcsCharacter);
	eAgpmSkillWeaponDamageType	GetWeaponDamageType(AgpdCharacter *pcsCharacter);

	INT32					GetDivideDMGNumForShow(AgpdSkill *pcsSkill);
	INT32					GetDivideDMGNumForShow(AgpdSkillTemplate *pcsSkillTemplate, INT32 lSkillLevel);
	INT32					GetDivideDMGNumForShow(INT32 lTID, INT32 lSkillLevel);
	
	AgpdSkill*				GetSkillByAction(AgpdCharacter *pcsCharacter, AgpdCharacterActionResultType eActionResult);

	INT32					GetRandomCreatureTID(AgpdSkillTemplate* pcsSkillTemplate, INT32 lSkillLevel);

	INT32					GetBonusExpRate(AgpdCharacter* pcsCharacter);
	INT32					GetBonusMoneyRate(AgpdCharacter* pcsCharacter);
	INT32					GetBonusCharismaRate(AgpdCharacter* pcsCharacter);

	// skill point setting functions
	BOOL					AddSkillPoint(AgpdSkill *pcsSkill, INT32 lPoint);
	BOOL					SubtractSkillPoint(AgpdSkill *pcsSkill, INT32 lPoint);
//	INT32					GetRemainSkillPointToArriveMaxLevel(AgpdSkill *pcsSkill, INT32 lPoint);
	INT32					GetMaxInputSkillPoint(AgpdSkill *pcsSkill);

	BOOL					UpdateSkillPoint(AgpdSkill *pcsSkill, INT32 lPoint);
	BOOL					UpdateSkillLevelAndExp(AgpdSkill *pcsSkill, INT32 lLevel, INT32 lExp);

	INT32					GetTotalSkillPoint(ApBase *pcsBase, INT32 lRace = -1, INT32 lClass = -1 , BOOL bIncludingHeroic = TRUE);
	BOOL					IsWantedRaceClassSkill(AgpdSkill *pcsSkill, INT32 lRace, INT32 lClass);

	INT32					GetTotalUsedHeroicPoint(ApBase *pcsBase);

	// DIRT point setting functions
	BOOL					UpdateDIRT(AgpdSkill *pcsSkill, AgpdFactor *pcsFactor);

	// Skill Mastery 처리 함수들
//	AgpdSkillMastery*		GetMastery(ApBase *pcsBase);
//	AgpdSkillMasteryTemplate*	GetMasteryTemplate(ApBase *pcsBase);
//
//	INT32					GetMasteryPoint(AgpdSkill *pcsSkill);

	BOOL					AdjustSkillPoint(AgpdCharacter *pcsCharacter);
	BOOL					AdjustHeroicPoint(AgpdCharacter *pcsCharacter);

//	BOOL					IsMasteryActive(ApBase *pcsBase, INT32 lMasteryIndex);
//	BOOL					IsMasterySPFull(ApBase *pcsBase, INT32 lMasteryIndex);

//	INT32					GetInputTotalSP(ApBase *pcsBase, INT32 lMasteryIndex);

	BOOL					CheckOwnerSP(ApBase *pcsBase);

//	INT64					GetRollbackCost(ApBase *pcsBase);
//	BOOL					CheckMasteryRollbackCost(ApBase *pcsBase);
//	BOOL					PayMasteryRollbackCost(ApBase *pcsBase);

//	BOOL					ResetAllMastery(ApBase *pcsBase);

//	BOOL					AddSP(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectTID, INT32 lArraySize);
//
//	INT32					CheckAddSPToMastery(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectTID, INT32 lArraySize);
//	INT32					AddSPToMastery(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectTID, INT32 lArraySize);
//
//	INT32					GetBaseSPForActive(ApBase *pcsBase, INT32 lSkillTID);
//	INT32					GetBaseSPForActive(ApBase *pcsBase, INT32 lSkillTID, INT32 lMasteryIndex);

	//BOOL					AddSPToCurrentActiveSkill(ApBase *pcsBase, INT32 lMasteryIndex);
	//BOOL					SubSPToCurrentActiveSkill(ApBase *pcsBase, INT32 lMasteryIndex);

//	BOOL					CalcSpecializeLevel(ApBase *pcsBase);
//
//	INT64					GetSpecializeCost(ApBase *pcsBase, INT32 lMasteryIndex);
//	BOOL					CheckSpecializeCost(ApBase *pcsBase, INT32 lMasteryIndex);
//	BOOL					CheckSpecializeLevel(ApBase *pcsBase, INT32 *plLevel);
//	BOOL					PaySpecializeCost(ApBase *pcsBase, INT32 lMasteryIndex);
//
//	BOOL					CheckSpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant);
//	BOOL					MasterySpecializeCheckOnly(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant);
//	BOOL					MasterySpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant);
//	BOOL					ProcessSpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant);
//	BOOL					MasteryUnSpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType);
//
//	INT32					GetSpecializedCost(AgpdSkill *pcsSkill, INT32 lOriginalCost);
//	INT32					GetSpecializedDamage(AgpdSkill *pcsSkill, INT32 lOriginalDamage);
//	INT32					GetSpecializedDuration(AgpdSkill *pcsSkill, INT32 lOriginalDuration);
//	INT32					GetSpecializedArea(AgpdSkill *pcsSkill, INT32 lOriginalArea);
//	INT32					GetSpecializedDistance(AgpdSkill *pcsSkill, INT32 lOriginalDistance);
//
//	BOOL					InitMastery(ApBase *pcsBase);

	BOOL					ProcessRemove(UINT32 ulClockCount);
	BOOL					AddRemoveSkill(AgpdSkill *pcsSkill);

	BOOL					AddEventSkillTID(INT32 lSkillTID);

	BOOL					IsHighLevelSkill(AgpdSkillTemplate* pcsSkillTemplate) { return pcsSkillTemplate->m_bHighLevelSkill; }
	BOOL					IsHeroicSkill(AgpdSkillTemplate* pcsSkillTemplate) { return pcsSkillTemplate->m_bHeroicSkill; }

	eAgpmCharacterRoleType	GetCharacterSkillRoleType(AgpdCharacter *pcsCharacter);
};

#endif //__AGPMSKILL_H__
