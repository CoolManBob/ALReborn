#include "AgcmUIGuild.h"

/*
	핫키 토글로 인해 길드 리스트 창이 뜰 경우
*/
BOOL AgcmUIGuild::CBGuildListToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_bGuildListRelationBtn	=	FALSE;


	pThis->m_eListMode = AGCMUIGUILD_LIST_VIEW_MODE_NORMAL;

	pThis->m_bEnableGuildBattle = TRUE;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( pcsCharacter )
	{
		AgpdSiegeWar*	GetSiegeWar(AgpdCharacter *pcsCharacter);

		if( pThis->m_pcsAgpmCharacter->IsInDungeon( pcsCharacter ) || pThis->m_pcsAgpmCharacter->IsInSiege( pcsCharacter ) )
			pThis->m_bEnableGuildBattle = FALSE;

		AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
		if ( pcsGuild )
		{
			if ((!pThis->m_pcsAgpmGuild ->IsMaster(pcsGuild, pcsCharacter->m_szID)) || (pThis->m_pcsAgpmGuild->IsBattleStatus(pcsGuild)))
				pThis->m_bEnableGuildBattle = FALSE;
			

			if( strlen(pcsGuild->m_szID) )
			{
				if( pThis->m_pcsAgpmGuild->IsMaster( pcsGuild->m_szID , pcsCharacter->m_szID ) )
				{
					pThis->m_bGuildListRelationBtn	=	TRUE;
				}
			}

			pcsGuild->m_Mutex.Release();
		}
	}
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableGuildBattle );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstGuildListRelationBtn);

	pThis->InitGuildListUIData();
	pThis->RefreshGuildList();
	pThis->m_pcsAgcmGuild->SendGuildListRequest(0);
	pThis->GuildListGuildMarkDraw(0, 0xFFFFFFFF);		//	길드 마크 그리드를 초기화한다.
	
	return TRUE;
}


/*
	다른 탭에서 길드 리스트 창을 띄울 경우
*/
BOOL AgcmUIGuild::CBGuildListTabClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)			return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_bGuildListRelationBtn		=	FALSE;
	pThis->m_eListMode = AGCMUIGUILD_LIST_VIEW_MODE_NORMAL;
	pThis->m_cGuildInfoUILastTab = 2;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildNoticeUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildGuildListUI);

	pThis->InitGuildListUIData();
	pThis->RefreshGuildList();

	CHAR*	szGuildID		=	pThis->m_pcsAgcmGuild->GetSelfGuildID();
	
	if( szGuildID && pThis->m_pcsAgpmGuild->IsMaster( szGuildID , pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_szID  ) )
		pThis->m_bGuildListRelationBtn		=	TRUE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstGuildListRelationBtn );

	return TRUE;
}

/*
	길드 리스트창에서 닫기버튼(x버튼과 닫기버튼)을 눌렀을 경우 UI닫는 함수
*/
BOOL AgcmUIGuild::CBGuildListClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_bGuildInfoUIOpened = FALSE;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildGuildListUI);

	return TRUE;
}

/*
	길드 리스트창에서 길드를 선택했을 경우
*/
BOOL AgcmUIGuild::CBGuildListClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= pThis->m_lVisibleGuildCount)
		return FALSE;

	// List Item 을 체크해준다.
	pThis->m_lSelectedGuildIndex = pcsSourceControl->m_lUserDataIndex;

	pThis->RefreshGuildList();
	
	return TRUE;
}

/*
	길드리스트창에서 이전 버튼을 클릭했다.
*/
BOOL AgcmUIGuild::CBGuildListLeftClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 현재 페이지가 0(첫페이지) 면 무시때림
	if(pThis->m_lGuildListCurrentPage == 0)
		return FALSE;
	
	pThis->m_pcsAgcmGuild->SendGuildListRequest(pThis->m_lGuildListCurrentPage-1);

	return TRUE;
}

/*
	길드리스트창에서 다음 버튼을 클릭했다.
*/
BOOL AgcmUIGuild::CBGuildListRightClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)		return FALSE;

	AgcmUIGuild* pThis = static_cast< AgcmUIGuild* >(pClass);

	if(pThis->m_lGuildListCurrentPage >= pThis->m_lMaxGuildListPage)
		return FALSE;
	pThis->m_pcsAgcmGuild->SendGuildListRequest(pThis->m_lGuildListCurrentPage+1);

	return TRUE;
}

