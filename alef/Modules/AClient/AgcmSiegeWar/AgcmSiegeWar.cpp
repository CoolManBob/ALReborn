#include "AgcmSiegeWar.h"
#include "AuTimeStamp.h"

AgcmSiegeWar::AgcmSiegeWar()
{
	SetModuleName("AgcmSiegeWar");

	ZeroMemory(m_pCatapultStatusTexture, sizeof(m_pCatapultStatusTexture));
	ZeroMemory(m_pAtkResTowerStatusTexture, sizeof(m_pAtkResTowerStatusTexture));
	ZeroMemory(m_pCastleOwnerTexture, sizeof(m_pCastleOwnerTexture));
}

AgcmSiegeWar::~AgcmSiegeWar()
{
}

BOOL AgcmSiegeWar::OnAddModule()
{
	m_pcsAgpmCharacter	= (AgpmCharacter	*)	GetModule("AgpmCharacter"	);
	m_pcsAgpmSiegeWar	= (AgpmSiegeWar		*)	GetModule("AgpmSiegeWar"	);
	m_pcsAgpmGuild		= (AgpmGuild		*)	GetModule("AgpmGuild"		);
	m_pcsAgcmCharacter	= (AgcmCharacter	*)	GetModule("AgcmCharacter"	);
	m_pcsAgcmMinimap	= (AgcmMinimap		*)	GetModule("AgcmMinimap"		);

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmSiegeWar ||
		!m_pcsAgpmGuild ||
		//!m_pcsAgcmMinimap	 //이놈은 없어도 됌..
		!m_pcsAgcmCharacter)
		return FALSE;

	if( m_pcsAgcmMinimap )
	{
		// 미니맵이 있을때만 콜백 등록.
		if( !m_pcsAgpmCharacter->SetCallbackInitChar			( CBInitCharacter		, this ) ) return FALSE;
		if( !m_pcsAgpmCharacter->SetCallbackUpdateActionStatus	( CBUpdateCharStatus	, this ) ) return FALSE;
		if( !m_pcsAgpmCharacter->SetCallbackRemoveChar			( CBRemoveCharacter		, this ) ) return FALSE;
	}

	if(!m_pcsAgpmSiegeWar->SetCallbackUpdateCastleInfo(CBRemoveNPCGuildCharacter,	this))
		return FALSE;

	return TRUE;
}

