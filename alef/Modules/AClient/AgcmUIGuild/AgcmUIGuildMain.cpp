// AgcmUIGuildMain.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 08.

#include "AgcmUIGuild.h"

BOOL AgcmUIGuild::CBMainOpenRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateConfirmUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateCompleteUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyConfirmUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyCompleteUI);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildMainUI);

	return TRUE;
}

BOOL AgcmUIGuild::CBMainToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateConfirmUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateCompleteUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyConfirmUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyCompleteUI);

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter)
		return FALSE;

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	CHAR szBuffer[512] = {0, };
	
	INT32 lNeedGheld = 100000;
	AgpdRequireItemIncreaseMaxMember *pcsRequirement = pThis->m_pcsAgpmGuild->GetRequireIncreaseMaxMember(50);
	if (pcsRequirement)
	{	
		lNeedGheld = pcsRequirement->m_lGheld;
		INT32 lTaxRatio = pThis->m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter );
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lNeedGheld * lTaxRatio) / 100;
		}
		lNeedGheld = lNeedGheld + lTax;
	}

	if ( AP_SERVICE_AREA_WESTERN != g_eServiceArea )
	{
		_snprintf_s(szBuffer, 512, _TRUNCATE,
					pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_MAIN_TEXT),
					AGPMGUILD_CREATE_REQUIRE_LEVEL, lNeedGheld);
	}
	else
	{
		_snprintf_s(szBuffer, 512, _TRUNCATE,
					pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_MAIN_TEXT),
					AGPMGUILD_CREATE_REQUIRE_LEVEL);
	}

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText(szBuffer);

	pThis->m_bEnableCreateGuild			=	TRUE;
	pThis->m_bEnableGuildMaster			=	TRUE;
	pThis->m_bEnableRenameGuildID		=	FALSE;

	if (strlen(pThis->m_pcsAgcmGuild->GetSelfGuildID()) > 0)
		pThis->m_bEnableCreateGuild = FALSE;
	
	if ((INT64) lNeedGheld > pcsCharacter->m_llMoney)
		pThis->m_bEnableCreateGuild = FALSE;
	
	// 길드원이 존재하면 길드 해제 신청 버튼 비활성
	if( pThis->m_pcsAgcmGuild->IsSelfGuildMaster() )
	{	
		AgpdGuild*	pdGuild		=	pThis->m_pcsAgcmGuild->GetSelfGuildLock();
		pThis->m_bEnableGuildCancle = pThis->m_pcsAgpmGuild->GetAllMemberCount( pdGuild ) == 1 ? TRUE : FALSE;
		pdGuild->m_Mutex.Release();
	}

	AgpdGuild	*pdGuild	=	pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pdGuild )
	{
		pThis->m_bEnableGuildCancle			=	FALSE;
		pThis->m_bEnableGuildMaster			=	FALSE;
	}
	else
		pdGuild->m_Mutex.Release();


	if (pThis->m_bEnableGuildMaster)
	{
		pThis->m_bEnableRenameGuildID = pThis->m_pcsAgpmGuild->IsDuplicatedCharacterOfMigration(pThis->m_pcsAgcmGuild->GetSelfGuildID());
	}

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableGuildCancle);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableCreateGuild);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableGuildMaster);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableRenameGuildID);

	return TRUE;
}

BOOL AgcmUIGuild::CBMainCreateRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(strlen(pThis->m_szSelfGuildID) > 0)	// 이미 길드에 가입되어 있는 상태임
	{
		AgpdGuildSystemMessage stSystemMessage;
		memset(&stSystemMessage, 0, sizeof(AgpdGuildSystemMessage));

		stSystemMessage.m_lCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER;
		CBSystemMessage(&stSystemMessage, pThis, NULL);
		
		return TRUE;
	}
	
	if(pThis->m_pcsAgcmGuild->GetSelfCharLevel() < AGPMGUILD_CREATE_REQUIRE_LEVEL)
	{
		SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_NEED_MORE_LEVEL));
		return FALSE;
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMainUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildCreateUI);
	
	return TRUE;
}

BOOL AgcmUIGuild::CBMainDestroyRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	if( !pThis )			return FALSE;
	if( !strlen(pThis->m_szSelfGuildID ) )		return TRUE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return TRUE;

	// 마스터가 아니면 나간다.
	if(pThis->m_pcsAgpmGuild->IsMaster(pThis->m_szSelfGuildID, pcsCharacter->m_szID) == FALSE)
		return FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMainUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildDestroyUI);
	
	return TRUE;
}
