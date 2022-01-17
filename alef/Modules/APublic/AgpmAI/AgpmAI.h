// AgpmAI.h: interface for the AgpmAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMAI_H__A0174039_68D7_4935_BFD4_A316CA0FA4F3__INCLUDED_)
#define AFX_AGPMAI_H__A0174039_68D7_4935_BFD4_A316CA0FA4F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmEventSpawn.h"

#include "AgpdAI.h"
#include "AgpaAI.h"

#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmAID" )
#else
#pragma comment ( lib , "AgpmAI" )
#endif
#endif

#define AGPMAI_DEFAULT_TEMPLATE_NUMBER		100

#define AGPMAI_DEFAULT_VISIBILITY			1000
#define AGPMAI_DEFAULT_PROCESS_INTERVAL		500

#define AGPMAI_STREAM_NAME_END								"AIEnd"

#define AGPMAI_STREAM_NAME_NAME								"Name"
#define AGPMAI_STREAM_NAME_VISIBILITY						"Visibility"
#define AGPMAI_STREAM_NAME_INTERVAL							"Interval"
#define AGPMAI_STREAM_NAME_AIFACTOR_ID						"AIFactorID"
#define AGPMAI_STREAM_NAME_AIFACTOR_TYPE					"AIFactorType"
#define AGPMAI_STREAM_NAME_AIFACTOR_CHAR_FACTOR_COMPARE		"AIFactorCFCompare"
#define AGPMAI_STREAM_NAME_AIFACTOR_CHAR_STATUS				"AIFactorCharStatus"
#define AGPMAI_STREAM_NAME_AIFACTOR_CHAR_ITEM				"AIFactorCharItem"
#define AGPMAI_STREAM_NAME_ACTION_TYPE						"AIActionType"
#define AGPMAI_STREAM_NAME_ACTION_RATE						"AIActionRate"
#define AGPMAI_STREAM_NAME_TARGET_BASE_TYPE					"AITargetBaseType"
#define AGPMAI_STREAM_NAME_TARGET_BASE_ID					"AITargetBaseID"
#define AGPMAI_STREAM_NAME_TARGET_POS						"AITargetPos"
#define AGPMAI_STREAM_NAME_TARGET_RADIUS					"AITargetRadius"
#define AGPMAI_STREAM_NAME_TARGET_FLAGS						"AITargetFlags"
#define AGPMAI_STREAM_NAME_TARGET_WEIGHT					"AITargetWeight"
#define AGPMAI_STREAM_NAME_TEMPLATE							"AITemplateID"

#define AGPMAI_STREAM_NAME_USABLE_ITEM_COUNT				"AIUsableItemCount"
#define AGPMAI_STREAM_NAME_USABLE_ITEM_TID					"AIUsableItemITID"
#define AGPMAI_STREAM_NAME_USABLE_ITEM_RATE					"AIUsableItemRate"

#define AGPMAI_STREAM_NAME_USABLE_SKILL_COUNT				"AIUsableSkillCount"
#define AGPMAI_STREAM_NAME_USABLE_SKILL_TID					"AIUsableSkillTID"
#define AGPMAI_STREAM_NAME_USABLE_SKILL_RATE				"AIUsableSkillRate"

#define AGPMAI_STREAM_NAME_SCREAM_USE						"AIScreamUse"
#define AGPMAI_STREAM_NAME_SCREAM_HELP_ALL					"AIScreamHelpAll"
#define AGPMAI_STREAM_NAME_SCREAM_HELP_CHAR_TYPE_COUNT		"AIScreamHelpCharTypeCount"
#define AGPMAI_STREAM_NAME_SCREAM_HELP_CHAR_TID				"AIScreamHelpCharTID"
#define AGPMAI_STREAM_NAME_SCREAM_FEARLV1					"AIScreamFearLV1"
#define AGPMAI_STREAM_NAME_SCREAM_FEARLV2					"AIScreamFearLV2"
#define AGPMAI_STREAM_NAME_SCREAM_FEARLV3					"AIScreamFearLV3"
#define AGPMAI_STREAM_NAME_SCREAM_HELPLV1					"AIScreamHelpLV1"
#define AGPMAI_STREAM_NAME_SCREAM_HELPLV2					"AIScreamHelpLV2"
#define AGPMAI_STREAM_NAME_SCREAM_HELPLV3					"AIScreamHelpLV3"

