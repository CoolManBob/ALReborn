	/******************************************************************************
Module:  AgsmAOIFilter.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 23
******************************************************************************/

#include "AgsmAOIFilter.h"
#include "ApMemoryTracker.h"
#include "AgpmSiegeWar.h"

AgsmAOIFilter::AgsmAOIFilter()
{
	SetModuleName("AgsmAOIFilter");

	SetModuleType(APMODULE_TYPE_SERVER);

	m_csCryptActor.SetCryptType(AUCRYPT_TYPE_PUBLIC);
	
	m_papmMap			= NULL;
	m_pagpmFactors		= NULL;
	m_pagpmCharacter	= NULL;
	m_pagpmItem			= NULL;
	m_pagpmSiegeWar		= NULL;
}

AgsmAOIFilter::~AgsmAOIFilter()
{
}

BOOL AgsmAOIFilter::OnAddModule()
{
	m_papmMap = (ApmMap *) GetModule("ApmMap");
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pagpmSiegeWar = (AgpmSiegeWar *) GetModule("AgpmSiegeWar");

	if (!m_papmMap || !m_pagpmFactors || !m_pagpmCharacter || !m_pagpmItem)
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackAddCharacterToMap(CallbackAddCharacterToMap, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackRemoveCharacterFromMap(CallbackRemoveCharacterFromMap, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackPreUpdatePosition(CallbackUpdateCharacterPosition, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackResetMonster(CallbackResetMonster, this))
		return FALSE;

	if (!m_pagpmItem->SetCallbackAddItemToMap(CallbackAddItemToMap, this))
		return FALSE;
	if (!m_pagpmItem->SetCallbackRemoveItemFromMap(CallbackRemoveItemFromMap, this))
		return FALSE;

	m_nIndexADCharacter = m_pagpmCharacter->AttachCharacterData(this, sizeof(AgsmCellUnit), ConCharacterCellUnit, DesCharacterCellUnit);
	m_nIndexADItem = m_pagpmItem->AttachItemData(this, sizeof(AgsmCellUnit), ConItemCellUnit, DesItemCellUnit);

	return TRUE;
}

BOOL AgsmAOIFilter::OnInit()
{
	if (!m_csAdminAllGroup.InitializeObject(sizeof(UINT32), 10000))
		return FALSE;

	return TRUE;
}

BOOL AgsmAOIFilter::OnDestroy()
{
	m_csAdminAllGroup.RemoveObjectAll();

	return TRUE;
}

BOOL AgsmAOIFilter::InitAttachData(AgpdCharacter *pcsCharacter)
{
	AgsmCellUnit	*pstCellUnit	= GetADCharacter(pcsCharacter);
	if (!pstCellUnit)
		return FALSE;

	pstCellUnit->lID				= 0;
	pstCellUnit->pcsCurrentCell		= NULL;
	pstCellUnit->pNext				= NULL;
	pstCellUnit->pPrev				= NULL;
	pstCellUnit->bIsPC				= FALSE;

	pstCellUnit->GuardByte1			= 0;
	pstCellUnit->GuardByte2			= 0;
	pstCellUnit->GuardByte3			= 0;

	pstCellUnit->bWriteLog			= FALSE;

	return TRUE;
}

BOOL AgsmAOIFilter::ConCharacterCellUnit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter	*pThis			= (AgsmAOIFilter *)	pClass;

	return pThis->InitAttachData((AgpdCharacter *) pData);
}

BOOL AgsmAOIFilter::DesCharacterCellUnit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter	*pThis			= (AgsmAOIFilter *)	pClass;

	AgsmCellUnit	*pstCellUnit	= pThis->GetADCharacter(pData);

	if (pstCellUnit->pcsCurrentCell)
	{
		// 이런 경우는 있으면 안된다...

		pstCellUnit->pcsCurrentCell->RemoveCharacter(pstCellUnit);
	}

	return TRUE;
}

BOOL AgsmAOIFilter::ConItemCellUnit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter	*pThis			= (AgsmAOIFilter *)	pClass;

	AgsmCellUnit	*pstCellUnit	= pThis->GetADItem(pData);

	pstCellUnit->lID				= 0;
	pstCellUnit->pcsCurrentCell		= NULL;
	pstCellUnit->pNext				= NULL;
	pstCellUnit->pPrev				= NULL;

	pstCellUnit->GuardByte1			= 0;
	pstCellUnit->GuardByte2			= 0;
	pstCellUnit->GuardByte3			= 0;

	pstCellUnit->bWriteLog			= FALSE;

	return TRUE;
}

