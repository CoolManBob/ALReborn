#include "AgcmItem.h"
#include "AgcCharacterUtil.h"
#include "RtPITexD.h"

/******************************************************************************
* Purpose :
*
* 100202. Bob Jung
******************************************************************************/
BOOL AgcmItem::ItemInventoryCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmItem *pThis						  = (AgcmItem *)(pClass);
	AgpdItem *pcsAgpdItem				  = (AgpdItem *)(pData);
	AgcdItem *pstAgcdItem				  = pThis->GetItemData(pcsAgpdItem);

	pThis->ReleaseItemData(pcsAgpdItem);

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);

	return pThis->SetInventoryItem(pcsAgpdItem);
}

BOOL AgcmItem::SetInventoryItem(AgpdItem *pcsAgpdItem)
{
	PROFILE("AgcmItem::SetItemInventory");

	TRACE("AgcmItem::SetInventoryItem() Item : %s\n", ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName);

	AgcdItem *pstAgcdItem				  = GetItemData(pcsAgpdItem);

	return TRUE;
}
