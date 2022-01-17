/*==========================================================================

	AgcmUISalesBox.cpp
	
==========================================================================*/


#include "AgcmUIItem.h"
#include "AuStrTable.h"


//	Module Callback
//===========================================
//
BOOL AgcmUIItem::CBSalesBox2Add(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;
	AgpdAuctionSales	*pAgpdAuctionSales = (AgpdAuctionSales *) pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;

	AgpdAuctionCAD *pAgpdAuctionCAD = pThis->m_pcsAgpmAuction->GetCAD(pAgpdCharacter);
	if (NULL == pAgpdAuctionCAD)
		return FALSE;

	INT32 lIndex = pAgpdAuctionCAD->GetIndex(pAgpdAuctionSales->m_lID);

	if (0 > lIndex || lIndex >= AGPMAUCTION_MAX_REGISTER)
		return FALSE;
	
	AgpdGridItem *pAgpdGridItem = pThis->m_pcsAgpmGrid->CreateGridItem();// m_pcsSalesBox2GridItem[lIndex];
	if (NULL == pAgpdGridItem)
		return FALSE;

	AgpdItemTemplate *pAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pAgpdAuctionSales->m_lItemTID);
	if (NULL == pAgpdItemTemplate)
		return FALSE;

	pAgpdGridItem->m_lItemID = pAgpdAuctionSales->m_lID;
	pAgpdGridItem->m_lItemTID = pAgpdAuctionSales->m_lItemTID;
	
	// set texture
	pThis->m_pcsAgcmItem->SetGridItemTemplateAttachedTexture(pAgpdItemTemplate, pAgpdItemTemplate->m_pcsGridItem);
	RwTexture **ppRwTexture = (RwTexture **)(pThis->m_pcsAgpmGrid->GetAttachedModuleData( 
							pThis->m_pcsAgcmUIControl->m_lItemGridTextureADDataIndex, pAgpdGridItem));
	AgcdItemTemplate *pAgcdItemTemplate = pThis->m_pcsAgcmItem->GetTemplateData(pAgpdItemTemplate);
	if (pAgcdItemTemplate && pAgcdItemTemplate->m_pTexture)
	{
		RwTextureAddRef(pAgcdItemTemplate->m_pTexture);	
		(RwTexture *) *ppRwTexture = pAgcdItemTemplate->m_pTexture;
	}

	CHAR szQuantity[21];
	_stprintf(szQuantity, _T("%d"), pAgpdAuctionSales->m_nQuantity);
	pAgpdGridItem->SetRightBottomString(szQuantity);
	
	pThis->m_pcsAgpmGrid->Add(&pThis->m_csSalesBox2Grid[lIndex], 0, 0, 0, pAgpdGridItem, 1, 1);

	//등록 판매갯수 업데이트
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesOn);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesComplete);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2ButtonEnable);

	return TRUE;
}


BOOL AgcmUIItem::CBSalesBox2Remove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;
		
	AgcmUIItem		*pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = (AgpdAuctionSales *) pData;
	
	AgpdGridItem *pAgpdGridItem = NULL;
	for (INT32 lIndex=0; lIndex<AGPMAUCTION_MAX_REGISTER; lIndex++)
		{
		pAgpdGridItem = pThis->m_pcsAgpmGrid->GetItem(&pThis->m_csSalesBox2Grid[lIndex], 0, 0, 0);
		if (NULL != pAgpdGridItem && pAgpdGridItem->m_lItemID == pAgpdAuctionSales->m_lID)
			{
			// remove from grid
			pThis->m_pcsAgpmGrid->RemoveItem(&pThis->m_csSalesBox2Grid[lIndex], pAgpdGridItem);
			pThis->m_pcsAgpmGrid->DeleteGridItem(pAgpdGridItem);

			// shift all next grid items
			for (INT32 lEmptySlot = lIndex+1; lEmptySlot<AGPMAUCTION_MAX_REGISTER; lEmptySlot++)
			{
				pAgpdGridItem = pThis->m_pcsAgpmGrid->GetItem(&pThis->m_csSalesBox2Grid[lEmptySlot], 0, 0, 0 );
				if (pAgpdGridItem)
				{
					pThis->m_pcsAgpmGrid->RemoveItem(&pThis->m_csSalesBox2Grid[lEmptySlot], pAgpdGridItem);
					pThis->m_pcsAgpmGrid->Add(&pThis->m_csSalesBox2Grid[lEmptySlot-1], 0, 0, 0, pAgpdGridItem, 1, 1);
				}
			}

			break;
			}
		}

	// refresh
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesOn);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesComplete);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2ButtonEnable);

	return TRUE;
}