BOOL AgsmAOIFilter::DesItemCellUnit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter	*pThis			= (AgsmAOIFilter *)	pClass;

	AgsmCellUnit	*pstCellUnit	= pThis->GetADItem(pData);

	if (pstCellUnit->pcsCurrentCell)
	{
		// 이런 경우는 있으면 안된다...

		pstCellUnit->pcsCurrentCell->RemoveItem(pstCellUnit);
	}

	return TRUE;
}

BOOL AgsmAOIFilter::CallbackAddCharacterToMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter		*pThis				= (AgsmAOIFilter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	AgsmAOICell			*pcsCell			= pThis->m_csAOIMap.GetCell(&pcsCharacter->m_stPos);
	if (!pcsCell)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackAddCharacterToMap"));

	pThis->EnumCallback(AGSMAOI_CB_ID_ADD_CHAR, pcsCell, pcsCharacter);

	UINT_PTR	ulDPNID;
	pThis->EnumCallback(AGSMAOI_CB_ID_GET_DPNID, pcsCharacter, &ulDPNID);

	AgsmCellUnit		*pstCellUnit		= pThis->GetADCharacter(pcsCharacter);
	pstCellUnit->lID						= (INT_PTR) pcsCharacter;
	pstCellUnit->pcsCurrentCell				= NULL;
	pstCellUnit->pNext						= NULL;
	pstCellUnit->pPrev						= NULL;
	pstCellUnit->ulNID						= (UINT32)ulDPNID;

	pThis->CheckValidLock(&pcsCharacter->m_Mutex);

	if (ulDPNID != 0 || pThis->m_pagpmCharacter->IsPC(pcsCharacter) || pThis->m_pagpmCharacter->IsCreature(pcsCharacter))
		pstCellUnit->bIsPC						= TRUE;

	if (pstCellUnit->bIsPC)
	{
		if (!pThis->m_csAOIMap.AddCharacterToMap(&pcsCharacter->m_stPos, pstCellUnit))
			return FALSE;

		pThis->m_csAdminAllGroup.AddObject(&ulDPNID, ulDPNID);
	}
	else if (pThis->m_pagpmCharacter->IsMonster(pcsCharacter))
	{
		if (!pThis->m_csAOIMap.AddMonsterToMap(&pcsCharacter->m_stPos,
											   pstCellUnit,
											   (pThis->m_pagpmSiegeWar) ? pThis->m_pagpmSiegeWar->IsSiegeWarMonster(pcsCharacter) : FALSE))
			return FALSE;
	}
	else
	{
		if (!pThis->m_csAOIMap.AddNPCToMap(&pcsCharacter->m_stPos,
												pstCellUnit,
												(pThis->m_pagpmSiegeWar) ? pThis->m_pagpmSiegeWar->IsSiegeWarMonster(pcsCharacter) : FALSE))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmAOIFilter::CallbackRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter		*pThis				= (AgsmAOIFilter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	AgsmCellUnit		*pstCellUnit		= pThis->GetADCharacter(pcsCharacter);
	if (!pstCellUnit)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackRemoveCharacterFromMap"));

	//AgsmAOICell			*pcsCell			= pThis->m_csAOIMap.GetCell(&pcsCharacter->m_stPos);
	AgsmAOICell			*pcsCell			= pstCellUnit->pcsCurrentCell;
	if (!pcsCell)
		return FALSE;

	pThis->CheckValidLock(&pcsCharacter->m_Mutex);

	if (pstCellUnit->bIsPC)
	{
		pThis->m_csAOIMap.RemoveCharacterFromMap(pstCellUnit);

		UINT_PTR				ulDPNID				= 0;

		if (pThis->EnumCallback(AGSMAOI_CB_ID_GET_DPNID, pcsCharacter, &ulDPNID))
			pThis->m_csAdminAllGroup.RemoveObject(ulDPNID);
	}
	else if (pThis->m_pagpmCharacter->IsMonster(pcsCharacter))
	{
		pThis->m_csAOIMap.RemoveMonsterFromMap(pstCellUnit);
	}
	else
	{
		pThis->m_csAOIMap.RemoveNPCFromMap(pstCellUnit);
	}

	pThis->EnumCallback(AGSMAOI_CB_ID_REMOVE_CHAR, pcsCell, pcsCharacter);

	return TRUE;
}

// 2008.04.08. steeple
// Update Position 콜백을 Region Update 하기 전에 불리우는 것으로 바꿨다.
BOOL AgsmAOIFilter::CallbackUpdateCharacterPosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;
	
	PROFILE("AgsmAOIFilter:CallbackUpdateCharacterPosition");

	AgsmAOIFilter		*pThis				= (AgsmAOIFilter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	AuPOS				*pstPrevPos			= (AuPOS *)				pCustData;

	if(!pThis || !pcsCharacter)
		return FALSE;

	AgsmCellUnit		*pstCellUnit		= pThis->GetADCharacter(pcsCharacter);
	if(!pstCellUnit)
		return FALSE;

	AgsmAOICell			*pcsPrevCell		= pstCellUnit->pcsCurrentCell;

	AgsmAOICell			*pcsNewCell			= NULL;

	pThis->CheckValidLock(&pcsCharacter->m_Mutex);
	
	if (pstCellUnit->bIsPC)
		pcsNewCell	= pThis->m_csAOIMap.UpdateCharacterPosition(&pcsCharacter->m_stPos, pstCellUnit);
	else if (pThis->m_pagpmCharacter->IsMonster(pcsCharacter))
		pcsNewCell	= pThis->m_csAOIMap.UpdateMonsterPosition(&pcsCharacter->m_stPos, pstCellUnit);
	else
		pcsNewCell	= pThis->m_csAOIMap.UpdateNPCPosition(&pcsCharacter->m_stPos, pstCellUnit);

	if (pcsNewCell)
	{
		PVOID	pvBuffer[3];
		pvBuffer[0]		= (PVOID) pcsNewCell;
		pvBuffer[1]		= (PVOID) pcsPrevCell;
		pvBuffer[2]		= (PVOID) pstPrevPos;

		{
			PROFILE("AgsmAOIFilter:MoveCharCallback");

			pThis->EnumCallback(AGSMAOI_CB_ID_MOVE_CHAR, pvBuffer, pcsCharacter);
		}
	}

	return TRUE;
}

BOOL AgsmAOIFilter::CallbackResetMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter		*pThis				= (AgsmAOIFilter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	return pThis->InitAttachData(pcsCharacter);
}

BOOL AgsmAOIFilter::CallbackAddItemToMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter		*pThis				= (AgsmAOIFilter *)		pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	AgsmAOICell			*pcsCell			= pThis->m_csAOIMap.GetCell(&pcsItem->m_posItem);

	AgsmCellUnit		*pcsCellUnit		= pThis->GetADItem(pcsItem);
	pcsCellUnit->lID						= (INT_PTR) pcsItem;
	pcsCellUnit->pcsCurrentCell				= NULL;
	pcsCellUnit->pNext						= NULL;
	pcsCellUnit->pPrev						= NULL;

	if (!pThis->m_csAOIMap.AddItemToMap(&pcsItem->m_posItem, pcsCellUnit))
		return FALSE;

	pThis->EnumCallback(AGSMAOI_CB_ID_ADD_ITEM, pcsCell, pcsItem);

	return TRUE;
}

BOOL AgsmAOIFilter::CallbackRemoveItemFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAOIFilter		*pThis				= (AgsmAOIFilter *)		pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	AgsmCellUnit		*pcsCellUnit		= pThis->GetADItem(pcsItem);
	
	AgsmAOICell			*pcsCell			= pcsCellUnit->pcsCurrentCell;

	if (!pThis->m_csAOIMap.RemoveItemFromMap(pcsCellUnit))
		return FALSE;

	pThis->EnumCallback(AGSMAOI_CB_ID_REMOVE_ITEM, pcsCell, pcsItem);

	return TRUE;
}

