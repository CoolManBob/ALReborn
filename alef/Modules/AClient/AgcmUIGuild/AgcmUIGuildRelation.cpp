// AgcmUIGuildRelation.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2006. 07. 20.

#include "AgcmUIGuild.h"
#include "AgcmTextBoardMng.h"

BOOL AgcmUIGuild::CBJointRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szJointGuildID = static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID = static_cast<CHAR*>(ppvBuffer[1]);

	// 자기 길드의 오퍼레이션이 아니면 나간다.
	if(_tcscmp(szGuildID, pThis->m_szSelfGuildID) != 0)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;
	
	// 내가 마스터인지 확인. 아니면 패스.
	if(!pThis->m_pcsAgpmGuild->IsMaster(szGuildID, pcsSelfCharacter->m_szID))
		return FALSE;

	// 거부 상태인지 확인
	if(pcsSelfCharacter->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return FALSE;

	_tcscpy(pThis->m_szRelationGuildID, szJointGuildID);
	_tcscpy(pThis->m_szRelationMasterID, szMasterID);

	// 확인창을 열어준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenJointAcceptConfirmUI);
	return TRUE;
}

BOOL AgcmUIGuild::CBJoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	AgpdGuildRelationUnit* pUnit = static_cast<AgpdGuildRelationUnit*>(pCustData);

	// 자기 길드 오퍼레이션이 아니면 나간다.
	if(pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID) == FALSE)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	if(!pThis->m_pcsAgpmGuild->AddJointGuild(pcsGuild, pUnit->m_szGuildID, pUnit->m_ulDate, pUnit->m_cRelation))
		return FALSE;

	// sort 해준다.
	std::sort(pcsGuild->m_csRelation.m_pJointVector->begin(), pcsGuild->m_csRelation.m_pJointVector->end(), SortRelation());

	if(pThis->m_pcsAgcmTextBoard)
	{
		AgcmTextBoardMng::CB_PvPRefreshGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
											pThis->m_pcsAgcmTextBoard,
											pUnit->m_szGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBJointLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szGuildID = static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szJointGuildID = static_cast<CHAR*>(ppvBuffer[1]);
	CHAR* szNewLeader = static_cast<CHAR*>(ppvBuffer[2]);

	if(pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID) == FALSE)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	if(_tcscmp(szGuildID, szJointGuildID) == 0)
	{
		// 자기 길드가 탈퇴한 것이라면 클리어
        pThis->m_pcsAgpmGuild->ClearJointGuild(pcsGuild);

		if(pThis->m_pcsAgcmTextBoard)
		{
			// 근처에 있는 기존 연대 길드색을 다시 그려준다.
			GuildDetailIter iter = pThis->m_JointDetailVector.begin();
			while(iter != pThis->m_JointDetailVector.end())
			{
				AgcmTextBoardMng::CB_PvPRefreshGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
													pThis->m_pcsAgcmTextBoard,
													iter->m_szID);

				++iter;
			}
		}

		pThis->m_JointDetailVector.clear();
	}
	else
	{
		if(!pThis->m_pcsAgpmGuild->RemoveJointGuild(pcsGuild, szJointGuildID))
			return FALSE;

		GuildDetailIter iter = std::find_if(pThis->m_JointDetailVector.begin(),
											pThis->m_JointDetailVector.end(),
											FindGuildDetail(szJointGuildID));
		if(iter != pThis->m_JointDetailVector.end())
			pThis->m_JointDetailVector.erase(iter);

		if(pThis->m_pcsAgcmTextBoard)
		{
			AgcmTextBoardMng::CB_PvPRefreshGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
												pThis->m_pcsAgcmTextBoard,
												szJointGuildID);
		}

		// szNewLeader 세팅.
		if(szNewLeader && _tcslen(szNewLeader) > 0)
		{
			JointIter iterData = std::find_if(pcsGuild->m_csRelation.m_pJointVector->begin(),
												pcsGuild->m_csRelation.m_pJointVector->end(),
												FindRelation(szNewLeader));
			if(iterData != pcsGuild->m_csRelation.m_pJointVector->end())
				iterData->m_cRelation = (INT8)AGPMGUILD_RELATION_JOINT_LEADER;

			// sort 해준다.
			std::sort(pcsGuild->m_csRelation.m_pJointVector->begin(), pcsGuild->m_csRelation.m_pJointVector->end(), SortRelation());

			pThis->SortJointDetail();
		}
	}

	// UI 를 다시 그려준다.
	if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_JOINT)
	{
		pThis->SetGuildListToJointList();
		pThis->RefreshGuildList();
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBHostileRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szHostileGuildID = static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID = static_cast<CHAR*>(ppvBuffer[1]);

	// 자기 길드의 오퍼레이션이 아니면 나간다.
	if(_tcscmp(szGuildID, pThis->m_szSelfGuildID) != 0)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;
	
	// 내가 마스터인지 확인. 아니면 패스.
	if(!pThis->m_pcsAgpmGuild->IsMaster(szGuildID, pcsSelfCharacter->m_szID))
		return FALSE;

	// 거부 상태인지 확인
	if(pcsSelfCharacter->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return FALSE;

	_tcscpy(pThis->m_szRelationGuildID, szHostileGuildID);
	_tcscpy(pThis->m_szRelationMasterID, szMasterID);

	// 확인창을 열어준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenHostileAcceptConfirmUI);
	return TRUE;
}

BOOL AgcmUIGuild::CBHostile(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	AgpdGuildRelationUnit* pUnit = static_cast<AgpdGuildRelationUnit*>(pCustData);

	// 자기 길드 오퍼레이션이 아니면 나간다.
	if(pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID) == FALSE)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	if(!pThis->m_pcsAgpmGuild->AddHostileGuild(pcsGuild, pUnit->m_szGuildID, pUnit->m_ulDate))
		return FALSE;

	if(pThis->m_pcsAgcmTextBoard)
	{
		AgcmTextBoardMng::CB_PvPRefreshGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
											pThis->m_pcsAgcmTextBoard,
											pUnit->m_szGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBHostileLeaveRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szHostileGuildID = static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID = static_cast<CHAR*>(ppvBuffer[1]);

	// 자기 길드의 오퍼레이션이 아니면 나간다.
	if(_tcscmp(szGuildID, pThis->m_szSelfGuildID) != 0)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;
	
	// 내가 마스터인지 확인. 아니면 패스.
	if(!pThis->m_pcsAgpmGuild->IsMaster(szGuildID, pcsSelfCharacter->m_szID))
		return FALSE;

	_tcscpy(pThis->m_szRelationGuildID, szHostileGuildID);
	_tcscpy(pThis->m_szRelationMasterID, szMasterID);

	// 확인창을 열어준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenHostileLeaveAcceptConfirmUI);
	return TRUE;
}

BOOL AgcmUIGuild::CBHostileLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szGuildID = static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szHostileGuildID = static_cast<CHAR*>(ppvBuffer[1]);

	// 자기 길드 오퍼레이션이 아니면 나간다.
	if(pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID) == FALSE)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	if(!pThis->m_pcsAgpmGuild->RemoveHostileGuild(pcsGuild, szHostileGuildID))
		return FALSE;

	GuildDetailIter iter = std::find_if(pThis->m_HostileDetailVector.begin(),
										pThis->m_HostileDetailVector.end(),
										FindGuildDetail(szHostileGuildID));
	if(iter != pThis->m_HostileDetailVector.end())
		pThis->m_HostileDetailVector.erase(iter);

	// UI 를 다시 그려준다.
	if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE)
	{
		pThis->SetGuildListToHostileList();
		pThis->RefreshGuildList();
	}

	if(pThis->m_pcsAgcmTextBoard)
	{
		AgcmTextBoardMng::CB_PvPRefreshGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
											pThis->m_pcsAgcmTextBoard,
											szHostileGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnJointRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0)
			return FALSE;

		AgcdGuildList* pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
		if(!pcsAgcdGuildList)
			return FALSE;

		AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if(!pcsSelfCharacter)
			return FALSE;

		AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pThis->m_szSelfGuildID);
		if(!pcsGuild)
			return FALSE;

		AuAutoLock csLock(pcsGuild->m_Mutex);
		if (!csLock.Result()) return FALSE;

		if(_tcscmp(pcsGuild->m_szID, pcsAgcdGuildList->m_szID) == 0)
			return FALSE;

		if(!pThis->m_pcsAgpmGuild->IsMaster(pcsGuild, pcsSelfCharacter->m_szID))
			return FALSE;

		if(pThis->m_pcsAgpmGuild->IsJointGuild(pcsGuild, pcsAgcdGuildList->m_szID) ||
			pThis->m_pcsAgpmGuild->IsHostileGuild(pcsGuild, pcsAgcdGuildList->m_szID))
			return FALSE;

		pThis->m_pcsAgcmGuild->SendJointRequest(pcsAgcdGuildList->m_szID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnJointAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		// 수락보낸다.
		pThis->m_pcsAgcmGuild->SendJoint(pThis->m_szRelationGuildID);
	}
	else
	{
		// 거절 보낸다.
		pThis->m_pcsAgcmGuild->SendJointReject(pThis->m_szRelationGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnJointLeave(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		pThis->m_pcsAgcmGuild->SendJointLeave();
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnHostileRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0)
			return FALSE;

		AgcdGuildList* pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
		if(!pcsAgcdGuildList)
			return FALSE;

		AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if(!pcsSelfCharacter)
			return FALSE;

		AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pThis->m_szSelfGuildID);
		if(!pcsGuild)
			return FALSE;

		AuAutoLock csLock(pcsGuild->m_Mutex);
		if (!csLock.Result()) return FALSE;

		if(_tcscmp(pcsGuild->m_szID, pcsAgcdGuildList->m_szID) == 0)
			return FALSE;

		if(!pThis->m_pcsAgpmGuild->IsMaster(pcsGuild, pcsSelfCharacter->m_szID))
			return FALSE;

		if(pThis->m_pcsAgpmGuild->IsJointGuild(pcsGuild, pcsAgcdGuildList->m_szID) ||
			pThis->m_pcsAgpmGuild->IsHostileGuild(pcsGuild, pcsAgcdGuildList->m_szID))
			return FALSE;

		pThis->m_pcsAgcmGuild->SendHostileRequest(pcsAgcdGuildList->m_szID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnHostileAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		// 수락보낸다.
		pThis->m_pcsAgcmGuild->SendHostile(pThis->m_szRelationGuildID);
	}
	else
	{
		// 거절 보낸다.
		pThis->m_pcsAgcmGuild->SendHostileReject(pThis->m_szRelationGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnHostileLeaveRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		pThis->m_pcsAgcmGuild->SendHostileLeaveRequest(pThis->m_szRelationGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBEventReturnHostileLeaveAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		pThis->m_pcsAgcmGuild->SendHostileLeave(pThis->m_szRelationGuildID);
	}
	else
	{
		pThis->m_pcsAgcmGuild->SendHostileLeaveReject(pThis->m_szRelationGuildID);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBOpenJointList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	pThis->SetGuildListToJointList();
	pThis->RefreshGuildList();

	return TRUE;
}

BOOL AgcmUIGuild::CBOpenHostileList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	pThis->SetGuildListToHostileList();
	pThis->RefreshGuildList();

	return TRUE;
}

BOOL AgcmUIGuild::CBOnJointRequestBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	// 현재 페이지 모드에 따라서 연대신청/탈퇴 로 구분된다.
	if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_NORMAL)
	{
		// 연대신청.
		//

		if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0)
			return FALSE;

		// 세팅한다.
		AgcdGuildList* pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
		if(!pcsAgcdGuildList)
			return FALSE;

		_tcscpy(pThis->m_szRelationGuildID, pcsAgcdGuildList->m_szID);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenJointRequestConfirmUI);
	}
	else if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_JOINT)
	{
		// 연대탈퇴.
		//

		AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if(!pcsSelfCharacter)
			return FALSE;

		AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pThis->m_szSelfGuildID);
		if(!pcsGuild)
			return FALSE;

		AuAutoLock csLock(pcsGuild->m_Mutex);
		if (!csLock.Result()) return FALSE;

		if(!pThis->m_pcsAgpmGuild->IsMaster(pcsGuild, pcsSelfCharacter->m_szID))
			return FALSE;

		// Joint 길드가 없다.
		if(pThis->m_pcsAgpmGuild->GetJointGuildCount(pcsGuild) == 0)
			return FALSE;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenJointLeaveConfirmUI);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBOnHostileRequestBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	// 현재 페이지 모드에 따라서 적대신청/해제 로 구분된다.
	if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_NORMAL)
	{
		// 적대신청.
		//

		if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0)
			return FALSE;

		// 세팅한다.
		AgcdGuildList* pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
		if(!pcsAgcdGuildList)
			return FALSE;

		_tcscpy(pThis->m_szRelationGuildID, pcsAgcdGuildList->m_szID);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenHostileRequestConfirmUI);
	}
	else if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE)
	{
		// 적대해제
		//

		if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0)
			return FALSE;

		// 세팅한다.
		AgcdGuildList* pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
		if(!pcsAgcdGuildList)
			return FALSE;

		_tcscpy(pThis->m_szRelationGuildID, pcsAgcdGuildList->m_szID);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenHostileLeaveRequestConfirmUI);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBJointDetail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	PVOID pvPacket = pData;

	PVOID pvGuildListItemPacket[AGPMGUILD_MAX_VISIBLE_GUILD_LIST] = {0,};

	INT32 lGuildListCurrentPage = 0;
	INT16 nVisibleGuildCount = 0;
	INT32 lMaxGuildCount = 0;
	pThis->m_pcsAgpmGuild->m_csGuildListPacket.GetField(FALSE, pvPacket, 0,
						&lMaxGuildCount,
						&lGuildListCurrentPage,
						&nVisibleGuildCount,
						&pvGuildListItemPacket[0],
						&pvGuildListItemPacket[1],
						&pvGuildListItemPacket[2],
						&pvGuildListItemPacket[3],
						&pvGuildListItemPacket[4],
						&pvGuildListItemPacket[5],
						&pvGuildListItemPacket[6],
						&pvGuildListItemPacket[7],
						&pvGuildListItemPacket[8],
						&pvGuildListItemPacket[9],
						&pvGuildListItemPacket[10],
						&pvGuildListItemPacket[11],
						&pvGuildListItemPacket[12],
						&pvGuildListItemPacket[13],
						&pvGuildListItemPacket[14],
						&pvGuildListItemPacket[15],
						&pvGuildListItemPacket[16],
						&pvGuildListItemPacket[17],
						&pvGuildListItemPacket[18]
						);

	AuPacket& cGuildListItemPacket = pThis->m_pcsAgpmGuild->m_csGuildListItemPacket;
	for(int i = 0; i < nVisibleGuildCount && i < AGPMGUILD_MAX_JOINT_GUILD; ++i)
	{
		if( !pvGuildListItemPacket[i] )		break;

		CHAR *szID			= NULL;
		CHAR *szMasterID	= NULL;
		CHAR *szSubMasterID = NULL;
		INT32 lLevel = 0;
		INT32 lWinPoint = 0;
		INT32 lMemberCount = 0;
		INT32 lMaxMemberCount = 0;
		INT32 lGuildBattle = 0;
		INT32 lGuildMarkTID = 0;
		INT32 lGuildMarkColor = 0;
		BOOL bIsWinner = FALSE;
		cGuildListItemPacket.GetField( FALSE, pvGuildListItemPacket[i], 0, 
										&szID,
										&szMasterID,
										&szSubMasterID,
										&lLevel,
										&lWinPoint,
										&lMemberCount,
										&lMaxMemberCount,
										&lGuildBattle,
										&lGuildMarkTID,
										&lGuildMarkColor,
										&bIsWinner );

		AgcdGuildList stItem;
		memset( &stItem, 0, sizeof(stItem) );
		if( szID )			strncpy(stItem.m_szID, szID		, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		if( szMasterID )	strncpy(stItem.m_szMasterID		, szMasterID	, AGPACHARACTER_MAX_ID_STRING);
		if( szSubMasterID )	strncpy(stItem.m_szSubMasterID	, szSubMasterID , AGPACHARACTER_MAX_ID_STRING);

		stItem.m_lLevel				= lLevel;
		stItem.m_lWinPoint			= lWinPoint;
		stItem.m_lMemberCount		= lMemberCount;
		stItem.m_lMaxMemberCount	= lMaxMemberCount;
		stItem.m_lGuildBattle		= lGuildBattle;
		stItem.m_lGuildMarkTID		= lGuildMarkTID;
		stItem.m_lGuildMarkColor	= lGuildMarkColor;
		pThis->AddJointDetail(stItem);

		// 현재 화면이 Joint Mode 이면 UI 다시 그려준다.
		if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_JOINT)
		{
			pThis->SetGuildListToJointList();
			pThis->RefreshGuildList();
		}
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBHostileDetail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	PVOID pvPacket = pData;

	PVOID pvGuildListItemPacket[AGPMGUILD_MAX_VISIBLE_GUILD_LIST] = {0,};

	INT32 lGuildListCurrentPage = 0;
	INT16 nVisibleGuildCount = 0;
	INT32 lMaxGuildCount = 0;
	pThis->m_pcsAgpmGuild->m_csGuildListPacket.GetField(FALSE, pvPacket, 0,
						&lMaxGuildCount,
						&lGuildListCurrentPage,
						&nVisibleGuildCount,
						&pvGuildListItemPacket[0],
						&pvGuildListItemPacket[1],
						&pvGuildListItemPacket[2],
						&pvGuildListItemPacket[3],
						&pvGuildListItemPacket[4],
						&pvGuildListItemPacket[5],
						&pvGuildListItemPacket[6],
						&pvGuildListItemPacket[7],
						&pvGuildListItemPacket[8],
						&pvGuildListItemPacket[9],
						&pvGuildListItemPacket[10],
						&pvGuildListItemPacket[11],
						&pvGuildListItemPacket[12],
						&pvGuildListItemPacket[13],
						&pvGuildListItemPacket[14],
						&pvGuildListItemPacket[15],
						&pvGuildListItemPacket[16],
						&pvGuildListItemPacket[17],
						&pvGuildListItemPacket[18]
						);

	AuPacket& cGuildListItemPacket = pThis->m_pcsAgpmGuild->m_csGuildListItemPacket;
	for(int i = 0; i < nVisibleGuildCount && i < AGPMGUILD_MAX_JOINT_GUILD; ++i)
	{
		if( !pvGuildListItemPacket[i] )		break;

		CHAR *szID			= NULL;
		CHAR *szMasterID	= NULL;
		CHAR *szSubMasterID = NULL;
		INT32 lLevel = 0;
		INT32 lWinPoint = 0;
		INT32 lMemberCount = 0;
		INT32 lMaxMemberCount = 0;
		INT32 lGuildBattle = 0;
		INT32 lGuildMarkTID = 0;
		INT32 lGuildMarkColor = 0;
		BOOL bIsWinner = FALSE;
		cGuildListItemPacket.GetField( FALSE, pvGuildListItemPacket[i], 0, 
										&szID,
										&szMasterID,
										&szSubMasterID,
										&lLevel,
										&lWinPoint,
										&lMemberCount,
										&lMaxMemberCount,
										&lGuildBattle,
										&lGuildMarkTID,
										&lGuildMarkColor,
										&bIsWinner);

		AgcdGuildList stItem;
		memset(&stItem, 0, sizeof(stItem));
		if( szID )			strncpy( stItem.m_szID, szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		if( szMasterID )	strncpy( stItem.m_szMasterID		, szMasterID	, AGPACHARACTER_MAX_ID_STRING );
		if( szSubMasterID ) strncpy( stItem.m_szSubMasterID	, szSubMasterID , AGPACHARACTER_MAX_ID_STRING );

		stItem.m_lLevel				= lLevel;
		stItem.m_lWinPoint			= lWinPoint;
		stItem.m_lMemberCount		= lMemberCount;
		stItem.m_lMaxMemberCount	= lMaxMemberCount;
		stItem.m_lGuildBattle		= lGuildBattle;
		stItem.m_lGuildMarkTID		= lGuildMarkTID;
		stItem.m_lGuildMarkColor	= lGuildMarkColor;
		
		pThis->AddHostileDetail(stItem);

		// 현재 화면이 Hostile Mode 이면 UI 다시 그려준다.
		if(pThis->m_eListMode == AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE)
		{
			pThis->SetGuildListToHostileList();
			pThis->RefreshGuildList();
		}
	}

	return TRUE;
}

BOOL AgcmUIGuild::AddJointDetail(AgcdGuildList& stItem)
{
	// 원래 Joint List 에 있는 놈이 아니라면 할 필요없다.
	AgpdGuild* pcsGuild = m_pcsAgpmGuild->GetGuild(m_szSelfGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	if(!m_pcsAgpmGuild->IsJointGuild(pcsGuild, stItem.m_szID))
		return FALSE;

	GuildDetailIter iter = std::find_if(m_JointDetailVector.begin(), m_JointDetailVector.end(), FindGuildDetail(stItem.m_szID));
	if(iter == m_JointDetailVector.end())
	{
		// 새로 추가해준다.
		m_JointDetailVector.push_back(stItem);
	}
	else
	{
		// 있으면 업데이트
		memcpy(&*iter, &stItem, sizeof(stItem));
	}

	SortJointDetail();
	return TRUE;
}

BOOL AgcmUIGuild::SortJointDetail()
{
	AgpdGuild* pcsGuild = m_pcsAgpmGuild->GetGuild(m_szSelfGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	// 정렬한다.
	GuildDetailVector vcTmp;
	JointIter iterData = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iterData != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		GuildDetailIter iter = std::find_if(m_JointDetailVector.begin(),
											m_JointDetailVector.end(),
											FindGuildDetail(iterData->m_szGuildID));
		if(iter != m_JointDetailVector.end())
			vcTmp.push_back(*iter);

		++iterData;
	}

	// Swap
	m_JointDetailVector.swap(vcTmp);

	return TRUE;
}

BOOL AgcmUIGuild::AddHostileDetail(AgcdGuildList& stItem)
{
	// 원래 Hostile List 에 있는 놈이 아니라면 할 필요없다.
	AgpdGuild* pcsGuild = m_pcsAgpmGuild->GetGuild(m_szSelfGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	if(!m_pcsAgpmGuild->IsHostileGuild(pcsGuild, stItem.m_szID))
		return FALSE;

	GuildDetailIter iter = std::find_if(m_HostileDetailVector.begin(), m_HostileDetailVector.end(), FindGuildDetail(stItem.m_szID));
	if(iter == m_HostileDetailVector.end())
	{
		m_HostileDetailVector.push_back(stItem);
	}
	else
	{
		memcpy(&*iter, &stItem, sizeof(stItem));
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayRelationGuildID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	sprintf(szDisplay, "%s", pThis->m_szRelationGuildID);
	
	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayRelationMasterID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	sprintf(szDisplay, "%s", pThis->m_szRelationMasterID);
	
	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayJointRequestBtn(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	CHAR* szMessage = NULL;
	switch(pThis->m_eListMode)
	{
		case AGCMUIGUILD_LIST_VIEW_MODE_NORMAL:
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_REQUEST_BUTTON_TEXT);
			break;

		case AGCMUIGUILD_LIST_VIEW_MODE_JOINT:
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_LEAVE_BUTTON_TEXT);
			break;

		case AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE:
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_JOINT_REQUEST_BUTTON_TEXT);
			break;
	}

	if(!szMessage)
		return FALSE;

    //_tcscpy(((AcUIButton*)pcsSourceControl->m_pcsBase)->m_szButtonText, szMessage);
	if (szMessage) strcpy( szDisplay, szMessage);
	else szDisplay[0] = '\0';

	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayHostileRequestBtn(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	CHAR* szMessage = NULL;
	switch(pThis->m_eListMode)
	{
		case AGCMUIGUILD_LIST_VIEW_MODE_NORMAL:
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_REQUEST_BUTTON_TEXT);
			break;

		case AGCMUIGUILD_LIST_VIEW_MODE_JOINT:
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_REQUEST_BUTTON_TEXT);
			break;

		case AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE:
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_HOSTILE_LEAVE_BUTTON_TEXT);
			break;
	}

	if(!szMessage)
		return FALSE;

    //_tcscpy(((AcUIButton*)pcsSourceControl->m_pcsBase)->m_szButtonText, szMessage);
	if (szMessage) strcpy( szDisplay, szMessage);
	else szDisplay[0] = '\0';

	return TRUE;
}

