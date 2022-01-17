#include "AgpmReturnToLogin.h"

AgpmReturnToLogin::AgpmReturnToLogin()
{
	SetModuleName("AgpmReturnToLogin");
	SetPacketType(AGPMRETURNTOLOGIN_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,				// operation
							AUTYPE_INT32,			1,				// character id
							AUTYPE_CHAR,			AGPDCHARACTER_MAX_ID_LENGTH + 1,		// account name
							AUTYPE_CHAR,			32,				// server group name
							AUTYPE_CHAR,			25,				// login server address
							AUTYPE_INT32,			1,				// encrypt code
							AUTYPE_END,				0
							);
}

AgpmReturnToLogin::~AgpmReturnToLogin()
{
}

BOOL AgpmReturnToLogin::OnAddModule()
{
	m_pcsAgpmCharacter	= (AgpmCharacter *)	GetModule("AgpmCharacter");

	if (!m_pcsAgpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgpmReturnToLogin::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pstCheckArg || !pvPacket || nSize < sizeof(PACKET_HEADER))
		return FALSE;

	INT8	cOperation			= (-1);
	INT32	lCID				= AP_INVALID_CID;
	CHAR	*pszAccountName		= NULL;
	CHAR	*pszServerName		= NULL;
	CHAR	*pszLoginServerAddr	= NULL;
	INT32	lAuthKey			= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&pszAccountName,
						&pszServerName,
						&pszLoginServerAddr,
						&lAuthKey);

	switch (cOperation) {
		case AGPMRETURN_OPERATION_REQUEST:
			{
				if (pstCheckArg->lSocketOwnerID != lCID)
					return FALSE;

				return OnOperationRequest(lCID);
			}
			break;

		case AGPMRETURN_OPERATION_REQUEST_FAILED:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationRequestFailed();
			}
			break;

		case AGPMRETURN_OPERATION_REQUEST_KEY:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationRequestKey(pszAccountName, pszServerName, ulNID);
			}
			break;

		case AGPMRETURN_OEPRATION_RESPONSE_KEY:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationResponseKey(pszAccountName, lAuthKey);
			}
			break;

		case AGPMRETURN_OPERATION_SEND_KEY_ADDR:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationSendKeyAddr(lCID, lAuthKey, pszLoginServerAddr);
			}
			break;

		case AGPMRETURN_OPERATION_RECONNECT_LOGIN_SERVER:
			{
				return OnOperationReconnectLoginServer(pszAccountName, lAuthKey, ulNID);
			}
			break;

		case AGPMRETURN_OPERATION_END_PROCESS:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationEndProcess();
			}
			break;
	}

	return TRUE;
}

BOOL AgpmReturnToLogin::OnOperationRequest(INT32 lCID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	BOOL	bResult	= EnumCallback(AGPMRETURN_CB_REQUEST, pcsCharacter, NULL);

	pcsCharacter->m_Mutex.Release();

	return	bResult;
}

BOOL AgpmReturnToLogin::OnOperationRequestFailed()
{
	return EnumCallback(AGPMRETURN_CB_REQUEST_FAILED, NULL, NULL);
}

BOOL AgpmReturnToLogin::OnOperationRequestKey(CHAR *pszAccountName, CHAR *pszServerName, UINT32 ulNID)
{
	PVOID	pvBuffer[3];
	pvBuffer[0]	= pszAccountName;
	pvBuffer[1]	= pszServerName;
	pvBuffer[2]	= &ulNID;

	return EnumCallback(AGPMRETURN_CB_REQEUST_KEY, pvBuffer, NULL);
}

BOOL AgpmReturnToLogin::OnOperationResponseKey(CHAR *pszAccountName, INT32 lAuthKey)
{
	return EnumCallback(AGPMRETURN_CB_RESPONSE_KEY, pszAccountName, &lAuthKey);
}

BOOL AgpmReturnToLogin::OnOperationSendKeyAddr(INT32 lCID, INT32 lAuthKey, CHAR *pszLoginServerAddr)
{
	return EnumCallback(AGPMRETURN_CB_SEND_KEY_ADDR, &lAuthKey, pszLoginServerAddr);
}

