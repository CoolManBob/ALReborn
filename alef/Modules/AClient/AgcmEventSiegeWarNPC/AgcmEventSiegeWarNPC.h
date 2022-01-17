#pragma once

#include "AgpmEventSiegeWarNPC.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

class AgcmEventSiegeWarNPC : public AgcModule {
public:
	AgcmEventSiegeWarNPC();
	virtual ~AgcmEventSiegeWarNPC();

	BOOL	OnAddModule();

public:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmEventSiegeWarNPC	*m_pcsAgpmEventSiegeWarNPC;
	AgcmEventManager		*m_pcsAgcmEventManager;
	AgcmCharacter			*m_pcsAgcmCharacter;

	static BOOL	CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
};