/*
	길드리스트 창에서 상세보기 Display 함수
*/
BOOL AgcmUIGuild::CBDisplayGuildListDetailInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");

	if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0) return FALSE;

	AgcdGuildList *pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
	if (!pcsAgcdGuildList) return FALSE;

	CHAR szGuildBattle[300];
	memset(szGuildBattle, 0, sizeof(CHAR) * 300);
	//if (pcsAgcdGuildList->m_lGuildBattle == 0) strcpy(szGuildBattle,"없음");
	//else wsprintf(szGuildBattle, "%d", pcsAgcdGuildList->m_lGuildBattle);

	CHAR szBuffer[300];
	memset(szBuffer, 0, sizeof(CHAR) * 300);

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_DETAIL_INFO);
	if(szUIMessage)
			wsprintf(szBuffer, szUIMessage,
				pcsAgcdGuildList->m_szID, pcsAgcdGuildList->m_lLevel, pcsAgcdGuildList->m_szMasterID, pcsAgcdGuildList->m_szSubMasterID ,
				pcsAgcdGuildList->m_lMemberCount, pcsAgcdGuildList->m_lMaxMemberCount, szGuildBattle);

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

	pThis->GuildListGuildMarkDraw(pcsAgcdGuildList->m_lGuildMarkTID, pcsAgcdGuildList->m_lGuildMarkColor);

	return TRUE;
}

/*
	2005.07.09. By SungHoon
	길드 리스트에서 길드 이름 보여주는 Display
*/
BOOL AgcmUIGuild::CBDisplayGuildName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if(*pcsIndex >= pThis->m_lVisibleGuildCount || *pcsIndex < 0)
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	CHAR szBuffer[AGPMGUILD_MAX_GUILD_ID_LENGTH + 10];
	memset(szBuffer, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH + 10);
	strncpy(szBuffer, pThis->m_parrAgcdGuildList[*pcsIndex]->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	for (UINT16 i = 0 ; i < strlen(szBuffer); ++i)
	{
		if (szBuffer[i] == '@')
		{
			strcpy(&szBuffer[i], "...");
			break;
		}
	}
	strncpy(szDisplay, szBuffer, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	return TRUE;
}

/*
	2005.07.09. By SungHoon
	길드 리스트에서 길드 인원 보여주는 Display
*/
BOOL AgcmUIGuild::CBDisplayGuildMemberCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if(*pcsIndex >= pThis->m_lVisibleGuildCount || *pcsIndex < 0)
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	wsprintf(szDisplay, "%d", pThis->m_parrAgcdGuildList[*pcsIndex]->m_lMemberCount);

	return TRUE;
}

/*
	2005.07.08. By SungHoon
	길드 리스트에서 길드 승리 포인트 보여주는 Display
*/
BOOL AgcmUIGuild::CBDisplayGuildWinPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if(*pcsIndex >= pThis->m_lVisibleGuildCount || *pcsIndex < 0)
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	wsprintf(szDisplay, "%d", pThis->m_parrAgcdGuildList[*pcsIndex]->m_lWinPoint);

	return TRUE;
}

/*
	2005.07.08 By SungHoon
	길드리스트를 Refresh 한다. 
*/
BOOL AgcmUIGuild::RefreshGuildList()
{
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstGuildListIndexUD);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDGuildListDetailInfo);

	if ( m_lGuildListMaxCount > 0 )
		m_lMaxGuildListPage = ((m_lGuildListMaxCount - 1)/ AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstGuildListPage);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstGuildListMaxPage);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstJointRequestBtnUD);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstHostileRequestBtnUD);

	return TRUE;
}

BOOL AgcmUIGuild::GuildListGuildMarkDraw( INT32 lGuildMarkTID, INT32 lGuildMarkColor )
{
	AgpdGridItem *ppGridItem[3];
	ppGridItem[0] = m_pcsGuildMarkGuildListGridItem;
	ppGridItem[1] = m_pcsGuildMarkGuildListPatternGridItem;
	ppGridItem[2] = m_pcsGuildMarkGuildListSymbolGridItem;

	AgcdUIUserData *ppGridControl[3];
	ppGridControl[0] = m_pcsUDGuildMarkGuildListGrid;
	ppGridControl[1] = m_pcsUDGuildMarkGuildListPatternGrid;
	ppGridControl[2] = m_pcsUDGuildMarkGuildListSymbolGrid;
	
	return (GuildMarkDraw(ppGridItem, ppGridControl, lGuildMarkTID, lGuildMarkColor));
}

/*
	2005.07.08. By SungHoon
	길드 리스트가 선택될 경우 상태를 변화시킨다.
*/
BOOL AgcmUIGuild::CBDisplayGuildSelected(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if(*pcsIndex >= pThis->m_lVisibleGuildCount || *pcsIndex < 0)
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	if(pThis->m_lSelectedGuildIndex == pcsSourceControl->m_lUserDataIndex)
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID);
	else
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[1].m_lStatusID);

	//pThis->RefreshMemberUI();

	return TRUE;
}

