// AgcmUIGuildNotice.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 12.

#include "AgcmUIGuild.h"


BOOL AgcmUIGuild::CBNoticeOpenRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(pThis->m_bGuildInfoUIOpened)	// 이미 열려 있으면 닫아준다.
	{
		pThis->m_bGuildInfoUIOpened = FALSE;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildNoticeUI);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildGuildListUI);

		return TRUE;
	}

	return (pThis->OpenGuildUI());
}

BOOL AgcmUIGuild::CBNoticeToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventInitGuildNoticeUI);
	return TRUE;
}


BOOL AgcmUIGuild::CBNoticeUIInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1 || !pData2 || !pData3)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl1 = (AgcdUIControl*)pData1;		//	공지사항
	AgcdUIControl* pcsEditControl2 = (AgcdUIControl*)pData2;		//	상세정보
	AgcdUIControl* pcsEditControl3 = (AgcdUIControl*)pData3;		//	길드정보

	((AcUIEdit*)pcsEditControl1->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit*)pcsEditControl3->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	pThis->m_bEnableLeaveGuildNotice = FALSE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter) return FALSE;

	INT32 lGuildMarkTID = 0;
	INT32 lGuildMarkColor = 0xFFFFFFFF;

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( pcsGuild )
	{
		pThis->m_bEnableLeaveGuildNotice = TRUE;
		AgpdGuildMember *pcsMember = pThis->m_pcsAgpmGuild->GetMember(pcsGuild, pcsSelfCharacter->m_szID);
		if ( pcsMember )
		{
			if ( pcsMember->m_lRank == AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST )
				pThis->m_bEnableLeaveGuildNotice = FALSE;
		}

		CHAR szBuffer[300];
		memset(szBuffer, 0, sizeof(CHAR) * 300);

		if(pcsGuild->m_szNotice && strlen(pcsGuild->m_szNotice) > 0 && strlen(pcsGuild->m_szNotice) <= AGPMGUILD_MAX_NOTICE_LENGTH)
			sprintf(szBuffer, pcsGuild->m_szNotice);
		else
			sprintf(szBuffer, "");

		((AcUIEdit*)pcsEditControl1->m_pcsBase)->SetText(szBuffer);

/*		상세정보로 변경됨 2005.07.05. By SungHoon
		sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_INFO),
									pcsGuild->m_szID, pcsGuild->m_szMasterID);
		((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetText(szBuffer);
*/
		//	2005.07.06 By SungHoon
		//	길드 정보 보여주기 수정		임의로 넣었음 나중 UIMessage로 빼야함
		CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_NOTICE_INFO);
		if(szUIMessage)
			wsprintf(szBuffer, szUIMessage,
				pcsGuild->m_szID, pcsGuild->m_szMasterID, pThis->m_pcsAgpmGuild->GetMemberCount(pcsGuild), pcsGuild->m_lMaxMemberCount, 
				pThis->m_pcsAgpmGuild->GetOnlineMemberCount(pcsGuild) );
		// 2005.04.20. steeple
		// 멤버 보여주는 건 한줄로 줄이고, 그 아래 줄에 전적을 보여주게 바꿨다.
/*		sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_MEMBER_INFO),
									pThis->m_pcsAgpmGuild->GetOnlineMemberCount(pcsGuild), pThis->m_pcsAgpmGuild->GetMemberCount(pcsGuild),
									pcsGuild->m_lWin, pcsGuild->m_lDraw, pcsGuild->m_lLose);
*/
		((AcUIEdit*)pcsEditControl3->m_pcsBase)->SetText(szBuffer);

		lGuildMarkTID	= pcsGuild->m_lGuildMarkTID;
		lGuildMarkColor	= pcsGuild->m_lGuildMarkColor;
		pcsGuild->m_Mutex.Release();
	}
	else
	{
		((AcUIEdit*)pcsEditControl1->m_pcsBase)->SetText("");
		((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetText("");
		((AcUIEdit*)pcsEditControl3->m_pcsBase)->SetText("");
	}
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableLeaveGuildNotice);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstGuildNoticeDetailInfo);

	if(pThis->m_pcsAgpmGuild->IsMaster(pThis->m_pcsAgcmGuild->GetSelfGuildID(), pcsSelfCharacter->m_szID))
		((AcUIEdit*)pcsEditControl1->m_pcsBase)->m_bReadOnly = FALSE;		// Notice Edit 의 Read Only 를 풀어준다.


	AgpdGridItem *ppGridItem[3];
	ppGridItem[0] = pThis->m_pcsGuildMarkMainGridItem;
	ppGridItem[1] = pThis->m_pcsGuildMarkMainPatternGridItem;
	ppGridItem[2] = pThis->m_pcsGuildMarkMainSymbolGridItem;

	AgcdUIUserData *ppGridControl[3];
	ppGridControl[0] = pThis->m_pcsUDGuildMarkMainGrid;
	ppGridControl[1] = pThis->m_pcsUDGuildMarkMainPatternGrid;
	ppGridControl[2] = pThis->m_pcsUDGuildMarkMainSymbolGrid;
	
	pThis->GuildMarkDraw(ppGridItem, ppGridControl, lGuildMarkTID, lGuildMarkColor);

	return TRUE;
}

BOOL AgcmUIGuild::CBNoticeTabClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_cGuildInfoUILastTab = 0;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildGuildListUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildNoticeUI);

	return TRUE;
}

