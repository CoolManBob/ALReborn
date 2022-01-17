// AgcmUIGuildCreateComplete.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 12.

#include "AgcmUIGuild.h"



BOOL AgcmUIGuild::CBCreateCompleteToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	CHAR szBuffer[255];
	memset(szBuffer, 0, sizeof(CHAR)*255);

	if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
		sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_CREATE_COMPLETE_TEXT),
			pThis->m_szInputGuildName, pcsCharacter->m_szID, pThis->m_szInputGuildName);
	else
		sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_CREATE_COMPLETE_TEXT),
			pThis->m_szInputGuildName, pThis->m_szInputGuildName, pcsCharacter->m_szID);

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
}

