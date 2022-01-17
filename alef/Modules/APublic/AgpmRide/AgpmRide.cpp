#include ".\agpmride.h"

AgpmRide::AgpmRide(void)
{
	SetModuleName("AgpmRide");
	EnableIdle(FALSE);

	SetPacketType(AGPMRIDE_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// Operation
							AUTYPE_INT32,	1,	// Character ID
							AUTYPE_INT32,	1,	// Item ID (Ride Item)
							AUTYPE_INT32,	1,	// Remain Time (sec)
							AUTYPE_END,		0
							);

	m_pcsAgpmItem = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgpmCharacter = NULL;
}

AgpmRide::~AgpmRide(void)
{
}

BOOL AgpmRide::OnAddModule()
{
	m_pcsAgpmItem		= (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgpmFactors	= (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");

	if (!m_pcsAgpmItem || !m_pcsAgpmFactors || !m_pcsAgpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgpmRide::OnInit()
{
	return TRUE;
}

BOOL AgpmRide::OnDestroy()
{
	return TRUE;
}

BOOL AgpmRide::SetCallbackRideReq(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRIDE_CB_RIDE_REQ, pfCallback, pClass);
}

BOOL AgpmRide::SetCallbackRideAck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRIDE_CB_RIDE_ACK, pfCallback, pClass);
}

BOOL AgpmRide::SetCallbackDismountReq(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRIDE_CB_DISMOUNT_REQ, pfCallback, pClass);
}

BOOL AgpmRide::SetCallbackDismountAck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRIDE_CB_DISMOUNT_ACK, pfCallback, pClass);
}

BOOL AgpmRide::SetCallbackTimeout(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRIDE_CB_TIMEOUT, pfCallback, pClass);
}

BOOL AgpmRide::SetCallbackRideTID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMRIDE_CB_RIDE_TID, pfCallback, pClass);
}

BOOL AgpmRide::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	INT8  cOperation	= -1;
	INT32 CID			= -1;
	INT32 ItemID		= -1;
	INT32 RemainTime	= -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&CID,
						&ItemID,
						&RemainTime);

	if (cOperation >= AGPMRIDE_OPERATION_MAX) 
		return FALSE;

	AgpdCharacter *pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacterLock(CID);
	if (!pcsAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		pcsAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation)
	{
	case AGPMRIDE_OPERATION_RIDE_REQ:		EnumCallback(AGPMRIDE_CB_RIDE_REQ, pcsAgpdCharacter, &ItemID);		break;
	case AGPMRIDE_OPERATION_RIDE_ACK:		EnumCallback(AGPMRIDE_CB_RIDE_ACK, pcsAgpdCharacter, &ItemID);		break;
	case AGPMRIDE_OPERATION_DISMOUNT_REQ:	EnumCallback(AGPMRIDE_CB_DISMOUNT_REQ, pcsAgpdCharacter, &ItemID);	break;
	case AGPMRIDE_OPERATION_DISMOUNT_ACK:	EnumCallback(AGPMRIDE_CB_DISMOUNT_ACK, pcsAgpdCharacter, &ItemID);	break;
	case AGPMRIDE_OPERATION_TIMEOUT:		EnumCallback(AGPMRIDE_CB_TIMEOUT, pcsAgpdCharacter, &ItemID);		break;
	case AGPMRIDE_OPERATION_RIDE_TID:		EnumCallback(AGPMRIDE_CB_RIDE_TID, pcsAgpdCharacter, &ItemID);		break;
	};

	pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

PVOID AgpmRide::MakePacketRide(INT16 *pPacketLength, INT8 cOperation, INT32 lCID, INT32 lRideItemID, INT32 lRemainTime)
{
	ASSERT(NULL != pPacketLength);
	ASSERT(0 < cOperation);
	ASSERT(0 < lCID);

	return m_csPacket.MakePacket(TRUE, pPacketLength, AGPMRIDE_PACKET_TYPE,
								&cOperation,
								&lCID,
								&lRideItemID,
								&lRemainTime);
}