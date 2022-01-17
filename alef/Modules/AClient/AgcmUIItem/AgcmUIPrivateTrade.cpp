#include "AgcmUIItem.h"
#include "AuStrTable.h"
#include "AgpdSystemMessage.h"

BOOL AgcmUIItem::CBClosePrivateTradeUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;
	INT32			lCID;

	bResult = FALSE;

	if( pThis != NULL )
	{
		//만약 PrivateTrade창이 열려있었다면 Cancel패킷을 날리고 UI를 닫는다. 원츄~
		if( pThis->m_pcsAgcmCharacter )
		{
			lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

			if( lCID != 0 )
			{
				//거래할 대상이있는가?
				if( pThis->m_pcsPrivateTradeSetClientNameChar )
				{
					pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( lCID );
				}

				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBDisplayPrivateTraderName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_PRIVATETRADERNAME || !szDisplay)
		return FALSE;

	pThis->m_pcsPrivateTradeSetClientNameChar = pcsAgpdCharacter;

	//sprintf(szDisplay, "%s님과 거래하시겠습니까?", pcsAgpdCharacter->m_szID );
	sprintf(szDisplay, ClientStr().GetStr(STI_WANT_TRADE), pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayRequestPrivateTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_REQUEST_PRIVATETRADE || !szDisplay)
		return FALSE;

	pThis->m_pcsPrivateTradeSetClientNameChar = pcsAgpdCharacter;

	//sprintf(szDisplay, "%s님의 응답을 기다리는 중입니다.", pcsAgpdCharacter->m_szID );
	sprintf(szDisplay, ClientStr().GetStr(STI_WAIT_TRADE), pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeSetName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_SET_NAME || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "[%s]", pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeSetClientName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_SET_CLIENTNAME || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "[%s]", pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeGhelld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	INT32			lGhelld = *((INT32 *)pData);

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 || lID != AGCMUI_ITEM_DISPLAY_ID_PT_GHELLD || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "%d", lGhelld );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeClientGhelld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	INT32			lGhelld = *((INT32 *)pData);

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 || lID != AGCMUI_ITEM_DISPLAY_ID_PT_CLIENTGHELLD || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "%d", lGhelld );

	return TRUE;
}

