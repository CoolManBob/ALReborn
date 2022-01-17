#include "AgcmUIItem.h"

BOOL AgcmUIItem::CBNPCTradeUIOpen(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		//거래를 하기위해서 NPC창과 인벤창을 연다.
		if( pThis->m_bUIOpenNPCTrade == FALSE )
		{
			AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if (pcsSelfCharacter)
			{
				pThis->m_stNPCTradeOpenPos.x		= pcsSelfCharacter->m_stPos.x;
				pThis->m_stNPCTradeOpenPos.z		= pcsSelfCharacter->m_stPos.z;
			}

			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeOpenUI);
			pThis->m_bUIOpenNPCTrade = TRUE;
		}

		pThis->OpenInventory();

		pThis->m_lOpenNPCTradeLayer	= 0;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
	}

	return bResult;
}

BOOL AgcmUIItem::CBNPCTradeMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;
	AgpdGridSelectInfo *	pstGridSelectInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;

	BOOL					bResult;

	bResult = FALSE;

	//원래 위치에서 이동된 Layer, Row, Column로 아이템을 옮긴다는 패킷을 보낸다.
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstGridSelectInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo)
		return bResult;

	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);

	if (pstGrid)
	{
		INT32			lItemID;
		AgpdItem		*pcsAgpdItem;

		lItemID = pstGridSelectInfo->pGridItem->m_lItemID;

		pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( lItemID );

		if( pcsAgpdItem != NULL )
		{
			//인벤에서 NPCTrade창으로 옮겨진 경우
			if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY || 
				pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY )
			{
				pThis->m_lNPCTradeSellItemID = lItemID;

				AgcdUIHotkeyType	eHotKeyType	= pThis->m_pcsAgcmUIManager2->GetKeyStatus();

				BOOL	bProcess	= FALSE;

				if (eHotKeyType == AGCDUI_HOTKEY_TYPE_SHIFT)
				{
				}

				if (!bProcess)
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmSell);
				}
			}
		}
	}

	return bResult;

}

BOOL AgcmUIItem::CBNPCTradeUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL			bResult;

	pThis->m_pcsAgpmCharacter->StopCharacter(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), NULL);

	bResult = FALSE;

	if( pThis )
	{
		if( pThis->m_bUIOpenNPCTrade == FALSE )
		{
			AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if (pcsSelfCharacter)
			{
				pThis->m_stNPCTradeOpenPos.x		= pcsSelfCharacter->m_stPos.x;
				pThis->m_stNPCTradeOpenPos.z		= pcsSelfCharacter->m_stPos.z;
			}

			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeOpenUI );
			pThis->m_bUIOpenNPCTrade = TRUE;
		}

		pThis->OpenInventory();

		pThis->m_lOpenNPCTradeLayer	= 0;

		//pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
		pThis->m_pcsAgcmUIManager2->RefreshUserData( pThis->m_pcsNPCTrade );
	}

	return bResult;
}

BOOL AgcmUIItem::CBNPCTradeUIClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( pThis->m_bUIOpenNPCTrade )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeCloseUI );
			pThis->m_bUIOpenNPCTrade = FALSE;
		}

		if( pThis->m_bUIOpenInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryUIClose);
			pThis->m_bUIOpenInventory = FALSE;
		}

		pThis->m_lOpenNPCTradeLayer	= 0;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
	}

	return bResult;
}

BOOL	AgcmUIItem::CBNPCTradeOpenNextLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	if (pThis->m_lOpenNPCTradeLayer >= AGPMITEM_NPCTRADEBOX_LAYER - 1)
		return TRUE;

	AgcdUIControl	*pcsControl		= (AgcdUIControl *)		pData1;

	if( (pcsControl->m_lType != AcUIBase::TYPE_GRID) || !pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid		*pcsUIGrid		= (AcUIGrid *)			pcsControl->m_pcsBase;

	AgpdGrid		*pcsGrid		= pcsUIGrid->GetAgpdGrid();
	if (!pcsGrid)
		return FALSE;

	if (!pThis->m_pcsAgpmGrid)
		return FALSE;

	// 비어 있는 그리드면 넘어가지 않는다.
	if (pThis->m_pcsAgpmGrid->IsEmptyGrid(pcsGrid, pThis->m_lOpenNPCTradeLayer + 1, 0, 0, 1, 1))
		return TRUE;

	pThis->m_lOpenNPCTradeLayer++;

	pThis->m_pcsNPCTrade->m_lSelectedIndex = pThis->m_lOpenNPCTradeLayer;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsNPCTrade );
//	pcsUIGrid->SetNowLayer(++pThis->m_lOpenNPCTradeLayer);

	return TRUE;
}

BOOL	AgcmUIItem::CBNPCTradeOpenPrevLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	if (pThis->m_lOpenNPCTradeLayer <= 0)
		return TRUE;

	AgcdUIControl	*pcsControl		= (AgcdUIControl *)		pData1;

	if( (pcsControl->m_lType != AcUIBase::TYPE_GRID) || !pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid		*pcsUIGrid		= (AcUIGrid *)			pcsControl->m_pcsBase;

	pThis->m_lOpenNPCTradeLayer--;

	pThis->m_pcsNPCTrade->m_lSelectedIndex = pThis->m_lOpenNPCTradeLayer;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsNPCTrade );

//	pcsUIGrid->SetNowLayer(--pThis->m_lOpenNPCTradeLayer);

	return TRUE;
}

BOOL	AgcmUIItem::CBNPCTradeInitLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
	
	AgcdUIControl	*pcsControl		= (AgcdUIControl *)		pData1;

	if( (pcsControl->m_lType != AcUIBase::TYPE_GRID) || !pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid		*pcsUIGrid		= (AcUIGrid *)			pcsControl->m_pcsBase;

//	pcsUIGrid->SetNowLayer(pThis->m_lOpenNPCTradeLayer);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsNPCTrade );

	return TRUE;
}