//		SendPacketNear
//	Functions
//		- 시야 내에 존재하는 섹터들에 데이타를 보낸다.
//	Arguments
//		- pvPacket : 보낼 패킷
//		- nLength : 패킷 길이
//		- posCurrent : 이 패킷을 보내는 기준점
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmAOIFilter::SendPacketNear(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, PACKET_PRIORITY ePriority, INT32 lRegionIndex)
{
	if (!pvPacket || !nLength)
		return FALSE;

	AgsmAOICell		*pcsCell		= m_csAOIMap.GetCell(&posCurrent);

	if(lRegionIndex < 0)
		lRegionIndex = m_papmMap->GetRegion(posCurrent.x, posCurrent.z);

	return SendPacketNear(pvPacket, nLength, pcsCell, lRegionIndex, ePriority);
}

//		SendPacketNear
//	Functions
//		- 시야 내에 존재하는 섹터들에 데이타를 보낸다.
//			1. pSector를 기준으로 주변에 있는 8개의 섹터들을 가져온다.
//			2. pSector를 포함한 주변 8개 섹터에 pvPacket을 보낸다.
//	Arguments
//		- pvPacket : 보낼 패킷
//		- nLength : 패킷 길이
//		- pcsCell : 기준 셀
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmAOIFilter::SendPacketNear(PVOID pvPacket, INT16 nLength, AgsmAOICell *pcsCell, INT32 lRegionIndex, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketNear");

	if (!pvPacket || nLength < 1 || !pcsCell)
	{
//		TRACEFILE("AgsmAOIFilter : SendPacketNear() failed (in SendPacket())");

		return FALSE;
	}

	//STOPWATCH2(GetModuleName(), _T("SendPacketNear"));

	AgsmAOICell		*pcsAroundCell	= NULL;
	
	INT32	lIndexX	= pcsCell->GetIndexX();
	INT32	lIndexZ	= pcsCell->GetIndexZ();

	// 자신과 주변에 있는 거리 2까지의 셀들을 가져와 차례로 섹터별로 데이타를 전송한다.
	//
	// 2006.07.04. steeple
	// 패킷 보내야할 주위의 셀 개수를 구하는 함수를 만들었다.
	INT32	lNearCellCount = GetNearCellCountByRegion(lRegionIndex);
	for (int i = lIndexX - lNearCellCount; i <= lIndexX + lNearCellCount; ++i)
	{
		for (int j = lIndexZ - lNearCellCount; j <= lIndexZ + lNearCellCount; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			if (pcsAroundCell->GetGroupID() == 0)
				continue;

			INT32	lDistance	= abs(i - lIndexX) + abs(j - lIndexZ);

			INT32	lPriority	= ePriority + lDistance;

			if ((PACKET_PRIORITY) lPriority >= PACKET_PRIORITY_MAX)
				lPriority	= PACKET_PRIORITY_MAX - 1;

			// lDistance 에 따라... packet priority를 준다.

			// 0이면 가장 높은놈.. 4면 젤 낮은놈..

			if (!SendPacketGroup(pvPacket, nLength, pcsAroundCell->GetGroupID(), (PACKET_PRIORITY) lPriority))
			{
//				TRACEFILE("AgsmAOIFilter : SendPacketGroup failed (SendPacket() return FALSE)");

				continue;
			}
		}
	}

	return TRUE;
}