BOOL AgcmUIItem::CBUIOpenPrivateTradeYesNo( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgpdCharacter	*pcsAgpdCharacter;
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	INT32			lRequesterCID = *((INT32 *)pData);

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		pThis->m_lPrivateTradeRequesterCID = lRequesterCID;

		pcsAgpdCharacter = pThis->m_pcsAgpmCharacter->GetCharacter( lRequesterCID );

		if( pcsAgpdCharacter != NULL )
		{
			pThis->m_pcsPrivateTraderName->m_stUserData.m_pvData = pcsAgpdCharacter;

			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeOpenUIYesNo);

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTraderName);

		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIClosePrivateTradeYesNo( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeCloseUIYesNo);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIOpenPrivateTrade( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;
	INT32			lCTargetID;

	bResult = FALSE;

	if( (pThis != NULL) && (pData != NULL) )
	{
		lCTargetID = *((INT32 *)pData);

		//PrivateTrade창이 뜨면 요청창은 무조건 닫힌다.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIClose);

		//인벤토리의 돈버리기 버튼을 비활성화시킨다.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryDisableDropGhelld );

		if( pThis->m_bUIOpenPrivateTrade == FALSE )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeOpenUI);
			pThis->m_bUIOpenPrivateTrade = TRUE;
		}

		pThis->OpenInventory();

		// 거래창 Grid 초기화
		pThis->m_pcsAgpmGrid->Reset( &pThis->m_PrivateTradeGridEx );
		pThis->m_pcsAgpmGrid->Reset( &pThis->m_PrivateTradeClientGridEx );

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsPrivateTradeEx	);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsPrivateTradeClientEx );

		AgpdCharacter		*pcsAgpdTargetChar;

		pcsAgpdTargetChar = pThis->m_pcsAgpmCharacter->GetCharacter( lCTargetID );

		if( pcsAgpdTargetChar != NULL )
		{
			AgpdItemADChar		*pcsAgpdItemADTargetChar;

			//나의 이름, 거래자의 이름을 보여주자~ 냠냠~
			pThis->m_pcsPrivateTradeSetName->m_stUserData.m_pvData = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			pThis->m_pcsPrivateTradeSetClientName->m_stUserData.m_pvData = pcsAgpdTargetChar;

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeSetName);
			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeSetClientName);
			
			pcsAgpdItemADTargetChar = pThis->m_pcsAgpmItem->GetADCharacter(pcsAgpdTargetChar);

			if( pcsAgpdItemADTargetChar != NULL )
			{
				pThis->m_pcsPrivateTradeClientGrid = &pcsAgpdItemADTargetChar->m_csTradeGrid;
				pThis->m_pcsPrivateTradeClient->m_stUserData.m_pvData	= pThis->m_pcsPrivateTradeClientGrid;
				pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClient);

				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIClosePrivateTrade( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
//		if( pThis->m_bUIOpenPrivateTrade )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lPrivateTradeCloseUI );
			pThis->m_bUIOpenPrivateTrade = FALSE;
		}

		//인벤토리의 돈버리기 버튼을 활성화시킨다.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryEnableDropGhelld );

		if( pThis->m_bUIOpenInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryUIClose);
			pThis->m_bUIOpenInventory = FALSE;
		}

		if( pThis->m_bUIOpenSubInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lSubInventoryUIClose);
			pThis->m_bUIOpenSubInventory = FALSE;
		}

		// 거래 확인 다이얼로그가 떠있으면 닫는다.
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lRequestPrivateTradeUIClose );

		pThis->m_pcsPrivateTradeSetNameChar = NULL;
		pThis->m_pcsPrivateTradeSetClientNameChar = NULL;

		//돈을 0으로 바꾼다.
		pThis->m_lPrivateTradeClientGhelld = 0;
		pThis->m_lPrivateTradeGhelld = 0;

		pThis->m_pcsPrivateTradeGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeGhelld);

		pThis->m_pcsPrivateTradeClientGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeClientGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClientGhelld);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBRequestCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	if( pThis != NULL )
	{
		if( pThis->m_pcsAgcmPrivateTrade != NULL && pThis->m_pcsAgcmCharacter )
		{
			if( pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter )
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_lID );

				//UI창을 닫자.
				if( bResult )
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIClose);
				}
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeAccept(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	AgcdUI*		pUI	=	pThis->m_pcsAgcmUIManager2->GetUI( "UI_Auction" );

	if( pThis != NULL )
	{
		if( pThis->m_pcsAgcmPrivateTrade != NULL && pThis->m_pcsAgcmCharacter != NULL)
		{
			AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if (pThis->m_pcsAgpmCharacter->HasPenalty(pcsSelfCharacter, AGPMCHAR_PENALTY_PRVTRADE)
				|| pThis->m_pcsAgpmCharacter->HasPenalty(pcsSelfCharacter, AGPMCHAR_PENALTY_AUCTION)
				|| pUI->m_pcsUIWindow->m_bOpened	)
			{

				if( pUI->m_pcsUIWindow->m_bOpened )
				{
					char* pMsg = pThis->m_pcsAgcmUIManager2->GetUIMessage( "CannotAuctionInPrivateTrade" );
					if( pMsg && strlen( pMsg ) > 0 )
					{
						pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pMsg );
					}

					return FALSE;
				}

				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( pcsSelfCharacter->m_lID );
			}
			else
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeRequestConfirm( pThis->m_lPrivateTradeRequesterCID );
			}

			//UI창을 닫자.
			if( bResult )
			{
				bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeCloseUIYesNo);
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeReject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;
	INT32			lCID;

	if( pThis != NULL )
	{
		if( pThis->m_pcsAgcmCharacter )
		{
			lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

			if( lCID != 0 )
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( lCID );

				//UI창을 닫자.
				if( bResult )
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeCloseUIYesNo);
				}
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem					*pThis				=	static_cast< AgcmUIItem		* >(pClass);
	AgcdUIControl				*pcsControl			=	static_cast< AgcdUIControl	* >(pData1);
	AgpdGridSelectInfo			*pstGridSelectInfo	=	NULL;
	AgpdGrid					*pstGrid			=	NULL;
	AcUIGrid					*pcsUIGrid			=	NULL;
	AgpdItem					*pcsAgpdItem		=	NULL;
	eAGPM_PRIVATE_TRADE_ITEM	eType				=	AGPM_PRIVATE_TRADE_ITEM_NORMAL;

	INT32						nItemID				=	0;

	//원래 위치에서 이동된 Layer, Row, Column로 아이템을 옮긴다는 패킷을 보낸다.
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return FALSE;

	pcsUIGrid	=	static_cast< AcUIGrid* >(pcsControl->m_pcsBase);

	pstGridSelectInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo)
		return FALSE;

	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if ( !pstGrid )
		return FALSE;

	nItemID = pstGridSelectInfo->pGridItem->m_lItemID;

	pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( nItemID );
	if( pcsAgpdItem == NULL )
		return FALSE;
	
	// 인벤에서 옮겨지는 아이템이 아니면 실패
	if( pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY &&
		pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY)
	{
		return FALSE;
	}

	
	if( strcmp( pcsControl->m_szName , UI_TRADE_SUPPORT_GRID_NAME) == 0 )
	{
		// 보조 그리드에 보조아이템을 올리지 않았다면 return FALSE
		if( pThis->m_pcsAgpmItem->GetUsableType( pcsAgpdItem ) != AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION )
			return FALSE;

		else
		{
			// 보조형 아이템은 한가지씩 올릴수 있다
			if( !pThis->_CheckSupportItemDuplicate( pcsAgpdItem ) )
				return FALSE;

			eType		=	AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER_ENABLE_TRADE;
		}
	}

	// 귀속 아이템은 마블스크롤이 올라왔을경우 한개만 거래 d가능하다
	else if ( pThis->m_pcsAgpmItem->IsBoundOnOwner(pcsAgpdItem) || pThis->m_pcsAgpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND )
	{
		eType		=	AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER;

		// 마블스크롤이 올라와서 거래가 가능한지 확인( 귀속 아이템이 이미 올라와 있는지도 체크 )
		if( !pThis->_CheckBoundItemReleaseTradeCheck( pcsAgpdItem ) )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTradeFailForBoundItem);
			return FALSE;
		}

	}

	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType) &&
		pcsAgpdItem->m_lCashItemUseCount > 0 &&
		pThis->m_pcsAgpmItem->IsUsingStamina(pcsAgpdItem->m_pcsItemTemplate) == FALSE)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTradeFailForUsingCashItem);
		return FALSE;
	}

	// 사용중인 아이템은 거래 할 수 없다
	if( pThis->m_pcsAgpmItem->IsUsingItem( pcsAgpdItem ) )
	{
		SystemMessage.ProcessSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_USE_ITEM_TRANDE_FAILED) , 0 , AGPMSYSTEMMESSAGE_PACKET_MODAL_MESSAGE );
		return FALSE;
	}

	// Trade Option 그리드에 올라가는 아이템이 아니라면 이벤트 아이템인지 
	// 검사한다( 이벤트 아이템은 거래 불가 )
	if( strcmp( pcsControl->m_szName , UI_TRADE_MAIN_GRID_NAME) == 0 )
	{

		// 귀속이면서 이벤트 아이템은 거래 할 수 없다 !!!
		if ( pThis->m_pcsAgpmItem->IsBoundOnOwner(pcsAgpdItem) || pThis->m_pcsAgpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND )
		{
			if( pThis->m_pcsAgpmItem->IsEventItem( pcsAgpdItem ) )
				return FALSE;
		}
	}

	return  pThis->m_pcsAgcmPrivateTrade->SendTradeAddItemToTradeGrid(	eType , pcsAgpdItem->m_ulCID, nItemID, 
																		pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
																		pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
																		pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
																		pcsUIGrid->m_lNowLayer, 
																		pstGridSelectInfo->lGridRow, 
																		pstGridSelectInfo->lGridColumn );
	
	
}

