#if !defined(_AGSMTITLE_H_)
#define _AGSMTITLE_H_

#include "AgsEngine.h"
#include "AgpmTitle.h"
#include "AgsmCharacter.h"
#include "AgsmServerManager2.h"
#include "AgpmItem.h"
#include "AgsmItemManager.h"
#include "AgspTitleRelayPacket.h"
#include "AgpmSkill.h"
#include "AgsmSkill.h"
#include "AgpmQuest.h"
#include "AgsmDeath.h"
#include "AgpmProduct.h"
#include "AgsmCharManager.h"
#include "AgsmAOIFilter.h"

#include "AuRandomNumber.h"

typedef enum _eTitleLogType
{
	AGSMTITLE_LOG_TITLEQUEST_ADD		= 0,
	AGSMTITLE_LOG_TITLEQUEST_COMPLETE,
	AGSMTITLE_LOG_TITLE_ADD,
	AGSMTITLE_LOG_TITLE_USE,
	AGSMTITLE_LOG_TITLE_DELETE,
	AGSMTITLE_LOG_TITLE_ADD_BY_GM,
	AGSMTITLE_LOG_TITLE_DELETE_BY_GM,
	AGSMTITLE_LOG_TITLE_EDIT_BY_GM,
	AGSMTITLE_LOG_TITLEQUEST_ADD_BY_GM,
	AGSMTITLE_LOG_TITLEQUEST_EDIT_BY_GM,
	AGSMTITLE_LOG_TITLEQUEST_DELETE_BY_GM,
	AGSMTITLE_LOG_TITLEQUEST_COMPLETE_BY_GM,
}eTitleLogType;

class AgsmDeath;
class AgsmItemManager;
class AgsmItem;
class AgpmGrid;
class AgsmParty;
class AgpmLog;

class AgsmTitle : public AgsModule
{
private:
	AgpmTitle				*m_pAgpmTitle;
	AgsmServerManager2		*m_pAgsmServerManager;
	AgsmCharacter			*m_pAgsmCharacter;
	AgpmCharacter			*m_pAgpmCharacter;

	AgpmGrid				*m_pAgpmGrid;
	AgpmItem				*m_pAgpmItem;
	AgsmItem				*m_pAgsmItem;
	AgsmItemManager			*m_pAgsmItemManager;
	AgpmSkill				*m_pAgpmSkill;
	AgsmSkill				*m_pAgsmSkill;
	AgpmQuest				*m_pAgpmQuest;
	AgsmDeath				*m_pAgsmDeath;
	AgpmProduct				*m_pAgpmProduct;
	AgsmCharManager			*m_pAgsmCharManager;
	AgsmAOIFilter			*m_pAgsmAOIFilter;
	AgsmParty				*m_pAgsmParty;
	AgpmLog					*m_pAgpmLog;