BOOL AgcmUIItem::CBSalesBox2Update(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	// refresh
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesOn);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesComplete);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2ButtonEnable);

	return TRUE;
}


BOOL AgcmUIItem::CBSetFocusOnBoard(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem	*pThis = (AgcmUIItem *) pClass;
	AgpdItem	*pAgpdItem = (AgpdItem *) pData;

	pThis->OpenToolTip(pAgpdItem, (INT32) pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.x, (INT32)pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.y, FALSE, FALSE);

	return TRUE;
}


BOOL AgcmUIItem::CBKillFocusOnBoard(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem	*pThis = (AgcmUIItem *) pClass;

	pThis->CloseToolTip();

	return TRUE;
}




//	UI Function Callback
//==================================================
//
BOOL AgcmUIItem::CBSalesBox2Open(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesOn);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2SalesComplete);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2ButtonEnable);

	// ##### 나눠팔기가 안된다는 메시지박스
	pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( ClientStr().GetStr(STI_NO_SPLIT) );

	return TRUE;
}


BOOL AgcmUIItem::CBSalesBoxMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem		*pThis = (AgcmUIItem *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;
	INT32			lItemID = 0;

	// 현재 개조중이라면 암짓도 하면 안된다.
	if (pThis->m_bIsProcessConvert)
		return TRUE;

	AcUIDragInfo *pcsAcUIDragInfo = pcsSourceControl->m_pcsBase->GetDragDropMessage();
	if (pcsAcUIDragInfo && pcsAcUIDragInfo->pSourceWindow)
	{
		AcUIGridItem *pcsUIGridItem = (AcUIGridItem *)pcsAcUIDragInfo->pSourceWindow;

		if (!pcsUIGridItem || !pcsUIGridItem->m_ppdGridItem)
			return FALSE;

		lItemID = pcsUIGridItem->m_ppdGridItem->m_lItemID;
	}
	else
	{
		if (pcsSourceControl->m_lType != AcUIBase::TYPE_GRID)
			return FALSE;
			
		AcUIGrid *pcsUIGrid = (AcUIGrid *) pcsSourceControl->m_pcsBase;
		AgpdGridSelectInfo *pstSelectInfo = pcsUIGrid->GetDragDropMessageInfo();

		if (!pstSelectInfo)
			return FALSE;

		lItemID = pstSelectInfo->pGridItem->m_lItemID;
	}

	if (0 == lItemID)
		return FALSE;
	
	AgpdItem *pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem(lItemID);
	if (NULL == pcsAgpdItem)
		return FALSE;

	// it must be in inventory
	if (AGPDITEM_STATUS_INVENTORY != pcsAgpdItem->m_eStatus)
		return FALSE;

	// sellable item
	if (0 == pcsAgpdItem->m_pcsItemTemplate->m_lFirstCategory
		|| 0 == pcsAgpdItem->m_pcsItemTemplate->m_lSecondCategory)
	{
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( ClientStr().GetStr(STI_WRONG_ITEM) );
		return FALSE;
	}

	CHAR *psz = ClientStr().GetStr(STI_INPUT_SELL_PRICE);

	pThis->m_pcsAgpdItem4Sale = pcsAgpdItem;
	if (pcsAgpdItem->m_pcsItemTemplate->m_bStackable)
		pThis->m_lSalesItemQuantity = pcsAgpdItem->m_nCount;
	else
		pThis->m_lSalesItemQuantity = 1;

	pThis->m_pcsAgcmUISplitItem->SetCondition(SplitItem_SalesTrue, CBSetPrice4SalesBox, pThis, 0, 499999999, 0 );
	pThis->m_pcsAgcmUISplitItem->OpenSplitItemUI();
	
	return TRUE;
}


