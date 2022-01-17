/******************************************************************************
Module:  AgsmObject.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 24
******************************************************************************/

#include "AgsmObject.h"

AgsmObject::AgsmObject()
{
	SetModuleName("AgsmObject");

	SetPacketType(AGPMOBJECT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,			// operation type
							AUTYPE_INT32,		1,			// object id
							AUTYPE_INT8,		1,			// object status
							AUTYPE_END,			0
							);
}

AgsmObject::~AgsmObject()
{
}

BOOL AgsmObject::OnAddModule()
{
	m_papmMap			= (ApmMap *)		GetModule("ApmMap");
	m_papmObject		= (ApmObject *)		GetModule("ApmObject");
	m_pagsmAOIFilter	= (AgsmAOIFilter *)	GetModule("AgsmAOIFilter");
	m_pagsmCharacter	= (AgsmCharacter *)	GetModule("AgsmCharacter");

	if (!m_papmMap || !m_papmObject || !m_pagsmAOIFilter || !m_pagsmCharacter)
		return FALSE;

	m_nIndexADSector = m_papmMap->AttachSectorData(this, sizeof(AgsdObjectADSector), ConAgsdObjectADSector, DesAgsdObjectADSector);
	if (m_nIndexADSector < 0)
		return FALSE;

	if (!m_papmObject->SetCallbackUpdateStatus(CBUpdateStatus, this))
		return FALSE;

//	if (!m_pagsmCharacter->SetCallbackSendSectorInfo(CBSendSectorInfo, this))
//		return FALSE;

	return TRUE;
}

BOOL AgsmObject::OnInit()
{
	return TRUE;
}

BOOL AgsmObject::OnDestroy()
{
	return TRUE;
}

BOOL AgsmObject::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmObject::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lOID			= 0;
	INT8		cStatus			= (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lOID,
						&cStatus);

	switch (cOperation) {
	case AGSMOBJECT_OPERATION_UPDATE:
		{
			if (lOID == 0)
				return FALSE;

			m_papmObject->UpdateStatus(lOID, cStatus);
		}
		break;

	default:
		break;
	}

	return TRUE;
}

BOOL AgsmObject::CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;
/*
	AgsmObject		*pThis		= (AgsmObject *)	pClass;
	ApdObject		*pcsObject	= (ApdObject *)		pData;
	INT32			lPrevStatus	= *((INT32 *)		pCustData);

	INT16			nPacketLength = 0;

	// 주변 넘들에게 뿌려준다.
	PVOID	pvPacket = pThis->MakePacketObjectStatus(pcsObject, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bRetval = TRUE;

	if (!pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsObject->m_stPosition))
	{
		pThis->m_csPacket.FreePacket(pvPacket);
		bRetval = FALSE;
	}

	pThis->m_csPacket.FreePacket(pvPacket);

	// sector에 붙여 놓은 데이타를 정리한다.
	// object의 상태가 APDOBJ_STATUS_NORMAL인 경우는 attachdata에서 빼고
	// APDOBJ_STATUS_NORMAL가 아닌경우 attachData에 추가한다.

	if (pcsObject->m_lCurrentStatus == APDOBJ_STATUS_NORMAL && lPrevStatus != APDOBJ_STATUS_NORMAL)
	{
		// 리스트에서 삭제한다.
		ApWorldSector		*pcsSector	= pThis->m_papmMap->GetSector(pcsObject->m_stPosition);
		if (!pcsSector) return FALSE;

		AgsdObjectADSector	*pcsAttachData = pThis->GetADSector((PVOID) pcsSector);
		if (!pcsAttachData) return FALSE;

		// 찾아서 삭제한다.
		for (int i = 0; i < MAX_OBJECT_COUNT_IN_SECTOR; i++)
		{
			if (pcsAttachData->lObjectID[i] == pcsObject->m_lID)
			{
//				CopyMemory(pcsAttachData->lObjectID + i, 
//						   pcsAttachData->lObjectID + i + 1,
//						   sizeof(INT32) * (MAX_OBJECT_COUNT_IN_SECTOR - i - 1));

				pcsAttachData->lObjectID.MemCopy(i, &pcsAttachData->lObjectID[i + 1], MAX_OBJECT_COUNT_IN_SECTOR - i - 1);

//				CopyMemory(pcsAttachData->pcsObject+ i, 
//						   pcsAttachData->pcsObject + i + 1,
//						   sizeof(ApdObject *) * (MAX_OBJECT_COUNT_IN_SECTOR - i - 1));

				pcsAttachData->pcsObject.MemCopy(i, &pcsAttachData->pcsObject[i + 1], MAX_OBJECT_COUNT_IN_SECTOR - i - 1);

				pcsAttachData->lObjectID[MAX_OBJECT_COUNT_IN_SECTOR - 1] = 0;
				pcsAttachData->pcsObject[MAX_OBJECT_COUNT_IN_SECTOR - 1] = NULL;
			}
		}

		if (i == MAX_OBJECT_COUNT_IN_SECTOR)
			return FALSE;
	}
	else if (pcsObject->m_lCurrentStatus != APDOBJ_STATUS_NORMAL && lPrevStatus == APDOBJ_STATUS_NORMAL)
	{
		// 리스트에 추가한다.
		ApWorldSector		*pcsSector	= pThis->m_papmMap->GetSector(pcsObject->m_stPosition);
		if (!pcsSector) return FALSE;

		AgsdObjectADSector	*pcsAttachData = pThis->GetADSector((PVOID) pcsSector);
		if (!pcsAttachData) return FALSE;

		// 먼저 있는지 찾아본다.
		for (int i = 0; i < MAX_OBJECT_COUNT_IN_SECTOR; i++)
		{
			if (pcsAttachData->lObjectID[i] == 0)
				break;

			if (pcsAttachData->lObjectID[i] == pcsObject->m_lID)
			{
				return FALSE;
			}
		}

		if (i == MAX_OBJECT_COUNT_IN_SECTOR)
			return FALSE;

		pcsAttachData->lObjectID[i] = pcsObject->m_lID;
		pcsAttachData->pcsObject[i] = pcsObject;
	}

	return bRetval;
*/
	return TRUE;
}