	MTRand					m_csRandom;

public:
	AgsmTitle();
	virtual ~AgsmTitle();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	static BOOL CBTitleAdd(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleUse(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleDelete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleQuestRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleQuestCheck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleQuestComplete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTitleQuestList(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBMonsterDeath(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBQuestComplete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRecalcFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGather(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBPickUpItemResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSendTitleDataAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL TitleAddRequest(AgpdCharacter* pcsCharacter, INT32 lTitleID);
	BOOL TitleUseRequest(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bUse);
	BOOL TitleDeleteRequest(AgpdCharacter* pcsCharacter, INT32 lTitleID);

	BOOL TitleQuestCheck(AgpdCharacter* pcsCharacter, INT32 lTitleID, INT32 lCheckType, INT32 lCheckSet, INT32 lNewValue);

	BOOL SkillCast(AgpdCharacter* pcsCharacter, INT32 SkillTid);
	BOOL TitleEffectCast(AgpdCharacter* pcsCharacter, INT32 TitleTid);
	BOOL TitleEffectEnd(AgpdCharacter* pcsCharacter, INT32 TitleTid);

	INT32 GetEffectiveSkillTid(AgpdCharacter* pcsCharacter);

	BOOL SetTitleQuestStatusCheckValue(AgpdCharacter* pcsCharacter, INT32 lTitleID);
	BOOL TitleRewardProcess(AgpdCharacter* pcsCharacter, INT32 lTitleID);

	BOOL RobCostItem(AgpdCharacter *pcsCharacter, INT32 lTitleID);
	BOOL RobItem(AgpdCharacter *pcsCharacter, INT32 nItemTid, INT32 nItemCount);
	INT32 RobItem(AgpdCharacter* pcsCharacter, AgpdGrid* pagpdGrid, INT32 ItemTID, INT32 Count);

	BOOL TitleUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 timeStamp);
	BOOL TitleUnUse(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 timeStamp);

	BOOL UsingTitleSendToClient(AgpdCharacter* pcsCharacter);

	BOOL MonsterDeathByCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMonster);
	BOOL MonsterDeathByParty(AgpdParty *pcsParty, AgpdCharacter* pcsMonster);
	BOOL QuestComplete(AgpdCharacter* pcsCharacter);
	BOOL CharacterLevelUp(AgpdCharacter* pcsCharacter, INT32 lChangeLevel);
	BOOL RecalcFactor(AgpdCharacter* pcsCharacter);
	BOOL OnGather(AgpdCharacter* pcsCharacter, AgpdSkill* pcsSkill, ApBase* pTarget);
	BOOL OnUpdateSkillPoint(AgpdCharacter* pcsCharacter);
	BOOL OnPickUpItemResult(AgpdCharacter* pcsCharacter, AgpdItemTemplate *pcsItemTemplate, INT32 lItemCount);
	BOOL OnPK(AgpdCharacter* pcsCharacter);
	BOOL OnItemUse(AgpdCharacter* pcsCharacter,	AgpdItem* pcsItem);
	BOOL OnEnterGameWorld(AgpdCharacter* pcsCharacter);

	BOOL UpdateTitleQuestCheckValue(AgpdCharacter* pcsCharacter, INT32 lCheckType);
	BOOL UpdateTitleQuestCheckValue(AgpdCharacter* pcsCharacter, INT32 lCheckType, INT32 lCheckSet, INT32 lAddValue);

	BOOL TitleAddResult(PACKET_AGSP_TITLE_ADD_RESULT_RELAY *pPacket);
	BOOL TitleUseResult(PACKET_AGSP_TITLE_USE_RESULT_RELAY *pPacket);
	BOOL TitleListResult(PACKET_AGSP_TITLE_LIST_RESULT_RELAY *pPacket);
	BOOL TitleDeleteResult(PACKET_AGSP_TITLE_DELETE_RESULT *pPacket);

	BOOL TitleListSend(AgpdCharacter* pcsCharacter);

	BOOL TitleQuestRequestResult(PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT *pPacket);
	BOOL TitleQuestCheckResult(PACKET_AGSP_TITLE_QUEST_CHECK_RESULT *pPacket);
	BOOL TitleQuestCompleteResult(PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT *pPacket);
	BOOL TitleQuestListResult(PACKET_AGSP_TITLE_QUEST_LIST_RESULT *pPacket);

	BOOL TitleQuestListSend(AgpdCharacter* pcsCharacter);

	UINT32 CurrentTimeStampStringToTimeStamp(TCHAR *szTimeBuf);

	BOOL GetSelectTitleResult(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter);
	BOOL GetSelectTitleQuestResult(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter);

	BOOL WriteLogTitle(INT32 nLogType, AgpdCharacter *pcsCharacter, INT32 nTitleTid);

	BOOL MakeAndSendTitleAddRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID);
	BOOL MakeAndSendTitleUseRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, INT32 nPauseTitleTid);
	BOOL MakeAndSendTitleListRelayPacket(AgpdCharacter* pcsCharacter);
	BOOL MakeAndSendTitleDeleteRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID);

	BOOL MakeAndSendTitleQuestRequestRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID);
	BOOL MakeAndSendTitleQuestCheckRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitle);
	BOOL MakeAndSendTitleQuestCompleteRelayPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID);
	BOOL MakeAndSendTitleQuestListRelayPacket(AgpdCharacter* pcsCharacter);

	BOOL MakeAndSendTitleAddResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bAddResult);
	BOOL MakeAndSendTitleUseResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bUseResult, BOOL bUse, UINT32 lTimeStamp, UINT32 lFailReason = AGPMTITLE_USE_SUCCESS);
	BOOL MakeAndSendTitleListResultPacket(AgpdCharacter* pcsCharacter, INT32 TitleID, UINT32 TimeStamp, BOOL UseTitle, BOOL TitleListEnd, BOOL ListResult);
	BOOL MakeAndSendTitleDeleteResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bDeleteResult);
	BOOL MakeAndSendTitleUseNearPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bUse);

	BOOL MakeAndSendTitleQuestRequestResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, UINT32 lTimeStamp, BOOL bQuestRequestResult);
	BOOL MakeAndSendTitleQuestCheckResultPacket(AgpdCharacter* pcsCharacter, INT32 lTitleID, BOOL bQuestCheckResult);
	BOOL MakeAndSendTitleQuestCompleteResultPacket(AgpdCharacter* pcsCharacter, INT32 TitleID, UINT32 lTimeStamp, BOOL bQuestCompleteResult);
	BOOL MakeAndSendTitleQuestListResultPacket(AgpdCharacter* pcsCharacter, INT32 TitleID, INT32 *nTitleCurrentValue, BOOL bComplete, UINT32 lAcceptTimeStamp, UINT32 lCompleteTimeStamp, BOOL bListEnd, BOOL bQuestListResult);
};

#endif //_AGSMTITLE_H_