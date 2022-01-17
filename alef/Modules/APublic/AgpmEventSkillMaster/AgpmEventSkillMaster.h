#ifndef	__AGPMEVENTSKILLMASTER_H__
#define	__AGPMEVENTSKILLMASTER_H__

#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AuPacket.h"
#include "AgpdEventSkillMaster.h"
#include "AgpmEventSkillMasterEvolution.h"

// Common Skill For Race
const INT32 AGPM_EVENT_SKILL_MASTER_RACE_HUMAN		   = 10001;
const INT32 AGPM_EVENT_SKILL_MASTER_RACE_ORC		   = 10002;
const INT32 AGPM_EVENT_SKILL_MASTER_RACE_MOONELF	   = 10003;
const INT32 AGPM_EVENT_SKILL_MASTER_RACE_DRAGON_SCION  = 10004;

typedef enum	_AgpmEventSkillOption {
	AGPMEVENT_SKILL_OPTION_CONDITIONA					= 3,
	AGPMEVENT_SKILL_OPTION_CONDITIONB,	
	AGPMEVENT_SKILL_OPTION_CONDITIONC,					
	AGPMEVENT_SKILL_OPTION_CONDITIOND,
} AgpmEventSkillOption;

typedef enum	_AgpmEventSkillHeroicOption {
	AGPMEVENT_SKILL_HEROIC_OPTION_PRECEDEDSKILLA			= 3,
	AGPMEVENT_SKILL_HEROIC_OPTION_PRECEDEDSKILLB,
	AGPMEVENT_SKILL_HEROIC_OPTION_TOTAL_HEROICPOINT,
}AgpdEventSkillHeroicOption;

typedef enum	_AgpmEventSkillOperation {
	AGPMEVENT_SKILL_OPERATION_BUY_SKILL_BOOK			= 0,
	AGPMEVENT_SKILL_OPERATION_LEARN_SKILL,
	AGPMEVENT_SKILL_OPERATION_REQUEST_UPGRADE,
	AGPMEVENT_SKILL_OPERATION_RESPONSE_UPGRADE,
	AGPMEVENT_SKILL_OPERATION_REQUEST_EVENT,
	AGPMEVENT_SKILL_OPERATION_RESPONSE_EVENT,
	AGPMEVENT_SKILL_OPERATION_LEARN_RESULT,
	AGPMEVENT_SKILL_OPERATION_BUY_RESULT,
	AGPMEVENT_SKILL_OPERATION_SKILL_INITIALIZE,
	AGPMEVENT_SKILL_OPERATION_SKILL_INITIALIZE_RESULT,
} AgpmEventSkillOperation;

typedef enum	_AgpmEventSkillBuyResult {
	AGPMEVENT_SKILL_BUY_RESULT_SUCCESS					= 0,
	AGPMEVENT_SKILL_BUY_RESULT_FAIL,
} AgpmEventSkillBuyResult;

typedef enum	_AgpmEventSkillInitResult {
	AGPMEVENT_SKILL_INITIALIZE_RESULT_SUCCESS					= 0,
	AGPMEVENT_SKILL_INITIALIZE_RESULT_FAIL,
	AGPMEVENT_SKILL_INITIALIZE_RESULT_FAIL_MONEY_OVER,
} AgpmEventSkillInitResult;

typedef enum	_AgpmEventSkillLearnResult {
	AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS				= 0,
	AGPMEVENT_SKILL_LEARN_RESULT_FAIL,
	AGPMEVENT_SKILL_LEARN_RESULT_ALREADY_LEARN,
	AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_SKILLPOINT,
	AGPMEVENT_SKILL_LEARN_RESULT_LOW_LEVEL,
	AGPMEVENT_SKILL_LEARN_RESULT_NOT_LEARNABLE_CLASS,
	AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_HEROICPOINT,
	AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_CHARISMAPOINT,
} AgpmEventSkillLearnResult;

typedef enum	_AgpmEventSkillUpgradeResult {
	AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS				= 0,
	AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_MONEY,
	AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_SKILLPOINT,
	AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_HEROICPOINT,
	AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_CHARISMAPOINT,
	AGPMEVENT_SKILL_UPGRADE_RESULT_FAIL
} AgpmEventSkillUpgradeResult;

