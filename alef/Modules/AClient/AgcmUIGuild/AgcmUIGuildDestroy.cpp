// AgcmUIGuildDestroy.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 08.

#include "AgcmUIGuild.h"

BOOL AgcmUIGuild::CBDestroyToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1 || !pData2)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;
	AgcdUIControl* pcsEditControl2 = (AgcdUIControl*)pData2;

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_DESTROY_TEXT));
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetText(pThis->m_szSelfGuildID);

	return TRUE;
}

BOOL AgcmUIGuild::CBDestroyOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// 마스터가 아니면 나간다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID))
		return FALSE;

	const char* szPassword = ((AcUIEdit*)pcsEditControl->m_pcsBase)->GetText();
	strncpy(pThis->m_szInputPassword, szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH);

	// UI 에 써진 것을 지우고
	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText("");

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildDestroyUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildDestroyConfirmUI);

	return TRUE;
}