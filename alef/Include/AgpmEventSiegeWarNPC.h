#pragma once

#include "ApmEventManager.h"
#include "AgpmSiegeWar.h"

typedef enum AgpmEventSiegeWarNPCOperation {
	AGPMEVENTSIEGEWAR_EVENT_REQUEST				= 1,
	AGPMEVENTSIEGEWAR_EVENT_GRANT,
};

typedef enum AgpmEventSiegeWarNPCCBID {
	AGPMEVENTSIEGEWAR_CB_EVENT_GRANT			= 0,
};

class AgpmEventSiegeWarNPC : public ApModule {
public:
	AgpmEventSiegeWarNPC();
	virtual ~AgpmEventSiegeWarNPC();

	BOOL	OnAddModule();

	AuPacket	m_csPacket;
	AuPacket	m_csPacketEventData;

public:
	ApmEventManager		*m_pcsApmEventManager;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmSiegeWar		*m_pcsAgpmSiegeWar;

public:
	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

private:
	BOOL				OnOperationEventRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationEventGrant(ApdEvent *pcsEvent);

public:
	static BOOL			CBActionSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID				MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketEventGrant(ApdEvent *pcsEvent, INT16 *pnPacketLength);

	AgpdSiegeWar*		GetSiegeWarInfo(ApdEvent *pcsEvent);
};