#ifndef	__AGCMEVENTSKILLMASTER_H__
#define	__AGCMEVENTSKILLMASTER_H__

#include "AgpmEventSkillMaster.h"

#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmSkill.h"

#include "AgcmEventManager.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventSkillMasterD" )
#else
#pragma comment ( lib , "AgcmEventSkillMaster" )
#endif
#endif

typedef enum	_AgcmEventSkillCB {
	AGCMEVENT_SKILL_CB_RESULT_SKILL_UPGRADE		= 0,
	AGCMEVENT_SKILL_CB_RESPONSE_GRANT,
} AgcmEventSkillCB;

class AgcmEventSkillMaster : public AgcModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;

	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmItem				*m_pcsAgcmItem;
	AgcmSkill				*m_pcsAgcmSkill;

	AgcmEventManager		*m_pcsAgcmEventManager;

public:
	AgcmEventSkillMaster();
	virtual ~AgcmEventSkillMaster();

	BOOL		OnAddModule();

	BOOL		SendRequestEvent(ApdEvent *pcsEvent);
	BOOL		SendBuySkillBook(ApdEvent *pcsEvent, INT32 lSkillTID);
	BOOL		SendLearnSkill(ApdEvent *pcsEvent, INT32 lSkillTID);
	BOOL		SendRequestSkillUpgrade(ApdEvent *pcsEvent, INT32 lSkillID, INT32 lUpgradePoint);
	BOOL		SendRequestSkillInitialize(ApdEvent *pcsEvent);

	static BOOL	CBExecuteEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBResponseSkillUpgrade(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResponseEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL		SetCallbackResultSkillUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackResponseGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGCMEVENTSKILLMASTER_H__