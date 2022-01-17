#include "AgpmSiegeWar.h"
#include "AuTimeStamp.h"

BOOL AgpmSiegeWar::CheckGuildRequirement(AgpdGuild *pcsGuild)
{
	if (!pcsGuild)
		return FALSE;

	AuAutoLock	lock(pcsGuild->m_Mutex);
	if (!lock.Result()) return FALSE;

	// 2007.09.03. steeple
	// 아크로드 길드는 신청할 수 없다.
	if (m_pcsAgpmCharacter->IsArchlord(pcsGuild->m_szMasterID))
		return FALSE;

	if (m_pcsAgpmConfig &&
		m_pcsAgpmConfig->IsTestServer())
	{
		return TRUE;
	}

	/*
	// 테섭전용
	// 길드 인원 10인 이상.
	if (m_pcsAgpmGuild->GetAllMemberCount(pcsGuild) < 10)
		return FALSE;
	*/

	// 길드 인원 20인 이상.
	if (m_pcsAgpmGuild->GetAllMemberCount(pcsGuild) < AGPMSIEGEWAR_GUILD_MEMBER_COUNT_REQUIREMENT)
		return FALSE;
	
	//길드 생성 후 14일 이상
	UINT32	ulCurrentTime	= AuTimeStamp::GetCurrentTimeStamp();
	if (ulCurrentTime - pcsGuild->m_lCreationDate < AGPMSIEGEWAR_GUILD_CREATE_TIME_REQUIREMENT)
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::CheckApplicationRequirement(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return FALSE;

	// 길마만 신청할 수 있다.
	if (!m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar ||
		pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
		return FALSE;

	if (!CheckGuildRequirement(pcsGuild))
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::IsAlreadyApplGuild(AgpdGuild *pcsGuild, BOOL bIsArchlordCastle)
{
	if (!pcsGuild)
		return FALSE;

	AgpdSiegeWar *pAgpdSiegeWarArchlord = GetArchlordCastle();

	if (bIsArchlordCastle)
	{
		if (pAgpdSiegeWarArchlord->m_csAttackApplicationGuild.GetObject(pcsGuild->m_szID) ||
			pAgpdSiegeWarArchlord->m_csAttackGuild.GetObject(pcsGuild->m_szID) ||
			pAgpdSiegeWarArchlord->m_csDefenseApplicationGuild.GetObject(pcsGuild->m_szID) ||
			pAgpdSiegeWarArchlord->m_csDefenseGuild.GetObject(pcsGuild->m_szID))		
			return TRUE;
	}
	else
	{
		for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
		{
			if (pAgpdSiegeWarArchlord == &(m_csSiegeWarInfo[i]))
				continue;
			
			// if owner guild(defense guild)
			if (strcmp(m_csSiegeWarInfo[i].m_strOwnerGuildName.GetBuffer(), pcsGuild->m_szID) == 0)
				return TRUE;

			if (m_csSiegeWarInfo[i].m_csAttackApplicationGuild.GetObject(pcsGuild->m_szID) ||
				m_csSiegeWarInfo[i].m_csAttackGuild.GetObject(pcsGuild->m_szID) ||
				m_csSiegeWarInfo[i].m_csDefenseApplicationGuild.GetObject(pcsGuild->m_szID) ||
				m_csSiegeWarInfo[i].m_csDefenseGuild.GetObject(pcsGuild->m_szID))
				return TRUE;
		}
	}
	return FALSE;
}

AgpmSiegeResult AgpmSiegeWar::AddDefenseApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild)
{
	if (!pcsSiegeWar || !pcsGuild)
		return AGPMSIEGE_RESULT_FAILED;

	if (pcsSiegeWar->m_strOwnerGuildName.IsEmpty())
		return AGPMSIEGE_RESULT_FAILED;

	AuAutoLock Lock(pcsSiegeWar->m_ApDefenseGuildSortList.m_Mutex);
	if (!Lock.Result()) return AGPMSIEGE_RESULT_FAILED;

	// 수성길드가 정해졌단 얘기
	//if (pcsSiegeWar->m_csDefenseGuild.GetObjectCount() > 0)
	//	return AGPMSIEGE_RESULT_ALREADY_SELECTED;

	if (IsAlreadyApplGuild(pcsGuild))
		return AGPMSIEGE_RESULT_ALREADY_APPLICATION;		// 이미 신청한 길드

	// 리스트가 AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD 수 만큼 꽉찼음
	if (pcsSiegeWar->m_csDefenseApplicationGuild.GetObjectCount() >= AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD )
		return AGPMSIEGE_RESULT_REGISTRATION_EXCEEDED;

	// SORT 리스트가 AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD 수 만큼 꽉찼음
	if (pcsSiegeWar->m_ApDefenseGuildSortList.SizeSortList() >= AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD )
		return AGPMSIEGE_RESULT_REGISTRATION_EXCEEDED;

	// 리스트 추가
	if (!pcsSiegeWar->m_csDefenseApplicationGuild.AddObject((PVOID) &pcsGuild, pcsGuild->m_szID))
		return AGPMSIEGE_RESULT_FAILED;

	// SORT 리스트 추가
	if ( FALSE == pcsSiegeWar->m_ApDefenseGuildSortList.Insert(pcsGuild) )
		return AGPMSIEGE_RESULT_FAILED;

	// SortDefenseGuild(pcsSiegeWar); // 신청순으로 리스트 출력하기 위해 정렬하지 않음 [ 2011-06-17 silvermoo ]

	return AGPMSIEGE_RESULT_DEFENSE_APPL_SUCCESS;
}

AgpmSiegeResult AgpmSiegeWar::RemoveDefenseApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild)
{
	if (!pcsSiegeWar || !pcsGuild)
		return AGPMSIEGE_RESULT_FAILED;

	AuAutoLock Lock(pcsSiegeWar->m_ApDefenseGuildSortList.m_Mutex);
	if (!Lock.Result()) return AGPMSIEGE_RESULT_FAILED;

	if (!pcsSiegeWar->m_csDefenseApplicationGuild.RemoveObject(pcsGuild->m_szID))
		return AGPMSIEGE_RESULT_FAILED;

	if ( FALSE == pcsSiegeWar->m_ApDefenseGuildSortList.Remove(pcsGuild) )
		return AGPMSIEGE_RESULT_FAILED;

	// SortDefenseGuild(pcsSiegeWar); // 신청순으로 리스트 출력하기 위해 정렬하지 않음 [ 2011-06-17 silvermoo ]

	return AGPMSIEGE_RESULT_SUCCESS;
}

AgpmSiegeResult AgpmSiegeWar::AddAttackApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild)
{
	if (!pcsSiegeWar || !pcsGuild)
		return AGPMSIEGE_RESULT_FAILED;

	AuAutoLock Lock(pcsSiegeWar->m_ApAttackGuildSortList.m_Mutex);
	if (!Lock.Result()) return AGPMSIEGE_RESULT_FAILED;

	// 공성길드가 정해졌단 얘기
	//if (pcsSiegeWar->m_csAttackGuild.GetObjectCount() > 0)
	//	return AGPMSIEGE_RESULT_ALREADY_SELECTED;

	if (IsAlreadyApplGuild(pcsGuild, IsArchlordCastle(pcsSiegeWar)))
		return AGPMSIEGE_RESULT_ALREADY_APPLICATION;		// 이미 신청한 길드

	// 리스트가 AGPMSIEGEWAR_TOTAL_ATTACK_GUILD 수 만큼 꽉찼음
	if (pcsSiegeWar->m_csAttackApplicationGuild.GetObjectCount() >= AGPMSIEGEWAR_TOTAL_ATTACK_GUILD )
		return AGPMSIEGE_RESULT_REGISTRATION_EXCEEDED;

	// SORT 리스트가 AGPMSIEGEWAR_TOTAL_ATTACK_GUILD 수 만큼 꽉찼음
	if (pcsSiegeWar->m_ApAttackGuildSortList.SizeSortList() >= AGPMSIEGEWAR_TOTAL_ATTACK_GUILD )
		return AGPMSIEGE_RESULT_REGISTRATION_EXCEEDED;

	// 리스트 추가
	if (!pcsSiegeWar->m_csAttackApplicationGuild.AddObject((PVOID) &pcsGuild, pcsGuild->m_szID))
		return AGPMSIEGE_RESULT_FAILED;						

	// SORT 리스트 추가
	if ( FALSE == pcsSiegeWar->m_ApAttackGuildSortList.Insert(pcsGuild) )
		return AGPMSIEGE_RESULT_FAILED;

	// SortAttackGuild(pcsSiegeWar); // 신청순으로 리스트 출력하기 위해 정렬하지 않음 [ 2011-06-17 silvermoo ]

	return AGPMSIEGE_RESULT_ATTACK_APPL_SUCCESS;
}

