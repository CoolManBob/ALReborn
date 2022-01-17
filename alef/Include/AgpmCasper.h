#ifndef	__AGPMCASPER_H__
#define	__AGPMCASPER_H__

#include "ApDefine.h"
#include "ApModule.h"
#include "AuPacket.h"


//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmCasperD" )
#else
#pragma comment ( lib , "AgpmCasper" )
#endif
#endif
//@} Jaewon

typedef enum	_AgpmCasperPacketOperation {
	AGPMCASPER_OPERATION_REQUEST_CONNETION			= 0,
	AGPMCASPER_OPERATION_CONNECTION_SUCCESS,
	AGPMCASPER_OPERATION_CONNECTION_FAIL,
	AGPMCASPER_OPERATION_MEMORY_OVERFLOW,
} AgpmCasperPacketOperation;

// callback function id
typedef enum	_AgpmCasperCB {
	AGPMCASPER_CB_REQUEST_CONNECTION				= 0,
	AGPMCASPER_CB_CONNECTION_SUCCESS,
	AGPMCASPER_CB_CONNECTION_FAIL,
	AGPMCASPER_CB_MEMORY_OVERFLOW,
} AgpmCasperCB;

class AgpmCasper : public ApModule {
public:
	AuPacket	m_csPacket;

public:
	AgpmCasper();
	virtual	~AgpmCasper();

	BOOL		OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	PVOID		MakePacketRequestConnection(INT16 *pnPacketLength);
	PVOID		MakePacketConnectionSuccess(INT32 lCreatedCID, INT16 *pnPacketLength);
	PVOID		MakePacketConnectionFail(INT16 *pnPacketLength);
	PVOID		MakePacketMemoryOverflow(INT16 *pnPacketLength);

	BOOL		SetCallbackRequestConnection(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackConnectionSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackConnectionFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackMemoryOverflow(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};


#endif	//__AGPMCASPER_H__
