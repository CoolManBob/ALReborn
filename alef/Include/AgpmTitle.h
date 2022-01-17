#if !defined(AGPM_TITLE_H)
#define AGPM_TITLE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "ApModule.h"
#include "AgpaTitle.h"
#include "AgpdTitle.h"
#include "AgppTitle.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmFactors.h"
#include "AgpmProduct.h"
#include "AgpmSkill.h"
#include "AgpmGrid.h"
#include "AgpmItemConvert.h"
#include "AgpmGuild.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmTitleD" )
#else
#pragma comment ( lib , "AgpmTitle" )
#endif
#endif

typedef enum _eAgpmTitleCB
{
	AGPMTITLE_CB_TITLE_ADD = 0,
	AGPMTITLE_CB_TITLE_ADD_RESULT,
	AGPMTITLE_CB_TITLE_USE,
	AGPMTITLE_CB_TITLE_USE_RESULT,
	AGPMTITLE_CB_TITLE_LIST,
	AGPMTITLE_CB_TITLE_LIST_RESULT,
	AGPMTITLE_CB_TITLE_QUEST_REQUEST,
	AGPMTITLE_CB_TITLE_QUEST_REQUEST_RESULT,
	AGPMTITLE_CB_TITLE_QUEST_CHECK,
	AGPMTITLE_CB_TITLE_QUEST_CHECK_RESULT,
	AGPMTITLE_CB_TITLE_QUEST_COMPLETE,
	AGPMTITLE_CB_TITLE_QUEST_COMPLETE_RESULT,
	AGPMTITLE_CB_TITLE_QUEST_LIST,
	AGPMTITLE_CB_TITLE_QUEST_LIST_RESULT,
	AGPMTITLE_CB_TITLE_DELETE,
	AGPMTITLE_CB_TITLE_DELETE_RESULT,
	AGPMTITLE_CB_TITLE_USE_NEAR,
	AGPMTITLE_CB_TITLE_UI_OPEN,
}eAgpmTitleCB;

class AgpmTitle : public ApModule
{
public:
	AgpmCharacter*				m_pAgpmCharacter;
	AgpmItem*					m_pAgpmItem;
	AgpmFactors*				m_pAgpmFactors;
	AgpmProduct*				m_pAgpmProduct;
	AgpmSkill*					m_pAgpmSkill;
	AgpmGrid*					m_pAgpmGrid;
	AgpmItemConvert*			m_pAgpmItemConvert;
	AgpmGuild*					m_pAgpmGuild;

	AgpaTitleTemplate			m_stAgpaTitleTemplate;
	AgpaTitleStringTemplate		m_stAgpaTitleStringTemplate;
	AgpaTitleCategory			m_stAgpaTitleCategory;

public:
	AgpmTitle();
	virtual ~AgpmTitle();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	AgpdTitleTemplate* GetTitleTemplate(INT32 nTitleTid);
	AgpdTitleStringTemplate* GetTitleStringTemplate(INT32 nTitleTid);
	BOOL SetMaxTitleTemplate(INT32 lCount);

	BOOL StreamReadTitleDataTemplate(CHAR* szFile, BOOL bDecryption);
	BOOL StreamReadTitleStringTemplate(CHAR* szFile, BOOL bDecryption);

	BOOL OnOperationTitleUseNear(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket);
	BOOL OnOperationTitleQuest(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket);

	BOOL OnOperationTitleAddResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket);
	BOOL OnOperationTitleUseResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket);
	BOOL OnOperationTitleListResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket);
	BOOL OnOperationTitleDeleteResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE* pvPacket);
	BOOL OnOperationTitleQuestRequestResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket);
	BOOL OnOperationTitleQuestCheckResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket);
	BOOL OnOperationTitleQuestCompleteResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket);
	BOOL OnOperationTitleQuestListResult(AgpdCharacter* pcsCharacter, PACKET_AGPPTITLE_QUEST* pvPacket);

	BOOL SatisfyTitleQuestRequireCondition(AgpdCharacter* pcsCharacter, INT32 lTitleID);
	BOOL SatisfyTitleQuestCompleteCondition(AgpdCharacter* pcsCharacter, INT32 lTitleID);

	BOOL CheckSameItemGroup(INT32 nItemTid1, INT32 nItemTid2);
	INT32 GetGroupItemCount(AgpdGrid *pcsAgpdGrid, INT32 nItemTid);
	INT32 GetItemCountFromCharacter(AgpdCharacter* pcsCharacter, INT32 nItemTid);

	INT32 GetProductSkillLevel(AgpdCharacter* pcsCharacter, INT32 m_eCategory);

	BOOL CheckItemCount(AgpdCharacter *pcsCharacter, INT32 nItemTid, INT32 nItemCount);

	BOOL IsGuildMaster(AgpdCharacter *pcsCharacter);

	BOOL GetEffectValue(AgpdCharacter* pcsCharacter, INT32 nEffectType, INT32 *pnEffectSet, INT32 *pnEffectValue1, INT32 *pnEffectValue2);

	INT32 GetFactorItemOptionValue(AgpdItemOptionTemplate* pcsItemOptionTemplate, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2);
	INT32 GetItemOptionValue(AgpdItem* pcsItem, INT32 lCharacterLevel, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2);
	INT32 GetCharacterStatusByItem(AgpdCharacter* pcsCharacter, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2);
	INT32 GetInitialCharacterStatus(AgpdCharacter* pcsCharacter, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2);
	INT32 GetCharacterStatusItemAndInitialValue(AgpdCharacter* pcsCharacter, eAgpdFactorsType eFactorType, INT32 lFactorSubType1, INT32 lFactorSubType2);
	INT32 GetTitleQuestCheckValueInStatus(AgpdCharacter* pcsCharacter, INT32 lCheckValueType);
	INT32 GetHaveTitleNumber(AgpdCharacter* pcsCharacter);

	BOOL SetTitleQuestCheckValue(AgpdCharacter* pcsCharacter, INT32 lTitleID, INT32 lTitleCheckType, INT32 lCheckSet, INT32 lTitleNewValue);

	BOOL TitleUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 lTimeStamp);
	BOOL TitleUnUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 lTimeStamp);

	BOOL SetCallBackTitleAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleUse(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleUseNear(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallBackTitleQuestRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleQuestCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleQuestComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleQuestList(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallBackTitleAddResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleUseResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleListResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleDeleteResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallBackTitleQuestRequestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleQuestCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleQuestCompleteResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallBackTitleQuestListResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif //AGPM_TITLE_H