BOOL AgcmUIItem::CBSalesBox2ClickCancelCompleteButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass || NULL == pcsSourceControl)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	AgpdCharacter *pSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdAuctionCAD *pAgpdAuctionCAD = pThis->m_pcsAgpmAuction->GetCAD(pSelfCharacter);
	if (NULL == pAgpdAuctionCAD)
		return FALSE;

	INT32 lSalesID = pAgpdAuctionCAD->m_Sales[pcsSourceControl->m_lUserDataIndex];
	AgpdAuctionSales *pAgpdAuctionSales = pThis->m_pcsAgpmAuction->GetSales(lSalesID);
	if (pAgpdAuctionSales)
	{
		if (AGPMAUCTION_SALES_STATUS_ONSALE == pAgpdAuctionSales->m_nStatus)
		{
			pThis->m_pcsAgcmAuction->SendCancel(pSelfCharacter->m_lID, lSalesID);
		}
		else if(AGPMAUCTION_SALES_STATUS_COMPLETE == pAgpdAuctionSales->m_nStatus)
		{
			pThis->m_pcsAgcmAuction->SendConfirm(pSelfCharacter->m_lID, lSalesID);
		}
		
		return TRUE;
	}
    
	return FALSE;
}


BOOL AgcmUIItem::CBSalesBox2Add(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsSalesBox2ButtonEnable);

	return TRUE;
}


BOOL AgcmUIItem::CBSalesBox2Remove(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsSalesBox2ButtonEnable);

	return TRUE;
}


BOOL AgcmUIItem::CBSalesBox2Update(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	return TRUE;
}


BOOL AgcmUIItem::CBUpdateSalesBoxGrid(PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox2Grid);
	
	return TRUE;
}

BOOL AgcmUIItem::CBSetQuantity4SalesBox(PVOID pData, PVOID pClass, PVOID pCustData )
{
	// 나눠팔기가 없어지므로 수량 다이얼로그는 띄우지 않는다.
	return FALSE;
}


BOOL AgcmUIItem::CBSetPrice4SalesBox(PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (NULL == pClass || NULL == pData)
		return FALSE;
	
	AgcmUIItem	*pThis = (AgcmUIItem *) pClass;
	INT32		lPrice = *(INT32 *)	pData;

	if (lPrice <= 0 || lPrice > 499999999)
	{
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( ClientStr().GetStr(STI_SELL_PRICE_RANGE) );
		return TRUE;
	}

	INT32 lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

	// if stackable
	if (pThis->m_pcsAgpdItem4Sale->m_pcsItemTemplate->m_bStackable)
	{
		// quantity exceeded
		if (pThis->m_lSalesItemQuantity > pThis->m_pcsAgpdItem4Sale->m_nCount)
			pThis->m_lSalesItemQuantity = pThis->m_pcsAgpdItem4Sale->m_nCount;
	}

	pThis->m_pcsAgcmAuction->SendSell(lCID, 
									pThis->m_pcsAgpdItem4Sale->m_lID, 
									pThis->m_lSalesItemQuantity, 
									lPrice 
									);

	return TRUE;
}


AgpdAuctionSales* AgcmUIItem::GetSalesOfIndex(INT32 lIndex)
{
	AgpdCharacter *pSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (NULL == pSelfCharacter)
		return NULL;
	
	AgpdAuctionCAD *pAgpdAuctionCAD = m_pcsAgpmAuction->GetCAD(pSelfCharacter);
	if (NULL == pAgpdAuctionCAD)
		return NULL;
	
	INT32 lSalesID = pAgpdAuctionCAD->m_Sales[lIndex];
	AgpdAuctionSales *pAgpdAuctionSales = m_pcsAgpmAuction->GetSales(lSalesID);
	
	return pAgpdAuctionSales;
}




//	Display Callbacks
//==========================================================
//
BOOL AgcmUIItem::CBDisplaySalesBox2ItemName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if (NULL == pClass || AGCDUI_USERDATA_TYPE_GRID != eType || NULL == pcsSourceControl)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = pThis->GetSalesOfIndex(pcsSourceControl->m_lUserDataIndex);
	if (pAgpdAuctionSales)
	{
		AgpdItemTemplate *pAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pAgpdAuctionSales->m_lItemTID);
		if (pAgpdItemTemplate)
			_stprintf(szDisplay, _T("%s"), pAgpdItemTemplate->m_szName);
	}
	
	return TRUE;
}


