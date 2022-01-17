/*====================================================================

	AgpmProduct.h: interface for the AgpmProduct class.

====================================================================*/

#ifndef _AGPM_PRODUCT_H_
	#define _AGPM_PRODUCT_H_

#include "ApModule.h"
#include "AuPacket.h"
#include "AgpdProduct.h"
#include "AgpaProduct.h"

#include "AgpmFactors.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"
#include "AgpmCharacter.h"
#include "AgpmSkill.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGPMPRODUCT_DATATYPE
	{
	AGPMPRODUCT_DATATYPE_SKILL_FACTOR		= 0,
	AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE,
	};

const enum eAGPMPRODUCT_PACKET_OPERATION
	{
	AGPMPRODUCT_PACKET_OPERATION_COMPOSE = 0,
	AGPMPRODUCT_PACKET_OPERATION_COMPOSE_INFO,
	AGPMPRODUCT_PACKET_OPERATION_GATHER,
	AGPMPRODUCT_PACKET_OPERATION_RESULT,
	};

const enum eAGPMPRODUCT_CALLBACK
	{
	AGPMPRODUCT_CB_COMPOSE = 0,
	AGPMPRODUCT_CB_GATHER,
	AGPMPRODUCT_CB_RESULT,
	AGPMPRODUCT_CB_COMPOSE_INFO,
	};

const enum eAGPMPRODUCT_RESULT_CODE
	{
	AGPMPRODUCT_RESULT_SUCCESS = 0,
	AGPMPRODUCT_RESULT_FAIL,
	AGPMPRODUCT_RESULT_FAIL_NO_TOOL,
	AGPMPRODUCT_RESULT_FAIL_INVALID_TARGET,
	AGPMPRODUCT_RESULT_FAIL_TARGET_TOO_FAR,
	AGPMPRODUCT_RESULT_FAIL_INVENTORY_FULL,
	AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_SOURCE_ITEM,
	AGPMPRODUCT_RESULT_FAIL_NO_REMAINED_ITEM,
	AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL,
	AGPMPRODUCT_RESULT_FAIL_HAS_NO_SKILL,
	};

#define AGPMPRODUCT_CATEGORY_ARRAY_MAX		10
#define	AGPMPRODUCT_GATHER_DURATION_MSEC	5400

// Forward declaration
extern CHAR* g_szProductCategory[AGPMPRODUCT_CATEGORY_MAX];

