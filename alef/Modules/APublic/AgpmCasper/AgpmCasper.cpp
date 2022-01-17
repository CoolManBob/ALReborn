#include "AgpmCasper.h"

AgpmCasper::AgpmCasper()
{
	SetModuleName("AgpmCasper");

	SetPacketType(AGPMCASPER_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(CHAR));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,				// operation
							AUTYPE_INT32,			1,				// created character id
							AUTYPE_END,				0
							);
}

AgpmCasper::~AgpmCasper()
{
}

BOOL AgpmCasper::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1 || ulType != AGPMCASPER_PACKET_TYPE)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lCreatedCID		= AP_INVALID_CID;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCreatedCID);

	switch (cOperation) {
	case AGPMCASPER_OPERATION_REQUEST_CONNETION:
		EnumCallback(AGPMCASPER_CB_REQUEST_CONNECTION, &ulNID, NULL);
		break;

	case AGPMCASPER_OPERATION_CONNECTION_SUCCESS:
		EnumCallback(AGPMCASPER_CB_CONNECTION_SUCCESS, &lCreatedCID, &ulNID);
		break;

	case AGPMCASPER_OPERATION_CONNECTION_FAIL:
		EnumCallback(AGPMCASPER_CB_CONNECTION_FAIL, &ulNID, NULL);
		break;

	case AGPMCASPER_OPERATION_MEMORY_OVERFLOW:
		EnumCallback(AGPMCASPER_CB_MEMORY_OVERFLOW, &ulNID, NULL);
		break;
	}

	return TRUE;
}

PVOID AgpmCasper::MakePacketRequestConnection(INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASPER_OPERATION_REQUEST_CONNETION;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASPER_PACKET_TYPE,
								 &cOperation,
								 NULL);
}

PVOID AgpmCasper::MakePacketConnectionSuccess(INT32 lCreatedCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength || lCreatedCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation	= AGPMCASPER_OPERATION_CONNECTION_SUCCESS;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASPER_PACKET_TYPE,
								 &cOperation,
								 &lCreatedCID);
}

PVOID AgpmCasper::MakePacketConnectionFail(INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASPER_OPERATION_CONNECTION_FAIL;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASPER_PACKET_TYPE,
								 &cOperation,
								 NULL);
}

PVOID AgpmCasper::MakePacketMemoryOverflow(INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASPER_OPERATION_MEMORY_OVERFLOW;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASPER_PACKET_TYPE,
		&cOperation,
		NULL);
}

BOOL AgpmCasper::SetCallbackRequestConnection(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASPER_CB_REQUEST_CONNECTION, pfCallback, pClass);
}

BOOL AgpmCasper::SetCallbackConnectionSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASPER_CB_CONNECTION_SUCCESS, pfCallback, pClass);
}

BOOL AgpmCasper::SetCallbackConnectionFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASPER_CB_CONNECTION_FAIL, pfCallback, pClass);
}

BOOL AgpmCasper::SetCallbackMemoryOverflow(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASPER_CB_MEMORY_OVERFLOW, pfCallback, pClass);
}