BOOL AgsmObject::ConAgsdObjectADSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmObject			*pThis			= (AgsmObject *)	pClass;
	AgsdObjectADSector	*pcsAttachData	= pThis->GetADSector(pData);

	if (!pcsAttachData) return FALSE;

//	ZeroMemory(pcsAttachData->lObjectID, sizeof(INT32) * MAX_OBJECT_COUNT_IN_SECTOR);
//	ZeroMemory(pcsAttachData->pcsObject, sizeof(ApdObject *) * MAX_OBJECT_COUNT_IN_SECTOR);

	pcsAttachData->lObjectID.MemSetAll();
	pcsAttachData->pcsObject.MemSetAll();

	return TRUE;
}

BOOL AgsmObject::DesAgsdObjectADSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgsdObjectADSector* AgsmObject::GetADSector(PVOID pData)
{
	if (!pData)
		return NULL;

	return (AgsdObjectADSector *) m_papmMap->GetAttachedModuleData(m_nIndexADSector, pData);
}

BOOL AgsmObject::CBSendSectorInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;
/*
	AgsmObject			*pThis			= (AgsmObject *)		pClass;
	ApWorldSector		*pcsSector		= (ApWorldSector *)		pData;
	UINT32				ulDPNID			= *(UINT32 *)			pCustData;

	AgsdObjectADSector	*pcsAttachData	= pThis->GetADSector((PVOID) pcsSector);
	if (!pcsAttachData) return FALSE;

	for (int i = 0; i < MAX_OBJECT_COUNT_IN_SECTOR; i++)
	{
		if (!pcsAttachData->pcsObject[i])
		{
			if (pcsAttachData->lObjectID[i] == 0)
				break;
			else
				pcsAttachData->pcsObject[i] = pThis->m_papmObject->GetObject(pcsAttachData->lObjectID[i]);
		}

		if (!pcsAttachData->pcsObject[i])
			break;

		INT16			nPacketLength = 0;

		PVOID	pvPacket = pThis->MakePacketObjectStatus(pcsAttachData->pcsObject[i], &nPacketLength);

		if (!pvPacket || nPacketLength < 1)
			continue;

		pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsSector);

		pThis->m_csPacket.FreePacket(pvPacket);
	}
*/
	return TRUE;
}

PVOID AgsmObject::MakePacketObjectStatus(ApdObject *pcsObject, INT16 *pnPacketLength)
{
	if (!pcsObject || !pnPacketLength)
		return NULL;

	INT8			cOperation	= AGSMOBJECT_OPERATION_UPDATE;
	INT8			cStatus		= (INT8) pcsObject->m_lCurrentStatus;

	// 주변 넘들에게 뿌려준다.
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMOBJECT_PACKET_TYPE,
													&cOperation,
													&pcsObject->m_lID,
													&cStatus);

	return pvPacket;
}