AgpmSiegeResult AgpmSiegeWar::RemoveAttackApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild)
{
	if (!pcsSiegeWar || !pcsGuild)
		return AGPMSIEGE_RESULT_FAILED;

	AuAutoLock Lock(pcsSiegeWar->m_ApAttackGuildSortList.m_Mutex);
	if (!Lock.Result()) return AGPMSIEGE_RESULT_FAILED;

	if (!pcsSiegeWar->m_csAttackApplicationGuild.RemoveObject(pcsGuild->m_szID))
		return AGPMSIEGE_RESULT_FAILED;

	if ( FALSE == pcsSiegeWar->m_ApAttackGuildSortList.Remove(pcsGuild) )
		return AGPMSIEGE_RESULT_FAILED;

	// SortAttackGuild(pcsSiegeWar); // 신청순으로 리스트 출력하기 위해 정렬하지 않음 [ 2011-06-17 silvermoo ]

	return AGPMSIEGE_RESULT_SUCCESS;
}

BOOL AgpmSiegeWar::SortAttackGuild(AgpdSiegeWar *pcsSiegeWar)
{
	// 현재 공성전 신청중인 모든 길드를 적당한 방법에 의해 소트 시킨다.
	// 소트 기준은 점수가 높은놈을 시작으로 내림차순이다.

	ResetAttackGuildTotalPoint(pcsSiegeWar);

	pcsSiegeWar->m_ApAttackGuildSortList.Sort(CompareGuildMemberDesc());
	CalcGuilPoint(AGPMSIEGE_GUILD_POINT_TYPE_MEMBER_COUNT, AGPMSIEGEWAR_GUILD_POINT_MEMBER_COUNT, &pcsSiegeWar->m_ApAttackGuildSortList);

	pcsSiegeWar->m_ApAttackGuildSortList.Sort(CompareGuildWarPointDesc());
	CalcGuilPoint(AGPMSIEGE_GUILD_POINT_TYPE_BATTLE, AGPMSIEGEWAR_GUILD_POINT_BATTLE, &pcsSiegeWar->m_ApAttackGuildSortList);

	pcsSiegeWar->m_ApAttackGuildSortList.Sort(CompareGuildDurationDesc());
	CalcGuilPoint(AGPMSIEGE_GUILD_POINT_TYPE_DURATION, AGPMSIEGEWAR_GUILD_POINT_DURATION, &pcsSiegeWar->m_ApAttackGuildSortList);

	pcsSiegeWar->m_ApAttackGuildSortList.Sort(CompareGuildTotalPointDesc());

	//////////////////////////////////////////////////////////////////////////
	//
	CHAR strBuff[1024];
	memset(strBuff, 0, sizeof(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "SortAttackGuild");
	AuLogFile_s("LOG\\SiegeWarApplication", strBuff);

	for(AgpdGuild* pcsGuild = pcsSiegeWar->m_ApAttackGuildSortList.GetHead();
		(pcsGuild && !pcsSiegeWar->m_ApAttackGuildSortList.IsEnd()); 
		pcsGuild = pcsSiegeWar->m_ApAttackGuildSortList.GetNext())
	{
		memset(strBuff, 0, sizeof(strBuff));
		sprintf_s(strBuff, sizeof(strBuff), "GuildID = [%s], Point = [%d]", pcsGuild->m_szID, GetAttachGuildData(pcsGuild)->m_ulGuildTotalPoint);
		AuLogFile_s("LOG\\SiegeWarApplication", strBuff);
	}

	return TRUE;
}

BOOL AgpmSiegeWar::SortDefenseGuild(AgpdSiegeWar *pcsSiegeWar)
{
	// 현재 공성전 신청중인 모든 길드를 적당한 방법에 의해 소트 시킨다.
	// 소트 기준은 점수가 높은놈을 시작으로 내림차순이다.

	ResetDefenseGuildTotalPoint(pcsSiegeWar);

	pcsSiegeWar->m_ApDefenseGuildSortList.Sort(CompareGuildMemberDesc());
	CalcGuilPoint(AGPMSIEGE_GUILD_POINT_TYPE_MEMBER_COUNT, AGPMSIEGEWAR_GUILD_POINT_MEMBER_COUNT, &pcsSiegeWar->m_ApDefenseGuildSortList);

	pcsSiegeWar->m_ApDefenseGuildSortList.Sort(CompareGuildWarPointDesc());
	CalcGuilPoint(AGPMSIEGE_GUILD_POINT_TYPE_BATTLE, AGPMSIEGEWAR_GUILD_POINT_BATTLE, &pcsSiegeWar->m_ApDefenseGuildSortList);

	pcsSiegeWar->m_ApDefenseGuildSortList.Sort(CompareGuildDurationDesc());
	CalcGuilPoint(AGPMSIEGE_GUILD_POINT_TYPE_DURATION, AGPMSIEGEWAR_GUILD_POINT_DURATION, &pcsSiegeWar->m_ApDefenseGuildSortList);

	pcsSiegeWar->m_ApDefenseGuildSortList.Sort(CompareGuildTotalPointDesc());

	//////////////////////////////////////////////////////////////////////////
	//
	CHAR strBuff[1024];
	memset(strBuff, 0, sizeof(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "SortDefenseGuild");
	AuLogFile_s("LOG\\SiegeWarApplication", strBuff);

	for(AgpdGuild* pcsGuild = pcsSiegeWar->m_ApDefenseGuildSortList.GetHead();
		(pcsGuild && !pcsSiegeWar->m_ApDefenseGuildSortList.IsEnd()); 
		pcsGuild = pcsSiegeWar->m_ApDefenseGuildSortList.GetNext())
	{
		memset(strBuff, 0, sizeof(strBuff));
		sprintf_s(strBuff, sizeof(strBuff), "GuildID = [%s], Point = [%d]", pcsGuild->m_szID, GetAttachGuildData(pcsGuild)->m_ulGuildTotalPoint);
		AuLogFile_s("LOG\\SiegeWarApplication", strBuff);
	}

	return TRUE;
}

BOOL AgpmSiegeWar::ResetAttackGuildTotalPoint(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	return ResetGuildTotalPoint(&pcsSiegeWar->m_csAttackApplicationGuild);
}

BOOL AgpmSiegeWar::ResetDefenseGuildTotalPoint(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	return ResetGuildTotalPoint(&pcsSiegeWar->m_csDefenseApplicationGuild);
}

BOOL AgpmSiegeWar::ResetGuildTotalPoint(ApAdmin *pcsAdmin)
{
	if (!pcsAdmin)
		return FALSE;

	INT32		lIndex		= 0;
	AgpdGuild	**ppcsGuild	= (AgpdGuild **) pcsAdmin->GetObjectSequence(&lIndex);

	while (ppcsGuild)
	{
		if (*ppcsGuild)
			GetAttachGuildData((*ppcsGuild))->m_ulGuildTotalPoint	= 0;

		ppcsGuild	= (AgpdGuild **) pcsAdmin->GetObjectSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CalcGuilPoint(AgpmSiegeGuildPointType eGuildPointType, INT32 lMaxPoint, ApSort <AgpdGuild *> *pcsSortList)
{
	if (eGuildPointType <= AGPMSIEGE_GUILD_POINT_TYPE_NONE ||
		eGuildPointType >= AGPMSIEGE_GUILD_POINT_TYPE_MAX ||
		lMaxPoint < 1 || !pcsSortList)
		return FALSE;

	AgpdGuild *pcsPrevGuild = NULL;
	AgpdGuild *pcsGuild = pcsSortList->GetHead();

	while(pcsGuild && !pcsSortList->IsEnd())
	{
		INT32 lAddedGuildPoint = 0;
		if(eGuildPointType == AGPMSIEGE_GUILD_POINT_TYPE_DURATION)
		{
			lAddedGuildPoint = 1;
			INT32 lGuildDuration = GetGuildPoint(eGuildPointType, pcsGuild);

			lAddedGuildPoint += 3*lGuildDuration/(7*AUTIMESTAMP_DAY);
		}
		else
			lAddedGuildPoint = GetGuildPoint(eGuildPointType, pcsGuild);

		GetAttachGuildData(pcsGuild)->m_ulGuildTotalPoint += lAddedGuildPoint;

		pcsPrevGuild = pcsGuild;
		pcsGuild = pcsSortList->GetNext();
	}

	return TRUE;
}

INT32 AgpmSiegeWar::GetGuildPoint(AgpmSiegeGuildPointType eGuildPointType, AgpdGuild *pcsGuild)
{
	if (eGuildPointType <= AGPMSIEGE_GUILD_POINT_TYPE_NONE ||
		eGuildPointType >= AGPMSIEGE_GUILD_POINT_TYPE_MAX ||
		!pcsGuild)
		return 0;

	switch (eGuildPointType) {
		case AGPMSIEGE_GUILD_POINT_TYPE_MEMBER_COUNT:
			return pcsGuild->m_pMemberList->GetObjectCount();
			break;

		case AGPMSIEGE_GUILD_POINT_TYPE_BATTLE:
			return GetGuildWarPoint(pcsGuild);
			break;

		case AGPMSIEGE_GUILD_POINT_TYPE_DURATION:
			return GetGuildDuration(pcsGuild);
			break;

		case AGPMSIEGE_GUILD_POINT_TYPE_ARCHON_SCROLL_COUNT:
			return GetGuildArchon(pcsGuild);
			break;

		case AGPMSIEGE_GUILD_POINT_TYPE_MASTER_LEVEL:
			return GetGuildMasterLevel(pcsGuild);
			break;
	}

	return 0;
}

BOOL AgpmSiegeWar::IsSamePoint(AgpmSiegeGuildPointType eGuildPointType, AgpdGuild *pcsGuildA, AgpdGuild *pcsGuildB)
{
	if (eGuildPointType <= AGPMSIEGE_GUILD_POINT_TYPE_NONE ||
		eGuildPointType >= AGPMSIEGE_GUILD_POINT_TYPE_MAX ||
		!pcsGuildA || !pcsGuildB)
		return FALSE;

	if (GetGuildPoint(eGuildPointType, pcsGuildA) == GetGuildPoint(eGuildPointType, pcsGuildB))
		return TRUE;

	return FALSE;
}

BOOL AgpmSiegeWar::ClearAttackGuild(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	return pcsSiegeWar->m_csAttackGuild.RemoveObjectAll();
}

BOOL AgpmSiegeWar::ClearDefenseGuild(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	return pcsSiegeWar->m_csDefenseGuild.RemoveObjectAll();
}

BOOL AgpmSiegeWar::ClearAttackApplGuild(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock Lock(pcsSiegeWar->m_ApAttackGuildSortList.m_Mutex);
	if (!Lock.Result()) return FALSE;

	pcsSiegeWar->m_ApAttackGuildSortList.Clear();

	return pcsSiegeWar->m_csAttackApplicationGuild.RemoveObjectAll();
}

BOOL AgpmSiegeWar::ClearDefenseApplGuild(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock Lock(pcsSiegeWar->m_ApDefenseGuildSortList.m_Mutex);
	if (!Lock.Result()) return FALSE;

	pcsSiegeWar->m_ApDefenseGuildSortList.Clear();
	
	return pcsSiegeWar->m_csDefenseApplicationGuild.RemoveObjectAll();
}

AgpmSiegeResult AgpmSiegeWar::SetAttackGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsAttackGuild, BOOL bCheckCondition)
{
	if (!pcsSiegeWar || !pcsAttackGuild)
		return AGPMSIEGE_RESULT_FAILED;

	if (bCheckCondition)
	{
		// 현재 길드 선정 가능한 시간인지 확인한다.
		if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
			return AGPMSIEGE_RESULT_INVALID_TIME;

		// 넘어온 길드가 공성 신청을 한 길드인지 본다.
		if (!pcsSiegeWar->m_csAttackApplicationGuild.GetObject(pcsAttackGuild->m_szID))
			return AGPMSIEGE_RESULT_FAILED;

		// 현재 공성 선택된 길드가 제한보다 적은지 본다.
		if (pcsSiegeWar->m_csAttackGuild.GetObjectCount() >= AGPMSIEGEWAR_TOTAL_ATTACK_GUILD - 1)
			return AGPMSIEGE_RESULT_FAILED;
	}

	// 공성 길드리스트에 추가한다.
	if (!pcsSiegeWar->m_csAttackGuild.AddObject((PVOID) &pcsAttackGuild, pcsAttackGuild->m_szID))
		return AGPMSIEGE_RESULT_FAILED;

	return AGPMSIEGE_RESULT_SUCCESS;
}

AgpmSiegeResult AgpmSiegeWar::SetDefenseGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsDefenseGuild, BOOL bCheckCondition)
{
	if (!pcsSiegeWar || !pcsDefenseGuild)
		return AGPMSIEGE_RESULT_FAILED;

	if (bCheckCondition)
	{
		// 현재 길드 선정 가능한 시간인지 확인한다.
		if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
			return AGPMSIEGE_RESULT_INVALID_TIME;

		// 넘어온 길드가 수성 신청을 한 길드인지 본다.
		if (!pcsSiegeWar->m_csDefenseApplicationGuild.GetObject(pcsDefenseGuild->m_szID))
			return AGPMSIEGE_RESULT_FAILED;

		// 현재 수성 선택된 길드가 제한보다 적은지 본다.
		if (pcsSiegeWar->m_csDefenseGuild.GetObjectCount() >= AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD - 1)
			return AGPMSIEGE_RESULT_FAILED;
	}

	// 수성 길드리스트에 추가한다.
	if (!pcsSiegeWar->m_csDefenseGuild.AddObject((PVOID) &pcsDefenseGuild, pcsDefenseGuild->m_szID))
		return AGPMSIEGE_RESULT_FAILED;

	return AGPMSIEGE_RESULT_SET_DEFENSE_GUILD_SUCCESS;
}
