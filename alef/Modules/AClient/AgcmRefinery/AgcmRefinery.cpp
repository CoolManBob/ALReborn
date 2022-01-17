/*===================================================================

	AgcmRefinery.cpp

===================================================================*/


#include "AgcmRefinery.h"


/********************************************************/
/*		The Implementation of AgcmRefinery class		*/
/********************************************************/
//
AgcmRefinery::AgcmRefinery()
{
	SetModuleName("AgcmRefinery");
	SetModuleType(APMODULE_TYPE_CLIENT);

	EnableIdle(FALSE);

	m_pAgpmRefinery = NULL;
}


AgcmRefinery::~AgcmRefinery()
{
}




//	ApModule inherited
//========================================
//
BOOL AgcmRefinery::OnAddModule()
{
	m_pAgpmRefinery = (AgpmRefinery *) GetModule("AgpmRefinery");

	if (!m_pAgpmRefinery)
		return FALSE;

	return TRUE;
}




//	Send Packet
//===================================
//
//BOOL AgcmRefinery::SendPacketRefine(INT32 lCID, INT32 lItemTID, INT32 *plSourceItems)
//{
//	INT16 nPacketLength = 0;
//	INT8 cOperation = AGPMREFINERY_PACKET_OPERATION_REFINE;
//
//	PVOID pvPacket = m_pAgpmRefinery->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMREFINERY_PACKET_TYPE,
//															&cOperation,
//															&lCID,
//															&lItemTID,
//															plSourceItems,
//															NULL,
//															NULL
//															);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL bResult = SendPacket(pvPacket, nPacketLength);
//	m_pAgpmRefinery->m_csPacket.FreePacket(pvPacket);
//
//	return bResult;
//}

BOOL AgcmRefinery::SendPacketRefineItem(INT32 lCID, INT32 lID, INT32 *plSourceItems)
{
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMREFINERY_PACKET_OPERATION_REFINE_ITEM;

	PVOID pvPacket = m_pAgpmRefinery->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMREFINERY_PACKET_TYPE,
															&cOperation,
															&lCID,
															&lID,
															plSourceItems,
															NULL,
															NULL
															);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmRefinery->m_csPacket.FreePacket(pvPacket);

	return bResult;
}