BOOL AgpmReturnToLogin::OnOperationReconnectLoginServer(CHAR *pszAccountName, INT32 lAuthKey, UINT32 ulNID)
{
	if (!pszAccountName || !pszAccountName[0])
		return FALSE;

	PVOID	pvBuffer[3];
	pvBuffer[0]	= pszAccountName;
	pvBuffer[1]	= &lAuthKey;
	pvBuffer[2]	= &ulNID;

	return EnumCallback(AGPMRETURN_CB_RECONNECT_LOGIN_SERVER, pvBuffer, NULL);
}

BOOL AgpmReturnToLogin::OnOperationEndProcess()
{
	return EnumCallback(AGPMRETURN_CB_END_PROCESS, NULL, NULL);
}

BOOL AgpmReturnToLogin::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_REQUEST, pfCallback, pClass);
}

BOOL AgpmReturnToLogin::SetCallbackRequestFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_REQUEST_FAILED, pfCallback, pClass);
}

BOOL AgpmReturnToLogin::SetCallbackRequestKey(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_REQEUST_KEY, pfCallback, pClass);
}

BOOL AgpmReturnToLogin::SetCallbackResponseKey(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_RESPONSE_KEY, pfCallback, pClass);
}

BOOL AgpmReturnToLogin::SetCallbackSendKeyAddr(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_SEND_KEY_ADDR, pfCallback, pClass);
}

BOOL AgpmReturnToLogin::SetCallbackReconnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_RECONNECT_LOGIN_SERVER, pfCallback, pClass);
}

BOOL AgpmReturnToLogin::SetCallbackEndProcess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRETURN_CB_END_PROCESS, pfCallback, pClass);
}

PVOID AgpmReturnToLogin::MakePacketRequest(INT32 lCID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMRETURN_OPERATION_REQUEST;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								&lCID,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmReturnToLogin::MakePacketRequestFailed(INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMRETURN_OPERATION_REQUEST_FAILED;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmReturnToLogin::MakePacketRequestKey(CHAR *pszAccountName, CHAR *pszServerName, INT16 *pnPacketLength)
{
	if (!pszAccountName || !pszAccountName[0] || !pszServerName || !pszServerName[0] || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMRETURN_OPERATION_REQUEST_KEY;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								NULL,
								pszAccountName,
								pszServerName,
								NULL,
								NULL);
}

PVOID AgpmReturnToLogin::MakePacketResponseKey(CHAR *pszAccountName, INT32 lAuthKey, INT16 *pnPacketLength)
{
	if (!pszAccountName || !pszAccountName[0] || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMRETURN_OEPRATION_RESPONSE_KEY;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								NULL,
								pszAccountName,
								NULL,
								NULL,
								&lAuthKey);
}

PVOID AgpmReturnToLogin::MakePacketReceivedAuthKey(INT32 lCID, INT32 lAuthKey, CHAR *pszLoginServerAddr, INT16 *pnPacketLength)
{
	if (/*!pszLoginServerAddr || !pszLoginServerAddr[0] ||*/ !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation	= AGPMRETURN_OPERATION_SEND_KEY_ADDR;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								&lCID,
								NULL,
								NULL,
								pszLoginServerAddr,
								&lAuthKey);
}

PVOID AgpmReturnToLogin::MakePacketReconnectLoginServer(CHAR *pszAccountName, INT32 lAuthKey, INT16 *pnPacketLength)
{
	if (!pszAccountName || !pszAccountName[0] || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMRETURN_OPERATION_RECONNECT_LOGIN_SERVER;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								NULL,
								pszAccountName,
								NULL,
								NULL,
								&lAuthKey);
}

PVOID AgpmReturnToLogin::MakePacketEndProcess(INT16 *pnPacketLength)
{
	INT8	cOperation	= AGPMRETURN_OPERATION_END_PROCESS;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMRETURNTOLOGIN_PACKET_TYPE,
								&cOperation,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}
