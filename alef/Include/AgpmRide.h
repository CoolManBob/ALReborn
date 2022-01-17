#pragma once

#include "ApModule.h"
#include "AuPacket.h"
#include "AgpmItem.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"

enum EnumAgpmRideOperation
{
	AGPMRIDE_OPERATION_NONE		= 0,
	AGPMRIDE_OPERATION_RIDE_REQ,
	AGPMRIDE_OPERATION_RIDE_ACK,
	AGPMRIDE_OPERATION_DISMOUNT_REQ,
	AGPMRIDE_OPERATION_DISMOUNT_ACK,
	AGPMRIDE_OPERATION_TIMEOUT,
	AGPMRIDE_OPERATION_RIDE_TID,
	AGPMRIDE_OPERATION_MAX,
};

enum EnumAgpmRideCallback
{
	AGPMRIDE_CB_NONE		= 0,
	AGPMRIDE_CB_RIDE_REQ,			// Client -> Server
	AGPMRIDE_CB_RIDE_ACK,			// Server -> Client
	AGPMRIDE_CB_DISMOUNT_REQ,		// Client -> Server
	AGPMRIDE_CB_DISMOUNT_ACK,		// Server -> Client
	AGPMRIDE_CB_TIMEOUT,			// Server -> Client
	AGPMRIDE_CB_RIDE_TID,			// Server -> Client
};

class AgpmRide : public ApModule
{
private:
	AgpmItem		*m_pcsAgpmItem;
	AgpmFactors		*m_pcsAgpmFactors;
	AgpmCharacter	*m_pcsAgpmCharacter;

public:
	AuPacket		m_csPacket;

public:
	AgpmRide(void);
	~AgpmRide(void);

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnDestroy();

	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Callback functions
	BOOL SetCallbackRideReq(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRideAck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDismountReq(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDismountAck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTimeout(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRideTID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID MakePacketRide(INT16 *pPacketLength, INT8 cOperation, INT32 lCID, INT32 lRideItemID, INT32 lRemainTime);
};