BOOL AgsmAOIFilter::SendPacketNearRace(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, UINT32 lRace)
{
	if (!pvPacket || nLength < 1)
		return FALSE;

	AgsmAOICell		*pcsCell		= m_csAOIMap.GetCell(&posCurrent);
	INT32			lRegionIndex	= m_papmMap->GetRegion(posCurrent.x, posCurrent.z);
	AgsmAOICell		*pcsAroundCell	= NULL;
	
	if(AuPacket::GetCryptManager().IsUseCrypt())
		pvPacket = EncryptGroupPacket(pvPacket, &nLength);

	INT32	lIndexX	= pcsCell->GetIndexX();
	INT32	lIndexZ	= pcsCell->GetIndexZ();

	INT32	lNearCellCount = GetNearCellCountByRegion(lRegionIndex);
	for (int i = lIndexX - lNearCellCount; i <= lIndexX + lNearCellCount; ++i)
	{
		for (int j = lIndexZ - lNearCellCount; j <= lIndexZ + lNearCellCount; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell || !pcsAroundCell->GetGroupID())
				continue;

			UINT32	aulNIDBuffer[1024] = { 0, };
			INT32	lCount	= 0;

			pcsAroundCell->m_csRWLock.LockReader();

			for (AgsmCellUnit *pstCellUnit = pcsAroundCell->GetCharacterUnit(); pstCellUnit; pstCellUnit = pcsAroundCell->GetNext(pstCellUnit))
			{
				if(!pstCellUnit) continue;
				if(pstCellUnit->ulNID <= 0) continue;
				
				AgpdCharacter* pcsCharacter = (AgpdCharacter*)pstCellUnit->lID;
				if(!pcsCharacter) continue;
				if(lRace != m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor)) continue;
				
				aulNIDBuffer[lCount++]	= pstCellUnit->ulNID;

				if (lCount >= 1024)
					break;
			}

			pcsAroundCell->m_csRWLock.UnlockReader();
			
			for (int i = 0; i < lCount; ++i)
			{
				SendPacket(pvPacket, nLength, aulNIDBuffer[i]);
			}
		}
	}

	return TRUE;
}

BOOL AgsmAOIFilter::SendPacketNearExceptSelf(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, INT32 lRegionIndex, UINT32 ulSelfNID, PACKET_PRIORITY ePriority)
{
	if (!pvPacket || !nLength)
		return FALSE;

	/*
	ApWorldSector *pSector = m_papmMap->GetSector(posCurrent);

	return SendPacketNearExceptSelf(pvPacket, nLength, pSector, ulSelfNID);
	*/

	AgsmAOICell		*pcsCell		= m_csAOIMap.GetCell(&posCurrent);

	if(lRegionIndex < 0)
		lRegionIndex = m_papmMap->GetRegion(posCurrent.x, posCurrent.z);

	return SendPacketNearExceptSelf(pvPacket, nLength, pcsCell, lRegionIndex, ulSelfNID, ePriority);
}

