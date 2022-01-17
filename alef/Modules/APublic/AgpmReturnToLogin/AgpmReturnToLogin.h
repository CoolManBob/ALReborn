#pragma once

#include "AgpmCharacter.h"

typedef enum	_AgpmReturnToLoginOperation {
	AGPMRETURN_OPERATION_REQUEST				= 1,
	AGPMRETURN_OPERATION_REQUEST_FAILED,
	AGPMRETURN_OPERATION_REQUEST_KEY,
	AGPMRETURN_OEPRATION_RESPONSE_KEY,
	AGPMRETURN_OPERATION_SEND_KEY_ADDR,
	AGPMRETURN_OPERATION_RECONNECT_LOGIN_SERVER,
	AGPMRETURN_OPERATION_END_PROCESS,
} AgpmReturnToLoginOperation;

typedef enum	_AgpmReturnToLoginCBID {
	AGPMRETURN_CB_REQUEST			= 0,
	AGPMRETURN_CB_REQUEST_FAILED,
	AGPMRETURN_CB_REQEUST_KEY,
	AGPMRETURN_CB_RESPONSE_KEY,
	AGPMRETURN_CB_SEND_KEY_ADDR,
	AGPMRETURN_CB_RECONNECT_LOGIN_SERVER,
	AGPMRETURN_CB_END_PROCESS,
} AgpmReturnToLoginCBID;

class AgpmReturnToLogin : public ApModule {
private:
	AgpmCharacter	*m_pcsAgpmCharacter;

	AuPacket		m_csPacket;

private:
	BOOL	OnOperationRequest(INT32 lCID);
	BOOL	OnOperationRequestFailed();
	BOOL	OnOperationRequestKey(CHAR *pszAccountName, CHAR *pszServerName, UINT32 ulNID);
	BOOL	OnOperationResponseKey(CHAR *pszAccountName, INT32 lAuthKey);
	BOOL	OnOperationSendKeyAddr(INT32 lCID, INT32 lAuthKey, CHAR *pszLoginServerAddr);
	BOOL	OnOperationReconnectLoginServer(CHAR *pszAccountName, INT32 lAuthKey, UINT32 ulNID);
	BOOL	OnOperationEndProcess();

public:
	AgpmReturnToLogin();
	virtual ~AgpmReturnToLogin();

	BOOL	OnAddModule();

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL	SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRequestFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRequestKey(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResponseKey(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSendKeyAddr(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReconnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackEndProcess(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID	MakePacketRequest(INT32 lCID, INT16 *pnPacketLength);
	PVOID	MakePacketRequestFailed(INT16 *pnPacketLength);
	PVOID	MakePacketRequestKey(CHAR *pszAccountName, CHAR *pszServerName, INT16 *pnPacketLength);
	PVOID	MakePacketResponseKey(CHAR *pszAccountName, INT32 lAuthKey, INT16 *pnPacketLength);
	PVOID	MakePacketReceivedAuthKey(INT32 lCID, INT32 lAuthKey, CHAR *pszLoginServerAddr, INT16 *pnPacketLength);
	PVOID	MakePacketReconnectLoginServer(CHAR *pszAccountName, INT32 lAuthKey, INT16 *pnPacketLength);
	PVOID	MakePacketEndProcess(INT16 *pnPacketLength);
};