BOOL AgcmUIGuild::CBNoticeClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_bGuildInfoUIOpened = FALSE;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildNoticeUI);

	return TRUE;
}

// Notice Edit Control 의 Read-Only 를 풀어준다.
BOOL AgcmUIGuild::CBNoticeEditClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsNoticeEdit = (AgcdUIControl*)pData1;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 마스터만이 할 수 있다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(pThis->m_pcsAgcmGuild->GetSelfGuildID(), pcsSelfCharacter->m_szID))
		return FALSE;

	// Notice Edit 의 Read Only 를 풀어준다.
	((AcUIEdit*)pcsNoticeEdit->m_pcsBase)->m_bReadOnly = FALSE;

	return TRUE;
}

// 공지사항을 전송한다.
BOOL AgcmUIGuild::CBNoticeSendClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsNoticeEdit = (AgcdUIControl*)pData1;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 마스터만이 할 수 있다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(pThis->m_pcsAgcmGuild->GetSelfGuildID(), pcsSelfCharacter->m_szID))
		return FALSE;

	AgcmChatting2* pcmChatting = ( AgcmChatting2* )pThis->GetModule( "AgcmChatting2" );
	if( !pcmChatting ) return FALSE;

	CHAR* pNoticeString = (CHAR*)((AcUIEdit*)pcsNoticeEdit->m_pcsBase)->GetText();
	if( !pNoticeString || strlen( pNoticeString ) <= 0 ) return FALSE;

	pcmChatting->OnFiltering( pNoticeString );


	// 보낸다.
	pThis->m_pcsAgcmGuild->SendNotice( pNoticeString );

	return TRUE;
}

// Notice Display
BOOL AgcmUIGuild::CBDisplayNotice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( pcsGuild )
	{
		CHAR szBuffer[300];
		memset(szBuffer, 0, sizeof(CHAR) * 300);

		if(pcsGuild->m_szNotice && strlen(pcsGuild->m_szNotice) > 0 && strlen(pcsGuild->m_szNotice) <= AGPMGUILD_MAX_NOTICE_LENGTH)
			sprintf(szBuffer, pcsGuild->m_szNotice);
		else
			sprintf(szBuffer, "");

		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

		pcsGuild->m_Mutex.Release();
	}
	else
	{
		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");
	}

	return TRUE;
}

/*
	2005.07.05 By SungHoon
	상세정보를 보여주긴 위한 Display
*/
BOOL AgcmUIGuild::CBDisplayDetailInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR *szMessage = NULL;
	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter) return FALSE;

	CHAR szBuffer[300];
	memset(szBuffer, 0, sizeof(CHAR) * 300);

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( pcsGuild )
	{
		AgpdGuildMember *pcsMember = pThis->m_pcsAgpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
		if (!pcsMember)
		{
			pcsGuild->m_Mutex.Release();
			return TRUE;
		}

		if (pcsMember->m_lRank != AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST)		//	탈퇴 대기중이 아니면 대기 시간 안뿌려준다.
		{
			pcsGuild->m_Mutex.Release();
			return TRUE;		
		}

		pcsGuild->m_Mutex.Release();

		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_NOTICE_REMAIN_LEAVE_TIME);
		if (!szMessage) return TRUE;

		CHAR* szTimeUnit = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE);

		UINT32 ulElapTime = AGPMGUILD_LEAVE_NEED_TIME - ( pThis->m_pcsAgpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate );
		if( ulElapTime > AGPMGUILD_LEAVE_NEED_TIME )
			ulElapTime = 0;
		else
		{
			if (ulElapTime > AGCMUIGUILD_SECOND_EQUAL_HOUR)
			{
				ulElapTime = ulElapTime / AGCMUIGUILD_SECOND_EQUAL_HOUR;
				szTimeUnit = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_HOUR);
			}
			else
			{
				ulElapTime = ulElapTime / AGCMUIGUILD_SECOND_EQUAL_MIN;
			}
		}
		if( szTimeUnit )	wsprintf(szBuffer, szMessage, ulElapTime , szTimeUnit );
		else				wsprintf(szBuffer, szMessage, ulElapTime , " ");
	}
	else
	{
		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_NOTICE_DETAIL_NO_GUILD_MEMBER);
		if( !szMessage )		return TRUE;

		strncpy(szBuffer, szMessage,299);
	}

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);
	return TRUE;
}

/*
	2005.07.21. By SungHoon
	길드 공지 창에서 길드 탈퇴 버튼을 눌렀을 경우
*/
BOOL AgcmUIGuild::CBGuildClickLeaveGuildNotice(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return FALSE;
		
	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pcsGuild )			return FALSE;
	
	AgpdGuildMember *pcsMember = pThis->m_pcsAgpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if (!pcsMember)
	{
		pcsGuild->m_Mutex.Release();
		return TRUE;
	}
	if (pcsMember->m_lRank == AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST)
	{
		pcsGuild->m_Mutex.Release();
		return TRUE;		//	탈퇴 대기중이면 무시
	}

	BOOL bIsMaster = pThis->m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID);
	pcsGuild->m_Mutex.Release();
	if (bIsMaster)
	{
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_SYSTEM_MESSAGE_COLOR);
		DWORD dwColor = 0xFFFFFF33;
		if(szColor) dwColor = atoi(szColor);

		CHAR *szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_LEAVE_ERROR_MASTER);
		if (szBuffer) SystemMessage.ProcessSystemMessage(szBuffer, dwColor);
	}
	else
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenLeaveConfirmUI);
	}
	

	return TRUE;
}

