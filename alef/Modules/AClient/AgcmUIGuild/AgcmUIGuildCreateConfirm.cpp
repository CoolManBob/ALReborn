// AgcmUIGuildCreateConfirm.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 12.

#include "AgcmUIGuild.h"

BOOL AgcmUIGuild::CBCreateConfirmToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1 || !pData2)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;
	AgcdUIControl* pcsEditControl2 = (AgcdUIControl*)pData2;

	if(strlen(pThis->m_szInputGuildName) == 0)
		return FALSE;
	if(strlen(pThis->m_szInputPassword) == 0)
		return FALSE;

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR szBuffer[255];
	memset(szBuffer, 0, sizeof(CHAR) * 255);

	sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_INFO), pThis->m_szInputGuildName, pcsCharacter->m_szID);
	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText(szBuffer);

	sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_CREATE_CONFIRM_TEXT));
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetText(szBuffer);
	
	return TRUE;
}

BOOL AgcmUIGuild::CBCreateConfirmOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(strlen(pThis->m_szInputGuildName) == 0)
		return FALSE;
	if(strlen(pThis->m_szInputPassword) == 0)
		return FALSE;

	pThis->m_pcsAgcmGuild->SendCreateGuild(pThis->m_szInputGuildName, pThis->m_szInputPassword);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateConfirmUI);

	return TRUE;
}