BOOL AgcmUIItem::CBPrivateTradeUpdateMoney(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem		*pThis				= (AgcmUIItem *)		pClass;

	AgcdUIControl	*pcsCountControl	= (AgcdUIControl *)		pData1;
	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;

	const CHAR			*szSendGhelldCount	= ((AcUIEdit *) (pcsCountControl->m_pcsBase))->GetText();
	if (!szSendGhelldCount ||
		!szSendGhelldCount[0])
		return FALSE;

	INT32			lGhelldCount		= atoi(szSendGhelldCount);
	if (lGhelldCount <= 0)
		return FALSE;

	((AcUIEdit *) (pcsCountControl->m_pcsBase))->SetText( "" );

	return pThis->m_pcsAgcmPrivateTrade->SendTradeUpdateMoney( pThis->m_pcsAgcmCharacter->GetSelfCID(), lGhelldCount );
}

BOOL AgcmUIItem::CBPrivateTradeLock(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeLock();

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUnlock(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeUnlock();

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;
	INT32			lCID;

	if( pThis->m_pcsAgcmCharacter )
	{
		lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

		if( lCID != 0 )
		{
			bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( lCID );
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateClientLock( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	bool			bPushDown = *((bool *)pData);

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		if( bPushDown )
		{
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeClientLock);
		}
		else
		{
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeClientUnlock);
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeActiveReadyToExchange( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeActiveReadyToExchange);
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeReadyToExchange(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem		*pThis = (AgcmUIItem *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;
	AcUIButton		*pcsUIButton;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis && pcsControl )
	{
		pcsUIButton = (AcUIButton *)pcsControl->m_pcsBase;

		if( pcsUIButton )
		{
			bResult = pThis->m_pcsAgcmPrivateTrade->SendReadyToExchange();

			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lRequestPrivateTradeUIOpen );
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeClientReadyToExchange( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeClientReadyToExchange);
	}

	return bResult;
}

/*
	2005.06.02. By SungHoon
	거래 요청한 사용자가 거래 거부 중일 경우
*/
BOOL AgcmUIItem::CBPrivateTradeRequestRefuseUser( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData) return FALSE;
	AgcmUIItem *pThis	= (AgcmUIItem *) pClass;
	INT32 lRequestCID	= *(INT32 *)	   pData;
	INT32 lTargetCID	= *(INT32 *)	   pCustData;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIClose);
	pThis->m_pcsPrivateTradeSetClientNameChar = NULL;

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(lTargetCID);
	if (pcsCharacter == NULL) return FALSE;

	DWORD dwColor = 0xFF0000;
	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRADE_NEGATIVE_TEXT_COLOR);
	if (szColor) dwColor = atol(szColor);

	CHAR szMessage[ 512 ] = { 0 };
	wsprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRADE_REQUEST_FAILED_REFUSE_USER), pcsCharacter->m_szID);

	SystemMessage.ProcessSystemMessage(szMessage, dwColor);

	return TRUE;
}