/************************************************/
/*		The Definition of AgpmProduct class		*/
/************************************************/
//
class AgpmProduct : public ApModule
	{
	public:
		AgpmItem		*m_pAgpmItem;
		AgpmGrid		*m_pAgpmGrid;
		AgpmFactors		*m_pAgpmFactors;
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmSkill		*m_pAgpmSkill;
		ApmEventManager	*m_pApmEventManager;

	private:
		// ... Admin
		AgpaComposeTemplate		m_csAdminComposeTemplate;
		AgpaProductSkillFactor	m_csAdminSkillFactor;
		ApSafeArray<AgpdProductCategory, AGPMPRODUCT_CATEGORY_ARRAY_MAX>	m_Category;

		// ... AD
		INT16			m_nIndexGatherCharacterTAD;
		INT16			m_nIndexGatherCharacterAD;
		INT16			m_nIndexComposeCharacterAD;

		// ... random no. gen.
		MTRand			m_csRandom;
	public:
		AuPacket		m_csPacket;

	public:
		AgpmProduct();
		virtual ~AgpmProduct();

		// ... ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnIdle(UINT32 ulClockCount);
		BOOL	OnDestroy();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Admin
		BOOL					SetMaxSkillFactor(INT32 lCount);
		BOOL					SetMaxComposeTemplate(INT32 lCount);

		AgpdProductSkillFactor*	GetProductSkillFactor(CHAR *pszSkill);
		AgpdComposeTemplate*	GetComposeTemplate(INT32 lComposeID);
		INT32					GetComposeList(INT32 eCategory, INT32 lComposeList[], INT32 lBufferSize);
		INT32					GetComposeList(AgpdCharacter *pAgpdCharacter, INT32 eCategory, INT32 lComposeList[], INT32 lBufferSize);
		INT32					GetSkillList(INT32 eCategory, INT32 lSkillList[], INT32 lBufferSize);
		BOOL					IsComposeSkill(INT32 lSkillTID, INT32 *plCategory);

		//	AD
		static BOOL	ConAgpdGatherCharacterTAD(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	DesAgpdGatherCharacterTAD(PVOID pData, PVOID pClass, PVOID pCustData);
		AgpdGatherCharacterTAD*	GetGatherCharacterTAD(ApBase *pcsBase);

		static BOOL	ConAgpdGatherCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	DesAgpdGatherCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData);
		AgpdGatherCharacterAD*	GetGatherCharacterAD(ApBase *pcsBase);

		static BOOL ConAgpdComposeCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL DesAgpdComposeCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData);
		AgpdComposeCharacterAD*	GetComposeCharacterAD(ApBase *pcsBase);

		static BOOL CBResetMonster(PVOID pData, PVOID pClass, PVOID pCustData);

		BOOL	InitGatherData(ApBase *pcsBase);
		BOOL	InitComposeData(ApBase *pcsBase);

		//	Stream
		BOOL	StreamReadCategory(CHAR* pszFile, BOOL bDecryption);
		BOOL	StreamReadCompose(CHAR *pszFile, BOOL bDecryption);
		BOOL	StreamReadFactor(CHAR *pszFile, BOOL bDecryption);
		BOOL	StreamReadGatherCharacterTAD(CHAR *pszFile, BOOL bDecryption);

		//	Compose validation
		BOOL	IsValidStatus(AgpdCharacter *pAgpdChracter, AgpdComposeTemplate *pAgpdComposeTemplate, AgpdSkill *pAgpdSkill, INT32 *plResult = NULL);
		BOOL	IsSatisfyPrerequisites(AgpdCharacter *pAgpdChracter, AgpdComposeTemplate *pAgpdComposeTemplate);
		BOOL	IsOwnCompose(AgpdCharacter *pAgpdChracter, AgpdComposeTemplate *pAgpdComposeTemplate);
		BOOL	IsValidItemStatus(AgpdCharacter *pAgpdChracter, AgpdItemTemplate *pItemTemplate, INT32 lCount = 0);
		BOOL	IsValidComposeLevel(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate, AgpdSkill *pAgpdSkill);

		//	Gather validation
		BOOL	IsValidStatus(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, ApBaseType eBaseType, INT32 lTargetID,
							  INT32 *plResult = NULL, INT32 *plSuccessProb = NULL, INT32 *plItemSetID = NULL);
		BOOL	CheckTargetStatus(ApBase *pTarget);
		BOOL	CheckTargetGatherApplicable(ApBase *pTarget, INT32 lSkillTID, INT32 lClass, INT32 *plSuccessProb, INT32 *plItemSetID);
		BOOL	CheckTargetHasProduct(ApBase *pTarget, INT32 lSkillTID);

		//	value related
		INT32	AddSkillExp(AgpdSkill *pAgpdSkill);
		INT32	GetLevelOfExp(AgpdSkill *pAgpdSkill, INT32 lExp);	// level of given exp.
		INT32	GetLevelOfExp(AgpdProductSkillFactor *pAgpdProductSkillFactor, INT32 lExp, INT32 lCurrLevel = 1);
		INT32	GetExpOfCurrLevel(AgpdSkill *pAgpdSkill);			// obtained exp. after current level
		INT32	GetMaxExpOfCurrLevel(AgpdSkill *pAgpdSkill);		// max exp. of current level (total exp. to obtain to next level from current level)
		BOOL	SetSkillLevel(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 lLevel);

		BOOL	LearnCompose(AgpdCharacter *pAgpdChracter, AgpdComposeTemplate *pAgpdComposeTemplate);
		BOOL	LearnCompose(AgpdCharacter *pAgpdChracter, INT32 lComposeID);

		//	Operation
		BOOL	OnOperationCompose(AgpdCharacter *pAgpdCharacter, INT32 lSkillID, INT32 lComposeID, INT32 lReceipeID);
		BOOL	OnOperationGather(AgpdCharacter* pAgpdCharacter, INT32 lSkillID, INT32 lTargetID);
		BOOL	OnOperationComposeInfo(AgpdCharacter *pAgpdCharacter, PVOID pvComposeInfo, INT16 nComposeInfo);

		//	Callback setting
		BOOL	SetCallbackCompose(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGather(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackComposeInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		BOOL	IsActiveRequireTitle( int nComposeID, int nCurrTitleID );
	};

#endif
