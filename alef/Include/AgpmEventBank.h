#ifndef	__AGPMEVENTBANK_H__
#define	__AGPMEVENTBANK_H__

#include "AgpmCharacter.h"

#include "ApmEventManager.h"

#include "AuPacket.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventBankD" )
#else
#pragma comment ( lib , "AgpmEventBank" )
#endif
#endif
//@} Jaewon


#define	AGPMEVENT_BANK_MAX_USE_RANGE				800


typedef enum _AgpmEventBankPacketOperation {
	AGPMEVENT_BANK_PACKET_OPERATION_REQUEST			= 0,
	AGPMEVENT_BANK_PACKET_OPERATION_GRANT,
	AGPMEVENT_BANK_PACKET_OPERATION_GRANT_ANY,
} AgpmEventBankPacketOperation;

typedef enum _AgpmEventBankCBID {
	AGPMEVENT_BANK_CB_REQUEST						= 0,
	AGPMEVENT_BANK_CB_GRANT,
	AGPMEVENT_BANK_CB_GRANT_ANY,
} AgpmEventBankCBID;


class AgpmEventBank : public ApModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;

	ApmEventManager		*m_pcsApmEventManager;

	BOOL				OnOperationRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationGrant(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);

public:
	AuPacket			m_csPacket;

public:
	AgpmEventBank();
	virtual ~AgpmEventBank();

	BOOL				OnAddModule();

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGrantAnywhere(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID				MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketGrantAnywhere(INT32 lCID, INT16 *pnPacketLength);

	static BOOL			CBActionBank(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGPMEVENTBANK_H__