typedef enum	_AgpmEventRequestResult {
	AGPMEVENT_SKILL_REQUEST_RESULT_SUCCESS				= 0,
	AGPMEVENT_SKILL_REQUEST_RESULT_FAIL
} AgpmEventRequestResult;

typedef enum	_AgpmEventSkillMasterCB {
	AGPMEVENT_SKILL_CB_BUY_SKILL_BOOK			= 0,
	AGPMEVENT_SKILL_CB_LEARN_SKILL,
	AGPMEVENT_SKILL_CB_REQUEST_UPGRADE,
	AGPMEVENT_SKILL_CB_RESPONSE_UPGRADE,

	AGPMEVENT_SKILL_CB_REQUEST_EVENT,
	AGPMEVENT_SKILL_CB_RESPONSE_EVENT,

	AGPMEVENT_SKILL_CB_LEARN_RESULT,

	AGPMEVENT_SKILL_CB_BUY_RESULT,

	AGPMEVENT_SKILL_CB_SAVE_ALL_DATA,

	AGPMEVENT_SKILL_CB_END_SKILL,
	AGPMEVENT_SKILL_CB_END_ALL_SKILL,
	AGPMEVENT_SKILL_CB_REMOVE_SKILL,

	AGPMEVENT_SKILL_CB_SKILL_INITIALIZE,
	AGPMEVENT_SKILL_CB_SKILL_INITIALIZE_RESULT,

	AGPMEVENT_SKILL_CB_SKILL_INIT_LOG,

} AgpmEventSkillMasterCB;

#define	AGPMEVENT_SKILL_MAX_USE_RANGE			800

#define	AGPMEVENT_SKILL_STREAM_NAME_RACE		"SkillRaceType"
#define	AGPMEVENT_SKILL_STREAM_NAME_CLASS		"SkillClassType"
#define AGPMEVENT_SKILL_STREAM_NAME_EVENT_END	"SkillMasterEnd"

#define AGPMEVENT_SKILL_STREAM_NAME_ARCHLORD	"Archlord"
#define AGPMEVENT_SKILL_STREAM_NAME_RACESKILL	"Race"

#define	AGPMEVENT_SKILL_STREAM_NAME_MASTERY_1	"Battle"
#define	AGPMEVENT_SKILL_STREAM_NAME_MASTERY_2	"Enchant"
#define	AGPMEVENT_SKILL_STREAM_NAME_MASTERY_3	"Special"
#define	AGPMEVENT_SKILL_STREAM_NAME_MASTERY_4	"Passive"
#define	AGPMEVENT_SKILL_STREAM_NAME_MASTERY_5	"Produce"

#define AGPMEVENT_HIGHLEVEL_PASSIVE_HUMAN		10001
#define AGPMEVENT_HIGHLEVEL_PASSIVE_ORC			10002
#define AGPMEVENT_HIGHLEVEL_PASSIVE_MOONELF		10003
#define AGPMEVENT_HIGHLEVEL_PASSIVE_SCION		10004

typedef vector<INT32>							ArchlordSkillList;
typedef vector<INT32>::iterator					ArchlordSkillIter;
typedef vector<INT32>							RaceSkillList;
typedef vector<INT32>::iterator					RaceSkillIter;

typedef vector<AgpdEventSkillHighLevel>					HighLevelSkillVector;
typedef vector<AgpdEventSkillHighLevel>::iterator		HighLevelSkillIter;
typedef std::map<INT32, HighLevelSkillVector>			HighLevelSkillMap;
typedef std::map<INT32, HighLevelSkillVector>::iterator	HighLevelSkillMapIter;

typedef vector<AgpdEventSkillHeroic>					HeroicSkillVector;
typedef vector<AgpdEventSkillHeroic>::iterator			HeroicSkillIter;
typedef std::map<INT32, HeroicSkillVector>				HeroicSkillMap;
typedef std::map<INT32, HeroicSkillVector>::iterator	HeroicSkillMapIter;