/*
	2005.07.08. By SungHoon
	길드 리스트 UI를 초기화 한다.
*/
BOOL AgcmUIGuild::InitGuildListUIData()
{
	for(int i = 0; i < AGPMGUILD_MAX_VISIBLE_GUILD_LIST; ++i)
		memset(m_parrAgcdGuildList[i], 0, sizeof(m_parrAgcdGuildList));
	m_lVisibleGuildCount = 0;

	m_lSelectedGuildIndex= -1;

	m_lGuildListCurrentPage = 0;
	m_lGuildListMaxCount = 0;

	return TRUE;
}

/*
	2005.07.08. By SungHoon
	길드 리스트를 받았을 경우 Call-Back
*/
BOOL AgcmUIGuild::CBReceiveGuildListInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	PVOID *pvArray = (PVOID *)pData;

	PVOID pvPacket = pvArray[ 0 ];
	CHAR *szGuildID = ( CHAR *)pvArray[ 1 ];

	if (!pvPacket) return FALSE;

	PVOID pvGuildListItemPacket[ AGPMGUILD_MAX_VISIBLE_GUILD_LIST ] = { 0 };

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

	if( nVisibleGuildCount > AGPMGUILD_MAX_VISIBLE_GUILD_LIST )	return FALSE; 

	pThis->m_lVisibleGuildCount		= nVisibleGuildCount;
	pThis->m_lGuildListCurrentPage	= lGuildListCurrentPage;
	pThis->m_lGuildListMaxCount		= lMaxGuildCount;

	for ( INT16 i = 0 ; i < AGPMGUILD_MAX_VISIBLE_GUILD_LIST; i++ )
		memset( pThis->m_parrAgcdGuildList[i], 0, sizeof(AgcdGuildList) );

	pThis->m_lSelectedGuildIndex = -1;

	AuPacket& cGuildListItemPacket = pThis->m_pcsAgpmGuild->m_csGuildListItemPacket;
	for ( INT16 i = 0 ; i < pThis->m_lVisibleGuildCount; i++ )
	{
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

		if (szID)			strncpy( pThis->m_parrAgcdGuildList[i]->m_szID				, szID			, AGPMGUILD_MAX_GUILD_ID_LENGTH );
		if (szMasterID)		strncpy( pThis->m_parrAgcdGuildList[i]->m_szMasterID		, szMasterID	, AGPACHARACTER_MAX_ID_STRING	);
		if (szSubMasterID)	strncpy( pThis->m_parrAgcdGuildList[i]->m_szSubMasterID		, szSubMasterID , AGPACHARACTER_MAX_ID_STRING	);

		pThis->m_parrAgcdGuildList[i]->m_lLevel				= lLevel;
		pThis->m_parrAgcdGuildList[i]->m_lWinPoint			= lWinPoint;
		pThis->m_parrAgcdGuildList[i]->m_lMemberCount		= lMemberCount;
		pThis->m_parrAgcdGuildList[i]->m_lMaxMemberCount	= lMaxMemberCount;
		pThis->m_parrAgcdGuildList[i]->m_lGuildBattle		= lGuildBattle;
		pThis->m_parrAgcdGuildList[i]->m_lGuildMarkTID		= lGuildMarkTID;
		pThis->m_parrAgcdGuildList[i]->m_lGuildMarkColor	= lGuildMarkColor;
		if( szGuildID && szID )
		{
			if ( !strcmp(szGuildID, szID) )
				pThis->m_lSelectedGuildIndex = i;
		}
	}
	pThis->m_pstGuildListIndexUD->m_stUserData.m_lCount = pThis->m_lVisibleGuildCount;
	pThis->RefreshGuildList();
	return TRUE;
}

/*
	2005.07.13. By SungHoon
	길드 최대 인원이 변경된다.
*/
BOOL AgcmUIGuild::CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32 lCID = *( INT32 *)pClass;
	INT32 lNewMemberCount = *( INT32 *)pCustData;

	for ( INT16 i = 0 ; i < pThis->m_lVisibleGuildCount; i++ )
	{
		if (!strcmp(pThis->m_parrAgcdGuildList[i]->m_szID, pThis->m_szSelfGuildID))
		{
			pThis->m_parrAgcdGuildList[i]->m_lMaxMemberCount = lNewMemberCount;
			pThis->RefreshGuildList();
			break;
		}
	}
	return TRUE;
}

