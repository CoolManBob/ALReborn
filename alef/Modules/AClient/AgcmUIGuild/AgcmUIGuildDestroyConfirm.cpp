// AgcmUIGuildDestroyConfirm.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 21.

#include "AgcmUIGuild.h"

BOOL AgcmUIGuild::CBDestroyConfirmToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1 || !pData2)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;
	AgcdUIControl* pcsEditControl2 = (AgcdUIControl*)pData2;

	if(strlen(pThis->m_szInputPassword) == 0)
		return FALSE;

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR szBuffer[255];
	memset(szBuffer, 0, sizeof(CHAR) * 255);

	sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_INFO), pThis->m_szSelfGuildID, pcsCharacter->m_szID);
	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText(szBuffer);

	sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_DESTROY_CONFIRM_TEXT));
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetText(szBuffer);

	INT32 lGuildMarkTID = 0;
	INT32 lGuildMarkColor = 0xFFFFFFFF;

	AgpdGuild *pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if (pcsGuild)
	{
		lGuildMarkTID = pcsGuild->m_lGuildMarkTID;
		lGuildMarkColor = pcsGuild->m_lGuildMarkColor;
		pcsGuild->m_Mutex.Release();
	}
	pThis->GuildDestroyConfirmGuildMarkDraw(lGuildMarkTID, lGuildMarkColor);

	return TRUE;
}

BOOL AgcmUIGuild::CBDestroyConfirmOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(strlen(pThis->m_szInputPassword) == 0)
		return FALSE;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// 마스터가 아니면 나간다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID))
		return FALSE;

	pThis->m_pcsAgcmGuild->SendDestroy(pThis->m_szInputPassword);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyConfirmUI);

	return TRUE;
}

/*
	2005.10.27. By SungHoon
	길드 해체 확인 창에 길드 마크를 뿌려준다.
*/
BOOL AgcmUIGuild::GuildDestroyConfirmGuildMarkDraw( INT32 lGuildMarkTID, INT32 lGuildMarkColor )
{
	AgpdGridItem *ppGridItem[3];
	ppGridItem[0] = m_pcsGuildMarkGuildDestroyGridItem;
	ppGridItem[1] = m_pcsGuildMarkGuildDestroyPatternGridItem;
	ppGridItem[2] = m_pcsGuildMarkGuildDestroySymbolGridItem;

	AgcdUIUserData *ppGridControl[3];
	ppGridControl[0] = m_pcsUDGuildMarkGuildDestroyGrid;
	ppGridControl[1] = m_pcsUDGuildMarkGuildDestroyPatternGrid;
	ppGridControl[2] = m_pcsUDGuildMarkGuildDestroySymbolGrid;
	
	return (GuildMarkDraw(ppGridItem, ppGridControl, lGuildMarkTID, lGuildMarkColor));
}
