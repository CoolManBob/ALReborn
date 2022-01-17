// AgcmEventGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 13.

#ifndef _AGCMEVENTGUILD_H_
#define _AGCMEVENTGUILD_H_

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventGuildD.lib")
#else
	#pragma comment(lib, "AgcmEventGuild.lib")
#endif
#endif

#include "AgpmEventGuild.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

typedef enum _eAgcmEventGuildCallback
{
	AGCMEVENTGUILD_CB_GRANT = 0,
	AGCMEVENTGUILD_CB_GRANT_WAREHOUSE,
	AGCMEVENTGUILD_CB_GRANT_WORLD_CHAMPIONSHIP,
} eAgcmEventGuildCallBack;

class AgcmEventGuild : public AgcModule
{
private:
	AgpmCharacter* m_pcsAgpmCharacter;
	AgpmEventGuild* m_pcsAgpmEventGuild;

	AgcmCharacter* m_pcsAgcmCharacter;
	AgcmEventManager* m_pcsAgcmEventManager;
	
public:
	AgcmEventGuild();
	virtual ~AgcmEventGuild();

	BOOL OnAddModule();
	
	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGrantWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventRequestWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrantWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);	
	static BOOL CBEventRequestWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrantWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif //_AGCMEVENTGUILD_H_