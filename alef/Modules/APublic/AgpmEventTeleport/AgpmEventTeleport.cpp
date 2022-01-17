#include <stdio.h>
#include "AgpmEventTeleport.h"
#include "AgpmArchlord.h"
#include "AgpmBattleGround.h"
#include "ApmMap.h"

#include "ApMemoryTracker.h"

const INT32 ARCHON_ITEM = 4608;

AgpmEventTeleport::AgpmEventTeleport()
{
	SetModuleName("AgpmEventTeleport");

	SetModuleData(sizeof(AgpdTeleportPoint), AGPMEVENT_TELEPORT_DATA_TYPE_POINT);
	SetModuleData(sizeof(AgpdTeleportGroup), AGPMEVENT_TELEPORT_DATA_TYPE_GROUP);

	SetPacketType(AGPMEVENT_TELEPORT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,	1,		// operation typef
							AUTYPE_PACKET,	1,		// Event Base
							AUTYPE_INT32,	1,		// Character CID
							AUTYPE_CHAR,	AGPD_TELEPORT_MAX_POINT_NAME + 1,		// Target TeleportPoint nAME
							//AUTYPE_INT32, 1,		// Target Position Index (Local Attached Data)
							AUTYPE_POS,		1,		// Target Custom Position
							AUTYPE_MEMORY_BLOCK,	1,		// disable target point id (array)
							AUTYPE_END, 0
							);

	m_lMaxPointID		= 0;
	m_lMaxGroupID		= 0;
	m_bProtectPoint		= TRUE;
	m_nIndexADCharacter	= -1;
	m_pszFeeName		= NULL;
}

AgpmEventTeleport::~AgpmEventTeleport()
{
}

