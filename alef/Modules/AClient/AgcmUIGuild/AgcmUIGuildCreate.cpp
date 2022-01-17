// AgcmUIGuildCreate.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 08.

#include "AgcmUIGuild.h"

BOOL AgcmUIGuild::CBCreateToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl = (AgcdUIControl*)pData1;

	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit*)pcsEditControl->m_pcsBase)->SetText(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_CREATE_TEXT));

	return TRUE;
}

BOOL AgcmUIGuild::CBCreateOkClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1 || !pData2 || !pData3)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgcdUIControl* pcsEditControl1 = (AgcdUIControl*)pData1;
	AgcdUIControl* pcsEditControl2 = (AgcdUIControl*)pData2;
	AgcdUIControl* pcsEditControl3 = (AgcdUIControl*)pData3;

	const char* szGuildName = ((AcUIEdit*)pcsEditControl1->m_pcsBase)->GetText();
	const char* szPassword1 = ((AcUIEdit*)pcsEditControl2->m_pcsBase)->GetText();
	const char* szPassword2 = ((AcUIEdit*)pcsEditControl3->m_pcsBase)->GetText();

	// 유효성 체크
	if(strlen(szGuildName) <= 0 || strlen(szGuildName) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;
	if(strlen(szPassword1) <= 0 || strlen(szPassword1) > AGPMGUILD_MAX_PASSWORD_LENGTH)
		return FALSE;
	if(strlen(szPassword2) <= 0 || strlen(szPassword2) > AGPMGUILD_MAX_PASSWORD_LENGTH)
		return FALSE;

	// 비밀번호가 같은지 체크
	if(strcmp(szPassword1, szPassword2) != 0)
	{
		// 왠지 비밀번호 다시 쓰라고 띄어줘야 할 것 같다.
		SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CHECK_PASSWORD));
		return FALSE;
	}

	// 멤버 변수에 저장을 하고
	strncpy(pThis->m_szInputGuildName, szGuildName, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	strncpy(pThis->m_szInputPassword, szPassword1, AGPMGUILD_MAX_PASSWORD_LENGTH);
	
	// UI 에 써진 것은 지운다.
	((AcUIEdit*)pcsEditControl1->m_pcsBase)->SetText("");
	((AcUIEdit*)pcsEditControl2->m_pcsBase)->SetText("");
	((AcUIEdit*)pcsEditControl3->m_pcsBase)->SetText("");

	// UI 설정
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildCreateUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildCreateConfirmUI);

	return TRUE;
}