BOOL AgsmAOIFilter::SendPacketNearExceptSelf(PVOID pvPacket, INT16 nLength, AgsmAOICell *pcsCell, INT32 lRegionIndex, UINT32 ulSelfNID, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketNearExceptSelf");

	if (!pvPacket || nLength < 1 || !pcsCell)
	{
//		TRACEFILE("AgsmAOIFilter : SendPacketNear() failed (in SendPacket())");

		return FALSE;
	}

	//STOPWATCH2(GetModuleName(), _T("SendPacketNEarExceptSelf"));

	AgsmAOICell		*pcsAroundCell	= NULL;

	INT32	lIndexX	= pcsCell->GetIndexX();
	INT32	lIndexZ	= pcsCell->GetIndexZ();

	// 자신과 주변에 있는 거리 2까지의 셀들을 가져와 차례로 섹터별로 데이타를 전송한다.
	//
	// 2006.07.04. steeple
	// 패킷 보내야할 주위의 셀 개수를 구하는 함수를 만들었다.
	INT32	lNearCellCount = GetNearCellCountByRegion(lRegionIndex);
	for (int i = lIndexX - lNearCellCount; i <= lIndexX + lNearCellCount; ++i)
	{
		for (int j = lIndexZ - lNearCellCount; j <= lIndexZ + lNearCellCount; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			if (pcsAroundCell->GetGroupID() == 0)
				continue;

			INT32	lDistance	= abs(i - lIndexX) + abs(j - lIndexZ);

			INT32	lPriority	= ePriority + lDistance;

			if ((PACKET_PRIORITY) lPriority >= PACKET_PRIORITY_MAX)
				lPriority	= PACKET_PRIORITY_MAX - 1;

			if (!SendPacketGroupExceptSelf(pvPacket, nLength, pcsAroundCell->GetGroupID(), ulSelfNID, (PACKET_PRIORITY) lPriority))
			{
//				TRACEFILE("AgsmAOIFilter : SendPacketGroup failed (SendPacket() return FALSE)");

				continue;
			}
		}
	}

	return TRUE;
}

BOOL AgsmAOIFilter::SendPacketNearExceptNIDs(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, INT32 lRegionIndex, UINT32 *pulExceptNID, INT32 nNumNIDs, PACKET_PRIORITY ePriority)
{
	if (!pvPacket || !nLength)
		return FALSE;

	/*
	ApWorldSector *pSector = m_papmMap->GetSector(posCurrent);

	return SendPacketNearExceptNIDs(pvPacket, nLength, pSector, pulExceptNID, nNumNIDs);
	*/

	AgsmAOICell		*pcsCell			= m_csAOIMap.GetCell(&posCurrent);
	
	if(lRegionIndex < 0)
		lRegionIndex = m_papmMap->GetRegion(posCurrent.x, posCurrent.z);

	return SendPacketNearExceptNIDs(pvPacket, nLength, pcsCell, lRegionIndex, pulExceptNID, nNumNIDs, ePriority);
}

BOOL AgsmAOIFilter::SendPacketNearExceptNIDs(PVOID pvPacket, INT16 nLength, AgsmAOICell *pcsCell, INT32 lRegionIndex, UINT32 *pulExceptNID, INT32 nNumNIDs, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketNearExceptNIDs");

	if (!pvPacket || nLength < 1 || !pcsCell)
	{
//		TRACEFILE("AgsmAOIFilter : SendPacketNear() failed (in SendPacket())");

		return FALSE;
	}

	//STOPWATCH2(GetModuleName(), _T("SendPacketNearExceptNIDs"));

	AgsmAOICell		*pcsAroundCell	= NULL;

	INT32	lIndexX	= pcsCell->GetIndexX();
	INT32	lIndexZ	= pcsCell->GetIndexZ();

	// 자신과 주변에 있는 거리 2까지의 셀들을 가져와 차례로 섹터별로 데이타를 전송한다.
	//
	// 2006.07.04. steeple
	// 패킷 보내야할 주위의 셀 개수를 구하는 함수를 만들었다.
	INT32	lNearCellCount = GetNearCellCountByRegion(lRegionIndex);
	for (int i = lIndexX - lNearCellCount; i <= lIndexX + lNearCellCount; ++i)
	{
		for (int j = lIndexZ - lNearCellCount; j <= lIndexZ + lNearCellCount; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			if (pcsAroundCell->GetGroupID() == 0)
				continue;

			INT32	lDistance	= abs(i - lIndexX) + abs(j - lIndexZ);

			INT32	lPriority	= ePriority + lDistance;

			if ((PACKET_PRIORITY) lPriority >= PACKET_PRIORITY_MAX)
				lPriority	= PACKET_PRIORITY_MAX - 1;

			if (!SendPacketGroupExceptNIDs(pvPacket, nLength, pcsAroundCell->GetGroupID(), pulExceptNID, nNumNIDs, (PACKET_PRIORITY) lPriority))
			{
//				TRACEFILE("AgsmAOIFilter : SendPacketGroup failed (SendPacket() return FALSE)");

				continue;
			}
		}
	}

	return TRUE;
}