typedef enum
{
	AGPMAI_DATA_AITEMPLATE	= 0,
} AgpmAIData;

class AgpmAI : public ApModule  
{
private:
	AgpaAI					m_csAITemplates;

	ApmMap *				m_pcsApmMap;
	AgpmFactors *			m_pcsAgpmFactors;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;
	AgpmEventSpawn *		m_pcsAgpmEventSpawn;

	INT16					m_nCharacterAttachIndex;
	INT16					m_nCharacterTemplateAttachIndex;
	INT16					m_nSpawnAttachIndex;

	INT32					m_lTemplates;

	INT32					m_lMaxTID;

public:
	AuPacket				m_csPacket;

public:
	AgpmAI();
	virtual ~AgpmAI();

	// Virtual Function 들
	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnIdle(UINT32 ulClockCount);
	BOOL				OnDestroy();

	VOID				SetMaxTemplate(INT32 lCount);
	AgpdAITemplate *	GetTemplateSequence(INT32 *plIndex);

	// Character / Template에 Attach 된 Data를 가져오기
	AgpdAIADChar *		GetCharacterData(AgpdCharacter *pcsCharacter);
	AgpdAIADChar *		GetCharacterTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate);
	AgpdAIADSpawn *		GetSpawnData(AgpdSpawn *pstSpawn);

	AgpdAITemplate *	AddAITemplate(INT32 lTID);
	AgpdAITemplate *	AddAITemplate(AgpdAITemplate *pstTemplate);

	AgpdAITemplate *	GetAITemplate(INT32 lTID);

	BOOL				RemoveAITemplate(INT32 lTID);

	AgpdAIFactor * 		AddAIFactor(AgpdCharacter *pcsCharacter, AgpdAIFactorType eType);
	AgpdAIFactor *		AddAIFactor(AgpdCharacterTemplate *pcsCharacterTemplate, AgpdAIFactorType eType);
	AgpdAIFactor *		AddAIFactor(AgpdAI *pstAI, AgpdAIFactorType eType);

	AgpdAIFactor *		GetAIFactor(AgpdCharacter *pcsCharacter, INT32 lFactorID);
	AgpdAIFactor *		GetAIFactor(AgpdCharacterTemplate *pcsCharacterTemplate, INT32 lFactorID);
	AgpdAIFactor *		GetAIFactor(AgpdAI *pstAI, INT32 lFactorID);

	AgpdAIFactor *		RemoveAIFactor(AgpdCharacter *pcsCharacter, INT32 lFactorID);
	AgpdAIFactor *		RemoveAIFactor(AgpdCharacterTemplate *pcsCharacterTemplate, INT32 lFactorID);
	AgpdAIFactor *		RemoveAIFactor(AgpdAI *pstAI, INT32 lFactorID);

	BOOL				DestroyAI(AgpdAI *pstAI);

	BOOL				CopyAI(AgpdAI *pstDstAI, AgpdAI *pstSrcAI, BOOL bAddFactor = FALSE);

	BOOL				StreamWriteTemplate(CHAR *szFile);
	BOOL				StreamReadTemplate(CHAR *szFile);

	static BOOL			CBCharacterInit(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL			CBCharacterConstructor(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL			CBCharacterDestructor(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL			CBCharacterTemplateConstructor(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL			CBCharacterTemplateDestructor(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL			CBSpawnConstructor(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL			CBSpawnDestructor(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL			CBAITemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			CBAITemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL			CBCharacterTemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			CBCharacterTemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL			CBSpawnWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			CBSpawnRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	PVOID				MakePacketAIADChar(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
};

#endif // !defined(AFX_AGPMAI_H__A0174039_68D7_4935_BFD4_A316CA0FA4F3__INCLUDED_)