//Private Trade MSG------------------------------------------------------------------------------------------------------
BOOL AgcmUIItem::CBPrivateTradeMSGComplete(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		pThis->_InitPrivateOptionItem();
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeMSGTradeComplete);
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeMSGCancel(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		pThis->_InitPrivateOptionItem();
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeMSGTradeCancel);
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateGhelld(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem			*pThis;
	BOOL				bResult;

	pThis = (AgcmUIItem *) pClass;
	bResult = FALSE;

	if ( pThis && pData )
	{
		pThis->m_lPrivateTradeGhelld = *((INT32 *)pData);

		//Data Setting.
		pThis->m_pcsPrivateTradeGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeGhelld);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateClientGhelld(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem			*pThis;
	BOOL				bResult;

	pThis = (AgcmUIItem *) pClass;
	bResult = FALSE;

	if ( pThis && pData )
	{
		pThis->m_lPrivateTradeClientGhelld = *((INT32 *)pData);

		//Data Setting.
		pThis->m_pcsPrivateTradeClientGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeClientGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClientGhelld);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::_CheckBoundItemReleaseTradeCheck( AgpdItem* pItem )
{
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );
	INT32					nCount			=	m_pcsAgpmGrid->GetItemCount( m_pcsPrivateTradeGrid );
	AgpdGridItem*			pcsGridItem		=	NULL;
	AgpdItem*				pcsItem			=	NULL;

	// 마블 스크롤이 올라 갔다면 귀속템은 하나만 올릴수 있다
	// 거래창에 귀속템이 올린적이 있는지 체크한다
	for( INT32 i = 0 ; i < nCount ; ++i )
	{
		pcsGridItem	=	m_pcsAgpmGrid->GetItem( m_pcsPrivateTradeGrid , i );
		pcsItem		=	m_pcsAgpmItem->GetItem( pcsGridItem );

		if( m_pcsAgpmItem->IsBoundOnOwner( pcsItem ) || m_pcsAgpmItem->GetBoundType(pcsItem) != E_AGPMITEM_NOT_BOUND )
			return FALSE;
	}

	// Private Trade Option 아이템이 올라 왔는지 확인 한다
	for( INT32 i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		if( !pADChar->m_stTradeOptionItem[i].m_lItemIID )
			break;

		pcsItem		=	m_pcsAgpmItem->GetItem( pADChar->m_stTradeOptionItem[i].m_lItemIID );

		if( m_pcsAgpmItem->IsMarvelScroll( pcsItem ) )
			return TRUE;
	}

	// 없으면 귀속템 못 올린다
	return FALSE;
}

BOOL AgcmUIItem::_CheckSupportItemDuplicate( AgpdItem* pItem )
{
	if( !pItem	)
		return FALSE;

	AgpdItem*				pcsItem			=	NULL;
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );

	// Private Trade Option 아이템은 종복되서 올라갈 수 없다
	for( INT32 i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		if( !pADChar->m_stTradeOptionItem[i].m_lItemIID )
			break;

		pcsItem		=	m_pcsAgpmItem->GetItem( pADChar->m_stTradeOptionItem[i].m_lItemIID );

		if( pcsItem->m_lTID	== pItem->m_lTID )
			return FALSE;
	}

	return TRUE;
}