BOOL	AgpmEventTeleport::OnAddModule()
{
	// 상위 Module들 가져온다.
	m_pcsApmEventManager= (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmSiegeWar	= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");
	m_pcsAgpmArchlord	= (AgpmArchlord *) GetModule("AgpmArchlord");
	m_pcsAgpmItem		= (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmGrid		= (AgpmGrid *) GetModule("AgpmGrid");
	m_pcsAgpmGuild		= (AgpmGuild *) GetModule("AgpmGuild");
	m_pcsAgpmFactors	= (AgpmFactors*) GetModule("AgpmFactors");

	if( !m_pcsApmEventManager || 
		!m_pcsAgpmCharacter || 
		!m_pcsAgpmSiegeWar || 
		!m_pcsAgpmArchlord ||
		!m_pcsAgpmItem || 
		!m_pcsAgpmGrid || 
		!m_pcsAgpmGuild ||
		!m_pcsAgpmFactors)
		return FALSE;

	// Event Manager에 Event를 등록한다.
	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_TELEPORT, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	if (!AddStreamCallback(AGPMEVENT_TELEPORT_DATA_TYPE_GROUP, GroupReadCB, GroupWriteCB, this))
	{
		OutputDebugString("AgpmEventTeleport::OnAddModule() Error (4) !!!\n");
		return FALSE;
	}

	if (!AddStreamCallback(AGPMEVENT_TELEPORT_DATA_TYPE_POINT, PointReadCB, PointWriteCB, this))
	{
		OutputDebugString("AgpmEventTeleport::OnAddModule() Error (5) !!!\n");
		return FALSE;
	}

	if (m_pcsAgpmCharacter)
	{
		if (!m_pcsAgpmCharacter->SetCallbackActionEventTeleport(CBActionEventTeleport, this))
			return FALSE;

		m_nIndexADCharacter = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdTeleportAttachToChar), ConAgpdTeleportAttachToChar, DesAgpdTeleportAttachToChar);
		if (m_nIndexADCharacter < 0)
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmEventTeleport::OnInit()
{
	if (!m_csAdminPoint.InitializeObject(sizeof(AgpdTeleportPoint *) , m_csAdminPoint.GetCount()))
		return FALSE;

	if (!m_csAdminGroup.InitializeObject(sizeof(AgpdTeleportGroup *) , m_csAdminGroup.GetCount()))
		return FALSE;

	m_pcsAgpmBattleGround = (AgpmBattleGround*) GetModule("AgpmBattleGround");
	m_pApmMap			  = (ApmMap*) GetModule ("ApmMap");

	if(!m_pcsAgpmBattleGround || !m_pApmMap)
		return FALSE;

	return TRUE;
}

BOOL AgpmEventTeleport::OnDestroy()
{
	INT32	lIndex = 0;
	AgpdTeleportPoint **ppcsTeleportPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	while (ppcsTeleportPoint && *ppcsTeleportPoint)
	{
		RemoveTeleportPoint( *ppcsTeleportPoint, TRUE );

		ppcsTeleportPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	}

	lIndex = 0;
	AgpdTeleportGroup **ppcsTeleportGroup = (AgpdTeleportGroup **) m_csAdminGroup.GetObjectSequence(&lIndex);
	while (ppcsTeleportGroup && *ppcsTeleportGroup)
	{
		DestroyTeleportGroup( *ppcsTeleportGroup );

		ppcsTeleportGroup = (AgpdTeleportGroup **) m_csAdminGroup.GetObjectSequence(&lIndex);
	}

	RemoveFee();

	m_csAdminPoint.RemoveObjectAll();
	m_csAdminGroup.RemoveObjectAll();

	return TRUE;
}

BOOL AgpmEventTeleport::RemoveFee()
{
	if (m_pszFeeName)
	{
		for (INT32 lIndex = 0; lIndex < m_csAdminFee.GetCount(); ++lIndex)
		{
			if (m_pszFeeName[lIndex])
				delete [] m_pszFeeName[lIndex];
		}

		delete [] m_pszFeeName;

		m_pszFeeName = NULL;
	}

	m_csAdminFee.RemoveObjectAll();

	return TRUE;
}

BOOL AgpmEventTeleport::SetMaxTeleportPoint(INT16 nCount)
{
	return m_csAdminPoint.SetCount(nCount);
}

BOOL AgpmEventTeleport::SetMaxTeleportGroup(INT16 nCount)
{
	return m_csAdminGroup.SetCount(nCount);
}

AgpdTeleportPoint* AgpmEventTeleport::CreateTeleportPoint()
{
	AgpdTeleportPoint* pcsTeleportPoint = (AgpdTeleportPoint *) CreateModuleData(AGPMEVENT_TELEPORT_DATA_TYPE_POINT);
	if( !pcsTeleportPoint )		return NULL;

	ZeroMemory( pcsTeleportPoint->m_szPointName, sizeof(CHAR) * AGPD_TELEPORT_MAX_POINT_NAME + 1 );
	pcsTeleportPoint->m_eType			= AGPDTELEPORT_TARGET_TYPE_NONE;
	pcsTeleportPoint->m_fRadiusMin		= 0.0;
	pcsTeleportPoint->m_fRadiusMax		= 0.0;
	pcsTeleportPoint->m_eRegionType		= AGPDTELEPORT_REGION_TYPE_NORMAL;
	pcsTeleportPoint->m_eSpecialType	= AGPDTELEPORT_SPECIAL_TYPE_NORMAL;
	pcsTeleportPoint->m_bActive			= FALSE;
	pcsTeleportPoint->m_byUseType		= 0;

	return pcsTeleportPoint;
}

BOOL AgpmEventTeleport::DestroyTeleportPoint(AgpdTeleportPoint *pcsTeleportPoint)
{
	return pcsTeleportPoint ? DestroyModuleData( pcsTeleportPoint, AGPMEVENT_TELEPORT_DATA_TYPE_POINT ) : FALSE;
}

AgpdTeleportGroup* AgpmEventTeleport::CreateTeleportGroup()
{
	AgpdTeleportGroup* pcsTeleportGroup	= (AgpdTeleportGroup *) CreateModuleData(AGPMEVENT_TELEPORT_DATA_TYPE_GROUP);
	if ( pcsTeleportGroup )
		ZeroMemory( pcsTeleportGroup, sizeof(AgpdTeleportGroup) );

	return pcsTeleportGroup;
}

BOOL AgpmEventTeleport::DestroyTeleportGroup(AgpdTeleportGroup *pcsTeleportGroup)
{
	if( !pcsTeleportGroup )		return FALSE;

	pcsTeleportGroup->m_listPoint.RemoveAll();
	return	DestroyModuleData( pcsTeleportGroup, AGPMEVENT_TELEPORT_DATA_TYPE_GROUP );
}

AgpdTeleportPoint* AgpmEventTeleport::AddTeleportPoint(CHAR *szPointName, CHAR *szPointDescription)
{
	if( !szPointName || !strlen(szPointName) )		return NULL;

	AgpdTeleportPoint* pcsPoint = CreateTeleportPoint();
	if( !pcsPoint )			return NULL;

	pcsPoint->m_lID = ++m_lMaxPointID;

	if( !m_csAdminPoint.AddObject( &pcsPoint, pcsPoint->m_lID, szPointName ))
	{
		--m_lMaxPointID;
		DestroyTeleportPoint( pcsPoint );
		return NULL;
	}

	ZeroMemory(pcsPoint->m_szPointName, sizeof(CHAR) * (AGPD_TELEPORT_MAX_POINT_NAME + 1));
	strncpy(pcsPoint->m_szPointName, szPointName, AGPD_TELEPORT_MAX_POINT_NAME);

	if( szPointDescription && strlen(szPointDescription) )
	{
		ZeroMemory(pcsPoint->m_szDescription, sizeof(CHAR) * (AGPD_TELEPORT_MAX_POINT_DESCRIPTION + 1));
		strncpy(pcsPoint->m_szDescription, szPointDescription, AGPD_TELEPORT_MAX_POINT_DESCRIPTION);
	}

	return pcsPoint;
}

BOOL AgpmEventTeleport::RemoveTeleportPoint(AgpdTeleportPoint *pcsPoint, BOOL bForce)
{
	if( !pcsPoint )		return FALSE;
	if( pcsPoint->m_bAttachEvent && !bForce )	return FALSE;
		
	// pcsPoint가 속한 그룹에서 이 포인트에 대한 정보를 삭제한다.
	BOOL bResult = FALSE;
	INT_PTR lIndex = 0;
	for ( AgpdTeleportGroup* pcsGroup = GetSequenceGroupInPoint(pcsPoint, &lIndex); pcsGroup; pcsGroup = GetSequenceGroupInPoint(pcsPoint, &lIndex))
		bResult |= RemovePointNGroup(pcsGroup, pcsPoint);

	m_csAdminPoint.RemoveObject(pcsPoint->m_lID, pcsPoint->m_szPointName);

	if( pcsPoint->m_pstAttach )
	{
		pcsPoint->m_pstAttach->m_lPointID = 0;
		pcsPoint->m_pstAttach->m_pcsPoint = NULL;
	}
	pcsPoint->m_listTargetGroup.RemoveAll();

	bResult	|= DestroyTeleportPoint( pcsPoint );

	return bResult;
}

AgpdTeleportPoint* AgpmEventTeleport::GetTeleportPoint(CHAR *szPointName)
{
	AgpdTeleportPoint** ppcsPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObject(szPointName);
	return ppcsPoint ? *ppcsPoint : NULL;
}

AgpdTeleportPoint* AgpmEventTeleport::GetTeleportPoint(INT32 lID)
{
	AgpdTeleportPoint** ppcsPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObject(lID);
	return ppcsPoint ? *ppcsPoint : NULL;
}

BOOL AgpmEventTeleport::ChangeTeleportPointName(AgpdTeleportPoint *pcsPoint, CHAR *szName)
{
	if( !m_csAdminPoint.UpdateKey(pcsPoint->m_lID, szName) )		return FALSE;

	ZeroMemory( pcsPoint->m_szPointName, sizeof(CHAR) * (AGPD_TELEPORT_MAX_POINT_NAME + 1) );
	strncpy( pcsPoint->m_szPointName, szName, AGPD_TELEPORT_MAX_POINT_NAME );

	return TRUE;
}

AgpdTeleportPoint* AgpmEventTeleport::GetSequencePoint(INT32 *plIndex)
{
	AgpdTeleportPoint** ppcsPoint = (AgpdTeleportPoint**)m_csAdminPoint.GetObjectSequence(plIndex);
	return ppcsPoint ? *ppcsPoint : NULL;
}

AgpdTeleportPoint* AgpmEventTeleport::GetSequencePointInGroup(AgpdTeleportGroup *pcsGroup, INT_PTR *plIndex)
{
	if( !pcsGroup )			return NULL;
	if( *plIndex == -1 )	return NULL;

	AuNode <AgpdTeleportPoint *>* pNode = *plIndex ? (AuNode<AgpdTeleportPoint *>*)*plIndex : pcsGroup->m_listPoint.GetHeadNode();
	if( !pNode )			return NULL;
		
	AgpdTeleportPoint* pcsPoint = pNode->GetData();

	pNode = pNode->GetNextNode();
	*plIndex = pNode ? (INT_PTR)pNode : -1;

	return pcsPoint;
}

AgpdTeleportGroup* AgpmEventTeleport::AddTeleportGroup(CHAR *szGroupName, CHAR *szGroupDescription)
{
	if( !szGroupName || !strlen(szGroupName) )		return NULL;

	AgpdTeleportGroup *pcsGroup	= CreateTeleportGroup();
	if( !pcsGroup )		return NULL;

	pcsGroup->m_lID = ++m_lMaxGroupID;

	if ( !m_csAdminGroup.AddObject(&pcsGroup, pcsGroup->m_lID, szGroupName) )
	{
		--m_lMaxGroupID;
		DestroyTeleportGroup(pcsGroup);
		return NULL;
	}

	ZeroMemory(pcsGroup->m_szGroupName, sizeof(CHAR) * (AGPD_TELEPORT_MAX_GROUP_NAME + 1));
	strncpy(pcsGroup->m_szGroupName, szGroupName, AGPD_TELEPORT_MAX_GROUP_NAME);

	if (szGroupDescription && strlen(szGroupDescription))
	{
		ZeroMemory(pcsGroup->m_szDescription, sizeof(CHAR) * (AGPD_TELEPORT_MAX_GROUP_DESCRIPTION + 1));
		strncpy(pcsGroup->m_szDescription, szGroupDescription, AGPD_TELEPORT_MAX_GROUP_DESCRIPTION);
	}

	return pcsGroup;
}

BOOL AgpmEventTeleport::RemoveTeleportGroup(AgpdTeleportGroup *pcsGroup)
{
	if( !pcsGroup )		return FALSE;

	BOOL	bResult	= FALSE;

	// pcsGroup에 속한 포인트들에서 이 그룹에 대한 정보를 삭제한다.
	INT_PTR lIndex = 0;
	for (AgpdTeleportPoint* pcsPoint = GetSequencePointInGroup(pcsGroup, &lIndex); pcsPoint; pcsPoint = GetSequencePointInGroup(pcsGroup, &lIndex))
		bResult |= RemovePointNGroup(pcsGroup, pcsPoint);
	
	INT32 lIndex2 = 0;
	for (AgpdTeleportPoint* pcsPoint = GetSequencePoint(&lIndex2); pcsPoint; pcsPoint = GetSequencePoint(&lIndex2))
		RemoveTargetGroupFromPoint(pcsGroup, pcsPoint);

	bResult	= m_csAdminGroup.RemoveObject(pcsGroup->m_lID, pcsGroup->m_szGroupName);
	bResult	|=	DestroyTeleportGroup(pcsGroup);

	return bResult;
}

AgpdTeleportGroup* AgpmEventTeleport::GetTeleportGroup(CHAR *szGroupName)
{
	AgpdTeleportGroup** ppcsGroup = (AgpdTeleportGroup **) m_csAdminGroup.GetObject(szGroupName);
	return ppcsGroup ? *ppcsGroup : NULL;
}

AgpdTeleportGroup* AgpmEventTeleport::GetTeleportGroup(INT32 lID)
{
	AgpdTeleportGroup** ppcsGroup = (AgpdTeleportGroup **) m_csAdminGroup.GetObject(lID);
	return ppcsGroup ? *ppcsGroup : NULL;
}

BOOL AgpmEventTeleport::ChangeTeleportGroupName(AgpdTeleportGroup *pcsGroup, CHAR *szName)
{
	if( !m_csAdminGroup.UpdateKey(pcsGroup->m_lID, szName) )		return FALSE;

	ZeroMemory(pcsGroup->m_szGroupName, sizeof(CHAR) * (AGPD_TELEPORT_MAX_GROUP_NAME + 1));
	strncpy(pcsGroup->m_szGroupName, szName, AGPD_TELEPORT_MAX_GROUP_NAME);

	return TRUE;
}

AgpdTeleportGroup* AgpmEventTeleport::GetSequenceGroup(INT32 *plIndex)
{
	AgpdTeleportGroup**	ppcsGroup = (AgpdTeleportGroup **) m_csAdminGroup.GetObjectSequence(plIndex);
	return ppcsGroup ? *ppcsGroup : NULL;
}

AgpdTeleportGroup* AgpmEventTeleport::GetSequenceGroupInPoint(AgpdTeleportPoint *pcsPoint, INT_PTR *plIndex)
{
	if( !pcsPoint )			return NULL;
	if( *plIndex == -1 )	return NULL;

	AuNode <AgpdTeleportGroup *>* pNode = (*plIndex) ? (AuNode<AgpdTeleportGroup*>*)*plIndex : pcsPoint->m_listGroup.GetHeadNode();
	if( !pNode )			return NULL;
		
	AgpdTeleportGroup* pcsGroup = pNode->GetData();

	pNode = pNode->GetNextNode();
	*plIndex = pNode ? (INT_PTR)pNode : -1;

	return pcsGroup;
}

AgpdTeleportGroup* AgpmEventTeleport::GetSequenceTargetGroup(AgpdTeleportPoint *pcsPoint, INT_PTR *plIndex)
{
	if( !pcsPoint )			return NULL;
	if( *plIndex == -1 )	return NULL;

	AuNode <AgpdTeleportGroup*>* pNode = (*plIndex) ? (AuNode<AgpdTeleportGroup*>*)*plIndex : pcsPoint->m_listTargetGroup.GetHeadNode();
	if( !pNode )			return NULL;

	AgpdTeleportGroup* pcsGroup = pNode->GetData();

	pNode = pNode->GetNextNode();
	*plIndex = pNode ? (INT_PTR)pNode : -1;

	return pcsGroup;
}

VOID AgpmEventTeleport::DeleteAllGroupInPoint(AgpdTeleportPoint *pcsPoint)
{
	INT_PTR lIndex = 0;
	for( AgpdTeleportGroup* pcsGroup = GetSequenceGroupInPoint(pcsPoint, &lIndex); pcsGroup; pcsGroup = GetSequenceGroupInPoint(pcsPoint, &lIndex))
		RemovePointNGroup(pcsGroup, pcsPoint);

	for( AgpdTeleportGroup* pcsGroup = GetSequenceTargetGroup(pcsPoint, &lIndex); pcsGroup; pcsGroup = GetSequenceTargetGroup(pcsPoint, &lIndex))
		RemoveTargetGroupFromPoint(pcsGroup, pcsPoint);
}

VOID AgpmEventTeleport::DeleteAllPointInGroup(AgpdTeleportGroup *pcsGroup)
{
	INT_PTR lIndex = 0;
	for( AgpdTeleportPoint* pcsPoint = GetSequencePointInGroup(pcsGroup, &lIndex); pcsPoint; pcsPoint = GetSequencePointInGroup(pcsGroup, &lIndex))
		RemovePointNGroup(pcsGroup, pcsPoint);
}

INT32 AgpmEventTeleport::GetTargetPointList(AgpdTeleportPoint *pcsPoint, AgpdTeleportPoint *apcsTargetPoint[], INT32 lBufferSize)
{
	if (!pcsPoint || !apcsTargetPoint || lBufferSize < 1)
		return -1;

	AgpdTeleportPoint *	pcsTarget;
	AgpdTeleportGroup *	pcsGroup;
	INT_PTR				lIndex1;
	INT_PTR				lIndex2;
	INT32				lIndex3 = 0;

	lIndex1 = 0;
	for (pcsGroup = GetSequenceTargetGroup(pcsPoint, &lIndex1); pcsGroup; pcsGroup = GetSequenceTargetGroup(pcsPoint, &lIndex1))
	{
		lIndex2 = 0;
		for (pcsTarget = GetSequencePointInGroup(pcsGroup, &lIndex2); pcsTarget; pcsTarget = GetSequencePointInGroup(pcsGroup, &lIndex2))
		{
			if (lIndex3 >= lBufferSize)
				return (lBufferSize + 1);

			apcsTargetPoint[lIndex3++] = pcsTarget;
		}
	}

	return lIndex3;
}

BOOL	AgpmEventTeleport::GetTargetPoint(AgpdTeleportPoint *pstPoint, AuPOS *pstPos, FLOAT *pfRadiusMin, FLOAT *pfRadiusMax)
{
	// Type에 따라 알맞은 Point를 return한다.
	switch (pstPoint->m_eType)
	{
		// Position의 경우, 그냥 Copy하여 return한다.
	case AGPDTELEPORT_TARGET_TYPE_POS:
		*pstPos = pstPoint->m_uniTarget.m_stPos;
		*pfRadiusMin = pstPoint->m_fRadiusMin;
		*pfRadiusMax = pstPoint->m_fRadiusMax;
		return TRUE;

	/*
		// Index인 경우, 해당 Point로 recursive하게 구하여 return한다.
	case AGPDTELEPORT_TARGET_TYPE_INDEX:
		if (pstPoint->m_uniTarget.m_ulIndex >= m_unTeleportPoint)
			return FALSE;

		return GetTargetPoint(m_astTeleportPoints + pstPoint->m_uniTarget.m_ulIndex, pstPos, pfRadius);
		*/

		// Base인 경우, 알맞은 Base형에 따라 위치를 구한다.
	case AGPDTELEPORT_TARGET_TYPE_BASE:
		ApBase *	pcsBase = m_pcsApmEventManager->GetBase(pstPoint->m_uniTarget.m_stBase.m_eBaseType, pstPoint->m_uniTarget.m_stBase.m_lID);
		if (!pcsBase)
			return FALSE;

		switch (pstPoint->m_uniTarget.m_stBase.m_eBaseType)
		{
			// 각종 Template의 경우는 위치를 구할 수 없다.
		case APBASE_TYPE_OBJECT_TEMPLATE:
		case APBASE_TYPE_CHARACTER_TEMPLATE:
		case APBASE_TYPE_ITEM_TEMPLATE:
			return FALSE;

			// Object의 경우 해당 Object의 위치를 return한다.
		case APBASE_TYPE_OBJECT:
			*pstPos = ((ApdObject *) pcsBase)->m_stPosition;
			*pfRadiusMin = pstPoint->m_fRadiusMin;
			*pfRadiusMax = pstPoint->m_fRadiusMax;
			return TRUE;

			// Character의 경우 해당 Character의 위치를 returng한다.
		case APBASE_TYPE_CHARACTER:
			*pstPos = ((AgpdCharacter *) pcsBase)->m_stPos;
			*pfRadiusMin = pstPoint->m_fRadiusMin;
			*pfRadiusMax = pstPoint->m_fRadiusMax;
			return TRUE;

		case APBASE_TYPE_ITEM:
			AgpdItem *	pcsItem = ((AgpdItem *) pcsBase);

			// 만약 아이템이 Field에 있다면, 현 Item의 위치를 return한다.
			if (pcsItem->m_eStatus == AGPDITEM_STATUS_FIELD)
			{
				*pstPos = pcsItem->m_posItem;
				*pfRadiusMin = pstPoint->m_fRadiusMin;
				*pfRadiusMax = pstPoint->m_fRadiusMax;
				return TRUE;
			}
			// Character가 가지고 있다면, 해당 Character위 위치를 recursive하게 구하여 return한다.
			else
			{
				AgpdTeleportPoint	stPoint = *pstPoint;

				stPoint.m_uniTarget.m_stBase.m_eBaseType = APBASE_TYPE_CHARACTER;
				stPoint.m_uniTarget.m_stBase.m_lID = pcsItem->m_ulCID;
				return GetTargetPoint(&stPoint, pstPos, pfRadiusMin, pfRadiusMax);
			}
		}
	}

	return FALSE;
}

BOOL AgpmEventTeleport::CheckValidTeleport(AgpdTeleportPoint *pcsSourcePoint, AgpdTeleportPoint *pcsTargetPoint)
{
	if (!pcsSourcePoint || !pcsTargetPoint)
		return FALSE;

	if (pcsTargetPoint->m_eRegionType == AGPDTELEPORT_REGION_TYPE_PVP)
		return TRUE;

	INT_PTR	lIndex1;
	INT_PTR	lIndex2;
	AgpdTeleportGroup *	pcsSourceGroup;
	AgpdTeleportGroup *	pcsTargetGroup;

	lIndex1 = 0;
	for (pcsSourceGroup = GetSequenceTargetGroup(pcsSourcePoint, &lIndex1); pcsSourceGroup; pcsSourceGroup = GetSequenceTargetGroup(pcsSourcePoint, &lIndex1))
	{
		lIndex2 = 0;
		for (pcsTargetGroup = GetSequenceGroupInPoint(pcsTargetPoint, &lIndex2); pcsTargetGroup; pcsTargetGroup = GetSequenceGroupInPoint(pcsTargetPoint, &lIndex2))
		{
			if (pcsSourceGroup == pcsTargetGroup)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmEventTeleport::CheckBaseStatus(ApBase *pcsBase)
{
	if ( !pcsBase )		return FALSE;
		
	if( pcsBase->m_eType != APBASE_TYPE_CHARACTER )									return TRUE;	// if criminal flag of pcsBase is setted, pcsBase can't teleport
	if( ((AgpdCharacter *) pcsBase)->m_unActionStatus == AGPDCHAR_STATUS_DEAD )		return FALSE;
	if( ((AgpdCharacter*)pcsBase)->m_unActionStatus == AGPDCHAR_STATUS_TRADE )		return FALSE;	//JK_거래중금지
	if( m_pcsAgpmCharacter->IsCombatMode((AgpdCharacter *) pcsBase) )				return FALSE;
	if( ((AgpdCharacter *) pcsBase)->m_bIsTrasform )								return TRUE;
	if( m_pcsAgpmCharacter->IsAllBlockStatus((AgpdCharacter *) pcsBase) )			return FALSE;
	

	return TRUE;
}

BOOL AgpmEventTeleport::Teleport(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bCheckRequirement, AgpdTeleportPoint *pcsSource, AgpdTeleportPoint *pcsTarget)
{
	if( !pcsCharacter || !pstTargetPos )		return FALSE;
		
	if( !EnumCallback(AGPMEVENT_TELEPORT_CB_ID_CALC_POS, NULL, pstTargetPos) )
		return FALSE;

	//	teleport 하는데 필요한 요구사항들을 체크한다.
	//	여기서 하나라도 통과 못하면 텔레포트 할 수 없다.
	///////////////////////////////////////////////////////////////////////
	
	// 로그를 위해 위로 올렸당.
	INT32	lFee = 0;
	INT32	lTax = 0;
	
	if( bCheckRequirement )
	{
		// 케릭터에서 돈을 뺀다.
		if (pcsSource && pcsTarget)
		{
			lFee = GetFee(pcsSource->m_szPointName, pcsTarget->m_szPointName, pcsCharacter, &lTax);

			// 돈이 부족하면, No
			if (!m_pcsAgpmCharacter->SubMoney(pcsCharacter, lFee))
				return FALSE;
			
			m_pcsAgpmCharacter->PayTax(pcsCharacter, lTax);
		}
	}

	pcsCharacter->m_bMove			= FALSE		;
	pcsCharacter->m_eMoveDirection	= MD_NODIRECTION;
	pcsCharacter->m_bPathFinding	= FALSE		;

	EnumCallback(AGPMEVENT_TELEPORT_CB_ID_SYNC_TELEPORT_LOADING, pcsCharacter, pstTargetPos);

	// 텔레포트 성공. 로그를 남기자.
	PVOID pvBuffer[3];
	pvBuffer[0] = pcsSource ? pcsSource->m_szPointName : NULL;
	pvBuffer[1] = pcsTarget ? pcsTarget->m_szPointName : NULL;
	pvBuffer[2] = &lFee;
	
	EnumCallback(AGPMEVENT_TELEPORT_CB_ID_TELEPORT_LOG, pcsCharacter, pvBuffer);

	// BattleGround Event를 위한 LOG작업
	INT32 lBattleGroundState = 0;
	EnumCallback(AGPMEVENT_TELEPORT_CB_ID_GET_BATTLEGROUND_STATE, &lBattleGroundState, NULL);

	INT32 lRegionIndex = m_pApmMap->GetRegion(pstTargetPos->x, pstTargetPos->z);
	if(lBattleGroundState != 0 && m_pcsAgpmBattleGround->IsBattleGround(lRegionIndex))
	{
		// Log를 남긴다.
		EnumCallback(AGPMEVENT_TELEPORT_CB_ID_BATTLEGROUND_TELEPORT_LOG, pcsCharacter, pvBuffer);
	}

	return TRUE;
}

BOOL AgpmEventTeleport::AddPointToGroup(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint)
{
	if (!pcsGroup || !pcsPoint)
		return FALSE;

	// Group에 Point 정보를 추가한다.
	if( !pcsGroup->m_listPoint.AddTail(pcsPoint) )
		return FALSE;

	// Point에 Group 정보를 추가한다.
	if( !pcsPoint->m_listGroup.AddTail(pcsGroup) )
	{
		pcsGroup->m_listPoint.RemoveData(pcsPoint);
		return FALSE;
	}

	return	TRUE;
}

BOOL AgpmEventTeleport::AddTargetGroupToPoint(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint)
{
	if( !pcsGroup || !pcsPoint )		return FALSE;
		
	return pcsPoint->m_listTargetGroup.AddTail(pcsGroup);
}

BOOL AgpmEventTeleport::RemovePointNGroup(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint)
{
	if( !pcsGroup || !pcsPoint )		return FALSE;

	if( !pcsGroup->m_listPoint.RemoveData(pcsPoint) )
		return FALSE;

	if( !pcsPoint->m_listGroup.RemoveData(pcsGroup) )
	{
		pcsGroup->m_listPoint.AddTail(pcsPoint);
		return FALSE;
	}

	return	TRUE;
}

BOOL AgpmEventTeleport::RemoveTargetGroupFromPoint(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint)
{
	return pcsPoint->m_listTargetGroup.RemoveData(pcsGroup);
}

BOOL	AgpmEventTeleport::SetCallbackTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_TELEPORT, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackCalcPos(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_CALC_POS, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackReceivedRequestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVED_REQUEST_RESULT, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackReceivedTeleportStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVED_TELEPORT_START, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackReceivedTeleportCanceled(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVED_TELEPORT_CANCELED, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackCheckUsableTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_CHECK_USABLE_TELEPORT, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackReturnTown(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_RETURN_TOWN, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackTeleportLoading(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVE_TELEPORT_LOADING, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackSyncTeleportLoading(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_SYNC_TELEPORT_LOADING, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackTeleportLog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_TELEPORT_LOG, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackBattleGroundTeleportLog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_BATTLEGROUND_TELEPORT_LOG, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SetCallbackArchlordMessageId(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_ARCHLORD_MESSAGE_ID, pfCallback, pClass);
}

BOOL AgpmEventTeleport::SetCallbackGetBattleGroundState(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_GET_BATTLEGROUND_STATE, pfCallback, pClass);
}

BOOL	AgpmEventTeleport::SendTeleportInfo(ApdEvent *pstEvent, INT32 *plTargetCID, CHAR *szPointName, AuPOS *pstPos)
{
	PVOID	pvPacketBase = NULL;
	INT8	cOperation;

	if( pstEvent )
	{
		cOperation		= AGPMEVENT_TELEPORT_OPERATION_TELEPORT_POINT;
		pvPacketBase	= m_pcsApmEventManager->MakeBasePacket( pstEvent );
		if( !pvPacketBase )
			return FALSE;
	}

	INT16 nSize;
	PVOID pvPacket = m_csPacket.MakePacket( TRUE, &nSize, AGPMEVENT_TELEPORT_PACKET_TYPE, &cOperation, pvPacketBase, plTargetCID, szPointName, pstPos, NULL );

	m_csPacket.FreePacket(pvPacketBase);

	BOOL	bSendResult	= SendPacket(pvPacket, nSize);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::SendTeleportReturn(ApdEvent *pstEvent, INT32 *plTargetCID)
{
	INT16	nSize		= 0;
	PVOID	pvPacket	= NULL;
	PVOID	pvPacketBase = NULL;

	INT8	cOperation	= (-1);

	if (pstEvent)
	{
		cOperation		= AGPMEVENT_TELEPORT_OPERATION_TELEPORT_RETURN;
		pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pstEvent);
		if (!pvPacketBase)
			return FALSE;
	}

	pvPacket = m_csPacket.MakePacket(TRUE, &nSize, AGPMEVENT_TELEPORT_PACKET_TYPE,
		&cOperation,
		pvPacketBase, 
		plTargetCID,
		NULL,
		NULL,
		NULL);

	m_csPacket.FreePacket(pvPacketBase);

	BOOL	bSendResult	= SendPacket(pvPacket, nSize);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::SendTeleportStart(INT32 *plTargetCID, AuPOS *pstPos, UINT32 ulNID)
{
	if (!plTargetCID)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMEVENT_TELEPORT_OPERATION_TELEPORT_START;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_TELEPORT_PACKET_TYPE,
													&cOperation,
													NULL,
													plTargetCID,
													NULL,
													pstPos,
													NULL);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::SendTeleportCanceled(INT32 *plTargetCID, UINT32 ulNID)
{
	if (!plTargetCID)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMEVENT_TELEPORT_OPERATION_TELEPORT_CANCELED;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_TELEPORT_PACKET_TYPE,
													&cOperation,
													NULL,
													plTargetCID,
													NULL,
													NULL,
													NULL);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::SendRequestTeleport(ApdEvent *pstEvent, INT32 *plTargetCID)
{
	if (!pstEvent || !plTargetCID)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pstEvent);
	if (!pvPacketBase)
		return FALSE;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_TELEPORT_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													plTargetCID,
													NULL,
													NULL,
													NULL);

	m_csPacket.FreePacket(pvPacketBase);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::SendRequestTeleportGranted(ApdEvent *pstEvent, AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pstEvent || !pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_GRANTED;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pstEvent);
	if (!pvPacketBase)
		return FALSE;


	INT32	alDisableTargetID[20];
	ZeroMemory(alDisableTargetID, sizeof(alDisableTargetID));

	INT32	lCount	= 0;

	AgpdTeleportAttach	*pcsAttachData	= (AgpdTeleportAttach*) pstEvent->m_pvData;
	if (pcsAttachData && pcsAttachData->m_pcsPoint)
	{
		if (IsDisablePoint(pcsAttachData->m_pcsPoint, pcsCharacter))
		{
			lCount	= 1;
			alDisableTargetID[0]	= pcsAttachData->m_pcsPoint->m_lID;
		}
		else
		{
			INT_PTR	lIndex1;
			INT_PTR	lIndex2;
			AgpdTeleportGroup *	pcsTargetGroup;
			AgpdTeleportPoint *	pcsTargetPoint;

			lIndex1 = 0;
			for (pcsTargetGroup = GetSequenceTargetGroup(pcsAttachData->m_pcsPoint, &lIndex1); pcsTargetGroup; pcsTargetGroup = GetSequenceTargetGroup(pcsAttachData->m_pcsPoint, &lIndex1))
			{
				lIndex2 = 0;
				for (pcsTargetPoint	= GetSequencePointInGroup(pcsTargetGroup, &lIndex2); pcsTargetPoint; pcsTargetPoint = GetSequencePointInGroup(pcsTargetGroup, &lIndex2))
				{
					if (pcsTargetPoint->m_eSpecialType == AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR &&
						IsDisablePoint(pcsTargetPoint, pcsCharacter))
						alDisableTargetID[lCount++]	= pcsTargetPoint->m_lID;

					if (lCount >= 20)
						break;
				}

				if (lCount >= 20)
					break;
			}
		}
	}

	UINT16	unDisableTargetIDLength	= (UINT16) ( lCount * (INT32) sizeof(INT32) );

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_TELEPORT_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&pcsCharacter->m_lID,
													NULL,
													NULL,
													alDisableTargetID, &unDisableTargetIDLength);

	m_csPacket.FreePacket(pvPacketBase);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::SendTeleportLoading(INT32 lCID, UINT32 ulNID)
{
	if (lCID == AP_INVALID_CID)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMEVENT_TELEPORT_OPERATION_TELEPORT_LOADING;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_TELEPORT_PACKET_TYPE,
													&cOperation,
													NULL,
													&lCID,
													NULL,
													NULL,
													NULL);

	BOOL	bSendResult	=	TRUE;
	if (ulNID == 0)
		bSendResult	= SendPacket(pvPacket, nPacketLength);
	else
		bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL	AgpmEventTeleport::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8		cOperation				= (-1);
	PVOID		pvPacketEventBase		= NULL;
	INT32		lCID					= AP_INVALID_CID;
	CHAR*		szPointName				= NULL;
	AuPOS		stPos;
	PVOID		pvDisableTargetID		= NULL;
	UINT16		unDisableTargetIDLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEventBase,
						&lCID,
						&szPointName,
						&stPos,
						&pvDisableTargetID, &unDisableTargetIDLength);

	if( !pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		AP_INVALID_CID != lCID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	switch (cOperation)
	{
	case AGPMEVENT_TELEPORT_OPERATION_TELEPORT_POINT:
		{
			if( !pvPacketEventBase || !szPointName || AP_INVALID_CID == lCID )
				return FALSE;

			AgpdTeleportPoint* pcsTeleportPoint = GetTeleportPoint(szPointName);
			if( !pcsTeleportPoint )					return FALSE;

			ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket( pvPacketEventBase );
			if( !pcsEvent || !pcsEvent->m_pvData )	return FALSE;
				
			// pcsEvent에 Teleport가 붙어 있는지.. 이 Event에서 lPointID로 텔레포트 할 수 있는지 등을 검사한다.
			////////////////////////////////////////////////////////////////////////////
			if( !CheckValidTeleport( ((AgpdTeleportAttach*)pcsEvent->m_pvData)->m_pcsPoint, pcsTeleportPoint ) )
				return FALSE;

			AuPOS	stTargetPos;
			FLOAT	fRadiusMin = 0.f;
			FLOAT	fRadiusMax = 0.f;

			// target position을 가져와서 텔레포트 시킨다.
			if( !GetTargetPoint( pcsTeleportPoint, &stTargetPos, &fRadiusMin, &fRadiusMax ) )
				return FALSE;

			// Teleport할 Character를 가져온다.
			AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock( lCID );
			if( !pcsCharacter )		return FALSE;

			if( !CheckBaseStatus( (ApBase*)pcsCharacter ) )
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			//도착할 포탈에 UsableType이 나와 맞는지 체크한다
			switch( m_pcsAgpmFactors->GetRace( &pcsCharacter->m_csFactor ) )
			{
			case AURACE_TYPE_HUMAN:			if( !pcsTeleportPoint->IsUseHuman() )		return FALSE;	break;
			case AURACE_TYPE_ORC:			if( !pcsTeleportPoint->IsUseOrc() )			return FALSE;	break;
			case AURACE_TYPE_MOONELF:		if( !pcsTeleportPoint->IsUseMoonElf() )		return FALSE;	break;
			case AURACE_TYPE_DRAGONSCION:	if( !pcsTeleportPoint->IsUseDragonScion() )	return FALSE;	break;
			default:																					return FALSE;
			}

			// 공성 전용 포탈은 수성길드만 이용할 수 있다.
			if (pcsTeleportPoint->m_eSpecialType == AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR &&
				IsDisablePoint(pcsTeleportPoint, pcsCharacter))
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			BOOL	bIsUsableTeleport	= TRUE;

			PVOID	pvBuffer[2];
			pvBuffer[0]		= &bIsUsableTeleport;
			pvBuffer[1]		= pcsTeleportPoint;

			EnumCallback(AGPMEVENT_TELEPORT_CB_ID_CHECK_USABLE_TELEPORT, pcsCharacter, pvBuffer);

			if (!bIsUsableTeleport)
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			BOOL bResult = FALSE;
			if( CheckValidRange( pcsEvent, &pcsCharacter->m_stPos ) )
			{
				AuPOS	stRandomPos;
				if( !m_pcsApmEventManager->GetRandomPos( &stTargetPos, &stRandomPos, fRadiusMin, fRadiusMax, TRUE ) )
				{
					SendTeleportCanceled(&pcsCharacter->m_lID, ulNID);
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				SendTeleportStart(&pcsCharacter->m_lID, &stRandomPos, ulNID);
				bResult = Teleport(pcsCharacter, &stRandomPos, TRUE, ((AgpdTeleportAttach *) pcsEvent->m_pvData)->m_pcsPoint, pcsTeleportPoint);
				if( !bResult )
					SendTeleportCanceled( &pcsCharacter->m_lID, ulNID );
				else
				{
					AgpdTeleportAttachToChar* pcsAttachToChar = GetADCharacter(pcsCharacter);
					if( pcsAttachToChar->m_pcsLastUseTeleportPoint )
						pcsAttachToChar->m_pcsLastUseTeleportPoint = ((AgpdTeleportAttach *) pcsEvent->m_pvData)->m_pcsPoint;
				}
			}

			pcsCharacter->m_Mutex.Release();

			return bResult;
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_TELEPORT_RETURN:
		{
			if( !pvPacketEventBase || lCID == AP_INVALID_CID )		return FALSE;

			ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
			if( !pcsEvent || !pcsEvent->m_pvData )	return FALSE;

			AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock( lCID );
			if( !pcsCharacter )						return FALSE;

			EnumCallback( AGPMEVENT_TELEPORT_CB_ID_RETURN_TOWN, pcsCharacter, NULL );

			/*
			AgpdTeleportAttachToChar	*pcsAttachToChar	= GetADCharacter(pcsCharacter);

			if (pcsAttachToChar->m_pcsLastUseTeleportPoint)
			{
				AuPOS				stTargetPos;
				FLOAT				fRadiusMin				= 0.0;
				FLOAT				fRadiusMax				= 0.0;

				// target position을 가져와서 텔레포트 시킨다.
				if (!GetTargetPoint(pcsAttachToChar->m_pcsLastUseTeleportPoint, &stTargetPos, &fRadiusMin, &fRadiusMax))
				{
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				if (!CheckBaseStatus((ApBase *) pcsCharacter))
				{
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				AuPOS	stRandomPos;
				if (!m_pcsApmEventManager->GetRandomPos(&stTargetPos, &stRandomPos, fRadiusMin, fRadiusMax, TRUE))
				{
					SendTeleportCanceled(&pcsCharacter->m_lID, ulNID);
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				SendTeleportStart(&pcsCharacter->m_lID, &stRandomPos, ulNID);
				BOOL bResult	= Teleport(pcsCharacter, &stRandomPos, TRUE);

				if (!bResult)
					SendTeleportCanceled(&pcsCharacter->m_lID, ulNID);
				else
				{
					pcsAttachToChar->m_pcsLastUseTeleportPoint	= ((AgpdTeleportAttach *) pcsEvent->m_pvData)->m_pcsPoint;
				}
			}
			*/

			pcsCharacter->m_Mutex.Release();

			return TRUE;
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_TELEPORT_CUSTOM_POS:
		{
			// Teleport할 Character를 가져온다.
			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
			if( !pcsCharacter )		return FALSE;

			BOOL bResult = Teleport( pcsCharacter, &stPos, FALSE );

			pcsCharacter->m_Mutex.Release();

			return bResult;
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_TELEPORT_START:
		{
			if( lCID == AP_INVALID_CID )		return FALSE;

			return EnumCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVED_TELEPORT_START, &lCID, &stPos);
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_TELEPORT_CANCELED:
		{
			if( lCID == AP_INVALID_CID )		return FALSE;

			return EnumCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVED_TELEPORT_CANCELED, &lCID, NULL);
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT:
		{
			// 거리를 체크하고 맞으면 OK 패킷 전송 아니면 가능 거리까지 이동
			if (!pvPacketEventBase || lCID == AP_INVALID_CID)
				return FALSE;

			ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
			if( !pcsEvent || !pcsEvent->m_pvData )		return FALSE;

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
			if( !pcsCharacter )		return FALSE;

			if( !CheckBaseStatus( (ApBase*)pcsCharacter ) )
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			AuPOS	stTargetPos;
			BOOL	bResult = FALSE;
			if( CheckValidRange( pcsEvent, &pcsCharacter->m_stPos, &stTargetPos ) )
			{
				AgpdTeleportAttach	*pcsAttachData	= (AgpdTeleportAttach*) pcsEvent->m_pvData;
				if( !pcsAttachData || !pcsAttachData->m_pcsPoint )
				{
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				//도착할 포탈에 UsableType이 나와 맞는지 체크한다
				switch( m_pcsAgpmFactors->GetRace( &pcsCharacter->m_csFactor ) )
				{
				case AURACE_TYPE_HUMAN:			if( !pcsAttachData->m_pcsPoint->IsUseHuman() )			return FALSE;	break;
				case AURACE_TYPE_ORC:			if( !pcsAttachData->m_pcsPoint->IsUseOrc() )			return FALSE;	break;
				case AURACE_TYPE_MOONELF:		if( !pcsAttachData->m_pcsPoint->IsUseMoonElf() )		return FALSE;	break;
				case AURACE_TYPE_DRAGONSCION:	if( !pcsAttachData->m_pcsPoint->IsUseDragonScion() )	return FALSE;	break;
				default:																								return FALSE;
				}

				if( AGPDTELEPORT_REGION_TYPE_PVP != pcsAttachData->m_pcsPoint->m_eRegionType )
				{
					if( !pcsAttachData->m_pcsPoint->m_listTargetGroup.GetCount() )
					{
						pcsCharacter->m_Mutex.Release();
						return FALSE;
					}
				}

				if( AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR == pcsAttachData->m_pcsPoint->m_eSpecialType )		// 공성 전용 포탈은 수성길드만 이용할 수 있다.
				{
					if( IsDisablePoint(pcsAttachData->m_pcsPoint, pcsCharacter) )
					{
						pcsCharacter->m_Mutex.Release();
						return FALSE;
					}
				}

				if( AGPDTELEPORT_SPECIAL_TYPE_CASTLE_TO_DUNGEON == pcsAttachData->m_pcsPoint->m_eSpecialType )
				{
					// 성주 사냥터로 접속할 때
					// 성주 사냥터 포탈은 성의 내부에 있으므로 성 내부의 공성 정보를 얻어와 이 캐릭터가
					// 이 성을 가진 길드원인지를 체크하고, 현재 공성중이 아닐 때 들어갈 수 있도록 한다.
					// 아크로드 결정전 중에는 이용하지 못하도록 한다.
					// 성주 사냥터 내부에 있는 포탈로 돌아올 수 있도록 함.
					if(!m_pcsAgpmSiegeWar->IsInSecretDungeon(pcsCharacter) && 
						(!m_pcsAgpmSiegeWar->IsThisRegionCastleOwnerGuildMember(pcsCharacter)
						|| m_pcsAgpmSiegeWar->IsThisRegionCastleInSiegeWar(pcsCharacter)
						|| m_pcsAgpmArchlord->GetCurrentStep() != AGPMARCHLORD_STEP_NONE))
					{
						pcsCharacter->m_Mutex.Release();
						return FALSE;
					}
				}

				if( AGPDTELEPORT_SPECIAL_TYPE_DUNGEON_TO_LANSPHERE == pcsAttachData->m_pcsPoint->m_eSpecialType )		// 비밀의 던젼에서 랜스피어로 갈 수 있는 포탈에 대한 처리
				{
					if( !IsArchlordDungeonPoint(pcsAttachData->m_pcsPoint, pcsCharacter) )
					{
						EnumCallback( AGPMEVENT_TELEPORT_CB_ID_ARCHLORD_MESSAGE_ID, pcsCharacter, NULL );
						pcsCharacter->m_Mutex.Release();
						return FALSE;
					}
				}

				bResult = SendRequestTeleportGranted( pcsEvent, pcsCharacter, ulNID );		// AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_GRANTED 패킷 전송
			}
			else
			{
				// stTargetPos 까지 이동
				pcsCharacter->m_stNextAction.m_bForceAction			= FALSE;
				pcsCharacter->m_stNextAction.m_eActionType			= AGPDCHAR_ACTION_TYPE_EVENT_TELEPORT;
				pcsCharacter->m_stNextAction.m_csTargetBase.m_eType	= pcsEvent->m_pcsSource->m_eType;
				pcsCharacter->m_stNextAction.m_csTargetBase.m_lID	= pcsEvent->m_pcsSource->m_lID;
				pcsCharacter->m_stNextAction.m_lUserData[0]			= (INT32) ulNID;

				bResult = m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
			}

			pcsCharacter->m_Mutex.Release();

			return bResult;
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_GRANTED:
		{
			if( !pvPacketEventBase || lCID == AP_INVALID_CID )		return FALSE;

			ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket( pvPacketEventBase );
			if( !pcsEvent || !pcsEvent->m_pvData )		return FALSE;

			PVOID	pvBuffer[4];
			pvBuffer[0]	= IntToPtr(lCID);
			pvBuffer[1] = IntToPtr(TRUE);
			pvBuffer[2]	= pvDisableTargetID;
			pvBuffer[3]	= IntToPtr(unDisableTargetIDLength);

			return EnumCallback( AGPMEVENT_TELEPORT_CB_ID_RECEIVED_REQUEST_RESULT, pcsEvent, pvBuffer );		// 요청이 승인되었다. UI에 리스트 보여준다.
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_IGNOIRED:
		{
			if( !pvPacketEventBase || lCID == AP_INVALID_CID )		return FALSE;

			ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket( pvPacketEventBase );
			if( !pcsEvent || !pcsEvent->m_pvData )					return FALSE;
				
			PVOID	pvBuffer[4];
			pvBuffer[0]	= IntToPtr(lCID);
			pvBuffer[1] = IntToPtr(FALSE);
			pvBuffer[2]	= NULL;
			pvBuffer[3]	= NULL;

			return EnumCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVED_REQUEST_RESULT, pcsEvent, pvBuffer);		// 이런, 텔레포트 하지 말란다.
		}
		break;

	case AGPMEVENT_TELEPORT_OPERATION_TELEPORT_LOADING:
		{
			if ( lCID == AP_INVALID_CID )		return FALSE;

			return EnumCallback(AGPMEVENT_TELEPORT_CB_ID_RECEIVE_TELEPORT_LOADING, &lCID, NULL);
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL	AgpmEventTeleport::CBActionEventTeleport(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventTeleport		*pThis					= (AgpmEventTeleport *)		pClass;
	AgpdCharacter			*pcsCharacter			= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pstActionData			= (AgpdCharacterAction *)	pCustData;

	if( pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter) )		return FALSE;

	ApdEvent* pcsEvent = pThis->m_pcsApmEventManager->GetEvent( pstActionData->m_csTargetBase.m_eType, pstActionData->m_csTargetBase.m_lID, APDEVENT_FUNCTION_TELEPORT );
	if( !pcsEvent )			return FALSE;
	if( !pThis->CheckBaseStatus((ApBase *) pcsCharacter) )		return FALSE;

	AuPOS	stTargetPos;
	if( !pThis->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, &stTargetPos) )		return FALSE;

	AgpdTeleportAttach* pcsAttachData = (AgpdTeleportAttach*) pcsEvent->m_pvData;

	// 2006.12.01. steeple
	// bug from netong~
	if (!pcsAttachData || !pcsAttachData->m_pcsPoint)
	{
		//pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	//도착할 포탈에 UsableType이 나와 맞는지 체크한다
	switch( pThis->m_pcsAgpmFactors->GetRace( &pcsCharacter->m_csFactor ) )
	{
	case AURACE_TYPE_HUMAN:			if( !pcsAttachData->m_pcsPoint->IsUseHuman() )			return FALSE;	break;
	case AURACE_TYPE_ORC:			if( !pcsAttachData->m_pcsPoint->IsUseOrc() )			return FALSE;	break;
	case AURACE_TYPE_MOONELF:		if( !pcsAttachData->m_pcsPoint->IsUseMoonElf() )		return FALSE;	break;
	case AURACE_TYPE_DRAGONSCION:	if( !pcsAttachData->m_pcsPoint->IsUseDragonScion() )	return FALSE;	break;
	default:																								return FALSE;
	}

	if( AGPDTELEPORT_REGION_TYPE_PVP != pcsAttachData->m_pcsPoint->m_eRegionType )
		if( !pcsAttachData->m_pcsPoint->m_listTargetGroup.GetCount() )
			return FALSE;

	// 공성 전용 포탈은 수성길드만 이용할 수 있다.
	if( AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR == pcsAttachData->m_pcsPoint->m_eSpecialType )
		if( pThis->IsDisablePoint(pcsAttachData->m_pcsPoint, pcsCharacter) )
			return FALSE;

	if ( AGPDTELEPORT_SPECIAL_TYPE_CASTLE_TO_DUNGEON == pcsAttachData->m_pcsPoint->m_eSpecialType )
		if ( !pcsAttachData->m_pcsPoint->m_bActive )
			return FALSE;

	// 비밀의 던젼에서 랜스피어로 갈 수 있는 포탈에 대한 처리
	if (AGPDTELEPORT_SPECIAL_TYPE_DUNGEON_TO_LANSPHERE == pcsAttachData->m_pcsPoint->m_eSpecialType)
	{
		if (!pThis->IsArchlordDungeonPoint(pcsAttachData->m_pcsPoint, pcsCharacter) )
		{
			pThis->EnumCallback(AGPMEVENT_TELEPORT_CB_ID_ARCHLORD_MESSAGE_ID, pcsCharacter, NULL);
			return FALSE;
		}
	}

	UINT32 ulNID = (UINT32)pstActionData->m_lUserData[0];
	return pThis->SendRequestTeleportGranted(pcsEvent, pcsCharacter, ulNID);		// AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_GRANTED 패킷 전송
}

BOOL	AgpmEventTeleport::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventTeleport *		pThis = (AgpmEventTeleport *) pClass;
	ApdEvent *				pstDstEvent = (ApdEvent *) pData;
	ApdEvent *				pstSrcEvent = (ApdEvent *) pCustData;

	pstDstEvent->m_pvData = (PVOID) new BYTE[sizeof(AgpdTeleportAttach)];
	if( !pstDstEvent->m_pvData )		return FALSE;

	memset(pstDstEvent->m_pvData, 0, sizeof(AgpdTeleportAttach));

	return TRUE;
}

BOOL	AgpmEventTeleport::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventTeleport *		pThis		= (AgpmEventTeleport	*	) pClass			;
	ApdEvent *				pstEvent	= (ApdEvent				*	) pData				;
	AgpdTeleportAttach *	pstAttach	= (AgpdTeleportAttach	*	) pstEvent->m_pvData;

	if( !pstAttach )		return TRUE;

	if( pstAttach->m_pcsPoint )
	{
		if( !pThis->m_bProtectPoint )
			pThis->RemoveTeleportPoint( pstAttach->m_pcsPoint );
		else if( !IsBadReadPtr(pstAttach->m_pcsPoint, sizeof(AgpdTeleportPoint)) && pstAttach->m_pcsPoint->m_pstAttach )
			pstAttach->m_pcsPoint->m_pstAttach = NULL;

		pstAttach->m_pcsPoint = NULL;
	}

	delete [] (BYTE*)pstAttach;
	pstEvent->m_pvData	= NULL;

	return TRUE;
}

BOOL	AgpmEventTeleport::CheckValidRange(ApdEvent *pcsEvent, AuPOS *pcsGenerator, AuPOS *pstDestPos)
{
	if (!pcsEvent || !pcsGenerator)
		return FALSE;

	return m_pcsApmEventManager->CheckValidRange(pcsEvent, pcsGenerator, AGPMTELEPORT_MAX_USE_RANGE, pstDestPos);
}

BOOL	AgpmEventTeleport::ConAgpdTeleportAttachToChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventTeleport	*pThis			= (AgpmEventTeleport *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	AgpdTeleportAttachToChar	*pcsAttachToChar	= pThis->GetADCharacter(pcsCharacter);
	if( pcsAttachToChar )
		pcsAttachToChar->m_pcsLastUseTeleportPoint	= NULL;

	return TRUE;
}

BOOL	AgpmEventTeleport::DesAgpdTeleportAttachToChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgpdTeleportAttachToChar* AgpmEventTeleport::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	if( m_pcsAgpmCharacter )
		return (AgpdTeleportAttachToChar*) m_pcsAgpmCharacter->GetAttachedModuleData( m_nIndexADCharacter, (PVOID)pcsCharacter );

	return NULL;
}

INT32	AgpmEventTeleport::GetFee(CHAR *szSource, CHAR *szTarget, AgpdCharacter *pcsCharacter, INT32 *plTax)
{
	CHAR					szSource2Target[128];
	INT32 *					plFee;
	INT32					lFee = 0;

	sprintf(szSource2Target, "%s-%s", szSource, szTarget);

	plFee = (INT32 *) m_csAdminFee.GetObject(szSource2Target);
	if (!plFee)
		return lFee;

	lFee = *plFee;
	
	INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = (lFee * lTaxRatio) / 100;
	}
	lFee = lFee + lTax;
	if (plTax)
		*plTax = lTax;
	
	return lFee;
}

BOOL AgpmEventTeleport::IsDisablePoint(AgpdTeleportPoint *pcsTeleportPoint, AgpdCharacter *pcsCharacter)
{
	if (!pcsTeleportPoint || !pcsCharacter)
		return FALSE;

	if( pcsTeleportPoint->m_eSpecialType != AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR )	return FALSE;

	AuPOS	stBasePos;
	FLOAT	fRadiusMin;
	FLOAT	fRadiusMax;
	if (!GetTargetPoint(pcsTeleportPoint, &stBasePos, &fRadiusMin, &fRadiusMax))
		return TRUE;

	AgpdSiegeWar* pcsSiegeWar = m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(stBasePos));
	if( m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar) )
	{
		if( m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar) ) // 아크로드 공성중이면 포탈 사용불가.
			return TRUE;
		if( !pcsSiegeWar->m_bIsActiveLifeTower )
			return TRUE;
		if( !m_pcsAgpmSiegeWar->IsDefenseGuild(pcsCharacter, pcsSiegeWar) )
			return TRUE;
	}
	else if( pcsSiegeWar )
	{
		if ( !m_pcsAgpmSiegeWar->IsOwnerGuild(pcsCharacter, pcsSiegeWar) )
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmEventTeleport::IsArchlordDungeonPoint(AgpdTeleportPoint *pcsTeleportPoint, AgpdCharacter *pcsCharacter)
{
	// 비밀의 던젼에서 랜스피어로 갈 수 있는 단계인지 확인
	if (AGPMARCHLORD_STEP_DUNGEON != m_pcsAgpmArchlord->GetCurrentStep())
		return FALSE;

	if (pcsTeleportPoint->m_bActive)
		return TRUE;

	AuAutoLock lock(pcsCharacter->m_Mutex);

	// 길드 마스터인지 확인한다.
	AgpdGuild *pcsGuild = m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (NULL == pcsGuild)
		return FALSE;

	if (FALSE == m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
		return FALSE;

	// 활성화 시키는데 필요한 아이템을 확인
	AgpdGrid *pcsInventory = m_pcsAgpmItem->GetInventory(pcsCharacter);
	if (NULL == pcsInventory)
		return FALSE;

	AgpdGridItem *pcsGridItem = m_pcsAgpmGrid->GetItemByTemplate(pcsInventory, AGPDGRID_ITEM_TYPE_ITEM, ARCHON_ITEM);
	if (NULL == pcsGridItem)
		return FALSE;

	// 아이템이 존재하면 아이템을 삭제후 포탈을 Active 시킨다.
	AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
	if (NULL == pcsItem)
		return FALSE;

	// 포털 활성화 그리고 아크로드 공성전 등록
	if (!m_pcsAgpmSiegeWar->AddArchlordCastleAttackGuild(pcsGuild))
	{
		ASSERT(!"AddArchlordCastleAttackGuild() is failed");
		return FALSE;
	}

	m_pcsAgpmItem->RemoveItem(pcsItem, TRUE);
	pcsTeleportPoint->m_bActive = TRUE;

	return TRUE;
}

void AgpmEventTeleport::DungeonTeleportDisable()
{
	// 비밀의 던젼용으로 활성화된 던젼을 막는다.
	INT32	lIndex = 0;
	AgpdTeleportPoint **ppcsTeleportPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	while (ppcsTeleportPoint && *ppcsTeleportPoint)
	{
		if ((AGPDTELEPORT_SPECIAL_TYPE_DUNGEON_TO_LANSPHERE == (*ppcsTeleportPoint)->m_eSpecialType))
			(*ppcsTeleportPoint)->m_bActive = FALSE;

		/*if (AGPDTELEPORT_SPECIAL_TYPE_CASTLE_TO_DUNGEON == (*ppcsTeleportPoint)->m_eSpecialType)
			(*ppcsTeleportPoint)->m_bActive = FALSE;*/

		ppcsTeleportPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	}	
}

void AgpmEventTeleport::ActiveTeleportCastleToDungeon()
{
	INT32	lIndex = 0;
	AgpdTeleportPoint **ppcsTeleportPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	while (ppcsTeleportPoint && *ppcsTeleportPoint)
	{
		/*if ((AGPDTELEPORT_SPECIAL_TYPE_CASTLE_TO_DUNGEON == (*ppcsTeleportPoint)->m_eSpecialType))
			(*ppcsTeleportPoint)->m_bActive = TRUE;*/

		ppcsTeleportPoint = (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	}	
}