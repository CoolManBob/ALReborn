#pragma once

#include "AgpmEventSiegeWarNPC.h"
#include "AgsmCharacter.h"
#include "AgsmSiegeWar.h"

class AgsmEventSiegeWarNPC : public AgsModule {
public:
	AgsmEventSiegeWarNPC();
	virtual ~AgsmEventSiegeWarNPC();

	BOOL	OnAddModule();

private:
	ApmEventManager			*m_pcsApmEventManager;
	ApmMap					*m_pcsApmMap;
	AgpmSiegeWar			*m_pcsAgpmSiegeWar;
	AgpmEventSiegeWarNPC	*m_pcsAgpmEventSiegeWarNPC;
	AgsmCharacter			*m_pcsAgsmCharacter;
	AgsmSiegeWar			*m_pcsAgsmSiegeWar;

public:
	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
};