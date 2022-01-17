#include "AgpmItemConvert.h"

PVOID AgpmItemConvert::MakePacketAdd(AgpdItem *pcsItem, INT16 *pnPacketLength)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumSocket == 0 &&
		pcsAttachData->m_lPhysicalConvertLevel == 0)
		return NULL;

	PVOID	pvPacketTID	= NULL;

	if (pcsAttachData->m_lNumConvert > 0)
	{
		pvPacketTID	= m_csPacketTID.MakePacket(FALSE, pnPacketLength, 0,
												(pcsAttachData->m_stSocketAttr[0].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[0].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[1].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[1].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[2].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[2].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[3].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[3].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[4].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[4].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[5].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[5].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[6].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[6].lTID : NULL,
												(pcsAttachData->m_stSocketAttr[7].lTID != AP_INVALID_IID) ? &pcsAttachData->m_stSocketAttr[7].lTID : NULL);
	}

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_ADD;

	INT8	cNumPhysicalConvert	= (INT8) pcsAttachData->m_lPhysicalConvertLevel;
	INT8	cNumSocket			= (INT8) pcsAttachData->m_lNumSocket;
	INT8	cNumConvertedSocket	= (INT8) pcsAttachData->m_lNumConvert;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
												&cOperation,
												(pcsItem->m_pcsCharacter) ? &pcsItem->m_pcsCharacter->m_lID : NULL,
												&pcsItem->m_lID,
												NULL,
												NULL,
												(cNumPhysicalConvert > 0) ? &cNumPhysicalConvert : NULL,
												(cNumSocket > 0) ? &cNumSocket : NULL,
												(cNumConvertedSocket > 0) ? &cNumConvertedSocket : NULL,
												NULL,
												(cNumConvertedSocket > 0) ? pvPacketTID : NULL);

	if (pvPacketTID)
		m_csPacketTID.FreePacket(pvPacketTID);

	return pvPacket;
}

PVOID AgpmItemConvert::MakePacketRequestPhysicalConvert(INT32 lCID, INT32 lIID, INT32 lCatalystIID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lCatalystIID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_REQUEST_PHYSICAL_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								&lCatalystIID,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketRequestSocketAdd(INT32 lCID, INT32 lIID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_REQUEST_ADD_SOCKET;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketRequestRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneIID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lRuneIID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_REQUEST_RUNE_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								&lRuneIID,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketRequestSpiritStoneConvert(INT32 lCID, INT32 lIID, INT32 lSpiritStoneIID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lSpiritStoneIID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_REQUEST_SPIRITSTONE_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								&lSpiritStoneIID,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketResponsePhysicalConvert(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_RESPONSE_PHYSICAL_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								NULL,
								&cActionResult,
								&cNumPhysicalConvert,
								&cNumSocket,
								&cNumConvertedSocket,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketResponseSocketAdd(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_RESPONSE_ADD_SOCKET;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								NULL,
								&cActionResult,
								&cNumPhysicalConvert,
								&cNumSocket,
								&cNumConvertedSocket,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketResponseSpiritStoneConvert(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_RESPONSE_SPIRITSTONE_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								NULL,
								&cActionResult,
								&cNumPhysicalConvert,
								&cNumSocket,
								&cNumConvertedSocket,
								&lTID,
								NULL);
}

PVOID AgpmItemConvert::MakePacketResponseRuneConvert(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_RESPONSE_RUNE_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								NULL,
								&cActionResult,
								&cNumPhysicalConvert,
								&cNumSocket,
								&cNumConvertedSocket,
								&lTID,
								NULL);
}

PVOID AgpmItemConvert::MakePacketResponseSpiritStoneCheckResult(INT32 lCID, INT32 lIID, INT32 lSpiritStoneID, INT8 cActionResult, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lSpiritStoneID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_RESPONSE_SPIRITSTONE_CHECK_RESULT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								&lSpiritStoneID,
								&cActionResult,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketResponseRuneCheckResult(INT32 lCID, INT32 lIID, INT32 lRuneID, INT8 cActionResult, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lRuneID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_RESPONSE_RUNE_CHECK_RESULT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lIID,
								&lRuneID,
								&cActionResult,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);
}

PVOID AgpmItemConvert::MakePacketCheckCashRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneIID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lRuneIID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_CHECK_CASH_RUNE_CONVERT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
		&cOperation,
		&lCID,
		&lIID,
		&lRuneIID,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
}

PVOID AgpmItemConvert::MakePacketSocketInitialize(INT32 lCID, INT32 lIID, INT32 lInitIID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID ||
		lInitIID == AP_INVALID_IID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDITEMCONVERT_OPERATION_SOCKET_INITIALIZE;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEMCONVERT_PACKET_TYPE,
		&cOperation,
		&lCID,
		&lIID,
		&lInitIID,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
}