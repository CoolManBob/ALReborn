#include "AgpmCashMall.h"

PVOID AgpmCashMall::MakePacketRequestMallInfo(INT32 lCID, INT16 *pnPacketLength, INT32 lTab)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_REQUEST_MALL_PRODUCT_LIST;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 NULL,
								 NULL,
								 &lCID,
								 &lTab,
								 NULL,
								 NULL,
								 NULL);
}

PVOID AgpmCashMall::MakePacketMallTabInfo(INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT32	lLength		= m_strMallEncodeString.GetLength();
	if (lLength < 1)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_RESPONSE_MALL_LIST;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 NULL,
								 &m_csCashMallInfo.m_ucMallListVersion,
								 NULL,
								 NULL,
								 NULL,
								 m_strMallEncodeString.GetBuffer(), &lLength,
								 NULL);
}

PVOID AgpmCashMall::MakePacketMallProductInfo(INT16 *pnPacketLength, INT32 lTab)
{
	if (!pnPacketLength)
		return NULL;

	INT32	lLength		= m_strProductListEncodeString[lTab].GetLength();
	if (lLength < 1)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_RESPONSE_PRODUCT_LIST;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 NULL,
								 &m_csCashMallInfo.m_ucMallListVersion,
								 NULL,
								 &lTab,
								 NULL,
								 NULL,
								 m_strProductListEncodeString[lTab].GetBuffer(), &lLength);
}

PVOID AgpmCashMall::MakePacketBuyProduct(INT32 lProductID, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_REQUEST_BUY_ITEM;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 NULL,
								 &m_csCashMallInfo.m_ucMallListVersion,
								 &lCID,
								 NULL,
								 &lProductID,
								 NULL,
								 NULL);
}

PVOID AgpmCashMall::MakePacketBuyResult(INT8 cResult, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_RESPONSE_BUY_RESULT;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 &cResult,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL);
}

PVOID AgpmCashMall::MakePacketRefreshCash(INT32 lCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_REFRESH_CASH;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 NULL,
								 NULL,
								 &lCID,
								 NULL,
								 NULL,
								 NULL,
								 NULL);
}

PVOID AgpmCashMall::MakePacketCheckListVersion(INT32 lCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCASH_OPERATION_CHECK_LIST_VERSION;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCASHMALL_PACKET_TYPE,
								 &cOperation,
								 NULL,
								 &m_csCashMallInfo.m_ucMallListVersion,
								 &lCID,
								 NULL,
								 NULL,
								 NULL,
								 NULL);
}