BOOL AgcmSiegeWar::SendNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSiegeWarTime)
{
	if (!pcsSiegeWar || ullNextSiegeWarTime == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketSetNextSiegeWarTime(pcsSiegeWar, ullNextSiegeWarTime, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendDefenseApplication(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketDefenseApplication(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendCancelDefenseApplication(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketCancelDefenseApplication(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendAttackApplication(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketAttackApplication(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendCancelAttackApplication(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketCancelAttackApplication(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRequestAttackApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRequestAttackApplGuildList(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), nPage, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRequestDefenseApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRequestDefenseApplGuildList(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), nPage, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendSelectDefenseGuild(AgpdSiegeWar *pcsSiegeWar, CHAR *szGuildName)
{
	if (!pcsSiegeWar || !szGuildName || !szGuildName[0])
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketSelectDefenseGuild(pcsSiegeWar, szGuildName, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRequestDefenseGuildList(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRequestDefenseGuildList(pcsSiegeWar, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRequestOpenAttackObject(AgpdCharacter *pcsTarget)
{
	if (!pcsTarget)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRequestOpenAttackObject(m_pcsAgcmCharacter->GetSelfCID(), pcsTarget->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRequestUseAttackObject(AgpdCharacter *pcsTarget)
{
	if (!pcsTarget)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRequestUseAttackObject(m_pcsAgcmCharacter->GetSelfCID(), pcsTarget->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRequestRepairAttackObject(AgpdCharacter *pcsTarget)
{
	if (!pcsTarget)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRequestRepairAttackObject(m_pcsAgcmCharacter->GetSelfCID(), pcsTarget->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendRemoveItemToAttackObject(AgpdCharacter* pcsTarget, AgpdItem* pcsItem)
{
	if (!pcsTarget || !pcsItem)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketRemoveItemToAttackObject(m_pcsAgcmCharacter->GetSelfCID(), pcsTarget->m_lID, pcsItem->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendAddItemToAttackObject(AgpdCharacter *pcsTarget, AgpdItem *pcsItem)
{
	if (!pcsTarget || !pcsItem)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketAddItemToAttackObject(m_pcsAgcmCharacter->GetSelfCID(), pcsTarget->m_lID, pcsItem->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::SendCarveASeal(AgpdCharacter *pcsTarget)
{
	if (!pcsTarget)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketCarveASeal(m_pcsAgcmCharacter->GetSelfCID(), pcsTarget->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmSiegeWar::ClickCharacter(AgpdCharacter *pcsTarget)
{
	if (!pcsTarget)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsTarget);

	switch (eMonsterType) {
		case AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER:
		case AGPD_SIEGE_MONSTER_CATAPULT:
			{
				// 공성길드인지 본다.
				if (!m_pcsAgpmSiegeWar->IsAttackGuild(m_pcsAgcmCharacter->GetSelfCharacter(), m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsTarget)))
					return FALSE;

				// 서버로 클릭정보를 보낸다.
				// 보낸후 결과에 따라 UI를 열어주던가 한다.
				return SendRequestOpenAttackObject(pcsTarget);

				/*
				// 공성중일때 투석기나, 공성부활탑을 클릭한 경우 UI를 띄워준다.
				AgpmSiegeResult	eCheckResult	= m_pcsAgpmSiegeWar->CheckOpenAttackObjectEvent(m_pcsAgcmCharacter->GetSelfCharacter(), pcsTarget);
				if (eCheckResult == AGPMSIEGE_RESULT_OPEN_ATTACK_OBJECT_EVENT)
					// 서버로 정보를 보낸다.
					return SendRequestOpenAttackObject(pcsTarget);
				else
					EnumCallback(AGCMSIEGEWAR_CB_OPEN_CHECK_RESULT, pcsTarget, &eCheckResult);
				*/

				/*
				if (m_pcsAgpmSiegeWar->CheckValidUseAttackObject(m_pcsAgcmCharacter->GetSelfCharacter(), pcsTarget) == AGPMSIEGE_RESULT_SUCCESS)
					return SendRequestUseAttackObject(pcsTarget);

				if (m_pcsAgpmSiegeWar->CheckValidRepairAttackObject(m_pcsAgcmCharacter->GetSelfCharacter(), pcsTarget) == AGPMSIEGE_RESULT_ALREADY_REPAIRED_ATTACK_OBJECT)
					return SendRequestRepairAttackObject(pcsTarget);
				*/
			}
			break;

		case AGPD_SIEGE_MONSTER_THRONE:
			{
				// 봉인성좌의 보호막이 제거된 후 길드마스터가 클릭으로 각인을 시작한다.
				if (m_pcsAgpmSiegeWar->CheckCarveASeal(m_pcsAgcmCharacter->GetSelfCharacter(), pcsTarget))
				{
					// 서버로 각인 시작을 알린다.
					return SendCarveASeal(pcsTarget);
				}
				else
					return FALSE;
			}
			break;
	}

	return FALSE;
}

BOOL AgcmSiegeWar::SetCatapultUseTexture(RwTexture *pTexture)
{
	if (!pTexture)
		return FALSE;

	m_pCatapultStatusTexture[0]	= pTexture;

	return TRUE;
}

BOOL AgcmSiegeWar::SetCatapultRepairTexture(RwTexture *pTexture)
{
	if (!pTexture)
		return FALSE;

	m_pCatapultStatusTexture[1]	= pTexture;

	return TRUE;
}

BOOL AgcmSiegeWar::SetAtkResTowerUseTexture(RwTexture *pTexture)
{
	if (!pTexture)
		return FALSE;

	m_pAtkResTowerStatusTexture[0]	= pTexture;

	return TRUE;
}

BOOL AgcmSiegeWar::SetAtkResTowerRepairTexture(RwTexture *pTexture)
{
	if (!pTexture)
		return FALSE;

	m_pAtkResTowerStatusTexture[1]	= pTexture;

	return TRUE;
}

BOOL AgcmSiegeWar::SetCastleOwnerTexture(RwTexture *pTexture, INT32 lIndex)
{
	if (!pTexture || lIndex < 0 || lIndex >= AGPMSIEGEWAR_MAX_CASTLE)
		return FALSE;

	m_pCastleOwnerTexture[lIndex]	= pTexture;

	return TRUE;
}

RwTexture* AgcmSiegeWar::GetCatapultUseTexture()
{
	return m_pCatapultStatusTexture[0];
}

RwTexture* AgcmSiegeWar::GetCatapultRepairTexture()
{
	return m_pCatapultStatusTexture[1];
}

RwTexture* AgcmSiegeWar::GetAtkResTowerUseTexture()
{
	return m_pAtkResTowerStatusTexture[0];
}

RwTexture* AgcmSiegeWar::GetAtkResTowerRepairTexture()
{
	return m_pAtkResTowerStatusTexture[1];
}

RwTexture* AgcmSiegeWar::GetCastleOwnerTextureTexture(INT32 lIndex)
{
	if (lIndex < 0 || lIndex >= AGPMSIEGEWAR_MAX_CASTLE)
		return NULL;

	return m_pCastleOwnerTexture[lIndex];
}

// 공격길드는 1, 수비길드는 2, 그외 놈들이나 에러인 경우는 (-1)을 리턴
INT32 AgcmSiegeWar::GetSiegeWarGuildType(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return (-1);

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	if (!m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
		return (-1);

	if (m_pcsAgpmSiegeWar->IsAttackGuild(pcsCharacter, pcsSiegeWar))
		return 1;
	else if (m_pcsAgpmSiegeWar->IsDefenseGuild(pcsCharacter, pcsSiegeWar))
		return 2;

	return (-1);
}

BOOL		AgcmSiegeWar::CBInitCharacter		(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSiegeWar	*pThis			=	(AgcmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	=	(AgpdCharacter *)	pData;

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);

	if(	eMonsterType == AGPD_SIEGE_MONSTER_GUARD_TOWER					||
		eMonsterType == AGPD_SIEGE_MONSTER_INNER_GATE					||
		eMonsterType == AGPD_SIEGE_MONSTER_OUTER_GATE					||
		eMonsterType == AGPD_SIEGE_MONSTER_LIFE_TOWER					||
		eMonsterType == AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER	||
		eMonsterType == AGPD_SIEGE_MONSTER_THRONE						||
		eMonsterType == AGPD_SIEGE_MONSTER_ARCHON_EYES					||
		eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER	||
		eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT						)
	{
		//들어가 있는지 확인차 검사
		AgcmSiegeWar::SiegeWarDisplayInfo * pInfo = pThis->GetSiegeWarDisplayInfo( pcsCharacter->m_lID );
		ASSERT( pInfo == NULL );
		if( !pInfo )
		{
			// 없어야 정상이쥐.

			INT32	nType;
			if( pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
				nType = AgcmMinimap::MPInfo::MPI_SIEGEDESTROIED;
			else
				nType = AgcmMinimap::MPInfo::MPI_SIEGENORMAL;

			// 미니맵에 표시하지 않는다
			if( !pcsCharacter->m_bNPCDisplayForMap )
			{
				INT32 nMinimapIndex = pThis->m_pcsAgcmMinimap->AddPoint( nType , pcsCharacter->m_szID , &pcsCharacter->m_stPos );
				pThis->AddSiegeWarDisplayInfo( pcsCharacter->m_lID , nMinimapIndex );
			}
		}
	}

	return TRUE;
}
BOOL AgcmSiegeWar::CBUpdateCharStatus	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSiegeWar	*pThis	= (AgcmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);

	if(	eMonsterType == AGPD_SIEGE_MONSTER_GUARD_TOWER					||
		eMonsterType == AGPD_SIEGE_MONSTER_INNER_GATE					||
		eMonsterType == AGPD_SIEGE_MONSTER_OUTER_GATE					||
		eMonsterType == AGPD_SIEGE_MONSTER_LIFE_TOWER					||
		eMonsterType == AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER	||
		eMonsterType == AGPD_SIEGE_MONSTER_THRONE						||
		eMonsterType == AGPD_SIEGE_MONSTER_ARCHON_EYES					||
		eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER	||
		eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT						)
	{
		//들어가 있는지 확인차 검사
		AgcmSiegeWar::SiegeWarDisplayInfo * pInfo = pThis->GetSiegeWarDisplayInfo( pcsCharacter->m_lID );
		if( pInfo )
		{
			INT32	nType;
			if( pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
				nType = AgcmMinimap::MPInfo::MPI_SIEGEDESTROIED;
			else
				nType = AgcmMinimap::MPInfo::MPI_SIEGENORMAL;

			AgcmMinimap::MPInfo *	pMPInfo = pThis->m_pcsAgcmMinimap->GetPointInfo( pInfo->nMinimapIndex );

			if( pMPInfo )
			{
				pMPInfo->nType = nType;
			}
		}
	}
	/*
	else if(eMonsterType == AGPD_SIEGE_WAR_STATUS_TIME_OVER)
	{
		//. 공성 종료 시점에서 NPC Guild Character를 일괄적으로 지운다.	
		INT32			lIndex	= 0;
		AgpdCharacter	*pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);

		//. character list를 돌다가 꼬일수 있다는 magoja아저씨의 조언으로
		//. 임시로 vector에 넣었다가 한번에 지움.
		std::list<AgpdCharacter*>	_vecNPCGuildCharacter;

		while (pcsCharacter)
		{
			//. NPC Guild인 몬스터의 경우
			if (pThis->m_pcsAgpmCharacter->IsMonster(pcsCharacter) &&
				pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter))
			{
				_vecNPCGuildCharacter.push_back(pcsCharacter);
			}

			pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		}

		std::list<AgpdCharacter*>::iterator it = _vecNPCGuildCharacter.begin();
		while(it != _vecNPCGuildCharacter.end())
		{
			 pThis->m_pcsAgpmCharacter->RemoveCharacter( (*it)->m_lID);
		}
	}
	*/

	return TRUE;
}

BOOL		AgcmSiegeWar::CBRemoveCharacter	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSiegeWar	*pThis	= (AgcmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);

	if(	eMonsterType == AGPD_SIEGE_MONSTER_GUARD_TOWER					||
		eMonsterType == AGPD_SIEGE_MONSTER_INNER_GATE					||
		eMonsterType == AGPD_SIEGE_MONSTER_OUTER_GATE					||
		eMonsterType == AGPD_SIEGE_MONSTER_LIFE_TOWER					||
		eMonsterType == AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER	||
		eMonsterType == AGPD_SIEGE_MONSTER_THRONE						||
		eMonsterType == AGPD_SIEGE_MONSTER_ARCHON_EYES					||
		eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER	||
		eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT						)
	{
		AgcmSiegeWar::SiegeWarDisplayInfo * pInfo = pThis->GetSiegeWarDisplayInfo( pcsCharacter->m_lID );
		if( pInfo )
		{
			pThis->m_pcsAgcmMinimap->RemovePoint( pInfo->nMinimapIndex );
			pThis->RemoveSiegeWarDisplayInfo( pcsCharacter->m_lID );
		}
	}

	return TRUE;
}

BOOL		AgcmSiegeWar::CBRemoveNPCGuildCharacter	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSiegeWar	*pThis	= (AgcmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)pData;

	if(!pThis || !pcsSiegeWar)
		return FALSE;

	if(pcsSiegeWar->m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_TIME_OVER)
	{
		//. 공성 종료 시점에서 NPC Guild Character를 일괄적으로 지운다.	
		INT32			lIndex	= 0;
		AgpdCharacter	*pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);

		//. character list를 돌다가 꼬일수 있다는 magoja아저씨의 조언으로
		//. 임시로 vector에 넣었다가 한번에 지움.
		std::list<AgpdCharacter*>	_vecNPCGuildCharacter;

		while (pcsCharacter)
		{
			//. NPC Guild인 몬스터의 경우
			if (pThis->m_pcsAgpmCharacter->IsMonster(pcsCharacter))
			{
				AgpdGuildADChar* pcsAttachedGuild  = pThis->m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
				
				if(pcsAttachedGuild && strlen(pcsAttachedGuild->m_szGuildID) > 0)
					_vecNPCGuildCharacter.push_back(pcsCharacter);
			}

			pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		}

		std::list<AgpdCharacter*>::iterator it = _vecNPCGuildCharacter.begin();
		while(it != _vecNPCGuildCharacter.end())
		{
			 pThis->m_pcsAgpmCharacter->RemoveCharacter( (*it)->m_lID);
			 it++;
		}
	}

	return TRUE;
}


AgcmSiegeWar::SiegeWarDisplayInfo *	AgcmSiegeWar::GetSiegeWarDisplayInfo( INT32 nCID )
{
	for( vector< AgcmSiegeWar::SiegeWarDisplayInfo >::iterator iter = m_vecSiegeWarDisplayInfo.begin();
			iter != m_vecSiegeWarDisplayInfo.end() ;
			iter++ )
	{
		AgcmSiegeWar::SiegeWarDisplayInfo * pInfo = &( *iter );

		if( pInfo->nCID == nCID ) return pInfo;
	}

	return NULL;
}

BOOL	AgcmSiegeWar::AddSiegeWarDisplayInfo( INT32 nCID , INT32 nMinimapIndex )
{
	AgcmSiegeWar::SiegeWarDisplayInfo	stInfo;
	stInfo.nCID = nCID;
	stInfo.nMinimapIndex = nMinimapIndex;

	m_vecSiegeWarDisplayInfo.push_back( stInfo );

	return TRUE;
}

BOOL	AgcmSiegeWar::RemoveSiegeWarDisplayInfo( INT32 nCID )
{
	for( vector< AgcmSiegeWar::SiegeWarDisplayInfo >::iterator iter = m_vecSiegeWarDisplayInfo.begin();
			iter != m_vecSiegeWarDisplayInfo.end() ;
			iter++ )
	{
		AgcmSiegeWar::SiegeWarDisplayInfo * pInfo = &( *iter );

		if( pInfo->nCID == nCID )
		{
			m_vecSiegeWarDisplayInfo.erase( iter );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	AgcmSiegeWar::SetCallbackOpenCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSIEGEWAR_CB_OPEN_CHECK_RESULT, pfCallback, pClass);
}