/*
	2005.07.15. By SungHoon
	길드찾기를 눌렀을 경우.
*/
BOOL AgcmUIGuild::CBGuildListSearch(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData1 )
		return FALSE;

	AgcmUIGuild				*pThis				= (AgcmUIGuild *)	pClass;
	AgcdUIControl	*pcsUIGuildSearchControl	= (AgcdUIControl *)	pData1;

	if (pcsUIGuildSearchControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;

	AcUIEdit *	pcsUIGuildSearchEdit = (AcUIEdit *) pcsUIGuildSearchControl->m_pcsBase;
	if (!pcsUIGuildSearchEdit) return FALSE;

	CHAR *szGuildID = (CHAR*)pcsUIGuildSearchEdit->GetText();
	if (!szGuildID) return FALSE;

	if (strlen(szGuildID) <= 0) return FALSE;

	CHAR szSearchGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	memset(szSearchGuildID, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
	strncpy(szSearchGuildID,szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH);

	pcsUIGuildSearchEdit->SetMeActiveEdit();
	pcsUIGuildSearchEdit->ClearText();

	pThis->m_pcsAgcmGuild->SendGuildListRequestFind(szSearchGuildID);

	return ( TRUE );
}

/*
	2005.07.20. By SungHoon
	길드 인원보기 페이지 리스트 보여주기
*/
BOOL AgcmUIGuild::CBDisplayGuildListPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if (pThis->m_lGuildListMaxCount <= 0)
	{
		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");
		return TRUE;
	}

	INT32 lFirstPage = pThis->CalcFirstPage(pThis->m_lGuildListCurrentPage, pThis->m_lMaxGuildListPage);
	INT32 lLastPage = min(pThis->m_lMaxGuildListPage, lFirstPage + AGCMUIGUILD_MAX_VIEW_PAGE);

	UINT32 lDeselectColor = 4286611584;
	UINT32 lSelectColor = 4294963400;

	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_SELECT_PAGE_COLOR);
	if(szColor) lSelectColor = atol(szColor);

	szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_DESELECT_PAGE_COLOR);
	if(szColor) lDeselectColor = atol(szColor);

	CHAR szBuffer[512] = { 0 };
	for ( INT i = lFirstPage ; i <= lLastPage; i++ )
	{
		CHAR szPage[ 50 ];
		if (i == pThis->m_lGuildListCurrentPage) wsprintf(szPage, "<C%d>%d", lSelectColor, i+1);
		else wsprintf(szPage, "<C%d>%d", lDeselectColor, i + 1);
		if (i != 0) strcat(szBuffer, " ");
		strcat(szBuffer, szPage);
	}

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);
	return TRUE;
}

/*
	2005.07.20. By SungHoon
	길드 인원보기 최대 페이지 보여주기
*/
BOOL AgcmUIGuild::CBDisplayGuildListMaxPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if (pThis->m_lGuildListMaxCount > 0)
	{
		CHAR szBuffer[ 50 ];
		wsprintf( szBuffer, "%d/%d", (pThis->m_lGuildListCurrentPage+1), (pThis->m_lMaxGuildListPage+1));
		strcpy( szDisplay, szBuffer);
	}
	else szDisplay[0] = '\0';

	return TRUE;
}

/*
	2005.07.22. By SungHoon
	길드전신청/길드 가입 버튼을 동적으로 하기위해 버튼에 Display 검
*/
BOOL AgcmUIGuild::CBDisplayGuildGuildBattleJoinRequest(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	CHAR *szMessage = NULL;
	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if(pcsGuild)
	{
		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_LIST_BATTLE_BUTTON_TEXT);
		pcsGuild->m_Mutex.Release();
	}
	else
		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_LIST_JOIN_REQUEST_BUTTON_TEXT);

	if (szMessage) strcpy( szDisplay, szMessage);
	else szDisplay[0] = '\0';

	return TRUE;
}

/*
	2005.08.10. By SungHoon
	길드 가입 신청 YES를 클릭헀을 경우 경우 불린다.
*/
BOOL AgcmUIGuild::CBEventReturnConfirmJoinRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount || pThis->m_lSelectedGuildIndex < 0) return FALSE;

	AgcdGuildList *pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
	if (!pcsAgcdGuildList) return FALSE;

	if (lTrueCancel == (INT32) TRUE)
	{
		if (strlen(pcsAgcdGuildList->m_szID) <= 0) return FALSE;

		// 마스터가 아니면 나간다.
		CHAR *szSelfGuildID = pThis->m_pcsAgcmGuild->GetSelfGuildID();
		if (szSelfGuildID)
			if (strlen(szSelfGuildID) > 0) return TRUE;
		pThis->m_pcsAgcmGuild->SendSelfJoin(pcsAgcdGuildList->m_szID);
	}
	return TRUE;
}