BOOL	AgcmUIItem::_AddSupportItem( AgpdItem* pItem , BOOL bSelfCharacter )
{
	
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );

	INT32					nCount			=	_SupportItemCount( bSelfCharacter );
		
	// 본인
	if( bSelfCharacter )
	{
		pADChar->m_stTradeOptionItem[ nCount ].m_lItemIID	=	pItem->m_lID;
		pADChar->m_stTradeOptionItem[ nCount ].m_lItemTID	=	pItem->m_lTID;
	}

	// 상대방
	else
	{
		pADChar->m_stClientTradeOptionItem[ nCount ].m_lItemIID	=	pItem->m_lID;
		pADChar->m_stClientTradeOptionItem[ nCount ].m_lItemTID	=	pItem->m_lTID;
	}

	return TRUE;
}

INT32	AgcmUIItem::_SupportItemCount( BOOL bSelfCharacter )
{
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );


	for( INT i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		if( bSelfCharacter )
		{
			if( !pADChar->m_stTradeOptionItem[i].m_lItemIID )
				return i;
		}
		else
		{
			if( !pADChar->m_stClientTradeOptionItem[i].m_lItemIID )
				return i;
		}

	}

	return 0;
}

VOID	AgcmUIItem::_InitPrivateOptionItem( VOID )
{
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );

	// 거래창 Grid 초기화
	m_pcsAgpmGrid->Reset( &m_PrivateTradeGridEx );
	m_pcsAgpmGrid->Reset( &m_PrivateTradeClientGridEx );
	
	for( INT i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		ZeroMemory( &pADChar->m_stTradeOptionItem[i] , sizeof(AgpdPrivateOptionItem) );
		ZeroMemory( &pADChar->m_stClientTradeOptionItem[i] , sizeof(AgpdPrivateOptionItem) );
	}

}

BOOL AgcmUIItem::OnSendPrivateTradeCancel( void )
{
	AgcmPrivateTrade* pcmPrivateTrade = ( AgcmPrivateTrade* )g_pEngine->GetModule( "AgcmPrivateTrade" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmPrivateTrade || !pcmCharacter ) return FALSE;

	int nMyCID = pcmCharacter->GetSelfCID();
	return pcmPrivateTrade->SendTradeCancel( nMyCID );
}