BOOL AgsmAOIFilter::SendPacketGroup(PVOID pvPacket, INT16 nLength, UINT_PTR ulGroupNID, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketGroup");

	if (!pvPacket || nLength < 1 || ulGroupNID == 0)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SendPacketGroup"));

	BOOL bEncrypted = FALSE;

	AgsmAOICell	*pstCell	= (AgsmAOICell *)	ulGroupNID;

	UINT32	aulNIDBuffer[1024];
	ZeroMemory(aulNIDBuffer, sizeof(aulNIDBuffer));

	INT32	lCount	= 0;

	pstCell->m_csRWLock.LockReader();

	for (AgsmCellUnit *pstCellUnit = pstCell->GetCharacterUnit(); pstCellUnit; pstCellUnit = pstCell->GetNext(pstCellUnit))
	{
		if (pstCellUnit->ulNID > 0)
			aulNIDBuffer[lCount++]	= pstCellUnit->ulNID;

		if (lCount >= 1024)
			break;
	}

	pstCell->m_csRWLock.UnlockReader();

	for (int i = 0; i < lCount; ++i)
	{
		// 2006.04.15. steeple
		// 암호화를 해야하는 데 아직 안했다면 해준다.
		if(!bEncrypted && AuPacket::GetCryptManager().IsUseCrypt())
		{
			// 암호화를 해야한다면, 암호화를 하고 나서 Send 한다.
			// 그래야 각 소켓 별로 따로 암호화 하는 오버헤드를 없앨 수 있다.
			pvPacket = EncryptGroupPacket(pvPacket, &nLength);
			bEncrypted = TRUE;

			// 암호화된 pvPacket 의 Validation 체크는 SendPacket 에 맡긴다.
		}

		SendPacket(pvPacket, nLength, aulNIDBuffer[i], ePriority);
	}

	return TRUE;
}

BOOL AgsmAOIFilter::SendPacketGroupExceptSelf(PVOID pvPacket, INT16 nLength, UINT_PTR ulGroupNID, UINT32 ulSelfNID, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketGroupExceptSelf");

	if (!pvPacket || nLength < 1 || ulGroupNID == 0)
		return FALSE;

	if (ulSelfNID == 0)
		return SendPacketGroup(pvPacket, nLength, ulGroupNID, ePriority);

	//STOPWATCH2(GetModuleName(), _T("SendPacketGroupExceptSelf"));

	BOOL bEncrypted = FALSE;

	AgsmAOICell	*pstCell	= (AgsmAOICell *)	ulGroupNID;

	UINT32	aulNIDBuffer[1024];
	ZeroMemory(aulNIDBuffer, sizeof(aulNIDBuffer));

	INT32	lCount	= 0;

	pstCell->m_csRWLock.LockReader();

	for (AgsmCellUnit *pstCellUnit = pstCell->GetCharacterUnit(); pstCellUnit; pstCellUnit = pstCell->GetNext(pstCellUnit))
	{
		if (pstCellUnit->ulNID > 0 && pstCellUnit->ulNID != ulSelfNID)
			aulNIDBuffer[lCount++]	= pstCellUnit->ulNID;

		if (lCount >= 1024)
			break;
	}

	pstCell->m_csRWLock.UnlockReader();

	for (int i = 0; i < lCount; ++i)
	{
		// 2006.04.15. steeple
		// 암호화를 해야하는 데 아직 안했다면 해준다.
		if(!bEncrypted && AuPacket::GetCryptManager().IsUseCrypt())
		{
			// 암호화를 해야한다면, 암호화를 하고 나서 Send 한다.
			// 그래야 각 소켓 별로 따로 암호화 하는 오버헤드를 없앨 수 있다.
			pvPacket = EncryptGroupPacket(pvPacket, &nLength);
			bEncrypted = TRUE;

			// 암호화된 pvPacket 의 Validation 체크는 SendPacket 에 맡긴다.
		}

		SendPacket(pvPacket, nLength, aulNIDBuffer[i], ePriority);
	}

	return TRUE;
}

