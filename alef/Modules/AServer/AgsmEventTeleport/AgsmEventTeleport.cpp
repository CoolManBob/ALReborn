// AgsmEventTeleport.cpp: implementation of the AgsmEventTeleport class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmEventTeleport.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmEventTeleport::AgsmEventTeleport()
{
	SetModuleName("AgsmEventTeleport");

	SetModuleType(APMODULE_TYPE_SERVER);
}

AgsmEventTeleport::~AgsmEventTeleport()
{

}

BOOL	AgsmEventTeleport::OnAddModule()
{
	// 상위 Module들 가져온다.
	m_pcsApmMap = (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmSummons = (AgpmSummons *) GetModule("AgpmSummons");
	m_pcsAgsmAOIFilter = (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pcsAgpmEventTeleport = (AgpmEventTeleport *) GetModule("AgpmEventTeleport");
	m_pcsAgpmEventBinding = (AgpmEventBinding *) GetModule("AgpmEventBinding");
	m_pcsAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pcsAgsmItem = (AgsmItem *) GetModule("AgsmItem");
	m_pcsAgsmDeath = (AgsmDeath *) GetModule("AgsmDeath");
	m_pcsAgsmParty = (AgsmParty *) GetModule("AgsmParty");
	m_pcsAgpmLog = (AgpmLog *) GetModule("AgpmLog");

	if (!m_pcsApmMap || !m_pcsAgpmCharacter ||
		!m_pcsAgpmSummons ||
		!m_pcsAgsmAOIFilter || !m_pcsAgpmEventTeleport || !m_pcsAgpmEventBinding || !m_pcsAgsmCharacter ||
		!m_pcsAgsmItem || !m_pcsAgsmDeath || !m_pcsAgsmParty)
		return FALSE;

	/*
	// Callback을 Set한다.
	if (!m_pcsAgpmEventTeleport->SetCallbackTeleport(CBTeleport, this))
		return FALSE;
	*/

	if (!m_pcsAgpmEventTeleport->SetCallbackCalcPos(CBCalcPos, this))
		return FALSE;
	
	if (!m_pcsAgpmEventTeleport->SetCallbackReturnTown(CBReturnTown, this))
		return FALSE;

	if (!m_pcsAgpmEventTeleport->SetCallbackTeleportLoading(CBReceiveTeleportLoading, this))
		return FALSE;

	if (!m_pcsAgpmEventTeleport->SetCallbackSyncTeleportLoading(CBSyncTeleportLoading, this))
		return FALSE;
		
	if (!m_pcsAgpmEventTeleport->SetCallbackTeleportLog(CBTeleportLog, this))
		return FALSE;

	if (!m_pcsAgpmEventTeleport->SetCallbackBattleGroundTeleportLog(CBBattleGroundTeleportLog, this))
		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackUseTeleportScroll(CBSyncTeleportLoading, this))
		return FALSE;

	if (!m_pcsAgsmDeath->SetCallbackResurrection(CBSyncTeleportLoading, this))
		return FALSE;

	if (!m_pcsAgsmParty->SetCallbackUseRecall(CBSyncTeleportLoading, this))
		return FALSE;

	return TRUE;
}

/*
BOOL	AgsmEventTeleport::CBTeleport(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmEventTeleport *	pThis = (AgsmEventTeleport *) pClass;
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pCustData;
	ApdEvent *			pstEvent = (ApdEvent *) pData;
	PVOID				pvPacket;
	INT16				nSize;

	if (pstEvent->m_lEID & APDEVENT_EID_FLAG_LOCAL)
	{
		pvPacket = pThis->m_pcsAgpmEventTeleport->m_csPacket.MakePacket(TRUE, &nSize, AGPMEVENT_TELEPORT_PACKET_TYPE,
							&pstEvent->m_pcsSource->m_eType,
							&pstEvent->m_pcsSource->m_lID,
							NULL,
							&pcsCharacter->m_lID,
							NULL,
							&pcsCharacter->m_stPos);
	}
	else
	{
		pvPacket = pThis->m_pcsAgpmEventTeleport->m_csPacket.MakePacket(TRUE, &nSize, AGPMEVENT_TELEPORT_PACKET_TYPE, 
							NULL,
							NULL,
							&pstEvent->m_lEID,
							&pcsCharacter->m_lID,
							NULL,
							&pcsCharacter->m_stPos);
	}

	if (!pThis->m_pcsAgsmAOIFilter->SendPacketNear(pvPacket, nSize, pcsCharacter->m_stPos))
	{
		OutputDebugString("AgsmEventTeleport::CBTeleport() Error (1) !!!\n");
		pThis->m_pcsAgpmEventTeleport->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	pThis->m_pcsAgpmEventTeleport->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}
*/

BOOL	AgsmEventTeleport::CBCalcPos(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmEventTeleport *		pThis = (AgsmEventTeleport *) pClass;
	AgpdTeleportPoint *		pstPoint = (AgpdTeleportPoint *) pData;
	AuPOS *					pstPos = (AuPOS *) pCustData;
	FLOAT					fRadiusMin = 0.0, fRadiusMax = 0.0;
	FLOAT					fAngle;

	// 임시로 TRUE로 바꾼다.
	if (!pstPoint)
		return TRUE;

	if (!pThis->m_pcsAgpmEventTeleport->GetTargetPoint(pstPoint, pstPos, &fRadiusMin, &fRadiusMax))
		return FALSE;

	fRadiusMax = (FLOAT) (rand() / (RAND_MAX + 0.0) * fRadiusMax);
	fAngle = (FLOAT) (rand() / (RAND_MAX + 0.0) * AGSMTELEPORT_PI * 2);

	pstPos->x += fRadiusMax * (FLOAT) sin(fAngle);
	pstPos->z += fRadiusMax * (FLOAT) cos(fAngle);

	return TRUE;
}

BOOL	AgsmEventTeleport::CBReturnTown(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmEventTeleport	*pThis			= (AgsmEventTeleport *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	AuPOS				stTeleportPos;
	if (pThis->m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stTeleportPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if (pcsCharacter->m_bMove)
			pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		pThis->SetStartTeleport(pcsCharacter, &stTeleportPos);
		//pThis->m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, &stTeleportPos, FALSE, TRUE);
	}

	return TRUE;
}

BOOL	AgsmEventTeleport::CBReceiveTeleportLoading(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmEventTeleport	*pThis	= (AgsmEventTeleport *)	pClass;
	INT32				lCID	= *(INT32 *)	pData;

	AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	pThis->EndTeleport(pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL	AgsmEventTeleport::CBSyncTeleportLoading(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventTeleport	*pThis	= (AgsmEventTeleport *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;
	AuPOS				*pstTargetPos	= (AuPOS *)	pCustData;

	pThis->SetStartTeleport(pcsCharacter, pstTargetPos);

	return TRUE;
}

BOOL	AgsmEventTeleport::CBTeleportLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;
		
	AgsmEventTeleport	*pThis	= (AgsmEventTeleport *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID				*ppvBuffer = (PVOID *) pCustData;

	CHAR				*pszSrc		= (CHAR *) ppvBuffer[0];
	CHAR				*pszDest	= (CHAR *) ppvBuffer[1];
	INT32				lFee		= *((INT32 *) ppvBuffer[2]);
	
	return pThis->WriteTeleportLog(pcsCharacter, pszSrc, pszDest, lFee);
}

BOOL	AgsmEventTeleport::CBBattleGroundTeleportLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventTeleport	*pThis	= (AgsmEventTeleport *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID				*ppvBuffer = (PVOID *) pCustData;

	CHAR				*pszSrc		= (CHAR *) ppvBuffer[0];
	CHAR				*pszDest	= (CHAR *) ppvBuffer[1];
	INT32				lFee		= *((INT32 *) ppvBuffer[2]);

	return pThis->WriteBattleGroundTeleportLog(pcsCharacter, pszSrc, pszDest, lFee);
}

BOOL AgsmEventTeleport::SetStartTeleport(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos)
{
	if (!pcsCharacter || !pstTargetPos)
		return FALSE;

	if (!pcsCharacter->m_bIsAddMap)
		return FALSE;

	AgsmCellUnit		*pstCellUnit		= m_pcsAgsmAOIFilter->GetADCharacter(pcsCharacter);
	if (!pstCellUnit)
		return FALSE;

	AgsmAOICell			*pcsCell			= pstCellUnit->pcsCurrentCell;
	if (!pcsCell)
		return FALSE;

	// 거리가 시야거리 이상 떨어져 있어야 한다
	AgsmAOICell			*pcsTargetCall		= m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(pstTargetPos);
	if (!pcsTargetCall)
		return FALSE;

	INT32	lSubX	= abs(pcsCell->GetIndexX() - pcsTargetCall->GetIndexX());
	INT32	lSubZ	= abs(pcsCell->GetIndexZ() - pcsTargetCall->GetIndexZ());

	if (lSubX > 4 || lSubZ > 4)
	{
		if (pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		AgsdCharacter	*pcsAgsdCharacter	= m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

		pcsAgsdCharacter->m_bIsNotLogout	= TRUE;

		if (!m_pcsAgpmCharacter->RemoveCharacterFromMap(pcsCharacter, TRUE))
			return FALSE;

		INT32	lRegionIndex	= m_pcsAgpmCharacter->GetRealRegionIndex(pcsCharacter);

		// 2006.07.05. steeple
		// 패킷 보내야할 주위의 셀 개수를 구하는 함수를 만들었다.
		INT32	lNearCellCount = m_pcsAgsmAOIFilter->GetNearCellCountByRegion(lRegionIndex);

		// 자신과 주변에 있는 8개의 섹터를 가져와 차례로 섹터별로 데이타를 전송한다.
		for (int i = (int) pcsCell->GetIndexX() - lNearCellCount; i <= (int) pcsCell->GetIndexX() + lNearCellCount; i++)
		{
			for (int j = (int) pcsCell->GetIndexZ() - lNearCellCount; j <= (int) pcsCell->GetIndexZ() + lNearCellCount; j++)
			{
				m_pcsAgsmCharacter->SendCellRemoveToChar(m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(i, j), pcsCharacter);
			}
		}

		INT8 cMoveFlag = AGPMCHARACTER_MOVE_FLAG_SYNC;
		INT16	nPacketLength	= 0;

		PVOID pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nPacketLength, 0,
																	pstTargetPos,
																	pstTargetPos,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	&cMoveFlag,
																	NULL);

		if (!pvPacketMove)
			return FALSE;

		INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
		PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																	&cOperation,							// Operation
																	&pcsCharacter->m_lID,					// Character ID
																	NULL,									// Character Template ID
																	NULL,									// Game ID
																	NULL,									// Character Status
																	pvPacketMove,							// Move Packet
																	NULL,									// Action Packet
																	NULL,									// Factor Packet
																	NULL,									// llMoney
																	NULL,									// bank money
																	NULL,									// cash
																	NULL,									// character action status
																	NULL,									// character criminal status
																	NULL,									// attacker id (정당방위 설정에 필요)
																	NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																	NULL,									// region index
																	NULL,									// social action index
																	NULL,									// special status
																	NULL,									// is transform status
																	NULL,									// skill initialization text
																	NULL,									// face index
																	NULL,									// hair index
																	NULL,									// Option Flag
																	NULL,									// bank size
																	NULL,									// event status flag
																	NULL,									// remained criminal status time
																	NULL,									// remained murderer point time
																	NULL,									// nick name
																	NULL,									// gameguard
																	NULL									// last killed time in battlesquare
																	);

		m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);

		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if (!SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_5))
		{
			OutputDebugString("AgsmEventTeleport::SetStartTeleport() Error (2) !!!\n");
			m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}

		m_pcsAgpmEventTeleport->SendTeleportLoading(pcsCharacter->m_lID, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

		pcsCharacter->m_stPos	= *pstTargetPos;
	}
	else
	{
		// 기본 무적 걸어준다. 2006.01.09. steeple
		// 텔레포트 시작하는순간 무적걸도록 바꿈. 2011-09-01. silvermoo 
		// m_pcsAgsmCharacter->SetDefaultInvincible(pcsCharacter);

		m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, pstTargetPos, FALSE, TRUE);
	}

	return TRUE;
}

BOOL AgsmEventTeleport::EndTeleport(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_bIsAddMap)
		return TRUE;

	// 2006.01.09. steeple
	// 기본 무적 걸어준다.
	// 텔레포트 시작하는순간 무적걸도록 바꿈. 2011-09-01. silvermoo 
	// m_pcsAgsmCharacter->SetDefaultInvincible(pcsCharacter);

	m_pcsAgpmCharacter->AddCharacterToMap(pcsCharacter, TRUE);

	AgsdCharacter	*pcsAgsdCharacter	= m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	pcsAgsdCharacter->m_bIsNotLogout	= FALSE;

	// 2006.11.22. steeple
	// 함수로 변경했다.
	m_pcsAgpmSummons->UpdateAllSummonsPosToOwner(pcsCharacter);

	return TRUE;
}

BOOL AgsmEventTeleport::WriteTeleportLog(AgpdCharacter *pcsCharacter, CHAR *pszSrc, CHAR *pszDest, INT32 lFee)
{
	if (!m_pcsAgpmLog || !pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);
	
	return m_pcsAgpmLog->WriteLog_Teleport(0,
								&pAgsdCharacter->m_strIPAddress[0],
								pAgsdCharacter->m_szAccountID,
								pAgsdCharacter->m_szServerName,
								pcsCharacter->m_szID,
								lCharTID,
								lLevel,
								llExp,
								pcsCharacter->m_llMoney,
								pcsCharacter->m_llBankMoney,
								pszSrc ? pszSrc : "",
								pszDest ? pszDest : "",
								lFee
								);

}

BOOL AgsmEventTeleport::WriteBattleGroundTeleportLog(AgpdCharacter *pcsCharacter, CHAR *pszSrc, CHAR *pszDest, INT32 lFee)
{
	if (!m_pcsAgpmLog || !pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);

	return m_pcsAgpmLog->WriteLog_BattleGroundTeleport(0,
		&pAgsdCharacter->m_strIPAddress[0],
		pAgsdCharacter->m_szAccountID,
		pAgsdCharacter->m_szServerName,
		pcsCharacter->m_szID,
		lCharTID,
		lLevel,
		llExp,
		pcsCharacter->m_llMoney,
		pcsCharacter->m_llBankMoney,
		pszSrc ? pszSrc : "",
		pszDest ? pszDest : "",
		lFee
		);

}

