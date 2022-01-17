#ifndef	__AGSMEVENTSKILLMASTER_H__
#define	__AGSMEVENTSKILLMASTER_H__

#include "AgpmEventSkillMaster.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmSkill.h"

#include "AgpmLog.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventSkillMasterD" )
#else
#pragma comment ( lib , "AgsmEventSkillMaster" )
#endif
#endif

typedef enum	_AgsmEventSkillCB {
	AGSMEVENTSKILL_CB_UPDATE_DB				= 0,
} AgsmEventSkillCB;

class AgsmEventSkillMaster : public AgsModule {
private:
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;

	AgsmCharacter			*m_pcsAgsmCharacter;
	AgsmItem				*m_pcsAgsmItem;
	AgsmItemManager			*m_pcsAgsmItemManager;
	AgsmSkill				*m_pcsAgsmSkill;

	AgpmLog					*m_pcsAgpmLog;

public:
	AgsmEventSkillMaster();
	virtual ~AgsmEventSkillMaster();

	BOOL			OnAddModule();

	static BOOL		CBBuySkillBook(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRequestUpgrade(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBUseItemSkillBook(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUseItemSkillScroll(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUseItemSkillRollbackScroll(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBParseSkillTreeString(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBSaveAllSkillData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEndAllSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEndSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBSkillInitialize(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSkillInitLog(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			SendPacketUpgradeResult(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent, INT32 lSkillID, AgpmEventSkillUpgradeResult eResult);
	BOOL			SendPacketLearnResult(AgpdCharacter *pcsCharacter, AgpmEventSkillLearnResult eResult);

	BOOL			EncodingSkillList(AgpdCharacter *pcsCharacter, CHAR *szBuffer, INT32 lBufferLength);
	BOOL			EncodingDefaultSkillList(AgpdCharacterTemplate *pcsCharacterTemplate, CHAR *szBuffer, INT32 lBufferLength);
	BOOL			DecodingSkillList(AgpdCharacter *pcsCharacter, CHAR *szBuffer, INT32 lBufferLength);

	BOOL			SetCallbackUpdateDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 2005.03.15. steeple
	BOOL			WriteLearnLog(AgpdSkill* pcsLearnSkill, INT32 lCostMoney);
	BOOL			WriteUpgradeLog(AgpdSkill* pcsSkill, INT32 lCostMoney);
};

#endif	//__AGSMEVENTSKILLMASTER_H__