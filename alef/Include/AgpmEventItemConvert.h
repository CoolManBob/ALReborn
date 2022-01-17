#ifndef	__AGPMEVENTITEMCONVERT_H__
#define	__AGPMEVENTITEMCONVERT_H__

#include "ApmEventManager.h"
#include "AgpmItemConvert.h"

#include "AuPacket.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventItemConvertD" )
#else
#pragma comment ( lib , "AgpmEventItemConvert" )
#endif
#endif
//@} Jaewon

#define	AGPMEVENT_ITEMCONVERT_MAX_USE_RANGE			800

typedef enum _AgpmEventItemConvertOperation {
	AGPMEVENT_ITEMCONVERT_OPERATION_NONE	= (-1),
	AGPMEVENT_ITEMCONVERT_OPERATION_REQUEST,
	AGPMEVENT_ITEMCONVERT_OPERATION_GRANT,
	AGPMEVENT_ITEMCONVERT_OPERATION_REJECT
} AgpmEventItemConvertOperation;

typedef enum _AgpmEventItemConvertCB {
	AGPMEVENT_ITEMCONVERT_CB_REQUEST,
	AGPMEVENT_ITEMCONVERT_CB_GRANT,
	AGPMEVENT_ITEMCONVERT_CB_REJECT
} AgpmEventItemConvertCB;

class AgpmEventItemConvert : public ApModule {
private:
	ApmEventManager		*m_pcsApmEventManager;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;

public:
	AuPacket			m_csPacket;

private:
	BOOL				OnOperationRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);

public:
	AgpmEventItemConvert();
	virtual ~AgpmEventItemConvert();

	BOOL				OnAddModule();

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	static BOOL			CBActionItemConvert(PVOID pData, PVOID pClass, PVOID pCustData);

	PVOID				MakePacketRequest(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength);
	PVOID				MakePacketGrant(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength);
	PVOID				MakePacketReject(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength);

	BOOL				SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGPMEVENTITEMCONVERT_H__