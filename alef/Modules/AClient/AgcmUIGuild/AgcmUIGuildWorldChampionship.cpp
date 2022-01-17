#include "AgcmUIGuild.h"
#include "AuStrTable.h"


BOOL AgcmUIGuild::CBDisplayDialogWorldChampionship(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if( !pClass || !pcsSourceControl )
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	TCHAR* pszText = ClientStr().GetStr( STI_EVENT_WORLD_CHAMPIONSHIP_NOTICE );
	( ( AcUIEdit* )pcsSourceControl->m_pcsBase )->SetText( pszText );

	return TRUE;
}

BOOL AgcmUIGuild::CBOnWorldChampionshipRequestBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	pThis->m_pcsAgcmGuild->SendWorldChampionshipRequest();

	return TRUE;
}

BOOL AgcmUIGuild::CBOnWorldChampionshipEnterBtnClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);

	pThis->m_pcsAgcmGuild->SendWorldChampionshipEnter();

	return TRUE;
}