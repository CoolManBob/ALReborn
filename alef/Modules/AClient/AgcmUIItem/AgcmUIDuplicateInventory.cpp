#include "AgcmUIItem.h"

BOOL AgcmUIItem::CBOpenDuplicateInven(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	BOOL			bResult;
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	bResult = FALSE;

/*	if( pThis )
	{
		//현재는 옥션 구입창을 여는데 사용되는 UI임.
		bResult = pThis->m_pcsAgcmUIAuction->OpenUI();
	}*/

	return bResult;
}