BOOL AgsmAOIFilter::SendPacketGroupExceptNIDs(PVOID pvPacket, INT16 nLength, UINT_PTR ulGroupNID, UINT32 *pulExceptNID, INT32 nNumNIDs, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketGroupExceptNIDs");

	if (!pvPacket || nLength < 1 || ulGroupNID == 0)
		return FALSE;

	if (!pulExceptNID || nNumNIDs < 1)
		return SendPacketGroup(pvPacket, nLength, ulGroupNID, ePriority);

	//STOPWATCH2(GetModuleName(), _T("SendPacketGroupExceptNIDs"));

	AgsmAOICell	*pstCell	= (AgsmAOICell *)	ulGroupNID;

	BOOL	bSendPacket	= TRUE;
	BOOL	bEncrypted	= FALSE;

	UINT32	aulNIDBuffer[1024];
	ZeroMemory(aulNIDBuffer, sizeof(aulNIDBuffer));

	INT32	lCount	= 0;

	pstCell->m_csRWLock.LockReader();

	for (AgsmCellUnit *pstCellUnit = pstCell->GetCharacterUnit(); pstCellUnit; pstCellUnit = pstCell->GetNext(pstCellUnit))
	{
		bSendPacket	= TRUE;

		for (int i = 0; i < nNumNIDs; ++i)
		{
			if (pstCellUnit->ulNID == 0 || pstCellUnit->ulNID == pulExceptNID[i])
			{
				bSendPacket	= FALSE;
				break;
			}
		}

		if (bSendPacket)
			aulNIDBuffer[lCount++]	= pstCellUnit->ulNID;

		if (lCount >= 1024)
			break;
	}

	pstCell->m_csRWLock.UnlockReader();

	for (int i = 0; i < lCount; ++i)
	{
		// 2006.04.15. steeple
		// 암호화를 해야하는 데 아직 안했다면 해준다.
		if(!bEncrypted && AuPacket::GetCryptManager().IsUseCrypt())
		{
			// 암호화를 해야한다면, 암호화를 하고 나서 Send 한다.
			// 그래야 각 소켓 별로 따로 암호화 하는 오버헤드를 없앨 수 있다.
			pvPacket = EncryptGroupPacket(pvPacket, &nLength);
			bEncrypted = TRUE;

			// 암호화된 pvPacket 의 Validation 체크는 SendPacket 에 맡긴다.
		}

		SendPacket(pvPacket, nLength, aulNIDBuffer[i], ePriority);
	}

	return TRUE;
}

// 2004.01.28. steeple
// Send Packet To All Groups (모든 접속자에게 보낸다. 접속자가 많을땐 매우 우울하니 주의해서 사용한다. -0-)
BOOL AgsmAOIFilter::SendPacketAllGroups(PVOID pvPacket, INT16 nLength, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmAOIFilter::SendPacketAllGroups");

	INT32 lIndex = 0;
	UINT32* pulNID = NULL;
	BOOL bEncrypted = FALSE;

	//STOPWATCH2(GetModuleName(), _T("SendPacketAllGroups"));

	pulNID = (UINT32*) m_csAdminAllGroup.GetObjectSequence(&lIndex);
	while(pulNID && *pulNID != 0)
	{
		// 2006.04.15. steeple
		// 암호화를 해야하는 데 아직 안했다면 해준다.
		if(!bEncrypted && AuPacket::GetCryptManager().IsUseCrypt())
		{
			// 암호화를 해야한다면, 암호화를 하고 나서 Send 한다.
			// 그래야 각 소켓 별로 따로 암호화 하는 오버헤드를 없앨 수 있다.
			pvPacket = EncryptGroupPacket(pvPacket, &nLength);
			bEncrypted = TRUE;

			// 암호화된 pvPacket 의 Validation 체크는 SendPacket 에 맡긴다.
		}

		SendPacket(pvPacket, nLength, *pulNID, ePriority);

		pulNID = (UINT32*) m_csAdminAllGroup.GetObjectSequence(&lIndex);
	}

	return TRUE;
}

AgsmCellUnit *AgsmAOIFilter::GetADCharacter(PVOID pData)
{
	if (!pData)
		return NULL;

	return (AgsmCellUnit *) m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, pData);
}

AgsmCellUnit *AgsmAOIFilter::GetADItem(PVOID pData)
{
	if (!pData)
		return NULL;

	return (AgsmCellUnit *) m_pagpmItem->GetAttachedModuleData(m_nIndexADItem, pData);
}

BOOL AgsmAOIFilter::SetCallbackAddChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_ADD_CHAR, pfCallback, pClass);
}

BOOL AgsmAOIFilter::SetCallbackRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_REMOVE_CHAR, pfCallback, pClass);
}

