// AgpmStartupEncryption.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#include "AgpmStartupEncryption.h"

AgpmStartupEncryption::AgpmStartupEncryption()
{
	SetModuleName("AgpmStartupEncryption");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	SetPacketType(AGPMSTARTUPENCRYPTION_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,		// Operation
							AUTYPE_MEMORY_BLOCK,	1,		// Data
							AUTYPE_END,				0
							);
}

AgpmStartupEncryption::~AgpmStartupEncryption()
{
}

BOOL AgpmStartupEncryption::OnAddModule()
{
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	BYTE* pData = NULL;
	INT16 nDataSize = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pData, &nDataSize
						);

	//AuLogFile("Enc.log", "pm; NID:%u, Op:0x%02X, nSize:%d, pData:%p, nDataSize:%d\n", ulNID, cOperation, nSize, pData, nDataSize);

	switch(cOperation)
	{
		case AGPMSTARTUPENCRYPTION_PACKET_REQUEST_PUBLIC:
			OnReceiveRequestPublic(ulNID);
			break;

		case AGPMSTARTUPENCRYPTION_PACKET_PUBLIC:
			OnReceivePublic(pData, nDataSize, ulNID);
			break;

		case AGPMSTARTUPENCRYPTION_PACKET_MAKE_PRIVATE:
			OnReceiveMakePrivate(pData, nDataSize, ulNID);
			break;

		case AGPMSTARTUPENCRYPTION_PACKET_COMPLETE:
			OnReceiveComplete(ulNID);
			break;

		case AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PUBLIC:
			OnReceiveDynCodePublic(pData, nDataSize, ulNID);
			break;

		case AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PRIVATE:
			OnReceiveDynCodePrivate(pData, nDataSize, ulNID);
			break;

		case AGPMSTARTUPENCRYPTION_PACKET_ALGORITHM_TYPE:
			OnReceiveAlgorithmType(pData, nDataSize, ulNID);
			break;
	}

	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceiveAlgorithmType(BYTE* pData, INT16 nSize, UINT32 ulNID)
{
	if(!pData || nSize < 1)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = pData;
	pvBuffer[1] = &nSize;

	EnumCallback(AGPMSTARTUPENCRYPTION_CB_ALGORITHM_TYPE, pvBuffer, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceiveRequestPublic(UINT32 ulNID)
{
	EnumCallback(AGPMSTARTUPENCRYPTION_CB_REQUEST_PUBLIC, NULL, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceivePublic(BYTE* pData, INT16 nSize, UINT32 ulNID)
{
	if(!pData || nSize < 1)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = pData;
	pvBuffer[1] = &nSize;

	EnumCallback(AGPMSTARTUPENCRYPTION_CB_PUBLIC, pvBuffer, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceiveMakePrivate(BYTE* pData, INT16 nSize, UINT32 ulNID)
{
	if(!pData || nSize < 1)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = pData;
	pvBuffer[1] = &nSize;

	EnumCallback(AGPMSTARTUPENCRYPTION_CB_MAKE_PRIVATE, pvBuffer, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceiveComplete(UINT32 ulNID)
{
	EnumCallback(AGPMSTARTUPENCRYPTION_CB_COMPLETE, NULL, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceiveDynCodePublic(BYTE* pCode, INT16 nSize, UINT32 ulNID)
{
	if(!pCode || nSize < 1)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = pCode;
	pvBuffer[1] = &nSize;

	EnumCallback(AGPMSTARTUPENCRYPTION_CB_DYNCODE_PUBLIC, pvBuffer, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::OnReceiveDynCodePrivate(BYTE* pCode, INT16 nSize, UINT32 ulNID)
{
	if(!pCode || nSize < 1)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = pCode;
	pvBuffer[1] = &nSize;

	EnumCallback(AGPMSTARTUPENCRYPTION_CB_DYNCODE_PRIVATE, pvBuffer, &ulNID);
	return TRUE;
}

BOOL AgpmStartupEncryption::SetCallbackAlgorithmType(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_ALGORITHM_TYPE, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackRequestPublic(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_REQUEST_PUBLIC, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackPublic(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_PUBLIC, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackMakePrivate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_MAKE_PRIVATE, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_COMPLETE, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackCheckComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_CHECK_COMPLETE, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackDynCodePublic(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_DYNCODE_PUBLIC, pfCallback, pClass);
}

BOOL AgpmStartupEncryption::SetCallbackDynCodePrivate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSTARTUPENCRYPTION_CB_DYNCODE_PRIVATE, pfCallback, pClass);
}

PVOID AgpmStartupEncryption::MakeStartupPacket(INT16* pnPacketLength, INT8 cOperation, BYTE* pData, INT16 nSize)
{
	if(pData && nSize)
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSTARTUPENCRYPTION_PACKET_TYPE,
									&cOperation,
									pData, &nSize);
	else
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSTARTUPENCRYPTION_PACKET_TYPE,
									&cOperation,
									NULL);
}

BOOL AgpmStartupEncryption::CheckCompleteStartupEncryption(UINT32 ulNID)
{
	BOOL bResult = TRUE;
	EnumCallback(AGPMSTARTUPENCRYPTION_CB_CHECK_COMPLETE, &bResult, &ulNID);
	return bResult;
}