BOOL AgcmUIItem::CBDisplaySalesBox2ItemPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass || AGCDUI_USERDATA_TYPE_GRID != eType || NULL == pcsSourceControl)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = pThis->GetSalesOfIndex(pcsSourceControl->m_lUserDataIndex);
	if (pAgpdAuctionSales)
		_stprintf(szDisplay, _T("%dG"), pAgpdAuctionSales->m_lPrice);
	
	return TRUE;
}
				

BOOL AgcmUIItem::CBDisplaySalesBox2ItemTime(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if (NULL == pClass || AGCDUI_USERDATA_TYPE_GRID != eType || NULL == pcsSourceControl)
		return FALSE;
	
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = pThis->GetSalesOfIndex(pcsSourceControl->m_lUserDataIndex);
	if (pAgpdAuctionSales)
	{
		char szDate[20] = {0, };
		_snprintf(szDate, 19, "%s", pAgpdAuctionSales->m_szDate);
		szDate[19] = 0;

		sprintf(szDisplay, "%s %s", szDate, ClientStr().GetStr(STI_SALES_OUT_OF_DATE));

		if (0 == strcmp(&pAgpdAuctionSales->m_szDate[19], "/0"))
			pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = 0xFFFFFFFF;
		else
			pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = 0xFFCC0000;
	}
	
	return TRUE;
}


BOOL AgcmUIItem::CBDisplaySalesBox2OkCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if (NULL == pClass || NULL == pcsSourceControl)
		return FALSE;
		
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = pThis->GetSalesOfIndex(pcsSourceControl->m_lUserDataIndex);
	if (pAgpdAuctionSales)
	{
		if (AGPMAUCTION_SALES_STATUS_ONSALE == pAgpdAuctionSales->m_nStatus)
			_stprintf(szDisplay, ClientStr().GetStr(STI_CANCEL) );
		else if (AGPMAUCTION_SALES_STATUS_COMPLETE == pAgpdAuctionSales->m_nStatus)
			_stprintf(szDisplay, ClientStr().GetStr(STI_OK) );
	}

	return TRUE;
}


BOOL AgcmUIItem::CBDisplaySalesBox2SalesOn(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = NULL;
	INT32 lOnSale = 0;
		
	for (INT32 i=0; i<AGPMAUCTION_MAX_REGISTER; i++)
	{
		pAgpdAuctionSales = pThis->GetSalesOfIndex(i);
		if (NULL == pAgpdAuctionSales)
			continue;

		if (AGPMAUCTION_SALES_STATUS_ONSALE == pAgpdAuctionSales->m_nStatus)
			lOnSale++;
	}

	_stprintf(szDisplay, "%s: %d", ClientStr().GetStr(STI_SELLING), lOnSale);

	return TRUE;
}


BOOL AgcmUIItem::CBDisplaySalesBox2SalesComplete(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdAuctionSales *pAgpdAuctionSales = NULL;
	INT32 lComplete = 0;
		
	for (INT32 i=0; i<AGPMAUCTION_MAX_REGISTER; i++)
	{
		pAgpdAuctionSales = pThis->GetSalesOfIndex(i);
		if (NULL == pAgpdAuctionSales)
			continue;
					
		if (AGPMAUCTION_SALES_STATUS_COMPLETE == pAgpdAuctionSales->m_nStatus)
			lComplete++;
	}

	_stprintf(szDisplay, "%s: %d", ClientStr().GetStr(STI_SELL_COMPLETE), lComplete);

	return TRUE;
}




//	Boolean Callback
//===============================================
//
BOOL AgcmUIItem::CBIsEnableSalesBox2Button(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass || NULL == pcsSourceControl)
		return FALSE;

	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	if (pThis->m_pcsAgpmGrid->GetItem(&pThis->m_csSalesBox2Grid[pcsSourceControl->m_lUserDataIndex], 0, 0, 0))
		return TRUE;
	else
		return FALSE;
}