BOOL AgsmAOIFilter::SetCallbackMoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_MOVE_CHAR, pfCallback, pClass);
}

BOOL AgsmAOIFilter::SetCallbackAddItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_ADD_ITEM, pfCallback, pClass);
}

BOOL AgsmAOIFilter::SetCallbackRemoveItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_REMOVE_ITEM, pfCallback, pClass);
}

BOOL AgsmAOIFilter::SetCallbackCheckControlServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_CHECK_CONTROLSERVER, pfCallback, pClass);
}

BOOL AgsmAOIFilter::SetCallbackGetDPNID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAOI_CB_ID_GET_DPNID, pfCallback, pClass);
}

// 2004.03.24. steeple. - 전체 유저를 관리하는 그룹에서 Object 수를 리턴한다.
INT32 AgsmAOIFilter::GetPlayerCountFromAllPlayerGroup()
{
	return m_csAdminAllGroup.GetObjectCount();
}

INT32 AgsmAOIFilter::GetNumAroundCharacter(AgsmAOICell *pcsCell, INT32 lRegionIndex)
{
	if (!pcsCell)
		return 0;

	INT32		lNumCharacter	= 0;

	AgsmAOICell	*pcsAroundCell	= NULL;

	// 2006.07.04. steeple
	// 패킷 보내야할 주위의 셀 개수를 구하는 함수를 만들었다.
	INT32	lNearCellCount = GetNearCellCountByRegion(lRegionIndex);
	for (int i = (int) pcsCell->GetIndexX() - lNearCellCount; i <= (int) pcsCell->GetIndexX() + lNearCellCount; ++i)
	{
		for (int j = (int) pcsCell->GetIndexZ() - lNearCellCount; j <= (int) pcsCell->GetIndexZ() + lNearCellCount; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			lNumCharacter	+= pcsAroundCell->GetNumCharacterUnit();
		}
	}

	return lNumCharacter;
}

inline PVOID AgsmAOIFilter::EncryptGroupPacket(PVOID pvPacket, INT16* pnSize)
{
	if(!pvPacket || !pnSize)
		return NULL;

	INT16 nOutputSize = 0;
	PVOID pOutput = AuPacket::EncryptPacket(pvPacket, *pnSize, &nOutputSize, m_csCryptActor, AUCRYPT_TYPE_PUBLIC);

	if(!pOutput || nOutputSize < 1)
		return NULL;

	*pnSize = nOutputSize;
	return pOutput;
}

VOID AgsmAOIFilter::CheckValidLock(ApMutualEx *pMutex)
{
	if (!pMutex)
		return;

	// 체크 하는 부분에 구멍이 있긴 있다. 하지만 모든 경우에 이 구멍을 통과할 가능성 또한 거의 없다.
	// 열번중 한번만 걸려도 된다.

	if (pMutex->GetOwningThreadID() == 0 ||
		::GetCurrentThreadId() != pMutex->GetOwningThreadID())
	{
		// 락을 안하고 들어왔다. 뭐 이런게 다있노...
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "OwningThreadID %d, IsInit %d, IsRemoveLock %d\n", pMutex->GetOwningThreadID(), pMutex->IsInit(), pMutex->IsRemoveLock());
		AuLogFile_s("LOG\\AOIFilterLock.txt", strCharBuff);

		//JK_크래쉬....일단 이건 주석 처리..
		/*
		if (g_eServiceArea == AP_SERVICE_AREA_KOREA)
		{
		CHAR	*pszCrash	= NULL;
		*pszCrash	= 0;
		}
		*/
		
	}

	return;
}

// 2006.07.04. steeple
// 셀에 들어왔을 때, 몇개의 셀까지 시야가 확보되는 지 개수를 리턴한다.
// 아무리 작아도 default 값이 리턴된다.
INT32 AgsmAOIFilter::GetNearCellCountByRegion(INT32 lRegionIndex)
{
	if(lRegionIndex < 0)
		return AGSMAOI_DEFAULT_NEAR_CELL_COUNT;

	ApmMap::RegionTemplate* pcsRegionTemplate = m_papmMap->GetTemplate(lRegionIndex);
	if(!pcsRegionTemplate || pcsRegionTemplate->fVisibleDistance == 0.0f)
		return AGSMAOI_DEFAULT_NEAR_CELL_COUNT;

	double dCount = (double)(pcsRegionTemplate->fVisibleDistance / AGSMAOI_DEFAULT_VISIBLE_DISTANCE_PER_CELL);
	if(dCount > (double)AGSMAOI_DEFAULT_NEAR_CELL_COUNT)
		return (INT32)(dCount - 1);
	else
		return AGSMAOI_DEFAULT_NEAR_CELL_COUNT;
}