class AgpmEventSkillMaster : public ApModule {
private:
	ApmEventManager		*m_pcsApmEventManager;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmItem			*m_pcsAgpmItem;
	AgpmSkill			*m_pcsAgpmSkill;
	
	INT32				m_stMastery[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX][AGPMEVENT_SKILL_MAX_MASTERY][AGPMEVENT_SKILL_MAX_MASTERY_SKILL];

	HighLevelSkillMap	m_mapHighLevelSkill;
	HeroicSkillMap		m_mapHeroicSkill;

	ArchlordSkillList	m_vcArchlordSkill;
	RaceSkillList		m_vcRaceSkill;

	INT16				m_nIndexADSkillTemplate;
	INT16				m_nIndexADCharacterTemplate;

	char*				m_pInitString;

public:
	AuPacket			m_csPacket;
	AuPacket			m_csPacketEventData;

	AgpmEventSkillMasterEvolution m_vcSkillMasteryEvolution;

private:
	BOOL				OnOperationBuySkillBook(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillTID);
	BOOL				OnOperationLearnSkill(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillTID);
	BOOL				OnOperationBuySkillResult(AgpdCharacter *pcsCharacter, INT8 cResult);
	BOOL				OnOperationRequestUpgrade(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillID);
	BOOL				OnOperationResponseUpgrade(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT32 lSkillID, INT8 cResult);
	BOOL				OnOperationRequestEvent(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationResponseEvent(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, INT8 cResult, INT8 cIsInitialize);
	BOOL				OnOperationLearnResult(AgpdCharacter *pcsCharacter, INT8 cResult);
	BOOL				OnOperationSkillInitialize(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationSkillInitializeResult(AgpdCharacter *pcsCharacter, INT8 cResult);

public:
	BOOL				InitializeSkillTree(AgpdCharacter *pcsCharacter, BOOL bAdmin = FALSE, BOOL bCash = FALSE);

public:
	AgpmEventSkillMaster();
	virtual ~AgpmEventSkillMaster();

	BOOL				OnAddModule();

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	PVOID				MakePacketBuySkill(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillTID, INT16 *pnPacketLength);
	PVOID				MakePacketLearnSkill(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillTID, INT16 *pnPacketLength);
	PVOID				MakePacketRequestUpgrade(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillID, INT32 lUpgradedSkillPoint, INT16 *pnPacketLength);
	PVOID				MakePacketResponseUpgrade(ApdEvent *pcsEvent, INT32 lCID, INT32 lSkillID, AgpmEventSkillUpgradeResult eResult, INT16 *pnPacketLength);

	PVOID				MakePacketRequestEvent(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketResponseEvent(ApdEvent *pcsEvent, INT32 lCID, AgpmEventRequestResult eResult, INT16 *pnPacketLength, INT8 cIsInitialize);

	PVOID				MakePacketBuySkillResult(INT32 lCID, AgpmEventSkillBuyResult eResult, INT16 *pnPacketLength);
	PVOID				MakePacketLearnResult(INT32 lCID, AgpmEventSkillLearnResult eResult, INT16 *pnPacketLength);

	PVOID				MakePacketEventData(ApdEvent *pcsEvent);
	BOOL				ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData);

	PVOID				MakePacketRequestSkillInitialize(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketResponseSkillInitialize(INT32 lCID, INT8 cResult, INT16 *pnPacketLength);

	BOOL				SetCallbackBuySkillBook(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackLearnSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackRequestUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackResponseUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackRequestEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackResponseEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackBuyResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackLearnResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackSaveAllData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackRemoveSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackEndAllSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackEndSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackInitSkillLog(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackSkillInitialize(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackSkillInitializeResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL			CBActionSkillMaster(PVOID pData, PVOID pClass, PVOID pCustData);

	INT32*				GetMastery(AuRaceType eRaceType, AuCharClassType eClassType, INT32 lMasteryIndex);
	INT32				GetCharTID(AuRaceType eRaceType, AuCharClassType eClassType);

	AgpdSkill*			GetSkill(AgpdCharacter *pcsCharacter, INT32 lSkillTID);
	INT32				GetOwnSkillCount(AgpdCharacter* pcsCharacter, BOOL bIncludeProduct = FALSE, BOOL bIncludeDefault = FALSE, BOOL bIncludeFreeCost = FALSE);

	INT32				GetBuyCost(INT32 lSkillTID, AgpdCharacter *pcsCharacter, INT32 *plTax = NULL);
	INT32				GetBuyCost(AgpdSkillTemplate *pcsSkillTemplate, AgpdCharacter *pcsCharacter, INT32 *plTax = NULL);

	INT32				GetLearnCostSP(INT32 lSkillTID);
	INT32				GetLearnCostSP(AgpdSkillTemplate *pcsSkillTemplate);

	INT32				GetLearnCostHeroicPoint(INT32 lSkillTID);
	INT32				GetLearnCostHeroicPoint(AgpdSkillTemplate *pcsSkillTemplate);

	INT32				GetLearnCostCharismaPoint(INT32 lSkillTID);
	INT32				GetLearnCostCharismaPoint(AgpdSkillTemplate *pcsSkillTemplate);

	INT32				GetLearnableLevel(INT32 lSkillTID);
	INT32				GetLearnableLevel(AgpdSkillTemplate *pcsSkillTemplate);

	INT32				GetUpgradeCost(AgpdSkill *pcsSkill, AgpdCharacter *pcsCharacter, INT32 *plTax = NULL);
	INT32				GetUpgradeCostSP(AgpdSkill *pcsSkill);

	INT32				GetUpgradeCostHeroicPoint(AgpdSkill *pcsSkill);
	INT32				GetUpgradeCostCharismaPoint(AgpdSkill *pcsSkill);

	INT32				GetInputTotalCostSP(AgpdSkill *pcsSkill);
	INT32				GetInputTotalCostHeroicPoint(AgpdSkill *pcsSkill);
	INT32				GetInputTotalCharismaPoint(AgpdSkill *pcsSkill);
		
	INT32				GetCharacterRaceID(AgpdCharacter* pcsCharacter);

	BOOL				IsFullUpgrade(AgpdSkill *pcsSkill);
	BOOL				IsUpgradable(AgpdSkill *pcsSkill);
	BOOL				IsUpgradable(AgpdSkillTemplate *pcsSkillTemplate);
	BOOL				IsLimitedMaxLevel(AgpdSkillTemplate* pcsSkillTemplate);

	BOOL				IsCurrentClassSkill( AgpdSkill*	pcsSkill );

	INT32				GetMasteryIndex(AgpdSkillTemplate *pcsSkillTemplate);

	BOOL				CheckCharacterStatus(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				CheckValidRange(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, AuPOS *pstDestPos);

	BOOL				CheckBuySkillBook(AgpdCharacter *pcsCharacter, INT32 lSkillTID);
	BOOL				CheckBuySkillBook(AgpdCharacter *pcsCharacter, AgpdSkillTemplate *pcsSkillTemplate);

	AgpmEventSkillLearnResult	CheckLearnSkill(AgpdCharacter *pcsCharacter, AgpdSkillTemplate *pcsSkillTemplate, INT32	nCharacterTID	=	0);
	AgpmEventSkillLearnResult	CheckLearnHighLevelSkill(AgpdCharacter* pcsCharacter, AgpdSkillTemplate* pcsSkillTemplate, INT32 nCharacterTID	=	0);
	AgpmEventSkillLearnResult	CheckLearnHeroicSkill(AgpdCharacter* pcsCharacter, AgpdSkillTemplate* pcsSkillTemplate, INT32 nCharacterTID = 0);


	AgpmEventSkillUpgradeResult	CheckUpgradeSkill(AgpdCharacter *pcsCharacter, AgpdSkill *pcsSkill);
	AgpmEventSkillUpgradeResult	CheckUpgradeHighLevelSkill(AgpdCharacter *pcsCharacter, AgpdSkill *pcsSkill);
	AgpmEventSkillUpgradeResult CheckUpgradeHeroicSkill(AgpdCharacter* pcsCharacter, AgpdSkill *pcsSkill);
	
	AgpdEventSkillAttachTemplateData*	GetSkillAttachTemplateData(AgpdSkillTemplate *pcsTemplate);
	AgpdEventSkillAttachCharTemplateData*	GetSkillAttachCharTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate);

	static BOOL			CBCharTemplateConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCharTemplateDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBMakePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBParsePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSetConstFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBCheckJoinItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBGetInputTotalCostSP(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBGetInputTotalCostHeroicPoint(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBRollbackSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	// streaming functions
	BOOL				StreamReadMasteryTxt(CHAR *szFile, BOOL bDecryption);
	BOOL				StreamReadDefaultSkill(CHAR *szFile, BOOL bDecryption);
	BOOL				StreamReadHighLevelSkill(CHAR* szFile, BOOL bDecryption);
	BOOL				StreamReadMasteryEvolution(CHAR *szFile, BOOL bDecryption);
	BOOL				StreamReadHeroicSkill(CHAR *szFile, BOOL bDecryption);

	INT32				GetTotalLearnCost(AgpdCharacter *pcsCharacter);
	INT32				GetInitializeCost(AgpdCharacter *pcsCharacter);
	INT32				GetTotalCharismaPointByLearnSkill(AgpdCharacter *pcsCharacter);

	BOOL				CheckTargetSkillForRollback(AgpdSkill *pcsSkill);
	BOOL				CheckHighLevelSkillForRollback(AgpdSkill* pcsSkill);

	ArchlordSkillList&	GetArchlordSkillList() { return m_vcArchlordSkill; }
	INT16				StreamReadArchlordSkill(AuExcelLib& csExcelTxtLib, INT16 nRow, INT16 nCol, INT16 nCurRow);

	RaceSkillList&		GetRaceSkillList() { return m_vcRaceSkill; }
	INT16				StreamReadRaceSkill(AuExcelLib& csExcelTxtLib, INT16 nRow, INT16 nCol, INT16 nCurRow);

	HighLevelSkillVector*			GetHighLevelSkillVector(INT32 lCharTID);
	const AgpdEventSkillHighLevel	GetHighLevelSkillInfo(INT32 lSkillTID, INT32 lCharTID = 0);
	const AgpdEventSkillHighLevel	GetHighLevelSkillInfoByIndex(INT32 lIndex, INT32 lCharTID);
	INT32							GetHighLevelSkillIndex(INT32 lSkillTID, INT32 lCharTID = 0);
	BOOL							IsHighLevelSkill(INT32 lSkillTID, INT32 lCharTID = 0);

	HeroicSkillVector*				GetHeroicSkillVector(INT32 lCharTID);
	const AgpdEventSkillHeroic		GetHeroicSkillInfo(INT32 lSkillTID, INT32 lCharTID = 0);
	const AgpdEventSkillHeroic		GetHeroicSkillInfoByIndex(INT32 lIndex, INT32 lCharTID);
	INT32							GetHeroicSkillIndex(INT32 lSkillTID, INT32 lCharTID = 0);
	BOOL							IsHeroicSkill(INT32 lSkillTID, INT32 lCharTID = 0);

	// Condition Check ¸¦ ÇÑ´Ù
	BOOL							CheckSkillCondition			( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate );
	BOOL							CheckSkillCondition			( AgpdSkill* pcsSkill );

	BOOL							CheckHeroicSkillCondition	( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate );
	BOOL							CheckHeroicSkillCondition	( AgpdSkill* pcsSkill );

	BOOL							CheckHeroicSkillForClass	( AgpdCharacter *pcsCharacter, AgpdSkillTemplate* pcsSkillTemplate);

private:
	BOOL							_CheckConditionAB				( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate  );
	BOOL							_CheckConditionC				( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate  );
	BOOL							_CheckConditionD				( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate  );

	BOOL							_CheckHeroicConditionA			( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate );
	BOOL							_CheckHeroicConditionD			( AgpdCharacter* pcsCharacter , AgpdSkillTemplate*	pcsSkillTemplate );


};

#endif	//__AGPMEVENTSKILLMASTER_H__