//NPC Trade MSG----------------------------------------------------------------------------------------------------------
BOOL AgcmUIItem::CBUINPCTradeMSGFullInven( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGInventoryFull);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmBuy( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		/*
		AgpdItem	*pcsAgpdItem	= pThis->m_pcsAgpmItem->GetItem( pThis->m_lNPCTradeBuyItemID );
		if (pcsAgpdItem &&
			pcsAgpdItem->m_pcsItemTemplate &&
			((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_bStackable)
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmBuyStackCount);
		else
		*/
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmBuy);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmBuy( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( lTrueCancel )
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( pThis->m_lNPCTradeBuyItemID );

			if( pcsAgpdItem != NULL )
			{
				bResult = pThis->m_pcsAgcmEventNPCTrade->SendBuy( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
																pcsAgpdItem->m_lID,
																pcsAgpdItem->m_nCount,
																pThis->m_nNPCTradeBuyStatus,
																(INT8) pThis->m_lNPCTradeBuyLayer,
																(INT8) pThis->m_lNPCTradeBuyRow,
																(INT8) pThis->m_lNPCTradeBuyColumn);

				pThis->m_nNPCTradeBuyStatus = (-1);
				pThis->m_lNPCTradeBuyLayer	= (-1);
				pThis->m_lNPCTradeBuyRow	= (-1);
				pThis->m_lNPCTradeBuyColumn	= (-1);
			}
		}

	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmBuyStackCountOK( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIItem		*pThis				= (AgcmUIItem *)		pClass;

	AgcdUIControl	*pcsCountControl	= (AgcdUIControl *)		pData1;
	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;

	const CHAR			*szStackCount		= ((AcUIEdit *) (pcsCountControl->m_pcsBase))->GetText();
	if (!szStackCount ||
		!szStackCount[0])
		return FALSE;

	INT32			lStackCount		= atoi(szStackCount);
	if (lStackCount <= 0)
		return FALSE;

	AgpdItem		*pcsAgpdItem	= pThis->m_pcsAgpmItem->GetItem( pThis->m_lNPCTradeBuyItemID );
	if (pcsAgpdItem && pcsAgpdItem->m_pcsItemTemplate)
	{
		if (!((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_bStackable ||
			((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount < 1)
			return FALSE;

		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount < lStackCount)
			lStackCount = ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount;

		pcsAgpdItem->m_nCount	= lStackCount;

		pThis->m_pcsAgcmEventNPCTrade->SendBuy( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
												pcsAgpdItem->m_lID,
												lStackCount,
												pThis->m_nNPCTradeBuyStatus);

		pThis->m_lNPCTradeBuyItemID = 0;
		pThis->m_nNPCTradeBuyStatus = -1;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmBuyStackCountCancel( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	return TRUE;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmSell( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmSell);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmSell( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( lTrueCancel )
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( pThis->m_lNPCTradeSellItemID );

			if( pcsAgpdItem != NULL )
			{
				AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
				if( ppmItem )
				{
					// 이 아이템이 NPC에게 판매불가능한 아이템이라면 판매할수 없다는 메세지를 출력하고 종료
					// 1. 퀘스트아이템의 경우
					if( ppmItem->IsQuestItem( pcsAgpdItem ) )
					{
						AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
						if( pcmUIManager )
						{
							char* pMsg = pcmUIManager->GetUIMessage( "CannotNPCTradeQuestItem" );
							if( pMsg && strlen( pMsg ) > 0 )
							{
								pcmUIManager->ActionMessageOKDialog( pMsg );
							}
						}

						return TRUE;
					}
				}

				bResult = pThis->m_pcsAgcmEventNPCTrade->SendSell( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
																pcsAgpdItem->m_lID,
																pcsAgpdItem->m_nCount );
			}
		}

	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGNotEnoughGhelld( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGNotEnoughGhelld);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGNotEnoughSpaceToGetGhelld( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGNotEnoughSpaceToGetGhelld);
	}

	return bResult;
}

BOOL AgcmUIItem::SelfUpdatePositionCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmUIItem::SelfUpdatePositionCB");

	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;

	if (pThis->m_bUIOpenNPCTrade)
	{
		FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stNPCTradeOpenPos);

		if ((INT32) fDistance < AGCMUIITEM_CLOSE_UI_DISTANCE)
			return TRUE;

		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeCloseUI );
		pThis->m_bUIOpenNPCTrade = FALSE;

		if( pThis->m_bUIOpenInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryUIClose);
			pThis->m_bUIOpenInventory = FALSE;
		}

		pThis->m_lOpenNPCTradeLayer	= 0;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBBuyItemDirect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	AcUIGrid*	pGrid = (AcUIGrid*)( pcsSourceControl->m_pcsBase );

	AgpdGridSelectInfo	*pcsSelectInfo	= pGrid->GetGridItemClickInfo();
	if (!pcsSelectInfo || !pcsSelectInfo->pGridItem || pcsSelectInfo->pGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	AgpdItem			*pcsItem		= pThis->m_pcsAgpmItem->GetItem(pcsSelectInfo->pGridItem->m_lItemID);
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (!pThis->m_bUIOpenNPCTrade)
		return FALSE;

	pThis->m_lNPCTradeBuyItemID = pcsItem->m_lID;

	pThis->m_lNPCTradeBuyLayer	= (-1);
	pThis->m_lNPCTradeBuyRow	= (-1);
	pThis->m_lNPCTradeBuyColumn	= (-1);

	return pThis->CBUINPCTradeMSGConfirmBuy((PVOID) NULL, (PVOID) pThis, (PVOID) NULL);

//	return pThis->m_pcsAgcmEventNPCTrade->SendBuy( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
//													pcsItem->m_lID,
//													pcsItem->m_nCount );

//	pThis->m_lNPCTradeSellItemID = pcsItem->m_lID;
//
//	if (!pThis->m_pcsAgpmGrid->GetItemByTemplate(pThis->m_pcsNPCTradeGrid, AGPDGRID_ITEM_TYPE_ITEM, ((ApBase *) pcsItem->m_pcsItemTemplate)->m_lID))
//		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmSell);
//	else
//		return pThis->m_pcsAgcmEventNPCTrade->SendBuy( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
//														pcsItem->m_lID,
//														pcsItem->m_nCount );

	return TRUE;
}

BOOL AgcmUIItem::CBNPCTradeSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeSuccess);

	return TRUE;
}

BOOL AgcmUIItem::CBSplitItemNPCTrade(PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)			pClass;
	INT32				lStackCount			= *(INT32 *)					pData;

	if (!pThis->m_pcsAgpdSplitItem)
		return FALSE;

	return pThis->m_pcsAgcmEventNPCTrade->SendSell( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
													pThis->m_pcsAgpdSplitItem->m_lID,
													pThis->m_pcsAgpdSplitItem->m_nCount );
}

BOOL AgcmUIItem::CBNPCTradeDeleteGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *	pThis	= (AgcmUIItem *)	pClass;
	AgpdGrid *		pcsGrid	= (AgpdGrid *)		pData;

	if (!pcsGrid)
		return TRUE;

	if (pThis->m_pcsNPCTradeGrid == pcsGrid)
	{
		//pThis->m_pcsAgcmEventNPCTrade	=	NULL;
		pThis->m_pcsNPCTradeGrid		=	NULL;
		pThis->m_pcsNPCTrade->m_stUserData.m_pvData	=	NULL;
		pThis->m_lOpenNPCTradeLayer		=	0;

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsNPCTrade);
	}

	return TRUE;
}