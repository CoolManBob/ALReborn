#include "AgcmUIGuild.h"

/*
	2005.07.15 by SungHoon
	길드전신청 인터페이스가 오픈가능하면 UI데이터를 초기화하고 오픈한다.
*/
BOOL AgcmUIGuild::CBGuildBattleTimeToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ) return FALSE;

	//AgcmUIGuild		*pThis				= (AgcmUIGuild *)	pClass;

	//AgcdUIControl	*pcsUIInvitedPlayerControl				= (AgcdUIControl *)	pData1;
	//if (pcsUIInvitedPlayerControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;

	//AcUIEdit *	pcsUIGuildBattleTimeEdit = (AcUIEdit *) pcsUIInvitedPlayerControl->m_pcsBase;
	//
	//pcsUIGuildBattleTimeEdit->ClearText();

	///*pThis->m_pstGuildBattleTimeCombo->m_lSelectedIndex = -1;*/

	//AgcdGuildList *pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
	//if (!pcsAgcdGuildList) return FALSE;

	//strncpy(pThis->m_szBattleEnemyGuildID, pcsAgcdGuildList->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	return TRUE;
}