BOOL AgcmUIGuild::SetGuildListToJointList()
{
	m_eListMode = AGCMUIGUILD_LIST_VIEW_MODE_JOINT;

	InitGuildListUIData();

	m_lVisibleGuildCount = m_JointDetailVector.size();
	m_lGuildListCurrentPage = 0;
	m_lGuildListMaxCount = m_JointDetailVector.size();

	INT32 lCount = 0;
	GuildDetailIter iter = m_JointDetailVector.begin();
	while(iter != m_JointDetailVector.end())
	{
		memcpy(m_parrAgcdGuildList[lCount++], &(*iter), sizeof(AgcdGuildList));
		++iter;
	}

	m_pstGuildListIndexUD->m_stUserData.m_lCount = m_lVisibleGuildCount;

	return TRUE;
}

BOOL AgcmUIGuild::SetGuildListToHostileList()
{
	m_eListMode = AGCMUIGUILD_LIST_VIEW_MODE_HOSTILE;

	InitGuildListUIData();

	m_lVisibleGuildCount = m_HostileDetailVector.size();
	m_lGuildListCurrentPage = 0;
	m_lGuildListMaxCount = m_HostileDetailVector.size();

	INT32 lCount = 0;
	GuildDetailIter iter = m_HostileDetailVector.begin();
	while(iter != m_HostileDetailVector.end())
	{
		memcpy(m_parrAgcdGuildList[lCount++], &(*iter), sizeof(AgcdGuildList));
		++iter;
	}

	m_pstGuildListIndexUD->m_stUserData.m_lCount = m_lVisibleGuildCount;
	return TRUE;
}

BOOL AgcmUIGuild::CBGetJointMark(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	INT32* plTID = static_cast<INT32*>(pCustData);

	GuildDetailIter iter = std::find_if(pThis->m_JointDetailVector.begin(),
										pThis->m_JointDetailVector.end(),
										FindGuildDetail(szGuildID));
	if(iter != pThis->m_JointDetailVector.end())
		*plTID = iter->m_lGuildMarkTID;

	return TRUE;
}

BOOL AgcmUIGuild::CBGetJointColor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	CHAR* szGuildID = static_cast<CHAR*>(pData);
	INT32* plColor = static_cast<INT32*>(pCustData);

	GuildDetailIter iter = std::find_if(pThis->m_JointDetailVector.begin(),
										pThis->m_JointDetailVector.end(),
										FindGuildDetail(szGuildID));
	if(iter != pThis->m_JointDetailVector.end())
		*plColor = iter->m_lGuildMarkColor;

	return TRUE;
}